/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_HIDS_COMMON_H__
#define __QAPI_BLE_HIDS_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_hids.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_hidstypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_HIDS_REPORT_REFERENCE_DATA_T_MIN_PACKED_SIZE                                           (2)
#define QAPI_BLE_HIDS_HID_INFORMATION_DATA_T_MIN_PACKED_SIZE                                            (4)
#define QAPI_BLE_HIDS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE                                  (20)
#define QAPI_BLE_HIDS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE                                (18)
#define QAPI_BLE_HIDS_GET_PROTOCOL_MODE_REQUEST_DATA_T_MIN_PACKED_SIZE                                  (16)
#define QAPI_BLE_HIDS_SET_PROTOCOL_MODE_REQUEST_DATA_T_MIN_PACKED_SIZE                                  (20)
#define QAPI_BLE_HIDS_GET_REPORT_MAP_REQUEST_DATA_T_MIN_PACKED_SIZE                                     (18)
#define QAPI_BLE_HIDS_GET_REPORT_REQUEST_DATA_T_MIN_PACKED_SIZE                                         (22)
#define QAPI_BLE_HIDS_SET_REPORT_REQUEST_DATA_T_MIN_PACKED_SIZE                                         ((24) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_HIDS_CONTROL_POINT_COMMAND_DATA_T_MIN_PACKED_SIZE                                      (16)
#define QAPI_BLE_HIDS_EVENT_DATA_T_MIN_PACKED_SIZE                                                      ((6) + (QS_POINTER_HEADER_SIZE) * (1))

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_HIDS_Report_Reference_Data_t(qapi_BLE_HIDS_Report_Reference_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HIDS_HID_Information_Data_t(qapi_BLE_HIDS_HID_Information_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HIDS_Read_Client_Configuration_Data_t(qapi_BLE_HIDS_Read_Client_Configuration_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HIDS_Client_Configuration_Update_Data_t(qapi_BLE_HIDS_Client_Configuration_Update_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HIDS_Get_Protocol_Mode_Request_Data_t(qapi_BLE_HIDS_Get_Protocol_Mode_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HIDS_Set_Protocol_Mode_Request_Data_t(qapi_BLE_HIDS_Set_Protocol_Mode_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HIDS_Get_Report_Map_Request_Data_t(qapi_BLE_HIDS_Get_Report_Map_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HIDS_Get_Report_Request_Data_t(qapi_BLE_HIDS_Get_Report_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HIDS_Set_Report_Request_Data_t(qapi_BLE_HIDS_Set_Report_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HIDS_Control_Point_Command_Data_t(qapi_BLE_HIDS_Control_Point_Command_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HIDS_Event_Data_t(qapi_BLE_HIDS_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_HIDS_Report_Reference_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HIDS_Report_Reference_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HIDS_HID_Information_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HIDS_HID_Information_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HIDS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HIDS_Read_Client_Configuration_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HIDS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HIDS_Client_Configuration_Update_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HIDS_Get_Protocol_Mode_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HIDS_Get_Protocol_Mode_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HIDS_Set_Protocol_Mode_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HIDS_Set_Protocol_Mode_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HIDS_Get_Report_Map_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HIDS_Get_Report_Map_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HIDS_Get_Report_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HIDS_Get_Report_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HIDS_Set_Report_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HIDS_Set_Report_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HIDS_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HIDS_Control_Point_Command_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HIDS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HIDS_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_HIDS_Report_Reference_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HIDS_Report_Reference_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HIDS_HID_Information_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HIDS_HID_Information_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HIDS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HIDS_Read_Client_Configuration_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HIDS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HIDS_Client_Configuration_Update_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HIDS_Get_Protocol_Mode_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HIDS_Get_Protocol_Mode_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HIDS_Set_Protocol_Mode_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HIDS_Set_Protocol_Mode_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HIDS_Get_Report_Map_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HIDS_Get_Report_Map_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HIDS_Get_Report_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HIDS_Get_Report_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HIDS_Set_Report_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HIDS_Set_Report_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HIDS_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HIDS_Control_Point_Command_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HIDS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HIDS_Event_Data_t *Structure);

#endif // __QAPI_BLE_HIDS_COMMON_H__
