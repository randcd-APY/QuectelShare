/******************************************************************************
 *  Copyright (c) 2016, The Linux Foundation. All rights reserved.
 *
 *  Not a contribution.
 ******************************************************************************/

/******************************************************************************
 *
 *  Copyright (C) 2004-2012 Broadcom Corporation
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
 *
 *  This is the private interface file for the BTA advanced audio/video.
 *
 ******************************************************************************/
#ifndef BTA_AVK_INT_H
#define BTA_AVK_INT_H

#include "bta_sys.h"
#include "bta_api.h"
#include "bta_avk_api.h"
#include "avdt_api.h"
#include "bta_avk_co.h"
#include "osi/include/list.h"


#define BTA_AVK_DEBUG TRUE
/*****************************************************************************
**  Constants
*****************************************************************************/

enum
{
    /* these events are handled by the AV main state machine */
    BTA_AVK_API_DISABLE_EVT = BTA_SYS_EVT_START(BTA_ID_AVK),
    BTA_AVK_API_REMOTE_CMD_EVT,
    BTA_AVK_API_VENDOR_CMD_EVT,
    BTA_AVK_API_VENDOR_RSP_EVT,
    BTA_AVK_API_META_RSP_EVT,
    BTA_AVK_API_RC_CLOSE_EVT,
    BTA_AVK_AVRC_OPEN_EVT,
    BTA_AVK_AVRC_BROWSE_OPEN_EVT,
    BTA_AVK_AVRC_BROWSE_CLOSE_EVT,
    BTA_AVK_AVRC_MSG_EVT,
    BTA_AVK_AVRC_NONE_EVT,

    /* these events are handled by the AV stream state machine */
    BTA_AVK_API_OPEN_EVT,
    BTA_AVK_API_CLOSE_EVT,
    BTA_AVK_AP_START_EVT,        /* the following 2 events must be in the same order as the *API_*EVT */
    BTA_AVK_AP_STOP_EVT,
    BTA_AVK_API_RECONFIG_EVT,
    BTA_AVK_API_PROTECT_REQ_EVT,
    BTA_AVK_API_PROTECT_RSP_EVT,
    BTA_AVK_API_RC_OPEN_EVT,
    BTA_AVK_SRC_DATA_READY_EVT,
    BTA_AVK_CI_SETCONFIG_OK_EVT,
    BTA_AVK_CI_SETCONFIG_FAIL_EVT,
    BTA_AVK_SDP_DISC_OK_EVT,
    BTA_AVK_SDP_DISC_FAIL_EVT,
    BTA_AVK_STR_DISC_OK_EVT,
    BTA_AVK_STR_DISC_FAIL_EVT,
    BTA_AVK_STR_GETCAP_OK_EVT,
    BTA_AVK_STR_GETCAP_FAIL_EVT,
    BTA_AVK_STR_OPEN_OK_EVT,
    BTA_AVK_STR_OPEN_FAIL_EVT,
    BTA_AVK_STR_START_OK_EVT,
    BTA_AVK_STR_START_FAIL_EVT,
    BTA_AVK_STR_CLOSE_EVT,
    BTA_AVK_STR_CONFIG_IND_EVT,
    BTA_AVK_STR_SECURITY_IND_EVT,
    BTA_AVK_STR_SECURITY_CFM_EVT,
    BTA_AVK_STR_WRITE_CFM_EVT,
    BTA_AVK_STR_SUSPEND_CFM_EVT,
    BTA_AVK_STR_RECONFIG_CFM_EVT,
    BTA_AVK_AVRC_TIMER_EVT,
    BTA_AVK_AVDT_CONNECT_EVT,
    BTA_AVK_AVDT_DISCONNECT_EVT,
    BTA_AVK_ROLE_CHANGE_EVT,
    BTA_AVK_AVDT_DELAY_RPT_EVT,
    BTA_AVK_ACP_CONNECT_EVT,

    /* these events are handled outside of the state machine */
    BTA_AVK_API_ENABLE_EVT,
    BTA_AVK_API_REGISTER_EVT,
    BTA_AVK_API_DEREGISTER_EVT,
    BTA_AVK_API_DISCONNECT_EVT,
    BTA_AVK_CI_SRC_DATA_READY_EVT,
    BTA_AVK_SIG_CHG_EVT,
    BTA_AVK_SIG_TIMER_EVT,
    BTA_AVK_SDP_AVRC_DISC_EVT,
    BTA_AVK_AVRC_CLOSE_EVT,
    BTA_AVK_CONN_CHG_EVT,
    BTA_AVK_DEREG_COMP_EVT,
#if (BTA_AV_SINK_INCLUDED == TRUE)
    BTA_AVK_API_SINK_ENABLE_EVT,
#endif
#if (AVDT_REPORTING == TRUE)
    BTA_AVK_AVDT_RPT_CONN_EVT,
#endif
    BTA_AVK_API_START_EVT,       /* the following 2 events must be in the same order as the *AP_*EVT */
    BTA_AVK_API_STOP_EVT,
    BTA_AVK_UPDATE_SUPP_CODECS,
    BTA_AVK_ENABLE_MULTICAST_EVT, /* Event for enable and disable multicast */

};

/* events for AV control block state machine */
#define BTA_AVK_FIRST_SM_EVT     BTA_AVK_API_DISABLE_EVT
#define BTA_AVK_LAST_SM_EVT      BTA_AVK_AVRC_NONE_EVT

/* events for AV stream control block state machine */
#define BTA_AVK_FIRST_SSM_EVT    BTA_AVK_API_OPEN_EVT

/* events that do not go through state machine */
#define BTA_AVK_FIRST_NSM_EVT    BTA_AVK_API_ENABLE_EVT
#define BTA_AVK_LAST_NSM_EVT     BTA_AVK_ENABLE_MULTICAST_EVT

/* API events passed to both SSMs (by bta_avk_api_to_ssm) */
#define BTA_AVK_FIRST_A2S_API_EVT    BTA_AVK_API_START_EVT
#define BTA_AVK_FIRST_A2S_SSM_EVT    BTA_AVK_AP_START_EVT

#define BTA_AVK_LAST_EVT             BTA_AVK_ENABLE_MULTICAST_EVT

/* maximum number of SEPS in stream discovery results */
#define BTA_AVK_NUM_SEPS         32

/* initialization value for AVRC handle */
#define BTA_AVK_RC_HANDLE_NONE   0xFF

/* size of database for service discovery */
#define BTA_AVK_DISC_BUF_SIZE        1000

/* offset of media type in codec info byte array */
#define BTA_AVK_MEDIA_TYPE_IDX       1

/* maximum length of AVDTP security data */
#define BTA_AVK_SECURITY_MAX_LEN     400

/* check number of buffers queued at L2CAP when this amount of buffers are queued to L2CAP */
#define BTA_AVK_QUEUE_DATA_CHK_NUM   L2CAP_HIGH_PRI_MIN_XMIT_QUOTA

/* the number of ACL links with AVDT */
#define BTA_AVK_NUM_LINKS            (AVDT_NUM_LINKS/2)

#define BTA_AVK_CO_ID_TO_BE_STREAM(p, u32) {*(p)++ = (UINT8)((u32) >> 16); *(p)++ = (UINT8)((u32) >> 8); *(p)++ = (UINT8)(u32); }
#define BTA_AVK_BE_STREAM_TO_CO_ID(u32, p) {u32 = (((UINT32)(*((p) + 2))) + (((UINT32)(*((p) + 1))) << 8) + (((UINT32)(*(p))) << 16)); (p) += 3;}

/* these bits are defined for bta_avk_cb.multi_av */
#define BTA_AVK_MULTI_AV_SUPPORTED   0x01
#define BTA_AVK_MULTI_AV_IN_USE      0x02

/*****************************************************************************
**  Data types
*****************************************************************************/

/* function types for call-out functions */
typedef BOOLEAN (*tBTA_AVK_CO_INIT) (UINT8 *p_codec_type, UINT8 *p_codec_info,
                                   UINT8 *p_num_protect, UINT8 *p_protect_info, UINT8 index);
typedef void (*tBTA_AVK_CO_DISC_RES) (tBTA_AVK_HNDL hndl, UINT8 num_seps,
                                     UINT8 num_snk, UINT8 num_src, BD_ADDR addr, UINT16 uuid_local);
typedef UINT8 (*tBTA_AVK_CO_GETCFG) (tBTA_AVK_HNDL hndl, tBTA_AVK_CODEC codec_type,
                                     UINT8 *p_codec_info, UINT8 *p_sep_info_idx, UINT8 seid,
                                     UINT8 *p_num_protect, UINT8 *p_protect_info);
typedef void (*tBTA_AVK_CO_SETCFG) (tBTA_AVK_HNDL hndl, tBTA_AVK_CODEC codec_type,
                                     UINT8 *p_codec_info, UINT8 seid, BD_ADDR addr,
                                     UINT8 num_protect, UINT8 *p_protect_info,
                                     UINT8 t_local_sep, UINT8 avdt_handle);
typedef void (*tBTA_AVK_CO_OPEN) (tBTA_AVK_HNDL hndl,
                                 tBTA_AVK_CODEC codec_type, UINT8 *p_codec_info,
                                   UINT16 mtu);
typedef void (*tBTA_AVK_CO_CLOSE) (tBTA_AVK_HNDL hndl, tBTA_AVK_CODEC codec_type, UINT16 mtu);
typedef void (*tBTA_AVK_CO_START) (tBTA_AVK_HNDL hndl, tBTA_AVK_CODEC codec_type,UINT8 *p_codec_info, BOOLEAN *p_no_rtp_hdr);
typedef void (*tBTA_AVK_CO_STOP) (tBTA_AVK_HNDL hndl, tBTA_AVK_CODEC codec_type);
typedef void * (*tBTA_AVK_CO_DATAPATH) (tBTA_AVK_CODEC codec_type,
                                       UINT32 *p_len, UINT32 *p_timestamp);
typedef void (*tBTA_AVK_CO_DELAY) (tBTA_AVK_HNDL hndl, UINT16 delay);

/* the call-out functions for one stream */
typedef struct
{
    tBTA_AVK_CO_INIT     init;
    tBTA_AVK_CO_DISC_RES disc_res;
    tBTA_AVK_CO_GETCFG   getcfg;
    tBTA_AVK_CO_SETCFG   setcfg;
    tBTA_AVK_CO_OPEN     open;
    tBTA_AVK_CO_CLOSE    close;
    tBTA_AVK_CO_START    start;
    tBTA_AVK_CO_STOP     stop;
    tBTA_AVK_CO_DATAPATH data;
    tBTA_AVK_CO_DELAY    delay;
} tBTA_AVK_CO_FUNCTS;

/* data type for BTA_AVK_API_ENABLE_EVT */
typedef struct
{
    BT_HDR              hdr;
    tBTA_AVK_CBACK       *p_cback;
    tBTA_AVK_FEAT        features;
    tBTA_SEC            sec_mask;
} tBTA_AVK_API_ENABLE;

/* data type for BTA_AVK_API_REG_EVT */
typedef struct
{
    BT_HDR              hdr;
    char                p_service_name[BTA_SERVICE_NAME_LEN+1];
    UINT8               app_id;
    tBTA_AVK_DATA_CBACK       *p_app_data_cback;
    UINT16              service_uuid;
} tBTA_AVK_API_REG;


enum
{
    BTA_AVK_RS_NONE,     /* straight API call */
    BTA_AVK_RS_OK,       /* the role switch result - successful */
    BTA_AVK_RS_FAIL,     /* the role switch result - failed */
    BTA_AVK_RS_DONE      /* the role switch is done - continue */
};
typedef UINT8 tBTA_AVK_RS_RES;
/* data type for BTA_AVK_API_OPEN_EVT */
typedef struct
{
    BT_HDR              hdr;
    BD_ADDR             bd_addr;
    BOOLEAN             use_rc;
    tBTA_SEC            sec_mask;
    tBTA_AVK_RS_RES      switch_res;
    UINT16              uuid;  /* uuid of initiator */
} tBTA_AVK_API_OPEN;

/* data type for BTA_AVK_API_STOP_EVT */
typedef struct
{
    BT_HDR              hdr;
    BOOLEAN             suspend;
    BOOLEAN             flush;
} tBTA_AVK_API_STOP;

/* data type for BTA_AVK_ENABLE_MULTICAST_EVT */
typedef struct
{
    BT_HDR              hdr;
    BOOLEAN             is_multicast_enabled;
} tBTA_AVK_ENABLE_MULTICAST;

/* data type for BTA_AVK_API_DISCONNECT_EVT */
typedef struct
{
    BT_HDR              hdr;
    BD_ADDR             bd_addr;
} tBTA_AVK_API_DISCNT;

/* data type for BTA_AVK_API_PROTECT_REQ_EVT */
typedef struct
{
    BT_HDR              hdr;
    UINT8               *p_data;
    UINT16              len;
} tBTA_AVK_API_PROTECT_REQ;

/* data type for BTA_AVK_API_PROTECT_RSP_EVT */
typedef struct
{
    BT_HDR              hdr;
    UINT8               *p_data;
    UINT16              len;
    UINT8               error_code;
} tBTA_AVK_API_PROTECT_RSP;

/* data type for BTA_AVK_API_REMOTE_CMD_EVT */
typedef struct
{
    BT_HDR              hdr;
    tAVRC_MSG_PASS      msg;
    UINT8               label;
} tBTA_AVK_API_REMOTE_CMD;

/* data type for BTA_AVK_API_VENDOR_CMD_EVT and RSP */
typedef struct
{
    BT_HDR              hdr;
    tAVRC_MSG_VENDOR    msg;
    UINT8               label;
} tBTA_AVK_API_VENDOR;

/* data type for BTA_AVK_API_RC_OPEN_EVT */
typedef struct
{
    BT_HDR              hdr;
} tBTA_AVK_API_OPEN_RC;

/* data type for BTA_AVK_API_RC_CLOSE_EVT */
typedef struct
{
    BT_HDR              hdr;
} tBTA_AVK_API_CLOSE_RC;

/* data type for BTA_AVK_API_META_RSP_EVT */
typedef struct
{
    BT_HDR              hdr;
    BOOLEAN             is_rsp;
    UINT8               label;
    tBTA_AVK_CODE        rsp_code;
    BT_HDR              *p_pkt;
} tBTA_AVK_API_META_RSP;


/* data type for BTA_AVK_API_RECONFIG_EVT */
typedef struct
{
    BT_HDR              hdr;
    UINT8               codec_info[AVDT_CODEC_SIZE];    /* codec configuration */
    UINT8               *p_protect_info;
    UINT8               num_protect;
    BOOLEAN             suspend;
    UINT8               sep_info_idx;
} tBTA_AVK_API_RCFG;

/* data type for BTA_AVK_CI_SETCONFIG_OK_EVT and BTA_AVK_CI_SETCONFIG_FAIL_EVT */
typedef struct
{
    BT_HDR              hdr;
    tBTA_AVK_HNDL        hndl;
    UINT8               err_code;
    UINT8               category;
    UINT8               num_seid;
    UINT8               *p_seid;
    BOOLEAN             recfg_needed;
    UINT8               avdt_handle;  /* local sep type for which this stream will be set up */
} tBTA_AVK_CI_SETCONFIG;

/* data type for all stream events from AVDTP */
typedef struct {
    BT_HDR              hdr;
    tAVDT_CFG           cfg;        /* configuration/capabilities parameters */
    tAVDT_CTRL          msg;        /* AVDTP callback message parameters */
    BD_ADDR             bd_addr;    /* bd address */
    UINT8               handle;
    UINT8               avdt_event;
    BOOLEAN             initiator; /* TRUE, if local device initiates the SUSPEND */
} tBTA_AVK_STR_MSG;

/* data type for BTA_AVK_AVRC_MSG_EVT */
typedef struct
{
    BT_HDR              hdr;
    tAVRC_MSG           msg;
    UINT8               handle;
    UINT8               label;
    UINT8               opcode;
} tBTA_AVK_RC_MSG;

/* data type for BTA_AVK_AVRC_OPEN_EVT, BTA_AVK_AVRC_CLOSE_EVT */
typedef struct
{
    BT_HDR              hdr;
    BD_ADDR             peer_addr;
    UINT8               handle;
} tBTA_AVK_RC_CONN_CHG;

/* data type for BTA_AVK_CONN_CHG_EVT */
typedef struct
{
    BT_HDR              hdr;
    BD_ADDR             peer_addr;
    BOOLEAN             is_up;
} tBTA_AVK_CONN_CHG;

/* data type for BTA_AVK_ROLE_CHANGE_EVT */
typedef struct
{
    BT_HDR              hdr;
    UINT8               new_role;
    UINT8               hci_status;
} tBTA_AVK_ROLE_RES;

/* data type for BTA_AVK_SDP_DISC_OK_EVT */
typedef struct
{
    BT_HDR              hdr;
    UINT16              avdt_version;   /* AVDTP protocol version */
} tBTA_AVK_SDP_RES;

/* type for SEP control block */
typedef struct
{
    UINT8               av_handle;         /* AVDTP handle */
    tBTA_AVK_CODEC       codec_type;        /* codec type */
    UINT8               tsep;              /* SEP type of local SEP */
    tBTA_AVK_DATA_CBACK  *p_app_data_cback; /* Application callback for media packets */
    UINT8               vendorId;          /* vendorId type */
    UINT8               codecId;           /* codecId type */
} tBTA_AVK_SEP;

/* data type for tBTA_AVK_UPDATE_SUPP_CODECS */
typedef struct
{
    BT_HDR              hdr;
    UINT8  codec_type[BTIF_SV_AVK_AA_SEP_INDEX]; /* Codec Type*/
    UINT8  vnd_id[BTIF_SV_AVK_AA_SEP_INDEX]; /* Vendor Id */
    UINT8  codec_id[BTIF_SV_AVK_AA_SEP_INDEX]; /* Codec Id */
    UINT8  codec_info[BTIF_SV_AVK_AA_SEP_INDEX][AVDT_CODEC_SIZE];
    UINT8   num_codec_configs;          /* Number of codec configs */
} tBTA_AVK_UPDATE_SUPP_CODECS;

/* initiator/acceptor role for adaption */
#define BTA_AVK_ROLE_AD_INT          0x00       /* initiator */
#define BTA_AVK_ROLE_AD_ACP          0x01       /* acceptor */

/* initiator/acceptor signaling roles */
#define BTA_AVK_ROLE_START_ACP       0x00
#define BTA_AVK_ROLE_START_INT       0x10    /* do not change this value */

#define BTA_AVK_ROLE_SUSPEND         0x20    /* suspending on start */
#define BTA_AVK_ROLE_SUSPEND_OPT     0x40    /* Suspend on Start option is set */

/* union of all event datatypes */
typedef union
{
    BT_HDR                    hdr;
    tBTA_AVK_API_ENABLE        api_enable;
    tBTA_AVK_API_REG           api_reg;
    tBTA_AVK_API_OPEN          api_open;
    tBTA_AVK_API_STOP          api_stop;
    tBTA_AVK_API_DISCNT        api_discnt;
    tBTA_AVK_API_PROTECT_REQ   api_protect_req;
    tBTA_AVK_API_PROTECT_RSP   api_protect_rsp;
    tBTA_AVK_API_REMOTE_CMD    api_remote_cmd;
    tBTA_AVK_API_VENDOR        api_vendor;
    tBTA_AVK_API_RCFG          api_reconfig;
    tBTA_AVK_CI_SETCONFIG      ci_setconfig;
    tBTA_AVK_STR_MSG           str_msg;
    tBTA_AVK_RC_MSG            rc_msg;
    tBTA_AVK_RC_CONN_CHG       rc_conn_chg;
    tBTA_AVK_CONN_CHG          conn_chg;
    tBTA_AVK_ROLE_RES          role_res;
    tBTA_AVK_SDP_RES           sdp_res;
    tBTA_AVK_API_META_RSP      api_meta_rsp;
    tBTA_AVK_UPDATE_SUPP_CODECS update_supp_codecs;
    tBTA_AVK_ENABLE_MULTICAST  multicast_state;
} tBTA_AVK_DATA;

typedef void (tBTA_AVK_VDP_DATA_ACT)(void *p_scb);

typedef struct
{
    tBTA_AVK_VDP_DATA_ACT    *p_act;
    UINT8                   *p_frame;
    UINT16                  buf_size;
    UINT32                  len;
    UINT32                  offset;
    UINT32                  timestamp;
} tBTA_AVK_VF_INFO;

typedef union
{
    tBTA_AVK_VF_INFO     vdp;            /* used for video channels only */
    tBTA_AVK_API_OPEN    open;           /* used only before open and role switch
                                           is needed on another AV channel */
} tBTA_AVK_Q_INFO;

#define BTA_AVK_Q_TAG_OPEN               0x01 /* after API_OPEN, before STR_OPENED */
#define BTA_AVK_Q_TAG_START              0x02 /* before start sending media packets */
#define BTA_AVK_Q_TAG_STREAM             0x03 /* during streaming */

#define BTA_AVK_WAIT_ACP_CAPS_ON         0x01 /* retriving the peer capabilities */
#define BTA_AVK_WAIT_ACP_CAPS_STARTED    0x02 /* started while retriving peer capabilities */
#define BTA_AVK_WAIT_ROLE_SW_RES_OPEN    0x04 /* waiting for role switch result after API_OPEN, before STR_OPENED */
#define BTA_AVK_WAIT_ROLE_SW_RES_START   0x08 /* waiting for role switch result before streaming */
#define BTA_AVK_WAIT_ROLE_SW_STARTED     0x10 /* started while waiting for role switch result */
#define BTA_AVK_WAIT_ROLE_SW_RETRY       0x20 /* set when retry on timeout */
#define BTA_AVK_WAIT_CHECK_RC            0x40 /* set when the timer is used by role switch */
#define BTA_AVK_WAIT_ROLE_SW_FAILED      0x80 /* role switch failed */

#define BTA_AVK_WAIT_ROLE_SW_BITS        (BTA_AVK_WAIT_ROLE_SW_RES_OPEN|BTA_AVK_WAIT_ROLE_SW_RES_START|BTA_AVK_WAIT_ROLE_SW_STARTED|BTA_AVK_WAIT_ROLE_SW_RETRY)

/* Bitmap for collision, coll_mask */
#define BTA_AVK_COLL_INC_TMR             0x01 /* Timer is running for incoming L2C connection */
#define BTA_AVK_COLL_API_CALLED          0x02 /* API open was called while incoming timer is running */
#define BTA_AVK_COLL_SETCONFIG_IND    0x04 /* SetConfig indication has been called by remote */

/* type for AV stream control block */
typedef struct
{
    const tBTA_AVK_ACT   *p_act_tbl;     /* the action table for stream state machine */
    const tBTA_AVK_CO_FUNCTS *p_cos;     /* the associated callout functions */
    BOOLEAN             sdp_discovery_started;     /* variable to determine whether SDP is started */
    tBTA_AVK_SEP         seps[BTA_AVK_MAX_SEPS];
    tAVDT_CFG           *p_cap;         /* buffer used for get capabilities */
    list_t              *a2d_list;      /* used for audio channels only */
    tBTA_AVK_Q_INFO      q_info;
    tAVDT_SEP_INFO      sep_info[BTA_AVK_NUM_SEPS];      /* stream discovery results */
    tAVDT_CFG           cfg;            /* local SEP configuration */
    alarm_t             *avrc_ct_timer; /* delay timer for AVRC CT */
    BD_ADDR             peer_addr;      /* peer BD address */
    UINT16              l2c_cid;        /* L2CAP channel ID */
    UINT16              stream_mtu;     /* MTU of stream */
    UINT16              avdt_version;   /* the avdt version of peer device */
    tBTA_SEC            sec_mask;       /* security mask */
    tBTA_AVK_CODEC       codec_type;     /* codec type */
    UINT8               media_type;     /* Media type */
    BOOLEAN             cong;           /* TRUE if AVDTP congested */
    tBTA_AVK_STATUS      open_status;    /* open failure status */
    tBTA_AVK_CHNL        chnl;           /* the channel: audio/video */
    tBTA_AVK_HNDL        hndl;           /* the handle: ((hdi + 1)|chnl) */
    UINT16              cur_psc_mask;   /* Protocol service capabilities mask for current connection */
    UINT8               avdt_handle;    /* AVDTP handle */
    UINT8               hdi;            /* the index to SCB[] */
    UINT8               num_seps;       /* number of seps returned by stream discovery */
    UINT8               num_disc_snks;  /* number of discovered snks */
    UINT8               num_disc_srcs;  /* number of discovered srcs */
    UINT8               sep_info_idx;   /* current index into sep_info */
    UINT8               sep_idx;        /* current index into local seps[] */
    UINT8               rcfg_idx;       /* reconfig requested index into sep_info */
    UINT8               state;          /* state machine state */
    UINT8               avdt_label;     /* AVDTP label */
    UINT8               app_id;         /* application id */
    UINT8               num_recfg;      /* number of reconfigure sent */
    UINT8               role;
    UINT8               l2c_bufs;       /* the number of buffers queued to L2CAP */
    UINT8               rc_handle;      /* connected AVRCP handle */
    BOOLEAN             use_rc;         /* TRUE if AVRCP is allowed */
    BOOLEAN             started;        /* TRUE if stream started */
    UINT8               co_started;     /* non-zero, if stream started from call-out perspective */
    BOOLEAN             recfg_sup;      /* TRUE if the first attempt to reconfigure the stream was successfull, else False if command fails */
    BOOLEAN             suspend_sup;    /* TRUE if Suspend stream is supported, else FALSE if suspend command fails */
    BOOLEAN             deregistring;   /* TRUE if deregistering */
    BOOLEAN             sco_suspend;    /* TRUE if SUSPEND is issued automatically for SCO */
    UINT8               coll_mask;      /* Mask to check incoming and outgoing collision */
    tBTA_AVK_API_OPEN    open_api;       /* Saved OPEN api message */
    UINT8               wait;           /* set 0x1, when getting Caps as ACP, set 0x2, when started */
    UINT8               q_tag;          /* identify the associated q_info union member */
    BOOLEAN             no_rtp_hdr;     /* TRUE if add no RTP header*/
    UINT16              uuid_int;       /*intended UUID of Initiator to connect to */
    BOOLEAN             skip_sdp;       /* Decides if sdp to be done prior to profile connection */
} tBTA_AVK_SCB;

#define BTA_AVK_RC_ROLE_MASK     0x10
#define BTA_AVK_RC_ROLE_INT      0x00
#define BTA_AVK_RC_ROLE_ACP      0x10

#define BTA_AVK_RC_CONN_MASK     0x20
#define BTA_AVK_RC_CONN_BR_MASK  0x40


/* type for AV RCP control block */
/* index to this control block is the rc handle */
typedef struct
{
    UINT8   status;
    UINT8   handle;
    UINT8   shdl;   /* stream handle (hdi + 1) */
    UINT8   lidx;   /* (index+1) to LCB */
    tBTA_AVK_FEAT        peer_features;  /* peer features mask */
    alarm_t   *br_conn_timer; /* timer to monitor browsing connection */
} tBTA_AVK_RCB;
#define BTA_AVK_NUM_RCB      (BTA_AVK_NUM_STRS  + 2 + 1)

enum
{
    BTA_AVK_LCB_FREE,
    BTA_AVK_LCB_FIND
};

/* type for AV ACL Link control block */
typedef struct
{
    BD_ADDR             addr;           /* peer BD address */
    UINT8               conn_msk;       /* handle mask of connected stream handle */
    UINT8               lidx;           /* index + 1 */
} tBTA_AVK_LCB;

/* type for stream state machine action functions */
typedef void (*tBTA_AVK_SACT)(tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);


/* type for AV control block */
typedef struct
{
    tBTA_AVK_SCB         *p_scb[BTA_AVK_NUM_STRS];    /* stream control block */
    tSDP_DISCOVERY_DB   *p_disc_db;     /* pointer to discovery database */
    tBTA_AVK_CBACK       *p_cback;       /* application callback function */
    tBTA_AVK_RCB         rcb[BTA_AVK_NUM_RCB];  /* RCB control block */
    tBTA_AVK_LCB         lcb[BTA_AVK_NUM_LINKS+1];  /* link control block */
    alarm_t             *link_signalling_timer;
    alarm_t             *accept_signalling_timer; /* timer to monitor signalling when accepting */
    UINT32              sdp_a2d_handle; /* SDP record handle for audio src */
#if (BTA_AV_SINK_INCLUDED == TRUE)
    UINT32              sdp_a2d_snk_handle; /* SDP record handle for audio snk */
#endif
    UINT32              sdp_vdp_handle; /* SDP record handle for video src */
    tBTA_AVK_FEAT        features;       /* features mask */
    tBTA_SEC            sec_mask;       /* security mask */
    tBTA_AVK_HNDL        handle;         /* the handle for SDP activity */
    BOOLEAN             disabling;      /* TRUE if api disabled called */
    UINT8               disc;           /* (hdi+1) or (rc_handle|BTA_AVK_CHNL_MSK) if p_disc_db is in use */
    UINT8               state;          /* state machine state */
    UINT8               conn_rc;        /* handle mask of connected RCP channels */
    UINT8               conn_audio;     /* handle mask of connected audio channels */
    UINT8               conn_video;     /* handle mask of connected video channels */
    UINT8               conn_lcb;       /* index mask of used LCBs */
    UINT8               audio_open_cnt; /* number of connected audio channels */
    UINT8               reg_audio;      /* handle mask of registered audio channels */
    UINT8               reg_video;      /* handle mask of registered video channels */
    UINT8               rc_acp_handle;
    UINT8               rc_acp_idx;     /* (index + 1) to RCB */
    UINT8               rs_idx;         /* (index + 1) to SCB for the one waiting for RS on open */
    BOOLEAN             sco_occupied;   /* TRUE if SCO is being used or call is in progress */
    UINT8               audio_streams;  /* handle mask of streaming audio channels */
    UINT8               video_streams;  /* handle mask of streaming video channels */
    UINT8               codec_type;     /* p_scb->codec_type */
} tBTA_AVK_CB;



/*****************************************************************************
**  Global data
*****************************************************************************/

/* control block declaration */
#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_AVK_CB bta_avk_cb;
#else
extern tBTA_AVK_CB *bta_avk_cb_ptr;
#define bta_avk_cb (*bta_avk_cb_ptr)
#endif

/* config struct */
extern tBTA_AVK_CFG *p_bta_avk_cfg;
extern const tBTA_AVK_CFG bta_avk_sink_cfg;

/* rc id config struct */
extern UINT16 *p_bta_avk_rc_id;
extern UINT16 *p_bta_avk_rc_id_ac;

extern const tBTA_AVK_SACT bta_avk_a2d_action[];
extern const tBTA_AVK_CO_FUNCTS bta_avk_a2d_cos;
extern const tBTA_AVK_SACT bta_avk_vdp_action[];
extern tAVDT_CTRL_CBACK * const bta_avk_dt_cback[];
extern void bta_avk_stream_data_cback(UINT8 handle, BT_HDR *p_pkt, UINT32 time_stamp, UINT8 m_pt);

/*****************************************************************************
**  Function prototypes
*****************************************************************************/
/* utility functions */
extern tBTA_AVK_SCB *bta_avk_hndl_to_scb(UINT16 handle);
extern BOOLEAN bta_avk_chk_start(tBTA_AVK_SCB *p_scb);
extern void bta_avk_restore_switch (void);
extern UINT16 bta_avk_chk_mtu(tBTA_AVK_SCB *p_scb, UINT16 mtu);
extern void bta_avk_conn_cback(UINT8 handle, BD_ADDR bd_addr, UINT8 event, tAVDT_CTRL *p_data);
extern UINT8 bta_avk_rc_create(tBTA_AVK_CB *p_cb, UINT8 role, UINT8 shdl, UINT8 lidx);
extern void bta_avk_stream_chg(tBTA_AVK_SCB *p_scb, BOOLEAN started);
extern BOOLEAN bta_avk_is_scb_opening (tBTA_AVK_SCB *p_scb);
extern BOOLEAN bta_avk_is_scb_incoming (tBTA_AVK_SCB *p_scb);
extern void bta_avk_set_scb_sst_init (tBTA_AVK_SCB *p_scb);
extern BOOLEAN bta_avk_is_scb_init (tBTA_AVK_SCB *p_scb);
extern void bta_avk_set_scb_sst_incoming (tBTA_AVK_SCB *p_scb);
extern tBTA_AVK_LCB * bta_avk_find_lcb(BD_ADDR addr, UINT8 op);
extern BOOLEAN bta_avk_is_multicast_enabled();
extern BOOLEAN bta_avk_is_scb_available();

/* main functions */
extern void bta_avk_api_deregister(tBTA_AVK_DATA *p_data);
extern void bta_avk_dup_audio_buf(tBTA_AVK_SCB *p_scb, BT_HDR *p_buf);
extern void bta_avk_sm_execute(tBTA_AVK_CB *p_cb, UINT16 event, tBTA_AVK_DATA *p_data);
extern void bta_avk_ssm_execute(tBTA_AVK_SCB *p_scb, UINT16 event, tBTA_AVK_DATA *p_data);
extern BOOLEAN bta_avk_hdl_event(BT_HDR *p_msg);
#if (defined(BTA_AVK_DEBUG) && BTA_AVK_DEBUG == TRUE)
extern char *bta_avk_evt_code(UINT16 evt_code);
#endif
extern BOOLEAN bta_avk_switch_if_needed(tBTA_AVK_SCB *p_scb);
extern BOOLEAN bta_avk_link_role_ok(tBTA_AVK_SCB *p_scb, UINT8 bits);
extern BOOLEAN bta_avk_is_rcfg_sst(tBTA_AVK_SCB *p_scb);

/* nsm action functions */
extern void bta_avk_api_disconnect(tBTA_AVK_DATA *p_data);
extern void bta_avk_sig_chg(tBTA_AVK_DATA *p_data);
extern void bta_avk_sig_timer(tBTA_AVK_DATA *p_data);
extern void bta_avk_rc_disc_done(tBTA_AVK_DATA *p_data);
extern void bta_avk_rc_closed(tBTA_AVK_DATA *p_data);
extern void bta_avk_rc_disc(UINT8 disc);
extern void bta_avk_conn_chg(tBTA_AVK_DATA *p_data);
extern void bta_avk_dereg_comp(tBTA_AVK_DATA *p_data);

/* sm action functions */
extern void bta_avk_disable (tBTA_AVK_CB *p_cb, tBTA_AVK_DATA *p_data);
extern void bta_avk_rc_opened (tBTA_AVK_CB *p_cb, tBTA_AVK_DATA *p_data);
extern void bta_avk_rc_br_opened (tBTA_AVK_CB *p_cb, tBTA_AVK_DATA *p_data);
extern void bta_avk_rc_remote_cmd (tBTA_AVK_CB *p_cb, tBTA_AVK_DATA *p_data);
extern void bta_avk_rc_vendor_cmd (tBTA_AVK_CB *p_cb, tBTA_AVK_DATA *p_data);
extern void bta_avk_rc_vendor_rsp (tBTA_AVK_CB *p_cb, tBTA_AVK_DATA *p_data);
extern void bta_avk_rc_msg (tBTA_AVK_CB *p_cb, tBTA_AVK_DATA *p_data);
extern void bta_avk_rc_close (tBTA_AVK_CB *p_cb, tBTA_AVK_DATA *p_data);
extern void bta_avk_rc_br_close (tBTA_AVK_CB *p_cb, tBTA_AVK_DATA *p_data);
extern void bta_avk_rc_meta_rsp (tBTA_AVK_CB *p_cb, tBTA_AVK_DATA *p_data);
extern void bta_avk_rc_free_rsp (tBTA_AVK_CB *p_cb, tBTA_AVK_DATA *p_data);
extern void bta_avk_rc_free_msg (tBTA_AVK_CB *p_cb, tBTA_AVK_DATA *p_data);
extern void bta_avk_rc_free_browse_msg(tBTA_AVK_CB* p_cb, tBTA_AVK_DATA* p_data);
extern tBTA_AVK_RCB * bta_avk_get_rcb_by_shdl(UINT8 shdl);
extern void bta_avk_del_rc(tBTA_AVK_RCB *p_rcb);

/* ssm action functions */
extern void bta_avk_do_disc_a2d (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_cleanup (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_free_sdb (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_config_ind (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_disconnect_req (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_security_req (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_security_rsp (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_setconfig_rsp (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_str_opened (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_security_ind (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_security_cfm (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_do_close (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_connect_req (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_sdp_failed (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_disc_results (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_disc_res_as_acp (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_open_failed (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_getcap_results (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_setconfig_rej (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_discover_req (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_conn_failed (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_do_start (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_str_stopped (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_reconfig (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_data_path (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_start_ok (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_start_failed (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_str_closed (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_clr_cong (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_suspend_cfm (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_rcfg_str_ok (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_rcfg_failed (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_rcfg_connect (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_rcfg_discntd (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_suspend_cont (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_rcfg_cfm (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_rcfg_open (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_security_rej (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_open_rc (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_chk_2nd_start (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_save_caps (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_rej_conn (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_rej_conn (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_set_use_rc (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_cco_close (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_switch_role (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_role_res (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_delay_co (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_open_at_inc (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);

/* ssm action functions - vdp specific */
extern void bta_avk_do_disc_vdp (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_vdp_str_opened (tBTA_AVK_SCB *p_scb, tBTA_AVK_DATA *p_data);
extern void bta_avk_reg_vdp (tAVDT_CS *p_cs, char *p_service_name, void *p_data);

#endif /* BTA_AVK_INT_H */
