/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_cl_device_temp.h
   @brief QAPI for the ZigBee device temperature configuration cluster.

   @addtogroup qapi_zb_cl_device_temp
   @{

   This API provides definitions, commands, and events related to the ZigBee
   device temperature configuration cluster.

   APIs are provided to create a device temperature configuration server and
   client cluster.  Cluster creation also registers an event callback that is
   used to receive events from the device temperature configuration cluster.
   Since this cluster does not have any commands, these callbacks are only used
   for custom attribute management or application-specific commands.

   @}
*/

#ifndef __QAPI_ZB_CL_DEVICE_TEMP_H__ // [
#define __QAPI_ZB_CL_DEVICE_TEMP_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_zb_cl.h"

/** @addtogroup qapi_zb_cl_device_temp
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following definitions represent the attribute IDs for the device temperature cluster.
*/
#define QAPI_ZB_CL_DEVICETEMP_ATTR_ID_CURRENT_TEMP                (0x0000) /**< Device temperature cluster attribute ID for the current temperature (read
                                                                                only, int16), which specifies the current internal temperature. This
                                                                                attribute's configuration cannot be overwritten by the application. */
#define QAPI_ZB_CL_DEVICETEMP_ATTR_ID_MIN_TEMP_EXPERIENCED        (0x0001) /**< Device temperature cluster attribute ID for the minimum temperature
                                                                                experienced (read only, int16), which specifies the minimum internal
                                                                                temperature. */
#define QAPI_ZB_CL_DEVICETEMP_ATTR_ID_MAX_TEMP_EXPERIENCED        (0x0002) /**< Device temperature cluster attribute ID for the maximum temperature
                                                                                experienced (read only, int16), which specifies the maximum internal
                                                                                temperature. */
#define QAPI_ZB_CL_DEVICETEMP_ATTR_ID_OVER_TEMP_TOTAL_DWELL       (0x0003) /**< Device temperature cluster attribute ID for the over temperature total
                                                                                dwell (read only, uint16), which specifies the length of time, in hours,
                                                                                the device has spent above the temperature specified by the
                                                                                HighTempThreshold. */
#define QAPI_ZB_CL_DEVICETEMP_ATTR_ID_DEVICE_TEMP_ALARM_MASK      (0x0010) /**< Device temperature cluster attribute ID for the device temperature alarm
                                                                                mask (read write, map8), which specifies which alarms may be generated. */
#define QAPI_ZB_CL_DEVICETEMP_ATTR_ID_LOW_TEMP_THRESHOLD          (0x0011) /**< Device temperature cluster attribute ID for the low temperature threshold
                                                                                (read write, int16), which specifies the lower alarm threshold, measured in
                                                                                degrees Celsius. */
#define QAPI_ZB_CL_DEVICETEMP_ATTR_ID_HIGH_TEMP_THRESHOLD         (0x0012) /**< Device temperature cluster attribute ID for the high temperature threshold
                                                                                (read write, int16), which specifies the upper alarm threshold, measured in
                                                                                degrees Celsius. */
#define QAPI_ZB_CL_DEVICETEMP_ATTR_ID_LOW_TEMP_DWELL_TRIP_POINT   (0x0013) /**< Device temperature cluster attribute ID for the low temperature dwell trip
                                                                                point (read write, uint24), which specifies the length of time, in seconds,
                                                                                that the value of CurrentTemperature may exist below LowTempThreshold
                                                                                before an alarm is generated. */
#define QAPI_ZB_CL_DEVICETEMP_ATTR_ID_HIGH_TEMP_DWELL_TRIP_POINT  (0x0014) /**< Device temperature cluster attribute ID for the high temperature dwell trip
                                                                                point (read write, uint24), which specifies the length of time, in seconds,
                                                                                that the value of CurrentTemperature may exist above HighTempThreshold
                                                                                before an alarm is generated. */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Enumeration of events that can be received by a device temperature server cluster.
*/
typedef enum
{
   QAPI_ZB_CL_DEVICETEMP_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an Attribute Custom Read event. */
   QAPI_ZB_CL_DEVICETEMP_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an Attribute Custom Write event. */
   QAPI_ZB_CL_DEVICETEMP_SERVER_EVENT_TYPE_UNPARSED_DATA_E      /**< Indicates an Unparsed Data event. */
} qapi_ZB_CL_DeviceTemp_Server_Event_Type_t;

/**
   Structure representing the data for events received by a device temperature server
   cluster.
*/
typedef struct qapi_ZB_CL_DeviceTemp_Server_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_DeviceTemp_Server_Event_Type_t Event_Type;

   /** Data for the event.  The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for custom read events. */
      qapi_ZB_CL_Attr_Custom_Read_t           Attr_Custom_Read;

      /** Information for custom write events. */
      qapi_ZB_CL_Attr_Custom_Write_t          Attr_Custom_Write;

      /** Information for unparsed data events. */
      qapi_ZB_CL_Unparsed_Data_t              Unparsed_Data;
   } Data;
} qapi_ZB_CL_DeviceTemp_Server_Event_Data_t;

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for a device temperature server cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Structure containing information on the event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_DeviceTemp_Server_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_DeviceTemp_Server_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Creates a device temperature server cluster for a specific endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   @param[in]  ZB_Handle    Handle of a ZigBee stack.
   @param[out] Cluster      Pointer to where the newly created cluster will be
                            stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for the
                            device temp server cluster.
   @param[in]  CB_Param     Callback parameter for the callback function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_DeviceTemp_Create_Server(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_DeviceTemp_Server_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Populates an attribute list with the attributes required for a device
          temperature cluster.

   @param[in,out] AttributeCount Pointer to the number of attributes in the
                                 attribute list. The initial value of this
                                 parameter should be set to the actual size of
                                 AttributeList. Upon successful return (or a
                                 QAPI_ERR_BOUNDS error), this value represents
                                 the number of attributes that were (or would
                                 be) populated into AttributeList.
   @param[out]    AttributeList  Pointer to the attribute list to be populated.

   @return
     - QAPI_OK          if the request executed successfully.
     - QAPI_ERR_BOUNDS  if the attribute list provided was not large enough.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_DeviceTemp_Populate_Attributes(uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList);

#endif // ] #ifndef __QAPI_ZB_CL_DEVICETEMP_H__

/** @} */

