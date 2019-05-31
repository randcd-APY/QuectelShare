/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_BASETYPES_COMMON_H__
#define __QAPI_BLE_BASETYPES_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_bttypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_NONALIGNEDWORD_T_MIN_PACKED_SIZE                                                       (2)
#define QAPI_BLE_NONALIGNEDDWORD_T_MIN_PACKED_SIZE                                                      (4)
#define QAPI_BLE_NONALIGNEDQWORD_T_MIN_PACKED_SIZE                                                      (8)
#define QAPI_BLE_NONALIGNEDSWORD_T_MIN_PACKED_SIZE                                                      (2)
#define QAPI_BLE_NONALIGNEDSDWORD_T_MIN_PACKED_SIZE                                                     (4)
#define QAPI_BLE_NONALIGNEDSQWORD_T_MIN_PACKED_SIZE                                                     (8)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_NonAlignedWord_t(qapi_BLE_NonAlignedWord_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_NonAlignedDWord_t(qapi_BLE_NonAlignedDWord_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_NonAlignedQWord_t(qapi_BLE_NonAlignedQWord_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_NonAlignedSWord_t(qapi_BLE_NonAlignedSWord_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_NonAlignedSDWord_t(qapi_BLE_NonAlignedSDWord_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_NonAlignedSQWord_t(qapi_BLE_NonAlignedSQWord_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_NonAlignedWord_t(PackedBuffer_t *Buffer, qapi_BLE_NonAlignedWord_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_NonAlignedDWord_t(PackedBuffer_t *Buffer, qapi_BLE_NonAlignedDWord_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_NonAlignedQWord_t(PackedBuffer_t *Buffer, qapi_BLE_NonAlignedQWord_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_NonAlignedSWord_t(PackedBuffer_t *Buffer, qapi_BLE_NonAlignedSWord_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_NonAlignedSDWord_t(PackedBuffer_t *Buffer, qapi_BLE_NonAlignedSDWord_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_NonAlignedSQWord_t(PackedBuffer_t *Buffer, qapi_BLE_NonAlignedSQWord_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_NonAlignedWord_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_NonAlignedWord_t *Structure);
SerStatus_t PackedRead_qapi_BLE_NonAlignedDWord_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_NonAlignedDWord_t *Structure);
SerStatus_t PackedRead_qapi_BLE_NonAlignedQWord_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_NonAlignedQWord_t *Structure);
SerStatus_t PackedRead_qapi_BLE_NonAlignedSWord_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_NonAlignedSWord_t *Structure);
SerStatus_t PackedRead_qapi_BLE_NonAlignedSDWord_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_NonAlignedSDWord_t *Structure);
SerStatus_t PackedRead_qapi_BLE_NonAlignedSQWord_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_NonAlignedSQWord_t *Structure);

#endif // __QAPI_BLE_BASETYPES_COMMON_H__
