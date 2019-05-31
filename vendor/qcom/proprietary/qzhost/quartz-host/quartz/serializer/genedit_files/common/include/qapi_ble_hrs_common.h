/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_HRS_COMMON_H__
#define __QAPI_BLE_HRS_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_hrs.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_hrstypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_DATA_T_MIN_PACKED_SIZE                                      (7)
#define QAPI_BLE_HRS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE                                               (8)
#define QAPI_BLE_HRS_SERVER_INFORMATION_T_MIN_PACKED_SIZE                                               (2)
#define QAPI_BLE_HRS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE                                   (20)
#define QAPI_BLE_HRS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE                                 (18)
#define QAPI_BLE_HRS_HEART_RATE_CONTROL_COMMAND_DATA_T_MIN_PACKED_SIZE                                  (16)
#define QAPI_BLE_HRS_EVENT_DATA_T_MIN_PACKED_SIZE                                                       ((6) + (QS_POINTER_HEADER_SIZE) * (1))

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_HRS_Heart_Rate_Measurement_Data_t(qapi_BLE_HRS_Heart_Rate_Measurement_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HRS_Client_Information_t(qapi_BLE_HRS_Client_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HRS_Server_Information_t(qapi_BLE_HRS_Server_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HRS_Read_Client_Configuration_Data_t(qapi_BLE_HRS_Read_Client_Configuration_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HRS_Client_Configuration_Update_Data_t(qapi_BLE_HRS_Client_Configuration_Update_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HRS_Heart_Rate_Control_Command_Data_t(qapi_BLE_HRS_Heart_Rate_Control_Command_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_HRS_Event_Data_t(qapi_BLE_HRS_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_HRS_Heart_Rate_Measurement_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HRS_Heart_Rate_Measurement_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HRS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_HRS_Client_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HRS_Server_Information_t(PackedBuffer_t *Buffer, qapi_BLE_HRS_Server_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HRS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HRS_Read_Client_Configuration_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HRS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HRS_Client_Configuration_Update_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HRS_Heart_Rate_Control_Command_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HRS_Heart_Rate_Control_Command_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_HRS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_HRS_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_HRS_Heart_Rate_Measurement_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HRS_Heart_Rate_Measurement_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HRS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HRS_Client_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HRS_Server_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HRS_Server_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HRS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HRS_Read_Client_Configuration_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HRS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HRS_Client_Configuration_Update_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HRS_Heart_Rate_Control_Command_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HRS_Heart_Rate_Control_Command_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_HRS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_HRS_Event_Data_t *Structure);

#endif // __QAPI_BLE_HRS_COMMON_H__
