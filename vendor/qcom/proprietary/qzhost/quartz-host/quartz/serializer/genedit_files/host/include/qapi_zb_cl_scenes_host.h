/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "qapi_zb_cl_scenes.h"

qapi_Status_t _qapi_ZB_CL_Scenes_Create_Client(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_Scenes_Client_Event_CB_t Event_CB, uint32_t CB_Param);

qapi_Status_t _qapi_ZB_CL_Scenes_Create_Server(uint8_t TargetID, qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, uint8_t MaxScenes, qapi_ZB_CL_Scenes_Server_Event_CB_t Event_CB, uint32_t CB_Param);

qapi_Status_t _qapi_ZB_CL_Scenes_Populate_Attributes(uint8_t TargetID, qbool_t Server, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList);

qapi_Status_t _qapi_ZB_CL_Scenes_Send_Add_Scene(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, const qapi_ZB_CL_Scenes_Add_Scene_t *AddScene);

qapi_Status_t _qapi_ZB_CL_Scenes_Send_View_Scene(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint16_t GroupId, uint8_t SceneId, qbool_t IsEnhanced);

qapi_Status_t _qapi_ZB_CL_Scenes_Send_Remove_Scene(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint16_t GroupId, uint8_t SceneId);

qapi_Status_t _qapi_ZB_CL_Scenes_Send_Remove_All_Scenes(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint16_t GroupId);

qapi_Status_t _qapi_ZB_CL_Scenes_Send_Store_Scene(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint16_t GroupId, uint8_t SceneId);

qapi_Status_t _qapi_ZB_CL_Scenes_Send_Recall_Scene(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint16_t GroupId, uint8_t SceneId);

qapi_Status_t _qapi_ZB_CL_Scenes_Send_Get_Scene_Membership(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint16_t GroupId);

qapi_Status_t _qapi_ZB_CL_Scenes_Send_Copy_Scene(uint8_t TargetID, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, const qapi_ZB_CL_Scenes_Copy_Scene_t *CopyScene);
