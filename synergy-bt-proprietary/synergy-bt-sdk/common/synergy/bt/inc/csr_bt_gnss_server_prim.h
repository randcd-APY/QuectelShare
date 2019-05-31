#ifndef CSR_BT_GNSS_SERVER_PRIM_H__
#define CSR_BT_GNSS_SERVER_PRIM_H__

/****************************************************************************

Copyright (c) 2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_result.h"
#include "csr_bt_profiles.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtGnssServerPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */


typedef CsrPrim CsrBtGnssServerPrim;

/* ---------- Defines the GNSS Server CsrBtResultCode ----------*/
#define CSR_BT_RESULT_CODE_GNSS_SERVER_SUCCESS                          ((CsrBtResultCode) (0x0))
#define CSR_BT_RESULT_CODE_GNSS_SERVER_EXCESS_PAYLOAD                   ((CsrBtResultCode) (0x1))
#define CSR_BT_RESULT_CODE_GNSS_SERVER_UNACCEPTABLE_PARAMETERS          ((CsrBtResultCode) (0x2))
#define CSR_BT_RESULT_CODE_GNSS_SERVER_DISCONNECT_LOCAL                 ((CsrBtResultCode) (0x3))
#define CSR_BT_RESULT_CODE_GNSS_SERVER_DISCONNECT_REMOTE                ((CsrBtResultCode) (0x4))
#define CSR_BT_RESULT_CODE_GNSS_SERVER_MAX_INSTANCE                     ((CsrBtResultCode) (0x5))
#define CSR_BT_RESULT_CODE_GNSS_SERVER_NO_CONNECTION                    ((CsrBtResultCode) (0x6))

#define GNSS_SERVER_INVALID_INSTANCE_ID                 CSR_BT_CM_CONTEXT_UNUSED

/* GNSS server low power mode policy */
#define CSR_BT_GNSS_SERVER_LPM_AUTO                     (0xFF)
#define CSR_BT_GNSS_SERVER_LPM_SNIFF                    (CSR_BT_SNIFF_MODE)
#define CSR_BT_GNSS_SERVER_LPM_ACTIVE                   (CSR_BT_ACTIVE_MODE)


/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_GNSS_SERVER_PRIM_DOWNSTREAM_LOWEST       (0x0000)

#define CSR_BT_GNSS_SERVER_ACTIVATE_REQ                 ((CsrBtGnssServerPrim) (0x0000 + CSR_BT_GNSS_SERVER_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GNSS_SERVER_DEACTIVATE_REQ               ((CsrBtGnssServerPrim) (0x0001 + CSR_BT_GNSS_SERVER_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GNSS_SERVER_DISCONNECT_REQ               ((CsrBtGnssServerPrim) (0x0002 + CSR_BT_GNSS_SERVER_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GNSS_SERVER_DATA_REQ                     ((CsrBtGnssServerPrim) (0x0003 + CSR_BT_GNSS_SERVER_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_GNSS_SERVER_LPM_REQ                      ((CsrBtGnssServerPrim) (0x0004 + CSR_BT_GNSS_SERVER_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_GNSS_SERVER_PRIM_DOWNSTREAM_HIGHEST      (0x0004 + CSR_BT_GNSS_SERVER_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_GNSS_SERVER_PRIM_UPSTREAM_LOWEST         (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_GNSS_SERVER_ACTIVATE_CFM                 ((CsrBtGnssServerPrim) (0x0000 + CSR_BT_GNSS_SERVER_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GNSS_SERVER_DEACTIVATE_CFM               ((CsrBtGnssServerPrim) (0x0001 + CSR_BT_GNSS_SERVER_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GNSS_SERVER_CONNECT_IND                  ((CsrBtGnssServerPrim) (0x0002 + CSR_BT_GNSS_SERVER_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GNSS_SERVER_DATA_CFM                     ((CsrBtGnssServerPrim) (0x0003 + CSR_BT_GNSS_SERVER_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GNSS_SERVER_DATA_IND                     ((CsrBtGnssServerPrim) (0x0004 + CSR_BT_GNSS_SERVER_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GNSS_SERVER_DISCONNECT_IND               ((CsrBtGnssServerPrim) (0x0005 + CSR_BT_GNSS_SERVER_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_GNSS_SERVER_MODE_CHANGE_IND              ((CsrBtGnssServerPrim) (0x0006 + CSR_BT_GNSS_SERVER_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_GNSS_SERVER_PRIM_UPSTREAM_HIGHEST        (0x0006 + CSR_BT_GNSS_SERVER_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_GNSS_SERVER_PRIM_DOWNSTREAM_COUNT        (CSR_BT_GNSS_SERVER_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_GNSS_SERVER_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_GNSS_SERVER_PRIM_UPSTREAM_COUNT          (CSR_BT_GNSS_SERVER_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_GNSS_SERVER_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/


/*******************************************************************************
 * Downstream messages
 ******************************************************************************/
/**
 * Activation request
 * It creates new independent GNSS server instance.
 * GNSS server would immediately reply back with server-instance-id and success
 * result in activation confirm primitive.
 * Application should use server-instance-id for all further interactions.
 */
typedef struct
{
    CsrBtGnssServerPrim                 type;                       /* Primitive type */
    CsrSchedQid                         phandle;                    /* Application handler (QID) */
    CsrUint16                           secLevel;                   /* Desired incoming security level */
} CsrBtGnssServerActivateReq;

/**
 * Deactivate request
 * Application can request for deactivation of GNSS server instance any time.
 * GNSS server would take care of state (disconnection, data transmission, etc) of server instance.
 * Any data encountered after deactivation request is received would be discarded.
 */
typedef struct
{
    CsrBtGnssServerPrim                 type;                       /* Primitive type */
    CsrUint16                           instanceId;                 /* Server instance id */
} CsrBtGnssServerDeactivateReq;

/**
 * Disconnect request
 * It will be processed only if server instance is connected otherwise would be ignored.
 */
typedef struct
{
    CsrBtGnssServerPrim                 type;                       /* Primitive type */
    CsrUint16                           instanceId;                 /* Server instance id */
    CsrBtConnId                         connId;                     /* Connection id */
} CsrBtGnssServerDisconnectReq;

/**
 * Data request
 * It will be processed only if server instance is connected otherwise server would reply with "no connection" in data confirm primitive.
 */
typedef struct
{
    CsrBtGnssServerPrim                 type;                       /* Primitive type */
    CsrUint16                           instanceId;                 /* Server instance id */
    CsrBtConnId                         connId;                     /* Connection id */
    CsrUint16                           payloadLength;              /* Payload length */
    CsrUint8                            *payload;                   /* Data payload */
} CsrBtGnssServerDataReq;

/**
 * Low power mode request
 * Auto mode - Server would take control of power mode and application would not be notified of mode change.
 * Server would vote for sniff mode and would temporarily move to active mode for activeToSniffTime while sending data.
 *
 * Manual mode (active and sniff) - Server would stay requested mode. Low power mode interface would be transparent to application.
 * All mode change events would be forwarded to application.
 */
typedef struct
{
    CsrBtGnssServerPrim                 type;                       /* Primitive type */
    CsrUint16                           instanceId;                 /* Server instance id */
    CsrUint8                            mode;                       /* 0 - active, 1 - sniff, 0xff - auto */
    CsrUint16                           activeToSniffTime;          /* Time to stay in active power mode if auto mode is enabled */
} CsrBtGnssServerLpmReq;


/*******************************************************************************
 * Upstream messages
 ******************************************************************************/
/**
 * Activation confirm
 */
typedef struct
{
    CsrBtGnssServerPrim                 type;                       /* Primitive type */
    CsrUint16                           instanceId;                 /* Server instance id */
    CsrBtResultCode                     resultCode;                 /* Result code
                                                                     Possible results from GNSS server:
                                                                     CSR_BT_RESULT_CODE_GNSS_SERVER_SUCCESS
                                                                     CSR_BT_RESULT_CODE_GNSS_SERVER_MAX_INSTANCE */
    CsrBtSupplier                       resultSupplier;             /* Result supplier */
} CsrBtGnssServerActivateCfm;

/**
 * Deactivation confirm
 */
typedef struct
{
    CsrBtGnssServerPrim                 type;                       /* Primitive type */
    CsrUint16                           instanceId;                 /* Server instance id */
    CsrBtResultCode                     resultCode;                 /* Result code
                                                                     Possible results from GNSS server:
                                                                     CSR_BT_RESULT_CODE_GNSS_SERVER_SUCCESS */
    CsrBtSupplier                       resultSupplier;             /* Result supplier */
} CsrBtGnssServerDeactivateCfm;

/**
 * Connection indication
 */
typedef struct
{
    CsrBtGnssServerPrim                 type;                       /* Primitive type */
    CsrUint16                           instanceId;                 /* Server instance id */
    CsrBtConnId                         connId;                     /* Connection id */
    CsrBtDeviceAddr                     deviceAddr;                 /* Peer device address */
    CsrUint16                           maxFrameSize;               /* Maximum packet size supported by GNSS client */
    CsrBtResultCode                     resultCode;                 /* Result code
                                                                     Possible results from GNSS server:
                                                                     CSR_BT_RESULT_CODE_GNSS_SERVER_SUCCESS */
    CsrBtSupplier                       resultSupplier;             /* Result supplier */
} CsrBtGnssServerConnectInd;

/**
 * Disconnection indication
 */
typedef struct
{
    CsrBtGnssServerPrim                 type;                       /* Primitive type */
    CsrUint16                           instanceId;                 /* Server instance id */
    CsrBtConnId                         connId;                     /* Connection id */
    CsrBtResultCode                     reasonCode;                 /* Result code
                                                                     Possible reasons from GNSS server:
                                                                     CSR_BT_RESULT_CODE_GNSS_SERVER_DISCONNECT_LOCAL
                                                                     CSR_BT_RESULT_CODE_GNSS_SERVER_DISCONNECT_REMOTE
                                                                     CSR_BT_RESULT_CODE_GNSS_SERVER_NO_CONNECTION */
    CsrBtSupplier                       reasonSupplier;             /* Result supplier */
} CsrBtGnssServerDisconnectInd;

/**
 * Data confirm
 */
typedef struct
{
    CsrBtGnssServerPrim                 type;                       /* Primitive type */
    CsrUint16                           instanceId;                 /* Server instance id */
    CsrBtConnId                         connId;                     /* Connection id */
    CsrBtResultCode                     resultCode;                 /* Result code
                                                                     Possible results from GNSS server:
                                                                     CSR_BT_RESULT_CODE_GNSS_SERVER_SUCCESS
                                                                     CSR_BT_RESULT_CODE_GNSS_SERVER_NO_CONNECTION
                                                                     CSR_BT_RESULT_CODE_GNSS_SERVER_EXCESS_PAYLOAD */
    CsrBtSupplier                       resultSupplier;             /* Result supplier */
} CsrBtGnssServerDataCfm;

/**
 * Data indication
 * This is an optional primitive and can be disabled by enabling cmake cache
 * parameter EXCLUDE_CSR_BT_GNSS_SERVER_MODULE_OPTIONAL.
 */
typedef struct
{
    CsrBtGnssServerPrim                 type;                       /* Primitive type */
    CsrUint16                           instanceId;                 /* Server instance id */
    CsrBtConnId                         connId;                     /* Connection id */
    CsrUint16                           payloadLength;              /* Data length */
    CsrUint8                            *payload;                   /* Data pointer */
} CsrBtGnssServerDataInd;

/**
 * Low power mode change indication
 * Sent on change of low power mode.
 * Applicable only if GNSS server instance is in manual mode.
 */
typedef struct
{
    CsrBtGnssServerPrim                 type;                       /* Primitive type */
    CsrUint16                           instanceId;                 /* Server instance id */
    CsrBtConnId                         connId;                     /* Connection id */
    CsrUint8                            linkMode;                   /* Current low power mode */
} CsrBtGnssServerModeChangeInd;

#ifdef __cplusplus
}
#endif

#endif
