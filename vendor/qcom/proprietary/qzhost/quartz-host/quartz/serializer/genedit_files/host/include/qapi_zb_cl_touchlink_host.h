/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_zb_cl_touchlink.h"

qapi_Status_t _qapi_ZB_CL_Touchlink_Create_Client(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, uint8_t Endpoint, qapi_ZB_CL_Touchlink_Client_CB_t Event_CB, uint32_t CB_Param);

qapi_Status_t _qapi_ZB_CL_Touchlink_Create_Server(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, uint8_t Endpoint, qapi_ZB_Cluster_t IdentifyCluster, qapi_ZB_CL_Touchlink_Server_CB_t Event_CB, uint32_t CB_Param);

qapi_Status_t _qapi_ZB_CL_Touchlink_Start(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Touchlink_Device_Type_t DeviceType, const uint8_t *PersistData, uint32_t PersistLength);

qapi_Status_t _qapi_ZB_CL_Touchlink_Scan_Request(uint8_t TargetID, qapi_ZB_Cluster_t ClientCluster);

qapi_Status_t _qapi_ZB_CL_Touchlink_Factory_Reset(uint8_t TargetID, qapi_ZB_Cluster_t ClientCluster);
