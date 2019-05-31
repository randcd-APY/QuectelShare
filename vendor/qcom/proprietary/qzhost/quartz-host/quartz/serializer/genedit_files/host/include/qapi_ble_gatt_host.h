/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_gatt.h"

int _qapi_BLE_GATT_Initialize(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t Flags, qapi_BLE_GATT_Connection_Event_Callback_t ConnectionEventCallback, uint32_t CallbackParameter);

int _qapi_BLE_GATT_Cleanup(uint8_t TargetID, uint32_t BluetoothStackID);

int _qapi_BLE_GATT_Register_Connection_Events(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_GATT_Connection_Event_Callback_t ConnectionEventCallback, uint32_t CallbackParameter);

int _qapi_BLE_GATT_Un_Register_Connection_Events(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t EventCallbackID);

int _qapi_BLE_GATT_Register_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint8_t ServiceFlags, uint32_t NumberOfServiceAttributeEntries, qapi_BLE_GATT_Service_Attribute_Entry_t *ServiceTable, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleGroupResult, qapi_BLE_GATT_Server_Event_Callback_t ServerEventCallback, uint32_t CallbackParameter);

void _qapi_BLE_GATT_Un_Register_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ServiceID);

int _qapi_BLE_GATT_Read_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, uint32_t DataLength, uint8_t *Data);

int _qapi_BLE_GATT_Write_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID);

int _qapi_BLE_GATT_Execute_Write_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID);

int _qapi_BLE_GATT_Error_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, uint16_t AttributeOffset, uint8_t ErrorCode);

int _qapi_BLE_GATT_Handle_Value_Indication(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ServiceID, uint32_t ConnectionID, uint16_t AttributeOffset, uint16_t AttributeValueLength, uint8_t *AttributeValue);

int _qapi_BLE_GATT_Handle_Value_Notification(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ServiceID, uint32_t ConnectionID, uint16_t AttributeOffset, uint16_t AttributeValueLength, uint8_t *AttributeValue);

boolean_t _qapi_BLE_GATT_Verify_Signature(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ServiceID, uint16_t AttributeOffset, uint32_t AttributeValueLength, uint8_t *AttributeValue, qapi_BLE_ATT_Authentication_Signature_t *ReceivedSignature, qapi_BLE_Encryption_Key_t *CSRK);

int _qapi_BLE_GATT_Service_Changed_Read_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, qapi_BLE_GATT_Service_Changed_Data_t *Service_Changed_Data);

int _qapi_BLE_GATT_Service_Changed_CCCD_Read_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, uint16_t CCCD);

int _qapi_BLE_GATT_Service_Changed_Indication(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, qapi_BLE_GATT_Service_Changed_Data_t *Service_Changed_Data);

int _qapi_BLE_GATT_Exchange_MTU_Request(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t RequestedMTU, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter);

int _qapi_BLE_GATT_Discover_Services(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t StartingHandle, uint16_t EndingHandle, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter);

int _qapi_BLE_GATT_Discover_Services_By_UUID(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t StartingHandle, uint16_t EndingHandle, qapi_BLE_GATT_UUID_t *UUID, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter);

int _qapi_BLE_GATT_Discover_Included_Services(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t ServiceStartingHandle, uint16_t ServiceEndingHandle, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter);

int _qapi_BLE_GATT_Discover_Characteristics(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t ServiceStartingHandle, uint16_t ServiceEndingHandle, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter);

int _qapi_BLE_GATT_Discover_Characteristic_Descriptors(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t CharacteristicStartingHandle, uint16_t CharacteristicEndingHandle, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter);

int _qapi_BLE_GATT_Read_Value_Request(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t AttributeHandle, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter);

int _qapi_BLE_GATT_Read_Long_Value_Request(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t AttributeHandle, uint16_t AttributeOffset, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter);

int _qapi_BLE_GATT_Read_Value_By_UUID_Request(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, qapi_BLE_GATT_UUID_t *AttributeUUID, uint16_t ServiceStartHandle, uint16_t ServiceEndHandle, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter);

int _qapi_BLE_GATT_Read_Multiple_Values_Request(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t NumberOfHandles, uint16_t *AttributeHandleList, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter);

int _qapi_BLE_GATT_Write_Request(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t AttributeHandle, uint16_t AttributeLength, void *AttributeValue, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter);

int _qapi_BLE_GATT_Write_Without_Response_Request(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t AttributeHandle, uint16_t AttributeLength, void *AttributeValue);

int _qapi_BLE_GATT_Signed_Write_Without_Response(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t AttributeHandle, uint32_t SignCounter, uint16_t AttributeLength, void *AttributeValue, qapi_BLE_Encryption_Key_t *CSRK);

int _qapi_BLE_GATT_Prepare_Write_Request(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t AttributeHandle, uint16_t AttributeLength, uint16_t AttributeValueOffset, void *AttributeValue, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter);

int _qapi_BLE_GATT_Execute_Write_Request(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, boolean_t CancelWrite, qapi_BLE_GATT_Client_Event_Callback_t ClientEventCallback, uint32_t CallbackParameter);

int _qapi_BLE_GATT_Handle_Value_Confirmation(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint32_t TransactionID);

int _qapi_BLE_GATT_Start_Service_Discovery(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint32_t NumberOfUUID, qapi_BLE_GATT_UUID_t *UUIDList, qapi_BLE_GATT_Service_Discovery_Event_Callback_t ServiceDiscoveryCallback, uint32_t CallbackParameter);

int _qapi_BLE_GATT_Start_Service_Discovery_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, qapi_BLE_GATT_Attribute_Handle_Group_t *DiscoveryHandleRange, uint32_t NumberOfUUID, qapi_BLE_GATT_UUID_t *UUIDList, qapi_BLE_GATT_Service_Discovery_Event_Callback_t ServiceDiscoveryCallback, uint32_t CallbackParameter);

int _qapi_BLE_GATT_Stop_Service_Discovery(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID);

int _qapi_BLE_GATT_Cancel_Transaction(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID);

int _qapi_BLE_GATT_Query_Maximum_Supported_MTU(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t *MTU);

int _qapi_BLE_GATT_Change_Maximum_Supported_MTU(uint8_t TargetID, uint32_t BluetoothStackID, uint16_t MTU);

int _qapi_BLE_GATT_Query_Connection_MTU(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t ConnectionID, uint16_t *MTU);

int _qapi_BLE_GATT_Query_Connection_ID(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_GATT_Connection_Type_t ConnectionType, qapi_BLE_BD_ADDR_t BD_ADDR, uint32_t *ConnectionID);

int _qapi_BLE_GATT_Query_Transaction_Opcode(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t *Opcode);

int _qapi_BLE_GATT_Set_Queuing_Parameters(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t MaximumNumberDataPackets, uint32_t QueuedDataPacketsThreshold, boolean_t DiscardOldest);

int _qapi_BLE_GATT_Get_Queuing_Parameters(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t *MaximumNumberDataPackets, uint32_t *QueuedDataPacketsThreshold, boolean_t *DiscardOldest);

boolean_t _qapi_BLE_GATT_Query_Service_Range_Availability(uint8_t TargetID, uint32_t BluetoothStackID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleGroup);
