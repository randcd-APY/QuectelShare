/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef __QAPI_BLE_BTTYPESH_INC__
#error "qapi_ble_basetypes.h should never be used directly.  Include qapi_ble_bttypes.h, instead."
#endif

#ifndef __QAPI_BLE_BASETYPES_H__
#define __QAPI_BLE_BASETYPES_H__

#include <stdint.h>

   /* Miscellaneous defined type declarations.                          */

   /* Simply BOOLEAN type.                                              */
typedef uint32_t boolean_t;

   /* Miscellaneous Type definitions that should already be defined,    */
   /* but are necessary.                                                */
#ifndef NULL
   #define NULL ((void *)0)
#endif

#ifndef TRUE
   #define TRUE (1 == 1)
#endif

#ifndef FALSE
   #define FALSE (0 == 1)
#endif

   /* Unaligned Unsigned basic types.                                   */
typedef uint8_t qapi_BLE_NonAlignedByte_t;      /* Unaligned Generic 8 Bit    */
                                                /* Container.                 */

#define QAPI_BLE_NON_ALIGNED_BYTE_SIZE                   (sizeof(qapi_BLE_NonAlignedByte_t))

typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_NonAlignedWord_s     /* Unaligned Generic 16 Bit   */
{                                                                             /* Container.                 */
   uint8_t NonAlignedWord0;
   uint8_t NonAlignedWord1;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_NonAlignedWord_t;

#define QAPI_BLE_NON_ALIGNED_WORD_SIZE                   (sizeof(qapi_BLE_NonAlignedWord_t))

typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_NonAlignedDWord_s    /* Unaligned Generic 32 Bit   */
{                                                                             /* Container.                 */
   uint8_t NonAlignedDWord0;
   uint8_t NonAlignedDWord1;
   uint8_t NonAlignedDWord2;
   uint8_t NonAlignedDWord3;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_NonAlignedDWord_t;

#define QAPI_BLE_NON_ALIGNED_DWORD_SIZE                  (sizeof(qapi_BLE_NonAlignedDWord_t))

typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_NonAlignedQWord_s    /* Unaligned Generic 64 Bit   */
{                                                                             /* Container.                 */
   uint8_t NonAlignedQWord0;
   uint8_t NonAlignedQWord1;
   uint8_t NonAlignedQWord2;
   uint8_t NonAlignedQWord3;
   uint8_t NonAlignedQWord4;
   uint8_t NonAlignedQWord5;
   uint8_t NonAlignedQWord6;
   uint8_t NonAlignedQWord7;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_NonAlignedQWord_t;

#define QAPI_BLE_NON_ALIGNED_QWORD_SIZE                  (sizeof(qapi_BLE_NonAlignedQWord_t))

   /* Unaligned Signed basic types.                                     */
typedef int8_t qapi_BLE_NonAlignedSByte_t;      /* Unaligned Signed 8 bit     */
                                                /* Container.                 */

#define QAPI_BLE_NON_ALIGNED_SBYTE_SIZE                  (sizeof(qapi_BLE_NonAlignedSByte_t))

typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_NonAlignedSWord_s    /* Unaligned Signed 16 Bit    */
{                                                                             /* Container.                 */
   int8_t NonAlignedSWord0;
   int8_t NonAlignedSWord1;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_NonAlignedSWord_t;

#define QAPI_BLE_NON_ALIGNED_SWORD_SIZE                  (sizeof(qapi_BLE_NonAlignedSWord_t))

typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_NonAlignedSDWord_s   /* Unaligned Signed 32 Bit    */
{                                                                             /* Container.                 */
   int8_t NonAlignedSDWord0;
   int8_t NonAlignedSDWord1;
   int8_t NonAlignedSDWord2;
   int8_t NonAlignedSDWord3;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_NonAlignedSDWord_t;

#define QAPI_BLE_NON_ALIGNED_SDWORD_SIZE                 (sizeof(qapi_BLE_NonAlignedSDWord_t))

typedef __QAPI_BLE_PACKED_STRUCT_BEGIN__ struct qapi_BLE_NonAlignedSQWord_s   /* Unaligned Signed 64 Bit    */
{                                                                             /* Container.                 */
   int8_t NonAlignedSQWord0;
   int8_t NonAlignedSQWord1;
   int8_t NonAlignedSQWord2;
   int8_t NonAlignedSQWord3;
   int8_t NonAlignedSQWord4;
   int8_t NonAlignedSQWord5;
   int8_t NonAlignedSQWord6;
   int8_t NonAlignedSQWord7;
} __QAPI_BLE_PACKED_STRUCT_END__ qapi_BLE_NonAlignedSQWord_t;

#define QAPI_BLE_NON_ALIGNED_SQWORD_SIZE                 (sizeof(qapi_BLE_NonAlignedSQWord_t))

#endif
