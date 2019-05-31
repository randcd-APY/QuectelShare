#ifndef MALLOC_H
#define MALLOC_H

/*
 * Copyright (c) 2009-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stddef.h>
#include "com_dtypes.h"

/*===========================================================================

FUNCTION malloc

DESCRIPTION
  Allocates from the either RTOS heap in FOM mode and internal heap in 
  SOM mode.

DEPENDENCIES
  None

RETURN VALUE
  A a pointer to the newly allocated block, or NULL if the block
  could not be allocated.

SIDE EFFECTS
  Replaces C Std Library implementation.

ARGUMENTS
  size  - Size of allocation request.

===========================================================================*/
void *malloc
(
  size_t size
);

/*===========================================================================

FUNCTION free

DESCRIPTION
  Deallocates a block of memory and returns it to the heap.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Replaces C Std Library implementation.

ARGUMENTS
  ptr - A pointer to the memory block that needs to be deallocated.

===========================================================================*/
void free
(
  void *ptr
);

/*===========================================================================
FUNCTION CALLOC

DESCRIPTION
  Allocates enough space for elt_count elements each of elt_size bytes
  from the heap and initializes the space to nul bytes.  If
  heap_ptr is NULL or elt_count or elt_size is 0, the NULL pointer will
  be silently returned.

  Returns a pointer to the newly allocated block, or NULL if the block
  could not be allocated.
===========================================================================*/
/*lint -sem(mem_calloc,1p,2n>=0&&3n>=0&&(@p==0||@P==2n*3n)) */
void* calloc(size_t count, size_t size);

/** @} */ /* end_addtogroup utils_services */
#endif  /* MALLOC_H */
