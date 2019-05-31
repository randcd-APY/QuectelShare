/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef __QAPI_BLE_IAS_TYPES_H__
#define __QAPI_BLE_IAS_TYPES_H__

#include "./qapi_ble_bttypes.h" /* Bluetooth Type Definitions.          */

   /* The following MACRO is a utility MACRO that assigns the Immediate */
   /* Alert Service 16 bit UUID to the specified qapi_BLE_UUID_16_t     */
   /* variable.  This MACRO accepts one parameter which is a pointer to */
   /* a qapi_BLE_UUID_16_t variable that is to receive the IAS UUID     */
   /* Constant value.                                                   */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_IAS_ASSIGN_IAS_SERVICE_UUID_16(_x)           QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16(*((qapi_BLE_UUID_16_t *)(_x)), 0x18, 0x02)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined IAS Service UUID in UUID16 form.  This     */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the IAS Service UUID (MACRO returns boolean result) NOT  */
   /* less than/greater than.  The first parameter is the               */
   /* qapi_BLE_UUID_16_t variable to compare to the IAS Service UUID.   */
#define QAPI_BLE_IAS_COMPARE_IAS_SERVICE_UUID_TO_UUID_16(_x)  QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x02)

   /* The following defines the IAS Parameter Service UUID that is      */
   /* used when building the IAS Service Table.                         */
#define QAPI_BLE_IAS_SERVICE_BLUETOOTH_UUID_CONSTANT          { 0x02, 0x18 }

   /* The following MACRO is a utility MACRO that assigns the IAS Alert */
   /* Level Characteristic 16 bit UUID to the specified                 */
   /* qapi_BLE_UUID_16_t variable.  This MACRO accepts one parameter    */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* IAS Alert Level UUID Constant value.                              */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_IAS_ASSIGN_ALERT_LEVEL_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x06)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined IAS Alert Level UUID in UUID16 form.  This */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the Alert Level UUID (MACRO returns boolean result) NOT  */
   /* less than/greater than.  The first parameter is the               */
   /* qapi_BLE_UUID_16_t variable to compare to the IAS Alert Level UUID*/
#define QAPI_BLE_IAS_COMPARE_ALERT_LEVEL_UUID_TO_UUID_16(_x)             QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x06)

   /* The following defines the IAS Alert Level Characteristic UUID that*/
   /* is used when building the IAS Service Table.                      */
#define QAPI_BLE_IAS_ALERT_LEVEL_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x06, 0x2A }

  /* The following defines the IAS GATT Service Flags MASK that should  */
  /* be passed into qapi_BLE_GATT_Register_Service() when the IAS       */
  /* Service is registered.                                             */
#define QAPI_BLE_IAS_ALERT_LEVEL_NO_ALERT                0x00
#define QAPI_BLE_IAS_ALERT_LEVEL_MILD_ALERT              0x01
#define QAPI_BLE_IAS_ALERT_LEVEL_HIGH_ALERT              0x02

   /* The following MACRO is a utility MACRO that exists to valid that a*/
   /* specified Alert Level is valid.  The only parameter to this       */
   /* function is the Alert Level structure to valid.  This MACRO       */
   /* returns TRUE if the Alert Level is valid or FALSE otherwise.      */
#define QAPI_BLE_IAS_ALERT_LEVEL_VALID(_x)               ((((uint8_t)(_x)) >= QAPI_BLE_IAS_ALERT_LEVEL_NO_ALERT) && (((uint8_t)(_x)) <= QAPI_BLE_IAS_ALERT_LEVEL_HIGH_ALERT))

   /* The following defines the length of the Alert Level Control Point */
   /* characteristic value.                                             */
#define QAPI_BLE_IAS_ALERT_LEVEL_CONTROL_POINT_VALUE_LENGTH  (QAPI_BLE_NON_ALIGNED_BYTE_SIZE)

   /* The following defines the IAS GATT Service Flags MASK that should */
   /* be passed into qapi_BLE_GATT_Register_Service() when the IAS      */
   /* Service is registered.                                            */
#define QAPI_BLE_IAS_SERVICE_FLAGS                       (QAPI_BLE_GATT_SERVICE_FLAGS_LE_SERVICE)

#endif
