/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_cl_ias_ace.h
   @brief QAPI for the ZigBee IAS Ancillary Control Equipment (ACE) cluster.

   @addtogroup qapi_zb_cl_ias_ace
   @{

   These APIs provide definitions, commands, and events related to the ZigBee
   Intruder Alarm System (IAS) ACE cluster.

   APIs are provided to create IAS Ancillary Control Equipment (ACE) server
   and client clusters. Cluster creation also registers an event callback that
   is used to receive events from the IAS ACE cluster. Client events are for
   command responses and server events indicate state changes.

   @}
*/

#ifndef __QAPI_ZB_CL_IASACE_H__ // [
#define __QAPI_ZB_CL_IASACE_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_zb_cl.h"
#include "qapi_zb_cl_ias_zone.h"

/** @addtogroup qapi_zb_cl_ias_ace
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following definitions represent the command IDs for the IAS ACE client
   cluster. These IDs are provided predominately for identifying command
   responses. */
#define QAPI_ZB_CL_IASACE_CLIENT_COMMAND_ID_ARM                                 (0x00) /**< IAS ACE client cluster command ID for the Arm command. */
#define QAPI_ZB_CL_IASACE_CLIENT_COMMAND_ID_BYPASS                              (0x01) /**< IAS ACE client cluster command ID for the Bypass command. */
#define QAPI_ZB_CL_IASACE_CLIENT_COMMAND_ID_EMERGENCY                           (0x02) /**< IAS ACE client cluster command ID for the Emergency command. */
#define QAPI_ZB_CL_IASACE_CLIENT_COMMAND_ID_FIRE                                (0x03) /**< IAS ACE client cluster command ID for the Fire command. */
#define QAPI_ZB_CL_IASACE_CLIENT_COMMAND_ID_PANIC                               (0x04) /**< IAS ACE client cluster command ID for the Panic command. */
#define QAPI_ZB_CL_IASACE_CLIENT_COMMAND_ID_GET_ZONE_ID_MAP                     (0x05) /**< IAS ACE client cluster command ID for the Get Zone ID Map command. */
#define QAPI_ZB_CL_IASACE_CLIENT_COMMAND_ID_GET_ZONE_INFORMATION                (0x06) /**< IAS ACE client cluster command ID for the Get Zone Information command. */
#define QAPI_ZB_CL_IASACE_CLIENT_COMMAND_ID_GET_PANEL_STATUS                    (0x07) /**< IAS ACE client cluster command ID for the Get Panel Status command. */
#define QAPI_ZB_CL_IASACE_CLIENT_COMMAND_ID_GET_BYPASSED_ZONE_LIST              (0x08) /**< IAS ACE client cluster command ID for the Get Bypasses Zone List command. */
#define QAPI_ZB_CL_IASACE_CLIENT_COMMAND_ID_GET_ZONE_STATUS                     (0x09) /**< IAS ACE client cluster command ID for the Get Zone Status command. */

/* The following definitions represent the command IDs for the IAS ACE server
   cluster. These IDs are provided predominately for identifying command
   responses. */
#define QAPI_ZB_CL_IASACE_SERVER_COMMAND_ID_ARM_RESPONSE                        (0x00) /**< IAS ACE server cluster command ID for the Arm Response command. */
#define QAPI_ZB_CL_IASACE_SERVER_COMMAND_ID_GET_ZONE_ID_MAP_RESPONSE            (0x01) /**< IAS ACE server cluster command ID for the Get Zone ID Map Response command. */
#define QAPI_ZB_CL_IASACE_SERVER_COMMAND_ID_GET_ZONE_INFORMATION_RESPONSE       (0x02) /**< IAS ACE server cluster command ID for the Get Zone Information Response
                                                                                            command. */
#define QAPI_ZB_CL_IASACE_SERVER_COMMAND_ID_ZONE_STATUS_CHANGED                 (0x03) /**< IAS ACE server cluster command ID for the Zone Status Changed command. */
#define QAPI_ZB_CL_IASACE_SERVER_COMMAND_ID_PANEL_STATUS_CHANGED                (0x04) /**< IAS ACE server cluster command ID for the Panel Status Changed command. */
#define QAPI_ZB_CL_IASACE_SERVER_COMMAND_ID_GET_PANEL_STATUS_RESPONSE           (0x05) /**< IAS ACE server cluster command ID for the Get Panel Status Response command. */
#define QAPI_ZB_CL_IASACE_SERVER_COMMAND_ID_SET_BYPASSED_ZONE_LIST              (0x06) /**< IAS ACE server cluster command ID for the Set Bypassed Zone List command. */
#define QAPI_ZB_CL_IASACE_SERVER_COMMAND_ID_BYPASS_RESPONSE                     (0x07) /**< IAS ACE server cluster command ID for the Bypass Response command. */
#define QAPI_ZB_CL_IASACE_SERVER_COMMAND_ID_GET_ZONE_STATUS_RESPONSE            (0x08) /**< IAS ACE server cluster command ID for the Get Zone Status Response command. */

/* The following definitions represent the valid values for the zone status. */
#define QAPI_ZB_CL_IASACE_ZONE_STATUS_ALARM1                                    (0x0001) /**< IAS ACE Zone status bitmask value for alarm 1. */
#define QAPI_ZB_CL_IASACE_ZONE_STATUS_ALARM2                                    (0x0002) /**< IAS ACE Zone status bitmask value for alarm 2. */
#define QAPI_ZB_CL_IASACE_ZONE_STATUS_TAMPER                                    (0x0004) /**< IAS ACE Zone status bitmask value indicating a tamper condition. */
#define QAPI_ZB_CL_IASACE_ZONE_STATUS_BATTERY                                   (0x0008) /**< IAS ACE Zone status bitmask value indicating a low battery. */
#define QAPI_ZB_CL_IASACE_ZONE_STATUS_SUPERVISION_REPORTS                       (0x0010) /**< IAS ACE Zone status bitmask value indicating the zone periodically sends
                                                                                              status change notifications. */
#define QAPI_ZB_CL_IASACE_ZONE_STATUS_RESTORE_REPORTS                           (0x0020) /**< IAS ACE Zone status bitmask value indicating the zone will send a status
                                                                                              change when an alarm clears. */
#define QAPI_ZB_CL_IASACE_ZONE_STATUS_TROUBLE                                   (0x0040) /**< IAS ACE Zone status bitmask value indicating a trouble condition. */
#define QAPI_ZB_CL_IASACE_ZONE_STATUS_AC_MAINS                                  (0x0080) /**< IAS ACE Zone status bitmask value indicating an AC/mains fault. */
#define QAPI_ZB_CL_IASACE_ZONE_STATUS_TEST                                      (0x0100) /**< IAS ACE Zone status bitmask value indicating the zone is in Test mode. */
#define QAPI_ZB_CL_IASACE_ZONE_STATUS_BATTERY_DEFECT                            (0x0200) /**< IAS ACE Zone status bitmask value indicating a battery defect. */

/* The following definitions represent the valid values for audible
   notifications. */
#define QAPI_ZB_CL_IASACE_AUDIBLE_NOTIFICATION_MUTE                             (0x00) /**< IAS ACE audible notification value indicating audio is muted. */
#define QAPI_ZB_CL_IASACE_AUDIBLE_NOTIFICATION_DEFAULT_SOUND                    (0x01) /**< IAS ACE audible notification value for the default sound. */
#define QAPI_ZB_CL_IASACE_AUDIBLE_NOTIFICATION_MANUFACTURER_SPECIFIC_START      (0x80) /**< IAS ACE audible notification value for the start value of manufacturer-specific sounds. */
#define QAPI_ZB_CL_IASACE_AUDIBLE_NOTIFICATION_MANUFACTURER_SPECIFIC_END        (0xFF) /**< IAS ACE audible notification value for the end value of manufacturer-specific sounds. */

#define QAPI_ZB_CL_IASACE_MAX_ZONE_ID                                           (254) /**< Maximum zone ID value that can be used with IAS ACE. */
#define QAPI_ZB_CL_IASACE_ZONE_ID_MAP_RESPONSE_SIZE                             (16)  /**< Number of 16-bit entries in the Get Zone ID Map Response. */
#define QAPI_ZB_CL_IASACE_ZONE_LABEL_MAX_LENGTH                                 (32)  /**< Maximum length of the IAS ACE zone label. */
#define QAPI_ZB_CL_IASACE_ARM_CODE_MAX_LENGTH                                   (32)  /**< Maximum length of the IAS ACE arm/disarm code. */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Enumeration of events that can be received by an IAS ACE client cluster.
*/
typedef enum
{
   QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E,              /**< Indicates an Attribute Custom Read event. */
   QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E,             /**< Indicates an Attribute Custom Write event. */
   QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E,              /**< Indicates a Default Response event. */
   QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E,              /**< Indicates a Command Complete event. */
   QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E,             /**< Indicates an Unparsed Response event. */
   QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_ARM_RESPONSE_E,                  /**< Indicates an Arm Response event. */
   QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_GET_ZONE_ID_MAP_RESPONSE_E,      /**< Indicates a Get Zone ID Map Response event. */
   QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_GET_ZONE_INFORMATION_RESPONSE_E, /**< Indicates a Get Zone Information Response event. */
   QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_ZONE_STATUS_CHANGED_E,           /**< Indicates a Zone Status Changed event. */
   QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_PANEL_STATUS_CHANGED_E,          /**< Indicates a Panel Status Changed event. */
   QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_GET_PANEL_STATUS_RESPONSE_E,     /**< Indicates a Get Panel Status Response event. */
   QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_SET_BYPASSED_ZONE_LIST_E,        /**< Indicates a Set Bypassed Zone List event. */
   QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_BYPASS_RESPONSE_E,               /**< Indicates a Bypass Response event. */
   QAPI_ZB_CL_IASACE_CLIENT_EVENT_TYPE_GET_ZONE_STATUS_RESPONSE_E       /**< Indicates a Get Zone Status Response event. */
} qapi_ZB_CL_IASACE_Client_Event_Type_t;

/**
   Enumeration of events that can be received by a IAS ACE server cluster.
*/
typedef enum
{
   QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an Attribute Custom Read event. */
   QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an Attribute Custom Write event. */
   QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_UNPARSED_DATA_E,     /**< Indicates an Unparsed Data event. */
   QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_ARM_E,               /**< Indicate an ARM event. */
   QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_BYPASS_E,            /**< Indicate a Bypass event. */
   QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_EMERGENCY_E,         /**< Indicate an Emergency event. */
   QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_FIRE_E,              /**< Indicate a Fire event. */
   QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_PANIC_E,             /**< Indicate a Panic event. */
   QAPI_ZB_CL_IASACE_SERVER_EVENT_TYPE_DELAY_TIMEOUT_E      /**< Indicates an exit or entry delay has reached zero. */
} qapi_ZB_CL_IASACE_Server_Event_Type_t;

/**
   Enumeration of the values for the arm mode.
*/
typedef enum
{
   QAPI_ZB_CL_IASACE_ARM_MODE_DISARM_E               = 0x00, /**< Disarm. */
   QAPI_ZB_CL_IASACE_ARM_MODE_ARM_DAY_ZONES_ONLY_E   = 0x01, /**< Arm day/home zones only. */
   QAPI_ZB_CL_IASACE_ARM_MODE_ARM_NIGHT_ZONES_ONLY_E = 0x02, /**< Arm night/sleep zones only. */
   QAPI_ZB_CL_IASACE_ARM_MODE_ARM_ALL_ZONES_E        = 0x03  /**< Arm all zones. */
} qapi_ZB_CL_IASACE_Arm_Mode_t;

/**
   Enumeration of the arm notification values.
*/
typedef enum
{
   QAPI_ZB_CL_IASACE_ARM_NOTIFICATION_ALL_ZONES_DISARMED_E     = 0x00, /**< All zones disarmed. */
   QAPI_ZB_CL_IASACE_ARM_NOTIFICATION_ONLY_DAY_ZONES_ARMED_E   = 0x01, /**< Only day/home zones armed.  */
   QAPI_ZB_CL_IASACE_ARM_NOTIFICATION_ONLY_NIGHT_ZONES_ARMED_E = 0x02, /**< Only night/sleep zones armed. */
   QAPI_ZB_CL_IASACE_ARM_NOTIFICATION_ALL_ZONES_ARMED_E        = 0x03, /**< All zones armed. */
   QAPI_ZB_CL_IASACE_ARM_NOTIFICATION_INVALID_ARM_CODE_E       = 0x04, /**< Invalid arm/disarm code. */
   QAPI_ZB_CL_IASACE_ARM_NOTIFICATION_NOT_READY_TO_ARM_E       = 0x05, /**< Not ready to arm. */
   QAPI_ZB_CL_IASACE_ARM_NOTIFICATION_ALREADY_DISARMED_E       = 0x06  /**< Already disarmed. */
} qapi_ZB_CL_IASACE_Arm_Notification_t;

/**
   Enumeration of the panel status values.
*/
typedef enum
{
   QAPI_ZB_CL_IASACE_PANEL_STATUS_READY_TO_ARM_E     = 0x00, /**< Panel is disarmed and ready to arm. */
   QAPI_ZB_CL_IASACE_PANEL_STATUS_ARMED_STAY_E       = 0x01, /**< Armed stay. */
   QAPI_ZB_CL_IASACE_PANEL_STATUS_ARMED_NIGHT_E      = 0x02, /**< Armed night. */
   QAPI_ZB_CL_IASACE_PANEL_STATUS_ARMED_AWAY_E       = 0x03, /**< Armed away. */
   QAPI_ZB_CL_IASACE_PANEL_STATUS_EXIT_DELAY_E       = 0x04, /**< Exit delay. */
   QAPI_ZB_CL_IASACE_PANEL_STATUS_ENTRY_DELAY_E      = 0x05, /**< Entry delay. */
   QAPI_ZB_CL_IASACE_PANEL_STATUS_NOT_READY_TO_ARM_E = 0x06, /**< Not ready to arm. */
   QAPI_ZB_CL_IASACE_PANEL_STATUS_IN_ALARM_E         = 0x07, /**< In alarm. */
   QAPI_ZB_CL_IASACE_PANEL_STATUS_ARMING_STAY_E      = 0x08, /**< Arming stay. */
   QAPI_ZB_CL_IASACE_PANEL_STATUS_ARMING_NIGHT_E     = 0x09, /**< Arming night. */
   QAPI_ZB_CL_IASACE_PANEL_STATUS_ARMING_AWAY_E      = 0x0A  /**< Arming away. */
} qapi_ZB_CL_IASACE_Panel_Status_t;

/**
   Enumeration of the alarm status values.
*/
typedef enum
{
   QAPI_ZB_CL_IASACE_ALARM_STATUS_NO_ALARM_E        = 0x00, /**< No alarm. */
   QAPI_ZB_CL_IASACE_ALARM_STATUS_BURGLAR_E         = 0x01, /**< Burglar. */
   QAPI_ZB_CL_IASACE_ALARM_STATUS_FIRE_E            = 0x02, /**< Fire. */
   QAPI_ZB_CL_IASACE_ALARM_STATUS_EMERGENCY_E       = 0x03, /**< Emergency. */
   QAPI_ZB_CL_IASACE_ALARM_STATUS_POLICE_PANIC_E    = 0x04, /**< Police panic. */
   QAPI_ZB_CL_IASACE_ALARM_STATUS_FIRE_PANIC_E      = 0x05, /**< Fire panic. */
   QAPI_ZB_CL_IASACE_ALARM_STATUS_EMERGENCY_PANIC_E = 0x06  /**< Emergency panic. */
} qapi_ZB_CL_IASACE_Alarm_Status_t;

/**
   Enumeration of the values for the bypass result.
*/
typedef enum
{
   QAPI_ZB_CL_IASACE_BYPASS_RESULT_ZONE_BYPASSED_E     = 0x00, /**< Zone bypassed. */
   QAPI_ZB_CL_IASACE_BYPASS_RESULT_ZONE_NOT_BYPASSED_E = 0x01, /**< Zone not bypassed. */
   QAPI_ZB_CL_IASACE_BYPASS_RESULT_NOT_ALLOWED_E       = 0x02, /**< Not allowed. */
   QAPI_ZB_CL_IASACE_BYPASS_RESULT_INVALID_ZONE_ID_E   = 0x03, /**< Invalid zone ID. */
   QAPI_ZB_CL_IASACE_BYPASS_RESULT_UNKNOWN_ZONE_ID_E   = 0x04, /**< Unknown zone ID. */
   QAPI_ZB_CL_IASACE_BYPASS_RESULT_INVALID_ARM_CODE_E  = 0x05  /**< Invalid arm/disarm code. */
} qapi_ZB_CL_IASACE_Bypass_Result_t;

/**
   Structure representing a zone table entry.
*/
typedef struct qapi_ZB_CL_IASACE_Zone_Table_Entry_s
{
   /** Zone ID. */
   uint8_t                        ZoneID;

   /** Zone type. */
   qapi_ZB_CL_IASZone_Zone_Type_t ZoneType;

   /** Zone address. */
   uint64_t                       ZoneAddress;

   /** Zone label.*/
   uint8_t                        ZoneLabel[QAPI_ZB_CL_IASACE_ZONE_LABEL_MAX_LENGTH + 1];
} qapi_ZB_CL_IASACE_Zone_Table_Entry_t;

/**
   Structure representing the arm response event for an IAS ACE client.
*/
typedef struct qapi_ZB_CL_IASACE_Arm_Response_s
{
   /** Arm notification. */
   qapi_ZB_CL_IASACE_Arm_Notification_t ArmNotification;
} qapi_ZB_CL_IASACE_Arm_Response_t;

/**
   Structure representing the Get Zone ID Map response event for an IAS ACE
   client.
*/
typedef struct qapi_ZB_CL_IASACE_Get_Zone_ID_Map_Response_s
{
   /** Array of 16 x 16-bit bitmap indicating which zone IDs are allocated. */
   const uint16_t *ZoneIDMap;
} qapi_ZB_CL_IASACE_Get_Zone_ID_Map_Response_t;

/**
   Structure representing the Get Zone Information Response command parameters.
*/
typedef struct qapi_ZB_CL_IASACE_Get_Zone_Info_Response_s
{
   /** Zone ID. */
   uint8_t                         ZoneID;

   /** Zone type. */
   qapi_ZB_CL_IASZone_Zone_Type_t  ZoneType;

   /** Zone address. This will be set to
       QAPI_ZB_CL_DATA_INVALID_VALUE_IEEE_ADDRESS if the zone is not allocated. */
   uint64_t                        ZoneAddress;

   /** Zone label. */
   const uint8_t                  *ZoneLabel;
} qapi_ZB_CL_IASACE_Get_Zone_Info_Response_t;

/**
   Structure representing the Zone Status Changed command parameters.
*/
typedef struct qapi_ZB_CL_IASACE_Zone_Status_Changed_s
{
   /** Zone ID. */
   uint8_t         ZoneID;

   /** Status of the zone. */
   uint16_t        Status;

   /** Audible notification configuration of the zone. */
   uint8_t         AudibleNotification;

   /** Zone label. */
   const uint8_t  *ZoneLabel;
} qapi_ZB_CL_IASACE_Zone_Status_Changed_t;

/**
   Structure representing the panel status information.
*/
typedef struct qapi_ZB_CL_IASACE_Panel_Status_Info_s
{
   /** Current panel status. */
   qapi_ZB_CL_IASACE_Panel_Status_t PanelStatus;

   /** Seconds remaining in the current panel status. */
   uint8_t                          SecondsRemaining;

   /** Audible notification configuration. */
   uint8_t                          AudibleNotification;

   /** Alarm status. */
   qapi_ZB_CL_IASACE_Alarm_Status_t AlarmStatus;
} qapi_ZB_CL_IASACE_Panel_Status_Info_t;

/**
   Structure representing the Set Bypassed Zone List command parameters.
*/
typedef struct qapi_ZB_CL_IASACE_Set_Bypassed_Zone_List_s
{
   /** Number of zones in the bypassed zone list. */
   uint8_t        NumberOfZones;

   /** List of zones to be bypassed. */
   const uint8_t *ZoneIDList;
} qapi_ZB_CL_IASACE_Set_Bypassed_Zone_List_t;

/**
   Structure representing the zone status.
*/
typedef struct qapi_ZB_CL_IASACE_Zone_Status_s
{
   /** Zone ID. */
   uint8_t  ZoneID;

   /** Status of the zone. */
   uint16_t Status;
} qapi_ZB_CL_IASACE_Zone_Status_t;

/**
   Structure representing the Bypass Response command parameters.
*/
typedef struct qapi_ZB_CL_IASACE_Bypass_Response_s
{
   /** Number of zones included in the response. */
   uint8_t                                  NumberOfZones;

   /** List of results for the requested zones. */
   const qapi_ZB_CL_IASACE_Bypass_Result_t *BypassResultList;
} qapi_ZB_CL_IASACE_Bypass_Response_t;

/**
   Structure representing the Get Zone Status Response command parameters.
*/
typedef struct qapi_ZB_CL_IASACE_Get_Zone_Status_Response_s
{
   /** Flag indicating whether all zone statuses have been queried. */
   qbool_t                                ZoneStatusComplete;

   /** Number of zones in the response. */
   uint8_t                                NumberOfZones;

   /** List of the zone statuses. */
   const qapi_ZB_CL_IASACE_Zone_Status_t *ZoneStatusList;
} qapi_ZB_CL_IASACE_Get_Zone_Status_Response_t;


/**
   Structure representing the Arm command parameters.
*/
typedef struct qapi_ZB_CL_IASACE_Arm_s
{
   /** APS data indication. */
   qapi_ZB_APSDE_Data_Indication_t       APSDEData;

   /** Header of the ZCL frame. */
   qapi_ZB_CL_Header_t                   ZCLHeader;

   /** Arm mode. */
   qapi_ZB_CL_IASACE_Arm_Mode_t          ArmMode;

   /** Zone ID. */
   uint8_t                               ZoneID;

   /** Arm/disarm code. */
   const uint8_t                        *ArmCode;

   /** Pointer to where the callback is to store the status result for the
       arm request. */
   qapi_Status_t                        *StatusResult;

   /** Response of the arm request. */
   qapi_ZB_CL_IASACE_Arm_Notification_t *ArmResponse;
} qapi_ZB_CL_IASACE_Arm_t;

/**
   Structure representing the Bypass command parameters.
*/
typedef struct qapi_ZB_CL_IASACE_Bypass_s
{
   /** APS data indication. */
   qapi_ZB_APSDE_Data_Indication_t    APSDEData;

   /** Header of the ZCL frame. */
   qapi_ZB_CL_Header_t                ZCLHeader;

   /** Number of zones. */
   uint8_t                            NumberOfZones;

   /** List of Zone IDs to be bypassed. */
   const uint8_t                     *ZoneIDList;

   /** Arm/disarm code. */
   const uint8_t                     *ArmCode;

   /** Pointer to where the callback is to store the status result for the
       bypass request. */
   qapi_Status_t                     *StatusResult;

   /** List of responses for the bypass request. The size of this list provided
       to the event callback is sufficient to hold the response for all
       requested zones. */
   qapi_ZB_CL_IASACE_Bypass_Result_t *BypassResultList;
} qapi_ZB_CL_IASACE_Bypass_t;

/**
   Structure representing the information for a delay timeout event.
*/
typedef struct qapi_ZB_CL_IASACE_Delay_Timeout_s
{
   /** The current status of the panel. */
   qapi_ZB_CL_IASACE_Panel_Status_t PanelStatus;
} qapi_ZB_CL_IASACE_Delay_Timeout_t;

/**
   Structure representing the Get Zone Info command parameters.
*/
typedef struct qapi_ZB_CL_IASACE_Get_Zone_Info_s
{
   /** Zone ID requested. */
   uint8_t  ZoneID;
} qapi_ZB_CL_IASACE_Get_Zone_Info_t;

/**
   Structure representing the Get Zone Status command parameters.
*/
typedef struct qapi_ZB_CL_IASACE_Get_Zone_Status_s
{
   /** Starting zone ID at which the client would like to obtain status
       information. */
   uint8_t  StartingZoneID;

   /** Maximum number of zone IDs to be returned by the IAS ACE server. */
   uint8_t  MaxNumberOfZones;

   /** Flag indicating whether the server should include only the zones whose status
       is equal to one or more values in ZoneStatusMask. */
   qbool_t  ZoneStatusMaskFlag;

   /** Zone status mask. */
   uint16_t ZoneStatusMask;
} qapi_ZB_CL_IASACE_Get_Zone_Status_t;

/**
   Structure representing the data for events received by an IAS ACE client
   cluster.
*/
typedef struct qapi_ZB_CL_IASACE_Client_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_IASACE_Client_Event_Type_t           Event_Type;

   /** Data for the event. The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for custom read events. */
      qapi_ZB_CL_Attr_Custom_Read_t                Attr_Custom_Read;

      /** Information for custom write events. */
      qapi_ZB_CL_Attr_Custom_Write_t               Attr_Custom_Write;

      /** Information for default response events. */
      qapi_ZB_CL_Default_Response_t                Default_Response;

      /** Information for command complete events. */
      qapi_ZB_CL_Command_Complete_t                Command_Complete;

      /** Information for an unparsed response. */
      qapi_ZB_CL_Unparsed_Response_t               Unparsed_Response;

      /** Information for an arm response. */
      qapi_ZB_CL_IASACE_Arm_Response_t             Arm_Response;

      /** Information for a Get Zone ID Map response. */
      qapi_ZB_CL_IASACE_Get_Zone_ID_Map_Response_t Get_Zone_ID_Map_Response;

      /** Information for a Get Zone Info response. */
      qapi_ZB_CL_IASACE_Get_Zone_Info_Response_t   Get_Zone_Info_Response;

      /** Information for a Zone Status Changed event. */
      qapi_ZB_CL_IASACE_Zone_Status_Changed_t      Zone_Status_Changed;

      /** Information for a Panel Status Changed event. */
      qapi_ZB_CL_IASACE_Panel_Status_Info_t        Panel_Status_Changed;

      /** Information for a Get Panel Status Changed event. */
      qapi_ZB_CL_IASACE_Panel_Status_Info_t        Get_Panel_Status_Response;

      /** Information for a Set Bypassed Zone List event. */
      qapi_ZB_CL_IASACE_Set_Bypassed_Zone_List_t   Set_Bypassed_Zone_List;

      /** Information for a Bypass response. */
      qapi_ZB_CL_IASACE_Bypass_Response_t          Bypass_Response;

      /** Information for a Get Zone Status response. */
      qapi_ZB_CL_IASACE_Get_Zone_Status_Response_t Get_Zone_Status_Response;
   } Data;
} qapi_ZB_CL_IASACE_Client_Event_Data_t;

/**
   Structure representing the data for events received by an IAS ACE server
   cluster.
*/
typedef struct qapi_ZB_CL_IASACE_Server_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_IASACE_Server_Event_Type_t  Event_Type;

   /** Data for the event. The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for custom read events. */
      qapi_ZB_CL_Attr_Custom_Read_t     Attr_Custom_Read;

      /** Information for custom write events. */
      qapi_ZB_CL_Attr_Custom_Write_t    Attr_Custom_Write;

      /** Information for unparsed data events. */
      qapi_ZB_CL_Unparsed_Data_t        Unparsed_Data;

      /** Information for an Arm command. */
      qapi_ZB_CL_IASACE_Arm_t           Arm;

      /** Information for a Bypass command. */
      qapi_ZB_CL_IASACE_Bypass_t        Bypass;

      /** Information for the delay timeout event. */
      qapi_ZB_CL_IASACE_Delay_Timeout_t Delay_Timeout;
   } Data;
} qapi_ZB_CL_IASACE_Server_Event_Data_t;

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for an IAS ACE client cluster.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] Cluster     Handle for the cluster.
   @param[in] Event_Data  Information for the cluster event.
   @param[in] CB_Param    User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_IASACE_Client_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_IASACE_Client_Event_Data_t *Event_Data, uint32_t CB_Param);

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for an IAS ACE server cluster.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] Cluster     Handle for the cluster.
   @param[in] Event_Data  Pointer to information for the cluster event.
   @param[in] CB_Param    User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_IASACE_Server_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_IASACE_Server_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Creates an IAS ACE cluster client for a specific endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function will provide responses to IAS ACE
   commands that are issued using this cluster.

   @param[in]  ZB_Handle    Handle of a ZigBee stack.
   @param[out] Cluster      Pointer to where the newly created cluster will be
                            stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for the
                            IAS ACE client cluster.
   @param[in]  CB_Param     Callback parameter for the cluster callback
                            function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASACE_Create_Client(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_IASACE_Client_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Creates an IAS ACE cluster server for a specific endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function will provide events when state
   change commands are received.

   @param[in]  ZB_Handle    Handle of a ZigBee stack.
   @param[out] Cluster      Pointer to where the newly created cluster will be
                            stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for the
                            IAS ACE client cluster.
   @param[in]  CB_Param     Callback parameter for the cluster callback
                            function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASACE_Create_Server(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_IASACE_Server_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Populates an attribute list with the attributes required for an IAS
          ACE cluster.

   @param[in]     Server         Flag indicating whether this function should
                                 populate the required IAS zone server
                                 attributes (true) or client attributes (false).
   @param[in,out] AttributeCount Pointer to the number of attributes in the
                                 attribute list. The initial value of this
                                 parameter should be set to the actual size of
                                 AttributeList. Upon successful return (or a
                                 QAPI_ERR_BOUNDS error), this value will
                                 represent the number of attributes that were
                                 (or would be) populated into AttributeList.
   @param[out]    AttributeList  Pointer to the attribute list that will be
                                 populated.

   @return
     - QAPI_OK          if the request executed successfully.
     - QAPI_ERR_BOUNDS  if the attribute list provided was not large enough.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASACE_Populate_Attributes(qbool_t Server, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList);

/**
   @brief Sends an Arm command to an IAS ACE server.

   @param[in] Cluster  Handle for the IAS ACE client cluster.
   @param[in] SendInfo General send information for the command packet.
   @param[in] ZoneID   ID of the zone to arm or disarm.
   @param[in] ArmMode  Arm mode.
   @param[in] ArmCode  Arm/disarm code.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASACE_Client_Send_Arm(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint8_t ZoneID, qapi_ZB_CL_IASACE_Arm_Mode_t ArmMode, const uint8_t *ArmCode);

/**
   @brief Sends a Bypass command to an IAS ACE server.

   @param[in] Cluster       Handle for the IAS ACE client cluster.
   @param[in] SendInfo      General send information for the command packet.
   @param[in] NumberOfZones Number of zones in the zone ID list.
   @param[in] ZoneIDList    List of zones to bypass.
   @param[in] ArmCode       Arm/disarm code.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASACE_Client_Send_Bypass(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint8_t NumberOfZones, const uint8_t *ZoneIDList, const uint8_t *ArmCode);

/**
   @brief Sends an Emergency command to an IAS ACE server.

   @param[in] Cluster  Handle for the IAS ACE client cluster.
   @param[in] SendInfo General send information for the command packet.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASACE_Client_Send_Emergency(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

/**
   @brief Sends a Fire command to an IAS ACE server.

   @param[in] Cluster  Handle for the IAS ACE client cluster.
   @param[in] SendInfo General send information for the command packet.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASACE_Client_Send_Fire(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

/**
   @brief Sends a Panic command to an IAS ACE server.

   @param[in] Cluster  Handle for the IAS ACE client cluster.
   @param[in] SendInfo General send information for the command packet.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASACE_Client_Send_Panic(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

/**
   @brief Sends a Get Zone ID Map command to an IAS ACE server.

   @param[in] Cluster  Handle for the IAS ACE client cluster.
   @param[in] SendInfo General send information for the command packet.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASACE_Client_Send_Get_ZoneID_Map(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

/**
   @brief Sends a Get Zone Information command to an IAS ACE server.

   @param[in] Cluster  Handle for the IAS ACE client cluster.
   @param[in] SendInfo General send information for the command packet.
   @param[in] ZoneID   ID of the Zone.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASACE_Client_Send_Get_Zone_Info(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint8_t ZoneID);

/**
   @brief Sends a Get Panel Status command to an IAS ACE server.

   @param[in] Cluster  Handle for the IAS ACE client cluster.
   @param[in] SendInfo General send information for the command packet.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASACE_Client_Send_Get_Panel_Status(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

/**
   @brief Sends a Get Bypassed Zone List command to an IAS ACE server.

   @param[in] Cluster  Handle for the IAS ACE client cluster.
   @param[in] SendInfo General send information for the command packet.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASACE_Client_Send_Get_Bypassed_Zone_List(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

/**
   @brief Sends a Get Zone Status command to an IAS ACE server.

   @param[in] Cluster  Handle for the IAS ACE client cluster.
   @param[in] SendInfo General send information for the command packet.
   @param[in] Request  Information for the Get Zone Status command.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASACE_Client_Send_Get_Zone_Status(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, const qapi_ZB_CL_IASACE_Get_Zone_Status_t *Request);

/**
   @brief Sets the Arm/Disarm code on the IAS ACE server.

   @param[in] Cluster Handle for the IAS ACE server cluster.
   @param[in] ArmCode Arm/disarm code to set.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASACE_Server_Set_Arm_Code(qapi_ZB_Cluster_t Cluster, const uint8_t *ArmCode);

/**
   @brief Gets the Arm/Disarm code on the IAS ACE server.

   @param[in]  Cluster       Handle for the IAS ACE server cluster.
   @param[in]  ArmCodeLength Length of the buffer provided for the arm code. It
                             is recommended this value is at least
                             QAPI_ZB_CL_IASACE_ARM_CODE_MAX_LENGTH + 1.
   @param[out] ArmCode       Buffer to store the arm/disarm code into.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASACE_Server_Get_Arm_Code(qapi_ZB_Cluster_t Cluster, uint8_t ArmCodeLength, uint8_t *ArmCode);

/**
   @brief Sets the Panel Status on the IAS ACE server.

   @param[in] Cluster     Handle for the IAS ACE server cluster.
   @param[in] PanelStatus Panel status information to set.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASACE_Server_Set_Panel_Status(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_IASACE_Panel_Status_Info_t *PanelStatus);

/**
   @brief Get the current Panel Status of the IAS ACE server.

   @param[in] Cluster     Handle for the IAS ACE server cluster.
   @param[in] PanelStatus Pointer to where the current panel status will be
                          stored upon successful return.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASACE_Server_Get_Panel_Status(qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_IASACE_Panel_Status_Info_t *PanelStatus);

/**
   @brief Adds a zone table entry on the IAS ACE server.

   @param[in] Cluster Handle for the IAS ACE server cluster.
   @param[in] Request Information for the zone table entry.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASACE_Server_Add_Zone(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_IASACE_Zone_Table_Entry_t *Request);

/**
   @brief Deletes a zone table entry on the IAS ACE server.

   @param[in] Cluster Handle for the IAS ACE server cluster.
   @param[in] ZoneID  ID of the zone to be deleted.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASACE_Server_Delete_Zone(qapi_ZB_Cluster_t Cluster, uint8_t ZoneID);

/**
   @brief Sets the zone status of a zone in the zone table on the IAS ACE
          server.

   @param[in] Cluster             Handle for the IAS ACE server cluster.
   @param[in] ZoneID              Zone ID.
   @param[in] ZoneStatus          New status for the zone.
   @param[in] AudibleNotification Audible notification configuration for the
                                  zone.
   @param[in] AlarmStatus         Alarm status for the panel. This is only
                                  applicable if alarm 1 or alarm 2 is active
                                  in the provided zone status.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASACE_Server_Set_Zone_Status(qapi_ZB_Cluster_t Cluster, uint8_t ZoneID, uint16_t ZoneStatus, uint8_t AudibleNotification, qapi_ZB_CL_IASACE_Alarm_Status_t AlarmStatus);

/**
   @brief Sets the zone status of a zone in the zone table on the IAS ACE
          server.

   @param[in] Cluster Handle for the IAS ACE server cluster.
   @param[in] ZoneID  ID of the zone to configure.
   @param[in] Permit  Flag indicating whether bypassing the zone is permitted.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASACE_Server_Set_Bypass_Permit(qapi_ZB_Cluster_t Cluster, uint8_t ZoneID, qbool_t Permit);

/**
   @brief Sets the zone status of a zone in the zone table on the IAS ACE
          server.

   @param[in]  Cluster      Handle for the IAS ACE server cluster.
   @param[in]  ZoneID       ID of the zone to configure.
   @param[in]  Bypass       Flag indicating whether the zone should be bypassed.
   @param[out] BypassResult Result of the operation.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASACE_Server_Set_Bypass(qapi_ZB_Cluster_t Cluster, uint8_t ZoneID, qbool_t Bypass, qapi_ZB_CL_IASACE_Bypass_Result_t *BypassResult);

/**
   @brief Retrieves a zone table entry by ID from the IAS ACE
          server.

   @param[in]  Cluster    Handle for the IAS ACE server cluster.
   @param[in]  ZoneID     ID of the zone to be found.
   @param[out] ZoneInfo   Table entry for the zone.
   @param[out] ZoneStatus Current status of the zone.

   @return
     - QAPI_OK if the command executed successfully.
     - QAPI_ERR_NO_ENTRY if the zone was not found.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASACE_Server_Find_Zone_By_ID(qapi_ZB_Cluster_t Cluster, uint8_t ZoneID, qapi_ZB_CL_IASACE_Zone_Table_Entry_t *ZoneInfo, uint16_t *ZoneStatus);

/**
   @brief Retrieves a zone table entry by address from the IAS ACE
          server.

   @param[in]  Cluster     Handle for the IAS ACE server cluster.
   @param[in]  ZoneAddress Address of the zone to be found.
   @param[out] ZoneInfo    Table entry for the zone.
   @param[out] ZoneStatus  Current status of the zone.

   @return
     - QAPI_OK if the command executed successfully.
     - QAPI_ERR_NO_ENTRY if the zone was not found.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASACE_Server_Find_Zone_By_Address(qapi_ZB_Cluster_t Cluster, uint64_t ZoneAddress, qapi_ZB_CL_IASACE_Zone_Table_Entry_t *ZoneInfo, uint16_t *ZoneStatus);

/**
   @brief Retrieves a list of zones currentltly registerd with the IAS ACE
          server.

   @param[in]     Cluster  Handle for the IAS ACE server cluster.
   @param[in,out] ListSize Pointer to the number of entries in the zone list.
                           The initial value of this parameter should be set to
                           the actual size of ZoneList. Upon successful
                           return (or a QAPI_ERR_BOUNDS error), this value will
                           represent the number of zones that were (or would be)
                           populated into ZoneList.
   @param[out]    ZoneList Pointer to the zone list that will be populated.

   @return
     - QAPI_OK if the command executed successfully.
     - QAPI_ERR_BOUNDS if the attribute list provided was not large enough.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASACE_Server_Get_Zone_List(qapi_ZB_Cluster_t Cluster, uint8_t *ListSize, uint8_t *ZoneList);

/** @} */

#endif // ] #ifndef __QAPI_ZB_CL_IASACE_H__

