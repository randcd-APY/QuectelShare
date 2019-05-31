/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef __QAPI_BLE_IPS_TYPES_H__
#define __QAPI_BLE_IPS_TYPES_H__

#include "./qapi_ble_bttypes.h" /* Bluetooth Type Definitions.          */

   /* The following defines the IPS Attribute Protocol (ATT) Application*/
   /* Error Codes that may be sent in a GATT response.                  */
   /* * NOTE * The Attribute Protocol (ATT) Error Codes have the form   */
   /*          QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX found in            */
   /*          qapi_ble_atttypes.h and may also be sent in a GATT       */
   /*          response.                                                */
#define QAPI_BLE_IPS_ERROR_CODE_SUCCESS                                 0x00
#define QAPI_BLE_IPS_ERROR_CODE_INVALID_VALUE                           0x80

   /* The following MACRO is a utility MACRO that assigns the IPS       */
   /* Service 16 bit UUID to the specified qapi_BLE_UUID_16_t variable. */
   /* This MACRO accepts one parameter which is a pointer to a          */
   /* qapi_BLE_UUID_16_t variable that is to receive the IPS UUID       */
   /* Constant value.                                                   */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_IPS_ASSIGN_IPS_SERVICE_UUID_16(_x)                     QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16(*((qapi_BLE_UUID_16_t *)(_x)), 0x18, 0x21)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined IPS Service UUID in UUID16 form. This MACRO*/
   /* only returns whether the qapi_BLE_UUID_16_t variable is equal to  */
   /* the IPS Service UUID (MACRO returns boolean result) NOT less      */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare to the IPS Service UUID.                      */
#define QAPI_BLE_IPS_COMPARE_IPS_SERVICE_UUID_TO_UUID_16(_x)            QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x21)

   /* The following defines the IPS Service UUID that is used when      */
   /* building the IPS Service Table.                                   */
#define QAPI_BLE_IPS_SERVICE_UUID_CONSTANT                              { 0x21, 0x18 }

   /* The following MACRO is a utility MACRO that assigns the IPS       */
   /* Service 16 bit UUID to the specified qapi_BLE_UUID_16_t variable. */
   /* This MACRO accepts one parameter which is a qapi_BLE_UUID_16_t    */
   /* variable that is to receive the IPS UUID Constant value.          */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Big-Endian format.                           */
#define QAPI_BLE_IPS_ASSIGN_IPS_SERVICE_SDP_UUID_16(_x)                 QAPI_BLE_ASSIGN_SDP_UUID_16((_x), 0x18, 0x21)

   /* The following MACRO is a utility MACRO that assigns the IPS Indoor*/
   /* Positioning Configuration Characteristic 16 bit UUID to the       */
   /* specified qapi_BLE_UUID_16_t variable. This MACRO accepts one     */
   /* parameter which is the qapi_BLE_UUID_16_t variable that is to     */
   /* receive the IPS Indoor Positioning Configuration UUID Constant    */
   /* Value.                                                            */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_IPS_ASSIGN_INDOOR_POSITIONING_CONFIGURATION_UUID_16(_x)            QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0xAD)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined IPS Indoor Positioning Configuration UUID  */
   /* in UUID16 form. This MACRO only returns whether the               */
   /* qapi_BLE_UUID_16_t variable is equal to the IPS Indoor Positioning*/
   /* Configuration UUID (MACRO returns boolean result) NOT less        */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare to the IPS Indoor Positioning Configuration   */
   /* UUID.                                                             */
#define QAPI_BLE_IPS_COMPARE_INDOOR_POSITIONING_CONFIGURATION_UUID_TO_UUID_16(_x)   QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0xAD)

   /* The following defines the IPS Indoor Positioning Configuration    */
   /* Characteristic UUID that is used when building the IPS Service    */
   /* Table.                                                            */
#define QAPI_BLE_IPS_INDOOR_POSITIONING_CONFIGURATION_CHARACTERISTIC_UUID_CONSTANT  { 0xAD, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the IPS       */
   /* Latitude Characteristic 16 bit UUID to the specified              */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* IPS Latitude UUID Constant value.                                 */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_IPS_ASSIGN_LATITUDE_UUID_16(_x)                        QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0xAE)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined IPS Latitude UUID in UUID16 form. This     */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the IPS Latitude UUID (MACRO returns boolean result) NOT */
   /* less than/greater than. The first parameter is the                */
   /* qapi_BLE_UUID_16_t variable to compare to the IPS Latitude UUID.  */
#define QAPI_BLE_IPS_COMPARE_LATITUDE_UUID_TO_UUID_16(_x)               QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0xAE)

   /* The following defines the IPS Latitude Characteristic UUID that is*/
   /* used when building the IPS Service Table.                         */
#define QAPI_BLE_IPS_LATITUDE_CHARACTERISTIC_UUID_CONSTANT              { 0xAE, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the IPS       */
   /* Longitude Characteristic 16 bit UUID to the specified             */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* IPS Longitude UUID Constant value.                                */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_IPS_ASSIGN_LONGITUDE_UUID_16(_x)                       QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0xAF)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined IPS Longitude UUID in UUID16 form. This    */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the IPS Longitude UUID (MACRO returns boolean result) NOT*/
   /* less than/greater than. The first parameter is the                */
   /* qapi_BLE_UUID_16_t variable to compare to the IPS Longitude UUID. */
#define QAPI_BLE_IPS_COMPARE_LONGITUDE_UUID_TO_UUID_16(_x)              QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0xAF)

   /* The following defines the IPS Longitude Characteristic UUID that  */
   /* is used when building the IPS Service Table.                      */
#define QAPI_BLE_IPS_LONGITUDE_CHARACTERISTIC_UUID_CONSTANT             { 0xAF, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the IPS Local */
   /* North Coordinate Characteristic 16 bit UUID to the specified      */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* IPS Local North Coordinate UUID Constant value.                   */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_IPS_ASSIGN_LOCAL_NORTH_COORDINATE_UUID_16(_x)            QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0xB0)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined IPS Local North Coordinate UUID in UUID16  */
   /* form. This MACRO only returns whether the qapi_BLE_UUID_16_t      */
   /* variable is equal to the IPS Local North Coordinate UUID (MACRO   */
   /* returns boolean result) NOT less than/greater than. The first     */
   /* parameter is the qapi_BLE_UUID_16_t variable to compare to the IPS*/
   /* Local North Coordinate UUID.                                      */
#define QAPI_BLE_IPS_COMPARE_LOCAL_NORTH_COORDINATE_UUID_TO_UUID_16(_x)   QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0xB0)

   /* The following defines the IPS Local North Coordinate              */
   /* Characteristic UUID that is used when building the IPS Service    */
   /* Table.                                                            */
#define QAPI_BLE_IPS_LOCAL_NORTH_COORDINATE_CHARACTERISTIC_UUID_CONSTANT  { 0xB0, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the IPS Local */
   /* East Coordinate Characteristic 16 bit UUID to the specified       */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* IPS Local East Coordinate UUID Constant value.                    */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_IPS_ASSIGN_LOCAL_EAST_COORDINATE_UUID_16(_x)            QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0xB1)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined IPS Local East Coordinate UUID in UUID16   */
   /* form. This MACRO only returns whether the qapi_BLE_UUID_16_t      */
   /* variable is equal to the IPS Local East Coordinate UUID (MACRO    */
   /* returns boolean result) NOT less than/greater than. The first     */
   /* parameter is the qapi_BLE_UUID_16_t variable to compare to the IPS*/
   /* Local East Coordinate UUID.                                       */
#define QAPI_BLE_IPS_COMPARE_LOCAL_EAST_COORDINATE_UUID_TO_UUID_16(_x)   QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0xB1)

   /* The following defines the IPS Local East Coordinate Characteristic*/
   /* UUID that is used when building the IPS Service Table.            */
#define QAPI_BLE_IPS_LOCAL_EAST_COORDINATE_CHARACTERISTIC_UUID_CONSTANT  { 0xB1, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the IPS Floor */
   /* Number Characteristic 16 bit UUID to the specified                */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* IPS Floor Number UUID Constant value.                             */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_IPS_ASSIGN_FLOOR_NUMBER_UUID_16(_x)                    QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0xB2)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined IPS Floor Number UUID in UUID16 form. This */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the IPS Floor Number UUID (MACRO returns boolean result) */
   /* NOT less than/greater than. The first parameter is the            */
   /* qapi_BLE_UUID_16_t variable to compare to the IPS Floor Number    */
   /* UUID.                                                             */
#define QAPI_BLE_IPS_COMPARE_FLOOR_NUMBER_UUID_TO_UUID_16(_x)           QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0xB2)

   /* The following defines the IPS Floor Number Characteristic UUID    */
   /* that is used when building the IPS Service Table.                 */
#define QAPI_BLE_IPS_FLOOR_NUMBER_CHARACTERISTIC_UUID_CONSTANT          { 0xB2, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the IPS       */
   /* Altitude Characteristic 16 bit UUID to the specified              */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* IPS Altitude UUID Constant value.                                 */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_IPS_ASSIGN_ALTITUDE_UUID_16(_x)                        QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0xB3)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined IPS Altitude UUID in UUID16 form. This     */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the IPS Altitude UUID (MACRO returns boolean result) NOT */
   /* less than/greater than. The first parameter is the                */
   /* qapi_BLE_UUID_16_t variable to compare to the IPS Altitude UUID.  */
#define QAPI_BLE_IPS_COMPARE_ALTITUDE_UUID_TO_UUID_16(_x)               QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0xB3)

   /* The following defines the IPS Altitude Characteristic UUID that is*/
   /* used when building the IPS Service Table.                         */
#define QAPI_BLE_IPS_ALTITUDE_CHARACTERISTIC_UUID_CONSTANT              { 0xB3, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the IPS       */
   /* Uncertainty Characteristic 16 bit UUID to the specified           */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* IPS Uncertainty UUID Constant value.                              */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_IPS_ASSIGN_UNCERTAINTY_UUID_16(_x)                     QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0xB4)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined IPS Uncertainty UUID in UUID16 form. This  */
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the IPS Uncertainty UUID (MACRO returns boolean result)  */
   /* NOT less than/greater than. The first parameter is the            */
   /* qapi_BLE_UUID_16_t variable to compare to the IPS Uncertainty     */
   /* UUID.                                                             */
#define QAPI_BLE_IPS_COMPARE_UNCERTAINTY_UUID_TO_UUID_16(_x)            QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0xB4)

   /* The following defines the IPS Uncertainty Characteristic UUID that*/
   /* is used when building the IPS Service Table.                      */
#define QAPI_BLE_IPS_UNCERTAINTY_CHARACTERISTIC_UUID_CONSTANT           { 0xB4, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the IPS       */
   /* Location Name Characteristic 16 bit UUID to the specified         */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* IPS Location Name UUID Constant value.                            */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_IPS_ASSIGN_LOCATION_NAME_UUID_16(_x)                   QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0xB5)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined IPS Location Name UUID in UUID16 form. This*/
   /* MACRO only returns whether the qapi_BLE_UUID_16_t variable is     */
   /* equal to the IPS Location Name UUID (MACRO returns boolean result)*/
   /* NOT less than/greater than. The first parameter is the            */
   /* qapi_BLE_UUID_16_t variable to compare to the IPS Location Name   */
   /* UUID.                                                             */
#define QAPI_BLE_IPS_COMPARE_LOCATION_NAME_UUID_TO_UUID_16(_x)          QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0xB5)

   /* The following defines the IPS Location Name Characteristic UUID   */
   /* that is used when building the IPS Service Table.                 */
#define QAPI_BLE_IPS_LOCATION_NAME_CHARACTERISTIC_UUID_CONSTANT         { 0xB5, 0x2A }

   /* The following defines the values that can be set for              */
   /* characteristics that are no configured.                           */
   /* * NOTE * Other characteristic values will be initialized to zero  */
   /*          when the service is registered.  These include the Indoor*/
   /*          Positioning Configuration, Uncertainty, and the Location */
   /*          Name characteristics.  Location Name will be empty string*/
   /*          have zero length if not configured.                      */
#define QAPI_BLE_IPS_LATITUDE_NOT_CONFIGURED                            (0x80000000)
#define QAPI_BLE_IPS_LONGITUDE_NOT_CONFIGURED                           (0x80000000)
#define QAPI_BLE_IPS_LOCAL_COORDINATE_NOT_CONFIGURED                    (0x8000)
#define QAPI_BLE_IPS_FLOOR_NUMBER_NOT_CONFIGURED                        (255)
#define QAPI_BLE_IPS_ALTITUDE_NOT_CONFIGURED                            (0xFFFF)

   /* The following defines the possible values for the IPS Indoor      */
   /* Positioning Configuration characteristic bit mask.  This values   */
   /* indicate the optional fields that may be included in advertising  */
   /* packets.                                                          */
#define QAPI_BLE_IPS_INDOOR_POSITIONING_CONFIG_COORDINATES_PRESENT      (0x01)
#define QAPI_BLE_IPS_INDOOR_POSITIONING_CONFIG_LOCAL_COORDINATES_USED   (0x02)
#define QAPI_BLE_IPS_INDOOR_POSITIONING_CONFIG_TX_POWER_PRESENT         (0x04)
#define QAPI_BLE_IPS_INDOOR_POSITIONING_CONFIG_ALTITUDE_PRESENT         (0x08)
#define QAPI_BLE_IPS_INDOOR_POSITIONING_CONFIG_FLOOR_NUMBER_PRESENT     (0x10)
#define QAPI_BLE_IPS_INDOOR_POSITIONING_CONFIG_UNCERTAINTY_PRESENT      (0x20)
#define QAPI_BLE_IPS_INDOOR_POSITIONING_CONFIG_LOCATION_NAME_AVAILABLE  (0x40)
#define QAPI_BLE_IPS_INDOOR_POSITIONING_CONFIG_RESERVED                 (0x80)

   /* The following defines the upper and lower bounds for the encoded  */
   /* Latitude N.  If the bounds are exceeded then the closest value    */
   /* MUST be used.  These precomputed values makes it easier to check  */
   /* the bounds.                                                       */
#define QAPI_BLE_IPS_ENCODED_LATITUDE_LOWER_BOUND                       (0x80000001)
#define QAPI_BLE_IPS_ENCODED_LATITUDE_UPPER_BOUND                       (0x7FFFFFFF)

   /* The following defines the upper and lower bounds for the encoded  */
   /* Latitude N.  If the bounds are exceeded then the closest value    */
   /* MUST be used.  These precomputed values makes it easier to check  */
   /* the bounds.                                                       */
#define QAPI_BLE_IPS_ENCODED_LONGITUDE_LOWER_BOUND                      (0x80000001)
#define QAPI_BLE_IPS_ENCODED_LONGITUDE_UPPER_BOUND                      (0x7FFFFFFF)

   /* The following defines the Local Coordinate Characteristics upper  */
   /* and lower range in decimeters (base 10 SINT16).                   */
#define QAPI_BLE_IPS_ENCODED_LOCAL_COORDINATES_RANGE_LOWER              (-32767)
#define QAPI_BLE_IPS_ENCODED_LOCAL_COORDINATES_RANGE_UPPER              (32767)

   /* The following defines the Altitude Characteristic's MAX and Min   */
   /* range.                                                            */
   /* * NOTE * The Maximum Altitude value is 64534 decimeters.          */
   /*          Repesented by the encoding below.                        */
   /* * NOTE * The Minimum Altitude value is -1000 decimeters.          */
   /*          Repesented by the encoding below.                        */
#define QAPI_BLE_IPS_ENCODED_ALTITUDE_GREATER_THAN_OR_EQUAL_TO_MAX      (65534)
#define QAPI_BLE_IPS_ENCODED_ALTITUDE_LESS_THAN_OR_EQUAL_TO_MIN         (0)

   /* The following defines the possible bit values for the Uncertainty */
   /* characteristic bit mask.                                          */
   /* * NOTE * Only the following can be directly assigned.  The other  */
   /*          bits have special meaning and MUST be set using the      */
   /*          MACROS below.                                            */
#define QAPI_BLE_IPS_UNCERTAINTY_MOBILE                                 (0x01)

   /* * NOTE * Only one value may be used for                           */
   /*          QAPI_BLE_IPS_UNCERTAINTY_UPDATE_TIME_XXX. This value may */
   /*          be passed to the MACRO below to set the value in the bit */
   /*          mask for the Uncertainty characteristic.                 */
#define QAPI_BLE_IPS_UNCERTAINTY_UPDATE_TIME_UP_TO_3_SEC                (0)
#define QAPI_BLE_IPS_UNCERTAINTY_UPDATE_TIME_UP_TO_4_SEC                (1)
#define QAPI_BLE_IPS_UNCERTAINTY_UPDATE_TIME_UP_TO_6_SEC                (2)
#define QAPI_BLE_IPS_UNCERTAINTY_UPDATE_TIME_UP_TO_12_SEC               (3)
#define QAPI_BLE_IPS_UNCERTAINTY_UPDATE_TIME_UP_TO_28_SEC               (4)
#define QAPI_BLE_IPS_UNCERTAINTY_UPDATE_TIME_UP_TO_89_SEC               (5)
#define QAPI_BLE_IPS_UNCERTAINTY_UPDATE_TIME_UP_TO_426_SEC              (6)
#define QAPI_BLE_IPS_UNCERTAINTY_UPDATE_TIME_UP_TO_3541_SEC             (7)

#define QAPI_BLE_IPS_UNCERTAINTY_UPDATE_TIME_BITS                       (0x0E)

   /* The following is a MACRO to set the update time for the           */
   /* Uncertainty characteristic. This MACRO takes the Uncertainty      */
   /* characteristic to update as the _X parameter. This MACRO takes a  */
   /* flag above QAPI_BLE_IPS_UNCERTAINTY_UPDATE_TIME_XXX to set the    */
   /* bits 1-3 with the Update time bits.                               */
#define QAPI_BLE_IPS_SET_UNCERTAINTY_UPDATE_TIME_BITS(_x, _y)           (((_x) |= ((_y) << 1)))

   /* The following is a MACRO to decode the update time for the        */
   /* Uncertainty characteristic.  This MACRO takes the Uncertainty     */
   /* characteristic to decode as the _X parameter.  This MACRO will    */
   /* clear the other bits and return the value defined above.          */
#define QAPI_BLE_IPS_DECODE_UNCERTAINTY_UPDATE_TIME_BITS(_x)            ((((_x) & 0x0E) >> 1))

   /* * NOTE * Only one value may be used for                           */
   /*          QAPI_BLE_IPS_UNCERTAINTY_PRECISION_XXX. This value may be*/
   /*          passed to the MACRO below to set the value in the bit    */
   /*          mask for the Uncertainty characteristic.                 */
#define QAPI_BLE_IPS_UNCERTAINTY_PRECISION_LESS_THAN_POINT_1_M          (0)
#define QAPI_BLE_IPS_UNCERTAINTY_PRECISION_POINT_1_TO_1_M               (1)
#define QAPI_BLE_IPS_UNCERTAINTY_PRECISION_1_TO_2_M                     (2)
#define QAPI_BLE_IPS_UNCERTAINTY_PRECISION_2_TO_5_M                     (3)
#define QAPI_BLE_IPS_UNCERTAINTY_PRECISION_5_TO_10_M                    (4)
#define QAPI_BLE_IPS_UNCERTAINTY_PRECISION_10_TO_50_M                   (5)
#define QAPI_BLE_IPS_UNCERTAINTY_PRECISION_GREATER_THAN_50_M            (6)

#define QAPI_BLE_IPS_UNCERTAINTY_PRECISION_BITS                         (0x70)

   /* The following is a MACRO to set the update time for the           */
   /* Uncertainty characteristic. This MACRO takes the Uncertainty      */
   /* characteristic to update as the _X parameter. This MACRO takes a  */
   /* flag above QAPI_BLE_IPS_UNCERTAINTY_UPDATE_TIME_XXX as parameter  */
   /* _y to set the bits 4-6 with the precision bits.                   */
#define QAPI_BLE_IPS_SET_UNCERTAINTY_PRECISION_BITS(_x, _y)             (((_x) |= ((_y) << 4)))

   /* The following is a MACRO to decode the precision for the          */
   /* Uncertainty characteristic.  This MACRO takes the Uncertainty     */
   /* characteristic to decode as the _X parameter.  This MACRO will    */
   /* clear the other bits and return the value defined above.          */
#define QAPI_BLE_IPS_DECODE_UNCERTAINTY_PRECISION_BITS(_x)              ((((_x) & 0x30) >> 4))

   /* The following is a hard coded value for the maximum size of the   */
   /* Broadcast data.                                                   */
#define QAPI_BLE_IPS_MAXIMUM_BROADCAST_DATA_SIZE                        (16)

   /* The following MACRO is used to determine the Length field of the  */
   /* broadcast data based on the Indoor Positioning Configuration.  The*/
   /* only parameter to this MACRO is the Indoor Positioning            */
   /* Configuration.                                                    */
#define QAPI_BLE_IPS_CALCULATE_BROADCAST_DATA_LENGTH(_x)       ((QAPI_BLE_NON_ALIGNED_BYTE_SIZE)                                                                                                               + \
                                                               (((_x))                                                               ?  QAPI_BLE_NON_ALIGNED_BYTE_SIZE : 0)                                    + \
                                                               (((_x) & QAPI_BLE_IPS_INDOOR_POSITIONING_CONFIG_COORDINATES_PRESENT)  ? (((_x) & QAPI_BLE_IPS_INDOOR_POSITIONING_CONFIG_LOCAL_COORDINATES_USED) ? QAPI_BLE_NON_ALIGNED_SDWORD_SIZE : QAPI_BLE_NON_ALIGNED_SQWORD_SIZE) : 0) + \
                                                               (((_x) & QAPI_BLE_IPS_INDOOR_POSITIONING_CONFIG_TX_POWER_PRESENT)     ?  QAPI_BLE_NON_ALIGNED_BYTE_SIZE : 0)                                    + \
                                                               (((_x) & QAPI_BLE_IPS_INDOOR_POSITIONING_CONFIG_FLOOR_NUMBER_PRESENT) ?  QAPI_BLE_NON_ALIGNED_BYTE_SIZE : 0)                                    + \
                                                               (((_x) & QAPI_BLE_IPS_INDOOR_POSITIONING_CONFIG_ALTITUDE_PRESENT)     ?  QAPI_BLE_NON_ALIGNED_WORD_SIZE : 0)                                    + \
                                                               (((_x) & QAPI_BLE_IPS_INDOOR_POSITIONING_CONFIG_UNCERTAINTY_PRESENT)  ?  QAPI_BLE_NON_ALIGNED_BYTE_SIZE : 0))

#endif
