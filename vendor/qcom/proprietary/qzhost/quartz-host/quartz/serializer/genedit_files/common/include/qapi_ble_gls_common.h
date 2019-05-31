/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_GLS_COMMON_H__
#define __QAPI_BLE_GLS_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_gls.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_glstypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_GLS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE                                               (14)
#define QAPI_BLE_GLS_SERVER_INFORMATION_T_MIN_PACKED_SIZE                                               (8)
#define QAPI_BLE_GLS_SEQUENCE_NUMBER_RANGE_DATA_T_MIN_PACKED_SIZE                                       (4)
#define QAPI_BLE_GLS_DATE_TIME_DATA_T_MIN_PACKED_SIZE                                                   (7)
#define QAPI_BLE_GLS_DATE_TIME_RANGE_DATA_T_MIN_PACKED_SIZE                                             (0)
#define QAPI_BLE_GLS_CONCENTRATION_DATA_T_MIN_PACKED_SIZE                                               (8)
#define QAPI_BLE_GLS_GLUCOSE_MEASUREMENT_DATA_T_MIN_PACKED_SIZE                                         (7)
#define QAPI_BLE_GLS_CARBOHYDRATE_DATA_T_MIN_PACKED_SIZE                                                (3)
#define QAPI_BLE_GLS_EXERCISE_DATA_T_MIN_PACKED_SIZE                                                    (3)
#define QAPI_BLE_GLS_MEDICATION_DATA_T_MIN_PACKED_SIZE                                                  (3)
#define QAPI_BLE_GLS_GLUCOSE_MEASUREMENT_CONTEXT_DATA_T_MIN_PACKED_SIZE                                 (9)
#define QAPI_BLE_GLS_RACP_RESPONSE_CODE_VALUE_T_MIN_PACKED_SIZE                                         (2)
#define QAPI_BLE_GLS_RECORD_ACCESS_CONTROL_POINT_RESPONSE_DATA_T_MIN_PACKED_SIZE                        (4)
#define QAPI_BLE_GLS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE                                   (20)
#define QAPI_BLE_GLS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE                                 (18)
#define QAPI_BLE_GLS_RECORD_ACCESS_CONTROL_POINT_FORMAT_DATA_T_MIN_PACKED_SIZE                          (12)
#define QAPI_BLE_GLS_RECORD_ACCESS_CONTROL_POINT_COMMAND_DATA_T_MIN_PACKED_SIZE                         (16)
#define QAPI_BLE_GLS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE                                                (13)
#define QAPI_BLE_GLS_EVENT_DATA_T_MIN_PACKED_SIZE                                                       ((6) + (QS_POINTER_HEADER_SIZE) * (1))

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_GLS_Client_Information_t(qapi_BLE_GLS_Client_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Server_Information_t(qapi_BLE_GLS_Server_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Sequence_Number_Range_Data_t(qapi_BLE_GLS_Sequence_Number_Range_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Date_Time_Data_t(qapi_BLE_GLS_Date_Time_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Date_Time_Range_Data_t(qapi_BLE_GLS_Date_Time_Range_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Concentration_Data_t(qapi_BLE_GLS_Concentration_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Glucose_Measurement_Data_t(qapi_BLE_GLS_Glucose_Measurement_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Carbohydrate_Data_t(qapi_BLE_GLS_Carbohydrate_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Exercise_Data_t(qapi_BLE_GLS_Exercise_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Medication_Data_t(qapi_BLE_GLS_Medication_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Glucose_Measurement_Context_Data_t(qapi_BLE_GLS_Glucose_Measurement_Context_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_RACP_Response_Code_Value_t(qapi_BLE_GLS_RACP_Response_Code_Value_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Record_Access_Control_Point_Response_Data_t(qapi_BLE_GLS_Record_Access_Control_Point_Response_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Read_Client_Configuration_Data_t(qapi_BLE_GLS_Read_Client_Configuration_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Client_Configuration_Update_Data_t(qapi_BLE_GLS_Client_Configuration_Update_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t(qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Record_Access_Control_Point_Command_Data_t(qapi_BLE_GLS_Record_Access_Control_Point_Command_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Confirmation_Data_t(qapi_BLE_GLS_Confirmation_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Event_Data_t(qapi_BLE_GLS_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_GLS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Client_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Server_Information_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Server_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Sequence_Number_Range_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Sequence_Number_Range_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Date_Time_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Date_Time_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Date_Time_Range_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Date_Time_Range_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Concentration_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Concentration_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Glucose_Measurement_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Glucose_Measurement_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Carbohydrate_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Carbohydrate_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Exercise_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Exercise_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Medication_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Medication_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Glucose_Measurement_Context_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Glucose_Measurement_Context_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_RACP_Response_Code_Value_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_RACP_Response_Code_Value_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Record_Access_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Record_Access_Control_Point_Response_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Read_Client_Configuration_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Client_Configuration_Update_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Record_Access_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Record_Access_Control_Point_Command_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Confirmation_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_GLS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Client_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Server_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Server_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Sequence_Number_Range_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Sequence_Number_Range_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Date_Time_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Date_Time_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Date_Time_Range_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Date_Time_Range_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Concentration_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Concentration_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Glucose_Measurement_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Glucose_Measurement_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Carbohydrate_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Carbohydrate_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Exercise_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Exercise_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Medication_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Medication_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Glucose_Measurement_Context_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Glucose_Measurement_Context_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_RACP_Response_Code_Value_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_RACP_Response_Code_Value_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Record_Access_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Record_Access_Control_Point_Response_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Read_Client_Configuration_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Client_Configuration_Update_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Record_Access_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Record_Access_Control_Point_Command_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Confirmation_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Event_Data_t *Structure);

#endif // __QAPI_BLE_GLS_COMMON_H__
