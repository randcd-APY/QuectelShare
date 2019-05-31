/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef __QAPI_BLE_RSCS_TYPES_H__
#define __QAPI_BLE_RSCS_TYPES_H__

#include "./qapi_ble_bttypes.h" /* Bluetooth Type Definitions.          */

   /* The following define the defined RSCS Error Codes that may be     */
   /* sent in a GATT Error Response.                                    */
#define QAPI_BLE_RSCS_ERROR_CODE_SUCCESS                                             0x00
#define QAPI_BLE_RSCS_ERROR_CODE_PROCEDURE_ALREADY_IN_PROGRESS                       0x80
#define QAPI_BLE_RSCS_ERROR_CODE_CHARACTERISTIC_CONFIGURATION_IMPROPERLY_CONFIGURED  0x81

   /* The following MACRO is a utility MACRO that assigns the RSCS      */
   /* Service 16 bit UUID to the specified qapi_BLE_UUID_16_t variable. */
   /* This MACRO accepts one parameter which is a pointer to a          */
   /* qapi_BLE_UUID_16_t variable that is to receive the RSCS UUID      */
   /* Constant value.                                                   */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_RSCS_ASSIGN_RSCS_SERVICE_UUID_16(_x)           QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16(*((qapi_BLE_UUID_16_t *)(_x)), 0x18, 0x14)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined RSCS Service UUID in UUID16 form.  This    */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the RSCS Service UUID (MACRO returns boolean result) NOT */
   /* less than/greater than.  The first parameter is the               */
   /* qapi_BLE_UUID_16_t variable to compare to the RSCS Service UUID.  */
#define QAPI_BLE_RSCS_COMPARE_RSCS_SERVICE_UUID_TO_UUID_16(_x)  QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x14)

   /* The following defines the RSCS Service UUID that is used when     */
   /* building the RSCS Service Table.                                  */
#define QAPI_BLE_RSCS_SERVICE_UUID_CONSTANT                     { 0x14, 0x18 }

   /* The following MACRO is a utility MACRO that assigns the RSCS      */
   /* Measurement Characteristic 16 bit UUID to the specified           */
   /* qapi_BLE_UUID_16_t variable.  This MACRO accepts one parameter    */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* RSCS Measurement UUID Constant value.                             */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_RSCS_ASSIGN_RSC_MEASUREMENT_UUID_16(_x)            QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x53)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined RSCS Measurement UUID in UUID16 form.  This*/
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the RSCS Measurement UUID (MACRO returns boolean result) */
   /* NOT less than/greater than.  The first parameter is the           */
   /* qapi_BLE_UUID_16_t variable to compare to the RSCS Measurement    */
   /* UUID.                                                             */
#define QAPI_BLE_RSCS_COMPARE_RSC_MEASUREMENT_UUID_TO_UUID_16(_x)   API_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x53)

   /* The following defines the RSCS Measurement Characteristic UUID    */
   /* that is used when building the RSCS Service Table.                */
#define QAPI_BLE_RSCS_RSC_MEASUREMENT_CHARACTERISTIC_UUID_CONSTANT  { 0x53, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the RSCS      */
   /* Feature Characteristic 16 bit UUID to the specified               */
   /* qapi_BLE_UUID_16_t variable.  This MACRO accepts one parameter    */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* RSCS Feature UUID Constant value.                                 */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_RSCS_ASSIGN_RSC_FEATURE_UUID_16(_x)            QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x54)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined RSCS Feature UUID in UUID16 form.  This    */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the RSCS Feature UUID (MACRO returns boolean result) NOT */
   /* less than/greater than.  The first parameter is the               */
   /* qapi_BLE_UUID_16_t variable to compare to the RSCS Feature UUID.  */
#define QAPI_BLE_RSCS_COMPARE_RSC_FEATURE_UUID_TO_UUID_16(_x)   QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x54)

   /* The following defines the RSCS Feature Characteristic UUID        */
   /* that is used when building the RSCS Service Table.                */
#define QAPI_BLE_RSCS_RSC_FEATURE_CHARACTERISTIC_UUID_CONSTANT  { 0x54, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the Sensor    */
   /* Location Characteristic 16 bit UUID to the specified              */
   /* qapi_BLE_UUID_16_t variable.  This MACRO accepts one parameter    */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* RSCS Feature UUID Constant value.                                 */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_RSCS_ASSIGN_SENSOR_LOCATION_UUID_16(_x)            QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x5D)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined Sensor Location UUID in UUID16 form.  This */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the Sensor Location UUID (MACRO returns boolean result)  */
   /* NOT less than/greater than.  The first parameter is the           */
   /* qapi_BLE_UUID_16_t variable to compare to the RSCS Feature UUID.  */
#define QAPI_BLE_RSCS_COMPARE_SENSOR_LOCATION_UUID_TO_UUID_16(_x)   QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x5D)

   /* The following defines the Sensor Location Characteristic UUID     */
   /* that is used when building the RSCS Service Table.                */
#define QAPI_BLE_RSCS_SENSOR_LOCATION_CHARACTERISTIC_UUID_CONSTANT  { 0x5D, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the SC Control*/
   /* point Characteristic 16 bit UUID to the specified                 */
   /* qapi_BLE_UUID_16_t variable.This MACRO accepts one parameter which*/
   /* is the qapi_BLE_UUID_16_t variable that is to receive the RSCS    */
   /* Feature UUID Constant Value.                                      */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_RSCS_ASSIGN_SC_CONTROL_POINT_UUID_16(_x)            QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x55)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined SC Control Point UUID in UUID16 form.  This*/
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the SC Control Point UUID (MACRO returns boolean result) */
   /* NOT less than/greater than.  The first parameter is the           */
   /* qapi_BLE_UUID_16_t variable to compare to the SC Control Point    */
   /* UUID.                                                             */
#define QAPI_BLE_RSCS_COMPARE_SC_CONTROL_POINT_UUID_TO_UUID_16(_x)   QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x55)

   /* The following defines the SC Control Point Characteristic UUID    */
   /* that is used when building the RSCS Service Table.                */
#define QAPI_BLE_RSCS_SC_CONTROL_POINT_CHARACTERISTIC_UUID_CONSTANT  { 0x55, 0x2A }


   /* The following defines the Maximum size of any RSCS Packet that    */
   /* is sent or received.                                              */
#define QAPI_BLE_MAX_RSCS_PACKET_SIZE                    23

   /* The following defines the valid values that may be uses as the    */
   /* Flag value of a RSC Feature Characteristic                        */
#define QAPI_BLE_RSCS_FEATURE_FLAG_INSTANTANEOUS_STRIDE_LENGTH_MEASUREMENT_SUPPORTED  0x0001
#define QAPI_BLE_RSCS_FEATURE_FLAG_TOTAL_DISTANCE_MEASUREMENT_SUPPORTED               0x0002
#define QAPI_BLE_RSCS_FEATURE_FLAG_WALKING_OR_RUNNING_STATUS_SUPPORTED                0x0004
#define QAPI_BLE_RSCS_FEATURE_FLAG_CALIBRATION_PROCEDURE_SUPPORTED                    0x0008
#define QAPI_BLE_RSCS_FEATURE_FLAG_MULTIPLE_SENSOR_LOCATIONS_SUPPORTED                0x0010

   /* The following defines the valid values that may be set as the     */
   /* value for the OpCode field of SC Control Point characteristic.    */
#define QAPI_BLE_RSCS_SC_CONTROL_POINT_OPCODE_SET_CUMULATIVE_VALUE                0x01
#define QAPI_BLE_RSCS_SC_CONTROL_POINT_OPCODE_START_SENSOR_CALIBRATION            0x02
#define QAPI_BLE_RSCS_SC_CONTROL_POINT_OPCODE_UPDATE_SENSOR_LOCATION              0x03
#define QAPI_BLE_RSCS_SC_CONTROL_POINT_OPCODE_REQUEST_SUPPORTED_SENSOR_LOCATIONS  0x04
#define QAPI_BLE_RSCS_SC_CONTROL_POINT_OPCODE_RESPONSE                            0x10

   /* The following defines the valid values that may be set as the     */
   /* value for the Response Code value field of SC Control Point       */
   /* characteristic.                                                   */
#define QAPI_BLE_RSCS_SC_CONTROL_POINT_RESPONSE_CODE_SUCCESS          0x01
#define QAPI_BLE_RSCS_SC_CONTROL_POINT_RESPONSE_OPCODE_NOT_SUPPORTED  0x02
#define QAPI_BLE_RSCS_SC_CONTROL_POINT_RESPONSE_INVALID_PARAMETER     0x03
#define QAPI_BLE_RSCS_SC_CONTROL_POINT_RESPONSE_OPERATION_FAILED      0x04

   /* The following define the valid RSC Measurement Flags bit that may */
   /* be set in the Flags field of a RSC Measurement                    */
#define QAPI_BLE_RSCS_RSC_MEASUREMENT_INSTANTANEOUS_STRIDE_LENGTH_PRESENT  0x01
#define QAPI_BLE_RSCS_RSC_MEASUREMENT_TOTAL_DISTANCE_PRESENT               0x02
#define QAPI_BLE_RSCS_RSC_MEASUREMENT_STATUS_RUNNING                       0x04

   /*The following define the valid Sensor Locations that may  be used. */
#define QAPI_BLE_RSCS_SENSOR_LOCATION_OTHER              0x00
#define QAPI_BLE_RSCS_SENSOR_LOCATION_TOP_OF_SHOE        0x01
#define QAPI_BLE_RSCS_SENSOR_LOCATION_IN_SHOE            0x02
#define QAPI_BLE_RSCS_SENSOR_LOCATION_HIP                0x03
#define QAPI_BLE_RSCS_SENSOR_LOCATION_FRONT_WHEEL        0x04
#define QAPI_BLE_RSCS_SENSOR_LOCATION_LEFT_CRANK         0x05
#define QAPI_BLE_RSCS_SENSOR_LOCATION_RIGHT_CRANK        0x06
#define QAPI_BLE_RSCS_SENSOR_LOCATION_LEFT_PEDAL         0x07
#define QAPI_BLE_RSCS_SENSOR_LOCATION_RIGHT_PEDAL        0x08
#define QAPI_BLE_RSCS_SENSOR_LOCATION_FRONT_HUB          0x09
#define QAPI_BLE_RSCS_SENSOR_LOCATION_REAR_DROPOUT       0x0A
#define QAPI_BLE_RSCS_SENSOR_LOCATION_CHAINSTAY          0x0B
#define QAPI_BLE_RSCS_SENSOR_LOCATION_REAR_WHEEL         0x0C
#define QAPI_BLE_RSCS_SENSOR_LOCATION_REAR_HUB           0x0D
#define QAPI_BLE_RSCS_SENSOR_LOCATION_CHEST              0x0E

   /* The following MACRO is a utility MACRO that exists to validate    */
   /* that a specified Sensor Location is valid.  The only parameter to */
   /* this function is the Sensor Location to validate.  This MACRO     */
   /* returns TRUE if the Sensor Location is valid or FALSE otherwise.  */
#define QAPI_BLE_RSCS_SENSOR_LOCATION_VALID(_x)          ((((uint8_t)(_x)) >= QAPI_BLE_RSCS_SENSOR_LOCATION_OTHER) && (((uint8_t)(_x)) <= QAPI_BLE_RSCS_SENSOR_LOCATION_CHEST))

   /* The following structure defines the format of the RSC             */
   /* Measurement characteristic. The following structure contains the  */
   /* Flags Value                                                       */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_RSCS_RSC_Measurement_s
{
   qapi_BLE_NonAlignedByte_t Flags;
   qapi_BLE_NonAlignedWord_t Instantaneous_Speed;
   qapi_BLE_NonAlignedByte_t Instantaneous_Cadence;
   qapi_BLE_NonAlignedByte_t Variable_Data[1];
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_RSCS_RSC_Measurement_t;


#define QAPI_BLE_RSCS_RSC_MEASUREMENT_SIZE(_x)           (QAPI_BLE_BTPS_STRUCTURE_OFFSET(qapi_BLE_RSCS_RSC_Measurement_t, Variable_Data) + _x)

  /* The following MACRO is a utility MACRO that exists to aid in       */
  /* calculating the minimum size of a RSC Measurement value based on   */
  /* the RSC Measurement Flags. The only parameter to this MACRO is the */
  /* Blood Pressure Measurement Flags.                                  */
#define QAPI_BLE_RSCS_RSC_MEASUREMENT_MINIMUM_LENGTH(_x)  (QAPI_BLE_RSCS_RSC_MEASUREMENT_SIZE(0) + ( (( (_x) & QAPI_BLE_RSCS_RSC_MEASUREMENT_INSTANTANEOUS_STRIDE_LENGTH_PRESENT)?QAPI_BLE_NON_ALIGNED_WORD_SIZE :0  ) + (( (_x) & QAPI_BLE_RSCS_RSC_MEASUREMENT_TOTAL_DISTANCE_PRESENT)?QAPI_BLE_NON_ALIGNED_DWORD_SIZE :0 ) ))

  /* The following structure defines the format of the                  */
  /* SC_Control_Point_Response.                                         */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_RSCS_SC_Control_Point_s
{
   qapi_BLE_NonAlignedByte_t Op_Code;
   qapi_BLE_NonAlignedByte_t Variable_Data[1];
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_RSCS_SC_Control_Point_t;

#define QAPI_BLE_RSCS_SC_CONTROL_POINT_SIZE(_x)          (QAPI_BLE_BTPS_STRUCTURE_OFFSET(qapi_BLE_RSCS_SC_Control_Point_t, Variable_Data) + _x)

   /* The following defines the RSCS GATT Service Flags MASK that should*/
   /* be passed into qapi_BLE_GATT_Register_Service() when the RSC      */
   /* Service is registered.                                            */
#define QAPI_BLE_RSCS_SERVICE_FLAGS                      (QAPI_BLE_GATT_SERVICE_FLAGS_LE_SERVICE)

#endif
