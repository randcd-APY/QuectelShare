#ifndef _BT_AUDIO_HW_H_
#define _BT_AUDIO_HW_H_

/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/
#include "connx_common_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TEXT_PLATFORM_MDM9640             "MDM9640"
#define TEXT_PLATFORM_MDM9628             "MDM9628"
#define TEXT_PLATFORM_MSMZIRC             "MSMZIRC"
#define TEXT_PLATFORM_MDM9650             "MDM9650"

#define TEXT_MULTIMEDIA_1                 "MultiMedia1"
#define TEXT_MULTIMEDIA_5                 "MultiMedia5"
#define TEXT_CS_VOICE                     "CS-Voice"
#define TEXT_MMODE1                       "VoiceMMode1"


#define INT_PLATFORM_UNKNOWN              0x0000
#define INT_PLATFORM_MDM9640              0x0001
#define INT_PLATFORM_MDM9628              0x0002
#define INT_PLATFORM_MSMZIRC              0x0003
#define INT_PLATFORM_MDM9650              0x0004


#define PCM_CAPTURE                       0x0000
#define PCM_PLAYBACK                      0x0001

#define OVERALL_LATENCY                   200000

#define AUDIO_ROUTE_MODEM                 0x0000
#define AUDIO_ROUTE_PCM                   0x0001

typedef struct
{
	bool          init;				   /* true: instance is initialized, false: not. */
    unsigned int  platform;
    int           local_card_id;
    int           local_device_id;
    int           bt_card_id;
    int           bt_device_id;
    int           modem_card_id;
    int           modem_device_id;
}AudioDeviceInfo;

AudioDeviceInfo *GetDeviceInfo();
void InitPlatformDeviceInfo();

#ifdef __cplusplus
}
#endif

#endif  /* _BT_AUDIO_HW_H_ */

