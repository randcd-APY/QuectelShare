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
#include "qapi_zb_cl_alarm.h"

#define ALARM_LOG_SIZE                                                  (4)

static QCLI_Group_Handle_t ZCL_Alarms_QCLI_Handle;

/* Function prototypes. */
static QCLI_Command_Status_t cmd_ZCL_Alarms_ResetAlarm(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_Alarms_ResetAllAlarms(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_Alarms_GetAlarm(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_Alarms_ResetAlarmLog(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_Alarms_Alarm(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static void ZCL_Alarms_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Alarm_Server_Event_Data_t *EventData, uint32_t CB_Param);
static void ZCL_Alarms_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Alarm_Client_Event_Data_t *EventData, uint32_t CB_Param);

/* Command list for the ZigBee light demo. */
const QCLI_Command_t ZigBee_Alarms_CMD_List[] =
{
   /* cmd_function                 thread  cmd_string        usage_string                                           description */
   {cmd_ZCL_Alarms_ResetAlarm,     false,  "ResetAlarm",     "[DevId][ClientEndpoint][SourceClusterId][AlarmCode]", "Sends a ResetAlarm command to an Alarms server."},
   {cmd_ZCL_Alarms_ResetAllAlarms, false,  "ResetAllAlarms", "[DevId][ClientEndpoint]",                             "Sends a ResetAllAlarms command to an Alarms server."},
   {cmd_ZCL_Alarms_GetAlarm,       false,  "GetAlarm",       "[DevId][ClientEndpoint]",                             "Sends a GetAlarm command to an Alarms server."},
   {cmd_ZCL_Alarms_ResetAlarmLog,  false,  "ResetAlarmLog",  "[DevId][ClientEndpoint]",                             "Sends a ResetAlarmLog command to an Alarms server."},
   {cmd_ZCL_Alarms_Alarm,          false,  "Alarm",          "[ServerEndpoint][SourceCluster][AlarmCode]",          "Sends an Alarm command to an Alarms client."},
};

const QCLI_Command_Group_t ZCL_Alarms_Cmd_Group = {"Alarms", sizeof(ZigBee_Alarms_CMD_List) / sizeof(QCLI_Command_t), ZigBee_Alarms_CMD_List};

/**
   @brief Executes the "ResetAlarm" command to send a ResetAlarm on command to a
          device.

   Parameter_List[0] Index of the device to send to. Use index zero to use the
                     binding table (if setup).
   Parameter_List[1] Endpoint of the Alarms client cluster to use to send the
                     command.
   Parameter_List[2] ID of the cluster which generated the alarm.
   Parameter_List[3] The Alarm code to reset

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_Alarms_ResetAlarm(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t                   Result;
   QCLI_Command_Status_t           Ret_Val;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   uint8_t                         DeviceId;
   uint16_t                        SourceClusterId;
   uint8_t                         AlarmCode;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      /* Validate the device ID. */
      if((Parameter_Count >= 4) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFF)))
      {
         DeviceId        = (uint8_t)(Parameter_List[0].Integer_Value);
         ClusterInfo     = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_ALARMS, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         SourceClusterId = (uint16_t)(Parameter_List[2].Integer_Value);
         AlarmCode       = (uint8_t)(Parameter_List[3].Integer_Value);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_Alarm_Send_Reset_Alarm(ClusterInfo->Handle, &SendInfo, SourceClusterId, AlarmCode);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZCL_Alarms_QCLI_Handle, "qapi_ZB_CL_Alarm_Send_Reset_Alarm");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZCL_Alarms_QCLI_Handle, "qapi_ZB_CL_Alarm_Send_Reset_Alarm", Result);
               }
            }
            else
            {
               QCLI_Printf(ZCL_Alarms_QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZCL_Alarms_QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZCL_Alarms_QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "ResetAllAlarms" command to send a ResetAllAlarms on
          command to a device.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the Alarms client cluster to use to send the
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
static QCLI_Command_Status_t cmd_ZCL_Alarms_ResetAllAlarms(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t                   Result;
   QCLI_Command_Status_t           Ret_Val;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   uint8_t                         DeviceId;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      /* Validate the device ID. */
      if((Parameter_Count >= 2) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         DeviceId    = (uint8_t)(Parameter_List[0].Integer_Value);
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_ALARMS, ZCL_DEMO_CLUSTERTYPE_CLIENT);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_Alarm_Send_Reset_All_Alarms(ClusterInfo->Handle, &SendInfo);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZCL_Alarms_QCLI_Handle, "qapi_ZB_CL_Alarm_Send_Reset_All_Alarms");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZCL_Alarms_QCLI_Handle, "qapi_ZB_CL_Alarm_Send_Reset_All_Alarms", Result);
               }
            }
            else
            {
               QCLI_Printf(ZCL_Alarms_QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZCL_Alarms_QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZCL_Alarms_QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "GetAlarm" command to send a GetAlarm on command to a
          device.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the Alarms client cluster to use to send the
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
static QCLI_Command_Status_t cmd_ZCL_Alarms_GetAlarm(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t                   Result;
   QCLI_Command_Status_t           Ret_Val;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   uint8_t                         DeviceId;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      /* Validate the device ID. */
      if((Parameter_Count >= 2) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         DeviceId    = (uint8_t)(Parameter_List[0].Integer_Value);
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_ALARMS, ZCL_DEMO_CLUSTERTYPE_CLIENT);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_Alarm_Send_Get_Alarm(ClusterInfo->Handle, &SendInfo);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZCL_Alarms_QCLI_Handle, "qapi_ZB_CL_Alarm_Send_Get_Alarm");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZCL_Alarms_QCLI_Handle, "qapi_ZB_CL_Alarm_Send_Get_Alarm", Result);
               }
            }
            else
            {
               QCLI_Printf(ZCL_Alarms_QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZCL_Alarms_QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZCL_Alarms_QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "ResetAlarmLog" command to send a ResetAlarmLog on
          command to a device.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the Alarms client cluster to use to send the
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
static QCLI_Command_Status_t cmd_ZCL_Alarms_ResetAlarmLog(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t                   Result;
   QCLI_Command_Status_t           Ret_Val;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   uint8_t                         DeviceId;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      /* Validate the device ID. */
      if((Parameter_Count >= 2) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         DeviceId    = (uint8_t)(Parameter_List[0].Integer_Value);
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_ALARMS, ZCL_DEMO_CLUSTERTYPE_CLIENT);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_Alarm_Send_Reset_Alarm_Log(ClusterInfo->Handle, &SendInfo);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZCL_Alarms_QCLI_Handle, "qapi_ZB_CL_Alarm_Send_Reset_Alarm_Log");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZCL_Alarms_QCLI_Handle, "qapi_ZB_CL_Alarm_Send_Reset_Alarm_Log", Result);
               }
            }
            else
            {
               QCLI_Printf(ZCL_Alarms_QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZCL_Alarms_QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZCL_Alarms_QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "Alarm" command to send an alarm to a client device.

   Parameter_List[0] Endpoint of the Alarms client cluster to use to send the
                     command.
   Parameter_List[1] Index of the Source cluster for the alarm.
   Parameter_List[2] The Alarm code to reset

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_Alarms_Alarm(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t                   Result;
   QCLI_Command_Status_t           Ret_Val;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   ZCL_Demo_Cluster_Info_t        *SourceClusterInfo;
   uint8_t                         AlarmCode;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      /* Validate the device ID. */
      if((Parameter_Count >= 3) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFF)))
      {
         ClusterInfo       = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[0].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_ALARMS, ZCL_DEMO_CLUSTERTYPE_SERVER);
         SourceClusterInfo = ZCL_FindClusterByIndex((uint16_t)(Parameter_List[1].Integer_Value), ZCL_DEMO_IGNORE_CLUSTER_ID);
         AlarmCode         = (uint8_t)(Parameter_List[2].Integer_Value);

         if((ClusterInfo != NULL) && (SourceClusterInfo != NULL))
         {
            Result = qapi_ZB_CL_Alarm_Send_Alarm(ClusterInfo->Handle, SourceClusterInfo->Handle, AlarmCode);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(ZCL_Alarms_QCLI_Handle, "qapi_ZB_CL_Alarm_Send_Alarm");
            }
            else
            {
               Ret_Val = QCLI_STATUS_ERROR_E;
               Display_Function_Error(ZCL_Alarms_QCLI_Handle, "qapi_ZB_CL_Alarm_Send_Alarm", Result);
            }
         }
         else
         {
            QCLI_Printf(ZCL_Alarms_QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZCL_Alarms_QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Handles callbacks for the Alarms server cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_Alarms_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Alarm_Server_Event_Data_t *EventData, uint32_t CB_Param)
{
   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      QCLI_Printf(ZCL_Alarms_QCLI_Handle, "Unhandled Alarms server event %d.\n", EventData->Event_Type);
      QCLI_Display_Prompt();
   }
}

/**
   @brief Handles callbacks for the Alarms client cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_Alarms_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Alarm_Client_Event_Data_t *EventData, uint32_t CB_Param)
{
   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      switch(EventData->Event_Type)
      {
         case QAPI_ZB_CL_ALARM_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E:
            QCLI_Printf(ZCL_Alarms_QCLI_Handle, "Alarms Client Default Response:\n");
            QCLI_Printf(ZCL_Alarms_QCLI_Handle, "  Status:        %d\n", EventData->Data.Default_Response.Status);
            QCLI_Printf(ZCL_Alarms_QCLI_Handle, "  CommandID:     0x%02X\n", EventData->Data.Default_Response.CommandId);
            QCLI_Printf(ZCL_Alarms_QCLI_Handle, "  CommandStatus: %d\n", EventData->Data.Default_Response.CommandStatus);
            break;

         case QAPI_ZB_CL_ALARM_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E:
            QCLI_Printf(ZCL_Alarms_QCLI_Handle, "Command Complete:\n");
            QCLI_Printf(ZCL_Alarms_QCLI_Handle, "  CommandStatus: %d\n", EventData->Data.Command_Complete.CommandStatus);
            break;

         case QAPI_ZB_CL_ALARM_CLIENT_EVENT_TYPE_ALARM_E:
            QCLI_Printf(ZCL_Alarms_QCLI_Handle, "Alarm:\n");
            QCLI_Printf(ZCL_Alarms_QCLI_Handle, "  ClusterId: %d\n", EventData->Data.Alarm_Command.ClusterId);
            QCLI_Printf(ZCL_Alarms_QCLI_Handle, "  AlarmCode: %d\n", EventData->Data.Alarm_Command.AlarmCode);
            break;

         case QAPI_ZB_CL_ALARM_CLIENT_EVENT_TYPE_GET_ALARM_RESPONSE_E:
            QCLI_Printf(ZCL_Alarms_QCLI_Handle, "Get Alarm Response:\n");
            QCLI_Printf(ZCL_Alarms_QCLI_Handle, "  Status:    %d\n", EventData->Data.Get_Alarm_Response.Status);
            QCLI_Printf(ZCL_Alarms_QCLI_Handle, "  ClusterId: %d\n", EventData->Data.Get_Alarm_Response.ClusterId);
            QCLI_Printf(ZCL_Alarms_QCLI_Handle, "  AlarmCode: %d\n", EventData->Data.Get_Alarm_Response.AlarmCode);
            QCLI_Printf(ZCL_Alarms_QCLI_Handle, "  Timestamp: %d\n", EventData->Data.Get_Alarm_Response.TimeStamp);
            break;

         default:
            QCLI_Printf(ZCL_Alarms_QCLI_Handle, "Unhandled Alarms client event %d.\n", EventData->Event_Type);
            break;
      }

      QCLI_Display_Prompt();
   }
}

/**
   @brief Initializes the ZCL Alarms demo.

   @param ZigBee_QCLI_Handle is the parent QCLI handle for the Alarms demo.

   @return true if the ZigBee light demo initialized successfully, false
           otherwise.
*/
qbool_t Initialize_ZCL_Alarms_Demo(QCLI_Group_Handle_t ZigBee_QCLI_Handle)
{
   qbool_t Ret_Val;

   /* Register Alarms command group. */
   ZCL_Alarms_QCLI_Handle = QCLI_Register_Command_Group(ZigBee_QCLI_Handle, &ZCL_Alarms_Cmd_Group);
   if(ZCL_Alarms_QCLI_Handle != NULL)
   {
      Ret_Val = true;
   }
   else
   {
      QCLI_Printf(ZigBee_QCLI_Handle, "Failed to register ZCL Alarms command group.\n");
      Ret_Val = false;
   }

   return(Ret_Val);
}

/**
   @brief Creates an Alarms server cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_Alarms_Demo_Create_Server(uint8_t Endpoint, void **PrivData)
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

      Result = qapi_ZB_CL_Alarm_Create_Server(ZigBee_Handle, &Ret_Val, &ClusterInfo, ALARM_LOG_SIZE, ZCL_Alarms_Demo_Server_CB, 0);
      if(Result != QAPI_OK)
      {
         Display_Function_Error(ZCL_Alarms_QCLI_Handle, "qapi_ZB_CL_Alarm_Create_Server", Result);
         Ret_Val = NULL;
      }
   }
   else
   {
      QCLI_Printf(ZCL_Alarms_QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

/**
   @brief Creates an Alarms client cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_Alarms_Demo_Create_Client(uint8_t Endpoint, void **PrivData)
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

      Result = qapi_ZB_CL_Alarm_Create_Client(ZigBee_Handle, &Ret_Val, &ClusterInfo, ZCL_Alarms_Demo_Client_CB, 0);
      if(Result != QAPI_OK)
      {
         Display_Function_Error(ZCL_Alarms_QCLI_Handle, "qapi_ZB_CL_Alarm_Create_Client", Result);
         Ret_Val = NULL;
      }
   }
   else
   {
      QCLI_Printf(ZCL_Alarms_QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

