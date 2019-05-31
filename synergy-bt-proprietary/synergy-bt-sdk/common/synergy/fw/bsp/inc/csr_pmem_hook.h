#ifndef CSR_PMEM_HOOK_H__
#define CSR_PMEM_HOOK_H__
/*****************************************************************************

Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************

    NAME
        CsrPmemHookSet

    DESCRIPTION
        This function is used to install the memory hooks and set up the
        amount of header and tail space to reserve in allocations.

    PARAMETERS
        cbAlloc -- callback for allocation
        cbFree -- callback for deallocation
        hdrSz -- size of header installed by allocation hook
        tailSz -- size of tail installed by allocation hook

    RETURNS
        Pointer to allocated memory.

*****************************************************************************/

#ifdef CSR_MEMALLOC_PROFILING

typedef void (*CsrPmemHookAlloc)(void *hdr, void *buf,
                                 CsrSize count, CsrSize size,
                                 const CsrCharString *file, CsrUint32 line);
typedef void (*CsrPmemHookFree)(void *hdr, void *buf);

void CsrPmemHookSet(CsrPmemHookAlloc cbAlloc, CsrPmemHookFree cbFree,
    CsrSize hdrSz, CsrSize tailSz);

#endif

#ifdef __cplusplus
}
#endif

#endif /* CSR_PMEM_HOOK_H__ */
