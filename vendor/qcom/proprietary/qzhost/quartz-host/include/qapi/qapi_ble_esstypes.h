/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef __QAPI_BLE_ESS_TYPES_H__
#define __QAPI_BLE_ESS_TYPES_H__

#include "./qapi_ble_bttypes.h" /* Bluetooth Type Definitions.          */

   /* The following defines the OTS Attribute Protocol (ATT) Application*/
   /* Error Codes that may be sent in a GATT response.                  */
   /* * NOTE * The Attribute Protocol (ATT) Error Codes have the form   */
   /*          QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX found in            */
   /*          qapi_ble_atttypes.h and may also be sent in a GATT       */
   /*          response.                                                */
#define QAPI_BLE_ESS_ERROR_CODE_SUCCESS                                 (0x00)
#define QAPI_BLE_ESS_ERROR_CODE_WRITE_REQUEST_REJECTED                  (0x80)
#define QAPI_BLE_ESS_ERROR_CODE_CONDITION_NOT_SUPPORTED                 (0x81)

   /* ESS Characteristic UUID Assigns, Comparisons, and Constants.      */

   /* The following MACRO is a utility MACRO that assigns the           */
   /* Environmental Sensing Service 16 bit UUID to the specified        */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is a pointer to a qapi_BLE_UUID_16_t variable that is to    */
   /* receive the ESS UUID Constant value.                              */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_ESS_SERVICE_UUID_16(_x)                     QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16(*((qapi_BLE_UUID_16_t *)(_x)), 0x18, 0x1A)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined ESS Service UUID in UUID16 form. This MACRO*/
   /* only returns whether the qapi_BLE_UUID_16_t variable is equal to  */
   /* the ESS Service UUID (MACRO returns boolean result) NOT less      */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare to the ESS Service UUID.                      */
#define QAPI_BLE_ESS_COMPARE_ESS_SERVICE_UUID_TO_UUID_16(_x)            QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x1A)

   /* The following MACRO is a utility MACRO that assigns the AIOS      */
   /* Service 16 bit UUID to the specified qapi_BLE_UUID_16_t variable. */
   /* This MACRO accepts one parameter which is a qapi_BLE_UUID_16_t    */
   /* variable that is to receive the AIOS UUID Constant value.         */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Big-Endian format.                           */
#define QAPI_BLE_ESS_ASSIGN_ESS_SERVICE_SDP_UUID_16(_x)                 QAPI_BLE_ASSIGN_SDP_UUID_16((_x), 0x18, 0x1A)

   /* The following defines the ESS Parameter Service UUID that is      */
   /* used when building the ESS Service Table.                         */
#define QAPI_BLE_ESS_SERVICE_BLUETOOTH_UUID_CONSTANT                    { 0x1A, 0x18 }

   /* The following MACRO is a utility MACRO that assigns the Descriptor*/
   /* Value Changed Characteristic 16 bit UUID to the specified         */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* UUID Constant value.                                              */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_DESCRIPTOR_VALUE_CHANGED_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x7D)

   /* The following MACRO is a utility MACRO that exist to compare a 16 */
   /* bit UUID to the defined ESS Descriptor Value Changed UUID in      */
   /* UUID16 form. This MACRO only returns whether the                  */
   /* qapi_BLE_UUID_16_t variable is equal to the UUID (MACRO returns   */
   /* boolean result) NOT less than/greater than. The first parameter is*/
   /* the qapi_BLE_UUID_16_t variable to compare.                       */
#define QAPI_BLE_ESS_COMPARE_DESCRIPTOR_VALUE_CHANGED_UUID_TO_UUID_16(_x)             QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x7D)

   /* The following defines the ESS Descriptor Value Changed            */
   /* Characteristic UUID that is used when building the ESS Service    */
   /* Table.                                                            */
#define QAPI_BLE_ESS_DESCRIPTOR_VALUE_CHANGED_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x7D, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the Apparent  */
   /* Wind Direction Characteristic 16 bit UUID to the specified        */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* UUID Constant value.                                              */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_APPARENT_WIND_DIRECTION_UUID_16(_x)                       QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x73)

   /* The following MACRO is a utility MACRO that exist to compare a 16 */
   /* bit UUID to the defined ESS Apparent Wind Direction UUID in UUID16*/
   /* form. This MACRO only returns whether the qapi_BLE_UUID_16_t      */
   /* variable is equal to the UUID (MACRO returns boolean result) NOT  */
   /* less than/greater than. The first parameter is the                */
   /* qapi_BLE_UUID_16_t variable to compare.                           */
#define QAPI_BLE_ESS_COMPARE_APPARENT_WIND_DIRECTION_UUID_TO_UUID_16(_x)              QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x73)

   /* The following defines the ESS Apparent Wind Direction             */
   /* Characteristic UUID that is used when building the ESS Service    */
   /* Table.                                                            */
#define QAPI_BLE_ESS_APPARENT_WIND_DIRECTION_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT   { 0x73, 0x2A }

   /* The following defines additional definitions for the ESS Apparent */
   /* Wind Direction.                                                   */
#define QAPI_BLE_ESS_APPARENT_WIND_DIRECTION_MINIMUM_VALUE                            (0)
#define QAPI_BLE_ESS_APPARENT_WIND_DIRECTION_MAXIMUM_VALUE                            (35999)

   /* The following MACRO is a utility MACRO that assigns the Apparent  */
   /* Wind Speed Characteristic 16 bit UUID to the specified            */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* UUID Constant value.                                              */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_APPARENT_WIND_SPEED_UUID_16(_x)                       QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x72)

   /* The following MACRO is a utility MACRO that exist to compare a 16 */
   /* bit UUID to the defined ESS Apparent Wind Speed UUID in UUID16    */
   /* form. This MACRO only returns whether the qapi_BLE_UUID_16_t      */
   /* variable is equal to the UUID (MACRO returns boolean result) NOT  */
   /* less than/greater than. The first parameter is the                */
   /* qapi_BLE_UUID_16_t variable to compare.                           */
#define QAPI_BLE_ESS_COMPARE_APPARENT_WIND_SPEED_UUID_TO_UUID_16(_x)              QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x72)

   /* The following defines the ESS Apparent Wind Speed Characteristic  */
   /* UUID that is used when building the ESS Service Table.            */
#define QAPI_BLE_ESS_APPARENT_WIND_SPEED_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT   { 0x72, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the Dew Point */
   /* Characteristic 16 bit UUID to the specified qapi_BLE_UUID_16_t    */
   /* variable. This MACRO accepts one parameter which is the           */
   /* qapi_BLE_UUID_16_t variable that is to receive the UUID Constant  */
   /* value.                                                            */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_DEW_POINT_UUID_16(_x)                                 QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x7B)

   /* The following MACRO is a utility MACRO that exist to compare a 16 */
   /* bit UUID to the defined ESS Dew Point UUID in UUID16 form. This   */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the UUID (MACRO returns boolean result) NOT less         */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare.                                              */
#define QAPI_BLE_ESS_COMPARE_DEW_POINT_UUID_TO_UUID_16(_x)                        QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x7B)

   /* The following defines the ESS Dew Point Characteristic UUID that  */
   /* is used when building the ESS Service Table.                      */
#define QAPI_BLE_ESS_DEW_POINT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT             { 0x7B, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the Elevation */
   /* Characteristic 16 bit UUID to the specified qapi_BLE_UUID_16_t    */
   /* variable. This MACRO accepts one parameter which is the           */
   /* qapi_BLE_UUID_16_t variable that is to receive the UUID Constant  */
   /* value.                                                            */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_ELEVATION_UUID_16(_x)                                 QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x6C)

   /* The following MACRO is a utility MACRO that exist to compare a 16 */
   /* bit UUID to the defined ESS Elevation UUID in UUID16 form. This   */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the UUID (MACRO returns boolean result) NOT less         */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare.                                              */
#define QAPI_BLE_ESS_COMPARE_ELEVATION_UUID_TO_UUID_16(_x)                        QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x6C)

   /* The following defines the ESS Elevation Characteristic UUID that  */
   /* is used when building the ESS Service Table.                      */
#define QAPI_BLE_ESS_ELEVATION_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT             { 0x6C, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the Gust      */
   /* Factor Characteristic 16 bit UUID to the specified                */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* UUID Constant value.                                              */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_GUST_FACTOR_UUID_16(_x)                               QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x74)

   /* The following MACRO is a utility MACRO that exist to compare a 16 */
   /* bit UUID to the defined ESS Gust Factor UUID in UUID16 form. This */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the UUID (MACRO returns boolean result) NOT less         */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare.                                              */
#define QAPI_BLE_ESS_COMPARE_GUST_FACTOR_UUID_TO_UUID_16(_x)                      QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x74)

   /* The following defines the ESS Gust Factor Characteristic UUID that*/
   /* is used when building the ESS Service Table.                      */
#define QAPI_BLE_ESS_GUST_FACTOR_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT           { 0x74, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the Heat Index*/
   /* Characteristic 16 bit UUID to the specified qapi_BLE_UUID_16_t    */
   /* variable. This MACRO accepts one parameter which is the           */
   /* qapi_BLE_UUID_16_t variable that is to receive the UUID Constant  */
   /* value.                                                            */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_HEAT_INDEX_UUID_16(_x)                                QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x7A)

   /* The following MACRO is a utility MACRO that exist to compare a 16 */
   /* bit UUID to the defined ESS Heat Index UUID in UUID16 form. This  */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the UUID (MACRO returns boolean result) NOT less         */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare.                                              */
#define QAPI_BLE_ESS_COMPARE_HEAT_INDEX_UUID_TO_UUID_16(_x)                       QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x7A)

   /* The following defines the ESS Heat Index Characteristic UUID that */
   /* is used when building the ESS Service Table.                      */
#define QAPI_BLE_ESS_HEAT_INDEX_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT            { 0x7A, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the Humidity  */
   /* Characteristic 16 bit UUID to the specified qapi_BLE_UUID_16_t    */
   /* variable. This MACRO accepts one parameter which is the           */
   /* qapi_BLE_UUID_16_t variable that is to receive the UUID Constant  */
   /* value.                                                            */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_HUMIDITY_UUID_16(_x)                                  QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x6F)

   /* The following MACRO is a utility MACRO that exist to compare a 16 */
   /* bit UUID to the defined ESS Humidity UUID in UUID16 form. This    */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the UUID (MACRO returns boolean result) NOT less         */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare.                                              */
#define QAPI_BLE_ESS_COMPARE_HUMIDITY_UUID_TO_UUID_16(_x)                         QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x6F)

   /* The following defines the ESS Humidity Characteristic UUID that is*/
   /* used when building the ESS Service Table.                         */
#define QAPI_BLE_ESS_HUMIDITY_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT              { 0x6F, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the Irradiance*/
   /* Characteristic 16 bit UUID to the specified qapi_BLE_UUID_16_t    */
   /* variable. This MACRO accepts one parameter which is the           */
   /* qapi_BLE_UUID_16_t variable that is to receive the UUID Constant  */
   /* value.                                                            */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_IRRADIANCE_UUID_16(_x)                                  QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x77)

   /* The following MACRO is a utility MACRO that exist to compare a 16 */
   /* bit UUID to the defined ESS Irradiance UUID in UUID16 form. This  */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the UUID (MACRO returns boolean result) NOT less         */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare.                                              */
#define QAPI_BLE_ESS_COMPARE_IRRADIANCE_UUID_TO_UUID_16(_x)                         QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x77)

   /* The following defines the ESS Irradiance Characteristic UUID that */
   /* is used when building the ESS Service Table.                      */
#define QAPI_BLE_ESS_IRRADIANCE_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT              { 0x77, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the Pollen    */
   /* Concentration Characteristic 16 bit UUID to the specified         */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* UUID Constant value.                                              */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_POLLEN_CONCENTRATION_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x75)

   /* The following MACRO is a utility MACRO that exist to compare a 16 */
   /* bit UUID to the defined ESS Pollen Concentration UUID in UUID16   */
   /* form. This MACRO only returns whether the qapi_BLE_UUID_16_t      */
   /* variable is equal to the UUID (MACRO returns boolean result) NOT  */
   /* less than/greater than. The first parameter is the                */
   /* qapi_BLE_UUID_16_t variable to compare.                           */
#define QAPI_BLE_ESS_COMPARE_POLLEN_CONCENTRATION_UUID_TO_UUID_16(_x)             QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x75)

   /* The following defines the ESS Pollen Concentration Characteristic */
   /* UUID that is used when building the ESS Service Table.            */
#define QAPI_BLE_ESS_POLLEN_CONCENTRATION_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x75, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the Rainfall  */
   /* Characteristic 16 bit UUID to the specified qapi_BLE_UUID_16_t    */
   /* variable. This MACRO accepts one parameter which is the           */
   /* qapi_BLE_UUID_16_t variable that is to receive the UUID Constant  */
   /* value.                                                            */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_RAINFALL_UUID_16(_x)                                  QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x78)

   /* The following MACRO is a utility MACRO that exist to compare a 16 */
   /* bit UUID to the defined ESS Rainfall UUID in UUID16 form. This    */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the UUID (MACRO returns boolean result) NOT less         */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare.                                              */
#define QAPI_BLE_ESS_COMPARE_RAINFALL_UUID_TO_UUID_16(_x)                         QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x78)

   /* The following defines the ESS Rainfall Characteristic UUID that is*/
   /* used when building the ESS Service Table.                         */
#define QAPI_BLE_ESS_RAINFALL_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT              { 0x78, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the Pressure  */
   /* Characteristic 16 bit UUID to the specified qapi_BLE_UUID_16_t    */
   /* variable. This MACRO accepts one parameter which is the           */
   /* qapi_BLE_UUID_16_t variable that is to receive the UUID Constant  */
   /* value.                                                            */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_PRESSURE_UUID_16(_x)                                  QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x6D)

   /* The following MACRO is a utility MACRO that exist to compare a 16 */
   /* bit UUID to the defined ESS Pressure UUID in UUID16 form. This    */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the UUID (MACRO returns boolean result) NOT less         */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare.                                              */
#define QAPI_BLE_ESS_COMPARE_PRESSURE_UUID_TO_UUID_16(_x)                         QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x6D)

   /* The following defines the ESS Pressure Characteristic UUID that is*/
   /* used when building the ESS Service Table.                         */
#define QAPI_BLE_ESS_PRESSURE_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT              { 0x6D, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the           */
   /* Temperature Characteristic 16 bit UUID to the specified           */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* UUID Constant value.                                              */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_TEMPERATURE_UUID_16(_x)                               QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x6E)

   /* The following MACRO is a utility MACRO that exist to compare a 16 */
   /* bit UUID to the defined ESS Temperature UUID in UUID16 form. This */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the UUID (MACRO returns boolean result) NOT less         */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare.                                              */
#define QAPI_BLE_ESS_COMPARE_TEMPERATURE_UUID_TO_UUID_16(_x)                      QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x6E)

   /* The following defines the ESS Temperature Characteristic UUID that*/
   /* is used when building the ESS Service Table.                      */
#define QAPI_BLE_ESS_TEMPERATURE_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT           { 0x6E, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the True Wind */
   /* Direction Characteristic 16 bit UUID to the specified             */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* UUID Constant value.                                              */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_TRUE_WIND_DIRECTION_UUID_16(_x)                       QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x71)

   /* The following MACRO is a utility MACRO that exist to compare a 16 */
   /* bit UUID to the defined ESS True Wind Direction UUID in UUID16    */
   /* form. This MACRO only returns whether the qapi_BLE_UUID_16_t      */
   /* variable is equal to the UUID (MACRO returns boolean result) NOT  */
   /* less than/greater than. The first parameter is the                */
   /* qapi_BLE_UUID_16_t variable to compare.                           */
#define QAPI_BLE_ESS_COMPARE_TRUE_WIND_DIRECTION_UUID_TO_UUID_16(_x)              QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x71)

   /* The following defines the ESS True Wind Direction Characteristic  */
   /* UUID that is used when building the ESS Service Table.            */
#define QAPI_BLE_ESS_TRUE_WIND_DIRECTION_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT   { 0x71, 0x2A }

   /* The following defines the additional definitions for the ESS True */
   /* Wind Direction.                                                   */
#define QAPI_BLE_ESS_TRUE_WIND_DIRECTION_MINIMUM_VALUE                            (0)
#define QAPI_BLE_ESS_TRUE_WIND_DIRECTION_MAXIMUM_VALUE                            (35999)

   /* The following MACRO is a utility MACRO that assigns the True Wind */
   /* Speed Characteristic 16 bit UUID to the specified                 */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* UUID Constant value.                                              */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_TRUE_WIND_SPEED_UUID_16(_x)                           QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x70)

   /* The following MACRO is a utility MACRO that exist to compare a 16 */
   /* bit UUID to the defined ESS True Wind Speed UUID in UUID16 form.  */
   /* This MACRO only returns whether the qapi_BLE_UUID_16_t variable is*/
   /* equal to the UUID (MACRO returns boolean result) NOT less         */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare.                                              */
#define QAPI_BLE_ESS_COMPARE_TRUE_WIND_SPEED_UUID_TO_UUID_16(_x)                  QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x70)

   /* The following defines the ESS True Wind Speed Characteristic UUID */
   /* that is used when building the ESS Service Table.                 */
#define QAPI_BLE_ESS_TRUE_WIND_SPEED_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT       { 0x70, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the UV Index  */
   /* Characteristic 16 bit UUID to the specified qapi_BLE_UUID_16_t    */
   /* variable. This MACRO accepts one parameter which is the           */
   /* qapi_BLE_UUID_16_t variable that is to receive the UUID Constant  */
   /* value.                                                            */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_UV_INDEX_UUID_16(_x)                                  QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x76)

   /* The following MACRO is a utility MACRO that exist to compare a 16 */
   /* bit UUID to the defined ESS UV Index UUID in UUID16 form. This    */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the UUID (MACRO returns boolean result) NOT less         */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare.                                              */
#define QAPI_BLE_ESS_COMPARE_UV_INDEX_UUID_TO_UUID_16(_x)                         QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x76)

   /* The following defines the ESS UV Index Characteristic UUID that is*/
   /* used when building the ESS Service Table.                         */
#define QAPI_BLE_ESS_UV_INDEX_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT              { 0x76, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the Wind Chill*/
   /* Characteristic 16 bit UUID to the specified qapi_BLE_UUID_16_t    */
   /* variable. This MACRO accepts one parameter which is the           */
   /* qapi_BLE_UUID_16_t variable that is to receive the UUID Constant  */
   /* value.                                                            */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_WIND_CHILL_UUID_16(_x)                                QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x79)

   /* The following MACRO is a utility MACRO that exist to compare a 16 */
   /* bit UUID to the defined ESS Wind Chill UUID in UUID16 form. This  */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the UUID (MACRO returns boolean result) NOT less         */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare.                                              */
#define QAPI_BLE_ESS_COMPARE_WIND_CHILL_UUID_TO_UUID_16(_x)                       QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x79)

   /* The following defines the ESS Wind Chill Characteristic UUID that */
   /* is used when building the ESS Service Table.                      */
#define QAPI_BLE_ESS_WIND_CHILL_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT            { 0x79, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the Barometric*/
   /* Pressure Trend Characteristic 16 bit UUID to the specified        */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* UUID Constant value.                                              */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_BAROMETRIC_PRESSURE_TREND_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0xA3)

   /* The following MACRO is a utility MACRO that exist to compare a 16 */
   /* bit UUID to the defined ESS Barometric Pressure Trend UUID in     */
   /* UUID16 form. This MACRO only returns whether the                  */
   /* qapi_BLE_UUID_16_t variable is equal to the UUID (MACRO returns   */
   /* boolean result) NOT less than/greater than. The first parameter is*/
   /* the qapi_BLE_UUID_16_t variable to compare.                       */
#define QAPI_BLE_ESS_COMPARE_BAROMETRIC_PRESSURE_TREND_UUID_TO_UUID_16(_x)             QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0xA3)

   /* The following defines the ESS Barometric Pressure Trend           */
   /* Characteristic UUID that is used when building the ESS Service    */
   /* Table.                                                            */
#define QAPI_BLE_ESS_BAROMETRIC_PRESSURE_TREND_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0xA3, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the Magnetic  */
   /* Declination Characteristic 16 bit UUID to the specified           */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* UUID Constant value.                                              */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_MAGNETIC_DECLINATION_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0x2C)

   /* The following MACRO is a utility MACRO that exist to compare a 16 */
   /* bit UUID to the defined ESS Magnetic Declination UUID in UUID16   */
   /* form. This MACRO only returns whether the qapi_BLE_UUID_16_t      */
   /* variable is equal to the UUID (MACRO returns boolean result) NOT  */
   /* less than/greater than. The first parameter is the                */
   /* qapi_BLE_UUID_16_t variable to compare.                           */
#define QAPI_BLE_ESS_COMPARE_MAGNETIC_DECLINATION_UUID_TO_UUID_16(_x)             QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0x2C)

   /* The following defines the ESS Magnetic Declination Characteristic */
   /* UUID that is used when building the ESS Service Table.            */
#define QAPI_BLE_ESS_MAGNETIC_DECLINATION_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x2C, 0x2A }

   /* The following defines the additional definitions for the ESS      */
   /* Magnetic Declination.                                             */
#define QAPI_BLE_ESS_MAGNETIC_DECLINATION_MINIMUM_VALUE                           (0)
#define QAPI_BLE_ESS_MAGNETIC_DECLINATION_MAXIMUM_VALUE                           (35999)

   /* The following MACRO is a utility MACRO that assigns the Magnetic  */
   /* Flux Density - 2D Characteristic 16 bit UUID to the specified     */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* UUID Constant value.                                              */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_MAGNETIC_FLUX_DENSITY_2D_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0xA0)

   /* The following MACRO is a utility MACRO that exist to compare a 16 */
   /* bit UUID to the defined ESS Magnetic Flux Density - 2D UUID in    */
   /* UUID16 form. This MACRO only returns whether the                  */
   /* qapi_BLE_UUID_16_t variable is equal to the UUID (MACRO returns   */
   /* boolean result) NOT less than/greater than. The first parameter is*/
   /* the qapi_BLE_UUID_16_t variable to compare.                       */
#define QAPI_BLE_ESS_COMPARE_MAGNETIC_FLUX_DENSITY_2D_UUID_TO_UUID_16(_x)             QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0xA0)

   /* The following defines the ESS Magnetic Flux Density - 2D          */
   /* Characteristic UUID that is used when building the ESS Service    */
   /* Table.                                                            */
#define QAPI_BLE_ESS_MAGNETIC_FLUX_DENSITY_2D_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0xA0, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the Magnetic  */
   /* Flux Density - 3D Characteristic 16 bit UUID to the specified     */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* UUID Constant value.                                              */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_MAGNETIC_FLUX_DENSITY_3D_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0xA1)

   /* The following MACRO is a utility MACRO that exist to compare a 16 */
   /* bit UUID to the defined ESS Magnetic Flux Density - 3D UUID in    */
   /* UUID16 form. This MACRO only returns whether the                  */
   /* qapi_BLE_UUID_16_t variable is equal to the UUID (MACRO returns   */
   /* boolean result) NOT less than/greater than. The first parameter is*/
   /* the qapi_BLE_UUID_16_t variable to compare.                       */
#define QAPI_BLE_ESS_COMPARE_MAGNETIC_FLUX_DENSITY_3D_UUID_TO_UUID_16(_x)             QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0xA1)

   /* The following defines the ESS Magnetic Flux Density - 3D          */
   /* Characteristic UUID that is used when building the ESS Service    */
   /* Table.                                                            */
#define QAPI_BLE_ESS_MAGNETIC_FLUX_DENSITY_3D_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0xA1, 0x2A }

   /* ESS Characteristic Descriptor UUID Assigns, Comparisons, and      */
   /* Constants.                                                        */
   /* * NOTE * GATT Client Characteristic Configuration Descriptor and  */
   /*          GATT User Description Descriptor Assigns, Compares, and  */
   /*          Constants may be found in GATTType.h.                    */

   /* The following MACRO is a utility MACRO that assigns the           */
   /* Environmental Sensing Measurement Characteristic 16 bit UUID to   */
   /* the specified qapi_BLE_UUID_16_t variable. This MACRO accepts one */
   /* parameter which is the qapi_BLE_UUID_16_t variable that is to     */
   /* receive the UUID Constant value.                                  */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_ENVIRONMENTAL_SENSING_MEASUREMENT_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x29, 0x0C)

   /* The following MACRO is a utility MACRO that exist to compare a 16 */
   /* bit UUID to the defined ESS Environmental Sensing Measurement UUID*/
   /* in UUID16 form. This MACRO only returns whether the               */
   /* qapi_BLE_UUID_16_t variable is equal to the UUID (MACRO returns   */
   /* boolean result) NOT less than/greater than. The first parameter is*/
   /* the qapi_BLE_UUID_16_t variable to compare.                       */
#define QAPI_BLE_ESS_COMPARE_ENVIRONMENTAL_SENSING_MEASUREMENT_UUID_TO_UUID_16(_x)             QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x29, 0x0C)

   /* The following defines the ESS Environmental Sensing Measurement   */
   /* Characteristic UUID that is used when building the ESS Service    */
   /* Table.                                                            */
#define QAPI_BLE_ESS_ENVIRONMENTAL_SENSING_MEASUREMENT_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x0C, 0x29 }

   /* The following MACRO is a utility MACRO that assigns the           */
   /* Environmental Sensing Trigger Setting Characteristic 16 bit UUID  */
   /* to the specified qapi_BLE_UUID_16_t variable. This MACRO accepts  */
   /* one parameter which is the qapi_BLE_UUID_16_t variable that is to */
   /* receive the UUID Constant value.                                  */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_ENVIRONMENTAL_SENSING_TRIGGER_SETTING_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x29, 0x0D)

   /* The following MACRO is a utility MACRO that exist to compare a 16 */
   /* bit UUID to the defined ESS Environmental Sensing Trigger Setting */
   /* UUID in UUID16 form. This MACRO only returns whether the          */
   /* qapi_BLE_UUID_16_t variable is equal to the UUID (MACRO returns   */
   /* boolean result) NOT less than/greater than. The first parameter is*/
   /* the qapi_BLE_UUID_16_t variable to compare.                       */
#define QAPI_BLE_ESS_COMPARE_ENVIRONMENTAL_SENSING_TRIGGER_SETTING_UUID_TO_UUID_16(_x)             QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x29, 0x0D)

   /* The following defines the ESS Environmental Sensing Trigger       */
   /* Setting Characteristic UUID that is used when building the ESS    */
   /* Service Table.                                                    */
#define QAPI_BLE_ESS_ENVIRONMENTAL_SENSING_TRIGGER_SETTING_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x0D, 0x29 }

   /* The following MACRO is a utility MACRO that assigns the           */
   /* Environmental Sensing Configuration Characteristic 16 bit UUID to */
   /* the specified qapi_BLE_UUID_16_t variable. This MACRO accepts one */
   /* parameter which is the qapi_BLE_UUID_16_t variable that is to     */
   /* receive the UUID Constant value.                                  */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_ENVIRONMENTAL_SENSING_CONFIGURATION_UUID_16(_x)                      QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x29, 0x0B)

   /* The following MACRO is a utility MACRO that exist to compare a 16 */
   /* bit UUID to the defined ESS Environmental Sensing Configuration   */
   /* UUID in UUID16 form. This MACRO only returns whether the          */
   /* qapi_BLE_UUID_16_t variable is equal to the UUID (MACRO returns   */
   /* boolean result) NOT less than/greater than. The first parameter is*/
   /* the qapi_BLE_UUID_16_t variable to compare.                       */
#define QAPI_BLE_ESS_COMPARE_ENVIRONMENTAL_SENSING_CONFIGURATION_UUID_TO_UUID_16(_x)             QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x29, 0x0B)

   /* The following defines the ESS Environmental Sensing Configuration */
   /* Characteristic UUID that is used when building the ESS Service    */
   /* Table.                                                            */
#define QAPI_BLE_ESS_ENVIRONMENTAL_SENSING_CONFIGURATION_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT  { 0x0B, 0x29 }

   /* The following MACRO is a utility MACRO that assigns the Valid     */
   /* Range Characteristic 16 bit UUID to the specified                 */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* UUID Constant value.                                              */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_ESS_ASSIGN_VALID_RANGE_UUID_16(_x)                               QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x29, 0x06)

   /* The following MACRO is a utility MACRO that exist to compare a 16 */
   /* bit UUID to the defined ESS Valid Range UUID in UUID16 form. This */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the UUID (MACRO returns boolean result) NOT less         */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare.                                              */
#define QAPI_BLE_ESS_COMPARE_VALID_RANGE_UUID_TO_UUID_16(_x)                      QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x29, 0x06)

   /* The following defines the ESS Valid Range Characteristic UUID that*/
   /* is used when building the ESS Service Table.                      */
#define QAPI_BLE_ESS_VALID_RANGE_CHARACTERISTIC_BLUETOOTH_UUID_CONSTANT           { 0x06, 0x29 }

   /* The following defines the ESS unsigned/signed int 24 structure.   */
   /* * NOTE * The fields are reversed since this is a contiguous       */
   /*          multi-byte value that will be formatted in Little-Endian.*/
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_ESS_Int_24_s
{
   qapi_BLE_NonAlignedWord_t Lower;
   qapi_BLE_NonAlignedByte_t Upper;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_ESS_Int_24_t;

#define QAPI_BLE_ESS_INT_24_SIZE                                        (sizeof(qapi_BLE_ESS_Int_24_t))

   /* The folllowing defines the ESS Magnetic Flux Density 2D           */
   /* characteristic structure.                                         */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_ESS_Magnetic_Flux_Density_2D_s
{
   qapi_BLE_NonAlignedSWord_t X_Axis;
   qapi_BLE_NonAlignedSWord_t Y_Axis;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_ESS_Magnetic_Flux_Density_2D_t;

#define QAPI_BLE_ESS_MAGNETIC_FLUX_DENSITY_2D_SIZE                      (sizeof(qapi_BLE_ESS_Magnetic_Flux_Density_2D_t))

   /* The folllowing defines the ESS Magnetic Flux Density 3D           */
   /* characteristic structure.                                         */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_ESS_Magnetic_Flux_Density_3D_s
{
   qapi_BLE_NonAlignedSWord_t X_Axis;
   qapi_BLE_NonAlignedSWord_t Y_Axis;
   qapi_BLE_NonAlignedSWord_t Z_Axis;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_ESS_Magnetic_Flux_Density_3D_t;

#define QAPI_BLE_ESS_MAGNETIC_FLUX_DENSITY_3D_SIZE                      (sizeof(qapi_BLE_ESS_Magnetic_Flux_Density_3D_t))

   /* The following defines the valid values for the Flags field of the */
   /* qapi_BLE_ESS_ES_Measurement_t structure below.                    */
   /* * NOTE * These may be added in the future.                        */
#define QAPI_BLE_ESS_MEASUREMENT_DATA_FLAGS_RESERVED                    (0xFFFF)

   /* The following defines the valid values for the Sampling_Function  */
   /* field of the qapi_BLE_ESS_ES_Measurement_t structure below.       */
#define QAPI_BLE_ESS_ES_MEASUREMENT_SAMPLING_FUNCTION_UNSPECIFIED       (0x00)
#define QAPI_BLE_ESS_ES_MEASUREMENT_SAMPLING_FUNCTION_INSTANTANEOUS     (0x01)
#define QAPI_BLE_ESS_ES_MEASUREMENT_SAMPLING_FUNCTION_ARITHMETIC_MEAN   (0x02)
#define QAPI_BLE_ESS_ES_MEASUREMENT_SAMPLING_FUNCTION_RMS               (0x03)
#define QAPI_BLE_ESS_ES_MEASUREMENT_SAMPLING_FUNCTION_MAXIMUM           (0x04)
#define QAPI_BLE_ESS_ES_MEASUREMENT_SAMPLING_FUNCTION_MINIMUM           (0x05)
#define QAPI_BLE_ESS_ES_MEASUREMENT_SAMPLING_FUNCTION_ACCUMULATED       (0x06)
#define QAPI_BLE_ESS_ES_MEASUREMENT_SAMPLING_FUNCTION_COUNT             (0x07)

   /* The following defines the valid values for the Measurement_Period */
   /* field of the qapi_BLE_ESS_ES_Measurement_t structure below.       */
   /* * NOTE * Any other value from 0x000001-0xFFFFFF is Time period in */
   /*          seconds.                                                 */
#define QAPI_BLE_ESS_ES_MEASUREMENT_MEASUREMENT_PERIOD_NOT_IN_USE       (0x000000)

   /* The following defines the valid values for the Update_Interval    */
   /* field of the qapi_BLE_ESS_ES_Measurement_t structure below.       */
   /* * NOTE * Any other value from 0x000001-0xFFFFFF is Time period in */
   /*          seconds.                                                 */
#define QAPI_BLE_ESS_ES_MEASUREMENT_UPDATE_INTERVAL_NOT_IN_USE          (0x000000)

   /* The following defines the valid values for the Application field  */
   /* of the qapi_BLE_ESS_ES_Measurement_t structure below. Specifies   */
   /* the intended application for the ESS Characteristic.              */
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_UNSPECIFIED                           (0x00)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_AIR                                   (0x01)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_WATER                                 (0x02)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_BAROMETRIC                            (0x03)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_SOIL                                  (0x04)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_INFRARED                              (0x05)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_MAP_DATABASE                          (0x06)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_BAROMETRIC_ELEVATION_SOURCE           (0x07)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_GPS_ONLY_ELEVATION_SOURCE             (0x08)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_GPS_AND_MAP_DATABASE_ELEVATION_SOURCE (0x09)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_VERTICAL_DATUM_ELEVATION_SOURCE       (0x0A)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_ONSHORE                               (0x0B)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_ONBOARD_VESSEL_OR_VEHICLE             (0x0C)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_FRONT                                 (0x0D)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_BACK_OR_REAR                          (0x0E)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_UPPER                                 (0x0F)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_LOWER                                 (0x10)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_PRIMARY                               (0x11)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_SECONDARY                             (0x12)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_OUTDOOR                               (0x13)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_INDOOR                                (0x14)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_TOP                                   (0x15)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_BOTTOM                                (0x16)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_MAIN                                  (0x17)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_BACKUP                                (0x18)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_AUXILIARY                             (0x19)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_SUPPLEMENTARY                         (0x1A)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_INSIDE                                (0x1B)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_OUTSIDE                               (0x1C)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_LEFT                                  (0x1D)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_RIGHT                                 (0x1E)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_INTERNAL                              (0x1F)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_EXTERNAL                              (0x20)
#define QAPI_BLE_ESS_ES_MEASUREMENT_APPLICATION_SOLAR                                 (0x21)

   /* The following defines the valid values for the Update_Interval    */
   /* field of the qapi_BLE_ESS_ES_Measurement_t structure below.       */
   /* * NOTE * Any other value from 0x00-0xFE is the maximum error from */
   /*          the actual value, expressedd as a percentage of the      */
   /*          reported value.                                          */
#define QAPI_BLE_ESS_ES_MEASUREMENT_UNCERTAINTY_INFO_NOT_AVAILABLE      (0xFF)

   /* The following defines the ESS Environmental Sensing Measurement   */
   /* structure.                                                        */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_ESS_ES_Measurement_s
{
   qapi_BLE_NonAlignedWord_t Flags;
   qapi_BLE_NonAlignedByte_t Sampling_Function;
   qapi_BLE_ESS_Int_24_t     Measurement_Period;
   qapi_BLE_ESS_Int_24_t     Update_Interval;
   qapi_BLE_NonAlignedByte_t Application;
   qapi_BLE_NonAlignedByte_t Measurement_Uncertainty;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_ESS_ES_Measurement_t;

#define QAPI_BLE_ESS_ES_MEASUREMENT_SIZE                                (sizeof(qapi_BLE_ESS_ES_Measurement_t))

   /* The following defines the valid values for the Condition_Value    */
   /* field of the qapi_BLE_ESS_ES_Trigger_Setting_Descriptor_t         */
   /* structure below.                                                  */
#define QAPI_BLE_ESS_ES_TRIGGER_SETTING_CONDITION_TRIGGER_INACTIVE                       (0x00)
#define QAPI_BLE_ESS_ES_TRIGGER_SETTING_CONDITION_FIXED_TIME_INTERVAL                    (0x01)
#define QAPI_BLE_ESS_ES_TRIGGER_SETTING_CONDITION_NO_LESS_THAN_SPECIFIED_TIME            (0x02)
#define QAPI_BLE_ESS_ES_TRIGGER_SETTING_CONDITION_VALUE_CHANGED                          (0x03)
#define QAPI_BLE_ESS_ES_TRIGGER_SETTING_CONDITION_LESS_THAN_SPECIFIED_VALUE              (0x04)
#define QAPI_BLE_ESS_ES_TRIGGER_SETTING_CONDITION_LESS_THAN_OR_EQUAL_SPECIFIED_VALUE     (0x05)
#define QAPI_BLE_ESS_ES_TRIGGER_SETTING_CONDITION_GREATER_THAN_SPECIFIED_VALUE           (0x06)
#define QAPI_BLE_ESS_ES_TRIGGER_SETTING_CONDITION_GREATER_THAN_OR_EQUAL_SPECIFIED_VALUE  (0x07)
#define QAPI_BLE_ESS_ES_TRIGGER_SETTING_CONDITION_EQUAL_TO_SPECIFIED_VALUE               (0x08)
#define QAPI_BLE_ESS_ES_TRIGGER_SETTING_CONDITION_NOT_EQUAL_TO_SPECIFIED_VALUE           (0x09)

   /* The following defines the ESS Environmental Sensing Trigger       */
   /* Setting descriptor structure.  The variable data MAY contain an   */
   /* operand (format type and units) depending on the Condition_Value  */
   /* field.                                                            */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_ESS_ES_Trigger_Setting_s
{
   qapi_BLE_NonAlignedByte_t Condition_Value;
   qapi_BLE_NonAlignedByte_t Variable_Data[1];
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_ESS_ES_Trigger_Setting_t;

#define QAPI_BLE_ESS_ES_TRIGGER_SETTING_SIZE                            (sizeof(qapi_BLE_ESS_ES_Trigger_Setting_t))

   /* The following defines the valid values that may be set for the ES */
   /* Configuration Descriptor.                                         */
#define QAPI_BLE_ESS_ES_TRIGGER_LOGIC_VALUE_BOOLEAN_AND                 (0x00)
#define QAPI_BLE_ESS_ES_TRIGGER_LOGIC_VALUE_BOOLEAN_OR                  (0x01)

   /* The following defines the valid values that may be set for the    */
   /* Flags field (bitmask) of the                                      */
   /* qapi_BLE_ESS_Descriptor_Value_Changed_t structure below.          */
#define QAPI_BLE_ESS_DESCRIPTOR_VALUE_CHANGED_FLAGS_SOURCE_OF_CHANGE_CLIENT              (0x01)
#define QAPI_BLE_ESS_DESCRIPTOR_VALUE_CHANGED_FLAGS_ES_TRIGGER_SETTING_CHANGED           (0x02)
#define QAPI_BLE_ESS_DESCRIPTOR_VALUE_CHANGED_FLAGS_ES_CONFIGURATION_DESCRIPTOR_CHANGED  (0x04)
#define QAPI_BLE_ESS_DESCRIPTOR_VALUE_CHANGED_FLAGS_ES_MEASUREMENT_DESCRIPTOR_CHANGED    (0x08)
#define QAPI_BLE_ESS_DESCRIPTOR_VALUE_CHANGED_FLAGS_USER_DESCRIPTION_CHANGED             (0x10)

   /* The following defines the ESS Descriptor Value Changed            */
   /* characteristic structure.                                         */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_ESS_Descriptor_Value_Changed_s
{
   qapi_BLE_NonAlignedWord_t Flags;
   qapi_BLE_NonAlignedByte_t Variable_Data[1];
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_ESS_Descriptor_Value_Changed_t;

#define QAPI_BLE_ESS_DESCRIPTOR_VALUE_CHANGED_SIZE                      (sizeof(qapi_BLE_ESS_Descriptor_Value_Changed_t))

#endif
