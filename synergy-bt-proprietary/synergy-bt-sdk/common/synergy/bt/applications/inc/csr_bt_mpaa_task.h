#ifndef CSR_BT_MPAA_TASK_H__
#define CSR_BT_MPAA_TASK_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_app_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSR_BT_MPAA_IFACEQUEUE_PRIM CSR_BT_MPAA_APP_PRIM

/* Queue definition for CSR_BT_MPAA */
extern CsrUint16 CSR_BT_MPAA_IFACEQUEUE;

/* Task definition for CSR_BT_MPAA */
void CsrBtMpaaInit(void **blank);
void CsrBtMpaaDeinit(void **blank);
void CsrBtMpaaHandler(void **blank);

#ifdef __cplusplus
}
#endif
#endif /* CSR_BT_MPAA_TASK_H__ */
