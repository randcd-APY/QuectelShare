/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_l2cap_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_otstypes_common.h"

int Mnl_qapi_BLE_OTS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t ServiceFlags, qapi_BLE_OTS_Initialize_Data_t *InitializeData, qapi_BLE_OTS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);
int Mnl_qapi_BLE_OTS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t ServiceFlags, qapi_BLE_OTS_Initialize_Data_t *InitializeData, qapi_BLE_OTS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);
int Mnl_qapi_BLE_OTS_Read_Object_Metadata_Request_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_OTS_Object_Metadata_Type_t Type, qapi_BLE_OTS_Object_Metadata_Data_t *Metadata, uint8_t Offset);
int Mnl_qapi_BLE_OTS_Format_Object_Metadata(uint8_t TargetID, qapi_BLE_OTS_Object_Metadata_Type_t Type, qapi_BLE_OTS_Object_Metadata_Data_t *Metadata, uint16_t BufferLength, uint8_t *Buffer);
int Mnl_qapi_BLE_OTS_Decode_Object_Metadata(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_OTS_Object_Metadata_Type_t Type, qapi_BLE_OTS_Object_Metadata_Data_t *Metadata);
