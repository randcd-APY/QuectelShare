/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef __QAPI_BLE_GLS_TYPES_H__
#define __QAPI_BLE_GLS_TYPES_H__

#include "./qapi_ble_bttypes.h" /* Bluetooth Type Definitions.          */

   /* The following define the defined GLS Error Codes that may be sent */
   /* in a GATT Error Response.                                         */
#define QAPI_BLE_GLS_ERROR_CODE_SUCCESS                                             0x00
#define QAPI_BLE_GLS_ERROR_CODE_PROCEDURE_ALREADY_IN_PROGRESS                       0x80
#define QAPI_BLE_GLS_ERROR_CODE_CHARACTERISTIC_CONFIGURATION_IMPROPERLY_CONFIGURED  0x81

   /* The following MACRO is a utility MACRO that assigns the Glucose   */
   /* Service 16 bit UUID to the specified qapi_BLE_UUID_16_t variable. */
   /* This MACRO accepts one parameter which is a pointer to a          */
   /* qapi_BLE_UUID_16_t variable that is to receive the GLS UUID       */
   /* Constant value.                                                   */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_GLS_ASSIGN_SERVICE_UUID_16(_x)           QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16(*((qapi_BLE_UUID_16_t *)(_x)), 0x18, 0x08)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined GLS Service UUID in UUID16 form.  This     */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the GLS Service UUID (MACRO returns boolean result) NOT  */
   /* less than/greater than.  The first parameter is the               */
   /* qapi_BLE_UUID_16_t variable to compare to the GLS Service UUID.   */
#define QAPI_BLE_GLS_COMPARE_SERVICE_UUID_TO_UUID_16(_x)  QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x08)

   /* The following defines the Glucose Service UUID that is used when  */
   /* building the GLS Service Table.                                   */
#define QAPI_BLE_GLS_SERVICE_BLUETOOTH_UUID_CONSTANT      { 0x08, 0x18 }

   /* The following MACRO is a utility MACRO that assigns the Glucose   */
   /* Measurement Characteristic 16 bit UUID to the specified           */
   /* qapi_BLE_UUID_16_t variable.  This MACRO accepts one parameter    */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* Glucose Measurement UUID Constant value.                          */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_GLS_ASSIGN_MEASUREMENT_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x18)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined Glucose Measurement UUID in UUID16 form.   */
   /* This MACRO only returns whether the qapi_BLE_UUID_16_t variable is*/
   /* equal to the Glucose Measurement UUID (MACRO returns boolean      */
   /* result) NOT less than/greater than.  The first parameter is the   */
   /* qapi_BLE_UUID_16_t variable to compare to the Glucose Measurement */
   /* UUID.                                                             */
#define QAPI_BLE_GLS_COMPARE_MEASUREMENT_UUID_TO_UUID_16(_x)             QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x18)

   /* The following defines the Glucose Measurement Characteristic UUID */
   /* that is used when building the GLS Service Table.                 */
#define QAPI_BLE_GLS_MEASUREMENT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x18, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the Glucose   */
   /* Measurement Context Characteristic 16 bit UUID to the specified   */
   /* qapi_BLE_UUID_16_t variable.  This MACRO accepts one parameter    */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* Glucose Measurement Context UUID Constant value.                  */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_GLS_ASSIGN_MEASUREMENT_CONTEXT_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x34)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined Glucose Measurement Context UUID in UUID16 */
   /* form.  This MACRO only returns whether the qapi_BLE_UUID_16_t     */
   /* variable is equal to the Glucose Measurement Context UUID (MACRO  */
   /* returns boolean result) NOT less than/greater than.  The first    */
   /* parameter is the qapi_BLE_UUID_16_t variable to compare to the    */
   /* Glucose Measurement Context UUID.                                 */
#define QAPI_BLE_GLS_COMPARE_MEASUREMENT_CONTEXT_UUID_TO_UUID_16(_x)             QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x34)

   /* The following defines the Glucose Measurement Characteristic UUID */
   /* that is used when building the GLS Service Table.                 */
#define QAPI_BLE_GLS_MEASUREMENT_CONTEXT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x34, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the Glucose   */
   /* Feature Type Characteristic 16 bit UUID to the specified          */
   /* qapi_BLE_UUID_16_t variable.  This MACRO accepts one parameter    */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* Glucose Feature Type UUID Constant value.                         */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_GLS_ASSIGN_FEATURE_TYPE_UUID_16(_x)                 QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x51)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined Glucose Feature Type UUID in UUID16 form.  */
   /* This MACRO only returns whether the qapi_BLE_UUID_16_t variable is*/
   /* equal to the Feature Type UUID (MACRO returns boolean result) NOT */
   /* less than/greater than.  The first parameter is the               */
   /* qapi_BLE_UUID_16_t variable to compare to the Glucose Feature Type*/
   /* UUID.                                                             */
#define QAPI_BLE_GLS_COMPARE_FEATURE_TYPE_UUID_TO_UUID_16(_x)        QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x51)

   /* The following defines the Glucose Feature Type Characteristic UUID*/
   /* that is used when building the GLS Service Table.                 */
#define QAPI_BLE_GLS_FEATURE_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x51, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the Record    */
   /* Access Control Point Type Characteristic 16 bit UUID to the       */
   /* specified qapi_BLE_UUID_16_t variable.  This MACRO accepts one    */
   /* parameter which is the qapi_BLE_UUID_16_t variable that is to     */
   /* receive the Record Access Control Point Type UUID Constant value. */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_GLS_ASSIGN_RECORD_ACCESS_CONTROL_POINT_TYPE_UUID_16(_x)                 QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x52)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined Record Access Control Point Type UUID in   */
   /* UUID16 form.  This MACRO only returns whether the                 */
   /* qapi_BLE_UUID_16_t variable is equal to the Record Access Control */
   /* Point Type UUID (MACRO returns boolean result) NOT less           */
   /* than/greater than.  The first parameter is the qapi_BLE_UUID_16_t */
   /* variable to compare to the Record Access Control Point Type UUID. */
#define QAPI_BLE_GLS_COMPARE_RECORD_ACCESS_CONTROL_POINT_TYPE_UUID_TO_UUID_16(_x)        QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x52)

   /* The following defines the Record Access Control Point Type        */
   /* Characteristic UUID that is used when building the GLS Service    */
   /* Table.                                                            */
#define QAPI_BLE_GLS_RECORD_ACCESS_CONTROL_POINT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x52, 0x2A }

   /* The following defines the valid Glucose Measurement Flags bit that*/
   /* may be set in the Flags field of a Glucose Measurement            */
   /* characteristic.                                                   */
#define QAPI_BLE_GLS_MEASUREMENT_FLAGS_TIME_OFFSET_PRESENT                             0x01
#define QAPI_BLE_GLS_MEASUREMENT_FLAGS_CONCENTRATION_AND_TYPE_SAMPLE_LOCATION_PRESENT  0x02
#define QAPI_BLE_GLS_MEASUREMENT_FLAGS_CONCENTRATION_IN_MOL_PER_LITER                  0x04
#define QAPI_BLE_GLS_MEASUREMENT_FLAGS_SENSOR_STATUS_ANNUNCIATION_PRESENT              0x08
#define QAPI_BLE_GLS_MEASUREMENT_FLAGS_CONTEXT_INFORMATION_PRESENT                     0x10

   /* The following defines the valid Time Offset values that may be set*/
   /* as the value for the Time Offset field a Glucose Measurement      */
   /* characteristic.                                                   */
#define QAPI_BLE_GLS_TIME_OFFSET_VALUE_OVERRUN           0x7FFF
#define QAPI_BLE_GLS_TIME_OFFSET_VALUE_UNDERRUN          0x8000

   /* The following defines the valid Glucose Measurement Type values   */
   /* that may be set in the Type field of a Glucose Measurement        */
   /* characteristic.                                                   */
#define QAPI_BLE_GLS_TYPE_CAPILLARY_WHOLE_BLOOD          0x01
#define QAPI_BLE_GLS_TYPE_CAPILLARY_PLASMA               0x02
#define QAPI_BLE_GLS_TYPE_VENOUS_WHOLE_BLOOD             0x03
#define QAPI_BLE_GLS_TYPE_VENOUS_PLASMA                  0x04
#define QAPI_BLE_GLS_TYPE_ARTERIAL_WHOLE_BLOOD           0x05
#define QAPI_BLE_GLS_TYPE_ARTERIAL_PLASMA                0x06
#define QAPI_BLE_GLS_TYPE_UNDETERMINED_WHOLE_BLOOD       0x07
#define QAPI_BLE_GLS_TYPE_UNDETERMINED_PLASMA            0x08
#define QAPI_BLE_GLS_TYPE_INTERSTITIAL_FLUID             0x09
#define QAPI_BLE_GLS_TYPE_CONTROL_SOLUTION               0x0A

   /* The following defines the valid Glucose Measurement Sample        */
   /* Location values that may be set in the Sample Location of a       */
   /* Glucose Measurement characteristic.                               */
#define QAPI_BLE_GLS_SAMPLE_LOCATION_FINGER               0x01
#define QAPI_BLE_GLS_SAMPLE_LOCATION_ALTERNATE_SITE_TEST  0x02
#define QAPI_BLE_GLS_SAMPLE_LOCATION_EARLOBE              0x03
#define QAPI_BLE_GLS_SAMPLE_LOCATION_CONTROL_SOLUTION     0x04
#define QAPI_BLE_GLS_SAMPLE_LOCATION_NOT_AVAILABLE        0x0F

   /* The following defines the valid Glucose Measurement Sensor Status */
   /* Annunciation values that may be set in the Sensor Status          */
   /* Annunciation field of a Glucose Measurement characteristic.       */
#define QAPI_BLE_GLS_SENSOR_STATUS_ANNUNCIATION_BATTERY_LOW_AT_TIME_OF_MEASUREMENT  0x0001
#define QAPI_BLE_GLS_SENSOR_STATUS_ANNUNCIATION_SENSOR_MALFUNCTION                  0x0002
#define QAPI_BLE_GLS_SENSOR_STATUS_ANNUNCIATION_SAMPLE_SIZE_INSUFFICIENT            0x0004
#define QAPI_BLE_GLS_SENSOR_STATUS_ANNUNCIATION_STRIP_INSERTION_ERROR               0x0008
#define QAPI_BLE_GLS_SENSOR_STATUS_ANNUNCIATION_STRIP_TYPE_INCORRECT                0x0010
#define QAPI_BLE_GLS_SENSOR_STATUS_ANNUNCIATION_SENSOR_RESULT_TOO_HIGH              0x0020
#define QAPI_BLE_GLS_SENSOR_STATUS_ANNUNCIATION_SENSOR_RESULT_TOO_LOW               0x0040
#define QAPI_BLE_GLS_SENSOR_STATUS_ANNUNCIATION_SENSOR_TEMPERATURE_TOO_HIGH         0x0080
#define QAPI_BLE_GLS_SENSOR_STATUS_ANNUNCIATION_SENSOR_TEMPERATURE_TOO_LOW          0x0100
#define QAPI_BLE_GLS_SENSOR_STATUS_ANNUNCIATION_SENSOR_READ_INTERRUPTED             0x0200
#define QAPI_BLE_GLS_SENSOR_STATUS_ANNUNCIATION_GENERAL_DEVICE_FAULT                0x0400
#define QAPI_BLE_GLS_SENSOR_STATUS_ANNUNCIATION_TIME_FAULT                          0x0800

   /* The following defines the valid Glucose Measurement Context Flags */
   /* bit values that may be set in the Flags field of a Glucose        */
   /* Measurement Context characteristic.                               */
#define QAPI_BLE_GLS_MEASUREMENT_CONTEXT_FLAGS_CARBOHYDRATE_PRESENT     0x01
#define QAPI_BLE_GLS_MEASUREMENT_CONTEXT_FLAGS_MEAL_PRESENT             0x02
#define QAPI_BLE_GLS_MEASUREMENT_CONTEXT_FLAGS_TESTER_HEALTH_PRESENT    0x04
#define QAPI_BLE_GLS_MEASUREMENT_CONTEXT_FLAGS_EXERCISE_PRESENT         0x08
#define QAPI_BLE_GLS_MEASUREMENT_CONTEXT_FLAGS_MEDICATION_PRESENT       0x10
#define QAPI_BLE_GLS_MEASUREMENT_CONTEXT_FLAGS_MEDICATION_UNITS_LITERS  0x20
#define QAPI_BLE_GLS_MEASUREMENT_CONTEXT_FLAGS_HBA1C_PRESENT            0x40
#define QAPI_BLE_GLS_MEASUREMENT_CONTEXT_FLAGS_EXTENDED_FLAGS_PRESENT   0x80

   /* The following defines the valid values that may be set as the     */
   /* value for the Charbohydrate ID field of Glucose Measurement       */
   /* Context characteristic.                                           */
#define QAPI_BLE_GLS_CARBOHYDRATE_FIELD_BREAKFAST        1
#define QAPI_BLE_GLS_CARBOHYDRATE_FIELD_LUNCH            2
#define QAPI_BLE_GLS_CARBOHYDRATE_FIELD_DINNER           3
#define QAPI_BLE_GLS_CARBOHYDRATE_FIELD_SNACK            4
#define QAPI_BLE_GLS_CARBOHYDRATE_FIELD_DRINK            5
#define QAPI_BLE_GLS_CARBOHYDRATE_FIELD_SUPPER           6
#define QAPI_BLE_GLS_CARBOHYDRATE_FIELD_BRUNCH           7

   /* The following defines the valid values that may be set as the     */
   /* value for the Meal field of Glucose Measurement Context           */
   /* characteristic.                                                   */
#define QAPI_BLE_GLS_MEAL_FIELD_PREPRANDIAL              1
#define QAPI_BLE_GLS_MEAL_FIELD_POSTPRANDIAL             2
#define QAPI_BLE_GLS_MEAL_FIELD_FASTING                  3
#define QAPI_BLE_GLS_MEAL_FIELD_CASUAL                   4
#define QAPI_BLE_GLS_MEAL_FIELD_BEDTIME                  5

   /* The following defines the valid values that may be set as the     */
   /* value for the Tester field of Glucose Measurement Context         */
   /* characteristic.                                                   */
#define QAPI_BLE_GLS_TESTER_SELF                         0x01
#define QAPI_BLE_GLS_TESTER_HEALTH_CARE_PROFESSIONAL     0x02
#define QAPI_BLE_GLS_TESTER_LAB_TEST                     0x03
#define QAPI_BLE_GLS_TESTER_NOT_AVAILABLE                0x0F

   /* The following defines the valid values that may be set as the     */
   /* value for the Health field Glucose Measurement Context            */
   /* characteristic.                                                   */
#define QAPI_BLE_GLS_HEALTH_MINOR_HEALTH_ISSUES          0x01
#define QAPI_BLE_GLS_HEALTH_MAJOR_HEALTH_ISSUES          0x02
#define QAPI_BLE_GLS_HEALTH_DURING_MENSES                0x03
#define QAPI_BLE_GLS_HEALTH_UNDER_STRESS                 0x04
#define QAPI_BLE_GLS_HEALTH_NO_HEALTH_ISSUES             0x05
#define QAPI_BLE_GLS_HEALTH_NOT_AVAILABLE                0x0F

   /* The following define the valid values that may be set as the value*/
   /* for the Exersise field of Glucose Measurement Context             */
   /* characteristic.                                                   */
#define QAPI_BLE_GLS_EXERSISE_DURATION_IN_SECONDS_OVERRUN  0xFFFF

   /* The following defines the valid values that may be set as the     */
   /* value for the Medication field of Glucose Measurement Context     */
   /* characteristic.                                                   */
#define QAPI_BLE_GLS_MEDICATION_RAPID_ACTING_INSULIN         0x01
#define QAPI_BLE_GLS_MEDICATION_SHORT_ACTING_INSULIN         0x02
#define QAPI_BLE_GLS_MEDICATION_INTERMEDIATE_ACTING_INSULIN  0x03
#define QAPI_BLE_GLS_MEDICATION_LONG_ACTING_INSULIN          0x04
#define QAPI_BLE_GLS_MEDICATION_PRE_MIXED_INSULIN            0x05

   /* The following defines the valid values that may be set as the     */
   /* value for the Glucose Feature characteristic.                     */
#define QAPI_BLE_GLS_FEATURE_LOW_BATTERY_DETECTION_DURING_MEASUREMENT  0x0001
#define QAPI_BLE_GLS_FEATURE_SENSOR_MALFUNCTION_DETECTION              0x0002
#define QAPI_BLE_GLS_FEATURE_SENSOR_SAMPLE_SIZE                        0x0004
#define QAPI_BLE_GLS_FEATURE_SENSOR_STRIP_INSERTION_ERROR_DETECTION    0x0008
#define QAPI_BLE_GLS_FEATURE_SENSOR_TYPE_ERROR_DETECTION               0x0010
#define QAPI_BLE_GLS_FEATURE_SENSOR_RESULT_HIGH_LOW_DETECTION          0x0020
#define QAPI_BLE_GLS_FEATURE_SENSOR_TEMPERATURE_HIGH_LOW_DETECTION     0x0040
#define QAPI_BLE_GLS_FEATURE_SENSOR_READ_INTERRUPT_DETECTION           0x0080
#define QAPI_BLE_GLS_FEATURE_GENERAL_DEVICE_FAULT                      0x0100
#define QAPI_BLE_GLS_FEATURE_TIME_FAULT                                0x0200
#define QAPI_BLE_GLS_FEATURE_MULTIPLE_BOND_SUPPORT                     0x0400

   /* The following defines the valid values that may be set as the     */
   /* value for the OpCode field of Record Access Control Point         */
   /* characteristic.                                                   */
#define QAPI_BLE_GLS_RECORD_ACCESS_OPCODE_REPORT_STORED_RECORDS        0x01
#define QAPI_BLE_GLS_RECORD_ACCESS_OPCODE_DELETE_STORED_RECORDS        0x02
#define QAPI_BLE_GLS_RECORD_ACCESS_OPCODE_ABORT_OPERATION              0x03
#define QAPI_BLE_GLS_RECORD_ACCESS_OPCODE_REPORT_NUM_STORED_RECORDS    0x04
#define QAPI_BLE_GLS_RECORD_ACCESS_OPCODE_NUM_STORED_RECORDS_RESPONSE  0x05
#define QAPI_BLE_GLS_RECORD_ACCESS_OPCODE_RESPONSE_CODE                0x06

   /* The following defines the valid values that may be set as the     */
   /* value for the Operator field of Record Access Control Point       */
   /* characteristic.                                                   */
#define QAPI_BLE_GLS_RECORD_ACCESS_OPERATOR_NULL                      0x00
#define QAPI_BLE_GLS_RECORD_ACCESS_OPERATOR_ALL_RECORDS               0x01
#define QAPI_BLE_GLS_RECORD_ACCESS_OPERATOR_LESS_THAN_OR_EQUAL_TO     0x02
#define QAPI_BLE_GLS_RECORD_ACCESS_OPERATOR_GREATER_THAN_OR_EQUAL_TO  0x03
#define QAPI_BLE_GLS_RECORD_ACCESS_OPERATOR_WITHIN_RANGE_OF           0x04
#define QAPI_BLE_GLS_RECORD_ACCESS_OPERATOR_FIRST_RECORD              0x05
#define QAPI_BLE_GLS_RECORD_ACCESS_OPERATOR_LAST_RECORD               0x06

   /* The following defines the valid values that may be used as the    */
   /* Filter Types value of a Record Access Control Point characteristic*/
#define QAPI_BLE_GLS_RECORD_ACCESS_FILTER_TYPE_SEQUENCE_NUMBER   0x01
#define QAPI_BLE_GLS_RECORD_ACCESS_FILTER_TYPE_USER_FACING_TIME  0x02

   /* The following defines the valid values that may be used as the    */
   /* Operand value of a Record Access Control Point characteristic.    */
#define QAPI_BLE_GLS_RECORD_ACCESS_RESPONSE_CODE_SUCCESS                  0x01
#define QAPI_BLE_GLS_RECORD_ACCESS_RESPONSE_CODE_OPCODE_NOT_SUPPORTED     0x02
#define QAPI_BLE_GLS_RECORD_ACCESS_RESPONSE_CODE_INVALID_OPERATOR         0x03
#define QAPI_BLE_GLS_RECORD_ACCESS_RESPONSE_CODE_OPERATOR_NOT_SUPPORTED   0x04
#define QAPI_BLE_GLS_RECORD_ACCESS_RESPONSE_CODE_INVALID_OPERAND          0x05
#define QAPI_BLE_GLS_RECORD_ACCESS_RESPONSE_CODE_NO_RECORDS_FOUND         0x06
#define QAPI_BLE_GLS_RECORD_ACCESS_RESPONSE_CODE_ABORT_UNSUCCESSFUL       0x07
#define QAPI_BLE_GLS_RECORD_ACCESS_RESPONSE_CODE_PROCEDURE_NOT_COMPLETED  0x08
#define QAPI_BLE_GLS_RECORD_ACCESS_RESPONSE_CODE_OPERAND_NOT_SUPPORTED    0x09

   /* The following structure defines the format of a Glucose           */
   /* Measurement value that must always be specified in the Glucose    */
   /* Measurement characteristic value.                                 */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_GLS_Measurement_Header_s
{
   qapi_BLE_NonAlignedByte_t                Flags;
   qapi_BLE_NonAlignedWord_t                Sequence_Number;
   qapi_BLE_GATT_Date_Time_Characteristic_t Base_Time;
   qapi_BLE_NonAlignedByte_t                Variable_Data[1];
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_GLS_Measurement_Header_t;

#define QAPI_BLE_GLS_MEASUREMENT_HEADER_SIZE(_x)         (QAPI_BLE_BTPS_STRUCTURE_OFFSET(qapi_BLE_GLS_Measurement_Header_t, Variable_Data) + (_x))

   /* The following structure defines the format of the optional Time   */
   /* Offset field of the Glucose Mesaurement characteristic.  The Time */
   /* Offset specifies the difference in time (in minutes) between the  */
   /* local time and the time at the location where the measurement was */
   /* made.                                                             */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_GLS_Time_Offset_s
{
   qapi_BLE_NonAlignedWord_t Time_Offset;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_GLS_Time_Offset_t;

#define QAPI_BLE_GLS_TIME_OFFSET_SIZE                    (sizeof(qapi_BLE_GLS_Time_Offset_t))

   /* The following defines that values used to denote a Time Offset    */
   /* Underrun and Overrun.                                             */
#define QAPI_BLE_GLS_TIME_OFFSET_OVERRUN_VALUE           (0x7FFF)
#define QAPI_BLE_GLS_TIME_OFFSET_UNDERRUN_VALUE          (0x8000)

   /* The following structure defines the format of the optional Glucose*/
   /* Concentration field of the Glucose Mesaurement characteristic.    */
   /* The following structure contains the Concentrataion Value the Type*/
   /* and Sample Location data of the Glucose Mesaurement.  The Type and*/
   /* Sample Location data are composed of 2 nibbles of information     */
   /* packed into a byte of data, where the upper nibble represents the */
   /* Sample Location and the lower nibble represents that Type.        */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_GLS_Concentration_s
{
   qapi_BLE_NonAlignedWord_t Concentration;
   qapi_BLE_NonAlignedByte_t Type_Sample_Location;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_GLS_Concentration_t;

#define QAPI_BLE_GLS_CONCENTRATION_SIZE                  (sizeof(qapi_BLE_GLS_Concentration_t))

   /* The following structure defines the format of the optional Glucose*/
   /* Sensor Status Annunciation field of the Glucose Mesaurement       */
   /* characteristic.                                                   */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_GLS_Sensor_Status_Annunciation_s
{
   qapi_BLE_NonAlignedWord_t Sensor_Status;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_GLS_Sensor_Status_Annunciation_t;

#define QAPI_BLE_GLS_SENSOR_STATUS_ANNUNCIATION_SIZE     (sizeof(qapi_BLE_GLS_Sensor_Status_Annunciation_t))

   /* The following structure defines the format of a Glucose           */
   /* Measurement Context Header that may be set in the Glucose         */
   /* Measurement Context characteristic value.  This structure ONLY    */
   /* represnts the static values of the characteristic as there are a  */
   /* number of optional parameters.                                    */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_GLS_Glucose_Measurement_Context_Header_s
{
   qapi_BLE_NonAlignedByte_t Flags;
   qapi_BLE_NonAlignedWord_t Sequence_Number;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_GLS_Glucose_Measurement_Context_Header_t;

#define QAPI_BLE_GLS_GLUCOSE_MEASUREMENT_CONTEXT_HEADER_SIZE  (sizeof(qapi_BLE_GLS_Glucose_Measurement_Context_Header_t))

   /* The following structure defines the format of the optional Glucose*/
   /* Extended Flags field of the Glucose Context characteristic.       */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_GLS_Context_Extended_Flags_s
{
   qapi_BLE_NonAlignedByte_t Extended_Flags;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_GLS_Context_Extended_Flags_t;

#define QAPI_BLE_GLS_CONTEXT_EXTENDED_FLAGS_SIZE         (sizeof(qapi_BLE_GLS_Context_Extended_Flags_t))

   /* The following structure defines the format of the optional Glucose*/
   /* Carbohydrate field of the Glucose Context characteristic.         */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_GLS_Context_Carbohydrate_s
{
   qapi_BLE_NonAlignedByte_t Carbohydrate_ID;
   qapi_BLE_NonAlignedWord_t Carbohydrate;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_GLS_Context_Carbohydrate_t;

#define QAPI_BLE_GLS_CONTEXT_CARBOHYDRATE_SIZE           (sizeof(qapi_BLE_GLS_Context_Carbohydrate_t))

   /* The following structure defines the format of the optional Glucose*/
   /* Meal field of the Glucose Context characteristic.                 */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_GLS_Context_Meal_s
{
   qapi_BLE_NonAlignedByte_t Meal;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_GLS_Context_Meal_t;

#define QAPI_BLE_GLS_CONTEXT_MEAL_SIZE                   (sizeof(qapi_BLE_GLS_Context_Meal_t))

   /* The following structure defines the format of the optional Glucose*/
   /* Tester/Health field of the Glucose Mesaurement Context            */
   /* characteristic.  This field is composed of 2 nibbles of           */
   /* information, where the upper nibble represents the Health field   */
   /* and the lower nibble represents that Tester.                      */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_GLS_Health_Tester_s
{
   qapi_BLE_NonAlignedByte_t Health_Tester;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_GLS_Health_Tester_t;

#define QAPI_BLE_GLS_HEALTH_TESTER_SIZE                  (sizeof(qapi_BLE_GLS_Health_Tester_t))

   /* The following structure defines the format of the optional Glucose*/
   /* Exercise Duration and Intensity fields of the Glucose Context     */
   /* characteristic.  This field represents the number of seconds of   */
   /* exercise that is associated with this Glucose sample.             */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_GLS_Context_Exercise_s
{
   qapi_BLE_NonAlignedWord_t Exercise_Duration;
   qapi_BLE_NonAlignedByte_t Exercise_Intensity;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_GLS_Context_Exercise_t;

#define QAPI_BLE_GLS_CONTEXT_EXERCISE_SIZE               (sizeof(qapi_BLE_GLS_Context_Exercise_t))

   /* The following structure defines the format of the optional Glucose*/
   /* Medication field of the Glucose Context characteristic.           */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_GLS_Context_Medication_s
{
   qapi_BLE_NonAlignedByte_t Medication_ID;
   qapi_BLE_NonAlignedWord_t Medication;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_GLS_Context_Medication_t;

#define QAPI_BLE_GLS_CONTEXT_MEDICATION_SIZE             (sizeof(qapi_BLE_GLS_Context_Medication_t))

   /* The following structure defines the format of the optional Glucose*/
   /* HbA1c field of the Glucose Context characteristic.                */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_GLS_Context_HBA1C_s
{
   qapi_BLE_NonAlignedWord_t HbA1c;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_GLS_Context_HBA1C_t;

#define QAPI_BLE_GLS_CONTEXT_HBA1C_SIZE                  (sizeof(qapi_BLE_GLS_Context_HBA1C_t))

   /* The following structure defines the structure that contains two   */
   /* sequence numbers that are to be used as a starting and ending     */
   /* sequence number to defing a sequence number range.                */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_Sequence_Number_Range_s
{
   qapi_BLE_NonAlignedWord_t Starting_Sequence_Number;
   qapi_BLE_NonAlignedWord_t Ending_Sequence_Number;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_GLS_Sequence_Number_Range_t;

#define QAPI_BLE_GLS_SEQUENCE_NUMBER_RANGE_SIZE          (sizeof(qapi_BLE_GLS_Sequence_Number_Range_t))

   /* The following structure defines a structure that contains two     */
   /* Data_Time_t structures.  This structure is used to define a       */
   /* Date/Time Range.                                                  */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_GLS_Date_Time_Range_s
{
   qapi_BLE_GATT_Date_Time_Characteristic_t Minimum_Value;
   qapi_BLE_GATT_Date_Time_Characteristic_t Maximum_Value;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_GLS_Date_Time_Range_t;

#define QAPI_BLE_GLS_DATE_TIME_RANGE_SIZE                (sizeof(qapi_BLE_GLS_Date_Time_Range_t))

  /* The following structure defines the format of the                  */
  /* Record_Access_Control_Point_Response.                              */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_GLS_RACP_Response_Operand_s
{
   qapi_BLE_NonAlignedByte_t Request_Op_Code;
   qapi_BLE_NonAlignedByte_t Response_Code_Value;
}__QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_GLS_RACP_Response_Code_t;

  /* The following structure defines the format of the                  */
  /* Record_Access_Control_Point_Response.                              */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_GLS_Record_Access_Control_Point_s
{
   qapi_BLE_NonAlignedByte_t Op_Code;
   qapi_BLE_NonAlignedByte_t Operator;
   qapi_BLE_NonAlignedByte_t Variable_Data[1];
}__QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_GLS_Record_Access_Control_Point_t;

#define QAPI_BLE_GLS_RECORD_ACCESS_CONTROL_POINT_SIZE(_x)  (QAPI_BLE_BTPS_STRUCTURE_OFFSET(qapi_BLE_GLS_Record_Access_Control_Point_t, Variable_Data) + _x)

#define QAPI_BLE_GLS_STORED_RECORDS_VALUE_LENGTH         (QAPI_BLE_NON_ALIGNED_BYTE_SIZE)

   /* The following defines the GLS GATT Service Flags MASK that should */
   /* be passed into qapi_BLE_GATT_Register_Service() when the GLS      */
   /* Service is registered.                                            */
#define QAPI_BLE_GLS_SERVICE_FLAGS                       (QAPI_BLE_GATT_SERVICE_FLAGS_LE_SERVICE)

#endif
