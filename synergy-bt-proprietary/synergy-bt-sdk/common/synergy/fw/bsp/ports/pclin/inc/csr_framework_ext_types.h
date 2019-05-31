#ifndef CSR_FRAMEWORK_EXT_TYPES_H__
#define CSR_FRAMEWORK_EXT_TYPES_H__
/*****************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"

#ifdef __KERNEL__
#include <linux/kthread.h>
#include <linux/semaphore.h>
#include <linux/spinlock.h>
#include <linux/timer.h>
#else
#include <pthread.h>
#include <time.h>
#include <signal.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __KERNEL__

struct CsrEvent
{
    /* wait_queue for waking the kernel thread */
    wait_queue_head_t wakeup_q;
    spinlock_t        lock;
    unsigned long     lock_flag;
    unsigned int      wakeup_flag;
};

typedef struct CsrEvent CsrEventHandle;
typedef struct semaphore CsrMutexHandle;
typedef struct task_struct *CsrThreadHandle;

#define CSR_HAVE_SPINLOCK
typedef struct
{
    spinlock_t    lock;
    unsigned long flags;
} CsrSpinlock;

#define CSR_SPINLOCK_DEFINE(lock__) CsrSpinlock lock__ = {__SPIN_LOCK_INITIALIZER(lock), 0}
#define CsrSpinlockCreate(lock__) spin_lock_init(&(lock__)->lock)
#define CsrSpinlockDestroy(lock__)
#define CsrSpinlockLock(lock__) spin_lock_irqsave(&(lock__)->lock, (lock__)->flags)
#define CsrSpinlockUnlock(lock__) spin_unlock_irqrestore(&(lock__)->lock, (lock__)->flags)

#define CSR_HAVE_TIMER
typedef struct timer_list CsrTimerHandle;

#define CSR_THREAD_POLICY_REALTIME ((CsrUint16) 0x8000)

#else /* __KERNEL __ */

struct CsrEvent
{
    pthread_cond_t  event;
    pthread_mutex_t mutex;
    CsrUint32       eventBits;
};

typedef struct CsrEvent CsrEventHandle;
typedef pthread_mutex_t CsrMutexHandle;
typedef pthread_t CsrThreadHandle;

#define CSR_HAVE_TIMER
typedef timer_t CsrTimerHandle;

#endif /* __KERNEL__ */

#ifdef __cplusplus
}
#endif

#endif
