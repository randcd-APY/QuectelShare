/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_ble_wss.h
 *
 * @brief
 * QCA QAPI for Bluetopia Bluetooth Weight Scale Service
 * (GATT based) API Type Definitions, Constants, and
 * Prototypes.
 *
 * @details
 * The Weight Scale Service (WSS) programming interface defines the
 * protocols and procedures to be used to implement the Weight Scale Service
 * capabilities.
 */

#ifndef __QAPI_BLE_WSS_H__
#define __QAPI_BLE_WSS_H__

#include "./qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.          */
#include "./qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.    */
#include "./qapi_ble_gatt.h"      /* qapi GATT prototypes.                    */
#include "./qapi_ble_wsstypes.h"  /* QAPI WSS prototypes.                     */

/** @addtogroup qapi_ble_services
@{
*/

   /* Error Return Codes.                                               */

   /* Error Codes that are smaller than these (less than -1000) are     */
   /* related to the Bluetooth Protocol Stack itself (see               */
   /* qapi_ble_errors.h).                                               */
#define QAPI_BLE_WSS_ERROR_INVALID_PARAMETER                      (-1000)
/**< Invalid parameter. */
#define QAPI_BLE_WSS_ERROR_INVALID_BLUETOOTH_STACK_ID             (-1001)
/**< Invalid Bluetooth Stack ID. */
#define QAPI_BLE_WSS_ERROR_INSUFFICIENT_RESOURCES                 (-1002)
/**< Insufficient resources. */
#define QAPI_BLE_WSS_ERROR_INSUFFICIENT_BUFFER_SPACE              (-1003)
/**< Insufficient buffer space. */
#define QAPI_BLE_WSS_ERROR_SERVICE_ALREADY_REGISTERED             (-1004)
/**< Service is already registered. */
#define QAPI_BLE_WSS_ERROR_INVALID_INSTANCE_ID                    (-1005)
/**< Invalid service instance ID. */
#define QAPI_BLE_WSS_ERROR_MALFORMATTED_DATA                      (-1006)
/**< Malformatted data. */

/**
 * Structure that contains the attribute handles that will need to be
 * cached by a WSS client in order to only do service discovery once.
 */
typedef struct qapi_BLE_WSS_Client_Information_s
{
   /**
    * WSS Weight Scale Feature attribute handle.
    */
   uint16_t Weight_Scale_Feature;

   /**
    * WSS Weight Measurement attribute handle.
    */
   uint16_t Weight_Measurement;

   /**
    * WSS Weight Measurement Client Characteristic Configuration
    * Descriptor (CCCD) attribute handle.
    */
   uint16_t Weight_Measurement_CCCD;
} qapi_BLE_WSS_Client_Information_t;

#define QAPI_BLE_WSS_CLIENT_INFORMATION_DATA_SIZE                 (sizeof(qapi_BLE_WSS_Client_Information_t))
/**<
 * Size of the #qapi_BLE_WSS_Client_Information_t structure.
 */

/**
 * Structure that contains all of the per client data that will need to
 * be stored by a WSS server.
 */
typedef struct qapi_BLE_WSS_Server_Information_s
{
   /**
    * WSS Weight Measurement CCCD.
    */
   uint16_t Weight_Measurement_Configuration;
} qapi_BLE_WSS_Server_Information_t;

#define QAPI_BLE_WSS_SERVER_INFORMATION_DATA_SIZE                 (sizeof(qapi_BLE_WSS_Server_Information_t))
/**<
 * Size of the #qapi_BLE_WSS_Server_Information_t structure.
 */

/**
 * Structure that represents the Date/Time data for WSS.
 */
typedef struct qapi_BLE_WSS_Date_Time_Data_s
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
    * Hours.
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
}  qapi_BLE_WSS_Date_Time_Data_t;

#define QAPI_BLE_WSS_DATE_TIME_DATA_SIZE                          (sizeof(qapi_BLE_WSS_Date_Time_Data_t))
/**<
 * Size of the #qapi_BLE_WSS_Date_Time_Data_t structure.
 */

#define QAPI_BLE_WSS_DATE_TIME_VALID(_x)                          ((QAPI_BLE_GATT_DATE_TIME_VALID_YEAR(((_x)).Year)) && (QAPI_BLE_GATT_DATE_TIME_VALID_MONTH(((_x)).Month)) && (QAPI_BLE_GATT_DATE_TIME_VALID_DAY(((_x)).Day)) && (QAPI_BLE_GATT_DATE_TIME_VALID_HOURS(((_x)).Hours)) && (QAPI_BLE_GATT_DATE_TIME_VALID_MINUTES(((_x)).Minutes)) && (QAPI_BLE_GATT_DATE_TIME_VALID_SECONDS(((_x)).Seconds)))
/**<
 * This macro is a utility macro that exists to validate that a specified
 * Date Time is valid. The only parameter to this function is the
 * #qapi_BLE_WSS_Date_Time_Data_t structure to validate. This macro
 * returns TRUE if the Date Time is valid or FALSE otherwise.
 */

/**
 * Structure that represents the data for a WSS Weight Measurement.
 *
 * The value QAPI_BLE_WSS_WEIGHT_MEASUREMENT_UNSUCCESSFUL may be set for
 * the Weight field to indicate that a measurement was unsuccessful.  If
 * this is the case, all optional fields other than the Time stamp field
 * and the User ID field must be disabled.
 *
 * The User_ID field must be included if the WSS server supports
 * multiple users. The value QAPI_BLE_WSS_USER_ID_UNKNOWN may be used to
 * indicate that the user ID is not known if the User_ID field is
 * included in the weight measurement.
 *
 * The BMI and Height fields are optional and must be included together.
 * The WSS server must know the height of the user, since the BMI is
 * calculated by dividing the Weight (pounds) by the square of the Height
 * (inches), and multiplying this value by a factor of approximately
 * (703.07).
 */
typedef struct qapi_BLE_WSS_Weight_Measurement_Data_s
{
   /**
    * Bitmask that is used to control the optional features and fields
    * that can be included for a Weight Measurement. The bitmask has
    * the form QAPI_BLE_WSS_WEIGHT_MEASUREMENT_FLAG_XXX and can be
    * found in qapi_ble_wsstypes.h.
    */
   uint8_t                       Flags;

   /**
    * Weight.
    */
   uint16_t                      Weight;

   /**
    * Date/Time of the measurement.
    */
   qapi_BLE_WSS_Date_Time_Data_t Time_Stamp;

   /**
    * User ID.
    */
   uint8_t                       User_ID;

   /**
    * BMI.
    */
   uint16_t                      BMI;

   /**
    * Height.
    */
   uint16_t                      Height;
} qapi_BLE_WSS_Weight_Measurement_Data_t;

#define QAPI_BLE_WSS_WEIGHT_MEASUREMENT_DATA_SIZE                 (sizeof(qapi_BLE_WSS_Weight_Measurement_Data_t))
/**<
 * Size of the #qapi_BLE_WSS_Weight_Measurement_Data_t structure.
 */

/**
 * Enumeration that represents all the events generated by the WSS
 * service. These are used to determine the type of each event generated,
 * and to ensure the proper union element is accessed for the
 * #qapi_BLE_WSS_Event_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_WSS_ET_SERVER_READ_WEIGHT_MEASUREMENT_CCCD_REQUEST_E,
   /**< Read Weight Measurement CCCD request event. */
   QAPI_BLE_WSS_ET_SERVER_WRITE_WEIGHT_MEASUREMENT_CCCD_REQUEST_E,
   /**< Write Weight Measurement CCCD request event. */
   QAPI_BLE_WSS_ET_SERVER_CONFIRMATION_DATA_E
   /**< Confirmation event. */
} qapi_BLE_WSS_Event_Type_t;

/**
 * Structure that represents the format for the data that is dispatched
 * to a WSS server when a WSS client has sent a request to read the
 * WSS Measurment characteristic's CCCD.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_WSS_Read_Weight_Measurement_CCCD_Request_Response() function.
 */
typedef struct qapi_BLE_WSS_Read_Weight_Measurement_CCCD_Request_Data_s
{
   /**
    * WSS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the WSS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the WSS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * Bluetooth address of the WSS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;
} qapi_BLE_WSS_Read_Weight_Measurement_CCCD_Request_Data_t;

#define QAPI_BLE_WSS_READ_WEIGHT_MEASUREMENT_CCCD_REQUEST_DATA_SIZE  (sizeof(qapi_BLE_WSS_Read_Weight_Measurement_CCCD_Request_Data_t))
/**<
 * Size of the #qapi_BLE_WSS_Read_Weight_Measurement_CCCD_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a WSS server when a WSS client has sent a request to write the WSS
 * Measurement characteristic's CCCD.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_WSS_Write_Weight_Measurement_CCCD_Request_Response() function.
 */
typedef struct qapi_BLE_WSS_Write_Weight_Measurement_CCCD_Request_Data_s
{
   /**
    * WSS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the WSS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the WSS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * Bluetooth address of the WSS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;

   /**
    * CCCD value that has been requested to be written.
    */
   uint16_t                        ClientConfiguration;
} qapi_BLE_WSS_Write_Weight_Measurement_CCCD_Request_Data_t;

#define QAPI_BLE_WSS_WRITE_WEIGHT_MEASUREMENT_CCCD_REQUEST_DATA_SIZE  (sizeof(qapi_BLE_WSS_Write_Weight_Measurement_CCCD_Request_Data_t))
/**<
 * Size of the #qapi_BLE_WSS_Write_Weight_Measurement_CCCD_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to the WSS server when a WSS client has responded to an outstanding
 * indication that was previously sent by the WSS server.
 */
typedef struct qapi_BLE_WSS_Confirmation_Data_s
{
   /**
    * WSS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the WSS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the WSS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * Bluetooth address of the WSS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;

   /**
    * Status of the outstanding indication. Values will be of the
    * form QAPI_BLE_GATT_CONFIRMATION_STATUS_XXX and can be found in
    * qapi_ble_gatt.h.
    */
   uint8_t                         Status;

   /**
    * Number of bytes successfully indicated.
    */
   uint16_t                        BytesWritten;
} qapi_BLE_WSS_Confirmation_Data_t;

#define QAPI_BLE_WSS_CONFIRMATION_DATA_SIZE                       (sizeof(qapi_BLE_WSS_Confirmation_Data_t))
/**<
 * Size of the #qapi_BLE_WSS_Confirmation_Data_t structure.
 */

/**
 * Structure that represents the container structure for holding all the
 * WSS server event data for a WSS instance.
 */
typedef struct qapi_BLE_WSS_Event_Data_s
{
   /**
    * Event type used to determine the appropriate union member of
    * the Event_Data field to access.
    */
   qapi_BLE_WSS_Event_Type_t Event_Data_Type;

   /**
    * Total size of the data contained in the event.
    */
   uint8_t                   Event_Data_Size;

   union
   {
      /**
       * WSS Read Weight Measurment CCCD event data.
       */
      qapi_BLE_WSS_Read_Weight_Measurement_CCCD_Request_Data_t  *WSS_Read_Weight_Measurement_CCCD_Request_Data;

      /**
       * WSS Write Weight Measurment CCCD event data.
       */
      qapi_BLE_WSS_Write_Weight_Measurement_CCCD_Request_Data_t *WSS_Write_Weight_Measurement_CCCD_Request_Data;

      /**
       * WSS Confirmation event data.
       */
      qapi_BLE_WSS_Confirmation_Data_t                          *WSS_Confirmation_Data;
   }
   /**
    * Event data. @newpagetable
    */
   Event_Data;
} qapi_BLE_WSS_Event_Data_t;

#define QAPI_BLE_WSS_EVENT_DATA_SIZE                              (sizeof(qapi_BLE_WSS_Event_Data_t))
/**<
 * Size of the #qapi_BLE_WSS_Event_Data_t structure.
 */

/**
 * @brief
 * This declared type represents the prototype function for a
 * WSS instance event callback. This function will be called
 * whenever a WSS instance event occurs that is associated with the
 * specified Bluetooth stack ID.

 * @details
 * The caller should use the contents of the WSS instance event data
 * only in the context of this callback. If the caller requires
 * the data for a longer period of time, the callback function
 * must copy the data into another data buffer.
 *
 * This function is guaranteed not to be invoked more than once
 * simultaneously for the specified installed callback (i.e., this
 * function does not have be reentrant). It should be noted, however,
 * that if the same event callback is installed more than once, the
 * event callbacks will be called serially. Because of this, the
 * processing in this function should be as efficient as possible.
 *
 * It should also be noted that this function is called in the thread
 * context of a thread that the user does not own. Therefore, processing
 * in this function should be as efficient as possible (this argument holds
 * anyway because another WSS instance event will not be processed while
 * this function call is outstanding).
 *
 * @note1hang
 * This function must not block and wait for WSS instance events that can
 * only be satisfied by receiving other Bluetooth Protocol Stack events.
 * A Deadlock will occur because other event callbacks will not be issued
 * while this function is currently outstanding.
 *
 * @param[in]  BluetoothStackID      Unique identifier assigned to this
 *                                   Bluetooth Protocol Stack on which the
 *                                   event occurred.
 *
 * @param[in]  WSS_Event_Data        Pointer to a structure that contains
 *                                   information about the event that has
 *                                   occurred.
 *
 * @param[in]  CallbackParameter     User-defined value that was supplied
 *                                   as an input parameter when the WSS
 *                                   instance event callback was
 *                                   installed.
 *
 * @return None.
 */
typedef void (QAPI_BLE_BTPSAPI *qapi_BLE_WSS_Event_Callback_t)(uint32_t BluetoothStackID, qapi_BLE_WSS_Event_Data_t *WSS_Event_Data, uint32_t CallbackParameter);

   /* WSS server API.                                                   */

/**
 * @brief
 * Initializes a WSS instance (WSS server) on a specified
 * Bluetooth Protocol Stack.
 *
 * @details
 * Only one WSS instance may be initialized at a time, per the Bluetooth
 * Protocol Stack ID.
 *
 * See the qapi_BLE_WSS_Initialize_Data_t structure for more
 * information about the InitializeData parameter. If this
 * parameter is not configured correctly, a WSS error will be returned.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  Service_Flags       Flags that are used to
 *                                 register WSS for the specified
 *                                 transport. These flags can be found
 *                                 in qapi_ble_gatt.h and have the form
 *                                 QAPI_BLE_GATT_SERVICE_FLAGS_XXX.
 *
 * @param[in]  EventCallback       WSS event callback that will
 *                                 receive WSS instance events.
 *
 * @param[in]  CallbackParameter   User-defined value that will be
 *                                 received with the specified
 *                                 EventCallback parameter.
 *
 * @param[out] ServiceID           Unique GATT service ID of the
 *                                 registered WSS instance returned from
 *                                 the qapi_BLE_GATT_Register_Service()
 *                                 API.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the WSS instance ID of the WSS server that was
 *              successfully initialized for the specified Bluetooth
 *              Protocol Stack ID. This is the value that should be used
 *              in all subsequent function calls that require the WSS
 *              Instance ID.
 *
 * @return      An error code if negative. WSS error codes can be
 *              found in qapi_ble_wss.h (QAPI_BLE_WSS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_WSS_Initialize_Service(uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_WSS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

/**
 * @brief
 * Initializes a WSS instance (WSS server) on a specified
 * Bluetooth Protocol Stack.
 *
 * Unlike qapi_BLE_WSS_Initialize_Service(), this function allows the
 * application to select a attribute handle range in GATT to store the
 * service.
 *
 * @details
 * Only one WSS instance may be initialized at a time, per the Bluetooth
 * Protocol Stack ID.
 *
 * See the qapi_BLE_WSS_Initialize_Data_t structure for more
 * information about the InitializeData parameter. If this
 * parameter is not configured correctly, a WSS error will be returned.
 *
 * If the application wants GATT to select the attribute handle range for
 * the service, all fields of the ServiceHandleRange parameter must
 * be initialized to zero. The qapi_BLE_WSS_Query_Number_Attributes()
 * function may be used after initializing a WSS instance to determine
 * the attribute handle range for the WSS instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  Service_Flags       Flags that are used to
 *                                 register WSS for the specified
 *                                 transport. These flags can be found
 *                                 in qapi_ble_gatt.h and have the form
 *                                 QAPI_BLE_GATT_SERVICE_FLAGS_XXX.
 *
 *
 * @param[in]  EventCallback       WSS event callback that will
 *                                 receive WSS instance events.
 *
 * @param[in]  CallbackParameter   User-defined value that will be
 *                                 received with the specified
 *                                 EventCallback parameter.
 *
 * @param[out] ServiceID           Unique GATT service ID of the
 *                                 registered WSS instance returned from
 *                                 the qapi_BLE_GATT_Register_Service()
 *                                 API.
 *
 * @param[in,out]  ServiceHandleRange   Pointer that, on input, holds
 *                                      the handle range to store the
 *                                      service in GATT and, on output,
 *                                      contains the handle range for
 *                                      where the service is stored in
 *                                      GATT.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the WSS instance ID of the WSS server that was
 *              successfully initialized for the specified Bluetooth
 *              Protocol Stack ID. This is the value that should be used
 *              in all subsequent function calls that require the WSS
 *              Instance ID.
 *
 * @return      An error code if negative. WSS error codes can be
 *              found in qapi_ble_wss.h (QAPI_BLE_WSS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_WSS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_WSS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t  *ServiceHandleRange);

/**
 * @brief
 * Cleans up and frees all resources
 * associated with a WSS instance (WSS server).
 *
 * @details
 * After this function is called, no other WSS
 * function can be called until after a successful call to either of the
 * qapi_BLE_WSS_Initialize_XXX() functions.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the WSS instance.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. WSS error codes can be
 *              found in qapi_ble_wss.h (QAPI_BLE_WSS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_WSS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * Queries the number of attributes
 * that are contained in a WSS instance that is registered with a call
 * to either of the qapi_BLE_WSS_Initialize_XXX() functions.
 *
 * @return   Positive, nonzero, number of attributes for the
 *           registered WSS instance.
 *
 * @return   Zero on failure.
 */
QAPI_BLE_DECLARATION unsigned int QAPI_BLE_BTPSAPI qapi_BLE_WSS_Query_Number_Attributes(void);

/**
 * @brief
 * Sets the supported features of the WSS server.
 *
 * @details
 * The SupportedFeatures parameter is a bitmask made up of bits of the form
 * QAPI_BLE_WSS_WEIGHT_SCALE_FEATURE_FLAG_XXX from qapi_ble_wsstypes.h. The
 * Configuration may be zero.
 *
 * @note1hang
 * This function must be called after the WSS service is registered with
 * a successful call to either of the qapi_BLE_WSS_Initialize_XXX()
 * functions in order to set the features of the WSS server.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the WSS instance.
 *
 * @param[in]  SupportedFeatures   Bitmask for the supported features
 *                                 of the WSS server.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. WSS error codes can be
 *              found in qapi_ble_wss.h.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_WSS_Set_Weight_Scale_Feature(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t SupportedFeatures);

/**
 * @brief
 * Queries the supported features of the WSS server.
 *
 * @details
 * The SupportedFeatures parameter is a bitmask made up of bits of the form
 * QAPI_BLE_WSS_WEIGHT_SCALE_FEATURE_FLAG_XXX from qapi_ble_wsstypes.h. The
 * Configuration may be zero.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the WSS instance.
 *
 * @param[in]  SupportedFeatures   Pointer that will hold the supported
 *                                 features of the WSS server if this
 *                                 function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. WSS error codes can be
 *              found in qapi_ble_wss.h.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_WSS_Query_Weight_Scale_Feature(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t *SupportedFeatures);

/**
 * @brief
 * Responds to a read request
 * received from a WSS client for a WSS characteristic's CCCD.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * qapi_ble_wsstypes.h (WSS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * If the request is rejected, the Configuration parameter may be
 * ignored.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the WSS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whether the request was
 *                                 accepted or rejected.
 *
 * @param[in]  Configuration       CCCD that
 *                                 will be sent if the request is
 *                                 accepted.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. WSS error codes can be
 *              found in qapi_ble_wss.h (QAPI_BLE_WSS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_WSS_Read_Weight_Measurement_CCCD_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, uint16_t Configuration);

/**
 * @brief
 * Responds to a write request
 * received from a WSS client for a WSS characteristic's CCCD.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * qapi_ble_wsstypes.h (WSS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the WSS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whether the request was
 *                                 accepted or rejected.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. WSS error codes can be
 *              found in qapi_ble_wss.h (QAPI_BLE_WSS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_WSS_Write_Weight_Measurement_CCCD_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode);

/**
 * @brief
 * Indicates a WSS Weight Measurement
 * to a WSS client.
 *
 * @details
 * This function sends an indication that provides a guarantee that the
 * WSS client will receive the value, since the WSS client must confirm
 * that it has been received.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the WSS
 * Measurment Characteristic's CCCD has been
 * previously configured for indications. A WSS client must have written
 * the WSS Measurement Characteristic's CCCD to enable indications.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the WSS instance that
 *                                 received the request.
 *
 * @param[in]  ConnectionID        GATT connection ID for the
 *                                 connection with the WSS client.
 *
 * @param[in]  MeasurementData     Pointer to the WSS Measurement data
 *                                 that will be sent in the indication.
 *
 * @return      Positive, nonzero if successful (represents the
 *              GATT transaction ID for the indication).
 *
 * @return      An error code if negative. WSS error codes can be
 *              found in qapi_ble_wss.h (QAPI_BLE_WSS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_WSS_Indicate_Weight_Measurement(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_WSS_Weight_Measurement_Data_t *MeasurementData);

   /* WSS client API.                                                   */

/**
 * @brief
 * Parses a value received in
 * a GATT indication from a remote WSS server, interpreting it as a WSS
 * Measurement.
 *
 * @param[in]  ValueLength   Length of the value received from the
 *                           WSS server.
 *
 * @param[in]  Value         Value received from the
 *                           WSS server.
 *
 * @param[out]  MeasurementData   Pointer that will hold the
 *                                WSS Measurement data if
 *                                this function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. WSS error codes can be
 *              found in qapi_ble_wss.h (QAPI_BLE_WSS_ERROR_XXX).
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_WSS_Decode_Weight_Measurement(uint32_t ValueLength, uint8_t *Value, qapi_BLE_WSS_Weight_Measurement_Data_t *MeasurementData);

/**
 * @}
 */

#endif

