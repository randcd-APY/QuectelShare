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
#include "qapi_zb_cl_time.h"

typedef enum
{
   tatUnknown,
   tatUint8,
   tatUint32,
   tatInt32
} ZCL_Time_Demo_Attr_Type_t;

typedef struct ZCL_Time_Demo_Attr_Info_s
{
   uint16_t                  AttrId;
   ZCL_Time_Demo_Attr_Type_t AttrType;
} ZCL_Time_Demo_Attr_Info_t;

static QCLI_Group_Handle_t ZCL_Time_QCLI_Handle;

static const ZCL_Time_Demo_Attr_Info_t TimeAttrInfoList[] =
{
   {QAPI_ZB_CL_TIME_ATTR_ID_TIME,             tatUint32},
   {QAPI_ZB_CL_TIME_ATTR_ID_TIME_STATUS,      tatUint8},
   {QAPI_ZB_CL_TIME_ATTR_ID_TIME_ZONE,        tatInt32},
   {QAPI_ZB_CL_TIME_ATTR_ID_DST_START,        tatUint32},
   {QAPI_ZB_CL_TIME_ATTR_ID_DST_END,          tatUint32},
   {QAPI_ZB_CL_TIME_ATTR_ID_DST_SHIFT,        tatInt32},
   {QAPI_ZB_CL_TIME_ATTR_ID_STANDARD_TIME,    tatUint32},
   {QAPI_ZB_CL_TIME_ATTR_ID_LOCAL_TIME,       tatUint32},
   {QAPI_ZB_CL_TIME_ATTR_ID_LAST_SET_TIME,    tatUint32},
   {QAPI_ZB_CL_TIME_ATTR_ID_VALID_UNTIL_TIME, tatUint32}
};

#define TIME_ATTR_INFO_LIST_SIZE       (sizeof(TimeAttrInfoList) / sizeof(ZCL_Time_Demo_Attr_Info_t))

/* Function prototypes. */
static ZCL_Time_Demo_Attr_Type_t ZCL_Time_Demo_GetAttrType(uint16_t AttrId);

static QCLI_Command_Status_t cmd_ZCL_Time_Read(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZCL_Time_Write(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static void ZCL_Time_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_Custom_Cluster_Event_Data_t *EventData, uint32_t CB_Param);

/* Command list for the ZigBee light demo. */
const QCLI_Command_t ZigBee_Time_CMD_List[] =
{
   /* cmd_function      thread  cmd_string  usage_string       description */
   {cmd_ZCL_Time_Read,  false,  "Read",     "[AttrId]",        "Reads an attribute from the local time server."},
   {cmd_ZCL_Time_Write, false,  "Write",    "[AttrId][Value]", "Writes an attribute to the local time server."}
};

const QCLI_Command_Group_t ZCL_Time_Cmd_Group = {"Time", sizeof(ZigBee_Time_CMD_List) / sizeof(QCLI_Command_t), ZigBee_Time_CMD_List};

/**
   @brief Gets the type of attribute given its ID.

   @param AttrId is the ID of the attribute to find.

   @return The type of attribute or tatUnknown if the ID wasn't found.
*/
static ZCL_Time_Demo_Attr_Type_t ZCL_Time_Demo_GetAttrType(uint16_t AttrId)
{
   ZCL_Time_Demo_Attr_Type_t Ret_Val;
   uint8_t                   Index;

   Ret_Val = tatUnknown;
   for(Index = 0; Index < TIME_ATTR_INFO_LIST_SIZE; Index ++)
   {
      if(TimeAttrInfoList[Index].AttrId == AttrId)
      {
         Ret_Val = TimeAttrInfoList[Index].AttrType;
         break;
      }
   }

   return(Ret_Val);
}

/**
   @brief Executes the "Read" command to read an attribute from the local time
          server.

   Parameter_List[0] is the index of the device to send to. Use index zero to
                     use the binding table (if setup).

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_Time_Read(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t             Result;
   QCLI_Command_Status_t     Ret_Val;
   qapi_ZB_Handle_t          ZB_Handle;
   uint16_t                  AttrId;
   uint16_t                  Length;
   ZCL_Time_Demo_Attr_Type_t AttrType;

   union
   {
      uint8_t  Value8;
      uint32_t Value32;
   } Data;

   ZB_Handle = GetZigBeeHandle();
   if(ZB_Handle != NULL)
   {
      /* Validate the device ID. */
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 0xFFFF)))
      {
         AttrId = Parameter_List[0].Integer_Value;

         AttrType = ZCL_Time_Demo_GetAttrType(AttrId);

         Length = sizeof(Data);
         Result = qapi_ZB_CL_Time_Server_Read_Attribute(ZB_Handle, AttrId, &Length, (uint8_t *)&Data);
         if(Result == QAPI_OK)
         {
            Display_Function_Success(ZCL_Time_QCLI_Handle, "qapi_ZB_CL_Time_Server_Read_Attribute");
            QCLI_Printf(ZCL_Time_QCLI_Handle, "Attribute Value: ");
            switch(AttrType)
            {
               case tatUint8:
                  QCLI_Printf(ZCL_Time_QCLI_Handle, "0x%02X\n", Data.Value8);
                  break;

               case tatUint32:
                  QCLI_Printf(ZCL_Time_QCLI_Handle, "%u\n", Data.Value32);
                  break;

               case tatInt32:
                  QCLI_Printf(ZCL_Time_QCLI_Handle, "%d\n", Data.Value32);
                  break;

               case tatUnknown:
               default:
                  QCLI_Printf(ZCL_Time_QCLI_Handle, "\n");
                  Dump_Data(ZCL_Time_QCLI_Handle, "  ", Length, (uint8_t *)&Data);
                  break;
            }

            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }
         else
         {
            Ret_Val = QCLI_STATUS_ERROR_E;
            Display_Function_Error(ZCL_Time_QCLI_Handle, "qapi_ZB_CL_Time_Server_Read_Attribute", Result);
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZCL_Time_QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "Write" command to write an attribute to the local Time
          server.

   Parameter_List[0] is the ID of the attribute to write.
   Parameter_List[1] is the value for the attribute.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZCL_Time_Write(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t      Ret_Val;
   qapi_Status_t              Result;
   qapi_ZB_Handle_t           ZB_Handle;
   uint16_t                   AttrId;
   uint16_t                   Length;
   ZCL_Time_Demo_Attr_Type_t  AttrType;

   union
   {
      uint8_t  Value8;
      uint32_t Value32;
   } Data;

   ZB_Handle = GetZigBeeHandle();
   if(ZB_Handle != NULL)
   {
      /* Validate the device ID. */
      if((Parameter_Count >= 2) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 0xFFFF)))
      {
         AttrId   = Parameter_List[0].Integer_Value;
         AttrType = ZCL_Time_Demo_GetAttrType(AttrId);

         Ret_Val      = QCLI_STATUS_SUCCESS_E;
         Data.Value32 = 0;
         Length       = 0;

         switch(AttrType)
         {
            case tatUint8:
               if(Verify_Integer_Parameter(&(Parameter_List[1]), 0, 0xFF))
               {
                  Data.Value8 = (uint8_t)(Parameter_List[1].Integer_Value);
                  Length  = sizeof(uint8_t);
               }
               else
               {
                  QCLI_Printf(ZCL_Time_QCLI_Handle, "Invalid value for attribute.\n");
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
               break;
            case tatUint32:
            case tatInt32:
               if(Parameter_List[1].Integer_Is_Valid)
               {
                  Data.Value32 = (uint32_t)(Parameter_List[1].Integer_Value);
                  Length       = sizeof(uint32_t);
               }
               else
               {
                  QCLI_Printf(ZCL_Time_QCLI_Handle, "Invalid value for attribute.\n");
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
               break;
            case tatUnknown:
            default:
               Display_Function_Success(ZCL_Time_QCLI_Handle, "Unknown attribute type.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
               break;
         }

         /* Format the destination addr. mode, address, and endpoint. */
         if(Ret_Val == QCLI_STATUS_SUCCESS_E)
         {
            Result = qapi_ZB_CL_Time_Server_Write_Attribute(ZB_Handle, AttrId, Length, (uint8_t *)&Data);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(ZCL_Time_QCLI_Handle, "qapi_ZB_CL_Time_Server_Write_Attribute");
            }
            else
            {
               Ret_Val = QCLI_STATUS_ERROR_E;
               Display_Function_Error(ZCL_Time_QCLI_Handle, "qapi_ZB_CL_Time_Server_Write_Attribute", Result);
            }
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZCL_Time_QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Handles callbacks for the time client cluster.

   @param ZB_Handle is the handle of the ZigBee instance.
   @param EventData is the information for the cluster event.
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZCL_Time_Demo_Client_CB(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_Custom_Cluster_Event_Data_t *EventData, uint32_t CB_Param)
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
            QCLI_Printf(GetZigBeeQCLIHandle(), "Unhandled Time client event %d.\n", EventData->Event_Type);
            break;
      }

      if(DisplayPrompt)
      {
         QCLI_Display_Prompt();
      }
   }
}

/**
   @brief Initializes the ZCL Time demo.

   @param ZigBee_QCLI_Handle is the parent QCLI handle for the Time demo.

   @return true if the ZigBee light demo initialized successfully, false
           otherwise.
*/
qbool_t Initialize_ZCL_Time_Demo(QCLI_Group_Handle_t ZigBee_QCLI_Handle)
{
   qbool_t Ret_Val;

   /* Register Time command group. */
   ZCL_Time_QCLI_Handle = QCLI_Register_Command_Group(ZigBee_QCLI_Handle, &ZCL_Time_Cmd_Group);
   if(ZCL_Time_QCLI_Handle != NULL)
   {
      Ret_Val = true;
   }
   else
   {
      QCLI_Printf(ZigBee_QCLI_Handle, "Failed to register ZCL Time command group.\n");
      Ret_Val = false;
   }

   return(Ret_Val);
}

/**
   @brief Creates an Time client cluster.

   @param Endpoint is the endpoint the cluster will be part of.
   @param PrivData is a pointer to the private data for the cluster demo.  This
                   will be initaialized to NULL before the create function is
                   called so can be ignored if the demo has no private data.

   @return The handle for the newly created function or NULL if there was an
           error.
*/
qapi_ZB_Cluster_t ZCL_Time_Demo_Create_Client(uint8_t Endpoint, void **PrivData)
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
      ClusterInfo.AttributeCount = 0;
      ClusterInfo.AttributeList  = NULL;

      Result = qapi_ZB_CL_Create_Cluster(ZigBee_Handle, &Ret_Val, QAPI_ZB_CL_CLUSTER_ID_TIME, &ClusterInfo, QAPI_ZB_CL_FRAME_DIRECTION_TO_CLIENT_E, ZCL_Time_Demo_Client_CB, 0);
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

