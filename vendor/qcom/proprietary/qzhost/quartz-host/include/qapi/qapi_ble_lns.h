/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_ble_lns.h
 *
 * @brief
 * QCA QAPI for Bluetopia Bluetooth Location and Navigation Service
 * (GATT based) API Type Definitions, Constants, and
 * Prototypes.
 *
 * @details
 * The Location and Navigation Service (LNS) programming interface defines the
 * protocols and procedures to be used to implement the Location and Navigation
 * Service capabilities.
 */

#ifndef __QAPI_BLE_LNS_H__
#define __QAPI_BLE_LNS_H__

#include "./qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.          */
#include "./qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.    */
#include "./qapi_ble_gatt.h"      /* qapi GATT prototypes.                    */
#include "./qapi_ble_lnstypes.h"  /* QAPI LNS prototypes.                     */

/** @addtogroup qapi_ble_services
@{
*/

   /* Error Return Codes.                                               */

   /* Error Codes that are smaller than these (less than -1000) are     */
   /* related to the Bluetooth Protocol Stack itself (see               */
   /* qapi_ble_errros.h).                                               */
#define QAPI_BLE_LNS_ERROR_INVALID_PARAMETER             (-1000)
/**< Invalid parameter. */
#define QAPI_BLE_LNS_ERROR_INVALID_BLUETOOTH_STACK_ID    (-1001)
/**< Invalid Bluetooth Stack ID. */
#define QAPI_BLE_LNS_ERROR_INSUFFICIENT_RESOURCES        (-1002)
/**< Insufficient resources. */
#define QAPI_BLE_LNS_ERROR_INSUFFICIENT_BUFFER_SPACE     (-1003)
/**< Insufficient buffer space. */
#define QAPI_BLE_LNS_ERROR_SERVICE_ALREADY_REGISTERED    (-1004)
/**< Service is already registered. */
#define QAPI_BLE_LNS_ERROR_INVALID_INSTANCE_ID           (-1005)
/**< Invalid service instance ID. */
#define QAPI_BLE_LNS_ERROR_MALFORMATTED_DATA             (-1006)
/**< Malformatted data. */
#define QAPI_BLE_LNS_ERROR_INDICATION_OUTSTANDING        (-1007)
/**< Indication outstanding. */

/**
 * Structure that contains the attribute handles that will need to be
 * cached by a LNS client in order to only do service discovery once.
 */
typedef struct qapi_BLE_LNS_Client_Information_s
{
   /**
    * LNS Feature attribute handle.
    */
   uint16_t LN_Feature;

   /**
    * LNS Location and Speed attribute handle.
    */
   uint16_t Location_And_Speed;

   /**
    * LNS Location and Speed Client Characteristic Configuration
    * Descriptor (CCCD) attribute handle.
    */
   uint16_t Location_And_Speed_Client_Configuration;

   /**
    * LNS Position Quality attribute handle.
    */
   uint16_t Position_Quality;

   /**
    * LNS LN Control Point attribute handle.
    */
   uint16_t LN_Control_Point;

   /**
    * LNS LN Control Point CCCD attribute handle.
    */
   uint16_t LN_Control_Point_Client_Configuration;

   /**
    * LNS Navigation attribute handle.
    */
   uint16_t Navigation;

   /**
    * LNS Navigation CCCD attribute handle.
    */
   uint16_t Navigation_Client_Configuration;
} qapi_BLE_LNS_Client_Information_t;

#define QAPI_BLE_LNS_CLIENT_INFORMATION_DATA_SIZE        (sizeof(qapi_BLE_LNS_Client_Information_t))
/**<
 * Size of the #qapi_BLE_LNS_Client_Information_t structure.
 */

/**
 * Structure that contains all of the per client data that will need to
 * be stored by a LNS server.
 */
typedef struct qapi_BLE_LNS_Server_Information_s
{
   /**
    * LNS Location and Speed CCCD.
    */
   uint16_t Location_And_Speed_Client_Configuration;

   /**
    * LNS LN Control Point CCCD.
    */
   uint16_t LN_Control_Point_Client_Configuration;

   /**
    * LNS Navigation CCCD.
    */
   uint16_t Navigation_Client_Configuration;
} qapi_BLE_LNS_Server_Information_t;

#define QAPI_BLE_LNS_SERVER_INFORMATION_DATA_SIZE        (sizeof(qapi_BLE_LNS_Server_Information_t))
/**<
 * Size of the #qapi_BLE_LNS_Server_Information_t structure.
 */

/**
 * Structure that represents the Date/Time data for LNS.
 */
typedef struct qapi_BLE_LNS_Date_Time_Data_s
{
   /**
    * Year.
    */
   uint16_t Year;

   /**
    * Month.
    */
   uint8_t  Month;

   /**
    * Day.
    */
   uint8_t  Day;

   /**
    * Hour.
    */
   uint8_t  Hours;

   /**
    * Minutes.
    */
   uint8_t  Minutes;

   /**
    * Seconds.
    */
   uint8_t  Seconds;
} qapi_BLE_LNS_Date_Time_Data_t;

#define QAPI_BLE_LNS_DATE_TIME_DATA_SIZE                 (sizeof(qapi_BLE_LNS_Date_Time_Data_t))
/**<
 * Size of the #qapi_BLE_LNS_Date_Time_Data_t structure.
 */

/**
 * Structure that represents the LNS Location and Speed data.
 */
typedef struct qapi_BLE_LNS_Location_Speed_Data_s
{
   /**
    * Bitmask that indicates the optional fields and features that may
    * be included for the LNS Location and Speed data.
    *
    * Valid values have the form QAPI_BLE_LNS_LOCATION_AND_SPEED_FLAG_XXX and
    * can be found in qapi_ble_lnstypes.h.
    */
   uint16_t                      Flags;

   /**
    * Instantaneous speed.
    */
   uint16_t                      InstantaneousSpeed;

   /**
    * Total distance.
    */
   uint8_t                       TotalDistance[3];

   /**
    * Latitude.
    */
   int32_t                       LocationLatitude;

   /**
    * Longitude.
    */
   int32_t                       LocationLongitude;

   /**
    * Elevation.
    */
   int8_t                        Elevation[3];

   /**
    * Heading.
    */
   uint16_t                      Heading;

   /**
    * Rolling time.
    */
   uint8_t                       RollingTime;

   /**
    * UTC date/time.
    */
   qapi_BLE_LNS_Date_Time_Data_t UTCTime;
} qapi_BLE_LNS_Location_Speed_Data_t;

#define QAPI_BLE_LNS_LOCATION_SPEED_DATA_SIZE            (sizeof(qapi_BLE_LNS_Location_Speed_Data_t))
/**<
 * Size of the #qapi_BLE_LNS_Location_Speed_Data_t structure.
 */

#define QAPI_BLE_LNS_DATE_TIME_VALID(_x)                 ((QAPI_BLE_GATT_DATE_TIME_VALID_YEAR(((_x)).Year)) && (QAPI_BLE_GATT_DATE_TIME_VALID_MONTH(((_x)).Month)) && (QAPI_BLE_GATT_DATE_TIME_VALID_DAY(((_x)).Day)) && (QAPI_BLE_GATT_DATE_TIME_VALID_HOURS(((_x)).Hours)) && (QAPI_BLE_GATT_DATE_TIME_VALID_MINUTES(((_x)).Minutes)) && (QAPI_BLE_GATT_DATE_TIME_VALID_SECONDS(((_x)).Seconds)))
/**<
 * This macro is a utility macro that exists to valid that a specified
 * Date Time is valid. The only parameter to this function is the
 * #qapi_BLE_LNS_Date_Time_Data_t structure to valid. This macro returns
 * TRUE if the Date Time is valid or FALSE otherwise.
 */

/**
 * Structure that represents the LNS Position Quality data.
 */
typedef struct qapi_BLE_LNS_Position_Quality_Data_s
{
   /**
    * Bitmask that indicates the optional fields and features that may
    * be included for the LNS Location and Speed data.
    *
    * Valid values have the form QAPI_BLE_LNS_POSITION_QUALITY_FLAG_XXX and
    * can be found in qapi_ble_lnstypes.h.
    */
   uint16_t  Flags;

   /**
    * Number of beacons in the solution.
    */
   uint8_t   NumberofBeaconsinSolution;

   /**
    * Number of beacons in the view.
    */
   uint8_t   NumberofBeaconsinView;

   /**
    * Time to first fix.
    */
   uint16_t  TimetoFirstfix;

   /**
    * Estimated horizontal position error.
    */
   uint32_t  EstimatedHorizontalPositionError;

   /**
    * Estimated vertical position error.
    */
   uint32_t  EstimatedVerticalPositionError;

   /**
    * Horizontal dilution of precision.
    */
   uint8_t   HorizontalDilutionofPrecision;

   /**
    * Vertical dilution of precision.
    */
   uint8_t   VerticalDilutionofPrecision;
} qapi_BLE_LNS_Position_Quality_Data_t;

#define QAPI_BLE_LNS_POSITION_QUALITY_DATA_SIZE          (sizeof(qapi_BLE_LNS_Position_Quality_Data_t))
/**<
 * Size of the #qapi_BLE_LNS_Position_Quality_Data_t structure.
 */

/**
 * Enumeration that represents the valid values that may be set as the
 * value for the OpCode field of LN Control Point characteristic.
 */
typedef enum
{
   QAPI_BLE_LNC_SET_CUMULATIVE_VALUE_E                        = QAPI_BLE_LNS_LN_CONTROL_POINT_OPCODE_SET_CUMULATIVE_VALUE,
   /**< Set cumulative value. */
   QAPI_BLE_LNC_MASK_LOCATION_SPEED_CHARACTERISTICS_CONTENT_E = QAPI_BLE_LNS_LN_CONTROL_POINT_MASK_LOCATION_SPEED_CHARACTERISTIC_CONTENT,
   /**< Mask location speed characteristic's content. */
   QAPI_BLE_LNC_NAVIGATION_CONTROL_E                          = QAPI_BLE_LNS_LN_CONTROL_POINT_NAVIGATION_CONTROL,
   /**< Navigation control. */
   QAPI_BLE_LNC_REQUEST_NUMBER_OF_ROUTES_E                    = QAPI_BLE_LNS_LN_CONTROL_POINT_REQUEST_NUMBER_OF_ROUTES,
   /**< Request number of routes. */
   QAPI_BLE_LNC_REQUEST_NAME_OF_ROUTE_E                       = QAPI_BLE_LNS_LN_CONTROL_POINT_REQUEST_NAME_OF_ROUTE,
   /**< Request name of route. */
   QAPI_BLE_LNC_SELECT_ROUTE_E                                = QAPI_BLE_LNS_LN_CONTROL_POINT_SELECT_ROUTE,
   /**< Select route. */
   QAPI_BLE_LNC_SET_FIX_RATE_E                                = QAPI_BLE_LNS_LN_CONTROL_POINT_SET_FIX_RATE,
   /**< Set fix rate. */
   QAPI_BLE_LNC_SET_ELEVATION_E                               = QAPI_BLE_LNS_LN_CONTROL_POINT_SET_ELEVATION
   /**< Set elevation. */
} qapi_BLE_LNS_LNCP_Command_Type_t;

/**
 * Enumeration that represents the valid values that may be set as the
 * value for the Response Opcode field of Location and Navigation
 * Operation Control Point characteristic.
 */
typedef enum
{
   QAPI_BLE_LNC_SUCCESS_E              = QAPI_BLE_LNS_LN_CONTROL_POINT_RESPONSE_CODE_SUCCESS,
   /**< Success response. */
   QAPI_BLE_LNC_OPCODE_NOT_SUPPORTED_E = QAPI_BLE_LNS_LN_CONTROL_POINT_RESPONSE_OPCODE_NOT_SUPPORTED,
   /**< Op code not supported response. */
   QAPI_BLE_LNC_INVALID_PARAMETER_E    = QAPI_BLE_LNS_LN_CONTROL_POINT_RESPONSE_INVALID_PARAMETER,
   /**< Invalid parameter response. */
   QAPI_BLE_LNC_OPERATION_FAILED_E     = QAPI_BLE_LNS_LN_CONTROL_POINT_RESPONSE_OPERATION_FAILED
   /**< Operation failed response. */
} qapi_BLE_LNS_LNCP_Response_Value_t;

/**
 * Structure that represents the LNS LN Control Point request data.
 */
typedef struct qapi_BLE_LNS_LN_Control_Point_Format_Data_s
{
   /**
    * LN Control Point command type.
    */
   qapi_BLE_LNS_LNCP_Command_Type_t CommandType;
   union
   {
      /**
       * Cumulative value.
       */
      uint8_t  CumulativeValue[3];

      /**
       * Location and speed content mask flags.
       */
      int16_t  LocationSpeedContentMaskFlags;

      /**
       * Navigation and control codes flags.
       */
      uint8_t  NavigationControlCodesFlags;

      /**
       * Name of route.
       */
      uint16_t NameOfRoute;

      /**
       * Selected route.
       */
      uint16_t SelectRoute;

      /**
       * Intended fix rate seconds.
       */
      uint8_t  DesiredFixRateSeconds;

      /**
       * Elevation value.
       */
      int8_t   ElevationValue[3];
   }
   /**
    * Command parameters. @newpagetable
    */
   CommandParameters;
} qapi_BLE_LNS_LN_Control_Point_Format_Data_t;

#define QAPI_BLE_LNS_LN_CONTROL_POINT_FORMAT_DATA_SIZE   (sizeof(qapi_BLE_LNS_LN_Control_Point_Format_Data_t))
/**<
 * Size of the #qapi_BLE_LNS_LN_Control_Point_Format_Data_t structure.
 */

/**
 * Structure that represents the LNS LN Control Point response data.
 *
 * The value of the ResponseCodeOpCode field has the form
 * QAPI_BLE_LNS_LN_CONTROL_POINT_XXX and can be found in
 * qapi_ble_lnstypes.h.
 *
 * The value of the ResponseCode field has the form
 * QAPI_BLE_LNS_LN_CONTROL_POINT_RESPONSE_XXX and can be found in
 * qapi_ble_lnstypes.h.
 *
 * NameOfRoute is a pointer to a NULL-terminated UTF-8 string.
 */
typedef struct qapi_BLE_LNS_LN_Control_Point_Response_Data_s
{
   /**
    * Response code op-code.
    */
   uint8_t                            ResponseCodeOpCode;

   /**
    * Request op-code.
    */
   qapi_BLE_LNS_LNCP_Command_Type_t   RequestOpCode;

   /**
    * Response code value.
    */
   qapi_BLE_LNS_LNCP_Response_Value_t ResponseCode;
   union
   {
      /**
       * Number of routes.
       */
      uint16_t  NumberOfRoutes;

      /**
       * Name of the route.
       */
      char     *NameOfRoute;
   }
   /**
    * Response parameters.
    */
   ResponseParameter;
} qapi_BLE_LNS_LN_Control_Point_Response_Data_t;

#define QAPI_BLE_LNS_LN_CONTROL_POINT_RESPONSE_DATA_SIZE  (sizeof(qapi_BLE_LNS_LN_Control_Point_Response_Data_t))
/**<
 * Size of the #qapi_BLE_LNS_LN_Control_Point_Response_Data_t structure.
 */

/**
 * Structure that represents the LNS Navigation data.
 */
typedef struct qapi_BLE_LNS_Navigation_Data_s
{
   /**
    * Bitmask that indicates the optional fields and features that may
    * be included for the LNS Navigation data.
    *
    * Valid values have the form QAPI_BLE_LNS_NAVIGATION_FLAG_XXX and can
    * be found in qapi_ble_lnstypes.h.
    */
   uint16_t                      Flags;

   /**
    * Bearing.
    */
   uint16_t                      Bearing;

   /**
    * Heading.
    */
   uint16_t                      Heading;

   /**
    * Remaining distance.
    */
   uint8_t                       RemainingDistance[3];

   /**
    * Remaining vertical distance.
    */
   int8_t                        RemainingVerticalDistance[3];

   /**
    * Estimated time of arrival.
    */
   qapi_BLE_LNS_Date_Time_Data_t EstimatedTimeofArrival;
} qapi_BLE_LNS_Navigation_Data_t;

#define QAPI_BLE_LNS_NAVIGATION_DATA_SIZE                (sizeof(qapi_BLE_LNS_Navigation_Data_t))
/**<
 * Size of the #qapi_BLE_LNS_Navigation_Data_t structure.
 */

/**
 * Enumeration that represents the valid Read Request types that a server
 * may receive in a
 * QAPI_BLE_ET_LNS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E or
 * QAPI_BLE_ETLNS_SERVER_CLIENT_CONFIGURATION_UPDATE_E event. This is
 * also used by the qapi_BLE_LNS_Send_Notification() function to  denote
 * the characteristic value to notify.
 *
 * For each event, it is up to the application to return (or write) the
 * correct Client Configuration descriptor based on this value.
 */
typedef enum
{
   QAPI_BLE_CT_LNS_LOCATION_AND_SPEED_E, /**< Location and Speed. */
   QAPI_BLE_CT_LN_CONTROL_POINT_E,       /**< Control Point. */
   QAPI_BLE_CT_NAVIGATION_E              /**< Navigation. */
} qapi_BLE_LNS_Characteristic_Type_t;

/**
 * Enumeration that represents all the events generated by the LNS
 * Profile. These are used to determine the type of each event
 * generated, and to ensure the proper union element is accessed for the
 * #qapi_BLE_LNS_Event_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_ET_LNS_READ_CLIENT_CONFIGURATION_REQUEST_E,
   /**< Read CCCD request event. */
   QAPI_BLE_ET_LNS_CLIENT_CONFIGURATION_UPDATE_E,
   /**< Write CCCD request event. */
   QAPI_BLE_ET_LNS_LN_CONTROL_POINT_COMMAND_E,
   /**< Write Control Point request event. */
   QAPI_BLE_ET_LNS_CONFIRMATION_DATA_E
   /**< Confirmation event. */
} qapi_BLE_LNS_Event_Type_t;

/**
 * Structure that represents the format for the data that is dispatched
 * to an LNS server when an LNS client has sent a request to read an LNS
 * characteristic's CCCD.
 *
 * Some of thr structure fields will be required when responding to a
 * request using the
 * qapi_BLE_LNS_Read_Client_Configuration_Response() function.
 */
typedef struct qapi_BLE_LNS_Read_Client_Configuration_Data_s
{
   /**
    * LNS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the LNS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the LNS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * Bluetooth address of the LNS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the requested CCCD based on the LNS Characteristic
    * type.
    */
   qapi_BLE_LNS_Characteristic_Type_t ClientConfigurationType;
} qapi_BLE_LNS_Read_Client_Configuration_Data_t;

#define QAPI_BLE_LNS_READ_CLIENT_CONFIGURATION_DATA_SIZE  (sizeof(qapi_BLE_LNS_Read_Client_Configuration_Data_t))
/**<
 * Size of the #qapi_BLE_LNS_Read_Client_Configuration_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an LNS server when an LNS client has sent a request to write an
 * LNS characteristic's CCCD.
 */
typedef struct qapi_BLE_LNS_Client_Configuration_Update_Data_s
{
   /**
    * LNS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the LNS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the LNS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * Bluetooth address of the LNS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the requested CCCD based on the LNS Characteristic
    * type.
    */
   qapi_BLE_LNS_Characteristic_Type_t ClientConfigurationType;

   /**
    * CCCD value that has been requested to be written.
    */
   uint16_t                           ClientConfiguration;
} qapi_BLE_LNS_Client_Configuration_Update_Data_t;

#define QAPI_BLE_LNS_CLIENT_CONFIGURATION_UPDATE_DATA_SIZE  (sizeof(qapi_BLE_LNS_Client_Configuration_Update_Data_t))
/**<
 * Size of the #qapi_BLE_LNS_Client_Configuration_Update_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an LNS server when an LNS client has sent a request to write the LNS
 * LN Control Point.
 */
typedef struct qapi_BLE_LNS_LN_Control_Point_Command_Data_s
{
   /**
    * LNS instance that dispatched the event.
    */
   uint32_t                                    InstanceID;

   /**
    * GATT connection ID for the connection with the LNS client
    * that made the request.
    */
   uint32_t                                    ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                                    TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the LNS client.
    */
   qapi_BLE_GATT_Connection_Type_t             ConnectionType;

   /**
    * Bluetooth address of the LNS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                          RemoteDevice;

   /**
    * LNS LN Control Point request data that has been requested to be
    * written
    */
   qapi_BLE_LNS_LN_Control_Point_Format_Data_t FormatData;
} qapi_BLE_LNS_LN_Control_Point_Command_Data_t;

#define QAPI_BLE_LNS_LN_CONTROL_POINT_COMMAND_DATA_SIZE  (sizeof(qapi_BLE_LNS_LN_Control_Point_Command_Data_t))
/**<
 * Size of the #qapi_BLE_LNS_LN_Control_Point_Command_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an LNS server when an LNS client has responded to an outstanding
 * indication that was previously sent by the LNS server.
 */
typedef struct qapi_BLE_LNS_Confirmation_Data_s
{
   /**
    * LNS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the LNS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the LNS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * Bluetooth address of the LNS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;

   /**
    * Status of the indication.
    */
   uint8_t                         Status;
} qapi_BLE_LNS_Confirmation_Data_t;

#define QAPI_BLE_LNS_CONFIRMATION_DATA_SIZE              (sizeof(qapi_BLE_LNS_Confirmation_Data_t))
/**<
 * Size of the #qapi_BLE_LNS_Confirmation_Data_t structure.
 */

/**
 * Structure that represents the container structure for holding all the
 * event data for an LNS instance.
 */
typedef struct qapi_BLE_LNS_Event_Data_s
{
   /**
    * Event type used to determine the appropriate union member of
    * the Event_Data field to access.
    */
   qapi_BLE_LNS_Event_Type_t Event_Data_Type;

   /**
    * Total size of the data contained in the event.
    */
   uint16_t                  Event_Data_Size;
   union
   {
      /**
       * LNS Read CCCD event data.
       */
      qapi_BLE_LNS_Read_Client_Configuration_Data_t   *LNS_Read_Client_Configuration_Data;

      /**
       * LNS Write CCCD event data.
       */
      qapi_BLE_LNS_Client_Configuration_Update_Data_t *LNS_Client_Configuration_Update_Data;

      /**
       * LNS Write LN Control Point event data.
       */
      qapi_BLE_LNS_LN_Control_Point_Command_Data_t    *LNS_LN_Control_Point_Command_Data;

      /**
       * LNS Confirmation event data.
       */
      qapi_BLE_LNS_Confirmation_Data_t                *LNS_Confirmation_Data;
   }
   /**
    * Event data.
    */
   Event_Data;
} qapi_BLE_LNS_Event_Data_t;

#define QAPI_BLE_LNS_EVENT_DATA_SIZE                     (sizeof(qapi_BLE_LNS_Event_Data_t))
/**<
 * Size of the #qapi_BLE_LNS_Event_Data_t structure.
 */

/**
 * @brief
 * This declared type represents the prototype function for
 * an LNS Service Event Receive Data Callback. This function will be called
 * whenever an LNS Service Event occurs that is associated with the
 * specified Bluetooth stack ID.

 * @details
 * The caller should use the contents of the LNS Profile Event Data
 * only in the context of this callback. If the caller requires
 * the data for a longer period of time, the callback function
 * must copy the data into another data buffer.
 *
 * This function is guaranteed not to be invoked more than once
 * simultaneously for the specified installed callback (i.e., this
 * function does not have to be reentrant). It should be noted, however,
 * that if the same callback is installed more than once, the
 * callbacks will be called serially. Because of this, the processing
 * in this function should be as efficient as possible.
 *
 * It should also be noted that this function is called in the Thread
 * Context of a Thread that the user does not own. Therefore, processing
 * in this function should be as efficient as possible (this argument holds
 * anyway because another LNS Service Event will not be processed while this
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
 * @param[in]  LNS_Event_Data        Pointer to a structure that contains
 *                                   information about the event that has
 *                                   occurred.
 *
 * @param[in]  CallbackParameter     User-defined value that was supplied
 *                                   as an input parameter from a prior
 *                                   LNS request.
 *
 * @return None.
 */
typedef void (QAPI_BLE_BTPSAPI *qapi_BLE_LNS_Event_Callback_t)(uint32_t BluetoothStackID, qapi_BLE_LNS_Event_Data_t *LNS_Event_Data, uint32_t CallbackParameter);

   /* LNS server API.                                                   */

/**
 * @brief
 * Opens an LNS server on a specified Bluetooth Stack.
 *
 * @details
 * Only one LNS server may be open at a time, per the Bluetooth Stack ID.
 *
 * All Client Requests will be dispatch to the EventCallback function
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
 *                                  registered LNS service returned from
 *                                  qapi_BLE_GATT_Register_Service() API.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of LNS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_LNS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_LNS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_LNS_Initialize_Service(uint32_t BluetoothStackID, qapi_BLE_LNS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

/**
 * @brief
 * Opens an LNS server on a specified Bluetooth Stack with
 * the ability to control the location of the service in the GATT datalnse.
 *
 * @details
 * Only one1 LNS server may be open at a time, per the Bluetooth Stack ID.
 *
 * All Client Requests will be dispatched to the EventCallback function
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
 *                                       Range structure that, on input
 *                                       can be used to control the
 *                                       location of the service in the
 *                                       GATT database, and on output,
 *                                       returns the handle range that
 *                                       the service is using in the GATT
 *                                       database.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of LNS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_LNS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_LNS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BT_GATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_LNS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, qapi_BLE_LNS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

/**
 * @brief
 * Cleans up and frees all resources
 * associated with an LNS Service Instance.
 *
 * @details
 * After this function is called, no other LNS Service function can be
 * called until after a successful call to the qapi_BLE_LNS_Initialize_Service()
 * function is performed.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of the qapi_BLE_LNS_Initialize_XXX()
 *                                 functions.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_LNS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_LNS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_LNS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * Queries the number of attributes
 * that are contained in the LNS service that is registered with a
 * call to qapi_BLE_LNS_Initialize_Service() or
 * qapi_BLE_LNS_Initialize_Service_Handle_Range().
 *
 * @return   Positive, nonzero, number of attributes that would be
 *           registered by a LNS service instance.
 *
 * @return   Zero on failure.
 */
QAPI_BLE_DECLARATION unsigned int QAPI_BLE_BTPSAPI qapi_BLE_LNS_Query_Number_Attributes(void);

/**
 * @brief
 * Provides a mechanism for an LNS server to
 * successfully respond to a received read client configuration request.
 *
 * @details
 * Possible Events:
 *
 *    QAPI_BLE_ET_LNS_READ_CLIENT_CONFIGURATION_REQUEST_E
 *
 * @param[in]  BluetoothStackID        Unique identifier assigned to this
 *                                     Bluetooth Protocol Stack via a
 *                                     call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID              Service instance ID to close.
 *                                     This is the value that was returned
 *                                     from the qapi_BLE_LNS_Initialize_Service()
 *                                     function.
 *
 * @param[in]  TransactionID           Transaction ID of the original
 *                                     read request. This value was
 *                                     received in the
 *                                     QAPI_BLE_ET_LNS_READ_CLIENT_CONFIGURATION_REQUEST_E
 *                                     event.
 *
 * @param[in]  ClientConfiguration     Specifies the CCCD to send
 *                                     to the remote device.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_LNS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_LNS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_LNS_Read_Client_Configuration_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t ClientConfiguration);

/**
 * @brief
 * Sets the supported LNS features
 * for the LNS server.
 *
 * @details
 * The SupportedFeatures parameter is a bitmask made up of bits of
 * the form QAPI_BLE_LNS_LN_FEATURE_FLAG_XXX.
 *
 * This function must be called after the LNS service is registered with
 * a successful call to qapi_BLE_LNS_Initialize_XXX() in order to set the default
 * features of the LNS server.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_LNS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  SupportedFeatures    Bitmask that contains the
 *                                  supported features that the LNS
 *                                  server will support if this function
 *                                  is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_LNS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_LNS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_LNS_Set_LN_Feature(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t SupportedFeatures);

/**
 * @brief
 * Queries the supported LNS features
 * for the LNS server.
 *
 * @details
 * The SupportedFeatures parameter is a bitmask made up of bits of
 * the form QAPI_BLE_LNS_LN_FEATURE_FLAG_XXX.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_LNS_Initialize_XXX()
 *                                 functions.
 *
 * @param[out]  SupportedFeatures    Pointer to a bitmask that will
 *                                   hold the supported features that the
 *                                   LNS server supports if this function
 *                                   is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_LNS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_LNS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_LNS_Query_LN_Feature(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t *SupportedFeatures);

/**
 * @brief
 * Sends a
 * notification for the LNS Location and Speed to a remote LNS client.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_LNS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the LNS
 *                                 client that will receive the
 *                                 notification.
 *
 * @param[in]  LocationAndSpeedData   Pointer to the LNS Location and
 *                                    Speed data to notify to the remote LNS
 *                                    client.
 *
 * @return      Positive nonzero if successful (represents the
 *              length of the notification).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_LNS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_LNS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_LNS_Notify_Location_And_Speed(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_LNS_Location_Speed_Data_t *LocationAndSpeedData);

/**
 * @brief
 * Sets the Position and Quality on the
 * specified LNS Instance.
 *
 * @details
 * The Flag value of Position_Quality parameter is a bitmask made up of bits of
 * the form QAPI_BLE_LNS_POSITION_QUALITY_FLAG_XXX.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_LNS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  Position_Quality    Bitmask that contains the Position
 *                                 Quality that the LNS server will support if
 *                                 this function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_LNS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_LNS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_LNS_Set_Position_Quality(uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_LNS_Position_Quality_Data_t *Position_Quality);

/**
 * @brief
 * Queries the Position and Quality on the
 * specified LNS Instance.
 *
 * @details
 * The Flag value of Position_Quality parameter is a bitmask made up of bits of
 * the form QAPI_BLE_LNS_POSITION_QUALITY_FLAG_XXX.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_LNS_Initialize_XXX()
 *                                 functions.
 *
 * @param[out]  Position_Quality    Pointer that will hold the Position
 *                                  Quality that the LNS server will support
 *                                  if this function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_LNS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_LNS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_LNS_Query_Position_Quality(uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_LNS_Position_Quality_Data_t *Position_Quality);

/**
 * @brief
 * Sends a
 * notification for the LNS Navigation to a remote LNS client.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_LNS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the LNS
 *                                 client that will receive the
 *                                 notification.
 *
 * @param[in]  Navigation          Pointer to the LNS Navigation data
 *                                 to notify to the remote LNS client.
 *
 * @return      Positive nonzero if successful (represents the
 *              length of the notification).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_LNS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_LNS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_LNS_Notify_Navigation(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_LNS_Navigation_Data_t *Navigation);

/**
 * @brief
 * Responds to a write
 * request from an LNS client for the LNS LN Control Point
 * Characteristic.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * QPAI_BLE_LNS_ERROR_CODE_XXX from qapi_ble_lnstypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * This function is primarily provided to allow a way to reject RACP
 * commands if the corresponding CCCD
 * has not been configured for indications by the
 * LNS client that made the request, a procedure is already in
 * progress, or the LNS client does not have
 * the required security such as authentication, authorization, or
 * encryption.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  ErrorCode           Error code for the request, which
 *                                 indicates whether the request was
 *                                 successful or an error has occured.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_LNS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_LNS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_LNS_LN_Control_Point_Response(uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t ErrorCode);

/**
 * @brief
 * Sends an indication for the
 * LN Control Point request to the LNS client. This function
 * is used to indicate if the LN procedure was successful or an error
 * occured.
 *
 * @details
 * This function sends an indication, which provides a guarantee that the
 * LNS client will receive the value, since the LNS client must confirm
 * that it has been received.
 *
 * This function must not be used if the LNS server is responding to a request
 * for the Number of Routers and the request has been accepted.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the LN Control Point
 * Characteristic that is going to be indicated has been previously configured
 * for indications. An LNS client must have written the LNS LN Control Point
 * Characteristic's CCCD to enable indications.
 *
 * @note1hang
 * Only one indication may be outstanding per LNS Instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_LNS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the
 *                                 connection for the indication.
 *
 * @param[in]  RequestOpCode       Op Code of the request.
 *
 * @param[in]  ResponseCode        Response Code of the procedure
 *                                 result. This indicates whether the procedure
 *                                 was successful or an error occured.
 *
 * @return      Positive nonzero if successful (represents the
 *              GATT Transaction ID for the indication).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_LNS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_LNS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_LNS_Indicate_LN_Control_Point_Result(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_LNS_LNCP_Command_Type_t RequestOpCode, uint8_t ResponseCode);

/**
 * @brief
 * Sends an indication for the LNS
 * Number of Routers. This is in response to an LN Control Point request for
 * the Number of Routes that was successful.
 *
 * @details
 * This function sends an indication, which provides a guarantee that the
 * LNS client will receive the value, since the LNS client must confirm
 * that it has been received.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the LN Control Point
 * Characteristic that is going to be indicated has been previously configured
 * for indications. An LNS client must have written the LNS LN Control Point
 * Characteristic's CCCD to enable indications.
 *
 * @note1hang
 * Only one indication may be outstanding per LNS Instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_LNS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the
 *                                 connection for the indication.
 *
 * @param[in]  RequestOpCode       Op Code of the request.
 *
 * @param[in]  ResponseCode        Response Code of the procedure
 *                                 result. This indicates whether the procedure
 *                                 was successful or an error occured.
 *
 * @param[in]  NumberOfRoutes       Number of Routes to indicate
 *                                  to the LNS client.
 *
 * @return      Positive nonzero if successful (represents the
 *              GATT Transaction ID for the indication).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_LNS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_LNS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_LNS_Indicate_Number_Of_Routes(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_LNS_LNCP_Command_Type_t RequestOpCode, uint8_t ResponseCode, uint16_t NumberOfRoutes);

/**
 * @brief
 * Sends an indication for the LNS
 * Name of Route. This is in response to an LN Control Point request for the
 * Name of Route that was successful.
 *
 * @details
 * This function sends an indication, which provides a guarantee that the
 * LNS client will receive the value, since the LNS client must confirm
 * that it has been received.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the LN Control Point
 * Characteristic that is going to be indicated has been previously configured
 * for indications. An LNS client must have written the LNS LN Control Point
 * Characteristic's CCCD to enable indications.
 *
 * @note1hang
 * Only one indication may be outstanding per LNS Instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_LNS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the
 *                                 connection for the indication.
 *
 * @param[in]  RequestOpCode       Op Code of the request.
 *
 * @param[in]  ResponseCode        Response Code of the procedure
 *                                 result. This indicates whether the procedure
 *                                 was successful or an error occured.
 *
 * @param[in]  NameOfRoute         Name of the Route to indicate
 *                                 to the LNS client.
 *
 * @return      Positive nonzero if successful (represents the
 *              GATT Transaction ID for the indication).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_LNS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_LNS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_LNS_Indicate_Name_Of_Route(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_LNS_LNCP_Command_Type_t RequestOpCode, uint8_t ResponseCode, char *NameOfRoute);

   /* LNS client API.                                                   */

/**
 * @brief
 * Formats the LNS LN
 * Control Point Command into a user-specified buffer for a GATT Write
 * request that will be sent to the LNS server.
 *
 * @details
 * The third parameter BufferLength is the size of input request and it
   will hold the size of output Buffer after formatting.
 *
 * @param[in]  FormatData            Pointer to the RACP Request data
 *                                   that will be formatted into the
 *                                   user-specified buffer.
 *
 * @param[in, out]  BufferLength     Pointer that, on input, will hold
 *                                   the length of the user-specified
 *                                   buffer and, on output, hold the
 *                                   utilized size of the formatted
 *                                   buffer.
 *
 * @param[out] Buffer                User-specified buffer that will
 *                                   hold the formatted data if this
 *                                   function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_LNS_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_LNS_Format_LN_Control_Point_Command(qapi_BLE_LNS_LN_Control_Point_Format_Data_t *FormatData, uint32_t *BufferLength, uint8_t *Buffer);

/**
 * @brief
 * Parses a value received in an
 * indication from a remote LNS server, interpreting it as the LNS LN
 * Control Point response.
 *
 * @details
 * qapi_BLE_LNS_Free_LN_Control_Point_Response() should be called after
 * LNCPResponseData has been processed by the application. A memory leak
 * may occur if this function is not called.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            LNS server.
 *
 * @param[in]  Value          Value received from the
 *                            LNS server.
 *
 * @param[out]  LNCPResponseData    Pointer that will hold the LN
 *                                  Control Point response data if this
 *                                  function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_LNS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_LNS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_LNS_Decode_LN_Control_Point_Response(uint32_t ValueLength, uint8_t *Value, qapi_BLE_LNS_LN_Control_Point_Response_Data_t *LNCPResponseData);

   /* LNS client API.                                                  */

/**
 * @brief
 * Provides a mechanism to free
 * all resources that were allocated to parse a Raw Control Point
 * Response into a Parsed Control Point Response. See the
 * qapi_BLE_LNS_Decode_LN_Control_Point_Response() function for more
 * information.
 *
 * @param[in]  LNCPResponseData    Pointer to the LN Control Point
 *                                 response data.
 *
 * @return      None.
 */
QAPI_BLE_DECLARATION void QAPI_BLE_BTPSAPI qapi_BLE_LNS_Free_LN_Control_Point_Response(qapi_BLE_LNS_LN_Control_Point_Response_Data_t *LNCPResponseData);

/**
 * @brief
 * Parses a value received in an
 * indication from a remote LNS server, interpreting it as the LNS Location
 * and Speed data.
 *
 * @details
 * It is possible that the notification for this characteristic may have
 * only been partially received if the MTU size is not large enough to
 * contain the entire characteristic. If this function detects that the
 * value buffer passed in is not the proper size, the function will
 * return a positive nonzero value that indicates the expected size of
 * the data field. If this occurs, the application should store
 * the temporary buffer and concatenate the next Location and Speed
 * notifications until the correct buffer size is received.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            LNS server.
 *
 * @param[in]  Value          Value received from the
 *                            LNS server.
 *
 * @param[out]  LocationAndSpeedData    Pointer that will hold the
 *                                      decoded LNS Location and Speed
 *                                      data if this function is
 *                                      successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_LNS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_LNS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_LNS_Decode_Location_And_Speed(uint32_t ValueLength, uint8_t *Value, qapi_BLE_LNS_Location_Speed_Data_t *LocationAndSpeedData);

/**
 * @brief
 * Parses a value received in an
 * indication from a remote LNS server, interpreting it as the LNS
 * Navigation data.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            LNS server.
 *
 * @param[in]  Value          Value received from the
 *                            LNS server.
 *
 * @param[out]  NavigationData    Pointer that will hold the decoded
 *                                LNS Navigation data if this
 *                                function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_LNS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_LNS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_LNS_Decode_Navigation(uint32_t ValueLength, uint8_t *Value, qapi_BLE_LNS_Navigation_Data_t *NavigationData);

/**
 * @brief
 * Parses a value received in an
 * indication from a remote LNS server, interpreting it as the LNS Position
 * and Quality data.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            LNS server.
 *
 * @param[in]  Value          Value received from the
 *                            LNS server.
 *
 * @param[out]  PositionQualityData    Pointer that will hold the
 *                                     decoded LNS Position and Quality
 *                                     data if this function is
 *                                     successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_LNS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_LNS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_LNS_Decode_Position_Quality(uint32_t ValueLength, uint8_t *Value, qapi_BLE_LNS_Position_Quality_Data_t *PositionQualityData);

/**
 * @}
 */

#endif

