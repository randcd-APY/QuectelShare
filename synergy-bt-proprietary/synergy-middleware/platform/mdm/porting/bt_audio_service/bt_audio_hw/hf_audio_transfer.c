/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

#include "connx_common_def.h"
#include "connx_log.h"

#include "hf_audio_transfer.h"
#include "pcm_alsa.h"
#include "amix_alsa.h"

struct hf_audio
{
    struct pcm *local_capture;
    struct pcm *bt_playback;
    struct pcm *bt_capture;
    struct pcm *local_playback;
};

static struct hf_audio hfAudio =
{
    .local_capture  = NULL,
    .bt_playback    = NULL,
    .bt_capture     = NULL,
    .local_playback = NULL,
};

typedef struct
{
    int                 mic_pipe[2];
    int                 recv_pipe[2];
    pthread_t           recv_in_thread;
    pthread_t           recv_out_thread;
    pthread_t           mic_out_thread;
    pthread_t           mic_in_thread;
    pthread_mutex_t     mutex;
    char                *mic_in_buff;
    char                *recv_in_buff;
    char                *mic_out_buff;
    char                *recv_out_buff;
    long                delay_playback;
    int                 stop;
    int                 mic_muted;
} HfAudioTransferInstance;

static HfAudioTransferInstance hfAudioInst =
{
    .mic_in_buff       = NULL,
    .recv_in_buff      = NULL,
    .mic_out_buff      = NULL,
    .recv_out_buff     = NULL,
};

HfAudioTransferInstance *GetHfAudioTransferInstance(void)
{
    return &hfAudioInst;
}

static void *recv_in_thread_func(void *arg)
{
    HfAudioTransferInstance *inst = GetHfAudioTransferInstance();

    while (!inst->stop)
    {
        pcm_read_data(hfAudio.bt_capture, inst->recv_in_buff, hfAudio.bt_capture->chunk_size);

        if (inst->stop)
            break;

        write(inst->recv_pipe[1], inst->recv_in_buff, hfAudio.bt_capture->chunk_size * 2);

        if (inst->stop)
            break;
    }

    return NULL;
}

static void *recv_out_thread_func(void *arg)
{
    HfAudioTransferInstance *inst = GetHfAudioTransferInstance();
    ssize_t r;

    while (!inst->stop)
    {
        r = read(inst->recv_pipe[0], inst->recv_out_buff, hfAudio.local_playback->chunk_size * 2);

        if (inst->stop)
            break;

        if (r > 0)
        {
            pcm_write_data(hfAudio.local_playback, inst->recv_out_buff, r / 2);

            if (inst->stop)
                break;
        }
    }

    return NULL;
}

static void *mic_in_thread_func(void *arg)
{
    HfAudioTransferInstance *inst = GetHfAudioTransferInstance();

    while (!inst->stop)
    {
        pcm_read_data(hfAudio.local_capture, inst->mic_in_buff, hfAudio.local_capture->chunk_size);

        if (inst->stop)
            break;

        /* Clear mic input if muted */
        if (inst->mic_muted)
            memset(inst->mic_in_buff, 0, hfAudio.local_capture->chunk_size * 2);

        write(inst->mic_pipe[1], inst->mic_in_buff, hfAudio.local_capture->chunk_size * 2);

        if (inst->stop)
            break;
    }

    return NULL;
}

static void *mic_out_thread_func(void *arg)
{
    HfAudioTransferInstance *inst = GetHfAudioTransferInstance();
    ssize_t r;

    while (!inst->stop)
    {
        r = read(inst->mic_pipe[0], inst->mic_out_buff, hfAudio.bt_playback->chunk_size * 2);

        if (inst->stop)
            break;

        if (r > 0)
            pcm_write_data(hfAudio.bt_playback, inst->mic_out_buff, r / 2);

        if (inst->stop)
            break;
    }

    return NULL;
}

void init_hf_audio()
{
    AudioDeviceInfo *info = GetDeviceInfo();
    unsigned int platform = (info == NULL) ? INT_PLATFORM_UNKNOWN : info->platform;

    switch (platform)
    {
        case INT_PLATFORM_MDM9640:
        case INT_PLATFORM_MSMZIRC:
            init_amix_hf_audio_for_9x45_le();
            break;
        case INT_PLATFORM_MDM9650:
            init_amix_hf_audio_for_9x50_le();
            break;
        case INT_PLATFORM_MDM9628:
            /* [MDM] it is a little strange here, it seems that utilizing the config of 9x45_le
                can make audio workable. */
            init_amix_hf_audio_for_9x45_le();
            break;

        default:
            break;
    }
}

void deinit_hf_audio()
{
    AudioDeviceInfo *info = GetDeviceInfo();
    unsigned int platform = (info == NULL) ? INT_PLATFORM_UNKNOWN : info->platform;

    switch (platform)
    {
        case INT_PLATFORM_MDM9640:
        case INT_PLATFORM_MSMZIRC:
            deinit_amix_hf_audio_for_9x45_le();
            break;
        case INT_PLATFORM_MDM9650:
            deinit_amix_hf_audio_for_9x50_le();
            break;
        case INT_PLATFORM_MDM9628:
            /* [MDM] it is a little strange here, it seems that utilizing the config of 9x45_le
                can make audio workable. */
            deinit_amix_hf_audio_for_9x45_le();
            break;

        default:
            break;
    }
}

int start_hf_audio(unsigned int sampleRate)
{
    char local_dev[PCM_DEVICE_NAME_MAX_LENGTH];
    char bt_dev[PCM_DEVICE_NAME_MAX_LENGTH];
    AudioDeviceInfo *info = GetDeviceInfo();
    HfAudioTransferInstance *inst = GetHfAudioTransferInstance();
    int ret, r;

    IFLOG(DebugOut(DEBUG_MESSAGE, TEXT("<%s> start hf audio transfer"), __FUNCTION__));

    if (!info || !inst)
        return -1;

    unsigned int platform = info->platform;

    snprintf(local_dev, sizeof(local_dev), "hw:%d,%d", info->local_card_id, info->local_device_id);
    snprintf(bt_dev, sizeof(bt_dev), "hw:%d,%d", info->bt_card_id, info->bt_device_id);
    /***
     *** For 9650 device audio device is different
     *** AFE DL:bt_capture & local playback used SEC_AUXPCM_HOSTLESS Capture hw:0,33 Primary MI2S_RX  hw:0,3
     *** ASM UL:local_capture & bt_play both used MultiMedia6 hw:0,15
     ***/
    
    if (platform == INT_PLATFORM_MDM9650)
        hfAudio.local_capture = pcm_open_dev("hw:0,15", PCM_CAPTURE, sampleRate, OVERALL_LATENCY);
    else
        hfAudio.local_capture = pcm_open_dev(local_dev, PCM_CAPTURE, sampleRate, OVERALL_LATENCY);

    if (!hfAudio.local_capture)
    {
        IFLOG(DebugOut(DEBUG_ERROR, TEXT("<%s> Opening local PCM capture device card_id(%d) device_id(%d) failed."),
                       __FUNCTION__, info->local_card_id, info->local_device_id));
        ret = -EIO;
        goto exit;
    }
    
    if (platform == INT_PLATFORM_MDM9650)
        hfAudio.bt_playback = pcm_open_dev("hw:0,15", PCM_PLAYBACK, sampleRate, OVERALL_LATENCY);
    else
        hfAudio.bt_playback = pcm_open_dev(bt_dev, PCM_PLAYBACK, sampleRate, OVERALL_LATENCY);

    if (!hfAudio.bt_playback)
    {
        IFLOG(DebugOut(DEBUG_ERROR, TEXT("<%s> Opening bt PCM playback device card_id(%d) device_id(%d) failed."),
                       __FUNCTION__, info->bt_card_id, info->bt_device_id));
        ret = -EIO;
        goto exit;
    }
    
    if (platform == INT_PLATFORM_MDM9650)    
        hfAudio.bt_capture = pcm_open_dev("hw:0,33", PCM_CAPTURE, sampleRate, OVERALL_LATENCY);    
    else
        hfAudio.bt_capture = pcm_open_dev(bt_dev, PCM_CAPTURE, sampleRate, OVERALL_LATENCY);

    if (!hfAudio.bt_capture)
    {
        IFLOG(DebugOut(DEBUG_ERROR, TEXT("<%s> Opening bt PCM capture device card_id(%d) device_id(%d) failed."),
                       __FUNCTION__, info->bt_card_id, info->bt_device_id));
        ret = -EIO;
        goto exit;
    }
    
    if (platform == INT_PLATFORM_MDM9650)   
        hfAudio.local_playback = pcm_open_dev("hw:0,3", PCM_PLAYBACK, sampleRate, OVERALL_LATENCY);
    else
        hfAudio.local_playback = pcm_open_dev(local_dev, PCM_PLAYBACK, sampleRate, OVERALL_LATENCY);

    if (!hfAudio.local_playback)
    {
        IFLOG(DebugOut(DEBUG_ERROR, TEXT("<%s> Opening local PCM playback device card_id(%d) device_id(%d) failed."),
                       __FUNCTION__, info->local_card_id, info->local_device_id));
        ret = -EIO;
        goto exit;
    }

    inst->mic_in_buff = malloc(hfAudio.local_capture->chunk_size * 2);

    if (!inst->mic_in_buff)
        goto mem_alloc_failed;

    inst->recv_in_buff = malloc(hfAudio.bt_capture->chunk_size * 2);

    if (!inst->recv_in_buff)
        goto mem_alloc_failed;

    inst->mic_out_buff = malloc(hfAudio.bt_playback->chunk_size * 2);

    if (!inst->mic_out_buff)
        goto mem_alloc_failed;

    inst->recv_out_buff = malloc(hfAudio.local_playback->chunk_size * 2);

    if (!inst->recv_out_buff)
        goto mem_alloc_failed;

    pipe(inst->mic_pipe);
    pipe(inst->recv_pipe);
    fcntl(inst->mic_pipe[1], F_SETFL, O_NONBLOCK);
    fcntl(inst->recv_pipe[1], F_SETFL, O_NONBLOCK);

    inst->stop = 0;
    inst->delay_playback = -1;
    pthread_mutex_init(&inst->mutex, NULL);

    /* Prepare read some date, to avoid underrun */
    r = pcm_read_data(hfAudio.bt_capture, inst->recv_in_buff, hfAudio.bt_capture->chunk_size);

    if (r > 0)
    {
        write(inst->recv_pipe[1], inst->recv_in_buff, r * 2);
    }

    r = pcm_read_data(hfAudio.local_capture, inst->mic_in_buff, hfAudio.local_capture->chunk_size);

    if (r > 0)
    {
        write(inst->mic_pipe[1], inst->mic_in_buff, r * 2);
    }

    pthread_create(&inst->recv_in_thread, NULL, recv_in_thread_func, NULL);
    pthread_create(&inst->recv_out_thread, NULL, recv_out_thread_func, NULL);
    pthread_create(&inst->mic_in_thread, NULL, mic_in_thread_func, NULL);
    pthread_create(&inst->mic_out_thread, NULL, mic_out_thread_func, NULL);
    return 0;

mem_alloc_failed:

    IFLOG(DebugOut(DEBUG_ERROR, TEXT("<%s> mem_alloc_failed"), __FUNCTION__));

    if (inst->mic_in_buff)
        free(inst->mic_in_buff);

    if (inst->recv_in_buff)
        free(inst->recv_in_buff);

    if (inst->mic_out_buff)
        free(inst->mic_out_buff);

    if (inst->recv_out_buff)
        free(inst->recv_out_buff);

    return -ENOMEM;

exit:
    stop_hf_audio();
    return ret;
}

void stop_hf_audio()
{
    HfAudioTransferInstance *inst = GetHfAudioTransferInstance();

    IFLOG(DebugOut(DEBUG_MESSAGE, TEXT("<%s> stop hf audio transfer"), __FUNCTION__));

    if (!inst)
		return;

    inst->stop = 1;
    inhibit_alsa_err();
    /* [QTI] Fix KW issue#267496. */
    if (hfAudio.bt_capture && hfAudio.bt_capture->handle)
        snd_pcm_drop(hfAudio.bt_capture->handle);
    /* [QTI] Fix KW issue#267497. */
    if (hfAudio.bt_playback && hfAudio.bt_playback->handle)
        snd_pcm_drop(hfAudio.bt_playback->handle);
    /* [QTI] Fix KW issue#267498. */
    if (hfAudio.local_capture && hfAudio.local_capture->handle)
        snd_pcm_drop(hfAudio.local_capture->handle);
    /* [QTI] Fix KW issue#267499. */
    if (hfAudio.local_playback && hfAudio.local_playback->handle)
        snd_pcm_drop(hfAudio.local_playback->handle);

    close(inst->mic_pipe[0]);
    close(inst->mic_pipe[1]);
    close(inst->recv_pipe[0]);
    close(inst->recv_pipe[1]);

    pthread_join(inst->recv_in_thread, NULL);
    pthread_join(inst->mic_out_thread, NULL);
    pthread_join(inst->recv_out_thread, NULL);
    pthread_join(inst->mic_in_thread, NULL);

    pthread_mutex_destroy(&inst->mutex);

    if (inst->mic_in_buff)
        free(inst->mic_in_buff);

    if (inst->recv_in_buff)
        free(inst->recv_in_buff);

    if (inst->mic_out_buff)
        free(inst->mic_out_buff);

    if (inst->recv_out_buff)
        free(inst->recv_out_buff);

    if (hfAudio.local_capture)
    {
        pcm_close_dev(hfAudio.local_capture);
        hfAudio.local_capture = NULL;
    }

    if (hfAudio.bt_playback)
    {
        pcm_close_dev(hfAudio.bt_playback);
        hfAudio.bt_playback = NULL;
    }

    if (hfAudio.bt_capture)
    {
        pcm_close_dev(hfAudio.bt_capture);
        hfAudio.bt_capture = NULL;
    }

    if (hfAudio.local_playback)
    {
        pcm_close_dev(hfAudio.local_playback);
        hfAudio.local_playback = NULL;
    }
}

void set_mic_mute(int muted)
{
    HfAudioTransferInstance *inst = GetHfAudioTransferInstance();
    
    if (!inst)
        return;
    
    inst->mic_muted = muted;
}
