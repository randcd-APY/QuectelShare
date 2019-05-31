/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @addtogroup qapi_zb_cl_fan_control
@{ */

/** @file qapi_zb_cl_fan_control.h
   @brief QAPI for the ZigBee fan control cluster.

   This API provides definitions, commands and events related to the ZigBee
   fan control cluster.

   APIs are provided to create a fan control server cluster. Cluster creation
   also registers an event callback that is used to receive events from the fan
   control cluster.

   As this cluster doesn't generate any commands, the client cluster can be
   created with qapi_ZB_CL_Create_Cluster().
*/

#ifndef __QAPI_ZB_CL_FAN_CONTROL_H__ // [
#define __QAPI_ZB_CL_FAN_CONTROL_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_zb_cl.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following definitions represent the fan control attribute for the fan
   control cluster. */
#define QAPI_ZB_CL_FANCONTROL_ATTR_ID_FAN_MODE                                 (0x0000) /**< Fan control cluster attribute ID for the fan mode (enum8). */
#define QAPI_ZB_CL_FANCONTROL_ATTR_ID_FAN_MODE_SEQUENCE                        (0x0001) /**< Fan control cluster attribute ID for the fan mode sequence (enum8). */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/
/**
   Enumeration of events that can be received by a Fan control server
   cluster.
*/
typedef enum
{
   QAPI_ZB_CL_FANCONTROL_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an Attribute Custom Read event. */
   QAPI_ZB_CL_FANCONTROL_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an Attribute Custom Write event. */
   QAPI_ZB_CL_FANCONTROL_SERVER_EVENT_TYPE_UNPARSED_DATA_E,     /**< Indicates an Unparsed Data event. */
} qapi_ZB_CL_FanControl_Server_Event_Type_t;

/**
   Enumeration of fan mode associated with the fan mode attribute of fan control
   cluster.
*/
typedef enum
{
   QAPI_ZB_CL_FANCONTROL_FAN_MODE_OFF_E,    /**< Off. */
   QAPI_ZB_CL_FANCONTROL_FAN_MODE_LOW_E,    /**< Low. */
   QAPI_ZB_CL_FANCONTROL_FAN_MODE_MEDIUM_E, /**< Medium. */
   QAPI_ZB_CL_FANCONTROL_FAN_MODE_HIGH_E,   /**< High. */
   QAPI_ZB_CL_FANCONTROL_FAN_MODE_ON_E,     /**< On. */
   QAPI_ZB_CL_FANCONTROL_FAN_MODE_AUTO_E,   /**< Auto (fan speed is self-regulated). */
   QAPI_ZB_CL_FANCONTROL_FAN_MODE_SMART_E   /**< Smart (Fan is on when space is occupied). */
} qapi_ZB_CL_FanControl_Fan_Mode_t;

/**
   Enumeration of system mode associated with Fan control cluster.
*/
typedef enum
{
   QAPI_ZB_CL_FANCONTROL_FAN_MODE_SEQUENCE_LOW_MED_HIGH_E,      /**< Low/Medium/High. */
   QAPI_ZB_CL_FANCONTROL_FAN_MODE_SEQUENCE_LOW_HIGH_E,          /**< Low/High. */
   QAPI_ZB_CL_FANCONTROL_FAN_MODE_SEQUENCE_LOW_MED_HIGH_AUTO_E, /**< Low/Medium/High/Auto. */
   QAPI_ZB_CL_FANCONTROL_FAN_MODE_SEQUENCE_LOW_HIGH_AUTO_E,     /**< Low/High/Auto. */
   QAPI_ZB_CL_FANCONTROL_FAN_MODE_SEQUENCE_ON_AUTO_E            /**< On/Auto. */
} qapi_ZB_CL_FanControl_Fan_Mode_Sequence_t;

/**
   Structure representing the data for events received by a fan control server
   cluster.
*/
typedef struct qapi_ZB_CL_FanControl_Server_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_FanControl_Server_Event_Type_t Event_Type;

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
} qapi_ZB_CL_FanControl_Server_Event_Data_t;

/**
   @brief This function definition represents a callback function which handles
          asynchronous events for a temperature configuration server cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Information for the cluster event.
   @param[in] CB_Param   User specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_FanControl_Server_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_FanControl_Server_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Creates a fan control cluster server for a specific endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function will provide events when state
   change commands are received.

   @param[in]  ZB_Handle    Handle of a ZigBee stack.
   @param[out] Cluster      Pointer to where the newly created cluster is to be
                            stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for the
                            fan control client cluster.
   @param[in]  CB_Param     Callback parameter for the cluster callback
                            function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_FanControl_Create_Server(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_FanControl_Server_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Populate an attribute list with the attributes required for a fan
          control cluster.

   @param[in]     Server         Flag indicating if this function should
                                 populate the required fan control server
                                 attributes (true) or client attributes (false).
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
qapi_Status_t qapi_ZB_CL_FanControl_Populate_Attributes(qbool_t Server, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList);

#endif // ] #ifndef __QAPI_ZB_CL_FAN_CONTROL_H__

/** @} */

