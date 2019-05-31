#ifndef CSR_TM_BLUECORE_LIB_H__
#define CSR_TM_BLUECORE_LIB_H__
/*****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/


#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_sched.h"
#include "csr_result.h"
#include "csr_tm_bluecore_prim.h"
#include "csr_msg_transport.h"
#ifdef CSR_BTM_TASK
#include "csr_btm_task.h"
#define CSR_TM_BLUECORE_IFACEQUEUE CSR_BTM_IFACEQUEUE
#else
#include "csr_tm_bluecore_task.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

void CsrTmBluecoreFreeUpstreamMessageContents(CsrUint16 eventClass, void *message);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTmBlueCoreActivateTransportReqSend
 *
 *  DESCRIPTION
 *      Request to activate a transport
 *
 *  PARAMETERS
 *      phandle : The identity of the calling process.
 *----------------------------------------------------------------------------*/
CsrTmBluecoreActivateTransportReq *CsrTmBlueCoreActivateTransportReq_struct(CsrSchedQid phandle);
#define CsrTmBlueCoreActivateTransportReqSend(_ph){ \
        CsrTmBluecoreActivateTransportReq *msg__; \
        msg__ = CsrTmBlueCoreActivateTransportReq_struct(_ph); \
        CsrMsgTransport(CSR_TM_BLUECORE_IFACEQUEUE, CSR_TM_BLUECORE_PRIM, msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTmBlueCoreActivateTransportReqSend
 *
 *  DESCRIPTION
 *      Request to deactivate a transport
 *
 *  PARAMETERS
 *      phandle : The identity of the calling process.
 *----------------------------------------------------------------------------*/
CsrTmBluecoreDeactivateTransportReq *CsrTmBlueCoreDeactivateTransportReq_struct(CsrSchedQid phandle);
#define CsrTmBlueCoreDeactivateTransportReqSend(_ph){ \
        CsrTmBluecoreDeactivateTransportReq *msg__; \
        msg__ = CsrTmBlueCoreDeactivateTransportReq_struct(_ph); \
        CsrMsgTransport(CSR_TM_BLUECORE_IFACEQUEUE, CSR_TM_BLUECORE_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTmBlueCoreCmStatusSubscribeReqSend
 *
 *  DESCRIPTION
 *      Request to subscribe for chip manager status
 *
 *  PARAMETERS
 *      phandle : The identity of the calling process.
 *----------------------------------------------------------------------------*/
CsrTmBluecoreCmStatusSubscribeReq *CsrTmBlueCoreCmStatusSubscribeReq_struct(CsrSchedQid phandle);
#define CsrTmBlueCoreCmStatusSubscribeReqSend(_ph){            \
        CsrTmBluecoreCmStatusSubscribeReq *msg__; \
        msg__ = CsrTmBlueCoreCmStatusSubscribeReq_struct(_ph); \
        CsrMsgTransport(CSR_TM_BLUECORE_IFACEQUEUE, CSR_TM_BLUECORE_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTmBlueCoreCmStatusUnsubscribeReqSend
 *
 *  DESCRIPTION
 *      Request to unsubscribe for chip manager status
 *
 *  PARAMETERS
 *      phandle : The identity of the calling process.
 *----------------------------------------------------------------------------*/
CsrTmBluecoreCmStatusUnsubscribeReq *CsrTmBlueCoreCmStatusUnsubscribeReq_struct(CsrSchedQid phandle);
#define CsrTmBlueCoreCmStatusUnsubscribeReqSend(_ph){            \
        CsrTmBluecoreCmStatusUnsubscribeReq *msg__; \
        msg__ = CsrTmBlueCoreCmStatusUnsubscribeReq_struct(_ph); \
        CsrMsgTransport(CSR_TM_BLUECORE_IFACEQUEUE, CSR_TM_BLUECORE_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTmBluecoreCmPingIntervalSetReq
 *
 *  DESCRIPTION
 *      Request to set the chip manager ping interval
 *
 *  PARAMETERS
 *      phandle : The identity of the calling process.
 *      interval: The ping interval in micoseconds - if 0 ping is turned off.
 *----------------------------------------------------------------------------*/
CsrTmBluecoreCmPingIntervalSetReq *CsrTmBlueCoreCmPingIntervalSetReq_struct(CsrSchedQid phandle, CsrTime interval);
#define CsrTmBlueCoreCmPingIntervalSetReq(_ph, _i){        \
        CsrTmBluecoreCmPingIntervalSetReq *msg__; \
        msg__ = CsrTmBlueCoreCmPingIntervalSetReq_struct(_ph, _i);                 \
        CsrMsgTransport(CSR_TM_BLUECORE_IFACEQUEUE, CSR_TM_BLUECORE_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTmBlueCoreCmReplayRegisterReqSend
 *
 *  DESCRIPTION
 *      Request to register a chip manager replayer
 *
 *  PARAMETERS
 *      phandle : The identity of the calling process.
 *----------------------------------------------------------------------------*/
CsrTmBluecoreCmReplayRegisterReq *CsrTmBlueCoreCmReplayRegisterReq_struct(CsrSchedQid phandle);
#define CsrTmBlueCoreCmReplayRegisterReqSend(_ph){            \
        CsrTmBluecoreCmReplayRegisterReq *msg__; \
        msg__ = CsrTmBlueCoreCmReplayRegisterReq_struct(_ph); \
        CsrMsgTransport(CSR_TM_BLUECORE_IFACEQUEUE, CSR_TM_BLUECORE_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTmBlueCoreCmReplayStartResSend
 *
 *  DESCRIPTION
 *      Response to chip manager replay start indication
 *
 *  PARAMETERS
 *      phandle : The identity of the calling process.
 *----------------------------------------------------------------------------*/
CsrTmBluecoreCmReplayStartRes *CsrTmBlueCoreCmReplayStartRes_struct(CsrSchedQid phandle, CsrResult result);
#define CsrTmBlueCoreCmReplayStartResSend(_ph, _res){       \
        CsrTmBluecoreCmReplayStartRes *msg__; \
        msg__ = CsrTmBlueCoreCmReplayStartRes_struct(_ph, _res);                     \
        CsrMsgTransport(CSR_TM_BLUECORE_IFACEQUEUE, CSR_TM_BLUECORE_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTmBlueCoreCmReplayErrorReqSend
 *
 *  DESCRIPTION
 *      Signal to chip manager that replay of a HCI cmd failed.
 *
 *  NOTE
 *      This request has no corresponding cfm. It is forwarded
 *      as a CsrTmBlueCoreCmStatusReplayErrorInd to all chip manager
 *      status subscribers.
 *
 *  PARAMETERS
 *      hciOpcode: The opcode for the failing HCI cmd
 *----------------------------------------------------------------------------*/
CsrTmBluecoreCmReplayErrorReq *CsrTmBlueCoreCmReplayErrorReq_struct(CsrUint16 hciOpcode);
#define CsrTmBlueCoreCmReplayErrorReqSend(_opc){       \
        CsrTmBluecoreCmReplayErrorReq *msg__; \
        msg__ = CsrTmBlueCoreCmReplayErrorReq_struct(_opc);                     \
        CsrMsgTransport(CSR_TM_BLUECORE_IFACEQUEUE, CSR_TM_BLUECORE_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTmBlueCoreRegisterReqSend
 *
 *  DESCRIPTION
 *      Register a task as a delegate which will receive (and must respond to)
 *      indications when the transport is activated and deactivated.
 *
 *  PARAMETERS
 *      phandle : The identity of the calling process.
 *----------------------------------------------------------------------------*/
CsrTmBluecoreRegisterReq *CsrTmBlueCoreRegisterReq_struct(CsrSchedQid phandle);
#define CsrTmBlueCoreRegisterReqSend(_phandle){ \
        CsrTmBluecoreRegisterReq *msg__; \
        msg__ = CsrTmBlueCoreRegisterReq_struct(_phandle); \
        CsrMsgTransport(CSR_TM_BLUECORE_IFACEQUEUE, CSR_TM_BLUECORE_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTmBlueCoreUnregisterReqSend
 *
 *  DESCRIPTION
 *      Unregister a task that was previously registered as a delegate.
 *
 *  PARAMETERS
 *      phandle : The identity of the calling process.
 *----------------------------------------------------------------------------*/
CsrTmBluecoreUnregisterReq *CsrTmBlueCoreUnregisterReq_struct(CsrSchedQid phandle);
#define CsrTmBlueCoreUnregisterReqSend(_phandle){ \
        CsrTmBluecoreUnregisterReq *msg__; \
        msg__ = CsrTmBlueCoreUnregisterReq_struct(_phandle); \
        CsrMsgTransport(CSR_TM_BLUECORE_IFACEQUEUE, CSR_TM_BLUECORE_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTmBluecoreActivateTransportResSend
 *
 *  DESCRIPTION
 *      Unregister a task that was previously registered as a delegate.
 *
 *  PARAMETERS
 *      phandle : The identity of the calling process.
 *----------------------------------------------------------------------------*/
CsrTmBluecoreActivateTransportRes *CsrTmBlueCoreActivateTransportRes_struct(CsrSchedQid phandle);
#define CsrTmBluecoreActivateTransportResSend(_phandle){ \
        CsrTmBluecoreActivateTransportRes *msg__; \
        msg__ = CsrTmBlueCoreActivateTransportRes_struct(_phandle); \
        CsrMsgTransport(CSR_TM_BLUECORE_IFACEQUEUE, CSR_TM_BLUECORE_PRIM, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTmBluecoreDeactivateTransportResSend
 *
 *  DESCRIPTION
 *      Unregister a task that was previously registered as a delegate.
 *
 *  PARAMETERS
 *      phandle : The identity of the calling process.
 *----------------------------------------------------------------------------*/
CsrTmBluecoreDeactivateTransportRes *CsrTmBlueCoreDeactivateTransportRes_struct(CsrSchedQid phandle);
#define CsrTmBluecoreDeactivateTransportResSend(_phandle){ \
        CsrTmBluecoreDeactivateTransportRes *msg__; \
        msg__ = CsrTmBlueCoreDeactivateTransportRes_struct(_phandle); \
        CsrMsgTransport(CSR_TM_BLUECORE_IFACEQUEUE, CSR_TM_BLUECORE_PRIM, msg__);}

#ifdef __cplusplus
}
#endif

#endif
