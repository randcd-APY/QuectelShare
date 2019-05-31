/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_ble_hrs.h
 *
 * @brief
 * QCA QAPI for Bluetopia Bluetooth Heart Rate Service
 * (GATT based) API Type Definitions, Constants, and
 * Prototypes.
 *
 * @details
 * The Heart Rate Service (HRS) programming interface defines the
 * protocols and procedures to be used to implement the Heart Rate
 * Service capabilities.
 */

#ifndef __QAPI_BLE_HRS_H__
#define __QAPI_BLE_HRS_H__

#include "./qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.          */
#include "./qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.    */
#include "./qapi_ble_gatt.h"      /* qapi GATT prototypes.                    */
#include "./qapi_ble_hrstypes.h"  /* QAPI HRS prototypes.                     */

/** @addtogroup qapi_ble_services
@{
*/

   /* Error Return Codes.                                               */

   /* Error Codes that are smaller than these (less than -1000) are     */
   /* related to the Bluetooth Protocol Stack itself (see               */
   /* qapi_ble_errros.h).                                               */
#define QAPI_BLE_HRS_ERROR_INVALID_PARAMETER             (-1000)
/**< Invalid parameter. */
#define QAPI_BLE_HRS_ERROR_INVALID_BLUETOOTH_STACK_ID    (-1001)
/**< Invalid Bluetooth Stack ID. */
#define QAPI_BLE_HRS_ERROR_INSUFFICIENT_RESOURCES        (-1002)
/**< Insufficient resourcest. */
#define QAPI_BLE_HRS_ERROR_SERVICE_ALREADY_REGISTERED    (-1003)
/**< Service is already registered. */
#define QAPI_BLE_HRS_ERROR_INVALID_INSTANCE_ID           (-1004)
/**< Invalid service instance ID. */
#define QAPI_BLE_HRS_ERROR_MALFORMATTED_DATA             (-1005)
/**< Malformatted data. */
#define QAPI_BLE_HRS_ERROR_INDICATION_OUTSTANDING        (-1006)
/**< Indication is outstanding. */
#define QAPI_BLE_HRS_ERROR_UNKNOWN_ERROR                 (-1007)
/**< Unknown error. */

   /* The following flags specifiy that Heart Rate Control Point        */
   /* commands that are to be supported by a Heart Rate instance.  These*/
   /* will be passed into either of the qapi_BLE_HRS_Initialize_XXX()   */
   /* functions.                                                        */
#define QAPI_BLE_HRS_HEART_RATE_CONTROL_POINT_RESET_ENERGY_EXPENDED_SUPPORTED  0x01
/**<
 * Indicates whether HRS supports the Reset Enery Expended feature for the HRS
 * Control Point.
 */

/**
 * Structure that represents the format for a Heart Rate Measurement.
 *
 * The Heart_Rate member that is a word in the structure is
 * truncated to a byte value unless the following flag is specified in
 * the Flags member:
 * QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_FLAGS_HEART_RATE_IS_WORD
 *
 * The Energy_Expended member of this structure is ignored unless the
 * following flag is specified in the Flags member:
 * QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_FLAGS_ENERGY_EXPENDED_PRESENT.
 *
 * The Number_Of_RR_Intervals and RR_Intervals members are ignored
 * unless the following flag is specified in the Flags member:
 * QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_FLAGS_RR_INTERVAL_PRESENT. If this
 * flag is specified in the Flags member, there must be at least one
 * entry in the RR_Intervals array when notifying the Heart Rate
 * Measurement value. When decoding a received Heart Rate Measurement,
 * this field may bet set to 0.
 */
typedef struct qapi_BLE_HRS_Heart_Rate_Measurement_Data_s
{
   /**
    * Bitmask that indicates the optional fields and features that may
    * be included for a HRS Measurement.
    *
    * Valid values have the form
    * QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_FLAGS_XXX and can be found in
    * qapi_ble_hrstypes.h.
    */
   uint8_t  Flags;

   /**
    * Heart rate
    */
   uint16_t Heart_Rate;

   /**
    * Energy expended
    */
   uint16_t Energy_Expended;

   /**
    * Number of RR intervals
    */
   uint16_t Number_Of_RR_Intervals;

   /**
    * RR intervals
    */
   uint16_t RR_Intervals[1];
} qapi_BLE_HRS_Heart_Rate_Measurement_Data_t;

#define QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_DATA_SIZE(_x)  (QAPI_BLE_BTPS_STRUCTURE_OFFSET(qapi_BLE_HRS_Heart_Rate_Measurement_Data_t, RR_Intervals) + ((_x) * sizeof(uint16_t)))
/**<
 * This macro is a utility macro that exists to aid in determining the
 * amount of memory that is needed to hold a Heart Rate Measurement with
 * a certain number of RR Interval values present. The only parameter to
 * this macro is the number of RR Intervals that will be present in the
 * Heart Rate Measurement..
 */

/**
 * Enumeration that represents the valid Read Request types that a server
 * may receive in a
 * QAPI_BLE_ET_HRS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E or
 * QAPI_BLE_ET_HRS_SERVER_CLIENT_CONFIGURATION_UPDATE_E event.
 *
 * For each event, it is up to the application to return (or write) the
 * correct Client Configuration descriptor based on this value.
 */
typedef enum
{
   QAPI_BLE_CT_HEART_RATE_MEASUREMENT_E /**< Heart Rate Measurement. */
} qapi_BLE_HRS_Characteristic_Type_t;

/**
 * Enumeration that represents the valid Heart Rate Control Point
 * commands that may be sent or received.
 */
typedef enum
{
   QAPI_BLE_CC_RESET_ENERGY_EXPENDED_E = QAPI_BLE_HRS_HEART_RATE_CONTROL_POINT_RESET_ENERGY_EXPENDED
   /**< Reset Enerty Expended op code. */
} qapi_BLE_HRS_Heart_Rate_Control_Command_t;

/**
 * Enumeration that represents all the events generated by the HRS
 * service. These are used to determine the type of each event
 * generated, and to ensure the proper union element is accessed for the
 * #qapi_BLE_HRS_Event_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_ET_HRS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E,
   /**< Read CCCD request event. */
   QAPI_BLE_ET_HRS_SERVER_CLIENT_CONFIGURATION_UPDATE_E,
   /**< Write CCCD request event. */
   QAPI_BLE_ET_HRS_SERVER_HEART_RATE_CONTROL_POINT_COMMAND_E
   /**< Write Control Point request event. */
} qapi_BLE_HRS_Event_Type_t;

/**
 * Structure that contains the attribute handles that will need to be
 * cached by a HRS client in order to only do service discovery once.
 */
typedef struct qapi_BLE_HRS_Client_Information_s
{
   /**
    * HRS Heart Rate Measurement attribute handle.
    */
   uint16_t Heart_Rate_Measurement;

   /**
    * HRS Heart Rate Measurement Client Characteristic Configuration
    * Descriptor (CCCD) attribute handle.
    */
   uint16_t Heart_Rate_Measurement_Client_Configuration;

   /**
    * HRS Body Sensor Location attribute handle.
    */
   uint16_t Body_Sensor_Location;

   /**
    * HRS Heart Rate Control Point attribute handle.
    */
   uint16_t Heart_Rate_Control_Point;
} qapi_BLE_HRS_Client_Information_t;

#define QAPI_BLE_HRS_CLIENT_INFORMATION_DATA_SIZE        (sizeof(qapi_BLE_HRS_Client_Information_t))

/**
 * Structure that contains all of the per Client data that will need to
 * be stored by a HRS server.
 */
typedef struct qapi_BLE_HRS_Server_Information_s
{
   /**
    * HRS Heart Rate Measurement attribute handle.
    */
   uint16_t Heart_Rate_Measurement_Client_Configuration;
} qapi_BLE_HRS_Server_Information_t;

#define QAPI_BLE_HRS_SERVER_INFORMATION_DATA_SIZE        (sizeof(qapi_BLE_HRS_Server_Information_t))

/**
 * Structure that represents the format for the data that is dispatched
 * to a HRS server when a HRS client has sent a request to read a HRS
 * characteristic's CCCD.
 *
 * Some of this structures fields will be required when responding to a
 * request using the
 * qapi_BLE_HRS_Read_Client_Configuration_Response() function.
 */
typedef struct qapi_BLE_HRS_Read_Client_Configuration_Data_s
{
   /**
    * HRS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the HRS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the HRS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * Bluetooth address of the HRS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the requested CCCD based on the HRS Characteristic
    * type.
    */
   qapi_BLE_HRS_Characteristic_Type_t ClientConfigurationType;
} qapi_BLE_HRS_Read_Client_Configuration_Data_t;

#define QAPI_BLE_HRS_READ_CLIENT_CONFIGURATION_DATA_SIZE  (sizeof(qapi_BLE_HRS_Read_Client_Configuration_Data_t))
/**<
 * Size of the #qapi_BLE_HRS_Read_Client_Configuration_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a HRS server when a HRS client has sent a request to write a
 * HRS characteristic's CCCD.
 */
typedef struct qapi_BLE_HRS_Client_Configuration_Update_Data_s
{
   /**
    * HRS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the HRS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the HRS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * Bluetooth address of the HRS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the requested CCCD based on the HRS Characteristic
    * type.
    */
   qapi_BLE_HRS_Characteristic_Type_t ClientConfigurationType;

   /**
    * CCCD value that has been requested to be written.
    */
   uint16_t                           ClientConfiguration;
} qapi_BLE_HRS_Client_Configuration_Update_Data_t;

#define QAPI_BLE_HRS_CLIENT_CONFIGURATION_UPDATE_DATA_SIZE  (sizeof(qapi_BLE_HRS_Client_Configuration_Update_Data_t))
/**<
 * Size of the #qapi_BLE_HRS_Client_Configuration_Update_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an HRS server when an HRS client has sent a request to write the
 * HRS Control Point Characteristic.
 */
typedef struct qapi_BLE_HRS_Heart_Rate_Control_Command_Data_s
{
   /**
    * HRS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the HRS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the HRS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * Bluetooth address of the HRS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * HRS Heart Rate Control Point command.
    */
   qapi_BLE_HRS_Heart_Rate_Control_Command_t Command;
} qapi_BLE_HRS_Heart_Rate_Control_Command_Data_t;

#define QAPI_BLE_HRS_HEART_RATE_CONTROL_COMMAND_DATA_SIZE  (sizeof(qapi_BLE_HRS_Heart_Rate_Control_Command_Data_t))
/**<
 * Size of the #qapi_BLE_HRS_Heart_Rate_Control_Command_Data_t structure.
 */

/**
 * Structure that represents the container structure for holding all the
 * event data for an HRS instance.
 */
typedef struct qapi_BLE_HRS_Event_Data_s
{
   /**
    * Event type used to determine the appropriate union member of
    * the Event_Data field to access.
    */
   qapi_BLE_HRS_Event_Type_t Event_Data_Type;

   /**
    * Total size of the data contained in the event.
    */
   uint16_t                  Event_Data_Size;
   union
   {
      /**
       * HRS Read CCCD data.
       */
      qapi_BLE_HRS_Read_Client_Configuration_Data_t   *HRS_Read_Client_Configuration_Data;

      /**
       * HRS Write CCCD data.
       */
      qapi_BLE_HRS_Client_Configuration_Update_Data_t *HRS_Client_Configuration_Update_Data;

      /**
       * HRS Heart Rate Control Point Command data.
       */
      qapi_BLE_HRS_Heart_Rate_Control_Command_Data_t  *HRS_Heart_Rate_Control_Command_Data;
   }
   /**
    * Event data.
    */
   Event_Data;
} qapi_BLE_HRS_Event_Data_t;

#define QAPI_BLE_HRS_EVENT_DATA_SIZE                     (sizeof(qapi_BLE_HRS_Event_Data_t))
/**<
 * Size of the #qapi_BLE_HRS_Event_Data_t structure.
 */

/**
 * @brief
 * This declared type represents the Prototype Function for an
 * HRS Event Callback. This function will be called whenever a define
 * HRS Event occurs within the Bluetooth Protocol Stack that is
 * specified with the specified Bluetooth Stack ID.
 *
 * @details
 * The event information is passed to the user in the #qapi_BLE_HRS_Event_Data_t
 * structure, which is a tructure that contains all the information about the
 * event that occurred.
 *
 * The caller should use the contents of the HRS Event Data
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
 * anyway because another HRS Event will not be processed while this
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
 * @param[in]  HRS_Event_Data        Pointer to a structure that contains
 *                                   information about the event that has
 *                                   occurred.
 *
 * @param[in]  CallbackParameter     User-defined value that will be
 *                                   received with the HRS Event data.
 *
 * @return None.
 */
typedef void (QAPI_BLE_BTPSAPI *qapi_BLE_HRS_Event_Callback_t)(uint32_t BluetoothStackID, qapi_BLE_HRS_Event_Data_t *HRS_Event_Data, uint32_t CallbackParameter);

   /* HRS server API.                                                   */

/**
 * @brief
 * Initializes an HRS server on a specified Bluetooth Stack.
 *
 * @details
 * Only one HRS server may be open at a time, per the Bluetooth Stack ID.
 *
 * @param[in]  BluetoothStackID     Unique identifier assigned to this
 *                                  Bluetooth Protocol Stack via a
 *                                  call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  Supported_Commands   Commands that will be
 *                                  supported by the HRS service instance. These
 *                                  commands can be found in qapi_ble_hrstypes.h
 *                                  and have the form
 *                                  QAPI_BLE_HRS_HEART_RATE_CONTROL_POINT_XXX_SUPPORTED.
 *
 * @param[in]  EventCallback        HRS Event Callback that will
 *                                  receive HRS server events.
 *
 * @param[in]  CallbackParameter    User-defined value that will be
 *                                  received with the specified
 *                                  EventCallback parameter.
 *
 * @param[out]  ServiceID           Unique GATT service ID of the
 *                                  registered HRS service returned from
 *                                  qapi_BLE_GATT_Register_Service() API.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of HRS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HRS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_HRS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BTGATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HRS_Initialize_Service(uint32_t BluetoothStackID, uint32_t Supported_Commands, qapi_BLE_HRS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

/**
 * @brief
 * Initializes an HRS server on a specified Bluetooth Stack.
 *
 * @details
 * Unlike qapi_BLE_HRS_Initialize_Service(), this function allows the
 * application to select a handle range in GATT to store the service.
 *
 * Only one HRS server may be open at a time, per the Bluetooth Stack ID.
 *
 * @param[in]  BluetoothStackID     Unique identifier assigned to this
 *                                  Bluetooth Protocol Stack via a
 *                                  call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  Supported_Commands   Commands that will be
 *                                  supported by the HRS service instance. These
 *                                  commands can be found in qapi_ble_hrstypes.h
 *                                  and have the form
 *                                  QAPI_BLE_HRS_HEART_RATE_CONTROL_POINT_XXX_SUPPORTED.
 *
 * @param[in]  EventCallback        HRS Event Callback that will
 *                                  receive HRS server events.
 *
 * @param[in]  CallbackParameter    User-defined value that will be
 *                                  received with the specified
 *                                  EventCallback parameter.
 *
 * @param[out]  ServiceID           Unique GATT service ID of the
 *                                  registered HRS service returned from
 *                                  qapi_BLE_GATT_Register_Service() API.
 *
 * @param[in,out]  ServiceHandleRange    Pointer that, on input, holds
 *                                       the handle range to store the
 *                                       service in GATT, and on output,
 *                                       contains the handle range for
 *                                       where the service is stored in
 *                                       GATT.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of HRS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HRS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_HRS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BTGATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HRS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, uint32_t Supported_Commands, qapi_BLE_HRS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

/**
 * @brief
 * Cleans up and frees all resources
 * associated with a Heart Rate Service (HRS) Instance.
 *
 * @details
 * After this function is called, no other HRS
 * function can be called until after a successful call to either of the
 * qapi_BLE_HRS_Initialize_XXX() functions are performed.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of the qapi_BLE_HRS_Initialize_XXX()
 *                                 functions.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HRS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HRS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HRS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * Queries the number of attributes
 * that are contained in the HRS service that is registered with a
 * call to qapi_BLE_HRS_Initialize_Service() or
 * qapi_BLE_HRS_Initialize_Service_Handle_Range().
 *
 * @return   Positive, nonzero, number of attributes that would be
 *           registered by a HRS service instance.
 *
 * @return   Zero on failure.
 */
QAPI_BLE_DECLARATION unsigned int QAPI_BLE_BTPSAPI qapi_BLE_HRS_Query_Number_Attributes(void);

/**
 * @brief
 * Sets the HRS Body Sensor Location.
 *
 * @details
 * The Body_Sensor_Location parameter should be an enumerated value of the
 * form QAPI_BLE_HRS_BODY_SENSOR_LOCATION_XXX.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_HRS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  Body_Sensor_Location    Enumerated value for the HRS
 *                                     Body Sensor Location that will be
 *                                     set if this function is
 *                                     successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HRS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HRS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HRS_Set_Body_Sensor_Location(uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t Body_Sensor_Location);

/**
 * @brief
 * Queries the HRS Body Sensor Location.
 *
 * @details
 * The Body_Sensor_Location parameter should be an enumerated value of the
 * form QAPI_BLE_HRS_BODY_SENSOR_LOCATION_XXX.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_HRS_Initialize_XXX()
 *                                 functions.
 *
 * @param[out]  Body_Sensor_Location    Pointer to an enumerated value
 *                                      that will hold the HRS Body
 *                                      Sensor Location if this function
 *                                      is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HRS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HRS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HRS_Query_Body_Sensor_Location(uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t *Body_Sensor_Location);

/**
 * @brief
 * Provides a mechanism for an HRS server to
 * successfully respond to a received read client configuration request.
 *
 * @details
 * Possible Events:
 *
 *    QAPI_BLE_ET_HRS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E
 *
 * @param[in]  BluetoothStackID        Unique identifier assigned to this
 *                                     Bluetooth Protocol Stack via a
 *                                     call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID              Service instance ID to close.
 *                                     This is the value that was returned
 *                                     from the qapi_BLE_HRS_Initialize_Service()
 *                                     function.
 *
 * @param[in]  TransactionID           Transaction ID of the original
 *                                     read request. This value was
 *                                     received in the
 *                                     QAPI_BLE_ET_HRS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E
 *                                     event.
 *
 * @param[in]  Client_Configuration    Specifies the CCCD to send
 *                                     to the remote device.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HRS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HRS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HRS_Read_Client_Configuration_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t Client_Configuration);

/**
 * @brief
 * Sends an HRS Measurement notification
 * to an HRS client.
 *
 * @details
 * If the flag
 * QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_FLAGS_RR_INTERVAL_PRESENT is set in
 * the Flags member of the Heart_Rate_Measurement parameter, the
 * Number_Of_RR_Intervals member of the Heart_Rate_Measurement parameter on
 * input must contain the number of RR Intervals values that are contained
 * in the RR_Intervals member of the Heart_Rate_Measurement parameter. If
 * this function returns success, on output, the Number_Of_RR_Intervals
 * member will contain the total number of RR Interval values that were
 * formatted into the notification (this is guaranteed to be less than or
   equal to the value of Number_Of_RR_Intervals on INPUT).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of the
 *                                 qapi_BLE_HRS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the HRS
 *                                 client that will receive the
 *                                 notification.
 *
 * @param[in]  Heart_Rate_Measurement    Pointer to the HRS Measurement
 *                                       data that will be notified to the
 *                                       HRS client.
 *
 * @return      Positive nonzero if successful (represents the
 *              length of the notification).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HRS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HRS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HRS_Notify_Heart_Rate_Measurement(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_HRS_Heart_Rate_Measurement_Data_t *Heart_Rate_Measurement);

   /* HRS client API.                                                   */

/**
 * @brief
 * Parses a value received in an
 * indication from a remote HRS server, interpreting it as an HRS
 * Measurement.
 *
 * @details
 * On input, the Number_Of_RR_Intervals member of the HeartRateMeasurement
 * parameter must contain the number of entries in
 * the RR_Intervals array. On return, this parameter will contain the
 * actual number of RR Interval values that were parsed from the Heart
 * Rate Measurement value (which will always be less than or equal to the
 * number of entries allocated in the structure).
 *
 * It is possible to query the total number of RR Interval values in the
 * Heart Rate Measurement value by passing 0 for the
 * Number_Of_RR_Intervals member of the HeartRateMeasurement parameter.
 * In this case, on return, the Number_Of_RR_Intervals member will contain
 * the total number of RR Interval values in the Heart Rate Measurement
 * value, but no RR Intervals will be parsed into the HeartRateMeasurement
 * structure.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            HRS server.
 *
 * @param[in]  Value          Value received from the
 *                            HRS server.
 *
 * @param[out]  HeartRateMeasurement    Pointer that will hold the
 *                                      decoded HRS Measurement data if
 *                                      this function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HRS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HRS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HRS_Decode_Heart_Rate_Measurement(uint32_t ValueLength, uint8_t *Value, qapi_BLE_HRS_Heart_Rate_Measurement_Data_t *HeartRateMeasurement);

/**
 * @brief
 * Parses a value received in an
 * indication from a remote HRS server, interpreting it as the HRS Body
 * Sensor Location.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            HRS server.
 *
 * @param[in]  Value          Value received from the
 *                            HRS server.
 *
 * @param[out]  BodySensorLocation    Pointer that will hold the
 *                                    decoded HRS Body Sensor Location if
 *                                    this function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HRS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_HRS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HRS_Decode_Body_Sensor_Location(uint32_t ValueLength, uint8_t *Value, uint8_t *BodySensorLocation);

/**
 * @brief
 * Formats the HRS Heart Rate
 * Control Command into a user-specified buffer for a GATT Write request
 * that will be sent to the HRS server.
 *
 * @details
 * The BufferLength and Buffer parameter must point to a buffer of at
 * least QAPI_BLE_HRS_HEART_RATE_CONTROL_POINT_VALUE_LENGTH in size.
 *
 * @param[in]  Command         Enumeration for the command that
 *                             will be formatted into the user-specified
 *                             buffer.
 *
 * @param[in]  BufferLength    Length of the user-specified buffer.
 *
 * @param[out] Buffer          User-specified buffer that will hold
 *                             the formatted data if this function is
 *                             successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_HRS_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_HRS_Format_Heart_Rate_Control_Command(qapi_BLE_HRS_Heart_Rate_Control_Command_t Command, uint32_t BufferLength, uint8_t *Buffer);

/**
 * @}
 */

#endif

