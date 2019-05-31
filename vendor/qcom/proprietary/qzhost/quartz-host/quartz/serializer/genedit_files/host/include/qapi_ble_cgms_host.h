/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_cgms.h"

int _qapi_BLE_CGMS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t Flags, qapi_BLE_CGMS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

int _qapi_BLE_CGMS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t Flags, qapi_BLE_CGMS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_CGMS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

unsigned int _qapi_BLE_CGMS_Query_Number_Attributes(uint8_t TargetID);

int _qapi_BLE_CGMS_Read_Client_Configuration_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t ClientConfiguration);

int _qapi_BLE_CGMS_Notify_CGMS_Measurements(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, uint32_t Flags, uint32_t NumberOfMeasurements, qapi_BLE_CGMS_Measurement_Data_t **MeasurementData);

int _qapi_BLE_CGMS_Feature_Read_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint32_t Flags, uint8_t ErrorCode, qapi_BLE_CGMS_Feature_Data_t *CGMSFeature);

int _qapi_BLE_CGMS_Status_Read_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint32_t Flags, uint8_t ErrorCode, qapi_BLE_CGMS_Status_Data_t *CGMSStatus);

int _qapi_BLE_CGMS_Session_Start_Time_Read_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint32_t Flags, uint8_t ErrorCode, qapi_BLE_CGMS_Session_Start_Time_Data_t *SessionStartTime);

int _qapi_BLE_CGMS_Session_Start_Time_Write_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode);

int _qapi_BLE_CGMS_Session_Run_Time_Read_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint32_t Flags, uint8_t ErrorCode, qapi_BLE_CGMS_Session_Run_Time_Data_t *SessionRunTime);

int _qapi_BLE_CGMS_Record_Access_Control_Point_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode);

int _qapi_BLE_CGMS_Indicate_Number_Of_Stored_Records(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, uint16_t NumberOfStoredRecords);

int _qapi_BLE_CGMS_Indicate_Record_Access_Control_Point_Result(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, uint8_t RequestOpCode, uint8_t ResponseCode);

int _qapi_BLE_CGMS_Specific_Ops_Control_Point_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode);

int _qapi_BLE_CGMS_Indicate_CGMS_Specific_Ops_Control_Point_Result(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, uint32_t Flags, uint8_t RequestOpCode, uint8_t ResponseCodeValue);

int _qapi_BLE_CGMS_Indicate_Communication_Interval(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, uint32_t Flags, uint8_t CommunicationIntervalMinutes);

int _qapi_BLE_CGMS_Indicate_Calibration_Data(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, uint32_t Flags, qapi_BLE_CGMS_Calibration_Data_Record_t *CalibrationData);

int _qapi_BLE_CGMS_Indicate_Alert_Level(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, uint32_t Flags, qapi_BLE_CGMS_SOCP_Response_Type_t ResponseOpCode, uint16_t AlertLevel);

int _qapi_BLE_CGMS_Decode_CGMS_Measurement(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, uint32_t NumberOfMeasurements, qapi_BLE_CGMS_Measurement_Data_t **MeasurementData);

int _qapi_BLE_CGMS_Decode_CGMS_Feature(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_CGMS_Feature_Data_t *CGMSFeature);

int _qapi_BLE_CGMS_Decode_CGMS_Status(uint8_t TargetID, uint32_t Flags, uint32_t ValueLength, uint8_t *Value, qapi_BLE_CGMS_Status_Data_t *CGMSStatus);

int _qapi_BLE_CGMS_Decode_Session_Start_Time(uint8_t TargetID, uint32_t Flags, uint32_t ValueLength, uint8_t *Value, qapi_BLE_CGMS_Session_Start_Time_Data_t *SessionTime);

int _qapi_BLE_CGMS_Format_Session_Start_Time(uint8_t TargetID, uint32_t Flags, qapi_BLE_CGMS_Session_Start_Time_Data_t *SessionStartTime, uint32_t BufferLength, uint8_t *Buffer);

int _qapi_BLE_CGMS_Decode_Session_Run_Time(uint8_t TargetID, uint32_t Flags, uint32_t ValueLength, uint8_t *Value, qapi_BLE_CGMS_Session_Run_Time_Data_t *CGMSRunTime);

int _qapi_BLE_CGMS_Decode_Record_Access_Control_Point_Response(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_CGMS_RACP_Response_Data_t *RACPData);

int _qapi_BLE_CGMS_Format_Record_Access_Control_Point_Command(uint8_t TargetID, qapi_BLE_CGMS_RACP_Format_Data_t *FormatData, uint32_t *BufferLength, uint8_t *Buffer);

int _qapi_BLE_CGMS_Decode_CGMS_Specific_Ops_Control_Point_Response(uint8_t TargetID, uint32_t Flags, uint32_t ValueLength, uint8_t *Value, qapi_BLE_CGMS_Specific_Ops_Control_Point_Response_Data_t *CGMSCPData);

int _qapi_BLE_CGMS_Format_CGMS_Specific_Ops_Control_Point_Command(uint8_t TargetID, uint32_t Flags, qapi_BLE_CGMS_Specific_Ops_Control_Point_Format_Data_t *FormatData, uint32_t BufferLength, uint8_t *Buffer);
