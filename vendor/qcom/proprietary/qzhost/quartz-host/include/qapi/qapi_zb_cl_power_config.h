/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_cl_power_config.h
   @brief QAPI for the ZigBee power configuration cluster.

   @addtogroup qapi_zb_cl_power_config
   @{

   This API provides definitions, commands, and events related to the ZigBee
   power configuration cluster.

   APIs are provided to create a power configuration server cluster.
   Cluster creation also registers an event callback that is used to receive
   events from the power configuration cluster. Since this cluster does not have
   any commands, this callback is only used for custom attribute management or
   application-specific commands.

   @}
*/

#ifndef __QAPI_ZB_CL_POWERCONFIG_H__ // [
#define __QAPI_ZB_CL_POWERCONFIG_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_zb_cl.h"

/** @addtogroup qapi_zb_cl_power_config
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following definitions represent the attribute IDs for the power
   configuration cluster. */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_MAINS_VOLTAGE                        (0x0000) /**< Power config cluster attribute ID for the mains voltage (uint16, read
                                                                                           only). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_MAINS_FREQUENCY                      (0x0001) /**< Power config cluster attribute ID for the mains frequency (uint8, read
                                                                                           only). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_MAINS_ALARM_MASK                     (0x0010) /**< Power config cluster attribute ID for the mains alarm configuration (map8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_MAINS_VOLTAGE_MIN_THRESHOLD          (0x0011) /**< Power config cluster attribute ID for the threshold of the mains minimum
                                                                                           voltage alarm (uint16). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_MAINS_VOLTAGE_MAX_THRESHOLD          (0x0012) /**< Power config cluster attribute ID for the threshold of the mains maximum
                                                                                           voltage alarm (uint16). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_MAINS_VOLTAGE_DWELL_TRIP_POINT       (0x0013) /**< Power config cluster attribute ID for the time the voltage may exist beyond
                                                                                           a trip point before an alarm is generated. */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY_VOLTAGE                      (0x0020) /**< Power config cluster attribute ID for the battery voltage (uint8, read
                                                                                           only). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY_PERCENTAGE_REMAINING         (0x0021) /**< Power config cluster attribute ID for the percentage of battery life
                                                                                           remaining (uint8, read only, reportable). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY_MANUFACTURER                 (0x0030) /**< Power config cluster attribute ID for the battery manufacturer (16 byte
                                                                                           string). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY_SIZE                         (0x0031) /**< Power config cluster attribute ID for the battery size (enum8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY_AMP_HOUR_RATING              (0x0032) /**< Power config cluster attribute ID for the battery amp-hour rating (uint16). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY_QUANTITY                     (0x0033) /**< Power config cluster attribute ID for the number of cells for the battery
                                                                                           (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY_RATED_VOLTAGE                (0x0034) /**< Power config cluster attribute ID for the rated voltage of the battery
                                                                                           (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY_ALARM_MASK                   (0x0035) /**< Power config cluster attribute ID for the battery alarm configuration
                                                                                           (map8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY_VOLTAGE_MIN_THRESHOLD        (0x0036) /**< Power config cluster attribute ID for the threshold of the battery voltage
                                                                                           low alarm (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY_VOLTAGE_THRESHOLD1           (0x0037) /**< Power config cluster attribute ID for the battery voltage threshold 1
                                                                                           (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY_VOLTAGE_THRESHOLD2           (0x0038) /**< Power config cluster attribute ID for the battery voltage threshold 2
                                                                                           (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY_VOLTAGE_THRESHOLD3           (0x0039) /**< Power config cluster attribute ID for the battery voltage threshold 3
                                                                                           (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY_PERCENTAGE_MIN_THRESHOLD     (0x003A) /**< Power config cluster attribute ID for the low battery percentage alarm
                                                                                           threshold (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY_PERCENTAGE_THRESHOLD1        (0x003B) /**< Power config cluster attribute ID for the battery voltage percentage
                                                                                           threshold 1 (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY_PERCENTAGE_THRESHOLD2        (0x003C) /**< Power config cluster attribute ID for the battery voltage percentage
                                                                                           threshold 2 (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY_PERCENTAGE_THRESHOLD3        (0x003D) /**< Power config cluster attribute ID for the battery voltage percentage
                                                                                           threshold 3 (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY_ALARM_STATE                  (0x003E) /**< Power config cluster attribute ID for the mask of current alarms for the
                                                                                           battery (map32). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY2_VOLTAGE                     (0x0040) /**< Power config cluster attribute ID for the battery 2 voltage (uint8, read
                                                                                           only). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY2_PERCENTAGE_REMAINING        (0x0041) /**< Power config cluster attribute ID for the percentage of battery 2 life
                                                                                           remaining (uint8, read only, reportable). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY2_MANUFACTURER                (0x0050) /**< Power config cluster attribute ID for the battery 2 manufacturer (16 byte
                                                                                           string). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY2_SIZE                        (0x0051) /**< Power config cluster attribute ID for the battery 2 size (enum8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY2_AMP_HOUR_RATING             (0x0052) /**< Power config cluster attribute ID for the battery 2 amp-hour rating
                                                                                           (uint16). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY2_QUANTITY                    (0x0053) /**< Power config cluster attribute ID for the number of cells for battery 2
                                                                                           (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY2_RATED_VOLTAGE               (0x0054) /**< Power config cluster attribute ID for the rated voltage of battery 2
                                                                                           (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY2_ALARM_MASK                  (0x0055) /**< Power config cluster attribute ID for the battery 2 alarm configuration
                                                                                           (map8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY2_VOLTAGE_MIN_THRESHOLD       (0x0056) /**< Power config cluster attribute ID for the threshold of the battery 2
                                                                                           voltage low alarm (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY2_VOLTAGE_THRESHOLD1          (0x0057) /**< Power config cluster attribute ID for the battery 2 voltage threshold 1
                                                                                           (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY2_VOLTAGE_THRESHOLD2          (0x0058) /**< Power config cluster attribute ID for the battery 2 voltage threshold 2
                                                                                           (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY2_VOLTAGE_THRESHOLD3          (0x0059) /**< Power config cluster attribute ID for the battery 2 voltage threshold 3
                                                                                           (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY2_PERCENTAGE_MIN_THRESHOLD    (0x005A) /**< Power config cluster attribute ID for the low battery 2 percentage alarm
                                                                                           threshold (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY2_PERCENTAGE_THRESHOLD1       (0x005B) /**< Power config cluster attribute ID for the battery 2 voltage percentage
                                                                                           threshold 1 (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY2_PERCENTAGE_THRESHOLD2       (0x005C) /**< Power config cluster attribute ID for the battery 2 voltage percentage
                                                                                           threshold 2 (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY2_PERCENTAGE_THRESHOLD3       (0x005D) /**< Power config cluster attribute ID for the battery 2 voltage percentage
                                                                                           threshold 3 (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY2_ALARM_STATE                 (0x005E) /**< Power config cluster attribute ID for the mask of current alarms for
                                                                                           battery 2 (map32). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY3_VOLTAGE                     (0x0060) /**< Power config cluster attribute ID for the battery 3 voltage (uint8, read
                                                                                           only). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY3_PERCENTAGE_REMAINING        (0x0071) /**< Power config cluster attribute ID for the percentage of battery 3 life
                                                                                           remaining (uint8, read only, reportable). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY3_MANUFACTURER                (0x0070) /**< Power config cluster attribute ID for the battery 3 manufacturer (16 byte
                                                                                           string). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY3_SIZE                        (0x0071) /**< Power config cluster attribute ID for the battery 3 size (enum8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY3_AMP_HOUR_RATING             (0x0072) /**< Power config cluster attribute ID for the battery 3 amp-hour rating
                                                                                           (uint16). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY3_QUANTITY                    (0x0073) /**< Power config cluster attribute ID for the number of cells for battery 3
                                                                                           (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY3_RATED_VOLTAGE               (0x0074) /**< Power config cluster attribute ID for the rated voltage of battery 3
                                                                                           (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY3_ALARM_MASK                  (0x0075) /**< Power config cluster attribute ID for the battery 3 alarm configuration
                                                                                           (map8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY3_VOLTAGE_MIN_THRESHOLD       (0x0076) /**< Power config cluster attribute ID for the threshold of the battery 3
                                                                                           voltage low alarm (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY3_VOLTAGE_THRESHOLD1          (0x0077) /**< Power config cluster attribute ID for the battery 3 voltage threshold 1
                                                                                           (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY3_VOLTAGE_THRESHOLD2          (0x0078) /**< Power config cluster attribute ID for the battery 3 voltage threshold 2
                                                                                           (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY3_VOLTAGE_THRESHOLD3          (0x0079) /**< Power config cluster attribute ID for the battery 3 voltage threshold 3
                                                                                           (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY3_PERCENTAGE_MIN_THRESHOLD    (0x007A) /**< Power config cluster attribute ID for the low battery 3 percentage alarm
                                                                                           threshold (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY3_PERCENTAGE_THRESHOLD1       (0x007B) /**< Power config cluster attribute ID for the battery 3 voltage percentage
                                                                                           threshold 1 (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY3_PERCENTAGE_THRESHOLD2       (0x007C) /**< Power config cluster attribute ID for the battery 3 voltage percentage
                                                                                           threshold 2 (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY3_PERCENTAGE_THRESHOLD3       (0x007D) /**< Power config cluster attribute ID for the battery 3 voltage percentage
                                                                                           threshold 3 (uint8). */
#define QAPI_ZB_CL_POWERCONFIG_ATTR_ID_BATTERY3_ALARM_STATE                 (0x007E) /**< Power config cluster attribute ID for the mask of current alarms for
                                                                                           battery 3 (map32). */

/* The following definitions represent the bitmask value fo the mains alarm mask
   attribute. */
#define QAPI_ZB_CL_POWERCONFIG_MAINS_ALARM_MASK_VOLTAGE_TOO_LOW        (0x01) /**< Power config cluster mains alarm mask for voltage is below the minimum
                                                                                    threshold. */
#define QAPI_ZB_CL_POWERCONFIG_MAINS_ALARM_MASK_VOLTAGE_TOO_HIGH       (0x02) /**< Power config cluster mains alarm mask for voltage is above the maximum
                                                                                    threshold. */
#define QAPI_ZB_CL_POWERCONFIG_MAINS_ALARM_MASK_POWER_LOST             (0x04) /**< Power config cluster mains alarm mask for supply is lost or unavailable. */

/* The following definitions represent the battery types. */
#define QAPI_ZB_CL_POWERCONFIG_BATTERY_TYPE_NO_BATTERY                 (0x00) /**< Power config battery type indicating no battery. */
#define QAPI_ZB_CL_POWERCONFIG_BATTERY_TYPE_BUILT_IN                   (0x01) /**< Power config battery type indicating a built-in battery. */
#define QAPI_ZB_CL_POWERCONFIG_BATTERY_TYPE_OTHER                      (0x02) /**< Power config battery type indicating another type of battery. */
#define QAPI_ZB_CL_POWERCONFIG_BATTERY_TYPE_AA                         (0x03) /**< Power config battery type indicating AA battery. */
#define QAPI_ZB_CL_POWERCONFIG_BATTERY_TYPE_AAA                        (0x04) /**< Power config battery type indicating AAA battery. */
#define QAPI_ZB_CL_POWERCONFIG_BATTERY_TYPE_C                          (0x05) /**< Power config battery type indicating C battery. */
#define QAPI_ZB_CL_POWERCONFIG_BATTERY_TYPE_D                          (0x06) /**< Power config battery type indicating D battery. */
#define QAPI_ZB_CL_POWERCONFIG_BATTERY_TYPE_CR2                        (0x07) /**< Power config battery type indicating CR2 battery. */
#define QAPI_ZB_CL_POWERCONFIG_BATTERY_TYPE_CR123A                     (0x08) /**< Power config battery type indicating CR123A battery. */
#define QAPI_ZB_CL_POWERCONFIG_BATTERY_TYPE_UNKNOWN                    (0xFF) /**< Power config battery type indicating an unknown battery type. */

/* The following definitions represent the bitmask value fo the battery alarm
   mask attributes. */
#define QAPI_ZB_CL_POWERCONFIG_BATTERY_ALARM_MASK_MIN_THRESHOLD        (0x01) /**< Power config cluster battery alarm mask for voltage is too low to continue
                                                                                    operating. */
#define QAPI_ZB_CL_POWERCONFIG_BATTERY_ALARM_MASK_THRESHOLD1           (0x02) /**< Power config cluster battery alarm mask for voltage is below threshold 1. */
#define QAPI_ZB_CL_POWERCONFIG_BATTERY_ALARM_MASK_THRESHOLD2           (0x04) /**< Power config cluster battery alarm mask for voltage is below threshold 2. */
#define QAPI_ZB_CL_POWERCONFIG_BATTERY_ALARM_MASK_THRESHOLD3           (0x08) /**< Power config cluster battery alarm mask for voltage is below threshold 3. */

/* The following definitions represent the alarm codes that can be generated
   by the power config cluster. */
#define QAPI_ZB_CL_POWERCONFIG_ALARM_CODE_MAINS_VOLTAGE_TOO_LOW        (0x00) /**< Power config alarm code indicating that the mains voltage has reached the
                                                                                    minimum threshold. */
#define QAPI_ZB_CL_POWERCONFIG_ALARM_CODE_MAINS_VOLTAGE_TOO_HIGH       (0x01) /**< Power config alarm code indicating that the mains voltage has reached the
                                                                                    maximum threshold. */
#define QAPI_ZB_CL_POWERCONFIG_ALARM_CODE_BATTERY_MIN_THRESHOLD        (0x10) /**< Power config alarm code indicating that the battery minimum threshold or minimum
                                                                                    percent threshold has been reached. */
#define QAPI_ZB_CL_POWERCONFIG_ALARM_CODE_BATTERY_THRESHOLD1           (0x11) /**< Power config alarm code indicating that the battery threshold 1 or percent
                                                                                    threshold 1 has been reached. */
#define QAPI_ZB_CL_POWERCONFIG_ALARM_CODE_BATTERY_THRESHOLD2           (0x12) /**< Power config alarm code indicating that the battery threshold 2 or percent
                                                                                    threshold 2 has been reached. */
#define QAPI_ZB_CL_POWERCONFIG_ALARM_CODE_BATTERY_THRESHOLD3           (0x13) /**< Power config alarm code indicating that the battery threshold 3 or percent
                                                                                    threshold 3 has been reached. */
#define QAPI_ZB_CL_POWERCONFIG_ALARM_CODE_BATTERY2_MIN_THRESHOLD       (0x20) /**< Power config alarm code indicating that the battery 2 minimum threshold or
                                                                                    minimum percent threshold has been reached. */
#define QAPI_ZB_CL_POWERCONFIG_ALARM_CODE_BATTERY2_THRESHOLD1          (0x21) /**< Power config alarm code indicating that the battery 2 threshold 1 or percent
                                                                                    threshold 1 has been reached. */
#define QAPI_ZB_CL_POWERCONFIG_ALARM_CODE_BATTERY2_THRESHOLD2          (0x22) /**< Power config alarm code indicating that the battery 2 threshold 2 or percent
                                                                                    threshold 2 has been reached. */
#define QAPI_ZB_CL_POWERCONFIG_ALARM_CODE_BATTERY2_THRESHOLD3          (0x23) /**< Power config alarm code indicating that the battery 2 threshold 3 or percent
                                                                                    threshold 3 has been reached. */
#define QAPI_ZB_CL_POWERCONFIG_ALARM_CODE_BATTERY3_MIN_THRESHOLD       (0x30) /**< Power config alarm code indicating that the battery 3 minimum threshold or
                                                                                    minimum percent threshold has been reached. */
#define QAPI_ZB_CL_POWERCONFIG_ALARM_CODE_BATTERY3_THRESHOLD1          (0x31) /**< Power config alarm code indicating that the battery 3 threshold 1 or percent
                                                                                    threshold 1 has been reached. */
#define QAPI_ZB_CL_POWERCONFIG_ALARM_CODE_BATTERY3_THRESHOLD2          (0x32) /**< Power config alarm code indicating that the battery 3 threshold 2 or percent
                                                                                    threshold 2 has been reached. */
#define QAPI_ZB_CL_POWERCONFIG_ALARM_CODE_BATTERY3_THRESHOLD3          (0x33) /**< Power config alarm code indicating that the battery 3 threshold 3 or percent
                                                                                    threshold 3 has been reached. */
#define QAPI_ZB_CL_POWERCONFIG_ALARM_CODE_MAINS_UNAVAILABLE            (0x3A) /**< Power config alarm code indicating that the mains power is lost or unavaiable. */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Enumeration of events that can be received by a power configuration server
   cluster.
*/
typedef enum
{
   QAPI_ZB_CL_POWERCONFIG_SERVER_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates an attribute custom read event. */
   QAPI_ZB_CL_POWERCONFIG_SERVER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates an attribute custom write event. */
   QAPI_ZB_CL_POWERCONFIG_SERVER_EVENT_TYPE_UNPARSED_DATA_E,     /**< Indicates an unparsed data event. */
   QAPI_ZB_CL_POWERCONFIG_SERVER_EVENT_TYPE_ALARM_RESET_E        /**< Indicates a reset alarm event. */
} qapi_ZB_CL_PowerConfig_Server_Event_Type_t;

/**
   Structure representing the data for events received by a temperature
   configuration server cluster.
*/
typedef struct qapi_ZB_CL_PowerConfig_Server_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_PowerConfig_Server_Event_Type_t Event_Type;

   /** Data for the event.  The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for custom read events. */
      qapi_ZB_CL_Attr_Custom_Read_t           Attr_Custom_Read;

      /** Information for custom write events. */
      qapi_ZB_CL_Attr_Custom_Write_t          Attr_Custom_Write;

      /** Information for unparsed data. */
      qapi_ZB_CL_Unparsed_Data_t              Unparsed_Data;

      /** Information for reset alarm events. */
      qapi_ZB_CL_Custom_Cluster_Reset_Alarm_t Reset_Alarm_Data;
   } Data;
} qapi_ZB_CL_PowerConfig_Server_Event_Data_t;

/**
   @brief Function definition that represents a callback function that handles
          asynchronous events for a temperature configuration server cluster.

   @param[in] ZB_Handle  Handle of the ZigBee instance.
   @param[in] Cluster    Handle for the cluster.
   @param[in] Event_Data Information for the cluster event.
   @param[in] CB_Param   User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_PowerConfig_Server_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_PowerConfig_Server_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Creates a temperature configuration server cluster for
          a specific endpoint.

   The cluster can be destroyed using qapi_ZB_CL_Destroy_Cluster().

   @param[in]  ZB_Handle    Handle of the ZigBee instance.
   @param[out] Cluster      Pointer to where the newly-created cluster will be
                            stored upon successful return.
   @param[in]  Cluster_Info Structure containing the endpoint and attribute
                            information for the cluster.
   @param[in]  Event_CB     Callback function that will handle events for the
                            temperature configuration server cluster.
   @param[in]  CB_Param     Callback parameter for the callback function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_PowerConfig_Create_Server(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_PowerConfig_Server_CB_t Event_CB, uint32_t CB_Param);

#endif // ] #ifndef __QAPI_ZB_CL_POWERCONFIG_H__

/** @} */

