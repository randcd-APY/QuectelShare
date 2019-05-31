/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "connx_hf_audio.h"
#include "connx_handsfree_audio_common.h"
#ifdef USE_BT_AUDIO_SERVICE
#include "connx_app_bas_external_lib.h"
#endif

#define BT_HF_AUDIO_EVENT(l, h)         SET_DWORD(l, h)
#define GET_HF_AUDIO_REQ(val)           GET_LOWORD(val)
#define GET_HF_AUDIO_SAMPLE_RATE(val)   GET_HIWORD(val)

#define BT_HF_AUDIO_ENABLE_REQ          0x1
#define BT_HF_AUDIO_DISABLE_REQ         0x2

#define BT_HF_AUDIO_SERVICE             "bt_hf_audio.sh"


static ConnxHandsfreeAudioInstance hf_audio_inst = { false, false, false, NULL, NULL, { NULL, NULL, NULL } };


static void InitHfAudioService(void *context)
{
    CONNX_UNUSED(context);

#ifdef USE_BT_AUDIO_SERVICE
    ConnxAppBtAudioServiceReq(CONNX_BAS_INIT_HF_CALL_AUDIO_IND, 0);
#else
    ConnxHandsfreeAudioInitService_I(BT_HF_AUDIO_SERVICE);
#endif
}

static void DeinitHfAudioService(void *context)
{
    CONNX_UNUSED(context);
    
#ifdef USE_BT_AUDIO_SERVICE
    ConnxAppBtAudioServiceReq(CONNX_BAS_DEINIT_HF_CALL_AUDIO_IND, 0);
#else
    ConnxHandsfreeAudioDeinitService_I(BT_HF_AUDIO_SERVICE);
#endif
}

static void StartHfAudio(int sample_rate)
{
#ifdef USE_BT_AUDIO_SERVICE
    ConnxAppBtAudioServiceReq(CONNX_BAS_START_HF_CALL_AUDIO_IND, sample_rate);
#else
    ConnxHandsfreeAudioStartService_I(BT_HF_AUDIO_SERVICE, sample_rate);
#endif
}

static void StopHfAudio(void)
{
#ifdef USE_BT_AUDIO_SERVICE
    ConnxAppBtAudioServiceReq(CONNX_BAS_STOP_HF_CALL_AUDIO_IND, 0);
#else
    ConnxHandsfreeAudioStopService_I(BT_HF_AUDIO_SERVICE);
#endif
}

static void HandlerHfAudioEvent(void *context, uint32_t eventBits)
{
    ConnxHandsfreeAudioInstance *inst = &hf_audio_inst;
    uint16_t req = GET_HF_AUDIO_REQ(eventBits);
    uint16_t sample_rate = GET_HF_AUDIO_SAMPLE_RATE(eventBits);

    CONNX_UNUSED(context);

    if (req == BT_HF_AUDIO_ENABLE_REQ)
    {
        /* Switch audio into HF. */
        if (!inst->audio_on)
        {
            StartHfAudio(sample_rate);

            inst->audio_on = true;
        }
    }
    else if (req == BT_HF_AUDIO_DISABLE_REQ)
    {
        /* Switch audio into AG. */
        if (inst->audio_on)
        {
            StopHfAudio();

            inst->audio_on = false;
        }
    }
    else
    {
        /* Unknown HF audio event. */
    }
}

int ConnxHfAudioInit(void)
{
    ConnxHandsfreeAudioInstance *inst = &hf_audio_inst;
    ConnxHandsfreeAudioCallbackT callback;

    ConnxHandsfreeAudioInitCallback_I(&callback,
                                      InitHfAudioService,
                                      DeinitHfAudioService,
                                      HandlerHfAudioEvent);

    return ConnxHandsfreeAudioInit_I(inst, &callback);
}

void ConnxHfAudioDeinit(void)
{
    ConnxHandsfreeAudioInstance *inst = &hf_audio_inst;

    ConnxHandsfreeAudioDeinit_I(inst);
}

int ConnxHfAudioStart(int sample_rate)
{
    ConnxHandsfreeAudioInstance *inst = &hf_audio_inst;
    uint32_t eventBits = BT_HF_AUDIO_EVENT(BT_HF_AUDIO_ENABLE_REQ, sample_rate);

    return ConnxHandsfreeAudioSetEvent_I(inst, eventBits);
}

int ConnxHfAudioStop(void)
{
    ConnxHandsfreeAudioInstance *inst = &hf_audio_inst;
    uint32_t eventBits = BT_HF_AUDIO_EVENT(BT_HF_AUDIO_DISABLE_REQ, 0);

    return ConnxHandsfreeAudioSetEvent_I(inst, eventBits);
}