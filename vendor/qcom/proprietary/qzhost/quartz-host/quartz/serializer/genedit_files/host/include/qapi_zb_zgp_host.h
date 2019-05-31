/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_zb_zgp.h"

qapi_Status_t _qapi_ZB_ZGP_Register_Callback(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_ZGP_Event_CB_t ZGP_Event_CB, uint32_t CB_Param);

qapi_Status_t _qapi_ZB_ZGP_Unregister_Callback(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_ZGP_Event_CB_t ZGP_Event_CB);

qapi_Status_t _qapi_ZB_ZGP_Data_Request(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_ZGP_GP_Data_Request_t *Data_Request);

qapi_Status_t _qapi_ZB_ZGP_Add_Key(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_ZGP_Key_Request_t *Key_Request);
