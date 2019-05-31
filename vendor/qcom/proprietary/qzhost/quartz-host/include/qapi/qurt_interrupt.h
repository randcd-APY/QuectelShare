#ifndef QURT_INTERRUPT_H
#define QURT_INTERRUPT_H

/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
  @file qurt_interrupt.h 
  @brief  Prototypes of QuRT Interrupt handler API 

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

*/

#include "qurt_types.h"

/*=============================================================================
                                 CONSTANTS AND MACROS
=============================================================================*/ 
/*=============================================================================
                                    TYPEDEFS
=============================================================================*/

/** @addtogroup interrupt_types
@{ */

/**
Function pointer to an application-specified interrupt handler function.

  @param[in] n_irq   IRQ number.

  @return
  None.

  @dependencies
  None.
*/
typedef void (*qurt_interrupt_handler_func_ptr_t) ( uint32 n_irq );


/*=============================================================================
                                    FUNCTIONS
=============================================================================*/
/**
  Initial handler when an interrupt is invoked. 

  @return
  None.

  @dependencies
  None.
  
*/
void qurt_interrupt_irq_handler ( void );

/**
  Common IRQ handler that is invoked when each interrupt comes in.
  
  @param[in] qurt_interrupt_default_irq_handler_ptr Pointer to the IRQ handler. 

  @return
  None.

  @dependencies
  None.
  
*/

void qurt_interrupt_register_default_irq( qurt_interrupt_handler_func_ptr_t 
                             qurt_interrupt_default_irq_handler_ptr );

/** @} */ /* end_addtogroup interrupt_types */
						 
#endif /* QURT_INIT_H */

