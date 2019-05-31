#ifndef CSR_BT_PAC_PRIM_H__
#define CSR_BT_PAC_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2010-2015 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/

#include "csr_types.h"
#include "csr_unicode.h"
#include "csr_bt_result.h"
#include "csr_bt_profiles.h"
#include "csr_bt_obex.h"
#include "csr_bt_pb_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtPacPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim                         CsrBtPacPrim;

#define CSR_BT_PAC_PROFILE_VERSION      (0x0102)

#define PAC_DOWNLOAD_SUPPORT            CSR_BT_PB_FEATURE_DOWNLOAD
#define PAC_BROWSING_SUPPORT            CSR_BT_PB_FEATURE_BROWSING

typedef CsrUint16                       CsrBtPacSrcType;
#define CSR_BT_PAC_SRC_NONE             (CsrBtPacSrcType)(CSR_BT_PB_REPO_NONE)
#define CSR_BT_PAC_SRC_PHONE            (CsrBtPacSrcType)(CSR_BT_PB_REPO_PHONE)
#define CSR_BT_PAC_SRC_SIM              (CsrBtPacSrcType)(CSR_BT_PB_REPO_SIM)

typedef CsrUint8                        CsrBtPacFormatType;
#define CSR_BT_PAC_FORMAT_VCARD2_1      (CsrBtPacFormatType)(CSR_BT_PB_FORMAT_VCARD2_1)
#define CSR_BT_PAC_FORMAT_VCARD3_0      (CsrBtPacFormatType)(CSR_BT_PB_FORMAT_VCARD3_0)

typedef CsrUint8                        CsrBtPacOrderType;
#define CSR_BT_PAC_ORDER_INDEXED        (CsrBtPacOrderType)(CSR_BT_PB_ORDER_INDEXED)
#define CSR_BT_PAC_ORDER_ALPHABETICAL   (CsrBtPacOrderType)(CSR_BT_PB_ORDER_ALPHABETICAL)
#define CSR_BT_PAC_ORDER_PHONETICAL     (CsrBtPacOrderType)(CSR_BT_PB_ORDER_PHONETICAL)

typedef CsrUint8                        CsrBtPacSearchAtt;
#define CSR_BT_PAC_SEARCH_ATT_NAME      (CsrBtPacSearchAtt)(CSR_BT_PB_SEARCH_ATT_NAME)
#define CSR_BT_PAC_SEARCH_ATT_NUMBER    (CsrBtPacSearchAtt)(CSR_BT_PB_SEARCH_ATT_NUMBER)
#define CSR_BT_PAC_SEARCH_ATT_SOUND     (CsrBtPacSearchAtt)(CSR_BT_PB_SEARCH_ATT_SOUND)


/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_PAC_PRIM_DOWNSTREAM_LOWEST                  (0x0000)

#define CSR_BT_PAC_CONNECT_REQ             ((CsrBtPacPrim) (0x0000 + CSR_BT_PAC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAC_AUTHENTICATE_RES        ((CsrBtPacPrim) (0x0001 + CSR_BT_PAC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAC_PULL_PB_REQ             ((CsrBtPacPrim) (0x0002 + CSR_BT_PAC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAC_PULL_PB_RES             ((CsrBtPacPrim) (0x0003 + CSR_BT_PAC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAC_SET_FOLDER_REQ          ((CsrBtPacPrim) (0x0004 + CSR_BT_PAC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAC_SET_BACK_FOLDER_REQ     ((CsrBtPacPrim) (0x0005 + CSR_BT_PAC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAC_SET_ROOT_FOLDER_REQ     ((CsrBtPacPrim) (0x0006 + CSR_BT_PAC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAC_PULL_VCARD_LIST_REQ     ((CsrBtPacPrim) (0x0007 + CSR_BT_PAC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAC_PULL_VCARD_LIST_RES     ((CsrBtPacPrim) (0x0008 + CSR_BT_PAC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAC_PULL_VCARD_ENTRY_REQ    ((CsrBtPacPrim) (0x0009 + CSR_BT_PAC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAC_PULL_VCARD_ENTRY_RES    ((CsrBtPacPrim) (0x000A + CSR_BT_PAC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAC_ABORT_REQ               ((CsrBtPacPrim) (0x000B + CSR_BT_PAC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAC_DISCONNECT_REQ          ((CsrBtPacPrim) (0x000C + CSR_BT_PAC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAC_CANCEL_CONNECT_REQ      ((CsrBtPacPrim) (0x000D + CSR_BT_PAC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAC_SECURITY_OUT_REQ        ((CsrBtPacPrim) (0x000E + CSR_BT_PAC_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_PAC_PRIM_DOWNSTREAM_HIGHEST (0x000E + CSR_BT_PAC_PRIM_DOWNSTREAM_LOWEST)

/*  Not part of interface, used internally only */
#define CSR_BT_PAC_HOUSE_CLEANING          ((CsrBtPacPrim) (0x007F + CSR_BT_PAC_PRIM_DOWNSTREAM_LOWEST))

/*******************************************************************************/

#define CSR_BT_PAC_PRIM_UPSTREAM_LOWEST                    (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_PAC_CONNECT_CFM             ((CsrBtPacPrim) (0x0000 + CSR_BT_PAC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAC_AUTHENTICATE_IND        ((CsrBtPacPrim) (0x0001 + CSR_BT_PAC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAC_PULL_PB_CFM             ((CsrBtPacPrim) (0x0002 + CSR_BT_PAC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAC_PULL_PB_IND             ((CsrBtPacPrim) (0x0003 + CSR_BT_PAC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAC_SET_FOLDER_CFM          ((CsrBtPacPrim) (0x0004 + CSR_BT_PAC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAC_SET_BACK_FOLDER_CFM     ((CsrBtPacPrim) (0x0005 + CSR_BT_PAC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAC_SET_ROOT_FOLDER_CFM     ((CsrBtPacPrim) (0x0006 + CSR_BT_PAC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAC_PULL_VCARD_LIST_CFM     ((CsrBtPacPrim) (0x0007 + CSR_BT_PAC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAC_PULL_VCARD_LIST_IND     ((CsrBtPacPrim) (0x0008 + CSR_BT_PAC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAC_PULL_VCARD_ENTRY_CFM    ((CsrBtPacPrim) (0x0009 + CSR_BT_PAC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAC_PULL_VCARD_ENTRY_IND    ((CsrBtPacPrim) (0x000A + CSR_BT_PAC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAC_ABORT_CFM               ((CsrBtPacPrim) (0x000B + CSR_BT_PAC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAC_DISCONNECT_IND          ((CsrBtPacPrim) (0x000C + CSR_BT_PAC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAC_SECURITY_OUT_CFM        ((CsrBtPacPrim) (0x000D + CSR_BT_PAC_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_PAC_PRIM_UPSTREAM_HIGHEST                   (0x000D + CSR_BT_PAC_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_PAC_PRIM_DOWNSTREAM_COUNT   (CSR_BT_PAC_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_PAC_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_PAC_PRIM_UPSTREAM_COUNT     (CSR_BT_PAC_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_PAC_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/


/************************************ Downstream primitive structures ****************************/
typedef struct
{
    CsrBtPacPrim           type;
    CsrSchedQid            appHandle;
    CsrUint16              maxPacketSize;
    CsrBtDeviceAddr        destination;
    CsrUint16              windowSize;
} CsrBtPacConnectReq;

typedef struct
{
    CsrBtPacPrim            type;
    CsrUint8               *password;
    CsrUint16               passwordLength;
    CsrCharString          *userId;         /*Max length is BT_OBEX_MAX_AUTH_USERID_LENGTH */
} CsrBtPacAuthenticateRes;

typedef struct
{
    CsrBtPacPrim            type;
    CsrUcs2String          *ucs2name;       /* Unicode name, zero-terminated */
    CsrBtPacSrcType         src;
    CsrUint8                filter[8];
    CsrBtPacFormatType      format;
    CsrUint16               maxListCnt;
    CsrUint16               listStartOffset;
    CsrBool                 srmpOn;
    CsrUint8                resetNewMissedCalls;
    CsrUint8                vCardSelector[8];
    CsrUint8                vCardSelectorOperator;
} CsrBtPacPullPbReq;

typedef struct
{
    CsrBtPacPrim            type;
    CsrBool                 srmpOn;
} CsrBtPacPullPbRes;


typedef struct
{
    CsrBtPacPrim            type;
    CsrUcs2String          *ucs2name;       /* Unicode name, zero-terminated */
} CsrBtPacSetFolderReq;

typedef struct
{
    CsrBtPacPrim            type;
} CsrBtPacSetBackFolderReq;

typedef struct
{
    CsrBtPacPrim            type;
} CsrBtPacSetRootFolderReq;

typedef struct
{
    CsrBtPacPrim            type;
    CsrUcs2String          *ucs2name;       /* Unicode name, zero-terminated */
    CsrBtPacOrderType       order;
    CsrUint8               *searchVal;
    CsrUint16               searchValLen;
    CsrBtPacSearchAtt       searchAtt;
    CsrUint16               maxListCnt;
    CsrUint16               listStartOffset;
    CsrBool                 srmpOn;
    CsrUint8                resetNewMissedCalls;
    CsrUint8                vCardSelector[8];
    CsrUint8                vCardSelectorOperator;
} CsrBtPacPullVcardListReq;

typedef struct
{
    CsrBtPacPrim            type;
    CsrBool                 srmpOn;
} CsrBtPacPullVcardListRes;

typedef struct
{
    CsrBtPacPrim            type;
    CsrUcs2String          *ucs2name;       /* Unicode name, zero-terminated */
    CsrUint8                filter[8];
    CsrBtPacFormatType      format;
    CsrBool                 srmpOn;
} CsrBtPacPullVcardEntryReq;

typedef struct
{
    CsrBtPacPrim            type;
    CsrBool                 srmpOn;
} CsrBtPacPullVcardEntryRes;

typedef struct
{
    CsrBtPacPrim            type;
} CsrBtPacAbortReq;

typedef struct
{
    CsrBtPacPrim            type;
    CsrBool                 normalDisconnect;
} CsrBtPacDisconnectReq;

typedef struct
{
    CsrBtPacPrim            type;
} CsrBtPacCancelConnectReq;

typedef struct
{
    CsrBtPacPrim            type;
    CsrSchedQid             appHandle;
    CsrUint16               secLevel;
} CsrBtPacSecurityOutReq;


/********************************** Upstream primitive structures ********************************/
/* Connect Confirm
 * PBAP client responds with CSR_BT_PAC_CONNECT_CFM on finishing connection procedure */
typedef struct
{
    CsrBtPacPrim            type;                   /* Always set to CSR_BT_PAC_CONNECT_CFM */
    CsrBtResultCode         resultCode;             /* This denotes outcome of the operation.
                                                    Possible values depend on the resultSupplier.
                                                    For example, if
                                                        resultSupplier == CSR_BT_SUPPLIER_CM
                                                    then the possible result codes can be found in
                                                    csr_bt_cm_prim.h.
                                                    All values that are currently not specified in
                                                    the respective prim.h file are regarded as
                                                    reserved and the application should consider
                                                    them as errors.
                                                    Possible result codes from PAC are defined in
                                                    csr_bt_obex.h. */
    CsrBtSupplier           resultSupplier;         /* This parameter specifies the supplier of the result
                                                    given in resultCode.
                                                    Possible values can be found in csr_bt_result.h */
    CsrUint16               obexPeerMaxPacketSize;  /* Indicates the maximum size OBEX packet that is
                                                    allowed to send to the server */
    CsrBtPacSrcType         supportedRepositories;  /* Indicates the supported repositories in the server.
                                                    The possible values are combination of:
                                                        CSR_BT_PB_REPO_PHONE (CSR_BT_PAC_SRC_PHONE) - Phone
                                                        CSR_BT_PB_REPO_SIM (CSR_BT_PAC_SRC_SIM) - Sim
                                                        CSR_BT_PB_REPO_SPD - Speeddial key supported
                                                        CSR_BT_PB_REPO_FAV - Favourite contacts */
    CsrBtConnId             btConnId;               /* Global Bluetooth connection ID */
    CsrUint32               supportedFeatures;      /* (New) Features supported by remote PBAP server. Possible values
                                                    are combinations of following values:
                                                        CSR_BT_PB_FEATURE_DOWNLOAD
                                                        CSR_BT_PB_FEATURE_BROWSING
                                                        CSR_BT_PB_FEATURE_DATABASE_ID
                                                        CSR_BT_PB_FEATURE_FOLDER_VER
                                                        CSR_BT_PB_FEATURE_VCARD_SEL
                                                        CSR_BT_PB_FEATURE_MISSED_CALL
                                                        CSR_BT_PB_FEATURE_UCI
                                                        CSR_BT_PB_FEATURE_UID
                                                        CSR_BT_PB_FEATURE_CONTACT_REF
                                                        CSR_BT_PB_FEATURE_CONTACT_IMG */
} CsrBtPacConnectCfm;

/* Authentication Indication
 * This message indicate that remote PBAP server wants OBEX authentication. Application is expected to
 * respond with CSR_BT_PAC_AUTHENTICATE_RES */
typedef struct
{
    CsrBtPacPrim            type;           /* Always set to CSR_BT_PAC_AUTHENTICATE_IND */
    CsrBtDeviceAddr         deviceAddr;     /* The Bluetooth address of the device that has initiated the
                                            OBEX authentication procedure */
    CsrUint8                options;        /* Challenge information of type CsrUint8.
                                                * Bit 0 controls the responding of a valid user Id.
                                                If bit 0 is set it means that the application must response
                                                with a user Id in a CSR_BT_PAC_AUTHENTICATE_RES message. If
                                                bit 0 is not set the application can just set the userId to NULL.
                                                * Bit 1 indicates the access mode being offered by the sender
                                                If bit 1 is set the access mode is read only. If bit 1 is not
                                                set the sender gives full access, e.g. both read and write.
                                                * Bit 2 - 7 is reserved */
    CsrUint16               realmLength;    /* Number of bytes in realm of type CsrUint16 */
    CsrUint8               *realm;          /* A displayable string indicating for the user which userid
                                            and/or password to use. The first byte of the string is the
                                            character set of the string. The table below shows the different
                                            values for character set. Note that this pointer must be pfree by
                                            the application, and that this pointer can be NULL because the
                                            realm field is optional to set by the peer device.*/
} CsrBtPacAuthenticateInd;

/* Pull Phonebook Confirm
 * PBAP client notifies application of phone book pull operation result using this message. Headers of pull
 * phonebook response are delivered in this message. */
typedef struct
{
    CsrBtPacPrim            type;           /* Always set to CSR_BT_PAC_PULL_PB_CFM */
    CsrBtObexResponseCode   responseCode;   /* The valid response codes are defined (in csr_bt_obex.h).
                                            For success the code is CSR_BT_OBEX_SUCCESS_RESPONSE_CODE.
                                            Any other response code indicates a failure in the operation. */
    CsrUint16               pbSize;         /* The numbers of entries in the phone book of interest.
                                            This parameter is valid only when maxListCnt is zero in the
                                            Pull Phonebook request. In any other case application should
                                            ignore this parameter.*/
    CsrUint8                newMissedCall;  /* This parameter is only used when the phone book of interest
                                            is mch (mch.vcf) or cch (cch.vcf). It indicates the numbers of
                                            missed calls that had not been checked yet since the PBAP
                                            session was started. */
    CsrBtPbVersionInfo      versionInfo;    /* (New) This parameter consists of folder version and database
                                            identifier */
} CsrBtPacPullPbCfm;

/* Pull Phonebook Indication
 * The PAC passes phonebook contents, if received from PBAP server, to application through Pull Phonebook Indication
 * (CSR_BT_PAC_PULL_PB_IND). The Application acknowledges each Pull Phonebook indication through a Pull Phonebook
 * Response (CSR_BT_PAC_PULL_PB_RES).
 * On receiving Pull Phonebook response, the PAC responds with further Pull Phonebook indications to application
 * if more of phonebook content is available. */
typedef struct
{
    CsrBtPacPrim            type;           /* Always set to CSR_BT_PAC_PULL_PB_IND */
    CsrUint16               bodyLength;     /* The length of the body (object). */
    CsrUint16               bodyOffset;     /* Payload relative offset for the object itself. */
    CsrUint8               *payload;        /* OBEX payload data. Offsets are relative to this
                                            pointer */
    CsrUint16               payloadLength;  /* Number of bytes in payload. */
} CsrBtPacPullPbInd;

/* Set Folder Confirm
 * PBAP client informs application of folder change using this message. */
typedef struct
{
    CsrBtPacPrim            type;           /* Always set to CSR_BT_PAC_SET_FOLDER_CFM */
    CsrBtObexResponseCode   responseCode;   /* The valid response codes are defined (in csr_bt_obex.h).
                                            For success in the confirm signal the code is
                                            CSR_BT_OBEX_SUCCESS_RESPONSE_CODE. Any other response code
                                            indicates a failure in the confirm signal. */
} CsrBtPacSetFolderCfm;

/* Set Back Folder Confirm
 * Confirmation for CSR_BT_PAC_SET_BACK_FOLDER_REQ primtive */
typedef struct
{
    CsrBtPacPrim            type;           /* Always set to CSR_BT_PAC_SET_BACK_FOLDER_CFM */
    CsrBtObexResponseCode   responseCode;   /* The valid response codes are defined (in csr_bt_obex.h).
                                            For success in the confirm signal the code is
                                            CSR_BT_OBEX_SUCCESS_RESPONSE_CODE. Any other response code
                                            indicates a failure in the confirm signal. */
} CsrBtPacSetBackFolderCfm;

/* Set Root Folder Confirm
 * Confirmation for CSR_BT_PAC_SET_ROOT_FOLDER_REQ primitive */
typedef struct
{
    CsrBtPacPrim            type;           /* Always set to CSR_BT_PAC_SET_ROOT_FOLDER_CFM */
    CsrBtObexResponseCode   responseCode;   /* The valid response codes are defined (in csr_bt_obex.h).
                                            For success in the confirm signal the code is
                                            CSR_BT_OBEX_SUCCESS_RESPONSE_CODE. Any other response code
                                            indicates a failure in the confirm signal. */
} CsrBtPacSetRootFolderCfm;

/* Phonebook Listing Confirm
 * PBAP client sends this message on receiving response for Pull listing from PBAP Server */
typedef struct
{
    CsrBtPacPrim            type;           /* Always set to CSR_BT_PAC_PULL_VCARD_LIST_CFM */
    CsrBtObexResponseCode   responseCode;   /* The valid response codes are defined (in csr_bt_obex.h).
                                            For success in the confirm signal the code is
                                            CSR_BT_OBEX_SUCCESS_RESPONSE_CODE. Any other response code
                                            indicates a failure in the confirm signal. */
    CsrUint16               pbSize;         /* The numbers of entries in the phone book of interest see
                                            maxListCnt. This parameter is only used when maxListCnt is
                                            zero in the request, see maxListCnt.*/
    CsrUint8                newMissedCall;  /* This parameter is used when the phone book of interest is mch.
                                            It indicates the numbers of missed calls that had not been
                                            checked yet since the PBAP session was started.*/
    CsrBtPbVersionInfo     versionInfo;     /* (New) This parameter consists of folder version and database
                                            identifier */
} CsrBtPacPullVcardListCfm;

/* Phonebook Listing Indication
 * Body of pull listing response is delivered in this message. */
typedef struct
{
    CsrBtPacPrim            type;           /* Always set to CSR_BT_PAC_PULL_VCARD_LIST_IND */
    CsrUint16               bodyLength;     /* The length of the body (object). */
    CsrUint16               bodyOffset;     /* Offset relative to payload of the object itself. */
    CsrUint8               *payload;        /* OBEX payload data. Offsets are relative to this pointer. */
    CsrUint16               payloadLength;  /* Number of bytes in payload. */
} CsrBtPacPullVcardListInd;

/* Pull vCard Confirm
 * This message is sent on completion of Pull vcard operation. */
typedef struct
{
    CsrBtPacPrim            type;           /* Always set to CSR_BT_PAC_PULL_VCARD_CFM */
    CsrBtObexResponseCode   responseCode;   /* The valid response codes are defined (in csr_bt_obex.h).
                                            For success in the confirm signal the code is
                                            CSR_BT_OBEX_SUCCESS_RESPONSE_CODE. Any other response code
                                            indicates a failure in the confirm signal. */
    CsrUint8                databaseId[16]; /* (New) Unique database identifier of the PBAP server. */
} CsrBtPacPullVcardEntryCfm;

/* Pull vCard Indication
 * Content of Pull vcard response is delivered in this message. */
typedef struct
{
    CsrBtPacPrim            type;           /* Always set to CSR_BT_PAC_PULL_VCARD_IND */
    CsrUint16               bodyLength;     /* The length of the body (object). */
    CsrUint16               bodyOffset;     /* Offset relative to payload of the object itself. */
    CsrUint8               *payload;        /* OBEX payload data. Offsets are relative to this pointer.*/
    CsrUint16               payloadLength;  /* Number of bytes in payload. */
} CsrBtPacPullVcardEntryInd;

/* Abort Confirm
 * This message confirms the completion of Abort operation */
typedef struct
{
    CsrBtPacPrim            type;   /* Always set to CSR_BT_PAC_ABORT_CFM */
} CsrBtPacAbortCfm;

/* Disconnect Indication
 * This message indicates disconnection from PBAP server */
typedef struct
{
    CsrBtPacPrim            type;           /* Always set to CSR_BT_PAC_DISCONNECT_IND */
    CsrBtReasonCode         reasonCode;     /* The reason code of the operation. Possible values depend on
                                            the value of reasonSupplier.
                                            If e.g. the
                                                reasonSupplier == CSR_BT_SUPPLIER_CM
                                            then the possible reason codes can be found in csr_bt_cm_prim.h.
                                            If the
                                                reasonSupplier == CSR_BT_SUPPLIER_OBEX
                                            then the possible result codes can be found in csr_bt_obex.h.
                                            All values which are currently not specified in the respective
                                            prim.h files or csr_bt_obex.h are regarded as reserved and
                                            the application should consider them as errors.*/
    CsrBtSupplier           reasonSupplier; /* This parameter specifies the supplier of the reason given in
                                            reasonCode. Possible values can be found in csr_bt_result.h */
} CsrBtPacDisconnectInd;

/* Security Confirm
 * PBAP client responds with this message to notify outcome of CSR_BT_PAC_SECURITY_OUT_REQ primitive */
typedef struct
{
    CsrBtPacPrim            type;           /* Always set to CSR_BT_PAC_SECURITY_OUT_CFM */
    CsrBtResultCode         resultCode;     /* The valid response codes are defined (in csr_bt_obex.h).
                                            For success in the confirm signal the code is
                                            CSR_BT_OBEX_SUCCESS_RESPONSE_CODE. Any other response code
                                            indicates a failure in the confirm signal. */
    CsrBtSupplier           resultSupplier; /* This parameter specifies the supplier of the reason given in
                                            reasonCode. Possible values can be found in csr_bt_result.h */
} CsrBtPacSecurityOutCfm;


/******************************* Local primitive structures **********************************/
typedef struct
{
    CsrBtPacPrim            type;
} PacLpSupervisionTimer;

typedef struct
{
    CsrBtPacPrim            type;
} CsrBtPacHouseCleaning;


#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_PAC_PRIM_H__ */

