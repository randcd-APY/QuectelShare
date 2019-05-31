/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_ble_ans.h
 *
 * @brief
 * QCA QAPI for Bluetopia Bluetooth Alert Notification Service (ANS)
 * (GATT based) API Type Definitions, Constants, and
 * Prototypes.
 *
 * @details
 * The Alert Notification Service (ANS) programming interface
 * defines the protocols and procedures to be used to
 * implement Generic Access Profile Service capabilities.
 */

#ifndef __QAPI_BLE_ANS_H__
#define __QAPI_BLE_ANS_H__

#include "./qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.          */
#include "./qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.    */
#include "./qapi_ble_gatt.h"      /* qapi GATT prototypes.                    */
#include "./qapi_ble_anstypes.h"  /* QAPI ANS prototypes.                     */

/** @addtogroup qapi_ble_services
@{
*/

   /** @name Error Return Codes

      Error codes that are smaller than these (less than -1000) are
      related to the Bluetooth Protocol Stack itself (see
      qapi_ble_errors.h).
      @{ */
#define QAPI_BLE_ANS_ERROR_INVALID_PARAMETER                      (-1000)
/**< Invalid parameter. */
#define QAPI_BLE_ANS_ERROR_INSUFFICIENT_RESOURCES                 (-1001)
/**< Insufficient resources. */
#define QAPI_BLE_ANS_ERROR_SERVICE_ALREADY_REGISTERED             (-1002)
/**< Service is already registered. */
#define QAPI_BLE_ANS_ERROR_INVALID_INSTANCE_ID                    (-1003)
/**< Invalid service instance ID. */
#define QAPI_BLE_ANS_ERROR_MALFORMATTED_DATA                      (-1004)
/**< Malformatted data. */
#define QAPI_BLE_ANS_ERROR_UNKNOWN_ERROR                          (-1005)
/**< Unknown error. */

/** @} */ /* end namegroup */


   /** @name Supported Categories
      The following structure defines the valid Supported Categories bits
      that may be set to specify the supported New Alert and Unread
      Alert Status categories.
      @{ */
#define QAPI_BLE_ANS_SUPPORTED_CATEGORIES_SIMPLE_ALERT            0x0001
/**< Simple alert category is supported. */
#define QAPI_BLE_ANS_SUPPORTED_CATEGORIES_EMAIL                   0x0002
/**< Email category is supported. */
#define QAPI_BLE_ANS_SUPPORTED_CATEGORIES_NEWS                    0x0004
/**< News category is supported. */
#define QAPI_BLE_ANS_SUPPORTED_CATEGORIES_CALL                    0x0008
/**< Call category is supported. */
#define QAPI_BLE_ANS_SUPPORTED_CATEGORIES_MISSED_CALL             0x0010
/**< Missed call category is supported. */
#define QAPI_BLE_ANS_SUPPORTED_CATEGORIES_SMS_MMS                 0x0020
/**< SMS MMS ccategory is supported. */
#define QAPI_BLE_ANS_SUPPORTED_CATEGORIES_VOICE_MAIL              0x0040
/**< Voice mail category is supported. */
#define QAPI_BLE_ANS_SUPPORTED_CATEGORIES_SCHEDULE                0x0080
/**< Schedule category is supported. */
#define QAPI_BLE_ANS_SUPPORTED_CATEGORIES_HIGH_PRIORITY_ALERT     0x0100
/**< High priority category is supported. */
#define QAPI_BLE_ANS_SUPPORTED_CATEGORIES_INSTANT_MESSAGE         0x0200
/**< Instant message category is supported. */

/** @} */ /* end namegroup */

/**
 * Enumeration of all of the valid commands that may be
 * received in an
 * QAPI_BLE_ET_ANS_SERVER_CONTROL_POINT_COMMAND_INDICATION_E server event
 * or that may be written to a remote ANS server.
 */
typedef enum
{
   QAPI_BLE_PC_ENABLE_NEW_ALERT_NOTIFICATIONS_E        = QAPI_BLE_ANS_COMMAND_ID_ENABLE_NEW_INCOMING_ALERT_NOTIFICATION,
   /**< Enable new alert notifications. */
   QAPI_BLE_PC_ENABLE_UNREAD_CATEGORY_NOTIFICATIONS_E  = QAPI_BLE_ANS_COMMAND_ID_ENABLE_UNREAD_CATEGORY_STATUS_NOTIFICATION,
   /**< Enable unread category alert notifications. */
   QAPI_BLE_PC_DISABLE_NEW_ALERT_NOTIFICATIONS_E       = QAPI_BLE_ANS_COMMAND_ID_DISABLE_NEW_INCOMING_ALERT_NOTIFICATION,
   /**< Disable new alert notifications. */
   QAPI_BLE_PC_DISABLE_UNREAD_CATEGORY_NOTIFICATIONS_E = QAPI_BLE_ANS_COMMAND_ID_DISABLE_UNREAD_CATEGORY_STATUS_NOTIFICATION,
   /**< Disable unread category alert notifications. */
   QAPI_BLE_PC_NOTIFY_NEW_ALERT_IMMEDIATELY_E          = QAPI_BLE_ANS_COMMAND_ID_NOTIFY_NEW_INCOMING_ALERT_IMMEDIATELY,
   /**< Notify a new alert immediately. */
   QAPI_BLE_PC_NOTIFY_UNREAD_CATEGORY_IMMEDIATELY_E    = QAPI_BLE_ANS_COMMAND_ID_NOTIFY_UNREAD_CATEGORY_STATUS_IMMEDIATELY
   /**< Notify an unread category immediately. */
} qapi_BLE_ANS_Control_Point_Command_t;

/**
 * Enumeration of all of the CategoryIdentifications that
 * may be received in an
 * QAPI_BLE_ET_ANS_SERVER_CONTROL_POINT_COMMAND_INDICATION_E server event
 * or that may be written to a remote ANS Server.
 */
typedef enum
{
   QAPI_BLE_CI_SIMPLE_ALERT_E        = QAPI_BLE_ANS_ALERT_CATEGORY_ID_SIMPLE_ALERT,
   /**< Simple alert. */
   QAPI_BLE_CI_EMAIL_E               = QAPI_BLE_ANS_ALERT_CATEGORY_ID_EMAIL,
   /**< Email. */
   QAPI_BLE_CI_NEWS_E                = QAPI_BLE_ANS_ALERT_CATEGORY_ID_NEWS,
   /**< News. */
   QAPI_BLE_CI_CALL_E                = QAPI_BLE_ANS_ALERT_CATEGORY_ID_CALL,
   /**< Call. */
   QAPI_BLE_CI_MISSED_CALL_E         = QAPI_BLE_ANS_ALERT_CATEGORY_ID_MISSED_CALL,
   /**< Missed call. */
   QAPI_BLE_CI_SMS_MMS_E             = QAPI_BLE_ANS_ALERT_CATEGORY_ID_SMS_MMS,
   /**< SMS MMS. */
   QAPI_BLE_CI_VOICEMAIL_E           = QAPI_BLE_ANS_ALERT_CATEGORY_ID_VOICE_MAIL,
   /**< Voice mail. */
   QAPI_BLE_CI_SCHEDULE_E            = QAPI_BLE_ANS_ALERT_CATEGORY_ID_SCHEDULE,
   /**< Schedule. */
   QAPI_BLE_CI_HIGH_PRIORITY_ALERT_E = QAPI_BLE_ANS_ALERT_CATEGORY_ID_HIGH_PRIORITY_ALERT,
   /**< High priority alert. */
   QAPI_BLE_CI_INSTANT_MESSAGE_E     = QAPI_BLE_ANS_ALERT_CATEGORY_ID_INSTANT_MESSAGE,
   /**< Instant message. */
   QAPI_BLE_CI_ALL_CATEGORIES_E      = QAPI_BLE_ANS_ALERT_CATEGORY_ID_ALL_CATEGORIES
   /**< All categories. */
} qapi_BLE_ANS_Category_Identification_t;

/**
 * Enumeration of the valid Read Request types that a server
 * may receive in a
 * QAPI_BLE_ET_ANS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E or
 * QAPI_BLE_ET_ANS_SERVER_CLIENT_CONFIGURATION_UPDATE_E event.
 *
 * For each event, it is up to the application to return (or write) the
 * correct Client Configuration descriptor based on this value.
 */
typedef enum
{
   QAPI_BLE_CT_NEW_ALERT_E,          /**< New alert. */
   QAPI_BLE_CT_UNREAD_ALERT_STATUS_E /**< Unread alert status. */
} qapi_BLE_ANS_Characteristic_Type_t;

/**
 * Enumerations of the valid Supported Categories type that
 * may be set in the qapi_BLE_ANS_Set_Supported_Categories() and
 * qapi_BLE_ANS_Query_Supported_Categories() functions.
 */
typedef enum
{
   QAPI_BLE_SC_NEW_ALERT_E,          /**< New alert. */
   QAPI_BLE_SC_UNREAD_ALERT_STATUS_E /**< Unread alert status. */
} qapi_BLE_ANS_Supported_Categories_Type_t;

/**
 * Structure that contains the attribute handles that will need to be
 * cached by an Alert Notification Service (ANS) client in order to
 * only do service discovery once.
 */
typedef struct qapi_BLE_ANS_Client_Information_s
{
   /**
    * ANS Supported New Alert Category attribute handle.
    */
   uint16_t Supported_New_Alert_Category;

   /**
    * ANS New Alert attribute handle.
    */
   uint16_t New_Alert;

   /**
    * ANS New Alert  attribute handle.
    */
   uint16_t New_Alert_Client_Configuration;

   /**
    * ANS Supported Unread Alert Category attribute handle.
    */
   uint16_t Supported_Unread_Alert_Category;

   /**
    * ANS Unread Alert Status attribute handle.
    */
   uint16_t Unread_Alert_Status;

   /**
    * ANS Unread Alert Status Client Characteristic Configuration
    * Descriptor (CCCD) attribute handle.
    */
   uint16_t Unread_Alert_Status_Client_Configuration;

   /**
    * ANS Control Point attribute handle.
    */
   uint16_t Control_Point;
} qapi_BLE_ANS_Client_Information_t;

#define QAPI_BLE_ANS_CLIENT_INFORMATION_DATA_SIZE        (sizeof(qapi_BLE_ANS_Client_Information_t))
/**< Size of the #qapi_BLE_ANS_Client_Information_t structure. */

/**
 * Structure that contains all of the per client data that will need to
 * be stored by an ANS server.
 */
typedef struct qapi_BLE_ANS_Server_Information_s
{
   /**
    * Flags if the New Alert CCCD has been configured for notifications.
    */
   boolean_t New_Alert_Client_Configuration;

   /**
    * Flags if the Unread Alert Status CCCD has been configured for
    * notifications.
    */
   boolean_t Unread_Alert_Status_Client_Configuration;

   /**
    * New alert categories that have been enabled.
    */
   uint16_t  Enabled_New_Alert_Categories;

   /**
    * Unread alert categories that have been enabled.
    */
   uint16_t  Enabled_Unread_Alert_Categories;
} qapi_BLE_ANS_Server_Information_t;

#define QAPI_BLE_ANS_SERVER_INFORMATION_DATA_SIZE        (sizeof(qapi_BLE_ANS_Server_Information_t))
/**< Size of the #qapi_BLE_ANS_Server_Information_t structure. */

#define QAPI_BLE_ANS_SERVER_INFORMATION_ENABLE_CATEGORY(_x, _y)   \
   (((uint16_t *)(_x))[0]) |= (((_y) != QAPI_BLE_ANS_ALERT_CATEGORY_ID_ALL_CATEGORIES)?((uint16_t)((0x0001) << ((_y) % (QAPI_BLE_WORD_SIZE*8)))):((uint16_t)0xFFFF))
/**<
 * This macro is a utility macro that exists to aid in enabling a
 * specific ANS Category. The first parameter to this macro should be a
 * uint16_t pointer to enable a category in. The second parameter should
 * be the category to enable.
 *
 * The second parameter must be of the form:
 * QAPI_BLE_ANS_ALERT_CATEGORY_ID_XXX.
 */

#define QAPI_BLE_ANS_SERVER_INFORMATION_DISABLE_CATEGORY(_x, _y)  \
   (((uint16_t *)(_x))[0]) &= (((_y) != QAPI_BLE_ANS_ALERT_CATEGORY_ID_ALL_CATEGORIES)?((uint16_t)~((0x0001) << ((_y) % (QAPI_BLE_WORD_SIZE*8)))):((uint16_t)0x0000))
/**<
 * This macro is a utility macro that exists to aid in disabling a
 * specific ANS Category. The first parameter to this macro should be a
 * uint16_t pointer to disable a category in. The second parameter
 * should be the category to enable.
 *
 * The second parameter must be of the form:
 * QAPI_BLE_ANS_ALERT_CATEGORY_ID_XXX.
 */

#define QAPI_BLE_ANS_SERVER_INFORMATION_CATEGORY_ENABLED(_x, _y)  \
   ((((uint16_t *)(_x))[0]) & ((0x0001) << ((_y) % (QAPI_BLE_WORD_SIZE*8))))
/**<
 * This macro is a utility macro that exists to aid in determining
 * if a specific ANS Category is enabled.  The first parameter to this
 * macro should be a uint16_t pointer to test for the category. The
 * second parameter should be the category to check. This macro returns
 * a Boolean_t TRUE if the category is enabled or FALSE otherwise.
 *
 * The second parameter must be of the form:
 * QAPI_BLE_ANS_ALERT_CATEGORY_ID_XXX.
 */

/**
 * Structure that represents the structure of a New Alert. This is used
 * to notify a remote ANS Client of a New Alert in a specified category.
 */
typedef struct qapi_BLE_ANS_New_Alert_Data_s
{
   /**
    * Category ID.
    *
    * The CategoryID field may not be set to QAPI_BLE_CI_ALL_CATEGORIES_E in
    * this structure.
    */
   qapi_BLE_ANS_Category_Identification_t  CategoryID;

   /**
    * Number of new alerts.
    */
   uint8_t                                 NumberOfNewAlerts;

   /**
    * Pointer to the last alert string.
    *
    * This field is optional and may be set to NULL.
    */
   char                                   *LastAlertString;
} qapi_BLE_ANS_New_Alert_Data_t;

#define QAPI_BLE_ANS_NEW_ALERT_DATA_SIZE                 (sizeof(qapi_BLE_ANS_New_Alert_Data_t))
/**< Size of the #qapi_BLE_ANS_New_Alert_Data_t structure. */

/**
 * Structure thatrepresents the structure of an Unread Alert. This is
 * used to notify a remote ANS Client of an Unread Alert in a specified
 * category.
 */
typedef struct qapi_BLE_ANS_Un_Read_Alert_Data_s
{
   /**
    * Category ID.
    *
    * The CategoryID field may not be set to QAPI_BLE_CI_ALL_CATEGORIES_E in
    * this structure.
    */
   qapi_BLE_ANS_Category_Identification_t CategoryID;

   /**
    * Number of unread alerts.
    */
   uint8_t                                NumberOfUnreadAlerts;
} qapi_BLE_ANS_Un_Read_Alert_Data_t;

#define QAPI_BLE_ANS_UN_READ_ALERT_DATA_SIZE             (sizeof(qapi_BLE_ANS_Un_Read_Alert_Data_t))
/**< Size of the #qapi_BLE_ANS_Un_Read_Alert_Data_t structure. */

/**
 * Enumerations of all the events generated by the ANS
 * Profile. These are used to determine the type of each event
 * generated and to ensure the proper union element is accessed for the
 * #qapi_BLE_ANS_Event_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_ET_ANS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E,
   /**< Read CCCD request event. */
   QAPI_BLE_ET_ANS_SERVER_CLIENT_CONFIGURATION_UPDATE_E,
   /**< Write CCCD request event. */
   QAPI_BLE_ET_ANS_SERVER_CONTROL_POINT_COMMAND_INDICATION_E
   /**< Control Point indication event. */
} qapi_BLE_ANS_Event_Type_t;

/**
 * Structure that represents the format for the data that is dispatched
 * to an ANS server when an ANS client has sent a request to read an ANS
 * characteristic's CCCD.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_ANS_Read_Client_Configuration_Response() function.
 */
typedef struct qapi_BLE_ANS_Read_Client_Configuration_Data_s
{
   /**
    * ANS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the ANS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the ANS Client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * Bluetooth address of the ANS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the requested CCCD based on the ANS characteristic type.
    */
   qapi_BLE_ANS_Characteristic_Type_t ClientConfigurationType;
} qapi_BLE_ANS_Read_Client_Configuration_Data_t;

#define QAPI_BLE_ANS_READ_CLIENT_CONFIGURATION_DATA_SIZE  (sizeof(qapi_BLE_ANS_Read_Client_Configuration_Data_t))
/**<
 * Size of the #qapi_BLE_ANS_Read_Client_Configuration_Data_t*
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an ANS server when an ANS client has sent a request to write an ANS
 * characteristic's CCCD.
 */
typedef struct qapi_BLE_ANS_Client_Configuration_Update_Data_s
{
   /**
    *  ANS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the ANS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the ANS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * Bluetooth address of the ANS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the requested CCCD based on the ANS characteristic
    * type.
    */
   qapi_BLE_ANS_Characteristic_Type_t ClientConfigurationType;

   /**
    * Flags if notifications have been enabled for the requested CCCD.
    */
   boolean_t                          NotificationsEnabled;
} qapi_BLE_ANS_Client_Configuration_Update_Data_t;

#define QAPI_BLE_ANS_CLIENT_CONFIGURATION_UPDATE_DATA_SIZE  (sizeof(qapi_BLE_ANS_Client_Configuration_Update_Data_t))
/**<
 * Size of the #qapi_BLE_ANS_Client_Configuration_Update_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an ANS server when an ANS client has sent a request to write an ANS
 * Control Point characteristic.
 */
typedef struct qapi_BLE_ANS_Control_Point_Command_Data_s
{
   /**
    * ANS instance that dispatched the event.
    */
   uint32_t                               InstanceID;

   /**
    * GATT connection ID for the connection with the ANS client
    * that made the request.
    */
   uint32_t                               ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the ANS client.
    */
   qapi_BLE_GATT_Connection_Type_t        ConnectionType;

   /**
    * Bluetooth address of the ANS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                     RemoteDevice;

   /**
    * Control Point command.
    */
   qapi_BLE_ANS_Control_Point_Command_t   Command;

   /**
    * Category identification.
    */
   qapi_BLE_ANS_Category_Identification_t Category;
} qapi_BLE_ANS_Control_Point_Command_Data_t;

#define QAPI_BLE_ANS_CONTROL_POINT_COMMAND_DATA_SIZE     (sizeof(qapi_BLE_ANS_Control_Point_Command_Data_t))
/**< Size of the #qapi_BLE_ANS_Control_Point_Command_Data_t structure. */

/**
 * Structure that represents the container structure for holding all the
 * event data for an ANS instance.
 */
typedef struct qapi_BLE_ANS_Event_Data_s
{
   /**
    * Event type used to determine the appropriate union member of
    * the Event_Data field to access.
    */
   qapi_BLE_ANS_Event_Type_t Event_Data_Type;

   /**
    * Total size of the data contained in the event.
    */
   uint16_t                  Event_Data_Size;
   union
   {
      /**
       * ANS read client configuration data.
       */
      qapi_BLE_ANS_Read_Client_Configuration_Data_t   *ANS_Read_Client_Configuration_Data;

      /**
       * ANS client configuration update data.
       */
      qapi_BLE_ANS_Client_Configuration_Update_Data_t *ANS_Client_Configuration_Update_Data;

      /**
       * ANS control point data.
       */
      qapi_BLE_ANS_Control_Point_Command_Data_t       *ANS_Control_Point_Command_Data;
   }
   /**
    * Event data. @newpagetable
    */
   Event_Data;
} qapi_BLE_ANS_Event_Data_t;

#define QAPI_BLE_ANS_EVENT_DATA_SIZE                     (sizeof(qapi_BLE_ANS_Event_Data_t))
/**< Size of the #qapi_BLE_ANS_Event_Data_t structure. */

/**
 * @brief
 * Prototype function for an
 * ANS event callback. This function will be called whenever a define
 * ANS event occurs within the Bluetooth Protocol Stack that is
 * specified with the specified Bluetooth Stack ID.
 *
 * @details
 * The event information is passed to the user in an #qapi_BLE_ANS_Event_Data_t
 * structure. This structure contains all the information about the
 * event that occurred.
 *
 * The caller should use the contents of the ANS Event Data
 * only in the context of this callback. If the caller requires
 * the data for a longer period of time, then the callback function
 * must copy the data into another Data Buffer.
 *
 * This function is guaranteed not to be invoked more than once
 * simultaneously for the specified installed callback (i.e., this
 * function does not have be reentrant). It needs to be noted however,
 * that if the same callback is installed more than once, the
 * callbacks will be called serially. Because of this, the processing
 * in this function should be as efficient as possible.
 *
 * It should also be noted that this function is called in the Thread
 * Context of a thread that the user does not own. Therefore, processing
 * in this function should be as efficient as possible (this argument holds
 * anyway because another ANS event will not be processed while this
 * function call is outstanding).
 *
 * @note1hang
 * This function must not block and wait for events that can only be
 * satisfied by receiving other Bluetooth Stack Events. A Deadlock
 * will occur because other callbacks might not be issued while
 * this function is currently outstanding.
 *
 * @param[in]  BluetoothStackID      Unique identifier assigned to this
 *                                   Bluetooth Protocol Stack on which the
 *                                   event occurred.
 *
 * @param[in]  ANS_Event_Data        Pointer to a structure that contains
 *                                   information about the event that has
 *                                   occurred.
 *
 * @param[in]  CallbackParameter     User-defined value that will be
 *                                   received with the ANS event data.
 *
 * @return None.
 */
typedef void (QAPI_BLE_BTPSAPI *qapi_BLE_ANS_Event_Callback_t)(uint32_t BluetoothStackID, qapi_BLE_ANS_Event_Data_t *ANS_Event_Data, uint32_t CallbackParameter);

   /* ANS Server API.                                                   */

/**
 * @brief
 * Opens an ANS server on a specified Bluetooth stack.
 *
 * @details
 * Only one ANS server may be open at a time, per the Bluetooth Stack ID.
 *
 * All client requests will be dispatched to the EventCallback function
 * that is specified by the second parameter to this function.
 *
 * @param[in]  BluetoothStackID     Unique identifier assigned to this
 *                                  Bluetooth Protocol Stack via a
 *                                  call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  EventCallback        Callback function that is registered
 *                                  to receive events that are associated
 *                                  with the specified service.
 *
 * @param[in]  CallbackParameter    User-defined parameter that will be
 *                                  passed back to the user in the callback
 *                                  function.
 *
 * @param[out]  ServiceID           Unique GATT service ID of the
 *                                  registered ANS service returned from
 *                                  qapi_BLE_GATT_Register_Service() API.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of ANS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_ANS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_ANS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BT_GATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ANS_Initialize_Service(uint32_t BluetoothStackID, qapi_BLE_ANS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

/**
 * @brief
 * Opens an ANS server on a specified Bluetooth stack with
 * the ability to control the location of the service in the GATT database.
 *
 * @details
 * Only one ANS server may be open at a time, per the Bluetooth Stack ID.
 *
 * All client requests will be dispatch to the EventCallback function
 * that is specified by the second parameter to this function.
 *
 * @param[in]      BluetoothStackID      Unique identifier assigned to
 *                                       this Bluetooth Protocol Stack
 *                                       via a call to
 *                                       qapi_BLE_BSC_Initialize().
 *
 * @param[in]      EventCallback         Callback function that is
 *                                       registered to receive events
 *                                       that are associated with the
 *                                       specified service.
 *
 * @param[in]      CallbackParameter     User-defined parameter that
 *                                       will be passed back to the user
 *                                       in the callback function.
 *
 * @param[out]     ServiceID             Unique GATT Service ID of the
 *                                       registered service returned
 *                                       from
 *                                       qapi_BLE_GATT_Register_Service()
 *                                       API.
 *
 * @param[in,out]  ServiceHandleRange    Pointer to a Service Handle
 *                                       Range structure that on input
 *                                       can be used to control the
 *                                       location of the service in the
 *                                       GATT database, and on output
 *                                       returns the handle range that
 *                                       the service is using in the GATT
 *                                       database.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of ANS server that was successfully
 *              opened on the specified Bluetooth stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_ANS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_ANS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BT_GATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ANS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, qapi_BLE_ANS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

/**
 * @brief
 * Cleans up and frees all resources
 * associated with a ANS Service Instance.
 *
 * @details
 * After this function is called, no other ANS service function can be
 * called until after a successful call to the qapi_BLE_ANS_Initialize_Service()
 * function is performed.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from the qapi_BLE_ANS_Initialize_Service()
 *                                 function.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_ANS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_ANS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ANS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * Queries the number of attributes
 * that are contained in the ANS service that is registered with a
 * call to qapi_BLE_ANS_Initialize_Service() or
 * qapi_BLE_ANS_Initialize_Service_Handle_Range().
 *
 * @return   Positive, nonzero, number of attributes that are
 *           registered by a ANS service instance. \n
 *           Zero on failure.
 */
QAPI_BLE_DECLARATION unsigned int QAPI_BLE_BTPSAPI qapi_BLE_ANS_Query_Number_Attributes(void);

/**
 * @brief
 * Sets the Alert Notification
 * Supported Categories for the specified category type.
 *
 * @details
 * The SupportedCategoriesMask is a pointer to a bitmask that is made up of
 * bitmasks of the form QAPI_BLE_ANS_SUPPORTED_CATEGORIES_XXX, if success.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of the
 *                                 qapi_BLE_ANS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  SupportedCategoryType    Enumeration that specifies the
 *                                      category type that will be set.
 *
 * @param[out]  SupportedCategoriesMask    Supported categories bitmask
 *                                         that will be set if this
 *                                         function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_ANS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_ANS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ANS_Set_Supported_Categories(uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_ANS_Supported_Categories_Type_t SupportedCategoryType, uint16_t SupportedCategoriesMask);

/**
 * @brief
 * Gets the Alert Notification
 * Supported Categories for the specified category type.
 *
 * @details
 * The SupportedCategoriesMask is a pointer to a bitmask that will be
 * made up of bitmasks of the form
 * QAPI_BLE_ANS_SUPPORTED_CATEGORIES_XXX, if success.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of the
 *                                 qapi_BLE_ANS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  SupportedCategoryType    Enumeration that specifies the
 *                                      category type to query.
 *
 * @param[out]  SupportedCategoriesMask    Pointer that will store the
 *                                         supported categories bitmask
 *                                         if this function is
 *                                         successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_ANS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_ANS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ANS_Query_Supported_Categories(uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_ANS_Supported_Categories_Type_t SupportedCategoryType, uint16_t *SupportedCategoriesMask);

/**
 * @brief
 * Allows a mechanism for a ANS service to
 * successfully respond to a received read client configuration request.
 *
 * @details
 * Possible events: \n
 *    QAPI_BLE_ET_ANS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E
 *
 * @param[in]  BluetoothStackID        Unique identifier assigned to this
 *                                     Bluetooth Protocol Stack via a
 *                                     call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID              Service iInstance ID to close.
 *                                     This is the value that was returned from either of the
 *                                     qapi_BLE_ANS_Initialize_XXX() functions.
 *
 * @param[in]  TransactionID           Transaction ID of the original
 *                                     read request. This value was
 *                                     received in the
 *                                     QAPI_BLE_ET_ANS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E
 *                                     event.
 *
 * @param[in]  NotificationsEnabled    Boolean value that indicates whether
 *                                     notifications have been enabled.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_ANS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_ANS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ANS_Read_Client_Configuration_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, boolean_t NotificationsEnabled);

/**
 * @brief
 * Sends a new alert notification to a
 * specified ANS client.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned from either of the
 *                                 qapi_BLE_ANS_Initialize_XXX() functions.
 *
 * @param[in]  ConnectionID        Connection ID of the ANS client to which to send
 *                                 the notification.
 *
 * @param[in]  NewAlert            Pointer to the new alert that is
 *                                 going to be notified to the ANS
 *                                 client.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_ANS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_ANS_ERROR_UNKNOWN_ERROR \n
 *                 QAPI_BLE_ANS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_HANDLE_VALUE \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_CONNECTION_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ANS_New_Alert_Notification(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_ANS_New_Alert_Data_t *NewAlert);

/**
 * @brief
 * Sends an unread alert notification to
 * a specified ANS client.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned from
 *                                 either of the
 *                                 qapi_BLE_ANS_Initialize_XXX() functions.
 *
 * @param[in]  ConnectionID        Connection ID of the ANS client to which to send
 *                                 the notification.
 *
 * @param[in]  UnReadAlert         Pointer to the unread alert that is
 *                                 going to be notified to the ANS client.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_ANS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_ANS_ERROR_UNKNOWN_ERROR \n
 *                 QAPI_BLE_ANS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_HANDLE_VALUE \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_CONNECTION_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ANS_Un_Read_Alert_Status_Notification(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_ANS_Un_Read_Alert_Data_t *UnReadAlert);

   /* ANS Client API.                                                   */

/**
 * @brief
 * Parses a value received from a remote
 * ANS server interpreting it as a New Alert notification.
 *
 * @details
 * The return value from this function must be freed by calling
 * qapi_BLE_ANS_Free_New_Alert_Data() when the decoded New Alert
 * Notification is no longer needed.
 *
 * @param[in]   ValueLength    Specifies the length of the value returned
 *                             by the remote ANS server.
 *
 * @param[in]   Value          Pointer to the data returned by
 *                             the remote ANS server.
 *
 * @return       A pointer to the decoded alert data. \n
 *               NULL if an error occurs.
 */
QAPI_BLE_DECLARATION qapi_BLE_ANS_New_Alert_Data_t *QAPI_BLE_BTPSAPI qapi_BLE_ANS_Decode_New_Alert_Notification(uint32_t ValueLength, uint8_t *Value);

/**
 * @brief
 * Freed the decoded New Alert data
 * that was returned by a successful call to
 * qapi_BLE_ANS_Decode_New_Alert_Notification().
 *
 * @param[in]   NewAlertData  Pointer to the New Alert data that is
 *                            going to be freed.
 *
 * @return      None.
 */
QAPI_BLE_DECLARATION void QAPI_BLE_BTPSAPI qapi_BLE_ANS_Free_New_Alert_Data(qapi_BLE_ANS_New_Alert_Data_t *NewAlertData);

/**
 * @brief
 * Parses a value received from a remote
 * ANS server interpreting it as an unread alert notification.
 *
 * @param[in]   ValueLength    Specifies the length of the value returned
 *                             by the remote ANS server.
 *
 * @param[in]   Value          Pointer to the data returned by
 *                             the remote ANS server.
 *
 * @param[out]  UnReadAlert    Pointer that will hold the decoded
 *                             unread alert if this function is
 *                             successful
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_ANS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_ANS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ANS_Decode_Un_Read_Alert_Status_Notification(uint32_t ValueLength, uint8_t *Value, qapi_BLE_ANS_Un_Read_Alert_Data_t *UnReadAlert);

/**
 * @brief
 * Parses a value received from a remote
 * ANS server, interpreting it as the Supported Categories value.
 *
 * @param[in]   ValueLength    Specifies the length of the value returned
 *                             by the remote ANS server.
 *
 * @param[in]   Value          Pointer to the data returned by
 *                             the remote ANS server.
 *
 * @param[out]  SupportedCategoriesMask    Pointer that will hold the
 *                                         decoded supported categories
 *                                         mask if this function is
 *                                         successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_ANS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_ANS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ANS_Decode_Supported_Categories(uint32_t ValueLength, uint8_t *Value, uint16_t *SupportedCategoriesMask);

/**
 * @brief
 * Formats an Alert Notification
 * Control Point Command into a user-specified buffer to send to a remote
 * ANS server.
 *
 * @param[in]   Command          ANS Control Point Command that will
 *                               be formatted into the user-specified
 *                               buffer.
 *
 * @param[in]   CommandCategory   Enumeration that specifies the
 *                                category to which the command applies.
 *
 * @param[out]  CommandBuffer     Pointer to the ANS Control Point
 *                                Command Buffer that will receive the
 *                                command and command category if this
 *                                function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_ANS_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_ANS_Format_Control_Point_Command(qapi_BLE_ANS_Control_Point_Command_Value_t *CommandBuffer, qapi_BLE_ANS_Control_Point_Command_t Command, qapi_BLE_ANS_Category_Identification_t CommandCategory);

/** @}
 */

#endif

