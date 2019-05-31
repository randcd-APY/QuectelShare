/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*
 * Copyright (c) 2004-2006 Atheros Communications Inc.
 * All rights reserved.
 */

/*
 * Implementation of Host Target Communication
 * API v1 and HTC Protocol v1
 * over Qualcomm QCA mailbox-based SDIO/SPI interconnects.
 */

#ifndef USER_SPACE

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/slab.h>

#else
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <malloc.h>
#endif

#include "htc_internal.h"
#include "spi_regs.h"
#include "hif/spi/hif.h"

#include "mbox_host_reg.h"
#include "htc_mbox_internal.h"
#include "dbg.h"

HTC_TARGET *HTCTargetList[HTC_NUM_DEVICES_MAX];

/*
 * Single thread module initialization, module shutdown,
 * target start and target stop.
 */
#ifndef USER_SPACE
static DEFINE_MUTEX(HTC_startup_mutex);
#else
static sem_t HTC_startup_mutex;
#endif

static bool HTCInitialized = false;

void _HTCStop(HTC_TARGET * target);

/*
 * Initialize the HTC software module.
 * Typically invoked exactly once.
 */
HTC_STATUS HTCInit(void *pInfo)
{
	HIF_STATUS status;
	
    struct cbs_from_os callbacks;

	HTCPrintf(HTC_PRINT_GRP9, "HTCInit...\n");
#ifndef USER_SPACE
    if (mutex_lock_interruptible(&HTC_startup_mutex)) {
        return HTC_ERROR;       /* interrupted */
    }
#else
    sem_init(&HTC_startup_mutex, 0, 1);	

	if (sem_wait(&HTC_startup_mutex)) {
        return HTC_ERROR;       /* interrupted */
    }
#endif

    if (HTCInitialized) {
#ifndef USER_SPACE
        mutex_unlock(&HTC_startup_mutex);
#else
        sem_post(&HTC_startup_mutex);
#endif
		return HTC_OK;          /* Already initialized */
    }

    HTCInitialized = true;

    memset(&callbacks, 0, sizeof(callbacks));
    callbacks.deviceInsertedHandler = htcTargetInsertedHandler;
    callbacks.deviceRemovedHandler = htcTargetRemovedHandler;
	
	HTCPrintf(HTC_PRINT_GRP9, "HTCInit\n");
	
    status = HIFInit(pInfo, &callbacks);
#ifndef USER_SPACE
    mutex_unlock(&HTC_startup_mutex);
#else
    sem_post(&HTC_startup_mutex);
#endif

    if (status != HIF_OK)
	{
		printf("HIFInit fails\n");
	}
    return status;
}

/*
 * Shutdown the entire module and free all module data.
 * Inverse of HTCInit.
 *
 * May be invoked only after all Targets are stopped.
 */
void HTCShutDown(void)
{
    int i;

#ifndef USER_SPACE
    if (mutex_lock_interruptible(&HTC_startup_mutex)) {
        return;                 /* interrupted */
    }
#else
    sem_wait(&HTC_startup_mutex);

#endif

    if (!HTCInitialized) {
#ifndef USER_SPACE
        mutex_unlock(&HTC_startup_mutex);
#else
        sem_post(&HTC_startup_mutex);
#endif
		return;                 /* Not initialized, so nothing to shut down */
    }

    for (i = 0; i < HTC_NUM_DEVICES_MAX; i++) {
        if (htcTargetInstance(i)) {
            /*
             * One or more Targets are still active --
             * cannot shutdown software.
             */
#ifndef USER_SPACE
            mutex_unlock(&HTC_startup_mutex);
#else
			sem_post(&HTC_startup_mutex);
#endif
			BUG_ON(1);
            return;
        }
    }

    HIFShutDownDevice(NULL);    /* Tell HIF that we're all done */
    HTCInitialized = false;

#ifndef USER_SPACE
    mutex_unlock(&HTC_startup_mutex);
#else
	sem_post(&HTC_startup_mutex);	
#endif
}

/*
 * Start a Target. This typically happens once per Target after
 * the module has been initialized and a Target is powered on.
 *
 * When a Target starts, it posts a single credit to each mailbox
 * and it enters "HTC configuration". During configuration
 * negotiation, block sizes for each HTC endpoint are established
 * that both Host and Target agree. Once this is complete, the
 * Target starts normal operation so it can send/receive.
 */
HTC_STATUS HTCStart(HTC_TARGET * target)
{
    HIF_STATUS status;
    uint32_t address;

#ifndef USER_SPACE
    mutex_lock(&HTC_startup_mutex);
#else
    sem_wait(&HTC_startup_mutex);
#endif

    if (!HTCInitialized) {
#ifndef USER_SPACE
        mutex_unlock(&HTC_startup_mutex);
#else
        sem_post(&HTC_startup_mutex);
#endif
		return HTC_ERROR;
    }

#ifndef USER_SPACE
    init_waitqueue_head(&target->targetInitWait);
#else
	target->targetInitWait = htc_signal_init();
#endif

    /* Unmask Host controller interrupts associated with this Target */
    HIFUnMaskInterrupt(target->HIF_handle);

    /* Enable all interrupts of interest on the Target. */

    target->enb.int_status_enb =
        INT_STATUS_ENABLE_ERROR_SET(0x01) |
        INT_STATUS_ENABLE_CPU_SET(0x01) | INT_STATUS_ENABLE_COUNTER_SET(0x01) | INT_STATUS_ENABLE_MBOX_DATA_SET(0x0F);

    target->enb.cpu_int_status_enb = CPU_INT_STATUS_ENABLE_BIT_SET(0x00);

    target->enb.err_status_enb = ERROR_STATUS_ENABLE_RX_UNDERFLOW_SET(0x01) | ERROR_STATUS_ENABLE_TX_OVERFLOW_SET(0x01);

    target->enb.counter_int_status_enb = COUNTER_INT_STATUS_ENABLE_BIT_SET(0xFF);

    /* Commit interrupt register values to Target HW. */
    address = getRegAddr(INTR_ENB_REG, ENDPOINT_UNUSED);
	
#ifndef USER_SPACE
    status = HIFReadWrite(target->HIF_handle, address, &target->enb, sizeof(target->enb), HIF_WR_SYNC_BYTE_INC, NULL);
#else
    status = HIFReadWrite(target->HIF_handle, address, &target->enb, 1, HIF_WR_SYNC_BYTE_INC_HOST, NULL);

    HIFEnableInterrupt(target->HIF_handle, SPI_INTR_ENABLE);
#endif
		
    if (status != HIF_OK) 
	{
        _HTCStop(target);
#ifndef USER_SPACE
        mutex_unlock(&HTC_startup_mutex);
#else
        sem_post(&HTC_startup_mutex);
#endif
		return HTC_ERROR;
    }

    /*
     * At this point, we're waiting for the Target to post
     * 1 credit to each mailbox. This allows us to begin
     * configuration negotiation. We should see an interrupt
     * as soon as the first credit is posted. The remaining
     * credits should be posted almost immediately after.
     */

    /*
     * Wait indefinitely until configuration negotiation with
     * the Target completes and the Target tells us it is ready to go.
     */
    if (!target->ready) 
	{
        /*
         * NB: Retain the HTC_statup_mutex during this wait.
         * This serializes startup but should be OK.
         */

#ifndef USER_SPACE
        wait_event_interruptible(target->targetInitWait, target->ready);
#else
		target->ready = htc_signal_wait(target->targetInitWait, TARGET_INIT_DONE, HTC_EVENT_WAIT_ANY_CLEAR_ALL);
#endif
        if (target->ready) {
            status = HTC_OK;
        } else {
            status = HTC_ERROR;
            _HTCStop(target);
        }
    }

#ifndef USER_SPACE
    mutex_unlock(&HTC_startup_mutex);
#else
    sem_post(&HTC_startup_mutex);
#endif

    return status;
}

void _HTCStop(HTC_TARGET * target)
{
#ifndef USER_SPACE
    uint ep;
#else
    uint8_t ep;
#endif
    struct htc_endpoint *endPoint;
    uint32_t address;

    /* Note: HTC_startup_mutex must be held on entry */

    if (!HTCInitialized) {
        return;
    }

    htcWorkTaskStop(target);

    /* Disable interrupts at source, on Target */
    target->enb.int_status_enb = 0;
    target->enb.cpu_int_status_enb = 0;
    target->enb.err_status_enb = 0;
    target->enb.counter_int_status_enb = 0;

    address = getRegAddr(INTR_ENB_REG, ENDPOINT_UNUSED);

    /* Try to disable all interrupts on the Target. */
#ifndef USER_SPACE
    (void)HIFReadWrite(target->HIF_handle, address, &target->enb, sizeof(target->enb), HIF_WR_SYNC_BYTE_INC, NULL);
#else
    (void)HIFReadWrite(target->HIF_handle, address, &target->enb, sizeof(target->enb), HIF_WR_SYNC_BYTE_INC_HOST, NULL);
#endif

    /* Disable Host controller interrupts */
    HIFMaskInterrupt(target->HIF_handle);

    /* Flush all the queues and return the buffers to their owner */
    for (ep = 0; ep < HTC_NUM_MBOX; ep++) {
#ifndef USER_SPACE
        unsigned long flags;
#endif
        endPoint = &target->endPoint[ep];

#ifndef USER_SPACE
        spin_lock_irqsave(&endPoint->txCreditLock, flags);
#else
		sem_wait(&endPoint->txCreditLock);
#endif
		endPoint->txCreditsAvailable = 0;
		
#ifndef USER_SPACE
        spin_unlock_irqrestore(&endPoint->txCreditLock, flags);
#else
		sem_post(&endPoint->txCreditLock);
#endif
        endPoint->enabled = false;

        /* Flush the Pending Receive Queue */
        htcMboxQueueFlush(endPoint, &endPoint->recvPendingQueue, &endPoint->recvFreeQueue, HTC_EVENT_BUFFER_RECEIVED);

        /* Flush the Pending Send Queue */
        htcMboxQueueFlush(endPoint, &endPoint->sendPendingQueue, &endPoint->sendFreeQueue, HTC_EVENT_BUFFER_SENT);
    }

    target->ready = false;

    HIFDetach(target->HIF_handle);

    /* Remove this Target from the global list */
    htcTargetInstanceRemove(target);

    /* Free target memory */
#ifndef USER_SPACE
    kfree(target);
#else
    free(target);
#endif
}

void HTCStop(HTC_TARGET * target)
{
    htcWorkTaskStop(target);
    htcComplTaskStop(target);

#ifndef USER_SPACE
    mutex_lock(&HTC_startup_mutex);
#else
    sem_wait(&HTC_startup_mutex);

#endif
    _HTCStop(target);
#ifndef USER_SPACE
    mutex_unlock(&HTC_startup_mutex);
#else
    sem_post(&HTC_startup_mutex);	
#endif
}

/* 
 * Provides an interface for the caller to register for 
 * various events supported by the HTC module.
 */
HTC_STATUS
HTCEventReg(HTC_TARGET * target, HTC_ENDPOINT_ID endPointId,
            HTC_EVENT_ID eventId, HTC_EVENT_HANDLER eventHandler, void *param)
{
    HTC_STATUS status;
    struct htc_endpoint *endPoint;
    HTC_EVENT_INFO eventInfo;

	HTCPrintf(HTC_PRINT_GRP9, "HTCEventReg...\n");
    /*
     * Register a new handler BEFORE dispatching events.
     * UNregister a handler AFTER dispatching events.
     */

    if (eventHandler) {
        /* Register a new event handler */
	
		HTCPrintf(HTC_PRINT_GRP9, "endPointId=%d\n", endPointId);
		
        if ((status = htcAddToEventTable(target, endPointId, eventId, eventHandler, param)) != HTC_OK) {
            return status;      /* Fail to register handler */
        }
    }
	HTCPrintf(HTC_PRINT_GRP9, "HTCEventReg...#1\n");

    /* Handle events associated with this handler */
    switch (eventId) {
    case HTC_EVENT_TARGET_AVAILABLE:
        if (eventHandler) {
            HTC_TARGET *targ;
            int i;

            /* 
             * Dispatch a Target Available event for all Targets 
             * that are already present.
             */
            for (i = 0; i < HTC_NUM_DEVICES_MAX; i++) {
                targ = HTCTargetList[i];
                if (targ) {
                    FRAME_EVENT(eventInfo,
                                (uint8_t *) targ->HIF_handle, sizeof(HIF_DEVICE *), sizeof(HIF_DEVICE *), HTC_OK, NULL);

                    htcDispatchEvent(targ, ENDPOINT_UNUSED, HTC_EVENT_TARGET_AVAILABLE, &eventInfo);
                }
            }
        }
        break;

    case HTC_EVENT_TARGET_UNAVAILABLE:
        break;

    case HTC_EVENT_BUFFER_RECEIVED:
        if (!eventHandler) {
            /*
             * Flush the Pending Recv queue before unregistering
             * the event handler.
             */
            endPoint = &target->endPoint[endPointId];
            htcMboxQueueFlush(endPoint, &endPoint->recvPendingQueue,
                              &endPoint->recvFreeQueue, HTC_EVENT_BUFFER_RECEIVED);

        }
        break;

    case HTC_EVENT_BUFFER_SENT:
        if (!eventHandler) {
            /*
             * Flush the Pending Send queue before unregistering
             * the event handler.
             */
            endPoint = &target->endPoint[endPointId];
            htcMboxQueueFlush(endPoint, &endPoint->sendPendingQueue, &endPoint->sendFreeQueue, HTC_EVENT_BUFFER_SENT);
        }
        break;

    case HTC_EVENT_DATA_AVAILABLE:
        /* 
         * We could dispatch a data available event. Instead,
         * we require users to register this event handler
         * before posting receive buffers.
         */
        break;

    default:
        return HTC_EINVAL;      /* unknown event? */
    }
	HTCPrintf(HTC_PRINT_GRP9, "HTCEventReg...#2\n");

    if (!eventHandler) {
        /* Unregister an event handler */
        if ((status = htcRemoveFromEventTable(target, endPointId, eventId)) != HTC_OK) {
            return status;
        }
    }

    return HTC_OK;
}

/*
 * Enqueue to the endpoint's recvPendingQueue an empty buffer
 * which will receive data from the Target.
 */
HTC_STATUS
HTCBufferReceive(HTC_TARGET * target, HTC_ENDPOINT_ID endPointId, uint8_t * buffer, uint32_t length, void *cookie)
{
    struct htc_endpoint *endPoint;
    struct htc_mbox_request *mboxRequest;
    struct htc_event_table_element *ev;
#ifndef USER_SPACE
    unsigned long flags;
#endif

    endPoint = &target->endPoint[endPointId];

    if (!endPoint->enabled) {
        return HTC_ERROR;
    }

    ev = htcEventIDtoEvent(target, endPointId, HTC_EVENT_BUFFER_RECEIVED);
    if (ev->handler == NULL) {
        /*
         * In order to use this API, caller must
         * register an event handler for HTC_EVENT_BUFFER_RECEIVED.
         */
        return HTC_ERROR;
    }

#ifndef USER_SPACE
    spin_lock_irqsave(&endPoint->mboxQueueLock, flags);
#else
  	sem_wait(&endPoint->mboxQueueLock);
#endif
    mboxRequest = (struct htc_mbox_request *)htcRequestDeqHead(&endPoint->recvFreeQueue);
#ifndef USER_SPACE
    spin_unlock_irqrestore(&endPoint->mboxQueueLock, flags);
#else
   	sem_post(&endPoint->mboxQueueLock);
#endif
    if (!mboxRequest) {
        return HTC_ENOMEM;
    }

    mboxRequest->buffer = buffer;
    mboxRequest->bufferLength = length; /* plus HTC_HEADER_LENGTH_MAX */
    mboxRequest->actualLength = 0;      /* filled in after message is received */
    mboxRequest->endPoint = endPoint;
    mboxRequest->cookie = cookie;

#ifndef USER_SPACE
    spin_lock_irqsave(&endPoint->mboxQueueLock, flags);
#else
  	sem_wait(&endPoint->mboxQueueLock);
#endif
    htcRequestEnqTail(&endPoint->recvPendingQueue, (struct htc_request *)mboxRequest);
#ifndef USER_SPACE
    spin_unlock_irqrestore(&endPoint->mboxQueueLock, flags);
#else
   	sem_post(&endPoint->mboxQueueLock);	
#endif
    /* Alert the Target's task that there may be work to do */
    htcWorkTaskPoke(target);

    return HTC_OK;
}

/*
 * Enqueue a buffer to be sent to the Target.
 * Supplied buffer must be preceded by HTC_HEADER_LEN bytes for the HTC header.
 * Must be followed with sufficient space for block-size padding.
 *
 * Example:
 * To send a 10B message over an endpoint that uses 64B blocks, caller
 * specifies length=10. HTC adds HTC_HEADER_LEN bytes just before buffer.
 * HTC sends 64B starting at buffer-HTC_HEADER_LEN.
 */
HTC_STATUS
HTCBufferSend(HTC_TARGET * target, HTC_ENDPOINT_ID endPointId, uint8_t * buffer, uint32_t length, void *cookie)
{
    struct htc_endpoint *endPoint;
    struct htc_mbox_request *mboxRequest;
    struct htc_event_table_element *ev;
#ifndef USER_SPACE
    unsigned long flags;
#endif

    endPoint = &target->endPoint[endPointId];

    if (!endPoint->enabled) {
        return HTC_ERROR;
    }

    ev = htcEventIDtoEvent(target, endPointId, HTC_EVENT_BUFFER_SENT);

    if (ev->handler == NULL) {
        /*
         * In order to use this API, caller must
         * register an event handler for HTC_EVENT_BUFFER_SENT.
         */
        return HTC_ERROR;
    }

#ifndef USER_SPACE
    spin_lock_irqsave(&endPoint->mboxQueueLock, flags);
#else
	sem_wait(&endPoint->mboxQueueLock);
#endif

    mboxRequest = (struct htc_mbox_request *)htcRequestDeqHead(&endPoint->sendFreeQueue);
	
#ifndef USER_SPACE
    spin_unlock_irqrestore(&endPoint->mboxQueueLock, flags);
#else
	sem_post(&endPoint->mboxQueueLock);
#endif

    if (!mboxRequest) {
        return HTC_ENOMEM;
    }

    mboxRequest->buffer = buffer;
    mboxRequest->bufferLength = length;
    mboxRequest->actualLength = length;
    mboxRequest->endPoint = endPoint;
    mboxRequest->cookie = cookie;

#ifndef USER_SPACE
    spin_lock_irqsave(&endPoint->mboxQueueLock, flags);
#else
	sem_wait(&endPoint->mboxQueueLock);
#endif

    htcRequestEnqTail(&endPoint->sendPendingQueue, (struct htc_request *)mboxRequest);
	
#ifndef USER_SPACE
    spin_unlock_irqrestore(&endPoint->mboxQueueLock, flags);
#else
	sem_post(&endPoint->mboxQueueLock);	
#endif
    /* Alert the workTask that there may be work to do */
    htcWorkTaskPoke(target);

    return HTC_OK;
}

uint32_t HTCGetBlockSize(HTC_TARGET * target, HTC_ENDPOINT_ID epId)
{
    struct htc_endpoint *endPoint;

    endPoint = &target->endPoint[epId];
	return endPoint->blockSize;
}
