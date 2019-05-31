/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_DIS_COMMON_H__
#define __QAPI_BLE_DIS_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_dis.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_distypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_DIS_PNP_ID_DATA_T_MIN_PACKED_SIZE                                                      (7)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_DIS_PNP_ID_Data_t(qapi_BLE_DIS_PNP_ID_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_DIS_PNP_ID_Data_t(PackedBuffer_t *Buffer, qapi_BLE_DIS_PNP_ID_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_DIS_PNP_ID_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_DIS_PNP_ID_Data_t *Structure);

#endif // __QAPI_BLE_DIS_COMMON_H__
