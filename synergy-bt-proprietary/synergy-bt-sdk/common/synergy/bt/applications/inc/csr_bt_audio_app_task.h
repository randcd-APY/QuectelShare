#ifndef CSR_BT_AUDIO_APP_TASK_H__
#define CSR_BT_AUDIO_APP_TASK_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
*****************************************************************************/

#include "csr_types.h"
#include "csr_bt_app_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSR_BT_AUDIO_APP_IFACEQUEUE_PRIM CSR_BT_AUDIO_APP_PRIM

/* Queue definition */
extern CsrUint16 CSR_BT_AUDIO_APP_IFACEQUEUE;

/* Task definitions */
void CsrBtAudioAppInit(void **blank);
void CsrBtAudioAppDeinit(void **blank);
void CsrBtAudioAppHandler(void **blank);

#ifdef __cplusplus
}
#endif
#endif /* CSR_BT_AUDIO_APP_TASK_H__ */
