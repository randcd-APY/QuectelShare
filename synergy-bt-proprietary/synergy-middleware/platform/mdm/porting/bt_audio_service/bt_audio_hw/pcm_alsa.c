/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include <stdio.h>
#include <sys/time.h>
#include <alsa/asoundlib.h>

#include "connx_log.h"

#include "bt_audio_hw.h"
#include "pcm_alsa.h"

static int inhibit_err;
static long recv_out_minVol, recv_out_maxVol;
static snd_mixer_elem_t *playback_elem;
static snd_mixer_t *recv_out_mixer;

struct pcm *pcm_open_dev(const char *device, unsigned int type, unsigned int sample_rate, unsigned int latency)
{
    int err;
    snd_pcm_stream_t stream;
    snd_pcm_t *handle;
    struct pcm *pcm;
    
    snd_pcm_sw_params_t *swparams;

    snd_pcm_sw_params_alloca(&swparams);

    if (type == PCM_PLAYBACK)
        stream = SND_PCM_STREAM_PLAYBACK;
    else if (type == PCM_CAPTURE)
        stream = SND_PCM_STREAM_CAPTURE;
    /* [QTI] Fix KW issue#267655. */
    else
        return NULL;

    pcm = calloc(1, sizeof(struct pcm));

    if (!pcm)
        return NULL;

    if (stream == SND_PCM_STREAM_PLAYBACK)
        err = snd_pcm_open(&handle, device, stream, SND_PCM_NONBLOCK);
    else
        err = snd_pcm_open(&handle, device, stream, 0);

    if (err < 0)
    {
        IFLOG(DebugOut(DEBUG_ERROR, TEXT("<%s> Open native playback device failed for %s: %s"), __FUNCTION__, device, snd_strerror(err)));
        return NULL;
    }

    if (stream == SND_PCM_STREAM_PLAYBACK)
    {
        err = snd_pcm_nonblock(handle, 1);

        if (err < 0)
        {
            IFLOG(DebugOut(DEBUG_ERROR, TEXT("<%s> nonblock setting error for %s: %s"), __FUNCTION__, device, snd_strerror(err)));
            goto err;
        }
    }

    pcm->stream = stream;
    pcm->sample_rate = sample_rate;
    err = snd_pcm_set_params(handle, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED,
                             1, sample_rate, 1, latency);

    if (err < 0)
    {
        IFLOG(DebugOut(DEBUG_ERROR, TEXT("<%s> Set parameters failed for %s: %s."), __FUNCTION__, device, snd_strerror(err)));
        goto err;
    }

    err = snd_pcm_sw_params_current(handle, swparams);

    if (err < 0)
    {
        IFLOG(DebugOut(DEBUG_ERROR, TEXT("<%s> Unable to determine current swparams for %s: %s"), __FUNCTION__, device, snd_strerror(err)));
        goto err;
    }

    err = snd_pcm_sw_params_set_tstamp_mode(handle, swparams, SND_PCM_TSTAMP_ENABLE);

    if (err < 0)
    {
        IFLOG(DebugOut(DEBUG_ERROR, TEXT("<%s> Unable to set tstamp(enable) mode for %s: %s"), __FUNCTION__, device, snd_strerror(err)));
        goto err;
    }

    if (stream == SND_PCM_STREAM_CAPTURE)
    {
        err = snd_pcm_sw_params_set_start_threshold(handle, swparams, 1);

        if (err < 0)
        {
            IFLOG(DebugOut(DEBUG_ERROR, TEXT("<%s> Unable to set start threshold for %s: %s"), __FUNCTION__, device, snd_strerror(err)));
            goto err;
        }
    }

    err = snd_pcm_sw_params(handle, swparams);

    if (err < 0)
    {
        IFLOG(DebugOut(DEBUG_ERROR, TEXT("<%s> Unable to set sw params for %s: %s"), __FUNCTION__, device, snd_strerror(err)));
        goto err;
    }

    snd_pcm_get_params(handle, &pcm->buffer_size, &pcm->chunk_size);

    pcm->handle = handle;

    inhibit_err = 0;

    return pcm;

err:
    snd_pcm_close(handle);
    free(pcm);
    return NULL;
}

void set_recv_out_volume(int percent)
{
    int volume;

    if (!recv_out_mixer)
    {
        IFLOG(DebugOut(DEBUG_WARN, TEXT("<%s> Cannot set volume, because no mixer available."), __FUNCTION__));
        return;
    }

    if (!playback_elem)
    {
        IFLOG(DebugOut(DEBUG_WARN, TEXT("<%s> Cannot set volume, because no \"Playback\" element available."), __FUNCTION__));
        return;
    }

    volume = (recv_out_maxVol - recv_out_minVol) * percent / 100;

    snd_mixer_selem_set_playback_volume_all(playback_elem, volume);
}

void pcm_close_dev(struct pcm *pcm)
{
    if (recv_out_mixer)
    {
        snd_mixer_close(recv_out_mixer);
        recv_out_mixer = NULL;
        playback_elem = NULL;
    }

    if (!pcm)
        return;

    if (pcm->handle)
        snd_pcm_close(pcm->handle);

    free(pcm);
}

void pcm_start(struct pcm *pcm)
{
    if (!pcm->running)
    {
        snd_pcm_prepare(pcm->handle);
        snd_pcm_start(pcm->handle);
        pcm->running = 1;
    }
}

ssize_t pcm_read_data(struct pcm *pcm, void *buffer, int count)
{
    ssize_t r;
    u_char *data = buffer;
    int len = count;

    if (!pcm->running)
    {
        snd_pcm_prepare(pcm->handle);
        snd_pcm_start(pcm->handle);
        pcm->running = 1;
    }

    while (count > 0)
    {
        r = snd_pcm_readi(pcm->handle, data, count);

        if (r == -EAGAIN)
            continue;
        else if (r == -EPIPE || r == -ESTRPIPE)
            snd_pcm_recover(pcm->handle, r, inhibit_err);
        else if (r < 0)
        {
            if (!inhibit_err)
                IFLOG(DebugOut(DEBUG_ERROR, TEXT("<%s> Read error: %s, %d"), __FUNCTION__, snd_strerror(r), r));

            return 0;
        }

        if (r > 0)
        {
            count -= r;
            data += r * 2;
        }
    }

    return len;
}

void pcm_write_data(struct pcm *pcm, void *buffer, int count)
{
    ssize_t r;
    u_char *data = buffer;

    if (!pcm->running)
    {
        snd_pcm_prepare(pcm->handle);
        pcm->running = 1;
    }

    while (count > 0)
    {
        r = snd_pcm_writei(pcm->handle, data, count);

        if (r == -EAGAIN)
            continue;
        else if (r == -EPIPE || r == -ESTRPIPE)
            snd_pcm_recover(pcm->handle, r, inhibit_err);
        else if (r < 0)
        {
            if (!inhibit_err)
                IFLOG(DebugOut(DEBUG_ERROR, TEXT("<%s> write error: %s, %d"), __FUNCTION__, snd_strerror(r), r));

            return;
        }

        if (r > 0)
        {
            count -= r;
            data += r * 2;
        }
    }
}

void inhibit_alsa_err()
{
    inhibit_err = 1;
}

long pcm_get_delay(struct pcm *pcm,  snd_pcm_uframes_t frames, struct timespec *tstamp)
{
    snd_pcm_uframes_t alsaFr;
    snd_pcm_stream_t stream = pcm->stream;
    long delayNs = 0L;

    if (snd_pcm_htimestamp(pcm->handle, &alsaFr, tstamp) < 0)
    {
        tstamp->tv_sec = 0;
        tstamp->tv_nsec = 0;
        IFLOG(DebugOut(DEBUG_ERROR, TEXT("<%s> pcm_get_delay(): pcm_get_htimestamp error, setting playbackTimestamp to 0"), __FUNCTION__));
        return delayNs;
    }

    if (stream == SND_PCM_STREAM_PLAYBACK)
    {
        alsaFr = pcm->buffer_size - alsaFr;
        delayNs = (long)(((int64_t)(alsaFr + frames) * 1000000000) / pcm->sample_rate);
    }
    else if (stream == SND_PCM_STREAM_CAPTURE)
    {
        delayNs = (long)(((int64_t)alsaFr * 1000000000) / pcm->sample_rate);
    }

    return delayNs;
}

