#ifndef CSR_BT_SMLS_PRIM_H__
#define CSR_BT_SMLS_PRIM_H__

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

/* search_string="CsrBtSmlsPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim    CsrBtSmlsPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_SMLS_PRIM_DOWNSTREAM_LOWEST                         (0x0000)

#define CSR_BT_SMLS_ACTIVATE_REQ                   ((CsrBtSmlsPrim) (0x0000 + CSR_BT_SMLS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SMLS_DEACTIVATE_REQ                 ((CsrBtSmlsPrim) (0x0001 + CSR_BT_SMLS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SMLS_CONNECT_RES                    ((CsrBtSmlsPrim) (0x0002 + CSR_BT_SMLS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SMLS_AUTHENTICATE_REQ               ((CsrBtSmlsPrim) (0x0003 + CSR_BT_SMLS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SMLS_AUTHENTICATE_RES               ((CsrBtSmlsPrim) (0x0004 + CSR_BT_SMLS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SMLS_PUT_SML_MSG_OBJ_RES            ((CsrBtSmlsPrim) (0x0005 + CSR_BT_SMLS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SMLS_PUT_SML_MSG_OBJ_NEXT_RES       ((CsrBtSmlsPrim) (0x0006 + CSR_BT_SMLS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SMLS_GET_SML_MSG_OBJ_RES            ((CsrBtSmlsPrim) (0x0007 + CSR_BT_SMLS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SMLS_GET_SML_MSG_OBJ_NEXT_RES       ((CsrBtSmlsPrim) (0x0008 + CSR_BT_SMLS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SMLS_SECURITY_IN_REQ                ((CsrBtSmlsPrim) (0x0009 + CSR_BT_SMLS_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_SMLS_PRIM_DOWNSTREAM_HIGHEST                        (0x0009 + CSR_BT_SMLS_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_SMLS_PRIM_UPSTREAM_LOWEST                           (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_SMLS_ACTIVATE_CFM                   ((CsrBtSmlsPrim) (0x0000 + CSR_BT_SMLS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SMLS_DEACTIVATE_CFM                 ((CsrBtSmlsPrim) (0x0001 + CSR_BT_SMLS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SMLS_CONNECT_IND                    ((CsrBtSmlsPrim) (0x0002 + CSR_BT_SMLS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SMLS_AUTHENTICATE_CFM               ((CsrBtSmlsPrim) (0x0003 + CSR_BT_SMLS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SMLS_AUTHENTICATE_IND               ((CsrBtSmlsPrim) (0x0004 + CSR_BT_SMLS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SMLS_PUT_SML_MSG_OBJ_IND            ((CsrBtSmlsPrim) (0x0005 + CSR_BT_SMLS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SMLS_PUT_SML_MSG_OBJ_NEXT_IND       ((CsrBtSmlsPrim) (0x0006 + CSR_BT_SMLS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SMLS_GET_SML_MSG_OBJ_IND            ((CsrBtSmlsPrim) (0x0007 + CSR_BT_SMLS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SMLS_GET_SML_MSG_OBJ_NEXT_IND       ((CsrBtSmlsPrim) (0x0008 + CSR_BT_SMLS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SMLS_ABORT_IND                      ((CsrBtSmlsPrim) (0x0009 + CSR_BT_SMLS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SMLS_DISCONNECT_IND                 ((CsrBtSmlsPrim) (0x000A + CSR_BT_SMLS_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SMLS_SECURITY_IN_CFM                ((CsrBtSmlsPrim) (0x000B + CSR_BT_SMLS_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_SMLS_PRIM_UPSTREAM_HIGHEST                          (0x000B + CSR_BT_SMLS_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_SMLS_PRIM_DOWNSTREAM_COUNT          (CSR_BT_SMLS_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_SMLS_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_SMLS_PRIM_UPSTREAM_COUNT            (CSR_BT_SMLS_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_SMLS_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

/* ---------- End of Primitives ----------*/

/* ---------- Begin defines used for interface parameters ----------*/

/*BEGIN defines used in the out-going API indicating what syncml service that connects*/
/*used in CsrBtSmlsConnectInd*/
#define CSR_BT_SMLS_SYNCMLSYNC_SERVICE             0
#define CSR_BT_SMLS_SYNCMLDM_SERVICE               1
#define CSR_BT_SMLS_SYNCML_SERVICE_UNKNOWN_TO_BIG  2
/*END defines used in the out-going API indicating what syncml service that connects*/

/*BEGIN defines used in result-param and req-param of datatransmission related signalling*/
/*e.g. in CsrBtSmlsPutSmlMsgObjInd*/
#define CSR_BT_SMLS_MIME_DATASYNC_XML      0
#define CSR_BT_SMLS_MIME_DATASYNC_WBXML    1
#define CSR_BT_SMLS_MIME_DEVMAN_XML        2
#define CSR_BT_SMLS_MIME_DEVMAN_WBXML      3
#define CSR_BT_SMLS_MIME_INVALID_TO_BIG    4
/*END defines used in result-param and req-param of datatransmission related signalling*/

/* ------------ END defines used for interface parameters ----------*/

typedef struct
{
    CsrBtSmlsPrim           type;
    CsrSchedQid             appHandle;
    CsrUint16               obexMaxPacketSize;
    CsrUint16               windowSize;
    CsrBool                 srmEnable;
} CsrBtSmlsActivateReq;

typedef struct
{
    CsrBtSmlsPrim           type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtSmlsActivateCfm;

typedef struct
{
    CsrBtSmlsPrim           type;
} CsrBtSmlsDeactivateReq;


typedef struct
{
    CsrBtSmlsPrim           type;
} CsrBtSmlsDeactivateCfm;


typedef struct
{
    CsrBtSmlsPrim            type;
    CsrUint32                connectionId;
    CsrUint16                obexPeerMaxPacketSize;
    CsrBtDeviceAddr          deviceAddr;
    CsrUint8                 targetService; /* What kind of sync-service is to be setup 
                                             * either SMLS_SYNCMLSYNC_SERVICE OR SMLS_SYNCMLDM_SERVICE*/
    CsrUint32                length;
    CsrUint32                count;
    CsrBtConnId              btConnId;      /* Global Bluetooth connection ID */
} CsrBtSmlsConnectInd;

typedef struct
{
    CsrBtSmlsPrim            type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
    CsrUint8                 targetService; /* What kind of sync-service is acknowledged connecting
                                             * (MUST be the same as the one recv in SmlsConnectInd->targetService)*/
} CsrBtSmlsConnectRes;

typedef struct
{
    CsrBtSmlsPrim            type;
    CsrUint16                realmLength;
    CsrUint8                *realm;
    CsrUint16                passwordLength;
    CsrUint8                *password;
    CsrCharString           *userId;/*Max length is BT_OBEX_MAX_AUTH_USERID_LENGTH */
} CsrBtSmlsAuthenticateReq;

typedef struct
{
    CsrBtSmlsPrim           type;
} CsrBtSmlsAuthenticateCfm;

typedef struct
{
    CsrBtSmlsPrim            type;
    CsrBtDeviceAddr          deviceAddr;
    CsrUint8                 options;
    CsrUint16                realmLength;
    CsrUint8                *realm;
} CsrBtSmlsAuthenticateInd;

typedef struct
{
    CsrBtSmlsPrim            type;
    CsrUint16                passwordLength;
    CsrUint8                *password;
    CsrCharString           *userId;/*Max length is BT_OBEX_MAX_AUTH_USERID_LENGTH */
} CsrBtSmlsAuthenticateRes;

typedef struct
{
    CsrBtSmlsPrim            type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrUint32                lengthOfObject;
    CsrUint8                 mimeType;
    CsrUint16                bodyOffset;
    CsrUint16                bodyLength;
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtSmlsPutSmlMsgObjInd;


typedef struct
{
    CsrBtSmlsPrim            type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
    CsrBool                  srmpOn;
} CsrBtSmlsPutSmlMsgObjRes;


typedef struct
{
    CsrBtSmlsPrim            type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrUint8                 mimeType;
    CsrUint16                bodyOffset;
    CsrUint16                bodyLength;
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtSmlsPutSmlMsgObjNextInd;

typedef struct
{
    CsrBtSmlsPrim            type;
    CsrUint32                connectionId;
    CsrBtObexResponseCode    responseCode;
    CsrBool                  srmpOn;
} CsrBtSmlsPutSmlMsgObjNextRes;

typedef struct
{
    CsrBtSmlsPrim            type;
    CsrUint32                connectionId;
    CsrUint8                 mimeType;       /*e.g. SMLS_MIME_DEVMAN_WBXML*/
} CsrBtSmlsGetSmlMsgObjInd;

typedef struct
{
    CsrBtSmlsPrim            type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrBtObexResponseCode    responseCode;
    CsrUint32                lengthOfObject;
    CsrUint16                bodyLength;
    CsrUint8                *body;
    CsrBool                  srmpOn;
} CsrBtSmlsGetSmlMsgObjRes;


typedef struct
{
    CsrBtSmlsPrim            type;
    CsrUint32                connectionId;
    CsrUint8                 mimeType;
} CsrBtSmlsGetSmlMsgObjNextInd;

typedef struct
{
    CsrBtSmlsPrim            type;
    CsrUint32                connectionId;
    CsrUint8                 finalFlag;
    CsrBtObexResponseCode    responseCode;
    CsrUint16                bodyLength;
    CsrUint8                *body;
    CsrBool                  srmpOn;
} CsrBtSmlsGetSmlMsgObjNextRes;


typedef struct
{
    CsrBtSmlsPrim            type;
    CsrUint32                connectionId;
    CsrUint16                descriptionOffset;
    CsrUint16                descriptionLength;
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtSmlsAbortInd;

typedef struct
{
    CsrBtSmlsPrim            type;
    CsrUint32                connectionId;
    CsrBtDeviceAddr          deviceAddr;
    CsrBtReasonCode          reasonCode;
    CsrBtSupplier            reasonSupplier;
} CsrBtSmlsDisconnectInd;

typedef struct
{
    CsrBtSmlsPrim            type;
    CsrSchedQid              appHandle;
    CsrUint16                secLevel;
} CsrBtSmlsSecurityInReq;

typedef struct
{
    CsrBtSmlsPrim            type;
    CsrBtResultCode          resultCode;
    CsrBtSupplier            resultSupplier;
} CsrBtSmlsSecurityInCfm;

#ifdef __cplusplus
}
#endif

#endif

