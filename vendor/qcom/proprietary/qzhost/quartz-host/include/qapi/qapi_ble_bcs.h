/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_ble_bcs.h
 *
 * @brief
 * QCA QAPI for Bluetopia Bluetooth Body Composition Service
 * (GATT based) API Type Definitions, Constants, and
 * Prototypes.
 *
 * @details
 * The Body Composition Service (BCS) programming interface defines the
 * protocols and procedures to be used to implement the Body Composition
 * Service capabilities.
 */

#ifndef __QAPI_BLE_BCS_H__
#define __QAPI_BLE_BCS_H__

#include "./qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.          */
#include "./qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.    */
#include "./qapi_ble_gatt.h"      /* qapi GATT prototypes.                    */
#include "./qapi_ble_bcstypes.h"  /* QAPI BCS prototypes.                     */

/** @addtogroup qapi_ble_services
@{
*/

   /** @name Error Return Codes

      Error codes that are smaller than these (less than -1000) are
      related to the Bluetooth Protocol Stack itself (see
      qapi_ble_errros.h).
      @{ */
#define QAPI_BLE_BCS_ERROR_INVALID_PARAMETER                          (-1000)
/**< Invalid parameter. */
#define QAPI_BLE_BCS_ERROR_INVALID_BLUETOOTH_STACK_ID                 (-1001)
/**< Invalid Bluetooth Stack ID. */
#define QAPI_BLE_BCS_ERROR_INSUFFICIENT_RESOURCES                     (-1002)
/**< Insufficient resources. */
#define QAPI_BLE_BCS_ERROR_INSUFFICIENT_BUFFER_SPACE                  (-1003)
/**< Insufficient buffer space. */
#define QAPI_BLE_BCS_ERROR_SERVICE_ALREADY_REGISTERED                 (-1004)
/**< Services is already registered. */
#define QAPI_BLE_BCS_ERROR_INVALID_INSTANCE_ID                        (-1005)
/**< Invalid instance ID. */
#define QAPI_BLE_BCS_ERROR_MALFORMATTED_DATA                          (-1006)
/**< Malformatted data. */
#define QAPI_BLE_BCS_ERROR_BODY_COMPOSITON_MEASUREMENT_FORMAT_FAILED  (-1007)
/**< BCS measurement format failed. */

/** @} */ /* end namegroup */

/**
 * Structure that contains the attribute handles that must be
 * cached by a BCS client in order to only do service discovery once.
 */
typedef struct qapi_BLE_BCS_Client_Information_s
{
   /**
    * BCS feature attribute handle.
    */
   uint32_t BC_Feature_Handle;

   /**
    * BCS measurement attribute handle.
    */
   uint32_t BC_Measurement_Handle;

   /**
    * BCS measurement CCCD attribute handle.
    */
   uint32_t BC_Measurement_CCCD_Handle;
} qapi_BLE_BCS_Client_Information_t;

#define QAPI_BLE_BCS_CLIENT_INFORMATION_DATA_SIZE        (sizeof(qapi_BLE_BCS_Client_Information_t))
/**< Size of the #qapi_BLE_BCS_Client_Information_t structure. */

/**
 * Structure that contains all of the per client data that is to
 * be stored by the BCS server.
 */
typedef struct qapi_BLE_BCS_Server_Information_s
{
   /**
    * BCS measurement CCCD.
    */
   uint32_t BC_Measurement_CCCD;
} qapi_BLE_BCS_Server_Information_t;

#define QAPI_BLE_BCS_SERVER_INFORMATION_DATA_SIZE        (sizeof(qapi_BLE_BCS_Server_Information_t))
/**< Size of the #qapi_BLE_BCS_Server_Information_t structure. */

/**
 * Structure that represents the date/time data for BCS.
 */
typedef struct qapi_BLE_BCS_Date_Time_Data_s
{
   /**
    * Year.
    */
   uint32_t Year;

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
} qapi_BLE_BCS_Date_Time_Data_t;

#define QAPI_BLE_BCS_DATE_TIME_DATA_SIZE                 (sizeof(qapi_BLE_BCS_Date_Time_Data_t))
/**< Size of the #qapi_BLE_BCS_Date_Time_Data_t structure. */

#define QAPI_BLE_BCS_DATE_TIME_VALID(_x)                 ((QAPI_BLE_GATT_DATE_TIME_VALID_YEAR(((_x)).Year)) && (QAPI_BLE_GATT_DATE_TIME_VALID_MONTH(((_x)).Month)) && (QAPI_BLE_GATT_DATE_TIME_VALID_DAY(((_x)).Day)) && (QAPI_BLE_GATT_DATE_TIME_VALID_HOURS(((_x)).Hours)) && (QAPI_BLE_GATT_DATE_TIME_VALID_MINUTES(((_x)).Minutes)) && (QAPI_BLE_GATT_DATE_TIME_VALID_SECONDS(((_x)).Seconds)))
/**<
 * This macro is a utility macro that exists to validate that a specified
 * date/time is valid. The only parameter to this function is the
 * #qapi_BLE_BCS_Date_Time_Data_t structure to validate. This macro
 * returns TRUE if the date/time is valid or FALSE otherwise.
 */

/**
 * Structure that represents the BCS measurement data.
 */
typedef struct qapi_BLE_BCS_BC_Measurement_Data_s
{
   /**
    * Bitmask that indicates the optional fields and features that may
    * be included for a BCS measurement.
    *
    * Valid values have the form QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_XXX and
    * can be found in qapi_ble_bcstypes.h.
    */
   uint32_t                      Flags;

   /**
    * Body fat percentage.
    */
   uint32_t                      BodyFatPercentage;

   /**
    * Time stamp for the BCS measurement.
    */
   qapi_BLE_BCS_Date_Time_Data_t TimeStamp;

   /**
    * User ID.
    */
   uint8_t                       UserID;

   /**
    * Basal metabolism.
    */
   uint32_t                      BasalMetabolism;

   /**
    * Muscle percentage.
    */
   uint32_t                      MusclePercentage;

   /**
    * Muscle mass.
    */
   uint32_t                      MuscleMass;

   /**
    * Fat free mass.
    */
   uint32_t                      FatFreeMass;

   /**
    * Soft lean mass.
    */
   uint32_t                      SoftLeanMass;

   /**
    * Body water mass.
    */
   uint32_t                      BodyWaterMass;

   /**
    * Impedance.
    */
   uint32_t                      Impedance;

   /**
    * Weight.
    */
   uint32_t                      Weight;

   /**
    * Height.
    */
   uint32_t                      Height;
} qapi_BLE_BCS_BC_Measurement_Data_t;

#define QAPI_BLE_BCS_BC_MEASUREMENT_DATA_SIZE            (sizeof(qapi_BLE_BCS_BC_Measurement_Data_t))
/**<
 * Size of the #qapi_BLE_BCS_BC_Measurement_Data_t structure.
 */

/**
 * Enumeration of all the events generated by the BCS
 * service. These are used to determine the type of each event generated
 * and to ensure the proper union element is accessed for the
 * #qapi_BLE_BCS_Event_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_ET_BCS_SERVER_READ_CCCD_REQUEST_E,
   /**< Read a CCCD request event. */
   QAPI_BLE_ET_BCS_SERVER_WRITE_CCCD_REQUEST_E,
   /**< Write a CCCD request event. */
   QAPI_BLE_ET_BCS_CONFIRMATION_DATA_E
   /**< Confirmation request event. */
} qapi_BLE_BCS_Event_Type_t;

/**
 * Structure that represents the format for the data that is dispatched
 * to a BCS server when a BCS client has sent a request to read a BCS
 * characteristic's CCCD.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_BCS_Read_CCCD_Request_Response() function.
 */
typedef struct qapi_BLE_BCS_Read_CCCD_Request_Data_s
{
   /**
    * BCS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the BCS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the BCS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * Bluetooth address of the BCS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;
} qapi_BLE_BCS_Read_CCCD_Request_Data_t;

#define QAPI_BLE_BCS_READ_CCCD_REQUEST_DATA_SIZE         (sizeof(qapi_BLE_BCS_Read_CCCD_Request_Data_t))
/**<
 * Size of the #qapi_BLE_BCS_Read_CCCD_Request_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a BCS server when a BCS client has sent a request to write a BCS
 * characteristic's CCCD.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_BCS_Write_CCCD_Request_Response() function.
 */
typedef struct qapi_BLE_BCS_Write_CCCD_Request_Data_s
{
   /**
    * BCS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the BCS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the BCS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * Bluetooth address of the BCS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;

   /**
    * The Client Characteristic Configuration Descriptor (CCCD) value
    * that has been requested to be written.
    */
   uint32_t                        ClientConfiguration;
} qapi_BLE_BCS_Write_CCCD_Request_Data_t;

#define QAPI_BLE_BCS_WRITE_CCCD_REQUEST_DATA_SIZE        (sizeof(qapi_BLE_BCS_Write_CCCD_Request_Data_t))
/**<
 * Size of the #qapi_BLE_BCS_Write_CCCD_Request_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a BCS server when a BCS client has responded to an outstanding
 * indication that was previously sent by the BCS server.
 */
typedef struct qapi_BLE_BCS_Confirmation_Data_s
{
   /**
    * The BCS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the BCS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the BCS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * Bluetooth address of the BCS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;

   /**
    * Status of the indication.
    */
   uint8_t                         Status;

   /**
    * Number of bytes that were written by the indication.
    */
   uint16_t                        BytesWritten;
} qapi_BLE_BCS_Confirmation_Data_t;

#define QAPI_BLE_BCS_CONFIRMATION_DATA_SIZE              (sizeof(qapi_BLE_BCS_Confirmation_Data_t))
/**<
 * Size of the #qapi_BLE_BCS_Confirmation_Data_t structure.
 */

/**
 * Structure that represents the container structure for holding all the
 * event data for a Body Composition Service (BCS) instance.
 */
typedef struct qapi_BLE_BCS_Event_Data_s
{
   /**
    * Event type used to determine the appropriate union member of
    * the Event_Data field to access.
    */
   qapi_BLE_BCS_Event_Type_t                  Event_Data_Type;

   /**
    * Total size of the data contained in the event.
    */
   uint32_t                                   Event_Data_Size;
   union
   {
      /**
       * BCS read CCCD data.
       */
      qapi_BLE_BCS_Read_CCCD_Request_Data_t  *BCS_Read_CCCD_Request_Data;

      /**
       * BCS write CCCD data.
       */
      qapi_BLE_BCS_Write_CCCD_Request_Data_t *BCS_Write_CCCD_Request_Data;

      /**
       * BCS confirmation data.
       */
      qapi_BLE_BCS_Confirmation_Data_t       *BCS_Confirmation_Data;
   }
   /**
    * Event data. @newpagetable
    */
   Event_Data;
} qapi_BLE_BCS_Event_Data_t;

#define QAPI_BLE_BCS_EVENT_DATA_SIZE                     (sizeof(qapi_BLE_BCS_Event_Data_t))
/**<
 * Size of the #qapi_BLE_BCS_Event_Data_t structure.
 */

/**
 * @brief
 * This declared type represents the prototype function for
 * a BCS Service Event Receive Data Callback. This function will be called
 * whenever a BCS Service Event occurs that is associated with the
 * specified Bluetooth stack ID.

 * @details
 * The caller can use the contents of the BCS Profile Event Data
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
 * anyway because another BCS Service Event will not be processed while this
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
 * @param[in]  BCS_Event_Data        Pointer to a structure that contains
 *                                   information about the event that has
 *                                   occurred.
 *
 * @param[in]  CallbackParameter     User-defined value that was supplied
 *                                   as an input parameter from a prior
 *                                   BCS request.
 *
 * @return None.
 */
typedef void (QAPI_BLE_BTPSAPI *qapi_BLE_BCS_Event_Callback_t)(uint32_t BluetoothStackID, qapi_BLE_BCS_Event_Data_t *BCS_Event_Data, uint32_t CallbackParameter);

   /* BCS Server API.                                                   */

/**
 * @brief
 * Initializes a BCS server on a specified Bluetooth Stack.
 *
 * @details
 * Only one BCS server may be open at a time, per the Bluetooth Stack ID.
 *
 * @param[in]  BluetoothStackID     Unique identifier assigned to this
 *                                  Bluetooth Protocol Stack via a
 *                                  call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  Flags                Flags that are used to register BCS
 *                                  for the specified transport. These
 *                                  flags can be found in
 *                                  qapi_ble_bcstypes.h and have the form
 *                                  QAPI_BLE_BCS_SERVICE_FLAGS_XXX.
 *
 * @param[in]  EventCallback        BCS Event Callback that will
 *                                  receive BCS server events.
 *
 * @param[in]  CallbackParameter    User-defined value that will be
 *                                  received with the specified
 *                                  EventCallback parameter.
 *
 * @param[out]  ServiceID           Unique GATT service ID of the
 *                                  registered BCS service returned from the
 *                                  qapi_BLE_GATT_Register_Service() API.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of BCS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BCS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BCS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BTGATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BCS_Initialize_Service(uint32_t BluetoothStackID, uint32_t Flags, qapi_BLE_BCS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

/**
 * @brief
 * Initializes a BCS server on a specified Bluetooth Stack.
 *
 * @details
 * Unlike qapi_BLE_BCS_Initialize_Service(), this function allows the
 * application to select a handle range in GATT to store the service.
 *
 * Only one BCS server may be open at a time, per the Bluetooth Stack ID.
 *
 * @param[in]  BluetoothStackID     Unique identifier assigned to this
 *                                  Bluetooth Protocol Stack via a
 *                                  call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  Flags                Flags that are used to register BCS
 *                                  for the specified transport. These
 *                                  flags can be found in
 *                                  qapi_ble_bcstypes.h and have the form
 *                                  QAPI_BLE_BCS_SERVICE_FLAGS_XXX.
 *
 * @param[in]  EventCallback        BCS Event Callback that will
 *                                  receive BCS server events.
 *
 * @param[in]  CallbackParameter    User-defined value that will be
 *                                  received with the specified
 *                                  EventCallback parameter.
 *
 * @param[out]  ServiceID           Unique GATT service ID of the
 *                                  registered BCS service returned from
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
 *              be the Service Instance ID of BCS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BCS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BCS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BTGATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BCS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, uint32_t Flags, qapi_BLE_BCS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

/**
 * @brief
 * Cleans up and frees all resources
 * associated with a Body Composition Service (BCS) Instance.
 *
 * @details
 * After this function is called, no other BCS
 * function can be called until after a successful call to either of the
 * qapi_BLE_BCS_Initialize_XXX() functions are performed.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of the qapi_BLE_BCS_Initialize_XXX()
 *                                 functions.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BCS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BCS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BCS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * Queries the number of attributes
 * that are contained in the BCS service that is registered with a
 * call to qapi_BLE_BCS_Initialize_Service() or
 * qapi_BLE_BCS_Initialize_Service_Handle_Range().
 *
 * @return   Positive, nonzero, number of attributes that can be
 *           registered by a BCS service instance. \n
 *           Zero on failure.
 */
QAPI_BLE_DECLARATION unsigned int QAPI_BLE_BTPSAPI qapi_BLE_BCS_Query_Number_Attributes(void);

/**
 * @brief
 * Sets the supported BCS features
 * for the BCS server.
 *
 * @details
 * The SupportedFeatures parameter is a bitmask made up of bits of the form
 * QAPI_BLE_BCS_BC_FEATURE_FLAG_XXX.
 *
 * This function must be called after the BCS service is registered with
 * a successful call to qapi_BLE_BCS_Initialize_XXX() in order to set the default
 * features of the BCS server.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_BCS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  SupportedFeatures    Bitmask that contains the
 *                                  supported features that the BCS
 *                                  server will support if this function
 *                                  is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BCS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BCS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BCS_Set_BC_Feature(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t SupportedFeatures);

/**
 * @brief
 * Queries the supported BCS features
 * for the BCS server.
 *
 * @details
 * The SupportedFeatures parameter is a bitmask made up of bits of the form
 * QAPI_BLE_BCS_BC_FEATURE_FLAG_XXX.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_BCS_Initialize_XXX()
 *                                 functions.
 *
 * @param[out]  SupportedFeatures    Pointer to a bitmask that will
 *                                   hold the supported features that the
 *                                   BCS server supports if this function
 *                                   is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BCS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BCS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BCS_Query_BC_Feature(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t *SupportedFeatures);

/**
 * @brief
 * Responds to a read
 * request from a BCS client for a BCS characteristic's CCCD.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * QPAI_BLE_BCS_ERROR_CODE_XXX from qapi_ble_bcstypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * The Client_Configuration parameter is only required if the
 * ErrorCode parameter is QAPI_BLE_BCS_ERROR_CODE_SUCCESS.
 * Otherwise it will be ignored.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_BCS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  ErrorCode           Error code for the request, which
 *                                 indicates whether the request was
 *                                 successful or an error has occured.
 *
 * @param[in]  Client_Configuration   Current value of the CCCD that will be sent
 *                                    if the request was successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BCS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BCS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BCS_Read_CCCD_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, uint32_t Client_Configuration);

/**
 * @brief
 * Responds to a write
 * request from a BCS client for a BCS characteristic's CCCD.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * QPAI_BLE_BCS_ERROR_CODE_XXX from qapi_ble_bcstypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_BCS_Initialize_XXX()
 *                                 functions.
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
 *                 QAPI_BLE_BCS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BCS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BCS_Write_CCCD_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode);

/**
 * @brief
 * Sends an indication for a
 * BCS characteristic's value to a BCS client.
 *
 * @details
 * This function sends an indication, which provides a guarantee that the
 * BCS client will receive the value, since the BCS client must confirm
 * that it has been received.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the BCS
 * characteristic that is going to be indicated has been previously configured
 * for indications. A BCS client must have written the BCS characteristic's
 * CCCD to enable indications.
 *
 * @note1hang
 * This function will determine if the BC_Measurement field needs to be
 * split and sent in multiple indications. If this is the case, this
 * function will set the
 * QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_MULTIPLE_PACKET_MEASUREMENT flag in
 * the Flags field of the #qapi_BLE_BCS_BC_Measurement_Data_t structure
 * and will send multiple indications for the BC Measurement.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_BCS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the
 *                                 connection for the indication.
 *
 * @param[in]  BC_Measurement      Pointer that contains the
 *                                 BCS measurement value that will be
 *                                 indicated to the BCS client.
 *
 * @return      Positive nonzero if successful (represents the
 *              GATT Transaction ID for the indication).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BCS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BCS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID \n
 *                 QAPI_BLE_BCS_ERROR_BODY_COMPOSITON_MEASUREMENT_FORMAT_FAILED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BCS_Indicate_Body_Composition_Measurement(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_BCS_BC_Measurement_Data_t *BC_Measurement);

   /* BCS Client API.                                                   */

/**
 * @brief
 * Parses a value received in an
 * indication from a remote BCS server interpreting it as a Body Composition
 * Measurement.
 *
 * @note1hang
 * If the BC_Measurement's Flags Body Fat Percentage field is set to
 * BCS_BC_MEASUREMENT_UNSUCCESSFUL, all bits except
 * QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_MEASUREMENT_UNITS_IMPERIAL,
 * QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_TIME_STAMP_PRESENT, and
 * QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_USER_ID_PRESENT will be cleared if
 * set. The timestamp and user ID should not be included in an
 * unsuccessful measurement.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            BCS server.
 *
 * @param[in]  Value          Value received from the
 *                            BCS server.
 *
 * @param[out]  BC_Measurement    Pointer that will hold the decoded
 *                                BCS Measurement data if this
 *                                function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_BCS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BCS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_BCS_Decode_Body_Composition_Measurement(uint32_t ValueLength, uint8_t *Value, qapi_BLE_BCS_BC_Measurement_Data_t *BC_Measurement);

/** @}
 */

#endif

