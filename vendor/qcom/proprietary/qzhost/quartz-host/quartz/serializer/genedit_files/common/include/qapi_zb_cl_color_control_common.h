/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_ZB_CL_COLOR_CONTROL_COMMON_H__
#define __QAPI_ZB_CL_COLOR_CONTROL_COMMON_H__
#include "qsCommon.h"
#include "qapi_zb.h"
#include "qapi_zb_cl_color_control.h"
#include "qapi_zb_cl_common.h"

/* Packed structure minimum size macros. */
#define QAPI_ZB_CL_COLORCONTROL_COLOR_LOOP_SET_T_MIN_PACKED_SIZE                                        (13)
#define QAPI_ZB_CL_COLORCONTROL_STEP_COLOR_TEMP_T_MIN_PACKED_SIZE                                       (10)
#define QAPI_ZB_CL_COLORCONTROL_CLIENT_EVENT_DATA_T_MIN_PACKED_SIZE                                     (4)
#define QAPI_ZB_CL_COLORCONTROL_SERVER_MOVE_TO_HUE_AND_SAT_T_MIN_PACKED_SIZE                            (13)
#define QAPI_ZB_CL_COLORCONTROL_SERVER_MOVE_TO_COLOR_T_MIN_PACKED_SIZE                                  (12)
#define QAPI_ZB_CL_COLORCONTROL_SERVER_MOVE_TO_COLOR_TEMP_T_MIN_PACKED_SIZE                             (8)
#define QAPI_ZB_CL_COLORCONTROL_SERVER_EVENT_DATA_T_MIN_PACKED_SIZE                                     (4)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_ZB_CL_ColorControl_Color_Loop_Set_t(qapi_ZB_CL_ColorControl_Color_Loop_Set_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_ColorControl_Step_Color_Temp_t(qapi_ZB_CL_ColorControl_Step_Color_Temp_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_ColorControl_Client_Event_Data_t(qapi_ZB_CL_ColorControl_Client_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_ColorControl_Server_Move_To_Hue_And_Sat_t(qapi_ZB_CL_ColorControl_Server_Move_To_Hue_And_Sat_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_ColorControl_Server_Move_To_Color_t(qapi_ZB_CL_ColorControl_Server_Move_To_Color_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_ColorControl_Server_Move_To_Color_Temp_t(qapi_ZB_CL_ColorControl_Server_Move_To_Color_Temp_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_ColorControl_Server_Event_Data_t(qapi_ZB_CL_ColorControl_Server_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_ZB_CL_ColorControl_Color_Loop_Set_t(PackedBuffer_t *Buffer, qapi_ZB_CL_ColorControl_Color_Loop_Set_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_ColorControl_Step_Color_Temp_t(PackedBuffer_t *Buffer, qapi_ZB_CL_ColorControl_Step_Color_Temp_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_ColorControl_Client_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_ColorControl_Client_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_ColorControl_Server_Move_To_Hue_And_Sat_t(PackedBuffer_t *Buffer, qapi_ZB_CL_ColorControl_Server_Move_To_Hue_And_Sat_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_ColorControl_Server_Move_To_Color_t(PackedBuffer_t *Buffer, qapi_ZB_CL_ColorControl_Server_Move_To_Color_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_ColorControl_Server_Move_To_Color_Temp_t(PackedBuffer_t *Buffer, qapi_ZB_CL_ColorControl_Server_Move_To_Color_Temp_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_ColorControl_Server_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_ColorControl_Server_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_ZB_CL_ColorControl_Color_Loop_Set_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_ColorControl_Color_Loop_Set_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_ColorControl_Step_Color_Temp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_ColorControl_Step_Color_Temp_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_ColorControl_Client_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_ColorControl_Client_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_ColorControl_Server_Move_To_Hue_And_Sat_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_ColorControl_Server_Move_To_Hue_And_Sat_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_ColorControl_Server_Move_To_Color_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_ColorControl_Server_Move_To_Color_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_ColorControl_Server_Move_To_Color_Temp_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_ColorControl_Server_Move_To_Color_Temp_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_ColorControl_Server_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_ColorControl_Server_Event_Data_t *Structure);

#endif // __QAPI_ZB_CL_COLOR_CONTROL_COMMON_H__
