/****************************************************************************

Copyright (c) 2009-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

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
#include "csr_bt_aac_parser.h"
#include "csr_app_lib.h"

/* Settings */
#define AAC_PRN            "AAC: "

/* #define DEBUG */

/* Special MPEG constants */
#define MPEG_AUDIO_MEDIA_CODEC_SC_SIZE 8

/* Filter entry prototypes */
static CsrBool aac_init(void **instance, av2filter_t *filter, av2instance_t *unused);
static CsrBool aac_deinit(void **instance);
static CsrBool aac_open(void **instance);
static CsrBool aac_close(void **instance);
static CsrBool aac_start(void **instance);
static CsrBool aac_stop(void **instance);
static CsrBool aac_process(CsrUint8 index,
                          av2filter_t **chain,
                          CsrBool freeData,
                          void *data,
                          CsrUint32 length);
static CsrUint8 *aac_get_config(void **instance,
                               CsrUint8 *conf_len);
static CsrBtAvResult aac_set_config(void **instance,
                                  CsrUint8 *conf,
                                  CsrUint8 conf_len);
static CsrUint8 *aac_get_caps(void **instance,
                             CsrUint8 *cap_len);
static CsrUint8 *aac_remote_caps(void **instance,
                                CsrUint8 *caps,
                                CsrUint8 caps_len,
                                CsrUint8 *conf_len);
static void aac_qos_update(void **instance,
                           CsrUint16 qosLevel);
static void aac_menu(void **instance,
                     CsrUint8 *num_options);
static CsrBool aac_setup(void **instance,
                        CsrUint8 index,
                        char *value);
static char* aac_return_filename(void **instance);


/* Filter structure setup for aac streamer */
av2filter_t filter_aac =
{
    "csr_aac_streamer",
    FILTER_PRODUCER | FILTER_ENCODER | FILTER_PASS,
    CSR_BT_AV_AUDIO,
    CSR_BT_AV_MPEG12_AUDIO,
    0,
    NULL,
    aac_init,
    aac_deinit,
    aac_open,
    aac_close,
    aac_start,
    aac_stop,
    aac_process,
    aac_get_config,
    aac_set_config,
    aac_get_caps,
    aac_remote_caps,
    aac_qos_update,
    aac_menu,
    aac_setup,
    aac_return_filename
};

/* Filter instance data structure */
typedef struct
{
    char           *name;
    FILE           *file;
    CsrUint32        file_length;
    CsrUint8         channels;
    int             sampling_rate;
    av2filter_t    *filter;
    CsrUint16        maxFrameSize;
    int             framebits;
    CsrTime            delay;
    CsrTime            duration;
    CsrUint8        *frame_data;
    CsrUint16        frame_length;
} aac_instance_t;


/* Initialise aac filter */
static CsrBool aac_init(void **instance, struct av2filter_t *filter, av2instance_t *av2inst)
{
    aac_instance_t *inst;
    *instance = CsrPmemZalloc(sizeof(aac_instance_t));
    inst = (aac_instance_t*)*instance;

    /* Set default filename */
    inst->name = CsrStrDup(av2inst->aac_filename_in);

    /* Set other defaults */
    inst->delay = 0;
    inst->maxFrameSize = getMaxFrameSize();

    return TRUE;
}

/* Deinitialise aac filter */
static CsrBool aac_deinit(void **instance)
{
    aac_instance_t *inst;
    inst = (aac_instance_t*)*instance;

    /*we need to deinit the parser*/
    AACDeInitParser();


    /* Free instance */
    if(inst->file)
    {
        inst->file = NULL;
    }
    if(inst->name)
    {
        CsrPmemFree(inst->name);
        inst->name = NULL;
    }
    /*deinit the AAC parser*/

    CsrPmemFree(inst);
    *instance = NULL;

    return TRUE;
}

/* Open aac with current configuration */
static CsrBool aac_open(void **instance)
{
    aac_instance_t *inst;
    CsrBool result;


    inst = (aac_instance_t*)*instance;

    /* init the AAC Parser */
    result = AACInitParser(inst->name);


    if( result )
    {
        /*We need to peek at the header so we actually read the first frame*/
        inst->frame_length = AACGetLatmPacket((void **)&inst->frame_data,
            inst->maxFrameSize, &inst->duration);


        /* Get default values */
        inst->channels      = AACGetChannels();
        inst->sampling_rate = AACGetSampleRate();
        inst->delay         = 0;/* could be inst->duration; */
    }

    return result;
}

/* Close aac configuration */
static CsrBool aac_close(void **instance)
{
    return TRUE;
}

/* Start aac streaming */
static CsrBool aac_start(void **instance)
{
    aac_instance_t *inst;
    inst = (aac_instance_t*)*instance;

#ifdef DEBUG
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,AAC_PRN "usec timer: %i\n", inst->delay);
#endif
    filtersStartTimer(inst->delay);

    /* Not required */
    return TRUE;
}

/* Stop aac streaming */
static CsrBool aac_stop(void **instance)
{
    filtersStopTimer();

    /* Not required */
    return TRUE;
}

/* Process aac data: Empty windows buffers */
static CsrBool aac_process(CsrUint8 index,
                          av2filter_t **chain,
                          CsrBool freeData,
                          void *data,
                          CsrUint32 length)
{
    aac_instance_t *inst;
    int             max_payload_size;
    CsrUint8        *packet_ptr;
    CsrUint8        *payload_ptr;
    CsrBool          result;

    inst             = (aac_instance_t*)(chain[index]->f_instance);
    result           = TRUE;
    max_payload_size = inst->maxFrameSize - CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE;

    packet_ptr       = CsrPmemZalloc(max_payload_size+ CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE);
    payload_ptr      = packet_ptr + CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE;

    /* set new delay */
    if(inst->duration != inst->delay){
        inst->delay = inst->duration;
        filtersStartTimer(inst->delay);
    }

    while(inst->frame_length > max_payload_size)
    {
        /* we need to fragment the packet */
        CsrMemCpy(payload_ptr, inst->frame_data, max_payload_size);
        result &= chain[index+1]->f_process((CsrUint8)(index+1),
                                            chain,
                                            FALSE,
                                            packet_ptr,
                                            max_payload_size + CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE);
#ifdef DEBUG
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,AAC_PRN "Sending new LATM frame fragment. Length: %d, duration: %d usec\n",inst->frame_length, inst->duration);
#endif
        packet_ptr          = CsrPmemZalloc(inst->frame_length - max_payload_size + CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE);
        payload_ptr         = packet_ptr + CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE;
        inst->frame_length -= max_payload_size;
        inst->frame_data   += inst->frame_length;

    }
    /* The payload data is already in our instance data */
    /* we expect the payload of an aac frame to be smaller than the RTP packet */
    CsrMemCpy(payload_ptr, inst->frame_data, inst->frame_length);

    /* Send final data on to next stage */
    result &= chain[index+1]->f_process((CsrUint8)(index+1),
                                        chain,
                                        TRUE,
                                        packet_ptr,
                                        inst->frame_length + CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE);
#ifdef DEBUG
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,AAC_PRN "Sending new LATM final frame fragment. Length: %d, duration: %d usec\n",inst->frame_length, inst->duration);
#endif
    /*we get a packet for the next roundtrip */
    inst->frame_length = AACGetLatmPacket((void **)&inst->frame_data,
        (CsrUint16)(inst->maxFrameSize - CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE),
        &inst->duration);


    /* All done for this AAC frame */
    if (!inst->frame_length)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,AAC_PRN "could not read next frame, stopping\n");
        filtersStopTimer();
        return FALSE;
    }
    else
    {

#ifdef DEBUG
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,AAC_PRN "usec timer: %i\n", inst->delay);
#endif
    }
    return result;
}

/* Return aac configuration */
static CsrUint8 *aac_get_config(void **instance,
                               CsrUint8 *conf_len)
{
    CsrUint8 *conf;
    aac_instance_t *inst;
    inst = (aac_instance_t*)*instance;

#ifdef DEBUG
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,AAC_PRN "get_config\n");
#endif

    /* If no file is opened, try reading the current and close it again */
    if(inst->frame_data == NULL)
    {
        if(aac_open(instance))
        {
            aac_close(instance);
        }
        else
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,AAC_PRN "AAC file not valid -- can't send back configuration\n");
            *conf_len = 0;
            return NULL;
        }
    }

#ifdef DEBUG
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,AAC_PRN "samplerate index: %i\n", AACGetSampleRate());
#endif

    *conf_len = MPEG_AUDIO_MEDIA_CODEC_SC_SIZE+2;
    conf = CsrPmemAlloc(*conf_len);

    conf[0] = CSR_BT_AV_SC_MEDIA_CODEC;
    conf[1] = MPEG_AUDIO_MEDIA_CODEC_SC_SIZE ;
    conf[2] = CSR_BT_AV_AUDIO << 4;
    conf[3] = CSR_BT_AV_MPEG24_AAC;
    switch(AACGetObjectType()) /* can be profile+1 for ADST*/
    {
        case 0: /*special case for MPEG2*/
        case 2:
            conf[4] = 0x40; /* AAC LC */
            break;
        case 4:
            conf[4] = 0x20; /* AAC LTP */
            break;
        case 6:
            conf[4] = 0x10; /* AAC Scalable */
            break;
    }
    conf[5] = 0;
    conf[6] = 0;
    switch(AACGetSampleRate())
    {
        case 8000:  conf[5] = 0x80; break;
        case 11025: conf[5] = 0x40; break;
        case 12000: conf[5] = 0x20; break;
        case 16000: conf[5] = 0x10; break;
        case 22050: conf[5] = 0x08; break;
        case 24000: conf[5] = 0x04; break;
        case 32000: conf[5] = 0x02; break;
        case 44100: conf[5] = 0x01; break;
        case 48000: conf[6] = 0x80; break;
        case 64000: conf[6] = 0x40; break;
        case 88200: conf[6] = 0x20; break;
        case 96000: conf[6] = 0x10; break;
    }

    if(inst->channels==1){
        conf[6] |= 0x08;
    }
    else /* 2 channels */
    {
        conf[6] |= 0x04;
    }
    /*VBR=true*/
    conf[7]=0x80;

    /* bitrate */
    conf[7] |= 0;
    conf[8] = 0;  /* bitrate set to 0 = unknown */
    conf[9] = 0;
    return conf;
}

/* Set aac configuration */
static CsrBtAvResult aac_set_config(void **instance,
                                  CsrUint8 *conf,
                                  CsrUint8 conf_len)
{
    /* We do not support this feature */
    return CSR_BT_AV_ACCEPT;
}

/* Return aac service capabilities */
static CsrUint8 *aac_get_caps(void **instance,
                             CsrUint8 *cap_len)
{
    /* Not supported */
    *cap_len = 0;
    return NULL;
}

/* Investigate remote capabilities, return optimal configuration */
static CsrUint8 *aac_remote_caps(void **instance,
                                CsrUint8 *caps,
                                CsrUint8 caps_len,
                                CsrUint8 *conf_len)
{
    CsrUint8 *conf;
    aac_instance_t *inst;
    inst = (aac_instance_t*)*instance;

#ifdef DEBUG
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,AAC_PRN "remote_caps\n");
#endif



   /* If no file is opened, try reading the current and close it again */
    if(inst->frame_data == NULL)
    {
        if(aac_open(instance))
        {
            aac_close(instance);
        }
        else
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,AAC_PRN "AAC file not valid -- can't send back configuration\n");
            *conf_len = 0;
            return NULL;
        }
    }
    /* Dummy check that service capabilities are AAC */
    if(!(*(caps+2)>>4 == CSR_BT_AV_AUDIO) ||
       !(*(caps+3) == CSR_BT_AV_MPEG24_AAC))
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,AAC_PRN "format either not audio or AAC\n");
        *conf_len = 0;
        return NULL;
    }

#ifdef DEBUG
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,AAC_PRN "samplerate index: %i\n", AACGetSampleRate());
#endif

    *conf_len = MPEG_AUDIO_MEDIA_CODEC_SC_SIZE+2;
    conf = CsrPmemAlloc(*conf_len);

    conf[0] = CSR_BT_AV_SC_MEDIA_CODEC;
    conf[1] = MPEG_AUDIO_MEDIA_CODEC_SC_SIZE ;
    conf[2] = CSR_BT_AV_AUDIO << 4;
    conf[3] = CSR_BT_AV_MPEG24_AAC;
    switch(AACGetObjectType()) /* can be profile+1 for ADST*/
    {
        case 0: /*special case for MPEG2*/
        case 2:
            conf[4] = 0x40; /* AAC LC */
            break;
        case 4:
            conf[4] = 0x20; /* AAC LTP */
            break;
        case 6:
            conf[4] = 0x10 ; /* AAC Scalable */
            break;
    }
    conf[5] = 0;
    conf[6] = 0;
    switch(AACGetSampleRate())
    {
        case 8000:  conf[5] = 0x80; break;
        case 11025: conf[5] = 0x40; break;
        case 12000: conf[5] = 0x20; break;
        case 16000: conf[5] = 0x10; break;
        case 22050: conf[5] = 0x08; break;
        case 24000: conf[5] = 0x04; break;
        case 32000: conf[5] = 0x02; break;
        case 44100: conf[5] = 0x01; break;
        case 48000: conf[6] = 0x80; break;
        case 64000: conf[6] = 0x40; break;
        case 88200: conf[6] = 0x20; break;
        case 96000: conf[6] = 0x10; break;
    }

    if(inst->channels==1){
        conf[6] |= 0x08;
    }
    else /* 2 channels */
    {
        conf[6] |= 0x04;
    }
    /*VBR=true*/
    conf[7]=0x80;

    /* bitrate */
    conf[7] |= 0;
    conf[8] = 0;  /* bitrate set to 0 = unknown */
    conf[9] = 0;
    return conf;


}

/* Remote sink side buffer level update */
static void aac_qos_update(void **instance,
                           CsrUint16 qosLevel)
{
    /* Not supported */
}

/* Print menu options to screen */
static void aac_menu(void **instance,
                     CsrUint8 *num_options)
{
    aac_instance_t *inst = *instance;
    CsrCharString *str;

    str = ((inst->name != NULL)
           ? inst->name
           : "n/a");

    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  0) Change filename (%s)\n", str);
    *num_options = 1;
}

/* Set user option */
static CsrBool aac_setup(void **instance,
                        CsrUint8 index,
                        char *value)
{
    aac_instance_t *inst;
    int i;
    inst = (aac_instance_t*)*instance;

    switch(index)
    {
        case 0:
            {
                CsrPmemFree(inst->name);
                inst->name = value;
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
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Wrong selection or invalid format typed.\n");

    }

    return (i > 0);
}

static char* aac_return_filename(void **instance)
{
    aac_instance_t *inst = NULL;
    inst = (aac_instance_t *)*instance;

    return inst->name;
}
