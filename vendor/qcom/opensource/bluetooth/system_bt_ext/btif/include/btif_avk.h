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
 *  Filename:      btif_av.h
 *
 *  Description:   Main API header file for all BTIF AV functions accessed
 *                 from internal stack.
 *
 *******************************************************************************/

#ifndef BTIF_AVK_H
#define BTIF_AVK_H

#include "btif_common.h"
#include "btif_sm.h"
#include "bta_avk_api.h"

#define BTIF_AVK_RC_HANDLE_NONE 0xFF

/*******************************************************************************
**  Type definitions for callback functions
********************************************************************************/

typedef enum {
    /* Reuse BTA_AVK_XXX_EVT - No need to redefine them here */
    BTIF_AVK_CONNECT_REQ_EVT = BTA_AVK_MAX_EVT,
    BTIF_AVK_DISCONNECT_REQ_EVT,
    BTIF_AVK_START_STREAM_REQ_EVT,
    BTIF_AVK_SINK_START_STREAM_REQ_EVT,
    BTIF_AVK_STOP_STREAM_REQ_EVT,
    BTIF_AVK_SUSPEND_STREAM_REQ_EVT,
    BTIF_AVK_SINK_SUSPEND_STREAM_REQ_EVT,
    BTIF_AVK_SINK_CONFIG_REQ_EVT,
    BTIF_AVK_CLEANUP_REQ_EVT,
    BTIF_AVK_SINK_FOCUS_REQ_EVT,
    BTIF_AVK_INIT_REQ_EVT,
} btif_avk_sm_event_t;

UINT64 average_delay;

/*******************************************************************************
**  BTIF AV API
********************************************************************************/

/*******************************************************************************
**
** Function         btif_avk_get_sm_handle
**
** Description      Fetches current av SM handle
**
** Returns          None
**
*******************************************************************************/

btif_sm_handle_t btif_avk_get_sm_handle(void);

/*******************************************************************************
**
** Function         btif_avk_get_addr
**
** Description      Fetches current AV BD address
**
** Returns          BD address
**
*******************************************************************************/

bt_bdaddr_t btif_avk_get_addr(BD_ADDR address);

/*******************************************************************************
**
** Function         btif_avk_stream_ready
**
** Description      Checks whether AV is ready for starting a stream
**
** Returns          None
**
*******************************************************************************/

BOOLEAN btif_avk_stream_ready(void);

/*******************************************************************************
**
** Function         btif_avk_stream_started_ready
**
** Description      Checks whether AV ready for media start in streaming state
**
** Returns          None
**
*******************************************************************************/

BOOLEAN btif_avk_stream_started_ready(void);

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
void btif_avk_dispatch_sm_event(btif_avk_sm_event_t event, void *p_data, int len);

/*******************************************************************************
**
** Function         btif_avk_init
**
** Description      Initializes btif AV if not already done
**
** Returns          bt_status_t
**
*******************************************************************************/

bt_status_t btif_avk_init(int service_id);

/*******************************************************************************
**
** Function         btif_avk_is_connected
**
** Description      Checks if av has a connected sink
**
** Returns          BOOLEAN
**
*******************************************************************************/

BOOLEAN btif_avk_is_connected(void);


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

BOOLEAN btif_avk_is_peer_edr(void);

#ifdef USE_AUDIO_TRACK
/*******************************************************************************
**
** Function         btif_avk_queue_focus_rquest
**
** Description      This is used to move context to btif and
**                  queue audio_focus_request
**
** Returns          none
**
*******************************************************************************/
void btif_avk_queue_focus_rquest(void);
#endif

/*******************************************************************************
 **
 ** Function         btif_media_enque_sink_data
 **
 ** Description      queues PCM data
 **
 ** Returns          void
 **
 *******************************************************************************/
UINT32 btif_media_enque_sink_data(UINT16 codec_type, UINT8 *data, UINT16 size, BD_ADDR bd_addr, UINT64 enque_time);

/*******************************************************************************
 **
 ** Function         btif_media_avk_fetch_pcm_data
 **
 ** Description      fetch PCM data
 **
 ** Returns          size of data read
 **
 *******************************************************************************/
UINT32 btif_media_avk_fetch_pcm_data(UINT16 codec_type, UINT8 *data, UINT32 size);

/******************************************************************************
**
** Function         btif_avk_clear_remote_suspend_flag
**
** Description      Clears remote suspended flag
**
** Returns          Void
********************************************************************************/

void btif_avk_clear_remote_suspend_flag(void);

/*******************************************************************************
**
** Function         btif_avk_peer_supports_3mbps
**
** Description      Check if the connected a2dp device supports
**                  3mbps edr. Only when connected this function
**                  will accurately provide a true capability of
**                  remote peer. If not connected it will always be false.
**
** Returns          TRUE if remote device is EDR and supports 3mbps
**
*******************************************************************************/
BOOLEAN btif_avk_peer_supports_3mbps(void);


#endif /* BTIF_AVK_H */
