/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "bt_audio_hw.h"

#include "connx_util.h"
#include "connx_log.h"

static AudioDeviceInfo deviceInfo =
{
    .init = false,
};

AudioDeviceInfo *GetDeviceInfo()
{
    return &deviceInfo;
}

static char *GetPlatformName()
{
    FILE *fp;
    char buffer[MAX_BUF_SIZE];
    char name[MAX_BUF_SIZE];
    char *match;

    if ((fp = fopen("/proc/cpuinfo", "r")) == NULL)
    {
        printf("can't open file, err %d (%s)", errno, strerror(errno));
        return NULL;
    }

    while (fgets(buffer, MAX_BUF_SIZE - 1, fp) != NULL)
    {
        match = strstr(buffer, "Hardware");

        if (match != NULL)
        {
            sscanf(match, "Hardware        : Qualcomm Technologies, Inc %s", name);
            return ConnxStrDup(name);
        }
    }

    return NULL;
}

static void GetAudioCardDeviceNumber(char *alias, int *card, int *device)
{
    FILE *fp;
    int card_number;
    int device_number;
    char buffer[MAX_BUF_SIZE];
    char name[MAX_BUF_SIZE];
    char *match;

    if ((fp = fopen("/proc/asound/pcm", "r")) == NULL)
    {
        printf("can't open file, err %d (%s)", errno, strerror(errno));
        return;
    }

    while (fgets(buffer, MAX_BUF_SIZE - 1, fp) != NULL)
    {
        match = strstr(buffer, alias);

        if (match != NULL)
        {
            sscanf(buffer, "%d-%d: ", &card_number, &device_number);
            *card = card_number;
            *device = device_number;
            return;
        }
    }
}

static unsigned int Map2PlatformName(char *platform)
{
    unsigned int name = INT_PLATFORM_UNKNOWN;

    if (!strcmp(platform, TEXT_PLATFORM_MDM9640))
    {
        name = INT_PLATFORM_MDM9640;
    }
    else if (!strcmp(platform, TEXT_PLATFORM_MDM9628))
    {
        name = INT_PLATFORM_MDM9628;
    }
    else if (!strcmp(platform, TEXT_PLATFORM_MSMZIRC))
    {
        name = INT_PLATFORM_MSMZIRC;
    }
    else if (!strcmp(platform, TEXT_PLATFORM_MDM9650))
    {
        name = INT_PLATFORM_MDM9650;
    }

    return name;
}

void InitPlatformDeviceInfo()
{
    char *platform;
    AudioDeviceInfo *info = GetDeviceInfo();
    int local_card_id, local_device_id;
    int bt_card_id, bt_device_id;
    int modem_card_id, modem_device_id;

    if (!info)
    {
        return;
    }

    platform = GetPlatformName();

    if (!platform)
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Failed to get platform name"), __FUNCTION__));
        return;
    }

    if (!strcmp(platform, TEXT_PLATFORM_MDM9640) ||
        !strcmp(platform, TEXT_PLATFORM_MDM9628) ||
        !strcmp(platform, TEXT_PLATFORM_MSMZIRC))
    {
        GetAudioCardDeviceNumber(TEXT_MULTIMEDIA_1, &local_card_id, &local_device_id);
        GetAudioCardDeviceNumber(TEXT_MULTIMEDIA_5, &bt_card_id, &bt_device_id);
        GetAudioCardDeviceNumber(TEXT_CS_VOICE, &modem_card_id, &modem_device_id);
    }
    else if (!strcmp(platform, TEXT_PLATFORM_MDM9650))
    {
        GetAudioCardDeviceNumber(TEXT_MULTIMEDIA_1, &local_card_id, &local_device_id);
        GetAudioCardDeviceNumber(TEXT_MULTIMEDIA_5, &bt_card_id, &bt_device_id);
        GetAudioCardDeviceNumber(TEXT_MMODE1, &modem_card_id, &modem_device_id);
    }

    else
    {
        /* TODO  add other known platform */
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Unknown platform name"), __FUNCTION__));
        return;
    }

    info->platform         = Map2PlatformName(platform);
    info->local_card_id    = local_card_id;
    info->local_device_id  = local_device_id;
    info->bt_card_id       = bt_card_id;
    info->bt_device_id     = bt_device_id;
    info->modem_card_id    = modem_card_id;
    info->modem_device_id  = modem_device_id;
}

