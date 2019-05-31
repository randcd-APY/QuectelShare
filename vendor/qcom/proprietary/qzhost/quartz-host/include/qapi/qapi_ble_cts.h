/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_ble_cts.h
 *
 * @brief
 * QCA QAPI for Bluetopia Bluetooth Current Time Service (CTS)
 * (GATT based) API Type Definitions, Constants, and Prototypes.
 *
 * @details
 * The Current Time Service (CTS) programming interface defines the protocols and
 * procedures to be used to implement the Current Time Service
 * capabilities.
 */

#ifndef __QAPI_BLE_CTS_H__
#define __QAPI_BLE_CTS_H__

#include "./qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.          */
#include "./qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.    */
#include "./qapi_ble_gatt.h"      /* QAPI GATT prototypes.                    */
#include "./qapi_ble_ctstypes.h"  /* QAPI CTS prototypes.                     */

/**
@addtogroup qapi_ble_services
@{
 */

   /* Error Return Codes.                                               */

   /* Error Codes that are smaller than these (less than -1000) are     */
   /* related to the Bluetooth Protocol Stack itself (see               */
   /* qapi_ble_errors.h).                                               */
#define QAPI_BLE_CTS_ERROR_INVALID_PARAMETER             (-1000)
/**< Invalid parameter. */
#define QAPI_BLE_CTS_ERROR_INVALID_BLUETOOTH_STACK_ID    (-1001)
/**< Invalid Bluetooth Stack ID. */
#define QAPI_BLE_CTS_ERROR_INSUFFICIENT_RESOURCES        (-1002)
/**< Insufficient Resources. */
#define QAPI_BLE_CTS_ERROR_SERVICE_ALREADY_REGISTERED    (-1003)
/**< Service is already registered. */
#define QAPI_BLE_CTS_ERROR_INVALID_INSTANCE_ID           (-1004)
/**< Invalid service instance ID. */
#define QAPI_BLE_CTS_ERROR_MALFORMATTED_DATA             (-1005)
/**< Malformatted data. */
#define QAPI_BLE_CTS_ERROR_UNKNOWN_ERROR                 (-1006)
/**< Unknown error. */

   /* The following defines the values of the Flags parameter that is   */
   /* provided in the CTS_Initialize_Service_Flags() function.          */
   /* * NOTE * Other CTS_Initialize_XXX API's will implicitly register  */
   /*          GATT for LE only and will not allow the use of these     */
   /*          flags.                                                   */
#define QAPI_BLE_CTS_FLAGS_SUPPORT_LE                       0x0001
/**< CTS supports LE. */
#define QAPI_BLE_CTS_FLAGS_SUPPORT_BR_EDR                   0x0002
/**< CTS supports BR/EDR. */
#define QAPI_BLE_CTS_FLAGS_SUPPORT_CURRENT_TIME_GATT_WRITE  0x0004
/**< CTS supports writing the Current Time Characteristic. */
#define QAPI_BLE_CTS_FLAGS_SUPPORT_LOCAL_TIME_GATT_WRITE    0x0008
/**< CTS supports writing the Local Time Characteristic. */

#define QAPI_BLE_CTS_DEFAULT_FEATURES_BIT_MASK           (QAPI_BLE_CTS_FLAGS_SUPPORT_LOCAL_TIME_GATT_WRITE | QAPI_BLE_CTS_FLAGS_SUPPORT_CURRENT_TIME_GATT_WRITE | QAPI_BLE_CTS_FLAGS_SUPPORT_BR_EDR | QAPI_BLE_CTS_FLAGS_SUPPORT_LE)

/**
 * Enumeration of all of the valid Months of the Year values
 * that may be assigned in the Current Time.
 */
typedef enum
{
   QAPI_BLE_MY_UNKNOWN_E   = QAPI_BLE_CTS_MONTH_OF_YEAR_UNKNOWN,
   /**< Unknown month. */
   QAPI_BLE_MY_JANUARY_E   = QAPI_BLE_CTS_MONTH_OF_YEAR_JANUARY,
   /**< January. */
   QAPI_BLE_MY_FEBRUARY_E  = QAPI_BLE_CTS_MONTH_OF_YEAR_FEBRUARY,
   /**< February. */
   QAPI_BLE_MY_MARCH_E     = QAPI_BLE_CTS_MONTH_OF_YEAR_MARCH,
   /**< March. */
   QAPI_BLE_MY_APRIL_E     = QAPI_BLE_CTS_MONTH_OF_YEAR_APRIL,
   /**< April. */
   QAPI_BLE_MY_MAY_E       = QAPI_BLE_CTS_MONTH_OF_YEAR_MAY,
   /**< May. */
   QAPI_BLE_MY_JUNE_E      = QAPI_BLE_CTS_MONTH_OF_YEAR_JUNE,
   /**< June. */
   QAPI_BLE_MY_JULY_E      = QAPI_BLE_CTS_MONTH_OF_YEAR_JULY,
   /**< July. */
   QAPI_BLE_MY_AUGUST_E    = QAPI_BLE_CTS_MONTH_OF_YEAR_AUGUST,
   /**< August. */
   QAPI_BLE_MY_SEPTEMBER_E = QAPI_BLE_CTS_MONTH_OF_YEAR_SEPTEMBER,
   /**< September. */
   QAPI_BLE_MY_OCTOBER_E   = QAPI_BLE_CTS_MONTH_OF_YEAR_OCTOBER,
   /**< October. */
   QAPI_BLE_MY_NOVEMBER_E  = QAPI_BLE_CTS_MONTH_OF_YEAR_NOVEMBER,
   /**< November. */
   QAPI_BLE_MY_DECEMBER_E  = QAPI_BLE_CTS_MONTH_OF_YEAR_DECEMBER
   /**< December. */
} qapi_BLE_CTS_Month_Of_Year_Type_t;

/**
 * Enumeration of all of the valid Day of the Week values
 * that may be assigned in the Current Time.
 */
typedef enum
{
   QAPI_BLE_WD_UNKNOWN_E   = QAPI_BLE_CTS_DAY_OF_WEEK_UNKNOWN,
   /**< Unknown day. */
   QAPI_BLE_WD_MONDAY_E    = QAPI_BLE_CTS_DAY_OF_WEEK_MONDAY,
   /**< Monday. */
   QAPI_BLE_WD_TUESDAY_E   = QAPI_BLE_CTS_DAY_OF_WEEK_TUESDAY,
   /**< Tuesday. */
   QAPI_BLE_WD_WEDNESDAY_E = QAPI_BLE_CTS_DAY_OF_WEEK_WEDNESDAY,
   /**< Wednesday. */
   QAPI_BLE_WD_THURSDAY_E  = QAPI_BLE_CTS_DAY_OF_WEEK_THURSDAY,
   /**< Thursday. */
   QAPI_BLE_WD_FRIDAY_E    = QAPI_BLE_CTS_DAY_OF_WEEK_FRIDAY,
   /**< Friday. */
   QAPI_BLE_WD_SATURDAY_E  = QAPI_BLE_CTS_DAY_OF_WEEK_SATURDAY,
   /**< Saturday. */
   QAPI_BLE_WD_SUNDAY_E    = QAPI_BLE_CTS_DAY_OF_WEEK_SUNDAY
   /**< Sunday. */
} qapi_BLE_CTS_Week_Day_Type_t;

/**
 * Enumeration of all of the valid Time Zone values that may
 * be assigned in the Local Time Information.
 */
typedef enum
{
   QAPI_BLE_TZ_UTC_MINUS_1200_E = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_MINUS_12_00,
   /**< UTC -1200. */
   QAPI_BLE_TZ_UTC_MINUS_1100_E = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_MINUS_11_00,
   /**< UTC -1100. */
   QAPI_BLE_TZ_UTC_MINUS_1000_E = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_MINUS_10_00,
   /**< UTC -1000. */
   QAPI_BLE_TZ_UTC_MINUS_930_E  = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_MINUS_9_30,
   /**< UTC -930. */
   QAPI_BLE_TZ_UTC_MINUS_900_E  = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_MINUS_9_00,
   /**< UTC -900. */
   QAPI_BLE_TZ_UTC_MINUS_800_E  = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_MINUS_8_00,
   /**< UTC -800. */
   QAPI_BLE_TZ_UTC_MINUS_700_E  = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_MINUS_7_00,
   /**< UTC -700. */
   QAPI_BLE_TZ_UTC_MINUS_600_E  = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_MINUS_6_00,
   /**< UTC -600. */
   QAPI_BLE_TZ_UTC_MINUS_500_E  = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_MINUS_5_00,
   /**< UTC -500. */
   QAPI_BLE_TZ_UTC_MINUS_430_E  = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_MINUS_4_30,
   /**< UTC -430. */
   QAPI_BLE_TZ_UTC_MINUS_400_E  = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_MINUS_4_00,
   /**< UTC -400. */
   QAPI_BLE_TZ_UTC_MINUS_330_E  = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_MINUS_3_30,
   /**< UTC -330. */
   QAPI_BLE_TZ_UTC_MINUS_300_E  = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_MINUS_3_00,
   /**< UTC -300. */
   QAPI_BLE_TZ_UTC_MINUS_200_E  = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_MINUS_2_00,
   /**< UTC -200. */
   QAPI_BLE_TZ_UTC_MINUS_100_E  = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_MINUS_1_00,
   /**< UTC -100. */
   QAPI_BLE_TZ_UTC_PLUS_000_E   = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_PLUS_0_00,
   /**< UTC 0. */
   QAPI_BLE_TZ_UTC_PLUS_100_E   = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_PLUS_1_00,
   /**< UTC 100. */
   QAPI_BLE_TZ_UTC_PLUS_200_E   = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_PLUS_2_00,
   /**< UTC 200. */
   QAPI_BLE_TZ_UTC_PLUS_300_E   = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_PLUS_3_00,
   /**< UTC 300. */
   QAPI_BLE_TZ_UTC_PLUS_330_E   = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_PLUS_3_30,
   /**< UTC 330. */
   QAPI_BLE_TZ_UTC_PLUS_400_E   = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_PLUS_4_00,
   /**< UTC 400. */
   QAPI_BLE_TZ_UTC_PLUS_430_E   = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_PLUS_4_30,
   /**< UTC 430. */
   QAPI_BLE_TZ_UTC_PLUS_500_E   = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_PLUS_5_00,
   /**< UTC 500. */
   QAPI_BLE_TZ_UTC_PLUS_530_E   = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_PLUS_5_30,
   /**< UTC 530. */
   QAPI_BLE_TZ_UTC_PLUS_545_E   = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_PLUS_5_45,
   /**< UTC 545. */
   QAPI_BLE_TZ_UTC_PLUS_600_E   = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_PLUS_6_00,
   /**< UTC 600. */
   QAPI_BLE_TZ_UTC_PLUS_630_E   = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_PLUS_6_30,
   /**< UTC 630. */
   QAPI_BLE_TZ_UTC_PLUS_700_E   = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_PLUS_7_00,
   /**< UTC 700. */
   QAPI_BLE_TZ_UTC_PLUS_800_E   = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_PLUS_8_00,
   /**< UTC 800. */
   QAPI_BLE_TZ_UTC_PLUS_845_E   = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_PLUS_8_45,
   /**< UTC 845. */
   QAPI_BLE_TZ_UTC_PLUS_900_E   = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_PLUS_9_00,
   /**< UTC 900. */
   QAPI_BLE_TZ_UTC_PLUS_930_E   = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_PLUS_9_30,
   /**< UTC 930. */
   QAPI_BLE_TZ_UTC_PLUS_1000_E  = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_PLUS_10_00,
   /**< UTC 1000. */
   QAPI_BLE_TZ_UTC_PLUS_1030_E  = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_PLUS_10_30,
   /**< UTC 1030. */
   QAPI_BLE_TZ_UTC_PLUS_1100_E  = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_PLUS_11_00,
   /**< UTC 1100. */
   QAPI_BLE_TZ_UTC_PLUS_1130_E  = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_PLUS_11_30,
   /**< UTC 1130. */
   QAPI_BLE_TZ_UTC_PLUS_1200_E  = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_PLUS_12_00,
   /**< UTC 1200. */
   QAPI_BLE_TZ_UTC_PLUS_1245_E  = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_PLUS_12_45,
   /**< UTC 1245. */
   QAPI_BLE_TZ_UTC_PLUS_1300_E  = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_PLUS_13_00,
   /**< UTC 1300. */
   QAPI_BLE_TZ_UTC_PLUS_1400_E  = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_PLUS_14_00,
   /**< UTC 1400. */
   QAPI_BLE_TZ_UTC_UNKNOWN_E    = QAPI_BLE_CTS_TIME_ZONE_UTC_OFFSET_UNKNOWN
   /**< UTC Unknown. */
} qapi_BLE_CTS_Time_Zone_Type_t;

/**
 * Enumeration of all of the valid Daylight Savings Time
 * (DST) Offset values that may be assigned in the Local Time
 * Information.
 */
typedef enum
{
   QAPI_BLE_DO_STANDARD_TIME_E               = QAPI_BLE_CTS_DST_OFFSET_STANDARD_TIME,
   /**< Daylight Savings Time standard time. */
   QAPI_BLE_DO_HALF_AN_HOUR_DAY_LIGHT_TIME_E = QAPI_BLE_CTS_DST_OFFSET_HALF_AN_HOUR_DAYLIGHT_TIME,
   /**< Daylight Savings Time Half an Hour Day time. */
   QAPI_BLE_DO_DAY_LIGHT_TIME_E              = QAPI_BLE_CTS_DST_OFFSET_DAYLIGHT_TIME,
   /**< Daylight Savings Time Daylight time. */
   QAPI_BLE_DO_DOUBLE_DAY_LIGHT_TIME_E       = QAPI_BLE_CTS_DST_OFFSET_DOUBLE_DAYLIGHT_TIME,
   /**< Daylight Savings Time Double Daylight time. */
   QAPI_BLE_DO_UNKNOWN_E                     = QAPI_BLE_CTS_DST_OFFSET_UNKNOWN
   /**< Daylight Savings Time Unknown. */
} qapi_BLE_CTS_DST_Offset_Type_t;

/**
 * Enumeration of all of the valid Time Source values that
 * may be assigned in the Reference Time Information.
 */
typedef enum
{
   QAPI_BLE_TS_UNKNOWN_E               = QAPI_BLE_CTS_TIME_SOURCE_UNKNOWN,
   /**< Time source is unknown. */
   QAPI_BLE_TS_NETWORK_TIME_PROTOCOL_E = QAPI_BLE_CTS_TIME_SOURCE_NETWORK_TIME_PROTOCOL,
   /**< Time source is NTP. */
   QAPI_BLE_TS_GPS_E                   = QAPI_BLE_CTS_TIME_SOURCE_GPS,
   /**< Time source is GPS. */
   QAPI_BLE_TS_RADIO_TIME_SIGNAL_E     = QAPI_BLE_CTS_TIME_SOURCE_RADIO_TIME_SIGNAL,
   /**< Time source is radio time signal. */
   QAPI_BLE_TS_MANUAL_E                = QAPI_BLE_CTS_TIME_SOURCE_MANUAL,
   /**< Time source is manual. */
   QAPI_BLE_TS_ATOMIC_CLOCK_E          = QAPI_BLE_CTS_TIME_SOURCE_ATOMIC_CLOCK,
   /**< Time source is atomic clock. */
   QAPI_BLE_TS_CELLULAR_NETWORK_E      = QAPI_BLE_CTS_TIME_SOURCE_CELLULAR_NETWORK
   /**< Time source is cellular network. */
} qapi_BLE_CTS_Time_Source_Type_t;

/**
 * Structure that represents the Date/Time data for CTS.
 */
typedef struct qapi_BLE_CTS_Date_Time_Data_s
{
   /**
    * Year.
    */
   uint16_t                          Year;

   /**
    * Month.
    */
   qapi_BLE_CTS_Month_Of_Year_Type_t Month;

   /**
    * Day.
    */
   uint8_t                           Day;

   /**
    * Hour.
    */
   uint8_t                           Hours;

   /**
    * Minutes.
    */
   uint8_t                           Minutes;

   /**
    * Seconds.
    */
   uint8_t                           Seconds;
} qapi_BLE_CTS_Date_Time_Data_t;

#define QAPI_BLE_CTS_DATE_TIME_DATA_SIZE                 (sizeof(qapi_BLE_CTS_Date_Time_Data_t))
/**<
 * Size of the #qapi_BLE_CTS_Date_Time_Data_t structure.
 */

#define QAPI_BLE_CTS_DATE_TIME_VALID(_x)                 ((QAPI_BLE_GATT_DATE_TIME_VALID_YEAR(((_x)).Year)) && (QAPI_BLE_GATT_DATE_TIME_VALID_MONTH(((_x)).Month)) && (QAPI_BLE_GATT_DATE_TIME_VALID_DAY(((_x)).Day)) && (QAPI_BLE_GATT_DATE_TIME_VALID_HOURS(((_x)).Hours)) && (QAPI_BLE_GATT_DATE_TIME_VALID_MINUTES(((_x)).Minutes)) && (QAPI_BLE_GATT_DATE_TIME_VALID_SECONDS(((_x)).Seconds)))
/**<
 * This macro is a utility macro that exists to valid that a specified
 * Date Time is valid. The only parameter to this function is the
 * CTS_Date_Time_Data_t structure to valid. This macro returns TRUE if
 * the Date Time is valid or FALSE otherwise..
 */

/**
 * Structure that represents the CPS Day/Date/Time data.
 */
typedef  struct qapi_BLE_CTS_Day_Date_Time_Data_s
{
   /**
    * Date/time.
    */
   qapi_BLE_CTS_Date_Time_Data_t Date_Time;

   /**
    * Day of the week.
    */
   qapi_BLE_CTS_Week_Day_Type_t  Day_Of_Week;
} qapi_BLE_CTS_Day_Date_Time_Data_t;

#define QAPI_BLE_CTS_DAY_DATE_TIME_DATA_SIZE             (sizeof(qapi_BLE_CTS_Day_Date_Time_Data_t))
/**<
 * Size of the #qapi_BLE_CTS_Day_Date_Time_Data_t structure.
 */

/**
 * Structure that represents the CPS Exact Time data.
 */
typedef  struct qapi_BLE_CTS_Exact_Time_Data_s
{
   /**
    * Day and date/time data.
    */
   qapi_BLE_CTS_Day_Date_Time_Data_t Day_Date_Time;

   /**
    * The 1/256 of a second value.
    */
   uint8_t                           Fractions256;
} qapi_BLE_CTS_Exact_Time_Data_t;

#define QAPI_BLE_CTS_EXACT_TIME_DATA_SIZE                (sizeof(qapi_BLE_CTS_Exact_Time_Data_t))
/**<
 * Size of the #qapi_BLE_CTS_Exact_Time_Data_t structure.
 */

/**
 * Structure that represents the CPS Current Time data.
 */
typedef  struct qapi_BLE_CTS_Current_Time_Data_s
{
   /**
    * Exact time data.
    */
   qapi_BLE_CTS_Exact_Time_Data_t Exact_Time;

   /**
    * Bitmask that indicates the reason the time was adjusted.
    *
    * Valid values have the form
    * QAPI_BLE_CTS_CURRENT_TIME_ADJUST_REASON_XXX and can be found in
    * qapi_ble_ctstypes.h.
    */
   uint8_t                        Adjust_Reason_Mask;
}  qapi_BLE_CTS_Current_Time_Data_t;

#define QAPI_BLE_CTS_CURRENT_TIME_DATA_SIZE              (sizeof(qapi_BLE_CTS_Current_Time_Data_t))
/**<
 * Size of the #qapi_BLE_CTS_Current_Time_Data_t structure.
 */

/**
 * Structure that represents the CPS Local Time Information data.
 */
typedef  struct qapi_BLE_CTS_Local_Time_Information_Data_s
{
   /**
    * Time zone.
    */
   qapi_BLE_CTS_Time_Zone_Type_t  Time_Zone;

   /**
    * Daylight Savings Time (DST) offset.
    */
   qapi_BLE_CTS_DST_Offset_Type_t Daylight_Saving_Time;
}  qapi_BLE_CTS_Local_Time_Information_Data_t;

#define QAPI_BLE_CTS_LOCAL_TIME_INFORMATION_DATA_SIZE    (sizeof(qapi_BLE_CTS_Local_Time_Information_Data_t))
/**<
 * Size of the #qapi_BLE_CTS_Local_Time_Information_Data_t structure.
 */

/**
 * Structure that represents the CPS Reference Time Information data.
 */
typedef  struct qapi_BLE_CTS_Reference_Time_Information_Data_s
{
   /**
    * Source of the time information.
    */
   qapi_BLE_CTS_Time_Source_Type_t Source;

   /**
    * Accuracy of the time information.
    */
   uint8_t                         Accuracy;

   /**
    * Days since the time was updated.
    */
   uint8_t                         Days_Since_Update;

   /**
    * Hours since the time was updated.
    */
   uint8_t                         Hours_Since_Update;
}  qapi_BLE_CTS_Reference_Time_Information_Data_t;

#define QAPI_BLE_CTS_REFERENCE_TIME_INFORMATION_DATA_SIZE  (sizeof(qapi_BLE_CTS_Reference_Time_Information_Data_t))
/**<
 * Size of the #qapi_BLE_CTS_Reference_Time_Information_Data_t structure.
 */

/**
 * Enumeration of the valid Read Request types that a server
 * may receive in a
 * QAPI_BLE_ET_CTS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E or
 * QAPI_BLE_ET_CTS_SERVER_CLIENT_CONFIGURATION_UPDATE_E event.This is
 * also used by the qapi_BLE_CTS_Send_Notification() to denote the
 * characteristic value to notify.
 *
 * For each event, it is up to the application to return (or write) the
 * correct Client Configuration descriptor based on this value.
 */
typedef enum
{
   QAPI_BLE_CT_CURRENT_TIME_E /**< Current Time. */
} qapi_BLE_CTS_Characteristic_Type_t;

/**
 * Enumeration of all the events generated by the CTS
 * Service.These are used to determine the type of each event generated,
 * and to ensure the proper union element is accessed for the
 * #qapi_BLE_CTS_Event_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_ET_CTS_SERVER_READ_CLIENT_CONFIGURATION_REQUEST_E,
   /**< Read CCCD request event. */
   QAPI_BLE_ET_CTS_SERVER_UPDATE_CLIENT_CONFIGURATION_REQUEST_E,
   /**< Write CCCD request event. */
   QAPI_BLE_ET_CTS_SERVER_READ_CURRENT_TIME_REQUEST_E,
   /**< Read Current Time request event. */
   QAPI_BLE_ET_CTS_SERVER_WRITE_CURRENT_TIME_REQUEST_E,
   /**< Write Current Time request event. */
   QAPI_BLE_ET_CTS_SERVER_WRITE_LOCAL_TIME_INFORMATION_REQUEST_E,
   /**< Read Local Time request event. */
   QAPI_BLE_ET_CTS_SERVER_READ_REFERENCE_TIME_INFORMATION_REQUEST_E
   /**< Read Reference Time Information request event. */
} qapi_BLE_CTS_Event_Type_t;

/**
 * Structure that represents the format for the data that is dispatched
 * to a CTS server when a CTS client has sent a request to read a CTS
 * characteristic's CCCD.
 *
 * Some of this structures fields will be required when responding to a
 * request using the
 * qapi_BLE_CTS_Read_Client_Configuration_Response() function.
 */
typedef struct qapi_BLE_CTS_Read_Client_Configuration_Data_s
{
   /**
    * CTS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the CTS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the CTS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * Bluetooth address of the CTS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the requested CCCD based on the CTS Characteristic
    * type.
    */
   qapi_BLE_CTS_Characteristic_Type_t ClientConfigurationType;
} qapi_BLE_CTS_Read_Client_Configuration_Data_t;

#define QAPI_BLE_CTS_READ_CLIENT_CONFIGURATION_DATA_SIZE  (sizeof(qapi_BLE_CTS_Read_Client_Configuration_Data_t))
/**<
 * Size of the #qapi_BLE_CTS_Read_Client_Configuration_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a CTS server when a CTS client has sent a request to write a
 * CTS characteristic's CCCD.
 */
typedef struct qapi_BLE_CTS_Client_Configuration_Update_Data_s
{
   /**
    * CTS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * GATT connection ID for the connection with the CTS client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the CTS client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * Bluetooth address of the CTS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the requested CCCD based on the CTS characteristic
    * type.
    */
   qapi_BLE_CTS_Characteristic_Type_t ClientConfigurationType;

   /**
    * CCCD value that has been requested to be written.
    */
   uint16_t                           ClientConfiguration;
} qapi_BLE_CTS_Client_Configuration_Update_Data_t;

#define QAPI_BLE_CTS_CLIENT_CONFIGURATION_UPDATE_DATA_SIZE  (sizeof(qapi_BLE_CTS_Client_Configuration_Update_Data_t))
/**<
 * Size of the #qapi_BLE_CTS_Client_Configuration_Update_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a CTS server when a CTS client has sent a request to read the CTS
 * Current Time Characteristic.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_CTS_Current_Time_Read_Request_Response() or
 * qapi_BLE_CTS_Current_Time_Read_Request_Error_Response() functions.
 */
typedef struct qapi_BLE_CTS_Read_Current_Time_Request_Data_s
{
   /**
    * CTS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the CTS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the CTS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * Bluetooth address of the CTS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;
} qapi_BLE_CTS_Read_Current_Time_Request_Data_t;

#define QAPI_BLE_CTS_READ_CURRENT_TIME_REQUEST_DATA_SIZE  (sizeof(qapi_BLE_CTS_Read_Current_Time_Request_Data_t))
/**<
 * Size of the #qapi_BLE_CTS_Read_Current_Time_Request_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a CTS server when a CTS client has sent a request to write the CTS
 * Current Time Characteristic.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_CTS_Current_Time_Write_Request_Response() function.
 */
typedef struct qapi_BLE_CTS_Write_Current_Time_Request_Data_s
{
   /**
    * CTS instance that dispatched the event.
    */
   uint32_t                         InstanceID;

   /**
    * GATT connection ID for the connection with the CTS client
    * that made the request.
    */
   uint32_t                         ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                         TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the CTS client.
    */
   qapi_BLE_GATT_Connection_Type_t  ConnectionType;

   /**
    * Bluetooth address of the CTS client that made the request.
    */
   qapi_BLE_BD_ADDR_t               RemoteDevice;

   /**
    * CTS Current Time data that has been requested to be written.
    */
   qapi_BLE_CTS_Current_Time_Data_t CurrentTime;
} qapi_BLE_CTS_Write_Current_Time_Request_Data_t;

#define QAPI_BLE_CTS_WRITE_CURRENT_TIME_REQUEST_DATA_SIZE  (sizeof(qapi_BLE_CTS_Write_Current_Time_Request_Data_t))
/**<
 * Size of the #qapi_BLE_CTS_Write_Current_Time_Request_Data_t structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a CTS server when a CTS client has sent a request to write the CTS
 * Local Time Characteristic.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_CTS_Local_Time_Information_Write_Request_Response()
 * function.
 */
typedef struct qapi_BLE_CTS_Write_Local_Time_Information_Request_Data_s
{
   /**
    * CTS instance that dispatched the event.
    */
   uint32_t                                   InstanceID;

   /**
    * GATT connection ID for the connection with the CTS client
    * that made the request.
    */
   uint32_t                                   ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                                   TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the CTS client.
    */
   qapi_BLE_GATT_Connection_Type_t            ConnectionType;

   /**
    * Bluetooth address of the CTS client that made the request.
    */
   qapi_BLE_BD_ADDR_t                         RemoteDevice;

   /**
    * CTS Local Time data that has been requested to be written.
    */
   qapi_BLE_CTS_Local_Time_Information_Data_t LocalTime;
} qapi_BLE_CTS_Write_Local_Time_Information_Request_Data_t;

#define QAPI_BLE_CTS_WRITE_LOCAL_TIME_INFORMATION_REQUEST_DATA_SIZE  (sizeof(qapi_BLE_CTS_Write_Local_Time_Information_Request_Data_t))
/**<
 * Size of the #qapi_BLE_CTS_Write_Local_Time_Information_Request_Data_t
 * structure.
 */

/**
 * Structure that represents the format for the data that is dispatched
 * to a CTS server when a CTS client has sent a request to read the CTS
 * Reference Time Characteristic.
 *
 * Some of the structure fields will be required when responding to a
 * request using the
 * qapi_BLE_CTS_Reference_Time_Information_Read_Request_Response() or
 * qapi_BLE_CTS_Reference_Time_Information_Read_Request_Error_Response()
 * functions.
 */
typedef struct qapi_BLE_CTS_Read_Reference_Time_Information_Request_Data_s
{
   /**
    * CTS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * GATT connection ID for the connection with the CTS client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * GATT transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * GATT connection type, which identifies the transport used for
    * the connection with the CTS client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * Bluetooth address of the CTS client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;
} qapi_BLE_CTS_Read_Reference_Time_Information_Request_Data_t;

#define QAPI_BLE_CTS_READ_REFERENCE_TIME_INFORMATION_REQUEST_DATA_SIZE (sizeof(qapi_BLE_CTS_Read_Reference_Time_Information_Request_Data_t))
/**<
 * Size of the
 * #qapi_BLE_CTS_Read_Reference_Time_Information_Request_Data_t structure.
 */

/**
 * Structure that represents the container structure for holding all the
 * event data for a CTS instance.
 */
typedef struct qapi_BLE_CTS_Event_Data_s
{
   /**
    * Event type used to determine the appropriate union member of
    * the Event_Data field to access.
    */
   qapi_BLE_CTS_Event_Type_t Event_Data_Type;

   /**
    * Total size of the data contained in the event.
    */
   uint16_t                  Event_Data_Size;
   union
   {
      /**
       * CTS Read CCCD data.
       */
      qapi_BLE_CTS_Read_Client_Configuration_Data_t               *CTS_Read_Client_Configuration_Data;

      /**
       * CTS Write CCCD data.
       */
      qapi_BLE_CTS_Client_Configuration_Update_Data_t             *CTS_Client_Configuration_Update_Data;

      /**
       * CTS Read Current Time data.
       */
      qapi_BLE_CTS_Read_Current_Time_Request_Data_t               *CTS_Read_Current_Time_Request_Data;

      /**
       * CTS Write Current Time data.
       */
      qapi_BLE_CTS_Write_Current_Time_Request_Data_t              *CTS_Write_Current_Time_Request_Data;

      /**
       * CTS Write Local Time Information data.
       */
      qapi_BLE_CTS_Write_Local_Time_Information_Request_Data_t    *CTS_Write_Local_Time_Information_Request_Data;

      /**
       * CTS Read Reference Time data.
       */
      qapi_BLE_CTS_Read_Reference_Time_Information_Request_Data_t *CTS_Read_Reference_Time_Information_Request_Data;
   }
   /**
    * Event data.
    */
   Event_Data;
} qapi_BLE_CTS_Event_Data_t;

#define QAPI_BLE_CTS_EVENT_DATA_SIZE                     (sizeof(qapi_BLE_CTS_Event_Data_t))
/**<
 * Size of the #qapi_BLE_CTS_Event_Data_t structure.
 */

/**
 * Structure that contains the attribute handles that will need to be
 * cached by a CTS client in order to only do service discovery once.
 */
typedef struct qapi_BLE_CTS_Client_Information_s
{
   /**
    * CTS Current Time attribute handle.
    */
   uint16_t Current_Time;

   /**
    * CTS Current Time CCCD attribute handle.
    */
   uint16_t Current_Time_Client_Configuration;

   /**
    * CTS Local Time Information attribute handle.
    */
   uint16_t Local_Time_Information;

   /**
    * CTS Reference Time attribute handle.
    */
   uint16_t Reference_Time_Information;
} qapi_BLE_CTS_Client_Information_t;

#define QAPI_BLE_CTS_CLIENT_INFORMATION_DATA_SIZE        (sizeof(qapi_BLE_CTS_Client_Information_t))
/**<
 * Size of the #qapi_BLE_CTS_Client_Information_t structure.
 */

/**
 * Structure that contains all of the per Client data that will need to
 * be stored by a CGMS Server.
 */
typedef struct qapi_BLE_CTS_Server_Information_s
{
   /**
    * CTS Current Time CCCD.
    */
   uint16_t Current_Time_Client_Configuration;
} qapi_BLE_CTS_Server_Information_t;

#define QAPI_BLE_CTS_SERVER_INFORMATION_DATA_SIZE        sizeof(qapi_BLE_CTS_Server_Information_t))
/**<
 * Size of the #qapi_BLE_CTS_Server_Information_t structure.
 */

/**
 * @brief
 * This declared type represents the Prototype Function for a CTS
 * Event Callback. This function will be called whenever a defined CTS Event
 * occurs within the Bluetooth Protocol Stack that is specified with the
 * specified Bluetooth Stack ID.
 *
 * @details
 * The event information is passed to the user in an #qapi_BLE_CTS_Event_Data_t
 * structure that contains all the information about the
 * event that occurred.
 *
 * The caller should use the contents of the CTS Event Data
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
 * Context of a Thread that the user does not own. Therefore, processing
 * in this function should be as efficient as possible (this argument holds
 * anyway because another CTS Event will not be processed while this
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
 * @param[in]  CTS_Event_Data        Pointer to a structure that contains
 *                                   information about the event that has
 *                                   occurred.
 *
 * @param[in]  CallbackParameter     User-defined value that will be
 *                                   received with the CTS Event data.
 *
 * @return None.
 */
typedef void (QAPI_BLE_BTPSAPI *qapi_BLE_CTS_Event_Callback_t)(uint32_t BluetoothStackID, qapi_BLE_CTS_Event_Data_t *CTS_Event_Data, uint32_t CallbackParameter);

   /* CTS server API.                                                   */

/**
 * @brief
 * Initializes a CTS server on a specified Bluetooth Stack.
 *
 * @details
 * Only one CTS server may be open at a time, per the Bluetooth Stack ID.
 *
 * This function does not support CTS 1.1 and will not allow the optional
 * writing of the Current Time and Local Time Information
 * Characteristics.
 *
 * If CTS 1.1 is needed, use qapi_BLE_CTS_Initialize_Service_Flags().
 *
 * @param[in]  BluetoothStackID     Unique identifier assigned to this
 *                                  Bluetooth Protocol Stack via a
 *                                  call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  EventCallback        CTS Event Callback that will
 *                                  receive CTS server events.
 *
 * @param[in]  CallbackParameter    User-defined value that will be
 *                                  received with the specified
 *                                  EventCallback parameter.
 *
 * @param[out]  ServiceID           Unique GATT Service ID of the
 *                                  registered CTS service returned from
 *                                  qapi_BLE_GATT_Register_Service() API.
 *
 * @return      Positive, nonzero if successful. The return value will
 *              be the Service Instance ID of CTS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CTS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_CTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BTGATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CTS_Initialize_Service(uint32_t BluetoothStackID, qapi_BLE_CTS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

/**
 * @brief
 * Initializes a CTS server on a specified Bluetooth Stack.
 *
 * @details
 * Unlike qapi_BLE_CTS_Initialize_Service(), this function allows the
 * application to select a handle range in GATT to store the service.
 *
 * Only one CTS server may be open at a time, per the Bluetooth Stack ID.
 *
 * This function does not support CTS 1.1 and will not allow the optional
 * writing of the Current Time and Local Time Information
 * Characteristics.
 *
 * If CTS 1.1 is needed, use qapi_BLE_CTS_Initialize_Service_Flags().
 *
 * @param[in]  BluetoothStackID     Unique identifier assigned to this
 *                                  Bluetooth Protocol Stack via a
 *                                  call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  EventCallback        CTS Event Callback that will
 *                                  receive CTS server events.
 *
 * @param[in]  CallbackParameter    User-defined value that will be
 *                                  received with the specified
 *                                  EventCallback parameter.
 *
 * @param[out]  ServiceID           Unique GATT Service ID of the
 *                                  registered CTS service returned from
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
 *              be the Service Instance ID of CTS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CTS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_CTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BTGATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CTS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, qapi_BLE_CTS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

/**
 * @brief
 * Initializes a CTS server on a specified Bluetooth Stack.
 *
 * @details
 * Unlike qapi_BLE_CTS_Initialize_Service(), this function allows the
 * application to select a handle range in GATT to store the service. The
 * application should set the ServiceHandleRange parameter to NULL if the
 * feature is not needed.
 *
 * Only one CTS server may be open at a time, per the Bluetooth Stack ID.
 * This function supports CTS 1.1 and will allow the optional writing  of
 * the Current Time and Local Time Information Characteristics if the
 * QAPI_BLE_CTS_FLAGS_SUPPORT_CURRENT_TIME_GATT_WRITE or
 * QAPI_BLE_CTS_FLAGS_SUPPORT_LOCAL_TIME_GATT_WRITE are set.
 *
 * @param[in]  BluetoothStackID     Unique identifier assigned to this
 *                                  Bluetooth Protocol Stack via a
 *                                  call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  Flags                Flags that are used to register CTS
 *                                  for the specified transport and
 *                                  configure optional features. These
 *                                  flags can be found in qapi_ble_cts.h
 *                                  and have the form
 *                                  QAPI_BLE_CTS_FLAGS_XXX.
 *
 * @param[in]  EventCallback        CTS Event Callback that will
 *                                  receive CTS server events.
 *
 * @param[in]  CallbackParameter    User-defined value that will be
 *                                  received with the specified
 *                                  EventCallback parameter.
 *
 * @param[out]  ServiceID           Unique GATT Service ID of the
 *                                  registered CTS service returned from
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
 *              be the Service Instance ID of CTS server that was successfully
 *              opened on the specified Bluetooth Stack ID. This is the value
 *              that should be used in all subsequent function calls that
 *              require Instance ID.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CTS_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_CTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_SERVICE_TABLE_FORMAT \n
 *                 QAPI_BLE_BTGATT_ERROR_INSUFFICIENT_RESOURCES \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_BTGATT_ERROR_INVALID_BLUETOOTH_STACK_ID \n
 *                 QAPI_BLE_BTGATT_ERROR_NOT_INITIALIZED
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CTS_Initialize_Service_Flags(uint32_t BluetoothStackID, uint32_t Flags, qapi_BLE_CTS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

/**
 * @brief
 * Cleans up and frees all resources
 * associated with a Current Time Service (CTS) instance.
 *
 * @details
 * After this function is called, no other Device Information Service
 * function can be called until after a successful call to either of the
 * qapi_BLE_CTS_Initialize_XXX() functions are performed.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of the qapi_BLE_CTS_Initialize_XXX()
 *                                 functions.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CTS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CTS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * Queries the number of attributes
 * that are contained in the CTS Service that is registered with a
 * call to either the qapi_BLE_CTS_Initialize_Service() or the
 * qapi_BLE_CTS_Initialize_Service_Handle_Range() function.
 *
 * @details
 * This function will return a variable number of attributes depending
 * on how the service was configured.
 *
 * @return      Positive non-zero if successful (represents the number of attributes
 *              for CTS).
 *
 * @return      Zero for failure.
 */
QAPI_BLE_DECLARATION unsigned int QAPI_BLE_BTPSAPI qapi_BLE_CTS_Query_Number_Attributes(void);

/**
 * @brief
 * Responds to a
 * successful read request from a CTS client for the CTS Current Time
 * Characteristic.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  Current_Time        Pointer to the current time to send
 *                                 in the response to the CTS client.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CTS_Current_Time_Read_Request_Response(uint32_t BluetoothStackID, uint32_t TransactionID, qapi_BLE_CTS_Current_Time_Data_t *Current_Time);

/**
 * @brief
 * Responds to a read
 * request from a CTS client for the CTS Current Time Characteristic
 * that resulted in an error.
 *
 * @details
 * The ErrorCode parameter must be a valid value of the form
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  ErrorCode           Error code for the request, which
 *                                 indicates the error that has
 *                                 occured.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CTS_Current_Time_Read_Request_Error_Response(uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t ErrorCode);

/**
 * @brief
 * Responds to a write
 * request from a CTS client for the CTS Current Time Characteristic.
 *
 * @details
 * The ErrorCode parameter must be a valid value of the form
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.

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
 *                 QAPI_BLE_CTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CTS_Current_Time_Write_Request_Response(uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t ErrorCode);

/**
 * @brief
 * Allows the CTS server
 * to set the CTS Local Time Characteristic.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CTS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  Local_Time          Pointer to the local time that
 *                                 will be set if this function
 *                                 returns success.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CTS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CTS_Set_Local_Time_Information(uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_CTS_Local_Time_Information_Data_t *Local_Time);

/**
 * @brief
 * Allows the CTS server
 * to query the CTS Local Time Characteristic.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CTS_Initialize_XXX()
 *                                 functions.
 *
 * @param[out]  Local_Time         Pointer that will hold the local
 *                                 time if this function returns
 *                                 success.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CTS_ERROR_INVALID_INSTANCE_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CTS_Query_Local_Time_Information(uint32_t BluetoothStackID, uint32_t InstanceID, qapi_BLE_CTS_Local_Time_Information_Data_t *Local_Time);

/**
 * @brief
 * Responds to a write
 * request from a CTS client for the CTS Local Time Characteristic.
 *
 * @details
 * The ErrorCode parameter must be a valid value of the form
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.

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
 *                 QAPI_BLE_CTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CTS_Local_Time_Information_Write_Request_Response(uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t ErrorCode);

/**
 * @brief
 * Responds to a
 * successful read request from a CTS client for the CTS Reference
 * Time Information Characteristic.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  Reference_Time      Pointer to the reference time
 *                                 information to send in the
 *                                 response to the CTS client.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CTS_Reference_Time_Information_Read_Request_Response(uint32_t BluetoothStackID, uint32_t TransactionID, qapi_BLE_CTS_Reference_Time_Information_Data_t *Reference_Time);

/**
 * @brief
 * Responds to a read
 * request from a CTS client for the CTS Reference Time Information
 * Characteristic that resulted in an error.
 *
 * @details
 * The ErrorCode parameter must be a valid value of the form
 * QPAI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX from qapi_ble_atttypes.h.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  ErrorCode           Error code for the request, which
 *                                 indicates the error that has
 *                                 occured.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CTS_Reference_Time_Information_Read_Request_Error_Response(uint32_t BluetoothStackID, uint32_t TransactionID, uint8_t ErrorCode);

/**
 * @brief
 * Responds to a read
 * request from a CTS client for the CTS Current Time Characteristic's
 * CCCD.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CTS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  TransactionID       GATT transaction ID for the
 *                                 request.
 *
 * @param[in]  Client_Configuration   Current value of the CCCD that will be
 *                                    sent in the response.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CTS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_TRANSACTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CTS_Read_Client_Configuration_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint16_t Client_Configuration);

/**
 * @brief
 * Sends a notification
 * for an CTS Characteristic's value to an CTS client.
 *
 * @details
 * This function sends a notification, which provides no guarantee
 * that the CTS client will receive the value.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the CTS
 * Characteristic that is going to be notified has been previously
 * configured for notifications. A CTS client must have written
 * the CTS Characteristic's CCCD to enable notifications.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Service instance ID to close.
 *                                 This is the value that was returned
 *                                 from either of
 *                                 the qapi_BLE_CTS_Initialize_XXX()
 *                                 functions.
 *
 * @param[in]  ConnectionID        GATT connection ID of the
 *                                 connection for the notification.
 *
 * @param[in]  Current_Time        Pointer that contains the
 *                                 Current Time to send in the
 *                                 notification.
 *
 * @return      Positive nonzero if successful (represents the
 *              length of the notification).
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CTS_ERROR_INVALID_PARAMETER \n
 *                 QAPI_BLE_CTS_ERROR_INVALID_INSTANCE_ID \n
 *                 QAPI_BLE_GATT_ERROR_INVALID_CONNECTION_ID
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CTS_Notify_Current_Time(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_CTS_Current_Time_Data_t *Current_Time);

   /* CTS Client API.                                                   */

/**
 * @brief
 * Formatts the CTS Current Time
 * Characteristic into a user-specified buffer.
 *
 * @param[in]  Current_Time    Pointer that holds the Current Time that
 *                             will be formatted in the user-specified
 *                             buffer.
 *
 * @param[in]  BufferLength    Length of the user specified buffer.
 *
 * @param[out] Buffer          User-specified buffer that will hold
 *                             the formatted data if this function is
 *                             successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CTS_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CTS_Format_Current_Time(qapi_BLE_CTS_Current_Time_Data_t *Current_Time, uint32_t BufferLength, uint8_t *Buffer);

/**
 * @brief
 * Parses a value received from a
 * remote CTS server interpreting it as the CTS Current Time Characteristic.
 *
 * @details
 * This function should be used to decode the CTS Current Time
 * Characteristic received in a GATT Read response or GATT Notification.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            CTS server.
 *
 * @param[in]  Value          Value received from the
 *                            CTS server.
 *
 * @param[out]  Current_Time    Pointer that will hold the
 *                              Current Time if this function returns
 *                              success.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CTS_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CTS_Decode_Current_Time(uint32_t ValueLength, uint8_t *Value, qapi_BLE_CTS_Current_Time_Data_t *Current_Time);

/**
 * @brief
 * Formats the CTS Local Time
 * Characteristic into a user-specified buffer.
 *
 * @param[in]  Local_Time      Pointer that holds the Local Time that
 *                             will be formatted in the user-specified
 *                             buffer.
 *
 * @param[in]  BufferLength    Length of the user specified buffer.
 *
 * @param[out]  Buffer         The user-specified buffer that will hold
 *                             the formatted data if this function is
 *                             successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CTS_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CTS_Format_Local_Time_Information(qapi_BLE_CTS_Local_Time_Information_Data_t *Local_Time, uint32_t BufferLength, uint8_t *Buffer);

/**
 * @brief
 * Parses a value received from a
 * remote CTS server interpreting it as the CTS Local Time Characteristic.
 *
 * @details
 * This function should be used to decode the CTS Local Time Characteristic
 * received in a GATT Read response.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            CTS server.
 *
 * @param[in]  Value          Value received from the
 *                            CTS server.
 *
 * @param[out]  Local_Time    Pointer that will hold the
 *                            Local Time if this function returns
 *                            success.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CTS_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CTS_Decode_Local_Time_Information(uint32_t ValueLength, uint8_t *Value, qapi_BLE_CTS_Local_Time_Information_Data_t *Local_Time);

/**
 * @brief
 * Parses a value received from a
 * remote CTS server interpreting it as the CTS Reference Time
 * Characteristic.
 *
 * @details
 * This function should be used to decode the CTS Reference Time
 * Characteristic received in a GATT Read response.
 *
 * @param[in]  ValueLength    Length of the value received from the
 *                            CTS server.
 *
 * @param[in]  Value          Value received from the
 *                            CTS server.
 *
 * @param[out]  Reference_Time    Pointer that will hold the
 *                                Reference Time if this function returns
 *                                success.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative; one of the following values:
 *              @par
 *                 QAPI_BLE_CTS_ERROR_INVALID_PARAMETER
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_CTS_Decode_Reference_Time_Information(uint32_t ValueLength, uint8_t *Value, qapi_BLE_CTS_Reference_Time_Information_Data_t *Reference_Time);

#endif

/**
 *  @}
 */
