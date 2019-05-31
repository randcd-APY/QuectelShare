#ifndef CSR_BT_SC_LIB_H__
#define CSR_BT_SC_LIB_H__

/******************************************************************************
 Copyright (c) 2001-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

 REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_bt_profiles.h"
#include "csr_bt_sc_db.h"
#include "dm_prim.h"
#include "csr_bt_sc_prim.h"
#include "csr_pmem.h"
#include "csr_util.h"
#include "csr_bt_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtScMsgTransport(void* msg);

#ifdef CSR_BT_INSTALL_SC_MODE
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScModeReqSend
 *
 *  DESCRIPTION
 *      Request to change the mode
 *
 *    PARAMETERS
 *      deviceAddr:         address of device which should be notified
 *      duration:           the duration of the mode
 *      mode:               the mode of SC. When the duration
 *                          has passed, the previous mode takes effect.
 *                          To set bondable forever, use CSR_BT_INFINITE_TIME.
 *----------------------------------------------------------------------------*/
#define CsrBtScModeReqSend(_duration, _mode) {                          \
        CsrBtScModeReq *msg__ = (CsrBtScModeReq *) CsrPmemAlloc(sizeof(CsrBtScModeReq)); \
        msg__->type = CSR_BT_SC_MODE_REQ;                               \
        msg__->duration = _duration;                                    \
        msg__->mode = _mode;                                            \
        CsrBtScMsgTransport(msg__);}
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScBondReqSend
 *
 *  DESCRIPTION
 *      Function to build and send a CSR_BT_SC_BOND_REQ message to the SC message
 *        queue.
 *
 *    PARAMETERS
 *        phandle:        protocol handle
 *        deviceAddr:     address of device to bond with.
 *----------------------------------------------------------------------------*/
#define CsrBtScBondReqSend(_phandle,_theDeviceAddr) {                   \
        CsrBtScBondReq *msg__ = (CsrBtScBondReq *) CsrPmemAlloc(sizeof(CsrBtScBondReq)); \
        msg__->type = CSR_BT_SC_BOND_REQ;                               \
        msg__->phandle = _phandle;                                      \
        msg__->deviceAddr = _theDeviceAddr;                             \
        CsrBtScMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScDebondReqSend
 *
 *  DESCRIPTION
 *        Remove an existing bond between a pair of devices.
 *
 *    PARAMETERS
 *        phandle:        protocol handle
 *        deviceAddr:     address of device to bond with.

 *----------------------------------------------------------------------------*/
#define CsrBtScDebondReqSendEx(_phandle, _deviceAddr, _addressType)     \
    do                                                                  \
    {                                                                   \
        CsrBtScDebondReq *msg__ = (CsrBtScDebondReq *) CsrPmemAlloc(sizeof(CsrBtScDebondReq)); \
        msg__->type = CSR_BT_SC_DEBOND_REQ;                             \
        msg__->phandle = _phandle;                                      \
        msg__->deviceAddr = _deviceAddr;                                \
        msg__->addressType = _addressType;                              \
        msg__->keyType = CSR_BT_SC_LE_KEY_TYPE_ALL;                     \
        CsrBtScMsgTransport(msg__);                                     \
    } while(0)

#define CsrBtScDebondReqSend(_phandle,_deviceAddr) CsrBtScDebondReqSendEx(_phandle,_deviceAddr,CSR_BT_ADDR_PUBLIC)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScPasskeyResSend
 *
 *  DESCRIPTION
 *      ...

 *
 *    PARAMETERS
 *      accept:         TRUE to accept the passkey request, FALSE to reject
 *      deviceAddr:     address of device for which a passkey is requested
 *      passKeyLength:  The length of the passKey. The maximum number is 16
 *      passKey:        The passKey
 *      addDevice:      If TRUE the device is added to the Security Manager's device database
 *      authorised:     TRUE if authorisation is automatic granted, FALSE if not
 *----------------------------------------------------------------------------*/
void CsrBtScPasskeyResSendEx(CsrBool accept, CsrBtDeviceAddr deviceAddr, CsrUint8 passkeyLength, CsrUint8 *passkey, CsrBool addDevice, CsrBool authorised, CsrBtAddressType addressType);

#define CsrBtScPasskeyResSend(_accept, _theDeviceAddr, _thePasskeyLength, _thePasskey, _addDevice, _authorised) CsrBtScPasskeyResSendEx(_accept, _theDeviceAddr, _thePasskeyLength, _thePasskey, _addDevice, _authorised, CSR_BT_ADDR_PUBLIC)
#define CsrBtScPasskeyNegResSend(_d) CsrBtScPasskeyResSend(FALSE,_d, 0, NULL, FALSE, FALSE)
#define CsrBtScPasskeyNegResSendEx(_d,_at) CsrBtScPasskeyResSendEx(FALSE,_d, 0, NULL, FALSE, FALSE, _at)


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScSetSecurityModeReqSend
 *
 *  DESCRIPTION
 *      Set the security mode to be used by the device
 *
 *    PARAMETERS
 *        phandle:        protocol handle
 *        deviceAddr:     address of device to bond with.
 *
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_SC_SECURITY_MODE
#define CsrBtScSetSecurityModeReqSend(_phandle,_securityMode) {         \
        CsrBtScSetSecurityModeReq *msg__ = (CsrBtScSetSecurityModeReq *) CsrPmemAlloc(sizeof(CsrBtScSetSecurityModeReq)); \
        msg__->type = CSR_BT_SC_SET_SECURITY_MODE_REQ;                  \
        msg__->phandle = _phandle;                                      \
        msg__->securityMode = _securityMode;                            \
        CsrBtScMsgTransport(msg__);}
#endif
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScEncryptionReqSend
 *
 *  DESCRIPTION
 *      Change the encryption mode for a specified device connection (BdAddr)
 *
 *    PARAMETERS
 *        phandle:            protocol handle
 *        theDeviceAddr:        address of device for which encryption mode should change
 *        theEncryptionMode:    wanted encryption mode (ON=TRUE, OFF=FALSE)
 *
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_SC_ENCRYPTION
#define CsrBtScEncryptionReqSend(_thePhandle,_theDeviceAddr,_theEncryptionMode) { \
        CsrBtScEncryptionReq *msg__ = (CsrBtScEncryptionReq *) CsrPmemAlloc(sizeof(CsrBtScEncryptionReq)); \
        msg__->type = CSR_BT_SC_ENCRYPTION_REQ;                         \
        msg__->phandle = _thePhandle;                                   \
        msg__->encEnable = _theEncryptionMode;                          \
        msg__->deviceAddr = _theDeviceAddr;                             \
        CsrBtScMsgTransport(msg__);}
#endif
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScAuthoriseResSend
 *
 *  DESCRIPTION
 *      .....
 *
 *    PARAMETERS
 *        authorised:     TRUE if authorisation granted, FALSE to reject
 *        deviceAddr:     address of device to bond with.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtScAuthoriseResSend(_authorised,_deviceAddr) {              \
        CsrBtScAuthoriseRes *msg__ = (CsrBtScAuthoriseRes *) CsrPmemAlloc(sizeof(CsrBtScAuthoriseRes)); \
        msg__->type = CSR_BT_SC_AUTHORISE_RES;                          \
        msg__->authorised = _authorised;                                \
        msg__->deviceAddr = _deviceAddr;                                \
        CsrBtScMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScActivateReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        phandle    Phandle to register in the SC for handling security
 *---------------------------------------------------------------------------*/
#define CsrBtScActivateReqSend(_phandle) {                              \
        CsrBtScActivateReq *msg__ = (CsrBtScActivateReq *) CsrPmemAlloc(sizeof(CsrBtScActivateReq)); \
        msg__->type = CSR_BT_SC_ACTIVATE_REQ;                           \
        msg__->phandle = _phandle;                                      \
        CsrBtScMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScSetTrustLevelReqSend
 *
 *  DESCRIPTION
 *      ......
 *
 *    PARAMETERS
 *        authorised:     TRUE=trusted, FALSE=untrusted.
 *        deviceAddr:     address of device to bond with.
 *
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_SC_TRUST_LEVEL
#define CsrBtScSetTrustLevelReqSend(_authorised, _deviceAddr) {         \
        CsrBtScSetTrustLevelReq *msg__ = (CsrBtScSetTrustLevelReq *) CsrPmemAlloc(sizeof(CsrBtScSetTrustLevelReq)); \
        msg__->type = CSR_BT_SC_SET_TRUST_LEVEL_REQ;                    \
        msg__->authorised = _authorised;                                \
        msg__->deviceAddr = _deviceAddr;                                \
        msg__->addressType = CSR_BT_ADDR_PUBLIC;                        \
        CsrBtScMsgTransport(msg__);}
#endif
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScUpdateDeviceReqSend
 *
 *  DESCRIPTION
 *      Function to update the properties of a stored device
 *
 *    PARAMETERS
 *        phandle:            handle to the queue where SC must return the confirm
 *        deviceProperties:   properties of the device which is to be updated
 *
 *----------------------------------------------------------------------------*/
#define CsrBtScUpdateDeviceReqSend(_phandle,_deviceProperties) {        \
        CsrBtScUpdateDeviceReq *msg__ = (CsrBtScUpdateDeviceReq *) CsrPmemAlloc(sizeof(CsrBtScUpdateDeviceReq)); \
        msg__->type = CSR_BT_SC_UPDATE_DEVICE_REQ;                      \
        msg__->remoteName = CsrUtf8StrDup(_deviceProperties->friendlyName); \
        msg__->classOfDevice = _deviceProperties->classOfDevice;        \
        msg__->deviceAddr = _deviceProperties->deviceAddr;              \
        msg__->addressType = _deviceProperties->addrType;               \
        msg__->knownServices11_00_31 = _deviceProperties->knownServices11_00_31; \
        msg__->knownServices11_32_63 = _deviceProperties->knownServices11_32_63; \
        msg__->knownServices12_00_31 = _deviceProperties->knownServices12_00_31; \
        msg__->knownServices13_00_31 = _deviceProperties->knownServices13_00_31; \
        msg__->authorised = _deviceProperties->authorised;              \
        msg__->phandle = _phandle;                                      \
        CsrPmemFree(_deviceProperties);                                 \
        CsrBtScMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScReadDeviceRecordReqSend
 *
 *  DESCRIPTION
 *      Function to read the entire SC DB
 *
 *    PARAMETERS
 *        phandle:        handle to the queue where SC must return indications and confirm
 *        maxNumOfBytes:    maximum allowed bytes in a result indication
 *
 *----------------------------------------------------------------------------*/
#define CsrBtScReadDeviceRecordReqSend(_phandle,_maxNumOfBytes) {       \
        CsrBtScReadDeviceRecordReq *msg__ = (CsrBtScReadDeviceRecordReq *) CsrPmemAlloc(sizeof(*msg__)); \
        msg__->type = CSR_BT_SC_READ_DEVICE_RECORD_REQ;                 \
        msg__->maxNumOfBytes = _maxNumOfBytes;                          \
        msg__->phandle = _phandle;                                      \
        msg__->flags   = CSR_BT_SC_READ_RECORD_FLAG_DEFAULT;            \
        CsrBtScMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScLeReadResolvedDeviceRecordReqSend
 *
 *  DESCRIPTION
 *      Retrieves the list of all LE trusted devices whose RPAs can be resolved.
 *      SC responds the list of LE devices whose IRKs are available in persistent
 *      storage. Device list is sent through CSR_BT_SC_READ_DEVICE_RECORD_IND/CFM.
 *
 *      Note: When controller based privacy is used, all peer device IRKs are 
 *      stored in controller. If controller's resolving list is full, pairing
 *      with devices sharing IRK fails with result code
 *      CSR_BT_RESULT_CODE_SC_RESOLVING_LIST_FULL. In order to bond with such
 *      device, applications must first debond with unwanted existing device(s)
 *      from resolving list to create vacancy. This API can be used to
 *      retrive the records of such devices currently stored in resolving list.
 *  PARAMETERS
 *      phandle      :  protocol handle
 *      maxNumOfBytes:  maximum allowed bytes in a result indication
 *----------------------------------------------------------------------------*/
#define CsrBtScLeReadResolvedDeviceRecordReqSend(_phandle, _maxNumOfBytes)     \
    do                                                                         \
    {                                                                          \
        CsrBtScReadDeviceRecordReq *msg__;                                     \
        msg__ = (CsrBtScReadDeviceRecordReq *) CsrPmemAlloc(sizeof(*msg__));   \
        msg__->type          = CSR_BT_SC_READ_DEVICE_RECORD_REQ;               \
        msg__->maxNumOfBytes = _maxNumOfBytes;                                 \
        msg__->phandle       = _phandle;                                       \
        msg__->flags         = CSR_BT_SC_READ_RECORD_FLAG_LE_RESOLVED;         \
        CsrBtScMsgTransport(msg__);                                            \
   } while(0)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScCancelBondReqSend
 *
 *  DESCRIPTION
 *        Cancel a CSR_BT_SC_BOND_REQ
 *
 *    PARAMETERS
 *        phandle:        protocol handle
 *        deviceAddr:        address of device to cancel bond.
 *----------------------------------------------------------------------------*/
#define CsrBtScCancelBondReqSendEx(_thePhandle,_theDeviceAddr,_addressType) { \
        CsrBtScCancelBondReq *msg__ = (CsrBtScCancelBondReq *) CsrPmemAlloc(sizeof(CsrBtScCancelBondReq)); \
        msg__->type = CSR_BT_SC_CANCEL_BOND_REQ;                        \
        msg__->phandle = _thePhandle;                                   \
        msg__->deviceAddr = _theDeviceAddr;                             \
        msg__->addressType = _addressType;                              \
        CsrBtScMsgTransport(msg__);}

#define CsrBtScCancelBondReqSend(_thePhandle,_theDeviceAddr) CsrBtScCancelBondReqSendEx(_thePhandle, _theDeviceAddr, CSR_BT_ADDR_PUBLIC)

#ifdef CSR_BT_INSTALL_SC_SET_IO_CAPABILITY
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScSetIoCapabilityReqSend
 *
 *  DESCRIPTION
 *        Set IO capability of the local device
 *
 *    PARAMETERS
 *        ioCapability:   IO capability of the local device.
 *----------------------------------------------------------------------------*/
#define CsrBtScSetIoCapabilityReqSend(_ioCapability) {                  \
        CsrBtScSetIoCapabilityReq *msg__ = (CsrBtScSetIoCapabilityReq *) CsrPmemAlloc(sizeof(CsrBtScSetIoCapabilityReq)); \
        msg__->type = CSR_BT_SC_SET_IO_CAPABILITY_REQ;                  \
        msg__->ioCapability = _ioCapability;                            \
        CsrBtScMsgTransport(msg__);}
#endif /* CSR_BT_INSTALL_SC_SET_IO_CAPABILITY */

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScSetIoCapabilityReqSend
 *
 *  DESCRIPTION
 *        Set authentication requirements of the local device
 *
 *    PARAMETERS
 *        authRequirements:   Authentication requirements of the local device.
 *----------------------------------------------------------------------------*/
#define CsrBtScSetAuthRequirementsReqSendEx(_authRequirements,_leAuthRequirements) { \
        CsrBtScSetAuthRequirementsReq *msg__ = (CsrBtScSetAuthRequirementsReq *) CsrPmemAlloc(sizeof(CsrBtScSetAuthRequirementsReq)); \
        msg__->type = CSR_BT_SC_SET_AUTH_REQUIREMENTS_REQ;              \
        msg__->authRequirements = _authRequirements;                    \
        msg__->leAuthRequirements = _leAuthRequirements;                \
        CsrBtScMsgTransport(msg__);}

#define CsrBtScSetAuthRequirementsReqSend(_authRequirements) CsrBtScSetAuthRequirementsReqSendEx(_authRequirements,CSR_BT_SC_LE_USE_DEFAULT)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScSspCompareResSend
 *
 *  DESCRIPTION
 *      Respond to a Numeric Comparison indication
 *
 *    PARAMETERS
 *      accept:         TRUE to accept the comparison request, FALSE to reject
 *      deviceAddr:     address of device for which a passkey is requested
 *      addDevice:      If TRUE the device is added to the Security Manager's device database
 *      authorised:     TRUE if authorisation is automatic granted, FALSE if not
 *----------------------------------------------------------------------------*/
#if CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1
#define CsrBtScSspCompareResSendEx(_accept,_deviceAddr, _addDevice, _authorised, _addressType) { \
        CsrBtScSspCompareRes *msg__ = (CsrBtScSspCompareRes *) CsrPmemAlloc(sizeof(CsrBtScSspCompareRes)); \
        msg__->type = CSR_BT_SC_SSP_COMPARE_RES;                        \
        msg__->accept = _accept;                                        \
        msg__->deviceAddr = _deviceAddr;                                \
        msg__->addDevice = _addDevice;                                  \
        msg__->authorised = _authorised;                                \
        msg__->addressType = _addressType;                              \
        CsrBtScMsgTransport(msg__);}

#define CsrBtScSspCompareResSend(_accept,_deviceAddr, _addDevice, _authorised) CsrBtScSspCompareResSendEx(_accept,_deviceAddr, _addDevice, _authorised, CSR_BT_ADDR_PUBLIC)

#else
#define CsrBtScSspCompareResSend(_a,_d, _ad, _au)
#define CsrBtScSspCompareResSendEx(_a,_d, _ad, _au, _at)
#endif

#define CsrBtScSspCompareNegResSend(_d) CsrBtScSspCompareResSend(FALSE,_d, FALSE, FALSE)
#define CsrBtScSspCompareNegResSendEx(_d,_at) CsrBtScSspCompareResSendEx(FALSE,_d, FALSE, FALSE, _at)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScSspNotificationResSend
 *
 *  DESCRIPTION
 *      Respond to a SSP Passkey Notification indication
 *
 *    PARAMETERS
 *      accept:         TRUE to accept the passkey notification request, FALSE to reject
 *      deviceAddr:     address of device for which a passkey is requested
 *      addDevice:      If TRUE the device is added to the Security Manager's device database
 *      authorised:     TRUE if authorisation is automatic granted, FALSE if not
 *----------------------------------------------------------------------------*/
#if CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1
#define CsrBtScSspNotificationResSendEx(_accept,_deviceAddr, _addDevice, _authorised, _addressType) { \
        CsrBtScSspNotificationRes *msg__ = (CsrBtScSspNotificationRes *) CsrPmemAlloc(sizeof(CsrBtScSspNotificationRes)); \
        msg__->type = CSR_BT_SC_SSP_NOTIFICATION_RES;                   \
        msg__->accept = _accept;                                        \
        msg__->deviceAddr = _deviceAddr;                                \
        msg__->addDevice = _addDevice;                                  \
        msg__->authorised = _authorised;                                \
        msg__->addressType = _addressType;                              \
        CsrBtScMsgTransport(msg__);}

#define CsrBtScSspNotificationResSend(_accept,_deviceAddr, _addDevice, _authorised) CsrBtScSspNotificationResSendEx(_accept,_deviceAddr, _addDevice, _authorised, CSR_BT_ADDR_PUBLIC)

#else
#define CsrBtScSspNotificationResSend(_a,_d, _ad, _au)
#define CsrBtScSspNotificationResSendEx(_a,_d, _ad, _au, _at)
#endif

#define CsrBtScSspNotificationNegResSend(_d) CsrBtScSspNotificationResSend(FALSE, _d, FALSE, FALSE)
#define CsrBtScSspNotificationNegResSendEx(_d, _at) CsrBtScSspNotificationResSendEx(FALSE, _d, FALSE, FALSE, _at)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScSspPasskeyResSend
 *
 *  DESCRIPTION
 *      Respond to a SSP Passkey indication
 *
 *    PARAMETERS
 *      accept:         TRUE to accept the SSP passkey request, FALSE to reject
 *      deviceAddr:     address of device for which a passkey is requested
 *      numericValue:   The numerical value
 *      addDevice:      If TRUE the device is added to the Security Manager's device database
 *      authorised:     TRUE if authorisation is automatic granted, FALSE if not
 *----------------------------------------------------------------------------*/
#if CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1
#define CsrBtScSspPasskeyResSendEx(_accept, _deviceAddr, _numericValue, _addDevice, _authorised, _addressType) { \
        CsrBtScSspPasskeyRes *msg__ = (CsrBtScSspPasskeyRes *) CsrPmemAlloc(sizeof(CsrBtScSspPasskeyRes)); \
        msg__->type = CSR_BT_SC_SSP_PASSKEY_RES;                        \
        msg__->accept = _accept;                                        \
        msg__->deviceAddr = _deviceAddr;                                \
        msg__->numericValue = _numericValue;                            \
        msg__->addDevice = _addDevice;                                  \
        msg__->authorised = _authorised;                                \
        msg__->addressType = _addressType;                              \
        CsrBtScMsgTransport(msg__);}

#define CsrBtScSspPasskeyResSend(_accept, _deviceAddr, _numericValue, _addDevice, _authorised) CsrBtScSspPasskeyResSendEx(_accept, _deviceAddr, _numericValue, _addDevice, _authorised, CSR_BT_ADDR_PUBLIC)

#else
#define CsrBtScSspPasskeyResSend(_a, _d, _n, _ad, _au)
#define CsrBtScSspPasskeyResSendEx(_a, _d, _n, _ad, _au, _at)
#endif

#define CsrBtScSspPasskeyNegResSend(_d) CsrBtScSspPasskeyResSend(FALSE, _d, 0, FALSE, FALSE)
#define CsrBtScSspPasskeyNegResSendEx(_d,_at) CsrBtScSspPasskeyResSendEx(FALSE, _d, 0, FALSE, FALSE, _at)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScSspJustWorksResSend
 *
 *  DESCRIPTION
 *      Respond to a SSP Just Works indication
 *
 *    PARAMETERS
 *      accept:         TRUE to accept the just works request, FALSE to reject
 *      deviceAddr:     address of device for which just works is requested
 *      addDevice:      If TRUE the device is added to the Security Manager's device database
 *      authorised:     TRUE if authorisation is automatic granted, FALSE if not
 *----------------------------------------------------------------------------*/
#if CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1
#define CsrBtScSspJustWorksResSendEx(_accept,_deviceAddr, _addDevice, _authorised,_addressType) { \
        CsrBtScSspJustworksRes *msg__ = (CsrBtScSspJustworksRes *) CsrPmemAlloc(sizeof(CsrBtScSspJustworksRes)); \
        msg__->type = CSR_BT_SC_SSP_JUSTWORKS_RES;                      \
        msg__->accept = _accept;                                        \
        msg__->deviceAddr = _deviceAddr;                                \
        msg__->addDevice = _addDevice;                                  \
        msg__->authorised = _authorised;                                \
        msg__->addressType = _addressType;                              \
        CsrBtScMsgTransport(msg__);}

#define CsrBtScSspJustWorksResSend(_accept,_deviceAddr, _addDevice, _authorised) CsrBtScSspJustWorksResSendEx(_accept,_deviceAddr, _addDevice, _authorised, CSR_BT_ADDR_PUBLIC)

#else
#define CsrBtScSspJustWorksResSend(_ac,_d, _ad, _au)
#define CsrBtScSspJustWorksResSendEx(_ac,_d, _ad, _au, _at)
#endif

#define CsrBtScSspJustWorksNegResSend(_d) CsrBtScSspJustWorksResSend(FALSE, _d, FALSE, FALSE)
#define CsrBtScSspJustWorksNegResSendEx(_d, _at) CsrBtScSspJustWorksResSendEx(FALSE, _d, FALSE, FALSE, _at)

#ifdef CSR_BT_INSTALL_SC_OOB
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScReadLocalOobDataReqSend
 *
 *  DESCRIPTION
 *
 *
 *    PARAMETERS
 *        phandle:        protocol handle
 *----------------------------------------------------------------------------*/
#if CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1
#define CsrBtScReadLocalOobDataReqSend(_phandle, _transportType)        \
    do {                                                                \
        CsrBtScReadLocalOobDataReq *msg__ = (CsrBtScReadLocalOobDataReq *) CsrPmemAlloc(sizeof(*msg__)); \
        msg__->type = CSR_BT_SC_READ_LOCAL_OOB_DATA_REQ;                \
        msg__->phandle = _phandle;                                      \
        msg__->transportType = _transportType;                          \
        CsrBtScMsgTransport(msg__);                                     \
    } while (0)
#else
#define CsrBtScReadLocalOobDataReqSend(_thePhandle)
#endif
#endif
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScSspKeypressNotificationReqSend
 *
 *  DESCRIPTION
 *      Send a SSP Keypress notification to remote device
 *
 *    PARAMETERS
 *      deviceAddr:         address of device which should be notified
 *      addressType:        Address type
 *      notificationType:   the type of notification
 *----------------------------------------------------------------------------*/
#if CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1
#define CsrBtScSspKeypressNotificationReqSend(_notificationType)    \
    do                                                              \
    {                                                               \
        CsrBtScSspKeypressNotificationReq *msg = (CsrBtScSspKeypressNotificationReq *) CsrPmemAlloc(sizeof(*msg)); \
        msg->type = CSR_BT_SC_SSP_KEYPRESS_NOTIFICATION_REQ;        \
        msg->notificationType = _notificationType;                  \
        CsrBtScMsgTransport(msg);                                   \
    } while (0)

#else
#define CsrBtScSspKeypressNotificationReqSend(_notificationType)
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScAuthenticateReqSend
 *
 *  DESCRIPTION
 *      Function to build and send a CSR_BT_SC_AUTHENTICATE_REQ message to the SC message
 *      queue.
 *
 *  PARAMETERS
 *      phandle:            Handle for the requesting process
 *      deviceAddr:         Address of the remote device being authenticated
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_SC_AUTHENTICATE
#define CsrBtScAuthenticateReqSend(_phandle, _deviceAddr) {             \
        CsrBtScAuthenticateReq *msg__ = (CsrBtScAuthenticateReq *) CsrPmemAlloc(sizeof(CsrBtScAuthenticateReq)); \
        msg__->type = CSR_BT_SC_AUTHENTICATE_REQ;                       \
        msg__->phandle = _phandle;                                      \
        msg__->deviceAddr = _deviceAddr;                                \
        CsrBtScMsgTransport(msg__);}
#endif
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScRebondResSend
 *
 *  DESCRIPTION
 *      Function to build and send a CSR_BT_SC_REBOND_RES message to the SC
 *      queue.
 *
 *  PARAMETERS
 *      deviceAddr:         Address of the remote device being authenticated
 *      accept:             TRUE to accept Rebonding, FALSE to reject
 *----------------------------------------------------------------------------*/
#define CsrBtScRebondResSend(_deviceAddr, _accept) {                    \
        CsrBtScRebondRes *msg__ = (CsrBtScRebondRes *) CsrPmemAlloc(sizeof(CsrBtScRebondRes)); \
        msg__->type = CSR_BT_SC_REBOND_RES;                             \
        msg__->accept = _accept;                                        \
        msg__->deviceAddr = _deviceAddr;                                \
        CsrBtScMsgTransport(msg__);}

#define CsrBtScRebondNegResSend(_d) CsrBtScRebondResSend(_d, FALSE)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScSspPairingResSend
 *
 *  DESCRIPTION
 *      Function to build and send a CSR_BT_SC_SSP_PAIRING_RES message to the SC
 *      queue.
 *
 *  PARAMETERS
 *      deviceAddr:     Address of the remote device being authenticated
 *      accept:         TRUE to accept Rebonding, FALSE to reject
 *      addDevice:      If TRUE the device is added to the Security Manager's device database
 *      authorised:     TRUE if authorisation is automatic granted, FALSE if not
 *----------------------------------------------------------------------------*/
#define CsrBtScSspPairingResSendEx(_deviceAddr,  _addressType, _accept, _addDevice, _authorised) { \
        CsrBtScSspPairingRes *msg__ = (CsrBtScSspPairingRes *) CsrPmemAlloc(sizeof(CsrBtScSspPairingRes)); \
        msg__->type = CSR_BT_SC_SSP_PAIRING_RES;                        \
        msg__->deviceAddr = _deviceAddr;                                \
        msg__->addressType = _addressType;                              \
        msg__->accept = _accept;                                        \
        msg__->addDevice = _addDevice;                                  \
        msg__->authorised = _authorised;                                \
        CsrBtScMsgTransport(msg__);}

#define CsrBtScSspPairingResSend(_deviceAddr, _accept, _addDevice, _authorised)     \
                CsrBtScSspPairingResSendEx(_deviceAddr,                             \
                                           CSR_BT_ADDR_PUBLIC,                      \
                                           _accept,                                 \
                                           _addDevice,                              \
                                           _authorised)

#define CsrBtScSspPairingNegResSend(_d)                 \
    CsrBtScSspPairingResSend(_d, FALSE, FALSE, FALSE)
#define CsrBtScSspPairingNegResSendEx(_d, _at, _tt)                  \
    CsrBtScSspPairingResSendEx(_d, _at, FALSE, FALSE, FALSE)

#ifdef CSR_BT_INSTALL_SC_SET_EVENT_MASK
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScSetEventMaskReqSendEx
 *
 *  DESCRIPTION
 *      Interface to subscribe for particular event which application.is
 *      interested to get informed when ever the requested event occurs in SC.
 *      Apart from eventMask the subscriber shall provide its protocol handle.
 *      SC uses subscribed handles to inform the corresponding subscribers.
 *
 *  PARAMETERS
 *        phandle    :  Protocol handle
 *        eventMask  :  Describes which extended information an application is
 *                      subscribing for
 *----------------------------------------------------------------------------*/
#define CsrBtScSetEventMaskReqSendEx(_phandle, _eventMask)                     \
    do                                                                         \
    {                                                                          \
        CsrBtScSetEventMaskReq *msg__;                                         \
        msg__ = (CsrBtScSetEventMaskReq *) CsrPmemAlloc(sizeof(*msg__));       \
        msg__->type      = CSR_BT_SC_SET_EVENT_MASK_REQ;                       \
        msg__->phandle   = _phandle;                                           \
        msg__->eventMask = _eventMask;                                         \
        CsrBtScMsgTransport(msg__);                                            \
    } while(0)

#define CsrBtScSetEventMaskReqSend(_eventMask)                                 \
        CsrBtScSetEventMaskReqSendEx(CSR_SCHED_QID_INVALID,                    \
                                     _eventMask)
#endif /* CSR_BT_INSTALL_SC_SET_EVENT_MASK */

#ifdef CSR_BT_INSTALL_SC_OOB
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScAddRemoteOobDataReq
 *
 *  DESCRIPTION
 *      Function to build and send a CSR_BT_SC_ADD_REMOTE_OOB_DATA_REQ to the SC
 *
 *    PARAMETERS
 *      deviceAddr:         Address of device which should be notified
 *      addressType:        Address type
 *      transportType:      Transport type
 *      oobHashC192:        SSP hash parameter for P192
 *      oobRandR192:        SSP random value for P192
 *      oobHashC256:        SSP hash parameter for P256
 *      oobRandR256:        SSP random value for P256
 *----------------------------------------------------------------------------*/
void CsrBtScAddRemoteOobDataReqSend(CsrBtDeviceAddr deviceAddr,
                                    CsrBtAddressType addressType,
                                    CsrBtTransportType transportType,
                                    CsrBtScOobData *oobHashC192,
                                    CsrBtScOobData *oobRandR192,
                                    CsrBtScOobData *oobHashC256,
                                    CsrBtScOobData *oobRandR256);
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScDebugModeReqSend
 *
 *  DESCRIPTION
 *      Function to build and send a CSR_BT_SC_DEBUG_MODE_REQ message to the SC message
 *      queue.
 *
 *  PARAMETERS
 *      eventMask:            set default eventmask
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_SC_DEBUG_MODE
#define CsrBtScDebugModeReqSend(_phandle, _enable) {                    \
        CsrBtScDebugModeReq *msg__ = (CsrBtScDebugModeReq *) CsrPmemAlloc(sizeof(CsrBtScDebugModeReq)); \
        msg__->type = CSR_BT_SC_DEBUG_MODE_REQ;                         \
        msg__->enable = _enable;                                        \
        msg__->phandle = _phandle;                                      \
        CsrBtScMsgTransport(msg__);}
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScConfigReqSend
 *
 *  DESCRIPTION
 *      Function to build and send a CSR_BT_SC_CONFIG_REQ message to the SC message
 *      queue.
 *
 *  PARAMETERS
 *      configMask: set the SC configuration mask
 *----------------------------------------------------------------------------*/
#ifdef CSR_BT_INSTALL_SC_CONFIG_MASK
#define CsrBtScConfigReqSend(_configMask) {                             \
        CsrBtScConfigReq *msg__ = (CsrBtScConfigReq *) CsrPmemAlloc(sizeof(CsrBtScConfigReq)); \
        msg__->type = CSR_BT_SC_CONFIG_REQ;                             \
        msg__->configMask = _configMask;                                \
        CsrBtScMsgTransport(msg__);}
#endif


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScAcceptBondReqSend
 *
 *  DESCRIPTION
 *      Function to build and send a CSR_BT_SC_ACCEPT_BOND_REQ message to the SC message
 *      queue.
 *
 *  PARAMETERS
 *      time: set number of msecs to stay discoverable; forever if set to 0
 *----------------------------------------------------------------------------*/
#define CsrBtScAcceptBondReqSend(_phandle, _time) {                     \
        CsrBtScAcceptBondReq *msg__ = (CsrBtScAcceptBondReq *)CsrPmemAlloc(sizeof(CsrBtScAcceptBondReq)); \
        msg__->type = CSR_BT_SC_ACCEPT_BOND_REQ;                        \
        msg__->phandle = _phandle;                                      \
        msg__->time = _time;                                            \
        CsrBtScMsgTransport(msg__);}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScCancelAcceptBondReqSend
 *
 *  DESCRIPTION
 *      Function to build and send a CSR_BT_SC_CANCEL_ACCEPT_BOND_REQ message to
 *      the SC message queue.
 *
 *  PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtScCancelAcceptBondReqSend(_phandle) {                      \
        CsrBtScCancelAcceptBondReq *msg__ = (CsrBtScCancelAcceptBondReq *)CsrPmemAlloc(sizeof(CsrBtScCancelAcceptBondReq)); \
        msg__->type = CSR_BT_SC_CANCEL_ACCEPT_BOND_REQ;                 \
        msg__->phandle = _phandle;                                      \
        CsrBtScMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScEncryptionKeySizeReqSend
 *
 *  DESCRIPTION
 *        Read size of encryption key size
 *
 *    PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtScEncryptionKeySizeReqSend(_appHandle,_addr) {             \
        CsrBtScEncryptionKeySizeReq *msg__ = (CsrBtScEncryptionKeySizeReq*)CsrPmemAlloc(sizeof(CsrBtScEncryptionKeySizeReq)); \
        msg__->type = CSR_BT_SC_ENCRYPTION_KEY_SIZE_REQ;                \
        msg__->appHandle = _appHandle;                                  \
        msg__->address = _addr;                                         \
        CsrBtScMsgTransport(msg__);}

#ifdef CSR_BT_LE_ENABLE
#ifdef CSR_BT_INSTALL_SC_LE_KEY_DISTRIBUTION
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScLeKeyDistributionReqSend
 *
 *  DESCRIPTION
 *        Set low energy key distribution
 *
 *    PARAMETERS
 *        keyDistribution: Bitmask using DM_SM_KEY_DIST_ defines
 *----------------------------------------------------------------------------*/
#define CsrBtScLeKeyDistributionReqSend(_keyDistribution) {             \
        CsrBtScLeKeyDistributionReq *msg__ = (CsrBtScLeKeyDistributionReq*)CsrPmemAlloc(sizeof(CsrBtScLeKeyDistributionReq)); \
        msg__->type = CSR_BT_SC_LE_KEY_DISTRIBUTION_REQ;                \
        msg__->keyDistribution = _keyDistribution;                      \
        CsrBtScMsgTransport(msg__);}
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScSetEncryptionKeySizeReqSend
 *
 *  DESCRIPTION
 *        Set min/max encryption key size values for future LE
 *        security negotiations
 *
 *    PARAMETERS
 *
 *----------------------------------------------------------------------------*/
#define CsrBtScSetEncryptionKeySizeReqSend(_minKeySize, _maxKeySize) {  \
        CsrBtScSetEncryptionKeySizeReq *msg__ = CsrPmemAlloc(sizeof(CsrBtScSetEncryptionKeySizeReq)); \
        msg__->type = CSR_BT_SC_SET_ENCRYPTION_KEY_SIZE_REQ;            \
        msg__->minKeySize = _minKeySize;                                \
        msg__->maxKeySize = _maxKeySize;                                \
        CsrBtScMsgTransport(msg__);}
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      During Bluetooth shutdown all allocated payload in the Synergy BT SC
 *      message must be deallocated. This is done by this function
 *
 *
 *    PARAMETERS
 *      eventClass :  Must be CSR_BT_SC_PRIM,
 *      msg:          The message received from Synergy BT SC
 *----------------------------------------------------------------------------*/
void CsrBtScFreeUpstreamMessageContents(CsrUint16 eventClass, void * message);


#ifdef __cplusplus
}
#endif

#endif

