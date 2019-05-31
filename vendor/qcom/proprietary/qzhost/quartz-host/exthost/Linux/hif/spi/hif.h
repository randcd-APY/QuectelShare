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

#ifndef _HIF_H_
#define _HIF_H_

#include    <semaphore.h>

//==============================================================================
// HIF specific declarations and prototypes
//==============================================================================

typedef int HIF_STATUS;
#define HIF_OK          0
#define HIF_PENDING     1
#define HIF_ERROR       2
#define HIF_EINVAL      3

typedef struct hif_device HIF_DEVICE;

/*
 * direction - Direction of transfer (HIF_READ/HIF_WRITE).
 */
#define HIF_READ                    0x00000001
#define HIF_WRITE                   0x00000002
#define HIF_DIR_MASK                (HIF_READ | HIF_WRITE)

/*
 *     type - An interface may support different kind of read/write commands.
 *            For example: SDIO supports CMD52/CMD53s. In case of MSIO it
 *            translates to using different kinds of TPCs. The command type
 *            is thus divided into a basic and an extended command and can
 *            be specified using HIF_BASIC_IO/HIF_EXTENDED_IO.
 */
#define HIF_BASIC_IO                0x00000004
#define HIF_EXTENDED_IO             0x00000008
#define HIF_TYPE_MASK               (HIF_BASIC_IO | HIF_EXTENDED_IO)

/*
 *     emode - This indicates the whether the command is to be executed in a
 *             blocking or non-blocking fashion (HIF_SYNCHRONOUS/
 *             HIF_ASYNCHRONOUS). The read/write data paths in HTC have been
 *             implemented using the asynchronous mode allowing the the bus
 *             driver to indicate the completion of operation through the
 *             registered callback routine. The requirement primarily comes
 *             from the contexts these operations get called from (a driver's
 *             transmit context or the ISR context in case of receive).
 *             Support for both of these modes is essential.
 */
#define HIF_SYNCHRONOUS             0x00000010
#define HIF_ASYNCHRONOUS            0x00000020
#define HIF_EMODE_MASK              (HIF_SYNCHRONOUS | HIF_ASYNCHRONOUS)

/*
 *     dmode - An interface may support different kinds of commands based on
 *             the tradeoff between the amount of data it can carry and the
 *             setup time. Byte and Block modes are supported (HIF_BYTE_BASIS/
 *             HIF_BLOCK_BASIS). In case of latter, the data is rounded off
 *             to the nearest block size by padding. The size of the block is
 *             configurable at compile time using the HIF_BLOCK_SIZE and is
 *             negotiated with the target during initialization after the
 *             AR6000 interrupts are enabled.
 */
#define HIF_BYTE_BASIS              0x00000040
#define HIF_BLOCK_BASIS             0x00000080
#define HIF_DMODE_MASK              (HIF_BYTE_BASIS | HIF_BLOCK_BASIS)

/*
 *     amode - This indicates if the address has to be incremented on AR6000
 *             after every read/write operation (HIF?FIXED_ADDRESS/
 *             HIF_INCREMENTAL_ADDRESS).
 */
#define HIF_FIXED_ADDRESS           0x00000100
#define HIF_INCREMENTAL_ADDRESS     0x00000200
#define HIF_AMODE_MASK              (HIF_FIXED_ADDRESS | HIF_INCREMENTAL_ADDRESS)

/*
 *     hmode - This indicates if read/write operation is on interface/Host 
 *             address space  (HIF? HIF_HOST_ADDRESS/HIF_INTERFACE_ADDRESS).
 *             
 */
#define HIF_HOST_ADDRESS            0x00000400
#define HIF_INTERFACE_ADDRESS       0x00000800
#define HIF_FIFO_ADDRESS            0x00001000
#define HIF_HMODE_MASK              (HIF_HOST_ADDRESS | HIF_INTERFACE_ADDRESS)

/*
 *     rmode - This indicates if read/write operation is on byte/short/int(32bits) 
 *             address space  (HIF? HIF_HOST_ADDRESS/HIF_INTERFACE_ADDRESS).
 *             
 */
#define HIF_REGTYPE_BYTE            0x00002000
#define HIF_REGTYPE_SHORT           0x00004000
#define HIF_REGTYPE_INT32           0x00008000
#define HIF_REGTYPE_MASK            (HIF_REGTYPE_BYTE | HIF_REGTYPE_SHORT | HIF_REGTYPE_INT32)

#define HIF_WR_ASYNC_BYTE_FIX   \
    (HIF_WRITE | HIF_ASYNCHRONOUS | HIF_EXTENDED_IO | HIF_BYTE_BASIS | HIF_FIXED_ADDRESS)
#define HIF_WR_ASYNC_BYTE_INC   \
    (HIF_WRITE | HIF_ASYNCHRONOUS | HIF_EXTENDED_IO | HIF_BYTE_BASIS | HIF_INCREMENTAL_ADDRESS)
#define HIF_WR_ASYNC_BLOCK_INC  \
    (HIF_WRITE | HIF_ASYNCHRONOUS | HIF_EXTENDED_IO | HIF_BLOCK_BASIS | HIF_INCREMENTAL_ADDRESS)
#define HIF_WR_SYNC_BYTE_FIX    \
    (HIF_WRITE | HIF_SYNCHRONOUS | HIF_EXTENDED_IO | HIF_BYTE_BASIS | HIF_FIXED_ADDRESS)
#define HIF_WR_SYNC_BYTE_INC    \
    (HIF_WRITE | HIF_SYNCHRONOUS | HIF_EXTENDED_IO | HIF_BYTE_BASIS | HIF_INCREMENTAL_ADDRESS)
#define HIF_WR_SYNC_BLOCK_INC  \
    (HIF_WRITE | HIF_SYNCHRONOUS | HIF_EXTENDED_IO | HIF_BLOCK_BASIS | HIF_INCREMENTAL_ADDRESS)
#define HIF_WR_ASYNC_BLOCK_FIX \
    (HIF_WRITE | HIF_ASYNCHRONOUS | HIF_EXTENDED_IO | HIF_BLOCK_BASIS | HIF_FIXED_ADDRESS)
#define HIF_WR_SYNC_BLOCK_FIX  \
    (HIF_WRITE | HIF_SYNCHRONOUS | HIF_EXTENDED_IO | HIF_BLOCK_BASIS | HIF_FIXED_ADDRESS)
#define HIF_RD_SYNC_BYTE_INC    \
    (HIF_READ | HIF_SYNCHRONOUS | HIF_EXTENDED_IO | HIF_BYTE_BASIS | HIF_INCREMENTAL_ADDRESS)
#define HIF_RD_SYNC_BYTE_FIX    \
    (HIF_READ | HIF_SYNCHRONOUS | HIF_EXTENDED_IO | HIF_BYTE_BASIS | HIF_FIXED_ADDRESS)
#define HIF_RD_ASYNC_BYTE_FIX   \
    (HIF_READ | HIF_ASYNCHRONOUS | HIF_EXTENDED_IO | HIF_BYTE_BASIS | HIF_FIXED_ADDRESS)
#define HIF_RD_ASYNC_BLOCK_FIX  \
    (HIF_READ | HIF_ASYNCHRONOUS | HIF_EXTENDED_IO | HIF_BLOCK_BASIS | HIF_FIXED_ADDRESS)
#define HIF_RD_ASYNC_BYTE_INC   \
    (HIF_READ | HIF_ASYNCHRONOUS | HIF_EXTENDED_IO | HIF_BYTE_BASIS | HIF_INCREMENTAL_ADDRESS)
#define HIF_RD_ASYNC_BLOCK_INC  \
    (HIF_READ | HIF_ASYNCHRONOUS | HIF_EXTENDED_IO | HIF_BLOCK_BASIS | HIF_INCREMENTAL_ADDRESS)
#define HIF_RD_SYNC_BLOCK_INC  \
    (HIF_READ | HIF_SYNCHRONOUS | HIF_EXTENDED_IO | HIF_BLOCK_BASIS | HIF_INCREMENTAL_ADDRESS)
#define HIF_RD_SYNC_BLOCK_FIX  \
    (HIF_READ | HIF_SYNCHRONOUS | HIF_EXTENDED_IO | HIF_BLOCK_BASIS | HIF_FIXED_ADDRESS)


#define HIF_WR_SYNC_INTERFACE       (HIF_WR_SYNC_BYTE_INC | HIF_INTERFACE_ADDRESS)
#define HIF_RD_SYNC_INTERFACE       (HIF_RD_SYNC_BYTE_INC | HIF_INTERFACE_ADDRESS)

#define HIF_WR_SYNC_HOST            (HIF_WR_SYNC_BYTE_INC | HIF_HOST_ADDRESS)

#define HIF_WR_SYNC_BYTE_INC_HOST   (HIF_WR_SYNC_BYTE_INC | HIF_HOST_ADDRESS)
#define HIF_RD_SYNC_BYTE_INC_HOST   (HIF_RD_SYNC_BYTE_INC | HIF_HOST_ADDRESS)

#define HIF_WR_SYNC_SHORT_INC_HOST  (HIF_WR_SYNC_BYTE_INC | HIF_REGTYPE_SHORT | HIF_HOST_ADDRESS)
#define HIF_RD_SYNC_SHORT_INC_HOST  (HIF_RD_SYNC_BYTE_INC | HIF_REGTYPE_SHORT | HIF_HOST_ADDRESS)

#define HIF_WR_SYNC_INT32_INC_HOST  (HIF_WR_SYNC_BYTE_INC | HIF_REGTYPE_INT32 | HIF_HOST_ADDRESS)
#define HIF_RD_SYNC_INT32_INC_HOST  (HIF_RD_SYNC_BYTE_INC | HIF_REGTYPE_INT32 | HIF_HOST_ADDRESS)

#define HIF_WR_ASYNC_BYTE_INC_HOST  (HIF_WR_SYNC_BYTE_INC | HIF_HOST_ADDRESS)
#define HIF_RD_ASYNC_BYTE_INC_HOST  (HIF_RD_SYNC_BYTE_INC | HIF_HOST_ADDRESS)
#define HIF_RD_SYNC_BYTE_INC_HOST   (HIF_RD_SYNC_BYTE_INC | HIF_HOST_ADDRESS)
#define HIF_RD_ASYNC_BYTE_FIX_HOST  (HIF_RD_SYNC_BYTE_FIX | HIF_HOST_ADDRESS)

#define HIF_WR_SYNC_BYTE_INC_FIFO   (HIF_WR_SYNC_BYTE_INC | HIF_FIFO_ADDRESS)
#define HIF_WR_ASYNC_BLOCK_INC_FIFO (HIF_WR_ASYNC_BLOCK_INC | HIF_FIFO_ADDRESS)
#define HIF_WR_ASYNC_BYTE_INC_FIFO  (HIF_WR_ASYNC_BYTE_INC | HIF_FIFO_ADDRESS)
#define HIF_RD_ASYNC_BLOCK_INC_FIFO (HIF_RD_ASYNC_BLOCK_INC | HIF_FIFO_ADDRESS)
#define HIF_RD_ASYNC_BYTE_INC_FIFO  (HIF_RD_ASYNC_BYTE_INC | HIF_FIFO_ADDRESS)

typedef enum {
    HIF_DEVICE_GET_MBOX_BLOCK_SIZE,
    HIF_DEVICE_SET_CONTEXT,
    HIF_DEVICE_GET_CONTEXT,
} HIF_DEVICE_CONFIG_OPCODE;

/*
 *  this stuct is re-name of SPI_CXT_T in iotd_context.h
 *
 */
typedef struct HIF_INTERFACE_CONFIG_s {
    void  *pTarget;     /* HTC instance context pointer */
    sem_t rx_sem;       /* HTC receive buffer semaphore */
    uint8_t dev[32];    /*UART dev e.g. /dev/ttyUSB0*/
    uint32_t baud;      /*Configured baud rate*/
    uint32_t block_size;    /*Configured block size*/
    uint16_t  spi_intr_pin_num;  
} HIF_SPI_INTERFACE_CONFIG;

/*
 * HIF CONFIGURE definitions:
 *
 *   HIF_DEVICE_GET_MBOX_BLOCK_SIZE
 *   input : none
 *   output : array of 4 uint32_ts
 *   notes: block size is returned for each mailbox (4)
 *
 *   HIF_DEVICE_SET_CONTEXT
 *   input : arbitrary pointer-sized value
 *   output: none
 *   notes: stores an arbitrary value which can be retrieved later
 *
 *   HIF_DEVICE_GET_CONTEXT
 *   input: none
 *   output : arbitrary pointer-sized value
 *   notes: retrieves an arbitrary value which was set earlier
 */

#define HIF_MAX_DEVICES                 1

struct cbs_from_HIF {
    void *context;              /* context to pass to the dsrhandler
                                   note : rwCompletionHandler is provided the context passed to HIFReadWrite  */
    int (*rwCompletionHandler) (void *rwContext, int status);
    int (*dsrHandler) (void *context);
#ifndef USER_SPACE
#else
    uint16_t (*disableIRQ)(void *context);
    int (*enableIRQ)(void *context);
#endif
};

struct cbs_from_os {
    void *context;              /* context to pass for all callbacks except deviceRemovedHandler
                                   the deviceRemovedHandler is only called if the device is claimed */
    int (*deviceInsertedHandler) (void *context, void *hif_handle);
    int (*deviceRemovedHandler) (void *claimedContext, void *hif_handle);
    int (*deviceSuspendHandler) (void *context);
    int (*deviceResumeHandler) (void *context);
    int (*deviceWakeupHandler) (void *context);
#if defined(LENXXX)
    int (*devicePowerChangeHandler) (void *context, HIF_DEVICE_POWER_CHANGE_TYPE config);
#endif                          /* LENXXX */
};

#define HIF_OTHER_EVENTS     (1 << 0)   /* other interrupts (non-Recv) are pending, host
                                           needs to read the register table to figure out what */
#define HIF_RECV_MSG_AVAIL   (1 << 1)   /* pending recv packet */

typedef struct _HIF_PENDING_EVENTS_INFO {
    uint32_t Events;
    uint32_t LookAhead;
    uint32_t AvailableRecvBytes;
} HIF_PENDING_EVENTS_INFO;

    /* function to get pending events , some HIF modules use special mechanisms
     * to detect packet available and other interrupts */
typedef int (*HIF_PENDING_EVENTS_FUNC) (HIF_DEVICE * device, HIF_PENDING_EVENTS_INFO * pEvents, void *AsyncContext);

#define HIF_MASK_RECV    TRUE
#define HIF_UNMASK_RECV  FALSE
    /* function to mask recv events */
typedef int (*HIF_MASK_UNMASK_RECV_EVENT) (HIF_DEVICE * device, bool Mask, void *AsyncContext);

#ifdef HIF_MBOX_SLEEP_WAR
/* This API is used to update the target sleep state */
void HIFSetMboxSleep(HIF_DEVICE * device, bool sleep, bool wait, bool cache);
#endif
/*
 * This API is used to perform any global initialization of the HIF layer
 * and to set OS driver callbacks (i.e. insertion/removal) to the HIF layer
 *
 */
int HIFInit(void *pInfo, struct cbs_from_os *callbacks);

/* This API claims the HIF device and provides a context for handling removal.
 * The device removal callback is only called when the OS claims
 * a device.  The claimed context must be non-NULL */
void HIFClaimDevice(HIF_DEVICE * device, void *claimedContext);

/* release the claimed device */
void HIFReleaseDevice(HIF_DEVICE * device);

/* This API allows the calling layer to attach callbacks from HIF */
int HIFAttach(HIF_DEVICE * device, struct cbs_from_HIF *callbacks);

/* This API allows the calling layer to detach callbacks from HIF */
void HIFDetach(HIF_DEVICE * device);

void HIFSetHandle(void *hif_handle, void *handle);

HIF_STATUS
HIFSyncRead(HIF_DEVICE * device, uint32_t address, uint8_t * buffer, uint32_t length, uint32_t request, void *context);

/*
 * This API is used to provide the read/write interface over the specific bus
 * interface.
 * address - Starting address in the AR6000's address space. For mailbox
 *           writes, it refers to the start of the mbox boundary. It should
 *           be ensured that the last byte falls on the mailbox's EOM. For
 *           mailbox reads, it refers to the end of the mbox boundary.
 * buffer - Pointer to the buffer containg the data to be transmitted or
 *          received.
 * length - Amount of data to be transmitted or received.
 * request - Characterizes the attributes of the command.
 */
int HIFReadWrite(HIF_DEVICE * device, uint32_t address, void *buffer, uint32_t length, uint32_t request, void *context);

/*
 * This can be initiated from the unload driver context when the OS has no more use for
 * the device.
 */
void HIFShutDownDevice(HIF_DEVICE * device);
void HIFSurpriseRemoved(HIF_DEVICE * device);

/*
 * This should translate to an acknowledgment to the bus driver indicating that
 * the previous interrupt request has been serviced and the all the relevant
 * sources have been cleared. HTC is ready to process more interrupts.
 * This should prevent the bus driver from raising an interrupt unless the
 * previous one has been serviced and acknowledged using the previous API.
 */
void HIFAckInterrupt(HIF_DEVICE * device);

void HIFMaskInterrupt(HIF_DEVICE * device);

void HIFUnMaskInterrupt(HIF_DEVICE * device);

int HIFConfigureDevice(HIF_DEVICE * device, HIF_DEVICE_CONFIG_OPCODE opcode, void *config, uint32_t configLen);

/*
 * This API wait for the remaining MBOX messages to be drained
 * This should be moved to HTC AR6K layer
 */
int hifWaitForPendingRecv(HIF_DEVICE * device);

/****************************************************************/
/* BMI and Diag window abstraction                              */
/****************************************************************/

#define HIF_BMI_EXCHANGE_NO_TIMEOUT  ((uint32_t)(0))

#define DIAG_TRANSFER_LIMIT 2048U       /* maximum number of bytes that can be
                                           handled atomically by DiagRead/DiagWrite */

#define HIFDeviceToOsDevice(hif_device) NULL

#ifdef FEATURE_RUNTIME_PM
/* Runtime power management API of HIF to control
 * runtime pm. During Runtime Suspend the get API
 * return -EAGAIN. The caller can queue the cmd or return.
 * The put API decrements the usage count.
 * The get API increments the usage count.
 * The API's are exposed to HTT and WMI Services only.
 */
int hif_pm_runtime_get(HIF_DEVICE *);
int hif_pm_runtime_put(HIF_DEVICE *);
void *hif_runtime_pm_prevent_suspend_init(const char *);
void hif_runtime_pm_prevent_suspend_deinit(void *data);
int hif_pm_runtime_prevent_suspend(void *ol_sc, void *data);
int hif_pm_runtime_allow_suspend(void *ol_sc, void *data);
int hif_pm_runtime_prevent_suspend_timeout(void *ol_sc, void *data, unsigned int delay);
void hif_request_runtime_pm_resume(void *ol_sc);
#else
static inline int hif_pm_runtime_get(HIF_DEVICE * device)
{
    return 0;
}

static inline int hif_pm_runtime_put(HIF_DEVICE * device)
{
    return 0;
}

static inline int hif_pm_runtime_prevent_suspend(void *ol_sc, void *context)
{
    return 0;
}

static inline int hif_pm_runtime_allow_suspend(void *ol_sc, void *context)
{
    return 0;
}

static inline int hif_pm_runtime_prevent_suspend_timeout(void *ol_sc, void *context, unsigned int msec)
{
    return 0;
}

static inline void *hif_runtime_pm_prevent_suspend_init(const char *name)
{
    return NULL;
}

static inline void hif_runtime_pm_prevent_suspend_deinit(void *context)
{
}

static inline void hif_request_runtime_pm_resume(void *ol_sc)
{
}
#endif

void hif_spi_int_setup();
void hif_spi_deint_setup();
void hifIRQHandler();
HIF_STATUS HIFPacketAvailSet(void *hif_handle);
HIF_STATUS HIFPacketAvailReset(void *hif_handle);
bool HIFIsPacketAvailInt(void *hif_handle);
bool HIFIsCreditInt(void *hif_handle);
HIF_STATUS HIFSetWorkTaskProcessInt(void *hif_handle);
int HIFInWorkTaskProcessInt(void *hif_handle);
HIF_STATUS HIFResetWorkTaskProcessInt(void *hif_handle);
uint16_t HIFInterruptFlagsCheck(void *hif_handle, uint16_t flags);

HIF_STATUS HIFInterruptLock(void *hif_handle);
HIF_STATUS HIFInterruptUnLock(void *hif_handle);
HIF_STATUS HIFSetIntInProcessVar(void *hif_handle);
HIF_STATUS HIFWorkTaskProcessIntCondSignal(void *hif_handle);
HIF_STATUS HIFWorkTaskProcessIntCondWait(void *hif_handle);
void HIFEnableInterrupt(HIF_DEVICE * device, uint16_t enble_bits);
void HIFDisableInterrupt(HIF_DEVICE * device, uint16_t enble_bits);
int HIFHasMoreInterrupt(void *hif_handle, uint16_t  int_cause);

#endif                          /* _HIF_H_ */
