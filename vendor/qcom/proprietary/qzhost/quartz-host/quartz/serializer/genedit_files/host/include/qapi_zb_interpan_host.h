/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_zb_interpan.h"

qapi_Status_t _qapi_ZB_InterPAN_Register_Callback(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_InterPAN_Event_CB_t InterPAN_Event_CB, uint32_t CB_Param);

qapi_Status_t _qapi_ZB_InterPAN_Unregister_Callback(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_InterPAN_Event_CB_t InterPAN_Event_CB);

qapi_Status_t _qapi_ZB_InterPAN_Data_Request(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_InterPAN_Data_Request_t *Data_Request);
