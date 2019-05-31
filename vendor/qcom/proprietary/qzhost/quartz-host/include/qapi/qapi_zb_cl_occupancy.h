/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @addtogroup qapi_zb_cl_occupancy
@{ */

/** @file qapi_zb_cl_occupancy.h
   @brief QAPI for the ZigBee occupancy sensing cluster.

   This API provides definitions, commands and events related to the ZigBee
   occupancy sensing cluster.

   APIs are provided to create a occupancy sensing server and client cluster.
   Cluster creation also registers an event callback that is used to receive
   events from the Occupancy sensing cluster.  Client events for the Occupancy
   sense cluster are for command responses and server events indicate state
   changes.
*/

#ifndef __QAPI_ZB_CL_OCCUPANCY_H__ // [
#define __QAPI_ZB_CL_OCCUPANCY_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_zb_cl.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following definitions represent the attribute IDs for the occupancy
   sensing cluster. */
#define QAPI_ZB_CL_OCCUPANCY_ATTR_ID_OCCUPANCY                                      (0x0000) /**< Occupancy sensing cluster attribute ID for the occupancy (read only,
                                                                                                  reportable, map8). */
#define QAPI_ZB_CL_OCCUPANCY_ATTR_ID_OCCUPANCY_SENSOR_TYPE                          (0x0001) /**< Occupancy sensing cluster attribute ID for the occupancy sensor type (read
                                                                                                  only, enum8). */
#define QAPI_ZB_CL_OCCUPANCY_ATTR_ID_PIR_OCCUPIED_TO_UNOCCUPIED_DELAY               (0x0010) /**< Occupancy sensing cluster attribute ID for the PIR occupied to unoccupied
                                                                                                  delay (uint16). */
#define QAPI_ZB_CL_OCCUPANCY_ATTR_ID_PIR_UNOCCUPIED_TO_OCCUPIED_DELAY               (0x0011) /**< Occupancy sensing cluster attribute ID for the PIR unoccupied to occupied
                                                                                                  delay (uint16). */
#define QAPI_ZB_CL_OCCUPANCY_ATTR_ID_PIR_UNOCCUPIED_TO_OCCUPIED_THRESHOLD           (0x0012) /**< Occupancy sensing cluster attribute ID for the PIR unoccupied to occupied
                                                                                                  threshold (uint8). */
#define QAPI_ZB_CL_OCCUPANCY_ATTR_ID_ULTRASONIC_OCCUPIED_TO_UNOCCUPIED_DELAY        (0x0020) /**< Occupancy sensing cluster attribute ID for the ultrasonic occupied to
                                                                                                  unoccupied delay (uint16). */
#define QAPI_ZB_CL_OCCUPANCY_ATTR_ID_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_DELAY        (0x0021) /**< Occupancy sensing cluster attribute ID for the ultrasonic unoccupied to
                                                                                                  occupied delay (uint16). */
#define QAPI_ZB_CL_OCCUPANCY_ATTR_ID_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_THRESHOLD    (0x0022) /**< Occupancy sensing cluster attribute ID for the ultrasonic unoccupied to
                                                                                                  occupied threshold (uint8). */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/
/**
   Enumeration of events that can be received by a occupancy sensing server
   cluster.
*/
typedef enum
{
   QAPI_ZB_CL_OCCUPANCY_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an Attribute Custom Read event. */
   QAPI_ZB_CL_OCCUPANCY_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an Attribute Custom Write event. */
   QAPI_ZB_CL_OCCUPANCY_SERVER_EVENT_TYPE_UNPARSED_DATA_E      /**< Indicates an Unparsed Data event. */
} qapi_ZB_CL_Occupancy_Server_Event_Type_t;

/**
   Enumeration of occupancy sensor type.
*/
typedef enum
{
   QAPI_ZB_CL_OCCUPANCY_SENSOR_TYPE_PIR_E                = 0x00, /**< PIR. */
   QAPI_ZB_CL_OCCUPANCY_SENSOR_TYPE_ULTRASONIC_E         = 0x01, /**< ultra sonic. */
   QAPI_ZB_CL_OCCUPANCY_SENSOR_TYPE_PIR_AND_ULTRASONIC_E = 0x02, /**< PIR and ultra sonic. */
} qapi_ZB_CL_Occupancy_Fan_Mode_t;

/**
   Structure representing the data for events received by a occupancy sensing
   server cluster.
*/
typedef struct qapi_ZB_CL_Occupancy_Server_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_Occupancy_Server_Event_Type_t Event_Type;

   /** Data for the event.  The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for custom read events. */
      qapi_ZB_CL_Attr_Custom_Read_t           Attr_Custom_Read;

      /** Information for custom write events. */
      qapi_ZB_CL_Attr_Custom_Write_t          Attr_Custom_Write;

      /** Information for unparsed data. */
      qapi_ZB_CL_Unparsed_Data_t              Unparsed_Data;
   } Data;
} qapi_ZB_CL_Occupancy_Server_Event_Data_t;

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for an occupancy sensing server cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Pointer to information for the cluster event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_Occupancy_Server_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Occupancy_Server_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Creates an occupancy sensing cluster server for a specific endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function will provide responses to
   occupancy sensing commands that are issued using this cluster.

   @param[in]  ZB_Handle    Handle of a ZigBee stack.
   @param[out] Cluster      Pointer to where the newly created cluster is to be
                            stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for the
                            occupancy sensing client cluster.
   @param[in]  CB_Param     Callback parameter for the cluster callback
                            function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Occupancy_Create_Server(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_Occupancy_Server_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Populate an attribute list with the attributes required for an
          occupancy sensing cluster.

   @param[in]     Server         Flag indicating if this function should
                                 populate the required On/Off server attributes
                                 (true) or client attributes (false).
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
qapi_Status_t qapi_ZB_CL_Occupancy_Populate_Attributes(qbool_t Server, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList);

#endif // ] #ifndef __QAPI_ZB_CL_OCCUPANCY_H__

/** @} */

