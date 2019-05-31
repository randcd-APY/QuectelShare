/****************************************************************************

Copyright (c) 2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"

#include "csr_bt_av_app_lib.h"
#include "csr_pmem.h"

/* --------------------------------------------------------------------
   Name
       CSR_BT_AV_APP_SUSPEND_REQ

   Description
       Ask the AV task to suspend the stream

 * -------------------------------------------------------------------- */
CsrBtAvAppSuspendReq *CsrBtAvAppSuspendReq_struct(CsrSchedQid pHandle)
{
    CsrBtAvAppSuspendReq *prim;

    prim = (CsrBtAvAppSuspendReq *) pnew(CsrBtAvAppSuspendReq);
    prim->type = CSR_BT_AV_APP_SUSPEND_REQ;
    prim->pHandle = pHandle;

    return prim;
}

/* --------------------------------------------------------------------
   Name
       CSR_BT_AV_APP_START_REQ

   Description
       Ask the AV task to start the stream

 * -------------------------------------------------------------------- */
CsrBtAvAppStartReq *CsrBtAvAppStartReq_struct(CsrSchedQid pHandle)
{
    CsrBtAvAppStartReq *prim;

    prim = (CsrBtAvAppStartReq *) pnew(CsrBtAvAppStartReq);
    prim->type = CSR_BT_AV_APP_START_REQ;
    prim->pHandle = pHandle;

    return prim;
}

