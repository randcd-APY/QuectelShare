#ifndef __QAPI_HEAP_H__
#define __QAPI_HEAP_H__

/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**=================================================================================
 *
 *                       HEAP MANAGEMENT
 *
 *==================================================================================
 *
 * @file qapi_heap.h
 *
 * @brief Heap Memory Manager
 *
 * @addtogroup qapi_heap
 * @{
 * This interface implements a simple suballocator to manage memory 
 * allocations and deallocations using a First Fit strategy. 
 * 
 * 
 * These routines are FULLY re-entrant. Furthermore, while
 *          performing memory allocation/deallocation calls on one heap, the routines
 *          may be interrupted to perform memory allocation/deallocation calls on
 *          different heaps without blocking or corruption. However, should an
 *          interrupting task attempt to perform memory allocation/deallocation
 *          on the same heap that had a critical section interrupted, it will block,
 *          allowing the first call to finish. All this is accomplished by giving
 *          each heap its own semaphore.
 *          
 *
 * @code {.c}
 *
 *    * The code snippet below demonstrates usage of heap interface. In the example
 *    * below, a client creates a heap by passing a pointer to a buffer and its size.
 *    * This API will initialize a heap structure, which can then be used to allocate
 *    * and deallocate memory from this buffer dynamically. 
 *    * 
 *    * For brevity, the sequence assumes that all calls succeed. 
 * 
 *    qapi_HEAP_handle_t heap_handle;	  
 *    char *ptr; 
 *    qapi_Heap_Init( &heap_handle, &mem_buf[0], sizeof(mem_buf));
 *    qapi_Heap_Malloc(heap_handle, &ptr, 30);
 *    qapi_Heap_free(heap_handle, ptr);
 *    qapi_Heap_Deinit(heap_handle);
 *
 * @endcode
 * @}
*/

/*==================================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.ioe/1.0/api/services/qapi_heap.h#6 $

when       who     what, where, why
--------   ---    ---------------------------------------------------------- 
02/10/17   leo    (Tech Comm) Edited/added Doxyen comments and markup.
02/23/16   ps     Created
===========================================================================*/


/*==================================================================================

                               TYPE DEFINITIONS

==================================================================================*/

/** @addtogroup qapi_heap
 @{ */

/**
* Heap handle provided by the module to the client. Clients must pass this
* handle as a token with subsequent calls. Note that the clients
* should cache the handle. Once lost, it cannot be queried back from
* the module.
*/
typedef void* qapi_HEAP_handle_t;

/**
  @brief Initializes the heap object and sets up heap_mem_ptr for use with 
  the heap object. 
 
  The heap_mem_ptr parameter may be aligned on any boundary. Beginning bytes 
  are skipped until a paragraph boundary is reached. NULL pointers must not 
  be passed in for heap_mem_ptr.
  
  There is no protection for initializing a heap more than once. If a heap
  is reinitialized, all pointers previously allocated from the heap are
  immediately invalidated and their contents possibly destroyed. If this is 
  the intended behavior, a heap may be initialized more than once.
 
  @param[in,out] handle    Pointer to the memory where the allocated 
                           handle is to be saved.
  @param[in] pool_start    Pointer to the contiguous block of memory used 
                           for this heap.
  @param[in] pool_size     Size in bytes of the memory pointed to by 
                           heap_mem_ptr.
 
  @return
  Returns QAPI_OK on success and an error code is returned on failure

  @dependencies
  None.
*/
qapi_Status_t qapi_Heap_Init(
   qapi_HEAP_handle_t *handle, 
   void *pool_start,
   unsigned long pool_size );


/**
  @brief  
  Deinitializes the handle object only if the heap is in the Reset state.
  Users are responsible for freeing all allocated pointers before calling 
  this function.
  
  @param[in] handle  Handle to the heap.
  
  @return
  Returns QAPI_OK on success and an error code on failure.
  
  @dependencies
  None.
*/
qapi_Status_t qapi_Heap_Deinit(
   qapi_HEAP_handle_t handle);

/**
  @brief Allocates a block of size bytes from the heap handle. 

  @param[in] handle     Handle to the heap.
  @param[out] ptr       Pointer to the address in which to store the allocated memory.
  @param[in] size       Number of bytes to allocate.  

  @return
  Returns QAPI_OK on success and an error code on failure.
 
  @dependencies
  The heap handle must not be NULL. \n
  The size must not be 0.
*/
/*lint -sem(mem_malloc,1p,2n>=0&&(@p==0||@P==2n)) */

qapi_Status_t qapi_Heap_Malloc(
   qapi_HEAP_handle_t handle,
   void **ptr, 
   unsigned int size);

/**
  @brief Deallocates the pointer block of memory.
  
  @param[in] handle    Handle to the heap from which to deallocate.
  @param[in] ptr       Pointer to the memory to be freed.

  @return
  Returns QAPI_OK on success and an error code on failure.

  @dependencies
  None.
*/
/*lint -sem(mem_free,1p) */
qapi_Status_t qapi_Heap_free(
   qapi_HEAP_handle_t handle, 
   void *ptr);


/*@}*/ /* group heap */

#endif /*__QAPI_HEAP_H__ */
