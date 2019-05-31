/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_NDCS_COMMON_H__
#define __QAPI_BLE_NDCS_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_ndcs.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_ndcstypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_NDCS_DATE_TIME_DATA_T_MIN_PACKED_SIZE                                                  (7)
#define QAPI_BLE_NDCS_TIME_WITH_DST_DATA_T_MIN_PACKED_SIZE                                              (1)
#define QAPI_BLE_NDCS_READ_TIME_WITH_DST_REQUEST_DATA_T_MIN_PACKED_SIZE                                 (16)
#define QAPI_BLE_NDCS_EVENT_DATA_T_MIN_PACKED_SIZE                                                      ((6) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_NDCS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE                                              (2)
#define QAPI_BLE_NDCS_SERVER_INFORMATION_T_MIN_PACKED_SIZE                                              (2)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_NDCS_Date_Time_Data_t(qapi_BLE_NDCS_Date_Time_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_NDCS_Time_With_Dst_Data_t(qapi_BLE_NDCS_Time_With_Dst_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_NDCS_Read_Time_With_DST_Request_Data_t(qapi_BLE_NDCS_Read_Time_With_DST_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_NDCS_Event_Data_t(qapi_BLE_NDCS_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_NDCS_Client_Information_t(qapi_BLE_NDCS_Client_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_NDCS_Server_Information_t(qapi_BLE_NDCS_Server_Information_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_NDCS_Date_Time_Data_t(PackedBuffer_t *Buffer, qapi_BLE_NDCS_Date_Time_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_NDCS_Time_With_Dst_Data_t(PackedBuffer_t *Buffer, qapi_BLE_NDCS_Time_With_Dst_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_NDCS_Read_Time_With_DST_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_NDCS_Read_Time_With_DST_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_NDCS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_NDCS_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_NDCS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_NDCS_Client_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_NDCS_Server_Information_t(PackedBuffer_t *Buffer, qapi_BLE_NDCS_Server_Information_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_NDCS_Date_Time_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_NDCS_Date_Time_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_NDCS_Time_With_Dst_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_NDCS_Time_With_Dst_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_NDCS_Read_Time_With_DST_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_NDCS_Read_Time_With_DST_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_NDCS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_NDCS_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_NDCS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_NDCS_Client_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_NDCS_Server_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_NDCS_Server_Information_t *Structure);

#endif // __QAPI_BLE_NDCS_COMMON_H__
