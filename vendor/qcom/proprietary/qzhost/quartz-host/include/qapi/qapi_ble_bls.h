/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_ble_bls.h
 *
 * @brief
 * QCA QAPI for Bluetopia Bluetooth Blood Pressure Service
 * (GATT based) API Type Definitions, Constants, and
 * Prototypes.
 *
 * @details
 * The Blood Pressure Service (BLS) programming interface defines the
 * protocols and procedures to be used to implement Blood Pressue
 * Service capabilities.
 */

#ifndef __QAPI_BLE_BLS_H__
#define __QAPI_BLE_BLS_H__

#include "./qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.          */
#include "./qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.    */
#include "./qapi_ble_gatt.h"      /* qapi GATT prototypes.                    */
#include "./qapi_ble_blstypes.h"  /* QAPI BLS prototypes.                     */

/** @addtogroup qapi_ble_services
@{
*/

   /* Error Return Codes.                                               */

   /* Error Codes that are smaller than these (less than -1000) are     */
   /* related to the Bluetooth Protocol Stack itself (see               */
   /* qapi_ble_errros.h).                                               */
#define QAPI_BLE_BLS_ERROR_INVALID_PARAMETER           (-1000)
/**< Invalid parameter. */
#define QAPI_BLE_BLS_ERROR_INVALID_BLUETOOTH_STACK_ID  (-1001)
/**< Invalid Bluetooth Stack ID. */
#define QAPI_BLE_BLS_ERROR_INSUFFICIENT_RESOURCES      (-1002)
/**< Insufficient resources. */
#define QAPI_BLE_BLS_ERROR_INSUFFICIENT_BUFFER_SPACE   (-1003)
/**< Insufficient buffer space. */
#define QAPI_BLE_BLS_ERROR_SERVICE_ALREADY_REGISTERED  (-1004)
/**< Service is already registered. */
#define QAPI_BLE_BLS_ERROR_INVALID_INSTANCE_ID         (-1005)
/**< Invalid service instance ID. */
#define QAPI_BLE_BLS_ERROR_MALFORMATTED_DATA           (-1006)
/**< Malformatted data. */
#define QAPI_BLE_BLS_ERROR_INDICATION_OUTSTANDING      (-1007)
/**< Indication outstanding. */

/**
 * Structure that contains the attribute handles that will need to be
 * cached by a BLS client in order to only do service discovery once.
 */
typedef struct qapi_BLE_BLS_Client_Information_s
{
   /**
    * BLS Blood Pressure Measurement attribute handle.
    */
   uint16_t Blood_Pressure_Measurement;

   /**
    * BLS Blood Pressure Measurement CCCD attribute handle.
    */
   uint16_t Blood_Pressure_Measurement_Client_Configuration;

   /**
    * BLS Intermediate Cuff Pressure attribute handle.
    */
   uint16_t Intermediate_Cuff_Pressure;

   /**
    * BLS Intermediate Cuff Pressure CCCD attribute handle.
    */
   uint16_t Intermediate_Cuff_Pressure_Client_Configuration;

   /**
    * BLS Blood Pressure Feature attribute handle.
    */
   uint16_t Blood_Pressure_Feature;
} qapi_BLE_BLS_Client_Information_t;

#define QAPI_BLE_BLS_CLIENT_INFORMATION_DATA_SIZE        (sizeof(qapi_BLE_BLS_Client_Information_t))
/**<
 * Size of the #qapi_BLE_BLS_Client_Information_t structure.
 */

/**
 * Structure that contains all of the per client data that will need to
 * be stored by a BLS server.
 */
typedef struct qapi_BLE_BLS_Server_Information_s
{
   /**
    * BLS Blood Pressure Measurement CCCD.
    */
   uint16_t Blood_Pressure_Measurement_Client_Configuration;

   /**
    * BLS Intermediate Cuff Pressure CCCD.
    */
   uint16_t Intermediate_Cuff_Pressure_Client_Configuration;
} qapi_BLE_BLS_Server_Information_t;

#define QAPI_BLE_BLS_SERVER_INFORMATION_DATA_SIZE        (sizeof(qapi_BLE_BLS_Server_Information_t))
/**<
 * Size of the #qapi_BLE_BLS_Server_Information_t structure.
 */

/**
 * Enumeration of the valid Read Request types that a server
 * may receive in a
 * QAPI_BLE_ET_BLS_Server_Read_Client_Configuration_Request_E or
 * QAPI_BLE_ET_BLS_Server_Client_Configuration_Update_E event. This is
 * also used by the qapi_BLE_BLS_Send_Notification() function to denote
 * the characteristic value to notify.
 *
 * For each event it is up to the application to return (or write) the
 * correct Client Configuration descriptor based on this value.
 */
typedef enum
{
   QAPI_BLE_CT_BLOOD_PRESSURE_MEASUREMENT_E, /**< Blood Pressure Measurement. */
   QAPI_BLE_CT_INTERMEDIATE_CUFF_PRESSURE_E  /**< Intermediate Cuff Pressure. */
} qapi_BLE_BLS_Characteristic_Type_t;

/**
 * Structure that represents the Date/Time data for BLS.
 */
typedef struct qapi_BLE_BLS_Date_Time_Data_s
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
} qapi_BLE_BLS_Date_Time_Data_t;

#define QAPI_BLE_BLS_DATE_TIME_DATA_SIZE                 (sizeof(qapi_BLE_BLS_Date_Time_Data_t))
/**<
 * Size of the #qapi_BLE_BLS_Date_Time_Data_t structure.
 */

#define QAPI_BLE_BLS_TIME_STAMP_VALID(_x)                ((QAPI_BLE_GATT_DATE_TIME_VALID_YEAR(((_x)).Year)) && (QAPI_BLE_GATT_DATE_TIME_VALID_MONTH(((_x)).Month)) && (QAPI_BLE_GATT_DATE_TIME_VALID_DAY(((_x)).Day)) && (QAPI_BLE_GATT_DATE_TIME_VALID_HOURS(((_x)).Hours)) && (QAPI_BLE_GATT_DATE_TIME_VALID_MINUTES(((_x)).Minutes)) && (QAPI_BLE_GATT_DATE_TIME_VALID_SECONDS(((_x)).Seconds)))
/**<
 * This macro is a utility macro that exists to valid that a specified
 * Time Stamp is valid. The only parameter to this function is the
 * #qapi_BLE_BLS_Date_Time_Data_t structure to valid.This macro returns
 * TRUE if the Time Stamp is valid or FALSE otherwise.
 */

/**
 * Structure that represents the BLS Blood Pressure Measurement Compound
 * that is included in a BLS Measurement.
 *
 * If a value for the Systolic, Diastolic or MAP fields is
 * unavailable, the special short float value NaN will be filled in each
 * of the unavailable subfields.
 */
typedef struct qapi_BLE_BLS_Compound_Value_Data_s
{
   /**
    * Systolic blood pressure.
    */
   uint16_t Systolic;

   /**
    * Diastolic blood pressure.
    */
   uint16_t Diastolic;

   /**
    * Mean arterial pressure.
    */
   uint16_t Mean_Arterial_Pressure;
}qapi_BLE_BLS_Compound_Value_Data_t;

#define QAPI_BLE_BLS_COMPOUND_VALUE_DATA_SIZE            (sizeof(qapi_BLE_BLS_Compound_Value_Data_t))
/**<
 * Size of the #qapi_BLE_BLS_Compound_Value_Data_t structure.
 */

/**
 * Structure that represents the BLS Blood Pressure Measurement data.
 */
typedef struct qapi_BLE_BLS_Blood_Pressure_Measurement_Data_s
{
   /**
    * Bitmask that indicates the optional fields and features that may
    * be included for a BLS Measurement.
    *
    * Valid values have the form QAPI_BLE_BLS_MEASUREMENT_FLAGS_XXX and
    * can be found in qapi_ble_blstypes.h.
    */
   uint8_t                            Flags;

   /**
    * BLS Blood Pressure Measurement Compound.
    */
   qapi_BLE_BLS_Compound_Value_Data_t CompoundValue;

   /**
    * Timestamp for the BLS Measurement.
    */
   qapi_BLE_BLS_Date_Time_Data_t      TimeStamp;

   /**
    * Pulse rate.
    */
   uint16_t                           PulseRate;

   /**
    * User ID.
    */
   uint8_t                            UserID;

   /**
    * BLS Measurement status.
    */
   uint16_t                           MeasurementStatus;

   /**
    * Reserved fields.
    */
   uint8_t                            Reserved[2];
} qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t;

#define QAPI_BLE_BLS_BLOOD_PRESSURE_MEASUREMENT_DATA_SIZE  (sizeof(qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t))
/**<
 * Size of the #qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t structure.
 */

/**
 * Enumeration of all the events generated by the BLS
 * Profile. These are used to determine the type of each event
 * generated, and to ensure the proper union element is accessed for the
 * qapi_BLE_BLS_Event_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_ET_BLS_READ_CLIENT_CONFIGURATION_REQUEST_E,
   /**< Read CCCD request event. */
   QAPI_BLE_ET_BLS_CLIENT_CONFIGURATION_UPDATE_E,
   /**< Write CCCD request event. */
   QAPI_BLE_ET_BLS_CONFIRMATION_DATA_E
   /**< Confirmation event. */
} qapi_BLE_BLS_Event_Type_t;

/**
 * Structure that represents the format for the data that is dispatched
 * to a BLS server when a BLS client has sent a request to read a BLS
 * characteristic's CCCD.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_BLS_Read_Client_Configuration_Response() function.
 */
typedef struct qapi_BLE_BLS_Read_Client_Configuration_Data_s
{
   /**
    * BLS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the BLS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the BLS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * Bluetooth address of the BLS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the requested CCCD based on the BLS characteristic type.
    */
   qapi_BLE_BLS_Characteristic_Type_t ClientConfigurationType;
} qapi_BLE_BLS_Read_Client_Configuration_Data_t;

#define QAPI_BLE_BLS_READ_CLIENT_CONFIGURATION_DATA_SIZE  (sizeof(qapi_BLE_BLS_Read_Client_Configuration_Data_t))
/**<
 * Size of the #qapi_BLE_BLS_Read_Client_Configuration_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a BLS server when a BLS client has sent a request to write a BLS
 * characteristic's CCCD.
 */
typedef struct qapi_BLE_BLS_Client_Configuration_Update_Data_s
{
   /**
    * BLS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the BLS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the BLS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * Bluetooth address of the BLS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the requested CCCD based on the BLS Characteristic
    * type.
    */
   qapi_BLE_BLS_Characteristic_Type_t ClientConfigurationType;

   /**
    * CCCD value that has been requested to be written.
    */
   uint16_t                           ClientConfiguration;
} qapi_BLE_BLS_Client_Configuration_Update_Data_t;

#define QAPI_BLE_BLS_CLIENT_CONFIGURATION_UPDATE_DATA_SIZE  (sizeof(qapi_BLE_BLS_Client_Configuration_Update_Data_t))
/**<
 * Size of the #qapi_BLE_BLS_Client_Configuration_Update_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a BLS server when a BLS client has responded to an outstanding
 * indication that was previously sent by the BLS server.
 */
typedef struct qapi_BLE_BLS_Confirmation_Data_s
{
   /**
    * BLS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the BLS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * Status of the indication.
    */
   uint8_t                         Status;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the BLS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * Bluetooth address of the BLS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;
} qapi_BLE_BLS_Confirmation_Data_t;

#define QAPI_BLE_BLS_CONFIRMATION_DATA_SIZE              (sizeof(qapi_BLE_BLS_Confirmation_Data_t))
/**<
 * Size of the #qapi_BLE_BLS_Confirmation_Data_t structure.
 */

/**
 * Structure that represents the container structure for holding all the
 * event data for a BLS instance.
 */
typedef struct qapi_BLE_BLS_Event_Data_s
{
   /**
    * Event type used to determine the appropriate union member of
    * the Event_Data field to access.
    */
   qapi_BLE_BLS_Event_Type_t Event_Data_Type;

   /**
    * Total size of the data contained in the event.
    */
   uint16_t                  Event_Data_Size;
   union
   {
      /**
       * BLS Read CCCD data.
       */
      qapi_BLE_BLS_Read_Client_Configuration_Data_t   *BLS_Read_Client_Configuration_Data;

      /**
       * BLS Write CCCD data.
       */
      qapi_BLE_BLS_Client_Configuration_Update_Data_t *BLS_Client_Configuration_Update_Data;

      /**
       * BLS confirmation data.
       */
      qapi_BLE_BLS_Confirmation_Data_t                *BLS_Confirmation_Data;
   }
   /**
    * Event data.
    */
   Event_Data;
}qapi_BLE_BLS_Event_Data_t;

#define QAPI_BLE_BLS_EVENT_DATA_SIZE                     (sizeof(qapi_BLE_BLS_Event_Data_t))
/**<
 * Size of the #qapi_BLE_BLS_Event_Data_t structure.
 */

/**
 * @brief
 * This declared type represents the prototype function for
 * a BLS Service Event Receive Data Callback. This function will be called
 * whenever a BLS Service Event occurs that is associated with the
 * specified Bluetooth stack ID.

 * @details
 * The caller should use the contents of the BLS Profile Event Data
 * only in the context of this callback. If the caller requires
 * the data for a longer period of time, then the callback function
 * must copy the data into another Data Buffer.
 *
 * This function is guaranteed not to be invoked more than once
 * simultaneously for the specified installed callback (i.e., this
 * function does not have be reentrant). It should be noted however,
 * that if the same callback is installed more than once, the
 * callbacks will be called serially. Because of this, the processing
 * in this function should be as efficient as possible.
 *
 * It should also be noted that this function is called in the Thread
 * Context of a Thread that the User does not own. Therefore, processing
 * in this function should be as efficient as possible (this argument holds
 * anyway because another BLS Service Event will not be processed while this
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
 * @param[in]  BLS_Event_Data        Pointer to a structure that contains
 *                                   information about the event that has
 *                                   occurred.
 *
 * @param[in]  CallbackParameter     User-defined value that was supplied
 *                                   as an input parameter from a prior
 *                                   BLS request.
 *
 * @return None.
 */
typedef void (QAPI_BLE_BTPSAPI *qapi_BLE_BLS_Event_Callback_t)(uint32_t BluetoothStackID, qapi_BLE_BLS_Event_Data_t *BLS_Event_Data, uint32_t CallbackParameter);

   /* BLS Server API.                                                   */

/**
 * @brief
 * Opens a BLS server on a specified Bluetooth stack.
 *
 * @details
 * Only one BLS server may be open at a time, per the Bluetooth Stack ID.
 *
 * All Client Requests will be dispatched to the EventCallback function
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
 *                                  registered BLS service returned from the
 *                                  qapi_BLE_GATT_Register_Service() API.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of BLS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BLS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BT_GATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BLS_Initialize_Service(uint32_t BluetoothStackID, qapi_BLE_BLS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

/**
 * @brief
 * Opens a BLS server on a specified Bluetooth Stack with
 * the ability to control the location of the service in the GATT datablse.
 *
 * @details
 * Only one BLS server may be open at a time, per the Bluetooth Stack ID.
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
 *                                       Range structure that, on input,
 *                                       can be used to control the
 *                                       location of the service in the
 *                                       GATT datablse, and on output,
 *                                       returns the handle range that
 *                                       the service is using in the GATT
 *                                       datablse.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of BLS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BLS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BT_GATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BLS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, qapi_BLE_BLS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

/**
 * @brief
 * Cleans up and frees all resources
 * associated with a BLS Service Instance.
 *
 * @details
 * After this function is called, no other BLS service function can be
 * called until after a successful call to the qapi_BLE_BLS_Initialize_Service()
 * function is performed.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from the qapi_BLE_BLS_Initialize_Service()
 *                                 function.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BLS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BLS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * Queries the number of attributes
 * that are contained in the BLS service that is registered with a
 * call to qapi_BLE_BLS_Initialize_Service() or
 * qapi_BLE_BLS_Initialize_Service_Handle_Range().
 *
 * @return   Positive, nonzero, number of attributes that can be
 *           registered by a BLS service instance. \n
 *           Zero on failure.
 */
QAPI_BLE_DECLARATION unsigned int QAPI_BLE_BTPSAPI qapi_BLE_BLS_Query_Number_Attributes(void);

/**
 * @brief
 * Allows a mechanism for a BLS service to
 * successfully respond to a received read client configuration request
 * from a BLS client.
 *
 * @param[in]  BluetoothStackID        Unique identifier assigned to this
 *                                     Bluetooth Protocol Stack via a
 *                                     call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID              Service instance ID to close.
 *                                     This is the value that was returned
 *                                     from the qapi_BLE_BLS_Initialize_Service()
 *                                     function.
 *
 * @param[in]  TransactionID           Transaction ID of the original
 *                                     read request. This value was
 *                                     received in the
 *                                     QAPI_BLE_ET_BLS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E
 *                                     event.
 *
 * @param[in]  ClientConfiguration     Specifies the Client Characteristic
 *                                     Configuration descriptor to send
 *                                     to the remote device.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BLS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BLS_Read_Client_Configuration_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t ClientConfiguration);

/**
 * @brief
 * Sends a Blood Pressure Measurement
 * indication to a remote BLS client.
 *
 * @details
 * This function sends an indication, which provides a guarantee that the
 * BLS client will receive the value, since the BLS client must confirm
 * that it has been received.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the BLS
 * characteristic that is going to be indicated has been previously
 * configured for indications. A BLS client must have written
 * the BLS characteristic's CCCD to enable indications.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from the qapi_BLE_BLS_Initialize_Service()
 *                                 function.
 *
 * @param[in]  ConnectionID        GATT connection ID of the remote
 *                                 device to which to send the indication.
 *
 * @param[in]  Measurement_Data    Pointer to the BLS Measurement that
 *                                 will be indicated if this function is
 *                                 successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BLS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_HANDLE_VALUE \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_CONNECTION_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BLS_Indicate_Blood_Pressure_Measurement(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t *Measurement_Data);

/**
 * @brief
 * Sends an Intermediate Cuff
 * Pressure notification to a remote BLS client.
 *
 * @details
 * This function sends a notification, which provides no guarantee that the
 * BLS client will receive the value, since the BLS client will not confirm
 * that it has been received.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the BLS
 * characteristic that is going to be notified has been previously
 * configured for notifications. A BLS client must have written
 * the BLS characteristic's CCCD to enable notifications.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from the qapi_BLE_BLS_Initialize_Service()
 *                                 function.
 *
 * @param[in]  ConnectionID        GATT connection ID of the remote
 *                                 device to which to send the notification.
 *
 * @param[in]  Intermediate_Cuff_Pressure    Pointer to the BLS
 *                                           Intermediate Cuff Pressure data
 *                                           that will be notified if this
 *                                           function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BLS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_HANDLE_VALUE \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_CONNECTION_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BLS_Notify_Intermediate_Cuff_Pressure(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t *Intermediate_Cuff_Pressure);

/**
 * @brief
 * Sets the supported BLS features for the BLS server.
 *
 * @details
 * The SupportedFeatures parameter is a bitmask made up of bits of the form
 * QAPI_BLE_BLS_FEATURE_XXX found in qapi_ble_bls.h.
 *
 * This function must be called after the BLS service is registered with
 * a successful call to qapi_BLE_BLS_Initialize_XXX() in order to set the default
 * features of the BLS server.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from the qapi_BLE_BAS_Initialize_Service()
 *                                 function.
 *
 * @param[in]  SupportedFeatures    Bitmask that contains the
 *                                  supported features that the BLS
 *                                  server will support if this function
 *                                  is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BLS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_BTPS_ERROR_FEATURE_NOT_AVAILABLE
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BLS_Set_Blood_Pressure_Feature(uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t SupportedFeatures);

/**
 * @brief
 * Gets the supported BLS features for the BLS server.
 *
 * @details
 * The SupportedFeatures parameter is a bitmask made up of bits of the form
 * QAPI_BLE_BLS_FEATURE_XXX found in qapi_ble_bls.h.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from the qapi_BLE_BAS_Initialize_Service()
 *                                 function.
 *
 * @param[out] SupportedFeatures    A pointer to a bit mask that will
 *                                  hold the supported features of the
 *                                  BLS Server if this function is
 *                                  successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BLS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_BTPS_ERROR_FEATURE_NOT_AVAILABLE
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BLS_Query_Blood_Pressure_Feature(uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t *SupportedFeatures);

   /* BLS Client API.                                                  */

/**
 * @brief
 * Parses a value received in an
 * indication from a remote BLS server interpreting it as a Blood Pressure
 * Measurement.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            BLS server.
 *
 * @param[in]  Value          Value received from the
 *                            BLS server.
 *
 * @param[out]  BloodPressureMeasurement    Pointer that will hold the
 *                                          decoded BLS Measurement data
 *                                          if this function is
 *                                          successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BLS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BLS_Decode_Blood_Pressure_Measurement(uint32_t ValueLength, uint8_t *Value, qapi_BLE_BLS_Blood_Pressure_Measurement_Data_t *BloodPressureMeasurement);

/** @}
 */

#endif

