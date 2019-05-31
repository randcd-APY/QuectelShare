/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_WSS_COMMON_H__
#define __QAPI_BLE_WSS_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_wss.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_wsstypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_WSS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE                                               (6)
#define QAPI_BLE_WSS_SERVER_INFORMATION_T_MIN_PACKED_SIZE                                               (2)
#define QAPI_BLE_WSS_DATE_TIME_DATA_T_MIN_PACKED_SIZE                                                   (7)
#define QAPI_BLE_WSS_WEIGHT_MEASUREMENT_DATA_T_MIN_PACKED_SIZE                                          (8)
#define QAPI_BLE_WSS_READ_WEIGHT_MEASUREMENT_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE                        (16)
#define QAPI_BLE_WSS_WRITE_WEIGHT_MEASUREMENT_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE                       (18)
#define QAPI_BLE_WSS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE                                                (19)
#define QAPI_BLE_WSS_EVENT_DATA_T_MIN_PACKED_SIZE                                                       ((5) + (QS_POINTER_HEADER_SIZE) * (1))

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_WSS_Client_Information_t(qapi_BLE_WSS_Client_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_WSS_Server_Information_t(qapi_BLE_WSS_Server_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_WSS_Date_Time_Data_t(qapi_BLE_WSS_Date_Time_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_WSS_Weight_Measurement_Data_t(qapi_BLE_WSS_Weight_Measurement_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_WSS_Read_Weight_Measurement_CCCD_Request_Data_t(qapi_BLE_WSS_Read_Weight_Measurement_CCCD_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_WSS_Write_Weight_Measurement_CCCD_Request_Data_t(qapi_BLE_WSS_Write_Weight_Measurement_CCCD_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_WSS_Confirmation_Data_t(qapi_BLE_WSS_Confirmation_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_WSS_Event_Data_t(qapi_BLE_WSS_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_WSS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_WSS_Client_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_WSS_Server_Information_t(PackedBuffer_t *Buffer, qapi_BLE_WSS_Server_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_WSS_Date_Time_Data_t(PackedBuffer_t *Buffer, qapi_BLE_WSS_Date_Time_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_WSS_Weight_Measurement_Data_t(PackedBuffer_t *Buffer, qapi_BLE_WSS_Weight_Measurement_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_WSS_Read_Weight_Measurement_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_WSS_Read_Weight_Measurement_CCCD_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_WSS_Write_Weight_Measurement_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_WSS_Write_Weight_Measurement_CCCD_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_WSS_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_WSS_Confirmation_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_WSS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_WSS_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_WSS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_WSS_Client_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_WSS_Server_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_WSS_Server_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_WSS_Date_Time_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_WSS_Date_Time_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_WSS_Weight_Measurement_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_WSS_Weight_Measurement_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_WSS_Read_Weight_Measurement_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_WSS_Read_Weight_Measurement_CCCD_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_WSS_Write_Weight_Measurement_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_WSS_Write_Weight_Measurement_CCCD_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_WSS_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_WSS_Confirmation_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_WSS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_WSS_Event_Data_t *Structure);

#endif // __QAPI_BLE_WSS_COMMON_H__
