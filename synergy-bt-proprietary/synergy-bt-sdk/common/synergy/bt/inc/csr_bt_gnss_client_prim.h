#ifndef CSR_BT_GNSS_CLIENT_PRIM_H__
#define CSR_BT_GNSS_CLIENT_PRIM_H__

/****************************************************************************

Copyright (c) 2013-2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_result.h"
#include "csr_bt_profiles.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* search_string="CsrBtGnssClientPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */


#define CSR_BT_RESULT_CODE_GNSS_CLIENT_SUCCESS                          ((CsrBtResultCode) (0x0))
#define CSR_BT_RESULT_CODE_GNSS_CLIENT_UNSPECIFICED_ERROR               ((CsrBtResultCode) (0x1))
#define CSR_BT_RESULT_CODE_GNSS_CLIENT_CONNECTION_CANCELLED             ((CsrBtResultCode) (0x2))
#define CSR_BT_RESULT_CODE_GNSS_CLIENT_DISCONNECT_REMOTE                ((CsrBtResultCode) (0x3))
#define CSR_BT_RESULT_CODE_GNSS_CLIENT_DISCONNECT_LOCAL                 ((CsrBtResultCode) (0x4))
#define CSR_BT_RESULT_CODE_GNSS_CLIENT_ALREADY_CONNECTED                ((CsrBtResultCode) (0x5))
#define CSR_BT_RESULT_CODE_GNSS_CLIENT_NO_CONNECTION                    ((CsrBtResultCode) (0x6))
#define CSR_BT_RESULT_CODE_GNSS_CLIENT_EXCESS_PAYLOAD                   ((CsrBtResultCode) (0x7))
#define CSR_BT_RESULT_CODE_GNSS_CLIENT_UNACCEPTABLE_PARAMETER           ((CsrBtResultCode) (0x8))
#define CSR_BT_RESULT_CODE_GNSS_CLIENT_UNEXPECTED_REQUEST               ((CsrBtResultCode) (0x9))

typedef CsrPrim CsrBtGnssClientPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_GNSS_CLIENT_PRIM_DOWNSTREAM_LOWEST               (0x0000)

#define CSR_BT_GNSS_CLIENT_CONNECT_REQ                          ((CsrBtGnssClientPrim) (0x0000 + CSR_BT_GNSS_CLIENT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GNSS_CLIENT_CANCEL_CONNECT_REQ                   ((CsrBtGnssClientPrim) (0x0001 + CSR_BT_GNSS_CLIENT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GNSS_CLIENT_DISCONNECT_REQ                       ((CsrBtGnssClientPrim) (0x0002 + CSR_BT_GNSS_CLIENT_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GNSS_CLIENT_DATA_REQ                             ((CsrBtGnssClientPrim) (0x0003 + CSR_BT_GNSS_CLIENT_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_GNSS_CLIENT_PRIM_DOWNSTREAM_HIGHEST              (0x0003 + CSR_BT_GNSS_CLIENT_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_GNSS_CLIENT_PRIM_UPSTREAM_LOWEST                 (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_GNSS_CLIENT_CONNECT_CFM                          ((CsrBtGnssClientPrim) (0x0000 + CSR_BT_GNSS_CLIENT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GNSS_CLIENT_DISCONNECT_IND                       ((CsrBtGnssClientPrim) (0x0001 + CSR_BT_GNSS_CLIENT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GNSS_CLIENT_DATA_CFM                             ((CsrBtGnssClientPrim) (0x0002 + CSR_BT_GNSS_CLIENT_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GNSS_CLIENT_DATA_IND                             ((CsrBtGnssClientPrim) (0x0003 + CSR_BT_GNSS_CLIENT_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_GNSS_CLIENT_PRIM_UPSTREAM_HIGHEST                (0x0003 + CSR_BT_GNSS_CLIENT_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_GNSS_CLIENT_PRIM_DOWNSTREAM_COUNT                (CSR_BT_GNSS_CLIENT_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_GNSS_CLIENT_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_GNSS_CLIENT_PRIM_UPSTREAM_COUNT                  (CSR_BT_GNSS_CLIENT_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_GNSS_CLIENT_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

/* Downstream messages */
typedef struct
{
    CsrBtGnssClientPrim         type;           /* Primitive/message identity. */
    CsrSchedQid                 appHandle;      /* Application handle. */
    BD_ADDR_T                   bdAddr;         /* Bluetooth address of the device to connect. */
    CsrUint16                   secLevel;       /* Outgoing security level. */
} CsrBtGnssClientConnectReq;

typedef struct
{
    CsrBtGnssClientPrim         type;           /* Primitive/message identity. */
    CsrSchedQid                 appHandle;      /* Application handle. */
    BD_ADDR_T                   bdAddr;         /* Bluetooth address of the device to which a connection is in progress. */
} CsrBtGnssClientCancelConnectReq;

typedef struct
{
    CsrBtGnssClientPrim         type;           /* Primitive/message identity */
    CsrBtConnId                 btConnId;       /* Connection id for the established connection. */
} CsrBtGnssClientDisconnectReq;

typedef struct
{
    CsrBtGnssClientPrim         type;           /* primitive/message identity */
    CsrBtConnId                 btConnId;       /* Connection id for the established connection. */
    CsrUint16                   payloadLength;  /* Length of payload. */
    CsrUint8                    *payload;       /* Payload. */
} CsrBtGnssClientDataReq;


/* Upstream messages */
typedef struct
{
    CsrBtGnssClientPrim         type;           /* primitive/message identity */
    CsrBtConnId                 btConnId;       /* Connection id for the established connection. */
    BD_ADDR_T                   bdAddr;         /* Bluetooth address of the device to connect. */
    CsrUint16                   maxFrameSize;   /* Maximum packet size supported by GNSS server */
    CsrBtResultCode             resultCode;     /* Result code
                                                 Possible results from GNSS client:
                                                 CSR_BT_RESULT_CODE_GNSS_CLIENT_SUCCESS
                                                 CSR_BT_RESULT_CODE_GNSS_CLIENT_ALREADY_CONNECTED
                                                 CSR_BT_RESULT_CODE_GNSS_CLIENT_UNSPECIFICED_ERROR
                                                 CSR_BT_RESULT_CODE_GNSS_CLIENT_UNEXPECTED_REQUEST
                                                 CSR_BT_RESULT_CODE_GNSS_CLIENT_CONNECTION_CANCELLED */
    CsrBtSupplier               resultSupplier;
} CsrBtGnssClientConnectCfm;

typedef struct
{
    CsrBtGnssClientPrim         type;           /* primitive/message identity */
    CsrBtConnId                 btConnId;       /* Connection id of the disconnected connection. */
    BD_ADDR_T                   bdAddr;         /* Bluetooth address of the remote device. */
    CsrBtResultCode             reasonCode;     /* Reason code
                                                 Possible reasons from GNSS client
                                                 CSR_BT_RESULT_CODE_GNSS_CLIENT_DISCONNECT_LOCAL
                                                 CSR_BT_RESULT_CODE_GNSS_CLIENT_DISCONNECT_REMOTE
                                                 CSR_BT_RESULT_CODE_GNSS_CLIENT_NO_CONNECTION */
    CsrBtSupplier               reasonSupplier;
} CsrBtGnssClientDisconnectInd;

typedef struct
{
    CsrBtGnssClientPrim         type;           /* primitive/message identity */
    CsrBtConnId                 btConnId;       /* Connection id for the established connection. */
    CsrBtResultCode             resultCode;     /* Result code
                                                 Possible results from GNSS client:
                                                 CSR_BT_RESULT_CODE_GNSS_CLIENT_SUCCESS
                                                 CSR_BT_RESULT_CODE_GNSS_CLIENT_NO_CONNECTION
                                                 CSR_BT_RESULT_CODE_GNSS_CLIENT_EXCESS_PAYLOAD */
    CsrBtSupplier               resultSupplier;
} CsrBtGnssClientDataCfm;

typedef struct
{
    CsrBtGnssClientPrim         type;           /* primitive/message identity */
    CsrBtConnId                 btConnId;       /* Connection id for the established connection. */
    CsrUint16                   payloadLength;  /* Length of payload. */
    CsrUint8                    *payload;       /* Payload. */
} CsrBtGnssClientDataInd;

#ifdef __cplusplus
}
#endif

#endif
