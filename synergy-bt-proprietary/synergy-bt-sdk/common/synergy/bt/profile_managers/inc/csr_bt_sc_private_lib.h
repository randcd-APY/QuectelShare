#ifndef CSR_BT_SC_PRIVATE_LIB_H__
#define CSR_BT_SC_PRIVATE_LIB_H__
/******************************************************************************
 Copyright (c) 2012-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

 REVISION:      $Revision: #5 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_bt_result.h"
#include "csr_bt_profiles.h"
#include "csr_bt_sc_db.h"
#include "dm_prim.h"
#include "csr_bt_sc_prim.h"
#include "csr_bt_sc_private_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message function to reduce code size */
void CsrBtScMessagePutDownstream(void* msg);
void CsrBtScMsgTransport(void* msg);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScSetSecInLevel
 *
 *  DESCRIPTION
 *
 *  PARAMETERS
 *      secInLevel:         the mapped security settings
 *      secLevel:           the security level
 *      secManLevel:        the mandatory security level
 *      secDefLevel:        the default security level
 *      successCode:        the value to return if operation succeeds
 *      errorCode:          the value to return if operation fails
 *----------------------------------------------------------------------------*/
CsrBtResultCode CsrBtScSetSecInLevel(CsrUint16 *secOutLevel,
                                     CsrUint16 secLevel,
                                     CsrUint16 secManLevel,
                                     CsrUint16 secDefLevel,
                                     CsrBtResultCode successCode,
                                     CsrBtResultCode errorCode);
void CsrBtScMapSecInLevel(CsrUint16 secInput, CsrUint16 *secOutput);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScSetSecOutLevel
 *
 *  DESCRIPTION
 *
 *  PARAMETERS
 *      secOutLevel:        the mapped security settings
 *      secLevel:           the security level
 *      secManLevel:        the mandatory security level
 *      secDefLevel:        the default security level
 *      successCode:        the value to return if operation succeeds
 *      errorCode:          the value to return if operation fails
 *----------------------------------------------------------------------------*/
CsrBtResultCode CsrBtScSetSecOutLevel(CsrUint16 *secOutLevel,
                                      CsrUint16 secLevel,
                                      CsrUint16 secManLevel,
                                      CsrUint16 secDefLevel,
                                      CsrBtResultCode successCode,
                                      CsrBtResultCode errorCode);
void CsrBtScMapSecOutLevel(CsrUint16 secInput, CsrUint16 *secOutput);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScRegisterReqSend
 *
 *  DESCRIPTION
 *      Function to build and send a CSR_BT_SC_REGISTER_REQ message to the SC message
 *      queue.
 *
 *  PARAMETERS
 *      profileUuid:        Service uuid16_t for server service
 *      protocolId:         Protocol at which security is to be applied
 *      secLevel:           Level of security to be applied
 *      channel:            Channel for that protocol
 *      outgoingOk:         TRUE if this also applies to outgoing connections
 *----------------------------------------------------------------------------*/
#define CsrBtScRegisterReqSend(_theProfileUuid, _theChannel, _outgoingOk, _theProtocolId, _theSecLevel) { \
        CsrBtScRegisterReq *msg__ = (CsrBtScRegisterReq *) CsrPmemAlloc(sizeof(CsrBtScRegisterReq)); \
        msg__->type = CSR_BT_SC_REGISTER_REQ;                           \
        msg__->profileUuid = _theProfileUuid;                           \
        msg__->protocolId = _theProtocolId;                             \
        msg__->secLevel = _theSecLevel;                                 \
        msg__->channel = _theChannel;                                   \
        msg__->outgoingOk = _outgoingOk;                                \
        CsrBtScMessagePutDownstream(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScDeregisterReqSend
 *
 *  DESCRIPTION
 *      Function to build and send a CSR_BT_SC_DEREGISTER_REQ message to the SC message
 *      queue.
 *
 *  PARAMETERS
 *      protocolId:         Protocol at which security is to be applied
 *      channel:            Channel for that protocol
 *----------------------------------------------------------------------------*/
void CsrBtScDeregisterReqSend(dm_protocol_id_t protocolId, CsrUint16 channel);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScDatabaseReqSend
 *
 *  DESCRIPTION
 *      Function to build and send a CSR_BT_SC_DATABASE_REQ message to the SC message
 *      queue.
 *
 *  PARAMETERS
 *      appHandle   where the CFM shall be sent to
 *      deviceAddr  what BT device in the database to access
 *      opcode      DB operation code
 *      entry       what DB entry (link key index) to access
 *      keyType     key type of data (only used during write operations)
 *      length      length of data
 *      data        pointer to data
 *----------------------------------------------------------------------------*/
void CsrBtScDatabaseReqSend(CsrSchedQid appHandle, CsrBtDeviceAddr *deviceAddr, CsrUint8 opcode, CsrUint8 entry, CsrUint8 keyType, CsrUint8 length, CsrUint8 *data);

#ifdef CSR_BT_LE_ENABLE

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScLeSecurityReqSend
 *
 *  DESCRIPTION
 *      Send a LE security request to DM_SM/SM.
 *
 *----------------------------------------------------------------------------*/
#define CsrBtScLeSecurityReqSend(_appHandle, _securityRequirements, _address, _l2caConFlags) { \
        CsrBtScLeSecurityReq *msg__ = (CsrBtScLeSecurityReq*)CsrPmemAlloc(sizeof(CsrBtScLeSecurityReq)); \
        msg__->type = CSR_BT_SC_LE_SECURITY_REQ;                        \
        msg__->phandle = _appHandle;                                    \
        msg__->securityRequirements = _securityRequirements;            \
        CsrBtAddrCopy(&msg__->address, &_address);                      \
        msg__->l2caConFlags = _l2caConFlags;                            \
        CsrBtScMessagePutDownstream(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScLeAddServiceChangedReqSend
 *
 *  DESCRIPTION
 *      Add the Attribute Handle of the Peer device Service Changed 
 *      Characteristic Value anbd the Handle of the Client  

 *    PARAMETERS
 *      deviceAddr:           what BT device in the database to access
 *      serviceChangedHandle: Consist of two handles the Peer devices 
 *                            Characteristic Value and the Client Config. 
 *                            Upper Client lower Value.
 *      clientConfigValue:    Indicates if ServiceChanged Messages shall
 *                            be sent to remote device or not 
 *                            (0 == NO, 2 == YES)  
 *----------------------------------------------------------------------------*/
#define CsrBtScLeAddServiceChangedReqSend(_addr,_addressType,_serviceChangedHandle,_clientConfigValue) { \
        CsrBtScLeAddServiceChangedReq *msg__ = (CsrBtScLeAddServiceChangedReq*)CsrPmemAlloc(sizeof(CsrBtScLeAddServiceChangedReq)); \
        msg__->type = CSR_BT_SC_LE_ADD_SERVICE_CHANGED_REQ;             \
        msg__->serviceChangedHandle = _serviceChangedHandle;            \
        msg__->clientConfigValue = _clientConfigValue;                  \
        msg__->addr = _addr;                                            \
        msg__->addressType = _addressType;                              \
        CsrBtScMessagePutDownstream(msg__);}

#ifdef CSR_BT_INSTALL_LE_PRIVACY_1P2_SUPPORT
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScLeReadCarCharValueReqSend
 *
 *  DESCRIPTION
 *      Read cached CAR characteristic value of remote device.
 *
 *  PARAMETERS
 *      phandle: Handle to the queue where SC must return indications and confirm
 *      address: Peer device address whose CAR characteristic's value to be read.
 *----------------------------------------------------------------------------*/
#define CsrBtScLeReadCarCharValueReqSend(_phandle,                          \
                                         _address)                          \
    do                                                                      \
    {                                                                       \
        CsrBtScLeReadCarCharValueReq *msg__ = CsrPmemAlloc(sizeof(*msg__)); \
        msg__->type        = CSR_BT_SC_LE_READ_CAR_CHAR_VALUE_REQ;          \
        msg__->phandle     = _phandle;                                      \
        msg__->address     = _address;                                      \
        CsrBtScMessagePutDownstream(msg__);                                 \
    } while(0)
#endif /* CSR_BT_INSTALL_LE_PRIVACY_1P2_SUPPORT */

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtScReadDeviceRecordWithLeInfoReqSend
 *
 *  DESCRIPTION
 *      Function to read the entire SC DB including LE information
 *
 *    PARAMETERS
 *        phandle:        handle to the queue where SC must return indications and confirm
 *        maxNumOfBytes:    maximum allowed bytes in a result indication
 * *----------------------------------------------------------------------------*/
#define CsrBtScReadDeviceRecordWithLeInfoReqSend(_phandle, _maxNumOfBytes) { \
        CsrBtScReadDeviceRecordReq *msg__ = (CsrBtScReadDeviceRecordReq *) CsrPmemAlloc(sizeof(CsrBtScReadDeviceRecordReq)); \
        msg__->type = CSR_BT_SC_READ_DEVICE_RECORD_REQ;                 \
        msg__->maxNumOfBytes = _maxNumOfBytes;                          \
        msg__->phandle = _phandle;                                      \
        msg__->flags   = CSR_BT_SC_READ_RECORD_FLAG_ADD_LE;             \
        CsrBtScMsgTransport(msg__);}

#define CsrBtScDeleteKeysReqSend(_phandle, _deviceAddr, _addressType, _keyType)     \
    do                                                                              \
    {                                                                               \
        CsrBtScDebondReq *msg__ = (CsrBtScDebondReq *) CsrPmemAlloc(sizeof(*msg__));\
        msg__->type = CSR_BT_SC_DEBOND_REQ;                                         \
        msg__->phandle = _phandle;                                                  \
        msg__->deviceAddr = _deviceAddr;                                            \
        msg__->addressType = _addressType;                                          \
        msg__->keyType = _keyType;                                                  \
        CsrBtScMsgTransport(msg__);                                                 \
    } while (0)

#endif /* CSR_BT_LE_ENABLE */

#ifdef __cplusplus
}
#endif

#endif

