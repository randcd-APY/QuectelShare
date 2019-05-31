#ifndef CSR_BT_CM_LIB_H__
#define CSR_BT_CM_LIB_H__

/******************************************************************************

Copyright (c) 2001-2018 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_bt_profiles.h"
#include "csr_types.h"
#include "csr_msg_transport.h"
#include "rfcomm_prim.h"
#include "hci_prim.h"
#include "csr_bt_cm_prim.h"
#include "csr_bt_tasks.h"
#include "csr_bt_addr.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message functions to reduce code size */
void CsrBtCmMsgTransport(void *msg);
void CsrBtCmPutMessageDownstream(void *msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmSetLocalNameReqSend
 *
 *  DESCRIPTION
 *      .....
 *
 *  PARAMETERS
 *        phandle:            protocol handle
 *        friendlyName;        new name of local device
 *----------------------------------------------------------------------------*/
#define CsrBtCmSetLocalNameReqSend(_phandle, _friendlyName) {           \
        CsrBtCmSetLocalNameReq *msg__ = (CsrBtCmSetLocalNameReq*) CsrPmemAlloc(sizeof(CsrBtCmSetLocalNameReq)); \
        msg__->type = CSR_BT_CM_SET_LOCAL_NAME_REQ;                     \
        msg__->phandle = _phandle;                                      \
        msg__->friendlyName = _friendlyName;                            \
        CsrBtCmMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmReadLocalBdAddrReqSend
 *
 *  DESCRIPTION
 *      .....
 *
 *  PARAMETERS
 *        phandle:            protocol handle
 *----------------------------------------------------------------------------*/
CsrBtCmReadLocalBdAddrReq *CsrBtCmReadLocalBdAddrReq_struct(CsrSchedQid    thePhandle);

#define CsrBtCmReadLocalBdAddrReqSend(_phandle) {               \
        CsrBtCmReadLocalBdAddrReq *__msg;                       \
        __msg=CsrBtCmReadLocalBdAddrReq_struct(_phandle);       \
        CsrBtCmMsgTransport(__msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmReadLocalNameReqSend
 *
 *  DESCRIPTION
 *      .....
 *
 *  PARAMETERS
 *        phandle:            protocol handle
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_CM_READ_LOCAL_NAME 
CsrBtCmReadLocalNameReq *CsrBtCmReadLocalNameReq_struct(CsrSchedQid    thePhandle);

#define CsrBtCmReadLocalNameReqSend(_phandle) {         \
        CsrBtCmReadLocalNameReq *__msg;                 \
        __msg=CsrBtCmReadLocalNameReq_struct(_phandle); \
        CsrBtCmMsgTransport(__msg);}
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmReadRemoteNameReqSend
 *
 *  DESCRIPTION
 *      .....
 *
 *  PARAMETERS
 *        phandle:            protocol handle
 *        deviceAddr:            BT address of the device to read remote name
 *----------------------------------------------------------------------------*/
void CsrBtCmReadRemoteNameReqSend(CsrSchedQid       thePhandle,
                                  CsrBtDeviceAddr    theDeviceAddr);




/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmReadRemoteVersionReqSend
 *
 *  DESCRIPTION
 *      .....
 *
 *  PARAMETERS
 *        phandle:            protocol handle
 *        deviceAddr:            BT address of the device to read remote version
 *----------------------------------------------------------------------------*/
void CsrBtCmReadRemoteVersionReqSend(CsrSchedQid       thePhandle,
                                     CsrBtDeviceAddr    theDeviceAddr,
                                     CsrBtAddressType   theAddressType,
                                     CsrBtTransportType thetransportType);


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmAclDetachReqSend
 *
 *  DESCRIPTION
 *      .....
 *
 *  PARAMETERS
 *      phandle:               protocol handle
 *      deviceAddr:            BT address of the device acl to detach
 *      flags:                 Set to CM_ACL_FLAG_ALL in order to always detach. Alternatives can
 *                             be found in csr_bt_cm_prim.h
 *----------------------------------------------------------------------------*/
void CsrBtCmAclDetachReqSendEx(CsrSchedQid thePhandle,
                               CsrBtDeviceAddr  theDeviceAddr,
                               CsrBtCmDetach flags,
                               CsrBtAddressType addressType);

#define CsrBtCmAclDetachReqSend(_phandle,_deviceAddr, _flags) CsrBtCmAclDetachReqSendEx(_phandle,_deviceAddr, _flags, CSR_BT_ADDR_PUBLIC)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtcmWriteLinkSuperVisionTimeoutReqSend
 *
 *  DESCRIPTION
 *      This command will write the value of the link supervision timeout
 *        parameter. It is used to monitor link loss. Value of 0x0000 disables it.
 *        The timeout value N range from 0x0001 - 0xffff. In seconds it means from
 *        N*0.625ms = 0,625ms - 40.9 seconds.
 *
 *  PARAMETERS
 *        phandle:            protocol handle
 *        CsrBtDeviceAddr        theDeviceAddr
 *        CsrUint16            Link supervision timeout value.
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_CM_WRITE_LINK_SUPERVISION_TIMEOUT
CsrBtCmWriteLinkSupervTimeoutReq *CsrBtCmWriteLinkSuperVisionTimeoutReq_struct(CsrSchedQid       thePhandle,
                                                                               CsrBtDeviceAddr    theDeviceAddr,
                                                                               CsrUint16        timeout);

#define CsrBtcmWriteLinkSuperVisionTimeoutReqSend(_phandle,_deviceAddr,_timeout) { \
        CsrBtCmWriteLinkSupervTimeoutReq *__msg;                        \
        __msg=CsrBtCmWriteLinkSuperVisionTimeoutReq_struct(_phandle,_deviceAddr,_timeout); \
        CsrBtCmMsgTransport(__msg);}
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmReadTxPowerLevelReqSend
 *
 *  DESCRIPTION
 *      .....
 *
 *  PARAMETERS
 *        phandle:            protocol handle
 *        deviceAddr:       BT address of the device to read remote name
 *        levelType:         The maximum power level as defined in the Bluetooth HCI
 *                                specification.
 *        addressType:     Address type of 'deviceAddr' (see CSR_BT_ADDR_ defines
 *                                in csr_bt_addr.h)
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_CM_READ_TX_POWER_LEVEL
CsrBtCmReadTxPowerLevelReq *CsrBtCmReadTxPowerLevelReq_struct(CsrSchedQid thePhandle,
                                                              CsrBtDeviceAddr theDeviceAddr,
                                                              CsrBtAddressType theAddressType,
                                                              CsrBtTransportType theTransportType,
                                                              CsrUint8 theLevelType);

#define CsrBtCmReadTxPowerLevelReqSendEx(_phandle,_deviceAddr,_addressType, _transportType, _levelType) { \
        CsrBtCmReadTxPowerLevelReq *__msg;                              \
        __msg=CsrBtCmReadTxPowerLevelReq_struct(_phandle,_deviceAddr,_addressType, _transportType, _levelType); \
        CsrBtCmMsgTransport(__msg);}

#define CsrBtCmReadTxPowerLevelReqSend(_phandle,_deviceAddr,_levelType) \
        CsrBtCmReadTxPowerLevelReqSendEx(_phandle,                      \
                                         _deviceAddr,                   \
                                         CSR_BT_ADDR_PUBLIC,            \
                                         CSR_BT_TRANSPORT_BREDR,        \
                                         _levelType)
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmGetLinkQualityReqSend
 *
 *  DESCRIPTION
 *      .....
 *
 *  PARAMETERS
 *        phandle:            protocol handle
 *        deviceAddr:         BT address of the device to read remote name
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_CM_GET_LINK_QUALITY
#define CsrBtCmGetLinkQualityReqSend(_phandle,_deviceAddr) {            \
        CsrBtCmGetLinkQualityReq *__msg = (CsrBtCmGetLinkQualityReq*) CsrPmemAlloc(sizeof(CsrBtCmGetLinkQualityReq)); \
        __msg->type = CSR_BT_CM_GET_LINK_QUALITY_REQ;                   \
        __msg->appHandle = _phandle;                                    \
        __msg->deviceAddr = _deviceAddr;                                \
        CsrBtCmMsgTransport(__msg);}
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmReadRssiReqSend
 *
 *  DESCRIPTION
 *      .....
 *
 *  PARAMETERS
 *        phandle:            protocol handle
 *        deviceAddr:       BT address of the device to read remote name
 *        addressType:     Address type of 'deviceAddr' (see CSR_BT_ADDR_ defines
 *                                in csr_bt_addr.h)
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_CM_READ_RSSI
#define CsrBtCmReadRssiReqSendEx(_phandle,_deviceAddr,_addressType, _transportType) {   \
        CsrBtCmReadRssiReq *__msg = (CsrBtCmReadRssiReq*) CsrPmemAlloc(sizeof(CsrBtCmReadRssiReq)); \
        __msg->type = CSR_BT_CM_READ_RSSI_REQ;                          \
        __msg->appHandle = _phandle;                                    \
        __msg->deviceAddr = _deviceAddr;                                \
        __msg->addressType = _addressType;                              \
        __msg->transportType = _transportType;                              \
        CsrBtCmMsgTransport(__msg);}

#define CsrBtCmReadRssiReqSend(_phandle, _deviceAddr)    \
                CsrBtCmReadRssiReqSendEx(_phandle, _deviceAddr, CSR_BT_ADDR_PUBLIC, CSR_BT_TRANSPORT_BREDR)
#endif
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmWriteMajorMinorCodReqSend
 *
 *  DESCRIPTION
 *        ......
 *
 *  PARAMETERS
 *      phandle:   protocol handle
 *      service:   A Class of device value given from the application
 *      major:     A Major Class of device value given from the application
 *      minor:     A Minor Class of device value given from the application
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_CM_WRITE_COD
CsrBtCmWriteCodReq *CsrBtCmWriteCodReq_struct(CsrSchedQid phandle,
                                              CsrBtCmUpdateFlags updateFlags,
                                              CsrBtClassOfDevice   service,
                                              CsrBtClassOfDevice   major,
                                              CsrBtClassOfDevice   minor);

#define CsrBtCmWriteMajorMinorCodReqSend(_phandle, _major, _minor) {    \
        CsrBtCmWriteCodReq *__msg;                                      \
        __msg=CsrBtCmWriteCodReq_struct(_phandle, CSR_BT_CM_WRITE_COD_UPDATE_FLAG_MAJOR_MINOR_CLASS, 0, _major, _minor); \
        CsrBtCmMsgTransport(__msg);}
#endif
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmWriteServiceCodReqSend
 *
 *  DESCRIPTION
 *        ......
 *
 *  PARAMETERS
 *        phandle:    protocol handle
 *        service:    A Class of device value given from the application
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_CM_WRITE_COD
#define CsrBtCmWriteServiceCodReqSend(_phandle,_service) {              \
        CsrBtCmWriteCodReq *__msg;                                      \
        __msg=CsrBtCmWriteCodReq_struct(_phandle, CSR_BT_CM_WRITE_COD_UPDATE_FLAG_SERVICE_CLASS, _service, 0, 0); \
        CsrBtCmMsgTransport(__msg);}
#endif
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmReadCodReqSend
 *
 *  DESCRIPTION
 *        ......
 *
 *  PARAMETERS
 *        phandle             :    protocol handle
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_CM_READ_COD
#define CsrBtCmReadCodReqSend(_phandle) {                               \
        CsrBtCmReadCodReq *__msg = (CsrBtCmReadCodReq*) CsrPmemAlloc(sizeof(CsrBtCmReadCodReq)); \
        __msg->type = CSR_BT_CM_READ_COD_REQ;                           \
        __msg->appHandle = _phandle;                                    \
        CsrBtCmMsgTransport(__msg);}
#endif
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmWriteScanEnableReqSend
 *
 *  DESCRIPTION
 *        ......
 *
 *  PARAMETERS
 *        phandle:            protocol handle
 *        disableInquiryScan:    TRUE disable inquiry scan
 *      disablePageScan:    TRUE disable page scan
 *----------------------------------------------------------------------------*/
#define CsrBtCmWriteScanEnableReqSend(_phandle,_disableInquiryScan, _disablePageScan) { \
        CsrBtCmWriteScanEnableReq *__msg = (CsrBtCmWriteScanEnableReq*) CsrPmemAlloc(sizeof(CsrBtCmWriteScanEnableReq)); \
        __msg->type = CSR_BT_CM_WRITE_SCAN_ENABLE_REQ;                  \
        __msg->appHandle = _phandle;                                    \
        __msg->disableInquiryScan = _disableInquiryScan;                \
        __msg->disablePageScan = _disablePageScan;                      \
        CsrBtCmMsgTransport(__msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmReadScanEnableReqSend
 *
 *  DESCRIPTION
 *        ......
 *
 *  PARAMETERS
 *        phandle             :    protocol handle
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_CM_READ_SCAN_EANBLE
#define CsrBtCmReadScanEnableReqSend(_phandle) {                        \
        CsrBtCmReadScanEnableReq *msg__ = (CsrBtCmReadScanEnableReq*) CsrPmemAlloc(sizeof(CsrBtCmReadScanEnableReq)); \
        msg__->type = CSR_BT_CM_READ_SCAN_ENABLE_REQ;                   \
        msg__->appHandle = _phandle;                                    \
        CsrBtCmMsgTransport(msg__);}
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmSdcSearchReqSend
 *      CsrBtCmSdcSearchExtReqSend
 *
 *  DESCRIPTION
 *      Submits a search request to the SDC sub-system
 *
 *  PARAMETERS
 *      appHandle:           protocol handle
 *      deviceAddr:          BT address of the device to read remote name
 *      serviceList:         A list of Services (UUID) to search for
 *      serviceListSize:     Number of services to search for
 *      extendedUuidSearch:  Defines when a UUID must be consider valid
 *----------------------------------------------------------------------------*/
void CsrBtCmSdcSearchReqSendFunc(CsrSchedQid     appHandle,
                                 CsrBtDeviceAddr deviceAddr,
                                 CsrBtUuid32    *serviceList,
                                 CsrUint8        serviceListSize,
                                 CsrBool         extendedUuidSearch);

#define CsrBtCmSdcSearchReqSend(_phandle,_deviceAddr,_serviceList,_serviceListSize) CsrBtCmSdcSearchReqSendFunc(_phandle, _deviceAddr, _serviceList, _serviceListSize, FALSE)

#define CsrBtCmSdcSearchExtReqSend(_phandle,_deviceAddr,_serviceList,_serviceListSize) CsrBtCmSdcSearchReqSendFunc(_phandle, _deviceAddr, _serviceList, _serviceListSize, TRUE)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmSdcCancelSearchReqSend
 *
 *  DESCRIPTION
 *      Cancel a CSR_BT_CM_SDC_SEARCH_REQ
 *
 *  PARAMETERS
 *        appHandle:             protocol handle
 *        deviceAddr:             BT address of the device to read remote name
 *----------------------------------------------------------------------------*/
#define CsrBtCmSdcCancelSearchReqSend(_appHandle,_deviceAddr) {         \
        CsrBtCmSdcCancelSearchReq *__msg = (CsrBtCmSdcCancelSearchReq*) CsrPmemAlloc(sizeof(CsrBtCmSdcCancelSearchReq)); \
        __msg->type = CSR_BT_CM_SDC_CANCEL_SEARCH_REQ;                  \
        __msg->appHandle = _appHandle;                                  \
        __msg->deviceAddr = _deviceAddr;                                \
        __msg->typeToCancel = CSR_BT_CM_SDC_SEARCH_REQ;                 \
        CsrBtCmMsgTransport(__msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmSdcUuid128SearchReqSend
 *
 *  DESCRIPTION
 *      Submits a search request with a 128 bit uuid16_t to the SDC sub-system
 *
 *  PARAMETERS
 *        appHandle:             protocol handle
 *        deviceAddr:             BT address of the device to read remote name
 *        serviceList:         A list of 128 bit Services (UUID128) to search for
 *      serviceListSize:     Number of services of 128 bit to search for
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_128_BIT_SERVICE_SEARCH
CsrBtCmSdcUuid128SearchReq *CsrBtCmSdcUuid128SearchReq_struct(CsrSchedQid       appHandle,
                                                              CsrBtDeviceAddr    deviceAddr,
                                                              CsrBtUuid128       *serviceList,
                                                              CsrUint8         serviceListSize);

#define CsrBtCmSdcUuid128SearchReqSend(_appHandle,_deviceAddr,_serviceList,_serviceListSize) { \
        CsrBtCmSdcUuid128SearchReq *__msg;                              \
        __msg=CsrBtCmSdcUuid128SearchReq_struct(_appHandle,_deviceAddr,_serviceList,_serviceListSize); \
        CsrBtCmMsgTransport(__msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmSdcCancelUuid128SearchReqSend
 *
 *  DESCRIPTION
 *      Cancel a CSR_BT_CM_SDC_UUID128_SEARCH_REQ
 *
 *  PARAMETERS
 *        appHandle:             protocol handle
 *        deviceAddr:             BT address of the device to read remote name
 *----------------------------------------------------------------------------*/
#define CsrBtCmSdcCancelUuid128SearchReqSend(_appHandle,_deviceAddr) {  \
        CsrBtCmSdcCancelSearchReq *__msg = (CsrBtCmSdcCancelSearchReq*) CsrPmemAlloc(sizeof(CsrBtCmSdcCancelSearchReq)); \
        __msg->type = CSR_BT_CM_SDC_CANCEL_SEARCH_REQ;                  \
        __msg->appHandle = _appHandle;                                  \
        __msg->deviceAddr = _deviceAddr;                                \
        __msg->typeToCancel = CSR_BT_CM_SDC_UUID128_SEARCH_REQ;         \
        CsrBtCmMsgTransport(__msg);}
#endif
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmSdcAttributeReqSend
 *      CsrBtCmSdcAttributeRangeReqSend
 *
 *  DESCRIPTION
 *      Submits an attribute request to the SDC sub-system
 *
 *  PARAMETERS
 *        serviceHandle:                The handle of the service
 *        attributeIdentifier:          The attribute to retrieve. Note, if
 *                                      CsrBtCmSdcAttributeRangeReqSend is used
 *                                      then it defines the beginning attributeId
 *                                      of the range
 *        upperRangeAttributeIdentifier:The ending attributeId of the range
 *        maxBytesToReturn:             The maximum number of attribute bytes
 *                                      to be returned
 *----------------------------------------------------------------------------*/
void CsrBtCmSdcAttributeRangeReqSend(CsrBtUuid32 serviceHandle,
                                     CsrUint16   attributeIdentifier,
                                     CsrUint16   upperRangeAttributeIdentifier,
                                     CsrUint16   maxBytesToReturn);

#define CsrBtCmSdcAttributeReqSend(_serviceHandle,_attributeIdentifier,_maxBytesToReturn) CsrBtCmSdcAttributeRangeReqSend(_serviceHandle,_attributeIdentifier,0,_maxBytesToReturn)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmSdcCloseReqSend
 *
 *  DESCRIPTION
 *      Closing the SDC channel
 *
 *  PARAMETERS
 *        appHandle:            protocol handle
 *----------------------------------------------------------------------------*/
void CsrBtCmSdcCloseReqSend(CsrSchedQid   appHandle);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmSdsRegisterReqSend
 *
 *  DESCRIPTION
 *      Request to register a service with the service discovery server
 *
 *  PARAMETERS
 *        appHandle:            protocol handle
 *        serviceRecord:        The service record
 *        serviceRecordSize:    Size of the service record
 *----------------------------------------------------------------------------*/
void CsrBtCmSdsRegisterReqExtSend(CsrSchedQid      appHandle,
                                  CsrUint8    *serviceRecord,
                                  CsrUint16   serviceRecordSize,
                                  CsrUint16   context);

#define CsrBtCmSdsRegisterReqSend(_appHandle,_serviceRecord,_serviceRecordSize) CsrBtCmSdsRegisterReqExtSend(_appHandle,_serviceRecord,_serviceRecordSize, CSR_BT_CM_CONTEXT_UNUSED)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmSdsUnRegisterReqSend
 *
 *  DESCRIPTION
 *      Request to unregister a service with the service discovery server
 *
 *  PARAMETERS
 *        appHandle:            protocol handle
 *        serviceRecHandle:    The service record handle
 *----------------------------------------------------------------------------*/
void CsrBtCmSdsUnRegisterExtReqSend(CsrSchedQid      appHandle,
                                    CsrUint32   serviceRecHandle,
                                    CsrUint16   context);

#define CsrBtCmSdsUnRegisterReqSend(_appHandle,_serviceRecHandle) CsrBtCmSdsUnRegisterExtReqSend(_appHandle,_serviceRecHandle, CSR_BT_CM_CONTEXT_UNUSED)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmEnableDutModeReqSend
 *
 *  DESCRIPTION
 *        ......
 *
 *  PARAMETERS
 *        phandle:        protocol handle
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_CM_DUT_MODE
#define CsrBtCmEnableDutModeReqSend(_phandle) {                         \
        CsrBtCmEnableDutModeReq *__msg = (CsrBtCmEnableDutModeReq*) CsrPmemAlloc(sizeof(CsrBtCmEnableDutModeReq)); \
        __msg->type = CSR_BT_CM_ENABLE_DUT_MODE_REQ;                    \
        __msg->appHandle = _phandle;                                     \
        CsrBtCmMsgTransport(__msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmDisableDutModeReqSend
 *
 *  DESCRIPTION
 *        ......
 *
 *  PARAMETERS
 *        phandle:        protocol handle
 *----------------------------------------------------------------------------*/
#define CsrBtCmDisableDutModeReqSend(_phandle) {                        \
        CsrBtCmDisableDutModeReq *__msg = (CsrBtCmDisableDutModeReq *) CsrPmemAlloc(sizeof(CsrBtCmDisableDutModeReq)); \
        __msg->type = CSR_BT_CM_DISABLE_DUT_MODE_REQ;                   \
        __msg->appHandle = _phandle;                                    \
        CsrBtCmMsgTransport(__msg);}
#endif
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmConnectableReqSend
 *
 *  DESCRIPTION
 *        ......
 *
 *  PARAMETERS
 *        phandle        :    protocol handle
 *        connectAble :    Set to TRUE to be inform about rejected RFCOMM connection
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_CM_CONNECTABLE
#define CsrBtCmConnectableReqSend(_phandle,_connectAble) {              \
        CsrBtCmConnectableReq *__msg = (CsrBtCmConnectableReq*) CsrPmemAlloc(sizeof(CsrBtCmConnectableReq)); \
        __msg->type = CSR_BT_CM_CONNECTABLE_REQ;                        \
        __msg->appHandle = _phandle;                                    \
        __msg->connectAble = _connectAble;                              \
        CsrBtCmMsgTransport(__msg);}
#endif
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmWritePageToReqSend
 *
 *  DESCRIPTION
 *        ......
 *
 *  PARAMETERS
 *        phandle:        protocol handle
 *        thePageTimeout:    the timeout value
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_CM_WRITE_PAGE_TO
#define CsrBtCmWritePageToReqSend(_phandle,_pageTimeout) {              \
        CsrBtCmWritePageToReq *__msg = (CsrBtCmWritePageToReq*) CsrPmemAlloc(sizeof(CsrBtCmWritePageToReq)); \
        __msg->type = CSR_BT_CM_WRITE_PAGE_TO_REQ;                      \
        __msg->appHandle = _phandle;                                    \
        __msg->pageTimeout = _pageTimeout;                              \
        CsrBtCmMsgTransport(__msg);}
#endif
/*----------------------------------------------------------------------------*
 *  NAME
 *      CmWritePageSettingsReqSend
 *
 *  DESCRIPTION
 *        ......
 *
 *  PARAMETERS
 *        phandle:           protocol handle
 *        interval:          the page scan interval
 *        window:            the page scan window
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_CM_WRITE_PAGE_SCAN
#define CsrBtCmWritePageScanSettingsReqSend(_phandle,_interval,_window) { \
        CsrBtCmWritePagescanSettingsReq *_msg = (CsrBtCmWritePagescanSettingsReq*)CsrPmemAlloc(sizeof(CsrBtCmWritePagescanSettingsReq)); \
        _msg->type = CSR_BT_CM_WRITE_PAGESCAN_SETTINGS_REQ;             \
        _msg->appHandle = _phandle;                                     \
        _msg->interval = _interval;                                     \
        _msg->window = _window;                                         \
        CsrBtCmMsgTransport( _msg);}
/*----------------------------------------------------------------------------*
 *  NAME
 *      CmWritePageSettingsReqSend
 *
 *  DESCRIPTION
 *        ......
 *
 *  PARAMETERS
 *        phandle:           protocol handle
 *        type:              normal/interlaced scan type
 *----------------------------------------------------------------------------*/
#define CsrBtCmWritePageScanTypeReqSend(_phandle,_scanType) {           \
        CsrBtCmWritePagescanTypeReq *_msg = (CsrBtCmWritePagescanTypeReq*)CsrPmemAlloc(sizeof(CsrBtCmWritePagescanTypeReq)); \
        _msg->type = CSR_BT_CM_WRITE_PAGESCAN_TYPE_REQ;                 \
        _msg->appHandle = _phandle;                                     \
        _msg->scanType = _scanType;                                     \
        CsrBtCmMsgTransport( _msg);}
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CmWriteInquirySettingsReqSend
 *
 *  DESCRIPTION
 *        ......
 *
 *  PARAMETERS
 *        phandle:           protocol handle
 *        interval:          the inquiry scan interval
 *        window:            the inquiry scan window
 *----------------------------------------------------------------------------*/
void CsrBtCmWriteInquiryScanSettingsReqSend(CsrSchedQid  phandle,
                                            CsrUint16   interval,
                                            CsrUint16   window);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CmWriteInquirySettingsReqSend
 *
 *  DESCRIPTION
 *        ......
 *
 *  PARAMETERS
 *        phandle:           protocol handle
 *        type:              normal/interlaced scan type
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_CM_WRITE_INQUIRY_SCAN_TYPE
#define CsrBtCmWriteInquiryScanTypeReqSend(_phandle,_scanType) {        \
        CsrBtCmWriteInquiryscanTypeReq *_msg = (CsrBtCmWriteInquiryscanTypeReq*)CsrPmemAlloc(sizeof(CsrBtCmWriteInquiryscanTypeReq)); \
        _msg->type = CSR_BT_CM_WRITE_INQUIRYSCAN_TYPE_REQ;              \
        _msg->appHandle = _phandle;                                     \
        _msg->scanType = _scanType;                                     \
        CsrBtCmMsgTransport( _msg);}
#endif
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmReadRemoteExtFeaturesReqSend
 *
 *  DESCRIPTION
 *        ......
 *
 *  PARAMETERS
 *        phandle:        protocol handle
 *        pageNum:        the requested feature page
 *        theDeviceAddr:    the device address of the device which features is
 *                        requested
 *----------------------------------------------------------------------------*/
#define CsrBtCmReadRemoteExtFeaturesReqSend(_phandle,_pageNum,_deviceAddr) { \
        CsrBtCmReadRemoteExtFeaturesReq *__msg = (CsrBtCmReadRemoteExtFeaturesReq*) CsrPmemAlloc(sizeof(CsrBtCmReadRemoteExtFeaturesReq)); \
        __msg->type = CSR_BT_CM_READ_REMOTE_EXT_FEATURES_REQ;           \
        __msg->appHandle = _phandle;                                    \
        __msg->pageNum = _pageNum;                                      \
        __msg->bd_addr = _deviceAddr;                                   \
        CsrBtCmMsgTransport(__msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmReadLocalExtFeaturesReqSend
 *
 *  DESCRIPTION
 *        ......
 *
 *  PARAMETERS
 *        phandle:        protocol handle
 *        pageNum:        the requested feature page
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_CM_READ_LOCAL_EXT_FEATURES
#define CsrBtCmReadLocalExtFeaturesReqSend(_phandle,_pageNum) {         \
        CsrBtCmReadLocalExtFeaturesReq *__msg = (CsrBtCmReadLocalExtFeaturesReq*) CsrPmemAlloc(sizeof(CsrBtCmReadLocalExtFeaturesReq)); \
        __msg->type = CSR_BT_CM_READ_LOCAL_EXT_FEATURES_REQ;            \
        __msg->appHandle = _phandle;                                    \
        __msg->pageNum = _pageNum;                                      \
        CsrBtCmMsgTransport(__msg);}
#endif
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmSetAfhChannelClassReqSend
 *
 *  DESCRIPTION
 *        ......
 *
 *  PARAMETERS
 *        appHandle:        protocol handle
 *        map:              BR/EDR channel map (NB: array copied, not consumed)
 *
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_CM_AFH
#define CsrBtCmSetAfhChannelClassReqSend(_appHandle,_map) {             \
        CsrBtCmSetAfhChannelClassReq *__msg = (CsrBtCmSetAfhChannelClassReq*) CsrPmemAlloc(sizeof(CsrBtCmSetAfhChannelClassReq)); \
        __msg->type = CSR_BT_CM_SET_AFH_CHANNEL_CLASS_REQ;              \
        __msg->appHandle = _appHandle;                                  \
        CsrMemCpy(__msg->map, _map, 10);                                \
        CsrBtCmMsgTransport(__msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmReadAfhChannelAssessmentModeReqSend
 *
 *  DESCRIPTION
 *        ......
 *
 *  PARAMETERS
 *        phandle:        protocol handle
 *----------------------------------------------------------------------------*/
#define CsrBtCmReadAfhChannelAssessmentModeReqSend(_phandle) {                \
        CsrBtCmReadAfhChannelAssessmentModeReq *__msg = (CsrBtCmReadAfhChannelAssessmentModeReq*) CsrPmemAlloc(sizeof(CsrBtCmReadAfhChannelAssessmentModeReq)); \
        __msg->type = CSR_BT_CM_READ_AFH_CHANNEL_ASSESSMENT_MODE_REQ;   \
        __msg->appHandle = _phandle;                                    \
        CsrBtCmMsgTransport(__msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmWriteAfhChannelAssessmentModeReqSend
 *
 *  DESCRIPTION
 *        ......
 *
 *  PARAMETERS
 *        phandle:        protocol handle
 * theClassMode:        The requested Afh Assessment mode
 *----------------------------------------------------------------------------*/
#define CsrBtCmWriteAfhChannelAssessmentModeReqSend(_phandle,_classMode) { \
        CsrBtCmWriteAfhChannelAssessmentModeReq *__msg = (CsrBtCmWriteAfhChannelAssessmentModeReq*) CsrPmemAlloc(sizeof(CsrBtCmWriteAfhChannelAssessmentModeReq)); \
        __msg->type = CSR_BT_CM_WRITE_AFH_CHANNEL_ASSESSMENT_MODE_REQ;  \
        __msg->appHandle = _phandle;                                    \
        __msg->classMode = _classMode;                                  \
        CsrBtCmMsgTransport(__msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmReadAfhChannelMapReqSend
 *
 *  DESCRIPTION
 *        ......
 *
 *  PARAMETERS
 *        phandle:        protocol handle
 * theDeviceAddr:
 *----------------------------------------------------------------------------*/
#define CsrBtCmReadAfhChannelMapReqSend(_phandle,_deviceAddr) {                \
        CsrBtCmReadAfhChannelMapReq *__msg = (CsrBtCmReadAfhChannelMapReq*) CsrPmemAlloc(sizeof(CsrBtCmReadAfhChannelMapReq)); \
        __msg->type = CSR_BT_CM_READ_AFH_CHANNEL_MAP_REQ;               \
        __msg->appHandle = _phandle;                                    \
        __msg->bd_addr = _deviceAddr;                                   \
        CsrBtCmMsgTransport(__msg);}
#endif
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmReadClockReqSend
 *
 *  DESCRIPTION
 *        ......
 *
 *  PARAMETERS
 *        phandle:        protocol handle
 * theDeviceAddr:
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_CM_READ_CLOCK
#define CsrBtCmReadClockReqSend(_phandle,_clock,_deviceAddr) {             \
        CsrBtCmReadClockReq *__msg = (CsrBtCmReadClockReq*) CsrPmemAlloc(sizeof(CsrBtCmReadClockReq)); \
        __msg->type = CSR_BT_CM_READ_CLOCK_REQ;                         \
        __msg->appHandle = _phandle;                                    \
        __msg->bd_addr = _deviceAddr;                                   \
        __msg->whichClock = _clock;                                     \
        CsrBtCmMsgTransport(__msg);}
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmReadLocalVersionReqSend
 *
 *  DESCRIPTION
 *        ......
 *
 *  PARAMETERS
 *        phandle:        protocol handle
 *----------------------------------------------------------------------------*/
#define CsrBtCmReadLocalVersionReqSend(_phandle) {                      \
        CsrBtCmReadLocalVersionReq *__msg = (CsrBtCmReadLocalVersionReq*) CsrPmemAlloc(sizeof(CsrBtCmReadLocalVersionReq)); \
        __msg->type = CSR_BT_CM_READ_LOCAL_VERSION_REQ;                 \
        __msg->appHandle = _phandle;                                    \
        CsrBtCmMsgTransport(__msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmRoleDiscoveryReqSend
 *
 *  DESCRIPTION
 *      Discover the current role (Master or Slave)
 *
 *  PARAMETERS
 *        appHandle:             protocol handle
 *        deviceAddr:             BT address
 *----------------------------------------------------------------------------*/
void CsrBtCmRoleDiscoveryReqSend(CsrSchedQid       appHandle,
                                 CsrBtDeviceAddr    deviceAddr);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmWriteLinkPolicyReqSend
 *
 *  DESCRIPTION
 *      .....
 *
 *  PARAMETERS
 *        deviceAddr:        BT address of the device to write link policy
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_CM_LINK_POLICY
#define CsrBtCmWriteLinkPolicyReqSend(_appHandle, _deviceAddr, _linkPolicySetting, _parkSettings, _setupLinkPolicySetting, _sniffSettings) { \
        CsrBtCmWriteLinkPolicyReq *__msg = (CsrBtCmWriteLinkPolicyReq*) CsrPmemZalloc(sizeof(CsrBtCmWriteLinkPolicyReq)); \
        __msg->type = CSR_BT_CM_WRITE_LINK_POLICY_REQ;                  \
        __msg->appHandle = _appHandle;                                  \
        __msg->deviceAddr = _deviceAddr;                                \
        __msg->linkPolicySetting = _linkPolicySetting;                  \
        __msg->setupLinkPolicySetting = _setupLinkPolicySetting;        \
        if (_parkSettings)                                              \
        {                                                               \
            CsrPmemFree(_parkSettings);                                 \
        }                                                               \
        if (_sniffSettings)                                             \
        {                                                               \
            __msg->sniffSettingsCount = 1;                              \
            __msg->sniffSettings = _sniffSettings;                      \
        }                                                               \
        CsrBtCmMsgTransport(__msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmReadLinkPolicyReqSend
 *
 *  DESCRIPTION
 *      .....
 *
 *  PARAMETERS
 *        apphandle:        protocol handle
 *        deviceAddr:        BT address of the device to read link policy
 *----------------------------------------------------------------------------*/
#define CsrBtCmReadLinkPolicyReqSend(_appHandle, _deviceAddr) {         \
        CsrBtCmReadLinkPolicyReq *__msg = (CsrBtCmReadLinkPolicyReq*) CsrPmemAlloc(sizeof(CsrBtCmReadLinkPolicyReq)); \
        __msg->type = CSR_BT_CM_READ_LINK_POLICY_REQ;                   \
        __msg->appHandle = _appHandle;                                  \
        __msg->deviceAddr = _deviceAddr;                                \
        CsrBtCmMsgTransport(__msg);}
#endif
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmCancelReadRemoteNameReqSend
 *
 *  DESCRIPTION
 *      .....
 *
 *  PARAMETERS
 *        apphandle:        protocol handle
 *        deviceAddr:        BT address of the device that is being read
 *----------------------------------------------------------------------------*/
void CsrBtCmCancelReadRemoteNameReqSend(CsrSchedQid      theAppHandle,
                                        CsrBtDeviceAddr   theDeviceAddr);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmEirUpdateManufacturerDataReqSend
 *
 *  DESCRIPTION
 *      Used for setting a the manufacturer specific data in an Extended Inquiry
 *      Response.
 *
 *    PARAMETERS
 *      appHandle:                  Application handle
 *      manufacturerDataSettings:   Settings for handling the manufacturer data.
 *                                  Refer to the documentation for further details.
 *      manufacturerDataLength:     Length of the data in *manufacturerData
 *      manufacturerData:           The actual manufacturer data as it will
 *                                  appear in the EIR.
 *----------------------------------------------------------------------------*/
void CsrBtCmEirUpdateManufacturerDataReqSend(CsrSchedQid appHandle,
                                             CsrUint8        manufacturerDataSettings,
                                             CsrUint8        manufacturerDataLength,
                                             CsrUint8        *manufacturerData);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmEirFlagsReq
 *
 *  DESCRIPTION
 *       Update the EIR 'flags' with eg. discoverable mode (general/limited).
 *
 *    PARAMETERS
 *      appHandle:                  Application handle
 *      eirFlags:                   EIR flags bitmask, see CSR_BT_EIR_FLAG_
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_CM_EIR_FLAGS
#define CsrBtCmEirFlagsReqSend(_appHandle,_flags) {         \
        CsrBtCmEirFlagsReq *msg__ = (CsrBtCmEirFlagsReq*)CsrPmemAlloc(sizeof(CsrBtCmEirFlagsReq)); \
        msg__->type = CSR_BT_CM_EIR_FLAGS_REQ;                          \
        msg__->appHandle = _appHandle;                                   \
        msg__->eirFlags = _flags;                                        \
        CsrBtCmMsgTransport(msg__);}
#endif
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmRoleSwitchConfigReqSend
 *
 *  DESCRIPTION
 *        ......
 *
 *  PARAMETERS
 *        config            :    role switch config. See csr_bt_cm_prim.h
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_CM_ROLE_SWITCH_CONFIG
#define CsrBtCmRoleSwitchConfigReqSend(_config) {            \
        CsrBtCmRoleSwitchConfigReq *__msg = (CsrBtCmRoleSwitchConfigReq*) CsrPmemAlloc(sizeof(CsrBtCmRoleSwitchConfigReq)); \
        __msg->type    = CSR_BT_CM_ROLE_SWITCH_CONFIG_REQ;              \
        __msg->config  = _config;                                       \
        CsrBtCmMsgTransport(__msg);}
#endif
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmReadFailedContactCounterReqSend
 *
 *  DESCRIPTION
 *        ......
 *
 *  PARAMETERS
 *        phandle               :    protocol handle
 *        deviceAddr            :    BT address of the device to read remote version
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_CM_READ_FAILED_CONTACT_COUNTER
#define CsrBtCmReadFailedContactCounterReqSend(_phandle, _deviceAddr) {         \
        CsrBtCmReadFailedContactCounterReq *__msg = (CsrBtCmReadFailedContactCounterReq*) CsrPmemAlloc(sizeof(CsrBtCmReadFailedContactCounterReq)); \
        __msg->type        = CSR_BT_CM_READ_FAILED_CONTACT_COUNTER_REQ; \
        __msg->appHandle   = _phandle;                                  \
        __msg->deviceAddr  = _deviceAddr;                               \
        CsrBtCmMsgTransport(__msg);}
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmSetEventMaskReqSend
 *
 *  DESCRIPTION
 *        ......
 *
 *  PARAMETERS
 *        phandle    :  Protocol handle
 *        eventMask  :  Describes which extended information an application
 *                      will subscribe for
 *        condition  :  Filter condition
 *----------------------------------------------------------------------------*/
void CsrBtCmSetEventMaskReqSend(CsrSchedQid phandle, CsrUint32 eventMask, CsrUint32 conditionMask);


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmSwitchRoleReqSend
 *
 *  DESCRIPTION
 *        ......
 *
 *  PARAMETERS
 *        phandle       :    protocol handle
 *        deviceAddr    :    BT address of the device to read remote version
 *        role          :    Requested role (master/slave/unknown)
 *        roleType      :    Requested role (only CSR_BT_CM_SWITCH_ROLE_TYPE_ONESHOT supported)
 *        config        :    RFU - shall be zero
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_CM_SWITCH_ROLE_PUBLIC
void CsrBtCmSwitchRoleReqSend(CsrSchedQid phandle,
                              CsrBtDeviceAddr deviceAddr,
                              CsrUint8 role,
                              CsrBtCmRoleType roleType,
                              CsrUint32 config);
#endif
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmSniffModeReqSend
 *
 *  DESCRIPTION
 *        The command request the ACL connection associated the given deviceAddr
 *        address to alter sniff mode
 *
 *  PARAMETERS
 *        phandle            : Protocol handle
 *        deviceAddr         : Use to identify the ACL connection which must be
 *                             put in sniff mode
 *        maxInterval        : Specify the acceptable maximum period in sniff mode.
 *                             Note that maxInterval must be greater than
 *                             minInterval and that maxInterval shall be less than
 *                             the Link  Supervision Timeout
 *        minInterval        : Specify the acceptable minimum period in sniff mode
 *        attempt            : Specify the number of baseband received slots
 *                             for sniff attempts
 *        timeout            : Specify the number of baseband received slots for
 *                             sniff timeout
 *        forceSniffSettings : If TRUE and the current mode is Sniff then
 *                             will the CM Exit Sniff mode and enter sniff mode
 *                             again with the given sniff setiings. If FALSE and
 *                             current mode is sniff, it will just stay in sniff
 *                             mode
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_CM_LOW_POWER_CONFIG_PUBLIC
#define CsrBtCmSniffModeReqSend(_phandle, _deviceAddr, _maxInterval, _minInterval, _attempt, _timeout, _forceSniffSettings) {     \
        CsrBtCmModeChangeReq *__msg = (CsrBtCmModeChangeReq *) CsrPmemAlloc(sizeof(CsrBtCmModeChangeReq)); \
        __msg->type                          = CSR_BT_CM_MODE_CHANGE_REQ; \
        __msg->phandle                       = _phandle;                \
        __msg->deviceAddr                    = _deviceAddr;             \
        __msg->mode                          = CSR_BT_SNIFF_MODE;       \
        __msg->sniffSettings.attempt         = _attempt;                \
        __msg->sniffSettings.max_interval    = _maxInterval;            \
        __msg->sniffSettings.min_interval    = _minInterval;            \
        __msg->sniffSettings.timeout         = _timeout;                \
        __msg->forceSniffSettings            = _forceSniffSettings;     \
        CsrBtCmMsgTransport(__msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmExitSniffModeReqSend
 *
 *  DESCRIPTION
 *        The command request the ACL connection associated the given deviceAddr
 *        address to exit sniff mode and return to active mode
 *
 *  PARAMETERS
 *        phandle       : Protocol handle
 *        deviceAddr    : Use to identify the ACL connection to exit the sniff mode 
 *                       
 *----------------------------------------------------------------------------*/
#define CsrBtCmExitSniffModeReqSend(_phandle, _deviceAddr) {     \
    CsrBtCmModeChangeReq *__msg ; \
    __msg=(CsrBtCmModeChangeReq *)CsrPmemAlloc(sizeof(CsrBtCmModeChangeReq)); \
    __msg->type                          = CSR_BT_CM_MODE_CHANGE_REQ; \
    __msg->phandle                       = _phandle;                  \
    __msg->deviceAddr                    = _deviceAddr;               \
    __msg->mode                          = CSR_BT_ACTIVE_MODE;        \
    __msg->sniffSettings.attempt         = 0;                         \
    __msg->sniffSettings.max_interval    = 0;                         \
    __msg->sniffSettings.min_interval    = 0;                         \
    __msg->sniffSettings.timeout         = 0;                         \
    __msg->forceSniffSettings            = FALSE;                     \
    CsrBtCmMsgTransport(__msg);}                            
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmModeChangeConfigReqSend
 *
 *  DESCRIPTION
 *        The command request is used to configure how low power modes shall
 *        be handle. It can either be done by Synergy BT or the application
 *
 *
 *  PARAMETERS
 *        phandle       : Protocol handle
 *        deviceAddr    : Use to identify the ACL connection for which low
 *                        power mode handling shall be configured. Note setting
 *                        the deviceAddr to 0 will change the default setting.
 *                        E.g. the config parameter is set to CSR_BT_CM_MODE_CHANGE_ENABLE
 *                        the Application will always have 100% control over the
 *                        low power mode handling
 *
 *        config        : Setting this parameter to CSR_BT_CM_MODE_CHANGE_DISABLE (Default
 *                        Setting) means that Synergy BT is controlling low power modes.
 *                        Setting this parameter to CSR_BT_CM_MODE_CHANGE_ENABLE means that
 *                        the application is controlling low power modes
 *----------------------------------------------------------------------------*/
#define CsrBtCmModeChangeConfigReqSend(_phandle, _deviceAddr, _config) {            \
        CsrBtCmModeChangeConfigReq *__msg = (CsrBtCmModeChangeConfigReq *) CsrPmemAlloc(sizeof(CsrBtCmModeChangeConfigReq)); \
        __msg->type                       = CSR_BT_CM_MODE_CHANGE_CONFIG_REQ; \
        __msg->phandle                    = _phandle;                   \
        __msg->deviceAddr                 = _deviceAddr;                \
        __msg->config                     = _config;                    \
        CsrBtCmMsgTransport(__msg);}
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmAlwaysMasterDevicesReqSend
 *
 *  DESCRIPTION
 *        This message can be use to maintain list of remote devices for which
 *        the Device Manager will always try to become master during any ACL
 *        connection creation,  even if there are no existing ACLs connected.
 *        For locally-initiated connection requests to devices in the list,
 *        the Device Manager will prohibit role-switch, thus ensuring that
 *        the local device becomes master. For remotely-initiated connection
 *        requests to devices in the list, the Device Manager will request a
 *        role-switch during connection creation. This may or may not be
 *        accepted by the remote device. Please note that this message should
 *        be used only to work around problems with severely misbehaving remote
 *        devices. Any other use is likely to produce a severely misbehaving
 *        local device and lead to major interoperability problems.
 *        E.g. this primitive should only be used when it is necessary to
 *        become master, even when there are no existing connections, because
 *        the remote device is badly behaved and will not role-switch after
 *        connection creation and it is likely that further ACLs will soon
 *        be connected.
 *
 *  PARAMETERS
 *        phandle       : Protocol handle
 *        deviceAddr    : The Bluetooth address of the peer device, which
 *                        shall be added to, or delete from, the list
 *
 *        operation     : CSR_BT_CM_ALWAYS_MASTER_DEVICES_CLEAR - CLEAR the entire list
 *                        CSR_BT_CM_ALWAYS_MASTER_DEVICES_ADD - ADD a new device to the list
 *                        CSR_BT_CM_ALWAYS_MASTER_DEVICES_DELETE - DELETE a device from the list
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_CM_SWITCH_ROLE_PUBLIC
#define CsrBtCmAlwaysMasterDevicesReqSend(_phandle, _deviceAddr, _operation) { \
        CsrBtCmAlwaysMasterDevicesReq *__msg = (CsrBtCmAlwaysMasterDevicesReq *) CsrPmemAlloc(sizeof(CsrBtCmAlwaysMasterDevicesReq)); \
        __msg->type          = CSR_BT_CM_ALWAYS_MASTER_DEVICES_REQ;     \
        __msg->phandle       = _phandle;                                \
        __msg->deviceAddr    = _deviceAddr;                             \
        __msg->operation     = _operation;                              \
        CsrBtCmMsgTransport(__msg);}
#endif
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmPublicRegisterReqSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *        phandle:            protocol handle
 *        context             Opaque context number returned in CsrBtCmRegisterCfm
 *        serverChannel       server channel requested
 *----------------------------------------------------------------------------*/
CsrBtCmRegisterReq *CsrBtCmRegisterReq_struct(CsrSchedQid phandle, CsrUint16 context, CsrUint8 serverChannel);

#define CsrBtCmPublicRegisterReqSend(_phandle, _context, _serverChannel) { \
        CsrBtCmRegisterReq *msg__;                                      \
        msg__=CsrBtCmRegisterReq_struct(_phandle, _context, _serverChannel); \
        CsrBtCmPutMessageDownstream( msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmUnRegisterReqSend
 *
 *  DESCRIPTION
 *      ....
 *
 *  PARAMETERS
 *        serverChannel:        local server channel to unregister
 *----------------------------------------------------------------------------*/
void CsrBtCmUnRegisterReqSend(CsrUint8    theServerChannel);

#ifdef CSR_BT_LE_ENABLE
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmReadAdvertisingChTxPowerReqSend
 *
 *  DESCRIPTION
 *      Read Tx power for low energy advertising channel
 *
 *  PARAMETERS
 *        phandle:           protocol handle
 *        context             Opaque context number returned in CsrBtCmReadAdvertisingChTxPowerCfm
 *----------------------------------------------------------------------------*/
#define CsrBtCmReadAdvertisingChTxPowerReqSend(_appHandle,_context) {                \
        CsrBtCmReadAdvertisingChTxPowerReq *msg__ = CsrPmemAlloc(sizeof(CsrBtCmReadAdvertisingChTxPowerReq)); \
        msg__->type = CSR_BT_CM_READ_ADVERTISING_CH_TX_POWER_REQ;       \
        msg__->appHandle = _appHandle;                                  \
        msg__->context = _context;                                      \
        CsrBtCmPutMessageDownstream(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmLeReceiverTestReqSend
 *
 *  DESCRIPTION
 *      Send LE receiver test command
 *
 *  PARAMETERS
 *        phandle:           Application handle
 *        rxFrequency:       Rx frequency to perform test on
 *----------------------------------------------------------------------------*/
#define CsrBtCmLeReceiverTestReqSend(_appHandle,_rxFrequency) {         \
        CsrBtCmLeReceiverTestReq *msg__ = CsrPmemAlloc(sizeof(CsrBtCmLeReceiverTestReq)); \
        msg__->type = CSR_BT_CM_LE_RECEIVER_TEST_REQ;                   \
        msg__->appHandle = _appHandle;                                  \
        msg__->rxFrequency = _rxFrequency;                              \
        CsrBtCmPutMessageDownstream( msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmLeTransmitterTestReqSend
 *
 *  DESCRIPTION
 *      Send LE transmitter test command
 *
 *  PARAMETERS
 *        phandle:           Application handle
 *        txFrequency:       Tx frequency to perform test on
 *----------------------------------------------------------------------------*/
#define CsrBtCmLeTransmitterTestReqSend(_appHandle,_txFrequency,_lengthOfTestData,_packetPayload) { \
        CsrBtCmLeTransmitterTestReq *msg__ = CsrPmemAlloc(sizeof(CsrBtCmLeTransmitterTestReq)); \
        msg__->type = CSR_BT_CM_LE_TRANSMITTER_TEST_REQ;                \
        msg__->appHandle = _appHandle;                                  \
        msg__->txFrequency = _txFrequency;                              \
        msg__->lengthOfTestData = _lengthOfTestData;                    \
        msg__->packetPayload = _packetPayload;                          \
        CsrBtCmPutMessageDownstream( msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmLeTestEndReqSend
 *
 *  DESCRIPTION
 *      Send LE test end command
 *
 *  PARAMETERS
 *        phandle:           Application handle
 *----------------------------------------------------------------------------*/
#define CsrBtCmLeTestEndReqSend(_appHandle) {           \
        CsrBtCmLeTestEndReq *msg__ = CsrPmemAlloc(sizeof(CsrBtCmLeTestEndReq)); \
        msg__->type = CSR_BT_CM_LE_TEST_END_REQ;                        \
        msg__->appHandle = _appHandle;                                  \
        CsrBtCmPutMessageDownstream( msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmLeSetOwnAddressTypeReqSend
 *
 *  DESCRIPTION
 *      Sets the value of own address type to be used for LE GAP procedures.
 *      If application does not set own address type, default public address
 *      type shall be used as own address type for LE GAP procedures.
 *
 *  PARAMETERS
 *      appHandle       :  protocol handle
 *      ownAddressType  :  Own address type to be set. Below values are allowed.
 *                         0x00 : Public address
 *                         0x01 : Random address
 *                         0x02 : Controller generated RPA, use public address
 *                                if controller can't generate RPA.
 *                         0x03 : Controller generated RPA, use random address
 *                                from LE_Set_Random_Address if controller can't
 *                                generate RPA.
 * Note : 1. Setting own address type's value to 0x02 and 0x03 is allowed only
 *           if controller supports LL_PRIVACY.
 *        2. If CMake configuration flag "CSR_BT_LE_RANDOM_ADDRESS_TYPE" set as
 *           "STATIC", changing own address type is not allowed.
 *----------------------------------------------------------------------------*/
CsrBtCmLeSetOwnAddressTypeReq *CsrBtCmLeSetOwnAddressTypeReq_struct(CsrSchedQid appHandle,
                                                                    CsrUint8 ownAddressType);

#define CsrBtCmLeSetOwnAddressTypeReqSend(_a,                   \
                                          _oat)                 \
    do                                                          \
    {                                                           \
        CsrBtCmLeSetOwnAddressTypeReq *msg__;                   \
        msg__ = CsrBtCmLeSetOwnAddressTypeReq_struct(_a, _oat); \
        CsrBtCmPutMessageDownstream(msg__);                     \
    } while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmLeSetPvtAddrTimeoutReqSend
 *
 *  DESCRIPTION
 *      Sets the value of Resolvable/Non-resolvable Private Address timeout.
 *      Length of the time Synergy uses a RPA/NRPA before a new RPA/NRPA is
 *      generated & starts being used.
 *
 *  PARAMETERS
 *      appHandle       :  protocol handle
 *      timeout         :  Sets the value of private address time out.
 *                         Range : 0x0001(1 sec) - 0xA1B8(~11.5 hours)
 *----------------------------------------------------------------------------*/
CsrBtCmLeSetPvtAddrTimeoutReq *CsrBtCmLeSetPvtAddrTimeoutReq_struct(CsrSchedQid appHandle,
                                                                    CsrUint16 timeout);

#define CsrBtCmLeSetPvtAddrTimeoutReqSend(_a,                       \
                                          _timeOut)                 \
    do                                                              \
    {                                                               \
        CsrBtCmLeSetPvtAddrTimeoutReq *msg__;                       \
        msg__ = CsrBtCmLeSetPvtAddrTimeoutReq_struct(_a, _timeOut); \
        CsrBtCmPutMessageDownstream(msg__);                         \
    } while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmLeSetStaticAddressReqSend
 *
 *  DESCRIPTION
 *      Configures local static random address for current power cycle. This
 *      API configures application's provided static address as local random
 *      address for current power cycle only if CMake configuration flag
 *      "CSR_BT_LE_RANDOM_ADDRESS_TYPE" set as "STATIC".
 *
 *  Note: Static address shall be configured only once for current power cycle.
 *
 *  PARAMETERS
 *      appHandle       :  protocol handle
 *      staticAddress   :  Static address to be used as local device random
 *                         address.
 *----------------------------------------------------------------------------*/
CsrBtCmLeSetStaticAddressReq *CsrBtCmLeSetStaticAddressReq_struct(CsrSchedQid appHandle,
                                                                  CsrBtDeviceAddr staticAddress);
#define CsrBtCmLeSetStaticAddressReqSend(_a,                             \
                                         _staticAddress)                 \
    do                                                                   \
    {                                                                    \
        CsrBtCmLeSetStaticAddressReq *msg__;                             \
        msg__ = CsrBtCmLeSetStaticAddressReq_struct(_a, _staticAddress); \
        CsrBtCmPutMessageDownstream(msg__);                              \
    } while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmLeReadRandomAddressReqSend
 *
 *  DESCRIPTION
 *      Read local or peer device's random address for trusted devices. This API
 *      expects trusted peer device identity address(IA) and provides local or
 *      peer current available random address based on the value set for "flag".
 *      If given peer IA fields are all zeros, "flag" value is ignored and local
 *      random address is returned. Since local RPAs can be different for 
 *      multiple LE connections(multiple peer devices) so it is mendatory for
 *      application to provide peer's IA to retrieve actual local or peer RPA
 *      used for existing LE connection.
 *
 *      This API expects peer device's IA from subsequent connection with
 *      trusted devices. If it is called at new connection, application has to
 *      pass peer device's connected address as peer's "idAddress" even after
 *      device gets bonded in the same connection.
 *
 *  PARAMETERS
 *      appHandle       :  protocol handle
 *      idAddress       :  Identity address of the connected peer device.
 *      flag            :  Retrieve Random address either for local or peer
 *                         device. Values can be set as:
 *                         0x01 : local and 0x02 : peer device
 *----------------------------------------------------------------------------*/
CsrBtCmLeReadRandomAddressReq *CsrBtCmLeReadRandomAddressReq_struct(CsrSchedQid appHandle,
                                                                    CsrBtTypedAddr idAddress,
                                                                    CsrBtDeviceFlag flag);
#define CsrBtCmLeReadRandomAddressReqSend(_a,                           \
                                          _addr,                        \
                                          _flag)                        \
    do                                                                  \
    {                                                                   \
        CsrBtCmLeReadRandomAddressReq *msg__;                           \
        msg__ = CsrBtCmLeReadRandomAddressReq_struct(_a, _addr, _flag); \
        CsrBtCmPutMessageDownstream(msg__);                             \
    } while(0)

#endif /* End of CSR_BT_LE_ENABLE */

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtCmFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT CM
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_CM_PRIM,
 *      msg:          The message received from Synergy BT CM
 *----------------------------------------------------------------------------*/
void CsrBtCmFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);

#ifdef __cplusplus
}
#endif

#endif

