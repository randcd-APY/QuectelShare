/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_zb_whitelist.h"

qapi_Status_t _qapi_ZB_Whitelist_Enable(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qbool_t Enable);

qapi_Status_t _qapi_ZB_Whitelist_Add(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, uint64_t ExtendedAddress, uint16_t ShortAddress);

qapi_Status_t _qapi_ZB_Whitelist_Remove(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, uint64_t ExtendedAddress, uint16_t ShortAddress);
