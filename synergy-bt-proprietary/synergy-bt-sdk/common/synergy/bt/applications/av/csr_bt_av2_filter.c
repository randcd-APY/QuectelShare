/******************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

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
#include "csr_bt_platform.h"
#include "csr_bt_av2.h"
#include "csr_bt_av2_filter.h"
#include "csr_app_lib.h"

/*
#define DEBUG          1
#define DEBUG_VERBOSE  1
*/

/* Externally defined filter definitions */
#ifdef USE_WINAUDIO
extern av2filter_t filter_winplay;
extern av2filter_t filter_wingrab;
#endif

#ifdef USE_ALSA
extern av2filter_t filter_alsa_play;
extern av2filter_t filter_alsa_capture;
#endif

#ifdef USE_SBC
extern av2filter_t filter_sbc_encode;
extern av2filter_t filter_sbc_decode;
#endif

#ifdef USE_SBCFILE
extern av2filter_t filter_sbcfile_streamer;
extern av2filter_t filter_sbcfile_writer;
#endif

#ifdef USE_FILEDUMP
extern av2filter_t filter_filedump;
#endif

#ifdef USE_MP3
extern av2filter_t filter_mp3;
#endif

#ifdef USE_AAC
extern av2filter_t filter_aac;
#endif

#ifdef USE_WAVE
extern av2filter_t filter_stream_wave;
extern av2filter_t filter_write_wave;
#endif

#ifdef USE_AVROUTER
extern av2filter_t filter_avrouter_encode;
extern av2filter_t filter_avrouter_decode;
#endif

/* Special AV2 filters */
extern av2filter_t av2_source_filter;
extern av2filter_t av2_sink_filter;

/**************************************************************************************************
 * filtersSetup
 **************************************************************************************************/
void filtersSetup(av2instance_t *instData)
{
    CsrUint8 idx;

    /* This function defines the two filters that are assigned to the
     * "filter rule" when the role (source/sink) has been chosen. The
     * filters will be initialised when the first connection has been
     * established and deinitialsed again when it is closed.
 */

    /* This is the default SOURCE filter chain */
    idx = 0;

#ifdef USE_WINAUDIO
    instData->filters_src[idx++] = &filter_wingrab;
#endif
#if defined(USE_ALSA) && !defined (USE_WAVE_SRC_ALSA_SNK_ONLY)
    instData->filters_src[idx++] = &filter_alsa_capture;
#endif
#if defined (USE_WAVE) || defined (USE_WAVE_SRC_ALSA_SNK_ONLY)
    instData->filters_src[idx++] = &filter_stream_wave;
#endif
#ifdef USE_SBC
    instData->filters_src[idx++] = &filter_sbc_encode;
#endif
#ifdef USE_SBCFILE
    instData->filters_src[idx++] = &filter_sbcfile_streamer;
#endif
#ifdef USE_MP3
    instData->filters_src[idx++] = &filter_mp3;
#endif
#ifdef USE_AAC
    instData->filters_src[idx++] = &filter_aac;
#endif
#ifdef USE_AVROUTER
    instData->filters_src[idx++] = &filter_avrouter_encode;
#endif
    instData->filters_src[idx++] = &av2_source_filter; /* Internal filter for sending data */
    instData->filters_src[idx++] = NULL;               /* Terminate list */

    /* This is the default SINK filter chain */
    idx = 0;
#ifdef USE_SBC
    instData->filters_snk[idx++] = &filter_sbc_decode;
#endif
#ifdef USE_SBCFILE
    instData->filters_snk[idx++] = &filter_sbcfile_writer;
#endif
#if defined (USE_WAVE) && !defined (USE_WAVE_SRC_ALSA_SNK_ONLY)
    instData->filters_snk[idx++] = &filter_write_wave;
#endif
#ifdef USE_WINAUDIO
    instData->filters_snk[idx++] = &filter_winplay;
#endif
#if defined (USE_ALSA) || defined (USE_WAVE_SRC_ALSA_SNK_ONLY)
    instData->filters_snk[idx++] = &filter_alsa_play;
#endif
#ifdef USE_FILEDUMP
    instData->filters_snk[idx++] = &filter_filedump;
#endif
#ifdef USE_AVROUTER
    instData->filters_snk[idx++] = &filter_avrouter_decode;
#endif
    instData->filters_snk[idx++] = &av2_sink_filter;   /* Internal filter for stopping chain */
    instData->filters_snk[idx++] = NULL;               /* Terminate list */
}

/**************************************************************************************************
 * filtersInstall
 **************************************************************************************************/
void filtersInstall(av2instance_t *instData)
{
    CsrUint8 i;
    av2filter_t **filters_list;

    /* Get filter to use and number of connections to setup */
    if(instData->avConfig == DA_AV_CONFIG_AV_SRC)
    {
        filters_list = instData->filters_src;
#ifdef DEBUG
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Selecting source filters setup...\n");
#endif
    }
    else
    {
        filters_list = instData->filters_snk;
#ifdef DEBUG
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Selecting sink filters setup...\n");
#endif
    }

    /* Clear all filter entries */
    for(i=0; i<MAX_NUM_FILTERS; i++)
    {
        instData->filters[i] = NULL;
        instData->filter_instance[i] = NULL;
    }
    instData->filter_count = 0;

    /* Install the filters */
    i=0;
    /* [QTI] Fix KW issue#267045~267065. */
    while((i < MAX_NUM_FILTERS) && (filters_list[i] != NULL))
    {
        instData->filters[i] = filters_list[i];
#ifdef DEBUG
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Installing filter index %i: '%s'\n",
               i, filters_list[i]->f_name);
#endif
        i++;
    }
    instData->filter_count = i;

    /* Check consistency of filters */
    if(filtersCheck(instData))
    {
#ifdef DEBUG
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Filter combinations seems to be good\n");
#endif
    }
    else
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Filter combinations are invalid, check 'csr_bt_av2_filter.c' function filtersSetup()\n");
    }
}

/**************************************************************************************************
 * filtersUninstall
 **************************************************************************************************/
void filtersUninstall(av2instance_t *instData)
{
    instData->filter_count = 0;
}

/**************************************************************************************************
 * filterBackgroundInterrupt
 **************************************************************************************************/
void filtersBackgroundInterrupt(void *arg)
{
    av2instance_t *av2inst;

    av2inst = (av2instance_t *)arg;

    /* Invoke first filter */
    av2inst->filters[0]->f_process(0,
                                       av2inst->filters,
                                       FALSE,
                                       NULL,
                                       0);
}

/**************************************************************************************************
 * filterTimerInterrupt
 **************************************************************************************************/
void filtersTimerInterrupt(CsrUint16 mi, void *mv)
{
    extern av2instance_t *Av2Instance;
    CsrTime now;
    CsrInt64 adj;
    CsrInt64 last     = Av2Instance->filter_lasttime;
    CsrInt64 delay    = Av2Instance->filter_delay;
    CsrInt64 interval = Av2Instance->filter_interval;

    /* Reschedule ourself, taking elapsed time into account */
    now = CsrTimeGet(NULL);
    adj = ((CsrInt64)now - last) - delay;

    /* Secure timer calculation */
    if((interval - adj) < (CsrInt32)(CSR_SCHED_MILLISECOND))
    {
        delay = (CsrInt32)(CSR_SCHED_MILLISECOND);
#ifdef DEBUG
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"\nTimer too slow compensation:\n   now:%08lu last:%08lu interval:%08lu delay:%06lu adjust:%i\n",
               now, last, interval, delay, adj);
#endif
    }
    else
    {
        delay = interval - adj;
#ifdef DEBUG_VERBOSE
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"\nTimer compensation, now:%08lu last:%08lu interval:%08lu delay:%08lu adjust:%i\n",
               now, last, interval, delay, adj);
#endif
    }

    Av2Instance->filter_delay    = delay;
    Av2Instance->filter_lasttime = now;

    /* Schedule another timed event _before_ we invoke filter so that
     * the processing has as little influence as possible */
    if((Av2Instance->filter_timer != 0) &&
       (Av2Instance->filter_delay > 0))
    {
        Av2Instance->filter_timer = CsrSchedTimerSet(Av2Instance->filter_delay,
                                                   filtersTimerInterrupt,
                                                   mi,
                                                   mv);
    }
    else
    {
        Av2Instance->filter_timer = 0;
        Av2Instance->filter_delay = 0;
        Av2Instance->filter_interval = 0;
        Av2Instance->filter_lasttime = 0;
    }

    /* Invoke first filter by calling the filter directly */
    Av2Instance->filters[0]->f_process(0,
                                       Av2Instance->filters,
                                       FALSE,
                                       NULL,
                                       0);
}

/**************************************************************************************************
 * filtersStartTimer
 **************************************************************************************************/
void filtersStartTimer(CsrTime interval)
{
    extern av2instance_t *Av2Instance;

    Av2Instance->filter_interval = interval;

    if(Av2Instance->filter_timer == 0)
    {
        Av2Instance->filter_delay = interval;
        Av2Instance->filter_lasttime = CsrTimeGet(NULL);
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Periodic timer started, interval %i usec\n", Av2Instance->filter_delay);

        /* Start filter if not runnig, otherwise let it trigger and the new
         * interval will then be used */
        Av2Instance->filter_timer = CsrSchedTimerSet(Av2Instance->filter_delay,
                                                   filtersTimerInterrupt,
                                                   0,
                                                   NULL);
    }
}
/**************************************************************************************************
 * filtersStopTimer
 **************************************************************************************************/
void filtersStopTimer()
{
    extern av2instance_t *Av2Instance;
    CsrUint16 mi;
    void *mv;

    if(Av2Instance->filter_timer != 0)
    {
        CsrSchedTimerCancel(Av2Instance->filter_timer,
                           &mi,
                           &mv);
        Av2Instance->filter_timer = 0;
        Av2Instance->filter_delay = 0;
        Av2Instance->filter_interval = 0;
        Av2Instance->filter_lasttime = 0;
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Periodic timer stopped\n");
    }
}

/**************************************************************************************************
 * filtersKill
 **************************************************************************************************/
void filtersKill(av2instance_t *instData)
{
    CsrUint8 idx;

    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Deinitialising filters...\n");
    for(idx=0; idx<instData->filter_count; idx++)
    {
        /* Try going from start -> open */
        if(instData->filters[idx]->f_state == FILTER_ST_START)
        {
            instData->filters[idx]->f_stop(&(instData->filter_instance[idx]));
            instData->filters[idx]->f_state = FILTER_ST_OPEN;
#ifdef DEBUG
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Filter %i: %15s (%s)\n",
                   idx, "start->open", instData->filters[idx]->f_name);
#endif
        }
        /* Try going from open -> DaAvStateAppActive */
        if(instData->filters[idx]->f_state == FILTER_ST_OPEN)
        {
            instData->filters[idx]->f_close(&(instData->filter_instance[idx]));
            instData->filters[idx]->f_state = FILTER_ST_IDLE;
#ifdef DEBUG
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Filter %i: %15s (%s)\n",
                   idx, "open->DaAvStateAppActive", instData->filters[idx]->f_name);
#endif
        }
        /* Try going from DaAvStateAppActive -> un-initialised */
        if(instData->filters[idx]->f_state == FILTER_ST_IDLE)
        {
            instData->filters[idx]->f_close(&(instData->filter_instance[idx]));
            instData->filters[idx]->f_state = FILTER_ST_NOINIT;
#ifdef DEBUG
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Filter %i: %15s (%s)\n",
                   idx, "DaAvStateAppActive->noinit", instData->filters[idx]->f_name);
#endif
        }
    }
#ifdef DEBUG
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Filters killed\n");
#endif
}


/**************************************************************************************************
 * filtersRun
 **************************************************************************************************/
CsrBool filtersRun(av2instance_t *instData, CsrUint8 op)
{
    CsrInt8 i;
    CsrBool result;
    CsrBool stateok;
    CsrUint8 *state;
    char *name;
    CsrUint8 filter_count;

    /* If we are in multi-source mode and both connections are up, do not
     * run for the _second_ connection */
    if((instData->connectionMode == DA_AV_CONN_MODE_MULTI) &&
       (instData->currentConnection > 0))
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Skipping filter run for second connection\n");
        return TRUE;
    }

    /* Run through all filters */
    state = NULL;
    result = TRUE;
    stateok = TRUE;
    name = "n/a";

    /* [QTI] Fix KW issue#32484~32503. */
    filter_count = CSRMIN(instData->filter_count, MAX_NUM_FILTERS);
    for(i=0;
        (i < filter_count) && result && stateok;
        i++)
    {
        name = instData->filters[i]->f_name;
        state = &(instData->filters[i]->f_state);
        stateok = FALSE;

        switch(op)
        {
            case FILTER_INIT:
                /* Ignore same state */
                if(*state == FILTER_ST_IDLE)
                {
                    stateok = TRUE;
                }

                /* Go from un-initialised to DaAvStateAppActive */
                if(*state == FILTER_ST_NOINIT)
                {
                    stateok = TRUE;
#ifdef DEBUG
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Filter init: Index %i: %s\n", i, name);
#endif
                    result = instData->filters[i]->f_init(&(instData->filter_instance[i]),
                                                          (struct av2filter_t*)instData->filters[i],
                                                          instData);
                    instData->filters[i]->f_instance = instData->filter_instance[i];
                    *state = FILTER_ST_IDLE;
                }
                break;

            case FILTER_DEINIT:
                /* Ignore same state */
                if(*state == FILTER_ST_NOINIT)
                {
                    stateok = TRUE;
                }

                /* Go from DaAvStateAppActive to un-initialised */
                if(*state == FILTER_ST_IDLE)
                {
                    stateok = TRUE;
#ifdef DEBUG
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Filter deinit: Index %i: %s\n", i, name);
#endif
                    result = instData->filters[i]->f_deinit(&(instData->filter_instance[i]));
                    instData->filters[i]->f_instance = instData->filter_instance[i];
                    *state = FILTER_ST_NOINIT;
                }
                break;

            case FILTER_OPEN:
                /* Ignore same state */
                if(*state == FILTER_ST_OPEN)
                {
                    stateok = TRUE;
                }

                /* Go from open or DaAvStateAppActive to open */
                if((*state == FILTER_ST_IDLE) ||
                   (*state == FILTER_ST_OPEN))
                {
                    stateok = TRUE;
#ifdef DEBUG
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Filter open: Index %i: %s\n", i, name);
#endif
                    result = instData->filters[i]->f_open(&(instData->filter_instance[i]));
                    *state = FILTER_ST_OPEN;
                }
                break;

            case FILTER_CLOSE:
                /* Ignore same state */
                if(*state == FILTER_ST_IDLE)
                {
                    stateok = TRUE;
                }

                /* Go from started to open (moves on to next one directly below) */
                if(*state == FILTER_ST_START)
                {
                    stateok = TRUE;
#ifdef DEBUG
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Filter halt: Index %i: %s\n", i, name);
#endif
                    instData->filters[i]->f_stop(&(instData->filter_instance[i]));
                    *state = FILTER_ST_OPEN;
                }

                /* Go from open to DaAvStateAppActive */
                if(*state == FILTER_ST_OPEN)
                {
                    stateok = TRUE;
#ifdef DEBUG
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Filter close: Index %i: %s\n", i, name);
#endif
                    result = instData->filters[i]->f_close(&(instData->filter_instance[i]));
                    *state = FILTER_ST_IDLE;
                }
                break;

            case FILTER_START:
                /* Ignore same state */
                if(*state == FILTER_ST_START)
                {
                    stateok = TRUE;
                }

                /* Go from open to started */
                if(*state == FILTER_ST_OPEN)
                {
                    stateok = TRUE;
#ifdef DEBUG
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Filter start: Index %i: %s\n", i, name);
#endif
                    result = instData->filters[i]->f_start(&(instData->filter_instance[i]));
                    *state = FILTER_ST_START;
                }
                break;

            case FILTER_STOP:
                /* Ignore same state */
                if(*state == FILTER_ST_OPEN)
                {
                    stateok = TRUE;
                }

                /* Go from started to open */
                if(*state == FILTER_ST_START)
                {
                    stateok = TRUE;
#ifdef DEBUG
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Filter stop: Index %i: %s\n", i, name);
#endif
                    result = instData->filters[i]->f_stop(&(instData->filter_instance[i]));
                    *state = FILTER_ST_OPEN;
                }
                break;

            default:
                CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Invalid opcode to filter helper\n");
                result = FALSE;
                stateok = TRUE;
                state = NULL;
                break;
        }
    }

    /* Make sure state points to something */
    if(state == NULL)
    {
        CsrUint8 tmp = 0xFF;
        state = &tmp;
    }

    /* Print error */
    if(!stateok)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Filter transition error: Index %i, opcode %i, state %i: %s\n",
               i, op, *state, name);
    }
    if(!result)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Filter function error: Index %i, opcode %i, state %i: %s\n",
               i, op, *state, name);
    }
    return result;
}

/**************************************************************************************************
 * filtersCheck
 **************************************************************************************************/
CsrBool filtersCheck(av2instance_t *instData)
{
    int j;
    CsrUint8 mask;
    CsrBool fail;

    /* We havn't detected anything yet */
    mask = 0;
    fail = FALSE;

    j = 0;
    while((j<instData->filter_count) &&
          (instData->filters[j] != NULL) &&
          !fail)
    {
#ifdef DEBUG
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Filter check: Index %i: %s : ",
               j, instData->filters[j]->f_name);
#endif

        /* Check for producer */
        if(instData->filters[j]->f_type & FILTER_PRODUCER)
        {
#ifdef DEBUG
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"producer ");
#endif
            if(mask & FILTER_PRODUCER)
            {
                fail = TRUE;
                break;
            }
            mask |= FILTER_PRODUCER;
        }

        /* Check for consumer */
        if(instData->filters[j]->f_type & FILTER_CONSUMER)
        {
#ifdef DEBUG
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"consumer ");
#endif
            if(mask & FILTER_CONSUMER)
            {
                fail = TRUE;
                break;
            }
            mask |= FILTER_CONSUMER;
        }

        /* Check for encoder */
        if(instData->filters[j]->f_type & FILTER_ENCODER)
        {
#ifdef DEBUG
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"encoder ");
#endif
            if(mask & FILTER_ENCODER)
            {
                fail = TRUE;
                break;
            }
            mask |= FILTER_ENCODER;
        }

        /* Check for decoder */
        if(instData->filters[j]->f_type & FILTER_DECODER)
        {
#ifdef DEBUG
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"decoder ");
#endif
            if(mask & FILTER_DECODER)
            {
                fail = TRUE;
                break;
            }
            mask |= FILTER_DECODER;
        }

        /* Check for pass-through (multiple of these allowed) */
        if(instData->filters[j]->f_type & FILTER_PASS)
        {
#ifdef DEBUG
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"pass-through ");
#endif
        }

#ifdef DEBUG
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"\n");
#endif
        j++;
    }

    if(fail)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Multiple producers/consumers/encoders/decoders detected!\n");
    }

    return !fail;
}

/**************************************************************************************************
 * filtersQosUpdate
 **************************************************************************************************/
void filtersQosUpdate(av2instance_t *instData,
                      CsrUint16 qosLevel)
{
    int i;

    i = 0;
    while(instData->filters[i] != NULL)
    {
        instData->filters[i]->f_qos_update(&(instData->filter_instance[i]),
                                           qosLevel);
        i++;
    }
}

/**************************************************************************************************
 * filtersGetConfig
 **************************************************************************************************/
CsrUint8 *filtersGetConfig(av2instance_t *instData,
                          CsrUint8 *length)
{
    int i;

    i = 0;
    while(instData->filters[i] != NULL)
    {
        /* Check if the filter is a codec */
        if(instData->filters[i]->f_type & (FILTER_DECODER|FILTER_ENCODER))
        {
            return instData->filters[i]->f_get_config(&(instData->filter_instance[i]),
                                                      length);
        }
        i++;
    }

    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Did not find a codec filter that supports 'get configuration'\n");
    *length = 0;
    return NULL;
}

/**************************************************************************************************
 * filtersSetConfig
 **************************************************************************************************/
CsrBtAvResult filtersSetConfig(av2instance_t *instData,
                             CsrUint8 *conf,
                             CsrUint8 conf_len)
{
    int i;

    i = 0;
    while(instData->filters[i] != NULL)
    {
        /* Check if the filter is a codec */
        if(instData->filters[i]->f_type & (FILTER_DECODER|FILTER_ENCODER))
        {
            return instData->filters[i]->f_set_config(&(instData->filter_instance[i]),
                                                      conf,
                                                      conf_len);

        }
        i++;
    }

    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Did not find a codec filter that supports 'set configuration'\n");
    return CSR_BT_RESULT_CODE_A2DP_INVALID_CODEC_TYPE;
}

/**************************************************************************************************
 * filtersGetCaps
 **************************************************************************************************/
CsrUint8 *filtersGetCaps(av2instance_t *instData,
                        CsrUint8 *length)
{
    int i;

    i = 0;
    while(instData->filters[i] != NULL)
    {
        /* Check if the filter is a codec */
        if(instData->filters[i]->f_type & (FILTER_DECODER|FILTER_ENCODER))
        {
            return instData->filters[i]->f_get_caps(&(instData->filter_instance[i]),
                                                    length);
        }
        i++;
    }

    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Did not find a codec filter that supports 'get capabilities'\n");
    *length = 0;
    return NULL;
}

/**************************************************************************************************
 * filtersGetCaps
 **************************************************************************************************/
CsrUint8 *filtersRemoteCaps(av2instance_t *instData,
                           CsrUint8 *caps,
                           CsrUint8 caps_len,
                           CsrUint8 *conf_len)
{
    int i;

    i = 0;
    while(instData->filters[i] != NULL)
    {
        /* Check if the filter is a codec */
        if(instData->filters[i]->f_type & (FILTER_DECODER|FILTER_ENCODER))
        {
            return instData->filters[i]->f_remote_caps(&(instData->filter_instance[i]),
                                                       caps,
                                                       caps_len,
                                                       conf_len);
        }
        i++;
    }

    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Did not find a codec filter that supports 'remote configuration'\n");
    *conf_len = 0;
    return NULL;
}
