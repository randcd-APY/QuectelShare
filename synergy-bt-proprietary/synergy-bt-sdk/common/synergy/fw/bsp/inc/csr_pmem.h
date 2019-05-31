#ifndef CSR_PMEM_H__
#define CSR_PMEM_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************

    NAME
        CsrPmemAlloc

    DESCRIPTION
        This function will allocate a contiguous block of memory of at least
        the specified size in bytes and return a pointer to the allocated
        memory. This function is not allowed to return NULL. A size of 0 is a
        valid request, and a unique and valid (not NULL) pointer must be
        returned in this case.

    PARAMETERS
        size - Size of memory requested. Note that a size of 0 is valid.

    RETURNS
        Pointer to allocated memory.

*****************************************************************************/
#ifdef CSR_PMEM_DEBUG
void *CsrPmemAllocDebug(CsrSize size,
    const CsrCharString *file, CsrUint32 line);
#define CsrPmemAlloc(sz) CsrPmemAllocDebug((sz), __FILE__, __LINE__)
#else
void *CsrPmemAlloc(CsrSize size);
#endif


/*****************************************************************************

    NAME
        CsrPmemZalloc

    DESCRIPTION
        This function is equivalent to CsrPmemAlloc, but the allocated memory
        is initialised to zero.

    PARAMETERS
        size - Size of memory requested. Note that a size of 0 is valid.

    RETURNS
        Pointer to allocated memory.

*****************************************************************************/
#define CsrPmemZalloc(s) (CsrMemSet(CsrPmemAlloc(s), 0x00, (s)))


/*****************************************************************************

    NAME
        CsrPmemFree

    DESCRIPTION
        This function will deallocate a previously allocated block of memory.

    PARAMETERS
        ptr - Pointer to allocated memory.

*****************************************************************************/
void CsrPmemFree(void *ptr);


/*****************************************************************************

    NAME
        pnew and zpnew

    DESCRIPTIOM
        Type-safe wrappers for CsrPmemAlloc and CsrPmemZalloc, for allocating
        single instances of a specified and named type.

    PARAMETERS
        t - type to allocate.

*****************************************************************************/
#define pnew(t) ((t *) (CsrPmemAlloc(sizeof(t))))
#define zpnew(t) ((t *) (CsrPmemZalloc(sizeof(t))))


#ifdef __cplusplus
}
#endif

#endif
