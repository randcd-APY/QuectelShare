/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_OTSTYPES_COMMON_H__
#define __QAPI_BLE_OTSTYPES_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_otstypes.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatttype_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_OTS_FEATURE_T_MIN_PACKED_SIZE                                                          (8)
#define QAPI_BLE_OTS_OBJECT_SIZE_T_MIN_PACKED_SIZE                                                      (8)
#define QAPI_BLE_OTS_UINT48_T_MIN_PACKED_SIZE                                                           (6)
#define QAPI_BLE_OTS_DATE_TIME_RANGE_T_MIN_PACKED_SIZE                                                  (0)
#define QAPI_BLE_OTS_SIZE_RANGE_T_MIN_PACKED_SIZE                                                       (8)
#define QAPI_BLE_OTS_OBJECT_CHANGED_T_MIN_PACKED_SIZE                                                   (1)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_OTS_Feature_t(qapi_BLE_OTS_Feature_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_OTS_Object_Size_t(qapi_BLE_OTS_Object_Size_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_OTS_UINT48_t(qapi_BLE_OTS_UINT48_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_OTS_Date_Time_Range_t(qapi_BLE_OTS_Date_Time_Range_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_OTS_Size_Range_t(qapi_BLE_OTS_Size_Range_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_OTS_Object_Changed_t(qapi_BLE_OTS_Object_Changed_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_OTS_Feature_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Feature_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_OTS_Object_Size_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Object_Size_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_OTS_UINT48_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_UINT48_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_OTS_Date_Time_Range_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Date_Time_Range_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_OTS_Size_Range_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Size_Range_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_OTS_Object_Changed_t(PackedBuffer_t *Buffer, qapi_BLE_OTS_Object_Changed_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_OTS_Feature_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Feature_t *Structure);
SerStatus_t PackedRead_qapi_BLE_OTS_Object_Size_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Object_Size_t *Structure);
SerStatus_t PackedRead_qapi_BLE_OTS_UINT48_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_UINT48_t *Structure);
SerStatus_t PackedRead_qapi_BLE_OTS_Date_Time_Range_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Date_Time_Range_t *Structure);
SerStatus_t PackedRead_qapi_BLE_OTS_Size_Range_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Size_Range_t *Structure);
SerStatus_t PackedRead_qapi_BLE_OTS_Object_Changed_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_OTS_Object_Changed_t *Structure);

#endif // __QAPI_BLE_OTSTYPES_COMMON_H__
