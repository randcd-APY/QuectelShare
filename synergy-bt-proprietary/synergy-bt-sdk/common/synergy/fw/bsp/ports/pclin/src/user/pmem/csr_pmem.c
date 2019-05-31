/*****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/
#include "csr_synergy.h"

#include <stdlib.h>
#include <string.h>
#include "csr_types.h"
#include "csr_sched.h"
#include "csr_pmem.h"
#ifdef CSR_MEMALLOC_PROFILING
#include "csr_pmem_hook.h"
#endif
#include "csr_panic.h"

#include "csr_log.h"

#include "platform/csr_pmem_init.h"

/* Align data buffer to an 8 byte boundary. */
#define bufAlignBytes 8
#define bufAlign(ptr, align)    ((ptr + (align - 1)) & ~(align - 1))
#define hdrBuf(hdr, hlen)         (((CsrUint8 *) (hdr)) + hlen)
#define bufHdr(ptr, hlen)      ((((CsrUint8 *) (ptr)) - hlen))

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrPmemInit
 *
 *  DESCRIPTION
 *      Sets up the pool control blocks and establishes the pools' free
 *      lists.   Use only at the system's initialisation.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
void CsrPmemInit(void)
{
}

void CsrPmemDeinit(void)
{
}

#ifdef CSR_MEMALLOC_PROFILING
static CsrPmemHookAlloc cbAlloc;
static CsrPmemHookFree cbFree;
static CsrSize headerSize;
static CsrSize tailSize;

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrPmemHookSet
 *
 *  DESCRIPTION
 *      Installs hooks to be called during memory allocation
 *      and deallocation.
 *
 *  RETURNS
 *
 *----------------------------------------------------------------------------*/
void CsrPmemHookSet(CsrPmemHookAlloc allocCb, CsrPmemHookFree freeCb,
    CsrSize hdrSz, CsrSize tailSz)
{
    headerSize = bufAlign(hdrSz, bufAlignBytes); /* Align once */
    tailSize = tailSz; /* Unaligned!  Immediately follows buffer. */
    cbAlloc = allocCb;
    cbFree = freeCb;
}

#else
#define headerSize 0
#define tailSize 0
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrPmemAlloc
 *
 *  DESCRIPTION
 *      Returns a pointer to a block of memory of length "size" bytes obtained
 *      from the pools.
 *
 *      Panics on failure.
 *
 *  RETURNS
 *      void * - pointer to allocated block
 *
 *----------------------------------------------------------------------------*/
#ifdef CSR_PMEM_DEBUG
#undef CsrPmemAlloc
void *CsrPmemAlloc(CsrSize size)
{
    return CsrPmemAllocDebug(size, __FILE__, __LINE__);
}

void *CsrPmemAllocDebug(CsrSize size,
    const CsrCharString *file, CsrUint32 line)
#else
void *CsrPmemAlloc(CsrSize size)
#endif
{
    void *hdr;

    hdr = malloc(size + headerSize + tailSize);

    if (hdr == NULL)
    {
        CsrPanic(CSR_TECH_FW, CSR_PANIC_FW_HEAP_EXHAUSTION,
            "out of memory");
    }
#ifdef CSR_MEMALLOC_PROFILING
    else if (cbAlloc != NULL)
    {
#ifdef CSR_PMEM_DEBUG
        cbAlloc(hdr, hdrBuf(hdr, headerSize), 1, size, file, line);
#else
        cbAlloc(hdr, hdrBuf(hdr, headerSize), 1, size, "n/a", 0);
#endif
    }
#endif

    return hdrBuf(hdr, headerSize);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrPmemFree
 *
 *  DESCRIPTION
 *      Return a memory block previously obtained via CsrPmemAlloc to the pools.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
void CsrPmemFree(void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }
#ifdef CSR_MEMALLOC_PROFILING
    else if (cbFree != NULL)
    {
        cbFree(bufHdr(ptr, headerSize), ptr);
    }
#endif

    free(bufHdr(ptr, headerSize));
}
