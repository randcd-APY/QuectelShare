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
#include <linux/thread.h>
#include <linux/spinlock_types.h>
#include <linux/wait.h>
#include <linux/completion.h>

#else

#include <stdio.h>
#include <stdint.h>

#endif

#include "htc_internal.h"
#include "hif/spi/hif.h"

#include "mbox_host_reg.h"
#include "htc_mbox_internal.h"
#include "dbg.h"
#include "spi_regs.h"

/* Host Target Communications Completion Management */

/*
 * Top-level callback handler, registered with HIF to be invoked
 * whenever a read/write HIF operation completes. Executed in the
 * context of an HIF task, so we don't want to take much time
 * here. Pass processing to HTC's complTask.
 *
 * Used for both reg_requests and mbox_requests.
 */
HTC_STATUS htcRWCompletionHandler(void *context, HIF_STATUS status)
{
    struct htc_request *req;
    struct htc_target *target;
#ifndef USER_SPACE
    unsigned long flags;
#endif

    req = (struct htc_request *)context;

    if (context == NULL) {
        /*
         * No completion required for this request.
         * (e.g. Fire-and-forget register write.)
         */
        return HTC_OK;
    }

    target = req->target;
    req->status = status;

    /*
     * Enqueue this completed request on the
     * Target completion queue.
     */
#ifndef USER_SPACE
    spin_lock_irqsave(&target->complQueueLock, flags);
#else
	sem_wait(&target->complQueueLock);	
#endif

    htcRequestEnqTail(&target->complQueue, (struct htc_request *)req);

#ifndef USER_SPACE
    spin_unlock_irqrestore(&target->complQueueLock, flags);
#else
	sem_post(&target->complQueueLock);	
#endif

    /* Notify the completion task that it has work */
    htcComplTaskPoke(target);

    return HTC_OK;
}

/*
 * Request-specific callback invoked by the HTC Completion Task
 * when a Mbox Send Request completes. Note: Used for Mbox Send
 * requests; not used for Reg requests.
 *
 * Simply dispatch a BUFFER_SENT event to the originator of the request.
 */
void htcSendCompl(struct htc_request *req, HIF_STATUS status)
{
    HTC_TARGET *target;
    HTC_ENDPOINT_ID endPointId;
    HTC_EVENT_INFO eventInfo;
    struct htc_endpoint *endPoint;
    struct htc_mbox_request *mboxRequest = (struct htc_mbox_request *)req;
#ifndef USER_SPACE
    unsigned long flags;
#endif

    endPoint = mboxRequest->endPoint;
    target = endPoint->target;
    endPointId = GET_ENDPOINT_ID(endPoint);

    /* Strip off the HTC header that was added earlier */
    mboxRequest->buffer += HTC_HEADER_LEN;

    /*  Prepare event frame to notify caller */
    FRAME_EVENT(eventInfo,
                mboxRequest->buffer,
                mboxRequest->bufferLength,
                mboxRequest->actualLength, (status == HIF_OK) ? HTC_OK : HTC_ECANCELED, mboxRequest->cookie);

    /* Recycle the request */
#ifndef USER_SPACE
    spin_lock_irqsave(&endPoint->mboxQueueLock, flags);
#else
	sem_wait(&endPoint->mboxQueueLock);
#endif

    htcRequestEnqTail(&endPoint->sendFreeQueue, (struct htc_request *)mboxRequest);

#ifndef USER_SPACE
    spin_unlock_irqrestore(&endPoint->mboxQueueLock, flags);
#else
	sem_post(&endPoint->mboxQueueLock);
#endif

    /*
     * Regardless of success/failure, notify caller that HTC is done
     * with his buffer.
     */
    htcDispatchEvent(target, endPointId, HTC_EVENT_BUFFER_SENT, &eventInfo);

    return;
}

/*
 * Request-specific callback invoked by the HTC Completion Task
 * when a Mbox Recv Request completes. Note: Used for Mbox Recv
 * requests; not used for Reg requests.
 *
 * Simply dispatch a BUFFER_RECEIVED event to the originator
 * of the request.
 */
void htcRecvCompl(struct htc_request *req, HIF_STATUS status)
{
    HTC_TARGET *target;
    HTC_EVENT_INFO eventInfo;
    HTC_ENDPOINT_ID endPointId;
    struct htc_endpoint *endPoint;
    struct htc_mbox_request *mboxRequest = (struct htc_mbox_request *)req;
#ifndef USER_SPACE
    unsigned long flags;
#endif

#ifdef HTC_TOGGLE_ENABLE
    {
        extern void toggle(void **p, int gpio_pin_num);
            
        toggle(NULL, 49);    
    }
#endif

	HTCPrintf(HTC_PRINT_GRP2, "htcRecvCompl htc_request=%p\n", req);
	
    endPoint = mboxRequest->endPoint;
    target = endPoint->target;
    endPointId = GET_ENDPOINT_ID(endPoint);
    
    HIFResetWorkTaskProcessInt(target->HIF_handle);
    HIFWorkTaskProcessIntCondSignal(target->HIF_handle);
    
    HIFInterruptUnLock(target->HIF_handle);   

    if (status == HIF_OK) {
        uint32_t checkLength;
        /* actualLength coming from Target is always LittleEndian */
        checkLength = ((mboxRequest->buffer[0] << 0) | (mboxRequest->buffer[1] << 8));
#ifndef USER_SPACE
        BUG_ON(mboxRequest->actualLength != checkLength);
#else
        if (mboxRequest->actualLength != checkLength)
        {
            HTCPrintf(HTC_PRINT_GRP2, "F: htcRecvCompl request length:%d\n", checkLength);
        }
#endif
        mboxRequest->actualLength -= HTC_HEADER_LEN;
    } else {
        mboxRequest->actualLength = 0;
    }

    mboxRequest->buffer += HTC_HEADER_LEN;

    FRAME_EVENT(eventInfo,
                mboxRequest->buffer,
                mboxRequest->bufferLength,
                mboxRequest->actualLength, (status == HIF_OK) ? HTC_OK : HTC_ECANCELED, mboxRequest->cookie);

    /* Recycle the request */
#ifndef USER_SPACE
    spin_lock_irqsave(&endPoint->mboxQueueLock, flags);
#else
	sem_wait(&endPoint->mboxQueueLock);
#endif

    htcRequestEnqTail(&endPoint->recvFreeQueue, (struct htc_request *)mboxRequest);

#ifndef USER_SPACE
    spin_unlock_irqrestore(&endPoint->mboxQueueLock, flags);
#else
	sem_post(&endPoint->mboxQueueLock);
#endif

    htcDispatchEvent(target, endPointId, HTC_EVENT_BUFFER_RECEIVED, &eventInfo);

    return;
}

/*
 * Request-specific callback invoked when a register read/write
 * request completes. regRequest structures are not used for
 * register WRITE requests so there's not much to do for writes.
 *
 * Note: For Mbox Request completions see htcSendCompl
 * and htcRecvCompl.
 */

/*
 * Request-specific callback invoked by the HTC Completion Task
 * when a Reg Read Request completes. Note: Used for Reg requests;
 * not used for Mbox requests. Reg Write requests are fire-and-forget;
 * they are filtered out by htcRWCompletionHandler.
 */
void htcRegCompl(struct htc_request *req, HIF_STATUS status)
{
    HTC_TARGET *target;
    struct htc_reg_request *regRequest = (struct htc_reg_request *)req;
#ifndef USER_SPACE
    unsigned long flags;
#endif

    BUG_ON(!regRequest);

    /* Process async register read completion */

    target = regRequest->req.target;
    if (status != HIF_OK) {
        /* Recycle the request */
#ifndef USER_SPACE
        spin_lock_irqsave(&target->regQueueLock, flags);
#else
		sem_wait(&target->regQueueLock);
#endif
		htcRequestEnqTail(&target->regFreeQueue, (struct htc_request *)regRequest);

#ifndef USER_SPACE
        spin_unlock_irqrestore(&target->regQueueLock, flags);
#else
		sem_post(&target->regQueueLock);
#endif
        /*
         * A register read/write accepted by HIF
         * should never fail.
         */
        htcReportFailure(HTC_EPROTO);
        return;
    }

    switch (regRequest->purpose) {

    case INTR_REFRESH:
        /*
         * Target register state, including interrupt
         * registers, has been fetched.
         */
        htcRegisterRefreshCompl(target, regRequest);
        break;

    case CREDIT_REFRESH:
        htcCreditRefreshCompl(target, regRequest);
        break;

    case START_POLLING:
    case STOP_POLLING:
        htcUpdateIntrEnbsCompl(target, regRequest);
        break;

    default:
        BUG_ON(1);              /* unhandled request type */
        break;
    }

    /* Recycle this register read/write request */
#ifndef USER_SPACE
    spin_lock_irqsave(&target->regQueueLock, flags);
#else
	sem_wait(&target->regQueueLock);
#endif

    htcRequestEnqTail(&target->regFreeQueue, (struct htc_request *)regRequest);
	
#ifndef USER_SPACE
    spin_unlock_irqrestore(&target->regQueueLock, flags);
#else
	sem_post(&target->regQueueLock);
#endif

    return;
}

/* After a Register Refresh, uppdate txCreditsToReap for each endPoint.  */
void htcUpdateTxCreditsToReap(struct htc_target *target, struct htc_reg_request *regRequest)
{
    struct htc_endpoint *endPoint;
    int ep;

	HTCPrintf(HTC_PRINT_GRP2, "htcUpdateTxCreditsToReap counter_int_status:%x\n", regRequest->u.regTable.status.counter_int_status);

    for (ep = 0; ep < HTC_NUM_MBOX; ep++) {
        endPoint = &target->endPoint[ep];

#ifndef USER_SPACE
        if (regRequest->u.regTable.status.counter_int_status & (4 << ep)) {
            endPoint->txCreditsToReap = true;
        } else {
            endPoint->txCreditsToReap = false;
        }
#else
        if (regRequest->u.regTable.status.counter_int_status & (0x10 << ep)) {
            endPoint->txCreditsToReap = true;
        } else {
            endPoint->txCreditsToReap = false;
        }
#endif
    }

    return;
}

/* After a Register Refresh, uppdate rxFrameLength for each endPoint.  */
void htcUpdateRxFrameLengths(struct htc_target *target, struct htc_reg_request *regRequest)
{
    struct htc_endpoint *endPoint;
    uint32_t rx_lookahead;
    uint32_t frameLength;
    int ep;


    for (ep = 0; ep < HTC_NUM_MBOX; ep++) 
    {
        endPoint = &target->endPoint[ep];

        if (!(regRequest->u.regTable.rx_lookahead_valid & (1 << ep))) 
        {
            endPoint->rxFrameLength = 0;
            continue;
        }

        /*
         * The length of the incoming message is contained
         * in the first two (HTC_HEADER_LEN) bytes in
         * LittleEndian order.
         */
        rx_lookahead = regRequest->u.regTable.rx_lookahead[ep];
        frameLength = ((rx_lookahead & 0x00ff) << 8) + (rx_lookahead & 0xff00);
        frameLength += HTC_HEADER_LEN;

        endPoint->rxFrameLength = frameLength;
    }
#ifndef USER_SPACE
#else
{
    HIF_STATUS status;
    uint16_t  int_cause;
    uint16_t  lookahead1, lookahead2;

    if (HIFIsPacketAvailInt(target->HIF_handle) == true)
    {
        status = HIFReadWrite(target->HIF_handle, SPI_REG_RDBUF_LOOKAHEAD1, 
                                &lookahead1, sizeof(int_cause), HIF_RD_SYNC_INTERFACE, NULL);
        status = HIFReadWrite(target->HIF_handle, SPI_REG_RDBUF_LOOKAHEAD2, 
                                &lookahead2, sizeof(int_cause), HIF_RD_SYNC_INTERFACE, NULL);
        if (status == HIF_OK)
        {
            HTCPrintf(HTC_PRINT_GRP1, "htcUpdateRxFrameLengths  hd1[%02x] hd2=[%02x]\n", 
                    lookahead1, lookahead2);           
        }

        endPoint = &target->endPoint[0];

        frameLength = BE2CPU16(lookahead1);
        if (frameLength)
        {
            frameLength += HTC_HEADER_LEN;
            endPoint->rxFrameLength = frameLength;
            HIFPacketAvailSet(target->HIF_handle);
        }

    }
}

#endif

    return;
}

unsigned int htc_debug_DSR_with_no_pending;

/*
 * Completion for a register refresh.
 *
 * Update rxFrameLengths and txCreditsToReap info for
 * each endpoint. Then handle all pending interrupts (o
 * if interrupts are currently masked at the Host, handle
 * all interrupts that would be pending if interrupts
 * were enabled).
 *
 * Called in the context of HIF's completion task whenever
 * results from a register refresh are received.
 */
void htcRegisterRefreshCompl(struct htc_target *target, struct htc_reg_request *req)
{
    uint8_t host_int_status;
    uint8_t pnd_enb_intrs;      /* pending and enabled interrupts */
    uint8_t pending_int;
    uint8_t enabled_int;

	HTCPrintf(HTC_PRINT_GRP2, "htcRegisterRefreshCompl...\n");
	
    /* Update rxFrameLengths */
    htcUpdateRxFrameLengths(target, req);

    /* Update txCreditsToReap */
    htcUpdateTxCreditsToReap(target, req);

    /* Process pending Target interrupts. */

    /* Restrict attention to pending interrupts of interest */
    host_int_status = req->u.regTable.status.host_int_status;

    /* Unexpected and unhandled */
    BUG_ON(host_int_status & HOST_INT_STATUS_DRAGON_INT_MASK);

    /*
     * Form software's idea of pending and enabled interrupts.
     * Start with ERRORs and CPU interrupts.
     */
    pnd_enb_intrs = host_int_status & (HOST_INT_STATUS_ERROR_MASK | HOST_INT_STATUS_CPU_MASK);

    /*
     * Software may have intended to enable/disable credit
     * counter interrupts; but we commit these updates to
     * Target hardware lazily, just before re-enabling
     * interrupts. So registers that we have now may not
     * reflect the intended state of interrupt enables.
     */

    /*
     * Based on software credit enable bits, update pnd_enb_intrs
     * (which is like a software copy of host_int_status) as if
     * all desired interrupt enables had been commited to HW.
     */
    pending_int = req->u.regTable.status.counter_int_status;
    enabled_int = target->enb.counter_int_status_enb;
    if (pending_int & enabled_int) {
        pnd_enb_intrs |= HOST_INT_STATUS_COUNTER_MASK;
    }

    /*
     * Based on software recv data enable bits, update
     * pnd_enb_intrs AS IF all the interrupt enables had
     * been commited to HW.
     */
    pending_int = host_int_status & HOST_INT_STATUS_MBOX_DATA_MASK;
    enabled_int = target->enb.int_status_enb;
    pnd_enb_intrs |= (pending_int & enabled_int);

#ifndef USER_SPACE
#else
{
    struct htc_endpoint *endPoint;
	
	endPoint = &target->endPoint[0];
	
 	HTCPrintf(HTC_PRINT_GRP2, "end point 0 len=%d\n", endPoint->rxFrameLength);
	
	if (endPoint->rxFrameLength)	
        htcServiceRxdataInterrupt(target, req);
}
#endif

    if (!pnd_enb_intrs) {
        /* No enabled interrupts are pending. */
        htc_debug_DSR_with_no_pending++;
        return;
    }

    /*
     * Invoke specific handlers for each enabled and pending interrupt.
     * The goal of each service routine is to clear interrupts at the
     * source (on the Target).
     *
     * We deal with four types of interrupts in the HOST_INT_STATUS
     * summary register:
     *     errors
     *        This remains set until bits in ERROR_INT_STATUS are cleared
     *
     *     CPU
     *        This remains set until bits in CPU_INT_STATUS are cleared
     *
     *     rx data available
     *        These remain set as long as rx data is available. HW clears
     *        the rx data available enable bits when receive buffers
     *        are exhausted. If we exhaust Host-side received buffers, we
     *        mask the rx data available interrupt.
     *
     *     tx credits available
     *        This remains set until all bits in COUNTER_INT_STATUS are
     *        cleared by HW after Host SW reaps all credits on a mailbox.
     *        If credits on an endpoint are sufficient, we mask the
     *        corresponding COUNTER_INT_STATUS bit. We avoid "dribbling"
     *        one credit at a time and instead reap them en masse.
     *
     * The HOST_INT_STATUS register is read-only these bits are cleared 
     * by HW when the underlying condition is cleared.
     */

    if (HOST_INT_STATUS_ERROR_GET(pnd_enb_intrs)) {
        htcServiceErrorInterrupt(target, req);
    }

    if (HOST_INT_STATUS_CPU_GET(pnd_enb_intrs)) {
        htcServiceCPUInterrupt(target, req);
    }

    if (HOST_INT_STATUS_MBOX_DATA_GET(pnd_enb_intrs)) {
        htcServiceRxdataInterrupt(target, req);
    }

    if (HOST_INT_STATUS_COUNTER_GET(pnd_enb_intrs)) {
        htcServiceCreditCounterInterrupt(target, req);
    }
}

/* Complete an update of interrupt enables. */
void htcUpdateIntrEnbsCompl(struct htc_target *target, struct htc_reg_request *req)
{
 	HTCPrintf(HTC_PRINT_GRP2, "htcUpdateIntrEnbsCompl\n");
	
    if (req->purpose == START_POLLING) {
        /*
         * While waiting for rxdata and txcred
         * interrupts to be disabled at the Target,
         * we temporarily masked interrupts at
         * the Host. It is now safe to allow
         * interrupts (esp. ERROR and CPU) at
         * the Host.
         */
        HIFUnMaskInterrupt(target->HIF_handle);
    }
}

/*
 * Called to complete htcCreditRefreshStart.
 *
 * Ends a credit refresh cycle. Called after decrementing a
 * credit counter register (many times in a row). HW atomically
 * decrements the counter and returns the OLD value but HW will
 * never reduce it below 0.
 *
 * Called in the context of the workTask when the credit counter
 * decrement operation completes synchronously. Called in the
 * context of the complTask when the credit counter decrement
 * operation completes asynchronously.
 */
void htcCreditRefreshCompl(struct htc_target *target, struct htc_reg_request *regRequest)
{
#ifndef USER_SPACE
    struct htc_endpoint *endPoint;
    int reaped = 0;
    int i;
    unsigned long flags;
#endif

#ifndef USER_SPACE
    /*
     * A non-zero value indicates 1 credit reaped.
     * Typically, we will find monotonically descending
     * values that reach 0 with the remaining values
     * all zero. But we must scan the entire results
     * to handle the case where the Target just happened
     * to increment credits simultaneously with our
     * series of credit decrement operations.
     */
    endPoint = &target->endPoint[regRequest->epid];
    for (i = 0; i < HTC_TX_CREDITS_REAP_MAX; i++) {
		
        if (regRequest->u.credit_dec_results[i]) {
			HTCPrintf(HTC_PRINT_GRP2, "ep [%d] = %x\n", i, regRequest->u.credit_dec_results[i]);
            reaped++;
        }
    }

    if (reaped)
	{
        spin_lock_irqsave(&endPoint->txCreditLock, flags);
		endPoint->txCreditsAvailable += reaped;
        endPoint->txCreditRefreshInProgress = false;
		
        spin_unlock_irqrestore(&endPoint->txCreditLock, flags);

        htcWorkTaskPoke(target);
    }
#endif

    /* Recycle this request */
#ifndef USER_SPACE
    spin_lock_irqsave(&target->regQueueLock, flags);
#else
	sem_wait(&target->regQueueLock);
#endif

    htcRequestEnqTail(&target->regFreeQueue, (struct htc_request *)regRequest);
	
#ifndef USER_SPACE
    spin_unlock_irqrestore(&target->regQueueLock, flags);
#else
	sem_post(&target->regQueueLock);
#endif
}

