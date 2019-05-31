/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_ble_cgms.h
 *
 * @brief
 * QCA QAPI for Bluetopia Bluetooth Continuous Glucose Monitoring Service
 * (GATT based) API Type Definitions, Constants, and
 * Prototypes.
 *
 * @details
 * The Continuous Glucose Monitoring Service (CGMS) programming interface defines the
 * protocols and procedures to be used to implement the Continuous Glucose Monitoring
 * Service capabilities.
 */

#ifndef __QAPI_BLE_CGMS_H__
#define __QAPI_BLE_CGMS_H__

#include "./qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.          */
#include "./qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.    */
#include "./qapi_ble_gatt.h"      /* qapi GATT prototypes.                    */
#include "./qapi_ble_cgmstypes.h" /* QAPI CGMS prototypes.                    */

/** @addtogroup qapi_ble_services
@{
*/

   /* Error Return Codes.                                               */

   /* Error Codes that are smaller than these (less than -1000) are     */
   /* related to the Bluetooth Protocol Stack itself (see               */
   /* qapi_ble_errros.h).                                               */
#define QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER                    (-1000)
/**< Invalid parameter. */
#define QAPI_BLE_CGMS_ERROR_INVALID_BLUETOOTH_STACK_ID           (-1001)
/**< Invalid Bluetooth Stack ID. */
#define QAPI_BLE_CGMS_ERROR_INSUFFICIENT_RESOURCES               (-1002)
/**< Insufficient resources. */
#define QAPI_BLE_CGMS_ERROR_INSUFFICIENT_BUFFER_SPACE            (-1003)
/**< Insufficient buffer space. */
#define QAPI_BLE_CGMS_ERROR_SERVICE_ALREADY_REGISTERED           (-1004)
/**< Service is already registered. */
#define QAPI_BLE_CGMS_ERROR_INVALID_INSTANCE_ID                  (-1005)
/**< Invalid service instance ID. */
#define QAPI_BLE_CGMS_ERROR_MALFORMATTED_DATA                    (-1006)
/**< Malformatted data. */
#define QAPI_BLE_CGMS_ERROR_INDICATION_OUTSTANDING               (-1007)
/**< Indication is outstanding. */
#define QAPI_BLE_CGMS_ERROR_INVALID_RESPONSE_CODE                (-1008)
/**< Invalid response code. */
#define QAPI_BLE_CGMS_ERROR_UNKNOWN_ERROR                        (-1009)
/**< Unknown error. */
#define QAPI_BLE_CGMS_ERROR_CRC_MISSING                          (-1010)
/**< CRC is missing. */
#define QAPI_BLE_CGMS_ERROR_CRC_INVALID                          (-1011)
/**< CRC is invalid. */
#define QAPI_BLE_CGMS_ERROR_MEASUREMENT_SIZE_INVALID             (-1012)
/**< Measurement size is invalid. */
#define QAPI_BLE_CGMS_ERROR_MEASUREMENT_AND_FLAG_SIZE_NOT_EQUAL  (-1013)
/**< Expected measurement size is invalid based on the Flags field. */
#define QAPI_BLE_CGMS_ERROR_MEASUREMENT_FAILED_TO_DECODE         (-1014)
/**< Measurement failed to decode. */
#define QAPI_BLE_CGMS_ERROR_BUFFER_NOT_EMPTY                     (-1015)
/**< Buffer is not empty. */

   /* This following defines the bit values for whether a E2E-CRC       */
   /* calculation is required (Flags parameter for many API's).  For    */
   /* CGMS Server API's this will indicate whether a CRC needs to be    */
   /* calculated and included with the data being sent.  For CGMS Client*/
   /* API's this will indicate whether the client expects a CRC.        */
   /* * NOTE * The CGMS Client can ignore a CRC sent by the CGMS Server.*/
#define QAPI_BLE_CGMS_E2E_CRC_NOT_SUPPORTED              (0x00)
/**< E2E CRC is not supported. */
#define QAPI_BLE_CGMS_E2E_CRC_SUPPORTED                  (0x01)
/**< E2E CRC is supported. */

   /* The following defines bit values that will be used to indicate    */
   /* whether a CRC is present or valid.  This will allow the CGMS      */
   /* Server and Client to determine if a CRC is present with incoming  */
   /* data and valid for the following situations.                      */
   /* * NOTE * If the Session Start Time Characteristic or the Specific */
   /*          Ops Control Point Characteristic is written, we cannot   */
   /*          internally determine if a CRC is present and valid with  */
   /*          the incoming data from the CGMS Client.  In order to     */
   /*          handle this situation, the CGMS Server will be notified  */
   /*          if a CRC is present and valid by either the generated    */
   /*          QAPI_BLE_ET_CGMS_SERVER_WRITE_SESSION_START_TIME_..      */
   /*          .._REQUEST_E or                                          */
   /*          QAPI_BLE_ET_CGMS_SERVER_SPECIFIC_OPS_CONTROL_POINT_..    */
   /*          .._COMMAND_E CGMS events, in the                         */
   /*          qapi_BLE_CGMS_Event_Callback().  These events will       */
   /*          contain the following structures that use these bit      */
   /*          values: qapi_BLE_CGMS_Write_Session_Start_Time_Data_t    */
   /*          (Flags field), and                                       */
   /*          qapi_BLE_CGMS_Specific_Ops_Control_Point_Command_Data_t  */
   /*          (Flags field).  This way the CGMS Server will be able to */
   /*          determine if a CRC is present and valid, and send the    */
   /*          appropriate response to the CGMS Client.                 */
   /* * NOTE * Since the Flags field of a CGMS Measurement does not     */
   /*          indicate whether a CRC is present as an optional field   */
   /*          (Size indicates this), the CGMS Measurements, when       */
   /*          decoded, will each be marked in the (CRCFlags field) of  */
   /*          the qapi_BLE_CGMS_Measurement_Data_t structure by the bit*/
   /*          values QAPI_BLE_CGMS_E2E_CRC_PRESENT and                 */
   /*          QAPI_BLE_CGMS_E2E_CRC_VALID to indicate if each CGMS     */
   /*          Measurement has a CRC present and if it is valid.  This  */
   /*          way if multiple CGMS Measurements are decoded and some   */
   /*          contain a CRC while others do not, they can still be     */
   /*          decoded.                                                 */
#define QAPI_BLE_CGMS_E2E_CRC_PRESENT                    (0x01)
/**< CRC is present. */
#define QAPI_BLE_CGMS_E2E_CRC_VALID                      (0x02)
/**< CRC is valid. */

/**
 * Structure that contains the attribute handles that will need to be
 * cached by a CGMS Client in order to only do service discovery once.
 */
typedef struct qapi_BLE_CGMS_Client_Information_s
{
   /**
    * CGMS Measurement attribute handle.
    */
   uint16_t CGMS_Measurement;

   /**
    * CGMS Measurement Client Characteristic Configuration
    * Descriptor (CCCD) attribute handle.
    */
   uint16_t CGMS_Measurement_Client_Configuration;

   /**
    * CGMS Feature attribute handle.
    */
   uint16_t CGMS_Feature;

   /**
    * CGMS Status attribute handle.
    */
   uint16_t CGMS_Status;

   /**
    * CGMS Session Start Time attribute handle.
    */
   uint16_t CGMS_Session_Start_Time;

   /**
    * CGMS Session Run Time attribute handle.
    */
   uint16_t CGMS_Session_Run_Time;

   /**
    * CGMS Record Access Control Point attribute handle.
    */
   uint16_t Record_Access_Control_Point;

   /**
    * CGMS Record Access Control Point Client Characteristic
    * Configuration Descriptor (CCCD) attribute handle.
    */
   uint16_t RACP_Client_Configuration;

   /**
    * CGMS Specific Ops Control Point attribute handle.
    */
   uint16_t Specific_Ops_Control_Point;

   /**
    * CGMS Specific Ops Control Point CCCD attribute handle.
    */
   uint16_t SOCP_Client_Configuration;
} qapi_BLE_CGMS_Client_Information_t;

#define QAPI_BLE_CGMS_CLIENT_INFORMATION_DATA_SIZE       (sizeof(qapi_BLE_CGMS_Client_Information_t))
/**<
 * Size of the #qapi_BLE_CGMS_Client_Information_t structure.
 */

/**
 * Structure that contains all of the per-client data that will need to
 * be stored by a CGMS server.
 */
typedef struct qapi_BLE_CGMS_Server_Information_s
{

   /**
    * CGMS Measurement CCCD.
    */
   uint16_t CGMS_Measurement_Client_Configuration;

   /**
    * CGMS Record Access Control Point CCCD.
    */
   uint16_t RACP_Client_Configuration;

   /**
    * CGMS Specific Ops Control Point CCCD.
    */
   uint16_t SOCP_Client_Configuration;
} qapi_BLE_CGMS_Server_Information_t;

#define QAPI_BLE_CGMS_SERVER_INFORMATION_DATA_SIZE       (sizeof(qapi_BLE_CGMS_Server_Information_t))
/**<
 * Size of the #qapi_BLE_CGMS_Server_Information_t structure.
 */

/**
 * Structure represents the CGMS Measurement data.
 */
typedef struct qapi_BLE_CGMS_Measurement_Data_s
{
    /**
    * Size of the CGMS Measurement (including the optional CRC).
    *
    * The Size field does not include itself.
    */
   uint32_t Size;

   /**
    * Bitmask that indicates the optional fields and features that may
    * be included for a CGMS Measurement.
    *
    * Valid values have the form QAPI_BLE_CGMS_MEASUREMENT_FLAG_XXX and
    * can be found in qapi_ble_cgmstypes.h.
    */
   uint32_t Flags;

   /**
    * Glucose concentration.
    */
   uint16_t GlucoseConcentration;

   /**
    * Time offset.
    */
   uint16_t TimeOffset;

   /**
    * CGMS Sensor Status Annunciation (Status).
    */
   uint8_t  SensorStatus;

   /**
    * CGMS Sensor Status Annunciation (Cal/Temp).
    */
   uint8_t  SensorCalTemp;

   /**
    * CGMS Sensor Status Annunciation (Warning).
    */
   uint8_t  SensorWarning;

   /**
    * Trend information.
    */
   uint16_t TrendInformation;

   /**
    * Quality.
    */
   uint16_t Quality;

   /**
    * Bitmask that indicates if a CRC is included with a CGMS
    * Measurement and if it is valid.
    */
   uint8_t  CRCFlags;
} qapi_BLE_CGMS_Measurement_Data_t;

#define QAPI_BLE_CGMS_MEASUREMENT_DATA_SIZE              (sizeof(qapi_BLE_CGMS_Measurement_Data_t))
/**<
 * Size of the #qapi_BLE_CGMS_Measurement_Data_t structure.
 */

/**
 * Structure that represents the CGMS Feature data.
 */
typedef struct qapi_BLE_CGMS_Feature_Data_s
{
   /**
    * Bitmask that indicates the supported features of the CGMS
    * server.
    *
    * Valid values have the from QAPI_BLE_CGMS_FEATURE_FLAG_XXX and can
    * be found in qapi_ble_cgmstypes.h.
    */
   uint32_t Features;

   /**
    * Type and sample location of the CGMS Sensor.
    *
    * The TypeSampleLocation field is a combination of
    * QAPI_BLE_CGMS_FEATURE_TYPE_XXX for the CGMS Feature Type and
    * QAPI_BLE_CGMS_FEATURE_SAMPLE_XXX for the CGMS Sample Locationm
    * which can be found in qapi_ble_cgmstypes.h. The CGMS Feature Type
    * is located at the upper nibble and The CGMS Sample Location is
    * located at the lower nibble.
    */
   uint8_t  TypeSampleLocation;
} qapi_BLE_CGMS_Feature_Data_t;

#define QAPI_BLE_CGMS_FEATURE_DATA_SIZE                  (sizeof(qapi_BLE_CGMS_Feature_Data_t))
/**<
 * Size of the #qapi_BLE_CGMS_Feature_Data_t structure.
 */

/**
 * Structure that represents the CGMS Status data.
 */
typedef struct qapi_BLE_CGMS_Status_Data_s
{
   /**
    * Time offset.
    */
   uint16_t TimeOffset;

   /**
    * Bitmask for the CGMS Status.
    *
    * Valid values have the form
    * QAPI_BLE_CGMS_SENSOR_STATUS_ANNUNCIATION_XXX and can be found in
    * qapi_ble_cgmstypes.h.
    */
   uint32_t Status;
} qapi_BLE_CGMS_Status_Data_t;

#define QAPI_BLE_CGMS_STATUS_DATA_SIZE                   (sizeof(qapi_BLE_CGMS_Status_Data_t))
/**<
 * Size of the #qapi_BLE_CGMS_Status_Data_t structure.
 */

/**
 * Structure that represents the Date/Time data for CGMS.
 */
typedef struct qapi_BLE_CGMS_Date_Time_Data_s
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

   /** Hour.
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
}  qapi_BLE_CGMS_Date_Time_Data_t;

#define QAPI_BLE_CGMS_DATE_TIME_DATA_SIZE                (sizeof(qapi_BLE_CGMS_Date_Time_Data_t))
/**<
 * Size of the #qapi_BLE_CGMS_Date_Time_Data_t structure.
 */

#define QAPI_BLE_CGMS_DATE_TIME_VALID(_x)                ((QAPI_BLE_GATT_DATE_TIME_VALID_YEAR(((_x)).Year)) && (QAPI_BLE_GATT_DATE_TIME_VALID_MONTH(((_x)).Month)) && (QAPI_BLE_GATT_DATE_TIME_VALID_DAY(((_x)).Day)) && (QAPI_BLE_GATT_DATE_TIME_VALID_HOURS(((_x)).Hours)) && (QAPI_BLE_GATT_DATE_TIME_VALID_MINUTES(((_x)).Minutes)) && (QAPI_BLE_GATT_DATE_TIME_VALID_SECONDS(((_x)).Seconds)))
/**<
 * This macro is a utility macro that exists to validate that a specified
 * Date Time is valid. The only parameter to this function is the
 * qapi_BLE_CGMS_Date_Time_Data_t structure to validate. This macro
 * returns TRUE if the Date Time is valid or FALSE otherwise.
 */

/**
 * Enumeration of all the valid values of Time Zone. It will
 * be used as the Time Zone field of CGMS Session Start Time characteristic.
 */
typedef enum
{
   QAPI_BLE_CGMS_TIZ_UTC_MINUS_1200_E = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_MINUS_12_00,
   /**< UTC -1200. */
   QAPI_BLE_CGMS_TIZ_UTC_MINUS_1100_E = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_MINUS_11_00,
   /**< UTC -1100. */
   QAPI_BLE_CGMS_TIZ_UTC_MINUS_1000_E = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_MINUS_10_00,
   /**< UTC -1000. */
   QAPI_BLE_CGMS_TIZ_UTC_MINUS_930_E  = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_MINUS_9_30,
   /**< UTC -930. */
   QAPI_BLE_CGMS_TIZ_UTC_MINUS_900_E  = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_MINUS_9_00,
   /**< UTC -900. */
   QAPI_BLE_CGMS_TIZ_UTC_MINUS_800_E  = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_MINUS_8_00,
   /**< UTC -800. */
   QAPI_BLE_CGMS_TIZ_UTC_MINUS_700_E  = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_MINUS_7_00,
   /**< UTC -700. */
   QAPI_BLE_CGMS_TIZ_UTC_MINUS_600_E  = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_MINUS_6_00,
   /**< UTC -600. */
   QAPI_BLE_CGMS_TIZ_UTC_MINUS_500_E  = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_MINUS_5_00,
   /**< UTC -500. */
   QAPI_BLE_CGMS_TIZ_UTC_MINUS_430_E  = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_MINUS_4_30,
   /**< UTC -430. */
   QAPI_BLE_CGMS_TIZ_UTC_MINUS_400_E  = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_MINUS_4_00,
   /**< UTC -400. */
   QAPI_BLE_CGMS_TIZ_UTC_MINUS_330_E  = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_MINUS_3_30,
   /**< UTC -330. */
   QAPI_BLE_CGMS_TIZ_UTC_MINUS_300_E  = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_MINUS_3_00,
   /**< UTC -300. */
   QAPI_BLE_CGMS_TIZ_UTC_MINUS_200_E  = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_MINUS_2_00,
   /**< UTC -200. */
   QAPI_BLE_CGMS_TIZ_UTC_MINUS_100_E  = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_MINUS_1_00,
   /**< UTC -100. */
   QAPI_BLE_CGMS_TIZ_UTC_PLUS_000_E   = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_PLUS_0_00,
   /**< UTC 0. */
   QAPI_BLE_CGMS_TIZ_UTC_PLUS_100_E   = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_PLUS_1_00,
   /**< UTC 100. */
   QAPI_BLE_CGMS_TIZ_UTC_PLUS_200_E   = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_PLUS_2_00,
   /**< UTC 200. */
   QAPI_BLE_CGMS_TIZ_UTC_PLUS_300_E   = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_PLUS_3_00,
   /**< UTC 300. */
   QAPI_BLE_CGMS_TIZ_UTC_PLUS_330_E   = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_PLUS_3_30,
   /**< UTC 330. */
   QAPI_BLE_CGMS_TIZ_UTC_PLUS_400_E   = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_PLUS_4_00,
   /**< UTC 400. */
   QAPI_BLE_CGMS_TIZ_UTC_PLUS_430_E   = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_PLUS_4_30,
   /**< UTC 430. */
   QAPI_BLE_CGMS_TIZ_UTC_PLUS_500_E   = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_PLUS_5_00,
   /**< UTC 500. */
   QAPI_BLE_CGMS_TIZ_UTC_PLUS_530_E   = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_PLUS_5_30,
   /**< UTC 530. */
   QAPI_BLE_CGMS_TIZ_UTC_PLUS_545_E   = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_PLUS_5_45,
   /**< UTC 545. */
   QAPI_BLE_CGMS_TIZ_UTC_PLUS_600_E   = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_PLUS_6_00,
   /**< UTC 600. */
   QAPI_BLE_CGMS_TIZ_UTC_PLUS_630_E   = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_PLUS_6_30,
   /**< UTC 630. */
   QAPI_BLE_CGMS_TIZ_UTC_PLUS_700_E   = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_PLUS_7_00,
   /**< UTC 700. */
   QAPI_BLE_CGMS_TIZ_UTC_PLUS_800_E   = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_PLUS_8_00,
   /**< UTC 800. */
   QAPI_BLE_CGMS_TIZ_UTC_PLUS_845_E   = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_PLUS_8_45,
   /**< UTC 845. */
   QAPI_BLE_CGMS_TIZ_UTC_PLUS_900_E   = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_PLUS_9_00,
   /**< UTC 900. */
   QAPI_BLE_CGMS_TIZ_UTC_PLUS_930_E   = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_PLUS_9_30,
   /**< UTC 930. */
   QAPI_BLE_CGMS_TIZ_UTC_PLUS_1000_E  = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_PLUS_10_00,
   /**< UTC 1000. */
   QAPI_BLE_CGMS_TIZ_UTC_PLUS_1030_E  = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_PLUS_10_30,
   /**< UTC 1030. */
   QAPI_BLE_CGMS_TIZ_UTC_PLUS_1100_E  = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_PLUS_11_00,
   /**< UTC 1100. */
   QAPI_BLE_CGMS_TIZ_UTC_PLUS_1130_E  = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_PLUS_11_30,
   /**< UTC 1130. */
   QAPI_BLE_CGMS_TIZ_UTC_PLUS_1200_E  = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_PLUS_12_00,
   /**< UTC 1200. */
   QAPI_BLE_CGMS_TIZ_UTC_PLUS_1245_E  = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_PLUS_12_45,
   /**< UTC 1245. */
   QAPI_BLE_CGMS_TIZ_UTC_PLUS_1300_E  = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_PLUS_13_00,
   /**< UTC 1300. */
   QAPI_BLE_CGMS_TIZ_UTC_PLUS_1400_E  = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_PLUS_14_00,
   /**< UTC 1400. */
   QAPI_BLE_CGMS_TIZ_UTC_UNKNOWN_E    = QAPI_BLE_CGMS_TIME_ZONE_UTC_OFFSET_UNKNOWN
   /**< UTC unknown. */
} qapi_BLE_CGMS_Time_Zone_Type_t;

#define QAPI_BLE_CGMS_TIME_ZONE_VALID(_x)                (((_x) >= QAPI_BLE_TIZ_UTC_MINUS_1200_E) && ((_x) <= QAPI_BLE_TIZ_UTC_PLUS_1400_E))
/**<
 * This macro is a utility macro that exists to validate that a specified
 * Time Zone is valid. The only parameter to this function is the Time
 * Zone value to validate. This macro returns TRUE if the Time Zone is
 * valid or FALSE otherwise..
 */

/**
 * Enumeration of all the valid values of DST Offset. It
 * will be used as the DST Offset field of CGMS Session Start Time
 * characteristic.
 */
typedef enum
{
   QAPI_BLE_CGMS_DSO_STANDARD_TIME_E               = QAPI_BLE_CGMS_DST_OFFSET_STANDARD_TIME,
   /**< Daylight Savings Time standard time. */
   QAPI_BLE_CGMS_DSO_HALF_AN_HOUR_DAY_LIGHT_TIME_E = QAPI_BLE_CGMS_DST_OFFSET_HALF_AN_HOUR_DAYLIGHT_TIME,
   /**< Daylight Savings Time Half an Hour Day time. */
   QAPI_BLE_CGMS_DSO_DAY_LIGHT_TIME_E              = QAPI_BLE_CGMS_DST_OFFSET_DAYLIGHT_TIME,
   /**< Daylight Savings Time Day Light time. */
   QAPI_BLE_CGMS_DSO_DOUBLE_DAY_LIGHT_TIME_E       = QAPI_BLE_CGMS_DST_OFFSET_DOUBLE_DAYLIGHT_TIME,
   /**< Daylight Savings Time Double Day Light time. */
   QAPI_BLE_CGMS_DSO_UNKNOWN_E                     = QAPI_BLE_CGMS_DST_OFFSET_UNKNOWN
   /**< Daylight Savings Time Unknown. */
} qapi_BLE_CGMS_DST_Offset_Type_t;

#define QAPI_BLE_CGMS_DST_OFFSET_VALID(_x)               (((_x) >= QAPI_BLE_DSO_STANDARD_TIME_E) && ((_x) <= QAPI_BLE_DSO_DOUBLE_DAY_LIGHT_TIME_E))
/**<
 * This macro is a utility macro that exists to validate that a specified
 * DST Offset is valid. The only parameter to this function is the DST
 * Offset value to validate. This macro returns TRUE if the DST Offset
 * is valid or FALSE otherwise..
 */

/**
 * Structure that represents the CGMS Session Start Time.
 */
typedef struct qapi_BLE_CGMS_Session_Start_Time_Data_s
{
   /**
    * Date/time.
    */
   qapi_BLE_CGMS_Date_Time_Data_t  Time;

   /**
    * Timezone.
    */
   qapi_BLE_CGMS_Time_Zone_Type_t  TimeZone;

   /**
    * Daylight savings time (DST) offset.
    */
   qapi_BLE_CGMS_DST_Offset_Type_t DSTOffset;
} qapi_BLE_CGMS_Session_Start_Time_Data_t;

#define QAPI_BLE_CGMS_SESSION_START_TIME_DATA_SIZE       (sizeof(qapi_BLE_CGMS_Session_Start_Time_Data_t))
/**<
 * Size of the #qapi_BLE_CGMS_Session_Start_Time_Data_t structure.
 */

#define QAPI_BLE_CGMS_SESSION_TIME_VALID(_x)             ((QAPI_BLE_CGMS_DATE_TIME_VALID((_x).Time)) && (QAPI_BLE_CGMS_TIME_ZONE_VALID((_x).TimeZone)) && (QAPI_BLE_CGMS_DST_OFFSET_VALID((_x).DSTOffset)))
/**<
 * This macro is a utility macro that exists to validate that a specified
 * CGMS Session Time is valid. The only parameter to this function is
 * the qapi_BLE_CGMS_Session_Time_Data_t structure to validate. This
 * macro returns TRUE if the CGMS Session Time is valid or FALSE
 * otherwise.
 */

/**
 * Structure that represents the CGMS Session Run Time.
 */
typedef struct qapi_BLE_CGMS_Session_Run_Time_Data_s
{
   /**
    * Session run time.
    */
   uint16_t SessionRunTime;
} qapi_BLE_CGMS_Session_Run_Time_Data_t;

#define QAPI_BLE_CGMS_SESSION_RUN_TIME_DATA_SIZE         (sizeof(qapi_BLE_CGMS_Session_Run_Time_Data_t))
/**<
 * Size of the #qapi_BLE_CGMS_Session_Run_Time_Data_t structure.
 */

/**
 * Enumeration of the RACP Response Type that is passed in the
 * RACP Response structure.
 */
typedef enum
{
   QAPI_BLE_CGMS_NUMBER_OF_STORED_RECORDS_E = QAPI_BLE_CGMS_RACP_OPCODE_NUMBER_OF_STORED_RECORDS_RESPONSE,
   /**< Number of stored records response. */
   QAPI_BLE_CGMS_RESPONSE_CODE_E            = QAPI_BLE_CGMS_RACP_OPCODE_RESPONSE_CODE
   /**< Response code response. */
} qapi_BLE_CGMS_RACP_Response_Type_t;

/**
 * Structure that represents the CGMS Record Access Control Point (RACP)
 * Response Code operand.
 */
typedef struct qapi_BLE_CGMS_RACP_Response_Code_Value_s
{
   /**
    * Request op code.
    */
   uint8_t RequestOpCode;

   /**
    * Response code value.
    */
   uint8_t ResponseCodeValue;
} qapi_BLE_CGMS_RACP_Response_Code_Value_t;

#define QAPI_BLE_CGMS_RACP_RESPONSE_CODE_VALUE_SIZE      (sizeof(qapi_BLE_CGMS_RACP_Response_Code_Value_t))
/**<
 * Size of the #qapi_BLE_CGMS_RACP_Response_Code_Value_t structure.
 */

/**
 * Structure that represents the CGMS RACP Response.
 */
typedef struct qapi_BLE_CGMS_RACP_Response_Data_s
{
   /**
    * Response type.
    */
   qapi_BLE_CGMS_RACP_Response_Type_t ResponseType;
   union
   {
      /**
       * Number of stored records.
       */
      uint16_t                                 NumberOfStoredRecordsResult;

      /**
       * Response code value operand.
       */
      qapi_BLE_CGMS_RACP_Response_Code_Value_t ResponseCodeValue;
   }
   /**
    * Operand that is included depending on the ResponseType field.
    */
   ResponseData;
} qapi_BLE_CGMS_RACP_Response_Data_t;

#define QAPI_BLE_CGMS_RACP_RESPONSE_DATA_SIZE            (sizeof(qapi_BLE_CGMS_RACP_Response_Data_t))
/**<
 * Size of the #qapi_BLE_CGMS_RACP_Response_Data_t structure.
 */

/**
 * Enumeration of the valid Read Request types that a server
 * may receive in a
 * QAPI_BLE_ET_CGMS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E or
 * QAPI_BLE_ET_CGMS_SERVER_CLIENT_CONFIGURATION_UPDATE_E event. This is
 * also used by the qapi_BLE_CGMS_Send_Notification() function to
 * indicate the CGMS characteristic to notify.
 *
 * For each event, it is up to the application to read/write the correct
 * Client Configuration descriptor based on this value.
 */
typedef enum
{
   QAPI_BLE_CGMS_CT_CGMS_MEASUREMENT_E,            /**< CGMS Measurement */
   QAPI_BLE_CGMS_CT_RECORD_ACCESS_CONTROL_POINT_E, /**< CGMS RACP */
   QAPI_BLE_CGMS_CT_SPECIFIC_OPS_CONTROL_POINT_E   /**< CGMS SOCP */
} qapi_BLE_CGMS_Characteristic_Type_t;

/**
 * Enumeration of all the events generated by the CGMS
 * Service. These are used to determine the type of each event
 * generated, and to ensure the proper union element is accessed for the
 * qapi_BLE_CGMS_Event_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_ET_CGMS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E,
   /**< Read CCCD request event. */
   QAPI_BLE_ET_CGMS_SERVER_CLIENT_CONFIGURATION_UPDATE_E,
   /**< Write CCCD request event. */
   QAPI_BLE_ET_CGMS_SERVER_READ_FEATURE_REQUEST_E,
   /**< Read Feature request event. */
   QAPI_BLE_ET_CGMS_SERVER_READ_STATUS_REQUEST_E,
   /**< Read Status request event. */
   QAPI_BLE_ET_CGMS_SERVER_READ_SESSION_START_TIME_REQUEST_E,
   /**< Read Session Start Time request event. */
   QAPI_BLE_ET_CGMS_SERVER_WRITE_SESSION_START_TIME_REQUEST_E,
   /**< Write Session Start Time request event. */
   QAPI_BLE_ET_CGMS_SERVER_READ_SESSION_RUN_TIME_REQUEST_E,
   /**< Read Session Run Time request event. */
   QAPI_BLE_ET_CGMS_SERVER_RECORD_ACCESS_CONTROL_POINT_COMMAND_E,
   /**< Write RACP request event. */
   QAPI_BLE_ET_CGMS_SERVER_SPECIFIC_OPS_CONTROL_POINT_COMMAND_E,
   /**< Write SOCP request event. */
   QAPI_BLE_ET_CGMS_SERVER_CONFIRMATION_DATA_E
} qapi_BLE_CGMS_Event_Type_t;

/**
 * Structure that represents the format for the data that is dispatched
 * to a CGMS server when a CGMS client has sent a request to read a CGMS
 * characteristic's CCCD.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_CGMS_Read_Client_Configuration_Response() function.
 */
typedef struct qapi_BLE_CGMS_Read_Client_Configuration_Data_s
{
   /**
    * CGMS instance that dispatched the event.
    */
   uint32_t                            InstanceID;

   /**
    * GATT connection ID for the connection with the CGMS client
    * that made the request.
    */
   uint32_t                            ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                            TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the CGMS client.
    */
   qapi_BLE_GATT_Connection_Type_t     ConnectionType;

   /**
    * Bluetooth address of the CGMS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                  RemoteDevice;

   /**
    * Identifies the requested CCCD based on the CGMS characteristic
    * type.
    */
   qapi_BLE_CGMS_Characteristic_Type_t ClientConfigurationType;
} qapi_BLE_CGMS_Read_Client_Configuration_Data_t;

#define QAPI_BLE_CGMS_READ_CLIENT_CONFIGURATION_DATA_SIZE  (sizeof(qapi_BLE_CGMS_Read_Client_Configuration_Data_t))
/**<
 * Size of the #qapi_BLE_CGMS_Read_Client_Configuration_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a CGMS server when a CGMS client has sent a request to write a
 * CGMS characteristic's CCCD.
 */
typedef struct qapi_BLE_CGMS_Client_Configuration_Update_Data_s
{
   /**
    * CGMS instance that dispatched the event.
    */
   uint32_t                            InstanceID;

   /**
    * GATT connection ID for the connection with the CGMS client
    * that made the request.
    */
   uint32_t                            ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the CGMS client.
    */
   qapi_BLE_GATT_Connection_Type_t     ConnectionType;

   /**
    * Bluetooth address of the CGMS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                  RemoteDevice;

   /**
    * Identifies the requested CCCD based on the CGMS Characteristic
    * type.
    */
   qapi_BLE_CGMS_Characteristic_Type_t ClientConfigurationType;

   /**
    * CCCD value that has been requested to be written.
    */
   uint16_t                            ClientConfiguration;
} qapi_BLE_CGMS_Client_Configuration_Update_Data_t;

#define QAPI_BLE_CGMS_CLIENT_CONFIGURATION_UPDATE_DATA_SIZE  (sizeof(qapi_BLE_CGMS_Client_Configuration_Update_Data_t))
/**<
 * Size of the #qapi_BLE_CGMS_Client_Configuration_Update_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a CGMS server when a CGMS client has sent a request to read the
 * CGMS Feature Characteristic.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_CGMS_Feature_Read_Request_Response() function.
 */
typedef struct qapi_BLE_CGMS_Read_Feature_Data_s
{
   /**
    * CGMS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the CGMS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the CGMS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * Bluetooth address of the CGMS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;
} qapi_BLE_CGMS_Read_Feature_Data_t;

#define QAPI_BLE_CGMS_READ_FEATURE_DATA_SIZE             (sizeof(qapi_BLE_CGMS_Read_Feature_Data_t))
/**<
 * Size of the #qapi_BLE_CGMS_Read_Feature_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a CGMS server when a CGMS client has sent a request to read the
 * CGMS Status Characteristic.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_CGMS_Status_Read_Request_Response() function.
 */
typedef struct qapi_BLE_CGMS_Read_Status_Data_s
{
   /**
    * CGMS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the CGMS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the CGMS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * Bluetooth address of the CGMS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;
} qapi_BLE_CGMS_Read_Status_Data_t;

#define QAPI_BLE_CGMS_READ_STATUS_DATA_SIZE              (sizeof(qapi_BLE_CGMS_Read_Status_Data_t))
/**<
 * Size of the #qapi_BLE_CGMS_Read_Status_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a CGMS server when a CGMS client has sent a request to read the
 * CGMS Session Start Time Characteristic.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_CGMS_Session_Start_Time_Read_Request_Response() function.
 */
typedef struct qapi_BLE_CGMS_Read_Session_Start_Time_Data_s
{
   /**
    * CGMS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the CGMS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the CGMS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * Bluetooth address of the CGMS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;
} qapi_BLE_CGMS_Read_Session_Start_Time_Data_t;

#define QAPI_BLE_CGMS_READ_SESSION_START_TIME_DATA_SIZE  (sizeof(qapi_BLE_CGMS_Read_Session_Start_Time_Data_t))
/**<
 * Size of the #qapi_BLE_CGMS_Read_Session_Start_Time_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a CGMS server when a CGMS client has sent a request to write the
 * CGMS Session Start Time Characteristic.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_CGMS_Session_Start_Time_Write_Request_Response() function.
 */
typedef struct qapi_BLE_CGMS_Write_Session_Start_Time_Data_s
{
   /**
    * CGMS instance that dispatched the event.
    */
   uint32_t                                InstanceID;

   /**
    * GATT connection ID for the connection with the CGMS client
    * that made the request.
    */
   uint32_t                                ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                                TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the CGMS client.
    */
   qapi_BLE_GATT_Connection_Type_t         ConnectionType;

   /**
    * Bluetooth address of the CGMS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                      RemoteDevice;

   /**
    * Bitmask that indicates if a CRC is included and valid.
    */
   uint8_t                                 Flags;

   /**
    * CGMS Session Start Time data that has been requested to be
    * written.
    */
   qapi_BLE_CGMS_Session_Start_Time_Data_t SessionStartTime;
} qapi_BLE_CGMS_Write_Session_Start_Time_Data_t;

#define QAPI_BLE_CGMS_WRITE_SESSION_START_TIME_DATA_SIZE  (sizeof(qapi_BLE_CGMS_Write_Session_Start_Time_Data_t))
/**<
 * Size of the #qapi_BLE_CGMS_Write_Session_Start_Time_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a CGMS server when a CGMS client has sent a request to read the
 * CGMS Session Run Time Characteristic.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_CGMS_Session_Run_Time_Read_Request_Response() function.
 */
typedef struct qapi_BLE_CGMS_Read_Session_Run_Time_Data_s
{
   /**
    * CGMS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the CGMS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the CGMS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * Bluetooth address of the CGMS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;
} qapi_BLE_CGMS_Read_Session_Run_Time_Data_t;

#define QAPI_BLE_CGMS_READ_SESSION_RUN_TIME_DATA_SIZE    (sizeof(qapi_BLE_CGMS_Read_Session_Run_Time_Data_t))
/**<
 * Size of the #qapi_BLE_CGMS_Read_Session_Run_Time_Data_t structure.
 */

/**
 * Enumeration of the valid values that may be set as the
 * value for the OpCode field of the Record Access Control Point (RACP)
 * characteristic.
 */
typedef enum
{
   QAPI_BLE_CGMS_COMMAND_REPORT_STORED_RECORDS_REQUEST_E    = QAPI_BLE_CGMS_RACP_OPCODE_REPORT_STORED_RECORDS,
   /**< Report Stored Records Op Code. */
   QAPI_BLE_CGMS_COMMAND_DELETE_STORED_RECORDS_REQUEST_E    = QAPI_BLE_CGMS_RACP_OPCODE_DELETE_STORED_RECORDS,
   /**< Delete Stored Records Op Code. */
   QAPI_BLE_CGMS_COMMAND_ABORT_OPERATION_REQUEST_E          = QAPI_BLE_CGMS_RACP_OPCODE_ABORT_OPERATION,
   /**< Abort Operation Op Code. */
   QAPI_BLE_CGMS_COMMAND_NUMBER_OF_STORED_RECORDS_REQUEST_E = QAPI_BLE_CGMS_RACP_OPCODE_REPORT_NUMBER_OF_STORED_RECORDS
   /**< Report Number of Stored Records Op Code. */
} qapi_BLE_CGMS_RACP_Command_Type_t;

/**
 * Enumeration of the valid values that may be set as the
 * value for the Operator field of the RACP
 * characteristic.
 */
typedef enum
{
   QAPI_BLE_CGMS_NULL_E                     = QAPI_BLE_CGMS_RACP_OPERATOR_NULL,
   /**< NULL operator. */
   QAPI_BLE_CGMS_ALL_RECORDS_E              = QAPI_BLE_CGMS_RACP_OPERATOR_ALL_RECORDS,
   /**< All records operator. */
   QAPI_BLE_CGMS_LESS_THAN_OR_EQUAL_TO_E    = QAPI_BLE_CGMS_RACP_OPERATOR_LESS_THAN_OR_EQUAL_TO,
   /**< Less than or equal to operator. */
   QAPI_BLE_CGMS_GREATER_THAN_OR_EQUAL_TO_E = QAPI_BLE_CGMS_RACP_OPERATOR_GREATER_THAN_OR_EQUAL_TO,
   /**< Greater than or equal to operator. */
   QAPI_BLE_CGMS_WITHIN_RANGE_OF_E          = QAPI_BLE_CGMS_RACP_OPERATOR_WITHIN_RANGE_OF,
   /**< Within range of operator. */
   QAPI_BLE_CGMS_FIRST_RECORD_E             = QAPI_BLE_CGMS_RACP_OPERATOR_FIRST_RECORD,
   /**< First record operator. */
   QAPI_BLE_CGMS_LAST_RECORD_E              = QAPI_BLE_CGMS_RACP_OPERATOR_LAST_RECORD
   /**< Last record operator. */
} qapi_BLE_CGMS_RACP_Operator_Type_t;

/**
 * Enumeration of the valid values that may be used as the
 * Filter Type values of a RACP
 * characteristic.
 */
typedef enum
{
   QAPI_BLE_CGMS_FILTER_TIME_OFFSET_E = QAPI_BLE_CGMS_RACP_FILTER_TYPE_TIME_OFFSET,
} qapi_BLE_CGMS_RACP_Filter_Type_t;

/**
 * Structure that represents the time offset range.
 */
typedef struct qapi_BLE_CGMS_Time_Offset_Range_Data_s
{
   /**
    * Minimum time offset.
    */
   uint16_t Minimum;

   /**
    * Maximum time offset.
    */
   uint16_t Maximum;
} qapi_BLE_CGMS_Time_Offset_Range_Data_t;

#define QAPI_BLE_CGMS_TIME_OFFSET_RANGE_DATA_SIZE        (sizeof(qapi_BLE_CGMS_Time_Offset_Range_Data_t))
/**<
 * Size of the #qapi_BLE_CGMS_Time_Offset_Range_Data_t structure.
 */

/**
 * Structure that represents the CGMS RACP request
 * data.
 */
typedef struct qapi_BLE_CGMS_RACP_Format_Data_s
{
   /**
    * Command (request) type.
    */
   uint8_t CommandType;

   /**
    * Operator type.
    */
   uint8_t OperatorType;

   /**
    * Filter type.
    */
   uint8_t FilterType;
   union
   {
      /**
       * Time offset operand.
       */
      uint16_t                               TimeOffset;

      /**
       * Time offset range operand.
       */
      qapi_BLE_CGMS_Time_Offset_Range_Data_t TimeOffsetRange;
   }
   /**
    * Oerand that is required depending on the CommandType field.
    */
   FilterParameters;
} qapi_BLE_CGMS_RACP_Format_Data_t;

#define QAPI_BLE_CGMS_RACP_FORMAT_DATA_SIZE              (sizeof(qapi_BLE_CGMS_RACP_Format_Data_t))
/**<
 * Size of the #qapi_BLE_CGMS_RACP_Format_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a CGMS server when a CGMS client has sent a request to write the
 * RACP characteristic.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_CGMS_Record_Access_Control_Point_Response() function.
 */
typedef struct qapi_BLE_CGMS_RACP_Command_Data_s
{
   /**
    * CGMS instance that dispatched the event.
    */
   uint32_t                         InstanceID;

   /**
    * GATT connection ID for the connection with the CGMS client
    * that made the request.
    */
   uint32_t                         ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                         TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the CGMS client.
    */
   qapi_BLE_GATT_Connection_Type_t  ConnectionType;

   /**
    * Bluetooth address of the CGMS client that made the request.
    */
   qapi_BLE_BD_ADDR_t               RemoteDevice;

   /**
    * CGMS RACP data that has been
    * requested to be written.
    */
   qapi_BLE_CGMS_RACP_Format_Data_t FormatData;
} qapi_BLE_CGMS_Record_Access_Control_Point_Command_Data_t;

#define QAPI_BLE_CGMS_RACP_COMMAND_DATA_SIZE             (sizeof(qapi_BLE_CGMS_Record_Access_Control_Point_Command_Data_t))
/**<
 * Size of the #qapi_BLE_CGMS_Record_Access_Control_Point_Command_Data_t
 * structure.
 */

   /* The following defines the values that may be set as the value of  */
   /* CGMS Communication Interval field in CGMS Specific Operation      */
   /* Control Point Characteristic.                                     */
#define QAPI_BLE_CGMS_COMMUNICATION_INTERVAL_VALUE_DISABLE_PERIODIC_COMMUNICATION          0x00
/**< Disables periodic communications. */
#define QAPI_BLE_CGMS_COMMUNICATION_INTERVAL_SMALLEST_INTERVAL_VALUE                       0xFF
/**< Lowest communication interval value. */

   /* The following defines the values that may be set as the value of  */
   /* Calibration Data Record Number field during the Get Glucose       */
   /* Calibration procedure in CGMS Specific Ops Control Point          */
   /* Characteristic.                                                   */
#define QAPI_BLE_CGMS_GLUCOSE_CALIBRATION_DATA_RECORD_NUMBER_NO_STORED_CALIBRATION_DATA    0x0000
/**< No stored calibration data. */
#define QAPI_BLE_CGMS_GLUCOSE_CALIBRATION_DATA_RECORD_NUMBER_LAST_STORED_CALIBRATION_DATA  0xFFFF
/**< Last calibration data record number. */

/**
 * Enumeration of the valid values that may be set as the
 * value for the OpCode field of CGMS Specific Ops Control Point
 * characteristic.
 */
typedef enum
{
   QAPI_BLE_CGMS_COMMAND_SET_CGM_COMMUNICATION_INTERVAL_E   = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_SET_CGM_COMMUNICATION_INTERVAL,
   /**< Set communication interval op code. */
   QAPI_BLE_CGMS_COMMAND_GET_CGM_COMMUNICATION_INTERVAL_E   = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_GET_CGM_COMMUNICATION_INTERVAL,
   /**< Get communication interval op code. */
   QAPI_BLE_CGMS_COMMAND_SET_GLUCOSE_CALIBRATION_VALUE_E    = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_SET_GLUCOSE_CALIBRATION_VALUE,
   /**< Set glucose calibration op code. */
   QAPI_BLE_CGMS_COMMAND_GET_GLUCOSE_CALIBRATION_VALUE_E    = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_GET_GLUCOSE_CALIBRATION_VALUE,
   /**< Get glucose calibration op code. */
   QAPI_BLE_CGMS_COMMAND_SET_PATIENT_HIGH_ALERT_LEVEL_E     = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_SET_PATIENT_HIGH_ALERT_LEVEL,
   /**< Set patient high alert level op code. */
   QAPI_BLE_CGMS_COMMAND_GET_PATIENT_HIGH_ALERT_LEVEL_E     = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_GET_PATIENT_HIGH_ALERT_LEVEL,
   /**< Get patient high alert level op code. */
   QAPI_BLE_CGMS_COMMAND_SET_PATIENT_LOW_ALERT_LEVEL_E      = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_SET_PATIENT_LOW_ALERT_LEVEL,
   /**< Set patient low alert level op code. */
   QAPI_BLE_CGMS_COMMAND_GET_PATIENT_LOW_ALERT_LEVEL_E      = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_GET_PATIENT_LOW_ALERT_LEVEL,
   /**< Get patient low alert level op code. */
   QAPI_BLE_CGMS_COMMAND_SET_HYPO_ALERT_LEVEL_E             = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_SET_HYPO_ALERT_LEVEL,
   /**< Set hypo alert level op code. */
   QAPI_BLE_CGMS_COMMAND_GET_HYPO_ALERT_LEVEL_E             = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_GET_HYPO_ALERT_LEVEL,
   /**< Get hypo alert level op code. */
   QAPI_BLE_CGMS_COMMAND_SET_HYPER_ALERT_LEVEL_E            = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_SET_HYPER_ALERT_LEVEL,
   /**< Set hyper alert level op code. */
   QAPI_BLE_CGMS_COMMAND_GET_HYPER_ALERT_LEVEL_E            = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_GET_HYPER_ALERT_LEVEL,
   /**< Get hyper alert level op code. */
   QAPI_BLE_CGMS_COMMAND_SET_RATE_OF_DECREASE_ALERT_LEVEL_E = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_SET_RATE_OF_DECREASE_ALERT_LEVEL,
   /**< Set rate of decrease alert level op code. */
   QAPI_BLE_CGMS_COMMAND_GET_RATE_OF_DECREASE_ALERT_LEVEL_E = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_GET_RATE_OF_DECREASE_ALERT_LEVEL,
   /**< Get rate of decrease alert level op code. */
   QAPI_BLE_CGMS_COMMAND_SET_RATE_OF_INCREASE_ALERT_LEVEL_E = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_SET_RATE_OF_INCREASE_ALERT_LEVEL,
   /**< Set rate of increase alert level op code. */
   QAPI_BLE_CGMS_COMMAND_GET_RATE_OF_INCREASE_ALERT_LEVEL_E = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_GET_RATE_OF_INCREASE_ALERT_LEVEL,
   /**< Get rate of increase alert level op code. */
   QAPI_BLE_CGMS_COMMAND_RESET_DEVICE_SPECIFIC_ALERT_E      = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_RESET_DEVICE_SPECIFIC_ALERT,
   /**< Reset device specific alert op code. */
   QAPI_BLE_CGMS_COMMAND_START_SESSION_E                    = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_START_SESSION,
   /**< Start session op code. */
   QAPI_BLE_CGMS_COMMAND_STOP_SESSION_E                     = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_STOP_SESSION
   /**< Stop session op code. */
} qapi_BLE_CGMS_SOCP_Command_Type_t;

/**
 * Enumeration of the valid values that may be set as the
 * value for the Response Opcode field of CGMS Specific Ops Control Point
 * Characteristic.
 */
typedef enum
{
   QAPI_BLE_CGMS_RESPONSE_COMMUNICATION_INTERVAL_RESPONSE_E       = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_COMMUNICATION_INTERVAL_RESPONSE,
   /**< Communication interval response op code. */
   QAPI_BLE_CGMS_RESPONSE_CALIBRATION_VALUE_RESPONSE_E            = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_CALIBRATION_VALUE_RESPONSE,
   /**< Calibration value response op code. */
   QAPI_BLE_CGMS_RESPONSE_PATIENT_HIGH_ALERT_LEVEL_RESPONSE_E     = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_PATIENT_HIGH_ALERT_LEVEL_RESPONSE,
   /**< Patient high alert level response op code. */
   QAPI_BLE_CGMS_RESPONSE_PATIENT_LOW_ALERT_LEVEL_RESPONSE_E      = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_PATIENT_LOW_ALERT_LEVEL_RESPONSE,
   /**< Patient low alert level response op code. */
   QAPI_BLE_CGMS_RESPONSE_HYPO_ALERT_LEVEL_RESPONSE_E             = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_HYPO_ALERT_LEVEL_RESPONSE,
   /**< Hypo alert level response op code. */
   QAPI_BLE_CGMS_RESPONSE_HYPER_ALERT_LEVEL_RESPONSE_E            = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_HYPER_ALERT_LEVEL_RESPONSE,
   /**< Hyper alert level response op code. */
   QAPI_BLE_CGMS_RESPONSE_RATE_OF_DECREASE_ALERT_LEVEL_RESPONSE_E = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_RATE_OF_DECREASE_ALERT_LEVEL_RESPONSE,
   /**< Rate of decrease alert level response op code. */
   QAPI_BLE_CGMS_RESPONSE_RATE_OF_INCREASE_ALERT_LEVEL_RESPONSE_E = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_RATE_OF_INCREASE_ALERT_LEVEL_RESPONSE,
   /**< Rate of increase alert level response op code. */
   QAPI_BLE_CGMS_RESPONSE_RESPONSE_E                              = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_RESPONSE
   /**< Response code op code. */
} qapi_BLE_CGMS_SOCP_Response_Type_t;

/**
 * Enumeration of the valid values that may be set as the
 * value for the Response Opcode field of CGMS Specific Ops Control Point
 * Characteristic.
 */
typedef enum
{
   QAPI_BLE_CGMS_RESPONSE_VALUE_SUCCESS_E                  = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_RESPONSE_OPCODE_SUCCESS,
   /**< Response code success. */
   QAPI_BLE_CGMS_RESPONSE_VALUE_OPCODE_NOT_SUPPORTED_E     = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_RESPONSE_OPCODE_NOT_SUPPORTED,
   /**< Response code op code is not supported. */
   QAPI_BLE_CGMS_RESPONSE_VALUE_INVALID_OPERAND_E          = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_RESPONSE_INVALID_OPERAND,
   /**< Response code invalid operand. */
   QAPI_BLE_CGMS_RESPONSE_VALUE_PROCEDURE_NOT_COMPLETED_E  = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_RESPONSE_OPCODE_PROCEDURE_NOT_COMPLETED,
   /**< Response code procedure is not completed. */
   QAPI_BLE_CGMS_RESPONSE_VALUE_PARAMETER_OUT_OF_RANGE_E   = QAPI_BLE_CGMS_SPECIFIC_OPS_CP_RESPONSE_OPCODE_PARAMETER_OUT_OF_RANGE
   /**< Response code parameter is out of range. */
} qapi_BLE_CGMS_SOCP_Response_Value_t;

/**
 * Structure that represents the CGMS Specific Ops Control Point (SOCP)
 * Response Code operand.
 */
typedef struct qapi_BLE_CGMS_SOCP_Response_Code_Value_s
{
   /**
    * Request op code.
    */
   uint8_t RequestOpCode;

   /**
    * Response code value.
    */
   uint8_t ResponseCodeValue;
} qapi_BLE_CGMS_SOCP_Response_Code_Value_t;

#define QAPI_BLE_CGMS_SOCP_RESPONSE_CODE_VALUE_SIZE      (sizeof(qapi_BLE_CGMS_SOCP_Response_Code_Value_t))
/**<
 * Size of the #qapi_BLE_CGMS_SOCP_Response_Code_Value_t structure.
 */

/**
 * Structure that represents a CGMS Calibration Data Record.
 */
typedef struct qapi_BLE_CGMS_Calibration_Data_Record_s
{
   /**
    * Glucose concentration.
    */
   uint16_t CalibrationGlucoseConcentration;

   /**
    * Calibration time offset.
    */
   uint16_t CalibrationTime;

   /**
    * Type sample location.
    */
   uint8_t  CalibrationTypeSampleLocation;

   /**
    * Next calibration time offset.
    */
   uint16_t NextCalibrationTime;

   /**
    * Calibration record number.
    */
   uint16_t CalibrationDataRecordNumber;

   /**
    * Calibration status.
    */
   uint8_t  CalibrationStatus;
} qapi_BLE_CGMS_Calibration_Data_Record_t;

#define QAPI_BLE_CGMS_CALIBRATION_DATA_RECORD_SIZE       (sizeof(qapi_BLE_CGMS_Calibration_Data_Record_t))
/**<
 * Size of the #qapi_BLE_CGMS_Calibration_Data_Record_t structure.
 */

/**
 * Structure that represents the CGMS SOCP request data.
 */
typedef struct qapi_BLE_CGMS_Specific_Ops_Control_Point_Format_Data_s
{
   /**
    * Request type.
    */
   qapi_BLE_CGMS_SOCP_Command_Type_t CommandType;
   union
   {
      /**
       * Communication interval operand.
       */
      uint8_t                                 CommunicationIntervalMinutes;

      /**
       * Calibration data record operand.
       */
      qapi_BLE_CGMS_Calibration_Data_Record_t CalibrationDataRecord;

      /**
       * Calibration data record number operand.
       */
      uint16_t                                CalibrationDataRecordNumber;

      /**
       * Alert level operand. @newpagetable
       */
      uint16_t                                AlertLevel;
   }
   /**
    * Operand that may be included based on the CommandType field.
    */
   CommandParameters;
} qapi_BLE_CGMS_Specific_Ops_Control_Point_Format_Data_t;

#define QAPI_BLE_CGMS_SPECIFIC_OPS_CONTROL_POINT_FORMAT_DATA_SIZE  (sizeof(qapi_BLE_CGMS_Specific_Ops_Control_Point_Format_Data_t))
/**<
 * Size of the #qapi_BLE_CGMS_Specific_Ops_Control_Point_Format_Data_t
 * structure.
 */

/**
 * Structure that represents the CGMS SOCP response data.
 */
typedef struct qapi_BLE_CGMS_Specific_Ops_Control_Point_Response_Data_s
{
   /**
    * Response type.
    */
   qapi_BLE_CGMS_SOCP_Response_Type_t ResponseType;
   union
   {
      /**
       * Communication interval operand.
       */
      uint8_t                                  CommunicationIntervalMinutes;

      /**
       * Calibration data record operand.
       */
      qapi_BLE_CGMS_Calibration_Data_Record_t  CalibrationDataRecord;

      /**
       * Alert level operand.
       */
      uint16_t                                 AlertLevel;

      /**
       * Response code operand.
       */
      qapi_BLE_CGMS_SOCP_Response_Code_Value_t ResponseCodeValue;
   }
   /**
    * Operand that may be included based on the ResponseType field.
    */
   ResponseData;
} qapi_BLE_CGMS_Specific_Ops_Control_Point_Response_Data_t;

#define QAPI_BLE_CGMS_SPECIFIC_OPS_CONTROL_POINT_RESPONSE_DATA_SIZE  (sizeof(qapi_BLE_CGMS_Specific_Ops_Control_Point_Response_Data_t))
/**<
 * Size of the #qapi_BLE_CGMS_Specific_Ops_Control_Point_Response_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a CGMS server when a CGMS client has sent a request to write the
 * SOCP characteristic.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_CGMS_Specific_Ops_Control_Point_Response() function.
 */
typedef struct qapi_BLE_CGMS_Specific_Ops_Control_Point_Command_Data_s
{
   /**
    * CGMS instance that dispatched the event.
    */
   uint32_t                                               InstanceID;

   /**
    * GATT connection ID for the connection with the CGMS client
    * that made the request.
    */
   uint32_t                                               ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                                               TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the CGMS client.
    */
   qapi_BLE_GATT_Connection_Type_t                        ConnectionType;

   /**
    * Bluetooth address of the CGMS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                                     RemoteDevice;

   /**
    * Bitmask that indicates if a CRC is included and valid.
    */
   uint8_t                                                Flags;

   /**
    * CGMS SOCP data that has been requested to be written.
    */
   qapi_BLE_CGMS_Specific_Ops_Control_Point_Format_Data_t FormatData;
} qapi_BLE_CGMS_Specific_Ops_Control_Point_Command_Data_t;

#define QAPI_BLE_CGMS_SPECIFIC_OPS_CONTROL_POINT_COMMAND_DATA_SIZE  (sizeof(qapi_BLE_CGMS_Specific_Ops_Control_Point_Command_Data_t))
/**<
 * Size of the #qapi_BLE_CGMS_Specific_Ops_Control_Point_Command_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a CGMS server when a CGMS client has responded to an outstanding
 * indication that was previously sent by the CGMS server.
 */
typedef struct qapi_BLE_CGMS_Confirmation_Data_s
{
   /**
    * CGMS instance that dispatched the event.
    */
   uint32_t                            InstanceID;

   /**
    * GATT connection ID for the connection with the CGMS client
    * that made the request.
    */
   uint32_t                            ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the CGMS client.
    */
   qapi_BLE_GATT_Connection_Type_t     ConnectionType;

   /**
    * Bluetooth address of the CGMS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                  RemoteDevice;

   /**
    * Identifies the CGMS Characteristic that was previously
    * indicated.
    */
   qapi_BLE_CGMS_Characteristic_Type_t CharacteristicType;

   /**
    * Status of the indication.
    */
   uint8_t                             Status;
} qapi_BLE_CGMS_Confirmation_Data_t;

#define QAPI_BLE_CGMS_CONFIRMATION_DATA_SIZE             (sizeof(qapi_BLE_CGMS_Confirmation_Data_t))
/**<
 * Size of the #qapi_BLE_CGMS_Confirmation_Data_t structure.
 */

/**
 * Structure that represents the container structure for holding all the
 * event data for a CGMS instance.
 */
typedef struct qapi_BLE_CGMS_Event_Data_s
{
   /**
    * Event type used to determine the appropriate union member of
    * the Event_Data field to access.
    */
   qapi_BLE_CGMS_Event_Type_t Event_Data_Type;

   /**
    * Total size of the data contained in the event.
    */
   uint16_t                   Event_Data_Size;
   union
   {
      /**
       * CGMS Read CCCD event data.
       */
      qapi_BLE_CGMS_Read_Client_Configuration_Data_t           *CGMS_Read_Client_Configuration_Data;

      /**
       * CGMS Write CCCD event data.
       */
      qapi_BLE_CGMS_Client_Configuration_Update_Data_t         *CGMS_Client_Configuration_Update_Data;

      /**
       * CGMS Read Feature event data.
       */
      qapi_BLE_CGMS_Read_Feature_Data_t                        *CGMS_Read_Feature_Data;

      /**
       * CGMS Read Status event data.
       */
      qapi_BLE_CGMS_Read_Status_Data_t                         *CGMS_Read_Status_Data;

      /**
       * CGMS Read Session Start Time event data.
       */
      qapi_BLE_CGMS_Read_Session_Start_Time_Data_t             *CGMS_Read_Session_Start_Time_Data;

      /**
       * CGMS Write Session Start Time event data.
       */
      qapi_BLE_CGMS_Write_Session_Start_Time_Data_t            *CGMS_Write_Session_Start_Time_Data;

      /**
       * CGMS Read Session Run Time event data.
       */
      qapi_BLE_CGMS_Read_Session_Run_Time_Data_t               *CGMS_Read_Session_Run_Time_Data;

      /**
       * CGMS Record Access Control Point (RACP) event data.
       */
      qapi_BLE_CGMS_Record_Access_Control_Point_Command_Data_t *CGMS_Record_Access_Control_Point_Command_Data;

      /**
       * CGMS SOCP event data.
       */
      qapi_BLE_CGMS_Specific_Ops_Control_Point_Command_Data_t  *CGMS_Specific_Ops_Control_Point_Command_Data;

      /**
       * CGMS Confirmation event data.
       */
      qapi_BLE_CGMS_Confirmation_Data_t                        *CGMS_Confirmation_Data;
   }
   /**
    * The event data.
    */
   Event_Data;
} qapi_BLE_CGMS_Event_Data_t;

#define QAPI_BLE_CGMS_EVENT_DATA_SIZE                    (sizeof(qapi_BLE_CGMS_Event_Data_t))
/**<
 * Size of the #qapi_BLE_CGMS_Event_Data_t structure.
 */

/**
 * @brief
 * This declared type represents the prototype function for
 * a CGMS Service Event Receive Data Callback. This function will be called
 * whenever a CGMS Service Event occurs that is associated with the
 * specified Bluetooth stack ID.

 * @details
 * The caller should use the contents of the CGMS Profile Event Data
 * only in the context of this callback. If the caller requires
 * the data for a longer period of time, the callback function
 * must copy the data into another data buffer.
 *
 * This function is guaranteed not to be invoked more than once
 * simultaneously for the specified installed callback (i.e., this
 * function does not have be reentrant). It should noted, however,
 * that if the same callback is installed more than once, the
 * callbacks will be called serially. Because of this, the processing
 * in this function should be as efficient as possible.
 *
 * It should also be noted that this function is called in the Thread
 * Context of a Thread that the user does not own. Therefore, processing
 * in this function should be as efficient as possible (this argument holds
 * anyway because another CGMS Service Event will not be processed while this
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
 * @param[in]  CGMS_Event_Data       Pointer to a structure that contains
 *                                   information about the event that has
 *                                   occurred.
 *
 * @param[in]  CallbackParameter     User-defined value to was supplied
 *                                   as an input parameter from a prior
 *                                   CGMS request.
 *
 * @return None.
 */
typedef void (QAPI_BLE_BTPSAPI *qapi_BLE_CGMS_Event_Callback_t)(uint32_t BluetoothStackID, qapi_BLE_CGMS_Event_Data_t *CGMS_Event_Data, uint32_t CallbackParameter);

   /* CGMS Server API.                                                  */

/**
 * @brief
 * Initializes a CGMS server on a specified Bluetooth Stack.
 *
 * @details
 * Only one CGMS server may be open at a time, per the Bluetooth Stack ID.
 *
 * @param[in]  BluetoothStackID     Unique identifier assigned to this
 *                                  Bluetooth Protocol Stack via a
 *                                  call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  Flags                Flags that are used to register CGMS
 *                                  for the specified transport. These
 *                                  flags can be found in
 *                                  qapi_ble_cgmstypes.h and have the
 *                                  form QAPI_BLE_CGMS_SERVICE_FLAGS_XXX.
 *
 * @param[in]  EventCallback        CGMS Event Callback that will
 *                                  receive CGMS server events.
 *
 * @param[in]  CallbackParameter    User-defined value that will be
 *                                  received with the specified
 *                                  EventCallback parameter.
 *
 * @param[out]  ServiceID           Unique GATT service ID of the
 *                                  registered CGMS service returned from
 *                                  qapi_BLE_GATT_Register_Service() API.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of CGMS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CGMS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BTGATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Initialize_Service(uint32_t BluetoothStackID, uint32_t Flags, qapi_BLE_CGMS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

/**
 * @brief
 * Initializes an CGMS server on a specified Bluetooth Stack.
 *
 * @details
 * Unlike qapi_BLE_CGMS_Initialize_Service(), this function allows the
 * application to select a handle range in GATT to store the service.
 * Only one CGMS server may be open at a time, per the Bluetooth Stack ID.
 *
 * @param[in]  BluetoothStackID     Unique identifier assigned to this
 *                                  Bluetooth Protocol Stack via a
 *                                  call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  Flags                Flags that are used to register CGMS
 *                                  for the specified transport. These
 *                                  flags can be found in
 *                                  qapi_ble_cgmstypes.h and have the
 *                                  form QAPI_BLE_CGMS_SERVICE_FLAGS_XXX.
 *
 * @param[in]  EventCallback        CGMS Event Callback that will
 *                                  receive CGMS server events.
 *
 * @param[in]  CallbackParameter    User-defined value that will be
 *                                  received with the specified
 *                                  EventCallback parameter.
 *
 * @param[out]  ServiceID           Unique GATT Service ID of the
 *                                  registered CGMS service returned from
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
 *              be the Service Instance ID of CGMS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CGMS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BTGATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, uint32_t Flags, qapi_BLE_CGMS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

/**
 * @brief
 * Cleans up and frees all resources
 * associated with a CGMS Instance.
 *
 * @details
 * After this function is called, no other CGMS
 * function can be called until after a successful call to either of the
 * qapi_BLE_CGMS_Initialize_XXX() functions are performed.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of the qapi_BLE_CGMS_Initialize_XXX()
 *                                 functions.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CGMS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * Queries the number of attributes
 * that are contained in the CGMS Service that is registered with a
 * call to qapi_BLE_CGMS_Initialize_Service() or
 * qapi_BLE_CGMS_Initialize_Service_Handle_Range().
 *
 * @return   Positive, nonzero, number of attributes that would be
 *           registered by a CGMS service instance. \n
 *           Zero on failure.
 */
QAPI_BLE_DECLARATION unsigned int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Query_Number_Attributes(void);

/**
 * @brief
 * Allows a mechanism for a CGMS Service to
 * successfully respond to a received read client configuration request.
 *
 * @param[in]  BluetoothStackID        Unique identifier assigned to this
 *                                     Bluetooth Protocol Stack via a
 *                                     call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID              Service instance ID to close.
 *                                     This is the value that was returned
 *                                     from the qapi_BLE_CGMS_Initialize_Service()
 *                                     function.
 *
 * @param[in]  TransactionID           Transaction ID of the original
 *                                     read request. This value was
 *                                     received in the
 *                                     QAPI_BLE_ET_CGMS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E
 *                                     event.
 *
 * @param[in]  ClientConfiguration     Specifies the CCCD to send
 *                                     to the remote device.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CGMS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_BT_GATT_ERROR_NOT_INITIALIZED \n
 *                 QAPI_BLE_BT_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Read_Client_Configuration_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t ClientConfiguration);

/**
 * @brief
 * Sends a CGMS Measurement
 * notification to a specified remote CGMS client.
 *
 * @details
 * The Flags parameter can either be QAPI_BLE_CGMS_E2E_CRC_SUPPORTED
 * or QAPI_BLE_CGMS_E2E_CRC_NOT_SUPPORTED to indicate whether a
 * CRC needs to be calculated and included with each CGMS Measurement.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from the qapi_BLE_CGMS_Initialize_Service()
 *                                 function.
 *
 * @param[in]  Flags               Flags used to control whether a
 *                                 CRC needs to be included with each CGMS
 *                                 Measurement. See above for more info.
 *
 * @param[in]  ConnectionID        ConnectionID of the remote device to
 *                                 send the notification to.
 *
 * @param[in]  NumberOfMeasurements  Number of measurements
 *
 * @param[in]  MeasurementData     Pointer to a list of pointers to
 *                                 CGMS Measurements that will be
 *                                 notified if this function is
 *                                 successful.
 *
 * @return      Positive non-zero if successful (represents the
 *              length of the notification).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CGMS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Notify_CGMS_Measurements(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, uint32_t Flags, uint32_t NumberOfMeasurements, qapi_BLE_CGMS_Measurement_Data_t **MeasurementData);

/**
 * @brief
 * Responds to a read
 * request from a CGMS client for the CGMS Feature Characteristic.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * QPAI_BLE_CGMS_ERROR_CODE_XXX from qapi_ble_cgmstypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * The CGMSFeature parameter is only required if the
 * ErrorCode parameter is QAPI_BLE_CGMS_ERROR_CODE_SUCCESS.
 * Otherwise it may be excluded (NULL).
 *
 * The Flags parameter can either be QAPI_BLE_CGMS_E2E_CRC_SUPPORTED
 * or QAPI_BLE_CGMS_E2E_CRC_NOT_SUPPORTED to indicate whether a
 * CRC needs to be calculated and included with the CGMS Feature. It is
 * worth noting that the CGMS client may choose to ignore the CRC, since
 * the CGMS client cannot determine support for the E2E-CRC without the
 * CGMS Feature.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CGMS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  Flags               Flags used to control whether a
 *                                 CRC needs to be included with the
 *                                 CGMS Feature. See above for more
 *                                 info.
 *
 * @param[in]  ErrorCode           Error code for the request, which
 *                                 indicates whether the request was
 *                                 successful or an error has occured.
 *
 * @param[in]  CGMSFeature         CGMS Features that are supported
 *                                 by the CGMS server that will be sent
 *                                 in the response if the ErrorCode is
 *                                 for success.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CGMS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Feature_Read_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint32_t Flags, uint8_t ErrorCode, qapi_BLE_CGMS_Feature_Data_t *CGMSFeature);

/**
 * @brief
 * Responds to a read
 * request from a CGMS client for the CGMS Status Characteristic.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * QPAI_BLE_CGMS_ERROR_CODE_XXX from qapi_ble_cgmstypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * The CGMSStatus parameter is only required if the
 * ErrorCode parameter is QAPI_BLE_CGMS_ERROR_CODE_SUCCESS. Otherwise it
 * may be excluded (NULL).
 *
 * The Flags parameter can either be QAPI_BLE_CGMS_E2E_CRC_SUPPORTED
 * or QAPI_BLE_CGMS_E2E_CRC_NOT_SUPPORTED to indicate whether a
 * CRC needs to be calculated and included with the CGMS Status.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CGMS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  Flags               Flags used to control whether a
 *                                 CRC needs to be included with the
 *                                 CGMS Status. See above for more
 *                                 info.
 *
 * @param[in]  ErrorCode           Error code for the request, which
 *                                 indicates whether the request was
 *                                 successful or an error has occured.
 *
 * @param[in]  CGMSStatus          CGMS Status Characteristic that
 *                                 will be sent in the response if the
 *                                 ErrorCode is for success.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CGMS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Status_Read_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint32_t Flags, uint8_t ErrorCode, qapi_BLE_CGMS_Status_Data_t *CGMSStatus);

/**
 * @brief
 * Responds to a read
 * request from a CGMS client for the CGMS Session Start Time
 * Characteristic.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * QPAI_BLE_CGMS_ERROR_CODE_XXX from qapi_ble_cgmstypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * The SessionStartTime parameter is only required if the
 * ErrorCode parameter is QAPI_BLE_CGMS_ERROR_CODE_SUCCESS. Otherwise it
 * may be excluded (NULL).
 *
 * The Flags parameter can either be QAPI_BLE_CGMS_E2E_CRC_SUPPORTED
 * or QAPI_BLE_CGMS_E2E_CRC_NOT_SUPPORTED to indicate whether a
 * CRC needs to be calculated and included with the CGMS Session Start
 * Time.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CGMS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  Flags                Flags used to control whether a
 *                                 CRC needs to be included with the
 *                                 CGMS Session Start Time. See above
 *                                 for more info.
 *
 * @param[in]  ErrorCode           Error code for the request, which
 *                                 indicates whether the request was
 *                                 successful or an error has occured.
 *
 * @param[in]  SessionStartTime    CGMS Session Start Time
 *                                 Characteristic that will be sent in
 *                                 the response if the ErrorCode is for
 *                                 success.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CGMS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Session_Start_Time_Read_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint32_t Flags, uint8_t ErrorCode, qapi_BLE_CGMS_Session_Start_Time_Data_t *SessionStartTime);

/**
 * @brief
 * Responds to a write
 * request from a CGMS client for the CGMS Session Start Time
 * Characteristic.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * QPAI_BLE_CGMS_ERROR_CODE_XXX from qapi_ble_cgmstypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CGMS_Initialize_XXX()
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
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CGMS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Session_Start_Time_Write_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode);

/**
 * @brief
 * Responds to a read
 * request from a CGMS client for the CGMS Session Run Time
 * Characteristic.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * QPAI_BLE_CGMS_ERROR_CODE_XXX from qapi_ble_cgmstypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * The SessionRunTime parameter is only required if the
 * ErrorCode parameter is QAPI_BLE_CGMS_ERROR_CODE_SUCCESS. Otherwise it
 * may be excluded (NULL).
 *
 * The Flags parameter can either be QAPI_BLE_CGMS_E2E_CRC_SUPPORTED
 * or QAPI_BLE_CGMS_E2E_CRC_NOT_SUPPORTED to indicate whether a
 * CRC needs to be calculated and included with the CGMS Session Run
 * Time.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CGMS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  Flags               Flags used to control whether a
 *                                 CRC needs to be included with the
 *                                 CGMS Session Run Time. See above
 *                                 for more info.
 *
 * @param[in]  ErrorCode           Error code for the request, which
 *                                 indicates whether the request was
 *                                 successful or an error has occured.
 *
 * @param[in]  SessionRunTime      CGMS Session Run Time
 *                                 Characteristic that will be sent in
 *                                 the response if the ErrorCode is for
 *                                 success.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CGMS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Session_Run_Time_Read_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint32_t Flags, uint8_t ErrorCode, qapi_BLE_CGMS_Session_Run_Time_Data_t *SessionRunTime);

/**
 * @brief
 * Responds to a write
 * request from a CGMS client for the CGMS RACP characteristic.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * QPAI_BLE_CGMS_ERROR_CODE_XXX from qapi_ble_cgmstypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * This function is primarily provided to allow a way to reject RACP
 * Commands if the corresponding CCCD has not been configured for indications by the
 * CGMS client that made the request, a procedure is already in
 * progress, or the CGMS client does not have
 * the required security such as authentication, authorization, or
 * encryption.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CGMS_Initialize_XXX()
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
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CGMS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Record_Access_Control_Point_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode);

/**
 * @brief
 * Sends an indication for the
 * number of stored records. This is in response to an RACP request for the
 * Number of Stored Records that was successful.
 *
 * @details
 * This function sends an indication, which provides a guarantee that the
 * CGMS client will receive the value, since the CGMS client must confirm
 * that it has been received.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the Number Of Stored
 * Records Characteristic that is going to be indicated has been previously
 * configured for indications. A CGMS client must have written the CGMS RACP
 * characteristic's CCCD to enable indications.
 *
 * @note1hang
 * Only one indication may be outstanding per CGMS Instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CGMS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the
 *                                 connection for the indication.
 *
 * @param[in]  NumberOfStoredRecords    Number of stored records to
 *                                      indicate to the CGMS client.
 *
 * @return      Positive nonzero if successful (represents the
 *              GATT Transaction ID for the indication).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CGMS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Indicate_Number_Of_Stored_Records(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, uint16_t NumberOfStoredRecords);

/**
 * @brief
 * Sends an indication for the
 * RACP request to the CGMS client. This function
 * is used to indicate if the RACP Procedure was successful or an error
 * occured.
 *
 * @details
 * This function sends an indication, which provides a guarantee that the
 * CGMS client will receive the value, since the CGMS client must confirm
 * that it has been received.
 *
 * This function must not be used if the CGMS server is responding to an RACP
 * Request for the Number Of Stored Records and the request has been accepted.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the RACP
 * characteristic that is going to be indicated has been
 * previously configured for indications. A CGMS client must have written the
 * CGMS RACP characteristic's CCCD to enable
 * indications.
 *
 * @note1hang
 * Only one indication may be outstanding per CGMS Instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CGMS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the
 *                                 connection for the indication.
 *
 * @param[in]  RequestOpCode       Op Code of the RACP Request.
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
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CGMS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Indicate_Record_Access_Control_Point_Result(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, uint8_t RequestOpCode, uint8_t ResponseCode);

/**
 * @brief
 * Responds to a write
 * request from a CGMS client for the CGMS
 * SOCP characteristic.
 *
 * @details
 * The ErrorCode parameter must be a valid value from
 * QPAI_BLE_CGMS_ERROR_CODE_XXX from qapi_ble_cgmstypes.h or
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * This function is primarily provided to allow a way to reject SOCP
 * Commands if the corresponding CCCD has not been configured for indications by the
 * CGMS client that made the request, or the CGMS client does not have
 * the required security such as authentication, authorization, or
 * encryption.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CGMS_Initialize_XXX()
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
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CGMS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Specific_Ops_Control_Point_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode);

/**
 * @brief
 * Sends an indication for the
 * SOCP request to the CGMS client. This function
 * is used to indicate if the SOCP Procedure was successful or an error
 * occured.
 *
 * @details
 * This function sends an indication, which provides a guarantee that the
 * CGMS client will receive the value, since the CGMS client must confirm
 * that it has been received.
 *
 * This function should not be used if the CGMS server is sending an indication
 * for the Communication Interval, Calibartion Data, or Alert Level and the
 * indcation is for the success response.
 *
 * The Flags parameter can either be QAPI_BLE_CGMS_E2E_CRC_SUPPORTED
 * or QAPI_BLE_CGMS_E2E_CRC_NOT_SUPPORTED to indicate whether a
 * CRC needs to be calculated and included with the indication.
 *
 * RequestOpCode must be of the form: \n
 *                QAPI_BLE_CGMS_SPECIFIC_OPS_CP_OPCODE_XXX
 *
 * ResponseCodeValue must be of the form: \n
 *                QAPI_BLE_CGMS_SPECIFIC_OPS_CP_RESPONSE_OPCODE_XXX
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the SOCP
 * characteristic that is going to be indicated has been
 * previously configured for indications. A CGMS client must have written the
 * CGMS SOCP characteristic's CCCD to enable
 * indications.
 *
 * @note1hang
 * Only one indication may be outstanding per CGMS Instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CGMS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the
 *                                 connection for the indication.
 *
 * @param[in]  Flags               Flags used to control whether a
 *                                 CRC needs to be included with the
 *                                 indication. See above
 *                                 for more info.
 *
 * @param[in]  RequestOpCode       Op Code of the SOCP Request.
 *
 * @param[in]  ResponseCodeValue   Response Code of the SOCP
 *                                 Procedure result. This indicates
 *                                 whether the procedure was successful
 *                                 or an error occured.
 *
 * @return      Positive nonzero if successful (represents the
 *              GATT Transaction ID for the indication).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CGMS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Indicate_CGMS_Specific_Ops_Control_Point_Result(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, uint32_t Flags, uint8_t RequestOpCode, uint8_t ResponseCodeValue);

/**
 * @brief
 * Sends an indication of the for
 * the SOCP Communication Interval request to the
 * CGMS client. If this indication is sent, the indication will imply that the
 * SOCP Procedure was successful.
 *
 * @details
 * This function sends an indication, which provides a guarantee that the
 * CGMS client will receive the value, since the CGMS client must confirm
 * that it has been received.
 *
 * The Flags parameter can either be QAPI_BLE_CGMS_E2E_CRC_SUPPORTED
 * or QAPI_BLE_CGMS_E2E_CRC_NOT_SUPPORTED to indicate whether a
 * CRC needs to be calculated and included with the indication.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the SOCP
 * characteristic that is going to be indicated has been
 * previously configured for indications. A CGMS client must have written the
 * CGMS SOCP characteristic's CCCD to enable
 * indications.
 *
 * @note1hang
 * Only one indication may be outstanding per CGMS Instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CGMS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the
 *                                 connection for the indication.
 *
 * @param[in]  Flags               Flags used to control whether a
 *                                 CRC needs to be included with the
 *                                 indication. See above
 *                                 for more info.
 *
 * @param[in]  CommunicationIntervalMinutes    Communication interval
 *                                             to send in the indication.
 *
 * @return      Positive nonzero if successful (represents the
 *              GATT Transaction ID for the indication).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CGMS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Indicate_Communication_Interval(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, uint32_t Flags, uint8_t CommunicationIntervalMinutes);

/**
 * @brief
 * Sends an indication of the for
 * the SOCP Calibration Data request to the
 * CGMS client. If this indication is sent, the indication will imply that the
 * SOCP Procedure was successful.
 *
 * @details
 * This function sends an indication, which provides a guarantee that the
 * CGMS client will receive the value, since the CGMS client must confirm
 * that it has been received.
 *
 * The Flags parameter can either be QAPI_BLE_CGMS_E2E_CRC_SUPPORTED
 * or QAPI_BLE_CGMS_E2E_CRC_NOT_SUPPORTED to indicate whether a
 * CRC needs to be calculated and included with the indication.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the SOCP
 * characteristic that is going to be indicated has been
 * previously configured for indications. A CGMS client must have written the
 * CGMS SOCP characteristic's CCCD to enable
 * indications.
 *
 * @note1hang
 * Only one indication may be outstanding per CGMS Instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CGMS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the
 *                                 connection for the indication.
 *
 * @param[in]  Flags               Flags used to control whether a
 *                                 CRC needs to be included with the
 *                                 indication. See above
 *                                 for more info.
 *
 * @param[in]  CalibrationData     Calibration data to
 *                                 send in the indication.
 *
 * @return      Positive nonzero if successful (represents the
 *              GATT Transaction ID for the indication).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CGMS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Indicate_Calibration_Data(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, uint32_t Flags, qapi_BLE_CGMS_Calibration_Data_Record_t *CalibrationData);

/**
 * @brief
 * Sends an indication of the for
 * the SOCP Alert Level request to the
 * CGMS client. If this indication is sent, the indication will imply that the
 * SOCP Procedure was successful.
 *
 * @details
 * This function sends an indication, which provides a guarantee that the
 * CGMS client will receive the value, since the CGMS client must confirm
 * that it has been received.
 *
 * The Flags parameter can either be QAPI_BLE_CGMS_E2E_CRC_SUPPORTED
 * or QAPI_BLE_CGMS_E2E_CRC_NOT_SUPPORTED to indicate whether a
 * CRC needs to be calculated and included with the indication.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the SOCP
 * characteristic that is going to be indicated has been
 * previously configured for indications. A CGMS client must have written the
 * CGMS SOCP characteristic's CCCD to enable
 * indications.
 *
 * @note1hang
 * Only one indication may be outstanding per CGMS Instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CGMS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the
 *                                 connection for the indication.
 *
 * @param[in]  Flags               Flags used to control whether a
 *                                 CRC needs to be included with the
 *                                 indication. See above
 *                                 for more info.
 *
 * @param[in]  ResponseOpCode      Response op code that indicates
 *                                 the SOCP Procedure this indication is
 *                                 for. This must be an Alert Level
 *                                 related response op code.
 *
 * @param[in]  AlertLevel          Alert Level to
 *                                 send in the indication.
 *
 * @return      Positive nonzero if successful (represents the
 *              GATT Transaction ID for the indication).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CGMS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Indicate_Alert_Level(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, uint32_t Flags, qapi_BLE_CGMS_SOCP_Response_Type_t ResponseOpCode, uint16_t AlertLevel);

   /* CGMS Client API.                                                  */

/**
 * @brief
 * Parses a value received in an
 * indication from a remote CGMS server, interpreting it as a CGMS
 * Measurement.
 *
 * @details
 * If the NumberOfMeasurements parameter is 0, the MeasurementData
 * parameter may be excluded (NULL), and this function will return a
 * positive nonzero value, which represents the size of the MeasurementData needed
 * to hold the decoded data. The CGMS client may use this size to
 * allocate a buffer necessary to hold the decoded data.
 *
 * If a CRC is attached to a decoded CGMS Measurement, the CRCFlags field of
 * the #qapi_BLE_CGMS_Measurement_Data_t structure will be marked that there is a
 * CRC present and if it is valid. See QAPI_BLE_CGMS_E2E_CRC_PRESENT and
 * QAPI_BLE_CGMS_E2E_CRC_VALID for more information. This way the CGMS client can
 * determine if a CRC has been included and is valid for a CGMS Measurement.
 *
 * Since the Flags field of a CGMS Measurement does not indicate whether a CRC
 * is present as an optional field (Size indicates this), the CGMS Measurements,
 * when decoded, will each be marked in the (CRCFlags field) of the
 * qapi_BLE_CGMS_Measurement_Data_t structure by the bit values
 * QAPI_BLE_CGMS_E2E_CRC_PRESENT and QAPI_BLE_CGMS_E2E_CRC_VALID to indicate if
 * each CGMS Measurement has a CRC present and if it is valid. This way, if
 * multiple CGMS Measurements are decoded and some contain a CRC while others do
 * not, they can still be decoded.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            CGMS server.
 *
 * @param[in]  Value          Value received from the
 *                            CGMS server.
 *
 * @param[in]   NumberOfMeasurements    Number of measurements to
 *                                      decode from the Value parameter.
 *
 * @param[out]  MeasurementData    Pointer to a list of CGMS
 *                                 Measurement pointers that will hold
 *                                 the decoded CGMS Measurement data if
 *                                 this function is successful.
 *
 * @return      Positive nonzero if the length of the expected MeasurementData is calculated
 *              (represents the minimum size needed for the MeasurementData parameter).
 *
 * @return      Zero if the CGMS Measurements are decoded.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CGMS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Decode_CGMS_Measurement(uint32_t ValueLength, uint8_t *Value, uint32_t NumberOfMeasurements, qapi_BLE_CGMS_Measurement_Data_t **MeasurementData);

/**
 * @brief
 * Parses a value received in a read
 * response from a remote CGMS server, interpreting it as the CGMS Feature.
 *
 * @details
 * Since the CGMS client cannot determine if E2E-CRC is supported by the CGMS
 * Server before the CGMS Feature has been decoded, this function will simply
 * compare the incoming CRC to the QAPI_BLE_CGMS_E2E_NOT_SUPPORTED_VALUE found in
 * qapi_ble_cgmstypes.h. If the incoming CRC is not set to this value then the CRC
 * will be verified for the CGMS Feature.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            CGMS server.
 *
 * @param[in]  Value          Value received from the
 *                            CGMS server.
 *
 * @param[out]  CGMSFeature    Pointer that will hold the CGMS Feature
 *                             Characteristic if this function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CGMS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Decode_CGMS_Feature(uint32_t ValueLength, uint8_t *Value, qapi_BLE_CGMS_Feature_Data_t *CGMSFeature);

/**
 * @brief
 * Parses a value received in a read
 * response from a remote CGMS server, interpreting it as the CGMS Status.
 *
 * @details
 * The Flags parameter can either be QAPI_BLE_CGMS_E2E_CRC_SUPPORTED
 * or QAPI_BLE_CGMS_E2E_CRC_NOT_SUPPORTED to indicate whether a
 * CRC needs to be verified for the decoded CGMS Status.
 *
 * @param[in]  Flags          Flags used to control whether a
 *                            CRC needs to be verified. See above for more info.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            CGMS server.
 *
 * @param[in]  Value          Value received from the
 *                            CGMS server.
 *
 * @param[out]  CGMSStatus    Pointer that will hold the CGMS Status
 *                            Characteristic if this function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CGMS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Decode_CGMS_Status(uint32_t Flags, uint32_t ValueLength, uint8_t *Value, qapi_BLE_CGMS_Status_Data_t *CGMSStatus);

/**
 * @brief
 * Parses a value received in a read
 * response from a remote CGMS server, interpreting it as the CGMS Session Start
 * Time.
 *
 * @details
 * The Flags parameter can either be QAPI_BLE_CGMS_E2E_CRC_SUPPORTED
 * or QAPI_BLE_CGMS_E2E_CRC_NOT_SUPPORTED to indicate whether a
 * CRC needs to be verified for the decoded CGMS Session Start Time.
 *
 * @param[in]  Flags          Flags used to control whether a
 *                            CRC needs to be verified. See above for more info.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            CGMS server.
 *
 * @param[in]  Value          Value received from the
 *                            CGMS server.
 *
 * @param[out]  SessionTime   Pointer that will hold the CGMS Session
 *                            Start Time Characteristic if this function
 *                            is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CGMS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Decode_Session_Start_Time(uint32_t Flags, uint32_t ValueLength, uint8_t *Value, qapi_BLE_CGMS_Session_Start_Time_Data_t *SessionTime);

/**
 * @brief
 * Formats the CGMS Session Start
 * Time Characteristic descriptor into a user-specified buffer, for a GATT
 * Write request, that will be sent to the CGMS server. This function may also
 * be used to determine the size of the buffer to hold the formatted data (see
 * below).
 *
 * @details
 * The Flags parameter can either be QAPI_BLE_CGMS_E2E_CRC_SUPPORTED
 * or QAPI_BLE_CGMS_E2E_CRC_NOT_SUPPORTED to indicate whether a
 * CRC needs to be include for the CGMS Session Start Time Characteristic.
 *
 * If the BufferLength parameter is 0, the Buffer parameter may be excluded
 * (NULL), and this function will return a positive nonzero value, which
 * represents the size of the buffer needed to hold the formatted data. The
 * CGMS client may use this size to allocate a buffer necessary to hold the
 * formatted data.
 *
 * @param[in]  Flags          Flags used to control whether a
 *                            CRC needs to be included with the formatted data.
 *                            See above for more info.
 *
 * @param[in]  SessionStartTime      Pointer to the Session Start Time
 *                                   Characteristic data that will be formatted
 *                                   into the user-specified buffer.
 *
 * @param[in]  BufferLength          Length of the user-specified
 *                                   buffer.
 *
 * @param[out] Buffer                User-specified buffer that will
 *                                   hold the formatted data if this
 *                                   function is successful.
 *
 * @return      Positive nonzero if the length of the expected buffer is calculated
 *              (represents the minimum size needed for the Buffer parameter).
 *
 * @return      Zero if the SessionStartTime is formatted into the specified buffer.
 *
 *              An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CGMS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Format_Session_Start_Time(uint32_t Flags, qapi_BLE_CGMS_Session_Start_Time_Data_t *SessionStartTime, uint32_t BufferLength, uint8_t *Buffer);

/**
 * @brief
 * Parsesg a value received in a read
 * response from a remote CGMS server, interpreting it as the CGMS Session Run
 * Time.
 *
 * @details
 * The Flags parameter can either be QAPI_BLE_CGMS_E2E_CRC_SUPPORTED
 * or QAPI_BLE_CGMS_E2E_CRC_NOT_SUPPORTED to indicate whether a
 * CRC needs to be verified for the decoded CGMS Session Run Time.
 *
 * @param[in]  Flags          Flags used to control whether a
 *                            CRC needs to be verified. See above for more info.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            CGMS server.
 *
 * @param[in]  Value          Value received from the
 *                            CGMS server.
 *
 * @param[out]  CGMSRunTime   Pointer that will hold the CGMS Session
 *                            Run Time Characteristic if this function is
 *                            successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CGMS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Decode_Session_Run_Time(uint32_t Flags, uint32_t ValueLength, uint8_t *Value, qapi_BLE_CGMS_Session_Run_Time_Data_t *CGMSRunTime);

/**
 * @brief
 * Parses a value received in an
 * indication from a remote CGMS server, interpreting it as the CGMS
 * RACP characteristic.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            CGMS server.
 *
 * @param[in]  Value          Value received from the
 *                            CGMS server.
 *
 * @param[out]  RACPData      Pointer that will hold the RACP Response
 *                            data if this function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CGMS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Decode_Record_Access_Control_Point_Response(uint32_t ValueLength, uint8_t *Value, qapi_BLE_CGMS_RACP_Response_Data_t *RACPData);

/**
 * @brief
 * Formats the CGMS RACP
 * characteristic request into a user-specified buffer, for
 * a GATT Write request, that will be sent to the CGMS server. This function may
 * also be used to determine the size of the buffer to hold the formatted data
 * (see below).
 *
 * @details
 * If the BufferLength parameter is 0, the Buffer parameter may be excluded
 * (NULL), and this function will return a positive nonzero value, which
 * represents the size of the buffer needed to hold the formatted data.  The
 * CGMS client may use this size to allocate a buffer necessary to hold the
 * formatted data.
 *
 * @param[in]  FormatData            Pointer to the RACP Request data
 *                                   that will be formatted into the
 *                                   user-specified buffer.
 *
 * @param[in]  BufferLength          Length of the user-specified
 *                                   buffer.
 *
 * @param[out] Buffer                User-specified buffer that will
 *                                   hold the formatted data if this
 *                                   function is successful.
 *
 * @return      Positive nonzero if the length of the expected buffer is calculated
 *              (represents the minimum size needed for the Buffer parameter).
 *
 * @return      Zero if the FormatData is formatted into the specified buffer.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Format_Record_Access_Control_Point_Command(qapi_BLE_CGMS_RACP_Format_Data_t *FormatData, uint32_t *BufferLength, uint8_t *Buffer);

/**
 * @brief
 * Parses a value received in an
 * indication from a remote CGMS server interpreting it as the CGMS
 * SOCP characteristic.
 *
 * @details
 * The Flags parameter can either be QAPI_BLE_CGMS_E2E_CRC_SUPPORTED
 * or QAPI_BLE_CGMS_E2E_CRC_NOT_SUPPORTED to indicate whether a
 * CRC needs to be verified for the decoded CGMS SOCP response.
 *
 * @param[in]  Flags          Flags used to control whether a
 *                            CRC needs to be verified. See above for more info.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            CGMS server.
 *
 * @param[in]  Value          Value received from the
 *                            CGMS server.
 *
 * @param[out]  CGMSCPData    Pointer that will hold the SOCP Response
 *                            data if this function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CGMS_ERROR_MALFORMATTED_DATA
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Decode_CGMS_Specific_Ops_Control_Point_Response(uint32_t Flags, uint32_t ValueLength, uint8_t *Value, qapi_BLE_CGMS_Specific_Ops_Control_Point_Response_Data_t *CGMSCPData);

/**
 * @brief
 * Formatd the CGMS SOCP
 * characteristic request into a user-specified buffer, for
 * a GATT Write request, that will be sent to the CGMS server. This function may
 * also be used to determine the size of the buffer to hold the formatted data
 * (see below).
 *
 * @details
 * If the BufferLength parameter is 0, the Buffer parameter may be excluded
 * (NULL), and this function will return a positive nonzero value, which
 * represents the size of the buffer needed to hold the formatted data.  The
 * CGMS client may use this size to allocate a buffer necessary to hold the
 * formatted data.
 *
 * The Flags parameter can either be QAPI_BLE_CGMS_E2E_CRC_SUPPORTED or
 * QAPI_BLE_CGMS_E2E_CRC_NOT_SUPPORTED to indicate whether a CRC needs to be
 * included for the CGMS SOCP response.
 *
 * @param[in]  Flags                 Flags used to control whether a
 *                                   CRC needs to be included.
 *
 * @param[in]  FormatData            Pointer to the SOCP Request data
 *                                   that will be formatted into the
 *                                   user-specified buffer.
 *
 * @param[in]  BufferLength          Length of the user-specified
 *                                   buffer.
 *
 * @param[out] Buffer                User-specified buffer that will
 *                                   hold the formatted data if this
 *                                   function is successful.
 *
 * @return      Positive nonzero if the length of the expected buffer is calculated
 *              (represents the minimum size needed for the Buffer parameter).
 *
 * @return      Zero if the FormatData is formatted into the specified buffer.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CGMS_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CGMS_Format_CGMS_Specific_Ops_Control_Point_Command(uint32_t Flags, qapi_BLE_CGMS_Specific_Ops_Control_Point_Format_Data_t *FormatData, uint32_t BufferLength, uint8_t *Buffer);

/** @}
 */

#endif

