/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_ans.h"

int _qapi_BLE_ANS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_ANS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

int _qapi_BLE_ANS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_ANS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_ANS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

unsigned int _qapi_BLE_ANS_Query_Number_Attributes(uint8_t TargetID);

int _qapi_BLE_ANS_Set_Supported_Categories(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_ANS_Supported_Categories_Type_t SupportedCategoryType, uint16_t SupportedCategoriesMask);

int _qapi_BLE_ANS_Query_Supported_Categories(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_ANS_Supported_Categories_Type_t SupportedCategoryType, uint16_t *SupportedCategoriesMask);

int _qapi_BLE_ANS_Read_Client_Configuration_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, boolean_t NotificationsEnabled);

int _qapi_BLE_ANS_New_Alert_Notification(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_ANS_New_Alert_Data_t *NewAlert);

int _qapi_BLE_ANS_Un_Read_Alert_Status_Notification(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_ANS_Un_Read_Alert_Data_t *UnReadAlert);

qapi_BLE_ANS_New_Alert_Data_t *_qapi_BLE_ANS_Decode_New_Alert_Notification(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value);

void _qapi_BLE_ANS_Free_New_Alert_Data(uint8_t TargetID, qapi_BLE_ANS_New_Alert_Data_t *NewAlertData);

int _qapi_BLE_ANS_Decode_Un_Read_Alert_Status_Notification(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_ANS_Un_Read_Alert_Data_t *UnReadAlert);

int _qapi_BLE_ANS_Decode_Supported_Categories(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, uint16_t *SupportedCategoriesMask);

int _qapi_BLE_ANS_Format_Control_Point_Command(uint8_t TargetID, qapi_BLE_ANS_Control_Point_Command_Value_t *CommandBuffer, qapi_BLE_ANS_Control_Point_Command_t Command, qapi_BLE_ANS_Category_Identification_t CommandCategory);
