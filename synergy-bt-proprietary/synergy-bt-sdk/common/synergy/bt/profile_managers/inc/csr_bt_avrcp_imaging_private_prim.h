#ifndef CSR_BT_AVRCP_IMAGING_PRIVATE_PRIM_H__
#define CSR_BT_AVRCP_IMAGING_PRIVATE_PRIM_H__

/****************************************************************************

Copyright (c) 2015 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_synergy.h"
#include "csr_bt_avrcp_imaging_prim.h"
#include "csr_bt_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtAvrcpImagingPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

/**** Controller ****/

#define CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_DOWNSTREAM_PRIVATE_LOWEST  (0x0200)

#define CSR_BT_AVRCP_IMAGING_CLIENT_CONNECT_REQ                    ((CsrBtAvrcpImagingPrim) (0x0000 + CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_DOWNSTREAM_PRIVATE_LOWEST))
#define CSR_BT_AVRCP_IMAGING_CLIENT_DISCONNECT_REQ                 ((CsrBtAvrcpImagingPrim) (0x0001 + CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_DOWNSTREAM_PRIVATE_LOWEST))
#define CSR_BT_AVRCP_IMAGING_CLIENT_CANCEL_CONNECT_REQ             ((CsrBtAvrcpImagingPrim) (0x0002 + CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_DOWNSTREAM_PRIVATE_LOWEST))
#define CSR_BT_AVRCP_IMAGING_CLIENT_INSTANCE_CLEANUP               ((CsrBtAvrcpImagingPrim) (0x0003 + CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_DOWNSTREAM_PRIVATE_LOWEST))

#define CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_DOWNSTREAM_PRIVATE_HIGHEST (0x0003 + CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_DOWNSTREAM_PRIVATE_LOWEST)

#define CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_UPSTREAM_PRIVATE_LOWEST    (CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_DOWNSTREAM_PRIVATE_LOWEST + CSR_PRIM_UPSTREAM)

#define CSR_BT_AVRCP_IMAGING_CLIENT_CONNECT_CFM                    ((CsrBtAvrcpImagingPrim) (0x0000 + CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_UPSTREAM_PRIVATE_LOWEST))
#define CSR_BT_AVRCP_IMAGING_CLIENT_DISCONNECT_IND                 ((CsrBtAvrcpImagingPrim) (0x0001 + CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_UPSTREAM_PRIVATE_LOWEST))

#define CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_UPSTREAM_PRIVATE_HIGHEST   (0x0001 + CSR_BT_AVRCP_IMAGING_CLIENT_PRIM_UPSTREAM_PRIVATE_LOWEST)

/***********************************************************************************
 Private Primitive typedefs
************************************************************************************/
typedef struct
{
    CsrBtAvrcpImagingPrim type;
    CsrSchedQid           pHandle; /* control task handle */
    CsrUint8              connectionId; /* AVRCP Unique Connection identifier for a remote device */
    CsrBtDeviceAddr       deviceAddr; /* remote device address */
    CsrUint16             coverArtpsm; /* Cover Art Service OBEX PSM */
    dm_security_level_t   security;
} CsrBtAvrcpImagingClientConnectReq;

typedef struct
{
    CsrBtAvrcpImagingPrim type;
    CsrBtDeviceAddr       deviceAddr; /* remote device address */
} CsrBtAvrcpImagingClientCancelConnectReq;

typedef struct
{
    CsrBtAvrcpImagingPrim type;
    CsrUint8              connectionId; /* AVRCP Unique Connection identifier for a remote device */
    CsrBtConnId           obexBtConnId; /* OBEX Channel Bluetooth connection identifier */
    CsrBtResultCode       resultCode;
    CsrBtSupplier         resultSupplier;
} CsrBtAvrcpImagingClientConnectCfm;

typedef struct
{
    CsrBtAvrcpImagingPrim type;
    CsrUint8              connectionId;
    CsrBool               serviceOnly;
} CsrBtAvrcpImagingClientDisconnectReq;

typedef struct
{
    CsrBtAvrcpImagingPrim type;
    CsrUint8              connectionId; /* AVRCP Unique Connection identifier for a remote device */
    CsrBool               releaseTransport;
    /*CsrBool               initiator;*/ /* local side initiated or remote initiated */
    CsrBtReasonCode       resultCode;
    CsrBtSupplier         resultSupplier;
} CsrBtAvrcpImagingClientDisconnectInd;

typedef struct
{
    CsrBtAvrcpImagingPrim type;
    CsrUint8              connectionId;
} CsrBtAvrcpImagingClientInstanceCleanup;

/***********************************************************************************
 * End Private Primitive typedefs
 ***********************************************************************************/

/**** Target ****/

#define CSR_BT_AVRCP_IMAGING_SERVER_PRIM_DOWNSTREAM_PRIVATE_LOWEST  (0x0300)

#define CSR_BT_AVRCP_IMAGING_SERVER_ACTIVATE_REQ                    ((CsrBtAvrcpImagingPrim) (0x0000 + CSR_BT_AVRCP_IMAGING_SERVER_PRIM_DOWNSTREAM_PRIVATE_LOWEST))
#define CSR_BT_AVRCP_IMAGING_SERVER_ASSOCIATE_CONNECTION_REQ        ((CsrBtAvrcpImagingPrim) (0x0001 + CSR_BT_AVRCP_IMAGING_SERVER_PRIM_DOWNSTREAM_PRIVATE_LOWEST))
#define CSR_BT_AVRCP_IMAGING_SERVER_DEACTIVATE_REQ                  ((CsrBtAvrcpImagingPrim) (0x0002 + CSR_BT_AVRCP_IMAGING_SERVER_PRIM_DOWNSTREAM_PRIVATE_LOWEST))
#define CSR_BT_AVRCP_IMAGING_SERVER_MP_HANDLE_UPDATE_REQ            ((CsrBtAvrcpImagingPrim) (0x0003 + CSR_BT_AVRCP_IMAGING_SERVER_PRIM_DOWNSTREAM_PRIVATE_LOWEST))

#define CSR_BT_AVRCP_IMAGING_SERVER_PRIM_DOWNSTREAM_PRIVATE_HIGHEST (0x0003 + CSR_BT_AVRCP_IMAGING_SERVER_PRIM_DOWNSTREAM_PRIVATE_LOWEST)

#define CSR_BT_AVRCP_IMAGING_SERVER_PRIM_UPSTREAM_PRIVATE_LOWEST    (CSR_BT_AVRCP_IMAGING_SERVER_PRIM_DOWNSTREAM_PRIVATE_LOWEST + CSR_PRIM_UPSTREAM)

#define CSR_BT_AVRCP_IMAGING_SERVER_ACTIVATE_CFM                    ((CsrBtAvrcpImagingPrim) (0x0000 + CSR_BT_AVRCP_IMAGING_SERVER_PRIM_UPSTREAM_PRIVATE_LOWEST))
#define CSR_BT_AVRCP_IMAGING_SERVER_CONNECT_IND                     ((CsrBtAvrcpImagingPrim) (0x0001 + CSR_BT_AVRCP_IMAGING_SERVER_PRIM_UPSTREAM_PRIVATE_LOWEST))
#define CSR_BT_AVRCP_IMAGING_SERVER_ASSOCIATE_CONNECTION_CFM        ((CsrBtAvrcpImagingPrim) (0x0002 + CSR_BT_AVRCP_IMAGING_SERVER_PRIM_UPSTREAM_PRIVATE_LOWEST))
#define CSR_BT_AVRCP_IMAGING_SERVER_DEACTIVATE_CFM                  ((CsrBtAvrcpImagingPrim) (0x0003 + CSR_BT_AVRCP_IMAGING_SERVER_PRIM_UPSTREAM_PRIVATE_LOWEST))
#define CSR_BT_AVRCP_IMAGING_SERVER_DISCONNECT_IND                  ((CsrBtAvrcpImagingPrim) (0x0004 + CSR_BT_AVRCP_IMAGING_SERVER_PRIM_UPSTREAM_PRIVATE_LOWEST))

#define CSR_BT_AVRCP_IMAGING_SERVER_PRIM_UPSTREAM_PRIVATE_HIGHEST   (0x0004 + CSR_BT_AVRCP_IMAGING_SERVER_PRIM_UPSTREAM_PRIVATE_LOWEST)

/***********************************************************************************
 Private Primitive typedefs
************************************************************************************/
typedef struct
{
    CsrBtAvrcpImagingPrim type;
    CsrSchedQid           pHandle; /* control task handle */
    CsrUint16             psm; /* Cover Art Service OBEX PSM */
    CsrBtDeviceAddr       address;
    CsrUint8              connId; /* AVRCP Connection ID */
    dm_security_level_t   security;
    CsrSchedQid           mpHandle; /* Media player handle */
} CsrBtAvrcpImagingServerActivateReq;

typedef struct
{
    CsrBtAvrcpImagingPrim type;
    CsrUint8              connId; /* AVRCP Connection ID */
    CsrBtResultCode       resultCode;
    CsrBtSupplier         resultSupplier;
} CsrBtAvrcpImagingServerActivateCfm;

typedef struct
{
    CsrBtAvrcpImagingPrim type;
    CsrBtDeviceAddr       address;
    CsrUint8              connId; /* AVRCP Connection ID */
    CsrBtConnId           btConnId;
    CsrUint16             maxPeerObexPacketLength;
} CsrBtAvrcpImagingServerConnectInd;

typedef struct
{
    CsrBtAvrcpImagingPrim type;
    CsrBtDeviceAddr       address;
    CsrUint8              connId; /* AVRCP Connection ID */
    CsrSchedQid           mpHandle; /* Media player handle */
} CsrBtAvrcpImagingServerAssociateConnectionReq;

typedef struct
{
    CsrBtAvrcpImagingPrim type;
    CsrBtDeviceAddr       address;
    CsrBtResultCode       resultCode;
    CsrBtSupplier         resultSupplier;
} CsrBtAvrcpImagingServerAssociateConnectionCfm;

typedef struct
{
    CsrBtAvrcpImagingPrim type;
    CsrUint8              connId; /* AVRCP Connection ID */
    CsrSchedQid           mpHandle; /* Media player handle */
} CsrBtAvrcpImagingServerMpHandleUpdateReq;

typedef struct
{
    CsrBtAvrcpImagingPrim type;
    CsrUint16             psm; /* Cover Art Service OBEX PSM */
    CsrUint8              connId; /* AVRCP Connection ID */
} CsrBtAvrcpImagingServerDeactivateReq;

typedef struct
{
    CsrBtAvrcpImagingPrim type;
    CsrUint8              connId; /* AVRCP Connection ID */
    CsrBtResultCode       resultCode;
    CsrBtSupplier         resultSupplier;
} CsrBtAvrcpImagingServerDeactivateCfm;

typedef struct
{
    CsrBtAvrcpImagingPrim type;
    CsrUint8              connId; /* AVRCP Connection ID */
} CsrBtAvrcpImagingServerDisconnectInd;

/***********************************************************************************
 * End Private Primitive typedefs
 ***********************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_AVRCP_IMAGING_PRIVATE_PRIM_H__ */
