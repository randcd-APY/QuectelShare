/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*
 * Copyright (c) 2004-2005 Atheros Communications Inc.
 * All rights reserved.
 */
#ifndef _HTC_INTERNAL_H_
#define _HTC_INTERNAL_H_

#ifdef USER_SPACE
#include <semaphore.h>
#include "htc_event_signal.h"

#else
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/spinlock_types.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/completion.h>
#endif

#ifdef USER_SPACE
#define BARRIER  
#else
#define BARRIER  barrier()
#endif 

#define lendebug(str) printk(str)

/* HTC internal specific declarations and prototypes */

/* Target-side SDIO/SPI (mbox) controller supplies 4 mailboxes */
#define HTC_NUM_MBOX                       4

/* Software supports at most this many Target devices */
#define HTC_NUM_DEVICES_MAX                2

/*
 * Maximum mailbox request message size.
 * Quartz' SDIO/SPI mailbox alias spaces are 2KB each; so changes
 * would be required to exceed that. WLAN restricts packets to
 * under 1500B anyway.
 */
#define HTC_MESSAGE_SIZE_MAX               ((3*512)- HTC_HEADER_LEN)

#define HTC_TARGET_RESPONSE_TIMEOUT        2000 /* in ms */

/*
 * The maximum number of credits that we will reap
 * from the Target at one time.
 */
#ifndef USER_SPACE
#define HTC_TX_CREDITS_REAP_MAX           32
#else
#define HTC_TX_CREDITS_REAP_MAX           4
#endif

/* Mailbox address in SDIO address space */
#define MBOX_BASE_ADDR                 0x800    /* Start of MBOX alias spaces */
#define MBOX_WIDTH                     0x800    /* Width of each mailbox alias space */

#define MBOX_START_ADDR(mbox) \
   (MBOX_BASE_ADDR + ((mbox) * (MBOX_WIDTH)))

/* The byte just before this causes an EndOfMessage interrupt to be generated */
#define MBOX_END_ADDR(mbox) \
    (MBOX_START_ADDR(mbox) + MBOX_WIDTH)

/* extended MBOX address for larger MBOX writes to MBOX 0 (not used) */
#define MBOX0_EXTENDED_BASE_ADDR       0x2800
#define MBOX0_EXTENDED_WIDTH           (6*1024)

/* HTC message header */
struct HTC_header {
    uint32_t total_msg_length;
} __attribute__ ((packed));
#define HTC_HEADER_LEN sizeof(struct HTC_header)

/* ------- Event Related Data Structures ------- */
/*
 * Populate an HTC_EVENT_INFO structure to be passed to
 * a user's event handler.
 */
#define FRAME_EVENT(_eventInfo, _buffer, _bufferLength, \
                    _actualLength, _status, _cookie) \
do { \
    (_eventInfo).buffer  = (_buffer); \
    (_eventInfo).bufferLength = (_bufferLength); \
    (_eventInfo).actualLength = (_actualLength); \
    (_eventInfo).status = (_status); \
    (_eventInfo).cookie = (_cookie); \
} while (0)

/*
 * Global and endpoint-specific event tables use these to
 * map an event ID --> handler + param.
 */
struct htc_event_table_element {
    HTC_EVENT_HANDLER handler;
    void *param;
};

/* ------ Mailbox Related Data Structures ------ */

/* This layout MUST MATCH Target hardware layout! */
struct htc_intr_status {
    uint8_t host_int_status;
    uint8_t cpu_int_status;
    uint8_t err_int_status;
    uint8_t counter_int_status;
} __attribute__ ((packed));

/* This layout MUST MATCH Target hardware layout! */
struct htc_intr_enables {
    uint8_t int_status_enb;
    uint8_t cpu_int_status_enb;
    uint8_t err_status_enb;
    uint8_t counter_int_status_enb;
} __attribute__ ((packed));

/* 
 * The Register table contains Target SDIO/SPI interrupt/rxstatus
 * registers used by HTC. Rather than read particular registers,
 * we use a bulk "register refresh" to read all at once.
 *
 * This layout MUST MATCH Target hardware layout!
 */
struct htc_register_table {
    struct htc_intr_status status;

    uint8_t mbox_frame;
    uint8_t rx_lookahead_valid;
    uint8_t hole[2];

    /* Four lookahead bytes for each mailbox */
    uint32_t rx_lookahead[HTC_NUM_MBOX];
} __attribute__ ((packed));

/*
 * Two types of requests/responses are supported:
 *   "mbox requests" are messages or data which
 *            are sent to a Target mailbox
 *   "register requests" are to read/write Target registers
 *
 * Mbox requests are managed with a per-endpoint
 * pending list and free list.
 *
 * Register requests are managed with a per-Target
 * pending list and free list.
 *
 * A generic HTC request -- one which is either an
 * htc_mbox_request or an htc_reg_request is represented
 * by an htc_request.
 */

/* Number of mbox_requests and reg_requests allocated initially.  */
#define HTC_MBOX_REQUEST_COUNT 24       /* per mailbox */
#define HTC_REG_REQUEST_COUNT  (4*HTC_NUM_MBOX) /* per target */

/*
 * An htc_request is at the start of a mbox_request structure
 * and at the start of a reg_request structure.
 *
 * Specific request types may be cast to a generic htc_request
 * (e.g. in order to invoke the completion callback function)
 */
struct htc_request {
    struct htc_request *next;   /* linkage */
    struct htc_target *target;
    void (*completionCB) (struct htc_request * req, HTC_STATUS status);
    HIF_STATUS status;          /* completion status from HIF */
};

struct htc_endpoint;            /* forward reference */

/* Mailbox request -- a message or bulk data */
struct htc_mbox_request {
    struct htc_request req;     /* Must be first -- (cast to htc_request) */

    /* Caller-supplied cookie associated with this request */
    void *cookie;

    /* 
     * Pointer to the start of the buffer. In the transmit 
     * direction this points to the start of the payload. In the 
     * receive direction, however, the buffer when queued up 
     * points to the start of the HTC header but when returned 
     * to the caller points to the start of the payload 
     *
     * Note: buffer is set to NULL whenever this request is free.
     */
    uint8_t *buffer;

    /* length, in bytes, of the buffer */
    uint32_t bufferLength;

    /* length, in bytes, of the payload within the buffer */
    uint32_t actualLength;

    struct htc_endpoint *endPoint;
};

/* Round up a value (e.g. length) to a power of 2 (e.g. block size).  */
#define HTC_ROUND_UP(value, pwrof2) (((value) + (pwrof2) - 1) & ~((pwrof2) - 1))

/* Indicates reasons that we might access Target register space */
typedef enum {
    UNUSED_PURPOSE,
    INTR_REFRESH,               /* Fetch latest interrupt/status registers */
    CREDIT_REFRESH,             /* Reap credits */
    STOP_POLLING,               /* Use interrupts only -- no polling */
    START_POLLING,              /* Use polling mode, with some interrupts */
} htc_req_purpose;

/*
 * Register read request -- used to read registers from SDIO/SPI space.
 * Register writes are fire and forget; no completion is needed.
 *
 */
struct htc_reg_request {
    struct htc_request req;     /* Must be first -- (cast to htc_request) */
//    HTC_TARGET *target;
    uint8_t *buffer;            /* register value(s) */
    uint32_t length;

    htc_req_purpose purpose;    /* Indicates the purpose this request was made */

    /*
     * Which endpoint this read is for.
     * Used when processing a completed credit refresh request.
     */
    HTC_ENDPOINT_ID epid;       /* which endpoint ID [0..3] */

    /*
     * A read to Target register space returns
     *     one specific Target register value OR
     *     all values in the register_table OR
     *     a special repeated read-and-dec from a credit register
     *
     * FUTURE: We could separate these into separate request
     * types in order to perhaps save a bit of space....
     * eliminate the union.
     */
    union {
        struct htc_intr_enables enb;
        struct htc_register_table regTable;
        uint32_t credit_dec_results[HTC_TX_CREDITS_REAP_MAX];
    } u;
};

struct htc_request_queue {
    struct htc_request *head;
    struct htc_request *tail;
};

#define HTC_IS_QUEUE_EMPTY(q) ((q)->head == NULL)

/* List of Target registers in SDIO/SPI space which can be accessed by Host */
typedef enum {
    UNUSED_REG = 0,
    INTR_ENB_REG = INT_STATUS_ENABLE_ADDRESS,
    ALL_STATUS_REG = HOST_INT_STATUS_ADDRESS,
    ERROR_INT_STATUS_REG = ERROR_INT_STATUS_ADDRESS,
    CPU_INT_STATUS_REG = CPU_INT_STATUS_ADDRESS,
    TX_CREDIT_COUNTER_DECREMENT_REG = COUNT_DEC_ADDRESS,
    INT_TARGET_REG = INT_TARGET_ADDRESS,
} TARGET_REGISTERS;

#define getRegAddr(which, epid) \
        (((which) == TX_CREDIT_COUNTER_DECREMENT_REG) ? \
                (COUNT_DEC_ADDRESS + (HTC_NUM_MBOX + (epid)) * 4) : (which))

/*
 * FUTURE: See if we can use lock-free operations
 * to manage credits and linked lists.
 * FUTURE: Use standard Linux queue ops; ESPECIALLY
 * if they support lock-free operation.
 */

/* One of these per endpoint */
struct htc_endpoint {
    /* Enabled or Disabled */
    bool enabled;

    /*
     * If data is available, rxLengthPending
     * indicates the length of the incoming message.
     */
    uint32_t rxFrameLength;     /* incoming frame length on this endpoint */
    /* includes HTC header */
    /* Modified only by complTask */

    bool rxDataAlerted;         /* Caller was sent a BUFFER_AVAILABLE event */
    /* and has not supplied a new recv buffer */
    /* since that warning was sent.  */
    /* Modified only by workTask */

    bool txCreditsToReap;       /* At least one credit available at the */
    /* Target waiting to be reaped. */
    /* Modified only by complTask */

    /* Guards txCreditsAvailable and txCreditRefreshInProgress */
#ifndef USER_SPACE 
    spinlock_t txCreditLock;
#else
	sem_t    txCreditLock;
#endif

    /*
     * The number of credits that we have already reaped
     * from the Target. (i.e. we have decremented the Target's
     * count register so that we have ability to send future
     * messages). We have the ability to send txCreditsAvailable
     * messages without blocking.
     *
     * The size of a message is endpoint-dependent and always
     * a multiple of the endpoint's blockSize.
     */
    uint32_t txCreditsAvailable;

    /* Indicates that we are in the midst of a credit refresh cycle */
    bool txCreditRefreshInProgress;

    /*
     * Free/Pending Send/Recv queues are used for mbox requests.
     * An mbox Send request cannot be given to HIF until we have
     * a tx credit. An mbox Recv request cannot be given to HIF
     * until we have a pending rx msg.
     *
     * The HIF layer maintains its own queue of requests, which
     * it uses to serialize access to SDIO. Its queue contains
     * a mixture of sends/recvs and mbox/reg requests. HIF is
     * "beyond" flow control so once a requets is given to HIF
     * it is guaranteed to complete (after all previous requests
     * complete).
     */

    /* Guards Free/Pending send/recv queues */
#ifndef USER_SPACE
    spinlock_t mboxQueueLock;
#else
	sem_t    mboxQueueLock;

#endif
    struct htc_request_queue sendFreeQueue;
    struct htc_request_queue sendPendingQueue;
    struct htc_request_queue recvFreeQueue;
    struct htc_request_queue recvPendingQueue;

    /* Inverse reference to the target */
    HTC_TARGET *target;

    /* Block size configured for the endpoint */
    uint32_t blockSize;

    /* Mapping table for per-endpoint events */
    struct htc_event_table_element endpointEventTable[HTC_EVENT_EP_COUNT];

    /* Location of the endpoint's mailbox space */
    uint32_t mboxStartAddr;
    uint32_t mboxEndAddr;
};

/* Convert an endpoint POINTER into an endpoint ID [0..3] */
#define GET_ENDPOINT_ID(_ep) ((_ep) - (_ep)->target->endPoint)

#define ENDPOINT_UNUSED 0

/* ------- Target Related Data structures ------- */

/* Target interrupt states. */
typedef enum {
    /*
     * rxdata and txcred interrupts enabled.
     * Only the DSR context can switch us to
     * polled state.
     */
    HTC_INTERRUPT,

    /*
     * rxdata and txcred interrupts are disabled.
     * We are polling (via RegisterRefresh).
     * Only the workTask can switch us to
     * interrupt state.
     */
    HTC_POLL,
} intr_state_e;

/* One of these per connected QCA402X device. */
typedef struct htc_target {
    /*
     * Target device is initialized and ready to go?
     * This has little o do with Host state;
     * it reflects readiness of the Target.
     */
    bool ready;

    /* Handle passed to HIF layer for SDIO/SPI Host controller access */
    void *HIF_handle;           /* HIF_DEVICE */

    /* Per-endpoint info */
    struct htc_endpoint endPoint[HTC_NUM_MBOX];

    /*
     * Used during startup while the Host waits for the
     * Target to initialize.
     */
#ifndef USER_SPACE
    wait_queue_head_t targetInitWait;
#else
    htc_event_signal_t *targetInitWait;
#endif

    /*
     * Free queue for htc_reg_requests.
     *
     * We don't need a regPendingQueue because reads/writes to
     * Target register space are not flow controlled by the Target.
     * There is no need to wait for credits in order to hand off a
     * register read/write to HIF.
     *
     * The register read/write may end up queued in a HIF queue
     * behind both register and mbox reads/writes that were
     * handed to HIF earlier. But they will never be queued
     * by HTC.
     */
#ifndef USER_SPACE
    spinlock_t regQueueLock;
#else
    sem_t    regQueueLock;
#endif
    struct htc_request_queue regFreeQueue;

#ifndef USER_SPACE
    struct mutex task_mutex;
#else
    sem_t    task_mutex;
#endif

#ifndef USER_SPACE
    struct task_struct *workTask;
    struct task_struct *complTask;
#else
    pthread_t  workTask;
    pthread_t  complTask;
#endif

    /* workTask synchronization */
#ifndef USER_SPACE
    wait_queue_head_t workTaskWait;     /* wait for work to do */
#else
    htc_event_signal_t *workTaskWait;
#endif
    bool workTaskHasWork;       /* work available? */
    bool workTaskShutdown;      /* requested stop? */
#ifndef USER_SPACE
    struct completion workTaskCompletion;
#else
#endif

    /* complTask synchronization */
#ifndef USER_SPACE
    wait_queue_head_t complTaskWait;    /* wait for work to do */
#else
	htc_event_signal_t *complTaskWait;
#endif

    bool complTaskHasWork;      /* work available? */
    bool complTaskShutdown;     /* requested stop? */
#ifndef USER_SPACE
    struct completion complTaskCompletion;
#else
#endif

    /* Queue of completed mailbox and register requests */
#ifndef USER_SPACE
    spinlock_t complQueueLock;
#else
    sem_t complQueueLock;
#endif
    struct htc_request_queue complQueue;

    /*
     * Software's shadow copy of interrupt enables.
     * Only the workTask changes intr_enable bits,
     * so no locking necessary.
     *
     * Commited to Target HW when
     *    we convert from polling to interrupts or
     *    we are using interrupts and enables have changed
     */
    struct htc_intr_enables enb;
    struct htc_intr_enables last_committed_enb;

    intr_state_e intr_state;

    /*
     * Set after we read data from a mailbox (to
     * update lookahead and mailbox status bits).
     * used only by workTask even though refreshes
     * may be started in other contexts.
     */
    bool needRegisterRefresh;

#ifndef USER_SPACE
#else
    sem_t eventLock;    /* protects all event tables */
    sem_t int_enbs_change_lock;    /* protects interrupt enable */

    uint8_t  need_credit_int_enable;   /* current credit counter INT status enable */
#endif

} HTC_TARGET;

/* ------- Function Prototypes for Receive -------- */
extern void htcReceiveFrame(struct htc_endpoint *endPoint);

extern uint32_t htcGetFrameLength(struct htc_endpoint *endPoint);

/* ------- Function Prototypes for Transmit -------- */
extern void htcSendFrame(struct htc_endpoint *endPoint);

extern void htcSendBlkSize(struct htc_endpoint *endPoint);

/* ------- Function Prototypes for Events and Callbacks  ------- */
extern HTC_STATUS htcRWCompletionHandler(void *req, HTC_STATUS status);

extern void htcSendCompl(struct htc_request *req, HTC_STATUS status);

extern void htcRecvCompl(struct htc_request *req, HTC_STATUS status);

extern void htcRegCompl(struct htc_request *req, HTC_STATUS status);

extern HTC_STATUS htcTargetInsertedHandler(void *context, void *hif_handle);

extern HTC_STATUS htcTargetRemovedHandler(void *context, void *hif_handle);

extern HTC_STATUS htcDSRHandler(void *target_ctxt);

extern void htcServiceCPUInterrupt(HTC_TARGET * target, struct htc_reg_request *req);

extern void htcServiceErrorInterrupt(HTC_TARGET * target, struct htc_reg_request *req);

extern void htcServiceCreditCounterInterrupt(HTC_TARGET * target, struct htc_reg_request *req);

extern void htcServiceRxdataInterrupt(HTC_TARGET * target, struct htc_reg_request *req);

extern void htcEnableCreditCounterInterrupt(HTC_TARGET * target, HTC_ENDPOINT_ID endPointId);

extern void htcDisableCreditCounterInterrupt(HTC_TARGET * target, HTC_ENDPOINT_ID endPointId);

/* ------- Function Prototypes for Utility routines ------- */
extern HTC_STATUS htcAddToMboxQueue(struct htc_mbox_request *queue,
                                    uint8_t * buffer, uint32_t bufferLength, uint32_t actualLength, void *cookie);

extern struct htc_mbox_request *htcRemoveFromMboxQueue(struct
                                                       htc_mbox_request
                                                       *queue);

extern void htcMboxQueueFlush(struct htc_endpoint *endPoint,
                              struct htc_request_queue *pendingQueue,
                              struct htc_request_queue *freeQueue, HTC_EVENT_ID eventId);

extern HTC_STATUS htcAddToEventTable(HTC_TARGET * target,
                                     HTC_ENDPOINT_ID endPointId,
                                     HTC_EVENT_ID eventId, HTC_EVENT_HANDLER handler, void *param);

extern HTC_STATUS htcRemoveFromEventTable(HTC_TARGET * target, HTC_ENDPOINT_ID endPointId, HTC_EVENT_ID eventId);

extern void htcDispatchEvent(HTC_TARGET * target,
                             HTC_ENDPOINT_ID endPointId, HTC_EVENT_ID eventId, HTC_EVENT_INFO * eventInfo);

extern void htcReportFailure(HTC_STATUS status);

extern HTC_TARGET *htcTargetInstance(int i);

extern void htcTargetInstanceAdd(HTC_TARGET * target);

extern void htcTargetInstanceRemove(HTC_TARGET * target);

extern uint8_t htcGetBitNumSet(uint32_t data);

extern void htcRegisterRefresh(HTC_TARGET * target);

extern void freeRequest(struct htc_request *req, struct htc_request_queue *queue);

extern HTC_TARGET *HTCTargetList[HTC_NUM_DEVICES_MAX];

extern void htcWorkTaskStop(HTC_TARGET * target);
extern void htcComplTaskStop(HTC_TARGET * target);
extern void htcWorkTaskPoke(struct htc_target *target);
extern void htcComplTaskPoke(struct htc_target *target);

extern struct htc_event_table_element *htcEventIDtoEvent(HTC_TARGET * target,
                                                         HTC_ENDPOINT_ID endPointId, HTC_EVENT_ID eventID);

extern void htcRequestEnqTail(struct htc_request_queue *queue, struct htc_request *req);
extern struct htc_request *htcRequestDeqHead(struct htc_request_queue *queue);

extern void htcRegisterRefreshStart(struct htc_target *target);
extern void htcRegisterRefreshCompl(struct htc_target *target, struct htc_reg_request *req);

extern void htcCreditRefreshStart(struct htc_endpoint *endPoint);
extern void htcCreditRefreshCompl(struct htc_target *target, struct htc_reg_request *req);

extern void htcUpdateIntrEnbs(struct htc_target *target);
extern void htcUpdateIntrEnbsCompl(struct htc_target *target, struct htc_reg_request *req);

extern bool htcNegotiateConfig(struct htc_target *target);

extern HTC_STATUS htcRecvRequestToHIF(struct htc_endpoint *endPoint, struct htc_mbox_request *mboxRequest);
extern HTC_STATUS htcSendRequestToHIF(struct htc_endpoint *endpoint, struct htc_mbox_request *mboxRequest);

extern int htcManagePendingSends(struct htc_target *target, int epid);
extern int htcManagePendingRecvs(struct htc_target *target, int epid);

extern HTC_STATUS htcWorkTaskStart(struct htc_target * target);
extern HTC_STATUS htcComplTaskStart(struct htc_target * target);

#endif                          /* _HTC_INTERNAL_H_ */
