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

#include "htc_internal.h"
#include "hif/spi/hif.h"

#include "mbox_host_reg.h"
#include "htc_mbox_internal.h"
#include "spi_regs.h"
#include "dbg.h"


/* HTC utility routines  */

/* Invoked when shutting down */
void
htcMboxQueueFlush(struct htc_endpoint *endPoint,
                  struct htc_request_queue *pendingQueue, struct htc_request_queue *freeQueue, HTC_EVENT_ID eventId)
{
    HTC_EVENT_INFO eventInfo;
    HTC_ENDPOINT_ID endPointId;
    HTC_TARGET *target;
    struct htc_mbox_request *mboxRequest;
#ifndef USER_SPACE
    unsigned long flags;
#endif

    target = endPoint->target;
    endPointId = GET_ENDPOINT_ID(endPoint);

#ifndef USER_SPACE
    spin_lock_irqsave(&endPoint->mboxQueueLock, flags);
#else
	sem_wait(&endPoint->mboxQueueLock);
#endif
    for (;;) {
        mboxRequest = (struct htc_mbox_request *)
            htcRequestDeqHead(pendingQueue);
#ifndef USER_SPACE
        spin_unlock_irqrestore(&endPoint->mboxQueueLock, flags);
#else
		sem_post(&endPoint->mboxQueueLock);
#endif

        if (!mboxRequest) {
            break;
        }

        FRAME_EVENT(eventInfo, mboxRequest->buffer, mboxRequest->bufferLength, 0, HTC_ECANCELED, mboxRequest->cookie);

        htcDispatchEvent(target, endPointId, eventId, &eventInfo);

        /* Recycle the request */
#ifndef USER_SPACE
        spin_lock_irqsave(&endPoint->mboxQueueLock, flags);
#else
		sem_wait(&endPoint->mboxQueueLock);
#endif
		htcRequestEnqTail(freeQueue, (struct htc_request *)mboxRequest);
    }
#ifndef USER_SPACE
    spin_unlock_irqrestore(&endPoint->mboxQueueLock, flags);
#else
	sem_post(&endPoint->mboxQueueLock);
#endif
}

void htcReportFailure(HTC_STATUS status)
{
    BUG_ON(1);                  /* TBD */
}

HTC_TARGET *htcTargetInstance(int i)
{
    return HTCTargetList[i];
}

void htcTargetInstanceAdd(HTC_TARGET * target)
{
    int i;

    for (i = 0; i < HTC_NUM_DEVICES_MAX; i++) {
        if (HTCTargetList[i] == NULL) {
            HTCTargetList[i] = target;
            break;
        }
    }
    BUG_ON(i >= HTC_NUM_DEVICES_MAX);
}

void htcTargetInstanceRemove(HTC_TARGET * target)
{
    int i;

    for (i = 0; i < HTC_NUM_DEVICES_MAX; i++) {
        if (HTCTargetList[i] == target) {
            HTCTargetList[i] = NULL;
            break;
        }
    }
    BUG_ON(i >= HTC_NUM_DEVICES_MAX);
}

/*
 * Add a request to the tail of a queue.
 * Caller must handle any locking required.
 * TBD: Use Linux queue support
 */
void htcRequestEnqTail(struct htc_request_queue *queue, struct htc_request *req)
{
    req->next = NULL;

    if (queue->tail) {
        queue->tail->next = req;
    } else {
        queue->head = req;
    }

    queue->tail = req;
}

/*
 * Remove a request from the start of a queue.
 * Caller must handle any locking required.
 * TBD: Use Linux queue support
 * TBD: If cannot allocate from FREE queue, caller may add more elements.
 */
struct htc_request *htcRequestDeqHead(struct htc_request_queue *queue)
{
    struct htc_request *req;

    req = queue->head;
    if (!req) {
        return NULL;
    }

    queue->head = req->next;
    if (queue->head == NULL) {
        queue->tail = NULL;
    }
    req->next = NULL;

    return req;
}

/*
 * processing packet avail interrupt
 * return     1    packet is available
 *            0    no packet
 */
int packet_avail(struct htc_target *target, uint16_t *pIntCause)
{
    struct htc_endpoint *endPoint;
    uint32_t  frameLength;
    uint16_t  int_cause;
    uint16_t  lookahead1;
    HIF_STATUS status;

    if (HIFIsPacketAvailInt(target->HIF_handle) == true)
    {
        status = HIFReadWrite(target->HIF_handle, SPI_REG_RDBUF_LOOKAHEAD1, 
                                &lookahead1, sizeof(int_cause), HIF_RD_SYNC_INTERFACE, NULL);
        if ( status != HIF_OK)
        {
            HTCPrintf(HTC_PRINT_GRP1, "F: packet_avail hd1[0x%02x]\n", lookahead1);           
            return 0;
        }    
        

        endPoint = &target->endPoint[0];

        frameLength = BE2CPU16(lookahead1);
        if (frameLength && frameLength <= endPoint->blockSize)
        {
            frameLength += HTC_HEADER_LEN;                        // quartz side htc issue
            endPoint->rxFrameLength = frameLength;
            HIFPacketAvailSet(target->HIF_handle);
            
            HIFSetIntInProcessVar(target->HIF_handle);
        }

#ifdef  DEBUG_HTC
        {
            uint16_t    rdbuf_rdptr, rdbuf_wrptr;

            status = HIFReadWrite(target->HIF_handle, SPI_REG_RDBUF_RDPTR, 
                                &rdbuf_rdptr, sizeof(rdbuf_rdptr), HIF_RD_SYNC_INTERFACE, NULL);

            status = HIFReadWrite(target->HIF_handle, SPI_REG_RDBUF_WRPTR, 
                                &rdbuf_wrptr, sizeof(rdbuf_wrptr), HIF_RD_SYNC_INTERFACE, NULL);
        }
#endif

    }
    return 1;
}

/*
 * Start a Register Refresh cycle.
 *
 * Submits a request to fetch ALL relevant registers from Target.
 * When this completes, we'll take actions based on the new
 * register values.
 */
void htcRegisterRefreshStart(struct htc_target *target)
{
    HIF_STATUS status;
    uint32_t address;
    uint32_t pkt_avail = 0;

#ifndef USER_SPACE
    struct htc_reg_request *regRequest;
    unsigned long flags;

    lendebug("Enter htcRegisterRefreshStart\n");

    spin_lock_irqsave(&target->regQueueLock, flags);

    regRequest = (struct htc_reg_request *)htcRequestDeqHead(&target->regFreeQueue);

    spin_unlock_irqrestore(&target->regQueueLock, flags);

    BUG_ON(!regRequest);
    regRequest->buffer = (uint8_t *) & regRequest->u.regTable;
    regRequest->length = sizeof(regRequest->u.regTable);
    regRequest->purpose = INTR_REFRESH;
    regRequest->epid = 0;       /* not used */

    address = getRegAddr(ALL_STATUS_REG, ENDPOINT_UNUSED);

    status = HIFReadWrite(target->HIF_handle,
                          address,
       &regRequest->u.regTable, sizeof(regRequest->u.regTable), HIF_RD_ASYNC_BYTE_INC, regRequest);

    if (status == HIF_OK) {
        HTCPrintf(HTC_PRINT_GRP10, "htcRegisterRefreshStart counter int status=0x%02X\n", regRequest->u.regTable.status.counter_int_status);

        regRequest->req.completionCB((struct htc_request *)regRequest, HIF_OK);
        /* htcRegisterRefreshCompl */
    } else if (status == HIF_PENDING) {
        HTCPrintf(HTC_PRINT_GRP10, "htcRegisterRefreshStart Pending\n");
        /* Will complete later */
    } else {                    /* HIF error */
        HTCPrintf(HTC_PRINT_GRP10, "htcRegisterRefreshStart Bug\n");
        BUG_ON(1);
    }

#else
    struct htc_endpoint *endPoint;
    int   poke = 0;
    uint16_t  int_cause;
    
    pkt_avail = packet_avail(target, &int_cause);

    address = getRegAddr(ALL_STATUS_REG, ENDPOINT_UNUSED);

/*
 *  read credit interrupt registers
 */
    if (HIFIsCreditInt(target->HIF_handle) == true)
    {
        uint8_t   int_status, mask, epid, reaped;
        uint32_t  credits, i, credit_val;

        status = HIFReadWrite(target->HIF_handle,
                                COUNTER_INT_STATUS_ADDRESS, &int_status, 
                                sizeof(int_status), HIF_RD_SYNC_BYTE_INC_HOST, /*regRequest*/NULL);

        HIFDisableInterrupt(target->HIF_handle, INTR_ENABLE_CREDIT_COUNT);

        address = CREDIT_COUNT_DEC_RECV;
        mask = 0x10;

        for (epid=0; epid<4; epid++, address+=4, mask <<= 1)
        {
            if ((int_status & /*int_status_mask & */mask) == 0)
                continue;

            reaped = 0;
            for (i=0; i<HTC_TX_CREDITS_REAP_MAX; i++)
            {
                status = HIFReadWrite(target->HIF_handle, address, &credits, 
                                sizeof(credits), HIF_RD_SYNC_BYTE_INC_HOST, NULL);
                if (status != HIF_OK)
                    HTCPrintf(HTC_PRINT_GRP10, "htcRegisterRefreshStart read credit fails\n");
                
                credit_val = BE2CPU32(credits);
                
                if (credit_val <= 1)
                {
                    if (credit_val == 1)
                        reaped += 1;
                    break;
                }
                reaped ++;
            }

            if (reaped != 0)
            {
                endPoint = &target->endPoint[epid];

                sem_wait(&endPoint->txCreditLock);

                endPoint->txCreditsAvailable += reaped;
                endPoint->txCreditRefreshInProgress = false;

                sem_post(&endPoint->txCreditLock);

                poke = 1;
            }
        }
    }    


/*
 *  wake up WorkTask 
 */

    if (poke || (pkt_avail != 0))
    {
        htcWorkTaskPoke(target);
    }

#endif
}
