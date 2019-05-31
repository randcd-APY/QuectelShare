/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_pass.h"

int _qapi_BLE_PASS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_PASS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

int _qapi_BLE_PASS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_PASS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_PASS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

unsigned int _qapi_BLE_PASS_Query_Number_Attributes(uint8_t TargetID);

int _qapi_BLE_PASS_Set_Alert_Status(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_PASS_Alert_Status_t AlertStatus);

int _qapi_BLE_PASS_Query_Alert_Status(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_PASS_Alert_Status_t *AlertStatus);

int _qapi_BLE_PASS_Set_Ringer_Setting(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_PASS_Ringer_Setting_t RingerSetting);

int _qapi_BLE_PASS_Query_Ringer_Setting(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_PASS_Ringer_Setting_t *RingerSetting);

int _qapi_BLE_PASS_Read_Client_Configuration_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, boolean_t NotificationsEnabled);

int _qapi_BLE_PASS_Send_Notification(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_PASS_Characteristic_Type_t CharacteristicType);

int _qapi_BLE_PASS_Decode_Alert_Status(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_PASS_Alert_Status_t *AlertStatusResult);

int _qapi_BLE_PASS_Decode_Ringer_Setting(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_PASS_Ringer_Setting_t *RingerSetting);

int _qapi_BLE_PASS_Format_Ringer_Control_Command(uint8_t TargetID, qapi_BLE_PASS_Ringer_Control_Command_t RingerControlCommand, uint32_t BufferLength, uint8_t *Buffer);
