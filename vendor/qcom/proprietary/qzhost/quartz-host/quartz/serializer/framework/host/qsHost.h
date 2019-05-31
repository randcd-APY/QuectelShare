/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QS_HOST_H__
#define __QS_HOST_H__

#include <pthread.h>
#include "qsCommon.h"

#ifdef CONFIG_DAEMON_MODE
#include <mqueue.h>
#endif /* CONFIG_DAEMON_MODE */

/* Buffer pool */
#define QS_BUFPOOL_WAIT_TIME  (5) /*Time in milliseconds to suspend when buf pools are empty*/
#define BUF_SIZE_2048      2048
#define BUF_NUM_2048       16

/* Values of event */
#define QS_EVENT_SERVER_EXIT     (0x01<<0)
#define QS_EVENT_TARGET_ASSERT   (0x01<<1)

typedef void (* WlanCb_t)(uint8_t dev_Id, uint32_t cb_Id, void *app_Cxt, void  *buf, uint32_t buf_Len);
typedef void (* WpanCb_t)(uint8_t module, uint8_t fileId, uint16_t eventId, uint32_t buf_Len, uint8_t *buf);
typedef void (* eventCb_t)(uint32_t eventId, void *param);

typedef struct {
    void *head; //queue head
    void *tail; // queue tail
    pthread_mutex_t mutex;
} queue_t;

typedef union {
    WlanCb_t  wlanCb;
    WpanCb_t  wpanCb;
} qsEventCb_e;

/* This structure defines buffers used for resquests and responses */
typedef struct bufInfo_s {
    char *buf;              /* Character buffer */
    struct bufInfo_s* next; /* Next buffer in the list */
} bufInfo_t;

/* Linked list of buffers */
typedef struct bufPool_s {
    pthread_mutex_t mutex;  /* Mutex to ensure atomic operations on the count */
    uint16_t threshold;  /* Maximum buffers in a given buffer pool */
    uint16_t count;      /* Number of available free buffers */
    queue_t queue;       /* Queue of free buffers */
} bufPool_t;


/* This structure holds state for each module */
#ifdef CONFIG_DAEMON_MODE
typedef  struct {
    eventCb_t cb;
    void *parameter;
} eventCxt_t;

typedef struct {
    uint32_t        mask;
    pthread_mutex_t mutex;
    pthread_cond_t  event;
    queue_t         queue;
    qsEventCb_e     event_Cb;
} moduleCxt_t;

/* Structure to hold IPC related data */
typedef struct {
    pthread_t rxThread;    /* Thread to receive QAPI responses and async events from the daemon */
    mqd_t qdServer;        /* Server queue */
    char  serverName[64];  /* Server queue name */
    mqd_t qdClient;        /* Client Rx queue */
    char  clientName[64];  /* Client queue name */
    uint8_t initialized;   /* Set to 1 when HELLO is received from server */
    pthread_cond_t cv;     /* Conditional variable for synchronization */
    pthread_mutex_t mutex; /* Mutex for atomic operation */
} ipcCxt_t;

/* Common host structure with all the contexts */
typedef struct qsCommonCxt_s {
    bufPool_t   *bufPool; /* Free buffer pool */
    moduleCxt_t *cxts[QS_MODULE_MAX];    /* Individual module contexts */
    ipcCxt_t    *ipcCxt;                 /* IPC context */
    eventCxt_t  *eventCxt;               /* Event context */
} qsCommonCxt_t;
extern qsCommonCxt_t *qsCommonCxt;

extern uint8_t qsTargetId;

/* Macros to retrieve individual contexts */
#define QH_COMMON_CXT(cxt)                    ((cxt) ? ((qsCommonCxt_t *)(cxt)) : NULL)
#define QH_BUFPOOL(cxt,i)                     (QH_COMMON_CXT(cxt) ? QH_COMMON_CXT(cxt)->bufPool[(i)] : NULL)
#define QH_IS_MODULE_VALID(module)            ((((module) >= 0) && ((module) < QS_MODULE_MAX)) ? 1 : 0)
#define QH_MODULE_INFO_CXT(cxt,module)        ((QH_COMMON_CXT(cxt) && QH_IS_MODULE_VALID(module)) ? \
        QH_COMMON_CXT(cxt)->cxts[(module)] : NULL)

#else

typedef struct {
    uint16_t        transport;
    int32_t         handle;
    uint32_t        mask;
    pthread_mutex_t mutex;
    pthread_cond_t  event;
    queue_t         queue;
    qsEventCb_e     event_Cb;
} qsHostModuleCxt_t;

#endif /* CONFIG_DAEMON_MODE */


/**
  Allocates a packed buffer.

  @param TargetID     Target device ID.
  @param PacketType   Type of packet to be created.
  @param ModuleID     ID of the module sending this packet.
  @param FileID       ID of the file sending this packet.
  @param FunctionID   ID of the function sending this packet.
  @param PackedBuffer Pointer to the packed buffer to be allocated.
  @param Size         Required size of the buffer.

  @return the status result of the allocation.
*/
Boolean_t AllocatePackedBuffer(uint8_t TargetID, qsPacketType_t PacketType, uint8_t ModuleID, uint16_t FileID, uint16_t FunctionID, PackedBuffer_t *PackedBuffer, uint16_t Size);

/**
  Frees a packed buffer.

  @param PackedBuffer Pointer to the packed buffer to be freed.

*/
void FreePackedBuffer(PackedBuffer_t *PackedBuffer);

/**
  Send QAPI from host to target.

  @param InputBuffer  Input bufer with QAPI request.
  @param OutputBuffer Output bufer to retrieve QAPI response.

  @return 0 for success, non-zero for failure.
*/
SerStatus_t SendCommand(PackedBuffer_t *InputBuffer, PackedBuffer_t *OutputBuffer);


/**
  Send QAPI from host to target.

  @param InputBuffer  Input bufer with QAPI request.
  @param OutputBuffer Output bufer to retrieve QAPI response.
  @param WaitForResp  Set to true for all except QAPI data commands.

  @return 0 for success, non-zero for failure.
*/
SerStatus_t _SendCommand(PackedBuffer_t *InputBuffer, PackedBuffer_t *OutputBuffer, uint8_t WaitForResp);

#ifdef CONFIG_DAEMON_MODE

/**
  Set target ID (used only for TABI.

  @param targetId   Name of daemon server queue.

  @return 0 for success, non-zero for failure.
*/
uint32_t QsSetTargetId(uint8_t targetId);

/**
  Initialize Quartz serializer feature.

  @param ServerName   Name of daemon server queue.
  @param MaxMsgSize   Maximum possible message size.
  @param MaxMsgCnt    Maximum possible message count.

  @return 0 for success, non-zero for failure.
*/
uint32_t QsInit(char *ServerName, uint16_t MaxMsgSize, uint16_t MaxMsgCnt);

/**
  De-initialize Quartz serializer feature.
*/
void QsDeInit();

/* register event callback.

  @param eventNum     Event Number.
  @param cb           Event callback.
  @param parameter    Parameter to pass the callback.

  @return 0 for success, non-zero for failure.
*/
uint32_t QsRegisterCb(eventCb_t cb, void *parameter);


#else

/**
  Initialize Quartz serializer feature.

  @param ModuleID     ID of the module to be initialized.
  @param TransportID  ID of the transport type.
  @param Ifname       Interface name.

  @return 0 for success, non-zero for failure.
*/
uint32_t QsInit(uint8_t ModuleID, uint32_t TransportID, char *Ifname);

/**
  De-initialize Quartz serializer feature.

  @param ModuleID     ID of the module to be de-initialized.

  @return 0 for success, non-zero for failure.
*/
uint32_t QsDeInit(uint8_t ModuleID);

#endif /* CONFIG_DAEMON_MODE */


/**
  De-initialize Quartz serializer feature.

  @param ModuleID     Module ID.
  @param Callback     Callback function.

  @return 0 for success, non-zero for failure.
  */
uint32_t QsSetModuleCb(uint8_t ModuleID, void *Callback);


#ifdef LATENCY_DEBUG
long QsGetMicrotime();
void QsGetTotalTime(void *arg);
#endif /* LATENCY_DEBUG */


#endif // __QS_HOST_H__

