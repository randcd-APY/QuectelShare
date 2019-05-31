/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef __QAPI_BLE_HRS_TYPES_H__
#define __QAPI_BLE_HRS_TYPES_H__

#include "./qapi_ble_bttypes.h" /* Bluetooth Type Definitions.          */

   /* The following define the defined HRS Error Codes that may be sent */
   /* in a GATT Error Response.                                         */
#define QAPI_BLE_HRS_ERROR_CODE_CONTROL_POINT_NOT_SUPPORTED  0x80

   /* The following MACRO is a utility MACRO that assigns the Heart Rate*/
   /* Service 16 bit UUID to the specified qapi_BLE_UUID_16_t variable. */
   /* This MACRO accepts one parameter which is a pointer to a          */
   /* qapi_BLE_UUID_16_t variable that is to receive the HRS UUID       */
   /* Constant value.                                                   */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_HRS_ASSIGN_HRS_SERVICE_UUID_16(_x)           QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16(*((qapi_BLE_UUID_16_t *)(_x)), 0x18, 0x0D)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined HRS Service UUID in UUID16 form.  This     */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the HRS Service UUID (MACRO returns boolean result) NOT  */
   /* less than/greater than.  The first parameter is the               */
   /* qapi_BLE_UUID_16_t variable to compare to the HRS Service UUID.   */
#define QAPI_BLE_HRS_COMPARE_HRS_SERVICE_UUID_TO_UUID_16(_x)  QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x0D)

   /* The following defines the Heart Rate Service UUID that is used    */
   /* when building the HRS Service Table.                              */
#define QAPI_BLE_HRS_SERVICE_BLUETOOTH_UUID_CONSTANT          { 0x0D, 0x18 }

   /* The following MACRO is a utility MACRO that assigns the HRS Heart */
   /* Rate Measurement Characteristic 16 bit UUID to the specified      */
   /* qapi_BLE_UUID_16_t variable.  This MACRO accepts one parameter    */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* HRS Heart Rate Measurement UUID Constant value.                   */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_HRS_ASSIGN_HEART_RATE_MEASUREMENT_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x37)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined HRS Heart Rate Measurement UUID in UUID16  */
   /* form.  This MACRO only returns whether the qapi_BLE_UUID_16_t     */
   /* variable is equal to the Heart Rate Measurement UUID (MACRO       */
   /* returns boolean result) NOT less than/greater than.  The first    */
   /* parameter is the qapi_BLE_UUID_16_t variable to compare to the HRS*/
   /* Heart Rate Measurement UUID.                                      */
#define QAPI_BLE_HRS_COMPARE_HRS_HEART_RATE_MEASUREMENT_UUID_TO_UUID_16(_x)         QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x37)

   /* The following defines the HRS Heart Rate Measurement              */
   /* Characteristic UUID that is used when building the HRS Service    */
   /* Table.                                                            */
#define QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x37, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the HRS Body  */
   /* Sensor Location Characteristic 16 bit UUID to the specified       */
   /* qapi_BLE_UUID_16_t variable.  This MACRO accepts one parameter    */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* HRS Body Sensor Location UUID Constant value.                     */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_HRS_ASSIGN_BODY_SENSOR_LOCATION_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x38)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined HRS Body Sensor Location UUID in UUID16    */
   /* form.  This MACRO only returns whether the qapi_BLE_UUID_16_t     */
   /* variable is equal to the Body Sensor Location UUID (MACRO returns */
   /* boolean result) NOT less than/greater than.  The first parameter  */
   /* is the qapi_BLE_UUID_16_t variable to compare to the HRS Body     */
   /* Sensor Location UUID.                                             */
#define QAPI_BLE_HRS_COMPARE_HRS_BODY_SENSOR_LOCATION_UUID_TO_UUID_16(_x)         QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x38)

   /* The following defines the HRS Body Sensor Location Characteristic */
   /* UUID that is used when building the HRS Service Table.            */
#define QAPI_BLE_HRS_BODY_SENSOR_LOCATION_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x38, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the HRS Heart */
   /* Rate Control Point Characteristic 16 bit UUID to the specified    */
   /* qapi_BLE_UUID_16_t variable.  This MACRO accepts one parameter    */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* HRS Heart Rate Control Point UUID Constant value.                 */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_HRS_ASSIGN_HEART_RATE_CONTROL_POINT_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x39)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined HRS Heart Rate Control Point UUID in UUID16*/
   /* form.  This MACRO only returns whether the qapi_BLE_UUID_16_t     */
   /* variable is equal to the Heart Rate Control Point UUID (MACRO     */
   /* returns boolean result) NOT less than/greater than.  The first    */
   /* parameter is the qapi_BLE_UUID_16_t variable to compare to the HRS*/
   /* Heart Rate Control Point UUID.                                    */
#define QAPI_BLE_HRS_COMPARE_HRS_HEART_RATE_CONTROL_POINT_UUID_TO_UUID_16(_x)         QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x39)

   /* The following defines the HRS Heart Rate Control Point            */
   /* Characteristic UUID that is used when building the HRS Service    */
   /* Table.                                                            */
#define QAPI_BLE_HRS_HEART_RATE_CONTROL_POINT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x39, 0x2A }

   /* The following define the valid Heart Rate Measurement Flags bit   */
   /* that may be set in the Flags field of a Heart Rate Measurement.   */
#define QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_FLAGS_HEART_RATE_IS_WORD               0x01
#define QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_FLAGS_SENSOR_CONTACT_STATUS_DETECTED   0x02
#define QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_FLAGS_SENSOR_CONTACT_STATUS_SUPPORTED  0x04
#define QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_FLAGS_ENERGY_EXPENDED_PRESENT          0x08
#define QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_FLAGS_RR_INTERVAL_PRESENT              0x10

   /* The following define the valid Body Sensor Location enumerated    */
   /* values that may be set as the value for the Body Sensor Location  */
   /* characteristic value.                                             */
#define QAPI_BLE_HRS_BODY_SENSOR_LOCATION_OTHER          0
#define QAPI_BLE_HRS_BODY_SENSOR_LOCATION_CHEST          1
#define QAPI_BLE_HRS_BODY_SENSOR_LOCATION_WRIST          2
#define QAPI_BLE_HRS_BODY_SENSOR_LOCATION_FINGER         3
#define QAPI_BLE_HRS_BODY_SENSOR_LOCATION_HAND           4
#define QAPI_BLE_HRS_BODY_SENSOR_LOCATION_EAR_LOBE       5
#define QAPI_BLE_HRS_BODY_SENSOR_LOCATION_FOOT           6

   /* The following MACRO is a utility MACRO that exists to valid that a*/
   /* specified Body Sensor Location is valid.  The only parameter to   */
   /* this function is the Body Sensor Location to valid.  This MACRO   */
   /* returns TRUE if the Body Sensor Location is valid or FALSE        */
   /* otherwise.                                                        */
#define QAPI_BLE_HRS_BODY_SENSOR_LOCATION_VALID_TEMPERATURE_TYPE(_x)  ((((uint8_t)(_x)) >= QAPI_BLE_HRS_BODY_SENSOR_LOCATION_OTHER) && (((uint8_t)(_x)) <= QAPI_BLE_HRS_BODY_SENSOR_LOCATION_FOOT))

   /* The following define the valid Heart Rate Control Point commands  */
   /* that may be written to the Heart Rate Control Point characteristic*/
   /* value.                                                            */
#define QAPI_BLE_HRS_HEART_RATE_CONTROL_POINT_RESET_ENERGY_EXPENDED  0x01

   /* The following MACRO is a utility MACRO that exists to determine if*/
   /* the value written to the Heart Rate Control Point is a valid      */
   /* command.                                                          */
#define QAPI_BLE_HRS_HEART_RATE_CONTROL_POINT_VALID_COMMAND(_x)  (((uint8_t)(_x)) == QAPI_BLE_HRS_HEART_RATE_CONTROL_POINT_RESET_ENERGY_EXPENDED)

   /* The following structure represents the header of the Heart Rate   */
   /* Measurement value that is present in every Heart Rate Measurement.*/
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_HRS_Heart_Rate_Measurement_Header_s
{
   qapi_BLE_NonAlignedByte_t Flags;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_HRS_Heart_Rate_Measurement_Header_t;

#define QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_HEADER_DATA_SIZE  (sizeof(qapi_BLE_HRS_Heart_Rate_Measurement_Header_t))

   /* The following structure represents a Generic Heart Rate           */
   /* Measurement value with the variable data that it contains.        */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_HRS_Heart_Rate_Measurement_s
{
   qapi_BLE_HRS_Heart_Rate_Measurement_Header_t Header;
   qapi_BLE_NonAlignedByte_t                    Variable_Data[1];
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_HRS_Heart_Rate_Measurement_t;

#define QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_SIZE(_x)     (QAPI_BLE_BTPS_STRUCTURE_OFFSET(qapi_BLE_HRS_Heart_Rate_Measurement_t, Variable_Data) + ((_x)* QAPI_BLE_NON_ALIGNED_BYTE_SIZE))

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* calculating the minimum size of a Heart Rate Measurement value    */
   /* based on the Heart Rate Measurement Flags.  The only parameter to */
   /* this MACRO is the Heart Rate Measurement Flags.                   */
#define QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_MINIMUM_LENGTH(_x)  (QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_HEADER_DATA_SIZE + (((_x) & QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_FLAGS_HEART_RATE_IS_WORD)?QAPI_BLE_NON_ALIGNED_WORD_SIZE:QAPI_BLE_NON_ALIGNED_BYTE_SIZE) + (((_x) & QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_FLAGS_ENERGY_EXPENDED_PRESENT)?QAPI_BLE_NON_ALIGNED_WORD_SIZE:0) + (((_x) & QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_FLAGS_RR_INTERVAL_PRESENT)?QAPI_BLE_NON_ALIGNED_WORD_SIZE:0))

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* calculating the length of a Heart Rate Measurement value based on */
   /* the Heart Rate Measurement Flags for a given number of RR         */
   /* Intervals.  The first parameter to this MACRO is the Heart Rate   */
   /* Measurement Flags.  The final parameter to this macro is the      */
   /* number of RR Intervals in the Heart Rate Measurement.             */
#define QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_LENGTH(_x, _y)  (QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_HEADER_DATA_SIZE + (((_x) & QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_FLAGS_HEART_RATE_IS_WORD)?QAPI_BLE_NON_ALIGNED_WORD_SIZE:QAPI_BLE_NON_ALIGNED_BYTE_SIZE) + (((_x) & QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_FLAGS_ENERGY_EXPENDED_PRESENT)?QAPI_BLE_NON_ALIGNED_WORD_SIZE:0) + (((_x) & QAPI_BLE_HRS_HEART_RATE_MEASUREMENT_FLAGS_RR_INTERVAL_PRESENT)?((_y)*QAPI_BLE_NON_ALIGNED_WORD_SIZE):0))

   /* The following defines the length of the Body Sensor Location      */
   /* characteristic value.                                             */
#define QAPI_BLE_HRS_BODY_SENSOR_LOCATION_VALUE_LENGTH   (QAPI_BLE_NON_ALIGNED_BYTE_SIZE)

   /* The following defines the length of the Heart Rate Control Point  */
   /* characteristic value.                                             */
#define QAPI_BLE_HRS_HEART_RATE_CONTROL_POINT_VALUE_LENGTH  (QAPI_BLE_NON_ALIGNED_BYTE_SIZE)

   /* The following defines the HRS GATT Service Flags MASK that should */
   /* be passed into qapi_BLE_GATT_Register_Service() when the HRS      */
   /* Service is registered.                                            */
#define QAPI_BLE_HRS_SERVICE_FLAGS                       (QAPI_BLE_GATT_SERVICE_FLAGS_LE_SERVICE)

#endif
