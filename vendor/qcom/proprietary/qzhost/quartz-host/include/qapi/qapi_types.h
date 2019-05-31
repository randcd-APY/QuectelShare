/* 
 * Copyright (c) 2011-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_types.h
 *
 * @brief QAPI base type definitions
 *
 * @details This file provides the base type definitions used by the QAPI.
 *          This includes the basic integer types (based on stdint.h and
 *          stddef.h) and a basic boolean type.
 */

#ifndef __QAPI_TYPES_H__ // [
#define __QAPI_TYPES_H__

#include <stdint.h>
#include <stddef.h>

typedef uint32_t qbool_t;

#ifndef false
   #define false     (0)
#endif

#ifndef true
   #define true      (1)
#endif

#endif // ] #ifndef __QAPI_TYPES_H__

