/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_SCPS_COMMON_H__
#define __QAPI_BLE_SCPS_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_scps.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_scpstypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_SCPS_SCAN_INTERVAL_WINDOW_DATA_T_MIN_PACKED_SIZE                                       (4)
#define QAPI_BLE_SCPS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE                                  (20)
#define QAPI_BLE_SCPS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE                                (18)
#define QAPI_BLE_SCPS_WRITE_SCAN_INTERVAL_WINDOW_DATA_T_MIN_PACKED_SIZE                                 (12)
#define QAPI_BLE_SCPS_EVENT_DATA_T_MIN_PACKED_SIZE                                                      ((6) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_SCPS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE                                              (6)
#define QAPI_BLE_SCPS_SERVER_INFORMATION_T_MIN_PACKED_SIZE                                              (2)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_SCPS_Scan_Interval_Window_Data_t(qapi_BLE_SCPS_Scan_Interval_Window_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_SCPS_Read_Client_Configuration_Data_t(qapi_BLE_SCPS_Read_Client_Configuration_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_SCPS_Client_Configuration_Update_Data_t(qapi_BLE_SCPS_Client_Configuration_Update_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_SCPS_Write_Scan_Interval_Window_Data_t(qapi_BLE_SCPS_Write_Scan_Interval_Window_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_SCPS_Event_Data_t(qapi_BLE_SCPS_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_SCPS_Client_Information_t(qapi_BLE_SCPS_Client_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_SCPS_Server_Information_t(qapi_BLE_SCPS_Server_Information_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_SCPS_Scan_Interval_Window_Data_t(PackedBuffer_t *Buffer, qapi_BLE_SCPS_Scan_Interval_Window_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_SCPS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, qapi_BLE_SCPS_Read_Client_Configuration_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_SCPS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_SCPS_Client_Configuration_Update_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_SCPS_Write_Scan_Interval_Window_Data_t(PackedBuffer_t *Buffer, qapi_BLE_SCPS_Write_Scan_Interval_Window_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_SCPS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_SCPS_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_SCPS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_SCPS_Client_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_SCPS_Server_Information_t(PackedBuffer_t *Buffer, qapi_BLE_SCPS_Server_Information_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_SCPS_Scan_Interval_Window_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_SCPS_Scan_Interval_Window_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_SCPS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_SCPS_Read_Client_Configuration_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_SCPS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_SCPS_Client_Configuration_Update_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_SCPS_Write_Scan_Interval_Window_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_SCPS_Write_Scan_Interval_Window_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_SCPS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_SCPS_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_SCPS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_SCPS_Client_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_SCPS_Server_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_SCPS_Server_Information_t *Structure);

#endif // __QAPI_BLE_SCPS_COMMON_H__
