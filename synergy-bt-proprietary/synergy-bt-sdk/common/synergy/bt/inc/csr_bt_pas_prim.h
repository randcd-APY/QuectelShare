#ifndef CSR_BT_PAS_PRIM_H__
#define CSR_BT_PAS_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2010-2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_result.h"
#include "csr_bt_profiles.h"
#include "csr_bt_obex.h"
#include "csr_bt_pb_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtPasPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim    CsrBtPasPrim;

#define CSR_BT_PAS_PROFILE_VERSION          (0x0102)

#define CSR_BT_PAS_SUPPORT_FEATURE_DOWNLOAD (CSR_BT_PB_FEATURE_DOWNLOAD)
#define CSR_BT_PAS_SUPPORT_FEATURE_BROWSING (CSR_BT_PB_FEATURE_BROWSING)

#define CSR_BT_PAS_SUPPORT_REPOSITORY_PB    (CSR_BT_PB_REPO_PHONE)
#define CSR_BT_PAS_SUPPORT_REPOSITORY_SIM   (CSR_BT_PB_REPO_SIM)

typedef CsrUint8 CsrBtPasOrder;
#define CSR_BT_PAS_ORDER_INDEXED            (CsrBtPasOrder)(CSR_BT_PB_ORDER_INDEXED)
#define CSR_BT_PAS_ORDER_ALPHABETICAL       (CsrBtPasOrder)(CSR_BT_PB_ORDER_ALPHABETICAL)
#define CSR_BT_PAS_ORDER_PHONETICAL         (CsrBtPasOrder)(CSR_BT_PB_ORDER_PHONETICAL)

typedef CsrUint8 CsrBtPasSearchAtt;
#define CSR_BT_PAS_SEARCH_ATT_NAME          (CsrBtPasSearchAtt)(CSR_BT_PB_SEARCH_ATT_NAME)
#define CSR_BT_PAS_SEARCH_ATT_NUMBER        (CsrBtPasSearchAtt)(CSR_BT_PB_SEARCH_ATT_NUMBER)
#define CSR_BT_PAS_SEARCH_ATT_SOUND         (CsrBtPasSearchAtt)(CSR_BT_PB_SEARCH_ATT_SOUND)

typedef CsrUint8 CsrBtPasFormat;
#define CSR_BT_PAS_FORMAT_VCARD2_1          (CsrBtPasFormat)(CSR_BT_PB_FORMAT_VCARD2_1)
#define CSR_BT_PAS_FORMAT_VCARD3_0          (CsrBtPasFormat)(CSR_BT_PB_FORMAT_VCARD3_0)

typedef CsrUint16 CsrBtPasSrc;
#define CSR_BT_PAS_SRC_PHONE                (CsrBtPasSrc)(0)
#define CSR_BT_PAS_SRC_SIM                  (CsrBtPasSrc)(1)


/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_PAS_PRIM_DOWNSTREAM_LOWEST                         (0x0000)

#define CSR_BT_PAS_ACTIVATE_REQ                    ((CsrBtPasPrim) (0x0000 + CSR_BT_PAS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAS_DEACTIVATE_REQ                  ((CsrBtPasPrim) (0x0001 + CSR_BT_PAS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAS_CONNECT_RES                     ((CsrBtPasPrim) (0x0002 + CSR_BT_PAS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAS_AUTHENTICATE_REQ                ((CsrBtPasPrim) (0x0003 + CSR_BT_PAS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAS_AUTHENTICATE_RES                ((CsrBtPasPrim) (0x0004 + CSR_BT_PAS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAS_PULL_PB_RES                     ((CsrBtPasPrim) (0x0005 + CSR_BT_PAS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAS_PULL_PB_SIZE_RES                ((CsrBtPasPrim) (0x0006 + CSR_BT_PAS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAS_PULL_MCH_RES                    ((CsrBtPasPrim) (0x0007 + CSR_BT_PAS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAS_PULL_MCH_SIZE_RES               ((CsrBtPasPrim) (0x0008 + CSR_BT_PAS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAS_SET_FOLDER_RES                  ((CsrBtPasPrim) (0x0009 + CSR_BT_PAS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAS_PULL_VCARD_LIST_RES             ((CsrBtPasPrim) (0x000A + CSR_BT_PAS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAS_PULL_VCARD_LIST_SIZE_RES        ((CsrBtPasPrim) (0x000B + CSR_BT_PAS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAS_PULL_VCARD_MCH_LIST_RES         ((CsrBtPasPrim) (0x000C + CSR_BT_PAS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAS_PULL_VCARD_MCH_LIST_SIZE_RES    ((CsrBtPasPrim) (0x000D + CSR_BT_PAS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAS_PULL_VCARD_ENTRY_RES            ((CsrBtPasPrim) (0x000E + CSR_BT_PAS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAS_NEXT_RES                        ((CsrBtPasPrim) (0x000F + CSR_BT_PAS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAS_SECURITY_IN_REQ                 ((CsrBtPasPrim) (0x0010 + CSR_BT_PAS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAS_PULL_CCH_RES                    ((CsrBtPasPrim) (0x0011 + CSR_BT_PAS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAS_PULL_CCH_SIZE_RES               ((CsrBtPasPrim) (0x0012 + CSR_BT_PAS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAS_PULL_VCARD_CCH_LIST_RES         ((CsrBtPasPrim) (0x0013 + CSR_BT_PAS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PAS_PULL_VCARD_CCH_LIST_SIZE_RES    ((CsrBtPasPrim) (0x0014 + CSR_BT_PAS_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_PAS_PRIM_DOWNSTREAM_HIGHEST                         (0x0014 + CSR_BT_PAS_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_PAS_PRIM_UPSTREAM_LOWEST                            (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_PAS_DEACTIVATE_CFM                  ((CsrBtPasPrim) (0x0000 + CSR_BT_PAS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAS_CONNECT_IND                     ((CsrBtPasPrim) (0x0001 + CSR_BT_PAS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAS_AUTHENTICATE_CFM                ((CsrBtPasPrim) (0x0002 + CSR_BT_PAS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAS_AUTHENTICATE_IND                ((CsrBtPasPrim) (0x0003 + CSR_BT_PAS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAS_PULL_PB_IND                     ((CsrBtPasPrim) (0x0004 + CSR_BT_PAS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAS_PULL_PB_SIZE_IND                ((CsrBtPasPrim) (0x0005 + CSR_BT_PAS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAS_PULL_MCH_IND                    ((CsrBtPasPrim) (0x0006 + CSR_BT_PAS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAS_PULL_MCH_SIZE_IND               ((CsrBtPasPrim) (0x0007 + CSR_BT_PAS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAS_SET_FOLDER_IND                  ((CsrBtPasPrim) (0x0008 + CSR_BT_PAS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAS_SET_BACK_FOLDER_IND             ((CsrBtPasPrim) (0x0009 + CSR_BT_PAS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAS_SET_ROOT_FOLDER_IND             ((CsrBtPasPrim) (0x000A + CSR_BT_PAS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAS_PULL_VCARD_LIST_IND             ((CsrBtPasPrim) (0x000B + CSR_BT_PAS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAS_PULL_VCARD_LIST_SIZE_IND        ((CsrBtPasPrim) (0x000C + CSR_BT_PAS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAS_PULL_VCARD_MCH_LIST_IND         ((CsrBtPasPrim) (0x000D + CSR_BT_PAS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAS_PULL_VCARD_MCH_LIST_SIZE_IND    ((CsrBtPasPrim) (0x000E + CSR_BT_PAS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAS_PULL_VCARD_ENTRY_IND            ((CsrBtPasPrim) (0x000F + CSR_BT_PAS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAS_NEXT_IND                        ((CsrBtPasPrim) (0x0010 + CSR_BT_PAS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAS_ABORT_IND                       ((CsrBtPasPrim) (0x0011 + CSR_BT_PAS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAS_DISCONNECT_IND                  ((CsrBtPasPrim) (0x0012 + CSR_BT_PAS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAS_SECURITY_IN_CFM                 ((CsrBtPasPrim) (0x0013 + CSR_BT_PAS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PAS_RESET_MISSED_CALLS_IND          ((CsrBtPasPrim) (0x0014 + CSR_BT_PAS_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_PAS_PRIM_UPSTREAM_HIGHEST           (0x0014 + CSR_BT_PAS_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_PAS_PRIM_DOWNSTREAM_COUNT           (CSR_BT_PAS_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_PAS_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_PAS_PRIM_UPSTREAM_COUNT             (CSR_BT_PAS_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_PAS_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

/* == SIGNAL TYPES =========================================================== */
/* downstream */
typedef struct
{
    CsrBtPasPrim             type;
    CsrSchedQid              appHandle;
    CsrUint32                feature;
    CsrUint8                 repository;
    CsrUint16                obexMaxPacketSize;
    CsrUint16                windowSize;
    CsrBool                  srmEnable;
} CsrBtPasActivateReq;

typedef struct
{
    CsrBtPasPrim             type;
} CsrBtPasDeactivateReq;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
} CsrBtPasConnectRes;

/* upstream */
typedef struct
{
    CsrBtPasPrim             type;
} CsrBtPasDeactivateCfm;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
    CsrUint16                obexPeerMaxPacketSize;
    CsrBtDeviceAddr          deviceAddr;
    CsrUint32                length;
    CsrUint32                count;
    CsrBtConnId              btConnId;               /* Global Bluetooth connection ID */
    CsrUint32                features;
} CsrBtPasConnectInd;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint16                realmLength;
    CsrUint8                *realm;
    CsrUint16                passwordLength;
    CsrUint8                *password;
    CsrCharString           *userId; /*Max length is BT_OBEX_MAX_AUTH_USERID_LENGTH */
} CsrBtPasAuthenticateReq;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint8                *password;
    CsrUint16                passwordLength;
    CsrCharString           *userId;/*Max length is BT_OBEX_MAX_AUTH_USERID_LENGTH */
} CsrBtPasAuthenticateRes;

typedef struct
{
    CsrBtPasPrim             type;
} CsrBtPasAuthenticateCfm;

typedef struct
{
    CsrBtPasPrim             type;
    CsrBtDeviceAddr          deviceAddr;
    CsrUint8                 options;
    CsrUint16                realmLength;
    CsrUint8                *realm;
} CsrBtPasAuthenticateInd;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
    CsrBool                  srmpOn;
    CsrBtPbVersionInfo       versionInfo;
} CsrBtPasPullPbRes;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
    CsrUint16                ucs2nameOffset;      /* Payload relative offset, zero-terminated */
    CsrUint8                 filter[8];
    CsrBtPasSrc              src;
    CsrBtPasFormat           format;
    CsrUint16                maxListCnt;
    CsrUint16                listStartOffset;
    CsrUint8                *payload;
    CsrUint16                payloadLength;
    CsrUint8                 vCardSelector[8];
    CsrUint8                 vCardSelectorOperator;
} CsrBtPasPullPbInd;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
    CsrUint16                pbSize;
    CsrBtObexResponseCode    responseCode;
    CsrBool                  srmpOn;
    CsrBtPbVersionInfo       versionInfo;
} CsrBtPasPullPbSizeRes;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
    CsrUint16                ucs2nameOffset;      /* Payload relative offset, zero-terminated */
    CsrBtPasSrc              src;
    CsrUint8                *payload;
    CsrUint16                payloadLength;
    CsrUint8                 vCardSelector[8];
    CsrUint8                 vCardSelectorOperator;
} CsrBtPasPullPbSizeInd;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
    CsrUint8                 newMissedCall;
    CsrBtObexResponseCode    responseCode;
    CsrBool                  srmpOn;
    CsrBtPbVersionInfo       versionInfo;
} CsrBtPasPullMchRes;

typedef  CsrBtPasPullMchRes CsrBtPasPullCchRes;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
    CsrUint8                 filter[8];
    CsrBtPasSrc              src;
    CsrBtPasFormat           format;
    CsrUint16                maxListCnt;
    CsrUint16                listStartOffset;
    CsrUint8                 vCardSelector[8];
    CsrUint8                 vCardSelectorOperator;
} CsrBtPasPullMchInd;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
    CsrUint16                pbSize;
    CsrUint8                 newMissedCall;
    CsrBtObexResponseCode    responseCode;
    CsrBool                  srmpOn;
    CsrBtPbVersionInfo       versionInfo;
} CsrBtPasPullMchSizeRes;

typedef CsrBtPasPullMchSizeRes CsrBtPasPullCchSizeRes;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
    CsrBtPasSrc              src;
    CsrUint8                 vCardSelector[8];
    CsrUint8                 vCardSelectorOperator;
} CsrBtPasPullMchSizeInd;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
} CsrBtPasSetFolderRes;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
    CsrUint16                ucs2nameOffset;      /* Payload relative offset, zero-terminated */
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtPasSetFolderInd;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
} CsrBtPasSetBackFolderInd;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
} CsrBtPasSetRootFolderInd;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
    CsrBool                  srmpOn;
    CsrBtPbVersionInfo       versionInfo;
} CsrBtPasPullVcardListRes;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
    CsrUint16                ucs2nameOffset;      /* Payload relative offset, zero-terminated */
    CsrBtPasOrder            order;
    CsrUint16                uft8SearchValOffset; /* Payload relative offset */
    CsrUint16                searchValLength;
    CsrBtPasSearchAtt        searchAtt;
    CsrUint16                maxListCnt;
    CsrUint16                listStartOffset;
    CsrUint8                *payload;
    CsrUint16                payloadLength;
    CsrUint8                 vCardSelector[8];
    CsrUint8                 vCardSelectorOperator;
} CsrBtPasPullVcardListInd;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
    CsrUint16                pbSize;
    CsrBtObexResponseCode    responseCode;
    CsrBool                  srmpOn;
    CsrBtPbVersionInfo       versionInfo;
} CsrBtPasPullVcardListSizeRes;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
    CsrUint16                ucs2nameOffset;      /* Payload relative offset, zero-terminated */
    CsrUint8                *payload;
    CsrUint16                payloadLength;
    CsrUint8                 vCardSelector[8];
    CsrUint8                 vCardSelectorOperator;
} CsrBtPasPullVcardListSizeInd;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
    CsrUint8                 newMissedCall;
    CsrBtObexResponseCode    responseCode;
    CsrBool                  srmpOn;
    CsrBtPbVersionInfo       versionInfo;
} CsrBtPasPullVcardMchListRes;

typedef CsrBtPasPullVcardMchListRes CsrBtPasPullVcardCchListRes;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
    CsrBtPasOrder            order;
    CsrUint16                uft8SearchValOffset; /* Payload relative offset */
    CsrUint16                searchValLength;
    CsrBtPasSearchAtt        searchAtt;
    CsrUint16                maxListCnt;
    CsrUint16                listStartOffset;
    CsrUint8                *payload;
    CsrUint16                payloadLength;
    CsrUint8                 vCardSelector[8];
    CsrUint8                 vCardSelectorOperator;
} CsrBtPasPullVcardMchListInd;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
    CsrUint16                pbSize;
    CsrUint8                 newMissedCall;
    CsrBtObexResponseCode    responseCode;
    CsrBool                  srmpOn;
    CsrBtPbVersionInfo       versionInfo;
} CsrBtPasPullVcardMchListSizeRes;

typedef CsrBtPasPullVcardMchListSizeRes CsrBtPasPullVcardCchListSizeRes;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
    CsrUint8                 vCardSelector[8];
    CsrUint8                 vCardSelectorOperator;
} CsrBtPasPullVcardMchListSizeInd;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrBtObexResponseCode    responseCode;
    CsrBool                  srmpOn;
    CsrUint8                 databaseIdentifier[16];
} CsrBtPasPullVcardEntryRes;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
    CsrUint16                ucs2nameOffset;      /* Payload relative offset, zero-terminated */
    CsrUint8                 filter[8];
    CsrBtPasFormat           format;
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtPasPullVcardEntryInd;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrBtObexResponseCode    responseCode;
    CsrUint16                bodyLength;
    CsrUint8                *body;
    CsrBool                  srmpOn;
} CsrBtPasNextRes;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
} CsrBtPasNextInd;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
    CsrUint16                descriptionOffset;   /* Payload relative offset */
    CsrUint16                descriptionLength;
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtPasAbortInd;

typedef struct
{
    CsrBtPasPrim             type;
    CsrUint32                connectionId;
    CsrBtDeviceAddr          deviceAddr;
    CsrBtReasonCode          reasonCode;
    CsrBtSupplier            reasonSupplier;
} CsrBtPasDisconnectInd;

typedef struct
{
    CsrBtPasPrim             type;
    CsrSchedQid              appHandle;
    CsrUint16                secLevel;
} CsrBtPasSecurityInReq;

typedef struct
{
    CsrBtPasPrim             type;
    CsrBtResultCode          resultCode;
    CsrBtSupplier            resultSupplier;
} CsrBtPasSecurityInCfm;

typedef struct
{
    CsrBtPasPrim            type;
    CsrBtPasSrc             src;
    CsrUint32               connectionId;
} CsrBtPasResetMissedCallsInd;

#ifdef __cplusplus
}
#endif

#endif

