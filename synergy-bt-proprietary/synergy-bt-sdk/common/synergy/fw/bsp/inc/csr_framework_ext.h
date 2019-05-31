#ifndef CSR_FRAMEWORK_EXT_H__
#define CSR_FRAMEWORK_EXT_H__
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_result.h"
#include "csr_framework_ext_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Result codes */
#define CSR_FE_RESULT_NO_MORE_EVENTS    ((CsrResult) 0x0001)
#define CSR_FE_RESULT_INVALID_POINTER   ((CsrResult) 0x0002)
#define CSR_FE_RESULT_INVALID_HANDLE    ((CsrResult) 0x0003)
#define CSR_FE_RESULT_NO_MORE_MUTEXES   ((CsrResult) 0x0004)
#define CSR_FE_RESULT_TIMEOUT           ((CsrResult) 0x0005)
#define CSR_FE_RESULT_NO_MORE_THREADS   ((CsrResult) 0x0006)
#define CSR_FE_RESULT_NO_MORE_TIMERS    ((CsrResult) 0x0007)

/* Thread priorities */
#define CSR_THREAD_PRIORITY_HIGHEST     ((CsrUint16) 0)
#define CSR_THREAD_PRIORITY_HIGH        ((CsrUint16) 1)
#define CSR_THREAD_PRIORITY_NORMAL      ((CsrUint16) 2)
#define CSR_THREAD_PRIORITY_LOW         ((CsrUint16) 3)
#define CSR_THREAD_PRIORITY_LOWEST      ((CsrUint16) 4)

/* Thread scheduling priority policy (can be overridden by csr_framework_ext_types.h) */
#ifndef CSR_THREAD_POLICY_REALTIME
#define CSR_THREAD_POLICY_REALTIME      ((CsrUint16) 0x0000)
#endif

#define CSR_EVENT_WAIT_INFINITE         ((CsrUint16) 0xFFFF)

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrEventCreate
 *
 *  DESCRIPTION
 *      Creates an event and returns a handle to the created event.
 *
 *  RETURNS
 *      Possible values:
 *          CSR_RESULT_SUCCESS          in case of success
 *          CSR_FE_RESULT_NO_MORE_EVENTS   in case of out of event resources
 *          CSR_FE_RESULT_INVALID_POINTER  in case the eventHandle pointer is invalid
 *
 *----------------------------------------------------------------------------*/
CsrResult CsrEventCreate(CsrEventHandle *eventHandle);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrEventWait
 *
 *  DESCRIPTION
 *      Wait for one or more of the event bits to be set.
 *      It is not possible to pass a bit mask in eventBits
 *      to wait for -- eventBits is an output variable only.
 *      If the wait times out before any events are signalled,
 *      the eventBits variable is zeroed.
 *
 *  RETURNS
 *      Possible values:
 *          CSR_RESULT_SUCCESS              in case of success
 *          CSR_FE_RESULT_TIMEOUT              in case of timeout
 *          CSR_FE_RESULT_INVALID_HANDLE       in case the eventHandle is invalid
 *          CSR_FE_RESULT_INVALID_POINTER      in case the eventBits pointer is invalid
 *
 *----------------------------------------------------------------------------*/
CsrResult CsrEventWait(CsrEventHandle *eventHandle, CsrUint16 timeoutInMs, CsrUint32 *eventBits);

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
CsrResult CsrEventSet(CsrEventHandle *eventHandle, CsrUint32 eventBits);

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
void CsrEventDestroy(CsrEventHandle *eventHandle);

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
CsrResult CsrMutexCreate(CsrMutexHandle *mutexHandle);

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
CsrResult CsrMutexLock(CsrMutexHandle *mutexHandle);

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
CsrResult CsrMutexUnlock(CsrMutexHandle *mutexHandle);

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
void CsrMutexDestroy(CsrMutexHandle *mutexHandle);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrGlobalMutexLock
 *
 *  DESCRIPTION
 *      Lock the global mutex. The global mutex is a single pre-initialised
 *      shared mutex, spinlock or similar that does not need to be created prior
 *      to use. The limitation is that there is only one single lock shared
 *      between all code. Consequently, it must only be used very briefly to
 *      either protect simple one-time initialisation or to protect the creation
 *      of a dedicated mutex by calling CsrMutexCreate.
 *
 *----------------------------------------------------------------------------*/
void CsrGlobalMutexLock(void);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrGlobalMutexUnlock
 *
 *  DESCRIPTION
 *      Unlock the global mutex.
 *
 *----------------------------------------------------------------------------*/
void CsrGlobalMutexUnlock(void);

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
 *
 *----------------------------------------------------------------------------*/
CsrResult CsrThreadCreate(void (*threadFunction)(void *pointer), void *pointer,
                          CsrUint32 stackSize, CsrUint16 priority,
                          const CsrCharString *threadName, CsrThreadHandle *threadHandle);

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
CsrResult CsrThreadGetHandle(CsrThreadHandle *threadHandle);

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
CsrResult CsrThreadEqual(CsrThreadHandle *threadHandle1, CsrThreadHandle *threadHandle2);

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
void CsrThreadSleep(CsrUint16 sleepTimeInMs);

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
void *CsrMemAllocDebug(CsrSize size,
                       const CsrCharString *file, CsrUint32 line);
#define CsrMemAlloc(sz) CsrMemAllocDebug((sz), __FILE__, __LINE__)
#else
void *CsrMemAlloc(CsrSize size);
#endif

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
void *CsrMemCallocDebug(CsrSize numberOfElements, CsrSize elementSize,
                        const CsrCharString *file, CsrUint32 line);
#define CsrMemCalloc(cnt, sz) CsrMemAllocDebug((cnt), (sz), __FILE__, __LINE__)
#else
void *CsrMemCalloc(CsrSize numberOfElements, CsrSize elementSize);
#endif

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
void CsrMemFree(void *pointer);

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
void *CsrMemAllocDmaDebug(CsrSize size,
                          const CsrCharString *file, CsrUint32 line);
#define CsrMemAllocDma(sz) CsrMemAllocDmaDebug((sz), __FILE__, __LINE__)
#else
void *CsrMemAllocDma(CsrSize size);
#endif


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
void CsrMemFreeDma(void *pointer);

#ifdef CSR_HAVE_SPINLOCK
/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSpinlockCreate
 *
 *  DESCRIPTION
 *      Create a spinlock that can be used for synchronising
 *      between interrupt and thread context.
 *      It is not permitted to pass a NULL pointer for the spinlock.
 *
 *      May be defined as a macro in csr_framework_ext_types.h.
 *
 *      May be called from both interrupt- and thread context.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
#ifndef CsrSpinlockCreate
void CsrSpinlockCreate(CsrSpinlock *spinlock);
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSpinlockDestroy
 *
 *  DESCRIPTION
 *      Destroy a spinlock previously created with CsrSpinlockCreate().
 *      After this call, the spinlock passed may no longer be used with
 *      CsrSpinlockLock(), CsrSpinlockUnlock(), and CsrSpinlockDestroy()
 *      and doing so results in undefined behavior.
 *
 *      May be defined as a macro in csr_framework_ext_types.h.
 *
 *      May be called from both interrupt- and thread context.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
#ifndef CsrSpinlockDestroy
void CsrSpinlockDestroy(CsrSpinlock *spinlock);
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSpinlockLock
 *
 *  DESCRIPTION
 *      Lock the spinlock referred to by `spinlock', saving current
 *      interrupt status, and disabling interrupts.
 *      Used for synchronising between interrupt and thread context.
 *
 *      May be defined as a macro in csr_framework_ext_types.h.
 *
 *      May be called from both interrupt- and thread context.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
#ifndef CsrSpinlockLock
void CsrSpinlockLock(CsrSpinlock *spinlock);
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSpinlockUnlock
 *
 *  DESCRIPTION
 *      Unlock the spinlock referred to by `spinlock' and restore
 *      interrupt state as saved in previous CsrSpinlockLock() call.
 *
 *      May be defined as a macro in csr_framework_ext_types.h.
 *
 *      May be called from both interrupt- and thread context.
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
#ifndef CsrSpinlockUnlock
void CsrSpinlockUnlock(CsrSpinlock *spinlock);
#endif

#endif /* CSR_HAVE_SPINLOCK */

#ifdef CSR_HAVE_TIMER
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
CsrResult CsrTimerCreate(void (*expirationFunction)(void *pointer), void *pointer, CsrTimerHandle *timerHandle);

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
void CsrTimerStart(CsrTimerHandle *timerHandle, CsrUint32 timeoutInUs);

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
void CsrTimerStop(CsrTimerHandle *timerHandle);

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
void CsrTimerDestroy(CsrTimerHandle *timerHandle);

#endif /* CSR_HAVE_TIMER */

#ifdef __cplusplus
}
#endif

#endif
