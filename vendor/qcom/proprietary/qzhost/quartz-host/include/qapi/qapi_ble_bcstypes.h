/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef __QAPI_BLE_BCS_TYPES_H__
#define __QAPI_BLE_BCS_TYPES_H__

#include "./qapi_ble_bttypes.h" /* Bluetooth Type Definitions.          */

   /* The following define the defined BCS Error Codes that may be sent */
   /* in a GATT Error Response.                                         */
#define QAPI_BLE_BCS_ERROR_CODE_SUCCESS                  0x00

   /* The following MACRO is a utility MACRO that assigns the BCS       */
   /* Service 16 bit UUID to the specified qapi_BLE_UUID_16_t variable. */
   /* This MACRO accepts one parameter which is a pointer to a          */
   /* qapi_BLE_UUID_16_t variable that is to receive the BCS UUID       */
   /* Constant value.                                                   */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_BCS_ASSIGN_BCS_SERVICE_UUID_16(_x)           QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16(*((qapi_BLE_UUID_16_t *)(_x)), 0x18, 0x1B)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined BCS Service UUID in UUID16 form.  This     */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the BCS Service UUID (MACRO returns boolean result) NOT  */
   /* less than/greater than.  The first parameter is the               */
   /* qapi_BLE_UUID_16_t variable to compare to the BCS Service UUID.   */
#define QAPI_BLE_BCS_COMPARE_BCS_SERVICE_UUID_TO_UUID_16(_x)  QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x1B)

   /* The following MACRO is a utility MACRO that assigns the BCS       */
   /* Service 16 bit UUID to the specified qapi_BLE_UUID_16_t variable. */
   /* This MACRO accepts one parameter which is a qapi_BLE_UUID_16_t    */
   /* variable that is to receive the BCS UUID Constant value.          */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Big-Endian format.                           */
#define QAPI_BLE_BCS_ASSIGN_BCS_SERVICE_SDP_UUID_16(_x)       QAPI_BLE_ASSIGN_SDP_UUID_16((_x), 0x18, 0x1B)

   /* The following defines the BCS Service UUID that is used when      */
   /* building the BCS Service Table.                                   */
#define QAPI_BLE_BCS_SERVICE_UUID_CONSTANT                    { 0x1B, 0x18 }

   /* The following MACRO is a utility MACRO that assigns the BCS       */
   /* Feature Characteristic 16 bit UUID to the specified               */
   /* qapi_BLE_UUID_16_t variable.  This MACRO accepts one parameter    */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* BCS feature UUID Constant value.                                  */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_BCS_ASSIGN_BC_FEATURE_UUID_16(_x)            QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x9B)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined BCS BC Feature UUID in UUID16 form.  This  */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the BCS BC Feature UUID (MACRO returns boolean result)   */
   /* NOT less than/greater than.  The first parameter is the           */
   /* qapi_BLE_UUID_16_t variable to compare to the BCS Feature UUID.   */
#define QAPI_BLE_BCS_COMPARE_BC_FEATURE_UUID_TO_UUID_16(_x)   QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x9B)

   /* The following defines the BCS BC Feature Characteristic UUID that */
   /* is used when building the BCS Service Table.                      */
#define QAPI_BLE_BCS_BC_FEATURE_CHARACTERISTIC_UUID_CONSTANT  { 0x9B, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the BCS Body  */
   /* Composition Measurement Characteristic 16 bit UUID to the         */
   /* specified qapi_BLE_UUID_16_t variable.  This MACRO accepts one    */
   /* parameter which is the qapi_BLE_UUID_16_t variable that is to     */
   /* receive the BCS Body Composition Measurement UUID Constant value. */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_BCS_ASSIGN_BC_MEASUREMENT_UUID_16(_x)            QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x9C)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined BCS Body Composition Measurement UUID in   */
   /* UUID16 form.  This MACRO only returns whether the                 */
   /* qapi_BLE_UUID_16_t variable is equal to the BCS Body Composition  */
   /* Measurement UUID (MACRO returns boolean result) NOT less          */
   /* than/greater than.  The first parameter is the qapi_BLE_UUID_16_t */
   /* variable to compare to the BCS Body Composition Measurement UUID. */
#define QAPI_BLE_BCS_COMPARE_BC_MEASUREMENT_UUID_TO_UUID_16(_x)   QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x9C)

   /* The following defines the BCS BC Feature Characteristic UUID that */
   /* is used when building the BCS Service Table.                      */
#define QAPI_BLE_BCS_BC_MEASUREMENT_CHARACTERISTIC_UUID_CONSTANT  { 0x9C, 0x2A }

   /* The following defines the values that may be used as the Flags of */
   /* a BC Feature Characteristic bitmask.                              */
   /* * NOTE * KG and LB in Macros are (kg) and (lb) units.  POINT      */
   /*          indicates a decimal placeholder.                         */
   /* * NOTE * M and INCH in Macros are (m) and (inch) units.  POINT    */
   /*          indicates a decimal placeholder.                         */
#define QAPI_BLE_BCS_BC_FEATURE_FLAG_TIME_STAMP_SUPPORTED                        0x00000001
#define QAPI_BLE_BCS_BC_FEATURE_FLAG_MULTIPLE_USERS_SUPPORTED                    0x00000002
#define QAPI_BLE_BCS_BC_FEATURE_FLAG_BASAL_METABOLISM_SUPPORTED                  0x00000004
#define QAPI_BLE_BCS_BC_FEATURE_FLAG_MUSCLE_PERCENTAGE_SUPPORTED                 0x00000008
#define QAPI_BLE_BCS_BC_FEATURE_FLAG_MUSCLE_MASS_SUPPORTED                       0x00000010
#define QAPI_BLE_BCS_BC_FEATURE_FLAG_FAT_FREE_MASS_SUPPORTED                     0x00000020
#define QAPI_BLE_BCS_BC_FEATURE_FLAG_SOFT_LEAN_MASS_SUPPORTED                    0x00000040
#define QAPI_BLE_BCS_BC_FEATURE_FLAG_BODY_WATER_MASS_SUPPORTED                   0x00000080
#define QAPI_BLE_BCS_BC_FEATURE_FLAG_IMPEDANCE_SUPPORTED                         0x00000100
#define QAPI_BLE_BCS_BC_FEATURE_FLAG_WEIGHT_SUPPORTED                            0x00000200
#define QAPI_BLE_BCS_BC_FEATURE_FLAG_HEIGHT_SUPPORTED                            0x00000400

#define QAPI_BLE_BCS_BC_FEATURE_FLAG_MM_RESOLUTION_NOT_SPECIFIED                 0x00000000
#define QAPI_BLE_BCS_BC_FEATURE_FLAG_MM_RESOLUTION_POINT_5_KG_OR_1_LB            0x00000800
#define QAPI_BLE_BCS_BC_FEATURE_FLAG_MM_RESOLUTION_POINT_2_KG_OR_POINT_5_LB      0x00001000
#define QAPI_BLE_BCS_BC_FEATURE_FLAG_MM_RESOLUTION_POINT_1_KG_OR_POINT_2_LB      0x00001800
#define QAPI_BLE_BCS_BC_FEATURE_FLAG_MM_RESOLUTION_POINT_05_KG_OR_POINT_1_LB     0x00002000
#define QAPI_BLE_BCS_BC_FEATURE_FLAG_MM_RESOLUTION_POINT_02_KG_OR_POINT_05_LB    0x00002800
#define QAPI_BLE_BCS_BC_FEATURE_FLAG_MM_RESOLUTION_POINT_01_KG_OR_POINT_02_LB    0x00004000
#define QAPI_BLE_BCS_BC_FEATURE_FLAG_MM_RESOLUTION_POINT_005_KG_OR_POINT_01_LB   0x00004800

#define QAPI_BLE_BCS_BC_FEATURE_FLAG_HM_RESOLUTION_NOT_SPECIFIED                 0x00000000
#define QAPI_BLE_BCS_BC_FEATURE_FLAG_HM_RESOLUTION_POINT_01_M_OR_1_INCH          0x00080000
#define QAPI_BLE_BCS_BC_FEATURE_FLAG_HM_RESOLUTION_POINT_005_M_OR_POINT_5_INCH   0x00100000
#define QAPI_BLE_BCS_BC_FEATURE_FLAG_HM_RESOLUTION_POINT_0001_M_OR_POINT_1_INCH  0x00180000

   /* The following defines the masks for the flags set in a BCS Body   */
   /* Composition Measurement value.                                    */
   /* * NOTE * The BCS_BC_MEASUREMENT_FLAG_MULTIPLE_PACKET_MEASUREMENT  */
   /*          flag will be set internally based on the other flags     */
   /*          below.  However, it is included here for reference.      */
#define QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_MEASUREMENT_UNITS_IMPERIAL   0x0001
#define QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_TIME_STAMP_PRESENT           0x0002
#define QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_USER_ID_PRESENT              0x0004
#define QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_BASAL_METABOLISM_PRESENT     0x0008
#define QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_MUSCLE_PERCENTAGE_PRESENT    0x0010
#define QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_MUSCLE_MASS_PRESENT          0x0020
#define QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_FAT_FREE_MASS_PRESENT        0x0040
#define QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_SOFT_LEAN_MASS_PRESENT       0x0080
#define QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_BODY_WATER_MASS_PRESENT      0x0100
#define QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_IMPEDANCE_PRESENT            0x0200
#define QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_WEIGHT_PRESENT               0x0400
#define QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_HEIGHT_PRESENT               0x0800
#define QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_MULTIPLE_PACKET_MEASUREMENT  0x1000

   /* The following defines the BC Measurement Flags for an unsuccessful*/
   /* measurement.                                                      */
#define QAPI_BLE_BCS_UNSUCCESSFUL_BC_MEASUREMENT_FLAGS   (BCS_BC_MEASUREMENT_FLAG_MEASUREMENT_UNITS_IMPERIAL |  \
                                                          BCS_BC_MEASUREMENT_FLAG_TIME_STAMP_PRESENT         |  \
                                                          BCS_BC_MEASUREMENT_FLAG_USER_ID_PRESENT)

   /* The following structure represents a Generic Body Composition     */
   /* Measurement value with the variable data that it contains.        */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_BCS_BCS_Body_Composition_Measurement_s
{
   qapi_BLE_NonAlignedWord_t   Flags;
   qapi_BLE_NonAlignedWord_t   Body_Fat_Percentage;
   qapi_BLE_NonAlignedByte_t   Variable_Data[1];
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_BCS_Body_Composition_Measurement_t;

#define QAPI_BLE_BCS_BODY_COMPOSITION_MEASUREMENT_SIZE(_x)  (QAPI_BLE_BTPS_STRUCTURE_OFFSET(qapi_BLE_BCS_Body_Composition_Measurement_t, Variable_Data) + (_x))

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* calculating the minimum size of a Body Composition Measurement    */
   /* value based on the Body Composition Measurement Flags. The only   */
   /* parameter to this MACRO is the Body Composition Measurement Flags.*/
#define QAPI_BLE_BCS_BODY_COMPOSITION_MEASUREMENT_MINIMUM_LENGTH(_x)  (QAPI_BLE_BCS_BODY_COMPOSITION_MEASUREMENT_SIZE(0)                                                                  + \
                                                                      (((_x) & QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_TIME_STAMP_PRESENT)        ? GATT_DATE_TIME_CHARACTERISTIC_DATA_SIZE : 0) + \
                                                                      (((_x) & QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_USER_ID_PRESENT)           ? QAPI_BLE_NON_ALIGNED_BYTE_SIZE          : 0) + \
                                                                      (((_x) & QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_BASAL_METABOLISM_PRESENT)  ? QAPI_BLE_NON_ALIGNED_WORD_SIZE          : 0) + \
                                                                      (((_x) & QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_MUSCLE_PERCENTAGE_PRESENT) ? QAPI_BLE_NON_ALIGNED_WORD_SIZE          : 0) + \
                                                                      (((_x) & QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_MUSCLE_MASS_PRESENT)       ? QAPI_BLE_NON_ALIGNED_WORD_SIZE          : 0) + \
                                                                      (((_x) & QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_FAT_FREE_MASS_PRESENT)     ? QAPI_BLE_NON_ALIGNED_WORD_SIZE          : 0) + \
                                                                      (((_x) & QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_SOFT_LEAN_MASS_PRESENT)    ? QAPI_BLE_NON_ALIGNED_WORD_SIZE          : 0) + \
                                                                      (((_x) & QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_BODY_WATER_MASS_PRESENT)   ? QAPI_BLE_NON_ALIGNED_WORD_SIZE          : 0) + \
                                                                      (((_x) & QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_IMPEDANCE_PRESENT)         ? QAPI_BLE_NON_ALIGNED_WORD_SIZE          : 0) + \
                                                                      (((_x) & QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_WEIGHT_PRESENT)            ? QAPI_BLE_NON_ALIGNED_WORD_SIZE          : 0) + \
                                                                      (((_x) & QAPI_BLE_BCS_BC_MEASUREMENT_FLAG_HEIGHT_PRESENT)            ? QAPI_BLE_NON_ALIGNED_WORD_SIZE          : 0))

   /* The following defines the special value for an unknown user ID    */
   /* in a BCS Body Composition Measurement value.                      */
#define QAPI_BLE_BCS_BC_UNKNOWN_USER_ID                  0xFF

   /* The following defines the special value for an unsuccessful BC    */
   /* Measurement. If this value is used, then the Time Stamp and  the  */
   /* User ID fields are the ONLY optional fields permitted in the      */
   /* measurement indication.                                           */
#define QAPI_BLE_BCS_BC_MEASUREMENT_UNSUCCESSFUL         0xFFFF


  /* The following defines the valid values (bit mask) that may be set  */
  /* for client characteristic configuration descriptors (CCCD).        */
#define QAPI_BLE_BCS_CLIENT_CHARACTERISTIC_CONFIGURATION_INDICATE_ENABLE  (QAPI_BLE_GATT_CLIENT_CONFIGURATION_CHARACTERISTIC_INDICATE_ENABLE)

   /* The following defines the BCS GATT Service Flags MASK that should */
   /* be passed into GATT_Register_Service when the BCS Service is      */
   /* registered.                                                       */
#define QAPI_BLE_BCS_SERVICE_FLAGS_LE                    (QAPI_BLE_GATT_SERVICE_FLAGS_LE_SERVICE)
#define QAPI_BLE_BCS_SERVICE_FLAGS_BR_EDR                (QAPI_BLE_GATT_SERVICE_FLAGS_BR_EDR_SERVICE)
#define QAPI_BLE_BCS_SERVICE_FLAGS_DUAL_MODE             (QAPI_BLE_GATT_SERVICE_FLAGS_LE_SERVICE | GATT_SERVICE_FLAGS_BR_EDR_SERVICE)

#endif
