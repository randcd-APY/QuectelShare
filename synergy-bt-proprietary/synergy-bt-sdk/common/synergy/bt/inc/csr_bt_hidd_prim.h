#ifndef CSR_BT_HIDD_PRIM_H__
#define CSR_BT_HIDD_PRIM_H__

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

/* search_string="CsrBtHiddPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

/* ---------- Defines the HIDD CsrBtResultCode ----------*/
#define CSR_BT_RESULT_CODE_HIDD_SUCCESS                         ((CsrBtResultCode) (0x0000))
#define CSR_BT_RESULT_CODE_HIDD_UNACCEPTABLE_PARAMETER          ((CsrBtResultCode) (0x0001))
#define CSR_BT_RESULT_CODE_HIDD_SDS_REGISTER_FAILED             ((CsrBtResultCode) (0x0002))
#define CSR_BT_RESULT_CODE_HIDD_SDS_UNREGISTER_FAILED           ((CsrBtResultCode) (0x0003))
#define CSR_BT_RESULT_CODE_HIDD_TIMEOUT                         ((CsrBtResultCode) (0x0004))
#define CSR_BT_RESULT_CODE_HIDD_CONNECTION_TERM_BY_REMOTE_HOST  ((CsrBtResultCode) (0x0005))
#define CSR_BT_RESULT_CODE_HIDD_CONNECTION_TERM_BY_LOCAL_HOST   ((CsrBtResultCode) (0x0006))

/* ---------- General HIDD typedefs: ---------- */
typedef CsrPrim CsrBtHiddPrim;

typedef CsrUint8 CsrBtHiddTransactionType;

typedef CsrUint8 CsrBtHiddParameterType;
typedef CsrBtHiddParameterType CsrBtHiddHandshakeType;
typedef CsrBtHiddParameterType CsrBtHiddCtrlType;
typedef CsrBtHiddParameterType CsrBtHiddReportType;
typedef CsrBtHiddParameterType CsrBtHiddProtocolType;

typedef CsrUint16 CsrBtHiddPowerModeType;

typedef CsrUint8 CsrBtHiddStatusType;

typedef struct
{
    CsrBtHiddPrim            type;
} CsrBtHiddType;

typedef struct
{
    CsrBtHiddPrim            type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtHiddResultType;

typedef struct
{
    CsrBtHiddPrim            type;
    CsrBtDeviceAddr            deviceAddr;         /* address of remote device */
} CsrBtHiddAddressType;

typedef struct
{
    CsrBtHiddPrim            type;
    CsrBtDeviceAddr          deviceAddr;         /* address of remote device */
    CsrBtResultCode          resultCode;
    CsrBtSupplier            resultSupplier;
} CsrBtHiddAddressResultType;

typedef struct
{
    CsrBtHiddPrim            type;
    CsrSchedQid                     appHandle;
    CsrUint16                secLevel;
} CsrBtHiddSecurityType;

typedef struct
{
    CsrBtHiddPrim            type;
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtHiddSecurityCfmType;

/* ---------- End of general typedefs ---------- */

/* ---------- Defines: ---------- */
/* SDP flags */
#define CSR_BT_HIDD_FLAGS_VIRTUAL_CABLE_BIT      ((CsrUint8)    0x01)
#define CSR_BT_HIDD_FLAGS_RECONNECT_INIT_BIT     ((CsrUint8)    0x02)
#define CSR_BT_HIDD_FLAGS_BOOT_DEVICE_BIT        ((CsrUint8)    0x04)
#define CSR_BT_HIDD_FLAGS_SDP_DISABLE_BIT        ((CsrUint8)    0x08)
#define CSR_BT_HIDD_FLAGS_BATTERY_POWER_BIT      ((CsrUint8)    0x10)
#define CSR_BT_HIDD_FLAGS_REMOTE_WAKE_BIT        ((CsrUint8)    0x20)
#define CSR_BT_HIDD_FLAGS_NORMALLY_CONNECT_BIT   ((CsrUint8)    0x40)

#define CSR_BT_HIDD_HANDSHAKE               ((CsrBtHiddTransactionType) 0x00)
#define CSR_BT_HIDD_CONTROL                 ((CsrBtHiddTransactionType) 0x10)
#define CSR_BT_HIDD_GET_REPORT              ((CsrBtHiddTransactionType) 0x40)
#define CSR_BT_HIDD_SET_REPORT              ((CsrBtHiddTransactionType) 0x50)
#define CSR_BT_HIDD_GET_PROTOCOL            ((CsrBtHiddTransactionType) 0x60)
#define CSR_BT_HIDD_SET_PROTOCOL            ((CsrBtHiddTransactionType) 0x70)
#define CSR_BT_HIDD_GET_IDLE                ((CsrBtHiddTransactionType) 0x80)
#define CSR_BT_HIDD_SET_IDLE                ((CsrBtHiddTransactionType) 0x90)
#define CSR_BT_HIDD_DATA                    ((CsrBtHiddTransactionType) 0xA0)
#define CSR_BT_HIDD_DATC                    ((CsrBtHiddTransactionType) 0xB0)

#define CSR_BT_HIDD_SUCCESS                 ((CsrBtHiddHandshakeType) 0x00)
#define CSR_BT_HIDD_NOT_READY               ((CsrBtHiddHandshakeType) 0x01)
#define CSR_BT_HIDD_ERR_INVALID_REPORT_ID   ((CsrBtHiddHandshakeType) 0x02)
#define CSR_BT_HIDD_ERR_UNSUPPORTED         ((CsrBtHiddHandshakeType) 0x03)
#define CSR_BT_HIDD_ERR_INVALID_PARAMETER   ((CsrBtHiddHandshakeType) 0x04)
#define CSR_BT_HIDD_ERR_UNKNOWN             ((CsrBtHiddHandshakeType) 0x0E)
#define CSR_BT_HIDD_ERR_FATAL               ((CsrBtHiddHandshakeType) 0x0F)

#define CSR_BT_HIDD_NOP                     ((CsrBtHiddCtrlType) 0x00)
#define CSR_BT_HIDD_HARD_RESET              ((CsrBtHiddCtrlType) 0x01)
#define CSR_BT_HIDD_SOFT_RESET              ((CsrBtHiddCtrlType) 0x02)
#define CSR_BT_HIDD_SUSPEND                 ((CsrBtHiddCtrlType) 0x03)
#define CSR_BT_HIDD_EXIT_SUSPEND            ((CsrBtHiddCtrlType) 0x04)
#define CSR_BT_HIDD_VC_UNPLUG               ((CsrBtHiddCtrlType) 0x05)

#define CSR_BT_HIDD_OTHER                   ((CsrBtHiddReportType) 0x00)
#define CSR_BT_HIDD_INPUT_REPORT            ((CsrBtHiddReportType) 0x01)
#define CSR_BT_HIDD_OUTPUT_REPORT           ((CsrBtHiddReportType) 0x02)
#define CSR_BT_HIDD_FEATURE_REPORT          ((CsrBtHiddReportType) 0x03)

#define CSR_BT_HIDD_BOOT_PROTOCOL           ((CsrBtHiddProtocolType) 0x00)
#define CSR_BT_HIDD_REPORT_PROTOCOL         ((CsrBtHiddProtocolType) 0x01)

#define CSR_BT_HIDD_ACTIVE_MODE             ((CsrBtHiddPowerModeType) CSR_BT_ACTIVE_MODE)
#define CSR_BT_HIDD_SNIFF_MODE              ((CsrBtHiddPowerModeType) CSR_BT_SNIFF_MODE)
#define CSR_BT_HIDD_DISCONNECT_MODE         ((CsrBtHiddPowerModeType) 0x0004)

#define CSR_BT_HIDD_DISCONNECTED            ((CsrBtHiddStatusType) 0x00)
#define CSR_BT_HIDD_CONNECTED               ((CsrBtHiddStatusType) 0x01)
#define CSR_BT_HIDD_CONNECT_FAILED          ((CsrBtHiddStatusType) 0x02)
#define CSR_BT_HIDD_UNREGISTER_FAILED       ((CsrBtHiddStatusType) 0x03)
#define CSR_BT_HIDD_RECONNECTING            ((CsrBtHiddStatusType) 0x04)

/* ---------- End of Defines ------------ */

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_HIDD_PRIM_DOWNSTREAM_LOWEST                     (0x0000)

#define CSR_BT_HIDD_ACTIVATE_REQ               ((CsrBtHiddPrim) (0x0000 + CSR_BT_HIDD_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HIDD_DEACTIVATE_REQ             ((CsrBtHiddPrim) (0x0001 + CSR_BT_HIDD_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HIDD_CONTROL_RES                ((CsrBtHiddPrim) (0x0002 + CSR_BT_HIDD_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HIDD_DATA_REQ                   ((CsrBtHiddPrim) (0x0003 + CSR_BT_HIDD_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HIDD_UNPLUG_REQ                 ((CsrBtHiddPrim) (0x0004 + CSR_BT_HIDD_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HIDD_MODE_CHANGE_REQ            ((CsrBtHiddPrim) (0x0005 + CSR_BT_HIDD_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HIDD_SECURITY_IN_REQ            ((CsrBtHiddPrim) (0x0006 + CSR_BT_HIDD_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HIDD_SECURITY_OUT_REQ           ((CsrBtHiddPrim) (0x0007 + CSR_BT_HIDD_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_HIDD_PRIM_DOWNSTREAM_HIGHEST                    (0x0007 + CSR_BT_HIDD_PRIM_DOWNSTREAM_LOWEST)

/* Not part of the interface - used internally only */
#define CSR_BT_HIDD_RESTORE_IND                ((CsrBtHiddPrim) (CSR_BT_HIDD_PRIM_DOWNSTREAM_HIGHEST + 1))
#define CSR_BT_HIDD_REACTIVATE_IND             ((CsrBtHiddPrim) (CSR_BT_HIDD_PRIM_DOWNSTREAM_HIGHEST + 2))

/*******************************************************************************/

#define CSR_BT_HIDD_PRIM_UPSTREAM_LOWEST                       (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_HIDD_ACTIVATE_CFM               ((CsrBtHiddPrim) (0x0000 + CSR_BT_HIDD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HIDD_DEACTIVATE_CFM             ((CsrBtHiddPrim) (0x0001 + CSR_BT_HIDD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HIDD_STATUS_IND                 ((CsrBtHiddPrim) (0x0002 + CSR_BT_HIDD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HIDD_CONTROL_IND                ((CsrBtHiddPrim) (0x0003 + CSR_BT_HIDD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HIDD_DATA_CFM                   ((CsrBtHiddPrim) (0x0004 + CSR_BT_HIDD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HIDD_DATA_IND                   ((CsrBtHiddPrim) (0x0005 + CSR_BT_HIDD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HIDD_UNPLUG_CFM                 ((CsrBtHiddPrim) (0x0006 + CSR_BT_HIDD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HIDD_UNPLUG_IND                 ((CsrBtHiddPrim) (0x0007 + CSR_BT_HIDD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HIDD_MODE_CHANGE_IND            ((CsrBtHiddPrim) (0x0008 + CSR_BT_HIDD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HIDD_SECURITY_IN_CFM            ((CsrBtHiddPrim) (0x0009 + CSR_BT_HIDD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HIDD_SECURITY_OUT_CFM           ((CsrBtHiddPrim) (0x000A + CSR_BT_HIDD_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_HIDD_PRIM_UPSTREAM_HIGHEST                      (0x000A + CSR_BT_HIDD_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_HIDD_PRIM_DOWNSTREAM_COUNT      (CSR_BT_HIDD_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_HIDD_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_HIDD_PRIM_UPSTREAM_COUNT        (CSR_BT_HIDD_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_HIDD_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

/* ---------- HIDD API typedef structures ---------- */
typedef struct
{
    CsrBtHiddPrim            type;
    CsrSchedQid               appHandle;              /* application handle */
    CsrUint8                 qosCtrlCount;           /* number of qosCtrl structures */
    L2CA_QOS_T                   *qosCtrl;               /* ref. to QoS structure (control ch.) */
    CsrUint8                 qosIntrCount;           /* number of qosIntr structures */
    L2CA_QOS_T                   *qosIntr;               /* ref. to QoS structure (interrupt ch.) */
    CsrUint16                flushTimeout;           /* flush timeout value */
    CsrBtDeviceAddr            deviceAddr;             /* address of device */
    CsrUint16                deviceIdSdpLen;         /* length of device id service record */
    CsrUint8                 *deviceIdSdp;           /* device id service record */
    CsrUint16                hidSdpLen;              /* length of hid service record */
    CsrUint8                 *hidSdp;                /* hid service record */
} CsrBtHiddActivateReq;

typedef CsrBtHiddResultType CsrBtHiddActivateCfm;

typedef CsrBtHiddType CsrBtHiddDeactivateReq;

typedef CsrBtHiddResultType CsrBtHiddDeactivateCfm;

typedef struct
{
    CsrBtHiddPrim            type;
    CsrBtDeviceAddr            deviceAddr;         /* address of remote device */
    CsrBtHiddStatusType          status;             /* connection status */
    CsrBtConnId              btConnId;               /* Global Bluetooth connection ID */
} CsrBtHiddStatusInd;

typedef CsrBtHiddAddressType CsrBtHiddUnplugReq;

typedef CsrBtHiddResultType CsrBtHiddUnplugCfm;

typedef CsrBtHiddAddressResultType CsrBtHiddUnplugInd;

typedef struct
{
    CsrBtHiddPrim            type;
    CsrBtHiddTransactionType     transactionType;    /* transaction type, CSR_BT_HIDD_CONTROL or HIDD_GET_/HIDD_SET_ */
    CsrBtHiddParameterType       parameter;          /* parameter, dependent on transaction type */
    CsrUint16                dataLen;            /* optional dependent of transactionType, data length incl. 1 byte transaction header*/
    CsrUint8                 *data;              /* optional if any data, reference to data incl. transaction header*/
} CsrBtHiddControlInd;

typedef CsrBtHiddControlInd CsrBtHiddControlRes;

typedef struct
{
    CsrBtHiddPrim            type;
    CsrBtHiddReportType          reportType;         /* report type */
    CsrUint16                reportLen;          /* report length incl. 1 byte transaction header*/
    CsrUint8                 *report;            /* reference to report */
} CsrBtHiddDataReq;

typedef CsrBtHiddResultType CsrBtHiddDataCfm;

typedef CsrBtHiddDataReq CsrBtHiddDataInd;

typedef struct
{
    CsrBtHiddPrim            type;
    CsrBtHiddPowerModeType       mode;               /* power mode to change to */
} CsrBtHiddModeChangeReq;

typedef struct
{
    CsrBtHiddPrim            type;
    CsrBtHiddPowerModeType       mode;               /* power mode to change to */
    CsrBtResultCode         resultCode;
    CsrBtSupplier     resultSupplier;
} CsrBtHiddModeChangeInd;

typedef CsrBtHiddType CsrBtHiddReactivateInd;
typedef CsrBtHiddType CsrBtHiddRestoreInd;

typedef CsrBtHiddSecurityType CsrBtHiddSecurityInReq;
typedef CsrBtHiddSecurityType CsrBtHiddSecurityOutReq;

typedef CsrBtHiddSecurityCfmType CsrBtHiddSecurityInCfm;
typedef CsrBtHiddSecurityCfmType CsrBtHiddSecurityOutCfm;

/* ---------- End HIDD API typedef structures ---------- */

#ifdef __cplusplus
}
#endif

#endif

