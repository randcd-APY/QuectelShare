/*
 * Copyright (c) 2013-2018 The Linux Foundation. All rights reserved.
 *
 * Previously licensed under the ISC license by Qualcomm Atheros, Inc.
 *
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * This file was originally distributed by Qualcomm Atheros, Inc.
 * under proprietary terms before Copyright ownership was assigned
 * to the Linux Foundation.
 */
 
#ifndef USER_SPACE

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/mmc/card.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/host.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/sdio.h>
#include <linux/mmc/sd.h>
#define lendebug(args...) printk(args)

#else
#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <pthread.h>
#include <string.h>

#include "spi_regs.h"
#include "htc_internal.h"
#include "hif_spi_rw.h"
#include "dbg.h"

#define	sdio_release_host(a)
#define	sdio_claim_host(a)
#define	sdio_claim_irq(a,b)  (1)
#define	sdio_release_irq(a)	(0)

#define lendebug(args...)

#endif

#include "hif.h"
#include "hif_internal.h"
#include "dbg.h"
#include "mbox_host_reg.h"

#if defined(CONFIG_QCA402X_DEBUG)
#define DEBUG
#endif


#define MAX_HIF_DEVICES 2

#ifndef USER_SPACE
unsigned int hif_mmcbuswidth = 0;

module_param(hif_mmcbuswidth, uint, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(hif_mmcbuswidth, "Set MMC driver Bus Width: 1-1Bit, 4-4Bit, 8-8Bit");
EXPORT_SYMBOL(hif_mmcbuswidth);

unsigned int hif_mmcclock = 400000; /* TBD */

module_param(hif_mmcclock, uint, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(hif_mmcclock, "Set MMC driver Clock value");
EXPORT_SYMBOL(hif_mmcclock);


unsigned int hif_writecccr1 = 0;

module_param(hif_writecccr1, uint, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

unsigned int hif_writecccr1value = 0;

module_param(hif_writecccr1value, uint, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

unsigned int hif_writecccr2 = 0;

module_param(hif_writecccr2, uint, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

unsigned int hif_writecccr2value = 0;

module_param(hif_writecccr2value, uint, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);


unsigned int hif_writecccr3 = 0;

module_param(hif_writecccr3, uint, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

unsigned int hif_writecccr3value = 0;

module_param(hif_writecccr3value, uint, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);


unsigned int hif_writecccr4 = 0;

module_param(hif_writecccr4, uint, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);


unsigned int hif_writecccr4value = 0;

module_param(hif_writecccr4value, uint, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	
#endif

static void delHifDevice(HIF_DEVICE * device);

static void hif_stop_HIF_task(HIF_DEVICE * device);
static BUS_REQUEST *hifAllocateBusRequest(HIF_DEVICE * device);
static void hifFreeBusRequest(HIF_DEVICE * device, BUS_REQUEST * busrequest);
static void hifAddToReqList(HIF_DEVICE * device, BUS_REQUEST * busrequest);
static HIF_STATUS spiEnableFunc(HIF_DEVICE * device, struct sdio_func *func);
static int spiDeviceInserted(struct sdio_func *func, const struct sdio_device_id *id);

int hif_reset_sdio_on_unload = 0;

#ifndef USER_SPACE
module_param(hif_reset_sdio_on_unload, int, 0644);	


uint32_t hif_forcedriverstrength = 1;	/* force driver strength to type D */

/* ------ Static Variables ------ */
static const struct sdio_device_id hif_sdio_id_table[] = {
    {SDIO_DEVICE(SDIO_ANY_ID, SDIO_ANY_ID)},    /* WAR: QCA402x IDs are hardwired to 0 */
    { /* null */ },
};

MODULE_DEVICE_TABLE(sdio, hif_sdio_id_table);

static struct sdio_driver hif_sdio_driver = {
    .name = "hif_sdio",
    .id_table = hif_sdio_id_table,
    .probe = hifDeviceInserted,
    .remove = hifDeviceRemoved,
};

/* make sure we unregister only when registered. */
/* TBD: synchronization needed.... */
/* device->completion_task, registered, ... */
#endif

static int registered = 0;

struct cbs_from_os HIFcallbacks;


static HIF_DEVICE *hif_devices[MAX_HIF_DEVICES];


static void ResetAllCards(void);

#ifndef USER_SPACE
static HIF_STATUS hifDisableFunc(HIF_DEVICE * device, struct sdio_func *func);
static HIF_STATUS hifEnableFunc(HIF_DEVICE * device, struct sdio_func *func);
#endif

static int hif_sdio_register_driver(struct cbs_from_os *callbacks)
{
    /* store the callback handlers */
    HIFcallbacks = *callbacks;  /* structure copy */

    /* Register with bus driver core */
    registered++;

#ifndef USER_SPACE
    return sdio_register_driver(&hif_sdio_driver);
#else
	return 0;
#endif	
}

static void hif_sdio_unregister_driver(void)
{
#ifndef USER_SPACE
    sdio_unregister_driver(&hif_sdio_driver);
#endif
    registered--;
}

/* ------ Functions ------ */
HIF_STATUS HIFInit(void *pInfo, struct cbs_from_os *callbacks)
{
    int status;
	HIF_SPI_INTERFACE_CONFIG *pConfig = (HIF_SPI_INTERFACE_CONFIG *)pInfo;

    if (callbacks == NULL)
        return HIF_ERROR;

    status = hif_sdio_register_driver(callbacks);
    if (status != 0) {
        return HIF_ERROR;
    }

	HTCPrintf(HTC_PRINT_GRP0_HIF, "HIFInit\n");
	
	status = custom_spi_init(pConfig);
    if (status != 0) {
		printf("spi init fails\n");
        return HIF_ERROR;
    }

	status = spiDeviceInserted(NULL, NULL);
	if (status != 0)
	{
		printf("spi dev insert fails\n");
		return HIF_ERROR;
	}
	
	hif_spi_int_setup();

    return HIF_OK;
}

/*
 * Function: __HIFReadWrite
 * Description:    this routine is called by HIF_task. 
 * Input: 
 *        device:     HIF context
 *        address:    SPI read/write address, Register or FIFO
 *        buffer:     read/write data buffer 
 *        length:     read/write data buffer length in bytes
 *        request:    operating type
 *        context:    complete callback parameter
 * Returns:
 *        HIF_ERROR:   fails
 *        HIF_EINVAL:  parameters invalid
 *        HIF_OK:      successful
 *
 */
 
static HIF_STATUS
__HIFReadWrite(HIF_DEVICE * device,
               uint32_t address, uint8_t * buffer, uint32_t length, uint32_t request, void *context)
{
    uint8_t opcode;
    HIF_STATUS status = HIF_OK;
    int ret = 0;
    uint16_t    val16;

#ifndef USER_SPACE
    if (!device || !device->func) {
        return HIF_ERROR;
    }
#endif

    if (!buffer) {
		HTCPrintf(HTC_PRINT_GRP0_HIF, "__HIFReadWrite buffer is NULL\n");
        return HIF_ERROR;
    }

    if (length == 0) {
		HTCPrintf(HTC_PRINT_GRP0_HIF,  "__HIFReadWrite length is 0\n");
        return HIF_ERROR;
    }

    do {
        if (request & HIF_INTERFACE_ADDRESS) 
		{
            opcode = CMD_INTERFACE_ADDRESS;
        } 
		else if (request & HIF_HOST_ADDRESS) 
		{
            opcode = CMD_HOST_ADDRESS;
        } 
		else if (request & HIF_FIFO_ADDRESS) 
		{
            opcode = CMD_FIFO_ADDRESS;
        } 
		else 
		{
            status = HIF_EINVAL;
			HTCPrintf(HTC_PRINT_GRP0_HIF, "__HIFReadWrite request=0x%x wrong\n", request);			
            break;
        }

        if (request & HIF_WRITE) 
		{
			switch (opcode)
			{
			case	CMD_HOST_ADDRESS:     // write slave host register
				HTCPrintf(HTC_PRINT_GRP0_HIF, "__HIFReadWrite request host write addr:%x len=%d\n", address, length);
				{
					int  i;
					
					HTCPrintf(HTC_PRINT_GRP0_HIF, "val = ");
					for(i=0; i < length; i++)
					{
						HTCPrintf(HTC_PRINT_GRP0_HIF, "0x%02x ", *(buffer+i));					
					}
					HTCPrintf(HTC_PRINT_GRP0_HIF, "\n");
				}

				ret = ccs_write_host_register(address, length, buffer);
				break;
			case    CMD_INTERFACE_ADDRESS:
				val16 = *((uint16_t *)buffer);
				HTCPrintf(HTC_PRINT_GRP0_HIF, "__HIFReadWrite interface write regaddr=0x%0x value=0x%x\n", 
                                                    address, val16);
				val16 = CPU2SPI16(val16);
				ret = ccs_spiWrite(address, (uint8_t *)&val16, 2);
				break;
			case    CMD_FIFO_ADDRESS:
				HTCPrintf(HTC_PRINT_GRP0_HIF, "__HIFReadWrite request interface write reg addr=0x%0x value=0x%x\n",
                                                SPI_REG_DMA_SIZE, length);
				ret = ccs_spiWriteRegisterCmd(SPI_REG_DMA_SIZE, length);
				HTCPrintf(HTC_PRINT_GRP0_HIF, "__HIFReadWrite request fifo write  addr=0x%0x leng=%d\n", 
                                                address, length);			
				ret = ccs_spiWriteFIFO(address, buffer, length);
				break;		
			}
        } 
		else if (request & HIF_READ) 
		{
			switch (opcode)
			{
			case	CMD_HOST_ADDRESS:
				HTCPrintf(HTC_PRINT_GRP0_HIF, "__HIFReadWrite host read addr=0x%0x\n", address);
				ret = ccs_read_host_register(address, length, buffer);
				break;
			case    CMD_INTERFACE_ADDRESS:
				ccs_spiReadRegisterCmd(address, (uint16_t *)buffer);
				HTCPrintf(HTC_PRINT_GRP0_HIF, "__HIFReadWrite interface read [0x%x] ret=0x%04x\n", address, *((uint16_t *)buffer));
				
				break;
			case    CMD_FIFO_ADDRESS:
				HTCPrintf(HTC_PRINT_GRP0_HIF, "__HIFReadWrite interface read regaddr=0x%0x\n", SPI_REG_DMA_SIZE);
				ret = ccs_spiWriteRegisterCmd(SPI_REG_DMA_SIZE, length);
				HTCPrintf(HTC_PRINT_GRP0_HIF, "__HIFReadWrite fifo read  addr=0x%0x leng=%d buffer=%p\n", address, length, buffer);
				ret = ccs_spiReadFIFO(address, buffer, length);
				break;
			}			
        }		
		else 
		{
			HTCPrintf(HTC_PRINT_GRP0_HIF, "__HIFReadWrite request wrong operation\n");
            status = HIF_EINVAL;        /* Neither read nor write */
            break;
        }

        if (ret) {
            status = HIF_ERROR;
        }
    }
    while (false);

    return status;
}

/* Add busrequest to tail of sdio_request request list */
static void hifAddToReqList(HIF_DEVICE * device, BUS_REQUEST * busrequest)
{
#ifndef USER_SPACE
    unsigned long flags;
#endif

    busrequest->next = NULL;

#ifndef USER_SPACE
    spin_lock_irqsave(&device->reqQlock, flags);
#else
    sem_wait(&device->reqQlock);
#endif
    if (device->reqQhead) {
        device->reqQtail->next = busrequest;
    } else {
        device->reqQhead = busrequest;
    }
    device->reqQtail = busrequest;
#ifndef USER_SPACE
    spin_unlock_irqrestore(&device->reqQlock, flags);
#else
    sem_post(&device->reqQlock);
#endif
}

#ifndef USER_SPACE
HIF_STATUS
HIFSyncRead(HIF_DEVICE * device, uint32_t address, uint8_t * buffer, uint32_t length, uint32_t request, void *context)
{
    HIF_STATUS status;

    if (!device || !device->func) {
        return HIF_ERROR;
    }

    sdio_claim_host(device->func);
    status = __HIFReadWrite(device, address, buffer, length, request & ~HIF_SYNCHRONOUS, NULL);
    sdio_release_host(device->func);
    return status;
}
#endif

/*
 * Function: HIFReadWrite
 * Description:  this routine is SPI read/write API. 
 *               caller can select synchronous or asynchronous mode; 
 *               it queues a read/write request, if asynchronous mode, caller wait for it to complete.
 * Input: 
 *        device:     HIF context
 *        address:    SPI read/write address, Register or FIFO
 *        buffer:     read/write data buffer 
 *        length:     read/write data buffer length in bytes
 *        request:    operating type
 *        context:    complete callback parameter
 * Returns:
 *        HIF_ERROR:   fails
 *        HIF_EINVAL:  parameters invalid
 *        HIF_OK:      successful
 *
 */

HIF_STATUS
HIFReadWrite(HIF_DEVICE * device, uint32_t address, void *buffer, uint32_t length, uint32_t reqType, void *context)
{
    BUS_REQUEST *busrequest;
    HIF_STATUS status;

#ifndef USER_SPACE
    if (!device || !device->func) {
        return HIF_ERROR;
    }
#endif

    if (!(reqType & HIF_ASYNCHRONOUS) && !(reqType & HIF_SYNCHRONOUS)) {
        return HIF_EINVAL;
    }

#ifndef USER_SPACE
#else
	reqType = (reqType & ~HIF_ASYNCHRONOUS) | HIF_SYNCHRONOUS;
#endif

    /* Serialize all requests through the reqlist and HIFtask */
    busrequest = hifAllocateBusRequest(device);
    if (!busrequest) {
        return HIF_ERROR;
    }

    /*
     * TBD: caller may pass buffers ON THE STACK, especially 4 Byte buffers.
     * If this is a problem on some platforms/drivers, this is one reasonable
     * place to handle it. If poentially using DMA
     *    reject large buffers on stack
     *    copy 4B buffers allow register writes (no DMA)
     */

    busrequest->address = address;
    busrequest->buffer = buffer;
    busrequest->length = length;
    busrequest->reqType = reqType;
    busrequest->context = context;

    hifAddToReqList(device, busrequest);

    device->HIF_taskWork = 1;
#ifndef USER_SPACE
    wake_up(&device->HIF_wait); /* Notify HIF task */
#else
    htc_signal_set(device->HIF_wait, EVENT_TASK_WAKEUP);
#endif

    if (reqType & HIF_ASYNCHRONOUS) {
        return HIF_PENDING;
    }

    /* Synchronous request -- wait for completion.  */
#ifndef USER_SPACE
    down(&busrequest->sem_req);
#else
    sem_wait(&busrequest->sem_req);
#endif
    status = busrequest->status;
    hifFreeBusRequest(device, busrequest);
    return status;
}

/**
 * add_to_completion_list() - Queue a completed request
 * @device:    context to the hif device.
 * @comple: SDIO bus access request.
 *
 * This function adds an sdio bus access request to the
 * completion list.
 *
 * Return: No return.
 */
static void add_to_completion_list(HIF_DEVICE * device, BUS_REQUEST * comple)
{
#ifndef USER_SPACE
    unsigned long flags;
#endif

    comple->next = NULL;

#ifndef USER_SPACE
    spin_lock_irqsave(&device->complQlock, flags);
#else
	sem_wait(&device->complQlock);
#endif

    if (device->complQhead) {
        device->complQtail->next = comple;
    } else {
        device->complQhead = comple;
    }
    device->complQtail = comple;
	
#ifndef USER_SPACE
    spin_unlock_irqrestore(&device->complQlock, flags);
#else
	sem_post(&device->complQlock);
#endif
}

/**
 * process_completion_list() - Remove completed requests from
 * the completion list, and invoke the corresponding callbacks.
 *
 * @device:  HIF device handle.
 *
 * Function to clean the completion list.
 *
 * Return: No
 */
static void process_completion_list(HIF_DEVICE * device)
{
    BUS_REQUEST *next_comple;
    BUS_REQUEST *request;
#ifndef USER_SPACE
    unsigned long flags;
#endif

    /* Pull the entire chain of completions from the list */
#ifndef USER_SPACE
    spin_lock_irqsave(&device->complQlock, flags);
#else
	sem_wait(&device->complQlock);
#endif

    request = device->complQhead;
    device->complQhead = NULL;
    device->complQtail = NULL;
	
#ifndef USER_SPACE
    spin_unlock_irqrestore(&device->complQlock, flags);
#else
	sem_post(&device->complQlock);
#endif

    while (request != NULL) {
        HIF_STATUS status;
        void *context;

        next_comple = request->next;

        status = request->status;
        context = request->context;
		
        hifFreeBusRequest(device, request);
        device->cbs_from_HIF.rwCompletionHandler(context, status);
		
		HTCPrintf(HTC_PRINT_GRP0_HIF,  "process_completion_list done\n");

        request = next_comple;
    }
}

/**
 * completion_task() - Thread to process request completions
 *
 * @param:   context to the hif device.
 *
 * Completed asynchronous requests are added to a completion
 * queue where they are processed by this task. This serves
 * multiple purposes:
 *   -minimizes processing by the HIFTask, which allows
 *      that task to keep SDIO busy
 *   -allows request processing to be parallelized on
 *      multiprocessor systems
 *   -provides a suspendable context for use by the
 *      caller's callback function, though this should
 *      not be abused since it will cause requests to
 *      sit on the completion queue (which makes us
 *      more likely to exhaust free requests).
 *
 * Return: 0 thread exits
 */
#ifndef USER_SPACE
static int completion_task(void *param)
#else
static void *completion_task(void *param)
#endif
{
    HIF_DEVICE *device;

    device = (HIF_DEVICE *) param;
#ifndef USER_SPACE
    set_current_state(TASK_INTERRUPTIBLE);
#endif

    for (;;) {
#ifndef USER_SPACE
        wait_event_interruptible(device->completion_wait, device->completion_work);
#else
		htc_signal_wait(device->completion_wait, EVENT_TASK_WAKEUP, HTC_EVENT_WAIT_ANY_CLEAR_ALL);
#endif
		HTCPrintf(HTC_PRINT_GRP0_HIF,  "completion_task #1\n");
		
		if (!device->completion_work) {
            break;
        }

        if (device->completion_shutdown) {
            break;
        }

        process_completion_list(device);
    }

    /*
     * Process any remaining completions.
     * This task should not be shut down
     * until after all requests are stopped.
     */
    process_completion_list(device);

#ifndef USER_SPACE
    complete_and_exit(&device->completion_exit, 0);
#endif
    return 0;
}


/**
 * hif_request_complete() - Completion processing after a request
 * is processed.
 *
 * @device:    device handle.
 * @request:   SIDO bus access request.
 *
 * All completed requests are queued onto a completion list
 * which is processed by complete_task.
 *
 * Return: None.
 */
static inline void hif_request_complete(HIF_DEVICE * device, BUS_REQUEST * request)
{
    add_to_completion_list(device, request);
    device->completion_work = 1;
#ifndef USER_SPACE
    wake_up(&device->completion_wait);
#else
	htc_signal_set(device->completion_wait, EVENT_TASK_WAKEUP);
#endif
}

#ifndef USER_SPACE
/*
 * hif_stop_completion_thread() - Destroy the completion task
 * @device: device handle.
 *
 * This function will destroy the completion thread.
 *
 * Return: None.
 */
static inline void hif_stop_completion_thread(HIF_DEVICE * device)
{
    if (device->completion_task) {
        init_completion(&device->completion_exit);
        device->completion_shutdown = 1;

        device->completion_work = 1;
        wake_up(&device->completion_wait);
        wait_for_completion(&device->completion_exit);
        device->completion_task = NULL;
    }
}
#endif

/*
 * This task tries to keep the SPI bus as busy as it
 * can. It pulls both requests off the request queue and
 * it uses the kernel device file API to make them happen.
 *
 * Requests may be one of
 *   synchronous (a thread is suspended until it completes)
 *   asynchronous (a completion callback will be invoked)
 * and one of
 *   reads (from Target SDIO space into Host RAM)
 *   writes (from Host RAM into Target SDIO space)
 * and it is to one of
 *   Target's mailbox space
 *   Target's register space
 * and lots of other choices.
 */
 
#ifndef USER_SPACE
static int HIF_task(void *param)
#else
static void *HIF_task(void *param)
#endif
{
    HIF_DEVICE *device;
    BUS_REQUEST *request;
    HIF_STATUS status;
#ifndef USER_SPACE
    unsigned long flags;

    set_user_nice(current, -3);
    device = (HIF_DEVICE *) param;
    set_current_state(TASK_INTERRUPTIBLE);
#else

// struct sched_param is used to store the scheduling priority
    struct sched_param params;
    int    policy;
    pthread_t this_thread = pthread_self();
    
    pthread_getschedparam(this_thread, &policy, &params);

// We'll set the priority to the maximum.
    params.sched_priority = sched_get_priority_max(SCHED_FIFO);
// Attempt to set thread real-time priority to the SCHED_FIFO policy
    pthread_setschedparam(this_thread, SCHED_FIFO, &params);    

    device = (HIF_DEVICE *) param;
#endif

    for (;;) {
#ifndef USER_SPACE
        /* wait for work */
        wait_event_interruptible(device->HIF_wait, device->HIF_taskWork);
#else
		htc_signal_wait(device->HIF_wait, EVENT_TASK_WAKEUP, HTC_EVENT_WAIT_ANY_CLEAR_ALL);
#endif
        if (!device->HIF_taskWork) {
            /* interrupted, exit */
            printf("HIF_task: exit #1\n");          
            break;
        }

        if (device->HIF_shutdown) {
            printf("HIF_task: exit #2\n");          
            break;
        }

        /*
         * We want to hold the host over multiple cmds if possible;
         * but holding the host blocks card interrupts.
         */
        sdio_claim_host(device->func);

        for (;;) {
            /* Pull the next request to work on */
#ifndef USER_SPACE
            spin_lock_irqsave(&device->reqQlock, flags);
#else
			sem_wait(&device->reqQlock);
#endif
			request = device->reqQhead;
			
            if (!request) {
#ifndef USER_SPACE
                spin_unlock_irqrestore(&device->reqQlock, flags);
#else
				sem_post(&device->reqQlock);
#endif
				break;
            }

            /* Remove request from queue */
            device->reqQhead = request->next;
            /* Note: No need to clean up reqQtail */

#ifndef USER_SPACE
            spin_unlock_irqrestore(&device->reqQlock, flags);
#else
			sem_post(&device->reqQlock);
#endif
			
            status =
                __HIFReadWrite(device, request->address,
                               request->buffer, request->length, request->reqType & ~HIF_SYNCHRONOUS, NULL);
			
            /* When we return, the read/write is done */
            request->status = status;

            if (request->reqType & HIF_ASYNCHRONOUS) 
			{
                hif_request_complete(device, request);
            } 
			else 
			{
                /* notify thread that's waiting on this request */
#ifndef USER_SPACE
                up(&request->sem_req);
#else
				sem_post(&request->sem_req);			
#endif
			}
        }
        sdio_release_host(device->func);
    }

#ifndef USER_SPACE
    complete_and_exit(&device->HIF_exit, 0);
#endif
    return 0;
}

HIF_STATUS HIFConfigureDevice(HIF_DEVICE * device, HIF_DEVICE_CONFIG_OPCODE opcode, void *config, uint32_t configLen)
{
    HIF_STATUS status = HIF_OK;

    switch (opcode) {

        /* TBD: Does this belong here?  */
    case HIF_DEVICE_GET_MBOX_BLOCK_SIZE:
        ((uint32_t *) config)[0] = HIF_MBOX0_BLOCK_SIZE;
        ((uint32_t *) config)[1] = HIF_MBOX1_BLOCK_SIZE;
        ((uint32_t *) config)[2] = HIF_MBOX2_BLOCK_SIZE;
        ((uint32_t *) config)[3] = HIF_MBOX3_BLOCK_SIZE;
        break;

    case HIF_DEVICE_SET_CONTEXT:
        device->context = config;
        break;

    case HIF_DEVICE_GET_CONTEXT:
        if (config == NULL) {
            return HIF_ERROR;
        }
        *(void **)config = device->context;
        break;

    default:
        status = HIF_ERROR;
    }

    return status;
}

void HIFShutDownDevice(HIF_DEVICE * device)
{
    if (device == NULL) {
        int i;
        /*
         * since we are unloading the driver, reset all cards
         * in case the SDIO card is externally powered and we
         * are unloading the SDIO stack.  This avoids the problem
         * when the SDIO stack is reloaded and attempts are made
         * to re-enumerate a card that is already enumerated.
         */
        ResetAllCards();

        /* Unregister with bus driver core */
        if (registered) {
            registered = 0;
            hif_sdio_unregister_driver();
            BUG_ON(1);
        }

        for (i = 0; i < MAX_HIF_DEVICES; ++i) {
            if (hif_devices[i] && hif_devices[i]->func == NULL) {
                delHifDevice(hif_devices[i]);
                hif_devices[i] = NULL;
            }
        }
    }
}

#ifndef USER_SPACE
#else
uint16_t hifDisableIRQ(HIF_DEVICE *device)
{
    return device->cbs_from_HIF.disableIRQ(device->cbs_from_HIF.context);
}

int hifEnableIRQ(HIF_DEVICE *device)
{
   return device->cbs_from_HIF.enableIRQ(device->cbs_from_HIF.context);
}

bool HIFIsPacketAvailInt(void *hif_handle)
{
    HIF_DEVICE * device = (HIF_DEVICE *)hif_handle;

    if (device->spi_int_cause & SPI_INTR_CAUSE_PKT_AVAIL)
        return true;
    return false;
}

bool HIFIsCreditInt(void *hif_handle)
{
    HIF_DEVICE * device = (HIF_DEVICE *)hif_handle;
    
    HTCPrintf(HTC_PRINT_GRP0_HIF, "HIFIsCreditInt cause:%0x enable:%02x\n", device->spi_int_cause, device->hif_int_enable);
        
    if ((device->spi_int_cause & SPI_INTR_CAUSE_COUNTER_INTR) && (device->hif_int_enable & INTR_ENABLE_CREDIT_COUNT))
        return true;
    return false;
}

#endif

void hifIRQHandler()
{
    HIF_DEVICE *device;
    int more_interrupts;


    device = htc_spi_ctxt.device;

    device->irqHandling = 1;
    /* release the host during ints so we can pick it back up when we process cmds */
    do 
    {
        device->spi_int_cause = hifDisableIRQ(device);
/*
 * have a check if we have interested interrupt happen.
 * if yes, we need to process it
 */
        if (device->spi_int_cause & (SPI_INTR_CAUSE_PKT_AVAIL | SPI_INTR_CAUSE_COUNTER_INTR))
            device->cbs_from_HIF.dsrHandler(device->cbs_from_HIF.context);
/* 
 * after Interrupt return, it is possible that we have another interrupt happen.
 * so here, we need to have a check
 */
        more_interrupts = hifEnableIRQ(device);
    } while (more_interrupts);

    device->irqHandling = 0;
}

static int spiDeviceInserted(struct sdio_func *func, const struct sdio_device_id *id)
{
    int i;
    int ret;
    HIF_DEVICE *device = NULL;
    int count;

    /* dma_mask should be populated here. Use the parent device's setting. */
#ifndef USER_SPACE
    func->dev.dma_mask = mmc_dev(func->card->host)->dma_mask;

    if (addHifDevice(func) == NULL) {
        return -1;
    }
    device = getHifDevice(func);
#else
    device = (HIF_DEVICE *) malloc(sizeof(HIF_DEVICE));
    if (device == NULL)
        return -1;
    memset(device, 0, sizeof(*device));
    device->func = func;
    htc_spi_ctxt.device = device;

#endif

    HTCPrintf(HTC_PRINT_GRP0_HIF, "spiDeviceInserted...\n");

    for (i = 0; i < MAX_HIF_DEVICES; ++i) {
        if (hif_devices[i] == NULL) {
            hif_devices[i] = device;
            break;
        }
    }
    BUG_ON(i >= MAX_HIF_DEVICES);

    device->id = id;
#ifndef USER_SPACE
    device->host = func->card->host;
#endif
    device->is_enabled = false;

#ifndef USER_SPACE
    spin_lock_init(&device->reqFreeQlock);
    spin_lock_init(&device->reqQlock);
#else
    sem_init(&device->reqFreeQlock, 0, 1);
    sem_init(&device->reqQlock, 0, 1);
    sem_init(&device->irqReceiveInProgresslock, 0, 1);
    
    pthread_mutex_init(&device->int_flag_lock, NULL);
	pthread_cond_init(&device->int_flag_cond, NULL);

#endif

	HTCPrintf(HTC_PRINT_GRP0_HIF,  "spiDeviceInserted busRequest=%p...\n", device->busRequest);
    /* Initialize the bus requests to be used later */
    memset(device->busRequest, 0, sizeof(device->busRequest));
    for (count = 0; count < BUS_REQUEST_MAX_NUM; count++) {
#ifndef USER_SPACE
        sema_init(&device->busRequest[count].sem_req, 0);
#else
        sem_init(&device->busRequest[count].sem_req, 0, 0);	
#endif
        hifFreeBusRequest(device, &device->busRequest[count]);
    }
#ifndef USER_SPACE
    init_waitqueue_head(&device->HIF_wait);
    spin_lock_init(&device->complQlock);
    init_waitqueue_head(&device->completion_wait);
#else
    device->HIF_wait = htc_signal_init();
    sem_init(&device->complQlock, 0, 1);
    device->completion_wait = htc_signal_init();
#endif

    ret = spiEnableFunc(device, func);
    if ((ret == HIF_OK) || (ret == HIF_PENDING)) 
    {
        return 0;
    } 
    else 
    {
        for (i = 0; i < MAX_HIF_DEVICES; i++) {
            if (hif_devices[i] == device) {
                hif_devices[i] = NULL;
                break;
            }
        }
#ifndef USER_SPACE
        sdio_set_drvdata(func, NULL);
#endif
        delHifDevice(device);
        return -1;
    }
}


void HIFAckInterrupt(HIF_DEVICE * device)
{
    /* Acknowledge our function IRQ -- nothing needed */
}

void HIFUnMaskInterrupt(HIF_DEVICE * device)
{
#ifndef USER_SPACE
    int ret;;

    if (!device || !device->func) {
        return;
    }

    /* Register the IRQ Handler */
    sdio_claim_host(device->func);
    ret = sdio_claim_irq(device->func, hifIRQHandler);
    sdio_release_host(device->func);
#else
	uint32_t address = SPI_REG_INTR_ENABLE;
	uint16_t val = SPI_INTR_ENABLE;
	uint32_t request = HIF_WR_SYNC_INTERFACE;
	
    HIFReadWrite(device, address, (uint8_t *)&val, sizeof(val), request, NULL);
#endif
}

void HIFEnableInterrupt(HIF_DEVICE * device, uint16_t enble_bits)
{
	HTCPrintf(HTC_PRINT_GRP0_HIF,  "HIFEnableInterrupt...device=%p enable:%0x  bits:%0x\n", 
                                                    device, device->hif_int_enable, enble_bits);
	device->hif_int_enable |= enble_bits;
    
    HIFReadWrite(device, SPI_REG_INTR_ENABLE, (uint8_t *)&device->hif_int_enable, 
                                sizeof(device->hif_int_enable), HIF_WR_SYNC_INTERFACE, NULL);
}

void HIFDisableInterrupt(HIF_DEVICE * device, uint16_t enble_bits)
{
	HTCPrintf(HTC_PRINT_GRP0_HIF,  "HIFDisableInterrupt...device=%p enable:%0x  bits:%0x\n", 
                                                    device, device->hif_int_enable, enble_bits);
	device->hif_int_enable &= ~enble_bits;
    
    HIFReadWrite(device, SPI_REG_INTR_ENABLE, (uint8_t *)&device->hif_int_enable, 
                                sizeof(device->hif_int_enable), HIF_WR_SYNC_INTERFACE, NULL);
}

/* Is there something that stops us from calling sdio_release_irq
 * while we are in hifIRQHandler?
 */
void HIFMaskInterrupt(HIF_DEVICE * device)
{
#ifndef USER_SPACE
    int ret;
	
    if (!device || !device->func) {
        return;
    }

    /* Mask our function IRQ */
    sdio_claim_host(device->func);
    ret = sdio_release_irq(device->func);
    BUG_ON(ret != 0);
    sdio_release_host(device->func);
#endif
}

static BUS_REQUEST *hifAllocateBusRequest(HIF_DEVICE * device)
{
    BUS_REQUEST *busrequest;
#ifndef USER_SPACE
    unsigned long flag;

    spin_lock_irqsave(&device->reqFreeQlock, flag);
#else
    sem_wait(&device->reqFreeQlock);	
#endif

    /* Remove first in list */
    busrequest = device->busReqFreeQhead;
    if (busrequest) {
        device->busReqFreeQhead = busrequest->next;
    }
	
#ifndef USER_SPACE
    spin_unlock_irqrestore(&device->reqFreeQlock, flag);
#else
    sem_post(&device->reqFreeQlock);
#endif

    return busrequest;
}

static void hifFreeBusRequest(HIF_DEVICE * device, BUS_REQUEST * busrequest)
{
#ifndef USER_SPACE
    unsigned long flag;
#endif

    if (busrequest == NULL) {
        return;
    }

    busrequest->next = NULL;

    /* Insert first in list */
#ifndef USER_SPACE
    spin_lock_irqsave(&device->reqFreeQlock, flag);
#else
	sem_wait(&device->reqFreeQlock);
#endif

    busrequest->next = device->busReqFreeQhead;
    device->busReqFreeQhead = busrequest;
	
#ifndef USER_SPACE
    spin_unlock_irqrestore(&device->reqFreeQlock, flag);
#else
    sem_post(&device->reqFreeQlock);
#endif	
}

static HIF_STATUS hifDisableFunc(HIF_DEVICE * device, struct sdio_func *func)
{
    HIF_STATUS status = HIF_OK;

    return status;
}

#ifndef USER_SPACE
static HIF_STATUS hifEnableFunc(HIF_DEVICE * device, struct sdio_func *func)
{
    int ret = HIF_OK;

    device = getHifDevice(func);

    if (!device) {
        return HIF_EINVAL;
    }

    if (!device->is_enabled) {
        /* enable the SDIO function */
        sdio_claim_host(func);

        /* give us some time to enable, in ms */
        func->enable_timeout = 100;
        ret = sdio_enable_func(func);
        if (ret) {
            sdio_release_host(func);
            return HIF_ERROR;
        }
        ret = sdio_set_block_size(func, HIF_MBOX_BLOCK_SIZE);

        sdio_release_host(func);
        if (ret) {
            return HIF_ERROR;
        }
        device->is_enabled = true;

        if (!device->completion_task) {
            device->complQhead = NULL;
            device->complQtail = NULL;
            device->completion_shutdown = 0;
            device->completion_task = kthread_create(completion_task, (void *)device, "HIFCompl");
            if (IS_ERR(device->completion_task)) {
                device->completion_shutdown = 1;
                return HIF_ERROR;
            }
            wake_up_process(device->completion_task);
        }

        /* create HIF I/O thread */
        if (!device->HIF_task) {
            device->HIF_shutdown = 0;
            device->HIF_task = kthread_create(HIF_task, (void *)device, "HIF");
            if (IS_ERR(device->HIF_task)) {
                device->HIF_shutdown = 1;
                return HIF_ERROR;
            }
            wake_up_process(device->HIF_task);
        }
    }

    if (!device->claimedContext) {
        ret = HIFcallbacks.deviceInsertedHandler(HIFcallbacks.context, device);
        if (ret != HIF_OK) {
            /*
             * Disable the SDIO func & Reset the sdio
             * for automated tests to move ahead, where
             * the card does not need to be removed at
             * the end of the test.
             */
            hifDisableFunc(device, func);
        }
    }

    return ret;
}
#else

static HIF_STATUS spiEnableFunc(HIF_DEVICE * device, struct sdio_func *func)
{
    int ret = HIF_OK;
	pthread_attr_t attr;
	int   iRet;
	

	HTCPrintf(HTC_PRINT_GRP0_HIF,  "spiEnableFunc...device=%p\n", device);

    if (!device->is_enabled) {
        /* enable the SDIO function */
        sdio_claim_host(func);

        /* give us some time to enable, in ms */
		
        device->is_enabled = true;

        if (!device->completion_task) 
		{
            device->complQhead = NULL;
            device->complQtail = NULL;
            device->completion_shutdown = 0;

			pthread_attr_init(&attr);
			pthread_attr_setstacksize(&attr, SPI_THREAD_STACKSIZE);
            iRet = pthread_create(&device->completion_task, &attr, completion_task, device);
			pthread_attr_destroy(&attr);

			if (iRet != 0) 
			{
				return HIF_ERROR;             /* Failed to create task */
			}			
        }

        /* create HIF I/O thread */
        if (!device->HIF_task) 
		{
            device->HIF_shutdown = 0;
						
			HTCPrintf(HTC_PRINT_GRP0_HIF,  "create HIF_task...\n");
			
			pthread_attr_init(&attr);
			pthread_attr_setstacksize(&attr, SPI_THREAD_STACKSIZE);
			iRet = pthread_create(&device->HIF_task, &attr, HIF_task, (void *)device);
			pthread_attr_destroy(&attr);

			if (iRet != 0) 
			{
				HTCPrintf(HTC_PRINT_GRP0_HIF,  "create HIF_task fail\n");
				return HIF_ERROR;             /* Failed to create task */
			}
        }
    }

    ret = HIFcallbacks.deviceInsertedHandler(HIFcallbacks.context, device);
    if (ret != HIF_OK) 
	{
            /*
             * Disable the SDIO func & Reset the sdio
             * for automated tests to move ahead, where
             * the card does not need to be removed at
             * the end of the test.
             */
        hifDisableFunc(device, func);
    }

    return ret;
}
#endif

#ifndef USER_SPACE

static void hifDeviceRemoved(struct sdio_func *func)
{
    HIF_STATUS status = HIF_OK;
    HIF_DEVICE *device;

    device = getHifDevice(func);
    if (!device) {
        return;
    }

    {
        int i;
        for (i = 0; i < MAX_HIF_DEVICES; ++i) {
            if (hif_devices[i] == device) {
                hif_devices[i] = NULL;
            }
        }
    }

    if (device->claimedContext != NULL) {
        status = HIFcallbacks.deviceRemovedHandler(device->claimedContext, device);
    }

    HIFMaskInterrupt(device);

    if (device->is_enabled) {
        status = hifDisableFunc(device, func);
    }

    delHifDevice(device);
}
#endif

static void delHifDevice(HIF_DEVICE * device)
{
    if (!device) {
        return;
    }

#ifndef USER_SPACE
    kfree(device);
#else
    sem_destroy(&device->reqFreeQlock);
    sem_destroy(&device->reqQlock);
    sem_destroy(&device->irqReceiveInProgresslock);

    pthread_mutex_destroy(&device->int_flag_lock);
    pthread_cond_destroy(&device->int_flag_cond);

    free(htc_spi_ctxt.device);
    htc_spi_ctxt.device = NULL;
#endif
}

static void ResetAllCards(void)
{
}

HIF_STATUS HIFAttach(HIF_DEVICE * device, struct cbs_from_HIF *callbacks)
{
    if (device->cbs_from_HIF.context != NULL) {
        /* already in use! */
        return HIF_ERROR;
    }
    device->cbs_from_HIF = *callbacks;
    return HIF_OK;
}


static void hif_stop_HIF_task(HIF_DEVICE * device)
{
    if (device->HIF_task) 
	{
#ifndef USER_SPACE
        init_completion(&device->HIF_exit);
#endif
		device->HIF_shutdown = 1;
        device->HIF_taskWork = 1;
#ifndef USER_SPACE
        wake_up(&device->HIF_wait);
        wait_for_completion(&device->HIF_exit);
		device->HIF_task = NULL;
#else
		device->HIF_task = 0;
#endif
    }
}

/**
 * hif_reset_target() - Reset target device
 * @hif_device: pointer to hif_device structure
 *
 * Reset the target by invoking power off and power on
 * sequence to bring back target into active state.
 * This API shall be called only when driver load/unload
 * is in progress.
 *
 * Return: 0 on success, error for failure case.
 */
static int hif_reset_target(HIF_DEVICE * hif_device)
{
    int ret;

#ifndef USER_SPACE
    if (!hif_device || !hif_device->func || !hif_device->func->card) {
        return -ENODEV;
    }
    /* Disable sdio func->pull down WLAN_EN-->pull down DAT_2 line */
    ret = mmc_power_save_host(hif_device->func->card->host);
    if (ret) {
        goto done;
    }
#else
	ret = 0;
#endif

    /* pull up DAT_2 line->pull up WLAN_EN-->Enable sdio func */
#ifndef USER_SPACE
    ret = mmc_power_restore_host(hif_device->func->card->host);
done:
#endif
    return ret;
}

void HIFDetach(HIF_DEVICE * device)
{
    hif_stop_HIF_task(device);
    if (device->ctrl_response_timeout) {
        /* Reset the target by invoking power off and power on sequence to
         * the card to bring back into active state.
         */
        if (hif_reset_target(device)) {
#ifndef USER_SPACE
            panic("BUG");
#endif
		}
        device->ctrl_response_timeout = false;
    }
	
	hif_spi_deint_setup();
    custom_spi_deinit();

    memset(&device->cbs_from_HIF, 0, sizeof(device->cbs_from_HIF));
}

void HIFSetHandle(void *hif_handle, void *handle)
{
    HIF_DEVICE *device = (HIF_DEVICE *) hif_handle;

    device->caller_handle = handle;

    return;
}

HIF_STATUS HIFPacketAvailSet(void *hif_handle)
{
    HIF_DEVICE *device = (HIF_DEVICE *)hif_handle;
    
    sem_wait(&device->irqReceiveInProgresslock);
    device->irqReceiveInProgress = 1;
    sem_post(&device->irqReceiveInProgresslock);	

    return HIF_OK;
}

HIF_STATUS HIFPacketAvailReset(void *hif_handle)
{
    HIF_DEVICE *device = (HIF_DEVICE *)hif_handle;
    
    sem_wait(&device->irqReceiveInProgresslock);
    device->irqReceiveInProgress = 0;
    sem_post(&device->irqReceiveInProgresslock);	
    return HIF_OK;
}

HIF_STATUS HIFSetWorkTaskProcessInt(void *hif_handle)
{
    HIF_DEVICE *device = (HIF_DEVICE *)hif_handle;
    device->worktask_process_int = 1;
	HTCPrintf(HTC_PRINT_GRP0_HIF,  "HIFSetWorkTaskProcessInt\n");
    return HIF_OK;    
}

HIF_STATUS HIFResetWorkTaskProcessInt(void *hif_handle)
{
    HIF_DEVICE *device = (HIF_DEVICE *)hif_handle;
    device->worktask_process_int = 0;
    return HIF_OK;    
}

uint16_t HIFInterruptFlagsCheck(void *hif_handle, uint16_t flags)
{
    HIF_DEVICE *device = (HIF_DEVICE *)hif_handle;
    return (device->spi_int_cause & flags);
}

HIF_STATUS HIFWorkTaskProcessIntCondSignal(void *hif_handle)
{
    HIF_DEVICE *device = (HIF_DEVICE *)hif_handle;
    
    device->worktask_process_int = 0;
    pthread_cond_signal(&device->int_flag_cond);    
    return HIF_OK;    
}

HIF_STATUS HIFWorkTaskProcessIntCondWait(void *hif_handle)
{
    HIF_DEVICE *device = (HIF_DEVICE *)hif_handle;

	while(device->worktask_process_int != 0)
	{
		pthread_cond_wait(&device->int_flag_cond, &device->int_flag_lock); //wait for the condition
	}				
    return HIF_OK;    
}

int HIFInWorkTaskProcessInt(void *hif_handle)
{
    HIF_DEVICE *device = (HIF_DEVICE *)hif_handle;
	HTCPrintf(HTC_PRINT_GRP0_HIF,  "HIFInWorkTaskProcessInt:%d\n", device->worktask_process_int);
    return device->worktask_process_int;
}

HIF_STATUS HIFResetTarget()
{
    setup_Qz_wakeup ();

    deassertQzWake();
    assertQzWake();
    sleep_ms(10);
    deassertQzWake(); 
    sleep_ms(10);
    return HIF_OK;    
}

HIF_STATUS HIFInterruptLock(void *hif_handle)
{
    HIF_DEVICE *device = (HIF_DEVICE *)hif_handle;
    pthread_mutex_lock(&device->int_flag_lock);    

    return HIF_OK;    
}

HIF_STATUS HIFSetIntInProcessVar(void *hif_handle)
{
    HIF_DEVICE *device = (HIF_DEVICE *)hif_handle;
    device->worktask_process_int = 1;

    return HIF_OK;    
}

HIF_STATUS HIFInterruptUnLock(void *hif_handle)
{
    HIF_DEVICE *device = (HIF_DEVICE *)hif_handle;
    pthread_mutex_unlock(&device->int_flag_lock);    
    
    return HIF_OK;    
}

/*
 * Function: HIFHasMoreInterrupt
 * Description:    check if there is more interrupt that need to process. 
 * Input: 
 *        hif_handle:     HIF context
 *        int_cause:      SPI INT_CAUSE content
 * Returns:
 *        0:   no more interrupt
 *        1:   more interrupt that need to process
 *
 */

int HIFHasMoreInterrupt(void *hif_handle, uint16_t  int_cause)
{
    uint8_t   more_credit_interrupt = 0;
    uint32_t  inProcess;

    HIF_DEVICE *device = (HIF_DEVICE *)hif_handle;

    inProcess = HIFPacketAvailInProcessing(device);
    HTCPrintf(HTC_PRINT_GRP0_HIF, "HIFMoreInterrupt  inProcess=%d\n", inProcess);
    
    do {
        if ((int_cause & SPI_INTR_CAUSE_PKT_AVAIL) && !inProcess)
        {
            more_credit_interrupt = 1;
            break;
        }
                
        if (((int_cause & SPI_INTR_CAUSE_COUNTER_INTR) != 0) && (device->hif_int_enable & INTR_ENABLE_CREDIT_COUNT))
        {
            more_credit_interrupt = 1;
            break;               
        }
        
    }  while (0);
    
    return more_credit_interrupt; 
}
