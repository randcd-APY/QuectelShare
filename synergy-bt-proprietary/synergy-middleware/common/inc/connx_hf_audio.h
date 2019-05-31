/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifndef _CONNX_HF_AUDIO_H_
#define _CONNX_HF_AUDIO_H_

#ifdef __cplusplus
extern "C" {
#endif


#define BT_HF_AUDIO_8K_SAMPLE_RATE      8000
#define BT_HF_AUDIO_16K_SAMPLE_RATE     16000


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfAudioInit
 *
 *  DESCRIPTION
 *        Init HF audio module.
 *
 *  PARAMETERS
 *
 *  RETURN
 *        0: success, other: failure
 *----------------------------------------------------------------------------*/

int ConnxHfAudioInit(void);


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfAudioDeinit
 *
 *  DESCRIPTION
 *        Deinit HF audio module.
 *
 *  PARAMETERS
 *
 *  RETURN
 *
 *----------------------------------------------------------------------------*/

void ConnxHfAudioDeinit(void);


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfAudioStart
 *
 *  DESCRIPTION
 *        Start HF audio.
 *
 *  PARAMETERS
 *        sample_rate:   HF audio sample rate(8K or 16K)
 *
 *  RETURN
 *        result code. 0 for success, other for failure.
 *----------------------------------------------------------------------------*/

int ConnxHfAudioStart(int sample_rate);


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfAudioStop
 *
 *  DESCRIPTION
 *        Stop HF audio.
 *
 *  PARAMETERS
 *
 *  RETURN
 *        result code. 0 for success, other for failure.
 *----------------------------------------------------------------------------*/

int ConnxHfAudioStop(void);


#ifdef __cplusplus
}
#endif

#endif  /* _CONNX_HF_AUDIO_H_ */