/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"

#include <stdio.h>
#include <string.h>

#include "csr_sched.h"
#include "csr_pmem.h"
#include "csr_bt_util.h"
#include "csr_bt_av_prim.h"
#include "csr_bt_av_lib.h"
#include "csr_bt_cm_lib.h"
#include "csr_bt_sc_lib.h"
#include "csr_sbc_api.h"
#include "csr_bt_platform.h"
#include "csr_bt_av2.h"
#include "csr_bt_av2_filter.h"
#include "csr_app_lib.h"

/* Settings */
#define SBC_PRN                      "filter_sbc_pts: "
#define SBC_BURST                    0  /* Split up into this many packets, 0 to disable */
#define SBC_DEFAULT_MAX_BITPOOL      64
#define SBC_DEFAULT_MIN_BITPOOL      24
#define SBC_DECODE_BUFFER_SIZE       4096
#define SBC_MEDIA_PAYLOAD_TYPE       96

/*
#define DEBUG                        1
*/

/* SBC codec definitions */
#define SC_HDR_SIZE                  2
#define SC_LENGTH_INDEX              1
#define SBC_MEDIA_CODEC_SC_SIZE      8
#define SBC_IE_SAMPLE_FREQ_MASK      ((CsrUint8)0xF0)
#define SBC_IE_CHANNEL_MODE_MASK     ((CsrUint8)0x0F)
#define SBC_IE_BLOCK_MASK            ((CsrUint8)0xF0)
#define SBC_IE_SUBBAND_MASK          ((CsrUint8)0x0C)
#define SBC_IE_ALLOC_METHOD_MASK     ((CsrUint8)0x03)

/* Instance data for this filter */
typedef struct
{
    CsrUint8          sbc_capabilities[4];
    CsrSbcChannelMode channel_mode;
    CsrSbcAllocMethod alloc_method;
    void             *sbcHdl;
    CsrUint16         sample_freq;
    CsrUint8          blocks;
    CsrUint8          subbands;
    CsrUint8          bitPool;
    CsrUint8          minBitpool;
    CsrUint8          maxBitpool;
    CsrUint8          remoteMinBitpool;
    CsrUint8          remoteMaxBitpool;
    CsrUint16         framesize;
    CsrUint8          channels;
    CsrUint16         frames_per_payload;
    CsrUint16         bytes_per_encoding;
    CsrUint16         maxFrameSize;
    CsrUint32         timestamp;
    CsrUint8          configure_pts[SBC_MEDIA_CODEC_SC_SIZE];  /* SBC_MEDIA_CODEC_SC_SIZE = 8 */
    av2filter_t      *filter;
} sbc_instance_t;


/* Filter entry prototypes */
static CsrBool sbc_init(void **instance, av2filter_t *filter, av2instance_t *unused);
static CsrBool sbc_deinit(void **instance);
static CsrBool sbc_open(void **instance);
static CsrBool sbc_close(void **instance);
static CsrBool sbc_start(void **instance);
static CsrBool sbc_stop(void **instance);
static CsrBool sbc_process_encode(CsrUint8 index,
                                 av2filter_t **chain,
                                 CsrBool freeData,
                                 void *data,
                                 CsrUint32 length);
static CsrBool sbc_process_decode(CsrUint8 index,
                                 av2filter_t **chain,
                                 CsrBool freeData,
                                 void *data,
                                 CsrUint32 length);
static CsrUint8 *sbc_get_config(void **instance,
                               CsrUint8 *conf_len);
static CsrBtAvResult sbc_set_config(void **instance,
                                  CsrUint8 *conf,
                                  CsrUint8 conf_len);
static CsrUint8 *sbc_get_caps(void **instance,
                             CsrUint8 *cap_len);
static CsrUint8 *sbc_remote_caps(void **instance,
                                CsrUint8 *caps,
                                CsrUint8 caps_len,
                                CsrUint8 *conf_len);
static void sbc_qos_update(void **instance,
                           CsrUint16 qosLevel);
static void sbc_menu(void **instance,
                     CsrUint8 *num_options);
static CsrBool sbc_setup(void **instance,
                        CsrUint8 index,
                        char *value);

/* Filter structure setup for SBC encoder */
struct av2filter_t filter_sbc_encode =
{
    "csr_sbc_encoder",
    FILTER_ENCODER | FILTER_PASS,
    CSR_BT_AV_AUDIO,
    CSR_BT_AV_SBC,
    0,
    NULL,
    sbc_init,
    sbc_deinit,
    sbc_open,
    sbc_close,
    sbc_start,
    sbc_stop,
    sbc_process_encode,
    sbc_get_config,
    sbc_set_config,
    sbc_get_caps,
    sbc_remote_caps,
    sbc_qos_update,
    sbc_menu,
    sbc_setup
};

/* Filter structure setup for SBC decoder */
av2filter_t filter_sbc_decode =
{
    "csr_sbc_decoder",
    FILTER_DECODER | FILTER_PASS,
    CSR_BT_AV_AUDIO,
    CSR_BT_AV_SBC,
    0,
    NULL,
    sbc_init,
    sbc_deinit,
    sbc_open,
    sbc_close,
    sbc_start,
    sbc_stop,
    sbc_process_decode,
    sbc_get_config,
    sbc_set_config,
    sbc_get_caps,
    sbc_remote_caps,
    sbc_qos_update,
    sbc_menu,
    sbc_setup
};

/* HCI/RFC platform variable, set in common/hci/csr_bt_rfc_main.c */
extern CsrUint16 targetBitrate;

/* Local function to calculate settings given SBC specific codec config */
static void sbc_store_config(sbc_instance_t *inst, CsrUint8 *config)
{
    /* Deleted. No use when using PTS*/
}

/* Initialise SBC filter */
static CsrBool sbc_init(void **instance, struct av2filter_t *filter, av2instance_t *unused)
{
    sbc_instance_t *inst;
    CsrUint8 bits;
    CsrUint16 bufsize;

    *instance = CsrPmemZalloc(sizeof(sbc_instance_t));
    inst = (sbc_instance_t*)*instance;
    inst->filter = filter;

    inst->sbcHdl = CsrSbcOpen();

    /* These are just defaults - can be changed online */
    inst->maxFrameSize = getMaxFrameSize();
    getPcmSettings(&(inst->channels), &bits, &(inst->sample_freq));
    if(inst->channels > 1)
    {
        inst->channel_mode = CSR_SBC_STEREO;
    }
    else
    {
        inst->channel_mode = CSR_SBC_MONO;
    }
    inst->blocks                = CSR_SBC_MAX_BLOCKS;
    inst->subbands              = CSR_SBC_MAX_SUBBANDS;
    inst->alloc_method          = CSR_SBC_METHOD_SNR;
    inst->maxBitpool            = SBC_DEFAULT_MAX_BITPOOL;
    inst->minBitpool            = SBC_DEFAULT_MIN_BITPOOL;
    inst->framesize             = (SBC_DEFAULT_MAX_BITPOOL + SBC_DEFAULT_MIN_BITPOOL)/2;
    inst->frames_per_payload    = (CsrUint16)((inst->maxFrameSize - 13) / inst->framesize);
    inst->timestamp             = 0;

    /* Set the preferred sample buffer size */
#if SBC_BURST > 0
    bufsize = (inst->frames_per_payload *
               (CsrUint16)inst->blocks *
               (CsrUint16)(inst->subbands);
               * SBC_BURST);
#else
    bufsize = 12288;
#endif
    setSampleBufferSize(bufsize);

    /* Support all settings */
    inst->sbc_capabilities[0] = 0xFF;
    inst->sbc_capabilities[1] = 0xFF;
    inst->sbc_capabilities[2] = 0x02;
    inst->sbc_capabilities[3] = 0xFA;

    return TRUE;
}

/* Deinitialise SBC filter */
static CsrBool sbc_deinit(void **instance)
{
    sbc_instance_t *inst;
    inst = (sbc_instance_t*)*instance;

    CsrSbcClose(&inst->sbcHdl);

    /* Free instance */
    CsrPmemFree(*instance);
    *instance = NULL;

    return TRUE;
}

/* Open SBC with current configuration */
static CsrBool sbc_open(void **instance)
{
    CsrUint8 bitPoolAlt;
    CsrUint8 togglePeriod;
    CsrUint8 bits;
    CsrUint16 bufsize;
    double frame_usage;
    sbc_instance_t *inst;
    inst = (sbc_instance_t*)*instance;

    if(getPcmLocked())
    {
        getPcmSettings(&(inst->channels), &bits, &(inst->sample_freq));
    }

    if(inst->filter->f_type & FILTER_DECODER)
    {
        CsrSbcInitDecoder(inst->sbcHdl);
    }

    /* Perform open */
    inst->bitPool = CsrSbcCalcBitPool(&bitPoolAlt,
                                    &togglePeriod,
                                    inst->channel_mode,
                                    inst->sample_freq,
                                    inst->blocks,
                                    inst->subbands,
                                    targetBitrate);

    if(inst->bitPool > inst->maxBitpool)
    {
        inst->bitPool = inst->maxBitpool;
    }

    inst->framesize = CsrSbcConfig(inst->sbcHdl,
                                 inst->channel_mode,
                                 inst->alloc_method,
                                 inst->sample_freq,
                                 inst->blocks,
                                 inst->subbands,
                                 inst->bitPool);

    if(inst->framesize != 0)
    {
        while(1)
        {
            frame_usage = ((double)((inst->maxFrameSize - 14) % inst->framesize)) / inst->framesize;

            /* Downgrade the bitpool size to have a good fit for the l2cap MTU
             * frame_usage: ~how well does the last sbc frame fit the l2cap frame */
            if((frame_usage > 0.9) && (inst->bitPool > inst->minBitpool))
            {
                inst->bitPool--;
            }
            else
            {
                break;
            }

            inst->framesize = CsrSbcConfig(inst->sbcHdl,
                                         inst->channel_mode,
                                         inst->alloc_method,
                                         inst->sample_freq,
                                         inst->blocks,
                                         inst->subbands,
                                         inst->bitPool);
        }

        /* Calculate how many SBC frames fit into maximum size payload packet */
        inst->frames_per_payload = (CsrUint16)((inst->maxFrameSize - 13) / inst->framesize);

        /* Calculate the amount of sample bytes used per SBC encode call */
        inst->bytes_per_encoding = (CsrUint16)(inst->blocks * inst->channels * inst->subbands<<1);

        /* Set the preferred sample buffer size */
        bufsize = inst->frames_per_payload *
            (CsrUint16)inst->blocks *
            (CsrUint16)(inst->subbands * SBC_BURST);
        setSampleBufferSize(bufsize);
        setMediaPayloadType(SBC_MEDIA_PAYLOAD_TYPE);

        return TRUE;
    }
    else
    {
        /* Unable to configure SBC with the given parameters */
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBC_PRN "invalid framesize\n");
        return FALSE;
    }

#ifdef DEBUG
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBC_PRN "chan_mode.............%i\n", inst->channel_mode);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBC_PRN "alloc_method..........%i\n", inst->alloc_method);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBC_PRN "sample_freq...........%i\n", inst->sample_freq);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBC_PRN "blocks................%i\n", inst->blocks);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBC_PRN "subbands..............%i\n", inst->subbands);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBC_PRN "minBitpool............%i\n", inst->minBitpool);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBC_PRN "maxBitpool............%i\n", inst->maxBitpool);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBC_PRN "bitPool...............%i\n", inst->bitPool);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBC_PRN "framesize.............%i\n", inst->framesize);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBC_PRN "channels..............%i\n", inst->channels);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBC_PRN "frames_per_payload....%i\n", inst->frames_per_payload);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBC_PRN "bytes_per_encoding....%i\n", inst->bytes_per_encoding);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBC_PRN "remoteMinBitpool......%i\n", inst->remoteMinBitpool);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBC_PRN "remoteMaxBitpool......%i\n", inst->remoteMaxBitpool);
#endif

    return TRUE;
}

/* Close SBC configuration */
static CsrBool sbc_close(void **instance)
{
    sbc_instance_t *inst;
    inst = (sbc_instance_t*)*instance;

    /* SBC does not require any special close handling */
    return TRUE;
}

/* Start SBC streaming */
static CsrBool sbc_start(void **instance)
{
    sbc_instance_t *inst;
    inst = (sbc_instance_t*)*instance;

    return TRUE;
}

/* Stop SBC streaming */
static CsrBool sbc_stop(void **instance)
{
    sbc_instance_t *inst;
    inst = (sbc_instance_t*)*instance;




    /* SBC does not require any special stop handling */
    return TRUE;
}

/* Process SBC data: Encoder */
static CsrBool sbc_process_encode(CsrUint8 index,
                                 av2filter_t **chain,
                                 CsrBool freeData,
                                 void *data,
                                 CsrUint32 length)
{
    sbc_instance_t *inst;
    CsrUint16 packet_size;   /* Entire packet size */
    CsrUint8 *packet_ptr;    /* Entire packet */
    CsrUint8 *payload_ptr;   /* SBC payload start */
    CsrUint8 *frame_ptr;     /* Single frame pointer */
    CsrUint8 *samples_ptr;   /* Pointer to sample to encode */
    CsrInt32 samples_left;   /* Current number of bytes to consume */
    CsrUint8 frames;         /* Number of frames */
    CsrUint8 burst;          /* Burst-run */
    CsrBool result;          /* Function result */
    inst = (sbc_instance_t*)(chain[index]->f_instance);

    /* Calculate maximum size for packet */
    packet_size = ((inst->frames_per_payload * inst->framesize) + 1
                   + CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE);
    result = TRUE;

#ifdef DEBUG
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBC_PRN "process_encode: sample length: %i, bytes_per_encoding: %i, estim_frames: %i\n",
           length,
           inst->bytes_per_encoding,
           length / inst->bytes_per_encoding);
#endif

    samples_ptr = (CsrUint8*)data;
#if SBC_BURST > 0
    for(burst=0; burst<SBC_BURST; burst++)
#else
    burst = 0;
    while(samples_ptr < ((CsrUint8*)data + length))
#endif
    {
        /* Allocate space for this packet,
         * Set offset pointer for payload and frame data */
        packet_ptr  = CsrPmemAlloc(packet_size);
        payload_ptr = packet_ptr + CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE;
        frame_ptr   = payload_ptr + 1;

#if SBC_BURST > 0
        samples_left = length / SBC_BURST;
#else
        samples_left = inst->frames_per_payload * inst->bytes_per_encoding;
#endif

        frames = 0;
        while((samples_left > 0) && (samples_ptr < ((CsrUint8*)data+length)))
        {
            inst->framesize = CsrSbcEncode(inst->sbcHdl,
                                         (short int(*)[2])samples_ptr,
                                         frame_ptr,
                                         inst->bitPool);

            frames++;
            frame_ptr += inst->framesize;

            samples_ptr  += inst->bytes_per_encoding;
            samples_left -= inst->bytes_per_encoding;
        }

        /* Add payload header */
        *payload_ptr = frames;

        /* Free data and deliver data to next stage */
#ifdef DEBUG
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBC_PRN "process_encode: send %i frames (%i bytes) in packet size %i bytes, last framesize: %i\n",
               frames,
               (frames * inst->framesize + CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE + 1),
               packet_size,
               inst->framesize);

#endif
        result &= chain[index+1]->f_process((CsrUint8)(index+1),
                                            chain,
                                            TRUE,
                                            packet_ptr,
                                            (frames * inst->framesize + CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE + 1));

        /* Update timestamp */
        inst->timestamp += frames * inst->blocks * inst->subbands;
        setTimestamp(inst->timestamp);
    }

    /* We finished the entire SBC encode run, free original buffer */
    if(freeData && data)
    {
        CsrPmemFree(data);
    }
    return result;
}

/* Process SBC data: Decoder */
static CsrBool sbc_process_decode(CsrUint8 index,
                                 av2filter_t **chain,
                                 CsrBool freeData,
                                 void *data,
                                 CsrUint32 length)
{
    CsrUint8 block;
    CsrUint8 subband;
    CsrUint8 chan;
    CsrUint8 *frame_ptr;
    CsrUint16 frame_size;
    CsrUint32 bytes_left;
    CsrUint16 audio[CSR_SBC_MAX_BLOCKS][2][CSR_SBC_MAX_SUBBANDS];
    CsrUint16 *audio_ptr;
    CsrUint8 *audio_out;
    CsrUint32 audio_len;
    sbc_instance_t *inst;
    inst = (sbc_instance_t*)(chain[index]->f_instance);

#ifdef DEBUG
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBC_PRN "process_decode\n");
#endif

    frame_ptr  = (CsrUint8*)data + CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE + 1;
    bytes_left = length - CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE - 1;
    frame_size = 0;

    audio_ptr = (CsrUint16*)CsrPmemAlloc(SBC_DECODE_BUFFER_SIZE);
    audio_out = (CsrUint8*)audio_ptr;
    audio_len  = 0;

    /* Process all frames in packet */
    do
    {
#ifdef DEBUG
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBC_PRN "decode, bytes_left:%i, frame_size:%i\n", bytes_left, frame_size);
#endif
        if((frame_size = CsrSbcReadHeader(inst->sbcHdl, frame_ptr)) != 0 )
        {
            CsrSbcDecode(inst->sbcHdl, frame_ptr, audio);

            for(block=0; block < inst->blocks; block++)
            {
                for(subband=0; subband < inst->subbands; subband++)
                {
                    for(chan=0; chan < inst->channels; chan++)
                    {
                        *(audio_ptr)++ = audio[block][chan][subband];
                        audio_len += 2;
                    }
                }
            }

            frame_ptr  += frame_size;
            bytes_left -= frame_size;
        }
        else
        {
            /* Unable to decode the packet, discarding it */
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBC_PRN "unable to decode SBC frame, discarding\n");
            CsrPmemFree(data);
            return FALSE;
        }
    } while((bytes_left >= frame_size) && (bytes_left > 0));

#ifdef DEBUG
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBC_PRN "done with this buffer, audio_len:%i, allocated %i\n",
           audio_len, SBC_DECODE_BUFFER_SIZE);
#endif


    /* Free packet and pass data to next stage */
    if(freeData && data)
    {
        CsrPmemFree(data);
    }
    return chain[index+1]->f_process((CsrUint8)(index+1),
                                     chain,
                                     TRUE,
                                     audio_out,
                                     audio_len);
}


/* Return SBC configuration */
static CsrUint8 *sbc_get_config(void **instance,
                               CsrUint8 *conf_len)
{
    CsrUint8 tmp;
    CsrUint8 *conf;
    sbc_instance_t *inst;
    inst = (sbc_instance_t*)*instance;

#ifdef DEBUG
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBC_PRN "get_config\n");
#endif

    *conf_len = SBC_MEDIA_CODEC_SC_SIZE;
    conf = CsrPmemAlloc(*conf_len);

    conf[0] = CSR_BT_AV_SC_MEDIA_CODEC;
    conf[1] = SBC_MEDIA_CODEC_SC_SIZE - 2;
    conf[2] = CSR_BT_AV_AUDIO<<4;
    conf[3] = CSR_BT_AV_SBC;

    /* Sampling rate */
    switch(inst->sample_freq)
    {
        case 48000:
            tmp = 0x10;
            break;

        case 44100:
            tmp = 0x20;
            break;

        case 32000:
            tmp = 0x40;
            break;

        default:
            tmp = 0x80;
            break;
    }

    /* Channel mode */
    conf[4] = (0x08>>inst->channel_mode) | tmp;

    /* Allocation */
    tmp = (inst->alloc_method == CSR_SBC_METHOD_SNR) ? 0x02 : 0x01;

    /* Blocks, allocation and subbands */
    conf[5] = ((0x80>>((inst->blocks>>2)-1)) |
               (0x08>>((inst->subbands>>2)-1)) |
               tmp);

    /* Bitpools */
    conf[6] = inst->minBitpool;
    conf[7] = inst->maxBitpool;

    return conf;
}

/* Set SBC configuration */
static CsrBtAvResult sbc_set_config(void **instance,
                                  CsrUint8 *conf,
                                  CsrUint8 conf_len)
{
    sbc_instance_t *inst;
    inst = (sbc_instance_t*)*instance;

#ifdef DEBUG
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBC_PRN "set_config\n");
#endif

    /* Dummy check that service capabilities are SBC */
    if(!(*(conf+2)>>4 == CSR_BT_AV_AUDIO) ||
       !(*(conf+3) == CSR_BT_AV_SBC))
    {
#ifdef DEBUG
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBC_PRN "set_config: unsupported config detected\n");
#endif
        return CSR_BT_RESULT_CODE_A2DP_UNSUPPORTED_CONFIGURATION;
    }

    /* Store configuration */
    inst->remoteMinBitpool = *(conf+6);
    inst->remoteMaxBitpool = *(conf+7);

    /* Perform a reconfiguration */
    sbc_stop(*instance);
    sbc_close(*instance);
    sbc_store_config(inst, conf+4);

    /* Store new PCM settings */
    setPcmSettings(inst->channels, 16, inst->sample_freq);

    if(!sbc_open(instance))
    {
        return CSR_BT_RESULT_CODE_A2DP_BAD_MEDIA_TRANSPORT_FORMAT;
    }
    else
    {
        return CSR_BT_AV_ACCEPT;
    }
}


/* Return SBC service capabilities */
static CsrUint8 *sbc_get_caps(void **instance,
                             CsrUint8 *cap_len)
{
    CsrUint8 *cap;
    CsrUint8 i;
    sbc_instance_t *inst;

    inst = (sbc_instance_t*)*instance;
    *cap_len = SBC_MEDIA_CODEC_SC_SIZE + 2;
    cap = CsrPmemAlloc(*cap_len);

    cap[0] = CSR_BT_AV_SC_MEDIA_TRANSPORT;
    cap[1] = 0;
    cap[2] = CSR_BT_AV_SC_MEDIA_CODEC;
    cap[3] = SBC_MEDIA_CODEC_SC_SIZE - 2;
    cap[4] = CSR_BT_AV_AUDIO<<4;
    cap[5] = CSR_BT_AV_SBC;
    for(i=0; i<4; i++)
    {
        cap[6+i] = inst->sbc_capabilities[i];
    }

    return cap;
}

static CsrUint8 *sbc_remote_caps(void **instance,
                                CsrUint8 *caps,
                                CsrUint8 caps_len,
                                CsrUint8 *conf_len)
{
    CsrUint8 *conf;
    sbc_instance_t *inst;

    inst = (sbc_instance_t*)*instance;

    /* Dummy check that service capabilities are SBC */
    if(!(*(caps+2)>>4 == CSR_BT_AV_AUDIO) ||
       !(*(caps+3) == CSR_BT_AV_SBC))
    {
        *conf_len = 0;
        return NULL;
    }

    /* Alloc config */
    *conf_len = SBC_MEDIA_CODEC_SC_SIZE;
    conf = (CsrUint8*)CsrPmemAlloc(*conf_len);


    conf[0] = inst->configure_pts[0];
    conf[1] = inst->configure_pts[1];
    conf[2] = inst->configure_pts[2];
    conf[3] = inst->configure_pts[3];
    conf[4] = inst->configure_pts[4];
    conf[5] = inst->configure_pts[5];
    conf[6] = inst->configure_pts[6];
    conf[7] = inst->configure_pts[7];

    /* If PCM settings are locked (due to wave streaming) we just
     * use the current configuration function */
    if(getPcmLocked())
    {
        CsrUint8 tmp;
        CsrPmemFree(conf);
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"PCM settings are locked, returning current configuration\n");

        /* Get newest PCM settings */
        getPcmSettings((&inst->channels),
                       &tmp,
                       &(inst->sample_freq));

        return sbc_get_config(instance, conf_len);
    }

    return conf;
}

/* Investigate remote capabilities, return optimal configuration */
static CsrUint8 *sbc_remote_caps_org(void **instance,
                                CsrUint8 *caps,
                                CsrUint8 caps_len,
                                CsrUint8 *conf_len)
{
    CsrUint8 i;
    CsrUint8 *conf;
    sbc_instance_t *inst;

    inst = (sbc_instance_t*)*instance;

    /* Dummy check that service capabilities are SBC */
    if(!(*(caps+2)>>4 == CSR_BT_AV_AUDIO) ||
       !(*(caps+3) == CSR_BT_AV_SBC))
    {
        *conf_len = 0;
        return NULL;
    }

    /* Alloc config */
    *conf_len = SBC_MEDIA_CODEC_SC_SIZE;
    conf = (CsrUint8*)CsrPmemAlloc(*conf_len);

    /* Store remote bitpools to use in reconfig */
    inst->minBitpool = *(caps+6);
    inst->maxBitpool = *(caps+7);
    inst->remoteMinBitpool = *(caps+6);
    inst->remoteMaxBitpool = *(caps+7);

    /* Common codec service capability */
    conf[0] = CSR_BT_AV_SC_MEDIA_CODEC;
    conf[1] = SBC_MEDIA_CODEC_SC_SIZE - 2;
    conf[2] = CSR_BT_AV_AUDIO<<4;
    conf[3] = CSR_BT_AV_SBC;

    /* SBC specific: Sampling frequency */
    for(i=0; i<4; i++)
    {
        /* ...a workaround for Sonorix (has very bad sound unless using 32kHz) */
        if((*(caps+4) == 0x6a) && (*(caps+7) == 0x2c))
        {
            conf[4] = 0x40;
            break;
        }
        if(*(caps+4) & (0x10<<i) & inst->sbc_capabilities[0])
        {
            conf[4] = (0x10<<i);
            break;
        }
    }

    /* SBC specific: Channel mode */
    for(i=0; i<4; i++)
    {
        if(*(caps+4) & (0x01<<i) & inst->sbc_capabilities[0])
        {
            conf[4] |= (0x01<<i);
            break;
        }
    }

    /* SBC specific: Block length */
    for( i=0; i<4; i++)
    {
        if(*(caps+5) & (0x10<<i) & inst->sbc_capabilities[1])
        {
            conf[5] = (0x10<<i);
            break;
        }
    }

    /* SBC specific: Subbands */
    for(i=0; i<2; i++)
    {
        if(*(caps+5) & (0x04<<i) & inst->sbc_capabilities[1])
        {
            conf[5] |= (0x04<<i);
            break;
        }
    }

    /* SBC specific: Allocation method */
    for(i=0; i<2; i++)
    {
        if(*(caps+5) & (0x01<<i) & inst->sbc_capabilities[1])
        {
            conf[5] |= (0x01<<i);
            break;
        }
    }

    /* SBC specific: Min and CSRMAX bitpool */
    conf[6] = inst->minBitpool;
    conf[7] = inst->maxBitpool;

    /* Store calculated configuration to be able to reconfig */
    sbc_store_config(inst, conf+4);

    /* If PCM settings are locked (due to wave streaming) we just
     * use the current configuration function */
    if(getPcmLocked())
    {
        CsrUint8 tmp;
        CsrPmemFree(conf);
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"PCM settings are locked, returning current configuration\n");

        /* Get newest PCM settings */
        getPcmSettings((&inst->channels),
                       &tmp,
                       &(inst->sample_freq));

        return sbc_get_config(instance, conf_len);
    }

    return conf;
}

/* Print menu options to screen */
static void sbc_menu(void **instance,
                     CsrUint8 *num_options)
{
    sbc_instance_t *inst;
    inst = (sbc_instance_t*)*instance;

    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  0) SBC capabilities (0x%02X 0x%02X 0x%02X 0x%02X)\n",
           inst->sbc_capabilities[0],
           inst->sbc_capabilities[1],
           inst->sbc_capabilities[2],
           inst->sbc_capabilities[3]);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  1) Channel mode (%i)\n", inst->channel_mode);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  2) Allocation method (%i)\n", inst->alloc_method);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  3) PCM sample frequency (%i)\n", inst->sample_freq);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  4) PCM channels (%i)\n", inst->channels);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  4) SBC blocks (%i)\n", inst->blocks);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  5) SBC subbands (%i)\n", inst->subbands);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  6) Minimum bitpool (%i)\n", inst->minBitpool);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  7) Maximum bitpool (%i)\n", inst->maxBitpool);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  8) Maximum frame size (%i)\n", inst->maxFrameSize);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  9) PTS testcase \n");

    *num_options = 10;
}

/* Remote sink side buffer level update */
static void sbc_qos_update(void **instance,
                           CsrUint16 qosLevel)
{
    sbc_instance_t *inst;
    inst = (sbc_instance_t*)*instance;

    /* Disable dynamic bitpool adjustment for the moment */
    return;

    /* Adjust the bitpool according to buffer-level (which is between 0 and 9) */
    inst->bitPool += (4 - qosLevel);

    /* Make sure we do not exceed CSRMIN/CSRMAX bitpool */
    if(inst->bitPool > inst->maxBitpool)
    {
        inst->bitPool = inst->maxBitpool;
    }
    else if(inst->bitPool < inst->minBitpool)
    {
        inst->bitPool = inst->minBitpool;
    }
}


void sbc_set_pts_config(void **instance ,CsrUint32 case_no);
/* Set user option */
static CsrBool sbc_setup(void **instance,
                        CsrUint8 index,
                        char *value)
{
    sbc_instance_t *inst;
    CsrUint32 a;
    CsrUint32 b;
    CsrUint32 c;
    CsrUint32 d;
    int i;
    inst = (sbc_instance_t*)*instance;

    switch(index)
    {
        case 0:
            i = sscanf(value, "0x%02X 0x%02X 0x%02X 0x%02X",
                       &a, &b, &c, &d);
            inst->sbc_capabilities[0] = (CsrUint8)(a & 0x000000ff);
            inst->sbc_capabilities[1] = (CsrUint8)(b & 0x000000ff);
            inst->sbc_capabilities[2] = (CsrUint8)(c & 0x000000ff);
            inst->sbc_capabilities[3] = (CsrUint8)(d & 0x000000ff);
            break;

        case 1:
            i = sscanf(value, "%u", &a);
            inst->channel_mode = (CsrUint8)(a & 0x000000ff);
            break;

        case 2:
            i = sscanf(value, "%u", &a);
            inst->alloc_method = (CsrUint8)(a & 0x000000ff);
            break;

        case 3:
            i = sscanf(value, "%u", &a);
            inst->sample_freq = (CsrUint16)(a & 0x0000ffff);
            break;

        case 4:
            i = sscanf(value, "%u", &a);
            inst->blocks = (CsrUint8)(a & 0x000000ff);
            break;

        case 5:
            i = sscanf(value, "%u", &a);
            inst->subbands = (CsrUint8)(a & 0x000000ff);
            break;

        case 6:
            i = sscanf(value, "%u", &a);
            inst->minBitpool = (CsrUint8)(a & 0x000000ff);
            break;

        case 7:
            i = sscanf(value, "%u", &a);
            inst->maxBitpool = (CsrUint8)(a & 0x000000ff);
            break;

        case 8:
            i = sscanf(value, "%u", &a);
            inst->maxFrameSize = (CsrUint16)(a & 0x0000ffff);
            break;

        case 9:
            i = sscanf(value, "%u", &a);
            sbc_set_pts_config(instance, a);
            break;

        default:
            i = 0;
            break;
    }

    /* Update globals */
    setPcmSettings(inst->channels, 16, inst->sample_freq);
    setMaxFrameSize(inst->maxFrameSize);

    CsrPmemFree(value);

    if(i == 0)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Wrong selection or invalid format typed.\n");
    }

    return (i > 0);
}


void sbc_set_pts_config(void **instance, CsrUint32 case_no)
{
    sbc_instance_t *inst;
    CsrUint8 tmp = 0;
    inst = (sbc_instance_t*)*instance;

    switch(case_no)
    {
        case 1:
            inst->minBitpool        = 18;               /* Range 2-250 */
            inst->maxBitpool        = 18;               /* Range 2-250 */
            inst->bitPool           = 18;               /* Range 2-250 */
            inst->alloc_method      = CSR_SBC_METHOD_SNR;   /* CSR_SBC_METHOD_SNR, CSR_SBC_METHOD_LOUDNESS */
            inst->subbands          = 4;                /* 4, 8         "Number of Subbands */
            inst->blocks            = 16;               /* 4,8,12,16    "Block length" */
            inst->channel_mode      = CSR_SBC_MONO;         /* CSR_SBC_MONO, CSR_SBC_DUAL, CSR_SBC_STEREO, CSR_SBC_JOINT_STEREO */
            inst->sample_freq       = 48000;            /* 16000, 32000, 44100, 48000; */
            break;
        case 2:
            inst->minBitpool        = 16;
            inst->maxBitpool        = 16;
            inst->bitPool           = 16;
            inst->alloc_method      = CSR_SBC_METHOD_SNR;
            inst->subbands          = 4;
            inst->blocks            = 16;
            inst->channel_mode      = CSR_SBC_DUAL;
            inst->sample_freq       = 48000;
            break;
        case 3:
            inst->minBitpool        = 32;
            inst->maxBitpool        = 32;
            inst->bitPool           = 32;
            inst->alloc_method      = CSR_SBC_METHOD_LOUDNESS;
            inst->subbands          = 8;
            inst->blocks            = 8;
            inst->channel_mode      = CSR_SBC_MONO;
            inst->sample_freq       = 44100;
            break;
        case 4:
            inst->minBitpool        = 56;
            inst->maxBitpool        = 56;
            inst->bitPool           = 56;
            inst->alloc_method      = CSR_SBC_METHOD_LOUDNESS;
            inst->subbands          = 8;
            inst->blocks            = 8;
            inst->channel_mode      = CSR_SBC_JOINT_STEREO;
            inst->sample_freq       = 44100;
            break;
        case 5:
            inst->minBitpool        = 24;
            inst->maxBitpool        = 24;
            inst->bitPool           = 24;
            inst->alloc_method      = CSR_SBC_METHOD_SNR;
            inst->subbands          = 8;
            inst->blocks            = 4;
            inst->channel_mode      = CSR_SBC_MONO;
            inst->sample_freq       = 32000;
            break;
        case 6:
            inst->minBitpool        = 18;
            inst->maxBitpool        = 18;
            inst->bitPool           = 18;
            inst->alloc_method      = CSR_SBC_METHOD_SNR;
            inst->subbands          = 4;
            inst->blocks            = 16;
            inst->channel_mode      = CSR_SBC_MONO;
            inst->sample_freq       = 48000;
            break;
        case 7:
            inst->minBitpool        = 48;
            inst->maxBitpool        = 48;
            inst->bitPool           = 48;
            inst->alloc_method      = CSR_SBC_METHOD_SNR;
            inst->subbands          = 4;
            inst->blocks            = 16;
            inst->channel_mode      = CSR_SBC_MONO;
            inst->sample_freq       = 48000;
            break;
        case 8:
            inst->minBitpool        = 42;
            inst->maxBitpool        = 42;
            inst->bitPool           = 42;
            inst->alloc_method      = CSR_SBC_METHOD_SNR;
            inst->subbands          = 4;
            inst->blocks            = 16;
            inst->channel_mode      = CSR_SBC_MONO;
            inst->sample_freq       = 48000;
            break;
        case 9:
            inst->minBitpool        = 14;
            inst->maxBitpool        = 15;
            inst->bitPool           = 14;
            inst->alloc_method      = CSR_SBC_METHOD_LOUDNESS;
            inst->subbands          = 4;
            inst->blocks            = 16;
            inst->channel_mode      = CSR_SBC_MONO;
            inst->sample_freq       = 44100;
            break;
        case 10:
            inst->minBitpool        = 18;
            inst->maxBitpool        = 18;
            inst->bitPool           = 18;
            inst->alloc_method      = CSR_SBC_METHOD_SNR;
            inst->subbands          = 4;
            inst->blocks            = 16;
            inst->channel_mode      = CSR_SBC_MONO;
            inst->sample_freq       = 48000;
            break;
        case 11:
            inst->minBitpool        = 18;
            inst->maxBitpool        = 18;
            inst->bitPool           = 18;
            inst->alloc_method      = CSR_SBC_METHOD_SNR;
            inst->subbands          = 4;
            inst->blocks            = 16;
            inst->channel_mode      = CSR_SBC_MONO;
            inst->sample_freq       = 48000;
            break;
        case 12:
            inst->minBitpool  = 18;
            inst->maxBitpool  = 18;
            inst->bitPool           = 18;
            inst->alloc_method      = CSR_SBC_METHOD_SNR;
            inst->subbands          = 4;
            inst->blocks            = 16;
            inst->channel_mode      = CSR_SBC_MONO;
            inst->sample_freq       = 48000;
            break;
        case 13:
            inst->minBitpool  = 18;
            inst->maxBitpool  = 18;
            inst->bitPool           = 18;
            inst->alloc_method      = CSR_SBC_METHOD_SNR;
            inst->subbands          = 4;
            inst->blocks            = 16;
            inst->channel_mode      = CSR_SBC_MONO;
            inst->sample_freq       = 48000;
            break;
        case 14:
            inst->minBitpool  = 18;
            inst->maxBitpool  = 18;
            inst->bitPool           = 18;
            inst->alloc_method      = CSR_SBC_METHOD_SNR;
            inst->subbands          = 4;
            inst->blocks            = 16;
            inst->channel_mode      = CSR_SBC_MONO;
            inst->sample_freq       = 48000;
            break;
        case 15:
            inst->minBitpool  = 18;
            inst->maxBitpool  = 18;
            inst->bitPool           = 18;
            inst->alloc_method      = CSR_SBC_METHOD_SNR;
            inst->subbands          = 4;
            inst->blocks            = 16;
            inst->channel_mode      = CSR_SBC_MONO;
            inst->sample_freq       = 48000;
            break;
        case 16:
            inst->minBitpool  = 18;
            inst->maxBitpool  = 18;
            inst->bitPool           = 18;
            inst->alloc_method      = CSR_SBC_METHOD_SNR;
            inst->subbands          = 4;
            inst->blocks            = 16;
            inst->channel_mode      = CSR_SBC_MONO;
            inst->sample_freq       = 48000;
            break;
        case 17:
            inst->minBitpool  = 18;
            inst->maxBitpool  = 18;
            inst->bitPool           = 18;
            inst->alloc_method      = CSR_SBC_METHOD_SNR;
            inst->subbands          = 4;
            inst->blocks            = 16;
            inst->channel_mode      = CSR_SBC_MONO;
            inst->sample_freq       = 48000;
            break;
        case 18:
            inst->minBitpool  = 18;
            inst->maxBitpool  = 18;
            inst->bitPool           = 18;
            inst->alloc_method      = CSR_SBC_METHOD_SNR;
            inst->subbands          = 4;
            inst->blocks            = 16;
            inst->channel_mode      = CSR_SBC_MONO;
            inst->sample_freq       = 48000;
            break;
        case 19:
            inst->minBitpool  = 18;
            inst->maxBitpool  = 18;
            inst->bitPool           = 18;
            inst->alloc_method      = CSR_SBC_METHOD_SNR;
            inst->subbands          = 4;
            inst->blocks            = 16;
            inst->channel_mode      = CSR_SBC_MONO;
            inst->sample_freq       = 48000;
            break;
        case 20:
            inst->minBitpool  = 18;
            inst->maxBitpool  = 18;
            inst->bitPool           = 18;
            inst->alloc_method      = CSR_SBC_METHOD_SNR;
            inst->subbands          = 4;
            inst->blocks            = 16;
            inst->channel_mode      = CSR_SBC_MONO;
            inst->sample_freq       = 48000;
            break;
        case 21:
            inst->minBitpool  = 18;
            inst->maxBitpool  = 18;
            inst->bitPool           = 18;
            inst->alloc_method      = CSR_SBC_METHOD_SNR;
            inst->subbands          = 4;
            inst->blocks            = 16;
            inst->channel_mode      = CSR_SBC_MONO;
            inst->sample_freq       = 48000;
            break;
        case 22:
            inst->minBitpool  = 18;
            inst->maxBitpool  = 18;
            inst->bitPool           = 18;
            inst->alloc_method      = CSR_SBC_METHOD_SNR;
            inst->subbands          = 4;
            inst->blocks            = 16;
            inst->channel_mode      = CSR_SBC_MONO;
            inst->sample_freq       = 48000;
            break;
        case 23:
            inst->minBitpool  = 18;
            inst->maxBitpool  = 18;
            inst->bitPool           = 18;
            inst->alloc_method      = CSR_SBC_METHOD_SNR;
            inst->subbands          = 4;
            inst->blocks            = 16;
            inst->channel_mode      = CSR_SBC_MONO;
            inst->sample_freq       = 48000;
            break;
        case 24:
            inst->minBitpool  = 18;
            inst->maxBitpool  = 18;
            inst->bitPool           = 18;
            inst->alloc_method      = CSR_SBC_METHOD_SNR;
            inst->subbands          = 4;
            inst->blocks            = 16;
            inst->channel_mode      = CSR_SBC_MONO;
            inst->sample_freq       = 48000;
            break;
        case 25:
            inst->minBitpool  = 18;
            inst->maxBitpool  = 18;
            inst->bitPool           = 18;
            inst->alloc_method      = CSR_SBC_METHOD_SNR;
            inst->subbands          = 4;
            inst->blocks            = 16;
            inst->channel_mode      = CSR_SBC_MONO;
            inst->sample_freq       = 48000;
            break;
        case 26:
            inst->minBitpool  = 18;
            inst->maxBitpool  = 18;
            inst->bitPool           = 18;
            inst->alloc_method      = CSR_SBC_METHOD_SNR;
            inst->subbands          = 4;
            inst->blocks            = 16;
            inst->channel_mode      = CSR_SBC_MONO;
            inst->sample_freq       = 48000;
            break;
        case 27:
            inst->minBitpool  = 18;
            inst->maxBitpool  = 18;
            inst->bitPool           = 18;
            inst->alloc_method      = CSR_SBC_METHOD_SNR;
            inst->subbands          = 4;
            inst->blocks            = 16;
            inst->channel_mode      = CSR_SBC_MONO;
            inst->sample_freq       = 48000;
            break;
        case 28:
            inst->minBitpool  = 18;
            inst->maxBitpool  = 18;
            inst->bitPool           = 18;
            inst->alloc_method      = CSR_SBC_METHOD_SNR;
            inst->subbands          = 4;
            inst->blocks            = 16;
            inst->channel_mode      = CSR_SBC_MONO;
            inst->sample_freq       = 48000;
            break;
         default:
             CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Must be a number between 1 and 28/n");
             break;
    }

    if(inst->channel_mode != CSR_SBC_MONO)
    {
        inst->channels = 2;
    }
    else
    {
        inst->channels = 1;
    }


    inst->configure_pts[0] = CSR_BT_AV_SC_MEDIA_CODEC;
    inst->configure_pts[1] = SBC_MEDIA_CODEC_SC_SIZE - 2;
    inst->configure_pts[2] = CSR_BT_AV_AUDIO<<4;
    inst->configure_pts[3] = CSR_BT_AV_SBC;

    /* Sampling rate */
    switch(inst->sample_freq)
    {
        case 48000:
            tmp = 0x10;
            break;

        case 44100:
            tmp = 0x20;
            break;

        case 32000:
            tmp = 0x40;
            break;

        default:
            tmp = 0x80;
            break;
    }


    /* Channel mode */
    inst->configure_pts[4] = (8>>inst->channel_mode) | tmp;

    /* Allocation */
    tmp = (inst->alloc_method == CSR_SBC_METHOD_SNR) ? 0x02 : 0x01;

    /* Blocks, allocation and subbands */
    inst->configure_pts[5] = ((0x80>>((inst->blocks>>2)-1)) |
               (0x08>>((inst->subbands>>2)-1)) |
               tmp);

    /* Bitpools */
    inst->configure_pts[6] = inst->minBitpool;
    inst->configure_pts[7] = inst->maxBitpool;

    inst->sbc_capabilities[0] = inst->configure_pts[4];
    inst->sbc_capabilities[1] = inst->configure_pts[5];
    inst->sbc_capabilities[2] = inst->configure_pts[6];
    inst->sbc_capabilities[3] = inst->configure_pts[7];


    /* Store new PCM settings */
    setPcmSettings(inst->channels, 16, inst->sample_freq);
}

