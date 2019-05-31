#ifndef CSR_BT_SYNCC_LIB_H__
#define CSR_BT_SYNCC_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2010 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_profiles.h"
#include "csr_bt_syncc_prim.h"
#include "csr_bt_tasks.h"
#include "csr_pmem.h"
#include "csr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtSynccMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSynccConnectReqSend
 *
 *  DESCRIPTION
 *      Try to make a connection the a peer device.
 *
 *    PARAMETERS
 *        appHandle:                ...
 *        maxPacketSize:            ...
 *        destination:              ...
 *        authorize:                ...
 *        realmLength:              ...
 *        realm:                    ...
 *        passwordLength:           ...
 *        password:    ...
 *        userId:      ...
 *        length:               Length is used to express the approximate total
 *                              length of the bodies of all the objects in the
 *                              transaction
 *        count:                Count is used to indicate the number of objects
 *                              that will be sent during this connection
 *----------------------------------------------------------------------------*/
#define CsrBtSynccConnectReqSend(_appHandle,_maxPacketSize,_deviceAddr,_authorize,_realmLength,_realm,_passwordLength,_password,_userId,_length,_count,_windowSize,_srmEnable) { \
        CsrBtSynccConnectReq *msg__      = pnew(CsrBtSynccConnectReq);  \
        msg__->type                      = CSR_BT_SYNCC_CONNECT_REQ;    \
        msg__->appHandle                 = _appHandle;                  \
        msg__->maxPacketSize             = _maxPacketSize;              \
        msg__->deviceAddr                = _deviceAddr;                 \
        msg__->authorize                 = _authorize;                  \
        msg__->realmLength               = _realmLength;                \
        msg__->realm                     = _realm;                      \
        msg__->passwordLength            = _passwordLength;             \
        msg__->password                  = _password;                   \
        msg__->userId                    = _userId;                     \
        msg__->length                    = _length;                     \
        msg__->count                     = _count;                      \
        msg__->windowSize                = _windowSize;                 \
        msg__->srmEnable                 = _srmEnable;                  \
        CsrBtSynccMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSynccCancelConnectReqSend
 *
 *  DESCRIPTION
 *      Try to cancel the connection which is being establish to a peer device.
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtSynccCancelConnectReqSend() {                              \
        CsrBtSynccCancelConnectReq *msg__ = pnew(CsrBtSynccCancelConnectReq); \
        msg__->type    = CSR_BT_SYNCC_CANCEL_CONNECT_REQ;               \
        CsrBtSynccMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSynccDisconnectReqSend
 *
 *  DESCRIPTION
 *      The OBEX - and the Bluetooth connection is release
 *
 *    PARAMETERS
 *        normalObexDisconnect : FALSE defines an Abnormal disconnect sequence where
 *                               the Bluetooth connection is release direct. TRUE
 *                               defines a normal disconnect sequence where the OBEX
 *                               connection is release before the Bluetooth connection
 *----------------------------------------------------------------------------*/
#define CsrBtSynccDisconnectReqSend(_normalObexDisconnect) {            \
        CsrBtSynccDisconnectReq *msg__ = pnew(CsrBtSynccDisconnectReq); \
        msg__->type                  = CSR_BT_SYNCC_DISCONNECT_REQ;     \
        msg__->normalObexDisconnect  = _normalObexDisconnect;           \
        CsrBtSynccMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSynccAuthenticateResSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        password:          ...
 *        passwordLength:    ...
 *        userId:            ...
 *----------------------------------------------------------------------------*/
#define CsrBtSynccAuthenticateResSend(_password,_passwordLength,_userId) { \
        CsrBtSynccAuthenticateRes *msg__ = pnew(CsrBtSynccAuthenticateRes); \
        msg__->type                      = CSR_BT_SYNCC_AUTHENTICATE_RES; \
        msg__->password          = _password;                           \
        msg__->passwordLength    = _passwordLength;                     \
        msg__->userId            = _userId;                             \
        CsrBtSynccMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      SynccGetObjectHeaderReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        ucs2name:           ...
 *----------------------------------------------------------------------------*/
#define CsrBtSynccGetObjectReqSend(_ucs2name, _srmpOn) {                \
        CsrBtSynccGetObjectReq *msg__ = pnew(CsrBtSynccGetObjectReq);   \
        msg__->type              = CSR_BT_SYNCC_GET_OBJECT_REQ;         \
        msg__->ucs2name          = _ucs2name;                           \
        msg__->srmpOn            = _srmpOn;                             \
        CsrBtSynccMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSynccGetObjectResSend
 *
 *  DESCRIPTION
 *      .....
 *
 *    PARAMETERS
 *                    None
 *----------------------------------------------------------------------------*/
#define CsrBtSynccGetObjectResSend(_srmpOn) {                           \
        CsrBtSynccGetObjectRes *msg__ = pnew(CsrBtSynccGetObjectRes);   \
        msg__->type              = CSR_BT_SYNCC_GET_OBJECT_RES;         \
        msg__->srmpOn            = _srmpOn;                             \
        CsrBtSynccMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSynccAddObjectReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        objectName:          ...
 *        lengthOfObject:      ...
 *----------------------------------------------------------------------------*/
#define CsrBtSynccAddObjectReqSend(_objectName,_lengthOfObject) {       \
        CsrBtSynccAddObjectReq *msg__ = pnew(CsrBtSynccAddObjectReq);   \
        msg__->type              = CSR_BT_SYNCC_ADD_OBJECT_REQ;         \
        msg__->lengthOfObject    = _lengthOfObject;                     \
        msg__->objectName        = _objectName;                         \
        CsrBtSynccMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSynccAddObjectResSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        finalFlag:        ...
 *        payloadLength:        ...
 *        *payload:            ...
 *----------------------------------------------------------------------------*/
#define CsrBtSynccAddObjectResSend(_finalFlag,_payloadLength,_payload) { \
        CsrBtSynccAddObjectRes *msg__ = pnew(CsrBtSynccAddObjectRes);   \
        msg__->type              = CSR_BT_SYNCC_ADD_OBJECT_RES;         \
        msg__->finalFlag         = _finalFlag;                          \
        msg__->payloadLength     = _payloadLength;                      \
        msg__->payload           = _payload;                            \
        CsrBtSynccMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSynccModifyObjectReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        objectName:               ...
 *        maxExpectedChangeCounter  ...
 *        lengthOfObject:           ...
 *----------------------------------------------------------------------------*/
#define CsrBtSynccModifyObjectReqSend(_objectName,_maxExpectedChangeCounter,_lengthOfObject) { \
        CsrBtSynccModifyObjectReq *msg__ = pnew(CsrBtSynccModifyObjectReq); \
        msg__->type                      = CSR_BT_SYNCC_MODIFY_OBJECT_REQ; \
        msg__->lengthOfObject            = _lengthOfObject;             \
        msg__->objectName                = _objectName;                 \
        msg__->maxExpectedChangeCounter  = _maxExpectedChangeCounter;   \
        CsrBtSynccMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSynccModifyObjectResSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        finalFlag:        ...
 *        payloadLength:        ...
 *        *payload:            ...
 *----------------------------------------------------------------------------*/
#define CsrBtSynccModifyObjectResSend(_finalFlag,_payloadLength,_payload) { \
        CsrBtSynccModifyObjectRes *msg__ = pnew(CsrBtSynccModifyObjectRes); \
        msg__->type              = CSR_BT_SYNCC_MODIFY_OBJECT_RES;      \
        msg__->finalFlag         = _finalFlag;                          \
        msg__->payloadLength     = _payloadLength;                      \
        msg__->payload           = _payload;                            \
        CsrBtSynccMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSynccDeleteObjectReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        name:          ...
 *        nameLength:    Length of name
 *----------------------------------------------------------------------------*/
#define CsrBtSynccDeleteObjectReqSend(_hardDelete, _maxExpectedChangeCounter, _objectName) { \
        CsrBtSynccDeleteObjectReq *msg__ = pnew(CsrBtSynccDeleteObjectReq); \
        msg__->type                      = CSR_BT_SYNCC_DELETE_OBJECT_REQ; \
        msg__->hardDelete                = _hardDelete;                 \
        msg__->maxExpectedChangeCounter  = _maxExpectedChangeCounter;   \
        msg__->objectName                = _objectName;                 \
        CsrBtSynccMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSynccAbortReqSend
 *
 *  DESCRIPTION
 *      .....
 *
 *    PARAMETERS
 *        None
 *----------------------------------------------------------------------------*/
#define CsrBtSynccAbortReqSend() {                              \
        CsrBtSynccAbortReq * msg__ = pnew(CsrBtSynccAbortReq);  \
        msg__->type                = CSR_BT_SYNCC_ABORT_REQ;    \
        CsrBtSynccMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSynccActivateSyncCommandReqSend
 *
 *  DESCRIPTION
 *      .....
 *
 *    PARAMETERS
 *        appHandle:          ...
 *        maxPacketSize:          ...
 *----------------------------------------------------------------------------*/
#define CsrBtSynccActivateSyncCommandReqSend(_appHandle,_maxPacketSize) { \
        CsrBtSynccActivateSyncCommandReq *msg__ = pnew(CsrBtSynccActivateSyncCommandReq); \
        msg__->type                  = CSR_BT_SYNCC_ACTIVATE_SYNC_COMMAND_REQ; \
        msg__->appHandle             = _appHandle;                      \
        msg__->maxPacketSize         = _maxPacketSize;                  \
        CsrBtSynccMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSynccSyncCommandResSend
 *
 *  DESCRIPTION
 *      .....
 *
 *    PARAMETERS
 *        responseCode:          ...
 *----------------------------------------------------------------------------*/
#define CsrBtSynccSyncCommandResSend(_responseCode, _srmpOn) {          \
        CsrBtSynccSyncCommandRes *msg__ = pnew(CsrBtSynccSyncCommandRes); \
        msg__->type                     = CSR_BT_SYNCC_SYNC_COMMAND_RES; \
        msg__->responseCode             = _responseCode;                \
        msg__->srmpOn                   = _srmpOn;                      \
        CsrBtSynccMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSynccDeactivateSyncCommandReqSend
 *
 *  DESCRIPTION
 *      .....
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtSynccDeactivateSyncCommandReqSend() {                      \
        CsrBtSynccDeactivateSyncCommandReq *msg__ = pnew(CsrBtSynccDeactivateSyncCommandReq); \
        msg__->type                  = CSR_BT_SYNCC_DEACTIVATE_SYNC_COMMAND_REQ; \
        CsrBtSynccMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSynccSecurityInReqSend
 *
 *  DESCRIPTION
 *      Set the default security settings for new incoming/outgoing connections
 *
 *  PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSynccSecurityInReqSend(_appHandle, _secLevel) {            \
        CsrBtSynccSecurityInReq *msg   = pnew(CsrBtSynccSecurityInReq); \
        msg->type          = CSR_BT_SYNCC_SECURITY_IN_REQ;              \
        msg->appHandle     = _appHandle;                                \
        msg->secLevel      = _secLevel;                                 \
        CsrBtSynccMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSynccSecurityOutReqSend
 *
 *  DESCRIPTION
 *      Set the default security settings for new incoming/outgoing connections
 *
 *  PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSynccSecurityOutReqSend(_appHandle, _secLevel) {           \
        CsrBtSynccSecurityOutReq *msg   = pnew(CsrBtSynccSecurityOutReq); \
        msg->type          = CSR_BT_SYNCC_SECURITY_OUT_REQ;             \
        msg->appHandle     = _appHandle;                                \
        msg->secLevel      = _secLevel;                                 \
        CsrBtSynccMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSynccFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT SYNCC
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_SYNCC_PRIM,
 *      msg:          The message received from Synergy BT SYNCC
 *----------------------------------------------------------------------------*/
void CsrBtSynccFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);


#ifdef __cplusplus
}
#endif

#endif /* _SYNCC_PRIM_H */
