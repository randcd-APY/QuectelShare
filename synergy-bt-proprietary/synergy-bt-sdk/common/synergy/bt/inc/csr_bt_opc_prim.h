#ifndef CSR_BT_OPC_PRIM_H__
#define CSR_BT_OPC_PRIM_H__

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

/* search_string="CsrBtOpcPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim    CsrBtOpcPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_OPC_PRIM_DOWNSTREAM_LOWEST                              (0x0000)

#define CSR_BT_OPC_CONNECT_REQ                          ((CsrBtOpcPrim) (0x0000 + CSR_BT_OPC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_OPC_ABORT_REQ                            ((CsrBtOpcPrim) (0x0001 + CSR_BT_OPC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_OPC_DISCONNECT_REQ                       ((CsrBtOpcPrim) (0x0002 + CSR_BT_OPC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_OPC_PUT_REQ                              ((CsrBtOpcPrim) (0x0003 + CSR_BT_OPC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_OPC_PUT_OBJECT_RES                       ((CsrBtOpcPrim) (0x0004 + CSR_BT_OPC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_OPC_GET_HEADER_REQ                       ((CsrBtOpcPrim) (0x0005 + CSR_BT_OPC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_OPC_GET_OBJECT_REQ                       ((CsrBtOpcPrim) (0x0006 + CSR_BT_OPC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_OPC_CANCEL_CONNECT_REQ                   ((CsrBtOpcPrim) (0x0007 + CSR_BT_OPC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_OPC_SECURITY_OUT_REQ                     ((CsrBtOpcPrim) (0x0008 + CSR_BT_OPC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_OPC_AUTHENTICATE_RES                     ((CsrBtOpcPrim) (0x0009 + CSR_BT_OPC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_OPC_PRIM_DOWNSTREAM_HIGHEST                              (0x0009 + CSR_BT_OPC_PRIM_DOWNSTREAM_LOWEST)


/*******************************************************************************/

#define CSR_BT_OPC_PRIM_UPSTREAM_LOWEST                                (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_OPC_CONNECT_CFM                          ((CsrBtOpcPrim) (0x0000 + CSR_BT_OPC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_OPC_ABORT_CFM                            ((CsrBtOpcPrim) (0x0001 + CSR_BT_OPC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_OPC_DISCONNECT_IND                       ((CsrBtOpcPrim) (0x0002 + CSR_BT_OPC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_OPC_PUT_CFM                              ((CsrBtOpcPrim) (0x0003 + CSR_BT_OPC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_OPC_PUT_OBJECT_IND                       ((CsrBtOpcPrim) (0x0004 + CSR_BT_OPC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_OPC_GET_HEADER_CFM                       ((CsrBtOpcPrim) (0x0005 + CSR_BT_OPC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_OPC_GET_OBJECT_CFM                       ((CsrBtOpcPrim) (0x0006 + CSR_BT_OPC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_OPC_SECURITY_OUT_CFM                     ((CsrBtOpcPrim) (0x0007 + CSR_BT_OPC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_OPC_AUTHENTICATE_IND                     ((CsrBtOpcPrim) (0x0008 + CSR_BT_OPC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_OPC_PRIM_UPSTREAM_HIGHEST                                (0x0008 + CSR_BT_OPC_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_OPC_PRIM_DOWNSTREAM_COUNT                (CSR_BT_OPC_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_OPC_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_OPC_PRIM_UPSTREAM_COUNT                  (CSR_BT_OPC_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_OPC_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrBtOpcPrim           type;
    CsrSchedQid            appHandle;
    CsrUint16              maxPacketSize;
    CsrBtDeviceAddr        deviceAddr;
    CsrUint32              count;
    CsrUint32              length; 
    CsrUint16              realmLength;
    CsrUint8              *realm;
    CsrUint16              passwordLength;
    CsrUint8              *password;
    CsrCharString         *userId;/*Max length is BT_OBEX_MAX_AUTH_USERID_LENGTH */
    CsrUint16              windowSize;
    CsrBool                srmEnable;
} CsrBtOpcConnectReq;

typedef struct
{
    CsrBtOpcPrim            type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
    CsrUint16               maxPeerPacketSize;
    CsrUint16               supportedFormats;
    CsrBtConnId             btConnId;               /* Global Bluetooth connection ID */
} CsrBtOpcConnectCfm;

typedef struct
{
    CsrBtOpcPrim            type;
} CsrBtOpcAbortReq;

typedef struct
{
    CsrBtOpcPrim            type;
    CsrBool                 normalDisconnect;
} CsrBtOpcDisconnectReq;

typedef struct
{
    CsrBtOpcPrim            type;
} CsrBtOpcAbortCfm;

typedef struct
{
    CsrBtOpcPrim           type;
    CsrBtReasonCode        reasonCode;
    CsrBtSupplier          reasonSupplier;
} CsrBtOpcDisconnectInd;

typedef struct
{
    CsrBtOpcPrim            type;
    CsrUint8               *bodyType;
    CsrUint16               bodyTypeLength;
    CsrUint32               lengthOfObject;
    CsrUcs2String          *ucs2name;           /* Unicode name, zero-terminated */
} CsrBtOpcPutReq;

typedef struct
{
    CsrBtOpcPrim            type;
    CsrUint16               objectLength;
} CsrBtOpcPutObjectInd;

typedef struct
{
    CsrBtOpcPrim            type;
    CsrBool                 finalFlag;
    CsrUint16               objectLength;
    CsrUint8               *object;
} CsrBtOpcPutObjectRes;

typedef struct
{
    CsrBtOpcPrim            type;
    CsrBtObexResponseCode  responseCode;
} CsrBtOpcPutCfm;

typedef struct
{
    CsrBtOpcPrim            type;
    CsrUint8                bodyType;
    CsrBool                 srmpOn;
} CsrBtOpcGetHeaderReq;

typedef struct
{
    CsrBtOpcPrim            type;
    CsrBool                 srmpOn;
} CsrBtOpcGetObjectReq;

typedef struct
{
    CsrBtOpcPrim            type;
    CsrBtObexResponseCode   responseCode;
    CsrUint32               totalObjectSize;     /* Object size in total (as it may be split across multiple packets) */
    CsrUint16               objectBodyOffset;    /* Payload relative offset */
    CsrUint16               objectBodyLength;
    CsrUint16               ucs2nameOffset;      /* Payload relative offset, unicode, zero-terminated */
    CsrUint8               *payload;
    CsrUint16               payloadLength;
} CsrBtOpcGetHeaderCfm;

typedef struct
{
    CsrBtOpcPrim            type;
    CsrBtObexResponseCode   responseCode;
    CsrUint16               objectBodyOffset;    /* Payload relative offset */
    CsrUint16               objectBodyLength;
    CsrUint8               *payload;
    CsrUint16               payloadLength;
} CsrBtOpcGetObjectCfm;

typedef struct
{
    CsrBtOpcPrim            type;
} CsrBtOpcHouseCleaning;

typedef struct
{
    CsrBtOpcPrim            type;
} CsrBtOpcCancelConnectReq;

typedef struct
{
    CsrBtOpcPrim            type;
    CsrSchedQid             appHandle;
    CsrUint16               secLevel;
} CsrBtOpcSecurityOutReq;

typedef struct
{
    CsrBtOpcPrim            type;
    CsrBtResultCode        resultCode;
    CsrBtSupplier    resultSupplier;
} CsrBtOpcSecurityOutCfm;

typedef struct
{
    CsrBtOpcPrim            type;
    CsrBtDeviceAddr         deviceAddr;
    CsrUint8                options;
    CsrUint16               realmLength;
    CsrUint8               *realm;
} CsrBtOpcAuthenticateInd;

typedef struct
{
    CsrBtOpcPrim           type;
    CsrUint8              *authPassword;
    CsrUint16              authPasswordLength;
    CsrCharString         *authUserId;/*Max length is BT_OBEX_MAX_AUTH_USERID_LENGTH */
    CsrUint16              chalRealmLength;
    CsrUint8              *chalRealm;
    CsrUint16              chalPasswordLength;
    CsrUint8              *chalPassword;
    CsrCharString         *chalUserId;/*Max length is BT_OBEX_MAX_AUTH_USERID_LENGTH */
} CsrBtOpcAuthenticateRes;

#ifdef __cplusplus
}
#endif

#endif
