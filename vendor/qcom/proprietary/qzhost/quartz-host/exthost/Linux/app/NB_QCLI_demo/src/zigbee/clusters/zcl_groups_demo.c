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

#include "qapi_zb.h"
#include "qapi_zb_cl.h"
#include "qapi_zb_cl_groups.h"

static QCLI_Group_Handle_t ZCL_Groups_QCLI_Handle;

/* Function prototypes. */
static QCLI_Command_Status_t cmd_ZCL_Groups_AddGroup(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_Groups_ViewGroup(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_Groups_GetGroupMembership(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_Groups_RemoveGroup(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_Groups_RemoveAllGroups(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static void ZCL_Groups_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Groups_Server_Event_Data_t *EventData, uint32_t CB_Param);
static void ZCL_Groups_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Groups_Client_Event_Data_t *EventData, uint32_t CB_Param);

/* Command list for the ZigBee light demo. */
const QCLI_Command_t ZigBee_Groups_CMD_List[] =
{
   /* cmd_function                     thread  cmd_string             usage_string                                          description */
   {cmd_ZCL_Groups_AddGroup,           false,  "AddGroup",           "[DevId][ClientEndpoint][GroupID][Name][Identifying]", "Sends an Add Groups command to a Groups server."},
   {cmd_ZCL_Groups_ViewGroup,          false,  "ViewGroup",          "[DevId][ClientEndpoint][GroupID]",                    "Sends a View Groups command to a Groups server."},
   {cmd_ZCL_Groups_GetGroupMembership, false,  "GetGroupMembership", "[DevId][ClientEndpoint][GroupID (Optional)]",         "Sends a Get Group Membership command to a Groups server."},
   {cmd_ZCL_Groups_RemoveGroup,        false,  "RemoveGroup",        "[DevId][ClientEndpoint][GroupID]",                    "Sends a Remove Group command to a Groups server."},
   {cmd_ZCL_Groups_RemoveAllGroups,    false,  "RemoveAllGroups",    "[DevId][ClientEndpoint]",                             "Sends a Remove All Groups command to a Groups server."}
};

const QCLI_Command_Group_t ZCL_Groups_Cmd_Group = {"Groups", sizeof(ZigBee_Groups_CMD_List) / sizeof(QCLI_Command_t), ZigBee_Groups_CMD_List};

/**
   @brief Executes the "AddGroup" command to add a group.

   Parameter_List[0] ID of the device to send the command to.
   Parameter_List[1] Endpoint of the Groups client cluster to use to send the
                     command.
   Parameter_List[2] ID of the group to add.
   Parameter_List[3] Name of the group to add.
   Parameter_List[4] Flag indicating if groups should be added only if the
                     device is identifying.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_Groups_AddGroup(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   uint32_t                        DeviceId;
   uint16_t                        GroupId;
   char                           *GroupName;
   qbool_t                         Identifying;

   /* Ensure both the stack is initialized. */
   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      if((Parameter_Count >= 5) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[4]), 0, 1)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_GROUPS, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         GroupId     = Parameter_List[2].Integer_Value;
         GroupName   = Parameter_List[3].String_Value;
         Identifying = (qbool_t)(Parameter_List[4].Integer_Value != 0);

         if(ClusterInfo != NULL)
         {
            /* Format the destination info. */
            memset(&SendInfo, 0, sizeof(qapi_ZB_CL_General_Send_Info_t));
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_Groups_Send_Add_Group(ClusterInfo->Handle, &SendInfo, GroupId, GroupName, Identifying);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZCL_Groups_QCLI_Handle, "qapi_ZB_CL_Groups_Send_Add_Group");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZCL_Groups_QCLI_Handle, "qapi_ZB_CL_Groups_Send_Add_Group", Result);
               }
            }
            else
            {
               QCLI_Printf(ZCL_Groups_QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZCL_Groups_QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZCL_Groups_QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "ViewGroup" command to view a group.

   Parameter_List[0] ID of the device to send the command to.
   Parameter_List[1] Endpoint of the Groups client cluster to use to send the
                     command.
   Parameter_List[2] ID of the group to view.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_Groups_ViewGroup(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
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
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_GROUPS, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         GroupId     = Parameter_List[2].Integer_Value;

         if(ClusterInfo != NULL)
         {
            /* Format the destination info. */
            memset(&SendInfo, 0, sizeof(qapi_ZB_CL_General_Send_Info_t));
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_Groups_Send_View_Group(ClusterInfo->Handle, &SendInfo, GroupId);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZCL_Groups_QCLI_Handle, "qapi_ZB_CL_Groups_Send_View_Group");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZCL_Groups_QCLI_Handle, "qapi_ZB_CL_Groups_Send_View_Group", Result);
               }
            }
            else
            {
               QCLI_Printf(ZCL_Groups_QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZCL_Groups_QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZCL_Groups_QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "GetGroupMembership" command to get a groups membership.

   Parameter_List[0] ID of the device to send the command to.
   Parameter_List[1] Endpoint of the Groups client cluster to use to send the
                     command.
   Parameter_List[2] ID of the group to request membership of Leave blank to
                     query all groups.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_Groups_GetGroupMembership(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   uint32_t                        DeviceId;
   uint8_t                         GroupCount;
   uint16_t                        GroupId;

   /* Ensure both the stack is initialized. */
   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      if((Parameter_Count >= 2) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_GROUPS, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         GroupId     = 0;
         GroupCount  = 0;

         if((Parameter_Count >= 3) &&
            (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFFFF)))
         {
            GroupCount = 1;
            GroupId    = Parameter_List[2].Integer_Value;
         }

         if(ClusterInfo != NULL)
         {
            /* Format the destination info. */
            memset(&SendInfo, 0, sizeof(qapi_ZB_CL_General_Send_Info_t));
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_Groups_Send_Get_Group_Membership(ClusterInfo->Handle, &SendInfo, GroupCount, (GroupCount != 0) ? &GroupId : NULL);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZCL_Groups_QCLI_Handle, "qapi_ZB_CL_Groups_Send_Get_Group_Membership");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZCL_Groups_QCLI_Handle, "qapi_ZB_CL_Groups_Send_Get_Group_Membership", Result);
               }
            }
            else
            {
               QCLI_Printf(ZCL_Groups_QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZCL_Groups_QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZCL_Groups_QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "RemoveGroup" command to remove a group.

   Parameter_List[0] ID of the device to send the command to.
   Parameter_List[1] Endpoint of the Groups client cluster to use to send the
                     command.
   Parameter_List[2] ID of the group to remove.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_Groups_RemoveGroup(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
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
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_GROUPS, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         GroupId     = Parameter_List[2].Integer_Value;

         if(ClusterInfo != NULL)
         {
            /* Format the destination info. */
            memset(&SendInfo, 0, sizeof(qapi_ZB_CL_General_Send_Info_t));
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_Groups_Send_Remove_Group(ClusterInfo->Handle, &SendInfo, GroupId);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZCL_Groups_QCLI_Handle, "qapi_ZB_CL_Groups_Send_Remove_Group");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZCL_Groups_QCLI_Handle, "qapi_ZB_CL_Groups_Send_Remove_Group", Result);
               }
            }
            else
            {
               QCLI_Printf(ZCL_Groups_QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZCL_Groups_QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZCL_Groups_QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "RemoveAllGroups" command to remove all groups.

   Parameter_List[0] ID of the device to send the command to.
   Parameter_List[1] Endpoint of the Groups client cluster to use to send the
                     command.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_Groups_RemoveAllGroups(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   uint32_t                        DeviceId;

   /* Ensure both the stack is initialized. */
   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      if((Parameter_Count >= 2) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_GROUPS, ZCL_DEMO_CLUSTERTYPE_CLIENT);

         if(ClusterInfo != NULL)
         {
            /* Format the destination info. */
            memset(&SendInfo, 0, sizeof(qapi_ZB_CL_General_Send_Info_t));
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_Groups_Send_Remove_All_Groups(ClusterInfo->Handle, &SendInfo);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZCL_Groups_QCLI_Handle, "qapi_ZB_CL_Groups_Send_Remove_All_Groups");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZCL_Groups_QCLI_Handle, "qapi_ZB_CL_Groups_Send_Remove_All_Groups", Result);
               }
            }
            else
            {
               QCLI_Printf(ZCL_Groups_QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZCL_Groups_QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZCL_Groups_QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Handles callbacks for the Groups server cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_Groups_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Groups_Server_Event_Data_t *EventData, uint32_t CB_Param)
{
   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      QCLI_Printf(ZCL_Groups_QCLI_Handle, "Unhandled Groups server event %d.\n", EventData->Event_Type);
      QCLI_Display_Prompt();
   }
}

/**
   @brief Handles callbacks for the Groups client cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_Groups_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Groups_Client_Event_Data_t *EventData, uint32_t CB_Param)
{
   uint8_t Index;

   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      switch(EventData->Event_Type)
      {
         case QAPI_ZB_CL_GROUPS_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E:
            QCLI_Printf(ZCL_Groups_QCLI_Handle, "Default Response:\n");
            QCLI_Printf(ZCL_Groups_QCLI_Handle, "  Status:        %d\n", EventData->Data.Default_Response.Status);
            QCLI_Printf(ZCL_Groups_QCLI_Handle, "  CommandId:     %d\n", EventData->Data.Default_Response.CommandId);
            QCLI_Printf(ZCL_Groups_QCLI_Handle, "  CommandStatus: %d\n", EventData->Data.Default_Response.CommandStatus);
            break;

         case QAPI_ZB_CL_GROUPS_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E:
            QCLI_Printf(ZCL_Groups_QCLI_Handle, "Command Complete:\n");
            QCLI_Printf(ZCL_Groups_QCLI_Handle, "  CommandStatus: %d\n", EventData->Data.Command_Complete.CommandStatus);
            break;

         case QAPI_ZB_CL_GROUPS_CLIENT_EVENT_TYPE_ADD_GROUP_RESPONSE_E:
            QCLI_Printf(ZCL_Groups_QCLI_Handle, "Add Group Response:\n");
            QCLI_Printf(ZCL_Groups_QCLI_Handle, "  Status:  %d\n", EventData->Data.Add_Group_Response.Status);
            QCLI_Printf(ZCL_Groups_QCLI_Handle, "  GroupId: %d\n", EventData->Data.Add_Group_Response.GroupId);
            break;

         case QAPI_ZB_CL_GROUPS_CLIENT_EVENT_TYPE_VIEW_GROUP_RESPONSE_E:
            QCLI_Printf(ZCL_Groups_QCLI_Handle, "View Group Response:\n");
            QCLI_Printf(ZCL_Groups_QCLI_Handle, "  Status:    %d\n", EventData->Data.View_Group_Response.Status);
            QCLI_Printf(ZCL_Groups_QCLI_Handle, "  GroupId:   %d\n", EventData->Data.View_Group_Response.GroupId);
            QCLI_Printf(ZCL_Groups_QCLI_Handle, "  GroupName: %s\n", EventData->Data.View_Group_Response.GroupName);
            break;

         case QAPI_ZB_CL_GROUPS_CLIENT_EVENT_TYPE_GET_GROUP_MEMBERSHIP_RESPONSE_E:
            QCLI_Printf(ZCL_Groups_QCLI_Handle, "Group Membership Response:\n");
            QCLI_Printf(ZCL_Groups_QCLI_Handle, "  Capacity:   %d\n", EventData->Data.Get_Group_Membership_Response.Capacity);
            QCLI_Printf(ZCL_Groups_QCLI_Handle, "  GroupCount: %d\n", EventData->Data.Get_Group_Membership_Response.GroupCount);

            if(EventData->Data.Get_Group_Membership_Response.GroupCount != 0)
            {
               QCLI_Printf(ZCL_Groups_QCLI_Handle, "  GroupList:  %d", EventData->Data.Get_Group_Membership_Response.GroupList[0]);

               for(Index = 1; Index < EventData->Data.Get_Group_Membership_Response.GroupCount; Index ++)
               {
                  QCLI_Printf(ZCL_Groups_QCLI_Handle, ", %d", EventData->Data.Get_Group_Membership_Response.GroupList[Index]);
               }

               QCLI_Printf(ZCL_Groups_QCLI_Handle, "\n");
            }
            break;

         case QAPI_ZB_CL_GROUPS_CLIENT_EVENT_TYPE_REMOVE_GROUP_RESPONSE_E:
            QCLI_Printf(ZCL_Groups_QCLI_Handle, "Remove Group Response:\n");
            QCLI_Printf(ZCL_Groups_QCLI_Handle, "  Status:  %d\n", EventData->Data.Remove_Group_Response.Status);
            QCLI_Printf(ZCL_Groups_QCLI_Handle, "  GroupId: %d\n", EventData->Data.Remove_Group_Response.GroupId);
            break;

         default:
            QCLI_Printf(ZCL_Groups_QCLI_Handle, "Unhandled Groups client event %d.\n", EventData->Event_Type);
            break;
      }
      QCLI_Display_Prompt();
   }
}

/**
   @brief Initializes the ZCL Groups demo.

   @param ZigBee_QCLI_Handle is the parent QCLI handle for the Groups demo.

   @return true if the ZigBee light demo initialized successfully, false
           otherwise.
*/
qbool_t Initialize_ZCL_Groups_Demo(QCLI_Group_Handle_t ZigBee_QCLI_Handle)
{
   qbool_t Ret_Val;

   /* Register Groups command group. */
   ZCL_Groups_QCLI_Handle = QCLI_Register_Command_Group(ZigBee_QCLI_Handle, &ZCL_Groups_Cmd_Group);
   if(ZCL_Groups_QCLI_Handle != NULL)
   {
      Ret_Val = true;
   }
   else
   {
      QCLI_Printf(ZigBee_QCLI_Handle, "Failed to register ZCL Groups command group.\n");
      Ret_Val = false;
   }

   return(Ret_Val);
}

/**
   @brief Creates an Groups server cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_Groups_Demo_Create_Server(uint8_t Endpoint, void **PrivData)
{
   qapi_ZB_Cluster_t          Ret_Val;
   qapi_Status_t              Result;
   qapi_ZB_Handle_t           ZigBee_Handle;
   qapi_ZB_CL_Cluster_Info_t  ClusterInfo;
   ZCL_Demo_Cluster_Info_t   *IdentifyClusterInfo;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      /* Try to find an identify cluster on the same endpoint to link with. */
      IdentifyClusterInfo = ZCL_FindClusterByEndpoint(Endpoint, QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, ZCL_DEMO_CLUSTERTYPE_SERVER);
      if(IdentifyClusterInfo != NULL)
      {
         memset(&ClusterInfo, 0, sizeof(qapi_ZB_CL_Cluster_Info_t));
         ClusterInfo.Endpoint = Endpoint;

         Result = qapi_ZB_CL_Groups_Create_Server(ZigBee_Handle, &Ret_Val, &ClusterInfo, IdentifyClusterInfo->Handle, ZCL_Groups_Demo_Server_CB, 0);
         if(Result != QAPI_OK)
         {
            Display_Function_Error(ZCL_Groups_QCLI_Handle, "qapi_ZB_CL_Groups_Create_Server", Result);
            Ret_Val = NULL;
         }
      }
      else
      {
         QCLI_Printf(ZCL_Groups_QCLI_Handle, "Failed to find Identify server on the endpoint.\n");
         Ret_Val = NULL;
      }
   }
   else
   {
      QCLI_Printf(ZCL_Groups_QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

/**
   @brief Creates an Groups client cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_Groups_Demo_Create_Client(uint8_t Endpoint, void **PrivData)
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

      Result = qapi_ZB_CL_Groups_Create_Client(ZigBee_Handle, &Ret_Val, &ClusterInfo, ZCL_Groups_Demo_Client_CB, 0);
      if(Result != QAPI_OK)
      {
         Display_Function_Error(ZCL_Groups_QCLI_Handle, "qapi_ZB_CL_Groups_Create_Client", Result);
         Ret_Val = NULL;
      }
   }
   else
   {
      QCLI_Printf(ZCL_Groups_QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

