#ifndef CSR_BT_HF_APP_LIB_H__
#define CSR_BT_HF_APP_LIB_H__

/****************************************************************************

Copyright (c) 2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_synergy.h"
#include "csr_pmem.h"
#include "csr_msg_transport.h"
#include "csr_bt_hf_app_task.h"
#include "csr_bt_hf_app_prim.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define CsrBtHfAppActivateReqSend(_pHandle, _maxHFConns, _maxHSConns, _maxSimulConns)\
{\
    CsrBtHfAppActivateReq* msg = (CsrBtHfAppActivateReq*)CsrPmemAlloc(sizeof(CsrBtHfAppActivateReq));\
    msg->type = CSR_BT_HF_APP_ACTIVATE_REQ;\
    msg->pHandle = _pHandle;\
    msg->maxHFConns = _maxHFConns;\
    msg->maxHSConns = _maxHSConns;\
    msg->maxSimulConns = _maxSimulConns;\
    CsrMsgTransport(CSR_BT_HF_APP_IFACEQUEUE, CSR_BT_HF_APP_PRIM, msg);\
}

#define CsrBtHfAppConnectReqSend(_pHandle, _deviceAddr)\
{\
    CsrBtHfAppConnectReq* msg = (CsrBtHfAppConnectReq*)CsrPmemAlloc(sizeof(CsrBtHfAppConnectReq));\
    msg->type = CSR_BT_HF_APP_CONNECT_REQ;\
    msg->pHandle = _pHandle;\
    msg->deviceAddr = _deviceAddr;\
    CsrMsgTransport(CSR_BT_HF_APP_IFACEQUEUE, CSR_BT_HF_APP_PRIM, msg);\
}

#define CsrBtHfAppDisconnectReqSend(_pHandle)\
{\
    CsrBtHfAppDisconnectReq* msg = (CsrBtHfAppDisconnectReq*)CsrPmemAlloc(sizeof(CsrBtHfAppDisconnectReq));\
    msg->type = CSR_BT_HF_APP_DISCONNECT_REQ;\
    msg->pHandle = _pHandle;\
    CsrMsgTransport(CSR_BT_HF_APP_IFACEQUEUE, CSR_BT_HF_APP_PRIM, msg);\
}

#define CsrBtHfAppDialReqSend(_pHandle, _dialType, _theNumber)\
{\
    CsrBtHfAppDialReq* msg = (CsrBtHfAppDialReq*)CsrPmemAlloc(sizeof(CsrBtHfAppDialReq));\
    msg->type = CSR_BT_HF_APP_DIAL_REQ;\
    msg->dialType = _dialType;\
    msg->theNumber = _theNumber;\
    msg->pHandle = _pHandle;\
    CsrMsgTransport(CSR_BT_HF_APP_IFACEQUEUE, CSR_BT_HF_APP_PRIM, msg);\
}

#define CsrBtHfAppCallEndReqSend(_pHandle, _acceptRejReq)\
{\
    CsrBtHfAppCallAnsRejReq* msg = (CsrBtHfAppCallAnsRejReq*)CsrPmemAlloc(sizeof(CsrBtHfAppCallAnsRejReq));\
    msg->type = CSR_BT_HF_APP_CALL_ANS_REJ_REQ;\
    msg->pHandle = _pHandle;\
    msg->acceptRejReq = _acceptRejReq;\
    CsrMsgTransport(CSR_BT_HF_APP_IFACEQUEUE, CSR_BT_HF_APP_PRIM, msg);\
}

#define CsrBtHfAppAudioConnectReqSend(_pHandle)\
{\
    CsrBtHfAppAudioConnectReq* msg = (CsrBtHfAppAudioConnectReq*)CsrPmemAlloc(sizeof(CsrBtHfAppAudioConnectReq));\
    msg->type = CSR_BT_HF_APP_AUDIO_CONNECT_REQ;\
    msg->pHandle = _pHandle;\
    CsrMsgTransport(CSR_BT_HF_APP_IFACEQUEUE, CSR_BT_HF_APP_PRIM, msg);\
}

#define CsrBtHfAppAudioDisconnectReqSend(_pHandle)\
{\
    CsrBtHfAppAudioDisconnectReq* msg = (CsrBtHfAppAudioDisconnectReq*)CsrPmemAlloc(sizeof(CsrBtHfAppAudioDisconnectReq));\
    msg->type = CSR_BT_HF_APP_AUDIO_DISCONNECT_REQ;\
    msg->pHandle = _pHandle;\
    CsrMsgTransport(CSR_BT_HF_APP_IFACEQUEUE, CSR_BT_HF_APP_PRIM, msg);\
}

#ifdef __cplusplus
}
#endif
#endif


