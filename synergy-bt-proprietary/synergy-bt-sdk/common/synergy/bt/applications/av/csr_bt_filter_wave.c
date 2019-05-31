/****************************************************************************

Copyright (c) 2009-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "csr_sched.h"
#include "csr_pmem.h"
#include "csr_bt_util.h"
#include "csr_bt_av_prim.h"
#include "csr_bt_av_lib.h"
#include "csr_bt_cm_lib.h"
#include "csr_bt_sc_lib.h"
#include "csr_bt_platform.h"
#include "csr_bt_av2.h"
#include "csr_bt_av2_filter.h"
#include "csr_bt_platform.h"
#include "csr_app_lib.h"
#include "csr_bt_file.h"
#include "csr_bt_av2.h"


/* Various constants */
#define WAVE_PRN             "filter_wave: "
#define HEADER_BUFFER        64                    /* Size of buffer to use for header-reading */
#define TIMER_FREQ           20                    /* How often to run timer (Hz) */
#define OUTPUT_ALIGN         512                   /* Byte-alignment of output to make SBC happy */
#define READ_SIZE            65536                 /* Size of buffer to use for streaming */

/*
#define DEBUG                1
#define DEBUG_VERBOSE        1
*/

/* Wave file header */
typedef struct
{
    char        chunkId[5];                        /* Chunk format "RIFF", which size should be '5' instead of '4'. */
    CsrUint32    chunk_length;                      /* Full chunk size*/
    char        txt1[9];                           /* Format "WAVE" and SubChunk1ID "fmt", which size should be '9' instead of '8'. */
    CsrUint32    format_len;                        /* SubChunk1Size, 0x10 for wave */
    CsrUint16    audio_format;                      /* AudioFormat, always 1 */
    CsrUint16    channel_numbers;                   /* NumChannels */
    CsrUint32    sample_freq;                       /* SampleRate */
    CsrUint32    bytes_per_sec;                     /* ByteRate */
    CsrUint16    bytes_per_sample;                  /* BlockAlign */
    CsrUint16    bits_per_sample;                   /* BitsPerSample */
    char        txt2[5];                           /* SubChunk2ID -> Letters "data", which size should be '5' instead of '4'. */
    CsrUint32    data_length;                       /* SubChunk2Size */
} wave_header_t;

/* Instance data for wave filter */
typedef struct
{
    av2filter_t      *filter;
    FILE             *file;
    char             *name;
    CsrUint32         length;
    CsrUint32         position;
    CsrUint8          channels;
    CsrUint16         sample_rate;
    CsrUint16         byte_rate;
    CsrUint8          align;
    CsrUint8          bits;
    CsrUint32         framesize;
    CsrUint32         delay;
    void             *buffer;
    CsrUint32         carry;
    CsrTime           lasttime;
    CsrTime           carryTime;
} wave_instance_t;

/* Filter entry prototypes */
static CsrBool wave_stream_init(void **instance,
                               av2filter_t *filter,
                               av2instance_t *CSR_UNUSED);
static CsrBool wave_write_init(void **instance,
                              av2filter_t *filter,
                              av2instance_t *CSR_UNUSED);
static CsrBool wave_deinit(void **instance);
static CsrBool wave_open(void **instance);
static CsrBool wave_close(void **instance);
static CsrBool wave_start(void **instance);
static CsrBool wave_stop(void **instance);
static CsrBool wave_stream_process(CsrUint8 index,
                                  av2filter_t **chain,
                                  CsrBool freeData,
                                  void *data,
                                  CsrUint32 length);
static CsrBool wave_write_process(CsrUint8 index,
                                 av2filter_t **chain,
                                 CsrBool freeData,
                                 void *data,
                                 CsrUint32 length);
static CsrUint8 *wave_get_config(void **instance,
                                CsrUint8 *conf_len);
static CsrBtAvResult wave_set_config(void **instance,
                                   CsrUint8 *conf,
                                   CsrUint8 conf_len);
static CsrUint8 *wave_get_caps(void **instance,
                              CsrUint8 *cap_len);
static CsrUint8 *wave_remote_caps(void **instance,
                                 CsrUint8 *caps,
                                 CsrUint8 caps_len,
                                 CsrUint8 *conf_len);
static void wave_qos_update(void **instance,
                            CsrUint16 qosLevel);
static void wave_menu(void **instance,
                      CsrUint8 *num_options);
static CsrBool wave_setup(void **instance,
                         CsrUint8 index,
                         char *value);
static char* wave_return_filename(void **instance);


/* Filter structure setup for wave decoder */
av2filter_t filter_stream_wave =
{
    "csr_wave_streamer",
    FILTER_PRODUCER,
    AV_NO_MEDIA,
    AV_NO_CODEC,
    0,
    NULL,
    wave_stream_init,
    wave_deinit,
    wave_open,
    wave_close,
    wave_start,
    wave_stop,
    wave_stream_process,
    wave_get_config,
    wave_set_config,
    wave_get_caps,
    wave_remote_caps,
    wave_qos_update,
    wave_menu,
    wave_setup,
    wave_return_filename
};

/* Filter structure setup for wave decoder */
av2filter_t filter_write_wave =
{
    "csr_wave_writer",
    FILTER_PASS,
    AV_NO_MEDIA,
    AV_NO_CODEC,
    0,
    NULL,
    wave_write_init,
    wave_deinit,
    wave_open,
    wave_close,
    wave_start,
    wave_stop,
    wave_write_process,
    wave_get_config,
    wave_set_config,
    wave_get_caps,
    wave_remote_caps,
    wave_qos_update,
    wave_menu,
    wave_setup,
    wave_return_filename
};


/* Write wave header to file */
static void wave_save_header(wave_instance_t *inst)
{
    CsrUint32 fpos;
    CsrUint32 res;
    wave_header_t head;

    /* Riff header
     * chunk_length = sizeof(wave_header_t) - sizeof(riff_header = 36 */
    CsrStrLCpy(head.chunkId, "RIFF", sizeof(head.chunkId));
    head.chunk_length = 36 + inst->position;

    /* Content header */
    CsrStrLCpy(head.txt1, "WAVEfmt ", sizeof(head.txt1));
    head.format_len = 0x10;
    head.audio_format = 1;
    head.channel_numbers = inst->channels;
    head.sample_freq = inst->sample_rate;
    head.bits_per_sample = 16;
    head.bytes_per_sec = (CsrUint32)(inst->sample_rate * inst->channels * inst->bits/8);
    head.bytes_per_sample = (CsrUint16) (inst->channels * inst->bits/8);
    CsrStrLCpy(head.txt2, "data", sizeof(head.txt2));
    head.data_length = inst->position;

    /* Write header to file, and jump back to
     * current position */
    fpos = CsrBtFtell(inst->file);
    res = CsrBtFseek(inst->file, 0, CSR_SEEK_SET);
    if(res != 0)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "could not seek to 'header position' in file\n");
        return;
    }

    res = CsrBtFwrite(&head, 1, sizeof(wave_header_t), inst->file);
    if(res != sizeof(wave_header_t))
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "error writing wave header (size:%i result:%i)\n",
               sizeof(wave_header_t), res);
        return;
    }

    res = CsrBtFseek(inst->file, fpos, CSR_SEEK_SET);
    if(res != 0)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "could not seek to 'append position' in file\n");
        return;
    }

#ifdef DEBUG
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "header jump position..%i\n", fpos);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "total size is.........0x%08x\n", inst->length);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "wave contents is......0x%08x\n", inst->position);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "channels..............%i\n", inst->channels);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "sample_freq...........%i\n", inst->sample_rate);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "bits_per_sample.......%i\n", head.bits_per_sample);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "bytes_per_sec.........%i\n", head.bytes_per_sec);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "bytes_per_sample......%i\n", head.bytes_per_sample);
#endif
}

/* Parse wave header file. File position will be set to the first sample, so
 * data can be read directly */
static CsrBool wave_parse_header(wave_instance_t *inst)
{
    CsrInt32 read;
    CsrUint8 buffer[HEADER_BUFFER];

    /* Dummy */
    if(inst->file == NULL)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "file not opened\n");
        return FALSE;
    }

    /* Read RIFF, fmt and data headers */
    CsrMemSet(buffer, 0, HEADER_BUFFER);
    read = CsrBtFread(buffer, 1, 44, inst->file);
    if(read != 44)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "error reading file, code %i\n", read);
        return FALSE;
    }

    /* Check for RIFF identifier */
    if((buffer[0] != 'R') || (buffer[1] != 'I') || (buffer[2] != 'F') || (buffer[3] != 'F'))
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "wavefile does not contain the RIFF identifier\n");
        return FALSE;
    }

    /* Read total file size */
    inst->length = 8 + buffer[4] + (buffer[5]<<8) + (buffer[6]<<16) + (buffer[7]<<24);
    if(inst->length < 44)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "wavefile length is less than RIFF+fmt+data header size, %i\n", inst->length);
        return FALSE;
    }

    /* Read WAVE and 'fmt ' identifiers */
    if((buffer[ 8] != 'W') || (buffer[ 9] != 'A') || (buffer[10] != 'V') || (buffer[11] != 'E') ||
       (buffer[12] != 'f') || (buffer[13] != 'm') || (buffer[14] != 't') || (buffer[15] != ' '))
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "wavefile does not contain the WAVE and/or fmt identifiers\n");
        return FALSE;
    }

    /* Check that samples are PCM */
    if((buffer[20] != 1) || (buffer[21] != 0))
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "wavefile is not PCM format\n");
        return FALSE;
    }

    /* Store format fields */
    inst->channels = buffer[22] + (buffer[23]<<8);
    inst->sample_rate = buffer[24] + (buffer[25]<<8) + (buffer[26]<<16) + (buffer[27]<<24);
    inst->byte_rate = buffer[28] + (buffer[29]<<8) + (buffer[30]<<16) + (buffer[31]<<24);
    inst->align = buffer[32] + (buffer[33]<<8);
    inst->bits = buffer[34] + buffer[35];
    inst->length = buffer[40] + (buffer[41]<<8) + (buffer[42]<<16) + (buffer[43]<<24);

    inst->framesize = (inst->sample_rate * inst->align);
    inst->framesize = inst->framesize / TIMER_FREQ;
    inst->position = 0;

#ifdef DEBUG
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "channels %i\n", inst->channels);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "sample_rate %i\n", inst->sample_rate);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "align %i\n", inst->align);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "bits %i\n", inst->bits);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "byte_rate %i\n", inst->byte_rate);

    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "data size: %i\n", inst->length);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "framesize: %i\n", inst->framesize);
#endif

    if(inst->bits != 16)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "warning: bits per sample is not 16\n");
    }
    if(inst->channels != 2)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "warning: not two-channel (stereo) audio\n");
    }

    return TRUE;
}

/* Initialise wave filter for streamer */
static CsrBool wave_stream_init(void **instance, struct av2filter_t *filter, av2instance_t *av2inst)
{
    wave_instance_t *inst;

    *instance = CsrPmemZalloc(sizeof(wave_instance_t));
    inst = (wave_instance_t*)*instance;

    /* Set default filename */
    inst->name = CsrStrDup(av2inst->wav_filename_in);

    inst->buffer = CsrPmemAlloc(READ_SIZE);
    inst->filter = filter;

    return TRUE;
}

/* Initialise wave filter for writer */
static CsrBool wave_write_init(void **instance, struct av2filter_t *filter, av2instance_t *av2inst)
{
    wave_instance_t *inst;

    *instance = CsrPmemZalloc(sizeof(wave_instance_t));
    inst = (wave_instance_t*)*instance;

    inst->name = CsrStrDup(av2inst->wav_filename_out);

    inst->filter = filter;

    return TRUE;
}

/* Deinitialise wave filter */
static CsrBool wave_deinit(void **instance)
{
    wave_instance_t *inst;
    inst = (wave_instance_t*)*instance;

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
    if(inst->buffer)
    {
        CsrPmemFree(inst->buffer);
        inst->buffer = NULL;
    }
    CsrPmemFree(inst);
    *instance = NULL;

    return TRUE;
}

/* Open wave with current configuration */
static CsrBool wave_open(void **instance)
{
    wave_instance_t *inst;
    CsrBool result;
    inst = (wave_instance_t*)*instance;

    result = FALSE;

    if(inst->file != NULL)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "file '%s' is opened already\n", inst->name);
        return TRUE;
    }

    if(inst->name != NULL)
    {
        /* Open file for either read or write */
        if(inst->filter->f_type & FILTER_PRODUCER)
        {
            inst->file = CsrBtFopen(inst->name, "rb");
        }
        else
        {
            inst->file = CsrBtFopen(inst->name, "wb");
        }

        if(inst->file != NULL)
        {
            result = TRUE;
        }
        else
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "could not open file '%s'\n", inst->name);
        }
    }

    /* Check file */
    if(result)
    {
        if(inst->filter->f_type & FILTER_PRODUCER)
        {
            if(!wave_parse_header(inst))
            {
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "error parsing wave file header\n");
                result = FALSE;
            }
            else
            {
                setPcmSettings(inst->channels,
                               inst->bits,
                               inst->sample_rate);
                setPcmLocked(TRUE);
            }
        }
        else
        {
            /* We need to skip (seek) past the wave-header.  The
             * header must be updated after receiving the stream
             * because the header contains the stream length, so
             * simply postpone writing the header until closing
             * the stream. */

            CsrUint8 buf[sizeof(wave_header_t)];
            CsrMemSet(buf, 0, sizeof(wave_header_t));
            if (CsrBtFwrite(buf, sizeof(wave_header_t), 1, inst->file) == sizeof(wave_header_t))
            {
                /* Flush file buffer to work around some
                 * bug where the above write is flushed to
                 * disk -after- application termination.
                 */
                CsrBtFflush(inst->file);

                inst->position = 0;
                inst->length = sizeof(wave_header_t);
            }
            else
            {
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "unable to seek past header area\n");
                result = FALSE;
            }
        }
    }

    /* Reset */
    inst->delay = CSR_SCHED_SECOND / TIMER_FREQ;
    inst->carry = 0;

    return result;
}

/* Close wave configuration */
static CsrBool wave_close(void **instance)
{
    wave_instance_t *inst;
    inst = (wave_instance_t*)*instance;

    if(inst->file)
    {
        /* Write wave header */
        if(!(inst->filter->f_type & FILTER_PRODUCER))
        {
            wave_save_header(inst);
        }

        CsrBtFclose(inst->file);
        inst->file = NULL;
    }
    inst->length = 0;
    inst->position = 0;

    return TRUE;
}

/* Start wave streaming */
static CsrBool wave_start(void **instance)
{
    wave_instance_t *inst;
    inst = (wave_instance_t*)*instance;

    if(inst->file == NULL)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "file not opened\n");
        return FALSE;
    }

    /* Start processing loop */
    if(inst->filter->f_type & FILTER_PRODUCER)
    {
        filtersStartTimer(inst->delay);
        inst->lasttime = CsrTimeGet(NULL);
        inst->carryTime = 0;
    }
    else
    {
        /* The PCM settings have settled at this point, so
         * read them in */
        getPcmSettings(&(inst->channels),
                       &(inst->bits),
                       &(inst->sample_rate));
    }

    return TRUE;
}

/* Stop wave streaming */
static CsrBool wave_stop(void **instance)
{
    wave_instance_t *inst;
    inst = (wave_instance_t*)*instance;

    /* Stop processing loop */
    if(inst->filter->f_type & FILTER_PRODUCER)
    {
        filtersStopTimer();
    }
    return TRUE;
}

/* Process wave data: Empty windows buffers */
static CsrBool wave_stream_process(CsrUint8 index,
                                  av2filter_t **chain,
                                  CsrBool freeData,
                                  void *data,
                                  CsrUint32 length)
{
    extern av2instance_t *Av2Instance;
    wave_instance_t *inst;
    CsrUint32 out_length;
    CsrUint32 read;
    CsrTime now;

    inst = (wave_instance_t*)(chain[index]->f_instance);

    if(inst->file == NULL)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "file handle invalid\n");
        return FALSE;
    }

    /* Calculate length of frame based on elapsed time */
    now = CsrTimeGet(NULL);

    if ((now - inst->lasttime) > 0)
    {
        out_length = (now - inst->lasttime + inst->carryTime) / CSR_SCHED_MILLISECOND;
        out_length = (inst->sample_rate * inst->align * out_length) / CSR_SCHED_MILLISECOND;
        inst->carryTime = (now - inst->lasttime + inst->carryTime) % CSR_SCHED_MILLISECOND;
    }
    else
    {
        /*handle timer wrap around*/
        out_length = (CSR_SCHED_SECOND / TIMER_FREQ) / CSR_SCHED_MILLISECOND;
        out_length = (inst->sample_rate * inst->align * out_length) / CSR_SCHED_MILLISECOND;
    }

    /* Make sure we don't send beyond end-of-file */
    if(out_length > (inst->length - inst->position))
    {
        out_length = inst->length - inst->position;
    }
    inst->lasttime = now;

    /* Adjust to OUTPUT_ALIGN alignment to make SBC happy */
    out_length  += inst->carry;
    inst->carry  = out_length % OUTPUT_ALIGN;
    out_length  -= inst->carry;

    if(out_length > READ_SIZE)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "out_length is too big %u\n",
               out_length);
        out_length = READ_SIZE;
        inst->carry = 0;
    }

    read = CsrBtFread(inst->buffer, 1, out_length, inst->file);
    inst->position += read;

    if(read == 0)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "file stream read error, requested %i, got %i\n",
               out_length, read);
        /* Since playing has finished(normal or abnormal)
         * we should stop reading, then close the file, and suspend the stream
         */
        filtersStopTimer();
        wave_close(&inst);
        suspendStream(Av2Instance);
        return FALSE;
    }
    else if(read != out_length)
    {
        out_length = read;
        /* Since playing has finished(normal or abnormal)
         * we should stop reading, then close the file, and suspend the stream
         */
        filtersStopTimer();
        wave_close(&inst);
        suspendStream(Av2Instance);
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "stopping timer as we have reached EOF. Last read: %i, total read %u\n", read, inst->position);
    }

    /* We should not receive data, but check anyway */
    if(data && freeData)
    {
        CsrPmemFree(data);
    }

#ifdef DEBUG_VERBOSE
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "streaming %i bytes...\n", out_length);
#endif

    /* Pass data on to next stage */
    return chain[index+1]->f_process((CsrUint8)(index+1),
                                     chain,
                                     FALSE,
                                     inst->buffer,
                                     out_length);
}

/* Process filedump data: Empty windows buffers */
static CsrBool wave_write_process(CsrUint8 index,
                                 av2filter_t **chain,
                                 CsrBool freeData,
                                 void *data,
                                 CsrUint32 length)
{
    wave_instance_t *inst;
    CsrUint32 wr;
    inst = (wave_instance_t*)(chain[index]->f_instance);

    /* Write data to file */
    if(length > 0)
    {
        wr = CsrBtFwrite(data,
                       1,
                       length,
                       inst->file);
        if(wr == 0)
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "file write error, code %i\n",
                   ferror(inst->file));
        }
        else if(wr != length)
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "%i bytes received, but only %i written\n",
                   length, wr);
        }

        inst->length += length;
        inst->position += length;
    }

#ifdef DEBUG
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WAVE_PRN "writing %10i bytes (%10i total)...\n",
           length, inst->length);
#endif

    /* Pass data on to next stage */
    return chain[index+1]->f_process((CsrUint8)(index+1),
                                     chain,
                                     freeData,
                                     data,
                                     length);
}


/* Return wave configuration */
static CsrUint8 *wave_get_config(void **instance,
                                CsrUint8 *conf_len)
{
    /* We do not support this feature */
    *conf_len = 0;
    return NULL;
}

/* Set wave configuration */
static CsrBtAvResult wave_set_config(void **instance,
                                   CsrUint8 *conf,
                                   CsrUint8 conf_len)
{
    /* We do not support this feature */
    return CSR_BT_AV_ACCEPT;
}

/* Return wave service capabilities */
static CsrUint8 *wave_get_caps(void **instance,
                              CsrUint8 *cap_len)
{
    /* We do not support this feature */
    *cap_len = 0;
    return NULL;
}

/* Investigate remote capabilities, return optimal configuration */
static CsrUint8 *wave_remote_caps(void **instance,
                                 CsrUint8 *caps,
                                 CsrUint8 caps_len,
                                 CsrUint8 *conf_len)
{
    /* We do not support this feature */
    *conf_len = 0;
    return NULL;
}

/* Sink buffer level update */
static void wave_qos_update(void **instance,
                            CsrUint16 qosLevel)
{
    /* Not supported */
}

/* Print menu options to screen */
static void wave_menu(void **instance,
                      CsrUint8 *num_options)
{
    wave_instance_t *inst = *instance;
    CsrCharString *str;

    str = ((inst->name != NULL)
           ? inst->name
           : "n/a");

    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  0) Change filename (%s)\n", str);
    *num_options = 1;
}

/* Set user option */
static CsrBool wave_setup(void **instance,
                         CsrUint8 index,
                         char *value)
{
    wave_instance_t *inst;
    int i;
    inst = (wave_instance_t*)*instance;

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


static char* wave_return_filename(void **instance)
{
    wave_instance_t *inst = NULL;
    inst = (wave_instance_t*)*instance;

    return inst->name;
}
