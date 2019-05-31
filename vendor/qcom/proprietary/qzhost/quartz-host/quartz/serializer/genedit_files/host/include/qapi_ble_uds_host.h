/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_uds.h"

int _qapi_BLE_UDS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_UDS_Initialize_Data_t *InitializeData, qapi_BLE_UDS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

int _qapi_BLE_UDS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_UDS_Initialize_Data_t *InitializeData, qapi_BLE_UDS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_UDS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

unsigned int _qapi_BLE_UDS_Query_Number_Attributes(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

int _qapi_BLE_UDS_Read_Characteristic_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, uint16_t Offset, qapi_BLE_UDS_Characteristic_Type_t Type, qapi_BLE_UDS_Characteristic_t *UDS_Characteristic);

int _qapi_BLE_UDS_Write_Characteristic_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_UDS_Characteristic_Type_t Type);

int _qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_UDS_Characteristic_Type_t Type);

int _qapi_BLE_UDS_Read_CCCD_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_UDS_CCCD_Characteristic_Type_t Type, uint16_t Configuration);

int _qapi_BLE_UDS_Write_CCCD_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_UDS_CCCD_Characteristic_Type_t Type);

int _qapi_BLE_UDS_Database_Change_Increment_Read_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, uint32_t DatabaseChangeIncrement);

int _qapi_BLE_UDS_Database_Change_Increment_Write_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode);

int _qapi_BLE_UDS_Notify_Database_Change_Increment(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, uint32_t DatabaseChangeIncrement);

int _qapi_BLE_UDS_User_Index_Read_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, uint8_t User_Index);

int _qapi_BLE_UDS_User_Control_Point_Write_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode);

int _qapi_BLE_UDS_Indicate_User_Control_Point_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_UDS_User_Control_Point_Response_Data_t *ResponseData);

int _qapi_BLE_UDS_Format_UDS_Characteristic_Request(uint8_t TargetID, qapi_BLE_UDS_Characteristic_Type_t Type, qapi_BLE_UDS_Characteristic_t *UDS_Characteristic, uint16_t BufferLength, uint8_t *Buffer);

int _qapi_BLE_UDS_Decode_UDS_Characteristic_Response(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_UDS_Characteristic_Type_t Type, qapi_BLE_UDS_Characteristic_t *UDS_Characteristic);

int _qapi_BLE_UDS_Format_User_Control_Point_Request(uint8_t TargetID, qapi_BLE_UDS_User_Control_Point_Request_Data_t *RequestData, uint16_t BufferLength, uint8_t *Buffer);

int _qapi_BLE_UDS_Decode_User_Control_Point_Response(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_UDS_User_Control_Point_Response_Data_t *ResponseData);
