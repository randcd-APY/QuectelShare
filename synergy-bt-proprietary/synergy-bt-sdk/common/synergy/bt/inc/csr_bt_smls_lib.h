#ifndef CSR_BT_SMLS_LIB_H__
#define CSR_BT_SMLS_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2010 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_profiles.h"
#include "csr_bt_smls_prim.h"
#include "csr_bt_tasks.h"
#include "csr_pmem.h"
#include "csr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtSmlsMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSmlsActivateReqSend
 *
 *  DESCRIPTION
 *      Set the SyncML server in discoverable mode and in connectable mode
 *
 *    PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSmlsActivateReqSend(_appHandle, _obexMaxPacketSize, _windowSize, _srmEnable) { \
        CsrBtSmlsActivateReq *msg__ = (CsrBtSmlsActivateReq *) CsrPmemAlloc(sizeof(CsrBtSmlsActivateReq)); \
        msg__->type      = CSR_BT_SMLS_ACTIVATE_REQ;                    \
        msg__->appHandle = _appHandle;                                  \
        msg__->obexMaxPacketSize = _obexMaxPacketSize;                  \
        msg__->windowSize = _windowSize;                                \
        msg__->srmEnable = _srmEnable;                                  \
        CsrBtSmlsMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSmlsDeactivateReqSend
 *
 *  DESCRIPTION
 *      ....
 *
 *    PARAMETERS
 *        None.
 *----------------------------------------------------------------------------*/
#define CsrBtSmlsDeactivateReqSend() {                                  \
        CsrBtSmlsDeactivateReq *msg__ = (CsrBtSmlsDeactivateReq *) CsrPmemAlloc(sizeof(CsrBtSmlsDeactivateReq)); \
        msg__->type  = CSR_BT_SMLS_DEACTIVATE_REQ;                      \
        CsrBtSmlsMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSmlsConnectResSend
 *
 *  DESCRIPTION
 *        ....
 *
 *    PARAMETERS
 *        obexMaxPacketSize:
 *        connectionId:
 *        responseCode:

 *----------------------------------------------------------------------------*/
#define CsrBtSmlsConnectResSend(_connectionId,_responseCode,_targetService) { \
        CsrBtSmlsConnectRes *msg__ = (CsrBtSmlsConnectRes *) CsrPmemAlloc(sizeof(CsrBtSmlsConnectRes)); \
        msg__->type              = CSR_BT_SMLS_CONNECT_RES;             \
        msg__->connectionId      = _connectionId;                       \
        msg__->responseCode      = _responseCode;                       \
        msg__->targetService     = _targetService;                      \
        CsrBtSmlsMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSmlsAuthenticateReqSend
 *
 *  DESCRIPTION
 *
 *    PARAMETERS
 *        reamlLength:       ...
 *        reaml:             ...
 *        passwordLength:    ...
 *        password:          ...
 *        userId:            ...
 *----------------------------------------------------------------------------*/
#define CsrBtSmlsAuthenticateReqSend(_realmLength,_realm,_passwordLength,_password,_userId) { \
        CsrBtSmlsAuthenticateReq *msg__ = (CsrBtSmlsAuthenticateReq *) CsrPmemAlloc(sizeof(CsrBtSmlsAuthenticateReq)); \
        msg__->type                      = CSR_BT_SMLS_AUTHENTICATE_REQ; \
        msg__->realm             = _realm;                              \
        msg__->realmLength       = _realmLength;                        \
        msg__->password          = _password;                           \
        msg__->passwordLength    = _passwordLength;                     \
        msg__->userId            = _userId;                             \
        CsrBtSmlsMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSmlsAuthenticateResSend
 *
 *  DESCRIPTION
 *
 *    PARAMETERS
 *        passwordLength:    ...
 *        password:          ...
 *        userId:            ...
 *----------------------------------------------------------------------------*/
#define CsrBtSmlsAuthenticateResSend(_passwordLength,_password,_userId) { \
        CsrBtSmlsAuthenticateRes *msg__ = (CsrBtSmlsAuthenticateRes *) CsrPmemAlloc(sizeof(CsrBtSmlsAuthenticateRes)); \
        msg__->type                          = CSR_BT_SMLS_AUTHENTICATE_RES; \
        msg__->password          = _password;                           \
        msg__->passwordLength    = _passwordLength;                     \
        msg__->userId            = _userId;                             \
        CsrBtSmlsMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSmlsPutSmlMsgObjResSend
 *
 *  DESCRIPTION
 *
 *    PARAMETERS
 *        connectionId:
 *        responseCode:
 *----------------------------------------------------------------------------*/
#define CsrBtSmlsPutSmlMsgObjResSend(_connectionId,_responseCode, _srmpOn) { \
        CsrBtSmlsPutSmlMsgObjRes *msg__ = (CsrBtSmlsPutSmlMsgObjRes *) CsrPmemAlloc(sizeof(CsrBtSmlsPutSmlMsgObjRes)); \
        msg__->type          = CSR_BT_SMLS_PUT_SML_MSG_OBJ_RES;         \
        msg__->connectionId  = _connectionId;                           \
        msg__->responseCode  = _responseCode;                           \
        msg__->srmpOn        = _srmpOn;                                 \
        CsrBtSmlsMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSmlsPutSmlMsgObjNextResSend
 *
 *  DESCRIPTION
 *
 *    PARAMETERS
 *        connectionId:
 *        responseCode:
 *----------------------------------------------------------------------------*/
#define CsrBtSmlsPutSmlMsgObjNextResSend(_connectionId,_responseCode) { \
        CsrBtSmlsPutSmlMsgObjNextRes *msg__ = (CsrBtSmlsPutSmlMsgObjNextRes *) CsrPmemAlloc(sizeof(CsrBtSmlsPutSmlMsgObjNextRes)); \
        msg__->type          = CSR_BT_SMLS_PUT_SML_MSG_OBJ_NEXT_RES;    \
        msg__->connectionId  = _connectionId;                           \
        msg__->responseCode  = _responseCode;                           \
        msg__->srmpOn        = FALSE;                                   \
        CsrBtSmlsMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSmlsGetSmlMsgObjResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *    PARAMETERS
 *        connectionId:
 *        finalFlag:
 *        responseCode:
 *        lengthOfObject:
 *        length:
 *        *body:
 ATT the pointer will be freed by the receiving task after calling this function
 *----------------------------------------------------------------------------*/
#define CsrBtSmlsGetSmlMsgObjResSend(_connectionId,_finalFlag,_responseCode,_lengthOfObject,_length,_body) { \
        CsrBtSmlsGetSmlMsgObjRes *msg__ = (CsrBtSmlsGetSmlMsgObjRes *) CsrPmemZalloc(sizeof(CsrBtSmlsGetSmlMsgObjRes)); \
        msg__->type              = CSR_BT_SMLS_GET_SML_MSG_OBJ_RES;     \
        msg__->connectionId      = _connectionId;                       \
        msg__->finalFlag         = _finalFlag;                          \
        msg__->responseCode      = _responseCode;                       \
        msg__->lengthOfObject    = _lengthOfObject;                     \
        msg__->srmpOn            = FALSE;                               \
        if (_body != NULL && _length > 0)                               \
        {                                                               \
            msg__->bodyLength    = _length;                             \
            msg__->body          = _body;                               \
        }                                                               \
        CsrBtSmlsMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSmlsGetSmlMsgObjNextResSend
 *
 *  DESCRIPTION
 *
 *    PARAMETERS
 *        connectionId:
 *        finalFlag:
 *        responseCode:
 *        length:
 *        body:
 ATT the pointer will be freed by the receiving task after calling this function
 *----------------------------------------------------------------------------*/
#define CsrBtSmlsGetSmlMsgObjNextResSend(_connectionId,_finalFlag,_responseCode,_length,_body) { \
        CsrBtSmlsGetSmlMsgObjNextRes *msg__ = (CsrBtSmlsGetSmlMsgObjNextRes *) CsrPmemZalloc(sizeof(CsrBtSmlsGetSmlMsgObjNextRes)); \
        msg__->type           = CSR_BT_SMLS_GET_SML_MSG_OBJ_NEXT_RES;   \
        msg__->connectionId   = _connectionId;                          \
        msg__->finalFlag      = _finalFlag;                             \
        msg__->responseCode   = _responseCode;                          \
        msg__->srmpOn         = FALSE;                                  \
        if (_body != NULL && _length > 0)                               \
        {                                                               \
            msg__->body       = _body;                                  \
            msg__->bodyLength = _length;                                \
        }                                                               \
        CsrBtSmlsMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSmlsSecurityInReqSend
 *
 *  DESCRIPTION
 *      Set the default security settings for new incoming/outgoing connections
 *
 *  PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSmlsSecurityInReqSend(_appHandle, _secLevel) {             \
        CsrBtSmlsSecurityInReq *msg = (CsrBtSmlsSecurityInReq*)CsrPmemAlloc(sizeof(CsrBtSmlsSecurityInReq)); \
        msg->type = CSR_BT_SMLS_SECURITY_IN_REQ;                        \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        CsrBtSmlsMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSmlsFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT SMLS
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_SMLS_PRIM,
 *      msg:          The message received from Synergy BT SMLS
 *----------------------------------------------------------------------------*/
void CsrBtSmlsFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);



#ifdef __cplusplus
}
#endif

#endif
