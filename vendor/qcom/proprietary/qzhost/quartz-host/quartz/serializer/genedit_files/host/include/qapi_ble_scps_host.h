/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_scps.h"

int _qapi_BLE_SCPS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_SCPS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

int _qapi_BLE_SCPS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_SCPS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_SCPS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

uint32_t _qapi_BLE_SCPS_Query_Number_Attributes(uint8_t TargetID);

int _qapi_BLE_SCPS_Read_Client_Configuration_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t Client_Configuration);

int _qapi_BLE_SCPS_Notify_Scan_Refresh(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, uint8_t ScanRefreshValue);

int _qapi_BLE_SCPS_Format_Scan_Interval_Window(uint8_t TargetID, qapi_BLE_SCPS_Scan_Interval_Window_Data_t *Scan_Interval_Window, uint32_t BufferLength, uint8_t *Buffer);
