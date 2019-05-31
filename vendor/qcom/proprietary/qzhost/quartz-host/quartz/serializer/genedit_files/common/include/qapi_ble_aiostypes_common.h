/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_AIOSTYPES_COMMON_H__
#define __QAPI_BLE_AIOSTYPES_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_aiostypes.h"
#include "qapi_ble_bttypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_AIOS_PRESENTATION_FORMAT_T_MIN_PACKED_SIZE                                             (7)
#define QAPI_BLE_AIOS_UINT24_T_MIN_PACKED_SIZE                                                          (3)
#define QAPI_BLE_AIOS_VALID_RANGE_T_MIN_PACKED_SIZE                                                     (4)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_AIOS_Presentation_Format_t(qapi_BLE_AIOS_Presentation_Format_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_AIOS_UINT24_t(qapi_BLE_AIOS_UINT24_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_AIOS_Valid_Range_t(qapi_BLE_AIOS_Valid_Range_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_AIOS_Presentation_Format_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Presentation_Format_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_AIOS_UINT24_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_UINT24_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_AIOS_Valid_Range_t(PackedBuffer_t *Buffer, qapi_BLE_AIOS_Valid_Range_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_AIOS_Presentation_Format_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Presentation_Format_t *Structure);
SerStatus_t PackedRead_qapi_BLE_AIOS_UINT24_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_UINT24_t *Structure);
SerStatus_t PackedRead_qapi_BLE_AIOS_Valid_Range_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_AIOS_Valid_Range_t *Structure);

#endif // __QAPI_BLE_AIOSTYPES_COMMON_H__
