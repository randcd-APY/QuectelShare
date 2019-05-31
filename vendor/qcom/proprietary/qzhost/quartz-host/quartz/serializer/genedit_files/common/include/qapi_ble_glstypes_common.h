/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_GLSTYPES_COMMON_H__
#define __QAPI_BLE_GLSTYPES_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_glstypes.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatttype_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_GLS_TIME_OFFSET_T_MIN_PACKED_SIZE                                                      (2)
#define QAPI_BLE_GLS_CONCENTRATION_T_MIN_PACKED_SIZE                                                    (3)
#define QAPI_BLE_GLS_SENSOR_STATUS_ANNUNCIATION_T_MIN_PACKED_SIZE                                       (2)
#define QAPI_BLE_GLS_GLUCOSE_MEASUREMENT_CONTEXT_HEADER_T_MIN_PACKED_SIZE                               (3)
#define QAPI_BLE_GLS_CONTEXT_EXTENDED_FLAGS_T_MIN_PACKED_SIZE                                           (1)
#define QAPI_BLE_GLS_CONTEXT_CARBOHYDRATE_T_MIN_PACKED_SIZE                                             (3)
#define QAPI_BLE_GLS_CONTEXT_MEAL_T_MIN_PACKED_SIZE                                                     (1)
#define QAPI_BLE_GLS_HEALTH_TESTER_T_MIN_PACKED_SIZE                                                    (1)
#define QAPI_BLE_GLS_CONTEXT_EXERCISE_T_MIN_PACKED_SIZE                                                 (3)
#define QAPI_BLE_GLS_CONTEXT_MEDICATION_T_MIN_PACKED_SIZE                                               (3)
#define QAPI_BLE_GLS_CONTEXT_HBA1C_T_MIN_PACKED_SIZE                                                    (2)
#define QAPI_BLE_GLS_SEQUENCE_NUMBER_RANGE_T_MIN_PACKED_SIZE                                            (4)
#define QAPI_BLE_GLS_DATE_TIME_RANGE_T_MIN_PACKED_SIZE                                                  (0)
#define QAPI_BLE_GLS_RACP_RESPONSE_CODE_T_MIN_PACKED_SIZE                                               (2)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_GLS_Time_Offset_t(qapi_BLE_GLS_Time_Offset_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Concentration_t(qapi_BLE_GLS_Concentration_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Sensor_Status_Annunciation_t(qapi_BLE_GLS_Sensor_Status_Annunciation_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Glucose_Measurement_Context_Header_t(qapi_BLE_GLS_Glucose_Measurement_Context_Header_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Context_Extended_Flags_t(qapi_BLE_GLS_Context_Extended_Flags_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Context_Carbohydrate_t(qapi_BLE_GLS_Context_Carbohydrate_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Context_Meal_t(qapi_BLE_GLS_Context_Meal_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Health_Tester_t(qapi_BLE_GLS_Health_Tester_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Context_Exercise_t(qapi_BLE_GLS_Context_Exercise_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Context_Medication_t(qapi_BLE_GLS_Context_Medication_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Context_HBA1C_t(qapi_BLE_GLS_Context_HBA1C_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Sequence_Number_Range_t(qapi_BLE_GLS_Sequence_Number_Range_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_Date_Time_Range_t(qapi_BLE_GLS_Date_Time_Range_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_GLS_RACP_Response_Code_t(qapi_BLE_GLS_RACP_Response_Code_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_GLS_Time_Offset_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Time_Offset_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Concentration_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Concentration_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Sensor_Status_Annunciation_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Sensor_Status_Annunciation_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Glucose_Measurement_Context_Header_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Glucose_Measurement_Context_Header_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Context_Extended_Flags_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Context_Extended_Flags_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Context_Carbohydrate_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Context_Carbohydrate_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Context_Meal_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Context_Meal_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Health_Tester_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Health_Tester_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Context_Exercise_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Context_Exercise_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Context_Medication_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Context_Medication_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Context_HBA1C_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Context_HBA1C_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Sequence_Number_Range_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Sequence_Number_Range_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_Date_Time_Range_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_Date_Time_Range_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_GLS_RACP_Response_Code_t(PackedBuffer_t *Buffer, qapi_BLE_GLS_RACP_Response_Code_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_GLS_Time_Offset_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Time_Offset_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Concentration_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Concentration_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Sensor_Status_Annunciation_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Sensor_Status_Annunciation_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Glucose_Measurement_Context_Header_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Glucose_Measurement_Context_Header_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Context_Extended_Flags_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Context_Extended_Flags_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Context_Carbohydrate_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Context_Carbohydrate_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Context_Meal_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Context_Meal_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Health_Tester_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Health_Tester_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Context_Exercise_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Context_Exercise_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Context_Medication_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Context_Medication_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Context_HBA1C_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Context_HBA1C_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Sequence_Number_Range_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Sequence_Number_Range_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_Date_Time_Range_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_Date_Time_Range_t *Structure);
SerStatus_t PackedRead_qapi_BLE_GLS_RACP_Response_Code_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_GLS_RACP_Response_Code_t *Structure);

#endif // __QAPI_BLE_GLSTYPES_COMMON_H__
