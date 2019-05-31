/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_cl_illuminance.h
   @brief QAPI for the ZigBee illuminance measurement cluster.

   @addtogroup qapi_zb_cl_illuminance
   @{

   This API provides definitions, commands and events related to the ZigBee
   illuminance measurement cluster.

   APIs are provided to create a illuminance measurement server cluster. Cluster
   creation also registers an event callback that is used to receive events from
   the illuminance measurement cluster.

   As this cluster doesn't generate any commands, the client cluster can be
   created with qapi_ZB_CL_Create_Cluster().
*/

#ifndef __QAPI_ZB_CL_ILLUMINANCE_H__ // [
#define __QAPI_ZB_CL_ILLUMINANCE_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_zb_cl.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following definitions represent the attribute IDs for the illuminance
   measurement cluster. */
#define QAPI_ZB_CL_ILLUMINANCE_ATTR_ID_MEASURED_VALUE                           (0x0000) /**< Illuminance measurement cluster attribute ID for the measured value (read
                                                                                              only, uint16). */
#define QAPI_ZB_CL_ILLUMINANCE_ATTR_ID_MIN_MEASURED_VALUE                       (0x0001) /**< Illuminance measurement cluster attribute ID for the minimum measured value
                                                                                              (read only, uint16). */
#define QAPI_ZB_CL_ILLUMINANCE_ATTR_ID_MAX_MEASURED_VALUE                       (0x0002) /**< Illuminance measurement cluster attribute ID for the maximum measured value
                                                                                              (read only, uint16). */
#define QAPI_ZB_CL_ILLUMINANCE_ATTR_ID_TOLERANCE                                (0x0003) /**< Illuminance measurement cluster attribute ID for the tolerance (read only,
                                                                                              uint16). */
#define QAPI_ZB_CL_ILLUMINANCE_ATTR_ID_LIGHT_SENSOR_TYPE                        (0x0004) /**< Illuminance measurement cluster attribute ID for the light sensor type
                                                                                              (read only, enum8). */

/* The following definitions represent the valid values for light sensor types. */
#define QAPI_ZB_CL_ILLUMINANCE_LIGHT_SENSOR_TYPE_PHOTODIODE                     (0x00) /**< Illuminance measurement cluster light sensor type indicating a photodiode. */
#define QAPI_ZB_CL_ILLUMINANCE_LIGHT_SENSOR_TYPE_CMOS                           (0x01) /**< Illuminance measurement cluster light sensor type indicating a CMOS sensor. */
#define QAPI_ZB_CL_ILLUMINANCE_LIGHT_SENSOR_TYPE_MANUFACTURER_START             (0x40) /**< Illuminance measurement cluster light sensor type value for the start value
                                                                                            of manufacturer specific sensor types. */
#define QAPI_ZB_CL_ILLUMINANCE_LIGHT_SENSOR_TYPE_MANUFACTURER_END               (0xFE) /**< Illuminance measurement cluster light sensor type value for the end value
                                                                                            of manufacturer specific sensor types. */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Enumeration of events that can be received by an illuminance measurement
   server cluster.
*/
typedef enum
{
   QAPI_ZB_CL_ILLUMINANCE_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an attribute custom read event. */
   QAPI_ZB_CL_ILLUMINANCE_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an attribute custom write event. */
   QAPI_ZB_CL_ILLUMINANCE_SERVER_EVENT_TYPE_UNPARSED_DATA_E,     /**< Indicates an unparsed data event. */
} qapi_ZB_CL_Illuminance_Server_Event_Type_t;

/**
   Structure representing the data for events received by a illuminance
   measurement server cluster.
*/
typedef struct qapi_ZB_CL_Illuminance_Server_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_Illuminance_Server_Event_Type_t Event_Type;

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
} qapi_ZB_CL_Illuminance_Server_Event_Data_t;

/**
   @brief This function definition represents a callback function that handles
          asynchronous events for a illuminance measurement server cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Information for the cluster event.
   @param[in] CB_Param   User specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_Illuminance_Server_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Illuminance_Server_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Creates a illuminance measurement cluster server for a specific
          endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   @param[in]  ZB_Handle    Handle of the ZigBee instance.
   @param[out] Cluster      Pointer to where the newly created cluster will be
                            stored upon successful return.
   @param[in]  Tolerance    Tolerance associated the measured value.
   @param[in]  SensorType   Type of sensor.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for the
                            illuminance measurement client cluster.
   @param[in]  CB_Param     Callback parameter for the callback function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Illuminance_Create_Server(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, uint16_t Tolerance, uint8_t SensorType, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_Illuminance_Server_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Used to populate an attribute list with the attributes required for an
          illuminance measurement cluster.

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
qapi_Status_t qapi_ZB_CL_Illuminance_Populate_Attributes(qbool_t Server, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList);

#endif // ] #ifndef __QAPI_ZB_CL_ILLUMINANCE_H__

/** @} */

