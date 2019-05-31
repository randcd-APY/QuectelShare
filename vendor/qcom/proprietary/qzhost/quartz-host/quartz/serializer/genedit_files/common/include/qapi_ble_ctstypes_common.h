/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_CTSTYPES_COMMON_H__
#define __QAPI_BLE_CTSTYPES_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_ctstypes.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatttype_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_CTS_DAY_DATE_TIME_T_MIN_PACKED_SIZE                                                    (1)
#define QAPI_BLE_CTS_EXACT_TIME_T_MIN_PACKED_SIZE                                                       (1)
#define QAPI_BLE_CTS_CURRENT_TIME_T_MIN_PACKED_SIZE                                                     (1)
#define QAPI_BLE_CTS_LOCAL_TIME_INFORMATION_T_MIN_PACKED_SIZE                                           (2)
#define QAPI_BLE_CTS_REFERENCE_TIME_INFORMATION_T_MIN_PACKED_SIZE                                       (4)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_CTS_Day_Date_Time_t(qapi_BLE_CTS_Day_Date_Time_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CTS_Exact_Time_t(qapi_BLE_CTS_Exact_Time_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CTS_Current_Time_t(qapi_BLE_CTS_Current_Time_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CTS_Local_Time_Information_t(qapi_BLE_CTS_Local_Time_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CTS_Reference_Time_Information_t(qapi_BLE_CTS_Reference_Time_Information_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_CTS_Day_Date_Time_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Day_Date_Time_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CTS_Exact_Time_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Exact_Time_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CTS_Current_Time_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Current_Time_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CTS_Local_Time_Information_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Local_Time_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CTS_Reference_Time_Information_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Reference_Time_Information_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_CTS_Day_Date_Time_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Day_Date_Time_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CTS_Exact_Time_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Exact_Time_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CTS_Current_Time_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Current_Time_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CTS_Local_Time_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Local_Time_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CTS_Reference_Time_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Reference_Time_Information_t *Structure);

#endif // __QAPI_BLE_CTSTYPES_COMMON_H__
