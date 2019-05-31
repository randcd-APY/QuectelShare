/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef __QAPI_BLE_WSS_TYPES_H__
#define __QAPI_BLE_WSS_TYPES_H__

#include "./qapi_ble_bttypes.h" /* Bluetooth Type Definitions.          */

   /* The following defines the WSS Attribute Protocol (ATT) Application*/
   /* Error Codes that may be sent in a GATT response.                  */
   /* * NOTE * The Attribute Protocol (ATT) Error Codes have the form   */
   /*          QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX found in            */
   /*          qapi_ble_atttypes.h and may also be sent in a GATT       */
   /*          response.                                                */
#define QAPI_BLE_WSS_ERROR_CODE_SUCCESS                                 0x00

   /* The following MACRO is a utility MACRO that assigns the Immediate */
   /* Alert Service 16 bit UUID to the specified qapi_BLE_UUID_16_t     */
   /* variable. This MACRO accepts one parameter which is a pointer to a*/
   /* qapi_BLE_UUID_16_t variable that is to receive the WSS UUID       */
   /* Constant value.                                                   */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_WSS_ASSIGN_WSS_SERVICE_UUID_16(_x)                     QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16(*((qapi_BLE_UUID_16_t *)(_x)), 0x18, 0x1D)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined WSS Service UUID in UUID16 form. This MACRO*/
   /* only returns whether the qapi_BLE_UUID_16_t variable is equal to  */
   /* the WSS Service UUID (MACRO returns boolean result) NOT less      */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare to the WSS Service UUID.                      */
#define QAPI_BLE_WSS_COMPARE_WSS_SERVICE_UUID_TO_UUID_16(_x)            QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x1D)

   /* The following MACRO is a utility MACRO that assigns the WSS       */
   /* Service 16 bit UUID to the specified qapi_BLE_UUID_16_t variable. */
   /* This MACRO accepts one parameter which is a qapi_BLE_UUID_16_t    */
   /* variable that is to receive the WSS UUID Constant value.          */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Big-Endian format.                           */
#define QAPI_BLE_WSS_ASSIGN_WSS_SERVICE_SDP_UUID_16(_x)                 QAPI_BLE_ASSIGN_SDP_UUID_16((_x), 0x18, 0x1D)

   /* The following defines the WSS Parameter Service UUID that is      */
   /* used when building the WSS Service Table.                         */
#define QAPI_BLE_WSS_SERVICE_BLUETOOTH_UUID_CONSTANT                    { 0x1D, 0x18 }

   /* The following MACRO is a utility MACRO that assigns the WSS       */
   /* Feature Characteristic 16 bit UUID to the specified               */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* WSS Feature UUID Constant value.                                  */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_WSS_ASSIGN_WEIGHT_SCALE_FEATURE_UUID_16(_x)            QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x9E)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined WSS Feature UUID in UUID16 form. This MACRO*/
   /* only returns whether the qapi_BLE_UUID_16_t variable is equal to  */
   /* the WSS Feature UUID (MACRO returns boolean result) NOT less      */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare to the WSS Feature UUID.                      */
#define QAPI_BLE_WSS_COMPARE_WEIGHT_SCALE_FEATURE_UUID_TO_UUID_16(_x)   QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x9E)

   /* The following defines the WSS Feature Characteristic UUID that is */
   /* used when building the WSS Service Table.                         */
#define QAPI_BLE_WSS_FEATURE_CHARACTERISTIC_UUID_CONSTANT               { 0x9E, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the WSS       */
   /* Measurement Characteristic 16 bit UUID to the specified           */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* WSS Measurement UUID Constant value.                              */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_WSS_ASSIGN_WEIGHT_MEASUREMENT_UUID_16(_x)              QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x9D)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined WSS Measurement UUID in UUID16 form. This  */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the WSS Measurement UUID (MACRO returns boolean result)  */
   /* NOT less than/greater than. The first parameter is the            */
   /* qapi_BLE_UUID_16_t variable to compare to the WSS Measurement     */
   /* UUID.                                                             */
#define QAPI_BLE_WSS_COMPARE_WEIGHT_MEASUREMENT_UUID_TO_UUID_16(_x)     QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x9D)

   /* The following defines the WSS Measurement Characteristic UUID that*/
   /* is used when building the WSS Service Table.                      */
#define QAPI_BLE_WSS_WEIGHT_MEASUREMENT_CHARACTERISTIC_UUID_CONSTANT    { 0x9D, 0x2A }

   /* The following defines the valid values of WSS Weight Scale Feature*/
   /* bits that may be set in the Feature field of WSS Feature          */
   /* characteristic.                                                   */
   /* ** NOTE ** KG and LB in Macros are (kg) and (lb) units.  POINT    */
   /*            indicates a decimal placeholder.                       */
   /* ** NOTE ** M and INCH in Macros are (m) and (inch) units.  POINT  */
   /*            indicates a decimal placeholder.                       */
#define QAPI_BLE_WSS_WEIGHT_SCALE_FEATURE_FLAG_TIME_STAMP_SUPPORTED                       0x00000001
#define QAPI_BLE_WSS_WEIGHT_SCALE_FEATURE_FLAG_MULTIPLE_USERS_SUPPORTED                   0x00000002
#define QAPI_BLE_WSS_WEIGHT_SCALE_FEATURE_FLAG_BMI_SUPPORTED                              0x00000004

#define QAPI_BLE_WSS_WEIGHT_SCALE_FEATURE_FLAG_WM_RESOLUTION_NOT_SPECIFIED                0x00000000
#define QAPI_BLE_WSS_WEIGHT_SCALE_FEATURE_FLAG_WM_RESOLUTION_POINT_5_KG_OR_1_LB           0x00000080
#define QAPI_BLE_WSS_WEIGHT_SCALE_FEATURE_FLAG_WM_RESOLUTION_POINT_2_KG_OR_POINT_5_LB     0x00000100
#define QAPI_BLE_WSS_WEIGHT_SCALE_FEATURE_FLAG_WM_RESOLUTION_POINT_1_KG_OR_POINT_2_LB     0x00000180
#define QAPI_BLE_WSS_WEIGHT_SCALE_FEATURE_FLAG_WM_RESOLUTION_POINT_05_KG_OR_POINT_1_LB    0x00000200
#define QAPI_BLE_WSS_WEIGHT_SCALE_FEATURE_FLAG_WM_RESOLUTION_POINT_02_KG_OR_POINT_05_LB   0x00000280
#define QAPI_BLE_WSS_WEIGHT_SCALE_FEATURE_FLAG_WM_RESOLUTION_POINT_01_KG_OR_POINT_02_LB   0x00000400
#define QAPI_BLE_WSS_WEIGHT_SCALE_FEATURE_FLAG_WM_RESOLUTION_POINT_005_KG_OR_POINT_01_LB  0x00000480

#define QAPI_BLE_WSS_WEIGHT_SCALE_FEATURE_FLAG_HM_RESOLUTION_NOT_SPECIFIED                0x00000000
#define QAPI_BLE_WSS_WEIGHT_SCALE_FEATURE_FLAG_HM_RESOLUTION_POINT_01_M_OR_1_INCH         0x00080000
#define QAPI_BLE_WSS_WEIGHT_SCALE_FEATURE_FLAG_HM_RESOLUTION_POINT_005_M_OR_POINT_5_INCH  0x00100000
#define QAPI_BLE_WSS_WEIGHT_SCALE_FEATURE_FLAG_HM_RESOLUTION_POINT_0001_M_OR_POINT_1_INCH 0x00180000

   /* The following defines the valid WSS Measurement Flags bits that   */
   /* may be set in the Flags field of a WSS Measurement characteristic.*/
#define QAPI_BLE_WSS_WEIGHT_MEASUREMENT_FLAG_MEASUREMENT_UNITS_IMPERIAL  0x01
#define QAPI_BLE_WSS_WEIGHT_MEASUREMENT_FLAG_TIME_STAMP_PRESENT          0x02
#define QAPI_BLE_WSS_WEIGHT_MEASUREMENT_FLAG_USER_ID_PRESENT             0x04
#define QAPI_BLE_WSS_WEIGHT_MEASUREMENT_FLAG_BMI_HEIGHT_PRESENT          0x08
#define QAPI_BLE_WSS_WEIGHT_MEASUREMENT_FLAG_RESERVED                    0xF0

   /* The following structure defines the format of a WSS Measurement   */
   /* value that must always be specified in the WSS Measurement        */
   /* characteristic value.                                             */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_WSS_Weight_Measurement_s
{
   qapi_BLE_NonAlignedByte_t Flags;
   qapi_BLE_NonAlignedWord_t Weight;
   qapi_BLE_NonAlignedByte_t Variable_Data[1];
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_WSS_Weight_Measurement_t;

#define QAPI_BLE_WSS_WEIGHT_MEASUREMENT_SIZE(_x)                        (QAPI_BLE_BTPS_STRUCTURE_OFFSET(qapi_BLE_WSS_Weight_Measurement_t, Variable_Data) + (_x))

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* calculating the minimum size of a WSS Weight Measurement value    */
   /* based on the WSS Weight Measurement Flags.  The only parameter to */
   /* this MACRO is the WSS Weight Measurement Flags.                   */
   /* * NOTE * We combine the sizes for BMI and Height to get size of   */
   /*          DWORD.                                                   */
#define QAPI_BLE_WSS_WEIGHT_MEASUREMENT_MINIMUM_LENGTH(_x)             (QAPI_BLE_WSS_WEIGHT_MEASUREMENT_SIZE(0)                                                                            + \
                                                               (((_x) & QAPI_BLE_WSS_WEIGHT_MEASUREMENT_FLAG_TIME_STAMP_PRESENT)   ? QAPI_BLE_GATT_DATE_TIME_CHARACTERISTIC_DATA_SIZE : 0)  + \
                                                               (((_x) & QAPI_BLE_WSS_WEIGHT_MEASUREMENT_FLAG_USER_ID_PRESENT)      ? QAPI_BLE_NON_ALIGNED_BYTE_SIZE : 0)                    + \
                                                               (((_x) & QAPI_BLE_WSS_WEIGHT_MEASUREMENT_FLAG_BMI_HEIGHT_PRESENT)   ? QAPI_BLE_NON_ALIGNED_DWORD_SIZE : 0))

    /* The following definition is set for a WSS Measurement if it is   */
    /* unsuccessful.                                                    */
#define QAPI_BLE_WSS_WEIGHT_MEASUREMENT_UNSUCCESSFUL                    (0xFFFF)

    /* The following definition is set for a WSS User ID that is        */
    /* unknown.                                                         */
#define QAPI_BLE_WSS_USER_ID_UNKNOWN                                    (0xFF)

#endif
