#ifndef CSR_BT_AV_APP_TASK_H__
#define CSR_BT_AV_APP_TASK_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_app_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSR_BT_AV_APP_IFACEQUEUE_PRIM CSR_BT_AV_APP_PRIM

/* Queue definition for CSR_BT_AV */
extern CsrUint16 CSR_BT_AV_APP_IFACEQUEUE;

/* Task definition for CSR_BT_AV */
void CsrBtAvAppInit(void ** gash);
void CsrBtAvAppDeinit(void ** gash);
void CsrBtAvAppHandler(void ** gash);
void CsrBtAvAppUsage(void);

#ifdef __cplusplus
}
#endif


#endif /* CSR_BT_AV_APP_TASK_H__ */
