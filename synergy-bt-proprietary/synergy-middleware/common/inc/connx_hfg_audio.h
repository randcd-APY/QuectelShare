/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifndef _CONNX_HFG_AUDIO_H_
#define _CONNX_HFG_AUDIO_H_

#ifdef __cplusplus
extern "C" {
#endif


#define BT_HFG_AUDIO_8K_SAMPLE_RATE      8000
#define BT_HFG_AUDIO_16K_SAMPLE_RATE     16000


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgAudioInit
 *
 *  DESCRIPTION
 *        Init HFG audio module.
 *
 *  PARAMETERS
 *
 *  RETURN
 *        0: success, other: failure
 *----------------------------------------------------------------------------*/

int ConnxHfgAudioInit(void);


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgAudioDeinit
 *
 *  DESCRIPTION
 *        Deinit HFG audio module.
 *
 *  PARAMETERS
 *
 *  RETURN
 *
 *----------------------------------------------------------------------------*/

void ConnxHfgAudioDeinit(void);


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgAudioStart
 *
 *  DESCRIPTION
 *        Start HFG audio.
 *
 *  PARAMETERS
 *        sample_rate:   HFG audio sample rate(8K or 16K)
 *
 *  RETURN
 *        result code. 0 for success, other for failure.
 *----------------------------------------------------------------------------*/

int ConnxHfgAudioStart(int sample_rate);


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgAudioStop
 *
 *  DESCRIPTION
 *        Stop HFG audio.
 *
 *  PARAMETERS
 *
 *  RETURN
 *        result code. 0 for success, other for failure.
 *----------------------------------------------------------------------------*/

int ConnxHfgAudioStop(void);


#ifdef __cplusplus
}
#endif

#endif  /* _CONNX_HFG_AUDIO_H_ */