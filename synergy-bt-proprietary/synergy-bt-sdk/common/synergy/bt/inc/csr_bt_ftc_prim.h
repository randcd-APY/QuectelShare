#ifndef CSR_BT_FTC_PRIM_H__
#define CSR_BT_FTC_PRIM_H__

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

/* search_string="CsrBtFtcPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim    CsrBtFtcPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_FTC_PRIM_DOWNSTREAM_LOWEST                             (0x0000)

#define CSR_BT_FTC_CONNECT_REQ                         ((CsrBtFtcPrim) (0x0000 + CSR_BT_FTC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTC_AUTHENTICATE_RES                    ((CsrBtFtcPrim) (0x0001 + CSR_BT_FTC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTC_GET_LIST_FOLDER_REQ                 ((CsrBtFtcPrim) (0x0002 + CSR_BT_FTC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTC_GET_LIST_FOLDER_BODY_REQ            ((CsrBtFtcPrim) (0x0003 + CSR_BT_FTC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTC_GET_OBJ_REQ                         ((CsrBtFtcPrim) (0x0004 + CSR_BT_FTC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTC_GET_OBJ_BODY_REQ                    ((CsrBtFtcPrim) (0x0005 + CSR_BT_FTC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTC_PUT_OBJ_HEADER_REQ                  ((CsrBtFtcPrim) (0x0006 + CSR_BT_FTC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTC_PUT_OBJ_BODY_REQ                    ((CsrBtFtcPrim) (0x0007 + CSR_BT_FTC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTC_DEL_OBJ_REQ                         ((CsrBtFtcPrim) (0x0008 + CSR_BT_FTC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTC_SET_FOLDER_REQ                      ((CsrBtFtcPrim) (0x0009 + CSR_BT_FTC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTC_SET_BACK_FOLDER_REQ                 ((CsrBtFtcPrim) (0x000A + CSR_BT_FTC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTC_SET_ROOT_FOLDER_REQ                 ((CsrBtFtcPrim) (0x000B + CSR_BT_FTC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTC_SET_ADD_FOLDER_REQ                  ((CsrBtFtcPrim) (0x000C + CSR_BT_FTC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTC_ABORT_REQ                           ((CsrBtFtcPrim) (0x000D + CSR_BT_FTC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTC_DISCONNECT_REQ                      ((CsrBtFtcPrim) (0x000E + CSR_BT_FTC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTC_CANCEL_CONNECT_REQ                  ((CsrBtFtcPrim) (0x000F + CSR_BT_FTC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTC_SECURITY_OUT_REQ                    ((CsrBtFtcPrim) (0x0010 + CSR_BT_FTC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTC_COPYING_OBJ_REQ                     ((CsrBtFtcPrim) (0x0011 + CSR_BT_FTC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTC_MOVING_OBJ_REQ                      ((CsrBtFtcPrim) (0x0012 + CSR_BT_FTC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTC_SET_OBJ_PERMISSIONS_REQ             ((CsrBtFtcPrim) (0x0013 + CSR_BT_FTC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_FTC_PRIM_DOWNSTREAM_HIGHEST                            (0x0013 + CSR_BT_FTC_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_FTC_PRIM_UPSTREAM_LOWEST                               (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_FTC_CONNECT_CFM                         ((CsrBtFtcPrim) (0x0000 + CSR_BT_FTC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTC_AUTHENTICATE_IND                    ((CsrBtFtcPrim) (0x0001 + CSR_BT_FTC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTC_GET_LIST_FOLDER_CFM                 ((CsrBtFtcPrim) (0x0002 + CSR_BT_FTC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTC_GET_LIST_FOLDER_BODY_CFM            ((CsrBtFtcPrim) (0x0003 + CSR_BT_FTC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTC_GET_OBJ_CFM                         ((CsrBtFtcPrim) (0x0004 + CSR_BT_FTC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTC_GET_OBJ_BODY_CFM                    ((CsrBtFtcPrim) (0x0005 + CSR_BT_FTC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTC_PUT_OBJ_HEADER_CFM                  ((CsrBtFtcPrim) (0x0006 + CSR_BT_FTC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTC_PUT_OBJ_BODY_CFM                    ((CsrBtFtcPrim) (0x0007 + CSR_BT_FTC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTC_DEL_OBJ_CFM                         ((CsrBtFtcPrim) (0x0008 + CSR_BT_FTC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTC_SET_FOLDER_CFM                      ((CsrBtFtcPrim) (0x0009 + CSR_BT_FTC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTC_SET_BACK_FOLDER_CFM                 ((CsrBtFtcPrim) (0x000A + CSR_BT_FTC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTC_SET_ROOT_FOLDER_CFM                 ((CsrBtFtcPrim) (0x000B + CSR_BT_FTC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTC_SET_ADD_FOLDER_CFM                  ((CsrBtFtcPrim) (0x000C + CSR_BT_FTC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTC_ABORT_CFM                           ((CsrBtFtcPrim) (0x000D + CSR_BT_FTC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTC_DISCONNECT_IND                      ((CsrBtFtcPrim) (0x000E + CSR_BT_FTC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTC_SECURITY_OUT_CFM                    ((CsrBtFtcPrim) (0x000F + CSR_BT_FTC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTC_COPYING_OBJ_CFM                     ((CsrBtFtcPrim) (0x0010 + CSR_BT_FTC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTC_MOVING_OBJ_CFM                      ((CsrBtFtcPrim) (0x0011 + CSR_BT_FTC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_FTC_SET_OBJ_PERMISSIONS_CFM             ((CsrBtFtcPrim) (0x0012 + CSR_BT_FTC_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_FTC_PRIM_UPSTREAM_HIGHEST                              (0x0012 + CSR_BT_FTC_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_FTC_PRIM_DOWNSTREAM_COUNT               (CSR_BT_FTC_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_FTC_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_FTC_PRIM_UPSTREAM_COUNT                 (CSR_BT_FTC_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_FTC_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrBtFtcPrim            type;
    CsrSchedQid             appHandle;
    CsrUint16               maxPacketSize;
    CsrBtDeviceAddr         destination;
    CsrBool                 authorize;
    CsrUint16               realmLength;
    CsrUint8               *realm;
    CsrUint16               passwordLength;
    CsrUint8               *password;
    CsrCharString          *userId; /* max length is BT_OBEX_MAX_AUTH_USERID_LENGTH */
    CsrUint32               count;
    CsrUint32               length;
    CsrUint16               windowSize; /* number of packets to queue */
    CsrBool                 srmEnable;
} CsrBtFtcConnectReq;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrUint16               obexPeerMaxPacketSize;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
    CsrBtConnId             btConnId;               /* Global Bluetooth connection ID */
} CsrBtFtcConnectCfm;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrBtDeviceAddr         deviceAddr;
    CsrUint8                options;
    CsrUint16               realmLength;
    CsrUint8               *realm;
} CsrBtFtcAuthenticateInd;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrUint8               *password;
    CsrUint16               passwordLength;
    CsrCharString          *userId; /* max length is BT_OBEX_MAX_AUTH_USERID_LENGTH */
} CsrBtFtcAuthenticateRes;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrUcs2String          *ucs2name;        /* Unicode name, zero-terminated */
    CsrBool                 srmpOn;
} CsrBtFtcGetListFolderReq;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrBtObexResponseCode   responseCode;
    CsrUint32               lengthOfObject;
    CsrUint8                finalFlag;
    CsrUint16               bodyOffset;       /* Payload relative offset */
    CsrUint16               bodyLength;       /* Length of body */
    CsrUint8               *payload;
    CsrUint16               payloadLength;
} CsrBtFtcGetListFolderCfm;

typedef struct
{
    CsrBtFtcPrim             type;
    CsrBool                  srmpOn;
} CsrBtFtcGetListFolderBodyReq;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrBtObexResponseCode   responseCode;
    CsrUint8                finalFlag;
    CsrUint16               bodyOffset;       /* Payload relative offset */
    CsrUint16               bodyLength;
    CsrUint8               *payload;
    CsrUint16               payloadLength;
} CsrBtFtcGetListFolderBodyCfm;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrUcs2String          *ucs2name;       /* Unicode name, zero-terminated */
    CsrBool                 srmpOn;
} CsrBtFtcGetObjReq;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrBtObexResponseCode   responseCode;
    CsrUint32               lengthOfObject;
    CsrUint8                finalFlag;
    CsrUint16               bodyOffset;      /* Payload relative offset */
    CsrUint16               bodyLength;
    CsrUint8               *payload;
    CsrUint16               payloadLength;
} CsrBtFtcGetObjCfm;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrBool                 srmpOn;
} CsrBtFtcGetObjBodyReq;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrBtObexResponseCode   responseCode;
    CsrUint8                finalFlag;
    CsrUint16               bodyOffset;       /* Payload relative offset */
    CsrUint16               bodyLength;
    CsrUint8               *payload;
    CsrUint16               payloadLength;
} CsrBtFtcGetObjBodyCfm;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrUcs2String          *ucs2name;        /* Unicode name, zero-terminated */
    CsrUint32               lengthOfObject;
} CsrBtFtcPutObjHeaderReq;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrBtObexResponseCode   responseCode;
    CsrUint16               bodySize;
} CsrBtFtcPutObjHeaderCfm;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrUint8                finalFlag;
    CsrUint16               bodyLength;
    CsrUint8               *body;
} CsrBtFtcPutObjBodyReq;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrBtObexResponseCode   responseCode;
    CsrUint16               bodySize;
} CsrBtFtcPutObjBodyCfm;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrUcs2String           *ucs2name;       /* Unicode name, zero-terminated */
} CsrBtFtcDelObjReq;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrBtObexResponseCode   responseCode;
} CsrBtFtcDelObjCfm;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrUcs2String          *ucs2name;       /* Unicode name, zero-terminated */
} CsrBtFtcSetFolderReq;

typedef struct
{
    CsrBtFtcPrim            type;
} CsrBtFtcSetBackFolderReq;

typedef struct
{
    CsrBtFtcPrim            type;
} CsrBtFtcSetRootFolderReq;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrUcs2String          *ucs2name;         /* Unicode name, zero-terminated */
} CsrBtFtcSetAddFolderReq;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrBtObexResponseCode   responseCode;
} CsrBtFtcSetFolderCfm;

typedef CsrBtFtcSetFolderCfm CsrBtFtcSetBackFolderCfm;
typedef CsrBtFtcSetFolderCfm CsrBtFtcSetRootFolderCfm;
typedef CsrBtFtcSetFolderCfm CsrBtFtcSetAddFolderCfm;

typedef struct
{
    CsrBtFtcPrim            type;
} CsrBtFtcAbortReq;

typedef struct
{
    CsrBtFtcPrim            type;
} CsrBtFtcAbortCfm;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrBool                 normalDisconnect;
} CsrBtFtcDisconnectReq;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrBtReasonCode         reasonCode;
    CsrBtSupplier           reasonSupplier;
} CsrBtFtcDisconnectInd;

typedef struct
{
    CsrBtFtcPrim            type;
} CsrBtFtcCancelConnectReq;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrSchedQid             appHandle;
    CsrUint16               secLevel;
} CsrBtFtcSecurityOutReq;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtFtcSecurityOutCfm;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrUcs2String          *ucs2SrcName;
    CsrUcs2String          *ucs2DestName;
} CsrBtFtcCopyingObjReq;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrBtObexResponseCode   responseCode;
} CsrBtFtcCopyingObjCfm;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrUcs2String          *ucs2SrcName;
    CsrUcs2String          *ucs2DestName;
} CsrBtFtcMovingObjReq;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrBtObexResponseCode   responseCode;
} CsrBtFtcMovingObjCfm;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrUcs2String           *ucs2name;
    CsrUint32               permissions;
} CsrBtFtcSetObjPermissionsReq;

typedef struct
{
    CsrBtFtcPrim            type;
    CsrBtObexResponseCode   responseCode;
} CsrBtFtcSetObjPermissionsCfm;

#ifdef __cplusplus
}
#endif

#endif

