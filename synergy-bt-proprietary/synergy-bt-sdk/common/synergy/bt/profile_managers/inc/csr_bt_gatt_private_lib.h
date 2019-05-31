#ifndef CSR_BT_GATT_PRIVATE_LIB_H__
#define CSR_BT_GATT_PRIVATE_LIB_H__
/******************************************************************************
 Copyright (c) 2011-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

 REVISION:      $Revision: #8 $
******************************************************************************/

#include "csr_synergy.h"

#include "csr_bt_profiles.h"
#include "csr_bt_gatt_private_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------------------------
  Name: 
    CsrBtGattReadRemoteLeNameReqSend

  Description: 
    Request to Read the remote LE name from SC only. GATT will always
    read the remote Name when an LE physical connection has been 
    established.

    As confirmation the application will receive a 
    CSR_BT_GATT_READ_REMOTE_LE_NAME_CFM message. 

    Note in case that the GATT do not have an LE physical 
    connection to the given address, the returned name will
    be set to NULL.
   
  Parameters: 
    CsrSchedQid  _qid       - Protocol handle of the higher layer entity
    CsrBtTypedAddr _address - The device address of the peer Le device 
   -------------------------------------------------------------------- */
CsrBtGattReadRemoteLeNameReq *CsrBtGattReadRemoteLeNameReq_struct(CsrSchedQid    qid,
                                                                  CsrBtTypedAddr address);
#define CsrBtGattReadRemoteLeNameReqSend(_qid,_address) { \
CsrBtGattReadRemoteLeNameReq *msg__; \
msg__=CsrBtGattReadRemoteLeNameReq_struct(_qid,_address); \
CsrMsgTransport(CSR_BT_GATT_IFACEQUEUE,CSR_BT_GATT_PRIM,msg__);}

#ifdef CSR_BT_INSTALL_LE_PRIVACY_1P2_SUPPORT
/* --------------------------------------------------------------------
  Name:
    CsrBtGattReadRemoteCarCharReqSend

  Description:
    Request to Read the remote Central Address resolution value.

    GATT responds to this request with
    CSR_BT_GATT_READ_REMOTE_CAR_CHAR_CFM message.

    Note: In case GATT is not already connected to the given address,
    the returned value will be set to CSR_BT_GATT_CAR_VALUE_INVALID
    and result will be set to CSR_BT_GATT_RESULT_UNKNOWN_CONN_ID.

    In success case the carValue will return the below values,
    0 = address resolution is not supported in this device
    1 = address resolution is supported in this device
    2 - 255 = Reserved for future use

  Parameters:
    CsrBtTypedAddr _address - The device address of the peer LE device
   -------------------------------------------------------------------- */
#define CsrBtGattReadRemoteCarCharReqSend(_address) { \
        CsrBtGattReadRemoteCarCharReq *msg__ = (CsrBtGattReadRemoteCarCharReq *) CsrPmemAlloc(sizeof(*msg__)); \
        msg__->type    = CSR_BT_GATT_READ_REMOTE_CAR_CHAR_REQ; \
        msg__->address = _address; \
        CsrMsgTransport(CSR_BT_GATT_IFACEQUEUE,CSR_BT_GATT_PRIM,msg__);}

/* --------------------------------------------------------------------
  Name:
    CsrBtGattReadRemoteRpaOnlyCharReqSend

  Description:
    Request to Read the Resolvable Private Address only characteristic value.

    GATT responds to this request with
    CSR_BT_GATT_READ_REMOTE_RPA_ONLY_CHAR_CFM message.

    Note: In case GATT is not already connected to the given address,
    the returned value will be set to CSR_BT_GATT_RPA_ONLY_VALUE_INVALID
    and result will be set to CSR_BT_GATT_RESULT_UNKNOWN_CONN_ID.

    In success case the rpaOnlyValue will return the below values,
    0 = only Resolvable Private Addresses will be used as local addresses
    after bonding
    1 - 255 = Reserved for future use

  Parameters:
    CsrBtTypedAddr _address - The device address of the peer LE device
   -------------------------------------------------------------------- */
#define CsrBtGattReadRemoteRpaOnlyCharReqSend(_address) { \
        CsrBtGattReadRemoteRpaOnlyCharReq *msg__ = (CsrBtGattReadRemoteRpaOnlyCharReq *)CsrPmemAlloc(sizeof(*msg__)); \
        msg__->type    = CSR_BT_GATT_READ_REMOTE_RPA_ONLY_CHAR_REQ; \
        msg__->address = _address; \
        CsrMsgTransport(CSR_BT_GATT_IFACEQUEUE,CSR_BT_GATT_PRIM,msg__);}
#endif /* CSR_BT_INSTALL_LE_PRIVACY_1P2_SUPPORT */

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_GATT_PRIVATE_LIB_H__ */

