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
#include "zcl_colorcontrol_demo.h"
#include "zcl_scenes_demo.h"

#include "qapi_zb.h"
#include "qapi_zb_cl.h"
#include "qapi_zb_cl_color_control.h"
#include "qsOSAL.h"

/* Capabilites used to initialize the color control server. */
#define COLOR_CONTROL_SERVER_CAPABILITIES                               (QAPI_ZB_CL_COLORCONTROL_CAPABILITY_HUE_SATURATION_SUPPORTED | \
                                                                         QAPI_ZB_CL_COLORCONTROL_CAPABILITY_ENHANCED_HUE_SUPPORTED |   \
                                                                         QAPI_ZB_CL_COLORCONTROL_CAPABILITY_COLOR_LOOP_SUPPORTED |     \
                                                                         QAPI_ZB_CL_COLORCONTROL_CAPABILITY_X_Y_SUPPORTED |            \
                                                                         QAPI_ZB_CL_COLORCONTROL_CAPABILITY_COLOR_TEMP_SUPPORTED)

/* Structure representing the ZigBee light demo context information. */
typedef struct ZCL_ColorControl_Demo_Data_s
{
   uint8_t  CurrentHue;
   uint8_t  CurrentSaturation;
   uint16_t EnhancedCurrentHue;
   uint16_t CurrentX;
   uint16_t CurrentY;
   uint16_t ColorTempMireds;
} ZCL_ColorControl_Demo_Data_t;

/* Structure representing the ZigBee color contorl demo context information. */
typedef struct ZigBee_ColorControl_Demo_Context_s
{
   QCLI_Group_Handle_t QCLI_Handle;     /*< QCLI handle for the main ZigBee demo. */
   uint8_t             SceneData[11];   /*< The color control scene data.*/
} ZigBee_ColorControl_Demo_Context_t;

/* The ZigBee color contorl demo context. */
static ZigBee_ColorControl_Demo_Context_t ZigBee_ColorControl_Demo_Context;

static const ZCL_ColorControl_Demo_Data_t ColorDefaults =
{
   0x00,   /* CurrentHue */
   0x00,   /* CurrentSaturation */
   0x0000, /* EnhancedCurrentHue */
   0x616B, /* CurrentX */
   0x607D, /* CurrentY */
   0x00FA, /* ColorTempMireds */
};

/* List of attributes overwritten by the demo. These are the attributes that the
   demo handles the attribute values. */
static const qapi_ZB_CL_Attribute_t ColorControlDemoCustomAttrList[] =
{
   {QAPI_ZB_CL_COLORCONTROL_ATTR_ID_CURRENT_HUE,          QAPI_ZB_CL_ATTRIBUTE_FLAG_REPORTABLE | QAPI_ZB_CL_ATTRIBUTE_FLAG_CUSTOM_READ_WRITE, QAPI_ZB_CL_DATA_TYPE_UNSIGNED_8BIT_INTEGER_E,  sizeof(uint8_t),  0, 0, 0, 0xFE},
   {QAPI_ZB_CL_COLORCONTROL_ATTR_ID_CURRENT_SATURATION,   QAPI_ZB_CL_ATTRIBUTE_FLAG_REPORTABLE | QAPI_ZB_CL_ATTRIBUTE_FLAG_CUSTOM_READ_WRITE, QAPI_ZB_CL_DATA_TYPE_UNSIGNED_8BIT_INTEGER_E,  sizeof(uint8_t),  0, 0, 0, 0xFE},
   {QAPI_ZB_CL_COLORCONTROL_ATTR_ID_ENHANCED_CURRENT_HUE, QAPI_ZB_CL_ATTRIBUTE_FLAG_REPORTABLE | QAPI_ZB_CL_ATTRIBUTE_FLAG_CUSTOM_READ_WRITE, QAPI_ZB_CL_DATA_TYPE_UNSIGNED_16BIT_INTEGER_E, sizeof(uint16_t), 0, 0, 0, 0xFFFF},
   {QAPI_ZB_CL_COLORCONTROL_ATTR_ID_CURRENT_X,            QAPI_ZB_CL_ATTRIBUTE_FLAG_REPORTABLE | QAPI_ZB_CL_ATTRIBUTE_FLAG_CUSTOM_READ_WRITE, QAPI_ZB_CL_DATA_TYPE_UNSIGNED_16BIT_INTEGER_E, sizeof(uint16_t), 0, 0, 0, 0xFEFF},
   {QAPI_ZB_CL_COLORCONTROL_ATTR_ID_CURRENT_Y,            QAPI_ZB_CL_ATTRIBUTE_FLAG_REPORTABLE | QAPI_ZB_CL_ATTRIBUTE_FLAG_CUSTOM_READ_WRITE, QAPI_ZB_CL_DATA_TYPE_UNSIGNED_16BIT_INTEGER_E, sizeof(uint16_t), 0, 0, 0, 0xFEFF},
   {QAPI_ZB_CL_COLORCONTROL_ATTR_ID_COLOR_TEMP_MIREDS,    QAPI_ZB_CL_ATTRIBUTE_FLAG_REPORTABLE | QAPI_ZB_CL_ATTRIBUTE_FLAG_CUSTOM_READ_WRITE, QAPI_ZB_CL_DATA_TYPE_UNSIGNED_16BIT_INTEGER_E, sizeof(uint16_t), 0, 0, 0, 0xFEFF}
};

#define COLOR_CONTROL_DEMO_CUSTOM_ATTR_LIST_SIZE                        (sizeof(ColorControlDemoCustomAttrList) / sizeof(qapi_ZB_CL_Attribute_t))

static const uint8_t DefaultSceneData[11] = {0x6B, 0x61, 0x7D, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* Function prototypes. */
static QCLI_Command_Status_t cmd_ZCL_ColorControl_MoveToHue(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_ColorControl_MoveHue(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_ColorControl_StepHue(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_ColorControl_MoveToSaturation(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_ColorControl_MoveSaturation(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_ColorControl_StepSaturation(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_ColorControl_MoveToHueAndSaturation(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_ColorControl_MoveToColor(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_ColorControl_MoveColor(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_ColorControl_StepColor(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_ColorControl_MoveToColorTemp(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_ColorControl_MoveColorTemp(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_ColorControl_StepColorTemp(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_ColorControl_StopMoveStep(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_ColorControl_ColorLoopSet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_ColorControl_SetSceneData(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static void ZCL_ColorControl_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_ColorControl_Server_Event_Data_t *EventData, uint32_t CB_Param);
static void ZCL_ColorControl_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_ColorControl_Client_Event_Data_t *EventData, uint32_t CB_Param);

/* Command list for the ZigBee light demo. */
const QCLI_Command_t ZigBee_ColorControl_CMD_List[] =
{
   /* cmd_function                               thread  cmd_string                usage_string                                                                                      description */
   {cmd_ZCL_ColorControl_MoveToHue,              false,  "MoveToHue",              "[DevId][ClientEndpoint][Direction(0-3)][Hue][Time][Normal=0,Enhanced=1]",                        "Sends a Move To Hue command to a Color Control server."},
   {cmd_ZCL_ColorControl_MoveHue,                false,  "MoveHue",                "[DevId][ClientEndpoint][Mode(2-4)][Rate][Normal=0,Enhanced=1]",                                  "Sends a Move Hue command to a Color Control server."},
   {cmd_ZCL_ColorControl_StepHue,                false,  "StepHue",                "[DevId][ClientEndpoint][Mode(2-3)][StepSize][Time][Normal=0,Enhanced=1]",                        "Sends a Step Hue command to a Color Control server."},
   {cmd_ZCL_ColorControl_MoveToSaturation,       false,  "MoveToSaturation",       "[DevId][ClientEndpoint][Saturation][Time]",                                                      "Sends a Move To Saturation command to a Color Control server."},
   {cmd_ZCL_ColorControl_MoveSaturation,         false,  "MoveSaturation",         "[DevId][ClientEndpoint][Mode(2-4)][Rate]",                                                       "Sends a Move Saturation command to a Color Control server."},
   {cmd_ZCL_ColorControl_StepSaturation,         false,  "StepSaturation",         "[DevId][ClientEndpoint][Mode(2-3)][StepSize][Time]",                                             "Sends a Step Saturation command to a Color Control server."},
   {cmd_ZCL_ColorControl_MoveToHueAndSaturation, false,  "MoveToHueAndSaturation", "[DevId][ClientEndpoint][Hue][Saturation][Time][Normal=0,Enhanced=1]",                            "Sends a Move To Hue And Saturation command to a Color Control server."},
   {cmd_ZCL_ColorControl_MoveToColor,            false,  "MoveToColor",            "[DevId][ClientEndpoint][ColorX][ColorY][Time]",                                                  "Sends a Move T oColor command to a Color Control server."},
   {cmd_ZCL_ColorControl_MoveColor,              false,  "MoveColor",              "[DevId][ClientEndpoint][RateX][RateY]",                                                          "Sends a Move Color command to a Color Control server."},
   {cmd_ZCL_ColorControl_StepColor,              false,  "StepColor",              "[DevId][ClientEndpoint][StepX][StepY][Time]",                                                    "Sends a Step Color command to a Color Control server."},
   {cmd_ZCL_ColorControl_MoveToColorTemp,        false,  "MoveToColorTemp",        "[DevId][ClientEndpoint][ColorTempMireds][Time]",                                                 "Sends a Move To Color Temp command to a Color Control server."},
   {cmd_ZCL_ColorControl_MoveColorTemp,          false,  "MoveColorTemp",          "[DevId][ClientEndpoint][Mode(2-4)][Rate][Limit]",                                                "Sends a Move Color Temp command to a Color Control server."},
   {cmd_ZCL_ColorControl_StepColorTemp,          false,  "StepColorTemp",          "[DevId][ClientEndpoint][Mode(2-3)][StepSize][Time][Limit]",                                      "Sends a Step Color Temp command to a Color Control server."},
   {cmd_ZCL_ColorControl_StopMoveStep,           false,  "StopMoveStep",           "[DevId][ClientEndpoint]",                                                                        "Sends a Stop Move Step command to a Color Control server."},
   {cmd_ZCL_ColorControl_ColorLoopSet,           false,  "ColorLoopSet",           "[DevId][ClientEndpoint][Flag][Action(0-2)][Direction(2-3)][Time][StartHue]",                     "Sends a Color Loop Set command to a Color Control server."},
   {cmd_ZCL_ColorControl_SetSceneData,           false,  "SetSceneData",           "[CurrentX][CurrentY][EnhancedCuHue][CurrentSa][ColorLoopActive][ColorLoopDirec][ColorLoopTime]", "Set the color control scene data."}
};

const QCLI_Command_Group_t ZCL_ColorControl_Cmd_Group = {"ColorControl", sizeof(ZigBee_ColorControl_CMD_List) / sizeof(QCLI_Command_t), ZigBee_ColorControl_CMD_List};

/**
   @brief Executes the "SendMoveToHue" command.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the Color Control client cluster to use to send
                     the command.
   Parameter_List[2] is the hue that is moved to.
   Parameter_List[3] is the direction of the move.
   Parameter_List[4] is the transition time.
   Parameter_List[5] is a flag indicating if this is an enhanced hue command
                     (1) or a normal one (0).

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_ColorControl_MoveToHue(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t                Ret_Val;
   qapi_Status_t                        Result;
   ZCL_Demo_Cluster_Info_t             *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t       SendInfo;
   uint32_t                             DeviceId;
   uint16_t                             Hue;
   qapi_ZB_CL_ColorControl_Move_Mode_t  MoveDirection;
   uint16_t                             TransTime;
   qbool_t                              IsEnhanced;

   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 5) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 3)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[4]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[5]), 0, 1)))
      {
         DeviceId      = Parameter_List[0].Integer_Value;
         ClusterInfo   = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_COLOR_CONTROL, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         MoveDirection = (qapi_ZB_CL_ColorControl_Move_Mode_t)(Parameter_List[2].Integer_Value);
         Hue           = (uint16_t)Parameter_List[3].Integer_Value;
         TransTime     = Parameter_List[4].Integer_Value;
         IsEnhanced    = (Parameter_List[5].Integer_Value != 0) ? true : false;

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_ColorControl_Send_Move_To_Hue(ClusterInfo->Handle, &SendInfo, Hue, MoveDirection, TransTime, IsEnhanced);
               if(Result == QAPI_OK)
               {
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
                  Display_Function_Success(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Move_To_Hue");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Move_To_Hue", Result);
               }
            }
            else
            {
               QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid Cluster Index.\n");
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
      QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SendMoveHue" command.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the Color Control client cluster to use to send
                     the command.
   Parameter_List[2] is the move mode.
   Parameter_List[3] is the move rate.
   Parameter_List[4] is a flag indicating if this is an enhanced hue command
                     (1) or a normal one (0).

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_ColorControl_MoveHue(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t                Ret_Val;
   qapi_Status_t                        Result;
   ZCL_Demo_Cluster_Info_t             *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t       SendInfo;
   uint32_t                             DeviceId;
   qapi_ZB_CL_ColorControl_Move_Mode_t  MoveMode;
   uint16_t                             Rate;
   qbool_t                              IsEnhanced;

   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 4) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 2, 4)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[4]), 0, 1)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_COLOR_CONTROL, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         MoveMode    = (qapi_ZB_CL_ColorControl_Move_Mode_t)(Parameter_List[2].Integer_Value);
         Rate        = (uint16_t)(Parameter_List[3].Integer_Value);
         IsEnhanced  = (Parameter_List[4].Integer_Value != 0) ? true : false;

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_ColorControl_Send_Move_Hue(ClusterInfo->Handle, &SendInfo, MoveMode, Rate, IsEnhanced);
               if(Result == QAPI_OK)
               {
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
                  Display_Function_Success(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Move_Hue");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Move_Hue", Result);
               }
            }
            else
            {
               QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid ClusterIndex.\n");
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
      QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SendStepHue" command.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the Color Control client cluster to use to send
                     the command.
   Parameter_List[2] is the step mode.
   Parameter_List[3] is the step size.
   Parameter_List[4] is the transition time.
   Parameter_List[5] is a flag indicating if this is an enhanced hue command
                     (1) or a normal one (0).

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_ColorControl_StepHue(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t                Ret_Val;
   qapi_Status_t                        Result;
   ZCL_Demo_Cluster_Info_t             *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t       SendInfo;
   uint32_t                             DeviceId;
   qapi_ZB_CL_ColorControl_Move_Mode_t  StepMode;
   uint16_t                             StepSize;
   uint16_t                             TransTime;
   qbool_t                              IsEnhanced;

   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 5) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 2, 3)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[4]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[5]), 0, 1)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_COLOR_CONTROL, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         StepMode    = (qapi_ZB_CL_ColorControl_Move_Mode_t)(Parameter_List[2].Integer_Value);
         StepSize    = (uint16_t)(Parameter_List[3].Integer_Value);
         TransTime   = (uint16_t)(Parameter_List[4].Integer_Value);
         IsEnhanced  = (Parameter_List[5].Integer_Value != 0) ? true : false;

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_ColorControl_Send_Step_Hue(ClusterInfo->Handle, &SendInfo, StepMode, StepSize, TransTime, IsEnhanced);
               if(Result == QAPI_OK)
               {
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
                  Display_Function_Success(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Step_Hue");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Step_Hue", Result);
               }
            }
            else
            {
               QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid ClusterIndex.\n");
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
      QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SendMoveToSaturation" command.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the Color Control client cluster to use to send
                     the command.
   Parameter_List[2] is the saturation that is moved to.
   Parameter_List[3] is the transition time.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_ColorControl_MoveToSaturation(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   uint32_t                        DeviceId;
   uint8_t                         Saturation;
   uint16_t                        TransTime;

   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 3) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFFFF)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_COLOR_CONTROL, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         Saturation  = (uint8_t)(Parameter_List[2].Integer_Value);
         TransTime   = (uint16_t)(Parameter_List[3].Integer_Value);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_ColorControl_Send_Move_To_Saturation(ClusterInfo->Handle, &SendInfo, Saturation, TransTime);
               if(Result == QAPI_OK)
               {
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
                  Display_Function_Success(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Move_To_Saturation");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Move_To_Saturation", Result);
               }
            }
            else
            {
               QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid ClusterIndex.\n");
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
      QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SendMoveSaturation" command.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the Color Control client cluster to use to send
                     the command.
   Parameter_List[2] is the move mode.
   Parameter_List[3] is the rate.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_ColorControl_MoveSaturation(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t                Ret_Val;
   qapi_Status_t                        Result;
   ZCL_Demo_Cluster_Info_t             *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t       SendInfo;
   uint32_t                             DeviceId;
   qapi_ZB_CL_ColorControl_Move_Mode_t  MoveMode;
   uint8_t                              Rate;

   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 3) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 2, 4)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFF)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_COLOR_CONTROL, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         MoveMode    = (qapi_ZB_CL_ColorControl_Move_Mode_t)(Parameter_List[2].Integer_Value);
         Rate        = (uint8_t)(Parameter_List[3].Integer_Value);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_ColorControl_Send_Move_Saturation(ClusterInfo->Handle, &SendInfo, MoveMode, Rate);
               if(Result == QAPI_OK)
               {
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
                  Display_Function_Success(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Move_Saturation");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Move_Saturation", Result);
               }
            }
            else
            {
               QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid ClusterIndex.\n");
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
      QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SendStepSaturation" command.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the Color Control client cluster to use to send
                     the command.
   Parameter_List[2] is the step mode.
   Parameter_List[3] is the step size.
   Parameter_List[4] is the transition time.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_ColorControl_StepSaturation(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t                Ret_Val;
   qapi_Status_t                        Result;
   ZCL_Demo_Cluster_Info_t             *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t       SendInfo;
   uint32_t                             DeviceId;
   qapi_ZB_CL_ColorControl_Move_Mode_t  StepMode;
   uint8_t                              StepSize;
   uint8_t                              TransTime;

   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 4) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 2, 3)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[4]), 0, 0xFF)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_COLOR_CONTROL, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         StepMode    = (qapi_ZB_CL_ColorControl_Move_Mode_t)(Parameter_List[2].Integer_Value);
         StepSize    = (uint8_t)(Parameter_List[3].Integer_Value);
         TransTime   = (uint8_t)(Parameter_List[4].Integer_Value);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_ColorControl_Send_Step_Saturation(ClusterInfo->Handle, &SendInfo, StepMode, StepSize, TransTime);
               if(Result == QAPI_OK)
               {
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
                  Display_Function_Success(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Step_Saturation");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Step_Saturation", Result);
               }
            }
            else
            {
               QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid ClusterIndex.\n");
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
      QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SendMoveToHueAndSaturation" command.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the Color Control client cluster to use to send
                     the command.
   Parameter_List[2] is the hue that is moved to.
   Parameter_List[3] is the saturation that is moved to.
   Parameter_List[4] is the transition time.
   Parameter_List[5] is a flag indicating if this is an enhanced hue command
                     (1) or a normal one (0).

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_ColorControl_MoveToHueAndSaturation(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   uint32_t                        DeviceId;
   uint16_t                        Hue;
   uint8_t                         Saturation;
   uint16_t                        TransTime;
   qbool_t                         IsEnhanced;

   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 5) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[4]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[5]), 0, 1)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_COLOR_CONTROL, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         Hue         = (uint16_t)(Parameter_List[2].Integer_Value);
         Saturation  = (uint8_t)(Parameter_List[3].Integer_Value);
         TransTime   = (uint16_t)(Parameter_List[4].Integer_Value);
         IsEnhanced  = (Parameter_List[5].Integer_Value != 0) ? true : false;

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_ColorControl_Send_Move_To_HueAndSaturation(ClusterInfo->Handle, &SendInfo, Hue, Saturation, TransTime, IsEnhanced);
               if(Result == QAPI_OK)
               {
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
                  Display_Function_Success(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Move_To_HueAndSaturation");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Move_To_HueAndSaturation", Result);
               }
            }
            else
            {
               QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid ClusterIndex.\n");
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
      QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SendMoveToColor" command.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the Color Control client cluster to use to send
                     the command.
   Parameter_List[2] is the color X.
   Parameter_List[3] is the color Y.
   Parameter_List[4] is the transition time.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_ColorControl_MoveToColor(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   uint32_t                        DeviceId;
   uint16_t                        ColorX;
   uint16_t                        ColorY;
   uint16_t                        TransTime;

   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 4) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[4]), 0, 0xFFFF)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_COLOR_CONTROL, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         ColorX      = (uint16_t)(Parameter_List[2].Integer_Value);
         ColorY      = (uint16_t)(Parameter_List[3].Integer_Value);
         TransTime   = (uint16_t)(Parameter_List[4].Integer_Value);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_ColorControl_Send_Move_To_Color(ClusterInfo->Handle, &SendInfo, ColorX, ColorY, TransTime);
               if(Result == QAPI_OK)
               {
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
                  Display_Function_Success(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Move_To_Color");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Move_To_Color", Result);
               }
            }
            else
            {
               QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid ClusterIndex.\n");
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
      QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SendMoveColor" command.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the Color Control client cluster to use to send
                     the command.
   Parameter_List[2] is the rate X.
   Parameter_List[3] is the rate Y.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_ColorControl_MoveColor(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   uint32_t                        DeviceId;
   int16_t                         RateX;
   int16_t                         RateY;

   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 3) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), -32768, 32767)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), -32768, 32767)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_COLOR_CONTROL, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         RateX       = (int16_t)(Parameter_List[2].Integer_Value);
         RateY       = (int16_t)(Parameter_List[3].Integer_Value);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_ColorControl_Send_Move_Color(ClusterInfo->Handle, &SendInfo, RateX, RateY);
               if(Result == QAPI_OK)
               {
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
                  Display_Function_Success(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Move_Color");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Move_Color", Result);
               }
            }
            else
            {
               QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid ClusterIndex.\n");
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
      QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SendStepColor" command.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the Color Control client cluster to use to send
                     the command.
   Parameter_List[2] is the step X.
   Parameter_List[3] is the step Y.
   Parameter_List[4] is the transition time.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_ColorControl_StepColor(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   uint32_t                        DeviceId;
   int16_t                         StepX;
   int16_t                         StepY;
   uint16_t                        TransTime;

   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 4) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), -32768, 32767)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), -32768, 32767)) &&
         (Verify_Integer_Parameter(&(Parameter_List[4]), 0, 0xFFFF)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_COLOR_CONTROL, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         StepX       = (int16_t)(Parameter_List[2].Integer_Value);
         StepY       = (int16_t)(Parameter_List[3].Integer_Value);
         TransTime   = (uint16_t)(Parameter_List[4].Integer_Value);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_ColorControl_Send_Step_Color(ClusterInfo->Handle, &SendInfo, StepX, StepY, TransTime);
               if(Result == QAPI_OK)
               {
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
                  Display_Function_Success(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Step_Color");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Step_Color", Result);
               }
            }
            else
            {
               QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid ClusterIndex.\n");
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
      QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SendMoveToColorTemp" command.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the Color Control client cluster to use to send
                     the command.
   Parameter_List[2] is the color temp mireds.
   Parameter_List[3] is the transition time.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_ColorControl_MoveToColorTemp(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   uint32_t                        DeviceId;
   uint16_t                        Mireds;
   uint16_t                        TransTime;

   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 3) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFFFF)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_COLOR_CONTROL, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         Mireds      = (uint16_t)(Parameter_List[2].Integer_Value);
         TransTime   = (uint16_t)(Parameter_List[3].Integer_Value);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_ColorControl_Send_Move_To_Color_Temp(ClusterInfo->Handle, &SendInfo, Mireds, TransTime);
               if(Result == QAPI_OK)
               {
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
                  Display_Function_Success(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Move_To_Color_Temp");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Move_To_Color_Temp", Result);
               }
            }
            else
            {
               QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid ClusterIndex.\n");
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
      QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SendMoveColorTemp" command.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the Color Control client cluster to use to send
                     the command.
   Parameter_List[2] is the move mode.
   Parameter_List[3] is the rate.
   Parameter_List[4] is the limit for the color temp.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_ColorControl_MoveColorTemp(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t                Ret_Val;
   qapi_Status_t                        Result;
   ZCL_Demo_Cluster_Info_t             *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t       SendInfo;
   uint32_t                             DeviceId;
   qapi_ZB_CL_ColorControl_Move_Mode_t  MoveMode;
   uint16_t                             Rate;
   uint16_t                             Limit;

   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 4) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 2, 4)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[4]), 0, 0xFFFF)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_COLOR_CONTROL, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         MoveMode    = (qapi_ZB_CL_ColorControl_Move_Mode_t)(Parameter_List[2].Integer_Value);
         Rate        = (uint16_t)(Parameter_List[3].Integer_Value);
         Limit       = (uint16_t)(Parameter_List[4].Integer_Value);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_ColorControl_Send_Move_Color_Temp(ClusterInfo->Handle, &SendInfo, MoveMode, Rate, Limit);
               if(Result == QAPI_OK)
               {
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
                  Display_Function_Success(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Move_Color_Temp");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Move_Color_Temp", Result);
               }
            }
            else
            {
               QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid ClusterIndex.\n");
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
      QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SendStepColorTemp" command.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the Color Control client cluster to use to send
                     the command.
   Parameter_List[2] is the step mode.
   Parameter_List[3] is the step size.
   Parameter_List[4] is the transition time.
   Parameter_List[5] is the min color temp mireds.
   Parameter_List[6] is the max color temp mireds.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_ColorControl_StepColorTemp(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t                      Ret_Val;
   qapi_Status_t                              Result;
   ZCL_Demo_Cluster_Info_t                   *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t             SendInfo;
   uint32_t                                   DeviceId;
   qapi_ZB_CL_ColorControl_Step_Color_Temp_t  StepColorTemp;

   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 5) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 2, 3)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[4]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[5]), 0, 0xFFFF)))
      {
         memset(&StepColorTemp, 0, sizeof(qapi_ZB_CL_ColorControl_Step_Color_Temp_t));
         DeviceId                     = Parameter_List[0].Integer_Value;
         ClusterInfo                  = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_COLOR_CONTROL, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         StepColorTemp.StepMode       = (qapi_ZB_CL_ColorControl_Move_Mode_t)(Parameter_List[2].Integer_Value);
         StepColorTemp.StepSize       = (uint16_t)(Parameter_List[3].Integer_Value);
         StepColorTemp.TransitionTime = (uint16_t)(Parameter_List[4].Integer_Value);
         StepColorTemp.Limit          = (uint16_t)(Parameter_List[5].Integer_Value);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_ColorControl_Send_Step_Color_Temp(ClusterInfo->Handle, &SendInfo, &StepColorTemp);
               if(Result == QAPI_OK)
               {
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
                  Display_Function_Success(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Step_Color_Temp");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Step_Color_Temp", Result);
               }
            }
            else
            {
               QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid ClusterIndex.\n");
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
      QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SendStopMoveStep" command.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the Color Control client cluster to use to send
                     the command.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_ColorControl_StopMoveStep(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t           Ret_Val;
   qapi_Status_t                   Result;
   ZCL_Demo_Cluster_Info_t        *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t  SendInfo;
   uint32_t                        DeviceId;

   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 1) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)))
      {
         DeviceId    = Parameter_List[0].Integer_Value;
         ClusterInfo = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_COLOR_CONTROL, ZCL_DEMO_CLUSTERTYPE_CLIENT);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_ColorControl_Send_Stop_Move_Step(ClusterInfo->Handle, &SendInfo);
               if(Result == QAPI_OK)
               {
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
                  Display_Function_Success(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Stop_Move_Step");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Stop_Move_Step", Result);
               }
            }
            else
            {
               QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid ClusterIndex.\n");
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
      QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SendColorLoopSet" command.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).
   Parameter_List[1] Endpoint of the Color Control client cluster to use to send
                     the command.
   Parameter_List[2] is the update flag.
   Parameter_List[3] is the action mode.
   Parameter_List[4] is the direction mode.
   Parameter_List[5] is the time.
   Parameter_List[6] is the start hue.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_ColorControl_ColorLoopSet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t                     Ret_Val;
   qapi_Status_t                             Result;
   ZCL_Demo_Cluster_Info_t                  *ClusterInfo;
   qapi_ZB_CL_General_Send_Info_t            SendInfo;
   uint32_t                                  DeviceId;
   qapi_ZB_CL_ColorControl_Color_Loop_Set_t  ColorLoopSet;

   if(GetZigBeeHandle() != NULL)
   {
      if((Parameter_Count >= 7) &&
         (Parameter_List[0].Integer_Is_Valid) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), QAPI_ZB_APS_MIN_ENDPOINT, QAPI_ZB_APS_MAX_ENDPOINT)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 2)) &&
         (Verify_Integer_Parameter(&(Parameter_List[4]), 2, 3)) &&
         (Verify_Integer_Parameter(&(Parameter_List[5]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[6]), 0, 0xFFFF)))
      {
         memset(&ColorLoopSet, 0, sizeof(qapi_ZB_CL_ColorControl_Color_Loop_Set_t));

         DeviceId                = Parameter_List[0].Integer_Value;
         ClusterInfo             = ZCL_FindClusterByEndpoint((uint8_t)(Parameter_List[1].Integer_Value), QAPI_ZB_CL_CLUSTER_ID_COLOR_CONTROL, ZCL_DEMO_CLUSTERTYPE_CLIENT);
         ColorLoopSet.UpdateFlag = (uint8_t)(Parameter_List[2].Integer_Value);
         ColorLoopSet.Action     = (qapi_ZB_CL_ColorControl_Loop_Action_t)(Parameter_List[3].Integer_Value);
         ColorLoopSet.Direction  = (qapi_ZB_CL_ColorControl_Move_Mode_t)(Parameter_List[4].Integer_Value);
         ColorLoopSet.Time       = (uint16_t)(Parameter_List[5].Integer_Value);
         ColorLoopSet.StartHue   = (uint16_t)(Parameter_List[6].Integer_Value);

         if(ClusterInfo != NULL)
         {
            memset(&SendInfo, 0, sizeof(SendInfo));

            /* Format the destination addr. mode, address, and endpoint. */
            if(Format_Send_Info_By_Device(DeviceId, &SendInfo))
            {
               Result = qapi_ZB_CL_ColorControl_Send_Color_Loop_Set(ClusterInfo->Handle, &SendInfo, &ColorLoopSet);
               if(Result == QAPI_OK)
               {
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
                  Display_Function_Success(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Color_Loop_Set");
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
                  Display_Function_Error(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Send_Color_Loop_Set", Result);
               }
            }
            else
            {
               QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid device ID.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Invalid ClusterIndex.\n");
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
      QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SendColorLoopSet" command.

   Parameter_List[0] CurrentX value for the scene.
   Parameter_List[1] CurrentY value for the scene.
   Parameter_List[2] EnhancedCurrentHue value for the scene.
   Parameter_List[3] CurrentSaturation value for the scene.
   Parameter_List[4] ColorLoopActive value for the scene.
   Parameter_List[5] ColorLoopDirection value for the scene.
   Parameter_List[6] ColorLoopTime value for the scene.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_ColorControl_SetSceneData(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;
   uint16_t              CurrentX;
   uint16_t              CurrentY;
   uint16_t              EnhancedCurrentHue;
   uint8_t               CurrentSaturation;
   uint8_t               ColorLoopActive;
   uint8_t               ColorLoopDirection;
   uint16_t              ColorLoopTime;

   if((Parameter_Count >= 7) &&
      (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 0xFEFF)) &&
      (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 0xFEFF)) &&
      (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFFFF)) &&
      (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 0xFE)) &&
      (Verify_Integer_Parameter(&(Parameter_List[4]), 0, 0xFF)) &&
      (Verify_Integer_Parameter(&(Parameter_List[5]), 0, 0xFF)) &&
      (Verify_Integer_Parameter(&(Parameter_List[6]), 0, 0xFFFF)))
   {
      CurrentX           = (uint16_t)(Parameter_List[0].Integer_Value);
      CurrentY           = (uint16_t)(Parameter_List[1].Integer_Value);
      EnhancedCurrentHue = (uint16_t)(Parameter_List[2].Integer_Value);
      CurrentSaturation  = (uint8_t)(Parameter_List[3].Integer_Value);
      ColorLoopActive    = (uint8_t)(Parameter_List[4].Integer_Value);
      ColorLoopDirection = (uint8_t)(Parameter_List[5].Integer_Value);
      ColorLoopTime      = (uint16_t)(Parameter_List[6].Integer_Value);

      ZigBee_ColorControl_Demo_Context.SceneData[0]  = (uint8_t)(CurrentX);
      ZigBee_ColorControl_Demo_Context.SceneData[1]  = (uint8_t)(CurrentX >> 8);
      ZigBee_ColorControl_Demo_Context.SceneData[2]  = (uint8_t)(CurrentY);
      ZigBee_ColorControl_Demo_Context.SceneData[3]  = (uint8_t)(CurrentY >> 8);
      ZigBee_ColorControl_Demo_Context.SceneData[4]  = (uint8_t)(EnhancedCurrentHue);
      ZigBee_ColorControl_Demo_Context.SceneData[5]  = (uint8_t)(EnhancedCurrentHue >> 8);
      ZigBee_ColorControl_Demo_Context.SceneData[6]  = CurrentSaturation;
      ZigBee_ColorControl_Demo_Context.SceneData[7]  = ColorLoopActive;
      ZigBee_ColorControl_Demo_Context.SceneData[8]  = ColorLoopDirection;
      ZigBee_ColorControl_Demo_Context.SceneData[9]  = (uint8_t)(ColorLoopTime);
      ZigBee_ColorControl_Demo_Context.SceneData[10] = (uint8_t)(ColorLoopTime >> 8);

      QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Scene data set successfully.\n");
      Ret_Val = QCLI_STATUS_SUCCESS_E;
   }
   else
   {
      Ret_Val = QCLI_STATUS_USAGE_E;
   }

   return(Ret_Val);
}

/**
   @brief Handles callbacks for the Color Control server cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_ColorControl_Demo_Server_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_ColorControl_Server_Event_Data_t *EventData, uint32_t CB_Param)
{
   ZCL_Demo_Cluster_Info_t      *ClusterInfo;
   ZCL_ColorControl_Demo_Data_t *ColorControlData;
   qbool_t                       DisplayPrompt;

   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      ClusterInfo = ZCL_FindClusterByHandle(Cluster);
      if(ClusterInfo != NULL)
      {
         ColorControlData = (ZCL_ColorControl_Demo_Data_t *)(ClusterInfo->PrivData);
      }
      else
      {
         ColorControlData = NULL;
      }

      DisplayPrompt = true;

      switch(EventData->Event_Type)
      {
         case QAPI_ZB_CL_COLORCONTROL_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E:
            switch(EventData->Data.Attr_Custom_Read.AttrId)
            {
               case QAPI_ZB_CL_COLORCONTROL_ATTR_ID_CURRENT_HUE:
                  if(ColorControlData != NULL)
                  {
                     if((*EventData->Data.Attr_Custom_Read.DataLength) >= sizeof(ColorControlData->CurrentHue))
                     {
                        QSOSAL_MemCopy_S(EventData->Data.Attr_Custom_Read.Data, *EventData->Data.Attr_Custom_Read.DataLength, &(ColorControlData->CurrentHue), sizeof(ColorControlData->CurrentHue));
                        *EventData->Data.Attr_Custom_Read.Result = QAPI_OK;
                     }
                     else
                     {
                        *EventData->Data.Attr_Custom_Read.Result = QAPI_ERR_BOUNDS;
                     }

                     *EventData->Data.Attr_Custom_Read.DataLength = sizeof(ColorControlData->CurrentHue);
                  }
                  break;

               case QAPI_ZB_CL_COLORCONTROL_ATTR_ID_CURRENT_SATURATION:
                  if(ColorControlData != NULL)
                  {
                     if((*EventData->Data.Attr_Custom_Read.DataLength) >= sizeof(ColorControlData->CurrentSaturation))
                     {
                        QSOSAL_MemCopy_S(EventData->Data.Attr_Custom_Read.Data, *EventData->Data.Attr_Custom_Read.DataLength, &(ColorControlData->CurrentSaturation), sizeof(ColorControlData->CurrentSaturation));
                        *EventData->Data.Attr_Custom_Read.Result = QAPI_OK;
                     }
                     else
                     {
                        *EventData->Data.Attr_Custom_Read.Result = QAPI_ERR_BOUNDS;
                     }

                     *EventData->Data.Attr_Custom_Read.DataLength = sizeof(ColorControlData->CurrentSaturation);
                  }
                  break;

               case QAPI_ZB_CL_COLORCONTROL_ATTR_ID_ENHANCED_CURRENT_HUE:
                  if(ColorControlData != NULL)
                  {
                     if((*EventData->Data.Attr_Custom_Read.DataLength) >= sizeof(ColorControlData->EnhancedCurrentHue))
                     {
                        QSOSAL_MemCopy_S(EventData->Data.Attr_Custom_Read.Data, *EventData->Data.Attr_Custom_Read.DataLength, &(ColorControlData->EnhancedCurrentHue), sizeof(ColorControlData->EnhancedCurrentHue));
                        *EventData->Data.Attr_Custom_Read.Result = QAPI_OK;
                     }
                     else
                     {
                        *EventData->Data.Attr_Custom_Read.Result = QAPI_ERR_BOUNDS;
                     }

                     *EventData->Data.Attr_Custom_Read.DataLength = sizeof(ColorControlData->EnhancedCurrentHue);
                  }
                  break;

               case QAPI_ZB_CL_COLORCONTROL_ATTR_ID_CURRENT_X:
                  if(ColorControlData != NULL)
                  {
                     if((*EventData->Data.Attr_Custom_Read.DataLength) >= sizeof(ColorControlData->CurrentX))
                     {
                        QSOSAL_MemCopy_S(EventData->Data.Attr_Custom_Read.Data, *EventData->Data.Attr_Custom_Read.DataLength, &(ColorControlData->CurrentX), sizeof(ColorControlData->CurrentX));
                        *EventData->Data.Attr_Custom_Read.Result = QAPI_OK;
                     }
                     else
                     {
                        *EventData->Data.Attr_Custom_Read.Result = QAPI_ERR_BOUNDS;
                     }

                     *EventData->Data.Attr_Custom_Read.DataLength = sizeof(ColorControlData->CurrentX);
                  }
                  break;

               case QAPI_ZB_CL_COLORCONTROL_ATTR_ID_CURRENT_Y:
                  if(ColorControlData != NULL)
                  {
                     if((*EventData->Data.Attr_Custom_Read.DataLength) >= sizeof(ColorControlData->CurrentY))
                     {
                        QSOSAL_MemCopy_S(EventData->Data.Attr_Custom_Read.Data, *EventData->Data.Attr_Custom_Read.DataLength, &(ColorControlData->CurrentY), sizeof(ColorControlData->CurrentY));
                        *EventData->Data.Attr_Custom_Read.Result = QAPI_OK;
                     }
                     else
                     {
                        *EventData->Data.Attr_Custom_Read.Result = QAPI_ERR_BOUNDS;
                     }

                     *EventData->Data.Attr_Custom_Read.DataLength = sizeof(ColorControlData->CurrentY);
                  }
                  break;

               case QAPI_ZB_CL_COLORCONTROL_ATTR_ID_COLOR_TEMP_MIREDS:
                  if(ColorControlData != NULL)
                  {
                     if((*EventData->Data.Attr_Custom_Read.DataLength) >= sizeof(ColorControlData->ColorTempMireds))
                     {
                        QSOSAL_MemCopy_S(EventData->Data.Attr_Custom_Read.Data, *EventData->Data.Attr_Custom_Read.DataLength, &(ColorControlData->ColorTempMireds), sizeof(ColorControlData->ColorTempMireds));
                        *EventData->Data.Attr_Custom_Read.Result = QAPI_OK;
                     }
                     else
                     {
                        *EventData->Data.Attr_Custom_Read.Result = QAPI_ERR_BOUNDS;
                     }

                     *EventData->Data.Attr_Custom_Read.DataLength = sizeof(ColorControlData->ColorTempMireds);
                  }
                  break;

               default:
                  *EventData->Data.Attr_Custom_Read.Result = QAPI_ZB_ERR_ZCL_UNSUPPORTED_ATTRIBUTE;
                  break;
            }

            DisplayPrompt = false;
            break;

         case QAPI_ZB_CL_COLORCONTROL_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E:
            switch(EventData->Data.Attr_Custom_Write.AttrId)
            {
               case QAPI_ZB_CL_COLORCONTROL_ATTR_ID_CURRENT_HUE:
                  if(ColorControlData != NULL)
                  {
                     if(EventData->Data.Attr_Custom_Write.DataLength == sizeof(ColorControlData->CurrentHue))
                     {
                        QSOSAL_MemCopy_S(&(ColorControlData->CurrentHue), sizeof(ColorControlData->CurrentHue), EventData->Data.Attr_Custom_Write.Data, EventData->Data.Attr_Custom_Write.DataLength);
                        *EventData->Data.Attr_Custom_Write.Result = QAPI_OK;
                     }
                     else
                     {
                        *EventData->Data.Attr_Custom_Write.Result = QAPI_ZB_ERR_ZCL_INVALID_FIELD;
                     }
                  }
                  break;

               case QAPI_ZB_CL_COLORCONTROL_ATTR_ID_CURRENT_SATURATION:
                  if(ColorControlData != NULL)
                  {
                     if(EventData->Data.Attr_Custom_Write.DataLength == sizeof(ColorControlData->CurrentSaturation))
                     {
                        QSOSAL_MemCopy_S(&(ColorControlData->CurrentSaturation), sizeof(ColorControlData->CurrentSaturation), EventData->Data.Attr_Custom_Write.Data, EventData->Data.Attr_Custom_Write.DataLength);
                        *EventData->Data.Attr_Custom_Write.Result = QAPI_OK;
                     }
                     else
                     {
                        *EventData->Data.Attr_Custom_Write.Result = QAPI_ZB_ERR_ZCL_INVALID_FIELD;
                     }
                  }
                  break;

               case QAPI_ZB_CL_COLORCONTROL_ATTR_ID_ENHANCED_CURRENT_HUE:
                  if(ColorControlData != NULL)
                  {
                     if(EventData->Data.Attr_Custom_Write.DataLength == sizeof(ColorControlData->EnhancedCurrentHue))
                     {
                        QSOSAL_MemCopy_S(&(ColorControlData->EnhancedCurrentHue), sizeof(ColorControlData->EnhancedCurrentHue), EventData->Data.Attr_Custom_Write.Data, EventData->Data.Attr_Custom_Write.DataLength);
                        *EventData->Data.Attr_Custom_Write.Result = QAPI_OK;
                     }
                     else
                     {
                        *EventData->Data.Attr_Custom_Write.Result = QAPI_ZB_ERR_ZCL_INVALID_FIELD;
                     }
                  }
                  break;

               case QAPI_ZB_CL_COLORCONTROL_ATTR_ID_CURRENT_X:
                  if(ColorControlData != NULL)
                  {
                     if(EventData->Data.Attr_Custom_Write.DataLength == sizeof(ColorControlData->CurrentX))
                     {
                        QSOSAL_MemCopy_S(&(ColorControlData->CurrentX), sizeof(ColorControlData->CurrentX), EventData->Data.Attr_Custom_Write.Data, EventData->Data.Attr_Custom_Write.DataLength);
                        *EventData->Data.Attr_Custom_Write.Result = QAPI_OK;
                     }
                     else
                     {
                        *EventData->Data.Attr_Custom_Write.Result = QAPI_ZB_ERR_ZCL_INVALID_FIELD;
                     }
                  }
                  break;

               case QAPI_ZB_CL_COLORCONTROL_ATTR_ID_CURRENT_Y:
                  if(ColorControlData != NULL)
                  {
                     if(EventData->Data.Attr_Custom_Write.DataLength == sizeof(ColorControlData->CurrentY))
                     {
                        QSOSAL_MemCopy_S(&(ColorControlData->CurrentY), sizeof(ColorControlData->CurrentY), EventData->Data.Attr_Custom_Write.Data, EventData->Data.Attr_Custom_Write.DataLength);
                        *EventData->Data.Attr_Custom_Write.Result = QAPI_ZB_ERR_ZCL_INVALID_FIELD;
                     }
                     else
                     {
                        *EventData->Data.Attr_Custom_Write.Result = QAPI_ZB_ERR_ZCL_INVALID_FIELD;
                     }
                  }
                  break;

               case QAPI_ZB_CL_COLORCONTROL_ATTR_ID_COLOR_TEMP_MIREDS:
                  if(ColorControlData != NULL)
                  {
                     if(EventData->Data.Attr_Custom_Write.DataLength == sizeof(ColorControlData->ColorTempMireds))
                     {
                        QSOSAL_MemCopy_S(&(ColorControlData->ColorTempMireds), sizeof(ColorControlData->ColorTempMireds), EventData->Data.Attr_Custom_Write.Data, EventData->Data.Attr_Custom_Write.DataLength);
                        *EventData->Data.Attr_Custom_Write.Result = QAPI_OK;
                     }
                     else
                     {
                        *EventData->Data.Attr_Custom_Write.Result = QAPI_ZB_ERR_ZCL_INVALID_FIELD;
                     }
                  }
                  break;

               default:
                  *EventData->Data.Attr_Custom_Write.Result = QAPI_ZB_ERR_ZCL_UNSUPPORTED_ATTRIBUTE;
                  break;
            }

            DisplayPrompt = false;
            break;

         case QAPI_ZB_CL_COLORCONTROL_SERVER_EVENT_TYPE_UNPARSED_DATA_E:
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "ColorControl Server Unparsed Event Data.\n");
            break;

         case QAPI_ZB_CL_COLORCONTROL_SERVER_EVENT_TYPE_MOVE_TO_HUE_AND_SAT_E:
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "ColorControl Server Move to Hue and Sat Event:\n");
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "  Mode:           %d\n", EventData->Data.Move_To_Hue_And_Sat.Mode);
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "  Hue:            %d\n", EventData->Data.Move_To_Hue_And_Sat.Hue);
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "  Saturation:     %d\n", EventData->Data.Move_To_Hue_And_Sat.Saturation);
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "  TransitionTime: %d\n", EventData->Data.Move_To_Hue_And_Sat.TransitionTime);
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "  IsEnhanced:     %s\n", EventData->Data.Move_To_Hue_And_Sat.IsEnhanced ? "True" : "False");

            if(ColorControlData != NULL)
            {
               ColorControlData->CurrentSaturation = EventData->Data.Move_To_Hue_And_Sat.Saturation;

               if(EventData->Data.Move_To_Hue_And_Sat.IsEnhanced)
               {
                  ColorControlData->CurrentHue         = EventData->Data.Move_To_Hue_And_Sat.Hue >> 8;
                  ColorControlData->EnhancedCurrentHue = EventData->Data.Move_To_Hue_And_Sat.Hue;

                  /* Cap the Hue. */
                  if(ColorControlData->CurrentHue > 0xFE)
                  {
                     ColorControlData->CurrentHue = 0xFE;
                  }
               }
               else
               {
                  ColorControlData->CurrentHue         = EventData->Data.Move_To_Hue_And_Sat.Hue;
                  ColorControlData->EnhancedCurrentHue = EventData->Data.Move_To_Hue_And_Sat.Hue << 8;
               }
            }
            break;

         case QAPI_ZB_CL_COLORCONTROL_SERVER_EVENT_TYPE_MOVE_TO_COLOR_E:
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "ColorControl Server Move To Color Event:\n");
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "  ColorX:          %d\n", EventData->Data.Move_To_Color.ColorX);
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "  ColorY:          %d\n", EventData->Data.Move_To_Color.ColorY);
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "  TransitionTimeX: %d\n", EventData->Data.Move_To_Color.TransitionTimeX);
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "  TransitionTimeY: %d\n", EventData->Data.Move_To_Color.TransitionTimeY);

            if(ColorControlData != NULL)
            {
               /* To keep the demo simply, set the attribute values assuming an
                  instant transition time. */
               ColorControlData->CurrentX = EventData->Data.Move_To_Color.ColorX;
               ColorControlData->CurrentY = EventData->Data.Move_To_Color.ColorY;
            }
            break;

         case QAPI_ZB_CL_COLORCONTROL_SERVER_EVENT_TYPE_MOVE_TO_COLOR_TEMP_E:
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "ColorControl Server Move To Color Temp Event:\n");
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "  Mode:           %d\n", EventData->Data.Move_To_Color_Temp.Mode);
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "  Mireds:         %d\n", EventData->Data.Move_To_Color_Temp.Mireds);
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "  TransitionTime: %d\n", EventData->Data.Move_To_Color_Temp.TransitionTime);

            if(ColorControlData != NULL)
            {
               ColorControlData->ColorTempMireds = EventData->Data.Move_To_Color_Temp.Mireds;
            }
            break;

         case QAPI_ZB_CL_COLORCONTROL_SERVER_EVENT_TYPE_COLOR_LOOP_SET_E:
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "ColorControl Server Color Loop Set Event:\n");
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "  UpdateFlag: %d\n", EventData->Data.Color_Loop_Set.UpdateFlag);
            if(EventData->Data.Color_Loop_Set.UpdateFlag & QAPI_ZB_CL_COLORCONTROL_COLORLOOP_SET_FLAG_UPDATE_ACTION)
            {
               QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "  Action:     %d\n", EventData->Data.Color_Loop_Set.Action);
            }
            if(EventData->Data.Color_Loop_Set.UpdateFlag & QAPI_ZB_CL_COLORCONTROL_COLORLOOP_SET_FLAG_UPDATE_DIRECTION)
            {
               QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "  Direction:  %d\n", EventData->Data.Color_Loop_Set.Direction);
            }
            if(EventData->Data.Color_Loop_Set.UpdateFlag & QAPI_ZB_CL_COLORCONTROL_COLORLOOP_SET_FLAG_UPDATE_TIME)
            {
               QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "  StartHue:   %d\n", EventData->Data.Color_Loop_Set.StartHue);
            }
            if(EventData->Data.Color_Loop_Set.UpdateFlag & QAPI_ZB_CL_COLORCONTROL_COLORLOOP_SET_FLAG_UPDATE_START_HUE)
            {
               QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "  Time:       %d\n", EventData->Data.Color_Loop_Set.Time);
            }
            break;

         case QAPI_ZB_CL_COLORCONTROL_SERVER_EVENT_TYPE_STOP_MOVE_STEP_E:
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "ColorControl Server Stop Move Step Event.\n");
            break;

         default:
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "ColorControl Server Unexpected Event %d\n", EventData->Event_Type);
            break;
      }

      if(DisplayPrompt)
      {
         QCLI_Display_Prompt();
      }
   }
}

/**
   @brief Handles callbacks for the Color Control client cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_ColorControl_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_ColorControl_Client_Event_Data_t *EventData, uint32_t CB_Param)
{
   if((ZB_Handle != NULL) && (Cluster != NULL) && (EventData != NULL))
   {
      switch(EventData->Event_Type)
      {
         case QAPI_ZB_CL_COLORCONTROL_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E:
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "ColorControl Client Default Response:\n");
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "  Status:        %d\n", EventData->Data.Default_Response.Status);
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "  CommandID:     0x%02X\n", EventData->Data.Default_Response.CommandId);
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "  CommandStatus: %d\n", EventData->Data.Default_Response.CommandStatus);
            break;

         case QAPI_ZB_CL_COLORCONTROL_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E:
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Command Complete:\n");
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "  CommandStatus: %d\n", EventData->Data.Command_Complete.CommandStatus);
            break;

         default:
            QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Unhandled ColorControl client event %d.\n", EventData->Event_Type);
            break;
      }

      QCLI_Display_Prompt();
   }
}

/**
   @brief Initializes the ZCL ColorControl demo.

   @param ZigBee_QCLI_Handle is the parent QCLI handle for the ColorControl demo.

   @return true if the ZigBee light demo initialized successfully, false
           otherwise.
*/
qbool_t Initialize_ZCL_ColorControl_Demo(QCLI_Group_Handle_t ZigBee_QCLI_Handle)
{
   qbool_t Ret_Val;

   /* Register ColorControl command group. */
   ZigBee_ColorControl_Demo_Context.QCLI_Handle = QCLI_Register_Command_Group(ZigBee_QCLI_Handle, &ZCL_ColorControl_Cmd_Group);
   if(ZigBee_ColorControl_Demo_Context.QCLI_Handle != NULL)
   {
      QSOSAL_MemCopy_S(ZigBee_ColorControl_Demo_Context.SceneData, sizeof(ZigBee_ColorControl_Demo_Context.SceneData), DefaultSceneData, sizeof(DefaultSceneData));
      Ret_Val = true;
   }
   else
   {
      QCLI_Printf(ZigBee_QCLI_Handle, "Failed to register ZCL ColorControl command group.\n");
      Ret_Val = false;
   }

   return(Ret_Val);
}

/**
   @brief Creates an ColorControl server cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_ColorControl_Demo_Create_Server(uint8_t Endpoint, void **PrivData)
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
      ClusterInfo.AttributeList  = ColorControlDemoCustomAttrList;
      ClusterInfo.AttributeCount = COLOR_CONTROL_DEMO_CUSTOM_ATTR_LIST_SIZE;

      *PrivData = malloc(sizeof(ZCL_ColorControl_Demo_Data_t));
      if(*PrivData != NULL)
      {
         QSOSAL_MemCopy_S(*PrivData, sizeof(ZCL_ColorControl_Demo_Data_t), &ColorDefaults, sizeof(ZCL_ColorControl_Demo_Data_t));

         Result = qapi_ZB_CL_ColorControl_Create_Server(ZigBee_Handle, &Ret_Val, COLOR_CONTROL_SERVER_CAPABILITIES, 0, &ClusterInfo, ZCL_ColorControl_Demo_Server_CB, (uint32_t)*PrivData);
         if(Result != QAPI_OK)
         {
            Display_Function_Error(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Create_Server", Result);

            free(*PrivData);
            *PrivData = NULL;
            Ret_Val   = NULL;
         }
      }
      else
      {
         QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "Failed to allocate cluster data.\n");
         Ret_Val = NULL;
      }
   }
   else
   {
      QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

/**
   @brief Creates an ColorControl ckient cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_ColorControl_Demo_Create_Client(uint8_t Endpoint, void **PrivData)
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

      Result = qapi_ZB_CL_ColorControl_Create_Client(ZigBee_Handle, &Ret_Val, &ClusterInfo, ZCL_ColorControl_Demo_Client_CB, 0);
      if(Result != QAPI_OK)
      {
         Display_Function_Error(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "qapi_ZB_CL_ColorControl_Create_Client", Result);
         Ret_Val = NULL;
      }
   }
   else
   {
      QCLI_Printf(ZigBee_ColorControl_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

/**
   @brief Gets the scenes data for a color control cluster.

   @param ExtData Buffer for the extension data of the cluster.

   @return true if the operation is successful false otherwise.
*/
qbool_t ZCL_ColorControl_GetScenesData(qapi_ZB_CL_Scenes_Extension_Field_Set_t *ExtData)
{
   qbool_t Ret_Val;

   if(ExtData != NULL)
   {
      ExtData->ClusterId = QAPI_ZB_CL_CLUSTER_ID_COLOR_CONTROL;
      ExtData->Length    = sizeof(ZigBee_ColorControl_Demo_Context.SceneData);
      ExtData->Data      = ZigBee_ColorControl_Demo_Context.SceneData;
      Ret_Val            = true;
   }
   else
   {
      Ret_Val = false;
   }

   return(Ret_Val);
}

