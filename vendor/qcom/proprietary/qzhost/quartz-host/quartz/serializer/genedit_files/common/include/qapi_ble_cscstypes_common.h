/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_CSCSTYPES_COMMON_H__
#define __QAPI_BLE_CSCSTYPES_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_cscstypes.h"
#include "qapi_ble_bttypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_CSCS_WHEEL_REVOLUTION_T_MIN_PACKED_SIZE                                                (6)
#define QAPI_BLE_CSCS_CRANK_REVOLUTION_T_MIN_PACKED_SIZE                                                (4)
#define QAPI_BLE_CSCS_SCCP_RESPONSE_CODE_T_MIN_PACKED_SIZE                                              (2)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_CSCS_Wheel_Revolution_t(qapi_BLE_CSCS_Wheel_Revolution_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CSCS_Crank_Revolution_t(qapi_BLE_CSCS_Crank_Revolution_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CSCS_SCCP_Response_Code_t(qapi_BLE_CSCS_SCCP_Response_Code_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_CSCS_Wheel_Revolution_t(PackedBuffer_t *Buffer, qapi_BLE_CSCS_Wheel_Revolution_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CSCS_Crank_Revolution_t(PackedBuffer_t *Buffer, qapi_BLE_CSCS_Crank_Revolution_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CSCS_SCCP_Response_Code_t(PackedBuffer_t *Buffer, qapi_BLE_CSCS_SCCP_Response_Code_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_CSCS_Wheel_Revolution_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CSCS_Wheel_Revolution_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CSCS_Crank_Revolution_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CSCS_Crank_Revolution_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CSCS_SCCP_Response_Code_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CSCS_SCCP_Response_Code_t *Structure);

#endif // __QAPI_BLE_CSCSTYPES_COMMON_H__
