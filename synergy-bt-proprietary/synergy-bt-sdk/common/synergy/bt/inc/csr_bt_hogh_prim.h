#ifndef CSR_BT_HOGH_PRIM_H_
#define CSR_BT_HOGH_PRIM_H_

/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_bt_result.h"
#include "csr_bt_profiles.h"
#include "csr_bt_gatt_prim.h"
#include "csr_bt_dis_common.h"
#include "csr_bt_scps_common.h"
#include "csr_bt_hids_common.h"


#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtHoghPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

/* ---------- Defines the HOGH CsrBtResultCode ----------*/
#define CSR_BT_RESULT_CODE_HOGH_SUCCESS                 ((CsrBtResultCode) (0x0000))
#define CSR_BT_RESULT_CODE_HOGH_NOT_READY               ((CsrBtResultCode) (0x0001))
#define CSR_BT_RESULT_CODE_HOGH_UNKNOWN_REPORT          ((CsrBtResultCode) (0x0002))
#define CSR_BT_RESULT_CODE_HOGH_NOT_SUPPORTED           ((CsrBtResultCode) (0x0003))
#define CSR_BT_RESULT_CODE_HOGH_INVALID_CONN_ID         ((CsrBtResultCode) (0x0004))
#define CSR_BT_RESULT_CODE_HOGH_UNEXPECTED              ((CsrBtResultCode) (0x0005))
#define CSR_BT_RESULT_CODE_HOGH_UNKNOWN_SERVICE         ((CsrBtResultCode) (0x0006))
#define CSR_BT_RESULT_CODE_HOGH_UNACCEPTABLE_PARAM      ((CsrBtResultCode) (0x0007))
#define CSR_BT_RESULT_CODE_HOGH_ALREADY_ADDED           ((CsrBtResultCode) (0x0008))
#define CSR_BT_RESULT_CODE_HOGH_MAX_CONNECTIONS         ((CsrBtResultCode) (0x0009))

typedef CsrPrim                                         CsrBtHoghPrim;

typedef CsrUint8                                        CsrBtHoghConnId;
#define CSR_BT_HOGH_CONN_ID_INVALID                     ((CsrBtHoghConnId) 0x00)

#define CSR_BT_HOGH_SERVICE_ID_INVALID                  ((CsrUint8) 0xFF)

typedef CsrUint8                                CsrBtHoghStatus;
#define CSR_BT_HOGH_STATUS_SCANNING             ((CsrBtHoghStatus) 0x00) /* Scanning for HID devices */
#define CSR_BT_HOGH_STATUS_CONNECTING           ((CsrBtHoghStatus) 0x01) /* Connecting to HID device */
#define CSR_BT_HOGH_STATUS_SERVICE_DISCOVERY    ((CsrBtHoghStatus) 0x02) /* ACL established; exchanging MTU; Finding HID and related services */
#define CSR_BT_HOGH_STATUS_CONFIGURING          ((CsrBtHoghStatus) 0x03) /* Enabling notifications and configuring scan parameters */
#define CSR_BT_HOGH_STATUS_READY                ((CsrBtHoghStatus) 0x04) /* HOGH is ready for use */
#define CSR_BT_HOGH_STATUS_DISCONNECTED         ((CsrBtHoghStatus) 0x05) /* ACL disconnected */
#define CSR_BT_HOGH_STATUS_RECONNECTING         ((CsrBtHoghStatus) 0x06) /* Attempting reconnection */
#define CSR_BT_HOGH_STATUS_SERVICE_CHANGED      ((CsrBtHoghStatus) 0x07) /* Service changed. Previously reported services are invalid */

/* Features supported by HOGP device */
typedef CsrUint8                                        CsrBtHoghSuppFeature; /* Combination of following values to be used */
#define CSR_BT_HOGH_FEATURE_NONE                        ((CsrBtHoghSuppFeature) 0x00) /* For internal use */
#define CSR_BT_HOGH_FEATURE_HID                         ((CsrBtHoghSuppFeature) 0x01) /* Supports HID service */
#define CSR_BT_HOGH_FEATURE_BATTERY                     ((CsrBtHoghSuppFeature) 0x02) /* Supports Battery service */
#define CSR_BT_HOGH_FEATURE_SCAN_PARAM                  ((CsrBtHoghSuppFeature) 0x04) /* Supports Scan parameter service */

typedef CsrUint8                                        CsrBtHoghReportType;
#define CSR_BT_HOGH_REPORT_TYPE_INPUT                   ((CsrBtHoghReportType) CSR_BT_HIDS_REPORT_TYPE_INPUT)       /* Input Report type */
#define CSR_BT_HOGH_REPORT_TYPE_OUTPUT                  ((CsrBtHoghReportType) CSR_BT_HIDS_REPORT_TYPE_OUTPUT)      /* Output Report type  */
#define CSR_BT_HOGH_REPORT_TYPE_FEATURE                 ((CsrBtHoghReportType) CSR_BT_HIDS_REPORT_TYPE_FEATURE)     /* Feature Report type */
#define CSR_BT_HOGH_REPORT_TYPE_BOOT_MOUSE_INPUT        ((CsrBtHoghReportType) CSR_BT_HIDS_REPORT_TYPE_FEATURE + 1) /* Boot Mouse Input type */
#define CSR_BT_HOGH_REPORT_TYPE_BOOT_KEYBOARD_INPUT     ((CsrBtHoghReportType) CSR_BT_HIDS_REPORT_TYPE_FEATURE + 2) /* Boot Keyboard Input type */
#define CSR_BT_HOGH_REPORT_TYPE_BOOT_KEYBOARD_OUTPUT    ((CsrBtHoghReportType) CSR_BT_HIDS_REPORT_TYPE_FEATURE + 3) /* Boot Keyboard Output type */
#define CSR_BT_HOGH_REPORT_TYPE_BATTERY_LEVEL           ((CsrBtHoghReportType) CSR_BT_HIDS_REPORT_TYPE_FEATURE + 4) /* Battery Level type. Only for stand-alone Battery service */

typedef CsrUint16                                       CsrBtHoghReportTypeFlag; /* Combination of following values to be used */
#define CSR_BT_HOGH_REPORT_FLAG_INPUT                   ((CsrBtHoghReportTypeFlag) (1 << CSR_BT_HOGH_REPORT_TYPE_INPUT               ))
#define CSR_BT_HOGH_REPORT_FLAG_OUTPUT                  ((CsrBtHoghReportTypeFlag) (1 << CSR_BT_HOGH_REPORT_TYPE_OUTPUT              ))
#define CSR_BT_HOGH_REPORT_FLAG_FEATURE                 ((CsrBtHoghReportTypeFlag) (1 << CSR_BT_HOGH_REPORT_TYPE_FEATURE             ))
#define CSR_BT_HOGH_REPORT_FLAG_BOOT_MOUSE_INPUT        ((CsrBtHoghReportTypeFlag) (1 << CSR_BT_HOGH_REPORT_TYPE_BOOT_MOUSE_INPUT    ))
#define CSR_BT_HOGH_REPORT_FLAG_BOOT_KEYBOARD_INPUT     ((CsrBtHoghReportTypeFlag) (1 << CSR_BT_HOGH_REPORT_TYPE_BOOT_KEYBOARD_INPUT ))
#define CSR_BT_HOGH_REPORT_FLAG_BOOT_KEYBOARD_OUTPUT    ((CsrBtHoghReportTypeFlag) (1 << CSR_BT_HOGH_REPORT_TYPE_BOOT_KEYBOARD_OUTPUT))
#define CSR_BT_HOGH_REPORT_FLAG_BATTERY_LEVEL           ((CsrBtHoghReportTypeFlag) (1 << CSR_BT_HOGH_REPORT_TYPE_BATTERY_LEVEL       )) /* This is mutually exclusive with other flags */

/* Used as idleTimeout and reconnectTimeout in Timeout request */
#define CSR_BT_HOGH_TIMEOUT_IMMEDIATE                   (0x0000)
#define CSR_BT_HOGH_TIMEOUT_NEVER                       (0xFFFF)

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_HOGH_PRIM_DOWNSTREAM_LOWEST               (0x0000)

#define CSR_BT_HOGH_REGISTER_REQ        ((CsrBtHoghPrim) (0x0000 + CSR_BT_HOGH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HOGH_ADD_DEVICE_REQ      ((CsrBtHoghPrim) (0x0001 + CSR_BT_HOGH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HOGH_CONN_PARAM_REQ      ((CsrBtHoghPrim) (0x0002 + CSR_BT_HOGH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HOGH_TIMEOUT_REQ         ((CsrBtHoghPrim) (0x0003 + CSR_BT_HOGH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HOGH_SECURITY_REQ        ((CsrBtHoghPrim) (0x0004 + CSR_BT_HOGH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HOGH_GET_REPORT_REQ      ((CsrBtHoghPrim) (0x0005 + CSR_BT_HOGH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HOGH_SET_REPORT_REQ      ((CsrBtHoghPrim) (0x0006 + CSR_BT_HOGH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HOGH_SET_PROTOCOL_REQ    ((CsrBtHoghPrim) (0x0007 + CSR_BT_HOGH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HOGH_SET_STATE_REQ       ((CsrBtHoghPrim) (0x0008 + CSR_BT_HOGH_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_HOGH_REMOVE_DEVICE_REQ   ((CsrBtHoghPrim) (0x0009 + CSR_BT_HOGH_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_HOGH_PRIM_DOWNSTREAM_HIGHEST              (0x0009 + CSR_BT_HOGH_PRIM_DOWNSTREAM_LOWEST)

/****************************************************************************************/

#define CSR_BT_HOGH_PRIM_UPSTREAM_LOWEST                 (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_HOGH_REGISTER_CFM        ((CsrBtHoghPrim) (0x0000 + CSR_BT_HOGH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HOGH_ADD_DEVICE_CFM      ((CsrBtHoghPrim) (0x0001 + CSR_BT_HOGH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HOGH_STATUS_IND          ((CsrBtHoghPrim) (0x0002 + CSR_BT_HOGH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HOGH_CONN_PARAM_CFM      ((CsrBtHoghPrim) (0x0003 + CSR_BT_HOGH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HOGH_TIMEOUT_CFM         ((CsrBtHoghPrim) (0x0004 + CSR_BT_HOGH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HOGH_SECURITY_CFM        ((CsrBtHoghPrim) (0x0005 + CSR_BT_HOGH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HOGH_DEVICE_INFO_IND     ((CsrBtHoghPrim) (0x0006 + CSR_BT_HOGH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HOGH_SERVICE_IND         ((CsrBtHoghPrim) (0x0007 + CSR_BT_HOGH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HOGH_SET_PROTOCOL_CFM    ((CsrBtHoghPrim) (0x0008 + CSR_BT_HOGH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HOGH_GET_REPORT_IND      ((CsrBtHoghPrim) (0x0009 + CSR_BT_HOGH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HOGH_SET_REPORT_CFM      ((CsrBtHoghPrim) (0x000A + CSR_BT_HOGH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HOGH_SET_STATE_CFM       ((CsrBtHoghPrim) (0x000B + CSR_BT_HOGH_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_HOGH_REMOVE_DEVICE_CFM   ((CsrBtHoghPrim) (0x000C + CSR_BT_HOGH_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_HOGH_PRIM_UPSTREAM_HIGHEST                (0x000C + CSR_BT_HOGH_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_HOGH_PRIM_DOWNSTREAM_COUNT   (CSR_BT_HOGH_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_HOGH_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_HOGH_PRIM_UPSTREAM_COUNT     (CSR_BT_HOGH_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_HOGH_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/


/*******************************************************************************
 * Downstream primitive structures - Refer csr_bt_hogh_lib.h for downstream APIs
 *******************************************************************************/
typedef struct
{
    CsrBtHoghPrim       type;               /* CSR_BT_HOGH_REGISTER_REQ */
    CsrSchedQid         pHandle;            /* Application handle */
} CsrBtHoghRegisterReq;

typedef struct
{
    CsrBtHoghPrim       type;               /* CSR_BT_HOGH_ADD_DEVICE_REQ */
    CsrBtTypedAddr      addr;
    CsrUint16           mtu;
    CsrUint16           connAttemptTimeout; /* Connection attempt timeout */
} CsrBtHoghAddDeviceReq;

typedef struct
{
    CsrBtHoghPrim       type;               /* CSR_BT_HOGH_SECURITY_REQ */
    CsrBtHoghConnId     hoghConnId;
    CsrUint16           scanInterval;       /* Scan interval for central */
    CsrUint16           scanWindow;         /* Scan window for central */
    CsrUint16           connIntervalMin;    /* Connection interval minimum */
    CsrUint16           connIntervalMax;    /* Connection interval maximum */
    CsrUint16           connLatency;        /* Connection latency */
    CsrUint16           supervisionTimeout; /* Connection supervision timeout */
} CsrBtHoghConnParamReq;

typedef struct
{
    CsrBtHoghPrim       type;               /* CSR_BT_HOGH_SECURITY_REQ */
    CsrBtHoghConnId     hoghConnId;
    CsrUint16           idleTimeout;        /* Time = idleTimeout * 0.1 seconds;
                                               CSR_BT_HOGH_TIMEOUT_IMMEDIATE - Immediately disconnect;
                                               CSR_BT_HOGH_TIMEOUT_NEVER - Never disconnect */
    CsrUint16           reconnectTimeout;   /* Time = reconnectTimeout * 0.1 seconds;
                                               CSR_BT_HOGH_TIMEOUT_IMMEDIATE - Immediately connect;
                                               CSR_BT_HOGH_TIMEOUT_NEVER - Never connect */
} CsrBtHoghTimeoutReq;

typedef struct
{
    CsrBtHoghPrim       type;               /* CSR_BT_HOGH_SECURITY_REQ */
    CsrBtHoghConnId     hoghConnId;
    CsrBtGattSecurityFlags secReq;          /* Security requirements */
} CsrBtHoghSecurityReq;

typedef struct
{
    CsrBtHoghPrim       type;               /* CSR_BT_HOGH_GET_REPORT_REQ */
    CsrBtHoghConnId     hoghConnId;
    CsrUint8            serviceId;
    CsrBtHoghReportType reportType;
    CsrUint8            reportId;
} CsrBtHoghGetReportReq;

typedef struct
{
    CsrBtHoghPrim       type;               /* CSR_BT_HOGH_SET_REPORT_REQ */
    CsrBtHoghConnId     hoghConnId;
    CsrUint8            serviceId;
    CsrBtHoghReportType reportType;
    CsrUint8            reportId;
    CsrUint16           reportLength;
    CsrUint8           *report;
} CsrBtHoghSetReportReq;

typedef struct
{
    CsrBtHoghPrim       type;               /* CSR_BT_HOGH_SET_PROTOCOL_REQ */
    CsrBtHoghConnId     hoghConnId;
    CsrUint8            serviceId;
    CsrBtHidsProtocol   protocol;
} CsrBtHoghSetProtocolReq;

typedef struct
{
    CsrBtHoghPrim       type;               /* CSR_BT_HOGH_SET_STATE_REQ */
    CsrBtHidsControlPoint controlPoint;
} CsrBtHoghSetStateReq;

typedef struct
{
    CsrBtHoghPrim       type;               /* CSR_BT_HOGH_REMOVE_DEVICE_REQ */
    CsrBtHoghConnId     hoghConnId;
} CsrBtHoghRemoveDeviceReq;



/*******************************************************************************
 * Upstream primitive structures
 *******************************************************************************/

/*************************** Register confirm *********************************
 * Response to Register request.
 *****************************************************************************/
typedef struct
{
    CsrBtHoghPrim       type;               /* CSR_BT_HOGH_REGISTER_CFM */
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtHoghRegisterCfm;

/**************************** Add Device confirm ******************************
 * Response to Add Device Request. It returns hoghConnId which must be used with
 * all communication against this connection.
 *****************************************************************************/
typedef struct
{
    CsrBtHoghPrim       type;               /* CSR_BT_HOGH_ADD_DEVICE_CFM */
    CsrBtHoghConnId     hoghConnId;         /* HOGH connection ID */
    CsrBtTypedAddr      addr;
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtHoghAddDeviceCfm;

/***************************** Status indication ******************************
 * Informs application of change in state of connection.
 *****************************************************************************/
typedef struct
{
    CsrBtHoghPrim       type;               /* CSR_BT_HOGH_STATUS_IND */
    CsrBtHoghConnId     hoghConnId;
    CsrBtHoghStatus     status;
} CsrBtHoghStatusInd;

/************************ Connection Parameter confirm ************************
 * Response to Connection Parameter request.
 *****************************************************************************/
typedef struct
{
    CsrBtHoghPrim       type;               /* CSR_BT_HOGH_CONN_PARAM_CFM */
    CsrBtHoghConnId     hoghConnId;
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtHoghConnParamCfm;

/*************************** Timeout confirm **********************************
 * Response to Timeout request.
 *****************************************************************************/
typedef struct
{
    CsrBtHoghPrim       type;               /* CSR_BT_HOGH_TIMEOUT_CFM */
    CsrBtHoghConnId     hoghConnId;
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtHoghTimeoutCfm;

/***************************** Security confirm *******************************
 * Response to Security request.
 *****************************************************************************/
typedef struct
{
    CsrBtHoghPrim       type;               /* CSR_BT_HOGH_SECURITY_CFM */
    CsrBtHoghConnId     hoghConnId;
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtHoghSecurityCfm;

/*********************** Device Information indication ************************
 * Indicates device information to application.
 * This indication is sent when service discovery and configuration is complete.
 *****************************************************************************/
typedef struct
{
    CsrBtHoghPrim       type;               /* CSR_BT_HOGH_DEVICE_INFO_IND */
    CsrBtHoghConnId     hoghConnId;
    CsrBtTypedAddr      addr;
    CsrBtDisPnpId       pnpId;
    CsrUtf8String      *name;
    CsrBtHoghSuppFeature optionalFeatures; /* Optional features supported by HOGP device */
} CsrBtHoghDeviceInfoInd;

/*************************** Service indication *******************************
 * Indicates capabilities of individual instance of HID or Battery (stand-alone) service
 *
 * One or more indications may be sent depending upon number of HID and Battery
 * service instances found on HID device.
 *
 * A HID service along with Battery services referenced by it are reported as
 * single service.
 *
 * Applications must extract Report ID and Report Type from reportMap to
 * identify different HID Report protocol reports.
 * If a Battery service is included in HID service, its Battery Level
 * characteristic is also presented as a report of HID service. Applications
 * must extract Report ID and Report Type for such reports from HID report map.
 *
 * Application must use 0 as Report ID and
 *      CSR_BT_HOGH_REPORT_TYPE_BOOT_MOUSE_INPUT,
 *      CSR_BT_HOGH_REPORT_TYPE_BOOT_KEYBOARD_INPUT or
 *      CSR_BT_HOGH_REPORT_TYPE_BOOT_KEYBOARD_OUTPUT
 * as Report Type for identifying HID Boot Protocol reports.
 *
 * Stand-alone Battery services which are not referenced in any HID service are
 * reported as separate services.
 *
 * Applications must use 0 as Report ID and CSR_BT_HOGH_REPORT_TYPE_BATTERY_LEVEL
 * as Report Type for identifying Battery Level reports from stand-alone
 * Battery service
 *
 * This indication is sent when service discovery and configuration is complete.
 *****************************************************************************/
typedef struct
{
    CsrBtHoghPrim           type;               /* CSR_BT_HOGH_SERVICE_IND */
    CsrBtHoghConnId         hoghConnId;
    CsrUint8                serviceId;          /* Identifier for the service */
    CsrBtHoghReportTypeFlag supportedReports;
    CsrBtHidsProtocol       protocol;
    CsrBtHidsControlPoint   controlPoint;       /* Only relevant for HID services */
    CsrBtHidsInformation    hidInformation;     /* Only relevant for HID services */
    CsrUint16               reportMapLength;    /* Only relevant for HID services
                                                   0 if report protocol is not supported in HID service */
    CsrUint8               *reportMap;          /* Only relevant for HID services.
                                                   NULL if report protocol is not supported in HID service */
} CsrBtHoghServiceInd;

/************************** Set Protocol confirm ******************************
 * Response to Set Protocol request
 *****************************************************************************/
typedef struct
{
    CsrBtHoghPrim       type;               /* CSR_BT_HOGH_SET_PROTOCOL_CFM */
    CsrBtHoghConnId     hoghConnId;
    CsrUint8            serviceId;
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtHoghSetProtocolCfm;

/*************************** Get Report indication ****************************
 * Indicates report from HID device.
 *
 * This indication is sent as result of either
 *      1) notification from HID device, or
 *      2) Get Report request from application.
 *****************************************************************************/
typedef struct
{
    CsrBtHoghPrim       type;               /* CSR_BT_HOGH_GET_REPORT_IND */
    CsrBtHoghConnId     hoghConnId;
    CsrUint8            serviceId;
    CsrBtHoghReportType reportType;
    CsrUint8            reportId;           /* Set to 0 for Boot or Battery reports */
    CsrUint16           reportLength;
    CsrUint8           *report;
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtHoghGetReportInd;

/**************************** Set Protocol confirm ****************************
 * Response to Set Report request
 *****************************************************************************/
typedef struct
{
    CsrBtHoghPrim       type;               /* CSR_BT_HOGH_SET_REPORT_CFM */
    CsrBtHoghConnId     hoghConnId;
    CsrUint8            serviceId;
    CsrBtHoghReportType reportType;
    CsrUint8            reportId;
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtHoghSetReportCfm;

/***************************** Set State confirm ******************************
 * Response to Set State confirm
 *****************************************************************************/
typedef struct
{
    CsrBtHoghPrim       type;               /* CSR_BT_HOGH_SET_STATE_CFM */
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtHoghSetStateCfm;

/**************************** Remove Device confirm ***************************
 * Indicates that connection has been removed.
 *
 * Application request to remove a connection using Remove Connection request.
 *
 * A connection may also be removed due to any failure during Connection,
 * Service Discovery or Configuring states
 *****************************************************************************/
typedef struct
{
    CsrBtHoghPrim       type;               /* CSR_BT_HOGH_REMOVE_DEVICE_CFM */
    CsrBtHoghConnId     hoghConnId;
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtHoghRemoveDeviceCfm;


#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_HOGH_PRIM_H_ */
