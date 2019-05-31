/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_BCS_COMMON_H__
#define __QAPI_BLE_BCS_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_bcs.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_bcstypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_BCS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE                                               (12)
#define QAPI_BLE_BCS_SERVER_INFORMATION_T_MIN_PACKED_SIZE                                               (4)
#define QAPI_BLE_BCS_DATE_TIME_DATA_T_MIN_PACKED_SIZE                                                   (9)
#define QAPI_BLE_BCS_BC_MEASUREMENT_DATA_T_MIN_PACKED_SIZE                                              (45)
#define QAPI_BLE_BCS_READ_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE                                           (16)
#define QAPI_BLE_BCS_WRITE_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE                                          (20)
#define QAPI_BLE_BCS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE                                                (19)
#define QAPI_BLE_BCS_EVENT_DATA_T_MIN_PACKED_SIZE                                                       ((8) + (QS_POINTER_HEADER_SIZE) * (1))

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_BCS_Client_Information_t(qapi_BLE_BCS_Client_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_BCS_Server_Information_t(qapi_BLE_BCS_Server_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_BCS_Date_Time_Data_t(qapi_BLE_BCS_Date_Time_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_BCS_BC_Measurement_Data_t(qapi_BLE_BCS_BC_Measurement_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_BCS_Read_CCCD_Request_Data_t(qapi_BLE_BCS_Read_CCCD_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_BCS_Write_CCCD_Request_Data_t(qapi_BLE_BCS_Write_CCCD_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_BCS_Confirmation_Data_t(qapi_BLE_BCS_Confirmation_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_BCS_Event_Data_t(qapi_BLE_BCS_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_BCS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_BCS_Client_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_BCS_Server_Information_t(PackedBuffer_t *Buffer, qapi_BLE_BCS_Server_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_BCS_Date_Time_Data_t(PackedBuffer_t *Buffer, qapi_BLE_BCS_Date_Time_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_BCS_BC_Measurement_Data_t(PackedBuffer_t *Buffer, qapi_BLE_BCS_BC_Measurement_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_BCS_Read_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_BCS_Read_CCCD_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_BCS_Write_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_BCS_Write_CCCD_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_BCS_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_BCS_Confirmation_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_BCS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_BCS_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_BCS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_BCS_Client_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_BCS_Server_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_BCS_Server_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_BCS_Date_Time_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_BCS_Date_Time_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_BCS_BC_Measurement_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_BCS_BC_Measurement_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_BCS_Read_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_BCS_Read_CCCD_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_BCS_Write_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_BCS_Write_CCCD_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_BCS_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_BCS_Confirmation_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_BCS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_BCS_Event_Data_t *Structure);

#endif // __QAPI_BLE_BCS_COMMON_H__
