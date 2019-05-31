#ifndef CSR_BT_AV_APP_LIB_H__
#define CSR_BT_AV_APP_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_msg_transport.h"
#include "csr_bt_av_app_prim.h"
#include "csr_bt_av_app_task.h"
#include "csr_pmem.h"


#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------
   Name
       CSR_BT_AV_APP_SUSPEND_REQ

   Description
       Ask the AV task to suspend the stream

 * -------------------------------------------------------------------- */
CsrBtAvAppSuspendReq *CsrBtAvAppSuspendReq_struct(CsrSchedQid pHandle);
#define CsrBtAvAppSuspendReqSend(_pHandle){\
    CsrBtAvAppSuspendReq *msg;\
    msg = CsrBtAvAppSuspendReq_struct(_pHandle);\
    CsrMsgTransport(CSR_BT_AV_APP_IFACEQUEUE,CSR_BT_AV_APP_PRIM,msg);\
}

/* --------------------------------------------------------------------
   Name
       CSR_BT_AV_APP_START_REQ

   Description
       Ask the AV task to start the stream

 * -------------------------------------------------------------------- */
CsrBtAvAppStartReq *CsrBtAvAppStartReq_struct(CsrSchedQid pHandle);
#define CsrBtAvAppStartReqSend(_pHandle){\
    CsrBtAvAppStartReq *msg;\
    msg = CsrBtAvAppStartReq_struct(_pHandle);\
    CsrMsgTransport(CSR_BT_AV_APP_IFACEQUEUE,CSR_BT_AV_APP_PRIM,msg);\
}

#define CsrBtAvAppActivateReqSend(_appQid, _avAppRole, _supportedCodec)\
{\
    CsrBtAvAppActivateReq* msg = (CsrBtAvAppActivateReq*)CsrPmemAlloc(sizeof(CsrBtAvAppActivateReq));\
    msg->type = CSR_BT_AV_APP_ACTIVATE_REQ;\
    msg->pHandle = _appQid;\
    msg->avAppRole = _avAppRole;\
    msg->supportedCodec = _supportedCodec;\
    CsrMsgTransport(CSR_BT_AV_APP_IFACEQUEUE, CSR_BT_AV_APP_PRIM, msg);\
}

#define CsrBtAvAppConnectReqSend(_appQid, _deviceAddr)\
{\
    CsrBtAvAppConnectReq* msg = (CsrBtAvAppConnectReq*)CsrPmemAlloc(sizeof(CsrBtAvAppConnectReq));\
    msg->type = CSR_BT_AV_APP_CONNECT_REQ;\
    msg->pHandle = _appQid;\
    msg->deviceAddr = _deviceAddr;\
    CsrMsgTransport(CSR_BT_AV_APP_IFACEQUEUE, CSR_BT_AV_APP_PRIM, msg);\
}


#define CsrBtAvAppDisconnectReqSend(_appQid)\
{\
    CsrBtAvAppDisconnectReq* msg = (CsrBtAvAppDisconnectReq*)CsrPmemAlloc(sizeof(CsrBtAvAppDisconnectReq));\
    msg->type = CSR_BT_AV_APP_DISCONNECT_REQ;\
    msg->pHandle = _appQid;\
    CsrMsgTransport(CSR_BT_AV_APP_IFACEQUEUE, CSR_BT_AV_APP_PRIM, msg);\
}

#define CsrBtAvAppPlayReqSend(_appQid)\
{\
    CsrBtAvAppPlayReq* msg = (CsrBtAvAppPlayReq*)CsrPmemAlloc(sizeof(CsrBtAvAppPlayReq));\
    msg->type = CSR_BT_AV_APP_PLAY_REQ;\
    msg->pHandle = _appQid;\
    CsrMsgTransport(CSR_BT_AV_APP_IFACEQUEUE, CSR_BT_AV_APP_PRIM, msg);\
}


#define CsrBtAvAppPauseReqSend(_appQid)\
{\
    CsrBtAvAppPauseReq* msg = (CsrBtAvAppPauseReq*)CsrPmemAlloc(sizeof(CsrBtAvAppPauseReq));\
    msg->type = CSR_BT_AV_APP_PAUSE_REQ;\
    msg->pHandle = _appQid;\
    CsrMsgTransport(CSR_BT_AV_APP_IFACEQUEUE, CSR_BT_AV_APP_PRIM, msg);\
}

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_AV_APP_LIB_H__ */
