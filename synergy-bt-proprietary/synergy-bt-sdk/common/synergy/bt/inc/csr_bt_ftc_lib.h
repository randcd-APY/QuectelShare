#ifndef CSR_BT_FTC_LIB_H__
#define CSR_BT_FTC_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2010 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_profiles.h"
#include "csr_bt_ftc_prim.h"
#include "csr_bt_tasks.h"
#include "csr_pmem.h"
#include "csr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtFtcMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtcConnectReqSend
 *
 *  DESCRIPTION
 *      Try to make a connection the a peer device.
 *
 *    PARAMETERS
 *        appHandle:        ...
 *        maxPacketSize:    ...
 *        destination:    ...
 *        authorise        ...
 *        reamlLength:       ...
 *        reaml:             ...
 *        passwordLength:    ...
 *        password:          ...
 *        userId:            ...
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
#define CsrBtFtcConnectReqSend(_theAppHandle, _theMaxPacketSize, _theDestination, _authorise, _realmLength, _realm, _passwordLength, _password, _userId, _length, _count, _windowSize, _srmEnable) { \
        CsrBtFtcConnectReq *msg__  = (CsrBtFtcConnectReq *) CsrPmemAlloc(sizeof(CsrBtFtcConnectReq)); \
        msg__->type              = CSR_BT_FTC_CONNECT_REQ;              \
        msg__->appHandle         = _theAppHandle;                       \
        msg__->maxPacketSize     = _theMaxPacketSize;                   \
        msg__->destination       = _theDestination;                     \
        msg__->authorize         = _authorise;                          \
        msg__->realmLength       = _realmLength;                        \
        msg__->realm             = _realm;                              \
        msg__->password          = _password;                           \
        msg__->passwordLength    = _passwordLength;                     \
        msg__->userId            = _userId;                             \
        msg__->length            = _length;                             \
        msg__->count             = _count;                              \
        msg__->windowSize        = _windowSize;                         \
        msg__->srmEnable         = _srmEnable;                          \
        CsrBtFtcMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtcAuthenticateResSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        password:          ...
 *        passwordLength:    ...
 *        userId:            ...
 *----------------------------------------------------------------------------*/
#define CsrBtFtcAuthenticateResSend(_password, _passwordLength, _userId) { \
        CsrBtFtcAuthenticateRes *msg__ = (CsrBtFtcAuthenticateRes *) CsrPmemAlloc(sizeof(CsrBtFtcAuthenticateRes)); \
        msg__->type              = CSR_BT_FTC_AUTHENTICATE_RES;         \
        msg__->password          = _password;                           \
        msg__->passwordLength    = _passwordLength;                     \
        msg__->userId            = _userId;                             \
        CsrBtFtcMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtcGetListFolderReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        name:          ...
 *----------------------------------------------------------------------------*/
#define CsrBtFtcGetListFolderReqSend(_theName, _srmpOn) {               \
        CsrBtFtcGetListFolderReq *msg__ = (CsrBtFtcGetListFolderReq *) CsrPmemAlloc(sizeof(CsrBtFtcGetListFolderReq)); \
        msg__->type              = CSR_BT_FTC_GET_LIST_FOLDER_REQ;      \
        msg__->ucs2name          = _theName;                            \
        msg__->srmpOn            = _srmpOn;                             \
        CsrBtFtcMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtcGetListFolderBodyReqSend
 *
 *  DESCRIPTION
 *      .....
 *
 *    PARAMETERS
 *        None
 *----------------------------------------------------------------------------*/
#define CsrBtFtcGetListFolderBodyReqSend(_srmpOn) {                     \
        CsrBtFtcGetListFolderBodyReq *msg__ = (CsrBtFtcGetListFolderBodyReq *) CsrPmemAlloc(sizeof(CsrBtFtcGetListFolderBodyReq)); \
        msg__->type              = CSR_BT_FTC_GET_LIST_FOLDER_BODY_REQ; \
        msg__->srmpOn            = _srmpOn;                             \
        CsrBtFtcMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      FtcGetObjHeaderReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        name:           ...
 *        nameLength:     Length of name
 *----------------------------------------------------------------------------*/
#define CsrBtFtcGetObjReqSend(_theName, _srmpOn) {                      \
        CsrBtFtcGetObjReq *msg__ = (CsrBtFtcGetObjReq *) CsrPmemAlloc(sizeof(CsrBtFtcGetObjReq)); \
        msg__->type              = CSR_BT_FTC_GET_OBJ_REQ;              \
        msg__->ucs2name          = _theName;                            \
        msg__->srmpOn            = _srmpOn;                             \
        CsrBtFtcMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtcGetObjBodyReqSend
 *
 *  DESCRIPTION
 *      .....
 *
 *    PARAMETERS
 *                    None
 *----------------------------------------------------------------------------*/
#define CsrBtFtcGetObjBodyReqSend(_srmpOn) {                            \
        CsrBtFtcGetObjBodyReq *msg__ = (CsrBtFtcGetObjBodyReq *) CsrPmemAlloc(sizeof(CsrBtFtcGetObjBodyReq)); \
        msg__->type              = CSR_BT_FTC_GET_OBJ_BODY_REQ;         \
        msg__->srmpOn            = _srmpOn;                             \
        CsrBtFtcMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtcPutObjHeaderReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        name:                ...
 *        nameLength:          Length of name
 *        lengthOfObject:      ...
 *----------------------------------------------------------------------------*/
#define CsrBtFtcPutObjHeaderReqSend(_theName, _theLengthOfObject) {     \
        CsrBtFtcPutObjHeaderReq *msg__ = (CsrBtFtcPutObjHeaderReq *) CsrPmemAlloc(sizeof(CsrBtFtcPutObjHeaderReq)); \
        msg__->type              = CSR_BT_FTC_PUT_OBJ_HEADER_REQ;       \
        msg__->lengthOfObject    = _theLengthOfObject;                  \
        msg__->ucs2name          = _theName;                            \
        CsrBtFtcMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtcPutObjBodyReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        finalFlag:        ...
 *        bodyLength:        ...
 *        *body:            ...
 *----------------------------------------------------------------------------*/
#define CsrBtFtcPutObjBodyReqSend(_theFinalFlag, _theBodyLength, _theBody) { \
        CsrBtFtcPutObjBodyReq *msg__ = (CsrBtFtcPutObjBodyReq *) CsrPmemAlloc(sizeof(CsrBtFtcPutObjBodyReq)); \
        msg__->type              = CSR_BT_FTC_PUT_OBJ_BODY_REQ;         \
        msg__->finalFlag         = _theFinalFlag;                       \
        msg__->bodyLength        = _theBodyLength;                      \
        msg__->body              = _theBody;                            \
        CsrBtFtcMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtcDelObjReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        name:          ...
 *        nameLength:    Length of name
 *----------------------------------------------------------------------------*/
#define CsrBtFtcDelObjReqSend(_theName) {                               \
        CsrBtFtcDelObjReq *msg__ = (CsrBtFtcDelObjReq *) CsrPmemAlloc(sizeof(CsrBtFtcDelObjReq)); \
        msg__->type              = CSR_BT_FTC_DEL_OBJ_REQ;              \
        msg__->ucs2name          = _theName;                            \
        CsrBtFtcMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtcSetFolderReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        name:        ...
 *        nameLength:  Length of name
 *----------------------------------------------------------------------------*/
#define CsrBtFtcSetFolderReqSend(_theName) {                            \
        CsrBtFtcSetFolderReq *msg__ = (CsrBtFtcSetFolderReq *) CsrPmemAlloc(sizeof(CsrBtFtcSetFolderReq)); \
        msg__->type              = CSR_BT_FTC_SET_FOLDER_REQ;           \
        msg__->ucs2name          = _theName;                            \
        CsrBtFtcMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtcSetBackFolderReqSend
 *
 *  DESCRIPTION
 *      .....
 *
 *    PARAMETERS
 *        None
 *----------------------------------------------------------------------------*/
#define CsrBtFtcSetBackFolderReqSend() {                                \
        CsrBtFtcSetBackFolderReq *msg__ = (CsrBtFtcSetBackFolderReq*) CsrPmemAlloc(sizeof(CsrBtFtcSetBackFolderReq)); \
        msg__->type                     = CSR_BT_FTC_SET_BACK_FOLDER_REQ; \
        CsrBtFtcMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtcSetRootFolderReqSend
 *
 *  DESCRIPTION
 *      .....
 *
 *    PARAMETERS
 *        None
 *----------------------------------------------------------------------------*/
#define CsrBtFtcSetRootFolderReqSend() {                                \
        CsrBtFtcSetRootFolderReq *msg__ = (CsrBtFtcSetRootFolderReq*) CsrPmemAlloc(sizeof(CsrBtFtcSetRootFolderReq)); \
        msg__->type                = CSR_BT_FTC_SET_ROOT_FOLDER_REQ;    \
        CsrBtFtcMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtcSetAddFolderReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        name:        ...
 *        nameLength:  Length of name
 *----------------------------------------------------------------------------*/
#define CsrBtFtcSetAddFolderReqSend(_theName) {                         \
        CsrBtFtcSetAddFolderReq *msg__ = (CsrBtFtcSetAddFolderReq *) CsrPmemAlloc(sizeof(CsrBtFtcSetAddFolderReq)); \
        msg__->type              = CSR_BT_FTC_SET_ADD_FOLDER_REQ;       \
        msg__->ucs2name          = _theName;                            \
        CsrBtFtcMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtcAbortReqSend
 *
 *  DESCRIPTION
 *      .....
 *
 *    PARAMETERS
 *        None
 *----------------------------------------------------------------------------*/
#define CsrBtFtcAbortReqSend() {                                        \
        CsrBtFtcAbortReq *msg__ = (CsrBtFtcAbortReq *) CsrPmemAlloc(sizeof(CsrBtFtcAbortReq)); \
        msg__->type             = CSR_BT_FTC_ABORT_REQ;                 \
        CsrBtFtcMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtcDisconnectReqSend
 *
 *  DESCRIPTION
 *      The OBEX - and the Bluetooth connection is release
 *
 *    PARAMETERS
 *        theNormalDisconnect : FALSE defines an Abnormal disconnect sequence where
 *                              the Bluetooth connection is release direct. TRUE
 *                              defines a normal disconnect sequence where the OBEX
 *                              connection is release before the Bluetooth connection
 *----------------------------------------------------------------------------*/
#define CsrBtFtcDisconnectReqSend(_theNormalDisconnect) {               \
        CsrBtFtcDisconnectReq *msg__ = (CsrBtFtcDisconnectReq *) CsrPmemAlloc(sizeof(CsrBtFtcDisconnectReq)); \
        msg__->type                = CSR_BT_FTC_DISCONNECT_REQ;         \
        msg__->normalDisconnect    = _theNormalDisconnect;              \
        CsrBtFtcMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtcCancelConnectReqSend
 *
 *  DESCRIPTION
 *      Try to cancel the connection which is being establish to a peer device.
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtFtcCancelConnectReqSend() {                                \
        CsrBtFtcCancelConnectReq *msg__ = (CsrBtFtcCancelConnectReq *) CsrPmemAlloc(sizeof(CsrBtFtcCancelConnectReq)); \
        msg__->type    = CSR_BT_FTC_CANCEL_CONNECT_REQ;                 \
        CsrBtFtcMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtcSecurityOutReqSend
 *
 *  DESCRIPTION
 *      Set the default security settings for new incoming/outgoing connections
 *
 *  PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtFtcSecurityOutReqSend(_appHandle, _secLevel) {             \
        CsrBtFtcSecurityOutReq *msg = (CsrBtFtcSecurityOutReq*)CsrPmemAlloc(sizeof(CsrBtFtcSecurityOutReq)); \
        msg->type = CSR_BT_FTC_SECURITY_OUT_REQ;                        \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        CsrBtFtcMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtcCopyingObjReqSend
 *
 *  DESCRIPTION
 *      Copying a file or folder
 *
 *    PARAMETERS
 *        srcName:      The name of the object (file or folder) to be copied.
 *        destName:     The name of the destination object (file or folder)
 *----------------------------------------------------------------------------*/
#define CsrBtFtcCopyingObjReqSend(_srcName, _destName) {                \
        CsrBtFtcCopyingObjReq *msg__ = (CsrBtFtcCopyingObjReq *) CsrPmemAlloc(sizeof(CsrBtFtcCopyingObjReq)); \
        msg__->type          = CSR_BT_FTC_COPYING_OBJ_REQ;              \
        msg__->ucs2SrcName   = _srcName;                                \
        msg__->ucs2DestName  = _destName;                               \
        CsrBtFtcMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtcMovingObjReqSend
 *
 *  DESCRIPTION
 *      Moving or Renaming a file or folder
 *
 *    PARAMETERS
 *        srcName:      The name of the file or folder on the Server to be moved or renamed.
 *        destName:     The new name or destination for the object (file or folder).
 *----------------------------------------------------------------------------*/
#define CsrBtFtcMovingObjReqSend(_srcName, _destName) {                 \
        CsrBtFtcMovingObjReq *msg__ = (CsrBtFtcMovingObjReq *) CsrPmemAlloc(sizeof(CsrBtFtcMovingObjReq)); \
        msg__->type          = CSR_BT_FTC_MOVING_OBJ_REQ;               \
        msg__->ucs2SrcName   = _srcName;                                \
        msg__->ucs2DestName  = _destName;                               \
        CsrBtFtcMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtcSetObjPermissionsReqSend
 *
 *  DESCRIPTION
 *      Setting permissions on a file or folder
 *
 *    PARAMETERS
 *        ucs2name:     The name of the file or folder for this operation.
 *        permissions:  The permissions to set.
 *----------------------------------------------------------------------------*/
#define CsrBtFtcSetObjPermissionsReqSend(_ucs2name, _permissions) {     \
        CsrBtFtcSetObjPermissionsReq *msg__ = (CsrBtFtcSetObjPermissionsReq *) CsrPmemAlloc(sizeof(CsrBtFtcSetObjPermissionsReq)); \
        msg__->type          = CSR_BT_FTC_SET_OBJ_PERMISSIONS_REQ;      \
        msg__->ucs2name      = _ucs2name;                               \
        msg__->permissions   = _permissions;                            \
        CsrBtFtcMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtFtcFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT FTC
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_FTC_PRIM,
 *      msg:          The message received from Synergy BT FTC
 *----------------------------------------------------------------------------*/
void CsrBtFtcFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);


#ifdef __cplusplus
}
#endif

#endif /* _FTC_PRIM_H */
