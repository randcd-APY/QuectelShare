/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_ble_gls.h
 *
 * @brief
 * QCA QAPI for Bluetopia Bluetooth Glucose Service (GLS)
 * (GATT based) API Type Definitions, Constants, and Prototypes.
 *
 * @details
 * The Glucose Service (GLS) programming interface defines the protocols and
 * procedures to be used to implement the Glucose Service
 * capabilities.
 */

#ifndef __QAPI_BLE_GLS_H__
#define __QAPI_BLE_GLS_H__

#include "./qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.          */
#include "./qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.    */
#include "./qapi_ble_gatt.h"      /* QAPI GATT prototypes.                    */
#include "./qapi_ble_glstypes.h"  /* QAPI GLS prototypes.                     */

/**
 * @addtogroup qapi_ble_services
 * @{
 */

   /* Error Return Codes.                                               */

   /* Error Codes that are smaller than these (less than -1000) are     */
   /* related to the Bluetooth Protocol Stack itself (see               */
   /* qapi_ble_errors.h).                                               */
#define QAPI_BLE_GLS_ERROR_INVALID_PARAMETER             (-1000)
/**< Invalid parameter. */
#define QAPI_BLE_GLS_ERROR_INVALID_BLUETOOTH_STACK_ID    (-1001)
/**< Invalid Bluetooth Stack ID. */
#define QAPI_BLE_GLS_ERROR_INSUFFICIENT_RESOURCES        (-1002)
/**< Insufficient resources. */
#define QAPI_BLE_GLS_ERROR_INSUFFICIENT_BUFFER_SPACE     (-1003)
/**< Insufficient buffer space. */
#define QAPI_BLE_GLS_ERROR_SERVICE_ALREADY_REGISTERED    (-1004)
/**< Service is already registered. */
#define QAPI_BLE_GLS_ERROR_INVALID_INSTANCE_ID           (-1005)
/**< Invalid service instance ID. */
#define QAPI_BLE_GLS_ERROR_MALFORMATTED_DATA             (-1006)
/**< Malformatted data. */
#define QAPI_BLE_GLS_ERROR_INDICATION_OUTSTANDING        (-1007)
/**< Indication is outstanding. */
#define QAPI_BLE_GLS_ERROR_NOT_CONFIGURED_RACP           (-1008)
/**< RACP CCCD has not been configured. */
#define QAPI_BLE_GLS_ERROR_NOT_AUTHENTICATION            (-1009)
/**< No authentication. */
#define QAPI_BLE_GLS_ERROR_UNKNOWN_ERROR                 (-1010)
/**< Unknown error. */

/**
 * Structure that contains the attribute handles that will need to be
 * cached by a GLS client in order to only do service discovery once.
 */
typedef struct qapi_BLE_GLS_Client_Information_s
{
   /**
    * GLS Glucose Measurement attribute handle.
    */
   uint16_t Glucose_Measurement;

   /**
    * GLS Glucose Measurement attribute CCCD attribute handle.
    */
   uint16_t Glucose_Measurement_Client_Configuration;

   /**
    * GLS Glucose Measurement Context attribute handle.
    */
   uint16_t Measurement_Context;

   /**
    * GLS Glucose Measurement Context CCCD attribute handle.
    */
   uint16_t Measurement_Context_Client_Configuration;

   /**
    * GLS Glucose Feature attribute handle.
    */
   uint16_t Glucose_Feature;

   /**
    * GLS Record Access Control Point (RACP) attribute handle.
    */
   uint16_t Record_Access_Control_Point;

   /**
    * GLS RACP CCCD attribute handle.
    */
   uint16_t Record_Access_Control_Point_Client_Configuration;
} qapi_BLE_GLS_Client_Information_t;

#define QAPI_BLE_GLS_CLIENT_INFORMATION_DATA_SIZE        (sizeof(qapi_BLE_GLS_Client_Information_t))
/**<
 * Size of the #qapi_BLE_GLS_Client_Information_t structure.
 */

/**
 * Structure that contains all of the per client data that will need to
 * be stored by a GLS server.
 */
typedef struct qapi_BLE_GLS_Server_Information_s
{
   /**
    * GLS Glucose Measurement attribute CCCD.
    */
   uint16_t Glucose_Measurement_Client_Configuration;

   /**
    * GLS Glucose Measurement Context CCCD.
    */
   uint16_t Glucose_Context_Client_Configuration;

   /**
    * GLS Glucose Feature.
    */
   uint16_t Glucose_Features;

   /**
    * GLS RACP CCCD.
    */
   uint16_t Record_Access_Control_Point_Client_Configuration;
} qapi_BLE_GLS_Server_Information_t;

#define QAPI_BLE_GLS_SERVER_INFORMATION_DATA_SIZE        (sizeof(qapi_BLE_GLS_Server_Information_t))
/**<
 * Size of the #qapi_BLE_GLS_Server_Information_t structure.
 */

/**
 * Structure that represents the format for the sequence number range.
 */
typedef struct qapi_BLE_GLS_Sequence_Number_Range_Data_s
{
   /**
    * Minimum sequence number.
    */
   uint16_t Minimum;

   /**
    * Maximum sequence number.
    */
   uint16_t Maximum;
} qapi_BLE_GLS_Sequence_Number_Range_Data_t;

/**
 * Structure that represents the Date/Time data for GLS.
 */
typedef struct qapi_BLE_GLS_Date_Time_Data_s
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
} qapi_BLE_GLS_Date_Time_Data_t;

#define QAPI_BLE_GLS_DATE_TIME_DATA_SIZE                 (sizeof(qapi_BLE_GLS_Date_Time_Data_t))

/**<
 * Size of the #qapi_BLE_GLS_Date_Time_Data_t structure.
 */

/**
 * Structure that represents the Date/Time range for GLS.
 */
typedef struct qapi_BLE_GLS_Date_Time_Range_Data_s
{
   /**
    * Minimum date/time.
    */
   qapi_BLE_GLS_Date_Time_Data_t Minimum;

   /**
    * Maximum date/time.
    */
   qapi_BLE_GLS_Date_Time_Data_t Maximum;
} qapi_BLE_GLS_Date_Time_Range_Data_t;

#define QAPI_BLE_GLS_DATE_TIME_RANGE_DATA_SIZE           (sizeof(qapi_BLE_GLS_Date_Time_Range_Data_t))

/**<
 * Size of the #qapi_BLE_GLS_Date_Time_Range_Data_t structure.
 */

/**
 * Structure that represents all of the information that is associated
 * with the Glucose Concentation Value.
 *
 * If the Concentration valus is
 * not available, the ConcentrationValid flag is set to FALSE,
 * otherwaise it is TRUE.
 *
 * The values for Type and SampleLocation are defined
 * inqapi_ble_glstypes.h.
 */
typedef struct qapi_BLE_GLS_Concentration_Data_s
{
   /**
    * Flags if the concentration is valid.
    */
   boolean_t ConcentrationValid;

   /**
    * Concentration value.
    */
   uint16_t  Value;

   /**
    * Sample type.
    */
   uint8_t   Type;

   /**
    * Sample location.
    */
   uint8_t   SampleLocation;
} qapi_BLE_GLS_Concentration_Data_t;

#define QAPI_BLE_GLS_CONCENTRATION_DATA_SIZE             (sizeof(qapi_BLE_GLS_Concentration_Data_t))

/**<
 * Size of the #qapi_BLE_GLS_Concentration_Data_t structure.
 */

/**
 * Structure that represents the format for a Glucose Measurement.
 *
 * If the QAPI_BLE_GLS_MEASUREMENT_TYPE_SAMPLE_LOCATION_PRESENT Flag is
 * set, a valid value must be entered for both Type and Sample
 * Location in GlucoseConcentration.
 */
typedef struct qapi_BLE_GLS_Glucose_Measurement_Data_s
{
   /**
    * A bitmask that indicates the optional fields and features that may
    * be included for a GLS Measurement.
    *
    * Valid values have the form QAPI_BLE_GLS_MEASUREMENT_FLAGS_XXX and
    * can be found in qapi_ble_glstypes.h.
    */
   uint8_t                           OptionFlags;

   /**
    * Sequence number.
    */
   uint16_t                          SequenceNumber;

   /**
    * Base time.
    */
   qapi_BLE_GLS_Date_Time_Data_t     BaseTime;

   /**
    * Time offset.
    */
   uint16_t                          TimeOffset;

   /**
    * Glucose concentration data.
    */
   qapi_BLE_GLS_Concentration_Data_t GlucoseConcentration;

   /**
    * Sensor status.
    */
   uint16_t                          SensorStatus;
} qapi_BLE_GLS_Glucose_Measurement_Data_t;

#define QAPI_BLE_GLS_GLUCOSE_MEASUREMENT_DATA_SIZE       (sizeof(qapi_BLE_GLS_Glucose_Measurement_Data_t))

/**<
 * Size of the #qapi_BLE_GLS_Glucose_Measurement_Data_t structure.
 */

/**
 * Structure that represents the format for the GLS Carbohydrate data.
 */
typedef struct qapi_BLE_GLS_Carbohydrate_Data_s
{
   /**
    * Carbohydrate ID.
    */
   uint8_t  ID;

   /**
    * Carbohydrate value.
    */
   uint16_t Value;
} qapi_BLE_GLS_Carbohydrate_Data_t;

#define QAPI_BLE_GLS_CARBOHYDRATE_DATA_SIZE              (sizeof(qapi_BLE_GLS_Carbohydrate_Data_t))

/**<
 * Size of the #qapi_BLE_GLS_Carbohydrate_Data_t structure.
 */

/**
 * Structure that represents the format for the GLS Exercise data.
 */
typedef struct qapi_BLE_GLS_Exercise_Data_s
{
   /**
    * Duration of the exercise.
    */
   uint16_t Duration;

   /**
    * Intensity of the exercise.
    */
   uint8_t  Intensity;
} qapi_BLE_GLS_Exercise_Data_t;

#define QAPI_BLE_GLS_EXERCISE_DATA_SIZE                  (sizeof(qapi_BLE_GLS_Exercise_Data_t))

/**<
 * Size of the #qapi_BLE_GLS_Exercise_Data_t structure.
 */

/**
 * Structure that represents the format for the GLS Medication data.
 */
typedef struct qapi_BLE_GLS_Medication_Data_s
{
   /**
    * Medication ID.
    */
   uint8_t  ID;

   /**
    * Medication value.
    */
   uint16_t Value;
} qapi_BLE_GLS_Medication_Data_t;

#define QAPI_BLE_GLS_MEDICATION_DATA_SIZE                (sizeof(qapi_BLE_GLS_Medication_Data_t))

/**<
 * Size of the #qapi_BLE_GLS_Medication_Data_t structure.
 */

/**
 * Structure that represents the format for the GLS Glucose context data.
 *
 * If the QAPI_BLE_GLS_CONTEXT_TESTER_HEALTH_PRESENT Flag is set, a
 * valid value must be entered for both Tester and Health.
 */
typedef struct qapi_BLE_GLS_Glucose_Measurement_Context_Data_s
{
   /**
    * Bitmask that indicates the optional fields and features that may
    * be included for the GLS Context data.
    *
    * Valid values have the form
    * QAPI_BLE_GLS_MEASUREMENT_CONTEXT_FLAGS_XXX and can be found in
    * qapi_ble_glstypes.h.
    */
   uint8_t                          OptionFlags;

   /**
    * Sequence number.
    */
   uint16_t                         SequenceNumber;

   /**
    * Bitmask that indicates the extended flags for the GLS Context
    * data.
    */
   uint8_t                          ExtendedFlags;

   /**
    * Carbohydrate data.
    */
   qapi_BLE_GLS_Carbohydrate_Data_t Carbohydrate;

    /**
    * Meal.
    */
   uint8_t                          Meal;

    /**
    * Tester.
    */
   uint8_t                          Tester;

    /**
    * Health.
    */
   uint8_t                          Health;

    /**
    * Exercise data.
    */
   qapi_BLE_GLS_Exercise_Data_t     ExerciseData;

   /**
    * Medication data.
    */
   qapi_BLE_GLS_Medication_Data_t   Medication;

   /**
    * HbA1c.
    */
   uint16_t                         HbA1c;
} qapi_BLE_GLS_Glucose_Measurement_Context_Data_t;

#define QAPI_BLE_GLS_GLUCOSE_MEASUREMENT_CONTEXT_DATA_SIZE  (sizeof(qapi_BLE_GLS_Glucose_Measurement_Context_Data_t))
/**<
 * Size of the #qapi_BLE_GLS_Glucose_Measurement_Context_Data_t structure.
 */

/**
 * Enumeration that represents the values for the RACP Response Type that
 * is passed in the RACP Response structure.
 */
typedef enum
{
   QAPI_BLE_GLS_NUMBER_OF_STORED_RECORDS_E,
   /**< Number of Stored Records op code.   */
   QAPI_BLE_GLS_RESPONSE_CODE_E
   /**< Response code op code.   */
} qapi_BLE_GLS_RACP_Response_Type_t;

/**
 * Structure that represents the format for the GLS Record Access Control
 * Point (RACP) data.
 *
 * Values of RequestOpCode must be of the form
 * QAPI_BLE_GLS_RECORD_ACCESS_OPCODE_XXX and the values of
 * ResponseCodeValue must be of the form
 * QAPI_BLE_GLS_RECORD_ACCESS_RESPONSE_CODE_XXX
 */
typedef struct qapi_BLE_GLS_RACP_Response_Code_Data_s
{
   /**
    * Request op code.
    */
   uint8_t RequestOpCode;

   /**
    * Response code value.
    */
   uint8_t ResponseCodeValue;
} qapi_BLE_GLS_RACP_Response_Code_Value_t;

/**
 * Structure that represents the format for the GLS RACP response data.
 */
typedef struct qapi_BLE_GLS_Record_Access_Control_Point_Response_Data_s
{
   /**
    * Response type.
    */
   qapi_BLE_GLS_RACP_Response_Type_t ResponseType;

   union
   {
      /**
       * Number of stored records.
       */
      uint16_t                                NumberOfStoredRecordsResult;

      /**
       * Response code value.
       */
      qapi_BLE_GLS_RACP_Response_Code_Value_t ResponseCodeValue;
   }
   /**
    * Response data.
    */
   ResponseData;
} qapi_BLE_GLS_Record_Access_Control_Point_Response_Data_t;

#define QAPI_BLE_GLS_RECORD_ACCESS_CONTROL_POINT_RESPONSE_DATA_SIZE  (sizeof(qapi_BLE_GLS_Record_Access_Control_Point_Response_Data_t))
/**<
 * Size of the #qapi_BLE_GLS_Record_Access_Control_Point_Response_Data_t
 * structure.
 */

/**
 * Enumeration that represents the valid Read Request types that a server
 * may receive in a
 * QAPI_BLE_ET_GLS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E or
 * QAPI_BLE_ET_GLS_SERVER_CLIENT_CONFIGURATION_UPDATE_E event. This is
 * also used by the qapi_BLE_GLS_Send_Notification() to denote the
 * characteristic value to notify.
 *
 * For each event, it is up to the application to return (or write) the
 * correct Client Configuration descriptor based on this value.
 */
typedef enum
{
   QAPI_BLE_GLS_CT_GLUCOSE_MEASUREMENT_E,
   /**< Glucose Measurement. */
   QAPI_BLE_GLS_CT_GLUCOSE_MEASUREMENT_CONTEXT_E,
   /**< Glucose Measurement Context. */
   QAPI_BLE_GLS_CT_RECORD_ACCESS_CONTROL_POINT_E
   /**< Record Access Control Point. */
} qapi_BLE_GLS_Characteristic_Type_t;

/**
 * Enumeration that represents all the events generated by the GLS
 * Profile. These are used to determine the type of each event
 * generated, and to ensure the proper union element is accessed for the
 * #qapi_BLE_GLS_Event_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_ET_GLS_READ_CLIENT_CONFIGURATION_REQUEST_E,
   /**< Read CCCD request event. */
   QAPI_BLE_ET_GLS_CLIENT_CONFIGURATION_UPDATE_E,
   /**< Write CCCD request event. */
   QAPI_BLE_ET_GLS_RECORD_ACCESS_CONTROL_POINT_COMMAND_E,
   /**< Write RACP request event. */
   QAPI_BLE_ET_GLS_CONFIRMATION_DATA_E
   /**< Confirmation event. */
} qapi_BLE_GLS_Event_Type_t;

/**
 * Structure that represents the format for the data that is dispatched
 * to a GLS server when a GLS client has sent a request to read a GLS
 * characteristic's CCCD.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_GLS_Read_Client_Configuration_Response() function.
 */
typedef struct qapi_BLE_GLS_Read_Client_Configuration_Data_s
{
   /**
    * GLS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the GLS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the GLS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * Bluetooth address of the GLS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the requested CCCD based on the GLS characteristic
    * type.
    */
   qapi_BLE_GLS_Characteristic_Type_t ClientConfigurationType;
} qapi_BLE_GLS_Read_Client_Configuration_Data_t;

#define QAPI_BLE_GLS_READ_CLIENT_CONFIGURATION_DATA_SIZE  (sizeof(qapi_BLE_GLS_Read_Client_Configuration_Data_t))
/**<
 * Size of the #qapi_BLE_GLS_Read_Client_Configuration_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a GLS server when a GLS client has sent a request to write a GLS
 * characteristic's CCCD.
 */
typedef struct qapi_BLE_GLS_Client_Configuration_Update_Data_s
{
   /**
    * GLS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the GLS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the GLS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * Bluetooth address of the GLS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the requested CCCD based on the GLS characteristic
    * type.
    */
   qapi_BLE_GLS_Characteristic_Type_t ClientConfigurationType;

   /**
    * CCCD value that has been requested to be written.
    */
   uint16_t                           ClientConfiguration;
} qapi_BLE_GLS_Client_Configuration_Update_Data_t;

#define QAPI_BLE_GLS_CLIENT_CONFIGURATION_UPDATE_DATA_SIZE  (sizeof(qapi_BLE_GLS_Client_Configuration_Update_Data_t))
/**<
 * Size of the #qapi_BLE_GLS_Client_Configuration_Update_Data_t structure.
 */

/**
 * Enumeration that represents the valid values that may be set as the
 * value for the OpCode field of Record Access Control Point
 * characteristic.
 */
typedef enum
{
   QAPI_BLE_GLS_COMMAND_REPORT_STORED_RECORDS_REQUEST_E    = QAPI_BLE_GLS_RECORD_ACCESS_OPCODE_REPORT_STORED_RECORDS,
   /**< Report stored records op code. */
   QAPI_BLE_GLS_COMMAND_DELETE_STORED_RECORDS_REQUEST_E    = QAPI_BLE_GLS_RECORD_ACCESS_OPCODE_DELETE_STORED_RECORDS,
   /**< Delete stored records op code. */
   QAPI_BLE_GLS_COMMAND_ABORT_OPERATION_REQUEST_E          = QAPI_BLE_GLS_RECORD_ACCESS_OPCODE_ABORT_OPERATION,
   /**< Abort operation op code. */
   QAPI_BLE_GLS_COMMAND_NUMBER_OF_STORED_RECORDS_REQUEST_E = QAPI_BLE_GLS_RECORD_ACCESS_OPCODE_REPORT_NUM_STORED_RECORDS
   /**< Report number of stored records op code. */
} qapi_BLE_GLS_RACP_Command_Type_t;

/**
 * Enumeration that represents the valid values that may be set as the
 * value for the Operator field of Record Access Control Point
 * characteristic.
 */
typedef enum
{
   QAPI_BLE_GLS_NULL_E                     = QAPI_BLE_GLS_RECORD_ACCESS_OPERATOR_NULL,
   /**< Null operator. */
   QAPI_BLE_GLS_ALL_RECORDS_E              = QAPI_BLE_GLS_RECORD_ACCESS_OPERATOR_ALL_RECORDS,
   /**< All record operator. */
   QAPI_BLE_GLS_LESS_THAN_OR_EQUAL_TO_E    = QAPI_BLE_GLS_RECORD_ACCESS_OPERATOR_LESS_THAN_OR_EQUAL_TO,
   /**< Less than or equal to operator. */
   QAPI_BLE_GLS_GREATER_THAN_OR_EQUAL_TO_E = QAPI_BLE_GLS_RECORD_ACCESS_OPERATOR_GREATER_THAN_OR_EQUAL_TO,
   /**< Greater than or equal to operator. */
   QAPI_BLE_GLS_WITHIN_RANGE_OF_E          = QAPI_BLE_GLS_RECORD_ACCESS_OPERATOR_WITHIN_RANGE_OF,
   /**< Within range of operator. */
   QAPI_BLE_GLS_FIRST_RECORD_E             = QAPI_BLE_GLS_RECORD_ACCESS_OPERATOR_FIRST_RECORD,
   /**< First record operator. */
   QAPI_BLE_GLS_LAST_RECORD_E              = QAPI_BLE_GLS_RECORD_ACCESS_OPERATOR_LAST_RECORD
   /**< Last record operator. */
} qapi_BLE_GLS_RACP_Operator_Type_t;

/**
 * Enumeration that represents the valid values that may be used as the
 * Filter Type values of a Record Access Control Point characteristic.
 */
typedef enum
{
   QAPI_BLE_GLS_FILTER_SEQUENCE_NUMBER_E  = QAPI_BLE_GLS_RECORD_ACCESS_FILTER_TYPE_SEQUENCE_NUMBER,
   /**< Filter by sequence number. */
   QAPI_BLE_GLS_FILTER_USER_FACING_TIME_E = QAPI_BLE_GLS_RECORD_ACCESS_FILTER_TYPE_USER_FACING_TIME
   /**< Filter by user facing time. */
} qapi_BLE_GLS_RACP_Filter_Type_t;

/**
 * Structure that represents the format of the GLS RACP request data.
 */
typedef struct qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_s
{
   /**
    * Command type.
    */
   qapi_BLE_GLS_RACP_Command_Type_t  CommandType;

   /**
    * Operator type.
    */
   qapi_BLE_GLS_RACP_Operator_Type_t OperatorType;

   /**
    * Filter type.
    */
   qapi_BLE_GLS_RACP_Filter_Type_t   FilterType;
   union
   {
      /**
       * Sequence number.
       */
      uint16_t                                  SequenceNumber;

      /**
       * User facing time.
       */
      qapi_BLE_GLS_Date_Time_Data_t             UserFacingTime;

      /**
       * Sequence number range.
       */
      qapi_BLE_GLS_Sequence_Number_Range_Data_t SequenceNumberRange;

      /**
       * User facing time range.
       */
      qapi_BLE_GLS_Date_Time_Range_Data_t       UserFacingTimeRange;
   }
   /**
    * Filter parameters.
    */
   FilterParameters;
} qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t;

#define QAPI_BLE_GLS_RECORD_ACCESS_CONTROL_POINT_FORMAT_DATA_SIZE  (sizeof(qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t))
/**<
 * Size of the #qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a GLS server when a GLS client has sent a request to write a GLS
 * RACP.
 *
 * Some of this structures fields will be required when responding to a
 * request using the
 * qapi_BLE_GLS_Record_Access_Control_Point_Response() function.
 */
typedef struct qapi_BLE_GLS_Record_Access_Control_Point_Command_Data_s
{
   /**
    * GLS instance that dispatched the event.
    */
   uint32_t                                               InstanceID;

   /**
    * GATT connection ID for the connection with the GLS client
    * that made the request.
    */
   uint32_t                                               ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                                               TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the GLS client.
    */
   qapi_BLE_GATT_Connection_Type_t                        ConnectionType;

   /**
    * Bluetooth address of the GLS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                                     RemoteDevice;

   /**
    * RACP request data.
    */
   qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t FormatData;
} qapi_BLE_GLS_Record_Access_Control_Point_Command_Data_t;

#define QAPI_BLE_GLS_RECORD_ACCESS_CONTROL_POINT_COMMAND_DATA_SIZE  (sizeof(qapi_BLE_GLS_Record_Access_Control_Point_Command_Data_t))
/**<
 * Size of the #qapi_BLE_GLS_Record_Access_Control_Point_Command_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a GLS server when a GLS client has responded to an outstanding
 * indication that was previously sent by the GLS server.
 */
typedef struct qapi_BLE_GLS_Confirmation_Data_s
{
   /**
    * GLS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the GLS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * Status of the indication.
    */
   uint8_t                         Status;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the GLS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * Bluetooth address of the GLS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;
} qapi_BLE_GLS_Confirmation_Data_t;

#define QAPI_BLE_GLS_CONFIRMATION_DATA_SIZE              (sizeof(qapi_BLE_GLS_Confirmation_Data_t))
/**<
 * Size of the #qapi_BLE_GLS_Confirmation_Data_t structure.
 */

/**
 * Structure that represents the container structure for holding all the
 * event data for a GLS instance.
 */
typedef struct qapi_BLE_GLS_Event_Data_s
{
   /**
    * Event type used to determine the appropriate union member of
    * the Event_Data field to access.
    */
   qapi_BLE_GLS_Event_Type_t Event_Data_Type;

   /**
    * Total size of the data contained in the event.
    */
   uint16_t                  Event_Data_Size;
   union
   {
      /**
       * GLS Read CCCD data.
       */
      qapi_BLE_GLS_Read_Client_Configuration_Data_t           *GLS_Read_Client_Configuration_Data;

      /**
       * GLS Write CCCD data.
       */
      qapi_BLE_GLS_Client_Configuration_Update_Data_t         *GLS_Client_Configuration_Update_Data;

      /**
       * GLS Write RACP data.
       */
      qapi_BLE_GLS_Record_Access_Control_Point_Command_Data_t *GLS_Record_Access_Control_Point_Command_Data;

      /**
       * GLS confirmation data.
       */
      qapi_BLE_GLS_Confirmation_Data_t                        *GLS_Confirmation_Data;
   }
   /**
    * Event data.
    */
   Event_Data;
} qapi_BLE_GLS_Event_Data_t;

#define QAPI_BLE_GLS_EVENT_DATA_SIZE                     (sizeof(qapi_BLE_GLS_Event_Data_t))
/**<
 * Size of the #qapi_BLE_GLS_Event_Data_t structure.
 */

/**
 * @brief
 * This declared type represents the Prototype Function for an
 * GLS Event Callback. This function will be called whenever a define
 * GLS Event occurs within the Bluetooth Protocol Stack that is
 * specified with the specified Bluetooth Stack ID.
 *
 * @details
 * The event information is passed to the user in an qapi_BLE_GLS_Event_Data_t
 * structure. This structure contains all the information about the
 * event that occurred.
 *
 * The caller should use the contents of the GLS Event Data
 * only in the context of this callback. If the caller requires
 * the data for a longer period of time, then the callback function
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
 * anyway because another GLS Event will not be processed while this
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
 * @param[in]  GLS_Event_Data        Pointer to a structure that contains
 *                                   information about the event that has
 *                                   occurred.
 *
 * @param[in]  CallbackParameter     User-defined value that will be
 *                                   received with the GLS Event data.
 *
 * @return None.
 */
typedef void (QAPI_BLE_BTPSAPI *qapi_BLE_GLS_Event_Callback_t)(uint32_t BluetoothStackID, qapi_BLE_GLS_Event_Data_t *GLS_Event_Data, uint32_t CallbackParameter);

   /* GLS server API.                                                   */

/**
 * @brief
 * Opens a GLS server on a specified Bluetooth Stack.
 *
 * @details
 * Only one GLS server may be open at a time, per the Bluetooth Stack ID.
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
 *                                  registered GLS service returned from
 *                                  qapi_BLE_GATT_Register_Service() API.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of GLS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_GLS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_GLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BT_GATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_GLS_Initialize_Service(uint32_t BluetoothStackID, qapi_BLE_GLS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

/**
 * @brief
 * Opens a GLS server on a specified Bluetooth Stack with
 * the ability to control the location of the service in the GATT dataglse.
 *
 * @details
 * Only one GLS server may be open at a time, per the Bluetooth Stack ID.
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
 *                                       GATT dataglse, and on output,
 *                                       returns the handle range that
 *                                       the service is using in the GATT
 *                                       dataglse.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of GLS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_GLS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_GLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BT_GATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_GLS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, qapi_BLE_GLS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

/**
 * @brief
 * Cleans up and frees all resources associated with a GLS Service Instance.
 *
 * @details
 * After this function is called, no other GLS Service function can be
 * called until after a successful call to the qapi_BLE_GLS_Initialize_Service()
 * function is performed.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from the qapi_BLE_GLS_Initialize_Service()
 *                                 function.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_GLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_GLS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_GLS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * Queries the number of attributes
 * that are contained in the GLS service that is registered with a
 * call to qapi_BLE_GLS_Initialize_Service() or
 * qapi_BLE_GLS_Initialize_Service_Handle_Range().
 *

 * @return   Positive, nonzero, number of attributes that would be
 *           registered by a GLS service instance.
 *
 * @return   Zero on failure.
 */
QAPI_BLE_DECLARATION unsigned int QAPI_BLE_BTPSAPI qapi_BLE_GLS_Query_Number_Attributes(void);

/**
 * @brief
 * Sets the supported GLS Features for the GLS server.
 *
 * @details
 * The SupportedFeatures parameter should be in the range between
 * QAPI_BLE_GLS_FEATURE_LOW_BATTERY_DETECTION_DURING_MEASUREMENT to
 * QAPI_BLE_GLS_FEATURE_MULTIPLE_BOND_SUPPORT.
 *
 * This function must be called after the GLS service is registered with
 * a successful call to qapi_BLE_GLS_Initialize_XXX() in order to set the default
 * features of the GLS server.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_GLS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  SupportedFeatures    Bitmask that contains the
 *                                  supported features that the GLS
 *                                  server will support if this function
 *                                  is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_GLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_GLS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_GLS_Set_Glucose_Feature(uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t SupportedFeatures);

/**
 * @brief
 * Queries the supported GLS features for the GLS server.
 *
 * @details
 * The SupportedFeatures parameter should be in the range between
 * QAPI_BLE_GLS_FEATURE_LOW_BATTERY_DETECTION_DURING_MEASUREMENT to
 * QAPI_BLE_GLS_FEATURE_MULTIPLE_BOND_SUPPORT.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_GLS_Initialize_XXX()
 *                                 functions.
 *
 * @param[out]  SupportedFeatures    Pointer to a bitmask that will
 *                                   hold the supported features that the
 *                                   GLS server supports if this function
 *                                   is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_GLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_GLS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_GLS_Query_Glucose_Feature(uint32_t BluetoothStackID, uint32_t InstanceID, uint16_t *SupportedFeatures);

/**
 * @brief
 * Provides a mechanism for a GLS server to
 * successfully respond to a received read client configuration request.
 *
 * @details
 * Possible Events:
 *
 *    QAPI_BLE_ET_GLS_READ_CLIENT_CONFIGURATION_REQUEST_E
 *
 * @param[in]  BluetoothStackID        Unique identifier assigned to this
 *                                     Bluetooth Protocol Stack via a
 *                                     call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID              Service instance ID to close.
 *                                     This is the value that was returned
 *                                     from the qapi_BLE_GLS_Initialize_Service()
 *                                     function.
 *
 * @param[in]  TransactionID           Transaction ID of the original
 *                                     read request. This value was
 *                                     received in the
 *                                     QAPI_BLE_ET_GLS_READ_CLIENT_CONFIGURATION_REQUEST_E
 *                                     event.
 *
 * @param[in]  ClientConfiguration     Specifies the CCCD to send
 *                                     to the remote device.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_GLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_GLS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_GLS_Read_Client_Configuration_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t ClientConfiguration);

/**
 * @brief
 * Responds to a write
 * request from a GLS client for the GLS Record Access Control Point
 * (RACP) Characteristic.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * QPAI_BLE_GLS_ERROR_CODE_XXX from qapi_ble_glstypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * This function is primarily provided to allow a way to reject RACP
 * commands if the corresponding CCCD has not been configured for
 * indications by the
 * GLS client that made the request, a procedure is already in
 * progress, or the GLS client does not have
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
 *                 QAPI_BLE_GLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_GLS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_GLS_Record_Access_Control_Point_Response(uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t ErrorCode);

/**
 * @brief
 * Sends a GLS Measurement notification to a GLS client.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of the
 *                                 qapi_BLE_GLS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the GLS
 *                                 client that will receive the
 *                                 notification.
 *
 * @param[in]  MeasurementData     Pointer to the GLS Measurement data
 *                                 that will be notified to the GLS
 *                                 client.
 *
 * @return      Positive nonzero if successful (represents the
 *              length of the notification).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_GLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_GLS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_GLS_Notify_Glucose_Measurement(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_GLS_Glucose_Measurement_Data_t *MeasurementData);

/**
 * @brief
 * Sends a GLS Measurement Context notification to a GLS client.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of the
 *                                 qapi_BLE_GLS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the GLS
 *                                 client that will receive the
 *                                 notification.
 *
 * @param[in]  ContextData         Pointer to the GLS Measurement
 *                                 Context data that will be notified to
 *                                 the GLS client.
 *
 * @return      Positive nonzero if successful (represents the
 *              length of the notification).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_GLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_GLS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_GLS_Notify_Glucose_Measurement_Context(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_GLS_Glucose_Measurement_Context_Data_t *ContextData);

/**
 * @brief
 * Sends an indication for the
 * number of stored records. This is in response to an RACP request for the
 * Number of Stored Records that was successful.
 *
 * @details
 * This function sends an indication, which provides a guarantee that the
 * GLS client will receive the value, since the GLS client must confirm
 * that it has been received.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the Number Of Stored
 * Records characteristic that is going to be indicated has been previously
 * configured for indications. A GLS client must have written the GLS RACP
 * characteristic's CCCD to enable indications.
 *
 * @note1hang
 * Only one indication may be outstanding per GLS instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_GLS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the
 *                                 connection for the indication.
 *
 * @param[in]  NumberOfStoredRecords    Number of stored records to
 *                                      indicate to the GLS client.
 *
 * @return      Positive nonzero if successful (represents the
 *              GATT Transaction ID for the indication).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_GLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_GLS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_GLS_Indicate_Number_Of_Stored_Records(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, uint16_t NumberOfStoredRecords);

/**
 * @brief
 * Sends an indication for the RACP
 * request to the GLS client. This function
 * is used to indicate whether the RACP Procedure was successful or an error
 * occured.
 *
 * @details
 * This function sends an indication, which provides a guarantee that the
 * GLS client will receive the value, since the GLS client must confirm
 * that it has been received.
 *
 * This function must not be used if the GLS server is responding to an RACP
 * Request for the Number Of Stored Records and the request has been accepted.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the RACP
 * characteristic that is going to be indicated has been
 * previously configured for indications. A GLS client must have written the
 * GLS RACP characteristic's CCCD to enable
 * indications.
 *
 * @note1hang
 * Only one indication may be outstanding per GLS Instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_GLS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the
 *                                 connection for the indication.
 *
 * @param[in]  CommandType         Enumeration for the Op Code of the
 *                                 RACP Request.
 *
 * @param[in]  ResponseCode        Response Code of the RACP
 *                                 Procedure result. This indicates
 *                                 whether the procedure was successful
 *                                 or an error occured.
 *
 * @return      Positive nonzero if successful (represents the
 *              GATT Transaction ID for the indication).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_GLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_GLS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_GLS_Indicate_Record_Access_Control_Point_Result(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_GLS_RACP_Command_Type_t CommandType, uint8_t ResponseCode);

   /* GLS client API.                                                   */

/**
 * @brief
 * Parses a value received in an
 * indication from a remote GLS server, interpreting it as a GLS
 * Measurement.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            GLS server.
 *
 * @param[in]  Value          Value received from the
 *                            GLS server.
 *
 * @param[out]  MeasurementData    Pointer that will hold the
 *                                 decoded GLS Measurement data if this
 *                                 function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_GLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_GLS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_GLS_Decode_Glucose_Measurement(uint32_t ValueLength, uint8_t *Value, qapi_BLE_GLS_Glucose_Measurement_Data_t *MeasurementData);

/**
 * @brief
 * Parses a value received in an
 * indication from a remote GLS server, interpreting it as a GLS Measurement
 * Context.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            GLS server.
 *
 * @param[in]  Value          Value received from the
 *                            GLS server.
 *
 * @param[out]  ContextData    Pointer that will hold the  decoded
 *                             GLS Measurement Context data if this
 *                             function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_GLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_GLS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_GLS_Decode_Glucose_Measurement_Context(uint32_t ValueLength, uint8_t *Value, qapi_BLE_GLS_Glucose_Measurement_Context_Data_t *ContextData);

/**
 * @brief
 * Parses a value received in an
 * indication from a remote GLS server, interpreting it as the GLS
 * RACP characteristic.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            GLS server.
 *
 * @param[in]  Value          Value received from the
 *                            GLS server.
 *
 * @param[out]  RACPData      Pointer that will hold the RACP Response
 *                            data if this function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_GLS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_GLS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_GLS_Decode_Record_Access_Control_Point_Response(uint32_t ValueLength, uint8_t *Value, qapi_BLE_GLS_Record_Access_Control_Point_Response_Data_t *RACPData);

/**
 * @brief
 * Formats the GLS RACP
 * characteristic request into a user-specified buffer for
 * a GATT Write request that will be sent to the GLS server.
 *
 * @details
 * The parameter BufferLength is the size of input request and it
 * will hold the size of theoutput Buffer after formatting.
 *
 * @param[in]  FormatData            Pointer to the RACP Request data
 *                                   that will be formatted into the
 *                                   user-specified buffer.
 *
 * @param[in,out]  BufferLength      Pointer that, on input, will hold
 *                                   the length of the user-specified
 *                                   buffer, and on output, will hold the
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
 *                 QAPI_BLE_GLS_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_GLS_Format_Record_Access_Control_Point_Command(qapi_BLE_GLS_Record_Access_Control_Point_Format_Data_t *FormatData, uint32_t *BufferLength, uint8_t *Buffer);

/**
 * @}
 */

#endif

