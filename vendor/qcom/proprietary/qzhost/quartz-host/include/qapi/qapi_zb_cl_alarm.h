/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_cl_alarm.h
   @brief QAPI for the ZigBee alarm cluster.

   @addtogroup qapi_zb_cl_alarm
   @{

   This API provides definitions, commands, and events related to the ZigBee
   alarm cluster.

   APIs are provided to create an alarm server and client cluster. Cluster
   creation also registers an event callback that is used to receive events from
   the alarm cluster. Client events are for command responses and alarm
   commands, and server events indicate state changes.

   APIs are provided to create an alarm server and a client cluster.  Cluster
   creation also registers an event callback that is used to receive events from
   the alarm cluster.

   @}
*/

#ifndef __QAPI_ZB_CL_ALARM_H__ // [
#define __QAPI_ZB_CL_ALARM_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_zb_cl.h"

/** @addtogroup qapi_zb_cl_alarm
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following definitions represent the attribute IDs for the alarm cluster. */
#define QAPI_ZB_CL_ALARM_ATTR_ID_ALARM_COUNT                            (0x0000) /**< Alarm attribute ID for the current alarm count (uint16). This attribute's
                                                                                      configuration cannot be overwritten by the application. */

/* The following defintions represent the command IDs for the Alarm cluster.
   These IDs are provided predominately for identifying command responses. */
#define QAPI_ZB_CL_ALARM_CLIENT_COMMAND_ID_RESET_ALARM                  (0x00) /**< Basic cluster client ID for the Reset Alarm command. */
#define QAPI_ZB_CL_ALARM_CLIENT_COMMAND_ID_RESET_ALL_ALARMS             (0x01) /**< Basic cluster client ID for the Reset All Alarms command. */
#define QAPI_ZB_CL_ALARM_CLIENT_COMMAND_ID_GET_ALARM                    (0x02) /**< Basic cluster client ID for the Get Alarm command. */
#define QAPI_ZB_CL_ALARM_CLIENT_COMMAND_ID_RESET_ALARM_LOG              (0x03) /**< Basic cluster client ID for the Reset Alarm Log command. */

#define QAPI_ZB_CL_ALARM_SERVER_COMMAND_ID_ALARM                        (0x00) /**< Basic cluster server ID for the Alarm command. */
#define QAPI_ZB_CL_ALARM_SERVER_COMMAND_ID_GET_ALARM_RESPONSE           (0x01) /**< Basic cluster server ID for the Get Alarm Response command. */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Enumeration of events that can be received by an alarm client cluster.
*/
typedef enum
{
   QAPI_ZB_CL_ALARM_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E,   /**< Indicates an attribute custom read event. */
   QAPI_ZB_CL_ALARM_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E,  /**< Indicates an attribute custom write event. */
   QAPI_ZB_CL_ALARM_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E,   /**< Indicates a default response event. */
   QAPI_ZB_CL_ALARM_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E,  /**< Indicates an unparsed response event. */
   QAPI_ZB_CL_ALARM_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E,   /**< Indicates a command complete event. */
   QAPI_ZB_CL_ALARM_CLIENT_EVENT_TYPE_ALARM_E,              /**< Indicates an alarm event. */
   QAPI_ZB_CL_ALARM_CLIENT_EVENT_TYPE_GET_ALARM_RESPONSE_E  /**< Indicates a get alarm response event. */
} qapi_ZB_CL_Alarm_Client_Event_Type_t;

/**
   Enumeration of events that can be received by an alarm server cluster.
*/
typedef enum
{
   QAPI_ZB_CL_ALARM_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an attribute custom read event. */
   QAPI_ZB_CL_ALARM_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an attribute custom write event. */
   QAPI_ZB_CL_ALARM_SERVER_EVENT_TYPE_UNPARSED_DATA_E      /**< Indicates an unparsed data event. */
} qapi_ZB_CL_Alarm_Server_Event_Type_t;

/**
   Structure representing the parameters the Alarm command sends from a server
   to a client.
*/
typedef struct qapi_ZB_CL_Alarm_Alarm_Command_s
{
   /** Cluster-specific alarm code. */
   uint8_t  AlarmCode;
   /** ID of the cluster that generated the alarm. */
   uint16_t ClusterId;
} qapi_ZB_CL_Alarm_Alarm_Command_t;

/**
   Structure representing the parameters the Get Alarm Response command sends
   from a server to a client.
*/
typedef struct qapi_ZB_CL_Alarm_Get_Alarm_Response_s
{
   /** Status of the get alarm response. */
   qapi_Status_t Status;
   /** Cluster-specific alarm code. */
   uint8_t       AlarmCode;
   /** ID of the cluster that generated the alarm. */
   uint16_t      ClusterId;
   /** Timestamp for when the alarm was generated. */
   uint32_t      TimeStamp;
} qapi_ZB_CL_Alarm_Get_Alarm_Response_t;

/**
   Structure representing the events received by an alarm client cluster.
*/
typedef struct qapi_ZB_CL_Alarm_Client_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_Alarm_Client_Event_Type_t Event_Type;

   /** Data for the event.  The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Contains the Attribute Custom Read data for the corresponding event. */
      qapi_ZB_CL_Attr_Custom_Read_t         Attr_Custom_Read;

      /** Contains the Attribute Custom Write data for the corresponding event. */
      qapi_ZB_CL_Attr_Custom_Write_t        Attr_Custom_Write;

      /** Contains the Command Complete data for the corresponding event. */
      qapi_ZB_CL_Command_Complete_t         Command_Complete;

      /** Contains the Default Response data for the corresponding event. */
      qapi_ZB_CL_Default_Response_t         Default_Response;

      /** Contains the data for an unparsed response. */
      qapi_ZB_CL_Unparsed_Response_t        Unparsed_Response;

      /** Contains the data for an alarm command event.  */
      qapi_ZB_CL_Alarm_Alarm_Command_t      Alarm_Command;

      /** Contains the data for a get alarm response. */
      qapi_ZB_CL_Alarm_Get_Alarm_Response_t Get_Alarm_Response;
   } Data;
} qapi_ZB_CL_Alarm_Client_Event_Data_t;

/**
   Structure representing the data for events received by a color control server
   cluster.
*/
typedef struct qapi_ZB_CL_Alarm_Server_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_Alarm_Server_Event_Type_t Event_Type;

   /** Data for the event.  The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Contains the Attribute Custom Read data for the corresponding event. */
      qapi_ZB_CL_Attr_Custom_Read_t  Attr_Custom_Read;

      /** Contains the Attribute Custom Write data for the corresponding event. */
      qapi_ZB_CL_Attr_Custom_Write_t Attr_Custom_Write;

      /** Contains the Unparsed Data for the corresponding event. */
      qapi_ZB_CL_Unparsed_Data_t     Unparsed_Data;
   } Data;
} qapi_ZB_CL_Alarm_Server_Event_Data_t;

/**
   @brief This function definition represents a callback function that handles
          asynchronous events for an alarm client cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Structure containing information for the cluster event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_Alarm_Client_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Alarm_Client_Event_Data_t *Event_Data, uint32_t CB_Param);

/**
   @brief This function definition represents a callback function that handles
          asynchronous events for an alarm server cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Structure containing information for the cluster event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_Alarm_Server_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Alarm_Server_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Creates an alarm cluster client for a specific endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function provides responses to alarm
   commands that are issued using this cluster.

   @param[in]  ZB_Handle    Handle of the ZigBee instance.
   @param[out] Cluster      Pointer to where the newly created cluster will be
                            stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for the
                            alarm client cluster.
   @param[in]  CB_Param     Callback parameter for the callback function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Alarm_Create_Client(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_Alarm_Client_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Creates an alarm cluster server for a specific endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function provides events when state change
   commands are received.

   @param[in]  ZB_Handle    Handle of the ZigBee instance.
   @param[out] Cluster      Pointer to where the newly created cluster will be
                            stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  AlarmLogSize Size of the server's alarm log.
   @param[in]  Event_CB     Callback function that will handle events for the
                            alarm server cluster.
   @param[in]  CB_Param     Callback parameter for the callback function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Alarm_Create_Server(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, uint8_t AlarmLogSize, qapi_ZB_CL_Alarm_Server_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Populates an attribute list with the attributes required for an alarm
          cluster.

   @param[in]     Server         Flag indicating whether this function should
                                 populate the required server attributes (true)
                                 or client attributes (false).
   @param[in,out] AttributeCount Pointer to the number of attributes in the
                                 attribute list. The initial value of this
                                 parameter should be set to the actual size of
                                 AttributeList. Upon successful return or a
                                 QAPI_ERR_BOUNDS error, this value will
                                 represent the number of attributes that were
                                 (or would be) populated into AttributeList.
   @param[out]    AttributeList  Pointer to the attribute list that will be
                                 populated.

   @return
     - QAPI_OK          if the request executed successfully.
     - QAPI_ERR_BOUNDS  if the attribute list provided was not large enough.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Alarm_Populate_Attributes(qbool_t Server, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList);

/**
   @brief Sends a Reset Alarm command via an alarm client.

   @param[in] Cluster         Handle of the alarm client cluster.
   @param[in] SendInfo        Destination information for the command.
   @param[in] SourceClusterId Identifier of the cluster that generated the
                              alarm.
   @param[in] AlarmCode       Code of the alarm being generated.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Alarm_Send_Reset_Alarm(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint16_t SourceClusterId, uint8_t AlarmCode);

/**
   @brief Sends a Reset All Alarms command via an alarm client.

   @param[in] Cluster  Handle to the client cluster.
   @param[in] SendInfo Destination information for the command.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Alarm_Send_Reset_All_Alarms(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

/**
   @brief Sends a Get Alarm command via an alarm client.

   @param[in] Cluster  Handle to the client cluster.
   @param[in] SendInfo Destination information for the command.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Alarm_Send_Get_Alarm(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

/**
   @brief Sends a Reset Alarm log command via an alarm client.

   @param[in] Cluster  Handle to the client cluster.
   @param[in] SendInfo Destination information for the command.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Alarm_Send_Reset_Alarm_Log(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

/**
   @brief Sends an Alarm command via an alarm server cluster and
          adds the alarm into the alarm table.

   Note that all alarms are sent using the binding table.

   @param[in] Cluster       Handle to the alarm server cluster.
   @param[in] SourceCluster Handle to the cluster that is generating the alarm.
   @param[in] AlarmCode     Code of the alarm being generated.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Alarm_Send_Alarm(qapi_ZB_Cluster_t Cluster, qapi_ZB_Cluster_t SourceCluster, uint8_t AlarmCode);

#endif // ] #ifndef __QAPI_ZB_CL_ALARM_H__

/** @} */

