/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_UDS_COMMON_H__
#define __QAPI_BLE_UDS_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_uds.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_udstypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_UDS_INITIALIZE_DATA_T_MIN_PACKED_SIZE                                                  (8)
#define QAPI_BLE_UDS_CHARACTERISTIC_HANDLES_T_MIN_PACKED_SIZE                                           (56)
#define QAPI_BLE_UDS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE                                               (10)
#define QAPI_BLE_UDS_SERVER_INFORMATION_T_MIN_PACKED_SIZE                                               (4)
#define QAPI_BLE_UDS_STRING_DATA_T_MIN_PACKED_SIZE                                                      ((2) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_UDS_DATE_DATA_T_MIN_PACKED_SIZE                                                        (4)
#define QAPI_BLE_UDS_FIVE_ZONE_HEART_RATE_LIMITS_DATA_T_MIN_PACKED_SIZE                                 (4)
#define QAPI_BLE_UDS_THREE_ZONE_HEART_RATE_LIMITS_DATA_T_MIN_PACKED_SIZE                                (2)
#define QAPI_BLE_UDS_USER_CONTROL_POINT_REQUEST_DATA_T_MIN_PACKED_SIZE                                  (7)
#define QAPI_BLE_UDS_USER_CONTROL_POINT_RESPONSE_DATA_T_MIN_PACKED_SIZE                                 (8)
#define QAPI_BLE_UDS_READ_CHARACTERISTIC_REQUEST_DATA_T_MIN_PACKED_SIZE                                 (22)
#define QAPI_BLE_UDS_WRITE_CHARACTERISTIC_REQUEST_DATA_T_MIN_PACKED_SIZE                                (20)
#define QAPI_BLE_UDS_PREPARE_WRITE_CHARACTERISTIC_REQUEST_DATA_T_MIN_PACKED_SIZE                        (20)
#define QAPI_BLE_UDS_READ_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE                                           (20)
#define QAPI_BLE_UDS_WRITE_CCCD_REQUEST_DATA_T_MIN_PACKED_SIZE                                          (22)
#define QAPI_BLE_UDS_READ_DATABASE_CHANGE_INCREMENT_REQUEST_DATA_T_MIN_PACKED_SIZE                      (16)
#define QAPI_BLE_UDS_WRITE_DATABASE_CHANGE_INCREMENT_REQUEST_DATA_T_MIN_PACKED_SIZE                     (20)
#define QAPI_BLE_UDS_READ_USER_INDEX_REQUEST_DATA_T_MIN_PACKED_SIZE                                     (16)
#define QAPI_BLE_UDS_WRITE_USER_CONTROL_POINT_REQUEST_DATA_T_MIN_PACKED_SIZE                            (16)
#define QAPI_BLE_UDS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE                                                (19)
#define QAPI_BLE_UDS_EVENT_DATA_T_MIN_PACKED_SIZE                                                       ((5) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_UDS_CHARACTERISTIC_T_MIN_PACKED_SIZE                                                   (0)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_UDS_Initialize_Data_t(qapi_BLE_UDS_Initialize_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UDS_Characteristic_Handles_t(qapi_BLE_UDS_Characteristic_Handles_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UDS_Client_Information_t(qapi_BLE_UDS_Client_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UDS_Server_Information_t(qapi_BLE_UDS_Server_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UDS_String_Data_t(qapi_BLE_UDS_String_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UDS_Date_Data_t(qapi_BLE_UDS_Date_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_t(qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_t(qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UDS_User_Control_Point_Request_Data_t(qapi_BLE_UDS_User_Control_Point_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UDS_User_Control_Point_Response_Data_t(qapi_BLE_UDS_User_Control_Point_Response_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UDS_Read_Characteristic_Request_Data_t(qapi_BLE_UDS_Read_Characteristic_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UDS_Write_Characteristic_Request_Data_t(qapi_BLE_UDS_Write_Characteristic_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Data_t(qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UDS_Read_CCCD_Request_Data_t(qapi_BLE_UDS_Read_CCCD_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UDS_Write_CCCD_Request_Data_t(qapi_BLE_UDS_Write_CCCD_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UDS_Read_Database_Change_Increment_Request_Data_t(qapi_BLE_UDS_Read_Database_Change_Increment_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UDS_Write_Database_Change_Increment_Request_Data_t(qapi_BLE_UDS_Write_Database_Change_Increment_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UDS_Read_User_Index_Request_Data_t(qapi_BLE_UDS_Read_User_Index_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UDS_Write_User_Control_Point_Request_Data_t(qapi_BLE_UDS_Write_User_Control_Point_Request_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UDS_Confirmation_Data_t(qapi_BLE_UDS_Confirmation_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UDS_Event_Data_t(qapi_BLE_UDS_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_UDS_Characteristic_t(qapi_BLE_UDS_Characteristic_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_UDS_Initialize_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Initialize_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UDS_Characteristic_Handles_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Characteristic_Handles_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UDS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Client_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UDS_Server_Information_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Server_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UDS_String_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_String_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UDS_Date_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Date_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UDS_User_Control_Point_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_User_Control_Point_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UDS_User_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_User_Control_Point_Response_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UDS_Read_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Read_Characteristic_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UDS_Write_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Write_Characteristic_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UDS_Read_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Read_CCCD_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UDS_Write_CCCD_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Write_CCCD_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UDS_Read_Database_Change_Increment_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Read_Database_Change_Increment_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UDS_Write_Database_Change_Increment_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Write_Database_Change_Increment_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UDS_Read_User_Index_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Read_User_Index_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UDS_Write_User_Control_Point_Request_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Write_User_Control_Point_Request_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UDS_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Confirmation_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UDS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_UDS_Characteristic_t(PackedBuffer_t *Buffer, qapi_BLE_UDS_Characteristic_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_UDS_Initialize_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Initialize_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UDS_Characteristic_Handles_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Characteristic_Handles_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UDS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Client_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UDS_Server_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Server_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UDS_String_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_String_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UDS_Date_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Date_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UDS_User_Control_Point_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_User_Control_Point_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UDS_User_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_User_Control_Point_Response_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UDS_Read_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Read_Characteristic_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UDS_Write_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Write_Characteristic_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UDS_Read_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Read_CCCD_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UDS_Write_CCCD_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Write_CCCD_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UDS_Read_Database_Change_Increment_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Read_Database_Change_Increment_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UDS_Write_Database_Change_Increment_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Write_Database_Change_Increment_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UDS_Read_User_Index_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Read_User_Index_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UDS_Write_User_Control_Point_Request_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Write_User_Control_Point_Request_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UDS_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Confirmation_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UDS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_UDS_Characteristic_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_UDS_Characteristic_t *Structure);

#endif // __QAPI_BLE_UDS_COMMON_H__
