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
#include "zcl_custom_demo.h"

#include "qapi_zb.h"
#include "qapi_zb_cl.h"

#define CUSTOM_CLUSTER_COMMAND_ID                                       (1)

static QCLI_Group_Handle_t ZCL_Custom_QCLI_Handle;

static const qapi_ZB_CL_Header_t Custom_ZCL_Header =
{
   CUSTOM_CLUSTER_COMMAND_ID,                                                                         /* CommandId */
   QAPI_ZB_CL_HEADER_FLAG_FRAME_TYPE_CLUSTER_SPECIFIC | QAPI_ZB_CL_HEADER_FLAG_MANUFACTURER_SPECIFIC, /* Flags */
   0x1234,                                                                                            /* ManufacturerCode */
   10,                                                                                                /* SequenceNumber */
};

static const qapi_ZB_CL_Attribute_t Custom_Server_Attr_List[] =
{
   /* AttributeId Flags                               DataType                                      DataLength       DefaultReportMin  DefaultReportMax  ValueMin  ValueMax */
   {0x0001,       QAPI_ZB_CL_ATTRIBUTE_FLAG_WRITABLE, QAPI_ZB_CL_DATA_TYPE_UNSIGNED_8BIT_INTEGER_E, sizeof(uint8_t), 0,                0,                0,        0xFF},
};

#define CUSTOM_SERVER_ATTR_COUNT                                        (sizeof(Custom_Server_Attr_List) / sizeof(qapi_ZB_CL_Attribute_t))

static const uint8_t Custom_Payload[] = "Test Packet";
#define CUSTOM_PAYLOAD_LENGTH                                           (sizeof(Custom_Payload))

/* Function prototypes. */
static QCLI_Command_Status_t cmd_ZCL_Custom_SendCommand(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static void ZCL_Custom_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_Custom_Cluster_Event_Data_t *EventData, uint32_t CB_Param);
static void ZCL_Custom_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_Custom_Cluster_Event_Data_t *EventData, uint32_t CB_Param);

/* Command list for the ZigBee light demo. */
static const QCLI_Command_t ZigBee_Custom_CMD_List[] =
{
   /* cmd_function              thread  cmd_string     usage_string               description */
   {cmd_ZCL_Custom_SendCommand, false,  "SendCommand", "[DevId][ClientEndpoint]", "Sends a command to a custom demo server"},
};

const QCLI_Command_Group_t ZCL_Custom_Cmd_Group = {"Custom", sizeof(ZigBee_Custom_CMD_List) / sizeof(QCLI_Command_t), ZigBee_Custom_CMD_List};

/**
   @brief Executes the "SendCommand" command to send a custom cluster command.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the Custom client cluster to use to send the
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
static QCLI_Command_Status_t cmd_ZCL_Custom_SendCommand(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
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
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), ZCL_CUSTOM_DEMO_CLUSTER_CLUSTER_ID, ZCL_DEMO_CLUSTERTYPE_CLIENT);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_Send_Command(ClusterInfo->Handle, &SendInfo, true, &Custom_ZCL_Header, CUSTOM_PAYLOAD_LENGTH, Custom_Payload);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZCL_Custom_QCLI_Handle, "qapi_ZB_CL_Send_Command");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZCL_Custom_QCLI_Handle, "qapi_ZB_CL_Send_Command", Result);
               }
            }
            else
            {
               QCLI_Printf(ZCL_Custom_QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZCL_Custom_QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZCL_Custom_QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Handles callbacks for the on/off server cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_Custom_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_Custom_Cluster_Event_Data_t *EventData, uint32_t CB_Param)
{
   qapi_Status_t Result;
   qbool_t       DisplayPrompt;

   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      DisplayPrompt = true;

      switch(EventData->Event_Type)
      {
         case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_UNPARSED_DATA_E:
            QCLI_Printf(ZCL_Custom_QCLI_Handle, "Custom Cluster Command %d:\n", EventData->Data.Unparsed_Data.Header.CommandId);
            Dump_Data(ZCL_Custom_QCLI_Handle, "  ", EventData->Data.Unparsed_Data.APSDEData.ASDULength, EventData->Data.Unparsed_Data.APSDEData.ASDU);
            *(EventData->Data.Unparsed_Data.Result) = QAPI_OK;

            Result = qapi_ZB_CL_Default_Response(Cluster, &(EventData->Data.Unparsed_Data.APSDEData), &(EventData->Data.Unparsed_Data.Header), QAPI_OK);
            if(Result != QAPI_OK)
            {
               Display_Function_Error(ZCL_Custom_QCLI_Handle, "qapi_ZB_CL_Default_Response", Result);
            }
            break;

         case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_ALARM_RESET_E:
            QCLI_Printf(ZCL_Custom_QCLI_Handle, "Reset Alarm:\n");
            QCLI_Printf(ZCL_Custom_QCLI_Handle, "  Cluster ID: 0x%04X\n", EventData->Data.Reset_Alarm_Data.ClusterId);
            QCLI_Printf(ZCL_Custom_QCLI_Handle, "  Alarm Code: %u\n", EventData->Data.Reset_Alarm_Data.AlarmCode);
            break;

         case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_CLEANUP_E:
            /* Nothing to clean up. */
            DisplayPrompt = false;
            break;

         default:
            QCLI_Printf(ZCL_Custom_QCLI_Handle, "Unhandled Custom server event %d.\n", EventData->Event_Type);
            break;
      }

      if(DisplayPrompt)
      {
         QCLI_Display_Prompt();
      }
   }
}

/**
   @brief Handles callbacks for the Custom client cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_Custom_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_Custom_Cluster_Event_Data_t *EventData, uint32_t CB_Param)
{
   qbool_t DisplayPrompt;

   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      DisplayPrompt = true;

      switch(EventData->Event_Type)
      {
         case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_DEFAULT_RESPONSE_E:
            QCLI_Printf(ZCL_Custom_QCLI_Handle, "Custom Client Default Response:\n");
            QCLI_Printf(ZCL_Custom_QCLI_Handle, "  Status:        %d\n", EventData->Data.Default_Response.Status);
            QCLI_Printf(ZCL_Custom_QCLI_Handle, "  CommandID:     0x%02X\n", EventData->Data.Default_Response.CommandId);
            QCLI_Printf(ZCL_Custom_QCLI_Handle, "  CommandStatus: %d\n", EventData->Data.Default_Response.CommandStatus);
            break;

         case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_COMMAND_COMPLETE_E:
            QCLI_Printf(ZCL_Custom_QCLI_Handle, "Command Complete:\n");
            QCLI_Printf(ZCL_Custom_QCLI_Handle, "  CommandStatus: %d\n", EventData->Data.Command_Complete.CommandStatus);
            break;

         case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_ALARM_RESET_E:
            QCLI_Printf(ZCL_Custom_QCLI_Handle, "Reset Alarm:\n");
            QCLI_Printf(ZCL_Custom_QCLI_Handle, "  ClusterId: 0x%04X\n", EventData->Data.Reset_Alarm_Data.ClusterId);
            QCLI_Printf(ZCL_Custom_QCLI_Handle, "  AlarmCode: %d\n", EventData->Data.Reset_Alarm_Data.AlarmCode);
            break;

         case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_CLEANUP_E:
            /* Nothing to clean up. */
            DisplayPrompt = false;
            break;

         case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_UNPARSED_RESPONSE_E:
            QCLI_Printf(ZCL_Custom_QCLI_Handle, "Unparsed client response: %d.\n", EventData->Event_Type);
            break;

         default:
            QCLI_Printf(ZCL_Custom_QCLI_Handle, "Unhandled Custom client event %d.\n", EventData->Event_Type);
            break;
      }

      if(DisplayPrompt)
      {
         QCLI_Display_Prompt();
      }
   }
}

/**
   @brief Initializes the ZCL Custom demo.

   @param ZigBee_QCLI_Handle is the parent QCLI handle for the Custom demo.

   @return true if the ZigBee light demo initialized successfully, false
           otherwise.
*/
qbool_t Initialize_ZCL_Custom_Demo(QCLI_Group_Handle_t ZigBee_QCLI_Handle)
{
   qbool_t Ret_Val;

   /* Register Custom command group. */
   ZCL_Custom_QCLI_Handle = QCLI_Register_Command_Group(ZigBee_QCLI_Handle, &ZCL_Custom_Cmd_Group);
   if(ZCL_Custom_QCLI_Handle != NULL)
   {
      Ret_Val = true;
   }
   else
   {
      QCLI_Printf(ZigBee_QCLI_Handle, "Failed to register ZCL Custom command group.\n");
      Ret_Val = false;
   }

   return(Ret_Val);
}

/**
   @brief Creates an Custom server cluster.

   @param Endpoint   is the endpoint the cluster will be part of.
   @param DeviceName is the string representation of the endpoint the cluster
                     will be part of.

   @return is the ClusterIndex of the newly created cluster or a negative value
           if there was an error.
*/
qapi_ZB_Cluster_t ZCL_Custom_Demo_Create_Server(uint8_t Endpoint, void **PrivData)
{
   qapi_ZB_Cluster_t         Ret_Val;
   qapi_Status_t             Result;
   qapi_ZB_Handle_t          ZigBee_Handle;
   qapi_ZB_CL_Cluster_Info_t Cluster_Info;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      Cluster_Info.Endpoint       = Endpoint;
      Cluster_Info.AttributeCount = CUSTOM_SERVER_ATTR_COUNT;
      Cluster_Info.AttributeList  = (qapi_ZB_CL_Attribute_t *)Custom_Server_Attr_List;

      Result = qapi_ZB_CL_Create_Cluster(ZigBee_Handle, &Ret_Val, ZCL_CUSTOM_DEMO_CLUSTER_CLUSTER_ID, &Cluster_Info, QAPI_ZB_CL_FRAME_DIRECTION_TO_SERVER_E, ZCL_Custom_Demo_Server_CB, 0);
      if(Result != QAPI_OK)
      {
         Display_Function_Error(ZCL_Custom_QCLI_Handle, "qapi_ZB_CL_Create_Cluster", Result);
         Ret_Val = NULL;
      }
   }
   else
   {
      QCLI_Printf(ZCL_Custom_QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

/**
   @brief Creates an Custom client cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_Custom_Demo_Create_Client(uint8_t Endpoint, void **PrivData)
{
   qapi_ZB_Cluster_t         Ret_Val;
   qapi_Status_t             Result;
   qapi_ZB_Handle_t          ZigBee_Handle;
   qapi_ZB_CL_Cluster_Info_t Cluster_Info;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      Cluster_Info.Endpoint       = Endpoint;
      Cluster_Info.AttributeCount = 0;
      Cluster_Info.AttributeList  = NULL;

      Result = qapi_ZB_CL_Create_Cluster(ZigBee_Handle, &Ret_Val, ZCL_CUSTOM_DEMO_CLUSTER_CLUSTER_ID, &Cluster_Info, QAPI_ZB_CL_FRAME_DIRECTION_TO_CLIENT_E, ZCL_Custom_Demo_Client_CB, 0);
      if(Result != QAPI_OK)
      {
         Display_Function_Error(ZCL_Custom_QCLI_Handle, "qapi_ZB_CL_Create_Cluster", Result);
         Ret_Val = NULL;
      }
   }
   else
   {
      QCLI_Printf(ZCL_Custom_QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

