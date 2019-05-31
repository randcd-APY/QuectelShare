/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @addtogroup qapi_ble_services
@{
*/

/**
 * @file qapi_ble_cscs.h
 *
 * @brief
 * QCA QAPI for Bluetopia Bluetooth Cycling Speed and Cadence Service
 * (GATT based) API Type Definitions, Constants, and Prototypes.
 *
 * @details
 * The Cycling Speed and Cadence Service programming interface defines
 * the protocols and procedures to be used to implement the Cycling Speed
 * and Cadence Service capabilities.
 */

#ifndef __QAPI_BLE_CSCS_H__
#define __QAPI_BLE_CSCS_H__

#include "./qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.          */
#include "./qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.    */
#include "./qapi_ble_gatt.h"      /* qapi GATT prototypes.                    */
#include "./qapi_ble_cscstypes.h" /* QAPI CSCS prototypes.                    */

   /* Error Return Codes.                                               */

   /* Error Codes that are smaller than these (less than -1000) are     */
   /* related to the Bluetooth Protocol Stack itself (see               */
   /* qapi_ble_errros.h).                                               */
#define QAPI_BLE_CSCS_ERROR_INVALID_PARAMETER                       (-1000)
/**< Invalid parameter. */
#define QAPI_BLE_CSCS_ERROR_INVALID_BLUETOOTH_STACK_ID              (-1001)
/**< Invalid Bluetooth Stack ID. */
#define QAPI_BLE_CSCS_ERROR_INSUFFICIENT_RESOURCES                  (-1002)
/**< Insufficient resources. */
#define QAPI_BLE_CSCS_ERROR_INSUFFICIENT_BUFFER_SPACE               (-1003)
/**< Insufficient buffer space. */
#define QAPI_BLE_CSCS_ERROR_SERVICE_ALREADY_REGISTERED              (-1004)
/**< Service already registered. */
#define QAPI_BLE_CSCS_ERROR_INVALID_INSTANCE_ID                     (-1005)
/**< Invalid service instance ID. */
#define QAPI_BLE_CSCS_ERROR_MALFORMATTED_DATA                       (-1006)
/**< Malformatted data. */
#define QAPI_BLE_CSCS_ERROR_INDICATION_OUTSTANDING                  (-1007)
/**< Indication outstanding. */
#define QAPI_BLE_CSCS_ERROR_NO_AUTHENTICATION                       (-1008)
/**< No authentication. */
#define QAPI_BLE_CSCS_ERROR_UNKNOWN_ERROR                           (-1009)
/**< Unknown error. */

/**
 * This structure contains the attribute handles that will need to be
 * cached by a CSCS Client in order to only do service discovery once.
 */
typedef struct qapi_BLE_CSCS_Client_Information_s
{
   /**
    * The CSC Measurement attribute handle.
    */
   uint16_t CSC_Measurement;

   /**
    * The CSC Measurement Client Characteristic
    * Configuration Descriptor (CCCD) attribute handle.
    */
   uint16_t CSC_Measurement_Client_Configuration;

   /**
    * The CSC Feature attribute handle.
    */
   uint16_t CSC_Feature;

   /**
    * The CSCS Sensor Location attribute handle.
    */
   uint16_t Sensor_Location;

   /**
    * The SC Control Point attribute handle.
    */
   uint16_t SC_Control_Point;

   /**
    * The SC Control Point Client Characteristic
    * Configuration Descriptor (CCCD) attribute handle.
    */
   uint16_t SC_Control_Point_Client_Configuration;
} qapi_BLE_CSCS_Client_Information_t;

#define QAPI_BLE_CSCS_CLIENT_INFORMATION_DATA_SIZE                (sizeof(qapi_BLE_CSCS_Client_Information_t))
/**<
 * Size of the qapi_BLE_CSCS_Client_Information_t structure.
 */

/**
 * This structure contains all of the per Client data that will need to
 * be stored by a CSCS Server.
 */
typedef struct qapi_BLE_CSCS_Server_Information_s
{
   /**
    * The CSC Measurement Client Characteristic
    * Configuration Descriptor (CCCD).
    */
   uint16_t CSC_Measurement_Client_Configuration;

   /**
    * The SC Control Point Client Characteristic
    * Configuration Descriptor (CCCD).
    */
   uint16_t SC_Control_Point_Client_Configuration;
} qapi_BLE_CSCS_Server_Information_t;

#define QAPI_BLE_CSCS_SERVER_INFORMATION_DATA_SIZE                (sizeof(qapi_BLE_CSCS_Server_Information_t))
/**<
 * Size of the qapi_BLE_CSCS_Server_Information_t structure.
 */

/**
 * This structure represents the format of the optional Wheel Revolution
 * Data field of the CSC Mesaurement characteristic.  The Cumulative
 * Wheel Revolutions field represents the number of times wheel was
 * rotated. The Last Wheel Event Time is free-running-count of 1/1024
 * second units and it represents the time when the last wheel revolution
 * was detected by the wheel rotation sensor.
 */
typedef struct qapi_BLE_CSCS_Wheel_Revolution_Data_s
{
   /**
    * The cumulative wheel revolution.
    */
   uint32_t CumulativeWheelRevolutions;

   /**
    * The last wheel event time.
    */
   uint16_t LastWheelEventTime;
} qapi_BLE_CSCS_Wheel_Revolution_Data_t;

#define QAPI_BLE_CSCS_WHEEL_REVOLUTIION_DATA_SIZE                 (sizeof(qapi_BLE_CSCS_Wheel_Revolution_Data_t))
/**<
 * Size of the qapi_BLE_CSCS_Wheel_Revolution_Data_t structure.
 */

/**
 * This structure represents the format of the optional Crank Revolution
 * Data field of the CSC Mesaurement characteristic.  The Cumulative
 * Crank Revolutions field represents the number of times crank was
 * rotated. The Last Crank Event Time is free-running-count of 1/1024
 * second units and it represents the time when the last crank revolution
 * was detected by the crank rotation sensor.
 */
typedef struct qapi_BLE_CSCS_Crank_Revolution_Data_s
{
   /**
    * The cumulative crank revolution.
    */
   uint16_t CumulativeCrankRevolutions;

   /**
    * The last crank event time.
    */
   uint16_t LastCrankEventTime;
} qapi_BLE_CSCS_Crank_Revolution_Data_t;

#define QAPI_BLE_CSCS_CRANK_REVOLUTIION_DATA_SIZE                 (sizeof(qapi_BLE_CSCS_Crank_Revolution_Data_t))
/**<
 * Size of the qapi_BLE_CSCS_Crank_Revolution_Data_t structure.
 */

/**
 * This structure represents the structure of the CSC Measurement Data
 * that is passed to the function that builds the CSC Measurement packet.
 *
 * If the
 * QAPI_BLE_CSCS_CSC_MEASUREMENT_FLAGS_WHEEL_REVOLUTION_DATA_PRESENT Flag
 * is set, then a valid value must be entered for Wheel Revolution Data.
 * If the
 * QAPI_BLE_CSCS_CSC_MEASUREMENT_FLAGS_CRANK_REVOLUTION_DATA_PRESENT Flag
 * is set, then a valid value must be entered for Crank Revolution Data.
 */
typedef struct qapi_BLE_CSCS_CSC_Measurement_Data_s
{
   /**
    * A bitmask that indicates the optional fields and features that may
    * be included for a CSC Measurement.
    *
    * Valid values have the form API_BLE_CSCS_CSC_MEASUREMENT_FLAGS_XXX
    * and can be found in qapi_ble_cscstypes.h.
    */
   uint8_t                               Flags;

   /**
    * The wheel revolution data.
    */
   qapi_BLE_CSCS_Wheel_Revolution_Data_t WheelRevolutionData;

   /**
    * The crank revolution data.
    */
   qapi_BLE_CSCS_Crank_Revolution_Data_t CrankRevolutionData;
} qapi_BLE_CSCS_CSC_Measurement_Data_t;

#define QAPI_BLE_CSCS_CSC_MEASUREMENT_DATA_SIZE                   (sizeof(qapi_BLE_CSCS_CSC_Measurement_Data_t))
/**<
 * Size of the qapi_BLE_CSCS_CSC_Measurement_Data_t structure.
 */

#define MAXIMUM_SUPPORTED_SENSOR_LOCATIONS               (17)

/**
 * This structure represents the format of a Supported Sensor Location
 * Values that will be used to respond to Supported Sensor Locations
 * request made by remote device. The first member represents the Byte
 * array of multiple Sensor Locations. The second member represents
 * Number of Sensor Locations available in the Byte array.
 */
typedef struct qapi_BLE_CSCS_SCCP_Supported_Sensor_Locations_s
{
   /**
    * The sensor locations data.
    */
   uint8_t SensorLocations[MAXIMUM_SUPPORTED_SENSOR_LOCATIONS];

   /**
    * The number of sensor locations.
    */
   uint8_t NumberOfSensorLocations;
} qapi_BLE_CSCS_SCCP_Supported_Sensor_Locations_t;

#define QAPI_BLE_CSCS_SCCP_SUPPORTED_SENSOR_LOCATIONS_SIZE        (sizeof(qapi_BLE_CSCS_SCCP_Supported_Sensor_Locations_t))
/**<
 * Size of the qapi_BLE_CSCS_SCCP_Supported_Sensor_Locations_t structure.
 */

/**
 * This structure represents the format of a SC Control Point Response
 * data. This structure will hold the SC Control Point response data
 * received from remote CSCS Server. The first member RequestOpCode must
 * be of the form QAPI_BLE_CSCS_SC_CONTROL_POINT_OPCODE_XXX And the
 * second member ResponseCodeValue must be of the form
 * QAPI_BLE_CSCS_SC_CONTROL_POINT_RESPONSE_CODE_XXX. The third member is
 * Supported Sensor Locations, this field must be filled with valid
 * values when client has made
 * QAPI_BLE_CSCS_SC_CONTROL_POINT_OPCODE_REQUEST_SUPPORTED_SENSOR_LOCATIONS
 * request via SC Control Point and when Multiple sensor locations are
 * available in CSCS Server.
 */
typedef struct qapi_BLE_CSCS_SC_Control_Point_Response_Data_s
{
   /**
    * The request op code that has the form
    * QAPI_BLE_CSCS_SC_CONTROL_POINT_OPCODE_XXX and can be found in
    * qapi_ble_cscstypes.h.
    */
   uint8_t                                         RequestOpCode;

   /**
    * The response code value that has the form
    * QAPI_BLE_CSCS_SC_CONTROL_POINT_RESPONSE_CODE_XXX and can be found
    * in qapi_ble_cscstypes.h.
    */
   uint8_t                                         ResponseCodeValue;

   /**
    * The supported sensor locations.
    */
   qapi_BLE_CSCS_SCCP_Supported_Sensor_Locations_t SupportedSensorLocations;
} qapi_BLE_CSCS_SC_Control_Point_Response_Data_t;

#define QAPI_BLE_CSCS_SC_CONTROL_POINT_RESPONSE_DATA_SIZE         (sizeof(qapi_BLE_CSCS_SC_Control_Point_Response_Data_t))
/**<
 * Size of the qapi_BLE_CSCS_SC_Control_Point_Response_Data_t structure.
 */

/**
 * This enumeration represents the valid Read/Write Client Configuration
 * Request types that a server may receive in a
 * QAPI_BLE_ET_CSCS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E or
 * QAPI_BLE_ET_CSCS_SERVER_CLIENT_CONFIGURATION_UPDATE_E event.  This
 * type is also used by Notify/Indicate APIs to denote the characteristic
 * value to notify or indicate.
 *
 * For each event it is up to the application to return (or write) the
 * correct Client Configuration descriptor based on this value.
 */
typedef enum
{
   QAPI_BLE_CT_CSC_MEASUREMENT_E,
   QAPI_BLE_CT_SC_CONTROL_POINT_E
} qapi_BLE_CSCS_Characteristic_Type_t;

/**
 * This enumeration represents all the events generated by the CSCS
 * Service. These are used to determine the type of each event generated,
 * and to ensure the proper union element is accessed for the
 * qapi_BLE_CSCS_Event_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_CSCS_ET_READ_CLIENT_CONFIGURATION_REQUEST_E,
   QAPI_BLE_CSCS_ET_CLIENT_CONFIGURATION_UPDATE_E,
   QAPI_BLE_CSCS_ET_CONTROL_POINT_COMMAND_E,
   QAPI_BLE_CSCS_ET_CONFIRMATION_DATA_E
} qapi_BLE_CSCS_Event_Type_t;

/**
 * This structure represents the format for the data that is dispatched
 * to a CSCS Server when a CSCS Client has sent a request to read a CSCS
 * Characteristic's Client Characteristic Configuration Descriptor
 * (CCCD) (Client).
 *
 * Some of this structures fields will be required when responding to a
 * request using the
 * qapi_BLE_CSCS_Read_Client_Configuration_Response() function.
 */
typedef struct qapi_BLE_CSCS_Read_Client_Configuration_Data_s
{
   /**
    * The CSCS instance that dispatched the event.
    */
   uint32_t                            InstanceID;

   /**
    * The GATT Connection ID for the connection with the CSCS Client
    * that made the request.
    */
   uint32_t                            ConnectionID;

   /**
    * The GATT Transaction ID for the request.
    */
   uint32_t                            TransactionID;

   /**
    * The GATT Connection type, which identifies the transport used for
    * the connection with the CSCS Client.
    */
   qapi_BLE_GATT_Connection_Type_t     ConnectionType;

   /**
    * The Bluetooth address of the CSCS Client that made the request.
    */
   qapi_BLE_BD_ADDR_t                  RemoteDevice;

   /**
    * Identifies the requested CCCD based on the CSCS Characteristic
    * type.
    */
   qapi_BLE_CSCS_Characteristic_Type_t ClientConfigurationType;
} qapi_BLE_CSCS_Read_Client_Configuration_Data_t;

#define QAPI_BLE_CSCS_READ_CLIENT_CONFIGURATION_DATA_SIZE         (sizeof(qapi_BLE_CSCS_Read_Client_Configuration_Data_t))
/**<
 * Size of the qapi_BLE_CSCS_Read_Client_Configuration_Data_t structure.
 */

/**
 * This structure represents the format for the data that is dispatched
 * to a CSCS Server when a CSCS Client has written the CSCS
 * Characteristic's Client Characteristic Configuration Descriptor
 * (CCCD) (Client).
 */
typedef struct qapi_BLE_CSCS_Client_Configuration_Update_Data_s
{
   /**
    * The CSCS instance that dispatched the event.
    */
   uint32_t                            InstanceID;

   /**
    * The GATT Connection ID for the connection with the CSCS Client
    * that made the request.
    */
   uint32_t                            ConnectionID;
   /**
    * The GATT Connection type, which identifies the transport used for
    * the connection with the CSCS Client.
    */
   qapi_BLE_GATT_Connection_Type_t     ConnectionType;

   /**
    * The Bluetooth address of the CSCS Client that made the request.
    */
   qapi_BLE_BD_ADDR_t                  RemoteDevice;

   /**
    * Identifies the requested CCCD based on the CSCS Characteristic
    * type.
    */
   qapi_BLE_CSCS_Characteristic_Type_t ClientConfigurationType;

   /**
    * The Client Characteristic Configuration Descriptor (CCCD) value
    * that has been written.
    */
   uint16_t                            ClientConfiguration;
} qapi_BLE_CSCS_Client_Configuration_Update_Data_t;

#define QAPI_BLE_CSCS_CLIENT_CONFIGURATION_UPDATE_DATA_SIZE       (sizeof(qapi_BLE_CSCS_Client_Configuration_Update_Data_t))
/**<
 * Size of the qapi_BLE_CSCS_Client_Configuration_Update_Data_t
 * structure.
 */

/**
 * This enumeration represents the valid values that may be set as the
 * value for the OpCode field of SC Control Point characteristic.
 */
typedef enum
{
   QAPI_BLE_CPC_SET_CUMULATIVE_VALUE_REQUEST_E       = QAPI_BLE_CSCS_SC_CONTROL_POINT_OPCODE_SET_CUMULATIVE_VALUE,
   QAPI_BLE_CPC_START_SENSOR_CALIBRATION_REQUEST_E   = QAPI_BLE_CSCS_SC_CONTROL_POINT_OPCODE_START_SENSOR_CALIBRATION,
   QAPI_BLE_CPC_UPDATE_SENSOR_LOCATION_REQUEST_E     = QAPI_BLE_CSCS_SC_CONTROL_POINT_OPCODE_UPDATE_SENSOR_LOCATION,
   QAPI_BLE_CPC_SUPPORTED_SENSOR_LOCATIONS_REQUEST_E = QAPI_BLE_CSCS_SC_CONTROL_POINT_OPCODE_REQUEST_SUPPORTED_SENSOR_LOCATIONS
} qapi_BLE_CSCS_SCCP_Command_Type_t;

/**
 * This structure represents the format of the SC Control Point Command
 * Request Data. This structure is passed as a parameter to
 * QAPI_BLE_CSCS_Format_Control_Point_Command API.
 */
typedef struct qapi_BLE_CSCS_SC_Control_Point_Format_Data_s
{
   /**
    * SC Control Point command type.
    */
   qapi_BLE_CSCS_SCCP_Command_Type_t CommandType;
   union
   {
      /**
       * The Cumulative value parameter.
       */
      uint32_t CumulativeValue;

      /**
       * The Sensor location parameter.
       */
      uint8_t  SensorLocation;
   }
   /**
    * The command parameter that depends on the value of the CommandType
    * field.
    */
   CmdParameter;
} qapi_BLE_CSCS_SC_Control_Point_Format_Data_t;

#define QAPI_BLE_CSCS_SC_CONTROL_POINT_FORMAT_DATA_SIZE           (sizeof(qapi_BLE_CSCS_SC_Control_Point_Format_Data_t))
/**<
 * Size of the qapi_BLE_CSCS_SC_Control_Point_Format_Data_t structure.
 */

/**
 * This structure is dispatched to a CSCS Server in response to the
 * reception of request from a Client to write to the CS Control Point.
 */
typedef struct qapi_BLE_CSCS_SC_Control_Point_Command_Data_s
{
   /**
    * The CSCS instance that dispatched the event.
    */
   uint32_t                                     InstanceID;

   /**
    * The GATT Connection ID for the connection with the CSCS Client
    * that made the request.
    */
   uint32_t                                     ConnectionID;

   /**
    * The GATT Transaction ID for the request.
    */
   uint32_t                                     TransactionID;

   /**
    * The GATT Connection type, which identifies the transport used for
    * the connection with the CSCS Client.
    */
   qapi_BLE_GATT_Connection_Type_t              ConnectionType;

   /**
    * The Bluetooth address of the CSCS Client that made the request.
    */
   qapi_BLE_BD_ADDR_t                           RemoteDevice;

   /**
    * The SC Control Point data that has been requested to be written.
    */
   qapi_BLE_CSCS_SC_Control_Point_Format_Data_t FormatData;
} qapi_BLE_CSCS_SC_Control_Point_Command_Data_t;

#define QAPI_BLE_CSCS_SC_CONTROL_POINT_COMMAND_DATA_SIZE          (sizeof(qapi_BLE_CSCS_SC_Control_Point_Command_Data_t))
/**<
 * Size of the qapi_BLE_CSCS_SC_Control_Point_Command_Data_t structure.
 */

/**
 * This structure represents the format for the data that is dispatched
 * to a CSCS Server when a CSCS Client has responded to an outstanding
 * indication that was previously sent by the CSCS Server.
 */
typedef struct qapi_BLE_CSCS_Confirmation_Data_s
{
   /**
    * The CSCS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * The GATT Connection ID for the connection with the CSCS Client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * The GATT Connection type, which identifies the transport used for
    * the connection with the CSCS Client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * The Bluetooth address of the CSCS Client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;

   /**
    * The status of the indication.
    */
   uint8_t                         Status;
} qapi_BLE_CSCS_Confirmation_Data_t;

#define QAPI_BLE_CSCS_CONFIRMATION_DATA_SIZE                      (sizeof(qapi_BLE_CSCS_Confirmation_Data_t))
/**<
 * Size of the qapi_BLE_CSCS_Confirmation_Data_t structure.
 */

/**
 * This structure represents the container structure for holding all the
 * event data for a CPS instance.
 */
typedef struct qapi_BLE_CSCS_Event_Data_s
{
   /**
    * The event type used to determine the appropriate union member of
    * the Event_Data field to access.
    */
   qapi_BLE_CSCS_Event_Type_t Event_Data_Type;

   /**
    * The total size of the data contained in the event.
    */
   uint16_t                   Event_Data_Size;
   union
   {
      /**
       * CSCS Read Client Characteristic Configuration Descriptor
       * (CCCD) data.
       */
      qapi_BLE_CSCS_Read_Client_Configuration_Data_t   *CSCS_Read_Client_Configuration_Data;

      /**
       * CSCS Write Client Characteristic Configuration Descriptor
       * (CCCD) data.
       */
      qapi_BLE_CSCS_Client_Configuration_Update_Data_t *CSCS_Client_Configuration_Update_Data;

      /**
       * CSCS SC Control Point data.
       */
      qapi_BLE_CSCS_SC_Control_Point_Command_Data_t    *CSCS_Control_Point_Command_Data;

      /**
       * CSCS Confirmation data.
       */
      qapi_BLE_CSCS_Confirmation_Data_t                *CSCS_Confirmation_Data;
   }
   /**
    * The event data.
    */
   Event_Data;
} qapi_BLE_CSCS_Event_Data_t;

#define QAPI_BLE_CSCS_EVENT_DATA_SIZE                             (sizeof(qapi_BLE_CSCS_Event_Data_t))
/**<
 * Size of the qapi_BLE_CSCS_Event_Data_t structure.
 */

/**
 * @brief
 * The following declared type represents the Prototype Function for an
 * CSCS Event Callback.  This function will be called whenever a define
 * CSCS Event occurs within the Bluetooth Protocol Stack that is
 * specified with the specified Bluetooth Stack ID.
 *
 * @details
 * The event information is passed to the user in an qapi_BLE_CSCS_Event_Data_t
 * structure.  This structure contains all the information about the
 * event that occurred.
 *
 * The caller is free to use the contents of the CSCS Event Data
 * ONLY in the context of this callback.  If the caller requires
 * the Data for a longer period of time, then the callback function
 * MUST copy the data into another Data Buffer.
 *
 * This function is guaranteed NOT to be invoked more than once
 * simultaneously for the specified installed callback (i.e. this
 * function DOES NOT have be reentrant).  It Needs to be noted however,
 * that if the same Callback is installed more than once, hen the
 * callbacks will be called serially.  Because of this, the processing
 * in this function should be as efficient as possible.
 *
 * It should also be noted that this function is called in the Thread
 * Context of a Thread that the User does NOT own.  Therefore, processing
 * in this function should be as efficient as possible (this argument holds
 * anyway because another CSCS Event will not be processed while this
 * function call is outstanding).
 *
 * @note1hang
 * This function MUST NOT Block and wait for events that can only be
 * satisfied by Receiving other Bluetooth Stack Events.  A Deadlock
 * WILL occur because other Callbacks might not be issued while
 * this function is currently outstanding.
 *
 * @param[in]  BluetoothStackID      Unique identifier assigned to this
 *                                   Bluetooth Protocol Stack on which the
 *                                   event occurred.
 *
 * @param[in]  CSCS_Event_Data        Pointer to a structure that contains
 *                                   information about the event that has
 *                                   occurred.
 *
 * @param[in]  CallbackParameter     User defined value that will be
 *                                   received with the CSCS Event data.
 *
 * @return None.
 */
typedef void (QAPI_BLE_BTPSAPI *qapi_BLE_CSCS_Event_Callback_t)(uint32_t BluetoothStackID, qapi_BLE_CSCS_Event_Data_t *CSCS_Event_Data, uint32_t CallbackParameter);

   /* CSCS Server API.                                                  */

/**
 * @brief
 * This function opens a CSCS Server on a specified Bluetooth Stack.
 *
 * @details
 * Only 1 CSCS Server may be open at a time, per Bluetooth Stack ID.
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
 * @param[in]  CallbackParameter    A user-defined parameter that will be
 *                                  passed back to the user in the callback
 *                                  function.
 *
 * @param[out]  ServiceID           Unique GATT Service ID of the
 *                                  registered CSCS service returned from
 *                                  qapi_BLE_GATT_Register_Service() API.
 *
 * @return      Positive, non-zero if successful. The return value will
 *              be the Service Instance ID of CSCS Server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *
 *                 QAPI_BLE_CSCS_ERROR_INSUFFICIENT_RESOURCES
 *                 QAPI_BLE_CSCS_ERROR_INVALID_PARAMETER
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_SERVICE_TABLE_FORMAT
 *                 QAPI_BLE_BT_GATT_ERROR_INSUFFICIENT_RESOURCES
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_PARAMETER
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_BLUETOOTH_STACK_ID
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CSCS_Initialize_Service(uint32_t BluetoothStackID, qapi_BLE_CSCS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

/**
 * @brief
 * This function opens a CSCS Server on a specified Bluetooth Stack with
 * the ability to control the location of the service in the GATT datacpse.
 *
 * @details
 * Only 1 CSCS Server may be open at a time, per Bluetooth Stack ID.
 *
 * All Client Requests will be dispatch to the EventCallback function
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
 * @param[in]      CallbackParameter     A user-defined parameter that
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
 *                                       Range structure, that on input
 *                                       can be used to control the
 *                                       location of the service in the
 *                                       GATT datacpse, and on output
 *                                       returns the handle range that
 *                                       the service is using in the GATT
 *                                       datacpse.
 *
 * @return      Positive, non-zero if successful. The return value will
 *              be the Service Instance ID of CSCS Server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *
 *                 QAPI_BLE_CSCS_ERROR_INSUFFICIENT_RESOURCES
 *                 QAPI_BLE_CSCS_ERROR_INVALID_PARAMETER
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_SERVICE_TABLE_FORMAT
 *                 QAPI_BLE_BT_GATT_ERROR_INSUFFICIENT_RESOURCES
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_PARAMETER
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_BLUETOOTH_STACK_ID
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CSCS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, qapi_BLE_CSCS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

/**
 * @brief
 * This function is responsible for cleaning up and freeing all resources
 * associated with a CSCS Service Instance.
 *
 * @details
 * After this function is called, no other CSCS Service function can be
 * called until after a successful call to the qapi_BLE_CSCS_Initialize_Service()
 * function is performed.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          The Service Instance ID to close.
 *                                 This is the value that was returned
 *                                 from the qapi_BLE_CSCS_Initialize_Service()
 *                                 function.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *
 *                 QAPI_BLE_CSCS_ERROR_INVALID_PARAMETER
 *                 QAPI_BLE_CSCS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CSCS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * This function is responsible for querying the number of attributes
 * that are contained in the CSCS Service that is registered with a
 * call to qapi_BLE_CSCS_Initialize_Service() or
 * qapi_BLE_CSCS_Initialize_Service_Handle_Range().
 *
 * @details
 * None.
 *
 * @return   Positive, non-zero, number of attributes that would be
 *           registered by a CSCS service instance.
 *
 * @return   Zero on failure.
 */
QAPI_BLE_DECLARATION unsigned int QAPI_BLE_BTPSAPI qapi_BLE_CSCS_Query_Number_Attributes(void);

/**
 * @brief
 * This function is provided to allow a mechanism for a CSCS Server to
 * successfully respond to a received read client configuration request.
 *
 * This function SHOULD NOT be used for a rejection of the request.
 *
 * @details
 * Possible Events:
 *
 *    QAPI_BLE_CSCS_ET_READ_CLIENT_CONFIGURATION_REQUEST_E
 *
 * @param[in]  BluetoothStackID        Unique identifier assigned to this
 *                                     Bluetooth Protocol Stack via a
 *                                     call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID              The Service Instance ID to close.
 *                                     This is the value that was returned
 *                                     from the qapi_BLE_CSCS_Initialize_Service()
 *                                     function.
 *
 * @param[in]  TransactionID           Transaction ID of the original
 *                                     read request. This value was
 *                                     received in the
 *                                     QAPI_BLE_CSCS_ET_READ_CLIENT_CONFIGURATION_REQUEST_E
 *                                     event.
 *
 * @param[in]  ClientConfiguration     Specifies Client Characteristic
 *                                     Configuration descriptor to send
 *                                     to remote device.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *
 *                 QAPI_BLE_CSCS_ERROR_INVALID_PARAMETER
 *                 QAPI_BLE_CSCS_ERROR_INVALID_INSTANCE_ID
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CSCS_Read_Client_Configuration_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t ClientConfiguration);

/**
 * @brief
 * This function is responsible for sending a CSC Measurement
 * notification to a CSCS Client.
 *
 * @details
 * None.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          The Service Instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CSCS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        The GATT Connection ID of the CSCS
 *                                 Client that will receive the
 *                                 notification.
 *
 * @param[in]  MeasurementData     The CSC Measurement data that will be
 *                                 sent in the notification.
 *
 * @return      Positive non-zero if successful (represents the
 *              length of the notification).
 *
 * @return      An error code if negative; one of the following values:
 *
 *                 QAPI_BLE_CSCS_ERROR_INVALID_PARAMETER
 *                 QAPI_BLE_CSCS_ERROR_INVALID_INSTANCE_ID
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CSCS_Notify_CSC_Measurement(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_CSCS_CSC_Measurement_Data_t *MeasurementData);

/**
 * @brief
 * This function is responsible for setting the supported CSCS Features
 * for the CSCS Server.
 *
 * @details
 * The SupportedFeatures parameter is a bitmask made up of bits of the form
 * QAPI_BLE_CSCS_CSC_FEATURE_XXX.
 *
 * This function MUST be called after the CSCS service is registered with
 * a successful call to qapi_BLE_CSCS_Initialize_XXX() in order to set the default
 * features of the CSCS server.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          The Service Instance ID to close.
 *                                 This is the value that was returned
 *                                 from the
 *                                 qapi_BLE_CSCS_Initialize_Service()
 *                                 function.
 *
 * @param[in]  SupportedFeatures    A bit mask that contains the
 *                                  supported features that the CSCS
 *                                  Server will support if this function
 *                                  is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *
 *                 QAPI_BLE_CSCS_ERROR_INVALID_PARAMETER
 *                 QAPI_BLE_CSCS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CSCS_Set_CSC_Feature(uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t SupportedFeatures);

/**
 * @brief
 * This function is responsible for querying the supported CSCS Features
 * for the CSCS Server.
 *
 * @details
 * The SupportedFeatures parameter is a bitmask made up of bits of the form
 * QAPI_BLE_CSCS_FEATURE_XXX.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          The Service Instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CSCS_Initialize_XXX()
 *                                 functions.
 *
 * @param[out]  SupportedFeatures    A pointer to a bit mask that will
 *                                   hold the supported features that the
 *                                   CSCS Server supports if this function
 *                                   is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *
 *                 QAPI_BLE_CSCS_ERROR_INVALID_PARAMETER
 *                 QAPI_BLE_CSCS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CSCS_Query_CSC_Feature(uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t *SupportedFeatures);

/**
 * @brief
 * This function is responsible for setting the CSCS Sensor Location.
 *
 * @details
 * The SensorLocation parameter should be in a range between
 * QAPI_BLE_CSCS_SENSOR_LOCATION_OTHER to
 * QAPI_BLE_CSCS_SENSOR_LOCATION_REAR_HUB
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          The Service Instance ID to close.
 *                                 This is the value that was returned
 *                                 from the
 *                                 qapi_BLE_CSCS_Initialize_Service()
 *                                 function.
 *
 * @param[in]  SensorLocation      A bit mask that contains the
 *                                 Sensor Location that will be set if this
 *                                 function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *
 *                 QAPI_BLE_CPS_ERROR_INVALID_PARAMETER
 *                 QAPI_BLE_CPS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CSCS_Set_Sensor_Location(uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t SensorLocation);

/**
 * @brief
 * This function is responsible for querying the supported CSCS Features
 * for the CSCS Server.
 *
 * @details
 * The SupportedFeatures parameter is a bitmask made up of bits of the form
 * QAPI_BLE_CSCS_LOCATION_XXX.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          The Service Instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CSCS_Initialize_XXX()
 *                                 functions.
 *
 * @param[out]  SensorLocation     A pointer to a bit mask that will hold
 *                                 the Sensor Location if this function
 *                                 is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *
 *                 QAPI_BLE_CSCS_ERROR_INVALID_PARAMETER
 *                 QAPI_BLE_CSCS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CSCS_Query_Sensor_Location(uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t *SensorLocation);

/**
 * @brief
 * The following function is responsible for responding to a write
 * request from a CSCS Client for the CSCS SC Control Point
 * Characteristic.
 *
 * @details
 * The ErrorCode parameter MUST be a valid value from
 * QPAI_BLE_CSCS_ERROR_CODE_XXX from qapi_ble_cpstypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * This function is primarily provided to allow a way to reject CSCS SC
 * Control Point Commands if the corresponding Client Characteristic
 * Configuration Descriptor (CCCD) has not been configured for
 * indications by the CSCS Client that made the request, a procedure is
 * already in progress, or the CSCS Client does not have the required
 * security such as authentication, authorization, or encryption.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  TransactionID       The GATT Transaction ID for the
 *                                 request.
 *
 * @param[in]  ErrorCode           The error code for the request, which
 *                                 indicates whether the request was
 *                                 successful or an error has occured.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *
 *                 QAPI_BLE_CSCS_ERROR_INVALID_PARAMETER
 *                 QAPI_BLE_CSCS_ERROR_INVALID_INSTANCE_ID
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CSCS_SC_Control_Point_Response(uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t ErrorCode);

/**
 * @brief
 * The following function is responsible for sending an indication for the
 * CSCS Control Point Result. This is in response to an CSCS SC Control
 * Point Command that was successful.
 *
 * @details
 * This function sends an indication, which provides a guarantee that the
 * CSCS Client will receive the value, since the CSCS Client MUST confirm
 * that it has been received.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the CSCS Control
 * Point Characteristic that is going to be indicated has been previously
 * configured for indications. A CSCS Client MUST have written the CSCS Control
 * Point's Characteristic's CCCD to enable indications.
 *
 * @note1hang
 * Only one indication may be outstanding per CSCS Instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          The Service Instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CSCS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        The GATT Connection ID of the
 *                                 connection for the indication.
 *
 * @param[in]  CommandType         An enumeration for the CSCS Control
 *                                 Point Command this indication is being
 *                                 sent for.
 *
 * @param[in]  ResponseCode        The response code for the indication,
 *                                 which represents the status of the CSCS
 *                                 Control Point Command.
 *
 * @return      Positive non-zero if successful (represents the
 *              GATT Transaction ID for the indication).
 *
 * @return      An error code if negative; one of the following values:
 *
 *                 QAPI_BLE_CSCS_ERROR_INVALID_PARAMETER
 *                 QAPI_BLE_CSCS_ERROR_INVALID_INSTANCE_ID
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CSCS_Indicate_SC_Control_Point_Result(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_CSCS_SCCP_Command_Type_t CommandType, uint8_t ResponseCode);

/**
 * @brief
 * The following function is responsible for sending an indication for the
 * CSCS Supported Sensor Locations.
 *
 * @details
 * This function sends an indication, which provides a guarantee that the
 * CSCS Client will receive the value, since the CSCS Client MUST confirm
 * that it has been received.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the CSCS
 * Sensor Location Characteristic that is going to be indicated has been
 * previously configured for indications. A CSCS Client MUST have written
 * the CSCS Sensor Location Characteristic's CCCD to enable indications.
 *
 * @note1hang
 * Only one indication may be outstanding per CSCS Instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          The Service Instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CSCS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        The GATT Connection ID of the
 *                                 connection for the indication.
 *
 * @param[in]  SupportedSensorLocations  A pointer to the supported
 *                                       sensor locations that will be
 *                                       indicated to the CSCS Client if
 *                                       this function is successful.
 *
 * @return      Positive non-zero if successful (represents the
 *              GATT Transaction ID for the indication).
 *
 * @return      An error code if negative; one of the following values:
 *
 *                 QAPI_BLE_CSCS_ERROR_INVALID_PARAMETER
 *                 QAPI_BLE_CSCS_ERROR_INVALID_INSTANCE_ID
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CSCS_Indicate_Supported_Sensor_Locations(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_CSCS_SCCP_Supported_Sensor_Locations_t *SupportedSensorLocations);

   /* CSCS Client API.                                                  */

/**
 * @brief
 * The following function is responsible for parsing a value received in an
 * notification from a remote CSCS Server interpreting it as CSCS
 * Measurement data.
 *
 * @details
 * None.
 *
 * @param[in]  ValueLength     The length of the Buffer received from the
 *                             CSCS Server.
 *
 * @param[in]  Value           The Buffer received from the
 *                             CSCS Server.
 *
 * @param[out]  MeasurementData   A pointer that will hold the decoded
 *                                CSCS Measurement data if this function
 *                                is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *
 *                 QAPI_BLE_CSCS_ERROR_INVALID_PARAMETER
 *                 QAPI_BLE_CSCS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CSCS_Decode_CSC_Measurement(uint32_t ValueLength, uint8_t *Value, qapi_BLE_CSCS_CSC_Measurement_Data_t *MeasurementData);

/**
 * @brief
 * The following function is responsible for parsing a value received in a
 * write response from a remote CSCS Server interpreting it as the CSCS
 * SC Control Point response.
 *
 * @details
 * None.
 *
 * @param[in]  ValueLength     The length of the Buffer received from the
 *                             CSCS Server.
 *
 * @param[in]  Value           The Buffer received from the
 *                             CSCS Server.
 *
 * @param[out]  SCControlPointResponse   A pointer to the decoded
 *                                       response data for the CSCS
 *                                       Control Point if this function
 *                                       is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *
 *                 QAPI_BLE_CSCS_ERROR_INVALID_PARAMETER
 *                 QAPI_BLE_CSCS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CSCS_Decode_SC_Control_Point_Response(uint32_t ValueLength, uint8_t *Value, qapi_BLE_CSCS_SC_Control_Point_Response_Data_t *SCControlPointResponse);

/**
 * @brief
 * The following function is responsible for formatting the CSCS Control Point
 * Command into a user-specified buffer, for a GATT Write request, that
 * will be sent to the CSCS Server.  This function may also be used to
 * determine the size of the buffer to hold the formatted data (see below).
 *
 * @details
 * None.
 *
 * @param[in]       FormatData         A pointer to the CSCS SC Control
 *                                     Point Command data that will be
 *                                     formatted into the user-specified
 *                                     buffer.
 *
 * @param[in, out]  BufferLength       The length of the user-specified
 *                                     buffer on input and the
 *                                     utilized/expected size of the
 *                                     formatted buffer on output.
 *
 * @param[out]      Buffer             The user-specified buffer that
 *                                     will hold the formatted data if
 *                                     this function is successful. This
 *                                     may be excluded (NULL) if the
 *                                     expected size of the input buffer
 *                                     is going to be determined.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *
 *                 QAPI_BLE_CSCS_ERROR_INVALID_PARAMETER
 *                 QAPI_BLE_CSCS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CSCS_Format_SC_Control_Point_Command(qapi_BLE_CSCS_SC_Control_Point_Format_Data_t *FormatData, uint32_t *BufferLength, uint8_t *Buffer);

#endif

/**
 *  @}
 */
