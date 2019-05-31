/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_ndcs.h"

int _qapi_BLE_NDCS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_NDCS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

int _qapi_BLE_NDCS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_NDCS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_NDCS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

unsigned int _qapi_BLE_NDCS_Query_Number_Attributes(uint8_t TargetID);

int _qapi_BLE_NDCS_Time_With_DST_Read_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, qapi_BLE_NDCS_Time_With_Dst_Data_t *Next_Dst_Change_Time);

int _qapi_BLE_NDCS_Time_With_DST_Read_Request_Error_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t ErrorCode);

int _qapi_BLE_NDCS_Decode_Time_With_Dst(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_NDCS_Time_With_Dst_Data_t *Next_Dst_Change_Time);
