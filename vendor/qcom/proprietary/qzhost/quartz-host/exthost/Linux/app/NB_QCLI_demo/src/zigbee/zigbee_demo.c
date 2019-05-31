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
#include "zdp_demo.h"
#include "zcl_demo.h"
#include "qapi_zb.h"
#include "qapi_zb_nwk.h"
#include "qapi_zb_bdb.h"
#include "qapi_zb_zdp.h"
#include "qapi_zb_cl_basic.h"
#include "qapi_zb_cl_identify.h"
#include "qapi_persist.h"
#include "qsOSAL.h"

/* The default PAN ID used by the ZigBee demo application. */
#define DEFAULT_ZIGBEE_PAN_ID                         (0xB89B)

/* The default end device time out value. */
#define DEFAULT_END_DEVICE_TIME_OUT                   (0xFF)

/* Channel mask used when forming a network. */
#define FORM_CHANNEL_MASK                             (0x07FFF800)

/* Channel mask used when joining a network. */
#define JOIN_CHANNEL_MASK                             (0x07FFF800)

/* The maximum number of registered devices for the demo's device list. */
#define DEV_ID_LIST_SIZE                              (8)

/* The number of scenes to have space allocated for the Scenes cluster. */
#define APP_MAX_NUM_SCENES                            (4)

/* The maxium length of an attribute being read and written.            */
#define MAXIMUM_ATTRIUBTE_LENGTH                                        (8)

/* Default ZigBee Link Key used during commissioning. */
#define DEFAULT_ZIGBEE_LINK_KEY                       {'Z', 'i', 'g', 'B', 'e', 'e', 'A', 'l', 'l', 'i', 'a', 'n', 'c', 'e', '0', '9'}

/* Capability falgs used for coordinators. */
#define COORDINATOR_CAPABILITIES                      (QAPI_MAC_CAPABILITY_FULL_FUNCTION | QAPI_MAC_CAPABILITY_MAINS_POWER | QAPI_MAC_CAPABILITY_RX_ON_IDLE | QAPI_MAC_CAPABILITY_ALLOCATE_ADDR)

/* Capability falgs used for end devices. */
#define END_DEVICE_CAPABILITIES                       (QAPI_MAC_CAPABILITY_ALLOCATE_ADDR)

/* Location and name for the ZigBee persistent storage. */
#define ZIGBEE_PERSIST_DIRECTORY                      ("/spinor/zigbee")
#define ZIGBEE_PERSIST_PREFIX                         ("pdata")
#define ZIGBEE_PERSIST_SUFFIX                         (".bin")

/* Structure representing the main ZigBee demo context information. */
typedef struct ZigBee_Demo_Context_s
{
   QCLI_Group_Handle_t   QCLI_Handle;                     /*< QCLI handle for the main ZigBee demo. */
   qapi_ZB_Handle_t      ZigBee_Handle;                   /*< Handle provided by the ZigBee stack when initialized. */
   ZB_Device_ID_t        DevIDList[DEV_ID_LIST_SIZE + 1]; /*< List of devices */
   uint8_t               ZCL_Sequence_Num;                /*< Sequence number used for sending packets. */
   qapi_Persist_Handle_t PersistHandle;                   /*< Persist handle used by the ZigBee demo. */
} ZigBee_Demo_Context_t;

/* The ZigBee demo context. */
static ZigBee_Demo_Context_t ZigBee_Demo_Context;

/* Security information used by the demo when joining or forming a network. */
const static qapi_ZB_Security_t Default_ZB_Secuity =
{
   QAPI_ZB_SECURITY_LEVEL_NONE_E,                     /* Security level 0.       */
   true,                                              /* Use Insecure Rejoin.    */
   0,                                                 /* Trust center address.   */
   DEFAULT_ZIGBEE_LINK_KEY,                           /* Preconfigured Link Key. */
   {0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
    0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF},  /* Distributed global key. */
   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  /* Network Key.            */
};

static QCLI_Command_Status_t cmd_ZB_Initialize(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_Shutdown(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_AddDevice(uint32_t Parameter_Count,  QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_RemoveDevice(uint32_t Parameter_Count,  QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_ShowDeviceList(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_Form(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_Join(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_Reconnect(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_Leave(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_LeaveReq(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_PermitJoin(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_GetNIB(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_SetNIB(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_GetAIB(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_SetAIB(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_SetBIB(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_GetBIB(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_SetExtAddress(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_GetAddresses(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_SetTxPower(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_GetTxPower(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ZB_ClearPersist(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static void ZB_Event_CB(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_Event_t *ZB_Event, uint32_t CB_Param);
static void ZB_Persist_Notify_CB(qapi_ZB_Handle_t ZB_Handle, uint32_t CB_Param);

/* Command list for the main ZigBee demo. */
const QCLI_Command_t ZigBee_CMD_List[] =
{
   /* cmd_function         thread  cmd_string        usage_string                                                                                    description */
   {cmd_ZB_Initialize,     false,  "Initialize",     "[UsePersist(Default=0)]",                                                                      "Initialize the ZigBee stack."},
   {cmd_ZB_Shutdown,       false,  "Shutdown",       "",                                                                                             "Shutdown the ZigBee stack."},
   {cmd_ZB_AddDevice,      false,  "AddDevice",      "[Mode(1=Grp,2=Nwk,3=Ext)][Address][Endpoint]",                                                 "Adds an address to the demo's device list for use with other commands."},
   {cmd_ZB_RemoveDevice,   false,  "RemoveDevice",   "[DevID]",                                                                                      "Removes an address from the demo's device list."},
   {cmd_ZB_ShowDeviceList, false,  "ShowDeviceList", "",                                                                                             "Display the demo's device list."},
   {cmd_ZB_Form,           false,  "Form",           "[UseSecurity(0=No,1=Yes)][Distributed(Default=0)][Channel(optional)]",                         "Start a ZigBee network."},
   {cmd_ZB_Join,           false,  "Join",           "[AsCoordinator(0=No,1=Yes)][UseSecurity(0=No,1=Yes)][IsRejoin(Default=0)][Channel(optional)]", "Join or Rejoin a ZigBee network"},
   {cmd_ZB_Reconnect,      false,  "Reconnect",      "[TcRejoin(Default=0)]",                                                                                             "Reconnect to a ZigBee network"},
   {cmd_ZB_Leave,          false,  "Leave",          "",                                                                                             "Leave the ZigBee netork."},
   {cmd_ZB_LeaveReq,       false,  "LeaveReq",       "[TargetDevID][LeaveDevId][RemoveChildren][Rejoin]",                                            "Sends a Leave request."},
   {cmd_ZB_PermitJoin,     false,  "PermitJoin",     "[Duration(0-255)]",                                                                            "Permit devices to join the network for a specified period."},
   {cmd_ZB_GetNIB,         false,  "GetNIB",         "[AttrId][AttrIndex][MaxLength (default=128)]",                                                 "Read a NWK attibute."},
   {cmd_ZB_SetNIB,         false,  "SetNIB",         "[AttrId][AttrIndex][Length][Value]",                                                           "Write a NWK attibute."},
   {cmd_ZB_GetAIB,         false,  "GetAIB",         "[AttrId][AttrIndex][MaxLength (default=128)]",                                                 "Read a APS attibute."},
   {cmd_ZB_SetAIB,         false,  "SetAIB",         "[AttrId][AttrIndex][Length][Value]",                                                           "Write a APS attibute."},
   {cmd_ZB_SetBIB,         false,  "SetBIB",         "[AttrId][AttrIndex][AttrLength][AttrValue]",                                                   "Set BDB attribute."},
   {cmd_ZB_GetBIB,         false,  "GetBIB",         "[AttrId][AttrIndex][AttrLength]",                                                              "Get BDB attribute."},
   {cmd_ZB_SetExtAddress,  false,  "SetExtAddress",  "[ExtAddr]",                                                                                    "Set the extended address of the local interface."},
   {cmd_ZB_GetAddresses,   false,  "GetAddresses",   "",                                                                                             "Get the local addresses of the ZigBee interface."},
   {cmd_ZB_SetTxPower,     false,  "SetTxPower",     "[TxPower(-32 - 31)]",                                                                          "Set the ZigBee Tx power."},
   {cmd_ZB_GetTxPower,     false,  "GetTxPower",     "",                                                                                             "Get the ZigBee Tx power."},
   {cmd_ZB_ClearPersist,   false,  "ClearPersist",   "",                                                                                             "Clears ZigBee persistent data."}
};

const QCLI_Command_Group_t ZigBee_CMD_Group = {"ZigBee", sizeof(ZigBee_CMD_List) / sizeof(QCLI_Command_t), ZigBee_CMD_List};

/**
   @brief Executes the "Initialize" command to initialize the ZigBee interface.

   Parameter_List[0] (0-1) is a flag indicating if persistense should be used.
                     If set to 1, the intialize function will try to load any
                     existing persistence data and register the callback to
                     periodically store persistent data.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_Initialize(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_Status_t          Result;
   uint64_t               Extended_Address;
   qbool_t                UsePersist;
   uint32_t               PersistLength;
   uint8_t               *PersistData;

   /* Verify the ZigBee layer had not been initialized yet. */
   if(ZigBee_Demo_Context.ZigBee_Handle == NULL)
   {
      UsePersist = false;

      if(Parameter_Count >= 1)
      {
         if(Verify_Integer_Parameter(&Parameter_List[0], 0, 1))
         {
            UsePersist = (qbool_t)(Parameter_List[0].Integer_Value != 0);
            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }
         else
         {
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_SUCCESS_E;
      }

      if(Ret_Val == QCLI_STATUS_SUCCESS_E)
      {
         Result = qapi_ZB_Initialize(&(ZigBee_Demo_Context.ZigBee_Handle), ZB_Event_CB, 0);

         if((Result == QAPI_OK) && (ZigBee_Demo_Context.ZigBee_Handle != NULL))
         {
#if 0
            if(UsePersist)
            {
               /* Attempt to load the persist data. */
               Result = qapi_Persist_Initialize(&(ZigBee_Demo_Context.PersistHandle), ZIGBEE_PERSIST_DIRECTORY, ZIGBEE_PERSIST_PREFIX, ZIGBEE_PERSIST_SUFFIX, NULL, 0);
               if(Result == QAPI_OK)
               {
                  Result = qapi_Persist_Get(ZigBee_Demo_Context.PersistHandle, &PersistLength, &PersistData);
                  if(Result == QAPI_OK)
                  {
                     Result = qapi_ZB_Restore_Persistent_Data(ZigBee_Demo_Context.ZigBee_Handle, PersistData, PersistLength);
                     if(Result == QAPI_OK)
                     {
                        QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Persist Data Loaded.\n");
                     }
                     else
                     {
                        Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_Restore_Persistent_Data", Result);
                     }

                     qapi_Persist_Free(ZigBee_Demo_Context.PersistHandle, PersistData);
                  }
                  else if(Result != QAPI_ERR_NO_ENTRY)
                  {
                     Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_Persist_Get", Result);
                  }

                  /* If it got this far, register the persist data callback. */
                  Result = qapi_ZB_Register_Persist_Notify_CB(ZigBee_Demo_Context.ZigBee_Handle, ZB_Persist_Notify_CB, (uint32_t)(ZigBee_Demo_Context.PersistHandle));
                  if(Result != QAPI_OK)
                  {
                     Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_Register_Persist_Notify_CB", Result);
                  }
               }
               else
               {
                  Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_Persist_Initialize", Result);
               }
            }
#endif

            if(Result == QAPI_OK)
            {
               if(ZDP_Demo_StackInitialize(ZigBee_Demo_Context.ZigBee_Handle))
               {
                  Result = qapi_ZB_Get_Extended_Address(ZigBee_Demo_Context.ZigBee_Handle, &Extended_Address);
                  if(Result == QAPI_OK)
                  {
                     QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "ZigBee stack initialized.\n");
                     QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "   Extended Address: %08X%08X\n", (uint32_t)(Extended_Address >> 32), (uint32_t)Extended_Address);
                  }
                  else
                  {
                     Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_Get_Extended_Address", Result);
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Register_Callback", Result);
               Ret_Val = QCLI_STATUS_ERROR_E;
            }

            if(Ret_Val != QCLI_STATUS_SUCCESS_E)
            {
               qapi_ZB_Shutdown(ZigBee_Demo_Context.ZigBee_Handle);
               ZigBee_Demo_Context.ZigBee_Handle = NULL;

#if 0
               if(ZigBee_Demo_Context.PersistHandle != NULL)
               {
                  qapi_Persist_Cleanup(ZigBee_Demo_Context.PersistHandle);
               }
#endif
            }
         }
         else
         {
            Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_Initialize", Result);
            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
   }
   else
   {
      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "ZigBee stack already initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "Shutdown" command to shut down the ZigBee interface.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E   indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E   indicates there is usage error associated with this
                            command.
*/
static QCLI_Command_Status_t cmd_ZB_Shutdown(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;

   /* Verify the ZigBee layer had been initialized. */
   if(ZigBee_Demo_Context.ZigBee_Handle != NULL)
   {
      /* Cleanup the clusters have have been created. */
      ZB_Cluster_Cleanup();

      /* Shutdown the ZigBee stack. */
      qapi_ZB_Shutdown(ZigBee_Demo_Context.ZigBee_Handle);
      ZigBee_Demo_Context.ZigBee_Handle = NULL;

#if 0
      if(ZigBee_Demo_Context.PersistHandle != NULL)
      {
         qapi_Persist_Cleanup(ZigBee_Demo_Context.PersistHandle);
      }
#endif

      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "ZigBee shutdown.\n");

      Ret_Val = QCLI_STATUS_SUCCESS_E;
   }
   else
   {
      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "ZigBee not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "AddDevice" command to add an address to the demo's
          device list.

   The addresses added with this command can be either a group address, a
   network address + endpoint or a extended address + endpoint.  Once
   registered, these addresses can be used to send commands to a remote device.

   Parameter_List[0] (1-3) is the type of address to be added. 0 for a group
                     address, 1 for a network address or 2 for an extended
                     address.
   Parameter_List[1] is the address to be added.  For group and network
                     addresses, this must be a valid 16-bit value and for
                     extended addresses, it will be a 64-bit value.
   Parameter_List[2] is the endpoint for the address. Note this isn't used for
                     group addresses.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_AddDevice(uint32_t Parameter_Count,  QCLI_Parameter_t *Parameter_List)
{
   uint32_t              Index;
   QCLI_Command_Status_t Ret_Val;
   qapi_ZB_Addr_Mode_t   DevType;
   uint8_t               DevEndpoint;
   uint64_t              DevAddr;

   if(ZigBee_Demo_Context.ZigBee_Handle != 0)
   {
      if((Parameter_Count >= 2) &&
         (Verify_Integer_Parameter(&Parameter_List[0], QAPI_ZB_ADDRESS_MODE_GROUP_ADDRESS_E, QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E)) &&
         (Hex_String_To_ULL(Parameter_List[1].String_Value, &DevAddr)))
      {
         DevType = (qapi_ZB_Addr_Mode_t)Parameter_List[0].Integer_Value;

         /* Assume success unless something goes wrong. */
         Ret_Val = QCLI_STATUS_SUCCESS_E;

         /* We either need a group address or an endpoint needs to also be
            specified with this command. */
         if(DevType != QAPI_ZB_ADDRESS_MODE_GROUP_ADDRESS_E)
         {
            if((Parameter_Count >= 3) &&
               (Verify_Integer_Parameter(&Parameter_List[2], 0, 0xFF)))
            {
               /* Record the endpoint parameter. */
               DevEndpoint = Parameter_List[2].Integer_Value;
            }
            else
            {
               Ret_Val = QCLI_STATUS_USAGE_E;
            }
         }
         else
         {
            /* Groups do not require endpoints. */
            DevEndpoint = 0;
         }

         /* Continue if all parameters validated successfully. */
         if(Ret_Val == QCLI_STATUS_SUCCESS_E)
         {
            /* Confirm the device address is valid for the type. */
            if((DevType == QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E) || (DevAddr <= 0xFFFF))
            {
               /* Find an unused spot in the array. */
               for(Index = 1; Index <= DEV_ID_LIST_SIZE; Index++)
               {
                  if(!(ZigBee_Demo_Context.DevIDList[Index].InUse))
                  {
                     /* Found a spare slot. */
                     ZigBee_Demo_Context.DevIDList[Index].Type                    = (qapi_ZB_Addr_Mode_t)DevType;
                     ZigBee_Demo_Context.DevIDList[Index].Endpoint                = DevEndpoint;
                     ZigBee_Demo_Context.DevIDList[Index].Address.ExtendedAddress = DevAddr;
                     ZigBee_Demo_Context.DevIDList[Index].InUse                   = true;

                     QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Registered device ID: %d\n", Index);
                     break;
                  }
               }

               if(Index > DEV_ID_LIST_SIZE)
               {
                  QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Could not find a spare device entry.\n");
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Address is too large for type specified.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
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
      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "RemoveDevice" command to remove an address to the demo's
          device list.

   Parameter_List[0] (1-DEV_ID_LIST_SIZE) is the index of the device to remove.
                     Note that address zero is reserved for the NULL address and
                     cannot be removed.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_RemoveDevice(uint32_t Parameter_Count,  QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;
   uint32_t              Index;

   if(ZigBee_Demo_Context.ZigBee_Handle != 0)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&Parameter_List[0], 1, DEV_ID_LIST_SIZE)))
      {
         Index = Parameter_List[0].Integer_Value;

         /* Make sure the device is actually in use. */
         if(ZigBee_Demo_Context.DevIDList[Index].InUse)
         {
            /* Clear the device mapping. */
            memset(&ZigBee_Demo_Context.DevIDList[Index], 0, sizeof(ZB_Device_ID_t));
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Device %d removed.\n", Index);
            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }
         else
         {
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Device ID is not in use.\n");
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
      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "ShowDeviceList" command to display the list of
          registered devices in the demo's device list.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_ShowDeviceList(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   uint32_t               Index;
   uint8_t                AddressString[17];
   const char            *TypeString;

   if(ZigBee_Demo_Context.ZigBee_Handle != 0)
   {
      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Device List:\n");
      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, " ID | Type | Address          | Endpoint\n");
      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "----+------+------------------+----------\n");

      for(Index = 0; Index <= DEV_ID_LIST_SIZE; Index++)
      {
         if(ZigBee_Demo_Context.DevIDList[Index].InUse)
         {
            switch(ZigBee_Demo_Context.DevIDList[Index].Type)
            {
               case QAPI_ZB_ADDRESS_MODE_GROUP_ADDRESS_E:
                  TypeString = "Grp ";
                  snprintf((char *)(AddressString), sizeof(AddressString), "%04X", ZigBee_Demo_Context.DevIDList[Index].Address.ShortAddress);
                  break;

               case QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E:
                  TypeString = "Nwk ";
                  snprintf((char *)(AddressString), sizeof(AddressString), "%04X", ZigBee_Demo_Context.DevIDList[Index].Address.ShortAddress);
                  break;

               case QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E:
                  TypeString = "Ext ";
                  snprintf((char *)(AddressString), sizeof(AddressString), "%08X%08X", (unsigned int)(ZigBee_Demo_Context.DevIDList[Index].Address.ExtendedAddress >> 32), (unsigned int)(ZigBee_Demo_Context.DevIDList[Index].Address.ExtendedAddress));
                  break;

               case QAPI_ZB_ADDRESS_MODE_NONE_E:
               default:
                  TypeString = "None";
                  AddressString[0] = '\0';
                  break;
            }

            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, " %2d | %4s | %16s | %d\n", Index, TypeString, AddressString, ZigBee_Demo_Context.DevIDList[Index].Endpoint);
         }
      }

      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "\n");
      Ret_Val = QCLI_STATUS_SUCCESS_E;
   }
   else
   {
      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "Form" command to start a ZigBee network.

   Parameter_List[0] (0-1) indicates if security should be used on the network.
                           1 will enable security and 0 will disable security.
   Parameter_List[1] (0-1) indicates if the device should form a distributed (1)
                           or centralized (0) network. Defaults to centralized.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_Form(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t   Ret_Val;
   qapi_Status_t           Result;
   qapi_ZB_NetworkConfig_t NetworkConfig;
   qbool_t                 UseSecurity;
   qbool_t                 Distributed;
   uint32_t                ChannelMask;

   /* Verify the ZigBee layer had been initialized. */
   if(ZigBee_Demo_Context.ZigBee_Handle != NULL)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 1)))
      {
         UseSecurity = (qbool_t)(Parameter_List[0].Integer_Value != 0);
         Distributed = false;
         ChannelMask = FORM_CHANNEL_MASK;

         Ret_Val = QCLI_STATUS_SUCCESS_E;
         if(Parameter_Count >= 2)
         {
            if(Verify_Integer_Parameter(&(Parameter_List[1]), 0, 1))
            {
               Distributed = (qbool_t)(Parameter_List[1].Integer_Value != 0);
            }
            else
            {
               Ret_Val = QCLI_STATUS_USAGE_E;
            }
         }

         if(Parameter_Count >= 3)
         {
            if(Verify_Integer_Parameter(&(Parameter_List[2]), 11, 26))
            {
               ChannelMask = 1 << Parameter_List[2].Integer_Value;
            }
            else
            {
               Ret_Val = QCLI_STATUS_USAGE_E;
            }
         }

         if(Ret_Val == QCLI_STATUS_SUCCESS_E)
         {
            memset(&NetworkConfig, 0, sizeof(qapi_ZB_NetworkConfig_t));

            NetworkConfig.ExtendedPanId = 0ULL;
            NetworkConfig.StackProfile  = QAPI_ZB_STACK_PROFILE_PRO_E;
            NetworkConfig.Page          = 0;
            NetworkConfig.ChannelMask   = ChannelMask;
            NetworkConfig.ScanAttempts  = 2;
            NetworkConfig.Capability    = COORDINATOR_CAPABILITIES;

            QSOSAL_MemCopy_S(&(NetworkConfig.Security), sizeof(qapi_ZB_Security_t), &Default_ZB_Secuity, sizeof(qapi_ZB_Security_t));

            /* Overwrite the default security level and trust center address. */
            NetworkConfig.Security.Security_Level       = UseSecurity ? QAPI_ZB_SECURITY_LEVEL_ENC_MIC32_E : QAPI_ZB_SECURITY_LEVEL_NONE_E;
            NetworkConfig.Security.Trust_Center_Address = Distributed ? QAPI_ZB_INVALID_EXTENDED_ADDRESS : 0;

            Result = qapi_ZB_Form(ZigBee_Demo_Context.ZigBee_Handle, &NetworkConfig);

            if(Result == QAPI_OK)
            {
               Display_Function_Success(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_Form");
               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_Form", Result);
               Ret_Val = QCLI_STATUS_ERROR_E;
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
      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "Join" command to join a ZigBee network.

   Parameter_List[0] (0-1) indicates if the device should join as a coordinator
                           (1) or as an end device (0).
   Parameter_List[1] (0-1) indicates if security should be used on the network
                           (1) or not (0).
   Parameter_List[2] (0-1) indicates if this is a join operation (0) or a rejoin
                           operation. Defaults to join if not specified.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_Join(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;
   qapi_Status_t         Result;
   qapi_ZB_Join_t        JoinConfig;
   qbool_t               IsCoordinator;
   qbool_t               UseSecurity;
   qbool_t               IsRejoin;
   uint32_t              ChannelMask;

   /* Verify the ZigBee layer had been initialized. */
   if(ZigBee_Demo_Context.ZigBee_Handle != NULL)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 2) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 1)) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 1)))
      {
         IsCoordinator = (qbool_t)(Parameter_List[0].Integer_Value != 0);
         UseSecurity   = (qbool_t)(Parameter_List[1].Integer_Value != 0);
         IsRejoin      = false;
         ChannelMask   = JOIN_CHANNEL_MASK;

         Ret_Val = QCLI_STATUS_SUCCESS_E;

         if(Parameter_Count >= 3)
         {
            if(Verify_Integer_Parameter(&(Parameter_List[2]), 0, 1))
            {
               IsRejoin = (qbool_t)(Parameter_List[2].Integer_Value != 0);
            }
            else
            {
               Ret_Val = QCLI_STATUS_USAGE_E;
            }
         }

         if(Parameter_Count >= 4)
         {
            if(Verify_Integer_Parameter(&(Parameter_List[3]), 11, 26))
            {
               ChannelMask = 1 << Parameter_List[3].Integer_Value;
            }
            else
            {
               Ret_Val = QCLI_STATUS_USAGE_E;
            }
         }

         if(Ret_Val == QCLI_STATUS_SUCCESS_E)
         {
            memset(&JoinConfig, 0, sizeof(qapi_ZB_Join_t));
            JoinConfig.IsRejoin                    = IsRejoin;
            JoinConfig.EndDeviceTimeout            = IsCoordinator ? 0 : DEFAULT_END_DEVICE_TIME_OUT;
            JoinConfig.NetworkConfig.ExtendedPanId = 0ULL;
            JoinConfig.NetworkConfig.StackProfile  = QAPI_ZB_STACK_PROFILE_PRO_E;
            JoinConfig.NetworkConfig.ScanAttempts  = 2;
            JoinConfig.NetworkConfig.Page          = 0;
            JoinConfig.NetworkConfig.ChannelMask   = ChannelMask;
            JoinConfig.NetworkConfig.Capability    = IsCoordinator ? COORDINATOR_CAPABILITIES : END_DEVICE_CAPABILITIES;

            QSOSAL_MemCopy_S(&(JoinConfig.NetworkConfig.Security), sizeof(qapi_ZB_Security_t), &Default_ZB_Secuity, sizeof(qapi_ZB_Security_t));

            /* Overwrite the default security level. */
            JoinConfig.NetworkConfig.Security.Security_Level = UseSecurity ? QAPI_ZB_SECURITY_LEVEL_ENC_MIC32_E : QAPI_ZB_SECURITY_LEVEL_NONE_E;

            Result = qapi_ZB_Join(ZigBee_Demo_Context.ZigBee_Handle, &JoinConfig);

            if(Result == QAPI_OK)
            {
               Display_Function_Success(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_Join");
            }
            else
            {
               Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_Join", Result);
               Ret_Val = QCLI_STATUS_ERROR_E;
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
      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "Reconnect" command to reconnect to the ZigBee network.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_Reconnect(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;
   qapi_Status_t         Result;
   qbool_t               TCRejoin;

   /* Verify the ZigBee layer had been initialized. */
   if(ZigBee_Demo_Context.ZigBee_Handle != NULL)
   {
      TCRejoin = false;
      Ret_Val  = QCLI_STATUS_SUCCESS_E;

      if(Parameter_Count >= 1)
      {
         if(Verify_Integer_Parameter(&(Parameter_List[0]), 0, 1))
         {
            TCRejoin = (qbool_t)(Parameter_List[0].Integer_Value != 0);
         }
         else
         {
            Ret_Val = QCLI_STATUS_USAGE_E;
         }
      }

      if(Ret_Val == QCLI_STATUS_SUCCESS_E)
      {
         Result = qapi_ZB_Reconnect(ZigBee_Demo_Context.ZigBee_Handle, TCRejoin);

         if(Result == QAPI_OK)
         {
            Display_Function_Success(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_Reconnect");
         }
         else
         {
            Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_Reconnect", Result);
            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
   }
   else
   {
      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "Leave" to tell the local device to leave the network.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_Leave(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;
   qapi_Status_t         Result;

   /* Verify the ZigBee layer had been initialized. */
   if(ZigBee_Demo_Context.ZigBee_Handle != NULL)
   {
      Result = qapi_ZB_Leave(ZigBee_Demo_Context.ZigBee_Handle);

      if(Result == QAPI_OK)
      {
         Display_Function_Success(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_Leave");
         Ret_Val = QCLI_STATUS_SUCCESS_E;
      }
      else
      {
         Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_Leave", Result);
         Ret_Val = QCLI_STATUS_ERROR_E;
      }
   }
   else
   {
      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "LeaveReq" command to send a leave request.

   Parameter_List[0] is the device ID to send the request to (NWK Address)
   Parameter_List[1] is the device ID for the device that needs to leave
                     (Extended Address).
   Parameter_List[1] is a flag indicating if the device should remove its
                     children.
   Parameter_List[2] is a flag indicating if the device should attempt to rejoin
                     the network.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_LeaveReq(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t   Ret_Val;
   qapi_Status_t           Result;
   ZB_Device_ID_t         *TargetDevice;
   ZB_Device_ID_t         *LeaveDevice;
   qbool_t                 RemoveChildren;
   qbool_t                 Rejoin;

   if(ZigBee_Demo_Context.ZigBee_Handle != NULL)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 4) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 1)) &&
         (Verify_Integer_Parameter(&(Parameter_List[3]), 0, 1)))
      {
         /* Get the device information. */
         TargetDevice   = GetDeviceListEntry(Parameter_List[0].Integer_Value);
         LeaveDevice    = GetDeviceListEntry(Parameter_List[1].Integer_Value);
         RemoveChildren = (qbool_t)(Parameter_List[2].Integer_Value);
         Rejoin         = (qbool_t)(Parameter_List[3].Integer_Value);

         if((TargetDevice != NULL) && (LeaveDevice != NULL))
         {
            /* Verify the address types for each specified device is valid. */
            if((TargetDevice->Type == QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E) && (LeaveDevice->Type == QAPI_ZB_ADDRESS_MODE_EXTENDED_ADDRESS_E))
            {
               Result = qapi_ZB_ZDP_Mgmt_Leave_Req(ZigBee_Demo_Context.ZigBee_Handle, TargetDevice->Address.ShortAddress, LeaveDevice->Address.ExtendedAddress, RemoveChildren, Rejoin);
               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Mgmt_Leave_Req");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_ZDP_Mgmt_Leave_Req", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Specified device is not a valid type.\n");
               Ret_Val = QCLI_STATUS_USAGE_E;
            }
         }
         else
         {
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Device ID is not valid.\n");
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
      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}
qapi_Status_t qapi_ZB_ZDP_Mgmt_Leave_Req(qapi_ZB_Handle_t ZB_Handle, uint16_t DstNwkAddr, uint64_t DeviceAddress, qbool_t RemoveChildren, qbool_t Rejoin);

/**
   @brief Executes the "PermitJoin" command to allow new devices to join the
          network.


   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_PermitJoin(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;
   qapi_Status_t         Result;

   /* Verify the ZigBee layer had been initialized. */
   if(ZigBee_Demo_Context.ZigBee_Handle != NULL)
   {
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0x00, 0xFF)))
      {
         Result = qapi_ZB_Permit_Join(ZigBee_Demo_Context.ZigBee_Handle, ((uint8_t)(Parameter_List[0].Integer_Value)));

         if(Result == QAPI_OK)
         {
            Display_Function_Success(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_Permit_Join");
            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }
         else
         {
            Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_Permit_Join", Result);
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
      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "GetNIB" command to read a NWK attribute.

   Parameter_List[0] Attribute ID to be read.
   Parameter_List[1] Attribute Index to read (default = 0).
   Parameter_List[2] Maximum size of the attribute (default = 128).

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_GetNIB(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t       Ret_Val;
   qapi_Status_t               Result;
   qapi_ZB_NIB_Attribute_ID_t  AttributeId;
   uint8_t                     AttributeIndex;
   uint16_t                    AttributeLength;
   void                       *AttributeValue;

   /* Check the parameters. */
   if(ZigBee_Demo_Context.ZigBee_Handle != NULL)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 0xFFFF)))
      {
         AttributeId     = (qapi_ZB_NIB_Attribute_ID_t)(Parameter_List[0].Integer_Value);
         AttributeIndex  = 0;
         AttributeLength = 128;

         if((Parameter_Count >= 2) && (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 0xFF)))
         {
            AttributeIndex = (uint8_t)(Parameter_List[1].Integer_Value);

            if((Parameter_Count >= 3) && (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFFFF)))
            {
               AttributeLength = (uint16_t)(Parameter_List[2].Integer_Value);
            }
         }

         /* Allocate a buffer for the attribute data. */
         AttributeValue = malloc(AttributeLength);
         if(AttributeValue != NULL)
         {
            Result = qapi_ZB_NLME_Get_Request(ZigBee_Demo_Context.ZigBee_Handle, AttributeId, AttributeIndex, &AttributeLength, (uint8_t *)AttributeValue);

            if(Result == QAPI_OK)
            {
               QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Attribute 0x%04X (%d bytes): ", AttributeId, AttributeLength);
               switch(AttributeLength)
               {
                  case sizeof(uint8_t):
                     QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "0x%02X", *(uint8_t *)AttributeValue);
                     break;

                  case sizeof(uint16_t):
                     QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "0x%04X", *(uint16_t *)AttributeValue);
                     break;

                  case sizeof(uint32_t):
                     QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "0x%08X", *(uint32_t *)AttributeValue);
                     break;

                  default:
                     QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "\n");
                     Dump_Data(ZigBee_Demo_Context.QCLI_Handle, "  ", AttributeLength, (uint8_t *)AttributeValue);
                     break;
               }

               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_NLME_Get_Request", Result);
               Ret_Val = QCLI_STATUS_ERROR_E;
            }

            free(AttributeValue);
         }
         else
         {
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Failed to allocate a buffer for the attribute.\n");
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
      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "GetNIB" command to write a NWK attribute.

   Parameter_List[0] Attribute ID to be write.
   Parameter_List[1] Attribute Index to write.
   Parameter_List[2] Size of the attribute being written in bytes.
   Parameter_List[3] Value for the attribute.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_SetNIB(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t       Ret_Val;
   qapi_Status_t               Result;
   qapi_ZB_NIB_Attribute_ID_t  AttributeId;
   uint8_t                     AttributeIndex;
   uint32_t                    AttributeLength;
   uint32_t                    TempLength;
   uint8_t                    *AttributeValue;

   /* Check the parameters. */
   if(ZigBee_Demo_Context.ZigBee_Handle != NULL)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 4) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFFFF)))
      {
         AttributeId     = (qapi_ZB_NIB_Attribute_ID_t)(Parameter_List[0].Integer_Value);
         AttributeIndex  = (uint8_t)(Parameter_List[1].Integer_Value);
         AttributeLength = (uint32_t)(Parameter_List[2].Integer_Value);

         /* Allocate a buffer for the attribute data. */
         AttributeValue = malloc(AttributeLength);
         if(AttributeValue != NULL)
         {
            TempLength = AttributeLength;
            if(Hex_String_To_Array(Parameter_List[3].String_Value, &TempLength, AttributeValue))
            {
               Result = qapi_ZB_NLME_Set_Request(ZigBee_Demo_Context.ZigBee_Handle, AttributeId, AttributeIndex, AttributeLength, AttributeValue);

               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_NLME_Set_Request");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_NLME_Set_Request", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Failed to parse the attribute value.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }

            free(AttributeValue);
         }
         else
         {
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Failed to allocate a buffer for the attribute.\n");
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
      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "GetAIB" command to read a APS attribute.

   Parameter_List[0] Attribute ID to be read.
   Parameter_List[1] Attribute Index to read.
   Parameter_List[2] Maximum size of the attribute.  Defaults to 128 if not
                     specified.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_GetAIB(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t       Ret_Val;
   qapi_Status_t               Result;
   qapi_ZB_AIB_Attribute_ID_t  AttributeId;
   uint8_t                     AttributeIndex;
   uint16_t                    AttributeLength;
   void                       *AttributeValue;

   /* Check the parameters. */
   if(ZigBee_Demo_Context.ZigBee_Handle != NULL)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 0xFFFF)))
      {
         AttributeId     = (qapi_ZB_AIB_Attribute_ID_t)(Parameter_List[0].Integer_Value);
         AttributeIndex  = 0;
         AttributeLength = 128;

         if((Parameter_Count >= 2) && (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 0xFF)))
         {
            AttributeIndex = (uint8_t)(Parameter_List[1].Integer_Value);

            if((Parameter_Count >= 3) && (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFFFF)))
            {
               AttributeLength = (uint16_t)(Parameter_List[2].Integer_Value);
            }
         }

         /* Allocate a buffer for the attribute data. */
         AttributeValue = malloc(AttributeLength);
         if(AttributeValue != NULL)
         {
            Result = qapi_ZB_APSME_Get_Request(ZigBee_Demo_Context.ZigBee_Handle, AttributeId, AttributeIndex, &AttributeLength, (uint8_t *)AttributeValue);

            if(Result == QAPI_OK)
            {
               QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Attribute %d (%d bytes): ", AttributeId, AttributeLength);
               switch(AttributeLength)
               {
                  case sizeof(uint8_t):
                     QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "0x%02X", *(uint8_t *)AttributeValue);
                     break;

                  case sizeof(uint16_t):
                     QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "0x%04X", *(uint16_t *)AttributeValue);
                     break;

                  case sizeof(uint32_t):
                     QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "0x%08X", *(uint32_t *)AttributeValue);
                     break;

                  default:
                     QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "\n");
                     Dump_Data(ZigBee_Demo_Context.QCLI_Handle, "  ", AttributeLength, (uint8_t *)AttributeValue);
                     break;
               }

               Ret_Val = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_APSME_Get_Request", Result);
               Ret_Val = QCLI_STATUS_ERROR_E;
            }

            free(AttributeValue);
         }
         else
         {
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Failed to allocate a buffer for the attribute.\n");
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
      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "GetAIB" command to write a APS attribute.

   Parameter_List[0] Attribute ID to be write.
   Parameter_List[1] Attribute Index to write.
   Parameter_List[2] Size of the attribute being written in bytes.
   Parameter_List[3] Value for the attribute.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_SetAIB(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t       Ret_Val;
   qapi_Status_t               Result;
   qapi_ZB_AIB_Attribute_ID_t  AttributeId;
   uint8_t                     AttributeIndex;
   uint32_t                    AttributeLength;
   uint32_t                    TempLength;
   uint8_t                    *AttributeValue;

   /* Check the parameters. */
   if(ZigBee_Demo_Context.ZigBee_Handle != NULL)
   {
      /* Check the parameters. */
      if((Parameter_Count >= 4) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, 0xFFFF)))
      {
         AttributeId     = (qapi_ZB_AIB_Attribute_ID_t)(Parameter_List[0].Integer_Value);
         AttributeIndex  = (uint8_t)(Parameter_List[1].Integer_Value);
         AttributeLength = (uint32_t)(Parameter_List[2].Integer_Value);

         /* Allocate a buffer for the attribute data. */
         AttributeValue = malloc(AttributeLength);
         if(AttributeValue != NULL)
         {
            TempLength = AttributeLength;
            if(Hex_String_To_Array(Parameter_List[3].String_Value, &TempLength, AttributeValue))
            {
               Result = qapi_ZB_APSME_Set_Request(ZigBee_Demo_Context.ZigBee_Handle, AttributeId, AttributeIndex, AttributeLength, AttributeValue);

               if(Result == QAPI_OK)
               {
                  Display_Function_Success(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_APSME_Set_Request");
                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_APSME_Set_Request", Result);
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
            }
            else
            {
               QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Failed to parse the attribute value.\n");
               Ret_Val = QCLI_STATUS_ERROR_E;
            }

            free(AttributeValue);
         }
         else
         {
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Failed to allocate a buffer for the attribute.\n");
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
      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SetBIB" command.

   Parameter_List[0] is the ID of BDB attributes. (AttrId)
   Parameter_List[1] is the index within an attribute identified by the AttrId.
                     (AttrIndex)
   Parameter_List[2] is the length of the attribute. (AttrLength)
   Parameter_List[3] is the value of the attribute. (AttrValue)

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_SetBIB(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_Status_t          Result;
   uint32_t               MaxValue;
   uint64_t               AttrValueULL;
   uint16_t               AttrId;
   uint8_t                AttrIndex;
   uint16_t               AttrLength;
   uint8_t               *AttrValue;

   if(ZigBee_Demo_Context.ZigBee_Handle != NULL)
   {
      if((Parameter_Count >= 4) &&
         (Verify_Integer_Parameter(&Parameter_List[0], 0x0000, 0xFFFF)) &&
         (Verify_Integer_Parameter(&Parameter_List[1], 0x00, 0xFF)) &&
         (Verify_Integer_Parameter(&Parameter_List[2], 1, 0xFF))
         )
      {
         AttrId     = (uint16_t)(Parameter_List[0].Integer_Value);
         AttrIndex  = (uint8_t)(Parameter_List[1].Integer_Value);
         AttrLength = (uint8_t)(Parameter_List[2].Integer_Value);
         switch(AttrLength)
         {
            case sizeof(uint8_t):
            case sizeof(uint16_t):
            case sizeof(uint32_t):
               /* Handle the basic integer types.                       */
               MaxValue = 0xFFFFFFFF >> ((sizeof(uint32_t) - AttrLength) * 8);

               if(Verify_Unsigned_Integer_Parameter(&(Parameter_List[3]), 0, MaxValue))
               {
                  AttrValue = (uint8_t *)&(Parameter_List[3].Integer_Value);

                  Ret_Val = QCLI_STATUS_SUCCESS_E;
               }
               else
               {
                  Ret_Val = QCLI_STATUS_ERROR_E;
               }
               break;

            case sizeof(uint64_t):
                  /* Attempt to convert the string to a 64-bit integer. */
                  if(Hex_String_To_ULL(Parameter_List[3].String_Value, &AttrValueULL))
                  {
                     AttrValue = (uint8_t *)&AttrValueULL;

                     Ret_Val = QCLI_STATUS_SUCCESS_E;
                  }
                  else
                  {
                     Ret_Val = QCLI_STATUS_ERROR_E;
                  }
               break;

            default:
               Ret_Val = QCLI_STATUS_ERROR_E;
               break;
         }

         if(Ret_Val == QCLI_STATUS_SUCCESS_E)
         {
            Result = qapi_ZB_BDB_Set_Request(ZigBee_Demo_Context.ZigBee_Handle , AttrId, AttrIndex, AttrLength, AttrValue);
            if(Result == QAPI_OK)
            {
               Display_Function_Success(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_BDB_Set_Request");
            }
            else
            {
               Ret_Val = QCLI_STATUS_ERROR_E;
               Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_BDB_Set_Request", Result);
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
      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Zigbee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "GetBIB" command.

   Parameter_List[0] is the ID of BDB attributes. (AttrId)
   Parameter_List[1] is the index within an attribute identified by the AttrId.
                     (AttrIndex)
   Parameter_List[2] is the length of the attribute. (AttrLength)

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_GetBIB(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t  Ret_Val;
   qapi_Status_t          Result;
   uint16_t               AttrId;
   uint8_t                AttrIndex;
   uint16_t               AttrLength;
   uint8_t                AttrValue[MAXIMUM_ATTRIUBTE_LENGTH];

   if(ZigBee_Demo_Context.ZigBee_Handle != NULL)
   {
      /* Check the parameters.                                          */
      if((Parameter_Count >= 3) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), 0x0000, 0xFFFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[1]), 0x00, 0xFF)) &&
         (Verify_Integer_Parameter(&(Parameter_List[2]), 0, sizeof(AttrValue)))
        )
      {
         /* Get the device information.                                 */
         AttrId     = (uint16_t)(Parameter_List[0].Integer_Value);
         AttrIndex  = (uint8_t)(Parameter_List[1].Integer_Value);
         AttrLength = (uint16_t)(Parameter_List[2].Integer_Value);

         Result = qapi_ZB_BDB_Get_Request(ZigBee_Demo_Context.ZigBee_Handle, (qapi_ZB_BDB_Attribute_ID_t)AttrId, AttrIndex, &AttrLength, AttrValue);
         switch(Result)
         {
            case QAPI_ERR_BOUNDS:
               Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_BDB_Get_Request", Result);
               QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "AttrLength: %d\n", AttrLength);
               Ret_Val = QCLI_STATUS_USAGE_E;
               break;

            case QAPI_OK:
               Display_Function_Success(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_BDB_Get_Request");
               QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "   AttrId:     0x%04X\n", AttrId);
               QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "   AttrLength: %d\n", AttrLength);
               QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "   AttrValue:  ");
               DisplayVariableLengthValue(ZigBee_Demo_Context.QCLI_Handle, AttrLength, AttrValue);
               Ret_Val = QCLI_STATUS_SUCCESS_E;
               break;

            default:
               Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_BDB_Get_Request", Result);
               Ret_Val = QCLI_STATUS_ERROR_E;
               break;
         }
      }
      else
      {
         Ret_Val = QCLI_STATUS_USAGE_E;
      }
   }
   else
   {
      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SetExtAddress" command to set the extended address of
          the local ZigBee interface.

   Parameter_List[0] EUI64 address to assign to the ZigBee interface.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_SetExtAddress(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;
   uint64_t              ExtendedAddress;
   qapi_Status_t         Result;

   /* Verify the ZigBee layer had been initialized. */
   if(ZigBee_Demo_Context.ZigBee_Handle != NULL)
   {
      if((Parameter_Count >= 1) &&
         (Hex_String_To_ULL(Parameter_List[0].String_Value, &ExtendedAddress)))
      {
         Result = qapi_ZB_Set_Extended_Address(ZigBee_Demo_Context.ZigBee_Handle, ExtendedAddress);

         if(Result == QAPI_OK)
         {
            Display_Function_Success(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_Set_Extended_Address");
            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }
         else
         {
            Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_Set_Extended_Address", Result);
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
      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "GetAddresses" command to get the addresses of the local
          ZigBee interface.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_GetAddresses(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;
   qapi_Status_t         Result;
   uint64_t              ExtendedAddress;
   uint16_t              ShortAddress;
   uint16_t              AttributeLength;

   /* Verify the ZigBee layer had been initialized. */
   if(ZigBee_Demo_Context.ZigBee_Handle != NULL)
   {
      Result = qapi_ZB_Get_Extended_Address(ZigBee_Demo_Context.ZigBee_Handle, &ExtendedAddress);
      if(Result == QAPI_OK)
      {
         AttributeLength = sizeof(uint16_t);
         Result = qapi_ZB_NLME_Get_Request(ZigBee_Demo_Context.ZigBee_Handle, QAPI_ZB_NIB_ATTRIBUTE_ID_NWK_NETWORK_ADDRESS_E, 0, &AttributeLength, (uint8_t *)&ShortAddress);
         if((Result == QAPI_OK) && (AttributeLength == sizeof(uint16_t)))
         {
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Extended Address: %08X%08X\n", (uint32_t)(ExtendedAddress >> 32), (uint32_t)ExtendedAddress);
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Short Address:    0x%04X\n", ShortAddress);
            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }
         else
         {
            Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_NLME_Get_Request", Result);
            Ret_Val = QCLI_STATUS_ERROR_E;
         }
      }
      else
      {
         Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_Get_Extended_Address", Result);
         Ret_Val = QCLI_STATUS_ERROR_E;
      }
   }
   else
   {
      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "SetTxPower" command to set the ZigBee Tx power.

   Parameter_List[0] The Tx power to be set.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_SetTxPower(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;
   qapi_Status_t         Result;
   int8_t                TxPower;

   /* Verify the ZigBee layer had been initialized. */
   if(ZigBee_Demo_Context.ZigBee_Handle != NULL)
   {
      if((Parameter_Count >= 1) &&
         (Verify_Integer_Parameter(&(Parameter_List[0]), -32, 31))
        )
      {
         TxPower = (int8_t)(Parameter_List[0].Integer_Value);
         Result  = qapi_ZB_Set_Transmit_Power(ZigBee_Demo_Context.ZigBee_Handle, TxPower);
         if(Result == QAPI_OK)
         {
            Display_Function_Success(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_Set_Transmit_Power");
            Ret_Val = QCLI_STATUS_SUCCESS_E;
         }
         else
         {
            Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_Set_Transmit_Power", Result);
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
      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);}

/**
   @brief Executes the "GetTxPower" command to get the ZigBee Tx power.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_GetTxPower(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t Ret_Val;
   qapi_Status_t         Result;
   int8_t                TxPower;

   /* Verify the ZigBee layer had been initialized. */
   if(ZigBee_Demo_Context.ZigBee_Handle != NULL)
   {
      Result = qapi_ZB_Get_Transmit_Power(ZigBee_Demo_Context.ZigBee_Handle, &TxPower);
      if(Result == QAPI_OK)
      {
         QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "TxPower: %d\n", TxPower);
      }
      else
      {
         Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_Get_Transmit_Power", Result);
         Ret_Val = QCLI_STATUS_ERROR_E;
      }
   }
   else
   {
      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "ZigBee stack is not initialized.\n");
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
}

/**
   @brief Executes the "ClearPersist" command to clear the perisistent ZigBee
          data from flash.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ZB_ClearPersist(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
#if 0
   QCLI_Command_Status_t Ret_Val;
   qapi_Status_t         Result;
   qapi_Persist_Handle_t PersistHandle;

   /* Get the handle for the persistent data. */
   if(ZigBee_Demo_Context.PersistHandle != NULL)
   {
      /* Use the persist handle we already have. */
      PersistHandle = ZigBee_Demo_Context.PersistHandle;
   }
   else
   {
      /* Initialize a temporary persist instance. */
      Result = qapi_Persist_Initialize(&PersistHandle, ZIGBEE_PERSIST_DIRECTORY, ZIGBEE_PERSIST_PREFIX, ZIGBEE_PERSIST_SUFFIX, NULL, 0);
      if(Result != QAPI_OK)
      {
         PersistHandle = NULL;
         Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_Persist_Initialize", Result);
      }
   }

   if(PersistHandle != NULL)
   {
      qapi_Persist_Delete(PersistHandle);

      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Persistent data cleared.\n");

      /* Cleanup the persist instance if it was temporary. */
      if(ZigBee_Demo_Context.PersistHandle == NULL)
      {
         qapi_Persist_Cleanup(PersistHandle);
      }

      Ret_Val = QCLI_STATUS_SUCCESS_E;
   }
   else
   {
      Ret_Val = QCLI_STATUS_ERROR_E;
   }

   return(Ret_Val);
#endif
   return(QCLI_STATUS_ERROR_E);
}

/**
   @brief Callback handler for the ZigBee stack.

   @param ZB_Handle is the handle of the ZigBee instance was returned by a
                    successful call to qapi_ZB_Initialize().
   @param CB_Param  is the user specified parameter for the callback function.
*/
static void ZB_Event_CB(qapi_ZB_Handle_t ZB_Handle, const qapi_ZB_Event_t *ZB_Event, uint32_t CB_Param)
{
   if((ZigBee_Demo_Context.ZigBee_Handle != NULL) && (ZB_Handle == ZigBee_Demo_Context.ZigBee_Handle) && (ZB_Event != NULL))
   {
      switch(ZB_Event->Event_Type)
      {
         case QAPI_ZB_EVENT_TYPE_FORM_CONFIRM_E:
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Form confirm:\n");
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "  Status:  %d\n", ZB_Event->Event_Data.Form_Confirm.Status);
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "  Channel: %d\n", ZB_Event->Event_Data.Form_Confirm.ActiveChannel);
            break;

         case QAPI_ZB_EVENT_TYPE_JOIN_CONFIRM_E:
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Join confirm:\n");
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "  Status:        %d\n", ZB_Event->Event_Data.Join_Confirm.Status);
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "  NwkAddress:    0x%04X\n", ZB_Event->Event_Data.Join_Confirm.NwkAddress);
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "  ExtendedPanId: %08X%08X\n", (uint32_t)(ZB_Event->Event_Data.Join_Confirm.ExtendedPanId >> 32), (uint32_t)(ZB_Event->Event_Data.Join_Confirm.ExtendedPanId));
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "  Channel:       %d\n", ZB_Event->Event_Data.Join_Confirm.ActiveChannel);
            break;

         case QAPI_ZB_EVENT_TYPE_RECONNECT_CONFIRM_E:
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Reconnect confirm:\n");
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "  Status:          %d\n", ZB_Event->Event_Data.Reconnect_Confirm.Status);
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "  Network Address: 0x%04X\n", ZB_Event->Event_Data.Reconnect_Confirm.NwkAddress);
            break;

         case QAPI_ZB_EVENT_TYPE_LEAVE_CONFIRM_E:
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Leave confirm:\n");
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "  Status: %d\n", ZB_Event->Event_Data.Leave_Confirm.Status);
            break;

         case QAPI_ZB_EVENT_TYPE_LEAVE_IND_E:
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Leave indication:\n");
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "  ExtendedAddress: %08X%08X\n", (uint32_t)(ZB_Event->Event_Data.Leave_Ind.ExtendedAddress >> 32), (uint32_t)(ZB_Event->Event_Data.Leave_Ind.ExtendedAddress));
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "  NetworkAddress:  0x%04X\n", ZB_Event->Event_Data.Leave_Ind.NwkAddress);
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "  Rejoin:          %s\n", (ZB_Event->Event_Data.Leave_Ind.Rejoin) ? "true" : "false");
            break;

         case QAPI_ZB_EVENT_TYPE_FACTORY_RESET_E:
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Factory Reset\n");
            break;

         default:
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Unhandled ZigBee Event: %d\n", ZB_Event->Event_Type);
            break;
      }

      QCLI_Display_Prompt();
   }
}

/**
   @brief Callback handler for persist notify indications.

   @param[in] ZB_Handle Handle of the ZigBee instance that was returned by a
                        successful call to qapi_ZB_Initialize().
   @param[in] CB_Param  User-specified parameter for the callback function.
*/
static void ZB_Persist_Notify_CB(qapi_ZB_Handle_t ZB_Handle, uint32_t CB_Param)
{
#if 0
   qapi_Status_t          Result;
   qapi_Persist_Handle_t  PersistHandle;
   uint32_t               PersistLength;
   uint8_t               *PersistData;

   if((ZB_Handle == ZigBee_Demo_Context.ZigBee_Handle) && (CB_Param != 0))
   {
      PersistHandle = (qapi_Persist_Handle_t)CB_Param;

      PersistLength = 0;
      Result = qapi_ZB_Get_Persistent_Data(ZB_Handle, NULL, &PersistLength);

      if((Result == QAPI_ERR_BOUNDS) && (PersistLength > 0))
      {
         PersistData = (uint8_t *)malloc(PersistLength);
         if(PersistData != NULL)
         {
            Result = qapi_ZB_Get_Persistent_Data(ZB_Handle, PersistData, &PersistLength);
            if(Result == QAPI_OK)
            {
               Result = qapi_Persist_Put(PersistHandle, PersistLength, PersistData);
               if(Result != QAPI_OK)
               {
                  Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_Persist_Put", Result);
               }
            }
            else
            {
               Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_Get_Persistent_Data", Result);
            }

            free(PersistData);
         }
         else
         {
            QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Failed to allocate persist data buffer.", Result);
         }
      }
      else
      {
         Display_Function_Error(ZigBee_Demo_Context.QCLI_Handle, "qapi_ZB_Get_Persistent_Data", Result);
      }
   }
#endif
}

/**
   @brief Registers ZigBee demo commands with QCLI.
*/
void Initialize_ZigBee_Demo(void)
{
   memset(&ZigBee_Demo_Context, 0, sizeof(ZigBee_Demo_Context_t));

   /* Initialize device list entry 0 to empty. */
   ZigBee_Demo_Context.DevIDList[0].InUse = true;
   ZigBee_Demo_Context.DevIDList[0].Type  = QAPI_ZB_ADDRESS_MODE_NONE_E;

   /* Register top level zigbee command group. */
   ZigBee_Demo_Context.QCLI_Handle = QCLI_Register_Command_Group(NULL, &ZigBee_CMD_Group);
   if(ZigBee_Demo_Context.QCLI_Handle != NULL)
   {
      /* Initialize ZDP demo. */
      Initialize_ZDP_Demo(ZigBee_Demo_Context.QCLI_Handle);

      /* Initialize ZCL demo. */
      Initialize_ZCL_Demo(ZigBee_Demo_Context.QCLI_Handle);

      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "ZigBee Demo Initialized.\n");
   }
   else
   {
      QCLI_Printf(ZigBee_Demo_Context.QCLI_Handle, "Failed to register ZigBee commands.\n");
   }
}

/**
   @brief Un-Registers the ZigBee interface commands with QCLI.
*/
void Cleanup_ZigBee_Demo(void)
{
   if(ZigBee_Demo_Context.QCLI_Handle)
   {
      /* Un-register the Zigbee Group.                                  */
      /* * NOTE * This function will un-register all sub-groups.        */
      QCLI_Unregister_Command_Group(ZigBee_Demo_Context.QCLI_Handle);

      ZigBee_Demo_Context.QCLI_Handle = NULL;
   }
}

/**
   @brief Helper function to format the send information for a packet.

   @param DeviceIndex is the index of the device to be sent.
   @param SendInfo    is a pointer to where the send information will be
                      formatted upon successful return.

   @return true if the send info was formatted successfully, false otherwise.
*/
qbool_t Format_Send_Info_By_Device(uint32_t DeviceIndex, qapi_ZB_CL_General_Send_Info_t *SendInfo)
{
   qbool_t         Ret_Val;
   ZB_Device_ID_t *DeviceEntry;

   DeviceEntry = GetDeviceListEntry(DeviceIndex);
   if((SendInfo != NULL) && (DeviceEntry != NULL))
   {
      /* Found the device, now determine if it is a short address or a
         group address (no extended addresses used at this layer). */
      SendInfo->DstAddrMode = DeviceEntry->Type;
      SendInfo->DstAddress  = DeviceEntry->Address;
      SendInfo->DstEndpoint = DeviceEntry->Endpoint;
      SendInfo->SeqNum      = GetNextSeqNum();

      Ret_Val = true;
   }
   else
   {
      Ret_Val = false;
   }

   return(Ret_Val);
}

/**
   @brief Helper function to format the send information for a packet.

   @param ReceiveInfo is the receive information for an event.
   @param SendInfo    is a pointer to where the send information will be
                      formatted upon successful return.

   @return true if the send info was formatted successfully, false otherwise.
*/
qbool_t Format_Send_Info_By_Receive_Info(const qapi_ZB_CL_General_Receive_Info_t *ReceiveInfo, qapi_ZB_CL_General_Send_Info_t *SendInfo)
{
   qbool_t Ret_Val;

   if((SendInfo != NULL) && (ReceiveInfo != NULL))
   {
      /* Found the device, now determine if it is a short address or a
         group address (no extended addresses used at this layer). */
      SendInfo->DstAddrMode             = QAPI_ZB_ADDRESS_MODE_SHORT_ADDRESS_E;
      SendInfo->DstAddress.ShortAddress = ReceiveInfo->SrcNwkAddress;
      SendInfo->DstEndpoint             = ReceiveInfo->SrcEndpoint;
      SendInfo->SeqNum                  = GetNextSeqNum();

      Ret_Val = true;
   }
   else
   {
      Ret_Val = false;
   }

   return(Ret_Val);
}

/**
   @brief Function to get a specified entry from the ZigBee demo's device list.

   @param DeviceID is the index of the device to retrieve.

   @return a pointer to the device list entry or NULL if either the DeviceID was
           not valid or not in use.
*/
ZB_Device_ID_t *GetDeviceListEntry(uint32_t DeviceID)
{
   ZB_Device_ID_t *Ret_Val;

   if(DeviceID <= DEV_ID_LIST_SIZE)
   {
      if(ZigBee_Demo_Context.DevIDList[DeviceID].InUse)
      {
         Ret_Val = &(ZigBee_Demo_Context.DevIDList[DeviceID]);
      }
      else
      {
         Ret_Val = NULL;
      }
   }
   else
   {
      Ret_Val = NULL;
   }

   return(Ret_Val);
}

/**
   @brief Function to get the ZigBee stack's handle.

   @return The handle of the ZigBee stack.
*/
qapi_ZB_Handle_t GetZigBeeHandle(void)
{
   return(ZigBee_Demo_Context.ZigBee_Handle);
}

/**
   @brief Function to get the next sequence number for sending packets.

   @return the next sequence number to be used for sending packets.
*/
uint8_t GetNextSeqNum(void)
{
   uint8_t Ret_Val = ZigBee_Demo_Context.ZCL_Sequence_Num;

   ZigBee_Demo_Context.ZCL_Sequence_Num++;
   return Ret_Val;
}

/**
   @brief Function to get the QCLI handle for the ZigBee demo.

   @return The QCLI handled used by the ZigBee demo.
*/
QCLI_Group_Handle_t GetZigBeeQCLIHandle(void)
{
   return(ZigBee_Demo_Context.QCLI_Handle);
}

/**
   @brief Helper function that displays a variable length value.

   @param Group_Handle is the QCLI group handle.
   @param Data_Length  is the length of the data to be displayed.
   @param Data         is the data to be displayed.
*/
void DisplayVariableLengthValue(QCLI_Group_Handle_t Group_Handle, uint16_t Data_Length, const uint8_t *Data)
{
   union
   {
      uint8_t  Uint8;
      uint16_t Uint16;
      uint32_t Uint32;
      uint64_t Uint64;
   } Value;

   if(Data != NULL)
   {
      switch(Data_Length)
      {
         case 1:
            Value.Uint8 = READ_UNALIGNED_LITTLE_ENDIAN_UINT8(Data);
            QCLI_Printf(Group_Handle, "0x%02X\n", Value.Uint8);
            break;

         case 2:
            Value.Uint16 = READ_UNALIGNED_LITTLE_ENDIAN_UINT16(Data);
            QCLI_Printf(Group_Handle, "0x%04X\n", Value.Uint16);
            break;

         case 3:
            Value.Uint32 = READ_UNALIGNED_LITTLE_ENDIAN_UINT24(Data);
            QCLI_Printf(Group_Handle, "0x%06X\n", Value.Uint32 & 0xFFFFFF);
            break;

         case 4:
            Value.Uint32 = READ_UNALIGNED_LITTLE_ENDIAN_UINT32(Data);
            QCLI_Printf(Group_Handle, "0x%08X\n", Value.Uint32);
            break;

         case 5:
            Value.Uint64 = READ_UNALIGNED_LITTLE_ENDIAN_UINT40(Data);
            QCLI_Printf(Group_Handle, "0x%02X%08X\n", (uint8_t)(Value.Uint64 >> 32), (uint32_t)(Value.Uint64));
            break;

         case 6:
            Value.Uint64 = READ_UNALIGNED_LITTLE_ENDIAN_UINT48(Data);
            QCLI_Printf(Group_Handle, "0x%04X%08X\n", (uint16_t)(Value.Uint64 >> 32), (uint32_t)(Value.Uint64));
            break;

         case 7:
            Value.Uint64 = READ_UNALIGNED_LITTLE_ENDIAN_UINT56(Data);
            QCLI_Printf(Group_Handle, "0x%06X%08X\n", (uint32_t)((Value.Uint64 >> 32)) & 0xFFFFFF, (uint32_t)(Value.Uint64));
            break;

         case 8:
            Value.Uint64 = READ_UNALIGNED_LITTLE_ENDIAN_UINT64(Data);
            QCLI_Printf(Group_Handle, "%08X%08X\n", (uint32_t)(Value.Uint64 >> 32), (uint32_t)(Value.Uint64));
            break;

         default:
            QCLI_Printf(Group_Handle, "Unsupported Data Length.\n");
            break;
      }
   }
}

