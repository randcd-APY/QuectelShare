#ifndef CSR_BT_HDP_PRIM_H__
#define CSR_BT_HDP_PRIM_H__

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

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtHdpPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

/* ---------- Defines the HDP CsrBtResultCode ----------*/
#define CSR_BT_RESULT_CODE_HDP_SUCCESS                          ((CsrBtResultCode) (0x0000))
#define CSR_BT_RESULT_CODE_HDP_NOT_REGISTERED                   ((CsrBtResultCode) (0x0001))
#define CSR_BT_RESULT_CODE_HDP_PEER_DEVICE_CAPAB_UNKNOWN        ((CsrBtResultCode) (0x0002))
#define CSR_BT_RESULT_CODE_HDP_MDEP_BUSY                        ((CsrBtResultCode) (0x0004))
#define CSR_BT_RESULT_CODE_HDP_INVALID_MDL                      ((CsrBtResultCode) (0x0005))
#define CSR_BT_RESULT_CODE_HDP_MDL_BUSY                         ((CsrBtResultCode) (0x0006))
#define CSR_BT_RESULT_CODE_HDP_INVALID_OP                       ((CsrBtResultCode) (0x0007))
#define CSR_BT_RESULT_CODE_HDP_RESOURCE_UNAVAILABLE             ((CsrBtResultCode) (0x0008))
#define CSR_BT_RESULT_CODE_HDP_UNSPECIFIED_ERROR                ((CsrBtResultCode) (0x0009))
#define CSR_BT_RESULT_CODE_HDP_CONNECT_ASSOCIATE_CHANNEL_FAIL   ((CsrBtResultCode) (0x000A))
#define CSR_BT_RESULT_CODE_HDP_INVALID_CONFIG                   ((CsrBtResultCode) (0x000B))
#define CSR_BT_RESULT_CODE_HDP_CONNECT_VIRTUAL_CHANNEL_FAIL     ((CsrBtResultCode) (0x000C))
#define CSR_BT_RESULT_CODE_HDP_ALREADY_REGISTERED               ((CsrBtResultCode) (0x000D))
#define CSR_BT_RESULT_CODE_HDP_ALREADY_ACTIVATED                ((CsrBtResultCode) (0x000E))
#define CSR_BT_RESULT_CODE_HDP_MAX_INSTANCES                    ((CsrBtResultCode) (0x000F))
#define CSR_BT_RESULT_CODE_HDP_UNACCEPTABLE_PARAMETER           ((CsrBtResultCode) (0x0011))
#define CSR_BT_RESULT_CODE_HDP_SEARCH_ACTIVE                    ((CsrBtResultCode) (0x0010))

#define CSR_BT_SNIFF_TIMEOUT_MCL                    (0)
#define CSR_BT_HDP_INSTANCE_MDL_MASK                (0x0000FFFF)
#define CSR_BT_HDP_MAX_PACKET_LEN                   672

/*----------- MCAP Config values ------*/
#define CSR_BT_HDP_CONFIG_UNKNOWN                   (0)
#define CSR_BT_HDP_CONFIG_RELIABLE_CHANNEL          (1)
#define CSR_BT_HDP_CONFIG_STREAMING_CHANNEL         (2)


/*----------- HDP Role---------*/
#define CSR_BT_HDP_SOURCE_ROLE      0x0001 << 0
#define CSR_BT_HDP_SINK_ROLE        0x0001 << 1
#define CSR_BT_HDP_ROLE_ANY         0x0001 << 2
#define CSR_BT_HDP_ROLE_INVALID     0x0000

#define CSR_BT_HDP_MDEP_SOURCE      0x00
#define CSR_BT_HDP_MDEP_SINK        0x01
#define CSR_BT_HDP_ECHO_TEST_MDEP   0x00


/*---------- MCAP Supported Features---*/
#define CSR_BT_HDP_SF_NONE                       (0x0000)
#define CSR_BT_HDP_SUPPORT_RECONNECT_INITIATION  (0x02)
#define CSR_BT_HDP_SUPPORT_RECONNECT_ACCEPTANCE  (0x04)
#define CSR_BT_HDP_SUPPORT_CSP                   (0x08)
#define CSR_BT_HDP_SUPPORT_SYNC_MASTER_ROLE      (0x10)

/*---------- Data Exchange Protocol----*/
#define CSR_BT_HDP_DATA_EXC_PROTO_INVALID   0x00
#define CSR_BT_HDP_DATA_EXC_PROTO_IEEE      0x01

/*---------- Device Bit Map------------*/
#define CSR_BT_DEVICE_BLOOD_PRESSURE        0x01
#define CSR_BT_DEVICE_WEIGHT_SCALE          0x02
#define CSR_BT_DEVICE_PEDOMETER             0x04
#define CSR_BT_DEVICE_BODY_COMPOSITION      0x08
#define CSR_BT_DEVICE_BODY_TEMP             0x10

#define CSR_BT_TOTAL_DEVICE_TYPES_SUPPORTED 5

/*----------- Services Notify-----*/
typedef CsrUint8  CsrBtMdepId;
typedef CsrUint8  CsrBtMdepRole;
typedef CsrUint16 CsrBtMdepDataType;


/* ---------- MPED DataTypes----*/

#define CSR_BT_HDP_DEVICE_TYPE_OXIM             4100 /* 0x1004 */
#define CSR_BT_HDP_DEVICE_TYPE_BP               4103 /* 0x1007 */
#define CSR_BT_HDP_DEVICE_TYPE_TEMP             4104 /* 0x1008 */
#define CSR_BT_HDP_DEVICE_TYPE_SCALE            4111 /* 0x100F */
#define CSR_BT_HDP_DEVICE_TYPE_GLUCOSE          4113 /* 0x1011 */
#define CSR_BT_HDP_DEVICE_TYPE_HF_CARDIO        4137 /* 0x1029 */
#define CSR_BT_HDP_DEVICE_TYPE_HF_STRENGTH      4138 /* 0x102A */
#define CSR_BT_HDP_DEVICE_TYPE_ACTIVITY_HUB     4167 /* 0x1047 */
#define CSR_BT_HDP_DEVICE_TYPE_AI_MED_MINDER    4168 /* 0x1048 */
#define CSR_BT_HDP_DEVICE_TYPE_ECHO_TEST        CSR_BT_HDP_DEVICE_TYPE_BP

typedef struct
{
    CsrBtMdepId         id;
    CsrBtMdepDataType   dataType;
    CsrBtMdepRole       role;
    CsrBtDeviceName     description;
} CsrBtHdpMdep;

typedef struct
{
    uuid16_t            role;               /* Set to HDP_SOURCE or HDP_SINK*/
    CsrUint8            dataExchangeSpecification; /*Supported Data Exchange Protocol*/
    CsrUint8            mcapFeatures;       /* Bit mask of MCAP Features*/
    CsrCharString       *serviceDescription; /*NULL-terminated string */
    CsrCharString       *providerName;      /* NULL-terminated string */
    CsrCharString       *serviceName;       /* NULL-terminated string */
    CsrUint8            supportedFeatureListLen; /*Number of Supported Features List*/
    CsrBtHdpMdep        *supportedFeatureList;
} CsrBtHdpSdpRecord;


typedef CsrPrim CsrBtHdpPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_HDP_PRIM_DOWNSTREAM_LOWEST                                 (0x0000)

#define CSR_BT_HDP_REGISTER_REQ                           ((CsrBtHdpPrim) (0x0000 + CSR_BT_HDP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HDP_REGISTER_RSP                           ((CsrBtHdpPrim) (0x0001 + CSR_BT_HDP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HDP_CTRL_GET_CAPAB_REQ                     ((CsrBtHdpPrim) (0x0002 + CSR_BT_HDP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HDP_ACTIVATE_REQ                           ((CsrBtHdpPrim) (0x0003 + CSR_BT_HDP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HDP_CONNECT_ASSOCIATE_CHANNEL_REQ          ((CsrBtHdpPrim) (0x0004 + CSR_BT_HDP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HDP_ACCEPT_CONNECT_ASSOCIATE_CHANNEL_RES   ((CsrBtHdpPrim) (0x0005 + CSR_BT_HDP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HDP_CONNECT_VIRTUAL_CHANNEL_REQ            ((CsrBtHdpPrim) (0x0006 + CSR_BT_HDP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HDP_ACCEPT_CONNECT_VIRTUAL_CHANNEL_RES     ((CsrBtHdpPrim) (0x0007 + CSR_BT_HDP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HDP_DATA_REQ                               ((CsrBtHdpPrim) (0x0008 + CSR_BT_HDP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HDP_DISCONNECT_ASSOCIATE_CHANNEL_REQ       ((CsrBtHdpPrim) (0x0009 + CSR_BT_HDP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HDP_DISCONNECT_VIRTUAL_CHANNEL_REQ         ((CsrBtHdpPrim) (0x000A + CSR_BT_HDP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HDP_SUSPEND_REQ                            ((CsrBtHdpPrim) (0x000B + CSR_BT_HDP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HDP_RESUME_REQ                             ((CsrBtHdpPrim) (0x000C + CSR_BT_HDP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HDP_RESUME_RSP                             ((CsrBtHdpPrim) (0x000D + CSR_BT_HDP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HDP_UNREGISTER_REQ                         ((CsrBtHdpPrim) (0x000E + CSR_BT_HDP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HDP_DEACTIVATE_REQ                         ((CsrBtHdpPrim) (0x000F + CSR_BT_HDP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HDP_SECURITY_IN_REQ                        ((CsrBtHdpPrim) (0x0010 + CSR_BT_HDP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HDP_SECURITY_OUT_REQ                       ((CsrBtHdpPrim) (0x0011 + CSR_BT_HDP_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_HDP_PRIM_DOWNSTREAM_HIGHEST                                (0x0011 + CSR_BT_HDP_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_HDP_PRIM_UPSTREAM_LOWEST                                   (0x0100 + CSR_PRIM_UPSTREAM)

#define CSR_BT_HDP_REGISTER_CFM                           ((CsrBtHdpPrim) (0x0000 + CSR_BT_HDP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HDP_REGISTER_IND                           ((CsrBtHdpPrim) (0x0001 + CSR_BT_HDP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HDP_CTRL_GET_CAPAB_CFM                     ((CsrBtHdpPrim) (0x0002 + CSR_BT_HDP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HDP_CTRL_GET_CAPAB_IND                     ((CsrBtHdpPrim) (0x0003 + CSR_BT_HDP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HDP_ACTIVATE_CFM                           ((CsrBtHdpPrim) (0x0004 + CSR_BT_HDP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HDP_CONNECT_ASSOCIATE_CHANNEL_CFM          ((CsrBtHdpPrim) (0x0005 + CSR_BT_HDP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HDP_ACCEPT_CONNECT_ASSOCIATE_CHANNEL_IND   ((CsrBtHdpPrim) (0x0006 + CSR_BT_HDP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HDP_CONNECT_ASSOCIATE_CHANNEL_IND          ((CsrBtHdpPrim) (0x0007 + CSR_BT_HDP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HDP_CONNECT_VIRTUAL_CHANNEL_CFM            ((CsrBtHdpPrim) (0x0008 + CSR_BT_HDP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HDP_ACCEPT_CONNECT_VIRTUAL_CHANNEL_IND     ((CsrBtHdpPrim) (0x0009 + CSR_BT_HDP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HDP_CONNECT_VIRTUAL_CHANNEL_IND            ((CsrBtHdpPrim) (0x000A + CSR_BT_HDP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HDP_DATA_IND                               ((CsrBtHdpPrim) (0x000B + CSR_BT_HDP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HDP_DISCONNECT_ASSOCIATE_CHANNEL_CFM       ((CsrBtHdpPrim) (0x000C + CSR_BT_HDP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HDP_DISCONNECT_ASSOCIATE_CHANNEL_IND       ((CsrBtHdpPrim) (0x000D + CSR_BT_HDP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HDP_DISCONNECT_VIRTUAL_CHANNEL_CFM         ((CsrBtHdpPrim) (0x000E + CSR_BT_HDP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HDP_DISCONNECT_VIRTUAL_CHANNEL_IND         ((CsrBtHdpPrim) (0x000F + CSR_BT_HDP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HDP_DATA_CFM                               ((CsrBtHdpPrim) (0x0010 + CSR_BT_HDP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HDP_SUSPEND_CFM                            ((CsrBtHdpPrim) (0x0011 + CSR_BT_HDP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HDP_RESUME_CFM                             ((CsrBtHdpPrim) (0x0012 + CSR_BT_HDP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HDP_RESUME_IND                             ((CsrBtHdpPrim) (0x0013 + CSR_BT_HDP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HDP_UNREGISTER_CFM                         ((CsrBtHdpPrim) (0x0014 + CSR_BT_HDP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HDP_DEACTIVATE_CFM                         ((CsrBtHdpPrim) (0x0015 + CSR_BT_HDP_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_HDP_PRIM_UPSTREAM_HIGHEST                                  (0x0015 + CSR_BT_HDP_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_HDP_PRIM_DOWNSTREAM_COUNT                  (CSR_BT_HDP_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_HDP_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_HDP_PRIM_UPSTREAM_COUNT                    (CSR_BT_HDP_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_HDP_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct /* CsrBtHdpRegisterReq */
{
    CsrBtHdpPrim        type;
    CsrSchedQid         appHandle;       /**< handle of the ctrl application */
    dm_security_level_t secLevel;            /**< security level of the device being activated */
    CsrCharString       *serviceName;        /**< name of the service ...upto 200 characters */
    CsrCharString       *serviceDescription; /**< brief description of the service ...
                                                   upto 200 characters */
    CsrCharString       *providerName;        /**< organsiation providing the service */
    CsrUint8            dataExchangeSpecification; /**< byte mask - Data Exchange Protocol */
    CsrTime             sniffTimeOut;        /**< Time (in ms) the MCL must be idle before requesting sniff */
    CsrUint8            numOfMdep;           /**< number of End Points */
    CsrUint8            supportedProcedures; /* byte mask - MCAP procedures supported by HDP */

}CsrBtHdpRegisterReq;


typedef struct /* CsrBtHdpRegisterRsp */
{
    CsrBtHdpPrim        type;
    CsrSchedQid         appHandle;       /**< handle of the ctrl application */
    CsrBtMdepDataType   datatype;
    CsrBtMdepRole       role;
    CsrUtf8String       *description;               /**< mdep information */
    CsrBool             reuseMdepId;         /**< offer device service on the same end-point*/
} CsrBtHdpRegisterRsp;


typedef struct
{
    CsrBtHdpPrim        type;
    CsrSchedQid         appHandle;
    CsrUint8            noOfConnections;
} CsrBtHdpActivateReq;


typedef struct
{
    CsrBtHdpPrim        type;
    CsrSchedQid         appHandle;
    CsrUint32           hdpInstanceId;
    CsrBtDeviceAddr     deviceAddr;
    CsrBtMdepId         mdepId;
    CsrUint16           mdepDataType;
    CsrUint8            mdepRole;
    CsrUint16           maxPacketLength;
} CsrBtHdpConnectAssociateChannelReq;


typedef struct
{
    CsrBtHdpPrim        type;
    CsrSchedQid         appHandle;
    CsrUint8            responseCode;
    l2ca_mtu_t          mtu;
    CsrUint32           assocChannelId;
}CsrBtHdpAcceptConnectAssociateChannelRes;


typedef struct
{
    CsrBtHdpPrim        type;
    CsrSchedQid         appHandle;
    CsrBtDeviceAddr     deviceAddr;
    CsrUint8            mdepDataTypeMask;            /* 0xFF searches for any datatype */
    CsrUint32           mdepDataTypeConditionMask;   /* Not yet implemented this fucntionality*/
    CsrUint8            mdepRoleMask;
}CsrBtHdpCtrlGetCapabReq;


typedef struct
{
    CsrBtHdpPrim        type;
    CsrSchedQid         appHandle;
    CsrUint32           assocChannelId;
    CsrUint8            virtualChannelConfig;
    CsrUint16           maxPacketLength;
}CsrBtHdpConnectVirtualChannelReq;



typedef struct
{
    CsrBtHdpPrim        type;
    CsrSchedQid         appHandle;
    CsrUint32           virtualChannelId;
    CsrUint16           maxPacketLength;
    CsrUint8            virtualChannelConfig;
    CsrUint8            responseCode;
}CsrBtHdpAcceptConnectVirtualChannelRes;


typedef struct
{
    CsrBtHdpPrim        type;
    CsrSchedQid         appHandle;
}CsrBtHdpUnregisterReq;


typedef struct
{
    CsrBtHdpPrim        type;
    CsrSchedQid         appHandle;
}CsrBtHdpDeactivateReq;


typedef struct
{
    CsrBtHdpPrim        type;
    CsrSchedQid         appHandle;
    CsrUint32           assocChId;
}CsrBtHdpDisconnectAssociateChannelReq;


typedef struct
{
    CsrBtHdpPrim        type;
    CsrSchedQid         appHandle;
    CsrUint32           virtualChId;
}CsrBtHdpDisconnectVirtualChannelReq;


typedef struct
{
    CsrBtHdpPrim        type;
    CsrSchedQid         appHandle;
    CsrUint32           chId;
    CsrUint16           dataLen;
    CsrUint8            *data;
}CsrBtHdpDataReq;

typedef struct
{
    CsrBtHdpPrim        type;
    CsrSchedQid         appHandle;
    CsrUint32           assocChId;
}CsrBtHdpSuspendReq;


typedef struct
{
    CsrBtHdpPrim        type;
    CsrSchedQid         appHandle;
    CsrUint32           assocChId;
}CsrBtHdpResumeReq;

typedef struct
{
    CsrBtHdpPrim        type;
    CsrSchedQid         appHandle;
    CsrUint32           chId;
    l2ca_mtu_t          mtu;
    CsrBool             resume;
}CsrBtHdpResumeRsp;

typedef struct
{
    CsrBtHdpPrim        type;
    CsrSchedQid         appHandle;
    CsrUint16           secLevel;
} CsrBtHdpSecurityInReq;

typedef struct
{
    CsrBtHdpPrim        type;
    CsrSchedQid         appHandle;
    CsrUint16           secLevel;
} CsrBtHdpSecurityOutReq;


/*************************************************/
/*                                               */
/*  Upstream                                     */
/*                                               */
/*************************************************/

typedef struct
{
    CsrBtHdpPrim            type;
    CsrBtMdepId             mdepId;              /**< mdep-id */
} CsrBtHdpRegisterInd;


typedef struct
{
    CsrBtHdpPrim            type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtHdpRegisterCfm;


typedef struct
{
    CsrBtHdpPrim            type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
} CsrBtHdpActivateCfm;


typedef struct
{
    CsrBtHdpPrim            type;
    CsrUint32               assocChannelId;
    CsrUint32               virtualChannelId;
    CsrUint16               maxPacketLength;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
    CsrBtConnId             btConnId;               /* Global Bluetooth connection ID */
}CsrBtHdpConnectVirtualChannelCfm;


typedef struct
{
    CsrBtHdpPrim            type;
    CsrUint32               assocChannelId;
    CsrUint32               virtualChannelId;
    CsrUint8                virtualChannelConfig;
    CsrBtConnId             btConnId;               /* Global Bluetooth connection ID */
}CsrBtHdpAcceptConnectVirtualChannelInd;


typedef struct
{
    CsrBtHdpPrim            type;
    CsrUint32               assocChannelId;
    CsrUint32               virtualChannelId;
    CsrUint16               maxPacketLength;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
    CsrBtConnId             btConnId;               /* Global Bluetooth connection ID */
}CsrBtHdpConnectVirtualChannelInd;


typedef struct
{
    CsrBtHdpPrim            type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
}CsrBtHdpDeactivateCfm;


typedef struct
{
    CsrBtHdpPrim            type;
    CsrUint32               hdpInstanceId;
    CsrBtDeviceAddr         deviceAddr;
    CsrBtMdepId             mdepId;
    CsrBtMdepDataType       dataType;
    CsrUint32               assocChannelId;
    CsrUint16               maxPacketLength;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
    CsrBtConnId             btConnId;               /* Global Bluetooth connection ID */
} CsrBtHdpConnectAssociateChannelCfm;


typedef struct
{
    CsrBtHdpPrim            type;
    CsrUint32               hdpInstanceId;
    CsrBtDeviceAddr         deviceAddr;
    CsrBtMdepId             mdepId;
    CsrBtMdepDataType       dataType;
    CsrUint32               assocChannelId;
    CsrUint16               maxPacketLength;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
    CsrBtConnId             btConnId;               /* Global Bluetooth connection ID */
} CsrBtHdpConnectAssociateChannelInd;


typedef struct
{
    CsrBtHdpPrim            type;
    CsrBtDeviceAddr         deviceAddr;
    CsrBtMdepId             mdepId;
    CsrUint32               assocChannelId;
    CsrBtConnId             btConnId;               /* Global Bluetooth connection ID */
}CsrBtHdpAcceptConnectAssociateChannelInd;


typedef struct
{
   CsrBtHdpPrim             type;
   CsrBtDeviceAddr          deviceAddr;
   CsrUint32                hdpInstanceId;
   CsrCharString            *serviceName;
   CsrCharString            *serviceDescrip;
   CsrCharString            *providerName;
   CsrUint8                 dataexchangeSpecification;
   CsrUint8                 supportedProcedures;
   CsrUint8                 supportedFeatureListLen;
   CsrBtHdpMdep             *supportedFeatureList;
}CsrBtHdpCtrlGetCapabInd;


typedef struct
{
    CsrBtHdpPrim             type;
    CsrBtResultCode          resultCode;
    CsrBtSupplier            resultSupplier;
    CsrBtDeviceAddr          deviceAddr;
}CsrBtHdpCtrlGetCapabCfm;


typedef struct
{
    CsrBtHdpPrim            type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
}CsrBtHdpUnregisterCfm;


typedef struct
{
    CsrBtHdpPrim            type;
    CsrUint32               assocChId;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
}CsrBtHdpDisconnectAssociateChannelInd;


typedef struct
{
    CsrBtHdpPrim            type;
    CsrUint32               assocChId;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
}CsrBtHdpDisconnectAssociateChannelCfm;



typedef struct
{
    CsrBtHdpPrim            type;
    CsrUint32               virtualChId;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
}CsrBtHdpDisconnectVirtualChannelCfm;


typedef struct
{
    CsrBtHdpPrim            type;
    CsrUint32               virtualChId;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
}CsrBtHdpDisconnectVirtualChannelInd;

typedef struct
{
    CsrBtHdpPrim            type;
    CsrUint32               assocChId;
    CsrUint16               length;
    CsrUint8                *data;
}CsrBtHdpDataInd;

typedef struct
{
    CsrBtHdpPrim            type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
    CsrUint32               assocChId;
}CsrBtHdpDataCfm;


typedef struct
{
    CsrBtHdpPrim            type;
    CsrUint32               assocChId;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
}CsrBtHdpSuspendCfm;

typedef struct
{
    CsrBtHdpPrim            type;
    CsrBtDeviceAddr         deviceAddr;
    CsrUint32               assocChId;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
}CsrBtHdpResumeCfm;


typedef struct
{
    CsrBtHdpPrim            type;
    CsrBtDeviceAddr         deviceAddr;
    CsrUint32               chId;
    CsrBtResultCode         resultCode;
    CsrBtSupplier           resultSupplier;
}CsrBtHdpResumeInd;


#ifdef __cplusplus
}
#endif

#endif
