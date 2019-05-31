/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QAPI_ZB_CL_POWER_CONFIG_COMMON_H__
#define __QAPI_ZB_CL_POWER_CONFIG_COMMON_H__
#include "qsCommon.h"
#include "qapi_zb.h"
#include "qapi_zb_cl_power_config.h"
#include "qapi_zb_cl_common.h"

/* Packed structure minimum size macros. */
#define QAPI_ZB_CL_POWERCONFIG_SERVER_EVENT_DATA_T_MIN_PACKED_SIZE                                      (4)

/* Packed structure size definitions. */
uint32_t CalcPackedSize_qapi_ZB_CL_PowerConfig_Server_Event_Data_t(qapi_ZB_CL_PowerConfig_Server_Event_Data_t *Structure);

/* Pack structure function definitions. */
SerStatus_t PackedWrite_qapi_ZB_CL_PowerConfig_Server_Event_Data_t(PackedBuffer_t *Buffer, qapi_ZB_CL_PowerConfig_Server_Event_Data_t *Structure);

/* Unpack structure function definitions. */
SerStatus_t PackedRead_qapi_ZB_CL_PowerConfig_Server_Event_Data_t(PackedBuffer_t *Buffer, BufferListEntry_t **BufferList, qapi_ZB_CL_PowerConfig_Server_Event_Data_t *Structure);

#endif // __QAPI_ZB_CL_POWER_CONFIG_COMMON_H__
