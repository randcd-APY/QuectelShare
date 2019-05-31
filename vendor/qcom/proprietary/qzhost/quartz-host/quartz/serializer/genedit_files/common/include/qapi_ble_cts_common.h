/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_CTS_COMMON_H__
#define __QAPI_BLE_CTS_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_cts.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_ctstypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_CTS_DATE_TIME_DATA_T_MIN_PACKED_SIZE                                                   (10)
#define QAPI_BLE_CTS_DAY_DATE_TIME_DATA_T_MIN_PACKED_SIZE                                               (4)
#define QAPI_BLE_CTS_EXACT_TIME_DATA_T_MIN_PACKED_SIZE                                                  (1)
#define QAPI_BLE_CTS_CURRENT_TIME_DATA_T_MIN_PACKED_SIZE                                                (1)
#define QAPI_BLE_CTS_LOCAL_TIME_INFORMATION_DATA_T_MIN_PACKED_SIZE                                      (8)
#define QAPI_BLE_CTS_REFERENCE_TIME_INFORMATION_DATA_T_MIN_PACKED_SIZE                                  (7)
#define QAPI_BLE_CTS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE                                   (20)
#define QAPI_BLE_CTS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE                                 (18)
#define QAPI_BLE_CTS_READ_CURRENT_TIME_REQUEST_DATA_T_MIN_PACKED_SIZE                                   (16)
#define QAPI_BLE_CTS_WRITE_CURRENT_TIME_REQUEST_DATA_T_MIN_PACKED_SIZE                                  (16)
#define QAPI_BLE_CTS_WRITE_LOCAL_TIME_INFORMATION_REQUEST_DATA_T_MIN_PACKED_SIZE                        (16)
#define QAPI_BLE_CTS_READ_REFERENCE_TIME_INFORMATION_REQUEST_DATA_T_MIN_PACKED_SIZE                     (16)
#define QAPI_BLE_CTS_EVENT_DATA_T_MIN_PACKED_SIZE                                                       ((6) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_CTS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE                                               (8)
#define QAPI_BLE_CTS_SERVER_INFORMATION_T_MIN_PACKED_SIZE                                               (2)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_CTS_Date_Time_Data_t(qapi_BLE_CTS_Date_Time_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CTS_Day_Date_Time_Data_t(qapi_BLE_CTS_Day_Date_Time_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CTS_Exact_Time_Data_t(qapi_BLE_CTS_Exact_Time_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CTS_Current_Time_Data_t(qapi_BLE_CTS_Current_Time_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CTS_Local_Time_Information_Data_t(qapi_BLE_CTS_Local_Time_Information_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CTS_Reference_Time_Information_Data_t(qapi_BLE_CTS_Reference_Time_Information_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CTS_Read_Client_Configuration_Data_t(qapi_BLE_CTS_Read_Client_Configuration_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CTS_Client_Configuration_Update_Data_t(qapi_BLE_CTS_Client_Configuration_Update_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CTS_Read_Current_Time_Request_Data_t(qapi_BLE_CTS_Read_Current_Time_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CTS_Write_Current_Time_Request_Data_t(qapi_BLE_CTS_Write_Current_Time_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CTS_Write_Local_Time_Information_Request_Data_t(qapi_BLE_CTS_Write_Local_Time_Information_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CTS_Read_Reference_Time_Information_Request_Data_t(qapi_BLE_CTS_Read_Reference_Time_Information_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CTS_Event_Data_t(qapi_BLE_CTS_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CTS_Client_Information_t(qapi_BLE_CTS_Client_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CTS_Server_Information_t(qapi_BLE_CTS_Server_Information_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_CTS_Date_Time_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Date_Time_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CTS_Day_Date_Time_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Day_Date_Time_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CTS_Exact_Time_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Exact_Time_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CTS_Current_Time_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Current_Time_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CTS_Local_Time_Information_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Local_Time_Information_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CTS_Reference_Time_Information_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Reference_Time_Information_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CTS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Read_Client_Configuration_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CTS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Client_Configuration_Update_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CTS_Read_Current_Time_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Read_Current_Time_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CTS_Write_Current_Time_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Write_Current_Time_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CTS_Write_Local_Time_Information_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Write_Local_Time_Information_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CTS_Read_Reference_Time_Information_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Read_Reference_Time_Information_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CTS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CTS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Client_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CTS_Server_Information_t(PackedBuffer_t *Buffer, qapi_BLE_CTS_Server_Information_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_CTS_Date_Time_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Date_Time_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CTS_Day_Date_Time_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Day_Date_Time_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CTS_Exact_Time_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Exact_Time_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CTS_Current_Time_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Current_Time_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CTS_Local_Time_Information_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Local_Time_Information_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CTS_Reference_Time_Information_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Reference_Time_Information_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CTS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Read_Client_Configuration_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CTS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Client_Configuration_Update_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CTS_Read_Current_Time_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Read_Current_Time_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CTS_Write_Current_Time_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Write_Current_Time_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CTS_Write_Local_Time_Information_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Write_Local_Time_Information_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CTS_Read_Reference_Time_Information_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Read_Reference_Time_Information_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CTS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CTS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Client_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CTS_Server_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CTS_Server_Information_t *Structure);

#endif // __QAPI_BLE_CTS_COMMON_H__
