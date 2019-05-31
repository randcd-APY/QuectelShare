#ifndef CSR_BT_MCAP_PRIVATE_PRIM_H__
#define CSR_BT_MCAP_PRIVATE_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_result.h"
#include "csr_bt_profiles.h"
#include "l2cap_prim.h"
#include "csr_bt_cm_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MCAP_CONTROL_CHANNEL_MAX_RETRANS            0xFF
#define MCAP_CONTROL_CHANNEL_RETRANS_TIMEOUT        0x012C
#define MCAP_CONTROL_CHANNEL_MONITOR_TIMEOUT        0x012C

/* ---------- Defines the MCAP CsrBtResultCode ----------*/
#define CSR_BT_RESULT_CODE_MCAP_SUCCESS                             ((CsrBtResultCode) (0x0000))
#define CSR_BT_RESULT_CODE_MCAP_UNACCEPTABLE_PARAMETER              ((CsrBtResultCode) (0x0081))
#define CSR_BT_RESULT_CODE_MCAP_INVALID_PARAMETER_VALUE             ((CsrBtResultCode) (0x0082))
#define CSR_BT_RESULT_CODE_MCAP_INVALID_OPERATION                   ((CsrBtResultCode) (0x0083))
#define CSR_BT_RESULT_CODE_MCAP_UNSPECIFIED_ERROR                   ((CsrBtResultCode) (0x0084))
#define CSR_BT_RESULT_CODE_MCAP_CONNECT_ATTEMPT_FAILED              ((CsrBtResultCode) (0x0085))
#define CSR_BT_RESULT_CODE_MCAP_CANCELLED_CONNECT_ATTEMPT           ((CsrBtResultCode) (0x0086))

/* ---------- Error codes: ----------*/
#define CSR_BT_RESULT_CODE_MCAP_PROTOCOL_SUCCESS                    ((CsrBtResultCode) (0x0000))
#define CSR_BT_RESULT_CODE_MCAP_PROTOCOL_INVALID_OPCODE             ((CsrBtResultCode) (0x0001))
#define CSR_BT_RESULT_CODE_MCAP_PROTOCOL_INVALID_PARAMETER_VALUE    ((CsrBtResultCode) (0x0002))
#define CSR_BT_RESULT_CODE_MCAP_PROTOCOL_INVALID_MDEP               ((CsrBtResultCode) (0x0003))
#define CSR_BT_RESULT_CODE_MCAP_PROTOCOL_MDEP_BUSY                  ((CsrBtResultCode) (0x0004))
#define CSR_BT_RESULT_CODE_MCAP_PROTOCOL_INVALID_MDL                ((CsrBtResultCode) (0x0005))
#define CSR_BT_RESULT_CODE_MCAP_PROTOCOL_MDL_BUSY                   ((CsrBtResultCode) (0x0006))
#define CSR_BT_RESULT_CODE_MCAP_PROTOCOL_INVALID_OPERATION          ((CsrBtResultCode) (0x0007))
#define CSR_BT_RESULT_CODE_MCAP_PROTOCOL_RESOURCE_UNAVAILABLE       ((CsrBtResultCode) (0x0008))
#define CSR_BT_RESULT_CODE_MCAP_PROTOCOL_UNSPECIFIED_ERROR          ((CsrBtResultCode) (0x0009))
#define CSR_BT_RESULT_CODE_MCAP_PROTOCOL_REQUEST_NOT_SUPPORTED      ((CsrBtResultCode) (0x000A))
#define CSR_BT_RESULT_CODE_MCAP_PROTOCOL_CONFIGURATION_REJECTED     ((CsrBtResultCode) (0x000B))


/* search_string="McapPrimType" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim McapPrimType;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_MCAP_PRIM_DOWNSTREAM_LOWEST                             (0x0000)

#define CSR_BT_MCAP_ACTIVATE_REQ                       ((McapPrimType) (0x0000 + CSR_BT_MCAP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MCAP_DEACTIVATE_REQ                     ((McapPrimType) (0x0001 + CSR_BT_MCAP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MCAP_CONNECT_MCL_REQ                    ((McapPrimType) (0x0002 + CSR_BT_MCAP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MCAP_DISCONNECT_MCL_REQ                 ((McapPrimType) (0x0003 + CSR_BT_MCAP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MCAP_CREATE_MDL_RES                     ((McapPrimType) (0x0004 + CSR_BT_MCAP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MCAP_RECONNECT_MDL_REQ                  ((McapPrimType) (0x0005 + CSR_BT_MCAP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MCAP_RECONNECT_MDL_RES                  ((McapPrimType) (0x0006 + CSR_BT_MCAP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MCAP_CONNECT_MDL_REQ                    ((McapPrimType) (0x0007 + CSR_BT_MCAP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MCAP_DISCONNECT_MDL_REQ                 ((McapPrimType) (0x0008 + CSR_BT_MCAP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MCAP_ABORT_MDL_RES                      ((McapPrimType) (0x0009 + CSR_BT_MCAP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MCAP_DELETE_MDL_REQ                     ((McapPrimType) (0x000A + CSR_BT_MCAP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MCAP_DELETE_MDL_RES                     ((McapPrimType) (0x000B + CSR_BT_MCAP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MCAP_DATA_SEND_REQ                      ((McapPrimType) (0x000C + CSR_BT_MCAP_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_MCAP_CTRL_PRIM_DOWNSTREAM_HIGHEST                       (0x000C + CSR_BT_MCAP_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_MCAP_SYNC_CAP_REQ                       ((McapPrimType) (0x000D + CSR_BT_MCAP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MCAP_SYNC_CAP_RES                       ((McapPrimType) (0x000E + CSR_BT_MCAP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MCAP_SYNC_SET_REQ                       ((McapPrimType) (0x000F + CSR_BT_MCAP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MCAP_SYNC_SET_RES                       ((McapPrimType) (0x0010 + CSR_BT_MCAP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_MCAP_SYNC_INFO_REQ                      ((McapPrimType) (0x0011 + CSR_BT_MCAP_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_MCAP_PRIM_DOWNSTREAM_HIGHEST                            (0x0011 + CSR_BT_MCAP_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST                               (0x0100 + CSR_PRIM_UPSTREAM)

#define CSR_BT_MCAP_ACTIVATE_CFM                       ((McapPrimType) (0x0000 + CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MCAP_DEACTIVATE_CFM                     ((McapPrimType) (0x0001 + CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MCAP_CONNECT_MCL_IND                    ((McapPrimType) (0x0002 + CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MCAP_CONNECT_MCL_CFM                    ((McapPrimType) (0x0003 + CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MCAP_DISCONNECT_MCL_IND                 ((McapPrimType) (0x0004 + CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MCAP_DISCONNECT_MCL_CFM                 ((McapPrimType) (0x0005 + CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MCAP_CREATE_MDL_IND                     ((McapPrimType) (0x0006 + CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MCAP_RECONNECT_MDL_IND                  ((McapPrimType) (0x0007 + CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MCAP_RECONNECT_MDL_CFM                  ((McapPrimType) (0x0008 + CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MCAP_CONNECT_MDL_IND                    ((McapPrimType) (0x0009 + CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MCAP_CONNECT_MDL_CFM                    ((McapPrimType) (0x000A + CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MCAP_DISCONNECT_MDL_IND                 ((McapPrimType) (0x000B + CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MCAP_DISCONNECT_MDL_CFM                 ((McapPrimType) (0x000C + CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MCAP_ABORT_MDL_IND                      ((McapPrimType) (0x000D + CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MCAP_DELETE_MDL_IND                     ((McapPrimType) (0x000E + CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MCAP_DELETE_MDL_CFM                     ((McapPrimType) (0x000F + CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MCAP_DATA_SEND_IND                      ((McapPrimType) (0x0010 + CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MCAP_DATA_SEND_CFM                      ((McapPrimType) (0x0011 + CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_MCAP_CTRL_PRIM_UPSTREAM_HIGHEST                         (0x0011 + CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_MCAP_SYNC_CAP_IND                       ((McapPrimType) (0x0012 + CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MCAP_SYNC_CAP_CFM                       ((McapPrimType) (0x0013 + CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MCAP_SYNC_SET_IND                       ((McapPrimType) (0x0014 + CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MCAP_SYNC_SET_CFM                       ((McapPrimType) (0x0015 + CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_MCAP_SYNC_INFO_IND                      ((McapPrimType) (0x0016 + CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_MCAP_PRIM_UPSTREAM_HIGHEST                              (0x0016 + CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_MCAP_PRIM_DOWNSTREAM_COUNT              (CSR_BT_MCAP_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_MCAP_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_MCAP_PRIM_UPSTREAM_COUNT                (CSR_BT_MCAP_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_MCAP_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

#define CSR_BT_MCAP_PRIVATE_PRIM (CSR_BT_MCAP_PRIM)


typedef CsrUint8 McapResponseType;

typedef struct
{
    McapPrimType            type;
    psm_t                   psm;
    CsrSchedQid             phandle;
    CsrUint24               classOfDevice;
    dm_security_level_t     secLevel;
    l2ca_mtu_t              mtu;
    CsrTime                 sniffTimeout;
    uuid16_t                uuid;
    CsrUint8                noOfConnections;
} CsrBtMcapActivateReq;

typedef struct
{
    McapPrimType            type;
    psm_t                   psm;
    CsrSchedQid             phandle;
} CsrBtMcapDeactivateReq;

typedef struct
{
    McapPrimType            type;
    CsrBtDeviceAddr         bd_addr;
    psm_t                   localPsm;
    psm_t                   remotePsm;
    CsrSchedQid             phandle;
    dm_security_level_t     secLevel;
    CsrTime                 sniffTimeout;
} CsrBtMcapConnectMclReq;

typedef struct
{
    McapPrimType            type;
    CsrBtDeviceAddr         bd_addr;
    psm_t                   psm;
    CsrSchedQid             phandle;
} CsrBtMcapDisconnectMclReq;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint16               mdlId;
    McapResponseType        mcapResponse;
    CsrUint8                configResponse;
    psm_t                   psm;
    dm_security_level_t     secLevel;
    l2ca_mtu_t              mtu;
    L2CA_QOS_T              *qos;       /* Reserved for future use. Always set to NULL */
    CsrUint8                qosCount;   /* Reserved for future use. Always set to 0 */
    L2CA_FLOW_T             *flow;
    CsrUint8                flowCount;  /* Reserved for future use. Always set to 1 */
} CsrBtMcapCreateMdlRes;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint16               mdlId;
    psm_t                   localPsm;
    psm_t                   remotePsm;
} CsrBtMcapReconnectMdlReq;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint16               mdlId;
    McapResponseType        mcapResponse;
    psm_t                   psm;
    dm_security_level_t     secLevel;
    l2ca_mtu_t              mtu;
    L2CA_QOS_T              *qos;       /* Reserved for future use. Always set to NULL */
    CsrUint8                qosCount;   /* Reserved for future use. Always set to 0 */
    L2CA_FLOW_T             *flow;
    CsrUint8                flowCount;  /* Reserved for future use. Always set to 1 */
} CsrBtMcapReconnectMdlRes;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint16               mdlId;
    CsrUint8                mdepId;
    CsrUint8                configuration;
    psm_t                   localPsm;
    psm_t                   remotePsm;
    dm_security_level_t     secLevel;
    l2ca_mtu_t              mtu;
    L2CA_QOS_T              *qos;       /* Reserved for future use. Always set to NULL */
    CsrUint8                qosCount;   /* Reserved for future use. Always set to 0 */
    L2CA_FLOW_T             *flow;
    CsrUint8                flowCount;  /* Reserved for future use. Always set to 1 */
} CsrBtMcapConnectMdlReq;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint16               mdlId;
} CsrBtMcapDisconnectMdlReq;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint16               mdlId;
    McapResponseType        mcapResponse;
} CsrBtMcapAbortMdlRes;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint16               mdlId;
} CsrBtMcapDeleteMdlReq;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint16               mdlId;
    McapResponseType        mcapResponse;
} CsrBtMcapDeleteMdlRes;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint16               mdlId;
    CsrUint16               length;
    void                    *payload;
} CsrBtMcapDataSendReq;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint16               timeStampRequiredAccuracy;
} CsrBtMcapSyncCapReq;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    McapResponseType        mcapResponse;
    CsrUint8                btClkAccessRes;
    CsrUint16               syncLeadTime;
    CsrUint16               timeStampNativeRes;
    CsrUint16               timeStampNativeAccuracy;
} CsrBtMcapSyncCapRes;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint8                timeStampUpdateInfo;
    CsrUint32               btClkSyncTime;
    CsrUint32               timeStampSyncTimeLsb;
    CsrUint32               timeStampSyncTimeMsb;
} CsrBtMcapSyncSetReq;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    McapResponseType        mcapResponse;
    CsrUint32               btClkSyncTime;
    CsrUint32               timeStampSyncTimeLsb;
    CsrUint32               timeStampSyncTimeMsb;
    CsrUint16               timeStampSampleAccuracy;
} CsrBtMcapSyncSetRes;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint32               btClkSyncTime;
    CsrUint32               timeStampSyncTimeLsb;
    CsrUint32               timeStampSyncTimeMsb;
    CsrUint16               timeStampSampleAccuracy;
} CsrBtMcapSyncInfoReq;



/*************************************************/
/*                                               */
/*  Upstream                                     */
/*                                               */
/*************************************************/

typedef struct
{
    McapPrimType            type;
    psm_t                   psm;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtMcapActivateCfm;

typedef struct
{
    McapPrimType            type;
    psm_t                   psm;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtMcapDeactivateCfm;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrBtDeviceAddr         bd_addr;
    psm_t                   localPsm;
    psm_t                   remotePsm; /*This remote PSM is not valid and it is always 0x0000*/
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtMcapConnectMclInd;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrBtDeviceAddr         bd_addr;
    psm_t                   psm;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtMcapConnectMclCfm;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrBtDeviceAddr         bd_addr;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtMcapDisconnectMclInd;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrBtDeviceAddr         bd_addr;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtMcapDisconnectMclCfm;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint16               mdlId;
    CsrUint8                mdepId;
    CsrUint8                configuration;
} CsrBtMcapCreateMdlInd;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint16               mdlId;
    psm_t                   localPsm;
    psm_t                   remotePsm; /*This remote PSM is not valid and it is always 0x0000*/
    l2ca_mtu_t              localMtu;
    l2ca_mtu_t              remoteMtu;
    CsrBtDeviceAddr         bd_addr;
} CsrBtMcapReconnectMdlInd;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint16               mdlId;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtMcapReconnectMdlCfm;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint16               mdlId;
    psm_t                   localPsm;
    psm_t                   remotePsm; /*This remote PSM is not valid and it is always 0x0000*/
    l2ca_mtu_t              localMtu;
    l2ca_mtu_t              remoteMtu;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtMcapConnectMdlInd;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint16               mdlId;
    psm_t                   localPsm;
    psm_t                   remotePsm; /*This remote PSM is not valid and it is always 0x0000*/
    l2ca_mtu_t              localMtu;
    l2ca_mtu_t              remoteMtu;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtMcapConnectMdlCfm;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint16               mdlId;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtMcapDisconnectMdlInd;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint16               mdlId;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtMcapDisconnectMdlCfm;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint16               mdlId;
} CsrBtMcapAbortMdlInd;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint16               mdlId;
} CsrBtMcapDeleteMdlInd;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint16               mdlId;
    McapResponseType        mcapResponse;
} CsrBtMcapDeleteMdlCfm;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint16               mdlId;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtMcapDataSendCfm;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint16               mdlId;
    CsrUint16               length;
    void                    *payload;
} CsrBtMcapDataSendInd;

typedef struct
{
    McapPrimType            type;
    CsrUint16                mclId;
    CsrUint16                timeStampRequiredAccuracy;
} CsrBtMcapSyncCapInd;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    McapResponseType        mcapResponse;
    CsrUint8                btClkAccessRes;
    CsrUint16               syncLeadTime;
    CsrUint16               timeStampNativeRes;
    CsrUint16               timeStampNativeAccuracy;
} CsrBtMcapSyncCapCfm;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint8                timeStampUpdateInfo;
    CsrUint32               btClkSyncTime;
    CsrUint32               timeStampSyncTimeLsb;
    CsrUint32               timeStampSyncTimeMsb;
} CsrBtMcapSyncSetInd;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    McapResponseType        mcapResponse;
    CsrUint32               btClkSyncTime;
    CsrUint32               timeStampSyncTimeLsb;
    CsrUint32               timeStampSyncTimeMsb;
    CsrUint16               timeStampSampleAccuracy;
} CsrBtMcapSyncSetCfm;

typedef struct
{
    McapPrimType            type;
    CsrUint16               mclId;
    CsrUint32               btClkSyncTime;
    CsrUint32               timeStampSyncTimeLsb;
    CsrUint32               timeStampSyncTimeMsb;
    CsrUint16               timeStampSampleAccuracy;
} CsrBtMcapSyncInfoInd;

void CsrBtMcapPrivateFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);

#ifdef __cplusplus
}
#endif

#endif
