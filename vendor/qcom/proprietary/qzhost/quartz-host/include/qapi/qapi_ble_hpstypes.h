/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef __QAPI_BLE_HPS_TYPES_H__
#define __QAPI_BLE_HPS_TYPES_H__

#include "./qapi_ble_bttypes.h" /* Bluetooth Type Definitions.          */

   /* The following defines the HPS Attribute Protocol (ATT) Application*/
   /* Error Codes that may be sent in a GATT response.                  */
   /* * NOTE * The Attribute Protocol (ATT) Error Codes have the form   */
   /*          QAPI_BLE_ATT_PROTOCOL_ERROR_CODE_XXX found in            */
   /*          qapi_ble_atttypes.h and may also be sent in a GATT       */
   /*          response.                                                */
#define QAPI_BLE_HPS_ERROR_CODE_SUCCESS                                 0x00
#define QAPI_BLE_HPS_ERROR_CODE_INVALID_REQUEST                         0x81
#define QAPI_BLE_HPS_ERROR_CODE_NETWORK_NOT_AVAILABLE                   0x82

   /* The following MACRO is a utility MACRO that assigns the HPS       */
   /* Service 16 bit UUID to the specified qapi_BLE_UUID_16_t variable. */
   /* This MACRO accepts one parameter which is a pointer to a          */
   /* qapi_BLE_UUID_16_t variable that is to receive the HPS UUID       */
   /* Constant value.                                                   */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_HPS_ASSIGN_HPS_SERVICE_UUID_16(_x)                     QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16(*((qapi_BLE_UUID_16_t *)(_x)), 0x18, 0x23)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined HPS Service UUID in UUID16 form. This MACRO*/
   /* only returns whether the qapi_BLE_UUID_16_t variable is equal to  */
   /* the HPS Service UUID (MACRO returns boolean result) NOT less      */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare to the HPS Service UUID.                      */
#define QAPI_BLE_HPS_COMPARE_HPS_SERVICE_UUID_TO_UUID_16(_x)            QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x18, 0x23)

   /* The following MACRO is a utility MACRO that assigns the HPS       */
   /* Service 16 bit UUID to the specified qapi_BLE_UUID_16_t variable. */
   /* This MACRO accepts one parameter which is a qapi_BLE_UUID_16_t    */
   /* variable that is to receive the HPS UUID Constant value.          */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Big-Endian format.                           */
#define QAPI_BLE_HPS_ASSIGN_HPS_SERVICE_SDP_UUID_16(_x)                 QAPI_BLE_ASSIGN_SDP_UUID_16((_x), 0x18, 0x23)

   /* The following defines the HPS Service UUID that is used when      */
   /* building the HPS Service Table.                                   */
#define QAPI_BLE_HPS_SERVICE_CHARACTERISTIC_UUID_CONSTANT               { 0x23, 0x18 }

   /* The following MACRO is a utility MACRO that assigns the HPS       */
   /* Uniform Resource Indentifier (URI) Characteristic 16 bit UUID to  */
   /* the specified qapi_BLE_UUID_16_t variable. This MACRO accepts one */
   /* parameter which is the qapi_BLE_UUID_16_t variable that is to     */
   /* receive the HPS Uniform Resource Indentifier (URI) Characteristic */
   /* UUID Constant Value.                                              */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_HPS_ASSIGN_URI_UUID_16(_x)                             QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0xB6)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined HPS Uniform Resource Indentifier           */
   /* Characteristic UUID in UUID16 form. This MACRO only returns       */
   /* whether the qapi_BLE_UUID_16_t variable is equal to the HPS       */
   /* Uniform Resource Indentifier (URI) Characteristic UUID (MACRO     */
   /* returns boolean result) NOT less than/greater than. The first     */
   /* parameter is the qapi_BLE_UUID_16_t variable to compare to the HPS*/
   /* Uniform Resource Indentifier (URI) Characteristic UUID.           */
#define QAPI_BLE_HPS_COMPARE_URI_UUID_TO_UUID_16(_x)                    QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0xB6)

   /* The following defines the HPS Uniform Resource Indentifier (URI)  */
   /* Characteristic UUID that is used when building the HPS Service    */
   /* Table.                                                            */
#define QAPI_BLE_HPS_URI_CHARACTERISTIC_UUID_CONSTANT                   { 0xB6, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the HPS HTTP  */
   /* Headers Characteristic 16 bit UUID to the specified               */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* HPS HTTP Headers Characteristic UUID Constant Value.              */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_HPS_ASSIGN_HTTP_HEADERS_UUID_16(_x)                    QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0xB7)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined HPS HTTP Headers Characteristic UUID in    */
   /* UUID16 form. This MACRO only returns whether the                  */
   /* qapi_BLE_UUID_16_t variable is equal to the HPS HTTP Headers      */
   /* Characteristic UUID (MACRO returns boolean result) NOT less       */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare to the HPS HTTP Headers Characteristic UUID.  */
#define QAPI_BLE_HPS_COMPARE_HTTP_HEADERS_UUID_TO_UUID_16(_x)           QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0xB7)

   /* The following defines the HPS HTTP Headers Characteristic UUID    */
   /* that is used when building the HPS Service Table.                 */
#define QAPI_BLE_HPS_HTTP_HEADERS_CHARACTERISTIC_UUID_CONSTANT          { 0xB7, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the HPS HTTP  */
   /* Entity Body Characteristic 16 bit UUID to the specified           */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* HPS HTTP Entity Body Characteristic UUID Constant Value.          */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_HPS_ASSIGN_HTTP_ENTITY_BODY_UUID_16(_x)                QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0xB9)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined HPS HTTP Entity Body Characteristic UUID in*/
   /* UUID16 form. This MACRO only returns whether the                  */
   /* qapi_BLE_UUID_16_t variable is equal to the HPS HTTP Entity Body  */
   /* Characteristic UUID (MACRO returns boolean result) NOT less       */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare to the HPS HTTP Entity Body Characteristic    */
   /* UUID.                                                             */
#define QAPI_BLE_HPS_COMPARE_HTTP_ENTITY_BODY_UUID_TO_UUID_16(_x)       QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0xB9)

   /* The following defines the HPS HTTP Entity Body Characteristic UUID*/
   /* that is used when building the HPS Service Table.                 */
#define QAPI_BLE_HPS_HTTP_ENTITY_BODY_CHARACTERISTIC_UUID_CONSTANT      { 0xB9, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the HPS HTTP  */
   /* Control point Characteristic 16 bit UUID to the specified         */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* HPS HTTP Control point Characteristic UUID Constant Value.        */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_HPS_ASSIGN_HTTP_CONTROL_POINT_UUID_16(_x)              QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0xBA)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined HPS HTTP Control point Characteristic UUID */
   /* in UUID16 form. This MACRO only returns whether the               */
   /* qapi_BLE_UUID_16_t variable is equal to the HPS HTTP Control point*/
   /* Characteristic UUID (MACRO returns boolean result) NOT less       */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare to the HPS HTTP Control point Characteristic  */
   /* UUID.                                                             */
#define QAPI_BLE_HPS_COMPARE_HTTP_CONTROL_POINT_UUID_TO_UUID_16(_x)     QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0xBA)

   /* The following defines the HPS HTTP Control point Characteristic   */
   /* UUID that is used when building the HPS Service Table.            */
#define QAPI_BLE_HPS_HTTP_CONTROL_POINT_CHARACTERISTIC_UUID_CONSTANT    { 0xBA, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the HPS HTTP  */
   /* Status Code Characteristic 16 bit UUID to the specified           */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* HPS HTTP Status Code Characteristic UUID Constant Value.          */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_HPS_ASSIGN_HTTP_STATUS_CODE_UUID_16(_x)                QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0xB8)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined HPS HTTP Status Code Characteristic UUID in*/
   /* UUID16 form. This MACRO only returns whether the                  */
   /* qapi_BLE_UUID_16_t variable is equal to the HPS HTTP Status Code  */
   /* Characteristic UUID (MACRO returns boolean result) NOT less       */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare to the HPS HTTP Status Code Characteristic    */
   /* UUID.                                                             */
#define QAPI_BLE_HPS_COMPARE_HTTP_STATUS_CODE_UUID_TO_UUID_16(_x)       QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0xB8)

   /* The following defines the HPS HTTP Status Code Characteristic UUID*/
   /* that is used when building the HPS Service Table.                 */
#define QAPI_BLE_HPS_HTTP_STATUS_CODE_CHARACTERISTIC_UUID_CONSTANT      { 0xB8, 0x2A }

   /* The following MACRO is a utility MACRO that assigns the HPS HTTPS */
   /* Security Characteristic 16 bit UUID to the specified              */
   /* qapi_BLE_UUID_16_t variable. This MACRO accepts one parameter     */
   /* which is the qapi_BLE_UUID_16_t variable that is to receive the   */
   /* HPS HTTPS Security Characteristic UUID Constant Value.            */
   /* * NOTE * The UUID will be assigned into the qapi_BLE_UUID_16_t    */
   /*          variable in Little-Endian format.                        */
#define QAPI_BLE_HPS_ASSIGN_HTTPS_SECURITY_UUID_16(_x)                  QAPI_BLE_ASSIGN_BLUETOOTH_UUID_16((_x), 0x2A, 0xBB)

   /* The following MACRO is a utility MACRO that exist to compare a    */
   /* UUID 16 to the defined HPS HTTPS Security Characteristic UUID in  */
   /* UUID16 form. This MACRO only returns whether the                  */
   /* qapi_BLE_UUID_16_t variable is equal to the HPS HTTPS Security    */
   /* Characteristic UUID (MACRO returns boolean result) NOT less       */
   /* than/greater than. The first parameter is the qapi_BLE_UUID_16_t  */
   /* variable to compare to the HPS HTTPS Security Characteristic UUID.*/
#define QAPI_BLE_HPS_COMPARE_HTTPS_SECURITY_UUID_TO_UUID_16(_x)         QAPI_BLE_COMPARE_BLUETOOTH_UUID_16_TO_CONSTANT((_x), 0x2A, 0xBB)

   /* The following defines the HPS HTTPS Security Characteristic UUID  */
   /* that is used when building the HPS Service Table.                 */
#define QAPI_BLE_HPS_HTTPS_SECURITY_CHARACTERISTIC_UUID_CONSTANT        { 0xBB, 0x2A }

   /* The following defines the valid values that may be set for the    */
   /* Op_Code field of the qapi_BLE_HPS_HTTP_Control_Point_t structure  */
   /* below.                                                            */
   /* * NOTE * Values (0;12-255) are reserved for future use.           */
#define QAPI_BLE_HPS_HTTP_CONTROL_POINT_OP_CODE_HTTP_GET_REQUEST        (1)
#define QAPI_BLE_HPS_HTTP_CONTROL_POINT_OP_CODE_HTTP_HEAD_REQUEST       (2)
#define QAPI_BLE_HPS_HTTP_CONTROL_POINT_OP_CODE_HTTP_POST_REQUEST       (3)
#define QAPI_BLE_HPS_HTTP_CONTROL_POINT_OP_CODE_HTTP_PUT_REQUEST        (4)
#define QAPI_BLE_HPS_HTTP_CONTROL_POINT_OP_CODE_HTTP_DELETE_REQUEST     (5)
#define QAPI_BLE_HPS_HTTP_CONTROL_POINT_OP_CODE_HTTPS_GET_REQUEST       (6)
#define QAPI_BLE_HPS_HTTP_CONTROL_POINT_OP_CODE_HTTPS_HEAD_REQUEST      (7)
#define QAPI_BLE_HPS_HTTP_CONTROL_POINT_OP_CODE_HTTPS_POST_REQUEST      (8)
#define QAPI_BLE_HPS_HTTP_CONTROL_POINT_OP_CODE_HTTPS_PUT_REQUEST       (9)
#define QAPI_BLE_HPS_HTTP_CONTROL_POINT_OP_CODE_HTTPS_DELETE_REQUEST    (10)
#define QAPI_BLE_HPS_HTTP_CONTROL_POINT_OP_CODE_HTTP_CANCEL_REQUEST     (11)

   /* The following defines the valid bit mask values that may be set   */
   /* for the Data_Status field of the qapi_BLE_HPS_HTTP_Status_Code_t  */
   /* structure below.                                                  */
   /* * NOTE * Bits (4-7) are reserved for future use.                  */
#define QAPI_BLE_HPS_DATA_STATUS_HEADER_RECEIVED                        (0x01)
#define QAPI_BLE_HPS_DATA_STATUS_HEADER_TRUNCATED                       (0x02)
#define QAPI_BLE_HPS_DATA_STATUS_BODY_RECEIVED                          (0x04)
#define QAPI_BLE_HPS_DATA_STATUS_BODY_TRUNCATED                         (0x08)

   /* The following defines the format of the HTTP Status Code          */
   /* Characteristic structure.                                         */
   /* * NOTE * The Status_Code field should be set to the Status-Line of*/
   /*          the first line of the HTTP Response Message.             */
typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_HPS_HTTP_Status_Code_s
{
   qapi_BLE_NonAlignedWord_t  Status_Code;
   qapi_BLE_NonAlignedByte_t  Data_Status;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_HPS_HTTP_Status_Code_t;

#define QAPI_BLE_HPS_HTTP_STATUS_CODE_SIZE                              (sizeof(qapi_BLE_HPS_HTTP_Status_Code_t))

#endif
