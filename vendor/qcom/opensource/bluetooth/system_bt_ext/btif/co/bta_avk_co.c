/******************************************************************************
 *  Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
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
 *  This is the advanced audio/video call-out function implementation for
 *  BTIF.
 *
 ******************************************************************************/

#include "string.h"
#include "a2d_api.h"
#include "a2d_sbc.h"
#include "bta_sys.h"
#include "bta_avk_api.h"
#include "bta_avk_co.h"
#include "bta_avk_ci.h"
#include "bta_avk_sbc.h"

#include "btif_avk_media.h"
#include "sbc_encoder.h"
#include "btif_avk_co.h"
#include "btif_util.h"
#include "bt_utils.h"
#if defined(AAC_DECODER_INCLUDED) && (AAC_DECODER_INCLUDED == TRUE)
#include "a2d_aac.h"
#include "bta_avk_aac.h"
#endif
#if defined(MP3_DECODER_INCLUDED) && (MP3_DECODER_INCLUDED == TRUE)
#include "a2d_mp3.h"
#include "bta_avk_mp3.h"
#endif
#if defined(APTX_CLASSIC_DECODER_INCLUDED) && (APTX_CLASSIC_DECODER_INCLUDED == TRUE)
#include "a2d_aptx.h"
#endif

/*****************************************************************************
 **  Constants
 *****************************************************************************/

#define FUNC_TRACE()     APPL_TRACE_DEBUG("%s", __FUNCTION__);

/* Macro to retrieve the number of elements in a statically allocated array */
#define BTA_AVK_CO_NUM_ELEMENTS(__a) (sizeof(__a)/sizeof((__a)[0]))

/* MIN and MAX macros */
#define BTA_AVK_CO_MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define BTA_AVK_CO_MAX(X,Y) ((X) > (Y) ? (X) : (Y))

/* Macro to convert audio handle to index and vice versa */
#define BTA_AVK_CO_AUDIO_HNDL_TO_INDX(hndl) (((hndl) & (~BTA_AVK_CHNL_MSK)) - 1)
#define BTA_AVK_CO_AUDIO_INDX_TO_HNDL(indx) (((indx) + 1) | BTA_AVK_CHNL_AUDIO)


/* Offsets to access codec information in SBC codec */
#define BTA_AVK_CO_SBC_FREQ_CHAN_OFF    3
#define BTA_AVK_CO_SBC_BLOCK_BAND_OFF   4
#define BTA_AVK_CO_SBC_MIN_BITPOOL_OFF  5
#define BTA_AVK_CO_SBC_MAX_BITPOOL_OFF  6

#define BTA_AVK_CO_SBC_MAX_BITPOOL  53

/* SCMS-T protect info */
const UINT8 bta_avk_co_cp_scmst[BTA_AVK_CP_INFO_LEN] = "\x02\x02\x00";

/* SBC SINK codec capabilities */
const tA2D_SBC_CIE bta_avk_co_sbc_caps =
{
    (A2D_SBC_IE_SAMP_FREQ_48 | A2D_SBC_IE_SAMP_FREQ_44), /* samp_freq */
    (A2D_SBC_IE_CH_MD_MONO | A2D_SBC_IE_CH_MD_STEREO | A2D_SBC_IE_CH_MD_JOINT | A2D_SBC_IE_CH_MD_DUAL), /* ch_mode */
    (A2D_SBC_IE_BLOCKS_16 | A2D_SBC_IE_BLOCKS_12 | A2D_SBC_IE_BLOCKS_8 | A2D_SBC_IE_BLOCKS_4), /* block_len */
    (A2D_SBC_IE_SUBBAND_4 | A2D_SBC_IE_SUBBAND_8), /* num_subbands */
    (A2D_SBC_IE_ALLOC_MD_L | A2D_SBC_IE_ALLOC_MD_S), /* alloc_mthd */
    A2D_SBC_IE_MAX_BITPOOL, /* max_bitpool */
    A2D_SBC_IE_MIN_BITPOOL /* min_bitpool */
};

#if defined(AAC_DECODER_INCLUDED) && (AAC_DECODER_INCLUDED == TRUE)
const tA2D_AAC_CIE bta_avk_co_aac_caps =
{
    (A2D_AAC_IE_OBJ_TYPE_MPEG_2_AAC_LC), /* obj type */
    (A2D_AAC_IE_SAMP_FREQ_44100 | A2D_AAC_IE_SAMP_FREQ_48000 ), /* samp_freq */
    (A2D_AAC_IE_CHANNELS_1 | A2D_AAC_IE_CHANNELS_2 ), /* channels  */
    A2D_AAC_IE_BIT_RATE, /* BIT RATE */
    A2D_AAC_IE_VBR_SUPP   /* variable bit rate */
};
#define BTIF_AVK_AAC_DEFAULT_BIT_RATE 0x000409B6
#endif

#if defined(MP3_DECODER_INCLUDED) && (MP3_DECODER_INCLUDED == TRUE)
const tA2D_MP3_CIE bta_avk_co_mp3_caps =
{
    (A2D_MP3_IE_LAYER_3), /* layer */
    (A2D_MP3_IE_CRC),     /* crc */
    (A2D_MP3_IE_CHANNEL_MONO | A2D_MP3_IE_CHANNEL_DUAL| A2D_MP3_IE_CHANNEL_STEREO|
                                           A2D_MP3_IE_CHANNEL_JOINT_STEREO ), /* channels  */
    (0), /* mpf not supported */
    (A2D_MP3_IE_SAMP_FREQ_44100 | A2D_MP3_IE_SAMP_FREQ_48000),                /* frequency */
    A2D_MP3_IE_VBR, /* VBR */
    A2D_MP3_IE_BIT_RATE  /* BIT_RATE */
};
#define BTIF_AVK_MP3_DEFAULT_BIT_RATE 0x096B
#endif

#if !defined(BTIF_AVK_SBC_DEFAULT_SAMP_FREQ)
#define BTIF_AVK_SBC_DEFAULT_SAMP_FREQ A2D_SBC_IE_SAMP_FREQ_48
#endif

/* Default SBC codec configuration */
const tA2D_SBC_CIE btif_avk_sbc_default_config =
{
    BTIF_AVK_SBC_DEFAULT_SAMP_FREQ,   /* samp_freq */
    A2D_SBC_IE_CH_MD_JOINT,         /* ch_mode */
    A2D_SBC_IE_BLOCKS_16,           /* block_len */
    A2D_SBC_IE_SUBBAND_8,           /* num_subbands */
    A2D_SBC_IE_ALLOC_MD_L,          /* alloc_mthd */
    BTA_AVK_CO_SBC_MAX_BITPOOL,      /* max_bitpool */
    A2D_SBC_IE_MIN_BITPOOL          /* min_bitpool */
};

#if defined(AAC_DECODER_INCLUDED) && (AAC_DECODER_INCLUDED == TRUE)
/* Default AAC codec configuration */
const tA2D_AAC_CIE btif_avk_aac_default_config =
{
    A2D_AAC_IE_OBJ_TYPE_MPEG_2_AAC_LC,  /* obj type */
    A2D_AAC_IE_SAMP_FREQ_44100,         /* samp_freq */
    A2D_AAC_IE_CHANNELS_2,              /* channels  */
    BTIF_AVK_AAC_DEFAULT_BIT_RATE,      /* bit rate */
    A2D_AAC_IE_VBR_SUPP                 /* variable bit rate */
};
#endif
#if defined(MP3_DECODER_INCLUDED) && (MP3_DECODER_INCLUDED == TRUE)
/* Default MP3 codec configuration */
const tA2D_MP3_CIE btif_avk_mp3_default_config =
{
    A2D_MP3_IE_LAYER_3,                 /* Layer */
    A2D_MP3_IE_CRC,                     /* CRC */
    A2D_MP3_IE_CHANNEL_JOINT_STEREO,    /* channels  */
    0,                                  /* mpf */
    A2D_MP3_IE_SAMP_FREQ_48000,         /* 48 Khz */
    A2D_MP3_IE_VBR,                     /* VBR Enabled */
    BTIF_AVK_MP3_DEFAULT_BIT_RATE       /* bit rate */
};
#endif
#if defined(APTX_CLASSIC_DECODER_INCLUDED) && (APTX_CLASSIC_DECODER_INCLUDED == TRUE)
/*  APTX codec capabilities */
const tA2D_APTX_CIE bta_avk_co_aptx_caps =
{
    A2D_APTX_VENDOR_ID,
    A2D_APTX_CODEC_ID_BLUETOOTH,
    A2D_APTX_SAMPLERATE_44100 | A2D_APTX_SAMPLERATE_48000,
    A2D_APTX_CHANNELS_STEREO | A2D_APTX_CHANNELS_MONO,
    A2D_APTX_FUTURE_1,
    A2D_APTX_FUTURE_2
};
/* Default APTX codec configuration */
const tA2D_APTX_CIE btif_avk_aptx_default_config =
{
    A2D_APTX_VENDOR_ID,
    A2D_APTX_CODEC_ID_BLUETOOTH,
    A2D_APTX_SAMPLERATE_48000,
    A2D_APTX_CHANNELS_STEREO,
    A2D_APTX_FUTURE_1,
    A2D_APTX_FUTURE_2
};
#endif
/*****************************************************************************
**  Local data
*****************************************************************************/
typedef struct
{
    UINT8 sep_info_idx;                 /* local SEP index (in BTA tables) */
    UINT8 seid;                         /* peer SEP index (in peer tables) */
    UINT8 codec_type;                   /* peer SEP codec type */
    UINT8 codec_caps[AVDT_CODEC_SIZE];  /* peer SEP codec capabilities */
    UINT8 num_protect;                  /* peer SEP number of CP elements */
    UINT8 protect_info[BTA_AVK_CP_INFO_LEN];  /* peer SEP content protection info */
} tBTA_AV_CO_SRC;

typedef struct
{
    BD_ADDR         addr;               /* address of audio/video peer */
    tBTA_AV_CO_SRC srcs[BTIF_SV_AVK_AA_SEP_INDEX]; /* array of supported srcs */
    UINT8           num_snks;           /* total number of sinks at peer */
    UINT8           num_srcs;           /* total number of srcs at peer */
    UINT8           num_seps;           /* total number of seids at peer */
    UINT8           num_rx_snks;        /* number of received sinks */
    UINT8           num_rx_srcs;        /* number of received srcs */
    UINT8           num_sup_srcs;       /* number of supported srcs in the srcs array */
    tBTA_AV_CO_SRC *p_src;             /* currently selected src */
    UINT8           codec_cfg[AVDT_CODEC_SIZE]; /* current codec configuration */
    BOOLEAN         cp_active;          /* current CP configuration */
    BOOLEAN         acp;                /* acceptor */
    BOOLEAN         recfg_needed;       /* reconfiguration is needed */
    BOOLEAN         opened;             /* opened */
    UINT16          mtu;                /* maximum transmit unit size */
} tBTA_AVK_CO_PEER;

typedef struct
{
    BOOLEAN active;
    UINT8 flag;
} tBTA_AVK_CO_CP;

typedef struct
{
    /* Connected peer information */
    tBTA_AVK_CO_PEER peers[BTA_AVK_NUM_STRS];
    /* Current codec configuration - access to this variable must be protected */
    tBTIF_AVK_CODEC_INFO codec_cfg;
    tBTIF_AVK_CODEC_INFO codec_cfg_setconfig; /* remote peer setconfig preference */
    tBTA_AVK_CO_CP cp;
} tBTA_AVK_CO_CB;

/* Control block instance */
static tBTA_AVK_CO_CB bta_avk_co_cb;

/* codec preferance, put corresponding codec id here */
UINT8 codec_pref[BTIF_SV_AVK_AA_SEP_INDEX] = {
#if defined(APTX_CLASSIC_DECODER_INCLUDED) && (APTX_CLASSIC_DECODER_INCLUDED == TRUE)
                                                    A2D_NON_A2DP_MEDIA_CT,
#endif
#if defined(AAC_DECODER_INCLUDED) && (AAC_DECODER_INCLUDED == TRUE)
                                                    BTA_AVK_CODEC_M24,
#endif
#if defined(MP3_DECODER_INCLUDED) && (MP3_DECODER_INCLUDED == TRUE)
                                                    BTA_AVK_CODEC_M12,
#endif
                                                    BTA_AVK_CODEC_SBC
};

static BOOLEAN bta_avk_co_audio_peer_supports_codec(tBTA_AVK_CO_PEER *p_peer,
            UINT8 *p_src_index, UINT8 *p_snk_index);
static BOOLEAN bta_avk_co_audio_supports_config(UINT8 codec_type, const UINT8 *p_codec_cfg);
tBTA_AVK_CO_CODEC_CAP_LIST *p_bta_avk_codec_pri_list = NULL;
tBTA_AVK_CO_CODEC_CAP_LIST bta_avk_supp_codec_cap[BTIF_SV_AVK_AA_SEP_INDEX];
UINT8 bta_avk_num_codec_configs;
extern pthread_mutex_t sink_codec_q_lock;

UINT8 bta_avk_get_current_codec()
{
    return (UINT8)bta_avk_co_cb.codec_cfg.id;
}

/*******************************************************************************
 **
 ** Function         bta_avk_co_get_peer
 **
 ** Description      find the peer entry for a given handle
 **
 ** Returns          the control block
 **
 *******************************************************************************/
static tBTA_AVK_CO_PEER *bta_avk_co_get_peer(tBTA_AVK_HNDL hndl)
{
    UINT8 index;
    FUNC_TRACE();

    index = BTA_AVK_CO_AUDIO_HNDL_TO_INDX(hndl);

    /* Sanity check */
    if (index >= BTA_AVK_CO_NUM_ELEMENTS(bta_avk_co_cb.peers))
    {
        APPL_TRACE_ERROR("bta_avk_co_get_peer peer index out of bounds:%d", index);
        return NULL;
    }

    return &bta_avk_co_cb.peers[index];
}

/*******************************************************************************
 **
 ** Function         append_codec
 **
 ** Description      This function checks if codec entry shld be appended or not
 **                  Don't append in case entry we have duplicate codec entry
 **                  Don't append in case codec is not supported.
 **
 **
 ** Returns          TRUE in case match found
 **                  FALSE  otherwise.
 **
 *******************************************************************************/
static BOOLEAN append_codec(tBTA_AVK_CODEC codec_type, tBTA_AVK_HNDL hndl)
{
    int index = 0;
    BOOLEAN duplicate_codec_entry_present = FALSE;
    BOOLEAN codec_supported = FALSE;
    tBTA_AVK_CO_PEER *p_peer;
    p_peer = bta_avk_co_get_peer(hndl);
    if (p_peer == NULL)
        return FALSE;
    for (index = 0; index < p_peer->num_sup_srcs; index++)
    {
        /* Iterate through all current entries */
        if (p_peer->srcs[index].codec_type == codec_type)
        {
            /* Match FOUND */
            duplicate_codec_entry_present = TRUE;
            break;
        }
    }
    return (!duplicate_codec_entry_present &&
            ((codec_type == BTA_AVK_CODEC_SBC)||
             (codec_type == BTA_AVK_CODEC_M12)||
             (codec_type == A2D_NON_A2DP_MEDIA_CT)||
             (codec_type == BTA_AVK_CODEC_M24)));
}
/*******************************************************************************
 **
 ** Function         bta_avk_co_audio_init
 **
 ** Description      This callout function is executed by AV when it is
 **                  started by calling BTA_AvkRegister().  This function can be
 **                  used by the phone to initialize audio paths or for other
 **                  initialization purposes.
 **
 **
 ** Returns          Stream codec and content protection capabilities info.
 **
 *******************************************************************************/
BOOLEAN bta_avk_co_audio_init(UINT8 *p_codec_type, UINT8 *p_codec_info, UINT8 *p_num_protect,
        UINT8 *p_protect_info, UINT8 index)
{
    FUNC_TRACE();

    APPL_TRACE_DEBUG("bta_avk_co_audio_init: %d", index);

    /* By default - no content protection info */
    *p_num_protect = 0;
    *p_protect_info = 0;

    /* reset remote preference through setconfig */
    bta_avk_co_cb.codec_cfg_setconfig.id = BTIF_AVK_CODEC_NONE;

    switch (index)
    {

    case BTIF_SV_AVK_AA_SBC_INDEX:
        *p_codec_type = BTA_AVK_CODEC_SBC;

        /* This should not fail because we are using constants for parameters */
        A2D_BldSbcInfo(AVDT_MEDIA_AUDIO, (tA2D_SBC_CIE *) &bta_avk_co_sbc_caps, p_codec_info);
        /* Codec is valid */
        break;
#if defined(AAC_DECODER_INCLUDED) && (AAC_DECODER_INCLUDED == TRUE)
    case BTIF_SV_AVK_AA_AAC_INDEX:
        *p_codec_type = BTA_AVK_CODEC_M24;

        /* This should not fail because we are using constants for parameters */
        A2D_BldAacInfo(AVDT_MEDIA_AUDIO, (tA2D_AAC_CIE *) &bta_avk_co_aac_caps, p_codec_info);
        /* Codec is valid */
        break;
#endif/* AAC_DECODER_INCLUDED */
#if defined(MP3_DECODER_INCLUDED) && (MP3_DECODER_INCLUDED == TRUE)
    case BTIF_SV_AVK_AA_MP3_INDEX:
        *p_codec_type = BTA_AVK_CODEC_M12;

        /* This should not fail because we are using constants for parameters */
        A2D_BldMp3Info(AVDT_MEDIA_AUDIO, (tA2D_MP3_CIE *) &bta_avk_co_mp3_caps, p_codec_info);
        /* Codec is valid */
        break;
#endif/* MP3_DECODER_INCLUDED */
#if defined(APTX_CLASSIC_DECODER_INCLUDED) && (APTX_CLASSIC_DECODER_INCLUDED == TRUE)
    case BTIF_SV_AVK_AA_APTX_INDEX:
        *p_codec_type = A2D_NON_A2DP_MEDIA_CT;

        /* This should not fail because we are using constants for parameters */
        A2D_BldAptxInfo(AVDT_MEDIA_AUDIO, (tA2D_APTX_CIE *) &bta_avk_co_aptx_caps, p_codec_info);
        /* Codec is valid */
        break;
#endif/* APTX_CLASSIC_DECODER_INCLUDED */
    default:
        APPL_TRACE_DEBUG("bta_avk_co_audio_init: invalid codec %d");
        return FALSE;
    }

    return TRUE;
}

/*******************************************************************************
 **
 ** Function         bta_avk_co_audio_disc_res
 **
 ** Description      This callout function is executed by AV to report the
 **                  number of stream end points (SEP) were found during the
 **                  AVDT stream discovery process.
 **
 **
 ** Returns          void.
 **
 *******************************************************************************/
void bta_avk_co_audio_disc_res(tBTA_AVK_HNDL hndl, UINT8 num_seps, UINT8 num_snk,
        UINT8 num_src, BD_ADDR addr, UINT16 uuid_local)
{
    tBTA_AVK_CO_PEER *p_peer;

    FUNC_TRACE();

    APPL_TRACE_DEBUG("bta_avk_co_audio_disc_res h:x%x num_seps:%d num_snk:%d num_src:%d",
            hndl, num_seps, num_snk, num_src);

    /* Find the peer info */
    p_peer = bta_avk_co_get_peer(hndl);
    if (p_peer == NULL)
    {
        APPL_TRACE_ERROR("bta_avk_co_audio_disc_res could not find peer entry");
        return;
    }

    /* Sanity check : this should never happen */
    if (p_peer->opened)
    {
        APPL_TRACE_ERROR("bta_avk_co_audio_disc_res peer already opened");
    }

    /* Copy the discovery results */
    bdcpy(p_peer->addr, addr);
    p_peer->num_snks = num_snk;
    p_peer->num_srcs = num_src;
    p_peer->num_seps = num_seps;
    p_peer->num_rx_snks = 0;
    p_peer->num_rx_srcs = 0;
}

/*******************************************************************************
 **
 ** Function         bta_avk_build_sbc_src_cfg
 **
 ** Description      This function will build preferred config from src capabilities for sbc
 **
 **
 ** Returns          Pass or Fail for current getconfig.
 **
 *******************************************************************************/
void bta_avk_build_sbc_src_cfg (UINT8 *p_pref_cfg, UINT8 *p_src_cap, UINT8 index)
{
    tA2D_SBC_CIE    src_cap;
    tA2D_SBC_CIE    pref_cap;
    UINT8           status = 0;
    pthread_mutex_lock(&sink_codec_q_lock);
    tA2D_SBC_CIE    *local_cap = &p_bta_avk_codec_pri_list[index].codec_cap.sbc_caps;
    pthread_mutex_unlock(&sink_codec_q_lock);

    /* initialize it to default SBC configuration */
    A2D_BldSbcInfo(AVDT_MEDIA_AUDIO, local_cap, p_pref_cfg);
    /* now try to build a preferred one */
    /* parse configuration */
    if ((status = A2D_ParsSbcInfo(&src_cap, p_src_cap, TRUE)) != 0)
    {
         APPL_TRACE_DEBUG(" Cant parse src cap ret = %d", status);
         return ;
    }

    if ((src_cap.samp_freq & A2D_SBC_IE_SAMP_FREQ_48) &&
        (local_cap->samp_freq & A2D_SBC_IE_SAMP_FREQ_48))
        pref_cap.samp_freq = A2D_SBC_IE_SAMP_FREQ_48;
    else if ((src_cap.samp_freq & A2D_SBC_IE_SAMP_FREQ_44) &&
        (local_cap->samp_freq & A2D_SBC_IE_SAMP_FREQ_44))
        pref_cap.samp_freq = A2D_SBC_IE_SAMP_FREQ_44;
    else if ((src_cap.samp_freq & A2D_SBC_IE_SAMP_FREQ_32) &&
        (local_cap->samp_freq & A2D_SBC_IE_SAMP_FREQ_32))
        pref_cap.samp_freq = A2D_SBC_IE_SAMP_FREQ_32;
    else if ((src_cap.samp_freq & A2D_SBC_IE_SAMP_FREQ_16) &&
        (local_cap->samp_freq & A2D_SBC_IE_SAMP_FREQ_16))
        pref_cap.samp_freq = A2D_SBC_IE_SAMP_FREQ_16;

    if (src_cap.ch_mode & A2D_SBC_IE_CH_MD_JOINT)
        pref_cap.ch_mode = A2D_SBC_IE_CH_MD_JOINT;
    else if (src_cap.ch_mode & A2D_SBC_IE_CH_MD_STEREO)
        pref_cap.ch_mode = A2D_SBC_IE_CH_MD_STEREO;
    else if (src_cap.ch_mode & A2D_SBC_IE_CH_MD_DUAL)
        pref_cap.ch_mode = A2D_SBC_IE_CH_MD_DUAL;
    else if (src_cap.ch_mode & A2D_SBC_IE_CH_MD_MONO)
        pref_cap.ch_mode = A2D_SBC_IE_CH_MD_MONO;

    if (src_cap.block_len & A2D_SBC_IE_BLOCKS_16)
        pref_cap.block_len = A2D_SBC_IE_BLOCKS_16;
    else if (src_cap.block_len & A2D_SBC_IE_BLOCKS_12)
        pref_cap.block_len = A2D_SBC_IE_BLOCKS_12;
    else if (src_cap.block_len & A2D_SBC_IE_BLOCKS_8)
        pref_cap.block_len = A2D_SBC_IE_BLOCKS_8;
    else if (src_cap.block_len & A2D_SBC_IE_BLOCKS_4)
        pref_cap.block_len = A2D_SBC_IE_BLOCKS_4;

    if (src_cap.num_subbands & A2D_SBC_IE_SUBBAND_8)
        pref_cap.num_subbands = A2D_SBC_IE_SUBBAND_8;
    else if(src_cap.num_subbands & A2D_SBC_IE_SUBBAND_4)
        pref_cap.num_subbands = A2D_SBC_IE_SUBBAND_4;

    if (src_cap.alloc_mthd & A2D_SBC_IE_ALLOC_MD_L)
        pref_cap.alloc_mthd = A2D_SBC_IE_ALLOC_MD_L;
    else if(src_cap.alloc_mthd & A2D_SBC_IE_ALLOC_MD_S)
        pref_cap.alloc_mthd = A2D_SBC_IE_ALLOC_MD_S;

    pref_cap.max_bitpool = src_cap.max_bitpool;
    pref_cap.min_bitpool = src_cap.min_bitpool;

    A2D_BldSbcInfo(AVDT_MEDIA_AUDIO, (tA2D_SBC_CIE *) &pref_cap, p_pref_cfg);
}
#if defined(AAC_DECODER_INCLUDED) && (AAC_DECODER_INCLUDED == TRUE)
/*******************************************************************************
 **
 ** Function         bta_avk_build_aac_src_cfg
 **
 ** Description      This function will build preferred config from src capabilities for aac
 **
 **
 ** Returns          Pass or Fail for current getconfig.
 **
 *******************************************************************************/
void bta_avk_build_aac_src_cfg (UINT8 *p_pref_cfg, UINT8 *p_src_cap, UINT8 index)
{
    tA2D_AAC_CIE    src_cap;
    tA2D_AAC_CIE    pref_cap;
    UINT8           status = 0;
    pthread_mutex_lock(&sink_codec_q_lock);
    tA2D_AAC_CIE    *local_cap = &p_bta_avk_codec_pri_list[index].codec_cap.aac_caps;
    pthread_mutex_unlock(&sink_codec_q_lock);

    /* initialize it to default SBC configuration */
    A2D_BldAacInfo(AVDT_MEDIA_AUDIO, local_cap, p_pref_cfg);
    /* now try to build a preferred one */
    /* parse configuration */
    if ((status = A2D_ParsAacInfo(&src_cap, p_src_cap, TRUE)) != 0)
    {
         APPL_TRACE_DEBUG(" Cant parse src cap ret = %d", status);
         return ;
    }

    /* add support here, if we add new capability */
    if ((src_cap.object_type & A2D_AAC_IE_OBJ_TYPE_MPEG_2_AAC_LC) &&
        (local_cap->object_type & A2D_AAC_IE_OBJ_TYPE_MPEG_2_AAC_LC))
        pref_cap.object_type = A2D_AAC_IE_OBJ_TYPE_MPEG_2_AAC_LC;
    else if ((src_cap.object_type & A2D_AAC_IE_OBJ_TYPE_MPEG_4_AAC_LC) &&
        (local_cap->object_type & A2D_AAC_IE_OBJ_TYPE_MPEG_4_AAC_LC))
        pref_cap.object_type = A2D_AAC_IE_OBJ_TYPE_MPEG_4_AAC_LC;

    if ((src_cap.samp_freq & A2D_AAC_IE_SAMP_FREQ_96000) &&
        (local_cap->samp_freq & A2D_AAC_IE_SAMP_FREQ_96000))
        pref_cap.samp_freq = A2D_AAC_IE_SAMP_FREQ_96000;
    else if ((src_cap.samp_freq & A2D_AAC_IE_SAMP_FREQ_88200) &&
        (local_cap->samp_freq & A2D_AAC_IE_SAMP_FREQ_88200))
        pref_cap.samp_freq = A2D_AAC_IE_SAMP_FREQ_88200;
    else if ((src_cap.samp_freq & A2D_AAC_IE_SAMP_FREQ_64000) &&
        (local_cap->samp_freq & A2D_AAC_IE_SAMP_FREQ_64000))
        pref_cap.samp_freq = A2D_AAC_IE_SAMP_FREQ_64000;
    else if ((src_cap.samp_freq & A2D_AAC_IE_SAMP_FREQ_48000) &&
        (local_cap->samp_freq & A2D_AAC_IE_SAMP_FREQ_48000))
        pref_cap.samp_freq = A2D_AAC_IE_SAMP_FREQ_48000;
    else if ((src_cap.samp_freq & A2D_AAC_IE_SAMP_FREQ_44100) &&
        (local_cap->samp_freq & A2D_AAC_IE_SAMP_FREQ_44100))
        pref_cap.samp_freq = A2D_AAC_IE_SAMP_FREQ_44100;
    else if ((src_cap.samp_freq & A2D_AAC_IE_SAMP_FREQ_32000) &&
        (local_cap->samp_freq & A2D_AAC_IE_SAMP_FREQ_32000))
        pref_cap.samp_freq = A2D_AAC_IE_SAMP_FREQ_32000;
    else if ((src_cap.samp_freq & A2D_AAC_IE_SAMP_FREQ_24000) &&
        (local_cap->samp_freq & A2D_AAC_IE_SAMP_FREQ_24000))
        pref_cap.samp_freq = A2D_AAC_IE_SAMP_FREQ_24000;
    else if ((src_cap.samp_freq & A2D_AAC_IE_SAMP_FREQ_22050) &&
        (local_cap->samp_freq & A2D_AAC_IE_SAMP_FREQ_22050))
        pref_cap.samp_freq = A2D_AAC_IE_SAMP_FREQ_22050;
    else if ((src_cap.samp_freq & A2D_AAC_IE_SAMP_FREQ_16000) &&
        (local_cap->samp_freq & A2D_AAC_IE_SAMP_FREQ_16000))
        pref_cap.samp_freq = A2D_AAC_IE_SAMP_FREQ_16000;
    else if ((src_cap.samp_freq & A2D_AAC_IE_SAMP_FREQ_12000) &&
        (local_cap->samp_freq & A2D_AAC_IE_SAMP_FREQ_12000))
        pref_cap.samp_freq = A2D_AAC_IE_SAMP_FREQ_12000;
    else if ((src_cap.samp_freq & A2D_AAC_IE_SAMP_FREQ_11025) &&
        (local_cap->samp_freq & A2D_AAC_IE_SAMP_FREQ_11025))
        pref_cap.samp_freq = A2D_AAC_IE_SAMP_FREQ_11025;
    else if ((src_cap.samp_freq & A2D_AAC_IE_SAMP_FREQ_8000) &&
        (local_cap->samp_freq & A2D_AAC_IE_SAMP_FREQ_8000))
        pref_cap.samp_freq = A2D_AAC_IE_SAMP_FREQ_8000;

    if (src_cap.channels & A2D_AAC_IE_CHANNELS_2)
        pref_cap.channels = A2D_AAC_IE_CHANNELS_2;
    else if (src_cap.channels & A2D_AAC_IE_CHANNELS_1)
        pref_cap.channels = A2D_AAC_IE_CHANNELS_1;

    pref_cap.bit_rate = src_cap.bit_rate;
    pref_cap.vbr = src_cap.vbr;

    APPL_TRACE_DEBUG("%s: obj type %x freq %x channel %x", __func__,
        pref_cap.object_type, pref_cap.samp_freq, pref_cap.channels);
    A2D_BldAacInfo(AVDT_MEDIA_AUDIO, (tA2D_AAC_CIE *) &pref_cap, p_pref_cfg);
}
#endif
#if defined(MP3_DECODER_INCLUDED) && (MP3_DECODER_INCLUDED == TRUE)
/*******************************************************************************
 **
 ** Function         bta_avk_build_mp3_src_cfg
 **
 ** Description      This function will build preferred config from src capabilities for mp3
 **
 **
 ** Returns          Pass or Fail for current getconfig.
 **
 *******************************************************************************/
void bta_avk_build_mp3_src_cfg (UINT8 *p_pref_cfg, UINT8 *p_src_cap, UINT8 index)
{
    tA2D_MP3_CIE    src_cap;
    tA2D_MP3_CIE    pref_cap;
    UINT8           status = 0;
    pthread_mutex_lock(&sink_codec_q_lock);
    tA2D_MP3_CIE    *local_cap = &p_bta_avk_codec_pri_list[index].codec_cap.mp3_caps;
    pthread_mutex_unlock(&sink_codec_q_lock);

    /* initialize it to default SBC configuration */
    A2D_BldMp3Info(AVDT_MEDIA_AUDIO, local_cap, p_pref_cfg);
    /* now try to build a preferred one */
    /* parse configuration */
    if ((status = A2D_ParsMp3Info(&src_cap, p_src_cap, TRUE)) != 0)
    {
         APPL_TRACE_DEBUG(" Cant parse src cap ret = %d", status);
         return ;
    }

    memset(&pref_cap, 0, sizeof(tA2D_MP3_CIE));
    /* add support here, if we add new capability */
    if ((src_cap.layer & A2D_MP3_IE_LAYER_3) &&
        (local_cap->layer & A2D_MP3_IE_LAYER_3))
        pref_cap.layer = A2D_MP3_IE_LAYER_3;

    if (src_cap.crc & A2D_MP3_IE_CRC)
        pref_cap.crc = A2D_MP3_IE_CRC;

    if (src_cap.channels & A2D_MP3_IE_CHANNEL_JOINT_STEREO)
        pref_cap.channels = A2D_MP3_IE_CHANNEL_JOINT_STEREO;
    else if(src_cap.channels & A2D_MP3_IE_CHANNEL_STEREO)
        pref_cap.channels = A2D_MP3_IE_CHANNEL_STEREO;
    else if(src_cap.channels & A2D_MP3_IE_CHANNEL_DUAL)
        pref_cap.channels = A2D_MP3_IE_CHANNEL_DUAL;
    else if(src_cap.channels & A2D_MP3_IE_CHANNEL_MONO)
        pref_cap.channels = A2D_MP3_IE_CHANNEL_MONO;

    pref_cap.mpf = 0;

    if ((src_cap.samp_freq & A2D_MP3_IE_SAMP_FREQ_48000) &&
        (local_cap->samp_freq & A2D_MP3_IE_SAMP_FREQ_48000))
        pref_cap.samp_freq = A2D_MP3_IE_SAMP_FREQ_48000;
    else if ((src_cap.samp_freq & A2D_MP3_IE_SAMP_FREQ_44100) &&
        (local_cap->samp_freq & A2D_MP3_IE_SAMP_FREQ_44100))
        pref_cap.samp_freq = A2D_MP3_IE_SAMP_FREQ_44100;
    else if ((src_cap.samp_freq & A2D_MP3_IE_SAMP_FREQ_32000) &&
        (local_cap->samp_freq & A2D_MP3_IE_SAMP_FREQ_32000))
        pref_cap.samp_freq = A2D_MP3_IE_SAMP_FREQ_32000;
    else if ((src_cap.samp_freq & A2D_MP3_IE_SAMP_FREQ_24000) &&
        (local_cap->samp_freq & A2D_MP3_IE_SAMP_FREQ_24000))
        pref_cap.samp_freq = A2D_MP3_IE_SAMP_FREQ_24000;
    else if ((src_cap.samp_freq & A2D_MP3_IE_SAMP_FREQ_22050) &&
        (local_cap->samp_freq & A2D_MP3_IE_SAMP_FREQ_22050))
        pref_cap.samp_freq = A2D_MP3_IE_SAMP_FREQ_22050;
    else if ((src_cap.samp_freq & A2D_MP3_IE_SAMP_FREQ_16000) &&
        (local_cap->samp_freq & A2D_MP3_IE_SAMP_FREQ_16000))
        pref_cap.samp_freq = A2D_MP3_IE_SAMP_FREQ_16000;

    pref_cap.bit_rate = src_cap.bit_rate;
    pref_cap.vbr = src_cap.vbr;

    A2D_BldMp3Info(AVDT_MEDIA_AUDIO, (tA2D_MP3_CIE *) &pref_cap, p_pref_cfg);
}
#endif
#if defined(APTX_CLASSIC_DECODER_INCLUDED) && (APTX_CLASSIC_DECODER_INCLUDED == TRUE)
/*******************************************************************************
 **
 ** Function         bta_avk_build_aptx_src_cfg
 **
 ** Description      This function will build preferred config from src capabilities for aptx
 **
 **
 ** Returns          Pass or Fail for current getconfig.
 **
 *******************************************************************************/
void bta_avk_build_aptx_src_cfg (UINT8 *p_pref_cfg, UINT8 *p_src_cap, UINT8 index)
{
    tA2D_APTX_CIE    src_cap;
    tA2D_APTX_CIE    pref_cap;
    UINT8           status = 0;
    pthread_mutex_lock(&sink_codec_q_lock);
    tA2D_APTX_CIE    *local_cap = &p_bta_avk_codec_pri_list[index].codec_cap.aptx_caps;
    pthread_mutex_unlock(&sink_codec_q_lock);

    /* initialize it to default APTX configuration */
    A2D_BldAptxInfo(AVDT_MEDIA_AUDIO, local_cap, p_pref_cfg);
    /* now try to build a preferred one */
    /* parse configuration */
    if ((status = A2D_ParsAptxInfo(&src_cap, p_src_cap, TRUE)) != 0)
    {
         APPL_TRACE_DEBUG(" Cant parse src cap ret = %d", status);
         return ;
    }

    memset(&pref_cap, 0, sizeof(tA2D_APTX_CIE));
    pref_cap.codecId = A2D_APTX_CODEC_ID_BLUETOOTH;
    pref_cap.vendorId = A2D_APTX_VENDOR_ID;
    /* add support here, if we add new capability */
    if ((src_cap.sampleRate & A2D_APTX_SAMPLERATE_48000) &&
        (local_cap->sampleRate & A2D_APTX_SAMPLERATE_48000))
        pref_cap.sampleRate = A2D_APTX_SAMPLERATE_48000;
    else if ((src_cap.sampleRate & A2D_APTX_SAMPLERATE_44100) &&
        (local_cap->sampleRate & A2D_APTX_SAMPLERATE_44100))
        pref_cap.sampleRate = A2D_APTX_SAMPLERATE_44100;

    if (src_cap.channelMode & A2D_APTX_CHANNELS_STEREO)
        pref_cap.channelMode = A2D_APTX_CHANNELS_STEREO;
    else if(src_cap.channelMode & A2D_APTX_CHANNELS_MONO)
        pref_cap.channelMode = A2D_APTX_CHANNELS_MONO;

    A2D_BldAptxInfo(AVDT_MEDIA_AUDIO, (tA2D_APTX_CIE *) &pref_cap, p_pref_cfg);
}
#endif
/*******************************************************************************
 **
 ** Function         bta_av_co_audio_getconfig
 **
 ** Description      This callout function is executed by AV to retrieve the
 **                  desired codec and content protection configuration for the
 **                  audio stream.
 **
 **
 ** Returns          Stream codec and content protection configuration info.
 **
 *******************************************************************************/
UINT8 bta_avk_co_audio_getconfig(tBTA_AVK_HNDL hndl, tBTA_AVK_CODEC codec_type,
        UINT8 *p_codec_info, UINT8 *p_sep_info_idx, UINT8 seid, UINT8 *p_num_protect,
        UINT8 *p_protect_info)

{
    UINT8 result = A2D_FAIL;
    BOOLEAN supported;
    tBTA_AVK_CO_PEER *p_peer;
    tBTA_AV_CO_SRC *p_src;
    UINT8 codec_cfg[AVDT_CODEC_SIZE];
    UINT8 pref_cfg[AVDT_CODEC_SIZE] = {0};
    UINT8 index, snk_index;

    FUNC_TRACE();

    APPL_TRACE_DEBUG("bta_avk_co_audio_getconfig handle:0x%x codec_type:%d seid:%d",
                                                               hndl, codec_type, seid);
    APPL_TRACE_DEBUG("num_protect:0x%02x protect_info:0x%02x%02x%02x",
        *p_num_protect, p_protect_info[0], p_protect_info[1], p_protect_info[2]);

    /* Retrieve the peer info */
    p_peer = bta_avk_co_get_peer(hndl);
    if (p_peer == NULL)
    {
        APPL_TRACE_ERROR("bta_avk_audio_sink_getconfig could not find peer entry");
        return A2D_FAIL;
    }

    APPL_TRACE_DEBUG("bta_avk_co_audio_getconfig peer(o=%d,n_snks=%d,n_rx_snks=%d,n_sup_snks=%d)",
            p_peer->opened, p_peer->num_srcs, p_peer->num_rx_srcs, p_peer->num_sup_srcs);

    p_peer->num_rx_srcs++;

    /* Check if this is a supported configuration */
    supported = FALSE;
    switch (codec_type)
    {
        case BTA_AVK_CODEC_M24:
        case BTA_AVK_CODEC_SBC:
        case BTA_AVK_CODEC_M12:
        case A2D_NON_A2DP_MEDIA_CT:
            supported = TRUE;
            break;

        default:
            break;
    }

    if (supported)
    {
        /* First check for duplicate entry in p_peer->srcs,
         * If there is an entry with same codec type ignore current config
         * do getcap for another SEP_ID
         */
        /* If there is room for a new one */
        if ((append_codec(codec_type, hndl))&&
           (p_peer->num_sup_srcs < BTA_AVK_CO_NUM_ELEMENTS(p_peer->srcs)))
        {
            p_src = &p_peer->srcs[p_peer->num_sup_srcs++];

            APPL_TRACE_DEBUG("bta_avk_co_audio_getconfig saved caps[%x:%x:%x:%x:%x:%x]",
                    p_codec_info[1], p_codec_info[2], p_codec_info[3],
                    p_codec_info[4], p_codec_info[5], p_codec_info[6]);

            memcpy(p_src->codec_caps, p_codec_info, AVDT_CODEC_SIZE);
            p_src->codec_type = codec_type;
            p_src->sep_info_idx = *p_sep_info_idx;
            p_src->seid = seid;
            p_src->num_protect = *p_num_protect;
            memcpy(p_src->protect_info, p_protect_info, BTA_AVK_CP_INFO_LEN);
        }
        else
        {
            APPL_TRACE_ERROR("bta_avk_co_audio_getconfig no more room for SRC info");
        }
    }

    /* If last SNK get capabilities or all supported codec caps retrieved */
    if ((p_peer->num_rx_srcs == p_peer->num_srcs) ||
        (p_peer->num_sup_srcs == BTA_AVK_CO_NUM_ELEMENTS(p_peer->srcs)))
    {
        APPL_TRACE_DEBUG("bta_avk_co_audio_getconfig last SRC reached");

        /* Protect access to bta_av_co_cb.codec_cfg */
        mutex_global_lock();

        /* Find a src that matches the codec config */
        if (bta_avk_co_audio_peer_supports_codec(p_peer, &index, &snk_index))
        {
            APPL_TRACE_DEBUG(" Codec Supported at index %d", snk_index);
            p_src = &p_peer->srcs[index];

            /* Build the codec configuration for this src */
            {
                /* Save the new configuration */
                p_peer->p_src = p_src;
                /* get preferred config from src_caps */
                switch(bta_avk_co_cb.codec_cfg_setconfig.id)
                {
                    case BTA_AVK_CODEC_SBC:
                        bta_avk_build_sbc_src_cfg(pref_cfg, p_src->codec_caps, snk_index);
                        break;
#if defined(AAC_DECODER_INCLUDED) && (AAC_DECODER_INCLUDED == TRUE)
                    case BTA_AVK_CODEC_M24:
                        bta_avk_build_aac_src_cfg(pref_cfg, p_src->codec_caps, snk_index);
                        break;
#endif
#if defined(MP3_DECODER_INCLUDED) && (MP3_DECODER_INCLUDED == TRUE)
                    case BTA_AVK_CODEC_M12:
                        bta_avk_build_mp3_src_cfg(pref_cfg, p_src->codec_caps, snk_index);
                        break;
#endif
#if defined(APTX_CLASSIC_DECODER_INCLUDED) && (APTX_CLASSIC_DECODER_INCLUDED == TRUE)
                    case A2D_NON_A2DP_MEDIA_CT:
                        bta_avk_build_aptx_src_cfg(pref_cfg, p_src->codec_caps, snk_index);
                        break;
#endif
                }
                memcpy(p_peer->codec_cfg, pref_cfg, AVDT_CODEC_SIZE);
                memcpy(bta_avk_co_cb.codec_cfg.info, pref_cfg, AVDT_CODEC_SIZE);

                APPL_TRACE_DEBUG("bta_avk_audio_sink_getconfig  p_codec_info[%x:%x:%x:%x:%x:%x]",
                        p_peer->codec_cfg[1], p_peer->codec_cfg[2], p_peer->codec_cfg[3],
                        p_peer->codec_cfg[4], p_peer->codec_cfg[5], p_peer->codec_cfg[6]);
                /* By default, no content protection */
                *p_num_protect = 0;

#if defined(BTA_AVK_CO_CP_SCMS_T) && (BTA_AVK_CO_CP_SCMS_T == TRUE)
                    p_peer->cp_active = FALSE;
                    bta_avk_co_cb.cp.active = FALSE;
#endif

                    *p_sep_info_idx = p_src->sep_info_idx;
                    memcpy(p_codec_info, p_peer->codec_cfg, AVDT_CODEC_SIZE);
                result =  A2D_SUCCESS;
            }
        }
        /* Protect access to bta_av_co_cb.codec_cfg */
        mutex_global_unlock();
    }
    return result;
}

/*******************************************************************************
 **
 ** Function         bta_avk_co_audio_setconfig
 **
 ** Description      This callout function is executed by AV to set the codec and
 **                  content protection configuration of the audio stream.
 **
 **
 ** Returns          void
 **
 *******************************************************************************/
void bta_avk_co_audio_setconfig(tBTA_AVK_HNDL hndl, tBTA_AVK_CODEC codec_type,
        UINT8 *p_codec_info, UINT8 seid, BD_ADDR addr, UINT8 num_protect, UINT8 *p_protect_info,
        UINT8 t_local_sep, UINT8 avdt_handle)
{
    tBTA_AVK_CO_PEER *p_peer;
    UINT8 status = A2D_SUCCESS;
    UINT8 category = A2D_SUCCESS;
    BOOLEAN recfg_needed = FALSE;
    BOOLEAN codec_cfg_supported = FALSE;
    UNUSED(seid);
    UNUSED(addr);

    FUNC_TRACE();

    APPL_TRACE_IMP("bta_avk_co_audio_setconfig codec_type:%d  p_codec_info[%x:%x:%x:%x:%x:%x:%x]",
            codec_type,p_codec_info[0],p_codec_info[1], p_codec_info[2], p_codec_info[3],
            p_codec_info[4], p_codec_info[5], p_codec_info[6]);
    APPL_TRACE_DEBUG("num_protect:0x%02x protect_info:0x%02x%02x%02x",
        num_protect, p_protect_info[0], p_protect_info[1], p_protect_info[2]);

    /* Retrieve the peer info */
    p_peer = bta_avk_co_get_peer(hndl);
    if (p_peer == NULL)
    {
        APPL_TRACE_ERROR("bta_avk_co_audio_setconfig could not find peer entry");

        /* Call call-in rejecting the configuration */
        bta_avk_ci_setconfig(hndl, A2D_BUSY, AVDT_ASC_CODEC, 0, NULL, FALSE, avdt_handle);
        return;
    }
    APPL_TRACE_DEBUG("bta_avk_co_audio_setconfig peer(o=%d,n_snks=%d,n_rx_snks=%d)",
            p_peer->opened, p_peer->num_snks, p_peer->num_rx_snks);

    /* Sanity check: should not be opened at this point */
    if (p_peer->opened)
    {
        APPL_TRACE_ERROR("bta_avk_co_audio_setconfig peer already in use");
    }

    /* Do not support content protection for the time being */
    if (num_protect != 0)
    {
        APPL_TRACE_ERROR("bta_av_co_audio_setconfig wrong CP configuration");
        status = A2D_BAD_CP_TYPE;
        category = AVDT_ASC_PROTECT;
    }
    if (status == A2D_SUCCESS)
    {
        if(AVDT_TSEP_SNK == t_local_sep)
        {
            codec_cfg_supported = bta_avk_co_audio_supports_config(codec_type, p_codec_info);
            APPL_TRACE_DEBUG(" Peer is  A2DP SRC ");
        }
        /* Check if codec configuration is supported */
        if (codec_cfg_supported)
        {

            /* Protect access to bta_avk_co_cb.codec_cfg */
            mutex_global_lock();

            /* Check if the configuration matches the current codec config */
            switch (codec_type)
            {
            case BTIF_AVK_CODEC_SBC:
#if defined(AAC_DECODER_INCLUDED) && (AAC_DECODER_INCLUDED == TRUE)
            case BTA_AVK_CODEC_M24:
#endif
#if defined(MP3_DECODER_INCLUDED) && (MP3_DECODER_INCLUDED == TRUE)
            case BTA_AVK_CODEC_M12:
#endif
#if defined(APTX_CLASSIC_DECODER_INCLUDED) && (APTX_CLASSIC_DECODER_INCLUDED == TRUE)
            case A2D_NON_A2DP_MEDIA_CT:
#endif
                if ((num_protect == 1) && (!bta_avk_co_cb.cp.active))
                {
                    recfg_needed = TRUE;
                    break;
                }

                bta_avk_co_cb.codec_cfg_setconfig.id = codec_type;
                memcpy(bta_avk_co_cb.codec_cfg_setconfig.info, p_codec_info, AVDT_CODEC_SIZE);
                break;


            default:
                APPL_TRACE_ERROR("bta_avk_co_audio_setconfig unsupported cid %d", codec_type);
                recfg_needed = TRUE;
                break;
            }
            /* Protect access to bta_avk_co_cb.codec_cfg */
            mutex_global_unlock();
        }
        else
        {
            category = AVDT_ASC_CODEC;
            status = A2D_WRONG_CODEC;
        }
    }

    if (status != A2D_SUCCESS)
    {
        APPL_TRACE_ERROR("bta_avk_co_audio_setconfig reject s=%d c=%d", status, category);

        /* Call call-in rejecting the configuration */
        bta_avk_ci_setconfig(hndl, status, category, 0, NULL, FALSE, avdt_handle);
    }
    else
    {
        /* Mark that this is an acceptor peer */
        p_peer->acp = TRUE;
        p_peer->recfg_needed = recfg_needed;

        APPL_TRACE_DEBUG("bta_avk_co_audio_setconfig accept reconf=%d", recfg_needed);

        /* Call call-in accepting the configuration */
        bta_avk_ci_setconfig(hndl, A2D_SUCCESS, A2D_SUCCESS, 0, NULL, recfg_needed, avdt_handle);
    }
}

/*******************************************************************************
 **
 ** Function         bta_avk_co_audio_open
 **
 ** Description      This function is called by AV when the audio stream connection
 **                  is opened.
 **
 **
 ** Returns          void
 **
 *******************************************************************************/
void bta_avk_co_audio_open(tBTA_AVK_HNDL hndl, tBTA_AVK_CODEC codec_type, UINT8 *p_codec_info,
                          UINT16 mtu)
{
    tBTA_AVK_CO_PEER *p_peer;
    UNUSED(p_codec_info);

    FUNC_TRACE();

    APPL_TRACE_DEBUG("bta_avk_co_audio_open mtu:%d codec_type:%d", mtu, codec_type);

    /* Retrieve the peer info */
    p_peer = bta_avk_co_get_peer(hndl);
    if (p_peer == NULL)
    {
        APPL_TRACE_ERROR("bta_avk_co_audio_setconfig could not find peer entry");
    }
    else
    {
        p_peer->opened = TRUE;
        p_peer->mtu = mtu;
    }
}

/*******************************************************************************
 **
 ** Function         bta_avk_co_audio_close
 **
 ** Description      This function is called by AV when the audio stream connection
 **                  is closed.
 **
 **
 ** Returns          void
 **
 *******************************************************************************/
void bta_avk_co_audio_close(tBTA_AVK_HNDL hndl, tBTA_AVK_CODEC codec_type, UINT16 mtu)

{
    tBTA_AVK_CO_PEER *p_peer;
    UNUSED(codec_type);
    UNUSED(mtu);

    FUNC_TRACE();

    APPL_TRACE_DEBUG("bta_avk_co_audio_close");

    /* Retrieve the peer info */
    p_peer = bta_avk_co_get_peer(hndl);
    if (p_peer)
    {
        /* Mark the peer closed and clean the peer info */
        memset(p_peer, 0, sizeof(*p_peer));
    }
    else
    {
        APPL_TRACE_ERROR("bta_avk_co_audio_close could not find peer entry");
    }

    /* reset remote preference through setconfig */
    bta_avk_co_cb.codec_cfg_setconfig.id = BTIF_AVK_CODEC_NONE;
}

/*******************************************************************************
 **
 ** Function         bta_avk_co_audio_start
 **
 ** Description      This function is called by AV when the audio streaming data
 **                  transfer is started.
 **
 **
 ** Returns          void
 **
 *******************************************************************************/
void bta_avk_co_audio_start(tBTA_AVK_HNDL hndl, tBTA_AVK_CODEC codec_type,
                           UINT8 *p_codec_info, BOOLEAN *p_no_rtp_hdr)
{
    tBTA_AVK_CO_PEER *p_peer;
    UNUSED(p_no_rtp_hdr);

    FUNC_TRACE();

    APPL_TRACE_DEBUG("bta_avk_co_audio_start");

    p_peer = bta_avk_co_get_peer(hndl);
    if (p_peer == NULL)
    {
        APPL_TRACE_ERROR("bta_avk_co_audio_start could not find peer entry");
    }
    else
    {

        if(p_peer->opened)
        {
            switch (codec_type)
            {
            case BTIF_AVK_CODEC_SBC:
#if defined(AAC_DECODER_INCLUDED) && (AAC_DECODER_INCLUDED == TRUE)
            case BTA_AVK_CODEC_M24:
#endif
#if defined(MP3_DECODER_INCLUDED) && (MP3_DECODER_INCLUDED == TRUE)
            case BTA_AVK_CODEC_M12:
#endif
#if defined(APTX_CLASSIC_DECODER_INCLUDED) && (APTX_CLASSIC_DECODER_INCLUDED == TRUE)
            case A2D_NON_A2DP_MEDIA_CT:
#endif
                bta_avk_co_cb.codec_cfg.id = codec_type;
                memcpy(bta_avk_co_cb.codec_cfg.info, p_codec_info, AVDT_CODEC_SIZE);
                APPL_TRACE_DEBUG("bta_avk_co_audio_start codec_type = %d",codec_type);
                break;
            default:
                APPL_TRACE_ERROR("bta_avk_co_audio_start unsupported cid %d", codec_type);
                break;
            }
        }
    }

}

/*******************************************************************************
 **
 ** Function         bta_avk_co_audio_stop
 **
 ** Description      This function is called by AV when the audio streaming data
 **                  transfer is stopped.
 **
 **
 ** Returns          void
 **
 *******************************************************************************/
extern void bta_avk_co_audio_stop(tBTA_AVK_HNDL hndl, tBTA_AVK_CODEC codec_type)
{
    UNUSED(hndl);
    UNUSED(codec_type);

    FUNC_TRACE();

    APPL_TRACE_DEBUG("bta_avk_co_audio_stop");
}

/*******************************************************************************
 **
 ** Function         bta_avk_co_audio_sink_data_path
 **
 ** Description      Dummy Function, Required just because of co fuctions structure definition
 **
 ** Returns          NULL
 **
 *******************************************************************************/
void * bta_avk_co_audio_sink_data_path(tBTA_AVK_CODEC codec_type, UINT32 *p_len,
                                     UINT32 *p_timestamp)
{
    UNUSED(p_len); UNUSED(p_timestamp);
    UNUSED(codec_type);
    APPL_TRACE_WARNING("bta_avk_co_audio_sink_data_path called, should not be called ");
    return NULL;
}

/*******************************************************************************
 **
 ** Function         bta_avk_co_audio_delay
 **
 ** Description      Dummy Function, Required just because of co fuctions structure definition
 **
 **
 ** Returns          void
 **
 *******************************************************************************/
void bta_avk_co_audio_delay(tBTA_AVK_HNDL hndl, UINT16 delay)
{
    FUNC_TRACE();

    APPL_TRACE_ERROR("bta_avk_co_audio_delay handle: x%x, delay:0x%x", hndl, delay);
}

/*******************************************************************************
 **
 ** Function         bta_avk_co_audio_codec_cfg_matches_caps
 **
 ** Description      Check if a codec config matches a codec capabilities
 **
 ** Returns          TRUE if codec config is supported, FALSE otherwise
 **
 *******************************************************************************/
static BOOLEAN bta_avk_co_audio_codec_cfg_matches_caps(UINT8 codec_id, const UINT8 *p_src_caps, const UINT8 *p_local_caps)
{
    FUNC_TRACE();

    switch(codec_id)
    {
    case BTIF_AVK_CODEC_SBC:
    {
        tA2D_SBC_CIE*  p_src_sbc_caps = (tA2D_SBC_CIE*)p_src_caps;
        tA2D_SBC_CIE*  p_local_sbc_caps = (tA2D_SBC_CIE*)p_local_caps;
        APPL_TRACE_EVENT("SBC freq: src %d local %d",
                         p_src_sbc_caps->samp_freq, p_local_sbc_caps->samp_freq);
        APPL_TRACE_EVENT(" SBC CH_MODE: src %d local %d",
                         p_src_sbc_caps->ch_mode, p_local_sbc_caps->ch_mode);
        APPL_TRACE_EVENT(" SBC block_len: src %d local %d",
                         p_src_sbc_caps->block_len, p_local_sbc_caps->block_len);
        APPL_TRACE_EVENT(" SBC sub_bands: src %d local %d",
                         p_src_sbc_caps->num_subbands, p_local_sbc_caps->num_subbands);
        APPL_TRACE_EVENT(" SBC alloc_mthd: src %d local %d",
                         p_src_sbc_caps->alloc_mthd, p_local_sbc_caps->alloc_mthd);
        APPL_TRACE_EVENT(" SBC max_bpool: src %d local %d",
                         p_src_sbc_caps->max_bitpool, p_local_sbc_caps->max_bitpool);
        APPL_TRACE_EVENT(" SBC min_bpool: src %d local %d",
                         p_src_sbc_caps->min_bitpool, p_local_sbc_caps->min_bitpool);

        return (((p_src_sbc_caps->samp_freq)&(p_local_sbc_caps->samp_freq))&&
               ((p_src_sbc_caps->ch_mode)&(p_local_sbc_caps->ch_mode))&&
               ((p_src_sbc_caps->block_len)&(p_local_sbc_caps->block_len))&&
               ((p_src_sbc_caps->num_subbands)&(p_local_sbc_caps->num_subbands))&&
               ((p_src_sbc_caps->alloc_mthd)&(p_local_sbc_caps->alloc_mthd)));
    }
        break;
#if defined(AAC_DECODER_INCLUDED) && (AAC_DECODER_INCLUDED == TRUE)
        /* in case of Sink we have to match if Src Cap is a subset of ours */
    case BTA_AVK_CODEC_M24:
    {
        tA2D_AAC_CIE  *p_src_aac_caps = (tA2D_AAC_CIE*)p_src_caps;
        tA2D_AAC_CIE  *p_local_aac_caps = (tA2D_AAC_CIE*)p_local_caps;
        APPL_TRACE_EVENT("AAC obj_type: src %d local %d",
                         p_src_aac_caps->object_type, p_local_aac_caps->object_type);
        APPL_TRACE_EVENT("AAC samp_freq: src %d local %d",
                         p_src_aac_caps->samp_freq, p_local_aac_caps->samp_freq);
        APPL_TRACE_EVENT("AAC channels: src %d local %d",
                         p_src_aac_caps->channels, p_local_aac_caps->channels);
        APPL_TRACE_EVENT("AAC bit_rate: src %d local %d",
                         p_src_aac_caps->bit_rate, p_local_aac_caps->bit_rate);
        APPL_TRACE_EVENT("AAC vbr: src %d local %d",
                         p_src_aac_caps->vbr, p_local_aac_caps->vbr);
        return (((p_src_aac_caps->object_type)&(p_local_aac_caps->object_type))&&
               ((p_src_aac_caps->samp_freq)&(p_local_aac_caps->samp_freq))&&
               ((p_src_aac_caps->channels)&(p_local_aac_caps->channels)));
    }
        break;
#endif
#if defined(MP3_DECODER_INCLUDED) && (MP3_DECODER_INCLUDED == TRUE)
        /* in case of Sink we have to match if Src Cap is a subset of ours */
    case BTA_AVK_CODEC_M12:
    {
        tA2D_MP3_CIE  *p_src_mp3_caps = (tA2D_MP3_CIE*)p_src_caps;
        tA2D_MP3_CIE  *p_local_mp3_caps = (tA2D_MP3_CIE*)p_local_caps;
        APPL_TRACE_EVENT("MP3 Layer: src %d local %d",
                         p_src_mp3_caps->layer, p_local_mp3_caps->layer);
        APPL_TRACE_EVENT("MP3 samp_freq: src %d local %d",
                         p_src_mp3_caps->samp_freq, p_local_mp3_caps->samp_freq);
        APPL_TRACE_EVENT("MP3 channels: src %d local %d",
                         p_src_mp3_caps->channels, p_local_mp3_caps->channels);
        APPL_TRACE_EVENT("MP3 bit_rate: src %d local %d",
                         p_src_mp3_caps->bit_rate, p_local_mp3_caps->bit_rate);
        APPL_TRACE_EVENT("MP3 vbr: src %d local %d",
                         p_src_mp3_caps->vbr, p_local_mp3_caps->vbr);
        APPL_TRACE_EVENT("MP3 mpf: src %d local %d",
                         p_src_mp3_caps->mpf, p_local_mp3_caps->mpf);
        return (((p_src_mp3_caps->layer)&(p_local_mp3_caps->layer))&&
               ((p_src_mp3_caps->samp_freq)&(p_local_mp3_caps->samp_freq))&&
               ((p_src_mp3_caps->mpf) == (p_local_mp3_caps->mpf)) &&
               ((p_src_mp3_caps->channels)&(p_local_mp3_caps->channels)));
    }
        break;
#endif
#if defined(APTX_CLASSIC_DECODER_INCLUDED) && (APTX_CLASSIC_DECODER_INCLUDED == TRUE)
        /* in case of Sink we have to match if Src Cap is a subset of ours */
    case A2D_NON_A2DP_MEDIA_CT:
    {
        tA2D_APTX_CIE  *p_src_aptx_caps = (tA2D_APTX_CIE*)p_src_caps;
        tA2D_APTX_CIE  *p_local_aptx_caps = (tA2D_APTX_CIE*)p_local_caps;
        APPL_TRACE_EVENT("APTX codec_id : src %d local %d",
                         p_src_aptx_caps->codecId, p_local_aptx_caps->codecId);
        APPL_TRACE_EVENT("APTX VendorId : src %d local %d",
                         p_src_aptx_caps->vendorId, p_local_aptx_caps->vendorId);
        APPL_TRACE_EVENT("APTX samplingFreq: src %d local %d",
                         p_src_aptx_caps->sampleRate, p_local_aptx_caps->sampleRate);
        APPL_TRACE_EVENT("APTX Channel Count: src %d local %d",
                         p_src_aptx_caps->channelMode, p_local_aptx_caps->channelMode);

        return (((p_src_aptx_caps->codecId)&(p_local_aptx_caps->codecId))&&
                ((p_src_aptx_caps->vendorId)&(p_local_aptx_caps->vendorId))&&
                ((p_src_aptx_caps->sampleRate)&(p_local_aptx_caps->sampleRate))&&
                ((p_src_aptx_caps->channelMode)&(p_local_aptx_caps->channelMode)));
    }
        break;
#endif
    default:
        APPL_TRACE_ERROR("bta_avk_co_audio_codec_cfg_matches_caps: unsupported codec id %d", codec_id);
        return FALSE;
        break;
    }
    APPL_TRACE_EVENT("TRUE");
    return TRUE;
}

/*******************************************************************************
 **
 ** Function         bta_avk_co_audio_peer_supports_codec
 **
 ** Description      Check if a connection supports the codec config
 **
 ** Returns          TRUE if the connection supports this codec, FALSE otherwise
 **
 *******************************************************************************/
static BOOLEAN bta_avk_co_audio_peer_supports_codec(tBTA_AVK_CO_PEER *p_peer,
            UINT8 *p_src_index, UINT8 *p_snk_index)
{
    int index;
    UINT8 codec_type;
    FUNC_TRACE();
    int preference_index = 0;

    pthread_mutex_lock(&sink_codec_q_lock);
    do
    {
        /* Configure the codec type to look for */
        codec_type = p_bta_avk_codec_pri_list[preference_index].codec_type;

        for (index = 0; index < p_peer->num_sup_srcs; index++)
        {
            APPL_TRACE_DEBUG(" sink preferred_type = %d  src_codec_type = %d",
                                          codec_type, p_peer->srcs[index].codec_type);
            if (p_peer->srcs[index].codec_type == codec_type)
            {
                switch (codec_type)
                {
                case BTIF_AVK_CODEC_SBC:
                {
                    tA2D_SBC_CIE src_sbc_cap;
                    if(A2D_ParsSbcInfo(&src_sbc_cap, p_peer->srcs[index].codec_caps, TRUE)
                                                                             != A2D_SUCCESS)
                    {
                        APPL_TRACE_ERROR(" Error in  A2D_ParsSbcInfo ");
                        break;
                    }
                    if (bta_avk_co_audio_codec_cfg_matches_caps(codec_type,
                                        (UINT8*)&src_sbc_cap,
                                        (UINT8*)&p_bta_avk_codec_pri_list[preference_index]
                                        .codec_cap.sbc_caps))
                    {
                        if (p_src_index) *p_src_index = index;
                        if (p_snk_index) *p_snk_index = preference_index;
                        bta_avk_co_cb.codec_cfg_setconfig.id = codec_type;
                        pthread_mutex_unlock(&sink_codec_q_lock);
                        return TRUE;
                    }
                }
                    break;

#if defined(AAC_DECODER_INCLUDED) && (AAC_DECODER_INCLUDED == TRUE)
                case BTA_AVK_CODEC_M24:
                {
                    tA2D_AAC_CIE src_aac_cap;
                    if(A2D_ParsAacInfo(&src_aac_cap, p_peer->srcs[index].codec_caps, TRUE)
                                                                             != A2D_SUCCESS)
                    {
                        APPL_TRACE_ERROR(" Error in  A2D_ParsAacInfo ");
                        break;
                    }
                    if (bta_avk_co_audio_codec_cfg_matches_caps(codec_type,
                                        (UINT8*)&src_aac_cap,
                                        (UINT8*)&p_bta_avk_codec_pri_list[preference_index]
                                        .codec_cap.aac_caps))
                    {
                        if (p_src_index) *p_src_index = index;
                        if (p_snk_index) *p_snk_index = preference_index;
                        bta_avk_co_cb.codec_cfg_setconfig.id = codec_type;
                        pthread_mutex_unlock(&sink_codec_q_lock);
                        return TRUE;
                    }
                }
                    break;
#endif
#if defined(MP3_DECODER_INCLUDED) && (MP3_DECODER_INCLUDED == TRUE)
                case BTA_AVK_CODEC_M12:
                {
                    tA2D_MP3_CIE src_mp3_cap;
                    if(A2D_ParsMp3Info(&src_mp3_cap, p_peer->srcs[index].codec_caps, TRUE)
                                                                             != A2D_SUCCESS)
                    {
                        APPL_TRACE_ERROR(" Error in  A2D_ParsMp3Info ");
                        break;
                    }
                    if (bta_avk_co_audio_codec_cfg_matches_caps(codec_type,
                                        (UINT8*)&src_mp3_cap,
                                        (UINT8*)&p_bta_avk_codec_pri_list[preference_index]
                                        .codec_cap.mp3_caps))
                    {
                        if (p_src_index) *p_src_index = index;
                        if (p_snk_index) *p_snk_index = preference_index;
                        bta_avk_co_cb.codec_cfg_setconfig.id = codec_type;
                        pthread_mutex_unlock(&sink_codec_q_lock);
                        return TRUE;
                    }
                }
                    break;
#endif
#if defined(APTX_CLASSIC_DECODER_INCLUDED) && (APTX_CLASSIC_DECODER_INCLUDED == TRUE)
                case A2D_NON_A2DP_MEDIA_CT:
                {
                    tA2D_APTX_CIE src_aptx_cap;
                    if(A2D_ParsAptxInfo(&src_aptx_cap, p_peer->srcs[index].codec_caps, TRUE)
                                                                             != A2D_SUCCESS)
                    {
                        APPL_TRACE_ERROR(" Error in  A2D_ParsAptxInfo ");
                        break;
                    }
                    if (bta_avk_co_audio_codec_cfg_matches_caps(codec_type,
                                        (UINT8*)&src_aptx_cap,
                                        (UINT8*)&p_bta_avk_codec_pri_list[preference_index]
                                        .codec_cap.aptx_caps))
                    {
                        if (p_src_index) *p_src_index = index;
                        if (p_snk_index) *p_snk_index = preference_index;
                        bta_avk_co_cb.codec_cfg_setconfig.id = codec_type;
                        pthread_mutex_unlock(&sink_codec_q_lock);
                        return TRUE;
                    }
                }
                    break;
#endif
                default:
                    APPL_TRACE_ERROR("bta_avk_co_audio_peer_supports_codec: unsupported codec id %d", codec_type);
                    pthread_mutex_unlock(&sink_codec_q_lock);
                    return FALSE;
                    break;
                }
            }
        }
        preference_index ++;
        APPL_TRACE_DEBUG(" preferred codec index = %d ", preference_index);
    }while (preference_index < bta_avk_num_codec_configs);
    pthread_mutex_unlock(&sink_codec_q_lock);
    return FALSE;
}


/*******************************************************************************
 **
 ** Function         bta_avk_co_audio_sink_supports_config
 **
 ** Description      Check if the media source supports a given configuration
 **
 ** Returns          TRUE if the media source supports this config, FALSE otherwise
 **
 *******************************************************************************/
static BOOLEAN bta_avk_co_audio_supports_config(UINT8 codec_type, const UINT8 *p_codec_cfg)
{
    FUNC_TRACE();

    switch (codec_type)
    {
    case BTA_AVK_CODEC_SBC:
        pthread_mutex_lock(&sink_codec_q_lock);
        if (bta_avk_sbc_cfg_in_cap((UINT8 *)p_codec_cfg,
            (tA2D_SBC_CIE *)&bta_avk_supp_codec_cap[BTIF_SV_AVK_AA_SBC_INDEX]
            .codec_cap.sbc_caps))
        {
            pthread_mutex_unlock(&sink_codec_q_lock);
            return FALSE;
        }
        pthread_mutex_unlock(&sink_codec_q_lock);
        break;
#if defined(AAC_DECODER_INCLUDED) && (AAC_DECODER_INCLUDED == TRUE)
    case BTA_AVK_CODEC_M24:
        pthread_mutex_lock(&sink_codec_q_lock);
        if (bta_avk_aac_cfg_in_cap((UINT8 *)p_codec_cfg,
            (tA2D_AAC_CIE *)&bta_avk_supp_codec_cap[BTIF_SV_AVK_AA_AAC_INDEX]
            .codec_cap.aac_caps))
        {
            pthread_mutex_unlock(&sink_codec_q_lock);
            return FALSE;
        }
        pthread_mutex_unlock(&sink_codec_q_lock);
        break;
#endif
#if defined(MP3_DECODER_INCLUDED) && (MP3_DECODER_INCLUDED == TRUE)
    case BTA_AVK_CODEC_M12:
        pthread_mutex_lock(&sink_codec_q_lock);
        if (bta_avk_mp3_cfg_in_cap((UINT8 *)p_codec_cfg,
            (tA2D_MP3_CIE *)&bta_avk_supp_codec_cap[BTIF_SV_AVK_AA_MP3_INDEX]
            .codec_cap.mp3_caps))
        {
            pthread_mutex_unlock(&sink_codec_q_lock);
            return FALSE;
        }
        pthread_mutex_unlock(&sink_codec_q_lock);
        break;
#endif
#if defined(APTX_CLASSIC_DECODER_INCLUDED) && (APTX_CLASSIC_DECODER_INCLUDED == TRUE)
    case A2D_NON_A2DP_MEDIA_CT:
        pthread_mutex_lock(&sink_codec_q_lock);
        if (a2d_av_aptx_cfg_in_cap((UINT8 *)p_codec_cfg,
            (tA2D_APTX_CIE *)&bta_avk_supp_codec_cap[BTIF_SV_AVK_AA_APTX_INDEX]
            .codec_cap.aptx_caps))
        {
            pthread_mutex_unlock(&sink_codec_q_lock);
            return FALSE;
        }
        pthread_mutex_unlock(&sink_codec_q_lock);
        break;
#endif
    default:
        APPL_TRACE_ERROR("bta_avk_co_audio_media_supports_config unsupported codec type %d", codec_type);
        return FALSE;
        break;
    }
    return TRUE;
}

/*******************************************************************************
 **
 ** Function         bta_avk_co_audio_codec_reset
 **
 ** Description      Reset the preffered codec configuration
 **
 ** Returns          void
 **
 *******************************************************************************/
void bta_avk_co_audio_codec_reset(void)
{
    mutex_global_lock();
    FUNC_TRACE();

    /* Reset the preferred  configuration */
    pthread_mutex_lock(&sink_codec_q_lock);
    bta_avk_co_cb.codec_cfg_setconfig.id = p_bta_avk_codec_pri_list[0].codec_type;
    switch(bta_avk_co_cb.codec_cfg_setconfig.id)
    {
    case BTA_AVK_CODEC_SBC:
        if (A2D_BldSbcInfo(A2D_MEDIA_TYPE_AUDIO,
                    &p_bta_avk_codec_pri_list[0].codec_cap.sbc_caps,
                    bta_avk_co_cb.codec_cfg.info) != A2D_SUCCESS)
        {
            APPL_TRACE_ERROR("bta_avk_co_audio_codec_reset A2D_BldSbcInfo failed");
        }
        break;
#if defined(AAC_DECODER_INCLUDED) && (AAC_DECODER_INCLUDED == TRUE)
    case BTA_AVK_CODEC_M24:
        if (A2D_BldAacInfo(A2D_MEDIA_TYPE_AUDIO,
                    &p_bta_avk_codec_pri_list[0].codec_cap.aac_caps,
                    bta_avk_co_cb.codec_cfg.info) != A2D_SUCCESS)
        {
            APPL_TRACE_ERROR("bta_avk_co_audio_codec_reset A2D_BldAacInfo failed");
        }
        break;
#endif
#if defined(MP3_DECODER_INCLUDED) && (MP3_DECODER_INCLUDED == TRUE)
    case BTA_AVK_CODEC_M12:
        if (A2D_BldMp3Info(A2D_MEDIA_TYPE_AUDIO,
                    &p_bta_avk_codec_pri_list[0].codec_cap.mp3_caps,
                    bta_avk_co_cb.codec_cfg.info) != A2D_SUCCESS)
        {
            APPL_TRACE_ERROR("bta_avk_co_audio_codec_reset A2D_BldMp3Info failed");
        }
        break;
#endif
#if defined(APTX_CLASSIC_DECODER_INCLUDED) && (APTX_CLASSIC_DECODER_INCLUDED == TRUE)
    case A2D_NON_A2DP_MEDIA_CT:
        if (A2D_BldAptxInfo(A2D_MEDIA_TYPE_AUDIO,
                    &p_bta_avk_codec_pri_list[0].codec_cap.aptx_caps,
                    bta_avk_co_cb.codec_cfg.info) != A2D_SUCCESS)
        {
            APPL_TRACE_ERROR("bta_avk_co_audio_codec_reset A2D_BldAptxInfo failed");
        }
        break;
#endif
    }
    pthread_mutex_unlock(&sink_codec_q_lock);
    mutex_global_unlock();
}

/*******************************************************************************
 **
 ** Function         bta_avk_co_init
 **
 ** Description      Initialization
 **
 ** Returns          Nothing
 **
 *******************************************************************************/
void bta_avk_co_init(void)
{
    FUNC_TRACE();

    /* Reset the control block */
    memset(&bta_avk_co_cb, 0, sizeof(bta_avk_co_cb));

    bta_avk_co_cb.codec_cfg_setconfig.id = BTIF_AVK_CODEC_NONE;
    /* Reset the current config */
    bta_avk_co_audio_codec_reset();
}

