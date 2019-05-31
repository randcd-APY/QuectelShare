#ifndef CSR_BT_SMLC_LIB_H__
#define CSR_BT_SMLC_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2010 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_profiles.h"
#include "csr_bt_smlc_prim.h"
#include "csr_bt_tasks.h"
#include "csr_pmem.h"
#include "csr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtSmlcMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSmlcActivateReqSend
 *
 *  DESCRIPTION
 *      Set the Syncml Client in discoverable (advertising) mode and in TBDTBD !!ATT: also .. connectable mode ???
 *      according to request parameters....
 *    PARAMETERS
 *        .

 *----------------------------------------------------------------------------*/
#define CsrBtSmlcActivateReqSend(_appHandle,_advEnable,_acceptServConnect) { \
        CsrBtSmlcActivateReq *msg__ = (CsrBtSmlcActivateReq *) CsrPmemAlloc(sizeof(CsrBtSmlcActivateReq)); \
        msg__->type              = CSR_BT_SMLC_ACTIVATE_REQ;            \
        msg__->appHandle         = _appHandle;                          \
        msg__->advEnable         = _advEnable;                          \
        msg__->acceptServConnect = _acceptServConnect;                  \
        CsrBtSmlcMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSmlcDeactivateReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *        None.
 *----------------------------------------------------------------------------*/
#define CsrBtSmlcDeactivateReqSend() {                                  \
        CsrBtSmlcDeactivateReq *msg__ = (CsrBtSmlcDeactivateReq *) CsrPmemAlloc(sizeof(CsrBtSmlcDeactivateReq)); \
        msg__->type  = CSR_BT_SMLC_DEACTIVATE_REQ;                      \
        CsrBtSmlcMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSmlcConnectReqSend
 *
 *  DESCRIPTION
 *      Try to make an obex-connection to a peer device.
 *
 *    PARAMETERS
 *        appHandle:        ...
 *        maxPacketSize:    ...
 *        *destination:    ...
 ATT caller has to handle(free) the pointer after calling this function
 *        authorize        ...
 *        reamlLength:       ...
 *        reaml:             ...
 *        passwordLength:    ...
 *        password:          ...
 *        userId:   ...
 *        length:               Length is used to express the approximate total
 *                              length of the bodies of all the objects in the
 *                              transaction
 *        count:                Count is used to indicate the number of objects
 *                              that will be sent during this connection
 *----------------------------------------------------------------------------*/
#define CsrBtSmlcConnectReqSend(_appHandle,_maxPacketSize,_destination,_targetService,_authorize,_realmLength,_realm,_passwordLength,_password,_userId,_length,_count,_windowSize,_srmEnable) { \
        CsrBtSmlcConnectReq *msg__ = (CsrBtSmlcConnectReq *) CsrPmemAlloc(sizeof(CsrBtSmlcConnectReq)); \
        msg__->type           = CSR_BT_SMLC_CONNECT_REQ;                \
        msg__->appHandle      = _appHandle;                             \
        msg__->maxPacketSize  = _maxPacketSize;                         \
        msg__->destination    = _destination;                           \
        msg__->targetService  = _targetService;                         \
        msg__->authorize      = _authorize;                             \
        msg__->realm          = _realm;                                 \
        msg__->realmLength    = _realmLength;                           \
        msg__->password       = _password;                              \
        msg__->passwordLength = _passwordLength;                        \
        msg__->userId         = _userId;                                \
        msg__->length         = _length;                                \
        msg__->count          = _count;                                 \
        msg__->windowSize     = _windowSize;                            \
        msg__->srmEnable      = _srmEnable;                             \
        CsrBtSmlcMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSmlcAuthenticateResSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        passwordLength:    ...
 *        password:          ...
 *        userId:            ...
 *----------------------------------------------------------------------------*/
#define CsrBtSmlcAuthenticateResSend(_passwordLength,_password,_userId) { \
        CsrBtSmlcAuthenticateRes *msg__ = (CsrBtSmlcAuthenticateRes *) CsrPmemAlloc(sizeof(CsrBtSmlcAuthenticateRes)); \
        msg__->type                          = CSR_BT_SMLC_AUTHENTICATE_RES; \
        msg__->password          = _password;                           \
        msg__->passwordLength    = _passwordLength;                     \
        msg__->userId            = _userId;                             \
        CsrBtSmlcMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSmlcGetSmlMsgObjReqSend
 *
 *  DESCRIPTION    initial function to call when to get a syncml msg from remote
 *      ......
 *
 *    PARAMETERS
 *        mimeType:
 *----------------------------------------------------------------------------*/
#define CsrBtSmlcGetSmlMsgObjReqSend(_mimeType, _srmpOn) {              \
        CsrBtSmlcGetSmlMsgObjReq *msg__ = (CsrBtSmlcGetSmlMsgObjReq *) CsrPmemAlloc(sizeof(CsrBtSmlcGetSmlMsgObjReq)); \
        msg__->type      = CSR_BT_SMLC_GET_SML_MSG_OBJ_REQ;             \
        msg__->mimeType  = _mimeType;                                   \
        msg__->srmpOn    = _srmpOn;                                     \
        CsrBtSmlcMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      (SmlcGetSmlMsgObjBodyReqSend)
 *
 *  DESCRIPTION
 *      .....
 *
 *    PARAMETERS
 *      None
 *----------------------------------------------------------------------------*/
#define CsrBtSmlcGetNextChunkSmlMsgObjReqSend(_srmpOn) {                \
        CsrBtSmlcGetNextChunkSmlMsgObjReq *msg__ = (CsrBtSmlcGetNextChunkSmlMsgObjReq *) CsrPmemAlloc(sizeof(CsrBtSmlcGetNextChunkSmlMsgObjReq)); \
        msg__->type      = CSR_BT_SMLC_GET_NEXT_CHUNK_SML_MSG_OBJ_REQ;  \
        msg__->srmpOn    = _srmpOn;                                     \
        CsrBtSmlcMsgTransport(msg__);}



/*----------------------------------------------------------------------------*
 *  NAME
 *    CsrBtSmlcPutSmlMsgObjReqSend
 *
 *  DESCRIPTION            initial function to call when putting a syncmlmsg
 *    ......
 *
 *  PARAMETERS
 *    mimeType:
 *    lengthOfObject:
 *    finalFlag:
 *    bodyLength:
 *    *body:
 ATT caller has to handle(free) the pointer after calling this function
 *----------------------------------------------------------------------------*/
#define CsrBtSmlcPutSmlMsgObjReqSend(_mimeType,_lengthOfObject,_finalFlag,_bodyLength,_body) { \
        CsrBtSmlcPutSmlMsgObjReq    *msg__ = (CsrBtSmlcPutSmlMsgObjReq *) CsrPmemAlloc(sizeof(CsrBtSmlcPutSmlMsgObjReq)); \
        msg__->type              = CSR_BT_SMLC_PUT_SML_MSG_OBJ_REQ;     \
        msg__->lengthOfObject    = _lengthOfObject;                     \
        msg__->mimeType          = _mimeType;                           \
        msg__->body              = _body;                               \
        msg__->bodyLength        = _bodyLength;                         \
        msg__->finalFlag         = _finalFlag;                          \
        CsrBtSmlcMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSmlcPutNextChunkSmlMsgObjReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *        finalFlag:
 *        bodyLength:
 *        *body:
 ATT caller has to handle(free) the pointer after calling this function
 *----------------------------------------------------------------------------*/
#define CsrBtSmlcPutNextChunkSmlMsgObjReqSend(_finalFlag,_bodyLength,_body) { \
        CsrBtSmlcPutNextChunkSmlMsgObjReq *msg__ = (CsrBtSmlcPutNextChunkSmlMsgObjReq *) CsrPmemAlloc(sizeof(CsrBtSmlcPutNextChunkSmlMsgObjReq)); \
        msg__->type          = CSR_BT_SMLC_PUT_NEXT_CHUNK_SML_MSG_OBJ_REQ; \
        msg__->finalFlag     = _finalFlag;                              \
        msg__->bodyLength    = _bodyLength;                             \
        msg__->body          = _body;                                   \
        CsrBtSmlcMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSmlcAbortReqSend
 *
 *  DESCRIPTION
 *      .....
 *
 *    PARAMETERS
 *        None
 *----------------------------------------------------------------------------*/
#define CsrBtSmlcAbortReqSend() {                                       \
        CsrBtSmlcAbortReq *msg__ = (CsrBtSmlcAbortReq *) CsrPmemAlloc(sizeof(CsrBtSmlcAbortReq)); \
        msg__->type  = CSR_BT_SMLC_ABORT_REQ;                           \
        CsrBtSmlcMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSmlcDisconnectReqSend
 *
 *  DESCRIPTION
 *      The OBEX - and the Bluetooth connection is released
 *
 *    PARAMETERS
 *        theNormalDisconnect : FALSE defines an Abnormal disconnect sequence where
 *                              the Bluetooth connection is released direct. TRUE
 *                              defines a normal disconnect sequence where the OBEX
 *                              connection is released before the Bluetooth connection
 *----------------------------------------------------------------------------*/
#define CsrBtSmlcDisconnectReqSend(_normalDisconnect) {                 \
        CsrBtSmlcDisconnectReq *msg__ = (CsrBtSmlcDisconnectReq *) CsrPmemAlloc(sizeof(CsrBtSmlcDisconnectReq)); \
        msg__->type              = CSR_BT_SMLC_DISCONNECT_REQ;          \
        msg__->normalDisconnect  = _normalDisconnect;                   \
        CsrBtSmlcMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSmlcCancelConnectReqSend
 *
 *  DESCRIPTION
 *      .....
 *
 *    PARAMETERS
 *        None
 *----------------------------------------------------------------------------*/
#define CsrBtSmlcCancelConnectReqSend() {                               \
        CsrBtSmlcCancelConnectReq *msg__ = (CsrBtSmlcCancelConnectReq *) CsrPmemAlloc(sizeof(CsrBtSmlcCancelConnectReq)); \
        msg__->type  = CSR_BT_SMLC_CANCEL_CONNECT_REQ;                  \
        CsrBtSmlcMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSmlcSecurityOutReqSend
 *      CsrBtSmlcSecurityInReqSend
 *
 *  DESCRIPTION
 *      Set the default security settings for new incoming/outgoing connections
 *
 *  PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSmlcSecurityInReqSend(_appHandle, _secLevel) {             \
        CsrBtSmlcSecurityInReq *msg = (CsrBtSmlcSecurityInReq*)CsrPmemAlloc(sizeof(CsrBtSmlcSecurityInReq)); \
        msg->type = CSR_BT_SMLC_SECURITY_IN_REQ;                        \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        CsrBtSmlcMsgTransport(msg);}

#define CsrBtSmlcSecurityOutReqSend(_appHandle, _secLevel) {            \
        CsrBtSmlcSecurityOutReq *msg = (CsrBtSmlcSecurityOutReq*)CsrPmemAlloc(sizeof(CsrBtSmlcSecurityOutReq)); \
        msg->type = CSR_BT_SMLC_SECURITY_OUT_REQ;                       \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        CsrBtSmlcMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSmlcFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT SMLC
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_SMLC_PRIM,
 *      msg:          The message received from Synergy BT SMLC
 *----------------------------------------------------------------------------*/
void CsrBtSmlcFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);


#ifdef __cplusplus
}
#endif

#endif

