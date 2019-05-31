/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$


/** @file qapi_zb_cl_wincover.h
   @brief QAPI for the ZigBee window covering cluster.

   @addtogroup qapi_zb_cl_wincover
   @{

   These APIs provides definitions, commands, and events related to the ZigBee
   window covering cluster.

   APIs are provided to create window covering server and client clusters.
   Cluster creation also registers an event callback that is used to receive
   events from the window covering cluster. Client events for the window
   covering cluster are for command responses and server events indicate state
   changes.

   @}
*/

#ifndef __QAPI_ZB_CL_WINCOVER_H__ // [
#define __QAPI_ZB_CL_WINCOVER_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_zb_cl.h"

/** @addtogroup qapi_zb_cl_wincover
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following definitions represent the attribute IDs for the window covering
   cluster. */
#define QAPI_ZB_CL_WINCOVER_ATTR_ID_WINDOW_COVERING_TYPE                        (0x0000) /**< Window covering cluster attribute ID for the window covering type (read only, enum8). */
#define QAPI_ZB_CL_WINCOVER_ATTR_ID_LIFT_CLOSED_LIMIT                           (0x0001) /**< Window covering cluster attribute ID for the physical closed lift limit (read only, uint16). */
#define QAPI_ZB_CL_WINCOVER_ATTR_ID_TILT_CLOSED_LIMIT                           (0x0002) /**< Window covering cluster attribute ID for the physical closed tilt limit (read only, uint16). */
#define QAPI_ZB_CL_WINCOVER_ATTR_ID_CURRENT_LIFT_POSITION                       (0x0003) /**< Window covering cluster attribute ID for the current lift position (read only, uint16). */
#define QAPI_ZB_CL_WINCOVER_ATTR_ID_CURRENT_TILT_POSITION                       (0x0004) /**< Window covering cluster attribute ID for the current tilt position (read only, uint16). */
#define QAPI_ZB_CL_WINCOVER_ATTR_ID_NUM_LIFT_ACTUATORS                          (0x0005) /**< Window covering cluster attribute ID for the number of lift actuators (read only, uint16). */
#define QAPI_ZB_CL_WINCOVER_ATTR_ID_NUM_TILT_ACTUATORS                          (0x0006) /**< Window covering cluster attribute ID for the number of tilt actuators (read only, uint16). */
#define QAPI_ZB_CL_WINCOVER_ATTR_ID_CONFIG_STATUS                               (0x0007) /**< Window covering cluster attribute ID for the configuration and status (read only, map8). */
#define QAPI_ZB_CL_WINCOVER_ATTR_ID_CURRENT_LIFT_PERCENTAGE                     (0x0008) /**< Window covering cluster attribute ID for the current lift percentage (read only, scene, reportable, uint8). */
#define QAPI_ZB_CL_WINCOVER_ATTR_ID_CURRENT_TILT_PERCENTAGE                     (0x0009) /**< Window covering cluster attribute ID for the current tilt percentage (read only, scene, reportable, uint8). */
#define QAPI_ZB_CL_WINCOVER_ATTR_ID_INSTALLED_LIFT_OPEN_LIMIT                   (0x0010) /**< Window covering cluster attribute ID for the installed lift open limit (read only, uint16). */
#define QAPI_ZB_CL_WINCOVER_ATTR_ID_INSTALLED_LIFT_CLOSED_LIMIT                 (0x0011) /**< Window covering cluster attribute ID for the installed lift closed limit (read only, uint16). */
#define QAPI_ZB_CL_WINCOVER_ATTR_ID_INSTALLED_TILT_OPEN_LIMIT                   (0x0012) /**< Window covering cluster attribute ID for the installed tilt open limit (read only, uint16). */
#define QAPI_ZB_CL_WINCOVER_ATTR_ID_INSTALLED_TILT_CLOSED_LIMIT                 (0x0013) /**< Window covering cluster attribute ID for the installed tilt closed limit (read only, uint16). */
#define QAPI_ZB_CL_WINCOVER_ATTR_ID_LIFT_VELOCITY                               (0x0014) /**< Window covering cluster attribute ID for the lift velocity (uint16). */
#define QAPI_ZB_CL_WINCOVER_ATTR_ID_LIFT_ACCELERATION_TIME                      (0x0015) /**< Window covering cluster attribute ID for the lift acceleration time (uint16). */
#define QAPI_ZB_CL_WINCOVER_ATTR_ID_LIFT_DECELERATION_TIME                      (0x0016) /**< Window covering cluster attribute ID for the lift deceleration time (uint16). */
#define QAPI_ZB_CL_WINCOVER_ATTR_ID_MODE                                        (0x0017) /**< Window covering cluster attribute ID for the mode (map8). */
#define QAPI_ZB_CL_WINCOVER_ATTR_ID_INTERMEDIATE_LIFT_SETPOINTS                 (0x0018) /**< Window covering cluster attribute ID for the intermediate lift setpoints (octstr). */
#define QAPI_ZB_CL_WINCOVER_ATTR_ID_INTERMEDIATE_TILT_SETPOINTS                 (0x0019) /**< Window covering cluster attribute ID for the intermediate tilt setpoints (octstr). */

/* The following definitions represent the command IDs for the door lock server
   cluster. These IDs are provided predominately for identifying command
   responses. */
#define QAPI_ZB_CL_WINCOVER_CLIENT_COMMAND_ID_UP_OPEN                           (0x00) /**< Window covering client command ID for the Up/Open command. */
#define QAPI_ZB_CL_WINCOVER_CLIENT_COMMAND_ID_DOWN_CLOSE                        (0x01) /**< Window covering client command ID for the Down/Close command. */
#define QAPI_ZB_CL_WINCOVER_CLIENT_COMMAND_ID_STOP                              (0x02) /**< Window covering client command ID for the Stop command. */
#define QAPI_ZB_CL_WINCOVER_CLIENT_COMMAND_ID_GO_TO_LIFT_VALUE                  (0x04) /**< Window covering client command ID for the Go To Lift Value command. */
#define QAPI_ZB_CL_WINCOVER_CLIENT_COMMAND_ID_GO_TO_LIFT_PERCENTAGE             (0x05) /**< Window covering client command ID for the Go to Lift Percentage command. */
#define QAPI_ZB_CL_WINCOVER_CLIENT_COMMAND_ID_GO_TO_TILT_VALUE                  (0x07) /**< Window covering client command ID for the Go to Tilt Value command. */
#define QAPI_ZB_CL_WINCOVER_CLIENT_COMMAND_ID_GO_TO_TILT_PERCENTAGE             (0x08) /**< Window covering client command ID for the Go to Tilt Percentage command. */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Enumeration of events that can be received by a Window Covering client
   cluster.
*/
typedef enum
{
   QAPI_ZB_CL_WINCOVER_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an Attribute Custom Read event. */
   QAPI_ZB_CL_WINCOVER_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an Attribute Custom Write event. */
   QAPI_ZB_CL_WINCOVER_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E,  /**< Indicates a Default Response event. */
   QAPI_ZB_CL_WINCOVER_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E, /**< Indicates an Unparsed Response event. */
   QAPI_ZB_CL_WINCOVER_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E   /**< Indicates a Command Complete event. */
} qapi_ZB_CL_WinCover_Client_Event_Type_t;

/**
   Enumeration of events that can be received by a Window Covering server
   cluster.
*/
typedef enum
{
   QAPI_ZB_CL_WINCOVER_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an Attribute Custom Read event. */
   QAPI_ZB_CL_WINCOVER_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an Attribute Custom Write event. */
   QAPI_ZB_CL_WINCOVER_SERVER_EVENT_TYPE_UNPARSED_DATA_E,     /**< Indicates an Unparsed Data event. */
   QAPI_ZB_CL_WINCOVER_SERVER_EVENT_TYPE_ACTION_E             /**< Indicates a window covering command has been received. */
} qapi_ZB_CL_WinCover_Server_Event_Type_t;

/**
   Enumeration of window covering types.
*/
typedef enum
{
   QAPI_ZB_CL_WINCOVER_COVER_TYPE_ROLLERSHADE_E                  = 0x00, /**< Rollershade. */
   QAPI_ZB_CL_WINCOVER_COVER_TYPE_ROLLERSHADE_2_MOTOR_E          = 0x00, /**< Two-motor rollershade. */
   QAPI_ZB_CL_WINCOVER_COVER_TYPE_ROLLERSHADE_EXTERIOR_E         = 0x00, /**< Exterior rollershade. */
   QAPI_ZB_CL_WINCOVER_COVER_TYPE_ROLLERSHADE_EXTERIOR_2_MOTOR_E = 0x00, /**< Two-motor exterior rollershade. */
   QAPI_ZB_CL_WINCOVER_COVER_TYPE_DRAPERY_E                      = 0x00, /**< Drapery. */
   QAPI_ZB_CL_WINCOVER_COVER_TYPE_AWNING_E                       = 0x00, /**< Awning. */
   QAPI_ZB_CL_WINCOVER_COVER_TYPE_SHUTTER_E                      = 0x00, /**< Shutter. */
   QAPI_ZB_CL_WINCOVER_COVER_TYPE_TILT_BLIND_TILT_ONLY_E         = 0x00, /**< Tilt blind (tilt only). */
   QAPI_ZB_CL_WINCOVER_COVER_TYPE_TILT_BLIND_LIFT_AND_TILT_E     = 0x00, /**< Tilt blind (lift and tilt). */
   QAPI_ZB_CL_WINCOVER_COVER_TYPE_PROJECTOR_SCREEN_E             = 0x00  /**< Projector screen. */
} qapi_ZB_CL_WinCover_Cover_Type_t;

/**
   Enumeration of window covering actions.
*/
typedef enum
{
   QAPI_ZB_CL_WINCOVER_ACTION_OPEN_E,  /**< Indicates that the window covering is to move to the up/open state. */
   QAPI_ZB_CL_WINCOVER_ACTION_CLOSE_E, /**< Indicates that the window covering is to move to the down/closed state. */
   QAPI_ZB_CL_WINCOVER_ACTION_STOP_E   /**< Indicates that the window covering is to stop moving. @newpage */
} qapi_ZB_CL_WinCover_Action_t;

/**
   Structure representing the data for events received by a window covering client
   cluster.
*/
typedef struct qapi_ZB_CL_WinCover_Client_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_WinCover_Client_Event_Type_t Event_Type;

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
} qapi_ZB_CL_WinCover_Client_Event_Data_t;

/**
   Structure representing the information for a window covering action event.
*/
typedef struct qapi_ZB_CL_WinCover_Server_Action_s
{
   /** State of the window cover. */
   qapi_ZB_CL_WinCover_Action_t Action;
} qapi_ZB_CL_WinCover_Server_Action_t;

/**
   Structure representing the data for events received by a window covering server
   cluster.
*/
typedef struct qapi_ZB_CL_WinCover_Server_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_WinCover_Server_Event_Type_t             Event_Type;

   /** Data for the event. The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Contains the Attribute Custom Read data for the corresponding
          event. */
      qapi_ZB_CL_Attr_Custom_Read_t       Attr_Custom_Read;

      /** Contains the Attribute Custom Write data for the corresponding
          event. */
      qapi_ZB_CL_Attr_Custom_Write_t      Attr_Custom_Write;

      /** Contains the Unparsed Data for the corresponding event. */
      qapi_ZB_CL_Unparsed_Data_t          Unparsed_Data;

      /** Information for action events. */
      qapi_ZB_CL_WinCover_Server_Action_t Action;

   } Data;
} qapi_ZB_CL_WinCover_Server_Event_Data_t;

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for a window covering client cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Information for the cluster event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_WinCover_Client_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_WinCover_Client_Event_Data_t *Event_Data, uint32_t CB_Param);

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for a window covering server cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Information for the cluster event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_WinCover_Server_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_WinCover_Server_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Creates a window covering cluster client for a specific endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function will provide responses to window
   covering commands that are issued using this cluster.

   @param[in]  ZB_Handle    Handle of a ZigBee stack.
   @param[out] Cluster      Pointer to where the newly created cluster is to be
                            stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for the
                            window covering client cluster.
   @param[in]  CB_Param     Callback parameter for the cluster callback
                            function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_WinCover_Create_Client(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_WinCover_Client_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Creates a window covering cluster server for a specific endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function will provide events when state
   change commands are received.

   @param[in]  ZB_Handle     Handle of a ZigBee stack.
   @param[out] Cluster       Pointer to where the newly created cluster is to be
                             stored upon successful return.
   @param[in]  Cluster_Info  Structure containing the endpoint and
                             attribute information for the cluster.
   @param[in]  Event_CB      Callback function that will handle events for
                             the window covering client cluster.
   @param[in]  CB_Param      Callback parameter for the cluster callback
                             function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_WinCover_Create_Server(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_WinCover_Server_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Populateq an attribute list with the attributes required for a window
          covering cluster.

   @param[in]     Server         Flag indicating whether this function should
                                 populate the required window covering server
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
qapi_Status_t qapi_ZB_CL_WinCover_Populate_Attributes(qbool_t Server, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList);

/**
   @brief Sends an up/open command to a window covering server.

   @param[in] Cluster  Handle for the window covering client cluster that is to be used
                       to send the command.
   @param[in] SendInfo General send information for the command packet.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_WinCover_Send_Open(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

/**
   @brief Sends a down/close command to a window covering server.

   @param[in] Cluster  Handle for the window covering client cluster that is to be used
                       to send the command.
   @param[in] SendInfo General send information for the command packet.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_WinCover_Send_Close(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

/**
   @brief Sends a stop command to a window covering server.

   @param[in] Cluster  Handle for the window covering client cluster that is to be used
                       to send the command.
   @param[in] SendInfo General send information for the command packet.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_WinCover_Send_Stop(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

#endif // ] #ifndef __QAPI_ZB_CL_WINCOVER_H__

/** @} */

