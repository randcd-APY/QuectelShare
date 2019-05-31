/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*
 * Copyright (c) 2004-2006 Atheros Communications Inc.
 * All rights reserved.
 */

#ifndef USER_SPACE

#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/spinlock_types.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/completion.h>
#include <linux/slab.h>

#else
#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>

#endif

#include "htc_internal.h"
#include "hif/spi/hif.h"

#include "mbox_host_reg.h"
#include "htc_mbox_internal.h"
#include "hif_internal.h"
#include "spi_regs.h"
#include "dbg.h"

/* Host Target Communications Interrupt Management */

/*
 * Interrupt Management
 * When an interrupt occurs at the Host, it is to tell us about
 *      a high-priority error interrupt
 *      a CPU interrupt (TBD)
 *      rx data available
 *      tx credits available
 *
 * From an interrupt management perspective, rxdata and txcred
 * interrupts are grouped together. When either of these occurs,
 * we enter a mode where we repeatedly refresh register state
 * and act on all interrupt information in the refreshed registers.
 * We are basically polling with rxdata and txcred interrupts
 * masked. Eventually, we refresh registers and find no rxdata
 * and no txcred interrupts pending. At this point, we unmask
 * those types of interrupts.
 *
 * Unmasking is selective: We unmask only the interrupts that
 * we want to receive which include
 *     -rxdata interrupts for endpoints that have received
 *      buffers on the recv pending queue
 *     -txcred interrupts for endpoints with a very low
 *      count of creditsAvailable
 * Other rxdata and txcred interrupts are masked. These include:
 *     -rxdata interrupts for endpoint that lack recv buffers
 *     -txcred interrupts for endpoint with lots of credits
 *
 * Very little activity takes place in the context of the
 * interrupt function (Delayed Service Routine). We mask
 * interrupts at the Host, send a command to disable all
 * rxdata/txcred interrupts and finally start a register
 * refresh. When the register refresh completes, we unmask
 * interrupts on the Host and poke the workTask which now
 * has valid register state to examine.
 *
 * The workTask repeatedly
 *      handles outstanding rx and tx service
 *      starts another register refresh
 * Every time a register refresh completes, it pokes the
 * workTask. This cycle continues until the workTask finds
 * nothing to do after a register refresh. At this point, 
 * it unmasks rxdata/txcred interrupts at the Target (again,
 * selectively).
 *
 * While in the workTask polling cycle, we maintain a notion
 * of interrupt enables in software rather than commit these
 * to Target HW.
 *
 *
 * Credit State Machine:
 * Credits are
 *     -Added by the Target whenever a Target-side receive
 *      buffer is added to a mailbox
 *     -Never rescinded by the Target
 *     -Reaped by this software after a credit refresh cycle
 *      which is initiated
 *           -as a result of a credit counter interrupt
 *           -after a send completes and the number of credits
 *            are below an acceptable threshold
 *     -used by this software when it sends a message HIF to
 *      be sent to the Target
 *
 * The process of "reaping" credits involves first issuing
 * a sequence of reads to the COUNTER_DEC register. (This is
 * known as the start of a credit refresh.) We issue a large
 * number of reads in order to reap as many credits at once
 * as we can. When these reads complete, we determine how
 * many credits were available and increase software's notion
 * of txCreditsAvailable accordingly.
 *
 * Note: All Target reads/writes issued from the interrupt path
 * should be asynchronous. HIF adds such a request to a queue
 * and immediately returns.
 *
 * TBD: It might be helpful for HIF to support a "priority
 * queue" -- requests that should be issued prior to anything
 * in its normal queue. Even with this, a request might have
 * to wait for a while as the current, read/write request
 * completes on SDIO and then wait for all prior priority
 * requests to finish. So probably not worth the additional
 * complexity.
 */

/*
 * Commit the shadow interrupt enables in software to
 * Target Hardware. This is where the "lazy commit"
 * occurs. Always called in the context of workTask.
 *
 * When the target's intr_state is POLL:
 *    -All credit count interrupts and all rx data interrupts
 *     are disabled at the Target.
 *
 * When the target's intr_state is INTERRUPT:
 *    -We commit the shadow copy of interrupt enables.
 *    -A mailbox with low credit count will have the credit
 *     interrupt enabled. A mailbox with high credit count
 *     will have the credit interrupt disabled.
 *    -A mailbox with no available receive buffers will have
 *     the mailbox data interrupt disabled. A mailbox with
 *     at least one receive buffer will have the mailbox
 *     data interrupt enabled.
 */
void htcUpdateIntrEnbs(struct htc_target *target)
{
    HIF_STATUS status;
    struct htc_reg_request *regRequest;
    struct htc_intr_enables *enbregs;
    uint32_t address;
	uint16_t  int_cause;
#ifndef USER_SPACE
    unsigned long flags;
#endif

	HTCPrintf(HTC_PRINT_GRP1, "htcUpdateIntrEnbs\n");
					
    if ((target->enb.int_status_enb == target->last_committed_enb.int_status_enb)
        && (target->enb.counter_int_status_enb == target->last_committed_enb.counter_int_status_enb)
        && (target->enb.cpu_int_status_enb == target->last_committed_enb.cpu_int_status_enb)
        && (target->enb.err_status_enb == target->last_committed_enb.err_status_enb)) {
		
		HTCPrintf(HTC_PRINT_GRP1, "htcUpdateIntrEnbs no changes\n");
		HTCPrintf(HTC_PRINT_GRP1, "int_status_enb=%x counter_int_status_enb=%x cpu_int_status_enb=%x err_status_enb=%x\n", 
					target->enb.int_status_enb,
					target->enb.counter_int_status_enb, target->enb.cpu_int_status_enb, target->enb.err_status_enb);

        return;                 /* no changes needed */
    }

#ifndef USER_SPACE
    spin_lock_irqsave(&target->regQueueLock, flags);
#else
	sem_wait(&target->regQueueLock);
#endif

    regRequest = (struct htc_reg_request *)
        htcRequestDeqHead(&target->regFreeQueue);
		
#ifndef USER_SPACE
    spin_unlock_irqrestore(&target->regQueueLock, flags);
#else
	sem_post(&target->regQueueLock);
#endif

    BUG_ON(!regRequest);

    regRequest->buffer = NULL;
    regRequest->length = 0;
    regRequest->epid = 0;       /* unused */
    enbregs = &regRequest->u.enb;

    if (target->intr_state == HTC_INTERRUPT) 
	{
        regRequest->purpose = STOP_POLLING;
        enbregs->int_status_enb = target->enb.int_status_enb;
        enbregs->counter_int_status_enb = target->enb.counter_int_status_enb;
    } 
	else 
	{
        regRequest->purpose = START_POLLING;

        /*
         * Disable rxdata and txcred interrupts.
         * These may be enabled piecemeal as we
         * continue to poll.
         */
        enbregs->int_status_enb = (target->enb.int_status_enb & ~HOST_INT_STATUS_MBOX_DATA_MASK);
        enbregs->counter_int_status_enb = 0;
    }

    enbregs->cpu_int_status_enb = target->enb.cpu_int_status_enb;
    enbregs->err_status_enb = target->enb.err_status_enb;

    target->last_committed_enb = *enbregs;      /* structure copy */

    address = getRegAddr(INTR_ENB_REG, ENDPOINT_UNUSED);

#ifndef USER_SPACE
    status = HIFReadWrite(target->HIF_handle, address, enbregs, sizeof(*enbregs), HIF_WR_ASYNC_BYTE_INC, NULL);
#else
    status = HIFReadWrite(target->HIF_handle, SPI_REG_INTR_CAUSE, 
								&int_cause, sizeof(int_cause), HIF_RD_SYNC_INTERFACE, NULL);
	if (int_cause & (SPI_INTR_CAUSE_PKT_AVAIL | SPI_INTR_CAUSE_COUNTER_INTR))
	{
		target->needRegisterRefresh = true;
        HIFPacketAvailSet(target->HIF_handle);
        
		HTCPrintf(HTC_PRINT_GRP1, "htcUpdateIntrEnbs  needRegisterRefresh\n");		
	}
	
	HTCPrintf(HTC_PRINT_GRP1, "htcUpdateIntrEnbs  target=%p int cause=[%02x]\n", target, int_cause);
	
	HTCPrintf(HTC_PRINT_GRP1, "htcUpdateIntrEnbs  int=[%02x] cpu_int=[%02x] err=[%02x] counter_int[%02x]\n", 
					enbregs->int_status_enb,
					enbregs->cpu_int_status_enb, enbregs->err_status_enb, enbregs->counter_int_status_enb);
					
    status = HIFReadWrite(target->HIF_handle, address, enbregs, sizeof(*enbregs), HIF_WR_ASYNC_BYTE_INC_HOST, NULL);
#endif

    if (status == HIF_OK) {
        regRequest->req.completionCB((struct htc_request *)regRequest, HIF_OK);
        /* htcUpdateIntrEnbsCompl */
    } else if (status == HIF_PENDING) {
        /* Will complete later */
    } else {                    /* HIF error */
		HTCPrintf(HTC_PRINT_GRP1, "fail\n");	
        BUG_ON(1);
    }
}


/*
 * Delayed Service Routine, invoked from HIF in thread context
 * (from sdio's irqhandler) in order to handle interrupts
 * caused by the Target.
 *
 * This serves as a top-level interrupt dispatcher for HTC.
 */
HTC_STATUS htcDSRHandler(void *htc_handle)
{
    HTC_TARGET *target = (HTC_TARGET *) htc_handle;

	HTCPrintf(HTC_PRINT_GRP1, "htcDSRHandler ready=%d\n", target->ready);

    if (target->ready) {
        /*
         * Transition state to polling mode.
         * Temporarily disable intrs at Host
         * until interrupts are stopped in
         * Target HW.
         */
        HIFMaskInterrupt(target->HIF_handle);
        HIFAckInterrupt(target->HIF_handle);

        target->intr_state = HTC_POLL;

        /*
         * Kick off a register refresh so we 
         * use updated registers in order to
         * figure out what needs to be serviced.
         */

		 
        htcRegisterRefreshStart(target);
    } else {                    /* startup time */
        /*
         * Assumption is that we are receiving an interrupt
         * because the Target made a TX Credit available
         * on each endpoint (for configuration negotiation).
         */

        if (htcNegotiateConfig(target)) {
            /*
             * All endpoints are configured.
             * Target is now ready for normal operation.
             */

            {
                /*
                 * HACK: Signal Target to read mbox Cfg info.
                 * TBD: Target should use EOM rather than an
                 * an explicit Target Interrupt for this.
                 */
                uint8_t my_targ_int;
                uint32_t address;
                HIF_STATUS status;

                my_targ_int = 1;        /* Set HTC_INT_TARGET_INIT_HOST_REQ */
                address = getRegAddr(INT_TARGET_REG, ENDPOINT_UNUSED);

#ifndef USER_SPACE
                status = HIFReadWrite(target->HIF_handle,
                                      address, &my_targ_int, sizeof(my_targ_int), HIF_WR_SYNC_BYTE_INC, NULL);
				BUG_ON(status != HIF_OK);
#else
                status = HIFReadWrite(target->HIF_handle,
                                      address, &my_targ_int, sizeof(my_targ_int), HIF_WR_ASYNC_BYTE_INC_HOST, NULL);
				if (status != HIF_OK)
					HTCPrintf(HTC_PRINT_GRP1, "htcDSRHandler HIFReadWrite trigger INT fails\n");	
#endif
            }

            target->ready = true;
#ifndef USER_SPACE
            wake_up(&target->targetInitWait);
#else
			htc_signal_set(target->targetInitWait, TARGET_INIT_DONE);
#endif
		}
    }

    return HTC_OK;
}

#ifndef USER_SPACE
#else
uint8_t  int_status_mask = 0;

uint16_t htcDisableIRQ(void *htc_handle)
{
    uint16_t   int_cause;
    HTC_TARGET *target = (HTC_TARGET *) htc_handle;
    
    {
        HIFInterruptLock(target->HIF_handle);

        HTCPrintf(HTC_PRINT_GRP1, "htcDisableIRQ\n");

        HIFReadWrite(target->HIF_handle, SPI_REG_INTR_CAUSE, 
                            &int_cause, sizeof(int_cause), HIF_RD_SYNC_INTERFACE, NULL);

        HTCPrintf(HTC_PRINT_GRP1, "htcDisableIRQ  int cause=[%02x]\n", int_cause);
        if (HIFInWorkTaskProcessInt(target->HIF_handle) != 0)
        {
            int_cause &= ~SPI_INTR_CAUSE_PKT_AVAIL;
            HTCPrintf(HTC_PRINT_GRP1, "htcDisableIRQ new int cause=[%02x]\n", int_cause);
        }
            
        HIFInterruptUnLock(target->HIF_handle);
    }
    return  int_cause;
}

int htcEnableIRQ(void *htc_handle)
{
    uint16_t  int_cause;
    uint8_t   more_credit_interrupt = 0;
    
    HTC_TARGET *target = (HTC_TARGET *) htc_handle;

    HIFInterruptLock(target->HIF_handle);   
    do {
        HTCPrintf(HTC_PRINT_GRP1, "htcEnableIRQ\n");
        
        HIFWorkTaskProcessIntCondWait(target->HIF_handle);        // waiting for packet avail done

        HTCPrintf(HTC_PRINT_GRP1, "htcEnableIRQ cont\n");
            
        HIFReadWrite(target->HIF_handle, SPI_REG_INTR_CAUSE, 
                    &int_cause, sizeof(int_cause), HIF_RD_SYNC_INTERFACE, NULL);

        more_credit_interrupt = HIFHasMoreInterrupt(target->HIF_handle, int_cause);
    } while (0);

    HIFInterruptUnLock(target->HIF_handle);

    HTCPrintf(HTC_PRINT_GRP1, "htcEnableIRQ  int cause=[%04x]\n", int_cause);


    return more_credit_interrupt;
}

#endif

/*
 * Handler for CPU interrupts that are explicitly
 * initiated by Target firmware. Not used by system firmware today.
 */
void htcServiceCPUInterrupt(HTC_TARGET * target, struct htc_reg_request *req)
{
    HIF_STATUS status;
    uint32_t address;
    uint8_t cpu_int_status;

    cpu_int_status = req->u.regTable.status.cpu_int_status & target->enb.cpu_int_status_enb;

    /* Clear pending interrupts on Target -- Write 1 to Clear */
    address = getRegAddr(CPU_INT_STATUS_REG, ENDPOINT_UNUSED);

#ifndef USER_SPACE
    status =
        HIFReadWrite(target->HIF_handle, address, &cpu_int_status, sizeof(cpu_int_status), HIF_WR_SYNC_BYTE_INC, NULL);
    BUG_ON(status != HIF_OK);
#else
    status = HIFReadWrite(target->HIF_handle, address, 
				&cpu_int_status, sizeof(cpu_int_status), HIF_WR_ASYNC_BYTE_INC_HOST, NULL);
    if (status != HIF_OK)
		HTCPrintf(HTC_PRINT_GRP1, "htcServiceCPUInterrupt  HIFReadWrite fails\n");
#endif


    /* Handle cpu_int_status actions here. None are currently used */

	HTCPrintf(HTC_PRINT_GRP1, "htcServiceCPUInterrupt target=%p\n", target);
    htcRegisterRefreshStart(target);
}

/*
 * Handler for error interrupts on Target.
 * If everything is working properly we hope never to see these.
 */
void htcServiceErrorInterrupt(HTC_TARGET * target, struct htc_reg_request *req)
{
    HIF_STATUS status = HIF_ERROR;
    uint32_t address;
    uint8_t err_int_status;

    err_int_status = req->u.regTable.status.err_int_status & target->enb.err_status_enb;

    /* Clear pending interrupts on Target -- Write 1 to Clear */
    address = getRegAddr(ERROR_INT_STATUS_REG, ENDPOINT_UNUSED);
#ifndef USER_SPACE
    status = HIFReadWrite(target->HIF_handle, address,
                          &err_int_status, sizeof(err_int_status), HIF_WR_SYNC_BYTE_INC, NULL);
    BUG_ON(status != HIF_OK);
#else
    status = HIFReadWrite(target->HIF_handle, address,
                          &err_int_status, sizeof(err_int_status), HIF_WR_ASYNC_BYTE_INC_HOST, NULL);
    if (status != HIF_OK);
		HTCPrintf(HTC_PRINT_GRP1, "htcServiceErrorInterrupt  HIFReadWrite fails\n");	
#endif


    if (ERROR_INT_STATUS_WAKEUP_GET(err_int_status)) {
        /* Wakeup */
        /* Nothing needed here */
    }

    if (ERROR_INT_STATUS_RX_UNDERFLOW_GET(err_int_status)) {
        /* TBD: Rx Underflow */
        /* Host posted a read to an empty mailbox? */
        /* Target DMA was not able to keep pace with Host reads? */
        BUG_ON(1);              /* TBD */
    }

    if (ERROR_INT_STATUS_TX_OVERFLOW_GET(err_int_status)) {
        /* TBD: Tx Overflow */
        /* Host posted a write to a mailbox with no credits? */
        /* Target DMA was not able to keep pace with Host writes? */
        BUG_ON(1);              /* TBD */
    }

	HTCPrintf(HTC_PRINT_GRP1, "htcServiceErrorInterrupt target=%p\n", target);
    htcRegisterRefreshStart(target);
}

/*
 * Handler for Credit Counter interrupts from Target.
 *
 * This occurs when the number of credits available on a mailbox
 * increases from 0 to non-zero. (i.e. when Target firmware queues a
 * DMA Receive buffer to an endpoint that previously had no buffers.)
 *
 * This interrupt is masked when we have a sufficient number of
 * credits available. It is unmasked only when we have reaped all
 * available credits and are still below a desired threshold.
 */
void htcServiceCreditCounterInterrupt(HTC_TARGET * target, struct htc_reg_request *req)
{
    struct htc_endpoint *endPoint;
    uint8_t counter_int_status;
    uint8_t eps_with_credits;
    int ep;

    counter_int_status = req->u.regTable.status.counter_int_status;

    /*
     * Service the credit counter interrupt.
     * COUNTER bits [4..7] are used for credits on endpoints [0..3].
     */
    eps_with_credits = counter_int_status & target->enb.counter_int_status_enb;

	HTCPrintf(HTC_PRINT_GRP1, "htcServiceCreditCounterInterrupt  counter_int_status=%x eps_with_credits=%x\n", counter_int_status,
					eps_with_credits);

    for (ep = 0; ep < HTC_NUM_MBOX; ep++) {

#ifndef USER_SPACE
        if (!(eps_with_credits & (4 << ep))) {
            continue;
        }
#else
        if (!(eps_with_credits & (0x10 << ep))) {
            continue;
        }
#endif

        endPoint = &target->endPoint[ep];

        /*
         * We need credits on this endpoint AND
         * the target tells us that there are some.
         * Start a credit refresh cycle on this
         * endpoint.
         */
        htcCreditRefreshStart(endPoint);
    }
}

/*
 * Handler for Mailbox (rxdata) interrupts from Target.
 *
 * This occurs when a mailbox/endpoint has something to read because the
 * Target pushed data to that mailbox.
 *
 * Enabled only for endpoints with one or more recv buffers waiting.
 */
void htcServiceRxdataInterrupt(HTC_TARGET * target, struct htc_reg_request *req)
{
    uint8_t host_int_status;

    host_int_status = req->u.regTable.status.host_int_status;

	HTCPrintf(HTC_PRINT_GRP1, "htcServiceRxdataInterruptn host_int_status:%x\n", host_int_status);

    if (host_int_status & target->enb.int_status_enb & INT_STATUS_ENABLE_MBOX_DATA_MASK) {
        htcWorkTaskPoke(target);
    }
#ifndef USER_SPACE
#else
{
    struct htc_endpoint *endPoint;

	endPoint = &target->endPoint[0];
	if (endPoint->rxFrameLength)
	{
		HTCPrintf(HTC_PRINT_GRP1, "htcServiceRxdataInterruptn trigger work task\n");
        htcWorkTaskPoke(target);		
	}
}
#endif

}

/*
 * Callback registered with HIF to be invoked when Target
 * presence is first detected.
 *
 * Allocate memory for Target, endpoints, requests, etc.
 */
HTC_STATUS htcTargetInsertedHandler(void *unused_context, void *HIF_handle)
{
    HTC_TARGET *target;
    struct htc_endpoint *endPoint;
    int ep;
    HTC_EVENT_INFO eventInfo;
    struct htc_request_queue *sendFreeQueue, *recvFreeQueue;
    struct htc_request_queue *regQueue;
    uint32_t blockSize[HTC_NUM_MBOX];
    struct cbs_from_HIF htc_callbacks;  /* Callbacks from HIF to HTC */
    HTC_STATUS status = HTC_OK;
    uint8_t *allocArea;
    int nbytes;
    int i;

	HTCPrintf(HTC_PRINT_GRP1, "htcTargetInsertedHandler...\n");

    /*
     * Allocate one big block of memory with sufficient space for our needs:
     *     target structure, including endpoint structures
     *     initial per-target reg requests
     *     initial per-endpoint mbox requests (both send and receive)
     */

    /* Start with space for the target structure */
    nbytes = sizeof(*target);

    /* ...add space for Mbox Reg requests */
    nbytes += HTC_REG_REQUEST_COUNT * sizeof(struct htc_reg_request);

    /* ...add space for Mbox Send requests */
    nbytes += HTC_NUM_MBOX * (HTC_MBOX_REQUEST_COUNT * sizeof(struct htc_mbox_request));

    /* ...add space for Mbox Recv requests */
    nbytes += HTC_NUM_MBOX * (HTC_MBOX_REQUEST_COUNT * sizeof(struct htc_mbox_request));

#ifndef USER_SPACE
    allocArea = (uint8_t *) kzalloc(nbytes, GFP_KERNEL);
#else
    allocArea = (uint8_t *) calloc(1, nbytes);
#endif
    if (!allocArea) {
        /* out of memory; abort setup */
		HTCPrintf(HTC_PRINT_GRP1, "htcTargetInsertedHandler...memory use-up\n");
        return HTC_ENOMEM;
    }

    /* NB: Entire allocArea is cleared */

    /*
     * Place target FIRST in the allocArea. This allows us to
     * kfree(target) later in order to free the entire area.
     */
    target = (HTC_TARGET *) allocArea;
    allocArea += sizeof(*target);
    target->ready = false;

	HTCPrintf(HTC_PRINT_GRP1, "htcTargetInsertedHandler target=%p\n", target);
	
    /* Give a handle to HIF for this target */
    target->HIF_handle = HIF_handle;
    HIFSetHandle(HIF_handle, (void *)target);

    /* Register htc_callbacks from HIF */
    memset(&htc_callbacks, 0, sizeof(htc_callbacks));
    htc_callbacks.rwCompletionHandler = htcRWCompletionHandler;
    htc_callbacks.dsrHandler = htcDSRHandler;
#ifndef USER_SPACE
#else
    htc_callbacks.disableIRQ = htcDisableIRQ;
    htc_callbacks.enableIRQ = htcEnableIRQ;
#endif
    htc_callbacks.context = target;
    status = HIFAttach(HIF_handle, &htc_callbacks);

    /* Get block sizes and start addresses for each mailbox */
    HIFConfigureDevice((HIF_DEVICE *) HIF_handle, HIF_DEVICE_GET_MBOX_BLOCK_SIZE, &blockSize, sizeof(blockSize));

    /* Initial software copies of interrupt enables */
    target->enb.int_status_enb = INT_STATUS_ENABLE_ERROR_MASK |
        INT_STATUS_ENABLE_CPU_MASK | INT_STATUS_ENABLE_COUNTER_MASK | INT_STATUS_ENABLE_MBOX_DATA_MASK;

    /* All 8 CPU interrupts enabled */
    target->enb.cpu_int_status_enb = CPU_INT_STATUS_ENABLE_BIT_MASK;

    target->enb.err_status_enb = ERROR_STATUS_ENABLE_RX_UNDERFLOW_MASK | ERROR_STATUS_ENABLE_TX_OVERFLOW_MASK;

    /* credit counters in upper bits */
    target->enb.counter_int_status_enb = COUNTER_INT_STATUS_ENABLE_BIT_MASK;

#ifndef USER_SPACE
    spin_lock_init(&target->regQueueLock);
    mutex_init(&target->task_mutex);
#else
	sem_init(&target->regQueueLock, 0, 1);
    sem_init(&target->task_mutex, 0, 1);

	sem_init(&target->eventLock, 0, 1);
	sem_init(&target->complQueueLock, 0, 1);
	
	sem_init(&target->int_enbs_change_lock, 0, 1);
		
	htcWorkTaskStart(target);
	htcComplTaskStart(target);
#endif

    /* Initialize the register request free list */
    regQueue = &target->regFreeQueue;
    for (i = 0; i < HTC_REG_REQUEST_COUNT; i++) {
        struct htc_reg_request *regRequest;

        /* Add a regRequest to the Reg Free Queue */
        regRequest = (struct htc_reg_request *)allocArea;
        allocArea += sizeof(*regRequest);
        regRequest->req.target = target;
        regRequest->req.completionCB = htcRegCompl;

        /* no lock required -- startup */
        htcRequestEnqTail(regQueue, (struct htc_request *)regRequest);
    }

	HTCPrintf(HTC_PRINT_GRP1, "htcTargetInsertedHandler#3\n");

    /* Initialize endpoints, mbox queues and event tables */
    for (ep = 0; ep < HTC_NUM_MBOX; ep++) {
        endPoint = &target->endPoint[ep];

#ifndef USER_SPACE
        spin_lock_init(&endPoint->txCreditLock);
        spin_lock_init(&endPoint->mboxQueueLock);
#else
		sem_init(&endPoint->txCreditLock, 0, 1);
		sem_init(&endPoint->mboxQueueLock, 0, 1);
#endif
        endPoint->txCreditsAvailable = 0;
        endPoint->rxFrameLength = 0;
        endPoint->txCreditsToReap = false;
        endPoint->target = target;
        endPoint->enabled = false;
        endPoint->blockSize = blockSize[ep];
        endPoint->mboxStartAddr = MBOX_START_ADDR(ep);
        endPoint->mboxEndAddr = MBOX_END_ADDR(ep);

        /* Initialize per-endpoint queues */
        endPoint->sendPendingQueue.head = NULL;
        endPoint->sendPendingQueue.tail = NULL;
        endPoint->recvPendingQueue.head = NULL;
        endPoint->recvPendingQueue.tail = NULL;

        sendFreeQueue = &endPoint->sendFreeQueue;
        recvFreeQueue = &endPoint->recvFreeQueue;
        for (i = 0; i < HTC_MBOX_REQUEST_COUNT; i++) {
            struct htc_mbox_request *mboxRequest;

            /* Add an mboxRequest to the mbox SEND Free Queue */
            mboxRequest = (struct htc_mbox_request *)allocArea;
            allocArea += sizeof(*mboxRequest);			
#ifndef USER_SPACE
#else
			mboxRequest->req.target = target;
#endif
            mboxRequest->req.completionCB = htcSendCompl;
            mboxRequest->endPoint = endPoint;
            htcRequestEnqTail(sendFreeQueue, (struct htc_request *)mboxRequest);

            /* Add an mboxRequest to the mbox RECV Free Queue */
            mboxRequest = (struct htc_mbox_request *)allocArea;
            allocArea += sizeof(*mboxRequest);
#ifndef USER_SPACE
#else
			mboxRequest->req.target = target;
#endif
            mboxRequest->req.completionCB = htcRecvCompl;
            mboxRequest->endPoint = endPoint;
            htcRequestEnqTail(recvFreeQueue, (struct htc_request *)mboxRequest);
        }
    }

    /*
     * Target and endpoint structures are now completely initialized.
     * Add the target instance to the global list of targets.
     */
    htcTargetInstanceAdd(target);

    /* 
     * Frame a TARGET_AVAILABLE event and send it to
     * the caller. Return the HIF_DEVICE handle as a
     * parameter with the event.
     */
    FRAME_EVENT(eventInfo, (uint8_t *) HIF_handle, sizeof(HIF_DEVICE *), sizeof(HIF_DEVICE *), HTC_OK, NULL);
    htcDispatchEvent(target, ENDPOINT_UNUSED, HTC_EVENT_TARGET_AVAILABLE, &eventInfo);

	HTCPrintf(HTC_PRINT_GRP1, "htcTargetInsertedHandler done\n");

    return status;
}

/*
 * Callback registered with HIF to be invoked when Target
 * is removed
 *
 * Also see HTCStop
 * Stop tasks
 * Free memory for Target, endpoints, requests, etc.
 *
 * TBD: Not yet supported
 */
HTC_STATUS htcTargetRemovedHandler(void *unused_context, void *htc_handle)
{
    HTC_TARGET *target = (HTC_TARGET *) htc_handle;
    HTC_EVENT_INFO eventInfo;
    struct htc_endpoint *endPoint;
    int ep;

    lendebug("Enter htcTargetRemovedHandler\n");
    /* Disable each of the endpoints to stop accepting requests. */
    for (ep = 0; ep < HTC_NUM_MBOX; ep++) {
        endPoint = &target->endPoint[ep];
        endPoint->enabled = false;
    }

    if (target) {
        /* Frame a TARGET_UNAVAILABLE event and send it to the host */
        FRAME_EVENT(eventInfo, NULL, 0, 0, HTC_OK, NULL);
        htcDispatchEvent(target, ENDPOINT_UNUSED, HTC_EVENT_TARGET_UNAVAILABLE, &eventInfo);
    }

    /* TBD: call HTCStop? */
    /* TBD: Must be sure that nothing is going on before we free. */
    BUG_ON(1);                  /* TBD */

    /*
     * Free the entire allocArea allocated earlier, including target
     * structure and all request structures.
     */
#ifndef USER_SPACE
    kfree(target);
#else
    free(target);
#endif
    return HTC_OK;
}
