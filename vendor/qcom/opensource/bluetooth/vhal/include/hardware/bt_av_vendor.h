/*
 * Copyright (c) 2016, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
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
 */

#ifndef ANDROID_INCLUDE_BT_AV_VENDOR_H
#define ANDROID_INCLUDE_BT_AV_VENDOR_H

#define BT_PROFILE_ADVANCED_AUDIO_VENDOR_ID "a2dp_vendor"
#define BT_PROFILE_ADVANCED_AUDIO_SINK_VENDOR_ID "a2dp_sink_vendor"

__BEGIN_DECLS

#define A2DP_SINK_ENABLE_SBC_DECODING       0x01
#define A2DP_SINK_RETREIVE_RTP_HEADER       0x02
#define A2DP_SINK_ENABLE_DELAY_REPORTING    0X04
#define A2DP_SINK_ENABLE_NOTIFICATION_CB    0x08
#define A2DP_SRC_ENABLE_DELAY_REPORTING     0x01
#define A2DP_SRC_PUMP_ENCODED_DATA          0x02

#define A2DP_SINK_AUDIO_CODEC_SBC       0x00
#define A2DP_SINK_AUDIO_CODEC_MP3       0x01
#define A2DP_SINK_AUDIO_CODEC_AAC       0x02
#define A2DP_SINK_AUDIO_CODEC_ATRAC     0x04
#define A2DP_SINK_AUDIO_CODEC_APTX      0x08
#define A2DP_SINK_AUDIO_CODEC_PCM       0x40

#define A2DP_SOURCE_AUDIO_CODEC_SBC       0x00
#define A2DP_SOURCE_AUDIO_CODEC_APTX      0x08

/* SBC Codec Macros */
#define SBC_SAMP_FREQ_16     0x80    /* 16 Khz */
#define SBC_SAMP_FREQ_32     0x40    /* 32 Khz */
#define SBC_SAMP_FREQ_44     0x20    /* 44.1 Khz */
#define SBC_SAMP_FREQ_48     0x10    /* 48 Khz */

#define SBC_CH_MONO          0x08    /* Channel Mono */
#define SBC_CH_DUAL          0x04    /* Channel Dual */
#define SBC_CH_STEREO        0x02    /* Channel STEREO */
#define SBC_CH_JOINT         0x01    /* Channel DUAL STEREO */

#define SBC_BLOCKS_4         0x80    /* BLOCKS: 4 */
#define SBC_BLOCKS_8         0x40    /* BLOCKS: 8 */
#define SBC_BLOCKS_12        0x20    /* BLOCKS: 12 */
#define SBC_BLOCKS_16        0x10    /* BLOCKS: 16 */

#define SBC_SUBBAND_4        0x08    /* SUBBAND 4 */
#define SBC_SUBBAND_8        0x04    /* SUBBAND 8 */

#define SBC_ALLOC_SNR        0x02    /* ALLOCATION_MODE: SNR */
#define SBC_ALLOC_LOUDNESS   0x01    /* ALLOCATION_MODE: LOUDNESS */

#define SBC_MAX_BITPOOL      250
#define SBC_MIN_BITPOOL      2

/* AAC Codec Macros */
#define AAC_OBJ_TYPE_MPEG_2_AAC_LC      0x80      /* b7:MPEG-2 AAC LC */
#define AAC_OBJ_TYPE_MPEG_4_AAC_LC      0x40      /* b7:MPEG-4 AAC LC */
#define AAC_OBJ_TYPE_MPEG_4_AAC_LTP     0x20      /* b7:MPEG-4 AAC LTP */
#define AAC_OBJ_TYPE_MPEG_4_AAC_SCA     0x10      /* b7:MPEG-4 AAC SCALABLE */

#define AAC_SAMP_FREQ_8000              0x8000    /* b15: 8000 */
#define AAC_SAMP_FREQ_11025             0x4000    /* b15: 11025 */
#define AAC_SAMP_FREQ_12000             0x2000    /* b15: 12000 */
#define AAC_SAMP_FREQ_16000             0x1000    /* b15: 16000 */
#define AAC_SAMP_FREQ_22050             0x0800    /* b15: 22050 */
#define AAC_SAMP_FREQ_24000             0x0400    /* b15: 24000 */
#define AAC_SAMP_FREQ_32000             0x0200    /* b15: 32000 */
#define AAC_SAMP_FREQ_44100             0x0100    /* b15: 44100 */
#define AAC_SAMP_FREQ_48000             0x0080    /* b15: 48000 */
#define AAC_SAMP_FREQ_64000             0x0040    /* b15: 64000 */
#define AAC_SAMP_FREQ_88200             0x0020    /* b15: 88200 */
#define AAC_SAMP_FREQ_96000             0x0010    /* b15: 96000 */

#define AAC_CHANNELS_1                  0x08      /* Channel 1 */
#define AAC_CHANNELS_2                  0x04      /* Channel 2 */

/* MP3 Codec Macros */
#define MP3_LAYER_1                     0x80    /* b7:MP1 */
#define MP3_LAYER_2                     0x40    /* b6:MP2 */
#define MP3_LAYER_3                     0x20    /* b5:MP3 */

#define MP3_CRC                         0x10    /* b4:SUPPORTED */

#define MP3_CHANNEL_MONO                0x08    /* Channel MONO */
#define MP3_CHANNEL_DUAL                0x04    /* Channel DUAL */
#define MP3_CHANNEL_STEREO              0x02    /* Channel STEREO */
#define MP3_CHANNEL_JOINT_STEREO        0x01    /* Channel JOINT_STEREO */

#define MP3_MPF_2                       0x40    /* b6:MPF2 supported, otwerwise 0 */

#define MP3_SAMP_FREQ_16000             0x20    /* b5: 16000 */
#define MP3_SAMP_FREQ_22050             0x10    /* b4: 22050 */
#define MP3_SAMP_FREQ_24000             0x08    /* b3: 24000 */
#define MP3_SAMP_FREQ_32000             0x04    /* b2: 32000 */
#define MP3_SAMP_FREQ_44100             0x02    /* b1: 441000 */
#define MP3_SAMP_FREQ_48000             0x01    /* b0: 48000 */

#define MP3_VBR                         0x80    /* supported */

/* APTX Codec Macros */
#define APTX_SAMPLERATE_44100       0x20
#define APTX_SAMPLERATE_48000       0x10
#define APTX_CHANNELS_STEREO        0x02
#define APTX_CHANNELS_MONO          0x01

#define MAX_NUM_CODEC_CONFIGS       20

/* Packet type Macros */
#define EDR_2MBPS        0x01
#define EDR_3MBPS        0x02

typedef struct {
    uint8_t   samp_freq;
    uint8_t   ch_mode;
    uint8_t   block_len;
    uint8_t   num_subbands;
    uint8_t   alloc_mthd;
    uint8_t   max_bitpool;
    uint8_t   min_bitpool;
}btav_sbc_codec_config_t;

typedef struct {
    uint8_t  obj_type;
    uint16_t sampling_freq;
    uint8_t  channel_count;
    uint32_t bit_rate;
    uint8_t  vbr; // variable bit rate
}btav_aac_codec_config_t;

typedef struct {
    uint8_t  layer;
    uint8_t  crc;
    uint8_t  channel_count;
    uint8_t  mpf;
    uint8_t  sampling_freq;
    uint8_t  vbr; // variable bit rate
    uint16_t bit_rate;
}btav_mp3_codec_config_t;

typedef struct {
    uint32_t vendor_id;
    uint16_t codec_id;
    uint8_t  channel_count;
    uint8_t  sampling_freq;
}btav_aptx_codec_config_t;

typedef union
{
    btav_sbc_codec_config_t sbc_config;
    btav_aac_codec_config_t aac_config;
    btav_mp3_codec_config_t mp3_config;
    btav_aptx_codec_config_t aptx_config;
} btav_codec_config_t;

typedef struct
{
    uint8_t codec_type;
    btav_codec_config_t codec_config;
} btav_codec_configuration_t;

/** Callback for audio codec configuration change.
 *  Used only for the A2DP sink interface.
 *  codec_type: codec negotiated over A2DP link
 *  codec_config: configuration parameters for which link is configured
 */
typedef void (* btav_audio_codec_config_vendor_callback)(bt_bdaddr_t *bd_addr,
                 uint16_t codec_type, btav_codec_config_t codec_config);

/** Vendor callback for connection priority of device for incoming connection
 * btav_connection_priority_t
 */
typedef void (* btav_connection_priority_vendor_callback)(bt_bdaddr_t *bd_addr);

/** Vendor callback for updating apps for A2dp multicast state.
 */
typedef void (* btav_is_multicast_enabled_vendor_callback)(int state);

/** Callback to notify reconfig a2dp when A2dp Soft Handoff is triggered
*/
typedef void(* btav_reconfig_a2dp_trigger_callback)(int reason, bt_bdaddr_t *bd_addr);

/*
 * Vendor callback for audio focus request to be used only in
 * case of A2DP Sink. This is required because we are using
 * AudioTrack approach for audio data rendering.
 */
typedef void (* btav_audio_focus_request_vendor_callback)(bt_bdaddr_t *bd_addr);

typedef void (* btav_delay_report_vendor_callback)(bt_bdaddr_t *bd_addr, uint16_t report_delay);

typedef void (* btav_audio_data_read_vendor_callback)(bt_bdaddr_t *bd_addr);

typedef void (*btav_mtu_packettype_config_vendor_callback)(uint16_t mtu,uint8_t packettype, bt_bdaddr_t *bd_addr);

typedef void (*btav_audio_registration_vendor_callback)(bool state);

/** BT-AV Vendor callback structure. */
typedef struct {
    /** set to sizeof(btav_vendor_callbacks_t) */
    size_t      size;
    btav_connection_priority_vendor_callback connection_priority_vendor_cb;
    btav_is_multicast_enabled_vendor_callback multicast_state_vendor_cb;
    btav_audio_focus_request_vendor_callback audio_focus_request_vendor_cb;
    btav_reconfig_a2dp_trigger_callback reconfig_a2dp_trigger_cb;
    btav_delay_report_vendor_callback delay_report_vendor_cb;
    btav_audio_codec_config_vendor_callback audio_codec_config_vendor_cb;
    btav_mtu_packettype_config_vendor_callback mtu_packettype_cb;
    btav_audio_registration_vendor_callback registration_vendor_cb;
} btav_vendor_callbacks_t;

typedef struct {
    /** set to sizeof(btav_sink_vendor_callbacks_t) */
    size_t      size;
    btav_audio_focus_request_vendor_callback audio_focus_request_vendor_cb;
    btav_audio_codec_config_vendor_callback audio_codec_config_vendor_cb;
    btav_audio_data_read_vendor_callback audio_data_read_vendor_cb;
    btav_audio_registration_vendor_callback registration_vendor_cb;
} btav_sink_vendor_callbacks_t;

/** Represents the standard BT-AV interface.
 *  Used for both the A2DP source and sink interfaces.
 */
typedef struct {
    /** set to sizeof(btav_vendor_interface_t) */
    size_t          size;
    /**
     * Register the BtAvVendorcallbacks
     */
    bt_status_t (*init_vendor)( btav_vendor_callbacks_t* callbacks , int max_a2dp_connections,
                        int a2dp_multicast_state, uint8_t streaming_prarm, const char *offload_cap);

    /** Send priority of device to stack*/
    void (*allow_connection_vendor)( int is_valid , bt_bdaddr_t *bd_addr);

   /** Sends Audio Focus State. */
   void  (*audio_focus_state_vendor)( int focus_state );
    /** request config codec information **/
    bool (*get_src_codec_config)(uint8_t *codecinfo , uint8_t *codectype);

   /** Request PCM sample. */
   uint32_t  (*get_pcm_data_vendor)( uint8_t* data, uint32_t size );

   /** Closes the av vendor interface. */
   void  (*cleanup_vendor)( void );

   /** Updates the supported codec by A2DP Source  */
   bt_status_t (*update_supported_codecs_param_vendor)( btav_codec_configuration_t
                        *p_codec_config_list, uint8_t num_codec_configs);
   /** Starts A2DP stream. */
   int (*start_stream)( bt_bdaddr_t *bd_addr);

   /** Sends encoded data. */
   ssize_t (*btav_send_encoded_data_vendor)( bt_bdaddr_t *bd_addr, const void* buffer, size_t bytes, uint8_t codectype);

   /** Suspends A2DP stream. */
   int (*suspend_stream)( bt_bdaddr_t *bd_addr);

} btav_vendor_interface_t;

/** Represents the standard BT-AV interface.
 *  Used for A2DP sink interfaces.
 */
typedef struct {
    /** set to sizeof(btav_vendor_interface_t) */
    size_t          size;
    /**
     * Register the BtAvVendorcallbacks
     */
    bt_status_t (*init_vendor)( btav_sink_vendor_callbacks_t* callbacks , int max_a2dp_connections,
                        int a2dp_multicast_state, uint8_t streaming_prarm);

    /** Sends Audio Focus State. */
    void  (*audio_focus_state_vendor)( int focus_state,  bt_bdaddr_t *bd_addr );

   /** Request PCM sample. */
   uint32_t  (*get_a2dp_sink_streaming_data_vendor)( uint16_t codec_type, uint8_t* data, uint32_t size );

   /** Send streaming device address to stack*/
   void (*update_streaming_device_vendor)( bt_bdaddr_t *bd_addr);

   /** Send flushing device address to stack*/
   void (*update_flushing_device_vendor)( bt_bdaddr_t *bd_addr);

   /** Closes the av vendor interface. */
   void  (*cleanup_vendor)( void );

   /** Send decoding delay to stack during decoding non_SBC stream in LPASS */
   void(*update_qahw_delay_vendor)(uint16_t aqhw_delay);

   /** Updates the supported codec by A2DP Sink  */
   bt_status_t (*update_supported_codecs_param_vendor)( btav_codec_configuration_t
                        *p_codec_config_list, uint8_t num_codec_configs);
} btav_sink_vendor_interface_t;
__END_DECLS

#endif /* ANDROID_INCLUDE_BT_AV_VENDOR_H */
