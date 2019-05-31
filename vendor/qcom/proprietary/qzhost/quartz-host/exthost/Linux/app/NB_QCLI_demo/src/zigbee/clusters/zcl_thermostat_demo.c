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
#include "zcl_thermostat_demo.h"

#include "qapi_zb.h"
#include "qapi_zb_cl.h"
#include "qapi_zb_cl_thermostat.h"

/* Structure representing the ZigBee Thermostat demo context information. */
typedef struct ZigBee_Thermostat_Demo_Context_s
{
   QCLI_Group_Handle_t QCLI_Handle;     /*< QCLI handle for the main ZigBee demo. */
} ZigBee_Thermostat_Demo_Context_t;

/* The ZigBee Thermostat demo context. */
static ZigBee_Thermostat_Demo_Context_t ZigBee_Thermostat_Demo_Context;

/* Function prototypes. */
static QCLI_Command_Status_t cmd_ZCL_Thermostat_ChangeSetpoint(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static void ZCL_Thermostat_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Thermostat_Server_Event_Data_t *EventData, uint32_t CB_Param);
static void ZCL_Thermostat_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Thermostat_Client_Event_Data_t *EventData, uint32_t CB_Param);

/* Command list for the ZigBee light demo. */
const QCLI_Command_t ZigBee_Thermostat_CMD_List[] =
{
   /* cmd_function                      thread  cmd_string         usage_string                                                                  description */
   {cmd_ZCL_Thermostat_ChangeSetpoint,  false,  "ChangeSetpoint",  "[DevId][ClientEndpoint][SetPointMode(0=Heat,1=Cool,2=Both)][ChangeAmount]",  "Sends a ChangeSetpoint command to a Thermostat server."},
};

const QCLI_Command_Group_t ZCL_Thermostat_Cmd_Group = {"Thermostat", sizeof(ZigBee_Thermostat_CMD_List) / sizeof(QCLI_Command_t), ZigBee_Thermostat_CMD_List};

/**
   @brief Executes the "ChangeSetpoint" command to send a Setpoint Raise/Lower
          command to a device.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the Thermostat client cluster to use to send the
                     command.
   Parameter_List[2] is the setpoint mode.
   Parameter_List[3] is the change amount.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_Thermostat_ChangeSetpoint(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
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
      if((Parameter_Count >= 4) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), QAPI_ZB_CL_THERMOSTAT_SETPOINT_MODE_HEAT_E, QAPI_ZB_CL_THERMOSTAT_SETPOINT_MODE_BOTH_E)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), -128, 127)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_THERMOSTAT, ZCL_DEMO_CLUSTERTYPE_CLIENT);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_Thermostat_Send_Change_Setpoint(ClusterInfo->Handle, &SendInfo, (qapi_ZB_CL_Thermostat_Setpoint_Mode_t)(Parameter_List[2].Integer_Value), (int8_t)(Parameter_List[3].Integer_Value));
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZigBee_Thermostat_Demo_Context.QCLI_Handle, "qapi_ZB_CL_Thermostat_Send_Change_Setpoint");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZigBee_Thermostat_Demo_Context.QCLI_Handle, "qapi_ZB_CL_Thermostat_Send_Change_Setpoint", Result);
               }
            }
            else
            {
               QCLI_Printf(ZigBee_Thermostat_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_Thermostat_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_Thermostat_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Handles callbacks for the Thermostat server cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_Thermostat_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Thermostat_Server_Event_Data_t *EventData, uint32_t CB_Param)
{
   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      switch(EventData->Event_Type)
      {
         case QAPI_ZB_CL_THERMOSTAT_SERVER_EVENT_TYPE_SETPOINT_CHANGE_E:
            QCLI_Printf(ZigBee_Thermostat_Demo_Context.QCLI_Handle, "Setpoint Mode: %d.\n", EventData->Data.Setpoint_Change.Mode);
            QCLI_Printf(ZigBee_Thermostat_Demo_Context.QCLI_Handle, "Change Amount: %d.\n", EventData->Data.Setpoint_Change.Amount);
            break;

         default:
            QCLI_Printf(ZigBee_Thermostat_Demo_Context.QCLI_Handle, "Unhandled Thermostat server event %d.\n", EventData->Event_Type);
            break;
      }

      QCLI_Display_Prompt();
   }
}

/**
   @brief Handles callbacks for the Thermostat client cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_Thermostat_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Thermostat_Client_Event_Data_t *EventData, uint32_t CB_Param)
{
   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      switch(EventData->Event_Type)
      {
         case QAPI_ZB_CL_THERMOSTAT_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E:
            QCLI_Printf(ZigBee_Thermostat_Demo_Context.QCLI_Handle, "Thermostat Client Default Response:\n");
            QCLI_Printf(ZigBee_Thermostat_Demo_Context.QCLI_Handle, "  Status:        %d\n", EventData->Data.Default_Response.Status);
            QCLI_Printf(ZigBee_Thermostat_Demo_Context.QCLI_Handle, "  CommandID:     0x%02X\n", EventData->Data.Default_Response.CommandId);
            QCLI_Printf(ZigBee_Thermostat_Demo_Context.QCLI_Handle, "  CommandStatus: %d\n", EventData->Data.Default_Response.CommandStatus);
            break;

         default:
            QCLI_Printf(ZigBee_Thermostat_Demo_Context.QCLI_Handle, "Unhandled Thermostat client event %d.\n", EventData->Event_Type);
            break;
      }

      QCLI_Display_Prompt();
   }
}

/**
   @brief Initializes the ZCL Thermostat demo.

   @param ZigBee_QCLI_Handle is the parent QCLI handle for the Thermostat demo.

   @return true if the ZigBee light demo initialized successfully, false
           otherwise.
*/
qbool_t Initialize_ZCL_Thermostat_Demo(QCLI_Group_Handle_t ZigBee_QCLI_Handle)
{
   qbool_t Ret_Val;

   /* Register Thermostat command group. */
   ZigBee_Thermostat_Demo_Context.QCLI_Handle = QCLI_Register_Command_Group(ZigBee_QCLI_Handle, &ZCL_Thermostat_Cmd_Group);
   if(ZigBee_Thermostat_Demo_Context.QCLI_Handle != NULL)
   {
      Ret_Val = true;
   }
   else
   {
      QCLI_Printf(ZigBee_QCLI_Handle, "Failed to register ZCL Thermostat command group.\n");
      Ret_Val = false;
   }

   return(Ret_Val);
}

/**
   @brief Creates an Thermostat server cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_Thermostat_Demo_Create_Server(uint8_t Endpoint, void **PrivData)
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

      Result = qapi_ZB_CL_Thermostat_Create_Server(ZigBee_Handle, &Ret_Val, &ClusterInfo, ZCL_Thermostat_Demo_Server_CB, 0);
      if(Result != QAPI_OK)
      {
         Display_Function_Error(ZigBee_Thermostat_Demo_Context.QCLI_Handle, "qapi_ZB_CL_Thermostat_Create_Server", Result);
         Ret_Val = NULL;
      }
   }
   else
   {
      QCLI_Printf(ZigBee_Thermostat_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

/**
   @brief Creates an Thermostat client cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_Thermostat_Demo_Create_Client(uint8_t Endpoint, void **PrivData)
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

      Result = qapi_ZB_CL_Thermostat_Create_Client(ZigBee_Handle, &Ret_Val, &ClusterInfo, ZCL_Thermostat_Demo_Client_CB, 0);
      if(Result != QAPI_OK)
      {
         Display_Function_Error(ZigBee_Thermostat_Demo_Context.QCLI_Handle, "qapi_ZB_CL_Thermostat_Create_Client", Result);
         Ret_Val = NULL;
      }
   }
   else
   {
      QCLI_Printf(ZigBee_Thermostat_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

