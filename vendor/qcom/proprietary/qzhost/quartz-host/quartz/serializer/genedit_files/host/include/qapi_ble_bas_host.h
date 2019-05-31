/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_bas.h"

int _qapi_BLE_BAS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BAS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

int _qapi_BLE_BAS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BAS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_BAS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

uint32_t _qapi_BLE_BAS_Query_Number_Attributes(uint8_t TargetID);

int _qapi_BLE_BAS_Battery_Level_Read_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t BatteryLevel);

int _qapi_BLE_BAS_Battery_Level_Read_Request_Error_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t ErrorCode);

int _qapi_BLE_BAS_Query_Characteristic_Presentation_Format(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_BAS_Presentation_Format_Data_t *CharacteristicPresentationFormat);

int _qapi_BLE_BAS_Set_Characteristic_Presentation_Format(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_BAS_Presentation_Format_Data_t *CharacteristicPresentationFormat);

int _qapi_BLE_BAS_Read_Client_Configuration_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t Client_Configuration);

int _qapi_BLE_BAS_Notify_Battery_Level(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, uint8_t BatteryLevel);

int _qapi_BLE_BAS_Decode_Characteristic_Presentation_Format(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_BAS_Presentation_Format_Data_t *CharacteristicPresentationFormat);
