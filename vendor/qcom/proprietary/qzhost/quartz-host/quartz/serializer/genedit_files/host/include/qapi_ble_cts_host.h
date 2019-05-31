/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_cts.h"

int _qapi_BLE_CTS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_CTS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

int _qapi_BLE_CTS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_CTS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_CTS_Initialize_Service_Flags(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t Flags, qapi_BLE_CTS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_CTS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

unsigned int _qapi_BLE_CTS_Query_Number_Attributes(uint8_t TargetID);

int _qapi_BLE_CTS_Current_Time_Read_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, qapi_BLE_CTS_Current_Time_Data_t *Current_Time);

int _qapi_BLE_CTS_Current_Time_Read_Request_Error_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t ErrorCode);

int _qapi_BLE_CTS_Current_Time_Write_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t ErrorCode);

int _qapi_BLE_CTS_Set_Local_Time_Information(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_CTS_Local_Time_Information_Data_t *Local_Time);

int _qapi_BLE_CTS_Query_Local_Time_Information(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_CTS_Local_Time_Information_Data_t *Local_Time);

int _qapi_BLE_CTS_Local_Time_Information_Write_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t ErrorCode);

int _qapi_BLE_CTS_Reference_Time_Information_Read_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, qapi_BLE_CTS_Reference_Time_Information_Data_t *Reference_Time);

int _qapi_BLE_CTS_Reference_Time_Information_Read_Request_Error_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t ErrorCode);

int _qapi_BLE_CTS_Read_Client_Configuration_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t Client_Configuration);

int _qapi_BLE_CTS_Notify_Current_Time(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_CTS_Current_Time_Data_t *Current_Time);

int _qapi_BLE_CTS_Format_Current_Time(uint8_t TargetID, qapi_BLE_CTS_Current_Time_Data_t *Current_Time, uint32_t BufferLength, uint8_t *Buffer);

int _qapi_BLE_CTS_Decode_Current_Time(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_CTS_Current_Time_Data_t *Current_Time);

int _qapi_BLE_CTS_Format_Local_Time_Information(uint8_t TargetID, qapi_BLE_CTS_Local_Time_Information_Data_t *Local_Time, uint32_t BufferLength, uint8_t *Buffer);

int _qapi_BLE_CTS_Decode_Local_Time_Information(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_CTS_Local_Time_Information_Data_t *Local_Time);

int _qapi_BLE_CTS_Decode_Reference_Time_Information(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_CTS_Reference_Time_Information_Data_t *Reference_Time);
