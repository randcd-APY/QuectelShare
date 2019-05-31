/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "connx_ipc_common.h"
#include "connx_log.h"
#include "connx_util.h"
#include "connx_callback.h"
#include "connx_ipc.h"
#include "connx_bas_common.h"

#include "bt_audio_service.h"
#include "bt_audio_service_transport.h"

static void BAS_InitIpcRegisterInfo(ConnxIpcRegisterInfo *ipcRegisterInfo, ConnxContext appContext);
static void BAS_FreeRegisterInfo(BasRegisterInfo *registerInfo);
static void BAS_IpcDataInd(ConnxHandle handle, ConnxContext context, void *data, size_t size);
static BasRegisterInfo *BAS_DuplicateRegisterInfo(BasRegisterInfo *srcRegisterInfo);
static void BasMsgHandlerThread(void *data);

#define BAT_LOCK(inst)       ConnxMutexLock((inst)->mutexHandle)
#define BAT_UNLOCK(inst)     ConnxMutexUnlock((inst)->mutexHandle)

static BasTransportInstance transportInst =
{
    .init = false,
};

BasTransportInstance *BasTransportGetInstance()
{
    return &transportInst;
}

static void HandlerBasMessage(void *context, void *msg)
{
    BasTransportInstance *inst = BasTransportGetInstance();
    uint8_t *prim = (uint8_t *) msg;
    uint8_t *tmpPrim = prim;
    uint16_t primType = 0;
    uint16_t sample_rate = 0;

    CONNX_UNUSED(context);
    IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("<%s> msg: %p"), __FUNCTION__, msg));

    if (!msg)
        return;

    CONNX_CONVERT_16_FROM_XAP(primType, tmpPrim);
    IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("<%s> primType: 0x%X"), __FUNCTION__, primType));

    switch (primType)
    {
        case CONNX_BAS_INIT_HF_CALL_AUDIO_IND:
        {
            HandleInitHfAudioInd();
            break;
        }

        case CONNX_BAS_START_HF_CALL_AUDIO_IND:
        {
            CONNX_CONVERT_16_FROM_XAP(sample_rate, tmpPrim);
            HandleStartHfAudioInd(sample_rate);
            break;
        }

        case CONNX_BAS_STOP_HF_CALL_AUDIO_IND:
        {
            HandleStopHfAudioInd();
            break;
        }

        case CONNX_BAS_DEINIT_HF_CALL_AUDIO_IND:
        {
            HandleDeinitHfAudioInd();
            break;
        }

        case CONNX_BAS_INIT_HFG_CALL_AUDIO_IND:
        {
            HandleInitHfgAudioInd();
            break;
        }

        case CONNX_BAS_START_HFG_CALL_AUDIO_IND:
        {
            CONNX_CONVERT_16_FROM_XAP(sample_rate, tmpPrim);
            HandleStartHfgAudioInd(sample_rate);
            break;
        }

        case CONNX_BAS_STOP_HFG_CALL_AUDIO_IND:
        {
            HandleStopHfgAudioInd();
            break;
        }

        case CONNX_BAS_DEINIT_HFG_CALL_AUDIO_IND:
        {
            HandleDeinitHfgAudioInd();
            break;
        }

        case CONNX_BAS_INIT_A2DP_STREAMING_IND:
        {
            HandleInitA2dpStreamingInd();
            break;
        }

        case CONNX_BAS_START_A2DP_STREAMING_IND:
        {
            CONNX_CONVERT_16_FROM_XAP(sample_rate, tmpPrim);
            HandleStartA2dpStreamingInd(sample_rate);
            break;
        }

        case CONNX_BAS_STOP_A2DP_STREAMING_IND:
        {
            HandleStopA2dpStreamingInd();
            break;
        }

        case CONNX_BAS_DEINIT_A2DP_STREAMING_IND:
        {
            HandleDeinitA2dpStreamingInd();
            break;
        }

        default:
        {
            /* Invalid request. */
            IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Invalid primType: 0x%04x"), __FUNCTION__, primType));
            return;
        }
    }

}

void ConnxBasInitCallback(ConnxBasCallbackT *callback, ConnxBasHandleMsgCb  handleMsgCb)
{
    if (!callback)
        return;

    memset(callback, 0, sizeof(ConnxBasCallbackT));

    callback->handleMsgCb = handleMsgCb;
}

bool BasInitTransportInst(BasTransportInstance *inst)
{
    ConnxHandle eventHandle = NULL;
    ConnxHandle threadHandle = NULL;

    if (!inst)
        return false;

    inst->quitThread = false;

    eventHandle = ConnxEventCreate();

    if (!eventHandle)
    {
        IFLOG(DebugOut(DEBUG_ERROR, TEXT("<%s> Create event fail"), __FUNCTION__));
        return false;
    }

    inst->eventHandle = eventHandle;

    threadHandle = ConnxThreadCreate(BasMsgHandlerThread,
                                     inst,
                                     0,
                                     CONNX_THREAD_PRIORITY_NORMAL,
                                     "BT Audio Service Msg Handler Thread");

    if (!threadHandle)
    {
        IFLOG(DebugOut(DEBUG_ERROR, TEXT("<%s> Create connx bt thread fail"), __FUNCTION__));
        return false;
    }

    inst->threadHandle = threadHandle;

    ConnxBasInitCallback(&inst->callback, HandlerBasMessage);
    ConnxCmnListInit(&inst->messageList, 0, NULL, NULL);

    return true;
}

void BasDeinitTransportInst(BasTransportInstance *inst)
{
    uint16_t delay = 5; /* ms */

    if (!inst || !inst->init)
        return;

    inst->quitThread = true;

    if (inst->eventHandle != NULL)
    {
        ConnxEventSet(inst->eventHandle, BAS_EVENT_QUIT_REQ);

        /* Add some delay to wait thread exit. */
        ConnxThreadSleep(delay);
    }

    if (inst->threadHandle != NULL)
    {
        ConnxThreadDestroy(inst->threadHandle);
        inst->threadHandle = NULL;
    }

    if (inst->eventHandle != NULL)
    {
        ConnxEventDestroy(inst->eventHandle);
        inst->eventHandle = NULL;
    }

    if (inst->mutexHandle != NULL)
    {
        ConnxMutexDestroy(inst->mutexHandle);
        inst->mutexHandle = NULL;
    }

    ConnxCmnListDeinit(&inst->messageList);

    inst->init = false;
}

static void AddBasMessage(BasTransportInstance *inst, void *msg, size_t size)
{
    ConnxBasDataElm_t *elm;

    BAT_LOCK(inst);

    elm = (ConnxBasDataElm_t *) ConnxCmnListElementAddLast(&inst->messageList, sizeof(ConnxBasDataElm_t));
    memcpy(&elm->message, msg, size);

    BAT_UNLOCK(inst);
}

static ConnxBasDataElm_t *GetBasMessage(BasTransportInstance *inst)
{
    ConnxBasDataElm_t *elm;

    BAT_LOCK(inst);

    elm = (ConnxBasDataElm_t *) ConnxCmnListElementGetFirst(&inst->messageList);

    BAT_UNLOCK(inst);

    return elm;
}

static void DeleteBasMessage(BasTransportInstance *inst, ConnxBasDataElm_t *elm)
{
    BAT_LOCK(inst);

    ConnxCmnListElementRemove(&inst->messageList, (ConnxCmnListElm_t *) elm);

    BAT_UNLOCK(inst);
}

static void FlushBasMessage(BasTransportInstance *inst)
{
    ConnxBasCallbackT *cb;
    ConnxBasDataElm_t *elm = NULL;

    cb = &inst->callback;

    while ((elm = GetBasMessage(inst)) != NULL)
    {
        if (cb->handleMsgCb != NULL)
            cb->handleMsgCb(inst, &elm->message);

        DeleteBasMessage(inst, elm);
    }
}


static void BAS_InitIpcRegisterInfo(ConnxIpcRegisterInfo *ipcRegisterInfo, ConnxContext appContext)
{
    if (!ipcRegisterInfo || !appContext)
    {
        return;
    }

    memset(ipcRegisterInfo, 0, sizeof(ConnxIpcRegisterInfo));

    ipcRegisterInfo->size		  = sizeof(ConnxIpcRegisterInfo);
    ipcRegisterInfo->appContext   = appContext;
    ipcRegisterInfo->type         = IPC_SOCKET;
    ipcRegisterInfo->flag         = SOCKET_SERVER;
    ipcRegisterInfo->name	      = SOCKET_PATH;
    ipcRegisterInfo->dataIndCb	  = BAS_IpcDataInd;
    ipcRegisterInfo->closeCb	  = NULL;
    ipcRegisterInfo->reservedCb	  = NULL;
}

static void BAS_FreeRegisterInfo(BasRegisterInfo *registerInfo)
{
    if (!registerInfo)
        return;

    free(registerInfo);
}

static BasRegisterInfo *BAS_DuplicateRegisterInfo(BasRegisterInfo *srcRegisterInfo)
{
    BasRegisterInfo *registerInfo = NULL;

    if (!srcRegisterInfo)
    {
        return NULL;
    }

    registerInfo = (BasRegisterInfo *) malloc(sizeof(BasRegisterInfo));

    if (!registerInfo)
    {
        return NULL;
    }

    memcpy(registerInfo, srcRegisterInfo, sizeof(BasRegisterInfo));

    return registerInfo;
}

static void BAS_IpcDataInd(ConnxHandle handle, ConnxContext context, void *data, size_t size)
{
    BasTransportInstance *inst = BasTransportGetInstance();
    uint8_t *buff = (uint8_t *) data;
    uint8_t *tmpBuff = NULL;
    size_t offset = 0;
    size_t remained = size;
    size_t primSize = 0;
    ConnxBasPrim primType = 0;
    ConnxBtAudioServiceReq *message = NULL;

    IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("<%s> handle: %p, context: %p, data: %p, size: %d"),
                   __FUNCTION__, handle, context, data, size));

    if (!handle || !context || !data || !size)
    {
        return;
    }

    while (offset < size)
    {
        tmpBuff = buff;
        CONNX_CONVERT_16_FROM_XAP(primType, tmpBuff);

        if (primType < CONNX_BAS_PRIM_UPSTREAM_HIGHEST)
        {
            primSize = CONNX_MIN(sizeof(ConnxBtAudioServiceReq), remained);

            if (primSize == sizeof(ConnxBtAudioServiceReq))
            {
                message = (ConnxBtAudioServiceReq *)malloc(sizeof(ConnxBtAudioServiceReq));

                if (!message)
                {
                    IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Malloc memory fail"), __FUNCTION__));
                    break;
                }
					
                memcpy(message, buff, sizeof(ConnxBtAudioServiceReq));
                AddBasMessage(inst, message, sizeof(ConnxBtAudioServiceReq));

                if (message)
                {
                    free(message);
                }

                ConnxEventSet(inst->eventHandle, BAS_EVENT_ADD_MSG_REQ);
            }
        }

        remained -= primSize;
        offset += primSize;
        buff += primSize;
    }
}

ConnxHandle BasTransportOpen(ConnxContext appContext)
{
    BasTransportInstance *inst = BasTransportGetInstance();
    BasRegisterInfo *registerInfo = (BasRegisterInfo *)appContext;
    BasRegisterInfo *appRegisterInfo = NULL;
    ConnxIpcRegisterInfo ipcRegisterInfo;

    if (!registerInfo ||
        (registerInfo->size != sizeof(BasRegisterInfo)))
    {
        return NULL;
    }

    appRegisterInfo = BAS_DuplicateRegisterInfo(registerInfo);
    BAS_InitIpcRegisterInfo(&ipcRegisterInfo, (ConnxContext) appRegisterInfo);

    if (!BasInitTransportInst(inst))
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Create thread fail"), __FUNCTION__));
        return NULL;
    }

    return ConnxIpcOpen((ConnxContext) &ipcRegisterInfo);
}

void BasTransportClose(ConnxHandle transportHandle)
{
    BasTransportInstance *inst = BasTransportGetInstance();
    BasRegisterInfo *registerInfo = NULL;

    registerInfo = (BasRegisterInfo *) ConnxIpcGetContext(transportHandle);
    BAS_FreeRegisterInfo(registerInfo);
    ConnxIpcClose(transportHandle);
    BasDeinitTransportInst(inst);
}

static void BasMsgHandlerThread(void *data)
{
    BasTransportInstance *inst = (BasTransportInstance *) data;
    ConnxHandle eventHandle = NULL;
    ConnxResult res = 0;
    uint32_t eventBits = 0;

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> "), __FUNCTION__));

    if (!inst)
        return;

    eventHandle = inst->eventHandle;

    while (!inst->quitThread)
    {
        res = ConnxEventWait(eventHandle, CONNX_EVENT_WAIT_INFINITE, &eventBits);

        if (res != CONNX_RESULT_SUCCESS)
        {
            /* Fail to wait event. */
            break;
        }

        if (inst->quitThread || eventBits == BAS_EVENT_QUIT_REQ)
        {
            IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Quit thread"), __FUNCTION__));
            break;
        }

        FlushBasMessage(inst);
    }

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> Return"), __FUNCTION__));
}

