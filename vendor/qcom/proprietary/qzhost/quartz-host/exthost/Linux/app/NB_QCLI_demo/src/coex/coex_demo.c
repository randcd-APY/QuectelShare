/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "string.h"
#include "malloc.h"
#include "qapi_wlan_base.h"
#include "qapi_coex.h"
#include "qcli_api.h"
#include "qcli_util.h"
#include "qapi_tlmm.h"

/* Defines the number of priority values to set. */
#define CONFIG_PRIORITY_LENGTH                  (15)

/* Defines coex statistics parameters. */
#define COEX_STATISTICS_MASK                    (0xFFFFFFFF)
#define COEX_STATISTICS_LENGTH                  (32)

/* Defines pin and function numbers for PTA interface. */
#define EPTA_PIN_NUMBER_STD_CONFIG_1            (5)
#define EPTA_PIN_NUMBER_STD_CONFIG_2            (6)
#define EPTA_PIN_NUMBER_STD_CONFIG_3            (7)
#define EPTA_PIN_FUNCTION_STD_CONFIG_1          (1)
#define EPTA_PIN_FUNCTION_STD_CONFIG_2          (1)
#define EPTA_PIN_FUNCTION_STD_CONFIG_3          (1)

#define EPTA_PIN_NUMBER_ALT_CONFIG_1            (16)
#define EPTA_PIN_NUMBER_ALT_CONFIG_2            (17)
#define EPTA_PIN_NUMBER_ALT_CONFIG_3            (60)
#define EPTA_PIN_FUNCTION_ALT_CONFIG_1          (12)
#define EPTA_PIN_FUNCTION_ALT_CONFIG_2          (12)
#define EPTA_PIN_FUNCTION_ALT_CONFIG_3          (9)

/* This structure represents the contextual information for the coex demo
   application. */
typedef struct Coex_Demo_Context_s
{
   QCLI_Group_Handle_t         QCLI_Handle;
   qapi_COEX_Priority_Config_t Priorities[CONFIG_PRIORITY_LENGTH];
} Coex_Demo_Context_t;

static Coex_Demo_Context_t  Coex_Demo_Context;

   /* Define the GPIO for Coex. */
static qapi_TLMM_Config_t EPTA_Pin_1_Std_Config;
static qapi_TLMM_Config_t EPTA_Pin_2_Std_Config;
static qapi_TLMM_Config_t EPTA_Pin_3_Std_Config;
static qapi_GPIO_ID_t     EPTA_Pin_1_Std_ID;
static qapi_GPIO_ID_t     EPTA_Pin_2_Std_ID;
static qapi_GPIO_ID_t     EPTA_Pin_3_Std_ID;
static qapi_TLMM_Config_t EPTA_Pin_1_Alt_Config;
static qapi_TLMM_Config_t EPTA_Pin_2_Alt_Config;
static qapi_TLMM_Config_t EPTA_Pin_3_Alt_Config;
static qapi_GPIO_ID_t     EPTA_Pin_1_Alt_ID;
static qapi_GPIO_ID_t     EPTA_Pin_2_Alt_ID;
static qapi_GPIO_ID_t     EPTA_Pin_3_Alt_ID;
static boolean            EPTA_Std_GPIO_Initialized;
static boolean            EPTA_Alt_GPIO_Initialized;

static QCLI_Command_Status_t cmd_Configure(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_ConfigureAdvanced(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_SetBLEPriority(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_SetI15P4Priority(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_SetEXTPriority(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_StatisticsEnable(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_StatisticsGet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_EPTAGPIOEnable(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_configureWlanCoex(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cmd_GetWlanCoexStats(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

/* The following is the complete command list for the coexistence demo. */
const QCLI_Command_t Coex_CMD_List[] =
{
    // cmd_function                 thread cmd_string                  usage_string                                                                                                                                                                                                                                                                                                                description
    {cmd_Configure,                 false, "Configure",                "[Config Flags] [Priority 1] [Priority 2] [Priority 3] [Priority 4] ",                                                                                                                                                                                                                                                      "Configures coexistence."},
    {cmd_ConfigureAdvanced,         false, "ConfigureAdvanced",        "[Config Flags] [Concurrency Flags] ",                                                                                                                                                                                                                                                                                      "Configures coexistence, advanced."},
    {cmd_SetBLEPriority,            false, "SetBLEPriority",           "[Adv] [Scan] [DataRequest] [DataActive]",                                                                                                                                                                                                                                                                                  "Sets BLE priorities for advanced configuration."},
    {cmd_SetI15P4Priority,          false, "SetI15P4Priority",         "[RxRequest] [TxRequest] [RxActive] [TxActive] [Ack] [EDScan] [HoldRequest]",                                                                                                                                                                                                                                               "Sets 802.15.4 priorities for advanced configuration."},
    {cmd_SetEXTPriority,            false, "SetEXTPriority",           "[LowRequest] [HighRequest] [LowActive] [HighActive]",                                                                                                                                                                                                                                                                      "Sets EXT priorities for advanced configuration."},
    {cmd_StatisticsEnable,          false, "StatisticsEnable",         "[Enable (0=Disable, 1=Enable)]",                                                                                                                                                                                                                                                                                           "Enables coexistence statistics."},
    {cmd_StatisticsGet,             false, "StatisticsGet",            "[Reset (0=FALSE, 1 = TRUE)]",                                                                                                                                                                                                                                                                                              "Retrieves coexistence statistics."},
    {cmd_EPTAGPIOEnable,            false, "EPTAGPIOEnable",           "[Mode (0=Disable, 1=Slave (External WiFi), 2=Master (External Bluetooth))] [GPIO Option (0=Standard (5,6,7), 1=Alternate (16,17,60)) (optional)]",                                                                                                                                                                         "Configures the EPTA GPIO."},
    {cmd_configureWlanCoex,         false, "EnableWlanCoex",           "<WLAN coex enablement. enable|disable> <coex mode. 3w|pta|epta> <profile-specific param1> <profile type. 1-15> <profile-specific param2>",                                                                                                                                                                                 "Enable and configure coex or disable coex. No subsequent parameters if 'coex enablement' is 'disable'"},
    {cmd_GetWlanCoexStats,          false, "GetWlanCoexStats",         "<0=Don't reset counters, 1=Reset counters>",                                                                                                                                                                                                                                                                               "Retrieve WLAN coex stats; optionally reset counters after fetching stats"},
};

const QCLI_Command_Group_t Coex_CMD_Group = {"Coex", sizeof(Coex_CMD_List) / sizeof(QCLI_Command_t), Coex_CMD_List};

/**
   @brief Executes the "Configure" command to configure the coexistence module.

   Parameter_List[0] Configuration flags.
   Parameter_List[1] Priority 1 value - 1 = BLE, 2 = I15P4, 3 = EXT, 4 = WLAN.
   Parameter_List[2] Priority 2 value - 1 = BLE, 2 = I15P4, 3 = EXT, 4 = WLAN.
   Parameter_List[3] Priority 3 value - 1 = BLE, 2 = I15P4, 3 = EXT, 4 = WLAN.
   Parameter_List[4] Priority 4 value - 1 = BLE, 2 = I15P4, 3 = EXT, 4 = WLAN.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_Configure(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t           Result;
   QCLI_Command_Status_t   RetVal;
   qapi_COEX_Config_Data_t ConfigData;

   /* Verify the input parameters. */
   if(Parameter_Count >= 5)
   {
      /* Set the configuration data. */
      ConfigData.config_Flags = Parameter_List[0].Integer_Value;
      ConfigData.priority_1   = Parameter_List[1].Integer_Value;
      ConfigData.priority_2   = Parameter_List[2].Integer_Value;
      ConfigData.priority_3   = Parameter_List[3].Integer_Value;
      ConfigData.priority_4   = Parameter_List[4].Integer_Value;

      /* Call the configuration API. */
      Result = qapi_COEX_Configure(&ConfigData);

      /* Print function success/failure. */
      if(Result == QAPI_OK)
      {
         Display_Function_Success(Coex_Demo_Context.QCLI_Handle, "qapi_COEX_Configure");
         RetVal = QCLI_STATUS_SUCCESS_E;
      }
      else
      {
         Display_Function_Error(Coex_Demo_Context.QCLI_Handle, "qapi_COEX_Configure", Result);
         RetVal = QCLI_STATUS_ERROR_E;
      }
   }
   else
   {
      RetVal = QCLI_STATUS_USAGE_E;
   }

   return RetVal;
}

/**
   @brief Executes the "Configure" command to configure the coexistence module.

   Parameter_List[0] Configuration flags.
   Parameter_List[1] Priority 1 value - 1 = BLE, 2 = I15P4, 3 = EXT, 4 = WLAN.
   Parameter_List[2] Priority 2 value - 1 = BLE, 2 = I15P4, 3 = EXT, 4 = WLAN.
   Parameter_List[3] Priority 3 value - 1 = BLE, 2 = I15P4, 3 = EXT, 4 = WLAN.
   Parameter_List[4] Priority 4 value - 1 = BLE, 2 = I15P4, 3 = EXT, 4 = WLAN.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_ConfigureAdvanced(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t                    Result;
   QCLI_Command_Status_t            RetVal;
   qapi_COEX_Advanced_Config_Data_t ConfigData;

   /* Verify the input parameters. */
   if(Parameter_Count >= 2)
   {
      /* Set the configuration values. */
      memset(&ConfigData, 0, sizeof(qapi_COEX_Advanced_Config_Data_t));

      ConfigData.priority_Config_Data   = Coex_Demo_Context.Priorities;
      ConfigData.priority_Config_Length = CONFIG_PRIORITY_LENGTH;
      ConfigData.config_Flags           = Parameter_List[0].Integer_Value;
      ConfigData.concurrency_Flags      = Parameter_List[1].Integer_Value;

      /* Call the configuration API. */
      Result = qapi_COEX_Configure_Advanced(&ConfigData);

      /* Print function success/failure. */
      if(Result == QAPI_OK)
      {
         Display_Function_Success(Coex_Demo_Context.QCLI_Handle, "qapi_COEX_Configure_Advanced");
         RetVal = QCLI_STATUS_SUCCESS_E;
      }
      else
      {
         Display_Function_Error(Coex_Demo_Context.QCLI_Handle, "qapi_COEX_Configure_Advanced", Result);
         RetVal = QCLI_STATUS_ERROR_E;
      }
   }
   else
   {
      RetVal = QCLI_STATUS_USAGE_E;
   }

   return RetVal;
}

/**
   @brief Sets the BLE priority values.

   Parameter_List[0] Advertise value.
   Parameter_List[1] Scan value.
   Parameter_List[2] Data request value.
   Parameter_List[3] Data active value.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_SetBLEPriority(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t   RetVal;

   /* Verify the input parameters. */
   if(Parameter_Count >= 4)
   {
      /* Set the values. */
      Coex_Demo_Context.Priorities[0].priority_Value = Parameter_List[0].Integer_Value;
      Coex_Demo_Context.Priorities[1].priority_Value = Parameter_List[1].Integer_Value;
      Coex_Demo_Context.Priorities[2].priority_Value = Parameter_List[2].Integer_Value;
      Coex_Demo_Context.Priorities[3].priority_Value = Parameter_List[3].Integer_Value;

      RetVal = QCLI_STATUS_SUCCESS_E;
   }
   else
   {
      RetVal = QCLI_STATUS_USAGE_E;
   }

   return RetVal;
}

/**
   @brief Sets the I15P4 priority values.

   Parameter_List[0] Rx request value.
   Parameter_List[1] Tx request value.
   Parameter_List[2] Rx active value.
   Parameter_List[3] Tx active value.
   Parameter_List[4] ACK value.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_SetI15P4Priority(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t   RetVal;

   /* Verify the input parameters. */
   if(Parameter_Count >= 7)
   {
      /* Set the values. */
      Coex_Demo_Context.Priorities[ 8].priority_Value = Parameter_List[0].Integer_Value;
      Coex_Demo_Context.Priorities[ 9].priority_Value = Parameter_List[1].Integer_Value;
      Coex_Demo_Context.Priorities[10].priority_Value = Parameter_List[2].Integer_Value;
      Coex_Demo_Context.Priorities[11].priority_Value = Parameter_List[3].Integer_Value;
      Coex_Demo_Context.Priorities[12].priority_Value = Parameter_List[4].Integer_Value;
      Coex_Demo_Context.Priorities[13].priority_Value = Parameter_List[5].Integer_Value;
      Coex_Demo_Context.Priorities[14].priority_Value = Parameter_List[6].Integer_Value;

      RetVal = QCLI_STATUS_SUCCESS_E;
   }
   else
   {
      RetVal = QCLI_STATUS_USAGE_E;
   }

   return RetVal;
}

/**
   @brief Sets the EXT priority values.

   Parameter_List[0] Low request value.
   Parameter_List[1] High request value.
   Parameter_List[2] Low active value.
   Parameter_List[3] High active value.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_SetEXTPriority(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t   RetVal;

   /* Verify the input parameters. */
   if(Parameter_Count >= 4)
   {
      /* Set the values. */
      Coex_Demo_Context.Priorities[4].priority_Value = Parameter_List[0].Integer_Value;
      Coex_Demo_Context.Priorities[5].priority_Value = Parameter_List[1].Integer_Value;
      Coex_Demo_Context.Priorities[6].priority_Value = Parameter_List[2].Integer_Value;
      Coex_Demo_Context.Priorities[7].priority_Value = Parameter_List[3].Integer_Value;

      RetVal = QCLI_STATUS_SUCCESS_E;
   }
   else
   {
      RetVal = QCLI_STATUS_USAGE_E;
   }

   return RetVal;
}

/**
   @brief Executes the "StatisticsEnable" command to enable coexistence statistics.

   Parameter_List[0] Enable/disable statistics.

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_StatisticsEnable(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t           Result;
   QCLI_Command_Status_t   RetVal;
   boolean                 Enable;

   /* Verify the input parameters. */
   if((Parameter_Count >= 1) && (Parameter_List[0].Integer_Value <= 1))
   {
      if(Parameter_List[0].Integer_Value == 0)
         Enable = FALSE;
      else
         Enable = TRUE;

      /* Call the enable statistics API. */
      Result = qapi_COEX_Statistics_Enable(Enable, COEX_STATISTICS_MASK, 0);

      /* Print function success/failure. */
      if(Result == QAPI_OK)
      {
         if(Enable)
         {
            Display_Function_Success(Coex_Demo_Context.QCLI_Handle, "qapi_COEX_Statistics_Enable (Enabled)");
         }
         else
         {
            Display_Function_Success(Coex_Demo_Context.QCLI_Handle, "qapi_COEX_Statistics_Enable (Disabled)");
         }

         RetVal = QCLI_STATUS_SUCCESS_E;
      }
      else
      {
         Display_Function_Error(Coex_Demo_Context.QCLI_Handle, "qapi_COEX_Statistics_Enable", Result);
         RetVal = QCLI_STATUS_ERROR_E;
      }
   }
   else
   {
      RetVal = QCLI_STATUS_USAGE_E;
   }

   return RetVal;
}

/**
   @brief Executes the "StatisticsGet" command to get coexistence statistics.

   Parameter_List[0] Length of the statistics buffer to use.
   Parameter_List[1] Statistics mask.
   Parameter_List[2] Reset statistics (0 = FALSE, 1 = TRUE).

   @param Parameter_Count is number of elements in Parameter_List.
   @param Parameter_List is list of parsed arguments associate with this
          command.

   @return
    - QCLI_STATUS_SUCCESS_E indicates the command is executed successfully.
    - QCLI_STATUS_ERROR_E indicates the command is failed to execute.
    - QCLI_STATUS_USAGE_E indicates there is usage error associated with this
      command.
*/
static QCLI_Command_Status_t cmd_StatisticsGet(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   qapi_Status_t                Result;
   QCLI_Command_Status_t        RetVal;
   boolean                      Reset;
   qapi_COEX_Statistics_Data_t *StatisticsData;
   uint8                        StatisticsDataLength;
   uint8                        Index;

   /* Verify the input parameters. */
   if((Parameter_Count >= 1) && (Parameter_List[0].Integer_Value <= 1))
   {
      /* Get the parameters. */
      if(Parameter_List[0].Integer_Value == 0)
         Reset = FALSE;
      else
         Reset = TRUE;

      StatisticsDataLength = COEX_STATISTICS_LENGTH;

      /* Allocate memory for the buffer. */
      StatisticsData = (qapi_COEX_Statistics_Data_t *)malloc(sizeof(qapi_COEX_Statistics_Data_t) * StatisticsDataLength);

      /* Make sure the memory was allocated. */
      if(StatisticsData)
      {
         /* Call the get statistics API. */
         Result = qapi_COEX_Statistics_Get(StatisticsData, &StatisticsDataLength, COEX_STATISTICS_MASK, Reset);

         /* Print function success/failure. */
         if(Result == QAPI_OK)
         {
            Display_Function_Success(Coex_Demo_Context.QCLI_Handle, "qapi_COEX_Statistics_Get");
            RetVal = QCLI_STATUS_SUCCESS_E;

            /* Loop through each item and print out the data. */
            for(Index = 0; Index < StatisticsDataLength; Index++)
            {
               switch(StatisticsData[Index].packet_Status_Type)
               {
                  QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "\r\n");

                  case QAPI_COEX_PACKET_STATUS_TYPE_BLE_SCAN_COMPLETE:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_BLE_SCAN_COMPLETE count:          %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
                  case QAPI_COEX_PACKET_STATUS_TYPE_BLE_SCAN_STOMP:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_BLE_SCAN_STOMP count:             %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
                  case QAPI_COEX_PACKET_STATUS_TYPE_BLE_SCAN_CONNECT_COMPLETE:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_BLE_SCAN_CONNECT_COMPLETE count:  %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
                  case QAPI_COEX_PACKET_STATUS_TYPE_BLE_SCAN_CONNECT_STOMP:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_BLE_SCAN_CONNECT_STOMP count:     %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
                  case QAPI_COEX_PACKET_STATUS_TYPE_BLE_ADVERTISE_COMPLETE:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_BLE_ADVERTISE_COMPLETE count:     %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
                  case QAPI_COEX_PACKET_STATUS_TYPE_BLE_ADVERTISE_STOMP:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_BLE_ADVERTISE_STOMP count:        %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
                  case QAPI_COEX_PACKET_STATUS_TYPE_BLE_DATA_RX_COMPLETE:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_BLE_DATA_RX_COMPLETE count:       %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
                  case QAPI_COEX_PACKET_STATUS_TYPE_BLE_DATA_RX_STOMP:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_BLE_DATA_RX_STOMP count:          %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
                  case QAPI_COEX_PACKET_STATUS_TYPE_BLE_DATA_TX_COMPLETE:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_BLE_DATA_TX_COMPLETE count:       %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
                  case QAPI_COEX_PACKET_STATUS_TYPE_BLE_DATA_TX_STOMP:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_BLE_DATA_TX_STOMP count:          %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
                  case QAPI_COEX_PACKET_STATUS_TYPE_BLE_ISOC_RX_COMPLETE:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_BLE_ISOC_RX_COMPLETE count:       %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
                  case QAPI_COEX_PACKET_STATUS_TYPE_BLE_ISOC_RX_STOMP:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_BLE_ISOC_RX_STOMP count:          %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
                  case QAPI_COEX_PACKET_STATUS_TYPE_BLE_ISOC_TX_COMPLETE:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_BLE_ISOC_TX_COMPLETE count:       %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
                  case QAPI_COEX_PACKET_STATUS_TYPE_BLE_ISOC_TX_STOMP:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_BLE_ISOC_TX_STOMP count:          %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
                  case QAPI_COEX_PACKET_STATUS_TYPE_I15P4_ED_SCAN_COMPLETE:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_I15P4_ED_SCAN_COMPLETE count:     %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
                  case QAPI_COEX_PACKET_STATUS_TYPE_I15P4_ED_SCAN_STOMP:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_I15P4_ED_SCAN_STOMP count:        %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
                  case QAPI_COEX_PACKET_STATUS_TYPE_I15P4_BEACON_SCAN_COMPLETE:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_I15P4_BEACON_SCAN_COMPLETE count: %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
                  case QAPI_COEX_PACKET_STATUS_TYPE_I15P4_BEACON_SCAN_STOMP:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_I15P4_BEACON_SCAN_STOMP count:    %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
                  case QAPI_COEX_PACKET_STATUS_TYPE_I15P4_DATA_RX_COMPLETE:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_I15P4_DATA_RX_COMPLETE count:     %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
                  case QAPI_COEX_PACKET_STATUS_TYPE_I15P4_DATA_RX_STOMP:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_I15P4_DATA_RX_STOMP count:        %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
                  case QAPI_COEX_PACKET_STATUS_TYPE_I15P4_DATA_TX_COMPLETE:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_I15P4_DATA_TX_COMPLETE count:     %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
                  case QAPI_COEX_PACKET_STATUS_TYPE_I15P4_DATA_TX_STOMP:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_I15P4_DATA_TX_STOMP count:        %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
                  case QAPI_COEX_PACKET_STATUS_TYPE_I15P4_ACK_RX_COMPLETE:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_I15P4_ACK_RX_COMPLETE count:      %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
                  case QAPI_COEX_PACKET_STATUS_TYPE_I15P4_ACK_RX_STOMP:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_I15P4_ACK_RX_STOMP count:         %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
                  case QAPI_COEX_PACKET_STATUS_TYPE_I15P4_ACK_TX_COMPLETE:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_I15P4_ACK_TX_COMPLETE count:      %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
                  case QAPI_COEX_PACKET_STATUS_TYPE_I15P4_ACK_TX_STOMP:
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "QAPI_COEX_PACKET_STATUS_TYPE_I15P4_ACK_TX_STOMP count:         %d.\r\n", StatisticsData[Index].packet_Status_Count);
                     break;
               }
            }
         }
         else
         {
            Display_Function_Error(Coex_Demo_Context.QCLI_Handle, "qapi_COEX_Statistics_Get", Result);
            RetVal = QCLI_STATUS_ERROR_E;
         }

         /* Free the data buffer. */
         free(StatisticsData);
      }
      else
      {
         QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "Memory allocation failed.\n");
         RetVal = QCLI_STATUS_ERROR_E;
      }
   }
   else
   {
      RetVal = QCLI_STATUS_USAGE_E;
   }

   return RetVal;
}

static QCLI_Command_Status_t cmd_EPTAGPIOEnable(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   QCLI_Command_Status_t RetVal;
   uint32_t              Mode;
   boolean               StdConfig;

   /* Verify the input parameters. */
   if((Parameter_Count >= 1) && (Parameter_List[0].Integer_Value >= 0) && (Parameter_List[0].Integer_Value <= 2))
   {
      /* Get the parameters. */
      Mode = Parameter_List[0].Integer_Value;

      /* Determine if using the standard or alternate GPIO. */
      if((Parameter_Count == 1) || ((Parameter_Count >= 2) && (Parameter_List[1].Integer_Value == 0)))
      {
         StdConfig = TRUE;
      }
      else
      {
         StdConfig = FALSE;
      }

      /* Get the standard GPIO IDs if not already retrieved. */
      if((StdConfig) && (!EPTA_Std_GPIO_Initialized))
      {
         EPTA_Pin_1_Std_Config.pin  = EPTA_PIN_NUMBER_STD_CONFIG_1;
         EPTA_Pin_2_Std_Config.pin  = EPTA_PIN_NUMBER_STD_CONFIG_2;
         EPTA_Pin_3_Std_Config.pin  = EPTA_PIN_NUMBER_STD_CONFIG_3;

         if((qapi_TLMM_Get_Gpio_ID(&EPTA_Pin_1_Std_Config, &EPTA_Pin_1_Std_ID) == QAPI_OK) &&
            (qapi_TLMM_Get_Gpio_ID(&EPTA_Pin_2_Std_Config, &EPTA_Pin_2_Std_ID) == QAPI_OK) &&
            (qapi_TLMM_Get_Gpio_ID(&EPTA_Pin_3_Std_Config, &EPTA_Pin_3_Std_ID) == QAPI_OK))
         {
            /* Flag that the standard EPTA GPIO IDs have been retrieved. */
            EPTA_Std_GPIO_Initialized = true;
         }
      }

      /* Get the standardalternate GPIO IDs if not already retrieved. */
      if((!StdConfig) && (!EPTA_Alt_GPIO_Initialized))
      {
         EPTA_Pin_1_Alt_Config.pin  = EPTA_PIN_NUMBER_ALT_CONFIG_1;
         EPTA_Pin_2_Alt_Config.pin  = EPTA_PIN_NUMBER_ALT_CONFIG_2;
         EPTA_Pin_3_Alt_Config.pin  = EPTA_PIN_NUMBER_ALT_CONFIG_3;

         if((qapi_TLMM_Get_Gpio_ID(&EPTA_Pin_1_Alt_Config, &EPTA_Pin_1_Alt_ID) == QAPI_OK) &&
            (qapi_TLMM_Get_Gpio_ID(&EPTA_Pin_2_Alt_Config, &EPTA_Pin_2_Alt_ID) == QAPI_OK) &&
            (qapi_TLMM_Get_Gpio_ID(&EPTA_Pin_3_Alt_Config, &EPTA_Pin_3_Alt_ID) == QAPI_OK))
         {
            /* Flag that the standard EPTA GPIO IDs have been retrieved. */
            EPTA_Alt_GPIO_Initialized = true;
         }
      }

      /* Continue if the GPIO IDs were retrieved. */
      if(((StdConfig) && (EPTA_Std_GPIO_Initialized)) || ((!StdConfig) && (EPTA_Alt_GPIO_Initialized)))
      {
         /* Handle the disabled mode. */
         if(Mode == 0)
         {
            /* Set the GPIO parameters. */
            EPTA_Pin_1_Std_Config.func = 0;
            EPTA_Pin_2_Std_Config.func = 0;
            EPTA_Pin_3_Std_Config.func = 0;
            EPTA_Pin_1_Alt_Config.func = 0;
            EPTA_Pin_2_Alt_Config.func = 0;
            EPTA_Pin_3_Alt_Config.func = 0;
            EPTA_Pin_1_Std_Config.dir  = QAPI_GPIO_INPUT_E;
            EPTA_Pin_2_Std_Config.dir  = QAPI_GPIO_INPUT_E;
            EPTA_Pin_3_Std_Config.dir  = QAPI_GPIO_INPUT_E;
            EPTA_Pin_1_Alt_Config.dir  = QAPI_GPIO_INPUT_E;
            EPTA_Pin_2_Alt_Config.dir  = QAPI_GPIO_INPUT_E;
            EPTA_Pin_3_Alt_Config.dir  = QAPI_GPIO_INPUT_E;
         }
         else
         {
            /* Set the functions. */
            EPTA_Pin_1_Std_Config.func = EPTA_PIN_FUNCTION_STD_CONFIG_1;
            EPTA_Pin_2_Std_Config.func = EPTA_PIN_FUNCTION_STD_CONFIG_2;
            EPTA_Pin_3_Std_Config.func = EPTA_PIN_FUNCTION_STD_CONFIG_3;
            EPTA_Pin_1_Alt_Config.func = EPTA_PIN_FUNCTION_ALT_CONFIG_1;
            EPTA_Pin_2_Alt_Config.func = EPTA_PIN_FUNCTION_ALT_CONFIG_2;
            EPTA_Pin_3_Alt_Config.func = EPTA_PIN_FUNCTION_ALT_CONFIG_3;

            /* Handle the slave mode. */
            if(Mode == 1)
            {
               /* Set the GPIO parameters. */
               EPTA_Pin_1_Std_Config.dir = QAPI_GPIO_OUTPUT_E; // BT_ACTIVE
               EPTA_Pin_2_Std_Config.dir = QAPI_GPIO_INPUT_E;  // WLAN_ACTIVE
               EPTA_Pin_3_Std_Config.dir = QAPI_GPIO_OUTPUT_E; // BT_PRIORITY
               EPTA_Pin_1_Alt_Config.dir = QAPI_GPIO_OUTPUT_E; // BT_ACTIVE
               EPTA_Pin_2_Alt_Config.dir = QAPI_GPIO_INPUT_E;  // WLAN_ACTIVE
               EPTA_Pin_3_Alt_Config.dir = QAPI_GPIO_OUTPUT_E; // BT_PRIORITY
            }
            /* Handle the master mode. */
            else
            {
               /* Set the GPIO parameters. */
               EPTA_Pin_1_Std_Config.dir = QAPI_GPIO_OUTPUT_E; // WLAN_ACTIVE
               EPTA_Pin_2_Std_Config.dir = QAPI_GPIO_INPUT_E;  // BT_ACTIVE
               EPTA_Pin_3_Std_Config.dir = QAPI_GPIO_INPUT_E;  // BT_PRIORITY
               EPTA_Pin_1_Alt_Config.dir = QAPI_GPIO_OUTPUT_E; // WLAN_ACTIVE
               EPTA_Pin_2_Alt_Config.dir = QAPI_GPIO_INPUT_E;  // BT_ACTIVE
               EPTA_Pin_3_Alt_Config.dir = QAPI_GPIO_INPUT_E;  // BT_PRIORITY
            }
         }

         /* Set the configuration. */
         if (StdConfig)
         {
            if((qapi_TLMM_Config_Gpio(EPTA_Pin_1_Std_ID, &EPTA_Pin_1_Std_Config) == QAPI_OK) && \
               (qapi_TLMM_Config_Gpio(EPTA_Pin_2_Std_ID, &EPTA_Pin_2_Std_Config) == QAPI_OK) && \
               (qapi_TLMM_Config_Gpio(EPTA_Pin_3_Std_ID, &EPTA_Pin_3_Std_Config) == QAPI_OK))
            {
               RetVal = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               RetVal = QCLI_STATUS_ERROR_E;
               QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "EPTA GPIO Failure.\r\n");
            }
         }
         else
         {
            if((qapi_TLMM_Config_Gpio(EPTA_Pin_1_Alt_ID, &EPTA_Pin_1_Alt_Config) == QAPI_OK) && \
               (qapi_TLMM_Config_Gpio(EPTA_Pin_2_Alt_ID, &EPTA_Pin_2_Alt_Config) == QAPI_OK) && \
               (qapi_TLMM_Config_Gpio(EPTA_Pin_3_Alt_ID, &EPTA_Pin_3_Alt_Config) == QAPI_OK))
            {
               RetVal = QCLI_STATUS_SUCCESS_E;
            }
            else
            {
               RetVal = QCLI_STATUS_ERROR_E;
               QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "EPTA GPIO Failure.\r\n");
            }
         }

         if (RetVal == QCLI_STATUS_SUCCESS_E)
         {
            /* Print the status. */
            if(Mode == 0)
            {
               if (StdConfig)
                  QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "EPTA GPIO Disabled (Standard GPIO).\r\n");
               else
                  QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "EPTA GPIO Disabled (Alternate GPIO).\r\n");
            }
            else
            {
               if(Mode == 1)
               {
                  if (StdConfig)
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "EPTA GPIO Enabled (Slave, Standard GPIO).\r\n");
                  else
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "EPTA GPIO Enabled (Slave, Alternate GPIO).\r\n");
               }
               else
               {
                  if (StdConfig)
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "EPTA GPIO Enabled (Master, Standard GPIO).\r\n");
                  else
                     QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "EPTA GPIO Enabled (Master, Alternate GPIO).\r\n");
               }
            }
         }
         else
         {
            RetVal = QCLI_STATUS_ERROR_E;
            QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "EPTA GPIO Failure, could not set configuration.\r\n");
         }
      }
      else
      {
         RetVal = QCLI_STATUS_ERROR_E;
         QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "EPTA GPIO Failure, GPIO already in use.\r\n");
      }
   }
   else
   {
      RetVal = QCLI_STATUS_USAGE_E;
   }

   return RetVal;
}

static void
print_WlanCoexArgCntMsg(uint32_t Parameter_Count, uint32_t Expected_Params, const char *Coex_Desc)
{
    QCLI_Printf(Coex_Demo_Context.QCLI_Handle,
                "ERROR: WLAN %s coexistence command takes %d args, not %d\r\n",
                Coex_Desc, Expected_Params, Parameter_Count);
}

static void
print_WlanCoexFailMsg(const char *Coex_Desc)
{
    QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "Error: %s WLAN coexistence command failed.\r\n", Coex_Desc);
}

static QCLI_Command_Status_t
WlanValidProfile(QCLI_Parameter_t *Parameter_List, uint32_t index)
{
    if (!Parameter_List[index].Integer_Is_Valid || (Parameter_List[index].Integer_Value < 1 || Parameter_List[index].Integer_Value > 15)) {
        QCLI_Printf(Coex_Demo_Context.QCLI_Handle,
                    "Specify a valid profile value:\n"
                    "\t1  - Bluetooth SCO profile \n"
                    "\t2  - Bluetooth A2DP profile\n"
                    "\t3  - Bluetooth Inquiry Page profile\n"
                    "\t4  - Bluetooth ESCO profile\n"
                    "\t5  - Bluetooth HID profile\n"
                    "\t6  - Bluetooth PAN profile\n"
                    "\t7  - Bluetooth RFCOMM profile\n");
        QCLI_Printf(Coex_Demo_Context.QCLI_Handle,
                    "\t8  - Bluetooth LE profile\n"
                    "\t9  - Bluetooth SDP profile\n"
                    "\t10 - Bluetooth PAGESCAN profile\n"
                    "\t11 - Always yield to non-WLAN traffic\n"
                    "\t12 - 802.15.4\n"
                    "\t13 - Mesh\n"
                    "\t14 - Reactive mode\n"
                    "\t15 - BLE protect\n\n");
        return QCLI_STATUS_ERROR_E;
    }

    return QCLI_STATUS_SUCCESS_E;
}

#define APP_STRICMP(_a, _b) Memcmpi((_a), (_b), strlen(_a))

static QCLI_Command_Status_t
cmd_configureWlanCoex(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    qapi_WLAN_Coex_Config_Data_t WLAN_CoexConfig;
    const char *Coex_Desc = "configureWlanCoex";

    if(!Parameter_List) {
      return QCLI_STATUS_USAGE_E;
    }

    if (APP_STRICMP(Parameter_List[0].String_Value, "enable") == 0)
    {
        // Check to see if other parameters are there
        if (Parameter_Count < 5)
        {
            print_WlanCoexArgCntMsg(Parameter_Count, 5, Coex_Desc);
            return QCLI_STATUS_ERROR_E;
        }

        WLAN_CoexConfig.enable_Disable_Coex  = QAPI_WLAN_COEX_ENABLED_E;
        if ((APP_STRICMP(Parameter_List[1].String_Value, "3w") == 0))
        {
            WLAN_CoexConfig.coex_Mode = QAPI_WLAN_COEX_3_WIRE_MODE_E;
        }
        else if ((APP_STRICMP(Parameter_List[1].String_Value, "pta") == 0))
        {
            WLAN_CoexConfig.coex_Mode = QAPI_WLAN_COEX_PTA_MODE_E;
        }
        else if ((APP_STRICMP(Parameter_List[1].String_Value, "epta") == 0))
        {
            WLAN_CoexConfig.coex_Mode = QAPI_WLAN_COEX_EPTA_MODE_E;
        }
        else
        {
            QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "Error: Incorrect parameter value for coex mode. Provide '3w', 'pta' or 'epta'\r\n");
            return QCLI_STATUS_ERROR_E;
        }

        /* 8-bit unsigned profile-specfic param */
        if (!Parameter_List[2].Integer_Is_Valid || Parameter_List[2].Integer_Value < 0 || Parameter_List[2].Integer_Value > 255) {
            QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "Specify a valid profile-specific param 3 in the range of 0-255\n\n");
            return QCLI_STATUS_ERROR_E;
        }
        WLAN_CoexConfig.coex_Profile_Specific_Param1 = Parameter_List[2].Integer_Value;

        if (WlanValidProfile(Parameter_List, 3) != QCLI_STATUS_SUCCESS_E) {
            return QCLI_STATUS_ERROR_E;
        }
        WLAN_CoexConfig.coex_Profile = Parameter_List[3].Integer_Value;

        /* 8-bit unsigned profile-specfic param */
        if (!Parameter_List[4].Integer_Is_Valid || Parameter_List[4].Integer_Value < 0 || Parameter_List[4].Integer_Value > 255) {
            QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "Specify a valid profile-specific param 5 in the range of 0-255\n\n");
            return QCLI_STATUS_ERROR_E;
        }
        WLAN_CoexConfig.coex_Profile_Specific_Param2 = Parameter_List[4].Integer_Value;
    }
    else if (APP_STRICMP(Parameter_List[0].String_Value, "disable") == 0)
    {
        WLAN_CoexConfig.enable_Disable_Coex  = QAPI_WLAN_COEX_DISABLE_E;
        if (Parameter_Count < 2)
        {
            print_WlanCoexArgCntMsg(Parameter_Count, 2, Coex_Desc);
            return QCLI_STATUS_ERROR_E;
        }

        if (WlanValidProfile(Parameter_List, 1) != QCLI_STATUS_SUCCESS_E) {
            return QCLI_STATUS_ERROR_E;
        }
        WLAN_CoexConfig.coex_Profile = Parameter_List[1].Integer_Value;
    }
    else
    {
        QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "Error: Provide 'enable' or 'disable' as the first parameter\r\n");
        return QCLI_STATUS_ERROR_E;
    }

    if (0 != qapi_WLAN_Coex_Control(&WLAN_CoexConfig))
    {
        print_WlanCoexFailMsg(Coex_Desc);
        return QCLI_STATUS_ERROR_E;
    }

    return QCLI_STATUS_SUCCESS_E;
}

static boolean
valid_WlanCoexParams(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, uint32_t Expected_Params, const char *Coex_Desc)
{
    uint32_t index;

    if (Parameter_Count < Expected_Params) {
        print_WlanCoexArgCntMsg(Parameter_Count, Expected_Params, Coex_Desc);
        return FALSE;
    }

    for (index = 0; index < Expected_Params; ++index) {
        if (!Parameter_List[index].Integer_Is_Valid)
        {
            QCLI_Printf(Coex_Demo_Context.QCLI_Handle,
                        "Error: Incorrect parameter %d format for %s coex config. Provide an integer value.\r\n",
                        index, Coex_Desc);
            return FALSE;
        }
    }

    return TRUE;
}

static QCLI_Command_Status_t
cmd_GetWlanCoexStats(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    qapi_WLAN_Coex_Stats_t *WLAN_CoexStats;
    QCLI_Command_Status_t status;
    const char *Coex_Desc = "GetWlanCoexStats";

    if (!valid_WlanCoexParams(Parameter_Count, Parameter_List, 1, Coex_Desc)) {
        return QCLI_STATUS_ERROR_E;
    }

    /* Allocate memory for the buffer. */
    WLAN_CoexStats = (qapi_WLAN_Coex_Stats_t *)malloc(sizeof(qapi_WLAN_Coex_Stats_t));
    if (!WLAN_CoexStats)
        return QCLI_STATUS_ERROR_E;

    memset(WLAN_CoexStats, 0, sizeof(qapi_WLAN_Coex_Stats_t));
    WLAN_CoexStats->resetStats = Parameter_List[0].Integer_Value;

    if (0 != qapi_Get_WLAN_Coex_Stats(WLAN_CoexStats))
    {
        print_WlanCoexFailMsg(Coex_Desc);
        status = QCLI_STATUS_ERROR_E;
    } else {
        status = QCLI_STATUS_SUCCESS_E;
        QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "---\nGENERAL COEX STATS\nhighRatePktCnt = \t\t%d\n"
                    "firstBmissCnt = \t\t%d\n"
                    "BmissCnt = \t\t%d\n"
                    "psPollFailureCnt = \t%d\n"
                    "nullFrameFailureCnt = \t%d\n"
                    "stompCnt = \t\t%d\n---\n",
                    WLAN_CoexStats->coex_Stats_Data.generalStats.highRatePktCnt,
                    WLAN_CoexStats->coex_Stats_Data.generalStats.firstBmissCnt,
                    WLAN_CoexStats->coex_Stats_Data.generalStats.BmissCnt,
                    WLAN_CoexStats->coex_Stats_Data.generalStats.psPollFailureCnt,
                    WLAN_CoexStats->coex_Stats_Data.generalStats.nullFrameFailureCnt,
                    WLAN_CoexStats->coex_Stats_Data.generalStats.stompCnt);

        switch(WLAN_CoexStats->coex_Stats_Data.profileType) {
        case QAPI_BT_PROFILE_SCO:
            QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "SCO STATS\nscoStompCntAvg = \t\t%d\n"
                        "scoStompIn100ms = \t\t%d\n"
                        "scoMaxContStomp = \t\t%d\n"
                        "scoAvgNoRetries = \t\t%d\n"
                        "scoMaxNoRetriesIn100ms = \t%d\n",
                        WLAN_CoexStats->coex_Stats_Data.statsU.scoStats.scoStompCntAvg,
                        WLAN_CoexStats->coex_Stats_Data.statsU.scoStats.scoStompIn100ms,
                        WLAN_CoexStats->coex_Stats_Data.statsU.scoStats.scoMaxContStomp,
                        WLAN_CoexStats->coex_Stats_Data.statsU.scoStats.scoAvgNoRetries,
                        WLAN_CoexStats->coex_Stats_Data.statsU.scoStats.scoMaxNoRetriesIn100ms);
            break;
        case QAPI_BT_PROFILE_A2DP:
             QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "A2DP STATS\na2dpBurstCnt = \t\t\t%d\n"
                        "a2dpMaxBurstCnt = \t\t%d\n"
                        "a2dpAvgIdletimeIn100ms = \t%d\n"
                        "a2dpAvgStompCnt = \t\t%d\n",
                        WLAN_CoexStats->coex_Stats_Data.statsU.a2dpStats.a2dpBurstCnt,
                        WLAN_CoexStats->coex_Stats_Data.statsU.a2dpStats.a2dpMaxBurstCnt,
                        WLAN_CoexStats->coex_Stats_Data.statsU.a2dpStats.a2dpAvgIdletimeIn100ms,
                        WLAN_CoexStats->coex_Stats_Data.statsU.a2dpStats.a2dpAvgStompCnt);
            break;
        case QAPI_BT_PROFILE_ESCO:
            QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "ESCO STATS\naclPktCntInBtTime = \t\t%d\n"
                        "aclStompCntInWlanTime = \t%d\n"
                        "aclPktCntIn100ms = \t\t%d\n",
                        WLAN_CoexStats->coex_Stats_Data.statsU.aclCoexStats.aclPktCntInBtTime,
                        WLAN_CoexStats->coex_Stats_Data.statsU.aclCoexStats.aclStompCntInWlanTime,
                        WLAN_CoexStats->coex_Stats_Data.statsU.aclCoexStats.aclPktCntIn100ms);
            break;
        default:
            break;
        }
    }

    free(WLAN_CoexStats);
    return status;
}

/**
   @brief Registers coex commands with QCLI and initializes the
          sample application.
*/
void Initialize_Coex_Demo(void)
{
   memset(&Coex_Demo_Context, 0, sizeof(Coex_Demo_Context_t));

   /* Set the priority types. */
   Coex_Demo_Context.Priorities[ 0].priority_Type  = QAPI_COEX_PRIORITY_BLE_ADVERTISE_E;
   Coex_Demo_Context.Priorities[ 0].priority_Value = 20;
   Coex_Demo_Context.Priorities[ 1].priority_Type  = QAPI_COEX_PRIORITY_BLE_SCAN_E;
   Coex_Demo_Context.Priorities[ 1].priority_Value = 19;
   Coex_Demo_Context.Priorities[ 2].priority_Type  = QAPI_COEX_PRIORITY_BLE_DATA_REQUEST_E;
   Coex_Demo_Context.Priorities[ 2].priority_Value = 21;
   Coex_Demo_Context.Priorities[ 3].priority_Type  = QAPI_COEX_PRIORITY_BLE_DATA_ACTIVE_E;
   Coex_Demo_Context.Priorities[ 3].priority_Value = 45;
   Coex_Demo_Context.Priorities[ 4].priority_Type  = QAPI_COEX_PRIORITY_EXT_LOW_REQUEST_E;
   Coex_Demo_Context.Priorities[ 4].priority_Value = 10;
   Coex_Demo_Context.Priorities[ 5].priority_Type  = QAPI_COEX_PRIORITY_EXT_HIGH_REQUEST_E;
   Coex_Demo_Context.Priorities[ 5].priority_Value = 18;
   Coex_Demo_Context.Priorities[ 6].priority_Type  = QAPI_COEX_PRIORITY_EXT_LOW_ACTIVE_E;
   Coex_Demo_Context.Priorities[ 6].priority_Value = 44;
   Coex_Demo_Context.Priorities[ 7].priority_Type  = QAPI_COEX_PRIORITY_EXT_HIGH_ACTIVE_E;
   Coex_Demo_Context.Priorities[ 7].priority_Value = 59;
   Coex_Demo_Context.Priorities[ 8].priority_Type  = QAPI_COEX_PRIORITY_I15P4_RX_REQUEST_E;
   Coex_Demo_Context.Priorities[ 8].priority_Value = 43;
   Coex_Demo_Context.Priorities[ 9].priority_Type  = QAPI_COEX_PRIORITY_I15P4_TX_REQUEST_E;
   Coex_Demo_Context.Priorities[ 9].priority_Value = 42;
   Coex_Demo_Context.Priorities[10].priority_Type  = QAPI_COEX_PRIORITY_I15P4_RX_ACTIVE_E;
   Coex_Demo_Context.Priorities[10].priority_Value = 58;
   Coex_Demo_Context.Priorities[11].priority_Type  = QAPI_COEX_PRIORITY_I15P4_TX_ACTIVE_E;
   Coex_Demo_Context.Priorities[11].priority_Value = 57;
   Coex_Demo_Context.Priorities[12].priority_Type  = QAPI_COEX_PRIORITY_I15P4_ACK_E;
   Coex_Demo_Context.Priorities[12].priority_Value = 60;
   Coex_Demo_Context.Priorities[13].priority_Type  = QAPI_COEX_PRIORITY_I15P4_ED_SCAN_E;
   Coex_Demo_Context.Priorities[13].priority_Value = 41;
   Coex_Demo_Context.Priorities[14].priority_Type  = QAPI_COEX_PRIORITY_I15P4_HOLD_REQUEST_E;
   Coex_Demo_Context.Priorities[14].priority_Value = 56;

   Coex_Demo_Context.QCLI_Handle = QCLI_Register_Command_Group(Coex_Demo_Context.QCLI_Handle, &Coex_CMD_Group);

   QCLI_Printf(Coex_Demo_Context.QCLI_Handle, "Coex Demo Initialized.\r\n");
}

/**
   @brief Cleans up the coex demo.
*/
void Cleanup_Coex_Demo(void)
{
}
