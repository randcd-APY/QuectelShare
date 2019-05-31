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
#include "qapi_zb_cl_ias_wd.h"

/* Structure representing the ZigBee onoff demo context information. */
typedef struct ZigBee_IASWD_Demo_Context_s
{
   QCLI_Group_Handle_t QCLI_Handle;   /*< QCLI handle for the main ZigBee demo. */
} ZigBee_IASWD_Demo_Context_t;

/* The ZigBee IASWD demo context. */
static ZigBee_IASWD_Demo_Context_t ZigBee_IASWD_Demo_Context;

/* Function prototypes. */
static QCLI_Command_Status_t cmd_ZCL_IASWD_StartWarning(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASWD_Squawk(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static void ZCL_IASWD_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_IASWD_Server_Event_Data_t *EventData, uint32_t CB_Param);
static void ZCL_IASWD_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_IASWD_Client_Event_Data_t *EventData, uint32_t CB_Param);

/* Command list for the ZigBee light demo. */
const QCLI_Command_t ZigBee_IASWD_CMD_List[] =
{
   /* cmd_function              thread  cmd_string      usage_string                                                                                      description */
   {cmd_ZCL_IASWD_StartWarning, false,  "StartWarning", "[DevId][ClientEndpoint][WarningMode][SirenLevel][StorbeMode][StrobeLevel][DutyCycle][Duration]", "Sends a Start Warning command to an IAS WD server."},
   {cmd_ZCL_IASWD_Squawk,       false,  "Squawk",       "[DevId][ClientEndpoint][SquawkMode][SquawkLevel][StrobeMode]",                                   "Sends a Squawk command to an IAS WD server."}
};

const QCLI_Command_Group_t ZCL_IASWD_Cmd_Group = {"IASWD", sizeof(ZigBee_IASWD_CMD_List) / sizeof(QCLI_Command_t), ZigBee_IASWD_CMD_List};

/**
   @brief Executes the "StartWarning" command to send an IAS WD start warning
          request to an IAS WD server device.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the IAS WD client cluster to use to send the
                     command.
   Parameter_List[2] is the warning mode.
   Parameter_List[3] is the siren level.
   Parameter_List[4] is the strobe mode.
   Parameter_List[5] is the strobe level.
   Parameter_List[6] is the strobe duty cycle.
   Parameter_List[7] is the warn duration.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_IASWD_StartWarning(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t                     Result;
   QCLI_Command_Status_t             Ret_Val;
   ZCL_Demo_Cluster_Info_t          *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t    SendInfo;
   uint8_t                           DeviceId;
   qapi_ZB_CL_IASWD_Start_Warning_t  Request;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      /* Validate the device ID. */
      if((Parameter_Count >= 8) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 6)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 3)) &&
         (Verify_Integer_Parameter(&(Parameter_List[4]), 0, 1)) &&
         (Verify_Integer_Parameter(&(Parameter_List[5]), 0, 3)) &&
         (Verify_Integer_Parameter(&(Parameter_List[6]), 0, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[7]), 0, 0xFFFF)))
      {
         memset(&Request, 0, sizeof(Request));

         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_WD, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         Request.WarningMode     = (qapi_ZB_CL_IASWD_Warning_Mode_t)(Parameter_List[2].Integer_Value);
         Request.SirenLevel      = (qapi_ZB_CL_IASWD_Level_t)(Parameter_List[3].Integer_Value);
         Request.StrobeMode      = (qapi_ZB_CL_IASWD_Strobe_Mode_t)(Parameter_List[4].Integer_Value);
         Request.StrobeLevel     = (qapi_ZB_CL_IASWD_Level_t)(Parameter_List[5].Integer_Value);
         Request.StrobeDutyCycle = (uint8_t)(Parameter_List[6].Integer_Value);
         Request.WarningDuration = (uint16_t)(Parameter_List[7].Integer_Value);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_IASWD_Client_Send_Start_Warning(ClusterInfo->Handle, &SendInfo, &Request);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZigBee_IASWD_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASWD_Client_Send_Start_Warning");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZigBee_IASWD_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASWD_Client_Send_Start_Warning", Result);
               }
            }
            else
            {
               QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "Squawk" command to send an IAS WD squawk request to an
          IAS WD server device.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the IAS WD server cluster to use to send the
                     command.
   Parameter_List[2] is the squawk mode.
   Parameter_List[3] is the strobe mode.
   Parameter_List[4] is the squawk level.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_IASWD_Squawk(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t                   Result;
   QCLI_Command_Status_t           Ret_Val;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   uint8_t                         DeviceId;
   qapi_ZB_CL_IASWD_Squawk_t       Request;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      /* Validate the device ID. */
      if((Parameter_Count >= 5) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 1)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 1)) &&
         (Verify_Integer_Parameter(&(Parameter_List[4]), 0, 3)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_WD, ZCL_DEMO_CLUSTERTYPE_CLIENT);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               memset(&Request, 0, sizeof(Request));

               Request.SquawkMode       = (qapi_ZB_CL_IASWD_Squawk_Mode_t)(Parameter_List[2].Integer_Value);
               Request.SquawkStrobeMode = (qapi_ZB_CL_IASWD_Strobe_Mode_t)(Parameter_List[3].Integer_Value);
               Request.SquawkLevel      = (qapi_ZB_CL_IASWD_Level_t)(Parameter_List[4].Integer_Value);

               Result = qapi_ZB_CL_IASWD_Client_Send_Squawk(ClusterInfo->Handle, &SendInfo, &Request);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZigBee_IASWD_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASWD_Client_Send_Squawk");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZigBee_IASWD_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASWD_Client_Send_Squawk", Result);
               }
            }
            else
            {
               QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Handles callbacks for the IAS WD server cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_IASWD_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_IASWD_Server_Event_Data_t *EventData, uint32_t CB_Param)
{
   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      switch(EventData->Event_Type)
      {
         case QAPI_ZB_CL_IASWD_SERVER_EVENT_TYPE_START_WARNING_E:
            QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "IASWD Server Start Warning:\n");
            QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "  WarningMode:     %d\n", EventData->Data.Start_Warning.WarningMode);
            QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "  StrobeMode:      %d\n", EventData->Data.Start_Warning.StrobeMode);
            QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "  SirenLevel:      %d\n", EventData->Data.Start_Warning.SirenLevel);
            QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "  WarningDuration: %d\n", EventData->Data.Start_Warning.WarningDuration);
            QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "  StrobeDutyCycle: %d\n", EventData->Data.Start_Warning.StrobeDutyCycle);
            QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "  StrobeLevel:     %d\n", EventData->Data.Start_Warning.StrobeLevel);
            break;

         case QAPI_ZB_CL_IASWD_SERVER_EVENT_TYPE_SQUAWK_E:
            QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "IASWD Server Squawk:\n");
            QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "  SquawkMode:  %d\n", EventData->Data.Squawk.SquawkMode);
            QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "  StrobeMode:  %d\n", EventData->Data.Squawk.SquawkStrobeMode);
            QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "  SquawkLevel: %d\n", EventData->Data.Squawk.SquawkLevel);
            break;

         default:
            QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "Unhandled IASWD Server Event %d.\n", EventData->Event_Type);
            break;
      }

      QCLI_Display_Prompt();
   }
}

/**
   @brief Handles callbacks for the IAS WD client cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_IASWD_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_IASWD_Client_Event_Data_t *EventData, uint32_t CB_Param)
{
   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      switch(EventData->Event_Type)
      {
         case QAPI_ZB_CL_IASWD_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E:
            QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "IASWD Client Default Response:\n");
            QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "  Status:        %d\n", EventData->Data.Default_Response.Status);
            QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "  CommandID:     0x%02X\n", EventData->Data.Default_Response.CommandId);
            QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "  CommandStatus: %d\n", EventData->Data.Default_Response.CommandStatus);
            break;

         case QAPI_ZB_CL_IASWD_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E:
            QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "Command Complete:\n");
            QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "  CommandStatus: %d\n", EventData->Data.Command_Complete.CommandStatus);
            break;

         default:
            QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "Unhandled IASWD client event %d.\n", EventData->Event_Type);
            break;
      }

      QCLI_Display_Prompt();
   }
}

/**
   @brief Initializes the ZCL IAS WD demo.

   @param ZigBee_QCLI_Handle is the parent QCLI handle for the IASWD demo.

   @return true if the ZigBee light demo initialized successfully, false
           otherwise.
*/
qbool_t Initialize_ZCL_IASWD_Demo(QCLI_Group_Handle_t ZigBee_QCLI_Handle)
{
   qbool_t Ret_Val;

   /* Register IASWD command group. */
   ZigBee_IASWD_Demo_Context.QCLI_Handle = QCLI_Register_Command_Group(ZigBee_QCLI_Handle, &ZCL_IASWD_Cmd_Group);
   if(ZigBee_IASWD_Demo_Context.QCLI_Handle != NULL)
   {
      Ret_Val = true;
   }
   else
   {
      QCLI_Printf(ZigBee_QCLI_Handle, "Failed to register ZCL IASWD command group.\n");
      Ret_Val = false;
   }

   return(Ret_Val);
}

/**
   @brief Creates an IAS WD server cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_IASWD_Demo_Create_Server(uint8_t Endpoint, void **PrivData)
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

      Result = qapi_ZB_CL_IASWD_Create_Server(ZigBee_Handle, &Ret_Val, &ClusterInfo, ZCL_IASWD_Demo_Server_CB, 0);
      if(Result != QAPI_OK)
      {
         Display_Function_Error(ZigBee_IASWD_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASWD_Create_Server", Result);
         Ret_Val = NULL;
      }
   }
   else
   {
      QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

/**
   @brief Creates an IAS WD client cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_IASWD_Demo_Create_Client(uint8_t Endpoint, void **PrivData)
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

      Result = qapi_ZB_CL_IASWD_Create_Client(ZigBee_Handle, &Ret_Val, &ClusterInfo, ZCL_IASWD_Demo_Client_CB, 0);
      if(Result != QAPI_OK)
      {
         Display_Function_Error(ZigBee_IASWD_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASWD_Create_Client", Result);
         Ret_Val = NULL;
      }
   }
   else
   {
      QCLI_Printf(ZigBee_IASWD_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

