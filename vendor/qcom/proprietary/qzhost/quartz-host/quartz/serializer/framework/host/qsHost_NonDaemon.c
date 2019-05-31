/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef CONFIG_DAEMON_MODE

#include <stdlib.h>
#include <string.h>
#include "qsCommon.h"
#include "qsHost.h"
#include "qsHostUart.h"
#include "qsOSAL.h"
#include "qsConfig.h"
#include "qsPack.h"
#include "idlist.h"

/* This structure holds state for each module */
static qsHostModuleCxt_t *ModuleCxt[QS_MODULE_MAX];
pthread_mutex_t ModuleArraymutex = PTHREAD_MUTEX_INITIALIZER;
static BufPool_t *BufPool[BUF_NUM_LISTS] = { NULL, NULL, NULL, NULL, NULL };


int32_t ReadWlanEventBuffer(void *Buffer, uint16_t qsLength, uint8_t *DevID, uint32_t *CbID, void **AppCxt, void **payload, uint32_t *payload_Length)
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


int32_t ReadEventBuffer(void *Buffer, uint8_t *ModuleID, uint8_t *FileID, uint16_t *FunctionID, uint32_t *Size)
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
    qsHostModuleCxt_t *cxt = NULL;
    uint32_t ret_Val = ssFailure;

    if ((QS_MODULE_MAX <= ModuleID) || !ModuleCxt[ModuleID])
    {
        return ssFailure;
    }

    pthread_mutex_lock(&ModuleArraymutex);
    cxt = ModuleCxt[ModuleID];
    pthread_mutex_lock(&cxt->mutex);
    pthread_mutex_unlock(&ModuleArraymutex);
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
            ret_Val = ssSuccess;
            cxt->event_Cb.wpanCb = (WpanCb_t)Callback;
            break;
        default:
            ret_Val = ssFailure;
    }
    pthread_mutex_unlock(&cxt->mutex);
    return ret_Val;
}

uint32_t QsInitQueue(hostQueue_t *Q)
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

SerStatus_t QsEnq(hostQueue_t *Q, BufInfo_t *Buf)
{
    if(!Q || !Buf)
    {
        return ssFailure;
    }

    /* Enqueue the buffer at the end of the queue */
    pthread_mutex_lock(&Q->mutex);
    if(!Q->head && !Q->tail)
    {
        Q->head = (void *)Buf;
        Q->tail = (void *)Buf;
    }
    else
    {
        ((BufInfo_t *)Q->tail)->next = Buf;
        Q->tail = (void *)Buf;
    }
    pthread_mutex_unlock(&Q->mutex);
    return ssSuccess;
}

SerStatus_t QsDeq(hostQueue_t *Q, BufInfo_t **BufPtr)
{
    BufInfo_t *buf = NULL;
    uint32_t ret_Val = ssFailure;
    if(!Q || !BufPtr)
    {
        return ssFailure;
    }

    /* Dequeue a buffer from the head of the queue */
    pthread_mutex_lock(&Q->mutex);
    if(Q->head)
    {
        buf = (BufInfo_t *)Q->head;
        Q->head = buf->next;
        buf->next = NULL;
        if (!Q->head)
        {
            Q->tail = NULL;
        }
        *BufPtr = buf;
        ret_Val = ssSuccess;
    }
    pthread_mutex_unlock(&Q->mutex);
    return ret_Val;
}

uint32_t QsDeInitQueue(hostQueue_t *Q)
{
    BufInfo_t *buf = NULL;

    pthread_mutex_lock(&Q->mutex);
    while(Q->head)
    {
        buf = (BufInfo_t *)Q->head;
        Q->head = buf->next;
        QsReleaseBuf(buf);
    }

    Q->tail = NULL;
    pthread_mutex_unlock(&Q->mutex);
    pthread_mutex_destroy(&Q->mutex);
    return ssSuccess;
}

uint32_t QsEnqResp(BufInfo_t *RespInfo)
{
    qsHostModuleCxt_t *cxt = NULL;
    uint8_t ModuleID = 0;
    pthread_cond_t *cv = NULL;
    qsQapiResp_t *resp = NULL;

    if(!RespInfo || !RespInfo->buf)
    {
        return ssFailure;
    }
    resp = (qsQapiResp_t *)RespInfo->buf;

    ModuleID = READ_UNALIGNED_LITTLE_ENDIAN_UINT8(&resp->Header.ModuleID);
    if ((QS_MODULE_MAX <= ModuleID) || !ModuleCxt[ModuleID])
    {
        return ssFailure;
    }

    if(resp->Header.UID[4] || resp->Header.UID[5] || resp->Header.UID[6] || resp->Header.UID[7])
    {
        cv = (pthread_cond_t *)((intptr_t)READ_UNALIGNED_LITTLE_ENDIAN_UINT64(resp->Header.UID));
    }
    else
    {
        cv = (pthread_cond_t *)((intptr_t)READ_UNALIGNED_LITTLE_ENDIAN_UINT32(resp->Header.UID));
    }

    RespInfo->u_Id = (void *)cv;
    RespInfo->next = NULL;

    cxt = ModuleCxt[ModuleID];
    if(ssSuccess == QsEnq(&cxt->queue, RespInfo))
    {
        /* Signal the worker thread and release the mutex */
        pthread_cond_signal(cv);
        return ssSuccess;
    }
    return ssFailure;
}

static uint32_t QsDeqResp(void *UID, uint32_t ModuleID, BufInfo_t **RespInfo)
{
    qsHostModuleCxt_t *cxt = NULL;
    BufInfo_t *current = NULL, *prev = NULL;

    if ((QS_MODULE_MAX <= ModuleID) || !ModuleCxt[ModuleID] || !RespInfo)
    {
        return ssFailure;
    }

    *RespInfo = NULL;
    cxt = ModuleCxt[ModuleID];
    pthread_mutex_lock(&cxt->queue.mutex);
    current = (BufInfo_t *)cxt->queue.head;
    while(current)
    {
        if(current->u_Id == UID)
        {
            if(current == (BufInfo_t *)cxt->queue.head)
            {
                cxt->queue.head = current->next;
                if (!cxt->queue.head)
                {
                    cxt->queue.tail = NULL;
                }
            }
            else if(prev)
            {
                prev->next = current->next;
            }
            current->next = NULL;
            *RespInfo = current;
            pthread_mutex_unlock(&cxt->queue.mutex);
            return ssSuccess;
        }
        else
        {
            prev = current;
            current = current->next;
        }
    }

    pthread_mutex_unlock(&cxt->queue.mutex);
    return ssFailure;
}

static void FreeModuleQueue(uint32_t ModuleID)
{
    BufInfo_t *current = NULL;
    qsHostModuleCxt_t *cxt = ModuleCxt[ModuleID];

    pthread_mutex_lock(&cxt->queue.mutex);
    while(cxt->queue.head)
    {
        current = (BufInfo_t *)cxt->queue.head;
        cxt->queue.head = (void *)current->next;
        QsReleaseBuf(current);
    }

    cxt->queue.tail = NULL;
    pthread_mutex_unlock(&cxt->queue.mutex);
    return;
}

uint32_t QsProcessEvent(BufInfo_t *EventInfo)
{
    char *event = NULL;
    qsHostModuleCxt_t *cxt = NULL;
    uint8_t module_Id = 0, file_Id = 0;
    uint16_t function_Id = 0;
    uint32_t size = 0;
    uint8_t dev_Id = 0;
    uint32_t cb_Id = 0, payload_Length = 0;
    void *app_Cxt = NULL, *payload = NULL;

    if(!EventInfo)
    {
        return ssFailure;
    }

    event = EventInfo->buf;
    if(-1 == ReadEventBuffer(event, &module_Id, &file_Id, &function_Id, &size))
    {
        return ssFailure;
    }

    if ((QS_MODULE_MAX <= module_Id) || !ModuleCxt[module_Id])
    {
        return ssFailure;
    }

    pthread_mutex_lock(&ModuleArraymutex);
    cxt = ModuleCxt[module_Id];
    pthread_mutex_unlock(&ModuleArraymutex);

    switch(module_Id)
    {
        case QS_MODULE_WLAN:
            if(cxt->event_Cb.wlanCb &&
                    (0 == ReadWlanEventBuffer(QSEVENT_DATA(event), size, &dev_Id, &cb_Id,
                                              &app_Cxt, &payload, &payload_Length)))
            {
                cxt->event_Cb.wlanCb(dev_Id, cb_Id, app_Cxt, payload, payload_Length);
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
            cxt->event_Cb.wpanCb(module_Id, file_Id, function_Id, size, QSEVENT_DATA(event));
            break;
        default:
            return ssFailure;
    }

    return ssSuccess;
}

static SerStatus_t BufpoolInit()
{
    BufInfo_t *BufInfo = NULL;
    int index = 0;
    uint16_t Size = 0;
    uint32_t ret_Val = ssSuccess;
    int buf_Cnt = 0;

    /* Buffer pools are already initialized */
    if(BufPool[0])
    {
        return ssSuccess;
    }

    for(index = 0; index < BUF_NUM_LISTS; index++)
    {
        BufPool[index] = (BufPool_t *)malloc(sizeof(BufPool_t));
        if(!BufPool[index])
        {
            ret_Val = ssFailure;
            break;
        }
        memset(BufPool[index], 0, sizeof(BufPool_t));

        BufPool[index]->count = 0;
        if(0 != pthread_mutex_init(&BufPool[index]->mutex, NULL))
        {
            ret_Val = ssFailure;
            break;
        }

        switch(index)
        {
            case BUF_INDEX_128:
                BufPool[index]->threshold = BUF_NUM_128;
                Size = BUF_SIZE_128;
                break;

            case BUF_INDEX_256:
                BufPool[index]->threshold = BUF_NUM_256;
                Size = BUF_SIZE_256;
                break;

            case BUF_INDEX_512:
                BufPool[index]->threshold = BUF_NUM_512;
                Size = BUF_SIZE_512;
                break;

            case BUF_INDEX_1024:
                BufPool[index]->threshold = BUF_NUM_1024;
                Size = BUF_SIZE_1024;
                break;

            case BUF_INDEX_2048:
                BufPool[index]->threshold = BUF_NUM_2048;
                Size = BUF_SIZE_2048;
                break;
        }

        if(ssSuccess != QsInitQueue(&BufPool[index]->queue))
        {
            ret_Val = ssFailure;
            break;
        }

        for (buf_Cnt = 0; buf_Cnt < BufPool[index]->threshold; buf_Cnt++)
        {
            BufInfo = (BufInfo_t *)malloc(sizeof(BufInfo_t));
            if (!BufInfo)
            {
                ret_Val = ssFailure;
                break;
            }

            BufInfo->size = Size;
            BufInfo->buf = (char *)malloc(Size);
            if (!BufInfo->buf)
            {
                free(BufInfo);
                ret_Val = ssFailure;
                break;
            }
            memset(BufInfo->buf, 0, Size);
            BufInfo->next = BufPool[index]->queue.head;
            BufInfo->u_Id = NULL;
            BufPool[index]->queue.head = (void *)BufInfo;
            BufPool[index]->count++;
        }

        if(ssSuccess != ret_Val)
        {
            break;
        }

        ret_Val = ssSuccess;
    }

    return ret_Val;
}

static void BufpoolDeInit()
{
    BufInfo_t *BufInfo = NULL;
    int index = 0;

    for(index = 0; index < BUF_NUM_LISTS; index++)
    {
        if(!BufPool[index])
        {
            continue;
        }

        pthread_mutex_lock(&BufPool[index]->mutex);
        while(BufPool[index]->queue.head)
        {
            BufInfo = (BufInfo_t *)BufPool[index]->queue.head;
            BufPool[index]->queue.head = (void *)BufInfo->next;
            free(BufInfo->buf);
            free(BufInfo);
        }
        pthread_mutex_unlock(&BufPool[index]->mutex);
        pthread_mutex_destroy(&BufPool[index]->mutex);
        BufPool[index] = NULL;
    }
    return;
}

uint32_t QsGetBuf(BufInfo_t **BufInfoPtr, uint16_t Size)
{
    BufInfo_t *BufInfo = NULL;
    BufPool_t *bufPool = NULL;

    if (!BufInfoPtr)
    {
        return ssInvalidParameter;
    }

    if(Size <= BUF_SIZE_128)
    {
        bufPool = BufPool[BUF_INDEX_128];
        Size = BUF_SIZE_128;
    }
    else if(Size <= BUF_SIZE_256)
    {
        bufPool = BufPool[BUF_INDEX_256];
        Size = BUF_SIZE_256;
    }
    else if(Size <= BUF_SIZE_512)
    {
        bufPool = BufPool[BUF_INDEX_512];
        Size = BUF_SIZE_512;
    }
    else if(Size <= BUF_SIZE_1024)
    {
        bufPool = BufPool[BUF_INDEX_1024];
        Size = BUF_SIZE_1024;
    }
    else if(Size <= BUF_SIZE_2048)
    {
        bufPool = BufPool[BUF_INDEX_2048];
        Size = BUF_SIZE_2048;
    }
    else
    {
        return ssInvalidParameter;
    }

    if(bufPool)
    {
        if(ssSuccess == QsDeq(&bufPool->queue, BufInfoPtr))
        {
            bufPool->count--;
            return ssSuccess;
        }
    }

    /* free_buf_list is empty or not initialzed */
    BufInfo = (BufInfo_t *)malloc(sizeof(BufInfo_t));
    if (!BufInfo)
    {
        return ssFailure;
    }

    memset(BufInfo, 0, sizeof(BufInfo_t));
    BufInfo->buf = (char *)malloc(Size);
    if (!BufInfo->buf)
    {
        free(BufInfo);
        return ssFailure;
    }
    memset(BufInfo->buf, 0, Size);
    BufInfo->next = NULL;
    *BufInfoPtr = BufInfo;

    return ssSuccess;
}


uint32_t QsReleaseBuf(BufInfo_t *BufInfo)
{
    BufPool_t *bufPool = NULL;

    if(!BufInfo)
    {
        return ssInvalidParameter;
    }

    switch(BufInfo->size)
    {
        case BUF_SIZE_128:
            bufPool = BufPool[BUF_INDEX_128];
            break;

        case BUF_SIZE_256:
            bufPool = BufPool[BUF_INDEX_256];
            break;

        case BUF_SIZE_512:
            bufPool = BufPool[BUF_INDEX_512];
            break;

        case BUF_SIZE_1024:
            bufPool = BufPool[BUF_INDEX_1024];
            break;

        case BUF_SIZE_2048:
            bufPool = BufPool[BUF_INDEX_2048];
            break;

        default:
            bufPool = NULL;
            break;
    }

    memset(BufInfo->buf, 0, BufInfo->size);
    if(bufPool)
    {
        pthread_mutex_lock(&bufPool->mutex);
        if(bufPool->count < bufPool->threshold)
        {
            /* Replenish the global free list if it is running low on buffers */
            BufInfo->next = bufPool->queue.head;
            bufPool->queue.head = (void *)BufInfo;
            bufPool->count++;
            pthread_mutex_unlock(&bufPool->mutex);
            return ssSuccess;
        }
        pthread_mutex_unlock(&bufPool->mutex);
    }

    /* If global list has enough buffers, free the buffer. */
    BufInfo->next = NULL;
    free(BufInfo->buf);
    free(BufInfo);
    return ssSuccess;

}

/* Common transport layer initialization */
static uint32_t QsTransportInit(uint32_t TransportID, char *IfName, int32_t *Handle)
{
    uint32_t ret_Val = ssSuccess;

    switch(TransportID)
    {
        case QS_UART_E:
            ret_Val = QsUartInit(IfName, Handle);
            break;

        default:
            break;

    }

    return ret_Val;
}


static uint32_t QsTransportDeInit(uint32_t TransportID, uint32_t Handle)
{
    uint32_t ret_Val = ssSuccess;

    switch(TransportID)
    {
        case QS_UART_E:
            ret_Val = QsUartDeInit(Handle);
            break;

        default:
            return ssFailure;

    }
    return ret_Val;
}

static uint32_t Transmit(char *Req, uint16_t Len)
{
    uint32_t ret_Val = ssSuccess;
    qsHostModuleCxt_t *cxt = NULL;
    uint8_t ModuleID = 0;
    qsQapiReq_t *command = (qsQapiReq_t *)QSPACKET_HEADER(Req);

    ModuleID = READ_UNALIGNED_LITTLE_ENDIAN_UINT8(&command->Header.ModuleID);
    pthread_mutex_lock(&ModuleArraymutex);
    cxt = ModuleCxt[ModuleID];
    pthread_mutex_unlock(&ModuleArraymutex);
    switch(cxt->transport)
    {
        case QS_UART_E:
            ret_Val = QsUartTransmit(cxt->handle, Len, Req);
            break;

        default:
            ret_Val = ssFailure;
            break;

    }
    return ret_Val;
}


static uint32_t QsWaitForResp(void *UID, uint32_t ModuleID, BufInfo_t **RespInfo)
{
    uint32_t ret_Val = ssSuccess;
    qsHostModuleCxt_t *cxt = NULL;

    if((QS_MODULE_MAX <= ModuleID) || !RespInfo || !ModuleCxt[ModuleID])
    {
        return ssFailure;
    }

    pthread_mutex_lock(&ModuleArraymutex);
    cxt = ModuleCxt[ModuleID];
    pthread_mutex_lock(&cxt->mutex);
    pthread_mutex_unlock(&ModuleArraymutex);

    pthread_cond_wait((pthread_cond_t *)UID, &cxt->mutex);
    ret_Val = QsDeqResp(UID, ModuleID, RespInfo);
    pthread_mutex_unlock(&cxt->mutex);
    return ret_Val;
}

uint32_t QsInit(uint8_t ModuleID, uint32_t TransportID, char *IfName)
{
    qsHostModuleCxt_t *cxt = NULL;

    if ((QS_MODULE_MAX <= ModuleID) || ModuleCxt[ModuleID])
    {
        return ssFailure;
    }

    if(!BufPool[0])
    {
        if(ssSuccess != BufpoolInit())
        {
            BufpoolDeInit();
            return ssFailure;
        }
    }

    pthread_mutex_lock(&ModuleArraymutex);
    ModuleCxt[ModuleID] = (qsHostModuleCxt_t *)malloc(sizeof(qsHostModuleCxt_t));
    if(!ModuleCxt[ModuleID])
    {
        pthread_mutex_unlock(&ModuleArraymutex);
        return ssFailure;
    }

    cxt = ModuleCxt[ModuleID];
    memset(cxt, 0, sizeof(qsHostModuleCxt_t));
    cxt->transport = QS_TRANSPORT_MAX_E;
    cxt->handle = -1;
    QsInitQueue(&cxt->queue);

    if (0 != pthread_mutex_init(&cxt->mutex, NULL))
    {
        free(cxt);
        ModuleCxt[ModuleID] = NULL;
        pthread_mutex_unlock(&ModuleArraymutex);
        return ssFailure;
    }
    if (0 != pthread_cond_init(&cxt->event, NULL))
    {
        pthread_mutex_destroy(&cxt->mutex);
        free(cxt);
        ModuleCxt[ModuleID] = NULL;
        pthread_mutex_unlock(&ModuleArraymutex);
        return ssFailure;
    }

    pthread_mutex_lock(&cxt->mutex);
    switch(ModuleID)
    {
        case QS_MODULE_WLAN:
            cxt->mask = QS_WLAN_SIG;
            break;
        case QS_MODULE_BLE:
            cxt->mask = QS_BLE_SIG;
            break;
        case QS_MODULE_HMI:
            cxt->mask = QS_HMI_SIG;
            break;
        case QS_MODULE_THREAD:
            cxt->mask = QS_TWN_SIG;
            break;
        case QS_MODULE_ZIGBEE:
            cxt->mask = QS_ZB_SIG;
            break;
        case QS_MODULE_HERH:
            cxt->mask = QS_HERH_SIG;
            break;
        case QS_MODULE_COEX:
            cxt->mask = QS_COEX_SIG;
            break;
        case QS_MODULE_FWUPGRADE:
            cxt->mask = QS_FWUPGRADE_SIG;
            break;

        default:
            pthread_mutex_unlock(&cxt->mutex);
            pthread_mutex_unlock(&ModuleArraymutex);
            QsDeInit(ModuleID);
            return ssFailure;
    }

    if (ssSuccess == QsTransportInit(TransportID, IfName, &cxt->handle))
    {
        cxt->transport = TransportID;
        pthread_mutex_unlock(&cxt->mutex);
        pthread_mutex_unlock(&ModuleArraymutex);
        return ssSuccess;
    }
    pthread_mutex_unlock(&cxt->mutex);
    pthread_mutex_unlock(&ModuleArraymutex);
    QsDeInit(ModuleID);
    return ssFailure;
}


uint32_t QsDeInit(uint8_t ModuleID)
{
    qsHostModuleCxt_t *cxt = NULL;
    uint32_t ret_Val = ssSuccess;

    pthread_mutex_lock(&ModuleArraymutex);
    if ((ModuleID >= QS_MODULE_MAX) || !ModuleCxt[ModuleID])
    {
        pthread_mutex_unlock(&ModuleArraymutex);
        return ssFailure;
    }

    cxt = ModuleCxt[ModuleID];
    pthread_mutex_lock(&cxt->mutex);

    if(QS_TRANSPORT_MAX_E != cxt->transport)
    {
        ret_Val = QsTransportDeInit(cxt->transport, cxt->handle);
        if(ssSuccess != ret_Val)
        {
            pthread_mutex_unlock(&cxt->mutex);
            pthread_mutex_unlock(&ModuleArraymutex);
            return ssFailure;
        }
    }

    QsDeInitQueue(&cxt->queue);
    pthread_cond_destroy(&cxt->event);
    pthread_mutex_unlock(&cxt->mutex);
    pthread_mutex_destroy(&cxt->mutex);
    free(cxt);
    ModuleCxt[ModuleID] = NULL;
    pthread_mutex_unlock(&ModuleArraymutex);
    BufpoolDeInit();
    return ret_Val;
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

static Boolean_t AllocateCommandPacket(uint8_t ModuleID, uint16_t FileID, uint16_t FunctionID, PackedBuffer_t *PackedBuffer, uint16_t Size)
{
    Boolean_t   RetVal = FALSE;
    qsQapiReq_t *Command = NULL;
    uint8_t     Module = 0;
    BufInfo_t   *BufInfo = NULL;

    RetVal = MapQSModuleID(ModuleID, &Module);

    /* Get buffer of required size. */
    if(ssSuccess != QsGetBuf(&BufInfo, QS_TOTAL_LEN(QSCOMMAND_T_SIZE(Size))))
    {
        return FALSE;
    }
    if(BufInfo && BufInfo->buf)
    {
        PackedBuffer->Packet = (uint8_t *)BufInfo;
        Command = (qsQapiReq_t *)(QSPACKET_HEADER(BufInfo->buf));

        /* Set the packed buffer parameters. */
        PackedBuffer->PacketType = QS_PACKET_E;
        PackedBuffer->Length = Size;
        PackedBuffer->Start = QSCOMMAND_DATA(Command);
        InitializePackedBuffer(PackedBuffer, PackedBuffer->Start, Size);

        /* Write the QAPI command parameters. */
        WRITE_UNALIGNED_LITTLE_ENDIAN_UINT16(&Command->Header.TotalLength, QSCOMMAND_T_SIZE(Size));
        WRITE_UNALIGNED_LITTLE_ENDIAN_UINT8( &Command->Header.ModuleID,    Module);
        WRITE_UNALIGNED_LITTLE_ENDIAN_UINT8( &Command->Header.PacketType,  QS_PACKET_E);
        WRITE_UNALIGNED_LITTLE_ENDIAN_UINT16(&Command->Header.FileID,      FileID);
        WRITE_UNALIGNED_LITTLE_ENDIAN_UINT16(&Command->Header.FunctionID,  FunctionID);
        WRITE_UNALIGNED_LITTLE_ENDIAN_UINT16(&Command->Header.DataLength,  Size);

        RetVal = TRUE;
    }
    else
    {
        RetVal = FALSE;
    }

    return(RetVal);
}

/**
  Allocates a packed buffer.

  @param ModuleID     ID of the module sending this packet.
  @param FileID       ID of the file sending this packet.
  @param FunctionID   ID of the function sending this packet.
  @param PackedBuffer Pointer to the packed buffer to be allocated.
  @param Size         Required size of the buffer.

  @return the status result of the allocation.
  */
Boolean_t AllocatePackedBuffer(qsPacketType_t PacketType, uint8_t ModuleID, uint16_t FileID, uint16_t FunctionID, PackedBuffer_t *PackedBuffer, uint16_t Size)
{
    Boolean_t RetVal;

    switch (PacketType)
    {
        case QS_PACKET_E:
            RetVal = AllocateCommandPacket(ModuleID, FileID, FunctionID, PackedBuffer, Size);
            break;
        default:
            RetVal = FALSE;
    }

    return(RetVal);
}

/**
  Frees a packed buffer.

  @param PackedBuffer Pointer to the packed buffer to be freed.
  */
void FreePackedBuffer(PackedBuffer_t *PackedBuffer)
{
    if(PackedBuffer->Packet != NULL)
    {
        QsReleaseBuf((BufInfo_t *)PackedBuffer->Packet);
        PackedBuffer->Packet = NULL;
    }
}

SerStatus_t SendCommand(PackedBuffer_t *InputBuffer, PackedBuffer_t *OutputBuffer)
{
    uint8_t        i = 0;
    uint16_t       TotalLen = 0;
    uint32_t       UID_upper_bytes = 0x00000000;
    SerStatus_t    retVal = ssFailure;
    BufInfo_t      *ResponseInfo = NULL;
    qsQapiResp_t   *Response = NULL;
    qsQapiReq_t    *Packet = NULL;
    pthread_cond_t UID;

    Packet = (qsQapiReq_t *)QSPACKET_HEADER(((BufInfo_t *)InputBuffer->Packet)->buf);
    for(i = 0; i < 8; i++)
    {
        Packet->Header.UID[i] = 0x00;
    }

    if (0 != pthread_cond_init(&UID, NULL))
    {
        return ssFailure;
    }

    WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(&Packet->Header.UID[4], &UID_upper_bytes);
    if((((unsigned long long)&UID) >> 32) != 0)
    {
        WRITE_UNALIGNED_LITTLE_ENDIAN_UINT64(&Packet->Header.UID[0], (unsigned long long)&UID);
    }
    else
    {
        WRITE_UNALIGNED_LITTLE_ENDIAN_UINT32(&Packet->Header.UID[0], &UID);
    }

    /* Transmit the packet and wait for the response. */
    TotalLen = QS_TOTAL_LEN(READ_UNALIGNED_LITTLE_ENDIAN_UINT16(&Packet->Header.TotalLength));
    if(Transmit((char *)(((BufInfo_t *)InputBuffer->Packet)->buf), TotalLen) == ssSuccess)
    {
        if(QsWaitForResp((void *)&UID, Packet->Header.ModuleID, &ResponseInfo) == ssSuccess)
        {
            /* Check the response values. */
            if((ResponseInfo != NULL) && (ResponseInfo->buf != NULL))
            {
                Response = (qsQapiResp_t *)ResponseInfo->buf;

                if(Response->Header.DataLength > 0)
                {
                    /* Set the output buffer parameters. */
                    OutputBuffer->PacketType = QS_RETURN_E;
                    OutputBuffer->Packet = (uint8_t *)ResponseInfo;
                    OutputBuffer->Start  = QSRESPONSE_DATA(Response);
                    InitializePackedBuffer(OutputBuffer, OutputBuffer->Start, READ_UNALIGNED_LITTLE_ENDIAN_UINT16(&Response->Header.DataLength));
                    retVal = ssSuccess;
                }
                else
                {
                    /* Error packet */
                    retVal = ssFailure;
                }
            }
            else
                retVal = ssFailure;
        }
        else
            retVal = ssFailure;
    }
    else
        retVal = ssFailure;

    return retVal;
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

    UartCxt_t *cxt = (UartCxt_t *)Arg;
    start = getMicrotime();
    QsUartTransmit(cxt->handle, size, buf);
    ++latency_test_started;
}

#endif /* LATENCY_DEBUG */
#endif /* CONFIG_DAEMON_MODE */

