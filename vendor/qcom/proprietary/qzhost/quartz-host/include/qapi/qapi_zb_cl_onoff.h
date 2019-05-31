/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_cl_onoff.h
   @brief QAPI for the ZigBee on/off cluster.

   @addtogroup qapi_zb_cl_onoff
   @{

   This API provides definitions, commands, and events related to the ZigBee
   On/Off cluster.

   APIs are provided to create an on/off server and client cluster.  Cluster
   creation also registers an event callback that is used to receive events from
   the on/off cluster.  Client events are for command responses and server
   events indicate state changes.

   @}
*/

#ifndef __QAPI_ZB_CL_ONOFF_H__ // [
#define __QAPI_ZB_CL_ONOFF_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_zb_cl.h"

/** @addtogroup qapi_zb_cl_onoff
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following definitions represent the attribute IDs for the on/off cluster. */
#define QAPI_ZB_CL_ONOFF_ATTR_ID_ON_OFF                                         (0x0000) /**< On/off attribute ID for the current state (read only, bool). */
#define QAPI_ZB_CL_ONOFF_ATTR_ID_GLOBAL_SCENE_CONTROL                           (0x4000) /**< On/off attribute ID for the global scene control flag (read only, bool). */
#define QAPI_ZB_CL_ONOFF_ATTR_ID_ON_TIME                                        (0x4001) /**< On/off attribute ID for the on time (uint16). */
#define QAPI_ZB_CL_ONOFF_ATTR_ID_OFF_WAIT_TIME                                  (0x4002) /**< On/off attribute ID for the off wait time (uint16). */

/* The following definitions represent the command IDs for the On/Off cluster.
   These IDs are provided predominately for identifying command responses. */
#define QAPI_ZB_CL_ONOFF_CLIENT_COMMAND_ID_OFF                                  (0x00) /**< On/Off cluster client command ID for the Off command. */
#define QAPI_ZB_CL_ONOFF_CLIENT_COMMAND_ID_ON                                   (0x01) /**< On/Off cluster client command ID for the On command. */
#define QAPI_ZB_CL_ONOFF_CLIENT_COMMAND_ID_TOGGLE                               (0x02) /**< On/Off cluster client command ID for the Toggle command. */
#define QAPI_ZB_CL_ONOFF_CLIENT_COMMAND_ID_OFF_WITH_EFFECT                      (0x03) /**< On/Off cluster client command ID for the Off with Effect command. */
#define QAPI_ZB_CL_ONOFF_CLIENT_COMMAND_ID_ON_WITH_RECALL_GLOBAL_SCENE          (0x04) /**< On/Off cluster client command ID for the On with Recall Global Scene command. */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Enumeration of events that can be received by an on/off client cluster.
*/
typedef enum
{
   QAPI_ZB_CL_ONOFF_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an Attribute Custom Read event. */
   QAPI_ZB_CL_ONOFF_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an Attribute Custom Write event. */
   QAPI_ZB_CL_ONOFF_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E,  /**< Indicates a Default Response event. */
   QAPI_ZB_CL_ONOFF_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E,  /**< Indicates a Command Complete event. */
   QAPI_ZB_CL_ONOFF_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E  /**< Indicates an Unparsed Response event. */
} qapi_ZB_CL_OnOff_Client_Event_Type_t;

/**
   Enumeration of events that can be received by a on/off server cluster.
*/
typedef enum
{
   QAPI_ZB_CL_ONOFF_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an Attribute Custom Read event. */
   QAPI_ZB_CL_ONOFF_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an Attribute Custom Write event. */
   QAPI_ZB_CL_ONOFF_SERVER_EVENT_TYPE_UNPARSED_DATA_E,     /**< Indicates an Unparsed Data event. */
   QAPI_ZB_CL_ONOFF_SERVER_EVENT_TYPE_STATE_CHANGE_E       /**< Indicates a State Change event. */
} qapi_ZB_CL_OnOff_Server_Event_Type_t;

/**
   Structure representing the data for events received by a on/off client
   cluster.
*/
typedef struct qapi_ZB_CL_OnOff_Client_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_OnOff_Client_Event_Type_t Event_Type;

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

      /** Information for an unparsed response. */
      qapi_ZB_CL_Unparsed_Response_t Unparsed_Response;
   } Data;
} qapi_ZB_CL_OnOff_Client_Event_Data_t;

/**
   Structure representing the information for a state change event on
   an on/off server.
*/
typedef struct qapi_ZB_CL_OnOff_Server_State_Changed_s
{
   /** Flag indicating whether the output should be turned on (TRUE) or off (FALSE). */
   qbool_t On;
} qapi_ZB_CL_OnOff_Server_State_Changed_t;

/**
   Structure representing the data for events received by a on/off server
   cluster.
*/
typedef struct qapi_ZB_CL_OnOff_Server_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_OnOff_Server_Event_Type_t Event_Type;

   /** Data for the event. The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for custom read events. */
      qapi_ZB_CL_Attr_Custom_Read_t           Attr_Custom_Read;

      /** Information for custom write events. */
      qapi_ZB_CL_Attr_Custom_Write_t          Attr_Custom_Write;

      /** Information for unparsed data events. */
      qapi_ZB_CL_Unparsed_Data_t              Unparsed_Data;

      /** Information for state change events. */
      qapi_ZB_CL_OnOff_Server_State_Changed_t State_Changed;
   } Data;
} qapi_ZB_CL_OnOff_Server_Event_Data_t;

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for an on/off client cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Information for the cluster event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_OnOff_Client_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_OnOff_Client_Event_Data_t *Event_Data, uint32_t CB_Param);

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for an on/off server cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Pointer to information for the cluster event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_OnOff_Server_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_OnOff_Server_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Creates an on/off cluster client for a specific endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function will provide responses to on/off
   commands that are issued using this cluster.

   @param[in]  ZB_Handle    Handle of a ZigBee stack.
   @param[out] Cluster      Pointer to where the newly created cluster is to be
                            stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for the
                            on/off client cluster.
   @param[in]  CB_Param     Callback parameter for the cluster callback
                            function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_OnOff_Create_Client(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_OnOff_Client_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Creates an on/off cluster server for a specific endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function will provide events when state
   change commands are received.

   @param[in]  ZB_Handle    Handle of a ZigBee stack.
   @param[out] Cluster      Pointer to where the newly created cluster is to be
                            stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for the
                            on/off client cluster.
   @param[in]  CB_Param     Callback parameter for the cluster callback
                            function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_OnOff_Create_Server(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_OnOff_Server_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Populate an attribute list with the attributes required for an On/Off
          cluster.

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
     - QAPI_ERR_BOUNDS  if the attribute list provided was not large enough.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_OnOff_Populate_Attributes(qbool_t Server, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList);

/**
   @brief Sends an off command to an on/off server.

   @param[in] Cluster  Handle for the on/off client cluster that is to be used
                       to send the command.
   @param[in] SendInfo General send information for the command packet.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_OnOff_Send_Off(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

/**
   @brief Sends an on command to an on/off server.

   @param[in] Cluster           Handle for the on/off client cluster that is to
                                be used to send the command.
   @param[in] SendInfo          General send information for the command packet.
   @param[in] RecallGlobalScene Flag that indicates if the target on/off
                                server should set its GlobalSceneControl
                                attribute (if supported).

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_OnOff_Send_On(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, qbool_t RecallGlobalScene);

/**
   @brief Sends a toggle command to an on/off server.

   @param[in] Cluster  Handle for the on/off client cluster that is to be used
                       to send the command.
   @param[in] SendInfo General send information for the command packet.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_OnOff_Send_Toggle(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

/** @} */

#endif // ] #ifndef __QAPI_ZB_CL_ONOFF_H__

