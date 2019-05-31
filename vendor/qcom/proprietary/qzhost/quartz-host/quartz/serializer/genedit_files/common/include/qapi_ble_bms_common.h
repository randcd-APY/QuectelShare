/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_BMS_COMMON_H__
#define __QAPI_BLE_BMS_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_bms.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_bmstypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_BMS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE                                               (4)
#define QAPI_BLE_BMS_BM_CONTROL_POINT_FORMAT_DATA_T_MIN_PACKED_SIZE                                     ((8) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_BMS_BM_CONTROL_POINT_COMMAND_DATA_T_MIN_PACKED_SIZE                                    (16)
#define QAPI_BLE_BMS_EVENT_DATA_T_MIN_PACKED_SIZE                                                       ((6) + (QS_POINTER_HEADER_SIZE) * (1))

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_BMS_Client_Information_t(qapi_BLE_BMS_Client_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_BMS_BM_Control_Point_Format_Data_t(qapi_BLE_BMS_BM_Control_Point_Format_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_BMS_BM_Control_Point_Command_Data_t(qapi_BLE_BMS_BM_Control_Point_Command_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_BMS_Event_Data_t(qapi_BLE_BMS_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_BMS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_BMS_Client_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_BMS_BM_Control_Point_Format_Data_t(PackedBuffer_t *Buffer, qapi_BLE_BMS_BM_Control_Point_Format_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_BMS_BM_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, qapi_BLE_BMS_BM_Control_Point_Command_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_BMS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_BMS_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_BMS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_BMS_Client_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_BMS_BM_Control_Point_Format_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_BMS_BM_Control_Point_Format_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_BMS_BM_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_BMS_BM_Control_Point_Command_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_BMS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_BMS_Event_Data_t *Structure);

#endif // __QAPI_BLE_BMS_COMMON_H__
