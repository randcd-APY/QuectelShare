/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_rscs.h"

int _qapi_BLE_RSCS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_RSCS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

int _qapi_BLE_RSCS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_RSCS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_RSCS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

unsigned int _qapi_BLE_RSCS_Query_Number_Attributes(uint8_t TargetID);

int _qapi_BLE_RSCS_Read_Client_Configuration_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t ClientConfiguration);

int _qapi_BLE_RSCS_Notify_RSC_Measurement(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_RSCS_RSC_Measurement_Data_t *MeasurementData);

int _qapi_BLE_RSCS_Set_RSC_Feature(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t SupportedFeatures);

int _qapi_BLE_RSCS_Query_RSC_Feature(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t *SupportedFeatures);

int _qapi_BLE_RSCS_Set_Sensor_Location(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t SensorLocation);

int _qapi_BLE_RSCS_Query_Sensor_Location(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t *SensorLocation);

int _qapi_BLE_RSCS_SC_Control_Point_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t ErrorCode);

int _qapi_BLE_RSCS_Indicate_SC_Control_Point_Result(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_RSCS_SCCP_Command_Type_t CommandType, uint8_t ResponseCode);

int _qapi_BLE_RSCS_Indicate_Supported_Sensor_Locations(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_RSCS_SCCP_Supported_Sensor_Locations_t *SupportedSensorLocations);

int _qapi_BLE_RSCS_Decode_RSC_Measurement(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_RSCS_RSC_Measurement_Data_t *MeasurementData);

int _qapi_BLE_RSCS_Format_SC_Control_Point_Command(uint8_t TargetID, qapi_BLE_RSCS_SC_Control_Point_Format_Data_t *FormatData, uint32_t *BufferLength, uint8_t *Buffer);

int _qapi_BLE_RSCS_Decode_SC_Control_Point_Response(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_RSCS_SC_Control_Point_Response_Data_t *SCCPResponseData);
