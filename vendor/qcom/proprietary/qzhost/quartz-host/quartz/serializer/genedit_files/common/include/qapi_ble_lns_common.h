/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_LNS_COMMON_H__
#define __QAPI_BLE_LNS_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_lns.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_lnstypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_LNS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE                                               (16)
#define QAPI_BLE_LNS_SERVER_INFORMATION_T_MIN_PACKED_SIZE                                               (6)
#define QAPI_BLE_LNS_DATE_TIME_DATA_T_MIN_PACKED_SIZE                                                   (7)
#define QAPI_BLE_LNS_LOCATION_SPEED_DATA_T_MIN_PACKED_SIZE                                              (15)
#define QAPI_BLE_LNS_POSITION_QUALITY_DATA_T_MIN_PACKED_SIZE                                            (16)
#define QAPI_BLE_LNS_LN_CONTROL_POINT_FORMAT_DATA_T_MIN_PACKED_SIZE                                     ((4) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_LNS_LN_CONTROL_POINT_RESPONSE_DATA_T_MIN_PACKED_SIZE                                   (9)
#define QAPI_BLE_LNS_NAVIGATION_DATA_T_MIN_PACKED_SIZE                                                  (6)
#define QAPI_BLE_LNS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE                                   (20)
#define QAPI_BLE_LNS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE                                 (22)
#define QAPI_BLE_LNS_LN_CONTROL_POINT_COMMAND_DATA_T_MIN_PACKED_SIZE                                    (16)
#define QAPI_BLE_LNS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE                                                (13)
#define QAPI_BLE_LNS_EVENT_DATA_T_MIN_PACKED_SIZE                                                       ((6) + (QS_POINTER_HEADER_SIZE) * (1))

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_LNS_Client_Information_t(qapi_BLE_LNS_Client_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_LNS_Server_Information_t(qapi_BLE_LNS_Server_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_LNS_Date_Time_Data_t(qapi_BLE_LNS_Date_Time_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_LNS_Location_Speed_Data_t(qapi_BLE_LNS_Location_Speed_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_LNS_Position_Quality_Data_t(qapi_BLE_LNS_Position_Quality_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_LNS_LN_Control_Point_Format_Data_t(qapi_BLE_LNS_LN_Control_Point_Format_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_LNS_LN_Control_Point_Response_Data_t(qapi_BLE_LNS_LN_Control_Point_Response_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_LNS_Navigation_Data_t(qapi_BLE_LNS_Navigation_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_LNS_Read_Client_Configuration_Data_t(qapi_BLE_LNS_Read_Client_Configuration_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_LNS_Client_Configuration_Update_Data_t(qapi_BLE_LNS_Client_Configuration_Update_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_LNS_LN_Control_Point_Command_Data_t(qapi_BLE_LNS_LN_Control_Point_Command_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_LNS_Confirmation_Data_t(qapi_BLE_LNS_Confirmation_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_LNS_Event_Data_t(qapi_BLE_LNS_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_LNS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_Client_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_LNS_Server_Information_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_Server_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_LNS_Date_Time_Data_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_Date_Time_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_LNS_Location_Speed_Data_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_Location_Speed_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_LNS_Position_Quality_Data_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_Position_Quality_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_LNS_LN_Control_Point_Format_Data_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_LN_Control_Point_Format_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_LNS_LN_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_LN_Control_Point_Response_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_LNS_Navigation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_Navigation_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_LNS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_Read_Client_Configuration_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_LNS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_Client_Configuration_Update_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_LNS_LN_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_LN_Control_Point_Command_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_LNS_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_Confirmation_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_LNS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_LNS_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_LNS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_Client_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_LNS_Server_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_Server_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_LNS_Date_Time_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_Date_Time_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_LNS_Location_Speed_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_Location_Speed_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_LNS_Position_Quality_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_Position_Quality_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_LNS_LN_Control_Point_Format_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_LN_Control_Point_Format_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_LNS_LN_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_LN_Control_Point_Response_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_LNS_Navigation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_Navigation_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_LNS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_Read_Client_Configuration_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_LNS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_Client_Configuration_Update_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_LNS_LN_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_LN_Control_Point_Command_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_LNS_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_Confirmation_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_LNS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_LNS_Event_Data_t *Structure);

#endif // __QAPI_BLE_LNS_COMMON_H__
