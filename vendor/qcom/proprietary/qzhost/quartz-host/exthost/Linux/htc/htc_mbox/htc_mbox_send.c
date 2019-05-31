/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*
 * Copyright (c) 2004-2005 Atheros Communications Inc.
 * All rights reserved.
 */

#ifndef USER_SPACE

#include <linux/kernel.h>
#include <linux/spinlock_types.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/completion.h>

#else
#include <stdio.h>
#include <stdint.h>

#endif

 
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

#define CPU2SPI16(a) SWAP16(a)
#define SPI2CPU16(a) SWAP16(a)

#define CPU2SPI32(a) SWAP32(a)
#define SPI2CPU32(a) SWAP32(a)

#else
#define	CPU2SPI16(a)  (a)
#endif

#include "htc_internal.h"
#include "hif/spi/hif.h"

#include "mbox_host_reg.h"
#include "htc_mbox_internal.h"
#include "spi_regs.h"
#include "dbg.h"

/*
 * Decide when an endpoint is low on tx credits and we should
 * initiate a credit refresh. If this is set very low, we may
 * exhaust credits entirely and pause while we wait for credits
 * to be reaped from the Target. If set very high we may end
 * up spending excessive time trying to reap when nothing is
 * available.
 *
 * TBD: We could make this something like a percentage of the
 * most credits we've ever seen on this endpoint. Or make it
 * a value that automatically adjusts -- increase by one whenever
 * we exhaust credits; decrease by one whenever a CREDIT_REFRESH
 * fails to reap any credits.
 * For now, wait until credits are completly exhausted; then
 * initiate a credit refresh cycle.
 */
#define HTC_EP_CREDITS_ARE_LOW(_endp) ((_endp)->txCreditsAvailable == 0)

/*
 * Pull as many Mailbox Send Requests off of the PendingSend queue
 * as we can (must have a credit for each send) and hand off the
 * request to HIF.
 *
 * This function returns when we exhaust Send Requests OR when we
 * exhaust credits.
 *
 * If we are low on credits, it starts a credit refresh cycle.
 *
 * Returns 0 if nothing was send to HIF
 * returns 1 if at least one request was sent to HIF
 */
int htcManagePendingSends(struct htc_target *target, int epid)
{
    struct htc_endpoint *endPoint;
    struct htc_request_queue *sendQueue;
    struct htc_mbox_request *mboxRequest;
    uint8_t txCreditsAvailable;
    int workDone = 0;
#ifndef USER_SPACE
    unsigned long flags;
#endif

    endPoint = &target->endPoint[epid];
    sendQueue = &endPoint->sendPendingQueue;

    /* 
     * Transmit messages as long as we have both
     *    something to send
     *    tx credits that permit us to send
     */

    while (!HTC_IS_QUEUE_EMPTY(sendQueue)) {
#ifndef USER_SPACE
        spin_lock_irqsave(&endPoint->txCreditLock, flags);
#else
		sem_wait(&endPoint->txCreditLock);
#endif

		txCreditsAvailable = endPoint->txCreditsAvailable;
		
        if (txCreditsAvailable) {
            endPoint->txCreditsAvailable--;
        }
		
#ifndef USER_SPACE
        spin_unlock_irqrestore(&endPoint->txCreditLock, flags);
#else
		sem_post(&endPoint->txCreditLock);
#endif

		
		if (!txCreditsAvailable) {
            /* We exhausted tx credits */
            break;
        }

        /* Get the request buffer from the Pending Send Queue */
#ifndef USER_SPACE
        spin_lock_irqsave(&endPoint->mboxQueueLock, flags);
#else
		sem_wait(&endPoint->mboxQueueLock);
#endif
		mboxRequest = (struct htc_mbox_request *)htcRequestDeqHead(sendQueue);
		
#ifndef USER_SPACE
        spin_unlock_irqrestore(&endPoint->mboxQueueLock, flags);
#else
		sem_post(&endPoint->mboxQueueLock);
#endif
        /* Hand off this Mbox Send request to HIF */
        if (htcSendRequestToHIF(endPoint, mboxRequest) == HTC_ERROR) 
		{
            HTC_EVENT_INFO eventInfo;

            /*
             * TBD: Could requeue this at the HEAD of the
             * pending send queue. Try again later?
             */

            /* Restore tx credit, since it was not used */
#ifndef USER_SPACE
            spin_lock_irqsave(&endPoint->txCreditLock, flags);
#else
			sem_wait(&endPoint->txCreditLock);
#endif
			endPoint->txCreditsAvailable++;
#ifndef USER_SPACE
            spin_unlock_irqrestore(&endPoint->txCreditLock, flags);
#else
			sem_post(&endPoint->txCreditLock);
#endif
            /* Frame an event to send to caller */
            FRAME_EVENT(eventInfo, mboxRequest->buffer,
                        mboxRequest->bufferLength, mboxRequest->actualLength, HTC_ECANCELED, mboxRequest->cookie);

            /* Free the Mailbox request */
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
            htcDispatchEvent(target, epid, HTC_EVENT_BUFFER_SENT, &eventInfo);
            goto done;
        }
        workDone = 1;
    }

    if (HTC_EP_CREDITS_ARE_LOW(endPoint)) {
#ifndef USER_SPACE
        target->enb.counter_int_status_enb |= (4 << epid);
#else

        target->enb.counter_int_status_enb |= (0x10 << epid);
#endif
        HTCPrintf(HTC_PRINT_GRP11,  "htcCreditRefreshStart credit is low(0) reap :%d\n", endPoint->txCreditsToReap);
        {
            htcCreditRefreshStart(endPoint);
            target->needRegisterRefresh = true;
        }
    } else {
#ifndef USER_SPACE
        target->enb.counter_int_status_enb &= ~(4 << epid);
#else
        target->enb.counter_int_status_enb &= ~(0x10 << epid);
#endif
    }

 done:
    return workDone;
}

/*
 * Send one request to HIF.
 *
 * Called from the HTC task while processing requests from
 * an endpoint's pendingSendQueue.
 *
 * Note: May consider calling this in the context of a process
 * submitting a new Send Request (i.e. when nothing else is
 * pending and credits are available). This would save the
 * cost of context switching to the HTC Work Task; but it would
 * require additional synchronization and would add some
 * complexity. For the high throughput case this optimization
 * would not help since we are likely to have requests
 * pending which must be submitted to HIF in the order received.
 */
HTC_STATUS htcSendRequestToHIF(struct htc_endpoint * endPoint, struct htc_mbox_request * mboxRequest)
{
    HIF_STATUS status;
    HTC_TARGET *target;
    uint32_t paddedLength;
    uint32_t mboxAddress;
    uint32_t reqType;

	
    target = endPoint->target;

    mboxRequest->actualLength += HTC_HEADER_LEN;

    /* Adjust length for power-of-2 block size */
    paddedLength = HTC_ROUND_UP(mboxRequest->actualLength, SPI_BLOCK_SIZE);

    /* 
     * Prepend the message's actual length to the  outgoing message.
     * Caller is REQUIRED to leave HTC_HEADER_LEN bytes before
     * the message for this purpose!
     *
     * TBD: We may enhance HIF so that a single write request
     * may have TWO consecutive components: one for the HTC header
     * and another for the payload. This would remove the burden
     * on callers to reserve space in their buffer for HTC.
     *
     * TBD: Since the messaging layer sitting on top of HTC may
     * have this same issue it may make sense to allow a Send
     * to pass in a "header buffer" along with a "payload buffer".
     * So two buffers (or more generally, a list of buffers)
     * rather than one on each call.  These buffers would be
     * guaranteed to be sent to HIF as a group and they would
     * be sent over SDIO back to back.
     */
    mboxRequest->buffer -= HTC_HEADER_LEN;

    /*
     * Target receives length in LittleEndian byte order
     * regardeless of Host endianness.
     */
    mboxRequest->buffer[0] = mboxRequest->actualLength & 0xff;
    mboxRequest->buffer[1] = (mboxRequest->actualLength >> 8) & 0xff;
	
    reqType = (endPoint->blockSize > 1) ? HIF_WR_ASYNC_BLOCK_INC : HIF_WR_ASYNC_BYTE_INC;
	
#ifndef USER_SPACE
#else
	reqType |= HIF_FIFO_ADDRESS;
#endif
	
    /*
     * Arrange for last byte of the message to generate an
     * EndOfMessage interrupt to the Target.
     */
    mboxAddress = endPoint->mboxEndAddr - paddedLength;

#ifdef HTC_TOGGLE_ENABLE
        {
            extern void toggle(void **p, int gpio_pin_num);
            
            toggle(NULL, 49);    
        }
#endif

    /* Send the request to HIF */
    status = HIFReadWrite(target->HIF_handle, mboxAddress, mboxRequest->buffer, paddedLength, reqType, mboxRequest);

    if (status == HIF_OK) {
        mboxRequest->req.completionCB((struct htc_request *)mboxRequest, HTC_OK);
        /* htcSendCompletionCB */
    } else if (status == HIF_PENDING) {
        /* Will complete later */
    } else {                    /* HIF error */
        /* Restore mboxRequest buffer */
        mboxRequest->buffer += HTC_HEADER_LEN;
        return HTC_ERROR;
    }

    return HTC_OK;
}

/*
 * Start a credit refresh cycle. Credits will appear in
 * endPoint->txCreditsAvailable when this refresh completes.
 *
 * Called in the context of the workTask when we are unable
 * to send any more requests because credits are exhausted.
 * Also called from HIF completion's context when a credit
 * interrupt occurs.
 *
 * TBD: Consider HTC v2 features: Quartz FW can send
 *    in-band TX Credit hint
 *    RX Length hint
 *    interrupt status registers
 * as opportunistic trailer(s) on an RX message. 
 * This increases code complexity but may reduce overhead
 * since we may reduce the number of explicit SDIO register
 * read operations which are relatively expensive "byte basis"
 * operations.
 */
void htcCreditRefreshStart(struct htc_endpoint *endPoint)
{
    HTC_ENDPOINT_ID endPointId;
    HIF_STATUS status;
    struct htc_target *target;
    bool alreadyInProgress;
    uint32_t address;
    uint32_t credits;
#ifndef USER_SPACE
    uint8_t  byte_credits;
    struct htc_reg_request *regRequest;
    unsigned long flags;
#endif

    HTCPrintf(HTC_PRINT_GRP11,  "htcCreditRefreshStart inprogress:%d\n", endPoint->txCreditRefreshInProgress);

#ifndef USER_SPACE
    spin_lock_irqsave(&endPoint->txCreditLock, flags);
#else
    sem_wait(&endPoint->txCreditLock);
#endif

    alreadyInProgress = endPoint->txCreditRefreshInProgress;
    endPoint->txCreditRefreshInProgress = true;

#ifndef USER_SPACE
    spin_unlock_irqrestore(&endPoint->txCreditLock, flags);
#else
    sem_post(&endPoint->txCreditLock);
#endif

    if (alreadyInProgress) {
        HTCPrintf(HTC_PRINT_GRP11,  "htcCreditRefreshStart in-progress\n");
        return;
    }

    target = endPoint->target;
    endPointId = GET_ENDPOINT_ID(endPoint);

#ifndef USER_SPACE
    spin_lock_irqsave(&target->regQueueLock, flags);

    regRequest = (struct htc_reg_request *)
        htcRequestDeqHead(&target->regFreeQueue);

    spin_unlock_irqrestore(&target->regQueueLock, flags);
    BUG_ON(!regRequest);

    regRequest->buffer = NULL;
    regRequest->length = 0;
    regRequest->purpose = CREDIT_REFRESH;
    regRequest->epid = endPointId;
#endif

    address = getRegAddr(TX_CREDIT_COUNTER_DECREMENT_REG, endPointId);

    /*
     * Note: reading many times FROM a FIXed register address,
     * the "atomic decrement address". The function htcCreditRefreshCompl
     * examines the results upon completion.
     */
    {
        uint8_t   ep_credits;
        uint32_t  i, credit_cpu_val;

        credits = 0;
        for (i=0; i<HTC_TX_CREDITS_REAP_MAX; i++)
        {
            status = HIFReadWrite(target->HIF_handle, address, &ep_credits, 
                                    sizeof(ep_credits), HIF_RD_SYNC_BYTE_INC_HOST, NULL);
            if (status != HIF_OK)
                HTCPrintf(HTC_PRINT_GRP10, "htcRegisterRefreshStart read credit fails\n");
                    
            credit_cpu_val = BE2CPU32(ep_credits);
                    
            if (credit_cpu_val <= 1)
            {
                if (credit_cpu_val == 1)
                    credits += 1;
                break;
            }
            credits ++;
        }

        if (credits != 0)
        {
            sem_wait(&endPoint->txCreditLock);
            endPoint->txCreditsAvailable += credits;
            endPoint->txCreditRefreshInProgress = false;
            sem_post(&endPoint->txCreditLock);
        }
        else
            target->need_credit_int_enable |= 1 << (endPointId+4);
    }

#ifndef USER_SPACE
    if (credits == 0)
    {
        HIFReadWrite(target->HIF_handle,
                          CREDIT_COUNT_RECV+endPointId, &byte_credits, 1, HIF_RD_SYNC_BYTE_INC_HOST, regRequest);
        HTCPrintf(HTC_PRINT_GRP11,  "credit count=%d\n", byte_credits);
        target->needRegisterRefresh = false;
    }
    regRequest->u.credit_dec_results[endPointId] = credits;
	
    if (status == HIF_OK) {
        regRequest->req.completionCB((struct htc_request *)regRequest, HIF_OK);
        /* htcCreditRefreshCompl */
    } else if (status == HIF_PENDING) {
        /* Will complete later */
    } else {                    /* HIF error */
        BUG_ON(1);
    }
#else

    if (credits != 0)
        htcWorkTaskPoke(target);
#endif
}

uint16_t htcGetMboxAddress(uint16_t start_address, uint16_t length)
{
    uint16_t address;

    address = (start_address + MBOX_WIDTH - length);

    return address;
}

/*
 * Used during Configuration Negotation at startup
 * to configure block sizes for each endpoint.
 *
 * Returns true if all endpoints have been configured,
 * by this pass and/or all earlier calls. (Typically
 * there should be only a single call which enables
 * all endpoints at once.)
 *
 * Returns false if at least one endpoint has not
 * yet been configured.
 */
bool htcNegotiateConfig(struct htc_target *target)
{
    HIF_STATUS status;
    struct htc_endpoint *endPoint;
    uint32_t address;
    int enb_count = 0;
    int ep;
    uint32_t tgt_val32;
	uint32_t   credits;

    /*
     * The Target should have posted 1 credit to
     * each endpoint by the time we reach here.
     */
    for (ep = 0; ep < HTC_NUM_MBOX; ep++) {
        endPoint = &target->endPoint[ep];
        if (endPoint->enabled) {
            /* This endpoint was already enabled */
            enb_count++;
            continue;
        }

        address = getRegAddr(TX_CREDIT_COUNTER_DECREMENT_REG, ep);

#ifndef USER_SPACE
        status = HIFReadWrite(target->HIF_handle,
                              address,
                              (uint8_t *) & endPoint->txCreditsAvailable,
                              sizeof(endPoint->txCreditsAvailable), HIF_RD_SYNC_BYTE_INC, NULL);
#else

        status = HIFReadWrite(target->HIF_handle,
                              address, (uint8_t *) &credits,
                              sizeof(credits), HIF_RD_SYNC_BYTE_INC_HOST, NULL);
		endPoint->txCreditsAvailable = (credits >> 24) & 0xFF;

		if (status != HIF_OK)
			HTCPrintf(HTC_PRINT_GRP11,  "htcNegotiateConfig  ep=%d  read credits fails\n", ep);	
#endif

        BUG_ON(status != HIF_OK);

        if (!endPoint->txCreditsAvailable) {
            /* not yet ready -- no credit posted.  Odd case. */
            continue;
        }

        endPoint->txCreditsAvailable--;
        BUG_ON(endPoint->txCreditsAvailable); /* expect only a single credit */

        /*
         * TBD: Tacitly assumes LittleEndian Host.
         * TBD: address should be set so that EOM is generated.
         * This -- rather than an explicit Host interrupt -- is
         * what should trigger Target to fetch blocksize.
         */
        /* "Negotiate" the block size for the endpoint */
#ifndef USER_SPACE
        address = endPoint->mboxStartAddr;
        status = HIFReadWrite(target->HIF_handle,
                              address,
                              (uint8_t *) & endPoint->blockSize,
                              sizeof(endPoint->blockSize), HIF_WR_SYNC_BYTE_INC, NULL);
#else
		address = htcGetMboxAddress(endPoint->mboxStartAddr, sizeof(tgt_val32));

		tgt_val32 = endPoint->blockSize;

		HTCPrintf(HTC_PRINT_GRP11,  "FIFO addr=%x value=%x  size=%d spi_val=%x\n", address, endPoint->blockSize, sizeof(endPoint->blockSize), tgt_val32);
        status = HIFReadWrite(target->HIF_handle,
                              address,
                              (uint8_t *)&tgt_val32,
                              sizeof(tgt_val32), HIF_WR_SYNC_BYTE_INC_FIFO, NULL);

#endif

        BUG_ON(status != HIF_OK);

        endPoint->enabled = true;
        enb_count++;
    }

#ifndef USER_SPACE
#else
	uint8_t	 int_vector = 1;
	uint8_t  host_int_status;
	uint16_t  int_cause;
	
	status = HIFReadWrite(target->HIF_handle, 0x400,
                          (uint8_t *)&host_int_status, sizeof(host_int_status), HIF_RD_SYNC_BYTE_INC_HOST, NULL);

	HTCPrintf(HTC_PRINT_GRP11,  "htcNegotiateConfig  host_int_status=%x\n", host_int_status);

	status = HIFReadWrite(target->HIF_handle, 0xC00,
                          (uint8_t *)&int_cause, sizeof(int_cause), HIF_RD_SYNC_INTERFACE, NULL);
	HTCPrintf(HTC_PRINT_GRP11,  "htcNegotiateConfig  int cause=%x\n", int_cause);
	
	status = HIFReadWrite(target->HIF_handle, INT_TARGET_ADDRESS,
                          (uint8_t *)&int_vector, sizeof(int_vector), HIF_WR_SYNC_BYTE_INC_HOST, NULL);

	target->intr_state = HTC_INTERRUPT;     // process first credits

#endif

    return (enb_count == HTC_NUM_MBOX);
}
