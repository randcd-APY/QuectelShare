/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/
#include "bt_audio_service_transport.h"
#include "bt_audio_service_sef.h"
#include "hf_audio_transfer.h"
#include "hfg_audio_transfer.h"

#include "connx_util.h"
#include "connx_log.h"

void HandleInitHfAudioInd()
{
    init_hf_audio();
}

void HandleStartHfAudioInd(uint16_t sampleRate)
{
    start_hf_audio(sampleRate);
}

void HandleStopHfAudioInd()
{
    stop_hf_audio();
}

void HandleDeinitHfAudioInd()
{
    deinit_hf_audio();
}

void HandleInitHfgAudioInd()
{
    init_hfg_audio();
}

void HandleStartHfgAudioInd(uint16_t sampleRate)
{
    start_hfg_audio(sampleRate);
}

void HandleStopHfgAudioInd()
{
    stop_hfg_audio();
}

void HandleDeinitHfgAudioInd()
{
    deinit_hfg_audio();
}

void HandleInitA2dpStreamingInd()
{

}

void HandleStartA2dpStreamingInd(uint16_t sampleRate)
{
    CONNX_UNUSED(sampleRate);
}

void HandleStopA2dpStreamingInd()
{

}

void HandleDeinitA2dpStreamingInd()
{

}

