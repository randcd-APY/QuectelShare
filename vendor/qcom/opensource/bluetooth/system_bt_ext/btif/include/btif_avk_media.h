/******************************************************************************
 *  Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
 *
 *  Not a contribution.
 ******************************************************************************/

/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/*******************************************************************************
 *
 *  Filename:      btif_avk_media.h
 *
 *  Description:   This is the audio module for the BTIF system.
 *
 *******************************************************************************/

#ifndef BTIF_AVK_MEDIA_H
#define BTIF_AVK_MEDIA_H

#include <stdbool.h>

#include "bta_api.h"
#include "btif_avk_api.h"
#include "audio_a2dp_hw.h"

/*******************************************************************************
 **  Constants
 *******************************************************************************/

/* Generic part */
#define BTIF_SUCCESS                         0

/**
 * AV (Audio Video source) Errors
 */
#define BTIF_ERROR_SRV_AV_NOT_ENABLED        700     /* AV is not enabled */
#define BTIF_ERROR_SRV_AV_FEEDING_NOT_SUPPORTED 701  /* Requested Feeding not supported */
#define BTIF_ERROR_SRV_AV_BUSY               702     /* Another operation ongoing */
#define BTIF_ERROR_SRV_AV_NOT_OPENED         703     /* No AV link opened */
#define BTIF_ERROR_SRV_AV_NOT_STARTED        704     /* AV is not started */
#define BTIF_ERROR_SRV_AV_CP_NOT_SUPPORTED   705     /* Content protection is not supported by all headsets */

/* Transcoding definition for TxTranscoding and RxTranscoding */
#define BTIF_AVK_MEDIA_TRSCD_OFF             0
#define BTIF_AVK_MEDIA_TRSCD_PCM_2_SBC       1  /* Tx */
#define BTIF_AVK_MEDIA_TRSCD_PCM_2_APTX      2

extern int btif_max_avk_clients;                      /* maximum number of AV clients supported */

/*******************************************************************************
 **  Data types
 *******************************************************************************/

typedef int tBTIF_STATUS;

/* tBTIF_AVK_MEDIA_INIT_AUDIO msg structure */
typedef struct
{
        BT_HDR hdr;
        UINT16 SamplingFreq; /* 16k, 32k, 44.1k or 48k*/
        UINT8 ChannelMode; /* mono, dual, stereo or joint stereo*/
        UINT8 NumOfSubBands; /* 4 or 8 */
        UINT8 NumOfBlocks; /* 4, 8, 12 or 16*/
        UINT8 AllocationMethod; /* loudness or SNR*/
        UINT16 MtuSize; /* peer mtu size */
        UINT8 CodecType; /* SBC or Non-A2DP */
        UINT8 BluetoothVendorID; /* Bluetooth Vendor ID */
        UINT8 BluetoothCodecID; /* Bluetooth Codec ID */
} tBTIF_AVK_MEDIA_INIT_AUDIO;

#if (BTA_AV_INCLUDED == TRUE)

typedef struct
{
        BT_HDR hdr;
        UINT8 codec_info[AVDT_CODEC_SIZE];
} tBTIF_AVK_MEDIA_SINK_CFG_UPDATE;
#endif

#ifdef USE_AUDIO_TRACK
typedef enum {
        BTIF_AVK_MEDIA_FOCUS_IDLE = 0,
        BTIF_AVK_MEDIA_FOCUS_READY,
        BTIF_AVK_MEDIA_FOCUS_REQUESTED,
        BTIF_AVK_MEDIA_FOCUS_GRANTED
}btif_avk_media_audio_focus_state;

typedef struct
{
        BT_HDR hdr;
        UINT8 focus_state;
} tBTIF_AVK_MEDIA_SINK_FOCUS_UPDATE;
#endif

/*******************************************************************************
 **  Public functions
 *******************************************************************************/

/*******************************************************************************
 **
 ** Function         btif_avk_task
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
extern void btif_avk_media_task(void);

/*******************************************************************************
 **
 ** Function         btif_avk_media_task_stop_aa_req
 **
 ** Description      Request to stop audio encoding task
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
extern BOOLEAN btif_avk_media_task_stop_aa_req(void);

/*******************************************************************************
 **
 ** Function         btif_avk_media_task_aa_rx_flush_req
 **
 ** Description      Request to flush audio decoding pipe
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
extern BOOLEAN btif_avk_media_task_aa_rx_flush_req(void);

/*******************************************************************************
 **
 ** Function         btif_avk_media_sink_enque_buf
 **
 ** Description      This function is called by the av_co to fill A2DP Sink Queue
 **
 **
 ** Returns          size of the queue
 *******************************************************************************/
 UINT8 btif_avk_media_sink_enque_buf(BT_HDR *p_buf, BD_ADDR bd_addr, BOOLEAN rpt_delay);

//#endif

/**
 * Local adaptation helper functions between btif and media task
 */

bool btif_avk_a2dp_start_media_task(void);
void btif_avk_a2dp_stop_media_task(void);
bool btif_avk_a2dp_is_media_task_stopped(void);
void btif_avk_a2dp_on_init(void);
void btif_avk_a2dp_on_idle(void);
void btif_avk_a2dp_on_open(void);

void btif_avk_a2dp_ack_fail(void);
void btif_a2dp_on_stop_req(void);
void btif_avk_a2dp_on_stopped(tBTA_AVK_SUSPEND *p_av);
void btif_a2dp_on_suspend(void);
void btif_avk_a2dp_on_suspended(tBTA_AVK_SUSPEND *p_av);
void btif_avk_a2dp_set_tx_flush(BOOLEAN enable);
void btif_avk_a2dp_set_rx_flush(BOOLEAN enable);
void btif_avk_media_check_iop_exceptions(UINT8 *peer_bda);
void btif_avk_reset_decoder(UINT8 *p_av);

int btif_avk_a2dp_get_sbc_track_frequency(UINT8 frequency);
int btif_avk_a2dp_get_sbc_track_channel_count(UINT8 channeltype);
#if defined(AAC_DECODER_INCLUDED) && (AAC_DECODER_INCLUDED == TRUE)
int btif_avk_a2dp_get_aac_track_frequency(UINT16 frequency);
int btif_avk_a2dp_get_aac_track_channel_count(UINT8 channeltype);
#endif
#if defined(MP3_DECODER_INCLUDED) && (MP3_DECODER_INCLUDED == TRUE)
int btif_avk_a2dp_get_mp3_track_frequency(UINT16 frequency);
int btif_avk_a2dp_get_mp3_track_channel_count(UINT8 channeltype);
#endif
#if defined(APTX_CLASSIC_DECODER_INCLUDED) && (APTX_CLASSIC_DECODER_INCLUDED == TRUE)
int btif_avk_a2dp_get_aptx_track_frequency(UINT8 frequency);
int btif_avk_a2dp_get_aptx_track_channel_count(UINT8 channeltype);
#endif
void btif_avk_a2dp_set_peer_sep(UINT8 sep);
#ifdef USE_AUDIO_TRACK
void btif_avk_a2dp_set_audio_focus_state(btif_avk_media_audio_focus_state state);
#endif
#endif
