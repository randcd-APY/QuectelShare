#ifndef CSR_BT_MAPS_LIB_H__
#define CSR_BT_MAPS_LIB_H__

/****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_bt_profiles.h"
#include "csr_bt_maps_prim.h"
#include "csr_pmem.h"
#include "csr_util.h"
#include "csr_bt_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtMapsMsgTransport(CsrSchedQid phandle, void *msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsGetInstanceIdsReqSend
 *
 *  DESCRIPTION
 *      This signal is used to get the list of registered Maps instances
 *      from the Maps instance that is also running as Maps-manager
 *
 *  PARAMETERS
 *        appHandle:    Application handle, so the Maps-manager
 *                      knows where to return the result to.
 *---------------------------------------------------------------------------*/
#define CsrBtMapsGetInstanceIdsReqSend(_appHandle) {                    \
        CsrBtMapsGetInstanceIdsReq *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type                      = CSR_BT_MAPS_GET_INSTANCE_IDS_REQ; \
        msg->appHandle                 = _appHandle;                    \
        CsrBtMapsMsgTransport(CSR_BT_MAPS_IFACEQUEUE,msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsSelectMasInstanceResSend
 *
 *  DESCRIPTION
 *      This signal is used in response to the CSR_BT_MAPS_SELECT_MAS_INSTANCE_IND
 *      where the application can specify which of the available masInstanceId's
 *      it wants to connect (if any).
 *
 *  PARAMETERS
 *      proceedWithConnection:    specifies whether to proceed with the connection
 *                                establishment or not.
 *      masInstanceId: the mas instance which the application wishes to connect
 *---------------------------------------------------------------------------*/
#define CsrBtMapsSelectMasInstanceResSend(_mapsInstanceId,_proceedWithConnection,_masInstanceId) { \
        CsrBtMapsSelectMasInstanceRes *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type                  = CSR_BT_MAPS_SELECT_MAS_INSTANCE_RES; \
        msg->proceedWithConnection = _proceedWithConnection;            \
        msg->masInstanceId         = _masInstanceId;                    \
        CsrBtMapsMsgTransport(_mapsInstanceId,msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsActivateReqSend
 *
 *  DESCRIPTION
 *      Try to make a connection the a peer device.
 *
 *    PARAMETERS
 *        appHandle:                ...
 *        supportedMessages        ...
 *----------------------------------------------------------------------------*/
#define CsrBtMapsActivateExReqSend(_mapsInstanceId,_appHandle,_supportedMessages,_obexMaxPacketSize,_windowSize,_srmEnable,_instanceId,_nameLen,_name, _supportedFeatures) { \
        CsrBtMapsActivateReq *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type              = CSR_BT_MAPS_ACTIVATE_REQ;            \
        msg->appHandle         = _appHandle;                          \
        msg->supportedMessages = _supportedMessages;                  \
        msg->obexMaxPacketSize = _obexMaxPacketSize;                  \
        msg->windowSize        = _windowSize;                         \
        msg->srmEnable         = _srmEnable;                          \
        msg->instanceId        = _instanceId;                         \
        msg->nameLen           = _nameLen;                            \
        msg->name              = _name;                               \
        msg->supportedFeatures = _supportedFeatures;                  \
        CsrBtMapsMsgTransport(_mapsInstanceId, msg);}

#define CsrBtMapsActivateExtReqSend(_mapsInstanceId,_appHandle,_supportedMessages,_obexMaxPacketSize,_windowSize,_srmEnable,_instanceId,_nameLen,_name) \
    CsrBtMapsActivateExReqSend(_mapsInstanceId,_appHandle,_supportedMessages,_obexMaxPacketSize,_windowSize,_srmEnable,_instanceId,_nameLen,_name, CSR_BT_MAP_SF_DEFAULT)

#define CsrBtMapsActivateReqSend(_mapsInstanceId,_appHandle,_supportedMessages,_obexMaxPacketSize,_windowSize,_srmEnable) \
    CsrBtMapsActivateExtReqSend(_mapsInstanceId,_appHandle,_supportedMessages,_obexMaxPacketSize,_windowSize,_srmEnable,0,0,NULL)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsDeactivateReqSend
 *
 *  DESCRIPTION
 *      Try to make a connection the a peer device.
 *
 *    PARAMETERS
 *----------------------------------------------------------------------------*/
#define CsrBtMapsDeactivateReqSend(_mapsInstanceId) {                   \
        CsrBtMapsDeactivateReq *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type = CSR_BT_MAPS_DEACTIVATE_REQ;                       \
        CsrBtMapsMsgTransport(_mapsInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsConnectResSend
 *
 *  DESCRIPTION
 *      ...
 *
 *    PARAMETERS
 *        obexMaxPacketSize;      ....
 *        responseCode;           ....
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapsConnectResSend(_mapsInstanceId, _responseCode) {       \
        CsrBtMapsConnectRes *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type = CSR_BT_MAPS_CONNECT_RES;                          \
        msg->responseCode = _responseCode;                            \
        CsrBtMapsMsgTransport(_mapsInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsSetBackFolderResSend
 *
 *  DESCRIPTION
 *      ...
 *
 *    PARAMETERS
 *        responseCode;           ....
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapsSetBackFolderResSend(_mapsInstanceId, _responseCode) { \
        CsrBtMapsSetBackFolderRes *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type = CSR_BT_MAPS_SET_BACK_FOLDER_RES;                  \
        msg->responseCode = _responseCode;                            \
        CsrBtMapsMsgTransport(_mapsInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsSetRootFolderResSend
 *
 *  DESCRIPTION
 *      ...
 *
 *    PARAMETERS
 *        responseCode;           ....
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapsSetRootFolderResSend(_mapsInstanceId, _responseCode) { \
        CsrBtMapsSetRootFolderRes *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type = CSR_BT_MAPS_SET_ROOT_FOLDER_RES;                  \
        msg->responseCode = _responseCode;                            \
        CsrBtMapsMsgTransport(_mapsInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsSetFolderResSend
 *
 *  DESCRIPTION
 *      ...
 *
 *    PARAMETERS
 *        responseCode;           ....
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapsSetFolderResSend(_mapsInstanceId, _responseCode) {     \
        CsrBtMapsSetFolderRes *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type = CSR_BT_MAPS_SET_FOLDER_RES;                       \
        msg->responseCode = _responseCode;                            \
        CsrBtMapsMsgTransport(_mapsInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsGetFolderListingHeaderResSend
 *
 *  DESCRIPTION
 *      ...
 *
 *    PARAMETERS
 *        fullFolderListingSize       ....
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapsGetFolderListingHeaderResSend(_mapsInstanceId, _fullFolderListingSize, _srmpOn) { \
        CsrBtMapsGetFolderListingHeaderRes *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type = CSR_BT_MAPS_GET_FOLDER_LISTING_HEADER_RES;        \
        msg->fullFolderListingSize = _fullFolderListingSize;          \
        msg->srmpOn = _srmpOn;                                        \
        CsrBtMapsMsgTransport(_mapsInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsGetFolderListingResSend
 *
 *  DESCRIPTION
 *      ...
 *
 *    PARAMETERS
 *        responseCode;           ....
 *        bodyLength;           ....
 *        *body;           ....
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapsGetFolderListingResSend(_mapsInstanceId, _responseCode, _bodyLength, _body, _srmpOn) { \
        CsrBtMapsGetFolderListingRes *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type = CSR_BT_MAPS_GET_FOLDER_LISTING_RES;               \
        msg->responseCode = _responseCode;                            \
        msg->bodyLength = _bodyLength;                                \
        msg->body = _body;                                            \
        msg->srmpOn = _srmpOn;                                        \
        CsrBtMapsMsgTransport(_mapsInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsGetMASInstanceInformationResSend
 *
 *  DESCRIPTION
 *      ...
 *
 *    PARAMETERS
 *        fullFolderListingSize       ....
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapsGetMasInstanceInformationResSend(_mapsInstanceId, _responseCode, _ownerUci, _bodyLength, _body, _srmpOn) { \
        CsrBtMapsGetMasInstanceInformationRes *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type = CSR_BT_MAPS_GET_MAS_INSTANCE_INFORMATION_RES;               \
        msg->responseCode = _responseCode;                            \
        msg->ownerUci = _ownerUci;                                    \
        msg->bodyLength = _bodyLength;                                \
        msg->body = _body;                                            \
        msg->srmpOn = _srmpOn;                                        \
        CsrBtMapsMsgTransport(_mapsInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsGetMessageListingHeaderResSend
 *
 *  DESCRIPTION
 *      ...
 *
 *    PARAMETERS
 *        newMessages               ....
 *       *mseTime                   ....
 *        fullMessageListingSize    ....
 *       *databaseId                ....
 *       *folderVersionCounter      ....
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapsGetMessageListingHeaderResSend(_mapsInstanceId, _newMessages, _mseTime, _fullMessageListingSize, _databaseId, _folderVersionCounter, _srmpOn) { \
        CsrBtMapsGetMessageListingHeaderRes *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type = CSR_BT_MAPS_GET_MESSAGE_LISTING_HEADER_RES;       \
        msg->newMessages = _newMessages;                              \
        msg->mseTime = _mseTime;                                      \
        msg->fullMessageListingSize = _fullMessageListingSize;        \
        msg->databaseId = _databaseId;                                \
        msg->folderVersionCounter = _folderVersionCounter;            \
        msg->srmpOn = _srmpOn;                                        \
        CsrBtMapsMsgTransport(_mapsInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsGetMessageListingResSend
 *
 *  DESCRIPTION
 *      ...
 *
 *    PARAMETERS
 *        responseCode;           ....
 *        bodyLength;           ....
 *        *body;           ....
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapsGetMessageListingResSend(_mapsInstanceId, _responseCode, _bodyLength, _body, _srmpOn) { \
        CsrBtMapsGetMessageListingRes *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type = CSR_BT_MAPS_GET_MESSAGE_LISTING_RES;              \
        msg->responseCode = _responseCode;                            \
        msg->bodyLength = _bodyLength;                                \
        msg->body = _body;                                            \
        msg->srmpOn = _srmpOn;                                        \
        CsrBtMapsMsgTransport(_mapsInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsGetMessageHeaderResSend
 *
 *  DESCRIPTION
 *      ...
 *
 *    PARAMETERS
 *        fractionDeliver      ....
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapsGetMessageHeaderResSend(_mapsInstanceId, _fractionDeliver, _srmpOn) { \
        CsrBtMapsGetMessageHeaderRes *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type = CSR_BT_MAPS_GET_MESSAGE_HEADER_RES;               \
        msg->fractionDeliver = _fractionDeliver;                      \
        msg->srmpOn = _srmpOn;                                        \
        CsrBtMapsMsgTransport(_mapsInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsGetMessageResSend
 *
 *  DESCRIPTION
 *      ...
 *
 *    PARAMETERS
 *        responseCode;           ....
 *        bodyLength;           ....
 *        *body;           ....
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapsGetMessageResSend(_mapsInstanceId, _responseCode, _bodyLength, _body, _srmpOn) { \
        CsrBtMapsGetMessageRes *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type = CSR_BT_MAPS_GET_MESSAGE_RES;                      \
        msg->responseCode = _responseCode;                            \
        msg->bodyLength = _bodyLength;                                \
        msg->body = _body;                                            \
        msg->srmpOn = _srmpOn;                                        \
        CsrBtMapsMsgTransport(_mapsInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsSetMessageStatusResSend
 *
 *  DESCRIPTION
 *      ...
 *
 *    PARAMETERS
 *        responseCode;           ....
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapsSetMessageStatusResSend(_mapsInstanceId, _responseCode, _srmpOn) { \
        CsrBtMapsSetMessageStatusRes *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type = CSR_BT_MAPS_SET_MESSAGE_STATUS_RES;               \
        msg->responseCode = _responseCode;                            \
        msg->srmpOn = _srmpOn;                                        \
        CsrBtMapsMsgTransport(_mapsInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsPushMessageHeaderResSend
 *
 *  DESCRIPTION
 *      ...
 *
 *    PARAMETERS
 *        messageHandler;           ....
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapsPushMessageHeaderResSend(_mapsInstanceId, _messageHandle, _srmpOn) { \
        CsrBtMapsPushMessageHeaderRes *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type          = CSR_BT_MAPS_PUSH_MESSAGE_HEADER_RES;     \
        msg->messageHandle = _messageHandle;                          \
        msg->srmpOn        = _srmpOn;                                 \
        CsrBtMapsMsgTransport(_mapsInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsPushMessageResSend
 *      CsrBtMapsPushMessageFinalResSend
 *  DESCRIPTION
 *      ...
 *
 *    PARAMETERS
 *        responseCode;           ....
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapsPushMessageFinalResSend(_mapsInstanceId, _responseCode, _srmpOn, _messageHandle) { \
        CsrBtMapsPushMessageRes *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type          = CSR_BT_MAPS_PUSH_MESSAGE_RES;            \
        msg->responseCode  = _responseCode;                           \
        msg->srmpOn        = _srmpOn;                                 \
        msg->messageHandle = _messageHandle;                          \
        CsrBtMapsMsgTransport(_mapsInstanceId, msg);}

#define CsrBtMapsPushMessageResSend(_mapsInstanceId, _responseCode, _srmpOn) CsrBtMapsPushMessageFinalResSend(_mapsInstanceId, _responseCode, _srmpOn, NULL)


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsUpdateInboxResSend
 *
 *  DESCRIPTION
 *      ...
 *
 *    PARAMETERS
 *        responseCode;           ....
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapsUpdateInboxResSend(_mapsInstanceId, _responseCode, _srmpOn) { \
        CsrBtMapsUpdateInboxRes *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type = CSR_BT_MAPS_UPDATE_INBOX_RES;                     \
        msg->responseCode = _responseCode;                            \
        msg->srmpOn = _srmpOn;                                        \
        CsrBtMapsMsgTransport(_mapsInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsNotificationRegistrationResSend
 *
 *  DESCRIPTION
 *      ...
 *
 *    PARAMETERS
 *        responseCode;           ....
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapsNotificationRegistrationResSend(_mapsInstanceId, _responseCode, _srmpOn) { \
        CsrBtMapsNotificationRegistrationRes *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type = CSR_BT_MAPS_NOTIFICATION_REGISTRATION_RES;        \
        msg->responseCode = _responseCode;                            \
        msg->srmpOn = _srmpOn;                                        \
        CsrBtMapsMsgTransport(_mapsInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsEventNotificationReqSend
 *
 *  DESCRIPTION
 *      ...
 *
 *    PARAMETERS
 *      ...
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapsEventNotificationReqSend(_mapsInstanceId) {            \
        CsrBtMapsEventNotificationReq *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type = CSR_BT_MAPS_EVENT_NOTIFICATION_REQ;               \
        msg->instanceId = _mapsInstanceId;                            \
        CsrBtMapsMsgTransport(CSR_BT_MAPS_IFACEQUEUE, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsEventNotificationResSend
 *
 *  DESCRIPTION
 *      ...
 *
 *    PARAMETERS
 *        finalFlag;           ....
 *        bodyLength;           ....
 *        *body;           ....
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapsEventNotificationResSend(_mapsInstanceId, _responseCode, _bodyLength, _body) { \
        CsrBtMapsEventNotificationRes *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type = CSR_BT_MAPS_EVENT_NOTIFICATION_RES;               \
        msg->responseCode = _responseCode;                            \
        msg->bodyLength = _bodyLength;                                \
        msg->body = _body;                                            \
        msg->instanceId = _mapsInstanceId;                            \
        CsrBtMapsMsgTransport(CSR_BT_MAPS_IFACEQUEUE, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsSecurityLevelReqSend
 *
 *  DESCRIPTION
 *      Set the default security settings for new incoming/outgoing connections
 *
 *  PARAMETERS
 *       secLevel        The security level to use
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapsSecurityLevelReqSend(_mapsInstanceId,_appHandle, _secLevel) { \
        CsrBtMapsSecurityLevelReq *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type = CSR_BT_MAPS_SECURITY_LEVEL_REQ;                   \
        msg->appHandle = _appHandle;                                  \
        msg->secLevel = _secLevel;                                    \
        CsrBtMapsMsgTransport(_mapsInstanceId,msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsSetOwnerStatusResSend
 *
 *  DESCRIPTION
 *      Send response to Set owner status.
 *
 *  PARAMETERS
 *        responseCode      ....
 *        srmpOn            ....
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapsSetOwnerStatusResSend(_mapsInstanceId, _responseCode, _srmpOn) { \
        CsrBtMapsSetOwnerStatusRes *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type = CSR_BT_MAPS_SET_OWNER_STATUS_RES;                 \
        msg->responseCode = _responseCode;                            \
        msg->srmpOn = _srmpOn;                                        \
        CsrBtMapsMsgTransport(_mapsInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsGetOwnerStatusResSend
 *
 *  DESCRIPTION
 *      Send response to Get owner status.
 *
 *    PARAMETERS
 *        responseCode              ....
 *        presenceAvailability      ....
 *       *presenceText              ....
 *       *lastActivity              ....
 *        chatState                 ....
 *        srmpOn                    ....
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapsGetOwnerStatusResSend(_mapsInstanceId, _responseCode, _presenceAvailability, _presenceText, _lastActivity, _chatState, _srmpOn) { \
        CsrBtMapsGetOwnerStatusRes *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type = CSR_BT_MAPS_GET_OWNER_STATUS_RES;                 \
        msg->responseCode = _responseCode;                            \
        msg->presenceAvailability = _presenceAvailability;            \
        msg->presenceText = _presenceText;                            \
        msg->lastActivity = _lastActivity;                            \
        msg->chatState = _chatState;                                  \
        msg->srmpOn = _srmpOn;                                        \
        CsrBtMapsMsgTransport(_mapsInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsGetConversationListingHeaderResSend
 *
 *  DESCRIPTION
 *      Send response to Get conversation listing header.
 *
 *    PARAMETERS
 *        responseCode                  ....
 *        fullMessageListingSize        ....
 *       *mseTime                       ....
 *       *convListingVersionCounter     ....
 *       *databaseId                    ....
 *        srmpOn                        ....
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapsGetConversationListingHeaderResSend(_mapsInstanceId, _listingSize, _mseTime, _convListingVersionCounter, _databaseId, _srmpOn) { \
        CsrBtMapsGetConversationListingHeaderRes *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type = CSR_BT_MAPS_GET_CONVERSATION_LISTING_HEADER_RES;               \
        msg->listingSize = _listingSize;                              \
        msg->mseTime = _mseTime;                                      \
        msg->convListingVersionCounter = _convListingVersionCounter;  \
        msg->databaseId = _databaseId;                                \
        msg->srmpOn = _srmpOn;                                        \
        CsrBtMapsMsgTransport(_mapsInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsGetConversationListingResSend
 *
 *  DESCRIPTION
 *      Send response to Get conversation listing.
 *
 *    PARAMETERS
 *        responseCode      ....
 *        bodyLength        ....
 *       *body              ....
 *        srmpOn            ....
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapsGetConversationListingResSend(_mapsInstanceId, _responseCode, _bodyLength, _body, _srmpOn) { \
        CsrBtMapsGetConversationListingRes *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type = CSR_BT_MAPS_GET_CONVERSATION_LISTING_RES;               \
        msg->responseCode = _responseCode;                            \
        msg->bodyLength = _bodyLength;                                \
        msg->body = _body;                                            \
        msg->srmpOn = _srmpOn;                                        \
        CsrBtMapsMsgTransport(_mapsInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsSetNotificationFilterResSend
 *
 *  DESCRIPTION
 *      Send response to Set notification filter.
 *
 *    PARAMETERS
 *        responseCode      ....
 *        srmpOn            ....
 *
 *----------------------------------------------------------------------------*/
#define CsrBtMapsSetNotificationFilterResSend(_mapsInstanceId, _responseCode, _srmpOn) { \
        CsrBtMapsSetNotificationFilterRes *msg = CsrPmemAlloc(sizeof(*msg));   \
        msg->type = CSR_BT_MAPS_SET_NOTIFICATION_FILTER_RES;               \
        msg->responseCode = _responseCode;                            \
        msg->srmpOn = _srmpOn;                                        \
        CsrBtMapsMsgTransport(_mapsInstanceId, msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtMapsFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT Maps
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_MAPS_PRIM,
 *      msg:          The message received from Synergy BT Maps
 *----------------------------------------------------------------------------*/
void CsrBtMapsFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);


#ifdef __cplusplus
}
#endif

#endif
