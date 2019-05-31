/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_esstypes_common.h"

int Mnl_qapi_BLE_ESS_Read_Characteristic_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo, qapi_BLE_ESS_Characteristic_Data_t *CharacteristicData);
int Mnl_qapi_BLE_ESS_Notify_Characteristic(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo, qapi_BLE_ESS_Characteristic_Data_t *CharacteristicData);
int Mnl_qapi_BLE_ESS_Decode_Characteristic(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_ESS_Characteristic_Type_t Type, qapi_BLE_ESS_Characteristic_Data_t *CharacteristicData);
