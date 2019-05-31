/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_TRDS_COMMON_H__
#define __QAPI_BLE_TRDS_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_trds.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_trdstypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_TRDS_INITIALIZE_DATA_T_MIN_PACKED_SIZE                                                 (4)
#define QAPI_BLE_TRDS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE                                              (4)
#define QAPI_BLE_TRDS_SERVER_INFORMATION_T_MIN_PACKED_SIZE                                              (2)
#define QAPI_BLE_TRDS_TRANSPORT_BLOCK_DATA_T_MIN_PACKED_SIZE                                            ((3) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_TRDS_CONTROL_POINT_REQUEST_DATA_T_MIN_PACKED_SIZE                                      ((7) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_TRDS_CONTROL_POINT_RESPONSE_DATA_T_MIN_PACKED_SIZE                                     ((10) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_TRDS_WRITE_CONTROL_POINT_REQUEST_DATA_T_MIN_PACKED_SIZE                                (16)
#define QAPI_BLE_TRDS_READ_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE                                          (20)
#define QAPI_BLE_TRDS_WRITE_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE                                         (22)
#define QAPI_BLE_TRDS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE                                               (19)
#define QAPI_BLE_TRDS_EVENT_DATA_T_MIN_PACKED_SIZE                                                      ((6) + (QS_POINTER_HEADER_SIZE) * (1))

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_TRDS_Initialize_Data_t(qapi_BLE_TRDS_Initialize_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_TRDS_Client_Information_t(qapi_BLE_TRDS_Client_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_TRDS_Server_Information_t(qapi_BLE_TRDS_Server_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_TRDS_Transport_Block_Data_t(qapi_BLE_TRDS_Transport_Block_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_TRDS_Control_Point_Request_Data_t(qapi_BLE_TRDS_Control_Point_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_TRDS_Control_Point_Response_Data_t(qapi_BLE_TRDS_Control_Point_Response_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_TRDS_Write_Control_Point_Request_Data_t(qapi_BLE_TRDS_Write_Control_Point_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_TRDS_Read_CCCD_Request_Data_t(qapi_BLE_TRDS_Read_CCCD_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_TRDS_Write_CCCD_Request_Data_t(qapi_BLE_TRDS_Write_CCCD_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_TRDS_Confirmation_Data_t(qapi_BLE_TRDS_Confirmation_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_TRDS_Event_Data_t(qapi_BLE_TRDS_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_TRDS_Initialize_Data_t(PackedBuffer_t *Buffer, qapi_BLE_TRDS_Initialize_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_TRDS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_TRDS_Client_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_TRDS_Server_Information_t(PackedBuffer_t *Buffer, qapi_BLE_TRDS_Server_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_TRDS_Transport_Block_Data_t(PackedBuffer_t *Buffer, qapi_BLE_TRDS_Transport_Block_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_TRDS_Control_Point_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_TRDS_Control_Point_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_TRDS_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_TRDS_Control_Point_Response_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_TRDS_Write_Control_Point_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_TRDS_Write_Control_Point_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_TRDS_Read_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_TRDS_Read_CCCD_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_TRDS_Write_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_TRDS_Write_CCCD_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_TRDS_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_TRDS_Confirmation_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_TRDS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_TRDS_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_TRDS_Initialize_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_TRDS_Initialize_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_TRDS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_TRDS_Client_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_TRDS_Server_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_TRDS_Server_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_TRDS_Transport_Block_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_TRDS_Transport_Block_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_TRDS_Control_Point_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_TRDS_Control_Point_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_TRDS_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_TRDS_Control_Point_Response_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_TRDS_Write_Control_Point_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_TRDS_Write_Control_Point_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_TRDS_Read_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_TRDS_Read_CCCD_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_TRDS_Write_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_TRDS_Write_CCCD_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_TRDS_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_TRDS_Confirmation_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_TRDS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_TRDS_Event_Data_t *Structure);

#endif // __QAPI_BLE_TRDS_COMMON_H__
