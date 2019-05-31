/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_plxs.h"

int _qapi_BLE_PLXS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_PLXS_Initialize_Data_t *InitializeData, qapi_BLE_PLXS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

int _qapi_BLE_PLXS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_PLXS_Initialize_Data_t *InitializeData, qapi_BLE_PLXS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_PLXS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

unsigned int _qapi_BLE_PLXS_Query_Number_Attributes(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

int _qapi_BLE_PLXS_Read_Features_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_PLXS_Features_Data_t *Features);

int _qapi_BLE_PLXS_Indicate_Spot_Check_Measurement(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_PLXS_Spot_Check_Measurement_Data_t *MeasurementData);

int _qapi_BLE_PLXS_Notify_Continuous_Measurement(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_PLXS_Continuous_Measurement_Data_t *MeasurementData);

int _qapi_BLE_PLXS_RACP_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode);

int _qapi_BLE_PLXS_Indicate_RACP_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_PLXS_RACP_Response_Data_t *ResponseData);

int _qapi_BLE_PLXS_Read_CCCD_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_PLXS_CCCD_Type_t Type, uint16_t Configuration);

int _qapi_BLE_PLXS_Write_CCCD_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_PLXS_CCCD_Type_t Type);

int _qapi_BLE_PLXS_Decode_Features(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_PLXS_Features_Data_t *Features);

int _qapi_BLE_PLXS_Decode_Spot_Check_Measurement(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_PLXS_Spot_Check_Measurement_Data_t *MeasurementData);

int _qapi_BLE_PLXS_Decode_Continuous_Measurement(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_PLXS_Continuous_Measurement_Data_t *MeasurementData);

int _qapi_BLE_PLXS_Decode_RACP_Response(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_PLXS_RACP_Response_Data_t *ResponseData);
