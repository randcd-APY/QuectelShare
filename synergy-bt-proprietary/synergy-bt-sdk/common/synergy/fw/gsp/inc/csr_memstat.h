#ifndef CSR_MEMSTAT_H__
#define CSR_MEMSTAT_H__
/*****************************************************************************

Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_sched.h"
#include "csr_framework_ext.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This interface is used to perform allocation accounting
 * and profiling of CSR Synergy components.
 *
 * All allocations done through this component (directly or
 * indirectly) are accounted in several ways:
 *
 *  o Globally for CSR Synergy
 *  o Globally as a function of requested allocation size
 *  o Per scheduler task
 *
 * It is the intention that the information through this
 * interface can be used for at least the following:
 *
 *  o Tuning pool-based memory allocators to reduce memory
 *    waste.
 *  o Obtaining a rough estimate for the memory usage for a
 *    given component throughout certain scenarios.
 *
 * NOTE:  The accounting performed by this interface tracks
 * how much a given component allocated.  It does not track
 * which component is currently holding on to a particular
 * chunk of memory, and as such may produce misleading results
 * for components that pass off large amounts of long lived
 * memory allocations to other components.
 */

#ifndef CSR_MEMALLOC_PROFILING_RANGES
#define CSR_MEMALLOC_PROFILING_RANGES 6
#endif

#ifndef CSR_MEMALLOC_PROFILING_INTERVAL
#define CSR_MEMALLOC_PROFILING_INTERVAL 32
#endif

#ifndef CSR_MEMALLOC_PROFILING_TASKS
#define CSR_MEMALLOC_PROFILING_TASKS    64
#endif

/*
 * Library initialization control.
 *
 * The library must be initialized by the application
 * prior to making any other calls into the library.
 */
void CsrMemStatInit(void);
void CsrMemStatDeinit(void (*leakCb)(void *ptr, CsrSize sz,
        const CsrCharString *file, CsrUint32 line));

/*
 * Memory allocation accounting.
 *
 * Two fundamental counters are kept:
 *
 *  o Number of live allocations
 *  o Total amount of live allocated memory
 *
 * A live allocation is a chunk of memory requested to be
 * allocated using CsrPmemAlloc() (or indirectly through it
 * such as using CsrPmemZalloc(), CsrStrDup(), and
 * CsrMemDup()) and not yet deallocated using CsrPmemFree().
 *
 * For both counters, the current value as well as the peak
 * value are kept.  In this regard, it is important to note
 * that the peak count and size values are independent:  It
 * is possible that the peak count value is smaller than the
 * peak size value if a lot of small allocations were made
 * at one point that did not size-wise add up to a greater
 * than a single, large allocation size at a different time
 * where only a few chunks of memory had been allocated.
 */
typedef struct
{
    CsrSize   currentAllocationSize;
    CsrSize   peakAllocationSize;
    CsrUint32 currentAllocationCount;
    CsrUint32 peakAllocationCount;
} CsrMemStat;

typedef struct
{
    CsrThreadHandle threadId;
    CsrSchedQid     taskId;
    CsrMemStat      stats;
} CsrMemStatApp;

/*
 * Retrieve allocation counters.
 */

/*
 * CsrMemStatTaskGet() takes a scheduler queue ID
 * and a CsrMemStat pointer and fills in the relevant
 * information for the given task identifier.
 *
 * If the task is not found (because it doesn't exist
 * or because it has not made any allocations yet), the
 * allocation counters are set to zero.
 */
void CsrMemStatTaskGet(CsrSchedQid qid, CsrMemStat *statsPtr);

/*
 * CsrMemStatAppGet() takes an array of a fixed size and
 * fills in per-task/per-thread specific allocation measurements.
 * It returns the number of valid entries provided in the
 * array contiguously from index zero.
 */
CsrUint32 CsrMemStatAppGet(
    CsrMemStatApp measurements[CSR_MEMALLOC_PROFILING_TASKS]);

/*
 * CsrMemStatGlobalGet() takes a CsrMemStat pointer
 * and fills in global counters for all allocations made
 * through CsrPmemAlloc().
 */
void CsrMemStatGlobalGet(CsrMemStat *statsPtr);

/*
 * CsrMemStatProfileGet() returns a profiled view of the
 * current memory allocations over a number of fixed-size
 * intervals.
 *
 * The function provides a pointer to a CsrMemStat array
 * with allocation profiling data as a function of the
 * requested allocation size.  Each array entry accounts
 * for memory allocation requests in a given size interval.
 *
 * The number of array entries is returned in the `pools'
 * parameter, and the interval size is returned in the
 * `poolWidth' parameter.
 *
 * As an example, consider a pools value of 4 and a poolWidth
 * value of 64.  In this case, the returned entries correspond
 * to the following profiling values:
 *
 *  Array index     Size interval
 *
 *      0               [0;   63]
 *      1               [64;  127]
 *      2               [128; 191]
 *      3               [192; ..]
 *
 * The array must be freed using CsrPmemFree() when it is
 * no longer needed.
 */
void CsrMemStatProfileGet(CsrUint32 *pools, CsrUint32 *poolWidth,
    CsrMemStat **statsPtr);

/*
 * Peak counter control.
 *
 * In order to be able to track `local peaks' throughout
 * different periods, the currently recorded peak value
 * can be reset.
 *
 * This is needed e.g. if a large allocation is done during
 * initialization and memory usage later drops significantly.
 * In this case it would be nice to forget about the initial
 * peak to be able to follow peaks across different scenarios.
 *
 * It is important to note that resetting the peak simply
 * sets the peak value to be identical to the current value.
 *
 * Peak reset can be done for a specific scheduler queue id,
 * or it can be done for the global allocation counters.  It
 * is permitted to request a reset of the counters for a task
 * that has not yet performed any allocations.
 *
 * The parameters `size' and `count', if TRUE, cause the size
 * and count peak values to be reset, respectively.
 */
void CsrMemStatTaskPeakReset(CsrSchedQid qid, CsrBool size, CsrBool count);
void CsrMemStatGlobalPeakReset(CsrBool size, CsrBool count);

#ifdef __cplusplus
}
#endif

#endif /* CSR_MEMSTAT_H__ */
