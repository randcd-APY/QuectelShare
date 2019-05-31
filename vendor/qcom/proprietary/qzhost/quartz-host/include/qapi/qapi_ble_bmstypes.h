/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef __QAPI_BLE_BMS_TYPES_H__
#define __QAPI_BLE_BMS_TYPES_H__

#include "./qapi_ble_bttypes.h" /* Bluetooth Type Definitions.          */

   /* The following defines the attribute protocol error codes.  These  */
   /* error codes may be set for the ResponseCode field of the          */
   /* qapi_BLE_BMS_BM_Control_Point_Response() function.                */
#define QAPI_BLE_BMS_ERROR_CODE_SUCCESS                  0x00
#define QAPI_BLE_BMS_ERROR_CODE_OPCODE_NOT_SUPPORTED     0x80
#define QAPI_BLE_BMS_ERROR_CODE_OPERATION_FAILED         0x81

   /* The following MACRO is a utility MACRO that assigns the BMS       */
   /* Service 16 bit UUID to the specified qapi_BLE_UUID_16_t variable. */
   /* This MACRO accepts one parameter which is a pointer to a          */
   /* qapi_BLE_UUID_16_t variable that is to receive the BMS UUID       */
   /* Constant value.                                                   */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_BMS_ASSIGN_BMS_SERVICE_UUID_16(_x)           QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16(*((qapi_BLE_UUID_16_t *)(_x)), 0x18, 0x1E)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined BMS Service UUID in UUID16 form.  This     */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the BMS Service UUID (MACRO returns boolean result) NOT  */
   /* less than/greater than.  The first parameter is the               */
   /* qapi_BLE_UUID_16_t variable to compare to the BMS Service UUID.   */
#define QAPI_BLE_BMS_COMPARE_BMS_SERVICE_UUID_TO_UUID_16(_x)  QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x1E)

   /* The following defines the BMS Service UUID that is used when      */
   /* building the BMS Service Table.                                   */
#define QAPI_BLE_BMS_SERVICE_UUID_CONSTANT                    { 0x1E, 0x18 }

   /* The following MACRO is a utility MACRO that assigns the BMS       */
   /* Service 16 bit UUID to the specified qapi_BLE_UUID_16_t variable. */
   /* This MACRO accepts one parameter which is a qapi_BLE_UUID_16_t    */
   /* variable that is to receive the BMS UUID Constant value.          */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Big-Endian format.                           */
#define QAPI_BLE_BMS_ASSIGN_BMS_SERVICE_SDP_UUID_16(_x)       ASSIGN_SDP_UUID_16((_x), 0x18, 0x1E)

   /* The following MACRO is a utility MACRO that assigns the BM Control*/
   /* point Characteristic 16 bit UUID to the specified                 */
   /* qapi_BLE_UUID_16_t variable.  This MACRO accepts one parameter    */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* BMS BM CONTROL POINT UUID Constant Value.                         */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_BMS_ASSIGN_BM_CONTROL_POINT_UUID_16(_x)            QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0xA4)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined BM Control Point UUID in UUID16 form.  This*/
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the BM Control Point UUID (MACRO returns boolean result) */
   /* NOT less than/greater than.  The first parameter is the           */
   /* qapi_BLE_UUID_16_t variable to compare to the BM Control Point    */
   /* UUID.                                                             */
#define QAPI_BLE_BMS_COMPARE_BM_CONTROL_POINT_UUID_TO_UUID_16(_x)   QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0xA4)

   /* The following defines the BM Control Point Characteristic UUID    */
   /* that is used when building the BMS Service Table.                 */
#define QAPI_BLE_BMS_BM_CONTROL_POINT_CHARACTERISTIC_UUID_CONSTANT  { 0xA4, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the BMS       */
   /* Feature Characteristic 16 bit UUID to the specified               */
   /* qapi_BLE_UUID_16_t variable.  This MACRO accepts one parameter    */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* BMS feature UUID Constant value.                                  */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_BMS_ASSIGN_BM_FEATURE_UUID_16(_x)            QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0xA5)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined BMS BM Feature UUID in UUID16 form.  This  */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the BMS BM Feature UUID (MACRO returns boolean result)   */
   /* NOT less than/greater than.  The first parameter is the           */
   /* qapi_BLE_UUID_16_t variable to compare to the BMS Feature UUID.   */
#define QAPI_BLE_BMS_COMPARE_BM_FEATURE_UUID_TO_UUID_16(_x)   QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0xA5)

   /* The following defines the BMS BM Feature Characteristic UUID that */
   /* is used when building the BMS Service Table.                      */
#define QAPI_BLE_BMS_BM_FEATURE_CHARACTERISTIC_UUID_CONSTANT  { 0xA5, 0x2A }

   /* Defines the maximum length of a BMS Authorization Code per BMS    */
   /* Bluetooth service specification.                                  */
#define QAPI_BLE_BMS_MAXIMUM_AUTHORIZATION_CODE_LENGTH   511

   /* The following defines the values that may be used as the Flags of */
   /* a BM Feature Characteristic bitmask.                              */
#define QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_REQUESTING_DEVICE_BREDR_LE       0x00000001
#define QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_REQUESTING_DEVICE_BREDR_LE_AUTH  0x00000002
#define QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_REQUESTING_DEVICE_BREDR          0x00000004
#define QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_REQUESTING_DEVICE_BREDR_AUTH     0x00000008
#define QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_REQUESTING_DEVICE_LE             0x00000010
#define QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_REQUESTING_DEVICE_LE_AUTH        0x00000020
#define QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_ALL_DEVICES_BREDR_LE             0x00000040
#define QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_ALL_DEVICES_BREDR_LE_AUTH        0x00000080
#define QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_ALL_DEVICES_BREDR                0x00000100
#define QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_ALL_DEVICES_BREDR_AUTH           0x00000200
#define QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_ALL_DEVICES_LE                   0x00000400
#define QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_ALL_DEVICES_LE_AUTH              0x00000800
#define QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_OTHER_DEVICES_BREDR_LE           0x00001000
#define QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_OTHER_DEVICES_BREDR_LE_AUTH      0x00002000
#define QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_OTHER_DEVICES_BREDR              0x00004000
#define QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_OTHER_DEVICES_BREDR_AUTH         0x00008000
#define QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_OTHER_DEVICES_LE                 0x00010000
#define QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_OTHER_DEVICES_LE_AUTH            0x00020000

#define QAPI_BLE_DEFAULT_BM_LE_FEATURES_BIT_MASK         ((QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_REQUESTING_DEVICE_LE) | (QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_REQUESTING_DEVICE_LE_AUTH) | \
                                                          (QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_ALL_DEVICES_LE)       | (QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_ALL_DEVICES_LE_AUTH)       | \
                                                          (QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_OTHER_DEVICES_LE)     | (QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_OTHER_DEVICES_LE_AUTH))

#define QAPI_BLE_DEFAULT_BM_BR_EDR_FEATURES_BIT_MASK     ((QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_REQUESTING_DEVICE_BREDR) | (QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_REQUESTING_DEVICE_BREDR_AUTH) | \
                                                          (QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_ALL_DEVICES_BREDR)       | (QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_ALL_DEVICES_BREDR_AUTH)       | \
                                                          (QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_OTHER_DEVICES_BREDR)     | (QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_OTHER_DEVICES_BREDR_AUTH))

#define QAPI_BLE_DEFAULT_BM_DUAL_MODE_FEATURES_BIT_MASK  ((QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_REQUESTING_DEVICE_BREDR_LE) | (QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_REQUESTING_DEVICE_BREDR_LE_AUTH) | \
                                                          (QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_ALL_DEVICES_BREDR_LE)       | (QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_ALL_DEVICES_BREDR_LE_AUTH)       | \
                                                          (QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_OTHER_DEVICES_BREDR_LE)     | (QAPI_BLE_BMS_BM_FEATURE_FLAG_DELETE_BOND_OTHER_DEVICES_BREDR_LE_AUTH))

   /* The following defines the values that may be set for the          */
   /* CommandType field of BM Control Point Format Data.                */
#define QAPI_BLE_BMS_BM_CONTROL_POINT_DELETE_BOND_REQUESTING_DEVICE_BREDR_LE  0x01
#define QAPI_BLE_BMS_BM_CONTROL_POINT_DELETE_BOND_REQUESTING_DEVICE_BREDR     0x02
#define QAPI_BLE_BMS_BM_CONTROL_POINT_DELETE_BOND_REQUESTING_DEVICE_LE        0x03
#define QAPI_BLE_BMS_BM_CONTROL_POINT_DELETE_BOND_ALL_DEVICES_BREDR_LE        0x04
#define QAPI_BLE_BMS_BM_CONTROL_POINT_DELETE_BOND_ALL_DEVICES_BREDR           0x05
#define QAPI_BLE_BMS_BM_CONTROL_POINT_DELETE_BOND_ALL_DEVICES_LE              0x06
#define QAPI_BLE_BMS_BM_CONTROL_POINT_DELETE_BOND_OTHER_DEVICES_BREDR_LE      0x07
#define QAPI_BLE_BMS_BM_CONTROL_POINT_DELETE_BOND_OTHER_DEVICES_BREDR         0x08
#define QAPI_BLE_BMS_BM_CONTROL_POINT_DELETE_BOND_OTHER_DEVICES_LE            0x09

   /* The following structure defines the format of the                 */
   /* BM_Control_Point structure. This structure will be used for both  */
   /* Control Point request and response purposes.                      */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_BMS_BM_Control_Point_s
{
   qapi_BLE_NonAlignedByte_t Op_Code;
   qapi_BLE_NonAlignedByte_t Variable_Data[1];
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_BMS_BM_Control_Point_t;

#define QAPI_BLE_BMS_BM_CONTROL_POINT_SIZE(_x)           (QAPI_BLE_BTPS_STRUCTURE_OFFSET(qapi_BLE_BMS_BM_Control_Point_t, Variable_Data) + _x)

#define QAPI_BLE_BMS_ATT_MTU_SIZE                        28

   /* The following defines the BMS GATT Service Flags MASK that should */
   /* be passed into qapi_BLE_GATT_Register_Service() when the BM       */
   /* Service is registered.                                            */
#define QAPI_BLE_BMS_SERVICE_FLAGS_LE                    (QAPI_BLE_GATT_SERVICE_FLAGS_LE_SERVICE)
#define QAPI_BLE_BMS_SERVICE_FLAGS_BR_EDR                (QAPI_BLE_GATT_SERVICE_FLAGS_BR_EDR_SERVICE)
#define QAPI_BLE_BMS_SERVICE_FLAGS_DUAL_MODE             (QAPI_BLE_GATT_SERVICE_FLAGS_LE_SERVICE | QAPI_BLE_GATT_SERVICE_FLAGS_BR_EDR_SERVICE)

#endif
