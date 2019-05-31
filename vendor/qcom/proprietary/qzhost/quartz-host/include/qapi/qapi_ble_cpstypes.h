/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef __QAPI_BLE_CPS_TYPES_H__
#define __QAPI_BLE_CPS_TYPES_H__

#include "./qapi_ble_bttypes.h" /* Bluetooth Type Definitions.          */

   /* The following define the defined CPS Error Codes that may be      */
   /* sent in a GATT Error Response.                                    */
#define QAPI_BLE_CPS_ERROR_CODE_SUCCESS                              0x00
#define QAPI_BLE_CPS_ERROR_CODE_INAPPROPRIATE_CONNECTION_PARAMETERS  0x80

   /* The following MACRO is a utility MACRO that assigns the CPS       */
   /* Service 16 bit UUID to the specified qapi_BLE_UUID_16_t variable. */
   /* This MACRO accepts one parameter which is a pointer to a          */
   /* qapi_BLE_UUID_16_t variable that is to receive the CPS UUID       */
   /* Constant value.                                                   */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_CPS_ASSIGN_SERVICE_UUID_16(_x)           QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16(*((qapi_BLE_UUID_16_t *)(_x)), 0x18, 0x18)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined CPS Service UUID in UUID16 form.  This     */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the CPS Service UUID (MACRO returns boolean result) NOT  */
   /* less than/greater than.  The first parameter is the               */
   /* qapi_BLE_UUID_16_t variable to compare to the CPS Service UUID.   */
#define QAPI_BLE_CPS_COMPARE_SERVICE_UUID_TO_UUID_16(_x)  QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x18)

   /* The following defines the CPS Service UUID that is used when      */
   /* building the CPS Service Table.                                   */
#define QAPI_BLE_CPS_SERVICE_BLUETOOTH_UUID_CONSTANT      { 0x18, 0x18 }

   /* The following MACRO is a utility MACRO that assigns the Cycling   */
   /* Power Measurement Characteristic 16 bit UUID to the specified     */
   /* qapi_BLE_UUID_16_t variable.  This MACRO accepts one parameter    */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* Cycling Power Measurement UUID Constant value.                    */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_CPS_ASSIGN_MEASUREMENT_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16(*((qapi_BLE_UUID_16_t *)(_x)), 0x2A, 0x63)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined Cycling Power Measurement UUID in UUID16   */
   /* form.  This MACRO only returns whether the qapi_BLE_UUID_16_t     */
   /* variable is equal to the Cycling Power Measurement UUID (MACRO    */
   /* returns boolean result) NOT less than/greater than.  The first    */
   /* parameter is the qapi_BLE_UUID_16_t variable to compare to the CPS*/
   /* Measurement UUID.                                                 */
#define QAPI_BLE_CPS_COMPARE_MEASUREMENT_UUID_TO_UUID_16(_x)             QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x63)

   /* The following defines the Cycling Power Measurement               */
   /* Characteristic UUID that is used when building the CPS Service    */
   /* Table.                                                            */
#define QAPI_BLE_CPS_MEASUREMENT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x63, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the Cycling   */
   /* Power Vector Characteristic 16 bit UUID to the specified          */
   /* qapi_BLE_UUID_16_t variable.  This MACRO accepts one parameter    */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* Cycling Power Vector UUID Constant value.                         */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_CPS_ASSIGN_VECTOR_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x64)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined Cycling Power Vector UUID in UUID16 form.  */
   /* This MACRO only returns whether the qapi_BLE_UUID_16_t variable is*/
   /* equal to the Cycling Power Vector UUID (MACRO returns boolean     */
   /* result) NOT less than/greater than.  The first parameter is the   */
   /* qapi_BLE_UUID_16_t variable to compare to the Cycling Power Vector*/
   /* UUID.                                                             */
#define QAPI_BLE_CPS_COMPARE_VECTOR_UUID_TO_UUID_16(_x)             QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x64)

   /* The following defines the Cycling Power Vector Characteristic     */
   /* UUID that is used when building the CPS Service Table.            */
#define QAPI_BLE_CPS_VECTOR_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x64, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the Cycling   */
   /* Power Feature Characteristic 16 bit UUID to the specified         */
   /* qapi_BLE_UUID_16_t variable.  This MACRO accepts one parameter    */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* Cycling Power Feature UUID Constant value.                        */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_CPS_ASSIGN_FEATURE_UUID_16(_x)                       QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x65)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined Cycling Power Feature UUID in UUID16 form. */
   /* This MACRO only returns whether the qapi_BLE_UUID_16_t variable is*/
   /* equal to the Cycling Power Feature UUID (MACRO returns boolean    */
   /* result) NOT less than/greater than.  The first parameter is the   */
   /* qapi_BLE_UUID_16_t variable to compare to the Cycling Power       */
   /* Feature UUID.                                                     */
#define QAPI_BLE_CPS_COMPARE_FEATURE_UUID_TO_UUID_16(_x)             QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x65)

   /* The following defines the Cycling Power Feature Characteristic    */
   /* UUID that is used when building the CPS Service Table.            */
#define QAPI_BLE_CPS_FEATURE_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x65, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the Cycling   */
   /* Power Control Point Characteristic 16 bit UUID to the specified   */
   /* qapi_BLE_UUID_16_t variable.  This MACRO accepts one parameter    */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* Cycling Power Control Point UUID Constant value.                  */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_CPS_ASSIGN_CONTROL_POINT_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x66)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined Cycling Power Control Point UUID in UUID16 */
   /* form.  This MACRO only returns whether the qapi_BLE_UUID_16_t     */
   /* variable is equal to the Cycling Power Control Point UUID (MACRO  */
   /* returns boolean result) NOT less than/greater than.  The first    */
   /* parameter is the qapi_BLE_UUID_16_t variable to compare to the CP */
   /* Control Point UUID.                                               */
#define QAPI_BLE_CPS_COMPARE_CONTROL_POINT_UUID_TO_UUID_16(_x)             QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x66)

   /* The following defines the CP Control Point Characteristic UUID    */
   /* that is used when building the CPS Service Table.                 */
#define QAPI_BLE_CPS_CONTROL_POINT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x66, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the Sensor    */
   /* Location Characteristic 16 bit UUID to the specified              */
   /* qapi_BLE_UUID_16_t variable.  This MACRO accepts one parameter    */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* Sensor Location UUID Constant value.                              */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_CPS_ASSIGN_SENSOR_LOCATION_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x5D)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined Sensor Location UUID in UUID16 form.  This */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the Sensor Location UUID (MACRO returns boolean result)  */
   /* NOT less than/greater than.  The first parameter is the           */
   /* qapi_BLE_UUID_16_t variable to compare to the Sensor Location     */
   /* UUID.                                                             */
#define QAPI_BLE_CPS_COMPARE_SENSOR_LOCATION_UUID_TO_UUID_16(_x)             QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x5D)

   /* The following defines the Sensor Location Characteristic UUID     */
   /* that is used when building the CPS Service Table.                 */
#define QAPI_BLE_CPS_SENSOR_LOCATION_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x5D, 0x2A }

   /* The following defines the valid CPS Measurement Flags bits that   */
   /* may be set in the Flags field of a CPS Measurement characteristic.*/
#define QAPI_BLE_CPS_MEASUREMENT_FLAGS_PEDAL_POWER_BALANCE_PRESENT            0x0001
#define QAPI_BLE_CPS_MEASUREMENT_FLAGS_PEDAL_POWER_BALANCE_REFERENCE_LEFT     0x0002
#define QAPI_BLE_CPS_MEASUREMENT_FLAGS_ACCUMULATED_TORQUE_PRESENT             0x0004
#define QAPI_BLE_CPS_MEASUREMENT_FLAGS_ACCUMULATED_TORQUE_SOURCE_CRANK_BASED  0x0008
#define QAPI_BLE_CPS_MEASUREMENT_FLAGS_WHEEL_REVOLUTION_DATA_PRESENT          0x0010
#define QAPI_BLE_CPS_MEASUREMENT_FLAGS_CRANK_REVOLUTION_DATA_PRESENT          0x0020
#define QAPI_BLE_CPS_MEASUREMENT_FLAGS_EXTREME_FORCE_MAGNITUDES_PRESENT       0x0040
#define QAPI_BLE_CPS_MEASUREMENT_FLAGS_EXTREME_TORQUE_MAGNITUDES_PRESENT      0x0080
#define QAPI_BLE_CPS_MEASUREMENT_FLAGS_EXTREME_ANGLES_PRESENT                 0x0100
#define QAPI_BLE_CPS_MEASUREMENT_FLAGS_TOP_DEAD_SPOT_ANGLE_PRESENT            0x0200
#define QAPI_BLE_CPS_MEASUREMENT_FLAGS_BOTTOM_DEAD_SPOT_ANGLE_PRESENT         0x0400
#define QAPI_BLE_CPS_MEASUREMENT_FLAGS_ACCUMULATED_ENERGY_PRESENT             0x0800
#define QAPI_BLE_CPS_MEASUREMENT_FLAGS_OFFSET_COMPENSATION_INDICATOR          0x1000

   /* The following defines the valid values that may be set as the     */
   /* value for the CPS Feature characteristic.                         */
#define QAPI_BLE_CPS_FEATURE_PEDAL_POWER_BALANCE_SUPPORTED                         0x00000001
#define QAPI_BLE_CPS_FEATURE_ACCUMULATED_TORQUE_SUPPORTED                          0x00000002
#define QAPI_BLE_CPS_FEATURE_WHEEL_REVOLUTION_DATA_SUPPORTED                       0x00000004
#define QAPI_BLE_CPS_FEATURE_CRANK_REVOLUTION_DATA_SUPPORTED                       0x00000008
#define QAPI_BLE_CPS_FEATURE_EXTREME_MAGNITUDES_SUPPORTED                          0x00000010
#define QAPI_BLE_CPS_FEATURE_EXTREME_ANGLES_SUPPORTED                              0x00000020
#define QAPI_BLE_CPS_FEATURE_TOP_AND_BOTTOM_DEAD_SPOT_ANGLES_SUPPORTED             0x00000040
#define QAPI_BLE_CPS_FEATURE_ACCUMULATED_ENERGY_SUPPORTED                          0x00000080
#define QAPI_BLE_CPS_FEATURE_OFFSET_COMPENSATION_INDICATOR_SUPPORTED               0x00000100
#define QAPI_BLE_CPS_FEATURE_OFFSET_COMPENSATION_SUPPORTED                         0x00000200
#define QAPI_BLE_CPS_FEATURE_MEASUREMENT_CHARACTERISTIC_CONTENT_MASKING_SUPPORTED  0x00000400
#define QAPI_BLE_CPS_FEATURE_MULTIPLE_SENSOR_LOCATIONS_SUPPORTED                   0x00000800
#define QAPI_BLE_CPS_FEATURE_CRANK_LENGTH_ADJUSTMENT_SUPPORTED                     0x00001000
#define QAPI_BLE_CPS_FEATURE_CHAIN_LENGTH_ADJUSTMENT_SUPPORTED                     0x00002000
#define QAPI_BLE_CPS_FEATURE_CHAIN_WEIGHT_ADJUSTMENT_SUPPORTED                     0x00004000
#define QAPI_BLE_CPS_FEATURE_SPAN_LENGTH_ADJUSTMENT_SUPPORTED                      0x00008000
#define QAPI_BLE_CPS_FEATURE_SENSOR_MEASUREMENT_CONTEXT                            0x00010000
#define QAPI_BLE_CPS_FEATURE_INSTANTANEOUS_MEASUREMENT_DIRECTION_SUPPORTED         0x00020000
#define QAPI_BLE_CPS_FEATURE_FACTORY_CALIBRATION_DATE_SUPPORTED                    0x00040000

   /* The following defines the valid values that may be set as the     */
   /* value for the OpCode field of CP Control Point characteristic.    */
#define QAPI_BLE_CPS_CONTROL_POINT_OPCODE_SET_CUMULATIVE_VALUE                     0x01
#define QAPI_BLE_CPS_CONTROL_POINT_OPCODE_UPDATE_SENSOR_LOCATION                   0x02
#define QAPI_BLE_CPS_CONTROL_POINT_OPCODE_REQUEST_SUPPORTED_SENSOR_LOCATIONS       0x03
#define QAPI_BLE_CPS_CONTROL_POINT_OPCODE_SET_CRANK_LENGTH                         0x04
#define QAPI_BLE_CPS_CONTROL_POINT_OPCODE_REQUEST_CRANK_LENGTH                     0x05
#define QAPI_BLE_CPS_CONTROL_POINT_OPCODE_SET_CHAIN_LENGTH                         0x06
#define QAPI_BLE_CPS_CONTROL_POINT_OPCODE_REQUEST_CHAIN_LENGTH                     0x07
#define QAPI_BLE_CPS_CONTROL_POINT_OPCODE_SET_CHAIN_WEIGHT                         0x08
#define QAPI_BLE_CPS_CONTROL_POINT_OPCODE_REQUEST_CHAIN_WEIGHT                     0x09
#define QAPI_BLE_CPS_CONTROL_POINT_OPCODE_SET_SPAN_LENGTH                          0x0A
#define QAPI_BLE_CPS_CONTROL_POINT_OPCODE_REQUEST_SPAN_LENGTH                      0x0B
#define QAPI_BLE_CPS_CONTROL_POINT_OPCODE_START_OFFSET_COMPENSATION                0x0C
#define QAPI_BLE_CPS_CONTROL_POINT_OPCODE_MASK_MEASUREMENT_CHARACTERISTIC_CONTENT  0x0D
#define QAPI_BLE_CPS_CONTROL_POINT_OPCODE_REQUEST_SAMPLING_RATE                    0x0E
#define QAPI_BLE_CPS_CONTROL_POINT_OPCODE_REQUEST_FACTORY_CALIBRATION_DATE         0x0F
#define QAPI_BLE_CPS_CONTROL_POINT_OPCODE_RESPONSE_CODE                            0x20

   /* The following MACRO is a utility MACRO that exists to determine   */
   /* if the value written to the CPS Control Point is a valid command. */
#define QAPI_BLE_CPS_CONTROL_POINT_VALID_COMMAND(_x)                    ((((uint8_t)(_x)) >= QAPI_BLE_CPS_CONTROL_POINT_OPCODE_SET_CUMULATIVE_VALUE) && (((uint8_t)(_x)) <= QAPI_BLE_CPS_CONTROL_POINT_OPCODE_REQUEST_FACTORY_CALIBRATION_DATE))

   /* The following defines the valid values that may be used as the    */
   /* Response Value of CP Control Point characteristic.                */
#define QAPI_BLE_CPS_CONTROL_POINT_RESPONSE_CODE_SUCCESS               0x01
#define QAPI_BLE_CPS_CONTROL_POINT_RESPONSE_CODE_OPCODE_NOT_SUPPORTED  0x02
#define QAPI_BLE_CPS_CONTROL_POINT_RESPONSE_CODE_INVALID_PARAMETER     0x03
#define QAPI_BLE_CPS_CONTROL_POINT_RESPONSE_CODE_OPERATION_FAILED      0x04

   /* The following defines the valid Sensor Location enumerated values */
   /* that may be set as the value for the Sensor Location              */
   /* characteristic value.                                             */
#define QAPI_BLE_CPS_SENSOR_LOCATION_OTHER               0x00
#define QAPI_BLE_CPS_SENSOR_LOCATION_TOP_OF_SHOE         0x01
#define QAPI_BLE_CPS_SENSOR_LOCATION_IN_SHOE             0x02
#define QAPI_BLE_CPS_SENSOR_LOCATION_HIP                 0x03
#define QAPI_BLE_CPS_SENSOR_LOCATION_FRONT_WHEEL         0x04
#define QAPI_BLE_CPS_SENSOR_LOCATION_LEFT_CRANK          0x05
#define QAPI_BLE_CPS_SENSOR_LOCATION_RIGHT_CRANK         0x06
#define QAPI_BLE_CPS_SENSOR_LOCATION_LEFT_PEDAL          0x07
#define QAPI_BLE_CPS_SENSOR_LOCATION_RIGHT_PEDAL         0x08
#define QAPI_BLE_CPS_SENSOR_LOCATION_FRONT_HUB           0x09
#define QAPI_BLE_CPS_SENSOR_LOCATION_REAR_DROPOUT        0x0A
#define QAPI_BLE_CPS_SENSOR_LOCATION_CHAINSTAY           0x0B
#define QAPI_BLE_CPS_SENSOR_LOCATION_REAR_WHEEL          0x0C
#define QAPI_BLE_CPS_SENSOR_LOCATION_REAR_HUB            0x0D
#define QAPI_BLE_CPS_SENSOR_LOCATION_CHEST               0x0E

   /* The following MACRO is a utility MACRO that exists to validate    */
   /* that a specified Sensor Location is valid.  The only parameter to */
   /* this function is the Sensor Location to validate.  This MACRO     */
   /* returns TRUE if the Sensor Location is valid or FALSE otherwise.  */
#define QAPI_BLE_CPS_SENSOR_LOCATION_VALID(_x)           ((((uint8_t)(_x)) >= QAPI_BLE_CPS_SENSOR_LOCATION_OTHER) && (((uint8_t)(_x)) <= QAPI_BLE_CPS_SENSOR_LOCATION_CHEST))

   /* The following defines the valid Cycling Power Vector Flags bits   */
   /* that may be set in the Flags field of a Cycling Power Vector      */
   /* characteristic.                                                   */
#define QAPI_BLE_CPS_VECTOR_FLAGS_CRANK_REVOLUTION_DATA_PRESENT                             0x01
#define QAPI_BLE_CPS_VECTOR_FLAGS_FIRST_CRANK_MEASUREMENT_ANGLE_PRESENT                     0x02
#define QAPI_BLE_CPS_VECTOR_FLAGS_INSTANTANEOUS_FORCE_MAGNITUDE_ARRAY_PRESENT               0x04
#define QAPI_BLE_CPS_VECTOR_FLAGS_INSTANTANEOUS_TORQUE_MAGNITUDE_ARRAY_PRESENT              0x08
#define QAPI_BLE_CPS_VECTOR_FLAGS_INSTANTANEOUS_MEASUREMENT_DIRECTION_BITS                  0x30
#define QAPI_BLE_CPS_VECTOR_FLAGS_INSTANTANEOUS_MEASUREMENT_DIRECTION_TANGENTIAL_COMPONENT  0x10
#define QAPI_BLE_CPS_VECTOR_FLAGS_INSTANTANEOUS_MEASUREMENT_DIRECTION_RADIAL_COMPONENT      0x20
#define QAPI_BLE_CPS_VECTOR_FLAGS_INSTANTANEOUS_MEASUREMENT_DIRECTION_LATERAL_COMPONENT     0x30

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* calculating the value of Instantaneous Measurement Direction      */
   /* value enabled in Cycling Power Vector Flags.                      */
   /* The only parameter to this MACRO is Cycling Power Vector Flags.   */
#define QAPI_BLE_CPS_VECTOR_INSTANTANEOUS_MEASUREMENT_DIRECTION_VALUE(_x)  ((uint8_t)(((_x) & QAPI_BLE_CPS_VECTOR_FLAGS_INSTANTANEOUS_MEASUREMENT_DIRECTION_BITS) >> 4))

   /* The following structure defines the format of the optional Wheel  */
   /* Revolution Data field of the Cycling Power Measurement            */
   /* characteristic.  The Cumulative Wheel Revolutions field represents*/
   /* the number of times wheel was rotated.  The Last Wheel Event Time */
   /* is free-running-count of 1/2048 second units and it represents    */
   /* the time when the wheel revolution was detected by the wheel      */
   /* rotation sensor.                                                  */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_CPS_Wheel_Revolution_s
{
   qapi_BLE_NonAlignedDWord_t Cumulative_Wheel_Revolutions;
   qapi_BLE_NonAlignedWord_t  Last_Wheel_Event_Time;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_CPSWheel_Revolution_t;

#define QAPI_BLE_CPS_WHEEL_REVOLUTION_SIZE               (sizeof(qapi_BLE_CPSWheel_Revolution_t))

   /* The following structure defines the format of the optional Crank  */
   /* Revolution Data field of the Cycling Power Measurement            */
   /* characteristic.  The Cumulative Crank Revolutions field represents*/
   /* the number of times crank was rotated.  The Last Crank Event Time */
   /* is free-running-count of 1/1024 second units and it represents    */
   /* the time when the last crank revolution was detected by the crank */
   /* rotation sensor.                                                  */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_CPS_Crank_Revolution_s
{
   qapi_BLE_NonAlignedWord_t Cumulative_Crank_Revolutions;
   qapi_BLE_NonAlignedWord_t Last_Crank_Event_Time;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_CPSCrank_Revolution_t;

#define QAPI_BLE_CPS_CRANK_REVOLUTION_SIZE               (sizeof(qapi_BLE_CPSCrank_Revolution_t))

   /* The following structure defines the format of the optional        */
   /* Extreme Force Magnitudes Data field of the Cycling Power          */
   /* Measurement characteristic.  The Maximum Force Magnitude field    */
   /* represents the maximum force value measured in a single crank     */
   /* revolution; respectively the Minimum Force Magnitude field        */
   /* represents the minimum force value measured in a single crank     */
   /* revolution.                                                       */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_CPS_Extreme_Force_Magnitudes_s
{
   qapi_BLE_NonAlignedSWord_t Maximum_Force_Magnitude;
   qapi_BLE_NonAlignedSWord_t Minimum_Force_Magnitude;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_CPSExtreme_Force_Magnitudes_t;

#define QAPI_BLE_CPS_EXTREME_FORCE_MAGNITUDES_SIZE       (sizeof(qapi_BLE_CPSExtreme_Force_Magnitudes_t))

   /* The following structure defines the format of the optional        */
   /* Extreme Torque Magnitudes Data field of the Cycling Power         */
   /* Measurement characteristic.  The Maximum Torque Magnitude field   */
   /* represents the maximum torque value measured in a single crank    */
   /* revolution; respectively the Minimum Torque Magnitude field       */
   /* represents the minimum torque value measured in a single crank    */
   /* revolution.                                                       */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_CPS_Extreme_Torque_Magnitudes_s
{
   qapi_BLE_NonAlignedSWord_t Maximum_Torque_Magnitude;
   qapi_BLE_NonAlignedSWord_t Minimum_Torque_Magnitude;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_CPSExtreme_Torque_Magnitudes_t;

#define QAPI_BLE_CPS_EXTREME_TORQUE_MAGNITUDES_SIZE      (sizeof(qapi_BLE_CPSExtreme_Torque_Magnitudes_t))

   /* The following structure defines the format of the optional        */
   /* Extreme Angles Data field of the Cycling Power Measurement        */
   /* characteristic.  Byte0 field represents the first 8bit value of   */
   /* 12 bit Maximum Angle value, Byte1 field represents the first 8bit */
   /* of value of 12 bit Minimum Angle value and Byte2 is combination   */
   /* of Last 4bit of Minimum and Maximum Angle values.                 */
   /* Example :-                                                        */
   /* If N is 4 bit Nibble                                              */
   /* then 12 bit MinimumAngle Nibbles will be (N2  N1  N0) MSO to LSO  */
   /* and  12 bit MaximumAngle Nibbles will be (N5  N4  N3) MSO to LSO  */
   /* As per this convention structure values will be                   */
   /* Byte0  = N1 N0                                                    */
   /* Byte1  = N4 N3                                                    */
   /* Byte2  = N5 N2                                                    */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_CPS_Extreme_Angles_s
{
   qapi_BLE_NonAlignedByte_t Byte0;
   qapi_BLE_NonAlignedByte_t Byte1;
   qapi_BLE_NonAlignedByte_t Byte2;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_CPSExtreme_Angles_t;

#define QAPI_BLE_CPS_EXTREME_ANGLES_SIZE                 (sizeof(qapi_BLE_CPSExtreme_Angles_t))

   /* The following structure defines the format of a CPS Measurement   */
   /* value that must always be specified in the CPS Measurement        */
   /* characteristic value.                                             */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_CPS_Measurement_s
{
   qapi_BLE_NonAlignedWord_t  Flags;
   qapi_BLE_NonAlignedSWord_t Instantaneous_Power;
   qapi_BLE_NonAlignedByte_t  Variable_Data[1];
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_CPSMeasurement_t;

#define QAPI_BLE_CPS_MEASUREMENT_HEADER_SIZE             (sizeof(qapi_BLE_CPSMeasurement_t) - QAPI_BLE_NON_ALIGNED_BYTE_SIZE)
#define QAPI_BLE_CPS_MEASUREMENT_SIZE(_x)                (QAPI_BLE_CPS_MEASUREMENT_HEADER_SIZE + (_x))

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* calculating the length of a Cycling Power Measurement value based */
   /* on the Cycling Power Measurement Flags.  The only parameter to    */
   /* this MACRO is the Cycling Power Measurement Flags.                */
#define QAPI_BLE_CPS_MEASUREMENT_LENGTH(_x)              (QAPI_BLE_CPS_MEASUREMENT_SIZE(0)                                                                                                + \
                                                         (((_x) & QAPI_BLE_CPS_MEASUREMENT_FLAGS_PEDAL_POWER_BALANCE_PRESENT)       ? QAPI_BLE_NON_ALIGNED_BYTE_SIZE:0)                   + \
                                                         (((_x) & QAPI_BLE_CPS_MEASUREMENT_FLAGS_ACCUMULATED_TORQUE_PRESENT)        ? QAPI_BLE_NON_ALIGNED_WORD_SIZE:0)                   + \
                                                         (((_x) & QAPI_BLE_CPS_MEASUREMENT_FLAGS_WHEEL_REVOLUTION_DATA_PRESENT)     ? QAPI_BLE_CPS_WHEEL_REVOLUTION_SIZE:0)               + \
                                                         (((_x) & QAPI_BLE_CPS_MEASUREMENT_FLAGS_CRANK_REVOLUTION_DATA_PRESENT)     ? QAPI_BLE_CPS_CRANK_REVOLUTION_SIZE:0)               + \
                                                         (((_x) & QAPI_BLE_CPS_MEASUREMENT_FLAGS_EXTREME_FORCE_MAGNITUDES_PRESENT)  ? QAPI_BLE_CPS_EXTREME_FORCE_MAGNITUDES_SIZE :          \
                                                         (((_x) & QAPI_BLE_CPS_MEASUREMENT_FLAGS_EXTREME_TORQUE_MAGNITUDES_PRESENT) ? QAPI_BLE_CPS_EXTREME_TORQUE_MAGNITUDES_SIZE:0))     + \
                                                         (((_x) & QAPI_BLE_CPS_MEASUREMENT_FLAGS_EXTREME_ANGLES_PRESENT)            ? QAPI_BLE_CPS_EXTREME_ANGLES_SIZE:0)                 + \
                                                         (((_x) & QAPI_BLE_CPS_MEASUREMENT_FLAGS_TOP_DEAD_SPOT_ANGLE_PRESENT)       ? QAPI_BLE_NON_ALIGNED_WORD_SIZE:0)                   + \
                                                         (((_x) & QAPI_BLE_CPS_MEASUREMENT_FLAGS_BOTTOM_DEAD_SPOT_ANGLE_PRESENT)    ? QAPI_BLE_NON_ALIGNED_WORD_SIZE:0)                   + \
                                                         (((_x) & QAPI_BLE_CPS_MEASUREMENT_FLAGS_ACCUMULATED_ENERGY_PRESENT)        ? QAPI_BLE_NON_ALIGNED_WORD_SIZE:0))

   /* The following macro defines the maximum length of a Cycling Power */
   /* Measurement value assuming all fields are present.                */
#define QAPI_BLE_CPS_MAXIMUM_MEASUREMENT_LENGTH          (QAPI_BLE_CPS_MEASUREMENT_SIZE(QAPI_BLE_NON_ALIGNED_BYTE_SIZE + QAPI_BLE_NON_ALIGNED_WORD_SIZE + QAPI_BLE_CPS_WHEEL_REVOLUTION_SIZE + QAPI_BLE_CPS_CRANK_REVOLUTION_SIZE + QAPI_BLE_CPS_EXTREME_FORCE_MAGNITUDES_SIZE + QAPI_BLE_CPS_EXTREME_ANGLES_SIZE + QAPI_BLE_NON_ALIGNED_WORD_SIZE + QAPI_BLE_NON_ALIGNED_WORD_SIZE + QAPI_BLE_NON_ALIGNED_WORD_SIZE))

   /* The following structure defines the format of a Cycling Power     */
   /* Vector value that must always be specified in the Cycling Power   */
   /* Vector characteristic value.                                      */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_CPS_Vector_s
{
   qapi_BLE_NonAlignedByte_t Flags;
   qapi_BLE_NonAlignedByte_t Variable_Data[1];
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_CPSVector_t;

#define QAPI_BLE_CPS_VECTOR_SIZE(_x)                     (QAPI_BLE_BTPS_STRUCTURE_OFFSET(qapi_BLE_CPSVector_t, Variable_Data) + ((_x) * QAPI_BLE_NON_ALIGNED_BYTE_SIZE))

   /* The following MACRO is a utility MACRO that exists to aid in      */
   /* calculating the minimum length of a Cycling Power Vector data     */
   /* including optional Crank Revolution Data and First Crank          */
   /* Measurement Angle based on Cycling Power Vector Flags.            */
   /* The only parameter to this MACRO is Cycling Power Vector Flags.   */
#define QAPI_BLE_CPS_VECTOR_MINIMUM_LENGTH_INCLUDING_CRANK_DATA(_x)  (QAPI_BLE_CPS_VECTOR_SIZE(0) + (((_x) & QAPI_BLE_CPS_VECTOR_FLAGS_CRANK_REVOLUTION_DATA_PRESENT) ? QAPI_BLE_CPS_CRANK_REVOLUTION_SIZE : 0) + ((_x) & QAPI_BLE_CPS_VECTOR_FLAGS_FIRST_CRANK_MEASUREMENT_ANGLE_PRESENT ? QAPI_BLE_NON_ALIGNED_WORD_SIZE : 0))

   /* The following structure defines the format of the Control Point   */
   /* Structure.  This structure will be used for both Control Point    */
   /* request and response purpose.                                     */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_CPS_Control_Point_s
{
   qapi_BLE_NonAlignedByte_t Op_Code;
   qapi_BLE_NonAlignedByte_t Variable_Data[1];
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_CPSControl_Point_t;

#define QAPI_BLE_CPS_CONTROL_POINT_SIZE(_x)              (QAPI_BLE_BTPS_STRUCTURE_OFFSET(qapi_BLE_CPSControl_Point_t, Variable_Data) + ((_x) * QAPI_BLE_NON_ALIGNED_BYTE_SIZE))

   /* The following defines the length of the CPS Feature               */
   /* characteristic value.                                             */
#define QAPI_BLE_CPS_FEATURE_VALUE_LENGTH                (QAPI_BLE_NON_ALIGNED_DWORD_SIZE)

   /* The following defines the length of the Sensor Location           */
   /* characteristic value.                                             */
#define QAPI_BLE_CPS_SENSOR_LOCATION_VALUE_LENGTH        (QAPI_BLE_NON_ALIGNED_BYTE_SIZE)

   /* The following defines the CPS GATT Service Flags MASK that should */
   /* be passed into qapi_BLE_GATT_Register_Service() when the CPS      */
   /* Service is registered.                                            */
#define QAPI_BLE_CPS_SERVICE_FLAGS                       (QAPI_BLE_GATT_SERVICE_FLAGS_LE_SERVICE)

#endif
