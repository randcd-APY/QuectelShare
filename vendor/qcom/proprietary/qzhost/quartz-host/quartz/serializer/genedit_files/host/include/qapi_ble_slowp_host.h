/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_slowp.h"

int _qapi_BLE_SLoWP_Initialize(uint8_t TargetID, uint32_t BluetoothStackID);

int _qapi_BLE_SLoWP_Cleanup(uint8_t TargetID, uint32_t BluetoothStackID);

int _qapi_BLE_SLoWP_Initialize_Node(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_L2CA_LE_Channel_Parameters_t *ChannelParameters, qapi_BLE_L2CA_Queueing_Parameters_t *QueueingParameters, qapi_BLE_SLoWP_Event_Callback_t EventCallback, uint32_t CallbackParameter, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_SLoWP_Cleanup_Node(uint8_t TargetID, uint32_t BluetoothStackID);

int _qapi_BLE_SLoWP_Open_Connection_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BD_ADDR_t BD_ADDR, boolean_t AcceptConnection);

int _qapi_BLE_SLoWP_Get_Node_Connection_Mode(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_IPSP_Node_Connection_Mode_t *ConnectionMode);

int _qapi_BLE_SLoWP_Set_Node_Connection_Mode(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_IPSP_Node_Connection_Mode_t ConnectionMode);

int _qapi_BLE_SLoWP_Connect_Remote_Node(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BD_ADDR_t BD_ADDR, qapi_BLE_L2CA_LE_Channel_Parameters_t *ChannelParameters, qapi_BLE_L2CA_Queueing_Parameters_t *QueueingParameters, qapi_BLE_SLoWP_Event_Callback_t EventCallback, uint32_t CallbackParameter);

int _qapi_BLE_SLoWP_Close_Connection(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BD_ADDR_t BD_ADDR);

int _qapi_BLE_SLoWP_Set_Queueing_Parameters(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BD_ADDR_t BD_ADDR, qapi_BLE_L2CA_Queueing_Parameters_t *QueueingParameters);
