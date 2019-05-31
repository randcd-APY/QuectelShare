/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_BLE_RSCS_COMMON_H__
#define __QAPI_BLE_RSCS_COMMON_H__
#include "qsCommon.h"
#include "qapi_ble.h"
#include "qapi_ble_rscs.h"
#include "qapi_ble_btapityp_common.h"
#include "qapi_ble_bttypes_common.h"
#include "qapi_ble_gatt_common.h"
#include "qapi_ble_rscstypes_common.h"

/* Packed structure minimum size macros. */
#define QAPI_BLE_RSCS_CLIENT_INFORMATION_T_MIN_PACKED_SIZE                                              (12)
#define QAPI_BLE_RSCS_SERVER_INFORMATION_T_MIN_PACKED_SIZE                                              (4)
#define QAPI_BLE_RSCS_RSC_MEASUREMENT_DATA_T_MIN_PACKED_SIZE                                            (10)
#define QAPI_BLE_RSCS_READ_CLIENT_CONFIGURATION_DATA_T_MIN_PACKED_SIZE                                  (20)
#define QAPI_BLE_RSCS_CLIENT_CONFIGURATION_UPDATE_DATA_T_MIN_PACKED_SIZE                                (22)
#define QAPI_BLE_RSCS_SCCP_SUPPORTED_SENSOR_LOCATIONS_T_MIN_PACKED_SIZE                                 (1)
#define QAPI_BLE_RSCS_SC_CONTROL_POINT_FORMAT_DATA_T_MIN_PACKED_SIZE                                    (4)
#define QAPI_BLE_RSCS_SC_CONTROL_POINT_RESPONSE_DATA_T_MIN_PACKED_SIZE                                  (5)
#define QAPI_BLE_RSCS_SC_CONTROL_POINT_COMMAND_DATA_T_MIN_PACKED_SIZE                                   (16)
#define QAPI_BLE_RSCS_CONFIRMATION_DATA_T_MIN_PACKED_SIZE                                               (17)
#define QAPI_BLE_RSCS_EVENT_DATA_T_MIN_PACKED_SIZE                                                      ((6) + (QS_POINTER_HEADER_SIZE) * (1))

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_BLE_RSCS_Client_Information_t(qapi_BLE_RSCS_Client_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_RSCS_Server_Information_t(qapi_BLE_RSCS_Server_Information_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_RSCS_RSC_Measurement_Data_t(qapi_BLE_RSCS_RSC_Measurement_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_RSCS_Read_Client_Configuration_Data_t(qapi_BLE_RSCS_Read_Client_Configuration_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_RSCS_Client_Configuration_Update_Data_t(qapi_BLE_RSCS_Client_Configuration_Update_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_RSCS_SCCP_Supported_Sensor_Locations_t(qapi_BLE_RSCS_SCCP_Supported_Sensor_Locations_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_RSCS_SC_Control_Point_Format_Data_t(qapi_BLE_RSCS_SC_Control_Point_Format_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_RSCS_SC_Control_Point_Response_Data_t(qapi_BLE_RSCS_SC_Control_Point_Response_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_RSCS_SC_Control_Point_Command_Data_t(qapi_BLE_RSCS_SC_Control_Point_Command_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_RSCS_Confirmation_Data_t(qapi_BLE_RSCS_Confirmation_Data_t *Structure);
uint32_t CalcPackedSize_qapi_BLE_RSCS_Event_Data_t(qapi_BLE_RSCS_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_BLE_RSCS_Client_Information_t(PackedBuffer_t *Buffer, qapi_BLE_RSCS_Client_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_RSCS_Server_Information_t(PackedBuffer_t *Buffer, qapi_BLE_RSCS_Server_Information_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_RSCS_RSC_Measurement_Data_t(PackedBuffer_t *Buffer, qapi_BLE_RSCS_RSC_Measurement_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_RSCS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, qapi_BLE_RSCS_Read_Client_Configuration_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_RSCS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, qapi_BLE_RSCS_Client_Configuration_Update_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_RSCS_SCCP_Supported_Sensor_Locations_t(PackedBuffer_t *Buffer, qapi_BLE_RSCS_SCCP_Supported_Sensor_Locations_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_RSCS_SC_Control_Point_Format_Data_t(PackedBuffer_t *Buffer, qapi_BLE_RSCS_SC_Control_Point_Format_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_RSCS_SC_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, qapi_BLE_RSCS_SC_Control_Point_Response_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_RSCS_SC_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, qapi_BLE_RSCS_SC_Control_Point_Command_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_RSCS_Confirmation_Data_t(PackedBuffer_t *Buffer, qapi_BLE_RSCS_Confirmation_Data_t *Structure);
SerStatus_t PackedWrite_qapi_BLE_RSCS_Event_Data_t(PackedBuffer_t *Buffer, qapi_BLE_RSCS_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_BLE_RSCS_Client_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_RSCS_Client_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_RSCS_Server_Information_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_RSCS_Server_Information_t *Structure);
SerStatus_t PackedRead_qapi_BLE_RSCS_RSC_Measurement_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_RSCS_RSC_Measurement_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_RSCS_Read_Client_Configuration_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_RSCS_Read_Client_Configuration_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_RSCS_Client_Configuration_Update_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_RSCS_Client_Configuration_Update_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_RSCS_SCCP_Supported_Sensor_Locations_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_RSCS_SCCP_Supported_Sensor_Locations_t *Structure);
SerStatus_t PackedRead_qapi_BLE_RSCS_SC_Control_Point_Format_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_RSCS_SC_Control_Point_Format_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_RSCS_SC_Control_Point_Response_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_RSCS_SC_Control_Point_Response_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_RSCS_SC_Control_Point_Command_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_RSCS_SC_Control_Point_Command_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_RSCS_Confirmation_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_RSCS_Confirmation_Data_t *Structure);
SerStatus_t PackedRead_qapi_BLE_RSCS_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_BLE_RSCS_Event_Data_t *Structure);

#endif // __QAPI_BLE_RSCS_COMMON_H__
