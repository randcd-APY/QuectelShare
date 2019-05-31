/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "malloc.h"
#include "string.h"
//#include "stringl.h"
#include "qcli_api.h"
#include "qcli_util.h"
#include "zigbee_demo.h"
#include "zcl_demo.h"

#include "qapi_zb.h"
#include "qapi_zb_cl.h"
#include "qapi_zb_cl_identify.h"

static QCLI_Group_Handle_t ZCL_Identify_QCLI_Handle;

/* Function prototypes. */
static QCLI_Command_Status_t cmd_ZCL_Identify_Identify(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_Identify_IdentifyQuery(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static void ZCL_Identify_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Identify_Server_Event_Data_t *EventData, uint32_t CB_Param);
static void ZCL_Identify_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Identify_Client_Event_Data_t *EventData, uint32_t CB_Param);

/* Command list for the ZigBee light demo. */
const QCLI_Command_t ZigBee_Light_CMD_List[] =
{
   /* cmd_function                  thread  cmd_string       usage_string                             description */
   {cmd_ZCL_Identify_Identify,      false,  "Identify",      "[DevId][ClientEndpoint][Time]", "Sends an Identify command to an Identify server."},
   {cmd_ZCL_Identify_IdentifyQuery, false,  "IdentifyQuery", "[DevId][ClientEndpoint]",       "Sends an Identify Query command to an Identify server."},
};

const QCLI_Command_Group_t ZCL_Identify_Cmd_Group = {"Identify", sizeof(ZigBee_Light_CMD_List) / sizeof(QCLI_Command_t), ZigBee_Light_CMD_List};

/**
   @brief Executes the "Identify" command to request an endpoint identify
          itself.

   Parameter_List[0] ID of the device to send the command to.
   Parameter_List[1] Endpoint of the Identify client cluster to use to send the
                     command.
   Parameter_List[2] Time for the device to Identify.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_Identify_Identify(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   uint32_t                        DeviceId;
   uint16_t                        Time;

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
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         Time        = Parameter_List[2].Integer_Value;

         if(ClusterInfo != NULL)
         {
            /* Format the destination info. */
            memset(&SendInfo, 0, sizeof(qapi_ZB_CL_General_Send_Info_t));
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_Identify_Send_Identify(ClusterInfo->Handle, &SendInfo, Time);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZCL_Identify_QCLI_Handle, "qapi_ZB_CL_Identify_Send_Identify");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZCL_Identify_QCLI_Handle, "qapi_ZB_CL_Identify_Send_Identify", Result);
               }
            }
            else
            {
               QCLI_Printf(ZCL_Identify_QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZCL_Identify_QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZCL_Identify_QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "IdentifyQuery" command to query its identify status.

   Parameter_List[0] ID of the device to send the command to.
   Parameter_List[1] Endpoint of the Identify client cluster to use to send the
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
static QCLI_Command_Status_t cmd_ZCL_Identify_IdentifyQuery(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
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
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IDENTIFY, ZCL_DEMO_CLUSTERTYPE_CLIENT);

         if(ClusterInfo != NULL)
         {
            /* Format the destination info. */
            memset(&SendInfo, 0, sizeof(qapi_ZB_CL_General_Send_Info_t));
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_Identify_Send_Identify_Query(ClusterInfo->Handle, &SendInfo);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZCL_Identify_QCLI_Handle, "qapi_ZB_CL_Identify_Send_Identify_Query");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZCL_Identify_QCLI_Handle, "qapi_ZB_CL_Identify_Send_Identify_Query", Result);
               }
            }
            else
            {
               QCLI_Printf(ZCL_Identify_QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZCL_Identify_QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZCL_Identify_QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Handles callbacks for the identlfy server cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_Identify_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Identify_Server_Event_Data_t *EventData, uint32_t CB_Param)
{
   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      switch(EventData->Event_Type)
      {
         case QAPI_ZB_CL_IDENTIFY_SERVER_EVENT_TYPE_IDENTIFY_START_E:
            QCLI_Printf(ZCL_Identify_QCLI_Handle, "Start Identifying on Endpoint %d.\n", CB_Param);
            break;

         case QAPI_ZB_CL_IDENTIFY_SERVER_EVENT_TYPE_IDENTIFY_STOP_E:
            QCLI_Printf(ZCL_Identify_QCLI_Handle, "Stop Identifying on Endpoint %d.\n", CB_Param);
            break;

         default:
            QCLI_Printf(ZCL_Identify_QCLI_Handle, "Unhandled Identify server event %d.\n", EventData->Event_Type);
            break;
      }

      QCLI_Display_Prompt();
   }
}

/**
   @brief Handles callbacks for the Identify client cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_Identify_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Identify_Client_Event_Data_t *EventData, uint32_t CB_Param)
{
   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      switch(EventData->Event_Type)
      {
         case QAPI_ZB_CL_IDENTIFY_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E:
            QCLI_Printf(ZCL_Identify_QCLI_Handle, "Identify Client Default Response:\n");
            QCLI_Printf(ZCL_Identify_QCLI_Handle, "  Status:        %d\n", EventData->Data.Default_Response.Status);
            QCLI_Printf(ZCL_Identify_QCLI_Handle, "  CommandID:     0x%02X\n", EventData->Data.Default_Response.CommandId);
            QCLI_Printf(ZCL_Identify_QCLI_Handle, "  CommandStatus: %d\n", EventData->Data.Default_Response.CommandStatus);
            break;

         case QAPI_ZB_CL_IDENTIFY_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E:
            QCLI_Printf(ZCL_Identify_QCLI_Handle, "Command Complete:\n");
            QCLI_Printf(ZCL_Identify_QCLI_Handle, "  CommandStatus: %d\n", EventData->Data.Command_Complete.CommandStatus);
            break;

         case QAPI_ZB_CL_IDENTIFY_CLIENT_EVENT_TYPE_QUERY_RESPONSE_E:
            QCLI_Printf(ZCL_Identify_QCLI_Handle, "Identify Query Response:\n");
            QCLI_Printf(ZCL_Identify_QCLI_Handle, "  Status:  %d\n", EventData->Data.Query_Response.Status);
            QCLI_Printf(ZCL_Identify_QCLI_Handle, "  TimeOut: 0x%04X\n", EventData->Data.Query_Response.TimeOut);
            break;

         default:
            QCLI_Printf(ZCL_Identify_QCLI_Handle, "Unhandled Identify client event %d.\n", EventData->Event_Type);
            break;
      }

      QCLI_Display_Prompt();
   }
}

/**
   @brief Initializes the ZCL Identify demo.

   @param ZigBee_QCLI_Handle is the parent QCLI handle for the Identify demo.

   @return true if the ZigBee light demo initialized successfully, false
           otherwise.
*/
qbool_t Initialize_ZCL_Identify_Demo(QCLI_Group_Handle_t ZigBee_QCLI_Handle)
{
   qbool_t Ret_Val;

   /* Register Identify command group. */
   ZCL_Identify_QCLI_Handle = QCLI_Register_Command_Group(ZigBee_QCLI_Handle, &ZCL_Identify_Cmd_Group);
   if(ZCL_Identify_QCLI_Handle != NULL)
   {
      Ret_Val = true;
   }
   else
   {
      QCLI_Printf(ZigBee_QCLI_Handle, "Failed to register ZCL Identify command group.\n");
      Ret_Val = false;
   }

   return(Ret_Val);
}

/**
   @brief Creates an Identify server cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_Identify_Demo_Create_Server(uint8_t Endpoint, void **PrivData)
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

      Result = qapi_ZB_CL_Identify_Create_Server(ZigBee_Handle, &Ret_Val, &ClusterInfo, ZCL_Identify_Demo_Server_CB, Endpoint);
      if(Result != QAPI_OK)
      {
         Display_Function_Error(ZCL_Identify_QCLI_Handle, "qapi_ZB_CL_Identify_Create_Server", Result);
         Ret_Val = NULL;
      }
   }
   else
   {
      QCLI_Printf(ZCL_Identify_QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

/**
   @brief Creates an Identify client cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_Identify_Demo_Create_Client(uint8_t Endpoint, void **PrivData)
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

      Result = qapi_ZB_CL_Identify_Create_Client(ZigBee_Handle, &Ret_Val, &ClusterInfo, ZCL_Identify_Demo_Client_CB, 0);
      if(Result != QAPI_OK)
      {
         Display_Function_Error(ZCL_Identify_QCLI_Handle, "qapi_ZB_CL_Identify_Create_Client", Result);
         Ret_Val = NULL;
      }
   }
   else
   {
      QCLI_Printf(ZCL_Identify_QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

