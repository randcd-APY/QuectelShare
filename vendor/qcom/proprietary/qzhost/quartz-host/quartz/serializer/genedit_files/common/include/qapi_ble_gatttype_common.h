/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_GATTTYPE_COMMON_H__
#define __QAPI_BLE_GATTTYPE_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_gatttype.h"
#include "qapi_ble_bttypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_GATT_PRIMARY_SERVICE_BY_UUID_ENTRY_T_MIN_PACKED_SIZE                                   (4)
#define QAPI_BLE_GATT_SERVICE_CHANGED_CHARACTERISTIC_VALUE_T_MIN_PACKED_SIZE                            (4)
#define QAPI_BLE_GATT_DATE_TIME_CHARACTERISTIC_T_MIN_PACKED_SIZE                                        (7)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_GATT_Primary_Service_By_UUID_Entry_t(qapi_BLE_GATT_Primary_Service_By_UUID_Entry_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Service_Changed_Characteristic_Value_t(qapi_BLE_GATT_Service_Changed_Characteristic_Value_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GATT_Date_Time_Characteristic_t(qapi_BLE_GATT_Date_Time_Characteristic_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_GATT_Primary_Service_By_UUID_Entry_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Primary_Service_By_UUID_Entry_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Service_Changed_Characteristic_Value_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Service_Changed_Characteristic_Value_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GATT_Date_Time_Characteristic_t(PackedBuffer_t *Buffer, qapi_BLE_GATT_Date_Time_Characteristic_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_GATT_Primary_Service_By_UUID_Entry_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Primary_Service_By_UUID_Entry_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Service_Changed_Characteristic_Value_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Service_Changed_Characteristic_Value_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GATT_Date_Time_Characteristic_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GATT_Date_Time_Characteristic_t *Structure);

#endif // __QAPI_BLE_GATTTYPE_COMMON_H__
