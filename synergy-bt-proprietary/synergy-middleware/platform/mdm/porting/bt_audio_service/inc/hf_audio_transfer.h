#ifndef _HF_AUDIO_TRANSFER_H_
#define _HF_AUDIO_TRANSFER_H_

/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/
#include "connx_common_def.h"
#include "bt_audio_hw.h"

#ifdef __cplusplus
extern "C" {
#endif

int start_hf_audio(unsigned int sampleRate);
void stop_hf_audio();
void init_hf_audio();
void deinit_hf_audio();

#ifdef __cplusplus
}
#endif

#endif  /* _HF_AUDIO_TRANSFER_H_ */

