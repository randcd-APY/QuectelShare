/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_lls.h"

int _qapi_BLE_LLS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_LLS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

int _qapi_BLE_LLS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_LLS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_LLS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

uint32_t _qapi_BLE_LLS_Query_Number_Attributes(uint8_t TargetID);

int _qapi_BLE_LLS_Set_Alert_Level(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t Alert_Level);

int _qapi_BLE_LLS_Query_Alert_Level(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t *Alert_Level);
