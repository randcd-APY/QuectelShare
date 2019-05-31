/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_PASS_COMMON_H__
#define __QAPI_BLE_PASS_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_pass.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_passtypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_PASS_ALERT_STATUS_T_MIN_PACKED_SIZE                                                    (12)
#define QAPI_BLE_PASS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE                                              (10)
#define QAPI_BLE_PASS_SERVER_INFORMATION_T_MIN_PACKED_SIZE                                              (8)
#define QAPI_BLE_PASS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE                                  (20)
#define QAPI_BLE_PASS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE                                (20)
#define QAPI_BLE_PASS_RINGER_CONTROL_COMMAND_DATA_T_MIN_PACKED_SIZE                                     (16)
#define QAPI_BLE_PASS_EVENT_DATA_T_MIN_PACKED_SIZE                                                      ((6) + (QS_POINTER_HEADER_SIZE) * (1))

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_PASS_Alert_Status_t(qapi_BLE_PASS_Alert_Status_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_PASS_Client_Information_t(qapi_BLE_PASS_Client_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_PASS_Server_Information_t(qapi_BLE_PASS_Server_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_PASS_Read_Client_Configuration_Data_t(qapi_BLE_PASS_Read_Client_Configuration_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_PASS_Client_Configuration_Update_Data_t(qapi_BLE_PASS_Client_Configuration_Update_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_PASS_Ringer_Control_Command_Data_t(qapi_BLE_PASS_Ringer_Control_Command_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_PASS_Event_Data_t(qapi_BLE_PASS_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_PASS_Alert_Status_t(PackedBuffer_t *Buffer, qapi_BLE_PASS_Alert_Status_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_PASS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_PASS_Client_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_PASS_Server_Information_t(PackedBuffer_t *Buffer, qapi_BLE_PASS_Server_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_PASS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, qapi_BLE_PASS_Read_Client_Configuration_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_PASS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_PASS_Client_Configuration_Update_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_PASS_Ringer_Control_Command_Data_t(PackedBuffer_t *Buffer, qapi_BLE_PASS_Ringer_Control_Command_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_PASS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_PASS_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_PASS_Alert_Status_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_PASS_Alert_Status_t *Structure);
SerStatus_t PackedRead_qapi_BLE_PASS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_PASS_Client_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_PASS_Server_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_PASS_Server_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_PASS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_PASS_Read_Client_Configuration_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_PASS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_PASS_Client_Configuration_Update_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_PASS_Ringer_Control_Command_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_PASS_Ringer_Control_Command_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_PASS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_PASS_Event_Data_t *Structure);

#endif // __QAPI_BLE_PASS_COMMON_H__
