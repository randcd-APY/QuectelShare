/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_lns.h"

int _qapi_BLE_LNS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_LNS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

int _qapi_BLE_LNS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_LNS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_LNS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

unsigned int _qapi_BLE_LNS_Query_Number_Attributes(uint8_t TargetID);

int _qapi_BLE_LNS_Read_Client_Configuration_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t ClientConfiguration);

int _qapi_BLE_LNS_Set_LN_Feature(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t SupportedFeatures);

int _qapi_BLE_LNS_Query_LN_Feature(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t *SupportedFeatures);

int _qapi_BLE_LNS_Notify_Location_And_Speed(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_LNS_Location_Speed_Data_t *LocationAndSpeedData);

int _qapi_BLE_LNS_Set_Position_Quality(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_LNS_Position_Quality_Data_t *Position_Quality);

int _qapi_BLE_LNS_Query_Position_Quality(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_LNS_Position_Quality_Data_t *Position_Quality);

int _qapi_BLE_LNS_Notify_Navigation(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_LNS_Navigation_Data_t *Navigation);

int _qapi_BLE_LNS_LN_Control_Point_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t ErrorCode);

int _qapi_BLE_LNS_Indicate_LN_Control_Point_Result(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_LNS_LNCP_Command_Type_t RequestOpCode, uint8_t ResponseCode);

int _qapi_BLE_LNS_Indicate_Number_Of_Routes(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_LNS_LNCP_Command_Type_t RequestOpCode, uint8_t ResponseCode, uint16_t NumberOfRoutes);

int _qapi_BLE_LNS_Indicate_Name_Of_Route(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_LNS_LNCP_Command_Type_t RequestOpCode, uint8_t ResponseCode, char *NameOfRoute);

int _qapi_BLE_LNS_Format_LN_Control_Point_Command(uint8_t TargetID, qapi_BLE_LNS_LN_Control_Point_Format_Data_t *FormatData, uint32_t *BufferLength, uint8_t *Buffer);

int _qapi_BLE_LNS_Decode_LN_Control_Point_Response(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_LNS_LN_Control_Point_Response_Data_t *LNCPResponseData);

void _qapi_BLE_LNS_Free_LN_Control_Point_Response(uint8_t TargetID, qapi_BLE_LNS_LN_Control_Point_Response_Data_t *LNCPResponseData);

int _qapi_BLE_LNS_Decode_Location_And_Speed(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_LNS_Location_Speed_Data_t *LocationAndSpeedData);

int _qapi_BLE_LNS_Decode_Navigation(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_LNS_Navigation_Data_t *NavigationData);

int _qapi_BLE_LNS_Decode_Position_Quality(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_LNS_Position_Quality_Data_t *PositionQualityData);
