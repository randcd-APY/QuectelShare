/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_cps.h"

int _qapi_BLE_CPS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_CPS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

int _qapi_BLE_CPS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_CPS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_CPS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

unsigned int _qapi_BLE_CPS_Query_Number_Attributes(uint8_t TargetID);

int _qapi_BLE_CPS_Read_Client_Configuration_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t ClientConfiguration);

int _qapi_BLE_CPS_Vector_Client_Configuration_Update_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t ErrorCode);

int _qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t ServerConfiguration);

int _qapi_BLE_CPS_Set_Feature(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t SupportedFeatures);

int _qapi_BLE_CPS_Query_Feature(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t *SupportedFeatures);

int _qapi_BLE_CPS_Notify_CP_Measurement(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_CPS_Measurement_Data_t *MeasurementData);

int _qapi_BLE_CPS_Set_Sensor_Location(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t SensorLocation);

int _qapi_BLE_CPS_Query_Sensor_Location(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t *SensorLocation);

int _qapi_BLE_CPS_Notify_CP_Vector(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_CPS_Vector_Data_t *VectorData);

int _qapi_BLE_CPS_Control_Point_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t ErrorCode);

int _qapi_BLE_CPS_Indicate_Control_Point_Result(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_CPS_Control_Point_Command_Type_t CommandType, uint8_t ResponseCode);

int _qapi_BLE_CPS_Indicate_Control_Point_Result_With_Data(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_CPS_Control_Point_Indication_Data_t *IndicationData);

int _qapi_BLE_CPS_Format_CP_Measurement(uint8_t TargetID, qapi_BLE_CPS_Measurement_Data_t *MeasurementData, uint16_t *BufferLength, uint8_t *Buffer);

int _qapi_BLE_CPS_Decode_CP_Measurement(uint8_t TargetID, uint32_t BufferLength, uint8_t *Buffer, qapi_BLE_CPS_Measurement_Data_t *MeasurementData);

qapi_BLE_CPS_Vector_Data_t *_qapi_BLE_CPS_Decode_CP_Vector(uint8_t TargetID, uint32_t BufferLength, uint8_t *Buffer);

void _qapi_BLE_CPS_Free_CP_Vector_Data(uint8_t TargetID, qapi_BLE_CPS_Vector_Data_t * VectorData);

int _qapi_BLE_CPS_Decode_Control_Point_Response(uint8_t TargetID, uint32_t BufferLength, uint8_t *Buffer, qapi_BLE_CPS_Control_Point_Response_Data_t *ResponseData);

void _qapi_BLE_CPS_Free_Supported_Sensor_Locations_Data(uint8_t TargetID, uint8_t *SensorLocations);

int _qapi_BLE_CPS_Format_Control_Point_Command(uint8_t TargetID, qapi_BLE_CPS_Control_Point_Format_Data_t *FormatData, uint32_t *BufferLength, uint8_t *Buffer);
