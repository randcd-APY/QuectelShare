#ifndef CSR_BT_PAS_LIB_H__
#define CSR_BT_PAS_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2010-2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_msg_transport.h"
#include "csr_bt_profiles.h"
#include "csr_bt_pas_prim.h"
#include "csr_bt_tasks.h"
#include "csr_pmem.h"
#include "csr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtPasMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPasActivateReqSend
 *
 *  DESCRIPTION
 *      Set the Phonebook Access server in discoverable mode and in connectable mode
 *
 *  PARAMETERS
 *      hApp:       ...
 *      feature:    ...
 *      repository: ...
 *
 *----------------------------------------------------------------------------*/
#define CsrBtPasActivateReqSend(_hApp, _feature, _repository, _obexMaxPacketSize, _windowSize, _srmEnable) { \
        CsrBtPasActivateReq *msg__ = CsrPmemAlloc(sizeof(CsrBtPasActivateReq)); \
        msg__->type = CSR_BT_PAS_ACTIVATE_REQ;                          \
        msg__->appHandle = _hApp;                                       \
        msg__->feature = _feature;                                      \
        msg__->repository = _repository;                                \
        msg__->obexMaxPacketSize = _obexMaxPacketSize;                  \
        msg__->windowSize = _windowSize;                                \
        msg__->srmEnable = _srmEnable;                                  \
        CsrBtPasMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPasDeactivateReqSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *      None.
 *----------------------------------------------------------------------------*/
#define CsrBtPasDeactivateReqSend() {                                   \
        CsrBtPasDeactivateReq *msg__ = CsrPmemAlloc(sizeof(CsrBtPasDeactivateReq)); \
        msg__->type = CSR_BT_PAS_DEACTIVATE_REQ;                        \
        CsrBtPasMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPasConnectResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *      obexMaxPacketSize:      ...
 *      connectionId:           ...
 *      responseCode:                 ...
 *----------------------------------------------------------------------------*/
#define CsrBtPasConnectResSend(_connectionId, _responseCode) {          \
        CsrBtPasConnectRes *msg__ = CsrPmemAlloc(sizeof(CsrBtPasConnectRes)); \
        msg__->type              = CSR_BT_PAS_CONNECT_RES;              \
        msg__->connectionId      = _connectionId;                       \
        msg__->responseCode      = _responseCode;                       \
        CsrBtPasMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      PasAuthReq
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *      realmLength:        ...
 *      realm:              ...
 *      passwordLength:     ...
 *      password:           ...
 *      userId:             ...
 *----------------------------------------------------------------------------*/
#define CsrBtPasAuthReqSend(_realmLength,_realm,_passwordLength,_password,_userId) { \
        CsrBtPasAuthenticateReq *msg__ = CsrPmemAlloc(sizeof(CsrBtPasAuthenticateReq)); \
        msg__->type = CSR_BT_PAS_AUTHENTICATE_REQ;                      \
        msg__->realmLength = _realmLength;                              \
        msg__->realm = _realm;                                          \
        msg__->password = _password;                                    \
        msg__->passwordLength = _passwordLength;                        \
        msg__->userId = _userId;                                        \
        CsrBtPasMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      PasAuthRes
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *      passwordLength:     ...
 *      password:           ...
 *      userId:             ...
 *----------------------------------------------------------------------------*/
#define CsrBtPasAuthResSend(_passwordLength,_password,_userId) {        \
        CsrBtPasAuthenticateRes *msg__ = CsrPmemAlloc(sizeof(CsrBtPasAuthenticateRes)); \
        msg__->type = CSR_BT_PAS_AUTHENTICATE_RES;                      \
        msg__->password = _password;                                    \
        msg__->passwordLength = _passwordLength;                        \
        msg__->userId = _userId;                                        \
        CsrBtPasMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPasPullPbResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *      connectionId:    ...
 *      responseCode:          ...
 *----------------------------------------------------------------------------*/
#define CsrBtPasPullPbResSend(_connectionId, _responseCode) \
                CsrBtPasPullPbResSendEx(_connectionId, NULL, _responseCode, FALSE)

#define CsrBtPasPullPbResSendEx(_connectionId,                                  \
                                _pVersionInfo,                                  \
                                _responseCode,                                  \
                                _srmpOn)                                        \
do{                                                                             \
        CsrBtPasPullPbRes *_msg = CsrPmemZalloc(sizeof(CsrBtPasPullPbRes));     \
        _msg->type         = CSR_BT_PAS_PULL_PB_RES;                            \
        if(_pVersionInfo != NULL)                                               \
        {                                                                       \
            CsrMemCpy(&_msg->versionInfo,                                       \
                      _pVersionInfo,                                            \
                      sizeof(_msg->versionInfo));                               \
        }                                                                       \
        _msg->connectionId = _connectionId;                                     \
        _msg->responseCode = _responseCode;                                     \
        _msg->srmpOn = _srmpOn;                                                 \
        CsrBtPasMsgTransport(_msg);                                             \
}while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPasPullPbSizeResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *      connectionId:    ...
 *      responseCode:          ...
 *      pbSize:          ...
 *----------------------------------------------------------------------------*/
#define CsrBtPasPullPbSizeResSend(_connectionId, _responseCode, _pbSize, _srmpOn)   \
    CsrBtPasPullPbSizeResSendEx(_connectionId, _responseCode, _pbSize, NULL, _srmpOn)

#define CsrBtPasPullPbSizeResSendEx(_connectionId,                                  \
                                    _responseCode,                                  \
                                    _pbSize,                                        \
                                    _pVersionInfo,                                  \
                                    _srmpOn)                                        \
do{                                                                                 \
        CsrBtPasPullPbSizeRes *_msg = CsrPmemZalloc(sizeof(CsrBtPasPullPbSizeRes)); \
        _msg->type          = CSR_BT_PAS_PULL_PB_SIZE_RES;                          \
        if(_pVersionInfo != NULL)                                                   \
        {                                                                           \
            CsrMemCpy(&_msg->versionInfo,                                           \
                      _pVersionInfo,                                                \
                      sizeof(_msg->versionInfo));                                   \
        }                                                                           \
        _msg->connectionId  = _connectionId;                                        \
        _msg->pbSize        = _pbSize;                                              \
        _msg->responseCode  = _responseCode;                                        \
        _msg->srmpOn        = _srmpOn;                                              \
        CsrBtPasMsgTransport(_msg);                                                 \
}while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPasPullMchResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *      connectionId:    ...
 *      responseCode:          ...
 *      newMissedCall:   ...
 *----------------------------------------------------------------------------*/
#define CsrBtPasPullMchResSend(_connectionId, _responseCode, _newMissedCalls, _srmpOn) \
    CsrBtPasPullMchResSendEx(_connectionId, _responseCode, _newMissedCalls, NULL, _srmpOn)

#define CsrBtPasPullMchResSendEx(_connectionId,                             \
                                 _responseCode,                             \
                                 _newMissedCalls,                           \
                                 _pVersionInfo,                             \
                                 _srmpOn)                                   \
do{                                                                         \
        CsrBtPasPullMchRes *_msg = CsrPmemZalloc(sizeof(CsrBtPasPullMchRes)); \
        _msg->type               = CSR_BT_PAS_PULL_MCH_RES;                 \
        if(_pVersionInfo != NULL)                                           \
        {                                                                   \
            CsrMemCpy(&_msg->versionInfo,                                   \
                      _pVersionInfo,                                        \
                      sizeof(_msg->versionInfo));                           \
        }                                                                   \
        _msg->connectionId       = _connectionId;                           \
        _msg->responseCode       = _responseCode;                           \
        _msg->newMissedCall      = _newMissedCalls;                         \
        _msg->srmpOn             = _srmpOn;                                 \
        CsrBtPasMsgTransport(_msg);                                         \
}while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPasPullCchResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *      connectionId:    ...
 *      responseCode:    ...
 *      newMissedCall:   ...
 *----------------------------------------------------------------------------*/
#define CsrBtPasPullCchResSend(_connectionId,                               \
                                 _responseCode,                             \
                                 _newMissedCalls,                           \
                                 _pVersionInfo,                             \
                                 _srmpOn)                                   \
do{                                                                         \
        CsrBtPasPullCchRes *_msg = CsrPmemZalloc(sizeof(CsrBtPasPullCchRes)); \
        _msg->type               = CSR_BT_PAS_PULL_CCH_RES;                 \
        if(_pVersionInfo != NULL)                                           \
        {                                                                   \
            CsrMemCpy(&_msg->versionInfo,                                   \
                      _pVersionInfo,                                        \
                      sizeof(_msg->versionInfo));                           \
        }                                                                   \
        _msg->connectionId       = _connectionId;                           \
        _msg->responseCode       = _responseCode;                           \
        _msg->newMissedCall      = _newMissedCalls;                         \
        _msg->srmpOn             = _srmpOn;                                 \
        CsrBtPasMsgTransport(_msg);                                         \
}while(0)


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPasPullMchSizeResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *      connectionId:    ...
 *      responseCode:          ...
 *      pbSize:          ...
 *      newMissedCall:   ...
 *----------------------------------------------------------------------------*/
#define CsrBtPasPullMchSizeResSend(_connectionId, _responseCode, _pbSize, _newMissedCalls, _srmpOn) \
    CsrBtPasPullMchSizeResSendEx(_connectionId, _responseCode, _pbSize, _newMissedCalls, NULL, _srmpOn)

#define CsrBtPasPullMchSizeResSendEx(_connectionId,                     \
                                     _responseCode,                     \
                                     _pbSize,                           \
                                     _newMissedCalls,                   \
                                     _pVersionInfo,                     \
                                     _srmpOn)                           \
do{                                                                     \
        CsrBtPasPullMchSizeRes *msg__ = CsrPmemZalloc(sizeof(CsrBtPasPullMchSizeRes)); \
        msg__->type          = CSR_BT_PAS_PULL_MCH_SIZE_RES;            \
        if(_pVersionInfo != NULL)                                       \
        {                                                               \
            CsrMemCpy(&msg__->versionInfo,                              \
                      _pVersionInfo,                                    \
                      sizeof(msg__->versionInfo));                      \
        }                                                               \
        msg__->connectionId  = _connectionId;                           \
        msg__->responseCode  = _responseCode;                           \
        msg__->pbSize        = _pbSize;                                 \
        msg__->newMissedCall = _newMissedCalls;                         \
        msg__->srmpOn        = _srmpOn;                                 \
        CsrBtPasMsgTransport(msg__);                                    \
}while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPasPullMchSizeResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *      connectionId:    ...
 *      responseCode:          ...
 *      pbSize:          ...
 *      newMissedCall:   ...
 *----------------------------------------------------------------------------*/
#define CsrBtPasPullCchSizeResSendEx(_connectionId,                     \
                                     _responseCode,                     \
                                     _pbSize,                           \
                                     _newMissedCalls,                   \
                                     _pVersionInfo,                     \
                                     _srmpOn)                           \
do{                                                                     \
        CsrBtPasPullCchSizeRes *msg__ = CsrPmemZalloc(sizeof(CsrBtPasPullCchSizeRes)); \
        msg__->type          = CSR_BT_PAS_PULL_CCH_SIZE_RES;            \
        if(_pVersionInfo != NULL)                                       \
        {                                                               \
            CsrMemCpy(&_msg->versionInfo,                               \
                      _pVersionInfo,                                    \
                      sizeof(_msg->versionInfo));                       \
        }                                                               \
        msg__->connectionId  = _connectionId;                           \
        msg__->responseCode  = _responseCode;                           \
        msg__->pbSize        = _pbSize;                                 \
        msg__->newMissedCall = _newMissedCalls;                         \
        msg__->srmpOn        = _srmpOn;                                 \
        CsrBtPasMsgTransport(msg__);                                    \
}while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPasSetFolderResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *      connectionId:    ...
 *      responseCode:          ...
 *----------------------------------------------------------------------------*/
#define CsrBtPasSetFolderResSend(_connectionId, _responseCode) {        \
        CsrBtPasSetFolderRes *msg__ = CsrPmemZalloc(sizeof(CsrBtPasSetFolderRes)); \
        msg__->type          = CSR_BT_PAS_SET_FOLDER_RES;               \
        msg__->connectionId  = _connectionId;                           \
        msg__->responseCode  = _responseCode;                           \
        CsrBtPasMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPasPullVcardListResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *      connectionId:    ...
 *      responseCode:          ...
 *----------------------------------------------------------------------------*/
#define CsrBtPasPullVcardListResSend(_connectionId, _responseCode, _srmpOn)  \
    CsrBtPasPullVcardListResSendEx(_connectionId, _responseCode, NULL, _srmpOn)

#define CsrBtPasPullVcardListResSendEx(_connectionId,                   \
                                       _responseCode,                   \
                                       _pVersionInfo,                   \
                                       _srmpOn)                         \
do{                                                                     \
        CsrBtPasPullVcardListRes *msg__ = CsrPmemZalloc(sizeof(CsrBtPasPullVcardListRes)); \
        msg__->type          = CSR_BT_PAS_PULL_VCARD_LIST_RES;          \
        msg__->connectionId  = _connectionId;                           \
        msg__->responseCode  = _responseCode;                           \
        if(_pVersionInfo != NULL)                                       \
        {                                                               \
            CsrMemCpy(&msg__->versionInfo,                              \
                      _pVersionInfo,                                    \
                      sizeof(msg__->versionInfo));                      \
        }                                                               \
        msg__->srmpOn        = _srmpOn;                                 \
        CsrBtPasMsgTransport(msg__);                                    \
}while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPasPullVcardListSizeResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *      connectionId:    ...
 *      responseCode:          ...
 *      pbSize:          ...
 *----------------------------------------------------------------------------*/
#define CsrBtPasPullVcardListSizeResSend(_connectionId, _responseCode, _pbSize, _srmpOn) \
    CsrBtPasPullVcardListSizeResSendEx(_connectionId, _responseCode, _pbSize, NULL, _srmpOn)

#define CsrBtPasPullVcardListSizeResSendEx(_connectionId,               \
                                           _responseCode,               \
                                           _pbSize,                     \
                                           _pVersionInfo,               \
                                           _srmpOn)                     \
do{                                                                     \
        CsrBtPasPullVcardListSizeRes *msg__ = CsrPmemZalloc(sizeof(CsrBtPasPullVcardListSizeRes)); \
        msg__->type = CSR_BT_PAS_PULL_VCARD_LIST_SIZE_RES;              \
        msg__->connectionId = _connectionId;                            \
        msg__->responseCode = _responseCode;                            \
        msg__->pbSize       = _pbSize;                                  \
        if(_pVersionInfo != NULL)                                       \
        {                                                               \
            CsrMemCpy(&msg__->versionInfo,                              \
                      _pVersionInfo,                                    \
                      sizeof(msg__->versionInfo));                      \
        }                                                               \
        msg__->srmpOn       = _srmpOn;                                  \
        CsrBtPasMsgTransport(msg__);                                    \
}while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPasPullVcardMchListResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *      connectionId:    ...
 *      responseCode:          ...
 *      newMissedCall:   ...
 *----------------------------------------------------------------------------*/
#define CsrBtPasPullVcardMchListResSend(_connectionId, _responseCode, _newMissedCall, _srmpOn)  \
    CsrBtPasPullVcardMchListResSendEx(_connectionId, _responseCode, _newMissedCall, NULL, _srmpOn)

#define CsrBtPasPullVcardMchListResSendEx(_connectionId,                \
                                        _responseCode,                  \
                                        _newMissedCall,                 \
                                        _pVersionInfo,                  \
                                        _srmpOn)                        \
do{                                                                     \
        CsrBtPasPullVcardMchListRes *msg__ = CsrPmemZalloc(sizeof(CsrBtPasPullVcardMchListRes)); \
        msg__->type          = CSR_BT_PAS_PULL_VCARD_MCH_LIST_RES;      \
        msg__->connectionId  = _connectionId;                           \
        msg__->responseCode  = _responseCode;                           \
        msg__->newMissedCall = _newMissedCall;                          \
        if(_pVersionInfo != NULL)                                       \
        {                                                               \
            CsrMemCpy(&msg__->versionInfo,                              \
                      _pVersionInfo,                                    \
                      sizeof(msg__->versionInfo));                      \
        }                                                               \
        msg__->srmpOn        = _srmpOn;                                 \
        CsrBtPasMsgTransport(msg__);                                    \
}while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPasPullVcardCchListResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *      connectionId:    ...
 *      responseCode:    ...
 *      newMissedCall:   ...
 *----------------------------------------------------------------------------*/
#define CsrBtPasPullVcardCchListResSend(_connectionId,                  \
                                        _responseCode,                  \
                                        _newMissedCall,                 \
                                        _pVersionInfo,                  \
                                        _srmpOn)                        \
do{                                                                     \
        CsrBtPasPullVcardCchListRes *msg__ = CsrPmemZalloc(sizeof(CsrBtPasPullVcardCchListRes)); \
        msg__->type          = CSR_BT_PAS_PULL_VCARD_CCH_LIST_RES;      \
        msg__->connectionId  = _connectionId;                           \
        msg__->responseCode  = _responseCode;                           \
        msg__->newMissedCall = _newMissedCall;                          \
        if(_pVersionInfo != NULL)                                       \
        {                                                               \
            CsrMemCpy(&_msg->versionInfo,                               \
                      _pVersionInfo,                                    \
                      sizeof(_msg->versionInfo));                       \
        }                                                               \
        msg__->srmpOn        = _srmpOn;                                 \
        CsrBtPasMsgTransport(msg__);                                    \
}while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPasPullVcardMchListSizeResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *      connectionId:    ...
 *      responseCode:          ...
 *      pbSize:          ...
 *      newMissedCall:   ...
 *----------------------------------------------------------------------------*/
#define CsrBtPasPullVcardMchListSizeResSend(_connectionId, _responseCode, _pbSize, _newMissedCall, _srmpOn) \
    CsrBtPasPullVcardMchListSizeResSendEx(_connectionId, _responseCode, _pbSize, _newMissedCall, NULL, _srmpOn)

#define CsrBtPasPullVcardMchListSizeResSendEx(_connectionId,            \
                                              _responseCode,            \
                                              _pbSize,                  \
                                              _newMissedCall,           \
                                              _pVersionInfo,            \
                                              _srmpOn)                  \
do{                                                                     \
        CsrBtPasPullVcardMchListSizeRes *msg__ = CsrPmemZalloc(sizeof(CsrBtPasPullVcardMchListSizeRes)); \
        msg__->type          = CSR_BT_PAS_PULL_VCARD_MCH_LIST_SIZE_RES; \
        msg__->connectionId  = _connectionId;                           \
        msg__->responseCode  = _responseCode;                           \
        msg__->pbSize        = _pbSize;                                 \
        msg__->newMissedCall = _newMissedCall;                          \
        if(_pVersionInfo != NULL)                                       \
        {                                                               \
            CsrMemCpy(&msg__->versionInfo,                              \
                      _pVersionInfo,                                    \
                      sizeof(msg__->versionInfo));                      \
        }                                                               \
        msg__->srmpOn        = _srmpOn;                                 \
        CsrBtPasMsgTransport(msg__);                                    \
}while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPasPullVcardCchListSizeResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *      connectionId:    ...
 *      responseCode:          ...
 *      pbSize:          ...
 *      newMissedCall:   ...
 *----------------------------------------------------------------------------*/
#define CsrBtPasPullVcardCchListSizeResSend(_connectionId,              \
                                              _responseCode,            \
                                              _pbSize,                  \
                                              _newMissedCall,           \
                                              _pVersionInfo,            \
                                              _srmpOn)                  \
do{                                                                     \
        CsrBtPasPullVcardCchListSizeRes *msg__ = CsrPmemZalloc(sizeof(CsrBtPasPullVcardCchListSizeRes)); \
        msg__->type          = CSR_BT_PAS_PULL_VCARD_MCH_LIST_SIZE_RES; \
        msg__->connectionId  = _connectionId;                           \
        msg__->responseCode  = _responseCode;                           \
        msg__->pbSize        = _pbSize;                                 \
        msg__->newMissedCall = _newMissedCall;                          \
        if(_pVersionInfo != NULL)                                       \
        {                                                               \
            CsrMemCpy(&msg__->versionInfo,                              \
                      _pVersionInfo,                                    \
                      sizeof(msg__->versionInfo));                      \
        }                                                               \
        msg__->srmpOn        = _srmpOn;                                 \
        CsrBtPasMsgTransport(msg__);                                    \
}while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPasPullVcardEntryResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *      connectionId:    ...
 *      finalFlag:       ...
 *      responseCode:          ...
 *----------------------------------------------------------------------------*/
#define CsrBtPasPullVcardEntryResSend(_connectionId, _finalFlag, _responseCode, _srmpOn) \
    CsrBtPasPullVcardEntryResSendEx(_connectionId, _finalFlag, _responseCode, NULL, _srmpOn)

#define CsrBtPasPullVcardEntryResSendEx(_connectionId, _finalFlag, _responseCode, _databaseId, _srmpOn) \
do{                                                                     \
        CsrBtPasPullVcardEntryRes *msg__ = CsrPmemZalloc(sizeof(CsrBtPasPullVcardEntryRes)); \
        msg__->type          = CSR_BT_PAS_PULL_VCARD_ENTRY_RES;         \
        msg__->connectionId  = _connectionId;                           \
        msg__->finalFlag     = _finalFlag;                              \
        msg__->responseCode  = _responseCode;                           \
        if(_databaseId != NULL)                                         \
        {                                                               \
            CsrMemCpy(&msg__->databaseIdentifier,                       \
                      _databaseId,                                      \
                      sizeof(msg__->databaseIdentifier));               \
        }                                                               \
        msg__->srmpOn        = _srmpOn;                                 \
        CsrBtPasMsgTransport(msg__);                                    \
}while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPasNextResSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *      connectionId:    ...
 *      finalFlag:       ...
 *      responseCode:          ...
 *      body:            ...
 *      bodyLen:         ...
 *----------------------------------------------------------------------------*/
#define CsrBtPasNextResSend(_connectionId, _finalFlag, _responseCode, _bodyLen, _body, _srmpOn) { \
        CsrBtPasNextRes *msg__ = CsrPmemAlloc(sizeof(CsrBtPasNextRes)); \
        msg__->type          = CSR_BT_PAS_NEXT_RES;                     \
        msg__->connectionId  = _connectionId;                           \
        msg__->finalFlag     = _finalFlag;                              \
        msg__->responseCode  = _responseCode;                           \
        msg__->body          = _body;                                   \
        msg__->bodyLength    = _bodyLen;                                \
        msg__->srmpOn        = _srmpOn;                                 \
        CsrBtPasMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPasSecurityInReqSend
 *
 *  DESCRIPTION
 *      Set the default security settings for new incoming/outgoing connections
 *
 *  PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtPasSecurityInReqSend(_appHandle, _secLevel) {              \
        CsrBtPasSecurityInReq *msg = (CsrBtPasSecurityInReq*)CsrPmemAlloc(sizeof(CsrBtPasSecurityInReq)); \
        msg->type = CSR_BT_PAS_SECURITY_IN_REQ;                         \
        msg->appHandle = _appHandle;                                    \
        msg->secLevel = _secLevel;                                      \
        CsrBtPasMsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPasFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT PAS
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_PAS_PRIM,
 *      msg:          The message received from Synergy BT PAS
 *----------------------------------------------------------------------------*/
void CsrBtPasFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);



#ifdef __cplusplus
}
#endif

#endif

