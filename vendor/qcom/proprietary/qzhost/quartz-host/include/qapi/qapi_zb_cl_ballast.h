/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_cl_ballast.h
   @brief QAPI for the ZigBee ballast configuration cluster.

   @addtogroup qapi_zb_cl_ballast
   @{

   This API provides definitions, commands, and events related to the ZigBee
   ballast configuration cluster.

   APIs are provided to create a ballast configuration server cluster. Cluster
   creation also registers an event callback that is used to receive events from
   the ballast configuration cluster.

   As this cluster does not generate any commands, the client cluster can be
   created with qapi_ZB_CL_Create_Cluster().

   @}
*/

#ifndef __QAPI_ZB_CL_BALLAST_H__ // [
#define __QAPI_ZB_CL_BALLAST_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_zb_cl.h"

/** @addtogroup qapi_zb_cl_ballast
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following definitions represent the attribute IDs for the ballast
   configuration cluster. */
#define QAPI_ZB_CL_BALLAST_ATTR_ID_PHYSICAL_MIN_LEVEL                           (0x0000) /**< Ballast configuration cluster attribute ID for the minimum physical level
                                                                                              (read only, uint8). */
#define QAPI_ZB_CL_BALLAST_ATTR_ID_PHYSICAL_MAX_LEVEL                           (0x0001) /**< Ballast configuration cluster attribute ID for the maximum physical level
                                                                                              (read only, uint8). */
#define QAPI_ZB_CL_BALLAST_ATTR_ID_BALLAST_STATUS                               (0x0002) /**< Ballast configuration cluster attribute ID for the ballast status (read
                                                                                              only, map8). */
#define QAPI_ZB_CL_BALLAST_ATTR_ID_MIN_LEVEL                                    (0x0010) /**< Ballast configuration cluster attribute ID for the minimum level (uint8). */
#define QAPI_ZB_CL_BALLAST_ATTR_ID_MAX_LEVEL                                    (0x0011) /**< Ballast configuration cluster attribute ID for the maximum level (uint8). */
#define QAPI_ZB_CL_BALLAST_ATTR_ID_POWER_ON_LEVEL                               (0x0012) /**< Ballast configuration cluster attribute ID for the power on level (uint8). */
#define QAPI_ZB_CL_BALLAST_ATTR_ID_POWER_ON_FADE_TIME                           (0x0013) /**< Ballast configuration cluster attribute ID for the power on fade time
                                                                                              (uint16). */
#define QAPI_ZB_CL_BALLAST_ATTR_ID_INTRINSIC_BALLAST_FACTOR                     (0x0014) /**< Ballast configuration cluster attribute ID for the intrinsic ballast factor
                                                                                              (uint8). */
#define QAPI_ZB_CL_BALLAST_ATTR_ID_BALLAST_FACTOR_ADJUSTMENT                    (0x0015) /**< Ballast configuration cluster attribute ID for the ballast factor
                                                                                              adjustment (uint8). */
#define QAPI_ZB_CL_BALLAST_ATTR_ID_LAMP_QUANTITY                                (0x0020) /**< Ballast configuration cluster attribute ID for the lamp quantity (read
                                                                                              only, uint8). */
#define QAPI_ZB_CL_BALLAST_ATTR_ID_LAMP_TYPE                                    (0x0030) /**< Ballast configuration cluster attribute ID for the lamp type (16 byte
                                                                                              string). */
#define QAPI_ZB_CL_BALLAST_ATTR_ID_LAMP_MANUFACTURER                            (0x0031) /**< Ballast configuration cluster attribute ID for the lamp manufacturer
                                                                                              (16 byte string). */
#define QAPI_ZB_CL_BALLAST_ATTR_ID_LAMP_RATED_HOURS                             (0x0032) /**< Ballast configuration cluster attribute ID for the lamp's rated hours
                                                                                              (read/write, uint24). */
#define QAPI_ZB_CL_BALLAST_ATTR_ID_LAMP_BURN_HOURS                              (0x0033) /**< Ballast configuration cluster attribute ID for the lamp's burn hours
                                                                                              (read/write, uint24). This attribute's configuration cannot be overwritten
                                                                                              by the application.*/
#define QAPI_ZB_CL_BALLAST_ATTR_ID_LAMP_ALARM_MODE                              (0x0034) /**< Ballast configuration cluster attribute ID for the alarm mode (read/write,
                                                                                              map8). This attribute's configuration cannot be overwritten by the
                                                                                              application. */
#define QAPI_ZB_CL_BALLAST_ATTR_ID_LAMP_BURN_HOURS_TRIP_POINT                   (0x0035) /**< Ballast configuration cluster attribute ID for the burn hours trip point
                                                                                              (read/write, uint24). This attribute's configuration cannot be overwritten
                                                                                              by the application. */

/* The following represent the lamp alarm mode mask. */
#define QAPI_ZB_CL_BALLAST_LAMP_ALARM_MODE_MASK_LAMP_BURN_HOURS                 (0x0001) /**< Ballast configuration cluster lamp alarm mode mask for lamp burn hours. */

/* The following definitions represent the bit mask values for the ballast
   status attribute. */
#define QAPI_ZB_CL_BALLAST_STATUS_NON_OPERATIONAL                               (0x01) /**< Ballast configuration cluster status bit mask indicating the ballast is not
                                                                                            operational. */
#define QAPI_ZB_CL_BALLAST_STATUS_LAMP_NOT_IN_SOCKET                            (0x02) /**< Ballast configuration cluster status bit mask indicating there isn't a lamp
                                                                                            in the socket. */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Enumeration of events that can be received by a ballast configuration server
   cluster.
*/
typedef enum
{
   QAPI_ZB_CL_BALLAST_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an attribute custom read event. */
   QAPI_ZB_CL_BALLAST_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an attribute custom write event. */
   QAPI_ZB_CL_BALLAST_SERVER_EVENT_TYPE_UNPARSED_DATA_E,     /**< Indicates an unparsed data event. */
} qapi_ZB_CL_Ballast_Server_Event_Type_t;

/**
   Structure representing the data for events received by a ballast
   configuration server cluster.
*/
typedef struct qapi_ZB_CL_Ballast_Server_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_Ballast_Server_Event_Type_t Event_Type;

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
} qapi_ZB_CL_Ballast_Server_Event_Data_t;

/**
   @brief This function definition represents a callback function that handles
          asynchronous events for a ballast configuration server cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Information for the cluster event.
   @param[in] CB_Param   User specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_Ballast_Server_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Ballast_Server_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Creates a ballast configuration cluster server for a specific
          endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   @param[in]  ZB_Handle    Handle of the ZigBee instance.
   @param[out] Cluster      Pointer to where the newly created cluster will be
                            stored upon successful return.
   @param[in]  PhysicalMin  Initial value for the PhysicalMinLevel attribute.
   @param[in]  PhysicalMax  Initial value for the PhysicalMaxLevel attribute.
   @param[in]  MaxFactorAdj Maximum allowed value for the
                            BallastFactorAdjustment attribute.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for the
                            ballast configuration server cluster.
   @param[in]  CB_Param     Callback parameter for the callback function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Ballast_Create_Server(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, uint8_t PhysicalMin, uint8_t PhysicalMax, uint8_t MaxFactorAdj, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_Ballast_Server_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Populates an attribute list with the attributes required for a 
          ballast configuration cluster.

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
qapi_Status_t qapi_ZB_CL_Ballast_Populate_Attributes(qbool_t Server, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList);

#endif // ] #ifndef __QAPI_ZB_CL_BALLAST_H__

/** @} */

