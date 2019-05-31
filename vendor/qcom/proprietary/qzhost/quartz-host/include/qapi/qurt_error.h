#ifndef QURT_ERROR_H
#define QURT_ERROR_H

/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
  @file qurt_error.h 
  @brief Error results- QuRT defines a set of standard symbols for the error result values. This file lists the
  symbols and their corresponding values.

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none
*/


/** @addtogroup qurt_error
@{ */

/*=====================================================================
Constants and macros
======================================================================*/
#define QURT_EOK                             0  /**< Operation was successfully performed. */
#define QURT_EFATAL                         -1  /**< Fatal error (which should never happen). */
#define QURT_EVAL                           -2  /**< Wrong values for parameters; the specified page does not exist. */
#define QURT_EMEM                           -3  /**< Not enough memory to perform the operation.*/

#define QURT_EINVALID                       -4  /**< Invalid argument value; invalid key. */ 
#define QURT_EFAILED_TIMEOUT                -5  /**< Time out. */
#define QURT_EUNKNOWN                       -6  /**< Defined but never used in QuRT. */
#define QURT_EFAILED                        -7  /**< Operation failed. */
#define QURT_ENOMSGS                        -8  /**< Message queue is empty. */

#define QURT_ENOTALLOWED                    -9  /**< Operation is not allowed. */

/** @} */ /* end_addtogroup qurt_error */
#endif /* QURT_ERROR_H */
