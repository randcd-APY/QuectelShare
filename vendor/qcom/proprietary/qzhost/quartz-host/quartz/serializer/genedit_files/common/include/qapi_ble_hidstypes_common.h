/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_HIDSTYPES_COMMON_H__
#define __QAPI_BLE_HIDSTYPES_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_hidstypes.h"
#include "qapi_ble_bttypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_HIDS_HID_INFORMATION_T_MIN_PACKED_SIZE                                                 (4)
#define QAPI_BLE_HIDS_REPORT_REFERENCE_T_MIN_PACKED_SIZE                                                (2)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_HIDS_HID_Information_t(qapi_BLE_HIDS_HID_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HIDS_Report_Reference_t(qapi_BLE_HIDS_Report_Reference_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_HIDS_HID_Information_t(PackedBuffer_t *Buffer, qapi_BLE_HIDS_HID_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HIDS_Report_Reference_t(PackedBuffer_t *Buffer, qapi_BLE_HIDS_Report_Reference_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_HIDS_HID_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HIDS_HID_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HIDS_Report_Reference_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HIDS_Report_Reference_t *Structure);

#endif // __QAPI_BLE_HIDSTYPES_COMMON_H__
