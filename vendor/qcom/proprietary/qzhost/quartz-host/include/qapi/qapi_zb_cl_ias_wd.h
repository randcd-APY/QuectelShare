/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_cl_ias_wd.h
   @brief QAPI for the ZigBee IAS Warning Device (WD) cluster.

   @addtogroup qapi_zb_cl_ias_wd
   @{

   These APIs provide definitions, commands, and events related to the ZigBee
   Intruder Alarm System (IAS) WD cluster.

   APIs are provided to create IAS WD server and client clusters. Cluster
   creation also registers an event callback that is used to receive events from
   the IAS WD cluster. Client events are for command responses and server
   events indicate state changes.

   @}
*/

#ifndef __QAPI_ZB_CL_IASWD_H__ // [
#define __QAPI_ZB_CL_IASWD_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_zb_cl.h"
#include "qapi_zb_cl_ias_zone.h"

/** @addtogroup qapi_zb_cl_ias_wd
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following definitions represent attributes defined by IAS WD cluster. */
#define QAPI_ZB_CL_IASWD_ATTR_ID_MAX_DURATION                                   (0x0000) /**< IAS WD attribute ID for the maximum duration (uint16). */

/* The following defintions represent the command IDs for the IAS WD client
   cluster. These IDs are provided predominately for identifying command
   responses. */
#define QAPI_ZB_CL_IASWD_CLIENT_COMMAND_ID_START_WARNING                        (0x00) /**< IAS WD client cluster command ID for the Start Warning. */
#define QAPI_ZB_CL_IASWD_CLIENT_COMMAND_ID_SQUAWK                               (0x01) /**< IAS WD client cluster command ID for the Squawk. */


/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Enumeration of events that can be received by an IAS WD client cluster.
*/
typedef enum
{
   QAPI_ZB_CL_IASWD_CLIENT_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an Attribute Custom Read event. */
   QAPI_ZB_CL_IASWD_CLIENT_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an Attribute Custom Write event. */
   QAPI_ZB_CL_IASWD_CLIENT_EVENT_TYPE_DEFAULT_RESPONSE_E,  /**< Indicates a Default Response event. */
   QAPI_ZB_CL_IASWD_CLIENT_EVENT_TYPE_COMMAND_COMPLETE_E,  /**< Indicates a Command Complete event. */
   QAPI_ZB_CL_IASWD_CLIENT_EVENT_TYPE_UNPARSED_RESPONSE_E, /**< Indicates an Unparsed Response event. */
} qapi_ZB_CL_IASWD_Client_Event_Type_t;

/**
   Enumeration of events that can be received by a IAS WD server cluster.
*/
typedef enum
{
   QAPI_ZB_CL_IASWD_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an Attribute Custom Read event. */
   QAPI_ZB_CL_IASWD_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an Attribute Custom Write event. */
   QAPI_ZB_CL_IASWD_SERVER_EVENT_TYPE_UNPARSED_DATA_E,     /**< Indicates an Unparsed Data event. */
   QAPI_ZB_CL_IASWD_SERVER_EVENT_TYPE_START_WARNING_E,     /**< Indicates a Start Warning event. */
   QAPI_ZB_CL_IASWD_SERVER_EVENT_TYPE_SQUAWK_E             /**< Indicates a Squawk event. */
} qapi_ZB_CL_IASWD_Server_Event_Type_t;

/**
   Enumeration of warning modes used by an IAS WD cluster.
*/
typedef enum
{
   QAPI_ZB_CL_IASWD_WARNING_MODE_STOP_E            = 0x00, /**< Stop. */
   QAPI_ZB_CL_IASWD_WARNING_MODE_BURGLAR_E         = 0x01, /**< Burglar. */
   QAPI_ZB_CL_IASWD_WARNING_MODE_FIRE_E            = 0x02, /**< Fire. */
   QAPI_ZB_CL_IASWD_WARNING_MODE_EMERGENCY_E       = 0x03, /**< Emergency. */
   QAPI_ZB_CL_IASWD_WARNING_MODE_POLICE_PANIC_E    = 0x04, /**< Police Panic. */
   QAPI_ZB_CL_IASWD_WARNING_MODE_FIRE_PANIC_E      = 0x05, /**< Fire Panic. */
   QAPI_ZB_CL_IASWD_WARNING_MODE_EMERGENCY_PANIC_E = 0x06  /**< Emergency Panic. */
} qapi_ZB_CL_IASWD_Warning_Mode_t;

/**
   Enumeration of strobe modes used by an IAS WD cluster.
*/
typedef enum
{
   QAPI_ZB_CL_IASWD_STROBE_MODE_NO_STORBE_E              = 0x00, /**< No strobe. */
   QAPI_ZB_CL_IASWD_STROBE_MODE_USE_STORBE_IN_PARALLEL_E = 0x01, /**< Use strobe in parallel to warning or squawk. */
} qapi_ZB_CL_IASWD_Strobe_Mode_t;

/**
   Enumeration of siren and strobe levels used by an IAS WD cluster.
*/
typedef enum
{
   QAPI_ZB_CL_IASWD_SIREN_LEVEL_LOW_E       = 0x00, /**< Low. */
   QAPI_ZB_CL_IASWD_SIREN_LEVEL_MEDIUM_E    = 0x01, /**< Medium. */
   QAPI_ZB_CL_IASWD_SIREN_LEVEL_HIGH_E      = 0x02, /**< High. */
   QAPI_ZB_CL_IASWD_SIREN_LEVEL_VERY_HIGH_E = 0x03  /**< Very High. */
} qapi_ZB_CL_IASWD_Level_t;

/**
   Enumeration of notification sounds used for the squawk command in the IAS WD
   cluster.
*/
typedef enum
{
   QAPI_ZB_CL_IASWD_NOTIFICATION_SOUND_SYSTEM_ARMED_E    = 0x00, /**< System armed. */
   QAPI_ZB_CL_IASWD_NOTIFICATION_SOUND_SYSTEM_DISARMED_E = 0x01  /**< System disarmed. */
} qapi_ZB_CL_IASWD_Squawk_Mode_t;

/**
   Structure representing the Start Warning command parameters.
*/
typedef struct qapi_ZB_CL_IASWD_Start_Warning_s
{
   /** Warning mode. */
   qapi_ZB_CL_IASWD_Warning_Mode_t  WarningMode;

   /** Strobe mode. */
   qapi_ZB_CL_IASWD_Strobe_Mode_t   StrobeMode;

   /** Siren level. */
   qapi_ZB_CL_IASWD_Level_t         SirenLevel;

   /** Warning duration in seconds. */
   uint16_t                         WarningDuration;

   /** Strobe duty cyle in percent. */
   uint8_t                          StrobeDutyCycle;

   /** Siren level. */
   qapi_ZB_CL_IASWD_Level_t         StrobeLevel;
} qapi_ZB_CL_IASWD_Start_Warning_t;

/**
   Structure representing the Start Warning command parameters.
*/
typedef struct qapi_ZB_CL_IASWD_Start_Warning_Event_s
{
   /** Warning mode. */
   qapi_ZB_CL_IASWD_Warning_Mode_t WarningMode;

   /** Strobe mode. */
   qapi_ZB_CL_IASWD_Strobe_Mode_t  StrobeMode;

   /** Siren level. */
   qapi_ZB_CL_IASWD_Level_t        SirenLevel;

   /** Warning duration in seconds. */
   uint16_t                        WarningDuration;

   /** Strobe duty cyle in percent. */
   uint8_t                         StrobeDutyCycle;

   /** Siren level. */
   qapi_ZB_CL_IASWD_Level_t        StrobeLevel;
} qapi_ZB_CL_IASWD_Start_Warning_Event_t;

/**
   Structure representing the Squawk command parameters.
*/
typedef struct qapi_ZB_CL_IASWD_Squawk_s
{
   /** Squawk mode. */
   qapi_ZB_CL_IASWD_Squawk_Mode_t SquawkMode;

   /** Strobe mode. */
   qapi_ZB_CL_IASWD_Strobe_Mode_t SquawkStrobeMode;

   /** Squawk level. */
   qapi_ZB_CL_IASWD_Level_t       SquawkLevel;
} qapi_ZB_CL_IASWD_Squawk_t;

/**
   Structure representing the Squawk event parameters.
*/
typedef struct qapi_ZB_CL_IASWD_Squawk_Event_s
{
   /** Squawk mode. */
   qapi_ZB_CL_IASWD_Squawk_Mode_t SquawkMode;

   /** Strobe mode. */
   qapi_ZB_CL_IASWD_Strobe_Mode_t SquawkStrobeMode;

   /** Squawk level. */
   qapi_ZB_CL_IASWD_Level_t       SquawkLevel;
} qapi_ZB_CL_IASWD_Squawk_Event_t;

/**
   Structure representing the client data for events received by a IAS WD client
   cluster.
*/
typedef struct qapi_ZB_CL_IASWD_Client_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_IASWD_Client_Event_Type_t Event_Type;

   /** Data for the event. The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for custom read events. */
      qapi_ZB_CL_Attr_Custom_Read_t  Attr_Custom_Read;

      /** Information for custom write events. */
      qapi_ZB_CL_Attr_Custom_Write_t Attr_Custom_Write;

      /** Information for default response events. */
      qapi_ZB_CL_Default_Response_t  Default_Response;

      /** Information for command complete events. */
      qapi_ZB_CL_Command_Complete_t  Command_Complete;

      /** Information for unparsed response. */
      qapi_ZB_CL_Unparsed_Response_t Unparsed_Response;

   } Data;  /**< Union of event data. */
} qapi_ZB_CL_IASWD_Client_Event_Data_t;

/**
   Structure representing the server data for events received by a IAS WD server
   cluster.
*/
typedef struct qapi_ZB_CL_IASWD_Server_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_IASWD_Server_Event_Type_t Event_Type;

   /** Data for the event. The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for custom read events. */
      qapi_ZB_CL_Attr_Custom_Read_t          Attr_Custom_Read;

      /** Information for custom write events. */
      qapi_ZB_CL_Attr_Custom_Write_t         Attr_Custom_Write;

      /** Information for unparsed data events. */
      qapi_ZB_CL_Unparsed_Data_t             Unparsed_Data;

      /** Infomation for the start warning event. */
      qapi_ZB_CL_IASWD_Start_Warning_Event_t Start_Warning;

      /** Infomation for the squawk event. */
      qapi_ZB_CL_IASWD_Squawk_Event_t        Squawk;
   } Data;  /**< Union of event data. */
} qapi_ZB_CL_IASWD_Server_Event_Data_t;

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for an IAS WD client cluster.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] Cluster     Handle for the cluster.
   @param[in] Event_Data  Information for the cluster event.
   @param[in] CB_Param    User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_IASWD_Client_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_IASWD_Client_Event_Data_t *Event_Data, uint32_t CB_Param);

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for an IAS WD server cluster.

   @param[in] ZB_Handle   Handle of the ZigBee instance.
   @param[in] Cluster     Handle for the cluster.
   @param[in] Event_Data  Pointer to information for the cluster event.
   @param[in] CB_Param    User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_IASWD_Server_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_IASWD_Server_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Creates an IAS WD cluster client for a specific endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function will provide responses to IAS WD
   commands that are issued using this cluster.

   @param[in]  ZB_Handle    Handle of a ZigBee stack.
   @param[out] Cluster      Pointer to where the newly created cluster is to be
                            stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for the
                            IAS WD client cluster.
   @param[in]  CB_Param     Callback parameter for the cluster callback
                            function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASWD_Create_Client(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_IASWD_Client_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Creates an IAS WD cluster server for a specific endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   The callback registered with this function will provide events when state
   change commands are received.

   @param[in]  ZB_Handle    Handle of a ZigBee stack.
   @param[out] Cluster      Pointer to where the newly created cluster is to be
                            stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for the
                            IAS WD client cluster.
   @param[in]  CB_Param     Callback parameter for the cluster callback
                            function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASWD_Create_Server(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_IASWD_Server_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Populates an attribute list with the attributes required for an IAS 
          WD cluster.

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
qapi_Status_t qapi_ZB_CL_IASWD_Populate_Attributes(qbool_t Server, uint8_t *AttributeCount, qapi_ZB_CL_Attribute_t *AttributeList);

/**
   @brief Sends a Start Warning command to an IAS WD server.

   @param[in] Cluster  Handle for the IAS WD client cluster that is to be used
                       to send the command.
   @param[in] SendInfo General send information for the command packet.
   @param[in] Request  Information for the command request.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASWD_Client_Send_Start_Warning(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, const qapi_ZB_CL_IASWD_Start_Warning_t *Request);

/**
   @brief Sends a Squawk command to an IAS WD server.

   @param[in] Cluster  Handle for the IAS WD client cluster that is to be used
                       to send the command.
   @param[in] SendInfo General send information for the command packet.
   @param[in] Request  Information for the command request.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_IASWD_Client_Send_Squawk(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, const qapi_ZB_CL_IASWD_Squawk_t *Request);

/** @} */

#endif // ] #ifndef __QAPI_ZB_CL_IASWD_H__

