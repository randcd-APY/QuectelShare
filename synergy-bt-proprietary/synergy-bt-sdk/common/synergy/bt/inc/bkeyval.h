/****************************************************************************

Copyright (c) 2010 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/
/*!
  \file   bkeyval.h

  \brief  This file contains the definitions (and macros) used for the
  Bluestack key-value-pair system. This is used by eg. L2CAP for
  the auto-connect system.

  Configuration specified as a 2D table encoded as a CsrUint16
  array. There's basically four types of fields: separators,
  keys, value fragments and end.

  *** Separators
  Separators mark the start of a new block and is interpreted as:
  Bit  15    : Always set
  Bits 14    : Always cleared
  Bits 13-8  : (Reserved - cleared)
  Bits 7-0   : Reference to other separator

  The 'reference' points to some other separate block in the
  table - effectively "I'm like that block, except...".
  Reference to block zero means "I'm like the default,
  except...". Separate blocks are implicitly numbered, starting
  with the first one as number 1.

  *** End separator
  The end separator marks the end of any reachable block.
  Bit  15-8  : All set (0xff)
  Bits 7-0   : Reference to other separator

  The end separator is used to mark the end of automatically
  reachable blocks. Unreachable blocks are only used on when
  other (reachable) block refer to them using the separator
  'reference'. The end separator is also known as the 'barrier'.

  *** Keys
  A key field is interpreted as:
  Bit  15    : Always clear
  Bits 14-12 : (reserved - cleared)
  Bit  11-10 : Value size encoding
  Bits 9-8   : Value type encoding
  Bits 7-0   : Key

  The 2 bit size encoding is:
  00 : Value(s) are 16 bit (0x0000)
  01 : Value(s) are 32 bit (0x0400)
  10 : (reserved - unused)
  11 : (reserved - unused)

  The 2 bit type encoding is:
  00 : Excact value        (0x0000)
  01 : Minimum value       (0x0100)
  10 : Maximum value       (0x0200)
  11 : Range - two values  (0x0300)

  The key can be freely encoded, however the convention says that
  the MSB is cleared for L2CAP keys and set for Synergy keys.

  *** Values
  Value fields always follow directly after a key. The length of the
  value fragment is determined by the size and type encoding in the
  key. A full value field can be between 1 and 4 CsrUint16's.

  Combine values to form the required type using Bluecore
  Friendly Format. Macros are supported for both 16 and 32 bit
  numbers. Ranges always have two values (min and max).
*/
#ifndef _BKEYVAL_H_
#define _BKEYVAL_H_

#include "csr_synergy.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Useful bits */
#define BKV_SEPARATOR                   (0x8000)
#define BKV_BARRIER                     (0xFF00)

#define BKV_SIZE_16                     (0x0000)
#define BKV_SIZE_32                     (0x0400)

#define BKV_TYPE_EXACT                  (0x0000)
#define BKV_TYPE_MIN                    (0x0100)
#define BKV_TYPE_MAX                    (0x0200)
#define BKV_TYPE_RANGE                  (0x0300) /* Note that this one have multiple bits set! */

/* Mask for keys */
#define BKV_KEY_MASK                    (0x00FF)

/* Bitmasks for subfields */
#define BKV_MASK_SIZE                   (0x0C00)
#define BKV_MASK_TYPE                   (0x0300)
#define BKV_MASK_VALUE                  (0x00FF)

/* Bit positions for size/type */
#define BKV_SHIFT_SIZE                  (10)
#define BKV_SHIFT_TYPE                  (8)

/* How much spaces does a key,value pair take up (in CsrUint16's)? */
#define BKV_KV_SPACE(key)                                          \
    (1 + (((((key) & BKV_SIZE_32) == BKV_SIZE_32) ? 2 : 1) *       \
          ((((key) & BKV_TYPE_RANGE) == BKV_TYPE_RANGE) ? 2 : 1)))

/* Encode single values */
#define BKV_UINT16(value)               ((value) & 0XFFFF)
#define BKV_UINT32H(value)              (CsrUint16)(((value) >> 16) & 0xFFFF)
#define BKV_UINT32L(value)              (CsrUint16)((value) & 0xFFFF)
#define BKV_UINT32(value)               BKV_UINT32H(value),BKV_UINT32L(value)

/* Encode a separator */
#define BKV_SEP(ref)                           \
    (BKV_SEPARATOR | ((ref) & BKV_MASK_VALUE))

/* Encode 16 bit exact */
#define BKV_16_EXACT(key, value)               \
    (((key) & BKV_KEY_MASK) |                  \
      (BKV_SIZE_16) |                          \
      (BKV_TYPE_EXACT)),                       \
     BKV_UINT16(value)
#define BKVD_16_EXACT(tab, idx, key, value)    \
    (   (tab)[(idx)] = (((key) & BKV_KEY_MASK) \
                     | BKV_SIZE_16             \
                     | BKV_TYPE_EXACT),        \
        (tab)[(idx)+1] = BKV_UINT16((value))   \
    )

/* Encode 16 bit minimum */
#define BKV_16_MIN(key, value)                 \
    (((key) & BKV_KEY_MASK) |                  \
      (BKV_SIZE_16) |                          \
      (BKV_TYPE_MIN)),                         \
     BKV_UINT16(value)
#define BKVD_16_MIN(tab, idx, key, value)      \
    (   (tab)[(idx)] = (((key) & BKV_KEY_MASK) \
                        | BKV_SIZE_16          \
                        | BKV_TYPE_MIN),       \
        (tab)[(idx)+1] = BKV_UINT16((value))   \
    )

/* Encode 16 bit maximum */
#define BKV_16_MAX(key, value)                 \
    (((key) & BKV_KEY_MASK) |                  \
      (BKV_SIZE_16) |                          \
      (BKV_TYPE_MAX)),                         \
     BKV_UINT16(value)
#define BKVD_16_MAX(tab, idx, key, value)      \
    (   (tab)[(idx)] = (((key) & BKV_KEY_MASK) \
                        | BKV_SIZE_16          \
                        | BKV_TYPE_MAX),       \
        (tab)[(idx)+1] = BKV_UINT16((value))   \
    )

/* Encode 16 bit min,max range */
#define BKV_16_RANGE(key, min, max)            \
    (((key) & BKV_KEY_MASK) |                  \
      (BKV_SIZE_16) |                          \
      (BKV_TYPE_RANGE)),                       \
     BKV_UINT16(min),                          \
     BKV_UINT16(max)
#define BKVD_16_RANGE(tab, idx, key, min, max) \
    (   (tab)[(idx)] = (((key) & BKV_KEY_MASK) \
                        | BKV_SIZE_16          \
                        | BKV_TYPE_RANGE),     \
        (tab)[(idx)+1] = BKV_UINT16((min)),    \
        (tab)[(idx)+2] = BKV_UINT16((max))     \
    )

/* Encode 32 bit exact */
#define BKV_32_EXACT(key, value)               \
    (((key) & BKV_KEY_MASK) |                  \
      (BKV_SIZE_32) |                          \
      (BKV_TYPE_EXACT)),                       \
     BKV_UINT32(value)
#define BKVD_32_EXACT(tab, idx, key, value)    \
    (   (tab)[(idx)] = (((key) & BKV_KEY_MASK) \
                        | BKV_SIZE_32          \
                        | BKV_TYPE_EXACT),     \
        (tab)[(idx)+1] = BKV_UINT32H((value)), \
        (tab)[(idx)+2] = BKV_UINT32L((value))  \
    )

/* Encode 32 bit minimum */
#define BKV_32_MIN(key, value)                 \
    (((key) & BKV_KEY_MASK) |                  \
      (BKV_SIZE_32) |                          \
      (BKV_TYPE_MIN)),                         \
     BKV_UINT32(value)
#define BKVD_32_MIN(tab, idx, key, value)      \
    (   (tab)[(idx)] = (((key) & BKV_KEY_MASK) \
                        | BKV_SIZE_32          \
                        | BKV_TYPE_MIN),       \
        (tab)[(idx)+1] = BKV_UINT32H((value)), \
        (tab)[(idx)+2] = BKV_UINT32L((value))  \
    )

/* Encode 32 bit maxmimum */
#define BKV_32_MAX(key, value)                 \
    (((key) & BKV_KEY_MASK) |                  \
      (BKV_SIZE_32) |                          \
      (BKV_TYPE_MAX)),                         \
     BKV_UINT32(value)
#define BKVD_32_MAX(tab, idx, key, value)      \
    (   (tab)[(idx)] = (((key) & BKV_KEY_MASK) \
                        | BKV_SIZE_32          \
                        | BKV_TYPE_MAX),       \
        (tab)[(idx)+1] = BKV_UINT32H((value)), \
        (tab)[(idx)+2] = BKV_UINT32L((value))  \
    )

/* Encode 32 bit min,max range */
#define BKV_32_RANGE(key, min, max)            \
    (((key) & BKV_KEY_MASK) |                  \
      (BKV_SIZE_32) |                          \
      (BKV_TYPE_RANGE)),                       \
     BKV_UINT32(min),                          \
     BKV_UINT32(max)
#define BKVD_32_RANGE(tab, idx, key, min, max) \
    (   (tab)[(idx)] = (((key) & BKV_KEY_MASK) \
                        | BKV_SIZE_32          \
                        | BKV_TYPE_RANGE),     \
        (tab)[(idx)+1] = BKV_UINT32H((min)),   \
        (tab)[(idx)+2] = BKV_UINT32L((min)),   \
        (tab)[(idx)+3] = BKV_UINT32H((max)),   \
        (tab)[(idx)+4] = BKV_UINT32L((max))    \
    )

typedef struct
{
    CsrUint16 *block;
    CsrUint16 size;
    CsrUint16 iterator;
} BKV_ITERATOR_T;

/* Utilities for counting and jumping around */
extern CsrUint16 BKV_CountBlocks(CsrUint16 *block, const CsrUint16 size, CsrBool cross);
extern CsrBool BKV_Validate(CsrUint16 *block, const CsrUint16 size);
extern CsrBool BKV_JumpToBlock(BKV_ITERATOR_T *iterator, const CsrUint16 num);
#ifdef BUILD_FOR_HOST
extern CsrBool BKV_JumpToKey(BKV_ITERATOR_T *iterator, CsrUint16 key, CsrBool cross);
#endif
extern CsrBool BKV_KeyExists(const BKV_ITERATOR_T *iterator, CsrUint16 key);
extern CsrBool BKV_ReadSeparator(CsrUint16 *block, const CsrUint16 size, CsrUint16 *iterator, CsrUint16 *reference);

/* Scan and read functions */
extern CsrBool BKV_Scan32Single(const BKV_ITERATOR_T *iterator, CsrUint16 key, CsrUint32 *value);
extern CsrBool BKV_Scan32Range(const BKV_ITERATOR_T *iterator, CsrUint16 key,  CsrUint32 *min, CsrUint32 *max);
extern CsrBool BKV_Scan16Single(const BKV_ITERATOR_T *iterator, CsrUint16 key, CsrUint16 *value);
extern CsrBool BKV_Scan16Range(const BKV_ITERATOR_T *iterator, CsrUint16 key, CsrUint16 *min, CsrUint16 *max);

#ifdef __cplusplus
}
#endif

#endif

