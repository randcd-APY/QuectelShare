/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_bls.h"

int _qapi_BLE_BLS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BLS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

int _qapi_BLE_BLS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BLS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_BLS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

unsigned int _qapi_BLE_BLS_Query_Number_Attributes(uint8_t TargetID);

int _qapi_BLE_BLS_Read_Client_Configuration_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t ClientConfiguration);

int _qapi_BLE_BLS_Indicate_Blood_Pressure_Measurement(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t *Measurement_Data);

int _qapi_BLE_BLS_Notify_Intermediate_Cuff_Pressure(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t *Intermediate_Cuff_Pressure);

int _qapi_BLE_BLS_Set_Blood_Pressure_Feature(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t SupportedFeatures);

int _qapi_BLE_BLS_Query_Blood_Pressure_Feature(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t *SupportedFeatures);

int _qapi_BLE_BLS_Decode_Blood_Pressure_Measurement(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t *BloodPressureMeasurement);
