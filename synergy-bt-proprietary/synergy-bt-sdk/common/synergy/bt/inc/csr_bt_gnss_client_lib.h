#ifndef CSR_BT_GNSS_CLIENT_LIB_H__
#define CSR_BT_GNSS_CLIENT_LIB_H__

/******************************************************************************

Copyright (c) 2013-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_bt_gnss_client_prim.h"
#include "csr_pmem.h"
#include "csr_bt_tasks.h"
#include "csr_util.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Common put_message function to reduce code size */
void CsrBtGnssClientMsgTransport(void *msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtGnssClientConnectReqSend
 *
 *  DESCRIPTION
 *      This signal is used to send a connect request to the GNSS server.
 *      GNSS client would reply back with result in CSR_BT_GNSS_CLIENT_CONNECT_CFM
 *      (connection indication).
 *      GNSS client cannot have more than one connection at time. GNSS client would
 *      reply with CSR_BT_RESULT_CODE_GNSS_CLIENT_UNSUFFICENT_RESOURCES as result
 *      in GNSS_BT_GNSS_CLIENT_CONNECT_CFM if new connection is requested in
 *      connected state.
 *      Application can request connection right after disconnection is requested.
 *
 *  PARAMETERS
 *      appHandle:          Application handle
 *      bdAddr:             Bluetooth address of remote device
 *      secLevel:           Minimum outgoing security level requested by application.
 *                          The application must specify one of the following values:
 *                          1. CSR_BT_SEC_DEFAULT : Use default security settings
 *                          2. CSR_BT_SEC_MANDATORY : Use mandatory security settings
 *                          3. CSR_BT_SEC_SPECIFY : Specify new security settings
 *
 *                          If CSR_BT_SEC_SPECIFY is set the following values can be OR'ed additionally:
 *                          1. CSR_BT_SEC_AUTHORISATION: Require authorisation
 *                          2. CSR_BT_SEC_AUTHENTICATION: Require authentication
 *                          3. CSR_BT_SEC_SEC_ENCRYPTION: Require encryption (implies authentication)
 *                          4. CSR_BT_SEC_MITM: Require MITM protection (implies encryption)
 *
 *                          CSR GNSS client mandates authentication and encryption (CSR_BT_GNSS_MANDATORY_SECURITY_OUTGOING).
 *                          A default security level (CSR_BT_GNSS_CLIENT_DEFAULT_SECURITY_OUTGOING) is defined in csr_bt_usr_config.h.
 *----------------------------------------------------------------------------*/
#define CsrBtGnssClientConnectReqSend(_appHandle, _bdAddr, _secLevel) do{   \
        CsrBtGnssClientConnectReq *msg = (CsrBtGnssClientConnectReq *) CsrPmemAlloc(sizeof(CsrBtGnssClientConnectReq)); \
        msg->type       = CSR_BT_GNSS_CLIENT_CONNECT_REQ;                   \
        msg->appHandle  = _appHandle;                                       \
        msg->bdAddr     = _bdAddr;                                          \
        msg->secLevel   = _secLevel;                                        \
        CsrBtGnssClientMsgTransport(msg);}while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtGnssClientCancelConnectReqSend
 *
 *  DESCRIPTION
 *      This signal is used to cancel connection procedure.
 *      GNSS client would reply back with CSR_BT_RESULT_CODE_GNSS_CLIENT_CONNECTION_CANCELLED
 *      as result in CSR_BT_GNSS_CLIENT_CONNECT_IND.
 *      This request is applicable only if connection procedure is going on. It
 *      will be ignored if connection has been established already.
 *
 *  PARAMETERS
 *      appHandle:          Application handle
 *      bdAddr:             Bluetooth address of remote device
 *----------------------------------------------------------------------------*/
#define CsrBtGnssClientCancelConnectReqSend(_appHandle, _bdAddr) do{    \
        CsrBtGnssClientCancelConnectReq *msg = (CsrBtGnssClientCancelConnectReq *) CsrPmemAlloc(sizeof(CsrBtGnssClientCancelConnectReq)); \
        msg->type       = CSR_BT_GNSS_CLIENT_CANCEL_CONNECT_REQ;        \
        msg->appHandle  = _appHandle;                                   \
        msg->bdAddr     = _bdAddr;                                      \
        CsrBtGnssClientMsgTransport(msg);}while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtGnssClientDisconnectReqSend
 *
 *  DESCRIPTION
 *      This signal is used to send a disconnect request to the server.
 *      GNSS client would reply with CSR_BT_GNSS_CLIENT_DISCONNECT_IND
 *      (disconnect indication).
 *      This request would be ignored if there is no connection or the connection
 *      id mentioned is wrong.
 *
 *  PARAMETERS
 *        btConnId:         Connection id
 *----------------------------------------------------------------------------*/
#define CsrBtGnssClientDisconnectReqSend(_connId) do{           \
        CsrBtGnssClientDisconnectReq *msg = (CsrBtGnssClientDisconnectReq *) CsrPmemAlloc(sizeof(CsrBtGnssClientDisconnectReq)); \
        msg->type       = CSR_BT_GNSS_CLIENT_DISCONNECT_REQ;    \
        msg->btConnId   = _connId;                              \
        CsrBtGnssClientMsgTransport(msg);}while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtGnssClientDataReqSend
 *
 *  DESCRIPTION
 *      This signal is used to send a data to the server.
 *      GNSS client would reply with CSR_BT_GNSS_CLIENT_DATA_CFM (data
 *      confirmation).
 *      This request is applicable only in connected state.
 *      GNSS client would reply with CSR_BT_RESULT_CODE_GNSS_CLIENT_NO_CONNECTION
 *      as result in CSR_BT_GNSS_CLIENT_DATA_CFM if connection id is wrong.
 *
 *      Note: Data request is optional feature and can be excluded by enabling
 *      cmake cache parameter EXCLUDE_CSR_BT_GNSS_CLIENT_MODULE_OPTIONAL during
 *      project configuration. In that case GNSS client would not act upon the
 *      data request and would treat it as unknown request.
 *
 *  PARAMETERS
 *        btConnId:         Connection id
 *        payload:          Data pointer
 *        payloadLength:    Payload length
 *                          Should be less than maxFrameSize returned in connection indication.
 *----------------------------------------------------------------------------*/
#define CsrBtGnssClientDataReqSend(_connId, _payload, _payloadLength) do{     \
        CsrBtGnssClientDataReq *msg = (CsrBtGnssClientDataReq *) CsrPmemAlloc(sizeof(CsrBtGnssClientDataReq)); \
        msg->type       = CSR_BT_GNSS_CLIENT_DATA_REQ;              \
        msg->btConnId   = _connId;                                  \
        msg->payload    = _payload;                                 \
        msg->payloadLength = _payloadLength;                        \
        CsrBtGnssClientMsgTransport(msg);}while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtGnssClientFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT GNSS_CLIENT
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass:         Must be CSR_BT_GNSS_CLIENT_PRIM,
 *      msg:                The message received from Synergy BT GNSS_CLIENT
 *----------------------------------------------------------------------------*/
void CsrBtGnssClientFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);

#ifdef __cplusplus
}
#endif

#endif
