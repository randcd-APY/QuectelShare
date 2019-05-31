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
#include "zdp_demo.h"
#include "qapi_zb.h"
#include "qapi_zb_zdp.h"
#include "qapi_zb_nwk.h"
#include "qsOSAL.h"

/* The maxium length of an attribute being read and written.            */
#define MAXIMUM_ATTRIUBTE_LENGTH                      (8)

static const uint8_t User_Desciptor_Set[] = "Dummy Text";
#define USER_DESCRIPTOR_SET_SIZE          (sizeof(User_Desciptor_Set) - 1)

/* Structure representing the main ZigBee demo context information. */
typedef struct ZigBee_ZDP_Demo_Context_s
{
   QCLI_Group_Handle_t QCLI_Handle;     /*< QCLI handle for the main ZigBee demo. */
} ZigBee_ZDP_Demo_Context_t;

/* The ZigBee ZDP demo context. */
static ZigBee_ZDP_Demo_Context_t ZDP_Demo_Context;

static void DisplayComplexDesc(uint8_t Length, uint8_t *Payload);

/* The following are the QCLI commands for ZB ZDP layer operations. */
static QCLI_Command_Status_t cmd_ZDP_NWKAddr(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZDP_IEEEAddr(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZDP_NodeDesc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZDP_PowerDesc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZDP_SimpleDesc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZDP_ActiveEP(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZDP_MatchDesc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZDP_ComplexDesc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZDP_UserDesc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZDP_UserDescSet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZDP_SysServerDisc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZDP_ExtSimpleDesc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZDP_ExtActiveEP(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZDP_EndBind(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZDP_Bind(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZDP_UnBind(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZDP_MgmtLqi(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZDP_MgmtRtg(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZDP_MgmtBind(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZDP_MgmtLeave(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZDP_MgmtPermitJoining(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZDP_MgmtNWKUpdate(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZDP_SetLocalComplexDesc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZDP_SetLocalUserDesc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static void ZB_ZDP_Event_CB(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_ZDP_Event_t *ZDP_Event_Data, uint32_t CB_Param);

/* Command list for the main ZigBee demo. */
const QCLI_Command_t ZigBee_ZDP_CMD_List[] =
{
   /* cmd_function               thread  cmd_string             usage_string                                                              description */
   {cmd_ZDP_NWKAddr,             false,  "NWKAddr",             "[TargetDevID][ExtAddr][RequestType]",                                    "Sends a ZDP network address request."},
   {cmd_ZDP_IEEEAddr,            false,  "IEEEAddr",            "[TargetDevID][NWKAddr][RequestType]",                                    "Sends a ZDP IEEE address request."},
   {cmd_ZDP_NodeDesc,            false,  "NodeDesc",            "[TargetDevID][NWKAddr(Optional)]",                                       "Sends a ZDP node descriptor reqeust."},
   {cmd_ZDP_PowerDesc,           false,  "PowerDesc",           "[TargetDevID][NWKAddr(Optional)]",                                       "Sends a ZDP power descriptor request"},
   {cmd_ZDP_SimpleDesc,          false,  "SimpleDesc",          "[TargetDevID][NWKAddr(Optional)][Endpoint(Optional)]",                   "Sends a ZDP simple descriptor request"},
   {cmd_ZDP_ActiveEP,            false,  "ActiveEP",            "[TargetDevID][NWKAddr(Optional)]",                                       "Sends a ZDP active EP request."},
   {cmd_ZDP_MatchDesc,           false,  "MatchDesc",           "[TargetDevID][NwkAddr][ClusterID][ServerClient(0=Server,1=Client)]",     "Find an endpoint using a match descriptor request."},
   {cmd_ZDP_ComplexDesc,         false,  "ComplexDesc",         "[TargetDevID][NWKAddr(Optional)]",                                       "Sends a ZDP complex descriptor reqeust."},
   {cmd_ZDP_UserDesc,            false,  "UserDesc",            "[TargetDevID][NWKAddr(Optional)]",                                       "Sends a ZDP user descriptor reqeust."},
   {cmd_ZDP_UserDescSet,         false,  "UserDescSet",         "[TargetDevID][NWKAddr(Optional)]",                                       "Sends a ZDP user descriptor reqeust."},
   {cmd_ZDP_SysServerDisc,       false,  "SysServerDisc",       "[ServerMask]",                                                           "Sends a ZDP system server discover request."},
   {cmd_ZDP_ExtSimpleDesc,       false,  "ExtSimpleDesc",       "[TargetDevID][NWKAddr(Optional)][Endpoint(Optional)]",                   "Sends a ZDP simple descriptor request"},
   {cmd_ZDP_ExtActiveEP,         false,  "ExtActiveEP",         "[TargetDevID][NWKAddr(Optional)]",                                       "Sends a ZDP active EP request."},
   {cmd_ZDP_EndBind,             false,  "EndBind",             "[TargetDevID][SrcDevID][ClusterID][IsServer]",                           "Sends a ZDP end device bind request."},
   {cmd_ZDP_Bind,                false,  "Bind",                "[TargetDevID][SrcDevID][DstDevID][ClusterID]",                           "Sends a ZDP bind request."},
   {cmd_ZDP_UnBind,              false,  "Unbind",              "[TargetDevID][SrcDevID][DstDevID][ClusterID]",                           "Sends a ZDP unbind request"},
   {cmd_ZDP_MgmtLqi,             false,  "MgmtLqi",             "[TargetDevID][StartIndex]",                                              "Sends a ZDP Mgmt Lqi request."},
   {cmd_ZDP_MgmtRtg,             false,  "MgmtRtg",             "[TargetDevID][StartIndex]",                                              "Sends a ZDP Mgmt Lqi request."},
   {cmd_ZDP_MgmtBind,            false,  "MgmtBindRequst",      "[TargetDevID][StartIndex]",                                              "Sends a ZDP Mgmt bind request."},
   {cmd_ZDP_MgmtLeave,           false,  "MgmtLeave",           "[TargetDevID][ExtAddr][RemoveChildren(0=No,1=Yes)][Rejoin(0=No,1=Yes)]", "Sends a ZDP Mgmt leave request"},
   {cmd_ZDP_MgmtPermitJoining,   false,  "MgmtPermitJoining",   "[TargetDevID][Duration][UseTCPolicy(0=No,1=Yes)]",                       "Sends a ZDP Mgmt permit join request."},
   {cmd_ZDP_MgmtNWKUpdate,       false,  "MgmtNWKUpdate",       "[TargetDevID][ScanChannel][ScanDuration][ScanCount(Optional)]",          "Sends a ZDP Mgmt network update request."},
   {cmd_ZDP_SetLocalComplexDesc, false,  "SetLocalComplexDesc", "[ManName][ModelName][SerialNum]"                                         "Sets the local complex descriptor."},
   {cmd_ZDP_SetLocalUserDesc,    false,  "SetLocalUserDesc",    "[Descriptor]"                                                            "Sets the local user descriptor."},
};

const QCLI_Command_Group_t ZigBee_ZDP_CMD_Group = {"ZDP", sizeof(ZigBee_ZDP_CMD_List) / sizeof(QCLI_Command_t), ZigBee_ZDP_CMD_List};

/**
   @brief Helper function to parse the complex descriptor.

   @param[in]     Length    is the length of the payload to be parsed.
   @param[in]     Payload   is the payload to be parsed.
   @param[in/out] ManuName  is where the parsed manufacture name stored.
   @param[in/out] ModleName is where the parsed model name stored.
   @param[in/out] SerialNum is where the parsed serial number stored.

   @return
      -true  if the operation successes.
      -false if the operation fails.
*/
static void DisplayComplexDesc(uint8_t Length, uint8_t *Payload)
{
   uint8_t FieldCount;
   uint8_t FieldType;
   uint8_t FieldLength;
   uint8_t TempString[17];
   qbool_t Error;

   if((Length >= 1) && (Payload != NULL))
   {
      /* Get the field count. */
      FieldCount = *Payload;
      Payload ++;
      Length  --;

      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  ComplexDesc:\n");
      Error = false;
      while((FieldCount > 0) && (!Error))
      {
         if(Length >= 2)
         {
            /* Get the field type. */
            FieldType   = *Payload;
            FieldLength = *(Payload + 1);
            Payload += 2;
            Length  -= 2;

            if(Length >= FieldLength)
            {
               QSOSAL_MemCopy_S(TempString, sizeof(TempString), Payload, FieldLength);
               TempString[FieldLength < sizeof(TempString) ? FieldLength : sizeof(TempString) - 1] = '\0';
               Payload += FieldLength;
               Length  -= FieldLength;

               switch(FieldType)
               {
                  case 0x02:
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "    ManufactureName: %s\n", TempString);
                     break;

                  case 0x03:
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "    ModelName:       %s\n", TempString);
                     break;

                  case 0x04:
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "    SerialNumber:    %s\n", TempString);
                     break;

                  default:
                     Error = true;
                     break;
               }
            }
            else
            {
               Error = true;
            }

            FieldCount --;
         }
         else
         {
            Error = true;
         }
      }

      if((Error) || (Length != 0))
      {
         QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Error parsing complex descriptor\n");
      }
   }
}

/**
   @brief Executes the "NWKAddr" command.

   Parameter_List[0] is the target device ID.
   Parameter_List[1] is the extended address of the device whose NWK address is
                     requested.
   Parameter_List[2] is the request type.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZDP_NWKAddr(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_ZB_Handle_t       ZigBee_Handle;
   ZB_Device_ID_t        *TargetDevice;
   qapi_Status_t          Result;
   uint64_t               ExtAddr;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      if((Parameter_Count >= 3) &&
         (Hex_String_To_ULL(Parameter_List[1].String_Value, &ExtAddr)) &&
         (Verify_Integer_Parameter(&Parameter_List[2], QAPI_ZB_ZDP_ADDR_REQUEST_TYPE_SINGLE_DEVICE_RESPONSE_E, QAPI_ZB_ZDP_ADDR_REQUEST_TYPE_EXTENDED_RESPONSE_E)))
      {
         TargetDevice = GetDeviceListEntry(Parameter_List[0].Integer_Value);
         if(TargetDevice != NULL)
         {
            if(TargetDevice->Type == QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E)
            {
               Result = qapi_ZB_ZDP_Nwk_Addr_Req(ZigBee_Handle, TargetDevice->Address.ShortAddress, ExtAddr, (qapi_ZB_ZDP_Addr_Request_Type_t)(Parameter_List[2].Integer_Value), 0);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Nwk_Addr_Req");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Nwk_Addr_Req", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Target device must be a NWK address.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Device ID is not valid.\n");
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "IEEEAddr" command.

   Parameter_List[0] is the target device ID.
   Parameter_List[1] is the NWK address of the device whose extended address is
                     requested.
   Parameter_List[2] is the request type.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZDP_IEEEAddr(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_Status_t          Result;
   qapi_ZB_Handle_t       ZigBee_Handle;
   ZB_Device_ID_t        *TargetDevice;
   uint16_t               NWKAddrInterested;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      if((Parameter_Count >= 3) &&
         (Verify_Integer_Parameter(&Parameter_List[1], 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&Parameter_List[2], QAPI_ZB_ZDP_ADDR_REQUEST_TYPE_SINGLE_DEVICE_RESPONSE_E, QAPI_ZB_ZDP_ADDR_REQUEST_TYPE_EXTENDED_RESPONSE_E)))
      {
         TargetDevice = GetDeviceListEntry(Parameter_List[0].Integer_Value);
         if(TargetDevice != NULL)
         {
            if(TargetDevice->Type == QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E)
            {
               NWKAddrInterested = (uint16_t)(Parameter_List[1].Integer_Value);
               Result = qapi_ZB_ZDP_IEEE_Addr_Req(ZigBee_Handle, TargetDevice->Address.ShortAddress, NWKAddrInterested, (qapi_ZB_ZDP_Addr_Request_Type_t)(Parameter_List[2].Integer_Value), 0);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_IEEE_Addr_Req");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_IEEE_Addr_Req", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Target device must be a NWK address.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Device ID is not valid.\n");
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "NodeDesc" command.

   Parameter_List[0] is the target device ID.
   Parameter_List[1] (optional) is the NWK address of the device whose node
                     descriptor is requested.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZDP_NodeDesc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_Status_t          Result;
   qapi_ZB_Handle_t       ZigBee_Handle;
   ZB_Device_ID_t        *TargetDevice;
   uint16_t               NWKAddrOfInterest;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      if(Parameter_Count >= 1)
      {
         TargetDevice = GetDeviceListEntry(Parameter_List[0].Integer_Value);
         if(TargetDevice != NULL)
         {
            if(TargetDevice->Type == QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E)
            {
               Ret_Val           = QCLI_STATUS_SUCCESS_E;
               NWKAddrOfInterest = TargetDevice->Address.ShortAddress;
               if(Parameter_Count >= 2)
               {
                  if(Verify_Integer_Parameter(&Parameter_List[1], 0, 0xFFFF))
                  {
                     NWKAddrOfInterest = (uint16_t)(Parameter_List[1].Integer_Value);
                  }
                  else
                  {
                     Ret_Val = QCLI_STATUS_USAGE_E;
                  }
               }

               if(Ret_Val == QCLI_STATUS_SUCCESS_E)
               {
                  Result = qapi_ZB_ZDP_Node_Desc_Req(ZigBee_Handle, TargetDevice->Address.ShortAddress, NWKAddrOfInterest);
                  if(Result == QAPI_OK)
                  {
                     Display_Function_Success(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Node_Desc_Req");
                     Ret_Val = QCLI_STATUS_SUCCESS_E;
                  }
                  else
                  {
                     Display_Function_Error(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Node_Desc_Req", Result);
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
               }
            }
            else
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Target device must be a NWK address.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Device ID is not valid.\n");
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "PowerDesc" command.

   Parameter_List[0] is the target device ID.
   Parameter_List[1] (optional) is the NWK address of the device whose power
                     descriptor is requested.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZDP_PowerDesc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_Status_t          Result;
   qapi_ZB_Handle_t       ZigBee_Handle;
   ZB_Device_ID_t        *TargetDevice;
   uint16_t               NWKAddrOfInterest;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      if(Parameter_Count >= 1)
      {
         TargetDevice = GetDeviceListEntry(Parameter_List[0].Integer_Value);
         if(TargetDevice != NULL)
         {
            if(TargetDevice->Type == QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E)
            {
               Ret_Val           = QCLI_STATUS_SUCCESS_E;
               NWKAddrOfInterest = TargetDevice->Address.ShortAddress;
               if(Parameter_Count >= 2)
               {
                  if(Verify_Integer_Parameter(&Parameter_List[1], 0, 0xFFFF))
                  {
                     NWKAddrOfInterest = (uint16_t)(Parameter_List[1].Integer_Value);
                  }
                  else
                  {
                     Ret_Val = QCLI_STATUS_USAGE_E;
                  }
               }

               if(Ret_Val == QCLI_STATUS_SUCCESS_E)
               {
                  Result = qapi_ZB_ZDP_Power_Desc_Req(ZigBee_Handle, TargetDevice->Address.ShortAddress, NWKAddrOfInterest);
                  if(Result == QAPI_OK)
                  {
                     Display_Function_Success(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Power_Desc_Req");
                     Ret_Val = QCLI_STATUS_SUCCESS_E;
                  }
                  else
                  {
                     Display_Function_Error(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Power_Desc_Req", Result);
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
               }
            }
            else
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Target device must be a NWK address.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Device ID is not valid.\n");
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SimpleDesc" command.

   Parameter_List[0] is the target device ID.
   Parameter_List[1] (optional) is the NWK address of the device whose simple
                     descriptor is requested.
   Parameter_List[2] (optional) is the endpoint on the device whose simple
                     descriptor is requested.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZDP_SimpleDesc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_Status_t          Result;
   qapi_ZB_Handle_t       ZigBee_Handle;
   ZB_Device_ID_t        *TargetDevice;
   uint16_t               NWKAddrOfInterest;
   uint8_t                EndPoint;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      if(Parameter_Count >= 1)
      {
         TargetDevice = GetDeviceListEntry(Parameter_List[0].Integer_Value);
         if(TargetDevice != NULL)
         {
            if(TargetDevice->Type == QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E)
            {
               Ret_Val           = QCLI_STATUS_SUCCESS_E;
               NWKAddrOfInterest = TargetDevice->Address.ShortAddress;
               EndPoint          = TargetDevice->Endpoint;
               if(Parameter_Count >= 2)
               {
                  if(Verify_Integer_Parameter(&Parameter_List[1], 0, 0xFFFF))
                  {
                     NWKAddrOfInterest = (uint16_t)(Parameter_List[1].Integer_Value);
                  }
                  else
                  {
                     Ret_Val = QCLI_STATUS_USAGE_E;
                  }
               }

               if(Parameter_Count >= 3)
               {
                  if(Verify_Integer_Parameter(&Parameter_List[2], 0, 0xFF))
                  {
                     EndPoint = (uint16_t)(Parameter_List[2].Integer_Value);
                  }
                  else
                  {
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
               }

               if(Ret_Val == QCLI_STATUS_SUCCESS_E)
               {
                  Result = qapi_ZB_ZDP_Simple_Desc_Req(ZigBee_Handle, TargetDevice->Address.ShortAddress, NWKAddrOfInterest, EndPoint);
                  if(Result == QAPI_OK)
                  {
                     Display_Function_Success(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Simple_Desc_Req");
                     Ret_Val = QCLI_STATUS_SUCCESS_E;
                  }
                  else
                  {
                     Display_Function_Error(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Simple_Desc_Req", Result);
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
               }
            }
            else
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Target device must be a NWK address.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Device ID is not valid.\n");
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "ActiveEP" command.

   Parameter_List[0] is the target device ID.
   Parameter_List[1] (optional) is the NWK address of the device whose power
                     descriptor is requested.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZDP_ActiveEP(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_Status_t          Result;
   qapi_ZB_Handle_t       ZigBee_Handle;
   ZB_Device_ID_t        *TargetDevice;
   uint16_t               NWKAddrOfInterest;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      if(Parameter_Count >= 1)
      {
         TargetDevice = GetDeviceListEntry(Parameter_List[0].Integer_Value);
         if(TargetDevice != NULL)
         {
            if(TargetDevice->Type == QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E)
            {
               Ret_Val           = QCLI_STATUS_SUCCESS_E;
               NWKAddrOfInterest = TargetDevice->Address.ShortAddress;
               if(Parameter_Count >= 2)
               {
                  if(Verify_Integer_Parameter(&Parameter_List[1], 0, 0xFFFF))
                  {
                     NWKAddrOfInterest = (uint16_t)(Parameter_List[1].Integer_Value);
                  }
                  else
                  {
                     Ret_Val = QCLI_STATUS_USAGE_E;
                  }
               }

               if(Ret_Val == QCLI_STATUS_SUCCESS_E)
               {
                  Result = qapi_ZB_ZDP_Active_EP_Req(ZigBee_Handle, TargetDevice->Address.ShortAddress, NWKAddrOfInterest);
                  if(Result == QAPI_OK)
                  {
                     Display_Function_Success(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Active_EP_Req");
                     Ret_Val = QCLI_STATUS_SUCCESS_E;
                  }
                  else
                  {
                     Display_Function_Error(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Active_EP_Req", Result);
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
               }
            }
            else
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Target device must be a NWK address.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Device ID is not valid.\n");
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "MatchDesc" command to issue a match descriptor request
          a specific cluster.

   Parameter_List[0] is the NWK addr of the destination.
   Parameter_List[1] is the type clusterID to match.
   Parameter_List[2] is a flag indicating if the cluster is a server (0) or
                     client (1).

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZDP_MatchDesc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t         Ret_Val;
   qapi_ZB_ZDP_Match_Desc_Req_t  Match_Request;
   qapi_Status_t                 Result;
   qapi_ZB_Handle_t              ZigBee_Handle;
   ZB_Device_ID_t              *TargetDevice;
   uint16_t                      ClusterId;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      if((Parameter_Count >= 4) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 1)))
      {
         TargetDevice = GetDeviceListEntry(Parameter_List[0].Integer_Value);
         if(TargetDevice != NULL)
         {
            if(TargetDevice->Type == QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E)
            {
               memset(&Match_Request, 0, sizeof(qapi_ZB_ZDP_Match_Desc_Req_t));

               ClusterId = (uint16_t)(Parameter_List[2].Integer_Value);

               if(Parameter_List[3].Integer_Value == 0)
               {
                  /* Server cluster. */
                  Match_Request.InClusterList = &ClusterId;
                  Match_Request.NumInClusters = 1;
               }
               else
               {
                  /* Client cluster. */
                  Match_Request.OutClusterList = &ClusterId;
                  Match_Request.NumOutClusters = 1;
               }

               Match_Request.NwkAddrOfInterest = (uint16_t)(Parameter_List[1].Integer_Value);
               Match_Request.ProfileID         = 0xFFFF;

               Result = qapi_ZB_ZDP_Match_Desc_Req(ZigBee_Handle, TargetDevice->Address.ShortAddress, &Match_Request);
               if(Result == QAPI_OK)
               {
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Finding Cluster 0x%04X.\n", ClusterId);
                  Ret_Val =  QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Match_Desc_Req", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Target device must be a NWK address.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Device ID is not valid.\n");
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "ComplexDesc" command.

   Parameter_List[0] is the target device ID.
   Parameter_List[1] (optional) is the NWK address of the device whose power
                     descriptor is requested.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZDP_ComplexDesc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_Status_t          Result;
   qapi_ZB_Handle_t       ZigBee_Handle;
   ZB_Device_ID_t        *TargetDevice;
   uint16_t               NWKAddrOfInterest;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      if(Parameter_Count >= 1)
      {
         TargetDevice = GetDeviceListEntry(Parameter_List[0].Integer_Value);
         if(TargetDevice != NULL)
         {
            if(TargetDevice->Type == QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E)
            {
               Ret_Val           = QCLI_STATUS_SUCCESS_E;
               NWKAddrOfInterest = TargetDevice->Address.ShortAddress;
               if(Parameter_Count >= 2)
               {
                  if(Verify_Integer_Parameter(&Parameter_List[1], 0, 0xFFFF))
                  {
                     NWKAddrOfInterest = (uint16_t)(Parameter_List[1].Integer_Value);
                  }
                  else
                  {
                     Ret_Val = QCLI_STATUS_USAGE_E;
                  }
               }

               if(Ret_Val == QCLI_STATUS_SUCCESS_E)
               {
                  Result = qapi_ZB_ZDP_Complex_Desc_Req(ZigBee_Handle, TargetDevice->Address.ShortAddress, NWKAddrOfInterest);
                  if(Result == QAPI_OK)
                  {
                     Display_Function_Success(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Complex_Desc_Req");
                     Ret_Val = QCLI_STATUS_SUCCESS_E;
                  }
                  else
                  {
                     Display_Function_Error(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Complex_Desc_Req", Result);
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
               }
            }
            else
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Target device must be a NWK address.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Device ID is not valid.\n");
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "UserDesc" command.

   Parameter_List[0] is the target device ID.
   Parameter_List[1] (optional) is the NWK address of the device whose power
                     descriptor is requested.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZDP_UserDesc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_Status_t          Result;
   qapi_ZB_Handle_t       ZigBee_Handle;
   ZB_Device_ID_t        *TargetDevice;
   uint16_t               NWKAddrOfInterest;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      if(Parameter_Count >= 1)
      {
         TargetDevice = GetDeviceListEntry(Parameter_List[0].Integer_Value);
         if(TargetDevice != NULL)
         {
            if(TargetDevice->Type == QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E)
            {
               Ret_Val           = QCLI_STATUS_SUCCESS_E;
               NWKAddrOfInterest = TargetDevice->Address.ShortAddress;
               if(Parameter_Count >= 2)
               {
                  if(Verify_Integer_Parameter(&Parameter_List[1], 0, 0xFFFF))
                  {
                     NWKAddrOfInterest = (uint16_t)(Parameter_List[1].Integer_Value);
                  }
                  else
                  {
                     Ret_Val = QCLI_STATUS_USAGE_E;
                  }
               }

               if(Ret_Val == QCLI_STATUS_SUCCESS_E)
               {
                  Result = qapi_ZB_ZDP_User_Desc_Req(ZigBee_Handle, TargetDevice->Address.ShortAddress, NWKAddrOfInterest);
                  if(Result == QAPI_OK)
                  {
                     Display_Function_Success(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_User_Desc_Req");
                     Ret_Val = QCLI_STATUS_SUCCESS_E;
                  }
                  else
                  {
                     Display_Function_Error(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_User_Desc_Req", Result);
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
               }
            }
            else
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Target device must be a NWK address.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Device ID is not valid.\n");
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "UserDescSet" command.

   Parameter_List[0] is the target device ID.
   Parameter_List[1] (optional) is the NWK address of the device whose power
                     descriptor is requested.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZDP_UserDescSet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_Status_t          Result;
   qapi_ZB_Handle_t       ZigBee_Handle;
   ZB_Device_ID_t        *TargetDevice;
   uint16_t               NWKAddrOfInterest;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      if(Parameter_Count >= 1)
      {
         TargetDevice = GetDeviceListEntry(Parameter_List[0].Integer_Value);
         if(TargetDevice != NULL)
         {
            if(TargetDevice->Type == QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E)
            {
               Ret_Val           = QCLI_STATUS_SUCCESS_E;
               NWKAddrOfInterest = TargetDevice->Address.ShortAddress;
               if(Parameter_Count >= 2)
               {
                  if(Verify_Integer_Parameter(&Parameter_List[1], 0, 0xFFFF))
                  {
                     NWKAddrOfInterest = (uint16_t)(Parameter_List[1].Integer_Value);
                  }
                  else
                  {
                     Ret_Val = QCLI_STATUS_USAGE_E;
                  }
               }

               if(Ret_Val == QCLI_STATUS_SUCCESS_E)
               {
                  Result = qapi_ZB_ZDP_User_Desc_Set(ZigBee_Handle, TargetDevice->Address.ShortAddress, NWKAddrOfInterest, USER_DESCRIPTOR_SET_SIZE, User_Desciptor_Set);
                  if(Result == QAPI_OK)
                  {
                     Display_Function_Success(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_User_Desc_Req");
                     Ret_Val = QCLI_STATUS_SUCCESS_E;
                  }
                  else
                  {
                     Display_Function_Error(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_User_Desc_Req", Result);
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
               }
            }
            else
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Target device must be a NWK address.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Device ID is not valid.\n");
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SysServerDisc" command.

   Parameter_List[0] is the server mask.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZDP_SysServerDisc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;
   qapi_Status_t         Result;
   qapi_ZB_Handle_t      ZigBee_Handle;
   uint16_t              ServerMask;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&Parameter_List[0], 0, 0xFFFF)))
      {
         ServerMask = (uint16_t)(Parameter_List[0].Integer_Value);

         Result = qapi_ZB_ZDP_System_Server_Discovery_Req(ZigBee_Handle, ServerMask);
         if(Result == QAPI_OK)
         {
            Display_Function_Success(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_System_Server_Discovery_Req");
            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }
         else
         {
            Display_Function_Error(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_System_Server_Discovery_Req", Result);
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
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "ExtSimpleDesc" command.

   Parameter_List[0] is the target device ID.
   Parameter_List[1] (optional) is the NWK address of the device whose simple
                     descriptor is requested.
   Parameter_List[2] (optional) is the endpoint on the device whose simple
                     descriptor is requested.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZDP_ExtSimpleDesc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_Status_t          Result;
   qapi_ZB_Handle_t       ZigBee_Handle;
   ZB_Device_ID_t        *TargetDevice;
   uint16_t               NWKAddrOfInterest;
   uint8_t                EndPoint;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      if(Parameter_Count >= 1)
      {
         TargetDevice = GetDeviceListEntry(Parameter_List[0].Integer_Value);
         if(TargetDevice != NULL)
         {
            if(TargetDevice->Type == QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E)
            {
               Ret_Val           = QCLI_STATUS_SUCCESS_E;
               NWKAddrOfInterest = TargetDevice->Address.ShortAddress;
               if(Parameter_Count >= 2)
               {
                  if(Verify_Integer_Parameter(&Parameter_List[1], 0, 0xFFFF))
                  {
                     NWKAddrOfInterest = (uint16_t)(Parameter_List[1].Integer_Value);
                  }
                  else
                  {
                     Ret_Val = QCLI_STATUS_USAGE_E;
                  }
               }

               if(Ret_Val == QCLI_STATUS_SUCCESS_E)
               {
                  if(Parameter_Count >= 3)
                  {
                     if(Verify_Integer_Parameter(&Parameter_List[2], 0, 0xFF))
                     {
                        EndPoint = (uint16_t)(Parameter_List[2].Integer_Value);
                     }
                     else
                     {
                        Ret_Val = QCLI_STATUS_ERROR_E;
                     }
                  }
                  else
                  {
                     EndPoint = TargetDevice->Endpoint;
                  }

                  if(Ret_Val == QCLI_STATUS_SUCCESS_E)
                  {
                     Result = qapi_ZB_ZDP_Extended_Simple_Desc_Req(ZigBee_Handle, TargetDevice->Address.ShortAddress, NWKAddrOfInterest, EndPoint, 0);
                     if(Result == QAPI_OK)
                     {
                        Display_Function_Success(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Extended_Simple_Desc_Req");
                        Ret_Val = QCLI_STATUS_SUCCESS_E;
                     }
                     else
                     {
                        Display_Function_Error(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Extended_Simple_Desc_Req", Result);
                        Ret_Val = QCLI_STATUS_ERROR_E;
                     }
                  }
               }
            }
            else
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Target device must be a NWK address.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Device ID is not valid.\n");
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "ExtActiveEP" command.

   Parameter_List[0] is the target device ID.
   Parameter_List[1] (optional) is the NWK address of the device whose active
                     endpoint is requested.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List  is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZDP_ExtActiveEP(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_Status_t          Result;
   qapi_ZB_Handle_t       ZigBee_Handle;
   ZB_Device_ID_t        *TargetDevice;
   uint16_t               NWKAddrOfInterest;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      if(Parameter_Count >= 1)
      {
         TargetDevice = GetDeviceListEntry(Parameter_List[0].Integer_Value);
         if(TargetDevice != NULL)
         {
            if(TargetDevice->Type == QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E)
            {
               Ret_Val           = QCLI_STATUS_SUCCESS_E;
               NWKAddrOfInterest = TargetDevice->Address.ShortAddress;
               if(Parameter_Count >= 2)
               {
                  if(Verify_Integer_Parameter(&Parameter_List[1], 0, 0xFFFF))
                  {
                     NWKAddrOfInterest = (uint16_t)(Parameter_List[1].Integer_Value);
                  }
                  else
                  {
                     Ret_Val = QCLI_STATUS_USAGE_E;
                  }
               }

               if(Ret_Val == QCLI_STATUS_SUCCESS_E)
               {
                  Result = qapi_ZB_ZDP_Extended_Active_EP_Req(ZigBee_Handle, TargetDevice->Address.ShortAddress, NWKAddrOfInterest, 0);
                  if(Result == QAPI_OK)
                  {
                     Display_Function_Success(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Extended_Active_EP_Req");
                     Ret_Val = QCLI_STATUS_SUCCESS_E;
                  }
                  else
                  {
                     Display_Function_Error(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Extended_Active_EP_Req", Result);
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
               }
            }
            else
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Target device must be a NWK address.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Device ID is not valid.\n");
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "EndBind" to create a binding between two endpoints.

   Parameter_List[0] is the device ID for the target device.  The address for
                     this device be a network address.
   Parameter_List[1] is the device ID for the source device of the bind.  The
                     address for this device be an extended address.
   Parameter_List[2] is the Cluster ID for the bind.
   Parameter_List[3] is a flag indicating if it is a server or client cluster.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZDP_EndBind(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t              Ret_Val;
   qapi_ZB_ZDP_End_Device_Bind_Req_t  Request;
   qapi_Status_t                      Result;
   qapi_ZB_Handle_t                   ZigBee_Handle;
   ZB_Device_ID_t                    *TargetDevice;
   ZB_Device_ID_t                    *SrcDevice;
   uint16_t                           ClusterID;
   qbool_t                            IsServer;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 4) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0x0000, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 1)))
      {
         /* Get the device information. */
         TargetDevice = GetDeviceListEntry(Parameter_List[0].Integer_Value);
         SrcDevice    = GetDeviceListEntry(Parameter_List[1].Integer_Value);
         ClusterID    = (uint16_t)(Parameter_List[2].Integer_Value);
         IsServer     = (qbool_t)(Parameter_List[3].Integer_Value != 0);

         if((TargetDevice != NULL) && (SrcDevice != NULL))
         {
            /* Set Request to a known state. */
            memset((void *)&Request, 0 , sizeof(qapi_ZB_ZDP_End_Device_Bind_Req_t));

            /* Verify the address types for each specified device is valid. */
            if((TargetDevice->Type == QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E) &&
               (SrcDevice->Type == QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E))
            {
               /* Set up the Request. */
               Request.BindingTarget  = TargetDevice->Address.ShortAddress;
               Request.SrcIEEEAddress = SrcDevice->Address.ExtendedAddress;
               Request.SrcEndpoint    = SrcDevice->Endpoint;
               Request.ProfileID      = QAPI_ZB_CL_PROFILE_ID_HOME_AUTOMATION;

               if(IsServer)
               {
                  Request.NumInClusters  = 1;
                  Request.InClusterList  = &ClusterID;
               }
               else
               {
                  Request.NumOutClusters = 1;
                  Request.OutClusterList = &ClusterID;
               }

               /* Issue the bind request. */
               Result = qapi_ZB_ZDP_End_Device_Bind_Req(ZigBee_Handle, &Request);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_End_Device_Bind_Req");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_End_Device_Bind_Req", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Specified device is not a valid type.\n");
               Ret_Val = QCLI_STATUS_USAGE_E;
            }
         }
         else
         {
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Device ID is not valid.\n");
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "Bind" to create a binding between two endpoints.

   Parameter_List[0] is the device ID for the target device.  The address for
                     this device be a network address.
   Parameter_List[1] is the device ID for the source device of the bind.  The
                     address for this device be an extended address.
   Parameter_List[2] is the device ID for the destination device of the bind.
                     The address for this device be a group or extended address.
   Parameter_List[3] is the cluster ID for the bind.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZDP_Bind(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t   Ret_Val;
   qapi_ZB_ZDP_Bind_Req_t  Request;
   qapi_Status_t           Result;
   qapi_ZB_Handle_t        ZigBee_Handle;
   ZB_Device_ID_t         *TargetDevice;
   ZB_Device_ID_t         *SrcDevice;
   ZB_Device_ID_t         *DstDevice;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 4) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0x0000, 0xFFFF)))
      {
         /* Get the device information. */
         TargetDevice = GetDeviceListEntry(Parameter_List[0].Integer_Value);
         SrcDevice    = GetDeviceListEntry(Parameter_List[1].Integer_Value);
         DstDevice    = GetDeviceListEntry(Parameter_List[2].Integer_Value);

         if((TargetDevice != NULL) && (SrcDevice != NULL) && (DstDevice != NULL))
         {
            /* Set Request to a known state. */
            memset((void *)&Request, 0 , sizeof(qapi_ZB_ZDP_Bind_Req_t));

            /* Verify the address types for each specified device is valid. */
            if((TargetDevice->Type == QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E) &&
               (SrcDevice->Type == QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E) &&
               ((DstDevice->Type == QAPI_ZB_ADDRESS_MODE_GROUP_ADDRESS_E) || (DstDevice->Type == QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E)))
            {
               /* Set up the Request. */
               Request.DestNwkAddr          = TargetDevice->Address.ShortAddress;
               Request.BindData.SrcAddress  = SrcDevice->Address.ExtendedAddress;
               Request.BindData.SrcEndpoint = SrcDevice->Endpoint;
               Request.BindData.ClusterId   = (uint16_t)(Parameter_List[3].Integer_Value);
               Request.BindData.DstAddrMode = DstDevice->Type;
               Request.BindData.DstAddress  = DstDevice->Address;
               Request.BindData.DstEndpoint = DstDevice->Endpoint;

               /* Issue the bind request. */
               Result = qapi_ZB_ZDP_Bind_Req(ZigBee_Handle, &Request);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Bind_Req");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Bind_Req", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Specified device is not a valid type.\n");
               Ret_Val = QCLI_STATUS_USAGE_E;
            }
         }
         else
         {
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Device ID is not valid.\n");
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "UnbindRequest" to unbind two endpoints.

   Parameter_List[0] is the device ID for the target device.  The address for
                     this device be a network address.
   Parameter_List[1] is the device ID for the source device of the bind.  The
                     address for this device be an extended address.
   Parameter_List[2] is the device ID for the destination device of the bind.
                     The address for this device be a group or extended address.
   Parameter_List[3] is the cluster ID for the bind.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZDP_UnBind(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t     Ret_Val;
   qapi_ZB_ZDP_Unbind_Req_t  Request;
   qapi_Status_t             Result;
   qapi_ZB_Handle_t          ZigBee_Handle;
   ZB_Device_ID_t           *TargetDevice;
   ZB_Device_ID_t           *SrcDevice;
   ZB_Device_ID_t           *DstDevice;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 4) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0x0000, 0xFFFF)))
      {
         /* Get the device information. */
         TargetDevice = GetDeviceListEntry(Parameter_List[0].Integer_Value);
         SrcDevice    = GetDeviceListEntry(Parameter_List[1].Integer_Value);
         DstDevice    = GetDeviceListEntry(Parameter_List[2].Integer_Value);

         if((TargetDevice != NULL) && (SrcDevice != NULL) && (DstDevice != NULL))
         {
            /* Set Request to a known state. */
            memset((void *)&Request, 0 , sizeof(qapi_ZB_ZDP_Bind_Req_t));

            /* Verify the address types for each specified device is valid. */
            if((TargetDevice->Type == QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E) &&
               (SrcDevice->Type == QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E) &&
               ((DstDevice->Type == QAPI_ZB_ADDRESS_MODE_GROUP_ADDRESS_E) || (DstDevice->Type == QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E)))
            {
               /* Set up the Request. */
               Request.DestNwkAddr            = TargetDevice->Address.ShortAddress;
               Request.UnbindData.SrcAddress  = SrcDevice->Address.ExtendedAddress;
               Request.UnbindData.SrcEndpoint = SrcDevice->Endpoint;
               Request.UnbindData.ClusterId   = (uint16_t)(Parameter_List[3].Integer_Value);
               Request.UnbindData.DstAddrMode = DstDevice->Type;
               Request.UnbindData.DstAddress  = DstDevice->Address;
               Request.UnbindData.DstEndpoint = DstDevice->Endpoint;

               /* Issue the unbind request. */
               Result = qapi_ZB_ZDP_Unbind_Req(ZigBee_Handle, &Request);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Unbind_Req");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Unbind_Req", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Specified device is not a valid type.\n");
               Ret_Val = QCLI_STATUS_USAGE_E;
            }
         }
         else
         {
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Device ID is not valid.\n");
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "MgmtLqi" command.

   Parameter_List[0] is the device ID for the target device. The address for
                     this device be a network address.
   Parameter_List[1] is the start index.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZDP_MgmtLqi(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_Status_t          Result;
   qapi_ZB_Handle_t       ZigBee_Handle;
   ZB_Device_ID_t        *TargetDevice;
   uint8_t                StartIndex;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      if((Parameter_Count >= 2) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 0xFF)))
      {
         TargetDevice = GetDeviceListEntry(Parameter_List[0].Integer_Value);
         if(TargetDevice != NULL)
         {
            if(TargetDevice->Type == QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E)
            {
               StartIndex = (uint8_t)(Parameter_List[1].Integer_Value);

               Result = qapi_ZB_ZDP_Mgmt_Lqi_Req(ZigBee_Handle, TargetDevice->Address.ShortAddress, StartIndex);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Mgmt_Lqi_Req");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Mgmt_Lqi_Req", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Target device must be a NWK address.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Device ID is not valid.\n");
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "MgmtRtg" command.

   Parameter_List[0] is the device ID for the target device. The address for
                     this device be a network address.
   Parameter_List[1] is the start index.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZDP_MgmtRtg(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_Status_t          Result;
   qapi_ZB_Handle_t       ZigBee_Handle;
   ZB_Device_ID_t        *TargetDevice;
   uint8_t                StartIndex;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      if((Parameter_Count >= 2) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 0xFF)))
      {
         TargetDevice = GetDeviceListEntry(Parameter_List[0].Integer_Value);
         if(TargetDevice != NULL)
         {
            if(TargetDevice->Type == QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E)
            {
               StartIndex = (uint8_t)(Parameter_List[1].Integer_Value);

               Result = qapi_ZB_ZDP_Mgmt_Rtg_Req(ZigBee_Handle, TargetDevice->Address.ShortAddress, StartIndex);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Mgmt_Rtg_Req");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Mgmt_Rtg_Req", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Target device must be a NWK address.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Device ID is not valid.\n");
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "MgmtBind" command.

   Parameter_List[0] is the device ID for the target device. The address for
                     this device be a network address.
   Parameter_List[1] is the start index.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZDP_MgmtBind(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_Status_t          Result;
   qapi_ZB_Handle_t       ZigBee_Handle;
   ZB_Device_ID_t        *TargetDevice;
   uint8_t                StartIndex;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      if((Parameter_Count >= 2) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 0xFF)))
      {
         TargetDevice = GetDeviceListEntry(Parameter_List[0].Integer_Value);
         if(TargetDevice != NULL)
         {
            if(TargetDevice->Type == QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E)
            {
               StartIndex = (uint8_t)(Parameter_List[1].Integer_Value);

               Result = qapi_ZB_ZDP_Mgmt_Bind_Req(ZigBee_Handle, TargetDevice->Address.ShortAddress, StartIndex);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Mgmt_Bind_Req");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Mgmt_Bind_Req", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Target device must be a NWK address.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Device ID is not valid.\n");
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "MgmtLeave" command.

   Parameter_List[0] is the device ID for the target device. The address for
                     this device be a network address.
   Parameter_List[1] is the IEEEAddr of the device being removed.
   Parameter_List[2] is the flag indicates if the children will be removed.
                     0=Remove
                     1=Kepp
   Parameter_List[3] is the flag indicates if the rejoin is required.
                     0=No
                     1=Yes

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZDP_MgmtLeave(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_Status_t          Result;
   qapi_ZB_Handle_t       ZigBee_Handle;
   uint64_t               ExtAddr;
   ZB_Device_ID_t        *TargetDevice;
   qbool_t                RemoveChildren;
   qbool_t                Rejoin;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      if((Parameter_Count >= 4) &&
         (Hex_String_To_ULL(Parameter_List[1].String_Value, &ExtAddr)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 1)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 1)))
      {
         TargetDevice = GetDeviceListEntry(Parameter_List[0].Integer_Value);
         if(TargetDevice != NULL)
         {
            if(TargetDevice->Type == QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E)
            {
               RemoveChildren = (qbool_t)(Parameter_List[2].Integer_Value);
               Rejoin         = (qbool_t)(Parameter_List[3].Integer_Value);

               Result = qapi_ZB_ZDP_Mgmt_Leave_Req(ZigBee_Handle, TargetDevice->Address.ShortAddress, ExtAddr, RemoveChildren, Rejoin);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Mgmt_Leave_Req");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Mgmt_Leave_Req", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Target device must be a NWK address.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Device ID is not valid.\n");
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "MgmtPermitJoining" command.

   Parameter_List[0] is the device ID for the target device. The address for
                     this device be a network address.
   Parameter_List[1] is the permit duration.
   Parameter_List[2] is the flag indicates if the rejoin request will affect
                     trust center if the remote device is the trust center.
                     0=No
                     1=Yes

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZDP_MgmtPermitJoining(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_Status_t          Result;
   qapi_ZB_Handle_t       ZigBee_Handle;
   ZB_Device_ID_t        *TargetDevice;
   uint8_t                PermitDuration;
   qbool_t                TC_Significance;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      if((Parameter_Count >= 3) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 1)))
      {
         TargetDevice = GetDeviceListEntry(Parameter_List[0].Integer_Value);
         if(TargetDevice != NULL)
         {
            if(TargetDevice->Type == QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E)
            {
               if((TargetDevice->Address.ShortAddress == QAPI_ZB_NWK_ADDRESS_BCAST_ROUTERS) || (TargetDevice->Address.ShortAddress == QAPI_ZB_NWK_ADDRESS_BCAST_RX_ON_WHEN_IDLE))
               {
                  PermitDuration  = (uint8_t)(Parameter_List[1].Integer_Value);
                  TC_Significance = (qbool_t)(Parameter_List[2].Integer_Value);

                  Result = qapi_ZB_ZDP_Mgmt_Permit_Joining_Req(ZigBee_Handle, TargetDevice->Address.ShortAddress, PermitDuration, TC_Significance);
                  if(Result == QAPI_OK)
                  {
                     Display_Function_Success(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Mgmt_Permit_Joining_Req");
                     Ret_Val = QCLI_STATUS_SUCCESS_E;
                  }
                  else
                  {
                     Display_Function_Error(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Mgmt_Permit_Joining_Req", Result);
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
               }
               else
               {
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "The target device should have a broadcast address.\n");
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Target device must be a NWK address.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Device ID is not valid.\n");
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "ZMgmtNWKUpdate" command.

   Parameter_List[0] is the device ID for the target device. The address for
                     this device be a network address.
   Parameter_List[1] is the scan channels.
   Parameter_List[2] is the scan duration.
   Parameter_List[3] is the scan count. Only if the scan duration is within the
                     range of 0x00 to 0x05

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZDP_MgmtNWKUpdate(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t              Ret_Val;
   qapi_Status_t                      Result;
   qapi_ZB_Handle_t                   ZigBee_Handle;
   uint16_t                           AttrLength;
   uint8_t                            AttrValue[MAXIMUM_ATTRIUBTE_LENGTH];
   qapi_ZB_ZDP_Mgmt_Nwk_Update_Req_t  Request;
   ZB_Device_ID_t                    *TargetDevice;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      if((Parameter_Count >= 3) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 0x07FFF800)) &&
         ((Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0x05)) || (Verify_Integer_Parameter(&Parameter_List[2], 0xFE, 0xFF))))
      {
         TargetDevice = GetDeviceListEntry(Parameter_List[0].Integer_Value);
         if(TargetDevice != NULL)
         {
            if(TargetDevice->Type == QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E)
            {
               /* Set Request to a known state. */
               memset(&Request, 0, sizeof(qapi_ZB_ZDP_Mgmt_Nwk_Update_Req_t));

               Request.DstNwkAddr   = TargetDevice->Address.ShortAddress;
               Request.ScanChannels = (uint32_t)(Parameter_List[1].Integer_Value);
               Request.ScanDuration = (uint8_t)(Parameter_List[2].Integer_Value);

               Ret_Val = QCLI_STATUS_SUCCESS_E;

               /* Determine if the scan count should present. */
               if((Request.ScanDuration >= 0x00) && (Request.ScanDuration <= 0x05))
               {
                  if((Parameter_Count >= 4) &&
                     (Verify_Integer_Parameter(&Parameter_List[3], 0, 0xFF)))
                  {
                     Request.ScanCount = (uint8_t)(Parameter_List[3].Integer_Value);
                  }
                  else
                  {
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "The scan count is needed.\n");
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
               }

               /* Determine if we should get NwkUpdateId and NwkManagerAddr based
                  on value of ScanDuration. */
               if(Request.ScanDuration == 0xFE)
               {
                  AttrLength = sizeof(uint8_t);
                  Result     = qapi_ZB_NLME_Get_Request(ZigBee_Handle, QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_UPDATE_ID_E, 0, &AttrLength, AttrValue);
                  if(Result == QAPI_OK)
                  {
                     Request.NwkUpdateId = READ_UNALIGNED_LITTLE_ENDIAN_UINT8(AttrValue);
                  }
                  else
                  {
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Can not get the NWK update ID.\n");
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
               }

               if(Request.ScanDuration == 0xFF)
               {
                  AttrLength = sizeof(uint16_t);
                  Result     = qapi_ZB_NLME_Get_Request(ZigBee_Handle, QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_MANAGER_ADDR_E, 0, &AttrLength, AttrValue);
                  if(Result == QAPI_OK)
                  {
                     Request.NwkManagerAddr = READ_UNALIGNED_LITTLE_ENDIAN_UINT16(AttrValue);
                  }
                  else
                  {
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Can not get the NWK manager address.\n");
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
               }

               if(Ret_Val == QCLI_STATUS_SUCCESS_E)
               {
                  /* Send out the NWK update request. */
                  Result = qapi_ZB_ZDP_Mgmt_Nwk_Update_Req(ZigBee_Handle, &Request);
                  if(Result == QAPI_OK)
                  {
                     Display_Function_Success(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Mgmt_Nwk_Update_Req");
                     Ret_Val = QCLI_STATUS_SUCCESS_E;
                  }
                  else
                  {
                     Display_Function_Error(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Mgmt_Nwk_Update_Req", Result);
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
               }
            }
            else
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Target device must be a NWK address.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }
         }
         else
         {
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Device ID is not valid.\n");
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SetLocalComplexDesc" command.

   @Parameter_List[0] ManName   is string of manufacture name.
   @Parameter_List[1] ModleName is string of model name.
   @Parameter_List[2] SerialNum is string of serial number.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZDP_SetLocalComplexDesc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_Status_t          Result;
   qapi_ZB_Handle_t       ZigBee_Handle;
   uint8_t               *ManufacturerName;
   uint8_t               *ModelName;
   uint8_t               *SerialNumber;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      if(Parameter_Count >= 3)
      {
         ManufacturerName = (uint8_t *)(Parameter_List[0].String_Value);
         ModelName        = (uint8_t *)(Parameter_List[1].String_Value);
         SerialNumber     = (uint8_t *)(Parameter_List[2].String_Value);

         Result = qapi_ZB_ZDP_Set_Local_Complex_Desc(ZigBee_Handle, ManufacturerName, ModelName, SerialNumber);
         if(Result == QAPI_OK)
         {
            Display_Function_Success(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Set_Local_Complex_Desc");
            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }
         else
         {
            Display_Function_Error(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Set_Local_Complex_Desc", Result);
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
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SetLocalUserDesc" command.

   @Parameter_List[0] ManName is string of manufacture name.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZDP_SetLocalUserDesc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_Status_t          Result;
   qapi_ZB_Handle_t       ZigBee_Handle;
   uint8_t                DescLength;
   uint8_t               *Desc;

   ZigBee_Handle = GetZigBeeHandle();
   if(ZigBee_Handle != NULL)
   {
      if(Parameter_Count >= 1)
      {
         DescLength = strlen(Parameter_List[0].String_Value);
         Desc       = (uint8_t *)(Parameter_List[0].String_Value);

         Result = qapi_ZB_ZDP_Set_Local_User_Desc(ZigBee_Handle, DescLength, Desc);
         if(Result == QAPI_OK)
         {
            Display_Function_Success(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Set_Local_User_Desc");
            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }
         else
         {
            Display_Function_Error(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Set_Local_User_Desc", Result);
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
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Callback handler for the ZDP event.

   @param ZB_Handle      is the handle of the ZigBee instance.
   @param ZDP_Event_Data is the information for the event.
   @param CB_Param       is the user specified parameter for the callback
                         function.
*/
static void ZB_ZDP_Event_CB(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_ZDP_Event_t *ZDP_Event_Data, uint32_t CB_Param)
{
   qapi_ZB_Handle_t ZigBee_Handle;
   uint8_t          Index;
   uint8_t          OutputListIndex;

   ZigBee_Handle = GetZigBeeHandle();
   if((ZB_Handle == ZigBee_Handle) && (ZDP_Event_Data != NULL))
   {
      switch(ZDP_Event_Data->Event_Type)
      {
         case QAPI_ZB_ZDP_EVENT_TYPE_SYSTEM_SERVER_DISCOVERY_RSP_E:
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "System Server Discover Response:\n");
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Status:     %d\n", ZDP_Event_Data->Event_Data.System_Server_Discovery_Rsp.Status);
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  ServerMask: %d\n", ZDP_Event_Data->Event_Data.System_Server_Discovery_Rsp.ServerMask);
            break;

         case QAPI_ZB_ZDP_EVENT_TYPE_NWK_ADDR_RSP_E:
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "NWK Address Discover Response:\n");
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Status:            %d\n", ZDP_Event_Data->Event_Data.Nwk_Addr_Rsp.Status);
            if(ZDP_Event_Data->Event_Data.Nwk_Addr_Rsp.Status == QAPI_OK)
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  IEEEAddrRemoteDev: 0x%08X%08X\n", (unsigned int)(ZDP_Event_Data->Event_Data.Nwk_Addr_Rsp.IEEEAddrRemoteDev >> 32), (unsigned int)(ZDP_Event_Data->Event_Data.Nwk_Addr_Rsp.IEEEAddrRemoteDev));
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  NwkAddrRemoteDev:  0x%04X\n", ZDP_Event_Data->Event_Data.Nwk_Addr_Rsp.NwkAddrRemoteDev);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  NumAssocDev:       %d\n", ZDP_Event_Data->Event_Data.Nwk_Addr_Rsp.NumAssocDev);
               if((ZDP_Event_Data->Event_Data.Nwk_Addr_Rsp.NumAssocDev > 0) && (ZDP_Event_Data->Event_Data.Nwk_Addr_Rsp.NwkAddrAssocDevList != NULL))
               {
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "    StartIndex: %d\n", ZDP_Event_Data->Event_Data.Nwk_Addr_Rsp.StartIndex);
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "    AssociatedDeviceList:\n");
                  for(Index = 0; Index < ZDP_Event_Data->Event_Data.Nwk_Addr_Rsp.NumAssocDev; Index++)
                  {
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "    0x%04X ", ZDP_Event_Data->Event_Data.Nwk_Addr_Rsp.NwkAddrAssocDevList[Index]);
                  }
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "\n");
               }
            }
            break;

         case QAPI_ZB_ZDP_EVENT_TYPE_IEEE_ADDR_RSP_E:
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "IEEE Address Discover Response:\n");
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Status:            %d\n", ZDP_Event_Data->Event_Data.IEEE_Addr_Rsp.Status);
            if(ZDP_Event_Data->Event_Data.IEEE_Addr_Rsp.Status == QAPI_OK)
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  IEEEAddrRemoteDev: 0x%08X%08X\n", (unsigned int)(ZDP_Event_Data->Event_Data.IEEE_Addr_Rsp.IEEEAddrRemoteDev >> 32), (unsigned int)(ZDP_Event_Data->Event_Data.IEEE_Addr_Rsp.IEEEAddrRemoteDev));
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  NwkAddrRemoteDev:  0x%04X\n", ZDP_Event_Data->Event_Data.IEEE_Addr_Rsp.NwkAddrRemoteDev);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  NumAssocDev:       %d\n", ZDP_Event_Data->Event_Data.IEEE_Addr_Rsp.NumAssocDev);
               if((ZDP_Event_Data->Event_Data.IEEE_Addr_Rsp.NumAssocDev > 0) && (ZDP_Event_Data->Event_Data.IEEE_Addr_Rsp.NwkAddrAssocDevList != NULL))
               {
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "    StartIndex: %d\n", ZDP_Event_Data->Event_Data.IEEE_Addr_Rsp.StartIndex);
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "    NWKAddrofAssocauedDevice:\n");
                  for(Index = 0; Index < ZDP_Event_Data->Event_Data.Nwk_Addr_Rsp.NumAssocDev; Index++)
                  {
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "    0x%04X ", ZDP_Event_Data->Event_Data.IEEE_Addr_Rsp.NwkAddrAssocDevList[Index]);
                  }
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "\n");
               }
            }
            break;

         case QAPI_ZB_ZDP_EVENT_TYPE_SIMPLE_DESC_RSP_E:
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Simple Descriptor Response:\n");
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Status:             %d\n", ZDP_Event_Data->Event_Data.Simple_Desc_Rsp.Status);
            if(ZDP_Event_Data->Event_Data.Simple_Desc_Rsp.Status == QAPI_OK)
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  NwkAddrOfInterest:  0x%04X\n", ZDP_Event_Data->Event_Data.Simple_Desc_Rsp.NwkAddrOfInterest);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Endpoint:           %d\n", ZDP_Event_Data->Event_Data.Simple_Desc_Rsp.SimpleDescriptor.Endpoint);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  ProfileID:          0x%04X\n", ZDP_Event_Data->Event_Data.Simple_Desc_Rsp.SimpleDescriptor.ProfileIdentifier);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  DeviceID:           0x%04X\n", ZDP_Event_Data->Event_Data.Simple_Desc_Rsp.SimpleDescriptor.DeviceIdentifier);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  DeviceVersion:      %d\n", ZDP_Event_Data->Event_Data.Simple_Desc_Rsp.SimpleDescriptor.DeviceVersion);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  InputClusterCount:  %d\n", ZDP_Event_Data->Event_Data.Simple_Desc_Rsp.SimpleDescriptor.InputClusterCount);
               if((ZDP_Event_Data->Event_Data.Simple_Desc_Rsp.SimpleDescriptor.InputClusterCount > 0) && (ZDP_Event_Data->Event_Data.Simple_Desc_Rsp.SimpleDescriptor.InputClusterList != NULL))
               {
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  InputCluster:");
                  for(Index = 0; Index < ZDP_Event_Data->Event_Data.Simple_Desc_Rsp.SimpleDescriptor.InputClusterCount; Index++)
                  {
                     if((Index % 6) == 0)
                     {
                        QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "\n    ");
                     }
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  0x%04X", ZDP_Event_Data->Event_Data.Simple_Desc_Rsp.SimpleDescriptor.InputClusterList[Index]);
                  }
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "\n");
               }
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  OutputClusterCount: %d\n", ZDP_Event_Data->Event_Data.Simple_Desc_Rsp.SimpleDescriptor.OutputClusterCount);
               if((ZDP_Event_Data->Event_Data.Simple_Desc_Rsp.SimpleDescriptor.OutputClusterCount > 0) && (ZDP_Event_Data->Event_Data.Simple_Desc_Rsp.SimpleDescriptor.OutputClusterList != NULL))
               {
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  OutputCluster:");
                  for(Index = 0; Index < ZDP_Event_Data->Event_Data.Simple_Desc_Rsp.SimpleDescriptor.OutputClusterCount; Index++)
                  {
                     if((Index % 6) == 0)
                     {
                        QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "\n    ");
                     }
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  0x%04X", ZDP_Event_Data->Event_Data.Simple_Desc_Rsp.SimpleDescriptor.OutputClusterList[Index]);
                  }
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "\n");
               }
            }
            break;

         case QAPI_ZB_ZDP_EVENT_TYPE_EXTENDED_SIMPLE_DESC_RSP_E:
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Extended Simple Descriptor Response:\n");
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Status:             %d\n", ZDP_Event_Data->Event_Data.Extended_Simple_Desc_Rsp.Status);
            if(ZDP_Event_Data->Event_Data.Extended_Simple_Desc_Rsp.Status == QAPI_OK)
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  NwkAddrOfInterest:  0x%04X\n", ZDP_Event_Data->Event_Data.Extended_Simple_Desc_Rsp.NwkAddrOfInterest);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Endpoint:           %d\n", ZDP_Event_Data->Event_Data.Extended_Simple_Desc_Rsp.Endpoint);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  InputClusterCount:  %d\n", ZDP_Event_Data->Event_Data.Extended_Simple_Desc_Rsp.AppInputClusterCount);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  OutputClusterCount: %d\n", ZDP_Event_Data->Event_Data.Extended_Simple_Desc_Rsp.AppOutputClusterCount);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  StartIndex:         %d\n", ZDP_Event_Data->Event_Data.Extended_Simple_Desc_Rsp.StartIndex);

               /* The inputlist is at the front section of the AppClusterList.
                  So it is the first to be displayed. */
               if((ZDP_Event_Data->Event_Data.Extended_Simple_Desc_Rsp.AppInputClusterCount > 0) && (ZDP_Event_Data->Event_Data.Extended_Simple_Desc_Rsp.AppClusterList != NULL))
               {
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  InputCluster:");
                  for(Index = 0; Index < ZDP_Event_Data->Event_Data.Extended_Simple_Desc_Rsp.AppInputClusterCount; Index++)
                  {
                     if((Index % 6) == 0)
                     {
                        QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "\n    ");
                     }
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  0x%04X", ZDP_Event_Data->Event_Data.Extended_Simple_Desc_Rsp.AppClusterList[Index]);
                  }
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "\n");

                  OutputListIndex = Index;
               }
               else
               {
                  OutputListIndex = 0;
               }

               /* Now we should display the output cluster list. */
               if((ZDP_Event_Data->Event_Data.Extended_Simple_Desc_Rsp.AppOutputClusterCount > 0) && (ZDP_Event_Data->Event_Data.Extended_Simple_Desc_Rsp.AppClusterList != NULL))
               {
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  OutputCluster:");

                  /* The OutputListIndex value will depend on if
                     AppInputClusterCount is zero or not. */
                  for(Index = 0; Index < ZDP_Event_Data->Event_Data.Extended_Simple_Desc_Rsp.AppOutputClusterCount; Index++)
                  {
                     if((Index % 6) == 0)
                     {
                        QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "\n    ");
                     }
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  0x%04X", ZDP_Event_Data->Event_Data.Extended_Simple_Desc_Rsp.AppClusterList[OutputListIndex]);
                     OutputListIndex++;
                  }
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "\n");
               }
            }
            break;

         case QAPI_ZB_ZDP_EVENT_TYPE_COMPLEX_DESC_RSP_E:
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Complex Descriptor Response:\n");
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Status:            %d\n", ZDP_Event_Data->Event_Data.Complex_Desc_Rsp.Status);
            if(ZDP_Event_Data->Event_Data.Complex_Desc_Rsp.Status == QAPI_OK)
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  NwkAddrOfInterest: 0x%04X\n", ZDP_Event_Data->Event_Data.Complex_Desc_Rsp.NwkAddrOfInterest);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  ResponseLength:    %d\n", ZDP_Event_Data->Event_Data.Complex_Desc_Rsp.Length);

               /* Parse the complex descriptor. */
               DisplayComplexDesc(ZDP_Event_Data->Event_Data.Complex_Desc_Rsp.Length, ZDP_Event_Data->Event_Data.Complex_Desc_Rsp.ComplexDescriptor);
            }
            break;

         case QAPI_ZB_ZDP_EVENT_TYPE_USER_DESC_RSP_E:
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "User Descriptor Response:\n");
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Status:            %d\n", ZDP_Event_Data->Event_Data.User_Desc_Rsp.Status);
            if(ZDP_Event_Data->Event_Data.User_Desc_Rsp.Status == QAPI_OK)
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  NwkAddrOfInterest: 0x%04X\n", ZDP_Event_Data->Event_Data.User_Desc_Rsp.NwkAddrOfInterest);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  ResponseLength:    %d\n", ZDP_Event_Data->Event_Data.User_Desc_Rsp.Length);
               if((ZDP_Event_Data->Event_Data.User_Desc_Rsp.Length > 0) && (ZDP_Event_Data->Event_Data.User_Desc_Rsp.UserDescriptor != NULL))
               {
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  UserDescriptor:");
                  for(Index = 0; Index < (ZDP_Event_Data->Event_Data.User_Desc_Rsp.Length); Index++)
                  {
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, " 0x%02X", ZDP_Event_Data->Event_Data.User_Desc_Rsp.UserDescriptor[Index]);
                  }
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "\n");
               }
            }
            break;

         case QAPI_ZB_ZDP_EVENT_TYPE_USER_DESC_CONF_E:
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Set User Descriptor Confirm:\n");
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Status:            %d\n", ZDP_Event_Data->Event_Data.User_Desc_Conf.Status);
            if(ZDP_Event_Data->Event_Data.User_Desc_Conf.Status == QAPI_OK)
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  NwkAddrOfInterest: 0x%04X\n", ZDP_Event_Data->Event_Data.User_Desc_Conf.NwkAddrOfInterest);
            }
            break;

         case QAPI_ZB_ZDP_EVENT_TYPE_POWER_DESC_RSP_E:
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Power Descriptor Response:\n");
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Status:                  %d\n", ZDP_Event_Data->Event_Data.Power_Desc_Rsp.Status);
            if(ZDP_Event_Data->Event_Data.Power_Desc_Rsp.Status == QAPI_OK)
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  NwkAddrOfInterest:       0x%04X\n", ZDP_Event_Data->Event_Data.Power_Desc_Rsp.NwkAddrOfInterest);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  PowerMode:               %d\n", ZDP_Event_Data->Event_Data.Power_Desc_Rsp.PowerDescriptor.CurrentPowerMode);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  AvailablePowerSource:    %d\n", ZDP_Event_Data->Event_Data.Power_Desc_Rsp.PowerDescriptor.AvailablePowerSources);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  CurrentPowerSource:      %d\n", ZDP_Event_Data->Event_Data.Power_Desc_Rsp.PowerDescriptor.CurrentPowerSource);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  CurrentPowerSourceLevel: %d\n", ZDP_Event_Data->Event_Data.Power_Desc_Rsp.PowerDescriptor.CurrentPowerSourceLevel);
            }
            break;

          case QAPI_ZB_ZDP_EVENT_TYPE_NODE_DESC_RSP_E:
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Node Descriptor Response:\n");
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Status:                     %d\n", ZDP_Event_Data->Event_Data.Node_Desc_Rsp.Status);
            if(ZDP_Event_Data->Event_Data.Node_Desc_Rsp.Status == QAPI_OK)
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  NwkAddrOfInterest:          0x%04X\n", ZDP_Event_Data->Event_Data.Node_Desc_Rsp.NwkAddrOfInterest);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  LogicalType:                %d\n", ZDP_Event_Data->Event_Data.Node_Desc_Rsp.NodeDescriptor.LogicalType);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  ComplexDescriptorAvailable: %d\n", ZDP_Event_Data->Event_Data.Node_Desc_Rsp.NodeDescriptor.ComplexDescriptorAvailable);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  UserDescriptorAvailable:    %d\n", ZDP_Event_Data->Event_Data.Node_Desc_Rsp.NodeDescriptor.UserDescriptorAvailable);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  APSFlags:                   %d\n", ZDP_Event_Data->Event_Data.Node_Desc_Rsp.NodeDescriptor.APSFlags);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  FrequencyBand:              %d\n", ZDP_Event_Data->Event_Data.Node_Desc_Rsp.NodeDescriptor.FrequencyBand);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  MACCapabilityFlags:         %d\n", ZDP_Event_Data->Event_Data.Node_Desc_Rsp.NodeDescriptor.MACCapabilityFlags);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  ManufacturerCode:           0x%04X\n", ZDP_Event_Data->Event_Data.Node_Desc_Rsp.NodeDescriptor.ManufacturerCode);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  ServerMask:                 %d\n", ZDP_Event_Data->Event_Data.Node_Desc_Rsp.NodeDescriptor.ServerMask);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  MaxOutgoingTransferSize:    %d\n", ZDP_Event_Data->Event_Data.Node_Desc_Rsp.NodeDescriptor.MaximumOutgoingTransferSize);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  DescriptorCapabilityField:  %d\n", ZDP_Event_Data->Event_Data.Node_Desc_Rsp.NodeDescriptor.DescriptorCapabilityField);
            }
            break;

         case QAPI_ZB_ZDP_EVENT_TYPE_MATCH_DESC_RSP_E:
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Match Descriptor Response:\n");
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Status:              %d\n", ZDP_Event_Data->Event_Data.Match_Desc_Rsp.Status);
            if(ZDP_Event_Data->Event_Data.Match_Desc_Rsp.Status == QAPI_OK)
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Address:             0x%04X\n", ZDP_Event_Data->Event_Data.Match_Desc_Rsp.NwkAddrOfInterest);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Number Of Endpoints: %d\n",     ZDP_Event_Data->Event_Data.Match_Desc_Rsp.MatchLength);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Matched Endpoints:   ");
               for(Index = 0; Index < ZDP_Event_Data->Event_Data.Match_Desc_Rsp.MatchLength; Index++)
               {
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "%d%s", (ZDP_Event_Data->Event_Data.Match_Desc_Rsp.MatchList)[Index], (Index == (ZDP_Event_Data->Event_Data.Match_Desc_Rsp.MatchLength - 1)) ? "\n" : ", ");
               }
            }
            break;

         case QAPI_ZB_ZDP_EVENT_TYPE_DEVICE_ANNCE_E:
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Device Annce:\n");
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  ExtendedAddress: %08X%08X\n", (uint32_t)(ZDP_Event_Data->Event_Data.Device_Annce.IEEEAddr >> 32), (uint32_t)(ZDP_Event_Data->Event_Data.Device_Annce.IEEEAddr));
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  NetworkAddress:  0x%04X\n", ZDP_Event_Data->Event_Data.Device_Annce.NwkAddr);
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Capability:      0x%02X\n", ZDP_Event_Data->Event_Data.Device_Annce.Capability);
            break;

         case QAPI_ZB_ZDP_EVENT_TYPE_END_DEVICE_BIND_RSP_E:
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "End Device Bind Response:\n");
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Status: %d\n", ZDP_Event_Data->Event_Data.End_Device_Bind_Rsp.Status);
            break;

         case QAPI_ZB_ZDP_EVENT_TYPE_BIND_RSP_E:
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Bind Response:\n");
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Status: %d\n", ZDP_Event_Data->Event_Data.Bind_Rsp.Status);
            break;

         case QAPI_ZB_ZDP_EVENT_TYPE_UNBIND_RSP_E:
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Unbind Response:\n");
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Status: %d\n", ZDP_Event_Data->Event_Data.Unbind_Rsp.Status);
            break;

         case QAPI_ZB_ZDP_EVENT_TYPE_ACTIVE_EP_RSP_E:
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Active Endpoint Response:\n");
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Status:            %d\n", ZDP_Event_Data->Event_Data.Active_EP_Rsp.Status);
            if(ZDP_Event_Data->Event_Data.Active_EP_Rsp.Status == QAPI_OK)
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  NwkAddrOfInterest: 0x%04X\n", ZDP_Event_Data->Event_Data.Active_EP_Rsp.NwkAddrOfInterest);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  ActiveEPCount:     %d\n",     ZDP_Event_Data->Event_Data.Active_EP_Rsp.ActiveEPCount);
               if((ZDP_Event_Data->Event_Data.Active_EP_Rsp.ActiveEPCount > 0) && (ZDP_Event_Data->Event_Data.Active_EP_Rsp.ActiveEPList != NULL))
               {
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  ActiveEPList: ");
                  for(Index = 0; Index < ZDP_Event_Data->Event_Data.Active_EP_Rsp.ActiveEPCount; Index++)
                  {
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "%d ", ZDP_Event_Data->Event_Data.Active_EP_Rsp.ActiveEPList[Index]);
                  }
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "\n");
               }
            }
            break;

         case QAPI_ZB_ZDP_EVENT_TYPE_EXTENDED_ACTIVE_EP_RSP_E:
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Extended Active Endpoint Response:\n");
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Status:            %d\n", ZDP_Event_Data->Event_Data.Extended_Active_EP_Rsp.Status);
            if(ZDP_Event_Data->Event_Data.Extended_Active_EP_Rsp.Status == QAPI_OK)
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  NwkAddrOfInterest: 0x%04X\n", ZDP_Event_Data->Event_Data.Extended_Active_EP_Rsp.NwkAddrOfInterest);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  ActiveEPCount:     %d\n",     ZDP_Event_Data->Event_Data.Extended_Active_EP_Rsp.ActiveEPCount);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  StartIndex:        %d\n",     ZDP_Event_Data->Event_Data.Extended_Active_EP_Rsp.StartIndex);
               if((ZDP_Event_Data->Event_Data.Extended_Active_EP_Rsp.ActiveEPCount > 0) && (ZDP_Event_Data->Event_Data.Extended_Active_EP_Rsp.AppEPList != NULL))
               {
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  ActiveEPList: ");
                  for(Index = 0; Index < ZDP_Event_Data->Event_Data.Extended_Active_EP_Rsp.ActiveEPCount; Index++)
                  {
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "%d ", ZDP_Event_Data->Event_Data.Extended_Active_EP_Rsp.AppEPList[Index]);
                  }
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "\n");
               }
            }
            break;

         case QAPI_ZB_ZDP_EVENT_TYPE_MGMT_BIND_RSP_E:
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Mgnt Bind Response:\n");
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Status:                %d\n", ZDP_Event_Data->Event_Data.Mgmt_Bind_Rsp.Status);
            if(ZDP_Event_Data->Event_Data.Mgmt_Bind_Rsp.Status == QAPI_OK)
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  BindingTableEntries:   %d\n", ZDP_Event_Data->Event_Data.Mgmt_Bind_Rsp.BindingTableEntries);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  StartIndex:            %d\n", ZDP_Event_Data->Event_Data.Mgmt_Bind_Rsp.StartIndex);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  BindingTableListCount: %d\n", ZDP_Event_Data->Event_Data.Mgmt_Bind_Rsp.BindingTableListCount);
               if((ZDP_Event_Data->Event_Data.Mgmt_Bind_Rsp.BindingTableListCount > 0) && (ZDP_Event_Data->Event_Data.Mgmt_Bind_Rsp.BindingTableList != NULL))
               {
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  BindingTableList: %d\n", ZDP_Event_Data->Event_Data.Mgmt_Bind_Rsp.BindingTableListCount);
                  for(Index = 0; Index < ZDP_Event_Data->Event_Data.Mgmt_Bind_Rsp.BindingTableListCount; Index++)
                  {
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  SrcAddress   0x%08X%08X\n", (unsigned int)(ZDP_Event_Data->Event_Data.Mgmt_Bind_Rsp.BindingTableList[Index].SrcAddress >> 32), (unsigned int)(ZDP_Event_Data->Event_Data.Mgmt_Bind_Rsp.BindingTableList[Index].SrcAddress));
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  SrcEndpoint: %d\n", ZDP_Event_Data->Event_Data.Mgmt_Bind_Rsp.BindingTableList[Index].SrcEndpoint);
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  ClusterID:   0x%04X\n", ZDP_Event_Data->Event_Data.Mgmt_Bind_Rsp.BindingTableList[Index].ClusterId);
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  DstAddrMode  %d\n", ZDP_Event_Data->Event_Data.Mgmt_Bind_Rsp.BindingTableList[Index].DstAddrMode);
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  DstAddress:  ");
                     switch(ZDP_Event_Data->Event_Data.Mgmt_Bind_Rsp.BindingTableList[Index].DstAddrMode)
                     {
                        case QAPI_ZB_ADDRESS_MODE_GROUP_ADDRESS_E:
                        case QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E:
                           QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "0x%04X\n", ZDP_Event_Data->Event_Data.Mgmt_Bind_Rsp.BindingTableList[Index].DstAddress.ShortAddress);
                           break;

                        case QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E:
                           QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "0x%08X%08X\n", (unsigned int)(ZDP_Event_Data->Event_Data.Mgmt_Bind_Rsp.BindingTableList[Index].DstAddress.ExtendedAddress >> 32), (unsigned int)(ZDP_Event_Data->Event_Data.Mgmt_Bind_Rsp.BindingTableList[Index].DstAddress.ExtendedAddress));
                           break;

                        case QAPI_ZB_ADDRESS_MODE_NONE_E:
                        default:
                           QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "None\n");
                           break;
                     }
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  DstEndpoint: %d\n", ZDP_Event_Data->Event_Data.Mgmt_Bind_Rsp.BindingTableList[Index].DstEndpoint);

                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "\n");
                  }
               }
            }
            break;

         case QAPI_ZB_ZDP_EVENT_TYPE_MGMT_LQI_RSP_E:
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Mgnt Lqi Response:\n");
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Status:               %d\n", ZDP_Event_Data->Event_Data.Mgmt_Lqi_Rsp.Status);
            if(ZDP_Event_Data->Event_Data.Mgmt_Lqi_Rsp.Status == QAPI_OK)
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  NeighborTableEntries:   %d\n", ZDP_Event_Data->Event_Data.Mgmt_Lqi_Rsp.NeighborTableEntries);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  StartIndex:             %d\n", ZDP_Event_Data->Event_Data.Mgmt_Lqi_Rsp.StartIndex);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  NeighborTableListCount: %d\n", ZDP_Event_Data->Event_Data.Mgmt_Lqi_Rsp.NeighborTableListCount);
               if((ZDP_Event_Data->Event_Data.Mgmt_Lqi_Rsp.NeighborTableListCount > 0) && (ZDP_Event_Data->Event_Data.Mgmt_Lqi_Rsp.NeighborTableList != NULL))
               {
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  NeighborTableList: \n");
                  for(Index = 0; Index < ZDP_Event_Data->Event_Data.Mgmt_Lqi_Rsp.NeighborTableListCount; Index++)
                  {
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  ExtendedPanId:          0x%08X%08X\n", (unsigned int)(ZDP_Event_Data->Event_Data.Mgmt_Lqi_Rsp.NeighborTableList[Index].ExtendedPanId >> 32), (unsigned int)(ZDP_Event_Data->Event_Data.Mgmt_Lqi_Rsp.NeighborTableList[Index].ExtendedPanId));
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  ExtendedAddress:        0x%08X%08X\n", (unsigned int)(ZDP_Event_Data->Event_Data.Mgmt_Lqi_Rsp.NeighborTableList[Index].ExtendedAddress >> 32),(unsigned int)(ZDP_Event_Data->Event_Data.Mgmt_Lqi_Rsp.NeighborTableList[Index].ExtendedAddress));
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  NetworkAddress:         0x%04X\n", ZDP_Event_Data->Event_Data.Mgmt_Lqi_Rsp.NeighborTableList[Index].NetworkAddress);
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  DeviceType              %d\n", ZDP_Event_Data->Event_Data.Mgmt_Lqi_Rsp.NeighborTableList[Index].DeviceType);
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  RxOnWhenIdle:           %d\n", ZDP_Event_Data->Event_Data.Mgmt_Lqi_Rsp.NeighborTableList[Index].RxOnWhenIdle);
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Relationship:           %d\n", ZDP_Event_Data->Event_Data.Mgmt_Lqi_Rsp.NeighborTableList[Index].Relationship);
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  PermitJoining           %d\n", ZDP_Event_Data->Event_Data.Mgmt_Lqi_Rsp.NeighborTableList[Index].PermitJoining);
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Depth:                  %d\n", ZDP_Event_Data->Event_Data.Mgmt_Lqi_Rsp.NeighborTableList[Index].Depth);
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  LQI:                    %d\n", ZDP_Event_Data->Event_Data.Mgmt_Lqi_Rsp.NeighborTableList[Index].LQI);
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "\n");
                  }
               }
            }
            break;

         case QAPI_ZB_ZDP_EVENT_TYPE_MGMT_RTG_RSP_E:
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Mgnt RTG Response:\n");
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Status:                %d\n", ZDP_Event_Data->Event_Data.Mgmt_Rtg_Rsp.Status);
            if(ZDP_Event_Data->Event_Data.Mgmt_Rtg_Rsp.Status == QAPI_OK)
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  RoutingTableEntries:   %d\n", ZDP_Event_Data->Event_Data.Mgmt_Rtg_Rsp.RoutingTableEntries);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  StartIndex:            %d\n", ZDP_Event_Data->Event_Data.Mgmt_Rtg_Rsp.StartIndex);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  RoutingTableListCount: %d\n", ZDP_Event_Data->Event_Data.Mgmt_Rtg_Rsp.RoutingTableListCount);
               if((ZDP_Event_Data->Event_Data.Mgmt_Rtg_Rsp.RoutingTableListCount > 0) && (ZDP_Event_Data->Event_Data.Mgmt_Rtg_Rsp.RoutingTableList != NULL))
               {
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  RouteList: \n");
                  for(Index = 0; Index < ZDP_Event_Data->Event_Data.Mgmt_Rtg_Rsp.RoutingTableListCount; Index++)
                  {
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  DstAddr:     0x%04X\n", ZDP_Event_Data->Event_Data.Mgmt_Rtg_Rsp.RoutingTableList[Index].DestinationAddress);
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Status:      %d\n", ZDP_Event_Data->Event_Data.Mgmt_Rtg_Rsp.RoutingTableList[Index].Status);
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  NextHopAddr: 0x%04X\n", ZDP_Event_Data->Event_Data.Mgmt_Rtg_Rsp.RoutingTableList[Index].NextHopAddress);
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Flags:       %d\n", ZDP_Event_Data->Event_Data.Mgmt_Rtg_Rsp.RoutingTableList[Index].Flags);
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "\n");
                  }
               }
            }
            break;

         case QAPI_ZB_ZDP_EVENT_TYPE_MGMT_LEAVE_RSP_E:
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Mmgt Leave Response:\n");
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Status: %d\n", ZDP_Event_Data->Event_Data.Mgmt_Leave_Rsp.Status);
            break;

         case QAPI_ZB_ZDP_EVENT_TYPE_MGMT_PERMIT_JOINING_RSP_E:
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Permit Join Response:\n");
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Status: %d\n", ZDP_Event_Data->Event_Data.Mgmt_Permit_Joining_Rsp.Status);
            break;

         case QAPI_ZB_ZDP_EVENT_TYPE_MGMT_NWK_UPDATE_NOTIFY_E:
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "NWK Update Notify:\n");
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  Status:                   %d\n", ZDP_Event_Data->Event_Data.Mgmt_Nwk_Update_Notify.Status);
            if(ZDP_Event_Data->Event_Data.Mgmt_Nwk_Update_Notify.Status == QAPI_OK)
            {
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  ScannedChannels:          0x%08X\n", ZDP_Event_Data->Event_Data.Mgmt_Nwk_Update_Notify.ScannedChannels);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  TotalTransmissions:       %d\n", ZDP_Event_Data->Event_Data.Mgmt_Nwk_Update_Notify.TotalTransmissions);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  TransmissionFailures:     %d\n", ZDP_Event_Data->Event_Data.Mgmt_Nwk_Update_Notify.TransmissionFailures);
               QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  ScannedChannelsListCount: %d\n", ZDP_Event_Data->Event_Data.Mgmt_Nwk_Update_Notify.ScannedChannelsListCount);
               if((ZDP_Event_Data->Event_Data.Mgmt_Nwk_Update_Notify.ScannedChannelsListCount > 0) && (ZDP_Event_Data->Event_Data.Mgmt_Nwk_Update_Notify.EnergyValues != NULL))
               {
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  EnergyValues: \n");
                  for(Index = 0; Index < ZDP_Event_Data->Event_Data.Mgmt_Nwk_Update_Notify.ScannedChannelsListCount; Index++)
                  {
                     QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "  %d ", ZDP_Event_Data->Event_Data.Mgmt_Nwk_Update_Notify.EnergyValues[Index]);
                  }
                  QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "\n");
               }
            }
            break;

         default:
            QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Unhandled ZDP Event: %d\n", ZDP_Event_Data->Event_Type);
            break;
      }

      QCLI_Display_Prompt();
   }
}

/**
   @brief Registers ZigBee ZDP demo commands with QCLI.

   @param ZigBee_QCLI_Handle is the QCLI handle for the main ZigBee demo.

   @return true if the ZigBee ZDP demo initialized successfully, false
           otherwise.

*/
qbool_t Initialize_ZDP_Demo(QCLI_Group_Handle_t ZigBee_QCLI_Handle)
{
   qbool_t Ret_Val;

   memset(&ZDP_Demo_Context, 0, sizeof(ZigBee_ZDP_Demo_Context_t));

   /* Register zigbee ZDP command group.                          */
   ZDP_Demo_Context.QCLI_Handle = QCLI_Register_Command_Group(ZigBee_QCLI_Handle, &ZigBee_ZDP_CMD_Group);
   if(ZDP_Demo_Context.QCLI_Handle != NULL)
   {
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "ZigBee ZDP Demo Initialized.\n");

      Ret_Val = true;
   }
   else
   {
      QCLI_Printf(ZDP_Demo_Context.QCLI_Handle, "Failed to register ZigBee ZDP commands.\n");

      Ret_Val = false;
   }

   return(Ret_Val);
}

/**
   @brief Initialize the ZigBee ZDP demo after the ZB stack had been
          initialized.

   @return true if the ZigBee ZDP demo initialized successfully, false
           otherwise.

*/
qbool_t ZDP_Demo_StackInitialize(qapi_ZB_Handle_t ZigBee_Handle)
{
   qbool_t       Ret_Val;
   qapi_Status_t Result;

   /* Register ZDP event callback. */
   Result = qapi_ZB_ZDP_Register_Callback(ZigBee_Handle, ZB_ZDP_Event_CB, 0);
   if(Result == QAPI_OK)
   {
      Ret_Val = true;
   }
   else
   {
      Display_Function_Error(ZDP_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Register_Callback", Result);
      Ret_Val = false;
   }

   return(Ret_Val);
}

