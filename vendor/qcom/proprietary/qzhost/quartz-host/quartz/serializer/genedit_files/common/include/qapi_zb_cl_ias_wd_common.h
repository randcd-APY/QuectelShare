/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_ZB_CL_IAS_WD_COMMON_H__
#define __QAPI_ZB_CL_IAS_WD_COMMON_H__
#include "qsCommon.h"
#include "qapi_zb.h"
#include "qapi_zb_cl_ias_wd.h"
#include "qapi_zb_cl_common.h"
#include "qapi_zb_cl_ias_zone_common.h"

/* Packed structure minimum size macros. */
#define QAPI_ZB_CL_IASWD_START_WARNING_T_MIN_PACKED_SIZE                                                (19)
#define QAPI_ZB_CL_IASWD_START_WARNING_EVENT_T_MIN_PACKED_SIZE                                          (19)
#define QAPI_ZB_CL_IASWD_SQUAWK_T_MIN_PACKED_SIZE                                                       (12)
#define QAPI_ZB_CL_IASWD_SQUAWK_EVENT_T_MIN_PACKED_SIZE                                                 (12)
#define QAPI_ZB_CL_IASWD_CLIENT_EVENT_DATA_T_MIN_PACKED_SIZE                                            (4)
#define QAPI_ZB_CL_IASWD_SERVER_EVENT_DATA_T_MIN_PACKED_SIZE                                            (4)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_ZB_CL_IASWD_Start_Warning_t(qapi_ZB_CL_IASWD_Start_Warning_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_IASWD_Start_Warning_Event_t(qapi_ZB_CL_IASWD_Start_Warning_Event_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_IASWD_Squawk_t(qapi_ZB_CL_IASWD_Squawk_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_IASWD_Squawk_Event_t(qapi_ZB_CL_IASWD_Squawk_Event_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_IASWD_Client_Event_Data_t(qapi_ZB_CL_IASWD_Client_Event_Data_t *Structure);
uint32_t CalcPackedSize_qapi_ZB_CL_IASWD_Server_Event_Data_t(qapi_ZB_CL_IASWD_Server_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_ZB_CL_IASWD_Start_Warning_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASWD_Start_Warning_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_IASWD_Start_Warning_Event_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASWD_Start_Warning_Event_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_IASWD_Squawk_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASWD_Squawk_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_IASWD_Squawk_Event_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASWD_Squawk_Event_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_IASWD_Client_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASWD_Client_Event_Data_t *Structure);
SerStatus_t PackedWrite_qapi_ZB_CL_IASWD_Server_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_IASWD_Server_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_ZB_CL_IASWD_Start_Warning_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASWD_Start_Warning_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_IASWD_Start_Warning_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASWD_Start_Warning_Event_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_IASWD_Squawk_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASWD_Squawk_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_IASWD_Squawk_Event_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASWD_Squawk_Event_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_IASWD_Client_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASWD_Client_Event_Data_t *Structure);
SerStatus_t PackedRead_qapi_ZB_CL_IASWD_Server_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_IASWD_Server_Event_Data_t *Structure);

#endif // __QAPI_ZB_CL_IAS_WD_COMMON_H__
