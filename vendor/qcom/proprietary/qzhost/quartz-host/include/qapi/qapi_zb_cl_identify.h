/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_cl_identify.h
   @brief QAPI for the Zigbee Identify cluster.

   @addtogroup qapi_zb_cl_identify
   @{

   This API provides definitions, commands, and events related to the ZigBee
   Identify cluster.

   APIs are provided to create an identify cluster server that also registers
   a callback for identify events on that server (namely when the device should
   start and stop identifying itself).

   By default, the cluster will track the time remaining for the endpoint to
   identify internally and generate the
   QAPI_ZB_CL_IDENTIFY_SERVER_EVENT_TYPE_IDENTIFY_START_E and
   QAPI_ZB_CL_IDENTIFY_SERVER_EVENT_TYPE_IDENTIFY_STOP_E events accordingly.
   However, if the application uses the custom read/write callbacks for the
   identify time attribute, it is assumed the application will then track the
   time remaining and these events will not be generated.
   @}
*/

#ifndef __QAPI_ZB_CL_IDENTIFY_H__ // [
#define __QAPI_ZB_CL_IDENTIFY_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi/qapi_types.h"
#include "qapi/qapi_status.h"
#include "qapi_zb_cl.h"

/** @addtogroup qapi_zb_cl_identify
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* Represents the attribute IDs for the identify
   cluster. */
#define QAPI_ZB_CL_IDENTIFY_ATTRIBUTE_ID_IDENTIFY_TIME                  (0x0000) /**< Identify cluster attribute ID for the identify time (uint16). This
                                                                                      attribute's configuration cannot be overwritten by the application. */

/* The following defintions represent the command IDs for the Identify cluster.
   These IDs are provided predominately for identifying command responses. */
#define QAPI_ZB_CL_IDENTIFY_CLIENT_COMMAND_ID_IDENTIFY                  (0x00) /**< Identify cluster client ID for the Identify command. */
#define QAPI_ZB_CL_IDENTIFY_CLIENT_COMMAND_ID_IDENTIFY_QUERY            (0x01) /**< Identify cluster client ID for the Identify Query command. */
#define QAPI_ZB_CL_IDENTIFY_CLIENT_COMMAND_ID_TRIGGER_EFFECT            (0x40) /**< Identify cluster client ID for the Trigger Effect command. */

#define QAPI_ZB_CL_IDENTIFY_SERVER_COMMAND_ID_IDENTIFY_QUERY_RESPONSE   (0x00) /**< Identify cluster server ID for the Identify Query Response command. */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Enumeration of events that can be received by a level control client cluster.
*/
typedef enum
{
   QAPI_ZB_CL_IDENTIFY_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an Attribute Custom Read event. */
   QAPI_ZB_CL_IDENTIFY_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an Attribute Custom Write event. */
   QAPI_ZB_CL_IDENTIFY_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E,  /**< Indicates a Default Response event. */
   QAPI_ZB_CL_IDENTIFY_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E,  /**< Indicates a Command Complete event. */
   QAPI_ZB_CL_IDENTIFY_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E, /**< Indicates an Unparsed Data event. */
   QAPI_ZB_CL_IDENTIFY_CLIENT_EVENT_TYPE_QUERY_RESPONSE_E     /**< Indicates a Query Response event. */
} qapi_ZB_CL_Identify_Client_Event_Type_t;

/**
   Enumeration of events that can be received by an identify server.
*/
typedef enum
{
   QAPI_ZB_CL_IDENTIFY_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an Attribute Custom Read event. */
   QAPI_ZB_CL_IDENTIFY_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an Attribute Custom Write event. */
   QAPI_ZB_CL_IDENTIFY_SERVER_EVENT_TYPE_UNPARSED_DATA_E,     /**< Indicates an Unparsed Data event. */
   QAPI_ZB_CL_IDENTIFY_SERVER_EVENT_TYPE_IDENTIFY_START_E,    /**< Indicates an identify event. */
   QAPI_ZB_CL_IDENTIFY_SERVER_EVENT_TYPE_IDENTIFY_STOP_E,     /**< Indicates an identify time out event. */
} qapi_ZB_CL_Identify_Server_Event_Type_t;

/**
   Structure representing the query response event on an identify client.
*/
typedef struct qapi_ZB_CL_Identify_Client_Query_Response_s
{
   /** Status result of the command. */
   qapi_Status_t Status;

   /** Current time remaining for the device to identify itself. */
   uint16_t      TimeOut;
} qapi_ZB_CL_Identify_Client_Query_Response_t;

/**
   Structure representing the data for events received for the identify
   cluster.
*/
typedef struct qapi_ZB_CL_Identify_Client_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_Identify_Client_Event_Type_t Event_Type;

   /** Data for the event. The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for custom read events. */
      qapi_ZB_CL_Attr_Custom_Read_t               Attr_Custom_Read;

      /** Information for custom write events. */
      qapi_ZB_CL_Attr_Custom_Write_t              Attr_Custom_Write;

      /** Information for default response events. */
      qapi_ZB_CL_Default_Response_t               Default_Response;

      /** Information for command complete events. */
      qapi_ZB_CL_Command_Complete_t               Command_Complete;

      /** Information for an unparsed response. */
      qapi_ZB_CL_Unparsed_Response_t              Unparsed_Response;

      /** Information for client query response events. */
      qapi_ZB_CL_Identify_Client_Query_Response_t Query_Response;
   } Data;
} qapi_ZB_CL_Identify_Client_Event_Data_t;

/**
   Structure representing the data for events received for the identify
   cluster.
*/
typedef struct qapi_ZB_CL_Identify_Server_Event_Data_s
{
   /** Type of event that was received. */
   qapi_ZB_CL_Identify_Server_Event_Type_t Event_Type;

   /** Data for the event. The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for custom read events. */
      qapi_ZB_CL_Attr_Custom_Read_t  Attr_Custom_Read;

      /** Information for custom write events. */
      qapi_ZB_CL_Attr_Custom_Write_t Attr_Custom_Write;

      /** Information for an unparsed data event. */
      qapi_ZB_CL_Unparsed_Data_t     Unparsed_Data;
   } Data;
} qapi_ZB_CL_Identify_Server_Event_Data_t;

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for a ZigBee identify cluster.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] Cluster     Handle for the cluster.
   @param[in] Event_Data  Information for the cluster event.
   @param[in] CB_Param    User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_Identify_Client_Event_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Identify_Client_Event_Data_t *Event_Data, uint32_t CB_Param);

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for a ZigBee identify cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Information for the cluster event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_Identify_Server_Event_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Identify_Server_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Creates an identify cluster client for an endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function will provide responses to identify
   commands that are issued using this cluster.

   @param[in]  ZB_Handle    Handle of a ZigBee stack.
   @param[out] Cluster      Pointer to where the newly created cluster will
                            be stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and
                            attribute information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for
                            the identify client cluster.
   @param[in]  CB_Param     Callback parameter for the cluster callback
                            function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Identify_Create_Client(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_Identify_Client_Event_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Creates an identify server for an endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function will be called when
   device/endpoint identification starts or stops.

   @param[in]  ZB_Handle    Handle of a ZigBee stack.
   @param[out] Cluster      Pointer to where the newly created cluster will be
                            stored upon successful return.
   @param[in]  Cluster_Info Pointer to the cluster information. Note that the
                            QAPI_ZB_CL_IDENTIFY_ATTRIBUTE_ID_IDENTIFY_TIME
                            attribute is handled internally and does not support
                            custom read/write callbacks.
   @param[in]  Event_CB     Callback function that will handle events for the
                            identify cluster.
   @param[in]  CB_Param     Callback parameter for the identify cluster callback
                            function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Identify_Create_Server(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_Identify_Server_Event_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Populates an attribute list with the attributes required for an
          identify cluster.

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
qapi_Status_t qapi_ZB_CL_Identify_Populate_Attributes(qbool_t Server, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList);

/**
   @brief Sends an Identify command via an identify client.

   @param[in] Cluster      Handle of the client cluster.
   @param[in] SendInfo     Destination information for the command.
   @param[in] IdentifyTime Identify time.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Identify_Send_Identify(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint16_t IdentifyTime);

/**
   @brief Sends an Identify Query command via an identify client.

   @param[in] Cluster  Handle to the client cluster.
   @param[in] SendInfo Destination information for the command.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Identify_Send_Identify_Query(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

#endif // ] #ifndef __QAPI_ZB_CL_IDENTIFY_H__

/** @} */

