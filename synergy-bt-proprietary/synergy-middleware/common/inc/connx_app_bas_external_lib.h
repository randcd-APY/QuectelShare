#ifndef _CONNX_APP_BAS_EXTERNAL_LIB_H_
#define _CONNX_APP_BAS_EXTERNAL_LIB_H_

/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/
#include "connx_bas_common.h"

#ifdef __cplusplus
extern "C" {
#endif

void ConnxAppBtAudioServiceReq(ConnxBasPrim type, uint16_t sampleRate);
void ConnxAppBasInitHfAudio();
void ConnxAppBasStartHfAudio(uint16_t sample_rate);
void ConnxAppBasStopHfAudio();
void ConnxAppBasDeinitHfAudio();
void ConnxAppBasInitHfgAudio();
void ConnxAppBasStartHfgAudio(uint16_t sample_rate);
void ConnxAppBasStopHfgAudio();
void ConnxAppBasDeinitHfgAudio();


#ifdef __cplusplus
}
#endif

#endif  /* _CONNX_APP_BAS_EXTERNAL_LIB_H_ */

