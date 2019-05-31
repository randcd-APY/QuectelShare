#ifndef CSR_BT_DG_LIB_H__
#define CSR_BT_DG_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2001 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "csr_bt_dg_prim.h"
#include "csr_pmem.h"
#include "csr_util.h"
#include "csr_bt_tasks.h"

/* Common put_message function to reduce code size */
void CsrBtDgMsgTransport(CsrSchedQid qid, void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtDgDataReqSend
 *
 *  DESCRIPTION
 *      Send data.
 *
 *  PARAMETERS
 *      btConnId:        local Bluetooth Connection id
 *      payloadLength:   number of data bytes in data area
 *      *payload:        pointer to allocated data
 *
 *----------------------------------------------------------------------------*/
#define CsrBtDgDataReqSend(_btConnId, _payloadLength, _payload) {       \
        CsrBtDgDataReq *msg = (CsrBtDgDataReq *)CsrPmemAlloc(sizeof(CsrBtDgDataReq)); \
        msg->type           = CSR_BT_DG_DATA_REQ;                       \
        msg->btConnId       = _btConnId;                                \
        msg->payloadLength  = _payloadLength;                           \
        msg->payload        = _payload;                                 \
        CsrBtDgMsgTransport(CSR_BT_DG_IFACEQUEUE, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtDgDisconnectReqSend
 *
 *  DESCRIPTION
 *      Release a connection between the local and remote device.
 *
 *  PARAMETERS
 *      btConnId:        local Bluetooth Connection id
 *
 *----------------------------------------------------------------------------*/
#define CsrBtDgDisconnectReqSend(_btConnId) {                           \
        CsrBtDgDisconnectReq *msg = (CsrBtDgDisconnectReq *)CsrPmemAlloc(sizeof(CsrBtDgDisconnectReq)); \
        msg->type           = CSR_BT_DG_DISCONNECT_REQ;                 \
        msg->btConnId       = _btConnId;                                \
        CsrBtDgMsgTransport(CSR_BT_DG_IFACEQUEUE, msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      DGControlReqSend
 *
 *  DESCRIPTION
 *      Send and receive modem status information.
 *
 *  PARAMETERS
 *      btConnId:        local Bluetooth Connection id
 *      modemstatus:     modemstatus
 *
 *----------------------------------------------------------------------------*/
#define CsrBtDgControlReqSend(_btConnId, _modemStatus, _breakSignal) {  \
        CsrBtDgControlReq *msg = (CsrBtDgControlReq *)CsrPmemAlloc(sizeof(CsrBtDgControlReq)); \
        msg->type          = CSR_BT_DG_CONTROL_REQ;                     \
        msg->btConnId      = _btConnId;                                 \
        msg->modemstatus   = _modemStatus;                              \
        msg->break_signal  = _breakSignal;                              \
        CsrBtDgMsgTransport(CSR_BT_DG_IFACEQUEUE, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtDgPortnegReqSend
 *
 *  DESCRIPTION
 *      Send port negotiation request to obtain new port settings
 *
 *  PARAMETERS
 *      dgQueueId:       queue id of this DG instance, see csr_bt_tasks.h for possible queues.
 *      btConnId:        local Bluetooth Connection id
 *      portPar:         port negotiation parameters
 *
 *----------------------------------------------------------------------------*/
#define CsrBtDgPortnegReqSend(dgQueueId,_btConnId, _portPar) {          \
        CsrBtDgPortnegReq *msg = (CsrBtDgPortnegReq*)CsrPmemAlloc(sizeof(CsrBtDgPortnegReq)); \
        msg->type          = CSR_BT_DG_PORTNEG_REQ;                     \
        msg->btConnId      = _btConnId;                                 \
        msg->portPar       = *((RFC_PORTNEG_VALUES_T *) _portPar);      \
        CsrBtDgMsgTransport(dgQueueId, msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtDgPortnegResSend
 *
 *  DESCRIPTION
 *      Send port negotiation response to the DG profile.
 *
 *  PARAMETERS
 *      btConnId:        local Bluetooth Connection id
 *      portPar:         accepted port parameters
 *
 *----------------------------------------------------------------------------*/
#define CsrBtDgPortnegResSend(_btConnId, _portVar) {                    \
        CsrBtDgPortnegRes *msg = (CsrBtDgPortnegRes *)CsrPmemAlloc(sizeof(CsrBtDgPortnegRes)); \
        msg->type          = CSR_BT_DG_PORTNEG_RES;                     \
        msg->btConnId      = _btConnId;                                 \
        msg->portPar       = *((RFC_PORTNEG_VALUES_T *) _portVar);      \
        CsrBtDgMsgTransport(CSR_BT_DG_IFACEQUEUE, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtDgActivateReqSend
 *
 *  DESCRIPTION
 *      This signal is used to activate a service and make it accessible from a
 *        remote device.
 *
 *  PARAMETERS
 *      phandle:        protocol handle
 *      lhandle:        reference for support of multiple instances
 *      timeout:        holds the pageScanTimeOut time

 *----------------------------------------------------------------------------*/
#define CsrBtDgActivateReqSend(_phandle, _timeout) {                    \
        CsrBtDgActivateReq *msg = (CsrBtDgActivateReq *)CsrPmemAlloc(sizeof(CsrBtDgActivateReq)); \
        msg->type           = CSR_BT_DG_ACTIVATE_REQ;                   \
        msg->phandle        = _phandle;                                 \
        msg->timeout        = _timeout;                                 \
        msg->role           = CSR_BT_DCE;                               \
        CsrBtDgMsgTransport(CSR_BT_DG_IFACEQUEUE, msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtDgDeactivateReqSend
 *
 *  DESCRIPTION
 *      Deactivate a service that has been activated previously.
 *
 *  PARAMETERS
 *        n/a
 *
 *----------------------------------------------------------------------------*/
#define CsrBtDgDeactivateReqSend() {                                    \
        CsrBtDgDeactivateReq *msg = (CsrBtDgDeactivateReq *)CsrPmemAlloc(sizeof(CsrBtDgDeactivateReq)); \
        msg->type          = CSR_BT_DG_DEACTIVATE_REQ;                  \
        CsrBtDgMsgTransport(CSR_BT_DG_IFACEQUEUE, msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtDgDataResSend
 *
 *  DESCRIPTION
 *      Give response to received data.
 *
 *  PARAMETERS
 *      btConnId:        local Bluetooth Connection id
 *
 *----------------------------------------------------------------------------*/
#define CsrBtDgDataResSend(_btConnId) {                                 \
        CsrBtDgDataRes *msg = (CsrBtDgDataRes *)CsrPmemAlloc(sizeof(CsrBtDgDataRes)); \
        msg->type          = CSR_BT_DG_DATA_RES;                        \
        msg->btConnId      = _btConnId;                                 \
        CsrBtDgMsgTransport(CSR_BT_DG_IFACEQUEUE, msg);}


/*---------------------------------------------------------------------------*
 *  NAME
 *      CsrBtDgRegisterDataPathHandleReqSend
 *
 *  DESCRIPTION
 *      Redirect all data signals to the given application handle
 *
 *  PARAMETERS
 *      dgQueueId:         queue id of this DG instance, see csr_bt_tasks.h for possible queues.
 *      dataHandle:        data application handle
 *
 *---------------------------------------------------------------------------*/
#define CsrBtDgRegisterDataPathHandleReqSend(_dgQueueId, _dataHandle) { \
        CsrBtDgRegisterDataPathHandleReq *msg = (CsrBtDgRegisterDataPathHandleReq*)CsrPmemAlloc(sizeof(CsrBtDgRegisterDataPathHandleReq)); \
        msg->type                          = CSR_BT_DG_REGISTER_DATA_PATH_HANDLE_REQ; \
        msg->dataAppHandle                 = _dataHandle;               \
        CsrBtDgMsgTransport(_dgQueueId, msg);}


/*---------------------------------------------------------------------------*
 *  NAME
 *      CsrBtDgDataPathStatusReqSend
 *
 *  DESCRIPTION
 *      Request DG to notify control application of data path status changes
 *
 *  PARAMETERS
 *      dgQueueId:        queue id of this dg instance, see csr_bt_tasks.h for possible queues.
 *      status:           new data application status
 *
 *---------------------------------------------------------------------------*/
#define CsrBtDgDataPathStatusReqSend(_dgQueueId, _status) {             \
        CsrBtDgDataPathStatusReq *msg = (CsrBtDgDataPathStatusReq*)CsrPmemAlloc(sizeof(CsrBtDgDataPathStatusReq)); \
        msg->type                 = CSR_BT_DG_DATA_PATH_STATUS_REQ;     \
        msg->dgInstanceQueue      = _dgQueueId;                         \
        msg->status               = _status;                            \
        CsrBtDgMsgTransport(_dgQueueId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtDgSecurityInReqSend
 *
 *  DESCRIPTION
 *      Set the default security settings for new incoming connections
 *
 *  PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtDgSecurityInReqSend(_appHandle, _secLevel) {               \
        CsrBtDgSecurityInReq *msg = (CsrBtDgSecurityInReq*)CsrPmemAlloc(sizeof(CsrBtDgSecurityInReq)); \
        msg->type      = CSR_BT_DG_SECURITY_IN_REQ;                     \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel  = _secLevel;                                     \
        CsrBtDgMsgTransport(CSR_BT_DG_IFACEQUEUE, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtDgFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT DG
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_DG_PRIM,
 *      msg:          The message received from Synergy BT DG
 *----------------------------------------------------------------------------*/
void CsrBtDgFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);

#ifdef __cplusplus
}
#endif

#endif

