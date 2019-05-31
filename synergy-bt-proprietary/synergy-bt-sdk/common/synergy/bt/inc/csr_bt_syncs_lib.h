#ifndef CSR_BT_SYNCS_LIB_H__
#define CSR_BT_SYNCS_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2010 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_profiles.h"
#include "csr_bt_syncs_prim.h"
#include "csr_bt_tasks.h"
#include "csr_pmem.h"
#include "csr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtSyncsMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsActivateReqSend
 *
 *  DESCRIPTION
 *      Set the Sync server in discoverable mode and in connectable mode
 *
 *  PARAMETERS
 *    appHandle:
 *    supportedFormats:            ...

 *----------------------------------------------------------------------------*/
#define CsrBtSyncsActivateReqSend(_appHandle,_supportedFormats,_obexMaxPacketSize,_windowSize,_srmEnable){ \
        CsrBtSyncsActivateReq *msg = CsrPmemAlloc(sizeof(CsrBtSyncsActivateReq)); \
        msg->type = CSR_BT_SYNCS_ACTIVATE_REQ;                          \
        msg->appHandle = _appHandle;                                    \
        msg->supportedFormats = _supportedFormats;                      \
        msg->obexMaxPacketSize = _obexMaxPacketSize;                    \
        msg->windowSize = _windowSize;                                  \
        msg->srmEnable = _srmEnable;                                    \
        CsrBtSyncsMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsDeactivateReqSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *        None.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSyncsDeactivateReqSend(){                                  \
        CsrBtSyncsDeactivateReq *msg = CsrPmemAlloc(sizeof(CsrBtSyncsDeactivateReq)); \
        msg->type = CSR_BT_SYNCS_DEACTIVATE_REQ;                        \
        CsrBtSyncsMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsConnectResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *    obexMaxPacketSize:           ...
 *    connectionId:                ...
 *    responseCode:                      ...
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSyncsConnectResSend(_connectionId,_responseCode){          \
        CsrBtSyncsConnectRes *msg = CsrPmemAlloc(sizeof(CsrBtSyncsConnectRes)); \
        msg->type               = CSR_BT_SYNCS_CONNECT_RES;             \
        msg->connectionId       = _connectionId;                        \
        msg->responseCode       = _responseCode;                        \
        CsrBtSyncsMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsAuthenticateReqSend
 *
 *  DESCRIPTION
 *      This message may be used to request OBEX authentication of the client
 *      whenever it initiates a connection, i.e. as a response to a ConnectInd.
 *      It may only be used to initiate authentication on the connect!!!
 *
 *    PARAMETERS
 *        reamlLength:       ...
 *        reaml:             ...
 *        passwordLength:    ...
 *        password:          ...
 *        userId:            ...
 *----------------------------------------------------------------------------*/
#define CsrBtSyncsAuthenticateReqSend(_realmLength,_realm,_passwordLength,_password,_userId) { \
        CsrBtSyncsAuthenticateReq *msg__ = (CsrBtSyncsAuthenticateReq *) CsrPmemAlloc(sizeof(CsrBtSyncsAuthenticateReq)); \
        msg__->type                      = CSR_BT_SYNCS_AUTHENTICATE_REQ; \
        msg__->realm             = _realm;                              \
        msg__->realmLength       = _realmLength;                        \
        msg__->password          = _password;                           \
        msg__->passwordLength    = _passwordLength;                     \
        msg__->userId            = _userId;                             \
        CsrBtSyncsMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsAuthenticateResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *        passwordLength:    ...
 *        password:          ...
 *        userId:            ...
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSyncsAuthenticateResSend(_passwordLength,_password,_userId){ \
        CsrBtSyncsAuthenticateRes *msg = CsrPmemAlloc(sizeof(CsrBtSyncsAuthenticateRes)); \
        msg->type                         = CSR_BT_SYNCS_AUTHENTICATE_RES; \
        msg->password          = _password;                             \
        msg->passwordLength    = _passwordLength;                       \
        msg->userId            = _userId;                               \
        CsrBtSyncsMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsGetResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *    connectionId:                ...
 *    finalFlag:                   ...
 *    responseCode:                      ...
 *    lengthOfObject:              ...
 *    length:                      Length of body
 *    body:                        ...
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSyncsGetResSend(_connectionId,_finalFlag,_responseCode,_totalObjectSize,_bodyLength,_body,_srmpOn){ \
        CsrBtSyncsGetRes *msg = CsrPmemAlloc(sizeof(CsrBtSyncsGetRes)); \
        msg->type            = CSR_BT_SYNCS_GET_RES;                    \
        msg->connectionId    = _connectionId;                           \
        msg->finalFlag       = _finalFlag;                              \
        msg->responseCode    = _responseCode;                           \
        msg->totalObjectSize = _totalObjectSize;                        \
        msg->bodyLength      = _bodyLength;                             \
        msg->body            = _body;                                   \
        msg->srmpOn          = _srmpOn;                                 \
        CsrBtSyncsMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsGetNextResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *    connectionId:                ...
 *    finalFlag:                   ...
 *    result:                      ...
 *    length:                      Length of body
 *    body:                        ...
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSyncsGetNextResSend(_connectionId,_finalFlag,_responseCode,_bodyLength,_body,_srmpOn){ \
        CsrBtSyncsGetNextRes    *msg = CsrPmemAlloc(sizeof(CsrBtSyncsGetNextRes)); \
        msg->type          = CSR_BT_SYNCS_GET_NEXT_RES;                 \
        msg->connectionId  = _connectionId;                             \
        msg->finalFlag     = _finalFlag;                                \
        msg->responseCode  = _responseCode;                             \
        msg->bodyLength    = _bodyLength;                               \
        msg->body          = _body;                                     \
        msg->srmpOn        = _srmpOn;                                   \
        CsrBtSyncsMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsPutResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *    connectionId:                ...
 *    result:                      ...
 *    theAppParameterlength:       ...
 *    appParameter:                ...
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSyncsPutResSend(_connectionId,_responseCode,_appParLength,_appParameter,_srmpOn){ \
        CsrBtSyncsPutRes *msg = CsrPmemAlloc(sizeof(CsrBtSyncsPutRes)); \
        msg->type                = CSR_BT_SYNCS_PUT_RES;                \
        msg->connectionId        = _connectionId;                       \
        msg->responseCode        = _responseCode;                       \
        msg->appParameterLength  = _appParLength;                       \
        msg->appParameter        = (CsrUint8*)_appParameter;            \
        msg->srmpOn              = _srmpOn;                             \
        CsrBtSyncsMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsGetPbChangeLogResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *    connectionId:                ...
 *    finalFlag:                   ...
 *    responseCode:                      ...
 *    lengthOfObject:              ...
 *    length:                      Length of body
 *    body:                        ...
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSyncsGetPbChangeLogResSend(_connectionId,_finalFlag,_responseCode,_totalObjectSize,_bodyLength,_body,_srmpOn){ \
        CsrBtSyncsGetPbChangeLogRes *msg = CsrPmemAlloc(sizeof(CsrBtSyncsGetPbChangeLogRes)); \
        msg->type              = CSR_BT_SYNCS_GET_PB_CHANGE_LOG_RES;    \
        msg->connectionId      = _connectionId;                         \
        msg->finalFlag         = _finalFlag;                            \
        msg->responseCode      = _responseCode;                         \
        msg->totalObjectSize   = _totalObjectSize;                      \
        msg->bodyLength        = _bodyLength;                           \
        msg->body              = _body;                                 \
        msg->srmpOn            = _srmpOn;                               \
        CsrBtSyncsMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsGetPbCurChangeLogResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *    connectionId:                 ...
 *    responseCode:                       ...
 *    lengthOfObject:               ...
 *    length:                       Length of changeCounter
 *    changeCounter:                ...
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSyncsGetPbCurChangeLogResSend(_connectionId,_responseCode,_totalObjectSize,_chgCntLength,_changeCounter,_srmpOn){ \
        CsrBtSyncsGetPbCurChangeLogRes *msg =  CsrPmemAlloc(sizeof(CsrBtSyncsGetPbCurChangeLogRes)); \
        msg->type                 = CSR_BT_SYNCS_GET_PB_CUR_CHANGE_LOG_RES; \
        msg->connectionId         = _connectionId;                      \
        msg->responseCode         = _responseCode;                      \
        msg->totalObjectSize      = _totalObjectSize;                   \
        msg->changeCounterLength  = _chgCntLength;                      \
        msg->changeCounter        = _changeCounter;                     \
        msg->srmpOn            = _srmpOn;                               \
        CsrBtSyncsMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsGetDeviceInfoResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *    connectionId:                ...
 *    finalFlag:                   ...
 *    responseCode:                      ...
 *    lengthOfObject:              ...
 *    length:                      Length of body
 *    body:                        ...
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSyncsGetDeviceInfoResSend(_connectionId,_finalFlag,_responseCode,_totalObjectSize,_bodyLength,_body,_srmpOn){ \
        CsrBtSyncsGetDeviceInfoRes *msg = CsrPmemAlloc(sizeof(CsrBtSyncsGetDeviceInfoRes)); \
        msg->type            = CSR_BT_SYNCS_GET_DEVICE_INFO_RES;        \
        msg->connectionId    = _connectionId;                           \
        msg->finalFlag       = _finalFlag;                              \
        msg->responseCode    = _responseCode;                           \
        msg->totalObjectSize = _totalObjectSize;                        \
        msg->bodyLength      = _bodyLength;                             \
        msg->body            = _body;                                   \
        msg->srmpOn          = _srmpOn;                                 \
        CsrBtSyncsMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsGetPbInfoLogResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *    connectionId:                ...
 *    finalFlag:                   ...
 *    responseCode:                      ...
 *    lengthOfObject:              ...
 *    length:                      Length of body
 *    body:                        ...
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSyncsGetPbInfoLogResSend(_connectionId,_finalFlag,_responseCode,_totalObjectSize,_bodyLength,_body,_srmpOn){ \
        CsrBtSyncsGetPbInfoLogRes *msg = CsrPmemAlloc(sizeof(CsrBtSyncsGetPbInfoLogRes)); \
        msg->type            = CSR_BT_SYNCS_GET_PB_INFO_LOG_RES;        \
        msg->connectionId    = _connectionId;                           \
        msg->finalFlag       = _finalFlag;                              \
        msg->responseCode    = _responseCode;                           \
        msg->totalObjectSize = _totalObjectSize;                        \
        msg->bodyLength      = _bodyLength;                             \
        msg->body            = _body;                                   \
        msg->srmpOn          = _srmpOn;                                 \
        CsrBtSyncsMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsGetPbEntryResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *    connectionId:         ...
 *    finalFlag:            ...
 *    responseCode:               ...
 *    lengthOfObject:       ...
 *    length:               Length of body
 *    body:                 ...
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSyncsGetPbEntryResSend(_connectionId,_finalFlag,_responseCode,_totalObjectSize,_bodyLength,_body,_srmpOn){ \
        CsrBtSyncsGetPbEntryRes *msg = CsrPmemAlloc(sizeof(CsrBtSyncsGetPbEntryRes)); \
        msg->type            = CSR_BT_SYNCS_GET_PB_ENTRY_RES;           \
        msg->connectionId    = _connectionId;                           \
        msg->finalFlag       = _finalFlag;                              \
        msg->responseCode    = _responseCode;                           \
        msg->totalObjectSize = _totalObjectSize;                        \
        msg->bodyLength      = _bodyLength;                             \
        msg->body            = _body;                                   \
        msg->srmpOn          = _srmpOn;                                 \
        CsrBtSyncsMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsGetPbAllResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *    connectionId:                ...
 *    finalFlag:                   ...
 *    responseCode:                      ...
 *    length:                      ...
 *    body:                        ...
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSyncsGetPbAllResSend(_connectionId,_finalFlag,_responseCode,_bodyLength,_body,_srmpOn){ \
        CsrBtSyncsGetPbAllRes *msg = CsrPmemAlloc(sizeof(CsrBtSyncsGetPbAllRes)); \
        msg->type          = CSR_BT_SYNCS_GET_PB_ALL_RES;               \
        msg->connectionId  = _connectionId;                             \
        msg->finalFlag     = _finalFlag;                                \
        msg->responseCode  = _responseCode;                             \
        msg->bodyLength    = _bodyLength;                               \
        msg->body          = _body;                                     \
        msg->srmpOn        = _srmpOn;                                   \
        CsrBtSyncsMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsPutPbEntryResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *    connectionId:                ...
 *    responseCode:                      ...
 *    length:                      ...
 *    appParameter:                ...
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSyncsPutPbEntryResSend(_connectionId,_responseCode,_appParLength,_appParameter,_srmpOn){ \
        CsrBtSyncsPutPbEntryRes *msg = CsrPmemAlloc(sizeof(CsrBtSyncsPutPbEntryRes)); \
        msg->type                = CSR_BT_SYNCS_PUT_PB_ENTRY_RES;       \
        msg->connectionId        = _connectionId;                       \
        msg->responseCode        = _responseCode;                       \
        msg->appParameterLength  = _appParLength;                       \
        msg->appParameter        = (CsrUint8*)_appParameter;            \
        msg->srmpOn              = _srmpOn;                             \
        CsrBtSyncsMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsPutNextResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *    connectionId:                 ...
 *    responseCode:                       ...
 *    length:                       ...
 *    appParameter:                 ...
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSyncsPutNextResSend(_connectionId,_responseCode,_appParLength,_appParameter,_srmpOn){ \
        CsrBtSyncsPutNextRes *msg = CsrPmemAlloc(sizeof(CsrBtSyncsPutNextRes)); \
        msg->type                = CSR_BT_SYNCS_PUT_NEXT_RES;           \
        msg->connectionId        = _connectionId;                       \
        msg->responseCode        = _responseCode;                       \
        msg->appParameterLength  = _appParLength;                       \
        msg->appParameter        = (CsrUint8*)_appParameter;            \
        msg->srmpOn              = _srmpOn;                             \
        CsrBtSyncsMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsPutPbAddEntryResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *    connectionId:                ...
 *    responseCode:                      ...
 *    length:                      ...
 *    appParameter:                ...
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSyncsPutPbAddEntryResSend(_connectionId,_responseCode,_appParLength,_appParameter,_srmpOn){ \
        CsrBtSyncsPutPbAddEntryRes *msg =  CsrPmemAlloc(sizeof(CsrBtSyncsPutPbAddEntryRes)); \
        msg->type                = CSR_BT_SYNCS_PUT_PB_ADD_ENTRY_RES;   \
        msg->connectionId        = _connectionId;                       \
        msg->responseCode        = _responseCode;                       \
        msg->appParameterLength  = _appParLength;                       \
        msg->appParameter        = (CsrUint8*)_appParameter;            \
        msg->srmpOn              = _srmpOn;                             \
        CsrBtSyncsMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsGetCalChangeLogResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *    connectionId:                ...
 *    finalFlag:                    ...
 *    responseCode:                        ...
 *    lengthOfObject:                ...
 *    length:                        ...
 *    body:                        ...
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSyncsGetCalChangeLogResSend(_connectionId,_finalFlag,_responseCode,_totalObjectSize,_bodyLength,_body,_srmpOn){ \
        CsrBtSyncsGetCalChangeLogRes *msg = CsrPmemAlloc(sizeof(CsrBtSyncsGetCalChangeLogRes)); \
        msg->type             = CSR_BT_SYNCS_GET_CAL_CHANGE_LOG_RES;    \
        msg->connectionId     = _connectionId;                          \
        msg->finalFlag        = _finalFlag;                             \
        msg->responseCode     = _responseCode;                          \
        msg->totalObjectSize  = _totalObjectSize;                       \
        msg->bodyLength       = _bodyLength;                            \
        msg->body             = _body;                                  \
        msg->srmpOn           = _srmpOn;                                \
        CsrBtSyncsMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsGetCalCurChangeLogResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *    connectionId:                ...
 *    responseCode:                        ...
 *    lengthOfObject:                ...
 *    length:                        ...
 *    changeCounter:                ...
 *----------------------------------------------------------------------------*/
#define CsrBtSyncsGetCalCurChangeLogResSend(_connectionId,_responseCode,_totalObjectSize,_chgCntLength,_changeCounter,_srmpOn){ \
        CsrBtSyncsGetCalCurChangeLogRes *msg = CsrPmemAlloc(sizeof(CsrBtSyncsGetCalCurChangeLogRes)); \
        msg->type                 = CSR_BT_SYNCS_GET_CAL_CUR_CHANGE_LOG_RES; \
        msg->connectionId         = _connectionId;                      \
        msg->responseCode         = _responseCode;                      \
        msg->totalObjectSize      = _totalObjectSize;                   \
        msg->changeCounterLength  = _chgCntLength;                      \
        msg->changeCounter        = _changeCounter;                     \
        msg->srmpOn               = _srmpOn;                            \
        CsrBtSyncsMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsGetCalInfoLogResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *    connectionId:                ...
 *    finalFlag:                    ...
 *    responseCode:                        ...
 *    lengthOfObject:                ...
 *    length:                        ...
 *    body:                        ...
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSyncsGetCalInfoLogResSend(_connectionId,_finalFlag,_responseCode,_totalObjectSize,_bodyLength,_body,_srmpOn){ \
        CsrBtSyncsGetCalInfoLogRes *msg = CsrPmemAlloc(sizeof(CsrBtSyncsGetCalInfoLogRes)); \
        msg->type             = CSR_BT_SYNCS_GET_CAL_INFO_LOG_RES;      \
        msg->connectionId     = _connectionId;                          \
        msg->finalFlag        = _finalFlag;                             \
        msg->responseCode     = _responseCode;                          \
        msg->totalObjectSize  = _totalObjectSize;                       \
        msg->bodyLength       = _bodyLength;                            \
        msg->body             = _body;                                  \
        msg->srmpOn           = _srmpOn;                                \
        CsrBtSyncsMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsGetCalEntryResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *    connectionId:                ...
 *    finalFlag:                    ...
 *    responseCode:                        ...
 *    lengthOfObject:                ...
 *    length:                        ...
 *    body:                        ...
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSyncsGetCalEntryResSend(_connectionId,_finalFlag,_responseCode,_totalObjectSize,_bodyLength,_body,_srmpOn){ \
        CsrBtSyncsGetCalEntryRes     *msg = CsrPmemAlloc(sizeof(CsrBtSyncsGetCalEntryRes)); \
        msg->type             = CSR_BT_SYNCS_GET_CAL_ENTRY_RES;         \
        msg->connectionId     = _connectionId;                          \
        msg->finalFlag        = _finalFlag;                             \
        msg->responseCode     = _responseCode;                          \
        msg->totalObjectSize  = _totalObjectSize;                       \
        msg->bodyLength       = _bodyLength;                            \
        msg->body             = _body;                                  \
        msg->srmpOn           = _srmpOn;                                \
        CsrBtSyncsMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsGetCalAllResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *    connectionId:                ...
 *    finalFlag:                    ...
 *    responseCode:                        ...
 *    length:                        ...
 *    body:                        ...
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSyncsGetCalAllResSend(_connectionId,_finalFlag,_responseCode,_bodyLength,_body,_srmpOn){ \
        CsrBtSyncsGetCalAllRes *msg = CsrPmemAlloc(sizeof(CsrBtSyncsGetCalAllRes)); \
        msg->type          = CSR_BT_SYNCS_GET_CAL_ALL_RES;              \
        msg->connectionId  = _connectionId;                             \
        msg->finalFlag     = _finalFlag;                                \
        msg->responseCode  = _responseCode;                             \
        msg->bodyLength    = _bodyLength;                               \
        msg->body          = _body;                                     \
        msg->srmpOn        = _srmpOn;                                   \
        CsrBtSyncsMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsPutCalEntryResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *    connectionId:                ...
 *    responseCode:                        ...
 *    length:                        ...
 *    appParameter:                        ...
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSyncsPutCalEntryResSend(_connectionId,_responseCode,_appParLength,_appParameter,_srmpOn){ \
        CsrBtSyncsPutCalEntryRes *msg = CsrPmemAlloc(sizeof(CsrBtSyncsPutCalEntryRes)); \
        msg->type                = CSR_BT_SYNCS_PUT_CAL_ENTRY_RES;      \
        msg->connectionId        = _connectionId;                       \
        msg->responseCode        = _responseCode;                       \
        msg->appParameterLength  = _appParLength;                       \
        msg->appParameter        = _appParameter;                       \
        msg->srmpOn              = _srmpOn;                             \
        CsrBtSyncsMsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsPutCalAddEntryResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *    connectionId:                ...
 *    responseCode:                        ...
 *    length:                        ...
 *    appParameter:                        ...
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSyncsPutCalAddEntryResSend(_connectionId,_responseCode,_appParLength,_appParameter,_srmpOn){ \
        CsrBtSyncsPutCalAddEntryRes *msg = CsrPmemAlloc(sizeof(CsrBtSyncsPutCalAddEntryRes)); \
        msg->type                = CSR_BT_SYNCS_PUT_CAL_ADD_ENTRY_RES;  \
        msg->connectionId        = _connectionId;                       \
        msg->responseCode        = _responseCode;                       \
        msg->appParameterLength  = _appParLength;                       \
        msg->appParameter        = _appParameter;                       \
        msg->srmpOn              = _srmpOn;                             \
        CsrBtSyncsMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsSecurityInReqSend
 *
 *  DESCRIPTION
 *      Set the default security settings for new incoming/outgoing connections
 *
 *  PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSyncsSecurityInReqSend(_appHandle, _secLevel) {            \
        CsrBtSyncsSecurityInReq *msg = (CsrBtSyncsSecurityInReq*)CsrPmemAlloc(sizeof(CsrBtSyncsSecurityInReq)); \
        msg->type = CSR_BT_SYNCS_SECURITY_IN_REQ;                       \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        CsrBtSyncsMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSyncsFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT SYNCS
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_SYNCS_PRIM,
 *      msg:          The message received from Synergy BT SYNCS
 *----------------------------------------------------------------------------*/
void CsrBtSyncsFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);


#ifdef __cplusplus
}
#endif

#endif
