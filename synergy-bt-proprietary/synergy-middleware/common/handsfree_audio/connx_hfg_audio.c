/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "connx_hfg_audio.h"
#include "connx_handsfree_audio_common.h"
#ifdef USE_BT_AUDIO_SERVICE
#include "connx_app_bas_external_lib.h"
#endif

#define BT_HFG_AUDIO_EVENT(l, h)        SET_DWORD(l, h)
#define GET_HFG_AUDIO_REQ(val)          GET_LOWORD(val)
#define GET_HFG_AUDIO_SAMPLE_RATE(val)  GET_HIWORD(val)

#define BT_HFG_AUDIO_ENABLE_REQ         0x1
#define BT_HFG_AUDIO_DISABLE_REQ        0x2

#define BT_HFG_AUDIO_SERVICE            "bt_hfg_audio.sh"


static ConnxHandsfreeAudioInstance hfg_audio_inst = { false, false, false, NULL, NULL, { NULL, NULL, NULL } };


static void InitHfgAudioService(void *context)
{
    CONNX_UNUSED(context);

#ifdef USE_BT_AUDIO_SERVICE
    ConnxAppBtAudioServiceReq(CONNX_BAS_INIT_HFG_CALL_AUDIO_IND, 0);
#else
    ConnxHandsfreeAudioInitService_I(BT_HFG_AUDIO_SERVICE);
#endif
}

static void DeinitHfgAudioService(void *context)
{
    CONNX_UNUSED(context);

#ifdef USE_BT_AUDIO_SERVICE
    ConnxAppBtAudioServiceReq(CONNX_BAS_DEINIT_HFG_CALL_AUDIO_IND, 0);
#else
    ConnxHandsfreeAudioDeinitService_I(BT_HFG_AUDIO_SERVICE);
#endif
}

static void StartHfgAudio(int sample_rate)
{
#ifdef USE_BT_AUDIO_SERVICE
    ConnxAppBtAudioServiceReq(CONNX_BAS_START_HFG_CALL_AUDIO_IND, sample_rate);
#else
    ConnxHandsfreeAudioStartService_I(BT_HFG_AUDIO_SERVICE, sample_rate);
#endif
}

static void StopHfgAudio(void)
{
#ifdef USE_BT_AUDIO_SERVICE
    ConnxAppBtAudioServiceReq(CONNX_BAS_STOP_HFG_CALL_AUDIO_IND, 0);
#else
    ConnxHandsfreeAudioStopService_I(BT_HFG_AUDIO_SERVICE);
#endif
}

static void HandlerHfgAudioEvent(void *context, uint32_t eventBits)
{
    ConnxHandsfreeAudioInstance *inst = &hfg_audio_inst;
    uint16_t req = GET_HFG_AUDIO_REQ(eventBits);
    uint16_t sample_rate = GET_HFG_AUDIO_SAMPLE_RATE(eventBits);

    CONNX_UNUSED(context);

    if (req == BT_HFG_AUDIO_ENABLE_REQ)
    {
        /* Switch audio into AG. */
        if (!inst->audio_on)
        {
            StartHfgAudio(sample_rate);

            inst->audio_on = true;
        }
    }
    else if (req == BT_HFG_AUDIO_DISABLE_REQ)
    {
        /* Switch audio into HF. */
        if (inst->audio_on)
        {
            StopHfgAudio();

            inst->audio_on = false;
        }
    }
    else
    {
        /* Unknown HFG audio event. */
    }
}

int ConnxHfgAudioInit(void)
{
    ConnxHandsfreeAudioInstance *inst = &hfg_audio_inst;
    ConnxHandsfreeAudioCallbackT callback;

    ConnxHandsfreeAudioInitCallback_I(&callback,
                                      InitHfgAudioService,
                                      DeinitHfgAudioService,
                                      HandlerHfgAudioEvent);

    return ConnxHandsfreeAudioInit_I(inst, &callback);
}

void ConnxHfgAudioDeinit(void)
{
    ConnxHandsfreeAudioInstance *inst = &hfg_audio_inst;

    ConnxHandsfreeAudioDeinit_I(inst);
}

int ConnxHfgAudioStart(int sample_rate)
{
    ConnxHandsfreeAudioInstance *inst = &hfg_audio_inst;
    uint32_t eventBits = BT_HFG_AUDIO_EVENT(BT_HFG_AUDIO_ENABLE_REQ, sample_rate);

    return ConnxHandsfreeAudioSetEvent_I(inst, eventBits);
}

int ConnxHfgAudioStop(void)
{
    ConnxHandsfreeAudioInstance *inst = &hfg_audio_inst;
    uint32_t eventBits = BT_HFG_AUDIO_EVENT(BT_HFG_AUDIO_DISABLE_REQ, 0);

    return ConnxHandsfreeAudioSetEvent_I(inst, eventBits);
}
