/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_cl.h
   @brief QAPI for common elements of the ZigBee Cluster Library (ZCL).

   @addtogroup qapi_zb_cl
   @{

   This API provides definitions, commands, and events related to the ZigBee
   Cluster Library, including the ZCL-specific status codes.

   The ZCL callback should be registered before sending ZCL commands in order to
   correctly receive the response events.

   @}
*/

#ifndef __QAPI_ZB_CL_H__ // [
#define __QAPI_ZB_CL_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi/qapi_types.h"
#include "qapi/qapi_status.h"

#include "qapi_zb.h"
#include "qapi_zb_aps.h"

/** @addtogroup qapi_zb_cl
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following defintions are the ZigBee qapi_Status_t codes for the ZCL
   layer of the ZigBee stack. Consult qapi_zb.h for other status codes. */
#define QAPI_ZB_ERR_ZCL_FAILURE                                         (QAPI_ZB_ERR(1))   /**< ZCL status indicating the operation was not successful.  Note that this
                                                                                                status overlaps with QZPI_ZB_ERR_MAC_ASSOCIATE_PAN_AT_CAPACITY*/
#define QAPI_ZB_ERR_ZCL_NOT_AUTHORIZED                                  (QAPI_ZB_ERR(126)) /**< ZCL status indicating the sender is not authorized to execute the command. */
#define QAPI_ZB_ERR_ZCL_RESERVED_FIELD_NOT_ZERO                         (QAPI_ZB_ERR(127)) /**< ZCL status indicating a reserved field contains a nonzero value. */
#define QAPI_ZB_ERR_ZCL_MALFORMED_COMMAND                               (QAPI_ZB_ERR(128)) /**< ZCL status indicating the command appears to be malformed. */
#define QAPI_ZB_ERR_ZCL_UNSUP_CLUSTER_COMMAND                           (QAPI_ZB_ERR(129)) /**< ZCL status indicating a cluster command is not supported on the device. */
#define QAPI_ZB_ERR_ZCL_UNSUP_GENERAL_COMMAND                           (QAPI_ZB_ERR(130)) /**< ZCL status indicating a general command is not supported on the device. */
#define QAPI_ZB_ERR_ZCL_UNSUP_MANUF_CLUSTER_COMMAND                     (QAPI_ZB_ERR(131)) /**< ZCL status indicating the manufacturer specific cluster command is not
                                                                                                supported on the device. */
#define QAPI_ZB_ERR_ZCL_UNSUP_MANUF_GENERAL_COMMAND                     (QAPI_ZB_ERR(132)) /**< ZCL status indicating a manufacturer specific general command is not
                                                                                                supported on the device. */
#define QAPI_ZB_ERR_ZCL_INVALID_FIELD                                   (QAPI_ZB_ERR(133)) /**< ZCL status indicating a field in the command is invalid. */
#define QAPI_ZB_ERR_ZCL_UNSUPPORTED_ATTRIBUTE                           (QAPI_ZB_ERR(134)) /**< ZCL status indicating the specified attribute is not supported by the device. */
#define QAPI_ZB_ERR_ZCL_INVALID_VALUE                                   (QAPI_ZB_ERR(135)) /**< ZCL status indicating the value is invalid. */
#define QAPI_ZB_ERR_ZCL_READ_ONLY                                       (QAPI_ZB_ERR(136)) /**< ZCL status indicating the write request was to a read only attribute. */
#define QAPI_ZB_ERR_ZCL_INSUFFICIENT_SPACE                              (QAPI_ZB_ERR(137)) /**< ZCL status indicating the operation failed due to insufficient space. */
#define QAPI_ZB_ERR_ZCL_DUPLICATE_EXISTS                                (QAPI_ZB_ERR(138)) /**< ZCL status indicating a duplicate entry already exists in the table. */
#define QAPI_ZB_ERR_ZCL_NOT_FOUND                                       (QAPI_ZB_ERR(139)) /**< ZCL status indicating the requested information was not found. */
#define QAPI_ZB_ERR_ZCL_UNREPORTABLE_ATTRIBUTE                          (QAPI_ZB_ERR(140)) /**< ZCL status indicating periodic reports are not supported for the attribute. */
#define QAPI_ZB_ERR_ZCL_INVALID_DATA_TYPE                               (QAPI_ZB_ERR(141)) /**< ZCL status indicating the data type for the attribute is incorrect. */
#define QAPI_ZB_ERR_ZCL_INVALID_SELECTOR                                (QAPI_ZB_ERR(142)) /**< ZCL status indicating the selector for the attribute is incorrect. */
#define QAPI_ZB_ERR_ZCL_WRITE_ONLY                                      (QAPI_ZB_ERR(143)) /**< ZCL status indicating the read request was to a write only attribute. */
#define QAPI_ZB_ERR_ZCL_INCONSISTENT_STARTUP_STATE                      (QAPI_ZB_ERR(144)) /**< ZCL status indicating that setting the requested value would put the device
                                                                                                in an inconsistent state on startup. */
#define QAPI_ZB_ERR_ZCL_DEFINED_OUT_OF_BAND                             (QAPI_ZB_ERR(145)) /**< ZCL status indicating a write failed because the attribute is defined out
                                                                                                of band. */
#define QAPI_ZB_ERR_ZCL_INCONSISTENT                                    (QAPI_ZB_ERR(146)) /**< ZCL status indicating the values are inconsistent. */
#define QAPI_ZB_ERR_ZCL_ACTION_DENIED                                   (QAPI_ZB_ERR(147)) /**< ZCL status indicating the action was denied. */
#define QAPI_ZB_ERR_ZCL_TIMEOUT                                         (QAPI_ZB_ERR(148)) /**< ZCL status indicating the command timed out. */
#define QAPI_ZB_ERR_ZCL_ABORT                                           (QAPI_ZB_ERR(149)) /**< ZCL status indicating the OTA process was aborted. */
#define QAPI_ZB_ERR_ZCL_INVALID_IMAGE                                   (QAPI_ZB_ERR(150)) /**< ZCL status indicating the OTA image is invalid. */
#define QAPI_ZB_ERR_ZCL_WAIT_FOR_DATA                                   (QAPI_ZB_ERR(151)) /**< ZCL status indicating the OTA server does not have the data block yet. */
#define QAPI_ZB_ERR_ZCL_NO_IMAGE_AVAILABLE                              (QAPI_ZB_ERR(152)) /**< ZCL status indicating no OTA update image is available for the client. */
#define QAPI_ZB_ERR_ZCL_REQUIRE_MORE_IMAGE                              (QAPI_ZB_ERR(153)) /**< ZCL status indicating the client still requires more of the image for the
                                                                                                upgrade. */
#define QAPI_ZB_ERR_ZCL_NOTIFICATION_PENDING                            (QAPI_ZB_ERR(154)) /**< ZCL status indicating the command is being processed. */
#define QAPI_ZB_ERR_ZCL_HARDWARE_FAILURE                                (QAPI_ZB_ERR(192)) /**< ZCL status indicating a hardware failure occurred. */
#define QAPI_ZB_ERR_ZCL_SOFTWARE_FAILURE                                (QAPI_ZB_ERR(193)) /**< ZCL status indicating a software failure occurred.  Note that this status
                                                                                                overlaps with QZPI_ZB_ERR_NWK_STATUS_INVALID_PARAMETER. */
#define QAPI_ZB_ERR_ZCL_CALIBRATION_ERROR                               (QAPI_ZB_ERR(194)) /**< ZCL status indicating an error occurred during calibration.  Note that this
                                                                                                status overlaps with QZPI_ZB_ERR_NWK_STATUS_INVALID_REQUEST. */
#define QAPI_ZB_ERR_ZCL_UNSUPPORTED_CLUSTER                             (QAPI_ZB_ERR(195)) /**< ZCL status indicating the cluster is not supported.  Note that this status
                                                                                                overlaps with QZPI_ZB_ERR_NWK_STATUS_NOT_PERMITTED. */
#define QAPI_ZB_ERR_ZCL_NO_DEFAULT_RESPONSE                             (QAPI_ZB_ERR(255)) /**< ZCL status indicating a default response should not be sent.  This status
                                                                                                code is only applicable for event callbacks that take a status result. It
                                                                                                instructs the stack to not send a response for the received command on the
                                                                                                assumption that the application will send the response instead. */

/* The following definitions represent the cluster identifiers. */
#define QAPI_ZB_CL_CLUSTER_ID_BASIC                                     (0x0000) /**< ZCL cluster ID for the basic cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_POWER_CONFIG                              (0x0001) /**< ZCL cluster ID for the power configuration cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_TEMPERATURE_CONFIG                        (0x0002) /**< ZCL cluster ID for the temperature configuration cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_IDENTIFY                                  (0x0003) /**< ZCL cluster ID for the identify cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_GROUPS                                    (0x0004) /**< ZCL cluster ID for the groups cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_SCENES                                    (0x0005) /**< ZCL cluster ID for the scenes cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_ONOFF                                     (0x0006) /**< ZCL cluster ID for the on/off cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_ONOFF_CONFIG                              (0x0007) /**< ZCL cluster ID for the on/off configuration cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_LEVEL_CONTROL                             (0x0008) /**< ZCL cluster ID for the level control cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_ALARMS                                    (0x0009) /**< ZCL cluster ID for the alarms cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_TIME                                      (0x000a) /**< ZCL cluster ID for the time cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_RSSI_LOCATION                             (0x000b) /**< ZCL cluster ID for the RSSI location cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_ANALOG_INPUT_BASIC                        (0x000c) /**< ZCL cluster ID for the analog input basic cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_ANALOG_OUTPUT_BASIC                       (0x000d) /**< ZCL cluster ID for the analog output basic cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_ANALOG_VALUE_BASIC                        (0x000e) /**< ZCL cluster ID for the analog value basic cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_BINARY_INPUT_BASIC                        (0x000f) /**< ZCL cluster ID for the binary input basic cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_BINARY_OUTPUT_BASIC                       (0x0010) /**< ZCL cluster ID for the binary output basic cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_BINARY_VALUE_BASIC                        (0x0011) /**< ZCL cluster ID for the binary value basic cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_MULTISTATE_INPUT_BASIC                    (0x0012) /**< ZCL cluster ID for the multistate input basic cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_MULTISTATE_OUTPUT_BASIC                   (0x0013) /**< ZCL cluster ID for the multistate output basic cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_MULTISTATE_VALUE_BASIC                    (0x0014) /**< ZCL cluster ID for the multistate value basic cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_COMMISSIONING                             (0x0015) /**< ZCL cluster ID for the commissioning cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_OTA_UPGRADE                               (0x0019) /**< ZCL cluster ID for the OTA upgrade cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_KEEP_ALIVE                                (0x0025) /**< ZCL cluster ID for the keep alive cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_DOORLOCK                                  (0x0101) /**< ZCL cluster ID for the door lock cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_WINDOW_COVERING                           (0x0102) /**< ZCL cluster ID for the window covering cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_THERMOSTAT                                (0x0201) /**< ZCL cluster ID for the thermostat cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_FAN_CONTROL                               (0x0202) /**< ZCL cluster ID for the fan control cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_COLOR_CONTROL                             (0x0300) /**< ZCL cluster ID for the color control cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_BALLAST                                   (0x0301) /**< ZCL cluster ID for the ballast cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_ILLUMINANCE                               (0x0400) /**< ZCL cluster ID for the illuminance cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_TEMP_MEASURE                              (0x0402) /**< ZCL cluster ID for the temp measurment cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_RELATIVE_HUMID                            (0x0405) /**< ZCL cluster ID for the realtive humidity cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_OCCUPANCY_SENSING                         (0x0406) /**< ZCL cluster ID for the occupancy sensing cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_IAS_ZONE                                  (0x0500) /**< ZCL cluster ID for the IAS zone cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_IAS_ACE                                   (0x0501) /**< ZCL cluster ID for the IAS ACE cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_IAS_WD                                    (0x0502) /**< ZCL cluster ID for the IAS WD cluster. */
#define QAPI_ZB_CL_CLUSTER_ID_TOUCHLINK_COMMISSIONING                   (0x1000) /**< ZCL cluster ID for the Touchlink commissioning cluster. */

/* The following definitions represent the global mandatory attributes. */
#define QAPI_ZB_CL_ATTRIBUTE_ID_CLUSTER_REVISION                        (0xFFFD) /**< ZCL cluster revision. */

/* The following definitions represent the supported profile IDs. */
#define QAPI_ZB_CL_PROFILE_ID_ZIGBEE_DEVICE_OBJECT                      (0x0000) /**< ZCL profile ID for the ZigBee device object (endpoint 0). */
#define QAPI_ZB_CL_PROFILE_ID_HOME_AUTOMATION                           (0x0104) /**< ZCL profile ID for the home automation profile. */
#define QAPI_ZB_CL_PROFILE_ID_SMART_ENERGY                              (0x0109) /**< ZCL profile ID for the smart energy profile. */
#define QAPI_ZB_CL_PROFILE_ID_RETAIL_SERVICES                           (0x010A) /**< ZCL profile ID for the retail services profile. */
#define QAPI_ZB_CL_PROFILE_ID_GATEWAY                                   (0x7F02) /**< ZCL profile ID for the gateway profile. */
#define QAPI_ZB_CL_PROFILE_ID_GREEN_POWER                               (0xA1E0) /**< ZCL profile ID for the green power profile. */
#define QAPI_ZB_CL_PROFILE_ID_ZIGBEE_LIGHT_LINK                         (0xC05E) /**< ZCL profile ID for the light link profile. */
#define QAPI_ZB_CL_PROFILE_ID_WILDCARD                                  (0xFFFF) /**< ZCL profile ID wildcard. */

/* The following definitions provide the invalid values for each of the ZCL data
   types. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_BOOLEAN                           (0xFF)                  /**< Value for the Boolean data type that indicates invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_UNSIGNED_8BIT_INTEGER             (0xFF)                  /**< Value for the uint8 data type that indicates invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_UNSIGNED_16BIT_INTEGER            (0xFFFF)                /**< Value for the uint16 data type that indicates invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_UNSIGNED_24BIT_INTEGER            (0xFFFFFF)              /**< Value for the uint24 data type that indicates invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_UNSIGNED_32BIT_INTEGER            (0xFFFFFFFF)            /**< Value for the uint32 data type that indicates invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_UNSIGNED_40BIT_INTEGER            (0xFFFFFFFFFFULL)       /**< Value for the uint40 data type that indicates invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_UNSIGNED_48BIT_INTEGER            (0xFFFFFFFFFFFFULL)     /**< Value for the uint48 data type that indicates invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_UNSIGNED_56BIT_INTEGER            (0xFFFFFFFFFFFFFFULL)   /**< Value for the uint56 data type that indicates invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_UNSIGNED_64BIT_INTEGER            (0xFFFFFFFFFFFFFFFFULL) /**< Value for the uint64 data type that indicates invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_SIGNED_8BIT_INTEGER               (0x80)                  /**< Value for the int8 data type that indicates invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_SIGNED_16BIT_INTEGER              (0x8000)                /**< Value for the int16 data type that indicates invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_SIGNED_24BIT_INTEGER              (0x800000)              /**< Value for the int24 data type that indicates invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_SIGNED_32BIT_INTEGER              (0x80000000)            /**< Value for the int32 data type that indicates invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_SIGNED_40BIT_INTEGER              (0x8000000000ULL)       /**< Value for the int40 data type that indicates invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_SIGNED_48BIT_INTEGER              (0x800000000000ULL)     /**< Value for the int48 data type that indicates invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_SIGNED_56BIT_INTEGER              (0x80000000000000ULL)   /**< Value for the int56 data type that indicates invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_SIGNED_64BIT_INTEGER              (0x8000000000000000ULL) /**< Value for the int64 data type that indicates invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_8BIT_ENUMERATION                  (0xFF)                  /**< Value for the enum8 data type that indicates invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_16BIT_ENUMERATION                 (0xFFFF)                /**< Value for the enum16 data type that indicates invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_SEMI_PRECISION_FLOATING_POINT     (0xFFFF)                /**< Value for the semi data type that indicates invalid data.  The
                                                                                                     QAPI_ZB_CL_DATA_CHECK_INVALID_SEMI_PRECISION_FLOATING_POINT() macro
                                                                                                     should be used to check if a value represents invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_SINGLE_PRECISION_FLOATING_POINT   (0xFFFFFFFF)            /**< Value for the single data type that indicates invalid data. The
                                                                                                     QAPI_ZB_CL_DATA_CHECK_INVALID_SINGLE_PRECISION_FLOATING_POINT() macro
                                                                                                     should be used to check if a value represents invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_DOUBLE_PRECISION_FLOATING_POINT   (0xFFFFFFFFFFFFFFFFULL) /**< Value for the double data type that indicates invalid data. The
                                                                                                     QAPI_ZB_CL_DATA_CHECK_INVALID_DOUBLE_PRECISION_FLOATING_POINT() macro
                                                                                                     should be used to check if a value represents invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_OCTET_STRING                      (0xFF)                  /**< Value for the first byte of an octstr data type that indicates invalid
                                                                                                     data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_CHARACTER_STRING                  (0xFF)                  /**< Value for the first byte of a string data type that indicates invalid
                                                                                                     data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_LONG_OCTET_STRING                 (0xFFFF)                /**< Value for the first two bytes of an octstr16 data type that indicates
                                                                                                     invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_LONG_CHARACTER_STRING             (0xFFFF)                /**< Value for the first two bytes of a string16 data type that indicates
                                                                                                     invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_ARRAY                             (0xFFFF)                /**< Value for the first two bytes of an array data type that indicates invalid
                                                                                                     data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_STRUCTURE                         (0xFFFF)                /**< Value for the first two bytes of a struct data type that indicates invalid
                                                                                                     data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_SET                               (0xFFFF)                /**< Value for the first two bytes of a set data type that indicates invalid
                                                                                                     data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_BAG                               (0xFFFF)                /**< Value for the first two bytes of a bag data type that indicates invalid
                                                                                                     data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_TIME_OF_DAY                       (0xFFFFFFFF)            /**< Value for the ToD data type that indicates invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_DATE                              (0xFFFFFFFF)            /**< Value for the date data type that indicates invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_UTC_TIME                          (0xFFFFFFFF)            /**< Value for the UTC data type that indicates invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_CLUSTER_ID                        (0xFFFF)                /**< Value for the clusterId data type that indicates invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_ATTRIBUTE_ID                      (0xFFFF)                /**< Value for the AttribId data type that indicates invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_BACNET_OID                        (0xFFFFFFFF)            /**< Value for the bacOID data type that indicates invalid data. */
#define QAPI_ZB_CL_DATA_INVALID_VALUE_IEEE_ADDRESS                      (0xFFFFFFFFFFFFFFFFULL) /**< Value for the EUI64 data type that indicates invalid data. */

/**
   @brief Macro to check a semi-precision floating point value to determine
          if it represents a valid value.

   @param[in] _value_ Semi-precision floating point value to be verified.

   @return
    - TRUE if the value is valid.
    - FALSE if the value is invalid.
*/
#define QAPI_ZB_CL_DATA_CHECK_INVALID_SEMI_PRECISION_FLOATING_POINT(_value_)     ((((_value_) & 0x7C00) == 0x7C00) && (((_value_) & 0x03FF) != 0))

/**
   @brief Macro to check a single precision floating point value to determine
          if it represents a valid value.

   @param[in] _value_ Single precision floating point value to be verified.

   @return
    - TRUE if the value is valid.
    - FALSE if the value is invalid.
*/
#define QAPI_ZB_CL_DATA_CHECK_INVALID_SINGLE_PRECISION_FLOATING_POINT(_value_)   ((((_value_) & 0x7F800000) == 0x7F80000) && (((_value_) & 0x007FFFFF) != 0))

/**
   @brief Macro to check a double precision floating point value to determine
          if it represents a valid value.

   @param[in] _value_ Double precision floating point value to be verified.

   @return
    - TRUE if the value is valid.
    - FALSE if the value is invalid.
*/
#define QAPI_ZB_CL_DATA_CHECK_INVALID_DOUBLE_PRECISION_FLOATING_POINT(_value_)   ((((_value_) & 0x7FF0000000000000ULL) == 0x7FF0000000000000ULL) && (((_value_) & 0x000FFFFFFFFFFFFFULL) != 0))

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   Handle of a ZigBee cluster.
*/
typedef struct qapi_ZB_Cluster_s *qapi_ZB_Cluster_t;

/**
   Enumeration of the ZCL data types.
*/
typedef enum
{
   QAPI_ZB_CL_DATA_TYPE_NO_DATA_E                         = 0x00, /**< NULL: No data (nodata). */
   QAPI_ZB_CL_DATA_TYPE_8BIT_DATA_E                       = 0x08, /**< Discrete, general data: 8-bit data (data8). */
   QAPI_ZB_CL_DATA_TYPE_16BIT_DATA_E                      = 0x09, /**< Discrete, general data: 16-bit data (data16). */
   QAPI_ZB_CL_DATA_TYPE_24BIT_DATA_E                      = 0x0A, /**< Discrete, general data: 24-bit data (data24). */
   QAPI_ZB_CL_DATA_TYPE_32BIT_DATA_E                      = 0x0B, /**< Discrete, general data: 32-bit data (data32). */
   QAPI_ZB_CL_DATA_TYPE_40BIT_DATA_E                      = 0x0C, /**< Discrete, general data: 40-bit data (data40). */
   QAPI_ZB_CL_DATA_TYPE_48BIT_DATA_E                      = 0x0D, /**< Discrete, general data: 48-bit data (data48). */
   QAPI_ZB_CL_DATA_TYPE_56BIT_DATA_E                      = 0x0E, /**< Discrete, general data: 56-bit data (data56). */
   QAPI_ZB_CL_DATA_TYPE_64BIT_DATA_E                      = 0x0F, /**< Discrete, general data: 64-bit data (data64). */
   QAPI_ZB_CL_DATA_TYPE_BOOLEAN_E                         = 0x10, /**< Discrete, logical data: Boolean (bool). */
   QAPI_ZB_CL_DATA_TYPE_8BIT_BITMAP_E                     = 0x18, /**< Discrete, bitmap: 8-bit bitmap (map8). */
   QAPI_ZB_CL_DATA_TYPE_16BIT_BITMAP_E                    = 0x19, /**< Discrete, bitmap: 16-bit bitmap (map16). */
   QAPI_ZB_CL_DATA_TYPE_24BIT_BITMAP_E                    = 0x1A, /**< Discrete, bitmap: 24-bit bitmap (map24). */
   QAPI_ZB_CL_DATA_TYPE_32BIT_BITMAP_E                    = 0x1B, /**< Discrete, bitmap: 32-bit bitmap (map32). */
   QAPI_ZB_CL_DATA_TYPE_40BIT_BITMAP_E                    = 0x1C, /**< Discrete, bitmap: 40-bit bitmap (map40). */
   QAPI_ZB_CL_DATA_TYPE_48BIT_BITMAP_E                    = 0x1D, /**< Discrete, bitmap: 48-bit bitmap (map48). */
   QAPI_ZB_CL_DATA_TYPE_56BIT_BITMAP_E                    = 0x1E, /**< Discrete, bitmap: 56-bit bitmap (map56). */
   QAPI_ZB_CL_DATA_TYPE_64BIT_BITMAP_E                    = 0x1F, /**< Discrete, bitmap: 64-bit bitmap (map64). */
   QAPI_ZB_CL_DATA_TYPE_UNSIGNED_8BIT_INTEGER_E           = 0x20, /**< Analog, unsigned integer: Unsigned 8-bit integer (uint8). */
   QAPI_ZB_CL_DATA_TYPE_UNSIGNED_16BIT_INTEGER_E          = 0x21, /**< Analog, unsigned integer: Unsigned 16-bit integer (uint16). */
   QAPI_ZB_CL_DATA_TYPE_UNSIGNED_24BIT_INTEGER_E          = 0x22, /**< Analog, unsigned integer: Unsigned 24-bit integer (uint24). */
   QAPI_ZB_CL_DATA_TYPE_UNSIGNED_32BIT_INTEGER_E          = 0x23, /**< Analog, unsigned integer: Unsigned 32-bit integer (uint32). */
   QAPI_ZB_CL_DATA_TYPE_UNSIGNED_40BIT_INTEGER_E          = 0x24, /**< Analog, unsigned integer: Unsigned 40-bit integer (uint40). */
   QAPI_ZB_CL_DATA_TYPE_UNSIGNED_48BIT_INTEGER_E          = 0x25, /**< Analog, unsigned integer: Unsigned 48-bit integer (uint48). */
   QAPI_ZB_CL_DATA_TYPE_UNSIGNED_56BIT_INTEGER_E          = 0x26, /**< Analog, unsigned integer: Unsigned 56-bit integer (uint56). */
   QAPI_ZB_CL_DATA_TYPE_UNSIGNED_64BIT_INTEGER_E          = 0x27, /**< Analog, unsigned integer: Unsigned 64-bit integer (uint64). */
   QAPI_ZB_CL_DATA_TYPE_SIGNED_8BIT_INTEGER_E             = 0x28, /**< Analog, signed integer: Signed 8-bit integer (int8). */
   QAPI_ZB_CL_DATA_TYPE_SIGNED_16BIT_INTEGER_E            = 0x29, /**< Analog, signed integer: Signed 16-bit integer (int16). */
   QAPI_ZB_CL_DATA_TYPE_SIGNED_24BIT_INTEGER_E            = 0x2A, /**< Analog, signed integer: Signed 24-bit integer (int24). */
   QAPI_ZB_CL_DATA_TYPE_SIGNED_32BIT_INTEGER_E            = 0x2B, /**< Analog, signed integer: Signed 32-bit integer (int32). */
   QAPI_ZB_CL_DATA_TYPE_SIGNED_40BIT_INTEGER_E            = 0x2C, /**< Analog, signed integer: Signed 40-bit integer (int40). */
   QAPI_ZB_CL_DATA_TYPE_SIGNED_48BIT_INTEGER_E            = 0x2D, /**< Analog, signed integer: Signed 48-bit integer (int48). */
   QAPI_ZB_CL_DATA_TYPE_SIGNED_56BIT_INTEGER_E            = 0x2E, /**< Analog, signed integer: Signed 56-bit integer (int56). */
   QAPI_ZB_CL_DATA_TYPE_SIGNED_64BIT_INTEGER_E            = 0x2F, /**< Analog, signed integer: Signed 64-bit integer (int64). */
   QAPI_ZB_CL_DATA_TYPE_8BIT_ENUMERATION_E                = 0x30, /**< Discrete, enumeration: 8-bit enumeration (enum8). */
   QAPI_ZB_CL_DATA_TYPE_16BIT_ENUMERATION_E               = 0x31, /**< Discrete, enumeration: 16-bit enumeration (enum16). */
   QAPI_ZB_CL_DATA_TYPE_SEMI_PRECISION_FLOATING_POINT_E   = 0x38, /**< Analog, floating point: Semi-precision (semi). */
   QAPI_ZB_CL_DATA_TYPE_SINGLE_PRECISION_FLOATING_POINT_E = 0x39, /**< Analog, floating point: Single precision (single). */
   QAPI_ZB_CL_DATA_TYPE_DOUBLE_PRECISION_FLOATING_POINT_E = 0x3A, /**< Analog, floating point: Double precision (double). */
   QAPI_ZB_CL_DATA_TYPE_OCTET_STRING_E                    = 0x41, /**< Discrete, string: Octet string (octstr). */
   QAPI_ZB_CL_DATA_TYPE_CHARACTER_STRING_E                = 0x42, /**< Discrete, string: Character string (string). */
   QAPI_ZB_CL_DATA_TYPE_LONG_OCTET_STRING_E               = 0x43, /**< Discrete, string: Long octet string (octstr16). */
   QAPI_ZB_CL_DATA_TYPE_LONG_CHARACTER_STRING_E           = 0x44, /**< Discrete, string: Long character string (string16). */
   QAPI_ZB_CL_DATA_TYPE_ARRAY_E                           = 0x48, /**< Discrete, ordered sequence: Array (array). */
   QAPI_ZB_CL_DATA_TYPE_STRUCTURE_E                       = 0x4C, /**< Discrete, ordered sequence: Structure (struct). */
   QAPI_ZB_CL_DATA_TYPE_SET_E                             = 0x50, /**< Discrete, collection: Set (set). */
   QAPI_ZB_CL_DATA_TYPE_BAG_E                             = 0x51, /**< Discrete, collection: Bag (bag). */
   QAPI_ZB_CL_DATA_TYPE_TIME_OF_DAY_E                     = 0xE0, /**< Analog, time: Time of day (ToD). */
   QAPI_ZB_CL_DATA_TYPE_DATE_E                            = 0xE1, /**< Analog, time: Date (date). */
   QAPI_ZB_CL_DATA_TYPE_UTC_TIME_E                        = 0xE2, /**< Analog, time: UTCTime (UTC). */
   QAPI_ZB_CL_DATA_TYPE_CLUSTER_ID_E                      = 0xE8, /**< Discrete, identifier: Cluster ID (clusterId). */
   QAPI_ZB_CL_DATA_TYPE_ATTRIBUTE_ID_E                    = 0xE9, /**< Discrete, identifier: Attribute ID (attribId). */
   QAPI_ZB_CL_DATA_TYPE_BACNET_OID_E                      = 0xEA, /**< Discrete, identifier: BACNnet OID (bacOID). */
   QAPI_ZB_CL_DATA_TYPE_IEEE_ADDRESS_E                    = 0xF0, /**< Discrete, miscellaneous: IEEE address (EUI64). */
   QAPI_ZB_CL_DATA_TYPE_128BIT_SECURITY_KEY_E             = 0xF1, /**< Discrete, miscellaneous: 128-bit security key (key128). */
   QAPI_ZB_CL_DATA_TYPE_UNKNOWN_E                         = 0xFF  /**< Discrete, unknown. */
} qapi_ZB_CL_Data_Type_t;

/**
   Enumeration of the defined ZigBee device IDs.
*/
typedef enum
{
   QAPI_ZB_CL_DEVICE_ID_ONOFF_LIGHT               = 0x0100, /**< On/off light. */
   QAPI_ZB_CL_DEVICE_ID_DIMMABLE_LIGHT            = 0x0101, /**< Dimmable light. */
   QAPI_ZB_CL_DEVICE_ID_COLOR_DIMMABLE_LIGHT      = 0x0102, /**< Color dimmable light. */
   QAPI_ZB_CL_DEVICE_ID_ONOFF_LIGHT_SWITCH        = 0x0103, /**< On/off light switch. */
   QAPI_ZB_CL_DEVICE_ID_DIMMER_SWITCH             = 0x0104, /**< Dimmer light switch. */
   QAPI_ZB_CL_DEVICE_ID_COLOR_DIMMER_SWITCH       = 0x0105, /**< Color dimmer switch. */
   QAPI_ZB_CL_DEVICE_ID_LIGHT_SENSOR              = 0x0106, /**< Light sensor. */
   QAPI_ZB_CL_DEVICE_ID_OCCUPANCY_SENSOR          = 0x0107, /**< Occupancy sensor. */
   QAPI_ZB_CL_DEVICE_ID_ONOFF_BALLAST             = 0x0108, /**< On/off ballast. */
   QAPI_ZB_CL_DEVICE_ID_DIMMABLE_BALLAST          = 0x0109, /**< Dimmable ballast. */
   QAPI_ZB_CL_DEVICE_ID_ONOFF_PLUGIN_UNIT         = 0x010A, /**< On/off plugin unit. */
   QAPI_ZB_CL_DEVICE_ID_DIMMABLE_PLUGIN_UNIT      = 0x010B, /**< Dimmable plugin unit. */
   QAPI_ZB_CL_DEVICE_ID_COLOR_TEMPERATURE_LIGHT   = 0x010C, /**< Color temperature light. */
   QAPI_ZB_CL_DEVICE_ID_EXTENDED_COLOR_LIGHT      = 0x010D, /**< Extended color light. */
   QAPI_ZB_CL_DEVICE_ID_LIGHT_LEVEL_SENSOR        = 0x010E, /**< Light level sensor. */
   QAPI_ZB_CL_DEVICE_ID_COLOR_CONTROLLER          = 0x0800, /**< Color controller. */
   QAPI_ZB_CL_DEVICE_ID_COLOR_SCENE_CONTROLLER    = 0x0810, /**< Color scene controller. */
   QAPI_ZB_CL_DEVICE_ID_NONCOLOR_CONTROLLER       = 0x0820, /**< Noncolor controller. */
   QAPI_ZB_CL_DEVICE_ID_NONCOLOR_SCENE_CONTROLLER = 0x0830, /**< Noncolor scene controller. */
   QAPI_ZB_CL_DEVICE_ID_CONTROL_BRIDGE            = 0x0840, /**< Control bridge. */
   QAPI_ZB_CL_DEVICE_ID_ONOFF_SENSOR              = 0x0850, /**< On/off sensor. */

   QAPI_ZB_CL_DEVICE_ID_TEST_DRIVER               = 0x0000, /**< Test driver device. */
   QAPI_ZB_CL_DEVICE_ID_DEVICE_UNDER_TEST         = 0xAAAA, /**< Device under test. */
   QAPI_ZB_CL_DEVICE_ID_FULL_DEVICE_UNDER_TEST    = 0xFFFF  /**< Full device under test. */
} qapi_ZB_CL_Device_ID_t;

/**
   Enumeration of ZCL command directions.
*/
typedef enum
{
   QAPI_ZB_CL_FRAME_DIRECTION_TO_SERVER_E = 0, /**< Indicates the command was sent from the client to the server. */
   QAPI_ZB_CL_FRAME_DIRECTION_TO_CLIENT_E = 1  /**< Indicates the command was sent from the server to the client. */
} qapi_ZB_CL_Frame_Direction_t;

/**
   Enumeration specifying how an attribute is written.
*/
typedef enum
{
   QAPI_ZB_CL_WRITE_MODE_NORMAL_E, /**< Normal write. */
   QAPI_ZB_CL_WRITE_MODE_TEST_E,   /**< Verify the write only. This is as a test when a write undivided command is
                                        received. If the test is performed successfully then a normal write to the
                                        attributes will be performed. */
   QAPI_ZB_CL_WRITE_MODE_FORCE_E   /**< Write even if the attribute is read only. Typically used for a write
                                         command to a local attribute. */
} qapi_ZB_CL_Write_Mode_t;

/**
   Enumeration of reporting configuration record directions.
*/
typedef enum
{
   QAPI_ZB_CL_ATTR_REPORT_DIRECTION_TO_REPORTER_E = 0, /**< Reporting configuration record is sent from a receiver to a reporter to
                                                            configure how reports are sent. */
   QAPI_ZB_CL_ATTR_REPORT_DIRECTION_TO_RECEIVER_E = 1  /**< Reporting configuration record is sent from a reporter to a receiver to
                                                            configure how it should expect to receive reports. */
} qapi_ZB_CL_Attr_Report_Direction_t;

/**
   Enumeration of event types provided to the ZCL callback.
*/
typedef enum
{
   QAPI_ZB_CL_EVENT_TYPE_UNPARSED_RESPONSE_E,              /**< Indicates that an unidentified command response was received. */
   QAPI_ZB_CL_EVENT_TYPE_READ_ATTR_RESPONSE_E,             /**< Indicates that a read attributes response was received. */
   QAPI_ZB_CL_EVENT_TYPE_WRITE_ATTR_RESPONSE_E,            /**< Indicates that a write attributes response was received. */
   QAPI_ZB_CL_EVENT_TYPE_CONFIG_REPORT_RESPONSE_E,         /**< Indicates that a default response was received. */
   QAPI_ZB_CL_EVENT_TYPE_READ_REPORT_CONFIG_RESPONSE_E,    /**< Indicates that a read reporting configuration response was received. */
   QAPI_ZB_CL_EVENT_TYPE_ATTR_REPORT_E,                    /**< Indicates that a report attributes command was received. */
   QAPI_ZB_CL_EVENT_TYPE_DEFAULT_RESPONSE_E,               /**< Indicates that a default response was received. */
   QAPI_ZB_CL_EVENT_TYPE_DISCOVER_ATTR_RESPONSE_E,         /**< Indicates that a discover attributes response was received. */
   QAPI_ZB_CL_EVENT_TYPE_READ_ATTR_STRUCTURED_RESPONSE_E,  /**< Indicates that a read attributes structured response was received. */
   QAPI_ZB_CL_EVENT_TYPE_WRITE_ATTR_STRUCTURED_RESPONSE_E, /**< Indicates that a write attributes structured response was received. */
} qapi_ZB_CL_Event_Type_t;

/**
   Enumeration of event types provided to custom clusters.
*/
typedef enum
{
   QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_DEFAULT_RESPONSE_E,  /**< Indicates that a default response was received. */
   QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_UNPARSED_RESPONSE_E, /**< Indicates that an unidentified command response was received. */
   QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_UNPARSED_DATA_E,     /**< Indicates that an unidentified command was received. */
   QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_COMMAND_COMPLETE_E,  /**< Indicates that a command that does not expect a response has completed. */
   QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_ATTR_CUSTOM_READ_E,  /**< Indicates that an application controlled attribute needs to be read. */
   QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_ATTR_CUSTOM_WRITE_E, /**< Indicates that an application controlled attribute needs to be written. */
   QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_ALARM_RESET_E,       /**< Indicates that an application controlled attribute needs to reset its alarm
                                                                  condition. */
   QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_GET_SCENE_DATA_E,    /**< Event to query scene data for the cluster. */
   QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_SET_SCENE_DATA_E,    /**< Event to set the scene data for the cluster. */
   QAPI_ZB_CL_CUSTOM_CLUSTER_EVENT_TYPE_CLEANUP_E,           /**< Event to clean up the cluster, freeing all allocated resources. */
} qapi_ZB_CL_Custom_Cluster_Event_Type_t;

/**
   Enumeration of the security levels for a ZCL frame.
*/
typedef enum
{
   QAPI_ZB_CL_SECURITY_LEVEL_UNSECURED_E,       /**< Indicates the packet not secured. */
   QAPI_ZB_CL_SECURITY_LEVEL_SECURED_NWK_KEY_E, /**< Indicates the packet is secured by network key. */
   QAPI_ZB_CL_SECURITY_LEVEL_SECURED_LINK_KEY_E /**< Indicates the packet is secured by a link key. */
} qapi_ZB_CL_Security_Level_t;

/**
   Structure containing the information for a custom attribute read event.
*/
typedef struct qapi_ZB_CL_Attr_Custom_Read_s
{
   /** ID of the attribute being read. */
   uint16_t       AttrId;

   /** Length of the attribute being read. */
   uint16_t      *DataLength;

   /** Pointer to the buffer where the attribute value should be copied before
       the callback function returns. The length of the field is determined by
       the DataLength member.*/
   uint8_t       *Data;

   /** Result of the read operation.*/
   qapi_Status_t *Result;
} qapi_ZB_CL_Attr_Custom_Read_t;

/**
   Structure containing the information for a custom attribute write event.
*/
typedef struct qapi_ZB_CL_Attr_Custom_Write_s
{
   /** Mode of the write operation. */
   qapi_ZB_CL_Write_Mode_t  Mode;

   /** ID of the attribute being written. */
   uint16_t                 AttrId;

   /** Length of the attribute being written. */
   uint16_t                 DataLength;

   /** Buffer containing the new value of the attribute. The length of the
     field is determined by the DataLength member.*/
   const uint8_t           *Data;

   /** Result of the write operation.*/
  qapi_Status_t            *Result;
} qapi_ZB_CL_Attr_Custom_Write_t;

/**
   Structure that defines an attribute.
*/
typedef struct qapi_ZB_CL_Attribute_s
{
   /** ID of the attribute. */
   uint16_t               AttributeId;

   /** Flags for the attribute. */
   uint16_t               Flags;

   /** Data type of the attribute. */
   qapi_ZB_CL_Data_Type_t DataType;

   /** Length of the attribute. This value is ignored for data types with a
       well-defined size. */
   uint32_t               DataLength;

   /** Default minimum reporting interval for the attribute. */
   uint16_t               DefaultReportMin;

   /** Default maximum reporting interval for the attribute. */
   uint16_t               DefaultReportMax;

   /** Minimum value of the attribute for automatic range checking.  Set both
       ValueMin and ValueMax to zero to disable range checking. */
   int64_t                ValueMin;

   /** Maximum value of the attribute for automatic range checking.  Set both
       ValueMin and ValueMax to zero to disable range checking. */
   int64_t                ValueMax;
} qapi_ZB_CL_Attribute_t;

#define QAPI_ZB_CL_ATTRIBUTE_FLAG_WRITABLE                              (0x0001) /**< ZCL attribute flag indicating the attribute is writable. */
#define QAPI_ZB_CL_ATTRIBUTE_FLAG_REPORTABLE                            (0x0002) /**< ZCL attribute flag indicating the attribute is reportable. */
#define QAPI_ZB_CL_ATTRIBUTE_FLAG_PERSISTABLE                           (0x0004) /**< ZCL attribute flag indicating the attribute is persistable. Persistable
                                                                                      attributes will be added to the persistent data store. */
#define QAPI_ZB_CL_ATTRIBUTE_FLAG_CUSTOM_READ_WRITE                     (0x8000) /**< ZCL attribute flag indicating that read and write operations for the
                                                                                      attribute will be handled by the application (via the custom read/write
                                                                                      events). If this flag is set, it is expected that the application will
                                                                                      maintain the value for the attribute, otherwise, the value will be
                                                                                      maintained by the ZigBee stack. */

/**
   Structure that contains the information for initializing a cluster.
*/
typedef struct qapi_ZB_CL_Cluster_Info_s
{
   /** ID of the endpoint the cluster will be part of. */
   uint8_t                       Endpoint;

   /** Number of attributes in the cluster. */
   uint8_t                       AttributeCount;

   /** List of attributes in the cluster. The size of the list is determined by
       the AttributeCount. */
   const qapi_ZB_CL_Attribute_t *AttributeList;
} qapi_ZB_CL_Cluster_Info_t;

/**
   Structure that represents the information for a ZCL frame header.
*/
typedef struct qapi_ZB_CL_Header_s
{
   /** Command represented in this header. */
   uint8_t  CommandId;

   /** Flags from the header control field as defined by
       QAPI_ZB_CL_HEADER_FLAG_*. */
   uint8_t  Flags;

   /** Assigned manufacturer code.  This field is only present if the
       QAPI_ZB_CL_HEADER_FLAG_MANUFACTURER_- SPECIFIC flag is set. */
   uint16_t ManufacturerCode;

   /** Identification number for the transaction. */
   uint8_t  SequenceNumber;
} qapi_ZB_CL_Header_t;

#define QAPI_ZB_CL_HEADER_FLAG_FRAME_TYPE_CLUSTER_SPECIFIC        (0x01) /**< ZCL header flag indicating the command ID is cluster specific. */
#define QAPI_ZB_CL_HEADER_FLAG_MANUFACTURER_SPECIFIC              (0x04) /**< ZCL header flag indicating the header is manufacturer specific. */
#define QAPI_ZB_CL_HEADER_FLAG_DIRECTION_SERVER_TO_CLIENT         (0x08) /**< ZCL header flag indicating the header is sent from the server to the
                                                                              client. */
#define QAPI_ZB_CL_HEADER_FLAG_DISABLE_DEFAULT_RESPONSE           (0x10) /**< ZCL general header flag indicating a response is only expected in an error
                                                                              case. */
/**
   Structure that represents the general packet information required by all
   cluster APIs that send packets.
*/
typedef struct qapi_ZB_CL_General_Send_Info_s
{
   /** Address mode for the destination address. */
   qapi_ZB_Addr_Mode_t DstAddrMode;

   /** Destination address for the frame. */
   qapi_ZB_Addr_t      DstAddress;

   /** Destination endpoint for the frame. */
   uint8_t             DstEndpoint;

   /** ZCL sequence number to use for the command. */
   uint8_t             SeqNum;
} qapi_ZB_CL_General_Send_Info_t;

/**
   Structure that represents the general packet information that is present for
   the main ZCL cluster callback for all frames being received.
*/
typedef struct qapi_ZB_CL_General_Receive_Info_s
{
   /** Network address of the source device. */
   uint16_t            SrcNwkAddress;

   /** Extended address of the source device. */
   uint64_t            SrcExtendedAddress;

   /** Source endpoint for the frame. */
   uint8_t             SrcEndpoint;

   /** Profile ID of the frame. */
   uint16_t            ProfileId;

   /** Link quality indicator of the frame. */
   uint8_t             LinkQuality;

   /** ZCL header information for the frame. */
   qapi_ZB_CL_Header_t ZCLHeader;
} qapi_ZB_CL_General_Receive_Info_t;

/**
   Structure that represents the per-attribute data that is contained within a
   Read Attributes Response structure.
*/
typedef struct qapi_ZB_CL_Read_Attr_Status_Record_s
{
   /** Satus code for reading this attribute. */
   qapi_Status_t           Status;

   /** Identifier of the attribute. */
   uint16_t                AttrId;

   /** Data type of the attribute. */
   qapi_ZB_CL_Data_Type_t  DataType;

   /** Length (in bytes) of the attribute. */
   uint16_t                AttrLength;

   /** Pointer to the actual attribute data. Note that this is not guaranteed
       to be aligned to the underlying type represented by DataType. */
   const uint8_t          *AttrValue;
} qapi_ZB_CL_Read_Attr_Status_Record_t;

/**
   Structure that represents the data that is received in response to a
   successful Read Attributes command.
*/
typedef struct qapi_ZB_CL_Read_Attr_Response_s
{
   /** APS status for the Request command. */
   qapi_Status_t                         Status;

   /** Number of entries in RecordList. */
   uint16_t                              RecordCount;

   /** List of read records for the attribute(s) that were requested. */
   qapi_ZB_CL_Read_Attr_Status_Record_t *RecordList;
} qapi_ZB_CL_Read_Attr_Response_t;

/**
   Structure that represents the per-attribute data that is used when sending
   a Write Attributes Request.
*/
typedef struct qapi_ZB_CL_Write_Attr_Record_s
{
   /** Identifier for the attribute. */
   uint16_t                AttrId;

   /** Data type for the attribute. */
   qapi_ZB_CL_Data_Type_t  DataType;

   /** Length of the data contained in AttrValue. */
   uint16_t                AttrLength;

   /** Pointer to the actual value to be written to the attribute. */
   uint8_t                *AttrValue;
} qapi_ZB_CL_Write_Attr_Record_t;

/**
   Structure that represents the per-attribute data that is received as part of
   a Write Attribute Response.
*/
typedef struct qapi_ZB_CL_Write_Attr_Status_Record_s
{
   /** Status for writing this particular attribute. */
   qapi_Status_t  Status;

   /** Identifier for the attribute. */
   uint16_t       AttrId;
} qapi_ZB_CL_Write_Attr_Status_Record_t;

/**
   Structure that represents the data that is received in reponse to a
   successful Write Attributes command.
*/
typedef struct qapi_ZB_CL_Write_Attr_Response_s
{
   /** APS status for the Request command. */
   qapi_Status_t                          Status;

   /** Number of entries in RecordList. */
   uint16_t                               RecordCount;

   /** List of write statuses for the attribute(s) specified in the Write
       Attributes Request. */
   qapi_ZB_CL_Write_Attr_Status_Record_t *RecordList;
} qapi_ZB_CL_Write_Attr_Response_t;

/**
   Structure that represents the attribute reporting configuration record that
   is used to configure the reporting mechanism for one or more the the
   attributes of a cluster.
*/
typedef struct qapi_ZB_CL_Attr_Reporting_Config_Record_s
{
   /** Direction for this configuration. Depending on this value, it can
       mean that the reporter is indicating how it will report attributes, or
       the receiver is indicating how attributes should be reported to it. */
   qapi_ZB_CL_Attr_Report_Direction_t Direction;

   /** Identifier for the attribute to be reported. */
   uint16_t                           AttrId;

   /** Maximum expected time (in seconds) between reports, otherwise it
       indicates a problem with reporting. A value of zero indicates no timeout
       is specified. Note that this should be set to a value higher than the
       MaxReportingInterval field. */
   uint16_t                           TimeoutPeriod;

   /** Data type of the attribute to be reported. */
   qapi_ZB_CL_Data_Type_t             DataType;

   /** Minimum time, in seconds, between reports. A value of zero indicates
       there is no minimum time to be respected between reports. */
   uint16_t                           MinReportingInterval;

   /** Maximum time, in seconds, between reports. A value of 0xFFFF
       indicates reporting should be disabled for this attribute. If this value
       is zero and MinReportingInterval is 0xFFFF, the reporting configuration
       should be reset to the default. */
   uint16_t                           MaxReportingInterval;

   /** Difference threshold to warrant a report to be
       generated, i.e., the underlying attribute should change by at least the
       number in this field before issuing a new report. @newpagetable */
   uint64_t                           ReportableChange;
} qapi_ZB_CL_Attr_Reporting_Config_Record_t;

/**
   Structure that represents an attribute status record field contained in the
   response to a successful Configure Reporting command.
*/
typedef struct qapi_ZB_CL_Attr_Status_Record_s
{
   /** Status of configuring this attribute's reporting. */
   qapi_Status_t                      Status;

   /** Direction indicating how this configuration is being sent. */
   qapi_ZB_CL_Attr_Report_Direction_t Direction;

   /** Identifier for the attribute. */
   uint16_t                           AttrId;
} qapi_ZB_CL_Attr_Status_Record_t;

/**
   Structure that represents the data that is received in response to a
   successful Configure Reporting command.
*/
typedef struct qapi_ZB_CL_Config_Report_Response_s
{
   /** APS status for the Request command. */
   qapi_Status_t                    Status;

   /** Number of entries in RecordList. */
   uint16_t                         RecordCount;

   /** List of status records returned in the response. */
   qapi_ZB_CL_Attr_Status_Record_t *RecordList;
} qapi_ZB_CL_Config_Report_Response_t;

/**
   Structure that represents the attribute status record field contained in the
   read reporting configuration command frame.
*/
typedef struct qapi_ZB_CL_Attr_Record_s
{
   /** Direction indicating how this report configuration is sent. */
   qapi_ZB_CL_Attr_Report_Direction_t Direction;

   /** Identifier for the attribute. */
   uint16_t                           AttrId;
} qapi_ZB_CL_Attr_Record_t;

/**
   Structure that represents the attribute reporting configuration record
   field contained in a response to a successful Read Reporting command.
*/
typedef struct qapi_ZB_CL_Reporting_Config_Status_Record_s
{
   /** Status for reading the reporting configuration for this attribute. */
   qapi_Status_t                      Status;

   /** Direction indicating how this report configuration is sent. */
   qapi_ZB_CL_Attr_Report_Direction_t Direction;

   /** Identifier for the attribute. */
   uint16_t                           AttrId;

   /** Data type for the attribute. */
   qapi_ZB_CL_Data_Type_t             DataType;

   /** Minimum reporting interval currently in use for this attribute. */
   uint16_t                           MinReportingInterval;

   /** Maximum reporting interval currently in use for this attribute. */
   uint16_t                           MaxReportingInterval;

   /** Length for the value contained in ReportableChange. This is
       dependent solely on the datatype configured in DataType. */
   uint16_t                           ReportableChangeLength;

   /** Threshold difference for generating a report that is currently
       configured for this attribute. */
   uint64_t                           ReportableChange;

   /** Maximum time between reports (in seconds) currently configured for
       this attribute. */
   uint16_t                           TimeoutPeriod;
} qapi_ZB_CL_Reporting_Config_Status_Record_t;

/**
   Structure that represents the data that is received in response to a
   successful Read Reporting command.
*/
typedef struct qapi_ZB_CL_Read_Report_Config_Response_s
{
   /** APS status for the Request command. */
   qapi_Status_t                                Status;

   /** Number of entries in RecordList. */
   uint8_t                                      RecordCount;

   /** List of status records requested to be read. */
   qapi_ZB_CL_Reporting_Config_Status_Record_t *RecordList;
} qapi_ZB_CL_Read_Report_Config_Response_t;

/**
   Structure that represents an individual attribute report used in the Report
   Attributes command frame.
*/
typedef struct qapi_ZB_CL_Attr_Report_s
{
   /** Identifier for the attribute. */
   uint16_t                AttrId;

   /** Data type for the attribute. */
   qapi_ZB_CL_Data_Type_t  DataType;

   /** Length, in bytes, of the data contained in AttrData. */
   uint8_t                 AttrLength;

   /** Pointer to the actual data being reported for this attribute. Note that
       this is not guaranteed to be aligned to the data type indicated in
       the DataType field. */
   const uint8_t          *AttrData;
} qapi_ZB_CL_Attr_Report_t;

/**
   Structure that represents the data that is received from a Default Response,
   generated by any ZCL command that requests a default response (or in error),
   as opposed to a command-specific response.
*/
typedef struct qapi_ZB_CL_Default_Response_s
{
   /** APS status for the Request command. */
   qapi_Status_t Status;

   /** Identifier for the command for which the default response was
       generated. */
   uint8_t       CommandId;

   /** Status being reported for the command. */
   qapi_Status_t CommandStatus;
} qapi_ZB_CL_Default_Response_t;

/**
   Structure that represents the event generated when sending a command that
   does not expect a response. This event is generated immediately after the
   APS frame is sent successfully.
*/
typedef struct qapi_ZB_CL_Command_Complete_s
{
   /** APS status of the command frame that was sent. */
   qapi_Status_t CommandStatus;
} qapi_ZB_CL_Command_Complete_t;

/**
   Structure that represents the per-attribute data that is received as a part
   of a Discover Attributes Response.
*/
typedef struct qapi_ZB_CL_Discover_Attr_Report_s
{
   /** Identifier for the attribute. */
   uint16_t               AttrId;

   /** Data type for the attribute. */
   qapi_ZB_CL_Data_Type_t DataType;
} qapi_ZB_CL_Discover_Attr_Report_t;

/**
   Structure that represents the data that is received in response to a
   successful Discover Attributes command.
*/
typedef struct qapi_ZB_CL_Discover_Attr_Response_s
{
   /** APS status for the Request command. */
   qapi_Status_t                      Status;

   /** Boolean value that indicates whether any attributes remain after the last
       attribute in ReportList. If this is set to 0, another Discover Attributes
       Request may be sent with the StartAttrId set to the next starting ID for
       any remaining attributes. */
   qbool_t                            DiscoverComplete;

   /** Number of entries in ReportList. */
   uint8_t                            ReportCount;

   /** List of attributes that were reported in this response. */
   qapi_ZB_CL_Discover_Attr_Report_t *ReportList;
} qapi_ZB_CL_Discover_Attr_Response_t;

/**
   Structure that represents the data used when generating a Read Attributes
   Structured request.
*/
typedef struct qapi_ZB_CL_Read_Attr_Structured_s
{
   /** Identifier for the attribute. */
   uint16_t  AttrId;

   /** Number of entries in IndexList. This is limited to [0-15]. If the
       value is zero, it indicates the entire attribute should be read. */
   uint8_t   Indicator;

   /** Index list to be read for this attribute, if provided. */
   uint16_t *IndexList;
} qapi_ZB_CL_Read_Attr_Structured_t;

/**
   Structure that represents the per-attribute data contained within a Write
   Attributes Structured Request.
*/
typedef struct qapi_ZB_CL_Write_Attr_Structured_Record_s
{
   /** Identifier of the attribute to be written. */
   uint16_t                AttrId;

   /** Lower four bits of this represent the number of indices in IndexList.
       When the attribute being written is a Set or Bag, the upper four bits are
       used for the following:
         - 0x00: Write whole set/bag.
         - 0x10: Add element(s) to the set/bag.
         - 0x20: Remove element(s) from the set/bag. */
   uint8_t                 Indicator;

   /** List of indices, if provided, for the data to be written. Note that the
       structure and array elements have a 1-based index, where index 0 is
       reserved for the length of the underlying array/struct. */
   uint16_t               *IndexList;

   /** Data type of the attribute being written. */
   qapi_ZB_CL_Data_Type_t  AttrDataType;

   /** Length of the data in AttrValue. */
   uint16_t                AttrValueLength;

   /** Pointer to the data being written to the attribute or subindices. */
   uint8_t                *AttrValue;
} qapi_ZB_CL_Write_Attr_Structured_Record_t;

/**
   Structure that represents the per-attribute data contained within a Write
   Attributes Structured Response.
*/
typedef struct qapi_ZB_CL_Write_Attr_Structured_Status_Record_s
{
   /** Status for writing this particular attribute. */
   qapi_Status_t  Status;

   /** Identifier for this attribute. */
   uint16_t       AttrId;

   /** This field has the same structure and meaning as the Indicator field in
       #qapi_ZB_CL_Write_Attr_Structured_Record_t. */
   uint8_t        Indicator;

   /** List of indices, if used, for the data that was written. */
   uint16_t      *IndexList;
} qapi_ZB_CL_Write_Attr_Structured_Status_Record_t;

/**
   Structure that represents the data that is received in response to a
   successful Write Attributes Structured command.
*/
typedef struct qapi_ZB_CL_Write_Attr_Structured_Response_s
{
   /** APS status for the Request command. */
   qapi_Status_t                                     Status;

   /** Number of entries in RecordList. */
   uint8_t                                           RecordCount;

   /** List of records returned from the Write Attributes Request. */
   qapi_ZB_CL_Write_Attr_Structured_Status_Record_t *RecordList;
} qapi_ZB_CL_Write_Attr_Structured_Response_t;

/**
   Structure that represents unparsed ZCL event data.
*/
typedef struct qapi_ZB_CL_Unparsed_Data_s
{
   /** Header of the ZCL frame. */
   qapi_ZB_CL_Header_t              Header;

   /** APS data indication. */
   qapi_ZB_APSDE_Data_Indication_t  APSDEData;

   /** Status for handling the unparsed data. */
   qapi_Status_t                   *Result;
} qapi_ZB_CL_Unparsed_Data_t;

/**
   Structure that represents unparsed ZCL response data.
*/
typedef struct qapi_ZB_CL_Unparsed_Response_s
{
   /** Response status. */
   qapi_Status_t          Status;

   /** Source address information of the received response. */
   qapi_ZB_APS_Address_t  SrcAddr;

   /** Header of the response frame. */
   qapi_ZB_CL_Header_t    Header;

   /** Profile ID. */
   uint16_t               Profile_ID;

   /** Link quality. */
   uint8_t                Link_Quality;

   /** Pointer to the unparsed response. */
   const void            *Payload;

   /** Length of the unparsed response, in bytes. */
   uint32_t               PayloadLength;
} qapi_ZB_CL_Unparsed_Response_t;

/**
   Structure that represents the event data for general ZCL command and response.
*/
typedef struct qapi_ZB_CL_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_Event_Type_t           Event_Type;

   /** Contains the originator address for the frame as well as link quality,
       the destination profile, and the ZCL frame header. */
   qapi_ZB_CL_General_Receive_Info_t Receive_Info;

   /** Data for the event.  The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Contains the Read Attributes Response data for the corresponding
          event. */
      qapi_ZB_CL_Read_Attr_Response_t                   Read_Attr_Response;

      /** Contains the Write Attributes Response data for the corresponding
          event. */
      qapi_ZB_CL_Write_Attr_Response_t                  Write_Attr_Response;

      /** Contains the Configure Reporting Response data for the corresponding
          event. */
      qapi_ZB_CL_Config_Report_Response_t               Config_Report_Response;

      /** Contains the Read Reporting Configuration Response data for the
          corresponding event. */
      qapi_ZB_CL_Read_Report_Config_Response_t          Read_Report_Config_Response;

      /** Contains the Attribute Report data for the corresponding event. */
      qapi_ZB_CL_Attr_Report_t                          Attr_Report;

      /** Contains the Default Response data for the corresponding event. */
      qapi_ZB_CL_Default_Response_t                     Default_Response;

      /** Contains the Discover Attributes Response data for the corresponding
          event. */
      qapi_ZB_CL_Discover_Attr_Response_t               Discover_Attr_Response;

      /** Contains the Write Attributes Structured Response data for the
          corresponding event. */
      qapi_ZB_CL_Write_Attr_Structured_Response_t       Write_Attr_Structured_Response;

      /** Contains the unparsed response. */
      qapi_ZB_CL_Unparsed_Response_t                    Unparsed_Response;
   } Data;
} qapi_ZB_CL_Event_Data_t;

/**
   Structure that contains the data for when a command is received for a
   cluster registered using qapi_ZB_CL_Create_Cluster().
*/
typedef struct qapi_ZB_CL_Custom_Cluster_Command_s
{
   /** ZCL header for the command frame. */
   qapi_ZB_CL_Header_t             ZCLHeader;

   /** APSDE frame information for the command. */
   qapi_ZB_APSDE_Data_Indication_t APSDEData;
} qapi_ZB_CL_Custom_Cluster_Command_t;

/**
   Structure that represents the event generated when scene data is requested
   for a cluster registered using qapi_ZB_CL_Create_Cluster().
*/
typedef struct qapi_ZB_CL_Custom_Cluster_Get_Scene_Data_s
{
   /** Length of the buffer provided to store scene data. After the operation
       is completed this field should contain the actual length of the scene
       data. */
   uint8_t           *DataLength;

   /** Buffer to which scene data should be written. */
   uint8_t           *Data;

   /** Result of the get scene data operaiton. */
   qapi_Status_t     *Result;
} qapi_ZB_CL_Custom_Cluster_Get_Scene_Data_t;

/**
   Structure that represents the event generated when scene data is written
   to a cluster registered using qapi_ZB_CL_Create_Cluster().
*/
typedef struct qapi_ZB_CL_Custom_Cluster_Set_Scene_Data_s
{
   /** Requested transition time to the new scene data. */
   uint16_t           TransitionTime;

   /** Length, in bytes, of the data payload. */
   uint8_t            DataLength;

   /** Buffer containing scene data to be made active. */
   uint8_t           *Data;

   /** Result of the set scene data operation. */
   qapi_Status_t     *Result;
} qapi_ZB_CL_Custom_Cluster_Set_Scene_Data_t;

/**
   Structure that represents the event generated when an alarm is reset
   on a cluster registered using qapi_ZB_CL_Create_Cluster().
*/
typedef struct qapi_ZB_CL_Custom_Cluster_Reset_Alarm_s
{
   /** Alarm code. */
   uint8_t           AlarmCode;

   /** Cluster ID */
   uint16_t          ClusterId;
} qapi_ZB_CL_Custom_Cluster_Reset_Alarm_t;

/**
   Structure that represents the event generated when an attribute should be set
   to its default value on a cluster registered using
   qapi_ZB_CL_Create_Cluster().
*/
typedef struct qapi_ZB_CL_Custom_Cluster_Attr_Set_Default_s
{
   /** Attribute ID. */
   uint16_t          AttrId;
} qapi_ZB_CL_Custom_Cluster_Attr_Set_Default_t;

/**
   Structure that represents the event generated when an attribute should be set
   to its default value on a cluster registered using
   qapi_ZB_CL_Create_Cluster().
*/
typedef struct qapi_ZB_CL_Custom_Cluster_Attr_Write_Notifiction_s
{
   /** Attribute ID. */
   uint16_t          AttrId;
} qapi_ZB_CL_Custom_Cluster_Attr_Write_Notifiction_t;

/**
   Structure that represents the events that can be generated for a custom
   cluster created using qapi_ZB_CL_Create_Cluster().
*/
typedef struct qapi_ZB_CL_Custom_Cluster_Event_Data_s
{
   /** Type of event represented by this structure. */
   qapi_ZB_CL_Custom_Cluster_Event_Type_t Event_Type;

   /** Data for the event. The actual structure used is determined by the
       Event_Type field. */
   union
   {
      /** Information for default response events. */
      qapi_ZB_CL_Default_Response_t                Default_Response;

      /** Information for unparsed command response events. */
      qapi_ZB_CL_Unparsed_Response_t               Unparsed_Response;

      /** Information for unparsed data events. */
      qapi_ZB_CL_Unparsed_Data_t                   Unparsed_Data;

      /** Information for command complete events. */
      qapi_ZB_CL_Command_Complete_t                Command_Complete;

      /** Information for custom attribute read events. */
      qapi_ZB_CL_Attr_Custom_Read_t                Attr_Custom_Read;

      /** Information for custom attribute write events. */
      qapi_ZB_CL_Attr_Custom_Write_t               Attr_Custom_Write;

      /** Information for get scene data events. */
      qapi_ZB_CL_Custom_Cluster_Get_Scene_Data_t   Get_Scene_Data;

      /** Information for set scene data events. */
      qapi_ZB_CL_Custom_Cluster_Set_Scene_Data_t   Set_Scene_Data;

      /** Information for reset alarm events. */
      qapi_ZB_CL_Custom_Cluster_Reset_Alarm_t      Reset_Alarm_Data;
   } Data;
} qapi_ZB_CL_Custom_Cluster_Event_Data_t;

/**
   @brief Callback function that handles asynchronous cluster events.

   @param[in] ZB_Handle Handle of the ZigBee instance that was returned by a
                        successful call to qapi_ZB_Initialize().
   @param[in] CL_Event  Information for the cluster event.
   @param[in] CB_Param  User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_Event_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_Event_Data_t *Event_Data, uint32_t CB_Param);

/**
   @brief Callback function that handles custom cluster events.

   @param[in] ZB_Handle Handle of the ZigBee instance was returned by a
                        successful call to qapi_ZB_Initialize().
   @param[in] CL_Event  Information for the cluster event.
   @param[in] CB_Param  User-specified parameter for the callback function.
*/
typedef void (*qapi_ZB_CL_Custom_Cluster_Event_CB_t)(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_Custom_Cluster_Event_Data_t *Event_Data, uint32_t CB_Param);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Registers a cluster event callback.

   @param[in] ZB_Handle   Handle of a ZigBee stack.
   @param[in] CL_Event_CB Cluster event callback function.
   @param[in] CB_Param    Callback parameter for the cluster event
                          callback function.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Register_Callback(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_CL_Event_CB_t CL_Event_CB, uint32_t CB_Param);

/**
   @brief Unregisters a ZCL callback function.

   @param[in] ZB_Handle   Handle of the ZigBee instance was returned by a
                          successful call to qapi_ZB_Initialize().
   @param[in] CL_Event_CB Callback function to unregister.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Unregister_Callback(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_CL_Event_CB_t CL_Event_CB);

/**
   @brief Creates a custom ZigBee cluster to receive low-level events.

   @param[in]  ZB_Handle    Handle of the ZigBee instance was returned by
                            a successful call to qapi_ZB_Initialize().
   @param[out] Cluster      Pointer to where the newly created cluster
                            handle is to be written.
   @param[in]  ClusterId    Identifier for the new cluster.
   @param[in]  Cluster_Info Attribute information and the endpoint
                            ID for the new cluster.
   @param[in]  Direction    Direction for the ZCL frame send from the cluster
                            being created.
   @param[in]  Event_CB     Callback function to register.
   @param[in]  CB_Param     Callback parameter.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Create_Cluster(qapi_ZB_Handle_t ZB_Handle, qapi_ZB_Cluster_t *Cluster, uint16_t ClusterId, qapi_ZB_CL_Cluster_Info_t *Cluster_Info, qapi_ZB_CL_Frame_Direction_t Direction, qapi_ZB_CL_Custom_Cluster_Event_CB_t Event_CB, uint32_t CB_Param);

/**
   @brief Destroys a ZigBee cluster created by either a cluster-specific API or
          from using qapi_ZB_CL_Create_Cluster().

   @param[in] Cluster Handle of cluster to be destroyed.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Destroy_Cluster(qapi_ZB_Cluster_t Cluster);

/**
   @brief Retrieves the endpoint, profile, and cluster identifiers for a
          ZigBee cluster.

   @param[in]  Cluster   Handle of the cluster to be used.
   @param[out] Endpoint  Pointer to where the endpoint identifier will
                         be written.
   @param[out] ProfileId Pointer to where the profile identifier will
                         be written.
   @param[out] ClusterId Pointer to where the cluster identifier will
                         be written.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Get_Cluster_Info(qapi_ZB_Cluster_t Cluster, uint8_t *Endpoint, uint16_t *ProfileId, uint16_t *ClusterId);

/**
   @brief Sets the profile ID for the cluster.

   @param[in] Cluster   Handle of the cluster.
   @param[in] ProfileID ProfileID for the cluster.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Set_Profile_ID(qapi_ZB_Cluster_t Cluster, uint16_t ProfileID);

/**
   @brief Updates the tx options for the cluster to reflect the security level
          specified.

   The Tx options this API modifies are used both for outgoing packets and to
   validate incomming packets.

   @param[in] Cluster     Handle of the cluster.
   @param[in] MinSecurity Minimum security level for the cluster.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Set_Min_Security(qapi_ZB_Cluster_t Cluster, qapi_ZB_CL_Security_Level_t MinSecurity);

/**
   @brief Sets the tx options for the cluster.

   These Tx options are used both for packets sent by the cluster and to
   validate packets received by the cluster (such as the minimum security
   level).

   @param[in] Cluster   Handle of the cluster.
   @param[in] TxOptions Tx options for the clsuter.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Set_Tx_Options(qapi_ZB_Cluster_t Cluster, uint8_t TxOptions);

/**
   @brief Gets the tx options for the cluster.

   @param[in]  Cluster   Handle of the cluster.
   @param[out] TxOptions Tx options for the clsuter.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Get_Tx_Options(qapi_ZB_Cluster_t Cluster, uint8_t *TxOptions);

/**
   @brief Sets the value of the discover route flag used when sending packets
          for the specified cluster.

   @param[in] Cluster       Handle of the cluster.
   @param[in] DiscoverRoute Value of the discover route flag.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Set_Discover_Route(qapi_ZB_Cluster_t Cluster, qbool_t DiscoverRoute);

/**
   @brief Gets the value of the discover route flag for the specified cluster.

   @param[in]  Cluster       Handle of the cluster.
   @param[out] DiscoverRoute Value of the discover route flag.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Get_Discover_Route(qapi_ZB_Cluster_t Cluster, qbool_t *DiscoverRoute);

/**
   @brief Sets the radius used when sending packets for the specified cluster.

   @param[in] Cluster Handle of the cluster.
   @param[in] Radius  Radius used when sending packets for this cluster.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Set_Radius(qapi_ZB_Cluster_t Cluster, uint8_t Radius);

/**
   @brief Sets the radius for the specified cluster.

   @param[in]  Cluster Handle of the cluster.
   @param[out] Radius  Radius used when sending packets for this cluster.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Get_Radius(qapi_ZB_Cluster_t Cluster, uint8_t *Radius);

/**
   @brief Sends a ZCL command to a remote device.

   @param[in] Cluster          Handle of the cluster to be used.
   @param[in] SendInfo         Destination information for the command.
   @param[in] ResponseExpected Flag indicating if a response is expected for
                               the command. This only applies for commands that
                               are not broadcast.
   @param[in] ZCL_Header       Information for the ZCL header of the command.
   @param[in] PayloadLength    Length of the payload for the command.
   @param[in] Payload          Payload for the command.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Send_Command(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, qbool_t ResponseExpected, const qapi_ZB_CL_Header_t *ZCL_Header, uint16_t PayloadLength, const uint8_t *Payload);

/**
   @brief Reads a local attribute on the speicified cluster.

   @param[in]     Cluster     Handle of the cluster on which the
                              attribute will be read.
   @param[in]     AttrId      ID of the attribute on the specied the
                              cluster.
   @param[in,out] Length      Pointer to where the length of the requested
                              attribute is stored. If the value in it is not
                              the actual attribute length, this function will
                              return QAPI_ERR_BOUNDS and set the actual length
                              into the memory identified by this pointer.
                              Otherwise, this function will try to read the
                              attribute specified.
   @param[out]    Data        Pointer to the buffer that will store the
                              requested attribute upon successful return.

   @return
     - QAPI_OK          if the request executed successfully.
     - QAPI_ERR_BOUNDS  if the buffer holding the attribute is not big enough.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Read_Local_Attribute(qapi_ZB_Cluster_t Cluster, uint16_t AttrId, uint16_t *Length, uint8_t *Data);

/**
   @brief Writes data to a single attribute in a local cluster.

   @param[in] Cluster Handle of the cluster to be used.
   @param[in] AttrId  Identifier of the attribute to written.
   @param[in] Length  Length of the buffer provided.
   @param[in] Data    Pointer to the buffer containing the data to write.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Write_Local_Attribute(qapi_ZB_Cluster_t Cluster, uint16_t AttrId, uint16_t Length, const uint8_t *Data);

/**
   @brief Issues a Read Attributes Request via a local cluster.

   @param[in] Cluster    Handle of the cluster to be used.
   @param[in] SendInfo   Destination information for the command.
   @param[in] AttrCount  Number of entries in AttrIdList.
   @param[in] AttrIdList List of attribute IDs to read.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Read_Attributes(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint8_t AttrCount, const uint16_t *AttrIdList);

/**
   @brief Issues a Write Attributes Request via a local cluster.

   @param[in] Cluster        Handle of the cluster to be used.
   @param[in] SendInfo       Destination information for the command.
   @param[in] AttrCount      Number of entries in AttrRecordList.
   @param[in] AttrRecordList List of records containing the attributes to
                             write as well as their data.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Write_Attributes(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint8_t AttrCount, const qapi_ZB_CL_Write_Attr_Record_t *AttrRecordList);

/**
   @brief Issues a Configure Reporting Request via a local cluster.

   @param[in] Cluster          Handle of the cluster to be used.
   @param[in] SendInfo         Destination information for the command.
   @param[in] ReportCount      Number of entries in ReportRecordList.
   @param[in] ReportRecordList List of reporting records for each attribute
                               to be configured for reporting.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Configure_Reporting(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint8_t ReportCount, const qapi_ZB_CL_Attr_Reporting_Config_Record_t *ReportRecordList);

/**
   @brief Issues a Read Reporting Configuration Request via a local cluster.

   @param[in] Cluster          Handle of the cluster to be used.
   @param[in] SendInfo         Destination information for the command.
   @param[in] ReportCount      Number of entries in ReportConfigList.
   @param[in] ReportConfigList List of reporting records for the attributes
                               to be read.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Read_Reporting_Config(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, uint8_t ReportCount, const qapi_ZB_CL_Attr_Record_t *ReportConfigList);

/**
   @brief Issues a Discover Attributes Request via a local cluster.

   @param[in] Cluster     Handle of the cluster to be used.
   @param[in] SendInfo    Destination information for the command.
   @param[in] StartAttrId Identifier of the first attribute requested to
                          be returned in the Discover Attributes Response.
   @param[in] AttrIdCount Number of attributes requested to be returned
                          in the Discover Attributes Response.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Discover_Attributes(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, const uint16_t StartAttrId, const uint8_t AttrIdCount);

/**
   @brief Issues a Read Attributes Structured Request via a local cluster.

   @param[in] Cluster            Handle of the cluster to be used.
   @param[in] SendInfo           Destination information for the command.
   @param[in] AttrCount          Number of entries in AttrStructuredList.
   @param[in] AttrStructuredList List of structures describing which
                                 attributes should be read and how they should
                                 be read (offsets, etc).

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Read_Attributes_Structured(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, const uint8_t AttrCount, const qapi_ZB_CL_Read_Attr_Structured_t *AttrStructuredList);

/**
   @brief Issues a Write Attributes Structured Request via a local cluster.

   @param[in] Cluster            Handle of the cluster to be used.
   @param[in] SendInfo           Destination information for the command.
   @param[in] AttrCount          Number of entries in AttrStructuredList.
   @param[in] AttrStructuredList List of structures describing which
                                 attributes to write, how to write them, and the
                                 data to be written.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Write_Attributes_Structured(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, const uint8_t AttrCount, const qapi_ZB_CL_Write_Attr_Structured_Record_t *AttrStructuredList);

/**
   @brief Issues a Report Attributes command via a local cluster.

   @param[in] Cluster     Handle of the cluster to be used.
   @param[in] SendInfo    Destination information for the command.
   @param[in] ReportCount Number of entries in ReportList.
   @param[in] ReportList  List of structures containing the attributes that
                          are being reported as well as the new value(s) for the
                          attributes.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Report_Attributes(qapi_ZB_Cluster_t Cluster, const qapi_ZB_CL_General_Send_Info_t *SendInfo, const uint8_t ReportCount, const qapi_ZB_CL_Attr_Report_t *ReportList);

/**
   @brief Sends a default response for a received ZCL command.

   @param[in] Cluster   Handle of the cluster to be used.
   @param[in] APSDEData Information about the APSDE-DATA.indication for the
                        received ZCL command.
   @param[in] ZCLHeader ZCL header of the recieved command.
   @param[in] Status    Status for the response.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Default_Response(qapi_ZB_Cluster_t Cluster, const qapi_ZB_APSDE_Data_Indication_t *APSDEData, const qapi_ZB_CL_Header_t *ZCLHeader, qapi_Status_t Status);

/**
   @brief Sends a generic response for a received ZCL command.

   @param[in] Cluster       Handle of the cluster to be used.
   @param[in] APSDEData     Information about the APSDE-DATA.indication for the
                            received ZCL command.
   @param[in] ZCLHeader     ZCL header of the recieved command.
   @param[in] CommandId     Command identifier for the response.
   @param[in] PayloadLength Length of the payload of the response.
   @param[in] Payload       Payload of the response.

   @return
     - QAPI_OK if the request executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Generic_Response(qapi_ZB_Cluster_t Cluster, const qapi_ZB_APSDE_Data_Indication_t *APSDEData, const qapi_ZB_CL_Header_t *ZCLHeader, uint8_t CommandId, uint8_t PayloadLength, uint8_t *PayLoad);

/** @}
*/

#endif  // ] #ifndef __QAPI_ZB_CL_H__

