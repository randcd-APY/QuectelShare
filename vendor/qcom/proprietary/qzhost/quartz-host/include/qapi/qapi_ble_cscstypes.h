/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef __QAPI_BLE_CSCS_TYPES_H__
#define __QAPI_BLE_CSCS_TYPES_H__

#include "./qapi_ble_bttypes.h" /* Bluetooth Type Definitions.          */

   /* The following define the defined CSCS Error Codes that may be     */
   /* sent in a GATT Error Response.                                    */
#define QAPI_BLE_CSCS_ERROR_CODE_SUCCESS                                             0x00
#define QAPI_BLE_CSCS_ERROR_CODE_PROCEDURE_ALREADY_IN_PROGRESS                       0x80
#define QAPI_BLE_CSCS_ERROR_CODE_CHARACTERISTIC_CONFIGURATION_IMPROPERLY_CONFIGURED  0x81

   /* The following MACRO is a utility MACRO that assigns the CSCS      */
   /* Service 16 bit UUID to the specified qapi_BLE_UUID_16_t variable. */
   /* This MACRO accepts one parameter which is a pointer to a          */
   /* qapi_BLE_UUID_16_t variable that is to receive the CSCS UUID      */
   /* Constant value.                                                   */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_CSCS_ASSIGN_SERVICE_UUID_16(_x)                  QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16(*((qapi_BLE_UUID_16_t *)(_x)), 0x18, 0x16)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined CSCS Service UUID in UUID16 form. This     */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the CSCS Service UUID (MACRO returns boolean result) NOT */
   /* less than/greater than. The first parameter is the                */
   /* qapi_BLE_UUID_16_t variable to compare to the CSCS Service UUID.  */
#define QAPI_BLE_CSCS_COMPARE_SERVICE_UUID_TO_UUID_16(_x)         QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x16)

   /* The following defines the CSCS Service UUID that is used when     */
   /* building the CSCS Service Table.                                  */
#define QAPI_BLE_CSCS_SERVICE_BLUETOOTH_UUID_CONSTANT             { 0x16, 0x18 }

   /* The following MACRO is a utility MACRO that assigns the CSCS      */
   /* Measurement Characteristic 16 bit UUID to the specified           */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* CSC Measurement UUID Constant value.                              */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_CSCS_ASSIGN_CSC_MEASUREMENT_UUID_16(_x)          QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x5B)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined CSC Measurement UUID in UUID16 form. This  */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the CSC Measurement UUID (MACRO returns boolean result)  */
   /* NOT less than/greater than. The first parameter is the            */
   /* qapi_BLE_UUID_16_t variable to compare to the CSC Measurement     */
   /* UUID.                                                             */
#define QAPI_BLE_CSCS_COMPARE_CSC_MEASUREMENT_UUID_TO_UUID_16(_x) QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x5B)

   /* The following defines the CSC Measurement Characteristic UUID     */
   /* that is used when building the CSCS Service Table.                */
#define QAPI_BLE_CSCS_CSC_MEASUREMENT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x5B, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the CSCS      */
   /* Feature Characteristic 16 bit UUID to the specified               */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* CSC Feature UUID Constant value.                                  */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_CSCS_ASSIGN_CSC_FEATURE_UUID_16(_x)              QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x5C)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined CSC Feature UUID in UUID16 form. This MACRO*/
   /* only returns whether the qapi_BLE_UUID_16_t variable is equal to  */
   /* the CSC Feature UUID (MACRO returns boolean result) NOT less      */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare to the CSC Feature UUID.                      */
#define QAPI_BLE_CSCS_COMPARE_CSC_FEATURE_UUID_TO_UUID_16(_x)     QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x5C)

   /* The following defines the CSC Feature Characteristic UUID that is */
   /* used when building the CSCS Service Table.                        */
#define QAPI_BLE_CSCS_CSC_FEATURE_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT   { 0x5C, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the Sensor    */
   /* Location Characteristic 16 bit UUID to the specified              */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* Sensor Location UUID Constant value.                              */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_CSCS_ASSIGN_SENSOR_LOCATION_UUID_16(_x)          QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x5D)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined Sensor Location UUID in UUID16 form. This  */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the Sensor Location UUID (MACRO returns boolean result)  */
   /* NOT less than/greater than. The first parameter is the            */
   /* qapi_BLE_UUID_16_t variable to compare to the Sensor Location     */
   /* UUID.                                                             */
#define QAPI_BLE_CSCS_COMPARE_SENSOR_LOCATION_UUID_TO_UUID_16(_x) QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x5D)

   /* The following defines the Sensor Location Characteristic UUID     */
   /* that is used when building the CSCS Service Table.                */
#define QAPI_BLE_CSCS_SENSOR_LOCATION_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x5D, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the SC Control*/
   /* Point Characteristic 16 bit UUID to the specified                 */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the SC*/
   /* Control Point UUID Constant value.                                */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_CSCS_ASSIGN_SC_CONTROL_POINT_UUID_16(_x)         QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x55)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined SC Control Point UUID in UUID16 form. This */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the SC Control Point UUID (MACRO returns boolean result) */
   /* NOT less than/greater than. The first parameter is the            */
   /* qapi_BLE_UUID_16_t variable to compare to the SC Control Point    */
   /* UUID.                                                             */
#define QAPI_BLE_CSCS_COMPARE_SC_CONTROL_POINT_UUID_TO_UUID_16(_x) QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x55)

   /* The following defines the SC Control Point Characteristic UUID    */
   /* that is used when building the CSCS Service Table.                */
#define QAPI_BLE_CSCS_SC_CONTROL_POINT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT { 0x55, 0x2A }

   /* The following defines the valid CSC Measurement Flags bits that   */
   /* may be set in the Flags field of a CSC Measurement characteristic.*/
#define QAPI_BLE_CSCS_CSC_MEASUREMENT_FLAGS_WHEEL_REVOLUTION_DATA_PRESENT        0x01
#define QAPI_BLE_CSCS_CSC_MEASUREMENT_FLAGS_CRANK_REVOLUTION_DATA_PRESENT        0x02

   /* The following defines the valid values that may be set as the     */
   /* value for the CSC Feature characteristic.                         */
#define QAPI_BLE_CSCS_CSC_FEATURE_WHEEL_REVOLUTION_DATA_SUPPORTED              0x0001
#define QAPI_BLE_CSCS_CSC_FEATURE_CRANK_REVOLUTION_DATA_SUPPORTED              0x0002
#define QAPI_BLE_CSCS_CSC_FEATURE_MULTIPLE_SENSOR_LOCATIONS_SUPPORTED          0x0004

   /* The following define the valid Sensor Location enumerated values  */
   /* that may be set as the value for the Sensor Location              */
   /* characteristic value.                                             */
#define QAPI_BLE_CSCS_SENSOR_LOCATION_OTHER                                      0x00
#define QAPI_BLE_CSCS_SENSOR_LOCATION_TOP_OF_SHOE                                0x01
#define QAPI_BLE_CSCS_SENSOR_LOCATION_IN_SHOE                                    0x02
#define QAPI_BLE_CSCS_SENSOR_LOCATION_HIP                                        0x03
#define QAPI_BLE_CSCS_SENSOR_LOCATION_FRONT_WHEEL                                0x04
#define QAPI_BLE_CSCS_SENSOR_LOCATION_LEFT_CRANK                                 0x05
#define QAPI_BLE_CSCS_SENSOR_LOCATION_RIGHT_CRANK                                0x06
#define QAPI_BLE_CSCS_SENSOR_LOCATION_LEFT_PEDAL                                 0x07
#define QAPI_BLE_CSCS_SENSOR_LOCATION_RIGHT_PEDAL                                0x08
#define QAPI_BLE_CSCS_SENSOR_LOCATION_FRONT_HUB                                  0x09
#define QAPI_BLE_CSCS_SENSOR_LOCATION_REAR_DROPOUT                               0x0A
#define QAPI_BLE_CSCS_SENSOR_LOCATION_CHAINSTAY                                  0x0B
#define QAPI_BLE_CSCS_SENSOR_LOCATION_REAR_WHEEL                                 0x0C
#define QAPI_BLE_CSCS_SENSOR_LOCATION_REAR_HUB                                   0x0D
#define QAPI_BLE_CSCS_SENSOR_LOCATION_CHEST                                      0x0E

   /* The following MACRO is a utility MACRO that exists to validate    */
   /* that a specified Sensor Location is valid.  The only parameter to */
   /* this function is the Sensor Location to validate.  This MACRO     */
   /* returns TRUE if the Sensor Location is valid or FALSE otherwise.  */
#define QAPI_BLE_CSCS_SENSOR_LOCATION_VALID(_x)          ((((uint8_t)(_x)) >= QAPI_BLE_CSCS_SENSOR_LOCATION_OTHER) && (((uint8_t)(_x)) <= QAPI_BLE_CSCS_SENSOR_LOCATION_CHEST))

   /* The following defines the valid values that may be set as the     */
   /* value for the OpCode field of SC Control Point characteristic.    */
#define QAPI_BLE_CSCS_SC_CONTROL_POINT_OPCODE_SET_CUMULATIVE_VALUE               0x01
#define QAPI_BLE_CSCS_SC_CONTROL_POINT_OPCODE_START_SENSOR_CALIBRATION           0x02
#define QAPI_BLE_CSCS_SC_CONTROL_POINT_OPCODE_UPDATE_SENSOR_LOCATION             0x03
#define QAPI_BLE_CSCS_SC_CONTROL_POINT_OPCODE_REQUEST_SUPPORTED_SENSOR_LOCATIONS 0x04
#define QAPI_BLE_CSCS_SC_CONTROL_POINT_OPCODE_RESPONSE_CODE                      0x10

   /* The following MACRO is a utility MACRO that exists to determine if*/
   /* the value written to the SC Control Point is a valid command.     */
#define QAPI_BLE_CSCS_SC_CONTROL_POINT_VALID_COMMAND(_x) ((((uint8_t)(_x)) >= QAPI_BLE_CSCS_SC_CONTROL_POINT_OPCODE_SET_CUMULATIVE_VALUE) && (((uint8_t)(_x)) <= QAPI_BLE_CSCS_SC_CONTROL_POINT_OPCODE_REQUEST_SUPPORTED_SENSOR_LOCATIONS))

   /* The following defines the valid values that may be used as the    */
   /* Response Value of SC Control Point characteristic.                */
#define QAPI_BLE_CSCS_SC_CONTROL_POINT_RESPONSE_CODE_SUCCESS                     0x01
#define QAPI_BLE_CSCS_SC_CONTROL_POINT_RESPONSE_CODE_OPCODE_NOT_SUPPORTED        0x02
#define QAPI_BLE_CSCS_SC_CONTROL_POINT_RESPONSE_CODE_INVALID_PARAMETER           0x03
#define QAPI_BLE_CSCS_SC_CONTROL_POINT_RESPONSE_CODE_OPERATION_FAILED            0x04

   /* The following MACRO is a utility MACRO that exists to determine if*/
   /* the value written to the SC Control Point is a valid command.     */
#define QAPI_BLE_CSCS_SC_CONTROL_POINT_VALID_COMMAND(_x) ((((uint8_t)(_x)) >= QAPI_BLE_CSCS_SC_CONTROL_POINT_OPCODE_SET_CUMULATIVE_VALUE) && (((uint8_t)(_x)) <= QAPI_BLE_CSCS_SC_CONTROL_POINT_OPCODE_REQUEST_SUPPORTED_SENSOR_LOCATIONS))

   /* The following structure defines the format of a CSC Measurement   */
   /* value that must always be specified in the CSC Measurement        */
   /* characteristic value. Variable_Data field represents none or      */
   /* combination of below optional data fields Optional Fields 1       */
   /* qapi_BLE_NonAlignedDWord_t Cumulative_Wheel_Revolutions           */
   /* qapi_BLE_NonAlignedWord_t Last_Wheel_Event_Time Optional Fields 2 */
   /* qapi_BLE_NonAlignedWord_t Cumulative_Crank_Revolutions            */
   /* qapi_BLE_NonAlignedWord_t Last_Crank_Event_Time                   */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_CSCS_CSC_Measurement_s
{
   qapi_BLE_NonAlignedByte_t Flags;
   qapi_BLE_NonAlignedByte_t Variable_Data[1];
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_CSCS_CSC_Measurement_t;

#define QAPI_BLE_CSCS_CSC_MEASUREMENT_SIZE(_x)            (QAPI_BLE_BTPS_STRUCTURE_OFFSET(qapi_BLE_CSCS_CSC_Measurement_t, Variable_Data) + (_x))

   /* The following structure defines the format of the optional Wheel  */
   /* Revolution Data field of the CSC Mesaurement characteristic.  The */
   /* Cumulative Wheel Revolutions field represents the number of times */
   /* wheel was rotated. The Las Wheel Event Time is free-running-count */
   /* of 1/1024 second units and it represents the time when the last   */
   /* wheel revolution was detected by the wheel rotation sensor.       */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_CSCS_Wheel_Revolution_s
{
   qapi_BLE_NonAlignedDWord_t Cumulative_Wheel_Revolutions;
   qapi_BLE_NonAlignedWord_t  Last_Wheel_Event_Time;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_CSCS_Wheel_Revolution_t;

#define QAPI_BLE_CSCS_WHEEL_REVOLUTION_SIZE          (sizeof(qapi_BLE_CSCS_Wheel_Revolution_t))

   /* The following structure defines the format of the optional Crank  */
   /* Revolution Data field of the CSC Mesaurement characteristic.  The */
   /* Cumulative Crank Revolutions field represents the number of times */
   /* crank was rotated. The Las Crank Event Time is free-running-count */
   /* of 1/1024 second units and it represents the time when the last   */
   /* crank revolution was detected by the crank rotation sensor.       */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_CSCS_Crank_Revolution_s
{
   qapi_BLE_NonAlignedWord_t Cumulative_Crank_Revolutions;
   qapi_BLE_NonAlignedWord_t Last_Crank_Event_Time;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_CSCS_Crank_Revolution_t;

#define QAPI_BLE_CSCS_CRANK_REVOLUTION_SIZE          (sizeof(qapi_BLE_CSCS_Crank_Revolution_t))

   /* The following structure defines the Response Code Value of the    */
   /* qapi_BLE_CSCS_SC_Control_Point_s response data.                   */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_CSCS_SCCP_Response_Code_s
{
   qapi_BLE_NonAlignedByte_t Request_Op_Code;
   qapi_BLE_NonAlignedByte_t Response_Code_Value;
}__QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_CSCS_SCCP_Response_Code_t;

#define QAPI_BLE_CSCS_SCCP_RESPONSE_CODE_SIZE         (sizeof(qapi_BLE_CSCS_SCCP_Response_Code_t))

  /* The following structure defines the format of the SC Control Point */
  /* Structure. This structure will be used for both SC Control Point   */
  /* request and response purpose                                       */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_CSCS_SC_Control_Point_s
{
   qapi_BLE_NonAlignedByte_t Op_Code;
   qapi_BLE_NonAlignedByte_t Variable_Data[1];
}__QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_CSCS_SC_Control_Point_t;

#define QAPI_BLE_CSCS_SC_CONTROL_POINT_SIZE(_x)       (QAPI_BLE_BTPS_STRUCTURE_OFFSET(qapi_BLE_CSCS_SC_Control_Point_t, Variable_Data) + _x)

   /* The following defines the length of the CSC Feature               */
   /* characteristic value.                                             */
#define QAPI_BLE_CSCS_CSC_FEATURE_VALUE_LENGTH        (QAPI_BLE_NON_ALIGNED_WORD_SIZE)

   /* The following defines the length of the Sensor Location           */
   /* characteristic value.                                             */
#define QAPI_BLE_CSCS_SENSOR_LOCATION_VALUE_LENGTH    (QAPI_BLE_NON_ALIGNED_BYTE_SIZE)

#endif
