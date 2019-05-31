#ifndef CSR_BT_HIDH_PRIM_H__
#define CSR_BT_HIDH_PRIM_H__

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
#include "csr_bt_cm_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtHidhPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

/* ---------- Defines the HIDH CsrBtResultCode ----------*/
#define CSR_BT_RESULT_CODE_HIDH_SUCCESS                         ((CsrBtResultCode) (0x0000))
#define CSR_BT_RESULT_CODE_HIDH_NOT_READY                       ((CsrBtResultCode) (0x0001))
#define CSR_BT_RESULT_CODE_HIDH_INVALID_REPORT_ID               ((CsrBtResultCode) (0x0002))
#define CSR_BT_RESULT_CODE_HIDH_UNSUPPORTED                     ((CsrBtResultCode) (0x0003))
#define CSR_BT_RESULT_CODE_HIDH_INVALID_PARAMETER               ((CsrBtResultCode) (0x0004))
#define CSR_BT_RESULT_CODE_HIDH_UNKNOWN                         ((CsrBtResultCode) (0x0005))
#define CSR_BT_RESULT_CODE_HIDH_FATAL                           ((CsrBtResultCode) (0x0006))
#define CSR_BT_RESULT_CODE_HIDH_UNACCEPTABLE_PARAMETER          ((CsrBtResultCode) (0x0007))
#define CSR_BT_RESULT_CODE_HIDH_CONNECTION_ALREADY_EXISTS       ((CsrBtResultCode) (0x0008))
#define CSR_BT_RESULT_CODE_HIDH_UNKNOWN_DEVICE_IDENTIFIER       ((CsrBtResultCode) (0x0009))
#define CSR_BT_RESULT_CODE_HIDH_COMMAND_DISALLOWED              ((CsrBtResultCode) (0x000a))
#define CSR_BT_RESULT_CODE_HIDH_MAX_NUM_OF_CONNECTIONS          ((CsrBtResultCode) (0x000b))
#define CSR_BT_RESULT_CODE_HIDH_NO_CONNECTION_TO_DEVICE         ((CsrBtResultCode) (0x000c))
#define CSR_BT_RESULT_CODE_HIDH_CONNECTION_TERM_BY_REMOTE_HOST  ((CsrBtResultCode) (0x000d))
#define CSR_BT_RESULT_CODE_HIDH_SDC_SEARCH_FAILED               ((CsrBtResultCode) (0x0007))

/* extended HID lower interface function types */
typedef void (*extConnector_t)(CsrUint8 devid);
typedef void (*extDisconnector_t)(CsrUint8 devid);
typedef void (*extModeChange_t)(CsrUint8 devid, CsrUint8 mode);
typedef void (*extSendMsg_t)(CsrUint32 cid, CsrUint8 *dat, CsrUint16 len);

typedef CsrPrim                 CsrBtHidhPrim;

/* SDP flags */
#define CSR_BT_FLAGS_VIRTUAL_CABLE_BIT      ((CsrUint8) 0x01)
#define CSR_BT_FLAGS_RECONNECT_INIT_BIT     ((CsrUint8) 0x02)
#define CSR_BT_FLAGS_BOOT_DEVICE_BIT        ((CsrUint8) 0x04)
#define CSR_BT_FLAGS_SDP_DISABLE_BIT        ((CsrUint8) 0x08)
#define CSR_BT_FLAGS_BATTERY_POWER_BIT      ((CsrUint8) 0x10)
#define CSR_BT_FLAGS_REMOTE_WAKE_BIT        ((CsrUint8) 0x20)
#define CSR_BT_FLAGS_NORMALLY_CONNECT_BIT   ((CsrUint8) 0x40)

/* Special slotId used in HidhRegisterUserReq to set ALL profile instances */
#define CSR_BT_HID_ALL_DEVICE_SLOTS         ((CsrUint8) 0xFF)

typedef struct
{
    CsrBool                  isValid;
    CsrUint8                 flags;
    CsrUint16                vendorId;
    CsrUint16                productId;
    CsrUint16                deviceVersion;
    CsrUint8                 deviceSubclass;
    CsrUint8                 countryCode;
    CsrUint16                profileVersion;
    CsrUint16                parserVersion;
    CsrUint16                supervisionTimeout;
} CsrBtHidhSdpInfo;

typedef CsrUint8 CsrBtHidhTransaction;
#define CSR_BT_HIDH_HANDSHAKE               ((CsrBtHidhTransaction) 0x00)
#define CSR_BT_HIDH_CONTROL                 ((CsrBtHidhTransaction) 0x10)
#define CSR_BT_HIDH_GET_REPORT              ((CsrBtHidhTransaction) 0x40)
#define CSR_BT_HIDH_SET_REPORT              ((CsrBtHidhTransaction) 0x50)
#define CSR_BT_HIDH_GET_PROTOCOL            ((CsrBtHidhTransaction) 0x60)
#define CSR_BT_HIDH_SET_PROTOCOL            ((CsrBtHidhTransaction) 0x70)
#define CSR_BT_HIDH_GET_IDLE                ((CsrBtHidhTransaction) 0x80)
#define CSR_BT_HIDH_SET_IDLE                ((CsrBtHidhTransaction) 0x90)
#define CSR_BT_HIDH_DATA                    ((CsrBtHidhTransaction) 0xA0)
#define CSR_BT_HIDH_DATC                    ((CsrBtHidhTransaction) 0xB0)

typedef CsrUint8 CsrBtHidhCtrl;
#define CSR_BT_HIDH_NOP                     ((CsrBtHidhCtrl) 0x00)
#define CSR_BT_HIDH_HARD_RESET              ((CsrBtHidhCtrl) 0x01)
#define CSR_BT_HIDH_SOFT_RESET              ((CsrBtHidhCtrl) 0x02)
#define CSR_BT_HIDH_SUSPEND                 ((CsrBtHidhCtrl) 0x03)
#define CSR_BT_HIDH_EXIT_SUSPEND            ((CsrBtHidhCtrl) 0x04)
#define CSR_BT_HIDH_VC_UNPLUG               ((CsrBtHidhCtrl) 0x05)

typedef CsrUint8 CsrBtHidhReport;
#define CSR_BT_HIDH_OTHER                   ((CsrBtHidhReport) 0x00)
#define CSR_BT_HIDH_INPUT_REPORT            ((CsrBtHidhReport) 0x01)
#define CSR_BT_HIDH_OUTPUT_REPORT           ((CsrBtHidhReport) 0x02)
#define CSR_BT_HIDH_FEATURE_REPORT          ((CsrBtHidhReport) 0x03)

typedef CsrUint8 CsrBtHidhProtocol;
#define CSR_BT_HIDH_BOOT_PROTOCOL           ((CsrBtHidhProtocol) 0x00)
#define CSR_BT_HIDH_REPORT_PROTOCOL         ((CsrBtHidhProtocol) 0x01)

typedef CsrUint8 CsrBtHidhStatus;
#define CSR_BT_HIDH_STATUS_DISCONNECTED     ((CsrBtHidhStatus) 0x01)
#define CSR_BT_HIDH_STATUS_CONNECTED        ((CsrBtHidhStatus) 0x02)
#define CSR_BT_HIDH_STATUS_RECONNECTING     ((CsrBtHidhStatus) 0x03)

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_HIDH_PRIM_DOWNSTREAM_LOWEST                       (0x0000)

#define CSR_BT_HIDH_CONNECT_REQ                   ((CsrBtHidhPrim) (0x0000 + CSR_BT_HIDH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HIDH_CONNECT_ACCEPT_REQ            ((CsrBtHidhPrim) (0x0001 + CSR_BT_HIDH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HIDH_CANCEL_CONNECT_ACCEPT_REQ     ((CsrBtHidhPrim) (0x0002 + CSR_BT_HIDH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HIDH_DISCONNECT_REQ                ((CsrBtHidhPrim) (0x0003 + CSR_BT_HIDH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HIDH_CONTROL_REQ                   ((CsrBtHidhPrim) (0x0004 + CSR_BT_HIDH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HIDH_GET_REPORT_REQ                ((CsrBtHidhPrim) (0x0005 + CSR_BT_HIDH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HIDH_SET_REPORT_REQ                ((CsrBtHidhPrim) (0x0006 + CSR_BT_HIDH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HIDH_GET_PROTOCOL_REQ              ((CsrBtHidhPrim) (0x0007 + CSR_BT_HIDH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HIDH_SET_PROTOCOL_REQ              ((CsrBtHidhPrim) (0x0008 + CSR_BT_HIDH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HIDH_GET_IDLE_REQ                  ((CsrBtHidhPrim) (0x0009 + CSR_BT_HIDH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HIDH_SET_IDLE_REQ                  ((CsrBtHidhPrim) (0x000A + CSR_BT_HIDH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HIDH_DATA_REQ                      ((CsrBtHidhPrim) (0x000B + CSR_BT_HIDH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HIDH_REGISTER_USER_REQ             ((CsrBtHidhPrim) (0x000C + CSR_BT_HIDH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HIDH_SECURITY_IN_REQ               ((CsrBtHidhPrim) (0x000E + CSR_BT_HIDH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HIDH_SECURITY_OUT_REQ              ((CsrBtHidhPrim) (0x000F + CSR_BT_HIDH_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_HIDH_PRIM_DOWNSTREAM_HIGHEST                      (0x000F + CSR_BT_HIDH_PRIM_DOWNSTREAM_LOWEST)

/****************************************************************************************/

#define CSR_BT_HIDH_PRIM_UPSTREAM_LOWEST                         (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_HIDH_CANCEL_CONNECT_ACCEPT_CFM     ((CsrBtHidhPrim) (0x0000 + CSR_BT_HIDH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HIDH_CONNECT_ACCEPT_IND            ((CsrBtHidhPrim) (0x0001 + CSR_BT_HIDH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HIDH_DISCONNECT_IND                ((CsrBtHidhPrim) (0x0002 + CSR_BT_HIDH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HIDH_STATUS_IND                    ((CsrBtHidhPrim) (0x0003 + CSR_BT_HIDH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HIDH_HANDSHAKE_IND                 ((CsrBtHidhPrim) (0x0004 + CSR_BT_HIDH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HIDH_CONTROL_IND                   ((CsrBtHidhPrim) (0x0005 + CSR_BT_HIDH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HIDH_DATA_IND                      ((CsrBtHidhPrim) (0x0006 + CSR_BT_HIDH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HIDH_CONNECT_CFM                   ((CsrBtHidhPrim) (0x0007 + CSR_BT_HIDH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HIDH_CONNECT_ACCEPT_CFM            ((CsrBtHidhPrim) (0x0008 + CSR_BT_HIDH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HIDH_DISCONNECT_CFM                ((CsrBtHidhPrim) (0x0009 + CSR_BT_HIDH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HIDH_REGISTER_USER_CFM             ((CsrBtHidhPrim) (0x000A + CSR_BT_HIDH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HIDH_SECURITY_IN_CFM               ((CsrBtHidhPrim) (0x000B + CSR_BT_HIDH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HIDH_SECURITY_OUT_CFM              ((CsrBtHidhPrim) (0x000C + CSR_BT_HIDH_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_HIDH_PRIM_UPSTREAM_HIGHEST                        (0x000C + CSR_BT_HIDH_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_HIDH_PRIM_DOWNSTREAM_COUNT         (CSR_BT_HIDH_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_HIDH_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_HIDH_PRIM_UPSTREAM_COUNT           (CSR_BT_HIDH_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_HIDH_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

/*============================================================================*
  typedefs
  *============================================================================*/
typedef struct
{
    CsrBtHidhPrim           type;               /* Primitive/message identity */
    CsrUint8               slotId;             /* Slot id to register */
    CsrSchedQid                   userHandle;         /* User application handle */
} CsrBtHidhRegisterUserReq;

typedef struct
{
    CsrBtHidhPrim         type;               /* Primitive/message identity */
    CsrBtResultCode       resultCode;
    CsrBtSupplier   resultSupplier;
    CsrUint8               slotId;             /* Registered slot id */
} CsrBtHidhRegisterUserCfm;

typedef struct
{
    CsrBtHidhPrim           type;               /* Primitive/message identity */
    CsrSchedQid             ctrlHandle;         /* control application handle */
    CsrUint8               slotId;             /* Use this slotId */
    CsrBtDeviceAddr          deviceAddr;         /* address of device to connect to */
    CsrUint16              flushTimeout;       /* flush timeout value */
    L2CA_QOS_T                 *qosCtrl;           /* ref. to QoS structure (control ch.) */
    CsrUint8               qosCtrlCount;       /* number of qosIntr structures */
    L2CA_QOS_T                 *qosIntr;           /* ref. to QoS structure (interrupt ch.) */
    CsrUint8               qosIntrCount;       /* number of qosCtrl structures */
    CsrBtHidhSdpInfo        *sdpInfo;           /* reference to device's SDP information */
    CsrUint8               sdpInfoCount;       /* number of sdpInfo structures */
    CsrCharString              *serviceName;       /* SDP service name */
    CsrUint16              descriptorLength;   /* SDP descriptor table length */
    CsrUint8               *descriptor;        /* SDP descriptor */
} CsrBtHidhConnectReq;

typedef struct
{
    CsrBtHidhPrim           type;
    CsrBtResultCode       resultCode;
    CsrBtSupplier   resultSupplier;
    CsrBtDeviceAddr          deviceAddr;         /* address of device connected to */
    CsrUint8               deviceId;           /* identity of device (profile instance) */
    CsrBtHidhSdpInfo        *sdpInfo;           /* reference to device's SDP information */
    CsrUint8               sdpInfoCount;       /* number of sdpInfo structures */
    CsrCharString              *serviceName;       /* SDP service name */
    CsrUint16              descriptorLength;   /* SDP descriptor table length */
    CsrUint8               *descriptor;        /* SDP descriptor */
    CsrBtConnId             btConnId;               /* Global Bluetooth connection ID */
} CsrBtHidhConnectCfm;

typedef struct
{
    CsrBtHidhPrim           type;
    CsrSchedQid             ctrlHandle;         /* control application handle */
    CsrUint8               slotId;             /* Use this slotId */
    CsrBtDeviceAddr          deviceAddr;         /* address of device to connect to */
    CsrUint16              flushTimeout;       /* flush timeout value */
    L2CA_QOS_T                 *qosIntr;           /* ref. to QoS structure (control ch.) */
    CsrUint8               qosIntrCount;       /* number of qosIntr structures */
    L2CA_QOS_T                 *qosCtrl;           /* ref. to QoS structure (interrupt ch.) */
    CsrUint8               qosCtrlCount;       /* number of qosIntr structures */
    CsrBtHidhSdpInfo        *sdpInfo;           /* reference to device's SDP information */
    CsrUint8               sdpInfoCount;       /* number of sdpInfo structures */
    CsrCharString              *serviceName;       /* SDP service name */
    CsrUint16              descriptorLength;   /* SDP descriptor table length */
    CsrUint8               *descriptor;        /* SDP descriptor */
} CsrBtHidhConnectAcceptReq;

typedef struct
{
    CsrBtHidhPrim           type;
    CsrBtResultCode       resultCode;
    CsrBtSupplier         resultSupplier;
    CsrBtDeviceAddr          deviceAddr;         /* address of device connected to */
    CsrUint8               deviceId;           /* identity of device(profile instance) */
    CsrBtConnId             btConnId;               /* Global Bluetooth connection ID */
} CsrBtHidhConnectAcceptCfm;

typedef struct
{
    CsrBtHidhPrim           type;
    CsrUint8               deviceId;           /* identity of device(profile instance) */
} CsrBtHidhCancelConnectAcceptReq;

typedef struct
{
    CsrBtHidhPrim           type;
    CsrBtResultCode       resultCode;
    CsrBtSupplier   resultSupplier;
    CsrUint8               deviceId;           /* identity of device(profile instance) */
} CsrBtHidhCancelConnectAcceptCfm;

typedef struct
{
    CsrBtHidhPrim           type;
    CsrBtResultCode       resultCode;
    CsrBtSupplier   resultSupplier;
    CsrUint8               deviceId;           /* identity of device(profile instance) */
} CsrBtHidhConnectAcceptInd;

typedef struct
{
    CsrBtHidhPrim           type;
    CsrUint8               deviceId;           /* identity of device(profile instance) */
} CsrBtHidhDisconnectReq;

typedef struct
{
    CsrBtHidhPrim           type;
    CsrBtResultCode       resultCode;
    CsrBtSupplier   resultSupplier;
    CsrUint8               deviceId;           /* identity of device(profile instance) */
} CsrBtHidhDisconnectCfm;

typedef struct
{
    CsrBtHidhPrim         type;
    CsrBtResultCode       reasonCode;
    CsrBtSupplier   reasonSupplier;
    CsrUint8               deviceId;           /* identity of device(profile instance) */
} CsrBtHidhDisconnectInd;

typedef struct
{
    CsrBtHidhPrim           type;
    CsrUint8               deviceId;           /* identity of device(profile instance) */
    CsrBtHidhCtrl           operation;          /* control operation requested */
} CsrBtHidhControlReq;

typedef CsrBtHidhControlReq CsrBtHidhControlInd;

typedef struct
{
    CsrBtHidhPrim           type;
    CsrBtResultCode       resultCode;
    CsrUint8               deviceId;           /* identity of device(profile instance) */
} CsrBtHidhHandshakeInd;

typedef struct
{
    CsrBtHidhPrim           type;
    CsrUint8               deviceId;           /* identity of device(profile instance) */
    CsrBtHidhReport         reportType;         /* the type of report requested */
    CsrUint8               reportId;           /* optional report id (if CSR_UNUSED = 0) */
    CsrUint16              bufferSize;         /* optional buffer size (if CSR_UNUSED = 0) */
} CsrBtHidhGetReportReq;

typedef struct
{
    CsrBtHidhPrim           type;
    CsrUint8               deviceId;           /* identity of device(profile instance) */
    CsrBtHidhReport         reportType;         /* the type of report requested */
    CsrUint16              reportLen;          /* report length incl. 1 byte trans. hdr */
    CsrUint8               *report;            /* reference to set packet(incl. hdr byte) */
} CsrBtHidhSetReportReq;

typedef struct
{
    CsrBtHidhPrim           type;
    CsrUint8               deviceId;           /* identity of device(profile instance) */
} CsrBtHidhGetProtocolReq;

typedef struct
{
    CsrBtHidhPrim           type;
    CsrUint8               deviceId;           /* identity of device(profile instance) */
    CsrBtHidhProtocol       protocol;           /* the type of protocol */
} CsrBtHidhSetProtocolReq;

typedef struct
{
    CsrBtHidhPrim           type;
    CsrUint8               deviceId;           /* identity of device(profile instance) */
} CsrBtHidhGetIdleReq;

typedef struct
{
    CsrBtHidhPrim           type;
    CsrUint8               deviceId;           /* identity of device(profile instance) */
    CsrUint8               idleRate;           /* idle rate, 0=infinite, 1-255[4ms-1.020s] */
} CsrBtHidhSetIdleReq;

typedef struct
{
    CsrBtHidhPrim           type;
    CsrUint8               deviceId;           /* identity of device(profile instance) */
    CsrBtHidhReport         reportType;         /* the type of report requested */
    CsrUint16              dataLen;            /* data length incl. 1 byte transaction hdr */
    CsrUint8               *data;              /* reference to data packet(incl. hdr byte) */
} CsrBtHidhDataReq;

typedef CsrBtHidhDataReq CsrBtHidhDataInd;

typedef struct
{
    CsrBtHidhPrim           type;
    CsrUint8               deviceId;           /* identity of device(profile instance) */
    CsrBtHidhStatus         status;             /* connection status */
    CsrBtHidhSdpInfo        *sdpInfo;           /* reference to device's SDP information */
    CsrUint8               sdpInfoCount;       /* number of sdpInfo structures */
    CsrCharString              *serviceName;       /* SDP service name */
    CsrUint16              descriptorLength;   /* SDP descriptor table length */
    CsrUint8               *descriptor;        /* SDP descriptor */
} CsrBtHidhStatusInd;

typedef struct
{
    CsrBtHidhPrim           type;
    CsrSchedQid             appHandle;
    CsrUint16              secLevel;
} CsrBtHidhSecurityInReq;

typedef struct
{
    CsrBtHidhPrim           type;
    CsrSchedQid             appHandle;
    CsrUint16              secLevel;
} CsrBtHidhSecurityOutReq;

typedef struct
{
    CsrBtHidhPrim           type;
    CsrBtResultCode       resultCode;
    CsrBtSupplier   resultSupplier;
} CsrBtHidhSecurityInCfm;

typedef struct
{
    CsrBtHidhPrim           type;
    CsrBtResultCode       resultCode;
    CsrBtSupplier   resultSupplier;
} CsrBtHidhSecurityOutCfm;

#ifdef __cplusplus
}
#endif

#endif
