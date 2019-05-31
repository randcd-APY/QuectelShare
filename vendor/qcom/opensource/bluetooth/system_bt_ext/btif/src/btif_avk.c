/******************************************************************************
 *  Copyright (c) 2014, The Linux Foundation. All rights reserved.
 *  Not a Contribution.
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


/*****************************************************************************
 *
 *  Filename:      btif_avk.c
 *
 *  Description:   Bluedroid AVK implementation
 *
 *****************************************************************************/

#include <assert.h>
#include <string.h>

#include <hardware/bluetooth.h>
#include "hardware/bt_av.h"
#include "osi/include/allocator.h"
#include <cutils/properties.h>
#include "btcore/include/bdaddr.h"

#define LOG_TAG "bt_btif_avk"

#include "btif_avk.h"
#include "btif_util.h"
#include "btif_profile_queue.h"
#include "bta_api.h"
#include "btif_avk_media.h"
#include "bta_avk_api.h"
#include "bta_avk_co.h"
#include "btu.h"
#include "bt_utils.h"
#include "hardware/bt_av_vendor.h"
#include "osi/include/fixed_queue.h"
#include "osi/include/list.h"

/*****************************************************************************
**  Constants & Macros
******************************************************************************/
#define BTIF_AVK_SERVICE_NAME "Advanced Audio Sink"
#define BTIF_TIMEOUT_AV_OPEN_ON_RC_SECS  2

/* Number of BTIF-AV control blocks */
/* Now supports Two AV connections. */
#define BTIF_AVK_NUM_CB       2
#define HANDLE_TO_INDEX(x) ((x & BTA_AVK_HNDL_MSK) - 1)

typedef enum {
    BTIF_AVK_STATE_IDLE = 0x0,
    BTIF_AVK_STATE_OPENING,
    BTIF_AVK_STATE_OPENED,
    BTIF_AVK_STATE_STARTED,
    BTIF_AVK_STATE_CLOSING
} btif_avk_state_t;

/* Should not need dedicated suspend state as actual actions are no
   different than open state. Suspend flags are needed however to prevent
   media task from trying to restart stream during remote suspend or while
   we are in the process of a local suspend */

#define BTIF_AVK_FLAG_LOCAL_SUSPEND_PENDING 0x1
#define BTIF_AVK_FLAG_REMOTE_SUSPEND        0x2
#define BTIF_AVK_FLAG_PENDING_START         0x4
#define BTIF_AVK_FLAG_PENDING_STOP          0x8
/* Host role defenitions */
#define HOST_ROLE_MASTER                   0x00
#define HOST_ROLE_SLAVE                    0x01
#define HOST_ROLE_UNKNOWN                  0xff

#define MAX_A2DP_SINK_DATA_QUEUE_SZ         20

#define DELAY_RECORD_COUNT                 100
#define DEFAULT_RENDERING_DELAY            60      //define a fix rendering delay
#define APTX_RENDERING_DELAY               580
#define AAC_RENDERING_DELAY                1480
/*****************************************************************************
**  Local type definitions
******************************************************************************/
typedef struct
{
    UINT16 codec_type;
    UINT16 len;
    UINT16 offset;
    BD_ADDR bd_addr;
    UINT64 enque_ns;    //time of packet enqueue RxDataQ (nanosecond)
} tBT_SINK_DATA_HDR;

typedef struct
{
    tBTA_AVK_HNDL bta_handle;
    bt_bdaddr_t peer_bda;
    btif_sm_handle_t sm_handle;
    UINT8 flags;
    tBTA_AVK_EDR edr;
    UINT8   peer_sep;  /* sep type of peer device */
    UINT8 edr_3mbps;
    BOOLEAN dual_handoff;
    BOOLEAN current_playing;
    btif_sm_state_t state;
    int service;
    BOOLEAN is_slave;
    BOOLEAN is_device_playing;
    /************ Variables used for A2DP Sink *************/
    UINT16 sink_codec_type;
    /************ Variables used for A2DP Sink *************/
    BOOLEAN avdt_sync;    //for AVDT1.3 delay reporting
} btif_avk_cb_t;

static pthread_mutex_t sink_data_q_lock;
pthread_mutex_t sink_codec_q_lock;

typedef struct
{
    bt_bdaddr_t *target_bda;
    uint16_t uuid;
} btif_avk_connect_req_t;

typedef struct
{
    int sample_rate;
    int channel_count;
    UINT8 peer_bd[6];
    UINT8 codec_type;
    btav_codec_config_t   codec_info;
} btif_avk_config_req_t;

/*****************************************************************************
**  Static variables
******************************************************************************/
static btav_callbacks_t *bt_avk_callbacks = NULL;
static btav_sink_vendor_callbacks_t *bt_av_sink_vendor_callbacks = NULL;
static btif_avk_cb_t btif_avk_cb[BTIF_AVK_NUM_CB];
static alarm_t *avk_open_on_rc_timer = NULL;
int btif_max_avk_clients = 1;
static BOOLEAN enable_multicast = FALSE;
static BOOLEAN is_multicast_supported = FALSE;
static BOOLEAN multicast_disabled = FALSE;
static UINT16 enable_stack_sbc_decoding = 1; // by default enable it
static UINT16 retreive_rtp_header = 0; // by default disable it
fixed_queue_t *RxDataQ = NULL;
static UINT16 enable_delay_reporting = 0; // by default disable it
static UINT16 enable_notification_cb = 0; // stack incoming data callback
static bt_bdaddr_t streaming_bda;
static UINT64 delay_record[DELAY_RECORD_COUNT] = {0};  //store latest packets delay
static int delay_record_idx = 0;
static UINT16 qahw_delay = 0;
int rendering_delay = DEFAULT_RENDERING_DELAY;
extern tBTA_AVK_CO_CODEC_CAP_LIST *p_bta_avk_codec_pri_list;
extern tBTA_AVK_CO_CODEC_CAP_LIST bta_avk_supp_codec_cap[BTIF_SV_AVK_AA_SEP_INDEX];
extern UINT8 bta_avk_num_codec_configs;
extern const tA2D_SBC_CIE bta_avk_co_sbc_caps;
#if defined(AAC_DECODER_INCLUDED) && (AAC_DECODER_INCLUDED == TRUE)
extern const tA2D_AAC_CIE bta_avk_co_aac_caps;
#endif
#if defined(MP3_DECODER_INCLUDED) && (MP3_DECODER_INCLUDED == TRUE)
const tA2D_MP3_CIE bta_avk_co_mp3_caps;
#endif
#if defined(APTX_CLASSIC_DECODER_INCLUDED) && (APTX_CLASSIC_DECODER_INCLUDED == TRUE)
extern const tA2D_APTX_CIE bta_avk_co_aptx_caps;
#endif


/* both interface and media task needs to be ready to alloc incoming request */
#define CHECK_BTAVK_INIT() if (((bt_avk_callbacks == NULL)) \
        || (btif_avk_cb[0].sm_handle == NULL))\
{\
     BTIF_TRACE_WARNING("%s: BTAVK not initialized", __FUNCTION__);\
     return BT_STATUS_NOT_READY;\
}\
else\
{\
     BTIF_TRACE_EVENT("%s", __FUNCTION__);\
}

/* Helper macro to avoid code duplication in the state machine handlers */
#define CHECK_AVK_RC_EVENT(e, d) \
    case BTA_AVK_RC_OPEN_EVT: \
    case BTA_AVK_RC_CLOSE_EVT: \
    case BTA_AVK_REMOTE_CMD_EVT: \
    case BTA_AVK_VENDOR_CMD_EVT: \
    case BTA_AVK_META_MSG_EVT: \
    case BTA_AVK_BROWSE_MSG_EVT: \
    case BTA_AVK_RC_BROWSE_OPEN_EVT: \
    case BTA_AVK_RC_FEAT_EVT: \
    case BTA_AVK_REMOTE_RSP_EVT: \
    { \
         btif_avk_rc_handler(e, d);\
    }break; \


static BOOLEAN btif_avk_state_idle_handler(btif_sm_event_t event, void *data, int index);
static BOOLEAN btif_avk_state_opening_handler(btif_sm_event_t event, void *data, int index);
static BOOLEAN btif_avk_state_opened_handler(btif_sm_event_t event, void *data, int index);
static BOOLEAN btif_avk_state_started_handler(btif_sm_event_t event, void *data,int index);

static BOOLEAN btif_avk_state_closing_handler(btif_sm_event_t event, void *data,int index);

static BOOLEAN btif_avk_get_valid_idx(int idx);
static UINT8 btif_avk_idx_by_bdaddr( BD_ADDR bd_addr);
static int btif_get_latest_playing_device_idx();
static int btif_get_latest_device_idx_to_start();
static int btif_avk_get_valid_idx_for_rc_events(BD_ADDR bd_addr, int rc_handle);
static int btif_get_conn_state_of_device(BD_ADDR address);
static bt_status_t connect_int(bt_bdaddr_t *bd_addr, uint16_t uuid);
static void btif_avk_update_current_playing_device(int index);
static void btif_avk_event_free_data(btif_sm_event_t event, void *p_data);
static void btif_avk_media_clear_pcm_queue();
static BOOLEAN btif_avk_is_connected_on_other_idx(int current_index);
static BOOLEAN btif_avk_is_playing_on_other_idx(int current_index);
static void btif_avk_update_multicast_state(int index);

static const btif_sm_handler_t btif_avk_state_handlers[] =
{
    btif_avk_state_idle_handler,
    btif_avk_state_opening_handler,
    btif_avk_state_opened_handler,
    btif_avk_state_started_handler,
    btif_avk_state_closing_handler
};

/*************************************************************************
** Extern functions
*************************************************************************/
extern void btif_avk_rc_handler(tBTA_AVK_EVT event, tBTA_AVK *p_data);
extern BOOLEAN btif_avk_rc_get_connected_peer(BD_ADDR peer_addr);
extern UINT8 btif_avk_rc_get_connected_peer_handle(BD_ADDR peer_addr);
extern void btif_avk_rc_check_handle_pending_play (BD_ADDR peer_addr, BOOLEAN bSendToApp);
extern void btif_avk_rc_get_playing_device(BD_ADDR address);
extern void btif_avk_rc_clear_playing_state(BOOLEAN play);
extern void btif_avk_rc_clear_priority(BD_ADDR address);
extern UINT16 btif_dm_get_br_edr_links();
extern UINT16 btif_dm_get_le_links();
extern void btif_avk_rc_ctrl_send_pause(bt_bdaddr_t *bd_addr);
extern void btif_avk_rc_ctrl_send_play(bt_bdaddr_t *bd_addr);

extern fixed_queue_t *btu_general_alarm_queue;
/*****************************************************************************
** Local helper functions
******************************************************************************/
void btif_avk_trigger_dual_handoff(BOOLEAN handoff, BD_ADDR address);
BOOLEAN btif_avk_is_device_connected(BD_ADDR address);
BOOLEAN btif_avk_is_playing();
BOOLEAN btif_avk_get_ongoing_multicast();

const char *dump_avk_sm_state_name(btif_avk_state_t state)
{
    switch (state)
    {
        CASE_RETURN_STR(BTIF_AVK_STATE_IDLE)
        CASE_RETURN_STR(BTIF_AVK_STATE_OPENING)
        CASE_RETURN_STR(BTIF_AVK_STATE_OPENED)
        CASE_RETURN_STR(BTIF_AVK_STATE_STARTED)
        CASE_RETURN_STR(BTIF_AVK_STATE_CLOSING)
        default: return "UNKNOWN_STATE";
    }
}

const char *dump_avk_sm_event_name(btif_avk_sm_event_t event)
{
    switch((int)event)
    {
        CASE_RETURN_STR(BTA_AVK_ENABLE_EVT)
        CASE_RETURN_STR(BTA_AVK_REGISTER_EVT)
        CASE_RETURN_STR(BTA_AVK_OPEN_EVT)
        CASE_RETURN_STR(BTA_AVK_CLOSE_EVT)
        CASE_RETURN_STR(BTA_AVK_START_EVT)
        CASE_RETURN_STR(BTA_AVK_STOP_EVT)
        CASE_RETURN_STR(BTA_AVK_PROTECT_REQ_EVT)
        CASE_RETURN_STR(BTA_AVK_PROTECT_RSP_EVT)
        CASE_RETURN_STR(BTA_AVK_RC_OPEN_EVT)
        CASE_RETURN_STR(BTA_AVK_RC_CLOSE_EVT)
        CASE_RETURN_STR(BTA_AVK_REMOTE_CMD_EVT)
        CASE_RETURN_STR(BTA_AVK_REMOTE_RSP_EVT)
        CASE_RETURN_STR(BTA_AVK_VENDOR_CMD_EVT)
        CASE_RETURN_STR(BTA_AVK_VENDOR_RSP_EVT)
        CASE_RETURN_STR(BTA_AVK_RECONFIG_EVT)
        CASE_RETURN_STR(BTA_AVK_SUSPEND_EVT)
        CASE_RETURN_STR(BTA_AVK_PENDING_EVT)
        CASE_RETURN_STR(BTA_AVK_META_MSG_EVT)
        CASE_RETURN_STR(BTA_AVK_REJECT_EVT)
        CASE_RETURN_STR(BTA_AVK_RC_FEAT_EVT)
        CASE_RETURN_STR(BTIF_SM_ENTER_EVT)
        CASE_RETURN_STR(BTIF_SM_EXIT_EVT)
        CASE_RETURN_STR(BTIF_AVK_CONNECT_REQ_EVT)
        CASE_RETURN_STR(BTIF_AVK_DISCONNECT_REQ_EVT)
        CASE_RETURN_STR(BTIF_AVK_START_STREAM_REQ_EVT)
        CASE_RETURN_STR(BTIF_AVK_SINK_START_STREAM_REQ_EVT)
        CASE_RETURN_STR(BTIF_AVK_STOP_STREAM_REQ_EVT)
        CASE_RETURN_STR(BTIF_AVK_SUSPEND_STREAM_REQ_EVT)
        CASE_RETURN_STR(BTIF_AVK_SINK_SUSPEND_STREAM_REQ_EVT)
        CASE_RETURN_STR(BTIF_AVK_SINK_CONFIG_REQ_EVT)
#ifdef USE_AUDIO_TRACK
        CASE_RETURN_STR(BTIF_AVK_SINK_FOCUS_REQ_EVT)
#endif
        default: return "UNKNOWN_EVENT";
   }
}
//TODO.. We will remove this data structure
static BD_ADDR bd_null= {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/****************************************************************************
**  Local helper functions
*****************************************************************************/
/*******************************************************************************
**
** Function         btif_initiate_avk_open_timer_timeout
**
** Description      Timer to trigger AV open if the remote headset establishes
**                  RC connection w/o AV connection. The timer is needed to IOP
**                  with headsets that do establish AV after RC connection.
**
** Returns          void
**
*******************************************************************************/
static void btif_initiate_avk_open_timer_timeout(UNUSED_ATTR void *data)
{
    BD_ADDR peer_addr;

    /* is there at least one RC connection - There should be */
    /*We have Two Connections.*/
    if (btif_avk_rc_get_connected_peer(peer_addr))
    {
        /*Check if this peer_addr is same as currently connected AV*/
        if (btif_get_conn_state_of_device(peer_addr) == BTIF_AVK_STATE_OPENED)
        {
            BTIF_TRACE_DEBUG("AV is already connected");
        }
        else
        {
            UINT8 rc_handle;
            int index;
            /* Multicast: Check if AV slot is available for connection
             * If not available, AV got connected to different devices.
             * Disconnect this RC connection without AV connection.
             */
            rc_handle = btif_avk_rc_get_connected_peer_handle(peer_addr);
            index = btif_avk_get_valid_idx_for_rc_events(peer_addr, rc_handle);
            if(index >= btif_max_avk_clients)
            {
                BTIF_TRACE_ERROR("%s No slot free for AV connection, back off",
                            __FUNCTION__);
                return;
            }
            BTIF_TRACE_DEBUG("%s Issuing connect to the remote RC peer", __FUNCTION__);
            if(bt_avk_callbacks != NULL)
                btif_queue_connect(UUID_SERVCLASS_AUDIO_SINK, (bt_bdaddr_t*)&peer_addr,
                        connect_int);
        }
    }
    else
    {
        BTIF_TRACE_ERROR("%s No connected RC peers", __FUNCTION__);
    }

}

/*****************************************************************************
**  Static functions
******************************************************************************/

static void btif_report_connection_state(btav_connection_state_t state, bt_bdaddr_t *bd_addr)
{
    if (bt_avk_callbacks != NULL) {
        HAL_CBACK(bt_avk_callbacks, connection_state_cb, state, bd_addr);
    }
}

static void btif_report_audio_state(btav_audio_state_t state, bt_bdaddr_t *bd_addr)
{
    if (bt_avk_callbacks != NULL) {
        HAL_CBACK(bt_avk_callbacks, audio_state_cb, state, bd_addr);
    }
}

/*****************************************************************************
**
** Function     btif_avk_state_idle_handler
**
** Description  State managing disconnected AV link
**
** Returns      TRUE if event was processed, FALSE otherwise
**
*******************************************************************************/

static BOOLEAN btif_avk_state_idle_handler(btif_sm_event_t event, void *p_data, int index)
{
    char a2dp_role[PROPERTY_VALUE_MAX] = "false";

    BTIF_TRACE_IMP("%s event:%s flags %x on Index = %d", __FUNCTION__,
                     dump_avk_sm_event_name(event), btif_avk_cb[index].flags, index);

    switch (event)
    {
        case BTIF_SM_ENTER_EVT:
            /* clear the peer_bda */
            BTIF_TRACE_EVENT("IDLE state for index: %d service %d", index,
                    btif_avk_cb[index].service);
            btif_avk_cb[index].flags = 0;
            btif_avk_cb[index].edr_3mbps = 0;
            btif_avk_cb[index].edr = 0;
            btif_avk_cb[index].current_playing = FALSE;
            btif_avk_cb[index].is_slave = FALSE;
            btif_avk_cb[index].is_device_playing = FALSE;
            btif_avk_cb[index].sink_codec_type = 0xFF;
            btif_avk_cb[index].avdt_sync = FALSE;
            for (int i = 0; i < btif_max_avk_clients; i++)
            {
                btif_avk_cb[i].dual_handoff = FALSE;
            }
            btif_avk_cb[index].peer_sep = AVDT_TSEP_SRC;
            btif_avk_a2dp_set_peer_sep(AVDT_TSEP_SRC);

            /* This API will be called twice at initialization
            ** Idle can be moved when device is disconnected too.
            ** Take care of other connected device here.*/
            if (!btif_avk_is_connected())
            {
                BTIF_TRACE_EVENT("reset A2dp states in IDLE ");
                btif_avk_a2dp_on_idle();
                btif_avk_media_clear_pcm_queue();
            }
            else
            {
                //There is another AV connection, update current playin
                BTIF_TRACE_EVENT("reset A2dp states in IDLE ");
                btif_avk_update_current_playing_device(index);
            }
            break;

        case BTIF_SM_EXIT_EVT:
            break;

        case BTA_AVK_ENABLE_EVT:
            BTIF_TRACE_EVENT("AVK is enabled now for index: %d", index);
            break;

        case BTA_AVK_REGISTER_EVT:
            BTIF_TRACE_EVENT("The AV Handle:%d", ((tBTA_AVK*)p_data)->registr.hndl);
            btif_avk_cb[index].bta_handle = ((tBTA_AVK*)p_data)->registr.hndl;
            if (btif_max_avk_clients == index + 1) {
                if (bt_av_sink_vendor_callbacks != NULL) {
                    HAL_CBACK(bt_av_sink_vendor_callbacks, registration_vendor_cb, TRUE);
                }
            }
            break;

        case BTIF_AVK_CONNECT_REQ_EVT:
            /* For outgoing connect stack and app are in sync.
            */
            memcpy(&btif_avk_cb[index].peer_bda, ((btif_avk_connect_req_t*)p_data)->target_bda,
                                                                        sizeof(bt_bdaddr_t));
            BTA_AvkOpen(btif_avk_cb[index].peer_bda.address, btif_avk_cb[index].bta_handle,
                        TRUE, BTA_SEC_NONE, ((btif_avk_connect_req_t*)p_data)->uuid);
            btif_sm_change_state(btif_avk_cb[index].sm_handle, BTIF_AVK_STATE_OPENING);
            break;

        case BTA_AVK_PENDING_EVT:
        case BTA_AVK_RC_OPEN_EVT:
            /* IOP_FIX: Jabra 620 only does RC open without AV open whenever it connects. So
             * as per the AV WP, an AVRC connection cannot exist without an AV connection. Therefore,
             * we initiate an AV connection if an RC_OPEN_EVT is received when we are in AV_CLOSED state.
             * We initiate the AV connection after a small 3s timeout to avoid any collisions from the
             * headsets, as some headsets initiate the AVRC connection first and then
             * immediately initiate the AV connection
             *
             * TODO: We may need to do this only on an AVRCP Play. FixMe
             */
            /* Check if connection allowed with this device */
            /* In Dual A2dp case, this event can come for both the headsets.
             * Reject second connection request as we are already checking
             * for device priority for first device and we cannot queue
             * incoming connections requests.
             */
            if (event == BTA_AVK_RC_OPEN_EVT)
            {
                if (((tBTA_AVK*)p_data)->rc_open.status == BTA_AVK_SUCCESS)
                {
                    bdcpy(btif_avk_cb[index].peer_bda.address,
                        ((tBTA_AVK*)p_data)->rc_open.peer_addr);
                }
                else
                {
                    return TRUE;
                }
            }
            else
            {
                bdcpy(btif_avk_cb[index].peer_bda.address, ((tBTA_AVK*)p_data)->pend.bd_addr);
            }

            // Only for AVDTP connection request move to opening state
            if (event == BTA_AVK_PENDING_EVT)
                btif_sm_change_state(btif_avk_cb[index].sm_handle, BTIF_AVK_STATE_OPENING);

            if (bt_avk_callbacks != NULL)
            {
                if(event == BTA_AVK_PENDING_EVT)
                {
                    BTA_AvkOpen(btif_avk_cb[index].peer_bda.address, btif_avk_cb[index].bta_handle,
                       TRUE, BTA_SEC_NONE, UUID_SERVCLASS_AUDIO_SINK);
                }
                else if(event == BTA_AVK_RC_OPEN_EVT)
                {
                    alarm_set_on_queue(avk_open_on_rc_timer,
                                       BTIF_TIMEOUT_AV_OPEN_ON_RC_SECS,
                                       btif_initiate_avk_open_timer_timeout, NULL,
                                       btu_general_alarm_queue);
                    btif_avk_rc_handler(event, p_data);
                }
            }
            break;

        case BTIF_AVK_SINK_CONFIG_REQ_EVT:
        {
            btif_avk_config_req_t req;
            bdstr_t addr1;
            // copy to avoid alignment problems
            /* in this case, L2CAP connection is still up, but bt-app moved to disc state
               so lets move bt-app to connected state first */
            memcpy(&req, p_data, sizeof(req));
            btif_report_connection_state(BTAV_CONNECTION_STATE_CONNECTED, &(req.peer_bd));
            BTIF_TRACE_WARNING("BTIF_AVK_SINK_CONFIG_REQ_EVT %d %d %s %d",
                    req.sample_rate, req.channel_count,
                    bdaddr_to_string(&(btif_avk_cb[index].peer_bda),
                    &addr1, sizeof(addr1)), req.codec_type);

            if (bt_av_sink_vendor_callbacks != NULL) {
                HAL_CBACK(bt_av_sink_vendor_callbacks, audio_codec_config_vendor_cb,
                        &(req.peer_bd), req.codec_type, req.codec_info);
            }
        } break;

        case BTA_AVK_OPEN_EVT:
        {
            /* We get this event in Idle State if Signaling
             * channel is not closed, only Streaming channel was
             * closed earlier, and now only stream setup process is
             * initiated.
             */
            tBTA_AVK *p_bta_data = (tBTA_AVK*)p_data;
            btav_connection_state_t state;
            BTIF_TRACE_DEBUG("status:%d, edr 0x%x",p_bta_data->open.status,
                               p_bta_data->open.edr);

            if (p_bta_data->open.status == BTA_AVK_SUCCESS)
            {
                 state = BTAV_CONNECTION_STATE_CONNECTED;
                 btif_avk_cb[index].edr = p_bta_data->open.edr;
                 if (p_bta_data->open.role == HOST_ROLE_SLAVE)
                 {
                    btif_avk_cb[index].is_slave = TRUE;
                 }
                 btif_avk_cb[index].peer_sep = p_bta_data->open.sep;
                 btif_avk_a2dp_set_peer_sep(p_bta_data->open.sep);

                 if (p_bta_data->open.edr & BTA_AVK_EDR_3MBPS)
                 {
                     BTIF_TRACE_DEBUG("remote supports 3 mbps");
                     btif_avk_cb[index].edr_3mbps = TRUE;
                 }

                 bdcpy(btif_avk_cb[index].peer_bda.address, ((tBTA_AVK*)p_data)->open.bd_addr);
            }
            else
            {
                BTIF_TRACE_WARNING("BTA_AVK_OPEN_EVT::FAILED status: %d",
                                     p_bta_data->open.status );
                state = BTAV_CONNECTION_STATE_DISCONNECTED;
            }

            /* change state to open based on the status */
            if (p_bta_data->open.status == BTA_AVK_SUCCESS)
            {
                /* inform the application of the event */
                btif_report_connection_state(state, &(btif_avk_cb[index].peer_bda));
                btif_sm_change_state(btif_avk_cb[index].sm_handle, BTIF_AVK_STATE_OPENED);
                /* BTIF AV State updated, now check
                 * and update multicast state
                 */
                btif_avk_update_multicast_state(index);
            }
            if (btif_avk_cb[index].peer_sep == AVDT_TSEP_SRC)
            {
                /* if queued PLAY command,  send it now */
                btif_avk_rc_check_handle_pending_play(p_bta_data->open.bd_addr, FALSE);
                /* Bring up AVRCP connection too */
                BTA_AvkOpenRc(btif_avk_cb[index].bta_handle);
            }
            btif_queue_advance();
        } break;

        case BTA_AVK_REMOTE_CMD_EVT:
        case BTA_AVK_VENDOR_CMD_EVT:
        case BTA_AVK_META_MSG_EVT:
        case BTA_AVK_RC_FEAT_EVT:
        case BTA_AVK_REMOTE_RSP_EVT:
        case BTA_AVK_BROWSE_MSG_EVT:
        case BTA_AVK_RC_BROWSE_OPEN_EVT:
            btif_avk_rc_handler(event, (tBTA_AVK*)p_data);
            break;

        case BTA_AVK_RC_CLOSE_EVT:
            BTIF_TRACE_DEBUG("BTA_AV_RC_CLOSE_EVT: Stopping AV timer.");
            alarm_cancel(avk_open_on_rc_timer);
            btif_avk_rc_handler(event, p_data);
            break;

        default:
            BTIF_TRACE_WARNING("%s : unhandled event:%s", __FUNCTION__,
                                dump_avk_sm_event_name(event));
            return FALSE;

    }

    return TRUE;
}
/*****************************************************************************
**
** Function        btif_avk_state_opening_handler
**
** Description     Intermediate state managing events during establishment
**                 of avdtp channel
**
** Returns         TRUE if event was processed, FALSE otherwise
**
*******************************************************************************/

static BOOLEAN btif_avk_state_opening_handler(btif_sm_event_t event, void *p_data, int index)
{
    int i;
    BTIF_TRACE_IMP("%s event:%s flags %x on index = %d", __FUNCTION__,
                     dump_avk_sm_event_name(event), btif_avk_cb[index].flags, index);
    switch (event)
    {
        case BTIF_SM_ENTER_EVT:
            /* inform the application that we are entering connecting state */
            if (bt_avk_callbacks != NULL)
            {
                HAL_CBACK(bt_avk_callbacks, connection_state_cb,
                         BTAV_CONNECTION_STATE_CONNECTING, &(btif_avk_cb[index].peer_bda));
            }
            break;

        case BTIF_SM_EXIT_EVT:
            break;

        case BTA_AVK_REJECT_EVT:
            BTIF_TRACE_DEBUG(" Received  BTA_AVK_REJECT_EVT ");
            btif_report_connection_state(BTAV_CONNECTION_STATE_DISCONNECTED,
                                        &(btif_avk_cb[index].peer_bda));
            btif_sm_change_state(btif_avk_cb[index].sm_handle, BTIF_AVK_STATE_IDLE);
            break;

        case BTA_AVK_OPEN_EVT:
        {
            tBTA_AVK *p_bta_data = (tBTA_AVK*)p_data;
            btav_connection_state_t state;
            btif_sm_state_t av_state;
            BTIF_TRACE_DEBUG("status:%d, edr 0x%x, role: 0x%x",p_bta_data->open.status,
                             p_bta_data->open.edr, p_bta_data->open.role);

            if (p_bta_data->open.status == BTA_AVK_SUCCESS)
            {
                 state = BTAV_CONNECTION_STATE_CONNECTED;
                 av_state = BTIF_AVK_STATE_OPENED;
                 btif_avk_cb[index].edr = p_bta_data->open.edr;
                 if (p_bta_data->open.role == HOST_ROLE_SLAVE)
                 {
                    btif_avk_cb[index].is_slave = TRUE;
                 }
                 btif_avk_cb[index].peer_sep = p_bta_data->open.sep;
                 btif_avk_a2dp_set_peer_sep(p_bta_data->open.sep);
                 if (p_bta_data->open.edr & BTA_AVK_EDR_3MBPS)
                 {
                     BTIF_TRACE_DEBUG("remote supports 3 mbps");
                     btif_avk_cb[index].edr_3mbps = TRUE;
                 }
                 btif_avk_cb[index].avdt_sync = bta_avk_is_avdt_sync(btif_avk_cb[index].bta_handle);
                 BTIF_TRACE_DEBUG(" %s ~~ BTA_AVK_OPEN_EVT btif_avk_cb[%d].avdt_sync is [%d]",__func__,index, btif_avk_cb[index].avdt_sync);
            }
            else
            {
                BTIF_TRACE_WARNING("BTA_AVK_OPEN_EVT::FAILED status: %d",
                                     p_bta_data->open.status );
                /* Multicast: Check if connected to AVRC only device
                 * disconnect when Dual A2DP/Multicast is supported.
                 */
                BD_ADDR peer_addr;
                if ((btif_avk_rc_get_connected_peer(peer_addr))
                    &&(!bdcmp(btif_avk_cb[index].peer_bda.address, peer_addr)))
                {
                    /* Disconnect AVRCP connection, if A2DP
                     * conneciton failed, for any reason
                     */
                    BTIF_TRACE_WARNING(" Disconnecting AVRCP ");
                    BTA_AvkCloseRc(btif_avk_rc_get_connected_peer_handle(peer_addr));
                }
                state = BTAV_CONNECTION_STATE_DISCONNECTED;
                av_state  = BTIF_AVK_STATE_IDLE;
            }

            /* inform the application of the event */
            btif_report_connection_state(state, &(btif_avk_cb[index].peer_bda));
            /* change state to open/idle based on the status */
            btif_sm_change_state(btif_avk_cb[index].sm_handle, av_state);
            /* Check if the other connected AV is playing,
            * If YES, trigger DUAL Handoff. */
            if ((p_bta_data->open.status == BTA_AVK_SUCCESS))
            {
                /* BTIF AV State updated, now check
                 * and update multicast state
                 */
                btif_avk_update_multicast_state(index);

                /*This device should be now ready for all next playbacks*/
                btif_avk_cb[index].current_playing = TRUE;
                if (enable_multicast == FALSE)
                {
                    for (i = 0; i < btif_max_avk_clients; i++)
                    {
                        //Other device is not current playing
                        if (i != index)
                            btif_avk_cb[i].current_playing = FALSE;
                    }
                    /* In A2dp Multicast, stack will take care of starting
                     * the stream on newly connected A2dp device. If Handoff
                     * is supported, trigger Handoff here. */
                    if (btif_avk_is_playing() && btif_avk_cb[index].peer_sep == AVDT_TSEP_SNK)
                    {
                        BTIF_TRACE_DEBUG("Trigger Dual A2dp Handoff on %d", index);
                        btif_avk_trigger_dual_handoff(TRUE, btif_avk_cb[index].peer_bda.address);
                    }
                }
                if (btif_avk_cb[index].peer_sep == AVDT_TSEP_SRC)
                {
                    /* if queued PLAY command,  send it now */
                    btif_avk_rc_check_handle_pending_play(p_bta_data->open.bd_addr, FALSE);
                    /* Bring up AVRCP connection too */
                    BTA_AvkOpenRc(btif_avk_cb[index].bta_handle);
                }
            }
            btif_queue_advance();
        } break;

        case BTIF_AVK_SINK_CONFIG_REQ_EVT:
        {
            btif_avk_config_req_t req;
            bdstr_t addr1;
            // copy to avoid alignment problems
            memcpy(&req, p_data, sizeof(req));

            BTIF_TRACE_DEBUG("BTIF_AVK_SINK_CONFIG_REQ_EVT %d %d %s %d",
                    req.sample_rate, req.channel_count,
                    bdaddr_to_string(&(btif_avk_cb[index].peer_bda),
                    &addr1, sizeof(addr1)), req.codec_type);

            if (bt_av_sink_vendor_callbacks != NULL) {
                HAL_CBACK(bt_av_sink_vendor_callbacks, audio_codec_config_vendor_cb,
                        &(btif_avk_cb[index].peer_bda), req.codec_type, req.codec_info);
            }
        } break;

        case BTIF_AVK_CONNECT_REQ_EVT:
            // Check for device, if same device which moved to opening then ignore callback
            if (memcmp ((bt_bdaddr_t*)p_data, &(btif_avk_cb[index].peer_bda),
                sizeof(btif_avk_cb[index].peer_bda)) == 0)
            {
                BTIF_TRACE_DEBUG("%s: Same device moved to Opening state,ignore Connect Req", __func__);
                btif_queue_advance();
                break;
            }
            else
            {
                BTIF_TRACE_DEBUG("%s: Moved from idle by Incoming Connection request", __func__);
                btif_report_connection_state(BTAV_CONNECTION_STATE_DISCONNECTED, (bt_bdaddr_t*)p_data);
                btif_queue_advance();
                break;
            }

        case BTA_AVK_PENDING_EVT:
            // Check for device, if same device which moved to opening then ignore callback
            if (memcmp (((tBTA_AVK*)p_data)->pend.bd_addr, &(btif_avk_cb[index].peer_bda),
                sizeof(btif_avk_cb[index].peer_bda)) == 0)
            {
                BTIF_TRACE_DEBUG("%s: Same device moved to Opening state,ignore Pending Req", __func__);
                break;
            }
            else
            {
                BTIF_TRACE_DEBUG("%s: Moved from idle by outgoing Connection request", __func__);
                BTA_AvkDisconnect(((tBTA_AVK*)p_data)->pend.bd_addr);
                break;
            }

        case BTA_AVK_CLOSE_EVT:
            /* avdtp link is closed */
            /* Check if any other device is playing
            * and this is not the one.*/
            if (!btif_avk_is_playing())
            {
                btif_avk_a2dp_on_stopped(NULL);
            }
            btif_avk_cb[index].avdt_sync = FALSE;
            qahw_delay = 0;
            /* inform the application that we are disconnected */
            btif_report_connection_state(BTAV_CONNECTION_STATE_DISCONNECTED,
                    &(btif_avk_cb[index].peer_bda));
            btif_sm_change_state(btif_avk_cb[index].sm_handle, BTIF_AVK_STATE_IDLE);
            break;

        case BTIF_AVK_DISCONNECT_REQ_EVT:
            btif_report_connection_state(BTAV_CONNECTION_STATE_DISCONNECTED,
                &(btif_avk_cb[index].peer_bda));
            BTA_AvkClose(btif_avk_cb[index].bta_handle);
            btif_queue_advance();
            btif_sm_change_state(btif_avk_cb[index].sm_handle, BTIF_AVK_STATE_IDLE);
            break;

        CHECK_AVK_RC_EVENT(event, p_data);

        default:
            BTIF_TRACE_WARNING("%s : unhandled event:%s", __FUNCTION__,
                                dump_avk_sm_event_name(event));
            return FALSE;

   }
   return TRUE;
}


/*****************************************************************************
**
** Function        btif_avk_state_closing_handler
**
** Description     Intermediate state managing events during closing
**                 of avdtp channel
**
** Returns         TRUE if event was processed, FALSE otherwise
**
*******************************************************************************/

static BOOLEAN btif_avk_state_closing_handler(btif_sm_event_t event, void *p_data, int index)
{
    BTIF_TRACE_IMP("%s event:%s flags %x and index = %d", __FUNCTION__,
                     dump_avk_sm_event_name(event), btif_avk_cb[index].flags, index);

    switch (event)
    {
        case BTIF_SM_ENTER_EVT:
            if (btif_avk_cb[index].peer_sep == AVDT_TSEP_SRC)
            {
                btif_avk_a2dp_set_rx_flush(TRUE);
            }
            break;

        case BTA_AVK_STOP_EVT:
        case BTIF_AVK_STOP_STREAM_REQ_EVT:
            if (btif_avk_cb[index].peer_sep == AVDT_TSEP_SRC)
            {
                btif_avk_a2dp_set_rx_flush(TRUE);
                btif_avk_a2dp_on_stopped(NULL);
            }
            break;

        case BTIF_SM_EXIT_EVT:
            break;

        case BTA_AVK_CLOSE_EVT:

            /* inform the application that we are disconnecting */
            btif_report_connection_state(BTAV_CONNECTION_STATE_DISCONNECTED, &(btif_avk_cb[index].peer_bda));

            btif_sm_change_state(btif_avk_cb[index].sm_handle, BTIF_AVK_STATE_IDLE);
            break;

        /* Handle the RC_CLOSE event for the cleanup */
        case BTA_AVK_RC_CLOSE_EVT:
            btif_avk_rc_handler(event, (tBTA_AVK*)p_data);
            break;

        default:
            BTIF_TRACE_WARNING("%s : unhandled event:%s", __FUNCTION__,
                                dump_avk_sm_event_name(event));
            return FALSE;
   }
   return TRUE;
}


/*****************************************************************************
**
** Function     btif_avk_state_opened_handler
**
** Description  Handles AV events while AVDTP is in OPEN state
**
** Returns      TRUE if event was processed, FALSE otherwise
**
*******************************************************************************/

static BOOLEAN btif_avk_state_opened_handler(btif_sm_event_t event, void *p_data, int index)
{
    tBTA_AVK *p_av = (tBTA_AVK*)p_data;
    tBTIF_STATUS status = BTIF_SUCCESS;

    BTIF_TRACE_IMP("%s event:%s flags %x and index = %d", __FUNCTION__,
                     dump_avk_sm_event_name(event), btif_avk_cb[index].flags, index);

    if ((event == BTA_AVK_REMOTE_CMD_EVT) &&
         (p_av->remote_cmd.rc_id == BTA_AVK_RC_PLAY) )
    {
        for (int i = 0; i < btif_max_avk_clients; i++)
        {
            if (btif_avk_cb[i].flags & BTIF_AVK_FLAG_REMOTE_SUSPEND)
            {
                BTIF_TRACE_EVENT("%s: Resetting remote suspend flag on RC PLAY",
                        __FUNCTION__);
                btif_avk_cb[i].flags &= ~BTIF_AVK_FLAG_REMOTE_SUSPEND;
            }
        }
    }

    switch (event)
    {
        case BTIF_SM_ENTER_EVT:
            btif_avk_cb[index].flags &= ~BTIF_AVK_FLAG_PENDING_STOP;
            btif_avk_cb[index].flags &= ~BTIF_AVK_FLAG_PENDING_START;
            break;

        case BTIF_SM_EXIT_EVT:
            btif_avk_cb[index].flags &= ~BTIF_AVK_FLAG_PENDING_START;
            break;

        case BTIF_AVK_START_STREAM_REQ_EVT:
            /* update multicast state here if new device is connected
             * after A2dp connection. New A2dp device is connected
             * whlie playing */
            btif_avk_update_multicast_state(index);
            if (btif_avk_cb[index].peer_sep == AVDT_TSEP_SRC)
            {
                BTA_AvkStart(btif_avk_cb[index].bta_handle);
                btif_avk_cb[index].flags |= BTIF_AVK_FLAG_PENDING_START;
                break;
            }
            break;
        case BTIF_AVK_SINK_START_STREAM_REQ_EVT:
            // TODO: check if AVRCP connection is there, send AVRCP_PLAY, otherwise send AVDTP_START
            // check if rc connected
            if(btif_avk_rc_get_connected_peer_handle(btif_avk_cb[index].peer_bda.address))
            {
                // send avrcp play
                btif_avk_rc_ctrl_send_play(&btif_avk_cb[index].peer_bda);
            }
            else
            {
                // send AVDTP START
                btif_avk_cb[index].flags |= BTIF_AVK_FLAG_PENDING_START;
                BTA_AvkStart(btif_avk_cb[index].bta_handle);
            }
            break;
        case BTA_AVK_START_EVT:
        {
            BTIF_TRACE_DEBUG("BTA_AVK_START_EVT status %d, suspending %d, init %d",
                p_av->start.status, p_av->start.suspending, p_av->start.initiator);
            BTIF_TRACE_DEBUG("BTA_AVK_START_EVT role: %d", p_av->start.role);
            if (p_av->start.role == HOST_ROLE_SLAVE)
            {
                btif_avk_cb[index].is_slave = TRUE;
            }
            else
            {
                // update if we are master after role switch before start
                btif_avk_cb[index].is_slave = FALSE;
            }
            /* There can be role switch after device is connected,
             * hence check for role before starting multicast, and
             * disable if we are in slave role for any connection
             */
            btif_avk_update_multicast_state(index);

            if ((p_av->start.status == BTA_SUCCESS) && (p_av->start.suspending == TRUE))
                return TRUE;

            /* remain in open state if status failed */
            /* Multicast-soft Handoff:
             * START failed, cleanup Handoff flag.
             */
            if (p_av->start.status != BTA_AVK_SUCCESS)
            {
                int i;

                for (i = 0; i < btif_max_avk_clients; i++)
                {
                    btif_avk_cb[i].dual_handoff = FALSE;
                }
                return FALSE;
            }

            if (btif_avk_cb[index].peer_sep == AVDT_TSEP_SRC)
            {
                btif_avk_a2dp_set_rx_flush(FALSE); /*  remove flush state, ready for streaming*/
#ifdef USE_AUDIO_TRACK
                sink_audio_focus_status_vendor(BTIF_AVK_MEDIA_FOCUS_READY);
#endif
            }

            btif_sm_change_state(btif_avk_cb[index].sm_handle, BTIF_AVK_STATE_STARTED);

        } break;

        case BTIF_AVK_DISCONNECT_REQ_EVT:
            BTA_AvkClose(btif_avk_cb[index].bta_handle);
            if (btif_avk_cb[index].peer_sep == AVDT_TSEP_SRC) {
                BTA_AvkCloseRc(btif_avk_cb[index].bta_handle);
            }

            /* inform the application that we are disconnecting */
            btif_report_connection_state(BTAV_CONNECTION_STATE_DISCONNECTING, &(btif_avk_cb[index].peer_bda));
            break;

        case BTA_AVK_CLOSE_EVT:
             /* avdtp link is closed */
             /*Dont close the A2dp when Dual playback is happening*/
             if (btif_avk_is_connected_on_other_idx(index))
             {
                 APPL_TRACE_WARNING("Conn is closing,close AV data channel");
                 if (!btif_avk_is_playing())
                 {
                     APPL_TRACE_WARNING("Suspend the AV Data channel");
                     /* ensure tx frames are immediately suspended */
                     btif_avk_a2dp_set_tx_flush(TRUE);
                 }
             }
             else
             {
                 APPL_TRACE_WARNING("Stop the AV Data channel");
                 btif_avk_a2dp_on_stopped(NULL);
             }

            /* inform the application that we are disconnected */
            btif_report_connection_state(BTAV_CONNECTION_STATE_DISCONNECTED,
                                        &(btif_avk_cb[index].peer_bda));

            /* change state to idle, send acknowledgement if start is pending */
            if (btif_avk_cb[index].flags & BTIF_AVK_FLAG_PENDING_START) {
                btif_avk_a2dp_ack_fail();
                /* pending start flag will be cleared when exit current state */
            }

            btif_sm_change_state(btif_avk_cb[index].sm_handle, BTIF_AVK_STATE_IDLE);
            break;

        case BTA_AVK_RECONFIG_EVT:
            if((btif_avk_cb[index].flags & BTIF_AVK_FLAG_PENDING_START) &&
                (p_av->reconfig.status == BTA_AVK_SUCCESS))
            {
               APPL_TRACE_WARNING("reconfig done BTA_AVstart()");
               BTA_AvkStart(btif_avk_cb[index].bta_handle);
            }
            else if(btif_avk_cb[index].flags & BTIF_AVK_FLAG_PENDING_START)
            {
               btif_avk_cb[index].flags &= ~BTIF_AVK_FLAG_PENDING_START;
               btif_avk_a2dp_ack_fail();
            }
            break;

        case BTIF_AVK_CONNECT_REQ_EVT:
            if (memcmp ((bt_bdaddr_t*)p_data, &(btif_avk_cb[index].peer_bda),
                sizeof(btif_avk_cb[index].peer_bda)) == 0)
            {
                BTIF_TRACE_DEBUG("%s: Ignore BTIF_AVK_CONNECT_REQ_EVT for same device", __func__);
            }
            else
            {
                BTIF_TRACE_DEBUG("%s: Moved to opened by Other Incoming Conn req", __func__);
                btif_report_connection_state(BTAV_CONNECTION_STATE_DISCONNECTED,
                        (bt_bdaddr_t*)p_data);
            }
            btif_queue_advance();
            break;

        CHECK_AVK_RC_EVENT(event, p_data);

        case BTIF_AVK_SINK_CONFIG_REQ_EVT:
        {
            btif_avk_config_req_t req;
            bdstr_t addr1;
            // copy to avoid alignment problems
            memcpy(&req, p_data, sizeof(req));

            BTIF_TRACE_DEBUG("BTIF_AVK_SINK_CONFIG_REQ_EVT %d %d %s %d",
                    req.sample_rate, req.channel_count,
                    bdaddr_to_string(&(btif_avk_cb[index].peer_bda), &addr1,
                    sizeof(addr1)), req.codec_type);
            if (bt_av_sink_vendor_callbacks != NULL) {
                HAL_CBACK(bt_av_sink_vendor_callbacks, audio_codec_config_vendor_cb,
                        &(btif_avk_cb[index].peer_bda), req.codec_type, req.codec_info);
            }
        } break;

        default:
            BTIF_TRACE_WARNING("%s : unhandled event:%s", __FUNCTION__,
                               dump_avk_sm_event_name(event));
            return FALSE;

    }
    return TRUE;
}

/*****************************************************************************
**
** Function     btif_avk_state_started_handler
**
** Description  Handles AV events while A2DP stream is started
**
** Returns      TRUE if event was processed, FALSE otherwise
**
*******************************************************************************/

static BOOLEAN btif_avk_state_started_handler(btif_sm_event_t event, void *p_data, int index)
{
    tBTA_AVK *p_av = (tBTA_AVK*)p_data;
    btif_sm_state_t state = BTIF_AVK_STATE_IDLE;
    int i;

    BTIF_TRACE_IMP("%s event:%s flags %x  index =%d", __FUNCTION__,
                     dump_avk_sm_event_name(event), btif_avk_cb[index].flags, index);

    switch (event)
    {
        case BTIF_SM_ENTER_EVT:
            /* we are again in started state, clear any remote suspend flags */
            btif_avk_cb[index].flags &= ~BTIF_AVK_FLAG_REMOTE_SUSPEND;

            btif_report_audio_state(BTAV_AUDIO_STATE_STARTED, &(btif_avk_cb[index].peer_bda));
            btif_avk_cb[index].is_device_playing = TRUE;

            /* increase the a2dp consumer task priority temporarily when start
            ** audio playing, to avoid overflow the audio packet queue. */
            adjust_priority_a2dp(TRUE);
            //Clear Dual Handoff for all SCBs
            for (i = 0; i < btif_max_avk_clients; i++)
            {
                btif_avk_cb[i].dual_handoff = FALSE;
                //Other device is not current playing
                if (i != index)
                    btif_avk_cb[i].current_playing = FALSE;
            }
            //This is latest device to play now
            btif_avk_cb[index].current_playing = TRUE;
            break;

        case BTIF_SM_EXIT_EVT:
            /* restore the a2dp consumer task priority when stop audio playing. */
            adjust_priority_a2dp(FALSE);

            break;

        case BTIF_AVK_START_STREAM_REQ_EVT:
            break;

        /* fixme -- use suspend = true always to work around issue with BTA AV */
        case BTIF_AVK_STOP_STREAM_REQ_EVT:
        case BTIF_AVK_SUSPEND_STREAM_REQ_EVT:

            /* set pending flag to ensure btif task is not trying to restart
             * stream while suspend is in progress.
             * Multicast: If streaming is happening on both devices, we need
             * to update flag for both connections as SUSPEND request will
             * be sent to only one stream as internally BTA takes care of
             * suspending both streams.
             */
            for(i = 0; i < btif_max_avk_clients; i++)
            {
                state = btif_sm_get_state(btif_avk_cb[i].sm_handle);
                if (state == BTIF_AVK_STATE_STARTED)
                {
                    btif_avk_cb[i].flags |= BTIF_AVK_FLAG_LOCAL_SUSPEND_PENDING;
                }
            }

            /* if we were remotely suspended but suspend locally, local suspend
               always overrides */
            btif_avk_cb[index].flags &= ~BTIF_AVK_FLAG_REMOTE_SUSPEND;

            if (btif_avk_cb[index].peer_sep == AVDT_TSEP_SRC) {
                btif_avk_a2dp_set_rx_flush(TRUE);
                btif_avk_a2dp_on_stopped(NULL);
            }

            BTA_AvkStop(TRUE, btif_avk_cb[index].bta_handle);
            break;
        case BTIF_AVK_SINK_SUSPEND_STREAM_REQ_EVT:
            /* this will be sent only in case we are A2DP SINK */
            // check if rc connected
            if(btif_avk_rc_get_connected_peer_handle(btif_avk_cb[index].peer_bda.address))
            {
                // send avrcp pause
                btif_avk_cb[index].flags &= ~BTIF_AVK_FLAG_REMOTE_SUSPEND;
                btif_avk_cb[index].flags |= BTIF_AVK_FLAG_LOCAL_SUSPEND_PENDING;
                btif_avk_rc_ctrl_send_pause(&btif_avk_cb[index].peer_bda);
            }
            else
            {
                // send AVDTP SUSPEND
                btif_avk_cb[index].flags &= ~BTIF_AVK_FLAG_REMOTE_SUSPEND;
                btif_avk_cb[index].flags |= BTIF_AVK_FLAG_LOCAL_SUSPEND_PENDING;
                BTA_AvkStop(TRUE, btif_avk_cb[index].bta_handle);
            }
            break;
        case BTIF_AVK_SINK_START_STREAM_REQ_EVT:
            BTIF_TRACE_EVENT("BTIF_AVK_SINK_START_STREAM_REQ_EVT  already in started state");
            break;
        case BTIF_AVK_DISCONNECT_REQ_EVT:

            //Now it is not the current playing
            btif_avk_cb[index].current_playing = FALSE;
            btif_avk_update_current_playing_device(index);
            btif_avk_rc_clear_priority(btif_avk_cb[index].peer_bda.address);
            /* request avdtp to close */
            BTA_AvkClose(btif_avk_cb[index].bta_handle);
            if (btif_avk_cb[index].peer_sep == AVDT_TSEP_SRC) {
                BTA_AvkCloseRc(btif_avk_cb[index].bta_handle);
            }

            /* inform the application that we are disconnecting */
            btif_report_connection_state(BTAV_CONNECTION_STATE_DISCONNECTING, &(btif_avk_cb[index].peer_bda));

            /* wait in closing state until fully closed */
            btif_sm_change_state(btif_avk_cb[index].sm_handle, BTIF_AVK_STATE_CLOSING);
            break;

        case BTA_AVK_SUSPEND_EVT:

            BTIF_TRACE_EVENT("BTA_AVK_SUSPEND_EVT status %d, init %d",
                 p_av->suspend.status, p_av->suspend.initiator);
            //Check if this suspend is due to DUAL_Handoff
            if ((btif_avk_cb[index].dual_handoff) &&
                (p_av->suspend.status == BTA_AVK_SUCCESS))
            {
                BTIF_TRACE_EVENT("BTA_AVK_SUSPEND_EVT: Dual handoff");
                btif_avk_dispatch_sm_event(BTIF_AVK_START_STREAM_REQ_EVT, NULL, 0);
            }
            if (p_av->suspend.initiator != TRUE)
            {
                /* remote suspend, notify HAL and await audioflinger to
                 * suspend/stop stream
                 * set remote suspend flag to block media task from restarting
                 * stream only if we did not already initiate a local suspend
                 * set remote suspend flag before suspending stream as in race conditions
                 * when stream is suspended, but flag is things ge tossed up
                 */
                BTIF_TRACE_EVENT("Clear before suspending");
                if ((btif_avk_cb[index].flags & BTIF_AVK_FLAG_LOCAL_SUSPEND_PENDING) == 0)
                    btif_avk_cb[index].flags |= BTIF_AVK_FLAG_REMOTE_SUSPEND;
                for (int i = 0; i < btif_max_avk_clients; i++)
                {
                    if ((i != index) && btif_avk_get_ongoing_multicast())
                    {
                        multicast_disabled = TRUE;
                        btif_avk_update_multicast_state(index);
                        BTIF_TRACE_EVENT("Initiate suspend for other HS also");
                        btif_sm_dispatch(btif_avk_cb[i].sm_handle,
                                BTIF_AVK_SUSPEND_STREAM_REQ_EVT, NULL);
                    }
                }
            }

            /* a2dp suspended, stop media task until resumed */
            /* Multicast: If streaming on other device, don't call onsuspended
             * as it unblocks the audio process and audio process may send
             * subsequent commands and create problem during the time where we
             * still did not receive response for SUSPEND sent to other device.
             * Keep the suspend failure handling untouched and handle
             * only success case to check and avoid calling onsuspended.
             */
            if ((p_av->suspend.status != BTA_AVK_SUCCESS) ||
                !btif_avk_is_playing_on_other_idx(index))
            {
                btif_avk_a2dp_on_suspended(&p_av->suspend);
            }
            else if(btif_avk_is_playing_on_other_idx(index))
            {
                BTIF_TRACE_DEBUG("Other device not suspended, don't ack the suspend");
            }

            /* if not successful, remain in current state */
            if (p_av->suspend.status != BTA_AVK_SUCCESS)
            {
                btif_avk_cb[index].flags &= ~BTIF_AVK_FLAG_LOCAL_SUSPEND_PENDING;
                return FALSE;
            }

            if (p_av->suspend.initiator != TRUE)
            {
                btif_report_audio_state(BTAV_AUDIO_STATE_REMOTE_SUSPEND, &(btif_avk_cb[index].peer_bda));
            }
            else
            {
                btif_report_audio_state(BTAV_AUDIO_STATE_REMOTE_SUSPEND, &(btif_avk_cb[index].peer_bda));
            }
            btif_avk_cb[index].is_device_playing = FALSE;
            btif_sm_change_state(btif_avk_cb[index].sm_handle, BTIF_AVK_STATE_OPENED);

            /* suspend completed and state changed, clear pending status */
            btif_avk_cb[index].flags &= ~BTIF_AVK_FLAG_LOCAL_SUSPEND_PENDING;
            /* clear delay recode array when stream suspended */
            BTIF_TRACE_DEBUG("clear delay recode array when stream suspended");
            delay_record_idx = 0;
            average_delay = 0;
            memset(delay_record, 0, sizeof(UINT64) * DELAY_RECORD_COUNT);
            break;

#ifdef USE_AUDIO_TRACK
            case BTIF_AVK_SINK_FOCUS_REQ_EVT:
                HAL_CBACK(bt_av_sink_vendor_callbacks, audio_focus_request_vendor_cb,
                                                   &(btif_avk_cb[index].peer_bda));
            break;
#endif

        case BTA_AVK_STOP_EVT:

            btif_avk_cb[index].flags |= BTIF_AVK_FLAG_PENDING_STOP;
            btif_avk_cb[index].current_playing = FALSE;
            if (btif_avk_is_connected_on_other_idx(index))
            {
                if (enable_multicast == FALSE)
                {
                    APPL_TRACE_WARNING("other Idx is connected, move to SUSPENDED");
                    btif_avk_a2dp_on_stopped(&p_av->suspend);
                }
            }
            else
            {
                APPL_TRACE_WARNING("Stop the AV Data channel as no connection is present");
                btif_avk_a2dp_on_stopped(&p_av->suspend);
            }
            btif_avk_cb[index].is_device_playing = FALSE;


            btif_report_audio_state(BTAV_AUDIO_STATE_STOPPED, &(btif_avk_cb[index].peer_bda));
            /* if stop was successful, change state to open */
            if (p_av->suspend.status == BTA_AVK_SUCCESS)
                btif_sm_change_state(btif_avk_cb[index].sm_handle, BTIF_AVK_STATE_OPENED);

            break;

        case BTA_AVK_CLOSE_EVT:

             btif_avk_cb[index].flags |= BTIF_AVK_FLAG_PENDING_STOP;

            /* avdtp link is closed */
            APPL_TRACE_WARNING("Stop the AV Data channel");
            btif_avk_a2dp_on_stopped(NULL);

            /* inform the application that we are disconnected */
            btif_report_connection_state(BTAV_CONNECTION_STATE_DISCONNECTED,
                                        &(btif_avk_cb[index].peer_bda));

            btif_sm_change_state(btif_avk_cb[index].sm_handle, BTIF_AVK_STATE_IDLE);
            break;

        CHECK_AVK_RC_EVENT(event, p_data);

        default:
            BTIF_TRACE_WARNING("%s : unhandled event:%s", __FUNCTION__,
                                 dump_avk_sm_event_name(event));
            return FALSE;

    }
    return TRUE;
}


void btif_avk_event_deep_copy(UINT16 event, char *p_dest, char *p_src)
{
    tBTA_AVK *av_src = (tBTA_AVK *)p_src;
    tBTA_AVK *av_dest = (tBTA_AVK *)p_dest;

    // First copy the structure
    memcpy(p_dest, p_src, sizeof(tBTA_AVK));

    switch (event)
    {
        case BTA_AVK_META_MSG_EVT:
            if (av_src->meta_msg.p_data && av_src->meta_msg.len)
            {
                av_dest->meta_msg.p_data = osi_calloc(av_src->meta_msg.len);
                assert(av_dest->meta_msg.p_data);
                memcpy(av_dest->meta_msg.p_data, av_src->meta_msg.p_data, av_src->meta_msg.len);
            }

            if (av_src->meta_msg.p_msg)
            {
                av_dest->meta_msg.p_msg = osi_calloc(sizeof(tAVRC_MSG));
                assert(av_dest->meta_msg.p_msg);
                memcpy(av_dest->meta_msg.p_msg, av_src->meta_msg.p_msg, sizeof(tAVRC_MSG));

                if ((av_src->meta_msg.p_msg->hdr.opcode == AVRC_OP_VENDOR) &&
                    av_src->meta_msg.p_msg->vendor.p_vendor_data &&
                    av_src->meta_msg.p_msg->vendor.vendor_len)
                {
                    av_dest->meta_msg.p_msg->vendor.p_vendor_data = osi_calloc(
                        av_src->meta_msg.p_msg->vendor.vendor_len);
                    assert(av_dest->meta_msg.p_msg->vendor.p_vendor_data);
                    memcpy(av_dest->meta_msg.p_msg->vendor.p_vendor_data,
                        av_src->meta_msg.p_msg->vendor.p_vendor_data,
                        av_src->meta_msg.p_msg->vendor.vendor_len);
                }
            }
            break;
 /*       case BTA_AVK_BROWSE_MSG_EVT:
            if (av_src->browse_msg.p_msg)
            {
                av_dest->browse_msg.p_msg = osi_calloc(sizeof(tAVRC_MSG));
                assert(av_dest->browse_msg.p_msg);
                memcpy(av_dest->browse_msg.p_msg, av_src->browse_msg.p_msg, sizeof(tAVRC_MSG));

                if (av_src->browse_msg.p_msg->browse.p_browse_data &&
                    av_src->browse_msg.p_msg->browse.browse_len)
                {
                    av_dest->browse_msg.p_msg->browse.p_browse_data = osi_calloc(
                        av_src->browse_msg.p_msg->browse.browse_len);
                    assert(av_dest->browse_msg.p_msg->browse.p_browse_data);
                    memcpy(av_dest->browse_msg.p_msg->browse.p_browse_data,
                        av_src->browse_msg.p_msg->browse.p_browse_data,
                        av_src->browse_msg.p_msg->browse.browse_len);
                }
            }
            break;
*/
        default:
            break;
    }
}

static void btif_avk_event_free_data(btif_sm_event_t event, void *p_data)
{
    switch (event)
    {
        case BTA_AVK_META_MSG_EVT:
            {
                tBTA_AVK *av = (tBTA_AVK*)p_data;
                if (av->meta_msg.p_data)
                    osi_free(av->meta_msg.p_data);

                if (av->meta_msg.p_msg) {
                  if (av->meta_msg.p_msg->hdr.opcode == AVRC_OP_VENDOR) {
                    osi_free(av->meta_msg.p_msg->vendor.p_vendor_data);
                  }
                  osi_free_and_reset((void**)&av->meta_msg.p_msg);
                }

            }
            break;
  /*      case BTA_AVK_BROWSE_MSG_EVT:
            {
                tBTA_AVK *av = (tBTA_AVK*)p_data;

                if (av->browse_msg.p_msg)
                {
                    if (av->browse_msg.p_msg->browse.p_browse_data)
                        osi_free(av->browse_msg.p_msg->browse.p_browse_data);
                    osi_free(av->browse_msg.p_msg);
                }
            }
            break;
*/
        default:
            break;
    }
}

/*****************************************************************************
**  Local event handlers
******************************************************************************/

static void btif_avk_handle_event(UINT16 event, char* p_param)
{
    int index = 0;
    tBTA_AVK *p_bta_data = (tBTA_AVK*)p_param;
    bt_bdaddr_t *bt_addr, bt_addr1;
    UINT8 role;
    int uuid;
    btif_avk_config_req_t req;
    switch (event)
    {
        case BTIF_AVK_INIT_REQ_EVT:
            BTIF_TRACE_IMP("%s: BTIF_AVK_INIT_REQ_EVT", __FUNCTION__);
            if(btif_avk_a2dp_start_media_task())
                btif_avk_a2dp_on_init();
            break;
        /*events from Upper layer and Media Task*/
        case BTIF_AVK_CLEANUP_REQ_EVT: /*Clean up to be called on default index*/
            BTIF_TRACE_IMP("%s: BTIF_AVK_CLEANUP_REQ_EVT", __FUNCTION__);
            bt_avk_callbacks = NULL;
            btif_avk_a2dp_stop_media_task();
            return;
        case BTIF_AVK_CONNECT_REQ_EVT:
            bt_addr = (bt_bdaddr_t *)p_param;
            index = btif_avk_idx_by_bdaddr(bt_addr->address);
            break;
        case BTIF_AVK_DISCONNECT_REQ_EVT:
            /*Bd address passed should help us in getting the handle*/
            bt_addr = (bt_bdaddr_t *)p_param;
            index = btif_avk_idx_by_bdaddr(bt_addr->address);
            break;
        case BTIF_AVK_START_STREAM_REQ_EVT:
        case BTIF_AVK_STOP_STREAM_REQ_EVT:
        case BTIF_AVK_SUSPEND_STREAM_REQ_EVT:
            index = btif_avk_idx_by_bdaddr(&streaming_bda.address);
            break;
        /*Events from the stack, BTA*/
        case BTA_AVK_ENABLE_EVT:
            index = 0;
            break;
        case BTA_AVK_REGISTER_EVT:
            index = HANDLE_TO_INDEX(p_bta_data->registr.hndl);
            break;
        case BTA_AVK_OPEN_EVT:
            index = HANDLE_TO_INDEX(p_bta_data->open.hndl);
            break;
        case BTA_AVK_ROLE_CHANGED_EVT:
            index = HANDLE_TO_INDEX(p_bta_data->role_changed.hndl);
            role = p_bta_data->role_changed.new_role;
            BTIF_TRACE_EVENT("Role change: 0x%x: new role: %s",
                p_bta_data->role_changed.hndl, (role == HOST_ROLE_SLAVE) ? "Slave" : "Master");
            if (index >= 0 && index < btif_max_avk_clients)
            {
                btif_avk_cb[index].is_slave = (role == HOST_ROLE_SLAVE) ? TRUE : FALSE;
                btif_avk_update_multicast_state(index);
            }
            else
            {
                BTIF_TRACE_ERROR("%s: Invalid index for connection", __FUNCTION__);
            }
            return;

        case BTA_AVK_PENDING_EVT:
            index = HANDLE_TO_INDEX(p_bta_data->pend.hndl);
            break;
        case BTA_AVK_REJECT_EVT:
            index = HANDLE_TO_INDEX(p_bta_data->reject.hndl);
            break;
        case BTA_AVK_STOP_EVT:
            index = HANDLE_TO_INDEX(p_bta_data->suspend.hndl);
            break;
        case BTA_AVK_CLOSE_EVT:
            index = HANDLE_TO_INDEX(p_bta_data->close.hndl);
            break;
        case BTA_AVK_START_EVT:
            index = HANDLE_TO_INDEX(p_bta_data->start.hndl);
            break;
        case BTA_AVK_RECONFIG_EVT:
            index = HANDLE_TO_INDEX(p_bta_data->reconfig.hndl);
            break;
        case BTA_AVK_SUSPEND_EVT:
            index = HANDLE_TO_INDEX(p_bta_data->suspend.hndl);
            break;

        /* Handle all RC events on default index. RC handling should take
         * care of the events. All events come with BD Address
         * Handled well in AV Opening, opened and started state
         * AV Idle handler needs to take care of this event properly.
         */
        case BTA_AVK_RC_OPEN_EVT:
            index = btif_avk_get_valid_idx_for_rc_events(p_bta_data->rc_open.peer_addr,
                    p_bta_data->rc_open.rc_handle);
            break;
        case BTA_AVK_RC_CLOSE_EVT:
        /* If there is no entry in the connection table
         * RC handler has to be called for cleanup.
         * Directly call the RC handler as we cannot
         * associate any AV handle to it.
         */
            index = btif_avk_idx_by_bdaddr(p_bta_data->rc_open.peer_addr);
            if (index == btif_max_avk_clients)
            {
                btif_avk_rc_handler(event, p_bta_data);
            }
            break;
        /* Let the RC handler decide on these passthrough cmds
         * Use rc_handle to get the active AV device and use that mapping.
         */
        case BTA_AVK_RC_BROWSE_CLOSE_EVT:
            index = btif_avk_idx_by_bdaddr(p_bta_data->rc_browse_close.peer_addr);
            btif_avk_rc_handler(event, p_bta_data);
            break;
        case BTA_AVK_REMOTE_CMD_EVT:
        case BTA_AVK_VENDOR_CMD_EVT:
        case BTA_AVK_META_MSG_EVT:
        case BTA_AVK_RC_FEAT_EVT:
        case BTA_AVK_BROWSE_MSG_EVT:
            index = 0;
            break;
        case BTIF_AVK_SINK_CONFIG_REQ_EVT:
            // copy to avoid alignment problems
            memcpy(&req, p_param, sizeof(req));
            memcpy(&bt_addr1, &(req.peer_bd), sizeof(bt_bdaddr_t));
            index = btif_avk_idx_by_bdaddr(&bt_addr1.address);
            break;
        default:
            BTIF_TRACE_ERROR("Unhandled AVK event = %d", event);
            break;
    }
    BTIF_TRACE_DEBUG("Handle the AVK event = %x on index = %d", event, index);
    if (index >= 0 && index < btif_max_avk_clients)
        btif_sm_dispatch(btif_avk_cb[index].sm_handle, event, (void*)p_param);
    else
        BTIF_TRACE_ERROR("Unhandled Index = %d", index);
    btif_avk_event_free_data(event, p_param);

}

/*******************************************************************************
**
** Function         btif_avk_get_valid_idx
**
** Description      Check the validity of the current index for the connection
**
** Returns          Boolean
**
*******************************************************************************/

static BOOLEAN btif_avk_get_valid_idx(int idx)
{
    btif_sm_state_t state = btif_sm_get_state(btif_avk_cb[idx].sm_handle);
    return ((state == BTIF_AVK_STATE_OPENED) ||
            (state ==  BTIF_AVK_STATE_STARTED) ||
            (state == BTIF_AVK_STATE_OPENING));
}

/*******************************************************************************
**
** Function         btif_avk_idx_by_bdaddr
**
** Description      Get the index corresponding to BD addr
**
** Returns          UNIT8
**
*******************************************************************************/

static UINT8 btif_avk_idx_by_bdaddr(BD_ADDR bd_addr)
{
    int i;
    for (i = 0; i < btif_max_avk_clients; i++)
    {
        if ((bdcmp(bd_addr,
                  btif_avk_cb[i].peer_bda.address) == 0))
            return i;
    }
    return i;
}

BOOLEAN btif_avk_is_current_device(BD_ADDR address)
{
    UINT8 index;

    index = btif_avk_idx_by_bdaddr(address);
    if((index < btif_max_avk_clients) && btif_avk_cb[index].current_playing)
    {
        return TRUE;
    }
    return FALSE;
}

/*******************************************************************************
**
** Function         btif_get_latest_device_idx_to_start
**
** Description      Get the index of the AV where streaming is to be started
**
** Returns          int
**
*******************************************************************************/

static int btif_get_latest_device_idx_to_start()
{
    int i, j;
    BD_ADDR playing_address;

    /* Get the device which sent PLAY command
     * If found, START on that index.
     */
    memset(playing_address, 0, sizeof(BD_ADDR));
    btif_avk_rc_get_playing_device(playing_address);
    if (bdcmp(playing_address, bd_addr_null) != 0)
    {
        /* Got some valid Playing device.
         * Get the AV index for this device.
         */
        i = btif_avk_idx_by_bdaddr(playing_address);
        if (i == btif_max_avk_clients)
            return btif_max_avk_clients;
        BTIF_TRACE_EVENT("Got some valid Playing device; %d", i);
        /*Clear the Current playing device*/
        for (j = 0; j < btif_max_avk_clients; j++)
        {
            if (j != i)
              btif_avk_cb[j].current_playing = FALSE;
        }
        /*Clear the Play command in RC*/
        btif_avk_rc_clear_playing_state(FALSE);
        return i;
    }

    /*No playing device, get the latest*/
    for (i = 0; i < btif_max_avk_clients; i++)
    {
        if (btif_avk_cb[i].current_playing)
            break;
    }
    if (i == btif_max_avk_clients)
    {
        BTIF_TRACE_ERROR("Play on default");
        i = 0; /*play on default*/
    }
    return i;
}

/*******************************************************************************
**
** Function         btif_get_latest_playing_device_idx
**
** Description      Get the index of AV where streaming is happening
**
** Returns          int
**
*******************************************************************************/

static int btif_get_latest_playing_device_idx()
{
    int i;
    btif_sm_state_t state;
    for (i = 0; i < btif_max_avk_clients; i++)
    {
        state = btif_sm_get_state(btif_avk_cb[i].sm_handle);
        if (state == BTIF_AVK_STATE_STARTED)
        {
            break;
        }
    }
    return i;
}

/*******************************************************************************
**
** Function         btif_avk_is_playing
**
** Description      Is AV in streaming state
**
** Returns          BOOLEAN
**
*******************************************************************************/

BOOLEAN btif_avk_is_playing()
{
    int i;
    for (i = 0; i < btif_max_avk_clients; i++)
    {
        btif_avk_cb[i].state = btif_sm_get_state(btif_avk_cb[i].sm_handle);
        if (btif_avk_cb[i].state == BTIF_AVK_STATE_STARTED)
        {
            BTIF_TRACE_EVENT("btif_avk_is_playing on index= %d", i);
            return TRUE;
        }
    }
    return FALSE;
}

/*******************************************************************************
**
** Function         btif_get_conn_state_of_device
**
** Description      Returns the state of AV scb
**
** Returns          int
**
*******************************************************************************/

static int btif_get_conn_state_of_device(BD_ADDR address)
{
    btif_sm_state_t state = BTIF_AVK_STATE_IDLE;
    int i;
    for (i = 0; i < btif_max_avk_clients; i++)
    {
        if ((bdcmp(address,
            btif_avk_cb[i].peer_bda.address) == 0))
        {
            state = btif_sm_get_state(btif_avk_cb[i].sm_handle);
            BTIF_TRACE_EVENT("BD Found: %02X %02X %02X %02X %02X %02X :state: %s",
                address[5], address[4], address[3],
                address[2], address[1], address[0],
                dump_avk_sm_state_name(state));
        }
    }
    return state;
}

/*******************************************************************************
**
** Function         btif_avk_get_valid_idx_for_rc_events
**
** Description      gets th valid index for the RC event address
**
** Returns          int
**
*******************************************************************************/

static int btif_avk_get_valid_idx_for_rc_events(BD_ADDR bd_addr, int rc_handle)
{
    int index = 0;
    /* First try to find if it is first event in AV IF
    * both the handles would be in IDLE state, pick the first
    * If we get second RC event while processing the priority
    * for the first, reject the second connection. */

    /*Get the index from connected SCBs*/
    index = btif_avk_idx_by_bdaddr(bd_addr);
    if (index == btif_max_avk_clients)
    {
        /* None of the SCBS matched
        * Allocate free SCB, null address SCB*/
        index = btif_avk_idx_by_bdaddr(bd_null);
        BTIF_TRACE_EVENT("btif_avk_get_valid_idx_for_rc_events is %d", index);
        if (index >= btif_max_avk_clients)
        {
            BTIF_TRACE_EVENT("disconnect only AVRCP device rc_handle %d", rc_handle);
            BTA_AvkCloseRc(rc_handle);
        }
    }
    return index;
}

static void bte_avk_callback(tBTA_AVK_EVT event, tBTA_AVK *p_data)
{
    btif_transfer_context(btif_avk_handle_event, event,
                          (char*)p_data, sizeof(tBTA_AVK), btif_avk_event_deep_copy);
}

UINT8 get_rtp_offset(UINT8* p_start, UINT16 codec_type)
{
    UINT8   rtp_version, padding, extension, csrc_count, extension_len;
    UINT8 offset = 0;
    UINT8* ptr = p_start;
    // NO RTP Header for APTX classic
    if ((codec_type == A2DP_SINK_AUDIO_CODEC_APTX) || (codec_type == A2D_NON_A2DP_MEDIA_CT))
        return 0;
    rtp_version = *(p_start) >> 6;
    padding = (*(p_start) >> 5) & 0x01;
    extension = (*(p_start) >> 4) & 0x01;
    csrc_count = *(p_start) & 0x0F;

    BTIF_TRACE_DEBUG(" rtp_v = %d, padding = %d, xtn = %d, csrc_count = %d",
             rtp_version, padding, extension, csrc_count);
    offset =  12 + csrc_count *4;
    if(extension)
    {
        ptr = ptr + offset + 2;
        BE_STREAM_TO_UINT16(extension_len, ptr);
        offset = offset + 4 + extension_len * 4;
    }
    APPL_TRACE_DEBUG(" %s codec_type = %d offset = %d", __FUNCTION__, codec_type, offset);
    return offset;
}
/*Called only in case of A2dp SInk*/
static void bte_avk_media_callback(tBTA_AVK_EVT event, tBTA_AVK_MEDIA *p_data, BD_ADDR bd_addr)
{
    btif_sm_state_t state;
    UINT8 que_len;
    tA2D_STATUS a2d_status;
    tA2D_SBC_CIE sbc_cie;
    UINT8* start_ptr;
    UINT16 data_len;
    BT_HDR* p_pkt;
    UINT8 rtp_offset = 0;
#if defined(AAC_DECODER_INCLUDED) && (AAC_DECODER_INCLUDED == TRUE)
    tA2D_AAC_CIE aac_cie;
#endif
#if defined(MP3_DECODER_INCLUDED) && (MP3_DECODER_INCLUDED == TRUE)
    tA2D_MP3_CIE mp3_cie;
#endif
#if defined(APTX_CLASSIC_DECODER_INCLUDED) && (APTX_CLASSIC_DECODER_INCLUDED == TRUE)
    tA2D_APTX_CIE aptx_cie;
#endif
    btif_avk_config_req_t config_req;
    int index = btif_avk_idx_by_bdaddr(bd_addr);
    if (index >= btif_max_avk_clients)
    {
        BTIF_TRACE_DEBUG("%s Invalid index for device", __FUNCTION__);
        return;
    }

    if (event == BTA_AVK_MEDIA_DATA_EVT)/* Switch to BTIF_MEDIA context */
    {
        state= btif_sm_get_state(btif_avk_cb[index].sm_handle);
        BTIF_TRACE_DEBUG("%s index = %d state = %d", __FUNCTION__, index, state);
        if (((state == BTIF_AVK_STATE_STARTED) || /* send SBC packets only in Started State */
             (state == BTIF_AVK_STATE_OPENED)) &&
             (&streaming_bda != NULL) && !memcmp(&streaming_bda.address, bd_addr, sizeof(BD_ADDR)))
        {
            p_pkt = (BT_HDR *)p_data;
            start_ptr = (UINT8*)(p_pkt + 1) + p_pkt->offset;
            // now we would always get RTP header from bta.
            switch (btif_avk_cb[index].sink_codec_type)
            {
                case BTIF_AVK_CODEC_SBC:
                    if (enable_stack_sbc_decoding) {
                        // in this case we have to remove RTP header and send for decoding
                        rtp_offset = get_rtp_offset(start_ptr, BTIF_AVK_CODEC_SBC);
                        p_pkt->offset = p_pkt->offset + rtp_offset;
                        p_pkt->len = p_pkt->len - rtp_offset;
                        que_len = btif_avk_media_sink_enque_buf((BT_HDR *)p_pkt, bd_addr, btif_avk_cb[index].avdt_sync);
                        BTIF_TRACE_DEBUG(" %s ~~ SBC btif_avk_media_sink_enque_buf que-len = %d, AVDT_SYNC = %d", __func__, que_len, btif_avk_cb[index].avdt_sync);
                        break;
                    }
                    if(!retreive_rtp_header) {
                        // host wants Raw SBC packets without RTP header
                        rtp_offset = get_rtp_offset(start_ptr, btif_avk_cb[index].sink_codec_type);
                        p_pkt->offset = p_pkt->offset + rtp_offset;
                        p_pkt->len = p_pkt->len - rtp_offset;
                    }
                    // adjust start and len again
                    start_ptr = (UINT8*)(p_pkt + 1) + p_pkt->offset;
                    data_len = p_pkt->len;
                    que_len = btif_media_enque_sink_data(btif_avk_cb[index].sink_codec_type,
                          start_ptr, data_len, bd_addr, 0);
                    break;
                case A2D_NON_A2DP_MEDIA_CT:
                    // APTX does not have RTP header
                    data_len = p_pkt->len;
                    que_len = btif_media_enque_sink_data(A2DP_SINK_AUDIO_CODEC_APTX,
                          start_ptr, data_len, bd_addr, 0);
                    break;
                default: // for all other codecs
                    if(!retreive_rtp_header) {
                        // host wants Raw SBC packets without RTP header
                        rtp_offset = get_rtp_offset(start_ptr, btif_avk_cb[index].sink_codec_type);
                        p_pkt->offset = p_pkt->offset + rtp_offset;
                        p_pkt->len = p_pkt->len - rtp_offset;
                    }
                    // adjust start and len again
                    start_ptr = (UINT8*)(p_pkt + 1) + p_pkt->offset;
                    data_len = p_pkt->len;
                    que_len = btif_media_enque_sink_data(btif_avk_cb[index].sink_codec_type,
                          start_ptr, data_len, bd_addr, 0);
                    break;
            }
            BTIF_TRACE_DEBUG(" Codec_Type = %d, Packets in Que %d sbc_decoding = %d",
                    btif_avk_cb[index].sink_codec_type, que_len, enable_stack_sbc_decoding);
        }
        else
            return;
    }

    if (event == BTA_AVK_MEDIA_SINK_CFG_EVT) {
        UINT8* config = (UINT8*)(p_data->avk_config.codec_info);
        UINT8 codec_type = config[2];
        /* send a command to BT Media Task */
        //memcpy(config_req.codec_info,(UINT8*)(p_data->avk_config.codec_info), AVDT_CODEC_SIZE);
        config_req.codec_type = codec_type;
        btif_avk_cb[index].sink_codec_type = codec_type;
        switch(codec_type)
        {
        case BTIF_AVK_CODEC_SBC:
            BTIF_TRACE_DEBUG("rendering_delay has been inited as :%d", DEFAULT_RENDERING_DELAY);
            rendering_delay = DEFAULT_RENDERING_DELAY;

            if(enable_stack_sbc_decoding) // if SBC decoding has to be done by Stack
                btif_avk_reset_decoder((UINT8*)(p_data->avk_config.codec_info));
            a2d_status = A2D_ParsSbcInfo(&sbc_cie, (UINT8 *)(p_data->avk_config.codec_info), FALSE);
            if (a2d_status == A2D_SUCCESS) {
                /* Switch to BTIF context */
                config_req.sample_rate = btif_a2dp_get_sbc_track_frequency(sbc_cie.samp_freq);
                config_req.channel_count = btif_a2dp_get_sbc_track_channel_count(sbc_cie.ch_mode);
                config_req.codec_info.sbc_config.samp_freq = sbc_cie.samp_freq;
                config_req.codec_info.sbc_config.ch_mode = sbc_cie.ch_mode;
                config_req.codec_info.sbc_config.block_len = sbc_cie.block_len;
                config_req.codec_info.sbc_config.alloc_mthd = sbc_cie.alloc_mthd;
                config_req.codec_info.sbc_config.max_bitpool = sbc_cie.max_bitpool;
                config_req.codec_info.sbc_config.min_bitpool = sbc_cie.min_bitpool;
                config_req.codec_info.sbc_config.num_subbands = sbc_cie.num_subbands;
                memcpy(config_req.peer_bd,(UINT8*)(p_data->avk_config.bd_addr),
                                                                  sizeof(config_req.peer_bd));
                btif_transfer_context(btif_avk_handle_event, BTIF_AVK_SINK_CONFIG_REQ_EVT,
                                         (char*)&config_req, sizeof(config_req), NULL);
            }
            else
            {
                APPL_TRACE_ERROR("ERROR dump_codec_info A2D_ParsSbcInfo fail:%d", a2d_status);
            }
            break;

#if defined(AAC_DECODER_INCLUDED) && (AAC_DECODER_INCLUDED == TRUE)
        case BTA_AVK_CODEC_M24:
            BTIF_TRACE_DEBUG("rendering_delay has been inited as :%d", AAC_RENDERING_DELAY);
            rendering_delay = AAC_RENDERING_DELAY;

            a2d_status = A2D_ParsAacInfo(&aac_cie, (UINT8 *)(p_data->avk_config.codec_info), FALSE);
            if (a2d_status == A2D_SUCCESS) {
                /* Switch to BTIF context */
                config_req.sample_rate = btif_a2dp_get_aac_track_frequency(aac_cie.samp_freq);
                config_req.channel_count = btif_a2dp_get_aac_track_channel_count(aac_cie.channels);
                config_req.codec_info.aac_config.bit_rate = aac_cie.bit_rate;
                config_req.codec_info.aac_config.sampling_freq = aac_cie.samp_freq;
                config_req.codec_info.aac_config.obj_type = aac_cie.object_type;
                config_req.codec_info.aac_config.channel_count = aac_cie.channels;
                config_req.codec_info.aac_config.vbr = aac_cie.vbr;
                memcpy(&config_req.peer_bd,(UINT8*)(p_data->avk_config.bd_addr),
                                                                  sizeof(config_req.peer_bd));
                btif_transfer_context(btif_avk_handle_event, BTIF_AVK_SINK_CONFIG_REQ_EVT,
                                     (char*)&config_req, sizeof(config_req), NULL);
            } else {
                APPL_TRACE_ERROR("ERROR dump_codec_info A2D_ParsAacInfo fail:%d", a2d_status);
            }
            break;
#endif
#if defined(MP3_DECODER_INCLUDED) && (MP3_DECODER_INCLUDED == TRUE)
        case BTA_AVK_CODEC_M12:
            BTIF_TRACE_DEBUG("rendering_delay has been inited as :%d", DEFAULT_RENDERING_DELAY);
            rendering_delay = DEFAULT_RENDERING_DELAY;

            a2d_status = A2D_ParsMp3Info(&mp3_cie, (UINT8 *)(p_data->avk_config.codec_info), FALSE);
            if (a2d_status == A2D_SUCCESS) {
                /* Switch to BTIF context */
                config_req.sample_rate = btif_a2dp_get_mp3_track_frequency(mp3_cie.samp_freq);
                config_req.channel_count = btif_a2dp_get_mp3_track_channel_count(mp3_cie.channels);
                config_req.codec_info.mp3_config.bit_rate = mp3_cie.bit_rate;
                config_req.codec_info.mp3_config.sampling_freq = mp3_cie.samp_freq;
                config_req.codec_info.mp3_config.layer = mp3_cie.layer;
                config_req.codec_info.mp3_config.channel_count = mp3_cie.channels;
                config_req.codec_info.mp3_config.vbr = mp3_cie.vbr;
                config_req.codec_info.mp3_config.mpf = mp3_cie.mpf;
                config_req.codec_info.mp3_config.crc = mp3_cie.crc;
                memcpy(&config_req.peer_bd,(UINT8*)(p_data->avk_config.bd_addr),
                                                                  sizeof(config_req.peer_bd));
                btif_transfer_context(btif_avk_handle_event, BTIF_AVK_SINK_CONFIG_REQ_EVT,
                                     (char*)&config_req, sizeof(config_req), NULL);
            } else {
                APPL_TRACE_ERROR("ERROR dump_codec_info A2D_ParsMp3Info fail:%d", a2d_status);
            }
            break;
#endif
#if defined(APTX_CLASSIC_DECODER_INCLUDED) && (APTX_CLASSIC_DECODER_INCLUDED == TRUE)
        case A2D_NON_A2DP_MEDIA_CT:
            BTIF_TRACE_DEBUG("rendering_delay has been inited as :%d", APTX_RENDERING_DELAY);
            rendering_delay = APTX_RENDERING_DELAY;

            a2d_status = A2D_ParsAptxInfo(&aptx_cie, (UINT8 *)(p_data->avk_config.codec_info), FALSE);
            if (a2d_status == A2D_SUCCESS) {
                /* Switch to BTIF context */
                /* setting APTX for Vendor specific codec right now */
                config_req.codec_type = A2DP_SINK_AUDIO_CODEC_APTX;
                config_req.sample_rate = btif_a2dp_get_aptx_track_frequency(aptx_cie.sampleRate);
                config_req.channel_count = btif_a2dp_get_aptx_track_channel_count(aptx_cie.channelMode);
                config_req.codec_info.aptx_config.vendor_id = aptx_cie.vendorId;
                config_req.codec_info.aptx_config.codec_id = aptx_cie.codecId;
                config_req.codec_info.aptx_config.sampling_freq = aptx_cie.sampleRate;
                config_req.codec_info.aptx_config.channel_count = aptx_cie.channelMode;
                memcpy(&config_req.peer_bd,(UINT8*)(p_data->avk_config.bd_addr),
                                                                  sizeof(config_req.peer_bd));
                btif_transfer_context(btif_avk_handle_event, BTIF_AVK_SINK_CONFIG_REQ_EVT,
                                     (char*)&config_req, sizeof(config_req), NULL);
            } else {
                APPL_TRACE_ERROR("ERROR dump_codec_info A2D_ParsAptxInfo fail:%d", a2d_status);
            }
            break;
#endif
            default:
            BTIF_TRACE_DEBUG("rendering_delay has been inited as :%d", DEFAULT_RENDERING_DELAY);
            rendering_delay = DEFAULT_RENDERING_DELAY;
            break;
        }
    }
}

/*******************************************************************************
**
** Function         UpdateRptDelay
**
** Description      Count average packet delay (include buffering, decoding, rending delay)
**
** Returns          delay value (nanosencond)
**
*******************************************************************************/
static UINT16 UpdateRptDelay(UINT64 enque_ns)
{
    struct timespec ts_now;
    memset(&ts_now, 0, sizeof(ts_now));
    clock_gettime(CLOCK_BOOTTIME, &ts_now);

    average_delay = 0;

    UINT64 deque_ns = (UINT64)ts_now.tv_sec * 1000000000 + ts_now.tv_nsec;
    //total delay = buffering + decoding + rending delay
    UINT64 delay_ns = deque_ns - enque_ns + (qahw_delay + rendering_delay) * 1000000;

    if(delay_record_idx >= DELAY_RECORD_COUNT)
    delay_record_idx = 0;

    delay_record[delay_record_idx++] = delay_ns;

    UINT64 sum_dealy = 0; int i = 0;
    for(; i < DELAY_RECORD_COUNT; i++)
    {
        if(delay_record[i] > 0)
            sum_dealy += delay_record[i];
        else
            break;
    }
    if(i >= DELAY_RECORD_COUNT)
        average_delay = (sum_dealy / DELAY_RECORD_COUNT);

    BTIF_TRACE_DEBUG(" %s ~~ deque_ns = [%09llu], enque_ns = [%09llu] delay_ns = [%09llu] average_delay = [%09llu] ", __func__,
                  deque_ns, enque_ns, delay_ns, average_delay);

    return average_delay;
}

/*******************************************************************************
**
** Function         btif_avk_init
**
** Description      Initializes btif AV if not already done
**
** Returns          bt_status_t
**
*******************************************************************************/

bt_status_t btif_avk_init(int service_id)
{
    int i;
    if (btif_avk_cb[0].sm_handle == NULL)
    {
        BTIF_TRACE_IMP("%s", __FUNCTION__);
        alarm_free(avk_open_on_rc_timer);
        avk_open_on_rc_timer = alarm_new("btif_av.avk_open_on_rc_timer");
        if(!btif_avk_a2dp_is_media_task_stopped())
            return BT_STATUS_FAIL;
        btif_avk_cb[0].service = service_id;
        RxDataQ = fixed_queue_new(SIZE_MAX);

        /* Also initialize the AV state machine */
        for (i = 0; i < btif_max_avk_clients; i++)
        {
            btif_avk_cb[i].sm_handle = btif_sm_init((const btif_sm_handler_t*)btif_avk_state_handlers,
                                                    BTIF_AVK_STATE_IDLE, i);
        }

        btif_transfer_context(btif_avk_handle_event, BTIF_AVK_INIT_REQ_EVT,
                (char*)&service_id, sizeof(int), NULL);

        btif_enable_service(service_id);
    }

    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         init_sink
**
** Description      Initializes the AV interface for sink mode
**
** Returns          bt_status_t
**
*******************************************************************************/

static bt_status_t init_sink(btav_callbacks_t* callbacks)
{
    bt_status_t status;

    BTIF_TRACE_EVENT("%s", __FUNCTION__);

    status = BT_STATUS_SUCCESS;

    if (status == BT_STATUS_SUCCESS) {
        bt_avk_callbacks = callbacks;
    }
    pthread_mutex_init(&sink_codec_q_lock, NULL);
    pthread_mutex_lock(&sink_codec_q_lock);
    if (p_bta_avk_codec_pri_list == NULL) {
        int i = 0;
        bta_avk_num_codec_configs = BTIF_SV_AVK_AA_SEP_INDEX;
        p_bta_avk_codec_pri_list = osi_calloc(bta_avk_num_codec_configs *
            sizeof(tBTA_AVK_CO_CODEC_CAP_LIST));
        if (p_bta_avk_codec_pri_list != NULL) {
            /* Set default priorty order as APTX (Classic) > AAC > MP3 > SBC */
#if defined(APTX_CLASSIC_DECODER_INCLUDED) && (APTX_CLASSIC_DECODER_INCLUDED == TRUE)
            p_bta_avk_codec_pri_list[i].codec_type = A2D_NON_A2DP_MEDIA_CT;
            memcpy(&p_bta_avk_codec_pri_list[i++].codec_cap.aptx_caps,
                &bta_avk_co_aptx_caps, sizeof(tA2D_APTX_CIE));
            memcpy(&bta_avk_supp_codec_cap[BTIF_SV_AVK_AA_APTX_INDEX]
                .codec_cap.aptx_caps, &bta_avk_co_aptx_caps, sizeof(tA2D_APTX_CIE));
#endif
#if defined(AAC_DECODER_INCLUDED) && (AAC_DECODER_INCLUDED == TRUE)
            p_bta_avk_codec_pri_list[i].codec_type = A2D_MEDIA_CT_M24;
            memcpy(&p_bta_avk_codec_pri_list[i++].codec_cap.aac_caps,
                &bta_avk_co_aac_caps, sizeof(tA2D_AAC_CIE));
            memcpy(&bta_avk_supp_codec_cap[BTIF_SV_AVK_AA_AAC_INDEX]
                .codec_cap.aac_caps, &bta_avk_co_aac_caps, sizeof(tA2D_AAC_CIE));
#endif
#if defined(MP3_DECODER_INCLUDED) && (MP3_DECODER_INCLUDED == TRUE)
            p_bta_avk_codec_pri_list[i].codec_type = A2D_MEDIA_CT_M12;
            memcpy(&p_bta_avk_codec_pri_list[i++].codec_cap.mp3_caps,
                &bta_avk_co_mp3_caps, sizeof(tA2D_MP3_CIE));
            memcpy(&bta_avk_supp_codec_cap[BTIF_SV_AVK_AA_MP3_INDEX]
                .codec_cap.mp3_caps, &bta_avk_co_mp3_caps, sizeof(tA2D_MP3_CIE));
#endif
            p_bta_avk_codec_pri_list[i].codec_type = A2D_MEDIA_CT_SBC;
            memcpy(&p_bta_avk_codec_pri_list[i++].codec_cap.sbc_caps,
                &bta_avk_co_sbc_caps, sizeof(tA2D_SBC_CIE));
            memcpy(&bta_avk_supp_codec_cap[BTIF_SV_AVK_AA_SBC_INDEX]
                .codec_cap.sbc_caps, &bta_avk_co_sbc_caps, sizeof(tA2D_SBC_CIE));
        }
        BTIF_TRACE_DEBUG(" %s: initialized codec list with default %d number of codecs",
            __FUNCTION__, i + 1);
    }
    pthread_mutex_unlock(&sink_codec_q_lock);

   return status;
}
/*******************************************************************************
**
** Function         init_sink_vendor
**
** Description      Initializes the AV interface for sink vendor mode
**
** Returns          bt_status_t
**
*******************************************************************************/
static bt_status_t init_sink_vendor(btav_sink_vendor_callbacks_t* callbacks, int max,
                             int a2dp_multicast_state, uint8_t streaming_prarm)
{
    bt_status_t status = BT_STATUS_FAIL;

    BTIF_TRACE_IMP("%s max = %d", __FUNCTION__, max);

    enable_multicast = FALSE; // Clear multicast flag for sink
    btif_max_avk_clients = max;
    if (bt_avk_callbacks != NULL)
        status = btif_avk_init(BTA_A2DP_SINK_SERVICE_ID);


    if (status == BT_STATUS_SUCCESS) {
        bt_av_sink_vendor_callbacks = callbacks;
        //BTA_AvEnable_Sink(TRUE);
    }

    enable_stack_sbc_decoding = streaming_prarm & A2DP_SINK_ENABLE_SBC_DECODING;
    retreive_rtp_header = streaming_prarm & A2DP_SINK_RETREIVE_RTP_HEADER;
    enable_delay_reporting = streaming_prarm & A2DP_SINK_ENABLE_DELAY_REPORTING;
    enable_notification_cb = streaming_prarm & A2DP_SINK_ENABLE_NOTIFICATION_CB;

    BTIF_TRACE_IMP(" ~~ enable_sbc_decoding = %d, retreive RTP header = %d, enable_delay_reporting = %d",
            enable_stack_sbc_decoding, retreive_rtp_header, enable_delay_reporting);

    /* initializing mutex for sink */
    pthread_mutex_init(&sink_data_q_lock, NULL);

    return status;
}

/* to be used for non-pcm codecs
 * size should include RTP header size
 */
static uint32_t get_frame_aligned_data (UINT16 codec_type, UINT8* data, uint32_t size)
{
    uint16_t q_bytes_left = 0;// bytes left in topmost element of Q
    tBT_SINK_DATA_HDR* p_data_q_buf; // pointer to first element in que;
    bdstr_t addr1, addr2;
    bt_bdaddr_t bda;
    UINT8* p_src;
    // map input buffer
    UINT8* p_start = data;
    UINT8* p_end = data + size;
    UINT8* p_curr = data;
    /* at any point p_curr - p_start => bytes written till now
     *              p_end  - p_curr =>  space left in input buffer
     */
    UINT8 rtp_offset = 0;
    UINT8 total_frames = 0;

    BTIF_TRACE_DEBUG(" %s size = %d codec_type_requested = %d ", __FUNCTION__, size, codec_type);

    if (p_end <= p_start) {
        BTIF_TRACE_DEBUG("%s input params wrong, returning", __FUNCTION__);
        return 0;
    }
    pthread_mutex_lock(&sink_data_q_lock);
    if(fixed_queue_is_empty(RxDataQ)) {
        BTIF_TRACE_DEBUG("%s Sink Que Empty, returning", __FUNCTION__);
        pthread_mutex_unlock(&sink_data_q_lock);
        return 0;
    }

    // consistency check: check codec from remote and codec info in Q
    p_data_q_buf = (tBT_SINK_DATA_HDR *)fixed_queue_try_peek_first(RxDataQ);
    if (p_data_q_buf == NULL) {
        pthread_mutex_unlock(&sink_data_q_lock);
        return 0;
    }
    if (codec_type != p_data_q_buf->codec_type)
    {
        BTIF_TRACE_IMP("%s codec mismatch, returning, requested_codec_type %d, codec_present %d",
            __FUNCTION__, codec_type, p_data_q_buf->codec_type);
        p_data_q_buf = (tBT_SINK_DATA_HDR *)fixed_queue_try_dequeue(RxDataQ);
        osi_free(p_data_q_buf);
        pthread_mutex_unlock(&sink_data_q_lock);
        return 0;
    }
    p_src = (UINT8*)(p_data_q_buf + 1) + p_data_q_buf->offset;

    if (retreive_rtp_header) {
        // if callback mechanism is enabled, move pointer ahead by size of timestamp
        UINT8 *rtp_start_addr = p_src + (enable_notification_cb ? sizeof(uint64_t) : 0);
        // rtp_offset will be same for all packets
        rtp_offset = get_rtp_offset(rtp_start_addr, codec_type);
    }
    while (!fixed_queue_is_empty(RxDataQ))
    {
        p_data_q_buf = (tBT_SINK_DATA_HDR *)fixed_queue_try_peek_first(RxDataQ);
        if (p_data_q_buf == NULL) {
            BTIF_TRACE_IMP(" %s Que Pointer Null, Bail out ", __FUNCTION__);
            break;
        }
        bdcpy(bda.address, p_data_q_buf->bd_addr);
        BTIF_TRACE_DEBUG(" %s bd_addr %s p_pcm_q_buf->bd_addr %s", __FUNCTION__,
                bdaddr_to_string(&streaming_bda, &addr1, sizeof(addr1)),
                bdaddr_to_string(&bda, &addr2, sizeof(addr2)));
        if ((&streaming_bda != NULL) &&
                memcmp(&streaming_bda.address, p_data_q_buf->bd_addr, sizeof(BD_ADDR)))
        {
            BTIF_TRACE_DEBUG("%s app fetching data for diff device, dequeue this packet",
                    __FUNCTION__);
            p_data_q_buf = (tBT_SINK_DATA_HDR *)fixed_queue_try_dequeue(RxDataQ);
            osi_free(p_data_q_buf);
            continue;
        }
        q_bytes_left = p_data_q_buf->len;// this will include RTP header
        /* Increment bytes left to cater for additional timestamp header in case
         * streaming with callback is enabled*/
        q_bytes_left += (enable_notification_cb ? sizeof(uint64_t): 0);
        //write timestamp if streaming with callback is enabled
        if (enable_notification_cb) {
            memcpy(p_curr, p_src, sizeof(uint64_t));
            BTIF_TRACE_DEBUG("%s timestamp = %llu", __FUNCTION__, *((uint64_t *)p_curr));
            p_curr += sizeof(uint64_t);
            p_data_q_buf->offset += (UINT16)sizeof(uint64_t);
        }
        if (retreive_rtp_header && codec_type != A2DP_SINK_AUDIO_CODEC_APTX) {
            // check if we have enough space for RTP Header and audio data
            if (((p_end - p_curr) < (q_bytes_left - rtp_offset)) ||
                ((p_end - p_curr) < (rtp_offset))) {
                BTIF_TRACE_IMP(" %s Not enough space, Bail out ", __FUNCTION__);
                break;
            }
            // write rtp header first.
            p_src = (UINT8*)(p_data_q_buf + 1) + p_data_q_buf->offset;
            memcpy(p_curr, p_src, rtp_offset); // copy p_src to p_curr to cater for timestamp
            /* if callback mechanism for streaming is used, increment streaming data start
             * address by extra 8 bytes */
            UINT8 *p_data_start = p_start + rtp_offset +
                    (enable_notification_cb ? p_data_q_buf->offset: 0);
            if ( p_data_start> p_curr) {
                // writing RTP header for first time
                p_curr += rtp_offset;
            }
            p_data_q_buf->offset += rtp_offset;
            p_data_q_buf->len = p_data_q_buf->len - rtp_offset;
        }
        q_bytes_left = p_data_q_buf->len;// readjust after removing RTP
        BTIF_TRACE_DEBUG(" %s Q_Len %d, input buffer space %d, bytes_left_in_Q %d", __FUNCTION__,
                  fixed_queue_length(RxDataQ), (p_end - p_curr), q_bytes_left);
        // write encoded packets
        if ((p_end - p_curr) >= q_bytes_left)
        {
            // read from topmost element and deque it
            p_data_q_buf = (tBT_SINK_DATA_HDR *)fixed_queue_try_dequeue(RxDataQ);
            if (p_data_q_buf == NULL) {
                BTIF_TRACE_IMP(" %s: p_data_q_buf is NULL", __FUNCTION__);
                break;
            }
            p_src = (UINT8*)(p_data_q_buf + 1) + p_data_q_buf->offset;
            total_frames += *(p_src);
            if(p_data_q_buf->codec_type == BTIF_AVK_CODEC_SBC)
            {
                /* When Callback mechanism for streaming is enabled, there is no need
                 * to increment p_src pointer and decrement q_bytes_left by 1 byte
                 * as we are sending only 1 media packet in callback mechanism, so
                 * by default after rtp data it will write #frames in that address*/
                if (((p_start + rtp_offset) < p_curr) && !enable_notification_cb)
                {
                    q_bytes_left=q_bytes_left-1;
                    p_src= p_src+1;
                }
            }
            //  BTIF_TRACE_IMP("**QCOM** have q_bytes_left =%d",q_bytes_left);
            memcpy(p_curr, p_src, q_bytes_left);
            //BTIF_TRACE_IMP("**QCOM** %hhu %hhu %hhu %hhu %hhu %hhu %hhu", 	2239
            //p_src[0],p_src[1],p_src[2],p_src[3],p_src[4],p_src[5],p_src[6]);
            int index = btif_get_latest_playing_device_idx();
            if(btif_avk_cb[index].avdt_sync)
                UpdateRptDelay(p_data_q_buf->enque_ns);

            osi_free(p_data_q_buf);
            p_curr += q_bytes_left;
            /* If Callbak mechanism is enabled for streaming, read one media packet
             * from RxDataQ at a time. Following condition will break from loop after
             * reading one media packet*/
            if (enable_notification_cb) {
                // wait for next callback, return
                break;
            }
        }
        else
        {
            /* we don't have enough space left in input buffer to fit a packet */
            break;
        }

    }
    if (codec_type == BTIF_AVK_CODEC_SBC && !enable_notification_cb)
    {
        // the first one byte store the total number of sbc frames
        *(p_start + rtp_offset) = total_frames;
    }
    BTIF_TRACE_DEBUG(" %s Wrote %d bytes",__FUNCTION__, (p_curr - p_start));
    pthread_mutex_unlock(&sink_data_q_lock);

    return (p_curr - p_start);
}

/*******************************************************************************
**
** Function         update_streaming_device_vendor
**
** Description      Updates the current streaming device from apps
**
** Returns          void
**
*******************************************************************************/
void update_streaming_device_vendor(bt_bdaddr_t *bd_addr)
{
    bdstr_t addr1;
    BTIF_TRACE_DEBUG(" %s ", __FUNCTION__);
    memset(&streaming_bda, 0, sizeof(bt_bdaddr_t));
    memcpy(&streaming_bda, bd_addr, sizeof(bt_bdaddr_t));
    BTIF_TRACE_DEBUG(" %s streaming bda %s ", __FUNCTION__, bdaddr_to_string(&streaming_bda, &addr1, sizeof(addr1)));

}

/*******************************************************************************
**
** Function         update_qahw_delay_vendor
**
** Description      Updates decoding delay during decoding non_SBC stream in LPASS from apps
**
** Returns          void
**
*******************************************************************************/
void update_qahw_delay_vendor(uint16_t qahwdelay)
{
    int index = btif_get_latest_playing_device_idx();
    if(btif_avk_cb[index].avdt_sync != TRUE )
    {
        BTIF_TRACE_DEBUG(" %s ~~ delay report feature is not enabled, return .", __FUNCTION__);
        return;
    }
    if(btif_avk_cb[index].sink_codec_type == BTIF_AVK_CODEC_SBC && enable_stack_sbc_decoding)
    {
        BTIF_TRACE_DEBUG(" %s ~~ SBC stream has been decoded in stack, no need update qahw delay, return .", __FUNCTION__);
        return;
    }
    BTIF_TRACE_DEBUG(" %s ~~ qahwdelay = [%d]", __FUNCTION__, qahwdelay);
    qahw_delay = qahwdelay;
}

static void is_value_to_be_updated(void *ptr1, void *ptr2, UINT8 num_of_bytes)
{
    UINT8 *p1 = (UINT8*)ptr1;
    UINT8 *p2 = (UINT8*)ptr2;
    int i;

    if (p1 == NULL || p2 == NULL)
        return;
    for (i = 0; i < num_of_bytes; i ++) {
        if (!(*p1 & *p2)) {
            *p1 |= *p2;
        }
        p1 ++;
        p2 ++;
        if (p1 == NULL || p2 == NULL)
            return;
   }
}

/*******************************************************************************
**
** Function         update_supported_codecs_param
**
** Description      Updates the codecs supported by Sink as requested by Application Layer
**
** Returns          bt_status_t
**
*******************************************************************************/
static bt_status_t update_supported_codecs_param_vendor(btav_codec_configuration_t
        *p_codec_config_list, uint8_t num_codec_configs)
{
    int i, j;

    if (num_codec_configs == 0 || num_codec_configs > MAX_NUM_CODEC_CONFIGS) {
        BTIF_TRACE_ERROR(" %s Invalid num_codec_configs = %d",
            __func__, num_codec_configs);
        return BT_STATUS_PARM_INVALID;
    }

    if (!p_codec_config_list) {
        BTIF_TRACE_ERROR(" %s codec list is NULL", __func__);
        return BT_STATUS_PARM_INVALID;
    }

    // Check if the codec params sent by upper layers are valid or not.
    for (i = 0; i < num_codec_configs; i ++) {
        switch (p_codec_config_list[i].codec_type) {
            case A2DP_SINK_AUDIO_CODEC_SBC:
                switch (p_codec_config_list[i].codec_config.sbc_config.samp_freq) {
                    case SBC_SAMP_FREQ_16:
                    case SBC_SAMP_FREQ_32:
                    case SBC_SAMP_FREQ_44:
                    case SBC_SAMP_FREQ_48:
                        break;
                    default:
                        BTIF_TRACE_ERROR(" %s Invalid SBC freq = %d",
                            __func__, p_codec_config_list[i].codec_config.sbc_config.samp_freq);
                        return BT_STATUS_PARM_INVALID;
                }
                break;
#if defined(AAC_DECODER_INCLUDED) && (AAC_DECODER_INCLUDED == TRUE)
            case A2DP_SINK_AUDIO_CODEC_AAC:
                switch (p_codec_config_list[i].codec_config.aac_config.sampling_freq) {
                    case AAC_SAMP_FREQ_8000:
                    case AAC_SAMP_FREQ_11025:
                    case AAC_SAMP_FREQ_12000:
                    case AAC_SAMP_FREQ_16000:
                    case AAC_SAMP_FREQ_22050:
                    case AAC_SAMP_FREQ_24000:
                    case AAC_SAMP_FREQ_32000:
                    case AAC_SAMP_FREQ_44100:
                    case AAC_SAMP_FREQ_48000:
                    case AAC_SAMP_FREQ_64000:
                    case AAC_SAMP_FREQ_88200:
                    case AAC_SAMP_FREQ_96000:
                        break;
                    default:
                        BTIF_TRACE_ERROR(" %s Invalid AAC freq = %d",
                            __func__, p_codec_config_list[i].codec_config.aac_config.sampling_freq);
                        return BT_STATUS_PARM_INVALID;
                }
                switch (p_codec_config_list[i].codec_config.aac_config.obj_type) {
                    case AAC_OBJ_TYPE_MPEG_2_AAC_LC:
                    case AAC_OBJ_TYPE_MPEG_4_AAC_LC:
                        break;
                    case AAC_OBJ_TYPE_MPEG_4_AAC_LTP:
                    case AAC_OBJ_TYPE_MPEG_4_AAC_SCA:
                        BTIF_TRACE_ERROR(" %s AAC Object Type = %d currently not supported",
                            __func__, p_codec_config_list[i].codec_config.aac_config.obj_type);
                        return BT_STATUS_UNSUPPORTED;
                    default:
                        BTIF_TRACE_ERROR(" %s Invalid AAC Object Type = %d",
                            __func__, p_codec_config_list[i].codec_config.aac_config.obj_type);
                        return BT_STATUS_PARM_INVALID;
                }
                break;
#endif
#if defined(MP3_DECODER_INCLUDED) && (MP3_DECODER_INCLUDED == TRUE)
            case A2DP_SINK_AUDIO_CODEC_MP3:
                switch (p_codec_config_list[i].codec_config.mp3_config.sampling_freq) {
                    case MP3_SAMP_FREQ_16000:
                    case MP3_SAMP_FREQ_22050:
                    case MP3_SAMP_FREQ_24000:
                    case MP3_SAMP_FREQ_32000:
                    case MP3_SAMP_FREQ_44100:
                    case MP3_SAMP_FREQ_48000:
                        break;
                    default:
                        BTIF_TRACE_ERROR(" %s Invalid MP3 freq = %d",
                            __func__, p_codec_config_list[i].codec_config.mp3_config.sampling_freq);
                        return BT_STATUS_PARM_INVALID;
                }
                switch (p_codec_config_list[i].codec_config.mp3_config.layer) {
                    case MP3_LAYER_3:
                        break;
                    case MP3_LAYER_1:
                    case MP3_LAYER_2:
                        BTIF_TRACE_ERROR(" %s MP3 layer = %d currently not supported",
                            __func__, p_codec_config_list[i].codec_config.mp3_config.layer);
                        return BT_STATUS_UNSUPPORTED;
                    default:
                        BTIF_TRACE_ERROR(" %s Invalid MP3 layer = %d",
                            __func__, p_codec_config_list[i].codec_config.mp3_config.layer);
                        return BT_STATUS_PARM_INVALID;
                }
                break;
#endif
#if defined(APTX_CLASSIC_DECODER_INCLUDED) && (APTX_CLASSIC_DECODER_INCLUDED == TRUE)
            case A2DP_SINK_AUDIO_CODEC_APTX:
                switch (p_codec_config_list[i].codec_config.aptx_config.sampling_freq) {
                    case APTX_SAMPLERATE_44100:
                    case APTX_SAMPLERATE_48000:
                        break;
                    default:
                        BTIF_TRACE_ERROR(" %s Invalid APTX freq = %d",
                            __func__, p_codec_config_list[i].codec_config.aptx_config.sampling_freq);
                        return BT_STATUS_PARM_INVALID;
                }
                break;
#endif
            default:
                BTIF_TRACE_ERROR(" %s Invalid codec type = %d",
                    __func__, p_codec_config_list[i].codec_type);
                return BT_STATUS_PARM_INVALID;
        }
    }

    pthread_mutex_lock(&sink_codec_q_lock);
    if (p_bta_avk_codec_pri_list == NULL) {
        BTIF_TRACE_ERROR(" %s p_bta_avk_codec_pri_list is NULL returning!!", __func__);
        pthread_mutex_unlock(&sink_codec_q_lock);
        return BT_STATUS_NOT_READY;
    }

    tA2D_SBC_CIE sbc_supported_cap;
#if defined(AAC_DECODER_INCLUDED) && (AAC_DECODER_INCLUDED == TRUE)
    tA2D_AAC_CIE aac_supported_cap;
#endif
#if defined(MP3_DECODER_INCLUDED) && (MP3_DECODER_INCLUDED == TRUE)
    tA2D_MP3_CIE mp3_supported_cap;
#endif
#if defined(APTX_CLASSIC_DECODER_INCLUDED) && (APTX_CLASSIC_DECODER_INCLUDED == TRUE)
    tA2D_APTX_CIE aptx_supported_cap;
#endif
    UINT8 codec_info[BTIF_SV_AVK_AA_SEP_INDEX][AVDT_CODEC_SIZE];

    /* Copy the codec parameters passed from application layer to create a pointer to
     * preferred codec list for outgoing connection */
    /* Free the memory already allocated and reallocate fresh memory */
    osi_free(p_bta_avk_codec_pri_list);
    p_bta_avk_codec_pri_list = osi_calloc((num_codec_configs +
        BTIF_SV_AVK_AA_SEP_INDEX) * sizeof(tBTA_AVK_CO_CODEC_CAP_LIST));
    if (p_bta_avk_codec_pri_list == NULL) {
        BTIF_TRACE_ERROR(" %s p_bta_avk_codec_pri_list is NULL returning!!", __func__);
        pthread_mutex_unlock(&sink_codec_q_lock);
        return BT_STATUS_NOMEM;
    }
    /* Set codec supported capabilities to mandatory capabilities for each codec */
    memcpy(&sbc_supported_cap, &bta_avk_co_sbc_caps, sizeof(tA2D_SBC_CIE));
#if defined(AAC_DECODER_INCLUDED) && (AAC_DECODER_INCLUDED == TRUE)
    memcpy(&aac_supported_cap, &bta_avk_co_aac_caps, sizeof(tA2D_AAC_CIE));
#endif
#if defined(MP3_DECODER_INCLUDED) && (MP3_DECODER_INCLUDED == TRUE)
    memcpy(&mp3_supported_cap, &bta_avk_co_mp3_caps, sizeof(tA2D_MP3_CIE));
#endif
#if defined(APTX_CLASSIC_DECODER_INCLUDED) && (APTX_CLASSIC_DECODER_INCLUDED == TRUE)
    memcpy(&aptx_supported_cap, &bta_avk_co_aptx_caps, sizeof(tA2D_APTX_CIE));
#endif
    for (i = 0; i < num_codec_configs; i ++) {
        p_bta_avk_codec_pri_list[i].codec_type =
            p_codec_config_list[i].codec_type;
        switch (p_codec_config_list[i].codec_type) {
            case A2DP_SINK_AUDIO_CODEC_SBC:
                /* Copy Mandatory SBC codec parameters */
                memcpy(&p_bta_avk_codec_pri_list[i].codec_cap.sbc_caps,
                    &bta_avk_co_sbc_caps, sizeof(tA2D_SBC_CIE));
                /* Update sampling frequency as per Application layer */
                p_bta_avk_codec_pri_list[i].codec_cap.sbc_caps.samp_freq =
                p_codec_config_list[i].codec_config.sbc_config.samp_freq;
                /* Check if supported capability needs to be updated */
                is_value_to_be_updated(&sbc_supported_cap.samp_freq,
                    &p_codec_config_list[i].codec_config.sbc_config.samp_freq, 1);
                break;
#if defined(AAC_DECODER_INCLUDED) && (AAC_DECODER_INCLUDED == TRUE)
            case A2DP_SINK_AUDIO_CODEC_AAC:
                /* Copy Mandatory AAC codec parameters */
                memcpy(&p_bta_avk_codec_pri_list[i].codec_cap.aac_caps,
                    &bta_avk_co_aac_caps, sizeof(tA2D_AAC_CIE));
                /* Update sampling frequency and object type as per Application layer */
                p_bta_avk_codec_pri_list[i].codec_cap.aac_caps.samp_freq =
                p_codec_config_list[i].codec_config.aac_config.sampling_freq;
                p_bta_avk_codec_pri_list[i].codec_cap.aac_caps.object_type =
                p_codec_config_list[i].codec_config.aac_config.obj_type;
                /* Check if supported capability needs to be updated */
                is_value_to_be_updated(&aac_supported_cap.samp_freq,
                    &p_codec_config_list[i].codec_config.aac_config.sampling_freq, 2);
                is_value_to_be_updated(&aac_supported_cap.object_type,
                    &p_codec_config_list[i].codec_config.aac_config.obj_type, 1);
                break;
#endif
#if defined(MP3_DECODER_INCLUDED) && (MP3_DECODER_INCLUDED == TRUE)
            case A2DP_SINK_AUDIO_CODEC_MP3:
                /* Copy Mandatory MP3 codec parameters */
                memcpy(&p_bta_avk_codec_pri_list[i].codec_cap.mp3_caps,
                    &bta_avk_co_mp3_caps, sizeof(tA2D_MP3_CIE));
                /* Update sampling frequency and layer as per Application layer */
                p_bta_avk_codec_pri_list[i].codec_cap.mp3_caps.samp_freq =
                p_codec_config_list[i].codec_config.mp3_config.sampling_freq;
                p_bta_avk_codec_pri_list[i].codec_cap.mp3_caps.layer =
                p_codec_config_list[i].codec_config.mp3_config.layer;
                /* Check if supported capability needs to be updated */
                is_value_to_be_updated(&mp3_supported_cap.samp_freq,
                    &p_codec_config_list[i].codec_config.mp3_config.sampling_freq, 1);
                is_value_to_be_updated(&mp3_supported_cap.layer,
                    & p_codec_config_list[i].codec_config.mp3_config.layer, 1);
                break;
#endif
#if defined(APTX_CLASSIC_DECODER_INCLUDED) && (APTX_CLASSIC_DECODER_INCLUDED == TRUE)
            case A2DP_SINK_AUDIO_CODEC_APTX:
                /* Update Codec Type for APTX */
                p_bta_avk_codec_pri_list[i].codec_type = A2D_NON_A2DP_MEDIA_CT;
                /* Copy Mandatory APTX codec parameters */
                memcpy(&p_bta_avk_codec_pri_list[i].codec_cap.aptx_caps,
                    &bta_avk_co_aptx_caps, sizeof(tA2D_APTX_CIE));
                /* Update sampling frequency as per Application layer */
                p_bta_avk_codec_pri_list[i].codec_cap.aptx_caps.sampleRate =
                p_codec_config_list[i].codec_config.aptx_config.sampling_freq;
                /* Check if supported capability needs to be updated */
                is_value_to_be_updated(&aptx_supported_cap.sampleRate,
                    &p_codec_config_list[i].codec_config.aptx_config.sampling_freq, 1);
                break;
#endif
        }
    }

    uint8_t codec_type_list[BTIF_SV_AVK_AA_SEP_INDEX];
    uint8_t vnd_id_list[BTIF_SV_AVK_AA_SEP_INDEX];
    uint8_t codec_id_list[BTIF_SV_AVK_AA_SEP_INDEX];
    uint8_t codec_type_added[MAX_NUM_CODEC_CONFIGS];
    memset(codec_type_list, A2DP_SINK_AUDIO_CODEC_SBC, BTIF_SV_AVK_AA_SEP_INDEX);
    memset(vnd_id_list, 0, BTIF_SV_AVK_AA_SEP_INDEX);
    memset(codec_id_list, 0, BTIF_SV_AVK_AA_SEP_INDEX);
    memset(codec_type_added, 0, MAX_NUM_CODEC_CONFIGS);

    /* Remove duplicate codecs from list. This will be used for hiding/showing codecs
     * for response to AVDTP discover command */
    j = 0;
    for (i = 0; i < num_codec_configs; i ++) {
        if (!codec_type_added[p_codec_config_list[i].codec_type]) {
#if defined(APTX_CLASSIC_DECODER_INCLUDED) && (APTX_CLASSIC_DECODER_INCLUDED == TRUE)
            if (p_codec_config_list[i].codec_type ==
                A2DP_SINK_AUDIO_CODEC_APTX) {
                codec_type_list[j] = A2D_NON_A2DP_MEDIA_CT;
                vnd_id_list[j] = A2D_APTX_VENDOR_ID;
                codec_id_list[j] = A2D_APTX_CODEC_ID_BLUETOOTH;
            }
            else
#endif
            {
                codec_type_list[j] = p_codec_config_list[i].codec_type;
            }
            codec_type_added[p_codec_config_list[i].codec_type] = 1;
            j ++;
            if (j >= BTIF_SV_AVK_AA_SEP_INDEX) {
                BTIF_TRACE_ERROR(" %s num of different codecs(%d) exceeds max limit",
                    __func__, j);
                break;
            }
        }
    }

    /* Add mandatory codec for all supported codec in the end of priority list to handle
         * case if the codec parameters sent by upper layers are not capable of creating connection.
         * In that case, use the below parameters to create connection. in order of priority of
         * APTX > AAC > MP3 > SBC */
#if defined(APTX_CLASSIC_DECODER_INCLUDED) && (APTX_CLASSIC_DECODER_INCLUDED == TRUE)
    if (codec_type_added[A2DP_SINK_AUDIO_CODEC_APTX]) {
        p_bta_avk_codec_pri_list[num_codec_configs].codec_type
            = A2D_NON_A2DP_MEDIA_CT;
        /* Copy Mandatory APTX codec parameters */
        memcpy(&p_bta_avk_codec_pri_list[num_codec_configs ++]
            .codec_cap.aptx_caps, &bta_avk_co_aptx_caps,
            sizeof(tA2D_APTX_CIE));
        BTIF_TRACE_DEBUG(" %s Added Mandatory APTX codec at index %d",
            __func__, num_codec_configs - 1);
    }
#endif
#if defined(AAC_DECODER_INCLUDED) && (AAC_DECODER_INCLUDED == TRUE)
    if (codec_type_added[A2DP_SINK_AUDIO_CODEC_AAC]) {
        p_bta_avk_codec_pri_list[num_codec_configs].codec_type
            = A2DP_SINK_AUDIO_CODEC_AAC;
        /* Copy Mandatory AAC codec parameters */
        memcpy(&p_bta_avk_codec_pri_list[num_codec_configs ++]
            .codec_cap.aac_caps, &bta_avk_co_aac_caps,
            sizeof(tA2D_AAC_CIE));
        BTIF_TRACE_DEBUG(" %s Added Mandatory AAC codec at index %d",
            __func__, num_codec_configs - 1);
    }
#endif
#if defined(MP3_DECODER_INCLUDED) && (MP3_DECODER_INCLUDED == TRUE)
    if (codec_type_added[A2DP_SINK_AUDIO_CODEC_MP3]) {
        p_bta_avk_codec_pri_list[num_codec_configs].codec_type
            = A2DP_SINK_AUDIO_CODEC_MP3;
        /* Copy Mandatory MP3 codec parameters */
        memcpy(&p_bta_avk_codec_pri_list[num_codec_configs ++]
            .codec_cap.mp3_caps, &bta_avk_co_mp3_caps,
            sizeof(tA2D_MP3_CIE));
        BTIF_TRACE_DEBUG(" %s Added Mandatory MP3 codec at index %d",
            __func__, num_codec_configs - 1);
    }
#endif
    p_bta_avk_codec_pri_list[num_codec_configs].codec_type
        = A2DP_SINK_AUDIO_CODEC_SBC;
    /* Copy Mandatory SBC codec parameters */
    memcpy(&p_bta_avk_codec_pri_list[num_codec_configs ++]
        .codec_cap.sbc_caps, &bta_avk_co_sbc_caps, sizeof(tA2D_SBC_CIE));
    BTIF_TRACE_DEBUG(" %s Added Mandatory SBC codec at index %d",
        __func__, num_codec_configs - 1);

    j = 0;
    /* Create Codec Config array for supported types as per application layer */
    memset(codec_info, 0, BTIF_SV_AVK_AA_SEP_INDEX * AVDT_CODEC_SIZE);
    A2D_BldSbcInfo(AVDT_MEDIA_AUDIO, &sbc_supported_cap, codec_info[j ++]);
    memcpy(&bta_avk_supp_codec_cap[BTIF_SV_AVK_AA_SBC_INDEX]
        .codec_cap.sbc_caps, &sbc_supported_cap, sizeof(tA2D_SBC_CIE));
#if defined(AAC_DECODER_INCLUDED) && (AAC_DECODER_INCLUDED == TRUE)
    if (codec_type_added[A2DP_SINK_AUDIO_CODEC_AAC]) {
        A2D_BldAacInfo(AVDT_MEDIA_AUDIO, &aac_supported_cap, codec_info[j ++]);
        memcpy(&bta_avk_supp_codec_cap[BTIF_SV_AVK_AA_AAC_INDEX]
            .codec_cap.aac_caps, &aac_supported_cap, sizeof(tA2D_AAC_CIE));
    }
#endif
#if defined(MP3_DECODER_INCLUDED) && (MP3_DECODER_INCLUDED == TRUE)
    if (codec_type_added[A2DP_SINK_AUDIO_CODEC_MP3]) {
        A2D_BldMp3Info(AVDT_MEDIA_AUDIO, &mp3_supported_cap, codec_info[j ++]);
        memcpy(&bta_avk_supp_codec_cap[BTIF_SV_AVK_AA_MP3_INDEX]
            .codec_cap.mp3_caps, &mp3_supported_cap, sizeof(tA2D_MP3_CIE));
    }
#endif
#if defined(APTX_CLASSIC_DECODER_INCLUDED) && (APTX_CLASSIC_DECODER_INCLUDED == TRUE)
    if (codec_type_added[A2DP_SINK_AUDIO_CODEC_APTX]) {
        A2D_BldAptxInfo(AVDT_MEDIA_AUDIO, &aptx_supported_cap, codec_info[j ++]);
        memcpy(&bta_avk_supp_codec_cap[BTIF_SV_AVK_AA_APTX_INDEX]
            .codec_cap.aptx_caps, &aptx_supported_cap, sizeof(tA2D_APTX_CIE));
    }
#endif
    bta_avk_num_codec_configs = num_codec_configs;
    BTIF_TRACE_DEBUG(" %s Num_codec_configs = %d", __func__, num_codec_configs);
    for (i = 0; i < j; i ++) {
        BTIF_TRACE_VERBOSE(" %s %d %d %d %d %d %d %d %d %d", __func__,
            codec_info[i][0], codec_info[i][1], codec_info[i][2], codec_info[i][3],
            codec_info[i][4], codec_info[i][5], codec_info[i][6], codec_info[i][7],
            codec_info[i][8]);
    }
    /* Update the codec config supported paratmers so that correct response can be
     * sent for AVDTP discover and get capabilities command from remote device */
    BTA_AvkUpdateCodecSupport(codec_type_list, vnd_id_list, codec_id_list,
        codec_info, j);
    pthread_mutex_unlock(&sink_codec_q_lock);
    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         update_flush_device_vendor
**
** Description      Updates the current streaming device from apps
**
** Returns          void
**
*******************************************************************************/
void update_flushing_device_vendor(bt_bdaddr_t *bd_addr)
{
    BTIF_TRACE_DEBUG(" %s ", __FUNCTION__);
    bdstr_t addr1, addr2;
    tBT_SINK_DATA_HDR* p_data_q_buf;
    bt_bdaddr_t bda;
    int count = 0, queue_size = 0;
    queue_size = fixed_queue_length(RxDataQ);
    BTIF_TRACE_DEBUG(" %s queue_size = %d", __FUNCTION__, queue_size);

    if(queue_size == 0)
        return;

    pthread_mutex_lock(&sink_data_q_lock);
    list_t *list = fixed_queue_get_list(RxDataQ);
    for (const list_node_t *node = list_begin(list); node != list_end(list); )
    {
        p_data_q_buf = (tBT_SINK_DATA_HDR *)list_node(node);
        node = list_next(node);
        bdcpy(bda.address, p_data_q_buf->bd_addr);
        BTIF_TRACE_DEBUG(" %s flushing_bda %s p_data_q_buf->bd_addr %s", __FUNCTION__,
            bdaddr_to_string(bd_addr, &addr1, sizeof(addr1)),
            bdaddr_to_string(&bda, &addr2, sizeof(addr2)));

        if ((bd_addr != NULL) &&
            !memcmp(bd_addr->address, p_data_q_buf->bd_addr, sizeof(BD_ADDR)))
        {
            BTIF_TRACE_DEBUG("%s flushing this dev packets, dequeue this packet",
                __FUNCTION__);
            fixed_queue_try_remove_from_queue(RxDataQ,(void *)p_data_q_buf);
            osi_free(p_data_q_buf);
        }
    }
    pthread_mutex_unlock(&sink_data_q_lock);
}
/*******************************************************************************
 **
 ** Function         attach_timestamp
 **
 ** Description      attaches current timestamp to media packet before queing
                     it to Data Queue.
 **
 ** Returns          address where media data should be wriiten after attaching
                     current timestamp.
 **
 *******************************************************************************/

void *attach_timestamp(UINT8 *p_timestamp) {
    struct timespec ts_now;
    memset(&ts_now, 0, sizeof(ts_now));
    clock_gettime(CLOCK_REALTIME, &ts_now);
    UINT64 timestamp = (UINT64)ts_now.tv_sec * 1000000 + ts_now.tv_nsec/1000;
    memcpy(p_timestamp, (UINT8*)&timestamp, sizeof(UINT64));
    BTIF_TRACE_DEBUG("%s: Attach current timestamp %llu to media data",  __FUNCTION__, timestamp);
    return (UINT8*)(p_timestamp + sizeof(UINT64));
}

/*******************************************************************************
**
** Function         get_a2dp_sink_streaming_data
**
** Description      get a2dp sink data stored from Data Q
**
** Returns          number of bytes returned
**
*******************************************************************************/
static uint32_t get_a2dp_sink_streaming_data_vendor (UINT16 codec_type, UINT8* data, uint32_t size)
{
    uint16_t q_bytes_left = 0;// bytes left in topmost element of PCM Q
    tBT_SINK_DATA_HDR* p_data_q_buf; // pointer to first element in que;
    uint32_t bytes_to_be_written = size;// bytes written to buffer supplied by app.
    UINT8* p_src; UINT8* p_dest;
    bdstr_t addr1, addr2;
    bt_bdaddr_t bda;
    BTIF_TRACE_DEBUG(" %s size = %d", __FUNCTION__, size);
    if (codec_type != A2DP_SINK_AUDIO_CODEC_PCM) {
        return get_frame_aligned_data(codec_type, data, size);
    }
    pthread_mutex_lock(&sink_data_q_lock);
    if(fixed_queue_is_empty(RxDataQ)) {
        BTIF_TRACE_DEBUG("%s Sink Que Empty, returning", __FUNCTION__);
        pthread_mutex_unlock(&sink_data_q_lock);
        return 0;
    }

    // consistency check: check codec from remote and codec info in Q
    p_data_q_buf = (tBT_SINK_DATA_HDR *)fixed_queue_try_peek_first(RxDataQ);
    if (p_data_q_buf == NULL)
    {
       BTIF_TRACE_IMP("%s p_data_q_buf is NULl", __FUNCTION__);
       pthread_mutex_unlock(&sink_data_q_lock);
       return 0;
    }
    if (codec_type != p_data_q_buf->codec_type)
    {
        BTIF_TRACE_IMP("%s codec mismatch, returning, requested_codec_type %d, codec_present %d",
            __FUNCTION__, codec_type, p_data_q_buf->codec_type);
        p_data_q_buf = (tBT_SINK_DATA_HDR *)fixed_queue_try_dequeue(RxDataQ);
        pthread_mutex_unlock(&sink_data_q_lock);
        osi_free(p_data_q_buf);
        return 0;
    }
    while ((bytes_to_be_written > 0) && (!fixed_queue_is_empty(RxDataQ)))
    {
        p_data_q_buf = (tBT_SINK_DATA_HDR *)fixed_queue_try_peek_first(RxDataQ);
        if (p_data_q_buf == NULL)
            break;
        bdcpy(bda.address, p_data_q_buf->bd_addr);
        BTIF_TRACE_DEBUG(" %s bd_addr %s p_data_q_buf->bd_addr %s", __FUNCTION__,
            bdaddr_to_string(&streaming_bda, &addr1, sizeof(addr1)),
            bdaddr_to_string(&bda, &addr2, sizeof(addr2)));

        if ((&streaming_bda != NULL) &&
                memcmp(&streaming_bda.address, p_data_q_buf->bd_addr, sizeof(BD_ADDR)))
        {
            BTIF_TRACE_DEBUG("%s app fetching data for diff device, dequeue this packet",
                    __FUNCTION__);
            p_data_q_buf = (tBT_SINK_DATA_HDR *)fixed_queue_try_dequeue(RxDataQ);
            osi_free(p_data_q_buf);
            continue;
        }
        q_bytes_left = p_data_q_buf->len - p_data_q_buf->offset;
        // if callback mechanism is enabled, update q_bytes_left by size of timestamp
        q_bytes_left += (enable_notification_cb ? sizeof(UINT64): 0);
        BTIF_TRACE_DEBUG(" %s Q_Len %d, bytes_to_be_written %d, bytes_left_in_Q %d", __FUNCTION__,
                 fixed_queue_length(RxDataQ), bytes_to_be_written, q_bytes_left);
        if (bytes_to_be_written >= q_bytes_left)
        {
            // read from topmost element and deque it
            p_data_q_buf = (tBT_SINK_DATA_HDR *)fixed_queue_try_dequeue(RxDataQ);
            if (p_data_q_buf == NULL) {
                BTIF_TRACE_IMP(" %s: p_data_q_buf is NULL", __FUNCTION__);
                break;
            }
            p_dest = data + (size - bytes_to_be_written);
            p_src = (UINT8*)(p_data_q_buf + 1) + p_data_q_buf->offset;
            memcpy(p_dest, p_src, q_bytes_left);

            int index = btif_get_latest_playing_device_idx();
            if(btif_avk_cb[index].avdt_sync)
                UpdateRptDelay(p_data_q_buf->enque_ns);

            osi_free(p_data_q_buf);
            bytes_to_be_written = bytes_to_be_written - q_bytes_left;
            if (enable_notification_cb) {
                // when callback mechanism is enabled, read one packet at a time and return
                break;
            }
        }
        else
        {
            // read only required data and keep the node in Q
            p_dest = data + (size - bytes_to_be_written);
            p_src = (UINT8*)(p_data_q_buf + 1) + p_data_q_buf->offset;
            memcpy(p_dest, p_src, bytes_to_be_written);
            p_data_q_buf->offset += bytes_to_be_written;
            bytes_to_be_written = 0;
        }

    }
    BTIF_TRACE_DEBUG(" %s Wrote %d bytes",__FUNCTION__, size - bytes_to_be_written);
    pthread_mutex_unlock(&sink_data_q_lock);
    return (size - bytes_to_be_written);
}

/*******************************************************************************
 **
 ** Function         btif_avk_media_fetch_pcm_data
 **
 ** Description      fetch PCM data
 **
 ** Returns          void
 **
 *******************************************************************************/
uint32_t btif_avk_media_fetch_pcm_data(UINT16 codec_type, UINT8 *data, UINT32 size)
{
    return get_a2dp_sink_streaming_data_vendor(codec_type, data, size);
}

/*******************************************************************************
 **
 ** Function         btif_media_enque_sink_data
 **
 ** Description      queues a2dp Sink data
 **
 ** Returns          void
 **
 *******************************************************************************/
UINT32 btif_media_enque_sink_data(UINT16 codec_type, UINT8 *data, UINT16 size, BD_ADDR bd_addr, UINT64 enque_time)
{
    tBT_SINK_DATA_HDR* p_msg;
    bdstr_t addr1;
    BTIF_TRACE_DEBUG("%s", __FUNCTION__);
    pthread_mutex_lock(&sink_data_q_lock);
    UINT16 alloc_packet_size = sizeof(tBT_SINK_DATA_HDR) + size;
    alloc_packet_size += (enable_notification_cb ? sizeof(UINT64) : 0);
    if(fixed_queue_length(RxDataQ) >= MAX_A2DP_SINK_DATA_QUEUE_SZ || (RxDataQ == NULL))
    {
        BTIF_TRACE_ERROR(" %s DATA Que not exit or Full size =%d, returning",
        __FUNCTION__,fixed_queue_length(RxDataQ));

        /* Code to give callback to BT-APP layer that Data is queued in Data Queue*/
        if (enable_notification_cb && bt_av_sink_vendor_callbacks != NULL) {
            bt_bdaddr_t bdAddr;
            memcpy(bdAddr.address, &bd_addr, sizeof(BD_ADDR));
            HAL_CBACK(bt_av_sink_vendor_callbacks, audio_data_read_vendor_cb, &bdAddr);
        }

        pthread_mutex_unlock(&sink_data_q_lock);
        return  fixed_queue_length(RxDataQ);
    }
    if ((p_msg = (tBT_SINK_DATA_HDR *) osi_malloc(alloc_packet_size)) != NULL)
    {
        UINT8 *p_dest, *p_timestamp;;
        if (enable_notification_cb) {
            p_timestamp = (UINT8*)(p_msg + 1);
            p_dest = attach_timestamp(p_timestamp);
        } else {
            p_dest = (UINT8*)(p_msg + 1);
        }
        memcpy(p_dest, (UINT8*)(data), size);
        p_msg->len = size;
        p_msg->offset = 0;
        p_msg->codec_type = codec_type;
        memcpy(p_msg->bd_addr, bd_addr, sizeof(BD_ADDR));

        int index = btif_avk_idx_by_bdaddr(bd_addr);
        if (index >= btif_max_avk_clients)
        {
            BTIF_TRACE_DEBUG("%s Invalid index for device", __FUNCTION__);
            osi_free(p_msg);
            pthread_mutex_unlock(&sink_data_q_lock);
            return fixed_queue_length(RxDataQ);
        }
        if(btif_avk_cb[index].avdt_sync == TRUE )
        {
                /* non_SBC steam packet enque, fill the enqueq_ns current time */
                if(enque_time == 0)
                {
                    struct timespec ts_now;
                    memset(&ts_now, 0, sizeof(ts_now));
                    clock_gettime(CLOCK_BOOTTIME, &ts_now);
                    p_msg->enque_ns = (UINT64)ts_now.tv_sec * 1000000000 + ts_now.tv_nsec;
                    BTIF_TRACE_VERBOSE(" %s ~~ non_SBC steam packet enque, enque_ns = [%09llu]", __func__,p_msg->enque_ns);
                }
                else	/* SBC steam decoded packet enque, set the enque_ns to SBC packet enque time */
                {
                    p_msg->enque_ns = enque_time;
                    BTIF_TRACE_DEBUG(" %s ~~ SBC steam decoded packet enque, enque_ns = [%09llu]", __func__,p_msg->enque_ns);
                }
        }
        fixed_queue_enqueue(RxDataQ, p_msg);
        BTIF_TRACE_DEBUG("%s pkt_size %d  DATA_Q_Size %d bd_addr %s, codec_type = %d",
                  __FUNCTION__, size, fixed_queue_length(RxDataQ),
                  bdaddr_to_string((bt_bdaddr_t *)p_msg->bd_addr, &addr1, sizeof(addr1)),
                  p_msg->codec_type);
    }

    /* Code to give callback to BT-APP layer that Data is queued in Data Queue*/
    if (enable_notification_cb && bt_av_sink_vendor_callbacks != NULL) {
        bt_bdaddr_t bdAddr;
        memcpy(bdAddr.address, &bd_addr, sizeof(BD_ADDR));
        HAL_CBACK(bt_av_sink_vendor_callbacks, audio_data_read_vendor_cb, &bdAddr);
    }
    pthread_mutex_unlock(&sink_data_q_lock);
    return fixed_queue_length(RxDataQ);
}
static void btif_avk_media_clear_pcm_queue()
{
    BTIF_TRACE_DEBUG(" Clear A2DP Data QUeue ");
     pthread_mutex_lock(&sink_data_q_lock);
    while (!fixed_queue_is_empty(RxDataQ))
    {
        osi_free(fixed_queue_try_dequeue(RxDataQ));
    }
    pthread_mutex_unlock(&sink_data_q_lock);
}
#ifdef USE_AUDIO_TRACK
/*******************************************************************************
**
** Function         sink_audio_focus_status_vendor
**
** Description      Update Audio Focus State
**
** Returns          None
**
*******************************************************************************/
void sink_audio_focus_status_vendor(int state, bt_bdaddr_t *bd_addr)
{
    BTIF_TRACE_DEBUG(" sink_audio_focus_status  %d ",state);
    btif_avk_a2dp_set_audio_focus_state(state);
}

/*******************************************************************************
**
** Function         btif_avk_queue_focus_rquest
**
** Description      This is used to move context to btif and queue audio_focus_request
**
** Returns          none
**
*******************************************************************************/
void btif_avk_queue_focus_rquest(void)
{
    btif_transfer_context(btif_avk_handle_event, BTIF_AVK_SINK_FOCUS_REQ_EVT, NULL, 0, NULL);
}
#endif

void btif_avk_get_latest_playing_device(BD_ADDR address)
{
    int index;
    index = btif_get_latest_playing_device_idx();
    if (index < btif_max_avk_clients)
    {
        //copy bdaddrsss
        bdcpy(address, btif_avk_cb[index].peer_bda.address);
    }
    else
    {
        bdcpy(address, bd_null);
    }
}

BOOLEAN btif_avk_is_device_connected(BD_ADDR address)
{
    btif_sm_state_t state = btif_get_conn_state_of_device(address);

    if ((state == BTIF_AVK_STATE_OPENED) ||
        (state == BTIF_AVK_STATE_STARTED))
        return TRUE;
    else
        return FALSE;
}

/*This function will trigger remote suspend for currently
* playing device and then initiate START on Handoff device
* whose address is passed as an argument. */
/*******************************************************************************
**
** Function         btif_avk_trigger_dual_handoff
**
** Description      Trigger the DUAL HANDOFF
**
** Returns          void
**
*******************************************************************************/

void btif_avk_trigger_dual_handoff(BOOLEAN handoff, BD_ADDR address)
{
    int index;
    /*Get the current playing device*/
    BTIF_TRACE_DEBUG("%s", __FUNCTION__);
    index = btif_get_latest_playing_device_idx();
    if (index != btif_max_avk_clients)
    {
        btif_avk_cb[index].dual_handoff = handoff; /*Initiate Handoff*/
        /*Initiate SUSPEND for this device*/
        BTIF_TRACE_DEBUG("Initiate SUSPEND for this device on index = %d", index);
        btif_sm_dispatch(btif_avk_cb[index].sm_handle, BTIF_AVK_SUSPEND_STREAM_REQ_EVT, NULL);
        //btif_avk_dispatch_sm_event(BTIF_AVK_SUSPEND_STREAM_REQ_EVT, NULL, 0);
    }
    else
    {
        BTIF_TRACE_ERROR("Handoff on invalid index");
    }
}

/*******************************************************************************
**
** Function         btif_avk_trigger_suspend
**
** Description      Trigger suspend when multicast is ongoing for tuch tones
**                  and new ACL is created.
**
** Returns          void
**
*******************************************************************************/

void btif_avk_trigger_suspend()
{
    int index;
    /*Get the current playing device*/
    BTIF_TRACE_DEBUG("%s", __FUNCTION__);
    index = btif_get_latest_playing_device_idx();
    if (index <= btif_max_avk_clients)
    {
        /*Initiate SUSPEND for this device*/
        BTIF_TRACE_DEBUG("Initiate SUSPEND for this device on index = %d", index);
        btif_sm_dispatch(btif_avk_cb[index].sm_handle, BTIF_AVK_SUSPEND_STREAM_REQ_EVT, NULL);
    }
    else
    {
        BTIF_TRACE_ERROR("suspend on invalid index");
    }
}

/*******************************************************************************
**
** Function         connect
**
** Description      Establishes the AV signalling channel with the remote headset
**
** Returns          bt_status_t
**
*******************************************************************************/

static bt_status_t connect_int(bt_bdaddr_t *bd_addr, uint16_t uuid)
{
    btif_avk_connect_req_t connect_req;
    int i;
    connect_req.target_bda = bd_addr;
    connect_req.uuid = uuid;
    BTIF_TRACE_EVENT("%s", __FUNCTION__);

    for (i = 0; i < btif_max_avk_clients;)
    {
        if(btif_avk_get_valid_idx(i))
        {
            if (bdcmp(bd_addr->address, btif_avk_cb[i].peer_bda.address) == 0)
            {
                BTIF_TRACE_ERROR("Attempting connection for non idle device.. back off ");
                btif_queue_advance();
                return BT_STATUS_SUCCESS;
            }
            i++;
        }
        else
            break;
    }
    if (i == btif_max_avk_clients)
    {
        UINT8 rc_handle;
        bdstr_t bdstr;

        BTIF_TRACE_ERROR("%s: All indexes are full", __FUNCTION__);

        /* Multicast: Check if AV slot is available for connection
         * If not available, AV got connected to different devices.
         * Disconnect this RC connection without AV connection.
         */
        rc_handle = btif_avk_rc_get_connected_peer_handle(bd_addr->address);
        if (rc_handle != BTIF_AVK_RC_HANDLE_NONE)
        {
            BTIF_TRACE_IMP("Disconnect only AVRC on : %s",
                    bdaddr_to_string (bd_addr, &bdstr, sizeof(bdstr)));
            BTA_AvkCloseRc(rc_handle);
        }
        btif_queue_advance();
        return BT_STATUS_FAIL;
    }

    btif_sm_dispatch(btif_avk_cb[i].sm_handle, BTIF_AVK_CONNECT_REQ_EVT, (char*)&connect_req);


    return BT_STATUS_SUCCESS;
}

static bt_status_t sink_connect_src(bt_bdaddr_t *bd_addr)
{
    BTIF_TRACE_EVENT("%s", __FUNCTION__);
    CHECK_BTAVK_INIT();

    return btif_queue_connect(UUID_SERVCLASS_AUDIO_SINK, bd_addr, connect_int);
}

/*******************************************************************************
**
** Function         disconnect
**
** Description      Tears down the AV signalling channel with the remote headset
**
** Returns          bt_status_t
**
*******************************************************************************/
static bt_status_t disconnect(bt_bdaddr_t *bd_addr)
{
    BTIF_TRACE_EVENT("%s", __FUNCTION__);

    CHECK_BTAVK_INIT();

    /* Switch to BTIF context */
    return btif_transfer_context(btif_avk_handle_event, BTIF_AVK_DISCONNECT_REQ_EVT,
                                 (char*)bd_addr, sizeof(bt_bdaddr_t), NULL);
}

/*******************************************************************************
**
** Function         cleanup
**
** Description      Shuts down the AV interface and does the cleanup
**
** Returns          None
**
*******************************************************************************/
static void cleanup(int service_uuid)
{
    int i;
    BTIF_TRACE_IMP("AVK %s", __FUNCTION__);

    btif_transfer_context(btif_avk_handle_event, BTIF_AVK_CLEANUP_REQ_EVT,
            (char*)&service_uuid, sizeof(int), NULL);
    btif_disable_service(service_uuid);
}

static void cleanup_sink(void) {
    BTIF_TRACE_EVENT("%s", __FUNCTION__);
    cleanup(BTA_A2DP_SINK_SERVICE_ID);
    btif_avk_media_clear_pcm_queue();
    enable_stack_sbc_decoding = 0;
    qahw_delay = 0;
    pthread_mutex_lock(&sink_data_q_lock);
    fixed_queue_free(RxDataQ,NULL);
    RxDataQ = NULL;
    pthread_mutex_unlock(&sink_data_q_lock);
    pthread_mutex_destroy(&sink_data_q_lock);
}

static void cleanup_sink_vendor(void) {
    BTIF_TRACE_EVENT("%s", __FUNCTION__);
    if (bt_av_sink_vendor_callbacks)
    {
        bt_av_sink_vendor_callbacks = NULL;
    }
    pthread_mutex_lock(&sink_codec_q_lock);
    if (p_bta_avk_codec_pri_list != NULL) {
        osi_free(p_bta_avk_codec_pri_list);
        p_bta_avk_codec_pri_list = NULL;
    }
    pthread_mutex_unlock(&sink_codec_q_lock);
    pthread_mutex_destroy(&sink_codec_q_lock);
    BTIF_TRACE_EVENT("%s completed", __FUNCTION__);
}

static const btav_interface_t bt_av_sink_interface = {
    sizeof(btav_interface_t),
    init_sink,
    sink_connect_src,
    disconnect,
    cleanup_sink,
    NULL,
    NULL,
};

static const btav_sink_vendor_interface_t bt_avk_sink_vendor_interface = {
    sizeof(btav_sink_vendor_interface_t),
    init_sink_vendor,
#ifdef USE_AUDIO_TRACK
    sink_audio_focus_status_vendor,
#else
    NULL,
#endif
    get_a2dp_sink_streaming_data_vendor,
    update_streaming_device_vendor,
    update_flushing_device_vendor,
    cleanup_sink_vendor,
    update_qahw_delay_vendor,
    update_supported_codecs_param_vendor,
};

/*******************************************************************************
**
** Function         btif_avk_get_sm_handle
**
** Description      Fetches current av SM handle
**
** Returns          None
**
*******************************************************************************/
/* Media task uses this info
* But dont use it. */
btif_sm_handle_t btif_avk_get_sm_handle(void)
{
    return btif_avk_cb[0].sm_handle;
}

/*******************************************************************************
**
** Function         btif_avk_get_addr
**
** Description      Fetches current AV BD address
**
** Returns          BD address
**
*******************************************************************************/

bt_bdaddr_t btif_avk_get_addr(BD_ADDR address)
{
    int i;
    bt_bdaddr_t not_found ;
    memset (&not_found, 0, sizeof(bt_bdaddr_t));
    for (i = 0; i < btif_max_avk_clients; i++)
    {
        if (bdcmp(btif_avk_cb[i].peer_bda.address, address) == 0)
            return btif_avk_cb[i].peer_bda;
    }
    return not_found;
}

/*******************************************************************************
**
** Function         btif_avk_stream_ready
**
** Description      Checks whether AV is ready for starting a stream
**
** Returns          None
**
*******************************************************************************/

BOOLEAN btif_avk_stream_ready(void)
{
    int i;
    BOOLEAN status = FALSE;
    /* also make sure main adapter is enabled */
    if (btif_is_enabled() == 0)
    {
        BTIF_TRACE_EVENT("main adapter not enabled");
        return FALSE;
    }

    for (i = 0; i < btif_max_avk_clients; i++)
    {
        BTIF_TRACE_DEBUG("btif_avk_stream_ready flags: %d", btif_avk_cb[i].flags);
        btif_avk_cb[i].state = btif_sm_get_state(btif_avk_cb[i].sm_handle);
        /* Multicast:
         * If any of the stream is in pending suspend state when
         * we initiate start, it will result in inconsistent behavior
         * Check the pending SUSPEND flag and return failure
         * if suspend is in progress.
         */
        if (btif_avk_cb[i].dual_handoff ||
            (btif_avk_cb[i].flags & BTIF_AVK_FLAG_LOCAL_SUSPEND_PENDING))
        {
            status = FALSE;
            break;
        }
        else if (btif_avk_cb[i].flags &
            (BTIF_AVK_FLAG_REMOTE_SUSPEND|BTIF_AVK_FLAG_PENDING_STOP))
        {
            status = FALSE;
            break;
        }
        else if (btif_avk_cb[i].state == BTIF_AVK_STATE_OPENED)
        {
            status = TRUE;
        }
    }
    BTIF_TRACE_DEBUG("btif_avk_stream_ready: %d", status);
    return status;
}

/*******************************************************************************
**
** Function         btif_avk_stream_started_ready
**
** Description      Checks whether AV ready for media start in streaming state
**
** Returns          None
**
*******************************************************************************/

BOOLEAN btif_avk_stream_started_ready(void)
{
    int i;
    BOOLEAN status = FALSE;

    for (i = 0; i < btif_max_avk_clients; i++)
    {
        btif_avk_cb[i].state = btif_sm_get_state(btif_avk_cb[i].sm_handle);
        if (btif_avk_cb[i].dual_handoff)
        {
            BTIF_TRACE_ERROR("%s: Under Dual handoff ",__FUNCTION__ );
            status = FALSE;
            break;
        } else if (btif_avk_cb[i].flags &
            (BTIF_AVK_FLAG_LOCAL_SUSPEND_PENDING |
            BTIF_AVK_FLAG_REMOTE_SUSPEND |
            BTIF_AVK_FLAG_PENDING_STOP))
        {
            status = FALSE;
            break;
        } else if (btif_avk_cb[i].state == BTIF_AVK_STATE_STARTED)
        {
            status = TRUE;
        }
    }
    BTIF_TRACE_DEBUG("btif_avk_stream_started_ready: %d", status);
    return status;
}

/*******************************************************************************
**
** Function         btif_avk_dispatch_sm_event
**
** Description      Send event to AV statemachine
**
** Returns          None
**
*******************************************************************************/

/* used to pass events to AV statemachine from other tasks */
void btif_avk_dispatch_sm_event(btif_avk_sm_event_t event, void *p_data, int len)
{
    /* Switch to BTIF context */
    BTIF_TRACE_IMP("%s: event: %d, len: %d", __FUNCTION__, event, len);
    btif_transfer_context(btif_avk_handle_event, event,
                          (char*)p_data, len, NULL);
    BTIF_TRACE_IMP("%s: event %d sent", __FUNCTION__, event);
}

/*******************************************************************************
**
** Function         btif_avk_sink_execute_service
**
** Description      Initializes/Shuts down the service
**
** Returns          BT_STATUS_SUCCESS on success, BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bt_status_t btif_avk_sink_execute_service(BOOLEAN b_enable)
{
     int i;
     if (b_enable)
     {
         /* Added BTA_AVK_FEAT_NO_SCO_SSPD - this ensures that the BTA does not
          * auto-suspend av streaming on AG events(SCO or Call). The suspend shall
          * be initiated by the app/audioflinger layers */
        if(enable_delay_reporting) {
         BTA_AvkEnable(BTA_SEC_AUTHENTICATE, BTA_AVK_FEAT_NO_SCO_SSPD|BTA_AVK_FEAT_RCCT|
                                            BTA_AVK_FEAT_METADATA|BTA_AVK_FEAT_VENDOR|BTA_AVK_FEAT_BROWSE|
                BTA_AVK_FEAT_ADV_CTRL|BTA_AVK_FEAT_RCTG|BTA_AVK_FEAT_DELAY_RPT,
                bte_avk_callback);
        BTIF_TRACE_DEBUG("%s ~~ BTA_AvkEnable Added BTA_AVk_FEAT_DELAY_RPT!", __FUNCTION__);
        }
        else
        {
            BTA_AvkEnable(BTA_SEC_AUTHENTICATE, BTA_AVK_FEAT_NO_SCO_SSPD|BTA_AVK_FEAT_RCCT|
                BTA_AVK_FEAT_METADATA|BTA_AVK_FEAT_VENDOR|BTA_AVK_FEAT_BROWSE|
                                            BTA_AVK_FEAT_ADV_CTRL|BTA_AVK_FEAT_RCTG,
                                                                        bte_avk_callback);
            BTIF_TRACE_DEBUG("%s ~~ BTA_AvkEnable NOT Added BTA_AVk_FEAT_DELAY_RPT!", __FUNCTION__);
        }
         for (i = 0; i < btif_max_avk_clients; i++)
         {
             BTA_AvkRegister(BTA_AVK_CHNL_AUDIO, BTIF_AVK_SERVICE_NAME, 0, bte_avk_media_callback,
                                                                UUID_SERVCLASS_AUDIO_SINK);
         }
     }
     else {
         for (i = 0; i < btif_max_avk_clients; i++)
         {
             if (btif_avk_cb[i].sm_handle != NULL)
             {
                 BTIF_TRACE_IMP("%s: shutting down AV SM", __FUNCTION__);
                 btif_sm_shutdown(btif_avk_cb[i].sm_handle);
                 btif_avk_cb[i].sm_handle = NULL;
             }
             BTA_AvkDeregister(btif_avk_cb[i].bta_handle);
         }
         BTA_AvkDisable();
     }
     return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_avk_get_sink_interface
**
** Description      Get the AV callback interface for A2DP sink profile
**
** Returns          btav_interface_t
**
*******************************************************************************/
const btav_interface_t *btif_avk_get_sink_interface(void)
{
    BTIF_TRACE_EVENT("%s", __FUNCTION__);
    return &bt_av_sink_interface;
}

/*******************************************************************************
**
** Function         btif_avk_get_sink_vendor_interface
**
** Description      Get the AV vendor callback interface for A2DP sink profile
**
** Returns          btav_interface_t
**
*******************************************************************************/
const btav_sink_vendor_interface_t *btif_avk_get_sink_vendor_interface(void)
{
    BTIF_TRACE_IMP("%s", __FUNCTION__);
    return &bt_avk_sink_vendor_interface;
}

/*******************************************************************************
**
** Function         btif_avk_is_connected
**
** Description      Checks if av has a connected sink
**
** Returns          BOOLEAN
**
*******************************************************************************/
BOOLEAN btif_avk_is_connected(void)
{
    int i;
    BOOLEAN status = FALSE;
    for (i = 0; i < btif_max_avk_clients; i++)
    {
        btif_avk_cb[i].state = btif_sm_get_state(btif_avk_cb[i].sm_handle);
        if ((btif_avk_cb[i].state == BTIF_AVK_STATE_OPENED) ||
            (btif_avk_cb[i].state ==  BTIF_AVK_STATE_STARTED))
            status = TRUE;
    }
    return status;
}

/*******************************************************************************
**
** Function         btif_avk_is_connected_on_other_idx
**
** Description      Checks if any other AV SCB is connected
**
** Returns          BOOLEAN
**
*******************************************************************************/

static BOOLEAN btif_avk_is_connected_on_other_idx(int current_index)
{
    //return true if other IDx is connected
    btif_sm_state_t state = BTIF_AVK_STATE_IDLE;
    int i;
    for (i = 0; i < btif_max_avk_clients; i++)
    {
        if (i != current_index)
        {
            state = btif_sm_get_state(btif_avk_cb[i].sm_handle);
            if ((state == BTIF_AVK_STATE_OPENED) ||
                (state == BTIF_AVK_STATE_STARTED))
                return TRUE;
        }
    }
    return FALSE;
}

/*******************************************************************************
**
** Function         btif_avk_is_playing_on_other_idx
**
** Description      Checks if any other AV SCB is connected
**
** Returns          BOOLEAN
**
*******************************************************************************/

static BOOLEAN btif_avk_is_playing_on_other_idx(int current_index)
{
    //return true if other IDx is playing
    btif_sm_state_t state = BTIF_AVK_STATE_IDLE;
    int i;
    for (i = 0; i < btif_max_avk_clients; i++)
    {
        if (i != current_index)
        {
            state = btif_sm_get_state(btif_avk_cb[i].sm_handle);
            if (state == BTIF_AVK_STATE_STARTED)
                return TRUE;
        }
    }
    return FALSE;
}

/*******************************************************************************
**
** Function         btif_avk_update_current_playing_device
**
** Description      Update the next connected device as playing
**
** Returns          void
**
*******************************************************************************/

static void btif_avk_update_current_playing_device(int index)
{
    int i;
    for (i = 0; i < btif_max_avk_clients; i++)
    {
        if (i != index)
            btif_avk_cb[i].current_playing = TRUE;
    }
}

/*******************************************************************************
**
** Function         btif_avk_is_peer_edr
**
** Description      Check if the connected a2dp device supports
**                  EDR or not. Only when connected this function
**                  will accurately provide a true capability of
**                  remote peer. If not connected it will always be false.
**
** Returns          TRUE if remote device is capable of EDR
**
*******************************************************************************/
BOOLEAN btif_avk_is_peer_edr(void)
{
    int index = 0;
    btif_sm_state_t state;
    BOOLEAN peer_edr = FALSE;

    ASSERTC(btif_avk_is_connected(), "No active a2dp connection", 0);

    /* If any of the remote in streaming state is BR
     * return FALSE to ensure proper configuration
     * is used. Ideally, since multicast is not supported
     * if any of the connected device is BR device,
     * we should not see both devices in START state.
     */
    for (index; index < btif_max_avk_clients; index ++)
    {
        state = btif_sm_get_state(btif_avk_cb[index].sm_handle);
        if ((btif_avk_cb[index].flags & BTIF_AVK_FLAG_PENDING_START)
            || (state == BTIF_AVK_STATE_STARTED))
        {
            if (btif_avk_cb[index].edr)
            {
                peer_edr = TRUE;
            }
            else
            {
                return FALSE;
            }
        }
    }
    return peer_edr;
}

/*******************************************************************************
**
** Function         btif_avk_any_br_peer
**
** Description      Check if the any of connected devices is BR device.
**
** Returns          TRUE if connected to any BR device, FALSE otherwise.
**
*******************************************************************************/
static BOOLEAN btif_avk_any_br_peer(void)
{
    int index = 0;
    btif_sm_state_t state;
    bdstr_t addr_string;

    for (index; index < btif_max_avk_clients; index ++)
    {
        state = btif_sm_get_state(btif_avk_cb[index].sm_handle);
        if (state >= BTIF_AVK_STATE_OPENED)
        {
            if (!btif_avk_cb[index].edr)
            {
                BTIF_TRACE_WARNING("%s : Connected to BR device : %s",
                    __FUNCTION__, bdaddr_to_string(&btif_avk_cb[index].peer_bda,
                    &addr_string, sizeof(addr_string)));
                return TRUE;
            }
        }
    }
    return FALSE;
}

/*******************************************************************************
**
** Function         btif_avk_peer_supports_3mbps
**
** Description      check if the connected a2dp device supports
**                  3mbps edr. Only when connected this function
**                  will accurately provide a true capability of
**                  remote peer. If not connected it will always be false.
**
** Returns          TRUE if remote device is EDR and supports 3mbps
**
*******************************************************************************/
BOOLEAN btif_avk_peer_supports_3mbps(void)
{
    int index =0;
    btif_sm_state_t state;
    ASSERTC(btif_avk_is_connected(), "No active a2dp connection", 0);

    for (index; index < btif_max_avk_clients; index ++)
    {
        state = btif_sm_get_state(btif_avk_cb[index].sm_handle);
        if ((btif_avk_cb[index].flags & BTIF_AVK_FLAG_PENDING_START)
            || (state == BTIF_AVK_STATE_STARTED))
        {
            if(btif_avk_cb[index].edr_3mbps)
                return TRUE;
        }
    }
    return FALSE;
}

/******************************************************************************
**
** Function        btif_avk_clear_remote_suspend_flag
**
** Description     Clears btif_avk_cd.flags if BTIF_AVK_FLAG_REMOTE_SUSPEND is set
**
** Returns         void
******************************************************************************/
void btif_avk_clear_remote_suspend_flag(void)
{
    int i;
    for (i = 0; i < btif_max_avk_clients; i++)
    {
        BTIF_TRACE_DEBUG(" flag :%x",btif_avk_cb[i].flags);
        btif_avk_cb[i].flags  &= ~BTIF_AVK_FLAG_REMOTE_SUSPEND;
    }
}

/*******************************************************************************
**
** Function         btif_avk_move_idle
**
** Description      Opening state is intermediate state. It cannot handle
**                  incoming/outgoing connect/disconnect requests.When ACL
**                  is disconnected and we are in opening state then move back
**                  to idle state which is proper to handle connections.
**
** Returns          Void
**
*******************************************************************************/
void btif_avk_move_idle(bt_bdaddr_t bd_addr)
{
    int index =0;
    if (btif_avk_cb[0].sm_handle == NULL) return;
    /* inform the application that ACL is disconnected and move to idle state */
    index = btif_avk_idx_by_bdaddr(bd_addr.address);
    if (index == btif_max_avk_clients)
    {
        BTIF_TRACE_DEBUG("btif_avk_move_idle: Already in IDLE");
        return;
    }
    btif_sm_state_t state = btif_sm_get_state(btif_avk_cb[index].sm_handle);
    BTIF_TRACE_DEBUG("ACL Disconnected state %d  is same device %d",state,
            memcmp (&bd_addr, &(btif_avk_cb[index].peer_bda), sizeof(bd_addr)));
    if (state == BTIF_AVK_STATE_OPENING &&
            (memcmp (&bd_addr, &(btif_avk_cb[index].peer_bda), sizeof(bd_addr)) == 0))
    {
        BTIF_TRACE_DEBUG("Moving State from Opening to Idle due to ACL disconnect");
        btif_report_connection_state(BTAV_CONNECTION_STATE_DISCONNECTED, &(btif_avk_cb[index].peer_bda));

        btif_sm_change_state(btif_avk_cb[index].sm_handle, BTIF_AVK_STATE_IDLE);
        btif_queue_advance();
    }
}
/******************************************************************************
**
** Function        btif_avk_get_num_playing_devices
**
** Description     Return number of A2dp playing devices
**
** Returns         int
******************************************************************************/
UINT16 btif_avk_get_num_playing_devices(void)
{
    UINT16 i;
    UINT16 playing_devices = 0;
    for (i = 0; i < btif_max_avk_clients; i++)
    {
        btif_avk_cb[i].state = btif_sm_get_state(btif_avk_cb[i].sm_handle);
        if (btif_avk_cb[i].state ==  BTIF_AVK_STATE_STARTED)
        {
            playing_devices++;
        }
    }
    BTIF_TRACE_DEBUG("AV devices playing: %d", playing_devices);

    return playing_devices;
}

/******************************************************************************
**
** Function        btif_avk_get_num_connected_devices
**
** Description     Return number of A2dp connected devices
**
** Returns         int
******************************************************************************/
UINT16 btif_avk_get_num_connected_devices(void)
{
    UINT16 i;
    UINT16 connected_devies = 0;
    for (i = 0; i < btif_max_avk_clients; i++)
    {
        btif_avk_cb[i].state = btif_sm_get_state(btif_avk_cb[i].sm_handle);
        if ((btif_avk_cb[i].state == BTIF_AVK_STATE_OPENED) ||
            (btif_avk_cb[i].state ==  BTIF_AVK_STATE_STARTED))
        {
            connected_devies++;
        }
    }
    BTIF_TRACE_DEBUG("AV Connection count: %d", connected_devies);

    return connected_devies;
}

/******************************************************************************
**
** Function        btif_avk_update_multicast_state
**
** Description     Enable Multicast only if below conditions are satisfied
**                 1. Connected to only 2 EDR HS.
**                 2. Connected to both HS as master.
**                 3. Connected to 2 EDR HS and one BLE device
**                 Multicast will fall back to soft handsoff in below conditions
**                 1. Number of ACL links is more than 2,like connected to HID
**                    initiating connection for HS1 and HS2.
**                 2. Connected to BR and EDR HS.
**                 3. Connected to more then 1 BLE device
**
** Returns         void
******************************************************************************/
static void btif_avk_update_multicast_state(int index)
{
    UINT16 num_connected_br_edr_devices = 0;
    UINT16 num_connected_le_devices = 0;
    UINT16 num_av_connected = 0;
    UINT16 i = 0;
    BOOLEAN is_slave = FALSE;
    BOOLEAN is_br_hs_connected = FALSE;
    BOOLEAN prev_multicast_state = enable_multicast;
    bdstr_t addr_string;

    if (!is_multicast_supported)
    {
        BTIF_TRACE_DEBUG("%s Multicast is Disabled", __FUNCTION__);
        return;
    }

    if (multicast_disabled == TRUE)
    {
        multicast_disabled = FALSE;
        enable_multicast = FALSE;
        BTA_AvkEnableMultiCast(FALSE, btif_avk_cb[index].bta_handle);
        return;
    }

    BTIF_TRACE_DEBUG("%s Multicast previous state : %s", __FUNCTION__,
        enable_multicast ? "Enabled" : "Disabled" );

    num_connected_br_edr_devices = btif_dm_get_br_edr_links();
    num_connected_le_devices = btif_dm_get_le_links();
    num_av_connected = btif_avk_get_num_connected_devices();
    is_br_hs_connected = btif_avk_any_br_peer();

    for (i = 0; i < btif_max_avk_clients; i++)
    {
        if (btif_avk_cb[i].is_slave == TRUE)
        {
            BTIF_TRACE_WARNING("Conected as slave to : %s",
                bdaddr_to_string(&btif_avk_cb[i].peer_bda, &addr_string,
                        sizeof(addr_string)));
            is_slave = TRUE;
            break;
        }
    }

    if ((num_av_connected <= 2) && (is_br_hs_connected != TRUE) &&
        (is_slave == FALSE) && ((num_connected_br_edr_devices <= 2) &&
        (num_connected_le_devices <= 1)))
    {
        enable_multicast = TRUE;
    }
    else
    {
        enable_multicast = FALSE;
    }

    BTIF_TRACE_DEBUG("%s Multicast current state : %s", __FUNCTION__,
        enable_multicast ? "Enabled" : "Disabled" );

    if (prev_multicast_state != enable_multicast)
    {
        BTA_AvkEnableMultiCast(enable_multicast, btif_avk_cb[index].bta_handle);
    }
}
/******************************************************************************
**
** Function        btif_avk_get_multicast_state
**
** Description     Returns TRUE if multicast is enabled else false
**
** Returns         BOOLEAN
******************************************************************************/
BOOLEAN btif_avk_get_multicast_state()
{
    return enable_multicast;
}
/******************************************************************************
**
** Function        btif_avk_get_ongoing_multicast
**
** Description     Returns TRUE if multicast is ongoing
**
** Returns         BOOLEAN
******************************************************************************/
BOOLEAN btif_avk_get_ongoing_multicast()
{
    int i = 0, j = 0;
    if (!is_multicast_supported)
    {
        BTIF_TRACE_DEBUG("Multicast is Disabled");
        return FALSE;
    }
    for (i = 0; i < btif_max_avk_clients; i++)
    {
        if (btif_avk_cb[i].is_device_playing)
        {
            j++;
        }
    }
    if (j == btif_max_avk_clients)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
