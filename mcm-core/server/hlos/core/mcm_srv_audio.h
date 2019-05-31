/***************************************************************************************************
    @file
    mcm_srv_audio.h

    @brief
    Supports functions for handling voice and audio stream in voice call.

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
***************************************************************************************************/

#ifndef MCM_SRV_AUDIO_H
#define MCM_SRV_AUDIO_H

int mcm_srv_enable_audio_stream();
int mcm_srv_enable_voice_stream();
int mcm_srv_disable_audio_stream();
int mcm_srv_disable_voice_stream();
int mcm_srv_mute_voice_stream(int mute_type);
int mcm_srv_snd_init();


#endif

