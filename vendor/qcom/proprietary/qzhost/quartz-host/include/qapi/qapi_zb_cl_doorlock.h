/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_cl_doorlock.h
   @brief QAPI for the ZigBee door lock cluster.

   @addtogroup qapi_zb_cl_doorlock
   @{

   These APIs provide definitions, commands, and events related to the ZigBee
   door lock cluster.

   APIs are provided to create door lock server and client clusters.
   Cluster creation also registers an event callback that is used to receive
   events from the door lock cluster. Client events for the door lock
   cluster are for command responses and server events indicate state changes.

   @}
*/

#ifndef __QAPI_ZB_CL_DOORLOCK_H__ // [
#define __QAPI_ZB_CL_DOORLOCK_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_zb_cl.h"

/** @addtogroup qapi_zb_cl_doorlock
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following definitions represent the attribute IDs for the Door lock
   cluster. */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_LOCK_STATE                                  (0x0000) /**< Door lock cluster attribute ID for the lock state (read only, reportable,
                                                                                              enum8). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_LOCK_TYPE                                   (0x0001) /**< Door lock cluster attribute ID for the lock type (read only, enum8). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_ACTUATOR_ENABLED                            (0x0002) /**< Door lock cluster attribute ID for actuator enabled (read only, bool). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_DOOR_STATE                                  (0x0003) /**< Door lock cluster attribute ID for the door state (read only, reportable,
                                                                                              enum8). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_DOOR_OPEN_EVENTS                            (0x0004) /**< Door lock cluster attribute ID for door open events (uint32). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_DOOR_CLOSED_EVENTS                          (0x0005) /**< Door lock cluster attribute ID for door closed events (uint32). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_OPEN_PERIOD                                 (0x0006) /**< Door lock cluster attribute ID for the open period (uint16). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_LOG_RECORDS_SUPPORTED                       (0x0010) /**< Door lock cluster attribute ID for the number of log records supported
                                                                                              (read only, uint16). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_TOTAL_USERS_SUPPORTED                       (0x0011) /**< Door lock cluster attribute ID for the total number of users supported
                                                                                              (read only, uint16). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_PIN_USERS_SUPPORTED                         (0x0012) /**< Door lock cluster attribute ID for the number of PIN users supported (read
                                                                                              only, uint16). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_RFID_USERS_SUPPORTED                        (0x0013) /**< Door lock cluster attribute ID for the number of RFID users supported (read
                                                                                              only, uint16). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_WEEKDAY_SCHEDULES_PER_USER                  (0x0014) /**< Door lock cluster attribute ID for the number of weekday schedules
                                                                                              supported per user (read only, uint8). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_YEARDAY_SCHEDULES_PER_USER                  (0x0015) /**< Door lock cluster attribute ID for the number of year-day schedules
                                                                                              supported per user (read only, uint8). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_HOLIDAY_SCHEDULES_PER_USER                  (0x0016) /**< Door lock cluster attribute ID for the number of holiday schedules
                                                                                              supported (read only, uint8). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_MAX_PIN_CODE_LENGTH                         (0x0017) /**< Door lock cluster attribute ID for the maximum length of a PIN code (read
                                                                                              only, uint8). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_MIN_PIN_CODE_LENGTH                         (0x0018) /**< Door lock cluster attribute ID for the minimum length of a PIN code (read
                                                                                              only, uint8). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_MAX_RFID_CODE_LENGTH                        (0x0019) /**< Door lock cluster attribute ID for the maximum length of an RFID code (read
                                                                                              only, uint8). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_MIN_RFID_CODE_LENGTH                        (0x001A) /**< Door lock cluster attribute ID for the minimum length of an RFID code (read
                                                                                              only, uint8). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_ENABLE_LOGGING                              (0x0020) /**< Door lock cluster attribute ID to enable logging (reportable, bool). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_LANGUAGE                                    (0x0021) /**< Door lock cluster attribute ID for the language (reportable, 3 byte
                                                                                              string). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_LED_SETTINGS                                (0x0022) /**< Door lock cluster attribute ID for the LED settings (reportable, uint8). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_AUTO_RELOCK_TIME                            (0x0023) /**< Door lock cluster attribute ID for the auto-relock time (reportable,
                                                                                              uint32). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_SOUND_VOLUME                                (0x0024) /**< Door lock cluster attribute ID for the sound volume (reportable, uint8). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_OPERATING_MODE                              (0x0025) /**< Door lock cluster attribute ID for the operating mode (reportable, enum8). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_SUPPORTED_OPERATING_MODES                   (0x0026) /**< Door lock cluster attribute ID for the supported operating modes (read
                                                                                              only, map16). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_DEFAULT_CFG_REGISTER                        (0x0027) /**< Door lock cluster attribute ID for the default configuration register (read
                                                                                              only, reportable, map16). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_ENABLE_LOCAL_PROGRAMMING                    (0x0028) /**< Door lock cluster attribute ID to enable local programming (reportable,
                                                                                              bool). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_ENABLE_ONE_TOUCH_LOCKING                    (0x0029) /**< Door lock cluster attribute ID to enable one-touch locking (reportable,
                                                                                              bool). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_ENABLE_INSIDE_STATUS_LED                    (0x002A) /**< Door lock cluster attribute ID to enable the inside status LED (reportable,
                                                                                              bool). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_ENABLE_PRIVACY_MODE_BUTTON                  (0x002B) /**< Door lock cluster attribute ID to enable the privacy mode button
                                                                                              (reportable, bool). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_WRONG_CODE_ENTRY_LIMIT                      (0x0030) /**< Door lock cluster attribute ID for the wrong code entry limit (reportable,
                                                                                              uint8). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_USER_CODE_TEMP_DISABLE_TIME                 (0x0031) /**< Door lock cluster attribute ID for the user code temporary disable time
                                                                                              (reportable, uint8). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_SEND_PIN_OVER_THE_AIR                       (0x0032) /**< Door lock cluster attribute ID to allow the PIN to be sent over the air
                                                                                              (reportable, bool). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_REQUIRE_PIN_FOR_RF_OPERATION                (0x0033) /**< Door lock cluster attribute ID to require the PIN to be included in RF
                                                                                              operations (reportable, bool). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_ZIGBEE_SECURITY_LEVEL                       (0x0034) /**< Door lock cluster attribute ID for the required ZigBee security level
                                                                                              (reportable, enum8). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_ALARM_MASK                                  (0x0040) /**< Door lock cluster attribute ID for the alarm mask (reportable, map16). */
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_KEYPAD_OPERATION_EVENT_MASK                 (0x0041) /**< Door lock cluster attribute ID for the keypad operation event mask
                                                                                              (reportable, map16).*/
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_RF_OPERATION_EVENT_MASK                     (0x0042) /**< Door lock cluster attribute ID for the RF operation event mask (reportable,
                                                                                              map16).*/
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_MANUAL_OPERATION_EVENT_MASK                 (0x0043) /**< Door lock cluster attribute ID for the manual operation event mask
                                                                                              (reportable, map16).*/
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_RFID_OPERATION_EVENT_MASK                   (0x0044) /**< Door lock cluster attribute ID for the RFID operation event mask
                                                                                              (reportable, map16).*/
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_KEYPAD_PROGRAMMING_EVENT_MASK               (0x0045) /**< Door lock cluster attribute ID for the keypad programming event mask
                                                                                              (reportable, map16).*/
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_RF_PROGRAMMING_EVENT_MASK                   (0x0046) /**< Door lock cluster attribute ID for the RF programming event mask
                                                                                              (reportable, map16).*/
#define QAPI_ZB_CL_DOORLOCK_ATTR_ID_RFID_PROGRAMMING_EVENT_MASK                 (0x0047) /**< Door lock cluster attribute ID for the RFID programming event mask
                                                                                              (reportable, map16).*/

/* The following definitions represent the command IDs for the Door lock server
   cluster. These IDs are provided predominately for identifying command
   responses. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_LOCK_DOOR                         (0x00) /**< Door lock client command ID for the lock door command. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_UNLOCK_DOOR                       (0x01) /**< Door lock client command ID for the unlock door command. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_TOGGLE                            (0x02) /**< Door lock client command ID for the toggle command. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_UNLOCK_WITH_TIMEOUT               (0x03) /**< Door lock client command ID for the unlock with timeout command. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_GET_LOG_RECORD                    (0x04) /**< Door lock client command ID for the get log record command. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_SET_PIN_CODE                      (0x05) /**< Door lock client command ID for the set PIN code command. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_GET_PIN_CODE                      (0x06) /**< Door lock client command ID for the get PIN code command. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_CLEAR_PIN_CODE                    (0x07) /**< Door lock client command ID for the clear PIN code command. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_CLEAR_ALL_PIN_CODE                (0x08) /**< Door lock client command ID for the clear all PIN codes command. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_SET_USER_STATUS                   (0x09) /**< Door lock client command ID for the set user status command. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_GET_USER_STATUS                   (0x0A) /**< Door lock client command ID for the get user status command. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_SET_WEEK_DAY_SCHEDULE             (0x0B) /**< Door lock client command ID for the set weekday schedule command. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_GET_WEEK_DAY_SCHEDULE             (0x0C) /**< Door lock client command ID for the get weekday schedule command. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_CLEAR_WEEK_DAY_SCHEDULE           (0x0D) /**< Door lock client command ID for the clear weekday schedule. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_SET_YEAR_DAY_SCHEDULE             (0x0E) /**< Door lock client command ID for the set year-day schedule command. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_GET_YEAR_DAY_SCHEDULE             (0x0F) /**< Door lock client command ID for the get year-day schedule command. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_CLEAR_YEAR_DAY_SCHEDULE           (0x10) /**< Door lock client command ID for the clear year-day schedule command.. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_SET_HOLIDAY_SCHEDULE              (0x11) /**< Door lock client command ID for the set holiday schedule command. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_GET_HOLIDAY_SCHEDULE              (0x12) /**< Door lock client command ID for the get holiday schedule command. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_CLEAR_HOLIDAY_SCHEDULE            (0x13) /**< Door lock client command ID for the clear holiday schedule command. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_SET_USER_TYPE                     (0x14) /**< Door lock client command ID for the set user type command. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_GET_USER_TYPE                     (0x15) /**< Door lock client command ID for the get user type command. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_SET_RFID_CODE                     (0x16) /**< Door lock client command ID for the set RFID code command. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_GET_RFID_CODE                     (0x17) /**< Door lock client command ID for the get RFID code command. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_CLEAR_RFID_CODE                   (0x18) /**< Door lock client command ID for the clear RFID code command. */
#define QAPI_ZB_CL_DOORLOCK_CLIENT_COMMAND_ID_CLEAR_ALL_RFID_CODE               (0x19) /**< Door lock client command ID for the clear all RFID codes command. */

#define QAPI_ZB_CL_DOORLOCK_PIN_CODE_MAX_LENGTH                                 (32)  /**< Maximum PIN code length supported by the door lock server cluster. */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Enumeration of events that can be received by a door lock client cluster.
*/
typedef enum
{
   QAPI_ZB_CL_DOORLOCK_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an attribute custom read event. */
   QAPI_ZB_CL_DOORLOCK_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an attribute custom write event. */
   QAPI_ZB_CL_DOORLOCK_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E,  /**< Indicates a Default Response event. */
   QAPI_ZB_CL_DOORLOCK_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E, /**< Indicates an Unparsed Response event. */
   QAPI_ZB_CL_DOORLOCK_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E,  /**< Indicates a Command Complete event. */
   QAPI_ZB_CL_DOORLOCK_CLIENT_EVENT_TYPE_LOCK_RESPONSE_E,     /**< Indicates a Lock Response event. */
   QAPI_ZB_CL_DOORLOCK_CLIENT_EVENT_TYPE_UNLOCK_RESPONSE_E,   /**< Indicates an Unlock Response event. */
   QAPI_ZB_CL_DOORLOCK_CLIENT_EVENT_TYPE_TOGGLE_RESPONSE_E    /**< Indicates a Toggle Response event. */
} qapi_ZB_CL_DoorLock_Client_Event_Type_t;

/**
   Enumeration of events that can be received by a door lock server cluster.
*/
typedef enum
{
   QAPI_ZB_CL_DOORLOCK_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an attribute custom read event. */
   QAPI_ZB_CL_DOORLOCK_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an attribute custom write event. */
   QAPI_ZB_CL_DOORLOCK_SERVER_EVENT_TYPE_UNPARSED_DATA_E,     /**< Indicates an unparsed data event. */
   QAPI_ZB_CL_DOORLOCK_SERVER_EVENT_TYPE_LOCK_STATE_CHANGE_E  /**< Indicates a door lock state change event. */
} qapi_ZB_CL_DoorLock_Server_Event_Type_t;

/**
   Structure representing the response from a door lock cluster's lock command.
*/
typedef struct qapi_ZB_CL_DoorLock_Client_Lock_Response_s
{
   /** Status result of the lock command. Note that per the ZCL specification,
       this status only means that the command was correctly received.*/
   qapi_Status_t Status;

} qapi_ZB_CL_DoorLock_Client_Lock_Response_t;

/**
   Structure representing the response from a door lock cluster's unlock
   command.
*/
typedef struct qapi_ZB_CL_DoorLock_Client_Unlock_Response_s
{
   /** Status result of the unlock operation. Note that per the ZCL
       specification, this status only means that the command was correctly
       received.*/
   qapi_Status_t Status;

} qapi_ZB_CL_DoorLock_Client_Unlock_Response_t;

/**
   Structure representing the response from a door lock cluster's toggle
   command.
*/
typedef struct qapi_ZB_CL_DoorLock_Client_Toggle_Response_s
{
   /** Status result of the toggle operation. Note that per the ZCL
       specification, this status only means that the command was correctly
       received.*/
   qapi_Status_t Status;

} qapi_ZB_CL_DoorLock_Client_Toggle_Response_t;

/**
   Structure representing the data for events received by a door lock client
   cluster.
*/
typedef struct qapi_ZB_CL_DoorLock_Client_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_DoorLock_Client_Event_Type_t Event_Type;

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

      /** Information for unparsed response. */
      qapi_ZB_CL_Unparsed_Response_t               Unparsed_Response;

      /** Information for lock operation response.*/
      qapi_ZB_CL_DoorLock_Client_Lock_Response_t   Lock_Response;

      /** Information for unlock operation response.*/
      qapi_ZB_CL_DoorLock_Client_Unlock_Response_t Unlock_Response;

      /** Information for toggle operation response.*/
      qapi_ZB_CL_DoorLock_Client_Toggle_Response_t Toggle_Response;
   } Data;
} qapi_ZB_CL_DoorLock_Client_Event_Data_t;

/**
   Structure that represents the PIN code used by the door lock cluster.
*/
typedef struct qapi_ZB_CL_DoorLock_PIN_s
{
   /** Length of the PIN. */
   uint8_t  PINLength;

   /** PIN code. */
   uint8_t *PINCode;
}  qapi_ZB_CL_DoorLock_PIN_t;

/**
   Structure representing the information for a state change event on a door
   lock server.
*/
typedef struct qapi_ZB_CL_DoorLock_Server_Lock_State_Changed_s
{
   /** Flag indicates if the state should be changed to locked (true) or
       unlocked (false). */
   qbool_t                   Locked;

   /** PIN code. */
   qapi_ZB_CL_DoorLock_PIN_t PIN;
} qapi_ZB_CL_DoorLock_Server_Lock_State_Changed_t;

/**
   Structure representing the data for events received by a door lock server
   cluster.
*/
typedef struct qapi_ZB_CL_DoorLock_Server_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_DoorLock_Server_Event_Type_t            Event_Type;

   /** Data for the event. The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Contains the Attribute Custom Read data for the corresponding
          event. */
      qapi_ZB_CL_Attr_Custom_Read_t                   Attr_Custom_Read;

      /** Contains the Attribute Custom Write data for the corresponding
          event. */
      qapi_ZB_CL_Attr_Custom_Write_t                  Attr_Custom_Write;

      /** Contains the unparsed data for the corresponding event. */
      qapi_ZB_CL_Unparsed_Data_t                      Unparsed_Data;

      /** Information for the lock state change events. */
      qapi_ZB_CL_DoorLock_Server_Lock_State_Changed_t State_Changed;

   } Data;
} qapi_ZB_CL_DoorLock_Server_Event_Data_t;

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for a door lock client cluster.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] Cluster     Handle for the cluster.
   @param[in] Event_Data  Information for the cluster event.
   @param[in] CB_Param    User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_DoorLock_Client_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_DoorLock_Client_Event_Data_t *Event_Data, uint32_t CB_Param);

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for a door lock server cluster.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] Cluster     Handle for the cluster.
   @param[in] Event_Data  Information for the cluster event.
   @param[in] CB_Param    User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_DoorLock_Server_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_DoorLock_Server_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Creates a door lock cluster client for an endpoint.

   This function is used to create an door lock client cluster for a
   specific endpoint. The cluster can be destroyed using
   qapi_ZB_CL_Destroy_Cluster() in the qapi_zb_cl.h API.

   The callback registered with this function will provide responses to door
   lock commands that are issued using this cluster.

   @param[in]  ZB_Handle    Handle of the ZigBee instance.
   @param[out] Cluster      Pointer to where the newly created cluster will be
                            stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for the
                            door lock client cluster.
   @param[in]  CB_Param     Callback parameter for the callback function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_DoorLock_Create_Client(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_DoorLock_Client_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Creates a door lock cluster server for an endpoint.

   This function is used to create a door lock client cluster for a
   specific endpoint. The cluster can be destroyed using
   qapi_ZB_CL_Destroy_Cluster() in the qapi_zb_cl.h API.

   The callback registered with this function will provide events when the lock
   state changes.

   @param[in]  ZB_Handle    Handle of the ZigBee instance.
   @param[out] Cluster      Pointer to where the newly created cluster will be
                            stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for the
                            door lock server cluster.
   @param[in]  CB_Param     Callback parameter for the callback function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_DoorLock_Create_Server(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_DoorLock_Server_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Populates an attribute list with the attributes required for a door
          lock cluster.

   @param[in]     Server         Flag indicating whether this function should
                                 populate the required door lock server
                                 attributes (true) or client attributes (false).
   @param[in,out] AttributeCount Pointer to the number of attributes in the
                                 attribute list. The initial value of this
                                 parameter should be set to the actual size of
                                 AttributeList. Upon successful return (or a
                                 QAPI_ERR_BOUNDS error), this value will
                                 represent the number of attributes that were
                                 (or would be) populated into the AttributeList.
   @param[out]    AttributeList  Pointer to the attribute list that will be
                                 populated.

   @return
     - QAPI_OK if the request executed successfully.
     - QAPI_ERR_BOUNDS if the attribute list provided was not large enough.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_DoorLock_Populate_Attributes(qbool_t Server, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList);

/**
   @brief Sets the PIN code on the door lock server.

   @param[in] Cluster Handle for the IAS ACE server cluster.
   @param[in] PIN     PIN code to be set. If the PIN code's length is zero, PIN
                      code verification is disabled.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_DoorLock_Server_Set_PIN_Code(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_DoorLock_PIN_t *PIN);

/**
   @brief Gets the PIN code on the door lock server.

   @param[in] Cluster Handle for the IAS ACE server cluster.
   @param[in] PIN     The buffer for the PIN code.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_DoorLock_Server_Get_PIN_Code(qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_DoorLock_PIN_t *PIN);

/**
   @brief Sends a lock command via a door lock client.

   @param[in] Cluster  Handle for the on/off client cluster that is to be used
                       to send the command.
   @param[in] SendInfo General send information for the command packet.
   @param[in] PIN      PIN for the operation. Can be NULL if a PIN code is not
                       required.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_DoorLock_Send_Lock(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, qapi_ZB_CL_DoorLock_PIN_t *PIN);

/**
   @brief Sends an unlock command via a door lock client.

   @param[in] Cluster  Handle for the on/off client cluster that is to be used
                       to send the command.
   @param[in] SendInfo General send information for the command packet.
   @param[in] PIN      PIN for the operation. Can be NULL if a PIN code is not
                       required.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_DoorLock_Send_Unlock(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, qapi_ZB_CL_DoorLock_PIN_t *PIN);

/**
   @brief Sends a toggle command via a door lock client.

   @param[in] Cluster  Handle for the on/off client cluster that is to be used
                       to send the command.
   @param[in] SendInfo General send information for the command packet.
   @param[in] PIN      PIN for the operation. Can be NULL if a PIN code is not
                       required.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_DoorLock_Send_Toggle(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, qapi_ZB_CL_DoorLock_PIN_t *PIN);

#endif // ] #ifndef __QAPI_ZB_CL_DOORLOCK_H__

/** @} */

