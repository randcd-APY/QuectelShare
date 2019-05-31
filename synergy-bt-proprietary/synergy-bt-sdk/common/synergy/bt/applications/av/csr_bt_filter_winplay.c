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
#include <math.h>
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

/* Max samples/sbc frame = 512 bytes */
#define WAVE_OUT_BUFFER_SIZE      (50*512)          /* Size of each buffer */
#define PLAYBACK_THRESHOLD        (30*512)          /* Copy to waveout when this much data in buffer */
#define AUDIO_OUT_BUFFERS         25                /* Number of buffers */
#define OVERFLOW_BUFFERS          3                 /* Flush when this many buffers are filled */
#define WINPLAY_PRN               "filter_winplay: "

/*
#define DEBUG                     1
#define DEBUG_VERBOSE             1
*/

/* States */
#define WINPLAY_IDLE              0
#define WINPLAY_OPEN              1
#define WINPLAY_STARTED           2

/* Mutual exclusion with the scheduler thread */
CRITICAL_SECTION winplay_mutex;

/* Instance data for winplay filter */
typedef struct
{
    av2filter_t     *filter;
    CsrUint8          state;
    CsrUint8          buffer_ready;
    CsrUint8          buffer_next;
    HWAVEOUT         handle;
    HANDLE           thread;
    WAVEHDR          header[AUDIO_OUT_BUFFERS];
    CsrUint8         *buffer;
    CsrUint32         length;
    CsrBool           playing;
    CsrUint8          channels;
    CsrUint8          bits;
    CsrUint16         sample_freq;
} winplay_instance_t;

/* Global variable needed as windows uses callbacks */
static winplay_instance_t *WinplayInst = NULL;

/* Filter entry prototypes */
static CsrBool winplay_init(void **instance, av2filter_t *filter, av2instance_t *unused);
static CsrBool winplay_deinit(void **instance);
static CsrBool winplay_open(void **instance);
static CsrBool winplay_close(void **instance);
static CsrBool winplay_start(void **instance);
static CsrBool winplay_stop(void **instance);
static CsrBool winplay_process(CsrUint8 index,
                              av2filter_t **chain,
                              CsrBool freeData,
                              void *data,
                              CsrUint32 length);
static CsrUint8 *winplay_get_config(void **instance,
                                   CsrUint8 *conf_len);
static CsrBtAvResult winplay_set_config(void **instance,
                                      CsrUint8 *conf,
                                      CsrUint8 conf_len);
static CsrUint8 *winplay_get_caps(void **instance,
                                 CsrUint8 *cap_len);
static CsrUint8 *winplay_remote_caps(void **instance,
                                    CsrUint8 *caps,
                                    CsrUint8 caps_len,
                                    CsrUint8 *conf_len);
static void winplay_qos_update(void **instance,
                               CsrUint16 qosLevel);
static void winplay_menu(void **instance,
                         CsrUint8 *num_options);
static CsrBool winplay_setup(void **instance,
                            CsrUint8 index,
                            char *value);
static char* winplay_return_filename(void **instance);


/* Filter structure setup for winplay */
av2filter_t filter_winplay =
{
    "csr_windows_playback",
    FILTER_CONSUMER,
    AV_NO_MEDIA,
    AV_NO_CODEC,
    0,
    NULL,
    winplay_init,
    winplay_deinit,
    winplay_open,
    winplay_close,
    winplay_start,
    winplay_stop,
    winplay_process,
    winplay_get_config,
    winplay_set_config,
    winplay_get_caps,
    winplay_remote_caps,
    winplay_qos_update,
    winplay_menu,
    winplay_setup,
    winplay_return_filename
};

/* Internal print-error function */
static void winplay_error_print(DWORD err, char *str)
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

    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINPLAY_PRN "%s, error: code 0x%08X, windows error '%s'\n",
           str, err, ext);
}

/* Internal function to set new fill-buffer */
static void winplay_new_buffer(winplay_instance_t *inst)
{
    EnterCriticalSection(&winplay_mutex);
    if(inst->header[inst->buffer_next].dwUser == 0)
    {
        inst->buffer = inst->header[inst->buffer_next].lpData;
        inst->length = 0;
    }
    LeaveCriticalSection(&winplay_mutex);
}

/* Internal thread used for playback */
static DWORD WINAPI winplay_proc(LPVOID arg)
{
    MSG msg;

    /* Wait for a message sent to me by the audio driver */
    while(GetMessage(&msg, 0, 0, 0) == 1)
    {
        /* Figure out which message was sent */
        switch(msg.message)
        {
            case MM_WOM_DONE:
                {
                    EnterCriticalSection(&winplay_mutex);
                    if (WinplayInst->state == WINPLAY_STARTED)
                    {
#ifdef DEBUG_VERBOSE
                        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINPLAY_PRN "MM_WOM_DONE received in winplay started state\n");
#endif
                        ((WAVEHDR*)msg.lParam)->dwUser = 0;
                        ((WAVEHDR*)msg.lParam)->dwFlags &= ~WHDR_DONE;

                        WinplayInst->buffer_ready--;
                        if(WinplayInst->buffer_ready <= 0)
                        {
#ifdef DEBUG
                            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINPLAY_PRN "Winplayer is Paused\n");
#endif
                            WinplayInst->playing = FALSE;
                            waveOutPause(WinplayInst->handle);
                        }
                    }
                    else
                    {
#ifdef DEBUG
                        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINPLAY_PRN "MM_WOM_DONE received, but winplay NOT started\n");
#endif
                    }
                    LeaveCriticalSection(&winplay_mutex);
                    break;
                }
            case MM_WOM_CLOSE:
                {
                    /* Terminate this thread */
#ifdef DEBUG
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINPLAY_PRN "close thread\n");
#endif
                    return 0;
                }
            default:
                {
                    break;
                }
        }
    }

    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINPLAY_PRN "message loop ended\n");
    return 0;
}

/* Initialise winplay filter */
static CsrBool winplay_init(void **instance, struct av2filter_t *filter, av2instance_t *unused)
{
    winplay_instance_t *inst;

    /* Use the global instance */
    if(WinplayInst == NULL)
    {
        *instance = CsrPmemZalloc(sizeof(winplay_instance_t));
        WinplayInst = *instance;
    }
    inst = WinplayInst;

    InitializeCriticalSection(&winplay_mutex);

    /* Setup defaults */
    inst = (winplay_instance_t*)*instance;
    inst->filter = filter;
    inst->state = WINPLAY_IDLE;
    inst->length = 0;
    inst->handle = NULL;
    inst->thread = NULL;
    inst->buffer_ready = 0;
    inst->buffer_next = 0;
    inst->playing = FALSE;

    getPcmSettings(&(inst->channels),
                   &(inst->bits),
                   &(inst->sample_freq));

    return TRUE;
}

/* Deinitialise winplay filter */
static CsrBool winplay_deinit(void **instance)
{
    winplay_instance_t *inst;
    inst = (winplay_instance_t*)*instance;

    /* Free instance */
    DeleteCriticalSection(&winplay_mutex);
    CsrPmemFree(*instance);
    *instance = NULL;
    WinplayInst = NULL;

    return TRUE;
}

/* Open winplay with current configuration */
static CsrBool winplay_open(void **instance)
{
    winplay_instance_t *inst;
    WAVEFORMATEX wave_format;
    DWORD err;
    CsrUint8 i;

    inst = (winplay_instance_t*)*instance;

    getPcmSettings(&(inst->channels),
                   &(inst->bits),
                   &(inst->sample_freq));

#ifdef DEBUG
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINPLAY_PRN "Opening winplay with channels:%i bits:%i samplerate:%i\n",
                   inst->channels,
                   inst->bits,
                   inst->sample_freq);
#endif

    /* Initialize the format */
    wave_format.wFormatTag      = WAVE_FORMAT_PCM;
    wave_format.nChannels       = inst->channels;
    wave_format.nSamplesPerSec  = inst->sample_freq;
    wave_format.wBitsPerSample  = inst->bits;
    wave_format.nBlockAlign     = wave_format.nChannels * (wave_format.wBitsPerSample/8);
    wave_format.nAvgBytesPerSec = wave_format.nSamplesPerSec * wave_format.nBlockAlign;
    wave_format.cbSize          = 0;

    /* Make sure thread will run */
    inst->state        = WINPLAY_OPEN;
    inst->buffer_ready = 0;

    /* Start thread */
    inst->thread       = CreateThread(0,
                                      0,
                                      (LPTHREAD_START_ROUTINE)winplay_proc,
                                      0,
                                      0,
                                      &err);
    if(!inst->thread)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINPLAY_PRN "wave playback thread creation failed, code 0x%08X\n", GetLastError());
        inst->state = WINPLAY_IDLE;
        return FALSE;
    }
    if(!SetThreadPriority(inst->thread, THREAD_PRIORITY_ABOVE_NORMAL))
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINPLAY_PRN "could not set high priority for playback thread\n");
    }
    CloseHandle(inst->thread);

    /* Open the default wave out device, specifying callback */
    err = waveOutOpen(&(inst->handle),
                      WAVE_MAPPER,
                      &wave_format,
                      (DWORD)err,
                      0,
                      CALLBACK_THREAD);
    if(err)
    {
        winplay_error_print(err, "could not open playback device");
        inst->state = WINPLAY_IDLE;
        return FALSE;
    }

    /* Allocate header buffers */
    inst->header[0].lpData         = (char *)CsrPmemAlloc(WAVE_OUT_BUFFER_SIZE * AUDIO_OUT_BUFFERS);
    inst->header[0].dwBufferLength = WAVE_OUT_BUFFER_SIZE;
    inst->header[0].dwUser         = 0;
    inst->header[0].dwFlags        = 0;

    if (inst->header[0].lpData == NULL)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINPLAY_PRN "Problem allocating the header buffers %i\n", (WAVE_OUT_BUFFER_SIZE * AUDIO_OUT_BUFFERS));
    }

    /* Fill in header fields for buffer starting address and size */
    for(i=1; i<AUDIO_OUT_BUFFERS; i++)
    {
        inst->header[i].lpData         = inst->header[i-1].lpData + WAVE_OUT_BUFFER_SIZE;
        inst->header[i].dwBufferLength = WAVE_OUT_BUFFER_SIZE;
        inst->header[i].dwUser         = 0;
        inst->header[i].dwFlags        = 0;
    }

    /* Prepare the headers */
    for(i=0; i<AUDIO_OUT_BUFFERS; i++)
    {
        if((err = waveOutPrepareHeader(inst->handle, &inst->header[i], sizeof(WAVEHDR))))
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINPLAY_PRN "error preparing wave header %d, code 0x%08X\n", i, err);
            inst->state = WINPLAY_IDLE;
            return FALSE;
        }
    }

    return TRUE;
}

/* Close winplay configuration */
static CsrBool winplay_close(void **instance)
{
    winplay_instance_t *inst;
    CsrUint8 i;
    DWORD err;
    inst = (winplay_instance_t*)*instance;

    /* Stop playback */
    inst->playing = FALSE;
    inst->state   = WINPLAY_IDLE;
    waveOutReset(inst->handle);

    /* Unprepare buffers */
    err = 1;
    for(i=0; (i<AUDIO_OUT_BUFFERS) && (err != 0); i++)
    {
        err = waveOutUnprepareHeader(inst->handle,
                                     &(inst->header[i]),
                                     sizeof(WAVEHDR));
    }

    /* Close device */
    err = waveOutClose(inst->handle);
    Sleep(0);
    if(err != 0)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINPLAY_PRN "error closing playback handle, code 0x%08X\n", err);
    }
    else
    {
        /* Free buffer */
        CsrPmemFree(inst->header[0].lpData);
        inst->header[0].lpData = NULL;
    }

    /* Make sure thread is down */
    if(inst->thread != NULL)
    {
        if(GetExitCodeThread(inst->thread, &err) != 0)
        {
#ifdef DEBUG
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINPLAY_PRN "playback thread still running, code 0x%08X, terminating it\n", err);
#endif
            TerminateThread(inst->thread, err);
#ifdef DEBUG
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINPLAY_PRN "thread termination code was 0x%08X\n", err);
#endif
        }
        inst->thread = NULL;
    }

    return TRUE;
}

/* Start winplay streaming */
static CsrBool winplay_start(void **instance)
{
    winplay_instance_t *inst;
    CsrUint8             channels;
    CsrUint8             bits;
    CsrUint16            sample_freq;

    inst = (winplay_instance_t*)*instance;

    /* Check that settings have not changed */
    getPcmSettings(&channels, &bits, &sample_freq);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINPLAY_PRN "PCM settings (winplay_start) are channels:%i bits:%i samplerate:%i\n",
           channels,
           bits,
           sample_freq);

    if((channels    != inst->channels)   ||
       (bits        != inst->bits)       ||
       (sample_freq != inst->sample_freq))
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINPLAY_PRN "PCM settings (winplay_start) have changed, reconfiguring\n");
        winplay_close(instance);
        winplay_open(instance);
    }

    /* Begin play (when there's something in the buffer) */
    inst->state   = WINPLAY_STARTED;
    inst->playing = FALSE; /* TRUE; */
    waveOutRestart(inst->handle);

    return TRUE;
}

/* Stop winplay streaming */
static CsrBool winplay_stop(void **instance)
{
    winplay_instance_t *inst;
    inst = (winplay_instance_t*)*instance;

    /* Pause */
    inst->playing = FALSE;
    inst->state   = WINPLAY_OPEN;
    waveOutPause(inst->handle);

    return TRUE;
}

/* Process winplay data: Empty windows buffers */
static CsrBool winplay_process(CsrUint8       index,
                              av2filter_t **chain,
                              CsrBool        freeData,
                              void         *data,
                              CsrUint32      length)
{
    winplay_instance_t *inst;
    inst = (winplay_instance_t*)(chain[index]->f_instance);

    /* Do we need a new collection buffer */
    if(inst->length == 0)
    {
        winplay_new_buffer(inst);
    }

    /* Transfer data to the collection buffer */
    CsrMemCpy((CsrUint8*)(inst->buffer + inst->length),
           data,
           length);
    inst->length += length;

    /* If we've collected enough data, transfer chunk to playback buffer */
    if(inst->length >= PLAYBACK_THRESHOLD)
    {
        MMRESULT err;

        EnterCriticalSection(&winplay_mutex);
        inst->header[inst->buffer_next].dwUser = 1;
        inst->header[inst->buffer_next].dwBufferLength = inst->length;
        inst->buffer_ready++;

        /* Wave out the data */
#ifdef DEBUG
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINPLAY_PRN "play buffer, buffer_next:%i, length:%i, buffer_ready:%i\n",
               inst->buffer_next, inst->length, inst->buffer_ready);
#endif
        err = waveOutWrite(inst->handle,
                           &inst->header[inst->buffer_next],
                           sizeof(WAVEHDR));
        if(err != MMSYSERR_NOERROR)
        {
            winplay_error_print(err, "error writing to playback device");
        }

        /* Do we need to restart playback? */
        if(inst->playing == FALSE)
        {
            if (inst->buffer_ready > OVERFLOW_BUFFERS)
            {
#ifdef DEBUG
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINPLAY_PRN "Winplayer is restarted\n");
#endif
                inst->playing = TRUE;
                waveOutRestart(inst->handle);
            }
            else
            {
#ifdef DEBUG
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,WINPLAY_PRN "Winplayer is not restarted due to lack of ready buffers\n");
#endif
            }
        }
        LeaveCriticalSection(&winplay_mutex);

        /* Get ready to fill next buffer */
        inst->buffer_next = (inst->buffer_next + 1) % AUDIO_OUT_BUFFERS;
        inst->length      = 0;
    }

    /* Free data and trigger next stage even though data has been consumed */
    if(freeData && data)
    {
        CsrPmemFree(data);
    }
    return chain[index+1]->f_process((CsrUint8)(index+1),
                                     chain,
                                     FALSE,
                                     NULL,
                                     0);
}

/* Return winplay configuration */
static CsrUint8 *winplay_get_config(void **instance,
                                   CsrUint8 *conf_len)
{
    /* We do not support this feature */
    *conf_len = 0;
    return NULL;
}

/* Set winplay configuration */
static CsrBtAvResult winplay_set_config(void **instance,
                                      CsrUint8 *conf,
                                      CsrUint8 conf_len)
{
    /* We do not support this feature */
    return CSR_BT_AV_ACCEPT;
}

/* Return winplay service capabilities */
static CsrUint8 *winplay_get_caps(void **instance,
                                 CsrUint8 *cap_len)
{
    /* We do not support this feature */
    *cap_len = 0;
    return NULL;
}

/* Investigate remote capabilities, return optimal configuration */
static CsrUint8 *winplay_remote_caps(void **instance,
                                    CsrUint8 *caps,
                                    CsrUint8 caps_len,
                                    CsrUint8 *conf_len)
{
    /* We do not support this feature */
    *conf_len = 0;
    return NULL;
}

/* Remote sink side buffer level update */
static void winplay_qos_update(void **instance,
                               CsrUint16 qosLevel)
{
    /* Not supported */
}

/* Print menu options to screen */
static void winplay_menu(void **instance,
                         CsrUint8 *num_options)
{
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  Windows playback filter does not support menu\n");
    *num_options = 0;
}

/* Set user option */
static CsrBool winplay_setup(void **instance,
                            CsrUint8 index,
                            char *value)
{
    CsrPmemFree(value);
    return FALSE;
}

static char* winplay_return_filename(void **instance)
{

    return NULL;
}


