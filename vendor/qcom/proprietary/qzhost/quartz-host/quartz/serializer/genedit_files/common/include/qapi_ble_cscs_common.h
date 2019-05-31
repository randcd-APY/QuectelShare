/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_CSCS_COMMON_H__
#define __QAPI_BLE_CSCS_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_cscs.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_cscstypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_CSCS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE                                              (12)
#define QAPI_BLE_CSCS_SERVER_INFORMATION_T_MIN_PACKED_SIZE                                              (4)
#define QAPI_BLE_CSCS_WHEEL_REVOLUTION_DATA_T_MIN_PACKED_SIZE                                           (6)
#define QAPI_BLE_CSCS_CRANK_REVOLUTION_DATA_T_MIN_PACKED_SIZE                                           (4)
#define QAPI_BLE_CSCS_CSC_MEASUREMENT_DATA_T_MIN_PACKED_SIZE                                            (1)
#define QAPI_BLE_CSCS_SCCP_SUPPORTED_SENSOR_LOCATIONS_T_MIN_PACKED_SIZE                                 (1)
#define QAPI_BLE_CSCS_SC_CONTROL_POINT_RESPONSE_DATA_T_MIN_PACKED_SIZE                                  (2)
#define QAPI_BLE_CSCS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE                                  (20)
#define QAPI_BLE_CSCS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE                                (18)
#define QAPI_BLE_CSCS_SC_CONTROL_POINT_FORMAT_DATA_T_MIN_PACKED_SIZE                                    (4)
#define QAPI_BLE_CSCS_SC_CONTROL_POINT_COMMAND_DATA_T_MIN_PACKED_SIZE                                   (16)
#define QAPI_BLE_CSCS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE                                               (13)
#define QAPI_BLE_CSCS_EVENT_DATA_T_MIN_PACKED_SIZE                                                      ((6) + (QS_POINTER_HEADER_SIZE) * (1))

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_CSCS_Client_Information_t(qapi_BLE_CSCS_Client_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CSCS_Server_Information_t(qapi_BLE_CSCS_Server_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CSCS_Wheel_Revolution_Data_t(qapi_BLE_CSCS_Wheel_Revolution_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CSCS_Crank_Revolution_Data_t(qapi_BLE_CSCS_Crank_Revolution_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CSCS_CSC_Measurement_Data_t(qapi_BLE_CSCS_CSC_Measurement_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CSCS_SCCP_Supported_Sensor_Locations_t(qapi_BLE_CSCS_SCCP_Supported_Sensor_Locations_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CSCS_SC_Control_Point_Response_Data_t(qapi_BLE_CSCS_SC_Control_Point_Response_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CSCS_Read_Client_Configuration_Data_t(qapi_BLE_CSCS_Read_Client_Configuration_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CSCS_Client_Configuration_Update_Data_t(qapi_BLE_CSCS_Client_Configuration_Update_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CSCS_SC_Control_Point_Format_Data_t(qapi_BLE_CSCS_SC_Control_Point_Format_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CSCS_SC_Control_Point_Command_Data_t(qapi_BLE_CSCS_SC_Control_Point_Command_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CSCS_Confirmation_Data_t(qapi_BLE_CSCS_Confirmation_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_CSCS_Event_Data_t(qapi_BLE_CSCS_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_CSCS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_CSCS_Client_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CSCS_Server_Information_t(PackedBuffer_t *Buffer, qapi_BLE_CSCS_Server_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CSCS_Wheel_Revolution_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CSCS_Wheel_Revolution_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CSCS_Crank_Revolution_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CSCS_Crank_Revolution_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CSCS_CSC_Measurement_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CSCS_CSC_Measurement_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CSCS_SCCP_Supported_Sensor_Locations_t(PackedBuffer_t *Buffer, qapi_BLE_CSCS_SCCP_Supported_Sensor_Locations_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CSCS_SC_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CSCS_SC_Control_Point_Response_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CSCS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CSCS_Read_Client_Configuration_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CSCS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CSCS_Client_Configuration_Update_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CSCS_SC_Control_Point_Format_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CSCS_SC_Control_Point_Format_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CSCS_SC_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CSCS_SC_Control_Point_Command_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CSCS_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CSCS_Confirmation_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_CSCS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_CSCS_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_CSCS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CSCS_Client_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CSCS_Server_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CSCS_Server_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CSCS_Wheel_Revolution_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CSCS_Wheel_Revolution_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CSCS_Crank_Revolution_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CSCS_Crank_Revolution_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CSCS_CSC_Measurement_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CSCS_CSC_Measurement_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CSCS_SCCP_Supported_Sensor_Locations_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CSCS_SCCP_Supported_Sensor_Locations_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CSCS_SC_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CSCS_SC_Control_Point_Response_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CSCS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CSCS_Read_Client_Configuration_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CSCS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CSCS_Client_Configuration_Update_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CSCS_SC_Control_Point_Format_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CSCS_SC_Control_Point_Format_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CSCS_SC_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CSCS_SC_Control_Point_Command_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CSCS_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CSCS_Confirmation_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_CSCS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_CSCS_Event_Data_t *Structure);

#endif // __QAPI_BLE_CSCS_COMMON_H__
