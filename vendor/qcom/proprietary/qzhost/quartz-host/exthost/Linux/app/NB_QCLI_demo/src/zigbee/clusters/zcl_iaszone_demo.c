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
#include "zcl_onoff_demo.h"
#include "zcl_scenes_demo.h"

#include "qapi_zb.h"
#include "qapi_zb_cl.h"
#include "qapi_zb_cl_ias_zone.h"

#define IASZONE_DEMO_SERVER_ZONE_TYPE                                           (QAPI_ZB_CL_IASZONE_ZONE_TYPE_MOTION_SENSOR_E)
#define IASZONE_DEMO_SERVER_MANUFACTURER_CODE                                   (0x1234)
#define IASZONE_DEMO_SERVER_USE_TRIP_TO_PAIR                                    (true)

/* Structure representing the ZigBee onoff demo context information. */
typedef struct ZigBee_IASZone_Demo_Context_s
{
   uint8_t             NextZoneID;
   QCLI_Group_Handle_t QCLI_Handle;   /*< QCLI handle for the main ZigBee demo. */
} ZigBee_IASZone_Demo_Context_t;

/* The ZigBee IASZone demo context. */
static ZigBee_IASZone_Demo_Context_t ZigBee_IASZone_Demo_Context;

/* Function prototypes. */
static QCLI_Command_Status_t cmd_ZCL_IASZone_Server_ZoneEnrollRequest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASZone_Client_InitiateZoneEnroll(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASZone_Client_ZoneEnrollResponse(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASZone_Client_InitateNormalMode(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASZone_Client_InitateTestMode(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static void ZCL_IASZone_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_IASZone_Server_Event_Data_t *EventData, uint32_t CB_Param);
static void ZCL_IASZone_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_IASZone_Client_Event_Data_t *EventData, uint32_t CB_Param);

/* Command list for the ZigBee light demo. */
const QCLI_Command_t ZigBee_IASZone_CMD_List[] =
{
   /* cmd_function                             thread cmd_string            usage_string                                      description */
   {cmd_ZCL_IASZone_Server_ZoneEnrollRequest,  false, "ZoneEnrollRequest",  "[DevId][ServerEndpoint]",                        "Sends an ZoneEnrollRequest command to an IAS zone client."},
   {cmd_ZCL_IASZone_Client_InitiateZoneEnroll, false, "InitiateZoneEnroll", "[DevId][ClientEndpoint]",                        "Initiates the enroll process with an IAS zone server."},
   {cmd_ZCL_IASZone_Client_ZoneEnrollResponse, false, "ZoneEnrollResponse", "[DevId][ClientEndpoint]",                        "Sends an ZoneEnrollResponse command to an IAS zone server."},
   {cmd_ZCL_IASZone_Client_InitateNormalMode,  false, "InitateNormalMode",  "[DevId][ClientEndpoint]",                        "Sends an InitateNormalMode command to an IAS zone server."},
   {cmd_ZCL_IASZone_Client_InitateTestMode,    false, "InitateTestMode",    "[DevId][ClientEndpoint][Duration][Sensitivity]", "Sends an InitateTestMode command to an IAS zone server."}
};

const QCLI_Command_Group_t ZCL_IASZone_Cmd_Group = {"IASZone", sizeof(ZigBee_IASZone_CMD_List) / sizeof(QCLI_Command_t), ZigBee_IASZone_CMD_List};

/**
   @brief Executes the "ZoneEnrollRequest" command to send an IAS zone enroll
          request command to an IAS zone client device.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the IAS zone server cluster to use to send the
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
static QCLI_Command_Status_t cmd_ZCL_IASZone_Server_ZoneEnrollRequest(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
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
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ZONE, ZCL_DEMO_CLUSTERTYPE_SERVER);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_IASZone_Server_Send_Zone_Enroll_Request(ClusterInfo->Handle, &SendInfo);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZigBee_IASZone_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASZone_Server_Send_Zone_Enroll_Request");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZigBee_IASZone_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASZone_Server_Send_Zone_Enroll_Request", Result);
               }
            }
            else
            {
               QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "InitiateAutoEnroll" command to send an IAS zone auto
          enroll command to an IAS zone server device.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the IAS zone client cluster to use to send the
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
static QCLI_Command_Status_t cmd_ZCL_IASZone_Client_InitiateZoneEnroll(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   qapi_ZB_Handle_t                ZB_Handle;
   qapi_ZB_CL_Write_Attr_Record_t  AttrRecord;
   uint32_t                        DeviceId;
   uint64_t                        Extended_Address;

   /* Ensure both the stack is initialized and the switch endpoint. */
   ZB_Handle = GetZigBeeHandle();
   if(ZB_Handle != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      /* Validate the device ID. */
      if((Parameter_Count >= 2) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ZONE, ZCL_DEMO_CLUSTERTYPE_CLIENT);

         if(ClusterInfo != NULL)
         {
            /* Write our extended address to the server's CIE address attribute. */
            memset(&AttrRecord, 0, sizeof(qapi_ZB_CL_Write_Attr_Record_t));
            AttrRecord.AttrId     = QAPI_ZB_CL_IASZONE_ATTR_ID_IAS_CIE_ADDRESS;
            AttrRecord.DataType   = QAPI_ZB_CL_DATA_TYPE_IEEE_ADDRESS_E;
            AttrRecord.AttrLength = sizeof(uint64_t);
            AttrRecord.AttrValue  = (uint8_t *)&Extended_Address;

            /* Read the local extended address. */
            Result = qapi_ZB_Get_Extended_Address(ZB_Handle, &Extended_Address);
            if(Result == QAPI_OK)
            {
               memset(&SendInfo, 0, sizeof(qapi_ZB_CL_General_Send_Info_t));

               if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
               {
                  Result = qapi_ZB_CL_Write_Attributes(ClusterInfo->Handle, &SendInfo, 1, &AttrRecord);
                  if(Result == QAPI_OK)
                  {
                     Display_Function_Success(ZigBee_IASZone_Demo_Context.QCLI_Handle, "qapi_ZB_CL_Write_Attributes");
                  }
                  else
                  {
                     Ret_Val = QCLI_STATUS_ERROR_E;
                     Display_Function_Error(ZigBee_IASZone_Demo_Context.QCLI_Handle, "qapi_ZB_CL_Write_Attributes", Result);
                  }
               }
               else
               {
                  QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               Display_Function_Error(ZigBee_IASZone_Demo_Context.QCLI_Handle, "qapi_ZB_Get_Extended_Address", Result);
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "AutoEnrollResponse" command to send an IAS zone auto
          enroll response to an IAS zone server device.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the IAS zone client cluster to use to send the
                     command.
   Parameter_List[2] Zone ID assoicated with the auto enroll response.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_IASZone_Client_ZoneEnrollResponse(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t                   Result;
   QCLI_Command_Status_t           Ret_Val;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   uint8_t                         DeviceId;
   uint8_t                         ZoneID;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      /* Validate the device ID. */
      if((Parameter_Count >= 2) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ZONE, ZCL_DEMO_CLUSTERTYPE_CLIENT);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               ZoneID = ZigBee_IASZone_Demo_Context.NextZoneID;
               ZigBee_IASZone_Demo_Context.NextZoneID ++;

               Result = qapi_ZB_CL_IASZone_Client_Send_Zone_Enroll_Reponse(ClusterInfo->Handle, &SendInfo, ZoneID);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZigBee_IASZone_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASZone_Client_Send_Auto_Enroll_Reponse");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZigBee_IASZone_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASZone_Client_Send_Auto_Enroll_Reponse", Result);
               }
            }
            else
            {
               QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "InitateNormalMode" command to send an IAS zone initiate
          normal operation command to an IAS zone server device.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the IAS zone client cluster to use to send the
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
static QCLI_Command_Status_t cmd_ZCL_IASZone_Client_InitateNormalMode(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
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
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ZONE, ZCL_DEMO_CLUSTERTYPE_CLIENT);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_IASZone_Client_Send_Initiate_Normal_Operation_Mode(ClusterInfo->Handle, &SendInfo);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZigBee_IASZone_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASZone_Client_Send_Initiate_Normal_Operation_Mode");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZigBee_IASZone_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASZone_Client_Send_Initiate_Normal_Operation_Mode", Result);
               }
            }
            else
            {
               QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "InitateTestMode" command to send an IAS zone initiate
          test mode command to an IAS zone server device.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the IAS zone client cluster to use to send the
                     command.
   Parameter_List[2] is the test mode duration.
   Parameter_List[3] is the sensitivity level.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_IASZone_Client_InitateTestMode(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t                   Result;
   QCLI_Command_Status_t           Ret_Val;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   uint8_t                         DeviceId;
   uint8_t                         Duration;
   uint8_t                         Sensitivity;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      Ret_Val = QCLI_STATUS_SUCCESS_E;

      /* Validate the device ID. */
      if((Parameter_Count >= 4) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFF)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ZONE, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         Duration    = (uint8_t)(Parameter_List[2].Integer_Value);
         Sensitivity = (uint8_t)(Parameter_List[3].Integer_Value);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_IASZone_Client_Send_Initiate_Test_Mode(ClusterInfo->Handle, &SendInfo, Duration, Sensitivity);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZigBee_IASZone_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASZone_Client_Send_Initiate_Test_Mode");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZigBee_IASZone_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASZone_Client_Send_Initiate_Test_Mode", Result);
               }
            }
            else
            {
               QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Handles callbacks for the IAS zone server cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_IASZone_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_IASZone_Server_Event_Data_t *EventData, uint32_t CB_Param)
{
   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      switch(EventData->Event_Type)
      {
         case QAPI_ZB_CL_IASZONE_SERVER_EVENT_TYPE_INITIATE_TEST_MODE_E:
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "IASZone Server Initiate Test Mode:\n");
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "  TestDuration:     %d\n", EventData->Data.Intiate_Test_Mode.TestModeDuration);
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "  SensitivityLevel: %d\n", EventData->Data.Intiate_Test_Mode.CurrentZoneSensitivityLevel);
            break;

         case QAPI_ZB_CL_IASZONE_SERVER_EVENT_TYPE_INITIATE_NORMAL_OPERATION_MODE_E:
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "IASZone Server Initiate Normal Operation Mode.\n");
            break;

         case QAPI_ZB_CL_IASZONE_SERVER_EVENT_TYPE_ZONE_ENROLL_RESPONSE_E:
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "IASZone Server Zone Enroll Response:\n");
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "  ResponseCode: %d\n", EventData->Data.Enroll_Response.ResponseCode);
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "  ZoneID:       %d\n", EventData->Data.Enroll_Response.ZoneID);
            break;

         default:
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "Unhandled IASZone Server Event %d.\n", EventData->Event_Type);
            break;
      }

      QCLI_Display_Prompt();
   }
}

/**
   @brief Handles callbacks for the IASZone client cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_IASZone_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_IASZone_Client_Event_Data_t *EventData, uint32_t CB_Param)
{
   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      switch(EventData->Event_Type)
      {
         case QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E:
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "IASZone Client Default Response:\n");
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "  Status:        %d\n", EventData->Data.Default_Response.Status);
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "  CommandID:     0x%02X\n", EventData->Data.Default_Response.CommandId);
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "  CommandStatus: %d\n", EventData->Data.Default_Response.CommandStatus);
            break;

         case QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E:
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "Command Complete:\n");
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "  CommandStatus: %d\n", EventData->Data.Command_Complete.CommandStatus);
            break;

         case QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_ZONE_STATUS_CHANGE_NOTIFICATION_E:
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "IASZone Client Zone Status Change Notification:\n");
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "  ZoneStatus:     0x%04X\n", EventData->Data.Change_Notification.ZoneStatus);
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "  ExtendedStatus: %d\n", EventData->Data.Change_Notification.ExtendedStatus);
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "  ZoneID:         %d\n", EventData->Data.Change_Notification.ZoneID);
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "  Delay:          %d\n", EventData->Data.Change_Notification.Delay);
            break;

         case QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_ZONE_ENROLL_REQUEST:
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "IASZone Client Zone Enroll Request:\n");
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "  RequsetSrcAddr:   %08X%08X\n", (uint32_t)(EventData->Data.Enroll_Request.SourceAddress >> 32), (uint32_t)EventData->Data.Enroll_Request.SourceAddress);
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "  ZoneType:         0x%04X\n", EventData->Data.Enroll_Request.ZoneType);
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "  ManufacturerCode: 0x%04X\n", EventData->Data.Enroll_Request.ManufacturerCode);
            *(EventData->Data.Enroll_Request.StatusResult) = QAPI_OK;
            *(EventData->Data.Enroll_Request.ResponseCode) = QAPI_ZB_CL_IASZONE_ZONE_ENROLL_RESPONSE_CODE_SUCCESS_E;
            *(EventData->Data.Enroll_Request.ZoneID)       = ZigBee_IASZone_Demo_Context.NextZoneID;
            ZigBee_IASZone_Demo_Context.NextZoneID ++;
            break;

         default:
            QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "Unhandled IASZone client event %d.\n", EventData->Event_Type);
            break;
      }

      QCLI_Display_Prompt();
   }
}

/**
   @brief Initializes the ZCL IASZone demo.

   @param ZigBee_QCLI_Handle is the parent QCLI handle for the IASZone demo.

   @return true if the ZigBee light demo initialized successfully, false
           otherwise.
*/
qbool_t Initialize_ZCL_IASZone_Demo(QCLI_Group_Handle_t ZigBee_QCLI_Handle)
{
   qbool_t Ret_Val;

   /* Register IASZone command group. */
   ZigBee_IASZone_Demo_Context.QCLI_Handle = QCLI_Register_Command_Group(ZigBee_QCLI_Handle, &ZCL_IASZone_Cmd_Group);
   if(ZigBee_IASZone_Demo_Context.QCLI_Handle != NULL)
   {
      ZigBee_IASZone_Demo_Context.NextZoneID = 1;
      Ret_Val = true;
   }
   else
   {
      QCLI_Printf(ZigBee_QCLI_Handle, "Failed to register ZCL IASZone command group.\n");
      Ret_Val = false;
   }

   return(Ret_Val);
}

/**
   @brief Creates an IASZone server cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_IASZone_Demo_Create_Server(uint8_t Endpoint, void **PrivData)
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

      Result = qapi_ZB_CL_IASZone_Create_Server(ZigBee_Handle, &Ret_Val, IASZONE_DEMO_SERVER_ZONE_TYPE, IASZONE_DEMO_SERVER_MANUFACTURER_CODE, IASZONE_DEMO_SERVER_USE_TRIP_TO_PAIR, &ClusterInfo, ZCL_IASZone_Demo_Server_CB, 0);
      if(Result != QAPI_OK)
      {
         Display_Function_Error(ZigBee_IASZone_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASZone_Create_Server", Result);
         Ret_Val = NULL;
      }
   }
   else
   {
      QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

/**
   @brief Creates an IASZone client cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_IASZone_Demo_Create_Client(uint8_t Endpoint, void **PrivData)
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

      Result = qapi_ZB_CL_IASZone_Create_Client(ZigBee_Handle, &Ret_Val, &ClusterInfo, ZCL_IASZone_Demo_Client_CB, 0);
      if(Result != QAPI_OK)
      {
         Display_Function_Error(ZigBee_IASZone_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASZone_Create_Client", Result);
         Ret_Val = NULL;
      }
   }
   else
   {
      QCLI_Printf(ZigBee_IASZone_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

