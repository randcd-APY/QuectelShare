/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "connx_handsfree_audio_common.h"


static void HandsfreeAudioThread(void *pointer)
{
    ConnxHandsfreeAudioInstance *inst = (ConnxHandsfreeAudioInstance *)pointer;
    ConnxHandle eventHandle;
    ConnxHandsfreeAudioCallbackT *cb;
    ConnxResult result = 0;
    uint32_t eventBits = 0;

    if (!inst)
        return;

    eventHandle = inst->eventHandle;
    cb = &inst->callback;

    if (cb->initCb != NULL)
        cb->initCb(inst);

    while (!inst->quit)
    {
        result = ConnxEventWait(eventHandle, CONNX_EVENT_WAIT_INFINITE, &eventBits);

        if (result != CONNX_RESULT_SUCCESS)
        {
            /* Fail to wait event. */
            break;
        }

        if (inst->quit || (eventBits == HANDSFREE_EVENT_QUIT_REQ))
        {
            /* Quit thread. */
            break;
        }

        if (cb->handleEventCb != NULL)
            cb->handleEventCb(inst, eventBits);
    }

    if (cb->deinitCb != NULL)
        cb->deinitCb(inst);
}

int ConnxHandsfreeAudioInit_I(ConnxHandsfreeAudioInstance *inst, ConnxHandsfreeAudioCallbackT *callback)
{
    if (!inst || !callback)
    {
        /* Invalid parameter. */
        return 1;
    }

    memset(inst, 0, sizeof(ConnxHandsfreeAudioInstance));

    memcpy(&inst->callback, callback, sizeof(ConnxHandsfreeAudioCallbackT));

    inst->audio_on = false;
    inst->quit = false;

    inst->eventHandle = ConnxEventCreate();

    inst->threadHandle = ConnxThreadCreate(HandsfreeAudioThread,
                                           inst,
                                           0,
                                           0,
                                           "Handsfree Audio Thread");

    inst->initialized = true;

    return 0;
}

void ConnxHandsfreeAudioDeinit_I(ConnxHandsfreeAudioInstance *inst)
{
    uint16_t delay = 5; /* ms */

    if (!inst || !inst->initialized)
        return;

    inst->audio_on = false;
    inst->quit = true;

    if (inst->eventHandle != NULL)
    {
        ConnxEventSet(inst->eventHandle, HANDSFREE_EVENT_QUIT_REQ);

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

    inst->initialized = false;
}

int ConnxHandsfreeAudioSetEvent_I(ConnxHandsfreeAudioInstance *inst, uint32_t eventBits)
{
    if (!inst || !inst->initialized)
    {
        /* Invalid parameter. */
        return 1;
    }

    ConnxEventSet(inst->eventHandle, eventBits);

    return 0;
}

bool ConnxHandsfreeAudioInitService_I(const char *script)
{
    return ConnxScriptLaunch(script, "init");
}

bool ConnxHandsfreeAudioDeinitService_I(const char *script)
{
    return ConnxScriptLaunch(script, "deinit");
}

bool ConnxHandsfreeAudioStartService_I(const char *script, int sample_rate)
{
    char cmd[CONNX_MAX_PATH_LENGTH];

    snprintf(cmd, sizeof(cmd), "start %d", sample_rate);

    return ConnxScriptLaunch(script, cmd);
}

bool ConnxHandsfreeAudioStopService_I(const char *script)
{
    return ConnxScriptLaunch(script, "stop");
}

void ConnxHandsfreeAudioInitCallback_I(ConnxHandsfreeAudioCallbackT    *callback,
                                       ConnxHandsfreeAudioInitCb        initCb,
                                       ConnxHandsfreeAudioDeinitCb      deinitCb,
                                       ConnxHandsfreeAudioHandleEventCb handleEventCb)
{
    if (!callback)
        return;

    memset(callback, 0, sizeof(ConnxHandsfreeAudioCallbackT));

    callback->initCb        = initCb;
    callback->deinitCb      = deinitCb;
    callback->handleEventCb = handleEventCb;
}