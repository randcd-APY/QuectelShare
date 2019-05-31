/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_zb_cl_doorlock.h"

qapi_Status_t _qapi_ZB_CL_DoorLock_Create_Client(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_DoorLock_Client_CB_t Event_CB, uint32_t CB_Param);

qapi_Status_t _qapi_ZB_CL_DoorLock_Create_Server(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_DoorLock_Server_CB_t Event_CB, uint32_t CB_Param);

qapi_Status_t _qapi_ZB_CL_DoorLock_Populate_Attributes(uint8_t TargetID, qbool_t Server, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList);

qapi_Status_t _qapi_ZB_CL_DoorLock_Server_Set_PIN_Code(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_DoorLock_PIN_t *PIN);

qapi_Status_t _qapi_ZB_CL_DoorLock_Server_Get_PIN_Code(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_DoorLock_PIN_t *PIN);

qapi_Status_t _qapi_ZB_CL_DoorLock_Send_Lock(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, qapi_ZB_CL_DoorLock_PIN_t *PIN);

qapi_Status_t _qapi_ZB_CL_DoorLock_Send_Unlock(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, qapi_ZB_CL_DoorLock_PIN_t *PIN);

qapi_Status_t _qapi_ZB_CL_DoorLock_Send_Toggle(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, qapi_ZB_CL_DoorLock_PIN_t *PIN);
