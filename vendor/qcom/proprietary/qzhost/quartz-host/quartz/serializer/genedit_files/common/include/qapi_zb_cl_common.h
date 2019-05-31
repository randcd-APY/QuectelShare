/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_ZB_CL_COMMON_H__
#define __QAPI_ZB_CL_COMMON_H__
#include "qsCommon.h"
#include "qapi_zb.h"
#include "qapi_zb_cl.h"
#include "qapi_zb_common.h"
#include "qapi_zb_aps_common.h"

/* Packed structure minimum size macros. */
#define QAPI_ZB_CL_ATTR_CUSTOM_READ_T_MIN_PACKED_SIZE                                                   ((2) + (QS_POINTER_HEADER_SIZE) * (3))
#define QAPI_ZB_CL_ATTR_CUSTOM_WRITE_T_MIN_PACKED_SIZE                                                  ((8) + (QS_POINTER_HEADER_SIZE) * (2))
#define QAPI_ZB_CL_ATTRIBUTE_T_MIN_PACKED_SIZE                                                          (32)
#define QAPI_ZB_CL_CLUSTER_INFO_T_MIN_PACKED_SIZE                                                       ((2) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_CL_HEADER_T_MIN_PACKED_SIZE                                                             (5)
#define QAPI_ZB_CL_GENERAL_SEND_INFO_T_MIN_PACKED_SIZE                                                  (6)
#define QAPI_ZB_CL_GENERAL_RECEIVE_INFO_T_MIN_PACKED_SIZE                                               (14)
#define QAPI_ZB_CL_READ_ATTR_STATUS_RECORD_T_MIN_PACKED_SIZE                                            ((12) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_CL_READ_ATTR_RESPONSE_T_MIN_PACKED_SIZE                                                 ((6) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_CL_WRITE_ATTR_RECORD_T_MIN_PACKED_SIZE                                                  ((8) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_CL_WRITE_ATTR_STATUS_RECORD_T_MIN_PACKED_SIZE                                           (6)
#define QAPI_ZB_CL_WRITE_ATTR_RESPONSE_T_MIN_PACKED_SIZE                                                ((6) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_CL_ATTR_REPORTING_CONFIG_RECORD_T_MIN_PACKED_SIZE                                       (24)
#define QAPI_ZB_CL_ATTR_STATUS_RECORD_T_MIN_PACKED_SIZE                                                 (10)
#define QAPI_ZB_CL_CONFIG_REPORT_RESPONSE_T_MIN_PACKED_SIZE                                             ((6) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_CL_ATTR_RECORD_T_MIN_PACKED_SIZE                                                        (6)
#define QAPI_ZB_CL_REPORTING_CONFIG_STATUS_RECORD_T_MIN_PACKED_SIZE                                     (30)
#define QAPI_ZB_CL_READ_REPORT_CONFIG_RESPONSE_T_MIN_PACKED_SIZE                                        ((5) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_CL_ATTR_REPORT_T_MIN_PACKED_SIZE                                                        ((7) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_CL_DEFAULT_RESPONSE_T_MIN_PACKED_SIZE                                                   (9)
#define QAPI_ZB_CL_COMMAND_COMPLETE_T_MIN_PACKED_SIZE                                                   (4)
#define QAPI_ZB_CL_DISCOVER_ATTR_REPORT_T_MIN_PACKED_SIZE                                               (6)
#define QAPI_ZB_CL_DISCOVER_ATTR_RESPONSE_T_MIN_PACKED_SIZE                                             ((9) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_CL_READ_ATTR_STRUCTURED_T_MIN_PACKED_SIZE                                               ((3) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_CL_WRITE_ATTR_STRUCTURED_RECORD_T_MIN_PACKED_SIZE                                       ((9) + (QS_POINTER_HEADER_SIZE) * (2))
#define QAPI_ZB_CL_WRITE_ATTR_STRUCTURED_STATUS_RECORD_T_MIN_PACKED_SIZE                                ((7) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_CL_WRITE_ATTR_STRUCTURED_RESPONSE_T_MIN_PACKED_SIZE                                     ((5) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_CL_UNPARSED_DATA_T_MIN_PACKED_SIZE                                                      ((QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_CL_UNPARSED_RESPONSE_T_MIN_PACKED_SIZE                                                  ((11) + (QS_POINTER_HEADER_SIZE) * (1))
#define QAPI_ZB_CL_EVENT_DATA_T_MIN_PACKED_SIZE                                                         (4)
#define QAPI_ZB_CL_CUSTOM_CLUSTER_COMMAND_T_MIN_PACKED_SIZE                                             (0)
#define QAPI_ZB_CL_CUSTOM_CLUSTER_GET_SCENE_DATA_T_MIN_PACKED_SIZE                                      ((QS_POINTER_HEADER_SIZE) * (3))
#define QAPI_ZB_CL_CUSTOM_CLUSTER_SET_SCENE_DATA_T_MIN_PACKED_SIZE                                      ((3) + (QS_POINTER_HEADER_SIZE) * (2))
#define QAPI_ZB_CL_CUSTOM_CLUSTER_RESET_ALARM_T_MIN_PACKED_SIZE                                         (3)
#define QAPI_ZB_CL_CUSTOM_CLUSTER_ATTR_SET_DEFAULT_T_MIN_PACKED_SIZE                                    (2)
#define QAPI_ZB_CL_CUSTOM_CLUSTER_ATTR_WRITE_NOTIFICTION_T_MIN_PACKED_SIZE                              (2)
#define QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_DATA_T_MIN_PACKED_SIZE                                          (4)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_ZB_CL_Attr_Custom_Read_t(qapi_ZB_CL_Attr_Custom_Read_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Attr_Custom_Write_t(qapi_ZB_CL_Attr_Custom_Write_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Attribute_t(qapi_ZB_CL_Attribute_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Cluster_Info_t(qapi_ZB_CL_Cluster_Info_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Header_t(qapi_ZB_CL_Header_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_General_Send_Info_t(qapi_ZB_CL_General_Send_Info_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_General_Receive_Info_t(qapi_ZB_CL_General_Receive_Info_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Read_Attr_Status_Record_t(qapi_ZB_CL_Read_Attr_Status_Record_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Read_Attr_Response_t(qapi_ZB_CL_Read_Attr_Response_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Write_Attr_Record_t(qapi_ZB_CL_Write_Attr_Record_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Write_Attr_Status_Record_t(qapi_ZB_CL_Write_Attr_Status_Record_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Write_Attr_Response_t(qapi_ZB_CL_Write_Attr_Response_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Attr_Reporting_Config_Record_t(qapi_ZB_CL_Attr_Reporting_Config_Record_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Attr_Status_Record_t(qapi_ZB_CL_Attr_Status_Record_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Config_Report_Response_t(qapi_ZB_CL_Config_Report_Response_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Attr_Record_t(qapi_ZB_CL_Attr_Record_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Reporting_Config_Status_Record_t(qapi_ZB_CL_Reporting_Config_Status_Record_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Read_Report_Config_Response_t(qapi_ZB_CL_Read_Report_Config_Response_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Attr_Report_t(qapi_ZB_CL_Attr_Report_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Default_Response_t(qapi_ZB_CL_Default_Response_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Command_Complete_t(qapi_ZB_CL_Command_Complete_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Discover_Attr_Report_t(qapi_ZB_CL_Discover_Attr_Report_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Discover_Attr_Response_t(qapi_ZB_CL_Discover_Attr_Response_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Read_Attr_Structured_t(qapi_ZB_CL_Read_Attr_Structured_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Write_Attr_Structured_Record_t(qapi_ZB_CL_Write_Attr_Structured_Record_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Write_Attr_Structured_Status_Record_t(qapi_ZB_CL_Write_Attr_Structured_Status_Record_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Write_Attr_Structured_Response_t(qapi_ZB_CL_Write_Attr_Structured_Response_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Unparsed_Data_t(qapi_ZB_CL_Unparsed_Data_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Unparsed_Response_t(qapi_ZB_CL_Unparsed_Response_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Event_Data_t(qapi_ZB_CL_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Custom_Cluster_Command_t(qapi_ZB_CL_Custom_Cluster_Command_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Custom_Cluster_Get_Scene_Data_t(qapi_ZB_CL_Custom_Cluster_Get_Scene_Data_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Custom_Cluster_Set_Scene_Data_t(qapi_ZB_CL_Custom_Cluster_Set_Scene_Data_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Custom_Cluster_Reset_Alarm_t(qapi_ZB_CL_Custom_Cluster_Reset_Alarm_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Custom_Cluster_Attr_Set_Default_t(qapi_ZB_CL_Custom_Cluster_Attr_Set_Default_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Custom_Cluster_Attr_Write_Notifiction_t(qapi_ZB_CL_Custom_Cluster_Attr_Write_Notifiction_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_Custom_Cluster_Event_Data_t(qapi_ZB_CL_Custom_Cluster_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_ZB_CL_Attr_Custom_Read_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Attr_Custom_Read_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Attr_Custom_Write_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Attr_Custom_Write_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Attribute_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Attribute_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Cluster_Info_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Cluster_Info_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Header_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Header_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_General_Send_Info_t(PackedBuffer_t *Buffer, qapi_ZB_CL_General_Send_Info_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_General_Receive_Info_t(PackedBuffer_t *Buffer, qapi_ZB_CL_General_Receive_Info_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Read_Attr_Status_Record_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Read_Attr_Status_Record_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Read_Attr_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Read_Attr_Response_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Write_Attr_Record_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Write_Attr_Record_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Write_Attr_Status_Record_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Write_Attr_Status_Record_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Write_Attr_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Write_Attr_Response_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Attr_Reporting_Config_Record_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Attr_Reporting_Config_Record_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Attr_Status_Record_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Attr_Status_Record_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Config_Report_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Config_Report_Response_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Attr_Record_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Attr_Record_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Reporting_Config_Status_Record_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Reporting_Config_Status_Record_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Read_Report_Config_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Read_Report_Config_Response_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Attr_Report_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Attr_Report_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Default_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Default_Response_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Command_Complete_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Command_Complete_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Discover_Attr_Report_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Discover_Attr_Report_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Discover_Attr_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Discover_Attr_Response_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Read_Attr_Structured_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Read_Attr_Structured_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Write_Attr_Structured_Record_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Write_Attr_Structured_Record_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Write_Attr_Structured_Status_Record_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Write_Attr_Structured_Status_Record_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Write_Attr_Structured_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Write_Attr_Structured_Response_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Unparsed_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Unparsed_Data_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Unparsed_Response_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Unparsed_Response_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Custom_Cluster_Command_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Custom_Cluster_Command_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Custom_Cluster_Get_Scene_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Custom_Cluster_Get_Scene_Data_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Custom_Cluster_Set_Scene_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Custom_Cluster_Set_Scene_Data_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Custom_Cluster_Reset_Alarm_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Custom_Cluster_Reset_Alarm_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Custom_Cluster_Attr_Set_Default_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Custom_Cluster_Attr_Set_Default_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Custom_Cluster_Attr_Write_Notifiction_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Custom_Cluster_Attr_Write_Notifiction_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_Custom_Cluster_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_Custom_Cluster_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_ZB_CL_Attr_Custom_Read_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Attr_Custom_Read_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Attr_Custom_Write_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Attr_Custom_Write_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Attribute_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Attribute_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Cluster_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Cluster_Info_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Header_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Header_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_General_Send_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_General_Send_Info_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_General_Receive_Info_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_General_Receive_Info_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Read_Attr_Status_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Read_Attr_Status_Record_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Read_Attr_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Read_Attr_Response_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Write_Attr_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Write_Attr_Record_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Write_Attr_Status_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Write_Attr_Status_Record_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Write_Attr_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Write_Attr_Response_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Attr_Reporting_Config_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Attr_Reporting_Config_Record_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Attr_Status_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Attr_Status_Record_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Config_Report_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Config_Report_Response_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Attr_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Attr_Record_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Reporting_Config_Status_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Reporting_Config_Status_Record_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Read_Report_Config_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Read_Report_Config_Response_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Attr_Report_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Attr_Report_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Default_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Default_Response_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Command_Complete_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Command_Complete_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Discover_Attr_Report_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Discover_Attr_Report_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Discover_Attr_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Discover_Attr_Response_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Read_Attr_Structured_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Read_Attr_Structured_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Write_Attr_Structured_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Write_Attr_Structured_Record_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Write_Attr_Structured_Status_Record_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Write_Attr_Structured_Status_Record_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Write_Attr_Structured_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Write_Attr_Structured_Response_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Unparsed_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Unparsed_Data_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Unparsed_Response_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Unparsed_Response_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Custom_Cluster_Command_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Custom_Cluster_Command_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Custom_Cluster_Get_Scene_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Custom_Cluster_Get_Scene_Data_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Custom_Cluster_Set_Scene_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Custom_Cluster_Set_Scene_Data_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Custom_Cluster_Reset_Alarm_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Custom_Cluster_Reset_Alarm_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Custom_Cluster_Attr_Set_Default_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Custom_Cluster_Attr_Set_Default_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Custom_Cluster_Attr_Write_Notifiction_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Custom_Cluster_Attr_Write_Notifiction_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_Custom_Cluster_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_Custom_Cluster_Event_Data_t *Structure);

#endif // __QAPI_ZB_CL_COMMON_H__
