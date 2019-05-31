/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_CPS_COMMON_H__
#define __QAPI_BLE_CPS_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_cps.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_cpstypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_CPS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE                                               (18)
#define QAPI_BLE_CPS_SERVER_INFORMATION_T_MIN_PACKED_SIZE                                               (8)
#define QAPI_BLE_CPS_WHEEL_REVOLUTION_DATA_T_MIN_PACKED_SIZE                                            (6)
#define QAPI_BLE_CPS_CRANK_REVOLUTION_DATA_T_MIN_PACKED_SIZE                                            (4)
#define QAPI_BLE_CPS_EXTREME_FORCE_MAGNITUDES_DATA_T_MIN_PACKED_SIZE                                    (4)
#define QAPI_BLE_CPS_EXTREME_TORQUE_MAGNITUDES_DATA_T_MIN_PACKED_SIZE                                   (4)
#define QAPI_BLE_CPS_EXTREME_ANGLES_DATA_T_MIN_PACKED_SIZE                                              (4)
#define QAPI_BLE_CPS_MEASUREMENT_DATA_T_MIN_PACKED_SIZE                                                 (13)
#define QAPI_BLE_CPS_VECTOR_DATA_T_MIN_PACKED_SIZE                                                      ((4) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_CPS_SUPPORTED_SENSOR_LOCATIONS_T_MIN_PACKED_SIZE                                       ((1) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_BLE_CPS_DATE_TIME_DATA_T_MIN_PACKED_SIZE                                                   (7)
#define QAPI_BLE_CPS_CONTROL_POINT_INDICATION_DATA_T_MIN_PACKED_SIZE                                    (1)
#define QAPI_BLE_CPS_CONTROL_POINT_RESPONSE_DATA_T_MIN_PACKED_SIZE                                      (2)
#define QAPI_BLE_CPS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE                                   (20)
#define QAPI_BLE_CPS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE                                 (22)
#define QAPI_BLE_CPS_READ_CP_MEASUREMENT_SERVER_CONFIGURATION_DATA_T_MIN_PACKED_SIZE                    (16)
#define QAPI_BLE_CPS_CP_MEASUREMENT_SERVER_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE                  (14)
#define QAPI_BLE_CPS_CONTROL_POINT_FORMAT_DATA_T_MIN_PACKED_SIZE                                        (4)
#define QAPI_BLE_CPS_CONTROL_POINT_COMMAND_DATA_T_MIN_PACKED_SIZE                                       (16)
#define QAPI_BLE_CPS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE                                                (13)
#define QAPI_BLE_CPS_EVENT_DATA_T_MIN_PACKED_SIZE                                                       ((6) + (QS_POINTER_HEADER_SIZE) * (1))

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_CPS_Client_Information_t(qapi_BLE_CPS_Client_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CPS_Server_Information_t(qapi_BLE_CPS_Server_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CPS_Wheel_Revolution_Data_t(qapi_BLE_CPS_Wheel_Revolution_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CPS_Crank_Revolution_Data_t(qapi_BLE_CPS_Crank_Revolution_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CPS_Extreme_Force_Magnitudes_Data_t(qapi_BLE_CPS_Extreme_Force_Magnitudes_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CPS_Extreme_Torque_Magnitudes_Data_t(qapi_BLE_CPS_Extreme_Torque_Magnitudes_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CPS_Extreme_Angles_Data_t(qapi_BLE_CPS_Extreme_Angles_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CPS_Measurement_Data_t(qapi_BLE_CPS_Measurement_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CPS_Vector_Data_t(qapi_BLE_CPS_Vector_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CPS_Supported_Sensor_Locations_t(qapi_BLE_CPS_Supported_Sensor_Locations_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CPS_Date_Time_Data_t(qapi_BLE_CPS_Date_Time_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CPS_Control_Point_Indication_Data_t(qapi_BLE_CPS_Control_Point_Indication_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CPS_Control_Point_Response_Data_t(qapi_BLE_CPS_Control_Point_Response_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CPS_Read_Client_Configuration_Data_t(qapi_BLE_CPS_Read_Client_Configuration_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CPS_Client_Configuration_Update_Data_t(qapi_BLE_CPS_Client_Configuration_Update_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Data_t(qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CPS_CP_Measurement_Server_Configuration_Update_Data_t(qapi_BLE_CPS_CP_Measurement_Server_Configuration_Update_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CPS_Control_Point_Format_Data_t(qapi_BLE_CPS_Control_Point_Format_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CPS_Control_Point_Command_Data_t(qapi_BLE_CPS_Control_Point_Command_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CPS_Confirmation_Data_t(qapi_BLE_CPS_Confirmation_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CPS_Event_Data_t(qapi_BLE_CPS_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_CPS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Client_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CPS_Server_Information_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Server_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CPS_Wheel_Revolution_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Wheel_Revolution_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CPS_Crank_Revolution_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Crank_Revolution_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CPS_Extreme_Force_Magnitudes_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Extreme_Force_Magnitudes_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CPS_Extreme_Torque_Magnitudes_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Extreme_Torque_Magnitudes_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CPS_Extreme_Angles_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Extreme_Angles_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CPS_Measurement_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Measurement_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CPS_Vector_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Vector_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CPS_Supported_Sensor_Locations_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Supported_Sensor_Locations_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CPS_Date_Time_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Date_Time_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CPS_Control_Point_Indication_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Control_Point_Indication_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CPS_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Control_Point_Response_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CPS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Read_Client_Configuration_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CPS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Client_Configuration_Update_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CPS_CP_Measurement_Server_Configuration_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_CP_Measurement_Server_Configuration_Update_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CPS_Control_Point_Format_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Control_Point_Format_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CPS_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Control_Point_Command_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CPS_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Confirmation_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CPS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CPS_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_CPS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Client_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CPS_Server_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Server_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CPS_Wheel_Revolution_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Wheel_Revolution_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CPS_Crank_Revolution_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Crank_Revolution_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CPS_Extreme_Force_Magnitudes_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Extreme_Force_Magnitudes_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CPS_Extreme_Torque_Magnitudes_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Extreme_Torque_Magnitudes_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CPS_Extreme_Angles_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Extreme_Angles_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CPS_Measurement_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Measurement_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CPS_Vector_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Vector_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CPS_Supported_Sensor_Locations_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Supported_Sensor_Locations_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CPS_Date_Time_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Date_Time_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CPS_Control_Point_Indication_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Control_Point_Indication_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CPS_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Control_Point_Response_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CPS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Read_Client_Configuration_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CPS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Client_Configuration_Update_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CPS_CP_Measurement_Server_Configuration_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_CP_Measurement_Server_Configuration_Update_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CPS_Control_Point_Format_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Control_Point_Format_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CPS_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Control_Point_Command_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CPS_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Confirmation_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CPS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CPS_Event_Data_t *Structure);

#endif // __QAPI_BLE_CPS_COMMON_H__
