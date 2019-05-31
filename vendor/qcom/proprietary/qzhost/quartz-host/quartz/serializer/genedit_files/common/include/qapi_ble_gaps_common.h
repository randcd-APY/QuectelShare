/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_GAPS_COMMON_H__
#define __QAPI_BLE_GAPS_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_gaps.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_gapstypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_GAP_PREFERRED_CONNECTION_PARAMETERS_T_MIN_PACKED_SIZE                                  (8)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_GAP_Preferred_Connection_Parameters_t(qapi_BLE_GAP_Preferred_Connection_Parameters_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_GAP_Preferred_Connection_Parameters_t(PackedBuffer_t *Buffer, qapi_BLE_GAP_Preferred_Connection_Parameters_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_GAP_Preferred_Connection_Parameters_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GAP_Preferred_Connection_Parameters_t *Structure);

#endif // __QAPI_BLE_GAPS_COMMON_H__
