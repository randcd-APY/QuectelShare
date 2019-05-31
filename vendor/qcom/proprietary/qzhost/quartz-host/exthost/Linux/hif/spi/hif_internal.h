/*
 * Copyright (c) 2013-2014, 2016-2018 The Linux Foundation. All rights reserved.
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

#ifndef _HIF_INTERNAL_H_
#define _HIF_INTERNAL_H_

#include <semaphore.h>

#include "hif.h"
#include "hif_spi_common.h"
#include "htc_event_signal.h"
#include "spi_hw.h"

/*
 * Make this large enough to avoid ever failing due to lack of bus requests.
 * A number that accounts for the total number of credits on the Target plus
 * outstanding register requests is good.
 *
 * FUTURE: could dyanamically allocate busrequest structs as needed.
 * FUTURE: would be nice for HIF to use HTC's htc_request. Seems 
 *      wasteful to use multiple structures -- one for HTC and another
 *      for HIF -- and to copy info from one to the other. Maybe should
 *      semi-merge these layers?
 */
#define BUS_REQUEST_MAX_NUM                128

#define SDIO_CLOCK_FREQUENCY_DEFAULT       25000000
#define SDWLAN_ENABLE_DISABLE_TIMEOUT      20
#define FLAGS_CARD_ENAB                    0x02
#define FLAGS_CARD_IRQ_UNMSK               0x04

/*
 * The block size is an attribute of the SDIO function which is
 * shared by all four mailboxes. We cannot support per-mailbox
 * block sizes over SDIO.
 */

#ifndef USER_SPACE

#define HIF_MBOX_BLOCK_SIZE                HIF_DEFAULT_IO_BLOCK_SIZE
#define HIF_MBOX0_BLOCK_SIZE               HIF_MBOX_BLOCK_SIZE
#define HIF_MBOX1_BLOCK_SIZE               HIF_MBOX_BLOCK_SIZE
#define HIF_MBOX2_BLOCK_SIZE               HIF_MBOX_BLOCK_SIZE
#define HIF_MBOX3_BLOCK_SIZE               HIF_MBOX_BLOCK_SIZE

#else

#define HIF_MBOX_BUFF_SIZE                 HIF_DEFAULT_IO_BLOCK_SIZE

#define HIF_MBOX0_BLOCK_SIZE               HIF_MBOX_BUFF_SIZE
#define HIF_MBOX1_BLOCK_SIZE               HIF_MBOX_BUFF_SIZE
#define HIF_MBOX2_BLOCK_SIZE               HIF_MBOX_BUFF_SIZE
#define HIF_MBOX3_BLOCK_SIZE               HIF_MBOX_BUFF_SIZE

#endif

typedef struct bus_request {
    struct bus_request *next;   /* link list of available requests */
#ifndef USER_SPACE
    struct semaphore sem_req;
#else
    sem_t sem_req;
#endif
    uint32_t address;           /* request data */
    uint8_t *buffer;
    uint32_t length;
    uint32_t reqType;
    void *context;
    HIF_STATUS status;
} BUS_REQUEST;

struct hif_device {
    struct sdio_func *func;

    /* Main HIF task */	
#ifndef USER_SPACE
    struct task_struct *HIF_task;       /* task to handle SDIO requests */
    wait_queue_head_t HIF_wait;
#else
    pthread_t HIF_task;       /* task to handle SDIO requests */
    htc_event_signal_t *HIF_wait;
#endif
    int HIF_taskWork;           /* Signals HIFtask that there is work */
    int HIF_shutdown;           /* signals HIFtask to stop */
#ifndef USER_SPACE
    struct completion HIF_exit; /* HIFtask completion */
#else
#endif

    /* HIF Completion task */
#ifndef USER_SPACE
    struct task_struct *completion_task;        /* task to handle SDIO completions */
    wait_queue_head_t completion_wait;
#else
    pthread_t completion_task;        /* task to handle SDIO completions */
    htc_event_signal_t *completion_wait;
#endif
    int completion_work;
    int completion_shutdown;
#ifndef USER_SPACE
    struct completion completion_exit;
#else

#endif

    /* pending request queue */
#ifndef USER_SPACE
    spinlock_t reqQlock;
#else
    sem_t reqQlock;	
#endif
    BUS_REQUEST *reqQhead;      /* head of request queue */
    BUS_REQUEST *reqQtail;      /* tail of request queue */

    /* completed request queue */
#ifndef USER_SPACE
    spinlock_t complQlock;
#else
    sem_t complQlock;
#endif
    BUS_REQUEST *complQhead;
    BUS_REQUEST *complQtail;

    /* request free list */
#ifndef USER_SPACE
    spinlock_t reqFreeQlock;
#else
    sem_t reqFreeQlock;
#endif
    BUS_REQUEST *busReqFreeQhead;       /* free queue */

    /* Space for requests, initially queued to busRequestFreeQueue */
    BUS_REQUEST busRequest[BUS_REQUEST_MAX_NUM];

    void *claimedContext;
    struct cbs_from_HIF cbs_from_HIF;   /* Callbacks made from HIF to caller */
    bool is_enabled;            /* device is currently enabled? */
    int irqHandling;            /* currently processing interrupts */
    const struct sdio_device_id *id;
    struct mmc_host *host;
    void *context;
    bool ctrl_response_timeout;
    void *caller_handle;        /* for debug; links hif device back to caller (e.g. HTC target) */

#ifdef USER_SPACE
    sem_t   irqReceiveInProgresslock;
    int     irqReceiveInProgress;     /* currently processing packet available interrupts */
    uint16_t    spi_int_cause;          //

    pthread_cond_t  int_flag_cond;              //  conditional signal
    pthread_mutex_t int_flag_lock;              //  mutex for conditional signal
    volatile uint16_t   worktask_process_int;   //  variable that conditional signal protects, that control interrupts
    uint16_t   hif_int_enable;                  //  interrupt enable copy
#endif
};

#define HIFPacketAvailInProcessing(hif_handle) (((HIF_DEVICE *)(hif_handle))->irqReceiveInProgress != 0)

#define CMD53_FIXED_ADDRESS 1
#define CMD53_INCR_ADDRESS  2

#define CMD_HOST_ADDRESS        1
#define CMD_INTERFACE_ADDRESS   2
#define CMD_FIFO_ADDRESS        3

#endif                          /* _HIF_INTERNAL_H_ */
