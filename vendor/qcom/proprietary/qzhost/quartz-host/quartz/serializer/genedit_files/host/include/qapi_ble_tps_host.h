/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_tps.h"

int _qapi_BLE_TPS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t *ServiceID);

int _qapi_BLE_TPS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_TPS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

unsigned int _qapi_BLE_TPS_Query_Number_Attributes(uint8_t TargetID);

int _qapi_BLE_TPS_Set_Tx_Power_Level(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, int8_t Tx_Power_Level);

int _qapi_BLE_TPS_Query_Tx_Power_Level(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, int8_t *Tx_Power_Level);
