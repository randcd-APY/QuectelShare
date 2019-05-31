/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_ZB_CL_LEVEL_CONTROL_COMMON_H__
#define __QAPI_ZB_CL_LEVEL_CONTROL_COMMON_H__
#include "qsCommon.h"
#include "qapi_zb.h"
#include "qapi_zb_cl_level_control.h"
#include "qapi_zb_cl_common.h"

/* Packed structure minimum size macros. */
#define QAPI_ZB_CL_LEVELCONTROL_CLIENT_EVENT_DATA_T_MIN_PACKED_SIZE                                     (4)
#define QAPI_ZB_CL_LEVELCONTROL_SERVER_TRANSITION_T_MIN_PACKED_SIZE                                     (7)
#define QAPI_ZB_CL_LEVELCONTROL_SERVER_EVENT_DATA_T_MIN_PACKED_SIZE                                     (4)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_ZB_CL_LevelControl_Client_Event_Data_t(qapi_ZB_CL_LevelControl_Client_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_LevelControl_Server_Transition_t(qapi_ZB_CL_LevelControl_Server_Transition_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_LevelControl_Server_Event_Data_t(qapi_ZB_CL_LevelControl_Server_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_ZB_CL_LevelControl_Client_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_LevelControl_Client_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_LevelControl_Server_Transition_t(PackedBuffer_t *Buffer, qapi_ZB_CL_LevelControl_Server_Transition_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_LevelControl_Server_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_LevelControl_Server_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_ZB_CL_LevelControl_Client_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_LevelControl_Client_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_LevelControl_Server_Transition_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_LevelControl_Server_Transition_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_LevelControl_Server_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_LevelControl_Server_Event_Data_t *Structure);

#endif // __QAPI_ZB_CL_LEVEL_CONTROL_COMMON_H__
