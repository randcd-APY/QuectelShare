/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_zb_common.h"
#include "qapi_zb_aps_common.h"
#include "qapi_zb_cl_common.h"

qapi_Status_t Mnl_qapi_ZB_CL_Create_Cluster(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, uint16_t ClusterId, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_Frame_Direction_t Direction, qapi_ZB_CL_Custom_Cluster_Event_CB_t Event_CB, uint32_t CB_Param);
