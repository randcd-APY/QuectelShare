/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_HTSTYPES_COMMON_H__
#define __QAPI_BLE_HTSTYPES_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_htstypes.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatttype_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_HTS_TEMPERATURE_DATA_T_MIN_PACKED_SIZE                                                 (4)
#define QAPI_BLE_HTS_TEMPERATURE_MEASUREMENT_HEADER_T_MIN_PACKED_SIZE                                   (1)
#define QAPI_BLE_HTS_TEMPERATURE_MEASUREMENT_WITH_TIME_STAMP_T_MIN_PACKED_SIZE                          (1)
#define QAPI_BLE_HTS_TEMPERATURE_MEASUREMENT_WITHOUT_TIME_STAMP_T_MIN_PACKED_SIZE                       (1)
#define QAPI_BLE_HTS_VALID_RANGE_T_MIN_PACKED_SIZE                                                      (4)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_HTS_Temperature_Data_t(qapi_BLE_HTS_Temperature_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HTS_Temperature_Measurement_Header_t(qapi_BLE_HTS_Temperature_Measurement_Header_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HTS_Temperature_Measurement_With_Time_Stamp_t(qapi_BLE_HTS_Temperature_Measurement_With_Time_Stamp_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HTS_Temperature_Measurement_Without_Time_Stamp_t(qapi_BLE_HTS_Temperature_Measurement_Without_Time_Stamp_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HTS_Valid_Range_t(qapi_BLE_HTS_Valid_Range_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_HTS_Temperature_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HTS_Temperature_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HTS_Temperature_Measurement_Header_t(PackedBuffer_t *Buffer, qapi_BLE_HTS_Temperature_Measurement_Header_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HTS_Temperature_Measurement_With_Time_Stamp_t(PackedBuffer_t *Buffer, qapi_BLE_HTS_Temperature_Measurement_With_Time_Stamp_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HTS_Temperature_Measurement_Without_Time_Stamp_t(PackedBuffer_t *Buffer, qapi_BLE_HTS_Temperature_Measurement_Without_Time_Stamp_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HTS_Valid_Range_t(PackedBuffer_t *Buffer, qapi_BLE_HTS_Valid_Range_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_HTS_Temperature_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HTS_Temperature_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HTS_Temperature_Measurement_Header_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HTS_Temperature_Measurement_Header_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HTS_Temperature_Measurement_With_Time_Stamp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HTS_Temperature_Measurement_With_Time_Stamp_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HTS_Temperature_Measurement_Without_Time_Stamp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HTS_Temperature_Measurement_Without_Time_Stamp_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HTS_Valid_Range_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HTS_Valid_Range_t *Structure);

#endif // __QAPI_BLE_HTSTYPES_COMMON_H__
