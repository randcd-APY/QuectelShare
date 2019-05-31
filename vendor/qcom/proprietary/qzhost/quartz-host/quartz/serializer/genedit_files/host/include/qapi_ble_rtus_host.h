/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_rtus.h"

int _qapi_BLE_RTUS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_RTUS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

int _qapi_BLE_RTUS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_RTUS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_RTUS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

unsigned int _qapi_BLE_RTUS_Query_Number_Attributes(uint8_t TargetID);

int _qapi_BLE_RTUS_Set_Time_Update_State(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_RTUS_Time_Update_State_Data_t *TimeUpdateState);

int _qapi_BLE_RTUS_Query_Time_Update_State(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_RTUS_Time_Update_State_Data_t *TimeUpdateState);

int _qapi_BLE_RTUS_Decode_Time_Update_State(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_RTUS_Time_Update_State_Data_t *TimeUpdateState);

int _qapi_BLE_RTUS_Format_Control_Point_Command(uint8_t TargetID, qapi_BLE_RTUS_Time_Update_Control_Command_t Command, uint32_t BufferLength, uint8_t *Buffer);
