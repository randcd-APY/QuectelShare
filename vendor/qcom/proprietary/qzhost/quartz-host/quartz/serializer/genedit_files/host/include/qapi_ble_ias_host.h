/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_ias.h"

int _qapi_BLE_IAS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_IAS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

int _qapi_BLE_IAS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_IAS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_IAS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

unsigned int _qapi_BLE_IAS_Query_Number_Attributes(uint8_t TargetID);

int _qapi_BLE_IAS_Format_Control_Point_Command(uint8_t TargetID, qapi_BLE_IAS_Control_Point_Command_t Command, uint32_t BufferLength, uint8_t *Buffer);
