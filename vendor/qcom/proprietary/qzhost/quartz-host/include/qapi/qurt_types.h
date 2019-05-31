#ifndef QURT_TYPES_H
#define QURT_TYPES_H

/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**

@file qurt_types.h

@brief  definition of basic types, constants, preprocessor macros
*/

#include "qurt_stddef.h"

/*=============================================================================
                        CONSTANTS AND MACROS
=============================================================================*/
/** @addtogroup qurt_types
@{ */

#define QURT_TIME_NO_WAIT       0x00000000 /**< Return immediately without any waiting. */
#define QURT_TIME_WAIT_FOREVER  0xFFFFFFFF /**< Block until the operation is successful. */

/*=============================================================================
                        TYPEDEFS
=============================================================================*/

/** QuRT time types. */
typedef uint32 qurt_time_t;

/** QuRT time unit types. */
typedef enum {
  QURT_TIME_TICK,                  /**< Return time in ticks. */
  QURT_TIME_MSEC,                  /**< Return time in milliseconds. */
  QURT_TIME_NONE=0x7FFFFFF        /**< Identifier to use if no particular return type is needed. */ 
}qurt_time_unit_t;

/** @} */ /* end_addtogroup qurt_types */
#endif /* QURT_TYPES_H */
