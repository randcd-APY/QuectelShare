/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef __QAPI_BLE_IPSP_TYPES_H__
#define __QAPI_BLE_IPSP_TYPES_H__

#include "./qapi_ble_bttypes.h" /* Bluetooth Type Definitions.          */

   /* The following MACRO is a utility MACRO that assigns the Immediate */
   /* Alert Service 16 bit UUID to the specified qapi_BLE_UUID_16_t     */
   /* variable.  This MACRO accepts one parameter which is a pointer to */
   /* a qapi_BLE_UUID_16_t variable that is to receive the IPSS UUID    */
   /* Constant value.                                                   */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_IPSS_ASSIGN_IPSS_SERVICE_UUID_16(_x)               QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16(*((qapi_BLE_UUID_16_t *)(_x)), 0x18, 0x20)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined IPSS Service UUID in UUID16 form.  This    */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the IPSS Service UUID (MACRO returns boolean result) NOT */
   /* less than/greater than.  The first parameter is the               */
   /* qapi_BLE_UUID_16_t variable to compare to the IPSS Service UUID.  */
#define QAPI_BLE_IPSS_COMPARE_IPSS_SERVICE_UUID_TO_UUID_16(_x)      QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x20)

   /* The following defines the IPSS Parameter Service UUID that is used*/
   /* when building the IPSS Service Table.                             */
#define QAPI_BLE_IPSS_SERVICE_BLUETOOTH_UUID_CONSTANT               { 0x20, 0x18 }

   /* The following defines the IPSP LE PSM.                            */
#define QAPI_BLE_IPSP_LE_PSM                                        (0x0023)

   /* The following defines the IPSS GATT Service Flags MASK that should*/
   /* be passed into GATT_Register_Service when the IPSS Service is     */
   /* registered.                                                       */
#define QAPI_BLE_IPSS_SERVICE_FLAGS                                 (QAPI_BLE_GATT_SERVICE_FLAGS_LE_SERVICE)

#endif
