/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef __QAPI_BLE_BLS_TYPES_H__
#define __QAPI_BLE_BLS_TYPES_H__

#include "./qapi_ble_bttypes.h" /* Bluetooth Type Definitions.          */

   /* The following MACRO is a utility MACRO that assigns the BLS       */
   /* Service 16 bit UUID to the specified qapi_BLE_UUID_16_t variable. */
   /* This MACRO accepts one parameter which is a pointer to a          */
   /* qapi_BLE_UUID_16_t variable that is to receive the BLS UUID       */
   /* Constant value.                                                   */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_BLS_ASSIGN_BLS_SERVICE_UUID_16(_x)           QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16(*((qapi_BLE_UUID_16_t *)(_x)), 0x18, 0x10)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined BLS Service UUID in UUID16 form.  This     */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the BLS Service UUID (MACRO returns boolean result) NOT  */
   /* less than/greater than.  The first parameter is the               */
   /* qapi_BLE_UUID_16_t variable to compare to the BLS Service UUID.   */
#define QAPI_BLE_BLS_COMPARE_BLS_SERVICE_UUID_TO_UUID_16(_x)  QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x10)

   /* The following defines the BLS Service UUID that is used when      */
   /* building the BLS Service Table.                                   */
#define QAPI_BLE_BLS_SERVICE_BLUETOOTH_UUID_CONSTANT          { 0x10, 0x18 }

   /* The following MACRO is a utility MACRO that assigns the Blood     */
   /* Pressure Measurement Characteristic 16 bit UUID to the specified  */
   /* qapi_BLE_UUID_16_t variable.  This MACRO accepts one parameter    */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* Blood Pressure Measurement UUID Constant value.                   */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_BLS_ASSIGN_BLOOD_PRESSURE_MEASUREMENT_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x35)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined Blood Pressure Measurement UUID in UUID16  */
   /* form.  This MACRO only returns whether the qapi_BLE_UUID_16_t     */
   /* variable is equal to the Blood Pressure Measurement UUID (MACRO   */
   /* returns boolean result) NOT less than/greater than.  The first    */
   /* parameter is the qapi_BLE_UUID_16_t variable to compare to the    */
   /* Blood Pressure Measurement UUID.                                  */
#define QAPI_BLE_BLS_COMPARE_BLOOD_PRESSURE_MEASUREMENT_UUID_TO_UUID_16(_x)             QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x35)

   /* The following defines the Blood Pressure Measurement              */
   /* Characteristic UUID that is used when building the BLS Service    */
   /* Table.                                                            */
#define QAPI_BLE_BLS_BLOOD_PRESSURE_MEASUREMENT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x35, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the           */
   /* Intermediate Cuff Pressure Characteristic 16 bit UUID to the      */
   /* specified qapi_BLE_UUID_16_t variable.  This MACRO accepts one    */
   /* parameter which is the qapi_BLE_UUID_16_t variable that is to     */
   /* receive the Intermediate Cuff Pressure UUID Constant value.       */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_BLS_ASSIGN_INTERMEDIATE_CUFF_PRESSURE_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x36)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined Intermediate Cuff Pressure UUID in UUID16  */
   /* form.  This MACRO only returns whether the qapi_BLE_UUID_16_t     */
   /* variable is equal to the Intermediate Cuff Pressure UUID (MACRO   */
   /* returns boolean result) NOT less than/greater than.  The first    */
   /* parameter is the qapi_BLE_UUID_16_t variable to compare to the    */
   /* Intermediate Cuff Pressure UUID.                                  */
#define QAPI_BLE_BLS_COMPARE_INTERMEDIATE_CUFF_PRESSURE_UUID_TO_UUID_16(_x)             QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x36)

   /* The following defines the Intermediate Cuff Pressure              */
   /* Characteristic UUID that is used when building the BLS Service    */
   /* Table.                                                            */
#define QAPI_BLE_BLS_INTERMEDIATE_CUFF_PRESSURE_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x36, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the Blood     */
   /* Pressure Feature Characteristic 16 bit UUID to the specified      */
   /* qapi_BLE_UUID_16_t variable.  This MACRO accepts one parameter    */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* Blood Pressure Feature UUID Constant value.                       */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_BLS_ASSIGN_BLOOD_PRESSURE_FEATURE_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x49)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined Blood Pressure Feature UUID in UUID16 form.*/
   /* This MACRO only returns whether the qapi_BLE_UUID_16_t variable is*/
   /* equal to the Blood Pressure Feature UUID (MACRO returns boolean   */
   /* result) NOT less than/greater than.  The first parameter is the   */
   /* qapi_BLE_UUID_16_t variable to compare to the Blood Pressure      */
   /* Feature UUID.                                                     */
#define QAPI_BLE_BLS_COMPARE_BLOOD_PRESSURE_FEATURE_UUID_TO_UUID_16(_x)             QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x49)

   /* The following defines the Blood Pressure Feature Characteristic   */
   /* UUID that is used when building the BLS Service Table.            */
#define QAPI_BLE_BLS_BLOOD_PRESSURE_FEATURE_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x49, 0x2A }

   /* The following defines the valid Blood Pressure Measurement Flags  */
   /* bit that may be set in the Flags field of a Blood Pressure        */
   /* Measurement characteristic.                                       */
#define QAPI_BLE_BLS_MEASUREMENT_FLAGS_BLOOD_PRESSURE_UNITS_IN_KPA  0x01
#define QAPI_BLE_BLS_MEASUREMENT_FLAGS_TIME_STAMP_PRESENT           0x02
#define QAPI_BLE_BLS_MEASUREMENT_FLAGS_PULSE_RATE_PRESENT           0x04
#define QAPI_BLE_BLS_MEASUREMENT_FLAGS_USER_ID_PRESENT              0x08
#define QAPI_BLE_BLS_MEASUREMENT_FLAGS_MEASUREMENT_STATUS_PRESENT   0x10

   /* The following defines the valid Blood Pressure Measurement  Status*/
   /* values that may be set in the Measurement Status  flag field of a */
   /* Blood Presure Measurement characteristic.                         */
#define QAPI_BLE_BLS_MEASUREMENT_STATUS_BODY_MOVEMENT_DURING_MEASUREMENT     0x0001
#define QAPI_BLE_BLS_MEASUREMENT_STATUS_CUFF_TOO_LOOSE                       0x0002
#define QAPI_BLE_BLS_MEASUREMENT_STATUS_IRREGULAR_PULSE_DETECTED             0x0004
#define QAPI_BLE_BLS_MEASUREMENT_STATUS_PULSE_RATE_EXCEEDS_UPPER_LIMIT       0x0008
#define QAPI_BLE_BLS_MEASUREMENT_STATUS_PULSE_RATE_IS_LESS_THAN_LOWER_LIMIT  0x0010
#define QAPI_BLE_BLS_MEASUREMENT_STATUS_IMPROPER_MEASUREMENT_POSITION        0x0020

   /* The following defines the valid values that may be used as the    */
   /* Flag value of a BLS Feature Characteristic                        */
#define QAPI_BLE_BLS_FEATURE_BODY_MOVEMENT_DETECTION_FEATURE_SUPPORTED         0x0001
#define QAPI_BLE_BLS_FEATURE_CUFF_FIT_DETECTION_FEATURE_SUPPORTED              0x0002
#define QAPI_BLE_BLS_FEATURE_IRREGULAR_PULSE_DETECTION_FEATURE_SUPPORTED       0x0004
#define QAPI_BLE_BLS_FEATURE_PULSE_RATE_RANGE_DETECTION_FEATURE_SUPPORTED      0x0008
#define QAPI_BLE_BLS_FEATURE_MEASUREMENT_POSITION_DETECTION_FEATURE_SUPPORTED  0x0010
#define QAPI_BLE_BLS_FEATURE_MULTIPLE_BONDS_SUPPORTED                          0x0020

   /* The following defines the structure of the Blood Pressure         */
   /* Measurement Compound Value included in the Blood Pressure         */
   /* Measurement characteristic.                                       */
   /* * NOTE * If a value for Systolic, Diastolic or MAP subfields is   */
   /*          unavailable , the special short float value NaN will be  */
   /*          filled in each of the unavailable subfields.             */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_BLS_Compound_Value_s
{
   qapi_BLE_NonAlignedWord_t Systolic;
   qapi_BLE_NonAlignedWord_t Diastolic;
   qapi_BLE_NonAlignedWord_t Mean_Arterial_Pressure;
}__QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_BLS_Compound_Value_t;

#define QAPI_BLE_BLS_COMPOUND_VALUE_SIZE                 (sizeof(qapi_BLE_BLS_Compound_Value_t))

   /* The following defines the structure of the Blood Pressure         */
   /* Measurement Data that is passed to the function that builds the   */
   /* Blood Pressure Measurement packet.                                */
   /* Variable_Data field represents none or more combination of below  */
   /* optional data fields                                              */
   /* qapi_BLE_ATT_Date_Time_Characteristic_t TimeStamp                 */
   /* qapi_BLE_NonAlignedWord_t PulseRate                               */
   /* qapi_BLE_NonAlignedByte_t UserID                                  */
   /* qapi_BLE_NonAlignedWord_t Measurement_Status                      */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_BLS_Blood_Pressure_Measurement_s
{
   qapi_BLE_NonAlignedByte_t     Flags;
   qapi_BLE_BLS_Compound_Value_t Compound_Value;
   qapi_BLE_NonAlignedByte_t     Variable_Data[1];
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_BLS_Blood_Pressure_Measurement_t;

#define QAPI_BLE_BLS_BLOOD_PRESSURE_MEASUREMENT_SIZE(_x)    (QAPI_BLE_BTPS_STRUCTURE_OFFSET(qapi_BLE_BLS_Blood_Pressure_Measurement_t, Variable_Data) + (_x))

#define QAPI_BLE_BLS_BLOOD_PRESSURE_MEASUREMENT_LENGTH(_x)  (QAPI_BLE_BLS_BLOOD_PRESSURE_MEASUREMENT_SIZE(0) + ( (( (_x) & QAPI_BLE_BLS_MEASUREMENT_FLAGS_TIME_STAMP_PRESENT) ? QAPI_BLE_GATT_DATE_TIME_CHARACTERISTIC_DATA_SIZE :0  ) + (( (_x) & QAPI_BLE_BLS_MEASUREMENT_FLAGS_PULSE_RATE_PRESENT)? QAPI_BLE_NON_ALIGNED_WORD_SIZE : 0  ) + (( (_x) & QAPI_BLE_BLS_MEASUREMENT_FLAGS_USER_ID_PRESENT)? QAPI_BLE_NON_ALIGNED_BYTE_SIZE :0  ) + (( (_x) & QAPI_BLE_BLS_MEASUREMENT_FLAGS_MEASUREMENT_STATUS_PRESENT)? QAPI_BLE_NON_ALIGNED_WORD_SIZE :0  )) )

   /* The following defines the BLS GATT Service Flags MASK that should */
   /* be passed into qapi_BLE_GATT_Register_Service when the BLS Service*/
   /* is registered.                                                    */
#define QAPI_BLE_BLS_SERVICE_FLAGS                       (QAPI_BLE_GATT_SERVICE_FLAGS_LE_SERVICE)

   /* The following defines are SFLOAT type constants.                  */
#define QAPI_BLE_BLS_SFLOAT_NOT_A_NUMBER                 (0x07FF)
#define QAPI_BLE_BLS_SFLOAT_NOT_AT_THIS_RESOLUTION       (0x0800)
#define QAPI_BLE_BLS_SFLOAT_POSITIVE_INFINITY            (0x07FE)
#define QAPI_BLE_BLS_SFLOAT_NEGATIVE_INFINITY            (0x0802)

#endif
