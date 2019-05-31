#ifndef CSR_BT_SMLC_PRIM_H__
#define CSR_BT_SMLC_PRIM_H__

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

/* search_string="CsrBtSmlcPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */
typedef CsrPrim                        CsrBtSmlcPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_SMLC_PRIM_DOWNSTREAM_LOWEST                         (0x0000)

#define CSR_BT_SMLC_ACTIVATE_REQ                   ((CsrBtSmlcPrim) (0x0000 + CSR_BT_SMLC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SMLC_DEACTIVATE_REQ                 ((CsrBtSmlcPrim) (0x0001 + CSR_BT_SMLC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SMLC_CONNECT_REQ                    ((CsrBtSmlcPrim) (0x0002 + CSR_BT_SMLC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SMLC_AUTHENTICATE_RES               ((CsrBtSmlcPrim) (0x0003 + CSR_BT_SMLC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SMLC_GET_SML_MSG_OBJ_REQ            ((CsrBtSmlcPrim) (0x0004 + CSR_BT_SMLC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SMLC_GET_NEXT_CHUNK_SML_MSG_OBJ_REQ ((CsrBtSmlcPrim) (0x0005 + CSR_BT_SMLC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SMLC_PUT_SML_MSG_OBJ_REQ            ((CsrBtSmlcPrim) (0x0006 + CSR_BT_SMLC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SMLC_PUT_NEXT_CHUNK_SML_MSG_OBJ_REQ ((CsrBtSmlcPrim) (0x0007 + CSR_BT_SMLC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SMLC_ABORT_REQ                      ((CsrBtSmlcPrim) (0x0008 + CSR_BT_SMLC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SMLC_DISCONNECT_REQ                 ((CsrBtSmlcPrim) (0x0009 + CSR_BT_SMLC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SMLC_CANCEL_CONNECT_REQ             ((CsrBtSmlcPrim) (0x000A + CSR_BT_SMLC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SMLC_SECURITY_IN_REQ                ((CsrBtSmlcPrim) (0x000B + CSR_BT_SMLC_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SMLC_SECURITY_OUT_REQ               ((CsrBtSmlcPrim) (0x000C + CSR_BT_SMLC_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_SMLC_PRIM_DOWNSTREAM_HIGHEST                        (0x000C + CSR_BT_SMLC_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_SMLC_PRIM_UPSTREAM_LOWEST                           (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_SMLC_ACTIVATE_CFM                   ((CsrBtSmlcPrim) (0x0000 + CSR_BT_SMLC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SMLC_DEACTIVATE_CFM                 ((CsrBtSmlcPrim) (0x0001 + CSR_BT_SMLC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SMLC_CONNECT_CFM                    ((CsrBtSmlcPrim) (0x0002 + CSR_BT_SMLC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SMLC_AUTHENTICATE_IND               ((CsrBtSmlcPrim) (0x0003 + CSR_BT_SMLC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SMLC_GET_SML_MSG_OBJ_CFM            ((CsrBtSmlcPrim) (0x0004 + CSR_BT_SMLC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SMLC_GET_NEXT_CHUNK_SML_MSG_OBJ_CFM ((CsrBtSmlcPrim) (0x0005 + CSR_BT_SMLC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SMLC_PUT_SML_MSG_OBJ_CFM            ((CsrBtSmlcPrim) (0x0006 + CSR_BT_SMLC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SMLC_PUT_NEXT_CHUNK_SML_MSG_OBJ_CFM ((CsrBtSmlcPrim) (0x0007 + CSR_BT_SMLC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SMLC_ABORT_CFM                      ((CsrBtSmlcPrim) (0x0008 + CSR_BT_SMLC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SMLC_DISCONNECT_IND                 ((CsrBtSmlcPrim) (0x0009 + CSR_BT_SMLC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SMLC_SECURITY_IN_CFM                ((CsrBtSmlcPrim) (0x000A + CSR_BT_SMLC_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SMLC_SECURITY_OUT_CFM               ((CsrBtSmlcPrim) (0x000B + CSR_BT_SMLC_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_SMLC_PRIM_UPSTREAM_HIGHEST                          (0x000B + CSR_BT_SMLC_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_SMLC_PRIM_DOWNSTREAM_COUNT          (CSR_BT_SMLC_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_SMLC_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_SMLC_PRIM_UPSTREAM_COUNT            (CSR_BT_SMLC_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_SMLC_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

/* ---------- BEGIN defines for API parameters: ----------*/

/*BEGIN defines used in the in-going API for CsrBool values indicating enable/or not*/
/*CsrBtSmlcDisconnectReq*/
#define CSR_BT_SMLC_FALSE      FALSE
#define CSR_BT_SMLC_TRUE       TRUE
/*END defines used in the in-going API for CsrBool values indicating enable/or not*/

/*BEGIN defines used in the in-going API indicating what syncml service*/
/*e.g in CsrBtSmlcConnectReq indicating what service-uuid16_t to use*/
#define CSR_BT_SMLC_SYNCMLSYNC_SERVICE     FALSE
#define CSR_BT_SMLC_SYNCMLDM_SERVICE       TRUE
/*END defines used in the in-going API indicating what syncml service*/

/*BEGIN defines used in result-param and req-param of datatransmission related signalling*/
/*e.g "SmlcSendGetSmlMsgObjReq"*/
#define CSR_BT_SMLC_MIME_DATASYNC_XML      0
#define CSR_BT_SMLC_MIME_DATASYNC_WBXML    1
#define CSR_BT_SMLC_MIME_DEVMAN_XML        2
#define CSR_BT_SMLC_MIME_DEVMAN_WBXML      3
#define CSR_BT_SMLC_MIME_INVALID_TO_BIG    4
/*END defines used in result-param and req-param of datatransmission related signalling*/

/* ---------- END defines for API parameters: ----------*/


typedef struct
{
    CsrBtSmlcPrim           type;
    CsrSchedQid             appHandle;
    CsrBool                 advEnable;          /*Has to be of value SMLC_FALSE*/
    CsrBool                 acceptServConnect;  /*Has to be of value SMLC_FALSE*/
} CsrBtSmlcActivateReq;

typedef struct
{
    CsrBtSmlcPrim           type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
    CsrBool                 advEnabled;
    CsrBool                 acceptEnabled;
} CsrBtSmlcActivateCfm;

typedef struct
{
    CsrBtSmlcPrim           type;
} CsrBtSmlcDeactivateReq;


typedef struct
{
    CsrBtSmlcPrim           type;
} CsrBtSmlcDeactivateCfm;


typedef struct
{
    CsrBtSmlcPrim            type;
    CsrSchedQid              appHandle;
    CsrUint16                maxPacketSize;
    CsrBtDeviceAddr          destination;    /*remote address*/
    CsrBool                  targetService;  /*either SMLC_SYNCMLSYNC_SERVICE OR SMLC_SYNCMLDM_SERVICE see define*/
    CsrBool                  authorize;      /*enable/disable*/
    CsrUint16                realmLength;
    CsrUint8                *realm;
    CsrUint16                passwordLength;
    CsrUint8                *password;
    CsrCharString           *userId;/*Max length is BT_OBEX_MAX_AUTH_USERID_LENGTH */
    CsrUint32                count;
    CsrUint32                length; 
    CsrUint16                windowSize;
    CsrBool                  srmEnable;
} CsrBtSmlcConnectReq;

typedef struct
{
    CsrBtSmlcPrim            type;
    CsrBtResultCode          resultCode;
    CsrBtSupplier            resultSupplier;
    CsrUint16                obexPeerMaxPacketSize;  /*CSRMAX data chunk size, complete obex-packet.....*/
    CsrUint16                availablePutBodySize;   /*byte available for bodydata in first put-req*/
    CsrBtConnId              btConnId;               /* Global Bluetooth connection ID */
} CsrBtSmlcConnectCfm;


typedef struct
{
    CsrBtSmlcPrim            type;
    CsrBtDeviceAddr          deviceAddr;
    CsrUint8                 options;
    CsrUint16                realmLength;
    CsrUint8                *realm;
} CsrBtSmlcAuthenticateInd;

typedef struct
{
    CsrBtSmlcPrim            type;
    CsrUint16                passwordLength;
    CsrUint8                *password;
    CsrCharString           *userId;/*Max length is BT_OBEX_MAX_AUTH_USERID_LENGTH */
} CsrBtSmlcAuthenticateRes;

typedef struct
{
    CsrBtSmlcPrim            type;
    CsrUint8                 mimeType;   /*See mimetype defines e.g. SMLC_MIME_DATASYNC_XML....*/
    CsrBool                  srmpOn;
} CsrBtSmlcGetSmlMsgObjReq;

typedef struct
{
    CsrBtSmlcPrim            type;
    CsrBtObexResponseCode    responseCode;
    CsrUint32                lengthOfObject;
    CsrUint8                 finalFlag;
    CsrUint16                bodyOffset;
    CsrUint16                bodyLength;
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtSmlcGetSmlMsgObjCfm;

typedef struct
{
    CsrBtSmlcPrim            type;
    CsrBool                  srmpOn;
} CsrBtSmlcGetNextChunkSmlMsgObjReq;

typedef struct
{
    CsrBtSmlcPrim            type;
    CsrBtObexResponseCode    responseCode;
    CsrUint8                 finalFlag;
    CsrUint16                bodyOffset;
    CsrUint16                bodyLength;
    CsrUint8                *payload;
    CsrUint16                payloadLength;
} CsrBtSmlcGetNextChunkSmlMsgObjCfm;

typedef struct
{
    CsrBtSmlcPrim            type;
    CsrUint8                 mimeType;       /*See mimetype defines e.g. SMLC_MIME_DATASYNC_XML...*/
    CsrUint32                lengthOfObject;
    CsrUint8                 finalFlag;
    CsrUint16                bodyLength;
    CsrUint8                *body;
} CsrBtSmlcPutSmlMsgObjReq;

typedef struct
{
    CsrBtSmlcPrim            type;
    CsrBtObexResponseCode    responseCode;
    CsrUint16                maxBodySize;    /*for nextcoming bodydata to send*/
} CsrBtSmlcPutSmlMsgObjCfm;

typedef struct
{
    CsrBtSmlcPrim            type;
    CsrUint8                 finalFlag;
    CsrUint16                bodyLength;
    CsrUint8                *body;
} CsrBtSmlcPutNextChunkSmlMsgObjReq;

typedef struct
{
    CsrBtSmlcPrim            type;
    CsrBtObexResponseCode    responseCode;
    CsrUint16                maxBodySize;    /*for nextcoming bodydata to send*/
} CsrBtSmlcPutNextChunkSmlMsgObjCfm;

typedef struct
{
    CsrBtSmlcPrim            type;
} CsrBtSmlcAbortReq;

typedef struct
{
    CsrBtSmlcPrim            type;
} CsrBtSmlcAbortCfm;

typedef struct
{
    CsrBtSmlcPrim            type;
    CsrBool                  normalDisconnect;
} CsrBtSmlcDisconnectReq;

typedef struct
{
    CsrBtSmlcPrim            type;
    CsrBtReasonCode          reasonCode;
    CsrBtSupplier            reasonSupplier;
} CsrBtSmlcDisconnectInd;

typedef struct
{
    CsrBtSmlcPrim            type;
} CsrBtSmlcCancelConnectReq;

typedef struct
{
    CsrBtSmlcPrim            type;
    CsrSchedQid              appHandle;
    CsrUint16                secLevel;
} CsrBtSmlcSecurityInReq;

typedef struct
{
    CsrBtSmlcPrim            type;
    CsrSchedQid              appHandle;
    CsrUint16                secLevel;
} CsrBtSmlcSecurityOutReq;

typedef struct
{
    CsrBtSmlcPrim            type;
    CsrBtResultCode          resultCode;
    CsrBtSupplier            resultSupplier;
} CsrBtSmlcSecurityOutCfm;

typedef struct
{
    CsrBtSmlcPrim            type;
    CsrBtResultCode          resultCode;
    CsrBtSupplier            resultSupplier;
} CsrBtSmlcSecurityInCfm;

#ifdef __cplusplus
}
#endif

#endif

