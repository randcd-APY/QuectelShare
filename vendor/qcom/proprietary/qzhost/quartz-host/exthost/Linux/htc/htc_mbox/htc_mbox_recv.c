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
#include <pthread.h>

#endif

#include "htc_internal.h"
#include "hif/spi/hif.h"
#include "hif/spi/spi_regs.h"

#include "mbox_host_reg.h"
#include "htc_mbox_internal.h"
#include "dbg.h"

/*
 * If there is data available to read on the specified mailbox,
 * pull a Mailbox Recv Request off of the PendingRecv queue
 * and request HIF to pull data from the mailbox into the
 * request's recv buffer.
 *
 * If we are not aware of data waiting on the endpoint, simply
 * return. Note that our awareness is based on slightly stale
 * data from Quartz registers. Upper layers insure that we are
 * called shortly after data becomes available on an endpoint.
 *
 * If we exhaust receive buffers, disable the mailbox's interrupt
 * until additional buffers are available.
 *
 * Returns 0 if no request was sent to HIF
 * returns 1 if at least one request was sent to HIF
 */
int htcManagePendingRecvs(struct htc_target *target, int epid)
{
    struct htc_endpoint *endPoint;
    struct htc_request_queue *recvQueue;
    struct htc_mbox_request *mboxRequest;
    uint32_t rxFrameLength;
    int workDone = 0;

    endPoint = &target->endPoint[epid];

    /* 
     * Hand off requests as long as we have both
     *    something to recv into
     *    data waiting to be read on the mailbox
     */

    /*
     * rxFrameLength of 0 --> nothing waiting;
     * otherwise, it's the length of data waiting
     * to be read, including HTC header.
     */
    rxFrameLength = endPoint->rxFrameLength;

    recvQueue = &endPoint->recvPendingQueue;
    if (HTC_IS_QUEUE_EMPTY(recvQueue)) {

        /*
         * Not interested in rxdata interrupts
         * since we have no recv buffers.
         */
        target->enb.int_status_enb &= ~(1 << epid);
		

        if (rxFrameLength) {
            HTC_EVENT_INFO eventInfo;
            
            HTCPrintf(HTC_PRINT_GRP11,  "htcManagePendingRecvs queue empty\n");

            /*
             * No buffer ready to receive but data
             * is ready. Alert the caller with a
             * DATA_AVAILABLE event.
             */
            if (!endPoint->rxDataAlerted) {
                endPoint->rxDataAlerted = true;

                FRAME_EVENT(eventInfo, NULL, rxFrameLength, 0, HTC_OK, NULL);

                htcDispatchEvent(target, epid, HTC_EVENT_DATA_AVAILABLE, &eventInfo);
            }

            return 0;
        }
#ifndef USER_SPACE
#else
        return 0;
#endif
    }

    /*
     * We have recv buffers available, so we are
     * interested in rxdata interrupts.
     */
    target->enb.int_status_enb |= (1 << epid);
    endPoint->rxDataAlerted = false;


    if (rxFrameLength == 0) {
        /*
         * We have a buffer but there's nothing
         * available on the Target to read.
         */
        return 0;
    }

    /* There is rxdata waiting and a buffer to read it into */

    /* Pull the request buffer off the Pending Recv Queue */
#ifndef USER_SPACE
    spin_lock_irqsave(&endPoint->mboxQueueLock, flags);
#else
	sem_wait(&endPoint->mboxQueueLock);
#endif

    mboxRequest = (struct htc_mbox_request *)
        htcRequestDeqHead(recvQueue);
		
#ifndef USER_SPACE
    spin_unlock_irqrestore(&endPoint->mboxQueueLock, flags);
#else
	sem_post(&endPoint->mboxQueueLock);
#endif

    /* Hand off this Mbox Recv request to HIF */
    mboxRequest->actualLength = rxFrameLength;

#ifndef USER_SPACE
#else
	endPoint->rxFrameLength = 0;
#endif

    if (htcRecvRequestToHIF(endPoint, mboxRequest) == HTC_ERROR) {
        HTC_EVENT_INFO eventInfo;

        /*
         * TBD: Could requeue this at the HEAD of the
         * pending recv queue. Try again later?
         */

        /* Frame an event to send to caller */
        FRAME_EVENT(eventInfo, mboxRequest->buffer,
                    mboxRequest->bufferLength,
                    mboxRequest->actualLength - HTC_HEADER_LEN, HTC_ECANCELED, mboxRequest->cookie);

        /* Free the Mailbox request */
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

        htcDispatchEvent(target, epid, HTC_EVENT_BUFFER_RECEIVED, &eventInfo);
        goto done;
    } else {
        workDone = 1;
        target->needRegisterRefresh = true;
    }

 done:
    return workDone;
}

HTC_STATUS htcRecvRequestToHIF(struct htc_endpoint * endPoint, struct htc_mbox_request * mboxRequest)
{
    HIF_STATUS status;
    HTC_TARGET *target;
    uint32_t paddedLength;
    uint32_t mboxAddress;
    uint32_t reqType;

    target = endPoint->target;
    mboxRequest->buffer -= HTC_HEADER_LEN;

#ifndef USER_SPACE
    /* Adjust length for power-of-2 block size */
    paddedLength = HTC_ROUND_UP(mboxRequest->actualLength, endPoint->blockSize);

    reqType = (endPoint->blockSize > 1) ? HIF_WR_ASYNC_BLOCK_INC : HIF_WR_ASYNC_BYTE_INC;

    mboxAddress = endPoint->mboxStartAddr;

    status = HIFReadWrite(target->HIF_handle, mboxAddress, mboxRequest->buffer, paddedLength, reqType, mboxRequest);
#else
    paddedLength = HTC_ROUND_UP(mboxRequest->actualLength, SPI_BLOCK_SIZE);
    reqType = (endPoint->blockSize > 1) ? HIF_RD_ASYNC_BLOCK_INC_FIFO : HIF_RD_ASYNC_BYTE_INC_FIFO;

    mboxAddress = endPoint->mboxStartAddr + MBOX_WIDTH - paddedLength;

#ifdef HTC_TOGGLE_ENABLE
    {
        extern void toggle(void **p, int gpio_pin_num);
            
        toggle(NULL, 49);    
    }
#endif
                                
    HIFInterruptLock(target->HIF_handle);
    status = HIFReadWrite(target->HIF_handle, mboxAddress, mboxRequest->buffer, paddedLength, reqType, mboxRequest);

#ifdef HTC_TOGGLE_ENABLE
    {
        extern void toggle(void **p, int gpio_pin_num);
            
        toggle(NULL, 49);    
    }
#endif

#endif
    HIFPacketAvailReset(target->HIF_handle);

    if (status == HIF_OK) {
        mboxRequest->req.completionCB((struct htc_request *)mboxRequest, HTC_OK);
        /* htcRecvCompl */
    } else if (status == HIF_PENDING) {
        /* Will complete later */
    } else {                    /* HIF error */
        /* Restore mboxRequest buffer */
        mboxRequest->buffer += HTC_HEADER_LEN;
        mboxRequest->actualLength -= HTC_HEADER_LEN;
        return HTC_ERROR;
    }

    return HTC_OK;
}
