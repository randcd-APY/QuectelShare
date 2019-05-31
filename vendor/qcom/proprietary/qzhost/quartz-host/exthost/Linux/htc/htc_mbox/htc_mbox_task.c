/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*
 * Implementation of Host Target Communication tasks,
 * WorkTask and complTask, which are used to manage
 * the Mbox Pending Queues.
 *
 * A mailbox Send request is queued in arrival order on
 * a per-mailbox Send queue until a credit is available
 * from the Target. Requests in this queue are
 * waiting for the Target to provide tx credits (i.e. recv
 * buffers on the Target-side).
 *
 * A mailbox Recv request is queued in arrival order on
 * a per-mailbox Recv queue until a message is available
 * to be read. So requests in this queue are waiting for
 * the Target to provide rx data.
 *
 * htcWorkTask dequeues requests from the SendPendingQueue
 * (once credits are available) and dequeues requests from
 * the RecvPendingQueue (once rx data is available) and
 * hands them to HIF for processing.
 *
 * htcComplTask handles completion processing after
 * HIF completes a request.
 *
 * The main purpose of these tasks is to provide a
 * suitable suspendable context for processing requests
 * and completions.
 */

#ifdef USER_SPACE
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

#endif

#include "htc_internal.h"
#include "hif/spi/hif.h"
#include "htc_event_signal.h"
#include "mbox_host_reg.h"
#include "htc_mbox_internal.h"
#include "spi_regs.h"
#include "dbg.h"

int htcManageCompl(struct htc_target *target);

/*
 * Wakeup the htcWorkTask.
 *
 * Invoked whenever send/recv state changes:
 *    new Send buffer added to the sendPendingQueue
 *    new Recv buffer added to the recvPendingQueue
 *    tx credits are reaped
 *    rx data available recognized 
 */
void htcWorkTaskPoke(struct htc_target *target)
{
	
    target->workTaskHasWork = true;
#ifndef USER_SPACE
    wake_up(&target->workTaskWait);
#else
	htc_signal_set(target->workTaskWait, EVENT_TASK_WAKEUP);
#endif
}

/*
 * Body of the htcWorkTask, which hands Send and
 * Receive requests to HIF.
 */
#ifndef USER_SPACE
int htcWorkTask(void *param)
#else
void *htcWorkTask(void *param)
#endif
{
    struct htc_target *target = (struct htc_target *)param;
    int ep;
	
    HTCPrintf(HTC_PRINT_GRP15, "htcWorkTask...\n");

#ifndef USER_SPACE
    set_user_nice(current, -3);
    set_current_state(TASK_INTERRUPTIBLE);
#endif

    for (;;) {
#ifndef USER_SPACE
        lendebug("htcWorkTask: top of loop\n");
		
        /* Wait for htcWorkTaskPoke */
        wait_event_interruptible(target->workTaskWait, target->workTaskHasWork);
#else
		htc_signal_wait(target->workTaskWait, EVENT_TASK_WAKEUP, HTC_EVENT_WAIT_ANY_CLEAR_ALL);
#endif

        if (!target->workTaskHasWork) {
	        HTCPrintf(HTC_PRINT_GRP15, "htcWorkTask quit #1\n");
            break;              /* exit, if this task was interrupted */
        }

        if (target->workTaskShutdown) {
	        HTCPrintf(HTC_PRINT_GRP15, "htcWorkTask quit #2\n");
            break;              /* htcTaskStop invoked */
        }

    BARRIER;
#ifndef USER_SPACE
        barrier();
#endif

    /*
     * Update interrupt enables, if needed.
     *
     * Only this workTask is permitted to update
     * interrupt enables. That restriction eliminates
     * complex race conditions.
     */
#ifndef USER_SPACE
  //  sem_wait(&target->int_enbs_change_lock);
    htcUpdateIntrEnbs(target);
  //  sem_post(&target->int_enbs_change_lock);
#endif

    /*
     * TBD: We might consider alternative ordering policies, here,
     * between Sends and Recvs and among mailboxes. The current
     * algorithm is simple.
     */

    /* Process sends/recvs */
    for (ep = 0; ep < HTC_NUM_MBOX; ep++) {
        HTCPrintf(HTC_PRINT_GRP15,"htcWorkTask: Call htcManagePendingSends\n");
        htcManagePendingSends(target, ep);
        HTCPrintf(HTC_PRINT_GRP15,"htcWorkTask: Call htcManagePendingRecvs\n");
        htcManagePendingRecvs(target, ep);
    }

    if (target->need_credit_int_enable) 
    {
        sem_wait(&target->int_enbs_change_lock);
        HIFEnableInterrupt(target->HIF_handle, INTR_ENABLE_CREDIT_COUNT);
        sem_post(&target->int_enbs_change_lock);            
        target->need_credit_int_enable = 0;
    }

        /*
         * As long as we are constantly refreshing register
         * state and reprocessing, there is no need to
         * enable interrupts. We are essentially POLLING for
         * interrupts anyway. But if we don't find any reason
         * to refresh register state then we must have processed
         * everything we could. In that case, we re-enable
         * interrupts and stop polling.
         *
         * It IS possible to be here with interrupts already
         * enabled. For instance, if we were idle (and therefore
         * interrupts were enabled) and the user gives us a new
         * buffer to send on an endpoint with plenty of credits.
         * In this case we may get woken with interrupts enabled
         * and make one pass through.
         *
         * Or if we were idle (with interrupts enabled) and the
         * user gives us a new buffer to receive on an endpoint
         * with awaiting rx data. In this case we may be woken
         * with interrupts enabled and make a pass through and
         * then initiate a register refresh which causes more
         * passes. Note that rxdata interrupts would NOT be
         * enabled for this mailbox until rxdata is exhausted.
         *
         * The point is that we can be polling for some changes
         * while using interrupts for others.
         */
#ifndef USER_SPACE
        if (target->needRegisterRefresh) 
	{
            /* continue polling */
            target->needRegisterRefresh = false;
			
            htcRegisterRefreshStart(target);
            /*
             * TBD: We could kick off Register Refreshes more
             * often (e.g. inside loop above).
             */
        } else {
            /* Now, only the DSR can change modes */
            target->intr_state = HTC_INTERRUPT;
								
            htcUpdateIntrEnbs(target);
			
            if (target->needRegisterRefresh) 
	    {
	        target->needRegisterRefresh = false;  // only refresh one time
    	        htcRegisterRefreshStart(target);
	    }	
        }
    complete_and_exit(&target->workTaskCompletion, 0);	
#endif

    }
    return 0;
}

HTC_STATUS htcWorkTaskStart(struct htc_target * target)
{
    HTC_STATUS status = HTC_ERROR;

	HTCPrintf(HTC_PRINT_GRP15, "htcWorkTaskStart ...\n");

#ifndef USER_SPACE
    if (mutex_lock_interruptible(&target->task_mutex)) {
        return HTC_ERROR;       /* interrupted */
    }
#else
	sem_wait(&target->task_mutex);	
#endif

    if (target->workTask) {
        goto done;              /* already started */
    }

#ifndef USER_SPACE
    target->workTask = kthread_create(htcWorkTask, target, "htcWork");
    if (!target->workTask) {
        goto done;              /* Failed to create task */
    }

#else
    pthread_attr_t attr;
    struct sched_param param;
    int   iRet;

    target->workTaskWait = htc_signal_init();

    pthread_attr_init(&attr);
 
    pthread_attr_getschedparam(&attr, &param);
    param.sched_priority += 10;
    pthread_attr_setschedparam(&attr, &param);

    pthread_attr_setstacksize(&attr, SPI_THREAD_STACKSIZE);
    iRet = pthread_create(&target->workTask,  &attr,  htcWorkTask,  target);

    if (iRet != 0) {
	HTCPrintf(HTC_PRINT_GRP15, "htcWorkTask create fail\n");
        goto done;              /* Failed to create task */
    }
	
#endif
    target->workTaskShutdown = false;
	
#ifndef USER_SPACE
    init_waitqueue_head(&target->workTaskWait);
    init_completion(&target->workTaskCompletion);
#endif


#ifndef USER_SPACE
    wake_up_process(target->workTask);

#endif
    status = HTC_OK;

 done:
 	HTCPrintf(HTC_PRINT_GRP15, "htcWorkTaskStart done\n");

#ifndef USER_SPACE
    mutex_unlock(&target->task_mutex);
#else
    sem_post(&target->task_mutex);
#endif
    return status;
}

void htcWorkTaskStop(HTC_TARGET * target)
{
#ifndef USER_SPACE
    if (mutex_lock_interruptible(&target->task_mutex)) {
        return;                 /* interrupted */
    }
#else
	
#endif

    if (!target->workTask) {
        goto done;
    }

    target->workTaskShutdown = true;
    htcWorkTaskPoke(target);
#ifndef USER_SPACE
    wait_for_completion(&target->workTaskCompletion);
    target->workTask = NULL;
#else
    target->workTask = 0;
#endif

 done:
#ifndef USER_SPACE
    mutex_unlock(&target->task_mutex);
#else
	;
#endif
}

/*
 * Wakeup the complTask.
 * Invoked after adding a new completion to the complQueue.
 */
void htcComplTaskPoke(struct htc_target *target)
{
    target->complTaskHasWork = true;
#ifndef USER_SPACE
    wake_up(&target->complTaskWait);
#else
    htc_signal_set(target->complTaskWait, EVENT_TASK_WAKEUP);
#endif
}



#ifndef USER_SPACE
int htcComplTask(void *param)
#else
void *htcComplTask(void *param)
#endif
{
    struct htc_target *target = (struct htc_target *)param;

#ifndef USER_SPACE
    set_user_nice(current, -3);
    set_current_state(TASK_INTERRUPTIBLE);
#endif

    for (;;) {
		
        HTCPrintf(HTC_PRINT_GRP15, "htcComplTask wait...\n");

        /* Wait for htcComplTaskPoke */
#ifndef USER_SPACE
        wait_event_interruptible(target->complTaskWait, target->complTaskHasWork);
#else
	htc_signal_wait(target->complTaskWait, EVENT_TASK_WAKEUP, HTC_EVENT_WAIT_ANY_CLEAR_ALL);
#endif
	HTCPrintf(HTC_PRINT_GRP15, "htcComplTask has some work to do\n");

	if (!target->complTaskHasWork) {
            break;              /* exit, if this task was interrupted */
        }

        if (target->complTaskShutdown) {
            break;              /* htcTaskStop invoked */
        }

        target->complTaskHasWork = false;       /* reset before we start work */
        BARRIER;
        /*
         * TBD: We could try to prioritize completions rather than
         * handle them strictly in order. Could use separate queues for 
         * register completions and mailbox completion on each endpoint.
         * In general, completion processing is expected to be short
         * so this probably isn't worth the additional complexity.
         */
        {
            int didWork;

            do {
                didWork = htcManageCompl(target);
            } while (didWork);
        }
    }
#ifndef USER_SPACE
    complete_and_exit(&target->complTaskCompletion, 0);
#endif

    return 0;
}

int htcManageCompl(struct htc_target *target)
{
    struct htc_request *req;
#ifndef USER_SPACE
    unsigned long flags;
#endif

    /* Pop a request from the completion queue */
#ifndef USER_SPACE
    spin_lock_irqsave(&target->complQueueLock, flags);
#else
    sem_wait(&target->complQueueLock);
#endif

    req = htcRequestDeqHead(&target->complQueue);
	
#ifndef USER_SPACE
    spin_unlock_irqrestore(&target->complQueueLock, flags);
#else
    sem_post(&target->complQueueLock);	
#endif

    if (!req) {
        return 0;               /* nothing to do */
    }

    /* Invoke request's corresponding completion function */
	
    req->completionCB(req, req->status);

    return 1;
}

HTC_STATUS htcComplTaskStart(struct htc_target * target)
{
    HTC_STATUS status = HTC_ERROR;

#ifndef USER_SPACE
    if (mutex_lock_interruptible(&target->task_mutex)) {
        return HTC_ERROR;       /* interrupted */
    }
#else
	sem_wait(&target->task_mutex);
#endif

    HTCPrintf(HTC_PRINT_GRP15, "htcComplTaskStart complTask:%d\n", target->complTask);

    if (target->complTask) {
        goto done;              /* already started */
    }

#ifndef USER_SPACE
    target->complTask = kthread_create(htcComplTask, target, "htcCompl");
    if (!target->complTask) {
        goto done;              /* Failed to create task */
    }
#else
	pthread_attr_t attr;
	int   iRet;
	
#ifdef USER_SPACE
	target->complTaskWait = htc_signal_init();
#endif

	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, SPI_THREAD_STACKSIZE);
	iRet = pthread_create(&target->complTask, &attr, htcComplTask, target);
    if (iRet != 0) {
	HTCPrintf(HTC_PRINT_GRP15, "htcComplTask create fail\n");
        goto done;              /* Failed to create task */
    }
#endif

    target->complTaskShutdown = false;
#ifndef USER_SPACE
    init_waitqueue_head(&target->complTaskWait);
    init_completion(&target->complTaskCompletion);
#endif

#ifndef USER_SPACE
    wake_up_process(target->complTask);
#else
#endif
    status = HTC_OK;

 done:
 	HTCPrintf(HTC_PRINT_GRP15, "htcComplTaskStart done\n");

#ifndef USER_SPACE
    mutex_unlock(&target->task_mutex);
#else
    sem_post(&target->task_mutex);
#endif
    return status;
}

void htcComplTaskStop(HTC_TARGET * target)
{
#ifndef USER_SPACE
    if (mutex_lock_interruptible(&target->task_mutex)) {
        return;                 /* interrupted */
    }
#endif

    if (!target->complTask) {
        goto done;
    }

    target->complTaskShutdown = true;
    htcComplTaskPoke(target);
#ifndef USER_SPACE
    wait_for_completion(&target->complTaskCompletion);
    target->complTask = NULL;
#else
    target->complTask = 0;	
#endif

 done:
#ifndef USER_SPACE
    mutex_unlock(&target->task_mutex);
#else
	;
#endif
}
