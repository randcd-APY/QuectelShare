#ifndef CSR_BT_MAPS_PRIM_H__
#define CSR_BT_MAPS_PRIM_H__

/******************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_unicode.h"
#include "csr_bt_profiles.h"
#include "csr_bt_obex.h"
#include "csr_bt_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtMapsPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim    CsrBtMapsPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST                               (0x0000)

#define CSR_BT_MAPS_GET_INSTANCE_IDS_REQ                ((CsrBtMapsPrim) (0x0000 + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_SELECT_MAS_INSTANCE_RES             ((CsrBtMapsPrim) (0x0001 + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_ACTIVATE_REQ                        ((CsrBtMapsPrim) (0x0002 + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_DEACTIVATE_REQ                      ((CsrBtMapsPrim) (0x0003 + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_CONNECT_RES                         ((CsrBtMapsPrim) (0x0004 + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_SET_BACK_FOLDER_RES                 ((CsrBtMapsPrim) (0x0005 + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_SET_ROOT_FOLDER_RES                 ((CsrBtMapsPrim) (0x0006 + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_SET_FOLDER_RES                      ((CsrBtMapsPrim) (0x0007 + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_GET_FOLDER_LISTING_HEADER_RES       ((CsrBtMapsPrim) (0x0008 + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_GET_FOLDER_LISTING_RES              ((CsrBtMapsPrim) (0x0009 + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_GET_MESSAGE_LISTING_HEADER_RES      ((CsrBtMapsPrim) (0x000A + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_GET_MESSAGE_LISTING_RES             ((CsrBtMapsPrim) (0x000B + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_GET_MESSAGE_HEADER_RES              ((CsrBtMapsPrim) (0x000C + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_GET_MESSAGE_RES                     ((CsrBtMapsPrim) (0x000D + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_SET_MESSAGE_STATUS_RES              ((CsrBtMapsPrim) (0x000E + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_PUSH_MESSAGE_HEADER_RES             ((CsrBtMapsPrim) (0x000F + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_PUSH_MESSAGE_RES                    ((CsrBtMapsPrim) (0x0010 + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_UPDATE_INBOX_RES                    ((CsrBtMapsPrim) (0x0011 + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_NOTIFICATION_REGISTRATION_RES       ((CsrBtMapsPrim) (0x0012 + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_EVENT_NOTIFICATION_REQ              ((CsrBtMapsPrim) (0x0013 + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_EVENT_NOTIFICATION_RES              ((CsrBtMapsPrim) (0x0014 + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_SECURITY_LEVEL_REQ                  ((CsrBtMapsPrim) (0x0015 + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_REGISTER_QID_REQ                    ((CsrBtMapsPrim) (0x0016 + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_GET_MAS_INSTANCE_INFORMATION_RES    ((CsrBtMapsPrim) (0x0017 + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_SET_OWNER_STATUS_RES                ((CsrBtMapsPrim) (0x0018 + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_GET_OWNER_STATUS_RES                ((CsrBtMapsPrim) (0x0019 + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_GET_CONVERSATION_LISTING_HEADER_RES ((CsrBtMapsPrim) (0x001A + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_GET_CONVERSATION_LISTING_RES        ((CsrBtMapsPrim) (0x001B + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_SET_NOTIFICATION_FILTER_RES         ((CsrBtMapsPrim) (0x001C + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MAPS_PRIM_DOWNSTREAM_HIGHEST                              (0x001C + CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST                                 (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_MAPS_GET_INSTANCE_IDS_CFM                    ((CsrBtMapsPrim) (0x0000 + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_SELECT_MAS_INSTANCE_IND                 ((CsrBtMapsPrim) (0x0001 + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_ACTIVATE_CFM                            ((CsrBtMapsPrim) (0x0002 + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_DEACTIVATE_CFM                          ((CsrBtMapsPrim) (0x0003 + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_CONNECT_IND                             ((CsrBtMapsPrim) (0x0004 + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_DISCONNECT_IND                          ((CsrBtMapsPrim) (0x0005 + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_ABORT_IND                               ((CsrBtMapsPrim) (0x0006 + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_SET_BACK_FOLDER_IND                     ((CsrBtMapsPrim) (0x0007 + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_SET_ROOT_FOLDER_IND                     ((CsrBtMapsPrim) (0x0008 + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_SET_FOLDER_IND                          ((CsrBtMapsPrim) (0x0009 + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_GET_FOLDER_LISTING_HEADER_IND           ((CsrBtMapsPrim) (0x000A + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_GET_FOLDER_LISTING_IND                  ((CsrBtMapsPrim) (0x000B + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_GET_MESSAGE_LISTING_HEADER_IND          ((CsrBtMapsPrim) (0x000C + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_GET_MESSAGE_LISTING_IND                 ((CsrBtMapsPrim) (0x000D + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_GET_MESSAGE_HEADER_IND                  ((CsrBtMapsPrim) (0x000E + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_GET_MESSAGE_IND                         ((CsrBtMapsPrim) (0x000F + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_SET_MESSAGE_STATUS_IND                  ((CsrBtMapsPrim) (0x0010 + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_PUSH_MESSAGE_HEADER_IND                 ((CsrBtMapsPrim) (0x0011 + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_PUSH_MESSAGE_IND                        ((CsrBtMapsPrim) (0x0012 + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_UPDATE_INBOX_IND                        ((CsrBtMapsPrim) (0x0013 + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_NOTIFICATION_REGISTRATION_IND           ((CsrBtMapsPrim) (0x0014 + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_NOTIFICATION_REGISTRATION_COMPLETE_IND  ((CsrBtMapsPrim) (0x0015 + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_EVENT_NOTIFICATION_IND                  ((CsrBtMapsPrim) (0x0016 + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_EVENT_NOTIFICATION_CFM                  ((CsrBtMapsPrim) (0x0017 + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_SECURITY_LEVEL_CFM                      ((CsrBtMapsPrim) (0x0018 + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_GET_MAS_INSTANCE_INFORMATION_IND        ((CsrBtMapsPrim) (0x0019 + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_SET_OWNER_STATUS_IND                    ((CsrBtMapsPrim) (0x001A + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_GET_OWNER_STATUS_IND                    ((CsrBtMapsPrim) (0x001B + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_GET_CONVERSATION_LISTING_HEADER_IND     ((CsrBtMapsPrim) (0x001C + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_GET_CONVERSATION_LISTING_IND            ((CsrBtMapsPrim) (0x001D + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_SET_NOTIFICATION_FILTER_IND             ((CsrBtMapsPrim) (0x001E + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MAPS_PRIM_UPSTREAM_HIGHEST                                    (0x001E + CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_MAPS_PRIM_DOWNSTREAM_COUNT               (CSR_BT_MAPS_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_MAPS_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_MAPS_PRIM_UPSTREAM_COUNT                 (CSR_BT_MAPS_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_MAPS_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/
/*** MAPS MULTI INSTANCE HANDLING ***/
typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             appHandle;                      /* application handle */
} CsrBtMapsGetInstanceIdsReq;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
     CsrUint8               instanceIdsListSize;            /* number of _items_ in instanceIdsList, _not_ length in bytes */
     CsrSchedQid            *instanceIdsList;               /* list of instance */
} CsrBtMapsGetInstanceIdsCfm;

typedef struct
{
    CsrBtDeviceName         serviceName;                    /* ServiceName from SDP record */
    CsrUint8                masInstanceId;                  /* MASInstanceID from SDP record */
    CsrBtMapMesSupport      supportedMessages;              /* bit pattern of supported message types in this MAS instance */
} CsrBtMapsMasInstance;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
    CsrBtMapsMasInstance    *masInstanceList;               /* pointer to the list of available MAS instances on the peer */
    CsrUint16               masInstanceListSize;            /* Number of items in masInstanceList */
} CsrBtMapsSelectMasInstanceInd;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrBool                 proceedWithConnection;          /* if TRUE the connection establishment is continued with the given masInstanceId */
    CsrUint8                masInstanceId;                  /* MASInstanceID to connect */
} CsrBtMapsSelectMasInstanceRes;


/*** CONNECTION HANDLING ***/
typedef struct
{
    CsrBtMapsPrim               type;                           /* primitive type */
    CsrSchedQid                 appHandle;                      /* application handle */
    CsrBtMapMesSupport          supportedMessages;              /* bit pattern of supported message types in the server */
    CsrUint16                   obexMaxPacketSize;
    CsrUint16                   windowSize;
    CsrBool                     srmEnable;
    CsrUint8                    instanceId;                     /* MASInstanceID to register */
    CsrUint16                   nameLen;                        /* Length of the name of the MASInstance */
    CsrUint8                    *name;                          /* Name of the MASInstance */
    CsrBtMapSupportedFeatures   supportedFeatures;              /* bit pattern of MAP Server instance supported features */
} CsrBtMapsActivateReq;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtMapsActivateCfm;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
} CsrBtMapsDeactivateReq;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
} CsrBtMapsDeactivateCfm;

typedef struct
{
    CsrBtMapsPrim               type;                       /* primitive type */
    CsrSchedQid                 instanceId;                 /* id of the MAPS instance that generated this event */
    CsrBtDeviceAddr             deviceAddr;
    CsrUint32                   length;
    CsrUint32                   count;
    CsrBtMapSupportedFeatures   mapSupportedFeatures;       /* MCE MAP supported features; Valid value only if remote MCE is v1.3 or higher else has zero */
    CsrBtConnId                 btConnId;                   /* Global Bluetooth connection ID */
} CsrBtMapsConnectInd;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
    CsrBtDeviceAddr         deviceAddr;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtMapsNotificationRegistrationCompleteInd;

typedef struct
{
    CsrBtMapsPrim           type;
    CsrBtObexResponseCode   responseCode;
} CsrBtMapsConnectRes;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
    CsrBtReasonCode         reasonCode;
    CsrBtSupplier           reasonSupplier;
} CsrBtMapsDisconnectInd;

typedef struct
{
    CsrBtMapsPrim           type;
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
    CsrUint16               descriptionOffset;              /* Payload relative offset */
    CsrUint16               descriptionLength;
    CsrUint8              *payload;
    CsrUint16               payloadLength;
} CsrBtMapsAbortInd;


/*** BROWSING ***/
typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
    CsrUcs2String           *folderName;                    /* Name header. Equals NULL if not name header is present */
} CsrBtMapsSetBackFolderInd;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrBtObexResponseCode   responseCode;                   /* OBEX response to the sync command */
} CsrBtMapsSetBackFolderRes;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
} CsrBtMapsSetRootFolderInd;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrBtObexResponseCode   responseCode;                   /* OBEX response to the sync command */
} CsrBtMapsSetRootFolderRes;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
    CsrUcs2String           *folderName;                    /* Name header. Equals NULL if not name header is present */
} CsrBtMapsSetFolderInd;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrBtObexResponseCode   responseCode;                   /* OBEX response to the sync command */
} CsrBtMapsSetFolderRes;


/*** GETTING ***/
typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
    CsrUint16               maxListCount;                   /* maximum number of folders in the listing */
    CsrUint16               listStartOffset;                /* offset of where to start the listing */
} CsrBtMapsGetFolderListingHeaderInd;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrUint16               fullFolderListingSize;          /* number of bytes in the complete folder listing */
    CsrBool                 srmpOn;
} CsrBtMapsGetFolderListingHeaderRes;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
    CsrUint16               obexResponsePacketLength;       /* Maximum allowed body length of the corresponding response */
} CsrBtMapsGetFolderListingInd;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrBtObexResponseCode   responseCode;                   /* OBEX response to the sync command */
    CsrUint16               bodyLength;
    CsrUint8                *body;
    CsrBool                 srmpOn;
} CsrBtMapsGetFolderListingRes;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
    CsrUcs2String           *folderName;                    /* null terminated name string of the folder from where the current message listing is to be retreived: NB: if NULL this means current folder */
    CsrUint16               maxListCount;                   /* maximum number of messages in the listing */
    CsrUint16               listStartOffset;                /* offset of where to start the listing */
    CsrUint8                maxSubjectLength;               /* maximum string length allowed of the subject field */
    CsrBtMapMesParms        parameterMask;                  /* bitmask of relevant parameters for the message listing. NB: a bit value of 1 means that the parameter should be present and a value of 0 means it should be filtered out */
    CsrBtMapMesTypes        filterMessageType;              /* bitmask specifying which message types to include/exclude in the listing. NB: a bit value of 1 means that the message type should be filtered and a value of 0 means that it should be present */
    CsrUtf8String           *filterPeriodBegin;             /* null terminated time string */
    CsrUtf8String           *filterPeriodEnd;               /* null terminated time string */
    CsrBtMapReadStatus      filterReadStatus;               /* bitmask specifying if filtering should be done on behalf of the read status */
    CsrUtf8String           *filterRecipient;               /* null terminated recipient string */
    CsrUtf8String           *filterOriginator;              /* null terminated originator string */
    CsrBtMapPriority        filterPriority;                 /* bitmask specifying which priority type to include in the listing */
    CsrUtf8String           *conversationId;                /* null terminated conversation id string */
    CsrUtf8String           *filterMessageHandle;           /* null terminated message handle string */
} CsrBtMapsGetMessageListingHeaderInd;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrBtMapNewMessage      newMessages;                    /* specifies if there's unread messages on the MSE or not */
    CsrUtf8String           *mseTime;                       /* current time basis and UTC-offset. Null terminated time string */
    CsrUint16               fullMessageListingSize;         /* number of bytes in the complete message listing */
    CsrUtf8String           *databaseId;                    /* 128-bit value in hex string format */
    CsrUtf8String           *folderVersionCounter;          /* 128-bit value in hex string format */
    CsrBool                 srmpOn;
} CsrBtMapsGetMessageListingHeaderRes;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
    CsrUint16               obexResponsePacketLength;       /* Maximum allowed body length of the corresponding response */
} CsrBtMapsGetMessageListingInd;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrBtObexResponseCode   responseCode;                   /* OBEX response to the sync command */
    CsrUint16               bodyLength;
    CsrUint8                *body;
    CsrBool                 srmpOn;
} CsrBtMapsGetMessageListingRes;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
    CsrUcs2String           *messageHandle;                 /* null terminated message handle string */
    CsrBtMapAttachment      attachment;                     /* bitmask specifying whether to include attachment or not */
    CsrBtMapCharset         charset;                        /* bitmask used to specifying trans-coding of the message */
    CsrBtMapFracReq         fractionRequest;                /* bitmask which fragment of the message to get _if any_ */
} CsrBtMapsGetMessageHeaderInd;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrBtMapFracDel         fractionDeliver;                /* bitmask specifying the fragment status of the message retrieved if sent by peer */
    CsrBool                 srmpOn;
} CsrBtMapsGetMessageHeaderRes;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
    CsrUint16               obexResponsePacketLength;       /* Maximum allowed body length of the corresponding response */
} CsrBtMapsGetMessageInd;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrBtObexResponseCode   responseCode;                   /* OBEX response to the sync command */
    CsrUint16               bodyLength;
    CsrUint8                *body;
    CsrBool                 srmpOn;
} CsrBtMapsGetMessageRes;


/*** PUTTING ***/
typedef struct
{
    CsrBtMapsPrim               type;                       /* primitive type */
    CsrSchedQid                 instanceId;                 /* id of the MAPS instance that generated this event */
    CsrUcs2String               *messageHandle;             /* null terminated message handle string */
    CsrBtMapStatusInd           statusIndicator;            /* specifies which status information to modify */
    CsrBtMapStatusVal           statusValue;                /* specifies the new value of the status indication in question */
    CsrUtf8String               *extendedData;              /* null terminated extended data string */
} CsrBtMapsSetMessageStatusInd;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrBtObexResponseCode   responseCode;                   /* OBEX response to the sync command */
    CsrBool                 srmpOn;
} CsrBtMapsSetMessageStatusRes;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
    CsrUcs2String           *folderName;                    /* null terminated string specifying the name of the folder where the message should be pushed */
    CsrBtMapTrans           transparent;                    /* specifies if the MSE should keep a copy of the message in the sent folder */
    CsrBtMapRetry           retry;                          /* specifies if the MSE should try to resent if first delivery to the network fails */
    CsrBtMapCharset         charset;                        /* used to specify the format of the content delivered */
    CsrUtf8String           *conversationId;                /* null terminated conversation id string */
} CsrBtMapsPushMessageHeaderInd;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrCharString           *messageHandle;                 /* null terminated string specifying message handle assigned by the MSE */
    CsrBool                 srmpOn;
} CsrBtMapsPushMessageHeaderRes;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
    CsrBool                 finalFlag;
    CsrUint16               bodyLength;
    CsrUint8                *body;
} CsrBtMapsPushMessageInd;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrBtObexResponseCode   responseCode;                   /* OBEX response to the sync command */
    CsrBool                 srmpOn;
    CsrCharString           *messageHandle;                 /* null terminated string specifying message handle assigned by the MSE */
} CsrBtMapsPushMessageRes;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
} CsrBtMapsUpdateInboxInd;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrBtObexResponseCode   responseCode;                   /* OBEX response to the sync command */
    CsrBool                 srmpOn;
} CsrBtMapsUpdateInboxRes;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
    CsrBool                 notificationStatus;             /* Boolean specifying if the notification service should be switched on or off */
} CsrBtMapsNotificationRegistrationInd;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrBtObexResponseCode   responseCode;                   /* OBEX response to the sync command */
    CsrBool                 srmpOn;
} CsrBtMapsNotificationRegistrationRes;


/*** EVENT NOTIFICATION ***/
typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
} CsrBtMapsEventNotificationReq;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
    CsrUint16               obexResponsePacketLength;       /* Maximum allowed body length of the corresponding response */
} CsrBtMapsEventNotificationInd;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
    CsrBtObexResponseCode   responseCode;                   /* OBEX response */
    CsrUint16               bodyLength;
    CsrUint8                *body;
} CsrBtMapsEventNotificationRes;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtMapsEventNotificationCfm;


/*** SECURITY ***/
typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             appHandle;                      /* application handle */
    CsrUint16               secLevel;                       /* outgoing security level */
} CsrBtMapsSecurityLevelReq;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtMapsSecurityLevelCfm;


/*** MAPS Internal Qid registration ***/
typedef struct
{
   CsrBtMapsPrim            type;                            /* primitive type */
   CsrSchedQid              mapsInstanceId;                  /* application handle */
   /* This is only to pass the serializer script */
   CsrUint16                mapsPrivInstanceLength;
   CsrUint8                *mapsPrivInstance;               /* instance private instance */
} CsrBtMapsRegisterQidReq;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
} CsrBtMapsGetMasInstanceInformationInd;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrBtObexResponseCode   responseCode;                   /* OBEX response to the sync command */
    CsrUtf8String           *ownerUci;                      /* null terminated UCI string */
    CsrUint16               bodyLength;
    CsrUint8                *body;
    CsrBool                 srmpOn;
} CsrBtMapsGetMasInstanceInformationRes;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
    CsrBtMapPresence        presenceAvailability;           /* MAP Presence availability state */
    CsrUtf8String           *presenceText;                  /* a description of the current status of the user */
    CsrUtf8String           *lastActivity;                  /* the date time of the owner's last activity */
    CsrBtMapChatState       chatState;                      /* the chat state of the owner */
    CsrUtf8String           *conversationId;                /* If NULL then chat state of the owner is conversation independent */
} CsrBtMapsSetOwnerStatusInd;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrBtObexResponseCode   responseCode;                   /* OBEX response to the sync command */
    CsrBool                 srmpOn;
} CsrBtMapsSetOwnerStatusRes;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
    CsrUtf8String           *conversationId;                /* If NULL then chat state of the owner is conversation independent */
} CsrBtMapsGetOwnerStatusInd;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrBtObexResponseCode   responseCode;                   /* OBEX response to the sync command */
    CsrBool                 srmpOn;
    CsrBtMapPresence        presenceAvailability;           /* MAP Presence availability state */
    CsrUtf8String           *presenceText;                  /* a description of the current status of the user */
    CsrUtf8String           *lastActivity;                  /* the date time of the owner's last activity */
    CsrBtMapChatState       chatState;                      /* the chat state of the owner */
} CsrBtMapsGetOwnerStatusRes;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
    CsrUint16               maxListCount;                   /* maximum number of messages in the listing */
    CsrUint16               listStartOffset;                /* offset of where to start the listing */
    CsrUtf8String           *filterLastActivityBegin;       /* null terminated time string */
    CsrUtf8String           *filterLastActivityEnd;         /* null terminated time string */
    CsrBtMapReadStatus      filterReadStatus;               /* bitmask specifying if filtering should be done on behalf of the read status */
    CsrUtf8String           *filterRecipient;               /* null terminated recipient string */
    CsrUtf8String           *conversationId;                /* If NULL then chat state of the owner is conversation independent */
    CsrBtMapConvParams      convParameterMask;              /* bitmask of relevant parameters for the conversation listing. NB: a bit value of 1 means that the parameter should be present and a value of 0 means it should be filtered out */
} CsrBtMapsGetConversationListingHeaderInd;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrUint16               listingSize;                    /* number of conversations in the complete message listing */
    CsrUtf8String           *mseTime;                       /* current time basis and UTC-offset. Null terminated time string */
    CsrUtf8String           *convListingVersionCounter;     /* 128-bit value in hex string format */
    CsrUtf8String           *databaseId;                    /* 128-bit value in hex string format */
    CsrBool                 srmpOn;
} CsrBtMapsGetConversationListingHeaderRes;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrSchedQid             instanceId;                     /* id of the MAPS instance that generated this event */
    CsrUint16               obexResponsePacketLength;       /* Maximum allowed body length of the corresponding response */
} CsrBtMapsGetConversationListingInd;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrBtObexResponseCode   responseCode;                   /* OBEX response to the sync command */
    CsrUint16               bodyLength;
    CsrUint8                *body;
    CsrBool                 srmpOn;
} CsrBtMapsGetConversationListingRes;

typedef struct
{
    CsrBtMapsPrim                   type;                   /* primitive type */
    CsrSchedQid                     instanceId;             /* id of the MAPS instance that generated this event */
    CsrBtMapNotificationFilterMask  notificationFilterMask;
} CsrBtMapsSetNotificationFilterInd;

typedef struct
{
    CsrBtMapsPrim           type;                           /* primitive type */
    CsrBtObexResponseCode   responseCode;                   /* OBEX response to the sync command */
    CsrBool                 srmpOn;
} CsrBtMapsSetNotificationFilterRes;

#ifdef __cplusplus
}
#endif

#endif

