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
#include "csr_app_lib.h"
#include "csr_bt_file.h"

/* Settings */
#define MP3_PRN            "mp3: "

/* #define DEBUG */

/* Special MPEG constants */
#define MPEG_AUDIO_MEDIA_CODEC_SC_SIZE 8

/* Filter entry prototypes */
static CsrBool mp3_init(void **instance, av2filter_t *filter, av2instance_t *unused);
static CsrBool mp3_deinit(void **instance);
static CsrBool mp3_open(void **instance);
static CsrBool mp3_close(void **instance);
static CsrBool mp3_start(void **instance);
static CsrBool mp3_stop(void **instance);
static CsrBool mp3_process(CsrUint8 index,
                          av2filter_t **chain,
                          CsrBool freeData,
                          void *data,
                          CsrUint32 length);
static CsrUint8 *mp3_get_config(void **instance,
                               CsrUint8 *conf_len);
static CsrBtAvResult mp3_set_config(void **instance,
                                  CsrUint8 *conf,
                                  CsrUint8 conf_len);
static CsrUint8 *mp3_get_caps(void **instance,
                             CsrUint8 *cap_len);
static CsrUint8 *mp3_remote_caps(void **instance,
                                CsrUint8 *caps,
                                CsrUint8 caps_len,
                                CsrUint8 *conf_len);
static void mp3_qos_update(void **instance,
                           CsrUint16 qosLevel);
static void mp3_menu(void **instance,
                     CsrUint8 *num_options);
static CsrBool mp3_setup(void **instance,
                        CsrUint8 index,
                        char *value);
static char* mp3_return_filename(void **instance);


/* Filter structure setup for mp3 streamer */
av2filter_t filter_mp3 =
{
    "csr_mp3_streamer",
    FILTER_PRODUCER | FILTER_ENCODER | FILTER_PASS,
    CSR_BT_AV_AUDIO,
    CSR_BT_AV_MPEG12_AUDIO,
    0,
    NULL,
    mp3_init,
    mp3_deinit,
    mp3_open,
    mp3_close,
    mp3_start,
    mp3_stop,
    mp3_process,
    mp3_get_config,
    mp3_set_config,
    mp3_get_caps,
    mp3_remote_caps,
    mp3_qos_update,
    mp3_menu,
    mp3_setup,
    mp3_return_filename
};


/* Distance in bytes to search for mpeg sync */
#define MPEG_SYNCRANGE (8 * 1024)

#define MPEG_HDRSZ 4
typedef struct {
    /* header fields */
    CsrUint16 sync;
    CsrUint8 ver_id;
    CsrUint8 layer_id;
    CsrUint8 has_crc;
    CsrUint8 bitrate_id;
    CsrUint8 freq_id;
    CsrUint8 padding;
    CsrUint8 private;
    CsrUint8 chan_mode;
    CsrUint8 js_mode_ext;
    CsrUint8 copyrighted;
    CsrUint8 original;
    CsrUint8 emph_id;

    /* raw data */
    unsigned char hdr[MPEG_HDRSZ]; /* Raw header */
    CsrUint8 *frame;

    /* av timer delay */
    CsrUint32 av_delay;
} mpegframe_t;

/* Filter instance */
typedef struct
{
    char           *name;
    FILE           *file;
    av2filter_t    *filter;
    mpegframe_t    frame;
    CsrUint16       maxFrameSize;
    CsrTime           delay;
} mp3_instance_t;

int mpeg_samplerates[] = {
    44100, 48000, 32000, -1
};

int mpeg_bitrates[] = {
    0, /* Free */
    32000,
    40000,
    48000,
    56000,
    64000,
    80000,
    96000,
    112000,
    128000,
    160000,
    192000,
    224000,
    256000,
    320000,
    -1 /* Bad */
};


CsrUint8
getbit(unsigned char word[4], unsigned char bit)
{
    CsrUint8 byteno, bitno, bitval;

    bitno = bit % 8;
    bitval = 1 << bitno;

    byteno = bit / 8;

    return (word[byteno] & bitval) ? 1 : 0;
}

CsrUint32
getbits(unsigned char word[4], unsigned char from, unsigned char to)
{
    CsrUint32 rval;
    CsrUint8 i;

    for (rval = 0, i = from; i < to + 1; i++) {
        rval |= getbit(word, i) << (i - from);
    }

    return rval;
}

int mpeg_framelen(mpegframe_t *h)
{
    int bitrate;
    int samplerate;

    bitrate = mpeg_bitrates[h->bitrate_id];
    samplerate = mpeg_samplerates[h->freq_id];

    return 144 * bitrate / samplerate + h->padding +
        (h->has_crc ? 0 : sizeof(CsrUint16));

}

int mpeg_findsync(FILE *f, CsrUint16 searchlen)
{
    CsrUint8 found;
    unsigned char byte[2];

    byte[0] = 0;
    byte[1] = 0;

    while (searchlen)
    {
        size_t rval;

        if (byte[0] == 0xff)
        {
            rval = fread(&byte[1], sizeof(byte[1]), 1, f);
        }
        else
        {
            rval = fread(&byte, sizeof(byte[0]), 2, f);
        }

        if (rval < 1)
        {
            /* Read error or eof; doesn't matter: quit. */
            break;
        }
        else if (byte[0] == 0xff)
        {
            if (((byte[1] & 0xf0) >> 4) == 0xf)
            {
                found = 1;
                break;
            }

        }

        if (searchlen <= rval)
        {
            /* Search range reached, give up. */
            found = 0;
            break;
        }
        else
        {
            searchlen -= rval;
            byte[0] = byte[1];
        }
    }

    if (found)
    {
        /*
         * If we've found the sync, we have
         * scanned two bytes past it:  Rewind.
         */
        if (fseek(f, -2, SEEK_CUR) != 0)
        {
            /*
             * .. but an error happened when seeking
             * back to the sync, so we can't really
             * say we've found it, because the caller
             * is supposed to read a valid header if
             * we return a nonzero value.
             */
            found = 0;
        }
    }

    return found;
}

int mpeg_readheader(FILE *f, mpegframe_t *h)
{
    mpegframe_t h2;
    size_t rval;
    CsrUint8 tmp[MPEG_HDRSZ];

    rval = fread(&h2.hdr, sizeof(h2.hdr), 1, f);
    if (rval == 1)
    {

        /* Swap bytes so bits are in order */
        tmp[0] = h2.hdr[3];
        tmp[1] = h2.hdr[2];
        tmp[2] = h2.hdr[1];
        tmp[3] = h2.hdr[0];

        h2.sync = (CsrUint16)getbits(tmp, 20, 31);

        if (h2.sync == 0xfff)
        {
            h2.ver_id = (CsrUint8)getbits(tmp, 19, 19);
            h2.layer_id = (CsrUint8)getbits(tmp, 17, 18);
            h2.has_crc = (CsrUint8)getbits(tmp, 16, 16);
            h2.bitrate_id = (CsrUint8)getbits(tmp, 12, 15);
            h2.freq_id = (CsrUint8)getbits(tmp, 10, 11);
            h2.padding = (CsrUint8)getbits(tmp, 9, 9);
            h2.private = (CsrUint8)getbits(tmp, 8, 8);
            h2.chan_mode = (CsrUint8)getbits(tmp, 6, 7);
            h2.js_mode_ext = (CsrUint8)getbits(tmp, 4, 5);
            h2.copyrighted = (CsrUint8)getbits(tmp, 3, 3);
            h2.original = (CsrUint8)getbits(tmp, 2, 2);
            h2.emph_id = (CsrUint8)getbits(tmp, 0, 1);

            /*
             * multiply by 8 to get framelen in bits, by
             * 10^6 ((.. * 1000) / (.. / 1000)) to get
             * usec.  * 10^6 is split to avoid int overflow.
             */
            h2.av_delay = (mpeg_framelen(&h2) * 8 * 1000)
                / (mpeg_bitrates[h2.bitrate_id] / 1000);

            CsrMemCpy(h, &h2, sizeof(*h));
        }
        else
        {
            rval = 0;
        }
    }

    return rval;
}

int mpeg_readframe(FILE *f, mpegframe_t *h)
{
    CsrUint8 *buf;
    CsrUint32 len;
    size_t rval;

    len = mpeg_framelen(h);

    buf = CsrPmemAlloc(len);
    rval = fread(buf + MPEG_HDRSZ, len - MPEG_HDRSZ, 1, f);
    if (rval != 1)
    {
        CsrPmemFree(buf);

        return 0;
    }

    CsrMemCpy(buf, h->hdr, MPEG_HDRSZ);
    h->frame = buf;

    return 1;
}

/* Initialise mp3 filter */
static CsrBool mp3_init(void **instance, struct av2filter_t *filter, av2instance_t *av2inst)
{
    mp3_instance_t *inst;
    inst = CsrPmemZalloc(sizeof(*inst));
    *instance = inst;

    /* Set default filename */
    inst->name = CsrStrDup(av2inst->mp3_filename_in);

    /* Set other defaults */
    inst->delay = 0;
    inst->maxFrameSize = getMaxFrameSize();

    return TRUE;
}

/* Deinitialise mp3 filter */
static CsrBool mp3_deinit(void **instance)
{
    mp3_instance_t *inst;
    inst = (mp3_instance_t*)*instance;

    /* Free instance */
    if(inst->file)
    {
        CsrBtFclose(inst->file);
        inst->file = NULL;
    }
    if(inst->name)
    {
        CsrPmemFree(inst->name);
        inst->name = NULL;
    }
    CsrPmemFree(inst);
    *instance = NULL;

    return TRUE;
}

/* Open mp3 with current configuration */
static CsrBool mp3_open(void **instance)
{
    mp3_instance_t *inst;
    CsrBool result;
    inst = (mp3_instance_t*)*instance;

    result = FALSE;
    if((inst->name != NULL) && *(inst->name) != 0)
    {
        inst->file = CsrBtFopen(inst->name, "rb");
        if(inst->file != NULL)
        {
            result = TRUE;
        }
        else
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,MP3_PRN "file could not be opened (%s)\n", inst->name);
        }
    }

    /* Check file */
    if(result == TRUE)
    {
        if (!mpeg_readheader(inst->file, &inst->frame))
        {
            if (mpeg_findsync(inst->file, MPEG_SYNCRANGE) &&
                mpeg_readheader(inst->file, &inst->frame))
            {
                result = TRUE;
            }
            else
            {
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,MP3_PRN "file is not a valid MP3 file (%s)\n", inst->name);
                result = FALSE;
            }
        }
        else
        {
            result = TRUE;
        }

        if (result == TRUE)
        {
            setPcmSettings((CsrUint8)((inst->frame.chan_mode == 3) ? 1 : 2),
                           (CsrUint8)16,
                           (CsrUint16)mpeg_samplerates[inst->frame.freq_id]);

            mpeg_readframe(inst->file, &inst->frame);
        }
    }

    return result;
}

/* Close mp3 configuration */
static CsrBool mp3_close(void **instance)
{
    mp3_instance_t *inst;
    inst = (mp3_instance_t*)*instance;

    if(inst->file)
    {
        CsrBtFclose(inst->file);
        inst->file = NULL;
    }

    return TRUE;
}

/* Start mp3 streaming */
static CsrBool mp3_start(void **instance)
{
    mp3_instance_t *inst;
    inst = (mp3_instance_t*)*instance;

    /* Start periodic timer */
    inst->delay = inst->frame.av_delay;
#ifdef DEBUG
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,MP3_PRN "timer: %luus\n", inst->delay);
#endif
    filtersStartTimer(inst->delay);

    /* Not required */
    return TRUE;
}

/* Stop mp3 streaming */
static CsrBool mp3_stop(void **instance)
{
    filtersStopTimer();

    /* Not required */
    return TRUE;
}

/* Process mp3 data: Empty windows buffers */
static CsrBool mp3_process(CsrUint8 index,
                          av2filter_t **chain,
                          CsrBool freeData,
                          void *data,
                          CsrUint32 length)
{
    mp3_instance_t *inst;
    CsrBool result;
    inst = (mp3_instance_t*)(chain[index]->f_instance);
    result = TRUE;

    if (!mpeg_readheader(inst->file, &inst->frame))
    {
        if (!mpeg_findsync(inst->file, MPEG_SYNCRANGE))
        {
            result = FALSE;
        }
        else
        {
            mpeg_readheader(inst->file, &inst->frame);
            result = TRUE;
        }
    }

    if (result == TRUE)
    {
        CsrUint32 current_index;
        CsrUint32 frame_size;

        current_index = 0;

        mpeg_readframe(inst->file, &inst->frame);
        frame_size = mpeg_framelen(&inst->frame);

        while(frame_size > 0)
        {
            CsrUint32 consume;
            CsrUint8 *packet_ptr, *payload_ptr;

            /* How much can the packet hold? */
            if (frame_size + CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE + 4 > inst->maxFrameSize)
            {
                consume = inst->maxFrameSize - CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE - 4;
            }
            else
            {
                consume = frame_size;
            }

            packet_ptr = CsrPmemAlloc(consume + CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE + 4);
            payload_ptr = packet_ptr + CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE;
            *payload_ptr++ = 0;
            *payload_ptr++ = 0;
            *payload_ptr++ = (CsrUint8)(current_index >> 8);
            *payload_ptr++ = (CsrUint8)(current_index & 0xFF);

            CsrMemCpy(payload_ptr,
                   inst->frame.frame + current_index,
                   consume);

            /* Send data on to next stage */
            result &= chain[index+1]->f_process((CsrUint8)(index+1),
                                                chain,
                                                TRUE,
                                                packet_ptr,
                                                consume + 4 + CSR_BT_AV_FIXED_MEDIA_PACKET_HDR_SIZE);

            current_index += consume;
            frame_size -= consume;
        }

        /* All done for this MP3 frame */
        return result;
    }
    else
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,MP3_PRN "could not read next frame, stopping\n");
        filtersStopTimer();
        return FALSE;
    }
}

/* Return mp3 configuration */
static CsrUint8 *mp3_get_config(void **instance,
                               CsrUint8 *conf_len)
{
    CsrUint8 chan_mode;
    CsrUint16 br_index;
    CsrUint8 *conf;
    mp3_instance_t *inst;
    inst = (mp3_instance_t*)*instance;

#ifdef DEBUG
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,MP3_PRN "get_config\n");
#endif

    /* If no file is opened, try reading the current and close it again */
    if(inst->file == NULL)
    {
        if(mp3_open(instance))
        {
            mp3_close(instance);
        }
        else
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,MP3_PRN "MP3 file not valid -- can't send back configuration\n");
            *conf_len = 0;
            return NULL;
        }
    }

#ifdef DEBUG
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,MP3_PRN "bitrate index: %i\n", inst->frame.bitrate_index);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,MP3_PRN "samplerate index: %i\n", inst->frame.samplerfindex);
#endif

    *conf_len = MPEG_AUDIO_MEDIA_CODEC_SC_SIZE;
    conf = CsrPmemAlloc(*conf_len);

    conf[0] = CSR_BT_AV_SC_MEDIA_CODEC;
    conf[1] = MPEG_AUDIO_MEDIA_CODEC_SC_SIZE - 2;
    conf[2] = CSR_BT_AV_AUDIO << 4;
    conf[3] = CSR_BT_AV_MPEG12_AUDIO;

    /* Channel mode */
    switch(inst->frame.chan_mode)
    {
        case 1:
            /* Joint stereo */
            chan_mode = 1;
            break;

        case 2:
            /* Dual channel */
            chan_mode = 4;
            break;

        case 3:
            /* Mono */
            chan_mode = 8;
            break;

        default:
            /* Stereo */
            chan_mode = 2;
            break;
    }

    /* Byte 5 */
    conf[4] = 0x20 | (inst->frame.has_crc << 4) | chan_mode;

    /* Sample rate */
    switch(mpeg_samplerates[inst->frame.freq_id])
    {
        case 16000:
            conf[5] = 0x20;
            break;

        case 22050:
            conf[5] = 0x10;
            break;

        case 24000:
            conf[5] = 0x08;
            break;

        case 32000:
            conf[5] = 0x04;
            break;

        case 44100:
            conf[5] = 0x02;
            break;

        case 48000:
            conf[5] = 0x01;
            break;
    }

    /* Bitrate */
    br_index = 0x01 << inst->frame.bitrate_id;
    conf[6] = (CsrUint8) (br_index >> 8);
    conf[7] = (CsrUint8) (br_index & 0xFF);

    return conf;
}

/* Set mp3 configuration */
static CsrBtAvResult mp3_set_config(void **instance,
                                  CsrUint8 *conf,
                                  CsrUint8 conf_len)
{
    /* We do not support this feature */
    return CSR_BT_AV_ACCEPT;
}

/* Return mp3 service capabilities */
static CsrUint8 *mp3_get_caps(void **instance,
                             CsrUint8 *cap_len)
{
    /* Not supported */
    *cap_len = 0;
    return NULL;
}

/* Investigate remote capabilities, return optimal configuration */
static CsrUint8 *mp3_remote_caps(void **instance,
                                CsrUint8 *caps,
                                CsrUint8 caps_len,
                                CsrUint8 *conf_len)
{
    CsrUint8 chan_mode;
    CsrUint16 br_index;
    CsrUint8 *conf;
    mp3_instance_t *inst;
    inst = (mp3_instance_t*)*instance;

#ifdef DEBUG
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,MP3_PRN "remote_caps\n");
#endif

    /* If no file is opened, try reading the current and close it again */
    if(inst->file == NULL)
    {
        if(mp3_open(instance))
        {
            mp3_close(instance);
        }
        else
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,MP3_PRN "MP3 file not valid -- can't send back configuration\n");
            *conf_len = 0;
            return NULL;
        }
    }

    /* Dummy check that service capabilities are MP3 */
    if(!(*(caps+2)>>4 == CSR_BT_AV_AUDIO) ||
       !(*(caps+3) == CSR_BT_AV_MPEG12_AUDIO))
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,MP3_PRN "format either not audio or MPEG\n");
        *conf_len = 0;
        return NULL;
    }

    /* Alloc config */
    *conf_len = MPEG_AUDIO_MEDIA_CODEC_SC_SIZE;
    conf = CsrPmemAlloc(*conf_len);

    /* Build service capabilities (media and if supported content protection) */
    conf[0] = CSR_BT_AV_SC_MEDIA_CODEC;
    conf[1] = MPEG_AUDIO_MEDIA_CODEC_SC_SIZE - 2;
    conf[2] = CSR_BT_AV_AUDIO<<4;
    conf[3] = CSR_BT_AV_MPEG12_AUDIO;

    /* Channel mode */
    switch(inst->frame.chan_mode)
    {
        case 1:
            /* Joint stereo */
            chan_mode = 1;
            break;

        case 2:
            /* Dual channel */
            chan_mode = 4;
            break;

        case 3:
            /* Mono */
            chan_mode = 8;
            break;

        default:
            /* Stereo */
            chan_mode = 2;
            break;
    }

    /* If there is a mismatch between what is supported by peer and
     * what the mp3 file contains, - take the best supported channel
     * mode */
    if(!(chan_mode & *(caps+4)) )
    {
        CsrUint8 i;

        for( i=0; i<4; i++)
        {
            if( (1<<i) & *(caps+4) )
            {
                chan_mode = 1<<i;
                break;
            }
        }
    }
    conf[4] = 0x20 | (inst->frame.has_crc << 4) | chan_mode;

    /* Sample rate */
    switch(mpeg_samplerates[inst->frame.freq_id])
    {
        case 16000:
            conf[5] = 0x20;
            break;

        case 22050:
            conf[5] = 0x10;
            break;

        case 24000:
            conf[5] = 0x08;
            break;

        case 32000:
            conf[5] = 0x04;
            break;

        case 44100:
            conf[5] = 0x02;
            break;

        case 48000:
            conf[5] = 0x01;
            break;
    }

    /* Bitrate */
    br_index = 0x01 << inst->frame.bitrate_id;
    conf[6] = (CsrUint8) (br_index >> 8);
    conf[7] = (CsrUint8) (br_index & 0xFF);

    return conf;
}

/* Remote sink side buffer level update */
static void mp3_qos_update(void **instance,
                           CsrUint16 qosLevel)
{
    /* Not supported */
}

/* Print menu options to screen */
static void mp3_menu(void **instance,
                     CsrUint8 *num_options)
{
    mp3_instance_t *inst = *instance;
    CsrCharString *str;

    str = ((inst->name != NULL)
           ? inst->name
           : "n/a");

    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  0) Change filename (%s)\n", str);
    *num_options = 1;
}

/* Set user option */
static CsrBool mp3_setup(void **instance,
                        CsrUint8 index,
                        char *value)
{
    mp3_instance_t *inst;
    int i;
    inst = (mp3_instance_t*)*instance;

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

static char* mp3_return_filename(void **instance)
{
    mp3_instance_t *inst = NULL;
    inst = (mp3_instance_t*)*instance;

    return inst->name;
}
