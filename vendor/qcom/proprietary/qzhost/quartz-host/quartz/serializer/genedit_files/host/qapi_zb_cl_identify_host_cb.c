/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include "qsCommon.h"
#include "qsHost.h"
#include "qsCallback.h"
#include "qapi_zb.h"
#include "qapi_zb_cl_identify_host_cb_mnl.h"
#include "qapi_zb_cl_identify_common.h"
#include "qapi_zb_cl_identify_host_cb.h"

void Host_qapi_ZB_CL_Identify_Client_Event_CB_t_Handler(uint32_t CallbackID, CallbackInfo_t *Info, uint16_t BufferLength, uint8_t *Buffer)
{
    Mnl_Host_qapi_ZB_CL_Identify_Client_Event_CB_t_Handler(CallbackID, Info, BufferLength, Buffer);
}
void Host_qapi_ZB_CL_Identify_Server_Event_CB_t_Handler(uint32_t CallbackID, CallbackInfo_t *Info, uint16_t BufferLength, uint8_t *Buffer)
{
    Mnl_Host_qapi_ZB_CL_Identify_Server_Event_CB_t_Handler(CallbackID, Info, BufferLength, Buffer);
}
