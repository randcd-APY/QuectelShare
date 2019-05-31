/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_cl_touchlink.h
   @brief QAPI for the ZigBee Touchlink commissioning cluster.

   @addtogroup qapi_zb_cl_touchlink
   @{

   This API provides definitions, commands, and events related to the ZigBee
   Touchlink commission cluster.

   APIs are provided to create a Touchlink commission server and client cluster.
   Cluster creation also registers an event callback that is used to receive
   events from the Touchlink commission cluster.

   @}
*/

#ifndef __QAPI_ZB_CL_TOUCHLINK_H__ // [
#define __QAPI_ZB_CL_TOUCHLINK_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_zb_cl.h"

/** @addtogroup qapi_zb_cl_touchlink
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Enumeration of the Touchlink device types.
*/
typedef enum
{
   QAPI_ZB_CL_TOUCHLINK_DEVICE_TYPE_COORDINATOR_E,                /**< Coordinator (not valid for most Touchlink use cases). */
   QAPI_ZB_CL_TOUCHLINK_DEVICE_TYPE_ROUTER_E,                     /**< Router device. */
   QAPI_ZB_CL_TOUCHLINK_DEVICE_TYPE_RX_ON_WHEN_IDLE_END_DEVICE_E, /**< Non-sleepy end device. */
   QAPI_ZB_CL_TOUCHLINK_DEVICE_TYPE_SLEEPY_END_DEVICE_E           /**< Sleepy end device. */
} qapi_ZB_CL_Touchlink_Device_Type_t;

/**
   Enumeration of events that can be received by a Touchlink client cluster.
*/
typedef enum
{
   QAPI_ZB_CL_TOUCHLINK_CLIENT_EVENT_TYPE_START_RESULT_E /**< Touchlink client start result event. */
} qapi_ZB_CL_Touchlink_Client_Event_Type_t;

/**
   Enumeration of events that can be received by a Touchlink server cluster.
*/
typedef enum
{
   QAPI_ZB_CL_TOUCHLINK_SERVER_EVENT_TYPE_START_RESULT_E /**< Touchlink server start result event. */
} qapi_ZB_CL_Touchlink_Server_Event_Type_t;

/**
   Structure that represents the Touchlink client event.
*/
typedef struct qapi_ZB_CL_Touchlink_Client_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_Touchlink_Client_Event_Type_t Event_Type;

   /** Data for the event. The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for start result events. */
      qapi_Status_t StartResult;
   } Data;
} qapi_ZB_CL_Touchlink_Client_Event_Data_t;

/**
   Structure that represents the Touchlink server event.
*/
typedef struct qapi_ZB_CL_Touchlink_Server_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_Touchlink_Server_Event_Type_t Event_Type;

   /** Data for the event. The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for start result events. */
      qapi_Status_t StartResult;
   } Data;
} qapi_ZB_CL_Touchlink_Server_Event_Data_t;

/**
   @brief Definition for a function that handles event callbacks for a Touchlink
          client cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle of the cluster.
   @param[in] Event_Data Information for the cluster event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_Touchlink_Client_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Touchlink_Client_Event_Data_t *Event_Data, uint32_t CB_Param);

/**
   @brief Definition for a function that handles event callbacks for a Touchlink
          server cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle of the cluster.
   @param[in] Event_Data Information for the cluster event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_Touchlink_Server_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Touchlink_Server_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Creates a Touchlink client cluster for an endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function will provide responses to
   Touchlink commands that are issued using this cluster.

   @param[in]  ZB_Handle Handle of a ZigBee stack.
   @param[out] Cluster   Pointer to where the newly created cluster will be
                         stored upon successful return.
   @param[in]  Endpoint  Endpoint the cluster will be a part of.
   @param[in]  Event_CB  Callback function that will handle events for the
                         Touchlink client cluster.
   @param[in]  CB_Param  Callback parameter for the cluster callback function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Touchlink_Create_Client(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, uint8_t Endpoint, qapi_ZB_CL_Touchlink_Client_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Creates a Touchlink server cluster for an endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function will provide events when state
   change commands are received.

   @param[in]  ZB_Handle       Handle of a ZigBee stack.
   @param[out] Cluster         Pointer to where the newly created cluster is to
                               be stored upon successful return.
   @param[in]  Endpoint        Endpoint the cluster will be a part of.
   @param[in]  IdentifyCluster Handle for the identify server cluster to be
                               associated with the touchlink server being
                               created.
   @param[in]  Event_CB        Callback function that will handle events for the
                               Touchlink server cluster.
   @param[in]  CB_Param        Callback parameter for the cluster callback
                               function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Touchlink_Create_Server(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, uint8_t Endpoint, qapi_ZB_Cluster_t IdentifyCluster, qapi_ZB_CL_Touchlink_Server_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Gets the Touchlink server or client cluster ready for the
          commissioning process.

   This command is all that is needed for the server cluster. For the client
   cluster however, qapi_ZB_CL_Touchlink_Scan_Request() should be called to kick
   off the commissioning process.

   @param[in] Cluster       Handle of the Touchlink cluster.
   @param[in] DeviceType    Type of the device to start as.
   @param[in] PersistData   Persistent data as provided by a previous call to
                            qapi_ZB_Get_Persistent_Data().  This can be set to
                            NULL if persistent data is not being used to start
                            Touchlink and the PersistLength is zero.
   @param[in] PersistLength Length of the persistent data.  This can be set to
                            NULL if persistent data is not being used to start
                            Touchlink.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Touchlink_Start(qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Touchlink_Device_Type_t DeviceType, const uint8_t *PersistData, uint32_t PersistLength);

/**
   @brief Sends a Touchlink scan request and starts off the commissioning
          process for a client cluster.

   @param[out] ClientCluster Handle for the Touchlink client cluster.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Touchlink_Scan_Request(qapi_ZB_Cluster_t ClientCluster);

/**
   @brief Performs a factory reset of a client cluster and restarts the
          commissioning process.

   @param[out] ClientCluster Handle of the Touchlink client cluster.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.

*/
qapi_Status_t qapi_ZB_CL_Touchlink_Factory_Reset(qapi_ZB_Cluster_t ClientCluster);

#endif // ] #ifndef __QAPI_ZB_CL_Touchlink_H__

/** @} */

