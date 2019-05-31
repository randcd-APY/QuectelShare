/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_SCPSTYPES_COMMON_H__
#define __QAPI_BLE_SCPSTYPES_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_scpstypes.h"
#include "qapi_ble_bttypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_SCPS_SCAN_INTERVAL_WINDOW_T_MIN_PACKED_SIZE                                            (4)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_SCPS_Scan_Interval_Window_t(qapi_BLE_SCPS_Scan_Interval_Window_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_SCPS_Scan_Interval_Window_t(PackedBuffer_t *Buffer, qapi_BLE_SCPS_Scan_Interval_Window_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_SCPS_Scan_Interval_Window_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_SCPS_Scan_Interval_Window_t *Structure);

#endif // __QAPI_BLE_SCPSTYPES_COMMON_H__
