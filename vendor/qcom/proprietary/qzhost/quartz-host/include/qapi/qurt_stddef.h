#ifndef QURT_STDDEF_H
#define QURT_STDDEF_H

/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/*=======================================================================

FILE:         qurt_stddef.h

DESCRIPTION:  QuRT standard definitions

=========================================================================*/

/*======================================================================

                            Data Declarations

======================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*======================================================================
                                Constants
======================================================================*/

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

#define TRUE   1   /* Boolean true value. */
#define FALSE  0   /* Boolean false value. */

#ifndef NULL
  #define NULL  ( (void *) 0)
#endif

/*======================================================================
                              Standard Types
======================================================================*/

/* The following definitions are the same across platforms*/

#ifndef _ARM_ASM_

#ifndef _BOOLEAN_DEFINED
typedef  unsigned char      boolean;     /* Boolean value type. */
#define _BOOLEAN_DEFINED
#endif

#ifndef _UINT32_DEFINED
typedef  unsigned long int  uint32;      /* Unsigned 32 bit value */
#define _UINT32_DEFINED
#endif

#ifndef _UINT16_DEFINED
typedef  unsigned short     uint16;      /* Unsigned 16 bit value */
#define _UINT16_DEFINED
#endif

#ifndef _UINT8_DEFINED
typedef  unsigned char      uint8;       /* Unsigned 8  bit value */
#define _UINT8_DEFINED
#endif

#ifndef _INT32_DEFINED
typedef  signed long int    int32;       /* Signed 32 bit value */
#define _INT32_DEFINED
#endif

#ifndef _INT16_DEFINED
typedef  signed short       int16;       /* Signed 16 bit value */
#define _INT16_DEFINED
#endif

#ifndef _INT8_DEFINED
typedef  signed char        int8;        /* Signed 8  bit value */
#define _INT8_DEFINED
#endif

#ifndef _INT64_DEFINED
   typedef long long           int64;
   #define _INT64_DEFINED
#endif

#ifndef _UINT64_DEFINED
   typedef unsigned long long  uint64;
   #define _UINT64_DEFINED
#endif
  
#endif /* _ARM_ASM_ */

#ifdef __cplusplus
}
#endif

#endif  /* QURT_STDDEF_H  */
