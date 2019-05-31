#ifndef CSR_BT_SYNCC_PRIM_H__
#define CSR_BT_SYNCC_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2010 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_unicode.h"
#include "csr_bt_result.h"
#include "csr_bt_profiles.h"
#include "csr_bt_obex.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SYNC_VCARD_EXTENSION_NAME                   ".vcf"
#define SYNC_VCALENDAR_EXTENSION_NAME               ".vcs"
#define SYNC_MESSAGING_EXTENSION_NAME               ".vmg"
#define SYNC_NOTES_EXTENSION_NAME                   ".vnt"

#define SYNC_PHONE_BOOK_OBJECT_STORE                "telecom/pb/"
#define SYNC_CALENDAR_OBJECT_STORE                  "telecom/cal/"
#define SYNC_MESSAGING_OBJECT_STORE                 "telecom/msg/"
#define SYNC_NOTES_OBJECT_STORE                     "telecom/nt/"

#define SYNC_DEVICE_INFORMATION_NAME                "telecom/devinfo.txt"
#define SYNC_REAL_TIME_CLOCK_NAME                   "telecom/rtc.txt"

#define SYNC_PHONE_BOOK_NAME                        "telecom/pb.vcf"
#define SYNC_PHONE_BOOK_INFORMATION_LOG_NAME        "telecom/pb/info.log"
#define SYNC_PHONE_BOOK_CHANGE_COUNTER_NAME         "telecom/pb/luid/cc.log"
#define SYNC_PHONE_BOOK_INCOMING_CALL_HISTORY_NAME  "telecom/pb/ich.vcf"
#define SYNC_PHONE_BOOK_OUTGOING_CALL_HISTORY_NAME  "telecom/pb/och.vcf"
#define SYNC_PHONE_BOOK_MISSED_CALL_HISTORY_NAME    "telecom/pb/mch.vcf"
#define SYNC_PHONE_BOOK_OWNERS_VCARD_STATIC_NAME    "telecom/pb/0.vcf"
#define SYNC_PHONE_BOOK_OWNERS_VCARD_UNIQUE_NAME    "telecom/pb/luid/0.vcf"

#define SYNC_CALENDAR_NAME                          "telecom/cal.vcs"
#define SYNC_CALENDAR_INFORMATION_LOG_NAME          "telecom/cal/info.log"
#define SYNC_CALENDAR_CHANGE_COUNTER_NAME           "telecom/cal/luid/cc.log"

#define SYNC_MESSAGING_INCOMING_NAME                "telecom/msg/in.vmg"
#define SYNC_MESSAGING_OUTGOING_NAME                "telecom/msg/out.vmg"
#define SYNC_MESSAGING_SENT_NAME                    "telecom/msg/sent.vmg"
#define SYNC_MESSAGING_IN_INFORMATION_LOG_NAME      "telecom/msg/in/info.log"
#define SYNC_MESSAGING_OUT_INFORMATION_LOG_NAME     "telecom/msg/out/info.log"
#define SYNC_MESSAGING_SENT_INFORMATION_LOG_NAME    "telecom/msg/sent/info.log"
#define SYNC_MESSAGING_IN_CHANGE_COUNTER_NAME       "telecom/msg/in/luid/cc.log"
#define SYNC_MESSAGING_OUT_CHANGE_COUNTER_NAME      "telecom/msg/out/luid/cc.log"
#define SYNC_MESSAGING_SENT_CHANGE_COUNTER_NAME     "telecom/msg/sent/luid/cc.log"
#define SYNC_MESSAGING_MISSED_MESSAGES_HISTORY_NAME "telecom/mmh.vmg"

#define SYNC_NOTES_NAME                             "telecom/nt.vnt"
#define SYNC_NOTES_INFORMATION_LOG_NAME             "telecom/nt/info.log"
#define SYNC_NOTES_CHANGE_COUNTER_NAME              "telecom/nt/luid/cc.log"


/* search_string="CsrBtSynccPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */
typedef CsrPrim    CsrBtSynccPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_SYNCC_PRIM_DOWNSTREAM_LOWEST                       (0x0000)

#define CSR_BT_SYNCC_CONNECT_REQ                 ((CsrBtSynccPrim) (0x0000 + CSR_BT_SYNCC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCC_CANCEL_CONNECT_REQ          ((CsrBtSynccPrim) (0x0001 + CSR_BT_SYNCC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCC_DISCONNECT_REQ              ((CsrBtSynccPrim) (0x0002 + CSR_BT_SYNCC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCC_AUTHENTICATE_RES            ((CsrBtSynccPrim) (0x0003 + CSR_BT_SYNCC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCC_GET_OBJECT_REQ              ((CsrBtSynccPrim) (0x0004 + CSR_BT_SYNCC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCC_GET_OBJECT_RES              ((CsrBtSynccPrim) (0x0005 + CSR_BT_SYNCC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCC_ADD_OBJECT_REQ              ((CsrBtSynccPrim) (0x0006 + CSR_BT_SYNCC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCC_ADD_OBJECT_RES              ((CsrBtSynccPrim) (0x0007 + CSR_BT_SYNCC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCC_MODIFY_OBJECT_REQ           ((CsrBtSynccPrim) (0x0008 + CSR_BT_SYNCC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCC_MODIFY_OBJECT_RES           ((CsrBtSynccPrim) (0x0009 + CSR_BT_SYNCC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCC_DELETE_OBJECT_REQ           ((CsrBtSynccPrim) (0x000A + CSR_BT_SYNCC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCC_ABORT_REQ                   ((CsrBtSynccPrim) (0x000B + CSR_BT_SYNCC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCC_ACTIVATE_SYNC_COMMAND_REQ   ((CsrBtSynccPrim) (0x000C + CSR_BT_SYNCC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCC_SYNC_COMMAND_RES            ((CsrBtSynccPrim) (0x000D + CSR_BT_SYNCC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCC_DEACTIVATE_SYNC_COMMAND_REQ ((CsrBtSynccPrim) (0x000E + CSR_BT_SYNCC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCC_SECURITY_IN_REQ             ((CsrBtSynccPrim) (0x000F + CSR_BT_SYNCC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SYNCC_SECURITY_OUT_REQ            ((CsrBtSynccPrim) (0x0010 + CSR_BT_SYNCC_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_SYNCC_PRIM_DOWNSTREAM_HIGHEST                      (0x0010 + CSR_BT_SYNCC_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_SYNCC_PRIM_UPSTREAM_LOWEST                         (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_SYNCC_CONNECT_CFM                 ((CsrBtSynccPrim) (0x0000 + CSR_BT_SYNCC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCC_DISCONNECT_IND              ((CsrBtSynccPrim) (0x0001 + CSR_BT_SYNCC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCC_AUTHENTICATE_IND            ((CsrBtSynccPrim) (0x0002 + CSR_BT_SYNCC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCC_GET_OBJECT_IND              ((CsrBtSynccPrim) (0x0003 + CSR_BT_SYNCC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCC_GET_OBJECT_CFM              ((CsrBtSynccPrim) (0x0004 + CSR_BT_SYNCC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCC_ADD_OBJECT_IND              ((CsrBtSynccPrim) (0x0005 + CSR_BT_SYNCC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCC_ADD_OBJECT_CFM              ((CsrBtSynccPrim) (0x0006 + CSR_BT_SYNCC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCC_MODIFY_OBJECT_IND           ((CsrBtSynccPrim) (0x0007 + CSR_BT_SYNCC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCC_MODIFY_OBJECT_CFM           ((CsrBtSynccPrim) (0x0008 + CSR_BT_SYNCC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCC_DELETE_OBJECT_CFM           ((CsrBtSynccPrim) (0x0009 + CSR_BT_SYNCC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCC_ABORT_CFM                   ((CsrBtSynccPrim) (0x000A + CSR_BT_SYNCC_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_SYNCC_ACTIVATE_SYNC_COMMAND_CFM   ((CsrBtSynccPrim) (0x000B + CSR_BT_SYNCC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCC_SYNC_COMMAND_IND            ((CsrBtSynccPrim) (0x000C + CSR_BT_SYNCC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCC_SYNC_COMMAND_ABORT_IND      ((CsrBtSynccPrim) (0x000D + CSR_BT_SYNCC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCC_DEACTIVATE_SYNC_COMMAND_CFM ((CsrBtSynccPrim) (0x000E + CSR_BT_SYNCC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCC_SECURITY_IN_CFM             ((CsrBtSynccPrim) (0x000F + CSR_BT_SYNCC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SYNCC_SECURITY_OUT_CFM            ((CsrBtSynccPrim) (0x0010 + CSR_BT_SYNCC_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_SYNCC_PRIM_UPSTREAM_HIGHEST                        (0x0010 + CSR_BT_SYNCC_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_SYNCC_PRIM_DOWNSTREAM_COUNT       (CSR_BT_SYNCC_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_SYNCC_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_SYNCC_PRIM_UPSTREAM_COUNT         (CSR_BT_SYNCC_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_SYNCC_PRIM_UPSTREAM_LOWEST)

/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrSchedQid              appHandle;                      /* application handle */
    CsrUint16                maxPacketSize;                  /* maximum OBEX packet size intented to be used */
    CsrBtDeviceAddr          deviceAddr;                     /* device address to connect to */
    CsrBool                  authorize;                      /* request authentication against the server */
    CsrUint8                 realmLength;                    /* length of the realm string in bytes to use if the autorize parameter is TRUE */
    CsrUint8                *realm;                          /* realm information to use if the autorize parameter is TRUE, if NULL
                                                              * a userId will not be requested in the authentication challenge.
                                                              * NB: if used the first bytes of this stream must specify the charset as
                                                              * per specification */
    CsrUint16                passwordLength;
    CsrUint8                *password;                      /* password to use if the autorize parameter is TRUE. Max length
                                                             * is BT_OBEX_MAX_AUTH_USERID_LENGTH */
    CsrCharString           *userId;                        /* userid (ASCII string) to use if the autorize parameter is TRUE,
                                                             * if NULL a userId will not be requested in the authentication challenge).
                                                             * NB: Only the first 20 bytes are considered as per specification */
    CsrUint32                count;
    CsrUint32                length; 
    CsrUint16                windowSize;
    CsrBool                  srmEnable;
} CsrBtSynccConnectReq;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrBtSyncStores          supportedStores;                /* bit pattern of supported stores in the server */
    CsrUint16                obexPeerMaxPacketSize;          /* CSRMAX OBEX packet size supported by the server */
    CsrBtResultCode          resultCode;
    CsrBtSupplier            resultSupplier;
    CsrBtConnId              btConnId;                       /* Global Bluetooth connection ID */
} CsrBtSynccConnectCfm;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
} CsrBtSynccCancelConnectReq;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrBool                  normalObexDisconnect;           /* if TRUE an OBEX disconnect is issued, if FALSE the transport
                                                              * channel is torn down without sending the OBEX disconnect first */
} CsrBtSynccDisconnectReq;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrBtReasonCode          reasonCode;
    CsrBtSupplier            reasonSupplier;
} CsrBtSynccDisconnectInd;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrBtDeviceAddr          deviceAddr;                     /* device address which requested the authentication */
    CsrUint8                 options;
    CsrUint16                realmLength;
    CsrUint8                *realm;
} CsrBtSynccAuthenticateInd;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrUint8                *password;
    CsrUint16                passwordLength;
    CsrCharString           *userId;                       /*Max length is BT_OBEX_MAX_AUTH_USERID_LENGTH */
} CsrBtSynccAuthenticateRes;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrUcs2String            *ucs2name;                      /* Unicode name, zero-terminated */
    CsrBool                  srmpOn;
} CsrBtSynccGetObjectReq;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrBool                  srmpOn;
} CsrBtSynccGetObjectRes;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrUint16                bodyOffset;                     /* payload relative offset to where the body part starts.
                                                              * NB: Only valid if bodyLength > 0 */
    CsrUint16                bodyLength;                     /* length of the object body carried with this payload */
    CsrUint8                *payload;                       /* pointer to the complete OBEX payload received from the server */
    CsrUint16                payloadLength;                  /* total length of the payload */
} CsrBtSynccGetObjectInd;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrBtObexResponseCode    responseCode;                   /* OBEX responseCode of the GET request */
    CsrUint16                bodyOffset;                     /* payload relative offset to where the body part starts. NB: Only valid if bodyLength > 0 */
    CsrUint16                bodyLength;                     /* length of the object body carried with this payload */
    CsrUint8                *payload;                       /* pointer to the complete OBEX payload received from the server */
    CsrUint16                payloadLength;                  /* total length of the payload */
} CsrBtSynccGetObjectCfm;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrUcs2String           *objectName;                     /* unicode string containing the full path and name of the object to send */
    CsrUint32                lengthOfObject;                 /* total length of the object to send */
} CsrBtSynccAddObjectReq;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrUint16                maxAllowedPayloadSize;          /* the maximum allowed payload size to include in the CsrBtSynccAddObjectRes */
} CsrBtSynccAddObjectInd;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrBool                  finalFlag;                      /* set this to TRUE if this response carries the final part of the object */
    CsrUint8                *payload;                       /* payload to send */
    CsrUint16                payloadLength;                  /* length of the payload to send */
} CsrBtSynccAddObjectRes;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrBtObexResponseCode    responseCode;                   /* OBEX responseCode of the add operation */
    CsrUtf8String           *luid;                           /* luid returned by server if add was level 4 operation */
    CsrUtf8String           *changeCounter;                  /* new change counter returned by server if it support change counter sync anchors */
    CsrUtf8String           *timeStamp;                      /* new timestamp returned by server if it support timestamps as sync anchors */
} CsrBtSynccAddObjectCfm;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrUtf8String           *maxExpectedChangeCounter;       /* zero-terminated ASCII number string specifying the maximum expected
                                                              * change counter value e.g. "5678", NB: if NULL the change counter value is not sent */
    CsrUcs2String           *objectName;                     /* unicode string containing the full path and name of the object to modify */
    CsrUint32                lengthOfObject;                 /* total length of the object to send */
} CsrBtSynccModifyObjectReq;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrUint16                maxAllowedPayloadSize;          /* the maximum allowed payload size to include in the CsrBtSynccModifyObjectRes */
} CsrBtSynccModifyObjectInd;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrBool                  finalFlag;                      /* set this to TRUE if this response carries the final part of the object */
    CsrUint8                *payload;                       /* payload to send */
    CsrUint16                payloadLength;                  /* length of the payload to send */
} CsrBtSynccModifyObjectRes;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrBtObexResponseCode    responseCode;                   /* OBEX responseCode of the modify operation */
    CsrUtf8String           *luid;                          /* luid returned by server if modify was level 4 operation */
    CsrUtf8String           *changeCounter;                 /* new change counter returned by server if it support change counter sync anchors */
    CsrUtf8String           *timeStamp;                     /* new timestamp returned by server if it support timestamps as sync anchors */
} CsrBtSynccModifyObjectCfm;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrBool                  hardDelete;                     /* if TRUE a hard delete of the object is requested, if FALSE a soft delete is requested */
    CsrUtf8String           *maxExpectedChangeCounter;       /* zero-terminated ASCII number string specifying the maximum expected change
                                                              * counter value e.g. "5678", NB: if NULL the change counter value is not sent */
    CsrUcs2String            *objectName;                    /* unicode string containing the full path and name of the object to delete */
} CsrBtSynccDeleteObjectReq;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrBtObexResponseCode    responseCode;                   /* OBEX result of the delete operation */
    CsrUtf8String           *luid;                          /* luid returned by server if delete was level 4 operation */
    CsrUtf8String           *changeCounter;                 /* new change counter returned by server if it support change counter sync anchors */
    CsrUtf8String           *timeStamp;                     /* new timestamp returned by server if it support timestamps as sync anchors */
} CsrBtSynccDeleteObjectCfm;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
} CsrBtSynccAbortReq;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
} CsrBtSynccAbortCfm;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrSchedQid              appHandle;                      /* application handle */
    CsrUint16                maxPacketSize;                  /* maximum OBEX packet size intented to be used */
} CsrBtSynccActivateSyncCommandReq;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrBtResultCode          resultCode;
    CsrBtSupplier            resultSupplier;
} CsrBtSynccActivateSyncCommandCfm;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrUint8                 finalFlag;                      /* if TRUE this is the complete obex packet from the peer,
                                                              * if FALSE the rest of the packet can be retrieved by sending a
                                                              * CSR_BT_OBEX_CONTINUE_RESPONSE_CODE in CsrBtSynccSyncCommandRes */
    CsrUint32                lengthOfObject;                 /* lenght of the object sent from the peer. NB: If this carries the
                                                              * value 0 it means that the peer has not included it in the PUT request */
    CsrUint16                ucs2nameOffset;                 /* payload relative offset to where the zero-terminated unicode object
                                                              * name starts. NB: Only valid if != 0 */
    CsrUint16                bodyOffset;                     /* payload relative offset to where the body part starts.
                                                              * NB: Only valid if bodyLength > 0 */
    CsrUint16                bodyLength;                     /* length of the object body carried with this payload */
    CsrUint8                *payload;                        /* pointer to the complete OBEX payload received from the server */
    CsrUint16                payloadLength;                  /* total length of the payload */
} CsrBtSynccSyncCommandInd;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrBtObexResponseCode    responseCode;                   /* OBEX responseCode to the sync command */
    CsrBool                  srmpOn;
} CsrBtSynccSyncCommandRes;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrUint16                descriptionOffset;              /* payload relative offset to where the description part starts. NB: Only valid if descriptionLength > 0 */
    CsrUint16                descriptionLength;              /* length of the description carried with this payload */
    CsrUint8                *payload;                       /* pointer to the complete OBEX payload received from the server */
    CsrUint16                payloadLength;                  /* total length of the payload */
} CsrBtSynccSyncCommandAbortInd;

typedef struct
{
    CsrBtSynccPrim          type;                           /* primitive type */
} CsrBtSynccDeactivateSyncCommandReq;

typedef struct
{
    CsrBtSynccPrim          type;                           /* primitive type */
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtSynccDeactivateSyncCommandCfm;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrSchedQid              appHandle;                      /* application handle */
    CsrUint16                secLevel;                       /* outgoing security level */
} CsrBtSynccSecurityOutReq;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrBtResultCode          resultCode;
    CsrBtSupplier            resultSupplier;
} CsrBtSynccSecurityOutCfm;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrSchedQid              appHandle;                      /* application handle */
    CsrUint16                secLevel;                       /* ingoing security level */
} CsrBtSynccSecurityInReq;

typedef struct
{
    CsrBtSynccPrim           type;                           /* primitive type */
    CsrBtResultCode          resultCode;
    CsrBtSupplier            resultSupplier;
} CsrBtSynccSecurityInCfm;

#ifdef __cplusplus
}
#endif

#endif

