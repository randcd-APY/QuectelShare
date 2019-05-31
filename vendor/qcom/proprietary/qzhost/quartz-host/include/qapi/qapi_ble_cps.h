/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_ble_cps.h
 *
 * @brief
 * QCA QAPI for Bluetopia Bluetooth Cycling Power Service
 * (GATT based) API Type Definitions, Constants, and
 * Prototypes.
 *
 * @details
 * The Cycling Power Service (CPS) programming interface defines the
 * protocols and procedures to be used to implement the Cycling Power
 * Service capabilities.
 */

#ifndef __QAPI_BLE_CPS_H__
#define __QAPI_BLE_CPS_H__

#include "./qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.          */
#include "./qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.    */
#include "./qapi_ble_gatt.h"      /* qapi GATT prototypes.                    */
#include "./qapi_ble_cpstypes.h"  /* QAPI CPS prototypes.                     */

/** @addtogroup qapi_ble_services
@{
*/

   /* Error Return Codes.                                               */

   /* Error Codes that are smaller than these (less than -1000) are     */
   /* related to the Bluetooth Protocol Stack itself (see               */
   /* qapi_ble_errros.h).                                               */
#define QAPI_BLE_CPS_ERROR_INVALID_PARAMETER             (-1000)
/**< Invalid parameter. */
#define QAPI_BLE_CPS_ERROR_INVALID_BLUETOOTH_STACK_ID    (-1001)
/**< Invalid Bluetooth Stack ID. */
#define QAPI_BLE_CPS_ERROR_INSUFFICIENT_RESOURCES        (-1002)
/**< Insufficient resources. */
#define QAPI_BLE_CPS_ERROR_INSUFFICIENT_BUFFER_SPACE     (-1003)
/**< Insufficient buffer space. */
#define QAPI_BLE_CPS_ERROR_SERVICE_ALREADY_REGISTERED    (-1004)
/**< Service is already registered. */
#define QAPI_BLE_CPS_ERROR_INVALID_INSTANCE_ID           (-1005)
/**< Invalid service instance ID. */
#define QAPI_BLE_CPS_ERROR_MALFORMATTED_DATA             (-1006)
/**< Malformatted data. */
#define QAPI_BLE_CPS_ERROR_INDICATION_OUTSTANDING        (-1007)
/**< Indication is outstanding. */
#define QAPI_BLE_CPS_ERROR_NO_AUTHENTICATION             (-1008)
/**< No authentication. */
#define QAPI_BLE_CPS_ERROR_UNKNOWN_ERROR                 (-1009)
/**< Unknown error. */

/**
 * Stucture that contains the attribute handles that will need to be
 * cached by a CPS client in order to only do service discovery once.
 */
typedef struct qapi_BLE_CPS_Client_Information_s
{
   /**
    * CPS Measurement attribute handle.
    */
   uint16_t CP_Measurement;

   /**
    * CPS Measurement CCCD (client) attribute handle.
    */
   uint16_t CP_Measurement_Client_Configuration;

   /**
    * CPS Measurement CCCD (server) attribute handle.
    */
   uint16_t CP_Measurement_Server_Configuration;

   /**
    * CPS Feature attribute handle.
    */
   uint16_t CP_Feature;

   /**
    * CPS Sensor Location attribute handle.
    */
   uint16_t Sensor_Location;

   /**
    * CPS Vector attribute handle.
    */
   uint16_t CP_Vector;

   /**
    * CPS Vector CCCD attribute handle.
    */
   uint16_t CP_Vector_Client_Configuration;

   /**
    * CPS Control Point attribute handle.
    */
   uint16_t CP_Control_Point;

   /**
    * CPS CCCD attribute handle.
    */
   uint16_t CP_Control_Point_Client_Configuration;
} qapi_BLE_CPS_Client_Information_t;

#define QAPI_BLE_CPS_CLIENT_INFORMATION_DATA_SIZE                 (sizeof(qapi_BLE_CPS_Client_Information_t))
/**<
 * Size of the #qapi_BLE_CPS_Client_Information_t structure.
 */

/**
 * Stucture that contains all of the per-client data that will need to
 * be stored by a CPS server.
 */
typedef struct qapi_BLE_CPS_Server_Information_s
{
   /**
    * CPS Measurement CCCD (client).
    */
   uint16_t CP_Measurement_Client_Configuration;

   /**
    * CPS Measurement CCCD (server).
    */
   uint16_t CP_Measurement_Server_Configuration;

   /**
    * CPS Control Point CCCD.
    */
   uint16_t CP_Control_Point_Client_Configuration;

   /**
    * CPS Vector CCCD.
    */
   uint16_t CP_Vector_Client_Configuration;
} qapi_BLE_CPS_Server_Information_t;

#define QAPI_BLE_CPS_SERVER_INFORMATION_DATA_SIZE                 (sizeof(qapi_BLE_CPS_Server_Information_t))\
/**<
 * Size of the #qapi_BLE_CPS_Server_Information_t structure.
 */

/**
 * Stucture that represents the format of the optional CPS Wheel
 * Revolution data field of the #qapi_BLE_CPS_Measurement_Data_t
 * structure.
 */
typedef struct qapi_BLE_CPS_Wheel_Revolution_Data_s
{
   /**
    * Number of times the wheel was rotated.
    */
   uint32_t CumulativeWheelRevolutions;

   /**
    * Free running count of 1/2048 second units and it represents the
    * time when the last wheel revolution was detected by the wheel
    * rotation sensor.
    */
   uint16_t LastWheelEventTime;
} qapi_BLE_CPS_Wheel_Revolution_Data_t;

#define QAPI_BLE_CPS_WHEEL_REVOLUTIION_DATA_SIZE                  (sizeof(qapi_BLE_CPS_Wheel_Revolution_Data_t))
/**<
 * Size of the #qapi_BLE_CPS_Wheel_Revolution_Data_t structure.
 */

/**
 * Stucture that represents the format of the optional CPS Crank
 * Revolution data field of the #qapi_BLE_CPS_Measurement_Data_t and
 * #qapi_BLE_CPS_Vector_Data_t structures.
 */
typedef struct qapi_BLE_CPS_Crank_Revolution_Data_s
{
   /**
    * Number of times the crank was rotated.
    */
   uint16_t CumulativeCrankRevolutions;

   /**
    * Free running count of 1/2048 second units and it represents the
    * time when the last crank revolution was detected by the crank
    * rotation sensor.
    */
   uint16_t LastCrankEventTime;
} qapi_BLE_CPS_Crank_Revolution_Data_t;

#define QAPI_BLE_CPS_CRANK_REVOLUTIION_DATA_SIZE                  (sizeof(qapi_BLE_CPS_Crank_Revolution_Data_t))
/**<
 * Size of the #qapi_BLE_CPS_Crank_Revolution_Data_t structure.
 */

/**
 * Stucture that represents the format of the optional CPS Extreme Force
 * Magnitudes data field of the #qapi_BLE_CPS_Measurement_Data_t
 * structure.
 */
typedef struct qapi_BLE_CPS_Extreme_Force_Magnitudes_Data_s
{
   /**
    * Maximum force value measured in a single crank revolution.
    */
   int16_t MaximumForceMagnitude;

   /**
    * Minimum force value measured in a single crank revolution.
    */
   int16_t MinimumForceMagnitude;
} qapi_BLE_CPS_Extreme_Force_Magnitudes_Data_t;

#define QAPI_BLE_CPS_EXTREME_FORCE_MAGNITUDES_DATA_SIZE           (sizeof(qapi_BLE_CPS_Extreme_Force_Magnitudes_Data_t))
/**<
 * Size of the #qapi_BLE_CPS_Extreme_Force_Magnitudes_Data_t structure.
 */

/**
 * Stucture that represents the format of the optional CPS Torque
 * Magnitudes data field of the #qapi_BLE_CPS_Measurement_Data_t
 * structure.
 */
typedef struct qapi_BLE_CPS_Extreme_Torque_Magnitudes_Data_s
{
   /**
    * Maximum torque value measured in a single crank revolution.
    */
   int16_t MaximumTorqueMagnitude;

   /**
    * Minimum torque value measured in a single crank revolution.
    */
   int16_t MinimumTorqueMagnitude;
} qapi_BLE_CPS_Extreme_Torque_Magnitudes_Data_t;

#define QAPI_BLE_CPS_EXTREME_TORQUE_MAGNITUDES_DATA_SIZE          (sizeof(qapi_BLE_CPS_Extreme_Torque_Magnitudes_Data_t))
/**<
 * Size of the #qapi_BLE_CPS_Extreme_Torque_Magnitudes_Data_t structure.
 */

/**
 * Stucture that represents the format of the optional CPS Extreme
 * Angles data field of the #qapi_BLE_CPS_Measurement_Data_t structure.
 *
 * The actual size of Minimum Angle and Maximum Angle data fields are
 * 12bits, so the Most Significant 4 bits of these fields will be unused and
 * should be set to zero.
 */
typedef struct qapi_BLE_CPS_Extreme_Angles_Data_s
{
   /**
    * Maximum angle value measured a single crank revolution.
    */
   uint16_t MaximumAngle;

   /**
    * Minimum angle value measured a the single crank revolution.
    */
   uint16_t MinimumAngle;
} qapi_BLE_CPS_Extreme_Angles_Data_t;

#define QAPI_BLE_CPS_EXTREMA_ANGLES_DATA_SIZE                     (sizeof(qapi_BLE_CPS_Extreme_Angles_Data_t))
/**<
 * Size of the #qapi_BLE_CPS_Extreme_Angles_Data_t structure.
 */

/**
 * Stucture that represents the CPS Measurement data.
 */
typedef struct qapi_BLE_CPS_Measurement_Data_s
{
   /**
    * Bitmask that indicates the optional fields and features that may
    * be included for a CPS Measurement.
    *
    * Valid values have the form QAPI_BLE_CPS_MEASUREMENT_FLAGSS_XXX and
    * can be found in qapi_ble_cpstypes.h.
    */
   uint16_t                                      Flags;

   /**
    * Instantaneous Power.
    */
   int16_t                                       InstantaneousPower;

   /**
    * Pedal power balance.
    */
   uint8_t                                       PedalPowerBalance;

   /**
    * Accumulated torque.
    */
   uint16_t                                      AccumulatedTorque;

   /**
    * Wheel revolution data.
    */
   qapi_BLE_CPS_Wheel_Revolution_Data_t          WheelRevolutionData;

   /**
    * Crank revolution data.
    */
   qapi_BLE_CPS_Crank_Revolution_Data_t          CrankRevolutionData;

   /**
    * Extreme force magnitudes data.
    */
   qapi_BLE_CPS_Extreme_Force_Magnitudes_Data_t  ExtremeForceMagnitudes;

   /**
    * Extreme torque magnitudes data.
    */
   qapi_BLE_CPS_Extreme_Torque_Magnitudes_Data_t ExtremeTorqueMagnitudes;

   /**
    * Extreme angles data.
    */
   qapi_BLE_CPS_Extreme_Angles_Data_t            ExtremeAngles;

   /**
    * Top dead spot angle.
    */
   uint16_t                                      TopDeadSpotAngle;

   /**
    * Bottom dead spot angle.
    */
   uint16_t                                      BottomDeadSpotAngle;

   /**
    * Accumulated energy.
    */
   uint16_t                                      AccumulatedEnergy;
} qapi_BLE_CPS_Measurement_Data_t;

#define QAPI_BLE_CPS_MEASUREMENT_DATA_SIZE               (sizeof(qapi_BLE_CPS_Measurement_Data_t))
/**<
 * Size of the #qapi_BLE_CPS_Measurement_Data_t structure.
 */

/**
 * Stucture that represents the CPS Cycling Power Vector data.
 *
 * The Instantaneous Force Magnitude array and the Instantaneous Torque
 * Magnitude array are mutually exclusive. Only one can be supported at
 * a time. The supported array should correspond with the Sensor
 * Measurement Context flag in the Cycling Power Feature value.
 */
typedef struct qapi_BLE_CPS_Vector_Data_s
{
   /**
    * Bitmask that indicates the optional fields and features that may
    * be included for the CPS Cyclying Power Vector data.
    *
    * Valid values have the form QAPI_BLE_CPS_VECTOR_FLAGS_XXX and can
    * be found in qapi_ble_cpsstypes.h.
    */
   uint8_t                               Flags;

   /**
    * Crank revolution data.
    */
   qapi_BLE_CPS_Crank_Revolution_Data_t  CrankRevolutionData;

   /**
    * First crank measurement angle.
    */
   uint16_t                              FirstCrankMeasurementAngle;

   /**
    * Magnitude data length.
    */
   uint8_t                               MagnitudeDataLength;

   /**
    * Pointer to the instantaneous magnitude.
    */
   int16_t                              *InstantaneousMagnitude;
} qapi_BLE_CPS_Vector_Data_t;

#define QAPI_BLE_CPS_VECTOR_DATA_SIZE                    (sizeof(qapi_BLE_CPS_Vector_Data_t))
/**<
 * Size of the #qapi_BLE_CPS_Vector_Data_t structure.
 */

#define QAPI_BLE_CPS_MAXIMUM_SUPPORTED_SENSOR_LOCATIONS  (17)
/**<
 * Maximum number of supported sensory locations.
 */

/**
 * Stucture that represents the format of the CPS Supported Sensor
 * Location values.
 */
typedef struct qapi_BLE_CPS_Supported_Sensor_Locations_s
{
   /**
    * Number of sensor locations.
    */
   uint8_t  NumberOfSensorLocations;

   /**
    * Pointer to the sensor locations.
    */
   uint8_t *SensorLocations;
} qapi_BLE_CPS_Supported_Sensor_Locations_t;

#define QAPI_BLE_CPS_SUPPORTED_SENSOR_LOCATIONS_SIZE     (sizeof(qapi_BLE_CPS_Supported_Sensor_Locations_t))
/**<
 * Size of the #qapi_BLE_CPS_Supported_Sensor_Locations_t structure.
 */

/**
 * Stucture that represents the Date/Time data for CPS.
 */
typedef struct qapi_BLE_CPS_Date_Time_Data_s
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
} qapi_BLE_CPS_Date_Time_Data_t;

#define QAPI_BLE_CPS_DATE_TIME_DATA_SIZE                 (sizeof(qapi_BLE_CPS_Date_Time_Data_t))
/**<
 * Size of the #qapi_BLE_CPS_Date_Time_Data_t structure.
 */

#define QAPI_BLE_CPS_DATE_TIME_VALID(_x)                 ((QAPI_BLE_GATT_DATE_TIME_VALID_YEAR(((_x)).Year)) && (QAPI_BLE_GATT_DATE_TIME_VALID_MONTH(((_x)).Month)) && (QAPI_BLE_GATT_DATE_TIME_VALID_DAY(((_x)).Day)) && (QAPI_BLE_GATT_DATE_TIME_VALID_HOURS(((_x)).Hours)) && (QAPI_BLE_GATT_DATE_TIME_VALID_MINUTES(((_x)).Minutes)) && (QAPI_BLE_GATT_DATE_TIME_VALID_SECONDS(((_x)).Seconds)))
/**<
 * This macro is a utility macro that exists to valid that a specified
 * Date Time is valid. The only parameter to this function is the
 * qapi_BLE_CPS_Date_Time_Data_t structure to valid. This macro returns
 * TRUE if the Date Time is valid or FALSE otherwise..
 */

/**
 * Enumeration of the valid values that may be set as the
 * value for the OpCode field of Cycling Power Control Point
 * characteristic.
 */
typedef enum
{
   QAPI_BLE_CPC_SET_CUMULATIVE_VALUE_E               = QAPI_BLE_CPS_CONTROL_POINT_OPCODE_SET_CUMULATIVE_VALUE,
   /**< Set cumulative value op code. */
   QAPI_BLE_CPC_UPDATE_SENSOR_LOCATION_E             = QAPI_BLE_CPS_CONTROL_POINT_OPCODE_UPDATE_SENSOR_LOCATION,
   /**< Update sensor location op code. */
   QAPI_BLE_CPC_REQUEST_SUPPORTED_SENSOR_LOCATIONS_E = QAPI_BLE_CPS_CONTROL_POINT_OPCODE_REQUEST_SUPPORTED_SENSOR_LOCATIONS,
   /**< Request supported sensor locations op code. */
   QAPI_BLE_CPC_SET_CRANK_LENGTH_E                   = QAPI_BLE_CPS_CONTROL_POINT_OPCODE_SET_CRANK_LENGTH,
   /**< Set crank length op code. */
   QAPI_BLE_CPC_REQUEST_CRANK_LENGTH_E               = QAPI_BLE_CPS_CONTROL_POINT_OPCODE_REQUEST_CRANK_LENGTH,
   /**< Request crank length op code. */
   QAPI_BLE_CPC_SET_CHAIN_LENGTH_E                   = QAPI_BLE_CPS_CONTROL_POINT_OPCODE_SET_CHAIN_LENGTH,
   /**< Set chain length op code. */
   QAPI_BLE_CPC_REQUEST_CHAIN_LENGTH_E               = QAPI_BLE_CPS_CONTROL_POINT_OPCODE_REQUEST_CHAIN_LENGTH,
   /**< Request chain length op code. */
   QAPI_BLE_CPC_SET_CHAIN_WEIGHT_E                   = QAPI_BLE_CPS_CONTROL_POINT_OPCODE_SET_CHAIN_WEIGHT,
   /**< Set chain weight op code. */
   QAPI_BLE_CPC_REQUEST_CHAIN_WEIGHT_E               = QAPI_BLE_CPS_CONTROL_POINT_OPCODE_REQUEST_CHAIN_WEIGHT,
   /**< Request chain weigth. */
   QAPI_BLE_CPC_SET_SPAN_LENGTH_E                    = QAPI_BLE_CPS_CONTROL_POINT_OPCODE_SET_SPAN_LENGTH,
   /**< Set span length op code. */
   QAPI_BLE_CPC_REQUEST_SPAN_LENGTH_E                = QAPI_BLE_CPS_CONTROL_POINT_OPCODE_REQUEST_SPAN_LENGTH,
   /**< Request span length op code. */
   QAPI_BLE_CPC_START_OFFSET_COMPENSATION_E          = QAPI_BLE_CPS_CONTROL_POINT_OPCODE_START_OFFSET_COMPENSATION,
   /**< Start offset compensation op code. */
   QAPI_BLE_CPC_MASK_CYCLING_POWER_MEASUREMENT_E     = QAPI_BLE_CPS_CONTROL_POINT_OPCODE_MASK_MEASUREMENT_CHARACTERISTIC_CONTENT,
   /**< Mask cycling power measurement op code. */
   QAPI_BLE_CPC_REQUEST_SAMPLING_RATE_E              = QAPI_BLE_CPS_CONTROL_POINT_OPCODE_REQUEST_SAMPLING_RATE,
   /**< Request sampling rate op code. */
   QAPI_BLE_CPC_REQUEST_FACTORY_CALIBRATION_DATE_E   = QAPI_BLE_CPS_CONTROL_POINT_OPCODE_REQUEST_FACTORY_CALIBRATION_DATE
   /**< Request factory calibration date. */
} qapi_BLE_CPS_Control_Point_Command_Type_t;

/**
 * Union that represents the format of the possible response parameter
 * values that will be used to respond to the Cycling Power Control Point
 * request made by the remote device.
 */
typedef union qapi_BLE_CPS_Control_Point_Response_Parameter_s
{
   /**
    * Supported sensor locations.
    */
   qapi_BLE_CPS_Supported_Sensor_Locations_t SupportedSensorLocations;

   /**
    * Factory calibration data.
    */
   qapi_BLE_CPS_Date_Time_Data_t             FactoryCalibrationDate;

   /**
    * Crank length.
    */
   uint16_t                                  CrankLength;

   /**
    * Chain length.
    */
   uint16_t                                  ChainLength;

   /**
    * Chain weight.
    */
   uint16_t                                  ChainWeight;

   /**
    * Span length.
    */
   uint16_t                                  SpanLength;

   /**
    * Offset compensation.
    */
   int16_t                                   OffsetCompensation;

   /**
    * Sampling rate.
    */
   uint8_t                                   SamplingRate;
} qapi_BLE_CPS_Control_Point_Response_Parameter_t;

/**
 * Stucture that represents the format of possible response parameter
 * values that will be used to respond to the CPS Control Point request
 * using the qapi_BLE_CPS_Indicate_Control_Point_Result_With_Data()
 * function.
 */
typedef struct qapi_BLE_CPS_Control_Point_Indication_Data_s
{
   /**
    * Request type.
    */
   uint8_t                                         CommandType;

   /**
    * Union for the response parameter. The union member depends on the
    * value of the CommandType field.
    */
   qapi_BLE_CPS_Control_Point_Response_Parameter_t ResponseParameter;
}qapi_BLE_CPS_Control_Point_Indication_Data_t;

#define QAPI_BLE_CPS_CONTROL_POINT_INDICATION_DATA_SIZE  (sizeof(qapi_BLE_CPS_Control_Point_Indication_Data_t))
/**<
 * Size of the #qapi_BLE_CPS_Control_Point_Indication_Data_t structure.
 */

/**
 * Stucture that represents the format of the CPS Control Point response
 * data.
 */
typedef struct qapi_BLE_CPS_Control_Point_Response_Data_s
{
   /**
    * Request op code that has the form
    * QAPI_BLE_CPS_CONTROL_POINT_OPCODE_XXX and can be found in
    * qapi_ble_cpstypes.h.
    */
   uint8_t                                         RequestOpCode;

   /**
    * Response code value that has the form
    * QAPI_BLE_CPS_CONTROL_POINT_RESPONSE_CODE_XXX and can be found in
    * qapi_ble_cpstypes.h.
    */
   uint8_t                                         ResponseCodeValue;

   /**
    * Union for the response data that must be set based on the
    * RequestOpCode field.
    */
   qapi_BLE_CPS_Control_Point_Response_Parameter_t ResponseParameter;
} qapi_BLE_CPS_Control_Point_Response_Data_t;

#define QAPI_BLE_CPS_CONTROL_POINT_RESPONSE_DATA_SIZE    (sizeof(qapi_BLE_CPS_Control_Point_Response_Data_t))
/**<
 * Size of the #qapi_BLE_CPS_Control_Point_Response_Data_t structure.
 */

/**
 * Enumeration of the valid Read/Write Client Configuration
 * Request types that a server may receive in a
 * QAPI_BLE_ET_CPS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E or
 * QAPI_BLE_ET_CPS_SERVER_CLIENT_CONFIGURATION_UPDATE_E event. This type
 * is also used by Notify/Indicate APIs to denote the characteristic
 * value to notify or indicate.
 *
 * For each event, it is up to the application to return (or write) the
 * correct Client Configuration descriptor based on this value.
 */
typedef enum
{
   QAPI_BLE_CT_CYCLING_POWER_MEASUREMENT_E,
   /**< Cycling Power Measurement. */
   QAPI_BLE_CT_CYCLING_POWER_CONTROL_POINT_E,
   /**< Cycling Power Control Point. */
   QAPI_BLE_CT_CYCLING_POWER_VECTOR_E
   /**< Cycling Power Vector. */
} qapi_BLE_CPS_Characteristic_Type_t;

/**
 * Enumeration of all the events generated by the CPS
 * Service. These are used to determine the type of each event
 * generated, and to ensure the proper union element is accessed for the
 * #qapi_BLE_CPS_Event_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_ET_CPS_READ_CLIENT_CONFIGURATION_REQUEST_E,
   /**< Read CCCD request event. */
   QAPI_BLE_ET_CPS_CLIENT_CONFIGURATION_UPDATE_E,
   /**< Write CCCD request event. */
   QAPI_BLE_ET_CPS_READ_CP_MEASUREMENT_SERVER_CONFIGURATION_REQUEST_E,
   /**< Read CP Measurement Server Configuration request event. */
   QAPI_BLE_ET_CPS_CP_MEASUREMENT_SERVER_CONFIGURATION_UPDATE_E,
   /**< Write CP Measurement Server Configuration request event. */
   QAPI_BLE_ET_CPS_CONTROL_POINT_COMMAND_E,
   /**< Write Control Point request event. */
   QAPI_BLE_ET_CPS_CONFIRMATION_DATA_E
   /**< Confirmation event. */
} qapi_BLE_CPS_Event_Type_t;

/**
 * Stucture that represents the format for the data that is dispatched
 * to a CPS server when a CPS client has sent a request to read a CPS
 * CCCD (client).
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_CPS_Read_Client_Configuration_Response() function.
 */
typedef struct qapi_BLE_CPS_Read_Client_Configuration_Data_s
{
   /**
    * CPS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the CPS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the CPS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * Bluetooth address of the CPS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the requested CCCD based on the CPS Characteristic
    * type.
    */
   qapi_BLE_CPS_Characteristic_Type_t ClientConfigurationType;
} qapi_BLE_CPS_Read_Client_Configuration_Data_t;

#define QAPI_BLE_CPS_READ_CLIENT_CONFIGURATION_DATA_SIZE  (sizeof(qapi_BLE_CPS_Read_Client_Configuration_Data_t))
/**<
 * Size of the #qapi_BLE_CPS_Read_Client_Configuration_Data_t structure.
 */

/**
 * Stucture that represents the format for the data that is dispatched
 * to a CPS server when a CPS client has sent a request to write a CPS
 * CCCD (client).
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_CPS_Vector_Client_Configuration_Update_Response() function.
 */
typedef struct qapi_BLE_CPS_Client_Configuration_Update_Data_s
{
   /**
    * CPS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the CPS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the CPS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * Bluetooth address of the CPS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the requested CCCD based on the CPS Characteristic
    * type.
    */
   qapi_BLE_CPS_Characteristic_Type_t ClientConfigurationType;

   /**
    * CCCD value that has been requested to be written.
    */
   uint16_t                           ClientConfiguration;
} qapi_BLE_CPS_Client_Configuration_Update_Data_t;

#define QAPI_BLE_CPS_CLIENT_CONFIGURATION_UPDATE_DATA_SIZE  (sizeof(qapi_BLE_CPS_Client_Configuration_Update_Data_t))
/**<
 * Size of the #qapi_BLE_CPS_Client_Configuration_Update_Data_t structure.
 */

/**
 * Stucture that represents the format for the data that is dispatched
 * to a CPS server when a CPS client has sent a request to read a CPS
 * characteristic's CCCD (server).
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Response()
 * function.
 */
typedef struct qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Data_s
{
   /**
    * CPS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the CPS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the CPS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * Bluetooth address of the CPS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;
} qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Data_t;

#define QAPI_BLE_CPS_READ_CP_MEASUREMENT_SERVER_CONFIGURATION_DATA_SIZE  (sizeof(qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Data_t))
/**<
 * Size of the
 * #qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Data_t
 * structure.
 */

/**
 * Stucture that represents the format for the data that is dispatched
 * to a CPS server when a CPS client has sent a request to write a CPS
 * characteristic's CCCD (server).
 */
typedef struct qapi_BLE_CPS_CP_Measurement_Server_Configuration_Update_Data_s
{
   /**
    * CPS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the CPS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the CPS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * Bluetooth address of the CPS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;

   /**
    * CCCD value that has been requested to be written.
    */
   uint16_t                        ServerConfiguration;
} qapi_BLE_CPS_CP_Measurement_Server_Configuration_Update_Data_t;

#define QAPI_BLE_CPS_CP_MEASUREMENT_SERVER_CONFIGURATION_UPDATE_DATA_SIZE  (sizeof(qapi_BLE_CPS_CP_Measurement_Server_Configuration_Update_Data_t))
/**<
 * Size of the
 * #qapi_BLE_CPS_CP_Measurement_Server_Configuration_Update_Data_t
 * structure.
 */

/**
 * Stucture that represents the format of CPS Control Point request
 * data.
 */
typedef struct qapi_BLE_CPS_Control_Point_Format_Data_s
{
   /**
    * Request type.
    */
   qapi_BLE_CPS_Control_Point_Command_Type_t CommandType;
   union
   {
      /**
       * Cumulative value.
       */
      uint32_t CumulativeValue;

      /**
       * Sensor location.
       */
      uint8_t  SensorLocation;

      /**
       * Crank length.
       */
      uint16_t CrankLength;

      /**
       * Chain length.
       */
      uint16_t ChainLength;

      /**
       * Chain weight.
       */
      uint16_t ChainWeight;

      /**
       * Span length.
       */
      uint16_t SpanLength;

      /**
       * Content mask.
       */
      uint16_t ContentMask;
   }
   /**
    * Request data.
    */
   CommandParameter;
} qapi_BLE_CPS_Control_Point_Format_Data_t;

#define QAPI_BLE_CPS_CONTROL_POINT_FORMAT_DATA_SIZE      (sizeof(qapi_BLE_CPS_Control_Point_Format_Data_t))
/**<
 * Size of the #qapi_BLE_CPS_Control_Point_Format_Data_t structure.
 */

/**
 * Stucture that represents the format for the data that is dispatched
 * to a CPS server when a CPS client has sent a request to write the CPS
 * Control Point.
 *
 * Some of the structure fields will be required when responding to a
 * request using the qapi_BLE_CPS_Control_Point_Response() function.
 */
typedef struct qapi_BLE_CPS_Control_Point_Command_Data_s
{
   /**
    * CPS instance that dispatched the event.
    */
   uint32_t                                 InstanceID;

   /**
    * GATT connection ID for the connection with the CPS client
    * that made the request.
    */
   uint32_t                                 ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                                 TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the CPS client.
    */
   qapi_BLE_GATT_Connection_Type_t          ConnectionType;

   /**
    * Bluetooth address of the CPS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                       RemoteDevice;

   /**
    * CPS Control Point request data.
    */
   qapi_BLE_CPS_Control_Point_Format_Data_t FormatData;
} qapi_BLE_CPS_Control_Point_Command_Data_t;

#define QAPI_BLE_CPS_CONTROL_POINT_COMMAND_DATA_SIZE     (sizeof(qapi_BLE_CPS_Control_Point_Command_Data_t))
/**<
 * Size of the #qapi_BLE_CPS_Control_Point_Command_Data_t structure.
 */

/**
 * Stucture that represents the format for the data that is dispatched
 * to a CPS server when a CPS client has responded to an outstanding
 * indication that was previously sent by the CPS server.
 */
typedef struct qapi_BLE_CPS_Confirmation_Data_s
{
   /**
    * CPS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the CPS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the CPS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * Bluetooth address of the CPS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;

   /**
    * Status of the indication.
    */
   uint8_t                         Status;
} qapi_BLE_CPS_Confirmation_Data_t;

#define QAPI_BLE_CPS_CONFIRMATION_DATA_SIZE              (sizeof(qapi_BLE_CPS_Confirmation_Data_t))
/**<
 * Size of the #qapi_BLE_CPS_Confirmation_Data_t structure.
 */

/**
 * Stucture that represents the container structure for holding all the
 * event data for a CPS instance.
 */
typedef struct qapi_BLE_CPS_Event_Data_s
{
   /**
    * Event type used to determine the appropriate union member of
    * the Event_Data field to access.
    */
   qapi_BLE_CPS_Event_Type_t Event_Data_Type;

   /**
    * Total size of the data contained in the event.
    */
   uint16_t                  Event_Data_Size;
   union
   {
      /**
       * CPS Read CCCD data.
       */
      qapi_BLE_CPS_Read_Client_Configuration_Data_t                  *CPS_Read_Client_Configuration_Data;

      /**
       * CPS Write CCCD data.
       */
      qapi_BLE_CPS_Client_Configuration_Update_Data_t                *CPS_Client_Configuration_Update_Data;

      /**
       * CPS Read CPS Measurement Server CCCD data.
       */
      qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Data_t   *CPS_Read_CP_Measurement_Server_Configuration_Data;

      /**
       * CPS Write CPS Measurement Server CCCD data.
       */
      qapi_BLE_CPS_CP_Measurement_Server_Configuration_Update_Data_t *CPS_CP_Measurement_Server_Configuration_Update_Data;

      /**
       * CPS Write Control Point data.
       */
      qapi_BLE_CPS_Control_Point_Command_Data_t                      *CPS_Control_Point_Command_Data;

      /**
       * CPS Confirmation data.
       */
      qapi_BLE_CPS_Confirmation_Data_t                               *CPS_Confirmation_Data;
   }
   /**
    * Event data.
    */
   Event_Data;
} qapi_BLE_CPS_Event_Data_t;

#define QAPI_BLE_CPS_EVENT_DATA_SIZE                     (sizeof(qapi_BLE_CPS_Event_Data_t))
/**<
 * Size of the #qapi_BLE_CPS_Event_Data_t structure.
 */

/**
 * @brief
 * This declared type represents the Prototype Function for an
 * CPS Event Callback. This function will be called whenever a define
 * CPS Event occurs within the Bluetooth Protocol Stack that is
 * specified with the specified Bluetooth Stack ID.
 *
 * @details
 * The event information is passed to the user in an #qapi_BLE_CPS_Event_Data_t
 * structure. This structure contains all the information about the
 * event that occurred.
 *
 * The caller should use the contents of the CPS Event Data
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
 * anyway because another CPS Event will not be processed while this
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
 * @param[in]  CPS_Event_Data        Pointer to a structure that contains
 *                                   information about the event that has
 *                                   occurred.
 *
 * @param[in]  CallbackParameter     User-defined value that will be
 *                                   received with the CPS Event data.
 *
 * @return None.
 */
typedef void (QAPI_BLE_BTPSAPI *qapi_BLE_CPS_Event_Callback_t)(uint32_t BluetoothStackID, qapi_BLE_CPS_Event_Data_t *CPS_Event_Data, uint32_t CallbackParameter);

   /* CPS Server API.                                                   */

/**
 * @brief
 * Opens a CPS server on a specified Bluetooth Stack.
 *
 * @details
 * Only one CPS server may be open at a time, per the Bluetooth Stack ID.
 *
 * All client Requests will be dispatch to the EventCallback function
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
 *                                  registered CPS service returned from
 *                                  qapi_BLE_GATT_Register_Service() API.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of CPS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CPS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_CPS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BT_GATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CPS_Initialize_Service(uint32_t BluetoothStackID, qapi_BLE_CPS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

/**
 * @brief
 * Opens a CPS server on a specified Bluetooth Stack with
 * the ability to control the location of the service in the GATT database.
 *
 * @details
 * Only one CPS server may be open at a time, per the Bluetooth Stack ID.
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
 *                                       Range structure, that, on input
 *                                       can be used to control the
 *                                       location of the service in the
 *                                       GATT database, and on output,
 *                                       returns the handle range that
 *                                       the service is using in the GATT
 *                                       database.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of CPS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CPS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_CPS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BT_GATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CPS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, qapi_BLE_CPS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

/**
 * @brief
 * Cleans up and frees all resources
 * associated with a CPS Service Instance.
 *
 * @details
 * After this function is called, no other CPS Service function can be
 * called until after a successful call to the qapi_BLE_CPS_Initialize_Service()
 * function is performed.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from the qapi_BLE_CPS_Initialize_Service()
 *                                 function.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CPS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CPS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CPS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * Queries the number of attributes
 * that are contained in the CPS Service that is registered with a
 * call to qapi_BLE_CPS_Initialize_Service() or
 * qapi_BLE_CPS_Initialize_Service_Handle_Range().
 *
 * @return   Positive, nonzero, number of attributes that would be
 *           registered by a CPS service instance. \n
 *           Zero on failure.
 */
QAPI_BLE_DECLARATION unsigned int QAPI_BLE_BTPSAPI qapi_BLE_CPS_Query_Number_Attributes(void);

/**
 * @brief
 * Allows a mechanism for a CPS server to
 * successfully respond to a received read client configuration request.
 *
 * This function should not be used for a rejection of the request.
 *
 * @details
 * Possible events: \n
 *    QAPI_BLE_ET_CPS_READ_CLIENT_CONFIGURATION_REQUEST_E
 *
 * @param[in]  BluetoothStackID        Unique identifier assigned to this
 *                                     Bluetooth Protocol Stack via a
 *                                     call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID              Service instance ID to close.
 *                                     This is the value that was returned
 *                                     from the qapi_BLE_CPS_Initialize_Service()
 *                                     function.
 *
 * @param[in]  TransactionID           Transaction ID of the original
 *                                     read request. This value was
 *                                     received in the
 *                                     QAPI_BLE_ET_CPS_READ_CLIENT_CONFIGURATION_REQUEST_E
 *                                     event.
 *
 * @param[in]  ClientConfiguration     Specifies the CCCD to send
 *                                     to the remote device.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CPS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CPS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CPS_Read_Client_Configuration_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t ClientConfiguration);

/**
 * @brief
 * Allows a mechanism for a CPS server to
 * respond to a received write client configuration request from the CPS client for
 * the CPS Vector Client Configuration.
 *
 * @details
 * Possible events: \n
 *    QAPI_BLE_ET_CPS_CLIENT_CONFIGURATION_UPDATE_E
 *
 * This function is primarily provided to allow a way to reject
 * Cycling Power Vector Client Configuration commands when the CPS client
 * update request is received by CPS server. The only valid Error Code
 * for this condition is
 * QAPI_BLE_CPS_ERROR_CODE_INAPPROPRIATE_CONNECTION_PARAMETERS. Otherwise
 * QAPI_BLE_CPS_ERROR_CODE_SUCCESS should be used.
 *
 * @param[in]  BluetoothStackID        Unique identifier assigned to this
 *                                     Bluetooth Protocol Stack via a
 *                                     call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  TransactionID           Transaction ID of the original
 *                                     read request. This value was
 *                                     received in the
 *                                     QAPI_BLE_ET_CPS_CLIENT_CONFIGURATION_UPDATE_E
 *                                     event.
 *
 * @param[in]  ErrorCode               Specifies whether the request has
 *                                     been accepted or rejected.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CPS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CPS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CPS_Vector_Client_Configuration_Update_Response(uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t ErrorCode);

/**
 * @brief
 * Allows a mechanism for a CPS server to
 * successfully respond to a received read CPS server Configuration request.
 *
 * This function should not be used for a rejection of the request.
 *
 * @details
 * Possible events: \n
 *    QAPI_BLE_ET_CPS_READ_CP_MEASUREMENT_SERVER_CONFIGURATION_REQUEST_E
 *
 * @param[in]  BluetoothStackID        Unique identifier assigned to this
 *                                     Bluetooth Protocol Stack via a
 *                                     call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID              Service instance ID to close.
 *                                     This is the value that was returned
 *                                     from the qapi_BLE_CPS_Initialize_Service()
 *                                     function.
 *
 * @param[in]  TransactionID           Transaction ID of the original
 *                                     read request. This value was
 *                                     received in the
 *                                     QAPI_BLE_ET_CPS_READ_CP_MEASUREMENT_SERVER_CONFIGURATION_REQUEST_E
 *                                     event.
 *
 * @param[in]  ServerConfiguration     Specifies the CCCD to send
 *                                     to the remote device.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CPS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CPS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CPS_Read_CP_Measurement_Server_Configuration_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t ServerConfiguration);

/**
 * @brief
 * Sets the supported CPS Features
 * for the CPS server.
 *
 * @details
 * The SupportedFeatures parameter is a bitmask made up of bits of the form
 * QAPI_BLE_CPS_FEATURE_XXX.
 *
 * This function must be called after the CPS service is registered with
 * a successful call to qapi_BLE_CPS_Initialize_XXX() in order to set the default
 * features of the CPS server.
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
 *                                  supported features that the CPS
 *                                  Server will support if this function
 *                                  is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CPS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CPS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CPS_Set_Feature(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t SupportedFeatures);

/**
 * @brief
 * Queries the supported CPS Features
 * for the CPS server.
 *
 * @details
 * The SupportedFeatures parameter is a bitmask made up of bits of the form
 * QAPI_BLE_CPS_FEATURE_XXX.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CPS_Initialize_XXX()
 *                                 functions.
 *
 * @param[out]  SupportedFeatures    Pointer to a bitmask that will
 *                                   hold the supported features that the
 *                                   CPS server supports if this function
 *                                   is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CPS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CPS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CPS_Query_Feature(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t *SupportedFeatures);

/**
 * @brief
 * Sends a CP Measurement notification to a CPS client.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CPS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the CPS
 *                                 Client that will received the
 *                                 notification.
 *
 * @param[in]  MeasurementData     CP Measurement data that will be
 *                                 sent in the notification.
 *
 * @return      Positive nonzero if successful (represents the
 *              length of the notification).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CPS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CPS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CPS_Notify_CP_Measurement(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_CPS_Measurement_Data_t *MeasurementData);

/**
 * @brief
 * Sets the CPS Sensor Location.
 *
 * @details
 * The SupportedFeatures parameter is a bitmask made up of bits of the form
 * QAPI_BLE_CPS_SENSOR_LOCATION_XXX.
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
 * @param[in]  SensorLocation      Bitmask that contains the
 *                                 Sensor Location that will be set if this
 *                                 function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CPS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CPS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CPS_Set_Sensor_Location(uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t SensorLocation);

/**
 * @brief
 * Queries the supported CPS Features
 * for the CPS server.
 *
 * @details
 * The SupportedFeatures parameter is a bitmask made up of bits of the form
 * QAPI_BLE_CPS_SENSOR_LOCATION_XXX.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CPS_Initialize_XXX()
 *                                 functions.
 *
 * @param[out]  SensorLocation     Pointer to a bitmask that will hold
 *                                 the Sensor Location if this function
 *                                 is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CPS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CPS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CPS_Query_Sensor_Location(uint32_t BluetoothStackID, uint32_t InstanceID, uint8_t *SensorLocation);

/**
 * @brief
 * Sends a CP Vector notification to a CPS client.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CPS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the CPS
 *                                 client that will received the
 *                                 notification.
 *
 * @param[in]  VectorData          CP Vector data that will be sent
 *                                 in the notification.
 *
 * @return      Positive nonzero if successful (represents the
 *              length of the notification).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CPS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CPS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CPS_Notify_CP_Vector(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_CPS_Vector_Data_t *VectorData);

/**
 * @brief
 * Responds to a write
 * request from a CPS client for the CPS Control Point
 * Characteristic.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * QPAI_BLE_CPS_ERROR_CODE_XXX from qapi_ble_cpstypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * This function is primarily provided to allow a way to reject CPS
 * Control Point Commands if the corresponding CCCD has not been configured for
 * indications by the CPS client that made the request, a procedure is
 * already in progress, or the CPS client does not have the required
 * security such as authentication, authorization, or encryption.
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
 *                 QAPI_BLE_CPS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CPS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CPS_Control_Point_Response(uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t ErrorCode);

/**
 * @brief
 * Sends an indication for the
 * CPS Control Point Result. This is in response to an CPS Control Point
 * Command that was successful.
 *
 * This function should not be used if data (other than the result) needs to be
 * sent in the indication. Instead,
 * qapi_BLE_CPS_Indicate_Control_Point_Result_With_Data() should be used.
 *
 * @details
 * This function sends an indication, which provides a guarantee that the
 * CPS client will receive the value, since the CPS client must confirm
 * that it has been received.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the CPS Control
 * Point Characteristic that is going to be indicated has been previously
 * configured for indications. A CPS client must have written the CPS Control
 * Point's characteristic's CCCD to enable indications.
 *
 * @note1hang
 * Only one indication may be outstanding per CPS Instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CPS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the
 *                                 connection for the indication.
 *
 * @param[in]  CommandType         Enumeration for the CPS Control
 *                                 Point Command this indication is being
 *                                 sent for.
 *
 * @param[in]  ResponseCode        Response code for the indication,
 *                                 which represents the status of the CPS
 *                                 Control Point Command.
 *
 * @return      Positive nonzero if successful (represents the
 *              GATT Transaction ID for the indication).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CPS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CPS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CPS_Indicate_Control_Point_Result(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_CPS_Control_Point_Command_Type_t CommandType, uint8_t ResponseCode);

/**
 * @brief
 * Sends an indication for the
 * CPS Control Point Result. This is in response to an CPS Control Point
 * Command that was successful.
 *
 * If any error response needs to be sent for the specified set of commands,
 * the qapi_BLE_CPS_Indicate_Control_Point_Result() function should be used.
 *
 * @details
 * This function sends an indication, which provides a guarantee that the
 * CPS client will receive the value, since the CPS client must confirm
 * that it has been received.
 *
 * Only the following set of commands are valid for this API:
 *     - QAPI_BLE_CPC_REQUEST_SUPPORTED_SENSOR_LOCATIONS_E
 *     - QAPI_BLE_CPC_REQUEST_FACTORY_CALIBRATION_DATA_E
 *     - QAPI_BLE_CPC_REQUEST_CRANK_LENGTH_E
 *     - QAPI_BLE_CPC_REQUEST_CHAIN_LENGTH_E
 *     - QAPI_BLE_CPC_REQUEST_CHAIN_WEIGHT_E
 *     - QAPI_BLE_CPC_REQUEST_SPAN_LENGTH_E
 *     - QAPI_BLE_CPC_START_OFFSET_COMPENSATION_E
 *     - QAPI_BLE_CPC_REQUEST_SAMPLING_RATE_E
 *
 * If the response data is for supported sensor locations and the
 * SensorLocations data in SupportedSensorLocations field in Response data is
 * not NULL, it should be freed by calling the
 * qapi_BLE_CPS_Free_Supported_Sensor_Locations_Data() function when the
 * decoded Response data is no longer needed.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the CPS Control
 * Point Characteristic that is going to be indicated has been previously
 * configured for indications. A CPS client must have written the CPS Control
 * Point's Characteristic's CCCD to enable indications.
 *
 * @note1hang
 * Only one indication may be outstanding per CPS Instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CPS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the
 *                                 connection for the indication.
 *
 * @param[in]  IndicationData      Pointer to the CPS Control Point
 *                                 indication data that will be sent in the
 *                                 indication.
 *
 * @return      Positive nonzero if successful (represents the
 *              GATT Transaction ID for the indication).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CPS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CPS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CPS_Indicate_Control_Point_Result_With_Data(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_CPS_Control_Point_Indication_Data_t *IndicationData);

   /* CPS client API.                                                   */

/**
 * @brief
 * Formats the CPS Measurement
 * data into a user-specified buffer, for a GATT Write request, that will be
 * sent to the CPS server. This function may also be used to determine the
 * size of the buffer to hold the formatted data (see below).
 *
 * @param[in]       MeasurementData    Pointer to the CPS Measurement
 *                                     data that will be formatted into the
 *                                     user-specified buffer.
 *
 * @param[in, out]  BufferLength       Length of the user-specified
 *                                     buffer on input and the
 *                                     utilized/expected size of the
 *                                     formatted buffer on output.
 *
 * @param[out]      Buffer             User-specified buffer that
 *                                     will hold the formatted data if
 *                                     this function is successful. This
 *                                     may be excluded (NULL) if the
 *                                     expected size of the input buffer
 *                                     is going to be determined.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CPS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CPS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CPS_Format_CP_Measurement(qapi_BLE_CPS_Measurement_Data_t *MeasurementData, uint16_t *BufferLength, uint8_t *Buffer);

/**
 * @brief
 * Parses a value received in an
 * indication from a remote CPS server, interpreting it as CPS Measurement data.
 *
 * @param[in]  BufferLength    Length of the Buffer received from the
 *                             CPS server.
 *
 * @param[in]  Buffer          Buffer received from the
 *                             CPS server.
 *
 * @param[out]  MeasurementData   Pointer that will hold the decoded
 *                                CPS Measurement data if this function
 *                                is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CPS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CPS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CPS_Decode_CP_Measurement(uint32_t BufferLength, uint8_t *Buffer, qapi_BLE_CPS_Measurement_Data_t *MeasurementData);

/**
 * @brief
 * Parses a value received in a read
 * response from a remote CPS server, interpreting it as CPS Vector data.
 *
 * @details
 * The return value from this function must be freed by calling
 * qapi_BLE_CPS_Free_CP_Vector_Data() when the decoded CP Vector data is
 * no longer needed.
 *
 * @param[in]  BufferLength    Length of the Buffer received from the
 *                             CPS server.
 *
 * @param[in]  Buffer          Buffer received from the
 *                             CPS server.
 *
 * @return      A pointer to the decoded CPS Vector data if this function is
 *              successful. \n
 *              NULL, if an error occured.
 */
QAPI_BLE_DECLARATION qapi_BLE_CPS_Vector_Data_t *QAPI_BLE_BTPSAPI qapi_BLE_CPS_Decode_CP_Vector(uint32_t BufferLength, uint8_t *Buffer);

/**
 * @brief
 * Frees the decoded CP Vector Data
 * that was returned with a successful call to qapi_BLE_CPS_Decode_CP_Vector().
 *
 * @param[in]  VectorData    Pointer to the CP Vector Data that is
 *                           going to be freed.
 *
 * @return      None.
 */
QAPI_BLE_DECLARATION void QAPI_BLE_BTPSAPI qapi_BLE_CPS_Free_CP_Vector_Data(qapi_BLE_CPS_Vector_Data_t * VectorData);

/**
 * @brief
 * Parses a value received in a
 * write response from a remote CPS server, interpreting it as the CPS Control
 * Point response.
 *
 * @param[in]  BufferLength    Length of the Buffer received from the
 *                             CPS server.
 *
 * @param[in]  Buffer          Buffer received from the
 *                             CPS server.
 *
 * @param[out]  ResponseData   Pointer to the decoded response data for
 *                             the CPS Control Point if this function is
 *                             successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CPS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CPS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CPS_Decode_Control_Point_Response(uint32_t BufferLength, uint8_t *Buffer, qapi_BLE_CPS_Control_Point_Response_Data_t *ResponseData);

/**
 * @brief
 * Frees the Sensor Location Data
 * that is a member of qapi_BLE_CPS_Supported_Sensor_Location_t data that was
 * returned by a successful call to
 * qapi_BLE_CPS_Decode_Control_Point_Response.
 *
 * @param[in]  SensorLocations    Pointer to the CPS Sensor Location
 *                                Data that is going to be freed.
 *
 * @return      None.
 */
QAPI_BLE_DECLARATION void QAPI_BLE_BTPSAPI qapi_BLE_CPS_Free_Supported_Sensor_Locations_Data(uint8_t *SensorLocations);

/**
 * @brief
 * Formats the CPS Control Point
 * Command into a user-specified buffer, for a GATT Write request, that
 * will be sent to the CPS server. This function may also be used to
 * determine the size of the buffer to hold the formatted data (see below).
 *
 * @param[in]       FormatData         Pointer to the CPS Control Point
 *                                     Command data that will be formatted
 *                                     into the user-specified buffer.
 *
 * @param[in, out]  BufferLength       Length of the user-specified
 *                                     buffer on input and the
 *                                     utilized/expected size of the
 *                                     formatted buffer on output.
 *
 * @param[out]      Buffer             User-specified buffer that
 *                                     will hold the formatted data if
 *                                     this function is successful. This
 *                                     may be excluded (NULL) if the
 *                                     expected size of the input buffer
 *                                     is going to be determined.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CPS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CPS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CPS_Format_Control_Point_Command(qapi_BLE_CPS_Control_Point_Format_Data_t *FormatData, uint32_t *BufferLength, uint8_t *Buffer);

/** @}
 */

#endif

