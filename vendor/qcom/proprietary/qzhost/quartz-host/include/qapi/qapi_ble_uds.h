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
 * @file qapi_ble_uds.h
 *
 * @brief
 * QCA QAPI for Bluetopia Bluetooth User Data Service
 * (GATT based) API Type Definitions, Constants, and
 * Prototypes.
 *
 * @details
 * The User Data Service (UDS) programming interface defines the
 * protocols and procedures to be used to implement the User Data Service
 * capabilities.
 */

#ifndef __QAPI_BLE_UDS_H__
#define __QAPI_BLE_UDS_H__

#include "./qapi_ble_btapityp.h"  /* Bluetooth API Type Definitions.          */
#include "./qapi_ble_bttypes.h"   /* Bluetooth Type Definitions/Constants.    */
#include "./qapi_ble_gatt.h"      /* qapi GATT prototypes.                    */
#include "./qapi_ble_udstypes.h"  /* QAPI UDS prototypes.                     */

   /* Error Return Codes.                                               */

   /* Error Codes that are smaller than these (less than -1000) are     */
   /* related to the Bluetooth Protocol Stack itself (see               */
   /* qapi_ble_errors.h).                                               */
#define QAPI_BLE_UDS_ERROR_INVALID_PARAMETER                           (-1000)
/**< Invalid parameter. */
#define QAPI_BLE_UDS_ERROR_INVALID_BLUETOOTH_STACK_ID                  (-1001)
/**< Invalid Bluetooth Stack ID. */
#define QAPI_BLE_UDS_ERROR_INSUFFICIENT_RESOURCES                      (-1002)
/**< Insufficient resources. */
#define QAPI_BLE_UDS_ERROR_INSUFFICIENT_BUFFER_SPACE                   (-1003)
/**< Insufficient buffer space. */
#define QAPI_BLE_UDS_ERROR_SERVICE_ALREADY_REGISTERED                  (-1004)
/**< Service already registered. */
#define QAPI_BLE_UDS_ERROR_INVALID_INSTANCE_ID                         (-1005)
/**< Invalid service instance ID. */
#define QAPI_BLE_UDS_ERROR_MALFORMATTED_DATA                           (-1006)
/**< Malformatted data. */

#define QAPI_BLE_UDS_ERROR_INVALID_UDS_CHARACTERISTIC_FLAGS            (-1007)
/**< Invalid UDS Characteristic flags. */
#define QAPI_BLE_UDS_ERROR_INVALID_CHARACTERISTIC_TYPE                 (-1008)
/**< Invalid Characteristic type. */
#define QAPI_BLE_UDS_ERROR_INVALID_ATTRIBUTE_HANDLE                    (-1009)
/**< Invalid attribute handle. */
#define QAPI_BLE_UDS_ERROR_INDICATION_OUTSTANDING                      (-1010)
/**< Indication outstanding. */
#define QAPI_BLE_UDS_ERROR_NOTIFICATIONS_NOT_SUPPORTED                 (-1011)
/**< Notifications not supported. */
#define QAPI_BLE_UDS_ERROR_INVALID_USER_CONTROL_POINT_REQUEST_OP_CODE  (-1012)
/**< Invalid user control point request op code. */
#define QAPI_BLE_UDS_ERROR_INVALID_USER_CONTROL_POINT_USER_INDEX       (-1013)
/**< Invalid user control point user index. */
#define QAPI_BLE_UDS_ERROR_INVALID_USER_CONTROL_POINT_CONSENT_CODE     (-1014)
/**< Invalid user control point consent code. */
#define QAPI_BLE_UDS_ERROR_INVALID_OFFSET                              (-1015)
/**< Invalid offset. */

   /* UDS Server Constants, Structures, Events, and Enumerations.       */

   /* The following defines the values of the UDS Characeristic Flags   */
   /* parameter for the qapi_BLE_UDS_Initialize_XXX() functions.        */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_FIRST_NAME                       0x00000001
/**< UDS supports the First Name Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_LAST_NAME                        0x00000002
/**< UDS supports the Last Name Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_EMAIL_ADDRESS                    0x00000004
/**< UDS supports the Email Address Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_AGE                              0x00000008
/**< UDS supports the Age Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_DATE_OF_BIRTH                    0x00000010
/**< UDS supports the Date of Birth Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_GENDER                           0x00000020
/**< UDS supports the Gender Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_WEIGHT                           0x00000040
/**< UDS supports the Weight Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_HEIGHT                           0x00000080
/**< UDS supports the Height Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_VO2_MAX                          0x00000100
/**< UDS supports the V02 Max Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_HEART_RATE_MAX                   0x00000200
/**< UDS supports the Heart Rate Max Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_RESTING_HEART_RATE               0x00000400
/**< UDS supports the Resting Heart Rate Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_MAXIMUM_RECOMMENDED_HEART_RATE   0x00000800
/**< UDS supports the Maximum Recommended Heart Rate Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_AEROBIC_THRESHOLD                0x00001000
/**< UDS supports the Aerobic Threshold Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_ANAEROBIC_THRESHOLD              0x00002000
/**< UDS supports the Anaerobic Threshold Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_SPORT_TYPE                       0x00004000
/**< UDS supports the Sport Type Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_DATE_OF_THRESHOLD                0x00008000
/**< UDS supports the Date of Threshold Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_WAIST_CIRCUMFERENCE              0x00010000
/**< UDS supports the Waist Circumference Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_HIP_CIRCUMFERENCE                0x00020000
/**< UDS supports the Hip Circumference Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_FAT_BURN_HEART_RATE_LOWER_LIMIT  0x00040000
/**< UDS supports the Fat Burn Heart Rate Lower Limit Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_FAT_BURN_HEART_RATE_UPPER_LIMIT  0x00080000
/**< UDS supports the Fat Burn Heart Rate Upper Limit Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_AEROBIC_HEART_RATE_LOWER_LIMIT   0x00100000
/**< UDS supports the Aerobic Heart Rate Lower Limit Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_AEROBIC_HEART_RATE_UPPER_LIMIT   0x00200000
/**< UDS supports the Aerobic Heart Rate Upper Limit Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_ANAEROBIC_HEART_RATE_LOWER_LIMIT 0x00400000
/**< UDS supports the Anaerobic Heart Rate Lower Limit Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_ANAEROBIC_HEART_RATE_UPPER_LIMIT 0x00800000
/**< UDS supports the Anaerobic Heart Rate Upper Limit Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_FIVE_ZONE_HEART_RATE_LIMITS      0x01000000
/**< UDS supports the Five Zone Heart Rate Limits Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_THREE_ZONE_HEART_RATE_LIMITS     0x02000000
/**< UDS supports the Three Zone Heart Rate Limits Characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_TWO_ZONE_HEART_RATE_LIMIT        0x04000000
/**< UDS supports the Two Zone Heart Rate Limit characteristic. */
#define QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_LANGUAGE                         0x08000000
/**< UDS supports the Language Characteristic. */

/**
 * This structure represents the data that is required to initialize the
 * User Data Service (a parameter for the
 * qapi_BLE_UDS_Initialize_Service() and
 * qapi_BLE_UDS_Initialize_Service_Handle_Range() functions).
 *
 * The qapi_BLE_UDS_Characteristic_Flags CANNOT be zero and
 * at least one value for the (bit mask) MUST be specified. These flags
 * have the form QAPI_BLE_UDS_CHARACTERISTIC_FLAGS_XXX, and can be found
 * in qapi_ble_uds.h.
 *
 * The Server_Update_Supported field is used to specify if the UDS Server
 * is capable of updating a UDS Characterisic through its user interface
 * or Out-of_Band (OOB) mechanism. If TRUE, then a Client Characteristic
 * Configuration Descriptor (CCCD) will be included for the Database
 * Change Increment Characteristic. This CCCD MUST be configured for
 * notifications by the UDS Client and allows the UDS Server to send a
 * notification to inform UDS Clients that the UDS Server has updated one
 * or more UDS Characteristics.
 */
typedef struct qapi_BLE_UDS_Initialize_Data_s
{
   /**
    * Controls the optional UDS Characteristics that are included for the
    * UDS instance.
    */
   uint32_t  UDS_Characteristic_Flags;

   /**
    * Flags whether the UDS Server is capable of updating a UDS
    * Characteristic through it's user interface or OOB mechanism.
    */
   boolean_t Server_Update_Supported;
} qapi_BLE_UDS_Initialize_Data_t;

#define QAPI_BLE_UDS_INITIALIZE_DATA_SIZE                         (sizeof(qapi_BLE_UDS_Initialize_Data_t))

/**
 * This enumeration represents the optional UDS Characteristic types.
 */
typedef enum
{
   QAPI_BLE_UDS_CT_FIRST_NAME_E,
   /**< First Name. */
   QAPI_BLE_UDS_CT_LAST_NAME_E,
    /**< Last Name. */
   QAPI_BLE_UDS_CT_EMAIL_ADDRESS_E,
    /**< Email address. */
   QAPI_BLE_UDS_CT_AGE_E,
    /**< Age. */
   QAPI_BLE_UDS_CT_DATE_OF_BIRTH_E,
    /**< Date of Birth. */
   QAPI_BLE_UDS_CT_GENDER_E,
    /**< Gender. */
   QAPI_BLE_UDS_CT_WEIGHT_E,
    /**< Weight. */
   QAPI_BLE_UDS_CT_HEIGHT_E,
    /**< Height. */
   QAPI_BLE_UDS_CT_VO2_MAX_E,
    /**< VO2 Max. */
   QAPI_BLE_UDS_CT_HEART_RATE_MAX_E,
    /**< Heart Rate Max. */
   QAPI_BLE_UDS_CT_RESTING_HEART_RATE_E,
    /**< Resting Heart Rate. */
   QAPI_BLE_UDS_CT_MAXIMUM_RECOMMENDED_HEART_RATE_E,
    /**< Maximum Recommended Heart Rate. */
   QAPI_BLE_UDS_CT_AEROBIC_THRESHOLD_E,
    /**< Aerobic Threshold. */
   QAPI_BLE_UDS_CT_ANAEROBIC_THRESHOLD_E,
    /**< Anaerobic Threshold. */
   QAPI_BLE_UDS_CT_SPORT_TYPE_E,
    /**< Sport type. */
   QAPI_BLE_UDS_CT_DATE_OF_THRESHOLD_E,
    /**< Date of Threshold. */
   QAPI_BLE_UDS_CT_WAIST_CIRCUMFERENCE_E,
    /**< Waist Circumference. */
   QAPI_BLE_UDS_CT_HIP_CIRCUMFERENCE_E,
    /**< Hip Circumference. */
   QAPI_BLE_UDS_CT_FAT_BURN_HEART_RATE_LOWER_LIMIT_E,
    /**< Fat Burn Heart Rate Lower Limit. */
   QAPI_BLE_UDS_CT_FAT_BURN_HEART_RATE_UPPER_LIMIT_E,
    /**< Fat Burn Heart Rate Upper Limit. */
   QAPI_BLE_UDS_CT_AEROBIC_HEART_RATE_LOWER_LIMIT_E,
    /**< Aerobic Heart Rate Lower Limit. */
   QAPI_BLE_UDS_CT_AEROBIC_HEART_RATE_UPPER_LIMIT_E,
    /**< Aerobic Heart Rate Upper Limit. */
   QAPI_BLE_UDS_CT_ANAEROBIC_HEART_RATE_LOWER_LIMIT_E,
    /**< Anaerobic Heart Rate Lower Limit. */
   QAPI_BLE_UDS_CT_ANAEROBIC_HEART_RATE_UPPER_LIMIT_E,
    /**< Anaerobic Heart Rate Upper Limit. */
   QAPI_BLE_UDS_CT_FIVE_ZONE_HEART_RATE_LIMITS_E,
    /**< Five Zone Heart Rate Limits. */
   QAPI_BLE_UDS_CT_THREE_ZONE_HEART_RATE_LIMITS_E,
    /**< Three Zone Heart Rate Limits. */
   QAPI_BLE_UDS_CT_TWO_ZONE_HEART_RATE_LIMIT_E,
    /**< Two Zone Heart Rate Limit. */
   QAPI_BLE_UDS_CT_LANGUAGE_E
    /**< Language. */
} qapi_BLE_UDS_Characteristic_Type_t;

/**
 * This enumeration represents the UDS Characteristics that contain a
 * Client Characteristic Configuration descriptor.
 */
typedef enum
{
   QAPI_BLE_UDS_CCT_DATABASE_CHANGE_INCREMENT,
   /**< Database Change Increment. */
   QAPI_BLE_UDS_CCT_USER_CONTROL_POINT
    /**< User Control Point. */
} qapi_BLE_UDS_CCCD_Characteristic_Type_t;

/**
 * This structure represents all of the attribute handles for the
 * optional UDS Characteristics that may need to be cached by the UDS
 * Client if supported by the UDS Server.
 *
 * The UDS Server MUST support at least one optional UDS Characteristic.
 */
typedef struct qapi_BLE_UDS_Characteristic_Handles_s
{
   /**
    * The UDS First Name attribute handle.
    */
   uint16_t First_Name;

   /**
    * The UDS Last Name attribute handle.
    */
   uint16_t Last_Name;

   /**
    * The UDS Email Address attribute handle.
    */
   uint16_t Email_Address;

   /**
    * The UDS Age attribute handle.
    */
   uint16_t Age;

   /**
    * The UDS Date of Birth attribute handle.
    */
   uint16_t Date_Of_Birth;

   /**
    * The UDS Gender attribute handle.
    */
   uint16_t Gender;

   /**
    * The UDS Weigth attribute handle.
    */
   uint16_t Weight;

   /**
    * The UDS Height attribute handle.
    */
   uint16_t Height;

   /**
    * The UDS VO2 Max attribute handle.
    */
   uint16_t VO2_Max;

   /**
    * The UDS Heart Rate Max attribute handle.
    */
   uint16_t Heart_Rate_Max;

   /**
    * The UDS Resting Heart Rate attribute handle.
    */
   uint16_t Resting_Heart_Rate;

   /**
    * The UDS Maximum Recommended Heart Rate attribute handle.
    */
   uint16_t Maximum_Recommended_Heart_Rate;

   /**
    * The UDS Aerobic Threshold attribute handle.
    */
   uint16_t Aerobic_Threshold;

   /**
    * The UDS Anaerobic Threshold attribute handle.
    */
   uint16_t Anaerobic_Threshold;

   /**
    * The UDS Sport Type attribute handle.
    */
   uint16_t Sport_Type;

   /**
    * The UDS Date of Threshold attribute handle.
    */
   uint16_t Date_Of_Threshold;

   /**
    * The UDS Waist Circumference attribute handle.
    */
   uint16_t Waist_Circumference;

   /**
    * The UDS Hip Circumference attribute handle.
    */
   uint16_t Hip_Circumference;

   /**
    * The UDS Fat Burn Heart Rate Lower Limit attribute handle.
    */
   uint16_t Fat_Burn_Heart_Rate_Lower_Limit;

   /**
    * The UDS Fat Burn Heart Rate Upper Limit attribute handle.
    */
   uint16_t Fat_Burn_Heart_Rate_Upper_Limit;

   /**
    * The UDS Aerobic Heart Rate Lower Limit attribute handle.
    */
   uint16_t Aerobic_Heart_Rate_Lower_Limit;

   /**
    * The UDS Aerobic Heart Rate Upper Limit attribute handle.
    */
   uint16_t Aerobic_Heart_Rate_Upper_Limit;

   /**
    * The UDS Anaerobic Heart Rate Lower Limit attribute handle.
    */
   uint16_t Anaerobic_Heart_Rate_Lower_Limit;

   /**
    * The UDS Anaerobic Heart Rate Upper Limit attribute handle.
    */
   uint16_t Anaerobic_Heart_Rate_Upper_Limit;

   /**
    * The UDS Five Zone Heart Rate Limits attribute handle.
    */
   uint16_t Five_Zone_Heart_Rate_Limits;

   /**
    * The UDS Three Zone Heart Rate Limits attribute handle.
    */
   uint16_t Three_Zone_Heart_Rate_Limits;

   /**
    * The UDS Two Zone Heart Rate Limit attribute handle.
    */
   uint16_t Two_Zone_Heart_Rate_Limit;

   /**
    * The UDS Language attribute handle.
    */
   uint16_t Language;
} qapi_BLE_UDS_Characteristic_Handles_t;

/**
 * This structure contains the attribute handles that will need to be
 * cached by a UDS Client in order to only do service discovery once.
 *
 * The Characteristic field contains the optional UDS
 * Characteristic handles that may need to be cached by a UDS Client. The
 * UDS Server MUST support at least one optional UDS Characteristic.
 */
typedef struct qapi_BLE_UDS_Client_Information_s
{
   /**
    * The UDS Characteristic attribute handles.
    */
   qapi_BLE_UDS_Characteristic_Handles_t Characteristic_Handles;

   /**
    * The UDS Database Change Increment attribute handle.
    */
   uint16_t                              Database_Change_Increment;

   /**
    * The UDS Database Change Increment Client Characteristic
    * Configuration Descriptor (CCCD) attribute handle.
    */
   uint16_t                              Database_Change_Increment_CCCD;

   /**
    * The UDS User Index attribute handle.
    */
   uint16_t                              User_Index;

   /**
    * The UDS User Control Point attribute handle.
    */
   uint16_t                              User_Control_Point;

   /**
    * The UDS User Control Point Client Characteristic
    * Configuration Descriptor (CCCD) attribute handle.
    */
   uint16_t                              User_Control_Point_CCCD;
} qapi_BLE_UDS_Client_Information_t;

#define QAPI_BLE_UDS_CLIENT_INFORMATION_DATA_SIZE                 (sizeof(qapi_BLE_UDS_Client_Information_t))
/**<
 * Size of the qapi_BLE_UDS_Client_Information_t structure.
 */

/**
 * This structure contains all of the per Client data that will need to
 * be stored by a UDS Server.
 */
typedef struct qapi_BLE_UDS_Server_Information_s
{
   /**
    * The UDS Database Change Increment Client Characteristic
    * Configuration Descriptor (CCCD).
    */
   uint16_t Database_Change_Increment_Configuration;

   /**
    * The UDS User Control Point Client Characteristic
    * Configuration Descriptor (CCCD).
    */
   uint16_t User_Control_Point_Configuration;
} qapi_BLE_UDS_Server_Information_t;

#define QAPI_BLE_UDS_SERVER_INFORMATION_DATA_SIZE                 (sizeof(qapi_BLE_UDS_Server_Information_t))
/**<
 * Size of the qapi_BLE_UDS_Server_Information_t structure.
 */

/**
 * This structure represents the UDS String data for the following UDS
 * Characteristics: First Name, Last Name, Email Address, and Language.
 */
typedef struct qapi_BLE_UDS_String_Data_s
{
   /**
    * The length of the buffer that contains the string data.
    */
   uint16_t  Buffer_Length;

   /**
    * The buffer that contains the string data.
    */
   uint8_t  *Buffer;
} qapi_BLE_UDS_String_Data_t;

/**
 * This structure represents the format for the Date of Birth
 * UDS Characteristic.
 */
typedef struct qapi_BLE_UDS_Date_Data_s
{
   /**
    * The Year.
    */
   uint16_t Year;

   /**
    * The Month.
    */
   uint8_t  Month;

   /**
    * The Day.
    */
   uint8_t  Day;
} qapi_BLE_UDS_Date_Data_t;

#define QAPI_BLE_UDS_DATE_DATA_SIZE                               (sizeof(qapi_BLE_UDS_Date_Data_t))
/**<
 * Size of the qapi_BLE_UDS_Date_Data_t structure.
 */

#define QAPI_BLE_UDS_DATE_OF_BIRTH_VALID(_x)                      ((QAPI_BLE_GATT_DATE_TIME_VALID_YEAR(((_x)).Year)) && (QAPI_BLE_GATT_DATE_TIME_VALID_MONTH(((_x)).Month)) && (QAPI_BLE_GATT_DATE_TIME_VALID_DAY(((_x)).Day)))
/**<
 * This MACRO is a utility MACRO that exists to validate that a specified
 * Date of Birth is valid. The only parameter to this function is the
 * qapi_BLE_UDS_Date_Data_t structure to validate. This MACRO returns
 * TRUE if the Date Time is valid or FALSE otherwise..
 */

/**
 * This structure represents the format for the Five Zone Heart Rate
 * Limits UDS Characteristic.
 */
typedef struct qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_s
{
   /**
    * The Light Limit.
    */
   uint8_t Light_Limit;

   /**
    * The Light Moderate Limit.
    */
   uint8_t Light_Moderate_Limit;

   /**
    * The Moderate Hard Limit.
    */
   uint8_t Moderate_Hard_Limit;

   /**
    * The Hard Maximum Limit.
    */
   uint8_t Hard_Maximum_Limit;
} qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_t;

#define QAPI_BLE_UDS_FIVE_ZONE_HEART_RATE_LIMITS_DATA_SIZE        (sizeof(qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_t))
/**<
 * Size of the qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_t structure.
 */

/**
 * This structure represents the format for the Three Zone Heart Rate
 * Limits UDS Characteristic.
 */
typedef struct qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_s
{
   /**
    * The Light Moderate Limit.
    */
   uint8_t Light_Moderate_Limit;

   /**
    * The Moderate Hard Limit.
    */
   uint8_t Moderate_Hard_Limit;
} qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_t;

#define QAPI_BLE_UDS_THREE_ZONE_HEART_RATE_LIMITS_DATA_SIZE       (sizeof(qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_t))
/**<
 * Size of the qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_t
 * structure.
 */

/**
 * This union represents the possible data types for a UDS
 * Characteristic.
 */
typedef union
{
   /**
    * The UDS First Name.
    */
   qapi_BLE_UDS_String_Data_t                       First_Name;

   /**
    * The UDS Last Name.
    */
   qapi_BLE_UDS_String_Data_t                       Last_Name;

   /**
    * The UDS Email Address.
    */
   qapi_BLE_UDS_String_Data_t                       Email_Address;

   /**
    * The UDS Age.
    */
   uint8_t                                          Age;

   /**
    * The UDS Date of Birth.
    */
   qapi_BLE_UDS_Date_Data_t                         Date_Of_Birth;

   /**
    * The UDS Gender.
    */
   uint8_t                                          Gender;

   /**
    * The UDS Weigth.
    */
   uint16_t                                         Weight;

   /**
    * The UDS Height.
    */
   uint16_t                                         Height;

   /**
    * The UDS VO2 Max.
    */
   uint8_t                                          VO2_Max;

   /**
    * The UDS Heart Rate Max.
    */
   uint8_t                                          Heart_Rate_Max;

   /**
    * The UDS Resting Heart Rate.
    */
   uint8_t                                          Resting_Heart_Rate;

   /**
    * The UDS Maximum Recommended Heart Rate.
    */
   uint8_t                                          Maximum_Recommended_Heart_Rate;

   /**
    * The UDS Aerobic Threshold.
    */
   uint8_t                                          Aerobic_Threshold;

   /**
    * The UDS Anaerobic Threshold.
    */
   uint8_t                                          Anaerobic_Threshold;

   /**
    * The UDS Sport Type.
    */
   uint8_t                                          Sport_Type;

   /**
    * The UDS Date of Threshold.
    */
   qapi_BLE_UDS_Date_Data_t                         Date_Of_Threshold;

   /**
    * The UDS Waist Circumference.
    */
   uint16_t                                         Waist_Circumference;

   /**
    * The UDS Hip Circumference.
    */
   uint16_t                                         Hip_Circumference;

   /**
    * The UDS Fat Burn Heart Rate Lower Limit.
    */
   uint8_t                                          Fat_Burn_Heart_Rate_Lower_Limit;

   /**
    * The UDS Fat Burn Heart Rate Upper Limit.
    */
   uint8_t                                          Fat_Burn_Heart_Rate_Upper_Limit;

   /**
    * The UDS Aerobic Heart Rate Lower Limit.
    */
   uint8_t                                          Aerobic_Heart_Rate_Lower_Limit;

   /**
    * The UDS Aerobic Heart Rate Upper Limit.
    */
   uint8_t                                          Aerobic_Heart_Rate_Upper_Limit;

   /**
    * The UDS Anaerobic Heart Rate Lower Limit.
    */
   uint8_t                                          Anaerobic_Heart_Rate_Lower_Limit;

   /**
    * The UDS Anaerobic Heart Rate Upper Limit.
    */
   uint8_t                                          Anaerobic_Heart_Rate_Upper_Limit;

   /**
    * The UDS Five Zone Heart Rate Limits.
    */
   qapi_BLE_UDS_Five_Zone_Heart_Rate_Limits_Data_t  Five_Zone_Heart_Rate_Limits;

   /**
    * The UDS Three Zone Heart Rate Limits.
    */
   qapi_BLE_UDS_Three_Zone_Heart_Rate_Limits_Data_t Three_Zone_Heart_Rate_Limits;

   /**
    * The UDS Two Zone Heart Rate Limits.
    */
   uint8_t                                          Two_Zone_Heart_Rate_Limit;

   /**
    * The UDS Language.
    */
   qapi_BLE_UDS_String_Data_t                       Language;
} qapi_BLE_UDS_Characteristic_t;

/**
 * This enumeration represents he possible UDS User Control Point Request
 * types (Request Op Codes) that may be set for the Op_Code field of the
 * qapi_BLE_UDS_User_Control_Point_Request_Data_t structure and the
 * Request_Op_Code field of the
 * qapi_BLE_UDS_User_Control_Point_Response_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_UDS_UCPRT_REGISTER_NEW_USER_E = QAPI_BLE_UDS_USER_CONTROL_POINT_OP_CODE_REGISTER_NEW_USER,
   /**< Register new user op code. */
   QAPI_BLE_UDS_UCPRT_CONSENT_E           = QAPI_BLE_UDS_USER_CONTROL_POINT_OP_CODE_CONSENT,
   /**< Consent op code. */
   QAPI_BLE_UDS_UCPRT_DELETE_USER_DATA_E  = QAPI_BLE_UDS_USER_CONTROL_POINT_OP_CODE_DELETE_USER_DATA
   /**< Delete user data op code. */
} qapi_BLE_UDS_User_Control_Point_Request_Type_t;

/**
 * This union represents the format of the UDS User Control Point request
 * data.
 *
 * If the Request_Op_Code field is set to
 * QAPI_BLE_UDS_UCPRT_REGISTER_NEW_USER_E, then the Consent_Code field of
 * the internal Parameter field MUST be valid.  The Consent code is
 * required to register a new user on the UDS Server.  It is worth noting
 * that the User Index for the successfully registered user will be
 * received in the User Control Point response indication if the user was
 * successfully registered.
 *
 * If the Request_Op_Code field is set to QAPI_BLE_UDS_UCPRT_CONSENT_E,
 * then the Consent_Code and User_Index fields of the internal Parameter
 * field MUST BOTH be valid. The User_Index is used to indicate which
 * user on the UDS Server the UDS Client wishes to access.  The
 * Consent_Code MUST be valid for the consent code that was set when the
 * user was registered.  It is worth noting that the number of consent
 * attempts is left up to the implementation of the UDS Server before
 * access is denied.
 */
typedef struct qapi_BLE_UDS_User_Control_Point_Request_Data_s
{
   /**
    * The op code for the UCP request.
    */
   qapi_BLE_UDS_User_Control_Point_Request_Type_t Op_Code;
   struct
   {
      /**
       * The consent code parameter.
       */
      uint16_t Consent_Code;

      /**
       * The user index parameter.
       */
      uint8_t  User_Index;
   }
   /**
    * The optional parameter for the UCP request. One or all structure
    * fields will be used depending on the value of the Request_Op_Code.
    */
   Parameter;
} qapi_BLE_UDS_User_Control_Point_Request_Data_t;

#define QAPI_BLE_UDS_USER_CONTROL_POINT_REQUEST_DATA_SIZE         (sizeof(qapi_BLE_UDS_User_Control_Point_Request_Data_t))
/**<
 * Size of the qapi_BLE_UDS_User_Control_Point_Request_Data_t structure.
 */

/**
 * This enumeration represents the valid values that may be set as the
 * value for the Response_Code_Value field of the
 * qapi_BLE_UDS_User_Control_Point_Response_Data_t structure.
 */
typedef enum
{
   QAPI_BLE_UDS_UCPRV_SUCCESS_E              = QAPI_BLE_UDS_USER_CONTROL_POINT_RESPONSE_VALUE_SUCCESS,
   /**< Success response value. */
   QAPI_BLE_UDS_UCPRV_OPCODE_NOT_SUPPORTED_E = QAPI_BLE_UDS_USER_CONTROL_POINT_RESPONSE_VALUE_OP_CODE_NOT_SUPPORTED,
   /**< Opcode not supported response value. */
   QAPI_BLE_UDS_UCPRV_INVALID_PARAMETER_E    = QAPI_BLE_UDS_USER_CONTROL_POINT_RESPONSE_VALUE_INVALID_PARAMETER,
   /**< Invalid parameter response value. */
   QAPI_BLE_UDS_UCPRV_OPERATION_FAILED_E     = QAPI_BLE_UDS_USER_CONTROL_POINT_RESPONSE_VALUE_OPERATION_FAILED,
   /**< Operation failed response value. */
   QAPI_BLE_UDS_UCPRV_USER_NOT_AUTHORIZED_E  = QAPI_BLE_UDS_USER_CONTROL_POINT_RESPONSE_VALUE_USER_NOT_AUTHORIZED
   /**< User not authorized response value. */
} qapi_BLE_UDS_User_Control_Point_Response_Value_t;

/**
 * This union represents the format of the UDS User Control Point
 * response data.
 *
 * If the Request_Op_Code field is set to
 * QAPI_BLE_UDS_UCPRT_REGISTER_NEW_USER_E, and the Response_Code_Value
 * field is set to QAPI_BLE_UDS_UCPRV_SUCCESS_E, then the User_Index
 * field of the internal Parameter field MUST be valid. This indicates
 * the User Index for the successfully registered user. It is worth
 * noting that the User Index Characteristic will not be updated till the
 * UDS Client has acquired consent to access the user's data via the User
 * Control Point Consent procedure. However, the UDS Client will need
 * this User Index to request consent.
 */
typedef struct qapi_BLE_UDS_User_Control_Point_Response_Data_s
{
   /**
    * The op code for the UCP request.
    */
   qapi_BLE_UDS_User_Control_Point_Request_Type_t   Request_Op_Code;

   /**
    * The response code value for the UCP response.
    */
   qapi_BLE_UDS_User_Control_Point_Response_Value_t Response_Code_Value;
   union
   {
      /**
       * The user index parameter.
       */
      uint8_t User_Index;
   }
   /**
    * The optional parameter that MUST be included for certain UCP
    * request op codes.
    */
   Parameter;
} qapi_BLE_UDS_User_Control_Point_Response_Data_t;

#define QAPI_BLE_UDS_USER_CONTROL_POINT_RESPONSE_DATA_SIZE        (sizeof(qapi_BLE_UDS_User_Control_Point_Response_Data_t))
/**<
 * Size of the qapi_BLE_UDS_User_Control_Point_Response_Data_t structure.
 */

/**
 * This enumeration represents all the events generated by the UDS
 * Service. These are used to determine the type of each event generated,
 * and to ensure the proper union element is accessed for the
 * qapi_BLE_UDS_Event_Data_t structure.
 */
typedef enum qapi_BLE_UDS_Event_Type_s
{
   QAPI_BLE_UDS_ET_SERVER_READ_CHARACTERISTIC_REQUEST_E,
   /**< Read Characteristic request event. */
   QAPI_BLE_UDS_ET_SERVER_WRITE_CHARACTERISTIC_REQUEST_E,
   /**< Write Characteristic request event. */
   QAPI_BLE_UDS_ET_SERVER_PREPARE_WRITE_CHARACTERISTIC_REQUEST_E,
   /**< Prepare Write Characteristic request event. */
   QAPI_BLE_UDS_ET_SERVER_READ_CCCD_REQUEST_E,
   /**< Read CCCD request event. */
   QAPI_BLE_UDS_ET_SERVER_WRITE_CCCD_REQUEST_E,
   /**< Write CCCD request event. */
   QAPI_BLE_UDS_ET_SERVER_READ_DATABASE_CHANGE_INCREMENT_REQUEST_E,
   /**< Read Database Change Increment request event. */
   QAPI_BLE_UDS_ET_SERVER_WRITE_DATABASE_CHANGE_INCREMENT_REQUEST_E,
   /**< Write Database Change request event. */
   QAPI_BLE_UDS_ET_SERVER_READ_USER_INDEX_REQUEST_E,
   /**< Read User Index request event. */
   QAPI_BLE_UDS_ET_SERVER_WRITE_USER_CONTROL_POINT_REQUEST_E,
   /**< Write User Control Point request event. */
   QAPI_BLE_UDS_ET_SERVER_CONFIRMATION_DATA_E
   /**< Confirmation event. */
} qapi_BLE_UDS_Event_Type_t;

/**
 * This structure represents the format for the data that is dispatched
 * to a UDS Server when a UDS Client has sent a request to read a UDS
 * Characteristic.
 *
 * The Offset field will be zero for the GATT Read Value request, which
 * is ALWAYS sent first to read a UDS Characteristic that has the
 * qapi_BLE_UDS_String_Data_t structure data type. However, if the UDS
 * Characteristic is set to any of the following:
 *
 * @li QAPI_BLE_UDS_CT_FIRST_NAME_E
 * @li QAPI_BLE_UDS_CT_LAST_NAME_E
 * @li QAPI_BLE_UDS_CT_EMAIL_ADDRESS_E
 * @li QAPI_BLE_UDS_CT_LANGUAGE_E
 *
 * and the UDS Characteristic response, CANNOT fit in the GATT Read Value
 * response due to the GATT Maximum Transmission Unit (MTU) constraints,
 * then the UDS Client MUST issue GATT Read Long Value requests until the
 * entire UDS Characteristic has been read. If a GATT Read Long Value
 * request has been sent, then the Offset field will be non-zero.
 *
 * Some of this structures fields will be required when responding to a
 * request using the
 * qapi_BLE_UDS_Read_Characteristic_Request_Response() function.
 */
typedef struct qapi_BLE_UDS_Read_Characteristic_Request_Data_s
{
   /**
    * The UDS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * The GATT Connection ID for the connection with the UDS Client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * The GATT Connection type, which identifies the transport used for
    * the connection with the UDS Client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * The GATT Transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * The Bluetooth address of the UDS Client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the UDS Characteristic that has been requested to be
    * read.
    */
   qapi_BLE_UDS_Characteristic_Type_t Type;

   /**
    * The Offset field is used to index the starting
    * offset for the UDS Characteristics that have the
    * qapi_BLE_UDS_String_Data_t structure data type.
    */
   uint16_t                           Offset;
} qapi_BLE_UDS_Read_Characteristic_Request_Data_t;

#define QAPI_BLE_UDS_READ_CHARACTERISTIC_REQUEST_DATA_SIZE        (sizeof(qapi_BLE_UDS_Read_Characteristic_Request_Data_t))
/**<
 * Size of the qapi_BLE_UDS_Read_Characteristic_Request_Data_t structure.
 */

/**
 * This structure represents the format for the data that is dispatched
 * to a UDS Server when a UDS Client has sent a request to write a UDS
 * Characteristic.
 *
 * Some of this structures fields will be required when responding to a
 * request using the
 * qapi_BLE_UDS_Write_Characteristic_Request_Response() function.
 */
typedef struct qapi_BLE_UDS_Write_Characteristic_Request_Data_s
{
   /**
    * The UDS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * The GATT Connection ID for the connection with the UDS Client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * The GATT Connection type, which identifies the transport used for
    * the connection with the UDS Client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * The GATT Transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * The Bluetooth address of the UDS Client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the UDS Characteristic that has been requested to be
    * written.
    */
   qapi_BLE_UDS_Characteristic_Type_t Type;

   /**
    * A union for the UDS Characteristic data that has been requested to
    * be written. The element accessed depends on the value of the Type
    * field.
    */
   qapi_BLE_UDS_Characteristic_t      UDS_Characteristic;
} qapi_BLE_UDS_Write_Characteristic_Request_Data_t;

#define QAPI_BLE_UDS_WRITE_CHARACTERISTIC_REQUEST_DATA_SIZE       (sizeof(qapi_BLE_UDS_Write_Characteristic_Request_Data_t))
/**<
 * Size of the qapi_BLE_UDS_Write_Characteristic_Request_Data_t
 * structure.
 */

/**
 * This structure represents the format for the data that is dispatched
 * to a UDS Server when a UDS Client has sent a request to prepare data
 * that will be written at a later time, for a UDS Characteristic.
 *
 * This event is primarily provided to reject a GATT Prepare Write
 * request for optional security reasons such as the UDS Client has
 * insufficient authentication, authorization, or encryption.  Therefore
 * we will not pass the prepared data up to the application until the the
 * GATT Execute Write request has been received by the
 * UDS Server, and the prepared writes are not cancelled. If the prepared
 * data is written the QAPI_BLE_UDS_ET_SERVER_WRITE_CHARACTERISTIC_REQUEST_E
 * event will be dispatched to the application.  Otherwise the prepared
 * data will be cleared.
 *
 * Only the following UDS Characteristic types
 * (qapi_BLE_UDS_String_Data_t structure data type) are valid for this
 * event:
 *
 * @li QAPI_BLE_UDS_CT_FIRST_NAME_E
 * @li QAPI_BLE_UDS_CT_LAST_NAME_E
 * @li QAPI_BLE_UDS_CT_EMAIL_ADDRESS_E
 * @li QAPI_BLE_UDS_CT_LANGUAGE_E
 *
 * Some of this structures fields will be required when responding to a
 * request using the
 * qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Response() function.
 */
typedef struct qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Data_s
{
   /**
    * The UDS instance that dispatched the event.
    */
   uint32_t                           InstanceID;

   /**
    * The GATT Connection ID for the connection with the UDS Client
    * that made the request.
    */
   uint32_t                           ConnectionID;

   /**
    * The GATT Connection type, which identifies the transport used for
    * the connection with the UDS Client.
    */
   qapi_BLE_GATT_Connection_Type_t    ConnectionType;

   /**
    * The GATT Transaction ID for the request.
    */
   uint32_t                           TransactionID;

   /**
    * The Bluetooth address of the UDS Client that made the request.
    */
   qapi_BLE_BD_ADDR_t                 RemoteDevice;

   /**
    * Identifies the UDS Characteristic that has been requested to be
    * read.
    */
   qapi_BLE_UDS_Characteristic_Type_t Type;
} qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Data_t;

#define QAPI_BLE_UDS_PREPARE_WRITE_CHARACTERISTIC_REQUEST_DATA_SIZE  (sizeof(qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Data_t))
/**<
 * Size of the qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Data_t
 * structure.
 */

/**
 * This structure represents the format for the data that is dispatched
 * to a UDS Server when a UDS Client has sent a request to read a UDS
 * Characteristic's Client Characteristic Configuration Descriptor
 * (CCCD).
 *
 * If the UDS Server does NOT support updating of a UDS Characteristic
 * through it's user interfrace or out-of-band mechanism, then the CCCD
 * will not be included for the Database Change Increment
   Characteristic.
 *
 * Some of this structures fields will be required when responding to a
 * request using the
 * qapi_BLE_UDS_Read_CCCD_Request_Response() function.
 */
typedef struct qapi_BLE_UDS_Read_CCCD_Request_Data_s
{
   /**
    * The UDS instance that dispatched the event.
    */
   uint32_t                                InstanceID;

   /**
    * The GATT Connection ID for the connection with the UDS Client
    * that made the request.
    */
   uint32_t                                ConnectionID;

   /**
    * The GATT Connection type, which identifies the transport used for
    * the connection with the UDS Client.
    */
   qapi_BLE_GATT_Connection_Type_t         ConnectionType;

   /**
    * The GATT Transaction ID for the request.
    */
   uint32_t                                TransactionID;

   /**
    * The Bluetooth address of the UDS Client that made the request.
    */
   qapi_BLE_BD_ADDR_t                      RemoteDevice;

   /**
    * Identifies the requested CCCD.
    */
   qapi_BLE_UDS_CCCD_Characteristic_Type_t Type;
} qapi_BLE_UDS_Read_CCCD_Request_Data_t;

#define QAPI_BLE_UDS_READ_CCCD_REQUEST_DATA_SIZE                  (sizeof(qapi_BLE_UDS_Read_CCCD_Request_Data_t))
/**<
 * Size of the qapi_BLE_UDS_Read_CCCD_Request_Data_t structure.
 */

/**
 * This structure represents the format for the data that is dispatched
 * to a UDS Server when a UDS Client has sent a request to write a UDS
 * Characteristic's Client Characteristic Configuration Descriptor
 * (CCCD).
 *
 * If the UDS Server does NOT support updating of a UDS Characteristic
 * through it's user interfrace or out-of-band mechanism, then the CCCD
 * will not be included for the Database Change Increment
   Characteristic.
 *
 * Some of this structures fields will be required when responding to a
 * request using the
 * qapi_BLE_UDS_Write_CCCD_Request_Response() function.
 */
typedef struct qapi_BLE_UDS_Write_CCCD_Request_Data_s
{
   /**
    * The UDS instance that dispatched the event.
    */
   uint32_t                                InstanceID;

   /**
    * The GATT Connection ID for the connection with the UDS Client
    * that made the request.
    */
   uint32_t                                ConnectionID;

   /**
    * The GATT Connection type, which identifies the transport used for
    * the connection with the UDS Client.
    */
   qapi_BLE_GATT_Connection_Type_t         ConnectionType;

   /**
    * The GATT Transaction ID for the request.
    */
   uint32_t                                TransactionID;

   /**
    * The Bluetooth address of the UDS Client that made the request.
    */
   qapi_BLE_BD_ADDR_t                      RemoteDevice;

   /**
    * Identifies the requested CCCD.
    */
   qapi_BLE_UDS_CCCD_Characteristic_Type_t Type;

   /**
    * The Client Characteristic Configuration Descriptor (CCCD) value
    * that has been requested to be written.
    */
   uint16_t                                ClientConfiguration;
} qapi_BLE_UDS_Write_CCCD_Request_Data_t;

#define QAPI_BLE_UDS_WRITE_CCCD_REQUEST_DATA_SIZE                 (sizeof(qapi_BLE_UDS_Write_CCCD_Request_Data_t))
/**<
 * Size of the qapi_BLE_UDS_Write_CCCD_Request_Data_t structure.
 */

/**
 * This structure represents the format for the data that is dispatched
 * to a UDS Server when a UDS Client has sent a request to read the UDS
 * Database Change Increment Characteristic.
 *
 * Some of this structures fields will be required when responding to a
 * request using the
 * qapi_BLE_UDS_Database_Change_Increment_Read_Request_Response()
 * function.
 */
typedef struct qapi_BLE_UDS_Read_Database_Change_Increment_Request_Data_s
{
   /**
    * The UDS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * The GATT Connection ID for the connection with the UDS Client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * The GATT Connection type, which identifies the transport used for
    * the connection with the UDS Client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * The GATT Transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * The Bluetooth address of the UDS Client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;
} qapi_BLE_UDS_Read_Database_Change_Increment_Request_Data_t;

#define QAPI_BLE_UDS_READ_DATABASE_CHANGE_INCREMENT_REQUEST_DATA_SIZE  (sizeof(qapi_BLE_UDS_Read_Database_Change_Increment_Request_Data_t))
/**<
 * Size of the qapi_BLE_UDS_Read_Database_Change_Increment_Request_Data_t
 * structure.
 */

/**
 * This structure represents the format for the data that is dispatched
 * to a UDS Server when a UDS Client has sent a request to write the UDS
 * Database Change Increment Characteristic.
 *
 * Some of this structures fields will be required when responding to a
 * request using the
 * qapi_BLE_UDS_Database_Change_Increment_Write_Request_Response()
 * function.
 */
typedef struct qapi_BLE_UDS_Write_Database_Change_Increment_Request_Data_s
{
   /**
    * The UDS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * The GATT Connection ID for the connection with the UDS Client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * The GATT Connection type, which identifies the transport used for
    * the connection with the UDS Client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * The GATT Transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * The Bluetooth address of the UDS Client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;

   /**
    * The UDS Database Change Increment value that has been requested to
    * be written.
    */
   uint32_t                        DatabaseChangeIncrement;
} qapi_BLE_UDS_Write_Database_Change_Increment_Request_Data_t;

#define QAPI_BLE_UDS_WRITE_DATABASE_CHANGE_INCREMENT_REQUEST_DATA_SIZE  (sizeof(qapi_BLE_UDS_Write_Database_Change_Increment_Request_Data_t))
/**<
 * Size of the
 * qapi_BLE_UDS_Write_Database_Change_Increment_Request_Data_t structure.
 */

/**
 * This structure represents the format for the data that is dispatched
 * to a UDS Server when a UDS Client has sent a request to read the UDS
 * User Index Characteristic.
 *
 * Some of this structures fields will be required when responding to a
 * request using the
 * qapi_BLE_UDS_User_Index_Read_Request_Response()
 * function.
 */
typedef struct qapi_BLE_UDS_Read_User_Index_Request_Data_s
{
   /**
    * The UDS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * The GATT Connection ID for the connection with the UDS Client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * The GATT Connection type, which identifies the transport used for
    * the connection with the UDS Client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * The GATT Transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * The Bluetooth address of the UDS Client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;
} qapi_BLE_UDS_Read_User_Index_Request_Data_t;

#define QAPI_BLE_UDS_READ_USER_INDEX_REQUEST_DATA_SIZE            (sizeof(qapi_BLE_UDS_Read_User_Index_Request_Data_t))
/**<
 * Size of the qapi_BLE_UDS_Read_User_Index_Request_Data_t structure.
 */

/**
 * This structure represents the format for the data that is dispatched
 * to a UDS Server when a UDS Client has sent a request to write the UDS
 * Control Point Characteristic.
 *
 * Some of this structures fields will be required when responding to a
 * request using the
 * qapi_BLE_UDS_User_Control_Point_Write_Request_Response() function.
 */
typedef struct qapi_BLE_UDS_Write_User_Control_Point_Request_Data_s
{
   /**
    * The UDS instance that dispatched the event.
    */
   uint32_t                                       InstanceID;

   /**
    * The GATT Connection ID for the connection with the UDS Client
    * that made the request.
    */
   uint32_t                                       ConnectionID;

   /**
    * The GATT Connection type, which identifies the transport used for
    * the connection with the UDS Client.
    */
   qapi_BLE_GATT_Connection_Type_t                ConnectionType;

   /**
    * The GATT Transaction ID for the request.
    */
   uint32_t                                       TransactionID;

   /**
    * The Bluetooth address of the UDS Client that made the request.
    */
   qapi_BLE_BD_ADDR_t                             RemoteDevice;

   /**
    * The User Control Point data that has been requested to be written.
    */
   qapi_BLE_UDS_User_Control_Point_Request_Data_t UserControlPoint;
} qapi_BLE_UDS_Write_User_Control_Point_Request_Data_t;

#define QAPI_BLE_UDS_WRITE_USER_CONTROL_POINT_REQUEST_DATA_SIZE   (sizeof(qapi_BLE_UDS_Write_User_Control_Point_Request_Data_t))
/**<
 * Size of the qapi_BLE_UDS_Write_User_Control_Point_Request_Data_t
 * structure.
 */

/**
 * This structure represents the format for the data that is dispatched
 * to the UDS Server when a UDS Client has responded to an outstanding
 * indication that was previously sent by the UDS Server.
 */
typedef struct qapi_BLE_UDS_Confirmation_Data_s
{
   /**
    * The UDS instance that dispatched the event.
    */
   uint32_t                        InstanceID;

   /**
    * The GATT Connection ID for the connection with the UDS Client
    * that made the request.
    */
   uint32_t                        ConnectionID;

   /**
    * The GATT Connection type, which identifies the transport used for
    * the connection with the UDS Client.
    */
   qapi_BLE_GATT_Connection_Type_t ConnectionType;

   /**
    * The GATT Transaction ID for the request.
    */
   uint32_t                        TransactionID;

   /**
    * The Bluetooth address of the UDS Client that made the request.
    */
   qapi_BLE_BD_ADDR_t              RemoteDevice;

   /**
    * The status of the outstanding indication. Values will be of the
    * form QAPI_BLE_GATT_CONFIRMATION_STATUS_XXX and can be found in
    * qapi_ble_gatt.h.
    */
   uint8_t                         Status;

   /**
    * The number of bytes successfully indicated.
    */
   uint16_t                        BytesWritten;
} qapi_BLE_UDS_Confirmation_Data_t;

#define QAPI_BLE_UDS_CONFIRMATION_DATA_SIZE                       (sizeof(qapi_BLE_UDS_Confirmation_Data_t))
/**<
 * Size of the qapi_BLE_UDS_Confirmation_Data_t structure.
 */

/**
 * This structure represents the container structure for holding all the
 * UDS Server event data for a UDS instance.
 */
typedef struct qapi_BLE_UDS_Event_Data_s
{
   /**
    * The event type used to determine the appropriate union member of
    * the Event_Data field to access.
    */
   qapi_BLE_UDS_Event_Type_t  Event_Data_Type;

   /**
    * The total size of the data contained in the event.
    */
   uint8_t                    Event_Data_Size;
   union
   {
      /**
       * UDS Read Characteristic event data.
       */
      qapi_BLE_UDS_Read_Characteristic_Request_Data_t             *UDS_Read_Characteristic_Request_Data;

      /**
       * UDS Write Characteristic event data.
       */
      qapi_BLE_UDS_Write_Characteristic_Request_Data_t            *UDS_Write_Characteristic_Request_Data;

      /**
       * UDS Prepare Write Characteristic event data.
       */
      qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Data_t    *UDS_Prepare_Write_Characteristic_Request_Data;

      /**
       * UDS Read Client Characteristic Configuration Descriptor (CCCD)
       * event data.
       */
      qapi_BLE_UDS_Read_CCCD_Request_Data_t                       *UDS_Read_CCCD_Request_Data;

      /**
       * UDS Write Client Characteristic Configuration Descriptor (CCCD)
       * event data.
       */
      qapi_BLE_UDS_Write_CCCD_Request_Data_t                      *UDS_Write_CCCD_Request_Data;

      /**
       * UDS Read Database Change Increment event data.
       */
      qapi_BLE_UDS_Read_Database_Change_Increment_Request_Data_t  *UDS_Read_Database_Change_Increment_Request_Data;

      /**
       * UDS Write Database Change Increment event data.
       */
      qapi_BLE_UDS_Write_Database_Change_Increment_Request_Data_t *UDS_Write_Database_Change_Increment_Request_Data;

      /**
       * UDS User Index event data.
       */
      qapi_BLE_UDS_Read_User_Index_Request_Data_t                 *UDS_Read_User_Index_Request_Data;

      /**
       * UDS Write User Control Point event data.
       */
      qapi_BLE_UDS_Write_User_Control_Point_Request_Data_t        *UDS_Write_User_Control_Point_Request_Data;

      /**
       * UDS Confirmation event data.
       */
      qapi_BLE_UDS_Confirmation_Data_t                            *UDS_Confirmation_Data;
   }
   /**
    * The event data.
    */
   Event_Data;
} qapi_BLE_UDS_Event_Data_t;

#define QAPI_BLE_UDS_EVENT_DATA_SIZE                              (sizeof(qapi_BLE_UDS_Event_Data_t))
/**<
 * Size of the qapi_BLE_UDS_Event_Data_t structure.
 */

/**
 * @brief
 * The following declared type represents the prototype function for an
 * UDS instance event callback.  This function will be called
 * whenever a UDS instance event occurs that is associated with the
 * specified Bluetooth stack ID.

 * @details
 * The caller is free to use the contents of the UDS instance event data
 * ONLY in the context of this callback.  If the caller requires
 * the data for a longer period of time, then the callback function
 * MUST copy the data into another data buffer.
 *
 * This function is guaranteed NOT to be invoked more than once
 * simultaneously for the specified installed callback (i.e. this
 * function DOES NOT have be reentrant).  It needs to be noted however,
 * that if the same event callback is installed more than once, then the
 * event callbacks will be called serially.  Because of this, the
 * processing in this function should be as efficient as possible.
 *
 * It should also be noted that this function is called in the thread
 * context of a thread that the user does NOT own.  Therefore, processing
 * in this function should be as efficient as possible (this argument holds
 * anyway because another UDS instance event will NOT be processed while
 * this function call is outstanding).
 *
 * @note1hang
 * This function MUST NOT Block and wait for UDS instance events that can
 * only be satisfied by receiving other Bluetooth Protocol Stack events.
 * A Deadlock WILL occur because other event callbacks will NOT be issued
 * while this function is currently outstanding.
 *
 * @param[in]  BluetoothStackID      Unique identifier assigned to this
 *                                   Bluetooth Protocol Stack on which the
 *                                   event occurred.
 *
 * @param[in]  UDS_Event_Data        Pointer to a structure that contains
 *                                   information about the event that has
 *                                   occurred.
 *
 * @param[in]  CallbackParameter     User defined value that was supplied
 *                                   as an input parameter when the UDS
 *                                   instance event callback was
 *                                   installed.
 *
 * @return None.
 */
typedef void (QAPI_BLE_BTPSAPI *qapi_BLE_UDS_Event_Callback_t)(uint32_t BluetoothStackID, qapi_BLE_UDS_Event_Data_t *UDS_Event_Data, uint32_t CallbackParameter);

   /* UDS Server API.                                                   */

/**
 * @brief
 * This function initializes a UDS instance (UDS Server) on a specified
 * Bluetooth Protocol Stack.
 *
 * @details
 * Only 1 UDS instance may be initialized at a time, per Bluetooth
 * Protocol Stack ID.
 *
 * See the qapi_BLE_UDS_Initialize_Data_t structure for more
 * information about the InitializeData parameter. If this
 * parameter is not configured correctly a UDS error will be returned.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  Service_Flags       Flags that are used to
 *                                 register UDS for the specified
 *                                 transport. These flags can be found
 *                                 in qapi_ble_gatt.h and have the form
 *                                 QAPI_BLE_GATT_SERVICE_FLAGS_XXX.
 *
 * @param[in]  InitializeData      A pointer to the data that is used to
 *                                 configure the UDS Server.
 *
 * @param[in]  EventCallback       The UDS event callback that will
 *                                 receive UDS instance events.
 *
 * @param[in]  CallbackParameter   A user-defined value that will be
 *                                 received with the specified
 *                                 EventCallback parameter.
 *
 * @param[out] ServiceID           Unique GATT Service ID of the
 *                                 registered UDS instance returned from
 *                                 the qapi_BLE_GATT_Register_Service()
 *                                 API.
 *
 * @return      Positive, non-zero if successful. The return value will
 *              be the UDS instance ID of the UDS Server that was
 *              successfully initialized for the specified Bluetooth
 *              Protocol Stack ID. This is the value that should be used
 *              in all subsequent function calls that require the UDS
 *              Instance ID.
 *
 * @return      An error code if negative. UDS error codes can be
 *              found in qapi_ble_uds.h (QAPI_BLE_UDS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_UDS_Initialize_Service(uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_UDS_Initialize_Data_t *InitializeData, qapi_BLE_UDS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID);

/**
 * @brief
 * This function initializes a UDS instance (UDS Server) on a specified
 * Bluetooth Protocol Stack.
 *
 * Unlike qapi_BLE_UDS_Initialize_Service(), this function allows the
 * application to select a attribute handle range in GATT to store the
 * service.
 *
 * @details
 * Only 1 UDS instance may be initialized at a time, per Bluetooth
 * Protocol Stack ID.
 *
 * See the qapi_BLE_UDS_Initialize_Data_t structure for more
 * information about the InitializeData parameter. If this
 * parameter is not configured correctly a UDS error will be returned.
 *
 * If the application wants GATT to select the attribute handle range for
 * the service, then all fields of the ServiceHandleRange parameter MUST
 * be initialized to zero. The qapi_BLE_UDS_Query_Number_Attributes()
 * function may be used, after initializing a UDS instance, to determine
 * the attribute handle range for the UDS instance.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  Service_Flags       Flags that are used to
 *                                 register UDS for the specified
 *                                 transport. These flags can be found
 *                                 in qapi_ble_gatt.h and have the form
 *                                 QAPI_BLE_GATT_SERVICE_FLAGS_XXX.
 *
 * @param[in]  InitializeData      A pointer to the data that is used to
 *                                 configure the UDS Server.
 *
 * @param[in]  EventCallback       The UDS event callback that will
 *                                 receive UDS instance events.
 *
 * @param[in]  CallbackParameter   A user-defined value that will be
 *                                 received with the specified
 *                                 EventCallback parameter.
 *
 * @param[out] ServiceID           Unique GATT Service ID of the
 *                                 registered UDS instance returned from
 *                                 the qapi_BLE_GATT_Register_Service()
 *                                 API.
 *
 * @param[in,out]  ServiceHandleRange   A pointer that on input, holds
 *                                      the handle range to store the
 *                                      service in GATT, and on output,
 *                                      contains the handle range for
 *                                      where the service is stored in
 *                                      GATT.
 *
 * @return      Positive, non-zero if successful. The return value will
 *              be the UDS instance ID of the UDS Server that was
 *              successfully initialized for the specified Bluetooth
 *              Protocol Stack ID. This is the value that should be used
 *              in all subsequent function calls that require the UDS
 *              Instance ID.
 *
 * @return      An error code if negative. UDS error codes can be
 *              found in qapi_ble_uds.h (QAPI_BLE_UDS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_UDS_Initialize_Service_Handle_Range(uint32_t BluetoothStackID, uint32_t Service_Flags, qapi_BLE_UDS_Initialize_Data_t *InitializeData, qapi_BLE_UDS_Event_Callback_t EventCallback, uint32_t CallbackParameter, uint32_t *ServiceID, qapi_BLE_GATT_Attribute_Handle_Group_t *ServiceHandleRange);

/**
 * @brief
 * This function is responsible for cleaning up and freeing all resources
 * associated with a UDS Instance (UDS Server).
 *
 * @details
 * After this function is called, no other UDS
 * function can be called until after a successful call to either of the
 * qapi_BLE_UDS_Initialize_XXX() functions.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the UDS instance that will
 *                                 be cleaned up.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. UDS error codes can be
 *              found in qapi_ble_uds.h (QAPI_BLE_UDS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_UDS_Cleanup_Service(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * This function is responsible for querying the number of attributes
 * that are contained in a UDS instance that is registered with a call to
 * either of the qapi_BLE_UDS_Initialize_XXX() functions.
 *
 * @details
 * None.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the UDS instance.
 *
 * @return   Positive, non-zero, number of attributes for the
 *           registered UDS instance.
 *
 * @return   Zero on failure.
 */
QAPI_BLE_DECLARATION unsigned int QAPI_BLE_BTPSAPI qapi_BLE_UDS_Query_Number_Attributes(uint32_t BluetoothStackID, uint32_t InstanceID);

/**
 * @brief
 * This function is responsible for responding to a read request,
 * received from a UDS Client, for a UDS Characteristic.
 *
 * @details
 * The ErrorCode parameter MUST be a valid value from
 * qapi_ble_udstypes.h (UDS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * The Offset parameter MUST match the Offset parameter in the
 * QAPI_BLE_UDS_ET_Server_Read_Characteristic_Request_E event.
 *
 * The Type parameter MUST be valid and correspond to the data in the
 * UDS_Characteristic parameter.  Otherwise, this function will
 * NOT format the data correctly, that will be sent in the response to
 * the UDS Client.
 *
 * If the request is rejected, the Offset parameter will be ignored and
 * the UDS_Characteristic parameter may be excluded (NULL).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the UDS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       The GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates if the request was
 *                                 accepted/rejected.
 *
 * @param[in]  Offset              The starting offset for the
 *                                 qapi_BLE_UDS_String_Data_t structure
 *                                 if the UDS Characteristic has the
 *                                 string data type.
 *
 * @param[in]  Type                Identifies the UDS Characteristic.
 *
 *
 * @param[in]  UDS_Characteristic   A pointer to the UDS Characteristic
 *                                  data that will be sent if the
 *                                  request is accepted.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. UDS error codes can be
 *              found in qapi_ble_uds.h (QAPI_BLE_UDS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_UDS_Read_Characteristic_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, uint16_t Offset, qapi_BLE_UDS_Characteristic_Type_t Type, qapi_BLE_UDS_Characteristic_t *UDS_Characteristic);

/**
 * @brief
 * This function is responsible for responding to a write request,
 * received from a UDS Client, for a UDS Characteristic.
 *
 * @details
 * The ErrorCode parameter MUST be a valid value from
 * qapi_ble_udstypes.h (UDS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the UDS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       The GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates if the request was
 *                                 accepted/rejected.
 *
 * @param[in]  Type                Identifies the UDS Characteristic.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. UDS error codes can be
 *              found in qapi_ble_uds.h (QAPI_BLE_UDS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_UDS_Write_Characteristic_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_UDS_Characteristic_Type_t Type);

/**
 * @brief
 * This function is responsible for responding to a request, received
 * from a UDS Client, to prepare data for a UDS Characteristic, that will
 * be written at a later time.
 *
 * @details
 * This event is primarily provided to reject a GATT Prepare Write
 * request for optional security reasons such as the UDS Client has
 * insufficient authentication, authorization, or encryption.  Therefore,
 * we will not pass the prepared data up to the application until the the
 * GATT Execute Write request has been received by the UDS Server, and
 * the prepared writes are not cancelled. If the prepared data is written
 * the QAPI_BLE_UDS_ET_SERVER_WRITE_CHARACTERISTIC_REQUEST_E event will
 * be dispatched to the application.  Otherwise the prepared
   data will be cleared.
 *
 * The ErrorCode parameter MUST be a valid value from
 * qapi_ble_udstypes.h (UDS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the UDS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       The GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates if the request was
 *                                 accepted/rejected.
 *
 * @param[in]  Type                Identifies the UDS Characteristic.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. UDS error codes can be
 *              found in qapi_ble_uds.h (QAPI_BLE_UDS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_UDS_Prepare_Write_Characteristic_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_UDS_Characteristic_Type_t Type);

/**
 * @brief
 * This function is responsible for responding to a read request,
 * received from a UDS Client, for a UDS Characteristic's Client
 * Characteristic Configuration Descriptor (CCCD).
 *
 * @details
 * The ErrorCode parameter MUST be a valid value from
 * qapi_ble_udstypes.h (UDS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * If the request is rejected, the Configuration parameter may be
 * ignored.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the UDS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       The GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates if the request was
 *                                 accepted/rejected.
 *
 * @param[in]  Type                Identifies the Client Characteristic
 *                                 Configuration descriptor (CCCD).
 *
 * @param[in]  Configuration       The Client Characteristic
 *                                 Configuration descriptor (CCCD) that
 *                                 will be sent if the request is
 *                                 accepted.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. UDS error codes can be
 *              found in qapi_ble_uds.h (QAPI_BLE_UDS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_UDS_Read_CCCD_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_UDS_CCCD_Characteristic_Type_t Type, uint16_t Configuration);

/**
 * @brief
 * This function is responsible for responding to a write request,
 * received from a UDS Client, for a UDS Characteristic's Client
 * Characteristic Configuration Descriptor (CCCD).
 *
 * @details
 * The ErrorCode parameter MUST be a valid value from
 * qapi_ble_udstypes.h (UDS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the UDS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       The GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates if the request was
 *                                 accepted/rejected.
 *
 * @param[in]  Type                Identifies the Client Characteristic
 *                                 Configuration descriptor (CCCD).
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. UDS error codes can be
 *              found in qapi_ble_uds.h (QAPI_BLE_UDS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_UDS_Write_CCCD_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, qapi_BLE_UDS_CCCD_Characteristic_Type_t Type);

/**
 * @brief
 * This function is responsible for responding to a read request,
 * received from a UDS Client, for a UDS Database Change Increment
 * Characteristic.
 *
 * @details
 * The ErrorCode parameter MUST be a valid value from
 * qapi_ble_udstypes.h (UDS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * If the request is rejected, the DatabaseChangeIncrement parameter
 * will be ignored.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the UDS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       The GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates if the request was
 *                                 accepted/rejected.
 *
 * @param[in]  DatabaseChangeIncrement   The UDS Database Change
 *                                       Increment Characteristic that
 *                                       will be sent if the request is
 *                                       accepted.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. UDS error codes can be
 *              found in qapi_ble_uds.h (QAPI_BLE_UDS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_UDS_Database_Change_Increment_Read_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, uint32_t DatabaseChangeIncrement);

/**
 * @brief
 * This function is responsible for responding to a write request,
 * received from a UDS Client, for a UDS Database Change Increment
 * Characteristic.
 *
 * @details
 * The ErrorCode parameter MUST be a valid value from
 * qapi_ble_udstypes.h (UDS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the UDS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       The GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates if the request was
 *                                 accepted/rejected.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. UDS error codes can be
 *              found in qapi_ble_uds.h (QAPI_BLE_UDS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_UDS_Database_Change_Increment_Write_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode);

/**
 * @brief
 * This function is responsible for notifying a UDS Continuous
 * Measurement to a UDS Client.
 *
 * @details
 * This function sends a notification, which provides NO guarantee that
 * the UDS Client will receive the value.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the UDS
 * Databse Change Increment Client Characteristic Configuration
 * Descriptor (CCCD) has been previously configured for notifications. A
 * UDS Client MUST have written the UDS Databse Change Increment
 * Characteristic's CCCD to enable notifications.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the UDS instance that
 *                                 received the request.
 *
 * @param[in]  ConnectionID        The GATT connection ID for the
 *                                 connection with the UDS Client.
 *
 * @param[in]  DatabaseChangeIncrement   The UDS Database Increment
 *                                       Change Characteristic that will
 *                                       be sent in the notification.
 *
 * @return      Positive non-zero if successful (represents the
 *              length of the notification).
 *
 * @return      An error code if negative. UDS error codes can be
 *              found in qapi_ble_uds.h (QAPI_BLE_UDS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_UDS_Notify_Database_Change_Increment(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, uint32_t DatabaseChangeIncrement);

/**
 * @brief
 * This function is responsible for responding to a read request,
 * received from a UDS Client, for a UDS User Index
 * Characteristic.
 *
 * @details
 * The ErrorCode parameter MUST be a valid value from
 * qapi_ble_udstypes.h (UDS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * If the request is rejected, the User_Index parameter
 * will be ignored.
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the UDS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       The GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates if the request was
 *                                 accepted/rejected.
 *
 * @param[in]  User_Index          The UDS User Index Characteristic that
 *                                 will be sent if the request is
 *                                 accepted.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. UDS error codes can be
 *              found in qapi_ble_uds.h (QAPI_BLE_UDS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_UDS_User_Index_Read_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode, uint8_t User_Index);

/**
 * @brief
 * This function is responsible for responding to a write request,
 * received from a UDS Client, for the UDS User Control Point
 * (UCP) Characteristic.
 *
 * This function does NOT indicate the result of the UCP Procedure. If
 * the request is accepted, then it indicates that the UCP Procedure has
 * started. An indication MUST be sent by the UDS Server once the UCP
 * Procedure has completed. The
 * qapi_BLE_UDS_Indicate_User_Control_Point_Response() function MUST be
 * used  to indicate the result of the UCP Procedure.
 *
 * @details
 * This function is primarily provided to allow a way to reject the UCP
 * write request when the UCP Client Characteristic Configuration
 * Descriptor (CCCD) has not been configured for indications, the UDS
 * Client does not have proper Authentication, Authorization, or
 * Encryption to write to the UCP, or an UCP request is already in
 * progress.  All other reasons should return
 * QAPI_BLE_UDS_ERROR_CODE_SUCCESS for the ErrorCode and then call the
 * qapi_BLE_UDS_Indicate_User_Control_Point_Response() to indicate the
 * response once the UCP procedure has completed.
 *
 * The ErrorCode parameter MUST be a valid value from
 * qapi_ble_udstypes.h (UDS_ERROR_CODE_XXX) or qapi_ble_atttypes.h
 * (QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX).
 *
 * @param[in]  BluetoothStackID    Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID          Identifies the UDS instance that
 *                                 received the request.
 *
 * @param[in]  TransactionID       The GATT transaction ID.
 *
 * @param[in]  ErrorCode           Indicates if the request was
 *                                 accepted/rejected.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. UDS error codes can be
 *              found in qapi_ble_uds.h (QAPI_BLE_UDS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_UDS_User_Control_Point_Write_Request_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t TransactionID, uint8_t ErrorCode);

/**
 * @brief
 * This function is responsible for indicating the UDS User Control Point
 * (UCP) Procedure result to a UDS Client.
 *
 * @details
 * This function sends an indication, which provides a guarantee that the
 * UDS Client will receive the value, since the UDS Client MUST confirm
 * that it has been received.
 *
 * @note1hang
 * It is the application's responsibilty to make sure that the UCP
 * Client Characteristic Configuration Descriptor (CCCD) has been
 * previously configured for indications. A UDS Client MUST have written
 * the UDS UCP Characteristic's CCCD to enable indications.
 *
 * @param[in]  BluetoothStackID   Unique identifier assigned to this
 *                                 Bluetooth Protocol Stack via a
 *                                 call to qapi_BLE_BSC_Initialize().
 *
 * @param[in]  InstanceID         Identifies the UDS instance that
 *                                 received the request.
 *
 * @param[in]  ConnectionID       The GATT connection ID for the
 *                                 connection with the UDS Client.
 *
 * @param[in]  ResponseData       A pointer to the UCP response data
 *                                 that will be sent in the indication.
 *
 * @return      Positive non-zero if successful (represents the
 *              GATT Transaction ID for the indication).
 *
 * @return      An error code if negative. UDS error codes can be
 *              found in qapi_ble_uds.h (QAPI_BLE_UDS_ERROR_XXX). Other
 *              error codes from qapi_ble_gatt.h
 *              (QAPI_BLE_GATT_ERROR_XXX) or qapi_ble_errors.h
 *              (QAPI_BLE_BTPS_ERROR_XXX) may also be returned if an
 *              internal error has occured.
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_UDS_Indicate_User_Control_Point_Response(uint32_t BluetoothStackID, uint32_t InstanceID, uint32_t ConnectionID, qapi_BLE_UDS_User_Control_Point_Response_Data_t *ResponseData);

   /* UDS Client API.                                                   */

/**
 * @brief
 * The following function is responsible for formatting a UDS
 * Characteristic into a buffer, for a GATT Write request, that will be
 * sent to the UDS Server.
 *
 * This function may also be used to determine the size of the buffer to
 * hold the formatted data (see details).
 *
 * @details
 * This function should NOT be used for the following UDS Characteristics
 * (since they have a qapi_BLE_UDS_String_Data_t structure data type and
 * are already formatted):
 *
 * @li QAPI_BLE_UDS_CT_FIRST_NAME_E
 * @li QAPI_BLE_UDS_CT_LAST_NAME_E
 * @li QAPI_BLE_UDS_CT_EMAIL_ADDRESS_E
 * @li QAPI_BLE_UDS_CT_LANGUAGE_E
 *
 * It is worth noting that the above UDS Characteristic types may use the
 * GATT Prepare Write Request and GATT Execute Write Request, in addition
 * to the GATT Write Request, if the UDS Characteristic CANNOT fit in a
 * GATT Write Request due to the constraints of the GATT Maximum
 * Transmission Unit (MTU).
 *
 * If the BufferLength parameter is 0, the Buffer parameter may be
 * excluded (NULL), and this function will return a positive non-zero
 * value, which represents the size of the buffer needed to hold the
 * formatted data.  The UDS Client may use this size to allocate a buffer
 * necessary to hold the formatted data.
 *
 * @param[in]  Type           Identifies the UDS Characteristic.
 *
 * @param[in]  UDS_Characteristic   A pointer to the UDS Chararacteristic
 *                                  data.
 *
 * @param[in]  BufferLength   The length of the user specified buffer.
 *
 * @param[out]  Buffer        The user specified buffer.
 *
 * @return      If this function is used to format the user specified
 *              buffer, then zero will be returned for success.
 *
 * @return      If this function is used to determine the size of the
 *              user specified buffer to hold the formatted data, then a
 *              positive non-zero value will be returned for success,
 *              which represents the minimum length required for the user
 *              specified buffer to hold the formatted data.
 *
 * @return      An error code if negative. UDS error codes can be
 *              found in qapi_ble_uds.h (QAPI_BLE_UDS_ERROR_XXX).
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_UDS_Format_UDS_Characteristic_Request(qapi_BLE_UDS_Characteristic_Type_t Type, qapi_BLE_UDS_Characteristic_t *UDS_Characteristic, uint16_t BufferLength, uint8_t *Buffer);

/**
 * @brief
 * The following function is responsible for parsing a value received in
 * a GATT Read response from a remote UDS Server interpreting it as a
 * UDS Characteristic.
 *
 * @details
 * This function should NOT be used for the following UDS Characteristics
 * (since they have a qapi_BLE_UDS_String_Data_t structure data type and
 * are already decoded):
 *
 * @li QAPI_BLE_UDS_CT_FIRST_NAME_E
 * @li QAPI_BLE_UDS_CT_LAST_NAME_E
 * @li QAPI_BLE_UDS_CT_EMAIL_ADDRESS_E
 * @li QAPI_BLE_UDS_CT_LANGUAGE_E
 *
 * It is worth noting that the above UDS Characteristic types may be
 * received in a GATT Read Long Value response, in addition to a GATT
 * Read Value response, if the UDS Characteristic could not completely
 * fit in the GATT Read Value response due to the constraints of the GATT
 * Maximum Transmission Unit (MTU). In this case the UDS Client MUST
 * issue GATT Read Long Value requests with the attribute offset set to
 * starting offset of the UDS Characteristic (string) that has not been
 * received.
 *
 * @param[in]  ValueLength   The length of the Value received from the
 *                           UDS Server.
 *
 * @param[in]  Value         The Value received from the
 *                           UDS Server.
 *
 * @param[in]  Type          Identifies the UDS Characteristic.
 *
 * @param[out]  UDS_Characteristic   A pointer that will hold the
 *                                   UDS Characteristic data if
 *                                   this function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. UDS error codes can be
 *              found in qapi_ble_uds.h (QAPI_BLE_UDS_ERROR_XXX).
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_UDS_Decode_UDS_Characteristic_Response(uint32_t ValueLength, uint8_t *Value, qapi_BLE_UDS_Characteristic_Type_t Type, qapi_BLE_UDS_Characteristic_t *UDS_Characteristic);

/**
 * @brief
 * The following function is responsible for formatting a UDS User
 * Control Point (UCP) Characteristic into a buffer, for a GATT Write
 * request, that will be sent to the UDS Server.
 *
 * This function may also be used to determine the size of the buffer to
 * hold the formatted data (see details).
 *
 * @details
 * If the BufferLength parameter is 0, the Buffer parameter may be
 * excluded (NULL), and this function will return a positive non-zero
 * value, which represents the size of the buffer needed to hold the
 * formatted data.  The UDS Client may use this size to allocate a buffer
 * necessary to hold the formatted data.
 *
 * @param[in]  RequestData    A pointer to the UDS User Control Point
 *                            data.
 *
 * @param[in]  BufferLength   The length of the user specified buffer.
 *
 * @param[out]  Buffer        The user specified buffer.
 *
 * @return      If this function is used to format the user specified
 *              buffer, then zero will be returned for success.
 *
 * @return      If this function is used to determine the size of the
 *              user specified buffer to hold the formatted data, then a
 *              positive non-zero value will be returned for success,
 *              which represents the minimum length required for the user
 *              specified buffer to hold the formatted data.
 *
 * @return      An error code if negative. UDS error codes can be
 *              found in qapi_ble_uds.h (QAPI_BLE_UDS_ERROR_XXX).
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_UDS_Format_User_Control_Point_Request(qapi_BLE_UDS_User_Control_Point_Request_Data_t *RequestData, uint16_t BufferLength, uint8_t *Buffer);

/**
 * @brief
 * The following function is responsible for parsing a value received in
 * a GATT Indication from a remote UDS Server interpreting it as the UDS
 * User Control Point (UCP) procedure result.
 *
 * @details
 * None.
 *
 * @param[in]  ValueLength   The length of the Value received from the
 *                           UDS Server.
 *
 * @param[in]  Value         The Value received from the
 *                           UDS Server.
 *
 * @param[out]  ResponseData   A pointer that will hold the
 *                             UDS User Control Point (UCP) response
 *                             data if this function is successful.
 *
 * @return      Zero if successful.
 *
 * @return      An error code if negative. UDS error codes can be
 *              found in qapi_ble_uds.h (QAPI_BLE_UDS_ERROR_XXX).
 */
QAPI_BLE_DECLARATION int QAPI_BLE_BTPSAPI qapi_BLE_UDS_Decode_User_Control_Point_Response(uint32_t ValueLength, uint8_t *Value, qapi_BLE_UDS_User_Control_Point_Response_Data_t *ResponseData);

#endif

/**
 *  @}
 */
