/******************************************************************************
 *
 *  Copyright (C) 2003-2012 Broadcom Corporation
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
 *  This is the interface file for advanced audio/video call-out functions.
 *
 ******************************************************************************/
#ifndef BTA_AVK_CO_H
#define BTA_AVK_CO_H

#include "l2c_api.h"
#include "bta_avk_api.h"
#include "a2d_sbc.h"
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
**  Constants and data types
*****************************************************************************/

/* TRUE to use SCMS-T content protection */
#ifndef BTA_AVK_CO_CP_SCMS_T
#define BTA_AVK_CO_CP_SCMS_T     FALSE
#endif

/* the content protection IDs assigned by BT SIG */
#define BTA_AVK_CP_SCMS_T_ID     0x0002
#define BTA_AVK_CP_DTCP_ID       0x0001

#define BTA_AVK_CP_LOSC                  2
#define BTA_AVK_CP_INFO_LEN              3

#define BTA_AVK_CP_SCMS_COPY_MASK        3
#define BTA_AVK_CP_SCMS_COPY_FREE        2
#define BTA_AVK_CP_SCMS_COPY_ONCE        1
#define BTA_AVK_CP_SCMS_COPY_NEVER       0

#define BTA_AVK_CO_DEFAULT_AUDIO_OFFSET      AVDT_MEDIA_OFFSET

enum
{
    BTIF_SV_AVK_AA_SBC_INDEX = 0,
#if defined(AAC_DECODER_INCLUDED) && (AAC_DECODER_INCLUDED == TRUE)
    BTIF_SV_AVK_AA_AAC_INDEX,
#endif
#if defined(MP3_DECODER_INCLUDED) && (MP3_DECODER_INCLUDED == TRUE)
    BTIF_SV_AVK_AA_MP3_INDEX,
#endif
#if defined(APTX_CLASSIC_DECODER_INCLUDED) && (APTX_CLASSIC_DECODER_INCLUDED == TRUE)
    BTIF_SV_AVK_AA_APTX_INDEX,
#endif
    BTIF_SV_AVK_AA_SEP_INDEX,  /* Last index */
};

typedef struct
{
    UINT8 codec_type;                   /* peer SEP codec type */
    union {
        tA2D_SBC_CIE sbc_caps;
        tA2D_AAC_CIE aac_caps;
        tA2D_MP3_CIE mp3_caps;
        tA2D_APTX_CIE aptx_caps;
    } codec_cap;
} tBTA_AVK_CO_CODEC_CAP_LIST;

/*******************************************************************************
**
** Function         bta_avk_co_audio_init
**
** Description      This callout function is executed by AV when it is
**                  started by calling BTA_AvkEnable().  This function can be
**                  used by the phone to initialize audio paths or for other
**                  initialization purposes.
**
**
** Returns          Stream codec and content protection capabilities info.
**
*******************************************************************************/
extern BOOLEAN bta_avk_co_audio_init(UINT8 *p_codec_type, UINT8 *p_codec_info,
                                    UINT8 *p_num_protect, UINT8 *p_protect_info, UINT8 index);

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
extern void bta_avk_co_audio_disc_res(tBTA_AVK_HNDL hndl, UINT8 num_seps,
                    UINT8 num_snk, UINT8 num_src, BD_ADDR addr, UINT16 uuid_local);

/*******************************************************************************
**
** Function         bta_avk_co_audio_getconfig
**
** Description      This callout function is executed by AV to retrieve the
**                  desired codec and content protection configuration for the
**                  audio stream.
**
**
** Returns          Stream codec and content protection configuration info.
**
*******************************************************************************/
extern UINT8 bta_avk_co_audio_getconfig(tBTA_AVK_HNDL hndl, tBTA_AVK_CODEC codec_type,
                                       UINT8 *p_codec_info, UINT8 *p_sep_info_idx, UINT8 seid,
                                       UINT8 *p_num_protect, UINT8 *p_protect_info);

/*******************************************************************************
**
** Function         bta_avk_co_audio_setconfig
**
** Description      This callout function is executed by AV to set the
**                  codec and content protection configuration of the audio stream.
**
**
** Returns          void
**
*******************************************************************************/
extern void bta_avk_co_audio_setconfig(tBTA_AVK_HNDL hndl, tBTA_AVK_CODEC codec_type,
                                        UINT8 *p_codec_info, UINT8 seid, BD_ADDR addr,
                                        UINT8 num_protect, UINT8 *p_protect_info,UINT8 t_local_sep, UINT8 avdt_handle);


/*******************************************************************************
**
** Function         bta_avk_co_audio_open
**
** Description      This function is called by AV when the audio stream connection
**                  is opened.
**                  BTA-AV maintains the MTU of A2DP streams.
**                  If this is the 2nd audio stream, mtu is the smaller of the 2
**                  streams.
**
** Returns          void
**
*******************************************************************************/
extern void bta_avk_co_audio_open(tBTA_AVK_HNDL hndl,
                                 tBTA_AVK_CODEC codec_type, UINT8 *p_codec_info,
                                 UINT16 mtu);

/*******************************************************************************
**
** Function         bta_avk_co_audio_close
**
** Description      This function is called by AV when the audio stream connection
**                  is closed.
**                  BTA-AV maintains the MTU of A2DP streams.
**                  When one stream is closed and no other audio stream is open,
**                  mtu is reported as 0.
**                  Otherwise, the MTU remains open is reported.
**
** Returns          void
**
*******************************************************************************/
extern void bta_avk_co_audio_close(tBTA_AVK_HNDL hndl, tBTA_AVK_CODEC codec_type,
                                  UINT16 mtu);

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
extern void bta_avk_co_audio_start(tBTA_AVK_HNDL hndl, tBTA_AVK_CODEC codec_type,
                                  UINT8 *p_codec_info, BOOLEAN *p_no_rtp_hdr);

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
extern void bta_avk_co_audio_stop(tBTA_AVK_HNDL hndl, tBTA_AVK_CODEC codec_type);

/*******************************************************************************
 **
 ** Function         bta_avk_co_audio_sink_data_path
 **
 ** Description      Dummy Function, Required just because of co fuctions structure definition
 **
 ** Returns          NULL
 **
 *******************************************************************************/
extern void * bta_avk_co_audio_sink_data_path(tBTA_AVK_CODEC codec_type,
                                                    UINT32 *p_len, UINT32 *p_timestamp);

/*******************************************************************************
**
** Function         bta_avk_co_audio_delay
**
** Description      Dummy Function, Required just because of co-fuctions structure definition
**
**
** Returns          void
**
*******************************************************************************/
extern void bta_avk_co_audio_delay(tBTA_AVK_HNDL hndl, UINT16 delay);

#endif /* BTA_AVK_CO_H */
