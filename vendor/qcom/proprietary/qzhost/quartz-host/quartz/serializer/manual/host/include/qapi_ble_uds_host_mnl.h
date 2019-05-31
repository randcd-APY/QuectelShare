/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_udstypes_common.h"

int Mnl_qapi_BLE_UDS_Read_Characteristic_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, uint16_t Offset, qapi_BLE_UDS_Characteristic_Type_t Type, qapi_BLE_UDS_Characteristic_t *UDS_Characteristic);
int Mnl_qapi_BLE_UDS_Format_UDS_Characteristic_Request(uint8_t TargetID, qapi_BLE_UDS_Characteristic_Type_t Type, qapi_BLE_UDS_Characteristic_t *UDS_Characteristic, uint16_t BufferLength, uint8_t *Buffer);
int Mnl_qapi_BLE_UDS_Decode_UDS_Characteristic_Response(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_UDS_Characteristic_Type_t Type, qapi_BLE_UDS_Characteristic_t *UDS_Characteristic);
