/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_gaps.h"

int _qapi_BLE_GAPS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t *ServiceID);

int _qapi_BLE_GAPS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_GAPS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

uint32_t _qapi_BLE_GAPS_Query_Number_Attributes(uint8_t TargetID);

int _qapi_BLE_GAPS_Set_Device_Name(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, char *DeviceName);

int _qapi_BLE_GAPS_Query_Device_Name(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, char *NameBuffer);

int _qapi_BLE_GAPS_Set_Device_Appearance(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t DeviceAppearance);

int _qapi_BLE_GAPS_Query_Device_Appearance(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t *DeviceAppearance);

int _qapi_BLE_GAPS_Set_Preferred_Connection_Parameters(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_GAP_Preferred_Connection_Parameters_t *PreferredConnectionParameters);

int _qapi_BLE_GAPS_Query_Preferred_Connection_Parameters(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_GAP_Preferred_Connection_Parameters_t *PreferredConnectionParameters);

int _qapi_BLE_GAPS_Set_Central_Address_Resolution(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_GAP_Central_Address_Resolution_t CentralAddressResolution);

int _qapi_BLE_GAPS_Query_Central_Address_Resolution(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_GAP_Central_Address_Resolution_t *CentralAddressResolution);

int _qapi_BLE_GAPS_Decode_Preferred_Connection_Parameters(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_GAP_Preferred_Connection_Parameters_t *PreferredConnectionParameters);

int _qapi_BLE_GAPS_Decode_Central_Address_Resolution(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_GAP_Central_Address_Resolution_t *CentralAddressResolution);
