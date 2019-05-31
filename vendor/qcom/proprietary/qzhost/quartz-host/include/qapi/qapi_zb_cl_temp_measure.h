/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @addtogroup qapi_zb_cl_temp_measure
@{ */

/** @file qapi_zb_cl_temp_measure.h
   @brief QAPI for the ZigBee temperature measurement cluster.

   This API provides definitions, commands and events related to the ZigBee
   temperature measurement cluster.

   APIs are provided to create a temperature measurement server and client
   cluster. Cluster creation also registers an event callback that is used to
   receive events from the temperature measurement cluster.  Client events for the
   temperature measurement cluster are for command responses and server events
   indicate state changes.
*/

#ifndef __QAPI_ZB_CL_TEMP_MEASURE_H__ // [
#define __QAPI_ZB_CL_TEMP_MEASURE_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_zb_cl.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following definitions represent the attribute IDs for the temperature
   measurement cluster. */
#define QAPI_ZB_CL_TEMPMEASURE_ATTR_ID_MEASURED_VALUE                           (0x0000) /**< Temperature measurement cluster attribute ID for the measured value (read
                                                                                              only, reportable, int16). */
#define QAPI_ZB_CL_TEMPMEASURE_ATTR_ID_MIN_MEASURED_VALUE                       (0x0001) /**< Temperature measurement cluster attribute ID for the min measured value
                                                                                              (read only, int16). */
#define QAPI_ZB_CL_TEMPMEASURE_ATTR_ID_MAX_MEASURED_VALUE                       (0x0002) /**< Temperature measurement cluster attribute ID for the max measured value
                                                                                              (read only, int16). */
#define QAPI_ZB_CL_TEMPMEASURE_ATTR_ID_TOLERANCE                                (0x0003) /**< Temperature measurement cluster attribute ID for the tolerance (read only,
                                                                                              reportable, uint16). */

#define QAPI_ZB_CL_TEMPMEASURE_TOLERANCE_MAX                                    ((uint16_t)(0x0800)) /**< Temperature measurement cluster's maximum tolerance value. */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Enumeration of events that can be received by a temperature measurement
   server cluster.
*/
typedef enum
{
   QAPI_ZB_CL_TEMPMEASURE_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an Attribute Custom Read event. */
   QAPI_ZB_CL_TEMPMEASURE_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an Attribute Custom Write event. */
   QAPI_ZB_CL_TEMPMEASURE_SERVER_EVENT_TYPE_UNPARSED_DATA_E      /**< Indicates an Unparsed Data event. */
} qapi_ZB_CL_TempMeasure_Server_Event_Type_t;

/**
   Structure representing the data for events received by a temperature
   measurement server cluster.
*/
typedef struct qapi_ZB_CL_TempMeasure_Server_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_TempMeasure_Server_Event_Type_t Event_Type;

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
} qapi_ZB_CL_TempMeasure_Server_Event_Data_t;

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for a temperature measurement server cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Information for the cluster event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_TempMeasure_Server_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_TempMeasure_Server_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Creates a temperature measurement cluster server for a specific
          endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function will provide events when state
   change commands are received.

   @param[in]  ZB_Handle    Handle of a ZigBee stack.
   @param[out] Cluster      Pointer to where the newly created cluster is to be
                            stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  Tolerance    Tolerance of the temperature measurements.  This
                            value is used to populate the tolerance attribute.
   @param[in]  Event_CB     Callback function that will handle events for
                            the temperature measurement client cluster.
   @param[in]  CB_Param     Callback parameter for the cluster callback
                            function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_TempMeasure_Create_Server(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, uint16_t Tolerance, qapi_ZB_CL_TempMeasure_Server_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Populates an attribute list with the attributes required for a
          temperature measurement cluster.

   @param[in]     Server         Flag indicating if this function should
                                 populate the required temperature measurement
                                 server attributes (true) or client attributes
                                 (false).
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
qapi_Status_t qapi_ZB_CL_TempMeasure_Populate_Attributes(qbool_t Server, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList);

#endif // ] #ifndef __QAPI_ZB_CL_TEMP_MEASURE_H__

/** @} */

