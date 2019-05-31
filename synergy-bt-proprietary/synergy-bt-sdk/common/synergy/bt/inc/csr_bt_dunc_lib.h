#ifndef CSR_BT_DUNC_LIB_H__
#define CSR_BT_DUNC_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_dunc_prim.h"
#include "csr_pmem.h"
#include "csr_util.h"
#include "csr_bt_tasks.h"

#ifdef  __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtDuncMsgTransport(CsrSchedQid phandle, void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtDuncConnectReqSend
 *
 *  DESCRIPTION
 *      Creates a connection to a Dialup Networking Profile Gateway to a specified
 *      Bluetooth device address. A DUNC_CONNECT_IND will be sent to the initiator
 *      as a response to this message, indicating if the connection could be established.
 *
 *  PARAMETERS
 *      duncInstanceId      For multiple DUNC instance purposes. Specifies the ID
 *                          of the DUNC instance to be used by the application.
 *      ctrlAppHandle       The handle of the application to forward the Bluetooth
 *                          control messages to.
 *      bdAddr              A pointer to the Bluetooth address to initiate a connection
 *                          to.
 *      lowPowerSupport     TRUE if the application wants the DUNC to support low power
 *                          (sniff) or FALSE if only active mode is wanted.
 *----------------------------------------------------------------------------*/
#define CsrBtDuncConnectReqSend(_duncInstanceId, _ctrlAppHandle, _bdAddr, _lowPowerSupport) { \
        CsrBtDuncConnectReq *msg__ = (CsrBtDuncConnectReq *) CsrPmemAlloc(sizeof(CsrBtDuncConnectReq)); \
        msg__->type              = CSR_BT_DUNC_CONNECT_REQ;             \
        msg__->ctrlHandle        = _ctrlAppHandle;                      \
        msg__->lowPowerSupport   = _lowPowerSupport;                    \
        msg__->bdAddr            = _bdAddr;                             \
        CsrBtDuncMsgTransport(_duncInstanceId, msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtDuncCancelConnectReqSend
 *
 *  DESCRIPTION
 *      Cancels a current request for a connection.
 *
 *  PARAMETERS
 *      duncInstanceId      For multiple DUNC instance purposes. Specifies the ID
 *                          of the DUNC instance to be used by the application.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtDuncCancelConnectReqSend(_duncInstanceId) {                \
        CsrBtDuncCancelConnectReq *msg__ = (CsrBtDuncCancelConnectReq *) CsrPmemAlloc(sizeof(CsrBtDuncCancelConnectReq)); \
        msg__->type                = CSR_BT_DUNC_CANCEL_CONNECT_REQ;    \
        CsrBtDuncMsgTransport(_duncInstanceId, msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      DuncDataReq
 *
 *  DESCRIPTION
 *      Sends data to the Bluetooth device connected to in CSR_BT_DUNC_CONNECT_REQ
 *
 *  PARAMETERS
 *      duncInstanceId      For multiple DUNC instance purposes. Specifies the ID
 *                          of the DUNC instance to be used by the application.
 *      data                Pointer to the data to be sent to the device. Is free'd
 *                          by the lower layers, and must therefore not be free'd by
 *                          the application.
 *      length              The length of the data.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtDuncDataReqSend(_duncInstanceId, _data, _length) {         \
        CsrBtDuncDataReq *msg__ = (CsrBtDuncDataReq *) CsrPmemAlloc(sizeof(CsrBtDuncDataReq)); \
        msg__->type             = CSR_BT_DUNC_DATA_REQ;                 \
        msg__->data             = _data;                                \
        msg__->dataLength       = _length;                              \
        CsrBtDuncMsgTransport(_duncInstanceId, msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      DuncDataRes
 *
 *  DESCRIPTION
 *      Is used to indicate to the profile that the latest received data
 *      (CSR_BT_DUNC_DATA_IND) is processed, and the application is ready for reception
 *      of another CSR_BT_DUNC_DATA_IND.
 *
 *  PARAMETERS
 *      duncInstanceId        For multiple DUNC instance purposes. Specifies the ID
 *                            of the DUNC instance to be used by the application.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtDuncDataResSend(_duncInstanceId) {                         \
        CsrBtDuncDataRes *msg__ = (CsrBtDuncDataRes *) CsrPmemAlloc(sizeof(CsrBtDuncDataRes)); \
        msg__->type             = CSR_BT_DUNC_DATA_RES;                 \
        CsrBtDuncMsgTransport(_duncInstanceId, msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      DuncControlReq
 *
 *  DESCRIPTION
 *      Send and receive modem status information.
 *
 *  PARAMETERS
 *      duncInstanceId       For multiple DUNC instance purposes. Specifies the ID
 *                           of the DUNC instance to be used by the application.
 *      modemStatus          Status of the modem
 *      breakSignal          Breaksignal
 *
 *----------------------------------------------------------------------------*/
#define CsrBtDuncControlReqSend(_duncInstanceId, _modemStatus, _breakSignal) { \
        CsrBtDuncControlReq *msg__ = (CsrBtDuncControlReq *) CsrPmemAlloc(sizeof(CsrBtDuncControlReq)); \
        msg__->type                = CSR_BT_DUNC_CONTROL_REQ;           \
        msg__->modemStatus         = _modemStatus;                      \
        msg__->breakSignal         = _breakSignal;                      \
        CsrBtDuncMsgTransport(_duncInstanceId, msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      DuncPortnegRes
 *
 *  DESCRIPTION
 *      Give response to a received port negotiation indication.
 *
 *  PARAMETERS
 *      duncInstanceId        For multiple DUNC instance purposes. Specifies the ID
 *                            of the DUNC instance to be used by the application.
 *      portPar               Port negotiation parameters
 *
 *----------------------------------------------------------------------------*/
#define CsrBtDuncPortnegResSend(_duncInstanceId, _portPar) {            \
        CsrBtDuncPortnegRes *msg__ = (CsrBtDuncPortnegRes *) CsrPmemAlloc(sizeof(CsrBtDuncPortnegRes)); \
        msg__->type                = CSR_BT_DUNC_PORTNEG_RES;           \
        msg__->portPar             = *((RFC_PORTNEG_VALUES_T *) _portPar); \
        CsrBtDuncMsgTransport(_duncInstanceId, msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      DuncPortnegReq
 *
 *  DESCRIPTION
 *      Requests a port negotiation to the other Bluetooth device
 *
 *  PARAMETERS
 *      duncInstanceId        For multiple DUNC instance purposes. Specifies the ID
 *                            of the DUNC instance to be used by the application.
 *      portPar               Port negotiation parameters
 *
 *----------------------------------------------------------------------------*/
#define CsrBtDuncPortnegReqSend(_duncInstanceId, _portPar, _request) {  \
        CsrBtDuncPortnegReq *msg__ = (CsrBtDuncPortnegReq *) CsrPmemAlloc(sizeof(CsrBtDuncPortnegReq)); \
        msg__->type                = CSR_BT_DUNC_PORTNEG_REQ;           \
        msg__->portPar             = *((RFC_PORTNEG_VALUES_T *) _portPar); \
        msg__->request             = _request;                          \
        CsrBtDuncMsgTransport(_duncInstanceId, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      DuncDisconnectReq
 *
 *  DESCRIPTION
 *      Disconnects the Bluetooth connection established using CSR_BT_DUNC_CONNECT_REQ
 *
 *  PARAMETERS
 *      duncInstanceId      For multiple DUNC instance purposes. Specifies the ID
 *                          of the DUNC instance to be used by the application.
 *----------------------------------------------------------------------------*/
#define CsrBtDuncDisconnectReqSend(_duncInstanceId) {                   \
        CsrBtDuncDisconnectReq *msg__ = (CsrBtDuncDisconnectReq *) CsrPmemAlloc(sizeof(CsrBtDuncDisconnectReq)); \
        msg__->type                   = CSR_BT_DUNC_DISCONNECT_REQ;     \
        CsrBtDuncMsgTransport(_duncInstanceId, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      DuncRegisterDataPathHandleReq
 *
 *  DESCRIPTION
 *      Register a data path application to receive data from the DUNC profile
 *
 *  PARAMETERS
 *      duncInstanceId      For multiple DUNC instance purposes. Specifies the ID
 *                          of the DUNC instance to be used by the application
 *      dataAppHandle       App handle of the application to receive DUNC data
 *                          and other data path related messages
 *----------------------------------------------------------------------------*/
#define CsrBtDuncRegisterDataPathHandleReqSend(_duncInstanceId, _dataHandle) { \
        CsrBtDuncRegisterDataPathHandleReq *msg__ = (CsrBtDuncRegisterDataPathHandleReq *) CsrPmemAlloc(sizeof(CsrBtDuncRegisterDataPathHandleReq)); \
        msg__->type              = CSR_BT_DUNC_REGISTER_DATA_PATH_HANDLE_REQ; \
        msg__->dataAppHandle     = _dataHandle;                         \
        CsrBtDuncMsgTransport(_duncInstanceId, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      DuncChangeDataPathStatusReq
 *
 *  DESCRIPTION
 *      Sends information about status change to the profile
 *
 *  PARAMETERS
 *      duncInstanceId      For multiple DUNC instance purposes. Specifies the ID
 *                          of the DUNC instance to be used by the application
 *      status              The new status of the data path. Can be either
 *                          CSR_BT_DATA_PATH_STATUS_OPEN (device driver opened),
 *                          CSR_BT_DATA_PATH_STATUS_CLOSED (device driver closed) or
 *                          CSR_BT_DATA_PATH_STATUS_LOST (device driver terminated).
 *----------------------------------------------------------------------------*/
#define CsrBtDuncChangeDataPathStatusReqSend(_duncInstanceId, _status) { \
        CsrBtDuncChangeDataPathStatusReq *msg__ = (CsrBtDuncChangeDataPathStatusReq *) CsrPmemAlloc(sizeof(CsrBtDuncChangeDataPathStatusReq)); \
        msg__->type              = CSR_BT_DUNC_CHANGE_DATA_PATH_STATUS_REQ; \
        msg__->status            = _status;                             \
        msg__->duncInstanceId    = CSR_BT_DUNC_IFACEQUEUE;              \
        CsrBtDuncMsgTransport(_duncInstanceId, msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtDuncSecurityOutReqSend
 *
 *  DESCRIPTION
 *      Set the default security settings for new outgoing connections
 *
 *    PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtDuncSecurityOutReqSend(_appHandle, _secLevel) {            \
        CsrBtDuncSecurityOutReq *msg = (CsrBtDuncSecurityOutReq*)CsrPmemAlloc(sizeof(CsrBtDuncSecurityOutReq)); \
        msg->type = CSR_BT_DUNC_SECURITY_OUT_REQ;                       \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        CsrBtDuncMsgTransport(CSR_BT_DUNC_IFACEQUEUE, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtDuncFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT DUNC
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_DUNC_PRIM,
 *      msg:          The message received from Synergy BT DUNC
 *----------------------------------------------------------------------------*/
void CsrBtDuncFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);


#ifdef __cplusplus
}
#endif

#endif /* _DUNC_LIB_H_ */
