/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_ips.h"

int _qapi_BLE_IPS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_IPS_Initialize_Data_t *InitializeData, qapi_BLE_IPS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

int _qapi_BLE_IPS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_IPS_Initialize_Data_t *InitializeData, qapi_BLE_IPS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_IPS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

unsigned int _qapi_BLE_IPS_Query_Number_Attributes(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

int _qapi_BLE_IPS_Set_Indoor_Positioning_Configuration(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t Configuration);

int _qapi_BLE_IPS_Query_Indoor_Positioning_Configuration(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t *Configuration);

int _qapi_BLE_IPS_Set_Latitude(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, int32_t Latitude);

int _qapi_BLE_IPS_Query_Latitude(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, int32_t *Latitude);

int _qapi_BLE_IPS_Set_Longitude(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, int32_t Longitude);

int _qapi_BLE_IPS_Query_Longitude(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, int32_t *Longitude);

int _qapi_BLE_IPS_Set_Local_North_Coordinate(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, int16_t LocalCoordinate);

int _qapi_BLE_IPS_Query_Local_North_Coordinate(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, int16_t *LocalCoordinate);

int _qapi_BLE_IPS_Set_Local_East_Coordinate(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, int16_t LocalCoordinate);

int _qapi_BLE_IPS_Query_Local_East_Coordinate(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, int16_t *LocalCoordinate);

int _qapi_BLE_IPS_Set_Floor_Number(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t FloorNumber);

int _qapi_BLE_IPS_Query_Floor_Number(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t *FloorNumber);

int _qapi_BLE_IPS_Set_Altitude(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t Altitude);

int _qapi_BLE_IPS_Query_Altitude(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t *Altitude);

int _qapi_BLE_IPS_Set_Uncertainty(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t Uncertainty);

int _qapi_BLE_IPS_Query_Uncertainty(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t *Uncertainty);

int _qapi_BLE_IPS_Set_Location_Name(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, char *LocationName);

int _qapi_BLE_IPS_Query_Location_Name(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t LocationNameLength, char *LocationName);

int _qapi_BLE_IPS_Authorization_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, boolean_t NotAuthorized);

int _qapi_BLE_IPS_Format_Broadcasting_Data(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t *BufferLength, uint8_t *Buffer);

int _qapi_BLE_IPS_Decode_Broadcasting_Data(uint8_t TargetID, uint8_t BufferLength, uint8_t *Buffer, qapi_BLE_IPS_Broadcast_Data_t *BroadcastData);
