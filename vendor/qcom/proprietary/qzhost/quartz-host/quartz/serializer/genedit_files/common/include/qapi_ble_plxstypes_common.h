/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_PLXSTYPES_COMMON_H__
#define __QAPI_BLE_PLXSTYPES_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_plxstypes.h"
#include "qapi_ble_bttypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_PLXS_INT24_T_MIN_PACKED_SIZE                                                           (3)
#define QAPI_BLE_PLXS_PLX_FEATURES_T_MIN_PACKED_SIZE                                                    (4)
#define QAPI_BLE_PLXS_RACP_REQUEST_T_MIN_PACKED_SIZE                                                    (2)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_PLXS_INT24_t(qapi_BLE_PLXS_INT24_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_PLXS_PLX_Features_t(qapi_BLE_PLXS_PLX_Features_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_PLXS_RACP_Request_t(qapi_BLE_PLXS_RACP_Request_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_PLXS_INT24_t(PackedBuffer_t *Buffer, qapi_BLE_PLXS_INT24_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_PLXS_PLX_Features_t(PackedBuffer_t *Buffer, qapi_BLE_PLXS_PLX_Features_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_PLXS_RACP_Request_t(PackedBuffer_t *Buffer, qapi_BLE_PLXS_RACP_Request_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_PLXS_INT24_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_PLXS_INT24_t *Structure);
SerStatus_t PackedRead_qapi_BLE_PLXS_PLX_Features_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_PLXS_PLX_Features_t *Structure);
SerStatus_t PackedRead_qapi_BLE_PLXS_RACP_Request_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_PLXS_RACP_Request_t *Structure);

#endif // __QAPI_BLE_PLXSTYPES_COMMON_H__
