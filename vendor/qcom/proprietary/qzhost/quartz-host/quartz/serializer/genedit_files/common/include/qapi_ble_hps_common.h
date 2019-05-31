/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_HPS_COMMON_H__
#define __QAPI_BLE_HPS_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_hps.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_hpstypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_HPS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE                                               (14)
#define QAPI_BLE_HPS_HTTP_STATUS_CODE_DATA_T_MIN_PACKED_SIZE                                            (3)
#define QAPI_BLE_HPS_SERVER_READ_CHARACTERISTIC_REQUEST_DATA_T_MIN_PACKED_SIZE                          (22)
#define QAPI_BLE_HPS_SERVER_WRITE_CHARACTERISTIC_REQUEST_DATA_T_MIN_PACKED_SIZE                         ((22) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_HPS_SERVER_PREPARE_CHARACTERISTIC_REQUEST_DATA_T_MIN_PACKED_SIZE                       (20)
#define QAPI_BLE_HPS_SERVER_WRITE_HTTP_CONTROL_POINT_REQUEST_DATA_T_MIN_PACKED_SIZE                     (20)
#define QAPI_BLE_HPS_SERVER_READ_HTTPS_SECURITY_REQUEST_DATA_T_MIN_PACKED_SIZE                          (16)
#define QAPI_BLE_HPS_SERVER_READ_HTTP_STATUS_CODE_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE                   (16)
#define QAPI_BLE_HPS_SERVER_WRITE_HTTP_STATUS_CODE_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE                  (18)
#define QAPI_BLE_HPS_EVENT_DATA_T_MIN_PACKED_SIZE                                                       ((6) + (QS_POINTER_HEADER_SIZE) * (1))

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_HPS_Client_Information_t(qapi_BLE_HPS_Client_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HPS_HTTP_Status_Code_Data_t(qapi_BLE_HPS_HTTP_Status_Code_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_t(qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HPS_Server_Write_Characteristic_Request_Data_t(qapi_BLE_HPS_Server_Write_Characteristic_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HPS_Server_Prepare_Characteristic_Request_Data_t(qapi_BLE_HPS_Server_Prepare_Characteristic_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HPS_Server_Write_HTTP_Control_Point_Request_Data_t(qapi_BLE_HPS_Server_Write_HTTP_Control_Point_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HPS_Server_Read_HTTPS_Security_Request_Data_t(qapi_BLE_HPS_Server_Read_HTTPS_Security_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data_t(qapi_BLE_HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data_t(qapi_BLE_HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HPS_Event_Data_t(qapi_BLE_HPS_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_HPS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_HPS_Client_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HPS_HTTP_Status_Code_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HPS_HTTP_Status_Code_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HPS_Server_Write_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HPS_Server_Write_Characteristic_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HPS_Server_Prepare_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HPS_Server_Prepare_Characteristic_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HPS_Server_Write_HTTP_Control_Point_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HPS_Server_Write_HTTP_Control_Point_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HPS_Server_Read_HTTPS_Security_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HPS_Server_Read_HTTPS_Security_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HPS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HPS_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_HPS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HPS_Client_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HPS_HTTP_Status_Code_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HPS_HTTP_Status_Code_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HPS_Server_Read_Characteristic_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HPS_Server_Write_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HPS_Server_Write_Characteristic_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HPS_Server_Prepare_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HPS_Server_Prepare_Characteristic_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HPS_Server_Write_HTTP_Control_Point_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HPS_Server_Write_HTTP_Control_Point_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HPS_Server_Read_HTTPS_Security_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HPS_Server_Read_HTTPS_Security_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HPS_Server_Read_HTTP_Status_Code_CCCD_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HPS_Server_Write_HTTP_Status_Code_CCCD_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HPS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HPS_Event_Data_t *Structure);

#endif // __QAPI_BLE_HPS_COMMON_H__
