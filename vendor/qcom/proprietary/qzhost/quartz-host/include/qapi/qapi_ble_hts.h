/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_ble_hts.h
 *
 * @brief
 * QCA QAPI for Bluetopia Bluetooth Health Thermometer Service (HTS)
 * (GATT based) API Type Definitions, Constants, and Prototypes.
 *
 * @details
 * The Health Thermometer Service programming interface defines the protocols and
 * procedures to be used to implement the Health Thermometer Service
 * capabilities.
 */

#ifndef __QAPI_BLE_HTS_H__
#define __QAPI_BLE_HTS_H__

#include "./qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.          */
#include "./qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.    */
#include "./qapi_ble_gatt.h"      /* QAPI GATT prototypes.                    */
#include "./qapi_ble_htstypes.h"  /* QAPI HTS prototypes.                     */

/**
 * @addtogroup qapi_ble_services
 *
 * @{
 */

   /* Error Return Codes.                                               */

   /* Error Codes that are smaller than these (less than -1000) are     */
   /* related to the Bluetooth Protocol Stack itself (see               */
   /* qapi_ble_errors.h).                                               */
#define QAPI_BLE_HTS_ERROR_INVALID_PARAMETER             (-1000)
/**< Invalid parameter. */
#define QAPI_BLE_HTS_ERROR_INVALID_BLUETOOTH_STACK_ID    (-1001)
/**< Invalid Bluetooth Stack ID. */
#define QAPI_BLE_HTS_ERROR_INSUFFICIENT_RESOURCES        (-1002)
/**< Insufficient resources. */
#define QAPI_BLE_HTS_ERROR_SERVICE_ALREADY_REGISTERED    (-1003)
/**< Service is already registerd. */
#define QAPI_BLE_HTS_ERROR_INVALID_INSTANCE_ID           (-1004)
/**< Invalid service instance ID. */
#define QAPI_BLE_HTS_ERROR_MALFORMATTED_DATA             (-1005)
/**< Malformatted data. */
#define QAPI_BLE_HTS_ERROR_INDICATION_OUTSTANDING        (-1006)
/**< Indication is outstanding. */
#define QAPI_BLE_HTS_ERROR_UNKNOWN_ERROR                 (-1007)
/**< Unknown error. */

/**
 * Structure that represents the Date/Time data for HTS.
 */
typedef struct qapi_BLE_HTS_Time_Stamp_Data_s
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
} qapi_BLE_HTS_Time_Stamp_Data_t;

#define QAPI_BLE_HTS_TIME_STAMP_DATA_SIZE                (sizeof(qapi_BLE_HTS_Time_Stamp_Data_t))
/**<
 * Size of the #qapi_BLE_HTS_Time_Stamp_Data_t structure.
 */

#define QAPI_BLE_HTS_TIME_STAMP_VALID_TIME_STAMP(_x)     ((QAPI_BLE_GATT_DATE_TIME_VALID_YEAR(((_x)).Year)) && (QAPI_BLE_GATT_DATE_TIME_VALID_MONTH(((_x)).Month)) && (QAPI_BLE_GATT_DATE_TIME_VALID_DAY(((_x)).Day)) && (QAPI_BLE_GATT_DATE_TIME_VALID_HOURS(((_x)).Hours)) && (QAPI_BLE_GATT_DATE_TIME_VALID_MINUTES(((_x)).Minutes)) && (QAPI_BLE_GATT_DATE_TIME_VALID_SECONDS(((_x)).Seconds)))
/**<
 * This macro is a utility macro that exists to valid that a specified
 * Time Stamp is valid. The only parameter to this function is the
 * qapi_BLE_HTS_Time_Stamp_t structure. This macro returns TRUE
 * if the Time Stamp is valid or FALSE otherwise..
 */

/**
 * Structure that represents the format of the Valid Range value that can
 * be set.
 */
typedef struct qapi_BLE_HTS_Valid_Range_Data_s
{
   /**
    * Lower bound.
    */
   uint16_t Lower_Bounds;

   /**
    * Upper bound.
    */
   uint16_t Upper_Bounds;
} qapi_BLE_HTS_Valid_Range_Data_t;

#define QAPI_BLE_HTS_VALID_RANGE_DATA_SIZE               (sizeof(qapi_BLE_HTS_Valid_Range_Data_t))
/**<
 * Size of the #qapi_BLE_HTS_Valid_Range_Data_t structure.
 */

/**
 * Structure that represents the format of an HTS Temperature
 * measurement.
 */
typedef struct qapi_BLE_HTS_Temperature_Measurement_Data_s
{
   /**
    * Bitmask that indicates the optional fields and features that may
    * be included for a HTS Measurement.
    *
    * Valid values have the form
    * QAPI_BLE_HTS_TEMPERATURE_MEASUREMENT_FLAGS_XXX and can be found in
    * qapi_ble_htstypes.h.
    */
   uint8_t                         Flags;

   /**
    * Temperature.
    */
   qapi_BLE_HTS_Temperature_Data_t Temperature;

   /**
    * Time stamp data.
    */
   qapi_BLE_HTS_Time_Stamp_Data_t  Time_Stamp;

   /**
    * Temperature type.
    */
   uint8_t                         Temperature_Type;
} qapi_BLE_HTS_Temperature_Measurement_Data_t;

#define QAPI_BLE_HTS_TEMPERATURE_MEASUREMENT_DATA_SIZE   (sizeof(qapi_BLE_HTS_Temperature_Measurement_Data_t))
/**<
 * Size of the #qapi_BLE_HTS_Temperature_Measurement_Data_t structure.
 */

/**
 * Enumeration that represents the valid Read Request types that a server
 * may receive in a
 * QAPI_BLE_ET_HTS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E or
 * QAPI_BLE_ET_HTS_SERVER_CLIENT_CONFIGURATION_UPDATE_E event. This is
 * also used by the qapi_BLE_HTS_Send_Notification() to denote the
 * characteristic value to notify.
 *
 * For each event, it is up to the application to return (or write) the
 * correct Client Configuration descriptor based on this value.
 */
typedef enum
{
   QAPI_BLE_CT_TEMPERATURE_MEASUREMENT_E,
   /**< Temperature Measurement. */
   QAPI_BLE_CT_INTERMEDIATE_TEMPERATURE_E,
   /**< Intermediate Temperature. */
   QAPI_BLE_CT_MEASUREMENT_INTERVAL_E
   /**< Measurement Interval. */
} qapi_BLE_HTS_Characteristic_Type_t;

/**
 * Enumeration that represents all the events generated by the HTS
 * service. These are used to determine the type of each event
 * generated, and to ensure the proper union element is accessed for the
 * #qapi_BLE_HTS_Event_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_ET_HTS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E,
   /**< Read CCCD request event. */
   QAPI_BLE_ET_HTS_SERVER_CLIENT_CONFIGURATION_UPDATE_E,
   /**< Write CCCD request event. */
   QAPI_BLE_ET_HTS_MEASUREMENT_INTERVAL_UPDATE_E,
   /**< Write Measurement Interval request event. */
   QAPI_BLE_ET_HTS_CONFIRMATION_RESPONSE_E
   /**< Confirmation event. */
} qapi_BLE_HTS_Event_Type_t;

/**
 * Structure that contains the attribute handles that will need to be
 * cached by an HTS client in order to only do service discovery once.
 */
typedef struct qapi_BLE_HTS_Client_Information_s
{
   /**
    * Temperature Measurement attribute handle.
    */
   uint16_t Temperature_Measurement;

   /**
    * Temperature Measurement CCCD attribute handle.
    */
   uint16_t Temperature_Measurement_Client_Configuration;

   /**
    * Temperature Type attribute handle.
    */
   uint16_t Temperature_Type;

   /**
    * Intermediate Temperature attribute handle.
    */
   uint16_t Intermediate_Temperature;

   /**
    * Intermediate Temperature CCCD attribute handle.
    */
   uint16_t Intermediate_Temperature_Client_Configuration;

   /**
    * Measurement Interval attribute handle.
    */
   uint16_t Measurement_Interval;

   /**
    * Measurement Interval CCCD attribute handle.
    */
   uint16_t Measurement_Interval_Client_Configuration;

   /**
    * Measurement Interval Valid Range attribute handle.
    */
   uint16_t Measurement_Interval_Valid_Range_Descriptor;
} qapi_BLE_HTS_Client_Information_t;

#define QAPI_BLE_HTS_CLIENT_INFORMATION_DATA_SIZE        (sizeof(qapi_BLE_HTS_Client_Information_t))
/**<
 * Size of the #qapi_BLE_HTS_Client_Information_t structure.
 */

/**
 * Structure that contains all of the per client data that will need to
 * be stored by an HTS server.
 */
typedef struct qapi_BLE_HTS_Server_Information_s
{
    /**
    * Temperature Measurement CCCD.
    */
   uint16_t Temperature_Measurement_Client_Configuration;

    /**
    * Intermediate Temperature CCCD.
    */
   uint16_t Intermediate_Temperature_Client_Configuration;

   /**
    * Measurement Interval CCCD.
    */
   uint16_t Measurement_Interval_Client_Configuration;
} qapi_BLE_HTS_Server_Information_t;

#define QAPI_BLE_HTS_SERVER_INFORMATION_DATA_SIZE        (sizeof(qapi_BLE_HTS_Server_Information_t))
/**<
 * Size of the #qapi_BLE_HTS_Server_Information_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an HTS server when an HTS client has sent a request to read an HTS
 * characteristic's CCCD.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_HTS_Read_Client_Configuration_Response() function.
 */
typedef struct qapi_BLE_HTS_Read_Client_Configuration_Data_s
{
   /**
    * HTS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the HTS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the HTS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * Bluetooth address of the HTS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the requested CCCD based on the HTS Characteristic
    * type.
    */
   qapi_BLE_HTS_Characteristic_Type_t ClientConfigurationType;
} qapi_BLE_HTS_Read_Client_Configuration_Data_t;

#define QAPI_BLE_HTS_READ_CLIENT_CONFIGURATION_DATA_SIZE  (sizeof(qapi_BLE_HTS_Read_Client_Configuration_Data_t))
/**<
 * Size of the #qapi_BLE_HTS_Read_Client_Configuration_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an HTS server when an HTS client has sent a request to write an HTS
 * characteristic's CCCD.
 */
typedef struct qapi_BLE_HTS_Client_Configuration_Update_Data_s
{
   /**
    * HTS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the HTS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the HTS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * Bluetooth address of the HTS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the requested CCCD based on the HTS Characteristic
    * type.
    */
   qapi_BLE_HTS_Characteristic_Type_t ClientConfigurationType;

   /**
    * CCCD value that has been requested to be written.
    */
   uint16_t                           ClientConfiguration;
} qapi_BLE_HTS_Client_Configuration_Update_Data_t;

#define QAPI_BLE_HTS_CLIENT_CONFIGURATION_UPDATE_DATA_SIZE  (sizeof(qapi_BLE_HTS_Client_Configuration_Update_Data_t))
/**<
 * Size of the #qapi_BLE_HTS_Client_Configuration_Update_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an HTS server when an HTS client has sent a request to write the HTS
 * Measurement Interval.
 */
typedef struct qapi_BLE_HTS_Measurement_Interval_Update_Data_s
{
   /**
    * HTS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the HTS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the HTS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * Bluetooth address of the HTS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;

   /**
    * Measurement Interval that has been requested to be written.
    */
   uint16_t                        NewMeasurementInterval;
} qapi_BLE_HTS_Measurement_Interval_Update_Data_t;

#define QAPI_BLE_HTS_MEASUREMENT_INTERVAL_UPDATE_DATA_SIZE  (sizeof(qapi_BLE_HTS_Measurement_Interval_Update_Data_t))
/**<
 * Size of the #qapi_BLE_HTS_Measurement_Interval_Update_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to aHTS server when aHTS client has responded to an outstanding
 * indication that was previously sent by the HTS server.
 *
 * The Characteristic_Type parameter will never be set to
 * QAPI_BLE_CT_INTERMEDIATE_TEMPERATURE_E for this event.
 */
typedef struct qapi_BLE_HTS_Confirmation_Data_s
{
   /**
    * HTS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the HTS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the HTS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * Bluetooth address of the HTS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the HTS Characteristic that was previously indicated.
    */
   qapi_BLE_HTS_Characteristic_Type_t Characteristic_Type;

   /**
    * Status of the indication.
    */
   uint8_t                            Status;
} qapi_BLE_HTS_Confirmation_Data_t;

#define QAPI_BLE_HTS_CONFIRMATION_DATA_SIZE              (sizeof(qapi_BLE_HTS_Confirmation_Data_t))
/**<
 * Size of the #qapi_BLE_HTS_Confirmation_Data_t structure.
 */

/**
 * Structure that represents the container structure for holding all the
 * event data for an HTS instance.
 */
typedef struct qapi_BLE_HTS_Event_Data_s
{
   /**
    * Event type used to determine the appropriate union member of
    * the Event_Data field to access.
    */
   qapi_BLE_HTS_Event_Type_t Event_Data_Type;

   /**
    * Total size of the data contained in the event.
    */
   uint16_t                  Event_Data_Size;
   union
   {
      /**
       * HTS Read CCCD data.
       */
      qapi_BLE_HTS_Read_Client_Configuration_Data_t   *HTS_Read_Client_Configuration_Data;

      /**
       * HTS Write CCCD data.
       */
      qapi_BLE_HTS_Client_Configuration_Update_Data_t *HTS_Client_Configuration_Update_Data;

      /**
       * HTS Write Measurement Interval data.
       */
      qapi_BLE_HTS_Measurement_Interval_Update_Data_t *HTS_Measurement_Interval_Update_Data;

      /**
       * HTS Confirmation data.
       */
      qapi_BLE_HTS_Confirmation_Data_t                *HTS_Confirmation_Data;
   }
   /**
    * Event data. @newpagetable
    */
   Event_Data;
} qapi_BLE_HTS_Event_Data_t;

#define QAPI_BLE_HTS_EVENT_DATA_SIZE                     (sizeof(qapi_BLE_HTS_Event_Data_t))
/**<
 * Size of the #qapi_BLE_HTS_Event_Data_t structure.
 */

/**
 * @brief
 * This declared type represents the Prototype Function for an
 * HTS Event Callback. This function will be called whenever a define
 * HTS Event occurs within the Bluetooth Protocol Stack that is
 * specified with the specified Bluetooth Stack ID.
 *
 * @details
 * The event information is passed to the user in a #qapi_BLE_HTS_Event_Data_t
 * structure. This structure contains all the information about the
 * event that occurred.
 *
 * The caller should use the contents of the HTS Event Data
 * only in the context of this callback. If the caller requires
 * the data for a longer period of time, the callback function
 * must copy the data into another data buffer.
 *
 * This function is guaranteed not to be invoked more than once
 * simultaneously for the specified installed callback (i.e., this
 * function does not have be reentrant). It should be noted however,
 * that if the same callback is installed more than once, the
 * callbacks will be called serially. Because of this, the processing
 * in this function should be as efficient as possible.
 *
 * It should also be noted that this function is called in the Thread
 * Context of a Thread that the user does not own. Therefore, processing
 * in this function should be as efficient as possible (this argument holds
 * anyway because another HTS Event will not be processed while this
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
 * @param[in]  HTS_Event_Data        Pointer to a structure that contains
 *                                   information about the event that has
 *                                   occurred.
 *
 * @param[in]  CallbackParameter     User-defined value that will be
 *                                   received with the HTS Event data.
 *
 * @return None.
 */
typedef void (QAPI_BLE_BTPSAPI *qapi_BLE_HTS_Event_Callback_t)(uint32_t BluetoothStackID, qapi_BLE_HTS_Event_Data_t *HTS_Event_Data, uint32_t CallbackParameter);

   /* HTS server API.                                                   */

/**
 * @brief
 * Opens an HTS server on a specified Bluetooth Stack.
 *
 * @details
 * Only one HTS server may be open at a time, per the Bluetooth Stack ID.
 *
 * All client requests will be dispatch to the EventCallback function
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
 *                                  registered HTS service returned from
 *                                  qapi_BLE_GATT_Register_Service() API.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of HTS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HTS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_HTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BT_GATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HTS_Initialize_Service(uint32_t BluetoothStackID, qapi_BLE_HTS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

/**
 * @brief
 * Opens an HTS server on a specified Bluetooth Stack with
 * the ability to control the location of the service in the GATT datahtse.
 *
 * @details
 * Only one HTS server may be open at a time, per the Bluetooth Stack ID.
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
 * @param[in]      ServiceID             Unique GATT service ID of the
 *                                       registered HTS service returned
 *                                       from
 *                                       qapi_BLE_GATT_Register_Service()
 *                                       API.
 *
 * @param[in,out]  ServiceHandleRange    Pointer to a Service Handle
 *                                       Range structure that, on input
 *                                       can be used to control the
 *                                       location of the service in the
 *                                       GATT datahtse, and on output
 *                                       returns the handle range that
 *                                       the service is using in the GATT
 *                                       datahtse.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of HTS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HTS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_HTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BT_GATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HTS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, qapi_BLE_HTS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

/**
 * @brief
 * Cleans up and frees all resources
 * associated with an HTS Service Instance.
 *
 * @details
 * After this function is called, no other HTS Service function can be
 * called until after a successful call to the qapi_BLE_HTS_Initialize_Service()
 * function is performed.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from the qapi_BLE_HTS_Initialize_Service()
 *                                 function.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HTS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HTS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * Queries the number of attributes
 * that are contained in the HTS service that is registered with a
 * call to qapi_BLE_HTS_Initialize_Service() or
 * qapi_BLE_HTS_Initialize_Service_Handle_Range().
 *
 * @return   Positive, nonzero, number of attributes that would be
 *           registered by a HTS service instance.
 *
 * @return   Zero on failure.
 */
QAPI_BLE_DECLARATION unsigned int QAPI_BLE_BTPSAPI qapi_BLE_HTS_Query_Number_Attributes(void);

/**
 * @brief
 * Sets the HTS Temperature Type on the
 * the HTS server.
 *
 * @details
 * The Temperature_Type parameter should be an enumerated value of
 * the form QAPI_BLE_HTS_TEMPERATURE_TYPE_XXX.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_HTS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  Temperature_Type    Enumeration for the HTS Temperature
 *                                 Type that will be set if this function
 *                                 is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HTS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HTS_Set_Temperature_Type(uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t Temperature_Type);

/**
 * @brief
 * Queries the HTS Temperature Type on
 * the the HTS server.
 *
 * @details
 * The Temperature_Type parameter should be an enumerated value of
 * the form QAPI_BLE_HTS_TEMPERATURE_TYPE_XXX.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_HTS_Initialize_XXX()
 *                                 functions.
 *
 * @param[out]  Temperature_Type    Pointer that will hold an
 *                                  enumerated value for the HTS
 *                                  Temperature Type if this function is
 *                                  successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HTS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HTS_Query_Temperature_Type(uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t *Temperature_Type);

/**
 * @brief
 * Sets the HTS Measurement Interval on
 * the the HTS server.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_HTS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  Measurement_Interval    Measurement Interval that will
 *                                     be set if this function is
 *                                     successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HTS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HTS_Set_Measurement_Interval(uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t Measurement_Interval);

/**
 * @brief
 * Queries the HTS Measurement Interval
 * on the the HTS server.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_HTS_Initialize_XXX()
 *                                 functions.
 *
 * @param[out]  Measurement_Interval    Pointer that will hold the HTS
 *                                      Measurement Interval if this
 *                                      function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HTS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HTS_Query_Measurement_Interval(uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t *Measurement_Interval);

/**
 * @brief
 * Sets the HTS Valid Range on the the
 * HTS server.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_HTS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ValidRange          Pointer to the Valid Range that will
 *                                 be set if this function is
 *                                 successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HTS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HTS_Set_Valid_Range(uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_HTS_Valid_Range_Data_t *ValidRange);

/**
 * @brief
 * Queries the HTS Valid Range
 * on the the HTS server.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_HTS_Initialize_XXX()
 *                                 functions.
 *
 * @param[out]  ValidRange         Pointer that will hold the HTS
 *                                 Valid Range if this
 *                                 function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HTS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HTS_Query_Valid_Range(uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_HTS_Valid_Range_Data_t *ValidRange);

/**
 * @brief
 * Provides a mechanism for an HTS server to
 * successfully respond to a received read client configuration request.
 *
 * @details
 * Possible Events:
 *
 *    QAPI_BLE_ET_HTS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E
 *
 * @param[in]  BluetoothStackID        Unique identifier assigned to this
 *                                     Bluetooth Protocol Stack via a
 *                                     call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID              Service instance ID to close.
 *                                     This is the value that was returned
 *                                     from the qapi_BLE_HTS_Initialize_Service()
 *                                     function.
 *
 * @param[in]  TransactionID           Transaction ID of the original
 *                                     read request. This value was
 *                                     received in the
 *                                     QAPI_BLE_ET_HTS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E
 *                                     event.
 *
 * @param[in]  Client_Configuration    Specifies the CCCD to send
 *                                     to the remote device.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HTS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HTS_Read_Client_Configuration_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t Client_Configuration);

/**
 * @brief
 * Sends an HTS Intermediate Temperature
 * Measurement notification to an HTS client.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of the
 *                                 qapi_BLE_HTS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the HTS
 *                                 Client that will receive the
 *                                 notification.
 *
 * @param[in]  Temperature_Measurement    Pointer to the HTS
 *                                        Intermediate Temperature
 *                                        Measurement data that will be
 *                                        notified to the HTS client.
 *
 * @return      Positive nonzero if successful (represents the
 *              length of the notification).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HTS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HTS_Notify_Intermediate_Temperature(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_HTS_Temperature_Measurement_Data_t *Temperature_Measurement);

/**
 * @brief
 * Sends an indication for the HTS Measurement Interval.
 *
 * @details
 * This function sends an indication, which provides a guarantee that the
 * HTS client will receive the value since the HTS client must confirm
 * that it has been received.
 *
 * @note1hang
 * Only one indication may be outstanding per HTS instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_HTS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the
 *                                 connection for the indication.
 *
 * @return      Positive nonzero if successful (represents the
 *              GATT Transaction ID for the indication).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HTS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HTS_Indicate_Measurement_Interval(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID);

/**
 * @brief
 * Sends an indication for an
 * HTS Temperature Measurement.
 *
 * @details
 * This function sends an indication, which provides a guarantee that the
 * HTS client will receive the value since the HTS client must confirm
 * that it has been received.
 *
 * @note1hang
 * Only one indication may be outstanding per HTS Instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_HTS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the
 *                                 connection for the indication.
 *
 * @param[in]  Temperature_Measurement    Pointer to the HTS
 *                                        Temperature Measurement that
 *                                        will be indicated to the HTS
 *                                        client.
 *
 * @return      Positive nonzero if successful (represents the
 *              GATT Transaction ID for the indication).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HTS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HTS_Indicate_Temperature_Measurement(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_HTS_Temperature_Measurement_Data_t *Temperature_Measurement);

   /* HTS Client API.                                                   */

/**
 * @brief
 * Parses a value received in an
 * indication from a remote HTS server, interpreting it as the HTS
 * Temperature Measurement data.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            HTS server.
 *
 * @param[in]  Value          Value received from the
 *                            HTS server.
 *
 * @param[out]  TemperatureMeasurement    Pointer that will hold the
 *                                        decoded HTS Temperature
 *                                        Measurement data if this function
 *                                        is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HTS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HTS_Decode_Temperature_Measurement(uint32_t ValueLength, uint8_t *Value, qapi_BLE_HTS_Temperature_Measurement_Data_t *TemperatureMeasurement);

/**
 * @brief
 * Parses a value received in an
 * indication from a remote HTS server, interpreting it as the HTS Valid
 * Range data.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            HTS server.
 *
 * @param[in]  Value          Value received from the
 *                            HTS server.
 *
 * @param[out]  ValidRange    Pointer that will hold the decoded HTS
 *                            Valid Range data if this function is
 *                            successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HTS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HTS_Decode_Valid_Range(uint32_t ValueLength, uint8_t *Value, qapi_BLE_HTS_Valid_Range_Data_t *ValidRange);

/**
 * @}
 */

#endif

