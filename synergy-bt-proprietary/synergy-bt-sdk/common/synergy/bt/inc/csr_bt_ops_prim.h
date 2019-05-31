#ifndef CSR_BT_OPS_PRIM_H__
#define CSR_BT_OPS_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2010 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_synergy.h"

#include "csr_types.h"
#include "csr_unicode.h"
#include "csr_bt_result.h"
#include "csr_bt_profiles.h"
#include "csr_bt_obex.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtOpsPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim    CsrBtOpsPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_OPS_PRIM_DOWNSTREAM_LOWEST                          (0x0000)

#define CSR_BT_OPS_ACTIVATE_REQ                     ((CsrBtOpsPrim) (0x0000 + CSR_BT_OPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_OPS_DEACTIVATE_REQ                   ((CsrBtOpsPrim) (0x0001 + CSR_BT_OPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_OPS_PUT_RES                          ((CsrBtOpsPrim) (0x0002 + CSR_BT_OPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_OPS_PUT_NEXT_RES                     ((CsrBtOpsPrim) (0x0003 + CSR_BT_OPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_OPS_GET_RES                          ((CsrBtOpsPrim) (0x0004 + CSR_BT_OPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_OPS_GET_NEXT_RES                     ((CsrBtOpsPrim) (0x0005 + CSR_BT_OPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_OPS_CONNECT_RES                      ((CsrBtOpsPrim) (0x0006 + CSR_BT_OPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_OPS_SECURITY_IN_REQ                  ((CsrBtOpsPrim) (0x0007 + CSR_BT_OPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_OPS_AUTHENTICATE_RES                 ((CsrBtOpsPrim) (0x0008 + CSR_BT_OPS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_OPS_PRIM_DOWNSTREAM_HIGHEST                          (0x0008 + CSR_BT_OPS_PRIM_DOWNSTREAM_LOWEST)
/*******************************************************************************/

#define CSR_BT_OPS_PRIM_UPSTREAM_LOWEST                            (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_OPS_DEACTIVATE_CFM                   ((CsrBtOpsPrim) (0x0000 + CSR_BT_OPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_OPS_PUT_IND                          ((CsrBtOpsPrim) (0x0001 + CSR_BT_OPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_OPS_PUT_NEXT_IND                     ((CsrBtOpsPrim) (0x0002 + CSR_BT_OPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_OPS_GET_IND                          ((CsrBtOpsPrim) (0x0003 + CSR_BT_OPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_OPS_GET_NEXT_IND                     ((CsrBtOpsPrim) (0x0004 + CSR_BT_OPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_OPS_ABORT_IND                        ((CsrBtOpsPrim) (0x0005 + CSR_BT_OPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_OPS_CONNECT_IND                      ((CsrBtOpsPrim) (0x0006 + CSR_BT_OPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_OPS_DISCONNECT_IND                   ((CsrBtOpsPrim) (0x0007 + CSR_BT_OPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_OPS_SECURITY_IN_CFM                  ((CsrBtOpsPrim) (0x0008 + CSR_BT_OPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_OPS_AUTHENTICATE_IND                 ((CsrBtOpsPrim) (0x0009 + CSR_BT_OPS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_OPS_PRIM_UPSTREAM_HIGHEST                            (0x0009 + CSR_BT_OPS_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_OPS_PRIM_DOWNSTREAM_COUNT                           (CSR_BT_OPS_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_OPS_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_OPS_PRIM_UPSTREAM_COUNT                             (CSR_BT_OPS_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_OPS_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

#define CSR_BT_NUMBER_OF_INDIVIDUAL_FORMATS 6 /*vCard 2.1 and 3.0, vCal, ical, vnote and vmessage*/

typedef struct
{
    CsrBtOpsPrim             type;
    CsrSchedQid              appHandle;
    CsrUint16                supportedFormats;
    CsrUint16                obexMaxPacketSize;
    CsrUint16                windowSize;
    CsrBool                  srmEnable;
} CsrBtOpsActivateReq;

typedef struct
{
    CsrBtOpsPrim             type;
} CsrBtOpsDeactivateReq;

typedef struct
{
    CsrBtOpsPrim             type;
} CsrBtOpsDeactivateCfm;

typedef struct
{
    CsrBtOpsPrim             type;
    CsrUint8                 finalFlag;
    CsrUint32                totalObjectSize;   /* Total object size (as it may be split into multiple packets) */
    CsrUint16                bodyTypeOffset;    /* Payload relative offset */
    CsrUint16                bodyTypeLength;
    CsrUint16                ucs2nameOffset;    /* Payload relative offset, unicode, zero-terminated */
    CsrUint16                bodyOffset;        /* Payload relative offset */
    CsrUint32                bodyLength;
    CsrUint16                payloadLength;
    CsrUint8                *payload;
} CsrBtOpsPutInd;

typedef struct
{
    CsrBtOpsPrim            type;
    CsrBtObexResponseCode   responseCode;
    CsrBool                 srmpOn;
} CsrBtOpsPutRes;

typedef struct
{
    CsrBtOpsPrim             type;
    CsrUint8                 finalFlag;
    CsrUint16                bodyTypeOffset;   /* Payload relative offset */
    CsrUint16                bodyTypeLength;
    CsrUint16                ucs2nameOffset;   /* Payload relative offset, unicode, zero-terminated */
    CsrUint16                bodyOffset;       /* Payload relative offset */
    CsrUint32                bodyLength;
    CsrUint16                payloadLength;
    CsrUint8                *payload;
} CsrBtOpsPutNextInd;

typedef struct
{
    CsrBtOpsPrim             type;
    CsrBtObexResponseCode    responseCode;
    CsrBool                  srmpOn;
} CsrBtOpsPutNextRes;

typedef struct
{
    CsrBtOpsPrim            type;
    CsrUint8                bodyType;
} CsrBtOpsGetInd;

typedef struct
{
    CsrBtOpsPrim             type;
    CsrBtObexResponseCode    responseCode;
    CsrUint8                 finalFlag;
    CsrUint8                 bodyType;
    CsrUint32                totalObjectSize;
    CsrUcs2String           *ucs2name;         /* Unicode string, zero-terminated */
    CsrUint16                bodyLength;
    CsrUint8                *body;
    CsrBool                  srmpOn;
} CsrBtOpsGetRes;

typedef struct
{
    CsrBtOpsPrim             type;
    CsrUint8                 bodyType;
} CsrBtOpsGetNextInd;

typedef struct
{
    CsrBtOpsPrim             type;
    CsrBtObexResponseCode    responseCode;
    CsrUint8                 finalFlag;
    CsrUint32                bodyLength;
    CsrUint8                *body;
    CsrBool                  srmpOn;
} CsrBtOpsGetNextRes;

typedef struct
{
    CsrBtOpsPrim             type;
    CsrUint16                descriptionOffset;
    CsrUint16                descriptionLength;
    CsrUint16                payloadLength;
    CsrUint8                *payload;
} CsrBtOpsAbortInd;

typedef struct
{
    CsrBtOpsPrim             type;
    CsrUint16                obexPeerMaxPacketSize;
    CsrBtDeviceAddr          deviceAddr;
    CsrUint32                length;
    CsrUint32                count;
    CsrBtConnId              btConnId;               /* Global Bluetooth connection ID */
#ifdef CSR_BT_OBEX_AUTH_OPS_NON_SPEC_COMPLIANT_TEST_DO_NOT_USE
    CsrBool                  authenticated;
    CsrBool                  challenge;
#endif /* CSR_BT_OBEX_AUTH_OPS_NON_SPEC_COMPLIANT_TEST_DO_NOT_USE */
} CsrBtOpsConnectInd;

typedef struct
{
    CsrBtOpsPrim             type;
    CsrBtObexResponseCode    responseCode;
#ifdef CSR_BT_OBEX_AUTH_OPS_NON_SPEC_COMPLIANT_TEST_DO_NOT_USE
    CsrUint16                realmLength;
    CsrUint8                *realm;
    CsrUint16                passwordLength;
    CsrUint8                *password;
    CsrCharString           *userId;/*Max length is BT_OBEX_MAX_AUTH_USERID_LENGTH */
    CsrBool                  authenticateResponse;
#endif
} CsrBtOpsConnectRes;

typedef struct
{
    CsrBtOpsPrim             type;
    CsrBtDeviceAddr          deviceAddr;
    CsrBtReasonCode          reasonCode;
    CsrBtSupplier            reasonSupplier;
} CsrBtOpsDisconnectInd;

typedef struct
{
    CsrBtOpsPrim             type;
    CsrSchedQid              appHandle;
    CsrUint16                secLevel;
} CsrBtOpsSecurityInReq;

typedef struct
{
    CsrBtOpsPrim             type;
    CsrBtResultCode          resultCode;
    CsrBtSupplier            resultSupplier;
} CsrBtOpsSecurityInCfm;

typedef struct
{
    CsrBtOpsPrim            type;
    CsrBtDeviceAddr         deviceAddr;
    CsrUint8                options;
    CsrUint16               realmLength;
    CsrUint8               *realm;
} CsrBtOpsAuthenticateInd;

typedef struct
{
    CsrBtOpsPrim             type;
    CsrUint8                *password;
    CsrUint16                passwordLength;
    CsrCharString           *userId;/*Max length is BT_OBEX_MAX_AUTH_USERID_LENGTH */
} CsrBtOpsAuthenticateRes;

#ifdef __cplusplus
}
#endif

#endif
