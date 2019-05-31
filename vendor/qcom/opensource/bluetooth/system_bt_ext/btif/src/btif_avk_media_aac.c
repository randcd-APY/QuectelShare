/******************************************************************************
 *
 *  Copyright (c) 2015,2017 The Linux Foundation. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *   met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************/

#include "btif_media.h"
#include "a2d_aac.h"


int btif_a2dp_get_aac_track_frequency(UINT16 frequency) {
    int freq = 44100;
    switch (frequency) {
        case A2D_AAC_IE_SAMP_FREQ_16000:
            freq = 16000;
            break;
        case A2D_AAC_IE_SAMP_FREQ_32000:
            freq = 32000;
            break;
        case A2D_AAC_IE_SAMP_FREQ_44100:
            freq = 44100;
            break;
        case A2D_AAC_IE_SAMP_FREQ_48000:
            freq = 48000;
            break;
    }
    return freq;
}

int btif_a2dp_get_aac_track_channel_count(UINT8 channeltype) {
    int count = 2;
    switch (channeltype) {
        case A2D_AAC_IE_CHANNELS_1:
            count = 1;
            break;
        case A2D_AAC_IE_CHANNELS_2:
/* If we use AudioTrack for rendering data, we need to set */
/* channel value as 3, for stereo */
#ifdef USE_AUDIO_TRACK
            count = 3;
#else
            count = 2;
#endif
            break;
    }
    return count;
}

