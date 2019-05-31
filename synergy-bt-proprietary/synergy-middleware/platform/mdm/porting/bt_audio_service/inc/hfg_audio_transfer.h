#ifndef _HFG_AUDIO_TRANSFER_H_
#define _HFG_AUDIO_TRANSFER_H_

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

void init_hfg_audio();
void deinit_hfg_audio();
int start_hfg_audio(unsigned int sampleRate);
void stop_hfg_audio();

#ifdef __cplusplus
}
#endif

#endif  /* _HFG_AUDIO_TRANSFER_H_ */

