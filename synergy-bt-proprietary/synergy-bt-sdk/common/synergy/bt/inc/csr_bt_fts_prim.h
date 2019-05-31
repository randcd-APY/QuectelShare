#ifndef CSR_BT_FTS_PRIM_H__
#define CSR_BT_FTS_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2010 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_result.h"
#include "csr_bt_profiles.h"
#include "csr_bt_obex.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtFtsPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim    CsrBtFtsPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_FTS_PRIM_DOWNSTREAM_LOWEST                         (0x0000)

#define CSR_BT_FTS_ACTIVATE_REQ                    ((CsrBtFtsPrim) (0x0000 + CSR_BT_FTS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTS_DEACTIVATE_REQ                  ((CsrBtFtsPrim) (0x0001 + CSR_BT_FTS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTS_CONNECT_RES                     ((CsrBtFtsPrim) (0x0002 + CSR_BT_FTS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTS_AUTHENTICATE_REQ                ((CsrBtFtsPrim) (0x0003 + CSR_BT_FTS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTS_AUTHENTICATE_RES                ((CsrBtFtsPrim) (0x0004 + CSR_BT_FTS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTS_PUT_OBJ_RES                     ((CsrBtFtsPrim) (0x0005 + CSR_BT_FTS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTS_PUT_OBJ_NEXT_RES                ((CsrBtFtsPrim) (0x0006 + CSR_BT_FTS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTS_DEL_OBJ_RES                     ((CsrBtFtsPrim) (0x0007 + CSR_BT_FTS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTS_GET_OBJ_RES                     ((CsrBtFtsPrim) (0x0008 + CSR_BT_FTS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTS_GET_OBJ_NEXT_RES                ((CsrBtFtsPrim) (0x0009 + CSR_BT_FTS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTS_GET_LIST_FOLDER_RES             ((CsrBtFtsPrim) (0x000A + CSR_BT_FTS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTS_GET_LIST_FOLDER_NEXT_RES        ((CsrBtFtsPrim) (0x000B + CSR_BT_FTS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTS_SET_FOLDER_RES                  ((CsrBtFtsPrim) (0x000C + CSR_BT_FTS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTS_SET_BACK_FOLDER_RES             ((CsrBtFtsPrim) (0x000D + CSR_BT_FTS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTS_SET_ROOT_FOLDER_RES             ((CsrBtFtsPrim) (0x000E + CSR_BT_FTS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTS_SET_ADD_FOLDER_RES              ((CsrBtFtsPrim) (0x000F + CSR_BT_FTS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTS_SECURITY_IN_REQ                 ((CsrBtFtsPrim) (0x0010 + CSR_BT_FTS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTS_COPY_OBJ_RES                    ((CsrBtFtsPrim) (0x0011 + CSR_BT_FTS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTS_MOVE_OBJ_RES                    ((CsrBtFtsPrim) (0x0012 + CSR_BT_FTS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTS_SET_OBJ_PERMISSIONS_RES         ((CsrBtFtsPrim) (0x0013 + CSR_BT_FTS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTS_PRIM_DOWNSTREAM_HIGHEST                         (0x0013 + CSR_BT_FTS_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_FTS_PRIM_UPSTREAM_LOWEST                           (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_FTS_DEACTIVATE_CFM                  ((CsrBtFtsPrim) (0x0000 + CSR_BT_FTS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTS_CONNECT_IND                     ((CsrBtFtsPrim) (0x0001 + CSR_BT_FTS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTS_AUTHENTICATE_CFM                ((CsrBtFtsPrim) (0x0002 + CSR_BT_FTS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTS_AUTHENTICATE_IND                ((CsrBtFtsPrim) (0x0003 + CSR_BT_FTS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTS_PUT_OBJ_IND                     ((CsrBtFtsPrim) (0x0004 + CSR_BT_FTS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTS_PUT_OBJ_NEXT_IND                ((CsrBtFtsPrim) (0x0005 + CSR_BT_FTS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTS_DEL_OBJ_IND                     ((CsrBtFtsPrim) (0x0006 + CSR_BT_FTS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTS_GET_OBJ_IND                     ((CsrBtFtsPrim) (0x0007 + CSR_BT_FTS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTS_GET_OBJ_NEXT_IND                ((CsrBtFtsPrim) (0x0008 + CSR_BT_FTS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTS_GET_LIST_FOLDER_IND             ((CsrBtFtsPrim) (0x0009 + CSR_BT_FTS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTS_GET_LIST_FOLDER_NEXT_IND        ((CsrBtFtsPrim) (0x000A + CSR_BT_FTS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTS_SET_FOLDER_IND                  ((CsrBtFtsPrim) (0x000B + CSR_BT_FTS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTS_SET_BACK_FOLDER_IND             ((CsrBtFtsPrim) (0x000C + CSR_BT_FTS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTS_SET_ROOT_FOLDER_IND             ((CsrBtFtsPrim) (0x000D + CSR_BT_FTS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTS_SET_ADD_FOLDER_IND              ((CsrBtFtsPrim) (0x000E + CSR_BT_FTS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTS_ABORT_IND                       ((CsrBtFtsPrim) (0x000F + CSR_BT_FTS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTS_DISCONNECT_IND                  ((CsrBtFtsPrim) (0x0010 + CSR_BT_FTS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTS_SECURITY_IN_CFM                 ((CsrBtFtsPrim) (0x0011 + CSR_BT_FTS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTS_COPY_OBJ_IND                    ((CsrBtFtsPrim) (0x0012 + CSR_BT_FTS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTS_MOVE_OBJ_IND                    ((CsrBtFtsPrim) (0x0013 + CSR_BT_FTS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTS_SET_OBJ_PERMISSIONS_IND         ((CsrBtFtsPrim) (0x0014 + CSR_BT_FTS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTS_PRIM_UPSTREAM_HIGHEST                           (0x0014 + CSR_BT_FTS_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_FTS_PRIM_DOWNSTREAM_COUNT           (CSR_BT_FTS_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_FTS_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_FTS_PRIM_UPSTREAM_COUNT             (CSR_BT_FTS_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_FTS_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

#define FTS_NO_TYPE_SUPPORT         0x0000
#define FTS_PHONEBOOK_SUPPORT       0x0001
#define FTS_CALENDAR_SUPPORT        0x0002
#define FTS_NOTE_SUPPORT            0x0004
#define FTS_MESSAGE_SUPPORT         0x0008
#define FTS_ANY_TYPE_SUPPORT        0xFFFF

#define FTS_NUMBER_OF_INDIVIDUAL_FORMATS 4

typedef struct
{
    CsrBtFtsPrim            type;
    CsrSchedQid             appHandle;
    CsrUint16               obexMaxPacketSize;
    CsrUint16               windowSize;
    CsrBool                 srmEnable;
} CsrBtFtsActivateReq;


typedef struct
{
    CsrBtFtsPrim            type;
} CsrBtFtsDeactivateReq;


typedef struct
{
    CsrBtFtsPrim            type;
} CsrBtFtsDeactivateCfm;


typedef struct
{
    CsrBtFtsPrim            type;
    CsrUint32               connectionId;
    CsrUint16               obexPeerMaxPacketSize;
    CsrBtDeviceAddr         deviceAddr;
    CsrUint32               length;
    CsrUint32               count;
    CsrBtConnId             btConnId;               /* Global Bluetooth connection ID */
} CsrBtFtsConnectInd;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
} CsrBtFtsConnectRes;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint16                realmLength;
    CsrUint8                *realm;
    CsrUint16                passwordLength;
    CsrUint8                *password;
    CsrCharString           *userId; /* max length is BT_OBEX_MAX_AUTH_USERID_LENGTH */
} CsrBtFtsAuthenticateReq;

typedef struct
{
    CsrBtFtsPrim             type;
} CsrBtFtsAuthenticateCfm;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrBtDeviceAddr          deviceAddr;
    CsrUint8                 options;
    CsrUint16                realmLength;
    CsrUint8                *realm;
} CsrBtFtsAuthenticateInd;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint8                *password;
    CsrUint16                passwordLength;
    CsrCharString           *userId; /* max length is BT_OBEX_MAX_AUTH_USERID_LENGTH */
} CsrBtFtsAuthenticateRes;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrUint32                lengthOfObject;
    CsrUint16                ucs2nameOffset;      /* Payload relative offset, Unicode, zero-terminated */
    CsrUint16                bodyOffset;          /* Payload relative offset */
    CsrUint16                bodyLength;
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtFtsPutObjInd;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
    CsrBool                  srmpOn;
} CsrBtFtsPutObjRes;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrUint16                bodyOffset;          /* Payload relative offset */
    CsrUint16                bodyLength;
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtFtsPutObjNextInd;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
    CsrBool                  srmpOn;
} CsrBtFtsPutObjNextRes;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrUint16                ucs2nameOffset;      /* Payload relative offset, zero-terminated */
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtFtsDelObjInd;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
    CsrBool                  srmpOn;
} CsrBtFtsDelObjRes;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrUint16                ucs2nameOffset;      /* Payload relative offset, zero-terminated */
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtFtsGetObjInd;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrBtObexResponseCode    responseCode;
    CsrUint32                lengthOfObject;
    CsrUint16                bodyLength;
    CsrUint8                *body;
    CsrBool                  srmpOn;
} CsrBtFtsGetObjRes;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
} CsrBtFtsGetObjNextInd;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrBtObexResponseCode    responseCode;
    CsrUint16                bodyLength;
    CsrUint8                *body;
    CsrBool                  srmpOn;
} CsrBtFtsGetObjNextRes;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrUint16                ucs2nameOffset;   /* Payload relative offset, zero-terminated */
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtFtsGetListFolderInd;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrBtObexResponseCode    responseCode;
    CsrUint32                lengthOfObject;
    CsrUint16                bodyLength;
    CsrUint8                *body;
    CsrBool                  srmpOn;
} CsrBtFtsGetListFolderRes;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
} CsrBtFtsGetListFolderNextInd;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrBtObexResponseCode    responseCode;
    CsrUint16                bodyLength;
    CsrUint8                *body;
    CsrBool                  srmpOn;
} CsrBtFtsGetListFolderNextRes;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrUint16                ucs2nameOffset; /* Payload relative offset, zero-terminated */
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtFtsSetFolderInd;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
} CsrBtFtsSetFolderRes;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
} CsrBtFtsSetBackFolderInd;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
} CsrBtFtsSetBackFolderRes;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
} CsrBtFtsSetRootFolderInd;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
} CsrBtFtsSetRootFolderRes;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrUint16                ucs2nameOffset;     /* Payload relative offset, zero-terminated */
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtFtsSetAddFolderInd;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
} CsrBtFtsSetAddFolderRes;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrUint16                descriptionOffset;   /* Payload relative offset */
    CsrUint16                descriptionLength;
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtFtsAbortInd;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrBtDeviceAddr          deviceAddr;
    CsrBtReasonCode          reasonCode;
    CsrBtSupplier            reasonSupplier;
} CsrBtFtsDisconnectInd;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrSchedQid              appHandle;
    CsrUint16                secLevel;
} CsrBtFtsSecurityInReq;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrBtResultCode          resultCode;
    CsrBtSupplier            resultSupplier;
} CsrBtFtsSecurityInCfm;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrUint16                ucs2srcNameOffset;
    CsrUint16                ucs2destNameOffset;
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtFtsCopyObjInd;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
} CsrBtFtsCopyObjRes;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrUint16                ucs2srcNameOffset;
    CsrUint16                ucs2destNameOffset;
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtFtsMoveObjInd;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
} CsrBtFtsMoveObjRes;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrUint16                ucs2nameOffset;
    CsrUint32                permissions;
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtFtsSetObjPermissionsInd;

typedef struct
{
    CsrBtFtsPrim             type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
} CsrBtFtsSetObjPermissionsRes;

#ifdef __cplusplus
}
#endif

#endif

