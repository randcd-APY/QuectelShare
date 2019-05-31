/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_ipsp.h"

int _qapi_BLE_IPSP_Initialize_Node_Role(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_L2CA_LE_Channel_Parameters_t *ChannelParameters, qapi_BLE_IPSP_Event_Callback_t EventCallback, uint32_t CallbackParameter, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_IPSP_Cleanup_Node_Role(uint8_t TargetID, uint32_t BluetoothStackID);

int _qapi_BLE_IPSP_Open_Connection_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BD_ADDR_t RemoteDevice, boolean_t AcceptConnection);

int _qapi_BLE_IPSP_Get_Node_Connection_Mode(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_IPSP_Node_Connection_Mode_t *ConnectionMode);

int _qapi_BLE_IPSP_Set_Node_Connection_Mode(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_IPSP_Node_Connection_Mode_t ConnectionMode);

int _qapi_BLE_IPSP_Connect_Remote_Node(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BD_ADDR_t BD_ADDR, qapi_BLE_L2CA_LE_Channel_Parameters_t *ChannelParameters, qapi_BLE_IPSP_Event_Callback_t EventCallback, uint32_t CallbackParameter);

int _qapi_BLE_IPSP_Close_Connection(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BD_ADDR_t RemoteDevice);

int _qapi_BLE_IPSP_Send_Packet(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BD_ADDR_t RemoteDevice, qapi_BLE_L2CA_Queueing_Parameters_t *QueueingParameters, uint16_t Data_Length, uint8_t *Data);

int _qapi_BLE_IPSP_Grant_Credits(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BD_ADDR_t RemoteDevice, uint16_t Credits);
