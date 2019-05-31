/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_distypes_common.h"

int Mnl_qapi_BLE_DIS_Query_Manufacturer_Name(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, char *ManufacturerName);
int Mnl_qapi_BLE_DIS_Query_Model_Number(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, char *ModelNumber);
int Mnl_qapi_BLE_DIS_Query_Serial_Number(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, char *SerialNumber);
int Mnl_qapi_BLE_DIS_Query_Hardware_Revision(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, char *Hardware_Revision);
int Mnl_qapi_BLE_DIS_Query_Firmware_Revision(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, char *FirmwareRevision);
int Mnl_qapi_BLE_DIS_Query_Software_Revision(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, char *SoftwareRevision);
int Mnl_qapi_BLE_DIS_Query_IEEE_Certification_Data(uint8_t TargetID, uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t *IEEE_Certification_Data);