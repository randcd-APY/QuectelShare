/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_l2cap.h"

int _qapi_BLE_L2CA_Register_LE_PSM(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t LE_PSM, qapi_BLE_L2CA_Event_Callback_t L2CA_Event_Callback, uint32_t CallbackParameter);

int _qapi_BLE_L2CA_Un_Register_LE_PSM(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t L2CAP_PSMID);

int _qapi_BLE_L2CA_Register_Fixed_Channel(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t FixedChannel, void *ChannelParameters, qapi_BLE_L2CA_Event_Callback_t L2CA_Event_Callback, uint32_t CallbackParameter);

int _qapi_BLE_L2CA_Un_Register_Fixed_Channel(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t FCID);

int _qapi_BLE_L2CA_LE_Connect_Request(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BD_ADDR_t BD_ADDR, uint16_t LE_PSM, qapi_BLE_L2CA_LE_Channel_Parameters_t *ChannelParameters, qapi_BLE_L2CA_Event_Callback_t L2CA_Event_Callback, uint32_t CallbackParameter);

int _qapi_BLE_L2CA_LE_Connect_Response(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BD_ADDR_t BD_ADDR, uint8_t Identifier, uint16_t LCID, uint16_t Result, qapi_BLE_L2CA_LE_Channel_Parameters_t *ChannelParameters);

int _qapi_BLE_L2CA_LE_Disconnect_Request(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t LCID);

int _qapi_BLE_L2CA_LE_Disconnect_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t LCID);

int _qapi_BLE_L2CA_Fixed_Channel_Data_Write(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BD_ADDR_t BD_ADDR, uint16_t FCID, uint16_t Data_Length, uint8_t *Data);

int _qapi_BLE_L2CA_Enhanced_Fixed_Channel_Data_Write(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BD_ADDR_t BD_ADDR, uint16_t FCID, qapi_BLE_L2CA_Queueing_Parameters_t *QueueingParameters, uint16_t Data_Length, uint8_t *Data);

int _qapi_BLE_L2CA_Enhanced_Dynamic_Channel_Data_Write(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t LCID, qapi_BLE_L2CA_Queueing_Parameters_t *QueueingParameters, uint16_t Data_Length, uint8_t *Data);

int _qapi_BLE_L2CA_LE_Flush_Channel_Data(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t CID);

int _qapi_BLE_L2CA_LE_Grant_Credits(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t CID, uint16_t CreditsToGrant);

int _qapi_BLE_L2CA_Connection_Parameter_Update_Request(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BD_ADDR_t BD_ADDR, uint16_t IntervalMin, uint16_t IntervalMax, uint16_t SlaveLatency, uint16_t TimeoutMultiplier);

int _qapi_BLE_L2CA_Connection_Parameter_Update_Response(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_BD_ADDR_t BD_ADDR, uint16_t Result);
