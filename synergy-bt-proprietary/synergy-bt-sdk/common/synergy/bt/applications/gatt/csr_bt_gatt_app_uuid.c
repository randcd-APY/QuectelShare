/******************************************************************************
 Copyright (c) 2016-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.

 REVISION:      $Revision: #2 $
******************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_bt_profiles.h"
#include "csr_bt_uuids.h"
#include "csr_bt_gatt_demo_defines.h"


#define CSR_BT_GATT_UUID_NOT_DEFINED_STR                            (NULL)
#define CSR_BT_GATT_UUID_UNKOWN_STR                                 "Unknown UUID"

static const CsrCharString *declarationName[] = {
  CSR_BT_GATT_UUID_PRIMARY_SERVICE_DECL_STR,                        /* 0x2800 */
  CSR_BT_GATT_UUID_SECONDARY_SERVICE_DECL_STR,                      /* 0x2801 */
  CSR_BT_GATT_UUID_INCLUDE_DECL_STR,                                /* 0x2802 */
  CSR_BT_GATT_UUID_CHARACTERISTIC_DECL_STR,                         /* 0x2803 */
};

#define CSR_BT_GATT_DESCRIPTOR_STR                                  " Descriptor"
static const CsrCharString *descriptorName[] = {
  CSR_BT_GATT_UUID_CHARACTERISTIC_EXTENDED_PROPERTIES_DESC_STR,     /* 0x2900 */
  CSR_BT_GATT_UUID_CHARACTERISTIC_USER_DESC_STRRIPTION_DESC_STR,    /* 0x2901 */
  CSR_BT_GATT_UUID_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_STR,    /* 0x2902 */
  CSR_BT_GATT_UUID_SERVER_CHARACTERISTIC_CONFIGURATION_DESC_STR,    /* 0x2903 */
  CSR_BT_GATT_UUID_CHARACTERISTIC_PRESENTATION_FORMAT_DESC_STR,     /* 0x2904 */
  CSR_BT_GATT_UUID_CHARACTERISTIC_AGGREGATE_FORMAT_DESC_STR,        /* 0x2905 */
  CSR_BT_GATT_UUID_VALID_RANGE_DESC_STR,                            /* 0x2906 */
  CSR_BT_GATT_UUID_EXTERNAL_REPORT_REFERENCE_DESC_STR,              /* 0x2907 */
  CSR_BT_GATT_UUID_REPORT_REFERENCE_DESC_STR,                       /* 0x2908 */
  CSR_BT_GATT_UUID_NUMBER_OF_DIGITALS_DESC_STR,                     /* 0x2909 */
  CSR_BT_GATT_UUID_VALUE_TRIGGER_SETTING_DESC_STR,                  /* 0x290A */
  CSR_BT_GATT_UUID_ENVIRONMENTAL_SENSING_CONFIGURATION_DESC_STR,    /* 0x290B */
  CSR_BT_GATT_UUID_ENVIRONMENTAL_SENSING_MEASUREMENT_DESC_STR,      /* 0x290C */
  CSR_BT_GATT_UUID_ENVIRONMENTAL_SENSING_TRIGGER_SETTING_DESC_STR,  /* 0x290D */
};

#define CSR_BT_GATT_SERVICE_STR                                     " Service"
static const CsrCharString *serviceName[] = {
  CSR_BT_GATT_UUID_GENERIC_ACCESS_SERVICE_STR,                      /* 0x1800 */
  CSR_BT_GATT_UUID_GENERIC_ATTRIBUTE_SERVICE_STR,                   /* 0x1801 */
  CSR_BT_GATT_UUID_IMMEDIATE_ALERT_SERVICE_STR,                     /* 0x1802 */
  CSR_BT_GATT_UUID_LINK_LOSS_SERVICE_STR,                           /* 0x1803 */
  CSR_BT_GATT_UUID_TX_POWER_SERVICE_STR,                            /* 0x1804 */
  CSR_BT_GATT_UUID_CURRENT_TIME_SERVICE_STR,                        /* 0x1805 */
  CSR_BT_GATT_UUID_REFERENCE_TIME_UPDATE_SERVICE_STR,               /* 0x1806 */
  CSR_BT_GATT_UUID_NEXT_DST_CHANGE_SERVICE_STR,                     /* 0x1807 */
  CSR_BT_GATT_UUID_GLUCOSE_SERVICE_STR,                             /* 0x1808 */
  CSR_BT_GATT_UUID_HEALTH_THERMOMETER_SERVICE_STR,                  /* 0x1809 */
  CSR_BT_GATT_UUID_DEVICE_INFORMATION_SERVICE_STR,                  /* 0x180A */
  CSR_BT_GATT_UUID_NOT_DEFINED_STR,                                 /*  */
  CSR_BT_GATT_UUID_NOT_DEFINED_STR,                                 /*  */
  CSR_BT_GATT_UUID_HEART_RATE_SERVICE_STR,                          /* 0x180D */
  CSR_BT_GATT_UUID_PHONE_ALERT_STATUS_SERVICE_STR,                  /* 0x180E */
  CSR_BT_GATT_UUID_BATTERY_SERVICE_STR,                             /* 0x180F */
  CSR_BT_GATT_UUID_BLOOD_PRESSURE_SERVICE_STR,                      /* 0x1810 */
  CSR_BT_GATT_UUID_ALERT_NOTIFICATION_SERVICE_STR,                  /* 0x1811 */
  CSR_BT_GATT_UUID_HUMAN_INTERFACE_DEVICE_SERVICE_STR,              /* 0x1812 */
  CSR_BT_GATT_UUID_SCAN_PARAMETERS_SERVICE_STR,                     /* 0x1813 */
  CSR_BT_GATT_UUID_RUNNING_SPEED_AND_CADENCE_SERVICE_STR,           /* 0x1814 */
  CSR_BT_GATT_UUID_AUTOMATION_IO_SERVICE_STR,                       /* 0x1815 */
  CSR_BT_GATT_UUID_CYCLING_SPEED_AND_CADENCE_SERVICE_STR,           /* 0x1816 */
  CSR_BT_GATT_UUID_NOT_DEFINED_STR,                                 /*  */
  CSR_BT_GATT_UUID_CYCLING_POWER_SERVICE_STR,                       /* 0x1818 */
  CSR_BT_GATT_UUID_LOCATION_AND_NAVIGATION_SERVICE_STR,             /* 0x1819 */
  CSR_BT_GATT_UUID_ENVIRONMENTAL_SENSING_SERVICE_STR,               /* 0x181A */
  CSR_BT_GATT_UUID_BODY_COMPOSITION_SERVICE_STR,                    /* 0x181B */
  CSR_BT_GATT_UUID_USER_DATA_SERVICE_STR,                           /* 0x181C */
  CSR_BT_GATT_UUID_WEIGHT_SCALE_SERVICE_STR,                        /* 0x181D */
  CSR_BT_GATT_UUID_BOND_MANAGEMENT_SERVICE_STR,                     /* 0x181E */
  CSR_BT_GATT_UUID_CONTINUOUS_GLUCOSE_MONITORING_SERVICE_STR,       /* 0x181F */
  CSR_BT_GATT_UUID_INTERNET_PROTOCOL_SUPPORT_SERVICE_STR,           /* 0x1820 */
  CSR_BT_GATT_UUID_INDOOR_POSITIONING_SERVICE_STR,                  /* 0x1821 */
  CSR_BT_GATT_UUID_PULSE_OXIMETER_SERVICE_STR,                      /* 0x1822 */
  CSR_BT_GATT_UUID_HTTP_PROXY_SERVICE_STR,                          /* 0x1823 */
  CSR_BT_GATT_UUID_TRANSPORT_DISCOVERY_SERVICE_STR,                 /* 0x1824 */
  CSR_BT_GATT_UUID_OBJECT_TRANSFER_SERVICE_STR,                     /* 0x1825 */
};

#define CSR_BT_GATT_CHARACTERISTIC_STR                              " Characteristic"
static const CsrCharString *characName[] = {
  CSR_BT_GATT_UUID_DEVICE_NAME_CHARAC_STR,                          /* 0x2A00 */
  CSR_BT_GATT_UUID_APPEARANCE_CHARAC_STR,                           /* 0x2A01 */
  CSR_BT_GATT_UUID_PERIPHERAL_PRIVACY_FLAG_CHARAC_STR,              /* 0x2A02 */
  CSR_BT_GATT_UUID_RECONNECTION_ADDRESS_CHARAC_STR,                 /* 0x2A03 */
  CSR_BT_GATT_UUID_PERIPHERAL_PREFERRED_CONN_PARAM_CHARAC_STR,      /* 0x2A04 */
  CSR_BT_GATT_UUID_SERVICE_CHANGED_CHARAC_STR,                      /* 0x2A05 */
  CSR_BT_GATT_UUID_ALERT_LEVEL_CHARAC_STR,                          /* 0x2A06 */
  CSR_BT_GATT_UUID_TX_POWER_LEVEL_CHARAC_STR,                       /* 0x2A07 */
  CSR_BT_GATT_UUID_DATE_TIME_CHARAC_STR,                            /* 0x2A08 */
  CSR_BT_GATT_UUID_DAY_OF_WEEK_CHARAC_STR,                          /* 0x2A09 */
  CSR_BT_GATT_UUID_DAY_DATE_TIME_CHARAC_STR,                        /* 0x2A0A */
  CSR_BT_GATT_UUID_NOT_DEFINED_STR,                                 /*  */
  CSR_BT_GATT_UUID_EXACT_TIME_256_CHARAC_STR,                       /* 0x2A0C */
  CSR_BT_GATT_UUID_DST_OFFSET_CHARAC_STR,                           /* 0x2A0D */
  CSR_BT_GATT_UUID_TIME_ZONE_CHARAC_STR,                            /* 0x2A0E */
  CSR_BT_GATT_UUID_LOCAL_TIME_INFORMATION_CHARAC_STR,               /* 0x2A0F */
  CSR_BT_GATT_UUID_NOT_DEFINED_STR,                                 /*  */
  CSR_BT_GATT_UUID_TIME_WITH_DST_CHARAC_STR,                        /* 0x2A11 */
  CSR_BT_GATT_UUID_TIME_ACCURACY_CHARAC_STR,                        /* 0x2A12 */
  CSR_BT_GATT_UUID_TIME_SOURCE_CHARAC_STR,                          /* 0x2A13 */
  CSR_BT_GATT_UUID_REFERENCE_TIME_INFORMATION_CHARAC_STR,           /* 0x2A14 */
  CSR_BT_GATT_UUID_NOT_DEFINED_STR,                                 /*  */
  CSR_BT_GATT_UUID_TIME_UPDATE_CONTROL_POINT_CHARAC_STR,            /* 0x2A16 */
  CSR_BT_GATT_UUID_TIME_UPDATE_STATE_CHARAC_STR,                    /* 0x2A17 */
  CSR_BT_GATT_UUID_GLUCOSE_MEASUREMENT_CHARAC_STR,                  /* 0x2A18 */
  CSR_BT_GATT_UUID_BATTERY_LEVEL_CHARAC_STR,                        /* 0x2A19 */
  CSR_BT_GATT_UUID_NOT_DEFINED_STR,                                 /*  */
  CSR_BT_GATT_UUID_NOT_DEFINED_STR,                                 /*  */
  CSR_BT_GATT_UUID_TEMPERATURE_MEASUREMENT_CHARAC_STR,              /* 0x2A1C */
  CSR_BT_GATT_UUID_TEMPERATURE_TYPE_CHARAC_STR,                     /* 0x2A1D */
  CSR_BT_GATT_UUID_INTERMEDIATE_TEMPERATURE_CHARAC_STR,             /* 0x2A1E */
  CSR_BT_GATT_UUID_NOT_DEFINED_STR,                                 /*  */
  CSR_BT_GATT_UUID_NOT_DEFINED_STR,                                 /*  */
  CSR_BT_GATT_UUID_MEASUREMENT_INTERVAL_CHARAC_STR,                 /* 0x2A21 */
  CSR_BT_GATT_UUID_BOOT_KEYBOARD_INPUT_REPORT_CHARAC_STR,           /* 0x2A22 */
  CSR_BT_GATT_UUID_SYSTEM_ID_CHARAC_STR,                            /* 0x2A23 */
  CSR_BT_GATT_UUID_MODEL_NUMBER_STRING_CHARAC_STR,                  /* 0x2A24 */
  CSR_BT_GATT_UUID_SERIAL_NUMBER_STRING_CHARAC_STR,                 /* 0x2A25 */
  CSR_BT_GATT_UUID_FIRMWARE_REVISION_STRING_CHARAC_STR,             /* 0x2A26 */
  CSR_BT_GATT_UUID_HARDWARE_REVISION_STRING_CHARAC_STR,             /* 0x2A27 */
  CSR_BT_GATT_UUID_SOFTWARE_REVISION_STRING_CHARAC_STR,             /* 0x2A28 */
  CSR_BT_GATT_UUID_MANUFACTURER_NAME_STRING_CHARAC_STR,             /* 0x2A29 */
  CSR_BT_GATT_UUID_IEEE_REG_CERT_DATA_LIST_CHARAC_STR,              /* 0x2A2A */
  CSR_BT_GATT_UUID_CURRENT_TIME_CHARAC_STR,                         /* 0x2A2B */
  CSR_BT_GATT_UUID_MAGNETIC_DECLINATION_CHARAC_STR,                 /* 0x2A2C */
  CSR_BT_GATT_UUID_NOT_DEFINED_STR,                                 /*  */
  CSR_BT_GATT_UUID_NOT_DEFINED_STR,                                 /*  */
  CSR_BT_GATT_UUID_NOT_DEFINED_STR,                                 /*  */
  CSR_BT_GATT_UUID_NOT_DEFINED_STR,                                 /*  */
  CSR_BT_GATT_UUID_SCAN_REFRESH_CHARAC_STR,                         /* 0x2A31 */
  CSR_BT_GATT_UUID_BOOT_KEYBOARD_OUTPUT_REPORT_CHARAC_STR,          /* 0x2A32 */
  CSR_BT_GATT_UUID_BOOT_MOUSE_INPUT_REPORT_CHARAC_STR,              /* 0x2A33 */
  CSR_BT_GATT_UUID_GLUCOSE_MEASUREMENT_CONTEXT_CHARAC_STR,          /* 0x2A34 */
  CSR_BT_GATT_UUID_BLOOD_PRESSURE_MEASUREMENT_CHARAC_STR,           /* 0x2A35 */
  CSR_BT_GATT_UUID_INTERMEDIATE_CUFF_PRESSURE_CHARAC_STR,           /* 0x2A36 */
  CSR_BT_GATT_UUID_HEART_RATE_MEASUREMENT_CHARAC_STR,               /* 0x2A37 */
  CSR_BT_GATT_UUID_BODY_SENSOR_LOCATION_CHARAC_STR,                 /* 0x2A38 */
  CSR_BT_GATT_UUID_HEART_RATE_CONTROL_POINT_CHARAC_STR,             /* 0x2A39 */
  CSR_BT_GATT_UUID_NOT_DEFINED_STR,                                 /*  */
  CSR_BT_GATT_UUID_NOT_DEFINED_STR,                                 /*  */
  CSR_BT_GATT_UUID_NOT_DEFINED_STR,                                 /*  */
  CSR_BT_GATT_UUID_NOT_DEFINED_STR,                                 /*  */
  CSR_BT_GATT_UUID_NOT_DEFINED_STR,                                 /*  */
  CSR_BT_GATT_UUID_ALERT_STATUS_CHARAC_STR,                         /* 0x2A3F */
  CSR_BT_GATT_UUID_RINGER_CONTROL_POINT_CHARAC_STR,                 /* 0x2A40 */
  CSR_BT_GATT_UUID_RINGER_SETTING_CHARAC_STR,                       /* 0x2A41 */
  CSR_BT_GATT_UUID_ALERT_CATEGORY_ID_BIT_MASK_CHARAC_STR,           /* 0x2A42 */
  CSR_BT_GATT_UUID_ALERT_CATEGORY_ID_CHARAC_STR,                    /* 0x2A43 */
  CSR_BT_GATT_UUID_ALERT_NOTIFICATION_CONTROL_POINT_CHARAC_STR,     /* 0x2A44 */
  CSR_BT_GATT_UUID_UNREAD_ALERT_STATUS_CHARAC_STR,                  /* 0x2A45 */
  CSR_BT_GATT_UUID_NEW_ALERT_CHARAC_STR,                            /* 0x2A46 */
  CSR_BT_GATT_UUID_SUPPORTED_NEW_ALERT_CATEGORY_CHARAC_STR,         /* 0x2A47 */
  CSR_BT_GATT_UUID_SUPPORTED_UNREAD_ALERT_CATEGORY_CHARAC_STR,      /* 0x2A48 */
  CSR_BT_GATT_UUID_BLOOD_PRESSURE_FEATURE_CHARAC_STR,               /* 0x2A49 */
  CSR_BT_GATT_UUID_HID_INFORMATION_CHARAC_STR,                      /* 0x2A4A */
  CSR_BT_GATT_UUID_REPORT_MAP_CHARAC_STR,                           /* 0x2A4B */
  CSR_BT_GATT_UUID_HID_CONTROL_POINT_CHARAC_STR,                    /* 0x2A4C */
  CSR_BT_GATT_UUID_REPORT_CHARAC_STR,                               /* 0x2A4D */
  CSR_BT_GATT_UUID_PROTOCOL_MODE_CHARAC_STR,                        /* 0x2A4E */
  CSR_BT_GATT_UUID_SCAN_INTERVAL_WINDOW_CHARAC_STR,                 /* 0x2A4F */
  CSR_BT_GATT_UUID_PNP_ID_CHARAC_STR,                               /* 0x2A50 */
  CSR_BT_GATT_UUID_GLUCOSE_FEATURE_CHARAC_STR,                      /* 0x2A51 */
  CSR_BT_GATT_UUID_RECORD_ACCESS_CONTROL_POINT_CHARAC_STR,          /* 0x2A52 */
  CSR_BT_GATT_UUID_RSC_MEASUREMENT_CHARAC_STR,                      /* 0x2A53 */
  CSR_BT_GATT_UUID_RSC_FEATURE_CHARAC_STR,                          /* 0x2A54 */
  CSR_BT_GATT_UUID_SC_CONTROL_POINT_CHARAC_STR,                     /* 0x2A55 */
  CSR_BT_GATT_UUID_DIGITAL_CHARAC_STR,                              /* 0x2A56 */
  CSR_BT_GATT_UUID_NOT_DEFINED_STR,                                 /*  */
  CSR_BT_GATT_UUID_ANALOG_CHARAC_STR,                               /* 0x2A58 */
  CSR_BT_GATT_UUID_NOT_DEFINED_STR,                                 /*  */
  CSR_BT_GATT_UUID_AGGREGATE_CHARAC_STR,                            /* 0x2A5A */
  CSR_BT_GATT_UUID_CSC_MEASUREMENT_CHARAC_STR,                      /* 0x2A5B */
  CSR_BT_GATT_UUID_CSC_FEATURE_CHARAC_STR,                          /* 0x2A5C */
  CSR_BT_GATT_UUID_SENSOR_LOCATION_CHARAC_STR,                      /* 0x2A5D */
  CSR_BT_GATT_UUID_PLX_SPOT_CHECK_MEASUREMENT_CHARAC_STR,           /* 0x2A5E */
  CSR_BT_GATT_UUID_PLX_CONTINUOUS_MEASUREMENT_CHARAC_STR,           /* 0x2A5F */
  CSR_BT_GATT_UUID_PLX_FEATURES_CHARAC_STR,                         /* 0x2A60 */
  CSR_BT_GATT_UUID_NOT_DEFINED_STR,                                 /*  */
  CSR_BT_GATT_UUID_NOT_DEFINED_STR,                                 /*  */
  CSR_BT_GATT_UUID_CYCLING_POWER_MEASUREMENT_CHARAC_STR,            /* 0x2A63 */
  CSR_BT_GATT_UUID_CYCLING_POWER_VECTOR_CHARAC_STR,                 /* 0x2A64 */
  CSR_BT_GATT_UUID_CYCLING_POWER_FEATURE_CHARAC_STR,                /* 0x2A65 */
  CSR_BT_GATT_UUID_CYCLING_POWER_CONTROL_POINT_CHARAC_STR,          /* 0x2A66 */
  CSR_BT_GATT_UUID_LOCATION_AND_SPEED_CHARAC_STR,                   /* 0x2A67 */
  CSR_BT_GATT_UUID_NAVIGATION_CHARAC_STR,                           /* 0x2A68 */
  CSR_BT_GATT_UUID_POSITION_QUALITY_CHARAC_STR,                     /* 0x2A69 */
  CSR_BT_GATT_UUID_LN_FEATURE_CHARAC_STR,                           /* 0x2A6A */
  CSR_BT_GATT_UUID_LN_CONTROL_POINT_CHARAC_STR,                     /* 0x2A6B */
  CSR_BT_GATT_UUID_ELEVATION_CHARAC_STR,                            /* 0x2A6C */
  CSR_BT_GATT_UUID_PRESSURE_CHARAC_STR,                             /* 0x2A6D */
  CSR_BT_GATT_UUID_TEMPERATURE_CHARAC_STR,                          /* 0x2A6E */
  CSR_BT_GATT_UUID_HUMIDITY_CHARAC_STR,                             /* 0x2A6F */
  CSR_BT_GATT_UUID_TRUE_WIND_SPEED_CHARAC_STR,                      /* 0x2A70 */
  CSR_BT_GATT_UUID_TRUE_WIND_DIRECTION_CHARAC_STR,                  /* 0x2A71 */
  CSR_BT_GATT_UUID_APPARENT_WIND_SPEED_CHARAC_STR,                  /* 0x2A72 */
  CSR_BT_GATT_UUID_APPARENT_WIND_DIRECTION__CHARAC_STR,             /* 0x2A73 */
  CSR_BT_GATT_UUID_GUST_FACTOR_CHARAC_STR,                          /* 0x2A74 */
  CSR_BT_GATT_UUID_POLLEN_CONCENTRATION_CHARAC_STR,                 /* 0x2A75 */
  CSR_BT_GATT_UUID_UV_INDEX_CHARAC_STR,                             /* 0x2A76 */
  CSR_BT_GATT_UUID_IRRADIANCE_CHARAC_STR,                           /* 0x2A77 */
  CSR_BT_GATT_UUID_RAINFALL_CHARAC_STR,                             /* 0x2A78 */
  CSR_BT_GATT_UUID_WIND_CHILL_CHARAC_STR,                           /* 0x2A79 */
  CSR_BT_GATT_UUID_HEAT_INDEX_CHARAC_STR,                           /* 0x2A7A */
  CSR_BT_GATT_UUID_DEW_POINT_CHARAC_STR,                            /* 0x2A7B */
  CSR_BT_GATT_UUID_NOT_DEFINED_STR,                                 /*  */
  CSR_BT_GATT_UUID_DESCRIPTOR_VALUE_CHANGED_CHARAC_STR,             /* 0x2A7D */
  CSR_BT_GATT_UUID_AEROBIC_HEART_RATE_LOWER_LIMIT_CHARAC_STR,       /* 0x2A7E */
  CSR_BT_GATT_UUID_AEROBIC_THRESHOLD_CHARAC_STR,                    /* 0x2A7F */
  CSR_BT_GATT_UUID_AGE_CHARAC_STR,                                  /* 0x2A80 */
  CSR_BT_GATT_UUID_ANAEROBIC_HEART_RATE_LOWER_LIMIT_CHARAC_STR,     /* 0x2A81 */
  CSR_BT_GATT_UUID_ANAEROBIC_HEART_RATE_UPPER_LIMIT_CHARAC_STR,     /* 0x2A82 */
  CSR_BT_GATT_UUID_ANAEROBIC_THRESHOLD_CHARAC_STR,                  /* 0x2A83 */
  CSR_BT_GATT_UUID_AEROBIC_HEART_RATE_UPPER_LIMIT_CHARAC_STR,       /* 0x2A84 */
  CSR_BT_GATT_UUID_DATE_OF_BIRTH_CHARAC_STR,                        /* 0x2A85 */
  CSR_BT_GATT_UUID_DATE_OF_THRESHOLD_ASSESSMENT_CHARAC_STR,         /* 0x2A86 */
  CSR_BT_GATT_UUID_EMAIL_ADDRESS_CHARAC_STR,                        /* 0x2A87 */
  CSR_BT_GATT_UUID_FAT_BURN_HEART_RATE_LOWER_LIMIT_CHARAC_STR,      /* 0x2A88 */
  CSR_BT_GATT_UUID_FAT_BURN_HEART_RATE_UPPER_LIMIT_CHARAC_STR,      /* 0x2A89 */
  CSR_BT_GATT_UUID_FIRST_NAME_CHARAC_STR,                           /* 0x2A8A */
  CSR_BT_GATT_UUID_FIVE_ZONE_HEART_RATE_LIMITS_CHARAC_STR,          /* 0x2A8B */
  CSR_BT_GATT_UUID_GENDER_CHARAC_STR,                               /* 0x2A8C */
  CSR_BT_GATT_UUID_HEART_RATE_MAX_CHARAC_STR,                       /* 0x2A8D */
  CSR_BT_GATT_UUID_HEIGHT_CHARAC_STR,                               /* 0x2A8E */
  CSR_BT_GATT_UUID_HIP_CIRCUMFERENCE_CHARAC_STR,                    /* 0x2A8F */
  CSR_BT_GATT_UUID_LAST_NAME_CHARAC_STR,                            /* 0x2A90 */
  CSR_BT_GATT_UUID_MAXIMUM_RECOMMENDED_HEART_RATE_CHARAC_STR,       /* 0x2A91 */
  CSR_BT_GATT_UUID_RESTING_HEART_RATE_CHARAC_STR,                   /* 0x2A92 */
  CSR_BT_GATT_UUID_SPORT_TYPE_AEROBIC_ANAEROBIC_THRES_CHARAC_STR,   /* 0x2A93 */
  CSR_BT_GATT_UUID_THREE_ZONE_HEART_RATE_LIMITS_CHARAC_STR,         /* 0x2A94 */
  CSR_BT_GATT_UUID_TWO_ZONE_HEART_RATE_LIMIT_CHARAC_STR,            /* 0x2A95 */
  CSR_BT_GATT_UUID_VO2_MAX_CHARAC_STR,                              /* 0x2A96 */
  CSR_BT_GATT_UUID_WAIST_CIRCUMFERENCE_CHARAC_STR,                  /* 0x2A97 */
  CSR_BT_GATT_UUID_WEIGHT_CHARAC_STR,                               /* 0x2A98 */
  CSR_BT_GATT_UUID_DATABASE_CHANGE_INCREMENT_CHARAC_STR,            /* 0x2A99 */
  CSR_BT_GATT_UUID_USER_INDEX_CHARAC_STR,                           /* 0x2A9A */
  CSR_BT_GATT_UUID_BODY_COMPOSITION_FEATURE_CHARAC_STR,             /* 0x2A9B */
  CSR_BT_GATT_UUID_BODY_COMPOSITION_MEASUREMENT_CHARAC_STR,         /* 0x2A9C */
  CSR_BT_GATT_UUID_WEIGHT_MEASUREMENT_CHARAC_STR,                   /* 0x2A9D */
  CSR_BT_GATT_UUID_WEIGHT_SCALE_FEATURE_CHARAC_STR,                 /* 0x2A9E */
  CSR_BT_GATT_UUID_USER_CONTROL_POINT_CHARAC_STR,                   /* 0x2A9F */
  CSR_BT_GATT_UUID_MAGNETIC_FLUX_DENSITY_2D_CHARAC_STR,             /* 0x2AA0 */
  CSR_BT_GATT_UUID_MAGNETIC_FLUX_DENSITY_3D_CHARAC_STR,             /* 0x2AA1 */
  CSR_BT_GATT_UUID_LANGUAGE_CHARAC_STR,                             /* 0x2AA2 */
  CSR_BT_GATT_UUID_BAROMETRIC_PRESSURE_TREND_CHARAC_STR,            /* 0x2AA3 */
  CSR_BT_GATT_UUID_BOND_MANAGEMENT_CONTROL_POINT_CHARAC_STR,        /* 0x2AA4 */
  CSR_BT_GATT_UUID_BOND_MANAGEMENT_FEATURE_CHARAC_STR,              /* 0x2AA5 */
  CSR_BT_GATT_UUID_CENTRAL_ADDRESS_RESOLUTION_CHARAC_STR,           /* 0x2AA6 */
  CSR_BT_GATT_UUID_CGM_MEASUREMENT_CHARAC_STR,                      /* 0x2AA7 */
  CSR_BT_GATT_UUID_CGM_FEATURE_CHARAC_STR,                          /* 0x2AA8 */
  CSR_BT_GATT_UUID_CGM_STATUS_CHARAC_STR,                           /* 0x2AA9 */
  CSR_BT_GATT_UUID_CGM_SESSION_START_TIME_CHARAC_STR,               /* 0x2AAA */
  CSR_BT_GATT_UUID_CGM_SESSION_RUN_TIME_CHARAC_STR,                 /* 0x2AAB */
  CSR_BT_GATT_UUID_CGM_SPECIFIC_OPS_CONTROL_POINT_CHARAC_STR,       /* 0x2AAC */
  CSR_BT_GATT_UUID_INDOOR_POSITIONING_CONFIGURATION_CHARAC_STR,     /* 0x2AAD */
  CSR_BT_GATT_UUID_LATITUDE_CHARAC_STR,                             /* 0x2AAE */
  CSR_BT_GATT_UUID_LONGITUDE_CHARAC_STR,                            /* 0x2AAF */
  CSR_BT_GATT_UUID_LOCAL_NORTH_COORDINATE_CHARAC_STR,               /* 0x2AB0 */
  CSR_BT_GATT_UUID_LOCAL_EAST_COORDINATE_CHARAC_STR,                /* 0x2AB1 */
  CSR_BT_GATT_UUID_FLOOR_NUMBER_CHARAC_STR,                         /* 0x2AB2 */
  CSR_BT_GATT_UUID_ALTITUDE_CHARAC_STR,                             /* 0x2AB3 */
  CSR_BT_GATT_UUID_UNCERTAINTY_CHARAC_STR,                          /* 0x2AB4 */
  /* [QTI] Fix KW issue#267136~267144. */
  CSR_BT_GATT_UUID_LOCATION_NAME_CHARAC_STR,                        /* 0x2AB5 */
  CSR_BT_GATT_UUID_URI_CHARAC_STR,                                  /* 0x2AB6 */
  CSR_BT_GATT_UUID_HTTP_HEADERS_CHARAC_STR,                         /* 0x2AB7 */
  CSR_BT_GATT_UUID_HTTP_STATUS_CODE_CHARAC_STR,                     /* 0x2AB8 */
  CSR_BT_GATT_UUID_HTTP_ENTITY_BODY_CHARAC_STR,                     /* 0x2AB9 */
  CSR_BT_GATT_UUID_HTTP_CONTROL_POINT_CHARAC_STR,                   /* 0x2ABA */
  CSR_BT_GATT_UUID_HTTPS_SECURITY_CHARAC_STR,                       /* 0x2ABB */
  CSR_BT_GATT_UUID_TDS_CONTROL_POINT_CHARAC_STR,                    /* 0x2ABC */
  CSR_BT_GATT_UUID_OTS_FEATURE_CHARAC_STR,                          /* 0x2ABD */
  CSR_BT_GATT_UUID_OBJECT_NAME_CHARAC_STR,                          /* 0x2ABE */
  CSR_BT_GATT_UUID_OBJECT_TYPE_CHARAC_STR,                          /* 0x2ABF */
  CSR_BT_GATT_UUID_OBJECT_SIZE_CHARAC_STR,                          /* 0x2AC0 */
  CSR_BT_GATT_UUID_OBJECT_FIRST_CREATED_CHARAC_STR,                 /* 0x2AC1 */
  CSR_BT_GATT_UUID_OBJECT_LAST_MODIFIED_CHARAC_STR,                 /* 0x2AC2 */
  CSR_BT_GATT_UUID_OBJECT_ID_CHARAC_STR,                            /* 0x2AC3 */
  CSR_BT_GATT_UUID_OBJECT_PROPERTIES_CHARAC_STR,                    /* 0x2AC4 */
  CSR_BT_GATT_UUID_OBJECT_ACTION_CONTROL_POINT_CHARAC_STR,          /* 0x2AC5 */
  CSR_BT_GATT_UUID_OBJECT_LIST_CONTROL_POINT_CHARAC_STR,            /* 0x2AC6 */
  CSR_BT_GATT_UUID_OBJECT_LIST_FILTER_CHARAC_STR,                   /* 0x2AC7 */
  CSR_BT_GATT_UUID_OBJECT_CHANGED_CHARAC_STR,                       /* 0x2AC8 */
  CSR_BT_GATT_UUID_RESOLVABLE_PRIVATE_ADDRESS_ONLY_CHARAC_STR,      /* 0x2AC9 */
};


const CsrCharString *CsrBtGattAppUuid16ToCharName(CsrBtUuid16 uuid)
{
    const CsrCharString *tmp = NULL;

    if (uuid >= CSR_BT_GATT_UUID_DECLRATIONS_START &&
        uuid <= CSR_BT_GATT_UUID_DECLRATIONS_END)
    {
        tmp = declarationName[uuid - CSR_BT_GATT_UUID_DECLRATIONS_START];
    }
    else if (uuid >= CSR_BT_GATT_UUID_DESCRIPTORS_START &&
             uuid <= CSR_BT_GATT_UUID_DESCRIPTORS_END)
    {
        tmp = descriptorName[uuid - CSR_BT_GATT_UUID_DESCRIPTORS_START];
    }
    else if (uuid >= CSR_BT_GATT_UUID_SERVICES_START &&
             uuid <= CSR_BT_GATT_UUID_SERVICES_END)
    {
        tmp = serviceName[uuid - CSR_BT_GATT_UUID_SERVICES_START];
    }
    else if (uuid >= CSR_BT_GATT_UUID_CHARACTERISTICS_START &&
             uuid <= CSR_BT_GATT_UUID_CHARACTERISTICS_END)
    {
        tmp = characName[uuid - CSR_BT_GATT_UUID_CHARACTERISTICS_START];
    }

    if (tmp)
    {
        return (tmp);
    }
    else
    {
        return (CSR_BT_GATT_UUID_UNKOWN_STR);
    }
}

