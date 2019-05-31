/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_coex.h"

qapi_Status_t _qapi_COEX_Configure(uint8_t TargetID, const qapi_COEX_Config_Data_t *config_Data);

qapi_Status_t _qapi_COEX_Configure_Advanced(uint8_t TargetID, const qapi_COEX_Advanced_Config_Data_t *advanced_Config_Data);

qapi_Status_t _qapi_COEX_Statistics_Enable(uint8_t TargetID, boolean enable, uint32 statistics_Mask, uint32 flags);

qapi_Status_t _qapi_COEX_Statistics_Get(uint8_t TargetID, qapi_COEX_Statistics_Data_t *statistics_Data, uint8 *statistics_Data_Length, uint32 statistics_Mask, boolean reset);
