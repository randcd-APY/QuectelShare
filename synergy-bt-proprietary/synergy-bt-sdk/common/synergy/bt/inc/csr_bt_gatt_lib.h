#ifndef CSR_BT_GATT_LIB_H__
#define CSR_BT_GATT_LIB_H__
/******************************************************************************
 Copyright (c) 2010-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

 REVISION:      $Revision: #4 $
******************************************************************************/


#include "csr_synergy.h"
#include "csr_msg_transport.h"
#include "csr_bt_gatt_prim.h"
#include "csr_bt_gatt_utils.h"
#include "csr_bt_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Common put_message functions to reduce code size */
void CsrBtGattMsgTransport(void *msg);

/* --------------------------------------------------------------------
   Name:
   CsrBtGattRegisterReqSend

   Description:
   Request to Register an application instance to Gatt.

   As confirmation the application will receive a
   CSR_BT_GATT_REGISTER_CFM message.
   
  Parameters: 
    CsrSchedQid             _qid      - Protocol handle of the higher layer entity 
                                        registering with GATT.
    CsrUint16               _context  - Registration context.
   -------------------------------------------------------------------- */
CsrBtGattRegisterReq *CsrBtGattRegisterReq_struct(CsrSchedQid qid, CsrUint16 context);
#define CsrBtGattRegisterReqSend(_qid,_context) {                       \
        CsrBtGattRegisterReq *msg__;                                    \
        msg__=CsrBtGattRegisterReq_struct(_qid,_context);               \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattUnregisterReqSend

   Description:
   Request to Unregister/Remove an application instance from Gatt.

   As confirmation the application will receive a
   CSR_BT_GATT_UNREGISTER_CFM message.

   Note that if the application has:
   1 A Connection or has created one, GATT will ensure that it is released.
   2 Added a local data base, GATT will ensure that it is removed.
   3 Reserved/allocated a range of attribute handles, GATT will ensure
   that these are deallocated.

   Also note GATT does NOT ensure that the following is done:

   1 If the application has requested a peer server to enable broadcast,
   by using CsrBtGattWriteServerConfigurationReqSend, GATT does
   NOT ensure that broadcast is disabled.

   2 If the application has requested a peer server to send notification
   or indication, by using CsrBtGattWriteClientConfigurationReqSend,
   and the local and remote device are still paired, GATT does
   NOT ensure that the server stops sending these events.

   E.g. it is up to the application to make sure this is done
   before CsrBtGattUnregisterReqSend is called.

   Parameters:
   CsrBtGattId _gattId - Application identifier
   -------------------------------------------------------------------- */
CsrBtGattUnregisterReq *CsrBtGattUnregisterReq_struct(CsrBtGattId gattId);

#define CsrBtGattUnregisterReqSend(_gattId) {                           \
        CsrBtGattUnregisterReq *msg__;                                  \
        msg__=CsrBtGattUnregisterReq_struct(_gattId);                   \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattCentralReqSend

   Description:
   Request to initiate the establishment of a LE physical
   connection as a Central.

   As confirmation the application will receive a
   CSR_BT_GATT_CENTRAL_CFM message. If the
   CSR_BT_GATT_CENTRAL_CFM message returns success
   the application will later receive the
   CSR_BT_GATT_CONNECT_IND message.

   The application is permitted to cancel this procedure
   after it has received a successful
   CSR_BT_GATT_CENTRAL_CFM message, by calling
   CsrBtGattDisconnectReqSend.

   Note that a device operating as Central will be
   in the Master Role. The Central role supports
   multiple connections and is the initiator for
   all connections with devices in the peripheral role.

   GATT does not support simultaneous Central and
   Peripheral procedures.
   Application has to subscribe for 
   CSR_BT_GATT_EVENT_MASK_SUBSCRIBE_PHYSICAL_LINK_STATUS
   using CsrBtGattSetEventMaskReqSend.
   Whenever a connection request fails with	result code
   CSR_BT_GATT_RESULT_BUSY, application has to wait for
   CSR_BT_GATT_PHYSICAL_LINK_STATUS_IND to request for
   connection again.

  Parameters: 
    CsrBtGattId _gattId       - Application identifier
    CsrBtTypedAddr _address   - The device address of the peer server 
    CsrBtGattConnFlags _flags - The Connection flags, which are defined 
                                in csr_bt_gatt_prim.h. These are valid
                                CSR_BT_GATT_FLAGS_NONE
                                CSR_BT_GATT_FLAGS_WHITELIST
                                CSR_BT_GATT_FLAGS_NO_AUTO_SECURITY
                                CSR_BT_GATT_FLAGS_CENTRAL_TIMEOUT
    CsrUint16 _preferredMtu   - The preferable packet size.
                                0 = no preference. Minimum/default is 
                                23 bytes. 
                                If set to larger than 23 bytes
                                and the mtu size in the 
                                CSR_BT_GATT_CONNECT_IND message is 23 bytes, 
                                the application may receive a 
                                CSR_BT_GATT_MTU_CHANGED_IND message with 
                                the new negotiated packet size shortly 
                                after it has received a successful 
                                CSR_BT_GATT_CONNECT_IND message.
   -------------------------------------------------------------------- */
CsrBtGattCentralReq *CsrBtGattCentralReq_struct(CsrBtGattId        gattId,
                                                CsrBtTypedAddr     address,
                                                CsrBtGattConnFlags flags,
                                                CsrUint16          preferredMtu);

#define CsrBtGattCentralReqSend(_gattId,_address,_flags,_preferredMtu) { \
        CsrBtGattCentralReq *msg__;                                     \
        msg__=CsrBtGattCentralReq_struct(_gattId,_address,_flags,_preferredMtu); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Common structure for the CsrBtGattPeripheralReq primitive which
   is used by the functions:
   CsrBtGattPeripheralReqSend
   CsrBtGattPeripheralReqDataSend
   -------------------------------------------------------------------- */
CsrBtGattPeripheralReq *CsrBtGattPeripheralReq_struct(CsrBtGattId        gattId,
                                                      CsrBtTypedAddr     address,
                                                      CsrBtGattConnFlags flags,
                                                      CsrUint16          preferredMtu,
                                                      CsrUint8           advLength,
                                                      CsrUint8           *advData,
                                                      CsrUint8           scanrspLength,
                                                      CsrUint8           *scanrsp);

/* --------------------------------------------------------------------
   Name:
   CsrBtGattPeripheralReqSend

   Description:
   Request to accept the establishment of a LE physical
   connection as a Peripheral.

   As confirmation the application will receive a
   CSR_BT_GATT_PERIPHERAL_CFM message. If the
   CSR_BT_GATT_PERIPHERAL_CFM message returns success
   the application will later receive the
   CSR_BT_GATT_CONNECT_IND message.

   The application is permitted to cancel this procedure
   after it has received a successful
   CSR_BT_GATT_PERIPHERAL_CFM message, by calling
   CsrBtGattDisconnectReqSend.

   Note that a device operating as Peripheral will be
   in the Slave Role.

   GATT does not support simultaneous Central and
   Peripheral procedures.
   Application has to subscribe for 
   CSR_BT_GATT_EVENT_MASK_SUBSCRIBE_PHYSICAL_LINK_STATUS
   using CsrBtGattSetEventMaskReqSend.
   Whenever a connection request fails with	result code
   CSR_BT_GATT_RESULT_BUSY, application has to wait for
   CSR_BT_GATT_PHYSICAL_LINK_STATUS_IND to request for
   connection again.
   
  Parameters: 
    CsrBtGattId _gattId       - Application identifier
    CsrBtTypedAddr _address   - The device address of the peer server 
    CsrBtGattConnFlags _flags - The Connection flags, which are defined 
                                in csr_bt_gatt_prim.h. These are valid:
                                CSR_BT_GATT_FLAGS_NONE
                                CSR_BT_GATT_FLAGS_WHITELIST
                                CSR_BT_GATT_FLAGS_UNDIRECTED
                                CSR_BT_GATT_FLAGS_UNDIRECTED_NEW
                                CSR_BT_GATT_FLAGS_ADVERTISE_TIMEOUT
                                CSR_BT_GATT_FLAGS_NONDISCOVERABLE
                                CSR_BT_GATT_FLAGS_LIMITED_DISCOVERABLE
                                CSR_BT_GATT_FLAGS_NO_AUTO_SECURITY
                                CSR_BT_GATT_FLAGS_WHITELIST_SCANRSP
    CsrUint16 _preferredMtu   - The preferable packet size.
                                0 = no preference. Minimum/default is 
                                23 bytes. 
                                If set to larger than 23 bytes
                                and the mtu size in the 
                                CSR_BT_GATT_CONNECT_IND message is 23 bytes, 
                                the application may receive a 
                                CSR_BT_GATT_MTU_CHANGED_IND message with 
                                the new negotiated packet size shortly 
                                after it has received a successful 
                                CSR_BT_GATT_CONNECT_IND message.
   -------------------------------------------------------------------- */
#define CsrBtGattPeripheralReqSend(_gattId,_address,_flags,_preferredMtu) { \
        CsrBtGattPeripheralReq *msg__;                                  \
        msg__=CsrBtGattPeripheralReq_struct(_gattId,_address,_flags,_preferredMtu,0,NULL,0,NULL); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattPeripheralReqDataSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3

   Description:
   Request to accept the establishment of a LE physical
   connection as a Peripheral, where the given Advertising
   and Scan Response data is sent in the advertising events.

   As confirmation the application will receive a
   CSR_BT_GATT_PERIPHERAL_CFM message. If the
   CSR_BT_GATT_PERIPHERAL_CFM message returns success
   the application will later receive the
   CSR_BT_GATT_CONNECT_IND message.

   The application is permitted to cancel this procedure
   after it has received a successful
   CSR_BT_GATT_PERIPHERAL_CFM message, by calling
   CsrBtGattDisconnectReqSend.

   Note that a device operating as Peripheral will be
   in the Slave Role. 

   GATT does not support simultaneous Central and
   Peripheral procedures.
   Application has to subscribe for 
   CSR_BT_GATT_EVENT_MASK_SUBSCRIBE_PHYSICAL_LINK_STATUS
   using CsrBtGattSetEventMaskReqSend.
   Whenever a connection request fails with	result code
   CSR_BT_GATT_RESULT_BUSY, application has to wait for
   CSR_BT_GATT_PHYSICAL_LINK_STATUS_IND to request for
   connection again.
   
  Parameters: 
    CsrBtGattId _gattId         - Application identifier
    CsrBtTypedAddr _address     - The device address of the peer server 
    CsrBtGattConnFlags _flags   - The Connection flags, which are defined 
                                  in csr_bt_gatt_prim.h. These are valid:
                                  CSR_BT_GATT_FLAGS_NONE
                                  CSR_BT_GATT_FLAGS_WHITELIST
                                  CSR_BT_GATT_FLAGS_UNDIRECTED
                                  CSR_BT_GATT_FLAGS_UNDIRECTED_NEW
                                  CSR_BT_GATT_FLAGS_ADVERTISE_TIMEOUT
                                  CSR_BT_GATT_FLAGS_NONDISCOVERABLE
                                  CSR_BT_GATT_FLAGS_LIMITED_DISCOVERABLE
                                  CSR_BT_GATT_FLAGS_APPEND_DATA
                                  CSR_BT_GATT_FLAGS_NO_AUTO_SECURITY
                                  CSR_BT_GATT_FLAGS_WHITELIST_SCANRSP
    CsrUint16 _preferredMtu     - The preferable packet size.
                                  0 = no preference. Minimum/default is 
                                  23 bytes. 
                                  If set to larger than 23 bytes
                                  and the mtu size in the 
                                  CSR_BT_GATT_CONNECT_IND message is 23 bytes, 
                                  the application may receive a 
                                  CSR_BT_GATT_MTU_CHANGED_IND message with 
                                  the new negotiated packet size shortly 
                                  after it has received a successful 
                                  CSR_BT_GATT_CONNECT_IND message.
    CsrUint8 _advDataLength     - Length of the advertising Data in octets.
    CsrUint8 *_advData          - The advertising data formatted as defined 
                                  in [Vol 3] Part C, Section 11 -
                                  ADVERTISING AND SCAN RESPONSE DATA FORMAT
    CsrUint8 _scanRspDataLength - Length of the Scan Response Data in octets.
    CsrUint8 *_scanRspData      - The Scan Response Data formatted as 
                                  defined in [Vol 3] Part C, Section 11 -
                                  ADVERTISING AND SCAN RESPONSE DATA FORMAT
   -------------------------------------------------------------------- */
#define CsrBtGattPeripheralReqDataSend(_gattId,_address,_flags,_preferredMtu, \
                                       _advDataLength, _advData,        \
                                       _scanRspDataLength,_scanRspData) { \
        CsrBtGattPeripheralReq *msg__;                                  \
        msg__=CsrBtGattPeripheralReq_struct(_gattId,_address,_flags,_preferredMtu, \
                                            _advDataLength,_advData,    \
                                            _scanRspDataLength,_scanRspData); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattBredrConnectReqSend

   Description:
   Request to initiate the establishment of a BR/EDR connection

   As confirmation the application will receive a
   CSR_BT_GATT_BREDR_CONNECT_CFM message. If the
   CSR_BT_GATT_BREDR_CONNECT_CFM message returns success
   the application will later receive the
   CSR_BT_GATT_CONNECT_IND message.

   The application is permitted to cancel this procedure
   after it has received a successful
   CSR_BT_GATT_BREDR_CONNECT_CFM message, by calling
   CsrBtGattDisconnectReqSend.

  Parameters: 
    CsrBtGattId _gattId       - Application identifier
    CsrBtTypedAddr _address   - The device address of the peer server 
    CsrBtGattConnFlags _flags - The Connection flags, which are defined 
                                in csr_bt_gatt_prim.h. These are valid:
                                CSR_BT_GATT_FLAGS_NONE
                                CSR_BT_GATT_FLAGS_NO_AUTO_SECURITY
   -------------------------------------------------------------------- */
CsrBtGattBredrConnectReq *CsrBtGattBredrConnectReq_struct(CsrBtGattId        gattId,
                                                          CsrBtTypedAddr     address,
                                                          CsrBtGattConnFlags flags);

#define CsrBtGattBredrConnectReqSend(_gattId,_address,_flags) {         \
        CsrBtGattBredrConnectReq *msg__;                                \
        msg__=CsrBtGattBredrConnectReq_struct(_gattId,_address,_flags); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattBredrAcceptReqSend

   Description:
   Request to accept the establishment of a BR/EDR connection.

   As confirmation the application will receive a
   CSR_BT_GATT_BREDR_ACCEPT_CFM message. If the
   CSR_BT_GATT_BREDR_ACCEPT_CFM message returns success
   the application will later receive the
   CSR_BT_GATT_CONNECT_IND message.

   The application is permitted to cancel this procedure
   after it has received a successful
   CSR_BT_GATT_BREDR_ACCEPT_CFM message, by calling
   CsrBtGattDisconnectReqSend.
   
  Parameters: 
    CsrBtGattId _gattId       - Application identifier
    CsrBtGattConnFlags _flags - The Connection flags, which are defined 
                                in csr_bt_gatt_prim.h. These are valid:
                                CSR_BT_GATT_FLAGS_NONE
                                CSR_BT_GATT_FLAGS_NO_AUTO_SECURITY
   -------------------------------------------------------------------- */
CsrBtGattBredrAcceptReq *CsrBtGattBredrAcceptReq_struct(CsrBtGattId        gattId,
                                                        CsrBtGattConnFlags flags);
#define CsrBtGattBredrAcceptReqSend(_gattId,_flags) {                   \
        CsrBtGattBredrAcceptReq *msg__;                                 \
        msg__=CsrBtGattBredrAcceptReq_struct(_gattId,_flags);           \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Common structure for the CsrBtGattScanReq primitive which
   is used by the functions:
   CsrBtGattScanReqStartSend
   CsrBtGattScanReqStartFilterSend
   CsrBtGattScanReqStopSend
   -------------------------------------------------------------------- */
CsrBtGattScanReq *CsrBtGattScanReq_struct(CsrBtGattId gattId,
                                          CsrBool enable,
                                          CsrBtGattScanFlags scanFlags,
                                          CsrUint8 filterAddrCount,
                                          CsrBtTypedAddr *filterAddr,
                                          CsrUint8 filterDataCount,
                                          CsrBtGattDataFilter *filterData);

/* --------------------------------------------------------------------
   Name:
   CsrBtGattScanReqStartSend

   Description:
   Request to start scanning with default parameters.

   As confirmation the application will receive a
   CSR_BT_GATT_SCAN_CFM message. If the CSR_BT_GATT_SCAN_CFM message
   returns success and a advertising devices is discovered a
   CSR_BT_GATT_REPORT_IND message is sent to the application.

   Note the application can extract EIR type information from a
   CsrBtGattReportInd message by using the function
   CsrBtGattUtilGetEirInfo which is defined in csr_bt_gatt_utils.h

   Parameters:
   CsrBtGattId _gattId       - Application identifier
   CsrBtGattScanFlags _flags - The scan flags, which are defined
   in csr_bt_gatt_prim.h.
   -------------------------------------------------------------------- */
#define CsrBtGattScanReqStartSend(_gattId,_flags) {                     \
        CsrBtGattScanReq *msg__;                                        \
        msg__=CsrBtGattScanReq_struct(_gattId,TRUE,_flags, 0,NULL,0,NULL); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattScanReqStartFilterSend

   Description:
   Request to start scanning with the given parameters.

   As confirmation the application will receive a CSR_BT_GATT_SCAN_CFM
   message. If the CSR_BT_GATT_SCAN_CFM message returns success and a
   advertising devices is discovered a CSR_BT_GATT_REPORT_IND
   messages is sent to the application.

   Note the application can extract EIR type information from a
   CsrBtGattReportInd message by using the function
   CsrBtGattUtilGetEirInfo which is defined in csr_bt_gatt_utils.h

   Parameters:
   CsrBtGattId _gattId              - Application identifier
   CsrBtGattScanFlags _flags        - The scan flags, which are defined
   in csr_bt_gatt_prim.h.
   CsrUint8 _filterAddrCount        - Number of address filters
   CsrBtTypedAddr *_filterAddr      - Address filter. Empty means all
   CsrUint8 _filterDataCount        - Number of data filters
   CsrBtGattDataFilter *_filterData - Data filters
   -------------------------------------------------------------------- */
#define CsrBtGattScanReqStartFilterSend(_gattId,_flags,_filterAddrCount, \
                                        _filterAddr,_filterDataCount,_filterData) { \
        CsrBtGattScanReq *msg__;                                        \
        msg__=CsrBtGattScanReq_struct(_gattId,TRUE,_flags,_filterAddrCount, \
                                      _filterAddr,_filterDataCount,_filterData); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattScanReqStopSend

   Description:
   Request to stop scanning.

   As confirmation the application will receive a CSR_BT_GATT_SCAN_CFM
   message. If the CSR_BT_GATT_SCAN_CFM message returns success the
   application will stop receiving CSR_BT_GATT_REPORT_IND messages.

   Note the application can extract EIR type information from a
   CsrBtGattReportInd message by using the function
   CsrBtGattUtilGetEirInfo which is defined in csr_bt_gatt_utils.h

   Parameters:
   CsrBtGattId _gattId - Application identifier
   -------------------------------------------------------------------- */
#define CsrBtGattScanReqStopSend(_gattId) {                             \
        CsrBtGattScanReq *msg__;                                        \
        msg__=CsrBtGattScanReq_struct(_gattId,FALSE,CSR_BT_GATT_SCAN_STANDARD,0,NULL,0,NULL); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Common structure for the CsrBtGattAdvertiseReq primitive which
   is used by the functions:
   CsrBtGattAdvertiseReqStartSend
   CsrBtGattAdvertiseReqStartDataSend
   CsrBtGattAdvertiseReqStopSend
   -------------------------------------------------------------------- */
CsrBtGattAdvertiseReq *CsrBtGattAdvertiseReq_struct(CsrBtGattId        gattId,
                                                    CsrBool            enable,
                                                    CsrBtGattConnFlags advertisingFlags,
                                                    CsrUint8           advLength,
                                                    CsrUint8           *advData,
                                                    CsrUint8           scanrspLength,
                                                    CsrUint8           *scanrsp);

/* --------------------------------------------------------------------
   Name:
   CsrBtGattAdvertiseReqStartSend

   Description:
   Request to start transmitting advertising packets.

   As confirmation the application will receive a
   CSR_BT_GATT_ADVERTISE_CFM message.

   Parameters:
   CsrBtGattId _gattId - Application identifier
   -------------------------------------------------------------------- */
#define CsrBtGattAdvertiseReqStartSend(_gattId) {                       \
        CsrBtGattAdvertiseReq *msg__;                                   \
        msg__=CsrBtGattAdvertiseReq_struct(_gattId,TRUE,CSR_BT_GATT_FLAGS_NONE,0,NULL,0,NULL); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattAdvertiseReqStartDataSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3

   Description:
   Request to start transmitting advertising packets, where the
   given Advertising and Scan Response data is sent in the
   advertising events.

   As confirmation the application will receive a
   CSR_BT_GATT_ADVERTISE_CFM message.
   
  Parameters: 
    CsrBtGattId _gattId          - Application identifier
    CsrBtGattConnFlags _advFlags - The advertising flags, which are defined 
                                   in csr_bt_gatt_prim.h. These are valid:
                                   CSR_BT_GATT_FLAGS_NONE
                                   CSR_BT_GATT_FLAGS_NONDISCOVERABLE
                                   CSR_BT_GATT_FLAGS_LIMITED_DISCOVERABLE
                                   CSR_BT_GATT_FLAGS_DISABLE_SCAN_RESPONSE
                                   CSR_BT_GATT_FLAGS_APPEND_DATA
                                   CSR_BT_GATT_FLAGS_WHITELIST_SCANRSP
    CsrUint8 _advDataLength      - Length of the advertising Data in octets.
    CsrUint8 *_advData           - The advertising data formatted as 
                                   defined in [Vol 3] Part C, Section 11 -
                                   ADVERTISING AND SCAN RESPONSE DATA FORMAT
    CsrUint8 _scanRspDataLength  - Length of the Scan Response Data in octets.
    CsrUint8 *_scanRspData       - The Scan Response Data formatted as 
                                   defined in [Vol 3] Part C, Section 11 -
                                   ADVERTISING AND SCAN RESPONSE DATA FORMAT
   -------------------------------------------------------------------- */
#define CsrBtGattAdvertiseReqStartDataSend(_gattId,_advFlags,_advDataLength, \
                                           _advData,_scanRspDataLength,_scanRspData) { \
        CsrBtGattAdvertiseReq *msg__;                                   \
        msg__=CsrBtGattAdvertiseReq_struct(_gattId,TRUE,_advFlags,_advDataLength, \
                                           _advData,_scanRspDataLength,_scanRspData); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattAdvertiseReqStopSend

   Description:
   Request to stop transmitting advertising packets.

   As confirmation the application will receive a
   CSR_BT_GATT_ADVERTISE_CFM message.

   Parameters:
   CsrBtGattId _gattId         - Application identifier
   -------------------------------------------------------------------- */
#define CsrBtGattAdvertiseReqStopSend(_gattId) {                        \
        CsrBtGattAdvertiseReq *msg__;                                   \
        msg__=CsrBtGattAdvertiseReq_struct(_gattId,FALSE,CSR_BT_GATT_FLAGS_NONE,0,NULL,0,NULL); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattParamScanReqSend

   Description:
   Change default parameters for scanning

   As confirmation the application will receive a
   CSR_BT_GATT_PARAM_SCAN_CFM message.

   Parameters:
   CsrBtGattId _gattId         - Application identifier
   CsrUint16   _scanInterval   - Scan interval (slots)
   CsrUint16   _scanWindow     - Scan window (slots)
   -------------------------------------------------------------------- */
CsrBtGattParamScanReq *CsrBtGattParamScanReq_struct(CsrBtGattId gattId,
                                                    CsrUint16 scanInterval,
                                                    CsrUint16 scanWindow);
#define CsrBtGattParamScanReqSend(_gattId,_scanInterval,_scanWindow) {  \
        CsrBtGattParamScanReq *msg__;                                   \
        msg__ = CsrBtGattParamScanReq_struct(_gattId,_scanInterval,_scanWindow); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattParamAdvertiseReqSend

   Description:
   Change default parameters for advertising

   As confirmation the application will receive a
   CSR_BT_GATT_PARAM_ADVERTISE_CFM message.

   Parameters:
   CsrBtGattId _gattId         - Application identifier
   CsrUint16   _advIntervalMin - Min. advertising interval (slots)
   CsrUint16   _advIntervalMax - Max. advertising interval (slots)
   -------------------------------------------------------------------- */
CsrBtGattParamAdvertiseReq *CsrBtGattParamAdvertiseReq_struct(CsrBtGattId gattId,
                                                              CsrUint16 advIntervalMin,
                                                              CsrUint16 advIntervalMax);
#define CsrBtGattParamAdvertiseReqSend(_gattId,_advIntervalMin, _advIntervalMax) { \
        CsrBtGattParamAdvertiseReq *msg__;                              \
        msg__ = CsrBtGattParamAdvertiseReq_struct(_gattId,_advIntervalMin, _advIntervalMax); \
        CsrBtGattMsgTransport(msg__);}

#ifdef CSR_BT_INSTALL_GATT_PARAM_CONNECTION
/* --------------------------------------------------------------------
   Name:
   CsrBtGattParamConnectionReqSend

  Description: 
    Change default parameters for future LE connections

    As confirmation the application will receive a 
    CSR_BT_GATT_PARAM_CONNECTION_CFM message. 
   
  Parameters: 
    CsrBtGattId _gattId         - Application identifier    
    CsrUint16 _scanInterval     - Scan interval (slots) 
                                  Range: 0x0004 to 0x4000
    CsrUint16 _scanWindow       - Scan window (slots) 
                                  Range: 0x0004 to 0x4000. 
                                  Shall be less than or equal to 
                                  the 'scanInterval' parameter
    CsrUint16 _connIntervalMin  - Connection interval (slots) 
                                  Range: 0x0006 to 0x0C80. 
                                  Shall be less than or equal to 
                                  the 'connIntervalMax' parameter.
    CsrUint16 _connIntervalMax  - Connection interval (slots) 
                                  Range: 0x0006 to 0x0C80. 
                                  Shall be greater than or equal to 
                                  the 'connIntervalMin' parameter
    CsrUint16 _connLatency      - Connection latency 
                                  Range: 0x0000 to 0x01F3. 
                                  Shall be less than or equal to 
                                  the 'connLatencyMax' parameter
    CsrUint16 _supervisionTimeout - Preferred LSTO
                                  Shall be greater than or equal 
                                  to the 'supervisionTimeoutMin' 
                                  parameter and it shall be less 
                                  than or equal to the 
                                  'supervisionTimeoutMax' parameter.  
    CsrUint16 _connAttemptTimeout - Connection attempt timeout
    CsrUint16 _advIntervalMin   - Advertising interval
    CsrUint16 _advIntervalMax   - Advertsing interval
    CsrUint16 _connLatencyMax   - Max connection latency
                                  Maximum value is 0x01F3  
    CsrUint16 _supervisionTimeoutMin - Min LSTO
                                       Minimum value is 0x000A
    CsrUint16 _supervisionTimeoutMax - Max LSTO
                                       Maximum value is 0x0C80 
   -------------------------------------------------------------------- */
CsrBtGattParamConnectionReq *CsrBtGattParamConnectionReq_struct(CsrBtGattId gattId,
                                                                CsrUint16 scanInterval,
                                                                CsrUint16 scanWindow,
                                                                CsrUint16 connIntervalMin,
                                                                CsrUint16 connIntervalMax,
                                                                CsrUint16 connLatency,
                                                                CsrUint16 supervisionTimeout,
                                                                CsrUint16 connAttemptTimeout,
                                                                CsrUint16 advIntervalMin,
                                                                CsrUint16 advIntervalMax,
                                                                CsrUint16 connLatencyMax,
                                                                CsrUint16 supervisionTimeoutMin,
                                                                CsrUint16 supervisionTimeoutMax);
#define CsrBtGattParamConnectionReqSend(_gattId,_scanInterval,_scanWindow, \
                                        _connIntervalMin,_connIntervalMax, \
                                        _connLatency,_supervisionTimeout, \
                                        _connAttemptTimeout,_advIntervalMin, \
                                        _advIntervalMax,_connLatencyMax, \
                                        _supervisionTimeoutMin,_supervisionTimeoutMax) { \
        CsrBtGattParamConnectionReq *msg__;                             \
        msg__ = CsrBtGattParamConnectionReq_struct(_gattId,_scanInterval,_scanWindow, \
                                                   _connIntervalMin,_connIntervalMax, \
                                                   _connLatency,_supervisionTimeout, \
                                                   _connAttemptTimeout,_advIntervalMin, \
                                                   _advIntervalMax,_connLatencyMax, \
                                                   _supervisionTimeoutMin,_supervisionTimeoutMax); \
        CsrBtGattMsgTransport(msg__);}
#endif

/* --------------------------------------------------------------------
   Name:
   CsrBtGattParamConnUpdateReqSend

   Description:
   Change connection parameters for an active connection

  Description: 
    Change connection parameters for an active LE connection.
    Note cannot be used by an BR/EDR connection

    As confirmation the application will receive a 
    CSR_BT_GATT_PARAM_CONN_UPDATE_CFM message. 
   
  Parameters: 
    CsrBtGattId _gattId         - Application identifier    
    CsrBtConnId _btConnId       - Connection identifier
    CsrUint16 _connIntervalMin  - Connection interval (slots)
                                  Range: 0x0006 to 0x0C80. 
                                  Shall be less than or equal to 
                                  the 'connIntervalMax' parameter.
    CsrUint16 _connIntervalMax  - Connection interval (slots)
                                  Range: 0x0006 to 0x0C80. 
                                  Shall be greater than or equal to 
                                  the 'connIntervalMin' parameter  
    CsrUint16 _connLatency      - Connection latency
                                  Range: 0x0000 to 0x01F3  
    CsrUint16 _supervisionTimeout - Preferred LSTO
                                    Range: 0x000A to 0x0C80
    CsrUint16 _minimumCeLength  - Expected min connection event length
                                  Range: 0x0000 - 0xFFFF  
    CsrUint16 _maximumCeLength  - Expected max connection event length
                                  Range: 0x0000 - 0xFFFF  
   -------------------------------------------------------------------- */
CsrBtGattParamConnUpdateReq *CsrBtGattParamConnUpdateReq_struct(CsrBtGattId gattId,
                                                                CsrBtConnId btConnId,
                                                                CsrUint16 connIntervalMin,
                                                                CsrUint16 connIntervalMax,
                                                                CsrUint16 connLatency,
                                                                CsrUint16 supervisionTimeout,
                                                                CsrUint16 minimumCeLength,
                                                                CsrUint16 maximumCeLength,
                                                                CsrBool   accept,
                                                                l2ca_identifier_t l2caSignalId);
#define CsrBtGattParamConnUpdateReqSend(_gattId,_btConnId,_connIntervalMin, \
                                        _connIntervalMax,_connLatency,  \
                                        _supervisionTimeout,_minimumCeLength, \
                                        _maximumCeLength) { \
        CsrBtGattParamConnUpdateReq *msg__; \
        msg__ = CsrBtGattParamConnUpdateReq_struct(_gattId,_btConnId,_connIntervalMin, \
                                                   _connIntervalMax,_connLatency, \
                                                   _supervisionTimeout,_minimumCeLength, \
                                                   _maximumCeLength,TRUE,0); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattDisconnectReqSend

   Description:
   Request to release/cancel a LE or BR/EDR connection

   As confirmation the application will receive a
   CSR_BT_GATT_DISCONNECT_IND message.

   Parameters:
   CsrBtGattId _gattId    - Application identifier
   CsrBtConnId _btConnId  - Connection identifier
   -------------------------------------------------------------------- */
CsrBtGattDisconnectReq *CsrBtGattDisconnectReq_struct(CsrBtGattId gattId,
                                                      CsrBtConnId btConnId);
#define CsrBtGattDisconnectReqSend(_gattId,_btConnId) {                 \
        CsrBtGattDisconnectReq *msg__;                                  \
        msg__=CsrBtGattDisconnectReq_struct(_gattId,_btConnId);         \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattDbAllocReqSend

   Description:

   This request shall be used by the server, for reserving/allocating
   a range of attribute handles which it can use later for
   creating the local database.

   As confirmation the application will receive a
   CSR_BT_GATT_DB_ALLOC_CFM message.

   Parameters:
   CsrBtGattId _gattId               - Application identifier
   CsrUint16   _numOfAttrHandles     - Number of attribute handles
   CsrUint16   _preferredStartHandle - The StartHandle the application
   prefers, 0 = no preference
   -------------------------------------------------------------------- */
CsrBtGattDbAllocReq *CsrBtGattDbAllocReq_struct(CsrBtGattId gattId,
                                                CsrUint16   numOfAttrHandles,
                                                CsrUint16   preferredStartHandle);

#define CsrBtGattDbAllocReqSend(_gattId,_numOfAttrHandles,_preferredStartHandle) { \
        CsrBtGattDbAllocReq *msg__;                                     \
        msg__=CsrBtGattDbAllocReq_struct(_gattId,_numOfAttrHandles,_preferredStartHandle); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattDbDeallocReqSend

   Description:

   The server application can free/deallocate the range of
   attribute handles that are previously allocated by calling
   CsrBtGattDbAllocReqSend.

   As confirmation the application will receive a
   CSR_BT_GATT_DB_DEALLOC_CFM message.

   Note that if some of the handles being deallocated
   are still present in the local Database, GATT will
   automatically remove them.

   Parameters:
   CsrBtGattId _gattId - Application identifier
   -------------------------------------------------------------------- */
CsrBtGattDbDeallocReq *CsrBtGattDbDeallocReq_struct(CsrBtGattId gattId);

#define CsrBtGattDbDeallocReqSend(_gattId) {                            \
        CsrBtGattDbDeallocReq *msg__;                                   \
        msg__=CsrBtGattDbDeallocReq_struct(_gattId);                    \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattDbAddReqSend

   Description:

   This request shall be used by the server, for adding attributes to
   the local database.

   As confirmation the application will receive a
   CSR_BT_GATT_DB_ADD_CFM message.

   Parameters:
   CsrBtGattId _gattId - Application identifier
   CsrBtGattDb _db     - The local database
   -------------------------------------------------------------------- */
CsrBtGattDbAddReq *CsrBtGattDbAddReq_struct(CsrBtGattId gattId,
                                            CsrBtGattDb *db);

#define CsrBtGattDbAddReqSend(_gattId,_db) {                            \
        CsrBtGattDbAddReq *msg__;                                       \
        msg__=CsrBtGattDbAddReq_struct(_gattId,_db);                    \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattDbRemoveReqSend

   Description:

   This request shall be use, by the server, for removing attributes
   from the local database.

   As confirmation the application will receive a
   CSR_BT_GATT_DB_REMOVE_CFM message.

   Parameters:
   CsrBtGattId _gattId    - Application identifier
   CsrBtGattHandle _start - First handle number to be removed
   CsrBtGattHandle _end   - Last handle number to be removed
   -------------------------------------------------------------------- */
CsrBtGattDbRemoveReq *CsrBtGattDbRemoveReq_struct(CsrBtGattId     gattId,
                                                  CsrBtGattHandle start,
                                                  CsrBtGattHandle end);

#define CsrBtGattDbRemoveReqSend(_gattId,_start,_end) {                 \
        CsrBtGattDbRemoveReq *msg__;                                    \
        msg__=CsrBtGattDbRemoveReq_struct(_gattId,_start,_end);         \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Common structure for the CsrBtGattDbAccessRes primitive which
   is used by the functions:
   CsrBtGattDbWriteAccessResSend
   CsrBtGattDbReadAccessResSend
   -------------------------------------------------------------------- */
CsrBtGattDbAccessRes *CsrBtGattDbAccessRes_struct(CsrBtGattId              gattId,
                                                  CsrBtConnId              btConnId,
                                                  CsrBtGattHandle          attrHandle,
                                                  CsrBtGattDbAccessRspCode responseCode,
                                                  CsrUint16                valueLength,
                                                  CsrUint8                 *value);

/* --------------------------------------------------------------------
   Name:
   CsrBtGattDbWriteAccessResSend

   Description:
   This function shall always be called after the
   application has received a CSR_BT_GATT_DB_ACCESS_WRITE_IND
   message.

   Parameters:
   CsrBtGattId _gattId               - Application identifier
   CsrBtConnId _btConnId             - Connection identifier
   CsrBtGattHandle _handle           - The attribute handle
   received in the
   CSR_BT_GATT_DB_ACCESS_WRITE_IND
   message.
   CsrBtGattDbAccessRspCode _rspCode - One of the responseCodes defined
   in csr_bt_gatt_prim.h
   -------------------------------------------------------------------- */
#define CsrBtGattDbWriteAccessResSend(_gattId,_btConnId,_handle,_rspCode) { \
        CsrBtGattDbAccessRes *msg__;                                    \
        msg__=CsrBtGattDbAccessRes_struct(_gattId,_btConnId,_handle,_rspCode,0,NULL); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattDbReadAccessResSend

   Description:
   This function shall always be called after the
   application has received a CSR_BT_GATT_DB_ACCESS_READ_IND
   message.

   Parameters:
   CsrBtGattId _gattId               - Application identifier
   CsrBtConnId _btConnId             - Connection identifier
   CsrBtGattHandle _handle           - The attribute handle
   received in the
   CSR_BT_GATT_DB_ACCESS_READ_IND
   message.
   CsrBtGattDbAccessRspCode _rspCode - One of the responseCodes defined
   in csr_bt_gatt_prim.h
   CsrUint16 _valueLength            - Length of the attribute value which
   has been read by the application
   CsrUint8 *_value                  - An allocated pointer of the attribute
   value.
   -------------------------------------------------------------------- */
#define CsrBtGattDbReadAccessResSend(_gattId,_btConnId,_handle,_rspCode,_valueLength,_value) { \
        CsrBtGattDbAccessRes *msg__;                                    \
        msg__=CsrBtGattDbAccessRes_struct(_gattId,_btConnId,_handle,_rspCode,_valueLength,_value); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Common structure for the CsrBtGattEventSendReq primitive which
   is used by the functions:
   CsrBtGattNotificationEventReqSend
   CsrBtGattIndicationEventReqSend
   CsrBtGattServiceChangedEventReqSend
   -------------------------------------------------------------------- */
CsrBtGattEventSendReq *CsrBtGattEventSendReq_struct(CsrBtGattId     gattId,
                                                    CsrBtConnId     btConnId,
                                                    CsrBtGattHandle attrHandle,
                                                    CsrBtGattHandle endGroupHandle,
                                                    CsrUint16       flags,
                                                    CsrUint16       valueLength,
                                                    CsrUint8        *value);
/* --------------------------------------------------------------------
   Name:
   CsrBtGattNotificationEventReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3

   Description:

   This request is used, by the server, for sending a Charateristic
   Value notification to a Client, as defined in [Vol. 3]
   Part G section 4.10.1 - Notifications.

   As confirmation the application will receive a
   CSR_BT_GATT_EVENT_SEND_CFM message.

   Parameters:
   CsrBtGattId     _gattId      - Application identifier
   CsrBtConnId     _btConnId    - Connection identifier
   CsrBtGattHandle _attrHandle  - The Characteristic Value Handle being notified
   CsrUint16       _valueLength - Length of the attribute value that must be sent to Client
   CsrUint8 *      _value       - An allocated pointer of the attribute value
   -------------------------------------------------------------------- */
#define CsrBtGattNotificationEventReqSend(_gattId,_btConnId,_attrHandle,_valueLength,_value) { \
        CsrBtGattEventSendReq *msg__;                                   \
        msg__=CsrBtGattEventSendReq_struct(_gattId,_btConnId,_attrHandle,CSR_BT_GATT_ATTR_HANDLE_INVALID, \
                                           CSR_BT_GATT_NOTIFICATION_EVENT,_valueLength,_value); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattIndicationEventReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3

   Description:

   This request is used, by the server, for sending a Charateristic
   Value indication to a Client, as defined in [Vol. 3]
   Part G section 4.11.1 - Indications.

   As confirmation the application will receive a
   CSR_BT_GATT_EVENT_SEND_CFM message.

   Parameters:
   CsrBtGattId     _gattId      - Application identifier
   CsrBtConnId     _btConnId    - Connection identifier
   CsrBtGattHandle _attrHandle  - The Characteristic Value Handle being indicated
   CsrUint16       _valueLength - Length of the attribute value that must be sent to Client
   CsrUint8 *      _value       - An allocated pointer of the attribute value
   -------------------------------------------------------------------- */
#define CsrBtGattIndicationEventReqSend(_gattId,_btConnId,_attrHandle,_valueLength,_value) { \
        CsrBtGattEventSendReq *msg__;                                   \
        msg__=CsrBtGattEventSendReq_struct(_gattId,_btConnId,_attrHandle,CSR_BT_GATT_ATTR_HANDLE_INVALID, \
                                           CSR_BT_GATT_INDICATION_EVENT,_valueLength,_value); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattServiceChangedEventReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3

   Description:
   This request shall be used, by the server application, for
   indicating to connected devices that services have changed.
   A GATT based service is considered modified if the
   binding of the Attribute Handles to the associated Attributes
   group within a service definition are changed

   As confirmation the application will receive a
   CSR_BT_GATT_EVENT_SEND_CFM message.

   Note, for clients that have a trusted relationship (i.e. bond)
   with the local server, the attribute cache is valid across connections.
   For clients with a trusted relationship and not in a connection
   when a service change occurs, the local server shall send an indication
   when the client reconnects to the local server. For clients that do not
   have a trusted relationship with the local server, the attribute cache is
   valid only during the connection. Clients without a trusted relationship
   shall receive an indication when the service change occurs only during
   the current connection. For more info please refer to
   [Vol. 3] Part G section 7.1 - Service Changed

   Parameters:
   CsrBtGattId     _gattId      - Application identifier
   CsrBtConnId     _btConnId    - Connection identifier
   CsrBtGattHandle _startHandle - Start of Affected Attribute Handle Range
   CsrBtGattHandle _endHandle   - End of Affected Attribute Handle Range
   -------------------------------------------------------------------- */
#define CsrBtGattServiceChangedEventReqSend(_gattId,_btConnId,_startHandle,_endHandle) { \
        CsrBtGattEventSendReq *msg__;                                   \
        msg__=CsrBtGattEventSendReq_struct(_gattId,_btConnId,_startHandle, \
                                           _endHandle,CSR_BT_GATT_SERVICE_CHANGED_EVENT,0,NULL); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Common structure for the CsrBtGattDiscoverServicesReq primitive
   which is used by the functions:
   CsrBtGattDiscoverAllPrimaryServicesReqSend
   CsrBtGattDiscoverAllPrimaryServicesLocalReqSend
   CsrBtGattDiscoverPrimaryServicesBy16BitUuidReqSend
   CsrBtGattDiscoverPrimaryServicesBy16BitUuidLocalReqSend
   CsrBtGattDiscoverPrimaryServicesBy128BitUuidReqSend
   CsrBtGattDiscoverPrimaryServicesBy128BitUuidLocalReqSend

   These functions Covers item 2, Primary Service Discovery,
   in the GATT feature table, which is defined in The BLUETOOTH
   SPECIFICATION Version 4.0 Vol 3 Part G section 4.2 -
   Feature Support And Procedure Mapping.
   -------------------------------------------------------------------- */
CsrBtGattDiscoverServicesReq *CsrBtGattDiscoverServicesReq_struct(CsrBtGattId gattId,
                                                                  CsrBtConnId btConnId,
                                                                  CsrBtUuid   uuid);

/* --------------------------------------------------------------------
   Name:
   CsrBtGattDiscoverAllPrimaryServicesReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3

   Description:
   This request is used by a client for discovering all the primary
   services on a server. This procedure is defined in [Vol. 3]
   Part G section 4.4.1 - Discover All Primary Services

   Every time a primary service has been discovered a
   CSR_BT_GATT_DISCOVER_SERVICES_IND message is sent to the
   application. This procedure is complete when a
   CSR_BT_GATT_DISCOVER_SERVICES_CFM message is received

   The application is permitted to cancel this procedure early
   if a desired primary service is found prior to discovering
   all the primary services on the server. This can be done by
   calling CsrBtGattCancelReqSend.

   As confirmation the application will still receive the
   CSR_BT_GATT_DISCOVER_SERVICES_CFM message.

   Parameters:
   CsrBtGattId _gattId   - Application identifier
   CsrBtConnId _btConnId - Connection identifier
   -------------------------------------------------------------------- */
#define CsrBtGattDiscoverAllPrimaryServicesReqSend(_gattId,_btConnId) { \
        CsrBtGattDiscoverServicesReq *msg__;                            \
        CsrBtUuid _uuid;                                                \
        CsrMemSet(_uuid.uuid, 0, CSR_BT_UUID128_SIZE);                  \
        _uuid.length = (CsrUint16) (sizeof(CsrBtUuid16));               \
        CSR_COPY_UINT16_TO_LITTLE_ENDIAN(CSR_BT_GATT_PRIMARY_SERVICE_UUID,_uuid.uuid); \
        msg__=CsrBtGattDiscoverServicesReq_struct(_gattId,_btConnId,_uuid); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattDiscoverAllPrimaryServicesLocalReqSend

   Description:
   The only differece re. the CsrBtGattDiscoverAllPrimaryServicesReqSend
   is that the primary services are discovered from the local
   database and not from a peer server.

   Parameters:
   CsrBtGattId _gattId   - Application identifier
   -------------------------------------------------------------------- */
#define CsrBtGattDiscoverAllPrimaryServicesLocalReqSend(_gattId) {      \
        CsrBtGattDiscoverAllPrimaryServicesReqSend(_gattId,CSR_BT_GATT_LOCAL_BT_CONN_ID);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattDiscoverPrimaryServicesBy16BitUuidReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3

   Description:
   This request is used by a client for discovering a specific
   primary service on a server when only the 16-Bit Service
   UUID is known. This procedure is defined in [Vol. 3]
   Part G section 4.4.2 - Discover Primary Service by Service UUID.

   Every time a primary service with a matching 16-Bit service UUID
   has been discovered a CSR_BT_GATT_DISCOVER_SERVICES_IND message is
   sent to the application. This procedure is complete when a
   CSR_BT_GATT_DISCOVER_SERVICES_CFM message is received.

   Please note, that a specific primary service may exist multiple
   times on a server.

   The application is permitted to cancel this procedure early
   if a desired primary service is found prior to discovering
   all the primary services on the server. This can is done by
   calling CsrBtGattCancelReqSend.

   As confirmation the application will still receive the
   CSR_BT_GATT_DISCOVER_SERVICES_CFM message.

   Parameters:
   CsrBtGattId _gattId   - Application identifier
   CsrBtConnId _btConnId - Connection identifier
   CsrBtUuid16 _uuid16   - The 16-bit service UUID to look for.
   -------------------------------------------------------------------- */
#define CsrBtGattDiscoverPrimaryServicesBy16BitUuidReqSend(_gattId,_btConnId,_uuid16) { \
        CsrBtGattDiscoverServicesReq *msg__;                            \
        CsrBtUuid _uuid;                                                \
        CsrMemSet(_uuid.uuid, 0, CSR_BT_UUID128_SIZE);                  \
        _uuid.length = (CsrUint16) (sizeof(CsrBtUuid16));               \
        CSR_COPY_UINT16_TO_LITTLE_ENDIAN(_uuid16,_uuid.uuid);           \
        msg__=CsrBtGattDiscoverServicesReq_struct(_gattId,_btConnId,_uuid); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattDiscoverPrimaryServicesBy16BitUuidLocalReqSend

   Description:
   As the only difference re. CsrBtGattDiscoverPrimaryServicesBy16BitUuidReqSend
   is that the specified primary service are discovered from the local database
   and not from a peer server.

   Parameters:
   CsrBtGattId _gattId   - Application identifier
   CsrBtUuid16 _uuid16   - The 16-bit service UUID to look for.
   -------------------------------------------------------------------- */
#define CsrBtGattDiscoverPrimaryServicesBy16BitUuidLocalReqSend(_gattId,_uuid16) { \
        CsrBtGattDiscoverPrimaryServicesBy16BitUuidReqSend(_gattId,CSR_BT_GATT_LOCAL_BT_CONN_ID,_uuid16);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattDiscoverPrimaryServicesBy128BitUuidReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3

   Description:
   This request is used by a client for discovering a specific
   primary service on a server when only the 128-Bit Service
   UUID is known. This procedure is defined in [Vol. 3]
   Part G section 4.4.2 - Discover Primary Service by Service UUID.

   Every time a primary service with a matching 128-Bit service UUID
   has been discovered a CSR_BT_GATT_DISCOVER_SERVICES_IND message is
   sent to the application. This procedure is complete when a
   CSR_BT_GATT_DISCOVER_SERVICES_CFM message is received

   Please note, that a specific primary service may exist multiple
   times on a server.

   The application is permitted to cancel this procedure early
   if a desired primary service is found prior to discovering
   all the primary services on the server. This can is done by
   calling CsrBtGattCancelReqSend.

   As confirmation the application will still receive the
   CSR_BT_GATT_DISCOVER_SERVICES_CFM message.

   Parameters:
   CsrBtGattId _gattId   - Application identifier
   CsrBtConnId _btConnId - Connection identifier
   CsrBtUuid128 _uuid128 - The 128-bit service UUID to look for.
   -------------------------------------------------------------------- */
#define CsrBtGattDiscoverPrimaryServicesBy128BitUuidReqSend(_gattId,_btConnId,_uuid128) { \
        CsrBtGattDiscoverServicesReq *msg__;                            \
        CsrBtUuid _uuid;                                                \
        _uuid.length = (CsrUint16) (sizeof(CsrBtUuid128));              \
        CsrMemCpy(_uuid.uuid,_uuid128,CSR_BT_UUID128_SIZE);             \
        msg__=CsrBtGattDiscoverServicesReq_struct(_gattId,_btConnId,_uuid); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattDiscoverPrimaryServicesBy128BitUuidLocalReqSend

   Description:
   As the only difference re. CsrBtGattDiscoverPrimaryServicesBy128BitUuidReqSend
   is that the specified primary service is discovered from the local
   database and not from a peer server.

   Parameters:
   CsrBtGattId  _gattId   - Application identifier
   CsrBtUuid128 _uuid128  - The 128-bit service UUID to look for.
   -------------------------------------------------------------------- */
#define CsrBtGattDiscoverPrimaryServicesBy128BitUuidLocalReqSend(_gattId,_uuid128) { \
        CsrBtGattDiscoverPrimaryServicesBy128BitUuidReqSend(_gattId,CSR_BT_GATT_LOCAL_BT_CONN_ID,_uuid128);}


/* --------------------------------------------------------------------
   Common structure for the CsrBtGattFindInclServicesReq primitive
   which is used by the functions:
   CsrBtGattFindInclServicesReqSend
   CsrBtGattFindInclServicesLocalReqSend

   These functions Covers item 3, Relationship Discovery,
   in the GATT feature table, which is defined in The BLUETOOTH
   SPECIFICATION Version 4.0 Vol 3 Part G section 4.2 -
   Feature Support And Procedure Mapping.
   -------------------------------------------------------------------- */
CsrBtGattFindInclServicesReq *CsrBtGattFindInclServicesReq_struct(CsrBtGattId gattId,
                                                                  CsrBtConnId btConnId,
                                                                  CsrBtGattHandle startHandle,
                                                                  CsrBtGattHandle endGroupHandle);

/* --------------------------------------------------------------------
   Name:
   CsrBtGattFindInclServicesReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3

   Description:
   This request is used by a client for finding include service
   declarations within a service definition on a server, as
   defined in [Vol. 3]  Part G section 4.5.1 - Find Included Services.

   The service specified is identified by the service handle range.
   E.g. the Starting Handle shall be set to starting handle of the
   specified service and the End Group Handle shall be set to the
   ending handle of the specified service.

   Every time an Included Service Declaration has been discovered a
   CSR_BT_GATT_FIND_INCL_SERVICES_IND message is
   sent to the application. This procedure is complete when a
   CSR_BT_GATT_FIND_INCL_SERVICES_CFM message is received.

   The application is permitted to cancel this procedure early
   if a desired included service is found prior to discovering
   all the included services on the server. This can be done by
   calling CsrBtGattCancelReqSend.

   As confirmation the application will still receive the
   CSR_BT_GATT_FIND_INCL_SERVICES_CFM message.

   Parameters:
   CsrBtGattId _gattId          - Application identifier
   CsrBtConnId _btConnId        - Connection identifier
   CsrBtGattHandle _startHandle - The start handle of the specified service.
   CsrBtGattHandle _endHandle   - The end handle of the specified service.
   -------------------------------------------------------------------- */
#define CsrBtGattFindInclServicesReqSend(_gattId,_btConnId,_startHandle,_endHandle) { \
        CsrBtGattFindInclServicesReq *msg__;                            \
        msg__=CsrBtGattFindInclServicesReq_struct(_gattId,_btConnId,_startHandle,_endHandle); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattFindInclServicesLocalReqSend

   Description:
   The only difference re. CsrBtGattFindInclServicesReqSend is that
   the include service declarations are discovered from the local
   database and not from a peer server.

   Parameters:
   CsrBtGattId     _gattId      - Application identifier
   CsrBtGattHandle _startHandle - The start handle of the specified service.
   CsrBtGattHandle _endHandle   - The end handle of the specified service.
   -------------------------------------------------------------------- */
#define CsrBtGattFindInclServicesLocalReqSend(_gattId,_startHandle,_endHandle) { \
        CsrBtGattFindInclServicesReqSend(_gattId,CSR_BT_GATT_LOCAL_BT_CONN_ID,_startHandle,_endHandle);}

/* --------------------------------------------------------------------
   Common structure for the CsrBtGattDiscoverCharacReq primitive
   which is used by the functions:
   CsrBtGattDiscoverAllCharacOfAServiceReqSend
   CsrBtGattDiscoverAllCharacOfAServiceLocalReqSend
   CsrBtGattDiscoverCharacBy16BitUuidReqSend
   CsrBtGattDiscoverCharacBy16BitUuidLocalReqSend
   CsrBtGattDiscoverCharacBy128BitUuidReqSend
   CsrBtGattDiscoverCharacBy128BitUuidLocalReqSend

   These functions cover item 4, Characteristic Discovery,
   in the GATT feature table, which is defined in The BLUETOOTH
   SPECIFICATION Version 4.0 Vol 3 Part G section 4.2 -
   Feature Support And Procedure Mapping.
   -------------------------------------------------------------------- */
CsrBtGattDiscoverCharacReq *CsrBtGattDiscoverCharacReq_struct(CsrBtGattId     gattId,
                                                              CsrBtConnId     btConnId,
                                                              CsrBtUuid       uuid,
                                                              CsrBtGattHandle startHandle,
                                                              CsrBtGattHandle endGroupHandle);

/* --------------------------------------------------------------------
   Name:
   CsrBtGattDiscoverAllCharacOfAServiceReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3

   Description:
   This request is used by a client for finding all the characteristic
   declarations within a service definition on a server when only
   the service handle range is known, as defined in [Vol. 3] Part
   G section 4.6.1 - Discover All Characteristics of a Service.

   The service specified is identified by the service handle range.
   E.g. the Starting Handle shall be set to starting handle of the
   specified service and the Ending Handle shall be set to the
   ending handle of the specified service.

   Every time a characteristic declaration has been discovered
   a CSR_BT_GATT_DISCOVER_CHARAC_IND message is
   sent to the application. This procedure is complete when a
   CSR_BT_GATT_DISCOVER_CHARAC_CFM message is received.

   The application is permitted to cancel this procedure early
   if a desired characteristic is found prior to discovering
   all the characteristics of the specified service supported
   on the server. This can is done by calling CsrBtGattCancelReqSend.

   As confirmation the application will still receive the
   CSR_BT_GATT_DISCOVER_CHARAC_CFM message.

   Parameters:
   CsrBtGattId _gattId          - Application identifier
   CsrBtConnId _btConnId        - Connection identifier
   CsrBtGattHandle _startHandle - The start handle of the specified service.
   CsrBtGattHandle _endHandle   - The end handle of the specified service.
   -------------------------------------------------------------------- */
#define CsrBtGattDiscoverAllCharacOfAServiceReqSend(_gattId,_btConnId,_startHandle,_endHandle) { \
        CsrBtGattDiscoverCharacReq *msg__;                              \
        CsrBtUuid _uuid;                                                \
        CsrMemSet(_uuid.uuid, 0, CSR_BT_UUID128_SIZE);                  \
        _uuid.length = (CsrUint16) (sizeof(CsrBtUuid16));               \
        CSR_COPY_UINT16_TO_LITTLE_ENDIAN(CSR_BT_GATT_CHARACTERISTIC_UUID,_uuid.uuid); \
        msg__=CsrBtGattDiscoverCharacReq_struct(_gattId,_btConnId,_uuid,_startHandle,_endHandle); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattDiscoverAllCharacOfAServiceLocalReqSend

   Description:
   The only difference re. CsrBtGattDiscoverAllCharacOfAServiceReqSend
   is that all the characteristic declarations within a service
   definition are discovered from the local database and not from
   a peer server.

   Parameters:
   CsrBtGattId _gattId          - Application identifier
   CsrBtGattHandle _startHandle - The start handle of the specified service.
   CsrBtGattHandle _endHandle   - The end handle of the specified service.
   -------------------------------------------------------------------- */
#define CsrBtGattDiscoverAllCharacOfAServiceLocalReqSend(_gattId,_startHandle,_endHandle) { \
        CsrBtGattDiscoverAllCharacOfAServiceReqSend(_gattId,CSR_BT_GATT_LOCAL_BT_CONN_ID, \
                                                    _startHandle,_endHandle);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattDiscoverCharacBy16BitUuidReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3

   Description:
   This request is used by a client for discovering service
   characteristics on a server when only the service handle
   ranges are known and the 16-Bit characteristic UUID is known,
   as defined in [Vol. 3] Part G section 4.6.2 - Discover
   Characteristics by UUID.

   The service specified is identified by the service handle range.
   E.g. the Starting Handle shall be set to starting handle of the
   specified service and the Ending Handle shall be set to the
   ending handle of the specified service.

   Every time a characteristic declaration with a matching 16-Bit
   characteristic UUID has been discovered a
   CSR_BT_GATT_DISCOVER_CHARAC_IND message is sent to the
   application. This procedure is complete when a
   CSR_BT_GATT_DISCOVER_CHARAC_CFM message is received.

   The application is permitted to cancel this procedure early
   if a desired characteristic is found prior to discovering
   all the characteristics of the specified service supported
   on the server. This can is done by calling CsrBtGattCancelReqSend.

   As confirmation the application will still receive the
   CSR_BT_GATT_DISCOVER_CHARAC_CFM message.

   Parameters:
   CsrBtGattId _gattId          - Application identifier
   CsrBtConnId _btConnId        - Connection identifier
   CsrBtGattHandle _startHandle - The start handle of the specified service.
   CsrBtGattHandle _endHandle   - The end handle of the specified service.
   CsrBtUuid16 _uuid16          - The 16-bit characteristic UUID
   -------------------------------------------------------------------- */
#define CsrBtGattDiscoverCharacBy16BitUuidReqSend(_gattId,_btConnId,_startHandle, \
                                                  _endHandle,_uuid16) { \
        CsrBtGattDiscoverCharacReq *msg__;                              \
        CsrBtUuid _uuid;                                                \
        CsrMemSet(_uuid.uuid, 0, CSR_BT_UUID128_SIZE);                  \
        _uuid.length = (CsrUint16) (sizeof(CsrBtUuid16));               \
        CSR_COPY_UINT16_TO_LITTLE_ENDIAN(_uuid16,_uuid.uuid);           \
        msg__=CsrBtGattDiscoverCharacReq_struct(_gattId,_btConnId,_uuid,_startHandle,_endHandle); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattDiscoverCharacBy16BitUuidLocalReqSend

   Description:
   The only difference re. CsrBtGattDiscoverCharacBy16BitUuidReqSend
   is that characteristics are discovered from the local database and
   not from a peer server.

   Parameters:
   CsrBtGattId _gattId          - Application identifier
   CsrBtGattHandle _startHandle - The start handle of the specified service.
   CsrBtGattHandle _endHandle   - The end handle of the specified service.
   CsrBtUuid16 _uuid16          - The 16-bit characteristic UUID
   -------------------------------------------------------------------- */
#define CsrBtGattDiscoverCharacBy16BitUuidLocalReqSend(_gattId,_startHandle,_endHandle,_uuid16) { \
        CsrBtGattDiscoverCharacBy16BitUuidReqSend(_gattId,CSR_BT_GATT_LOCAL_BT_CONN_ID, \
                                                  _startHandle,_endHandle,_uuid16);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattDiscoverCharacBy128BitUuidReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3

   Description:
   This request is used by a client for discovering service
   characteristics on a server when only the service handle
   ranges are known and the 128-Bit characteristic UUID is known,
   as defined in [Vol. 3] Part G section 4.6.2 - Discover
   Characteristics by UUID.

   The service specified is identified by the service handle range.
   E.g. the Starting Handle shall be set to starting handle of the
   specified service and the Ending Handle shall be set to the
   ending handle of the specified service.

   Every time a characteristic declaration with a matching 128-Bit
   characteristic UUID has been discovered a
   CSR_BT_GATT_DISCOVER_CHARAC_IND message is sent to the
   application. This procedure is complete when a
   CSR_BT_GATT_DISCOVER_CHARAC_CFM message is received.

   The application is permitted to cancel this procedure early
   if a desired characteristic is found prior to discovering
   all the characteristics of the specified service supported
   on the server. This can be done by calling CsrBtGattCancelReqSend.

   As confirmation the application will still receive the
   CSR_BT_GATT_DISCOVER_CHARAC_CFM message.

   Parameters:
   CsrBtGattId _gattId          - Application identifier
   CsrBtConnId _btConnId        - Connection identifier
   CsrBtGattHandle _startHandle - The start handle of the specified service.
   CsrBtGattHandle _endHandle   - The end handle of the specified service.
   CsrBtUuid128 _uuid128        - The 128-bit characteristic UUID
   -------------------------------------------------------------------- */
#define CsrBtGattDiscoverCharacBy128BitUuidReqSend(_gattId,_btConnId,_startHandle, \
                                                   _endHandle,_uuid128) { \
        CsrBtGattDiscoverCharacReq *msg__;                              \
        CsrBtUuid _uuid;                                                \
        _uuid.length = (CsrUint16) (sizeof(CsrBtUuid128));              \
        CsrMemCpy(_uuid.uuid,_uuid128,CSR_BT_UUID128_SIZE);             \
        msg__=CsrBtGattDiscoverCharacReq_struct(_gattId,_btConnId,_uuid,_startHandle,_endHandle); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattDiscoverCharacBy128BitUuidLocalReqSend

   Description:
   The only difference re. CsrBtGattDiscoverCharacBy128BitUuidReqSend
   is that characteristics are discovered from the local database and
   not from a peer server.

   Parameters:
   CsrBtGattId _gattId          - Application identifier
   CsrBtGattHandle _startHandle - The start handle of the specified service.
   CsrBtGattHandle _endHandle   - The end handle of the specified service.
   CsrBtUuid128 _uuid128        - The 128-bit characteristic UUID
   -------------------------------------------------------------------- */
#define CsrBtGattDiscoverCharacBy128BitUuidLocalReqSend(_gattId,_startHandle, \
                                                        _endHandle,_uuid128) { \
        CsrBtGattDiscoverCharacBy128BitUuidReqSend(_gattId,CSR_BT_GATT_LOCAL_BT_CONN_ID, \
                                                   _startHandle,_endHandle,_uuid128);}

/* --------------------------------------------------------------------
   Common structure for the CsrBtGattDiscoverCharacDescriptorsReq
   primitive which is used by the functions:
   CsrBtGattDiscoverAllCharacDescriptorsReqSend
   CsrBtGattDiscoverAllCharacDescriptorsLocalReqSend

   These functions Cover item 5, Characteristic Descriptor Discovery,
   in the GATT feature table, which is defined in The BLUETOOTH
   SPECIFICATION Version 4.0 Vol 3 Part G section 4.2 -
   Feature Support And Procedure Mapping.
   -------------------------------------------------------------------- */
CsrBtGattDiscoverCharacDescriptorsReq *CsrBtGattDiscoverCharacDescriptorsReq_struct(CsrBtGattId gattId,
                                                                                    CsrBtConnId btConnId,
                                                                                    CsrBtGattHandle startHandle,
                                                                                    CsrBtGattHandle endGroupHandle);

/* --------------------------------------------------------------------
   Name:
   CsrBtGattDiscoverAllCharacDescriptorsReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3

   Description:
   This request is used by a client for finding all the characteristic
   descriptor's Attribute Handles and Attribute Types within a
   characteristic definition when only the characteristic handle
   range is known, as defined in [Vol. 3] Part G section 4.7.1 -
   Discover All Characteristic Descriptors.

   The service specified is identified by the service handle range.
   E.g. the Starting Handle shall be set to the handle of the
   specified characteristic value + 1 and the Ending Handle set to
   the ending handle of the specified characteristic.

   Every time a characteristic descriptor has been discovered
   a CSR_BT_GATT_DISCOVER_CHARAC_DESCRIPTORS_IND message is
   sent to the application. This procedure is complete when a
   CSR_BT_GATT_DISCOVER_CHARAC_DESCRIPTORS_CFM message is received.

   The application is permitted to cancel this procedure early
   if a desired Characteristic Descriptor is found prior to
   discovering all the characteristic descriptors of the specified
   characteristic. This can is done by calling CsrBtGattCancelReqSend.

   As confirmation the application will still receive the
   CSR_BT_GATT_DISCOVER_CHARAC_DESCRIPTORS_CFM message.

   Parameters:
   CsrBtGattId _gattId          - Application identifier
   CsrBtConnId _btConnId        - Connection identifier
   CsrBtGattHandle _startHandle - The handle of the specified characteristic value + 1
   CsrBtGattHandle _endHandle   - The end handle of the specified characteristic.
   -------------------------------------------------------------------- */
#define CsrBtGattDiscoverAllCharacDescriptorsReqSend(_gattId,_btConnId, \
                                                     _startHandle,_endHandle) { \
        CsrBtGattDiscoverCharacDescriptorsReq *msg__;                   \
        msg__=CsrBtGattDiscoverCharacDescriptorsReq_struct(_gattId,_btConnId, \
                                                           _startHandle,_endHandle); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattDiscoverAllCharacDescriptorsLocalReqSend

   Description:
   The only difference re. CsrBtGattDiscoverAllCharacDescriptorsReqSend
   is that all the characteristic descriptor's Attribute Handles
   and Attribute Types within a characteristic definition are
   discovered from the local database and not from a peer server.

   Parameters:
   CsrBtGattId _gattId          - Application identifier
   CsrBtGattHandle _startHandle - The handle of the specified characteristic value + 1
   CsrBtGattHandle _endHandle   - The end handle of the specified characteristic.
   -------------------------------------------------------------------- */
#define CsrBtGattDiscoverAllCharacDescriptorsLocalReqSend(_gattId,_startHandle,_endHandle) { \
        CsrBtGattDiscoverAllCharacDescriptorsReqSend(_gattId,CSR_BT_GATT_LOCAL_BT_CONN_ID, \
                                                     _startHandle,_endHandle);}

/* --------------------------------------------------------------------
   Common structure for the CsrBtGattReadReq
   primitive which is used by the functions:
   CsrBtGattReadReqSend
   CsrBtGattReadLocalReqSend
   CsrBtGattReadExtendedPropertiesReqSend
   CsrBtGattReadExtendedPropertiesLocalReqSend
   CsrBtGattReadUserDescriptionReqSend
   CsrBtGattReadUserDescriptionLocalReqSend
   CsrBtGattReadClientConfigurationReqSend
   CsrBtGattReadServerConfigurationReqSend
   CsrBtGattReadServerConfigurationLocalReqSend
   CsrBtGattReadPresentationFormatReqSend
   CsrBtGattReadPresentationFormatLocalReqSend
   CsrBtGattReadAggregateFormatReqSend
   CsrBtGattReadAggregateFormatLocalReqSend
   CsrBtGattReadProfileDefinedDescriptorReqSend
   CsrBtGattReadProfileDefinedDescriptorReqSend
   CsrBtGattReadProfileDefinedDescriptorLocalReqSend

   These functions Cover part of item 6, Characteristic Value Read,
   and item 10, Characteristic Descriptor Value Read, in the GATT
   feature table, which is defined in The BLUETOOTH SPECIFICATION
   Version 4.0 Vol 3 Part G section 4.2 - Feature Support And
   Procedure Mapping.
   -------------------------------------------------------------------- */
CsrBtGattReadReq *CsrBtGattReadReq_struct(CsrBtGattId     gattId,
                                          CsrBtConnId     btConnId,
                                          CsrBtGattHandle handle,
                                          CsrUint16       offset,
                                          CsrUint16       flags);
/* --------------------------------------------------------------------
   Name:
   CsrBtGattReadReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3.

   Description:
   Request to Read a Characteristic Value or a Long Characteristic Value
   from a server, as defined in [Vol. 3] Part G, section 4.8.1 - Read
   Characteristic Value and section 4.8.3 - Read Long Characteristic Values

   As confirmation the application will receive a CSR_BT_GATT_READ_CFM
   message.

   This function shall only be used if the Characteristic Properties
   'Read' (0x02) bit is enabled. The Characteristic Properties bit
   field is part of the Characteristic Declaration Attribute Value
   which is defined in [Vol. 3] Part G,
   section 3.3.1.1 - Characteristic Properties.

   The application is permitted to cancel this procedure early,
   by calling CsrBtGattCancelReqSend.

   As confirmation the application will still receive the
   CSR_BT_GATT_READ_CFM message.

   Parameters:
   CsrBtGattId _gattId     - Application identifier
   CsrBtConnId _btConnId   - Connection identifier
   CsrBtGattHandle _handle - The Charateristic Value Handle
   CsrUint16 _offset       - The offset of the first octet that shall be read
   -------------------------------------------------------------------- */
#define CsrBtGattReadReqSend(_gattId,_btConnId,_handle,_offset) {       \
        CsrBtGattReadReq *msg__;                                        \
        msg__=CsrBtGattReadReq_struct(_gattId,_btConnId,_handle,_offset,CSR_BT_GATT_READ_VALUE); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattReadLocalReqSend

   Description:
   The only difference re. CsrBtGattReadReqSend is that the Characteristic Value
   or the Long Characteristic Value is read from the local database
   and not from a peer server.

   Parameters:
   CsrBtGattId _gattId     - Application identifier
   CsrBtGattHandle _handle - The Charateristic Value Handle
   -------------------------------------------------------------------- */
#define CsrBtGattReadLocalReqSend(_gattId,_handle) {                    \
        CsrBtGattReadReqSend(_gattId,CSR_BT_GATT_LOCAL_BT_CONN_ID,_handle,0);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattReadExtendedPropertiesReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3.

   Description:
   Request to Read a Characteristic Extended Properties declaration,
   by using the procedure defined in [Vol. 3] Part G,
   section 4.12.1 - Read Characteristic Descriptors

   As confirmation the application will receive a
   CSR_BT_GATT_READ_EXTENDED_PROPERTIES_CFM message.

   The application is permitted to cancel this procedure early,
   by calling CsrBtGattCancelReqSend.

   As confirmation the application will still receive the
   CSR_BT_GATT_READ_EXTENDED_PROPERTIES_CFM message.

   Parameters:
   CsrBtGattId _gattId     - Application identifier
   CsrBtConnId _btConnId   - Connection identifier
   CsrBtGattHandle _handle - The handle of the Characteristic Extended
   Properties declaration
   -------------------------------------------------------------------- */
#define CsrBtGattReadExtendedPropertiesReqSend(_gattId,_btConnId,_handle) { \
        CsrBtGattReadReq *msg__;                                        \
        msg__=CsrBtGattReadReq_struct(_gattId,_btConnId,_handle,0,CSR_BT_GATT_READ_EXT_PROPERTIES); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattReadExtendedPropertiesLocalReqSend

   Description:
   The only difference re. CsrBtGattReadExtendedPropertiesReqSend is that
   the Characteristic Extended Properties declaration is read from
   the local database and not from a peer server.

   Parameters:
   CsrBtGattId _gattId     - Application identifier
   CsrBtGattHandle _handle - The handle of the Characteristic Extended
   Properties declaration
   -------------------------------------------------------------------- */
#define CsrBtGattReadExtendedPropertiesLocalReqSend(_gattId,_handle) {  \
        CsrBtGattReadExtendedPropertiesReqSend(_gattId,CSR_BT_GATT_LOCAL_BT_CONN_ID,_handle);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattReadUserDescriptionReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3.

   Description:
   Request to Read a Characteristic User Description declaration,
   by using the procedures defined in [Vol. 3] Part G,
   section 4.12.1 - Read Characteristic Descriptors and
   section 4.12.2 - Read Long Characteristic Descriptors.

   As confirmation the application will received a
   CSR_BT_GATT_READ_USER_DESCRIPTION_CFM message.

   The application is permitted to cancel this procedure early,
   by calling CsrBtGattCancelReqSend.

   As confirmation the application will still receive the
   CSR_BT_GATT_READ_USER_DESCRIPTION_CFM message.

   Parameters:
   CsrBtGattId _gattId     - Application identifier
   CsrBtConnId _btConnId   - Connection identifier
   CsrBtGattHandle _handle - The handle of the Characteristic User
   Description declaration
   -------------------------------------------------------------------- */
#define CsrBtGattReadUserDescriptionReqSend(_gattId,_btConnId,_handle) { \
        CsrBtGattReadReq *msg__;                                        \
        msg__=CsrBtGattReadReq_struct(_gattId,_btConnId,_handle,0,      \
                                      CSR_BT_GATT_READ_USER_DESCRIPTOR); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattReadUserDescriptionLocalReqSend

   Description:
   The only difference re. CsrBtGattReadUserDescriptionReqSend is that
   the Characteristic User Description declaration is read from
   the local database and not from a peer server.

   Parameters:
   CsrBtGattId _gattId     - Application identifier
   CsrBtGattHandle _handle - The handle of the Characteristic User
   Description declaration
   -------------------------------------------------------------------- */
#define CsrBtGattReadUserDescriptionLocalReqSend(_gattId,_handle) {     \
        CsrBtGattReadUserDescriptionReqSend(_gattId,CSR_BT_GATT_LOCAL_BT_CONN_ID,_handle);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattReadClientConfigurationReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3.

   Description:
   Request to Read a Client Characteristic Configuration
   declaration, by using the procedures defined in [Vol. 3]
   Part G, section 4.12.1 - Read Characteristic Descriptors.

   As confirmation the application will receive a
   CSR_BT_GATT_READ_CLIENT_CONFIGURATION_CFM message.

   The application is permitted to cancel this procedure early,
   by calling CsrBtGattCancelReqSend.

   As confirmation the application will still receive the
   CSR_BT_GATT_READ_CLIENT_CONFIGURATION_CFM message.

   Parameters:
   CsrBtGattId _gattId     - Application identifier
   CsrBtConnId _btConnId   - Connection identifier
   CsrBtGattHandle _handle - The handle of the Client Characteristic
   Configuration declaration
   -------------------------------------------------------------------- */
#define CsrBtGattReadClientConfigurationReqSend(_gattId,_btConnId,_handle) { \
        CsrBtGattReadReq *msg__;                                        \
        msg__=CsrBtGattReadReq_struct(_gattId,_btConnId,_handle,0,      \
                                      CSR_BT_GATT_READ_CLIENT_CONFIGURATION); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattReadServerConfigurationReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3.

   Description:
   Request to Read a Server Characteristic Configuration declaration,
   by using the procedures defined in [Vol. 3] Part G,
   section 4.12.1 - Read Characteristic Descriptors.

   As confirmation the application will receive a
   CSR_BT_GATT_READ_SERVER_CONFIGURATION_CFM message.

   The application is permitted to cancel this procedure early,
   by calling CsrBtGattCancelReqSend.

   As confirmation the application will still receive the
   CSR_BT_GATT_READ_SERVER_CONFIGURATION_CFM message.

   Parameters:
   CsrBtGattId _gattId     - Application identifier
   CsrBtConnId _btConnId   - Connection identifier
   CsrBtGattHandle _handle - The handle of the Server Characteristic
   Configuration declaration
   -------------------------------------------------------------------- */
#define CsrBtGattReadServerConfigurationReqSend(_gattId,_btConnId,_handle) { \
        CsrBtGattReadReq *msg__;                                        \
        msg__=CsrBtGattReadReq_struct(_gattId,_btConnId,_handle,0,      \
                                      CSR_BT_GATT_READ_SERVER_CONFIGURATION); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattReadServerConfigurationLocalReqSend

   Description:
   The only difference re. CsrBtGattReadServerConfigurationReqSend is that
   the Server Characteristic Configuration declaration is read from
   the local database and not from a peer server.

   Parameters:
   CsrBtGattId _gattId     - Application identifier
   CsrBtGattHandle _handle - The handle of the Server Characteristic
   Configuration declaration
   -------------------------------------------------------------------- */
#define CsrBtGattReadServerConfigurationLocalReqSend(_gattId,_handle) { \
        CsrBtGattReadServerConfigurationReqSend(_gattId,CSR_BT_GATT_LOCAL_BT_CONN_ID,_handle);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattReadPresentationFormatReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3.

   Description:
   Request to Read a Characteristic Presentation Format declaration,
   by using the procedures defined in [Vol. 3] Part G,
   section 4.12.1 - Read Characteristic Descriptors.

   As confirmation the application will receive a
   CSR_BT_GATT_READ_PRESENTATION_FORMAT_CFM message.

   The application is permitted to cancel this procedure early,
   by calling CsrBtGattCancelReqSend.

   As confirmation the application will still receive the
   CSR_BT_GATT_READ_PRESENTATION_FORMAT_CFM message.

   Parameters:
   CsrBtGattId _gattId     - Application identifier
   CsrBtConnId _btConnId   - Connection identifier
   CsrBtGattHandle _handle - The handle of the Characteristic
   Presentation Format declaration
   -------------------------------------------------------------------- */
#define CsrBtGattReadPresentationFormatReqSend(_gattId,_btConnId,_handle) { \
        CsrBtGattReadReq *msg__;                                        \
        msg__=CsrBtGattReadReq_struct(_gattId,_btConnId,_handle,0,      \
                                      CSR_BT_GATT_READ_PRESENTATION_FORMAT); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattReadPresentationFormatLocalReqSend

   Description:
   The only diffence re. CsrBtGattReadPresentationFormatReqSend is that
   the Characteristic Presentation Format declaration is read from
   the local database and not from a peer server.

   Parameters:
   CsrBtGattId _gattId     - Application identifier
   CsrBtGattHandle _handle - The handle of the Characteristic
   Presentation Format declaration
   -------------------------------------------------------------------- */
#define CsrBtGattReadPresentationFormatLocalReqSend(_gattId,_handle) {  \
        CsrBtGattReadPresentationFormatReqSend(_gattId,CSR_BT_GATT_LOCAL_BT_CONN_ID,_handle);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattReadAggregateFormatReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3.

   Description:
   Request to Read a Characteristic Aggregate Format declaration,
   by using the procedures defined in [Vol. 3] Part G,
   section 4.12.1 - Read Characteristic Descriptors and
   section 4.12.2 - Read Long Characteristic Descriptors.

   As confirmation the application will receive a
   CSR_BT_GATT_READ_AGGREGATE_FORMAT_CFM message.

   The application is permitted to cancel this procedure early,
   by calling CsrBtGattCancelReqSend.

   As confirmation the application will still receive the
   CSR_BT_GATT_READ_AGGREGATE_FORMAT_CFM message.

   Parameters:
   CsrBtGattId _gattId     - Application identifier
   CsrBtConnId _btConnId   - Connection identifier
   CsrBtGattHandle _handle - The handle of the Characteristic
   Aggregate Format declaration
   -------------------------------------------------------------------- */
#define CsrBtGattReadAggregateFormatReqSend(_gattId,_btConnId,_handle) { \
        CsrBtGattReadReq *msg__;                                        \
        msg__=CsrBtGattReadReq_struct(_gattId,_btConnId,_handle,0,      \
                                      CSR_BT_GATT_READ_AGGREGATE_FORMAT); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattReadAggregateFormatLocalReqSend

   Description:
   The only differnce re. CsrBtGattReadAggregateFormatReqSend is that
   the Characteristic Aggregate Format declaration is read from
   the local database and not from a peer server.

   Parameters:
   CsrBtGattId _gattId     - Application identifier
   CsrBtGattHandle _handle - The handle of the Characteristic
   Aggregate Format declaration
   -------------------------------------------------------------------- */
#define CsrBtGattReadAggregateFormatLocalReqSend(_gattId,_handle) {     \
        CsrBtGattReadAggregateFormatReqSend(_gattId,CSR_BT_GATT_LOCAL_BT_CONN_ID,_handle);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattReadProfileDefinedDescriptorReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3.

   Description:
   Request to Read a Characteristic Descriptor that is defined
   by a higher layer Profile. E.g. a Characteristic Descriptor
   that is not part of the standard ones defined in [Vol. 3]
   Part G, section 3.3.3 - Characteristic Descriptor Declarations.

   To Read such a Descriptor GATT is using the procedures defined
   in [Vol. 3] Part G,
   section 4.12.1 - Read Characteristic Descriptors and
   section 4.12.2 - Read Long Characteristic Descriptors.

   As confirmation the application will receive a
   CSR_BT_GATT_READ_PROFILE_DEFINED_DESCRIPTOR_CFM message.

   The application is permitted to cancel this procedure early,
   by calling CsrBtGattCancelReqSend.

   As confirmation the application will still receive the
   CSR_BT_GATT_READ_PROFILE_DEFINED_DESCRIPTOR_CFM message.

   Parameters:
   CsrBtGattId _gattId     - Application identifier
   CsrBtConnId _btConnId   - Connection identifier
   CsrBtGattHandle _handle - The handle of the Profile Defined
   Description declaration
   CsrUint16 _offset       - The offset of the first octet that shall be read
   -------------------------------------------------------------------- */
#define CsrBtGattReadProfileDefinedDescriptorReqSend(_gattId,_btConnId,_handle,_offset) { \
        CsrBtGattReadReq *msg__;                                        \
        msg__=CsrBtGattReadReq_struct(_gattId,_btConnId,_handle,_offset, \
                                      CSR_BT_GATT_READ_PROFILE_DEFINED); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattReadProfileDefinedDescriptorLocalReqSend

   Description:
   The only difference re. CsrBtGattReadProfileDefinedDescriptorReqSend
   is that the Characteristic Descriptor which is defined by a
   higher layer Profile is read from the local database and not
   from a peer server.

   Parameters:
   CsrBtGattId _gattId     - Application identifier
   CsrBtGattHandle _handle - The handle of the Profile Defined
   Description declaration
   -------------------------------------------------------------------- */
#define CsrBtGattReadProfileDefinedDescriptorLocalReqSend(_gattId,_handle) { \
        CsrBtGattReadProfileDefinedDescriptorReqSend(_gattId,CSR_BT_GATT_LOCAL_BT_CONN_ID, \
                                                     _handle,0);}

/* --------------------------------------------------------------------
   Common structure for the CsrBtGattReadByUuidReq
   primitive which is used by the functions:
   CsrBtGattReadBy16BitUuidReqSend
   CsrBtGattReadBy16BitUuidLocalReqSend
   CsrBtGattReadBy128BitUuidReqSend
   CsrBtGattReadBy128BitUuidLocalReqSend

   These functions Cover part of item 6, Characteristic Value Read,
   in the GATT feature table, which is defined in The BLUETOOTH
   SPECIFICATION Version 4.0 Vol 3 Part G section 4.2 -
   Feature Support And Procedure Mapping.
   -------------------------------------------------------------------- */
CsrBtGattReadByUuidReq *CsrBtGattReadByUuidReq_struct(CsrBtGattId     gattId,
                                                      CsrBtConnId     btConnId,
                                                      CsrBtGattHandle startHandle,
                                                      CsrBtGattHandle endGroupHandle,
                                                      CsrBtUuid       uuid);

/* --------------------------------------------------------------------
   Name:
   CsrBtGattReadBy16BitUuidReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3.

   Description:
   Request to Read a Characteristic Value from a server when
   the client only knows the 16-bit characteristic UUID and
   does not know the handle of the characteristic. This
   procedure is defined in [Vol. 3] Part G, section 4.8.2 -
   Read Using Characteristic UUID

   Every time a Characteristic Value has been read the
   CSR_BT_GATT_READ_BY_UUID_IND message is sent to the
   application. This procedure is complete when a
   CSR_BT_GATT_READ_BY_UUID_CFM message is received

   This function shall only be used if the Characteristic Properties
   'Read' (0x02) bit is enabled. The Characteristic Properties bit
   field is part of the Characteristic Declaration Attribute Value
   which is defined in [Vol. 3] Part G,
   section 3.3.1.1 - Characteristic Properties.

   The application is permitted to cancel this procedure early
   if a desired Characteristic Value has been read prior to
   reading all the characteristic values. This can is done
   by calling CsrBtGattCancelReqSend.

   As confirmation the application will still receive the
   CSR_BT_GATT_READ_BY_UUID_CFM message.

   Parameters:
   CsrBtGattId _gattId          - Application identifier
   CsrBtConnId _btConnId        - Connection identifier
   CsrBtGattHandle _startHandle - The Start Handle
   CsrBtGattHandle _endHandle   - The End Handle.
   Note, the Start and End Handle
   parameters shall be set to the
   range over which this read
   is to be performed. This is
   typically the handle range for
   the service in which the
   characteristic belongs.
   CsrBtUuid16 _uuid16          - The known 16-bit characteristic UUID
   -------------------------------------------------------------------- */
#define CsrBtGattReadBy16BitUuidReqSend(_gattId,_btConnId,_startHandle,_endHandle,_uuid16) { \
        CsrBtGattReadByUuidReq *msg__;                                  \
        CsrBtUuid _uuid;                                                \
        CsrMemSet(_uuid.uuid, 0, CSR_BT_UUID128_SIZE);                  \
        _uuid.length = (CsrUint16) (sizeof(CsrBtUuid16));               \
        CSR_COPY_UINT16_TO_LITTLE_ENDIAN(_uuid16,_uuid.uuid);           \
        msg__=CsrBtGattReadByUuidReq_struct(_gattId,_btConnId,_startHandle,_endHandle,_uuid); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattReadBy16BitUuidLocalReqSend

   Description:
   The only difference re. CsrBtGattReadBy16BitUuidReqSend is that the
   Characteristic Value is read from the local database
   and not from a peer server.

   Parameters:
   CsrBtGattId _gattId          - Application identifier
   CsrBtGattHandle _startHandle - The Start Handle
   CsrBtGattHandle _endHandle   - The End Handle.
   Note, the Start and End Handle
   parameters shall be set to the
   range over which this read
   is to be performed. This is
   typically the handle range for
   the service in which the
   characteristic belongs.
   CsrBtUuid16 _uuid16          - The known 16-bit characteristic UUID
   -------------------------------------------------------------------- */
#define CsrBtGattReadBy16BitUuidLocalReqSend(_gattId,_startHandle,_endHandle,_uuid16) { \
        CsrBtGattReadBy16BitUuidReqSend(_gattId,CSR_BT_GATT_LOCAL_BT_CONN_ID,_startHandle, \
                                        _endHandle,_uuid16);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattReadBy128BitUuidReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3.

   Description:
   Request to Read a Characteristic Value from a server when
   the client only knows the 128-bit characteristic UUID and
   does not know the handle of the characteristic. This
   procedure is defined in [Vol. 3] Part G, section 4.8.2 -
   Read Using Characteristic UUID

   Every time a Characteristic Value has been read a
   CSR_BT_GATT_READ_BY_UUID_IND message is sent to the
   application. This procedure is complete when a
   CSR_BT_GATT_READ_BY_UUID_CFM message is received

   This function shall only be used if the Characteristic Properties
   'Read' (0x02) bit is enabled. The Characteristic Properties bit
   field is part of the Characteristic Declaration Attribute Value
   which is defined in [Vol. 3] Part G,
   section 3.3.1.1 - Characteristic Properties.

   The application is permitted to cancel this procedure early
   if a desired Characteristic Value has been read prior to
   reading all the characteristic values. This can is done
   by calling CsrBtGattCancelReqSend.

   As confirmation the application will still receive the
   CSR_BT_GATT_READ_BY_UUID_CFM message.

   Parameters:
   CsrBtGattId _gattId          - Application identifier
   CsrBtConnId _btConnId        - Connection identifier
   CsrBtGattHandle _startHandle - The Start Handle
   CsrBtGattHandle _endHandle   - The End Handle.
   Note, the Start and End Handle
   parameters shall be set to the
   range over which this read
   is to be performed. This is
   typically the handle range for
   the service in which the
   characteristic belongs.
   CsrBtUuid128 _uuid128        - The known 128-bit characteristic UUID
   -------------------------------------------------------------------- */
#define CsrBtGattReadBy128BitUuidReqSend(_gattId,_btConnId,_startHandle, \
                                         _endHandle,_uuid128) {         \
        CsrBtGattReadByUuidReq *msg__;                                  \
        CsrBtUuid _uuid;                                                \
        _uuid.length = (CsrUint16) (sizeof(CsrBtUuid128));              \
        CsrMemCpy(_uuid.uuid,_uuid128,CSR_BT_UUID128_SIZE);             \
        msg__=CsrBtGattReadByUuidReq_struct(_gattId,_btConnId,_startHandle,_endHandle,_uuid); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattReadBy128BitUuidLocalReqSend

   Description:
   As CsrBtGattReadBy128BitUuidReqSend only difference is that the
   Characteristic Value is read from the local database
   and not from a peer server.

   Parameters:
   CsrBtGattId _gattId          - Application identifier
   CsrBtGattHandle _startHandle - The Start Handle
   CsrBtGattHandle _endHandle   - The End Handle.
   Note, the Start and End Handle
   parameters shall be set to the
   range over which this read
   is to be performed. This is
   typically the handle range for
   the service in which the
   characteristic belongs.
   CsrBtUuid128 _uuid128        - The known 128-bit characteristic UUID
   -------------------------------------------------------------------- */
#define CsrBtGattReadBy128BitUuidLocalReqSend(_gattId,_startHandle,_endHandle,_uuid128) { \
        CsrBtGattReadBy128BitUuidReqSend(_gattId,CSR_BT_GATT_LOCAL_BT_CONN_ID,_startHandle, \
                                         _endHandle,_uuid128);}

/* --------------------------------------------------------------------
   Common structure for the CsrBtGattReadMultiReq
   primitive which is used by the functions:
   CsrBtGattReadMultiReqSend
   CsrBtGattReadMultiLocalReqSend

   These functions cover part of item 6, Characteristic Value Read,
   in the GATT feature table, which is defined in The BLUETOOTH
   SPECIFICATION Version 4.0 Vol 3 Part G section 4.2 -
   Feature Support And Procedure Mapping.
   -------------------------------------------------------------------- */
CsrBtGattReadMultiReq *CsrBtGattReadMultiReq_struct(CsrBtGattId gattId,
                                                    CsrBtConnId btConnId,
                                                    CsrUint16 handlesCount,
                                                    CsrBtGattHandle *handles);

/* --------------------------------------------------------------------
   Name:
   CsrBtGattReadMultiReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3.

   Description:
   Request to Read multiple Characteristic Values from a server, as
   defined in [Vol. 3] Part G, section 4.8.4 Read Multiple
   Characteristic Values

   As confirmation the application will receive a
   CSR_BT_GATT_READ_MULTI_CFM message.

   This function shall only be used if the Characteristic Properties
   'Read' (0x02) bit is enabled. The Characteristic Properties bit
   field is part of the Characteristic Declaration Attribute Value
   which is defined in [Vol. 3] Part G,
   section 3.3.1.1 - Characteristic Properties.

   Note: A client should not request to read multiple Characteristic
   Values when the Set Of Values parameter of the response is equal to
   (mtu - 1) octets in length, since it is not possible to determine
   if the last Characteristic Value was read or additional
   Characteristic Values exist but were truncated.

   The application is permitted to cancel this procedure early,
   by calling CsrBtGattCancelReqSend.

   As confirmation the application will still receive the
   CSR_BT_GATT_READ_MULTI_CFM message.

   Parameters:
   CsrBtGattId _gattId       - Application identifier
   CsrBtConnId _btConnId     - Connection identifier
   CsrUint16 _numOfHandles   - Number of attribute Handles that must be read
   CsrBtGattHandle *_handles - An allocated pointer of two or more attribute
   handles
   -------------------------------------------------------------------- */
#define CsrBtGattReadMultiReqSend(_gattId,_btConnId,_numOfHandles,_handles) { \
        CsrBtGattReadMultiReq *msg__;                                   \
        msg__=CsrBtGattReadMultiReq_struct(_gattId,_btConnId,_numOfHandles,_handles); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattReadMultiLocalReqSend

   Description:
   The only difference re. CsrBtGattReadMultiReqSend is that the multiple
   Characteristic Values are read from the local database and not
   from a peer server.

   Parameters:
   CsrBtGattId _gattId       - Application identifier
   CsrUint16 _numOfHandles   - Number of attribute handles that must be read
   CsrBtGattHandle *_handles - An allocated pointer of two or more attribute
   handles
   -------------------------------------------------------------------- */
#define CsrBtGattReadMultiLocalReqSend(_gattId,_numOfHandles,_handles) { \
        CsrBtGattReadMultiReqSend(_gattId,CSR_BT_GATT_LOCAL_BT_CONN_ID,_numOfHandles,_handles);}

/* --------------------------------------------------------------------
   Common structure for the CsrBtGattWriteReq primitive which
   is used by the functions:
   CsrBtGattWriteCmdReqSend
   CsrBtGattWriteReqSend
   CsrBtGattWriteLocalReqSend
   CsrBtGattWriteSignedCmdReqSend
   CsrBtGattReliableWritesReqSend
   CsrBtGattWriteUserDescriptionReqSend
   CsrBtGattWriteUserDescriptionLocalReqSend
   CsrBtGattWriteServerConfigurationReqSend
   CsrBtGattWriteServerConfigurationLocalReqSend
   CsrBtGattWriteProfileDefinedDescriptorReqSend
   CsrBtGattWriteProfileDefinedDescriptorLocalReqSend

   These functions covers item 7, Characteristic Value Write,
   and part of item 11, Characteristic Descriptor Value Write,
   in the GATT feature table, which is defined in The BLUETOOTH
   SPECIFICATION Version 4.0 Vol 3 Part G section 4.2 -
   Feature Support And Procedure Mapping.
   -------------------------------------------------------------------- */
CsrBtGattWriteReq *CsrBtGattWriteReq_struct(CsrBtGattId             gattId,
                                            CsrBtConnId             btConnId,
                                            CsrUint16               flags,
                                            CsrUint16               attrWritePairsCount,
                                            CsrBtGattAttrWritePairs *attrWritePairs);

#define csrBtGattCommonWriteReqSend(_gattId,_btConnId,_handle,_offset,_valueLength,_value,_flags) { \
        CsrBtGattWriteReq *msg__;                                       \
        CsrBtGattAttrWritePairs *_ap;                                   \
        _ap = (CsrBtGattAttrWritePairs *) CsrPmemAlloc(sizeof(CsrBtGattAttrWritePairs)); \
        _ap->attrHandle  = _handle;                                     \
        _ap->offset      = _offset;                                     \
        _ap->valueLength = _valueLength;                                \
        _ap->value       = _value;                                      \
        msg__=CsrBtGattWriteReq_struct(_gattId,_btConnId,_flags,1,_ap); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattWriteCmdReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3.

   Description:
   Request to Write a Characteristic Value to a server without any
   response, as defined in [Vol. 3] Part G, section 4.9.1 -
   Write Without Response.

   As confirmation the application will receive a CSR_BT_GATT_WRITE_CFM
   message.

   This function shall only be used if the Characteristic Properties
   -Write Without Response- (0x04) bit is enabled. The Characteristic
   Properties bit field is part of the Characteristic Declaration
   Attribute Value which is defined in [Vol. 3] Part G,
   section 3.3.1.1 - Characteristic Properties

   Note this sub-procedure cannot be used to write a long characteristic
   value, instead CsrBtGattWriteReqSend should be used.

   Parameters:
   CsrBtGattId _gattId     - Application identifier
   CsrBtConnId _btConnId   - Connection identifier
   CsrBtGattHandle _handle - The Charateristic Value Handle
   CsrUint16 _valueLength  - Length of the attribute value that must be written
   CsrUint8 *_value        - An allocated pointer of the attribute value
   -------------------------------------------------------------------- */
#define CsrBtGattWriteCmdReqSend(_gattId,_btConnId,_handle,_valueLength,_value) { \
        csrBtGattCommonWriteReqSend(_gattId,_btConnId,_handle,0,_valueLength, \
                                    _value,CSR_BT_GATT_WRITE_COMMAND);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattWriteReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3.

   Description:
   Request to Write a Characteristic Value or a Long Characteristic Value
   to a server, as defined in [Vol. 3] Part G, section 4.9.3 - Write
   Characteristic Value and section 4.9.4 - Write Long Characteristic Values

   As confirmation the application will receive a CSR_BT_GATT_WRITE_CFM
   message.

   This function shall only be used if the Characteristic Properties
   'Write' (0x08) bit is enabled. The Characteristic Properties bit
   field is part of the Characteristic Declaration Attribute Value
   which is defined in [Vol. 3] Part G,
   section 3.3.1.1 - Characteristic Properties.

   The application is permitted to cancel this procedure early,
   by calling CsrBtGattCancelReqSend. GATT will however only be able
   to cancel this procedure if a long characteristic value is being
   written.

   As confirmation the application will still receive the
   CSR_BT_GATT_WRITE_CFM message.

   Parameters:
   CsrBtGattId _gattId     - Application identifier
   CsrBtConnId _btConnId   - Connection identifier
   CsrBtGattHandle _handle - The Charateristic Value Handle
   CsrUint16 _offset       - The offset of the first octet to be written
   CsrUint16 _valueLength  - Length of the attribute value that must be written
   CsrUint8 *_value        - An allocated pointer of the attribute value
   -------------------------------------------------------------------- */
#define CsrBtGattWriteReqSend(_gattId,_btConnId,_handle,_offset,_valueLength,_value) { \
        csrBtGattCommonWriteReqSend(_gattId,_btConnId,_handle,_offset,  \
                                    _valueLength,_value,CSR_BT_GATT_WRITE_REQUEST);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattWriteLocalReqSend

   Description:
   Request to Write/Update a Characteristic Value or a Long Characteristic
   Value from the local database.

   As confirmation the application will receive a CSR_BT_GATT_WRITE_CFM
   message.

   Parameters:
   CsrBtGattId _gattId     - Application identifier
   CsrBtGattHandle _handle - The local Charateristic Value Handle
   CsrUint16 _valueLength  - Length of the local attribute value that must be written
   CsrUint8 *_value        - An allocated pointer of the local attribute value
   -------------------------------------------------------------------- */
#define CsrBtGattWriteLocalReqSend(_gattId,_handle,_valueLength,_value) { \
        csrBtGattCommonWriteReqSend(_gattId,CSR_BT_GATT_LOCAL_BT_CONN_ID,_handle,0, \
                                    _valueLength,_value,CSR_BT_GATT_WRITE_REQUEST);}


/* --------------------------------------------------------------------
   Name:
   CsrBtGattWriteLocalPreferredConnParametersReqSend

   Description:
   Request to Write/Update the Preferred Connection Parameter Characteristic Value
   from the local database.

   As confirmation the application will receive a CSR_BT_GATT_WRITE_CFM
   message.

   Parameters:
   CsrBtGattId _gattId           - Application identifier
   CsrUint8 *_value              - An allocated pointer of the local attribute value
   CsrUint16 *_connIntervalMin    - Connection Interval Minimum value
   CsrUint16 *_connIntervalMax    - Connection Interval Maximum value
   CsrUint16 *_connLatency        - Connection latency value
   CsrUint16 *_supervisionTimeout - supervision Timeout value
   -------------------------------------------------------------------- */
#define CsrBtGattWriteLocalPreferredConnParametersReqSend(_gattId, _connIntervalMin, _connIntervalMax, _connLatency, _supervisionTimeout) { \
        CsrUint8 *peripheralPreferredConnectionValue = CsrPmemZalloc(8); \
        CSR_COPY_UINT16_TO_LITTLE_ENDIAN(_connIntervalMin, &peripheralPreferredConnectionValue[0]); \
        CSR_COPY_UINT16_TO_LITTLE_ENDIAN(_connIntervalMax, &peripheralPreferredConnectionValue[2]); \
        CSR_COPY_UINT16_TO_LITTLE_ENDIAN(_connLatency, &peripheralPreferredConnectionValue[4]); \
        CSR_COPY_UINT16_TO_LITTLE_ENDIAN(_supervisionTimeout, &peripheralPreferredConnectionValue[6]); \
        csrBtGattCommonWriteReqSend(_gattId,CSR_BT_GATT_LOCAL_BT_CONN_ID,CSR_BT_GATT_ATTR_HANDLE_CONNECTION_PARAMS,0, \
                                    8,peripheralPreferredConnectionValue ,CSR_BT_GATT_WRITE_REQUEST);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattWriteSignedCmdReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3.

   Description:
   Request to Write a Signed Characteristic Value to a server without any
   response, as defined in [Vol. 3] Part G, section 4.9.2 - Signed Write
   Without Response.

   As confirmation the application will receive a CSR_BT_GATT_WRITE_CFM
   message.

   This function shall only be used if the Characteristic Properties
   'Authenticated Signed Writes' (0x40) bit is enabled and the client
   and server device share a bond as defined in [Vol. 3] Part C,
   Generic Access Profile. The Characteristic Properties bit field is
   part of the Characteristic Declaration Attribute Value which is defined
   in [Vol. 3] Part G, section 3.3.1.1 - Characteristic Properties.

   Note this sub-procedure cannot be used to write a long Attribute.

   Parameters:
   CsrBtGattId _gattId     - Application identifier
   CsrBtConnId _btConnId   - Connection identifier
   CsrBtGattHandle _handle - The Charateristic Value Handle
   CsrUint16 _valueLength  - Length of the attribute value that must be written
   CsrUint8 *_value        - An allocated pointer of the attribute value
   -------------------------------------------------------------------- */
#define CsrBtGattWriteSignedCmdReqSend(_gattId,_btConnId,_handle,_valueLength,_value) { \
        csrBtGattCommonWriteReqSend(_gattId,_btConnId,_handle,0,_valueLength,_value, \
                                    CSR_BT_GATT_WRITE_SIGNED_COMMAND);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattReliableWritesReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3.

   Description:
   Request to Write a Characteristic Value to a server with assurance,
   as defined in [Vol. 3] Part G, section 4.9.5 - Reliable Writes.

   Note this Request can also be used when multiple values must be written,
   in order and in a single operation.

   As confirmation the application will receive a CSR_BT_GATT_WRITE_CFM
   message.

   This function shall only be used if the Characteristic Extended
   Properties 'Reliable Write' (0x0001) bit is enabled.
   The Characteristic Extended Properties bit field is part of the
   Characteristic Extended Properties declaration Attribute Value
   which is defined in [Vol. 3] Part G, section 3.3.3.1 -
   Characteristic Extended Properties.

   The application is permitted to cancel this procedure early
   by calling CsrBtGattCancelReqSend.

   As confirmation the application will still receive the
   CSR_BT_GATT_WRITE_CFM message.

   Parameters:
   CsrBtGattId _gattId                      - Application identifier
   CsrBtConnId _btConnId                    - Connection identifier
   CsrUint16 _attrWritePairsCount           - Number of Charateristic
   Attributes Values to be
   written
   CsrBtGattAttrWritePairs *_attrWritePairs - An allocated list of
   Charateristic Attribute
   Value Handles, offsets
   and Attribute Values.
   -------------------------------------------------------------------- */
#define CsrBtGattReliableWritesReqSend(_gattId,_btConnId,               \
                                       _attrWritePairsCount,_attrWritePairs) { \
        CsrBtGattWriteReq *msg__;                                       \
        msg__=CsrBtGattWriteReq_struct(_gattId,_btConnId,CSR_BT_GATT_WRITE_RELIABLE, \
                                       _attrWritePairsCount,_attrWritePairs); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattWriteUserDescriptionReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3.

   Description:
   Request to Write a Characteristic User Description Declaration
   Attribute Value to a server, as defined in [Vol. 3] Part G,
   section 4.12.3 - Write Characteristic Descriptors and
   section 4.12.4 - Write Long Characteristic Descriptors.

   As confirmation the application will receive a CSR_BT_GATT_WRITE_CFM
   message.

   This function shall only be used if the Characteristic Extended
   Properties 'Writable Auxiliaries' (0x0002) bit is enabled.
   The Characteristic Extended Properties bit field is part of the
   Characteristic Extended Properties declaration Attribute Value
   which is defined in [Vol. 3] Part G, section 3.3.3.1 -
   Characteristic Extended Properties.

   The application is permitted to cancel this procedure early
   by calling CsrBtGattCancelReqSend.

   As confirmation the application will still receive the
   CSR_BT_GATT_WRITE_CFM message.

   Parameters:
   CsrBtGattId _gattId        - Application identifier
   CsrBtConnId _btConnId      - Connection identifier
   CsrBtGattHandle _handle    - The handle of the Characteristic User
   Description declaration
   CsrUtf8String *_utf8String - An allocated pointer of the characteristic
   user description UTF-8 string
   -------------------------------------------------------------------- */
#define CsrBtGattWriteUserDescriptionReqSend(_gattId,_btConnId,_handle,_utf8String) { \
        CsrUint16 _utf8StringLength = (CsrUint16)(CsrUtf8StringLengthInBytes(_utf8String) + 1); \
        csrBtGattCommonWriteReqSend(_gattId,_btConnId,_handle,0,_utf8StringLength, \
                                    _utf8String,CSR_BT_GATT_WRITE_REQUEST);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattWriteUserDescriptionLocalReqSend

   Description:
   Request to Write/Update a Characteristic User Description Declaration
   Attribute Value from the local database.

   As confirmation the application will receive a CSR_BT_GATT_WRITE_CFM
   message.

   Parameters:
   CsrBtGattId _gattId         - Application identifier
   CsrBtGattHandle _handle     - The handle of the Characteristic User
   Description declaration
   CsrUtf8String *_utf8String  - An allocated pointer of the characteristic
   user description UTF-8 string
   -------------------------------------------------------------------- */
#define CsrBtGattWriteUserDescriptionLocalReqSend(_gattId,_handle,_utf8String) { \
        CsrBtGattWriteUserDescriptionReqSend(_gattId,CSR_BT_GATT_LOCAL_BT_CONN_ID, \
                                             _handle,_utf8String);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattWriteServerConfigurationReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3.

   Description:
   Request to Write a Server Characteristic Configuration declaration
   Attribute Value to a server, as defined in [Vol. 3] Part G,
   section 4.12.3 - Write Characteristic Descriptors.

   As confirmation the application will receive a CSR_BT_GATT_WRITE_CFM
   message.

   This function shall only be used if the Characteristic Properties
   -Broadcast- (0x01) bit is enabled. The Characteristic Properties bit
   field is part of the Characteristic Declaration Attribute Value
   which is defined in [Vol. 3] Part G, section 3.3.1.1 -
   Characteristic Properties.

   Note a client may write this configuration descriptor to control
   the configuration of this characteristic on the server for all
   clients. There is a single instantiation of the Server
   Characteristic Configuration for all clients. Reads of the Server
   Characteristic Configuration shows the configuration all clients
   and writes affect the configuration for all clients.
   Authentication and authorisation may be required by the server
   to write the configuration descriptor.

   Parameters:
   CsrBtGattId _gattId                - Application identifier
   CsrBtConnId _btConnId              - Connection identifier
   CsrBtGattHandle _handle            - The handle of the Server
   Characteristic
   Configuration declaration
   CsrBtGattSrvConfigBits _configBits - The Server Characteristic
   Configuration bit field value:
   CSR_BT_GATT_SERVER_CHARAC_CONFIG_DISABLE -
   Disable Broadcast
   CSR_BT_GATT_SERVER_CHARAC_CONFIG_BROADCAST -
   Enable Broadcast
   -------------------------------------------------------------------- */
#define CsrBtGattWriteServerConfigurationReqSend(_gattId,_btConnId,_handle,_configBits) { \
        CsrUint16 _valueLength = (CsrUint16) (sizeof(CsrBtGattSrvConfigBits)); \
        CsrUint8 *_value = (CsrUint8 *) CsrPmemAlloc(_valueLength);     \
        CSR_COPY_UINT16_TO_LITTLE_ENDIAN(_configBits,_value);           \
        csrBtGattCommonWriteReqSend(_gattId,_btConnId,_handle,0,_valueLength, \
                                    _value,CSR_BT_GATT_WRITE_REQUEST);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattWriteServerConfigurationLocalReqSend

   Description:
   Request to Write/Update a Server Characteristic Configuration declaration
   Attribute Value from the local database.

   As confirmation the application will receive a CSR_BT_GATT_WRITE_CFM
   message.

   Parameters:
   CsrBtGattId _gattId                - Application identifier
   CsrBtGattHandle _handle            - The handle of the Server
   Characteristic Configuration
   declaration
   CsrBtGattSrvConfigBits _configBits - The Server Characteristic
   Configuration bit field value:
   CSR_BT_GATT_SERVER_CHARAC_CONFIG_DISABLE -
   Disable Broadcast
   CSR_BT_GATT_SERVER_CHARAC_CONFIG_BROADCAST -
   Enable Broadcast
   -------------------------------------------------------------------- */
#define CsrBtGattWriteServerConfigurationLocalReqSend(_gattId,_handle,_configBits) { \
        CsrBtGattWriteServerConfigurationReqSend(_gattId,CSR_BT_GATT_LOCAL_BT_CONN_ID,_handle,_configBits);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattWriteProfileDefinedDescriptorReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3.

   Description:
   Request to Write a higher layer characteristic descriptor declaration,
   which is not part of the standard ones defined in [Vol. 3] Part G,
   section 3.3.3 - Characteristic Descriptor Declarations.

   As confirmation the application will receive a CSR_BT_GATT_WRITE_CFM
   message.

   The application is permitted to cancel this procedure early
   by calling CsrBtGattCancelReqSend.

   As confirmation the application will still receive the
   CSR_BT_GATT_WRITE_CFM message.


   Parameters:
   CsrBtGattId _gattId     - Application identifier
   CsrBtConnId _btConnId   - Connection identifier
   CsrBtGattHandle _handle - The handle of the Profile Defined
   Description declaration
   CsrUint16 _offset       - The offset of the first octet to be written
   CsrUint16 _valueLength  - Length of the attribute value that must be written
   CsrUint8 *_value        - An allocated pointer of the attribute value
   -------------------------------------------------------------------- */
#define CsrBtGattWriteProfileDefinedDescriptorReqSend(_gattId,_btConnId,_handle,_offset,_valueLength,_value) { \
        CsrBtGattWriteReqSend(_gattId,_btConnId,_handle,_offset,_valueLength,_value);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattWriteProfileDefinedDescriptorLocalReqSend

   Description:
   Request to Write/Update a higher layer characteristic descriptor
   declaration Attribute Value of the local database.

   As confirmation the application will receive a CSR_BT_GATT_WRITE_CFM
   message

   Parameters:
   CsrBtGattId _gattId     - Application identifier.
   CsrBtGattHandle _handle - The handle of the Profile Defined
   Description declaration.
   CsrUint16 _valueLength  - Length of the attribute value that must be written
   CsrUint8 *_value        - An allocated pointer of the attribute value
   -------------------------------------------------------------------- */
#define CsrBtGattWriteProfileDefinedDescriptorLocalReqSend(_gattId,_handle, \
                                                           _valueLength,_value) { \
        CsrBtGattWriteProfileDefinedDescriptorReqSend(_gattId,CSR_BT_GATT_LOCAL_BT_CONN_ID, \
                                                      _handle,0,_valueLength,_value);}


/* --------------------------------------------------------------------
   Common structure for the CsrBtGattWriteClientConfigurationReq
   primitive which is used by the functions:

   CsrBtGattWriteClientConfigurationReqExtSend
   CsrBtGattWriteClientConfigurationReqSend
   CsrBtGattWriteClientConfigurationConnLessReqExtSend
   CsrBtGattWriteClientConfigurationConnLessReqSend

   These functions cover part of item 11, Characteristic
   Descriptor Value Write, in the GATT feature table, which
   is defined in The BLUETOOTH SPECIFICATION Version 4.0
   Vol 3 Part G section 4.2 - Feature Support And Procedure Mapping.
   -------------------------------------------------------------------- */
CsrBtGattWriteClientConfigurationReq *CsrBtGattWriteClientConfigurationReq_struct(CsrBtGattId            gattId,
                                                                                  CsrBtConnId            btConnId,
                                                                                  CsrBtTypedAddr         address,
                                                                                  CsrBtGattHandle        descriptorHandle,
                                                                                  CsrBtGattHandle        valueHandle,
                                                                                  CsrBtGattCliConfigBits configuration,
                                                                                  CsrBool                disableReadBlob);

/* --------------------------------------------------------------------
  Name: 
    CsrBtGattWriteClientConfigurationReqExtSend
    
  Description: 
    Similar to CsrBtGattWriteClientConfigurationReqSend. 
    Only difference is that the application can configure if GATT 
    automatic shall issue an read blob request when receiving 
    full PDU or not. Note per default and when calling 
    CsrBtGattWriteClientConfigurationReqSend the disableReadBlob 
    parameter is set to FALSE.

  Parameters: 
    CsrBtGattId _gattId                - Application identifier
    CsrBtConnId _btConnId              - Connection identifier
    CsrBtGattHandle _cliConfHdl        - The handle of the Client Characteristic 
                                         Configuration declaration
    CsrBtGattHandle _valueHdl          - The characteristic value handle. E.g.
                                         the attribute handle of the Characteristic 
                                         Value Declaration. 
    CsrBtGattCliConfigBits _configBits - The Client Characteristic Configuration 
                                         bit field value:
                                         CSR_BT_GATT_CLIENT_CHARAC_CONFIG_DEFAULT -
                                         The Characteristic Value shall not be 
                                         notified or indicated.
                                         CSR_BT_GATT_CLIENT_CHARAC_CONFIG_NOTIFICATION -
                                         The Characteristic Value shall be notified
                                         CSR_BT_GATT_CLIENT_CHARAC_CONFIG_INDICATION -
                                         The Characteristic Value shall be indicated
    CsrBool _disableReadBlob           - If FALSE, GATT will automatic issue an read 
                                         blob request when receiving full PDU. 
   -------------------------------------------------------------------- */
#define CsrBtGattWriteClientConfigurationReqExtSend(_gattId,_btConnId,_cliConfHdl, \
                                                    _valueHdl,_configBits,_disableReadBlob) { \
        CsrBtGattWriteClientConfigurationReq *msg__; \
        CsrBtTypedAddr _adrzero; CsrBtAddrZero(&(_adrzero)); \
        msg__=CsrBtGattWriteClientConfigurationReq_struct(_gattId,_btConnId,_adrzero, \
                                                          _cliConfHdl,_valueHdl,_configBits,_disableReadBlob); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattWriteClientConfigurationReqSend

   Document Reference:
   [Vol. 3] - The BLUETOOTH SPECIFICATION Version 4.0 Vol 3.

   Description:
   Request to Write a Client Characteristic Configuration declaration
   Attribute Value to a server, as defined in [Vol. 3] Part G,
   section 4.12.3 - Write Characteristic Descriptors.

   As confirmation the application will receive a CSR_BT_GATT_WRITE_CFM
   message.

   This function shall only be used if the Characteristic Properties
   'Notify' (0x10) bit or 'Indicate' (0x20) bit is enabled. The
   Characteristic Properties bit field is part of the Characteristic
   Declaration Attribute Value which is defined in [Vol. 3] Part G,
   section 3.3.1.1 - Characteristic Properties.

   Note a client may write this configuration descriptor to control
   the configuration of this characteristic on the server for the
   client. Each client has its own instantiation of the Client
   Characteristic Configuration. Reads of the Client Characteristic
   Configuration only shows the configuration for that client and
   writes only affect the configuration of that client. Authentication
   and authorisation may be required by the server to write the
   configuration descriptor.

   Parameters:
   CsrBtGattId _gattId                - Application identifier
   CsrBtConnId _btConnId              - Connection identifier
   CsrBtGattHandle _cliConfHdl        - The handle of the Client Characteristic
   Configuration declaration
   CsrBtGattHandle _valueHdl          - The characteristic value handle. E.g.
   the attribute handle of the Characteristic
   Value Declaration.
   CsrBtGattCliConfigBits _configBits - The Client Characteristic Configuration
   bit field value:
   CSR_BT_GATT_CLIENT_CHARAC_CONFIG_DEFAULT -
   The Characteristic Value shall not be
   notified or indicated.
   CSR_BT_GATT_CLIENT_CHARAC_CONFIG_NOTIFICATION -
   The Characteristic Value shall be notified
   CSR_BT_GATT_CLIENT_CHARAC_CONFIG_INDICATION -
   The Characteristic Value shall be indicated
   -------------------------------------------------------------------- */
#define CsrBtGattWriteClientConfigurationReqSend(_gattId,_btConnId,_cliConfHdl, \
                                                 _valueHdl,_configBits) { \
        CsrBtGattWriteClientConfigurationReqExtSend(_gattId,_btConnId,_cliConfHdl, \
                                                    _valueHdl,_configBits,FALSE);}

/* --------------------------------------------------------------------
  Name: 
    CsrBtGattWriteClientConfigurationConnLessReqExtSend
    
  Description: 
    Similar to CsrBtGattWriteClientConfigurationConnLessReqSend. 
    Only difference is that the application can configure if GATT 
    automatic shall issue an read blob request when receiving 
    full PDU or not. Note per default and when calling 
    CsrBtGattWriteClientConfigurationConnLessReqSend the 
    disableReadBlob parameter is set to FALSE.

  Parameters: 
    CsrBtGattId _gattId                - Application identifier.
    CsrBtTypedAddr   _address          - The device address of 
                                         the peer server.
    CsrBtGattHandle _valueHdl          - The characteristic value handle. 
                                         E.g. the attribute handle of 
                                         the Characteristic Value 
                                         Declaration which previously were 
                                         set by using the function
                                         CsrBtGattWriteClientConfigurationReqSend. 
    CsrBtGattCliConfigBits _configBits - The Client Characteristic Configuration 
                                         bit field value. Shall be set to the 
                                         same value as the one that was previously 
                                         set by using the function
                                         CsrBtGattWriteClientConfigurationReqSend. 
    CsrBool _disableReadBlob           - If FALSE, GATT will automatic issue an read 
                                         blob request when receiving full PDU. 
   -------------------------------------------------------------------- */
#define CsrBtGattWriteClientConfigurationConnLessReqExtSend(_gattId,_address, _valueHdl, \
                                                            _configBits,_disableReadBlob) { \
        CsrBtGattWriteClientConfigurationReq *msg__; \
        msg__=CsrBtGattWriteClientConfigurationReq_struct(_gattId,CSR_BT_GATT_LOCAL_BT_CONN_ID, \
                                                          _address,0,_valueHdl,_configBits,_disableReadBlob); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattWriteClientConfigurationConnLessReqSend

   Description:
   In the case that the GATT module has been reset and the Client
   has set the Client Characteristic Configuration declaration
   Attribute Value on a server, which is bonded to
   CSR_BT_GATT_CLIENT_CHARAC_CONFIG_NOTIFICATION or to
   CSR_BT_GATT_CLIENT_CHARAC_CONFIG_INDICATION, the Client shall
   call this function to re-enable the events requested by the
   function CsrBtGattWriteClientConfigurationReqSend. Otherwise
   GATT does not know to which client application the incoming server
   indication or notification message shall be sent.

   As confirmation the application will receive a CSR_BT_GATT_WRITE_CFM
   message.

   Parameters:
   CsrBtGattId _gattId                - Application identifier.
   CsrBtTypedAddr   _address          - The device address of
   the peer server.
   CsrBtGattHandle _valueHdl          - The characteristic value handle.
   E.g. the attribute handle of
   the Characteristic Value
   Declaration which previously were
   set by using the function
   CsrBtGattWriteClientConfigurationReqSend.
   CsrBtGattCliConfigBits _configBits - The Client Characteristic Configuration
   bit field value. Shall be set to the
   same value as the one that was previously
   set by using the function
   CsrBtGattWriteClientConfigurationReqSend.
   -------------------------------------------------------------------- */
#define CsrBtGattWriteClientConfigurationConnLessReqSend(_gattId,_address, \
                                                         _valueHdl,_configBits) { \
        CsrBtGattWriteClientConfigurationConnLessReqExtSend(_gattId,_address, _valueHdl, \
                                                            _configBits,FALSE);}


/* --------------------------------------------------------------------
   Common structure for the CsrBtGattSubscriptionReq
   primitive which is used by the functions:
   CsrBtGattSubscribeReqSend
   CsrBtGattUnsubscribeReqSend
   -------------------------------------------------------------------- */
CsrBtGattSubscriptionReq *CsrBtGattSubscriptionReq_struct(CsrBtGattId     gattId,
                                                          CsrBtTypedAddr  address,
                                                          CsrBtGattHandle valueHandle,
                                                          CsrBool         subscribe,
                                                          CsrBool         disableReadBlob);

/* --------------------------------------------------------------------
  Name: 
    CsrBtGattSubscribeReqExtSend
    
  Description: 
    Similar to CsrBtGattSubscribeReqSend. Only difference is that 
    the application can configure if GATT automatic shall issue 
    an read blob request when receiving full PDU or not. Note 
    per default and when calling CsrBtGattSubscribeReqSend
    the disableReadBlob parameter is set to FALSE.

  Parameters: 
    CsrBtGattId _gattId       - Application identifier
    CsrBtGattAddress _address - The device address of the peer server 
    CsrBtGattHandle _handle   - The handle of the Characteristic Value.
    CsrBool _disableReadBlob  - If FALSE, GATT will automatic issue an 
                                read blob request when receiving full PDU. 
   -------------------------------------------------------------------- */
#define CsrBtGattSubscribeReqExtSend(_gattId,_address,_handle,_disableReadBlob) { \
        CsrBtGattSubscriptionReq *msg__; \
        msg__=CsrBtGattSubscriptionReq_struct(_gattId,_address,_handle,TRUE,_disableReadBlob); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattSubscribeReqSend

   Description:

   Allows the Client application to subscribe for indication or
   notification events from a server when a Client Characteristic
   Configuration declaration do not exist.

   Note, whenever possible is it strongly recommended to use the
   function CsrBtGattWriteClientConfigurationReqSend, as
   CsrBtGattSubscribeReqSend only should be used if a
   Characteristic definition on a peer server does not
   include a Client Characteristic Configuration descriptor.
   Further, the server sends notification or indication event
   every time the Characteristic Value within the Characteristic
   definition has been changed and the application needs these
   events.

   As confirmation the application will receive a
   CSR_BT_GATT_SUBSCRIPTION_CFM message.

   Parameters:
   CsrBtGattId _gattId       - Application identifier
   CsrBtGattAddress _address - The device address of the peer server
   CsrBtGattHandle _handle   - The handle of the Characteristic Value.
   -------------------------------------------------------------------- */
#define CsrBtGattSubscribeReqSend(_gattId,_address,_handle) { \
        CsrBtGattSubscribeReqExtSend(_gattId,_address,_handle,FALSE);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattUnsubscribeReqSend

   Description:

   This function should only be used to unsubscribe the handle of
   the Characteristic Value which previously were set by calling
   CsrBtGattSubscribeReqSend.

   As confirmation the application will receive a
   CSR_BT_GATT_SUBSCRIPTION_CFM message.

   Parameters:
   CsrBtGattId _gattId       - Application identifier
   CsrBtGattAddress _address - The device address of the peer server
   CsrBtGattHandle _handle   - The handle of the Characteristic Value.
   -------------------------------------------------------------------- */
#define CsrBtGattUnsubscribeReqSend(_gattId,_address,_handle) { \
        CsrBtGattSubscriptionReq *msg__; \
        msg__=CsrBtGattSubscriptionReq_struct(_gattId,_address,_handle,FALSE,FALSE); \
        CsrBtGattMsgTransport(msg__);}
 
/* --------------------------------------------------------------------
   Name:
   CsrBtGattCancelReqSend

   Description:
   Request to Cancel a running GATT procedure.

   As confirmation the application will receive the confirm message
   of the GATT procedure being cancelled.

   Note the following procedures may be cancelled:

   CsrBtGattDiscoverAllPrimaryServicesReqSend
   CsrBtGattDiscoverAllPrimaryServicesLocalReqSend
   CsrBtGattDiscoverPrimaryServicesBy16BitUuidReqSend
   CsrBtGattDiscoverPrimaryServicesBy16BitUuidLocalReqSend
   CsrBtGattDiscoverPrimaryServicesBy128BitUuidReqSend
   CsrBtGattDiscoverPrimaryServicesBy128BitUuidLocalReqSend
   CsrBtGattFindInclServicesReqSend
   CsrBtGattFindInclServicesLocalReqSend
   CsrBtGattDiscoverAllCharacOfAServiceReqSend
   CsrBtGattDiscoverAllCharacOfAServiceLocalReqSend
   CsrBtGattDiscoverCharacBy16BitUuidReqSend
   CsrBtGattDiscoverCharacBy16BitUuidLocalReqSend
   CsrBtGattDiscoverCharacBy128BitUuidReqSend
   CsrBtGattDiscoverCharacBy128BitUuidLocalReqSend
   CsrBtGattDiscoverAllCharacDescriptorsReqSend
   CsrBtGattDiscoverAllCharacDescriptorsLocalReqSend
   CsrBtGattReadReqSend
   CsrBtGattReadBy16BitUuidReqSend
   CsrBtGattReadBy16BitUuidLocalReqSend
   CsrBtGattReadBy128BitUuidReqSend
   CsrBtGattReadBy128BitUuidLocalReqSend
   CsrBtGattReadMultiReqSend
   CsrBtGattWriteReqSend
   CsrBtGattReliableWritesReqSend
   CsrBtGattWriteUserDescriptionReqSend
   CsrBtGattWriteProfileDefinedDescriptorReqSend
   CsrBtGattReadExtendedPropertiesReqSend
   CsrBtGattReadUserDescriptionReqSend
   CsrBtGattReadClientConfigurationReqSend
   CsrBtGattReadServerConfigurationReqSend
   CsrBtGattReadPresentationFormatReqSend
   CsrBtGattReadAggregateFormatReqSend
   CsrBtGattReadAggregateFormatReqSend
   CsrBtGattReadProfileDefinedDescriptorReqSend

   Parameters:
   CsrBtGattId _gattId   - Application identifier
   CsrBtConnId _btConnId - Connection identifier
   -------------------------------------------------------------------- */
CsrBtGattCancelReq *CsrBtGattCancelReq_struct(CsrBtGattId gattId, CsrBtConnId btConnId);

#define CsrBtGattCancelReqSend(_gattId, _btConnId) {                    \
        CsrBtGattCancelReq *msg__;                                      \
        msg__=CsrBtGattCancelReq_struct(_gattId, _btConnId);            \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
  Name: 
    CsrBtGattSetEventMaskReqSend
    
  Description: 
    
    Allows the application to subscribe for the events:
    CSR_BT_GATT_EVENT_MASK_SUBSCRIBE_NONE,
    CSR_BT_GATT_EVENT_MASK_SUBSCRIBE_WHITELIST_CHANGE, 
    CSR_BT_GATT_EVENT_MASK_SUBSCRIBE_PHYSICAL_LINK_STATUS
    CSR_BT_GATT_EVENT_MASK_SUBSCRIBE_PARAM_CONN_UPDATE_IND.
    CSR_BT_GATT_EVENT_MASK_SUBSCRIBE_RESTART_IND
    CSR_BT_GATT_EVENT_MASK_SUBSCRIBE_SCAN_RSP_STATUS
    CSR_BT_GATT_EVENT_MASK_SUBSCRIBE_SERVICE_CHANGED

   If set to CSR_BT_GATT_EVENT_MASK_SUBSCRIBE_PHYSICAL_LINK_STATUS
   the application will receive a CSR_BT_GATT_PHYSICAL_LINK_STATUS_IND
   message whenever a physical GATT/ATT link is established or disconnected.

   If set to CSR_BT_GATT_EVENT_MASK_SUBSCRIBE_NONE the application
   will stop receiving the event(s) defined by CsrBtGattEventMask.

   As confirmation the application will receive a
   CSR_BT_GATT_SET_EVENT_MASK_CFM message.

   Parameters:
   CsrBtGattId _gattId        - Application identifier
   CsrBtGattEventMask _eventM - Defines the event(s) to subscribe for
   -------------------------------------------------------------------- */
CsrBtGattSetEventMaskReq *CsrBtGattSetEventMaskReq_struct(CsrBtGattId        gattId,
                                                          CsrBtGattEventMask eventMask);

#define CsrBtGattSetEventMaskReqSend(_gattId,_eventM) {                 \
        CsrBtGattSetEventMaskReq *msg__;                                \
        msg__=CsrBtGattSetEventMaskReq_struct(_gattId,_eventM);         \
        CsrBtGattMsgTransport(msg__);}


/* --------------------------------------------------------------------
   Name:
   CsrBtGattWhitelistAddReqSend

   Description:
   Add peer one or more addresses to the whitelist.

   When the operation is complete, CSR_BT_GATT_WHITELIST_ADD_CFM
   will be sent as confirmation

   Parameters:
   CsrBtGattId _gattId        - Application identifier
   CsrUint16 _addressCount    - Number of addresses in 'address'
   CsrBtTypedAddr _address    - Array of typed addresses
   -------------------------------------------------------------------- */
CsrBtGattWhitelistAddReq *CsrBtGattWhitelistAddReq_struct(CsrBtGattId gattId,
                                                          CsrUint16 addressCount,
                                                          CsrBtTypedAddr *address);
#define CsrBtGattWhitelistAddReqSend(_gattId,_addressCount,_address) {  \
        CsrBtGattWhitelistAddReq *msg__;                                \
        msg__=CsrBtGattWhitelistAddReq_struct(_gattId,_addressCount,_address); \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattWhitelistClearReqSend

   Description:
   Clears the entire whitelist.

   When the operation is complete, CSR_BT_GATT_WHITELIST_CLEAR_CFM
   will be sent as confirmation

   Parameters:
   CsrBtGattId _gattId        - Application identifier
   -------------------------------------------------------------------- */
CsrBtGattWhitelistClearReq *CsrBtGattWhitelistClearReq_struct(CsrBtGattId gattId);
#define CsrBtGattWhitelistClearReqSend(_gattId) {                       \
        CsrBtGattWhitelistClearReq *msg__;                              \
        msg__=CsrBtGattWhitelistClearReq_struct(_gattId);               \
        CsrBtGattMsgTransport(msg__);}

/* --------------------------------------------------------------------
   Name:
   CsrBtGattWhitelistReadReqSend

   Description:
   Read the whitelist. The result will be sent in a
   CSR_BT_GATT_WHITELIST_READ_CFM confirmation.

   Parameters:
   CsrBtGattId _gattId        - Application identifier
   -------------------------------------------------------------------- */
CsrBtGattWhitelistReadReq *CsrBtGattWhitelistReadReq_struct(CsrBtGattId gattId);
#define CsrBtGattWhitelistReadReqSend(_gattId) {                        \
        CsrBtGattWhitelistReadReq *msg__;                               \
        msg__=CsrBtGattWhitelistReadReq_struct(_gattId);                \
        CsrBtGattMsgTransport(msg__);}


/* --------------------------------------------------------------------
  Name: 
    CsrBtGattParamConnUpdateResSend
    
  Description: 
    This function shall always be called after the
    application has received a CSR_BT_GATT_PARAM_CONN_UPDATE_IND 
    message. 

  Parameters: 
    CsrBtGattId _gattId        - Application identifier
    CsrBtConnId _btConnId      - Connection identifier 
    CsrUint16   _identifier    - Used to identify the ParamConnUpdate signal 
    CsrBool     _accept        - TRUE - if parameters are acceptable, 
                                 FALSE - non-acceptable
   -------------------------------------------------------------------- */
CsrBtGattParamConnUpdateRes *CsrBtGattParamConnUpdateRes_struct(CsrBtGattId gattId, 
                                                                CsrBtConnId btConnId,
                                                                CsrUint16   identifier,
                                                                CsrBool     accept);
#define CsrBtGattParamConnUpdateResSend(_gattId,_btConnId,_identifier,_accept) { \
        CsrBtGattParamConnUpdateRes *msg__; \
        msg__=CsrBtGattParamConnUpdateRes_struct(_gattId,_btConnId,_identifier,_accept); \
        CsrBtGattMsgTransport(msg__);}


/* --------------------------------------------------------------------
  Name: 
    CsrBtGattSecurityReqSend
    
  Description: 
    This function will start the LE security procedure. Note, it can 
    only be used on an LE physical connection.
    LE secure connections pairing would be used if supported by both devices.

    The Security Controller (SC) controls whether or not the bonding 
    flag is set. The bonding flag is set if the the value of the key 
    distribution parameters for Low Energy security is different from
    0. The default value of the key distribution is defined by 
    CSR_BT_SC_KEY_DIST_DEFAULT in csr_bt_usr_config_default.h 
    and may be change runtime by using CsrBtScLeKeyDistributionReqSend, 
    see csr_bt_sc_lib.h
    
  Parameters: 
    CsrBtGattId _gattId        - Application identifier
    CsrBtConnId _btConnId      - Connection identifier 
    CsrBtGattSecurityFlags _secRequirements 
                               - The security flags, which are defined 
                                 in csr_bt_gatt_prim.h. Valid values are:
                                   - CSR_BT_GATT_SECURITY_FLAGS_DEFAULT 
                                        Default low energy authentication requirement.
                                   - CSR_BT_GATT_SECURITY_FLAGS_UNAUTHENTICATED
                                        Encrypt the link.
                                   - CSR_BT_GATT_SECURITY_FLAGS_AUTHENTICATED
                                        Encrypt the link with MITM protection.
   -------------------------------------------------------------------- */
CsrBtGattSecurityReq *CsrBtGattSecurityReq_struct(CsrBtGattId gattId, 
                                                  CsrBtConnId btConnId,
                                                  CsrBtGattSecurityFlags secRequirements);
#define CsrBtGattSecurityReqSend(_gattId,_btConnId,_secRequirements) { \
        CsrBtGattSecurityReq *msg__; \
        msg__=CsrBtGattSecurityReq_struct(_gattId,_btConnId,_secRequirements); \
        CsrBtGattMsgTransport(msg__);}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtGattFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      Function to free contents of all GATT upstream primitives *
 *
 *  PARAMETERS
 *      eventClass :  Must be CSR_BT_GATT_PRIM,
 *      msg:          The upstream message received from Synergy BT GATT
 *----------------------------------------------------------------------------*/
extern void CsrBtGattFreeUpstreamMessageContents(CsrUint16 eventClass,
                                                 void      *message);


#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_GATT_LIB_H__ */

