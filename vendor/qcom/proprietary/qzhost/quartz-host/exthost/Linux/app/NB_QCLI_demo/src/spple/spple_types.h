/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __SPPLETYPESH__
#define __SPPLETYPESH__

#include "qapi.h"

   /* The following MACRO is a utility MACRO that assigns the SPPLE     */
   /* Service 16 bit UUID to the specified qapi_BLE_UUID_128_t variable.*/
   /* This MACRO accepts one parameter which is a pointer to a          */
   /* qapi_BLE_UUID_128_t variable that is to receive the SPPLE UUID    */
   /* Constant value.                                                   */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_128_t   */
   /*          variable in Little-Endian format.                        */
#define SPPLE_ASSIGN_SPPLE_SERVICE_UUID_128(_x)                 QAPI_BLE_ASSIGN_BLUETOOTH_UUID_128(*((qapi_BLE_UUID_128_t *)(_x)), 0x14, 0x83, 0x9A, 0xC4, 0x7D, 0x7E, 0x41, 0x5c, 0x9A, 0x42, 0x16, 0x73, 0x40, 0xCF, 0x23, 0x39)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined SPPLE Service UUID in UUID16 form.  This   */
   /* MACRO only returns whether the qapi_BLE_UUID_128_t variable is    */
   /* equal to the SPPLE Service UUID (MACRO returns boolean result) NOT*/
   /* less than/greater than.  The first parameter is the               */
   /* qapi_BLE_UUID_128_t variable to compare to the SPPLE Service UUID.*/
#define SPPLE_COMPARE_SPPLE_SERVICE_UUID_TO_UUID_128(_x)        QAPI_BLE_COMPARE_BLUETOOTH_UUID_128_TO_CONSTANT((_x), 0x14, 0x83, 0x9A, 0xC4, 0x7D, 0x7E, 0x41, 0x5c, 0x9A, 0x42, 0x16, 0x73, 0x40, 0xCF, 0x23, 0x39)

   /* The following defines the SPPLE Service UUID that is used when    */
   /* building the SPPLE Service Table.                                 */
#define SPPLE_SERVICE_UUID_CONSTANT                      { 0x39, 0x23, 0xCF, 0x40, 0x73, 0x16, 0x42, 0x9A, 0x5c, 0x41, 0x7E, 0x7D, 0xC4, 0x9A, 0x83, 0x14 }

   /* The following MACRO is a utility MACRO that assigns the SPPLE TX  */
   /* Characteristic 16 bit UUID to the specified qapi_BLE_UUID_128_t   */
   /* variable.  This MACRO accepts one parameter which is the          */
   /* qapi_BLE_UUID_128_t variable that is to receive the SPPLE TX UUID */
   /* Constant value.                                                   */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_128_t   */
   /*          variable in Little-Endian format.                        */
#define SPPLE_ASSIGN_TX_UUID_128(_x)                            QAPI_BLE_ASSIGN_BLUETOOTH_UUID_128((_x), 0x07, 0x34, 0x59, 0x4A, 0xA8, 0xE7, 0x4b, 0x1a, 0xA6, 0xB1, 0xCD, 0x52, 0x43, 0x05, 0x9A, 0x57)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined SPPLE TX UUID in UUID16 form.  This MACRO  */
   /* only returns whether the qapi_BLE_UUID_128_t variable is equal to */
   /* the TX UUID (MACRO returns boolean result) NOT less than/greater  */
   /* than.  The first parameter is the qapi_BLE_UUID_128_t variable to */
   /* compare to the SPPLE TX UUID.                                     */
#define SPPLE_COMPARE_SPPLE_TX_UUID_TO_UUID_128(_x)             QAPI_BLE_COMPARE_BLUETOOTH_UUID_128_TO_CONSTANT((_x), 0x07, 0x34, 0x59, 0x4A, 0xA8, 0xE7, 0x4b, 0x1a, 0xA6, 0xB1, 0xCD, 0x52, 0x43, 0x05, 0x9A, 0x57)

   /* The following defines the SPPLE TX Characteristic UUID that is    */
   /* used when building the SPPLE Service Table.                       */
#define SPPLE_TX_CHARACTERISTIC_UUID_CONSTANT           { 0x57, 0x9A, 0x05, 0x43, 0x52, 0xCD, 0xB1, 0xA6, 0x1a, 0x4b, 0xE7, 0xA8, 0x4A, 0x59, 0x34, 0x07 }

   /* The following MACRO is a utility MACRO that assigns the SPPLE     */
   /* TX_CREDITS Characteristic 16 bit UUID to the specified            */
   /* qapi_BLE_UUID_128_t variable.  This MACRO accepts one parameter   */
   /* which is the qapi_BLE_UUID_128_t variable that is to receive the  */
   /* SPPLE TX_CREDITS UUID Constant value.                             */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_128_t   */
   /*          variable in Little-Endian format.                        */
#define SPPLE_ASSIGN_TX_CREDITS_UUID_128(_x)                    QAPI_BLE_ASSIGN_BLUETOOTH_UUID_128((_x), 0xBA, 0x04, 0xC4, 0xB2, 0x89, 0x2B, 0x43, 0xbe, 0xB6, 0x9C, 0x5D, 0x13, 0xF2, 0x19, 0x53, 0x92)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined SPPLE TX_CREDITS UUID in UUID16 form.  This*/
   /* MACRO only returns whether the qapi_BLE_UUID_128_t variable is    */
   /* equal to the TX_CREDITS UUID (MACRO returns boolean result) NOT   */
   /* less than/greater than.  The first parameter is the               */
   /* qapi_BLE_UUID_128_t variable to compare to the SPPLE TX_CREDITS   */
   /* UUID.                                                             */
#define SPPLE_COMPARE_SPPLE_TX_CREDITS_UUID_TO_UUID_128(_x)     QAPI_BLE_COMPARE_BLUETOOTH_UUID_128_TO_CONSTANT((_x), 0xBA, 0x04, 0xC4, 0xB2, 0x89, 0x2B, 0x43, 0xbe, 0xB6, 0x9C, 0x5D, 0x13, 0xF2, 0x19, 0x53, 0x92)

   /* The following defines the SPPLE TX_CREDITS Characteristic UUID    */
   /* that is used when building the SPPLE Service Table.               */
#define SPPLE_TX_CREDITS_CHARACTERISTIC_UUID_CONSTANT           { 0x92, 0x53, 0x19, 0xF2, 0x13, 0x5D, 0x9C, 0xB6, 0xbe, 0x43, 0x2B, 0x89, 0xB2, 0xC4, 0x04, 0xBA }

   /* The following MACRO is a utility MACRO that assigns the SPPLE RX  */
   /* Characteristic 16 bit UUID to the specified qapi_BLE_UUID_128_t   */
   /* variable.  This MACRO accepts one parameter which is the          */
   /* qapi_BLE_UUID_128_t variable that is to receive the SPPLE RX UUID */
   /* Constant value.                                                   */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_128_t   */
   /*          variable in Little-Endian format.                        */
#define SPPLE_ASSIGN_RX_UUID_128(_x)                            QAPI_BLE_ASSIGN_BLUETOOTH_UUID_128((_x), 0x8B, 0x00, 0xAC, 0xE7, 0xEB, 0x0B, 0x49, 0xb0, 0xBB, 0xE9, 0x9A, 0xEE, 0x0A, 0x26, 0xE1, 0xA3)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined SPPLE RX UUID in UUID16 form.  This MACRO  */
   /* only returns whether the qapi_BLE_UUID_128_t variable is equal to */
   /* the RX UUID (MACRO returns boolean result) NOT less than/greater  */
   /* than.  The first parameter is the qapi_BLE_UUID_128_t variable to */
   /* compare to the SPPLE RX UUID.                                     */
#define SPPLE_COMPARE_SPPLE_RX_UUID_TO_UUID_128(_x)             QAPI_BLE_COMPARE_BLUETOOTH_UUID_128_TO_CONSTANT((_x), 0x8B, 0x00, 0xAC, 0xE7, 0xEB, 0x0B, 0x49, 0xb0, 0xBB, 0xE9, 0x9A, 0xEE, 0x0A, 0x26, 0xE1, 0xA3)

   /* The following defines the SPPLE RX Characteristic UUID that is    */
   /* used when building the SPPLE Service Table.                       */
#define SPPLE_RX_CHARACTERISTIC_UUID_CONSTANT            { 0xA3, 0xE1, 0x26, 0x0A, 0xEE, 0x9A, 0xE9, 0xBB, 0xb0, 0x49, 0x0B, 0xEB, 0xE7, 0xAC, 0x00, 0x8B }

   /* The following MACRO is a utility MACRO that assigns the SPPLE     */
   /* RX_CREDITS Characteristic 16 bit UUID to the specified            */
   /* qapi_BLE_UUID_128_t variable.  This MACRO accepts one parameter   */
   /* which is the qapi_BLE_UUID_128_t variable that is to receive the  */
   /* SPPLE RX_CREDITS UUID Constant value.                             */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_128_t   */
   /*          variable in Little-Endian format.                        */
#define SPPLE_ASSIGN_RX_CREDITS_UUID_128(_x)                    QAPI_BLE_ASSIGN_BLUETOOTH_UUID_128((_x), 0xE0, 0x6D, 0x5E, 0xFB, 0x4F, 0x4A, 0x45, 0xc0, 0x9E, 0xB1, 0x37, 0x1A, 0xE5, 0xA1, 0x4A, 0xD4)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined SPPLE RX_CREDITS UUID in UUID16 form.  This*/
   /* MACRO only returns whether the qapi_BLE_UUID_128_t variable is    */
   /* equal to the RX_CREDITS UUID (MACRO returns boolean result) NOT   */
   /* less than/greater than.  The first parameter is the               */
   /* qapi_BLE_UUID_128_t variable to compare to the SPPLE RX_CREDITS   */
   /* UUID.                                                             */
#define SPPLE_COMPARE_SPPLE_RX_CREDITS_UUID_TO_UUID_128(_x)     QAPI_BLE_COMPARE_BLUETOOTH_UUID_128_TO_CONSTANT((_x), 0xE0, 0x6D, 0x5E, 0xFB, 0x4F, 0x4A, 0x45, 0xc0, 0x9E, 0xB1, 0x37, 0x1A, 0xE5, 0xA1, 0x4A, 0xD4)

   /* The following defines the SPPLE RX_CREDITS Characteristic UUID    */
   /* that is used when building the SPPLE Service Table.               */
#define SPPLE_RX_CREDITS_CHARACTERISTIC_UUID_CONSTANT           { 0xD4, 0x4A, 0xA1, 0xE5, 0x1A, 0x37, 0xB1, 0x9E, 0xc0, 0x45, 0x4A, 0x4F, 0xFB, 0x5E, 0x6D, 0xE0 }

   /* The following defines the structure that holds all of the SPPLE   */
   /* Characteristic Handles that need to be cached by a SPPLE Client.  */
typedef struct _tagSPPLE_Client_Info_t
{
   uint16_t Tx_Characteristic;
   uint16_t Tx_Client_Configuration_Descriptor;
   uint16_t Rx_Characteristic;
   uint16_t Tx_Credit_Characteristic;
   uint16_t Rx_Credit_Characteristic;
   uint16_t Rx_Credit_Client_Configuration_Descriptor;
} SPPLE_Client_Info_t;

#define SPPLE_CLIENT_INFO_DATA_SIZE                      (sizeof(SPPLE_Client_Info_t))

#define SPPLE_CLIENT_INFORMATION_VALID(_x)               (((_x).Tx_Characteristic) && ((_x).Tx_Client_Configuration_Descriptor) && ((_x).Rx_Characteristic) && ((_x).Tx_Credit_Characteristic) && ((_x).Rx_Credit_Characteristic) && ((_x).Rx_Credit_Client_Configuration_Descriptor))

   /* The following defines the structure that holds the information    */
   /* that needs to be cached by a SPPLE Server for EACH paired SPPLE   */
   /* Client.                                                           */
typedef struct _tagSPPLE_Server_Info_t
{
   uint16_t Tx_Client_Configuration_Descriptor;
   uint16_t Rx_Credit_Client_Configuration_Descriptor;
} SPPLE_Server_Info_t;

#define SPPLE_SERVER_INFO_DATA_SIZE                      (sizeof(SPPLE_Server_Info_t))

   /* The following defines the length of the SPPLE CTS characteristic  */
   /* value.                                                            */
#define SPPLE_TX_CREDIT_VALUE_LENGTH                     (sizeof(uint16_t))

   /* The following defines the length of the SPPLE RTS characteristic  */
   /* value.                                                            */
#define SPPLE_RX_CREDIT_VALUE_LENGTH                     (sizeof(uint16_t))

   /* The following defines the length of the Client Characteristic     */
   /* Configuration Descriptor.                                         */
#define SPPLE_CLIENT_CHARACTERISTIC_CONFIGURATION_VALUE_LENGTH (sizeof(uint16_t))

   /* The following defines the SPPLE GATT Service Flags MASK that      */
   /* should be passed into GATT_Register_Service when the HRS Service  */
   /* is registered.                                                    */
#define SPPLE_SERVICE_FLAGS                              (QAPI_BLE_GATT_SERVICE_FLAGS_LE_SERVICE)

#endif

