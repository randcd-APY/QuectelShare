#ifndef CSR_BT_FTS_LIB_H__
#define CSR_BT_FTS_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2010 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_profiles.h"
#include "csr_bt_fts_prim.h"
#include "csr_bt_tasks.h"
#include "csr_pmem.h"
#include "csr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtFtsMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtsActivateReqSend
 *
 *  DESCRIPTION
 *      Set the File transfer server in discoverable mode and in connectable mode
 *
 *  PARAMETERS
 *        appHandle:        Application handle to receive signals
 *
 *      singleResponseModeSupported: If TRUE Single Response Mode is enabled
 *                                   (if possible) otherwise NOT. Information
 *                                   regarding Single Response Mode can be
 *                                   found in IrDA spec Version 1.4 section 3.3
 *----------------------------------------------------------------------------*/
#define CsrBtFtsActivateReqSend(_appHandle, _maxPacketSize, _windowSize, _srmEnable) { \
        CsrBtFtsActivateReq *msg__ = CsrPmemAlloc(sizeof(CsrBtFtsActivateReq)); \
        msg__->type = CSR_BT_FTS_ACTIVATE_REQ;                          \
        msg__->appHandle = _appHandle;                                  \
        msg__->obexMaxPacketSize = _maxPacketSize;                      \
        msg__->windowSize = _windowSize;                                \
        msg__->srmEnable = _srmEnable;                                  \
        CsrBtFtsMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtsDeactivateReqSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *        None.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtFtsDeactivateReqSend() {                                   \
        CsrBtFtsDeactivateReq *msg__ = CsrPmemAlloc(sizeof(CsrBtFtsDeactivateReq)); \
        msg__->type = CSR_BT_FTS_DEACTIVATE_REQ;                        \
        CsrBtFtsMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtsConnectResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *        obexMaxPacketSize:        ...
 *        connectionId:            ...
 *        responseCode:                    ...

 *----------------------------------------------------------------------------*/
#define CsrBtFtsConnectResSend(_theConnectionId, _theResponseCode) {    \
        CsrBtFtsConnectRes *msg__    = CsrPmemAlloc(sizeof(CsrBtFtsConnectRes)); \
        msg__->type                  = CSR_BT_FTS_CONNECT_RES;          \
        msg__->connectionId          = _theConnectionId;                \
        msg__->responseCode          = _theResponseCode;                \
        CsrBtFtsMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtsAuthenticateReqSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *        realmLength:        ...
 *        realm:    ...
 *        passwordLength:        ...
 *        password:    ...
 *        userId:        ...
 *----------------------------------------------------------------------------*/
#define CsrBtFtsAuthenticateReqSend(_realmLength, _realm, _passwordLength, _password, _userId) { \
        CsrBtFtsAuthenticateReq *msg__ = CsrPmemAlloc(sizeof(CsrBtFtsAuthenticateReq)); \
        msg__->type = CSR_BT_FTS_AUTHENTICATE_REQ;                      \
        msg__->realmLength = _realmLength;                              \
        msg__->realm = _realm;                                          \
        msg__->password = _password;                                    \
        msg__->passwordLength = _passwordLength;                        \
        msg__->userId = _userId;                                        \
        CsrBtFtsMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtsAuthenticateResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *        password:            ...
 *        passwordLength:        ...
 *        userId:                ...
 *----------------------------------------------------------------------------*/
#define CsrBtFtsAuthenticateResSend(_password, _passwordLength, _userId) { \
        CsrBtFtsAuthenticateRes *msg__ = CsrPmemAlloc(sizeof(CsrBtFtsAuthenticateRes)); \
        msg__->type = CSR_BT_FTS_AUTHENTICATE_RES;                      \
        msg__->password = _password;                                    \
        msg__->passwordLength = _passwordLength;                        \
        msg__->userId = _userId;                                        \
        CsrBtFtsMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtsPutObjResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *        connectionId:            ...
 *        responseCode:                    ...
 *----------------------------------------------------------------------------*/
#define CsrBtFtsPutObjResSend(_theConnectionId, _theResponseCode, _srmpOn) { \
        CsrBtFtsPutObjRes *msg__ = CsrPmemAlloc(sizeof(CsrBtFtsPutObjRes)); \
        msg__->type              = CSR_BT_FTS_PUT_OBJ_RES;              \
        msg__->connectionId      = _theConnectionId;                    \
        msg__->responseCode      = _theResponseCode;                    \
        msg__->srmpOn            = _srmpOn;                             \
        CsrBtFtsMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtsPutObjNextResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *        connectionId:            ...
 *        responseCode:                    ...
 *----------------------------------------------------------------------------*/
#define CsrBtFtsPutObjNextResSend(_theConnectionId, _theResponseCode, _srmpOn) { \
        CsrBtFtsPutObjNextRes *msg__ = CsrPmemAlloc(sizeof(CsrBtFtsPutObjNextRes)); \
        msg__->type                  = CSR_BT_FTS_PUT_OBJ_NEXT_RES;     \
        msg__->connectionId          = _theConnectionId;                \
        msg__->responseCode          = _theResponseCode;                \
        msg__->srmpOn                = _srmpOn;                         \
        CsrBtFtsMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtsDelObjResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *        connectionId:            ...
 *        responseCode:                    ...
 *----------------------------------------------------------------------------*/
#define CsrBtFtsDelObjResSend(_theConnectionId, _theResponseCode, _srmpOn) { \
        CsrBtFtsDelObjRes *msg__ = CsrPmemAlloc(sizeof(CsrBtFtsDelObjRes)); \
        msg__->type              = CSR_BT_FTS_DEL_OBJ_RES;              \
        msg__->connectionId      = _theConnectionId;                    \
        msg__->responseCode      = _theResponseCode;                    \
        msg__->srmpOn            = _srmpOn;                             \
        CsrBtFtsMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtsGetObjResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *        connectionId:            ...
 *        finalFlag:                ...
 *        responseCode:                    ...
 *        lengthOfObject:            ...
 *        bodyLength:             ...
 *        body:                    ...
 *----------------------------------------------------------------------------*/
#define CsrBtFtsGetObjResSend(_theConnectionId, _theFinalFlag, _theResponseCode, _theLengthOfObject, _theBodyLength, _theBody, _srmpOn) { \
        CsrBtFtsGetObjRes *msg__ = CsrPmemAlloc(sizeof(CsrBtFtsGetObjRes)); \
        msg__->type              = CSR_BT_FTS_GET_OBJ_RES;              \
        msg__->connectionId      = _theConnectionId;                    \
        msg__->finalFlag         = _theFinalFlag;                       \
        msg__->responseCode      = _theResponseCode;                    \
        msg__->lengthOfObject    = _theLengthOfObject;                  \
        msg__->bodyLength        = _theBodyLength;                      \
        msg__->body              = _theBody;                            \
        msg__->srmpOn            = _srmpOn;                             \
        CsrBtFtsMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtsGetObjNextResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *        connectionId:            ...
 *        finalFlag:                ...
 *        responseCode:                    ...
 *        bodyLength:                ...
 *        body:                    ...
 *----------------------------------------------------------------------------*/
#define CsrBtFtsGetObjNextResSend(_theConnectionId, _theFinalFlag, _theResponseCode, _theBodyLength, _theBody, _srmpOn) { \
        CsrBtFtsGetObjNextRes *msg__ = CsrPmemAlloc(sizeof(CsrBtFtsGetObjNextRes)); \
        msg__->type          = CSR_BT_FTS_GET_OBJ_NEXT_RES;             \
        msg__->connectionId  = _theConnectionId;                        \
        msg__->finalFlag     = _theFinalFlag;                           \
        msg__->responseCode  = _theResponseCode;                        \
        msg__->bodyLength    = _theBodyLength;                          \
        msg__->body          = _theBody;                                \
        msg__->srmpOn        = _srmpOn;                                 \
        CsrBtFtsMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtsGetListFolderResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *        connectionId:            ...
 *        finalFlag:               ...
 *        responseCode:                  ...
 *        lengthOfObject:          ...
 *        bodyLength:              ...
 *        body:                    ...
 *----------------------------------------------------------------------------*/
#define CsrBtFtsGetListFolderResSend(_theConnectionId, _theFinalFlag, _theResponseCode, _theLengthOfObject, _theBodyLength, _theBody, _srmpOn) { \
        CsrBtFtsGetListFolderRes    *msg__ = CsrPmemAlloc(sizeof(CsrBtFtsGetListFolderRes)); \
        msg__->type              = CSR_BT_FTS_GET_LIST_FOLDER_RES;      \
        msg__->connectionId      = _theConnectionId;                    \
        msg__->finalFlag         = _theFinalFlag;                       \
        msg__->responseCode      = _theResponseCode;                    \
        msg__->lengthOfObject    = _theLengthOfObject;                  \
        msg__->bodyLength        = _theBodyLength;                      \
        msg__->body              = _theBody;                            \
        msg__->srmpOn            = _srmpOn;                             \
        CsrBtFtsMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtsGetListFolderNextResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *        connectionId:                ...
 *        finalFlag:                   ...
 *        responseCode:                      ...
 *        bodyLength:                  ...
 *        body:                        ...
 *----------------------------------------------------------------------------*/
#define CsrBtFtsGetListFolderNextResSend(_theConnectionId, _theFinalFlag, _theResponseCode, _theBodyLength, _theBody, _srmpOn) { \
        CsrBtFtsGetListFolderNextRes *msg__ = CsrPmemAlloc(sizeof(CsrBtFtsGetListFolderNextRes)); \
        msg__->type          = CSR_BT_FTS_GET_LIST_FOLDER_NEXT_RES;     \
        msg__->connectionId  = _theConnectionId;                        \
        msg__->finalFlag     = _theFinalFlag;                           \
        msg__->responseCode  = _theResponseCode;                        \
        msg__->bodyLength    = _theBodyLength;                          \
        msg__->body          = _theBody;                                \
        msg__->srmpOn        = _srmpOn;                                 \
        CsrBtFtsMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtsSetFolderResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *        connectionId:            ...
 *        responseCode:                  ...
 *----------------------------------------------------------------------------*/
#define CsrBtFtsSetFolderResSend(_theConnectionId, _theResponseCode) {  \
        CsrBtFtsSetFolderRes *msg__ = CsrPmemAlloc(sizeof(CsrBtFtsSetFolderRes)); \
        msg__->type         = CSR_BT_FTS_SET_FOLDER_RES;                \
        msg__->connectionId = _theConnectionId;                         \
        msg__->responseCode = _theResponseCode;                         \
        CsrBtFtsMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtsSetBackFolderResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *        connectionId:            ...
 *        responseCode:                  ...
 *----------------------------------------------------------------------------*/
#define CsrBtFtsSetBackFolderResSend(_theConnectionId, _theResponseCode) { \
        CsrBtFtsSetBackFolderRes *msg__ = CsrPmemAlloc(sizeof(CsrBtFtsSetBackFolderRes)); \
        msg__->type          = CSR_BT_FTS_SET_BACK_FOLDER_RES;          \
        msg__->connectionId  = _theConnectionId;                        \
        msg__->responseCode  = _theResponseCode;                        \
        CsrBtFtsMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtsSetRootFolderResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *        connectionId:            ...
 *----------------------------------------------------------------------------*/
#define CsrBtFtsSetRootFolderResSend(_theConnectionId) {                \
        CsrBtFtsSetRootFolderRes *msg__ = CsrPmemAlloc(sizeof(CsrBtFtsSetRootFolderRes)); \
        msg__->type = CSR_BT_FTS_SET_ROOT_FOLDER_RES;                   \
        msg__->connectionId = _theConnectionId;                         \
        CsrBtFtsMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtsSetAddFolderResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *        connectionId:            ...
 *        responseCode:                  ...
 *----------------------------------------------------------------------------*/
#define CsrBtFtsSetAddFolderResSend(_theConnectionId, _theResponseCode) { \
        CsrBtFtsSetAddFolderRes *msg__ = CsrPmemAlloc(sizeof(CsrBtFtsSetAddFolderRes)); \
        msg__->type         = CSR_BT_FTS_SET_ADD_FOLDER_RES;            \
        msg__->connectionId = _theConnectionId;                         \
        msg__->responseCode = _theResponseCode;                         \
        CsrBtFtsMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtsSecurityInReqSend
 *
 *  DESCRIPTION
 *      Set the default security settings for new incoming/outgoing connections
 *
 *  PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtFtsSecurityInReqSend(_appHandle, _secLevel) {              \
        CsrBtFtsSecurityInReq *msg;                                     \
        msg = (CsrBtFtsSecurityInReq*)CsrPmemAlloc(sizeof(CsrBtFtsSecurityInReq)); \
        msg->type = CSR_BT_FTS_SECURITY_IN_REQ;                         \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        CsrBtFtsMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtsCopyObjResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *        connectionId:            ...
 *        responseCode:                  ...
 *----------------------------------------------------------------------------*/
#define CsrBtFtsCopyObjResSend(_connectionId, _responseCode) {          \
        CsrBtFtsCopyObjRes *msg__ = (CsrBtFtsCopyObjRes *) CsrPmemAlloc(sizeof(CsrBtFtsCopyObjRes)); \
        msg__->type         = CSR_BT_FTS_COPY_OBJ_RES;                  \
        msg__->connectionId = _connectionId;                            \
        msg__->responseCode = _responseCode;                            \
        CsrBtFtsMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtsMoveObjResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *        connectionId:            ...
 *        responseCode:                  ...
 *----------------------------------------------------------------------------*/
#define CsrBtFtsMoveObjResSend(_connectionId, _responseCode) {          \
        CsrBtFtsMoveObjRes *msg__ = (CsrBtFtsMoveObjRes *) CsrPmemAlloc(sizeof(CsrBtFtsMoveObjRes)); \
        msg__->type         = CSR_BT_FTS_MOVE_OBJ_RES;                  \
        msg__->connectionId = _connectionId;                            \
        msg__->responseCode = _responseCode;                            \
        CsrBtFtsMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtsSetObjPermissionsResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *        connectionId:            ...
 *        responseCode:                  ...
 *----------------------------------------------------------------------------*/
#define CsrBtFtsSetObjPermissionsResSend(_connectionId, _responseCode) { \
        CsrBtFtsSetObjPermissionsRes *msg__ = (CsrBtFtsSetObjPermissionsRes *) CsrPmemAlloc(sizeof(CsrBtFtsSetObjPermissionsRes)); \
        msg__->type         = CSR_BT_FTS_SET_OBJ_PERMISSIONS_RES;       \
        msg__->connectionId = _connectionId;                            \
        msg__->responseCode = _responseCode;                            \
        CsrBtFtsMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtsFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT FTS
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_FTS_PRIM,
 *      msg:          The message received from Synergy BT FTS
 *----------------------------------------------------------------------------*/
void CsrBtFtsFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);


#ifdef __cplusplus
}
#endif

#endif /* _FTS_PRIM_H */
