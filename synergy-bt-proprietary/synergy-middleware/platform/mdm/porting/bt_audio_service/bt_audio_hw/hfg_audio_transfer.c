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

#include "connx_common_def.h"
#include "connx_log.h"

#include "bt_audio_hw.h"
#include "hfg_audio_transfer.h"
#include "pcm_alsa.h"
#include "amix_alsa.h"

struct hfg_audio
{
    struct pcm *modem_capture;
    struct pcm *modem_playback;
};

static struct hfg_audio hfgAudio =
{
    .modem_capture  = NULL,
    .modem_playback = NULL,
};

void init_hfg_audio()
{
    AudioDeviceInfo *info = GetDeviceInfo();
    unsigned int platform = (info == NULL) ? INT_PLATFORM_UNKNOWN : info->platform;

    switch (platform)
    {
        case INT_PLATFORM_MDM9640:
            init_amix_hfg_local_audio_for_9x45_le_1x();
            deinit_amix_hfg_local_audio_for_9x45_le_1x();
            init_amix_hfg_bt_audio_for_9x45_le_1x();
            deinit_amix_hfg_bt_audio_for_9x45_le_1x();
            break;

        case INT_PLATFORM_MSMZIRC:
            init_amix_hfg_local_audio_for_9x45_le_2x();
            deinit_amix_hfg_local_audio_for_9x45_le_2x();
            init_amix_hfg_bt_audio_for_9x45_le_2x();
            deinit_amix_hfg_bt_audio_for_9x45_le_2x();
            break;

        case INT_PLATFORM_MDM9628:
            init_amix_hfg_local_audio_for_9x07();
            deinit_amix_hfg_local_audio_for_9x07();
            init_amix_hfg_bt_audio_for_9x07();
            deinit_amix_hfg_bt_audio_for_9x07();
            break;

        case INT_PLATFORM_MDM9650:
            init_amix_hfg_local_audio_for_9x50();
            deinit_amix_hfg_local_audio_for_9x50();
            init_amix_hfg_bt_audio_for_9x50();
            deinit_amix_hfg_bt_audio_for_9x50();
            break;

        default:
            break;
    }
}

void deinit_hfg_audio()
{

}

static void config_hfg_audio(unsigned int platform, unsigned int sample_rate)
{
    switch (platform)
    {
        case INT_PLATFORM_MDM9640:
            deinit_amix_hfg_local_audio_for_9x45_le_1x();
            init_amix_hfg_bt_audio_for_9x45_le_1x();
            configure_afe_port_sample_rate(sample_rate);
            break;

        case INT_PLATFORM_MSMZIRC:
            deinit_amix_hfg_local_audio_for_9x45_le_2x();
            init_amix_hfg_bt_audio_for_9x45_le_2x();
            configure_afe_port_sample_rate(sample_rate);
            break;

        case INT_PLATFORM_MDM9628:
            deinit_amix_hfg_local_audio_for_9x07();
            init_amix_hfg_bt_audio_for_9x07();
            configure_afe_port_sample_rate(sample_rate);
            break;
        case INT_PLATFORM_MDM9650:
            deinit_amix_hfg_local_audio_for_9x50();
            init_amix_hfg_bt_audio_for_9x50();
            configure_afe_port_sample_rate(sample_rate);
            break;
        default:
            break;
    }
}

static void deconfig_hfg_audio(unsigned int platform)
{
    switch (platform)
    {
        case INT_PLATFORM_MDM9640:
            deinit_amix_hfg_bt_audio_for_9x45_le_1x();
            init_amix_hfg_local_audio_for_9x45_le_1x();
            break;

        case INT_PLATFORM_MSMZIRC:
            deinit_amix_hfg_bt_audio_for_9x45_le_2x();
            init_amix_hfg_local_audio_for_9x45_le_2x();
            break;

        case INT_PLATFORM_MDM9628:
            deinit_amix_hfg_bt_audio_for_9x07();
            init_amix_hfg_local_audio_for_9x07();
            break;

        case INT_PLATFORM_MDM9650:
            deinit_amix_hfg_bt_audio_for_9x50();
            init_amix_hfg_local_audio_for_9x50();
            break;

        default:
            break;
    }
}

int start_hfg_audio(unsigned int sampleRate)
{
    AudioDeviceInfo *info = GetDeviceInfo();
    unsigned int platform = (info == NULL) ? INT_PLATFORM_UNKNOWN : info->platform;
    char modem_dev[PCM_DEVICE_NAME_MAX_LENGTH];
    int ret;

    if (!info)
        return -1;

    IFLOG(DebugOut(DEBUG_MESSAGE, TEXT("<%s> start hfg audio transfer"), __FUNCTION__));
    config_hfg_audio(info->platform, sampleRate);
    
    snprintf(modem_dev, sizeof(modem_dev), "hw:%d,%d", info->modem_card_id, info->modem_device_id);

    hfgAudio.modem_capture = pcm_open_dev(modem_dev, PCM_CAPTURE, sampleRate, OVERALL_LATENCY);

    if (!hfgAudio.modem_capture)
    {
        IFLOG(DebugOut(DEBUG_ERROR, TEXT("<%s> Opening PCM capture device card_id(%d) device_id(%d) failed."),
                       __FUNCTION__, info->modem_card_id, info->modem_device_id));
        ret = -EIO;
        goto exit;
    }

    hfgAudio.modem_playback = pcm_open_dev(modem_dev, PCM_PLAYBACK, sampleRate, OVERALL_LATENCY);

    if (!hfgAudio.modem_playback)
    {
        IFLOG(DebugOut(DEBUG_ERROR, TEXT("<%s> Opening PCM playback device card_id(%d) device_id(%d) failed."),
                       __FUNCTION__, info->modem_card_id, info->modem_device_id));
        ret = -EIO;
        goto exit;
    }

    pcm_start(hfgAudio.modem_capture);
    pcm_start(hfgAudio.modem_playback);

    return 0;

exit:
    stop_hfg_audio();

    return ret;
}

void stop_hfg_audio()
{
    AudioDeviceInfo *info = GetDeviceInfo();
    unsigned int platform = (info == NULL) ? INT_PLATFORM_UNKNOWN : info->platform;

    IFLOG(DebugOut(DEBUG_MESSAGE, TEXT("<%s> stop hfg audio transfer"), __FUNCTION__));

    if (hfgAudio.modem_capture)
    {
        pcm_close_dev(hfgAudio.modem_capture);
        hfgAudio.modem_capture = NULL;
    }

    if (hfgAudio.modem_playback)
    {
        pcm_close_dev(hfgAudio.modem_playback);
        hfgAudio.modem_playback = NULL;
    }

    deconfig_hfg_audio(platform);
}

