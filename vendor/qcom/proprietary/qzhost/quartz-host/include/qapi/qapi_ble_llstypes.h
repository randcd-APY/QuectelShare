/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef __QAPI_BLE_LLS_TYPES_H__
#define __QAPI_BLE_LLS_TYPES_H__

#include "./qapi_ble_bttypes.h" /* Bluetooth Type Definitions.          */

   /* The following MACRO is a utility MACRO that assigns the Link Loss */
   /* Service 16 bit UUID to the specified qapi_BLE_UUID_16_t Variable. */
   /* This MACRO accepts one parameter which is a pointer to a          */
   /* qapi_BLE_UUID_16_t variable that is to receive the LLS UUID       */
   /* Constant value.                                                   */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_LLS_ASSIGN_LLS_SERVICE_UUID_16(_x)           QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16(*((qapi_BLE_UUID_16_t *)(_x)), 0x18, 0x03)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined LLS Service UUID in UUID16 form.  This     */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the LLS Service UUID (MACRO returns boolean result) NOT  */
   /* less than/greater than.  The first parameter is the               */
   /* qapi_BLE_UUID_16_t variable to compare to the LLS Service UUID.   */
#define QAPI_BLE_LLS_COMPARE_LLS_SERVICE_UUID_TO_UUID_16(_x)  QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x03)

   /* The following defines the Link Loss Service UUID that is used when*/
   /* building the LLS Service Table.                                   */
#define QAPI_BLE_LLS_SERVICE_BLUETOOTH_UUID_CONSTANT          { 0x03, 0x18 }

   /* The following MACRO is a utility MACRO that assigns the LLS Alert */
   /* Level Characteristic 16 bit UUID to the specified                 */
   /* qapi_BLE_UUID_16_t variable.  This MACRO accepts one parameter    */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* LLS Alert Level UUID Constant value.                              */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_LLS_ASSIGN_ALERT_LEVEL_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x06)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined LLS Alert Level UUID in UUID16 form.  This */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the Alert Level UUID (MACRO returns boolean result) NOT  */
   /* less than/greater than.  The first parameter is the               */
   /* qapi_BLE_UUID_16_t variable to compare to the LLS Alert Level UUID*/
#define QAPI_BLE_LLS_COMPARE_LLS_ALERT_LEVEL_UUID_TO_UUID_16(_x)         QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x06)

   /* The following defines the LLS Alert Level Characteristic UUID     */
   /* that is used when building the LLS Service Table.                 */
#define QAPI_BLE_LLS_ALERT_LEVEL_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x06, 0x2A }

  /* The following defines the valid values that may be used as the     */
  /* Operand value of a Alert Level characteristic.                     */
#define QAPI_BLE_LLS_ALERT_LEVEL_NO_ALERT                0x00
#define QAPI_BLE_LLS_ALERT_LEVEL_MILD_ALERT              0x01
#define QAPI_BLE_LLS_ALERT_LEVEL_HIGH_ALERT              0x02

   /* The following defines the length of the Alert Level characteristic*/
   /* value.                                                            */
#define QAPI_BLE_LLS_ALERT_LEVEL_LENGTH                  (QAPI_BLE_NON_ALIGNED_BYTE_SIZE)

   /* The following MACRO is a utility MACRO that exists to valid that a*/
   /* specified Alert Level is valid.  The only parameter to this       */
   /* function is the Alert Level structure to valid.  This MACRO       */
   /* returns TRUE if the Alert Level is valid or FALSE otherwise.      */
#define QAPI_BLE_LLS_ALERT_LEVEL_VALID(_x)               ((((uint8_t)(_x)) >= QAPI_BLE_LLS_ALERT_LEVEL_NO_ALERT) && (((uint8_t)(_x)) <= QAPI_BLE_LLS_ALERT_LEVEL_HIGH_ALERT))

   /* The following defines the LLS GATT Service Flags MASK that should */
   /* be passed into qapi_BLE_GATT_Register_Service() when the LLS      */
   /* Service is registered.                                            */
#define QAPI_BLE_LLS_SERVICE_FLAGS                       (QAPI_BLE_GATT_SERVICE_FLAGS_LE_SERVICE)

#endif
