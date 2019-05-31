/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_ANS_COMMON_H__
#define __QAPI_BLE_ANS_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_ans.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_anstypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_ANS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE                                               (14)
#define QAPI_BLE_ANS_SERVER_INFORMATION_T_MIN_PACKED_SIZE                                               (12)
#define QAPI_BLE_ANS_NEW_ALERT_DATA_T_MIN_PACKED_SIZE                                                   ((5) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_ANS_UN_READ_ALERT_DATA_T_MIN_PACKED_SIZE                                               (5)
#define QAPI_BLE_ANS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE                                   (20)
#define QAPI_BLE_ANS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE                                 (20)
#define QAPI_BLE_ANS_CONTROL_POINT_COMMAND_DATA_T_MIN_PACKED_SIZE                                       (20)
#define QAPI_BLE_ANS_EVENT_DATA_T_MIN_PACKED_SIZE                                                       ((6) + (QS_POINTER_HEADER_SIZE) * (1))

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_ANS_Client_Information_t(qapi_BLE_ANS_Client_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ANS_Server_Information_t(qapi_BLE_ANS_Server_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ANS_New_Alert_Data_t(qapi_BLE_ANS_New_Alert_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ANS_Un_Read_Alert_Data_t(qapi_BLE_ANS_Un_Read_Alert_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ANS_Read_Client_Configuration_Data_t(qapi_BLE_ANS_Read_Client_Configuration_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ANS_Client_Configuration_Update_Data_t(qapi_BLE_ANS_Client_Configuration_Update_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ANS_Control_Point_Command_Data_t(qapi_BLE_ANS_Control_Point_Command_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_ANS_Event_Data_t(qapi_BLE_ANS_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_ANS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_ANS_Client_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ANS_Server_Information_t(PackedBuffer_t *Buffer, qapi_BLE_ANS_Server_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ANS_New_Alert_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ANS_New_Alert_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ANS_Un_Read_Alert_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ANS_Un_Read_Alert_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ANS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ANS_Read_Client_Configuration_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ANS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ANS_Client_Configuration_Update_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ANS_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ANS_Control_Point_Command_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_ANS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_ANS_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_ANS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ANS_Client_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ANS_Server_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ANS_Server_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ANS_New_Alert_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ANS_New_Alert_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ANS_Un_Read_Alert_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ANS_Un_Read_Alert_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ANS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ANS_Read_Client_Configuration_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ANS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ANS_Client_Configuration_Update_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ANS_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ANS_Control_Point_Command_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_ANS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_ANS_Event_Data_t *Structure);

#endif // __QAPI_BLE_ANS_COMMON_H__
