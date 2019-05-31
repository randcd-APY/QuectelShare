/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_ble_plxs.h
 *
 * @brief
 * QCA QAPI for Bluetopia Bluetooth Pulse Oximeter Service
 * (GATT based) API Type Definitions, Constants, and
 * Prototypes.
 *
 * @details
 * The Pulse Oximeter Service (PLXS) programming interface defines the
 * protocols and procedures to be used to implement the Object Transfer Service
 * capabilities.
 */

#ifndef __QAPI_BLE_PLXS_H__
#define __QAPI_BLE_PLXS_H__

#include "./qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.          */
#include "./qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.    */
#include "./qapi_ble_gatt.h"      /* qapi GATT prototypes.                    */
#include "./qapi_ble_plxstypes.h" /* QAPI PLXS prototypes.                    */

/** @addtogroup qapi_ble_services
@{
*/

   /* Error Return Codes.                                               */

   /* Error Codes that are smaller than these (less than -1000) are     */
   /* related to the Bluetooth Protocol Stack itself (see               */
   /* qapi_ble_errors.h).                                               */
#define QAPI_BLE_PLXS_ERROR_INVALID_PARAMETER                     (-1000)
/**< Invalid parameter. */
#define QAPI_BLE_PLXS_ERROR_INVALID_BLUETOOTH_STACK_ID            (-1001)
/**< Invalid Bluetooth Stack ID. */
#define QAPI_BLE_PLXS_ERROR_INSUFFICIENT_RESOURCES                (-1002)
/**< Insufficient resources. */
#define QAPI_BLE_PLXS_ERROR_SERVICE_ALREADY_REGISTERED            (-1003)
/**< Service is already registered. */
#define QAPI_BLE_PLXS_ERROR_INVALID_INSTANCE_ID                   (-1004)
/**<Invalid service instance ID. */
#define QAPI_BLE_PLXS_ERROR_MALFORMATTED_DATA                     (-1005)
/**< Malformatted data. */
#define QAPI_BLE_PLXS_ERROR_INSUFFICIENT_BUFFER_SPACE             (-1006)
/**< Insufficient buffer space. */
#define QAPI_BLE_PLXS_ERROR_MEASUREMENT_NOT_SUPPORTED             (-1007)
/**< Measurement is not supported. */
#define QAPI_BLE_PLXS_ERROR_INVALID_CCCD_TYPE                     (-1008)
/**< Invalid CCCD Type. */
#define QAPI_BLE_PLXS_ERROR_INVALID_ATTRIBUTE_HANDLE              (-1009)
/**< Invalid attribute handle. */
#define QAPI_BLE_PLXS_ERROR_RACP_NOT_SUPPORTED                    (-1010)
/**< RACP is not supported. */
#define QAPI_BLE_PLXS_ERROR_INVALID_RACP_RESPONSE_OP_CODE         (-1011)
/**< Invalid RACP response op code. */

/**
 * Structure that represents the information needed to initialize the
 * PLXS server.
 *
 * The Spot_Check_Measurement and Continuous_Measurement fields indicate
 * whether the PLXS server will support these characteristics. At least
 * one of these fields must be supported.
 *
 * If the Spot Check Measurement Characteristic is supported, a Client
 * Characteristic Configuration Descriptor (CCCD), will automatically be
 * included so that a PLXS client can enable indications.
 *
 * If the Continuous_Measurement Characteristic is supported a CCCD
 * will automatically be
 * included so that a PLXS client can enable notifications.
 *
 * The Measurement_Storage field indicates if the PLXS server is capable
 * of storing Spot Check Measurements. The Spot_Check_Measurement field
 * must be TRUE, since only Spot-Check Measurements may be stored.
 * Otherwise, this field will be ignored if Spot_Check_Measurement is
 * FALSE.
 *
 * If Measurement_Storage is TRUE and all above requirements have been
 * met, a Record Access Control Point (RACP) characteristic will
 * automatically be included. A CCCD
 * CCCD will automatically be included so that a PLXS
 * client can enable indications.
 */
typedef struct qapi_BLE_PLXS_Initialize_Data_s
{
   /**
    * Flags if the Spot-Check Measurement is supported.
    */
   boolean_t Spot_Check_Measurement;

   /**
    * Flags if the Continuous Measurement is supported.
    */
   boolean_t Continuous_Measurement;

   /**
    * Flags if the PLXS server is capable of stored Spot-Check
    * Measurements.
    */
   boolean_t Measurement_Storage;
} qapi_BLE_PLXS_Initialize_Data_t;

#define QAPI_BLE_PLXS_INITIALIZE_DATA_SIZE                        (sizeof(qapi_BLE_PLXS_Initialize_Data_t))
/**<
 * Size of the #qapi_BLE_PLXS_Initialize_Data_t structure.
 */

/**
 * Structure that contains the attribute handles that will need to be
 * cached by a PLXS client in order to only do service discovery once.
 */
typedef struct qapi_BLE_PLXS_Client_Information_s
{
   /**
    * Spot Check Measurement attribute handle.
    */
   uint16_t Spot_Check_Measurement;

   /**
    * Spot Check Measurement CCCD attribute handle.
    */
   uint16_t Spot_Check_Measurement_CCCD;

   /**
    * Continuous Measurement attribute handle.
    */
   uint16_t Continuous_Measurement;

   /**
    * The Continuous Measurement CCCD attribute handle.
    */
   uint16_t Continuous_Measurement_CCCD;

   /**
    * PLX Features attribute handle.
    */
   uint16_t PLX_Features;

   /**
    * Record Access Control Point attribute handle.
    */
   uint16_t Record_Access_Control_Point;

   /**
    * Record Access Control Point CCCD attribute handle.
    */
   uint16_t Record_Access_Control_Point_CCCD;
} qapi_BLE_PLXS_Client_Information_t;

#define QAPI_BLE_PLXS_CLIENT_INFORMATION_DATA_SIZE                (sizeof(qapi_BLE_PLXS_Client_Information_t))
/**<
 * Size of the #qapi_BLE_PLXS_Client_Information_t structure.
 */

/**
 * Structure that contains all of the per client data that will need to
 * be stored by a PLXS server.
 */
typedef struct qapi_BLE_PLXS_Server_Information_s
{
   /**
    * Spot Check Measurement CCCD.
    */
   uint16_t Spot_Check_Measurement_CCCD;

   /**
    * Continuous Measurement CCCD.
    */
   uint16_t Continuous_Measurement_CCCD;

   /**
    * Record Access Control Point CCCD.
    */
   uint16_t Record_Access_Control_Point_CCCD;
} qapi_BLE_PLXS_Server_Information_t;

#define QAPI_BLE_PLXS_SERVER_INFORMATION_DATA_SIZE                (sizeof(qapi_BLE_PLXS_Server_Information_t))
/**<
 * Size of the #qapi_BLE_PLXS_Server_Information_t structure.
 */

/**
 * Structure that represents the format for the PLXS INT24 data type.
 */
typedef struct qapi_BLE_PLXS_INT24_Data_s
{
   /**
    * Lower 16 bits of the INT24 value.
    */
   uint16_t Lower;

   /**
    * Upper 8 bits of the INT24 value.
    */
   uint8_t  Upper;
} qapi_BLE_PLXS_INT24_Data_t;

#define QAPI_BLE_PLXS_INT24_DATA_SIZE                             (sizeof(qapi_BLE_PLXS_INT24_Data_t))
/**<
 * Size of the #qapi_BLE_PLXS_INT24_Data_t structure.
 */

/**
 * Enumeration that represents the PLXS client CCCD types.
 */
typedef enum
{
   QAPI_BLE_PLXS_CT_SPOT_CHECK_MEASUREMENT_E,
   /**< Spot Check Measurement. */
   QAPI_BLE_PLXS_CT_CONTINUOUS_MEASUREMENT_E,
   /**< Continuous Measurement. */
   QAPI_BLE_PLXS_CT_RACP_E
   /**< RACP. */
} qapi_BLE_PLXS_CCCD_Type_t;

/**
 * Structure that represents the PLXS Features. This structure is used
 * to identify the supported features of the PLXS server. This
 * structure is also used to identify the supported Measurement Status
 * bits and Device and Sensor Status bits that may optionally be included
 * in PLXS Measurements.
 *
 * These fields contain global features for the PLXS server. If a
 * feature or option is not supported, that feature cannot be used
 * for PLXS Measurements.
 */
typedef struct qapi_BLE_PLXS_Features_Data_s
{
   /**
    * Bitmask for the supported features of the PLXS server. Valid
    * values have the form QAPI_BLE_PLXS_FEATURES_XXX, and can be found
    * in qapi_ble_plxstypes.h.
    */
   uint16_t                   Support_Features;

   /**
    * Bitmask for the measurement statuses that are supported by the
    * PLXS server. Valid values have the form QAPI_BLE_PLXS_MSS_XXX, and
    * can be found in qapi_ble_plxstypes.h.
    */
   uint16_t                   Measurement_Status_Support;

   /**
    * Bitmask for the device and sensor statuses that are supported
    * by the PLXS server. Valid values have the form
    * QAPI_BLE_PLXS_DSSS__XXX, and can be found in qapi_ble_plxstypes.h.
    */
   qapi_BLE_PLXS_INT24_Data_t Device_And_Sensor_Status_Support;
} qapi_BLE_PLXS_Features_Data_t;

#define QAPI_BLE_PLXS_FEATURES_DATA_SIZE                          (sizeof(qapi_BLE_PLXS_Features_Data_t))
/**<
 * Size of the #qapi_BLE_PLXS_Features_Data_t structure.
 */

/**
 * Structure that represents the Date/Time data for PLXS.
 */
typedef struct qapi_BLE_PLXS_Date_Time_Data_s
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
} qapi_BLE_PLXS_Date_Time_Data_t;

#define QAPI_BLE_PLXS_DATE_TIME_DATA_SIZE                         (sizeof(qapi_BLE_PLXS_Date_Time_Data_t))
/**<
 * Size of the #qapi_BLE_PLXS_Date_Time_Data_t structure.
 */

#define QAPI_BLE_PLXS_DATE_TIME_VALID(_x)                         ((QAPI_BLE_GATT_DATE_TIME_VALID_YEAR(((_x)).Year)) && (QAPI_BLE_GATT_DATE_TIME_VALID_MONTH(((_x)).Month)) && (QAPI_BLE_GATT_DATE_TIME_VALID_DAY(((_x)).Day)) && (QAPI_BLE_GATT_DATE_TIME_VALID_HOURS(((_x)).Hours)) && (QAPI_BLE_GATT_DATE_TIME_VALID_MINUTES(((_x)).Minutes)) && (QAPI_BLE_GATT_DATE_TIME_VALID_SECONDS(((_x)).Seconds)))
/**<
 * This macro is a utility macro that exists to validate that a specified
 * Date Time is valid. The only parameter to this function is the
 * #qapi_BLE_PLXS_Date_Time_Data_t structure to validate. This macro
 * returns TRUE if the Date Time is valid or FALSE otherwise..
 */

/**
 * Enumeration that represents the PLXS Measurement types.
 */
typedef enum
{
   QAPI_BLE_PLXS_MT_SPOT_CHECK_MEASUREMENT_E,
   /**< Spot Check Measurement. */
   QAPI_BLE_PLXS_MT_CONTINUOUS_MEASUREMENT_E
   /**< Continuous Measurement. */
} qapi_BLE_PLXS_Measurement_Type_t;

/**
 * Structure that represents the format for a PLXS Spot-Check
 * Measurement.
 *
 * The Flags, SpO2, and PR fields are mandatory fields, however the
 * remaining fields may be optionally included if specfied by the Flags
 * field.
 *
 * In order to use some optional fields and their bits values, the PLXS
 * server must support the feature (see the #qapi_BLE_PLXS_Features_Data_t
 * structure for more information). These are global requirements that
 * affect each Spot-Check Measurement.
 */
typedef struct qapi_BLE_PLXS_Spot_Check_Measurement_Data_s
{
   /**
    * Bitmask for the optional fields and features of a Spot
    * Check Measurement. Valid values have the form
    * QAPI_BLE_PLXS_SPOT_CHECK_MEASUREMENT_FLAGS_XXX, and can be found in
    * qapi_ble_plxstypes.h.
    */
   uint8_t                        Flags;

   /**
    * SpO2.
    */
   uint16_t                       SpO2;

   /**
    * PR.
    */
   uint16_t                       PR;

   /**
    * Date/Time for the Spot Check Measurement.
    */
   qapi_BLE_PLXS_Date_Time_Data_t Timestamp;

   /**
    * Spot Check Measurement status.
    */
   uint16_t                       Measurement_Status;

   /**
    * Device and sensor status when the Spot Check Measurement was
    * taken.
    */
   qapi_BLE_PLXS_INT24_Data_t     Device_And_Sensor_Status;

   /**
    * Pulse amplitude index.
    */
   uint16_t                       Pulse_Amplitude_Index;
} qapi_BLE_PLXS_Spot_Check_Measurement_Data_t;

#define QAPI_BLE_PLXS_SPOT_CHECK_MEASUREMENT_DATA_SIZE            (sizeof(qapi_BLE_PLXS_Spot_Check_Measurement_Data_t))
/**<
 * Size of the #qapi_BLE_PLXS_Spot_Check_Measurement_Data_t structure.
 */

/**
 * Structure that represents the format for a PLXS Spot-Check
 * Measurement.
 *
 * The Flags, SpO2, and PR fields are mandatory fields, however the
 * remaining fields may be optionally included if specfied by the Flags
 * field.
 *
 * In order to use some optional fields and their bits values, the PLXS
 * server must support the feature (see the #qapi_BLE_PLXS_Features_Data_t
 * structure for more information). These are global requirements that
 * affect each Spot-Check Measurement.
 */
typedef struct qapi_BLE_PLXS_Continuous_Measurement_Data_s
{
   /**
    * Bitmask for the optional fields and features of a Spot
    * Check Measurement. Valid values have the form
    * QAPI_BLE_PLXS_CONTINUOUS_MEASUREMENT_FLAGS_XXX, and can be found
    * in qapi_ble_plxstypes.h.
    */
   uint8_t                    Flags;

   /**
    * Normal SpO2.
    */
   uint16_t                   SpO2_Normal;

   /**
    * Normal PR.
    */
   uint16_t                   PR_Normal;

   /**
    * Fast SpO2.
    */
   uint16_t                   SpO2_Fast;

   /**
    * Fast PR.
    */
   uint16_t                   PR_Fast;

   /**
    * Slow SpO2.
    */
   uint16_t                   SpO2_Slow;

   /**
    * Slow PR.
    */
   uint16_t                   PR_Slow;

   /**
    * Continuous Measurement status.
    */
   uint16_t                   Measurement_Status;

   /**
    * Device and sensor status when the Continuous Measurement was
    * taken.
    */
   qapi_BLE_PLXS_INT24_Data_t Device_And_Sensor_Status;

   /**
    * Pulse amplitude index.
    */
   uint16_t                   Pulse_Amplitude_Index;
} qapi_BLE_PLXS_Continuous_Measurement_Data_t;

#define QAPI_BLE_PLXS_CONTINUOUS_MEASUREMENT_DATA_SIZE            (sizeof(qapi_BLE_PLXS_Continuous_Measurement_Data_t))
/**<
 * Size of the #qapi_BLE_PLXS_Continuous_Measurement_Data_t structure.
 */

/**
 * Enumeration that represents the valid values that may be set for the
 * Op_Code field of the #qapi_BLE_PLXS_RACP_Request_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_PLXS_RRT_REPORT_STORED_RECORDS_REQUEST_E    = QAPI_BLE_PLXS_RACP_OPCODE_REPORT_STORED_RECORDS,
   /**< Report stored records op code. */
   QAPI_BLE_PLXS_RRT_DELETE_STORED_RECORDS_REQUEST_E    = QAPI_BLE_PLXS_RACP_OPCODE_DELETE_STORED_RECORDS,
   /**< Delete stored records op code. */
   QAPI_BLE_PLXS_RRT_ABORT_OPERATION_REQUEST_E          = QAPI_BLE_PLXS_RACP_OPCODE_ABORT_OPERATION,
   /**< Abort operation op code. */
   QAPI_BLE_PLXS_RRT_NUMBER_OF_STORED_RECORDS_REQUEST_E = QAPI_BLE_PLXS_RACP_OPCODE_REPORT_NUMBER_OF_STORED_RECORDS
   /**< Number of stored records op code. */
} qapi_BLE_PLXS_RACP_Request_Type_t;

/**
 * Enumeration that represents the valid values that may be set for the
 * Operator field of the #qapi_BLE_PLXS_RACP_Request_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_PLXS_ROT_NULL_E        = QAPI_BLE_PLXS_RACP_OPERATOR_NULL,
   /**< NULL operator. */
   QAPI_BLE_PLXS_ROT_ALL_RECORDS_E = QAPI_BLE_PLXS_RACP_OPERATOR_ALL_RECORDS,
   /**< All records operator. */
} qapi_BLE_PLXS_RACP_Operator_Type_t;

/**
 * Structure that represents the Record Access Control Point (RACP)
 * Request data.
 */
typedef struct qapi_BLE_PLXS_RACP_Request_Data_s
{
   /**
    * RACP Op Code for the request.
    */
   qapi_BLE_PLXS_RACP_Request_Type_t  Op_Code;

   /**
    * RACP Operator for the request.
    */
   qapi_BLE_PLXS_RACP_Operator_Type_t Operator;
} qapi_BLE_PLXS_RACP_Request_Data_t;

#define QAPI_BLE_PLXS_RACP_REQUEST_DATA_SIZE                      (sizeof(qapi_BLE_PLXS_RACP_Request_Data_t))
/**<
 * Size of the #qapi_BLE_PLXS_RACP_Request_Data_t structure.
 */

/**
 * Enumeration that represents the valid values that may be set for the
 * Op_Code field of the #qapi_BLE_PLXS_RACP_Response_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_PLXS_RRT_NUMBER_OF_STORED_RECORDS_RESPONSE_E = QAPI_BLE_PLXS_RACP_OPCODE_NUMBER_OF_STORED_RECORDS_RESPONSE,
   /**< Number of stored records response op code. */
   QAPI_BLE_PLXS_RRT_RESPONSE_OPCODE_E                   = QAPI_BLE_PLXS_RACP_OPCODE_RESPONSE_CODE
   /**< Response op code. */
} qapi_BLE_PLXS_RACP_Response_Type_t;

/**
 * Enumeration that represents the valid values that may be set for the
 * Operand field of the #qapi_BLE_PLXS_RACP_Response_Data_t structure if
 * the Response_Op_Code field is not set to
 * QAPI_BLE_PLXS_RRT_NUMBER_OF_STORED_RECORDS_RESPONSE_E.
 */
typedef enum
{
   QAPI_BLE_PLXS_RRCV_SUCCESS_E                 = QAPI_BLE_PLXS_RACP_RESPONSE_CODE_VALUE_SUCCESS,
   /**< Success result code. */
   QAPI_BLE_PLXS_RRCV_OPCODE_NOT_SUPPORTED_E    = QAPI_BLE_PLXS_RACP_RESPONSE_CODE_VALUE_OPCODE_NOT_SUPPORTED,
   /**< Op code not supported result code. */
   QAPI_BLE_PLXS_RRCV_INVALID_OPERATOR_E        = QAPI_BLE_PLXS_RACP_RESPONSE_CODE_VALUE_INVALID_OPERATOR,
   /**< Invalid operator result code. */
   QAPI_BLE_PLXS_RRCV_OPERATOR_NOT_SUPPORTED_E  = QAPI_BLE_PLXS_RACP_RESPONSE_CODE_VALUE_OPERATOR_NOT_SUPPORTED,
   /**< Operator not supported result code. */
   QAPI_BLE_PLXS_RRCV_INVALID_OPERAND_E         = QAPI_BLE_PLXS_RACP_RESPONSE_CODE_VALUE_INVALID_OPERAND,
   /**< Invalid operand result code. */
   QAPI_BLE_PLXS_RRCV_NO_RECORDS_FOUND_E        = QAPI_BLE_PLXS_RACP_RESPONSE_CODE_VALUE_NO_RECORDS_FOUND,
   /**< No records found result code. */
   QAPI_BLE_PLXS_RRCV_ABORT_UNSUCCESSFUL_E      = QAPI_BLE_PLXS_RACP_RESPONSE_CODE_VALUE_ABORT_UNSUCCESSFUL,
   /**< Abort unsuccessful result code. */
   QAPI_BLE_PLXS_RRCV_PROCEDURE_NOT_COMPLETED_E = QAPI_BLE_PLXS_RACP_RESPONSE_CODE_VALUE_PROCEDURE_NOT_COMPLETED,
   /**< Procedure not completed result code. */
   QAPI_BLE_PLXS_RRCV_OPERAND_NOT_SUPPORTED_E   = QAPI_BLE_PLXS_RACP_RESPONSE_CODE_VALUE_OPERAND_NOT_SUPPORTED,
   /**< Operand not supported result code. */
} qapi_BLE_PLXS_RACP_Response_Code_Value_t;

/**
 * Structure that represents the RACP response data.
 *
 * The Response_Op_Code field of this structure
 * specifies which Operand field is valid. Currently, the following
 * fields are valid for the following values of the Response_Op_Code
 * field:
 *
 * @li QAPI_BLE_PLXS_RRT_NUMBER_OF_STORED_RECORDS_RESPONSE_E -- Number_Of_Stored_Records
 * @li QAPI_BLE_PLXS_RRT_RESPONSE_OPCODE_E                   -- Response_Code
 *
 */
typedef struct qapi_BLE_PLXS_RACP_Response_Data_s
{
   /**
    * RACP Opcode for the response.
    */
   qapi_BLE_PLXS_RACP_Response_Type_t Response_Op_Code;

   /**
    * RACP Operator for the response.
    */
   qapi_BLE_PLXS_RACP_Operator_Type_t Operator;

   /**
    * RACP Request Opcode for the response.
    */
   qapi_BLE_PLXS_RACP_Request_Type_t  Request_Op_Code;

   union
   {
      /**
       * Number of stored records operand.
       */
      uint16_t                                 Number_Of_Stored_Records;

      /**
       * Response code value operand.
       */
      qapi_BLE_PLXS_RACP_Response_Code_Value_t Response_Code;
   }
   /**
    * Operand for RACP responses. The value depends on the value of
    * the Response_Op_Code field. @newpagetable
    */
   Operand;
} qapi_BLE_PLXS_RACP_Response_Data_t;

#define QAPI_BLE_PLXS_RACP_RESPONSE_DATA_SIZE                     (sizeof(qapi_BLE_PLXS_RACP_Response_Data_t))
/**<
 * Size of the #qapi_BLE_PLXS_RACP_Response_Data_t structure.
 */

/**
 * Enumeration that represents all the events generated by PLXS for
 * the PLXS server. These are used to determine the type of each event
 * generated, and to ensure the proper union element is accessed for the
 * #qapi_BLE_PLXS_Event_Data_t structure.
 */
typedef enum qapi_BLE_PLXS_Event_Type_s
{
   QAPI_BLE_PLXS_ET_SERVER_READ_FEATURES_REQUEST_E,
   /**< Read Features request event. */
   QAPI_BLE_PLXS_ET_SERVER_WRITE_RACP_REQUEST_E,
   /**< Write RACP request event. */
   QAPI_BLE_PLXS_ET_SERVER_READ_CCCD_REQUEST_E,
   /**< Read CCCD request event. */
   QAPI_BLE_PLXS_ET_SERVER_WRITE_CCCD_REQUEST_E,
   /**< Write CCCD request event. */
   QAPI_BLE_PLXS_ET_SERVER_CONFIRMATION_E
   /**< Confirmation event. */
} qapi_BLE_PLXS_Event_Type_t;

/**
 * Structure that represents the format for the data that is dispatched
 * to an PLXS server when a PLXS client has sent a request to read the
 * PLX Feature Characteristic.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_PLXS_Read_Features_Request_Response() function.
 */
typedef struct qapi_BLE_PLXS_Read_Features_Request_Data_s
{
   /**
    * PLXS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the PLXS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the PLXS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * Bluetooth address of the PLXS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;
} qapi_BLE_PLXS_Read_Features_Request_Data_t;

#define QAPI_BLE_PLXS_READ_FEATURES_REQUEST_DATA_SIZE             (sizeof(qapi_BLE_PLXS_Read_Features_Request_Data_t))
/**<
 * Size of the #qapi_BLE_PLXS_Read_Features_Request_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a PLXS server when a PLXS client has sent a request to read the PLX
 * Feature Characteristic.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_PLXS_RACP_Request_Response() function.
 */
typedef struct qapi_BLE_PLXS_Write_RACP_Request_Data_s
{
   /**
    * PLXS instance that dispatched the event.
    */
   uint32_t                          InstanceID;

   /**
    * GATT connection ID for the connection with the PLXS client
    * that made the request.
    */
   uint32_t                          ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the PLXS client.
    */
   qapi_BLE_GATT_Connection_Type_t   ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                          TransactionID;

   /**
    * Bluetooth address of the PLXS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                RemoteDevice;

   /**
    * RACP request data.
    */
   qapi_BLE_PLXS_RACP_Request_Data_t RequestData;
} qapi_BLE_PLXS_Write_RACP_Request_Data_t;

#define QAPI_BLE_PLXS_WRITE_RACP_REQUEST_DATA_SIZE                (sizeof(qapi_BLE_PLXS_Write_RACP_Request_Data_t))
/**<
 * Size of the #qapi_BLE_PLXS_Write_RACP_Request_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a PLXS server when a PLXS client has sent a request to read a PLXS
 * characteristic's CCCD.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_PLXS_Read_CCCD_Request_Response() function.
 */
typedef struct qapi_BLE_PLXS_Read_CCCD_Request_Data_s
{
   /**
    * PLXS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the PLXS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the PLXS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * Bluetooth address of the PLXS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;

   /**
    * Identifies the requested CCCD based on the PLXS characteristic
    * type.
    */
   qapi_BLE_PLXS_CCCD_Type_t       Type;
} qapi_BLE_PLXS_Read_CCCD_Request_Data_t;

#define QAPI_BLE_PLXS_READ_CCCD_REQUEST_DATA_SIZE                 (sizeof(qapi_BLE_PLXS_Read_CCCD_Request_Data_t))
/**<
 * Size of the #qapi_BLE_PLXS_Read_CCCD_Request_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a PLXS server when a PLXS client has sent a request to write a PLXS
 * characteristic's CCCD.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_PLXS_Write_CCCD_Request_Response() function.
 */
typedef struct qapi_BLE_PLXS_Write_CCCD_Request_Data_s
{
   /**
    * PLXS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the PLXS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the PLXS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * Bluetooth address of the PLXS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;

   /**
    * Identifies the requested CCCD based on the PLXS Characteristic
    * type.
    */
   qapi_BLE_PLXS_CCCD_Type_t       Type;

   /**
    * CCCD value that has been requested to be written.
    */
   uint16_t                        Configuration;
} qapi_BLE_PLXS_Write_CCCD_Request_Data_t;

#define QAPI_BLE_PLXS_WRITE_CCCD_REQUEST_DATA_SIZE                (sizeof(qapi_BLE_PLXS_Write_CCCD_Request_Data_t))
/**<
 * Size of the #qapi_BLE_PLXS_Write_CCCD_Request_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to the PLXS server when a PLXS client has responded to an outstanding
 * indication that was previously sent by the PLXS server.
 */
typedef struct qapi_BLE_PLXS_Confirmation_Data_s
{
   /**
    * PLXS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the PLXS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the PLXS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * Bluetooth address of the PLXS client that made the request.
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
} qapi_BLE_PLXS_Confirmation_Data_t;

#define QAPI_BLE_PLXS_CONFIRMATION_DATA_SIZE                      (sizeof(qapi_BLE_PLXS_Confirmation_Data_t))
/**<
 * Size of the #qapi_BLE_PLXS_Confirmation_Data_t structure.
 */

/**
 * Structure that represents the container structure for holding all the
 * PLXS server event data for a PLXS instance.
 */
typedef struct qapi_BLE_PLXS_Event_Data_s
{
   /**
    * Event type used to determine the appropriate union member of
    * the Event_Data field to access.
    */
   qapi_BLE_PLXS_Event_Type_t Event_Data_Type;

   /**
    * Total size of the data contained in the event.
    */
   uint16_t                   Event_Data_Size;

   union
   {
      /**
       * PLXS Read PLX Features event data.
       */
      qapi_BLE_PLXS_Read_Features_Request_Data_t *PLXS_Read_Features_Request_Data;

      /**
       * PLXS Write RACP event data.
       */
      qapi_BLE_PLXS_Write_RACP_Request_Data_t    *PLXS_Write_RACP_Request_Data;

      /**
       * PLXS Read CCCD event data.
       */
      qapi_BLE_PLXS_Read_CCCD_Request_Data_t     *PLXS_Read_CCCD_Request_Data;

      /**
       * PLXS Write CCCD event data.
       */
      qapi_BLE_PLXS_Write_CCCD_Request_Data_t    *PLXS_Write_CCCD_Request_Data;

      /**
       * OTS confirmation event data.
       */
      qapi_BLE_PLXS_Confirmation_Data_t          *PLXS_Confirmation_Data;
   }
   /**
    * Event data.
    */
   Event_Data;
} qapi_BLE_PLXS_Event_Data_t;

#define QAPI_BLE_PLXS_EVENT_DATA_SIZE                             (sizeof(qapi_BLE_PLXS_Event_Data_t))
/**<
 * Size of the #qapi_BLE_PLXS_Event_Data_t structure.
 */

/**
 * @brief
 * This declared type represents the prototype function for a
 * PLXS instance event callback. This function will be called
 * whenever a PLXS instance event occurs that is associated with the
 * specified Bluetooth stack ID.

 * @details
 * The caller should use the contents of the PLXS instance event data
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
 * anyway because another PLXS instance event will not be processed while
 * this function call is outstanding).
 *
 * @note1hang
 * This function must not block and wait for PLXS instance events that can
 * only be satisfied by receiving other Bluetooth Protocol Stack events.
 * A Deadlock will occur because other event callbacks will not be issued
 * while this function is currently outstanding.
 *
 * @param[in]  BluetoothStackID      Unique identifier assigned to this
 *                                   Bluetooth Protocol Stack on which the
 *                                   event occurred.
 *
 * @param[in]  PLXS_Event_Data       Pointer to a structure that contains
 *                                   information about the event that has
 *                                   occurred.
 *
 * @param[in]  CallbackParameter     User-defined value that was supplied
 *                                   as an input parameter when the PLXS
 *                                   instance event callback was
 *                                   installed.
 *
 * @return None.
 */
typedef void (QAPI_BLE_BTPSAPI *qapi_BLE_PLXS_Event_Callback_t)(uint32_t BluetoothStackID, qapi_BLE_PLXS_Event_Data_t *PLXS_Event_Data, uint32_t CallbackParameter);

   /* PLXS server API.                                                  */

/**
 * @brief
 * Initializes a PLXS instance (PLXS server) on a specified
 * Bluetooth Protocol Stack.
 *
 * @details
 * Only one PLXS instance may be initialized at a time, per the Bluetooth
 * Protocol Stack ID.
 *
 * See the #qapi_BLE_PLXS_Initialize_Data_t structure for more
 * information about the InitializeData parameter. If this
 * parameter is not configured correctly, a PLXS error will be returned.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  Service_Flags       Flags that are used to
 *                                 register PLXS for the specified
 *                                 transport. These flags can be found
 *                                 in qapi_ble_gatt.h and have the form
 *                                 QAPI_BLE_GATT_SERVICE_FLAGS_XXX.
 *
 * @param[in]  InitializeData      Pointer to the data that is used to
 *                                 configure the PLXS server.
 *
 * @param[in]  EventCallback       PLXS event callback that will
 *                                 receive PLXS instance events.
 *
 * @param[in]  CallbackParameter   User-defined value that will be
 *                                 received with the specified
 *                                 EventCallback parameter.
 *
 * @param[out] ServiceID           Unique GATT service ID of the
 *                                 registered PLXS instance returned from
 *                                 the qapi_BLE_GATT_Register_Service()
 *                                 API.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the PLXS instance ID of the PLXS server that was
 *              successfully initialized for the specified Bluetooth
 *              Protocol Stack ID. This is the value that should be used
 *              in all subsequent function calls that require the PLXS
 *              Instance ID.
 *
 * @return      An error code if negative. PLXS error codes can be
 *              found in qapi_ble_plxs.h (QAPI_BLE_PLXS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_PLXS_Initialize_Service(uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_PLXS_Initialize_Data_t *InitializeData, qapi_BLE_PLXS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

/**
 * @brief
 * Initializes a PLXS instance (PLXS server) on a specified
 * Bluetooth Protocol Stack.
 *
 * Unlike qapi_BLE_PLXS_Initialize_Service(), this function allows the
 * application to select a attribute handle range in GATT to store the
 * service.
 *
 * @details
 * Only one PLXS instance may be initialized at a time, per the Bluetooth
 * Protocol Stack ID.
 *
 * See the #qapi_BLE_PLXS_Initialize_Data_t structure for more
 * information about the InitializeData parameter. If this
 * parameter is not configured correctly, a PLXS error will be returned.
 *
 * If the application wants GATT to select the attribute handle range for
 * the service, all fields of the ServiceHandleRange parameter must
 * be initialized to zero. The qapi_BLE_PLXS_Query_Number_Attributes()
 * function may be used after initializing a PLXS instance to determine
 * the attribute handle range for the PLXS instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  Service_Flags       Flags that are used to
 *                                 register PLXS for the specified
 *                                 transport. These flags can be found
 *                                 in qapi_ble_gatt.h and have the form
 *                                 QAPI_BLE_GATT_SERVICE_FLAGS_XXX.
 *
 * @param[in]  InitializeData      Pointer to the data that is used to
 *                                 configure the PLXS server.
 *
 * @param[in]  EventCallback       PLXS event callback that will
 *                                 receive PLXS instance events.
 *
 * @param[in]  CallbackParameter   User-defined value that will be
 *                                 received with the specified
 *                                 EventCallback parameter.
 *
 * @param[out] ServiceID           Unique GATT service ID of the
 *                                 registered PLXS instance returned from
 *                                 the qapi_BLE_GATT_Register_Service()
 *                                 API.
 *
 * @param[in,out]  ServiceHandleRange   Pointer that, on input, holds
 *                                      the handle range to store the
 *                                      service in GATT, and on output,
 *                                      contains the handle range for
 *                                      where the service is stored in
 *                                      GATT.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the PLXS instance ID of the PLXS server that was
 *              successfully initialized for the specified Bluetooth
 *              Protocol Stack ID. This is the value that should be used
 *              in all subsequent function calls that require the PLXS
 *              Instance ID.
 *
 * @return      An error code if negative. PLXS error codes can be
 *              found in qapi_ble_plxs.h (QAPI_BLE_PLXS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_PLXS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_PLXS_Initialize_Data_t *InitializeData, qapi_BLE_PLXS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

/**
 * @brief
 * Cleans up and frees all resources
 * associated with a PLXS Instance (PLXS server).
 *
 * @details
 * After this function is called, no other PLXS
 * function can be called until after a successful call to either of the
 * qapi_BLE_PLXS_Initialize_XXX() functions.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the PLXS instance.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. PLXS error codes can be
 *              found in qapi_ble_plxs.h (QAPI_BLE_PLXS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_PLXS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * Queries the number of attributes
 * that are contained in a PLXS instance that is registered with a call
 * to either of the qapi_BLE_PLXS_Initialize_XXX() functions.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the PLXS instance.
 *
 * @return   Positive, nonzero, number of attributes for the
 *           registered PLXS instance.
 *
 * @return   Zero on failure.
 */
QAPI_BLE_DECLARATION unsigned int QAPI_BLE_BTPSAPI qapi_BLE_PLXS_Query_Number_Attributes(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * Responds to a read request
 * received from a PLXS client for the PLXS Features Characteristic.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * qapi_ble_plxstypes.h (PLXS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * If the request is rejected, the Features parameter may be excluded
 * (NULL).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the PLXS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whether the request was
 *                                 accepted or rejected.
 *
 * @param[in]  Features            Pointer to the PLXS Features data
 *                                 that will be sent if the request is
 *                                 accepted.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. PLXS error codes can be
 *              found in qapi_ble_plxs.h (QAPI_BLE_PLXS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_PLXS_Read_Features_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_PLXS_Features_Data_t *Features);

/**
 * @brief
 * Indicates a PLXS Spot Check
 * Measurement to a PLXS client.
 *
 * @details
 * This function sends an indication that provides a guarantee that the
 * PLXS client will receive the value, since the PLXS client must confirm
 * that it has been received.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the PLXS Spot
 * Check Measurement CCCD
 * has been previously configured for indications. A PLXS client
 * must have written the PLXS Spot Check Measurement Characteristic's
 * CCCD to enable indications.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the PLXS instance that
 *                                 received the request.
 *
 * @param[in]  ConnectionID        GATT connection ID for the
 *                                 connection with the PLXS client.
 *
 * @param[in]  MeasurementData     Pointer to the PLXS Spot Check
 *                                 Measurement data that will be sent in
 *                                 the indication.
 *
 * @return      Positive nonzero if successful (represents the
 *              GATT Transaction ID for the indication).
 *
 * @return      An error code if negative. PLXS error codes can be
 *              found in qapi_ble_plxs.h (QAPI_BLE_PLXS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_PLXS_Indicate_Spot_Check_Measurement(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_PLXS_Spot_Check_Measurement_Data_t *MeasurementData);

/**
 * @brief
 * Notifies a PLXS Continuous
 * Measurement to a PLXS client.
 *
 * @details
 * This function sends a notification that provides no guarantee that
 * the PLXS client will receive the value.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the PLXS
 * Continuous Measurement CCCD
 * has been previously configured for notifications. A PLXS client
 * must have written the PLXS Continuous Measurement Characteristic's
 * CCCD to enable notifications.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the PLXS instance that
 *                                 received the request.
 *
 * @param[in]  ConnectionID        GATT connection ID for the
 *                                 connection with the PLXS client.
 *
 * @param[in]  MeasurementData     Pointer to the PLXS Continuous
 *                                 Measurement data that will be sent in
 *                                 the notification.
 *
 * @return      Positive nonzero if successful (represents the
 *              length of the notification).
 *
 * @return      An error code if negative. PLXS error codes can be
 *              found in qapi_ble_plxs.h (QAPI_BLE_PLXS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_PLXS_Notify_Continuous_Measurement(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_PLXS_Continuous_Measurement_Data_t *MeasurementData);

/**
 * @brief
 * Respondis to a write request,
 * received from a PLXS client, for the PLXS
 * RACP characteristic.
 *
 * This function does not indicate the result of the RACP procedure. If
 * the request is accepted, it indicates that the RACP procedure has
 * started. An indication must be sent by the PLXS server once the RACP
 * procedure has completed. The
 * qapi_BLE_PLXS_Indicate_RACP_Response() function must be used to
 * indicate the result of the RACP Procedure.
 *
 * @details
 * This function is primarily provided to allow a way to reject the RACP
 * write request when the RACP Client CCCD
 * has not been configured for indications, the PLXS
 * Client does not have proper authentication, authorization, or
 * encryption to write to the RACP, or an RACP request is already in
 * progress. All other reasons should return
 * QAPI_BLE_PLXS_ERROR_CODE_SUCCESS for the ErrorCode and then call the
 * qapi_BLE_PLXS_Indicate_RACP_Response() to indicate the response once
 * the RACP procedure has completed.
 *
 * The ErrorCode parameter must be a valid value from qapi_ble_plxstypes.h
 * (PLXS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the PLXS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whether the request was
 *                                 accepted or rejected.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. PLXS error codes can be
 *              found in qapi_ble_plxs.h (QAPI_BLE_PLXS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_PLXS_RACP_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode);

/**
 * @brief
 * Indicates the PLXS RACP
 * procedure result to a PLXS client.
 *
 * @details
 * This function sends an indication that provides a guarantee that the
 * PLXS client will receive the value, since the PLXS client must confirm
 * that it has been received.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the RACP
 * CCCD has been
 * previously configured for indications. A PLXS client must have written
 * the PLXS RACP Characteristic's CCCD to enable indications.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the PLXS instance that
 *                                 received the request.
 *
 * @param[in]  ConnectionID        GATT connection ID for the
 *                                 connection with the PLXS client.
 *
 * @param[in]  ResponseData        Pointer to the RACP response data
 *                                 that will be sent in the indication.
 *
 * @return      Positive nonzero if successful (represents the
 *              GATT Transaction ID for the indication).
 *
 * @return      An error code if negative. PLXS error codes can be
 *              found in qapi_ble_plxs.h (QAPI_BLE_PLXS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_PLXS_Indicate_RACP_Response(uint32_t BluetoothStackID,uint32_t InstanceID,uint32_t ConnectionID, qapi_BLE_PLXS_RACP_Response_Data_t *ResponseData);

/**
 * @brief
 * Responds to a read request
 * received from a PLXS client for a PLXS characteristic's CCCD.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * qapi_ble_plxstypes.h (PLXS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * If the request is rejected, the Configuration parameter may be
 * ignored.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the PLXS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whether the request was
 *                                 accepted or rejected.
 *
 * @param[in]  Type                Identifies the CCCD.
 *
 * @param[in]  Configuration       CCCD that
 *                                 will be sent if the request is
 *                                 accepted.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. PLXS error codes can be
 *              found in qapi_ble_plxs.h (QAPI_BLE_PLXS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_PLXS_Read_CCCD_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_PLXS_CCCD_Type_t Type, uint16_t Configuration);

/**
 * @brief
 * Responds to a write request
 * received from a PLXS client, for a PLXS characteristic's CCCD.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * qapi_ble_plxstypes.h (PLXS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the PLXS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates whether the request was
 *                                 accepted or rejected.
 *
 * @param[in]  Type                Identifies the CCCD.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. PLXS error codes can be
 *              found in qapi_ble_plxs.h (QAPI_BLE_PLXS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_PLXS_Write_CCCD_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_PLXS_CCCD_Type_t Type);

   /* PLXS client API.                                                  */

/**
 * @brief
 * Parses a value received in
 * a GATT read response from a remote PLXS server, interpreting it as the
 * PLXS Features Characteristic.
 *
 * @param[in]  ValueLength   Length of the value received from the
 *                           PLXS server.
 *
 * @param[in]  Value         Value received from the
 *                           PLXS server.
 *
 * @param[out]  Features     Pointer that will hold the PLXS Features
 *                           Characteristic if this function is
 *                           successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. PLXS error codes can be
 *              found in qapi_ble_plxs.h (QAPI_BLE_PLXS_ERROR_XXX).
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_PLXS_Decode_Features(uint32_t ValueLength, uint8_t *Value, qapi_BLE_PLXS_Features_Data_t *Features);

/**
 * @brief
 * Parse a value received in
 * a GATT indication from a remote PLXS server, interpreting it as a PLXS
 * Spot Check Measurement.
 *
 * @param[in]  ValueLength   Length of the value received from the
 *                           PLXS server.
 *
 * @param[in]  Value         Value received from the
 *                           PLXS server.
 *
 * @param[out]  MeasurementData   Pointer that will hold the
 *                                PLXS Spot Check Measurement data if
 *                                this function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. PLXS error codes can be
 *              found in qapi_ble_plxs.h (QAPI_BLE_PLXS_ERROR_XXX).
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_PLXS_Decode_Spot_Check_Measurement(uint32_t ValueLength, uint8_t *Value, qapi_BLE_PLXS_Spot_Check_Measurement_Data_t *MeasurementData);

/**
 * @brief
 * Parses a value received in
 * a GATT notification from a remote PLXS server, interpreting it as a
 * PLXS Spot Check Measurement.
 *
 * @param[in]  ValueLength   Length of the value received from the
 *                           PLXS server.
 *
 * @param[in]  Value         Value received from the
 *                           PLXS server.
 *
 * @param[out]  MeasurementData   Pointer that will hold the
 *                                PLXS Spot Check Measurement data if
 *                                this function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. PLXS error codes can be
 *              found in qapi_ble_plxs.h (QAPI_BLE_PLXS_ERROR_XXX).
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_PLXS_Decode_Continuous_Measurement(uint32_t ValueLength,uint8_t *Value, qapi_BLE_PLXS_Continuous_Measurement_Data_t *MeasurementData);

/**
 * @brief
 * Parses a value received in
 * a GATT Indication from a remote PLXS server, interpreting it as the PLXS
 * RACP procedure result.
 *
 * @param[in]  ValueLength   Length of the value received from the
 *                           PLXS server.
 *
 * @param[in]  Value         Value received from the
 *                           PLXS server.
 *
 * @param[out]  ResponseData   Pointer that will hold the
 *                             PLXS RACP
 *                             response data if this function
 *                             is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. PLXS error codes can be
 *              found in qapi_ble_plxs.h (QAPI_BLE_PLXS_ERROR_XXX).
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_PLXS_Decode_RACP_Response(uint32_t ValueLength, uint8_t *Value, qapi_BLE_PLXS_RACP_Response_Data_t *ResponseData);

/**
 *  @}
 */

#endif

