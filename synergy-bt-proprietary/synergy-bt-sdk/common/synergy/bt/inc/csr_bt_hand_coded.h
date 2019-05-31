#ifndef CSR_BT_HAND_CODED_H__
#define CSR_BT_HAND_CODED_H__

/******************************************************************************

Copyright (c) 2004-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"

#include "csr_types.h"
#include "csr_bt_util.h"
#include "csr_msgconv.h"

#include "csr_bt_cm_prim.h"
#include "csr_bt_sd_prim.h"
#include "csr_bt_sc_prim.h"
#include "csr_bt_mapc_prim.h"
#include "csr_bt_maps_prim.h"
#include "csr_bt_spp_prim.h"
#include "csr_bt_gatt_prim.h"
#include "rfcomm_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------ CM-BNEP ------------------------------ */

CsrUint8 *CsrBtCmBnepConnectReqSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *CsrBtCmBnepConnectReqDes(CsrUint8 *buffer, CsrSize length);
void CsrBtCmBnepConnectReqSerFree(void *voidPrimitivePointer);
CsrSize CsrBtCmBnepConnectReqSizeof(void *voidPrimitivePointer);

CsrUint8 *CsrBtCmBnepConnectAcceptReqSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *CsrBtCmBnepConnectAcceptReqDes(CsrUint8 *buffer, CsrSize length);
void CsrBtCmBnepConnectAcceptReqSerFree(void *voidPrimitivePointer);
CsrSize CsrBtCmBnepConnectAcceptReqSizeof(void *voidPrimitivePointer);


/* ------------------------------ SD ------------------------------ */

CsrUint8 *CsrBtSdReadDeviceListIndSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *CsrBtSdReadDeviceListIndDes(CsrUint8 *buffer, CsrSize length);
CsrSize CsrBtSdReadDeviceListIndSizeof(void *voidPrimitivePointer);
void CsrBtSdReadDeviceListIndSerFree(void *voidPrimitivePointer);

CsrUint8 *CsrBtSdReadDeviceListCfmSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *CsrBtSdReadDeviceListCfmDes(CsrUint8 *buffer, CsrSize length);
CsrSize CsrBtSdReadDeviceListCfmSizeof(void *voidPrimitivePointer);
void CsrBtSdReadDeviceListCfmSerFree(void *voidPrimitivePointer);


CsrUint8 *CsrBtSdDiscoverGattDatabaseCfmSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *CsrBtSdDiscoverGattDatabaseCfmDes(CsrUint8 *buffer, CsrSize length);
CsrSize CsrBtSdDiscoverGattDatabaseCfmSizeof(void *voidPrimitivePointer);
void CsrBtSdDiscoverGattDatabaseCfmSerFree(void *voidPrimitivePointer);

/* ------------------------------ SC ------------------------------ */

CsrSize CsrBtCmSmIoCapabilityRequestResSizeof(void *msg);
CsrUint8 *CsrBtCmSmIoCapabilityRequestResSer(CsrUint8 *buffer, CsrSize *length, void *msg);
void *CsrBtCmSmIoCapabilityRequestResDes(CsrUint8 *buffer, CsrSize length);
void CsrBtCmSmIoCapabilityRequestResSerFree(void *msg);

CsrSize CsrBtScAddRemoteOobDataReqSizeof(void *msg);
CsrUint8 *CsrBtScAddRemoteOobDataReqSer(CsrUint8 *buffer, CsrSize *length, void *msg);
void *CsrBtScAddRemoteOobDataReqDes(CsrUint8 *buffer, CsrSize length);
void CsrBtScAddRemoteOobDataReqSerFree(void *msg);

CsrSize CsrBtScReadLocalOobDataCfmSizeof(void *msg);
CsrUint8 *CsrBtScReadLocalOobDataCfmSer(CsrUint8 *buffer, CsrSize *length, void *msg);
void *CsrBtScReadLocalOobDataCfmDes(CsrUint8 *buffer, CsrSize length);
void CsrBtScReadLocalOobDataCfmSerFree(void *msg);

/********************************* GATT ************************************/
CsrSize CsrBtGattDbAddReqSizeof(void *voidPrimitivePointer);
CsrUint8 *CsrBtGattDbAddReqSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *CsrBtGattDbAddReqDes(CsrUint8 *buffer, CsrSize length);
void CsrBtGattDbAddReqSerFree(void *voidPrimitivePointer);

CsrSize CsrBtGattWriteReqSizeof(void *voidPrimitivePointer);
CsrUint8 *CsrBtGattWriteReqSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *CsrBtGattWriteReqDes(CsrUint8 *buffer, CsrSize length);
void CsrBtGattWriteReqSerFree(void *voidPrimitivePointer);

CsrSize CsrBtGattScanReqSizeof(void *voidPrimitivePointer);
CsrUint8 *CsrBtGattScanReqSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *CsrBtGattScanReqDes(CsrUint8 *ptr, CsrSize length);
void CsrBtGattScanReqSerFree(void *voidPrimitivePointer);

CsrSize CsrBtGattDbAccessWriteIndSizeof(void *voidPrimitivePointer);
CsrUint8 *CsrBtGattDbAccessWriteIndSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *CsrBtGattDbAccessWriteIndDes(CsrUint8 *buffer, CsrSize length);
void CsrBtGattDbAccessWriteIndSerFree(void *voidPrimitivePointer);

/********************************* ATT ************************************/
CsrSize ATT_ADD_DB_REQ_TSizeof(void *voidPrimitivePointer);
CsrUint8 *ATT_ADD_DB_REQ_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *ATT_ADD_DB_REQ_TDes(CsrUint8 *buffer, CsrSize length);
void ATT_ADD_DB_REQ_TSerFree(void *voidPrimitivePointer);

CsrSize ATT_ADD_REQ_TSizeof(void *voidPrimitivePointer);
CsrUint8 *ATT_ADD_REQ_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *ATT_ADD_REQ_TDes(CsrUint8 *buffer, CsrSize length);
void ATT_ADD_REQ_TSerFree(void *voidPrimitivePointer);

/* ------------------------------ DM ------------------------------ */

CsrUint8 *DM_SYNC_CONNECT_REQ_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *DM_SYNC_CONNECT_REQ_TDes(CsrUint8 *buffer, CsrSize length);
void DM_SYNC_CONNECT_REQ_TSerFree(void *voidPrimitivePointer);
CsrSize DM_SYNC_CONNECT_REQ_TSizeof(void *voidPrimitivePointer);

CsrUint8 *DM_SYNC_RENEGOTIATE_REQ_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *DM_SYNC_RENEGOTIATE_REQ_TDes(CsrUint8 *buffer, CsrSize length);
void DM_SYNC_RENEGOTIATE_REQ_TSerFree(void *voidPrimitivePointer);
CsrSize DM_SYNC_RENEGOTIATE_REQ_TSizeof(void *voidPrimitivePointer);

CsrUint8 *DM_SM_LINK_KEY_REQUEST_RSP_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *DM_SM_LINK_KEY_REQUEST_RSP_TDes(CsrUint8 *buffer, CsrSize length);
void DM_SM_LINK_KEY_REQUEST_RSP_TSerFree(void *voidPrimitivePointer);
CsrSize DM_SM_LINK_KEY_REQUEST_RSP_TSizeof(void *voidPrimitivePointer);

CsrUint8 *DM_SM_IO_CAPABILITY_REQUEST_RSP_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *DM_SM_IO_CAPABILITY_REQUEST_RSP_TDes(CsrUint8 *buffer, CsrSize length);
void DM_SM_IO_CAPABILITY_REQUEST_RSP_TSerFree(void *voidPrimitivePointer);
CsrSize DM_SM_IO_CAPABILITY_REQUEST_RSP_TSizeof(void *voidPrimitivePointer);

CsrUint8 *DM_SM_READ_LOCAL_OOB_DATA_CFM_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *DM_SM_READ_LOCAL_OOB_DATA_CFM_TDes(CsrUint8 *buffer, CsrSize length);
void DM_SM_READ_LOCAL_OOB_DATA_CFM_TSerFree(void *voidPrimitivePointer);
CsrSize DM_SM_READ_LOCAL_OOB_DATA_CFM_TSizeof(void *voidPrimitivePointer);

CsrUint8 *DM_AMPM_NUMBER_COMPLETED_DATA_BLOCKS_REQ_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *DM_AMPM_NUMBER_COMPLETED_DATA_BLOCKS_REQ_TDes(CsrUint8 *buffer, CsrSize length);
void DM_AMPM_NUMBER_COMPLETED_DATA_BLOCKS_REQ_TSerFree(void *voidPrimitivePointer);
CsrSize DM_AMPM_NUMBER_COMPLETED_DATA_BLOCKS_REQ_TSizeof(void *voidPrimitivePointer);

CsrUint8 *DM_SM_KEYS_IND_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *DM_SM_KEYS_IND_TDes(CsrUint8 *buffer, CsrSize length);
void DM_SM_KEYS_IND_TSerFree(void *voidPrimitivePointer);
CsrSize DM_SM_KEYS_IND_TSizeof(void *voidPrimitivePointer);

CsrUint8 *DM_SM_KEY_REQUEST_RSP_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *DM_SM_KEY_REQUEST_RSP_TDes(CsrUint8 *buffer, CsrSize length);
void DM_SM_KEY_REQUEST_RSP_TSerFree(void *voidPrimitivePointer);
CsrSize DM_SM_KEY_REQUEST_RSP_TSizeof(void *voidPrimitivePointer);

CsrUint8 *DM_SM_ADD_DEVICE_REQ_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *DM_SM_ADD_DEVICE_REQ_TDes(CsrUint8 *buffer, CsrSize length);
void DM_SM_ADD_DEVICE_REQ_TSerFree(void *voidPrimitivePointer);
CsrSize DM_SM_ADD_DEVICE_REQ_TSizeof(void *voidPrimitivePointer);

CsrUint8 *DM_SM_READ_DEVICE_CFM_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *DM_SM_READ_DEVICE_CFM_TDes(CsrUint8 *buffer, CsrSize length);
void DM_SM_READ_DEVICE_CFM_TSerFree(void *voidPrimitivePointer);
CsrSize DM_SM_READ_DEVICE_CFM_TSizeof(void *voidPrimitivePointer);

/* ------------------------------ DM HCI ------------------------------ */

CsrUint8 *DM_HCI_INQUIRY_RESULT_IND_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *DM_HCI_INQUIRY_RESULT_IND_TDes(CsrUint8 *buffer, CsrSize length);
void DM_HCI_INQUIRY_RESULT_IND_TSerFree(void *voidPrimitivePointer);
CsrSize DM_HCI_INQUIRY_RESULT_IND_TSizeof(void *voidPrimitivePointer);

CsrUint8 *DM_HCI_INQUIRY_RESULT_WITH_RSSI_IND_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *DM_HCI_INQUIRY_RESULT_WITH_RSSI_IND_TDes(CsrUint8 *buffer, CsrSize length);
void DM_HCI_INQUIRY_RESULT_WITH_RSSI_IND_TSerFree(void *voidPrimitivePointer);
CsrSize DM_HCI_INQUIRY_RESULT_WITH_RSSI_IND_TSizeof(void *voidPrimitivePointer);

CsrUint8 *DM_HCI_REMOTE_NAME_CFM_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *DM_HCI_REMOTE_NAME_CFM_TDes(CsrUint8 *buffer, CsrSize length);
void DM_HCI_REMOTE_NAME_CFM_TSerFree(void *voidPrimitivePointer);
CsrSize DM_HCI_REMOTE_NAME_CFM_TSizeof(void *voidPrimitivePointer);

CsrUint8 *DM_HCI_READ_LOCAL_NAME_CFM_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *DM_HCI_READ_LOCAL_NAME_CFM_TDes(CsrUint8 *buffer, CsrSize length);
void DM_HCI_READ_LOCAL_NAME_CFM_TSerFree(void *voidPrimitivePointer);
CsrSize DM_HCI_READ_LOCAL_NAME_CFM_TSizeof(void *voidPrimitivePointer);

CsrUint8 *DM_HCI_READ_CURRENT_IAC_LAP_CFM_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *DM_HCI_READ_CURRENT_IAC_LAP_CFM_TDes(CsrUint8 *buffer, CsrSize length);
void DM_HCI_READ_CURRENT_IAC_LAP_CFM_TSerFree(void *voidPrimitivePointer);
CsrSize DM_HCI_READ_CURRENT_IAC_LAP_CFM_TSizeof(void *voidPrimitivePointer);

CsrUint8 *DM_HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_CFM_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *DM_HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_CFM_TDes(CsrUint8 *buffer, CsrSize length);
void DM_HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_CFM_TSerFree(void *voidPrimitivePointer);
CsrSize DM_HCI_READ_EXTENDED_INQUIRY_RESPONSE_DATA_CFM_TSizeof(void *voidPrimitivePointer);

CsrUint8 *DM_HCI_EXTENDED_INQUIRY_RESULT_IND_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *DM_HCI_EXTENDED_INQUIRY_RESULT_IND_TDes(CsrUint8 *buffer, CsrSize length);
void DM_HCI_EXTENDED_INQUIRY_RESULT_IND_TSerFree(void *voidPrimitivePointer);
CsrSize DM_HCI_EXTENDED_INQUIRY_RESULT_IND_TSizeof(void *voidPrimitivePointer);

CsrUint8 *DM_HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_REQ_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *DM_HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_REQ_TDes(CsrUint8 *buffer, CsrSize length);
void DM_HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_REQ_TSerFree(void *voidPrimitivePointer);
CsrSize DM_HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_DATA_REQ_TSizeof(void *voidPrimitivePointer);

CsrUint8 *DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_TDes(CsrUint8 *buffer, CsrSize length);
void DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_TSerFree(void *voidPrimitivePointer);
CsrSize DM_HCI_WRITE_CURRENT_IAC_LAP_REQ_TSizeof(void *voidPrimitivePointer);

CsrUint8 *DM_HCI_CHANGE_LOCAL_NAME_REQ_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *DM_HCI_CHANGE_LOCAL_NAME_REQ_TDes(CsrUint8 *buffer, CsrSize length);
void DM_HCI_CHANGE_LOCAL_NAME_REQ_TSerFree(void *voidPrimitivePointer);
CsrSize DM_HCI_CHANGE_LOCAL_NAME_REQ_TSizeof(void *voidPrimitivePointer);

CsrUint8 *DM_HCI_SET_EVENT_FILTER_REQ_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *DM_HCI_SET_EVENT_FILTER_REQ_TDes(CsrUint8 *buffer, CsrSize length);
void DM_HCI_SET_EVENT_FILTER_REQ_TSerFree(void *voidPrimitivePointer);
CsrSize DM_HCI_SET_EVENT_FILTER_REQ_TSizeof(void *voidPrimitivePointer);

CsrSize DM_HCI_RETURN_LINK_KEYS_IND_TSizeof(void *voidPrimitivePointer);
CsrUint8 *DM_HCI_RETURN_LINK_KEYS_IND_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *DM_HCI_RETURN_LINK_KEYS_IND_TDes(CsrUint8 *buffer, CsrSize length);
void DM_HCI_RETURN_LINK_KEYS_IND_TSerFree(void *voidPrimitivePointer);

CsrSize DM_HCI_WRITE_STORED_LINK_KEY_REQ_TSizeof(void *voidPrimitivePointer);
CsrUint8 *DM_HCI_WRITE_STORED_LINK_KEY_REQ_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *DM_HCI_WRITE_STORED_LINK_KEY_REQ_TDes(CsrUint8 *buffer, CsrSize length);
void DM_HCI_WRITE_STORED_LINK_KEY_REQ_TSerFree(void *voidPrimitivePointer);

CsrSize DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_TSizeof(void *voidPrimitivePointer);
CsrUint8 *DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_TDes(CsrUint8 *buffer, CsrSize length);
void DM_HCI_HOST_NUM_COMPLETED_PACKETS_REQ_TSerFree(void *voidPrimitivePointer);

CsrSize DM_HCI_ULP_ADVERTISING_REPORT_IND_TSizeof(void *voidPrimitivePointer);
CsrUint8 *DM_HCI_ULP_ADVERTISING_REPORT_IND_TSer(CsrUint8 *ptr, CsrSize *length, void *voidPrimitivePointer);
void *DM_HCI_ULP_ADVERTISING_REPORT_IND_TDes(CsrUint8 *buffer, CsrSize length);
void DM_HCI_ULP_ADVERTISING_REPORT_IND_TSerFree(void *voidPrimitivePointer);

CsrUint8 *DM_HCI_CSB_RECEIVE_IND_TSer(CsrUint8 *buffer, CsrSize *length, void *voidPrimitivePointer);
void *DM_HCI_CSB_RECEIVE_IND_TDes(CsrUint8 *buffer, CsrSize length);
void DM_HCI_CSB_RECEIVE_IND_TSerFree(void *voidPrimitivePointer);
CsrSize DM_HCI_CSB_RECEIVE_IND_TSizeof(void *voidPrimitivePointer);

CsrUint8 *DM_HCI_SET_CSB_DATA_REQ_TSer(CsrUint8 *buffer, CsrSize *length, void *voidPrimitivePointer);
void *DM_HCI_SET_CSB_DATA_REQ_TDes(CsrUint8 *buffer, CsrSize length);
void DM_HCI_SET_CSB_DATA_REQ_TSerFree(void *voidPrimitivePointer);
CsrSize DM_HCI_SET_CSB_DATA_REQ_TSizeof(void *voidPrimitivePointer);

#ifdef __cplusplus
}
#endif

#endif
