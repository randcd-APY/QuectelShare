#ifndef _BT_AUDIO_SERVICE_SEF_H_
#define _BT_AUDIO_SERVICE_SEF_H_

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

void HandleInitHfAudioInd();

void HandleStartHfAudioInd(uint16_t sampleRate);

void HandleStopHfAudioInd();

void HandleDeinitHfAudioInd();

void HandleInitHfgAudioInd();

void HandleStartHfgAudioInd(uint16_t sampleRate);

void HandleStopHfgAudioInd();

void HandleDeinitHfgAudioInd();

void HandleInitA2dpStreamingInd();

void HandleStartA2dpStreamingInd(uint16_t sampleRate);

void HandleStopA2dpStreamingInd();

void HandleDeinitA2dpStreamingInd();

#ifdef __cplusplus
}
#endif

#endif  /* _BT_AUDIO_SERVICE_SEF_H_ */

