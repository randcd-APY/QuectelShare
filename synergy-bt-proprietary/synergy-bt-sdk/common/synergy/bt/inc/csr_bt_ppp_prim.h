#ifndef CSR_BT_PPP_PRIM_H__
#define CSR_BT_PPP_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_profiles.h"
#include "csr_bt_result.h"
#include "csr_bt_bsl_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtPppPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrUint16 CsrBtPppPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_PPP_PRIM_DOWNSTREAM_LOWEST                            (0x0000)

#define CSR_BT_PPP_CONNECT_REQ                        ((CsrBtPppPrim) (0x0000 + CSR_BT_PPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PPP_REGISTER_NETWORK_PROTOCOL_REQ      ((CsrBtPppPrim) (0x0001 + CSR_BT_PPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PPP_DATA_REQ                           ((CsrBtPppPrim) (0x0002 + CSR_BT_PPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PPP_NCP_REQ                            ((CsrBtPppPrim) (0x0003 + CSR_BT_PPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PPP_DISCONNECT_REQ                     ((CsrBtPppPrim) (0x0004 + CSR_BT_PPP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PPP_AUTHENTICATE_RES                   ((CsrBtPppPrim) (0x0005 + CSR_BT_PPP_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_PPP_PRIM_DOWNSTREAM_HIGHEST                           (0x0005 + CSR_BT_PPP_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_PPP_PRIM_UPSTREAM_LOWEST                              (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_PPP_CONNECT_CFM                        ((CsrBtPppPrim) (0x0000 + CSR_BT_PPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PPP_REGISTER_NETWORK_PROTOCOL_CFM      ((CsrBtPppPrim) (0x0001 + CSR_BT_PPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PPP_DATA_IND                           ((CsrBtPppPrim) (0x0002 + CSR_BT_PPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PPP_NCP_IND                            ((CsrBtPppPrim) (0x0003 + CSR_BT_PPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PPP_DOWN_IND                           ((CsrBtPppPrim) (0x0004 + CSR_BT_PPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PPP_UP_IND                             ((CsrBtPppPrim) (0x0005 + CSR_BT_PPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PPP_DISCONNECT_IND                     ((CsrBtPppPrim) (0x0006 + CSR_BT_PPP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PPP_AUTHENTICATE_IND                   ((CsrBtPppPrim) (0x0007 + CSR_BT_PPP_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_PPP_PRIM_UPSTREAM_HIGHEST                             (0x0007 + CSR_BT_PPP_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_PPP_PRIM_DOWNSTREAM_COUNT              (CSR_BT_PPP_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_PPP_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_PPP_PRIM_UPSTREAM_COUNT                (CSR_BT_PPP_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_PPP_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrBtPppPrim            type;
    CsrSchedQid                  applicationQueue;
    CsrBtDeviceAddr         deviceAddr;
} CsrBtPppConnectReq;

typedef struct
{
    CsrBtPppPrim            type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
    CsrUint16               maxPayloadLength;
} CsrBtPppConnectCfm;

typedef struct
{
    CsrBtPppPrim            type;
    CsrSchedQid                  responseQueue;
    CsrUint16               ncpProtocol;
    CsrSchedQid                  dataQueue;
    CsrUint16               dataProtocol;
} CsrBtPppRegisterNetworkProtocolReq;

typedef struct
{
    CsrBtPppPrim            type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtPppRegisterNetworkProtocolCfm;


typedef struct
{
    CsrBtPppPrim            type;
    CsrUint16               protocol;    /* Network order */
    CsrUint16               payloadLength;
    CsrUint8                * payload;
} CsrBtPppDataReq;

typedef CsrBtPppDataReq CsrBtPppNcpReq;

typedef struct
{
    CsrBtPppPrim            type;
    CsrUint16               protocol;    /* Network order */
    CsrUint16               payloadLength;
    CsrUint8                * payload;
} CsrBtPppDataInd;

typedef CsrBtPppDataInd CsrBtPppNcpInd;

typedef struct
{
    CsrBtPppPrim            type;
} CsrBtPppDownInd;

typedef CsrBtPppDownInd CsrBtPppUpInd;

typedef struct
{
    CsrBtPppPrim            type;
} CsrBtPppDisconnectReq;

typedef struct
{
    CsrBtPppPrim            type;
    CsrUint8                reason;
} CsrBtPppDisconnectInd;

typedef struct
{
    CsrBtPppPrim            type;
} CsrBtPppAuthenticateInd;

typedef struct
{
    CsrBtPppPrim            type;
    CsrCharString               *loginName;
    CsrCharString               *password;
} CsrBtPppAuthenticateRes;

#ifdef __cplusplus
}
#endif

#endif
