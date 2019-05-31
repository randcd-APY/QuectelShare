#ifndef CSR_BT_SD_LIB_H
#define CSR_BT_SD_LIB_H
/******************************************************************************
 Copyright (c) 2009-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

 REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_synergy.h"

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_util.h"
#include "csr_bt_profiles.h"
#include "csr_bt_sd_prim.h"
#include "csr_bt_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtSdMsgTransport(void* msg);

#ifdef CSR_BT_INSTALL_SD_MEMORY_CONFIG
/*----------------------------------------------------------------------------*
 *  NAME
 *        CsrBtSdMemoryConfigReqSend
 *
 *  DESCRIPTION
 *      Use to change the memory-related behaviour of the SD
 *
 *  PARAMETERS
 *      phandle: Handle of the requesting application
 *
 *      memoryConfig: Configuration bitmask specifying memory-related behaviour of
 *                    the SD. Must be set to CSR_BT_SD_MEMORY_CONFIG_USE_STANDARD for now.
 *
 *      deviceListMax: Maximum number of devices in the device filter. Must be
 *                       set to CSR_BT_SD_MEMORY_LIST_MAX_DEFAULT for now.
 *
 *      deviceListInfoMax: Maximum number of devices to store extended info for.
 *----------------------------------------------------------------------------*/
#define CsrBtSdMemoryConfigReqSend(_phandle, _memoryConfig, _deviceListMax, _deviceListInfoMax) { \
        CsrBtSdMemoryConfigReq *msg__;                                  \
        msg__                    = (CsrBtSdMemoryConfigReq *) CsrPmemAlloc(sizeof(CsrBtSdMemoryConfigReq)); \
        msg__->type              = CSR_BT_SD_MEMORY_CONFIG_REQ;         \
        msg__->phandle           = _phandle;                            \
        msg__->memoryConfig      = _memoryConfig;                       \
        msg__->deviceListMax     = _deviceListMax;                      \
        msg__->deviceListInfoMax = _deviceListInfoMax;                  \
        CsrBtSdMsgTransport(msg__);}
#endif

#ifdef CSR_BT_INSTALL_SD_SEARCH_CONFIG
/*----------------------------------------------------------------------------*
 *  NAME
 *        CsrBtSdSearchConfigReqSend
 *
 *  DESCRIPTION
 *      Configures the search parameters used in the SD.
 *
 *  PARAMETERS
 *      phandle: Handle of the requesting application
 *
 *      searchConfig: Configuration bitmask for the search request. Allowed values are
 *                    defined in csr_bt_sd_prim.h
 *
 *      readRemoteNameTimeout: Timeout in milliseconds for remote name requests
 *
 *      maxSearchResults: Maximum number of CSR_BT_SD_SEARCH_RESULT_IND to send to the
 *                        application before search is cancelled. 0 = unlimited.
 *----------------------------------------------------------------------------*/
#define CsrBtSdSearchConfigReqSend(_phandle, _searchConfig, _readNameTimeout, _maxSearchResults) { \
        CsrBtSdSearchConfigReq *msg__;                                  \
        msg__                    = (CsrBtSdSearchConfigReq *) CsrPmemAlloc(sizeof(CsrBtSdSearchConfigReq)); \
        msg__->type              = CSR_BT_SD_SEARCH_CONFIG_REQ;         \
        msg__->phandle           = _phandle;                            \
        msg__->searchConfig      = _searchConfig;                       \
        msg__->readNameTimeout   = _readNameTimeout;                    \
        msg__->maxSearchResults  = _maxSearchResults;                   \
        CsrBtSdMsgTransport(msg__);}
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *        CsrBtSdReadDeviceInfoReqSend
 *
 *  DESCRIPTION
 *      Returns all information that the SD knows about a specific device
 *
 *  PARAMETERS
 *      phandle: Handle of the requesting application
 *
 *      deviceAddr: Address of the remote device
 *----------------------------------------------------------------------------*/
#define CsrBtSdReadDeviceInfoReqSendEx(_phandle,_deviceAddr,_addressType,_context) { \
        CsrBtSdReadDeviceInfoReq *msg__  = (CsrBtSdReadDeviceInfoReq*)CsrPmemAlloc(sizeof(CsrBtSdReadDeviceInfoReq)); \
        msg__->type = CSR_BT_SD_READ_DEVICE_INFO_REQ;                   \
        msg__->deviceAddr = _deviceAddr;                                \
        msg__->phandle = _phandle;                                      \
        msg__->addressType = _addressType;                              \
        msg__->context     = _context;                                  \
        CsrBtSdMsgTransport(msg__);}

#define CsrBtSdReadDeviceInfoReqSend(_phandle, _deviceAddr) CsrBtSdReadDeviceInfoReqSendEx(_phandle, _deviceAddr, CSR_BT_ADDR_PUBLIC, 0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSdSearchReqSend / CsrBtSdProximitySearchReqSend
 *
 *  DESCRIPTION
 *      Use the Discovery Service (SD) module to search for other devices
 *
 *  PARAMETERS
 *      phandle: Handle of the requesting application
 *
 *      searchConfig: Configuration bitmask for the search request. Allowed values are
 *                    defined in csr_bt_sd_prim.h
 *
 *      rssiBufferTime: Timeout for RSSI-buffering in milliseconds
 *
 *      totalSearchTime: Time in milliseconds to search for remote devices before search is cancelled
 *
 *      rssiThreshold: Defines a threshold RSSI value that a search result must meet in
 *                     order to result in a CSR_BT_SD_SEARCH_RESULT_IND. The value CSR_BT_SD_RSSI_THRESHOLD_DONT_CARE
 *                     can be sent to ignore this parameter.
 *
 *      deviceClass: Device class of devices that should be sent sent to the application
 *
 *      deviceClassMask: Significant bits of the deviceClass
 *
 *      inquiryAccessCode: Inquiry access code to search for. Must be set to CSR_BT_SD_ACCESS_CODE_GIAC
 *
 *      filterLen: Must be set to 0
 *
 *      filter: Must be set to NULL
 *
 *      inquiryTxPowerLevel: TX power level to use during inquiry - and inquiry only, i.e. all other traffic with use
 *                           the TX power level necessary to maintain any current connections.
 *                           The valid range for this parameter is -70 to +20.
 *
 *      leRssiThreshold: as rssiThreshold, but used for the LE radio
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSdSearchReqSend(_phandle, _searchConfig, _rssiBufferTime, _totalSearchTime, _rssiThreshold, _deviceClass, _deviceClassMask, _inquiryAccessCode, _filterLen, _filter) { \
        CsrBtSdSearchReq *msg__;                                        \
        msg__                        = (CsrBtSdSearchReq *) CsrPmemAlloc(sizeof(CsrBtSdSearchReq)); \
        msg__->type                  = CSR_BT_SD_SEARCH_REQ;            \
        msg__->phandle               = _phandle;                        \
        msg__->searchConfig          = _searchConfig;                   \
        msg__->rssiBufferTime        = _rssiBufferTime;                 \
        msg__->totalSearchTime       = _totalSearchTime;                \
        msg__->rssiThreshold         = _rssiThreshold;                  \
        msg__->deviceClass           = _deviceClass;                    \
        msg__->deviceClassMask       = _deviceClassMask;                \
        msg__->inquiryAccessCode     = _inquiryAccessCode;              \
        msg__->filterLen             = _filterLen;                      \
        msg__->filter                = _filter;                         \
        msg__->inquiryTxPowerLevel   = CSR_BT_SD_DEFAULT_INQUIRY_TX_POWER_LEVEL; \
        msg__->leRssiThreshold       = _rssiThreshold;                  \
        CsrBtSdMsgTransport(msg__);}

#define CsrBtSdProximitySearchReqSend(_phandle, _searchConfig, _rssiBufferTime, _totalSearchTime, _rssiThreshold, _deviceClass, _deviceClassMask, _inquiryAccessCode, _filterLen, _filter, _inquiryTxPowerLevel) { \
        CsrBtSdSearchReq *msg__;                                        \
        msg__                        = (CsrBtSdSearchReq *) CsrPmemAlloc(sizeof(CsrBtSdSearchReq)); \
        msg__->type                  = CSR_BT_SD_SEARCH_REQ;            \
        msg__->phandle               = _phandle;                        \
        msg__->searchConfig          = _searchConfig;                   \
        msg__->rssiBufferTime        = _rssiBufferTime;                 \
        msg__->totalSearchTime       = _totalSearchTime;                \
        msg__->rssiThreshold         = _rssiThreshold;                  \
        msg__->deviceClass           = _deviceClass;                    \
        msg__->deviceClassMask       = _deviceClassMask;                \
        msg__->inquiryAccessCode     = _inquiryAccessCode;              \
        msg__->filterLen             = _filterLen;                      \
        msg__->filter                = _filter;                         \
        msg__->inquiryTxPowerLevel   = _inquiryTxPowerLevel;            \
        msg__->leRssiThreshold       = _rssiThreshold;                  \
        CsrBtSdMsgTransport(msg__);}

#define CsrBtSdSearchReqSendEx(_phandle, _searchConfig, _rssiBufferTime, _totalSearchTime, _rssiThreshold, _deviceClass, _deviceClassMask, _inquiryAccessCode, _filterLen, _filter, _leRssiThreshold) { \
        CsrBtSdSearchReq *msg__;                                        \
        msg__                        = (CsrBtSdSearchReq *) CsrPmemAlloc(sizeof(CsrBtSdSearchReq)); \
        msg__->type                  = CSR_BT_SD_SEARCH_REQ;            \
        msg__->phandle               = _phandle;                        \
        msg__->searchConfig          = _searchConfig;                   \
        msg__->rssiBufferTime        = _rssiBufferTime;                 \
        msg__->totalSearchTime       = _totalSearchTime;                \
        msg__->rssiThreshold         = _rssiThreshold;                  \
        msg__->deviceClass           = _deviceClass;                    \
        msg__->deviceClassMask       = _deviceClassMask;                \
        msg__->inquiryAccessCode     = _inquiryAccessCode;              \
        msg__->filterLen             = _filterLen;                      \
        msg__->filter                = _filter;                         \
        msg__->inquiryTxPowerLevel   = CSR_BT_SD_DEFAULT_INQUIRY_TX_POWER_LEVEL; \
        msg__->leRssiThreshold       = _leRssiThreshold;                \
        CsrBtSdMsgTransport(msg__);}

#define CsrBtSdProximitySearchReqSendEx(_phandle, _searchConfig, _rssiBufferTime, _totalSearchTime, _rssiThreshold, _deviceClass, _deviceClassMask, _inquiryAccessCode, _filterLen, _filter, _inquiryTxPowerLevel, _leRssiThreshold) { \
        CsrBtSdSearchReq *msg__;                                        \
        msg__                        = (CsrBtSdSearchReq *) CsrPmemAlloc(sizeof(CsrBtSdSearchReq)); \
        msg__->type                  = CSR_BT_SD_SEARCH_REQ;            \
        msg__->phandle               = _phandle;                        \
        msg__->searchConfig          = _searchConfig;                   \
        msg__->rssiBufferTime        = _rssiBufferTime;                 \
        msg__->totalSearchTime       = _totalSearchTime;                \
        msg__->rssiThreshold         = _rssiThreshold;                  \
        msg__->deviceClass           = _deviceClass;                    \
        msg__->deviceClassMask       = _deviceClassMask;                \
        msg__->inquiryAccessCode     = _inquiryAccessCode;              \
        msg__->filterLen             = _filterLen;                      \
        msg__->filter                = _filter;                         \
        msg__->inquiryTxPowerLevel   = _inquiryTxPowerLevel;            \
        msg__->leRssiThreshold       = _leRssiThreshold;                \
        CsrBtSdMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *        CsrBtSdCancelSearchReqSend
 *
 *  DESCRIPTION
 *      Cancel the search for other devices
 *
 *    PARAMETERS
 *      phandle: Handle of the requesting application
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSdCancelSearchReqSend(_phandle) {                          \
        CsrBtSdCancelSearchReq *msg__;                                  \
        msg__                        = (CsrBtSdCancelSearchReq *) CsrPmemAlloc(sizeof(CsrBtSdCancelSearchReq)); \
        msg__->type                  = CSR_BT_SD_CANCEL_SEARCH_REQ;     \
        msg__->phandle               = _phandle;                        \
        CsrBtSdMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSdReadAvailableServicesReqSend
 *
 *  DESCRIPTION
 *      Use the Discovery Service (SD) module to find the services offered by another devices
 *
 *    PARAMETERS
 *      phandle: Handle of the requesting application
 *
 *      deviceAddr: The address of the remote device
 *
 *      serviceConfig: A bitmask for modifying behaviour of the search. Allowed values
 *                     are specified in csr_bt_sd_prim.h.
 *
 *      filterLen: Must be set to 0
 *
 *      filter: Must be set to NULL
 *----------------------------------------------------------------------------*/
#define CsrBtSdReadAvailableServicesReqSend(_phandle, _deviceAddr, _serviceConfig, _filterLen, _filter) { \
        CsrBtSdReadAvailableServicesReq *msg__;                         \
        msg__                        = (CsrBtSdReadAvailableServicesReq *) CsrPmemAlloc(sizeof(CsrBtSdReadAvailableServicesReq)); \
        msg__->type                  = CSR_BT_SD_READ_AVAILABLE_SERVICES_REQ; \
        msg__->phandle               = _phandle;                        \
        msg__->deviceAddr            = _deviceAddr;                     \
        msg__->serviceConfig         = _serviceConfig;                  \
        msg__->filterLen             = _filterLen;                      \
        msg__->filter                = _filter;                         \
        CsrBtSdMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSdReadServicesReqSendEx
 *
 *  DESCRIPTION
 *      Use the Discovery Service (SD) module to find all services offered by another devices
 *
 *    PARAMETERS
 *      phandle: Handle of the requesting application
 *
 *      deviceAddr: The address of the remote device
 *
 *      addressType: Address type of the remote device
 *
 *      serviceConfig: A bitmask for modifying behaviour of the search. Allowed values
 *                     are specified in csr_bt_sd_prim.h.
 *
 *      filterLen: Must be set to 0
 *
 *      filter: Must be set to NULL
 *----------------------------------------------------------------------------*/
#define CsrBtSdReadServicesReqSendEx(_phandle, _deviceAddr, _addressType, _serviceConfig, _filterLen, _filter) { \
        CsrBtSdReadServicesReq *msg__;                                  \
        msg__ = (CsrBtSdReadServicesReq*)CsrPmemAlloc(sizeof(CsrBtSdReadServicesReq)); \
        msg__->type = CSR_BT_SD_READ_SERVICES_REQ;                      \
        msg__->phandle = _phandle;                                      \
        msg__->deviceAddr = _deviceAddr;                                \
        msg__->addressType = _addressType;                              \
        msg__->serviceConfig = _serviceConfig;                          \
        msg__->filterLen = _filterLen;                                  \
        msg__->filter = _filter;                                        \
        CsrBtSdMsgTransport(msg__);}
#define CsrBtSdReadServicesReqSend(_phandle, _deviceAddr, _serviceConfig, _filterLen, _filter) CsrBtSdReadServicesReqSendEx(_phandle, _deviceAddr, CSR_BT_ADDR_PUBLIC, _serviceConfig, _filterLen, _filter)
/*----------------------------------------------------------------------------*
 *  NAME
 *        CsrBtSdCancelReadAvailableServicesReqSend
 *
 *  DESCRIPTION
 *      Cancels the service search
 *
 *    PARAMETERS
 *      phandle: Handle of the requesting application
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSdCancelReadAvailableServicesReqSend(_phandle) {           \
        CsrBtSdCancelReadAvailableServicesReq *msg__;                   \
        msg__                        = (CsrBtSdCancelReadAvailableServicesReq *) CsrPmemAlloc(sizeof(CsrBtSdCancelReadAvailableServicesReq)); \
        msg__->type                  = CSR_BT_SD_CANCEL_READ_AVAILABLE_SERVICES_REQ; \
        msg__->phandle               = _phandle;                        \
        CsrBtSdMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *        CsrBtSdReadServicesCancelReqSend
 *
 *  DESCRIPTION
 *      Cancels the service search
 *
 *    PARAMETERS
 *      phandle: Handle of the requesting application
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSdReadServicesCancelReqSend(_phandle) {                    \
        CsrBtSdReadServicesCancelReq *msg__;                            \
        msg__ = (CsrBtSdReadServicesCancelReq*)CsrPmemAlloc(sizeof(CsrBtSdReadServicesCancelReq)); \
        msg__->type = CSR_BT_SD_READ_SERVICES_CANCEL_REQ;               \
        msg__->phandle = _phandle;                                      \
        CsrBtSdMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *        CsrBtSdReadDeviceListReqSend
 *
 *  DESCRIPTION
 *      Returns all information that the SD holds about trusted devices
 *
 *    PARAMETERS
 *      phandle: Handle of the requesting application
 *
 *      maxNumOfBytesInEachResult: The maximum number of bytes that is allowed in a
 *                                 message to the calling process.
 *
 *      deviceListConfig: A bitmask defining restrictions on which device type to
 *                        include in the result. Allowed values are defined in csr_bt_sd_prim.h.
 *----------------------------------------------------------------------------*/
#define CsrBtSdReadDeviceListReqSend(_phandle, _maxNumOfBytesInEachResult, _deviceListConfig) { \
        CsrBtSdReadDeviceListReq *msg__;                                \
        msg__                            = (CsrBtSdReadDeviceListReq *) CsrPmemAlloc(sizeof(CsrBtSdReadDeviceListReq)); \
        msg__->type                      = CSR_BT_SD_READ_DEVICE_LIST_REQ; \
        msg__->phandle                   = _phandle;                    \
        msg__->maxNumOfBytesInEachResult = _maxNumOfBytesInEachResult;  \
        msg__->deviceListConfig          = _deviceListConfig;           \
        CsrBtSdMsgTransport(msg__);}

#ifndef EXCLUDE_CSR_BT_SD_SERVICE_RECORD_MODULE
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSdReadServiceRecordReqSend
 *
 *  DESCRIPTION
 *      Read service records
 *
 *    PARAMETERS
 *      phandle:  Handle of the requesting application
 *
 *      deviceAddr: The address of the remote device
 *
 *      flags:    Reserved for furture used
 *
 *      dataLen:  The length of the service record tag list
 *
 *      data:     Pointer to the service record tag list
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSdReadServiceRecordReqSend(_phandle, _deviceAddr, _flags, _dataLen, _data) { \
        CsrBtSdReadServiceRecordReq *msg__;                             \
        msg__ = (CsrBtSdReadServiceRecordReq *) CsrPmemAlloc(sizeof(CsrBtSdReadServiceRecordReq)); \
        msg__->type                  = CSR_BT_SD_READ_SERVICE_RECORD_REQ; \
        msg__->phandle               = _phandle;                        \
        msg__->deviceAddr            = _deviceAddr;                     \
        msg__->flags                 = _flags;                          \
        msg__->dataLen               = _dataLen;                        \
        msg__->data                  = _data;                           \
        CsrBtSdMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSdCancelReadServiceRecordReqSend
 *
 *  DESCRIPTION
 *      Cancel the read service record procedure
 *
 *    PARAMETERS
 *      phandle:  Handle of the requesting application
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSdCancelReadServiceRecordReqSend(_phandle) {               \
        CsrBtSdCancelReadServiceRecordReq *msg__;                       \
        msg__ = (CsrBtSdCancelReadServiceRecordReq *) CsrPmemAlloc(sizeof(CsrBtSdCancelReadServiceRecordReq)); \
        msg__->type                  = CSR_BT_SD_CANCEL_READ_SERVICE_RECORD_REQ; \
        msg__->phandle               = _phandle;                        \
        CsrBtSdMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSdRegisterServiceRecordReqSend
 *
 *  DESCRIPTION
 *      Registers a service record
 *
 *    PARAMETERS
 *      phandle:  Handle of the requesting application
 *
 *      flags:    Reserved for furture used
 *
 *      dataLen:  The length of the service record
 *
 *      data:     Pointer to the service record
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSdRegisterServiceRecordReqSend(_phandle, _flags, _dataLen, _data) { \
        CsrBtSdRegisterServiceRecordReq *msg__;                         \
        msg__ = (CsrBtSdRegisterServiceRecordReq *) CsrPmemAlloc(sizeof(CsrBtSdRegisterServiceRecordReq)); \
        msg__->type                       = CSR_BT_SD_REGISTER_SERVICE_RECORD_REQ; \
        msg__->phandle                    = _phandle;                   \
        msg__->flags                      = _flags;                     \
        msg__->dataLen                    = _dataLen;                   \
        msg__->data                       = _data;                      \
        CsrBtSdMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSdUnregisterServiceRecordReqSend
 *
 *  DESCRIPTION
 *      Unregisters a service record
 *
 *    PARAMETERS
 *      phandle:        Handle of the requesting application
 *
 *      flags:          Reserved for furture used
 *
 *      serviceHandle:  Handle of the service record that must be unregister
 *
 *----------------------------------------------------------------------------*/
#define CsrBtSdUnregisterServiceRecordReqSend(_phandle, _flags, _serviceHandle) { \
        CsrBtSdUnregisterServiceRecordReq *msg__;                       \
        msg__ = (CsrBtSdUnregisterServiceRecordReq *) CsrPmemAlloc(sizeof(CsrBtSdUnregisterServiceRecordReq)); \
        msg__->type                      = CSR_BT_SD_UNREGISTER_SERVICE_RECORD_REQ; \
        msg__->phandle                   = _phandle;                    \
        msg__->flags                     = _flags;                      \
        msg__->serviceHandle             = _serviceHandle;              \
        CsrBtSdMsgTransport(msg__);}
#endif /* EXCLUDE_CSR_BT_SD_SERVICE_RECORD_MODULE*/

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSdCleanupReqSend
 *
 *  DESCRIPTION
 *      Free memory used by the SD
 *
 *    PARAMETERS
 *      phandle:        Handle of the requesting application
 *
 *      cleanupMode:    One of the modes defined by SD_CLEANUP_ defines in csr_bt_sd_prim.h
 *----------------------------------------------------------------------------*/
#define CsrBtSdCleanupReqSend(_phandle, _cleanupMode) {                 \
        CsrBtSdCleanupReq *msg__;                                       \
        msg__ = (CsrBtSdCleanupReq *) CsrPmemAlloc(sizeof(CsrBtSdCleanupReq)); \
        msg__->type          = CSR_BT_SD_CLEANUP_REQ;                   \
        msg__->phandle       = _phandle;                                \
        msg__->cleanupMode   = _cleanupMode;                            \
        CsrBtSdMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      SdSynchronizeReqSend
 *
 *  DESCRIPTION
 *      Force the SD to update the Device List with information from the SCDB
 *
 *    PARAMETERS
 *      phandle:        Handle of the requesting application
 *----------------------------------------------------------------------------*/
#define CsrBtSdSynchronizeReqSend(_phandle) {                           \
        CsrBtSdSynchronizeReq *msg__;                                   \
        msg__ = (CsrBtSdSynchronizeReq *) CsrPmemAlloc(sizeof(CsrBtSdSynchronizeReq)); \
        msg__->type                  = CSR_BT_SD_SYNCHRONIZE_REQ;       \
        msg__->phandle               = _phandle;                        \
        CsrBtSdMsgTransport(msg__);}



/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

typedef struct
{
    CsrBool   specificationIdValid;
    CsrUint16 specificationIdValue;

    CsrBool   vendorIdValid;
    CsrUint16 vendorIdValue;

    CsrBool   productIdValid;
    CsrUint16 productIdValue;

    CsrBool   versionValid;
    CsrUint16 versionValue;

    CsrBool   primaryRecordValid;
    CsrBool   primaryRecordValue;

    CsrBool   vendorIdSourceValid;
    CsrUint16 vendorIdSourceValue;

    CsrBool    clientExecutableUrlValid;
    CsrUint8  *clientExecutableUrlValue;
    CsrUint16  clientExecutableUrlValueLen;

    CsrBool    serviceDescriptionValid;
    CsrUint8  *serviceDescriptionValue;
    CsrUint16  serviceDescriptionValueLen;

    CsrBool    documentationUrlValid;
    CsrUint8  *documentationUrlValue;
    CsrUint16  documentationUrlValueLen;
} CsrBtSdDiServiceRecordV13Struct;

/* Prototypes for Device Identification Service Record handling */
void CsrBtSdInitDiServiceRecordV13Struct(CsrBtSdDiServiceRecordV13Struct *v13);

void CsrBtSdExtractDiServiceRecordV13Data(CsrUint8 *data,
                                          CsrUint16 dataLen,
                                          CsrBtSdDiServiceRecordV13Struct *v13);

void CsrBtSdReadDiServiceRecordV13(CsrSchedQid     apphandle,
                                   CsrBtDeviceAddr  deviceAddr);

void CsrBtSdRegisterDiServiceRecordV13(CsrSchedQid apphandle,
                                       CsrUint16  vendorId,
                                       CsrUint16  productId,
                                       CsrUint16  version,
                                       CsrBool    primaryRecord,
                                       CsrUint16  vendorIdSource,
                                       CsrUint8  *serviceDescription,
                                       CsrUint16  serviceDescriptionLen,
                                       CsrUint8  *clientExecutableUrl,
                                       CsrUint16  clientExecutableUrlLen,
                                       CsrUint8  *documentationUrl,
                                       CsrUint16  documentationUrlLen);

/* Prototypes for misc. lib-functions */
CsrUint8  CsrBtSdDecodeFriendlyName(CsrUint8 *info, CsrUint32 infoLen, CsrUint8 **friendlyName);
CsrBool   CsrBtSdIsServicePresent(CsrUint8 *info, CsrUint32 infoLen, CsrUint32 uuid);
CsrUint16 CsrBtSdReadServiceList(CsrUint8 *info, CsrUint32 infoLen, CsrUint32 **serviceList);
CsrBool CsrBtSdInfoCheckTag(CsrUint16 infoLen, CsrUint8 *info, CsrUint16 tag);
CsrUint16 CsrBtSdInfoGetTag(CsrUint16 infoLen, CsrUint8 *info, CsrUint8 **tagVal, CsrUint16 tag);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtSdFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT SD
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_SD_PRIM,
 *      msg:          The message received from Synergy BT SD
 *----------------------------------------------------------------------------*/
void CsrBtSdFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);



#ifdef __cplusplus
}
#endif

#endif

