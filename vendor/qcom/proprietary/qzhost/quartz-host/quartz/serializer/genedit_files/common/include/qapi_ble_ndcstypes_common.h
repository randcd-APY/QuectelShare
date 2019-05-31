/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_NDCSTYPES_COMMON_H__
#define __QAPI_BLE_NDCSTYPES_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_ndcstypes.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatttype_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_NDCS_TIME_WITH_DST_T_MIN_PACKED_SIZE                                                   (1)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_NDCS_Time_With_Dst_t(qapi_BLE_NDCS_Time_With_Dst_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_NDCS_Time_With_Dst_t(PackedBuffer_t *Buffer, qapi_BLE_NDCS_Time_With_Dst_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_NDCS_Time_With_Dst_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_NDCS_Time_With_Dst_t *Structure);

#endif // __QAPI_BLE_NDCSTYPES_COMMON_H__
