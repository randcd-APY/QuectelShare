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

/******************************************************************************
 **
 **  Name:          btif_avk_media_task.c
 **
 **  Description:   This is the multimedia module for the BTIF system.  It
 **                 contains task implementations AV, HS and HF profiles
 **                 audio & video processing
 **
 ******************************************************************************/
//#define BT_AUDIO_SYSTRACE_LOG
#ifdef BT_AUDIO_SYSTRACE_LOG
#define ATRACE_TAG ATRACE_TAG_ALWAYS
#endif

#define LOG_TAG "bt_btif_avk_media"

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>
#include <errno.h>
#include <dlfcn.h>

#include "bt_utils.h"
#include "bt_target.h"
#include "bta_api.h"
#include "btu.h"
#include "bta_sys.h"
#include "bta_sys_int.h"

#include "bta_avk_api.h"
#include "a2d_api.h"
#include "a2d_sbc.h"
#include "a2d_int.h"
#include "a2d_aptx.h"
#include "bta_avk_sbc.h"
#include "bta_avk_ci.h"
#include "l2c_api.h"

#include "btif_avk_co.h"
#include "btif_avk_media.h"

#include "osi/include/alarm.h"
#include "osi/include/log.h"
#include "osi/include/thread.h"
#include "osi/include/fixed_queue.h"
#include "btcore/include/bdaddr.h"

#if (BTA_AV_INCLUDED == TRUE)
#include "sbc_encoder.h"
#endif

#include <hardware/bluetooth.h>
#include "audio_a2dp_hw.h"
#include "btif_avk.h"
#include "btif_sm.h"
#include "btif_util.h"
#if (BTA_AV_SINK_INCLUDED == TRUE)
#include "oi_codec_sbc.h"
#include "oi_status.h"
#endif
#include "stdio.h"
#include <dlfcn.h>

#if (BTA_AV_SINK_INCLUDED == TRUE)
static OI_CODEC_SBC_DECODER_CONTEXT context;
static uint32_t contextData[CODEC_DATA_WORDS(2, SBC_CODEC_FAST_FILTER_BUFFERS)];
static int16_t pcmData[15*SBC_MAX_SAMPLES_PER_FRAME*SBC_MAX_CHANNELS];
OI_STATUS OI_CODEC_SBC_DecodeFrame(OI_CODEC_SBC_DECODER_CONTEXT* context,
                                   const OI_BYTE** frameData,
                                   uint32_t* frameBytes, int16_t* pcmData,
                                   uint32_t* pcmBytes);
OI_STATUS OI_CODEC_SBC_DecoderReset(OI_CODEC_SBC_DECODER_CONTEXT* context,
                                    uint32_t* decoderData,
                                    uint32_t decoderDataBytes,
                                    uint8_t maxChannels, uint8_t pcmStride,
                                    OI_BOOL enhanced);
#endif

#ifdef BT_AUDIO_SYSTRACE_LOG
#include <cutils/trace.h>
#define PERF_SYSTRACE 1
#endif

//#define DUMP_PCM_DATA TRUE
#if (defined(DUMP_PCM_DATA) && (DUMP_PCM_DATA == TRUE))
FILE *outputPcmSampleFile;
char outputFilename [50] = "/etc/bluetooth/output_sample.pcm";
#endif
/*****************************************************************************
 **  Constants
 *****************************************************************************/

#ifndef AUDIO_CHANNEL_OUT_MONO
#define AUDIO_CHANNEL_OUT_MONO 0x01
#endif

#ifndef AUDIO_CHANNEL_OUT_STEREO
#define AUDIO_CHANNEL_OUT_STEREO 0x03
#endif
#define A2DP_SINK_AUDIO_CODEC_PCM       0x40

/* BTIF media cmd event definition : BTIF_AVK_MEDIA_TASK_CMD */
enum
{
    BTIF_AVK_MEDIA_START_AA_TX = 1,
    BTIF_AVK_MEDIA_STOP_AA_TX,
    BTIF_AVK_MEDIA_AA_RX_RDY,
    BTIF_AVK_MEDIA_UIPC_RX_RDY,
    BTIF_AVK_MEDIA_SBC_ENC_INIT,
    BTIF_AVK_MEDIA_SBC_ENC_UPDATE,
    BTIF_AVK_MEDIA_SBC_DEC_INIT,
    BTIF_AVK_MEDIA_VIDEO_DEC_INIT,
    BTIF_AVK_MEDIA_FLUSH_AA_TX,
    BTIF_AVK_MEDIA_FLUSH_AA_RX,
    BTIF_AVK_MEDIA_AUDIO_FEEDING_INIT,
    BTIF_AVK_MEDIA_AUDIO_RECEIVING_INIT,
    BTIF_AVK_MEDIA_AUDIO_SINK_CFG_UPDATE,
    BTIF_AVK_MEDIA_AUDIO_SINK_CLEAR_TRACK,
    BTIF_AVK_MEDIA_AUDIO_SINK_DECODE_REQ,
    BTIF_AVK_MEDIA_AUDIO_SINK_FEED_AUDIO_HAL
#ifdef USE_AUDIO_TRACK
    ,BTIF_AVK_MEDIA_AUDIO_SINK_SET_FOCUS_STATE
#endif
};

enum {
    AVK_MEDIA_TASK_STATE_OFF = 0,
    AVK_MEDIA_TASK_STATE_ON = 1,
    AVK_MEDIA_TASK_STATE_SHUTTING_DOWN = 2
};

/* Macro to multiply the media task tick */
#ifndef BTIF_AVK_MEDIA_NUM_TICK
#define BTIF_AVK_MEDIA_NUM_TICK      1
#endif

/* Media task tick in milliseconds, must be set to multiple of
   (1000/TICKS_PER_SEC) (10) */

#define BTIF_AVK_MEDIA_TIME_TICK                     (20 * BTIF_AVK_MEDIA_NUM_TICK)
#define A2DP_DATA_READ_POLL_MS    (BTIF_AVK_MEDIA_TIME_TICK / 2)
#define BTIF_SINK_MEDIA_TIME_TICK                (20 * BTIF_AVK_MEDIA_NUM_TICK)


/* buffer pool */
#define BTIF_AVK_MEDIA_AA_POOL_ID GKI_POOL_ID_3
#define BTIF_AVK_MEDIA_AA_BUF_SIZE GKI_BUF3_SIZE

/* offset */
#if (BTA_AV_CO_CP_SCMS_T == TRUE)
#define BTIF_AVK_MEDIA_AA_SBC_OFFSET (AVDT_MEDIA_OFFSET + BTA_AVK_SBC_HDR_SIZE + 1)
#else
#define BTIF_AVK_MEDIA_AA_SBC_OFFSET (AVDT_MEDIA_OFFSET + BTA_AVK_SBC_HDR_SIZE)
#endif

#if (BTA_AV_CO_CP_SCMS_T == TRUE)
#define BTIF_AVK_MEDIA_AA_APTX_OFFSET (AVDT_MEDIA_OFFSET + 1)
#else
#define BTIF_AVK_MEDIA_AA_APTX_OFFSET (AVDT_MEDIA_OFFSET - AVDT_MEDIA_HDR_SIZE) //no RTP header for aptX classic
#endif
/* Define the bitrate step when trying to match bitpool value */
#ifndef BTIF_AVK_MEDIA_BITRATE_STEP
#define BTIF_AVK_MEDIA_BITRATE_STEP 5
#endif

/* Middle quality quality setting @ 44.1 khz */
#define DEFAULT_SBC_BITRATE 328

#ifndef BTIF_A2DP_NON_EDR_MAX_RATE
#define BTIF_A2DP_NON_EDR_MAX_RATE 229
#endif

#if (BTA_AV_CO_CP_SCMS_T == TRUE)
/* A2DP header will contain a CP header of size 1 */
#define A2DP_HDR_SIZE               2
#else
#define A2DP_HDR_SIZE               1
#endif
#define MAX_SBC_HQ_FRAME_SIZE_44_1  119
#define MAX_SBC_HQ_FRAME_SIZE_48    115

/* 2DH5 payload size (679 bytes) - (4 bytes L2CAP Header + 12 bytes AVDTP Header) */
#define MAX_2MBPS_AVDTP_MTU         663

#define USEC_PER_SEC 1000000L
#define TPUT_STATS_INTERVAL_US (3000*1000)

/*
 * CONGESTION COMPENSATION CTRL ::
 *
 * Thus setting controls how many buffers we will hold in media task
 * during temp link congestion. Together with the stack buffer queues
 * it controls much temporary a2dp link congestion we can
 * compensate for. It however also depends on the default run level of sinks
 * jitterbuffers. Depending on type of sink this would vary.
 * Ideally the (SRC) max tx buffer capacity should equal the sinks
 * jitterbuffer runlevel including any intermediate buffers on the way
 * towards the sinks codec.
 */

/* fixme -- define this in pcm time instead of buffer count */

/* The typical runlevel of the tx queue size is ~1 buffer
   but due to link flow control or thread preemption in lower
   layers we might need to temporarily buffer up data */
/* 18 frames is equivalent to 6.89*18*2.9 ~= 360 ms @ 44.1 khz, 20 ms mediatick */
#define MAX_OUTPUT_A2DP_FRAME_QUEUE_SZ 10
#ifndef MAX_PCM_FRAME_NUM_PER_TICK
#define MAX_PCM_FRAME_NUM_PER_TICK     14
#endif
#define MAX_PCM_ITER_NUM_PER_TICK     3

/* In case of A2DP SINK, we will delay start by 5 AVDTP Packets*/
#define MAX_A2DP_DELAYED_START_FRAME_COUNT 3
#define PACKET_PLAYED_PER_TICK_48 8
#define PACKET_PLAYED_PER_TICK_44 7
#define PACKET_PLAYED_PER_TICK_32 5
#define PACKET_PLAYED_PER_TICK_16 3

#define MAX_MEDIA_WORKQUEUE_COUNT 1024

typedef struct
{
    UINT16 num_frames_to_be_processed;
    UINT16 len;
    UINT16 offset;
    UINT16 layer_specific;
    BD_ADDR bd_addr;
    UINT64 enque_ms;   //time of packet enqueue RxSbcQ (millisecond)
} tBT_AVK_SBC_HDR;

typedef struct
{
    UINT32 aa_frame_counter;
    INT32  aa_feed_counter;
    INT32  aa_feed_residue;
    UINT32 counter;
    UINT32 bytes_per_tick;  /* pcm bytes read each media task tick */
} tBTIF_AVK_MEDIA_FEEDINGS_PCM_STATE;

typedef union
{
    tBTIF_AVK_MEDIA_FEEDINGS_PCM_STATE pcm;
} tBTIF_AVK_MEDIA_FEEDINGS_STATE;

typedef struct
{
#if (BTA_AV_INCLUDED == TRUE)
    fixed_queue_t  *RxSbcQ;
    BOOLEAN is_tx_timer;
    BOOLEAN is_rx_timer;
    UINT16 TxAaMtuSize;
    UINT32 timestamp;
    UINT8 TxTranscoding;
    tBTIF_AVK_FEEDING_MODE feeding_mode;
    tBTIF_AVK_MEDIA_FEEDINGS media_feeding;
    tBTIF_AVK_MEDIA_FEEDINGS_STATE media_feeding_state;
    SBC_ENC_PARAMS encoder;
    UINT16 offset;
    A2D_APTX_ENC_PARAMS aptxEncoderParams;
    UINT16 as16PcmBuffer[1024];
    UINT8 busy_level;
    void* av_sm_hdl;
    UINT8 a2dp_cmd_pending; /* we can have max one command pending */
    BOOLEAN tx_flush; /* discards any outgoing data when true */
    BOOLEAN rx_flush; /* discards any incoming data when true */
    UINT8 peer_sep;
    BOOLEAN data_channel_open;
    UINT32  a2dp_sink_pcm_buf_size; // should be equivalent to 40ms of data
    UINT8   frames_to_process;
    UINT8*  a2dp_sink_pcm_buf;
    UINT32  sample_rate;
    UINT8   channel_count;
#ifdef USE_AUDIO_TRACK
    btif_avk_media_audio_focus_state rx_audio_focus_state;
#endif
    alarm_t *media_alarm;
    alarm_t *decode_alarm;
    UINT8 TxNumSBCFrames;
#endif

} tBTIF_AVK_MEDIA_CB;

static UINT64 last_frame_us = 0;

static void btif_a2dp_data_cb(tUIPC_CH_ID ch_id, tUIPC_EVENT event);
static void btif_a2dp_ctrl_cb(tUIPC_CH_ID ch_id, tUIPC_EVENT event);
static void btif_avk_media_flush_q(fixed_queue_t  *p_q);
static void btif_avk_media_task_aa_handle_stop_decoding(void );
static void btif_avk_media_task_aa_rx_flush(void);

static UINT32 get_frame_length();
static UINT8 check_for_max_number_of_frames_per_packet();
static const char *dump_media_event(UINT16 event);
static void btif_avk_media_thread_init(void *context);
static void btif_avk_media_thread_cleanup(void *context);
static void btif_avk_media_thread_handle_cmd(fixed_queue_t *queue, void *context);

/* Handle incoming media packets A2DP SINK streaming*/
static void btif_avk_media_task_handle_inc_media(tBT_AVK_SBC_HDR*p_msg);
static void btif_avk_media_task_aa_handle_decoder_reset(BT_HDR *p_msg);
static void btif_avk_media_task_aa_handle_clear_track(void);
static void btif_avk_media_task_aa_handle_start_decoding(void);
static BOOLEAN btif_avk_media_task_clear_track(void);
static void btif_avk_media_task_avk_handle_timer(UNUSED_ATTR void *context);
BOOLEAN btif_avk_media_task_feed_audio_hal(void);


static tBTIF_AVK_MEDIA_CB btif_avk_media_cb;
static int avk_media_task_running = AVK_MEDIA_TASK_STATE_OFF;

static fixed_queue_t *btif_avk_media_cmd_msg_queue;
static thread_t *avk_worker_thread;

UNUSED_ATTR static const char *dump_media_event(UINT16 event)
{
    switch(event)
    {
        CASE_RETURN_STR(BTIF_AVK_MEDIA_START_AA_TX)
        CASE_RETURN_STR(BTIF_AVK_MEDIA_STOP_AA_TX)
        CASE_RETURN_STR(BTIF_AVK_MEDIA_AA_RX_RDY)
        CASE_RETURN_STR(BTIF_AVK_MEDIA_UIPC_RX_RDY)
        CASE_RETURN_STR(BTIF_AVK_MEDIA_SBC_ENC_INIT)
        CASE_RETURN_STR(BTIF_AVK_MEDIA_SBC_ENC_UPDATE)
        CASE_RETURN_STR(BTIF_AVK_MEDIA_SBC_DEC_INIT)
        CASE_RETURN_STR(BTIF_AVK_MEDIA_VIDEO_DEC_INIT)
        CASE_RETURN_STR(BTIF_AVK_MEDIA_FLUSH_AA_TX)
        CASE_RETURN_STR(BTIF_AVK_MEDIA_FLUSH_AA_RX)
        CASE_RETURN_STR(BTIF_AVK_MEDIA_AUDIO_FEEDING_INIT)
        CASE_RETURN_STR(BTIF_AVK_MEDIA_AUDIO_RECEIVING_INIT)
        CASE_RETURN_STR(BTIF_AVK_MEDIA_AUDIO_SINK_CFG_UPDATE)
        CASE_RETURN_STR(BTIF_AVK_MEDIA_AUDIO_SINK_CLEAR_TRACK)
        CASE_RETURN_STR(BTIF_AVK_MEDIA_AUDIO_SINK_DECODE_REQ)
        CASE_RETURN_STR(BTIF_AVK_MEDIA_AUDIO_SINK_FEED_AUDIO_HAL)
#ifdef USE_AUDIO_TRACK
        CASE_RETURN_STR(BTIF_AVK_MEDIA_AUDIO_SINK_SET_FOCUS_STATE)
#endif
        default:
            return "UNKNOWN MEDIA EVENT";
    }
}


/*****************************************************************************
 **  A2DP CTRL PATH
 *****************************************************************************/

static const char* dump_a2dp_ctrl_event(UINT8 event)
{
    switch(event)
    {
        CASE_RETURN_STR(A2DP_CTRL_CMD_NONE)
        CASE_RETURN_STR(A2DP_CTRL_CMD_CHECK_READY)
        CASE_RETURN_STR(A2DP_CTRL_CMD_START)
        CASE_RETURN_STR(A2DP_CTRL_CMD_STOP)
        CASE_RETURN_STR(A2DP_CTRL_CMD_SUSPEND)
        default:
            return "UNKNOWN MSG ID";
    }
}

static void btif_audiopath_detached(void)
{
    APPL_TRACE_IMP("## AUDIO PATH DETACHED ##");

    /*  send stop request only if we are actively streaming and haven't received
        a stop request. Potentially audioflinger detached abnormally */
    if (btif_avk_media_cb.is_tx_timer)
    {
        /* post stop event and wait for audio path to stop */
        btif_avk_dispatch_sm_event(BTIF_AVK_STOP_STREAM_REQ_EVT, NULL, 0);
    }
}

static void a2dp_cmd_acknowledge(int status)
{
    UINT8 ack = status;

    APPL_TRACE_IMP("## a2dp ack : %s, status %d ##",
          dump_a2dp_ctrl_event(btif_avk_media_cb.a2dp_cmd_pending), status);

    /* sanity check */
    if (btif_avk_media_cb.a2dp_cmd_pending == A2DP_CTRL_CMD_NONE)
    {
        APPL_TRACE_ERROR("warning : no command pending, ignore ack");
        return;
    }

    /* clear pending */
    btif_avk_media_cb.a2dp_cmd_pending = A2DP_CTRL_CMD_NONE;

    /* acknowledge start request */
    UIPC_AVK_Send(UIPC_CH_ID_AVK_CTRL, 0, &ack, 1);
}


static void btif_recv_ctrl_data(void)
{
    UINT8 cmd = 0;
    int n;
    n = UIPC_AVK_Read(UIPC_CH_ID_AVK_CTRL, NULL, &cmd, 1);

    /* detach on ctrl channel means audioflinger process was terminated */
    if (n == 0)
    {
        APPL_TRACE_IMP("CTRL CH DETACHED");
        UIPC_AVK_Close(UIPC_CH_ID_AVK_CTRL);
        /* we can operate only on datachannel, if af client wants to
           do send additional commands the ctrl channel would be reestablished */
        //btif_audiopath_detached();
        return;
    }

    APPL_TRACE_IMP(" Sink a2dp-ctrl-cmd : %s", dump_a2dp_ctrl_event(cmd));

    btif_avk_media_cb.a2dp_cmd_pending = cmd;

    switch(cmd)
    {
        case A2DP_CTRL_CMD_CHECK_READY:

            if (avk_media_task_running == AVK_MEDIA_TASK_STATE_SHUTTING_DOWN)
            {
                a2dp_cmd_acknowledge(A2DP_CTRL_ACK_FAILURE);
                return;
            }

            /* check whether av is ready to setup a2dp datapath */
            if ((btif_avk_stream_ready() == TRUE) || (btif_avk_stream_started_ready() == TRUE))
            {
                a2dp_cmd_acknowledge(A2DP_CTRL_ACK_SUCCESS);
            }
            else
            {
                a2dp_cmd_acknowledge(A2DP_CTRL_ACK_FAILURE);
            }
            break;

        case A2DP_CTRL_CMD_CHECK_STREAM_STARTED:

            if((btif_avk_stream_started_ready() == TRUE))
                a2dp_cmd_acknowledge(A2DP_CTRL_ACK_SUCCESS);
            else
                a2dp_cmd_acknowledge(A2DP_CTRL_ACK_FAILURE);
            break;

        case A2DP_CTRL_CMD_START:
            /* Don't sent START request to stack while we are in call.
               Some headsets like the Sony MW600, don't allow AVDTP START
               in call and respond BAD_STATE. */

            // TODO: SRC, check for hf_is_call_idle
            if (btif_avk_media_cb.is_tx_timer == TRUE)
            {
                APPL_TRACE_IMP("Unexpected HAL Start. Stream in started state, bail out");
                a2dp_cmd_acknowledge(A2DP_CTRL_ACK_FAILURE);
                break;
            }
#if (BTA_AV_SINK_INCLUDED == TRUE)
            /* If we are Sink, check if AVRCP Connection is there, send AVRCP_PLAY, AVDTP_START otherwise */
            if (btif_avk_media_cb.peer_sep == AVDT_TSEP_SRC)
            {
                UIPC_AVK_Open(UIPC_CH_ID_AVK_AUDIO, btif_a2dp_data_cb);
                btif_avk_dispatch_sm_event(BTIF_AVK_SINK_START_STREAM_REQ_EVT, NULL, 0);
                //acknowlwdge here itself, because sock client will wait for recv call
                a2dp_cmd_acknowledge(A2DP_CTRL_ACK_SUCCESS);
                break;
            }
#endif
            /* In Dual A2dp, first check for started state of stream
            * as we dont want to START again as while doing Handoff
            * the stack state will be started, so it is not needed
            * to send START again, just open the media socket
            * and ACK the audio HAL.*/
            if (btif_avk_stream_started_ready())
            {
                /* already started, setup audio data channel listener
                * and ack back immediately */
                UIPC_AVK_Open(UIPC_CH_ID_AVK_AUDIO, btif_a2dp_data_cb);

                a2dp_cmd_acknowledge(A2DP_CTRL_ACK_SUCCESS);
            }
            else if (btif_avk_stream_ready() == TRUE)
            {
                /* setup audio data channel listener */
                UIPC_AVK_Open(UIPC_CH_ID_AVK_AUDIO, btif_a2dp_data_cb);

                /* post start event and wait for audio path to open */
                if (btif_avk_media_cb.peer_sep == AVDT_TSEP_SNK)
                    btif_avk_dispatch_sm_event(BTIF_AVK_START_STREAM_REQ_EVT, NULL, 0);
            }
            else
            {
                a2dp_cmd_acknowledge(A2DP_CTRL_ACK_FAILURE);
                break;
            }
            break;

        case A2DP_CTRL_CMD_STOP:
            if (btif_avk_media_cb.peer_sep == AVDT_TSEP_SNK && btif_avk_media_cb.is_tx_timer == FALSE)
            {
                /* we are already stopped, just ack back */
                a2dp_cmd_acknowledge(A2DP_CTRL_ACK_SUCCESS);
                break;
            }

            btif_avk_dispatch_sm_event(BTIF_AVK_STOP_STREAM_REQ_EVT, NULL, 0);
            a2dp_cmd_acknowledge(A2DP_CTRL_ACK_SUCCESS);
            break;

        case A2DP_CTRL_CMD_SUSPEND:
            /* local suspend */
            if (btif_avk_stream_started_ready())
            {
                btif_avk_dispatch_sm_event(BTIF_AVK_SINK_SUSPEND_STREAM_REQ_EVT, NULL, 0);
            }
            else
            {
                /* if we are not in started state, just ack back ok and let
                   audioflinger close the channel. This can happen if we are
                   remotely suspended, clear REMOTE SUSPEND Flag */
                btif_avk_clear_remote_suspend_flag();
                a2dp_cmd_acknowledge(A2DP_CTRL_ACK_SUCCESS);
            }
            break;

        case A2DP_CTRL_GET_AUDIO_CONFIG:
        {
            uint32_t sample_rate = btif_avk_media_cb.sample_rate;
            uint8_t channel_count = btif_avk_media_cb.channel_count;

            a2dp_cmd_acknowledge(A2DP_CTRL_ACK_SUCCESS);
            UIPC_AVK_Send(UIPC_CH_ID_AVK_CTRL, 0, (UINT8 *)&sample_rate, 4);
            UIPC_AVK_Send(UIPC_CH_ID_AVK_CTRL, 0, &channel_count, 1);
            break;
        }

        default:
            APPL_TRACE_ERROR("UNSUPPORTED CMD (%d)", cmd);
            a2dp_cmd_acknowledge(A2DP_CTRL_ACK_FAILURE);
            break;
    }
    APPL_TRACE_IMP("Sink a2dp-ctrl-cmd : %s DONE", dump_a2dp_ctrl_event(cmd));
}

static void btif_a2dp_ctrl_cb(tUIPC_CH_ID ch_id, tUIPC_EVENT event)
{
    UNUSED(ch_id);

    APPL_TRACE_IMP("A2DP-CTRL-CHANNEL EVENT %s", dump_avk_uipc_event(event));

    switch(event)
    {
        case UIPC_AVK_Open_EVT:
            /* fetch av statemachine handle */
            btif_avk_media_cb.av_sm_hdl = btif_avk_get_sm_handle();
            break;

        case UIPC_AVK_Close_EVT:
            /* restart ctrl server unless we are shutting down */
            if (avk_media_task_running == AVK_MEDIA_TASK_STATE_ON)
                UIPC_AVK_Open(UIPC_CH_ID_AVK_CTRL , btif_a2dp_ctrl_cb);
            break;

        case UIPC_AVK_RX_DATA_READY_EVT:
            btif_recv_ctrl_data();
            break;

        default :
            APPL_TRACE_ERROR("### A2DP-CTRL-CHANNEL EVENT %d NOT HANDLED ###", event);
            break;
    }
}

static void btif_a2dp_data_cb(tUIPC_CH_ID ch_id, tUIPC_EVENT event)
{
    UNUSED(ch_id);

    APPL_TRACE_DEBUG("BTIF MEDIA (A2DP-DATA) EVENT %s", dump_avk_uipc_event(event));

    switch(event)
    {
        case UIPC_AVK_Open_EVT:

            /*  read directly from media task from here on (keep callback for
                connection events */
            UIPC_AVK_Ioctl(UIPC_CH_ID_AVK_AUDIO, UIPC_REG_REMOVE_ACTIVE_READSET, NULL);
            UIPC_AVK_Ioctl(UIPC_CH_ID_AVK_AUDIO, UIPC_SET_READ_POLL_TMO,
                       (void *)A2DP_DATA_READ_POLL_MS);

            btif_avk_media_cb.data_channel_open = TRUE;

            /* ack back when media task is fully started */
            break;

        case UIPC_AVK_Close_EVT:
            a2dp_cmd_acknowledge(A2DP_CTRL_ACK_SUCCESS);
            btif_audiopath_detached();
            btif_avk_media_cb.data_channel_open = FALSE;
            break;

        default :
            APPL_TRACE_ERROR("### A2DP-DATA EVENT %d NOT HANDLED ###", event);
            break;
    }
}

/*****************************************************************************
 **  BTIF ADAPTATION
 *****************************************************************************/

static UINT16 btif_avk_media_task_get_sbc_rate(void)
{
    UINT16 rate = DEFAULT_SBC_BITRATE;

    /* restrict bitrate if a2dp link is non-edr */
    if (!btif_avk_is_peer_edr())
    {
        rate = BTIF_A2DP_NON_EDR_MAX_RATE;
        APPL_TRACE_DEBUG("non-edr a2dp sink detected, restrict rate to %d", rate);
    }

    return rate;
}


bool btif_avk_a2dp_is_media_task_stopped(void)
{
    if (avk_media_task_running != AVK_MEDIA_TASK_STATE_OFF)
    {
        APPL_TRACE_ERROR("btif_avk_a2dp_is_media_task_stopped: %d",
                                            avk_media_task_running);
        return false;
    }
    return true;
}

bool btif_avk_a2dp_start_media_task(void)
{
    APPL_TRACE_IMP("## A2DP START MEDIA THREAD ##");
    if (avk_media_task_running != AVK_MEDIA_TASK_STATE_OFF)
    {
        APPL_TRACE_ERROR("warning : media task state: %d",
                                            avk_media_task_running);
        return false;
    }

    btif_avk_media_cmd_msg_queue = fixed_queue_new(SIZE_MAX);
    if (btif_avk_media_cmd_msg_queue == NULL)
        goto error_exit;

    /* start a2dp media task */
    avk_worker_thread = thread_new_sized("media_worker", MAX_MEDIA_WORKQUEUE_COUNT);
    if (avk_worker_thread == NULL)
        goto error_exit;

    fixed_queue_register_dequeue(btif_avk_media_cmd_msg_queue,
        thread_get_reactor(avk_worker_thread),
        btif_avk_media_thread_handle_cmd,
        NULL);

    thread_post(avk_worker_thread, btif_avk_media_thread_init, NULL);

    btif_avk_media_cb.is_tx_timer = FALSE;
    btif_avk_media_cb.a2dp_sink_pcm_buf = NULL;
    btif_avk_media_cb.a2dp_sink_pcm_buf_size = 0;

    APPL_TRACE_IMP("## A2DP MEDIA THREAD STARTED ##");

    return true;

 error_exit:;
    APPL_TRACE_ERROR("%s unable to start up media thread", __func__);
    return false;
}

void btif_avk_a2dp_stop_media_task(void)
{
    APPL_TRACE_IMP("## A2DP STOP MEDIA THREAD ##");
    if (avk_media_task_running != AVK_MEDIA_TASK_STATE_ON)
    {
        APPL_TRACE_ERROR("warning: media task cleanup state: %d",
                                        avk_media_task_running);
        return;
    }
    /* make sure no channels are restarted while shutting down */
    avk_media_task_running = AVK_MEDIA_TASK_STATE_SHUTTING_DOWN;

    // Stop timer
    alarm_free(btif_avk_media_cb.media_alarm);
    btif_avk_media_cb.media_alarm = NULL;
    btif_avk_media_cb.is_tx_timer = FALSE;

    // Exit thread
    fixed_queue_free(btif_avk_media_cmd_msg_queue, NULL);
    thread_post(avk_worker_thread, btif_avk_media_thread_cleanup, NULL);
    thread_free(avk_worker_thread);

    avk_worker_thread = NULL;
    btif_avk_media_cmd_msg_queue = NULL;
    APPL_TRACE_IMP("## A2DP MEDIA THREAD STOPPED ##");
}

/*****************************************************************************
**
** Function        btif_avk_a2dp_on_init
**
** Description
**
** Returns
**
*******************************************************************************/

void btif_avk_a2dp_on_init(void)
{
    //tput_mon(1, 0, 1);
}

/*****************************************************************************
**
** Function        btif_avk_a2dp_on_idle
**
** Description
**
** Returns
**
*******************************************************************************/

void btif_avk_a2dp_on_idle(void)
{
    APPL_TRACE_IMP("## ON A2DP IDLE ##");
    bta_avk_co_init();
#if (BTA_AV_SINK_INCLUDED == TRUE)
    if (btif_avk_media_cb.peer_sep == AVDT_TSEP_SRC)
    {
        btif_avk_media_cb.rx_flush = TRUE;
        btif_avk_media_cb.a2dp_sink_pcm_buf_size = 0;
        if(btif_avk_media_cb.a2dp_sink_pcm_buf != NULL)
            osi_free(btif_avk_media_cb.a2dp_sink_pcm_buf);
        btif_avk_media_cb.a2dp_sink_pcm_buf = NULL;
        btif_avk_media_task_aa_rx_flush_req();
        //btif_avk_media_task_aa_handle_stop_decoding();
        btif_avk_media_task_clear_track();
#ifdef USE_AUDIO_TRACK
        btif_avk_media_cb.rx_audio_focus_state = BTIF_AVK_MEDIA_FOCUS_IDLE;
#endif
        APPL_TRACE_DEBUG("Stopped BT track");
    }
#endif
}

/*****************************************************************************
**
** Function        btif_avk_a2dp_on_open
**
** Description
**
** Returns
**
*******************************************************************************/

void btif_avk_a2dp_on_open(void)
{
    APPL_TRACE_IMP("## ON A2DP OPEN ##");

    /* always use callback to notify socket events */
    UIPC_AVK_Open(UIPC_CH_ID_AVK_AUDIO, btif_a2dp_data_cb);
}

/*******************************************************************************
 **
 ** Function         btif_avk_media_task_clear_track
 **
 ** Description
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
static BOOLEAN btif_avk_media_task_clear_track(void)
{
    BT_HDR *p_buf;

    if (NULL == (p_buf = osi_malloc(sizeof(BT_HDR))))
    {
        return FALSE;
    }

    p_buf->event = BTIF_AVK_MEDIA_AUDIO_SINK_CLEAR_TRACK;

    if (btif_avk_media_cmd_msg_queue != NULL)
        fixed_queue_enqueue(btif_avk_media_cmd_msg_queue, p_buf);
    return TRUE;
}

/*****************************************************************************
**
** Function        btif_avk_reset_decoder
**
** Description
**
** Returns
**
*******************************************************************************/

void btif_avk_reset_decoder(UINT8 *p_av)
{
    APPL_TRACE_EVENT("btif_avk_reset_decoder");
    APPL_TRACE_DEBUG("btif_avk_reset_decoder p_codec_info[%x:%x:%x:%x:%x:%x]",
            p_av[1], p_av[2], p_av[3],
            p_av[4], p_av[5], p_av[6]);

    tBTIF_AVK_MEDIA_SINK_CFG_UPDATE *p_buf;
    if (NULL == (p_buf = osi_malloc(sizeof(tBTIF_AVK_MEDIA_SINK_CFG_UPDATE))))
    {
        APPL_TRACE_EVENT("btif_avk_reset_decoder No Buffer ");
        return;
    }

    memcpy(p_buf->codec_info,p_av, AVDT_CODEC_SIZE);
    p_buf->hdr.event = BTIF_AVK_MEDIA_AUDIO_SINK_CFG_UPDATE;

    if (btif_avk_media_cmd_msg_queue != NULL)
        fixed_queue_enqueue(btif_avk_media_cmd_msg_queue, p_buf);
}


/*****************************************************************************
**
** Function        btif_avk_a2dp_ack_fail
**
** Description
**
** Returns
**
*******************************************************************************/

void btif_avk_a2dp_ack_fail(void)
{
    APPL_TRACE_IMP("## A2DP_CTRL_ACK_FAILURE ##");
    a2dp_cmd_acknowledge(A2DP_CTRL_ACK_FAILURE);
}

/*****************************************************************************
**
** Function        btif_avk_a2dp_on_stopped
**
** Description
**
** Returns
**
*******************************************************************************/

void btif_avk_a2dp_on_stopped(tBTA_AVK_SUSPEND *p_av)
{
    APPL_TRACE_IMP("## ON A2DP STOPPED ##");
    if (btif_avk_media_cb.peer_sep == AVDT_TSEP_SRC) /*  Handling for A2DP SINK cases*/
    {
        btif_avk_media_cb.rx_flush = TRUE;
        btif_avk_media_task_aa_rx_flush_req();
        //btif_avk_media_task_aa_handle_stop_decoding();
#ifndef USE_AUDIO_TRACK
        UIPC_AVK_Close(UIPC_CH_ID_AVK_AUDIO);
#endif
        btif_avk_media_cb.data_channel_open = FALSE;
        return;
    }
    /* allow using this api for other than suspend */
    if (p_av != NULL)
    {
        if (p_av->status != BTA_AVK_SUCCESS)
        {
            APPL_TRACE_EVENT("AV STOP FAILED (%d)", p_av->status);

            if (p_av->initiator)
                a2dp_cmd_acknowledge(A2DP_CTRL_ACK_FAILURE);
            return;
        }
    }

    /* ensure tx frames are immediately suspended */
    btif_avk_media_cb.tx_flush = 1;

}


/*****************************************************************************
**
** Function        btif_avk_a2dp_on_suspended
**
** Description
**
** Returns
**
*******************************************************************************/

void btif_avk_a2dp_on_suspended(tBTA_AVK_SUSPEND *p_av)
{
    APPL_TRACE_IMP("## ON A2DP SUSPENDED ##");
    if (btif_avk_media_cb.peer_sep == AVDT_TSEP_SRC)
    {
        btif_avk_media_cb.rx_flush = TRUE;
        btif_avk_media_task_aa_rx_flush_req();
        //btif_avk_media_task_aa_handle_stop_decoding();
        if (p_av->status == BTA_AVK_SUCCESS)
            a2dp_cmd_acknowledge(A2DP_CTRL_ACK_SUCCESS);
        else
            a2dp_cmd_acknowledge(A2DP_CTRL_ACK_FAILURE);
        return;
    }

    /* check for status failures */
    if (p_av->status != BTA_AVK_SUCCESS)
    {
        if (p_av->initiator == TRUE)
            a2dp_cmd_acknowledge(A2DP_CTRL_ACK_FAILURE);
    }

    /* once stream is fully stopped we will ack back */

    /* ensure tx frames are immediately flushed */
    btif_avk_media_cb.tx_flush = 1;

}

/* when true media task discards any rx frames */
void btif_avk_a2dp_set_rx_flush(BOOLEAN enable)
{
    APPL_TRACE_EVENT("## DROP RX %d ##", enable);
    btif_avk_media_cb.rx_flush = enable;
}

/* when true media task discards any tx frames */
void btif_avk_a2dp_set_tx_flush(BOOLEAN enable)
{
    APPL_TRACE_EVENT("## DROP TX %d ##", enable);
    btif_avk_media_cb.tx_flush = enable;
}
#ifdef USE_AUDIO_TRACK
void btif_avk_a2dp_set_audio_focus_state(btif_avk_media_audio_focus_state state)
{
    APPL_TRACE_EVENT("btif_avk_a2dp_set_audio_focus_state");
    tBTIF_AVK_MEDIA_SINK_FOCUS_UPDATE *p_buf;
    if (NULL == (p_buf = osi_malloc(sizeof(tBTIF_AVK_MEDIA_SINK_FOCUS_UPDATE))))
    {
        APPL_TRACE_EVENT("btif_avk_a2dp_set_audio_focus_state No Buffer ");
        return;
    }

    p_buf->focus_state = state;
    p_buf->hdr.event = BTIF_AVK_MEDIA_AUDIO_SINK_SET_FOCUS_STATE;
    if (btif_avk_media_cmd_msg_queue != NULL)
        fixed_queue_enqueue(btif_avk_media_cmd_msg_queue, p_buf);
}
#endif
#if (BTA_AV_SINK_INCLUDED == TRUE)
static void btif_avk_media_task_avk_handle_timer(UNUSED_ATTR void *context)
{
    tBT_AVK_SBC_HDR *p_msg;
    int num_sbc_frames;
    int num_frames_to_process;

    if (!fixed_queue_length(btif_avk_media_cb.RxSbcQ))
    {
        APPL_TRACE_DEBUG("  QUE  EMPTY ");
    }
    else
    {
#ifdef USE_AUDIO_TRACK
        switch(btif_avk_media_cb.rx_audio_focus_state)
        {
            /* Don't Do anything in case of Idle, Requested */
            case BTIF_AVK_MEDIA_FOCUS_REQUESTED:
            case BTIF_AVK_MEDIA_FOCUS_IDLE:
                return;
            break;
            /* In case of Ready, request for focus and wait to move in granted */
            case BTIF_AVK_MEDIA_FOCUS_READY:
                btif_avk_queue_focus_rquest();
                btif_avk_media_cb.rx_audio_focus_state = BTIF_AVK_MEDIA_FOCUS_REQUESTED;
                return;
            break;
            /* play only in this case */
            case BTIF_AVK_MEDIA_FOCUS_GRANTED:
            break;
        }
#endif
        if (btif_avk_media_cb.rx_flush == TRUE)
        {
            btif_avk_media_flush_q(btif_avk_media_cb.RxSbcQ);
            return;
        }

        num_frames_to_process = btif_avk_media_cb.frames_to_process;
        APPL_TRACE_DEBUG(" Process Frames + ");

        do
        {
            p_msg = (tBT_AVK_SBC_HDR *)fixed_queue_try_peek_first(btif_avk_media_cb.RxSbcQ);
            if (p_msg == NULL)
                return;
            num_sbc_frames  = p_msg->num_frames_to_be_processed; /* num of frames in Que Packets */
            APPL_TRACE_DEBUG(" Frames left in topmost packet %d", num_sbc_frames);
            APPL_TRACE_DEBUG(" Remaining frames to process in tick %d", num_frames_to_process);
            APPL_TRACE_DEBUG(" Num of Packets in Que %d", fixed_queue_length(btif_avk_media_cb.RxSbcQ));

            if ( num_sbc_frames > num_frames_to_process) /*  Que Packet has more frames*/
            {
                 p_msg->num_frames_to_be_processed= num_frames_to_process;
                 btif_avk_media_task_handle_inc_media(p_msg);
                 p_msg->num_frames_to_be_processed = num_sbc_frames - num_frames_to_process;
                 num_frames_to_process = 0;
                 break;
            }
            else                                        /*  Que packet has less frames */
            {
                btif_avk_media_task_handle_inc_media(p_msg);
                p_msg = (tBT_AVK_SBC_HDR *)fixed_queue_try_dequeue(btif_avk_media_cb.RxSbcQ);
                if( p_msg == NULL )
                {
                     APPL_TRACE_ERROR("Insufficient data in que ");
                     break;
                }
                num_frames_to_process = num_frames_to_process - p_msg->num_frames_to_be_processed;
                osi_free(p_msg);
            }
        }while(num_frames_to_process > 0);

        APPL_TRACE_DEBUG(" Process Frames - ");
    }
}
#else
static void btif_avk_media_task_avk_handle_timer(UNUSED_ATTR void *context) {}
#endif


#if (BTA_AV_INCLUDED == TRUE)
static void btif_avk_media_task_aa_handle_uipc_rx_rdy(void)
{
    /* send it */
    APPL_TRACE_IMP("btif_avk_media_task_aa_handle_uipc_rx_rdy calls bta_avk_ci_src_data_ready");
    bta_avk_ci_src_data_ready(BTA_AVK_CHNL_AUDIO);
}
#endif

static void btif_avk_media_thread_init(UNUSED_ATTR void *context) {
  APPL_TRACE_IMP(" btif_avk_media_thread_init");
  memset(&btif_avk_media_cb, 0, sizeof(btif_avk_media_cb));
  UIPC_AVK_Init(NULL);

#if (BTA_AV_INCLUDED == TRUE)
  btif_avk_media_cb.RxSbcQ = fixed_queue_new(SIZE_MAX);
  UIPC_AVK_Open(UIPC_CH_ID_AVK_CTRL , btif_a2dp_ctrl_cb);
#endif

  raise_priority_a2dp(TASK_HIGH_MEDIA);
  avk_media_task_running = AVK_MEDIA_TASK_STATE_ON;
  APPL_TRACE_IMP(" btif_avk_media_thread_init complete");
}

static void btif_avk_media_thread_cleanup(UNUSED_ATTR void *context) {
  APPL_TRACE_IMP(" btif_avk_media_thread_cleanup");

#if (BTA_AV_INCLUDED == TRUE)
  fixed_queue_free(btif_avk_media_cb.RxSbcQ,NULL);
  btif_avk_media_cb.RxSbcQ = NULL;
#endif

  /* this calls blocks until uipc is fully closed */
  UIPC_AVK_Close(UIPC_CH_ID_ALL);

  /* Clear media task flag */
  avk_media_task_running = AVK_MEDIA_TASK_STATE_OFF;
  APPL_TRACE_IMP(" btif_avk_media_thread_cleanup complete");
}

/*******************************************************************************
 **
 ** Function         btif_avk_media_task_send_cmd_evt
 **
 ** Description
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
static BOOLEAN btif_avk_media_task_send_cmd_evt(UINT16 Evt)
{
    BT_HDR *p_buf;
    if (NULL == (p_buf = osi_malloc(sizeof(BT_HDR))))
    {
        return FALSE;
    }

    p_buf->event = Evt;

    if (btif_avk_media_cmd_msg_queue != NULL)
        fixed_queue_enqueue(btif_avk_media_cmd_msg_queue, p_buf);
    return TRUE;
}

/*******************************************************************************
 **
 ** Function         btif_avk_media_flush_q
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btif_avk_media_flush_q(fixed_queue_t  *p_q)
{
    while (! fixed_queue_is_empty(p_q))
    {
        osi_free(fixed_queue_try_dequeue(p_q));
    }
}

static void btif_avk_media_thread_handle_cmd(fixed_queue_t *queue, UNUSED_ATTR void *context)
{
    UINT32 size;
    BT_HDR *p_msg = (BT_HDR *)fixed_queue_dequeue(queue);
    if(p_msg == NULL)
        return;
    APPL_TRACE_IMP("btif_avk_media_thread_handle_cmd : %d %s", p_msg->event,
             dump_media_event(p_msg->event));

    switch (p_msg->event)
    {
#if (BTA_AV_INCLUDED == TRUE)
    case BTIF_AVK_MEDIA_UIPC_RX_RDY:
        btif_avk_media_task_aa_handle_uipc_rx_rdy();
        break;
#ifdef USE_AUDIO_TRACK
    case BTIF_AVK_MEDIA_AUDIO_SINK_SET_FOCUS_STATE:
        if(!btif_avk_is_connected())
            break;
        btif_avk_media_cb.rx_audio_focus_state = ((tBTIF_AVK_MEDIA_SINK_FOCUS_UPDATE *)p_msg)->focus_state;
        APPL_TRACE_DEBUG("Setting focus state to %d ",btif_avk_media_cb.rx_audio_focus_state);
        break;
#endif
    case BTIF_AVK_MEDIA_AUDIO_SINK_CFG_UPDATE:
#if (BTA_AV_SINK_INCLUDED == TRUE)
        btif_avk_media_task_aa_handle_decoder_reset(p_msg);
#endif
        break;
    case BTIF_AVK_MEDIA_AUDIO_SINK_CLEAR_TRACK:
#if (BTA_AV_SINK_INCLUDED == TRUE)
        btif_avk_media_task_aa_handle_clear_track();
#endif
        break;
     case BTIF_AVK_MEDIA_FLUSH_AA_RX:
        btif_avk_media_task_aa_rx_flush();
        break;
     case BTIF_AVK_MEDIA_AUDIO_SINK_DECODE_REQ:
        btif_avk_media_task_decode();
        break;
    case BTIF_AVK_MEDIA_AUDIO_SINK_FEED_AUDIO_HAL:
       // get data from PCM que, and send to UIPC
       size = btif_avk_media_fetch_pcm_data(A2DP_SINK_AUDIO_CODEC_PCM, btif_avk_media_cb.a2dp_sink_pcm_buf,
               btif_avk_media_cb.a2dp_sink_pcm_buf_size);
       UIPC_AVK_Send(UIPC_CH_ID_AVK_AUDIO, 0, (UINT8 *)btif_avk_media_cb.a2dp_sink_pcm_buf, size);
       break;
#endif
    default:
        APPL_TRACE_ERROR("ERROR in %s unknown event %d", __func__, p_msg->event);
    }
    osi_free(p_msg);
    APPL_TRACE_IMP("%s:DONE", __func__);
}

#if (BTA_AV_SINK_INCLUDED == TRUE)

/*******************************************************************************
 **
 ** Function         btif_avk_media_task_handle_inc_media
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btif_avk_media_task_handle_inc_media(tBT_AVK_SBC_HDR*p_msg)
{
    uint8_t *sbc_start_frame = ((uint8_t*)(p_msg + 1) + p_msg->offset + 1);
    bdstr_t addr1;
    int count;
    uint32_t pcmBytes, availPcmBytes;
    int16_t *pcmDataPointer = pcmData; /*Will be overwritten on next packet receipt*/
    OI_STATUS status;
    int num_sbc_frames = p_msg->num_frames_to_be_processed;
    uint32_t sbc_frame_len = p_msg->len - 1;
    availPcmBytes = sizeof(pcmData);
    BD_ADDR bd_addr;

    memcpy(bd_addr, p_msg->bd_addr, sizeof(BD_ADDR));
#ifdef USE_AUDIO_TRACK
    int retwriteAudioTrack = 0;
#endif
    if ((btif_avk_media_cb.peer_sep == AVDT_TSEP_SNK) || (btif_avk_media_cb.rx_flush))
    {
        APPL_TRACE_DEBUG(" State Changed happened in this tick ");
        return;
    }
#ifndef USE_AUDIO_TRACK
    // ignore data if no one is listening
    if (!btif_avk_media_cb.data_channel_open)
    {
        APPL_TRACE_ERROR(" btif_avk_media_task_handle_inc_media Channel not open, returning");
        return;
    }
#endif
    APPL_TRACE_DEBUG("Number of sbc frames %d, frame_len %d bd_addr = %s",
            num_sbc_frames, sbc_frame_len,
            bdaddr_to_string((bt_bdaddr_t *)bd_addr, &addr1, sizeof(addr1)));

    for(count = 0; count < num_sbc_frames && sbc_frame_len != 0; count ++)
    {
        pcmBytes = availPcmBytes;
        status = OI_CODEC_SBC_DecodeFrame(&context, (const OI_BYTE**)&sbc_start_frame,
                                                        (uint32_t *)&sbc_frame_len,
                                                        (int16_t *)pcmDataPointer,
                                                        (uint32_t *)&pcmBytes);

        if (!OI_SUCCESS(status)) {
            APPL_TRACE_ERROR("Decoding failure: %d\n", status);
            break;
        }
        availPcmBytes -= pcmBytes;
        pcmDataPointer += pcmBytes/2;
        p_msg->offset += (p_msg->len - 1) - sbc_frame_len;
        p_msg->len = sbc_frame_len + 1;
    }

#ifdef USE_AUDIO_TRACK
#ifdef ANDROID
    retwriteAudioTrack = btWriteData((void*)pcmData, (sizeof(pcmData) - availPcmBytes));
#endif
    APPL_TRACE_ERROR("calling btif_media_enque_sink_data");
    btif_media_enque_sink_data(A2DP_SINK_AUDIO_CODEC_PCM,
            (void*)pcmData, (sizeof(pcmData) - availPcmBytes), bd_addr, p_msg->enque_ms);
    if(btif_avk_media_cb.data_channel_open) {
       APPL_TRACE_ERROR("Feeding to audio HAL");
        btif_avk_media_task_feed_audio_hal();
    }
#else
    //UIPC_AVK_Send(UIPC_CH_ID_AVK_AUDIO, 0, (UINT8 *)pcmData, (sizeof(pcmData) - availPcmBytes));
#endif
    APPL_TRACE_LATENCY_AUDIO("Written to audio, seq number %d", p_msg->layer_specific);
}
#endif

#if (BTA_AV_INCLUDED == TRUE)
/*******************************************************************************
 **
 ** Function         btif_avk_media_task_decode_req
 **
 ** Description
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
static BOOLEAN btif_avk_media_task_decode_req(void)
{
    BT_HDR *p_buf;

    if (NULL == (p_buf = osi_malloc(sizeof(BT_HDR))))
    {
        return FALSE;
    }

    p_buf->event = BTIF_AVK_MEDIA_AUDIO_SINK_DECODE_REQ;

    if (btif_avk_media_cmd_msg_queue != NULL)
        fixed_queue_enqueue(btif_avk_media_cmd_msg_queue, p_buf);
    return TRUE;
}
/*******************************************************************************
 **
 ** Function         btif_avk_media_task_feed_audio_hal
 **
 ** Description
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
BOOLEAN btif_avk_media_task_feed_audio_hal(void)
{
    BT_HDR *p_buf;

    if (NULL == (p_buf = osi_malloc(sizeof(BT_HDR))))
    {
        return FALSE;
    }

    p_buf->event = BTIF_AVK_MEDIA_AUDIO_SINK_FEED_AUDIO_HAL;

    if (btif_avk_media_cmd_msg_queue != NULL)
        fixed_queue_enqueue(btif_avk_media_cmd_msg_queue, p_buf);
    return TRUE;
}


/*******************************************************************************
 **
 ** Function         btif_avk_media_task_aa_rx_flush_req
 **
 ** Description
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
BOOLEAN btif_avk_media_task_aa_rx_flush_req(void)
{
    BT_HDR *p_buf;

    if (fixed_queue_is_empty(btif_avk_media_cb.RxSbcQ)== TRUE) /*  Que is already empty */
        return TRUE;

    if (NULL == (p_buf = osi_malloc(sizeof(BT_HDR))))
    {
        return FALSE;
    }

    p_buf->event = BTIF_AVK_MEDIA_FLUSH_AA_RX;

    if (btif_avk_media_cmd_msg_queue != NULL)
        fixed_queue_enqueue(btif_avk_media_cmd_msg_queue, p_buf);
    return TRUE;
}

/*******************************************************************************
 **
 ** Function         btif_avk_media_task_decode
 **
 ** Description
 **
 ** Returns          TRUE is success
 **
 *******************************************************************************/
void btif_avk_media_task_decode(void)
{
    tBT_AVK_SBC_HDR *p_msg;
    int num_sbc_frames;
    int num_frames_to_process;
    APPL_TRACE_DEBUG("%s: rs_flush = %d",__func__,btif_avk_media_cb.rx_flush);

    if(fixed_queue_is_empty(btif_avk_media_cb.RxSbcQ)) {
        APPL_TRACE_DEBUG("  QUE  EMPTY ");
        return;
    }
    if (btif_avk_media_cb.rx_flush == TRUE)
    {
        btif_avk_media_flush_q(btif_avk_media_cb.RxSbcQ);
        return;
    }
    p_msg = (tBT_AVK_SBC_HDR *)fixed_queue_try_dequeue(btif_avk_media_cb.RxSbcQ);
    if (p_msg == NULL)
        return;
    btif_avk_media_task_handle_inc_media(p_msg);
    osi_free(p_msg);
}
/*******************************************************************************
 **
 ** Function         btif_avk_media_task_aa_rx_flush
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btif_avk_media_task_aa_rx_flush(void)
{
    /* Flush all enqueued GKI SBC  buffers (encoded) */
    APPL_TRACE_DEBUG("btif_avk_media_task_aa_rx_flush");

    btif_avk_media_flush_q(btif_avk_media_cb.RxSbcQ);
}


int btif_a2dp_get_sbc_track_frequency(UINT8 frequency) {
    int freq = 48000;
    switch (frequency) {
        case A2D_SBC_IE_SAMP_FREQ_16:
            freq = 16000;
            break;
        case A2D_SBC_IE_SAMP_FREQ_32:
            freq = 32000;
            break;
        case A2D_SBC_IE_SAMP_FREQ_44:
            freq = 44100;
            break;
        case A2D_SBC_IE_SAMP_FREQ_48:
            freq = 48000;
            break;
    }
    return freq;
}

int btif_a2dp_get_sbc_track_channel_count(UINT8 channeltype) {
    int count = 1;
    switch (channeltype) {
        case A2D_SBC_IE_CH_MD_MONO:
            count = 1;
            break;
        case A2D_SBC_IE_CH_MD_DUAL:
        case A2D_SBC_IE_CH_MD_STEREO:
        case A2D_SBC_IE_CH_MD_JOINT:
            count = 2;
            break;
    }
    return count;
}
#ifdef USE_AUDIO_TRACK
static int btif_avk_a2dp_get_track_channel_type(UINT8 channeltype) {
        int count = 1;
        switch (channeltype) {
            case A2D_SBC_IE_CH_MD_MONO:
                count = 1;
                break;
            case A2D_SBC_IE_CH_MD_DUAL:
            case A2D_SBC_IE_CH_MD_STEREO:
            case A2D_SBC_IE_CH_MD_JOINT:
                count = 3;
                break;
        }
        return count;
    }
#endif

void btif_avk_a2dp_set_peer_sep(UINT8 sep) {
    btif_avk_media_cb.peer_sep = sep;
}

static void btif_decode_alarm_cb(UNUSED_ATTR void *context) {
    if(avk_worker_thread != NULL)
        thread_post(avk_worker_thread, btif_avk_media_task_avk_handle_timer, NULL);
}


static void btif_avk_media_task_aa_handle_stop_decoding(void) {
  alarm_free(btif_avk_media_cb.decode_alarm);
  btif_avk_media_cb.decode_alarm = NULL;
#ifdef USE_AUDIO_TRACK
#ifdef ANDROID
  btPauseTrack();
#endif
#endif
}

static void btif_avk_media_task_aa_handle_start_decoding(void) {
  if (btif_avk_media_cb.decode_alarm)
    return;
#ifdef USE_AUDIO_TRACK
#ifdef ANDROID
  btStartTrack();
#endif
#endif
  btif_avk_media_cb.decode_alarm = alarm_new_periodic("btif.media_decode");
  if (!btif_avk_media_cb.decode_alarm) {
    LOG_ERROR("%s unable to allocate decode alarm.", __func__);
    return;
  }

  alarm_set(btif_avk_media_cb.decode_alarm, BTIF_SINK_MEDIA_TIME_TICK, btif_decode_alarm_cb, NULL);
}

#if (BTA_AV_SINK_INCLUDED == TRUE)

static void btif_avk_media_task_aa_handle_clear_track (void)
{
    APPL_TRACE_DEBUG("btif_avk_media_task_aa_handle_clear_track");
#ifdef USE_AUDIO_TRACK
#ifdef ANDROID
    btStopTrack();
    btDeleteTrack();
#endif
#if (defined(DUMP_PCM_DATA) && (DUMP_PCM_DATA == TRUE))
    if (outputPcmSampleFile)
    {
        fclose(outputPcmSampleFile);
    }
    outputPcmSampleFile = NULL;
#endif
#endif
}

/*******************************************************************************
 **
 ** Function         btif_avk_media_task_aa_handle_decoder_reset
 **
 ** Description
 **
 ** Returns          void
 **
 *******************************************************************************/
static void btif_avk_media_task_aa_handle_decoder_reset(BT_HDR *p_msg)
{
    tBTIF_AVK_MEDIA_SINK_CFG_UPDATE *p_buf = (tBTIF_AVK_MEDIA_SINK_CFG_UPDATE*) p_msg;
    tA2D_STATUS a2d_status;
    tA2D_SBC_CIE sbc_cie;
    OI_STATUS       status;
    UINT32          freq_multiple = 48*20; /* frequency multiple for 20ms of data , initialize with 48K*/
    UINT32          num_blocks = 16;
    UINT32          num_subbands = 8;

    APPL_TRACE_DEBUG("btif_avk_media_task_aa_handle_decoder_reset p_codec_info[%x:%x:%x:%x:%x:%x]",
            p_buf->codec_info[1], p_buf->codec_info[2], p_buf->codec_info[3],
            p_buf->codec_info[4], p_buf->codec_info[5], p_buf->codec_info[6]);

    a2d_status = A2D_ParsSbcInfo(&sbc_cie, p_buf->codec_info, FALSE);
    if (a2d_status != A2D_SUCCESS)
    {
        APPL_TRACE_ERROR("ERROR dump_codec_info A2D_ParsSbcInfo fail:%d", a2d_status);
        return;
    }

    btif_avk_media_cb.sample_rate = btif_a2dp_get_sbc_track_frequency(sbc_cie.samp_freq);
    btif_avk_media_cb.channel_count = btif_a2dp_get_sbc_track_channel_count(sbc_cie.ch_mode);

    btif_avk_media_cb.rx_flush = FALSE;
    APPL_TRACE_DEBUG("Reset to sink role");
    status = OI_CODEC_SBC_DecoderReset(&context, contextData, sizeof(contextData), 2, 2, FALSE);
    if (!OI_SUCCESS(status)) {
        APPL_TRACE_ERROR("OI_CODEC_SBC_DecoderReset failed with error code %d\n", status);
    }

    APPL_TRACE_DEBUG("A2dpSink: sbc Create Track");
#if (defined(DUMP_PCM_DATA) && (DUMP_PCM_DATA == TRUE))
    outputPcmSampleFile = fopen(outputFilename, "ab");
#endif
#ifdef ANDROID
    if (btCreateTrack(btif_avk_a2dp_get_track_frequency(sbc_cie.samp_freq), btif_avk_a2dp_get_track_channel_type(sbc_cie.ch_mode)) == -1) {
        APPL_TRACE_ERROR("A2dpSink: Track creation fails!!!");
        return;
    }
#endif

    switch(sbc_cie.samp_freq)
    {
        case A2D_SBC_IE_SAMP_FREQ_16:
            APPL_TRACE_DEBUG("\tsamp_freq:%d (16000)", sbc_cie.samp_freq);
            freq_multiple = 16*20;
            break;
        case A2D_SBC_IE_SAMP_FREQ_32:
            APPL_TRACE_DEBUG("\tsamp_freq:%d (32000)", sbc_cie.samp_freq);
            freq_multiple = 32*20;
            break;
        case A2D_SBC_IE_SAMP_FREQ_44:
            APPL_TRACE_DEBUG("\tsamp_freq:%d (44100)", sbc_cie.samp_freq);
            freq_multiple = 441*2;
            break;
        case A2D_SBC_IE_SAMP_FREQ_48:
            APPL_TRACE_DEBUG("\tsamp_freq:%d (48000)", sbc_cie.samp_freq);
            freq_multiple = 48*20;
            break;
        default:
            APPL_TRACE_DEBUG(" Unknown Frequency ");
            break;
    }

    switch(sbc_cie.ch_mode)
    {
        case A2D_SBC_IE_CH_MD_MONO:
            APPL_TRACE_DEBUG("\tch_mode:%d (Mono)", sbc_cie.ch_mode);
            break;
        case A2D_SBC_IE_CH_MD_DUAL:
            APPL_TRACE_DEBUG("\tch_mode:%d (DUAL)", sbc_cie.ch_mode);
            break;
        case A2D_SBC_IE_CH_MD_STEREO:
            APPL_TRACE_DEBUG("\tch_mode:%d (STEREO)", sbc_cie.ch_mode);
            break;
        case A2D_SBC_IE_CH_MD_JOINT:
            APPL_TRACE_DEBUG("\tch_mode:%d (JOINT)", sbc_cie.ch_mode);
            break;
        default:
            APPL_TRACE_DEBUG(" Unknown Mode ");
            break;
    }

    switch(sbc_cie.block_len)
    {
        case A2D_SBC_IE_BLOCKS_4:
            APPL_TRACE_DEBUG("\tblock_len:%d (4)", sbc_cie.block_len);
            num_blocks = 4;
            break;
        case A2D_SBC_IE_BLOCKS_8:
            APPL_TRACE_DEBUG("\tblock_len:%d (8)", sbc_cie.block_len);
            num_blocks = 8;
            break;
        case A2D_SBC_IE_BLOCKS_12:
            APPL_TRACE_DEBUG("\tblock_len:%d (12)", sbc_cie.block_len);
            num_blocks = 12;
            break;
        case A2D_SBC_IE_BLOCKS_16:
            APPL_TRACE_DEBUG("\tblock_len:%d (16)", sbc_cie.block_len);
            num_blocks = 16;
            break;
        default:
            APPL_TRACE_DEBUG(" Unknown BlockLen ");
            break;
    }

    switch(sbc_cie.num_subbands)
    {
        case A2D_SBC_IE_SUBBAND_4:
            APPL_TRACE_DEBUG("\tnum_subbands:%d (4)", sbc_cie.num_subbands);
            num_subbands = 4;
            break;
        case A2D_SBC_IE_SUBBAND_8:
            APPL_TRACE_DEBUG("\tnum_subbands:%d (8)", sbc_cie.num_subbands);
            num_subbands = 8;
            break;
        default:
            APPL_TRACE_DEBUG(" Unknown SubBands ");
            break;
    }

    switch(sbc_cie.alloc_mthd)
    {
        case A2D_SBC_IE_ALLOC_MD_S:
            APPL_TRACE_DEBUG("\talloc_mthd:%d (SNR)", sbc_cie.alloc_mthd);
            break;
        case A2D_SBC_IE_ALLOC_MD_L:
            APPL_TRACE_DEBUG("\talloc_mthd:%d (Loudness)", sbc_cie.alloc_mthd);
            break;
        default:
            APPL_TRACE_DEBUG(" Unknown Allocation Method");
            break;
    }

    APPL_TRACE_DEBUG("\tBit pool Min:%d Max:%d", sbc_cie.min_bitpool, sbc_cie.max_bitpool);

    btif_avk_media_cb.frames_to_process = ((freq_multiple)/(num_blocks*num_subbands)) + 1;
    btif_avk_media_cb.a2dp_sink_pcm_buf_size  = freq_multiple * 4 * 2;
    if (btif_avk_media_cb.a2dp_sink_pcm_buf == NULL)
    {
        btif_avk_media_cb.a2dp_sink_pcm_buf = osi_malloc(btif_avk_media_cb.a2dp_sink_pcm_buf_size);
    }
    APPL_TRACE_DEBUG(" Frames to be processed in 20 ms %d",btif_avk_media_cb.frames_to_process);
}
#endif


static UINT64 time_now_us()
{
    struct timespec ts_now;
    clock_gettime(CLOCK_BOOTTIME, &ts_now);
    return ((UINT64)ts_now.tv_sec * USEC_PER_SEC) + ((UINT64)ts_now.tv_nsec / 1000);
}

/*******************************************************************************
 **
 ** Function         btif_avk_media_sink_enque_buf
 **
 ** Description      This function is called by the av_co to fill A2DP Sink Queue
 **
 **
 ** Returns          size of the queue
 *******************************************************************************/
UINT8 btif_avk_media_sink_enque_buf(BT_HDR *p_pkt, BD_ADDR bd_addr, BOOLEAN rpt_delay)
{
    tBT_AVK_SBC_HDR *p_msg;

    if(btif_avk_media_cb.rx_flush == TRUE) /* Flush enabled, do not enque*/
        return fixed_queue_length(btif_avk_media_cb.RxSbcQ);
    if(fixed_queue_length(btif_avk_media_cb.RxSbcQ) >= MAX_OUTPUT_A2DP_FRAME_QUEUE_SZ)
    {
         return fixed_queue_length(btif_avk_media_cb.RxSbcQ);
    }

    BTIF_TRACE_VERBOSE("btif_avk_media_sink_enque_buf + ");
    /* allocate and Queue this buffer */
    if ((p_msg = (tBT_AVK_SBC_HDR *)osi_malloc(sizeof(tBT_AVK_SBC_HDR) + p_pkt->len)) != NULL)
    {
        UINT8 *p_dest;

        p_dest = (UINT8*)(p_msg + 1);
        memcpy(p_dest, (UINT8*)(p_pkt + 1) + p_pkt->offset, p_pkt->len);

        p_msg->num_frames_to_be_processed = p_dest[0] & 0x0f;
        p_msg->len = p_pkt->len;
        p_msg->offset = 0;
        p_msg->layer_specific = p_pkt->layer_specific;
        memcpy(p_msg->bd_addr, bd_addr, sizeof(BD_ADDR));
        if(rpt_delay)
        {
            struct timespec ts_now;
            clock_gettime(CLOCK_BOOTTIME, &ts_now);
            p_msg->enque_ms = (UINT64)ts_now.tv_sec * 1000000000 + ts_now.tv_nsec;
            BTIF_TRACE_IMP(" %s ~~ SBC steam packet enque RxSbcQ, enque_ms = [%09llu]", __func__,p_msg->enque_ms);
        }

        BTIF_TRACE_VERBOSE("btif_avk_media_sink_enque_buf %d", p_msg->num_frames_to_be_processed);
        fixed_queue_enqueue(btif_avk_media_cb.RxSbcQ, p_msg);
        btif_avk_media_task_decode_req();
    }
    else
    {
        /* let caller deal with a failed allocation */
        BTIF_TRACE_VERBOSE("btif_avk_media_sink_enque_buf No Buffer left - ");
    }
    return fixed_queue_length(btif_avk_media_cb.RxSbcQ);
}

#endif /* BTA_AV_INCLUDED == TRUE */

