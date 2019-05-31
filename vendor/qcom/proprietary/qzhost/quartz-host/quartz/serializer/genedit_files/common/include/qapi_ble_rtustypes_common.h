/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_RTUSTYPES_COMMON_H__
#define __QAPI_BLE_RTUSTYPES_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_rtustypes.h"
#include "qapi_ble_bttypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_RTUS_TIME_UPDATE_STATE_T_MIN_PACKED_SIZE                                               (2)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_RTUS_Time_Update_State_t(qapi_BLE_RTUS_Time_Update_State_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_RTUS_Time_Update_State_t(PackedBuffer_t *Buffer, qapi_BLE_RTUS_Time_Update_State_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_RTUS_Time_Update_State_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_RTUS_Time_Update_State_t *Structure);

#endif // __QAPI_BLE_RTUSTYPES_COMMON_H__
