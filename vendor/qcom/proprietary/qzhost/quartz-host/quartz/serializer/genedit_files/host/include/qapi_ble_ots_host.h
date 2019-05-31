/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_ots.h"

int _qapi_BLE_OTS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t ServiceFlags, qapi_BLE_OTS_Initialize_Data_t *InitializeData, qapi_BLE_OTS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

int _qapi_BLE_OTS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t ServiceFlags, qapi_BLE_OTS_Initialize_Data_t *InitializeData, qapi_BLE_OTS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_OTS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

unsigned int _qapi_BLE_OTS_Query_Number_Attributes(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

int _qapi_BLE_OTS_Read_CCCD_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_OTS_CCCD_Characteristic_Type_t Type, uint16_t Configuration);

int _qapi_BLE_OTS_Write_CCCD_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_OTS_CCCD_Characteristic_Type_t Type);

int _qapi_BLE_OTS_Read_OTS_Feature_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_OTS_Feature_Data_t *OTSFeature);

int _qapi_BLE_OTS_Read_Object_Metadata_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_OTS_Object_Metadata_Type_t Type, qapi_BLE_OTS_Object_Metadata_Data_t *Metadata, uint8_t Offset);

int _qapi_BLE_OTS_Write_Object_Metadata_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_OTS_Object_Metadata_Type_t Type);

int _qapi_BLE_OTS_Write_OACP_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode);

int _qapi_BLE_OTS_Indicate_OACP_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_OTS_OACP_Response_Data_t *ResponseData);

int _qapi_BLE_OTS_Write_OLCP_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode);

int _qapi_BLE_OTS_Indicate_OLCP_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_OTS_OLCP_Response_Data_t *ResponseData);

int _qapi_BLE_OTS_Read_Object_List_Filter_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_OTS_Object_List_Filter_Instance_t Instance, qapi_BLE_OTS_Object_List_Filter_Data_t *ListFilterData, uint8_t Offset);

int _qapi_BLE_OTS_Write_Object_List_Filter_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_OTS_Object_List_Filter_Instance_t Instance);

int _qapi_BLE_OTS_Prepare_Write_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode);

int _qapi_BLE_OTS_Indicate_Object_Changed(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_OTS_Object_Changed_Data_t *ObjectChangedData);

int _qapi_BLE_OTS_Calculate_CRC_32(uint8_t TargetID, uint32_t BufferLength, uint8_t *Buffer, uint32_t *Checksum);

int _qapi_BLE_OTS_Decode_OTS_Feature(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_OTS_Feature_Data_t *OTSFeature);

int _qapi_BLE_OTS_Format_Object_Metadata(uint8_t TargetID, qapi_BLE_OTS_Object_Metadata_Type_t Type, qapi_BLE_OTS_Object_Metadata_Data_t *Metadata, uint16_t BufferLength, uint8_t *Buffer);

int _qapi_BLE_OTS_Decode_Object_Metadata(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_OTS_Object_Metadata_Type_t Type, qapi_BLE_OTS_Object_Metadata_Data_t *Metadata);

int _qapi_BLE_OTS_Format_OACP_Request(uint8_t TargetID, qapi_BLE_OTS_OACP_Request_Data_t *RequestData, uint16_t BufferLength, uint8_t *Buffer);

int _qapi_BLE_OTS_Decode_OACP_Response(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_OTS_OACP_Response_Data_t *ResponseData);

int _qapi_BLE_OTS_Format_OLCP_Request(uint8_t TargetID, qapi_BLE_OTS_OLCP_Request_Data_t *RequestData, uint16_t BufferLength, uint8_t *Buffer);

int _qapi_BLE_OTS_Decode_OLCP_Response(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_OTS_OLCP_Response_Data_t *ResponseData);

int _qapi_BLE_OTS_Format_Object_List_Filter_Data(uint8_t TargetID, qapi_BLE_OTS_Object_List_Filter_Data_t *ListFilterData, uint16_t BufferLength, uint8_t *Buffer);

int _qapi_BLE_OTS_Decode_Object_List_Filter_Data(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_OTS_Object_List_Filter_Data_t *ListFilterData);

int _qapi_BLE_OTS_Decode_Object_Changed_Data(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_OTS_Object_Changed_Data_t *ObjectChangedData);

int _qapi_BLE_OTS_Channel_Open_Connection_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t CID, boolean_t AcceptConnection);

int _qapi_BLE_OTS_Channel_Get_Connection_Mode(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_OTS_Channel_Connection_Mode_t *ConnectionMode);

int _qapi_BLE_OTS_Channel_Set_Connection_Mode(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_OTS_Channel_Connection_Mode_t ConnectionMode);

int _qapi_BLE_OTS_Format_Directory_Listing_Object_Contents(uint8_t TargetID, uint32_t NumberOfObjects, qapi_BLE_OTS_Object_Data_t *ObjectData, uint32_t BufferLength, uint8_t *Buffer);

int _qapi_BLE_OTS_Channel_Connect_Request(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_GATT_Connection_Type_t Type, qapi_BLE_BD_ADDR_t BD_ADDR, qapi_BLE_OTS_Channel_Event_Callback_t EventCallback, uint32_t CallbackParameter, qapi_BLE_L2CA_LE_Channel_Parameters_t *ChannelParameters);

int _qapi_BLE_OTS_Decode_Directory_Listing_Object_Contents(uint8_t TargetID, uint32_t BufferLength, uint8_t *Buffer, uint32_t NumberOfObjects, qapi_BLE_OTS_Object_Data_t *ObjectData);

int _qapi_BLE_OTS_Channel_Close_Connection(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t CID);

int _qapi_BLE_OTS_Channel_Send_Data(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t CID, qapi_BLE_L2CA_Queueing_Parameters_t *QueueingParameters, uint16_t Data_Length, uint8_t *Data);

int _qapi_BLE_OTS_Channel_Grant_Credits(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t CID, uint16_t Credits);
