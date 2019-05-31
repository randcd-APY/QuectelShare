/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_bms.h"

int _qapi_BLE_BMS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t Flags, qapi_BLE_BMS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

int _qapi_BLE_BMS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t Flags, qapi_BLE_BMS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_BMS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

unsigned int _qapi_BLE_BMS_Query_Number_Attributes(uint8_t TargetID);

int _qapi_BLE_BMS_Set_BM_Features(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t SupportedFeatures);

int _qapi_BLE_BMS_Query_BM_Features(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t *SupportedFeatures);

int _qapi_BLE_BMS_BM_Control_Point_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t ResponseCode);

int _qapi_BLE_BMS_Format_BM_Control_Point_Command(uint8_t TargetID, qapi_BLE_BMS_BM_Control_Point_Format_Data_t *FormatData, uint32_t *BufferLength, uint8_t *Buffer);
