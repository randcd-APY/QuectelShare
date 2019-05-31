/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_hcitypes_common.h"

int Mnl_qapi_BLE_GAP_LE_Set_Advertising_Data(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t Length, qapi_BLE_Advertising_Data_t *Advertising_Data);
int Mnl_qapi_BLE_GAP_LE_Convert_Advertising_Data(uint8_t TargetID, qapi_BLE_GAP_LE_Advertising_Data_t *GAP_LE_Advertising_Data, qapi_BLE_Advertising_Data_t *Advertising_Data);
int Mnl_qapi_BLE_GAP_LE_Parse_Advertising_Data(uint8_t TargetID, qapi_BLE_Advertising_Data_t *Advertising_Data, qapi_BLE_GAP_LE_Advertising_Data_t *GAP_LE_Advertising_Data);
int Mnl_qapi_BLE_GAP_LE_Set_Scan_Response_Data(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t Length, qapi_BLE_Scan_Response_Data_t *Scan_Response_Data);
int Mnl_qapi_BLE_GAP_LE_Convert_Scan_Response_Data(uint8_t TargetID, qapi_BLE_GAP_LE_Advertising_Data_t *GAP_LE_Advertising_Data, qapi_BLE_Scan_Response_Data_t *Scan_Response_Data);
int Mnl_qapi_BLE_GAP_LE_Parse_Scan_Response_Data(uint8_t TargetID, qapi_BLE_Scan_Response_Data_t *Scan_Response_Data, qapi_BLE_GAP_LE_Advertising_Data_t *GAP_LE_Advertising_Data);
