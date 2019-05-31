/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#define DEBUGLEVEL 0
#if DEBUGLEVEL > 0
#define DPRINTF(x) printf x
#else
#define DPRINTF(x)
#endif /* DEBUGLEVEL > 0 */

#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_framework_ext.h"

#include "csr_mem_hook.h"

static pthread_mutex_t globalMutex = PTHREAD_MUTEX_INITIALIZER;

#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING) && defined(USE_SCHED_RR)
static int mapPr(CsrUint16 pri, int minPri, int maxPri)
{
    int priority;

    priority = ((minPri + maxPri) / 2) + (2 - pri);

    if (priority < minPri)
    {
        priority = minPri;
    }

    if (priority > maxPri)
    {
        priority = maxPri;
    }

    return priority;
}

#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrEventCreate
 *
 *  DESCRIPTION
 *      Creates an event and returns a handle to the created event.
 *
 *  RETURNS
 *      Possible values:
 *          CSR_RESULT_SUCCESS             in case of success
 *          CSR_FE_RESULT_NO_MORE_EVENTS   in case of out of event resources
 *          CSR_FE_RESULT_INVALID_POINTER  in case the eventHandle pointer is invalid
 *----------------------------------------------------------------------------*/
CsrResult CsrEventCreate(CsrEventHandle *eventHandle)
{
    if (eventHandle == NULL)
    {
        return CSR_FE_RESULT_INVALID_POINTER;
    }

    if (pthread_mutex_init(&(eventHandle->mutex), NULL) == 0)
    {
#ifndef ANDROID
        pthread_condattr_t condattr;

        if ((pthread_condattr_init(&condattr) != 0) ||
            (pthread_condattr_setclock(&condattr, CLOCK_MONOTONIC) != 0) ||
            (pthread_cond_init(&(eventHandle->event), &condattr) != 0))
        {
            (void) pthread_condattr_destroy(&condattr);

            return CSR_FE_RESULT_NO_MORE_EVENTS;
        }

        (void) pthread_condattr_destroy(&condattr);
#else
        if (pthread_cond_init(&(eventHandle->event), NULL) != 0)
        {
            return CSR_FE_RESULT_NO_MORE_EVENTS;
        }
#endif

        eventHandle->eventBits = 0;
        return CSR_RESULT_SUCCESS;
    }
    else
    {
        return CSR_FE_RESULT_NO_MORE_EVENTS;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrEventWait
 *
 *  DESCRIPTION
 *      Wait for the event to be set.
 *
 *  RETURNS
 *      Possible values:
 *          CSR_RESULT_SUCCESS                 in case of success
 *          CSR_FE_RESULT_TIMEOUT              in case of timeout
 *          CSR_FE_RESULT_INVALID_HANDLE       in case the eventHandle is invalid
 *          CSR_FE_RESULT_INVALID_POINTER      in case the eventBits pointer is invalid
 *----------------------------------------------------------------------------*/
CsrResult CsrEventWait(CsrEventHandle *eventHandle, CsrUint16 timeoutInMs, CsrUint32 *eventBits)
{
    struct timespec ts;
    CsrResult result;
    int rc;

    if (eventHandle == NULL)
    {
        return CSR_FE_RESULT_INVALID_HANDLE;
    }

    if (eventBits == NULL)
    {
        return CSR_FE_RESULT_INVALID_POINTER;
    }

    (void) pthread_mutex_lock(&(eventHandle->mutex));
    if ((eventHandle->eventBits == 0) && (timeoutInMs != 0))
    {
        rc = 0;
        if (timeoutInMs != CSR_EVENT_WAIT_INFINITE)
        {
            time_t sec;

            /* [MDM] IR-200028. Don't use CLOCK_REALTIME, 
               or pthread_cond_timedwait cannot return when timeout. */
            clock_gettime(CLOCK_MONOTONIC, &ts);

            sec = timeoutInMs / 1000;
            ts.tv_sec = ts.tv_sec + sec;
            ts.tv_nsec = ts.tv_nsec + (timeoutInMs - sec * 1000) * 1000000;

            if (ts.tv_nsec >= 1000000000L)
            {
                ts.tv_nsec -= 1000000000L;
                ts.tv_sec++;
            }

            while (eventHandle->eventBits == 0 && rc == 0)
            {
                /* In Andriod N-Car for MSM platform, "pthread_cond_timedwait_monotonic_np" isn't supported. */
                rc = pthread_cond_timedwait(&(eventHandle->event), &(eventHandle->mutex), &ts);
            }
        }
        else
        {
            while (eventHandle->eventBits == 0 && rc == 0)
            {
                rc = pthread_cond_wait(&(eventHandle->event), &(eventHandle->mutex));
            }
        }
    }

    result = (eventHandle->eventBits == 0) ? CSR_FE_RESULT_TIMEOUT : CSR_RESULT_SUCCESS;
    /* Indicate to caller which events were triggered and cleared */
    *eventBits = eventHandle->eventBits;
    /* Clear triggered events */
    eventHandle->eventBits = 0;
    (void) pthread_mutex_unlock(&(eventHandle->mutex));
    return result;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrEventSet
 *
 *  DESCRIPTION
 *      Set an event.
 *
 *  RETURNS
 *      Possible values:
 *          CSR_RESULT_SUCCESS              in case of success
 *          CSR_FE_RESULT_INVALID_HANDLE       in case the eventHandle is invalid
 *
 *----------------------------------------------------------------------------*/
CsrResult CsrEventSet(CsrEventHandle *eventHandle, CsrUint32 eventBits)
{
    if (eventHandle == NULL)
    {
        return CSR_FE_RESULT_INVALID_HANDLE;
    }

    (void) pthread_mutex_lock(&(eventHandle->mutex));
    eventHandle->eventBits |= eventBits;
    (void) pthread_cond_signal(&(eventHandle->event));
    (void) pthread_mutex_unlock(&(eventHandle->mutex));
    return CSR_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrEventDestroy
 *
 *  DESCRIPTION
 *      Destroy the event associated.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
void CsrEventDestroy(CsrEventHandle *eventHandle)
{
    if (eventHandle == NULL)
    {
        return;
    }

    (void) pthread_cond_destroy(&(eventHandle->event));
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrMutexCreate
 *
 *  DESCRIPTION
 *      Create a mutex and return a handle to the created mutex.
 *
 *  RETURNS
 *      Possible values:
 *          CSR_RESULT_SUCCESS           in case of success
 *          CSR_FE_RESULT_NO_MORE_MUTEXES   in case of out of mutex resources
 *          CSR_FE_RESULT_INVALID_POINTER   in case the mutexHandle pointer is invalid
 *
 *----------------------------------------------------------------------------*/
CsrResult CsrMutexCreate(CsrMutexHandle *mutexHandle)
{
    if (mutexHandle == NULL)
    {
        return CSR_FE_RESULT_INVALID_POINTER;
    }

    if (pthread_mutex_init(mutexHandle, NULL) == 0)
    {
        return CSR_RESULT_SUCCESS;
    }
    else
    {
        return CSR_FE_RESULT_NO_MORE_MUTEXES;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrMutexLock
 *
 *  DESCRIPTION
 *      Lock the mutex refered to by the provided handle.
 *
 *  RETURNS
 *      Possible values:
 *          CSR_RESULT_SUCCESS           in case of success
 *          CSR_FE_RESULT_INVALID_HANDLE    in case the mutexHandle is invalid
 *
 *----------------------------------------------------------------------------*/
CsrResult CsrMutexLock(CsrMutexHandle *mutexHandle)
{
    if (mutexHandle == NULL)
    {
        return CSR_FE_RESULT_INVALID_HANDLE;
    }

    (void) pthread_mutex_lock(mutexHandle);
    return CSR_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrMutexUnlock
 *
 *  DESCRIPTION
 *      Unlock the mutex refered to by the provided handle.
 *
 *  RETURNS
 *      Possible values:
 *          CSR_RESULT_SUCCESS           in case of success
 *          CSR_FE_RESULT_INVALID_HANDLE    in case the mutexHandle is invalid
 *
 *----------------------------------------------------------------------------*/
CsrResult CsrMutexUnlock(CsrMutexHandle *mutexHandle)
{
    if (mutexHandle == NULL)
    {
        return CSR_FE_RESULT_INVALID_HANDLE;
    }

    (void) pthread_mutex_unlock(mutexHandle);
    return CSR_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrMutexDestroy
 *
 *  DESCRIPTION
 *      Destroy the previously created mutex.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
void CsrMutexDestroy(CsrMutexHandle *mutexHandle)
{
    if (mutexHandle == NULL)
    {
        return;
    }

    (void) pthread_mutex_destroy(mutexHandle);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrGlobalMutexLock
 *
 *  DESCRIPTION
 *      Lock the global mutex.
 *
 *----------------------------------------------------------------------------*/
void CsrGlobalMutexLock(void)
{
    (void) pthread_mutex_lock(&globalMutex);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrGlobalMutexUnlock
 *
 *  DESCRIPTION
 *      Unlock the global mutex.
 *
 *----------------------------------------------------------------------------*/
void CsrGlobalMutexUnlock(void)
{
    (void) pthread_mutex_unlock(&globalMutex);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrThreadCreate
 *
 *  DESCRIPTION
 *      Create thread function and return a handle to the created thread.
 *
 *  RETURNS
 *      Possible values:
 *          CSR_RESULT_SUCCESS           in case of success
 *          CSR_FE_RESULT_NO_MORE_THREADS   in case of out of thread resources
 *          CSR_FE_RESULT_INVALID_POINTER   in case one of the supplied pointers is invalid
 *          CSR_RESULT_FAILURE           otherwise
 *
 *----------------------------------------------------------------------------*/
CsrResult CsrThreadCreate(void (*threadFunction)(void *pointer), void *pointer,
                          CsrUint32 stackSize, CsrUint16 priority,
                          const CsrCharString *threadName, CsrThreadHandle *threadHandle)
{
    int rc;
    pthread_attr_t threadAttr;
#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING) && defined(USE_SCHED_RR)
    struct sched_param threadParam;
    int minPri, maxPri;
#endif

    CSR_UNUSED(stackSize);
    CSR_UNUSED(priority);
    CSR_UNUSED(threadName);

    if ((threadFunction == NULL) || (threadHandle == NULL))
    {
        return CSR_FE_RESULT_INVALID_POINTER;
    }

    rc = pthread_attr_init(&threadAttr);
    if (rc != 0)
    {
        DPRINTF(("thread attr init error: %d\n", rc));
        return CSR_RESULT_FAILURE;
    }

#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING) && defined(USE_SCHED_RR)
    /*
     * If priority scheduling is supported and the user has explicitly chosen
     * to use the SCHED_RR scheduling policy (define USE_SCHED_RR), then
     * attempt to use this policy. Otherwise, the default policy will be used
     * and priorities will be ignored.
     *
     * NOTE: the SCHED_RR policy will be attempted no matter what the default
     * scheduling policy is. This may interfere with other os threads running
     * at different policy (e.g. SCHED_OTHER), thus causing unwanted starvation.
     * This implementation will try to assign thread priorities in the middle
     * range of SCHED_RR - to prevent our threads to block everything else.
     *
     * The caller must have appropriate permission to
     * set the required scheduling parameters or policy. Otherwise, the
     * pthread_create call will fail with EPERM.
     */
    rc = pthread_attr_setschedpolicy(&threadAttr, SCHED_RR);
    if (rc != 0)
    {
        DPRINTF(("sched set policy SCHED_RR error: %d\n", rc));
        return CSR_RESULT_FAILURE;
    }

    minPri = sched_get_priority_min(SCHED_RR);
    if (minPri == -1)
    {
        DPRINTF(("sched get priority min error\n"));
        return CSR_RESULT_FAILURE;
    }

    maxPri = sched_get_priority_max(SCHED_RR);
    if (maxPri == -1)
    {
        DPRINTF(("sched get priority max error\n"));
        return CSR_RESULT_FAILURE;
    }

    /* Map priority range */
    threadParam.sched_priority = mapPr(priority, minPri, maxPri);

    DPRINTF(("SCHED_RR priority range is %d to %d: using %d\n",
             minPri, maxPri, threadParam.sched_priority));

    rc = pthread_attr_setschedparam(&threadAttr, &threadParam);
    if (rc != 0)
    {
        DPRINTF(("set sched param error: %d\n", rc));
        return CSR_RESULT_FAILURE;
    }

    rc = pthread_attr_setinheritsched(&threadAttr, PTHREAD_EXPLICIT_SCHED);
    if (rc != 0)
    {
        DPRINTF(("set inherit sched error: %d\n", rc));
        return CSR_RESULT_FAILURE;
    }

#ifdef _POSIX_THREAD_ATTR_STACKSIZE
    if (stackSize != 0)
    {
        rc = pthread_attr_setstacksize(&threadAttr, (size_t) stackSize);
        if (rc != 0)
        {
            DPRINTF(("set stack size error: %d\n", rc));
            return CSR_RESULT_FAILURE;
        }
    }
#endif

#else
#if DEBUGLEVEL > 0
    DPRINTF(("thread priorities not supported\n"));
#endif
#endif
    rc = pthread_create(threadHandle, &threadAttr, (void *(*)(void *))threadFunction, pointer);
    if (rc != 0)
    {
        DPRINTF(("thread create error: %d\n", rc));
        return CSR_FE_RESULT_NO_MORE_THREADS;
    }
    else
    {
        (void) pthread_detach(*threadHandle);
    }
    return CSR_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrThreadGetHandle
 *
 *  DESCRIPTION
 *      Return thread handle of calling thread.
 *
 *  RETURNS
 *      Possible values:
 *          CSR_RESULT_SUCCESS             in case of success
 *          CSR_FE_RESULT_INVALID_POINTER  in case the threadHandle pointer is invalid
 *
 *----------------------------------------------------------------------------*/
CsrResult CsrThreadGetHandle(CsrThreadHandle *threadHandle)
{
    if (threadHandle == NULL)
    {
        return CSR_FE_RESULT_INVALID_POINTER;
    }

    *threadHandle = pthread_self();
    return CSR_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrThreadEqual
 *
 *  DESCRIPTION
 *      Compare thread handles
 *
 *  RETURNS
 *      Possible values:
 *          CSR_RESULT_SUCCESS             in case thread handles are identical
 *          CSR_FE_RESULT_INVALID_POINTER  in case either threadHandle pointer is invalid
 *          CSR_RESULT_FAILURE             otherwise
 *
 *----------------------------------------------------------------------------*/
CsrResult CsrThreadEqual(CsrThreadHandle *threadHandle1, CsrThreadHandle *threadHandle2)
{
    if ((threadHandle1 == NULL) || (threadHandle2 == NULL))
    {
        return CSR_FE_RESULT_INVALID_POINTER;
    }

    if (pthread_equal(*threadHandle1, *threadHandle2) != 0)
    {
        return CSR_RESULT_SUCCESS;
    }
    else
    {
        return CSR_RESULT_FAILURE;
    }
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrThreadSleep
 *
 *  DESCRIPTION
 *      Sleep for a given period.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
void CsrThreadSleep(CsrUint16 sleepTimeInMs)
{
    struct timespec ts;
    CsrUint16 seconds = (sleepTimeInMs / 1000U);
    CsrUint32 nanoseconds = (sleepTimeInMs - seconds * 1000U) * 1000000U;

    ts.tv_sec = (time_t) seconds;
    ts.tv_nsec = (long) nanoseconds;

    (void) nanosleep(&ts, NULL);
}

#ifdef CSR_MEMALLOC_PROFILING
/* Align data buffer to an 8 byte boundary. */
#define bufAlignBytes           8
#define bufAlign(ptr, align)    ((ptr + (align - 1)) & ~(align - 1))

static CsrMemHookAlloc cbAlloc;
static CsrMemHookFree cbFree;
static CsrMemHookAlloc cbAllocDma;
static CsrMemHookFree cbFreeDma;
static CsrSize headerSize;
static CsrSize tailSize;

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrMemHookSet
 *
 *  DESCRIPTION
 *      Installs hooks to be called during memory allocation
 *      and deallocation.
 *
 *  RETURNS
 *
 *----------------------------------------------------------------------------*/
void CsrMemHookSet(CsrMemHookAlloc allocCb, CsrMemHookFree freeCb,
                   CsrMemHookAlloc allocDmaCb, CsrMemHookFree freeDmaCb,
                   CsrSize hdrSz, CsrSize tailSz)
{
    headerSize = bufAlign(hdrSz, bufAlignBytes); /* Align once */
    tailSize = tailSz; /* Unaligned!  Immediately follows buffer. */
    cbAlloc = allocCb;
    cbFree = freeCb;
    cbAllocDma = allocDmaCb;
    cbFreeDma = freeDmaCb;
}

#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrMemAlloc
 *
 *  DESCRIPTION
 *      Allocate dynamic memory of a given size.
 *
 *  RETURNS
 *      Pointer to allocated memory, or NULL in case of failure.
 *      Allocated memory is not initialised.
 *
 *----------------------------------------------------------------------------*/
#ifdef CSR_MEM_DEBUG
#undef CsrMemAlloc
void *CsrMemAlloc(CsrSize size)
{
    return CsrMemAllocDebug(size, __FILE__, __LINE__);
}

void *CsrMemAllocDebug(CsrSize size,
                       const CsrCharString *file, CsrUint32 line)
#else
void *CsrMemAlloc(CsrSize size)
#endif
{
    void *hdr;

#ifdef CSR_MEMALLOC_PROFILING
    hdr = malloc(size + headerSize + tailSize);
#else
    hdr = malloc(size);
#endif

#ifdef CSR_MEMALLOC_PROFILING
    if (hdr != NULL)
    {
        CsrUint8 *ptr = hdr;
        ptr += headerSize;

        if (cbAlloc != NULL)
        {
#ifdef CSR_MEM_DEBUG
            cbAlloc(hdr, ptr, 1, size, file, line);
#else
            cbAlloc(hdr, ptr, 1, size, "n/a", 0);
#endif
        }
        return ptr;
    }
#endif
    return hdr;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrMemCalloc
 *
 *  DESCRIPTION
 *      Allocate dynamic memory of a given size calculated as the
 *      numberOfElements times the elementSize.
 *
 *  RETURNS
 *      Pointer to allocated memory, or NULL in case of failure.
 *      Allocated memory is zero initialised.
 *
 *----------------------------------------------------------------------------*/
#ifdef CSR_MEM_DEBUG
#undef CsrMemCalloc
void *CsrMemCalloc(CsrSize numberOfElements, CsrSize elementSize)
{
    return CsrMemCallocDebug(numberOfElements, elementSize,
                             __FILE__, __LINE__);
}

void *CsrMemCallocDebug(CsrSize numberOfElements, CsrSize elementSize,
                        const CsrCharString *file, CsrUint32 line)
#else
void *CsrMemCalloc(CsrSize numberOfElements, CsrSize elementSize)
#endif
{
    void *hdr;

#ifdef CSR_MEMALLOC_PROFILING
    hdr = calloc(numberOfElements + (headerSize + tailSize) / elementSize + 1, elementSize);
#else
    hdr = calloc(numberOfElements, elementSize);
#endif

#ifdef CSR_MEMALLOC_PROFILING
    if (hdr != NULL)
    {
        CsrUint8 *ptr = hdr;
        ptr += headerSize;

        if (cbAlloc != NULL)
        {
#ifdef CSR_MEM_DEBUG
            cbAlloc(hdr, ptr, numberOfElements, elementSize,
                    file, line);
#else
            cbAlloc(hdr, ptr, numberOfElements, elementSize,
                    "n/a", 0);
#endif
        }
        return ptr;
    }
#endif
    return hdr;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrMemFree
 *
 *  DESCRIPTION
 *      Free dynamic allocated memory.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
void CsrMemFree(void *pointer)
{
    if (pointer == NULL)
    {
        return;
    }
#ifdef CSR_MEMALLOC_PROFILING
    {
        CsrUint8 *headerPointer = pointer;
        headerPointer -= headerSize;

        if (cbFree != NULL)
        {
            cbFree(headerPointer, pointer);
        }
        /*lint -save -e424 */ /* freeing modified pointer is ok */
        free(headerPointer);
        /*lint -restore */
    }
#else
    free(pointer);
#endif
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrMemAllocDma
 *
 *  DESCRIPTION
 *      Allocate dynamic memory suitable for DMA transfers.
 *
 *  RETURNS
 *      Pointer to allocated memory, or NULL in case of failure.
 *      Allocated memory is not initialised.
 *
 *----------------------------------------------------------------------------*/
#ifdef CSR_MEM_DEBUG
#undef CsrMemAllocDma
void *CsrMemAllocDma(CsrSize size)
{
    return CsrMemAllocDmaDebug(size, __FILE__, __LINE__);
}

void *CsrMemAllocDmaDebug(CsrSize size,
                          const CsrCharString *file, CsrUint32 line)
#else
void *CsrMemAllocDma(CsrSize size)
#endif
{
    void *hdr;

#ifdef CSR_MEMALLOC_PROFILING
    hdr = malloc(size + headerSize + tailSize);
#else
    hdr = malloc(size);
#endif

#ifdef CSR_MEMALLOC_PROFILING
    if (hdr != NULL)
    {
        CsrUint8 *ptr = hdr;
        ptr += headerSize;

        if (cbAllocDma != NULL)
        {
#ifdef CSR_MEM_DEBUG
            cbAllocDma(hdr, ptr, 1, size, file, line);
#else
            cbAllocDma(hdr, ptr, 1, size, "n/a", 0);
#endif
        }
        return ptr;
    }
#endif
    return hdr;
}


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrMemFreeDma
 *
 *  DESCRIPTION
 *      Free dynamic memory allocated by CsrMemAllocDma.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
void CsrMemFreeDma(void *pointer)
{
    if (pointer == NULL)
    {
        return;
    }
#ifdef CSR_MEMALLOC_PROFILING
    {
        CsrUint8 *headerPointer = pointer;
        headerPointer -= headerSize;

        if (cbFreeDma != NULL)
        {
            cbFreeDma(headerPointer, pointer);
        }
        /*lint -save -e424 */ /* freeing modified pointer is ok */
        free(headerPointer);
        /*lint -restore */
    }
#else
    free(pointer);
#endif
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTimerCreate
 *
 *  DESCRIPTION
 *      Create a timer.
 *
 *  RETURNS
 *      Possible values:
 *          CSR_RESULT_SUCCESS              in case of success
 *          CSR_FE_RESULT_NO_MORE_TIMERS    in case of out of timer resources
 *
 *----------------------------------------------------------------------------*/
CsrResult CsrTimerCreate(void (*expirationFunction)(void *pointer), void *pointer, CsrTimerHandle *timerHandle)
{
    struct sigevent sevp;

    /* Note: assumes sizeof(union sigval) == sizeof(void *) */
    sevp.sigev_notify = SIGEV_THREAD;
    sevp.sigev_notify_function = (void (*)(union sigval))expirationFunction;
    sevp.sigev_notify_attributes = NULL;
    sevp.sigev_value.sival_ptr = pointer;

    if (timer_create(CLOCK_REALTIME, &sevp, timerHandle) != 0)
    {
        return CSR_FE_RESULT_NO_MORE_TIMERS;
    }

    return CSR_RESULT_SUCCESS;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTimerStart
 *
 *  DESCRIPTION
 *      Start or restart a timer previously created with CsrTimerCreate.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
void CsrTimerStart(CsrTimerHandle *timerHandle, CsrUint32 timeoutInUs)
{
    struct itimerspec its = {{0, 0}, {0, 0}};
    CsrUint32 seconds = timeoutInUs / 1000000U;
    CsrUint32 nanoseconds = (timeoutInUs - seconds * 1000000U) * 1000U;

    its.it_value.tv_sec = (time_t) seconds;
    its.it_value.tv_nsec = (long) nanoseconds;

    (void) timer_settime(*timerHandle, 0, &its, NULL);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTimerStop
 *
 *  DESCRIPTION
 *      Stop a timer previously started with CsrTimerStart. If the timer has
 *      already been stopped, this will have no effect.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
void CsrTimerStop(CsrTimerHandle *timerHandle)
{
    struct itimerspec its = {{0, 0}, {0, 0}};
    (void) timer_settime(*timerHandle, 0, &its, NULL);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrTimerDestroy
 *
 *  DESCRIPTION
 *      Destroy a timer previously created with CsrTimerCreate.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
void CsrTimerDestroy(CsrTimerHandle *timerHandle)
{
    (void) timer_delete(*timerHandle);
}
