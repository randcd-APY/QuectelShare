/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_BLS_COMMON_H__
#define __QAPI_BLE_BLS_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_bls.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_blstypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_BLS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE                                               (10)
#define QAPI_BLE_BLS_SERVER_INFORMATION_T_MIN_PACKED_SIZE                                               (4)
#define QAPI_BLE_BLS_DATE_TIME_DATA_T_MIN_PACKED_SIZE                                                   (7)
#define QAPI_BLE_BLS_COMPOUND_VALUE_DATA_T_MIN_PACKED_SIZE                                              (6)
#define QAPI_BLE_BLS_BLOOD_PRESSURE_MEASUREMENT_DATA_T_MIN_PACKED_SIZE                                  (6)
#define QAPI_BLE_BLS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE                                   (20)
#define QAPI_BLE_BLS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE                                 (18)
#define QAPI_BLE_BLS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE                                                (13)
#define QAPI_BLE_BLS_EVENT_DATA_T_MIN_PACKED_SIZE                                                       ((6) + (QS_POINTER_HEADER_SIZE) * (1))

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_BLS_Client_Information_t(qapi_BLE_BLS_Client_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_BLS_Server_Information_t(qapi_BLE_BLS_Server_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_BLS_Date_Time_Data_t(qapi_BLE_BLS_Date_Time_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_BLS_Compound_Value_Data_t(qapi_BLE_BLS_Compound_Value_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t(qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_BLS_Read_Client_Configuration_Data_t(qapi_BLE_BLS_Read_Client_Configuration_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_BLS_Client_Configuration_Update_Data_t(qapi_BLE_BLS_Client_Configuration_Update_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_BLS_Confirmation_Data_t(qapi_BLE_BLS_Confirmation_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_BLS_Event_Data_t(qapi_BLE_BLS_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_BLS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_BLS_Client_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_BLS_Server_Information_t(PackedBuffer_t *Buffer, qapi_BLE_BLS_Server_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_BLS_Date_Time_Data_t(PackedBuffer_t *Buffer, qapi_BLE_BLS_Date_Time_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_BLS_Compound_Value_Data_t(PackedBuffer_t *Buffer, qapi_BLE_BLS_Compound_Value_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t(PackedBuffer_t *Buffer, qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_BLS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, qapi_BLE_BLS_Read_Client_Configuration_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_BLS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_BLS_Client_Configuration_Update_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_BLS_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_BLS_Confirmation_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_BLS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_BLS_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_BLS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_BLS_Client_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_BLS_Server_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_BLS_Server_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_BLS_Date_Time_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_BLS_Date_Time_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_BLS_Compound_Value_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_BLS_Compound_Value_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_BLS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_BLS_Read_Client_Configuration_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_BLS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_BLS_Client_Configuration_Update_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_BLS_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_BLS_Confirmation_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_BLS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_BLS_Event_Data_t *Structure);

#endif // __QAPI_BLE_BLS_COMMON_H__
