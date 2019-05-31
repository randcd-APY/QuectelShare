/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef __QAPI_BLE_TRDS_TYPES_H__
#define __QAPI_BLE_TRDS_TYPES_H__

#include "./qapi_ble_bttypes.h" /* Bluetooth Type Definitions.          */

   /* The following defines the TRDS Attribute Protocol (ATT)           */
   /* Application Error Codes that may be sent in a GATT response.      */
   /* * NOTE * The Attribute Protocol (ATT) Error Codes have the form   */
   /*          QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX found in            */
   /*          qapi_ble_atttypes.h and may also be sent in a GATT       */
   /*          response.                                                */
#define QAPI_BLE_TRDS_ERROR_CODE_SUCCESS                          0x00

   /* The following MACRO is a utility MACRO that assigns the Automation*/
   /* IO Service 16 bit UUID to the specified qapi_BLE_UUID_16_t        */
   /* variable. This MACRO accepts one parameter which is a pointer to a*/
   /* qapi_BLE_UUID_16_t variable that is to receive the TDS UUID       */
   /* Constant value.                                                   */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_TRDS_ASSIGN_TRDS_SERVICE_UUID_16(_x)             QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16(*((qapi_BLE_UUID_16_t *)(_x)), 0x18, 0x24)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined TDS Service UUID in UUID16 form. This MACRO*/
   /* only returns whether the qapi_BLE_UUID_16_t variable is equal to  */
   /* the TDS Service UUID (MACRO returns boolean result) NOT less      */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare to the TDS Service UUID.                      */
#define QAPI_BLE_TRDS_COMPARE_TRDS_SERVICE_UUID_TO_UUID_16(_x)    QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x24)

   /* The following defines the TDS UUID that is used when building the */
   /* TDS Service Table.                                                */
#define QAPI_BLE_TRDS_SERVICE_BLUETOOTH_UUID_CONSTANT             { 0x24, 0x18 }

   /* The following MACRO is a utility MACRO that assigns the TDS       */
   /* Service 16 bit UUID to the specified qapi_BLE_UUID_16_t variable. */
   /* This MACRO accepts one parameter which is a qapi_BLE_UUID_16_t    */
   /* variable that is to receive the TDS UUID Constant value.          */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_TRDS_ASSIGN_TRDS_SERVICE_SDP_UUID_16(_x)         QAPI_BLE_ASSIGN_SDP_UUID_16((_x), 0x18, 0x24)

   /* The following MACRO is a utility MACRO that assigns the TDS       */
   /* Control Point Characteristic 16 bit UUID to the specified         */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* TDS Control Point Characteristic UUID Constant value.             */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_TRDS_ASSIGN_CONTROL_POINT_CHARACTERISTIC_UUID_16(_x)           QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0xBC)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined TDS Control Point Characteristic UUID in   */
   /* UUID16 form. This MACRO only returns whether the                  */
   /* qapi_BLE_UUID_16_t variable is equal to the Control Point         */
   /* Characteristic UUID (MACRO returns boolean result) NOT less       */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare to the TDS Control Point Characteristic UUID. */
#define QAPI_BLE_TRDS_COMPARE_CONTROL_POINT_CHARACTERISTIC_UUID_TO_UUID_16(_x)  QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0xBC)

   /* The following defines the TDS Control Point Characteristic UUID   */
   /* that is used when building the TDS Service Table.                 */
#define QAPI_BLE_TRDS_CONTROL_POINT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT      { 0xBC, 0x2A }

   /* The following defines the valid value for the TDS Transport       */
   /* Discovery Data AD Type Code.                                      */
#define QAPI_BLE_TRDS_TRANSPORT_DISCOVERY_DATA_AD_TYPE_CODE       0x26

   /* The following defines the valid values that may be set for the    */
   /* Organization ID field of the Transpot Block.                      */
   /* * NOTE * Please refer to Bluetooth SIG Assigned Numbers for a     */
   /*          complete list of assigned numbers.                       */
#define QAPI_BLE_TRDS_ORGANIZATION_ID_BT_SIG                      0x01

   /* The following structure represents the TDS Transport Block.       */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_TRDS_Transport_Block_s
{
   qapi_BLE_NonAlignedByte_t Organization_ID;
   qapi_BLE_NonAlignedByte_t Flags;
   qapi_BLE_NonAlignedByte_t Transport_Data_Length;
   qapi_BLE_NonAlignedByte_t Transport_Data[1];
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_TRDS_Transport_Block_t;

#define QAPI_BLE_TRDS_TRANSPORT_BLOCK_SIZE(_x)                    (QAPI_BLE_BTPS_STRUCTURE_OFFSET(qapi_BLE_TRDS_Transport_Block_t, Transport_Data) + (_x))

   /* The following defines the valid values that may be set for the Op */
   /* Code field of the TDS Control Point.                              */
#define QAPI_BLE_TRDS_CONTROL_POINT_OP_CODE_ACTIVATE_TRANSPORT     0x01

   /* The following structure represents the TDS Control Point          */
   /* Characteristic (Request).                                         */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_TRDS_Control_Point_Request_s
{
   qapi_BLE_NonAlignedByte_t Op_Code;
   qapi_BLE_NonAlignedByte_t Organization_ID;
   qapi_BLE_NonAlignedByte_t Variable_Data[1];
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_TRDS_Control_Point_Request_t;

#define QAPI_BLE_TRDS_CONTROL_POINT_REQUEST_SIZE(_x)              (QAPI_BLE_BTPS_STRUCTURE_OFFSET(qapi_BLE_TRDS_Control_Point_Request_t, Variable_Data) + (_x))

   /* The following defines the valid values that may be set for the    */
   /* Result Code field of the TDS Control Point.                       */
#define QAPI_BLE_TRDS_CONTROL_POINT_RESULT_CODE_SUCCESS                     0x00
#define QAPI_BLE_TRDS_CONTROL_POINT_RESULT_CODE_OP_CODE_NOT_SUPPORTED       0x01
#define QAPI_BLE_TRDS_CONTROL_POINT_RESULT_CODE_INVALID_PARAMETER           0x02
#define QAPI_BLE_TRDS_CONTROL_POINT_RESULT_CODE_UNSUPPORTED_ORGANIZATION_ID 0x03
#define QAPI_BLE_TRDS_CONTROL_POINT_RESULT_CODE_OPERATION_FAILED            0x04

   /* The following structure represents the TDS Control Point          */
   /* Characteristic (Response).                                        */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_TRDS_Control_Point_Response_s
{
   qapi_BLE_NonAlignedByte_t Request_Op_Code;
   qapi_BLE_NonAlignedByte_t Result_Code;
   qapi_BLE_NonAlignedByte_t Variable_Data[1];
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_TRDS_Control_Point_Response_t;

#define QAPI_BLE_TRDS_CONTROL_POINT_RESPONSE_SIZE(_x)             (QAPI_BLE_BTPS_STRUCTURE_OFFSET(qapi_BLE_TRDS_Control_Point_Response_t, Variable_Data) + (_x))

#endif
