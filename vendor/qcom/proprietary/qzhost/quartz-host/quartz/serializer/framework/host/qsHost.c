/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifdef CONFIG_DAEMON_MODE

#include <stdlib.h>
#include <string.h>
#include "qsCommon.h"
#include "qsHost.h"
#include "qsHostUart.h"
#include "qsOSAL.h"
#include "qsConfig.h"
#include "qsPack.h"
#include "idlist.h"

#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdint.h>
#include <sys/time.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>

/*****************************************************************************/
static uint32_t QsProcessServerEvent(void *cxt, uint32_t event);
static void QsProcessControl(qsCommonCxt_t *cxt, char *packet);
static void QsProcessQapi(qsCommonCxt_t *cxt, char *packet);
static SerStatus_t QsGetBuf(char **Buf, uint16_t Size);
static SerStatus_t QsReleaseBuf(char *Buf);

/*****************************************************************************/

/* This structure is used to keep track of the unique cookie in each QAPI request and response.
 * The member 'cv' is the unique condition variable put in each request and copies into each response.
 * Application thread waits on this condition variable which is signaled when response is received.
 * The flag is initialized to QAPI_RESP_REQD when request is sent. When response comes, it is set to
 * QAPI_RESP_RCVD to indicate the same. If the command times out before the response is received then
 * it is set to QAPI_REQ_TIMED_OUT so that the response is dropped instead of being enqueued. */
#define QAPI_RESP_REQD      0
#define QAPI_RESP_RCVD      1
#define QAPI_REQ_TIMED_OUT  2
#define QAPI_RESP_WAIT_TIME 30 /* Seconds */

typedef struct
{
    uint8_t flag;
    pthread_cond_t cv;
} Uid_t;

qsCommonCxt_t *qsCommonCxt = NULL;
uint8_t qsTargetId = 0;

#define QUEUE_PERMISSIONS              0660

uint32_t QsSetTargetId(uint8_t targetId)
{
    if(targetId >= QS_NUM_DEVICES)
    {
        return ssFailure;
    }
    qsTargetId = targetId;
    return ssSuccess;
}

static qsCommonCxt_t *GetCommonCxt()
{
    return qsCommonCxt;
}

static void* QsRxThread(void* cxt)
{
    qsCommonCxt_t *commonCxt = (qsCommonCxt_t *)cxt;
    ipcCxt_t *ipcCxt = commonCxt->ipcCxt;
    uint8_t ctrlType;
    int sz;
    char *packet, hello_msg[64] = {'\0'};

    do
    {
        /* Send hello message to server.
         * Bytes 0-3 : HTC Header.
         * Bytes 4: Unused.
         * Byte  5: bit 0 is used to indicate Control packet
         * Bytes 6-9 : Client process ID used by daemon to find the correct process for response
         * Byte  10  : Message type (QS_HELLO_REQ)
         * Bytes 10+ : Variable length client queue name string.
         */
        QS_SET_CONTROL_BIT(hello_msg);
        hello_msg[QS_CONTROL_MSG_TYPE_OFFSET] = QS_HELLO_REQ;
        memcpy(&hello_msg[QS_CONTROL_MSG_TYPE_OFFSET + 1], ipcCxt->clientName,\
                strlen(ipcCxt->clientName));
        WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(&hello_msg[QS_IOTD_COOKIE_OFFSET],\
                                              getpid());

        printf("Sending Hello Message\n");
        sz = mq_send(ipcCxt->qdServer, (const char *)hello_msg,
                     QS_CONTROL_MSG_TYPE_OFFSET + 1 + strlen(ipcCxt->clientName), 0);
        if(-1 == sz)
        {
            perror ("Client: Not able to send message to server");
            break;
        }

        while(1)
        {
            packet = NULL;
            if((ssSuccess != QsGetBuf(&packet, BUF_SIZE_2048)) || !packet)
                break;

            sz = mq_receive(ipcCxt->qdClient, packet, BUF_SIZE_2048, NULL);
            if (-1 == sz)
            {
                break;
            }

            ctrlType = QS_GET_CONTROL_BIT(packet);
            if(ctrlType)
                QsProcessControl(commonCxt, packet);
            else
                QsProcessQapi(commonCxt, packet);
        }
    } while(0);

    /* Close server queue */
    mq_close(ipcCxt->qdServer);
    mq_close(ipcCxt->qdClient);
    mq_unlink(ipcCxt->clientName);
    ipcCxt->qdClient = -1;
    free(ipcCxt);
    commonCxt->ipcCxt = NULL;
    pthread_exit(cxt);
}

static int32_t ReadWlanEventBuffer(void *Buffer, uint16_t qsLength, uint8_t *DevID, uint32_t *CbID, void **AppCxt, void **payload, uint32_t *payload_Length)
{
    SerStatus_t        qsResult = ssSuccess;
    BufferListEntry_t *qsBufferList = NULL;
    PackedBuffer_t     qsInputBuffer;
    Boolean_t          qsPointerValid = FALSE;

    if(!Buffer || !DevID || !CbID || !AppCxt || !payload || !payload_Length)
    {
        return -1;
    }

    /* Read the packet parameters. */
    InitializePackedBuffer(&qsInputBuffer, (uint8_t *)Buffer, qsLength);
    if(qsResult == ssSuccess)
        qsResult = PackedRead_8(&qsInputBuffer, &qsBufferList, DevID);

    if(qsResult == ssSuccess)
        qsResult = PackedRead_32(&qsInputBuffer, &qsBufferList, CbID);

    if(qsResult == ssSuccess)
        qsResult = PackedRead_32(&qsInputBuffer, &qsBufferList, payload_Length);

    if(qsResult == ssSuccess)
        qsResult = PackedRead_PointerHeader(&qsInputBuffer, &qsBufferList, &qsPointerValid);

    if((qsResult == ssSuccess) && (qsPointerValid == TRUE))
    {
        *AppCxt = AllocateBufferListEntry(&qsBufferList, sizeof(uint32_t));

        if(*AppCxt == NULL)
        {
            qsResult = ssAllocationError;
        }
        else
        {
            qsResult = PackedRead_Array(&qsInputBuffer, &qsBufferList, *(void **)AppCxt, 1, 4);
        }
    }

    if(qsResult == ssSuccess)
        qsResult = PackedRead_PointerHeader(&qsInputBuffer, &qsBufferList, &qsPointerValid);

    if((qsResult == ssSuccess) && (qsPointerValid == TRUE) && (payload_Length != 0))
    {
        *payload = AllocateBufferListEntry(&qsBufferList, *payload_Length);

        if(*payload == NULL)
        {
            qsResult = ssAllocationError;
        }
        else
        {
            qsResult = PackedRead_Array(&qsInputBuffer, &qsBufferList, *(void **)payload, 1, *payload_Length);
        }
    }

    FreeBufferList(&qsBufferList);

    if(qsResult == ssSuccess)
    {
        return 0;
    }
    return -1;
}


static int32_t ReadEventBuffer(void *Buffer, uint8_t *ModuleID, uint8_t *FileID, uint16_t *FunctionID, uint32_t *Size)
{
    qsEvent_t *Event = (qsEvent_t *)Buffer;
    if(!Event || !ModuleID || !FileID || !FunctionID || !Size)
    {
        return -1;
    }

    /* Read the packet parameters. */
    *ModuleID = READ_UNALIGNED_LITTLE_ENDIAN_UINT8(&Event->Header.ModuleID);
    *FileID = READ_UNALIGNED_LITTLE_ENDIAN_UINT8(&Event->Header.FileID);
    *FunctionID = READ_UNALIGNED_LITTLE_ENDIAN_UINT16(&Event->Header.FunctionID);
    *Size = READ_UNALIGNED_LITTLE_ENDIAN_UINT16(&Event->Header.DataLength);
    return 0;
}

uint32_t QsSetModuleCb(uint8_t ModuleID, void *Callback)
{
    moduleCxt_t *cxt = NULL;
    uint32_t ret_Val = ssFailure;

    cxt = QH_MODULE_INFO_CXT(GetCommonCxt(), ModuleID);
    if(!cxt)
    {
        return ssFailure;
    }

    pthread_mutex_lock(&cxt->mutex);
    switch(ModuleID)
    {
        case QS_MODULE_WLAN:
            cxt->event_Cb.wlanCb = (WlanCb_t)Callback;
            ret_Val = ssSuccess;
            break;

            /* Intentational fall-through, all these modules will use    */
            /* the same Callback.                                        */
        case QS_MODULE_BLE:
        case QS_MODULE_HMI:
        case QS_MODULE_THREAD:
        case QS_MODULE_ZIGBEE:
        case QS_MODULE_HERH:
        case QS_MODULE_COEX:
        case QS_MODULE_FWUPGRADE:
        case QS_MODULE_TLMM:
            ret_Val = ssSuccess;
            cxt->event_Cb.wpanCb = (WpanCb_t)Callback;
            break;
        default:
            ret_Val = ssFailure;
    }
    pthread_mutex_unlock(&cxt->mutex);
    return ret_Val;
}

uint32_t QsInitQueue(queue_t *Q)
{
    if(!Q)
    {
        return ssFailure;
    }

    Q->head = NULL;
    Q->tail = NULL;
    if(0 != pthread_mutex_init(&Q->mutex, NULL))
    {
        return ssFailure;
    }
    return ssSuccess;
}

SerStatus_t QsEnq(queue_t *Q, void *Buf)
{
    bufInfo_t *BufInfo = NULL;
    if(!Q || !Buf)
    {
        return ssFailure;
    }

    /* Encapsulate data buffer in a shell */
    BufInfo = (bufInfo_t *)malloc(sizeof(bufInfo_t));
    if(!BufInfo)
        return ssFailure;
    BufInfo->buf = Buf;
    BufInfo->next = NULL;

    /* Enqueue the buffer at the end of the queue */
    pthread_mutex_lock(&Q->mutex);
    if(!Q->head && !Q->tail)
    {
        Q->head = (void *)BufInfo;
        Q->tail = (void *)BufInfo;
    }
    else
    {
        ((bufInfo_t *)Q->tail)->next = BufInfo;
        Q->tail = (void *)BufInfo;
    }
    pthread_mutex_unlock(&Q->mutex);
    return ssSuccess;
}

SerStatus_t QsDeq(queue_t *Q, void **buf)
{
    bufInfo_t *BufInfo = NULL;

    if(!Q || !buf)
    {
        return ssFailure;
    }

    /* Dequeue a buffer from the head of the queue */
    pthread_mutex_lock(&Q->mutex);
    if(Q->head)
    {
        BufInfo = (bufInfo_t *)Q->head;
        Q->head = BufInfo->next;
        if (!Q->head)
        {
            Q->tail = NULL;
        }

        /* Free the wrapper and return the data buffer pointer */
        *buf = BufInfo->buf;
        free(BufInfo);
        pthread_mutex_unlock(&Q->mutex);
        return ssSuccess;
    }

    *buf = NULL;
    pthread_mutex_unlock(&Q->mutex);
    return ssFailure;
}

SerStatus_t QsDeInitQueue(queue_t *Q)
{
    bufInfo_t *buf = NULL;

    if(!Q)
        return ssFailure;

    /* Delete all the items in given queue */
    pthread_mutex_lock(&Q->mutex);
    while(Q->head)
    {
        buf = (bufInfo_t *)Q->head;
        Q->head = buf->next;
        free(buf->buf);
        free(buf);
    }
    pthread_mutex_unlock(&Q->mutex);
    pthread_mutex_destroy(&Q->mutex);
    return ssSuccess;
}

static void QsProcessQapiResp(char *RespBuf)
{
    moduleCxt_t *cxt = NULL;
    uint8_t ModuleID = 0;
    uintptr_t uid = (uintptr_t)NULL;
    qsQapiResp_t *resp;

    if(!RespBuf)
        return;

    do
    {
        resp = (qsQapiResp_t *)(QSPACKET_HEADER(RespBuf));
        /* Retrieve module ID */
        ModuleID = READ_UNALIGNED_LITTLE_ENDIAN_UINT8(&resp->Header.ModuleID);
        cxt = QH_MODULE_INFO_CXT(GetCommonCxt(), ModuleID);
        if(!cxt)
            break;

        /* Retrieve UID. Free the packet if the request already timed out */
//        if(resp->Header.UID[4] || resp->Header.UID[5] || resp->Header.UID[6] || resp->Header.UID[7])
        if(sizeof(uintptr_t) > 4)
            uid = (uintptr_t)READ_UNALIGNED_LITTLE_ENDIAN_UINT64(resp->Header.UID);
        else
            uid = (uintptr_t)READ_UNALIGNED_LITTLE_ENDIAN_UINT32(resp->Header.UID);

        if(!uid || (QAPI_REQ_TIMED_OUT == ((Uid_t *)uid)->flag))
            break;

        /* Enqueue the received response in the module specific queue */
        if(ssSuccess != QsEnq(&cxt->queue, resp))
            break;

        /* Set the flag as response received, and signal the thread waiting it */
        pthread_mutex_lock(&cxt->mutex);
        ((Uid_t *)uid)->flag = QAPI_RESP_RCVD;
        pthread_cond_signal(&((Uid_t *)uid)->cv);
        pthread_mutex_unlock(&cxt->mutex);
        return;
    }while(0);

    /* Response error or request timed out. Free UID and response packet. */
    if(uid != (uintptr_t)NULL) {
        pthread_cond_destroy(&((Uid_t *)uid)->cv);
        free((void *)uid);
    }

    QsReleaseBuf(RespBuf);
    return;
}

static uint32_t QsDeqResp(uintptr_t UID, uint32_t ModuleID, char **RespInfo)
{
    moduleCxt_t *cxt;
    bufInfo_t *current, *prev = NULL;
    qsQapiResp_t *resp;
    uintptr_t respUID;
    queue_t *Q;

    if(!UID || !RespInfo)
        return ssFailure;

    cxt = QH_MODULE_INFO_CXT(GetCommonCxt(), ModuleID);
    if(!cxt)
        return ssFailure;

    Q = &cxt->queue;
    pthread_mutex_lock(&Q->mutex);
    current = (bufInfo_t *)(Q->head);
    while(current)
    {
        resp = (qsQapiResp_t *)(current->buf);

        /* Retrieve UID. Free the packet if the request already timed out */
//        if(resp->Header.UID[4] || resp->Header.UID[5] || resp->Header.UID[6] || resp->Header.UID[7])
        if(sizeof(uintptr_t) > 4)
            respUID = (uintptr_t)(READ_UNALIGNED_LITTLE_ENDIAN_UINT64(resp->Header.UID));
        else
            respUID = (uintptr_t)(READ_UNALIGNED_LITTLE_ENDIAN_UINT32(resp->Header.UID));

        if(UID == respUID)
        {
            /* UIDs for requeust and response match */
            if(current == (bufInfo_t *)Q->head)
            {
                Q->head = current->next;
                if (!Q->head)
                    Q->tail = NULL;
            }
            else if(prev)
                prev->next = current->next; /* Remove current from the  queue */

            *RespInfo = current->buf;
            free(current);
            pthread_mutex_unlock(&Q->mutex);
            return ssSuccess;
        }
        else
        {
            /* UIDs for requeust and response don't match, check the next response */
            prev = current;
            current = current->next;
        }
    }

    pthread_mutex_unlock(&cxt->queue.mutex);
    return ssFailure;
}

static void QsProcessQapiEvent(char *EventBuf)
{
    moduleCxt_t *cxt;
    uint8_t module_Id = 0, file_Id = 0, dev_Id = 0;
    uint16_t function_Id = 0;
    uint32_t size = 0, cb_Id = 0, payload_Length = 0;
    void *app_Cxt = NULL, *payload = NULL;
    char *event;

    if(!EventBuf)
        return;

    /* Skip the HTC+MML header */
    event = QSPACKET_HEADER(EventBuf);
    do {
        if(-1 == ReadEventBuffer(event, &module_Id, &file_Id, &function_Id, &size))
            break;

        cxt = QH_MODULE_INFO_CXT(GetCommonCxt(), module_Id);
        if(!cxt)
            break;

        switch(module_Id)
        {
            case QS_MODULE_WLAN:
                if(cxt->event_Cb.wlanCb &&
                        (0 == ReadWlanEventBuffer(QSEVENT_DATA(event), size,
                                                  &dev_Id, &cb_Id, &app_Cxt,
                                                  &payload, &payload_Length)))
                {
                    cxt->event_Cb.wlanCb(dev_Id, cb_Id, app_Cxt, payload,
                                         payload_Length);
                }
                break;

                /* Intentional fall-through, all these modules use the same  */
                /* callback.                                                 */
            case QS_MODULE_BLE:
            case QS_MODULE_HMI:
            case QS_MODULE_THREAD:
            case QS_MODULE_ZIGBEE:
            case QS_MODULE_HERH:
            case QS_MODULE_COEX:
            case QS_MODULE_FWUPGRADE:
            case QS_MODULE_TLMM:
                cxt->event_Cb.wpanCb(module_Id, file_Id, function_Id, size, QSEVENT_DATA(event));
                break;

            default:
                break;
        }
    } while(0);

    QsReleaseBuf(EventBuf);
    return;
}


static uint32_t QsProcessServerEvent(void *cxt, uint32_t event)
{
    qsCommonCxt_t *commonCxt = (qsCommonCxt_t *)cxt;

    if(!commonCxt)
    {
        return ssFailure;
    }
    if( commonCxt->eventCxt == NULL)
    {
            return ssSuccess;
    }

    if( commonCxt->eventCxt->cb != NULL )
    {
        commonCxt->eventCxt->cb(event, commonCxt->eventCxt->parameter);
    }
    return ssSuccess;
}

static void QsProcessControl(qsCommonCxt_t *cxt, char *packet)
{
    uint8_t type = 0;
    ipcCxt_t *ipcCxt = cxt->ipcCxt;

    type = packet[QS_CONTROL_MSG_TYPE_OFFSET];
    switch(type)
    {
        case QS_HELLO_RESP:
            printf("Received HELLO response from the server.\n");
            pthread_mutex_lock(&ipcCxt->mutex);
            ipcCxt->initialized = 1;
            pthread_cond_signal(&ipcCxt->cv);
            pthread_mutex_unlock(&ipcCxt->mutex);
            break;

        case QS_SERVER_EXIT:
            QsProcessServerEvent(cxt, QS_EVENT_SERVER_EXIT);
            break;

        case QS_TARGET_ASSERT:
            QsProcessServerEvent(cxt, QS_EVENT_TARGET_ASSERT);
            break;

        default:
            break;
    }

    QsReleaseBuf(packet);
    return;
}

static void QsProcessQapi(qsCommonCxt_t *cxt, char *packet)
{
    qsQapiPacketHeader_t *header = (qsQapiPacketHeader_t *)QSPACKET_HEADER(packet);
    uint8_t type = READ_UNALIGNED_LITTLE_ENDIAN_UINT8(&header->PacketType);
    switch(type)
    {
        case QS_RETURN_E:
            QsProcessQapiResp(packet);
            break;

        case QS_ASYNC_E:
            QsProcessQapiEvent(packet);
            break;

        default:
            printf("Unknown response\n");
            QsReleaseBuf(packet);
            break;
    }
    return;
}


static void BufpoolDeInit(void *cxt)
{
    bufPool_t *BufPool = NULL;
    qsCommonCxt_t *commonCxt = QH_COMMON_CXT(cxt);
    if(!commonCxt)
        return;

    BufPool = commonCxt->bufPool;
    if(!BufPool)
        return;

    QsDeInitQueue(&BufPool->queue);
    pthread_mutex_destroy(&BufPool->mutex);
    free(BufPool);
    commonCxt->bufPool = NULL;
    return;
}


static SerStatus_t BufpoolInit(void *cxt)
{
    char *Buf = NULL;
    bufPool_t *BufPool = NULL;
    uint32_t ret_Val = ssFailure;
    qsCommonCxt_t *commonCxt = QH_COMMON_CXT(cxt);
    uint8_t buf_Cnt = 0;

    /* Context is NULL or buffer pool is already initialized */
    if(!commonCxt || commonCxt->bufPool)
        return ssFailure;

    commonCxt->bufPool = (bufPool_t *)malloc(sizeof(bufPool_t));
    if(!commonCxt->bufPool)
        return ssFailure;


    BufPool = commonCxt->bufPool;
    memset(BufPool, 0, sizeof(bufPool_t));

    do {
        if(0 != pthread_mutex_init(&BufPool->mutex, NULL))
            break;

        if(ssSuccess != QsInitQueue(&BufPool->queue))
            break;

        for (buf_Cnt = 0; buf_Cnt < BUF_NUM_2048; buf_Cnt++)
        {
            Buf = (char *)malloc(BUF_SIZE_2048);
            if (!Buf)
                break;

            memset(Buf, 0, BUF_SIZE_2048);
            ret_Val = QsEnq(&BufPool->queue, Buf);
            if(ssSuccess != ret_Val) {
                free(Buf);
                break;
            }
            BufPool->count++;
        }

        if(ssSuccess != ret_Val)
            break;

        return ssSuccess;
    }while(0);

    BufpoolDeInit(commonCxt);
    return ssFailure;
}


SerStatus_t QsGetBuf(char **BufPtr, uint16_t Size)
{
    bufPool_t *BufPool = NULL;
    void *Buf = NULL;
    qsCommonCxt_t *commonCxt = GetCommonCxt();

    if (!BufPtr || !commonCxt || !commonCxt->bufPool)
        return ssInvalidParameter;

    *BufPtr = NULL;
    BufPool = commonCxt->bufPool;

    do {
        if((ssSuccess == QsDeq(&BufPool->queue, &Buf)) && Buf)
        {
            pthread_mutex_lock(&BufPool->mutex);
            BufPool->count--;
            pthread_mutex_unlock(&BufPool->mutex);
            *BufPtr = Buf;
            return ssSuccess;
        }
        /* If buffer pool is empty, sleep for pre-determined time and check again */
        usleep(QS_BUFPOOL_WAIT_TIME * 20); /* 100 microseconds */
        printf("Bufpool empty\n");
    } while(1);
}


static SerStatus_t QsReleaseBuf(char *Buf)
{
    bufPool_t *BufPool = NULL;
    qsCommonCxt_t *commonCxt = GetCommonCxt();

    if (!Buf || !commonCxt || !commonCxt->bufPool)
        return ssInvalidParameter;

    BufPool = commonCxt->bufPool;
    if(!BufPool)
    {
        free(Buf);
        return ssSuccess;
    }

    /* Replenish the global free list if it is running low on buffers */
    pthread_mutex_lock(&BufPool->mutex);
    if(ssSuccess == QsEnq(&BufPool->queue, Buf))
    {
        BufPool->count++;
        pthread_mutex_unlock(&BufPool->mutex);
        return ssSuccess;
    }

    /* Enqueue failed, free the buffer */
    pthread_mutex_unlock(&BufPool->mutex);
    free(Buf);
    return ssSuccess;
}

static uint32_t IpcSend(char *Req, uint16_t Len)
{
    uint32_t ret_Val = ssSuccess;
    uint32_t size = 0;
    qsCommonCxt_t *commonCxt = GetCommonCxt();

    if(!commonCxt || !commonCxt->ipcCxt)
    {
        return ssInvalidParameter;
    }

    /* Send message to server */
    size = mq_send(commonCxt->ipcCxt->qdServer, (const char *)Req, Len, 0);
    if (-1 == size) {
        perror ("Client: Not able to send message to server");
        return ssFailure;
    }
    ret_Val = ssSuccess;
    return ret_Val;
}

static void IpcDeInit(void *cxt)
{
    qsCommonCxt_t *commonCxt = (qsCommonCxt_t *)cxt;
    ipcCxt_t *ipcCxt = NULL;

    if(commonCxt && commonCxt->ipcCxt)
    {
        ipcCxt = commonCxt->ipcCxt;
        mq_close(ipcCxt->qdServer);

        if(-1 == mq_close(ipcCxt->qdClient))
        {
            perror ("Client: mq_close");
            return;
        }

        if(-1 == mq_unlink(ipcCxt->clientName))
        {
            perror ("Client: mq_unlink");
            return;
        }
        ipcCxt->qdClient = -1;
        pthread_cancel(ipcCxt->rxThread);
        pthread_mutex_destroy(&ipcCxt->mutex);
        pthread_cond_destroy(&ipcCxt->cv);
        free(ipcCxt);
        commonCxt->ipcCxt = NULL;
    }
    return;
}


void handle_signal(int sig)
{
    qsCommonCxt_t *commonCxt = GetCommonCxt();
    char exit_msg[64] = {'\0'};
    int sz = 0;
    /*Set Control message flag*/
    QS_SET_CONTROL_BIT(exit_msg);

    exit_msg[QS_CONTROL_MSG_TYPE_OFFSET] = QS_CLIENT_EXIT;
    WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(&exit_msg[QS_IOTD_COOKIE_OFFSET], getpid());

    sz = mq_send(commonCxt->ipcCxt->qdServer, (const char *)exit_msg, QS_CONTROL_MSG_TYPE_OFFSET + 1, 0);
    if (-1 == sz) {
        perror ("Client: Exit error\n");
        return;
    }
    QsDeInit();
    exit(0);
}



static uint32_t QsWaitForResp(void *UID, uint32_t ModuleID, char **Resp)
{
    uint32_t ret_Val = ssSuccess;
    moduleCxt_t *cxt = NULL;
    struct timespec time_to_wait = {0, 0};
    Uid_t *uid = (Uid_t *)UID;

    cxt = QH_MODULE_INFO_CXT(GetCommonCxt(), ModuleID);
    if(!cxt || !Resp)
    {
        pthread_cond_destroy(&uid->cv);
        free(uid);
        return ssFailure;
    }

    pthread_mutex_lock(&cxt->mutex);
    while(QAPI_RESP_RCVD != uid->flag)
    {
        /* wait for QAPI response from target */
        time_to_wait.tv_sec = time(NULL) + QAPI_RESP_WAIT_TIME;
        if(0 != pthread_cond_timedwait(&uid->cv, &cxt->mutex, &time_to_wait))
        {
            /* Do no free uid here, instead set the flag to inform receive path that the request timed out. */
            uid->flag = QAPI_REQ_TIMED_OUT;
            pthread_mutex_unlock(&cxt->mutex);
            *Resp = NULL;
            return ssFailure;
        }
    }

    ret_Val = QsDeqResp((uintptr_t)UID, ModuleID, Resp);
    pthread_cond_destroy(&uid->cv);
    free(uid);
    pthread_mutex_unlock(&cxt->mutex);
    return ret_Val;
}

/* Initialize Rx queue */
static uint32_t IpcInit(void *cxt, char *serverName, uint16_t maxMsgSize, uint16_t maxMsgCnt)
{
    qsCommonCxt_t *commonCxt = (qsCommonCxt_t *)cxt;
    ipcCxt_t *ipcCxt = NULL;
    struct mq_attr attr;

    /* Allocate IPC context */
    commonCxt->ipcCxt = (ipcCxt_t *)malloc(sizeof(ipcCxt_t));
    if(!commonCxt->ipcCxt)
    {
        return ssFailure;
    }
    memset(commonCxt->ipcCxt, 0, sizeof(ipcCxt_t));
    ipcCxt = commonCxt->ipcCxt;

    do
    {
        if (0 != pthread_mutex_init(&ipcCxt->mutex, NULL))
        {
            break;
        }
        if (0 != pthread_cond_init(&ipcCxt->cv, NULL))
        {
            break;
        }

        /* Open server queue */
        ipcCxt->qdServer = mq_open((const char *)serverName, O_WRONLY);
        if (-1 == ipcCxt->qdServer)
        {
            break;
        }

        /* Initialize receive queue attributes */
        attr.mq_flags = 0;
        attr.mq_maxmsg = maxMsgCnt;
        attr.mq_msgsize = maxMsgSize;
        attr.mq_curmsgs = 0;

        /* Configure and open client queue.
         * Send HELLO messages to the server and wait for response */
        sprintf(ipcCxt->clientName, "/client-%d", getpid());

        ipcCxt->qdClient = mq_open((const char *)ipcCxt->clientName,
                                   O_RDONLY | O_CREAT, QUEUE_PERMISSIONS,
                                   &attr);
        if (-1 == ipcCxt->qdClient)
            break;

        if(0 != pthread_create(&ipcCxt->rxThread, NULL, QsRxThread, cxt))
        {
            break;
        }

        pthread_mutex_lock(&ipcCxt->mutex);
        while(!ipcCxt->initialized)
        {
            pthread_cond_wait(&ipcCxt->cv, &ipcCxt->mutex);
        }
        pthread_mutex_unlock(&ipcCxt->mutex);

        return ssSuccess;
    } while(0);
    return ssFailure;
}

static uint32_t ModuleInit(void *cxt)
{
    qsCommonCxt_t *commonCxt = (qsCommonCxt_t *)cxt;
    moduleCxt_t *moduleCxt = NULL;
    uint8_t ModuleID = 0;

    for(ModuleID = 0; ModuleID < QS_MODULE_MAX; ModuleID++)
    {
        commonCxt->cxts[ModuleID] = (moduleCxt_t *)malloc(sizeof(moduleCxt_t));
        moduleCxt = QH_MODULE_INFO_CXT(commonCxt, ModuleID);
        if(!moduleCxt)
        {
            goto Error;
        }

        memset(moduleCxt, 0, sizeof(moduleCxt_t));
        QsInitQueue(&moduleCxt->queue);

        if (0 != pthread_mutex_init(&moduleCxt->mutex, NULL))
        {
            goto Error;
        }
        if (0 != pthread_cond_init(&moduleCxt->event, NULL))
        {
            goto Error;
        }

        pthread_mutex_lock(&moduleCxt->mutex);
        switch(ModuleID)
        {
            case QS_MODULE_WLAN:
                moduleCxt->mask = QS_WLAN_SIG;
                break;
            case QS_MODULE_BLE:
                moduleCxt->mask = QS_BLE_SIG;
                break;
            case QS_MODULE_HMI:
                moduleCxt->mask = QS_HMI_SIG;
                break;
            case QS_MODULE_THREAD:
                moduleCxt->mask = QS_TWN_SIG;
                break;
            case QS_MODULE_ZIGBEE:
                moduleCxt->mask = QS_ZB_SIG;
                break;
            case QS_MODULE_HERH:
                moduleCxt->mask = QS_HERH_SIG;
                break;
            case QS_MODULE_COEX:
                moduleCxt->mask = QS_COEX_SIG;
                break;
            case QS_MODULE_FWUPGRADE:
                moduleCxt->mask = QS_FWUPGRADE_SIG;
                break;
            case QS_MODULE_TLMM:
                moduleCxt->mask = QS_TLMM_SIG;
                break;

            default:
                pthread_mutex_unlock(&moduleCxt->mutex);
                goto Error;
        }
        pthread_mutex_unlock(&moduleCxt->mutex);
    }
    return ssSuccess;

Error:
    return ssFailure;
}

uint32_t QsInit(char *serverName, uint16_t maxMsgSize, uint16_t maxMsgCnt)
{
    qsCommonCxt_t *commonCxt = NULL;

    if(qsCommonCxt)
    {
        return ssFailure;
    }

    qsCommonCxt = (qsCommonCxt_t *)malloc(sizeof(qsCommonCxt_t));
    commonCxt = qsCommonCxt;
    if(!commonCxt)
    {
        return ssFailure;
    }

    memset(commonCxt, 0, (sizeof(qsCommonCxt_t)));

    do
    {
        if(ssSuccess != BufpoolInit(commonCxt))
        {
            break;
        }

        if(ssSuccess != ModuleInit(commonCxt))
        {
            break;
        }

        if(ssSuccess != IpcInit(commonCxt, serverName, maxMsgSize, maxMsgCnt))
        {
            break;
        }

        signal(SIGINT, handle_signal);
        signal(SIGHUP, handle_signal);

        return ssSuccess;
    } while(0);

    QsDeInit();
    return ssFailure;
}


void QsDeInit()
{
    moduleCxt_t *cxt = NULL;
    uint8_t ModuleID = 0;
    qsCommonCxt_t *commonCxt = GetCommonCxt();
    if(!commonCxt)
    {
        return;
    }

    for(ModuleID = 0; ModuleID < QS_MODULE_MAX; ModuleID++)
    {
        cxt = QH_MODULE_INFO_CXT(commonCxt, ModuleID);
        if(!cxt)
        {
            continue;
        }
        QsDeInitQueue(&cxt->queue);
        pthread_cond_destroy(&cxt->event);
        pthread_mutex_destroy(&cxt->mutex);
        free(cxt);
        commonCxt->cxts[ModuleID] = NULL;
    }
    if( commonCxt->eventCxt != NULL)
    {
        free(commonCxt->eventCxt);
        commonCxt->eventCxt = NULL;
    }
    BufpoolDeInit(commonCxt);
    IpcDeInit(commonCxt);
    return;
}

uint32_t QsRegisterCb(eventCb_t cb, void *parameter)
{
    qsCommonCxt_t *commonCxt = GetCommonCxt();

    if(!commonCxt)
    {
        return ssFailure;
    }

    if( commonCxt->eventCxt == NULL )
    {
        commonCxt->eventCxt = malloc(sizeof(eventCxt_t));
        if( commonCxt->eventCxt == NULL)
            return ssFailure;
    }

    commonCxt->eventCxt->cb = cb;
    commonCxt->eventCxt->parameter = parameter;
    return ssSuccess;
}

static Boolean_t MapQSModuleID(uint8_t ModuleID, uint8_t *QhModule)
{
    Boolean_t RetVal = TRUE;

    /* Map the module ID to the QH module type depending on the mux */
    /* configuration. */
    switch (ModuleID)
    {
        case MODULE_WLAN:
            *QhModule = QS_MODULE_WLAN;
            break;

        case MODULE_BLE:
#if QS_WPAN_BLE_MODULE_MAPPING == QS_WPAN_USE_BLE_THREAD
            *QhModule = QS_MODULE_BLE;
#elif QS_WPAN_BLE_MODULE_MAPPING == QS_WPAN_USE_HMI_THREAD
            *QhModule = QS_MODULE_HMI;
#elif QS_WPAN_BLE_MODULE_MAPPING == QS_WPAN_USE_TWN_THREAD
            *QhModule = QS_MODULE_THREAD;
#elif QS_WPAN_BLE_MODULE_MAPPING == QS_WPAN_USE_ZB_THREAD
            *QhModule = QS_MODULE_ZIGBEE;
#elif QS_WPAN_BLE_MODULE_MAPPING == QS_WPAN_USE_HERH_THREAD
            *QhModule = QS_MODULE_HERH;
#elif QS_WPAN_BLE_MODULE_MAPPING == QS_WPAN_USE_COEX_THREAD
            *QhModule = QS_MODULE_COEX;
#elif QS_WPAN_BLE_MODULE_MAPPING == QS_WPAN_USE_FWUPGRADE_THREAD
            *QhModule = QS_MODULE_FWUPGRADE;
#elif QS_WPAN_BLE_MODULE_MAPPING == QS_WPAN_USE_TLMM_THREAD
            *QhModule = QS_MODULE_TLMM;
#else
            RetVal = FALSE;
#endif
            break;

        case MODULE_HMI:
#if QS_WPAN_HMI_MODULE_MAPPING == QS_WPAN_USE_BLE_THREAD
            *QhModule = QS_MODULE_BLE;
#elif QS_WPAN_HMI_MODULE_MAPPING == QS_WPAN_USE_HMI_THREAD
            *QhModule = QS_MODULE_HMI;
#elif QS_WPAN_HMI_MODULE_MAPPING == QS_WPAN_USE_TWN_THREAD
            *QhModule = QS_MODULE_THREAD;
#elif QS_WPAN_HMI_MODULE_MAPPING == QS_WPAN_USE_ZB_THREAD
            *QhModule = QS_MODULE_ZIGBEE;
#elif QS_WPAN_HMI_MODULE_MAPPING == QS_WPAN_USE_HERH_THREAD
            *QhModule = QS_MODULE_HERH;
#elif QS_WPAN_HMI_MODULE_MAPPING == QS_WPAN_USE_COEX_THREAD
            *QhModule = QS_MODULE_COEX;
#elif QS_WPAN_HMI_MODULE_MAPPING == QS_WPAN_USE_FWUPGRADE_THREAD
            *QhModule = QS_MODULE_FWUPGRADE;
#elif QS_WPAN_HMI_MODULE_MAPPING == QS_WPAN_USE_TLMM_THREAD
            *QhModule = QS_MODULE_TLMM;
#else
            RetVal = FALSE;
#endif
            break;

        case MODULE_ZB:
#if QS_WPAN_ZB_MODULE_MAPPING == QS_WPAN_USE_BLE_THREAD
            *QhModule = QS_MODULE_BLE;
#elif QS_WPAN_ZB_MODULE_MAPPING == QS_WPAN_USE_HMI_THREAD
            *QhModule = QS_MODULE_HMI;
#elif QS_WPAN_ZB_MODULE_MAPPING == QS_WPAN_USE_TWN_THREAD
            *QhModule = QS_MODULE_THREAD;
#elif QS_WPAN_ZB_MODULE_MAPPING == QS_WPAN_USE_ZB_THREAD
            *QhModule = QS_MODULE_ZIGBEE;
#elif QS_WPAN_ZB_MODULE_MAPPING == QS_WPAN_USE_HERH_THREAD
            *QhModule = QS_MODULE_HERH;
#elif QS_WPAN_ZB_MODULE_MAPPING == QS_WPAN_USE_COEX_THREAD
            *QhModule = QS_MODULE_COEX;
#elif QS_WPAN_ZB_MODULE_MAPPING == QS_WPAN_USE_FWUPGRADE_THREAD
            *QhModule = QS_MODULE_FWUPGRADE;
#elif QS_WPAN_ZB_MODULE_MAPPING == QS_WPAN_USE_TLMM_THREAD
            *QhModule = QS_MODULE_TLMM;
#else
            RetVal = FALSE;
#endif
            break;
        case MODULE_TWN:
#if QS_WPAN_TWN_MODULE_MAPPING == QS_WPAN_USE_BLE_THREAD
            *QhModule = QS_MODULE_BLE;
#elif QS_WPAN_TWN_MODULE_MAPPING == QS_WPAN_USE_HMI_THREAD
            *QhModule = QS_MODULE_HMI;
#elif QS_WPAN_TWN_MODULE_MAPPING == QS_WPAN_USE_TWN_THREAD
            *QhModule = QS_MODULE_THREAD;
#elif QS_WPAN_TWN_MODULE_MAPPING == QS_WPAN_USE_ZB_THREAD
            *QhModule = QS_MODULE_ZIGBEE;
#elif QS_WPAN_TWN_MODULE_MAPPING == QS_WPAN_USE_HERH_THREAD
            *QhModule = QS_MODULE_HERH;
#elif QS_WPAN_TWN_MODULE_MAPPING == QS_WPAN_USE_COEX_THREAD
            *QhModule = QS_MODULE_COEX;
#elif QS_WPAN_TWN_MODULE_MAPPING == QS_WPAN_USE_FWUPGRADE_THREAD
            *QhModule = QS_MODULE_FWUPGRADE;
#elif QS_WPAN_TWN_MODULE_MAPPING == QS_WPAN_USE_TLMM_THREAD
            *QhModule = QS_MODULE_TLMM;
#else
            RetVal = FALSE;
#endif
            break;

        case MODULE_HERH:
#if QS_WPAN_HERH_MODULE_MAPPING == QS_WPAN_USE_BLE_THREAD
            *QhModule = QS_MODULE_BLE;
#elif QS_WPAN_HERH_MODULE_MAPPING == QS_WPAN_USE_HERH_THREAD
            *QhModule = QS_MODULE_HERH;
#elif QS_WPAN_HERH_MODULE_MAPPING == QS_WPAN_USE_TWN_THREAD
            *QhModule = QS_MODULE_THREAD;
#elif QS_WPAN_HERH_MODULE_MAPPING == QS_WPAN_USE_ZB_THREAD
            *QhModule = QS_MODULE_ZIGBEE;
#elif QS_WPAN_HERH_MODULE_MAPPING == QS_WPAN_USE_HERH_THREAD
            *QhModule = QS_MODULE_HERH;
#elif QS_WPAN_HERH_MODULE_MAPPING == QS_WPAN_USE_COEX_THREAD
            *QhModule = QS_MODULE_COEX;
#elif QS_WPAN_HERH_MODULE_MAPPING == QS_WPAN_USE_FWUPGRADE_THREAD
            *QhModule = QS_MODULE_FWUPGRADE;
#elif QS_WPAN_HERH_MODULE_MAPPING == QS_WPAN_USE_TLMM_THREAD
            *QhModule = QS_MODULE_TLMM;
#else
            RetVal = FALSE;
#endif
            break;
        case MODULE_COEX:
#if QS_WPAN_COEX_MODULE_MAPPING == QS_WPAN_USE_BLE_THREAD
            *QhModule = QS_MODULE_BLE;
#elif QS_WPAN_COEX_MODULE_MAPPING == QS_WPAN_USE_HMI_THREAD
            *QhModule = QS_MODULE_HMI;
#elif QS_WPAN_COEX_MODULE_MAPPING == QS_WPAN_USE_TWN_THREAD
            *QhModule = QS_MODULE_THREAD;
#elif QS_WPAN_COEX_MODULE_MAPPING == QS_WPAN_USE_ZB_THREAD
            *QhModule = QS_MODULE_ZIGBEE;
#elif QS_WPAN_COEX_MODULE_MAPPING == QS_WPAN_USE_HERH_THREAD
            *QhModule = QS_MODULE_HERH;
#elif QS_WPAN_COEX_MODULE_MAPPING == QS_WPAN_USE_COEX_THREAD
            *QhModule = QS_MODULE_COEX;
#elif QS_WPAN_COEX_MODULE_MAPPING == QS_WPAN_USE_FWUPGRADE_THREAD
            *QhModule = QS_MODULE_FWUPGRADE;
#elif QS_WPAN_COEX_MODULE_MAPPING == QS_WPAN_USE_TLMM_THREAD
            *QhModule = QS_MODULE_TLMM;
#else
            RetVal = FALSE;
#endif
            break;

        case MODULE_FIRMWARE_UPGRADE_EXT:
#if QS_WPAN_FWUPGRADE_MODULE_MAPPING == QS_WPAN_USE_BLE_THREAD
            *QhModule = QS_MODULE_BLE;
#elif QS_WPAN_FWUPGRADE_MODULE_MAPPING == QS_WPAN_USE_HMI_THREAD
            *QhModule = QS_MODULE_HMI;
#elif QS_WPAN_FWUPGRADE_MODULE_MAPPING == QS_WPAN_USE_TWN_THREAD
            *QhModule = QS_MODULE_THREAD;
#elif QS_WPAN_FWUPGRADE_MODULE_MAPPING == QS_WPAN_USE_ZB_THREAD
            *QhModule = QS_MODULE_ZIGBEE;
#elif QS_WPAN_FWUPGRADE_MODULE_MAPPING == QS_WPAN_USE_HERH_THREAD
            *QhModule = QS_MODULE_HERH;
#elif QS_WPAN_FWUPGRADE_MODULE_MAPPING == QS_WPAN_USE_COEX_THREAD
            *QhModule = QS_MODULE_COEX;
#elif QS_WPAN_FWUPGRADE_MODULE_MAPPING == QS_WPAN_USE_FWUPGRADE_THREAD
            *QhModule = QS_MODULE_FWUPGRADE;
#elif QS_WPAN_FWUPGRADE_MODULE_MAPPING == QS_WPAN_USE_TLMM_THREAD
            *QhModule = QS_MODULE_TLMM;
#else
            RetVal = FALSE;
#endif
            break;

        case MODULE_TLMM:
#if QS_WPAN_TLMM_MODULE_MAPPING == QS_WPAN_USE_BLE_THREAD
            *QhModule = QS_MODULE_BLE;
#elif QS_WPAN_TLMM_MODULE_MAPPING == QS_WPAN_USE_HMI_THREAD
            *QhModule = QS_MODULE_HMI;
#elif QS_WPAN_TLMM_MODULE_MAPPING == QS_WPAN_USE_TWN_THREAD
            *QhModule = QS_MODULE_THREAD;
#elif QS_WPAN_TLMM_MODULE_MAPPING == QS_WPAN_USE_ZB_THREAD
            *QhModule = QS_MODULE_ZIGBEE;
#elif QS_WPAN_TLMM_MODULE_MAPPING == QS_WPAN_USE_HERH_THREAD
            *QhModule = QS_MODULE_HERH;
#elif QS_WPAN_TLMM_MODULE_MAPPING == QS_WPAN_USE_COEX_THREAD
            *QhModule = QS_MODULE_COEX;
#elif QS_WPAN_TLMM_MODULE_MAPPING == QS_WPAN_USE_FWUPGRADE_THREAD
            *QhModule = QS_MODULE_FWUPGRADE;
#elif QS_WPAN_TLMM_MODULE_MAPPING == QS_WPAN_USE_TLMM_THREAD
            *QhModule = QS_MODULE_TLMM;
#else
            RetVal = FALSE;
#endif
            break;
        default:
            RetVal = FALSE;
            break;
    }

    return(RetVal);
}

static Boolean_t AllocateCommandPacket(uint8_t TargetID, uint8_t ModuleID, uint16_t FileID, uint16_t FunctionID, PackedBuffer_t *PackedBuffer, uint16_t Size)
{
    char *CommandHdr = NULL;
    qsQapiReq_t *Command = NULL;
    uint8_t     Module = 0;

    /* Find the mapped module ID */
    if(TRUE != MapQSModuleID(ModuleID, &Module))
        return FALSE;

    /* Get buffer of required size. */
    if((ssSuccess != QsGetBuf(&CommandHdr,
                                QS_TOTAL_LEN(QSCOMMAND_T_SIZE(Size)))) || \
                                !CommandHdr)
        return FALSE;

    PackedBuffer->Packet = (uint8_t *)CommandHdr;
    /* Skip HTC and MML headers and go to QAPI header */
    Command = (qsQapiReq_t *)(QSPACKET_HEADER(CommandHdr));

    /* Set the packed buffer parameters. */
    PackedBuffer->PacketType = QS_PACKET_E;
    PackedBuffer->Length = Size;
    PackedBuffer->Start = QSCOMMAND_DATA(Command);
    InitializePackedBuffer(PackedBuffer, PackedBuffer->Start, Size);

    /* Write the device ID */
    WRITE_UNALIGNED_LITTLE_ENDIAN_UINT8(MML_HEADER(CommandHdr), (TargetID << 4) & 0xF0);
    /*Write the Process ID (cookie) in the MML header)*/
    WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(QSPACKET_COOKIE(HTC_HEADER(CommandHdr)),
                                                          getpid());
    /* Write the QAPI command parameters. */
    WRITE_UNALIGNED_LITTLE_ENDIAN_UINT16(&Command->Header.TotalLength, QSCOMMAND_T_SIZE(Size));
    WRITE_UNALIGNED_LITTLE_ENDIAN_UINT8( &Command->Header.ModuleID,    Module);
    WRITE_UNALIGNED_LITTLE_ENDIAN_UINT8( &Command->Header.PacketType,  QS_PACKET_E);
    WRITE_UNALIGNED_LITTLE_ENDIAN_UINT16(&Command->Header.FileID,      FileID);
    WRITE_UNALIGNED_LITTLE_ENDIAN_UINT16(&Command->Header.FunctionID,  FunctionID);
    WRITE_UNALIGNED_LITTLE_ENDIAN_UINT16(&Command->Header.DataLength,  Size);

    return TRUE;
}

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
Boolean_t AllocatePackedBuffer(uint8_t TargetID, qsPacketType_t PacketType, uint8_t ModuleID, uint16_t FileID, uint16_t FunctionID, PackedBuffer_t *PackedBuffer, uint16_t Size)
{
    switch (PacketType)
    {
        case QS_PACKET_E:
            return AllocateCommandPacket(TargetID, ModuleID, FileID, FunctionID, PackedBuffer, Size);

        default:
            return FALSE;
    }
}

/**
  Frees a packed buffer.

  @param PackedBuffer Pointer to the packed buffer to be freed.
  */
void FreePackedBuffer(PackedBuffer_t *PackedBuffer)
{
    char *original_buf_ptr = NULL;

    if(!PackedBuffer->Packet)
        return;

    switch(PackedBuffer->PacketType)
    {
        case QS_PACKET_E:
            QsReleaseBuf((char *)PackedBuffer->Packet);
            PackedBuffer->Packet = NULL;
            break;

        default:
            /* Original buffer included QS_IOTD_HEADER_LEN bytes of HTC+MML headers */
            original_buf_ptr = ((char *)PackedBuffer->Packet) - QS_IOTD_HEADER_LEN;
            if(original_buf_ptr)
            {
                QsReleaseBuf(original_buf_ptr);
                PackedBuffer->Packet = NULL;
            }
            break;
    }
    return;
}

SerStatus_t _SendCommand(PackedBuffer_t *InputBuffer, PackedBuffer_t *OutputBuffer, uint8_t WaitForResp)
{
    uint16_t       TotalLen = 0;
    SerStatus_t    retVal = ssFailure;
    qsQapiReq_t    *Packet = NULL;
    qsQapiResp_t   *Resp = NULL;
    Uid_t          *uid = NULL;

    Packet = (qsQapiReq_t *)QSPACKET_HEADER(InputBuffer->Packet);

    /* Generate the unique ID which is the address of the conditional variable used to wait on for response.
     * No need to UID for QAPIs which do not require a response */
    if(WaitForResp)
    {
        uid = (Uid_t *)malloc(sizeof(Uid_t));
        if(!uid)
            return ssFailure;

        memset(uid, 0, sizeof(Uid_t));
        if(0 != pthread_cond_init(&uid->cv, NULL))
        {
            free(uid);
            return ssFailure;
        }

        memset(Packet->Header.UID, 0, 8);
        if(sizeof(uintptr_t) > 4) {
            WRITE_UNALIGNED_LITTLE_ENDIAN_UINT64(Packet->Header.UID,
                                                 (uintptr_t)uid);
        }
        else {
            WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(Packet->Header.UID,
                                                 (uintptr_t)uid);
        }
    }

    /* Transmit the packet and wait for the response. */
    TotalLen = QS_TOTAL_LEN(READ_UNALIGNED_LITTLE_ENDIAN_UINT16(&Packet->Header.TotalLength));
    /* Cleasr the control bit so that the packet will be sent to target */
    QS_RESET_CONTROL_BIT(InputBuffer->Packet);
    if(IpcSend((char *)InputBuffer->Packet, TotalLen) == ssSuccess)
    {
        /* Do not wait for response in case of some data QAPIs */
        if(!WaitForResp)
            return ssSuccess;

        if((QsWaitForResp((void *)uid, Packet->Header.ModuleID,
                          (char **)&Resp) == ssSuccess) || Resp)
        {
            /* Set the output buffer parameters. */
            OutputBuffer->PacketType = QS_RETURN_E;
            OutputBuffer->Packet     = (uint8_t *)Resp;

            /* Check the response values. */
            if(Resp->Header.DataLength > 0)
            {
                InitializePackedBuffer(OutputBuffer,
                                       QSRESPONSE_DATA(Resp),
                                       READ_UNALIGNED_LITTLE_ENDIAN_UINT16(&Resp->Header.DataLength));
            }
            else
            {
                InitializePackedBuffer(OutputBuffer,
                                       NULL,
                                       0);
            }

            retVal = ssSuccess;
        }
    }

    return retVal;
}

SerStatus_t SendCommand(PackedBuffer_t *InputBuffer, PackedBuffer_t *OutputBuffer)
{
    return _SendCommand(InputBuffer, OutputBuffer, TRUE);
}


#ifdef LATENCY_DEBUG
long start = 0, stop = 0;
uint8_t latency_test_started = 0;
long QsGetMicrotime()
{
    struct timespec tspec;
    clock_gettime(CLOCK_REALTIME, &tspec);
    return (tspec.tv_sec*(int)1e9 + tspec.tv_nsec);
}

void QsGetTotalTime(void *Arg)
{
    uint16_t size = 1000;
    char *buf = NULL;

    if(latency_test_started)
    {
        return;
    }

    buf = (char *)malloc(size);
    memset(buf, 0, size);
    WRITE_UNALIGNED_LITTLE_ENDIAN_UINT16(buf, size);

    start = getMicrotime();
    IpcSend(buf, size);
    ++latency_test_started;
}
#endif /* LATENCY_DEBUG */
#endif /* CONFIG_DAEMON_MODE */

