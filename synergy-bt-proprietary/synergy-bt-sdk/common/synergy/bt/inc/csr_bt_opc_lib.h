#ifndef CSR_BT_OPC_LIB_H__
#define CSR_BT_OPC_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2010-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_bt_profiles.h"
#include "csr_bt_opc_prim.h"
#include "csr_bt_tasks.h"
#include "csr_pmem.h"
#include "csr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtOpcMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtOpcConnectReqSend
 *      CsrBtOpcConnectWithAuthReqSend
 *
 *  DESCRIPTION
 *      Try to make a connection the a peer device.
 *
 *    PARAMETERS
 *        appHandle:            ...
 *        maxPacketSize:        ...
 *        destination:        ...
 *        length:               Length is used to express the approximate total
 *                              length of the bodies of all the objects in the
 *                              transaction
 *        count:                Count is used to indicate the number of objects
 *                              that will be sent during this connection
 *
 *      singleResponseModeSupported: If TRUE Single Response Mode is enabled
 *                                   (if possible) otherwise NOT. Information
 *                                   regarding Single Response Mode can be
 *                                   found in IrDA spec Version 1.4 section 3.3
 *----------------------------------------------------------------------------*/
#define CsrBtOpcConnectReqSend(_theAppHandle, _theMaxPacketSize, _theDestination, _length, _count, _windowSize, _srmEnable){ \
        CsrBtOpcConnectReq *msg = (CsrBtOpcConnectReq *) CsrPmemAlloc(sizeof(CsrBtOpcConnectReq)); \
        msg->type               = CSR_BT_OPC_CONNECT_REQ;               \
        msg->appHandle          = _theAppHandle;                        \
        msg->maxPacketSize      = _theMaxPacketSize;                    \
        msg->deviceAddr         = _theDestination;                      \
        msg->length             = _length;                              \
        msg->count              = _count;                               \
        msg->realmLength        = 0;                                    \
        msg->realm              = NULL;                                 \
        msg->passwordLength     = 0;                                    \
        msg->password           = NULL;                                 \
        msg->userId             = NULL;                                 \
        msg->windowSize         = _windowSize;                          \
        msg->srmEnable          = _srmEnable;                           \
        CsrBtOpcMsgTransport(msg);}

#ifdef CSR_BT_OBEX_AUTH_OPC_NON_SPEC_COMPLIANT_TEST_DO_NOT_USE
#define CsrBtOpcConnectWithAuthReqSend(_theAppHandle, _theMaxPacketSize, _theDestination, _length, _count, _realmLength, _realm, _passwordLength, _password, _userId, _windowSize, _srmEnable){ \
        CsrBtOpcConnectReq *msg = (CsrBtOpcConnectReq  *) CsrPmemAlloc(sizeof(CsrBtOpcConnectReq)); \
        msg->type               = CSR_BT_OPC_CONNECT_REQ;               \
        msg->appHandle          = _theAppHandle;                        \
        msg->maxPacketSize      = _theMaxPacketSize;                    \
        msg->deviceAddr         = _theDestination;                      \
        msg->length             = _length;                              \
        msg->count              = _count;                               \
        msg->realmLength        = _realmLength;                         \
        msg->realm              = _realm;                               \
        msg->passwordLength     = _passwordLength;                      \
        msg->password           = _password;                            \
        msg->userId             = _userId;                              \
        msg->windowSize         = _windowSize;                          \
        msg->srmEnable          = _srmEnable;                           \
        CsrBtOpcMsgTransport(msg);}
#endif /* CSR_BT_OBEX_AUTH_OPC_NON_SPEC_COMPLIANT_TEST_DO_NOT_USE */

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtOpcPutReqSend
 *
 *  DESCRIPTION
 *      Start pushing an object to a peer device
 *
 *    PARAMETERS
 *        bodyType:            Ascii string with body MIME type
 *        bodyTypeLength:      Length of bodyType
 *        name:                ...
 *        nameLength:          Length of name
 *----------------------------------------------------------------------------*/
#define CsrBtOpcPutReqSend(_theBodyType, _theBodyTypeLength, _theLengthOfObject, _theName){ \
        CsrBtOpcPutReq *msg = (CsrBtOpcPutReq *) CsrPmemAlloc(sizeof(CsrBtOpcPutReq)); \
        msg->type           = CSR_BT_OPC_PUT_REQ;                       \
        msg->lengthOfObject = _theLengthOfObject;                       \
        msg->ucs2name       = _theName;                                 \
        msg->bodyTypeLength = _theBodyTypeLength;                       \
        msg->bodyType       = (CsrUint8 *)_theBodyType;                 \
        if(_theBodyTypeLength > 0 && _theBodyType)                      \
        {                                                               \
            msg->bodyType[_theBodyTypeLength-1] = '\0';                 \
        }                                                               \
        CsrBtOpcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtOpcPutObjectResSend
 *
 *  DESCRIPTION
 *      pushing next piece of an object to a peer device
 *
 *    PARAMETERS
 *        finalFlag:            ...
 *        objectLength:        ...
 *        object:                ...
 *----------------------------------------------------------------------------*/
#define CsrBtOpcPutObjectResSend(_theFinalFlag, _theObjectlength, _theObject){ \
        CsrBtOpcPutObjectRes *msg = (CsrBtOpcPutObjectRes  *) CsrPmemAlloc(sizeof(CsrBtOpcPutObjectRes)); \
        msg->type                 = CSR_BT_OPC_PUT_OBJECT_RES;          \
        msg->finalFlag            = _theFinalFlag;                      \
        msg->object               = _theObject;                         \
        msg->objectLength         = _theObjectlength;                   \
        CsrBtOpcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtOpcGetHeaderReqSend
 *
 *  DESCRIPTION
 *      Request to get an object
 *
 *    PARAMETERS
 *        bodyType:            ...
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_OPC_GET
#define CsrBtOpcGetHeaderReqSend(_theBodyType, _srmpOn){                \
        CsrBtOpcGetHeaderReq *msg = (CsrBtOpcGetHeaderReq *) CsrPmemAlloc(sizeof(CsrBtOpcGetHeaderReq)); \
        msg->type                 = CSR_BT_OPC_GET_HEADER_REQ;          \
        msg->bodyType             = _theBodyType;                       \
        msg->srmpOn               = _srmpOn;                            \
        CsrBtOpcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtOpcGetObjectReqSend
 *
 *  DESCRIPTION
 *      Request to get the next piece of an object
 *
 *    PARAMETERS
 *        NONE
 *----------------------------------------------------------------------------*/
#define CsrBtOpcGetObjectReqSend(_srmpOn){                              \
        CsrBtOpcGetObjectReq *msg = (CsrBtOpcGetObjectReq *) CsrPmemAlloc(sizeof(CsrBtOpcGetObjectReq)); \
        msg->type                 = CSR_BT_OPC_GET_OBJECT_REQ;          \
        msg->srmpOn               = _srmpOn;                            \
        CsrBtOpcMsgTransport(msg);}
#endif
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtOpcAbortReqSend
 *
 *  DESCRIPTION
 *      Cancels the current operation
 *
 *    PARAMETERS
 *        NONE
 *----------------------------------------------------------------------------*/
#define CsrBtOpcAbortReqSend(){                                         \
        CsrBtOpcAbortReq *msg = (CsrBtOpcAbortReq *) CsrPmemAlloc(sizeof(CsrBtOpcAbortReq)); \
        msg->type             = CSR_BT_OPC_ABORT_REQ;                   \
        CsrBtOpcMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtOpcDisconnectReqSend
 *
 *  DESCRIPTION
 *      The OBEX - and the Bluetooth connection is release
 *
 *    PARAMETERS
 *        theNormalDisconnect :    FALSE defines an Abnormal disconnect sequence where
 *                                the Bluetooth connection is release direct. TRUE
 *                                defines a normal disconnect sequence where the OBEX
 *                                connection is release before the Bluetooth connection
 *----------------------------------------------------------------------------*/
#define CsrBtOpcDisconnectReqSend(_theNormalDisconnect){                \
        CsrBtOpcDisconnectReq *msg = (CsrBtOpcDisconnectReq *) CsrPmemAlloc(sizeof(CsrBtOpcDisconnectReq)); \
        msg->type                  = CSR_BT_OPC_DISCONNECT_REQ;         \
        msg->normalDisconnect      = _theNormalDisconnect;              \
        CsrBtOpcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtOpcCancelConnectReqSend
 *
 *  DESCRIPTION
 *      Try to cancel the connection which is being establish to a peer device.
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtOpcCancelConnectReqSend() {                                \
        CsrBtOpcCancelConnectReq *msg = (CsrBtOpcCancelConnectReq *) CsrPmemAlloc(sizeof(CsrBtOpcCancelConnectReq)); \
        msg->type                     = CSR_BT_OPC_CANCEL_CONNECT_REQ;  \
        CsrBtOpcMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtOpcSecurityOutReqSend
 *
 *  DESCRIPTION
 *      Set the default security settings for new incoming/outgoing connections
 *
 *  PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtOpcSecurityOutReqSend(_appHandle, _secLevel) {             \
        CsrBtOpcSecurityOutReq *msg = (CsrBtOpcSecurityOutReq*)CsrPmemAlloc(sizeof(CsrBtOpcSecurityOutReq)); \
        msg->type                   = CSR_BT_OPC_SECURITY_OUT_REQ;      \
        msg->appHandle              = _appHandle;                       \
        msg->secLevel               = _secLevel;                        \
        CsrBtOpcMsgTransport(msg);}

#ifdef CSR_BT_OBEX_AUTH_OPC_NON_SPEC_COMPLIANT_TEST_DO_NOT_USE
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtOpcAuthenticateResSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        password:          ...
 *        passwordLength:    ...
 *        userId:            ...
 *----------------------------------------------------------------------------*/
#define CsrBtOpcAuthenticateResSend(_authPassword, _authPasswordLength, _authUserId) { \
        CsrBtOpcAuthenticateRes *msg__ = (CsrBtOpcAuthenticateRes *) CsrPmemAlloc(sizeof(CsrBtOpcAuthenticateRes)); \
        msg__->type                    = CSR_BT_OPC_AUTHENTICATE_RES;   \
        msg__->authPassword            = _authPassword;                 \
        msg__->authPasswordLength      = _authPasswordLength;           \
        msg__->authUserId              = _authUserId;                   \
        msg__->chalRealmLength         = 0;                             \
        msg__->chalRealm               = NULL;                          \
        msg__->chalPasswordLength      = 0;                             \
        msg__->chalPassword            = NULL;                          \
        msg__->chalUserId              = NULL;                          \
        CsrBtOpcMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtOpcAuthenticateWithChalResSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        password:          ...
 *        passwordLength:    ...
 *        userId:            ...
 *----------------------------------------------------------------------------*/
#define CsrBtOpcAuthenticateWithChalResSend(_authPassword, _authPasswordLength, _authUserId, _chalRealmLength, _chalRealm, _chalPasswordLength, _chalPassword, _chalUserId) { \
        CsrBtOpcAuthenticateRes *msg__ = (CsrBtOpcAuthenticateRes *) CsrPmemAlloc(sizeof(CsrBtOpcAuthenticateRes)); \
        msg__->type                    = CSR_BT_OPC_AUTHENTICATE_RES;   \
        msg__->authPassword            = _authPassword;                 \
        msg__->authPasswordLength      = _authPasswordLength;           \
        msg__->authUserId              = _authUserId;                   \
        msg__->chalRealmLength         = _chalRealmLength;              \
        msg__->chalRealm               = _chalRealm;                    \
        msg__->chalPasswordLength      = _chalPasswordLength;           \
        msg__->chalPassword            = _chalPassword;                 \
        msg__->chalUserId              = _chalUserId;                   \
        CsrBtOpcMsgTransport(msg__);}

#endif /* CSR_BT_OBEX_AUTH_OPC_NON_SPEC_COMPLIANT_TEST_DO_NOT_USE */

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtOpcFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT OPC
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_OPC_PRIM,
 *      msg:          The message received from Synergy BT OPC
 *----------------------------------------------------------------------------*/
void CsrBtOpcFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);



#ifdef __cplusplus
}
#endif

#endif /* _OPC_PRIM_H */

