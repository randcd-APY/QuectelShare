/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_cl_level_control.h
   @brief QAPI for the ZigBee level control cluster.

   @addtogroup qapi_zb_cl_level_control
   @{

   This API provides definitions, commands, and events related to the ZigBee
   level control cluster.

   APIs are provided to create a level control server and client cluster.
   Cluster creation also registers an event callback that is used to receive
   events from the level control cluster.  Client events for the level control
   cluster are for command responses and server events indicate state changes.

   For level control servers, keep in mind that the actual rate of change for
   the current level is up to the application and, as such, the ZigBee stack
   cannot accurately know what the current level is at any given time. Because
   of this, management of the CurrentLevel attribute must be managed by the
   application.  For a similiar reason, it is also left up to the application to
   set the corresponding On/Off cluster's OnOff attribute according to the
   current level of the device.

   @}
*/

#ifndef __QAPI_ZB_CL_LEVEL_CONTROL_H__ // [
#define __QAPI_ZB_CL_LEVEL_CONTROL_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_zb_cl.h"

/** @addtogroup qapi_zb_cl_level_control
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following definitions represent the attribute IDs for the level control
   cluster. */
#define QAPI_ZB_CL_LEVELCONTROL_ATTR_ID_CURRENT_LEVEL                         (0x0000) /**< Level control cluster attribute ID for the current level (read only,
                                                                                            uint8). Because level transitions are over a period of time, it is expected
                                                                                            that the application will manually maintain this attribute. */
#define QAPI_ZB_CL_LEVELCONTROL_ATTR_ID_REMAINING_TIME                        (0x0001) /**< Level control cluster attribute ID for the remaining time (read only,
                                                                                            uint16).  If this attribute is supported, it should be manually managed by
                                                                                            the application. */
#define QAPI_ZB_CL_LEVELCONTROL_ATTR_ID_ON_OFF_TRANSITION_TIME                (0x0010) /**< Level control cluster attribute ID for the on/off transition time (uint16). */
#define QAPI_ZB_CL_LEVELCONTROL_ATTR_ID_ON_LEVEL                              (0x0011) /**< Level control cluster attribute ID for the on level (uint8). */
#define QAPI_ZB_CL_LEVELCONTROL_ATTR_ID_ON_TRANSITION_TIME                    (0x0012) /**< Level control cluster attribute ID for the on transition time (uint16). */
#define QAPI_ZB_CL_LEVELCONTROL_ATTR_ID_OFF_TRANSITION_TIME                   (0x0013) /**< Level control cluster attribute ID for the off transition time (uint16). */
#define QAPI_ZB_CL_LEVELCONTROL_ATTR_ID_DEFAULT_MOVE_RATE                     (0x0014) /**< Level control cluster attribute ID for the default move rate (uint16). */

/* The following invalid values are used by the transition events to indicates
   a stop command. */
#define QAPI_ZB_CL_LEVELCONTROL_INVALID_LEVEL                                 (0xFF)   /**< Level control cluster invalid level value.  This value is used by the
                                                                                            transition event callback to indicate a stop command. */
#define QAPI_ZB_CL_LEVELCONTROL_INVALID_TRANSITION_TIME                       (0xFFFF) /**< Level control cluster invalid transition time value.  This value is used by
                                                                                            the transition event callback to indicate a stop command. */
/* The following defintions represent the command IDs for the Level Control
   cluster. These IDs are provided predominately for identifying command
   responses. */
#define QAPI_ZB_CL_LEVELCONTROL_CLIENT_COMMAND_ID_MOVE_TO_LEVEL               (0x00) /**< Level Control cluster client ID for the Move to Level command. */
#define QAPI_ZB_CL_LEVELCONTROL_CLIENT_COMMAND_ID_MOVE                        (0x01) /**< Level Control cluster client ID for the Move command. */
#define QAPI_ZB_CL_LEVELCONTROL_CLIENT_COMMAND_ID_STEP                        (0x02) /**< Level Control cluster client ID for the Step command. */
#define QAPI_ZB_CL_LEVELCONTROL_CLIENT_COMMAND_ID_STOP                        (0x03) /**< Level Control cluster client ID for the Stop command. */
#define QAPI_ZB_CL_LEVELCONTROL_CLIENT_COMMAND_ID_MOVE_TO_LEVEL_WITH_ONOFF    (0x04) /**< Level Control cluster client ID for the Move to Level With On/Off command. */
#define QAPI_ZB_CL_LEVELCONTROL_CLIENT_COMMAND_ID_MOVE_WITH_ONOFF             (0x05) /**< Level Control cluster client ID for the Move With On/Off command. */
#define QAPI_ZB_CL_LEVELCONTROL_CLIENT_COMMAND_ID_STEP_WITH_ONOFF             (0x06) /**< Level Control cluster client ID for the Step With On/Off command. */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Enumeration of events that can be received by a level control client cluster.
*/
typedef enum
{
   QAPI_ZB_CL_LEVELCONTROL_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an Attribute Custom Read event. */
   QAPI_ZB_CL_LEVELCONTROL_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an Attribute Custom Write event. */
   QAPI_ZB_CL_LEVELCONTROL_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E,  /**< Indicates a Default Response event. */
   QAPI_ZB_CL_LEVELCONTROL_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E,  /**< Indicates a Command Complete event. */
   QAPI_ZB_CL_LEVELCONTROL_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E  /**< Indicates an Unparsed Response event. */
} qapi_ZB_CL_LevelControl_Client_Event_Type_t;

/**
   Enumeration of events that can be received by a level control server cluster.
*/
typedef enum
{
   QAPI_ZB_CL_LEVELCONTROL_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an Attribute Custom Read event. */
   QAPI_ZB_CL_LEVELCONTROL_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an Attribute Custom Write event. */
   QAPI_ZB_CL_LEVELCONTROL_SERVER_EVENT_TYPE_UNPARSED_DATA_E,     /**< Indicates an Unparsed Data event. */
   QAPI_ZB_CL_LEVELCONTROL_SERVER_EVENT_TYPE_TRANSITION_E,        /**< Indicates a transition event. */
   QAPI_ZB_CL_LEVELCONTROL_SERVER_EVENT_TYPE_STOP_E,              /**< Indicates a stop event. */
} qapi_ZB_CL_LevelControl_Server_Event_Type_t;

/**
   Structure representing the data for events received by a on/off client
   cluster.
*/
typedef struct qapi_ZB_CL_LevelControl_Client_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_LevelControl_Client_Event_Type_t Event_Type;

   /** Data for the event.  The actual structure used is determined by the
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

      /** Information for an unparsed response. */
      qapi_ZB_CL_Unparsed_Response_t Unparsed_Response;
   } Data;
} qapi_ZB_CL_LevelControl_Client_Event_Data_t;

/**
   Structure representing the information for a transition event from a
   level control server.
*/
typedef struct qapi_ZB_CL_LevelControl_Server_Transition_s
{
   /** Level the output should transition to. */
   uint8_t  Level;

   /** Time for the transition. */
   uint16_t TransitionTime;

   /** Flag indicating if the command was issued with on/off. This will result
       in an on/off state change event also being issued. */
   qbool_t  WithOnOff;
} qapi_ZB_CL_LevelControl_Server_Transition_t;

/**
   Structure representing the data for events received by a level control server
   cluster.
*/
typedef struct qapi_ZB_CL_LevelControl_Server_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_LevelControl_Server_Event_Type_t Event_Type;

   /** Data for the event. The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for custom read events. */
      qapi_ZB_CL_Attr_Custom_Read_t               Attr_Custom_Read;

      /** Information for custom write events. */
      qapi_ZB_CL_Attr_Custom_Write_t              Attr_Custom_Write;

      /** Information for unparsed data events. */
      qapi_ZB_CL_Unparsed_Data_t                  Unparsed_Data;

      /** Information for Level Transition events. */
      qapi_ZB_CL_LevelControl_Server_Transition_t Transition;
   } Data;
} qapi_ZB_CL_LevelControl_Server_Event_Data_t;

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for a level control client cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Pointer to information for the cluster event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_LevelControl_Client_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_LevelControl_Client_Event_Data_t *Event_Data, uint32_t CB_Param);

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for a level control server cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Pointer to information for the cluster event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_LevelControl_Server_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_LevelControl_Server_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Creates a level control cluster client for an endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function will provide responses to level
   control commands that are issued using this cluster.

   @param[in]  ZB_Handle    Handle of a ZigBee stack.
   @param[out] Cluster      Pointer to where the newly created cluster will
                            be stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and
                            attribute information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for
                            the level control client cluster.
   @param[in]  CB_Param     Callback parameter for the cluster callback
                            function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_LevelControl_Create_Client(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_LevelControl_Client_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Creates a level control cluster server for an endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function will provide events when state
   change commands are received.

   @param[in]  ZB_Handle    Handle of a ZigBee stack.
   @param[out] Cluster      Pointer to where the newly created cluster will be
                            stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  OnOffCluster Handle for the on/off cluster to be associated with
                            the level control server being created.
   @param[in]  Event_CB     Callback function that will handle events for the
                            level control server cluster.
   @param[in]  CB_Param     Callback parameter for the cluster callback
                            function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_LevelControl_Create_Server(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_Cluster_t OnOffCluster, qapi_ZB_CL_LevelControl_Server_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Populates an attribute list with the attributes required for a
          LevelControl cluster.

   @param[in]     Server         Flag indicating whether this function should
                                 populate the required On/Off server attributes
                                 (true) or client attributes (false).
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
     - QAPI_ERR_BOUNDS  if the buffer holding the attribute is not big enough.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_LevelControl_Populate_Attributes(qbool_t Server, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList);

/**
   @brief Sends a move to level command to a level control server.

   @param[in] Cluster        Handle for the level control client cluster
                             that will be used to send the command.
   @param[in] SendInfo       General send information for the command packet.
   @param[in] Level          Level to which the output is to transition.
   @param[in] TransitionTime Transition time for the level change.
   @param[in] WithOnOff      Flag indicating if the command should be sent with
                             on/off. If TRUE, the OnOff attribute of the on/off
                             cluster associated with the target level control
                             cluster will also be set according to the level
                             specified.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_LevelControl_Send_Move_To_Level(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint8_t Level, uint16_t TransitionTime, qbool_t WithOnOff);

/**
   @brief Sends a move command to a level control server.

   @param[in] Cluster   Handle for the level control client cluster that will
                        be used to send the command.
   @param[in] SendInfo  General send information for the command packet.
   @param[in] MoveDown  Flag indicating if the output should transition up
                        (FALSE) or down (TRUE).
   @param[in] Rate      Transition rate in units per second.
   @param[in] WithOnOff Flag indicating if the command should be sent with
                        on/off. If TRUE, the OnOff attribute of the on/off
                        cluster associated with the target level control cluster
                        will also be set according to the level specified.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_LevelControl_Send_Move(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, qbool_t MoveDown, uint8_t Rate, qbool_t WithOnOff);

/**
   @brief Sends a move to level command to a level control server.

   @param[in] Cluster        Handle for the level control client cluster that
                             will be used to send the command.
   @param[in] SendInfo       General send information for the command packet.
   @param[in] StepDown       Flag indicating if the output should transition up
                             (FALSE) or down (TRUE).
   @param[in] StepSize       Size of each step.
   @param[in] TransitionTime Transition time for each step in tenths of a
                             second.
   @param[in] WithOnOff      Flag indicating if the command should be sent with
                             on/off. If TRUE, the OnOff attribute of the on/off
                             cluster associated with the target level control
                             cluster will also be set according to the level
                             specified.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_LevelControl_Send_Step(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, qbool_t StepDown, uint8_t StepSize, uint16_t TransitionTime, qbool_t WithOnOff);

/**
   @brief Sends a stop command to a level control server.

   This command will stop any current level transition on the target device.

   @param[in] Cluster  Handle for the level control client cluster that will be
                       used to send the command.
   @param[in] SendInfo General send information for the command packet.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_LevelControl_Send_Stop(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

#endif // ] #ifndef __QAPI_ZB_CL_LEVEL_CONTROL_H__

/** @} */

