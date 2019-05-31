#ifndef CSR_BT_PAN_APP_TASK_H__
#define CSR_BT_PAN_APP_TASK_H__

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

#define CSR_BT_PAN_APP_IFACEQUEUE_PRIM CSR_BT_PAN_APP_PRIM
#define CSR_BT_PAN_APP_EXTRA_IFACEQUEUE_PRIM CSR_BT_PAN_APP_PRIM

extern CsrUint16 CSR_BT_PAN_APP_IFACEQUEUE;

/* Task definition for CSR_APP */
void CsrBtPanAppInit(void ** gash);
void CsrBtPanAppDeinit(void ** gash);
void CsrBtPanAppHandler(void ** gash);

#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_PAN_APP_TASK_H__ */
