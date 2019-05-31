#ifndef CSR_BT_OPS_LIB_H__
#define CSR_BT_OPS_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2010 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_util.h"
#include "csr_bt_profiles.h"
#include "csr_bt_ops_prim.h"
#include "csr_bt_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtOpsMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtOpsActivateReqSend
 *
 *  DESCRIPTION
 *      Set the Object Push server in discoverable mode and in connectable mode
 *
 *    PARAMETERS
 *        appHandle:
 *        supportedFormats:                ...
 *
 *      singleResponseModeSupported: If TRUE Single Response Mode is enabled
 *                                   (if possible) otherwise NOT. Information
 *                                   regarding Single Response Mode can be
 *                                   found in IrDA spec Version 1.4 section 3.3
 *----------------------------------------------------------------------------*/
#define CsrBtOpsActivateReqSend(_appHandle, _theSupportedFormats, _obexMaxPacketSize, _windowSize, _srmEnable){ \
        CsrBtOpsActivateReq *msg = CsrPmemAlloc(sizeof(CsrBtOpsActivateReq)); \
        msg->type = CSR_BT_OPS_ACTIVATE_REQ;                            \
        msg->appHandle  = _appHandle;                                   \
        msg->supportedFormats = _theSupportedFormats;                   \
        msg->obexMaxPacketSize = _obexMaxPacketSize;                    \
        msg->windowSize = _windowSize;                                  \
        msg->srmEnable = _srmEnable;                                    \
        CsrBtOpsMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtOpsPutResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *    PARAMETERS
 *        responseCode:                            ...
 *----------------------------------------------------------------------------*/
#define CsrBtOpsPutResSend(_responseCode, _srmpOn){                     \
        CsrBtOpsPutRes *msg = CsrPmemAlloc(sizeof(CsrBtOpsPutRes));     \
        msg->type         = CSR_BT_OPS_PUT_RES;                         \
        msg->responseCode = _responseCode;                              \
        msg->srmpOn       = _srmpOn;                                    \
        CsrBtOpsMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtOpsPutNextResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *    PARAMETERS
 *        responseCode:                            ...

 *----------------------------------------------------------------------------*/
#define CsrBtOpsPutNextResSend(_responseCode, _srmpOn){                 \
        CsrBtOpsPutNextRes *msg = CsrPmemAlloc(sizeof(CsrBtOpsPutNextRes)); \
        msg->type          = CSR_BT_OPS_PUT_NEXT_RES;                   \
        msg->responseCode  = _responseCode;                             \
        msg->srmpOn        = _srmpOn;                                   \
        CsrBtOpsMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtOpsGetResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *    PARAMETERS
 *    finalFlag:              ...
 *    responseCode:                 ...
 *    bodyType:               ...
 *    lengthOfObject:         ...
 *    name:                   ...
 *    bodyLength:             ...
 *    body                    ...
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_OPS_GET
#define CsrBtOpsGetResSend(_theFinalFlag, _responseCode, _theBodyType, _theTotalObjectSize, _theName, _theBodyLength, _theBody, _srmpOn){ \
        CsrBtOpsGetRes *msg = CsrPmemAlloc(sizeof(CsrBtOpsGetRes));     \
        msg->type            = CSR_BT_OPS_GET_RES;                      \
        msg->finalFlag       = _theFinalFlag;                           \
        msg->responseCode    = _responseCode;                           \
        msg->bodyType        = _theBodyType;                            \
        msg->totalObjectSize = _theTotalObjectSize;                     \
        msg->ucs2name        = _theName;                                \
        msg->body            = _theBody;                                \
        msg->bodyLength      = (CsrUint16)_theBodyLength;               \
        msg->srmpOn          = _srmpOn;                                 \
        CsrBtOpsMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtOpsGetNextResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *    PARAMETERS
 *    finalFlag:                ...
 *    responseCode:                    ...
 *    bodyLength:                    ...
 *    body                    ...
 *----------------------------------------------------------------------------*/
#define CsrBtOpsGetNextResSend(_theFinalFlag, _responseCode, _theBodyLength, _theBody, _srmpOn){ \
        CsrBtOpsGetNextRes *msg = CsrPmemAlloc(sizeof(CsrBtOpsGetNextRes)); \
        msg->type          = CSR_BT_OPS_GET_NEXT_RES;                   \
        msg->finalFlag     = _theFinalFlag;                             \
        msg->responseCode  = _responseCode;                             \
        msg->body          = _theBody;                                  \
        msg->bodyLength    = _theBodyLength;                            \
        msg->srmpOn        = _srmpOn;                                   \
        CsrBtOpsMsgTransport(msg);}
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtOpsDeactivateReqSend
 *
 *  DESCRIPTION
 *      ....
 *
 *    PARAMETERS
 *    None.

 *----------------------------------------------------------------------------*/
#define CsrBtOpsDeactivateReqSend(){                                    \
        CsrBtOpsDeactivateReq *msg = CsrPmemAlloc(sizeof(CsrBtOpsDeactivateReq)); \
        msg->type = CSR_BT_OPS_DEACTIVATE_REQ;                          \
        CsrBtOpsMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtOpsConnectResSend
 *      CsrBtOpsConnectExtAuthResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *    PARAMETERS
 *    obexMaxPacketSize:                ...
 *    responseCode:                    ...
 *----------------------------------------------------------------------------*/
#ifndef CSR_BT_OBEX_AUTH_OPS_NON_SPEC_COMPLIANT_TEST_DO_NOT_USE
#define CsrBtOpsConnectResSend(_responseCode){                          \
        CsrBtOpsConnectRes *msg    = CsrPmemAlloc(sizeof(CsrBtOpsConnectRes)); \
        msg->type                  = CSR_BT_OPS_CONNECT_RES;            \
        msg->responseCode          = _responseCode;                     \
        CsrBtOpsMsgTransport(msg);}

#else
#define CsrBtOpsConnectExtAuthResSend(_realmLength, _realm, _passwordLength, _password, _userId, _authenticateResponse){ \
        CsrBtOpsConnectRes *msg    = CsrPmemAlloc(sizeof(CsrBtOpsConnectRes)); \
        msg->type                  = CSR_BT_OPS_CONNECT_RES;            \
        msg->responseCode          = CSR_BT_OBEX_UNAUTHORIZED_CODE;     \
        msg->realmLength           = _realmLength;                      \
        msg->realm                 = _realm;                            \
        msg->passwordLength        = _passwordLength;                   \
        msg->password              = _password;                         \
        msg->userId                = _userId;                           \
        msg->authenticateResponse  = _authenticateResponse;             \
        CsrBtOpsMsgTransport(msg);}
#endif /* CSR_BT_OBEX_AUTH_OPS_NON_SPEC_COMPLIANT_TEST_DO_NOT_USE */


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtOpsSecurityInReqSend
 *
 *  DESCRIPTION
 *      Set the default security settings for new incoming/outgoing connections
 *
 *  PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtOpsSecurityInReqSend(_appHandle, _secLevel) {              \
        CsrBtOpsSecurityInReq *msg = (CsrBtOpsSecurityInReq*)CsrPmemAlloc(sizeof(CsrBtOpsSecurityInReq)); \
        msg->type = CSR_BT_OPS_SECURITY_IN_REQ;                         \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        CsrBtOpsMsgTransport(msg);}

#ifdef CSR_BT_OBEX_AUTH_OPS_NON_SPEC_COMPLIANT_TEST_DO_NOT_USE
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtOpsAuthenticateResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *        password:            ...
 *        passwordLength:        ...
 *        userId:                ...
 *----------------------------------------------------------------------------*/
#define CsrBtOpsAuthenticateResSend(_password, _passwordLength, _userId) { \
        CsrBtOpsAuthenticateRes *msg__ = CsrPmemAlloc(sizeof(CsrBtOpsAuthenticateRes)); \
        msg__->type = CSR_BT_OPS_AUTHENTICATE_RES;                      \
        msg__->password = _password;                                    \
        msg__->passwordLength = _passwordLength;                        \
        msg__->userId = _userId;                                        \
        CsrBtOpsMsgTransport( msg__);}
#endif /* CSR_BT_OBEX_AUTH_OPS_NON_SPEC_COMPLIANT_TEST_DO_NOT_USE */

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtOpsFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT Ops
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be Ops_PRIM,
 *      msg:          The message received from Synergy BT Ops
 *----------------------------------------------------------------------------*/
void CsrBtOpsFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);



#ifdef __cplusplus
}
#endif

#endif

