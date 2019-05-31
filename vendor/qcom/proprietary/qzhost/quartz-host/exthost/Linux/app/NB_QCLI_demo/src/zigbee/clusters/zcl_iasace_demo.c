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
#include "qapi_zb_cl_ias_ace.h"
#include "qsOSAL.h"

/* Maximum number of zones that can be read by the IAS ACE demo. */
#define IASACE_DEMO_MAX_ZONE_LIST_SIZE                                  (16)

/* Structure representing the ZigBee IAS ACE demo context information. */
typedef struct ZigBee_IASACE_Demo_Context_s
{
   QCLI_Group_Handle_t QCLI_Handle;
   uint8_t             ArmCode[QAPI_ZB_CL_IASACE_ARM_CODE_MAX_LENGTH];
} ZigBee_IASACE_Demo_Context_t;

/* The ZigBee IASACE demo context. */
static ZigBee_IASACE_Demo_Context_t ZigBee_IASACE_Demo_Context;

/* Function prototypes. */
static QCLI_Command_Status_t cmd_ZCL_IASACE_SetClientArmCode(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASACE_Arm(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASACE_Bypass(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASACE_Emergency(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASACE_Fire(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASACE_Panic(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASACE_GetZoneIDMap(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASACE_GetZoneInfo(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASACE_GetPanelStatus(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASACE_GetBypassedZoneList(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASACE_GetZoneStatus(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASACE_SetServerArmCode(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASACE_GetServerArmCode(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASACE_AddZone(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASACE_DeleteZone(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASACE_ListZones(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASACE_SetZoneStatus(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASACE_SetLocalPanelStatus(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASACE_GetLocalPanelStatus(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASACE_SetBypssPermit(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASACE_SetBypssStatus(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_IASACE_GetZoneList(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static void ZCL_IASACE_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_IASACE_Server_Event_Data_t *EventData, uint32_t CB_Param);
static void ZCL_IASACE_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_IASACE_Client_Event_Data_t *EventData, uint32_t CB_Param);

/* Command list for the ZigBee light demo. */
const QCLI_Command_t ZigBee_IASACE_CMD_List[] =
{
   /* cmd_function                      thread  cmd_string             usage_string                                                                   description */
   {cmd_ZCL_IASACE_SetClientArmCode,    false,  "SetClientArmCode",    "[ArmCode]",                                                                   "Sets the arm/disarm code used for IAS ACE client commands."},
   {cmd_ZCL_IASACE_Arm,                 false,  "Arm",                 "[DevId][ClientEndpoint][ZoneID][ArmMode]",                                    "Sends an arm command to an IAS ACE server."},
   {cmd_ZCL_IASACE_Bypass,              false,  "Bypass",              "[DevId][ClientEndpoint][ZoneID]",                                             "Sends a bypass command to an IAS ACE server."},
   {cmd_ZCL_IASACE_Emergency,           false,  "Emergency",           "[DevId][ClientEndpoint]",                                                     "Sends an emergency command to an IAS ACE server."},
   {cmd_ZCL_IASACE_Fire,                false,  "Fire",                "[DevId][ClientEndpoint]",                                                     "Sends a fire command to an IAS ACE server."},
   {cmd_ZCL_IASACE_Panic,               false,  "Panic",               "[DevId][ClientEndpoint]",                                                     "Sends a panic command to an IAS ACE server."},
   {cmd_ZCL_IASACE_GetZoneIDMap,        false,  "GetZoneIDMap",        "[DevId][ClientEndpoint]",                                                     "Sends a get Zone ID Map command to an IAS ACE server."},
   {cmd_ZCL_IASACE_GetZoneInfo,         false,  "GetZoneInfo",         "[DevId][ClientEndpoint][ZoneID]",                                             "Sends a get Zone Info command to an IAS ACE server."},
   {cmd_ZCL_IASACE_GetPanelStatus,      false,  "GetPanelStatus",      "[DevId][ClientEndpoint]",                                                     "Sends a get Panel Status command to an IAS ACE server."},
   {cmd_ZCL_IASACE_GetBypassedZoneList, false,  "GetBypassedZoneList", "[DevId][ClientEndpoint]",                                                     "Sends a get Bypassed Zone List command to an IAS ACE server."},
   {cmd_ZCL_IASACE_GetZoneStatus,       false,  "GetZoneStatus",       "[DevId][ClientEndpoint][StartZoneID][NumOfZoneID][StatusMask (0=Ignore)]",    "Sends a get Zone Status command to an IAS ACE server."},
   {cmd_ZCL_IASACE_SetServerArmCode,    false,  "SetServerArmCode",    "[ServerEndpoint][ArmCode]",                                                   "Sets the arm/disarm code on an IAS ACE server cluster."},
   {cmd_ZCL_IASACE_GetServerArmCode,    false,  "GetServerArmCode",    "[ServerEndpoint]",                                                            "Gets the arm/disarm code from an IAS ACE server cluster."},
   {cmd_ZCL_IASACE_AddZone,             false,  "AddZone",             "[ServerEndpoint][ZoneID][ZoneType][ZoneAddr][ZoneLabel]",                     "Adds a zone to the IAS ACE server."},
   {cmd_ZCL_IASACE_DeleteZone,          false,  "DeleteZone",          "[ServerEndpoint][ZoneID]",                                                    "Deletes a zone from the IAS ACE server."},
   {cmd_ZCL_IASACE_ListZones,           false,  "ListZones",           "[ServerEndpoint]",                                                            "List all zones on the IAS ACE server."},
   {cmd_ZCL_IASACE_SetZoneStatus,       false,  "SetZoneStatus",       "[ServerEndpoint][ZoneID][Status][AudibleNotify][AlarmStatus]",                "Sets the status of a zone on the IAS ACE server."},
   {cmd_ZCL_IASACE_SetLocalPanelStatus, false,  "SetLocalPanelStatus", "[ServerEndpoint][PanelStatus][SecondsRemaining][AudibleNotify][AlarmStatus]", "Sets panel status on the IAS ACE server."},
   {cmd_ZCL_IASACE_GetLocalPanelStatus, false,  "GetLocalPanelStatus", "[ServerEndpoint]",                                                            "Gets panel status from the IAS ACE server."},
   {cmd_ZCL_IASACE_SetBypssPermit,      false,  "SetBypssPermit",      "[ServerEndpoint][ZoneID][BypassPermit]",                                      "Sets bypass permit of a zone on the IAS ACE server."},
   {cmd_ZCL_IASACE_SetBypssStatus,      false,  "SetBypssStatus",      "[ServerEndpoint][ZoneID][BypassStatus]",                                      "Sets bypass status of a zone on the IAS ACE server."},
   {cmd_ZCL_IASACE_GetZoneList,         false,  "GetZoneList",         "[ServerEndpoint]",                                                            "Gets the list of zones currently registered with the IAS ACE server."}
};

const QCLI_Command_Group_t ZCL_IASACE_Cmd_Group = {"IASACE", sizeof(ZigBee_IASACE_CMD_List) / sizeof(QCLI_Command_t), ZigBee_IASACE_CMD_List};

/**
   @brief Executes the "SetClientArmCode" command to set the arm/disarm code
          used for IAS client cluster commands.

   Parameter_List[0] The arm/disarm code to be set.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_IASACE_SetClientArmCode(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;

   if(Parameter_Count >= 1)
   {
      QSOSAL_StringCopy_S((char *)(ZigBee_IASACE_Demo_Context.ArmCode), sizeof(ZigBee_IASACE_Demo_Context.ArmCode), Parameter_List[0].String_Value);
      QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Arm/Disarm code set.\n");

      Ret_Val = QCLI_STATUS_SUCCESS_E;
   }
   else
   {
      Ret_Val = QCLI_STATUS_USAGE_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SendArm" command to send Arm command to an IAS ACE
          server.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the IAS ACE client cluster to use to send the
                     command.
   Parameter_List[2] is the zone ID.
   Parameter_List[3] is the arm mode.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_IASACE_Arm(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   uint8_t                         DeviceId;
   uint8_t                         ZoneID;
   qapi_ZB_CL_IASACE_Arm_Mode_t    ArmMode;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      /* Validate the device ID. */
      if((Parameter_Count >= 4) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), QAPI_ZB_CL_IASACE_ARM_MODE_DISARM_E, QAPI_ZB_CL_IASACE_ARM_MODE_ARM_ALL_ZONES_E)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ACE, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         ZoneID      = (uint8_t)(Parameter_List[2].Integer_Value);
         ArmMode     = (qapi_ZB_CL_IASACE_Arm_Mode_t)(Parameter_List[3].Integer_Value);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_IASACE_Client_Send_Arm(ClusterInfo->Handle, &SendInfo, ZoneID, ArmMode, ZigBee_IASACE_Demo_Context.ArmCode);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Client_Send_Arm");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Client_Send_Arm", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SendBypass" command to send Bypass command to an IAS ACE
          server.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the IAS ACE client cluster to use to send the
                     command.
   Parameter_List[2] Zone to bypass.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_IASACE_Bypass(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   uint8_t                         DeviceId;
   uint8_t                         ZoneID;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      /* Validate the device ID. */
      if((Parameter_Count >= 3) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ACE, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         ZoneID      = (uint8_t)(Parameter_List[2].Integer_Value);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_IASACE_Client_Send_Bypass(ClusterInfo->Handle, &SendInfo, 1, &ZoneID, ZigBee_IASACE_Demo_Context.ArmCode);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Client_Send_Bypass");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Client_Send_Bypass", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SendEmergency" command to send Emergency command to an
          IAS ACE server.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the IAS ACE client cluster to use to send the
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
static QCLI_Command_Status_t cmd_ZCL_IASACE_Emergency(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   uint8_t                         DeviceId;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      /* Validate the device ID. */
      if((Parameter_Count >= 2) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ACE, ZCL_DEMO_CLUSTERTYPE_CLIENT);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_IASACE_Client_Send_Emergency(ClusterInfo->Handle, &SendInfo);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Client_Send_Emergency");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Client_Send_Emergency", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SendFire" command to send Fire command to an IAS ACE
          server.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the IAS ACE client cluster to use to send the
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
static QCLI_Command_Status_t cmd_ZCL_IASACE_Fire(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   uint8_t                         DeviceId;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      /* Validate the device ID. */
      if((Parameter_Count >= 2) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ACE, ZCL_DEMO_CLUSTERTYPE_CLIENT);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_IASACE_Client_Send_Fire(ClusterInfo->Handle, &SendInfo);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Client_Send_Fire");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Client_Send_Fire", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SendPanic" command to send Panic command to an IAS ACE
          server.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the IAS ACE client cluster to use to send the
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
static QCLI_Command_Status_t cmd_ZCL_IASACE_Panic(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   uint8_t                         DeviceId;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      /* Validate the device ID. */
      if((Parameter_Count >= 2) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ACE, ZCL_DEMO_CLUSTERTYPE_CLIENT);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_IASACE_Client_Send_Panic(ClusterInfo->Handle, &SendInfo);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Client_Send_Panic");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Client_Send_Panic", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "GetZoneIDMap" command to send Get Zone ID Map command
          to an IAS ACE server.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the IAS ACE client cluster to use to send the
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
static QCLI_Command_Status_t cmd_ZCL_IASACE_GetZoneIDMap(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   uint8_t                         DeviceId;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      /* Validate the device ID. */
      if((Parameter_Count >= 2) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ACE, ZCL_DEMO_CLUSTERTYPE_CLIENT);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));
            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_IASACE_Client_Send_Get_ZoneID_Map(ClusterInfo->Handle, &SendInfo);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Client_Send_Get_ZoneID_Map");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Client_Send_Get_ZoneID_Map", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "GetZoneInfo" command to Get Zone Info command to an IAS
          ACE server.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the IAS ACE client cluster to use to send the
                     command.
   Parameter_List[2] is the zone ID.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_IASACE_GetZoneInfo(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   uint8_t                         DeviceId;
   uint8_t                         ZoneID;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      /* Validate the device ID. */
      if((Parameter_Count >= 3) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFF)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ACE, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         ZoneID      = (uint8_t)(Parameter_List[2].Integer_Value);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_IASACE_Client_Send_Get_Zone_Info(ClusterInfo->Handle, &SendInfo, ZoneID);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Client_Send_Get_Zone_Info");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Client_Send_Get_Zone_Info", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "GetPanelStatus" command to send Get Panel Status command
          to an IAS ACE server.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the IAS ACE client cluster to use to send the
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
static QCLI_Command_Status_t cmd_ZCL_IASACE_GetPanelStatus(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   uint8_t                         DeviceId;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      /* Validate the device ID. */
      if((Parameter_Count >= 2) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ACE, ZCL_DEMO_CLUSTERTYPE_CLIENT);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_IASACE_Client_Send_Get_Panel_Status(ClusterInfo->Handle, &SendInfo);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Client_Send_Get_Panel_Status");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Client_Send_Get_Panel_Status", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "GetBypassedZoneList" command to send Get Bypassed Zone
          list command to an IAS ACE server.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the IAS ACE client cluster to use to send the
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
static QCLI_Command_Status_t cmd_ZCL_IASACE_GetBypassedZoneList(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   uint8_t                         DeviceId;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      /* Validate the device ID. */
      if((Parameter_Count >= 2) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ACE, ZCL_DEMO_CLUSTERTYPE_CLIENT);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));
            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_IASACE_Client_Send_Get_Bypassed_Zone_List(ClusterInfo->Handle, &SendInfo);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Client_Send_Get_Bypassed_Zone_List");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Client_Send_Get_Bypassed_Zone_List", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "GetZoneStatus" command to send Get Zone Status command
          to an IAS ACE server.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the IAS ACE client cluster to use to send the
                     command.
   Parameter_List[2] is the starting zone ID.
   Parameter_List[3] is the number of zone ID.
   Parameter_List[4] is the ZoneStatusMask.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_IASACE_GetZoneStatus(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t                Ret_Val;
   qapi_Status_t                        Result;
   ZCL_Demo_Cluster_Info_t             *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t       SendInfo;
   uint8_t                              DeviceId;
   qapi_ZB_CL_IASACE_Get_Zone_Status_t  Request;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      /* Validate the device ID. */
      if((Parameter_Count >= 5) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[4]), 0, 0xFFFF)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ACE, ZCL_DEMO_CLUSTERTYPE_CLIENT);

         memset(&Request, 0, sizeof(Request));
         Request.StartingZoneID     = (uint8_t)(Parameter_List[2].Integer_Value);
         Request.MaxNumberOfZones   = (uint8_t)(Parameter_List[3].Integer_Value);
         Request.ZoneStatusMaskFlag = (qbool_t)(Parameter_List[4].Integer_Value != 0);
         Request.ZoneStatusMask     = (uint16_t)(Parameter_List[4].Integer_Value);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));
            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_IASACE_Client_Send_Get_Zone_Status(ClusterInfo->Handle, &SendInfo, &Request);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Client_Send_Get_Zone_Status");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Client_Send_Get_Zone_Status", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SetServerArmCode" command to set the arm/disarm code on
          a IAS ACE server.

   Parameter_List[0] Endpoint of the IAS ACE server cluster.
   Parameter_List[1] The arm/disarm code to be set.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_IASACE_SetServerArmCode(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t            Result;
   QCLI_Command_Status_t    Ret_Val;
   ZCL_Demo_Cluster_Info_t *ClusterInfo;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 2) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[0].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ACE, ZCL_DEMO_CLUSTERTYPE_SERVER);
         if(ClusterInfo != NULL)
         {
            Result = qapi_ZB_CL_IASACE_Server_Set_Arm_Code(ClusterInfo->Handle, (uint8_t *)(Parameter_List[1].String_Value));
            if(Result == QAPI_OK)
            {
               Display_Function_Success(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Server_Set_Arm_Code");
               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Server_Set_Arm_Code", Result);
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "GetArmCode" command to set the arm/disarm code from a
          IAS ACE server.

   Parameter_List[0] Endpoint of the IAS ACE server cluster.
   Parameter_List[1] The arm/disarm code to be set.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_IASACE_GetServerArmCode(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t            Result;
   QCLI_Command_Status_t    Ret_Val;
   ZCL_Demo_Cluster_Info_t *ClusterInfo;
   uint8_t                  ArmCode[QAPI_ZB_CL_IASACE_ARM_CODE_MAX_LENGTH + 1];

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[0].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ACE, ZCL_DEMO_CLUSTERTYPE_SERVER);
         if(ClusterInfo != NULL)
         {
            Result = qapi_ZB_CL_IASACE_Server_Get_Arm_Code(ClusterInfo->Handle, sizeof(ArmCode), ArmCode);
            if(Result == QAPI_OK)
            {
               QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Server arm code: %s\n", ArmCode);
               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Server_Get_Arm_Code", Result);
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "AddZone" command to add a zone table entry to the zone
          table on an IAS ACE server.

   Parameter_List[0] Endpoint of the IAS ACE server cluster.
   Parameter_List[1] The zone ID.
   Parameter_List[2] The zone type.
   Parameter_List[3] The IEEE address of the zone.
   Parameter_List[4] The zone label.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_IASACE_AddZone(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t                 Ret_Val;
   qapi_Status_t                         Result;
   ZCL_Demo_Cluster_Info_t              *ClusterInfo;
   qapi_ZB_CL_IASACE_Zone_Table_Entry_t  Request;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      memset(&Request, 0, sizeof(qapi_ZB_CL_IASACE_Zone_Table_Entry_t));

      if((Parameter_Count >= 5) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), QAPI_ZB_CL_IASZONE_ZONE_TYPE_STANDARD_CIE_E, QAPI_ZB_CL_IASZONE_ZONE_TYPE_SECURITY_REPEATER)) &&
         (Hex_String_To_ULL(Parameter_List[3].String_Value, &Request.ZoneAddress)))

      {
         ClusterInfo         = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[0].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ACE, ZCL_DEMO_CLUSTERTYPE_SERVER);
         Request.ZoneID      = (uint8_t)(Parameter_List[1].Integer_Value);
         Request.ZoneType    = (qapi_ZB_CL_IASZone_Zone_Type_t)(Parameter_List[2].Integer_Value);
         QSOSAL_StringCopy_S((char *)(Request.ZoneLabel), QAPI_ZB_CL_IASACE_ZONE_LABEL_MAX_LENGTH + 1, Parameter_List[4].String_Value);

         if(ClusterInfo != NULL)
         {
            Result = qapi_ZB_CL_IASACE_Server_Add_Zone(ClusterInfo->Handle, &Request);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Server_Add_Zone");
               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Ret_Val = QCLI_STATUS_ERROR_E;
               Display_Function_Error(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Server_Add_Zone", Result);
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "DeleteZone" command to delete a zone table entry from
          the zone table on an IAS ACE server.

   Parameter_List[0] Endpoint of the IAS ACE server cluster.
   Parameter_List[1] The ID of the zone.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_IASACE_DeleteZone(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t            Result;
   QCLI_Command_Status_t    Ret_Val;
   ZCL_Demo_Cluster_Info_t *ClusterInfo;
   uint8_t                  ZoneID;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 2) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 0xFF)))
      {
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[0].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ACE, ZCL_DEMO_CLUSTERTYPE_SERVER);
         ZoneID      = (uint8_t)(Parameter_List[1].Integer_Value);

         if(ClusterInfo != NULL)
         {
            Result = qapi_ZB_CL_IASACE_Server_Delete_Zone(ClusterInfo->Handle, ZoneID);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Server_Delete_Zone");
               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Server_Delete_Zone", Result);
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "ListZones" command to list all zones on the server.

   Parameter_List[0] Endpoint of the IAS ACE server cluster.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_IASACE_ListZones(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t                         Result;
   QCLI_Command_Status_t                 Ret_Val;
   ZCL_Demo_Cluster_Info_t              *ClusterInfo;
   uint32_t                              Index;
   qapi_ZB_CL_IASACE_Zone_Table_Entry_t  ZoneInfo;
   uint16_t                              ZoneStatus;
   qbool_t                               ZoneFound;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[0].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ACE, ZCL_DEMO_CLUSTERTYPE_SERVER);

         if(ClusterInfo != NULL)
         {
            ZoneFound = false;

            for(Index = 0; Index <= 0xFF; Index ++)
            {
               Result = qapi_ZB_CL_IASACE_Server_Find_Zone_By_ID(ClusterInfo->Handle, (uint8_t)Index, &ZoneInfo, &ZoneStatus);
               if(Result == QAPI_OK)
               {
                  if(!ZoneFound)
                  {
                     /* Display the header only after a zone is found. */
                     QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, " ID  | Address          | Type   | Status | Label\n");
                     QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "-----+------------------+--------+--------+-------\n");
                     ZoneFound = true;
                  }

                  QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, " %3d | %08X%08X | 0x%04X | 0x%04X | %s\n", ZoneInfo.ZoneID, (uint32_t)(ZoneInfo.ZoneAddress >> 32), (uint32_t)(ZoneInfo.ZoneAddress), ZoneInfo.ZoneType, ZoneStatus, ZoneInfo.ZoneLabel);
               }
            }

            if(!ZoneFound)
            {
               QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "No zones found.\n");
            }

            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }
         else
         {
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SetZoneStatus" command to set zone status on IAS ACE
          server.

   Parameter_List[0] Endpoint of the IAS ACE server cluster.
   Parameter_List[1] ID of the zone whose status is going to be set.
   Parameter_List[2] Status of the zone being set.
   Parameter_List[3] Audible notificatin being set.
   Parameter_List[4] Alarm status.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_IASACE_SetZoneStatus(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t                     Result;
   QCLI_Command_Status_t             Ret_Val;
   ZCL_Demo_Cluster_Info_t          *ClusterInfo;
   uint8_t                           ZoneID;
   uint16_t                          ZoneStatus;
   uint8_t                           AudibleNotification;
   qapi_ZB_CL_IASACE_Alarm_Status_t  AlarmStatus;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      /* Validate the device ID. */
      if((Parameter_Count >= 5) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[4]), 0, 0xFF)))
      {
         ClusterInfo         = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[0].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ACE, ZCL_DEMO_CLUSTERTYPE_SERVER);
         ZoneID              = (uint8_t)(Parameter_List[1].Integer_Value);
         ZoneStatus          = (uint16_t)(Parameter_List[2].Integer_Value);
         AudibleNotification = (uint8_t)(Parameter_List[3].Integer_Value);
         AlarmStatus         = (qapi_ZB_CL_IASACE_Alarm_Status_t)(Parameter_List[4].Integer_Value);

         if(ClusterInfo != NULL)
         {
            Result = qapi_ZB_CL_IASACE_Server_Set_Zone_Status(ClusterInfo->Handle, ZoneID, ZoneStatus, AudibleNotification, AlarmStatus);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Server_Set_Zone_Status");
               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Server_Set_Zone_Status", Result);
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SetPanelStatus" command to set the panel status on IAS
          ACE server.

   Parameter_List[0] Endpoint of the IAS ACE server cluster.
   Parameter_List[1] Panel status to be set.
   Parameter_List[2] Seconds remaining for the the panel status.
   Parameter_List[3] Audible notification for the panel status.
   Parameter_List[4] Alarm status.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_IASACE_SetLocalPanelStatus(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t                          Result;
   QCLI_Command_Status_t                  Ret_Val;
   ZCL_Demo_Cluster_Info_t               *ClusterInfo;
   qapi_ZB_CL_IASACE_Panel_Status_Info_t  PanelStatusInfo;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      /* Validate the device ID. */
      if((Parameter_Count >= 5) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_CL_IASACE_PANEL_STATUS_READY_TO_ARM_E, QAPI_ZB_CL_IASACE_PANEL_STATUS_ARMING_AWAY_E)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[4]), QAPI_ZB_CL_IASACE_ALARM_STATUS_NO_ALARM_E, QAPI_ZB_CL_IASACE_ALARM_STATUS_EMERGENCY_PANIC_E)))
      {
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[0].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ACE, ZCL_DEMO_CLUSTERTYPE_SERVER);
         if(ClusterInfo != NULL)
         {
            memset(&PanelStatusInfo, 0, sizeof(PanelStatusInfo));

            PanelStatusInfo.PanelStatus           = (qapi_ZB_CL_IASACE_Panel_Status_t)(Parameter_List[1].Integer_Value);
            PanelStatusInfo.SecondsRemaining      = (uint8_t)(Parameter_List[2].Integer_Value);
            PanelStatusInfo.AudibleNotification   = (uint8_t)(Parameter_List[3].Integer_Value);
            PanelStatusInfo.AlarmStatus           = (qapi_ZB_CL_IASACE_Alarm_Status_t)(Parameter_List[4].Integer_Value);

            Result = qapi_ZB_CL_IASACE_Server_Set_Panel_Status(ClusterInfo->Handle, &PanelStatusInfo);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Server_Set_Panel_Status");
               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Server_Set_Panel_Status", Result);
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "GetPanelStatus" command to Get the panel status from a
          IAS ACE server.

   Parameter_List[0] Endpoint of the IAS ACE server cluster.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_IASACE_GetLocalPanelStatus(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t                          Result;
   QCLI_Command_Status_t                  Ret_Val;
   ZCL_Demo_Cluster_Info_t               *ClusterInfo;
   qapi_ZB_CL_IASACE_Panel_Status_Info_t  PanelStatusInfo;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      /* Validate the device ID. */
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[0].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ACE, ZCL_DEMO_CLUSTERTYPE_SERVER);
         if(ClusterInfo != NULL)
         {
            Result = qapi_ZB_CL_IASACE_Server_Get_Panel_Status(ClusterInfo->Handle, &PanelStatusInfo);

            if(Result == QAPI_OK)
            {
               QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Panel Status:\n");
               QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  PanelStatus:         %d\n", (uint32_t)(PanelStatusInfo.PanelStatus));
               QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  AudibleNotification: %d\n", PanelStatusInfo.AudibleNotification);

               if((PanelStatusInfo.PanelStatus == QAPI_ZB_CL_IASACE_PANEL_STATUS_EXIT_DELAY_E) ||
                  (PanelStatusInfo.PanelStatus == QAPI_ZB_CL_IASACE_PANEL_STATUS_ENTRY_DELAY_E))
               {
                  QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  SecondsRemaining:    %d\n", PanelStatusInfo.SecondsRemaining);
               }
               else if(PanelStatusInfo.PanelStatus == QAPI_ZB_CL_IASACE_PANEL_STATUS_IN_ALARM_E)
               {
                  QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  AlarmStatus:         %d\n", (uint32_t)(PanelStatusInfo.AlarmStatus));
               }

               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Server_Get_Panel_Status", Result);
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SetBypssPermit" command to set the bypass permit on the
         IAS ACE server.

   Parameter_List[0] Endpoint of the IAS ACE server cluster.
   Parameter_List[1] ID of the zone whose status is going to be set.
   Parameter_List[2] Flag of the bypass permit.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_IASACE_SetBypssPermit(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t                    Result;
   QCLI_Command_Status_t            Ret_Val;
   ZCL_Demo_Cluster_Info_t         *ClusterInfo;
   uint8_t                          ZoneID;
   qbool_t                          Permit;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      /* Validate the device ID. */
      if((Parameter_Count >= 3) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 1)))
      {
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[0].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ACE, ZCL_DEMO_CLUSTERTYPE_SERVER);
         ZoneID      = (uint8_t)(Parameter_List[1].Integer_Value);
         Permit      = (qbool_t)(Parameter_List[2].Integer_Value != 0);

         if(ClusterInfo != NULL)
         {
            Result = qapi_ZB_CL_IASACE_Server_Set_Bypass_Permit(ClusterInfo->Handle, ZoneID, Permit);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Server_Set_Bypass_Permit");
               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Server_Set_Bypass_Permit", Result);
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SetBypssStatus" command to set the bypass permit on the
         IAS ACE server.

   Parameter_List[0] Endpoint of the IAS ACE server cluster.
   Parameter_List[1] ID of the zone whose status is going to be set.
   Parameter_List[2] Flag of the bypass status.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_IASACE_SetBypssStatus(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t                      Result;
   QCLI_Command_Status_t              Ret_Val;
   ZCL_Demo_Cluster_Info_t           *ClusterInfo;
   uint8_t                            ZoneID;
   qbool_t                            BypassStatus;
   qapi_ZB_CL_IASACE_Bypass_Result_t  BypassResult;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      /* Validate the device ID. */
      if((Parameter_Count >= 3) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 1)))
      {
         ClusterInfo  = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[0].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ACE, ZCL_DEMO_CLUSTERTYPE_SERVER);
         ZoneID       = (uint8_t)(Parameter_List[1].Integer_Value);
         BypassStatus = (qbool_t)(Parameter_List[2].Integer_Value != 0);
         if(ClusterInfo != NULL)
         {
            Result = qapi_ZB_CL_IASACE_Server_Set_Bypass(ClusterInfo->Handle, ZoneID, BypassStatus, &BypassResult);
            if(Result == QAPI_OK)
            {
               QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Bypass Result: %d.\n", BypassResult);
               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Server_Set_Bypass_Status", Result);
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

static QCLI_Command_Status_t cmd_ZCL_IASACE_GetZoneList(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t            Result;
   QCLI_Command_Status_t    Ret_Val;
   ZCL_Demo_Cluster_Info_t *ClusterInfo;
   uint8_t                  ZoneList[IASACE_DEMO_MAX_ZONE_LIST_SIZE];
   uint8_t                  ListSize;
   uint32_t                 Index;

   /* Ensure both the stack is initialized and the switch endpoint. */
   if(GetZigBeeHandle() != NULL)
   {
      /* Validate the device ID. */
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         ClusterInfo  = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[0].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_IAS_ACE, ZCL_DEMO_CLUSTERTYPE_SERVER);
         if(ClusterInfo != NULL)
         {
            ListSize = IASACE_DEMO_MAX_ZONE_LIST_SIZE;
            Result = qapi_ZB_CL_IASACE_Server_Get_Zone_List(ClusterInfo->Handle, &ListSize, ZoneList);
            if(Result == QAPI_OK)
            {
               QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Zone List (%d entries)", ListSize);
               if(ListSize > 0)
               {
                  QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, ": %d", ZoneList[0]);

                  for(Index = 1; Index < ListSize; Index++)
                  {
                     QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, ", %d", ZoneList[Index]);
                  }
               }

               QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "\n");

               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else if(Result == QAPI_ERR_BOUNDS)
            {
               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Server_Set_Bypass_Status", Result);
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Handles callbacks for the IAS ACE server cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_IASACE_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_IASACE_Server_Event_Data_t *EventData, uint32_t CB_Param)
{
   uint8_t       Index;
   qapi_Status_t Result;

   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      switch(EventData->Event_Type)
      {
         case QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_ARM_E:
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "IASACE Server Arm:\n");
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  ZoneID:  %d\n", EventData->Data.Arm.ZoneID);
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  ArmMode: %d\n", EventData->Data.Arm.ArmMode);
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  ArmCode: %s\n", EventData->Data.Arm.ArmCode);

            switch(EventData->Data.Arm.ArmMode)
            {
               case QAPI_ZB_CL_IASACE_ARM_MODE_DISARM_E:
                  *(EventData->Data.Arm.ArmResponse) = QAPI_ZB_CL_IASACE_ARM_NOTIFICATION_ALL_ZONES_DISARMED_E;
                  break;

               case QAPI_ZB_CL_IASACE_ARM_MODE_ARM_DAY_ZONES_ONLY_E:
                  *(EventData->Data.Arm.ArmResponse) = QAPI_ZB_CL_IASACE_ARM_NOTIFICATION_ONLY_DAY_ZONES_ARMED_E;
                  break;

               case QAPI_ZB_CL_IASACE_ARM_MODE_ARM_NIGHT_ZONES_ONLY_E:
                  *(EventData->Data.Arm.ArmResponse) = QAPI_ZB_CL_IASACE_ARM_NOTIFICATION_ONLY_NIGHT_ZONES_ARMED_E;
                  break;

               case QAPI_ZB_CL_IASACE_ARM_MODE_ARM_ALL_ZONES_E:
                  *(EventData->Data.Arm.ArmResponse) = QAPI_ZB_CL_IASACE_ARM_NOTIFICATION_ALL_ZONES_ARMED_E;
                  break;

               default:
                  break;
            }
            break;

         case QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_BYPASS_E:
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "IASACE Server Bypass:\n");
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  NumOfZones: %d\n", EventData->Data.Bypass.NumberOfZones);
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  ArmCode:    %s\n", EventData->Data.Bypass.ArmCode);
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  ZoneIDs: ");

            for(Index = 0; Index < EventData->Data.Bypass.NumberOfZones; Index++)
            {
               QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "%d ", EventData->Data.Bypass.ZoneIDList[Index]);

               Result = qapi_ZB_CL_IASACE_Server_Set_Bypass(Cluster, EventData->Data.Bypass.ZoneIDList[Index], true, &(EventData->Data.Bypass.BypassResultList[Index]));
               if(Result != QAPI_OK)
               {
                  EventData->Data.Bypass.BypassResultList[Index] = QAPI_ZB_CL_IASACE_BYPASS_RESULT_NOT_ALLOWED_E;
               }
            }
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "\n");
            break;

         case QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_EMERGENCY_E:
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "IASACE Server Emergency.\n");
            break;

         case QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_FIRE_E:
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "IASACE Server Fire.\n");
            break;

         case QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_PANIC_E:
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "IASACE Server Panic.\n");
            break;

         case QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_DELAY_TIMEOUT_E:
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "IASACE Server Delay Timeout:\n");
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "   PanelStatus: %d\n", EventData->Data.Delay_Timeout.PanelStatus);
            break;

         default:
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Unhandled IASACE Server Event %d.\n", EventData->Event_Type);
            break;
      }

      QCLI_Display_Prompt();
   }
}

/**
   @brief Handles callbacks for the IASACE client cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_IASACE_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_IASACE_Client_Event_Data_t *EventData, uint32_t CB_Param)
{
   uint8_t Index;

   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      switch(EventData->Event_Type)
      {
         case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E:
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "IASACE Client Default Response:\n");
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  Status:        %d\n", EventData->Data.Default_Response.Status);
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  CommandID:     0x%02X\n", EventData->Data.Default_Response.CommandId);
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  CommandStatus: %d\n", EventData->Data.Default_Response.CommandStatus);
            break;

         case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E:
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Command Complete:\n");
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  CommandStatus: %d\n", EventData->Data.Command_Complete.CommandStatus);
            break;

         case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_ARM_RESPONSE_E:
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "IASACE Client Arm Response\n");
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  ArmNotification: %d\n", EventData->Data.Arm_Response.ArmNotification);
            break;

         case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_BYPASS_RESPONSE_E:
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "IASACE Client Bypass Response:\n");
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  NumofZones:   %d\n", EventData->Data.Bypass_Response.NumberOfZones);
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  BypassResult:");
            for(Index = 0; Index < EventData->Data.Bypass_Response.NumberOfZones; Index++)
            {
               QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, " %d ", EventData->Data.Bypass_Response.BypassResultList[Index]);
            }
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "\n");
            break;

         case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_GET_ZONE_ID_MAP_RESPONSE_E:
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "IASACE Client Get Zone ID Map Response:\n");
            for(Index = 0; Index < QAPI_ZB_CL_IASACE_ZONE_ID_MAP_RESPONSE_SIZE; Index++)
            {
               QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, " 0x%04X", EventData->Data.Get_Zone_ID_Map_Response.ZoneIDMap[Index]);
            }
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "\n");
            break;

         case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_GET_ZONE_INFORMATION_RESPONSE_E:
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "IASACE Client Get Zone Info Response:\n");
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  ZoneID:    %d\n", EventData->Data.Get_Zone_Info_Response.ZoneID);
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  ZoneType:  %d\n", EventData->Data.Get_Zone_Info_Response.ZoneType);
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  ZoneAddr:  %08X%08X\n", (uint32_t)(EventData->Data.Get_Zone_Info_Response.ZoneAddress >> 32), (uint32_t)(EventData->Data.Get_Zone_Info_Response.ZoneAddress));
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  ZoneLabel: %s\n", EventData->Data.Get_Zone_Info_Response.ZoneLabel);
            break;

         case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_GET_PANEL_STATUS_RESPONSE_E:
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "IASACE Client Get Panel Status Response:\n");
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  PanelStatus:      %d\n", EventData->Data.Get_Panel_Status_Response.PanelStatus);
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  SecondsRemaining: %d\n", EventData->Data.Get_Panel_Status_Response.SecondsRemaining);
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  Notification:     %d\n", EventData->Data.Get_Panel_Status_Response.AudibleNotification);
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  AlarmStatus:      %d\n", EventData->Data.Get_Panel_Status_Response.AlarmStatus);
            break;

         case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_PANEL_STATUS_CHANGED_E:
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "IASACE Client Panel Status Changed:\n");
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  PanelStatus:      %d\n", EventData->Data.Get_Panel_Status_Response.PanelStatus);
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  SecondsRemaining: %d\n", EventData->Data.Get_Panel_Status_Response.SecondsRemaining);
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  Notification:     %d\n", EventData->Data.Get_Panel_Status_Response.AudibleNotification);
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  AlarmStatus:      %d\n", EventData->Data.Get_Panel_Status_Response.AlarmStatus);
            break;

         case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_SET_BYPASSED_ZONE_LIST_E:
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "IASACE Client Set Bypassed Zone List:\n");
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  NumofZones: %d\n", EventData->Data.Set_Bypassed_Zone_List.NumberOfZones);
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  ZoneID:");
            for(Index = 0; Index < EventData->Data.Set_Bypassed_Zone_List.NumberOfZones; Index++)
            {
               QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, " %d", EventData->Data.Set_Bypassed_Zone_List.ZoneIDList[Index]);
            }
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "\n");
            break;

         case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_GET_ZONE_STATUS_RESPONSE_E:
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "IASACE Client Get Zone Status Response:\n");
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  ZoneStatusComplete: %s\n", (EventData->Data.Get_Zone_Status_Response.ZoneStatusComplete) ? "true" : "false");
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  NumberOfZones:      %d\n", EventData->Data.Get_Zone_Status_Response.NumberOfZones);
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  ZoneStatus:");
            for(Index = 0; Index < EventData->Data.Get_Zone_Status_Response.NumberOfZones; Index++)
            {
               QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, " 0x%04X", EventData->Data.Get_Zone_Status_Response.ZoneStatusList[Index].Status);
            }
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "\n");
            break;

         case QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_ZONE_STATUS_CHANGED_E:
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "IASACE Client Get Zone Status Changed:\n");
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  ZoneID:       %d\n", EventData->Data.Zone_Status_Changed.ZoneID);
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  Status:       0x%04X\n", EventData->Data.Zone_Status_Changed.Status);
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  Notification: %d\n", EventData->Data.Zone_Status_Changed.AudibleNotification);
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "  ZoneLabel:    %s\n", EventData->Data.Zone_Status_Changed.ZoneLabel);
            break;

         default:
            QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "Unhandled IASACE client event %d.\n", EventData->Event_Type);
            break;
      }

      QCLI_Display_Prompt();
   }
}

/**
   @brief Initializes the ZCL IASACE demo.

   @param ZigBee_QCLI_Handle is the parent QCLI handle for the IASACE demo.

   @return true if the ZigBee light demo initialized successfully, false
           otherwise.
*/
qbool_t Initialize_ZCL_IASACE_Demo(QCLI_Group_Handle_t ZigBee_QCLI_Handle)
{
   qbool_t Ret_Val;

   /* Register IASACE command group. */
   ZigBee_IASACE_Demo_Context.QCLI_Handle = QCLI_Register_Command_Group(ZigBee_QCLI_Handle, &ZCL_IASACE_Cmd_Group);
   if(ZigBee_IASACE_Demo_Context.QCLI_Handle != NULL)
   {
      Ret_Val = true;
   }
   else
   {
      QCLI_Printf(ZigBee_QCLI_Handle, "Failed to register ZCL IASACE command group.\n");
      Ret_Val = false;
   }

   return(Ret_Val);
}

/**
   @brief Creates an IASACE server cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_IASACE_Demo_Create_Server(uint8_t Endpoint, void **PrivData)
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

      Result = qapi_ZB_CL_IASACE_Create_Server(ZigBee_Handle, &Ret_Val, &ClusterInfo, ZCL_IASACE_Demo_Server_CB, 0);
      if(Result != QAPI_OK)
      {
         Display_Function_Error(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASZone_Create_Server", Result);
         Ret_Val = NULL;
      }
   }
   else
   {
      QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

/**
   @brief Creates an IASACE client cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_IASACE_Demo_Create_Client(uint8_t Endpoint, void **PrivData)
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

      Result = qapi_ZB_CL_IASACE_Create_Client(ZigBee_Handle, &Ret_Val, &ClusterInfo, ZCL_IASACE_Demo_Client_CB, 0);
      if(Result != QAPI_OK)
      {
         Display_Function_Error(ZigBee_IASACE_Demo_Context.QCLI_Handle, "qapi_ZB_CL_IASACE_Create_Client", Result);
         Ret_Val = NULL;
      }
   }
   else
   {
      QCLI_Printf(ZigBee_IASACE_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

