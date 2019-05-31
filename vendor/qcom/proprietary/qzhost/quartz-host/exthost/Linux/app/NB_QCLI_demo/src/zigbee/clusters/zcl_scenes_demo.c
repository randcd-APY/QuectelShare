/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "malloc.h"
#include "string.h"
#include "qcli_api.h"
#include "qcli_util.h"
#include "zigbee_demo.h"
#include "zcl_demo.h"
#include "zcl_scenes_demo.h"
#include "zcl_onoff_demo.h"
#include "zcl_levelcontrol_demo.h"
#include "zcl_colorcontrol_demo.h"

#include "qapi_zb.h"
#include "qapi_zb_cl.h"
#include "qapi_zb_cl_scenes.h"

#define ZCL_SCENES_DEMO_MAX_SCENES                                      (4)

static QCLI_Group_Handle_t ZCL_Scenes_QCLI_Handle;

/* Structure to describe a cluster that contains scenes data. */
typedef struct ZCL_Scenes_Cluster_Descriptor_s
{
   uint16_t                  ClusterID;      /* ID of the cluster. */
   ZCL_Scenes_GetData_Func_t GetExtDataFunc; /* Function called to get the scenes data for the cluster. */
} ZCL_Scenes_Cluster_Descriptor_t;

/* The list of clusters that are scene capable. */
static const ZCL_Scenes_Cluster_Descriptor_t SceneClusterDescriptorList[] =
{
   {QAPI_ZB_CL_CLUSTER_ID_ONOFF,         ZCL_OnOff_GetScenesData},
   {QAPI_ZB_CL_CLUSTER_ID_LEVEL_CONTROL, ZCL_LevelControl_GetScenesData},
   {QAPI_ZB_CL_CLUSTER_ID_COLOR_CONTROL, ZCL_ColorControl_GetScenesData}
};

#define SCENES_CLUSTER_DESCRIPTOR_LIST_SIZE     (sizeof(SceneClusterDescriptorList) / sizeof(ZCL_Scenes_Cluster_Descriptor_t))

/* Function prototypes. */
static QCLI_Command_Status_t ZCL_Scene_PopulateSceneData(uint8_t Endpoint, uint8_t *ExtFieldCount, qapi_ZB_CL_Scenes_Extension_Field_Set_t *ExtFieldSet);

static QCLI_Command_Status_t cmd_ZCL_Scenes_AddScene(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_Scenes_ViewScene(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_Scenes_RemoveScene(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_Scenes_RemoveAllScenes(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_Scenes_StoreScenes(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_Scenes_RecallScenes(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_Scenes_GetSceneMembership(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_Scenes_CopyScene(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static void ZCL_Scenes_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Scenes_Server_Event_Data_t *EventData, uint32_t CB_Param);
static void ZCL_Scenes_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Scenes_Client_Event_Data_t *EventData, uint32_t CB_Param);

/* Command list for the ZigBee light demo. */
static const QCLI_Command_t ZigBee_Scenes_CMD_List[] =
{
   /* cmd_function                     thread  cmd_string            usage_string                                                                    description */
   {cmd_ZCL_Scenes_AddScene,           false,  "AddScene",           "[DevId][ClientEndpoint][GroupID][SceneID][SceneName][TransTime][IsEnhanced]",  "Sends an Add Scene command to a Scenes server."},
   {cmd_ZCL_Scenes_ViewScene,          false,  "ViewScene",          "[DevId][ClientEndpoint][GroupID][SceneID][IsEnhanced]",                        "Sends a View Scene command to a Scenes server."},
   {cmd_ZCL_Scenes_RemoveScene,        false,  "RemoveScene",        "[DevId][ClientEndpoint][GroupID][SceneID]",                                    "Sends a Remove Scene command to a Scenes server."},
   {cmd_ZCL_Scenes_RemoveAllScenes,    false,  "RemoveAllScenes",    "[DevId][ClientEndpoint][GroupID]",                                             "Sends a Remove All Scenes command to a Scenes server."},
   {cmd_ZCL_Scenes_StoreScenes,        false,  "StoreScenes",        "[DevId][ClientEndpoint][GroupID][SceneID]",                                    "Sends a Store Scene command to a Scenes server."},
   {cmd_ZCL_Scenes_RecallScenes,       false,  "RecallScenes",       "[DevId][ClientEndpoint][GroupID][SceneID]",                                    "Sends a Recall Scene command to a Scenes server."},
   {cmd_ZCL_Scenes_GetSceneMembership, false,  "GetSceneMembership", "[DevId][ClientEndpoint][GroupID]",                                             "Sends a Get Scene Membership command to a Scenes server."},
   {cmd_ZCL_Scenes_CopyScene,          false,  "CopyScene",          "[DevId][ClientEndpoint][CopyAll][GroupFrom][SceneFrom][GroupTo][SceneTo]",     "Sends a Copy Scene command to a Scenes server."}
};

const QCLI_Command_Group_t ZCL_Scenes_Cmd_Group = {"Scenes", sizeof(ZigBee_Scenes_CMD_List) / sizeof(QCLI_Command_t), ZigBee_Scenes_CMD_List};

/**
   @brief Populate the extention field set.

   @param[in]  Endpoint      is the end point possibely holds the scene capable
                             clusters
   @param[out] ExtFieldCount is the number of the extension field.
   @paran[in]  ExtFieldSet   is the extension field set.

   @return
     - QAPI_OK    if the function executed successfully.
     - QAPI_ERROR if there was an error.
*/
static QCLI_Command_Status_t ZCL_Scene_PopulateSceneData(uint8_t Endpoint, uint8_t *ExtFieldCount, qapi_ZB_CL_Scenes_Extension_Field_Set_t *ExtFieldSet)
{
   QCLI_Command_Status_t    Ret_Val;
   ZCL_Demo_Cluster_Info_t *ClusterInfo;
   uint8_t                  Index;
   uint8_t                  LocalFieldCount;

   Ret_Val         = QCLI_STATUS_SUCCESS_E;
   LocalFieldCount = 0;

   /* Go throught the SceneClusterDescriptorList to check if there is a scene
      capable cluster on the given endpoint. */
   for(Index = 0; (Index < SCENES_CLUSTER_DESCRIPTOR_LIST_SIZE) && (Ret_Val == QCLI_STATUS_SUCCESS_E); Index ++)
   {
      ClusterInfo = ZCL_FindClusterByEndpoint(Endpoint, SceneClusterDescriptorList[Index].ClusterID, ZCL_DEMO_CLUSTERTYPE_CLIENT);
      if(ClusterInfo != NULL)
      {
         if(LocalFieldCount < *ExtFieldCount)
         {
            /* The scene capable cluster is found on the given endpoint. Updtae
               the value ExtFieldCount and populate the ExtFieldSet.*/
            if((*(SceneClusterDescriptorList[Index].GetExtDataFunc))(ExtFieldSet))
            {
               /* Adjust LocalExtFieldSet to point to the next entry. */
               LocalFieldCount ++;
               ExtFieldSet ++;
            }
            else
            {
               QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Failed to get scenes data for cluster %d.\n", SceneClusterDescriptorList[Index].ClusterID);
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "ExtField overflowed.\n");
            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
   }

   if(Ret_Val == QCLI_STATUS_SUCCESS_E)
   {
      *ExtFieldCount = LocalFieldCount;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "AddScene" command add a scene.

   Parameter_List[0] ID of the device to send the command to.
   Parameter_List[1] is the endpoint the scenes client cluster is on.
   Parameter_List[2] is the group ID.
   Parameter_List[3] is the scene ID.
   Parameter_List[4] is scene name.
   Parameter_List[5] is the trans time.
   Parameter_List[6] is the flag indicates if the enhanced add scene will be
                     used.
                     0=No, 1=Yes.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_Scenes_AddScene(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t                    Ret_Val;
   qapi_Status_t                            Result;
   qapi_ZB_CL_General_Send_Info_t           SendInfo;
   ZCL_Demo_Cluster_Info_t                 *SceneCluster;
   uint8_t                                  DeviceId;
   uint8_t                                  Endpoint;
   qapi_ZB_CL_Scenes_Extension_Field_Set_t  ExtFieldSet[SCENES_CLUSTER_DESCRIPTOR_LIST_SIZE];
   qapi_ZB_CL_Scenes_Add_Scene_t            Request;

   /* Ensure both the stack is initialized. */
   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      if((Parameter_Count >= 7) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[5]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[6]), 0, 1)))
      {
         DeviceId = (uint8_t)(Parameter_List[0].Integer_Value);
         Endpoint = (uint8_t)(Parameter_List[1].Integer_Value);

         /* Determine if the scene client cluster is on the given endpoint. */
         SceneCluster = ZCL_FindClusterByEndpoint(Endpoint, QAPI_ZB_CL_CLUSTER_ID_SCENES, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         if(SceneCluster != NULL)
         {
            /* Set the Request to a known state. */
            memset(&Request, 0, sizeof(qapi_ZB_CL_Scenes_Add_Scene_t));

            /* Set up ExtensionFieldCount and populate the ExtFieldSet. */
            Request.ExtensionFieldCount = SCENES_CLUSTER_DESCRIPTOR_LIST_SIZE;
            Ret_Val = ZCL_Scene_PopulateSceneData(Endpoint, &(Request.ExtensionFieldCount), ExtFieldSet);
            if(Ret_Val == QCLI_STATUS_SUCCESS_E)
            {
               /* Setup the request. */
               Request.GroupId            = (uint16_t)(Parameter_List[2].Integer_Value);
               Request.SceneId            = (uint8_t)(Parameter_List[3].Integer_Value);
               Request.SceneName          = (uint8_t *)(Parameter_List[4].String_Value);
               Request.TransitionTime     = (uint16_t)(Parameter_List[5].Integer_Value);
               Request.IsEnhanced         = (qbool_t)(Parameter_List[6].Integer_Value != 0);
               Request.ExtensionFieldSets = ExtFieldSet;

               /* Format the destination info. */
               memset(&SendInfo, 0, sizeof(qapi_ZB_CL_General_Send_Info_t));
               if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
               {
                  Result = qapi_ZB_CL_Scenes_Send_Add_Scene(SceneCluster->Handle, &SendInfo, &Request);
                  if(Result == QAPI_OK)
                  {
                     Display_Function_Success(ZCL_Scenes_QCLI_Handle, "qapi_ZB_CL_Scenes_Send_Add_Scene");
                  }
                  else
                  {
                     Ret_Val = QCLI_STATUS_ERROR_E;
                     Display_Function_Error(ZCL_Scenes_QCLI_Handle, "qapi_ZB_CL_Scenes_Send_Add_Scene", Result);
                  }
               }
               else
               {
                  QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Invalid device ID.\n");
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
         }
         else
         {
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Scene client cluster is not found on the given endpoint.\n");
            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "ViewScene" command to view a scene.

   Parameter_List[0] ID of the device to send the command to.
   Parameter_List[1] Endpoint of the Scenes client cluster to use to send the
                     command.
   Parameter_List[2] ID of the group of the scene to view.
   Parameter_List[3] ID of the scene to view.
   Parameter_List[4] Flag indicating if this is an enhanced view scenes command.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_Scenes_ViewScene(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   uint32_t                        DeviceId;
   uint16_t                        GroupId;
   uint8_t                         SceneId;
   qbool_t                         IsEnhanced;

   /* Ensure both the stack is initialized. */
   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      if((Parameter_Count >= 5) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[4]), 0, 1)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_SCENES, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         GroupId     = (uint16_t)(Parameter_List[2].Integer_Value);
         SceneId     = (uint8_t)(Parameter_List[3].Integer_Value);
         IsEnhanced  = (qbool_t)(Parameter_List[4].Integer_Value != 0);

         if(ClusterInfo != NULL)
         {
            /* Format the destination info. */
            memset(&SendInfo, 0, sizeof(qapi_ZB_CL_General_Send_Info_t));
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_Scenes_Send_View_Scene(ClusterInfo->Handle, &SendInfo, GroupId, SceneId, IsEnhanced);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZCL_Scenes_QCLI_Handle, "qapi_ZB_CL_Scenes_Send_View_Scene");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZCL_Scenes_QCLI_Handle, "qapi_ZB_CL_Scenes_Send_View_Scene", Result);
               }
            }
            else
            {
               QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Invalid Cluster Index.\n");
            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "RemoveScene" command to remove a scene.

   Parameter_List[0] ID of the device to send the command to.
   Parameter_List[1] Endpoint of the Scenes client cluster to use to send the
                     command.
   Parameter_List[2] ID of the group of the scene to remove.
   Parameter_List[3] ID of the scene to remove.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_Scenes_RemoveScene(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   uint32_t                        DeviceId;
   uint16_t                        GroupId;
   uint8_t                         SceneId;

   /* Ensure both the stack is initialized. */
   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      if((Parameter_Count >= 4) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFF)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_SCENES, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         GroupId     = (uint16_t)(Parameter_List[2].Integer_Value);
         SceneId     = (uint8_t)(Parameter_List[3].Integer_Value);

         if(ClusterInfo != NULL)
         {
            /* Format the destination info. */
            memset(&SendInfo, 0, sizeof(qapi_ZB_CL_General_Send_Info_t));
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_Scenes_Send_Remove_Scene(ClusterInfo->Handle, &SendInfo, GroupId, SceneId);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZCL_Scenes_QCLI_Handle, "qapi_ZB_CL_Scenes_Send_Remove_Scene");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZCL_Scenes_QCLI_Handle, "qapi_ZB_CL_Scenes_Send_Remove_Scene", Result);
               }
            }
            else
            {
               QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Invalid Cluster Index.\n");
            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "RemoveAllScenes" command to all scenes.

   Parameter_List[0] ID of the device to send the command to.
   Parameter_List[1] Endpoint of the Scenes client cluster to use to send the
                     command.
   Parameter_List[2] ID of the group of the scenes to remove.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_Scenes_RemoveAllScenes(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   uint32_t                        DeviceId;
   uint16_t                        GroupId;

   /* Ensure both the stack is initialized. */
   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      if((Parameter_Count >= 3) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFFFF)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_SCENES, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         GroupId     = (uint16_t)(Parameter_List[2].Integer_Value);

         if(ClusterInfo != NULL)
         {
            /* Format the destination info. */
            memset(&SendInfo, 0, sizeof(qapi_ZB_CL_General_Send_Info_t));
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_Scenes_Send_Remove_All_Scenes(ClusterInfo->Handle, &SendInfo, GroupId);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZCL_Scenes_QCLI_Handle, "qapi_ZB_CL_Scenes_Send_Remove_All_Scenes");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZCL_Scenes_QCLI_Handle, "qapi_ZB_CL_Scenes_Send_Remove_All_Scenes", Result);
               }
            }
            else
            {
               QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Invalid Cluster Index.\n");
            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "StoreScene" command to store a scene.

   Parameter_List[0] ID of the device to send the command to.
   Parameter_List[1] Endpoint of the Scenes client cluster to use to send the
                     command.
   Parameter_List[2] ID of the group of the scene to store.
   Parameter_List[3] ID of the scene to store.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_Scenes_StoreScenes(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   uint32_t                        DeviceId;
   uint16_t                        GroupId;
   uint8_t                         SceneId;

   /* Ensure both the stack is initialized. */
   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      if((Parameter_Count >= 4) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFF)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_SCENES, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         GroupId     = (uint16_t)(Parameter_List[2].Integer_Value);
         SceneId     = (uint8_t)(Parameter_List[3].Integer_Value);

         if(ClusterInfo != NULL)
         {
            /* Format the destination info. */
            memset(&SendInfo, 0, sizeof(qapi_ZB_CL_General_Send_Info_t));
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_Scenes_Send_Store_Scene(ClusterInfo->Handle, &SendInfo, GroupId, SceneId);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZCL_Scenes_QCLI_Handle, "qapi_ZB_CL_Scenes_Send_Store_Scene");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZCL_Scenes_QCLI_Handle, "qapi_ZB_CL_Scenes_Send_Store_Scene", Result);
               }
            }
            else
            {
               QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Invalid Cluster Index.\n");
            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "RecallScene" command to recall a scene.

   Parameter_List[0] ID of the device to send the command to.
   Parameter_List[1] Endpoint of the Scenes client cluster to use to send the
                     command.
   Parameter_List[2] ID of the group of the scene to recall.
   Parameter_List[3] ID of the scene to recall.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_Scenes_RecallScenes(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   uint32_t                        DeviceId;
   uint16_t                        GroupId;
   uint8_t                         SceneId;

   /* Ensure both the stack is initialized. */
   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      if((Parameter_Count >= 4) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFF)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_SCENES, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         GroupId     = (uint16_t)(Parameter_List[2].Integer_Value);
         SceneId     = (uint8_t)(Parameter_List[3].Integer_Value);

         if(ClusterInfo != NULL)
         {
            /* Format the destination info. */
            memset(&SendInfo, 0, sizeof(qapi_ZB_CL_General_Send_Info_t));
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_Scenes_Send_Recall_Scene(ClusterInfo->Handle, &SendInfo, GroupId, SceneId);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZCL_Scenes_QCLI_Handle, "qapi_ZB_CL_Scenes_Send_Recall_Scene");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZCL_Scenes_QCLI_Handle, "qapi_ZB_CL_Scenes_Send_Recall_Scene", Result);
               }
            }
            else
            {
               QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Invalid Cluster Index.\n");
            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "GetSceneMembership" command to get the membership of a
          scene.

   Parameter_List[0] ID of the device to send the command to.
   Parameter_List[1] Endpoint of the Scenes client cluster to use to send the
                     command.
   Parameter_List[2] ID of the group to get scene membership for.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_Scenes_GetSceneMembership(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   uint32_t                        DeviceId;
   uint16_t                        GroupId;

   /* Ensure both the stack is initialized. */
   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      if((Parameter_Count >= 3) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFFFF)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_SCENES, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         GroupId     = (uint16_t)(Parameter_List[2].Integer_Value);

         if(ClusterInfo != NULL)
         {
            /* Format the destination info. */
            memset(&SendInfo, 0, sizeof(qapi_ZB_CL_General_Send_Info_t));
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_Scenes_Send_Get_Scene_Membership(ClusterInfo->Handle, &SendInfo, GroupId);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZCL_Scenes_QCLI_Handle, "qapi_ZB_CL_Scenes_Send_Get_Scene_Membership");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZCL_Scenes_QCLI_Handle, "qapi_ZB_CL_Scenes_Send_Get_Scene_Membership", Result);
               }
            }
            else
            {
               QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Invalid Cluster Index.\n");
            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "CopyScene" command to copy one scene to another.

   Parameter_List[0] ID of the device to send the command to.
   Parameter_List[1] Endpoint of the Scenes client cluster to use to send the
                     command.
   Parameter_List[2] A flag to indicate if all scenes should be copied.
   Parameter_List[3] ID of the group to copy scenes from.
   Parameter_List[4] ID of the scene to copy from.
   Parameter_List[5] ID of the group to copy scenes to.
   Parameter_List[6] ID of the scene to copy to.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_Scenes_CopyScene(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   uint32_t                        DeviceId;
   qapi_ZB_CL_Scenes_Copy_Scene_t  CopyScene;

   /* Ensure both the stack is initialized. */
   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      if((Parameter_Count >= 7) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 1)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[4]), 0, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[5]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[6]), 0, 0xFF)))
      {
         DeviceId                = Parameter_List[0].Integer_Value;
         ClusterInfo             = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_SCENES, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         CopyScene.CopyAllScenes = (qbool_t)(Parameter_List[2].Integer_Value != 0);
         CopyScene.GroupIdFrom   = (uint16_t)(Parameter_List[3].Integer_Value);
         CopyScene.SceneIdFrom   = (uint8_t)(Parameter_List[4].Integer_Value);
         CopyScene.GroupIdTo     = (uint16_t)(Parameter_List[5].Integer_Value);
         CopyScene.SceneIdTo     = (uint8_t)(Parameter_List[6].Integer_Value);

         if(ClusterInfo != NULL)
         {
            /* Format the destination info. */
            memset(&SendInfo, 0, sizeof(qapi_ZB_CL_General_Send_Info_t));
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_Scenes_Send_Copy_Scene(ClusterInfo->Handle, &SendInfo, &CopyScene);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZCL_Scenes_QCLI_Handle, "qapi_ZB_CL_Scenes_Send_Copy_Scene");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZCL_Scenes_QCLI_Handle, "qapi_ZB_CL_Scenes_Send_Copy_Scene", Result);
               }
            }
            else
            {
               QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Invalid Cluster Index.\n");
            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Handles callbacks for the Scenes server cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_Scenes_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Scenes_Server_Event_Data_t *EventData, uint32_t CB_Param)
{
   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Unhandled Scenes server event %d.\n", EventData->Event_Type);
      QCLI_Display_Prompt();
   }
}

/**
   @brief Handles callbacks for the Scenes client cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_Scenes_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Scenes_Client_Event_Data_t *EventData, uint32_t CB_Param)
{
   uint8_t Index;

   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      switch(EventData->Event_Type)
      {
         case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E:
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Default Response:\n");
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  Status:        %d\n", EventData->Data.Default_Response.Status);
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  CommandId:     %d\n", EventData->Data.Default_Response.CommandId);
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  CommandStatus: %d\n", EventData->Data.Default_Response.CommandStatus);
            break;

         case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E:
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Command Complete:\n");
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  CommandStatus: %d\n", EventData->Data.Command_Complete.CommandStatus);
            break;

         case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_ADD_SCENE_RESPONSE_E:
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Add Scene Response:\n");
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  Status:     %d\n", EventData->Data.Add_Scene_Response.Status);
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  GroupId:    %d\n", EventData->Data.Add_Scene_Response.GroupId);
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  SceneId:    %d\n", EventData->Data.Add_Scene_Response.SceneId);
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  IsEnhanced: %s\n", EventData->Data.Add_Scene_Response.IsEnhanced ? "Yes" : "No");
            break;

         case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_VIEW_SCENE_RESPONSE_E:
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "View Scene Response:\n");
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  Status:          %d\n", EventData->Data.View_Scene_Response.Status);
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  GroupId:         %d\n", EventData->Data.View_Scene_Response.GroupId);
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  SceneId:         %d\n", EventData->Data.View_Scene_Response.SceneId);
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  IsEnhanced:      %s\n", EventData->Data.View_Scene_Response.IsEnhanced ? "Yes" : "No");
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  TransitionTime:  %d\n", EventData->Data.View_Scene_Response.TransitionTime);
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  SceneName:       %s\n", EventData->Data.View_Scene_Response.SceneName);
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  ExtFieldCount:   %d\n", EventData->Data.View_Scene_Response.ExtensionFieldCount);

            if(EventData->Data.View_Scene_Response.ExtensionFieldCount != 0)
            {
               QCLI_Printf(ZCL_Scenes_QCLI_Handle, "    ExtFieldSet:   0x%04X", EventData->Data.View_Scene_Response.ExtensionFieldSets[0].ClusterId);

               for(Index = 1; Index < EventData->Data.View_Scene_Response.ExtensionFieldCount; Index ++)
               {
                  QCLI_Printf(ZCL_Scenes_QCLI_Handle, ", 0x%04X", EventData->Data.View_Scene_Response.ExtensionFieldSets[Index].ClusterId);
               }

               QCLI_Printf(ZCL_Scenes_QCLI_Handle, "\n");
            }

            break;

         case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_REMOVE_SCENE_RESPONSE_E:
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Remove Scene Response:\n");
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  Status:  %d\n", EventData->Data.Remove_Scene_Response.Status);
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  GroupId: %d\n", EventData->Data.Remove_Scene_Response.GroupId);
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  SceneId: %d\n", EventData->Data.Remove_Scene_Response.SceneId);
            break;

         case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_REMOVE_ALL_SCENES_RESPONSE_E:
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Remove All Scenes Response:\n");
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  Status:  %d\n", EventData->Data.Remove_All_Scenes_Response.Status);
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  GroupId: %d\n", EventData->Data.Remove_All_Scenes_Response.GroupId);
            break;

         case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_STORE_SCENE_RESPONSE_E:
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Store Scene Response:\n");
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  Status:  %d\n", EventData->Data.Store_Scene_Response.Status);
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  GroupId: %d\n", EventData->Data.Store_Scene_Response.GroupId);
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  SceneId: %d\n", EventData->Data.Store_Scene_Response.SceneId);
            break;

         case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_GET_SCENE_MEMBERSHIP_RESPONSE_E:
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Get Scene Membership Response:\n");
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  Status:     %d\n", EventData->Data.Get_Scene_Membership_Response.Status);
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  Capacity:   %d\n", EventData->Data.Get_Scene_Membership_Response.Capacity);
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  GroupID:    %d\n", EventData->Data.Get_Scene_Membership_Response.GroupId);
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  SceneCount: %d\n", EventData->Data.Get_Scene_Membership_Response.SceneCount);

            if(EventData->Data.Get_Scene_Membership_Response.SceneCount != 0)
            {
               QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  SceneList:  %d\n", EventData->Data.Get_Scene_Membership_Response.SceneList[0]);
               for(Index = 1; Index < EventData->Data.Get_Scene_Membership_Response.SceneCount; Index ++)
               {
                  QCLI_Printf(ZCL_Scenes_QCLI_Handle, ", %d", EventData->Data.Get_Scene_Membership_Response.SceneList[Index]);
               }
            }
            break;

         case QAPI_ZB_CL_SCENES_CLIENT_EVENT_TYPE_COPY_SCENE_RESPONSE_E:
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Copy Scene Response:\n");
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  Status:      %d\n", EventData->Data.Copy_Scene_Response.Status);
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  GroupIdFrom: %d\n", EventData->Data.Copy_Scene_Response.GroupIdFrom);
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "  SceneIdFrom: %d\n", EventData->Data.Copy_Scene_Response.SceneIdFrom);
            break;

         default:
            QCLI_Printf(ZCL_Scenes_QCLI_Handle, "Unhandled Scenes client event %d.\n", EventData->Event_Type);
            break;
      }

      QCLI_Display_Prompt();
   }
}

/**
   @brief Initializes the ZCL Scenes demo.

   @param ZigBee_QCLI_Handle is the parent QCLI handle for the Scenes demo.

   @return true if the ZigBee light demo initialized successfully, false
           otherwise.
*/
qbool_t Initialize_ZCL_Scenes_Demo(QCLI_Group_Handle_t ZigBee_QCLI_Handle)
{
   qbool_t Ret_Val;

   /* Register Scenes command group. */
   ZCL_Scenes_QCLI_Handle = QCLI_Register_Command_Group(ZigBee_QCLI_Handle, &ZCL_Scenes_Cmd_Group);
   if(ZCL_Scenes_QCLI_Handle != NULL)
   {
      Ret_Val = true;
   }
   else
   {
      QCLI_Printf(ZigBee_QCLI_Handle, "Failed to register ZCL Scenes command group.\n");
      Ret_Val = false;
   }

   return(Ret_Val);
}

/**
   @brief Creates an Scenes server cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_Scenes_Demo_Create_Server(uint8_t Endpoint, void **PrivData)
{
   qapi_ZB_Cluster_t         Ret_Val;
   qapi_Status_t             Result;
   qapi_ZB_Handle_t          ZigBee_Handle;
   qapi_ZB_CL_Cluster_Info_t ClusterInfo;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      memset(&ClusterInfo, 0, sizeof(qapi_ZB_CL_Cluster_Info_t));
      ClusterInfo.Endpoint = Endpoint;

      Result = qapi_ZB_CL_Scenes_Create_Server(ZigBee_Handle, &Ret_Val, &ClusterInfo, ZCL_SCENES_DEMO_MAX_SCENES, ZCL_Scenes_Demo_Server_CB, 0);
      if(Result != QAPI_OK)
      {
         Display_Function_Error(ZCL_Scenes_QCLI_Handle, "qapi_ZB_CL_Scenes_Create_Server", Result);
         Ret_Val = NULL;
      }
   }
   else
   {
      QCLI_Printf(ZCL_Scenes_QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

/**
   @brief Creates an Scenes client cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_Scenes_Demo_Create_Client(uint8_t Endpoint, void **PrivData)
{
   qapi_ZB_Cluster_t         Ret_Val;
   qapi_Status_t             Result;
   qapi_ZB_Handle_t          ZigBee_Handle;
   qapi_ZB_CL_Cluster_Info_t ClusterInfo;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      memset(&ClusterInfo, 0, sizeof(qapi_ZB_CL_Cluster_Info_t));
      ClusterInfo.Endpoint = Endpoint;

      Result = qapi_ZB_CL_Scenes_Create_Client(ZigBee_Handle, &Ret_Val, &ClusterInfo, ZCL_Scenes_Demo_Client_CB, 0);
      if(Result != QAPI_OK)
      {
         Display_Function_Error(ZCL_Scenes_QCLI_Handle, "qapi_ZB_CL_Scenes_Create_Client", Result);
         Ret_Val = NULL;
      }
   }
   else
   {
      QCLI_Printf(ZCL_Scenes_QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

