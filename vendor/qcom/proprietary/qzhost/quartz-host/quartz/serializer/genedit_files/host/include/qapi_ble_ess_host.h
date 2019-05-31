/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_ess.h"

int _qapi_BLE_ESS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ServiceFlags, qapi_BLE_ESS_Initialize_Data_t *InitializeData, qapi_BLE_ESS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

int _qapi_BLE_ESS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ServiceFlags, qapi_BLE_ESS_Initialize_Data_t *InitializeData, qapi_BLE_ESS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_ESS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

unsigned int _qapi_BLE_ESS_Query_Number_Attributes(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

int _qapi_BLE_ESS_Read_Characteristic_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo, qapi_BLE_ESS_Characteristic_Data_t *CharacteristicData);

int _qapi_BLE_ESS_Read_CCCD_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo, uint16_t Configuration);

int _qapi_BLE_ESS_Write_CCCD_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo);

int _qapi_BLE_ESS_Query_Extended_Properties(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo, uint16_t *ExtendedProperties);

int _qapi_BLE_ESS_Read_ES_Measurement_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo, qapi_BLE_ESS_ES_Measurement_Data_t *MeasurementData);

int _qapi_BLE_ESS_Read_ES_Trigger_Setting_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo, qapi_BLE_ESS_ES_Trigger_Setting_Instance_t Instance, qapi_BLE_ESS_ES_Trigger_Setting_Data_t *TriggerSetting);

int _qapi_BLE_ESS_Write_ES_Trigger_Setting_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo, qapi_BLE_ESS_ES_Trigger_Setting_Instance_t Instance);

int _qapi_BLE_ESS_Read_ES_Configuration_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo, uint8_t Configuration);

int _qapi_BLE_ESS_Write_ES_Configuration_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo);

int _qapi_BLE_ESS_Read_User_Description_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo, char *UserDescription);

int _qapi_BLE_ESS_Write_User_Description_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo);

int _qapi_BLE_ESS_Read_Valid_Range_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo, qapi_BLE_ESS_Valid_Range_Data_t *ValidRange);

int _qapi_BLE_ESS_Read_Descriptor_Value_Changed_CCCD_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, uint16_t Configuration);

int _qapi_BLE_ESS_Write_Descriptor_Value_Changed_CCCD_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode);

int _qapi_BLE_ESS_Notify_Characteristic(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_ESS_Characteristic_Info_t *CharacteristicInfo, qapi_BLE_ESS_Characteristic_Data_t *CharacteristicData);

int _qapi_BLE_ESS_Indicate_Descriptor_Value_Changed(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_ESS_Descriptor_Value_Changed_Data_t *DescriptorValueChanged);

int _qapi_BLE_ESS_Decode_Characteristic(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_ESS_Characteristic_Type_t Type, qapi_BLE_ESS_Characteristic_Data_t *CharacteristicData);

int _qapi_BLE_ESS_Decode_ES_Measurement(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_ESS_ES_Measurement_Data_t *MeasurementData);

int _qapi_BLE_ESS_Format_ES_Trigger_Setting_Request(uint8_t TargetID, qapi_BLE_ESS_Characteristic_Type_t Type, qapi_BLE_ESS_ES_Trigger_Setting_Data_t *TriggerSetting, uint32_t BufferLength, uint8_t *Buffer);

int _qapi_BLE_ESS_Decode_ES_Trigger_Setting(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_ESS_Characteristic_Type_t Type, qapi_BLE_ESS_ES_Trigger_Setting_Data_t *TriggerSetting);

int _qapi_BLE_ESS_Decode_Valid_Range(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_ESS_Characteristic_Type_t Type, qapi_BLE_ESS_Valid_Range_Data_t *ValidRange);

int _qapi_BLE_ESS_Decode_Descriptor_Value_Changed(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_ESS_Descriptor_Value_Changed_Data_t *DescriptorValueChanged);
