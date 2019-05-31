/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_ble_rscs.h
 *
 * @brief
 * QCA QAPI for Bluetopia Bluetooth Running Speed and Cadence Service (RSCS)
 * (GATT based) API Type Definitions, Constants, and Prototypes.
 *
 * @details
 * The Running Speed and Cadence Service programming interface defines
 * the protocols and procedures to be used to implement the Running Speed
 * and Cadence Service capabilities.
 */

#ifndef __QAPI_BLE_RSCS_H__
#define __QAPI_BLE_RSCS_H__

#include "./qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.          */
#include "./qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.    */
#include "./qapi_ble_gatt.h"      /* QAPI GATT prototypes.                    */
#include "./qapi_ble_rscstypes.h" /* QAPI RSCS prototypes.                    */

/**
 * @addtogroup qapi_ble_services
 * @{
 */

   /* Error Return Codes.                                               */

   /* Error Codes that are smaller than these (less than -1000) are     */
   /* related to the Bluetooth Protocol Stack itself (see               */
   /* qapi_ble_errros.h).                                               */
#define QAPI_BLE_RSCS_ERROR_INVALID_PARAMETER            (-1000)
/**< Invalid parameter. */
#define QAPI_BLE_RSCS_ERROR_INVALID_BLUETOOTH_STACK_ID   (-1001)
/**< Invalid Bluetooth Stack ID. */
#define QAPI_BLE_RSCS_ERROR_INSUFFICIENT_RESOURCES       (-1002)
/**< Insufficient resources. */
#define QAPI_BLE_RSCS_ERROR_INSUFFICIENT_BUFFER_SPACE    (-1003)
/**< Insufficient buffer space. */
#define QAPI_BLE_RSCS_ERROR_SERVICE_ALREADY_REGISTERED   (-1004)
/**< Service is already registered. */
#define QAPI_BLE_RSCS_ERROR_INVALID_INSTANCE_ID          (-1005)
/**< Invalid service instance ID. */
#define QAPI_BLE_RSCS_ERROR_MALFORMATTED_DATA            (-1006)
/**< Malformatted data. */
#define QAPI_BLE_RSCS_ERROR_INDICATION_OUTSTANDING       (-1007)
/**< Indication is outstanding. */

/**
 * Structure that contains the attribute handles that will need to be
 * cached by a RSCS client in order to only do service discovery once.
 */
typedef struct qapi_BLE_RSCS_Client_Information_s
{
   /**
    * RSCS Measurement attribute handle.
    */
   uint16_t RSC_Measurement;

   /**
    * RSCS Measurement Client Characteristic Configuration
    * Descriptor (CCCD) attribute handle.
    */
   uint16_t RSC_Measurement_Client_Configuration;

   /**
    * RSCS Feature attribute handle.
    */
   uint16_t RSC_Feature;

   /**
    * RSCS Sensor Location attribute handle.
    */
   uint16_t Sensor_Location;

   /**
    * RSCS SC Control Point attribute handle.
    */
   uint16_t SC_Control_Point;

   /**
    * RSCS SC Control Point CCCD attribute handle.
    */
   uint16_t SC_Control_Point_Client_Configuration;
} qapi_BLE_RSCS_Client_Information_t;

#define QAPI_BLE_RSCS_CLIENT_INFORMATION_DATA_SIZE       (sizeof(qapi_BLE_RSCS_Client_Information_t))
/**<
 * Size of the #qapi_BLE_RSCS_Client_Information_t structure.
 */

/**
 * Structure that contains all of the per client data that will need to
 * be stored by an RSCS server.
 */
typedef struct qapi_BLE_RSCS_Server_Information_s
{
   /**
    * RSCS Measurement CCCD.
    */
   uint16_t RSC_Measurement_Client_Configuration;

   /**
    * RSCS SC Control Point CCCD.
    */
   uint16_t SC_Control_Point_Client_Configuration;
} qapi_BLE_RSCS_Server_Information_t;

#define QAPI_BLE_RSCS_SERVER_INFORMATION_DATA_SIZE       (sizeof(qapi_BLE_RSCS_Server_Information_t))
/**<
 * Size of the #qapi_BLE_RSCS_Server_Information_t structure.
 */

/**
 * Structure that represents the RSCS Measurement data.
 */
typedef struct qapi_BLE_RSCS_RSC_Measurement_Data_s
{
   /**
    * Bitmask that indicates the optional fields and features that may
    * be included for a RSCS Measurement.
    *
    * Valid values have the form QAPI_BLE_RSCS_RSC_MEASUREMENT_XXX and
    * can be found in qapi_ble_rscstypes.h.
    */
   uint8_t  Flags;

   /**
    * Instantaneous speed.
    */
   uint16_t Instantaneous_Speed;

   /**
    * Instantaneous cadence.
    */
   uint8_t  Instantaneous_Cadence;

   /**
    * Instantaneous stride length.
    */
   uint16_t Instantaneous_Stride_Length;

   /**
    * Total distance.
    */
   uint32_t Total_Distance;
} qapi_BLE_RSCS_RSC_Measurement_Data_t;

#define QAPI_BLE_RSCS_RSC_MEASUREMENT_DATA_SIZE          (sizeof(qapi_BLE_RSCS_RSC_Measurement_Data_t))
/**<
 * Size of the #qapi_BLE_RSCS_RSC_Measurement_Data_t structure.
 */

/**
 * Enumeration that represents the valid Read Request types that a server
 * may receive in a
 * QAPI_BLE_ET_RCS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E or
 * QAPI_BLE_ET_RSC_SERVER_CLIENT_CONFIGURATION_UPDATE_E event. This is
 * also used by the qapi_BLE_RSC_Send_Notification() to denote the
 * characteristic value to notify.
 *
 * For each event, it is up to the application to return (or write) the
 * correct Client Configuration descriptor based on this value.
 */
typedef enum
{
   QAPI_BLE_CT_RSC_MEASUREMENT_E,  /**< Measurement. */
   QAPI_BLE_CT_RSC_CONTROL_POINT_E /**< RSC Control Point. */
} qapi_BLE_RSCS_Characteristic_Type_t;

/**
 * Enumeration that represents all the events generated by the RSC
 * service. These are used to determine the type of each event
 * generated and to ensure the proper union element is accessed for the
 * #qapi_BLE_RSCS_Event_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_ET_RSCS_READ_CLIENT_CONFIGURATION_REQUEST_E,
   /**< Read CCCD request event. */
   QAPI_BLE_ET_RSCS_CLIENT_CONFIGURATION_UPDATE_E,
   /**< Write CCCD request event. */
   QAPI_BLE_ET_RSCS_SC_CONTROL_POINT_COMMAND_E,
   /**< Write SC Control Point request event. */
   QAPI_BLE_ET_RSCS_CONFIRMATION_DATA_E
   /**< Confirmation event. */
} qapi_BLE_RSCS_Event_Type_t;

/**
 * Structure that represents the format for the data that is dispatched
 * to an RSCS server when an RSCS client has sent a request to read an RSCS
 * characteristic's CCCD.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_RSCS_Read_Client_Configuration_Response() function.
 */
typedef struct qapi_BLE_RSCS_Read_Client_Configuration_Data_s
{
   /**
    * RSCS instance that dispatched the event.
    */
   uint32_t                            InstanceID;

   /**
    * GATT connection ID for the connection with the RSCS client
    * that made the request.
    */
   uint32_t                            ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                            TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the RSCS client.
    */
   qapi_BLE_GATT_Connection_Type_t     ConnectionType;

   /**
    * Bluetooth address of the RSCS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                  RemoteDevice;

   /**
    * Identifies the requested CCCD based on the RSCS characteristic
    * type.
    */
   qapi_BLE_RSCS_Characteristic_Type_t ClientConfigurationType;
} qapi_BLE_RSCS_Read_Client_Configuration_Data_t;

#define QAPI_BLE_RSCS_READ_CLIENT_CONFIGURATION_DATA_SIZE  (sizeof(qapi_BLE_RSCS_Read_Client_Configuration_Data_t))
/**<
 * Size of the #qapi_BLE_RSCS_Read_Client_Configuration_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an RSCS server when an RSCS client has sent a request to write an
 * RSCS characteristic's CCCD.
 */
typedef struct qapi_BLE_RSCS_Client_Configuration_Update_Data_s
{
   /**
    * RSCS instance that dispatched the event.
    */
   uint32_t                            InstanceID;

   /**
    * GATT connection ID for the connection with the RSCS client
    * that made the request.
    */
   uint32_t                            ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                            TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the RSCS client.
    */
   qapi_BLE_GATT_Connection_Type_t     ConnectionType;

   /**
    * Bluetooth address of the RSCS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                  RemoteDevice;

   /**
    * Identifies the requested CCCD based on the RSCS Characteristic
    * type.
    */
   qapi_BLE_RSCS_Characteristic_Type_t ClientConfigurationType;

   /**
    * CCCD value that has been requested to be written.
    */
   uint16_t                            ClientConfiguration;
} qapi_BLE_RSCS_Client_Configuration_Update_Data_t;

#define QAPI_BLE_RSCS_CLIENT_CONFIGURATION_UPDATE_DATA_SIZE  (sizeof(qapi_BLE_RSCS_Client_Configuration_Update_Data_t))
/**<
 * Size of the #qapi_BLE_RSCS_Client_Configuration_Update_Data_t
 * structure.
 */

#define QAPI_BLE_MAXIMUM_SUPPORTED_SENSOR_LOCATIONS      (17)
/**<
 * Maximum number of supported sensor locations.
 */

/**
 * Structure that represents the format of Supported Sensor
 * Location values that will be used to respond to Supported Sensor
 * Locations request made by remote device.
 */
typedef struct qapi_BLE_RSCS_RSCP_Supported_Sensor_Locations_s
{
   /**
   * Byte array of multiple sensor locations.
   */
   uint8_t SensorLocations[QAPI_BLE_MAXIMUM_SUPPORTED_SENSOR_LOCATIONS];

   /**
    * Number of sensor locations.
    */
   uint8_t NumberOfSensorLocations;
} qapi_BLE_RSCS_SCCP_Supported_Sensor_Locations_t;

#define QAPI_BLE_RSCS_SCCP_SUPPORTED_SENSOR_LOCATIONS_SIZE  (sizeof(qapi_BLE_RSCS_SCCP_Supported_Sensor_Locations_t))
/**<
 * Size of the #qapi_BLE_RSCS_SCCP_Supported_Sensor_Locations_t structure.
 */

/**
 * Enumeration that represents the valid values that may be set as the
 * value for the OpCode field of the SC Control Point characteristic.
 */
typedef enum
{
   QAPI_BLE_SCC_SET_CUMULATIVE_VALUE_E              = QAPI_BLE_RSCS_SC_CONTROL_POINT_OPCODE_SET_CUMULATIVE_VALUE,
   /**< Set cumulative value op code. */
   QAPI_BLE_SCC_START_SENSOR_CALIBRATION_E          = QAPI_BLE_RSCS_SC_CONTROL_POINT_OPCODE_START_SENSOR_CALIBRATION,
   /**< Start sensor calibration op code. */
   QAPI_BLE_SCC_UPDATE_SENSOR_LOCATION_E            = QAPI_BLE_RSCS_SC_CONTROL_POINT_OPCODE_UPDATE_SENSOR_LOCATION,
   /**< Update sensor location op code. */
   QAPI_BLE_SCC_REQUEST_SUPPORTED_SENSOR_LOCATION_E = QAPI_BLE_RSCS_SC_CONTROL_POINT_OPCODE_REQUEST_SUPPORTED_SENSOR_LOCATIONS
   /**< Request supported sensor location op code. */
} qapi_BLE_RSCS_SCCP_Command_Type_t;

/**
 * Enumeration that represents the valid values that may be set as the
 * value for the Response Opcode field of the Specific Operation Control
 * Point characteristic.
 */
typedef enum
{
   QAPI_BLE_SCC_SUCCESS_E              = QAPI_BLE_RSCS_SC_CONTROL_POINT_RESPONSE_CODE_SUCCESS,
   /**< Success response code. */
   QAPI_BLE_SCC_OPCODE_NOT_SUPPORTED_E = QAPI_BLE_RSCS_SC_CONTROL_POINT_RESPONSE_OPCODE_NOT_SUPPORTED,
   /**< Opcode not supported response code. */
   QAPI_BLE_SCC_INVALID_PARAMETER_E    = QAPI_BLE_RSCS_SC_CONTROL_POINT_RESPONSE_INVALID_PARAMETER,
   /**< Invalid parameter response code. */
   QAPI_BLE_SCC_OPERATION_FAILED_E     = QAPI_BLE_RSCS_SC_CONTROL_POINT_RESPONSE_OPERATION_FAILED
   /**< Operation failed response code. */
} qapi_BLE_RSCS_SCCP_Response_Value_t;

/**
 * Structure that represents the format of the RSCS SC operations Control
 * Point Command Request data
 */
typedef struct qapi_BLE_RSCS_SC_Control_Point_Format_Data_s
{
   /**
    * Control point command type.
    */
   qapi_BLE_RSCS_SCCP_Command_Type_t CommandType;
   union
   {
      /**
       * Cumalitive value.
       */
      uint32_t CumulativeValue;

      /**
       * Sensor location.
       */
      uint8_t  SensorLocation;
   }
   /**
    * Filter parameters.
    */
   FilterParameters;
} qapi_BLE_RSCS_SC_Control_Point_Format_Data_t;

#define QAPI_BLE_RSCS_SC_CONTROL_POINT_FORMAT_DATA_SIZE  (sizeof(qapi_BLE_RSCS_SC_Control_Point_Format_Data_t))
/**<
 * Size of the #qapi_BLE_RSCS_SC_Control_Point_Format_Data_t structure.
 */

/**
 * Structure that represents the format of the RSCS SC operations Control
 * Point Command Response data
 */
typedef struct qapi_BLE_RSCS_SC_Control_Point_Response_Data_s
{
   /**
    * Response code op-code.
    */
   uint8_t                                         ResponseOpCode;

   /**
    * Request op-code.
    */
   qapi_BLE_RSCS_SCCP_Command_Type_t               RequestOpCode;

   /**
    * Supported sensor locations.
    */
   qapi_BLE_RSCS_SCCP_Supported_Sensor_Locations_t SupportedSensorLocations;
} qapi_BLE_RSCS_SC_Control_Point_Response_Data_t;

#define QAPI_BLE_RSCS_SC_CONTROL_POINT_RESPONSE_DATA_SIZE  (sizeof(qapi_BLE_RSCS_SC_Control_Point_Response_Data_t))
/**<
 * Size of the #qapi_BLE_RSCS_SC_Control_Point_Response_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to an RSCS server when an RSCS client has sent a request to write the
 * RSCS SC Control Point.
 */
typedef struct qapi_BLE_RSCS_SC_Control_Point_Command_Data_s
{
   /**
    * RSCS instance that dispatched the event.
    */
   uint32_t                                     InstanceID;

   /**
    * GATT connection ID for the connection with the RSCS client
    * that made the request.
    */
   uint32_t                                     ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                                     TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the RSCS client.
    */
   qapi_BLE_GATT_Connection_Type_t              ConnectionType;

   /**
    * Bluetooth address of the RSCS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                           RemoteDevice;

   /**
    * SC Control Point request data that has been requested to be
    * written.
    */
   qapi_BLE_RSCS_SC_Control_Point_Format_Data_t FormatData;
} qapi_BLE_RSCS_SC_Control_Point_Command_Data_t;

#define QAPI_BLE_RSCS_SC_CONTROL_POINT_COMMAND_DATA_SIZE  (sizeof(qapi_BLE_RSCS_SC_Control_Point_Command_Data_t))
/**<
 * Size of the #qapi_BLE_RSCS_SC_Control_Point_Command_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a CGMS Server when an RSCS client has responded to an outstanding
 * indication that was previously sent by the RSCS server.
 */
typedef struct qapi_BLE_RSCS_Confirmation_Data_s
{
   /**
    * RSCS instance that dispatched the event.
    */
   uint32_t                            InstanceID;

   /**
    * GATT connection ID for the connection with the RSCS client
    * that made the request.
    */
   uint32_t                            ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the RSCS client.
    */
   qapi_BLE_GATT_Connection_Type_t     ConnectionType;

   /**
    * Bluetooth address of the RSCS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                  RemoteDevice;

   /**
    * Identifies the RSCS Characteristic that was previously
    * indicated.
    */
   qapi_BLE_RSCS_Characteristic_Type_t CharacteristicType;

   /**
    * Status of the indication.
    */
   uint8_t                             Status;
} qapi_BLE_RSCS_Confirmation_Data_t;

#define QAPI_BLE_RSCS_CONFIRMATION_DATA_SIZE             (sizeof(qapi_BLE_RSCS_Confirmation_Data_t))
/**<
 * Size of the #qapi_BLE_RSCS_Confirmation_Data_t structure.
 */

/**
 * Structure that represents the container structure for holding all the
 * event data for a RSCS instance.
 */
typedef struct qapi_BLE_RSCS_Event_Data_s
{
   /**
    * Event type used to determine the appropriate union member of
    * the Event_Data field to access.
    */
   qapi_BLE_RSCS_Event_Type_t Event_Data_Type;

   /**
    * Total size of the data contained in the event.
    */
   uint16_t                   Event_Data_Size;
   union
   {
      /**
       * RSCS Read CCCD event data.
       */
      qapi_BLE_RSCS_Read_Client_Configuration_Data_t   *RSCS_Read_Client_Configuration_Data;

      /**
       * RSCS Write CCCD event data.
       */
      qapi_BLE_RSCS_Client_Configuration_Update_Data_t *RSCS_Client_Configuration_Update_Data;

      /**
       * RSCS Write SC Control Point event data.
       */
      qapi_BLE_RSCS_SC_Control_Point_Command_Data_t    *RSCS_SC_Control_Point_Command_Data;

      /**
       * RSCS Confirmation event data.
       */
      qapi_BLE_RSCS_Confirmation_Data_t                *RSCS_Confirmation_Data;
   }
   /**
    * Event data.
    */
   Event_Data;
} qapi_BLE_RSCS_Event_Data_t;

#define QAPI_BLE_RSCS_EVENT_DATA_SIZE                    (sizeof(qapi_BLE_RSCS_Event_Data_t))
/**<
 * Size of the #qapi_BLE_RSCS_Event_Data_t structure.
 */

/**
 * @brief
 * This declared type represents the Prototype Function for an
 * RSCS Event Callback. This function will be called whenever a define
 * RSCS Event occurs within the Bluetooth Protocol Stack that is
 * specified with the specified Bluetooth Stack ID.
 *
 * @details
 * The event information is passed to the user in a #qapi_BLE_RSCS_Event_Data_t
 * structure. This structure contains all the information about the
 * event that occurred.
 *
 * The caller should use the contents of the RSCS Event Data
 * only in the context of this callback. If the caller requires
 * the data for a longer period of time, the callback function
 * must copy the data into another data buffer.
 *
 * This function is guaranteed not to be invoked more than once
 * simultaneously for the specified installed callback (i.e., this
 * function does not have be reentrant). It should be noted, however,
 * that if the same callback is installed more than once, the
 * callbacks will be called serially. Because of this, the processing
 * in this function should be as efficient as possible.
 *
 * It should also be noted that this function is called in the Thread
 * Context of a Thread that the user does not own. Therefore, processing
 * in this function should be as efficient as possible (this argument holds
 * anyway because another RSCS Event will not be processed while this
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
 * @param[in]  RSCS_Event_Data       Pointer to a structure that contains
 *                                   information about the event that has
 *                                   occurred.
 *
 * @param[in]  CallbackParameter     User-defined value that will be
 *                                   received with the RSCS Event data.
 *
 * @return None.
 */
typedef void (QAPI_BLE_BTPSAPI *qapi_BLE_RSCS_Event_Callback_t)(uint32_t BluetoothStackID, qapi_BLE_RSCS_Event_Data_t *RSCS_Event_Data, uint32_t CallbackParameter);

   /* RSCS server API.                                                  */

/**
 * @brief
 * Opens an RSCS server on a specified Bluetooth Stack.
 *
 * @details
 * Only one RSCS server may be open at a time, per the Bluetooth Stack ID.
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
 *                                  registered RSCS service returned from the
 *                                  qapi_BLE_GATT_Register_Service() API.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of RSCS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_RSCS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_RSCS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BT_GATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_RSCS_Initialize_Service(uint32_t BluetoothStackID, qapi_BLE_RSCS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

/**
 * @brief
 * Opens an RSCS server on a specified Bluetooth Stack with
 * the ability to control the location of the service in the GATT database.
 *
 * @details
 * Only one RSCS server may be open at a time, per the Bluetooth Stack ID.
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
 *                                       Range structure that, on input
 *                                       can be used to control the
 *                                       location of the service in the
 *                                       GATT database, and on output
 *                                       returns the handle range that
 *                                       the service is using in the GATT
 *                                       database.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of RSCS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_RSCS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_RSCS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BT_GATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_RSCS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, qapi_BLE_RSCS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

/**
 * @brief
 * Cleans up and frees all resources
 * associated with a RSCS Service Instance.
 *
 * @details
 * After this function is called, no other RSCS Service function can be
 * called until after a successful call to the qapi_BLE_RSCS_Initialize_Service()
 * function is performed.
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
 *                 QAPI_BLE_RSCS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_RSCS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_RSCS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * Queries the number of attributes
 * that are contained in the RSCS service that is registered with a
 * call to qapi_BLE_RSCS_Initialize_Service() or
 * qapi_BLE_RSCS_Initialize_Service_Handle_Range().
 *
 * @return   Positive, nonzero, number of attributes that would be
 *           registered by a RSCS service instance.
 *
 * @return   Zero on failure.
 */
QAPI_BLE_DECLARATION unsigned int QAPI_BLE_BTPSAPI qapi_BLE_RSCS_Query_Number_Attributes(void);

/**
 * @brief
 * Provides a mechanism for an RSCS server to
 * successfully respond to a received read client configuration request.
 *
 * @details
 * Possible Events:
 *
 *    QAPI_BLE_ET_RSCS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E
 *
 * @param[in]  BluetoothStackID        Unique identifier assigned to this
 *                                     Bluetooth Protocol Stack via a
 *                                     call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID              Service instance ID to close.
 *                                     This is the value that was returned
 *                                     from the qapi_BLE_RSCS_Initialize_Service()
 *                                     function.
 *
 * @param[in]  TransactionID           Transaction ID of the original
 *                                     read request. This value was
 *                                     received in the
 *                                     QAPI_BLE_ET_RSCS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E
 *                                     event.
 *
 * @param[in]  ClientConfiguration    Specifies the CCCD to send to
 *                                    the remote device.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_RSCS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_RSCS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_RSCS_Read_Client_Configuration_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t ClientConfiguration);

/**
 * @brief
 * Sends an RSCS Measurement
 * notification to a remote RSCS client.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from the
 *                                 qapi_BLE_RSCS_Initialize_Service()
 *                                 function.
 *
 * @param[in]  ConnectionID        GATT connection ID of the RSCS
 *                                 client that will receive the
 *                                 notification.
 *
 * @param[in]  MeasurementData     RSCS Measurement data that will be
 *                                 notified to the remote RSCS client.
 *
 * @return      Positive nonzero if successful (represents the
 *              length of the notification).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_RSCS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_RSCS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_RSCS_Notify_RSC_Measurement(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_RSCS_RSC_Measurement_Data_t *MeasurementData);

/**
 * @brief
 * Sets the supported RSCS Features
 * for the RSCS server.
 *
 * @details
 * The SupportedFeatures parameter is a bitmask with the bits between
 * QAPI_BLE_RSCS_FEATURE_FLAG_INSTANTANEOUS_STRIDE_LENGTH_MEASUREMENT_SUPPORTED
 * and QAPI_BLE_RSCS_FEATURE_FLAG_MULTIPLE_SENSOR_LOCATIONS_SUPPORTED.
 *
 * This function must be called after the RSCS service is registered with
 * a successful call to qapi_BLE_RSCS_Initialize_XXX() in order to set the default
 * features of the RSCS server.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_RSCS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  SupportedFeatures    Bitmask that contains the
 *                                  supported features that the RSCS
 *                                  server will support if this function
 *                                  is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_RSCS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_RSCS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_RSCS_Set_RSC_Feature(uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t SupportedFeatures);

/**
 * @brief
 * Queries the supported RSCS features
 * for the RSCS server.
 *
 * @details
 * The SupportedFeatures parameter is a bitmask with the bits between
 * QAPI_BLE_RSCS_FEATURE_FLAG_INSTANTANEOUS_STRIDE_LENGTH_MEASUREMENT_SUPPORTED
 * and QAPI_BLE_RSCS_FEATURE_FLAG_MULTIPLE_SENSOR_LOCATIONS_SUPPORTED.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_RSCS_Initialize_XXX()
 *                                 functions.
 *
 * @param[out]  SupportedFeatures    Pointer to a bitmask that will
 *                                   hold the supported features that the
 *                                   RSCS server supports if this
 *                                   function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_RSCS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_RSCS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_RSCS_Query_RSC_Feature(uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t *SupportedFeatures);

/**
 * @brief
 * Sets the RSCS Sensor Location on
 * the RSCS server.
 *
 * @details
 * The SensorLocation parameter should be in a range between
 * QAPI_BLE_RSCS_SENSOR_LOCATION_OTHER to RSCS_SENSOR_LOCATION_REAR_HUB.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_RSCS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  SensorLocation      This parameter will set the RSCS
 *                                 Sensor Location if this function is
 *                                 successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_RSCS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_RSCS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_RSCS_Set_Sensor_Location(uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t SensorLocation);

/**
 * @brief
 * Queries the RSCS Sensor Location
 * on the RSCS server.
 *
 * @details
 * The SensorLocation parameter should be in a range between
 * QAPI_BLE_RSCS_SENSOR_LOCATION_OTHER to RSCS_SENSOR_LOCATION_REAR_HUB.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_RSCS_Initialize_XXX()
 *                                 functions.
 *
 * @param[out]  SensorLocation     Pointer that will hold the RSCS
 *                                 Sensor Location if this function is
 *                                 successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_RSCS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_RSCS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_RSCS_Query_Sensor_Location(uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t *SensorLocation);

/**
 * @brief
 * Responds to a write
 * request from an RSCS client for the RSCS SC Control Point
 * command.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * QPAI_BLE_RSCS_ERROR_CODE_XXX from qapi_ble_rscstypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * This function is primarily provided to allow a way to reject SC
 * Control Point commands if the corresponding CCCD
 * has not been configured for
 * indications by the RSCS client that made the request, a procedure
 * is already in progress, or the RSCS client does not have
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
 *                 QAPI_BLE_RSCS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_RSCS_SC_Control_Point_Response(uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t ErrorCode);

/**
 * @brief
 * Sends an indication for the
 * SC Control Point request to the RSCS client. This function
 * is used to indicate whether the SC Control Point Procedure was successful or an
 * error occured.
 *
 * @details
 * This function sends an indication that provides a guarantee that the
 * RSCS client will receive the value, since the RSCS client must confirm
 * that it has been received.
 *
 * This function must not be used if the RSCS server is responding to a request
 * for the supported sensor locations and the request has been accepted.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the SC Control Point
 * Characteristic that is going to be indicated has been previously configured
 * for indications. An RSCS client must have written the RSCS SC Control Point
 * Characteristic's CCCD to enable indications.
 *
 * @note1hang
 * Only one indication may be outstanding per RSCS Instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_RSCS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the
 *                                 connection for the indication.
 *
 * @param[in]  CommandType         Enumeration for the Op Code of the
 *                                 request.
 *
 * @param[in]  ResponseCode        Response Code of the SC Control
 *                                 Point Procedure result. This indicates
 *                                 whether the procedure was successful
 *                                 or an error occured.
 *
 * @return      Positive nonzero if successful (represents the
 *              GATT Transaction ID for the indication).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_RSCS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_RSCS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_RSCS_Indicate_SC_Control_Point_Result(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_RSCS_SCCP_Command_Type_t CommandType, uint8_t ResponseCode);

/**
 * @brief
 * Sends an indication for the
 * supported sensor locations. This is in response to an SC Control Point
 * request for the RSCS Supported Sensor Locations that was successful.
 *
 * @details
 * This function sends an indication that provides a guarantee that the
 * RSCS client will receive the value, since the RSCS client must confirm
 * that it has been received.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the SC Control Point
 * Characteristic that is going to be indicated has been previously configured
 * for indications. An RSCS client must have written the RSCS SC Control Point
 * Characteristic's CCCD to enable indications.
 *
 * @note1hang
 * Only one indication may be outstanding per RSCS Instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_RSCS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the
 *                                 connection for the indication.
 *
 * @param[in]  SupportedSensorLocations    Pointer to the supported
 *                                         sensor locations that will be
 *                                         indicated to the remote RSCS
 *                                         client.
 *
 * @return      Positive nonzero if successful (represents the
 *              GATT Transaction ID for the indication).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_RSCS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_RSCS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_RSCS_Indicate_Supported_Sensor_Locations(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_RSCS_SCCP_Supported_Sensor_Locations_t *SupportedSensorLocations);

   /* RSCS client API.                                                  */

/**
 * @brief
 * Parses a value received in an
 * indication from a remote RSCS server, interpreting it as an RSCS
 * Measurement.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            RSCS server.
 *
 * @param[in]  Value          Value received from the
 *                            RSCS server.
 *
 * @param[out]  MeasurementData    Pointer that will hold the decoded
 *                                 RSCS Measurement data if this function
 *                                 is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_RSCS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_RSCS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_RSCS_Decode_RSC_Measurement(uint32_t ValueLength, uint8_t *Value, qapi_BLE_RSCS_RSC_Measurement_Data_t *MeasurementData);

/**
 * @brief
 * Formatts the RSCS SC
 * Control Point Command into a user-specified buffer for a GATT Write request
 * that will be sent to the RSCS server.
 *
 * @details
 * The third parameter, BufferLength, is the size of input request, and it
 * will also hold the size of output buffer after formatting.
 *
 * @param[in]  FormatData      Pointer to the SC Control Point Command
 *                             that will be formatted into the user-specified
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
 *                 QAPI_BLE_RSCS_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_RSCS_Format_SC_Control_Point_Command(qapi_BLE_RSCS_SC_Control_Point_Format_Data_t *FormatData, uint32_t *BufferLength, uint8_t *Buffer);

/**
 * @brief
 * Parses a value received in an
 * indication from a remote RSCS server, interpreting it as the RSCS
 * SC Control Point response.
 *
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            RSCS server.
 *
 * @param[in]  Value          Value received from the
 *                            RSCS server.
 *
 * @param[out]  SCCPResponseData    Pointer that will hold the decoded
 *                                  RSCS SC Control Point response if
 *                                  this function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_RSCS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_RSCS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_RSCS_Decode_SC_Control_Point_Response(uint32_t ValueLength, uint8_t *Value, qapi_BLE_RSCS_SC_Control_Point_Response_Data_t *SCCPResponseData);

/**
 * @}
 */

#endif

