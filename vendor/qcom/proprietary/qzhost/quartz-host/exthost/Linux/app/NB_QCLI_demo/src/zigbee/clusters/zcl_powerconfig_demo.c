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
#include "qapi_zb_cl_power_config.h"


qapi_ZB_CL_Attribute_t PowerConfigAttrList[] =
{
   /* AttributeId                                                   Flags                                 DataType                                       DataLength        DefaultReportMin  DefaultReportMax  ValueMin  ValueMax */
   {QAPI_ZB_CL_POWERCONFIG_ATTR_ID_MAINS_VOLTAGE,                  0,                                    QAPI_ZB_CL_DATA_TYPE_UNSIGNED_16BIT_INTEGER_E, sizeof(uint16_t), 0,                0,                0,        0xFFFF},
   {QAPI_ZB_CL_POWERCONFIG_ATTR_ID_MAINS_FREQUENCY,                0,                                    QAPI_ZB_CL_DATA_TYPE_UNSIGNED_8BIT_INTEGER_E,  sizeof(uint8_t),  0,                0,                0,        0xFF},
   {QAPI_ZB_CL_POWERCONFIG_ATTR_ID_MAINS_ALARM_MASK,               QAPI_ZB_CL_ATTRIBUTE_FLAG_WRITABLE,   QAPI_ZB_CL_DATA_TYPE_8BIT_BITMAP_E,            sizeof(uint8_t),  0,                0,                0,        0x03},
   {QAPI_ZB_CL_POWERCONFIG_ATTR_ID_MAINS_VOLTAGE_MIN_THRESHOLD,    QAPI_ZB_CL_ATTRIBUTE_FLAG_WRITABLE,   QAPI_ZB_CL_DATA_TYPE_UNSIGNED_16BIT_INTEGER_E, sizeof(uint16_t), 0,                0,                0,        0xFFFF},
   {QAPI_ZB_CL_POWERCONFIG_ATTR_ID_MAINS_VOLTAGE_MAX_THRESHOLD,    QAPI_ZB_CL_ATTRIBUTE_FLAG_WRITABLE,   QAPI_ZB_CL_DATA_TYPE_UNSIGNED_16BIT_INTEGER_E, sizeof(uint16_t), 0,                0,                0,        0xFFFF},
   {QAPI_ZB_CL_POWERCONFIG_ATTR_ID_MAINS_VOLTAGE_DWELL_TRIP_POINT, QAPI_ZB_CL_ATTRIBUTE_FLAG_WRITABLE,   QAPI_ZB_CL_DATA_TYPE_UNSIGNED_16BIT_INTEGER_E, sizeof(uint16_t), 0,                0,                0,        0xFFFF},
   {QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY_VOLTAGE,                0,                                    QAPI_ZB_CL_DATA_TYPE_UNSIGNED_8BIT_INTEGER_E,  sizeof(uint8_t),  0,                0,                0,        0xFF},
   {QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY_PERCENTAGE_REMAINING,   QAPI_ZB_CL_ATTRIBUTE_FLAG_REPORTABLE, QAPI_ZB_CL_DATA_TYPE_UNSIGNED_8BIT_INTEGER_E,  sizeof(uint8_t),  0x14,             0x64,             0,        0xFF},
   {QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY_ALARM_MASK,             QAPI_ZB_CL_ATTRIBUTE_FLAG_WRITABLE,   QAPI_ZB_CL_DATA_TYPE_8BIT_BITMAP_E,            sizeof(uint8_t),  0,                0,                0,        0x0F},
   {QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY_VOLTAGE_MIN_THRESHOLD,  QAPI_ZB_CL_ATTRIBUTE_FLAG_WRITABLE,   QAPI_ZB_CL_DATA_TYPE_UNSIGNED_8BIT_INTEGER_E,  sizeof(uint8_t),  0,                0,                0,        0xFF},
   {QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY_VOLTAGE_THRESHOLD1,     QAPI_ZB_CL_ATTRIBUTE_FLAG_WRITABLE,   QAPI_ZB_CL_DATA_TYPE_UNSIGNED_8BIT_INTEGER_E,  sizeof(uint8_t),  0,                0,                0,        0xFF},
   {QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY_VOLTAGE_THRESHOLD2,     QAPI_ZB_CL_ATTRIBUTE_FLAG_WRITABLE,   QAPI_ZB_CL_DATA_TYPE_UNSIGNED_8BIT_INTEGER_E,  sizeof(uint8_t),  0,                0,                0,        0xFF},
   {QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY_VOLTAGE_THRESHOLD3,     QAPI_ZB_CL_ATTRIBUTE_FLAG_WRITABLE,   QAPI_ZB_CL_DATA_TYPE_UNSIGNED_8BIT_INTEGER_E,  sizeof(uint8_t),  0,                0,                0,        0xFF}
};

#define POWER_CONFIG_ATTR_COUNT                                         (sizeof(PowerConfigAttrList) / sizeof(qapi_ZB_CL_Attribute_t))

static void ZCL_PowerConfig_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_PowerConfig_Server_Event_Data_t *EventData, uint32_t CB_Param);
static void ZCL_PowerConfig_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_Custom_Cluster_Event_Data_t *EventData, uint32_t CB_Param);

/**
   @brief Handles callbacks for the power config server cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_PowerConfig_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_PowerConfig_Server_Event_Data_t *EventData, uint32_t CB_Param)
{
   QCLI_Group_Handle_t QCLI_Handle;

   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      QCLI_Handle = GetZigBeeQCLIHandle();

      switch(EventData->Event_Type)
      {
         case QAPI_ZB_CL_POWERCONFIG_SERVER_EVENT_TYPE_ALARM_RESET_E:
            QCLI_Printf(QCLI_Handle, "Reset Power Config Alarm:\n");
            QCLI_Printf(QCLI_Handle, "  Cluster ID: 0x%04X\n", EventData->Data.Reset_Alarm_Data.ClusterId);
            QCLI_Printf(QCLI_Handle, "  Alarm Code: %u\n", EventData->Data.Reset_Alarm_Data.AlarmCode);
            break;

         default:
            QCLI_Printf(QCLI_Handle, "Unhandled PowerConfig server event %d.\n", EventData->Event_Type);
            break;
      }

      QCLI_Display_Prompt();
   }
}

/**
   @brief Handles callbacks for the power config client cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_PowerConfig_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_Custom_Cluster_Event_Data_t *EventData, uint32_t CB_Param)
{
   qbool_t DisplayPrompt;

   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      DisplayPrompt = true;

      switch(EventData->Event_Type)
      {
         case QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_CLEANUP_E:
            /* Nothing to clean up. */
            DisplayPrompt = false;
            break;

         default:
            QCLI_Printf(GetZigBeeQCLIHandle(), "Unhandled Power Config client event %d.\n", EventData->Event_Type);
            break;
      }

      if(DisplayPrompt)
      {
         QCLI_Display_Prompt();
      }
   }
}

/**
   @brief Creates an PowerConfig server cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_PowerConfig_Demo_Create_Server(uint8_t Endpoint, void **PrivData)
{
   qapi_ZB_Cluster_t         Ret_Val;
   qapi_Status_t             Result;
   qapi_ZB_Handle_t          ZigBee_Handle;
   qapi_ZB_CL_Cluster_Info_t ClusterInfo;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      memset(&ClusterInfo, 0, sizeof(qapi_ZB_CL_Cluster_Info_t));
      ClusterInfo.Endpoint       = Endpoint;
      ClusterInfo.AttributeCount = POWER_CONFIG_ATTR_COUNT;
      ClusterInfo.AttributeList  = (qapi_ZB_CL_Attribute_t *)&PowerConfigAttrList;

      Result = qapi_ZB_CL_PowerConfig_Create_Server(ZigBee_Handle, &Ret_Val, &ClusterInfo, ZCL_PowerConfig_Demo_Server_CB, 0);
      if(Result != QAPI_OK)
      {
         Display_Function_Error(GetZigBeeQCLIHandle(), "qapi_ZB_CL_PowerConfig_Create_Server", Result);
         Ret_Val = NULL;
      }
   }
   else
   {
      QCLI_Printf(GetZigBeeQCLIHandle(), "ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

/**
   @brief Creates an PowerConfig client cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_PowerConfig_Demo_Create_Client(uint8_t Endpoint, void **PrivData)
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

      Result = qapi_ZB_CL_Create_Cluster(ZigBee_Handle, &Ret_Val, QAPI_ZB_CL_CLUSTER_ID_POWER_CONFIG, &ClusterInfo, QAPI_ZB_CL_FRAME_DIRECTION_TO_CLIENT_E, ZCL_PowerConfig_Demo_Client_CB, 0);
      if(Result != QAPI_OK)
      {
         Display_Function_Error(GetZigBeeQCLIHandle(), "qapi_ZB_CL_Create_Cluster", Result);
         Ret_Val = NULL;
      }
   }
   else
   {
      QCLI_Printf(GetZigBeeQCLIHandle(), "ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

