#ifndef CSR_BT_SD_PRIM_H__
#define CSR_BT_SD_PRIM_H__
/******************************************************************************
 Copyright (c) 2004-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

 REVISION:      $Revision: #2 $
******************************************************************************/


#include "csr_synergy.h"

#include "csr_types.h"
#include "csr_bt_profiles.h"
#include "csr_bt_cm_prim.h"
#include "csr_bt_result.h"
#include "csr_bt_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtSdPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim    CsrBtSdPrim;

/* ---------- Defines the Service Manager (SD) CsrBtResultCode ----------*/
#define CSR_BT_RESULT_CODE_SD_SUCCESS                       ((CsrBtResultCode) (0x0000))
#define CSR_BT_RESULT_CODE_SD_COMMAND_DISALLOWED            ((CsrBtResultCode) (0x0001))
#define CSR_BT_RESULT_CODE_SD_UNACCEPTABLE_PARAMETER        ((CsrBtResultCode) (0x0002))
#define CSR_BT_RESULT_CODE_SD_UNKNOWN_SERVICE_RECORD_HANDLE ((CsrBtResultCode) (0x0003))
#define CSR_BT_RESULT_CODE_SD_DEVICE_NOT_FOUND              ((CsrBtResultCode) (0x0004))
#define CSR_BT_RESULT_CODE_SD_SEARCH_CANCELLED              ((CsrBtResultCode) (0x0005))
#define CSR_BT_RESULT_CODE_SD_MAX_RESULTS                   ((CsrBtResultCode) (0x0006))
#define CSR_BT_RESULT_CODE_SD_TIMEOUT                       ((CsrBtResultCode) (0x0007))
#define CSR_BT_RESULT_CODE_SD_RSSI_BUFFER_EMPTY             ((CsrBtResultCode) (0x0008))
#define CSR_BT_RESULT_CODE_SD_CONNECTION_TERMINATED         ((CsrBtResultCode) (0x0009))
#define CSR_BT_RESULT_CODE_SD_DISCOVERY_INCOMPLETE          ((CsrBtResultCode) (0x000A))
#define CSR_BT_RESULT_CODE_SD_INSUFFICIENT_RESOURCES        ((CsrBtResultCode) (0x000B))
#define CSR_BT_RESULT_CODE_SD_SERVICE_CHANGED               ((CsrBtResultCode) (0x000C))

typedef struct
{
    CsrBtDeviceAddr              deviceAddr;
    CsrBtClassOfDevice           deviceClass;
    CsrUint16                    infoLen;
    CsrUint8                    *info;
    CsrUint32                    deviceStatus;
} CsrBtSdDeviceInfoType;

/* Miscellaneous values related to searching */
#define CSR_BT_SD_RSSI_THRESHOLD_DONT_CARE                 CSR_BT_RSSI_INVALID/* Use this value in a CSR_BT_SD_SEARCH_REQ if all results, regardless of RSSI-level, should be sent to the application */
#define CSR_BT_SD_SEARCH_DISABLE_BUFFERING                 0x00000000  /* Use this value in a CSR_BT_SD_SEARCH_REQ if a standard search should be performed (eg. not buffered search)*/

/* Inquiry Access Code values */
#define CSR_BT_SD_ACCESS_CODE_GIAC                         CSR_BT_CM_ACCESS_CODE_GIAC /* 0x9e8b33 */
#define CSR_BT_SD_ACCESS_CODE_LIAC                         CSR_BT_CM_ACCESS_CODE_LIAC /* 0x9e8b00 */

/* Device status bit mask definitions. Unused bits are reserved, and must be ignored. */
#define CSR_BT_SD_STATUS_TRUSTED                           0x00000001  /* Device is trusted. All connections from this device are implicitly authorized. */
#define CSR_BT_SD_STATUS_PAIRED                            0x00000002  /* Device is paired, i.e. link keys have been exchanged to be used for authentication. */
#define CSR_BT_SD_STATUS_REMOTE_NAME_UPDATED               0x00000004  /* The name of the remote device has been changed since previous name search. This is also set for new devices. */
#define CSR_BT_SD_STATUS_RADIO_LE                          0x00000008  /* Bluetooth low energy radio detected */
#define CSR_BT_SD_STATUS_RADIO_BREDR                       0x00000010  /* Classic Bluetooth (BR/EDR) radio detected */
#define CSR_BT_SD_STATUS_PRIVATE_ADDR                      0x00000020  /* Low Energy private/random address */

/* Search configuration flags. Unused bits are reserved, and must be set to zero. */
#define CSR_BT_SD_SEARCH_USE_STANDARD                      0x00000000  /* Use the standard search behaviour of the SD meaning the negated option of all below SD_SEARCH* defines */
#define CSR_BT_SD_SEARCH_USE_PRECONFIGURED                 0x00000001  /* Use the values that have been set in CSR_BT_SD_SEARCH_CONFIG_REQ or csr_bt_usr_config.h. This also means that the rest of the bits in the configuration pattern are ignored. */
#define CSR_BT_SD_SEARCH_FORCE_NAME_UPDATE                 0x00000002  /* Force update of names even if they are present in the Device List */
#define CSR_BT_SD_SEARCH_SHOW_UNKNOWN_DEVICE_NAMES         0x00000004  /* Report search results where the remote name could not be retrieved */
#define CSR_BT_SD_SEARCH_HIDE_PAIRED_DEVICES               0x00000008  /* Do not report search results of devices that has been paired */
#define CSR_BT_SD_SEARCH_DO_NOT_CLEAR_FILTER               0x00000010  /* Keep the filter from the previous search so only new devices are reported */
#define CSR_BT_SD_SEARCH_CONTINUE_AFTER_RSSI_SORTING       0x00000020  /* Continue search after names have been reported for all devices found during the RSSI sorting period */
#define CSR_BT_SD_SEARCH_ALLOW_UNSORTED_SEARCH_RESULTS     0x00000040  /* Allow search results to be sent to the application without being sorted according to the RSSI level */
#define CSR_BT_SD_SEARCH_ENABLE_IMM_RESULTS                0x00000080  /* Enable immediate search results that will be sent immediately when a new device is discovered - will always be followed by a normal result */
#define CSR_BT_SD_SEARCH_DISABLE_NAME_READING              0x00000100  /* Only already known names will be sent to the app - dedicated name reading will not be performed */
#define CSR_BT_SD_SEARCH_LOW_INQUIRY_PRIORITY_DURING_ACL   0x00000200  /* Sets the priority level for inquiry to low if an ACL is present */
#define CSR_BT_SD_SEARCH_ENABLE_SCAN_DURING_INQUIRY        0x00000400  /* Enables inquiry scan to have priority while doing inquiry - so device is discoverable */
#define CSR_BT_SD_SEARCH_DISABLE_BT_LE                     0x00000800  /* Do not use Bluetooth Low Energy baseband */
#define CSR_BT_SD_SEARCH_DISABLE_BT_CLASSIC                0x00001000  /* Do not use classic Bluetooth baseband */
#define CSR_BT_SD_SEARCH_ENABLE_LE_PASSIVE_SCAN            0x00002000  /* Enable passive scan for LE devices. I.e. only obtain information of the LE devices from its AD reports, but not requesting the scan response.*/

/* Service configuration flags. Unused bits are reserved, and must be set to zero. */
#define CSR_BT_SD_SERVICE_USE_STANDARD                     0x00000000  /* Use the standard search behaviour in the SD */
#define CSR_BT_SD_SERVICE_NO_NAME_UPDATE                   0x00000001  /* Do not retrieve the remote name during service search */

/* Device list configuration flags. Unused bits are reserved, and must be set to zero. */
#define CSR_BT_SD_DEVICE_USE_STANDARD                      0x00000000  /* Use the standard behaviour when reading the device list */
#define CSR_BT_SD_DEVICE_EXCLUDE_NON_PAIRED_DEVICES        0x00000001  /* Exclude non-paired/non-trusted devices from the device list */
#define CSR_BT_SD_DEVICE_EXCLUDE_PAIRED_DEVICES            0x00000002  /* Exclude paired devices from the device list */

/* Memory configuration flags */
#define CSR_BT_SD_MEMORY_CONFIG_USE_STANDARD               0x00000000  /* Standard memory configuration */

/* Cleanup configuration modes */
#define CSR_BT_SD_CLEANUP_EVERYTHING                       0x00000001  /* Cleanup as much as possible */
#define CSR_BT_SD_CLEANUP_INSTANCE                         0x00000002  /* Cleanup everything related to a specific application */
#define CSR_BT_SD_CLEANUP_DEVICE_LIST                      0x00000003  /* Cleanup as much as possible in the Device List */

/* Device Info format */
#define CSR_BT_SD_DEVICE_INFO_FORMAT_LEN_SIZE              (2)
#define CSR_BT_SD_DEVICE_INFO_FORMAT_TYPE_SIZE             (2)
#define CSR_BT_SD_DEVICE_INFO_FORMAT_HEADER_SIZE           (4)

/* Device Info tag types */
#define CSR_BT_SD_DEVICE_INFO_TYPE_UUID16_INCOMPLETE       (CSR_BT_EIR_DATA_TYPE_MORE_16_BIT_UUID)
#define CSR_BT_SD_DEVICE_INFO_TYPE_UUID16_COMPLETE         (CSR_BT_EIR_DATA_TYPE_COMPLETE_16_BIT_UUID)
#define CSR_BT_SD_DEVICE_INFO_TYPE_UUID32_INCOMPLETE       (CSR_BT_EIR_DATA_TYPE_MORE_32_BIT_UUID)
#define CSR_BT_SD_DEVICE_INFO_TYPE_UUID32_COMPLETE         (CSR_BT_EIR_DATA_TYPE_COMPLETE_32_BIT_UUID)
#define CSR_BT_SD_DEVICE_INFO_TYPE_UUID128_INCOMPLETE      (CSR_BT_EIR_DATA_TYPE_MORE_128_BIT_UUID)
#define CSR_BT_SD_DEVICE_INFO_TYPE_UUID128_COMPLETE        (CSR_BT_EIR_DATA_TYPE_COMPLETE_128_BIT_UUID)
#define CSR_BT_SD_DEVICE_INFO_TYPE_NAME_SHORT              (CSR_BT_EIR_DATA_TYPE_SHORT_LOCAL_NAME)
#define CSR_BT_SD_DEVICE_INFO_TYPE_NAME_FULL               (CSR_BT_EIR_DATA_TYPE_COMPLETE_LOCAL_NAME)
#define CSR_BT_SD_DEVICE_INFO_TYPE_MANUFACTURER            (CSR_BT_EIR_DATA_TYPE_MANUFACTURER_SPECIFIC)

/* Advertising/scan-response data types for Low Energy (similar to EIR) */


/* Search defines */
#define CSR_BT_SD_DEFAULT_INQUIRY_TX_POWER_LEVEL           (4)

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_SD_PRIM_DOWNSTREAM_LOWEST                                 (0x0000)

#define CSR_BT_SD_MEMORY_CONFIG_REQ                        ((CsrBtSdPrim) (0x0000 + CSR_BT_SD_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SD_SEARCH_CONFIG_REQ                        ((CsrBtSdPrim) (0x0001 + CSR_BT_SD_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SD_READ_DEVICE_INFO_REQ                     ((CsrBtSdPrim) (0x0002 + CSR_BT_SD_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SD_SEARCH_REQ                               ((CsrBtSdPrim) (0x0003 + CSR_BT_SD_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SD_CANCEL_SEARCH_REQ                        ((CsrBtSdPrim) (0x0004 + CSR_BT_SD_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SD_READ_AVAILABLE_SERVICES_REQ              ((CsrBtSdPrim) (0x0005 + CSR_BT_SD_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SD_CANCEL_READ_AVAILABLE_SERVICES_REQ       ((CsrBtSdPrim) (0x0006 + CSR_BT_SD_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SD_READ_DEVICE_LIST_REQ                     ((CsrBtSdPrim) (0x0007 + CSR_BT_SD_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SD_READ_SERVICE_RECORD_REQ                  ((CsrBtSdPrim) (0x0008 + CSR_BT_SD_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SD_CANCEL_READ_SERVICE_RECORD_REQ           ((CsrBtSdPrim) (0x0009 + CSR_BT_SD_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SD_REGISTER_SERVICE_RECORD_REQ              ((CsrBtSdPrim) (0x000A + CSR_BT_SD_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SD_UNREGISTER_SERVICE_RECORD_REQ            ((CsrBtSdPrim) (0x000B + CSR_BT_SD_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SD_CLEANUP_REQ                              ((CsrBtSdPrim) (0x000C + CSR_BT_SD_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SD_SYNCHRONIZE_REQ                          ((CsrBtSdPrim) (0x000D + CSR_BT_SD_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SD_READ_SERVICES_REQ                        ((CsrBtSdPrim) (0x000E + CSR_BT_SD_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_SD_READ_SERVICES_CANCEL_REQ                 ((CsrBtSdPrim) (0x000F + CSR_BT_SD_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_SD_PRIM_DOWNSTREAM_HIGHEST                                 (0x000F + CSR_BT_SD_PRIM_DOWNSTREAM_LOWEST)

/*  Not part of interface, used internally only */
#define CSR_BT_SD_HOUSE_CLEANING                           ((CsrBtSdPrim) (0x0100))

/*******************************************************************************/

#define CSR_BT_SD_PRIM_UPSTREAM_LOWEST                                   (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_SD_READ_DEVICE_INFO_CFM                     ((CsrBtSdPrim) (0x0000 + CSR_BT_SD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SD_SEARCH_RESULT_IND                        ((CsrBtSdPrim) (0x0001 + CSR_BT_SD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SD_CLOSE_SEARCH_IND                         ((CsrBtSdPrim) (0x0002 + CSR_BT_SD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SD_READ_AVAILABLE_SERVICES_CFM              ((CsrBtSdPrim) (0x0003 + CSR_BT_SD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SD_CANCEL_READ_AVAILABLE_SERVICES_CFM       ((CsrBtSdPrim) (0x0004 + CSR_BT_SD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SD_READ_DEVICE_LIST_IND                     ((CsrBtSdPrim) (0x0005 + CSR_BT_SD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SD_READ_DEVICE_LIST_CFM                     ((CsrBtSdPrim) (0x0006 + CSR_BT_SD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SD_READ_SERVICE_RECORD_IND                  ((CsrBtSdPrim) (0x0007 + CSR_BT_SD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SD_READ_SERVICE_RECORD_CFM                  ((CsrBtSdPrim) (0x0008 + CSR_BT_SD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SD_REGISTER_SERVICE_RECORD_CFM              ((CsrBtSdPrim) (0x0009 + CSR_BT_SD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SD_UNREGISTER_SERVICE_RECORD_CFM            ((CsrBtSdPrim) (0x000A + CSR_BT_SD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SD_IMMEDIATE_SEARCH_RESULT_IND              ((CsrBtSdPrim) (0x000B + CSR_BT_SD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SD_MEMORY_CONFIG_CFM                        ((CsrBtSdPrim) (0x000C + CSR_BT_SD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SD_SEARCH_CONFIG_CFM                        ((CsrBtSdPrim) (0x000D + CSR_BT_SD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SD_CLEANUP_CFM                              ((CsrBtSdPrim) (0x000E + CSR_BT_SD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SD_SYNCHRONIZE_CFM                          ((CsrBtSdPrim) (0x000F + CSR_BT_SD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SD_READ_SERVICES_CFM                        ((CsrBtSdPrim) (0x0010 + CSR_BT_SD_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_SD_READ_SERVICES_CANCEL_CFM                 ((CsrBtSdPrim) (0x0011 + CSR_BT_SD_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_SD_PRIM_UPSTREAM_HIGHEST                                   (0x0011 + CSR_BT_SD_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_SD_PRIM_DOWNSTREAM_COUNT                    (CSR_BT_SD_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_SD_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_SD_PRIM_UPSTREAM_COUNT                      (CSR_BT_SD_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_SD_PRIM_UPSTREAM_LOWEST)

/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrBtSdPrim                 type;
    CsrSchedQid                 phandle;
    CsrUint32                   memoryConfig;       /* Currently CSR_UNUSED - use CSR_BT_SD_MEMORY_CONFIG_DEFAULT */
    CsrUint32                   deviceListMax;      /* Currently CSR_UNUSED - use CSR_BT_SD_MEMORY_LIST_MAX_DEFAULT */
    CsrUint32                   deviceListInfoMax;  /* Max number of devices for which to store additional info besides address and CoD */
} CsrBtSdMemoryConfigReq;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
    CsrUint32                   memoryConfig;
    CsrUint32                   deviceListMax;
    CsrUint32                   deviceListInfoMax;
} CsrBtSdMemoryConfigCfm;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrSchedQid                 phandle;
    CsrUint32                   searchConfig;
    CsrUint32                   readNameTimeout;
    CsrUint32                   maxSearchResults;
} CsrBtSdSearchConfigReq;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
    CsrUint32                   searchConfig;
    CsrUint32                   readNameTimeout;
    CsrUint32                   maxSearchResults;
} CsrBtSdSearchConfigCfm;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrSchedQid                 phandle;
    CsrBtDeviceAddr             deviceAddr;
    CsrBtAddressType            addressType; /* public/private addresses for LE */
    CsrUint16                   context; /* Opaque context number returned in CsrBtSdReadDeviceInfoCfm */
} CsrBtSdReadDeviceInfoReq;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
    CsrBtDeviceAddr             deviceAddr;
    CsrBtClassOfDevice          deviceClass;
    CsrUint32                   infoLen;
    CsrUint8                   *info;
    CsrUint32                   deviceStatus;
    CsrUint16                   context; /* Opaque context number from CsrBtSdReadDeviceInfoReq */
    CsrBtAddressType            addressType; /* public/private addresses for LE */
    CsrUint32                   serviceChangedHandle; /* For internal use only */
    CsrUint16                   clientConfigValue; /* For internal use only */
    CsrUint16                   startHandle; /* For internal use only */
    CsrUint16                   endHandle; /* For internal use only */
} CsrBtSdReadDeviceInfoCfm;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrSchedQid                 phandle;
    CsrUint32                   searchConfig;
    CsrUint32                   rssiBufferTime;
    CsrUint32                   totalSearchTime;
    CsrInt8                     rssiThreshold;
    CsrBtClassOfDevice          deviceClass;
    CsrBtClassOfDevice          deviceClassMask;
    CsrUint24                   inquiryAccessCode;
    CsrUint32                   filterLen;          /* Reserved for future use. Set to 0 */
    CsrUint8                   *filter;            /* Reserved for future use. Set to NULL */
    CsrInt8                     inquiryTxPowerLevel;
    CsrInt8                     leRssiThreshold;    /* RSSI threshold for low energy radio */
} CsrBtSdSearchReq;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrSchedQid                 phandle;
} CsrBtSdCancelSearchReq;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrBtDeviceAddr             deviceAddr;
    CsrBtClassOfDevice          deviceClass;
    CsrInt8                     rssi;
    CsrUint32                   infoLen;
    CsrUint8                   *info;
    CsrUint32                   deviceStatus;
    CsrBtAddressType            addressType;
} CsrBtSdSearchResultInd;

typedef CsrBtSdSearchResultInd  CsrBtSdImmediateSearchResultInd;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtSdCloseSearchInd;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrSchedQid                 phandle;
    CsrBtDeviceAddr             deviceAddr;
    CsrUint32                   serviceConfig;
    CsrUint32                   filterLen;      /* Reserved for future use. Set to 0 */
    CsrUint8                   *filter;         /* Reserved for future use. Set to NULL */
} CsrBtSdReadAvailableServicesReq;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrSchedQid                 phandle;
    CsrBtDeviceAddr             deviceAddr;
    CsrBtAddressType            addressType;
    CsrUint32                   serviceConfig;
    CsrUint32                   filterLen;      /* Reserved for future use. Set to 0 */
    CsrUint8                   *filter;         /* Reserved for future use. Set to NULL */
} CsrBtSdReadServicesReq;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
    CsrBtDeviceAddr             deviceAddr;
    CsrBtClassOfDevice          deviceClass;
    CsrUint32                   infoLen;
    CsrUint8                   *info;
    CsrUint32                   deviceStatus;
} CsrBtSdReadAvailableServicesCfm;


typedef struct {
    CsrBtSdPrim                 type;
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
    CsrBtDeviceAddr             deviceAddr;
    CsrBtClassOfDevice          deviceClass;
    CsrUint16                   listCount;
    CsrBtUuid                  *list;
    CsrUint32                   deviceStatus;
    CsrBtAddressType            addressType;
} CsrBtSdReadServicesCfm;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrSchedQid                 phandle;
} CsrBtSdCancelReadAvailableServicesReq;

/* Read (all) services primitive is equivalent to read available services */
typedef CsrBtSdCancelReadAvailableServicesReq CsrBtSdReadServicesCancelReq;

typedef struct
{
    CsrBtSdPrim                 type;
} CsrBtSdCancelReadAvailableServicesCfm;

/* Read (all) services primitive is equivalent to read available services */
typedef CsrBtSdCancelReadAvailableServicesCfm CsrBtSdReadServicesCancelCfm;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrSchedQid                 phandle;
    CsrUint32                   maxNumOfBytesInEachResult;
    CsrUint32                   deviceListConfig;
} CsrBtSdReadDeviceListReq;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrUint32                   deviceInfoCount;
    CsrBtSdDeviceInfoType      *deviceInfo;
} CsrBtSdReadDeviceListInd;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrUint32                   totalNumOfDevices;
    CsrUint32                   deviceInfoCount;
    CsrBtSdDeviceInfoType      *deviceInfo;
} CsrBtSdReadDeviceListCfm;

typedef struct
{
    CsrBtSdPrim                 type;
} CsrBtSdStartSavedSearchesReq;

typedef struct
{
    CsrBtSdPrim                 type;
} CsrBtSdStartSavedReadAvailableServicesReq;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrSchedQid                 phandle;
    CsrBtDeviceAddr             deviceAddr;
    CsrUint32                   flags;                    /* Reserved for future use */
    CsrUint16                   dataLen;
    CsrUint8                   *data;
} CsrBtSdReadServiceRecordReq;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrSchedQid                 phandle;
} CsrBtSdCancelReadServiceRecordReq;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrUint16                   dataLen;
    CsrUint8                   *data;
} CsrBtSdReadServiceRecordInd;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtSdReadServiceRecordCfm;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrSchedQid                 phandle;
    CsrUint32                   flags;                    /* Reserved for future use */
    CsrUint16                   dataLen;
    CsrUint8                   *data;
} CsrBtSdRegisterServiceRecordReq;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrUint32                   serviceHandle;
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtSdRegisterServiceRecordCfm;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrSchedQid                 phandle;
    CsrUint32                   flags;                    /* Reserved for future use */
    CsrUint32                   serviceHandle;
} CsrBtSdUnregisterServiceRecordReq;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrUint32                   serviceHandle;
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
} CsrBtSdUnregisterServiceRecordCfm;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrSchedQid                 phandle;
    CsrUint32                   cleanupMode;
} CsrBtSdCleanupReq;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrBtResultCode             resultCode;
    CsrBtSupplier               resultSupplier;
    CsrUint32                   cleanupMode;
} CsrBtSdCleanupCfm;

 typedef struct
{
    CsrBtSdPrim                 type;
    CsrSchedQid                 phandle;
} CsrBtSdSynchronizeReq;

typedef struct
{
    CsrBtSdPrim                 type;
    CsrUint32                   totalNumOfDevices;
} CsrBtSdSynchronizeCfm;

typedef struct
{
    CsrBtSdPrim                 type;
} CsrBtSdHouseCleaning;

#ifdef __cplusplus
}
#endif

#endif

