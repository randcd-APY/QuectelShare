#ifndef CSR_BT_CM_DM_SC_SSP_LIB_H__
#define CSR_BT_CM_DM_SC_SSP_LIB_H__

/******************************************************************************
Copyright (c) 2007-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "bluetooth.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CSR_BT_INSTALL_CM_SC_MODE_CONFIG
void CsrBtCmScDmSecModeConfigReq(CsrUint16 writeAuthEnable, CsrUint16 config);
#endif
void CsrBtCmScDmSspLinkKeyRequestRes(CsrBtDeviceAddr deviceAddr, CsrUint8 keyType, CsrUint8 *key,
                                     CsrBtAddressType addressType);
void CsrBtCmScDmSspLinkKeyRequestNegRes(CsrBtDeviceAddr deviceAddr,
                                        CsrBtAddressType addressTypeM);

#if CSR_BT_BT_VERSION >= CSR_BT_BLUETOOTH_VERSION_2P1
void CsrBtCmScDmIoCapabilityRequestRes(CsrBtDeviceAddr deviceAddr,
                                       CsrBtAddressType addressType,
                                       CsrBtTransportType transportType,
                                       CsrUint8 ioCapability,
                                       CsrUint8 authenticationRequirements,
                                       CsrUint8 oobDataPresent,
                                       CsrUint8 *oobHashC,
                                       CsrUint8 *oobRandR,
                                       CsrUint16 keyDistribution);
void CsrBtCmScDmIoCapabilityRequestNegRes(CsrBtDeviceAddr deviceAddr,
                                          CsrBtAddressType addressType,
                                          CsrBtTransportType transportType,
                                          hci_error_t reason);

#define CsrBtCmScDmUserConfirmationRequestRes(_deviceAddr, _addressType, _transportType){ \
        CsrBtCmSmUserConfirmationRequestRes *prim__ = pnew(CsrBtCmSmUserConfirmationRequestRes); \
        prim__->type       = CSR_BT_CM_SM_USER_CONFIRMATION_REQUEST_RES; \
        prim__->deviceAddr = _deviceAddr;                               \
        prim__->reason     = HCI_SUCCESS; /* user accepted */           \
        prim__->addressType = _addressType;                             \
        prim__->transportType = _transportType;                         \
        CsrBtCmPutMessageDownstream(prim__);}

void CsrBtCmScDmUserConfirmationRequestNegRes(CsrBtDeviceAddr deviceAddr,
                                              CsrBtAddressType addressType,
                                              CsrBtTransportType transportType);
#define CsrBtCmScDmUserPasskeyRequestRes(_deviceAddr, _addressType, _transportType, _numericValue){ \
        CsrBtCmSmUserPasskeyRequestRes *prim__ = pnew(CsrBtCmSmUserPasskeyRequestRes); \
        prim__->type         = CSR_BT_CM_SM_USER_PASSKEY_REQUEST_RES;   \
        prim__->deviceAddr   = _deviceAddr;                             \
        prim__->addressType = _addressType;                             \
        prim__->transportType = _transportType;                         \
        prim__->numericValue = _numericValue;                           \
        CsrBtCmPutMessageDownstream(prim__);}

void CsrBtCmScDmUserPasskeyRequestNegRes(CsrBtDeviceAddr deviceAddr,
                                         CsrBtAddressType addressType,
                                         CsrBtTransportType transportType);

#define CsrBtCmScDmReadLocalOobDataReq(_transportType)                              \
    do {                                                                            \
        CsrBtCmSmReadLocalOobDataReq *prim__ = pnew(CsrBtCmSmReadLocalOobDataReq);  \
        prim__->type             = CSR_BT_CM_SM_READ_LOCAL_OOB_DATA_REQ;            \
        prim__->transportType = _transportType;                                     \
        CsrBtCmPutMessageDownstream(prim__);                                        \
    } while (0)


#define CsrBtCmScDmSendKeypressNotificationReq(_deviceAddr, _addressType, _transportType, _notificationType){ \
        CsrBtCmSmSendKeypressNotificationReq *prim__ = pnew(CsrBtCmSmSendKeypressNotificationReq); \
        prim__->type = CSR_BT_CM_SM_SEND_KEYPRESS_NOTIFICATION_REQ; \
        prim__->deviceAddr = _deviceAddr;                           \
        prim__->addressType = _addressType;                         \
        prim__->transportType = _transportType;                     \
        prim__->notificationType = _notificationType;               \
        CsrBtCmPutMessageDownstream(prim__);}

void CsrBtCmSmSendRepairRes(CsrBtDeviceAddr deviceAddr,
                            CsrBool accept,
                            CsrUint16 repairId,
                            CsrBtAddressType addressType);
#else
#define CsrBtCmScDmIoCapabilityRequestRes(deviceAddr, addressType, transportType, ioCapability, authenticationRequirements, oobDataPresent,oobHashC, oobRandR)
#define CsrBtCmScDmIoCapabilityRequestNegRes(deviceAddr, addressType, transportType, reason)
#define CsrBtCmScDmUserConfirmationRequestRes(deviceAddr, addressType, transportType)
#define CsrBtCmScDmUserConfirmationRequestNegRes(deviceAddr, addressType, transportType)
#define CsrBtCmScDmUserPasskeyRequestRes(deviceAddr,  addressType, transportType, numericValue)
#define CsrBtCmScDmUserPasskeyRequestNegRes(deviceAddr, addressType, transportType)
#define CsrBtCmScDmReadLocalOobDataReq()
#define CsrBtCmScDmSendKeypressNotificationReq(deviceAddr, addressType, TransportType, notificationType)
#define CsrBtCmSmSendRepairRes(deviceAddr, accept, repairId, addressType);
#endif

#ifdef __cplusplus
}
#endif

#endif
