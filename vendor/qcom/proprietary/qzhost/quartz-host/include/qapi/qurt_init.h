#ifndef QURT_INIT_H
#define QURT_INIT_H

/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
  @file qurt_init.h 
  @brief  Prototypes of QuRT Initialization API 

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.
*/

#include "qurt_types.h"

/*=============================================================================
                                 CONSTANTS AND MACROS
=============================================================================*/ 

#define QURT_INFO_OBJ_SIZE_BYTES    64

/*=============================================================================
                                    TYPEDEFS
=============================================================================*/
/** @addtogroup init_data_types
@{ */

/** QuRT data structure. */
typedef struct qurt_data_s
{
  void *hPtr;
  /**< Pointer to the heap used by the RTOS. */
  void *rPtr;
  /**< Reserved pointer for future use. */
} qurt_data_t;

/** QuRT information type. */ /* 8 byte aligned */
typedef struct qurt_info
{  
   unsigned long long _bSpace[QURT_INFO_OBJ_SIZE_BYTES/sizeof(unsigned long long)];
   /**< Opaque OS object accessible via attribute accessor APIs. */
}qurt_info_t;



/*=============================================================================
                                    FUNCTIONS
=============================================================================*/
/**
  Initializes the RTOS and QuRT libraries. The data passed by the platform to QuRT depends
  on the underlying RTOS. 
  
  @param[in,out] arg Pointer to the initialization parameter.

  @return
  None.

  @dependencies
  None.
  
*/
int qurt_init (void *arg);

/** @} */ /* end_init data_types */

#endif /* QURT_INIT_H */

