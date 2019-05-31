#ifndef CSR_MEMBUF_H__
#define CSR_MEMBUF_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This API is used to initialize buffer elements of this type:
 */
typedef struct CsrMemBufTag
{
    struct CsrMemBufTag *next;
    CsrUint8            *buf;
    CsrSize              buflen;
    CsrUint8             chan;
} CsrMemBuf;

/*
 * The buffer elements are stored in a memory pool that consists
 * two things:
 *
 *  o A free list
 *  o An active buffer list
 *
 * The free list contains the buffer elements that are currently
 * not in use.  The active buffer list contains the elements that
 * have been filled with data.
 *
 * Specifically, this API makes it possible to create a memory
 * pool from which fixed-size memory buffers may be obtained in
 * interrupt context.
 */

/*
 * Pool flags
 */
#define CSR_MEMBUF_FLAG_DMA    (1 << 0) /* Request DMA-able buffer elements */

/*
 * CsrMemBufInit() sets up a memory pool and returns a handle to it
 * that is used when performing operations.
 * This function may only be called in thread context.
 *
 * Parameters:
 *
 *  elementCount: The number of buffer elements to allocate for the pool.
 *      If 0, elements are allocated ad hoc when needed and
 *      deallocated when returned to the pool.
 *      If nonzero, this number of buffer elements are allocated
 *      and added to the pool.  The buffer elements remain
 *      allocated until the pool is destroyed using CsrMemBufDeinit.
 *  elementSize: The size of the elements to allocate.
 *  flags:  Options for the memory pool defined as a bit mask.  For
 *      possible options, see below.
 *
 * Return value:
 *
 *  A handle of type void * identifying the pool.
 *
 */
void *CsrMemBufInit(CsrUint16 elementCount, CsrSize elementSize, CsrUint32 flags);

/*
 * CsrMemBufDeinit() destroys a memory pool.  It is the responsibility of
 * the pool owner to ensure that all buffer elements are returned to the
 * pool prior to destruction.  Failure to do so may result in the buffer
 * elements being leaked and the behavior when accessing the elements is
 * undefined.
 *
 * This function may only be called in thread context.
 *
 *  Parameters:
 */
void CsrMemBufDeinit(void *ph);

/*
 * CsrMemBufAlloc() obtains a buffer element from the free list of a
 * memory pool and returns a pointer to the beginning of the data
 * buffer of the element.
 *
 * This function may be called from thread context.
 * If the memory pool was initialized with a nonzero `elementCount'
 * count, this function mail also be called in interrupt context.
 *
 * Parameters:
 *
 *  ph: The memory pool handle.
 *  buf: A pointer to a pointer where the address of the allocated
 *      memory buffer is stored.
 *      May be NULL.
 *  context: A pointer to a void pointer where CsrMemBufAlloc(
 *      can store any context that may define the buffer
 *      element.  This value must be passed to CsrMemBufQueue().
 *      May be NULL.
 *
 * Return value:
 *
 *  A void pointer pointing at the beginning of the data buffer.
 *
 */
void *CsrMemBufAlloc(void *ph, CsrMemBuf **buf, void **context);

/*
 * CsrMemBufFree() returns a buffer element to the free list of a memory
 * pool.
 *
 * This function may only be called in thread context.
 *
 * Parameters:
 *
 *  ph: The memory pool handle.
 *  buf:    The buffer element.
 *
 */
void CsrMemBufFree(void *ph, CsrMemBuf *buf);

/*
 * CsrMemBufGet() returns the head of the active buffer list and sets the
 * active buffer list to the empty list.  Specifically, this function
 * returns the first element in a linked list that may be traversed using
 * the ``next'' member.
 *
 * This function may only be called in thread context.
 *
 * Parameters:
 *
 *  ph: The memory pool handle.
 *
 * Return value:
 *
 *  A pointer to the first element in the list.
 *
 */
CsrMemBuf *CsrMemBufGet(void *ph);

/*
 * CsrMemBufQueue() adds a buffer element to the end of the active
 * buffer list.
 *
 * This function may only be called from interrupt context.
 *
 * Parameters:
 *
 *  ph: The memory pool handle.
 *  context: The context previously obtained from a CsrMemBufAlloc() call.
 *  elementDataSize: The number of bytes of data stored in the buffer
 *      element data buffer.
 *  chan:   The BCSP channel the data was received on.
 */
void CsrMemBufQueue(void *ph, void *context, CsrSize elementDataSize, CsrUint8 chan);

#ifdef __cplusplus
}
#endif

#endif
