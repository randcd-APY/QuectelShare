/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_zb_cl_time.h"

qapi_Status_t _qapi_ZB_CL_Time_Server_Read_Attribute(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, uint16_t AttrId, uint16_t *Length, uint8_t *Data);

qapi_Status_t _qapi_ZB_CL_Time_Server_Write_Attribute(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, uint16_t AttrId, uint16_t Length, const uint8_t *Data);
