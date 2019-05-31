#ifndef _BT_AUDIO_SERVICE_H_
#define _BT_AUDIO_SERVICE_H_

/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/
#include "connx_common_def.h"

#include "bt_audio_service_common.h"
#include "bt_audio_hw.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BRI_GET_SIZE(ri)     ((ri)->size)

typedef struct
{
    uint32_t    size;  /* Total size of BT audio service register information. */
} BasRegisterInfo;

bool BtAudioServiceOpen(BasRegisterInfo *registerInfo);
void BtAudioServiceClose(void);

#ifdef __cplusplus
}
#endif

#endif  /* _BT_AUDIO_SERVICE_H_ */

