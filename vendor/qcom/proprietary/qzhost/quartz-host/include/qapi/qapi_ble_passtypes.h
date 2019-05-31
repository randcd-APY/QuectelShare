/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef __QAPI_BLE_PASS_TYPES_H__
#define __QAPI_BLE_PASS_TYPES_H__

#include "./qapi_ble_bttypes.h" /* Bluetooth Type Definitions.          */

   /* The following MACRO is a utility MACRO that assigns the Phone     */
   /* Alert Status Service 16 bit UUID to the specified                 */
   /* qapi_BLE_UUID_16_t variable.  This MACRO accepts one parameter    */
   /* which is a pointer to a qapi_BLE_UUID_16_t variable that is to    */
   /* receive the PASS UUID Constant value.                             */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_PASS_ASSIGN_PASS_SERVICE_UUID_16(_x)           QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16(*((qapi_BLE_UUID_16_t *)(_x)), 0x18, 0x0E)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined PASS Service UUID in UUID16 form.  This    */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the PASS Service UUID (MACRO returns boolean result) NOT */
   /* less than/greater than.  The first parameter is the               */
   /* qapi_BLE_UUID_16_t variable to compare to the PASS Service UUID.  */
#define QAPI_BLE_PASS_COMPARE_PASS_SERVICE_UUID_TO_UUID_16(_x)  QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x0E)

   /* The following defines the Phone Alert Status Service UUID that is */
   /* used when building the PASS Service Table.                        */
#define QAPI_BLE_PASS_SERVICE_BLUETOOTH_UUID_CONSTANT           { 0x0E, 0x18 }

   /* The following MACRO is a utility MACRO that assigns the PASS Alert*/
   /* Status Characteristic 16 bit UUID to the specified                */
   /* qapi_BLE_UUID_16_t variable.  This MACRO accepts one parameter    */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* PASS Alert Status UUID Constant value.                            */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_PASS_ASSIGN_ALERT_STATUS_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x3F)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined PASS Alert Status UUID in UUID16 form.     */
   /* This MACRO only returns whether the qapi_BLE_UUID_16_t variable is*/
   /* equal to the Alert Status UUID (MACRO returns boolean result) NOT */
   /* less than/greater than.  The first parameter is the               */
   /* qapi_BLE_UUID_16_t variable to compare to the PASS Alert Status   */
   /* UUID.                                                             */
#define QAPI_BLE_PASS_COMPARE_PASS_ALERT_STATUS_UUID_TO_UUID_16(_x)        QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x3F)

   /* The following defines the PASS Alert Status Characteristic UUID   */
   /* that is used when building the PASS Service Table.                */
#define QAPI_BLE_PASS_ALERT_STATUS_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x3F, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the PASS      */
   /* Ringer Setting Characteristic 16 bit UUID to the specified        */
   /* qapi_BLE_UUID_16_t variable.  This MACRO accepts one parameter    */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* PASS Ringer Setting UUID Constant value.                          */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_PASS_ASSIGN_RINGER_SETTING_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x41)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined PASS Ringer Setting UUID in UUID16 form.   */
   /* This MACRO only returns whether the qapi_BLE_UUID_16_t variable is*/
   /* equal to the Ringer Setting UUID (MACRO returns boolean result)   */
   /* NOT less than/greater than.  The first parameter is the           */
   /* qapi_BLE_UUID_16_t variable to compare to the PASS Ringer Setting */
   /* UUID.                                                             */
#define QAPI_BLE_PASS_COMPARE_PASS_RINGER_SETTING_UUID_TO_UUID_16(_x)        QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x41)

   /* The following defines the PASS Alert Status Characteristic UUID   */
   /* that is used when building the PASS Service Table.                */
#define QAPI_BLE_PASS_RINGER_SETTING_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x41, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the PASS      */
   /* Ringer Control Point Characteristic 16 bit UUID to the specified  */
   /* qapi_BLE_UUID_16_t variable.  This MACRO accepts one parameter    */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* PASS Ringer Control Point UUID Constant value.                    */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_PASS_ASSIGN_RINGER_CONTROL_POINT_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x40)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined PASS Ringer Control Point UUID in UUID16   */
   /* form.  This MACRO only returns whether the qapi_BLE_UUID_16_t     */
   /* variable is equal to the Ringer Control Point UUID (MACRO returns */
   /* boolean result) NOT less than/greater than.  The first parameter  */
   /* is the qapi_BLE_UUID_16_t variable to compare to the PASS Ringer  */
   /* Control Point UUID.                                               */
#define QAPI_BLE_PASS_COMPARE_PASS_RINGER_CONTROL_POINT_UUID_TO_UUID_16(_x)        QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x40)

   /* The following defines the PASS Alert Status Characteristic UUID   */
   /* that is used when building the PASS Service Table.                */
#define QAPI_BLE_PASS_RINGER_CONTROL_POINT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x40, 0x2A }

   /* The following defines are the valid bits that may be set in the   */
   /* PASS Alert Status Value.                                          */
#define QAPI_BLE_PASS_ALERT_STATUS_RINGER_STATE_ACTIVE         0x01
#define QAPI_BLE_PASS_ALERT_STATUS_VIBRATE_STATE_ACTIVE        0x02
#define QAPI_BLE_PASS_ALERT_STATUS_DISPLAY_ALERT_STATE_ACTIVE  0x04

   /* The following defines the length of the Alert Status              */
   /* characteristic value.                                             */
#define QAPI_BLE_PASS_ALERT_STATUS_VALUE_LENGTH          (QAPI_BLE_NON_ALIGNED_BYTE_SIZE)

   /* The following defines are the valid Ringer Settings that may be   */
   /* set.                                                              */
#define QAPI_BLE_PASS_RINGER_SETTING_RINGER_SILENT       0x00
#define QAPI_BLE_PASS_RINGER_SETTING_RINGER_NORMAL       0x01

   /* The following defines the length of the Ringer Setting            */
   /* characteristic value.                                             */
#define QAPI_BLE_PASS_RINGER_SETTING_VALUE_LENGTH        (QAPI_BLE_NON_ALIGNED_BYTE_SIZE)

   /* The following defines are the valid commands that may be written  */
   /* into the Ringer Control Point characteristic value.               */
#define QAPI_BLE_PASS_RINGER_CONTROL_COMMAND_SILENT_MODE         0x01
#define QAPI_BLE_PASS_RINGER_CONTROL_COMMAND_MUTE_ONCE           0x02
#define QAPI_BLE_PASS_RINGER_CONTROL_COMMAND_CANCEL_SILENT_MODE  0x03

   /* The following defines the length of the Ringer Control Point      */
   /* characteristic value.                                             */
#define QAPI_BLE_PASS_RINGER_CONTROL_POINT_VALUE_LENGTH  (QAPI_BLE_NON_ALIGNED_BYTE_SIZE)

#endif
