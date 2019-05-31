/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_cl_ias_zone.h
   @brief QAPI for the ZigBee IAS zone cluster.

   @addtogroup qapi_zb_cl_ias_zone
   @{

   These APIs provide definitions, commands, and events related to the ZigBee
   Intruder Alarm System (IAS) Zone cluster.

   APIs are provided to create IAS zone server and client clusters. Cluster
   creation also registers an event callback that is used to receive events from
   the IAS zone cluster. Client events are for command responses and server
   events indicate state changes.

   @}
*/

#ifndef __QAPI_ZB_CL_IAS_ZONE_H__ // [
#define __QAPI_ZB_CL_IAS_ZONE_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_zb_cl.h"

/** @addtogroup qapi_zb_cl_ias_zone
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following definitions represent the attribute IDs for the IAS Zone
   cluster. */
#define QAPI_ZB_CL_IASZONE_ATTR_ID_ZONE_STATE                                   (0x0000) /**< IAS zone attribute ID for the zone state (read only, enum8). */
#define QAPI_ZB_CL_IASZONE_ATTR_ID_ZONE_TYPE                                    (0x0001) /**< IAS zone attribute ID for the zone type (read only, enum16). */
#define QAPI_ZB_CL_IASZONE_ATTR_ID_ZONE_STATUS                                  (0x0002) /**< IAS zone attribute ID for the zone status (read only, map16). This
                                                                                              attribute is handled internally by the IAS zone server cluster, so it cannot
                                                                                              be overwritten by the application. */
#define QAPI_ZB_CL_IASZONE_ATTR_ID_IAS_CIE_ADDRESS                              (0x0010) /**< IAS zone attribute ID for the IAS control, which indicates the equipment address
                                                                                              (EUI64). */
#define QAPI_ZB_CL_IASZONE_ATTR_ID_ZONE_ID                                      (0x0011) /**< IAS zone attribute ID for the zone ID (read, uint8). */
#define QAPI_ZB_CL_IASZONE_ATTR_ID_SUPPORTED_ZONE_SENSITIVITY_LEVELS            (0x0012) /**< IAS zone attribute ID for the number of supported zone sensitivity levels
                                                                                              (read, uint8). */
#define QAPI_ZB_CL_IASZONE_ATTR_ID_CURRENT_ZONE_SENSITIVITY_LEVEL               (0x0013) /**< IAS zone attribute ID for the current zone sensitivity level (uint8). */

/* The following definitions represent the bit mask associated with the zone
   status attribute. */
#define QAPI_ZB_CL_IASZONE_ZONE_STATUS_ALARM1                                   (0x0001) /**< IAS zone status bitmask indicating that alarm 1 is active. */
#define QAPI_ZB_CL_IASZONE_ZONE_STATUS_ALARM2                                   (0x0002) /**< IAS zone status bitmask indicating that alarm 2 is active. */
#define QAPI_ZB_CL_IASZONE_ZONE_STATUS_TAMPER                                   (0x0004) /**< IAS zone status bitmask indicating a tamper condition. */
#define QAPI_ZB_CL_IASZONE_ZONE_STATUS_BATTERY                                  (0x0008) /**< IAS zone status bitmask indicating a low battery condition. */
#define QAPI_ZB_CL_IASZONE_ZONE_STATUS_SUPERVISION_REPORTS                      (0x0010) /**< IAS zone status bitmask indicating that supervision reports are enabled. */
#define QAPI_ZB_CL_IASZONE_ZONE_STATUS_RESTORE_REPORTS                          (0x0020) /**< IAS zone status bitmask indicating that restore reports are enabled. */
#define QAPI_ZB_CL_IASZONE_ZONE_STATUS_TROUBLE                                  (0x0040) /**< IAS zone status bitmask indicating a trouble condition. */
#define QAPI_ZB_CL_IASZONE_ZONE_STATUS_AC_MAINS                                 (0x0080) /**< IAS zone status bitmask indicating an AC mains fault. */
#define QAPI_ZB_CL_IASZONE_ZONE_STATUS_TEST                                     (0x0100) /**< IAS zone status bitmask indicating that a sensor is in Test mode. */
#define QAPI_ZB_CL_IASZONE_ZONE_STATUS_BATTERY_DEFECT                           (0x0200) /**< IAS zone status bitmask indicating that a battery is defective. */

/* The following definitions represent the command IDs for the IAS zone client
   cluster. These IDs are provided predominately for identifying command
   responses. */
#define QAPI_ZB_CL_IASZONE_CLIENT_COMMAND_ID_ZONE_ENROLL_RESPONSE               (0x00) /**< IAS zone client cluster command ID for the zone enroll response. */
#define QAPI_ZB_CL_IASZONE_CLIENT_COMMAND_ID_INITIATE_NORMAL_OPERATION_MODE     (0x01) /**< IAS zone client cluster command ID for the initiate normal operation mode command. */
#define QAPI_ZB_CL_IASZONE_CLIENT_COMMAND_ID_INITIATE_TEST_MODE                 (0x02) /**< IAS zone client cluster command ID for the initiate test mode command. */

/* The following definitions represent the command IDs for the IAS zone server
   cluster. These IDs are provided predominately for identifying command
   responses. */
#define QAPI_ZB_CL_IASZONE_SERVER_COMMAND_ID_ZONE_STATUS_CHANGE_NOTIFICATION    (0x00) /**< IAS zone server cluster command ID for the zone status change notification. */
#define QAPI_ZB_CL_IASZONE_SERVER_COMMAND_ID_ZONE_ENROLL_REQUEST                (0x01) /**< IAS zone server cluster command ID for the zone enroll request command. */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Enumeration of events that can be received by an IAS zone client cluster.
*/
typedef enum
{
   QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E,               /**< Indicates an attribute custom read event. */
   QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E,              /**< Indicates an attribute custom write event. */
   QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E,                /**< Indicates a default response event. */
   QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E,                /**< Indicates a command complete event. */
   QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E,               /**< Indicates an unparsed response event. */
   QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_ZONE_STATUS_CHANGE_NOTIFICATION_E, /**< Indicates a zone status change notification event. */
   QAPI_ZB_CL_IASZONE_CLIENT_EVENT_TYPE_ZONE_ENROLL_REQUEST                /**< Indicates a zone enroll request event. */
} qapi_ZB_CL_IASZone_Client_Event_Type_t;

/**
   Enumeration of events that can be received by an IAS zone server cluster.
*/
typedef enum
{
   QAPI_ZB_CL_IASZONE_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E,               /**< Indicates an attribute custom read event. */
   QAPI_ZB_CL_IASZONE_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E,              /**< Indicates an attribute custom write event. */
   QAPI_ZB_CL_IASZONE_SERVER_EVENT_TYPE_UNPARSED_DATA_E,                  /**< Indicates an unparsed data event. */
   QAPI_ZB_CL_IASZONE_SERVER_EVENT_TYPE_ZONE_ENROLL_RESPONSE_E,           /**< Indicates a zone enroll response event. */
   QAPI_ZB_CL_IASZONE_SERVER_EVENT_TYPE_INITIATE_NORMAL_OPERATION_MODE_E, /**< Indicates an initiate normal operation mode event. */
   QAPI_ZB_CL_IASZONE_SERVER_EVENT_TYPE_INITIATE_TEST_MODE_E              /**< Indicates an initiate test mode event. */
} qapi_ZB_CL_IASZone_Server_Event_Type_t;

/**
   Enumeration of the values for the Zone State attribute.
*/
typedef enum
{
   QAPI_ZB_CL_IASZONE_ZONE_STATE_NOT_ENROLLED_E = 0x00,   /**< Not enrolled. */
   QAPI_ZB_CL_IASZONE_ZONE_STATE_ENROLLED       = 0x01    /**< Enrolled. */
} qapi_ZB_CL_IASZone_Zone_State_t;

/**
   Enumeration of the values for the Zone Type attribute.
*/
typedef enum
{
   QAPI_ZB_CL_IASZONE_ZONE_TYPE_STANDARD_CIE_E              = 0x0000, /**< Standard CIE. */
   QAPI_ZB_CL_IASZONE_ZONE_TYPE_MOTION_SENSOR_E             = 0x000D, /**< Motion sensor. */
   QAPI_ZB_CL_IASZONE_ZONE_TYPE_CONTACT_SWITCH_E            = 0x0015, /**< Contact switch. */
   QAPI_ZB_CL_IASZONE_ZONE_TYPE_FIRE_SENSOR_E               = 0x0028, /**< Fire sensor. */
   QAPI_ZB_CL_IASZONE_ZONE_TYPE_WATER_SENSOR_E              = 0x002A, /**< Water sensor. */
   QAPI_ZB_CL_IASZONE_ZONE_TYPE_CARBON_MONOXIDE_SENSOR_E    = 0x002B, /**< Carbon monoxide sensor. */
   QAPI_ZB_CL_IASZONE_ZONE_TYPE_PERSONAL_EMERGENCY_DEVICE_E = 0x002C, /**< Personal emergency device. */
   QAPI_ZB_CL_IASZONE_ZONE_TYPE_VIBRATION_MOVEMENT_SENSOR_E = 0x002D, /**< Vibration/movement sensor. */
   QAPI_ZB_CL_IASZONE_ZONE_TYPE_REMOTE_CONTROL_E            = 0x010F, /**< Remote control. */
   QAPI_ZB_CL_IASZONE_ZONE_TYPE_KEY_FOB_E                   = 0x0115, /**< Key fob. */
   QAPI_ZB_CL_IASZONE_ZONE_TYPE_KEY_PAD_E                   = 0x021D, /**< Key pad. */
   QAPI_ZB_CL_IASZONE_ZONE_TYPE_STANDARD_WARNING_DEVICE_E   = 0x0225, /**< Standard warning device. */
   QAPI_ZB_CL_IASZONE_ZONE_TYPE_GLASS_BREAK_SENSOR_E        = 0x0226, /**< Glass break sensor. */
   QAPI_ZB_CL_IASZONE_ZONE_TYPE_SECURITY_REPEATER           = 0x2229, /**< Security repeater. */
   QAPI_ZB_CL_IASZONE_ZONE_TYPE_INVALID_ZONE_TYPE_E         = 0xFFFF  /**< Invalid zone type. */
} qapi_ZB_CL_IASZone_Zone_Type_t;

/**
   Enumeration of the values for the zone enroll response code.
*/
typedef enum
{
   QAPI_ZB_CL_IASZONE_ZONE_ENROLL_RESPONSE_CODE_SUCCESS_E          = 0x00,   /**< Success. */
   QAPI_ZB_CL_IASZONE_ZONE_ENROLL_RESPONSE_CODE_NOT_SUPPORTED_E    = 0x01,   /**< Not supported. */
   QAPI_ZB_CL_IASZONE_ZONE_ENROLL_RESPONSE_CODE_NO_ENROLL_PERMIT_E = 0x02,   /**< No enroll permit. */
   QAPI_ZB_CL_IASZONE_ZONE_ENROLL_RESPONSE_CODE_TOO_MANY_ZONES_E   = 0x03    /**< Too many zones. */
} qapi_ZB_CL_IASZone_Zone_Enroll_Response_Code_t;

/**
   Structure representing the zone enroll response command parameters.
*/
typedef struct qapi_ZB_CL_IASZone_Zone_Enroll_Response_s
{
   /** Response code of the zone enroll response. */
   qapi_ZB_CL_IASZone_Zone_Enroll_Response_Code_t ResponseCode;

   /** Zone ID of the zone enroll response. */
   uint8_t                                        ZoneID;
} qapi_ZB_CL_IASZone_Zone_Enroll_Response_t;

/**
   Structure representing the initiate Test mode command parameters.
*/
typedef struct qapi_ZB_CL_IASZone_Initiate_Test_Mode_s
{
   /** Duration in seconds that the IAS zone server is to operate in
       Test mode. */
   uint8_t TestModeDuration;

   /** Sensitivity level used by the IAS zone server during Test
       mode. */
   uint8_t CurrentZoneSensitivityLevel;
} qapi_ZB_CL_IASZone_Initiate_Test_Mode_t;

/**
   Structure representing the zone status change notification command
   parameters.
*/
typedef struct qapi_ZB_CL_IASZone_Zone_Status_Change_Notification_s
{
   /** Current value of the zone status attribute. */
   uint16_t ZoneStatus;

   /** Reserved for additional status information; should be set to 0. */
   uint8_t  ExtendedStatus;

   /** Index of the zone in the CIE's zone table. */
   uint8_t  ZoneID;

   /** Amount of time, in quarters of a seconds from the moment of zone status
       change to the successful transmission of a zone status change
       notification. */
   uint16_t Delay;
} qapi_ZB_CL_IASZone_Zone_Status_Change_Notification_t;

/**
   Structure representing the zone enroll request command parameters.
*/
typedef struct qapi_ZB_CL_IASZone_Zone_Enroll_Request_s
{
   /** APS data indication. */
   qapi_ZB_APSDE_Data_Indication_t                 APSDEData;

   /** Header of the ZCL frame. */
   qapi_ZB_CL_Header_t                             ZCLHeader;

   /** The extended address of the device sends the zone enroll request. */
   uint64_t                                        SourceAddress;

   /** Current value of the zone type attribute. */
   qapi_ZB_CL_IASZone_Zone_Type_t                  ZoneType;

   /** Manufacturer code held in the node descriptor for the device. */
   uint16_t                                        ManufacturerCode;

   /** Pointer to where the callback is to store the status result for the
       enroll response. */
   qapi_Status_t                                  *StatusResult;

   /** Pointer to where the callback is to store the response code for the
       enroll response. */
   qapi_ZB_CL_IASZone_Zone_Enroll_Response_Code_t *ResponseCode;

   /** Pointer to where the callback is to store the zone ID for the enroll
       response. */
   uint8_t                                        *ZoneID;
} qapi_ZB_CL_IASZone_Zone_Enroll_Request_t;

/**
   Structure representing the data for events received by an IAS zone client
   cluster.
*/
typedef struct qapi_ZB_CL_IASZone_Client_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_IASZone_Client_Event_Type_t Event_Type;

   /** Data for the event. The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for custom read events. */
      qapi_ZB_CL_Attr_Custom_Read_t                        Attr_Custom_Read;

      /** Information for custom write events. */
      qapi_ZB_CL_Attr_Custom_Write_t                       Attr_Custom_Write;

      /** Information for default response events. */
      qapi_ZB_CL_Default_Response_t                        Default_Response;

      /** Information for command complete events. */
      qapi_ZB_CL_Command_Complete_t                        Command_Complete;

      /** Information for unparsed response events. */
      qapi_ZB_CL_Unparsed_Response_t                       Unparsed_Response;

      /** Information for zone status change notifications. */
      qapi_ZB_CL_IASZone_Zone_Status_Change_Notification_t Change_Notification;

      /** Information for zone enroll request events. */
      qapi_ZB_CL_IASZone_Zone_Enroll_Request_t             Enroll_Request;
   } Data;
} qapi_ZB_CL_IASZone_Client_Event_Data_t;

/**
   Structure representing the data for events received by an IAS zone server
   cluster.
*/
typedef struct qapi_ZB_CL_IASZone_Server_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_IASZone_Server_Event_Type_t Event_Type;

   /** Data for the event. The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for custom read events. */
      qapi_ZB_CL_Attr_Custom_Read_t             Attr_Custom_Read;

      /** Information for custom write events. */
      qapi_ZB_CL_Attr_Custom_Write_t            Attr_Custom_Write;

      /** Information for unparsed data events. */
      qapi_ZB_CL_Unparsed_Data_t                Unparsed_Data;

      /** Information for zone enroll response events. */
      qapi_ZB_CL_IASZone_Zone_Enroll_Response_t Enroll_Response;

      /** Information for initiate test mode events. */
      qapi_ZB_CL_IASZone_Initiate_Test_Mode_t   Intiate_Test_Mode;
  } Data;
} qapi_ZB_CL_IASZone_Server_Event_Data_t;

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for an IAS zone client cluster.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] Cluster     Handle for the cluster.
   @param[in] Event_Data  Information for the cluster event.
   @param[in] CB_Param    User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_IASZone_Client_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_IASZone_Client_Event_Data_t *Event_Data, uint32_t CB_Param);

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for an IAS zone server cluster.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] Cluster     Handle for the cluster.
   @param[in] Event_Data  Pointer to information for the cluster event.
   @param[in] CB_Param    User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_IASZone_Server_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_IASZone_Server_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Creates an IAS zone cluster client for a specific endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function will provide responses to IAS zone
   commands that are issued using this cluster.

   @param[in]  ZB_Handle    Handle of a ZigBee stack.
   @param[out] Cluster      Pointer to where the newly created cluster is to be
                            stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for the
                            IAS zone client cluster.
   @param[in]  CB_Param     Callback parameter for the cluster callback
                            function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASZone_Create_Client(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_IASZone_Client_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Creates an IAS zone cluster server for a specific endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function will provide events when state
   change commands are received.

   @param[in]  ZB_Handle    Handle of a ZigBee stack.
   @param[out] Cluster      Pointer to where the newly created cluster is to be
                            stored upon successful return.
   @param[in]  Zone_Type    Zone type of associated with zone cluster.
   @param[in]  Manufacturer Manufacturer code.
   @param[in]  TripToPair   Flag indicating whether the server supports the
                            trip-to-pair and auto-enroll-request methods for
                            enrollment. If set to false, the auto-enroll-
                            response method is supported.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for the
                            IAS zone server cluster.
   @param[in]  CB_Param     Callback parameter for the cluster callback
                            function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASZone_Create_Server(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_IASZone_Zone_Type_t Zone_Type, uint16_t Manufacturer, qbool_t TripToPair, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_IASZone_Server_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Populates an attribute list with the attributes required for an IAS
          zone cluster.

   @param[in]     Server         Flag indicating whether this function should
                                 populate the required IAS zone server
                                 attributes (true) or client attributes (false).
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
qapi_Status_t qapi_ZB_CL_IASZone_Populate_Attributes(qbool_t Server, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList);

/**
   @brief Sends a zone enroll response command to an IAS zone server.

   @param[in] Cluster  Handle for the IAS zone client cluster that is to be
                       used to send the command.
   @param[in] SendInfo General send information for the command packet.
   @param[in] ZoneID   Zone ID.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASZone_Client_Send_Zone_Enroll_Reponse(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint8_t ZoneID);

/**
   @brief Sends an initiate normal operation mode command to an IAS zone server.

   @param[in] Cluster    Handle for the IAS zone client cluster that is to be
                         used to send the command.
   @param[in] SendInfo   General send information for the command packet.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASZone_Client_Send_Initiate_Normal_Operation_Mode(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

/**
   @brief Sends an initiate test mode command to an IAS zone server.

   @param[in] Cluster     Handle for the IAS zone client cluster that is to be
                          used to send the command.
   @param[in] SendInfo    General send information for the command packet.
   @param[in] Duration    Test mode duration in seconds.
   @param[in] Sensitivity The sensitivity level of the zone during test mode.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASZone_Client_Send_Initiate_Test_Mode(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint8_t Duration, uint8_t Sensitivity);

/**
   @brief Sends an enroll request command to an IAS zone client.

   @param[in] Cluster   Handle for the IAS zone server cluster that is to be
                        used to send the command.
   @param[in] SendInfo  General send information for the command packet.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASZone_Server_Send_Zone_Enroll_Request(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo);

/** @} */

#endif // ] #ifndef __QAPI_ZB_CL_IAS_ZONE_H__

