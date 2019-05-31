/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_cl_rel_humid.h
   @brief QAPI for the ZigBee relative humidity measurement cluster.

   @addtogroup qapi_zb_cl_rel_humid
   @{

   This API provides definitions, commands and events related to the ZigBee
   relative humidity measurement cluster.

   APIs are provided to create a relative humidity measurement server cluster.
   Cluster creation also registers an event callback that is used to receive
   events from the relative humidity measurement cluster.

   As this cluster doesn't generate any commands, the client cluster can be
   created with qapi_ZB_CL_Create_Cluster().

   Note that in the ZCL R7 specification, the relative humidity cluster was
   renamed to water content.
*/

#ifndef __QAPI_ZB_CL_REL_HUMID_H__ // [
#define __QAPI_ZB_CL_REL_HUMID_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_zb_cl.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following definitions represent the attribute IDs for the relative
   humidity cluster. */
#define QAPI_ZB_CL_REL_HUMID_ATTR_ID_MEASURED_VALUE                             (0x0000) /**< Relative humidity measurement cluster attribute ID for the measured value
                                                                                              (read only, uint16). */
#define QAPI_ZB_CL_REL_HUMID_ATTR_ID_MIN_MEASURED_VALUE                         (0x0001) /**< Relative humidity measurement cluster attribute ID for the minimum measured
                                                                                              value (read only, uint16). */
#define QAPI_ZB_CL_REL_HUMID_ATTR_ID_MAX_MEASURED_VALUE                         (0x0002) /**< Relative humidity measurement cluster attribute ID for the maximum measured
                                                                                              value (read only, uint16). */
#define QAPI_ZB_CL_REL_HUMID_ATTR_ID_TOLERANCE                                  (0x0003) /**< Relative humidity measurement cluster attribute ID for the tolerance (read
                                                                                              only, uint16). */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/
/**
   Enumeration of events that can be received by a relative humidity
   measurement cluster.
*/
typedef enum
{
   QAPI_ZB_CL_REL_HUMID_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an attribute custom read event. */
   QAPI_ZB_CL_REL_HUMID_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an attribute custom write event. */
   QAPI_ZB_CL_REL_HUMID_SERVER_EVENT_TYPE_UNPARSED_DATA_E,     /**< Indicates an unparsed data event. */
} qapi_ZB_CL_RelHumid_Server_Event_Type_t;

/**
   Structure representing the data for events received by a relative humidity
   measurement server cluster.
*/
typedef struct qapi_ZB_CL_RelHumid_Server_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_RelHumid_Server_Event_Type_t Event_Type;

   /** Data for the event.  The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for custom read events. */
      qapi_ZB_CL_Attr_Custom_Read_t  Attr_Custom_Read;

      /** Information for custom write events. */
      qapi_ZB_CL_Attr_Custom_Write_t Attr_Custom_Write;

      /** Information for unparsed data. */
      qapi_ZB_CL_Unparsed_Data_t     Unparsed_Data;
   } Data;
} qapi_ZB_CL_RelHumid_Server_Event_Data_t;

/**
   @brief This function definition represents a callback function that handles
          asynchronous events for a relative humidity measurement server
          cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Information for the cluster event.
   @param[in] CB_Param   User specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_RelHumid_Server_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_RelHumid_Server_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Creates a illuminance measurement cluster server for a specific
          endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   @param[in]  ZB_Handle    Handle of the ZigBee instance.
   @param[out] Cluster      Pointer to where the newly created cluster
                            will be stored upon successful return.
   @param[in]  Tolerance    Tolerance of the measured value.
   @param[in]  Cluster_Info Structure containing the endpoint and
                            attribut information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events
                            for the illuminance measurement client
                            cluster.
   @param[in]  CB_Param     Callback parameter for the callback function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_RelHumid_Create_Server(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, uint16_t Tolerance, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_RelHumid_Server_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Used to populate an attribute list with the attributes required for a
          relative humidity measurement cluster.

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
qapi_Status_t qapi_ZB_CL_RelHumid_Populate_Attributes(qbool_t Server, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList);

#endif // ] #ifndef __QAPI_ZB_CL_REL_HUMID_H__

/** @} */

