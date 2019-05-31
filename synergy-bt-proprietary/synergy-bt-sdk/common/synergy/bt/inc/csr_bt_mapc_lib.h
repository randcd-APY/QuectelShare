#ifndef CSR_BT_MAPC_LIB_H__
#define CSR_BT_MAPC_LIB_H__

#include "csr_synergy.h"
/******************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_profiles.h"
#include "csr_bt_mapc_prim.h"
#include "csr_pmem.h"
#include "csr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtMapcMsgTransport(CsrSchedQid phandle, void *msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcGetInstanceIdsReqSend
 *
 *  DESCRIPTION
 *      This signal is used to get the list of registered MAPC instances
 *      from the MAPC instance that is also running as MAPC-manager
 *
 *  PARAMETERS
 *        appHandle:    Application handle, so the MAPC-manager
 *                      knows where to return the result to.
 *---------------------------------------------------------------------------*/
#define CsrBtMapcGetInstanceIdsReqSend(_appHandle) {                       \
        CsrBtMapcGetInstanceIdsReq *msg = CsrPmemAlloc(sizeof(*msg));      \
        msg->type                      = CSR_BT_MAPC_GET_INSTANCE_IDS_REQ; \
        msg->appHandle                 = _appHandle;                       \
        CsrBtMapcMsgTransport(CSR_BT_MAPC_IFACEQUEUE,msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcConnectReqSend
 *
 *  DESCRIPTION
 *      Try to make a connection the a peer device.
 *
 *    PARAMETERS
 *        appHandle:                ...
 *        maxPacketSize:            ...
 *        deviceAddr:              ...
 *        length:               Length is used to express the approximate total
 *                              length of the bodies of all the objects in the
 *                              transaction
 *        count:                Count is used to indicate the number of objects
 *                              that will be sent during this connection
 *----------------------------------------------------------------------------*/
#define CsrBtMapcConnectReqSend(_mapcInstanceId,_appHandle,_maxPacketSize,_deviceAddr,_windowSize) { \
        CsrBtMapcConnectReq *msg = CsrPmemAlloc(sizeof(*msg));        \
        msg->type           = CSR_BT_MAPC_CONNECT_REQ;                \
        msg->appHandle      = _appHandle;                             \
        msg->maxPacketSize  = _maxPacketSize;                         \
        msg->deviceAddr     = _deviceAddr;                            \
        msg->windowSize     = _windowSize;                            \
        CsrBtMapcMsgTransport(_mapcInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcCancelConnectReqSend
 *
 *  DESCRIPTION
 *      Try to cancel the connection which is being establish to a peer device.
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtMapcCancelConnectReqSend(_mapcInstanceId) {                \
        CsrBtMapcCancelConnectReq *msg = CsrPmemAlloc(sizeof(*msg));    \
        msg->type    = CSR_BT_MAPC_CANCEL_CONNECT_REQ;                  \
        CsrBtMapcMsgTransport(_mapcInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcDisconnectReqSend
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
#define CsrBtMapcDisconnectReqSend(_mapcInstanceId,_normalObexDisconnect) { \
        CsrBtMapcDisconnectReq *msg = CsrPmemAlloc(sizeof(*msg));     \
        msg->type                  = CSR_BT_MAPC_DISCONNECT_REQ;      \
        msg->normalObexDisconnect  = _normalObexDisconnect;           \
        CsrBtMapcMsgTransport(_mapcInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcSelectMasInstanceResSend
 *
 *  DESCRIPTION
 *      This signal is used in response to the CSR_BT_MAPC_SELECT_MAS_INSTANCE_IND
 *      where the application can specify which of the available masInstanceId's
 *      it wants to connect (if any).
 *
 *  PARAMETERS
 *      proceedWithConnection:    specifies whether to proceed with the connection
 *                                establishment or not.
 *      masInstanceId: the mas instance which the application wishes to connect
 *---------------------------------------------------------------------------*/
#define CsrBtMapcSelectMasInstanceResSend(_mapcInstanceId,_proceedWithConnection,_masInstanceId) { \
        CsrBtMapcSelectMasInstanceRes *msg = CsrPmemAlloc(sizeof(*msg));  \
        msg->type                  = CSR_BT_MAPC_SELECT_MAS_INSTANCE_RES; \
        msg->proceedWithConnection = _proceedWithConnection;              \
        msg->masInstanceId         = _masInstanceId;                      \
        CsrBtMapcMsgTransport(_mapcInstanceId,msg);}



/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcSetFolderReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        folderName:          ...
 *----------------------------------------------------------------------------*/
#define CsrBtMapcSetFolderReqSend(_mapcInstanceId,_folderName) {        \
        CsrBtMapcSetFolderReq *msg = CsrPmemAlloc(sizeof(*msg));        \
        msg->type              = CSR_BT_MAPC_SET_FOLDER_REQ;            \
        msg->folderName              = _folderName;                     \
        CsrBtMapcMsgTransport(_mapcInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcSetBackFolderReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtMapcSetBackFolderReqSend(_mapcInstanceId) {                \
        CsrBtMapcSetBackFolderReq *msg = CsrPmemAlloc(sizeof(*msg));    \
        msg->type              = CSR_BT_MAPC_SET_BACK_FOLDER_REQ;       \
        CsrBtMapcMsgTransport(_mapcInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcSetBackFolderReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtMapcSetRootFolderReqSend(_mapcInstanceId) {                \
        CsrBtMapcSetRootFolderReq *msg = CsrPmemAlloc(sizeof(*msg));    \
        msg->type              = CSR_BT_MAPC_SET_ROOT_FOLDER_REQ;       \
        CsrBtMapcMsgTransport(_mapcInstanceId, msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcGetFolderListingReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        maxListCount:          ...
 *        listStartOffset:          ...
 *----------------------------------------------------------------------------*/
#define CsrBtMapcGetFolderListingReqSend(_mapcInstanceId,_maxListCount, _listStartOffset, _srmpOn) { \
        CsrBtMapcGetFolderListingReq *msg = CsrPmemAlloc(sizeof(*msg)); \
        msg->type              = CSR_BT_MAPC_GET_FOLDER_LISTING_REQ;    \
        msg->maxListCount      = _maxListCount;                         \
        msg->listStartOffset   = _listStartOffset;                      \
        msg->srmpOn            = _srmpOn;                               \
        CsrBtMapcMsgTransport(_mapcInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcGetFolderListingResSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtMapcGetFolderListingResSend(_mapcInstanceId, _srmpOn) {    \
        CsrBtMapcGetFolderListingRes *msg = CsrPmemAlloc(sizeof(*msg)); \
        msg->type              = CSR_BT_MAPC_GET_FOLDER_LISTING_RES;    \
        msg->srmpOn            = _srmpOn;                               \
        CsrBtMapcMsgTransport(_mapcInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcGetMessageListingReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *       folderName:            ...
 *       maxListCount:          ...
 *       listStartOffset:       ...
 *       maxSubjectLength:      ...
 *       parameterMask:         ...
 *       filterMessageType:     ...
 *       filterPeriodBegin:     ...
 *       filterPeriodEnd:       ...
 *       filterReadStatus:      ...
 *       filterRecipient:       ...
 *       filterOriginator:      ...
 *       filterPriority:        ...
 *       conversationId:        Conversation ID (optional). 
 *                              Should be allocated a total of 32 bytes + 1 for NUL
 *       filterMessageHandle    Message Handle. 
 *                              Should be allocated a total of 16 bytes + 1 for NUL
 *----------------------------------------------------------------------------*/
#define CsrBtMapcGetMessageListingReqSend(_mapcInstanceId, _folderName, \
                _maxListCount, _listStartOffset, _maxSubjectLength,     \
                _parameterMask, _filterMessageType, _filterPeriodBegin, \
                _filterPeriodEnd, _filterReadStatus, _filterRecipient,  \
                _filterOriginator, _filterPriority, _conversationId,    \
                _filterMessageHandle, _srmpOn) {                        \
        CsrBtMapcGetMessageListingReq *msg = CsrPmemAlloc(sizeof(*msg));    \
        msg->type                   = CSR_BT_MAPC_GET_MESSAGE_LISTING_REQ;  \
        msg->folderName             = _folderName;                          \
        msg->maxListCount           = _maxListCount;                        \
        msg->listStartOffset        = _listStartOffset;                     \
        msg->maxSubjectLength       = _maxSubjectLength;                    \
        msg->parameterMask          = _parameterMask;                       \
        msg->filterMessageType      = _filterMessageType;                   \
        msg->filterPeriodBegin      = _filterPeriodBegin;                   \
        msg->filterPeriodEnd        = _filterPeriodEnd;                     \
        msg->filterReadStatus       = _filterReadStatus;                    \
        msg->filterRecipient        = _filterRecipient;                     \
        msg->filterOriginator       = _filterOriginator;                    \
        msg->filterPriority         = _filterPriority;                      \
        msg->conversationId         = _conversationId;                      \
        msg->filterMessageHandle    = _filterMessageHandle;                 \
        msg->srmpOn                 = _srmpOn;                              \
        CsrBtMapcMsgTransport(_mapcInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcGetMessageListingResSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtMapcGetMessageListingResSend(_mapcInstanceId, _srmpOn) {    \
        CsrBtMapcGetMessageListingRes *msg = CsrPmemAlloc(sizeof(*msg)); \
        msg->type              = CSR_BT_MAPC_GET_MESSAGE_LISTING_RES;    \
        msg->srmpOn            = _srmpOn;                                \
        CsrBtMapcMsgTransport(_mapcInstanceId, msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      MapcGetMessageReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *       messageHandle:         ...
 *       attachment:            ...
 *       charset:               ...
 *       fractionRequest:       ...
 *----------------------------------------------------------------------------*/
#define CsrBtMapcGetMessageReqSend(_mapcInstanceId,_messageHandle, _attachment, _charset, _fractionRequest, _srmpOn) { \
        CsrBtMapcGetMessageReq *msg = CsrPmemAlloc(sizeof(*msg));     \
        msg->type                      = CSR_BT_MAPC_GET_MESSAGE_REQ; \
        msg->messageHandle             = _messageHandle;              \
        msg->attachment                = _attachment;                 \
        msg->charset                   = _charset;                    \
        msg->fractionRequest           = _fractionRequest;            \
        msg->srmpOn                    = _srmpOn;                     \
        CsrBtMapcMsgTransport(_mapcInstanceId, msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcGetMessageResSend
 *
 *  DESCRIPTION
 *      .....
 *
 *    PARAMETERS
 *                    None
 *----------------------------------------------------------------------------*/
#define CsrBtMapcGetMessageResSend(_mapcInstanceId, _srmpOn) {          \
        CsrBtMapcGetMessageRes *msg = CsrPmemAlloc(sizeof(*msg));       \
        msg->type              = CSR_BT_MAPC_GET_MESSAGE_RES;           \
        msg->srmpOn            = _srmpOn;                               \
        CsrBtMapcMsgTransport(_mapcInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcSetMessageStatusReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        messageHandle:            ...
 *        statusIndicator:          ...
 *        statusValue:              ...
 *----------------------------------------------------------------------------*/
#define CsrBtMapcSetMessageStatusReqSend(_mapcInstanceId,_messageHandle, _statusIndicator, _statusValue, _extendedData) { \
        CsrBtMapcSetMessageStatusReq *msg = CsrPmemAlloc(sizeof(*msg));      \
        msg->type                      = CSR_BT_MAPC_SET_MESSAGE_STATUS_REQ; \
        msg->messageHandle             = _messageHandle;                     \
        msg->statusIndicator           = _statusIndicator;                   \
        msg->statusValue               = _statusValue;                       \
        msg->extendedData              = _extendedData;                      \
        CsrBtMapcMsgTransport(_mapcInstanceId, msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcPushMessageReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        folderName:               ...
 *        lengthOfObject:               ...
 *        transparent:               ...
 *        retry:               ...
 *        charset:               ...
 *----------------------------------------------------------------------------*/
#define CsrBtMapcPushMessageReqSend(_mapcInstanceId,_folderName,_lengthOfObject, _transparent, _retry, _charset, _conversationId) { \
        CsrBtMapcPushMessageReq *msg = CsrPmemAlloc(sizeof(*msg));     \
        msg->type                      = CSR_BT_MAPC_PUSH_MESSAGE_REQ; \
        msg->folderName                = _folderName;                  \
        msg->lengthOfObject            = _lengthOfObject;              \
        msg->transparent               = _transparent;                 \
        msg->retry                     = _retry;                       \
        msg->charset                   = _charset;                     \
        msg->conversationId            = _conversationId;              \
        CsrBtMapcMsgTransport(_mapcInstanceId, msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcPushMessageResSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        finalFlag:        ...
 *        payloadLength:        ...
 *        *payload:            ...
 *----------------------------------------------------------------------------*/
#define CsrBtMapcPushMessageResSend(_mapcInstanceId,_finalFlag,_payloadLength,_payload) { \
        CsrBtMapcPushMessageRes *msg = CsrPmemAlloc(sizeof(*msg));    \
        msg->type                     = CSR_BT_MAPC_PUSH_MESSAGE_RES; \
        msg->finalFlag                = _finalFlag;                   \
        msg->payloadLength            = _payloadLength;               \
        msg->payload                  = _payload;                     \
        CsrBtMapcMsgTransport(_mapcInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcUpdateInboxReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        None
 *----------------------------------------------------------------------------*/
#define CsrBtMapcUpdateInboxReqSend(_mapcInstanceId) {                  \
    CsrBtMapcUpdateInboxReq *msg = CsrPmemAlloc(sizeof(*msg));          \
    msg->type                  = CSR_BT_MAPC_UPDATE_INBOX_REQ;          \
    CsrBtMapcMsgTransport(_mapcInstanceId, msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcAbortReqSend
 *
 *  DESCRIPTION
 *      .....
 *
 *    PARAMETERS
 *        None
 *----------------------------------------------------------------------------*/
#define CsrBtMapcAbortReqSend(_mapcInstanceId) {                        \
        CsrBtMapcAbortReq *msg = CsrPmemAlloc(sizeof(*msg));            \
        msg->type                  = CSR_BT_MAPC_ABORT_REQ;             \
        CsrBtMapcMsgTransport(_mapcInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcNotificationRegistrationReqSend
 *
 *  DESCRIPTION
 *      .....
 *
 *    PARAMETERS
 *        enableNotifications:        ...
 *----------------------------------------------------------------------------*/
#define CsrBtMapcNotificationRegistrationReqSend(_mapcInstanceId, _enableNotifications) { \
        CsrBtMapcNotificationRegistrationReq *msg = CsrPmemAlloc(sizeof(*msg));           \
        msg->type                  = CSR_BT_MAPC_NOTIFICATION_REGISTRATION_REQ;           \
        msg->enableNotifications   = _enableNotifications;                                \
        CsrBtMapcMsgTransport(_mapcInstanceId, msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcEventNotificationResSend
 *
 *  DESCRIPTION
 *      .....
 *
 *    PARAMETERS
 *        response:          ...
 *----------------------------------------------------------------------------*/
#define CsrBtMapcEventNotificationResSend(_mapcInstanceId, _response, _srmpOn) { \
        CsrBtMapcEventNotificationRes *msg = CsrPmemAlloc(sizeof(*msg));         \
        msg->type                  = CSR_BT_MAPC_EVENT_NOTIFICATION_RES;         \
        msg->instanceId            = _mapcInstanceId;                            \
        msg->response              = _response;                                  \
        msg->srmpOn                = _srmpOn;                                    \
        CsrBtMapcMsgTransport(CSR_BT_MAPC_IFACEQUEUE, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcSecurityInReqSend
 *
 *  DESCRIPTION
 *      Set the default security settings for new incoming/outgoing connections
 *
 *  PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapcSecurityInReqSend(_mapcInstanceId,_appHandle, _secLevel) { \
        CsrBtMapcSecurityInReq  *msg = CsrPmemAlloc(sizeof(*msg));          \
        msg->type          = CSR_BT_MAPC_SECURITY_IN_REQ;                   \
        msg->appHandle     = _appHandle;                                    \
        msg->secLevel      = _secLevel;                                     \
        CsrBtMapcMsgTransport(_mapcInstanceId,msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcSecurityOutReqSend
 *
 *  DESCRIPTION
 *      Set the default security settings for new incoming/outgoing connections
 *
 *  PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapcSecurityOutReqSend(_mapcInstanceId,_appHandle, _secLevel) { \
        CsrBtMapcSecurityOutReq *msg = CsrPmemAlloc(sizeof(*msg));           \
        msg->type          = CSR_BT_MAPC_SECURITY_OUT_REQ;                   \
        msg->appHandle     = _appHandle;                                     \
        msg->secLevel      = _secLevel;                                      \
        CsrBtMapcMsgTransport(_mapcInstanceId,msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcGetMasInstanceInformationReqSend
 *
 *  DESCRIPTION
 *      This allows MCE to get additional information about any advertised instance.
 *      The result would be text field containing user-readable information 
 *      about a given MAS Instance.
 *
 *    PARAMETERS
 *        srmpon:          ...
 *----------------------------------------------------------------------------*/
#define CsrBtMapcGetMasInstanceInformationReqSend(_mapcInstanceId) {             \
        CsrBtMapcGetMasInstanceInformationReq *msg = CsrPmemAlloc(sizeof(*msg)); \
        msg->type              = CSR_BT_MAPC_GET_MAS_INSTANCE_INFORMATION_REQ;   \
        CsrBtMapcMsgTransport(_mapcInstanceId, msg);}

/********************************************************************************/
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcGetConversationListingReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtMapcGetConversationListingReqSend(_mapcInstanceId, _maxListCount,          \
            _listStartOffset, _filterLastActivityBegin, _filterLastActivityEnd,         \
            _filterReadStatus, _filterRecipient, _conversationId,                       \
            _convParameterMask, _srmpOn) {                                              \
            CsrBtMapcGetConversationListingReq *msg = CsrPmemAlloc(sizeof(*msg));       \
            msg->type                    = CSR_BT_MAPC_GET_CONVERSATION_LISTING_REQ;    \
            msg->maxListCount            = _maxListCount;                               \
            msg->listStartOffset         = _listStartOffset;                            \
            msg->filterLastActivityBegin = _filterLastActivityBegin;                    \
            msg->filterLastActivityEnd   = _filterLastActivityEnd;                      \
            msg->filterReadStatus        = _filterReadStatus;                           \
            msg->filterRecipient         = _filterRecipient;                            \
            msg->conversationId          = _conversationId;                             \
            msg->convParameterMask       = _convParameterMask;                          \
            msg->srmpOn                  = _srmpOn;                                     \
            CsrBtMapcMsgTransport(_mapcInstanceId, msg);}
    
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcGetConversationListingResSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtMapcGetConversationListingResSend(_mapcInstanceId, _srmpOn) {              \
            CsrBtMapcGetConversationListingRes *msg = CsrPmemAlloc(sizeof(*msg));       \
            msg->type              = CSR_BT_MAPC_GET_CONVERSATION_LISTING_RES;          \
            msg->srmpOn            = _srmpOn;                                           \
            CsrBtMapcMsgTransport(_mapcInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcGetOwnerStatusReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtMapcGetOwnerStatusReqSend(_mapcInstanceId, _conversationId) {          \
        CsrBtMapcGetOwnerStatusReq *msg = CsrPmemAlloc(sizeof(*msg));               \
        msg->type                       = CSR_BT_MAPC_GET_OWNER_STATUS_REQ;         \
        msg->conversationId             = _conversationId;                          \
        CsrBtMapcMsgTransport(_mapcInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcSetOwnerStatusReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtMapcSetOwnerStatusReqSend(_mapcInstanceId, _presenceAvailability, _presenceText, _lastActivity, _chatState, _conversationId) { \
        CsrBtMapcSetOwnerStatusReq *msg = CsrPmemAlloc(sizeof(*msg));               \
        msg->type                      = CSR_BT_MAPC_SET_OWNER_STATUS_REQ  ;        \
        msg->presenceAvailability     = _presenceAvailability;                      \
        msg->presenceText             = _presenceText;                              \
        msg->lastActivity             = _lastActivity;                              \
        msg->chatState                = _chatState;                                 \
        msg->conversationId           = _conversationId;                            \
        CsrBtMapcMsgTransport(_mapcInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcSetNotificationFilterReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtMapcSetNotificationFilterReqSend(_mapcInstanceId, _notiFilterMask) {  \
        CsrBtMapcSetNotificationFilterReq *msg = CsrPmemAlloc(sizeof(*msg));       \
        msg->type                     = CSR_BT_MAPC_SET_NOTIFICATION_FILTER_REQ;   \
        msg->notiFilterMask           = _notiFilterMask;                           \
        CsrBtMapcMsgTransport(_mapcInstanceId, msg);}

/********************************************************************************/

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapcFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT MAPC
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_MAPC_PRIM,
 *      msg:          The message received from Synergy BT MAPC
 *----------------------------------------------------------------------------*/
void CsrBtMapcFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);


#ifdef __cplusplus
}
#endif

#endif
