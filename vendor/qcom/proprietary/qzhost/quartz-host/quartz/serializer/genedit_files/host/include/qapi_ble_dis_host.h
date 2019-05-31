/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_dis.h"

int _qapi_BLE_DIS_Initialize_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t *ServiceID);

int _qapi_BLE_DIS_Initialize_Service_Handle_Range(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

int _qapi_BLE_DIS_Cleanup_Service(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID);

uint32_t _qapi_BLE_DIS_Query_Number_Attributes(uint8_t TargetID);

int _qapi_BLE_DIS_Set_Manufacturer_Name(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, char *ManufacturerName);

int _qapi_BLE_DIS_Query_Manufacturer_Name(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, char *ManufacturerName);

int _qapi_BLE_DIS_Set_Model_Number(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, char *ModelNumber);

int _qapi_BLE_DIS_Query_Model_Number(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, char *ModelNumber);

int _qapi_BLE_DIS_Set_Serial_Number(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, char *SerialNumber);

int _qapi_BLE_DIS_Query_Serial_Number(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, char *SerialNumber);

int _qapi_BLE_DIS_Set_Hardware_Revision(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, char *Hardware_Revision);

int _qapi_BLE_DIS_Query_Hardware_Revision(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, char *Hardware_Revision);

int _qapi_BLE_DIS_Set_Firmware_Revision(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, char *FirmwareRevision);

int _qapi_BLE_DIS_Query_Firmware_Revision(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, char *FirmwareRevision);

int _qapi_BLE_DIS_Set_Software_Revision(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, char *SoftwareRevision);

int _qapi_BLE_DIS_Query_Software_Revision(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, char *SoftwareRevision);

int _qapi_BLE_DIS_Set_System_ID(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_DIS_System_ID_Value_t *SystemID);

int _qapi_BLE_DIS_Query_System_ID(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_DIS_System_ID_Value_t *SystemID);

int _qapi_BLE_DIS_Set_IEEE_Certification_Data(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t Length, uint8_t *IEEE_Certification_Data);

int _qapi_BLE_DIS_Query_IEEE_Certification_Data(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t *IEEE_Certification_Data);

int _qapi_BLE_DIS_Set_PNP_ID(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_DIS_PNP_ID_Data_t *PNP_ID_Data);

int _qapi_BLE_DIS_Query_PNP_ID(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_DIS_PNP_ID_Data_t *PNP_ID_Data);

int _qapi_BLE_DIS_Decode_PNP_ID(uint8_t TargetID, uint32_t ValueLength, uint8_t *Value, qapi_BLE_DIS_PNP_ID_Data_t *PNP_ID_Data);
