#ifndef CSR_BT_JSR82_LIB_H__
#define CSR_BT_JSR82_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_profiles.h"
#include "csr_bt_jsr82_prim.h"
#include "csr_pmem.h"
#include "csr_util.h"
#include "csr_bt_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtJsr82MsgTransport(void *msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      Jsr82StartInquiryReqSend
 *
 *  DESCRIPTION
 *       Starts discovery on bluetooth device, and sets java application
 *          ready to receive results.
 *
 *   PARAMETERS
 *       phandle:            application handle
 *       reqID:              request ID of the transaction
 *       iac:                inquiry access code
 *
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82DaStartInquiryReqSend(_phandle, _reqID, _iac) {       \
        CsrBtJsr82DaStartInquiryReq *msg = (CsrBtJsr82DaStartInquiryReq *)CsrPmemAlloc(sizeof(CsrBtJsr82DaStartInquiryReq)); \
        msg->type          = CSR_BT_JSR82_DA_START_INQUIRY_REQ;         \
        msg->appHandle     = _phandle;                                  \
        msg->reqID         = _reqID;                                    \
        msg->iac           = _iac;                                      \
        CsrBtJsr82MsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtJsr82DaCancelInquiryReqSend
 *
 *  DESCRIPTION
 *      Cancels discovery on bluetooth device
 *
 *   PARAMETERS
 *   phandle:            application handle
 *
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82DaCancelInquiryReqSend(_phandle, _reqID) {            \
        CsrBtJsr82DaCancelInquiryReq *msg = (CsrBtJsr82DaCancelInquiryReq *)CsrPmemAlloc(sizeof(CsrBtJsr82DaCancelInquiryReq)); \
        msg->type          = CSR_BT_JSR82_DA_CANCEL_INQUIRY_REQ;        \
        msg->appHandle     = _phandle;                                  \
        msg->reqID         = _reqID;                                    \
        CsrBtJsr82MsgTransport(msg);}



/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtJsr82DaSearchServicesReqSend
 *
 *  DESCRIPTION
 *      Starts a service search
 *
 *   PARAMETERS
 *   phandle:            application handle
 *   reqID:              request ID passed down from Java. This must be used if the search is to be cancelled again.
 *   deviceAddr:         bluetooth address of the remote device to search
 *   uuidSetLength:      length of the uuidSet in bytes
 *   uuidSet:            The search pattern to search for as a Data Element sequence of UUIDs
 *   attrSetLength:      length of attrSet in bytes
 *   attrSet:            the set of attributes to retrieve once the requested service
 *                       is found
 *
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82DaSearchServicesReqSend(_phandle, _reqID, _deviceAddr, _uuidSetLength, _uuidSet, _attrSetLength, _attrSet) { \
        CsrBtJsr82DaSearchServicesReq *msg = (CsrBtJsr82DaSearchServicesReq *)CsrPmemAlloc(sizeof(CsrBtJsr82DaSearchServicesReq)); \
        msg->type          = CSR_BT_JSR82_DA_SEARCH_SERVICES_REQ;       \
        msg->appHandle     = _phandle;                                  \
        msg->reqID         = _reqID;                                    \
        msg->deviceAddr    = _deviceAddr;                               \
        msg->uuidSetLength = _uuidSetLength;                            \
        msg->uuidSet       = _uuidSet;                                  \
        msg->attrSetLength = _attrSetLength;                            \
        msg->attrSet       = _attrSet;                                  \
        CsrBtJsr82MsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtJsr82DaCancelServiceSearchReqSend
 *
 *  DESCRIPTION
 *      Cancels a service search
 *
 *   PARAMETERS
 *   phandle:            application handle
 *   reqID:            request ID corresponding to the search to be cancelled
 *
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82DaCancelServiceSearchReqSend(_phandle, _reqID) {      \
        CsrBtJsr82DaCancelServiceSearchReq *msg = (CsrBtJsr82DaCancelServiceSearchReq *)CsrPmemAlloc(sizeof(CsrBtJsr82DaCancelServiceSearchReq )); \
        msg->type          = CSR_BT_JSR82_DA_CANCEL_SERVICE_SEARCH_REQ; \
        msg->appHandle     = _phandle;                                  \
        msg->reqID         = _reqID;                                    \
        CsrBtJsr82MsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtJsr82DaSelectServiceReqSend
 *
 *  DESCRIPTION
 *
 *
 *   PARAMETERS
 *   phandle:            application handle
 *   requestedUuid:               identifies the requested service
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82DaSelectServiceReqSend(_phandle, _reqID, _requestedUuidLength, _requestedUuid) { \
        CsrBtJsr82DaSelectServiceReq *msg = (CsrBtJsr82DaSelectServiceReq *)CsrPmemAlloc(sizeof(CsrBtJsr82DaSelectServiceReq )); \
        msg->type          = CSR_BT_JSR82_DA_SELECT_SERVICE_REQ;        \
        msg->appHandle     = _phandle;                                  \
        msg->reqID         = _reqID;                                    \
        msg->requestedUuidLength    = _requestedUuidLength;             \
        msg->requestedUuid          = _requestedUuid;                   \
        CsrBtJsr82MsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtJsr82DaRetrieveDevicesReqSend
 *
 *  DESCRIPTION
 *
 *
 *   PARAMETERS
 *   phandle:            application handle
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82DaRetrieveDevicesReqSend(_appHandle, _reqID, _option) { \
        CsrBtJsr82DaRetrieveDevicesReq *msg = (CsrBtJsr82DaRetrieveDevicesReq *)CsrPmemAlloc(sizeof(CsrBtJsr82DaRetrieveDevicesReq )); \
        msg->type          = CSR_BT_JSR82_DA_RETRIEVE_DEVICES_REQ;      \
        msg->appHandle     = _appHandle;                                \
        msg->reqID         = _reqID;                                    \
        msg->option        = _option;                                   \
        CsrBtJsr82MsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtJsr82LdGetBluetoothAddressReqSend
 *
 *  DESCRIPTION
 *      Gets bluetooth address of local device
 *
 *   PARAMETERS
 *   phandle:            application handle
 *
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82LdGetBluetoothAddressReqSend(_phandle, _reqID) {      \
        CsrBtJsr82LdGetBluetoothAddressReq *msg = (CsrBtJsr82LdGetBluetoothAddressReq *)CsrPmemAlloc(sizeof(CsrBtJsr82LdGetBluetoothAddressReq)); \
        msg->type          = CSR_BT_JSR82_LD_GET_BLUETOOTH_ADDRESS_REQ; \
        msg->appHandle     = _phandle;                                  \
        msg->reqID         = _reqID;                                    \
        CsrBtJsr82MsgTransport(msg);}



/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtJsr82LdUpdateRecordReqSend
 *
 *  DESCRIPTION
 *      Updates (unregisters and re-registers) a service record
 *
 *   PARAMETERS
 *   phandle:            application handle
 *
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82LdUpdateRecordReqSend(_phandle, _reqID, _serviceRecHandle, _serviceRecord, _serviceRecordSize) { \
        CsrBtJsr82LdUpdateRecordReq *msg = (CsrBtJsr82LdUpdateRecordReq *)CsrPmemAlloc(sizeof(CsrBtJsr82LdUpdateRecordReq )); \
        msg->type          = CSR_BT_JSR82_LD_UPDATE_RECORD_REQ;         \
        msg->appHandle     = _phandle;                                  \
        msg->reqID         = _reqID;                                    \
        msg->serviceRecHandle = _serviceRecHandle;                      \
        msg->serviceRecord = _serviceRecord;                            \
        msg->serviceRecordSize = _serviceRecordSize;                    \
        CsrBtJsr82MsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtJsr82LdGetFriendlyNameReqSend
 *
 *  DESCRIPTION
 *      Requests friendly name from local device
 *
 *   PARAMETERS
 *   phandle:            application handle
 *
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82LdGetFriendlyNameReqSend(_phandle, _reqID) {          \
        CsrBtJsr82LdGetFriendlyNameReq *msg = (CsrBtJsr82LdGetFriendlyNameReq  *)CsrPmemAlloc(sizeof(CsrBtJsr82LdGetFriendlyNameReq )); \
        msg->type          = CSR_BT_JSR82_LD_GET_FRIENDLY_NAME_REQ;     \
        msg->appHandle     = _phandle;                                  \
        msg->reqID         = _reqID;                                    \
        CsrBtJsr82MsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtJsr82LdGetDiscoverableReqSend
 *
 *  DESCRIPTION
 *      Requests visibility state of local device
 *
 *   PARAMETERS
 *   phandle:            application handle
 *
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82LdGetDiscoverableReqSend(_phandle, _reqID) {          \
        CsrBtJsr82LdGetDiscoverableReq *msg = (CsrBtJsr82LdGetDiscoverableReq  *)CsrPmemAlloc(sizeof(CsrBtJsr82LdGetDiscoverableReq )); \
        msg->type          = CSR_BT_JSR82_LD_GET_DISCOVERABLE_REQ;      \
        msg->appHandle     = _phandle;                                  \
        msg->reqID         = _reqID;                                    \
        CsrBtJsr82MsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtJsr82LdSetDiscoverableReqSend
 *
 *  DESCRIPTION
 *      Sets visibility state of local device
 *
 *   PARAMETERS
 *   phandle:            application handle
 *
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82LdSetDiscoverableReqSend(_phandle, _reqID, _mode) {   \
        CsrBtJsr82LdSetDiscoverableReq *msg = (CsrBtJsr82LdSetDiscoverableReq  *)CsrPmemAlloc(sizeof(CsrBtJsr82LdSetDiscoverableReq )); \
        msg->type          = CSR_BT_JSR82_LD_SET_DISCOVERABLE_REQ;      \
        msg->appHandle     = _phandle;                                  \
        msg->reqID         = _reqID;                                    \
        msg->mode          = _mode;                                     \
        CsrBtJsr82MsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtJsr82LdGetPropertyReqSend
 *
 *  DESCRIPTION
 *      Request information about the properties of the local bluetooth device
 *
 *   PARAMETERS
 *   phandle:            application handle
 *      propertyName:                   the name of the requested property, as defined
 *                                      the JSR82 API (#define's in csr_bt_jsr82_prim.h)
 *
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82LdGetPropertyReqSend(_phandle, _reqID,_propertyName) { \
        CsrBtJsr82LdGetPropertyReq *msg = (CsrBtJsr82LdGetPropertyReq  *)CsrPmemAlloc(sizeof(CsrBtJsr82LdGetPropertyReq )); \
        msg->type          = CSR_BT_JSR82_LD_GET_PROPERTY_REQ;          \
        msg->appHandle             = _phandle;                          \
        msg->reqID         = _reqID;                                    \
        msg->propertyName              = _propertyName;                 \
        CsrBtJsr82MsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      Jsr82LdGetDeviceClassReq
 *
 *  DESCRIPTION
 *      Request the class of device for the local device
 *
 *   PARAMETERS
 *   phandle:            application handle
 *
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82LdGetDeviceClassReqSend(_phandle, _reqID,_fieldsMask) { \
        CsrBtJsr82LdGetDeviceClassReq *msg = (CsrBtJsr82LdGetDeviceClassReq  *)CsrPmemAlloc(sizeof(CsrBtJsr82LdGetDeviceClassReq)); \
        msg->type          = CSR_BT_JSR82_LD_GET_DEVICE_CLASS_REQ;      \
        msg->appHandle     = _phandle;                                  \
        msg->reqID         = _reqID;                                    \
        msg->fieldsMask    = _fieldsMask;                               \
        CsrBtJsr82MsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *       Jsr82LdSetDeviceClassReq
 *
 *  DESCRIPTION
 *       Set the class of device for the local device. Only sets the service
 *       classes bits
 *
 *   PARAMETERS
 *   phandle:            application handle
 *
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82LdSetDeviceClassReqSend(_phandle, _reqID,_deviceClass) { \
        CsrBtJsr82LdSetDeviceClassReq *msg = (CsrBtJsr82LdSetDeviceClassReq  *)CsrPmemAlloc(sizeof(CsrBtJsr82LdSetDeviceClassReq)); \
        msg->type          = CSR_BT_JSR82_LD_SET_DEVICE_CLASS_REQ;      \
        msg->appHandle     = _phandle;                                  \
        msg->reqID         = _reqID;                                    \
        msg->deviceClass   = _deviceClass;                              \
        CsrBtJsr82MsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtJsr82LdGetSecurityLevelReqSend
 *
 *  DESCRIPTION
 *      Get the security level of the local device
 *
 *   PARAMETERS
 *   phandle:                application handle
 *
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82LdGetSecurityLevelReqSend(_appHandle, _reqID) {       \
        CsrBtJsr82LdGetSecurityLevelReq *msg = (CsrBtJsr82LdGetSecurityLevelReq  *)CsrPmemAlloc(sizeof(CsrBtJsr82LdGetSecurityLevelReq )); \
        msg->type                  = CSR_BT_JSR82_LD_GET_SECURITY_LEVEL_REQ; \
        msg->appHandle             = _appHandle;                        \
        msg->reqID                 = _reqID;                            \
        CsrBtJsr82MsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtJsr82LdIsMasterReqSend
 *
 *  DESCRIPTION
 *      Determine if the local device is master on the connection to device
 *
 *   PARAMETERS
 *   phandle:                application handle
 *
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82LdIsMasterReqSend(_appHandle, _reqID, _deviceAddr) {  \
        CsrBtJsr82LdIsMasterReq *msg = (CsrBtJsr82LdIsMasterReq  *)CsrPmemAlloc(sizeof(CsrBtJsr82LdIsMasterReq )); \
        msg->type                  = CSR_BT_JSR82_LD_IS_MASTER_REQ;     \
        msg->appHandle             = _appHandle;                        \
        msg->reqID                 = _reqID;                            \
        msg->deviceAddr            = _deviceAddr;                       \
        CsrBtJsr82MsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtJsr82SrCreateRecordReqSend
 *
 *  DESCRIPTION
 *      Add a service record created in a (java) app to the SDP layer
 *
 *   PARAMETERS
 *   phandle:                application handle
 *
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82SrCreateRecordReqSend(_phandle, _reqID) {             \
        CsrBtJsr82SrCreateRecordReq *msg = (CsrBtJsr82SrCreateRecordReq  *)CsrPmemAlloc(sizeof(CsrBtJsr82SrCreateRecordReq )); \
        msg->type                  = CSR_BT_JSR82_SR_CREATE_RECORD_REQ; \
        msg->appHandle             = _phandle;                          \
        msg->reqID                 = _reqID;                            \
        CsrBtJsr82MsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtJsr82SrRegisterRecordReqSend
 *
 *  DESCRIPTION
 *      Add a service record created in a (java) app to the SDP layer
 *
 *   PARAMETERS
 *   phandle:                application handle
 *  serviceRecordLength:    length in bytes of the service record to be registered
 *  serviceRecord:          pointer to the service record
 *
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82SrRegisterRecordReqSend(_phandle, _reqID, _serviceRecordLength, _serviceRecord) { \
        CsrBtJsr82SrRegisterRecordReq *msg = (CsrBtJsr82SrRegisterRecordReq  *)CsrPmemAlloc(sizeof(CsrBtJsr82SrRegisterRecordReq )); \
        msg->type                  = CSR_BT_JSR82_SR_REGISTER_RECORD_REQ; \
        msg->appHandle             = _phandle;                          \
        msg->reqID                 = _reqID;                            \
        msg->serviceRecordLength   = _serviceRecordLength;              \
        msg->serviceRecord         = _serviceRecord;                    \
        CsrBtJsr82MsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtJsr82SrRemoveRecordReqSend
 *
 *  DESCRIPTION
 *      Remove a service record from the SDP layer
 *
 *   PARAMETERS
 *   phandle:                application handle
 *  serviceRecHandle:       unique handle of the service record to be removed
 *
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82SrRemoveRecordReqSend(_phandle, _reqID, _serviceRecHandle) { \
        CsrBtJsr82SrRemoveRecordReq *msg = (CsrBtJsr82SrRemoveRecordReq  *)CsrPmemAlloc(sizeof(CsrBtJsr82SrRemoveRecordReq )); \
        msg->type          = CSR_BT_JSR82_SR_REMOVE_RECORD_REQ;         \
        msg->appHandle             = _phandle;                          \
        msg->reqID                 = _reqID;                            \
        msg->serviceRecHandle   = _serviceRecHandle;                    \
        CsrBtJsr82MsgTransport(msg);}



/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtJsr82SrPopulateRecordReqSend
 *
 *  DESCRIPTION
 *      Retrieve attributes to populate a service record from a remote device
 *
 *   PARAMETERS
 *   phandle:                application handle
 *  deviceAddr:             bluetooth address of the remote device, given as a 12 character string
 *  serviceRecHandle:       unique handle of the service record to be populated
 *  attrSetLength:          length in bytes of the set of requested attributes
 *   attributes:             set of requested attributes
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82SrPopulateRecordReqSend(_phandle, _reqID, _deviceAddr, _serviceRecHandle, _serviceDataBaseState, _attrSetLength, _attrSet) { \
        CsrBtJsr82SrPopulateRecordReq *msg = (CsrBtJsr82SrPopulateRecordReq  *)CsrPmemAlloc(sizeof(CsrBtJsr82SrPopulateRecordReq )); \
        msg->type          = CSR_BT_JSR82_SR_POPULATE_RECORD_REQ;       \
        msg->appHandle             = _phandle;                          \
        msg->reqID                 = _reqID;                            \
        msg->deviceAddr            = _deviceAddr;                       \
        msg->serviceRecHandle      = _serviceRecHandle;                 \
        msg->serviceDataBaseState  = _serviceDataBaseState;             \
        msg->attrSetLength         = _attrSetLength;                    \
        msg->attrSet               = _attrSet;                          \
        CsrBtJsr82MsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtJsr82RdGetFriendlyNameReqSend
 *
 *  DESCRIPTION
 *      Retrieve friendly name from a remote device
 *   PARAMETERS
 *   phandle:                application handle
 *   reqID:                  request ID passed down from Java
 *  deviceAddr:             bluetooth address of the remote device, given as a 12 character string
 *  alwaysAsk               CsrBool flag indicating if the name may be cached or not
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82RdGetFriendlyNameReqSend(_phandle, _reqID, _deviceAddr, _alwaysAsk) { \
        CsrBtJsr82RdGetFriendlyNameReq *msg = (CsrBtJsr82RdGetFriendlyNameReq  *)CsrPmemAlloc(sizeof(CsrBtJsr82RdGetFriendlyNameReq )); \
        msg->type          = CSR_BT_JSR82_RD_GET_FRIENDLY_NAME_REQ;     \
        msg->appHandle             = _phandle;                          \
        msg->reqID                 = _reqID;                            \
        msg->address = _deviceAddr;                                     \
        msg->alwaysAsk      = _alwaysAsk;                               \
        CsrBtJsr82MsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtJsr82RdAuthenticateReqSend
 *
 *  DESCRIPTION
 *      Start authentication process with a remote device
 *   PARAMETERS
 *   phandle:                application handle
 *   reqID:                  request ID passed down from Java
 *  deviceAddr:             bluetooth address of the remote device
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82RdAuthenticateReqSend(_phandle, _reqID, _deviceAddr) { \
        CsrBtJsr82RdAuthenticateReq *msg = (CsrBtJsr82RdAuthenticateReq  *)CsrPmemAlloc(sizeof(CsrBtJsr82RdAuthenticateReq )); \
        msg->type          = CSR_BT_JSR82_RD_AUTHENTICATE_REQ;          \
        msg->appHandle             = _phandle;                          \
        msg->reqID                 = _reqID;                            \
        msg->address = _deviceAddr;                                     \
        CsrBtJsr82MsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtJsr82RdIsAuthenticatedReqSend
 *
 *  DESCRIPTION
 *      Determine if the ACL link to the remote device has been authenticated
 *
 *   PARAMETERS
 *   phandle:                application handle
 *
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82RdIsAuthenticatedReqSend(_appHandle, _reqID, _deviceAddr) { \
        CsrBtJsr82RdIsAuthenticatedReq *msg = (CsrBtJsr82RdIsAuthenticatedReq  *)CsrPmemAlloc(sizeof(CsrBtJsr82RdIsAuthenticatedReq )); \
        msg->type          = CSR_BT_JSR82_RD_IS_AUTHENTICATED_REQ;      \
        msg->appHandle     = _appHandle;                                \
        msg->reqID         = _reqID;                                    \
        msg->deviceAddr    = _deviceAddr;                               \
        CsrBtJsr82MsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtJsr82RdEncryptReqSend
 *
 *  DESCRIPTION
 *      Request encryption on all connections for a remote device
 *   PARAMETERS
 *   phandle:                application handle
 *   reqID:                  request ID passed down from Java
 *  deviceAddr:             bluetooth address of the remote device
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82RdEncryptReqSend(_phandle, _reqID, _deviceAddr, _encrypt) { \
        CsrBtJsr82RdEncryptReq *msg = (CsrBtJsr82RdEncryptReq  *)CsrPmemAlloc(sizeof(CsrBtJsr82RdEncryptReq )); \
        msg->type          = CSR_BT_JSR82_RD_ENCRYPT_REQ;               \
        msg->appHandle             = _phandle;                          \
        msg->reqID                 = _reqID;                            \
        msg->address = _deviceAddr;                                     \
        msg->encrypt = _encrypt;                                        \
        CsrBtJsr82MsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtJsr82RdIsEncryptedReqSend
 *
 *  DESCRIPTION
 *      Determine if the ACL link to the remote device is encrypted
 *
 *   PARAMETERS
 *   phandle:                application handle
 *
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82RdIsEncryptedReqSend(_appHandle, _reqID, _deviceAddr) { \
        CsrBtJsr82RdIsEncryptedReq *msg = (CsrBtJsr82RdIsEncryptedReq  *)CsrPmemAlloc(sizeof(CsrBtJsr82RdIsEncryptedReq )); \
        msg->type          = CSR_BT_JSR82_RD_IS_ENCRYPTED_REQ;          \
        msg->appHandle     = _appHandle;                                \
        msg->reqID         = _reqID;                                    \
        msg->deviceAddr    = _deviceAddr;                               \
        CsrBtJsr82MsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtJsr82RdIsTrustedReqSend
 *
 *  DESCRIPTION
 *      Determine if the remote device is trusted
 *
 *   PARAMETERS
 *   phandle:                application handle
 *
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82RdIsTrustedReqSend(_appHandle, _reqID, _deviceAddr) { \
        CsrBtJsr82RdIsTrustedReq *msg = (CsrBtJsr82RdIsTrustedReq  *)CsrPmemAlloc(sizeof(CsrBtJsr82RdIsTrustedReq )); \
        msg->type          = CSR_BT_JSR82_RD_IS_TRUSTED_REQ;            \
        msg->appHandle     = _appHandle;                                \
        msg->reqID         = _reqID;                                    \
        msg->deviceAddr    = _deviceAddr;                               \
        CsrBtJsr82MsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *   NAME
 *       CsrBtJsr82RdIsConnectedReqSend
 *
 *   DESCRIPTION: Used to check if there's an active (Java related) connection
 *                on a specified Bluetooth address
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82RdIsConnectedReqSend(_appHandle, _reqID, _deviceAddr) { \
        CsrBtJsr82RdIsConnectedReq *msg = (CsrBtJsr82RdIsConnectedReq *)CsrPmemAlloc(sizeof(CsrBtJsr82RdIsConnectedReq)); \
        msg->type          = CSR_BT_JSR82_RD_IS_CONNECTED_REQ;          \
        msg->appHandle     = _appHandle;                                \
        msg->reqID         = _reqID;                                    \
        msg->deviceAddr    = _deviceAddr;                               \
        CsrBtJsr82MsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *   NAME
 *       CsrBtJsr82L2caGetPsmReqSend
 *
 *   DESCRIPTION
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82L2caGetPsmReqSend(_phandle, _reqID) {                 \
        CsrBtJsr82L2caGetPsmReq *msg = (CsrBtJsr82L2caGetPsmReq *)CsrPmemAlloc(sizeof(CsrBtJsr82L2caGetPsmReq)); \
        msg->type                  = CSR_BT_JSR82_L2CA_GET_PSM_REQ;     \
        msg->appHandle             = _phandle;                          \
        msg->reqID                 = _reqID;                            \
        CsrBtJsr82MsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *   NAME
 *       Jsr82L2caAcceptAndOpenReqSend
 *
 *   DESCRIPTION
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82L2caAcceptReqSend(_phandle, _reqID, _localPsm, _receiveMtu, _transmitMtu, _attrs) { \
        CsrBtJsr82L2caAcceptReq *msg = (CsrBtJsr82L2caAcceptReq *)CsrPmemAlloc(sizeof(CsrBtJsr82L2caAcceptReq)); \
        msg->type                  = CSR_BT_JSR82_L2CA_ACCEPT_REQ;      \
        msg->appHandle             = _phandle;                          \
        msg->reqID                 = _reqID;                            \
        msg->localPsm              = _localPsm;                         \
        msg->receiveMtu            = _receiveMtu;                       \
        msg->transmitMtu           = _transmitMtu;                      \
        msg->attrs                 = _attrs;                            \
        CsrBtJsr82MsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *   NAME
 *       Jsr82L2caConnectReqSend
 *
 *   DESCRIPTION
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82L2caOpenReqSend(_phandle, _reqID, _deviceAddr, _remotePsm, _receiveMtu, _transmitMtu, _attrs) { \
        CsrBtJsr82L2caOpenReq *msg = (CsrBtJsr82L2caOpenReq *)CsrPmemAlloc(sizeof(CsrBtJsr82L2caOpenReq)); \
        msg->type                  = CSR_BT_JSR82_L2CA_OPEN_REQ;        \
        msg->appHandle             = _phandle;                          \
        msg->reqID                 = _reqID;                            \
        msg->deviceAddr            = _deviceAddr;                       \
        msg->remotePsm             = _remotePsm;                        \
        msg->receiveMtu            = _receiveMtu;                       \
        msg->transmitMtu           = _transmitMtu;                      \
        msg->attrs                 = _attrs;                            \
        CsrBtJsr82MsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *   NAME
 *       CsrBtJsr82L2caDisconnectReqSend
 *
 *   DESCRIPTION: Close all L2CAP connections on a local psm
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82L2caDisconnectReqSend(_phandle, _reqID, _localPsm) {  \
        CsrBtJsr82L2caDisconnectReq *msg = (CsrBtJsr82L2caDisconnectReq *)CsrPmemAlloc(sizeof(CsrBtJsr82L2caDisconnectReq)); \
        msg->type                  = CSR_BT_JSR82_L2CA_DISCONNECT_REQ;  \
        msg->appHandle             = _phandle;                          \
        msg->reqID                 = _reqID;                            \
        msg->localPsm              = _localPsm;                         \
        CsrBtJsr82MsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *   NAME
 *       Jsr82L2caSendDataReqSend
 *
 *   DESCRIPTION
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82L2caTxDataReqSend(_phandle, _reqID, _btConnId, _payloadLength, _payload) { \
        CsrBtJsr82L2caTxDataReq *msg = (CsrBtJsr82L2caTxDataReq *)CsrPmemAlloc(sizeof(CsrBtJsr82L2caTxDataReq)); \
        msg->type                  = CSR_BT_JSR82_L2CA_TX_DATA_REQ;     \
        msg->appHandle             = _phandle;                          \
        msg->reqID                 = _reqID;                            \
        msg->btConnId              = _btConnId;                         \
        msg->payloadLength         = _payloadLength;                    \
        msg->payload               = _payload;                          \
        CsrBtJsr82MsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *   NAME
 *       Jsr82L2caReceiveDataReqSend
 *
 *   DESCRIPTION
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82L2caRxDataReqSend(_phandle, _reqID, _btConnId, _dataLength) { \
        CsrBtJsr82L2caRxDataReq *msg = (CsrBtJsr82L2caRxDataReq *)CsrPmemAlloc(sizeof(CsrBtJsr82L2caRxDataReq)); \
        msg->type                  = CSR_BT_JSR82_L2CA_RX_DATA_REQ;     \
        msg->appHandle             = _phandle;                          \
        msg->reqID                 = _reqID;                            \
        msg->btConnId              = _btConnId;                         \
        msg->dataLength            = _dataLength;                       \
        CsrBtJsr82MsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *   NAME
 *       Jsr82L2caDataReadyReqSend
 *
 *   DESCRIPTION
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82L2caRxReadyReqSend(_phandle, _reqID, _btConnId) {     \
        CsrBtJsr82L2caRxReadyReq *msg = (CsrBtJsr82L2caRxReadyReq *)CsrPmemAlloc(sizeof(CsrBtJsr82L2caRxReadyReq)); \
        msg->type                  = CSR_BT_JSR82_L2CA_RX_READY_REQ;    \
        msg->appHandle             = _phandle;                          \
        msg->reqID                 = _reqID;                            \
        msg->btConnId              = _btConnId;                         \
        CsrBtJsr82MsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *   NAME
 *       CsrBtJsr82L2caGetConfigReqSend
 *
 *   DESCRIPTION: Request the receive and transmit MTU of an L2CAP connection
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82L2caGetConfigReqSend(_phandle, _reqID, _btConnId) {   \
        CsrBtJsr82L2caGetConfigReq *msg = (CsrBtJsr82L2caGetConfigReq *)CsrPmemAlloc(sizeof(CsrBtJsr82L2caGetConfigReq)); \
        msg->type                  = CSR_BT_JSR82_L2CA_GET_CONFIG_REQ;  \
        msg->appHandle             = _phandle;                          \
        msg->reqID                 = _reqID;                            \
        msg->btConnId              = _btConnId;                         \
        CsrBtJsr82MsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *   NAME
 *       CsrBtJsr82L2caCloseReqSend
 *
 *   DESCRIPTION: Close an L2CAP connection
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82L2caCloseReqSend(_phandle, _reqID, _btConnId) {       \
        CsrBtJsr82L2caCloseReq *msg = (CsrBtJsr82L2caCloseReq *)CsrPmemAlloc(sizeof(CsrBtJsr82L2caCloseReq)); \
        msg->type                  = CSR_BT_JSR82_L2CA_CLOSE_REQ;       \
        msg->appHandle             = _phandle;                          \
        msg->reqID                 = _reqID;                            \
        msg->btConnId              = _btConnId;                         \
        CsrBtJsr82MsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *   NAME
 *       CsrBtJsr82RfcGetServerChannelReqSend
 *
 *   DESCRIPTION
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82RfcGetServerChannelReqSend(_phandle, _reqID) {        \
        CsrBtJsr82RfcGetServerChannelReq *msg = (CsrBtJsr82RfcGetServerChannelReq *)CsrPmemAlloc(sizeof(CsrBtJsr82RfcGetServerChannelReq)); \
        msg->type                  = CSR_BT_JSR82_RFC_GET_SERVER_CHANNEL_REQ; \
        msg->appHandle             = _phandle;                          \
        msg->reqID                 = _reqID;                            \
        CsrBtJsr82MsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *   NAME
 *       CsrBtJsr82RfcAcceptAndOpenReqSend
 *
 *   DESCRIPTION
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82RfcAcceptAndOpenReqSend(_phandle, _reqID, _localServerChannel, _attrs) { \
        CsrBtJsr82RfcAcceptAndOpenReq *msg = (CsrBtJsr82RfcAcceptAndOpenReq *)CsrPmemAlloc(sizeof(CsrBtJsr82RfcAcceptAndOpenReq)); \
        msg->type                  = CSR_BT_JSR82_RFC_ACCEPT_AND_OPEN_REQ; \
        msg->appHandle             = _phandle;                          \
        msg->reqID                 = _reqID;                            \
        msg->localServerChannel    = _localServerChannel;               \
        msg->attrs                 = _attrs;                            \
        CsrBtJsr82MsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *   NAME
 *       CsrBtJsr82RfcConnectReqSend
 *
 *   DESCRIPTION
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82RfcConnectReqSend(_phandle, _reqID, _remoteServerChannel, _deviceAddr, _attrs) { \
        CsrBtJsr82RfcConnectReq  *msg = (CsrBtJsr82RfcConnectReq*)CsrPmemAlloc(sizeof(CsrBtJsr82RfcConnectReq)); \
        msg->type                  = CSR_BT_JSR82_RFC_CONNECT_REQ;      \
        msg->appHandle             = _phandle;                          \
        msg->reqID                 = _reqID;                            \
        msg->remoteServerChannel    = _remoteServerChannel;             \
        msg->deviceAddr             = _deviceAddr;                      \
        msg->attrs                 = _attrs;                            \
        CsrBtJsr82MsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *   NAME
 *       CsrBtJsr82RfcDisconnectReqSend
 *
 *   DESCRIPTION
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82RfcDisconnectReqSend(_phandle, _reqID, _localServerChannel) { \
        CsrBtJsr82RfcDisconnectReq  *msg = (CsrBtJsr82RfcDisconnectReq *)CsrPmemAlloc(sizeof(CsrBtJsr82RfcDisconnectReq)); \
        msg->type                  = CSR_BT_JSR82_RFC_DISCONNECT_REQ;   \
        msg->appHandle             = _phandle;                          \
        msg->reqID                 = _reqID;                            \
        msg->localServerChannel    = _localServerChannel;               \
        CsrBtJsr82MsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *   NAME
 *       CsrBtJsr82RfcCloseReqSend
 *
 *   DESCRIPTION
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82RfcCloseReqSend(_phandle, _reqID, _localServerChannel) { \
        CsrBtJsr82RfcCloseReq  *msg = (CsrBtJsr82RfcCloseReq *)CsrPmemAlloc(sizeof(CsrBtJsr82RfcCloseReq)); \
        msg->type                  = CSR_BT_JSR82_RFC_CLOSE_REQ;        \
        msg->appHandle             = _phandle;                          \
        msg->reqID                 = _reqID;                            \
        msg->localServerChannel    = _localServerChannel;               \
        CsrBtJsr82MsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *   NAME
 *       CsrBtJsr82RfcSendDataReqSend
 *
 *   DESCRIPTION
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82RfcSendDataReqSend(_phandle, _reqID, _localServerChannel, _payloadLength, _payload) { \
        CsrBtJsr82RfcSendDataReq *msg = (CsrBtJsr82RfcSendDataReq *)CsrPmemAlloc(sizeof(CsrBtJsr82RfcSendDataReq)); \
        msg->type                  = CSR_BT_JSR82_RFC_SEND_DATA_REQ;    \
        msg->appHandle             = _phandle;                          \
        msg->reqID                 = _reqID;                            \
        msg->localServerChannel    = _localServerChannel;               \
        msg->payloadLength         = _payloadLength;                    \
        msg->payload               = _payload;                          \
        CsrBtJsr82MsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *   NAME
 *       CsrBtJsr82RfcReceiveDataReqSend
 *
 *   DESCRIPTION
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82RfcReceiveDataReqSend(_phandle, _reqID, _localServerChannel, _bytesToRead) { \
        CsrBtJsr82RfcReceiveDataReq *msg = (CsrBtJsr82RfcReceiveDataReq *)CsrPmemAlloc(sizeof(CsrBtJsr82RfcReceiveDataReq)); \
        msg->type                  = CSR_BT_JSR82_RFC_RECEIVE_DATA_REQ; \
        msg->appHandle             = _phandle;                          \
        msg->reqID                 = _reqID;                            \
        msg->localServerChannel    = _localServerChannel;               \
        msg->bytesToRead           = _bytesToRead;                      \
        CsrBtJsr82MsgTransport(msg);}


/*----------------------------------------------------------------------------*
 *   NAME
 *       CsrBtJsr82RfcGetAvailableReqSend
 *
 *   DESCRIPTION
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82RfcGetAvailableReqSend(_phandle, _reqID, _localServerChannel) { \
        CsrBtJsr82RfcGetAvailableReq  *msg = (CsrBtJsr82RfcGetAvailableReq *)CsrPmemAlloc(sizeof(CsrBtJsr82RfcGetAvailableReq)); \
        msg->type                  = CSR_BT_JSR82_RFC_GET_AVAILABLE_REQ; \
        msg->appHandle             = _phandle;                          \
        msg->reqID                 = _reqID;                            \
        msg->localServerChannel = _localServerChannel;                  \
        CsrBtJsr82MsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *   NAME
 *       CsrBtJsr82CleanupReqSend
 *
 *   DESCRIPTION
 *----------------------------------------------------------------------------*/
#define CsrBtJsr82CleanupReqSend() {                                    \
        CsrBtJsr82CleanupReq *msg = (CsrBtJsr82CleanupReq *)CsrPmemAlloc(sizeof(CsrBtJsr82CleanupReq)); \
        msg->type                 = CSR_BT_JSR82_CLEANUP_REQ;           \
        CsrBtJsr82MsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtJsr82SetEventMaskReqSend
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
#define CsrBtJsr82SetEventMaskReqSend(_phandle, _eventMask, _conditionMask) { \
        CsrBtJsr82SetEventMaskReq *msg = (CsrBtJsr82SetEventMaskReq *)CsrPmemAlloc(sizeof(CsrBtJsr82SetEventMaskReq)); \
        msg->type                  = CSR_BT_JSR82_SET_EVENT_MASK_REQ;   \
        msg->phandle               = _phandle;                          \
        msg->eventMask             = _eventMask;                        \
        msg->conditionMask         = _conditionMask;                    \
        CsrBtJsr82MsgTransport(msg);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtJsr82FreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT JSR82
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_JSR82_PRIM,
 *      msg:          The message received from Synergy BT JSR82
 *----------------------------------------------------------------------------*/
void CsrBtJsr82FreeUpstreamMessageContents(CsrUint16 eventClass, void * message);

#ifdef __cplusplus
}
#endif

#endif

