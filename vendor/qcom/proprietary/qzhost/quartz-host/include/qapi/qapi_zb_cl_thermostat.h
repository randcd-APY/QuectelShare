/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$


/** @file qapi_zb_cl_thermostat.h
   @brief QAPI for the ZigBee Thermostat cluster.

   @addtogroup qapi_zb_cl_thermostat
   @{

   These APIs provide definitions, commands, and events related to the ZigBee
   thermostat cluster.

   APIs are provided to create thermostat server and client clusters.
   Cluster creation also registers an event callback that is used to receive
   events from the thermostat cluster. Client events for the thermostat cluster
   are for command responses and server events indicate state changes.

   @}
*/

#ifndef __QAPI_ZB_CL_THERMOSTAT_H__ // [
#define __QAPI_ZB_CL_THERMOSTAT_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_zb_cl.h"

/** @addtogroup qapi_zb_cl_thermostat
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following definitions represent the attribute IDs for the thermostat
   cluster. */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_LOCAL_TEMPERATURE                         (0x0000) /**< Thermostat cluster attribute ID for local temperature (read only,
                                                                                              reportable, int16). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_OUTDOOR_TEMPERATURE                       (0x0001) /**< Thermostat cluster attribute ID for outdoor temperature (read only,
                                                                                              int16). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_OCCUPANCY                                 (0x0002) /**< Thermostat cluster attribute ID for the occupancy flag (read only, map8). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_ABS_MIN_HEAT_SETPOINT_LIMIT               (0x0003) /**< Thermostat cluster attribute ID for the minimum supported level of the
                                                                                              heating setpoint (read only, int16). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_ABS_MAX_HEAT_SETPOINT_LIMIT               (0x0004) /**< Thermostat cluster attribute ID for the maximum supported level of the
                                                                                              heating setpoint (read only, int16). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_ABS_MIN_COOL_SETPOINT_LIMIT               (0x0005) /**< Thermostat cluster attribute ID for the minimum supported level of the
                                                                                              cooling setpoint (read only, int16). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_ABS_MAX_COOL_SETPOINT_LIMIT               (0x0006) /**< Thermostat cluster attribute ID for the maximum supported level of the
                                                                                              cooling setpoint (read only, int16). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_PI_COOLING_DEMAND                         (0x0007) /**< Thermostat cluster attribute ID for the proportional integer cooling demand (read only, reportable, uint8). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_PI_HEATING_DEMAND                         (0x0008) /**< Thermostat cluster attribute ID for the proportional integer heating demand (read only, reportable, uint8). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_HVAC_SYSTEM_TYPE_CFG                      (0x0009) /**< Thermostat cluster attribute ID for the HVAC system type configuration
                                                                                              (map8). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_LOCAL_TEMPERATURE_CALIBRATION             (0x0010) /**< Thermostat cluster attribute ID for the local temperature calibration
                                                                                              (int8). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_OCCUPIED_COOLING_SETPOINT                 (0x0011) /**< Thermostat cluster attribute ID for the occupied cooling setpoint (scene,
                                                                                              int16). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_OCCUPIED_HEATING_SETPOINT                 (0x0012) /**< Thermostat cluster attribute ID for the occupied heating setpoint (scene,
                                                                                              int16). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_UNOCCUPIED_COOLING_SETPOINT               (0x0013) /**< Thermostat cluster attribute ID for the unoccupied cooling setpoint
                                                                                              (int16). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_UNOCCUPIED_HEATING_SETPOINT               (0x0014) /**< Thermostat cluster attribute ID for the unoccupied heating setpoint
                                                                                              (int16). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_MIN_HEAT_SETPOINT_LIMIT                   (0x0015) /**< Thermostat cluster attribute ID for the minimum heating setpoint limit
                                                                                              (int16). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_MAX_HEAT_SETPOINT_LIMIT                   (0x0016) /**< Thermostat cluster attribute ID for the maximum heating setpoint limit
                                                                                              (int16). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_MIN_COOL_SETPOINT_LIMIT                   (0x0017) /**< Thermostat cluster attribute ID for the minimum cooling setpoint limit
                                                                                              (int16). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_MAX_COOL_SETPOINT_LIMIT                   (0x0018) /**< Thermostat cluster attribute ID for the maximum cooling setpoint limit
                                                                                              (int16). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_MIN_SETPOINT_DEAD_BAND                    (0x0019) /**< Thermostat cluster attribute ID for the minimum distance between the
                                                                                              heating and cooling setpoints (int8). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_REMOTE_SENSING                            (0x001A) /**< Thermostat cluster attribute ID for the remote sensing (map8). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_CONTROL_SEQUENCE_OF_OPERATION             (0x001B) /**< Thermostat cluster attribute ID for the control sequence of operation
                                                                                              (enum8). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_SYSTEM_MODE                               (0x001C) /**< Thermostat cluster attribute ID for the system mode (scene, enum8). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_ALARM_MASK                                (0x001D) /**< Thermostat cluster attribute ID for the alarm mask (read only, map8). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_THERMOSTAT_RUNNING_MODE                   (0x001E) /**< Thermostat cluster attribute ID for the thermostat running mode (read only,
                                                                                              enum8). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_START_OF_WEEK                             (0x0020) /**< Thermostat cluster attribute ID for the start of the week (read only,
                                                                                              enum8). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_NUMBER_OF_WEEKLY_TRANSITIONS              (0x0021) /**< Thermostat cluster attribute ID for the number of weekly transitions
                                                                                              supported (read only, uint8). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_NUMBER_OF_DAILY_TRANSITIONS               (0x0022) /**< Thermostat cluster attribute ID for the number of daily transitions
                                                                                              supported (read only, uint8). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_TEMPERATURE_SETPOINT_HOLD                 (0x0023) /**< Thermostat cluster attribute ID for the temperature setpoint hold (enum8). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_TEMPERATURE_SETPOINT_HOLD_DURATION        (0x0024) /**< Thermostat cluster attribute ID for the temperature setpoint hold duration
                                                                                              (uint16). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_THERMOSTAT_PROGRAMMING_OPERATION_MODE     (0x0025) /**< Thermostat cluster attribute ID for the thermostat programming operation
                                                                                              mode (map8). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_THERMOSTAT_RUNNING_STATE                  (0x0029) /**< Thermostat cluster attribute ID for the thermostat running state (read
                                                                                              only, map16). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_SETPOINT_CHANGE_SOURCE                    (0x0030) /**< Thermostat cluster attribute ID for the setpoint change source (read only,
                                                                                              enum8). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_SETPOINT_CHANGE_AMOUNT                    (0x0031) /**< Thermostat cluster attribute ID for the setpoint change amount (read only,
                                                                                              int16). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_SETPOINT_CHANGE_SOURCE_TIMESTAMP          (0x0032) /**< Thermostat cluster attribute ID for the setpoint change timestamp (read
                                                                                              only, UTC). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_AC_TYPE                                   (0x0040) /**< Thermostat cluster attribute ID for the AC type (enum8). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_AC_CAPACITY                               (0x0041) /**< Thermostat cluster attribute ID for the AC capacity (uint16). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_AC_REFRIGERANT_TYPE                       (0x0042) /**< Thermostat cluster attribute ID for the AC refrigerant type (enum8). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_AC_COMPRESSOR_TYPE                        (0x0043) /**< Thermostat cluster attribute ID for the AC compressor type (enum8). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_AC_ERROR_CODE                             (0x0044) /**< Thermostat cluster attribute ID for the AC error code (map32). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_AC_LOUVER_POSITION                        (0x0045) /**< Thermostat cluster attribute ID for the AC louvre position (enum8). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_AC_COIL_TEMPERATURE                       (0x0046) /**< Thermostat cluster attribute ID for the AC coil temperature (read only,
                                                                                              int16). */
#define QAPI_ZB_CL_THERMOSTAT_ATTR_ID_AC_CAPACITY_FORMAT                        (0x0047) /**< Thermostat cluster attribute ID for the AC capacity format (enum8). */

/* The following definitions represent the command IDs for the thermostat server
   cluster. These IDs are provided predominately for identifying command
   responses. */
#define QAPI_ZB_CL_THERMOSTAT_CLIENT_COMMAND_ID_SETPOINT_RAISE_LOWER            (0x00) /**< Thermostat client command ID for the setpoint raise/lower command. */
#define QAPI_ZB_CL_THERMOSTAT_CLIENT_COMMAND_ID_SET_WEEKLY_SCHEDULE             (0x01) /**< Thermostat client command ID for the set weekly schedule command. */
#define QAPI_ZB_CL_THERMOSTAT_CLIENT_COMMAND_ID_GET_WEEKLY_SCHEDULE             (0x02) /**< Thermostat client command ID for the get weekly schedule command. */
#define QAPI_ZB_CL_THERMOSTAT_CLIENT_COMMAND_ID_CLEAR_WEEKLY_SCHEDULE           (0x04) /**< Thermostat client command ID for the clear weekly schedule command. */
#define QAPI_ZB_CL_THERMOSTAT_CLIENT_COMMAND_ID_GET_RELAY_STATUS_LOG            (0x05) /**< Thermostat client command ID for the get relay status log command. */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Enumeration of events that can be received by a thermostat client cluster.
*/
typedef enum
{
   QAPI_ZB_CL_THERMOSTAT_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an attribute custom read event. */
   QAPI_ZB_CL_THERMOSTAT_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an attribute custom write event. */
   QAPI_ZB_CL_THERMOSTAT_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E,  /**< Indicates a default response event. */
   QAPI_ZB_CL_THERMOSTAT_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E, /**< Indicates an unparsed response event. */
   QAPI_ZB_CL_THERMOSTAT_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E   /**< Indicates a command complete event. */
} qapi_ZB_CL_Thermostat_Client_Event_Type_t;

/**
   Enumeration of events that can be received by a thermostat server
   cluster.
*/
typedef enum
{
   QAPI_ZB_CL_THERMOSTAT_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an attribute custom read event. */
   QAPI_ZB_CL_THERMOSTAT_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an attribute custom write event. */
   QAPI_ZB_CL_THERMOSTAT_SERVER_EVENT_TYPE_UNPARSED_DATA_E,     /**< Indicates an unparsed data event. */
   QAPI_ZB_CL_THERMOSTAT_SERVER_EVENT_TYPE_SETPOINT_CHANGE_E    /**< Indicates a thermostat setpoint raise/lower event. */
} qapi_ZB_CL_Thermostat_Server_Event_Type_t;

/**
   Enumeration of the control sequence of operation values for the thermostat
   cluster.
*/
typedef enum
{
   QAPI_ZB_CL_THERMOSTAT_CONTROL_SEQUENCE_COOLING_ONLY_E                    = 0x00, /**< Cooling only. */
   QAPI_ZB_CL_THERMOSTAT_CONTROL_SEQUENCE_COOLING_WITH_REHEAT_E             = 0x01, /**< Cooling with reheat. */
   QAPI_ZB_CL_THERMOSTAT_CONTROL_SEQUENCE_HEATING_ONLY_E                    = 0x02, /**< Heating only. */
   QAPI_ZB_CL_THERMOSTAT_CONTROL_SEQUENCE_HEATING_WITH_REHEAT_E             = 0x03, /**< Heating with reheat. */
   QAPI_ZB_CL_THERMOSTAT_CONTROL_SEQUENCE_COOLING_AND_HEATING_E             = 0x04, /**< Cooling and heating 4 pipes. */
   QAPI_ZB_CL_THERMOSTAT_CONTROL_SEQUENCE_COOLING_AND_HEATING_WITH_REHEAT_E = 0x05  /**< Cooling and heating 4 with reheat.*/
} qapi_ZB_CL_Thermostat_Control_Sequence_t;

/**
   Enumeration of system modes for the thermostat cluster.
*/
typedef enum
{
   QAPI_ZB_CL_THERMOSTAT_SYSTEM_MODE_OFF_E               = 0x00,  /**< Off. */
   QAPI_ZB_CL_THERMOSTAT_SYSTEM_MODE_AUTO_E              = 0x01,  /**< Auto. */
   QAPI_ZB_CL_THERMOSTAT_SYSTEM_MODE_COOL_E              = 0x03,  /**< Cool. */
   QAPI_ZB_CL_THERMOSTAT_SYSTEM_MODE_HEAT_E              = 0x04,  /**< Heat. */
   QAPI_ZB_CL_THERMOSTAT_SYSTEM_MODE_EMERGENCY_HEATING_E = 0x05,  /**< Emergency heating. */
   QAPI_ZB_CL_THERMOSTAT_SYSTEM_MODE_PRECOOLING_E        = 0x06,  /**< Precooling .*/
   QAPI_ZB_CL_THERMOSTAT_SYSTEM_MODE_FAN_ONLY_E          = 0x07,  /**< Fan only. */
   QAPI_ZB_CL_THERMOSTAT_SYSTEM_MODE_DRY_E               = 0x08,  /**< Dry. */
   QAPI_ZB_CL_THERMOSTAT_SYSTEM_MODE_SLEEP_E             = 0x09   /**< Sleep. */
} qapi_ZB_CL_Thermostat_System_Mode_t;

/**
   Enumeration of setpoint modes for the thermostat cluster.
*/
typedef enum
{
   QAPI_ZB_CL_THERMOSTAT_SETPOINT_MODE_HEAT_E = 0x00,  /**< Heat. */
   QAPI_ZB_CL_THERMOSTAT_SETPOINT_MODE_COOL_E = 0x01,  /**< Cool. */
   QAPI_ZB_CL_THERMOSTAT_SETPOINT_MODE_BOTH_E = 0x02   /**< Both heat and cool. */
} qapi_ZB_CL_Thermostat_Setpoint_Mode_t;

/**
   Structure representing the data for events received by a thermostat client
   cluster.
*/
typedef struct qapi_ZB_CL_Thermostat_Client_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_Thermostat_Client_Event_Type_t Event_Type;

   /** Data for the event. The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for custom read events. */
      qapi_ZB_CL_Attr_Custom_Read_t  Attr_Custom_Read;

      /** Information for custom write events. */
      qapi_ZB_CL_Attr_Custom_Write_t Attr_Custom_Write;

      /** Information for default response events. */
      qapi_ZB_CL_Default_Response_t  Default_Response;

      /** Information for command complete events. */
      qapi_ZB_CL_Command_Complete_t  Command_Complete;

      /** Information for unparsed response. */
      qapi_ZB_CL_Unparsed_Response_t Unparsed_Response;
   } Data;
} qapi_ZB_CL_Thermostat_Client_Event_Data_t;

/**
   Structure representing the information for a setpoint change event on a
   thermostat server.
*/
typedef struct qapi_ZB_CL_Thermostat_Server_Setpoint_Change_s
{
   /** Setpoint mode. */
   qapi_ZB_CL_Thermostat_Setpoint_Mode_t Mode;

   /** Amount of change associated with the setpoint. */
   int8_t                                Amount;
} qapi_ZB_CL_Thermostat_Server_Setpoint_Change_t;

/**
   Structure representing the data for events received by a thermostat server
   cluster.
*/
typedef struct qapi_ZB_CL_Thermostat_Server_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_Thermostat_Server_Event_Type_t         Event_Type;

   /** Data for the event. The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Attribute custom read data for the corresponding
          event. */
      qapi_ZB_CL_Attr_Custom_Read_t                  Attr_Custom_Read;

      /** Attribute custom write data for the corresponding
          event. */
      qapi_ZB_CL_Attr_Custom_Write_t                 Attr_Custom_Write;

      /** Unparsed data for the corresponding event. */
      qapi_ZB_CL_Unparsed_Data_t                     Unparsed_Data;

      /** Information for the setpoint change event. */
      qapi_ZB_CL_Thermostat_Server_Setpoint_Change_t Setpoint_Change;
   } Data;
} qapi_ZB_CL_Thermostat_Server_Event_Data_t;

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for a thermostat client cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Pointer to information for the cluster event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_Thermostat_Client_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Thermostat_Client_Event_Data_t *Event_Data, uint32_t CB_Param);

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for a thermostat server cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Pointer to information for the cluster event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_Thermostat_Server_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Thermostat_Server_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Creates a thermostat cluster client for an endpoint.

   This function is used to create a thermostat client cluster for a
   specific endpoint. The cluster can be destroyed using
   qapi_ZB_CL_Destroy_Cluster() in the qapi_zb_cl.h API.

   The callback registered with this function will provide responses to thermostat
   commands that are issued using this cluster.

   @param[in]  ZB_Handle    Handle of a ZigBee stack.
   @param[out] Cluster      Pointer to where the newly created cluster is to be
                            stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for the
                            thermostat client cluster.
   @param[in]  CB_Param     Callback parameter for the cluster callback
                            function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Thermostat_Create_Client(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_Thermostat_Client_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Creates a thermostat cluster server for an endpoint.

   This function is used to create a thermostat client cluster for a
   specific endpoint. The cluster can be destroyed using
   qapi_ZB_CL_Destroy_Cluster() in the qapi_zb_cl.h API.

   The callback registered with this function will provide events when state
   change commands are received.

   @param[in]  ZB_Handle    Handle of a ZigBee stack.
   @param[out] Cluster      Pointer to where the newly created cluster is to be
                            stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for the
                            thermostat server cluster.
   @param[in]  CB_Param     Callback parameter for the cluster callback
                            function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Thermostat_Create_Server(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_Thermostat_Server_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Populates an attribute list with the attributes required for a
          thermostat cluster.

   @param[in]     Server         Flag indicating if this function should
                                 populate the required thermostat server
                                 attributes (true) or client attributes (false).
   @param[in,out] AttributeCount Pointer to the number of attributes in the
                                 attribute list. The initial value of this
                                 parameter should be set to the actual size of
                                 the AttributeList. Upon successful return (or a
                                 QAPI_ERR_BOUNDS error), this value will
                                 represent the number of attributes that were
                                 (or would be) populated into the AttributeList.
   @param[out]    AttributeList  Pointer to the attribute list that will be
                                 populated.

   @return
     - QAPI_OK          if the request executed successfully.
     - QAPI_ERR_BOUNDS  if the attribute list provided was not large enough.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Thermostat_Populate_Attributes(qbool_t Server, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList);

/**
   @brief Sends a setpoint raise/lower command via a Thermostat client.

   @param[in] Cluster      Handle for the thermostat client cluster that is to
                           be used to send the command.
   @param[in] SendInfo     General send information for the command packet.
   @param[in] Mode         Setpoint mode.
   @param[in] ChangeAmount Change amount in tenths of a degree Celsius.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Thermostat_Send_Change_Setpoint(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, qapi_ZB_CL_Thermostat_Setpoint_Mode_t Mode, int8_t ChangeAmount);

#endif // ] #ifndef __QAPI_ZB_CL_THERMOSTAT_H__

/** @} */

