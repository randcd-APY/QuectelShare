#ifndef CSR_BT_SAPC_LIB_H__
#define CSR_BT_SAPC_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_sapc_prim.h"
#include "csr_pmem.h"
#include "csr_bt_tasks.h"
#include "csr_pmem.h"
#include "csr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtSapcMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSapcConnectReqSend
 *
 *  DESCRIPTION
 *      This signal is used to send a connect request to the SAP server
 *
 *  PARAMETERS
 *        theMaxMsgSize:            The proposal of the largest message size to be
 *                                transmitted between the client and server
 *----------------------------------------------------------------------------*/
#define CsrBtSapcConnectReqSend(_appHandle, _maxMsgSize, _bdAddr) {     \
        CsrBtSapcConnectReq *msg = (CsrBtSapcConnectReq *) CsrPmemAlloc(sizeof(CsrBtSapcConnectReq)); \
        msg->type       = CSR_BT_SAPC_CONNECT_REQ;                      \
        msg->appHandle  = _appHandle;                                   \
        msg->bdAddr     = _bdAddr;                                      \
        msg->maxMsgSize = _maxMsgSize;                                  \
        CsrBtSapcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSapcTransferAtrReqSend
 *
 *  DESCRIPTION
 *      This signal is used to request an ATR (Answer To Reset) of the SIM at
 *        the server
 *
 *  PARAMETERS
 *        none
 *----------------------------------------------------------------------------*/
#define CsrBtSapcTransferAtrReqSend() {                                 \
        CsrBtSapcTransferAtrReq *msg = (CsrBtSapcTransferAtrReq *) CsrPmemAlloc(sizeof(CsrBtSapcTransferAtrReq)); \
        msg->type = CSR_BT_SAPC_TRANSFER_ATR_REQ;                       \
        CsrBtSapcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSapcTransferApduReqSend
 *
 *  DESCRIPTION
 *      This signal is used to send an command APDU (Application Protocol Data Unit)
 *        to the server
 *
 *  PARAMETERS
 *        commandApdu:            A pointer to the command APDU(s)
 *        commandApduLength:                The length of the command APDU(s)
 *        apdu7816Type:            True if the APDU is a 7816 type, else FALSE
 *----------------------------------------------------------------------------*/
#define CsrBtSapcTransferApduReqSend(_commandApdu, _commandApduLength, _apdu7816Type) { \
        CsrBtSapcTransferApduReq *msg = (CsrBtSapcTransferApduReq *) CsrPmemAlloc(sizeof(CsrBtSapcTransferApduReq)); \
        msg->type = CSR_BT_SAPC_TRANSFER_APDU_REQ;                      \
        msg->commandApdu = _commandApdu;                                \
        msg->commandApduLength = _commandApduLength;                    \
        msg->apdu7816Type = _apdu7816Type;                              \
        CsrBtSapcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSapcDisconnectReqSend
 *
 *  DESCRIPTION
 *      This signal is used to send a disconnect request to the server
 *
 *  PARAMETERS
 *        none
 *----------------------------------------------------------------------------*/
#define CsrBtSapcDisconnectReqSend() {                                  \
        CsrBtSapcDisconnectReq *msg = (CsrBtSapcDisconnectReq *) CsrPmemAlloc(sizeof(CsrBtSapcDisconnectReq)); \
        msg->type = CSR_BT_SAPC_DISCONNECT_REQ;                         \
        CsrBtSapcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSapcPowerSimOffReqSend
 *
 *  DESCRIPTION
 *      This signal is used to send a request for powering off the SIM
 *
 *  PARAMETERS
 *        none
 *----------------------------------------------------------------------------*/
#define CsrBtSapcPowerSimOffReqSend() {                                 \
        CsrBtSapcPowerSimOffReq *msg = (CsrBtSapcPowerSimOffReq *) CsrPmemAlloc(sizeof(CsrBtSapcPowerSimOffReq)); \
        msg->type = CSR_BT_SAPC_POWER_SIM_OFF_REQ;                      \
        CsrBtSapcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSapcPowerSimOnReqSend
 *
 *  DESCRIPTION
 *      This signal is used to send a request for powering on the SIM
 *
 *  PARAMETERS
 *        none
 *----------------------------------------------------------------------------*/
#define CsrBtSapcPowerSimOnReqSend() {                                  \
        CsrBtSapcPowerSimOnReq *msg = (CsrBtSapcPowerSimOnReq *) CsrPmemAlloc(sizeof(CsrBtSapcPowerSimOffReq)); \
        msg->type = CSR_BT_SAPC_POWER_SIM_ON_REQ;                       \
        CsrBtSapcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSapcResetSimReqSend
 *
 *  DESCRIPTION
 *      This signal is used if the client wants the server to reset the SIM
 *
 *  PARAMETERS
 *        none
 *----------------------------------------------------------------------------*/
#define CsrBtSapcResetSimReqSend() {                                    \
        CsrBtSapcResetSimReq *msg = (CsrBtSapcResetSimReq *) CsrPmemAlloc(sizeof(CsrBtSapcResetSimReq)); \
        msg->type = CSR_BT_SAPC_RESET_SIM_REQ;                          \
        CsrBtSapcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSapcTransferCardReaderStatusReqSend
 *
 *  DESCRIPTION
 *      This signal is used if the client wants the server to return the card
 *        reader status
 *
 *  PARAMETERS
 *        none
 *----------------------------------------------------------------------------*/
#define CsrBtSapcTransferCardReaderStatusReqSend() {                    \
        CsrBtSapcTransferCardReaderStatusReq *msg = (CsrBtSapcTransferCardReaderStatusReq *) CsrPmemAlloc(sizeof(CsrBtSapcTransferCardReaderStatusReq)); \
        msg->type = CSR_BT_SAPC_TRANSFER_CARD_READER_STATUS_REQ;        \
        CsrBtSapcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSapcSetTransferProtocolReqSend
 *
 *  DESCRIPTION
 *      This signal is used if the client wants the server to change the transfer
 *        protocol
 *
 *  PARAMETERS
 *        theTransferProtocol
 *----------------------------------------------------------------------------*/
#define CsrBtSapcSetTransferProtocolReqSend(_transferProtocol) {        \
        CsrBtSapcSetTransferProtocolReq * msg = (CsrBtSapcSetTransferProtocolReq *) CsrPmemAlloc(sizeof(CsrBtSapcSetTransferProtocolReq)); \
        msg->type = CSR_BT_SAPC_SET_TRANSFER_PROTOCOL_REQ;              \
        msg->transportProtocol = _transferProtocol;                     \
        CsrBtSapcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSapcSecurityOutReqSend
 *
 *  DESCRIPTION
 *      Set the default security settings for new outgoing connections
 *
 *  PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSapcSecurityOutReqSend(_appHandle, _secLevel) {            \
        CsrBtSapcSecurityOutReq *msg = (CsrBtSapcSecurityOutReq*)CsrPmemAlloc(sizeof(CsrBtSapcSecurityOutReq)); \
        msg->type = CSR_BT_SAPC_SECURITY_OUT_REQ;                       \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        CsrBtSapcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSapcFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT SAPC
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_SAPC_PRIM,
 *      msg:          The message received from Synergy BT SAPC
 *----------------------------------------------------------------------------*/
void CsrBtSapcFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);


#ifdef __cplusplus
}
#endif

#endif
