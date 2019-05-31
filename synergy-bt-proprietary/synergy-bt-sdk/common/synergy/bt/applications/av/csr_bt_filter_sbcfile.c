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
#include "csr_bt_av_app_util.h"
#include "csr_bt_file.h"

/* Settings */
#define SBCFILE_PRN                  "filter_sbcfile: "

#define SBC_MEDIA_PAYLOAD_TYPE       96
#define SBC_DEFAULT_BITPOOL          52
#define SBC_DEFAULT_MAX_BITPOOL      53
#define SBC_DEFAULT_MIN_BITPOOL      24

/* If set larger than 1, it can give some problems with audio drops with some
   headsets (Samsung SBH 500) when streaming from a BDB3. The reason is the
   resolution of the timer on BDB3 which is 10ms */
#define SBC_NOF_BURSTS                1

/*
#define DEBUG                         1
#define DEBUG_VERBOSE                 1
*/

/* SBC codec definitions */
#define SC_HDR_SIZE                   2
#define SC_LENGTH_INDEX               1
#define SBC_MEDIA_CODEC_SC_SIZE       8
#define SBC_MEDIA_CODEC_FREQ_MASK     0xF0

/* Instance data for this filter */
typedef struct
{
    av2filter_t      *filter;
    CsrUint16          sample_freq;
    CsrUint8           blocks;
    CsrUint8           channels;
    CsrSbcChannelMode  channel_mode;
    CsrSbcAllocMethod  alloc_method;
    void             *sbcHdl;
    CsrUint8           subbands;
    CsrUint8           bitpool;
    CsrUint32          timestamp;
    char             *filename;
    FILE             *file;
    CsrTime              delay;
    CsrTime              lasttime;
    char             *buffer;
    CsrUint16          framesize;
    CsrUint16          fpp;           /* frames per l2cap packet */
} sbcfile_instance_t;

/* Filter entry prototypes */
static CsrBool sbcfile_init_streamer(void **instance, av2filter_t *filter, av2instance_t *unused);
static CsrBool sbcfile_init_writer(void **instance, av2filter_t *filter, av2instance_t *unused);
static CsrBool sbcfile_deinit(void **instance);
static CsrBool sbcfile_open(void **instance);
static CsrBool sbcfile_close(void **instance);
static CsrBool sbcfile_start(void **instance);
static CsrBool sbcfile_stop(void **instance);
static CsrBool sbcfile_process_streamer(CsrUint8 index,
                                       av2filter_t **chain,
                                       CsrBool freeData,
                                       void *data,
                                       CsrUint32 length);
static CsrBool sbcfile_process_writer(CsrUint8 index,
                                     av2filter_t **chain,
                                     CsrBool freeData,
                                     void *data,
                                     CsrUint32 length);
static CsrUint8 *sbcfile_get_config(void **instance,
                                   CsrUint8 *conf_len);
static CsrBtAvResult sbcfile_set_config(void **instance,
                                      CsrUint8 *conf,
                                      CsrUint8 conf_len);
static CsrUint8 *sbcfile_get_caps(void **instance,
                                 CsrUint8 *cap_len);
static CsrUint8 *sbcfile_remote_caps(void **instance,
                                    CsrUint8 *caps,
                                    CsrUint8 caps_len,
                                    CsrUint8 *conf_len);
static void sbcfile_qos_update(void **instance,
                               CsrUint16 qosLevel);
static void sbcfile_menu(void **instance,
                         CsrUint8 *num_options);
static CsrBool sbcfile_setup(void **instance,
                            CsrUint8 index,
                            char *value);
static char* sbcfile_return_filename(void **instance);


/* Filter structure setup for SBC encoder */
struct av2filter_t filter_sbcfile_streamer =
{
    "csr_sbcfile_streamer",
    FILTER_PRODUCER | FILTER_ENCODER | FILTER_PASS,
    CSR_BT_AV_AUDIO,
    CSR_BT_AV_SBC,
    0,
    NULL,
    sbcfile_init_streamer,
    sbcfile_deinit,
    sbcfile_open,
    sbcfile_close,
    sbcfile_start,
    sbcfile_stop,
    sbcfile_process_streamer,
    sbcfile_get_config,
    sbcfile_set_config,
    sbcfile_get_caps,
    sbcfile_remote_caps,
    sbcfile_qos_update,
    sbcfile_menu,
    sbcfile_setup,
    sbcfile_return_filename
};

/* Filter structure setup for SBC decoder */
av2filter_t filter_sbcfile_writer =
{
    "csr_sbcfile_writer",
    FILTER_CONSUMER | FILTER_DECODER | FILTER_PASS,
    CSR_BT_AV_AUDIO,
    CSR_BT_AV_SBC,
    0,
    NULL,
    sbcfile_init_writer,
    sbcfile_deinit,
    sbcfile_open,
    sbcfile_close,
    sbcfile_start,
    sbcfile_stop,
    sbcfile_process_writer,
    sbcfile_get_config,
    sbcfile_set_config,
    sbcfile_get_caps,
    sbcfile_remote_caps,
    sbcfile_qos_update,
    sbcfile_menu,
    sbcfile_setup,
    sbcfile_return_filename
};

/* HCI/RFC platform variable, set in common/hci/csr_bt_rfc_main.c */
extern CsrUint16 targetBitrate;

/* Common initialiser */
static void sbcfile_common_init(sbcfile_instance_t *inst)
{
    CsrMemSet(inst, 0, sizeof(sbcfile_instance_t));
    inst->sample_freq  = 44100;
    inst->blocks       = CSR_SBC_MAX_BLOCKS;
    inst->channel_mode = CSR_SBC_STEREO;
    inst->alloc_method = CSR_SBC_METHOD_SNR;
    inst->subbands     = CSR_SBC_MAX_SUBBANDS;
    inst->bitpool      = SBC_DEFAULT_BITPOOL;
    inst->framesize    = CSR_SBC_MAX_FRAME_SIZE_BYTES;
    inst->timestamp    = 0;

    inst->sbcHdl = CsrSbcOpen();
}

/* Local function to calculate settings given SBC specific codec config */
static void sbcfile_store_config(sbcfile_instance_t *inst)
{
    /* Sample frequency */
    switch(CsrSbcGetSampleFreq(inst->sbcHdl))
    {
        case 0x0:
            inst->sample_freq = 16000;
            break;

        case 0x1:
            inst->sample_freq = 32000;
            break;

        case 0x2:
            inst->sample_freq = 44100;
            break;

        case 0x3:
            inst->sample_freq = 48000;
            break;
        default:
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "Wrong Sample Frequency extracted from SBC-Header\n");
            break;
    }

    /* Channel mode */
    switch(CsrSbcGetChannelMode(inst->sbcHdl))
    {
        case 0x3:
            inst->channel_mode = CSR_SBC_JOINT_STEREO;
            break;

        case 0x2:
            inst->channel_mode = CSR_SBC_STEREO;
            break;

        case 0x1:
            inst->channel_mode = CSR_SBC_DUAL;
            break;

        case 0x0:
            inst->channel_mode = CSR_SBC_MONO;
            break;
        default:
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "Wrong Channel Mode extracted from SBC-Header\n");
            break;
    }

    if(inst->channel_mode == CSR_SBC_MONO)
    {
        inst->channels = 1;
    }
    else
    {
        inst->channels = 2;
    }

    /* Block and subbands */
    inst->blocks   = CsrSbcGetNumBlocks(inst->sbcHdl);
    inst->subbands = CsrSbcGetNumSubBands(inst->sbcHdl);
    inst->bitpool  = CsrSbcGetBitPool(inst->sbcHdl);

    /* Allocation method */
    if(CsrSbcGetAllocMethod(inst->sbcHdl)== 0)
    {
        inst->alloc_method = CSR_SBC_METHOD_LOUDNESS;
    }
    else
    {
        inst->alloc_method = CSR_SBC_METHOD_SNR;
    }
}


/* Read SBC configuration of current frame */
#define CSR_BT_AV_SBC_FRAME_HEADER_SIZE 13 /* Including the scale factors for dual channel, 8 subbands - worst case
                                              Warning: If 4 subbands, single channel is used, and payload becomes less than
                                                       6 bytes, we will fall back to using fSeek, which is bad on bdb3. */
static CsrBool sbcfile_read_frame(sbcfile_instance_t *inst)
{
    CsrInt32 rd;
    CsrInt32 res;

    if(inst->file == NULL)
    {
        return FALSE;
    }

    /* Read from file */
    CsrMemSet(inst->buffer, 0, CSR_SBC_MAX_FRAME_SIZE_BYTES);
    rd = CsrBtFread(inst->buffer,
                  1,
                  CSR_BT_AV_SBC_FRAME_HEADER_SIZE,
                  inst->file);
    if(rd <= 0)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "read_frame: could not read from file (%s)\n", inst->filename);
        return FALSE;
    }

    /* Read SBC header (get framesize) */
    inst->framesize = CsrSbcReadHeader(inst->sbcHdl, (CsrUint8 *) inst->buffer);
    if(inst->framesize == 0)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "read_frame: invalid SBC header\n");
        return FALSE;
    }
#ifdef DEBUG_VERBOSE
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "read_frame: SBC frame size %i\n", inst->framesize);
#endif

    /* Set position of file to match next header-start */
    if(rd > inst->framesize)
    {
        /* THIS SHOULD NEVER HAPPEN. */
        res = CsrBtFseek(inst->file,
                        (-1 * (rd - inst->framesize)),
                        CSR_SEEK_CUR);
        CSR_UNUSED(res);
#ifdef DEBUG_VERBOSE
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "seek backwards by %i, result %i\n", (-1 * (rd - inst->framesize)), res);
#endif
    }
    else if(rd < inst->framesize)
    {
        rd = CsrBtFread(inst->buffer+CSR_BT_AV_SBC_FRAME_HEADER_SIZE,
                      1,
                      inst->framesize-CSR_BT_AV_SBC_FRAME_HEADER_SIZE,
                      inst->file);
        if((rd+CSR_BT_AV_SBC_FRAME_HEADER_SIZE) != inst->framesize)
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "read error, framesize %i, read %i\n", inst->framesize, rd);
            return FALSE;
        }
    }

    /* Update our local info of SBC */
    sbcfile_store_config(inst);
    return TRUE;
}

/* Initialise SBC filter */
static CsrBool sbcfile_init_streamer(void **instance, struct av2filter_t *filter, av2instance_t *av2inst)
{
    sbcfile_instance_t *inst;
    CsrInt32             i;

    *instance = CsrPmemAlloc(sizeof(sbcfile_instance_t));
    inst = (sbcfile_instance_t*)*instance;
    sbcfile_common_init(inst);

    inst->filter = filter;

    /* Set default filename */
    i = strlen(av2inst->sbc_filename_in) + 1;
    inst->filename = CsrPmemAlloc(i);
    CsrMemSet(inst->filename, 0, i);
    CsrStrLCpy(inst->filename, av2inst->sbc_filename_in, i);

    inst->framesize = CSR_SBC_MAX_FRAME_SIZE_BYTES;
    inst->buffer = CsrPmemAlloc(inst->framesize);

    return TRUE;
}

/* Initialise SBC filter */
static CsrBool sbcfile_init_writer(void **instance, struct av2filter_t *filter, av2instance_t *av2inst)
{
    sbcfile_instance_t *inst;
    CsrInt32             i;

    *instance = CsrPmemAlloc(sizeof(sbcfile_instance_t));
    inst = (sbcfile_instance_t*)*instance;
    sbcfile_common_init(inst);

    inst->filter = filter;

    /* Set default filename */
    i = strlen(av2inst->sbc_filename_out) + 1;
    inst->filename = CsrPmemAlloc(i);
    CsrMemSet(inst->filename, 0, i);
    CsrStrLCpy(inst->filename, av2inst->sbc_filename_out, i);

    return TRUE;
}

/* Deinitialise SBC filter */
static CsrBool sbcfile_deinit(void **instance)
{
    sbcfile_instance_t *inst;

    inst = (sbcfile_instance_t*)*instance;

    if(inst)
    {
        CsrSbcClose(&inst->sbcHdl);
        CsrPmemFree(inst->buffer);
        CsrPmemFree(inst->filename);
        if(inst->file != NULL)
        {
            CsrBtFclose(inst->file);
        }
    }

    /* Free instance */
    CsrPmemFree(*instance);
    *instance = NULL;

    return TRUE;
}

/* Open SBC with current configuration */
static CsrBool sbcfile_open(void **instance)
{
    sbcfile_instance_t *inst;
    CsrBool              result;

    inst = (sbcfile_instance_t*)*instance;

#ifdef DEBUG_VERBOSE
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "open\n");
#endif

    result = FALSE;
    if(inst->filename != NULL)
    {
        /* Open file for either read or write */
        if(inst->filter->f_type & FILTER_PRODUCER)
        {
            inst->file = CsrBtFopen(inst->filename, "rb");
        }
        else
        {
            inst->file = CsrBtFopen(inst->filename, "wb");
        }

        if(inst->file != NULL)
        {
            result = TRUE;
        }
        else
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "could not open file '%s'\n", inst->filename);
        }
    }

    if(result && (inst->filter->f_type & FILTER_PRODUCER))
    {
        /* Read SBC config from file, set position to zero */
        result = FALSE;
        if(sbcfile_read_frame(inst))
        {
            if(CsrBtFseek(inst->file, 0, CSR_SEEK_SET) == 0)
            {
#ifdef DEBUG
                char ch_mode      = '?';
                char alloc_method = '?';

                if(inst->channel_mode == CSR_SBC_JOINT_STEREO)   {ch_mode = 'J';}
                if(inst->channel_mode == CSR_SBC_STEREO)         {ch_mode = 'S';}
                if(inst->channel_mode == CSR_SBC_DUAL)           {ch_mode = 'D';}
                if(inst->channel_mode == CSR_SBC_MONO)           {ch_mode = 'M';}
                if(inst->alloc_method == CSR_SBC_METHOD_LOUDNESS){alloc_method = 'L';}
                if(inst->alloc_method == CSR_SBC_METHOD_SNR)     {alloc_method = 'S';}
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"\n");
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "subbands..............%i (0x%x)\n", inst->subbands,          inst->subbands);
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "blocks................%i (0x%x)\n", inst->blocks,            inst->blocks);
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "sample_freq...........%i (0x%x)\n", inst->sample_freq,       inst->sample_freq);
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "chan_mode.............%i (0x%x) (%c)\n", inst->channel_mode, inst->channel_mode, ch_mode);
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "alloc_method..........%i (0x%x) (%c)\n", inst->alloc_method, inst->alloc_method, alloc_method);
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "bitPool...............%i (0x%x)\n", inst->bitpool,           inst->bitpool);
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "channels..............%i (0x%x)\n", inst->channels,          inst->channels);
#endif
                result = TRUE;
            }
        }

        if(!result)
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "error reading file SBC header\n");
        }
    }

    /* Reset timer etc. */
    if(inst->framesize == 0)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "warning: unknown SBC framesize\n");
        inst->framesize = CSR_SBC_MAX_FRAME_SIZE_BYTES;
    }
    inst->fpp   = (getMaxFrameSize() - CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE - 1) / inst->framesize;
    inst->delay = (inst->fpp * inst->subbands * inst->blocks * CSR_SCHED_SECOND) / (inst->sample_freq);
    inst->delay = inst->delay * SBC_NOF_BURSTS;
    setMediaPayloadType(SBC_MEDIA_PAYLOAD_TYPE);

    /* Store new PCM settings */
    setPcmSettings(inst->channels, 16, inst->sample_freq);

    return result;
}

/* Close SBC configuration */
static CsrBool sbcfile_close(void **instance)
{
    sbcfile_instance_t *inst;

    inst = (sbcfile_instance_t*)*instance;

#ifdef DEBUG_VERBOSE
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "close\n");
#endif

    if(inst->file)
    {
        CsrBtFclose(inst->file);
        inst->file = NULL;
    }

    /* SBC does not require any special close handling */
    return TRUE;
}

/* Start SBC streaming */
static CsrBool sbcfile_start(void **instance)
{
    sbcfile_instance_t *inst;

    inst = (sbcfile_instance_t*)*instance;

#ifdef DEBUG_VERBOSE
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "start\n");
#endif

    /* Start processing loop */
    if(inst->filter->f_type & FILTER_PRODUCER)
    {
        filtersStartTimer(inst->delay);
    }

    return TRUE;
}

/* Stop SBC streaming */
static CsrBool sbcfile_stop(void **instance)
{
    sbcfile_instance_t *inst;

    inst = (sbcfile_instance_t*)*instance;

#ifdef DEBUG_VERBOSE
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "stop\n");
#endif

    /* Stop processing loop */
    if(inst->filter->f_type & FILTER_PRODUCER)
    {
        filtersStopTimer();
    }
    return TRUE;
}

/* Process SBC data: Encoder */
static CsrBool sbcfile_process_streamer(CsrUint8       index,
                                       av2filter_t **chain,
                                       CsrBool        freeData,
                                       void         *data,
                                       CsrUint32      length)
{
    sbcfile_instance_t *inst;
    CsrUint16            packet_size;
    CsrUint16            packet_idx;
    char               *packet;
    CsrBool              result;
    CsrUint8             frames;
    CsrUint32            nofBursts;

    for (nofBursts=SBC_NOF_BURSTS; nofBursts>0; nofBursts--)
    {
        inst   = (sbcfile_instance_t*)(chain[index]->f_instance);
        result = TRUE;

#ifdef DEBUG_VERBOSE
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "process_streamer\n");
#endif
        packet_size = getMaxFrameSize();
        packet      = CsrPmemAlloc(packet_size);
        packet_idx  = CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE + 1;
        frames      = 0;

        while(((packet_idx + inst->framesize) < packet_size) && sbcfile_read_frame(inst))
        {
            frames++;
            CsrMemCpy(packet + packet_idx, inst->buffer, inst->framesize);
            packet_idx = packet_idx + inst->framesize;
        }
        packet[CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE] = frames;

        if(frames > 0)
        {
            void *send;
            CsrTime  newtime;

            send = CsrPmemAlloc(packet_idx);
            CsrMemCpy(send, packet, packet_idx);
            CsrPmemFree(packet);

#ifdef DEBUG_VERBOSE
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "sending %i frames in packet\n", frames);
#endif
            result &= chain[index+1]->f_process((CsrUint8)(index+1),
                                                chain,
                                                TRUE,
                                                send,
                                                packet_idx);

            if (nofBursts == SBC_NOF_BURSTS)
            {
                /* Update timestamp */
                inst->timestamp += frames * inst->blocks * inst->subbands;
                setTimestamp(inst->timestamp);

                /* Correct timing */
                if(inst->framesize == 0)
                {
                    inst->framesize = CSR_SBC_MAX_FRAME_SIZE_BYTES;
                }
                inst->fpp = (getMaxFrameSize() - CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE - 1) / inst->framesize;
                newtime = (inst->fpp * inst->subbands * inst->blocks * CSR_SCHED_SECOND) / inst->sample_freq;
                newtime = newtime * SBC_NOF_BURSTS;

                if(inst->delay != newtime)
                {
                    inst->delay = newtime;
                    filtersStartTimer(inst->delay);
#ifdef DEBUG
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "adjust new packet delay to %u usec\n", inst->delay);
#endif
                }
            }
        }
        else
        {
            sbcfile_stop((void *)&inst);
        }
    }

    /* We shouldn't receive any data */
    if(freeData && data)
    {
        CsrPmemFree(data);
    }

    return result;
}

/* Process SBC data: Decoder */
static CsrBool sbcfile_process_writer(CsrUint8 index,
                                     av2filter_t **chain,
                                     CsrBool freeData,
                                     void *data,
                                     CsrUint32 length)
{
    sbcfile_instance_t *inst;
    CsrUint32            wr;
    CsrUint32            paysize;

    inst = (sbcfile_instance_t*)(chain[index]->f_instance);

#ifdef DEBUG_VERBOSe
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "process_writer\n");
#endif

    /* Write data to file */
    paysize = CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE+1;
    if(length > paysize)
    {
        wr = CsrBtFwrite((char*)data + (CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE + 1),
                       1,
                       (length - paysize),
                       inst->file);
        if(wr == 0)
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "file write error, code %i\n",
                   ferror(inst->file));
        }
        else if(wr != (length - paysize))
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "%i bytes received, but only %i written\n",
                   length, wr);
        }
    }

#ifdef DEBUG_VERBOSE
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "writing %i bytes...\n", length);
#endif

    /* Pass on data in the chain */
    return chain[index+1]->f_process((CsrUint8)(index+1),
                                     chain,
                                     freeData,
                                     data,
                                     length);
}

/* Return SBC configuration */
static CsrUint8 *sbcfile_get_config(void   **instance,
                                   CsrUint8 *conf_len)
{
    CsrUint8             tmp;
    CsrUint8            *conf;
    sbcfile_instance_t *inst;

    inst = (sbcfile_instance_t*)*instance;

#ifdef DEBUG_VERBOSE
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,SBCFILE_PRN "get_config\n");
#endif

    *conf_len = SBC_MEDIA_CODEC_SC_SIZE;
    conf      = CsrPmemAlloc(*conf_len);

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
    conf[6] = SBC_DEFAULT_MIN_BITPOOL;
    conf[7] = SBC_DEFAULT_MAX_BITPOOL;

    return conf;
}

/* Set SBC configuration */
static CsrBtAvResult sbcfile_set_config(void **instance,
                                      CsrUint8 *conf,
                                      CsrUint8 conf_len)
{
    /* Not support - always accept */
    return CSR_BT_AV_ACCEPT;
}

/* Return SBC service capabilities */
static CsrUint8 *sbcfile_get_caps(void   **instance,
                                 CsrUint8 *cap_len)
{
    CsrUint8            *cap;

    *cap_len = SBC_MEDIA_CODEC_SC_SIZE + 2;
    cap = CsrPmemAlloc(*cap_len);

    /* Support all settings */
    cap[0] = CSR_BT_AV_SC_MEDIA_TRANSPORT;
    cap[1] = 0;
    cap[2] = CSR_BT_AV_SC_MEDIA_CODEC;
    cap[3] = SBC_MEDIA_CODEC_SC_SIZE - 2;
    cap[4] = CSR_BT_AV_AUDIO<<4;
    cap[5] = CSR_BT_AV_SBC;
    cap[6] = 0xFF;
    cap[7] = 0xFF;
    cap[8] = 0x02;
    cap[9] = 0xFA;

    return cap;
}

/* Investigate remote capabilities, return optimal configuration */
static CsrUint8 *sbcfile_remote_caps(void    **instance,
                                    CsrUint8  *caps,
                                    CsrUint8   caps_len,
                                    CsrUint8  *conf_len)
{
    CsrUint8 *conf;

    /* Dummy check that service capabilities are SBC */
    if(!(*(caps+2)>>4 == CSR_BT_AV_AUDIO) || !(*(caps+3) == CSR_BT_AV_SBC))
    {
        *conf_len = 0;
        return NULL;
    }

    /* Fetch the current config based on file content */
    conf = sbcfile_get_config(instance, conf_len);

    /* check sample freq is supported by remote device
       - note all other parameters are mandatory for sink */
    if( !(*(conf+4) & *(caps+4) & SBC_MEDIA_CODEC_FREQ_MASK) )
    {
        CsrPmemFree(conf);
        *conf_len = 0;
        return NULL;
    }

    /* use the min/max bitpool from remote device */
    *(conf+6) = *(caps+6);
    *(conf+7) = *(caps+7);

    return conf;
}

/* Remote sink side buffer level update */
static void sbcfile_qos_update(void **instance,
                               CsrUint16 qosLevel)
{
    /* Not supported */
}

/* Print menu options to screen */
static void sbcfile_menu(void   **instance,
                         CsrUint8 *num_options)
{
    sbcfile_instance_t *inst;

    inst = (sbcfile_instance_t*)*instance;

    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  0) Change filename (%s)\n", inst->filename);
    *num_options = 1;
}

/* Set user option */
static CsrBool sbcfile_setup(void   **instance,
                            CsrUint8  index,
                            char    *value)
{
    int                 i;
    sbcfile_instance_t *inst;

    inst = (sbcfile_instance_t*)*instance;

    switch(index)
    {
        case 0:
            {
                CsrPmemFree(inst->filename);
                inst->filename = value;
                value = NULL;
                i = 1;
            }
            break;

        default:
            i = 0;
            break;
    }

    CsrPmemFree(value);
    if(i == 0)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"It is not possible to configure this setting for the sbc-file filter\n");


    }

    return (i > 0);
}

static char* sbcfile_return_filename(void **instance)
{

    return NULL;
}

