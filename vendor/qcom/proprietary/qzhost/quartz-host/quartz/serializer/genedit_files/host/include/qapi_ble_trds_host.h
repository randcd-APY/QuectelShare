/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_trds.h"

int _qapi_BLE_TRDS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_TRDS_Initialize_Data_t *InitializeData, qapi_BLE_TRDS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

int _qapi_BLE_TRDS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_TRDS_Initialize_Data_t *InitializeData, qapi_BLE_TRDS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_TRDS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

uint32_t _qapi_BLE_TRDS_Query_Number_Attributes(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

int _qapi_BLE_TRDS_Write_Control_Point_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode);

int _qapi_BLE_TRDS_Indicate_Control_Point_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_TRDS_Control_Point_Response_Data_t *ResponseData);

int _qapi_BLE_TRDS_Read_CCCD_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_TRDS_CCCD_Type_t Type, uint16_t Configuration);

int _qapi_BLE_TRDS_Write_CCCD_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_TRDS_CCCD_Type_t Type);

int _qapi_BLE_TRDS_Format_Control_Point_Request(uint8_t TargetID, qapi_BLE_TRDS_Control_Point_Request_Data_t *RequestData, uint32_t BufferLength, uint8_t *Buffer);

int _qapi_BLE_TRDS_Decode_Control_Point_Response(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_TRDS_Control_Point_Response_Data_t *ResponseData);

int _qapi_BLE_TRDS_Format_Transport_Block(uint8_t TargetID, qapi_BLE_TRDS_Transport_Block_Data_t *TransportBlockData, uint32_t BufferLength, uint8_t *Buffer);

int _qapi_BLE_TRDS_Decode_Transport_Block(uint8_t TargetID, uint32_t RemainingLength, uint8_t *Buffer, qapi_BLE_TRDS_Transport_Block_Data_t *TransportBlockData);
