/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_UDSTYPES_COMMON_H__
#define __QAPI_BLE_UDSTYPES_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_udstypes.h"
#include "qapi_ble_bttypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_UDS_DATE_T_MIN_PACKED_SIZE                                                             (4)
#define QAPI_BLE_UDS_FIVE_ZONE_HEART_RATE_LIMITS_T_MIN_PACKED_SIZE                                      (4)
#define QAPI_BLE_UDS_THREE_ZONE_HEART_RATE_LIMITS_T_MIN_PACKED_SIZE                                     (2)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_UDS_Date_t(qapi_BLE_UDS_Date_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_t(qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_t(qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_UDS_Date_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Date_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_UDS_Date_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Date_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_t *Structure);

#endif // __QAPI_BLE_UDSTYPES_COMMON_H__
