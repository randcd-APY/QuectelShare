/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <mmsystem.h>

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

#define AUDIO_IN_BUFFERS          6  /* Must be 2 or more! */
#define WINGRAB_PRN               "wingrab: "
#define WINGRAB_DEF_BUFFER_SIZE   12288
/* #define DEBUG */

/* States */
#define WINGRAB_IDLE              0
#define WINGRAB_OPEN              1
#define WINGRAB_STARTED           2

/* Mutual exclusion with the scheduler thread */
CRITICAL_SECTION wingrab_mutex;

/* Buffer to hold data ready for AV2 */
typedef struct
{
    CsrBool     has_valid;
    CsrUint32   length;
    CsrUint8    *data;
} bufferExchange_t;

/* Instance data for wingrab filter */
typedef struct
{
    av2filter_t      *filter;
    CsrUint8          state;
    HWAVEIN          handle;
    HANDLE           thread;
    HANDLE           event;
    WAVEHDR          header[AUDIO_IN_BUFFERS];
    bufferExchange_t buffers[AUDIO_IN_BUFFERS];
    CsrUint8          next_buffer;
    CsrUint8          take_buffer;
    CsrSchedBgint        bgint_filter;
} wingrab_instance_t;

/* Filter entry prototypes */
static CsrBool wingrab_init(void **instance, av2filter_t *filter, av2instance_t *av2inst);
static CsrBool wingrab_deinit(void **instance);
static CsrBool wingrab_open(void **instance);
static CsrBool wingrab_close(void **instance);
static CsrBool wingrab_start(void **instance);
static CsrBool wingrab_stop(void **instance);
static CsrBool wingrab_process(CsrUint8 index,
                              av2filter_t **chain,
                              CsrBool freeData,
                              void *data,
                              CsrUint32 length);
static CsrUint8 *wingrab_get_config(void **instance,
                                   CsrUint8 *conf_len);
static CsrBtAvResult wingrab_set_config(void **instance,
                                      CsrUint8 *conf,
                                      CsrUint8 conf_len);
static CsrUint8 *wingrab_get_caps(void **instance,
                                 CsrUint8 *cap_len);
static CsrUint8 *wingrab_remote_caps(void **instance,
                                    CsrUint8 *caps,
                                    CsrUint8 caps_len,
                                    CsrUint8 *conf_len);
static void wingrab_menu(void **instance,
                         CsrUint8 *num_options);
static void wingrab_qos_update(void **instance,
                               CsrUint16 qosLevel);
static CsrBool wingrab_setup(void **instance,
                            CsrUint8 index,
                            char *value);
static CsrBool wingrab_reopen(wingrab_instance_t *inst);
static char* wingrab_return_filename(void **instance);


/* Filter structure setup for wingrab encoder */
struct av2filter_t filter_wingrab =
{
    "csr_windows_grabber",
    FILTER_PRODUCER,
    AV_NO_MEDIA,
    AV_NO_CODEC,
    0,
    NULL,
    wingrab_init,
    wingrab_deinit,
    wingrab_open,
    wingrab_close,
    wingrab_start,
    wingrab_stop,
    wingrab_process,
    wingrab_get_config,
    wingrab_set_config,
    wingrab_get_caps,
    wingrab_remote_caps,
    wingrab_qos_update,
    wingrab_menu,
    wingrab_setup,
    wingrab_return_filename
};

/* Internal print-error function */
static void wingrab_error_print(DWORD err, char *str)
{
    char buffer[128];
    char *ext;

    if(mciGetErrorString(err, buffer, sizeof(buffer)))
    {
        ext = buffer;
    }
    else
    {
        ext = "n/a";
    }

    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINGRAB_PRN "%s, error code 0x%08X, windows error '%s'\n",
           str, err, ext);
}

/* Internal grabber thread callback */
DWORD WINAPI wingrab_proc(LPVOID arg)
{
    wingrab_instance_t *inst;
    WAVEHDR *wave_hdr;
    CsrUint8 i;

    inst = (wingrab_instance_t *)arg;

    /* Run this thread as long as device is open */
    while(inst->state >= WINGRAB_OPEN)
    {
        /* Wait until a buffer is ready */
        WaitForSingleObject(inst->event, INFINITE);
        if(inst->state < WINGRAB_OPEN)
        {
            break;
        }

        /* Find the wave header that is ready from the 'done' flag */
        wave_hdr = NULL;
        for(i=0; i<AUDIO_IN_BUFFERS; i++)
        {
            if(inst->header[i].dwFlags & WHDR_DONE)
            {
                wave_hdr = &(inst->header[i]);
                break;
            }
        }

        if(wave_hdr != NULL)
        {
            /* Found the wave header, store pointer for exchange with the scheduler process */
            EnterCriticalSection(&wingrab_mutex);
            if(inst->buffers[inst->next_buffer].has_valid == FALSE)
            {
                inst->buffers[inst->next_buffer].data = wave_hdr->lpData;
                inst->buffers[inst->next_buffer].length = wave_hdr->dwBytesRecorded;
                inst->buffers[inst->next_buffer].has_valid = TRUE;

                inst->next_buffer = (inst->next_buffer+1) % AUDIO_IN_BUFFERS;
                LeaveCriticalSection(&wingrab_mutex);
            }
            else
            {
                LeaveCriticalSection(&wingrab_mutex);
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINGRAB_PRN "error: audio in-buffer not ready\n");
                inst->thread = NULL;
                wingrab_reopen(inst);
                return 0;
            }

            /* Trigger background interrupt */
            if(inst->state == WINGRAB_STARTED)
            {
                CsrSchedBgintSet(inst->bgint_filter);
            }
            else
            {
                waveInStop(inst->handle);
            }

            /* Requeue the audio buffer to allow Windows to reuse it */
            wave_hdr->dwFlags &= ~WHDR_DONE;
            waveInAddBuffer(inst->handle,
                            wave_hdr,
                            sizeof(WAVEHDR));
        }
    }

    inst->next_buffer = 0;
    return 0;
}

/* Initialise wingrab filter */
static CsrBool wingrab_init(void **instance, struct av2filter_t *filter, av2instance_t *av2inst)
{
    wingrab_instance_t *inst;

    if (*instance == NULL)
    {
        *instance = CsrPmemZalloc(sizeof(wingrab_instance_t));
    }

    InitializeCriticalSection(&wingrab_mutex);

    /* Setup defaults */
    inst = (wingrab_instance_t*)*instance;
    inst->filter = filter;
    inst->next_buffer = 0;
    inst->take_buffer = 0;
    inst->bgint_filter = CsrSchedBgintReg(filtersBackgroundInterrupt, av2inst, "wingrab_filter");
    inst->event = CreateEvent(NULL, FALSE, FALSE, NULL);

    /* The scheduler should have high priority to provide quality audio */
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

    return TRUE;
}

/* Deinitialise wingrab filter */
static CsrBool wingrab_deinit(void **instance)
{
    wingrab_instance_t *inst;
    inst = (wingrab_instance_t*)*instance;

    CsrSchedBgintUnreg(inst->bgint_filter);

    /* Free instance */
    CloseHandle(inst->event);

    DeleteCriticalSection(&wingrab_mutex);

    CsrPmemFree(*instance);
    *instance = NULL;

    return TRUE;
}

/* Reopen the wave device because of reconfiguration */
static CsrBool wingrab_reopen(wingrab_instance_t *inst)
{
    CsrUint8 channels;
    CsrUint8 bits;
    CsrUint16 sample_freq;
    CsrUint16 bufsize;
    WAVEFORMATEX wave_format;
    DWORD err;
    CsrUint8 i;

#ifdef DEBUG
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINGRAB_PRN "reopen in progress...\n");
#endif

    /* Stop grab */
    inst->state = WINGRAB_IDLE;
    SetEvent(inst->event);
    Sleep(1);
    waveInStop(inst->handle);
    waveInReset(inst->handle);

    /* Unprepare buffers */
    err = 1;
    for(i=0; (i<AUDIO_IN_BUFFERS) && (err != 0); i++)
    {
        err = waveInUnprepareHeader(inst->handle,
                                    &inst->header[i],
                                    sizeof(WAVEHDR));
        if(err != 0)
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINGRAB_PRN "error unpreparing header %i\n", i);
        }
    }

    /* Close device */
    err = 1;
    if(inst->handle != NULL)
    {
        err = waveInClose(inst->handle);
        Sleep(0);
        if(err != 0)
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINGRAB_PRN "error closing grabber handle, code 0x%08X\n", err);
        }
    }

    /* Make sure thread is down */
    if(inst->thread != NULL)
    {
        Sleep(1);
        if(GetExitCodeThread(inst->thread, &err) != 0)
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINGRAB_PRN "grabber thread still running, code 0x%08X, terminating it\n", err);
            TerminateThread(inst->thread, err);
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINGRAB_PRN "thread termination code was 0x%08X\n", err);
        }
        inst->thread = NULL;
    }

    /* Close event and free buffer */
    if(inst->header[0].lpData != NULL)
    {
        CsrPmemFree(inst->header[0].lpData);
        inst->header[0].lpData = NULL;
    }


    /* Get settings */
    channels = 0;
    bits = 0;
    sample_freq = 0;
    bufsize = 0;
    getPcmSettings(&channels, &bits, &sample_freq);
    getSampleBufferSize(&bufsize);
    if(bufsize == 0)
    {
        bufsize = WINGRAB_DEF_BUFFER_SIZE;
    }

    /* We need to adjust the buffersize as Windows usually deliver more data
     * than we asked for... */
    bufsize = bufsize / 4;

    /* Initialize the wave format */
    wave_format.wFormatTag = WAVE_FORMAT_PCM;
    wave_format.nChannels = channels;
    wave_format.nSamplesPerSec = sample_freq;
    wave_format.wBitsPerSample = bits;
    wave_format.nBlockAlign = wave_format.nChannels * (wave_format.wBitsPerSample/8);
    wave_format.nAvgBytesPerSec = wave_format.nSamplesPerSec * wave_format.nBlockAlign;
    wave_format.cbSize = 0;

    /* Open the default audio-in device */
    err = waveInOpen(&(inst->handle),
                     WAVE_MAPPER,
                     &wave_format,
                     (DWORD)inst->event,
                     0,
                     CALLBACK_EVENT);
    if(err)
    {
        wingrab_error_print(err, "could not open grabber device");
        inst->handle = 0;
        return FALSE;
    }

    /* Allocate header buffers */
    inst->header[0].lpData = (char*)CsrPmemAlloc(AUDIO_IN_BUFFERS
                                            * bufsize
                                            * wave_format.nBlockAlign);
    inst->header[0].dwBufferLength = bufsize * wave_format.nBlockAlign;

    /* Fill in header fields for buffer starting address and size */
    for(i=1; i<AUDIO_IN_BUFFERS; i++)
    {
        inst->header[i].dwBufferLength = inst->header[0].dwBufferLength;
        inst->header[i].lpData = inst->header[i-1].lpData + inst->header[0].dwBufferLength;
    }

    /* Prepare and queue the headers */
    for(i=0; i<AUDIO_IN_BUFFERS; i++)
    {
        if((err = waveInPrepareHeader(inst->handle,
                                      &inst->header[i],
                                      sizeof(WAVEHDR))))
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINGRAB_PRN "error preparing wave header %i, code 0x%08X\n",
                   i, err);
            if(inst->header[0].lpData)
            {
                CsrPmemFree(inst->header[0].lpData);
                inst->header[0].lpData = NULL;
            }
            return FALSE;
        }

        inst->buffers[i].has_valid = FALSE;
        err = waveInAddBuffer(inst->handle,
                              &inst->header[i],
                              sizeof(WAVEHDR));
        if(err != 0)
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINGRAB_PRN "error queuing wave header %i, code 0x%08X\n", i, err);
            if(inst->header[0].lpData)
            {
                CsrPmemFree(inst->header[0].lpData);
                inst->header[0].lpData = NULL;
            }
            return FALSE;
        }
    }

    /* Done */
    inst->state = WINGRAB_OPEN;
    inst->take_buffer = 0;
    inst->next_buffer = 0;

    /* Create the grabber thread */
    inst->thread = CreateThread(0,
                                0,
                                (LPTHREAD_START_ROUTINE)wingrab_proc,
                                inst,
                                0,
                                &err);
    if(!inst->thread)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINGRAB_PRN "wave grabber thread creation failed, code 0x%08X\n", GetLastError());
        inst->state = WINGRAB_IDLE;
        return FALSE;
    }
    if(!SetThreadPriority(inst->thread, THREAD_PRIORITY_ABOVE_NORMAL))
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINGRAB_PRN "could not set high priority for grabber thread\n");
    }
    CloseHandle(inst->thread);

#ifdef DEBUG
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINGRAB_PRN "grabber device has been reconfigured\n");
#endif

    return TRUE;
}

/* Open wingrab with current configuration */
static CsrBool wingrab_open(void **instance)
{
    wingrab_instance_t *inst;
    CsrUint8 channels;
    CsrUint8 bits;
    CsrUint16 sample_freq;
    CsrUint16 bufsize;
    WAVEFORMATEX wave_format;
    DWORD err;
    CsrUint8 i;
    inst = (wingrab_instance_t*)*instance;

    /* Reopen (reconfigure) the wave device */
    if(inst->handle != NULL)
    {
        return wingrab_reopen(inst);
    }

    channels = 0;
    bits = 0;
    sample_freq = 0;
    bufsize = 0;
    getPcmSettings(&channels, &bits, &sample_freq);
    getSampleBufferSize(&bufsize);
    if(bufsize == 0)
    {
        bufsize = WINGRAB_DEF_BUFFER_SIZE;
    }

    /* We need to adjust the buffersize as Windows usually deliver more data
     * than we asked for... */
    bufsize = bufsize / 4;

    /* Initialize the wave format */
    wave_format.wFormatTag = WAVE_FORMAT_PCM;
    wave_format.nChannels = channels;
    wave_format.nSamplesPerSec = sample_freq;
    wave_format.wBitsPerSample = bits;
    wave_format.nBlockAlign = wave_format.nChannels * (wave_format.wBitsPerSample/8);
    wave_format.nAvgBytesPerSec = wave_format.nSamplesPerSec * wave_format.nBlockAlign;
    wave_format.cbSize = 0;

    /* Open the default audio-in device */
    err = waveInOpen(&(inst->handle),
                     WAVE_MAPPER,
                     &wave_format,
                     (DWORD)inst->event,
                     0,
                     CALLBACK_EVENT);
    if(err)
    {
        wingrab_error_print(err, "could not open grabber device");
        inst->handle = 0;
        return FALSE;
    }

    /* Allocate header buffers */
    inst->header[0].lpData = (char*)CsrPmemAlloc(AUDIO_IN_BUFFERS
                                            * bufsize
                                            * wave_format.nBlockAlign);
    inst->header[0].dwBufferLength = bufsize * wave_format.nBlockAlign;

    /* Fill in header fields for buffer starting address and size */
    for(i=1; i<AUDIO_IN_BUFFERS; i++)
    {
        inst->header[i].dwBufferLength = inst->header[0].dwBufferLength;
        inst->header[i].lpData = inst->header[i-1].lpData + inst->header[0].dwBufferLength;
    }

    /* Prepare and queue the headers */
    for(i=0; i<AUDIO_IN_BUFFERS; i++)
    {
        if((err = waveInPrepareHeader(inst->handle,
                                      &inst->header[i],
                                      sizeof(WAVEHDR))))
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINGRAB_PRN "error preparing wave header %i, code 0x%08X\n",
                   i, err);
            if(inst->header[0].lpData)
            {
                CsrPmemFree(inst->header[0].lpData);
                inst->header[0].lpData = NULL;
            }
            return FALSE;
        }

        inst->buffers[i].has_valid = FALSE;
        err = waveInAddBuffer(inst->handle,
                              &inst->header[i],
                              sizeof(WAVEHDR));
        if(err != 0)
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINGRAB_PRN "error queuing wave header %i, code 0x%08X\n", i, err);
            if(inst->header[0].lpData)
            {
                CsrPmemFree(inst->header[0].lpData);
                inst->header[0].lpData = NULL;
            }
            return FALSE;
        }
    }

    /* Done */
    inst->state = WINGRAB_OPEN;
    inst->take_buffer = 0;
    inst->next_buffer = 0;

    /* Create the grabber thread */
    inst->thread = CreateThread(0,
                                0,
                                (LPTHREAD_START_ROUTINE)wingrab_proc,
                                inst,
                                0,
                                &err);
    if(!inst->thread)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINGRAB_PRN "wave grabber thread creation failed, code 0x%08X\n", GetLastError());
        inst->state = WINGRAB_IDLE;
        return FALSE;
    }
    if(!SetThreadPriority(inst->thread, THREAD_PRIORITY_ABOVE_NORMAL))
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINGRAB_PRN "could not set high priority for grabber thread\n");
    }
    CloseHandle(inst->thread);

    return TRUE;
}

/* Close wingrab configuration */
static CsrBool wingrab_close(void **instance)
{
    wingrab_instance_t *inst;
    DWORD err;
    CsrUint8 i;
    inst = (wingrab_instance_t*)*instance;

    /* Stop grab */
    inst->state = WINGRAB_IDLE;
    PulseEvent(inst->event);
    Sleep(1);
    waveInReset(inst->handle);

    /* Unprepare buffers */
    err = 1;
    for(i=0; (i<AUDIO_IN_BUFFERS) && (err != 0); i++)
    {
        err = waveInUnprepareHeader(inst->handle,
                                    &inst->header[i],
                                    sizeof(WAVEHDR));
    }

    /* Close device */
    err = 1;
    if(inst->handle != NULL)
    {
        err = waveInClose(inst->handle);
        Sleep(0);
        if(err != 0)
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINGRAB_PRN "error closing grabber handle, code 0x%08X\n", err);
        }
    }
    inst->handle = NULL;

    /* Make sure thread is down */
    if(inst->thread != NULL)
    {
        Sleep(1);
        if(GetExitCodeThread(inst->thread, &err) != 0)
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINGRAB_PRN "grabber thread still running, code 0x%08X, terminating it\n", err);
            TerminateThread(inst->thread, err);
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINGRAB_PRN "thread termination code was 0x%08X\n", err);
        }
        Sleep(1);
        inst->thread = NULL;
    }

    /* Close event and free buffer */
    if(inst->header[0].lpData != NULL)
    {
        CsrPmemFree(inst->header[0].lpData);
        inst->header[0].lpData = NULL;
    }

    return TRUE;
}

/* Start wingrab streaming */
static CsrBool wingrab_start(void **instance)
{
    wingrab_instance_t *inst;
    DWORD err;
    inst = (wingrab_instance_t*)*instance;

    /* Start recording */
    err = waveInStart(inst->handle);
    if(err != 0)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINGRAB_PRN "could not start grabber device, code 0x%08X\n", err);
        return FALSE;
    }
    else
    {
        inst->state = WINGRAB_STARTED;
    }

    return TRUE;
}

/* Stop wingrab streaming */
static CsrBool wingrab_stop(void **instance)
{
    wingrab_instance_t *inst;
    DWORD err;
    inst = (wingrab_instance_t*)*instance;

    /* Stop */
    inst->state = WINGRAB_OPEN;
    err = waveInStop(inst->handle);
    if(err != 0)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINGRAB_PRN "could not stop grabber device, code 0x%08X\n", err);
    }

    return TRUE;
}

/* Process wingrab data: Encoder */
static CsrBool wingrab_process(CsrUint8 index,
                              av2filter_t **chain,
                              CsrBool freeData,
                              void *data,
                              CsrUint32 length)
{
    wingrab_instance_t *inst;
    void *out_data;
    CsrUint32 out_length;
    inst = (wingrab_instance_t*)(chain[index]->f_instance);

    /* Note that this function should be called by behalf of a background interrupt,
     * which was triggered by the wingrab_proc thread because a buffer
     * now contain valid samples. The background interrupt handler itself is a
     * part of AV2, that starts the processing-run. Note however, that we still
     * need to claim the mutex, as the thread is still running! */

    EnterCriticalSection(&wingrab_mutex);

    if(inst->buffers[inst->take_buffer].has_valid == TRUE)
    {
        /* Copy data from grabber buffer */
        out_length = inst->buffers[inst->take_buffer].length;
        out_data   = inst->buffers[inst->take_buffer].data;

        /* Buffer is now ready to receive new data */
        inst->buffers[inst->take_buffer].has_valid = FALSE;

        /* Get ready to receive next buffer */
        inst->take_buffer = (inst->take_buffer + 1) % AUDIO_IN_BUFFERS;

        /* Trigger a new run now? */
        if(inst->buffers[inst->take_buffer].has_valid == TRUE)
        {
            LeaveCriticalSection(&wingrab_mutex);
            CsrSchedBgintSet(inst->bgint_filter);
        }
        else
        {
            LeaveCriticalSection(&wingrab_mutex);
        }
    }
    else
    {
        /* Try next buffer */
        inst->take_buffer = (inst->take_buffer + 1) % AUDIO_IN_BUFFERS;

        LeaveCriticalSection(&wingrab_mutex);
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINGRAB_PRN "buffer did not contain valid samples\n");
        return TRUE;
    }

    /* Deliver data to next stage (send buffer directly, do not allow free) */
#ifdef DEBUG
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINGRAB_PRN "grabbed %i bytes\n", out_length);
#endif
    return chain[index+1]->f_process((CsrUint8)(index+1),
                                     chain,
                                     FALSE,
                                     out_data,
                                     out_length);
}

/* Return wingrab configuration */
static CsrUint8 *wingrab_get_config(void **instance,
                                   CsrUint8 *conf_len)
{
    /* We do not support this feature */
    *conf_len = 0;
    return NULL;
}

/* Set wingrab configuration */
static CsrBtAvResult wingrab_set_config(void **instance,
                                      CsrUint8 *conf,
                                      CsrUint8 conf_len)
{
    /* We do not support this feature */
    return CSR_BT_AV_ACCEPT;
}

/* Return wingrab service capabilities */
static CsrUint8 *wingrab_get_caps(void **instance,
                                 CsrUint8 *cap_len)
{
    /* We do not support this feature */
    *cap_len = 0;
    return NULL;
}

/* Investigate remote capabilities, return optimal configuration */
static CsrUint8 *wingrab_remote_caps(void **instance,
                                    CsrUint8 *caps,
                                    CsrUint8 caps_len,
                                    CsrUint8 *conf_len)
{
    /* We do not support this feature */
    *conf_len = 0;
    return NULL;
}

/* Remote sink buffer level update */
static void wingrab_qos_update(void **instance,
                               CsrUint16 qosLevel)
{
    /* Not supported */
}

/* Print menu options to screen */
static void wingrab_menu(void **instance,
                         CsrUint8 *num_options)
{
    /* Not supported */
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  Windows audio grabber does not support menu options\n");
    *num_options = 0;
}

/* Set user option */
static CsrBool wingrab_setup(void **instance,
                            CsrUint8 index,
                            char *value)
{
    /* Not supported */
    CsrPmemFree(value);
    return FALSE;
}

static char* wingrab_return_filename(void **instance)
{


    return NULL;
}


