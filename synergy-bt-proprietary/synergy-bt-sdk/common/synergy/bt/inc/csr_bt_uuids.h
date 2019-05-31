#ifndef CSR_BT_UUIDS_H__
#define CSR_BT_UUIDS_H__
/******************************************************************************
 Copyright (c) 2010-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/


#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_bt_profiles.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Declarations */
#define CSR_BT_GATT_UUID_DECLRATIONS_START          ((CsrBtUuid16) 0x2800)

#define CSR_BT_GATT_UUID_PRIMARY_SERVICE_DECL       ((CsrBtUuid16) 0x2800) /* Primary Service Declaration */
#define CSR_BT_GATT_UUID_SECONDARY_SERVICE_DECL     ((CsrBtUuid16) 0x2801) /* Secondary Service Declaration */
#define CSR_BT_GATT_UUID_INCLUDE_DECL               ((CsrBtUuid16) 0x2802) /* Include Declaration */
#define CSR_BT_GATT_UUID_CHARACTERISTIC_DECL        ((CsrBtUuid16) 0x2803) /* Characteristic Declaration */

#define CSR_BT_GATT_UUID_DECLRATIONS_END            ((CsrBtUuid16) 0x2803)

/* Characteristic descriptors */
#define CSR_BT_GATT_UUID_DESCRIPTORS_START                            ((CsrBtUuid16) 0x2900)

#define CSR_BT_GATT_UUID_CHARACTERISTIC_EXTENDED_PROPERTIES_DESC      ((CsrBtUuid16) 0x2900) /* Characteristic Extended Properties */
#define CSR_BT_GATT_UUID_CHARACTERISTIC_USER_DESCRIPTION_DESC         ((CsrBtUuid16) 0x2901) /* Characteristic User Description */
#define CSR_BT_GATT_UUID_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC     ((CsrBtUuid16) 0x2902) /* Client Characteristic Configuration */
#define CSR_BT_GATT_UUID_SERVER_CHARACTERISTIC_CONFIGURATION_DESC     ((CsrBtUuid16) 0x2903) /* Server Characteristic Configuration */
#define CSR_BT_GATT_UUID_CHARACTERISTIC_PRESENTATION_FORMAT_DESC      ((CsrBtUuid16) 0x2904) /* Characteristic Presentation Format */
#define CSR_BT_GATT_UUID_CHARACTERISTIC_AGGREGATE_FORMAT_DESC         ((CsrBtUuid16) 0x2905) /* Characteristic Aggregate Format */
#define CSR_BT_GATT_UUID_VALID_RANGE_DESC                             ((CsrBtUuid16) 0x2906) /* Valid Range */
#define CSR_BT_GATT_UUID_EXTERNAL_REPORT_REFERENCE_DESC               ((CsrBtUuid16) 0x2907) /* External Report Reference - Defined by HIDS */
#define CSR_BT_GATT_UUID_REPORT_REFERENCE_DESC                        ((CsrBtUuid16) 0x2908) /* Report Reference - Defined by HIDS */
#define CSR_BT_GATT_UUID_NUMBER_OF_DIGITALS_DESC                      ((CsrBtUuid16) 0x2909) /* Characteristic Number of Digitals */
#define CSR_BT_GATT_UUID_VALUE_TRIGGER_SETTING_DESC                   ((CsrBtUuid16) 0x290A) /* Value Trigger Setting */
#define CSR_BT_GATT_UUID_ENVIRONMENTAL_SENSING_CONFIGURATION_DESC     ((CsrBtUuid16) 0x290B) /* Environmental Sensing Configuration - Defined by ESS */
#define CSR_BT_GATT_UUID_ENVIRONMENTAL_SENSING_MEASUREMENT_DESC       ((CsrBtUuid16) 0x290C) /* Environmental Sensing Measurement - Defined by ESS */
#define CSR_BT_GATT_UUID_ENVIRONMENTAL_SENSING_TRIGGER_SETTING_DESC   ((CsrBtUuid16) 0x290D) /* Environmental Sensing Trigger Setting - Defined by ESS */

#define CSR_BT_GATT_UUID_DESCRIPTORS_END                              ((CsrBtUuid16) 0x290D)

/* Services */
#define CSR_BT_GATT_UUID_SERVICES_START                         ((CsrBtUuid16) 0x1800)

#define CSR_BT_GATT_UUID_GENERIC_ACCESS_SERVICE                 ((CsrBtUuid16) 0x1800) /* Generic Access */
#define CSR_BT_GATT_UUID_GENERIC_ATTRIBUTE_SERVICE              ((CsrBtUuid16) 0x1801) /* Generic Attribute */
#define CSR_BT_GATT_UUID_IMMEDIATE_ALERT_SERVICE                ((CsrBtUuid16) 0x1802) /* Immediate Alert */
#define CSR_BT_GATT_UUID_LINK_LOSS_SERVICE                      ((CsrBtUuid16) 0x1803) /* LinkLoss */
#define CSR_BT_GATT_UUID_TX_POWER_SERVICE                       ((CsrBtUuid16) 0x1804) /* Tx Power */
#define CSR_BT_GATT_UUID_CURRENT_TIME_SERVICE                   ((CsrBtUuid16) 0x1805) /* Current Time Service */
#define CSR_BT_GATT_UUID_REFERENCE_TIME_UPDATE_SERVICE          ((CsrBtUuid16) 0x1806) /* Reference Time Update Service */
#define CSR_BT_GATT_UUID_NEXT_DST_CHANGE_SERVICE                ((CsrBtUuid16) 0x1807) /* Next DST Change Service */
#define CSR_BT_GATT_UUID_GLUCOSE_SERVICE                        ((CsrBtUuid16) 0x1808) /* Glucose */
#define CSR_BT_GATT_UUID_HEALTH_THERMOMETER_SERVICE             ((CsrBtUuid16) 0x1809) /* Health Thermometer */
#define CSR_BT_GATT_UUID_DEVICE_INFORMATION_SERVICE             ((CsrBtUuid16) 0x180A) /* Device Information */
/* 0x180B not defined */
/* 0x180C not defined */
#define CSR_BT_GATT_UUID_HEART_RATE_SERVICE                     ((CsrBtUuid16) 0x180D) /* Heart Rate */
#define CSR_BT_GATT_UUID_PHONE_ALERT_STATUS_SERVICE             ((CsrBtUuid16) 0x180E) /* Phone Alert Status Service */
#define CSR_BT_GATT_UUID_BATTERY_SERVICE                        ((CsrBtUuid16) 0x180F) /* Battery Service */
#define CSR_BT_GATT_UUID_BLOOD_PRESSURE_SERVICE                 ((CsrBtUuid16) 0x1810) /* Blood Pressure */
#define CSR_BT_GATT_UUID_ALERT_NOTIFICATION_SERVICE             ((CsrBtUuid16) 0x1811) /* Alert Notification Service */
#define CSR_BT_GATT_UUID_HUMAN_INTERFACE_DEVICE_SERVICE         ((CsrBtUuid16) 0x1812) /* Human Interface Device */
#define CSR_BT_GATT_UUID_SCAN_PARAMETERS_SERVICE                ((CsrBtUuid16) 0x1813) /* Scan Parameters */
#define CSR_BT_GATT_UUID_RUNNING_SPEED_AND_CADENCE_SERVICE      ((CsrBtUuid16) 0x1814) /* Running Speed and Cadence */
#define CSR_BT_GATT_UUID_AUTOMATION_IO_SERVICE                  ((CsrBtUuid16) 0x1815) /* Automation IO */
#define CSR_BT_GATT_UUID_CYCLING_SPEED_AND_CADENCE_SERVICE      ((CsrBtUuid16) 0x1816) /* Cycling Speed and Cadence */
/* 0x1817 not defined */
#define CSR_BT_GATT_UUID_CYCLING_POWER_SERVICE                  ((CsrBtUuid16) 0x1818) /* Cycling Power */
#define CSR_BT_GATT_UUID_LOCATION_AND_NAVIGATION_SERVICE        ((CsrBtUuid16) 0x1819) /* Location and Navigation */
#define CSR_BT_GATT_UUID_ENVIRONMENTAL_SENSING_SERVICE          ((CsrBtUuid16) 0x181A) /* Environmental Sensing */
#define CSR_BT_GATT_UUID_BODY_COMPOSITION_SERVICE               ((CsrBtUuid16) 0x181B) /* Body Composition */
#define CSR_BT_GATT_UUID_USER_DATA_SERVICE                      ((CsrBtUuid16) 0x181C) /* User Data */
#define CSR_BT_GATT_UUID_WEIGHT_SCALE_SERVICE                   ((CsrBtUuid16) 0x181D) /* Weight Scale */
#define CSR_BT_GATT_UUID_BOND_MANAGEMENT_SERVICE                ((CsrBtUuid16) 0x181E) /* Bond Management */
#define CSR_BT_GATT_UUID_CONTINUOUS_GLUCOSE_MONITORING_SERVICE  ((CsrBtUuid16) 0x181F) /* Continuous Glucose Monitoring */
#define CSR_BT_GATT_UUID_INTERNET_PROTOCOL_SUPPORT_SERVICE      ((CsrBtUuid16) 0x1820) /* Internet Protocol Support */
#define CSR_BT_GATT_UUID_INDOOR_POSITIONING_SERVICE             ((CsrBtUuid16) 0x1821) /* Indoor Positioning */
#define CSR_BT_GATT_UUID_PULSE_OXIMETER_SERVICE                 ((CsrBtUuid16) 0x1822) /* Pulse Oximeter */
#define CSR_BT_GATT_UUID_HTTP_PROXY_SERVICE                     ((CsrBtUuid16) 0x1823) /* HTTP Proxy */
#define CSR_BT_GATT_UUID_TRANSPORT_DISCOVERY_SERVICE            ((CsrBtUuid16) 0x1824) /* Transport Discovery */
#define CSR_BT_GATT_UUID_OBJECT_TRANSFER_SERVICE                ((CsrBtUuid16) 0x1825) /* Object Transfer */

#define CSR_BT_GATT_UUID_SERVICES_END                           ((CsrBtUuid16) 0x1825)

/* Characteristics */
#define CSR_BT_GATT_UUID_CHARACTERISTICS_START                     ((CsrBtUuid16) 0x2A00)

#define CSR_BT_GATT_UUID_DEVICE_NAME_CHARAC                        ((CsrBtUuid16) 0x2A00) /* Device Name */
#define CSR_BT_GATT_UUID_APPEARANCE_CHARAC                         ((CsrBtUuid16) 0x2A01) /* Appearance */
#define CSR_BT_GATT_UUID_PERIPHERAL_PRIVACY_FLAG_CHARAC            ((CsrBtUuid16) 0x2A02) /* Peripheral Privacy Flag */
#define CSR_BT_GATT_UUID_RECONNECTION_ADDRESS_CHARAC               ((CsrBtUuid16) 0x2A03) /* Reconnection Address */
#define CSR_BT_GATT_UUID_PERIPHERAL_PREFERRED_CONN_PARAM_CHARAC    ((CsrBtUuid16) 0x2A04) /* Peripheral Preferred Connection Parameters */
#define CSR_BT_GATT_UUID_SERVICE_CHANGED_CHARAC                    ((CsrBtUuid16) 0x2A05) /* Service Changed */
#define CSR_BT_GATT_UUID_ALERT_LEVEL_CHARAC                        ((CsrBtUuid16) 0x2A06) /* Alert Level */
#define CSR_BT_GATT_UUID_TX_POWER_LEVEL_CHARAC                     ((CsrBtUuid16) 0x2A07) /* Tx Power Level */
#define CSR_BT_GATT_UUID_DATE_TIME_CHARAC                          ((CsrBtUuid16) 0x2A08) /* Date Time */
#define CSR_BT_GATT_UUID_DAY_OF_WEEK_CHARAC                        ((CsrBtUuid16) 0x2A09) /* Day of Week */
#define CSR_BT_GATT_UUID_DAY_DATE_TIME_CHARAC                      ((CsrBtUuid16) 0x2A0A) /* Day Date Time */
/* 0x2A0B not defined */
#define CSR_BT_GATT_UUID_EXACT_TIME_256_CHARAC                     ((CsrBtUuid16) 0x2A0C) /* Exact Time 256 */
#define CSR_BT_GATT_UUID_DST_OFFSET_CHARAC                         ((CsrBtUuid16) 0x2A0D) /* DST Offset */
#define CSR_BT_GATT_UUID_TIME_ZONE_CHARAC                          ((CsrBtUuid16) 0x2A0E) /* Time Zone */
#define CSR_BT_GATT_UUID_LOCAL_TIME_INFORMATION_CHARAC             ((CsrBtUuid16) 0x2A0F) /* Local Time Information */
/* 0x2A10 not defined */
#define CSR_BT_GATT_UUID_TIME_WITH_DST_CHARAC                      ((CsrBtUuid16) 0x2A11) /* Time with DST */
#define CSR_BT_GATT_UUID_TIME_ACCURACY_CHARAC                      ((CsrBtUuid16) 0x2A12) /* Time Accuracy */
#define CSR_BT_GATT_UUID_TIME_SOURCE_CHARAC                        ((CsrBtUuid16) 0x2A13) /* Time Source */
#define CSR_BT_GATT_UUID_REFERENCE_TIME_INFORMATION_CHARAC         ((CsrBtUuid16) 0x2A14) /* Reference Time Information */
/* 0x2A15 not defined */
#define CSR_BT_GATT_UUID_TIME_UPDATE_CONTROL_POINT_CHARAC          ((CsrBtUuid16) 0x2A16) /* Time Update Control Point */
#define CSR_BT_GATT_UUID_TIME_UPDATE_STATE_CHARAC                  ((CsrBtUuid16) 0x2A17) /* Time Update State */
#define CSR_BT_GATT_UUID_GLUCOSE_MEASUREMENT_CHARAC                ((CsrBtUuid16) 0x2A18) /* Glucose Measurement */
#define CSR_BT_GATT_UUID_BATTERY_LEVEL_CHARAC                      ((CsrBtUuid16) 0x2A19) /* Battery Level */
/* 0x2A1A not defined */
/* 0x2A1B not defined */
#define CSR_BT_GATT_UUID_TEMPERATURE_MEASUREMENT_CHARAC            ((CsrBtUuid16) 0x2A1C) /* Temperature Measurement */
#define CSR_BT_GATT_UUID_TEMPERATURE_TYPE_CHARAC                   ((CsrBtUuid16) 0x2A1D) /* Temperature Type */
#define CSR_BT_GATT_UUID_INTERMEDIATE_TEMPERATURE_CHARAC           ((CsrBtUuid16) 0x2A1E) /* Intermediate Temperature */
/* 0x2A1F not defined */
/* 0x2A20 not defined */
#define CSR_BT_GATT_UUID_MEASUREMENT_INTERVAL_CHARAC               ((CsrBtUuid16) 0x2A21) /* Measurement Interval */
#define CSR_BT_GATT_UUID_BOOT_KEYBOARD_INPUT_REPORT_CHARAC         ((CsrBtUuid16) 0x2A22) /* Boot Keyboard Input Report */
#define CSR_BT_GATT_UUID_SYSTEM_ID_CHARAC                          ((CsrBtUuid16) 0x2A23) /* System ID */
#define CSR_BT_GATT_UUID_MODEL_NUMBER_STRING_CHARAC                ((CsrBtUuid16) 0x2A24) /* Model Number String */
#define CSR_BT_GATT_UUID_SERIAL_NUMBER_STRING_CHARAC               ((CsrBtUuid16) 0x2A25) /* Serial Number String */
#define CSR_BT_GATT_UUID_FIRMWARE_REVISION_STRING_CHARAC           ((CsrBtUuid16) 0x2A26) /* Firmware Revision String */
#define CSR_BT_GATT_UUID_HARDWARE_REVISION_STRING_CHARAC           ((CsrBtUuid16) 0x2A27) /* Hardware Revision String */
#define CSR_BT_GATT_UUID_SOFTWARE_REVISION_STRING_CHARAC           ((CsrBtUuid16) 0x2A28) /* Software Revision String */
#define CSR_BT_GATT_UUID_MANUFACTURER_NAME_STRING_CHARAC           ((CsrBtUuid16) 0x2A29) /* Manufacturer Name String */
#define CSR_BT_GATT_UUID_IEEE_REG_CERT_DATA_LIST_CHARAC            ((CsrBtUuid16) 0x2A2A) /* IEEE 11073-20601 Regulatory Certification Data List */
#define CSR_BT_GATT_UUID_CURRENT_TIME_CHARAC                       ((CsrBtUuid16) 0x2A2B) /* Current Time */
#define CSR_BT_GATT_UUID_MAGNETIC_DECLINATION_CHARAC               ((CsrBtUuid16) 0x2A2C) /* Magnetic Declination */
/* 0x2A2D not defined */
/* 0x2A2E not defined */
/* 0x2A2F not defined */
/* 0x2A30 not defined */
#define CSR_BT_GATT_UUID_SCAN_REFRESH_CHARAC                       ((CsrBtUuid16) 0x2A31) /* Scan Refresh */
#define CSR_BT_GATT_UUID_BOOT_KEYBOARD_OUTPUT_REPORT_CHARAC        ((CsrBtUuid16) 0x2A32) /* Boot Keyboard Output Report */
#define CSR_BT_GATT_UUID_BOOT_MOUSE_INPUT_REPORT_CHARAC            ((CsrBtUuid16) 0x2A33) /* Boot Mouse Input Report */
#define CSR_BT_GATT_UUID_GLUCOSE_MEASUREMENT_CONTEXT_CHARAC        ((CsrBtUuid16) 0x2A34) /* Glucose Measurement Context */
#define CSR_BT_GATT_UUID_BLOOD_PRESSURE_MEASUREMENT_CHARAC         ((CsrBtUuid16) 0x2A35) /* Blood Pressure Measurement */
#define CSR_BT_GATT_UUID_INTERMEDIATE_CUFF_PRESSURE_CHARAC         ((CsrBtUuid16) 0x2A36) /* Intermediate Cuff Pressure */
#define CSR_BT_GATT_UUID_HEART_RATE_MEASUREMENT_CHARAC             ((CsrBtUuid16) 0x2A37) /* Heart Rate Measurement */
#define CSR_BT_GATT_UUID_BODY_SENSOR_LOCATION_CHARAC               ((CsrBtUuid16) 0x2A38) /* Body Sensor Location */
#define CSR_BT_GATT_UUID_HEART_RATE_CONTROL_POINT_CHARAC           ((CsrBtUuid16) 0x2A39) /* Heart Rate Control Point */
/* 0x2A3A not defined */
/* 0x2A3B not defined */
/* 0x2A3C not defined */
/* 0x2A3D not defined */
/* 0x2A3E not defined */
#define CSR_BT_GATT_UUID_ALERT_STATUS_CHARAC                       ((CsrBtUuid16) 0x2A3F) /* Alert Status */
#define CSR_BT_GATT_UUID_RINGER_CONTROL_POINT_CHARAC               ((CsrBtUuid16) 0x2A40) /* Ringer Control Point */
#define CSR_BT_GATT_UUID_RINGER_SETTING_CHARAC                     ((CsrBtUuid16) 0x2A41) /* Ringer Setting */
#define CSR_BT_GATT_UUID_ALERT_CATEGORY_ID_BIT_MASK_CHARAC         ((CsrBtUuid16) 0x2A42) /* Alert Category ID Bit Mask */
#define CSR_BT_GATT_UUID_ALERT_CATEGORY_ID_CHARAC                  ((CsrBtUuid16) 0x2A43) /* Alert Category ID */
#define CSR_BT_GATT_UUID_ALERT_NOTIFICATION_CONTROL_POINT_CHARAC   ((CsrBtUuid16) 0x2A44) /* Alert Notification Control Point */
#define CSR_BT_GATT_UUID_UNREAD_ALERT_STATUS_CHARAC                ((CsrBtUuid16) 0x2A45) /* Unread Alert Status */
#define CSR_BT_GATT_UUID_NEW_ALERT_CHARAC                          ((CsrBtUuid16) 0x2A46) /* New Alert */
#define CSR_BT_GATT_UUID_SUPPORTED_NEW_ALERT_CATEGORY_CHARAC       ((CsrBtUuid16) 0x2A47) /* Supported New Alert Category */
#define CSR_BT_GATT_UUID_SUPPORTED_UNREAD_ALERT_CATEGORY_CHARAC    ((CsrBtUuid16) 0x2A48) /* Supported Unread Alert Category */
#define CSR_BT_GATT_UUID_BLOOD_PRESSURE_FEATURE_CHARAC             ((CsrBtUuid16) 0x2A49) /* Blood Pressure Feature */
#define CSR_BT_GATT_UUID_HID_INFORMATION_CHARAC                    ((CsrBtUuid16) 0x2A4A) /* HID Information */
#define CSR_BT_GATT_UUID_REPORT_MAP_CHARAC                         ((CsrBtUuid16) 0x2A4B) /* Report Map */
#define CSR_BT_GATT_UUID_HID_CONTROL_POINT_CHARAC                  ((CsrBtUuid16) 0x2A4C) /* HID Control Point */
#define CSR_BT_GATT_UUID_REPORT_CHARAC                             ((CsrBtUuid16) 0x2A4D) /* Report */
#define CSR_BT_GATT_UUID_PROTOCOL_MODE_CHARAC                      ((CsrBtUuid16) 0x2A4E) /* Protocol Mode */
#define CSR_BT_GATT_UUID_SCAN_INTERVAL_WINDOW_CHARAC               ((CsrBtUuid16) 0x2A4F) /* Scan Interval Window */
#define CSR_BT_GATT_UUID_PNP_ID_CHARAC                             ((CsrBtUuid16) 0x2A50) /* PnP ID */
#define CSR_BT_GATT_UUID_GLUCOSE_FEATURE_CHARAC                    ((CsrBtUuid16) 0x2A51) /* Glucose Feature */
#define CSR_BT_GATT_UUID_RECORD_ACCESS_CONTROL_POINT_CHARAC        ((CsrBtUuid16) 0x2A52) /* Record Access Control Point */
#define CSR_BT_GATT_UUID_RSC_MEASUREMENT_CHARAC                    ((CsrBtUuid16) 0x2A53) /* RSC Measurement */
#define CSR_BT_GATT_UUID_RSC_FEATURE_CHARAC                        ((CsrBtUuid16) 0x2A54) /* RSC Feature */
#define CSR_BT_GATT_UUID_SC_CONTROL_POINT_CHARAC                   ((CsrBtUuid16) 0x2A55) /* SC Control Point */
#define CSR_BT_GATT_UUID_DIGITAL_CHARAC                            ((CsrBtUuid16) 0x2A56) /* Digital */
/* 0x2A57 not defined */
#define CSR_BT_GATT_UUID_ANALOG_CHARAC                             ((CsrBtUuid16) 0x2A58) /* Analog */
/* 0x2A59 not defined */
#define CSR_BT_GATT_UUID_AGGREGATE_CHARAC                          ((CsrBtUuid16) 0x2A5A) /* Aggregate */
#define CSR_BT_GATT_UUID_CSC_MEASUREMENT_CHARAC                    ((CsrBtUuid16) 0x2A5B) /* CSC Measurement */
#define CSR_BT_GATT_UUID_CSC_FEATURE_CHARAC                        ((CsrBtUuid16) 0x2A5C) /* CSC Feature */
#define CSR_BT_GATT_UUID_SENSOR_LOCATION_CHARAC                    ((CsrBtUuid16) 0x2A5D) /* Sensor Location */
#define CSR_BT_GATT_UUID_PLX_SPOT_CHECK_MEASUREMENT_CHARAC         ((CsrBtUuid16) 0x2A5E) /* PLX Spot-Check Measurement */
#define CSR_BT_GATT_UUID_PLX_CONTINUOUS_MEASUREMENT_CHARAC         ((CsrBtUuid16) 0x2A5F) /* PLX Continuous Measurement */
#define CSR_BT_GATT_UUID_PLX_FEATURES_CHARAC                       ((CsrBtUuid16) 0x2A60) /* PLX Features */
/* 0x2A61 not defined */
/* 0x2A62 not defined */
#define CSR_BT_GATT_UUID_CYCLING_POWER_MEASUREMENT_CHARAC          ((CsrBtUuid16) 0x2A63) /* Cycling Power Measurement */
#define CSR_BT_GATT_UUID_CYCLING_POWER_VECTOR_CHARAC               ((CsrBtUuid16) 0x2A64) /* Cycling Power Vector */
#define CSR_BT_GATT_UUID_CYCLING_POWER_FEATURE_CHARAC              ((CsrBtUuid16) 0x2A65) /* Cycling Power Feature */
#define CSR_BT_GATT_UUID_CYCLING_POWER_CONTROL_POINT_CHARAC        ((CsrBtUuid16) 0x2A66) /* Cycling Power Control Point */
#define CSR_BT_GATT_UUID_LOCATION_AND_SPEED_CHARAC                 ((CsrBtUuid16) 0x2A67) /* Location and Speed */
#define CSR_BT_GATT_UUID_NAVIGATION_CHARAC                         ((CsrBtUuid16) 0x2A68) /* Navigation */
#define CSR_BT_GATT_UUID_POSITION_QUALITY_CHARAC                   ((CsrBtUuid16) 0x2A69) /* Position Quality */
#define CSR_BT_GATT_UUID_LN_FEATURE_CHARAC                         ((CsrBtUuid16) 0x2A6A) /* LN Feature */
#define CSR_BT_GATT_UUID_LN_CONTROL_POINT_CHARAC                   ((CsrBtUuid16) 0x2A6B) /* LN Control Point */
#define CSR_BT_GATT_UUID_ELEVATION_CHARAC                          ((CsrBtUuid16) 0x2A6C) /* Elevation */
#define CSR_BT_GATT_UUID_PRESSURE_CHARAC                           ((CsrBtUuid16) 0x2A6D) /* Pressure */
#define CSR_BT_GATT_UUID_TEMPERATURE_CHARAC                        ((CsrBtUuid16) 0x2A6E) /* Temperature */
#define CSR_BT_GATT_UUID_HUMIDITY_CHARAC                           ((CsrBtUuid16) 0x2A6F) /* Humidity */
#define CSR_BT_GATT_UUID_TRUE_WIND_SPEED_CHARAC                    ((CsrBtUuid16) 0x2A70) /* True Wind Speed */
#define CSR_BT_GATT_UUID_TRUE_WIND_DIRECTION_CHARAC                ((CsrBtUuid16) 0x2A71) /* True Wind Direction */
#define CSR_BT_GATT_UUID_APPARENT_WIND_SPEED_CHARAC                ((CsrBtUuid16) 0x2A72) /* Apparent Wind Speed */
#define CSR_BT_GATT_UUID_APPARENT_WIND_DIRECTION__CHARAC           ((CsrBtUuid16) 0x2A73) /* Apparent Wind Direction  */
#define CSR_BT_GATT_UUID_GUST_FACTOR_CHARAC                        ((CsrBtUuid16) 0x2A74) /* Gust Factor */
#define CSR_BT_GATT_UUID_POLLEN_CONCENTRATION_CHARAC               ((CsrBtUuid16) 0x2A75) /* Pollen Concentration */
#define CSR_BT_GATT_UUID_UV_INDEX_CHARAC                           ((CsrBtUuid16) 0x2A76) /* UV Index */
#define CSR_BT_GATT_UUID_IRRADIANCE_CHARAC                         ((CsrBtUuid16) 0x2A77) /* Irradiance */
#define CSR_BT_GATT_UUID_RAINFALL_CHARAC                           ((CsrBtUuid16) 0x2A78) /* Rainfall */
#define CSR_BT_GATT_UUID_WIND_CHILL_CHARAC                         ((CsrBtUuid16) 0x2A79) /* Wind Chill */
#define CSR_BT_GATT_UUID_HEAT_INDEX_CHARAC                         ((CsrBtUuid16) 0x2A7A) /* Heat Index */
#define CSR_BT_GATT_UUID_DEW_POINT_CHARAC                          ((CsrBtUuid16) 0x2A7B) /* Dew Point */
/* 0x2A7C not defined */
#define CSR_BT_GATT_UUID_DESCRIPTOR_VALUE_CHANGED_CHARAC           ((CsrBtUuid16) 0x2A7D) /* Descriptor Value Changed */
#define CSR_BT_GATT_UUID_AEROBIC_HEART_RATE_LOWER_LIMIT_CHARAC     ((CsrBtUuid16) 0x2A7E) /* Aerobic Heart Rate Lower Limit */
#define CSR_BT_GATT_UUID_AEROBIC_THRESHOLD_CHARAC                  ((CsrBtUuid16) 0x2A7F) /* Aerobic Threshold */
#define CSR_BT_GATT_UUID_AGE_CHARAC                                ((CsrBtUuid16) 0x2A80) /* Age */
#define CSR_BT_GATT_UUID_ANAEROBIC_HEART_RATE_LOWER_LIMIT_CHARAC   ((CsrBtUuid16) 0x2A81) /* Anaerobic Heart Rate Lower Limit */
#define CSR_BT_GATT_UUID_ANAEROBIC_HEART_RATE_UPPER_LIMIT_CHARAC   ((CsrBtUuid16) 0x2A82) /* Anaerobic Heart Rate Upper Limit */
#define CSR_BT_GATT_UUID_ANAEROBIC_THRESHOLD_CHARAC                ((CsrBtUuid16) 0x2A83) /* Anaerobic Threshold */
#define CSR_BT_GATT_UUID_AEROBIC_HEART_RATE_UPPER_LIMIT_CHARAC     ((CsrBtUuid16) 0x2A84) /* Aerobic Heart Rate Upper Limit */
#define CSR_BT_GATT_UUID_DATE_OF_BIRTH_CHARAC                      ((CsrBtUuid16) 0x2A85) /* Date of Birth */
#define CSR_BT_GATT_UUID_DATE_OF_THRESHOLD_ASSESSMENT_CHARAC       ((CsrBtUuid16) 0x2A86) /* Date of Threshold Assessment */
#define CSR_BT_GATT_UUID_EMAIL_ADDRESS_CHARAC                      ((CsrBtUuid16) 0x2A87) /* Email Address */
#define CSR_BT_GATT_UUID_FAT_BURN_HEART_RATE_LOWER_LIMIT_CHARAC    ((CsrBtUuid16) 0x2A88) /* Fat Burn Heart Rate Lower Limit */
#define CSR_BT_GATT_UUID_FAT_BURN_HEART_RATE_UPPER_LIMIT_CHARAC    ((CsrBtUuid16) 0x2A89) /* Fat Burn Heart Rate Upper Limit */
#define CSR_BT_GATT_UUID_FIRST_NAME_CHARAC                         ((CsrBtUuid16) 0x2A8A) /* First Name */
#define CSR_BT_GATT_UUID_FIVE_ZONE_HEART_RATE_LIMITS_CHARAC        ((CsrBtUuid16) 0x2A8B) /* Five Zone Heart Rate Limits */
#define CSR_BT_GATT_UUID_GENDER_CHARAC                             ((CsrBtUuid16) 0x2A8C) /* Gender */
#define CSR_BT_GATT_UUID_HEART_RATE_MAX_CHARAC                     ((CsrBtUuid16) 0x2A8D) /* Heart Rate Max */
#define CSR_BT_GATT_UUID_HEIGHT_CHARAC                             ((CsrBtUuid16) 0x2A8E) /* Height */
#define CSR_BT_GATT_UUID_HIP_CIRCUMFERENCE_CHARAC                  ((CsrBtUuid16) 0x2A8F) /* Hip Circumference */
#define CSR_BT_GATT_UUID_LAST_NAME_CHARAC                          ((CsrBtUuid16) 0x2A90) /* Last Name */
#define CSR_BT_GATT_UUID_MAXIMUM_RECOMMENDED_HEART_RATE_CHARAC     ((CsrBtUuid16) 0x2A91) /* Maximum Recommended Heart Rate */
#define CSR_BT_GATT_UUID_RESTING_HEART_RATE_CHARAC                 ((CsrBtUuid16) 0x2A92) /* Resting Heart Rate */
#define CSR_BT_GATT_UUID_SPORT_TYPE_AEROBIC_ANAEROBIC_THRES_CHARAC ((CsrBtUuid16) 0x2A93) /* Sport Type for Aerobic and Anaerobic Thresholds */
#define CSR_BT_GATT_UUID_THREE_ZONE_HEART_RATE_LIMITS_CHARAC       ((CsrBtUuid16) 0x2A94) /* Three Zone Heart Rate Limits */
#define CSR_BT_GATT_UUID_TWO_ZONE_HEART_RATE_LIMIT_CHARAC          ((CsrBtUuid16) 0x2A95) /* Two Zone Heart Rate Limit */
#define CSR_BT_GATT_UUID_VO2_MAX_CHARAC                            ((CsrBtUuid16) 0x2A96) /* VO2 Max */
#define CSR_BT_GATT_UUID_WAIST_CIRCUMFERENCE_CHARAC                ((CsrBtUuid16) 0x2A97) /* Waist Circumference */
#define CSR_BT_GATT_UUID_WEIGHT_CHARAC                             ((CsrBtUuid16) 0x2A98) /* Weight */
#define CSR_BT_GATT_UUID_DATABASE_CHANGE_INCREMENT_CHARAC          ((CsrBtUuid16) 0x2A99) /* Database Change Increment */
#define CSR_BT_GATT_UUID_USER_INDEX_CHARAC                         ((CsrBtUuid16) 0x2A9A) /* User Index */
#define CSR_BT_GATT_UUID_BODY_COMPOSITION_FEATURE_CHARAC           ((CsrBtUuid16) 0x2A9B) /* Body Composition Feature */
#define CSR_BT_GATT_UUID_BODY_COMPOSITION_MEASUREMENT_CHARAC       ((CsrBtUuid16) 0x2A9C) /* Body Composition Measurement */
#define CSR_BT_GATT_UUID_WEIGHT_MEASUREMENT_CHARAC                 ((CsrBtUuid16) 0x2A9D) /* Weight Measurement */
#define CSR_BT_GATT_UUID_WEIGHT_SCALE_FEATURE_CHARAC               ((CsrBtUuid16) 0x2A9E) /* Weight Scale Feature */
#define CSR_BT_GATT_UUID_USER_CONTROL_POINT_CHARAC                 ((CsrBtUuid16) 0x2A9F) /* User Control Point */
#define CSR_BT_GATT_UUID_MAGNETIC_FLUX_DENSITY_2D_CHARAC           ((CsrBtUuid16) 0x2AA0) /* Magnetic Flux Density - 2D */
#define CSR_BT_GATT_UUID_MAGNETIC_FLUX_DENSITY_3D_CHARAC           ((CsrBtUuid16) 0x2AA1) /* Magnetic Flux Density - 3D */
#define CSR_BT_GATT_UUID_LANGUAGE_CHARAC                           ((CsrBtUuid16) 0x2AA2) /* Language */
#define CSR_BT_GATT_UUID_BAROMETRIC_PRESSURE_TREND_CHARAC          ((CsrBtUuid16) 0x2AA3) /* Barometric Pressure Trend */
#define CSR_BT_GATT_UUID_BOND_MANAGEMENT_CONTROL_POINT_CHARAC      ((CsrBtUuid16) 0x2AA4) /* Bond Management Control Point */
#define CSR_BT_GATT_UUID_BOND_MANAGEMENT_FEATURE_CHARAC            ((CsrBtUuid16) 0x2AA5) /* Bond Management Feature */
#define CSR_BT_GATT_UUID_CENTRAL_ADDRESS_RESOLUTION_CHARAC         ((CsrBtUuid16) 0x2AA6) /* Central Address Resolution */
#define CSR_BT_GATT_UUID_CGM_MEASUREMENT_CHARAC                    ((CsrBtUuid16) 0x2AA7) /* CGM Measurement */
#define CSR_BT_GATT_UUID_CGM_FEATURE_CHARAC                        ((CsrBtUuid16) 0x2AA8) /* CGM Feature */
#define CSR_BT_GATT_UUID_CGM_STATUS_CHARAC                         ((CsrBtUuid16) 0x2AA9) /* CGM Status */
#define CSR_BT_GATT_UUID_CGM_SESSION_START_TIME_CHARAC             ((CsrBtUuid16) 0x2AAA) /* CGM Session Start Time */
#define CSR_BT_GATT_UUID_CGM_SESSION_RUN_TIME_CHARAC               ((CsrBtUuid16) 0x2AAB) /* CGM Session Run Time */
#define CSR_BT_GATT_UUID_CGM_SPECIFIC_OPS_CONTROL_POINT_CHARAC     ((CsrBtUuid16) 0x2AAC) /* CGM Specific Ops Control Point */
#define CSR_BT_GATT_UUID_INDOOR_POSITIONING_CONFIGURATION_CHARAC   ((CsrBtUuid16) 0x2AAD) /* Indoor Positioning Configuration */
#define CSR_BT_GATT_UUID_LATITUDE_CHARAC                           ((CsrBtUuid16) 0x2AAE) /* Latitude */
#define CSR_BT_GATT_UUID_LONGITUDE_CHARAC                          ((CsrBtUuid16) 0x2AAF) /* Longitude */
#define CSR_BT_GATT_UUID_LOCAL_NORTH_COORDINATE_CHARAC             ((CsrBtUuid16) 0x2AB0) /* Local North Coordinate */
#define CSR_BT_GATT_UUID_LOCAL_EAST_COORDINATE_CHARAC              ((CsrBtUuid16) 0x2AB1) /* Local East Coordinate */
#define CSR_BT_GATT_UUID_FLOOR_NUMBER_CHARAC                       ((CsrBtUuid16) 0x2AB2) /* Floor Number */
#define CSR_BT_GATT_UUID_ALTITUDE_CHARAC                           ((CsrBtUuid16) 0x2AB3) /* Altitude */
#define CSR_BT_GATT_UUID_UNCERTAINTY_CHARAC                        ((CsrBtUuid16) 0x2AB4) /* Uncertainty */
#define CSR_BT_GATT_UUID_LOCATION_NAME_CHARAC                      ((CsrBtUuid16) 0x2AB5) /* Location Name */
#define CSR_BT_GATT_UUID_URI_CHARAC                                ((CsrBtUuid16) 0x2AB6) /* URI */
#define CSR_BT_GATT_UUID_HTTP_HEADERS_CHARAC                       ((CsrBtUuid16) 0x2AB7) /* HTTP Headers */
#define CSR_BT_GATT_UUID_HTTP_STATUS_CODE_CHARAC                   ((CsrBtUuid16) 0x2AB8) /* HTTP Status Code */
#define CSR_BT_GATT_UUID_HTTP_ENTITY_BODY_CHARAC                   ((CsrBtUuid16) 0x2AB9) /* HTTP Entity Body */
#define CSR_BT_GATT_UUID_HTTP_CONTROL_POINT_CHARAC                 ((CsrBtUuid16) 0x2ABA) /* HTTP Control Point */
#define CSR_BT_GATT_UUID_HTTPS_SECURITY_CHARAC                     ((CsrBtUuid16) 0x2ABB) /* HTTPS Security */
#define CSR_BT_GATT_UUID_TDS_CONTROL_POINT_CHARAC                  ((CsrBtUuid16) 0x2ABC) /* TDS Control Point */
#define CSR_BT_GATT_UUID_OTS_FEATURE_CHARAC                        ((CsrBtUuid16) 0x2ABD) /* OTS Feature */
#define CSR_BT_GATT_UUID_OBJECT_NAME_CHARAC                        ((CsrBtUuid16) 0x2ABE) /* Object Name */
#define CSR_BT_GATT_UUID_OBJECT_TYPE_CHARAC                        ((CsrBtUuid16) 0x2ABF) /* Object Type */
#define CSR_BT_GATT_UUID_OBJECT_SIZE_CHARAC                        ((CsrBtUuid16) 0x2AC0) /* Object Size */
#define CSR_BT_GATT_UUID_OBJECT_FIRST_CREATED_CHARAC               ((CsrBtUuid16) 0x2AC1) /* Object First-Created */
#define CSR_BT_GATT_UUID_OBJECT_LAST_MODIFIED_CHARAC               ((CsrBtUuid16) 0x2AC2) /* Object Last-Modified */
#define CSR_BT_GATT_UUID_OBJECT_ID_CHARAC                          ((CsrBtUuid16) 0x2AC3) /* Object ID */
#define CSR_BT_GATT_UUID_OBJECT_PROPERTIES_CHARAC                  ((CsrBtUuid16) 0x2AC4) /* Object Properties */
#define CSR_BT_GATT_UUID_OBJECT_ACTION_CONTROL_POINT_CHARAC        ((CsrBtUuid16) 0x2AC5) /* Object Action Control Point */
#define CSR_BT_GATT_UUID_OBJECT_LIST_CONTROL_POINT_CHARAC          ((CsrBtUuid16) 0x2AC6) /* Object List Control Point */
#define CSR_BT_GATT_UUID_OBJECT_LIST_FILTER_CHARAC                 ((CsrBtUuid16) 0x2AC7) /* Object List Filter */
#define CSR_BT_GATT_UUID_OBJECT_CHANGED_CHARAC                     ((CsrBtUuid16) 0x2AC8) /* Object Changed */
#define CSR_BT_GATT_UUID_RESOLVABLE_PRIVATE_ADDRESS_ONLY_CHARAC    ((CsrBtUuid16) 0x2AC9) /* Resolvable Private address only */

#define CSR_BT_GATT_UUID_CHARACTERISTICS_END                       ((CsrBtUuid16) 0x2AC9)

/************************* GATT Declaration names *****************************/
#define CSR_BT_GATT_UUID_PRIMARY_SERVICE_DECL_STR       "Primary Service Declaration"
#define CSR_BT_GATT_UUID_SECONDARY_SERVICE_DECL_STR     "Secondary Service Declaration"
#define CSR_BT_GATT_UUID_INCLUDE_DECL_STR               "Include Declaration"
#define CSR_BT_GATT_UUID_CHARACTERISTIC_DECL_STR        "Characteristic Declaration"

/************************* GATT descriptor names ******************************/
#define CSR_BT_GATT_UUID_CHARACTERISTIC_EXTENDED_PROPERTIES_DESC_STR      "Characteristic Extended Properties"
#define CSR_BT_GATT_UUID_CHARACTERISTIC_USER_DESC_STRRIPTION_DESC_STR     "Characteristic User Description"
#define CSR_BT_GATT_UUID_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_STR     "Client Characteristic Configuration"
#define CSR_BT_GATT_UUID_SERVER_CHARACTERISTIC_CONFIGURATION_DESC_STR     "Server Characteristic Configuration"
#define CSR_BT_GATT_UUID_CHARACTERISTIC_PRESENTATION_FORMAT_DESC_STR      "Characteristic Presentation Format"
#define CSR_BT_GATT_UUID_CHARACTERISTIC_AGGREGATE_FORMAT_DESC_STR         "Characteristic Aggregate Format"
#define CSR_BT_GATT_UUID_VALID_RANGE_DESC_STR                             "Valid Range"
#define CSR_BT_GATT_UUID_EXTERNAL_REPORT_REFERENCE_DESC_STR               "External Report Reference"
#define CSR_BT_GATT_UUID_REPORT_REFERENCE_DESC_STR                        "Report Reference"
#define CSR_BT_GATT_UUID_NUMBER_OF_DIGITALS_DESC_STR                      "Characteristic Number of Digitals"
#define CSR_BT_GATT_UUID_VALUE_TRIGGER_SETTING_DESC_STR                   "Value Trigger Setting"
#define CSR_BT_GATT_UUID_ENVIRONMENTAL_SENSING_CONFIGURATION_DESC_STR     "Environmental Sensing Configuration"
#define CSR_BT_GATT_UUID_ENVIRONMENTAL_SENSING_MEASUREMENT_DESC_STR       "Environmental Sensing Measurement"
#define CSR_BT_GATT_UUID_ENVIRONMENTAL_SENSING_TRIGGER_SETTING_DESC_STR   "Environmental Sensing Trigger Setting"

/************************** GATT service names ********************************/
#define CSR_BT_GATT_UUID_GENERIC_ACCESS_SERVICE_STR                 "Generic Access"
#define CSR_BT_GATT_UUID_GENERIC_ATTRIBUTE_SERVICE_STR              "Generic Attribute"
#define CSR_BT_GATT_UUID_IMMEDIATE_ALERT_SERVICE_STR                "Immediate Alert"
#define CSR_BT_GATT_UUID_LINK_LOSS_SERVICE_STR                      "LinkLoss"
#define CSR_BT_GATT_UUID_TX_POWER_SERVICE_STR                       "Tx Power"
#define CSR_BT_GATT_UUID_CURRENT_TIME_SERVICE_STR                   "Current Time Service"
#define CSR_BT_GATT_UUID_REFERENCE_TIME_UPDATE_SERVICE_STR          "Reference Time Update Service"
#define CSR_BT_GATT_UUID_NEXT_DST_CHANGE_SERVICE_STR                "Next DST Change Service"
#define CSR_BT_GATT_UUID_GLUCOSE_SERVICE_STR                        "Glucose"
#define CSR_BT_GATT_UUID_HEALTH_THERMOMETER_SERVICE_STR             "Health Thermometer"
#define CSR_BT_GATT_UUID_DEVICE_INFORMATION_SERVICE_STR             "Device Information"
/* 0x180B not defined */
/* 0x180C not defined */
#define CSR_BT_GATT_UUID_HEART_RATE_SERVICE_STR                     "Heart Rate"
#define CSR_BT_GATT_UUID_PHONE_ALERT_STATUS_SERVICE_STR             "Phone Alert Status Service"
#define CSR_BT_GATT_UUID_BATTERY_SERVICE_STR                        "Battery Service"
#define CSR_BT_GATT_UUID_BLOOD_PRESSURE_SERVICE_STR                 "Blood Pressure"
#define CSR_BT_GATT_UUID_ALERT_NOTIFICATION_SERVICE_STR             "Alert Notification Service"
#define CSR_BT_GATT_UUID_HUMAN_INTERFACE_DEVICE_SERVICE_STR         "Human Interface Device"
#define CSR_BT_GATT_UUID_SCAN_PARAMETERS_SERVICE_STR                "Scan Parameters"
#define CSR_BT_GATT_UUID_RUNNING_SPEED_AND_CADENCE_SERVICE_STR      "Running Speed and Cadence"
#define CSR_BT_GATT_UUID_AUTOMATION_IO_SERVICE_STR                  "Automation IO"
#define CSR_BT_GATT_UUID_CYCLING_SPEED_AND_CADENCE_SERVICE_STR      "Cycling Speed and Cadence"
/* 0x1817 not defined */
#define CSR_BT_GATT_UUID_CYCLING_POWER_SERVICE_STR                  "Cycling Power"
#define CSR_BT_GATT_UUID_LOCATION_AND_NAVIGATION_SERVICE_STR        "Location and Navigation"
#define CSR_BT_GATT_UUID_ENVIRONMENTAL_SENSING_SERVICE_STR          "Environmental Sensing"
#define CSR_BT_GATT_UUID_BODY_COMPOSITION_SERVICE_STR               "Body Composition"
#define CSR_BT_GATT_UUID_USER_DATA_SERVICE_STR                      "User Data"
#define CSR_BT_GATT_UUID_WEIGHT_SCALE_SERVICE_STR                   "Weight Scale"
#define CSR_BT_GATT_UUID_BOND_MANAGEMENT_SERVICE_STR                "Bond Management"
#define CSR_BT_GATT_UUID_CONTINUOUS_GLUCOSE_MONITORING_SERVICE_STR  "Continuous Glucose Monitoring"
#define CSR_BT_GATT_UUID_INTERNET_PROTOCOL_SUPPORT_SERVICE_STR      "Internet Protocol Support"
#define CSR_BT_GATT_UUID_INDOOR_POSITIONING_SERVICE_STR             "Indoor Positioning"
#define CSR_BT_GATT_UUID_PULSE_OXIMETER_SERVICE_STR                 "Pulse Oximeter"
#define CSR_BT_GATT_UUID_HTTP_PROXY_SERVICE_STR                     "HTTP Proxy"
#define CSR_BT_GATT_UUID_TRANSPORT_DISCOVERY_SERVICE_STR            "Transport Discovery"
#define CSR_BT_GATT_UUID_OBJECT_TRANSFER_SERVICE_STR                "Object Transfer"

/************************* GATT characteristic names **************************/
#define CSR_BT_GATT_UUID_DEVICE_NAME_CHARAC_STR                        "Device Name"
#define CSR_BT_GATT_UUID_APPEARANCE_CHARAC_STR                         "Appearance"
#define CSR_BT_GATT_UUID_PERIPHERAL_PRIVACY_FLAG_CHARAC_STR            "Peripheral Privacy Flag"
#define CSR_BT_GATT_UUID_RECONNECTION_ADDRESS_CHARAC_STR               "Reconnection Address"
#define CSR_BT_GATT_UUID_PERIPHERAL_PREFERRED_CONN_PARAM_CHARAC_STR    "Peripheral Preferred Connection Parameters"
#define CSR_BT_GATT_UUID_SERVICE_CHANGED_CHARAC_STR                    "Service Changed"
#define CSR_BT_GATT_UUID_ALERT_LEVEL_CHARAC_STR                        "Alert Level"
#define CSR_BT_GATT_UUID_TX_POWER_LEVEL_CHARAC_STR                     "Tx Power Level"
#define CSR_BT_GATT_UUID_DATE_TIME_CHARAC_STR                          "Date Time"
#define CSR_BT_GATT_UUID_DAY_OF_WEEK_CHARAC_STR                        "Day of Week"
#define CSR_BT_GATT_UUID_DAY_DATE_TIME_CHARAC_STR                      "Day Date Time"
/* 0x2A0B not defined */
#define CSR_BT_GATT_UUID_EXACT_TIME_256_CHARAC_STR                     "Exact Time 256"
#define CSR_BT_GATT_UUID_DST_OFFSET_CHARAC_STR                         "DST Offset"
#define CSR_BT_GATT_UUID_TIME_ZONE_CHARAC_STR                          "Time Zone"
#define CSR_BT_GATT_UUID_LOCAL_TIME_INFORMATION_CHARAC_STR             "Local Time Information"
/* 0x2A10 not defined */
#define CSR_BT_GATT_UUID_TIME_WITH_DST_CHARAC_STR                      "Time with DST"
#define CSR_BT_GATT_UUID_TIME_ACCURACY_CHARAC_STR                      "Time Accuracy"
#define CSR_BT_GATT_UUID_TIME_SOURCE_CHARAC_STR                        "Time Source"
#define CSR_BT_GATT_UUID_REFERENCE_TIME_INFORMATION_CHARAC_STR         "Reference Time Information"
/* 0x2A15 not defined */
#define CSR_BT_GATT_UUID_TIME_UPDATE_CONTROL_POINT_CHARAC_STR          "Time Update Control Point"
#define CSR_BT_GATT_UUID_TIME_UPDATE_STATE_CHARAC_STR                  "Time Update State"
#define CSR_BT_GATT_UUID_GLUCOSE_MEASUREMENT_CHARAC_STR                "Glucose Measurement"
#define CSR_BT_GATT_UUID_BATTERY_LEVEL_CHARAC_STR                      "Battery Level"
/* 0x2A1A not defined */
/* 0x2A1B not defined */
#define CSR_BT_GATT_UUID_TEMPERATURE_MEASUREMENT_CHARAC_STR            "Temperature Measurement"
#define CSR_BT_GATT_UUID_TEMPERATURE_TYPE_CHARAC_STR                   "Temperature Type"
#define CSR_BT_GATT_UUID_INTERMEDIATE_TEMPERATURE_CHARAC_STR           "Intermediate Temperature"
/* 0x2A1F not defined */
/* 0x2A20 not defined */
#define CSR_BT_GATT_UUID_MEASUREMENT_INTERVAL_CHARAC_STR               "Measurement Interval"
#define CSR_BT_GATT_UUID_BOOT_KEYBOARD_INPUT_REPORT_CHARAC_STR         "Boot Keyboard Input Report"
#define CSR_BT_GATT_UUID_SYSTEM_ID_CHARAC_STR                          "System ID"
#define CSR_BT_GATT_UUID_MODEL_NUMBER_STRING_CHARAC_STR                "Model Number String"
#define CSR_BT_GATT_UUID_SERIAL_NUMBER_STRING_CHARAC_STR               "Serial Number String"
#define CSR_BT_GATT_UUID_FIRMWARE_REVISION_STRING_CHARAC_STR           "Firmware Revision String"
#define CSR_BT_GATT_UUID_HARDWARE_REVISION_STRING_CHARAC_STR           "Hardware Revision String"
#define CSR_BT_GATT_UUID_SOFTWARE_REVISION_STRING_CHARAC_STR           "Software Revision String"
#define CSR_BT_GATT_UUID_MANUFACTURER_NAME_STRING_CHARAC_STR           "Manufacturer Name String"
#define CSR_BT_GATT_UUID_IEEE_REG_CERT_DATA_LIST_CHARAC_STR            "IEEE 11073-20601 Regulatory Certification Data List"
#define CSR_BT_GATT_UUID_CURRENT_TIME_CHARAC_STR                       "Current Time"
#define CSR_BT_GATT_UUID_MAGNETIC_DECLINATION_CHARAC_STR               "Magnetic Declination"
/* 0x2A2D not defined */
/* 0x2A2E not defined */
/* 0x2A2F not defined */
/* 0x2A30 not defined */
#define CSR_BT_GATT_UUID_SCAN_REFRESH_CHARAC_STR                       "Scan Refresh"
#define CSR_BT_GATT_UUID_BOOT_KEYBOARD_OUTPUT_REPORT_CHARAC_STR        "Boot Keyboard Output Report"
#define CSR_BT_GATT_UUID_BOOT_MOUSE_INPUT_REPORT_CHARAC_STR            "Boot Mouse Input Report"
#define CSR_BT_GATT_UUID_GLUCOSE_MEASUREMENT_CONTEXT_CHARAC_STR        "Glucose Measurement Context"
#define CSR_BT_GATT_UUID_BLOOD_PRESSURE_MEASUREMENT_CHARAC_STR         "Blood Pressure Measurement"
#define CSR_BT_GATT_UUID_INTERMEDIATE_CUFF_PRESSURE_CHARAC_STR         "Intermediate Cuff Pressure"
#define CSR_BT_GATT_UUID_HEART_RATE_MEASUREMENT_CHARAC_STR             "Heart Rate Measurement"
#define CSR_BT_GATT_UUID_BODY_SENSOR_LOCATION_CHARAC_STR               "Body Sensor Location"
#define CSR_BT_GATT_UUID_HEART_RATE_CONTROL_POINT_CHARAC_STR           "Heart Rate Control Point"
/* 0x2A3A not defined */
/* 0x2A3B not defined */
/* 0x2A3C not defined */
/* 0x2A3D not defined */
/* 0x2A3E not defined */
#define CSR_BT_GATT_UUID_ALERT_STATUS_CHARAC_STR                       "Alert Status"
#define CSR_BT_GATT_UUID_RINGER_CONTROL_POINT_CHARAC_STR               "Ringer Control Point"
#define CSR_BT_GATT_UUID_RINGER_SETTING_CHARAC_STR                     "Ringer Setting"
#define CSR_BT_GATT_UUID_ALERT_CATEGORY_ID_BIT_MASK_CHARAC_STR         "Alert Category ID Bit Mask"
#define CSR_BT_GATT_UUID_ALERT_CATEGORY_ID_CHARAC_STR                  "Alert Category ID"
#define CSR_BT_GATT_UUID_ALERT_NOTIFICATION_CONTROL_POINT_CHARAC_STR   "Alert Notification Control Point"
#define CSR_BT_GATT_UUID_UNREAD_ALERT_STATUS_CHARAC_STR                "Unread Alert Status"
#define CSR_BT_GATT_UUID_NEW_ALERT_CHARAC_STR                          "New Alert"
#define CSR_BT_GATT_UUID_SUPPORTED_NEW_ALERT_CATEGORY_CHARAC_STR       "Supported New Alert Category"
#define CSR_BT_GATT_UUID_SUPPORTED_UNREAD_ALERT_CATEGORY_CHARAC_STR    "Supported Unread Alert Category"
#define CSR_BT_GATT_UUID_BLOOD_PRESSURE_FEATURE_CHARAC_STR             "Blood Pressure Feature"
#define CSR_BT_GATT_UUID_HID_INFORMATION_CHARAC_STR                    "HID Information"
#define CSR_BT_GATT_UUID_REPORT_MAP_CHARAC_STR                         "Report Map"
#define CSR_BT_GATT_UUID_HID_CONTROL_POINT_CHARAC_STR                  "HID Control Point"
#define CSR_BT_GATT_UUID_REPORT_CHARAC_STR                             "Report"
#define CSR_BT_GATT_UUID_PROTOCOL_MODE_CHARAC_STR                      "Protocol Mode"
#define CSR_BT_GATT_UUID_SCAN_INTERVAL_WINDOW_CHARAC_STR               "Scan Interval Window"
#define CSR_BT_GATT_UUID_PNP_ID_CHARAC_STR                             "PnP ID"
#define CSR_BT_GATT_UUID_GLUCOSE_FEATURE_CHARAC_STR                    "Glucose Feature"
#define CSR_BT_GATT_UUID_RECORD_ACCESS_CONTROL_POINT_CHARAC_STR        "Record Access Control Point"
#define CSR_BT_GATT_UUID_RSC_MEASUREMENT_CHARAC_STR                    "RSC Measurement"
#define CSR_BT_GATT_UUID_RSC_FEATURE_CHARAC_STR                        "RSC Feature"
#define CSR_BT_GATT_UUID_SC_CONTROL_POINT_CHARAC_STR                   "SC Control Point"
#define CSR_BT_GATT_UUID_DIGITAL_CHARAC_STR                            "Digital"
/* 0x2A57 not defined */
#define CSR_BT_GATT_UUID_ANALOG_CHARAC_STR                             "Analog"
/* 0x2A59 not defined */
#define CSR_BT_GATT_UUID_AGGREGATE_CHARAC_STR                          "Aggregate"
#define CSR_BT_GATT_UUID_CSC_MEASUREMENT_CHARAC_STR                    "CSC Measurement"
#define CSR_BT_GATT_UUID_CSC_FEATURE_CHARAC_STR                        "CSC Feature"
#define CSR_BT_GATT_UUID_SENSOR_LOCATION_CHARAC_STR                    "Sensor Location"
#define CSR_BT_GATT_UUID_PLX_SPOT_CHECK_MEASUREMENT_CHARAC_STR         "PLX Spot-Check Measurement"
#define CSR_BT_GATT_UUID_PLX_CONTINUOUS_MEASUREMENT_CHARAC_STR         "PLX Continuous Measurement"
#define CSR_BT_GATT_UUID_PLX_FEATURES_CHARAC_STR                       "PLX Features"
/* 0x2A61 not defined */
/* 0x2A62 not defined */
#define CSR_BT_GATT_UUID_CYCLING_POWER_MEASUREMENT_CHARAC_STR          "Cycling Power Measurement"
#define CSR_BT_GATT_UUID_CYCLING_POWER_VECTOR_CHARAC_STR               "Cycling Power Vector"
#define CSR_BT_GATT_UUID_CYCLING_POWER_FEATURE_CHARAC_STR              "Cycling Power Feature"
#define CSR_BT_GATT_UUID_CYCLING_POWER_CONTROL_POINT_CHARAC_STR        "Cycling Power Control Point"
#define CSR_BT_GATT_UUID_LOCATION_AND_SPEED_CHARAC_STR                 "Location and Speed"
#define CSR_BT_GATT_UUID_NAVIGATION_CHARAC_STR                         "Navigation"
#define CSR_BT_GATT_UUID_POSITION_QUALITY_CHARAC_STR                   "Position Quality"
#define CSR_BT_GATT_UUID_LN_FEATURE_CHARAC_STR                         "LN Feature"
#define CSR_BT_GATT_UUID_LN_CONTROL_POINT_CHARAC_STR                   "LN Control Point"
#define CSR_BT_GATT_UUID_ELEVATION_CHARAC_STR                          "Elevation"
#define CSR_BT_GATT_UUID_PRESSURE_CHARAC_STR                           "Pressure"
#define CSR_BT_GATT_UUID_TEMPERATURE_CHARAC_STR                        "Temperature"
#define CSR_BT_GATT_UUID_HUMIDITY_CHARAC_STR                           "Humidity"
#define CSR_BT_GATT_UUID_TRUE_WIND_SPEED_CHARAC_STR                    "True Wind Speed"
#define CSR_BT_GATT_UUID_TRUE_WIND_DIRECTION_CHARAC_STR                "True Wind Direction"
#define CSR_BT_GATT_UUID_APPARENT_WIND_SPEED_CHARAC_STR                "Apparent Wind Speed"
#define CSR_BT_GATT_UUID_APPARENT_WIND_DIRECTION__CHARAC_STR           "Apparent Wind Direction "
#define CSR_BT_GATT_UUID_GUST_FACTOR_CHARAC_STR                        "Gust Factor"
#define CSR_BT_GATT_UUID_POLLEN_CONCENTRATION_CHARAC_STR               "Pollen Concentration"
#define CSR_BT_GATT_UUID_UV_INDEX_CHARAC_STR                           "UV Index"
#define CSR_BT_GATT_UUID_IRRADIANCE_CHARAC_STR                         "Irradiance"
#define CSR_BT_GATT_UUID_RAINFALL_CHARAC_STR                           "Rainfall"
#define CSR_BT_GATT_UUID_WIND_CHILL_CHARAC_STR                         "Wind Chill"
#define CSR_BT_GATT_UUID_HEAT_INDEX_CHARAC_STR                         "Heat Index"
#define CSR_BT_GATT_UUID_DEW_POINT_CHARAC_STR                          "Dew Point"
/* 0x2A7C not defined */
#define CSR_BT_GATT_UUID_DESCRIPTOR_VALUE_CHANGED_CHARAC_STR           "Descriptor Value Changed"
#define CSR_BT_GATT_UUID_AEROBIC_HEART_RATE_LOWER_LIMIT_CHARAC_STR     "Aerobic Heart Rate Lower Limit"
#define CSR_BT_GATT_UUID_AEROBIC_THRESHOLD_CHARAC_STR                  "Aerobic Threshold"
#define CSR_BT_GATT_UUID_AGE_CHARAC_STR                                "Age"
#define CSR_BT_GATT_UUID_ANAEROBIC_HEART_RATE_LOWER_LIMIT_CHARAC_STR   "Anaerobic Heart Rate Lower Limit"
#define CSR_BT_GATT_UUID_ANAEROBIC_HEART_RATE_UPPER_LIMIT_CHARAC_STR   "Anaerobic Heart Rate Upper Limit"
#define CSR_BT_GATT_UUID_ANAEROBIC_THRESHOLD_CHARAC_STR                "Anaerobic Threshold"
#define CSR_BT_GATT_UUID_AEROBIC_HEART_RATE_UPPER_LIMIT_CHARAC_STR     "Aerobic Heart Rate Upper Limit"
#define CSR_BT_GATT_UUID_DATE_OF_BIRTH_CHARAC_STR                      "Date of Birth"
#define CSR_BT_GATT_UUID_DATE_OF_THRESHOLD_ASSESSMENT_CHARAC_STR       "Date of Threshold Assessment"
#define CSR_BT_GATT_UUID_EMAIL_ADDRESS_CHARAC_STR                      "Email Address"
#define CSR_BT_GATT_UUID_FAT_BURN_HEART_RATE_LOWER_LIMIT_CHARAC_STR    "Fat Burn Heart Rate Lower Limit"
#define CSR_BT_GATT_UUID_FAT_BURN_HEART_RATE_UPPER_LIMIT_CHARAC_STR    "Fat Burn Heart Rate Upper Limit"
#define CSR_BT_GATT_UUID_FIRST_NAME_CHARAC_STR                         "First Name"
#define CSR_BT_GATT_UUID_FIVE_ZONE_HEART_RATE_LIMITS_CHARAC_STR        "Five Zone Heart Rate Limits"
#define CSR_BT_GATT_UUID_GENDER_CHARAC_STR                             "Gender"
#define CSR_BT_GATT_UUID_HEART_RATE_MAX_CHARAC_STR                     "Heart Rate Max"
#define CSR_BT_GATT_UUID_HEIGHT_CHARAC_STR                             "Height"
#define CSR_BT_GATT_UUID_HIP_CIRCUMFERENCE_CHARAC_STR                  "Hip Circumference"
#define CSR_BT_GATT_UUID_LAST_NAME_CHARAC_STR                          "Last Name"
#define CSR_BT_GATT_UUID_MAXIMUM_RECOMMENDED_HEART_RATE_CHARAC_STR     "Maximum Recommended Heart Rate"
#define CSR_BT_GATT_UUID_RESTING_HEART_RATE_CHARAC_STR                 "Resting Heart Rate"
#define CSR_BT_GATT_UUID_SPORT_TYPE_AEROBIC_ANAEROBIC_THRES_CHARAC_STR "Sport Type for Aerobic and Anaerobic Thresholds"
#define CSR_BT_GATT_UUID_THREE_ZONE_HEART_RATE_LIMITS_CHARAC_STR       "Three Zone Heart Rate Limits"
#define CSR_BT_GATT_UUID_TWO_ZONE_HEART_RATE_LIMIT_CHARAC_STR          "Two Zone Heart Rate Limit"
#define CSR_BT_GATT_UUID_VO2_MAX_CHARAC_STR                            "VO2 Max"
#define CSR_BT_GATT_UUID_WAIST_CIRCUMFERENCE_CHARAC_STR                "Waist Circumference"
#define CSR_BT_GATT_UUID_WEIGHT_CHARAC_STR                             "Weight"
#define CSR_BT_GATT_UUID_DATABASE_CHANGE_INCREMENT_CHARAC_STR          "Database Change Increment"
#define CSR_BT_GATT_UUID_USER_INDEX_CHARAC_STR                         "User Index"
#define CSR_BT_GATT_UUID_BODY_COMPOSITION_FEATURE_CHARAC_STR           "Body Composition Feature"
#define CSR_BT_GATT_UUID_BODY_COMPOSITION_MEASUREMENT_CHARAC_STR       "Body Composition Measurement"
#define CSR_BT_GATT_UUID_WEIGHT_MEASUREMENT_CHARAC_STR                 "Weight Measurement"
#define CSR_BT_GATT_UUID_WEIGHT_SCALE_FEATURE_CHARAC_STR               "Weight Scale Feature"
#define CSR_BT_GATT_UUID_USER_CONTROL_POINT_CHARAC_STR                 "User Control Point"
#define CSR_BT_GATT_UUID_MAGNETIC_FLUX_DENSITY_2D_CHARAC_STR           "Magnetic Flux Density - 2D"
#define CSR_BT_GATT_UUID_MAGNETIC_FLUX_DENSITY_3D_CHARAC_STR           "Magnetic Flux Density - 3D"
#define CSR_BT_GATT_UUID_LANGUAGE_CHARAC_STR                           "Language"
#define CSR_BT_GATT_UUID_BAROMETRIC_PRESSURE_TREND_CHARAC_STR          "Barometric Pressure Trend"
#define CSR_BT_GATT_UUID_BOND_MANAGEMENT_CONTROL_POINT_CHARAC_STR      "Bond Management Control Point"
#define CSR_BT_GATT_UUID_BOND_MANAGEMENT_FEATURE_CHARAC_STR            "Bond Management Feature"
#define CSR_BT_GATT_UUID_CENTRAL_ADDRESS_RESOLUTION_CHARAC_STR         "Central Address Resolution"
#define CSR_BT_GATT_UUID_CGM_MEASUREMENT_CHARAC_STR                    "CGM Measurement"
#define CSR_BT_GATT_UUID_CGM_FEATURE_CHARAC_STR                        "CGM Feature"
#define CSR_BT_GATT_UUID_CGM_STATUS_CHARAC_STR                         "CGM Status"
#define CSR_BT_GATT_UUID_CGM_SESSION_START_TIME_CHARAC_STR             "CGM Session Start Time"
#define CSR_BT_GATT_UUID_CGM_SESSION_RUN_TIME_CHARAC_STR               "CGM Session Run Time"
#define CSR_BT_GATT_UUID_CGM_SPECIFIC_OPS_CONTROL_POINT_CHARAC_STR     "CGM Specific Ops Control Point"
#define CSR_BT_GATT_UUID_INDOOR_POSITIONING_CONFIGURATION_CHARAC_STR   "Indoor Positioning Configuration"
#define CSR_BT_GATT_UUID_LATITUDE_CHARAC_STR                           "Latitude"
#define CSR_BT_GATT_UUID_LONGITUDE_CHARAC_STR                          "Longitude"
#define CSR_BT_GATT_UUID_LOCAL_NORTH_COORDINATE_CHARAC_STR             "Local North Coordinate"
#define CSR_BT_GATT_UUID_LOCAL_EAST_COORDINATE_CHARAC_STR              "Local East Coordinate"
#define CSR_BT_GATT_UUID_FLOOR_NUMBER_CHARAC_STR                       "Floor Number"
#define CSR_BT_GATT_UUID_ALTITUDE_CHARAC_STR                           "Altitude"
#define CSR_BT_GATT_UUID_UNCERTAINTY_CHARAC_STR                        "Uncertainty"
#define CSR_BT_GATT_UUID_LOCATION_NAME_CHARAC_STR                      "Location Name"
#define CSR_BT_GATT_UUID_URI_CHARAC_STR                                "URI"
#define CSR_BT_GATT_UUID_HTTP_HEADERS_CHARAC_STR                       "HTTP Headers"
#define CSR_BT_GATT_UUID_HTTP_STATUS_CODE_CHARAC_STR                   "HTTP Status Code"
#define CSR_BT_GATT_UUID_HTTP_ENTITY_BODY_CHARAC_STR                   "HTTP Entity Body"
#define CSR_BT_GATT_UUID_HTTP_CONTROL_POINT_CHARAC_STR                 "HTTP Control Point"
#define CSR_BT_GATT_UUID_HTTPS_SECURITY_CHARAC_STR                     "HTTPS Security"
#define CSR_BT_GATT_UUID_TDS_CONTROL_POINT_CHARAC_STR                  "TDS Control Point"
#define CSR_BT_GATT_UUID_OTS_FEATURE_CHARAC_STR                        "OTS Feature"
#define CSR_BT_GATT_UUID_OBJECT_NAME_CHARAC_STR                        "Object Name"
#define CSR_BT_GATT_UUID_OBJECT_TYPE_CHARAC_STR                        "Object Type"
#define CSR_BT_GATT_UUID_OBJECT_SIZE_CHARAC_STR                        "Object Size"
#define CSR_BT_GATT_UUID_OBJECT_FIRST_CREATED_CHARAC_STR               "Object First-Created"
#define CSR_BT_GATT_UUID_OBJECT_LAST_MODIFIED_CHARAC_STR               "Object Last-Modified"
#define CSR_BT_GATT_UUID_OBJECT_ID_CHARAC_STR                          "Object ID"
#define CSR_BT_GATT_UUID_OBJECT_PROPERTIES_CHARAC_STR                  "Object Properties"
#define CSR_BT_GATT_UUID_OBJECT_ACTION_CONTROL_POINT_CHARAC_STR        "Object Action Control Point"
#define CSR_BT_GATT_UUID_OBJECT_LIST_CONTROL_POINT_CHARAC_STR          "Object List Control Point"
#define CSR_BT_GATT_UUID_OBJECT_LIST_FILTER_CHARAC_STR                 "Object List Filter"
#define CSR_BT_GATT_UUID_OBJECT_CHANGED_CHARAC_STR                     "Object Changed"
#define CSR_BT_GATT_UUID_RESOLVABLE_PRIVATE_ADDRESS_ONLY_CHARAC_STR    "Resolvable Private Address Only"

/*************************** Deprecated UUID lists ****************************/
/* GATT Services */
#define CSR_BT_GAP_UUID                                     CSR_BT_GATT_UUID_GENERIC_ACCESS_SERVICE
#define CSR_BT_GATT_UUID                                    CSR_BT_GATT_UUID_GENERIC_ATTRIBUTE_SERVICE

/* GATT Attributes Types */
#define CSR_BT_GATT_PRIMARY_SERVICE_UUID                    CSR_BT_GATT_UUID_PRIMARY_SERVICE_DECL
#define CSR_BT_GATT_SECONDARY_SERVICE_UUID                  CSR_BT_GATT_UUID_SECONDARY_SERVICE_DECL
#define CSR_BT_GATT_INCLUDE_UUID                            CSR_BT_GATT_UUID_INCLUDE_DECL
#define CSR_BT_GATT_CHARACTERISTIC_UUID                     CSR_BT_GATT_UUID_CHARACTERISTIC_DECL

/* GATT Characteristic Types */
#define CSR_BT_GATT_DEVICE_NAME_UUID                        CSR_BT_GATT_UUID_DEVICE_NAME_CHARAC
#define CSR_BT_GATT_APPEARANCE_UUID                         CSR_BT_GATT_UUID_APPEARANCE_CHARAC
#define CSR_BT_GATT_PERIPHERAL_PRIVACY_FLAG_UUID            CSR_BT_GATT_UUID_PERIPHERAL_PRIVACY_FLAG_CHARAC
#define CSR_BT_GATT_RECONNECTION_ADDRESS_UUID               CSR_BT_GATT_UUID_RECONNECTION_ADDRESS_CHARAC
#define CSR_BT_GATT_PERIPHERAL_CONNECTION_PARAM_UUID        CSR_BT_GATT_UUID_PERIPHERAL_PREFERRED_CONN_PARAM_CHARAC
#define CSR_BT_GATT_SERVICE_CHANGED_UUID                    CSR_BT_GATT_UUID_SERVICE_CHANGED_CHARAC

/* GATT Characteristic Descriptor Declarations Types */
#define CSR_BT_GATT_CHARACTERISTIC_EXTENDED_PROPERTIES      CSR_BT_GATT_UUID_CHARACTERISTIC_EXTENDED_PROPERTIES_DESC
#define CSR_BT_GATT_CHARACTERISTIC_USER_DESCRIPTION         CSR_BT_GATT_UUID_CHARACTERISTIC_USER_DESCRIPTION_DESC
#define CSR_BT_GATT_CLIENT_CHARACTERISTIC_CONFIGURATION     CSR_BT_GATT_UUID_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC
#define CSR_BT_GATT_SERVER_CHARACTERISTIC_CONFIGURATION     CSR_BT_GATT_UUID_SERVER_CHARACTERISTIC_CONFIGURATION_DESC
#define CSR_BT_GATT_CHARACTERISTIC_PRESENTATION_FORMAT      CSR_BT_GATT_UUID_CHARACTERISTIC_PRESENTATION_FORMAT_DESC
#define CSR_BT_GATT_CHARACTERISTIC_AGGREGATE_FORMAT         CSR_BT_GATT_UUID_CHARACTERISTIC_AGGREGATE_FORMAT_DESC


/*********************** Expands UUID into 128-bit UUID ***********************
 * This function expands UUIDs into 128-bit length UUID. It returns TRUE if
 * UUID is successfully expanded.
 * 16-bit or 32-bit UUIDs are be expanded into 128-bit UUID.
 * 128-bit length UUID is copied as it is into destination UUID.
 * UUIDs of any other length cannot be expanded, in which case this function
 * returns FALSE.
 *
 * Parameters
 *  uuid [IN]           -   Source UUID
 *  expandedUuid [OUT]  -   Placeholder for expanded UUID
 *
 * Returns
 *  TRUE if source UUID is 16-bit, 32-bit or 128-bit length; else FALSE.
 *****************************************************************************/
CsrBool CsrBtUuidExpand(const CsrBtUuid *uuid, CsrBtUuid *expandedUuid);

/*********************** Creates UUID from 16-bit value ***********************
 * This function creates UUID from 16-bit value.
 *
 * Parameters
 *  uuid16 [IN]             -   16-bit UUID value
 *  uuid [OUT]              -   Placeholder for UUID
 *
 * Returns
 *  void
 *****************************************************************************/
void CsrBtUuid16ToUuid(CsrBtUuid16 uuid16, CsrBtUuid *uuid);

/*********************** Creates UUID from 32-bit value ***********************
 * This function creates UUID from 32-bit value.
 *
 * Parameters
 *  uuid32 [IN]             -   32-bit UUID value
 *  uuid [OUT]              -   Placeholder for UUID
 *
 * Returns
 *  void
 *****************************************************************************/
void CsrBtUuid32ToUuid(CsrBtUuid32 uuid32, CsrBtUuid *uuid);

/****************************** Compares UUIDs ********************************
 * This function compares two UUIDs. If UUID size differs, it tries to expand
 * them before comparing.
 * If UUIDs are of non-standard size (other than 16-bit, 32-bit or 128-bit),
 * they cannot be expanded. In such case they cannot be compared if their sizes
 * don't match and result in failed match.
 *
 * Parameters
 *  uuid1 [IN]          -   First UUID
 *  uuid2 [IN]          -   Second UUID
 *
 * Returns
 *  TRUE if UUIDs match, else FALSE.
 *****************************************************************************/
CsrBool CsrBtUuidCompare(const CsrBtUuid *uuid1, const CsrBtUuid *uuid2);


#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_UUIDS_H__ */
