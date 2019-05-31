/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_hts.h"

int _qapi_BLE_HTS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_HTS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

int _qapi_BLE_HTS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_HTS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_HTS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

unsigned int _qapi_BLE_HTS_Query_Number_Attributes(uint8_t TargetID);

int _qapi_BLE_HTS_Set_Temperature_Type(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t Temperature_Type);

int _qapi_BLE_HTS_Query_Temperature_Type(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t *Temperature_Type);

int _qapi_BLE_HTS_Set_Measurement_Interval(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t Measurement_Interval);

int _qapi_BLE_HTS_Query_Measurement_Interval(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t *Measurement_Interval);

int _qapi_BLE_HTS_Set_Valid_Range(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_HTS_Valid_Range_Data_t *ValidRange);

int _qapi_BLE_HTS_Query_Valid_Range(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_HTS_Valid_Range_Data_t *ValidRange);

int _qapi_BLE_HTS_Read_Client_Configuration_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t Client_Configuration);

int _qapi_BLE_HTS_Notify_Intermediate_Temperature(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_HTS_Temperature_Measurement_Data_t *Temperature_Measurement);

int _qapi_BLE_HTS_Indicate_Measurement_Interval(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID);

int _qapi_BLE_HTS_Indicate_Temperature_Measurement(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_HTS_Temperature_Measurement_Data_t *Temperature_Measurement);

int _qapi_BLE_HTS_Decode_Temperature_Measurement(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_HTS_Temperature_Measurement_Data_t *TemperatureMeasurement);

int _qapi_BLE_HTS_Decode_Valid_Range(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_HTS_Valid_Range_Data_t *ValidRange);
