
/*===============================================================================
** Copyright 2010, The Android Open-Source Project
** Copyright (c) 2011-2013, The Linux Foundation. All rights reserved.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
================================================================================*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <limits.h>
#include <stdint.h>

#include "mcm_constants.h"
#include "util_log.h"

typedef struct
{
  int (*init)();
  int (*enable_voice_stream)();
  int (*enable_audio_stream)();
  int (*disable_voice_stream)();
  int (*disable_audio_stream)();
  int (*mute_voice_stream)(int mute_type);
} mcm_audio_handler;

#define MCM_SRV_AUDIO_MAX_TRIES 5     // Number of attempts to open the sound device
#define MCM_SRV_AUDIO_OPEN_WAIT 2 // Number of seconds to sleep
                                  // before attempting to open sound device again

#ifndef MCM_AUDIO_SHARED_LIBRARY

#define MCM_AUDIO_SHARED_LIBRARY_NAME "libmcm_srv_audio.so.0"
#define MCM_AUDIO_GET_ENTRY_POINT "get_audio_handler"
#define MCM_AUDIO_RELEASE_ENTRY_POINT "release_audio_handler"
#include <dlfcn.h>
#include "mcm_srv_audio.h"
typedef mcm_audio_handler * (*get_audio_handler_ptr)(void);
typedef void (*release_audio_handler_ptr)(mcm_audio_handler *);

static mcm_audio_handler *audio_handler = NULL;
static get_audio_handler_ptr get_audio_handler = NULL;
static release_audio_handler_ptr release_audio_handler = NULL;

int mcm_srv_snd_init()
{
    void *library_handle;
    char *error;
    int unavailable = 0;
    int ret = 0;
    mcm_audio_handler *tmp_audio_handler = NULL;

    UTIL_LOG_MSG("Enter mcm_srv_snd_init, Audio shared Library: %s", MCM_AUDIO_SHARED_LIBRARY_NAME);

    dlerror();
    library_handle = dlopen(MCM_AUDIO_SHARED_LIBRARY_NAME, RTLD_NOW);
    UTIL_LOG_MSG("Got library handle: %d", library_handle);

    do
    {
        if (library_handle)
        {
            get_audio_handler = dlsym(library_handle, MCM_AUDIO_GET_ENTRY_POINT);
            error = dlerror();
            if (error)
            {
                UTIL_LOG_MSG("Unable to find %s in %s: %s",
                              MCM_AUDIO_GET_ENTRY_POINT,
                              MCM_AUDIO_SHARED_LIBRARY_NAME,
                              error);
                break;
            }
            else if (!get_audio_handler)
            {
                UTIL_LOG_MSG("%s defined as NULL in %s",
                              MCM_AUDIO_GET_ENTRY_POINT,
                              MCM_AUDIO_SHARED_LIBRARY_NAME);
                break;
            }

            release_audio_handler = dlsym(library_handle, MCM_AUDIO_RELEASE_ENTRY_POINT);
            error = dlerror();
            if (error)
            {
                UTIL_LOG_MSG("Unable to find %s in %s: %s",
                             MCM_AUDIO_RELEASE_ENTRY_POINT,
                             MCM_AUDIO_SHARED_LIBRARY_NAME,
                             error);
                /* Release is optional. Don't break here if not found */
            }

            UTIL_LOG_MSG("Getting Audio Handler");

            tmp_audio_handler = get_audio_handler();
            UTIL_LOG_MSG("Get audio handler: %d", tmp_audio_handler);

            if (tmp_audio_handler)
            {
                /* All the pointers in the audio handler are mandatory. Check that all are there */
                if (!tmp_audio_handler->init)
                {
                    UTIL_LOG_MSG("init not provided by handler");
                    unavailable ++;
                }
                if (!tmp_audio_handler->enable_voice_stream)
                {
                    UTIL_LOG_MSG("enable_voice_stream not provided by handler");
                    unavailable ++;
                }
                if (!tmp_audio_handler->enable_audio_stream)
                {
                    UTIL_LOG_MSG("enable_audio_stream not provided by handler");
                    unavailable++;
                }
                if (!tmp_audio_handler->disable_voice_stream)
                {
                    UTIL_LOG_MSG("disable_voice_stream not provided by handler");
                    unavailable++;
                }
                if (!tmp_audio_handler->mute_voice_stream)
                {
                    UTIL_LOG_MSG("mute_voice_stream not provided by handler");
                    unavailable++;
                }

                if (unavailable)
                {
                    UTIL_LOG_MSG("Releasing audio handler as not all operations supported");
                    if (release_audio_handler)
                    {
                        release_audio_handler(tmp_audio_handler);
                    }
                    release_audio_handler = NULL;
                    get_audio_handler = NULL;
                    dlclose(library_handle);
                    break;
                }
                UTIL_LOG_MSG("Audio handler available");
                audio_handler = tmp_audio_handler;

                UTIL_LOG_MSG("Calling audio handler init");
                ret = audio_handler->init();
            }
        }
        else
        {
            error = dlerror();
            UTIL_LOG_MSG("Unable to open audio handler %s: %s",
                         MCM_AUDIO_SHARED_LIBRARY_NAME,
                         error ? error : "<unknown error>");
        }
    } while(FALSE);
    UTIL_LOG_MSG("Exit mcm_srv_snd_init with ret: %d", ret);

    return ret;
}

int mcm_srv_enable_voice_stream()
{
    int ret = 0;
    UTIL_LOG_MSG("Enter mcm_srv_enable_voice_stream, audio_handler: %d", audio_handler);

    if (audio_handler)
    {
        ret = audio_handler->enable_voice_stream();
    }
    UTIL_LOG_MSG("Exit mcm_srv_enable_voice_stream with ret: %d", ret);

    return ret;
}

int mcm_srv_disable_voice_stream()
{
    int ret = 0;
    UTIL_LOG_MSG("Enter mcm_srv_disable_voice_stream, audio_handler: %d", audio_handler);

    if (audio_handler)
    {
        ret = audio_handler->disable_voice_stream();
    }
    UTIL_LOG_MSG("Exit mcm_srv_disable_voice_stream with ret: %d", ret);

    return ret;
}

int mcm_srv_mute_voice_stream(int mute_type)
{
    int ret = 0;
    UTIL_LOG_MSG("Enter mcm_srv_mute_voice_stream, audio_handler: %d", audio_handler);

    if (audio_handler)
    {
        ret = audio_handler->mute_voice_stream(mute_type);
    }
    UTIL_LOG_MSG("Exit mcm_srv_mute_voice_stream with ret: %d", ret);

    return ret;
}

int mcm_srv_enable_audio_stream()
{
    int ret = 0;
    UTIL_LOG_MSG("Enter mcm_srv_enable_audio_stream, audio_handler: %d", audio_handler);


    if (audio_handler)
    {
        ret = audio_handler->enable_audio_stream();
    }
    UTIL_LOG_MSG("Exit mcm_srv_enable_audio_stream with ret: %d", ret);

    return ret;
}

int mcm_srv_disable_audio_stream()
{
    int ret = 0;
    UTIL_LOG_MSG("Enter mcm_srv_disable_audio_stream, audio_handler: %d", audio_handler);

    if (audio_handler)
    {
        ret = audio_handler->disable_audio_stream();
    }
    UTIL_LOG_MSG("Exit mcm_srv_disable_audio_stream with ret: %d", ret);

    return ret;
}

#else

#include <sound/asound.h>

#include <alsa-intf/alsa_ucm.h>
#include <alsa-intf/alsa_audio.h>

/* Global data */
static snd_use_case_mgr_t *uc_mgr;

static struct mixer *mixer_hndl;
static struct mixer_ctl *ctl_hndl;

struct snd_card_mapping
{
    char kernel_card_name[50];
    char ucm_card_name[50];
};

static struct snd_card_mapping snd_card_mapping_list[] =
{
    {"mdm9615-tabla-snd-card", "snd_soc_msm_I2S"},
    {"mdm9615-tabla-snd-card-i2s", "snd_soc_msm_I2S"},
    {"mdm9625-taiko-i2s-snd-card", "snd_soc_msm_Taiko_I2S"},
    {"mdm9630-taiko-i2s-snd-card", "snd_soc_msm_Taiko_I2S"},
    {"mdm9640-tomtom-i2s-snd-card", "snd_soc_msm_9x40_Tomtom_I2S"},
    {"mdm9607-tomtom-i2s-snd-card", "snd_soc_msm_9x07_Tomtom_I2S"},
    {"mdm-tasha-i2s-snd-card", "snd_soc_msm_Tasha_I2S"},
    {"sdx-tavil-i2s-snd-card", "snd_soc_msm_Tavil_I2S"},
};

#define SOUND_CARD_BUFFER_SIZE 100

struct wav_header {
    uint32_t riff_id;
    uint32_t riff_sz;
    uint32_t riff_fmt;
    uint32_t fmt_id;
    uint32_t fmt_sz;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;       /* sample_rate * num_channels * bps / 8 */
    uint16_t block_align;     /* num_channels * bps / 8 */
    uint16_t bits_per_sample;
    uint32_t data_id;
    uint32_t data_sz;
};

static int pcm_flag = 1;
static uint32_t play_max_sz = 2147483648LL;
static int format = SNDRV_PCM_FORMAT_S16_LE;
static int period = 0;
static int set_channel_map = 0;
static int piped = 0;
static int outputMetadataLength = 0;
static int eosSet = 0;

#define COMPR_META_DATA_SIZE  64
static int duration = 0;
static struct pcm *pcm;
static struct pcm *audio_pcm;
static struct pcm *voice_pcm;

static struct wav_header hdr;
static char *data;

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164

#define FORMAT_PCM 1

static int rec_set_params(struct pcm *pcm)
{
    struct snd_pcm_hw_params *params;
    struct snd_pcm_sw_params *sparams;

    unsigned long periodSize, bufferSize, reqBuffSize;
    unsigned int periodTime, bufferTime;
    unsigned int requestedRate = pcm->rate;

    params = (struct snd_pcm_hw_params*) calloc(1, sizeof(struct snd_pcm_hw_params));
    if (!params)
    {
        UTIL_LOG_MSG("Arec:Failed to allocate ALSA hardware parameters!");
        return -ENOMEM;
    }

    param_init(params);

    param_set_mask(params, SNDRV_PCM_HW_PARAM_ACCESS,
                    (pcm->flags & PCM_MMAP)? SNDRV_PCM_ACCESS_MMAP_INTERLEAVED : SNDRV_PCM_ACCESS_RW_INTERLEAVED);
    param_set_mask(params, SNDRV_PCM_HW_PARAM_FORMAT, pcm->format);
    param_set_mask(params, SNDRV_PCM_HW_PARAM_SUBFORMAT,
                    SNDRV_PCM_SUBFORMAT_STD);
    if (period)
        param_set_min(params, SNDRV_PCM_HW_PARAM_PERIOD_BYTES, period);
    else
        param_set_min(params, SNDRV_PCM_HW_PARAM_PERIOD_TIME, 10);
    param_set_int(params, SNDRV_PCM_HW_PARAM_SAMPLE_BITS, 16);
    param_set_int(params, SNDRV_PCM_HW_PARAM_FRAME_BITS,
                    pcm->channels * 16);
    param_set_int(params, SNDRV_PCM_HW_PARAM_CHANNELS,
                    pcm->channels);
    param_set_int(params, SNDRV_PCM_HW_PARAM_RATE, pcm->rate);

    param_set_hw_refine(pcm, params);

    if (param_set_hw_params(pcm, params))
    {
        UTIL_LOG_MSG("Arec:cannot set hw params");
        return -errno;
    }

    pcm->buffer_size = pcm_buffer_size(params);
    pcm->period_size = pcm_period_size(params);
    pcm->period_cnt = pcm->buffer_size/pcm->period_size;

    UTIL_LOG_MSG("period_size (%d)", pcm->period_size);
    UTIL_LOG_MSG("buffer_size (%d)", pcm->buffer_size);
    UTIL_LOG_MSG("period_cnt  (%d)", pcm->period_cnt);
    
    sparams = (struct snd_pcm_sw_params*) calloc(1, sizeof(struct snd_pcm_sw_params));
    if (!sparams)
    {
        UTIL_LOG_MSG("Arec:Failed to allocate ALSA software parameters!");
        return -ENOMEM;
    }
    sparams->tstamp_mode = SNDRV_PCM_TSTAMP_NONE;
    sparams->period_step = 1;

    if (pcm->flags & PCM_MONO) {
        sparams->avail_min = pcm->period_size/2;
        sparams->xfer_align = pcm->period_size/2;
    } else if (pcm->flags & PCM_QUAD) {
        sparams->avail_min = pcm->period_size/8;
        sparams->xfer_align = pcm->period_size/8;
    } else if (pcm->flags & PCM_5POINT1) {
        sparams->avail_min = pcm->period_size/12;
        sparams->xfer_align = pcm->period_size/12;
    } else {
        sparams->avail_min = pcm->period_size/4;
        sparams->xfer_align = pcm->period_size/4;
    }

    sparams->start_threshold = 1;
    sparams->stop_threshold = INT_MAX;
    sparams->silence_size = 0;
    sparams->silence_threshold = 0;

    if (param_set_sw_params(pcm, sparams))
    {
        UTIL_LOG_MSG("Arec:cannot set sw params");
        return -errno;
    }
    UTIL_LOG_MSG("avail_min (%lu)", sparams->avail_min);
    UTIL_LOG_MSG("start_threshold (%lu)", sparams->start_threshold);
    UTIL_LOG_MSG("stop_threshold (%lu)", sparams->stop_threshold);
    UTIL_LOG_MSG("xfer_align (%lu)", sparams->xfer_align);
    
    return 0;
}

static int record_file(unsigned rate, unsigned channels, int fd, unsigned count,  unsigned flags, const char *device)
{
    unsigned xfer, bufsize, framesize;
    int r, avail;
    int nfds = 1;
    static int start = 0;
    struct snd_xferi x;
    long frames;
    unsigned offset = 0;
    int err;
    struct pollfd pfd[1];
    int rec_size = 0;
    framesize = 0;
    flags |= PCM_IN;

    if (channels == 1)
        flags |= PCM_MONO;
    else if (channels == 4)
        flags |= PCM_QUAD;
    else if (channels == 6)
        flags |= PCM_5POINT1;
    else
        flags |= PCM_STEREO;

    pcm = pcm_open(flags, device);
    if (!pcm_ready(pcm))
    {
        pcm_close(pcm);
        goto fail;
    }

    pcm->channels = channels;
    pcm->rate = rate;
    pcm->flags = flags;
    pcm->format = format;
    if (rec_set_params(pcm))
    {
        UTIL_LOG_MSG("Arec:params setting failed");
        pcm_close(pcm);
        return -EINVAL;
    }

    if (pcm_prepare(pcm))
    {
        UTIL_LOG_MSG("Arec:Failed in pcm_prepare");
        pcm_close(pcm);
        return -errno;
    }

    if (ioctl(pcm->fd, SNDRV_PCM_IOCTL_START))
    {
        UTIL_LOG_MSG("Arec: Hostless IOCTL_START Error no %d ", errno);
        pcm_close(pcm);
        return -errno;
    }
    while(1);

    bufsize = pcm->period_size;

    if (pcm_prepare(pcm))
    {
        UTIL_LOG_MSG("Arec:Failed in pcm_prepare");
        pcm_close(pcm);
        return -errno;
    }

    data = calloc(1, bufsize);
    if (!data)
    {
        UTIL_LOG_MSG("Arec:could not allocate %d bytes", bufsize);
        return -ENOMEM;
    }

    while (!pcm_read(pcm, data, bufsize))
    {
        if (write(fd, data, bufsize) != bufsize)
        {
            UTIL_LOG_MSG("Arec:could not write %d bytes", bufsize);
            break;
        }

        rec_size += bufsize;
        hdr.data_sz += bufsize;
        hdr.riff_sz = hdr.data_sz + 44 - 8;
        if (!piped)
        {
            lseek(fd, 0, SEEK_SET);
            write(fd, &hdr, sizeof(hdr));
            lseek(fd, 0, SEEK_END);
        }

        if (rec_size >= count)
            break;
    }
    UTIL_LOG_MSG("rec_size =%d count =%d", rec_size, count);
    close(fd);
    free(data);
    pcm_close(pcm);
    return hdr.data_sz;

fail:
    UTIL_LOG_MSG("Arec:pcm error: %s", pcm_error(pcm));
    return -errno;
}

static int play_set_params(struct pcm *pcm)
{
    struct snd_pcm_hw_params *params;
    struct snd_pcm_sw_params *sparams;

    unsigned long periodSize, bufferSize, reqBuffSize;
    unsigned int periodTime, bufferTime;
    unsigned int requestedRate = pcm->rate;
    int channels;
    if(pcm->flags & PCM_MONO)
        channels = 1;
    else if(pcm->flags & PCM_QUAD)
        channels = 4;
    else if(pcm->flags & PCM_5POINT1)
        channels = 6;
    /*else if(pcm->flags & PCM_7POINT1)
        channels = 8;*/
    else
        channels = 2;

    params = (struct snd_pcm_hw_params*) calloc(1, sizeof(struct snd_pcm_hw_params));
    if (!params) 
    {
        UTIL_LOG_MSG("Aplay:Failed to allocate ALSA hardware parameters!");
        return -ENOMEM;
    }

    param_init(params);

    param_set_mask(params, SNDRV_PCM_HW_PARAM_ACCESS,
                   (pcm->flags & PCM_MMAP)? SNDRV_PCM_ACCESS_MMAP_INTERLEAVED : SNDRV_PCM_ACCESS_RW_INTERLEAVED);
    param_set_mask(params, SNDRV_PCM_HW_PARAM_FORMAT, pcm->format);
    param_set_mask(params, SNDRV_PCM_HW_PARAM_SUBFORMAT,
                   SNDRV_PCM_SUBFORMAT_STD);
    if (period)
        param_set_min(params, SNDRV_PCM_HW_PARAM_PERIOD_BYTES, period);
    else
        param_set_min(params, SNDRV_PCM_HW_PARAM_PERIOD_TIME, 10);
    param_set_int(params, SNDRV_PCM_HW_PARAM_SAMPLE_BITS, 16);
    param_set_int(params, SNDRV_PCM_HW_PARAM_FRAME_BITS,
                   pcm->channels * 16);
    param_set_int(params, SNDRV_PCM_HW_PARAM_CHANNELS,
                   pcm->channels);
    param_set_int(params, SNDRV_PCM_HW_PARAM_RATE, pcm->rate);
    param_set_hw_refine(pcm, params);

    if (param_set_hw_params(pcm, params))
    {
        UTIL_LOG_MSG("Aplay:cannot set hw params");
        return -errno;
    }
    
    pcm->buffer_size = pcm_buffer_size(params);
    pcm->period_size = pcm_period_size(params);
    pcm->period_cnt = pcm->buffer_size/pcm->period_size;
    
    UTIL_LOG_MSG("period_cnt = %d", pcm->period_cnt);
    UTIL_LOG_MSG("period_size = %d", pcm->period_size);
    UTIL_LOG_MSG("buffer_size = %d", pcm->buffer_size);
    
    sparams = (struct snd_pcm_sw_params*) calloc(1, sizeof(struct snd_pcm_sw_params));
    if (!sparams)
    {
        UTIL_LOG_MSG("Aplay:Failed to allocate ALSA software parameters!");
        return -ENOMEM;
    }

    // Get the current software parameters
    sparams->tstamp_mode = SNDRV_PCM_TSTAMP_NONE;
    sparams->period_step = 1;

    sparams->avail_min = pcm->period_size/(channels * 2) ;
    sparams->start_threshold =  pcm->period_size/(channels * 2) ;
    sparams->stop_threshold =  pcm->buffer_size ;
    sparams->xfer_align =  pcm->period_size/(channels * 2) ; /* needed for old kernels */

    sparams->silence_size = 0;
    sparams->silence_threshold = 0;
    if (param_set_sw_params(pcm, sparams))
    {
        UTIL_LOG_MSG("Aplay:cannot set sw params");
        return -errno;
    }
    
    UTIL_LOG_MSG("sparams->avail_min= %lu", sparams->avail_min);
    UTIL_LOG_MSG("sparams->start_threshold= %lu", sparams->start_threshold);
    UTIL_LOG_MSG("sparams->stop_threshold= %lu", sparams->stop_threshold);
    UTIL_LOG_MSG("sparams->xfer_align= %lu", sparams->xfer_align);
    UTIL_LOG_MSG("sparams->boundary= %lu", sparams->boundary);
    
    return 0;
}

static int play_file(unsigned rate, unsigned channels, int fd,
              unsigned flags, const char *device, unsigned data_sz, struct pcm *pcm)
{
    //struct pcm *pcm;
    struct mixer *mixer;
    struct pcm_ctl *ctl = NULL;
    unsigned bufsize;
    char *data;
    long avail;
    long frames;
    int nfds = 1;
    struct snd_xferi x;
    unsigned offset = 0;
    int err;
    static int start = 0;
    struct pollfd pfd[1];
    int remainingData = 0;

    flags |= PCM_OUT;

    if (channels == 1)
        flags |= PCM_MONO;
    else if (channels == 4)
	flags |= PCM_QUAD;
    else if (channels == 6)
	flags |= PCM_5POINT1;
    /*else if (channels == 8)
	flags |= PCM_7POINT1;*/
    else
        flags |= PCM_STEREO;

    pcm = pcm_open(flags, device);
    if (pcm < 0)
    {
        UTIL_LOG_MSG("Aplay:failed to open pcm: %d", pcm);
        return pcm;
    }

    if (!pcm_ready(pcm))
    {
        UTIL_LOG_MSG("Aplay:pcm not ready");
        pcm_close(pcm);
        return -EBADFD;
    }
#if 0
    if (compressed) {
       struct snd_compr_caps compr_cap;
       struct snd_compr_params compr_params;
       if (ioctl(pcm->fd, SNDRV_COMPRESS_GET_CAPS, &compr_cap)) {
          fprintf(stderr, "Aplay: SNDRV_COMPRESS_GET_CAPS, failed Error no %d ", errno);
          pcm_close(pcm);
          return -errno;
       }
       if (!period)
           period = compr_cap.min_fragment_size;
           switch (get_compressed_format(compr_codec)) {
           case SND_AUDIOCODEC_MP3:
               compr_params.codec.id = SND_AUDIOCODEC_MP3;
               break;
           case SND_AUDIOCODEC_AC3_PASS_THROUGH:
               compr_params.codec.id = SND_AUDIOCODEC_AC3_PASS_THROUGH;
               printf("codec -d = %x", SND_AUDIOCODEC_AC3_PASS_THROUGH);
               break;
           case SND_AUDIOCODEC_AAC:
               compr_params.codec.id = SND_AUDIOCODEC_AAC;
               printf("codec -d = %x", SND_AUDIOCODEC_AAC);
               break;
           default:
               break;
           }
       if (ioctl(pcm->fd, SNDRV_COMPRESS_SET_PARAMS, &compr_params)) {
          fprintf(stderr, "Aplay: SNDRV_COMPRESS_SET_PARAMS,failed Error no %d ", errno);
          pcm_close(pcm);
          return -errno;
       }
       outputMetadataLength = sizeof(struct output_metadata_handle_t);
    } else if (channels > 2) {
        if(set_channel_map) {
            send_channel_map_driver(pcm);
        }
    }
#endif
    pcm->channels = channels;
    pcm->rate = rate;
    pcm->flags = flags;
    pcm->format = format;
    if (play_set_params(pcm))
    {
        UTIL_LOG_MSG("Aplay:params setting failed");;
        pcm_close(pcm);
        return -errno;
    }

    if (!pcm_flag)
    {
       if (pcm_prepare(pcm))
       {
           UTIL_LOG_MSG("Aplay:Failed in pcm_prepare");
           pcm_close(pcm);
           return -errno;
       }

       if (ioctl(pcm->fd, SNDRV_PCM_IOCTL_START))
       {
           UTIL_LOG_MSG("Aplay: Hostless IOCTL_START Error no %d", errno);
           pcm_close(pcm);
           return -errno;
       }
       while(1);
    }

    remainingData = data_sz;

    if (flags & PCM_MMAP)
    {
        u_int8_t *dst_addr = NULL;
        struct snd_pcm_sync_ptr *sync_ptr1 = pcm->sync_ptr;
        if (mmap_buffer(pcm))
        {
            UTIL_LOG_MSG("Aplay:params setting failed");
            pcm_close(pcm);
            return -errno;
        }
        if (pcm_prepare(pcm))
        {
            UTIL_LOG_MSG("Aplay:Failed in pcm_prepare");
            pcm_close(pcm);
            return -errno;
        }

        bufsize = pcm->period_size;

        UTIL_LOG_MSG("Aplay:bufsize = %d", bufsize);

        pfd[0].fd = pcm->timer_fd;
        pfd[0].events = POLLIN;

        frames = bufsize / (2*channels);
        for (;;)
        {
            if (!pcm->running)
            {
                if (pcm_prepare(pcm))
                {
                    UTIL_LOG_MSG("Aplay:Failed in pcm_prepare");
                    pcm_close(pcm);
                    return -errno;
                }
                pcm->running = 1;
                start = 0;
            }
            /* Sync the current Application pointer from the kernel */
            pcm->sync_ptr->flags = SNDRV_PCM_SYNC_PTR_APPL | SNDRV_PCM_SYNC_PTR_AVAIL_MIN;//SNDRV_PCM_SYNC_PTR_HWSYNC;
            err = sync_ptr(pcm);
            if (err == EPIPE)
            {
                UTIL_LOG_MSG("Aplay:Failed in sync_ptr");
                 /* we failed to make our window -- try to restart */
                pcm->underruns++;
                pcm->running = 0;
                continue;
            }
            /*
             * Check for the available buffer in driver. If available buffer is
             * less than avail_min we need to wait
             */
            avail = pcm_avail(pcm);
            if (avail < 0)
            {
                UTIL_LOG_MSG("Aplay:Failed in pcm_avail");
                pcm_close(pcm);
                return avail;
            }
            if (avail < pcm->sw_p->avail_min)
            {
                poll(pfd, nfds, TIMEOUT_INFINITE);
                continue;
            }
            /*
             * Now that we have buffer size greater than avail_min available to
             * to be written we need to calcutate the buffer offset where we can
             * start writting.
             */
            dst_addr = dst_address(pcm);

            UTIL_LOG_MSG("dst_addr = 0x%08x", dst_addr);
            UTIL_LOG_MSG("Aplay:avail = %d frames = %d",avail, frames);
            UTIL_LOG_MSG("Aplay:sync_ptr->s.status.hw_ptr %ld  pcm->buffer_size %d  sync_ptr->c.control.appl_ptr %ld",
                         pcm->sync_ptr->s.status.hw_ptr,
                         pcm->buffer_size,
                         pcm->sync_ptr->c.control.appl_ptr);

            /*
             * Read from the file to the destination buffer in kernel mmaped buffer
             * This reduces a extra copy of intermediate buffer.
             */
            memset(dst_addr, 0x0, bufsize);

            if (data_sz && !piped)
            {
                if (remainingData < bufsize)
                {
                    bufsize = remainingData;
                    frames = remainingData / (2*channels);
                }
            }
            UTIL_LOG_MSG("addr = %d, size = %d", (dst_addr + outputMetadataLength),(bufsize - outputMetadataLength));
            err = read(fd, (dst_addr + outputMetadataLength) , (bufsize - outputMetadataLength));

#if 0
            if(compressed) {
                 updateMetaData(err);
                 memcpy(dst_addr, &outputMetadataTunnel, outputMetadataLength);
            }
#endif
            UTIL_LOG_MSG("read %d bytes from file", err);
            if (err <= 0 )
            {
                UTIL_LOG_MSG("EOS set");
                eosSet = 1;
                break;
            }
            if (data_sz && !piped)
            {
                remainingData -= bufsize;
                if (remainingData <= 0)
                    break;
            }

            /*
             * Increment the application pointer with data written to kernel.
             * Update kernel with the new sync pointer.
             */
            pcm->sync_ptr->c.control.appl_ptr += frames;
            pcm->sync_ptr->flags = 0;

            err = sync_ptr(pcm);
            if (err == EPIPE)
            {
                UTIL_LOG_MSG("Aplay:Failed in sync_ptr 2");
                /* we failed to make our window -- try to restart */
                pcm->underruns++;
                pcm->running = 0;
                continue;
            }
            /*
             * If we have reached start threshold of buffer prefill,
             * its time to start the driver.
             */
            if(start)
                goto start_done;

            if (ioctl(pcm->fd, SNDRV_PCM_IOCTL_START))
            {
                err = -errno;
                if (errno == EPIPE)
                {
                    UTIL_LOG_MSG("Aplay:Failed in SNDRV_PCM_IOCTL_START");

                    /* we failed to make our window -- try to restart */
                    pcm->underruns++;
                    pcm->running = 0;
                    continue;
                }
                else
                {
                    UTIL_LOG_MSG("Aplay:Error no %d", errno);
                    pcm_close(pcm);
                    return -errno;
                }
            }
            else
                start = 1;

start_done:

            offset += frames;
        }//for

        while(1)
        {
            pcm->sync_ptr->flags = SNDRV_PCM_SYNC_PTR_APPL | SNDRV_PCM_SYNC_PTR_AVAIL_MIN;//SNDRV_PCM_SYNC_PTR_HWSYNC;
            sync_ptr(pcm);
            /*
             * Check for the available buffer in driver. If available buffer is
             * less than avail_min we need to wait
             */
            if (pcm->sync_ptr->s.status.hw_ptr >= pcm->sync_ptr->c.control.appl_ptr)
            {
                break;
            }
            else
                poll(pfd, nfds, TIMEOUT_INFINITE);
        }//while(1)
    }//if (flags & PCM_MMAP)
    else
    {
        if (pcm_prepare(pcm))
        {
            UTIL_LOG_MSG("Aplay:Failed in pcm_prepare");
            pcm_close(pcm);
            return -errno;
        }

        bufsize = pcm->period_size;

        data = calloc(1, bufsize);
        if (!data)
        {
            UTIL_LOG_MSG("Aplay:could not allocate %d bytes", bufsize);
            pcm_close(pcm);
            return -ENOMEM;
        }

        if (data_sz && !piped)
        {
            if (remainingData < bufsize)
                bufsize = remainingData;
        }

        while (read(fd, data, bufsize) > 0)
        {
            if (pcm_write(pcm, data, bufsize))
            {
                UTIL_LOG_MSG("Aplay: pcm_write failed");
                free(data);
                pcm_close(pcm);
                return -errno;
            }
            memset(data, 0, bufsize);

            if (data_sz && !piped) {
                remainingData -= bufsize;
                if (remainingData <= 0)
                    break;
                if (remainingData < bufsize)
                       bufsize = remainingData;
            }
        }
        free(data);
    }
    UTIL_LOG_MSG("Aplay: Done playing");
    pcm_close(pcm);
    return 0;
}

static int play_wav(const char *fg, int rate, int ch, const char *device, const char *fn, struct pcm *pcm)
{
    struct wav_header hdr;
    int fd;
    unsigned flag = 0;
    char *cformat = NULL;
    int err = -1;

    do
    {
        if (pcm_flag)
        {
            if (!fn)
            {
                UTIL_LOG_MSG("invalid input");
                break;
            }

            fd = open(fn, O_RDONLY);
            if (fd < 0)
            {
                UTIL_LOG_MSG("!!!!!!!Aplay:aplay: cannot open '%s'", fn);
                break;
            }

            if (read(fd, &hdr, sizeof(hdr)) != sizeof(hdr))
            {
                UTIL_LOG_MSG("Aplay:aplay: cannot read header");
                break;
            }

            if ((hdr.riff_id != ID_RIFF) ||
                (hdr.riff_fmt != ID_WAVE) ||
                (hdr.fmt_id != ID_FMT))
            {
                UTIL_LOG_MSG("Aplay:aplay: '%s' is not a riff/wave file", fn);
                break;
            }

            if ((hdr.audio_format != FORMAT_PCM) ||
                (hdr.fmt_sz != 16))
            {
                break;
            }
            if (hdr.bits_per_sample != 16)
            {
                break;
            }
        }
        else
        {
            fd = -EBADFD;
            hdr.sample_rate = rate;
            hdr.num_channels = ch;
            hdr.data_sz = 0;
        }

        if (!strncmp(fg, "M", sizeof("M")))
        {
            flag = PCM_MMAP;
        }
        else if (!strncmp(fg, "N", sizeof("N")))
        {
            flag = PCM_NMMAP;
        }

        cformat = get_format_desc(format);

        if (cformat != NULL && fn != NULL)
        {
            UTIL_LOG_MSG("aplay: Playing '%s':%s", fn, cformat);
        }

        err = play_file(hdr.sample_rate, hdr.num_channels, fd, flag, device, hdr.data_sz, pcm);
    }while(0);

    if (0 != err)
    {
       UTIL_LOG_MSG("failed to play file: %d", err);
    }
    return err;
}

static void play_voice_thrd()
{
    pcm_flag = 0;
    return play_wav("N", 44100, 2, "hw:0,2", "dummy", audio_pcm);
}

static void play_audio_thrd()
{
    pcm_flag = 1;
    return play_wav("N", 44100, 2, "hw:0,0", "/usr/bin/test_11.wav", voice_pcm);
}

static void rec_voice_thrd()
{
    record_file(44100, 2, 0, 2147483648LL, PCM_NMMAP, "hw:0,2");
}

static void play_voice()
{
    pthread_t play_thread;
    if (pthread_create(&play_thread, NULL, (void*)&play_voice_thrd, NULL))
    {
        UTIL_LOG_MSG("play_voice_thrd failed");
    }
}

static void play_audio()
{
    pthread_t play_audio_thread;
    if (pthread_create(&play_audio_thread, NULL, (void*)&play_audio_thrd, NULL))
    {
        UTIL_LOG_MSG("play_audio_thrd failed");
    }
}

static void rec_voice()
{
    pthread_t rec_thread;
    if (pthread_create(&rec_thread, NULL, (void*)&rec_voice_thrd, NULL)) 
    {
        UTIL_LOG_MSG("rec_voice_thrd failed");
    }
}

static int mcm_srv_enable_voice_stream()
{
    int err = 0;

    UTIL_LOG_MSG("mcm_srv_enable_voice_stream ENTER");
    do
    {
        if (NULL == uc_mgr)
        {
            UTIL_LOG_MSG("uc_mgr NULL");
            err = 1;
            break;
       }
       err = snd_use_case_set(uc_mgr, "_verb\0", "Voice Call");
       if (err < 0)
       {
           UTIL_LOG_MSG(" failed to use_case_set _verb err:%d", err);
           err = 1;
           break;
       }

       err = snd_use_case_set(uc_mgr, "_enadev", "Headphones");
       if ( err < 0)
       {
           UTIL_LOG_MSG(" failed to use_case_set enadev Headphone err:%d", err);
           err = 1;
           break;
       }

       err = snd_use_case_set(uc_mgr, "_enadev", "Headset");
       if ( err < 0)
       {
           UTIL_LOG_MSG(" failed to use_case_set enadev Headset err:%d", err);
           err = 1;
           break;
       }
       play_voice();

       sleep(2);
       rec_voice();
    }while (0); 

    UTIL_LOG_MSG(" mcm_srv_enable_voice_stream EXIT err:%d", err);
    return err;
}

static int mcm_srv_enable_audio_stream()
{
    int err = 0;

    UTIL_LOG_MSG(" mcm_srv_enable_audio_stream ENTER");
    do
    {
        if (NULL == uc_mgr)
        {
            UTIL_LOG_MSG("uc_mgr NULL");
            err = 1;
            break;
        }

        err = snd_use_case_set(uc_mgr, "_verb\0", "HiFi");
        if (err < 0)
        {
            UTIL_LOG_MSG(" failed to use_case_set _verb err:%d", err);
            err = 1;
            break;
        }

        err = snd_use_case_set(uc_mgr, "_enadev", "Headphones");
        if ( err < 0)
        {
            UTIL_LOG_MSG(" failed to use_case_set enadev Headphone err:%d", err);
            err = 1;
            break;
        }
        play_audio();
    }while (0);

    UTIL_LOG_MSG(" mcm_srv_enable_audio_stream EXIT err:%d", err);
    return err;
}

static int mcm_srv_disable_audio_stream()
{
    int err = 0;
    UTIL_LOG_MSG(" mcm_srv_disable_audio_stream ENTER");
    do
    {
        if (audio_pcm != NULL)
        {
            UTIL_LOG_MSG("audio_pcm not NULL, close it");
            pcm_close(audio_pcm);
        }

        err = snd_use_case_set(uc_mgr, "_verb\0", SND_USE_CASE_VERB_INACTIVE);
        if ( err < 0)
        {
            UTIL_LOG_MSG("failed to snd_use_case_set SND_USE_CASE_VERB_INACTIVE err:%d", err);
            err = 1;
            break;
        }

        err = snd_use_case_set(uc_mgr, "_disdev", "Headphones");
        if ( err < 0)
        {
            UTIL_LOG_MSG(" failed to use_case_set enadev Headphone err:%d", err);
            err = 1;
            break;
        }
    }while (0);

    UTIL_LOG_MSG(" mcm_srv_disable_audio_stream EXIT, err:%d", err);
    return err;
}

static int mcm_srv_disable_voice_stream()
{
    int err = 0;
    UTIL_LOG_MSG(" mcm_srv_disable_voice_stream ENTER");
   
    do
    {
        if (voice_pcm != NULL)
        {
            UTIL_LOG_MSG("voice_pcm not NULL, close it");
            pcm_close(voice_pcm);
        }

        err = snd_use_case_set(uc_mgr, "_disdev", "Headphones");
        if ( err < 0)
        {
            UTIL_LOG_MSG(" failed to use_case_set disdev Headphone err:%d", err);
            err = 1;
            break;
        }

        err = snd_use_case_set(uc_mgr, "_disdev", "Headset");
        if ( err < 0)
        {
            UTIL_LOG_MSG(" failed to use_case_set disdev Headset err:%d", err);
            err = 1;
            break;
        }
    }while(0);

    UTIL_LOG_MSG(" mcm_srv_disable_voice_stream EXIT err:%d", err);
    return err;
}

static int mcm_srv_mute_voice_stream(int mute_type)
{

   int ret = 0;
   mixer_hndl = mixer_open("/dev/snd/controlC0");

   UTIL_LOG_MSG(" mcm_srv_mute_voice_stream ENTRY");

   do
   {
       if (NULL == mixer_hndl)
       {
           UTIL_LOG_MSG("mixer_hndl is NULL ");
           ret = 1;
           break;
       }

       ctl_hndl = mixer_get_control(mixer_hndl, "Voice Tx Mute", 0);

       if (NULL == ctl_hndl)
       {
           UTIL_LOG_MSG("ctl_hndl is NULL");
           ret = 1;
           break;
       }

       ret = mixer_ctl_set(ctl_hndl, mute_type);

       if (ret < 0)
       {
           UTIL_LOG_MSG("failed from mixer_ctl_set, ret:%d", ret);
           ret = 1;
           break;
       }

       mixer_close(mixer_hndl);
   }while(0);

   UTIL_LOG_MSG(" mcm_srv_mute_voice_stream EXIT");
   return ret;
}

static int get_ucm_sound_card_name(char **ucm_sound_card_name)
{
    FILE *fp = NULL;
    int   ret = 0;
    int   reti = 0;
    char  sound_card_info[SOUND_CARD_BUFFER_SIZE];
    int   i = 0;
    int   ucm_card_name_size = 0;
    char *kernel_snd_card = NULL;
    int   snd_card_table_size;

    do
    {
        if ( NULL == (fp = fopen("/proc/asound/cards","r")))
        {
            UTIL_LOG_MSG("Cannot open /proc/asound/cards file to get sound card info");
            ret = 1;
            break;
        }

        if ( NULL != (fgets(sound_card_info, sizeof(sound_card_info), fp)))
        {
            UTIL_LOG_MSG("Kernel sound card=%s", sound_card_info);

            snd_card_table_size = sizeof(snd_card_mapping_list)/sizeof(struct snd_card_mapping);

            for (i = 0; i < snd_card_table_size; i++)
            {
                kernel_snd_card = strstr(sound_card_info,
                                         snd_card_mapping_list[i].kernel_card_name);

                if ( NULL != kernel_snd_card &&
                     !strncmp(kernel_snd_card,
                              snd_card_mapping_list[i].kernel_card_name,
                              strlen(snd_card_mapping_list[i].kernel_card_name)))
                {
                    ucm_card_name_size =  strlen(snd_card_mapping_list[i].ucm_card_name)+1;
                    *ucm_sound_card_name = (char *)malloc(sizeof(char ) * ucm_card_name_size);

                    if (*ucm_sound_card_name == NULL)
                    {
                        UTIL_LOG_MSG("memory allocation failed");
                        reti = 1;
                        break;
                    }
                    memset(*ucm_sound_card_name, 0, ucm_card_name_size);
                    memcpy(*ucm_sound_card_name,
                            snd_card_mapping_list[i].ucm_card_name,
                            ucm_card_name_size);
                    break;
                }
            }
            if (reti == 1 || i == snd_card_table_size)
            {
                ret = 1;
            }
        }/* if */
        else
        {
            ret = 1;
        }
    }while(0);
    if (ret == 1)
    {
        UTIL_LOG_MSG("No mapping b/w Kernel Sound card and UCM sound card");
    }

    if (NULL != fp)
    {
        fclose(fp);
    }
    return ret;
}

static int mcm_srv_snd_init()
{
    int ret = 0;
    char *ucm_sound_card_name = NULL;
    int tries = 0;

    do
    {
        if ( NULL != uc_mgr)
        {
            snd_use_case_mgr_close(uc_mgr);
            uc_mgr = NULL;
        }

        /* Try opening sound device. It sometimes is unavailable immediately
         * after bootup. So, retry a number of times before giving up
         */
        for (tries = 0; tries < MCM_SRV_AUDIO_MAX_TRIES; tries ++)
        {
            ret = get_ucm_sound_card_name(&ucm_sound_card_name);
            if (ret != 0)
            {
                UTIL_LOG_MSG("Failed to get UCM card name (attempt %d)", tries + 1);
                sleep(MCM_SRV_AUDIO_OPEN_WAIT);
                continue;
            }

            UTIL_LOG_MSG("ucm_sound_card_name:%s", ucm_sound_card_name);
            ret = snd_use_case_mgr_open(&uc_mgr, ucm_sound_card_name);

            if (ret != 0)
            {
                UTIL_LOG_MSG("failed to open sound card err:%d", ret);
                sleep(MCM_SRV_AUDIO_OPEN_WAIT);
            }
            else
            {
                UTIL_LOG_MSG("Succesfully opened ucm sound card %s", ucm_sound_card_name);
                break;
            }
        }
        if (tries == MCM_SRV_AUDIO_MAX_TRIES)
        {
            UTIL_LOG_MSG("Unable to open sound card %s after %d attempts."
                         "No audio will be available", ucm_sound_card_name, MCM_SRV_AUDIO_MAX_TRIES);
        }
    }while(0);

    return ret;
}

mcm_audio_handler the_audio_handler =
{
    init: mcm_srv_snd_init,
    enable_audio_stream: mcm_srv_enable_audio_stream,
    disable_audio_stream: mcm_srv_disable_audio_stream,
    enable_voice_stream: mcm_srv_enable_voice_stream,
    disable_voice_stream: mcm_srv_disable_voice_stream,
    mute_voice_stream: mcm_srv_mute_voice_stream
};
mcm_audio_handler * get_audio_handler()
{
    return &the_audio_handler;
}

#endif
