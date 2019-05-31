/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_CGMSTYPES_COMMON_H__
#define __QAPI_BLE_CGMSTYPES_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_cgmstypes.h"
#include "qapi_ble_bttypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_CGMS_TIME_OFFSET_RANGE_T_MIN_PACKED_SIZE                                               (4)
#define QAPI_BLE_CGMS_RACP_RESPONSE_CODE_T_MIN_PACKED_SIZE                                              (2)
#define QAPI_BLE_CGMS_CALIBRATION_RECORD_T_MIN_PACKED_SIZE                                              (10)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_CGMS_Time_Offset_Range_t(qapi_BLE_CGMS_Time_Offset_Range_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CGMS_RACP_Response_Code_t(qapi_BLE_CGMS_RACP_Response_Code_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CGMS_Calibration_Record_t(qapi_BLE_CGMS_Calibration_Record_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_CGMS_Time_Offset_Range_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_Time_Offset_Range_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CGMS_RACP_Response_Code_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_RACP_Response_Code_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CGMS_Calibration_Record_t(PackedBuffer_t *Buffer, qapi_BLE_CGMS_Calibration_Record_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_CGMS_Time_Offset_Range_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_Time_Offset_Range_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CGMS_RACP_Response_Code_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_RACP_Response_Code_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CGMS_Calibration_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CGMS_Calibration_Record_t *Structure);

#endif // __QAPI_BLE_CGMSTYPES_COMMON_H__
