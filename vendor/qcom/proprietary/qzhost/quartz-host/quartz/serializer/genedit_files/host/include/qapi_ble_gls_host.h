/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_gls.h"

int _qapi_BLE_GLS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_GLS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

int _qapi_BLE_GLS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_GLS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_GLS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

unsigned int _qapi_BLE_GLS_Query_Number_Attributes(uint8_t TargetID);

int _qapi_BLE_GLS_Set_Glucose_Feature(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t SupportedFeatures);

int _qapi_BLE_GLS_Query_Glucose_Feature(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t *SupportedFeatures);

int _qapi_BLE_GLS_Read_Client_Configuration_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t ClientConfiguration);

int _qapi_BLE_GLS_Record_Access_Control_Point_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t ErrorCode);

int _qapi_BLE_GLS_Notify_Glucose_Measurement(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_GLS_Glucose_Measurement_Data_t *MeasurementData);

int _qapi_BLE_GLS_Notify_Glucose_Measurement_Context(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_GLS_Glucose_Measurement_Context_Data_t *ContextData);

int _qapi_BLE_GLS_Indicate_Number_Of_Stored_Records(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, uint16_t NumberOfStoredRecords);

int _qapi_BLE_GLS_Indicate_Record_Access_Control_Point_Result(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_GLS_RACP_Command_Type_t CommandType, uint8_t ResponseCode);

int _qapi_BLE_GLS_Decode_Glucose_Measurement(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_GLS_Glucose_Measurement_Data_t *MeasurementData);

int _qapi_BLE_GLS_Decode_Glucose_Measurement_Context(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_GLS_Glucose_Measurement_Context_Data_t *ContextData);

int _qapi_BLE_GLS_Decode_Record_Access_Control_Point_Response(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_GLS_Record_Access_Control_Point_Response_Data_t *RACPData);

int _qapi_BLE_GLS_Format_Record_Access_Control_Point_Command(uint8_t TargetID, qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t *FormatData, uint32_t *BufferLength, uint8_t *Buffer);
