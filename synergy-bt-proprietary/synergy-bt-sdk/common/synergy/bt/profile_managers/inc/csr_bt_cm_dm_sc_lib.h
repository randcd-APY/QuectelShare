#ifndef CSR_BT_CM_DM_SC_LIB_H__
#define CSR_BT_CM_DM_SC_LIB_H__
/******************************************************************************
 Copyright (c) 2009-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

 REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"


#include "csr_bt_cm_prim.h"
#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void CsrBtCmScDmAddDeviceReq(CsrBtTypedAddr  typedAddr,
                             DM_SM_TRUST_T   trust,
                             DM_SM_KEYS_T    *keys);
void CsrBtCmScDmRemoveDeviceReq(CsrBtDeviceAddr deviceAddr, CsrBtAddressType addressType);

#define CsrBtCmScDmAclOpenReq(_appHandle, _deviceAddr) {                \
        CsrBtCmSmAclOpenReq *prim__ = (CsrBtCmSmAclOpenReq *) CsrPmemAlloc(sizeof(CsrBtCmSmAclOpenReq)); \
        prim__->type = CSR_BT_CM_SM_ACL_OPEN_REQ;                       \
        prim__->appHandle = _appHandle;                                 \
        prim__->deviceAddr = _deviceAddr;                               \
        CsrBtCmPutMessageDownstream(prim__);}

#define CsrBtCmScDmAclCloseReq(_deviceAddr) {                           \
        CsrBtCmSmAclCloseReq *prim__ = (CsrBtCmSmAclCloseReq *) CsrPmemAlloc(sizeof(CsrBtCmSmAclCloseReq)); \
        prim__->type = CSR_BT_CM_SM_ACL_CLOSE_REQ;                      \
        prim__->deviceAddr = _deviceAddr;                               \
        CsrBtCmPutMessageDownstream(prim__);}

#define CsrBtCmScDmCancelConnectReq(_deviceAddr){                       \
        CsrBtCmSmCancelConnectReq *prim__ = (CsrBtCmSmCancelConnectReq *) CsrPmemAlloc(sizeof(CsrBtCmSmCancelConnectReq)); \
        prim__->type = CSR_BT_CM_SM_CANCEL_CONNECT_REQ;                 \
        prim__->deviceAddr = _deviceAddr;                               \
        CsrBtCmPutMessageDownstream(prim__);}

void CsrBtCmScDmDeleteStoredLinkKeyReq(CsrBtDeviceAddr deviceAddr, delete_all_flag_t flag);
void CsrBtCmScDmAuthenticateReq(CsrBtDeviceAddr deviceAddr);

#ifdef CSR_BT_INSTALL_SC_ENCRYPTION
void CsrBtCmScDmEncryptionReq(CsrSchedQid appHandle, CsrBtDeviceAddr deviceAddr, CsrBool encMode);
#endif

#define CsrBtCmScDmSetSecModeReq(_mode, _mode3Enc){                     \
        CsrBtCmSmSetSecModeReq *prim__ = (CsrBtCmSmSetSecModeReq *) CsrPmemAlloc(sizeof(CsrBtCmSmSetSecModeReq)); \
        prim__->type = CSR_BT_CM_SM_SET_SEC_MODE_REQ;                   \
        prim__->mode = _mode;                                           \
        prim__->mode3Enc = _mode3Enc;                                   \
        CsrBtCmPutMessageDownstream(prim__);}

#define CsrBtCmScDmSetDefaultSecLevelReq(_seclDefault){                 \
        CsrBtCmSmSetDefaultSecLevelReq *prim__ = (CsrBtCmSmSetDefaultSecLevelReq *) CsrPmemAlloc(sizeof(CsrBtCmSmSetDefaultSecLevelReq)); \
        prim__->type = CSR_BT_CM_SM_SET_DEFAULT_SEC_LEVEL_REQ;          \
        prim__->seclDefault = seclDefault;                              \
        CsrBtCmPutMessageDownstream(prim__);}

#define CsrBtCmScDmUnRegisterReq(_channel, _protocolId){                \
        CsrBtCmSmUnregisterReq *prim__ = (CsrBtCmSmUnregisterReq *) CsrPmemAlloc(sizeof(CsrBtCmSmUnregisterReq)); \
        prim__->type = CSR_BT_CM_SM_UNREGISTER_REQ;                     \
        prim__->channel = _channel;                                     \
        prim__->protocolId = _protocolId;                               \
        CsrBtCmPutMessageDownstream(prim__);}

void CsrBtCmScDmSspLinkKeyRequestResSend(CsrBtDeviceAddr deviceAddr, CsrUint8 keyType, CsrUint8 *key,
                                         CsrBtAddressType addressType, CsrUint8 keyLength);

#define CsrBtCmScDmLinkKeyRequestNegRes(_deviceAddr) CsrBtCmScDmSspLinkKeyRequestResSend(_deviceAddr, DM_SM_LINK_KEY_NONE, NULL, 0, 0)

void CsrBtCmScDmPinRequestRes(CsrBtDeviceAddr deviceAddr, CsrUint8 pinLength, CsrUint8 *pin);
void CsrBtCmScDmPinRequestNegRes(CsrBtDeviceAddr deviceAddr);

void CsrBtCmScDmAuthoriseRes(CsrBtDeviceAddr deviceAddr,
                             CsrBool incoming,
                             CsrUint16 authorisation,
                             CsrUint16 channel,
                             dm_protocol_id_t protocolId,
                             CsrBtAddressType addressType);

#define CsrBtCmScDmAuthoriseNegRes(deviceAddr, incoming, channel, protocolId, addressType) CsrBtCmScDmAuthoriseRes(deviceAddr, incoming, FALSE, channel, protocolId, addressType)

void CsrBtCmScDmRegisterReq(dm_protocol_id_t protocolId,
                             CsrUint16 channel,
                             CsrBool outgoingOk,
                             dm_security_level_t securityLevel,
                             psm_t psm);

#define CsrBtCmScDmBondingReq(_deviceAddr, _authenticationRequirements) { \
        CsrBtCmSmBondingReq *prim__ = pnew(CsrBtCmSmBondingReq);        \
        prim__->type                = CSR_BT_CM_SM_BONDING_REQ;         \
        prim__->deviceAddr          = _deviceAddr;                      \
        CsrBtCmPutMessageDownstream(prim__);}

#define CsrBtCmScDmBondingCancelReq(_deviceAddr, _force, _addressType){    \
        CsrBtCmSmBondingCancelReq *prim__ = pnew(CsrBtCmSmBondingCancelReq); \
        prim__->type         = CSR_BT_CM_SM_BONDING_CANCEL_REQ;         \
        prim__->deviceAddr   = _deviceAddr;                             \
        prim__->force        = _force;                                  \
        prim__->addressType  = _addressType;                            \
        CsrBtCmPutMessageDownstream(prim__);}

#ifdef __cplusplus
}
#endif

#endif
