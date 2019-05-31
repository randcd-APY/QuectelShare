/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_HRSTYPES_COMMON_H__
#define __QAPI_BLE_HRSTYPES_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_hrstypes.h"
#include "qapi_ble_bttypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_HEADER_T_MIN_PACKED_SIZE                                    (1)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_HRS_Heart_Rate_Measurement_Header_t(qapi_BLE_HRS_Heart_Rate_Measurement_Header_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_HRS_Heart_Rate_Measurement_Header_t(PackedBuffer_t *Buffer, qapi_BLE_HRS_Heart_Rate_Measurement_Header_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_HRS_Heart_Rate_Measurement_Header_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HRS_Heart_Rate_Measurement_Header_t *Structure);

#endif // __QAPI_BLE_HRSTYPES_COMMON_H__
