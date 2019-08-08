/******************************************************************************
  @file    qmi_client_utils.c
  @brief   The QMI Client Utils for telephony adaptation or test code.

  DESCRIPTION
  QMI message send / receive high level utilities

  ---------------------------------------------------------------------------

  Copyright (c) 2010, 2013-2014 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdarg.h>             //va_list
#ifdef FEATURE_QMI_ANDROID
#include <cutils/properties.h>
#else
#include <syslog.h>
#endif

#include "qmi_client_utils.h"


#define ASSERT(a)
#define QMI_UTIL_SVC_CONTROL_EVT_QUIT (1)
#define QMI_UTIL_SVC_CONTROL_EVT_INDICATION_REPOST (2)
#define QMI_UTIL_SVC_CONTROL_EVT_EXECUTE_QMI_REQ (3)

#define MAX_QMIPORT_NAME 16
static int qmi_client_util_is_logging_enabled = 0;
static char qmi_client_util_port[MAX_QMIPORT_NAME];
extern pthread_mutex_t log_mutex;
extern FILE* qmi_simple_ril_suite_output_handle();

/*qmi message library handle*/
//static int qmi_handle = QMI_INVALID_CLIENT_HANDLE;
//--------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct qmi_util_outstanding_request_info
{
    qmi_util_request_id         request_id;
    int                         service_id;
    int                         message_id;

    int                         message_specific_payload_len;
    void*                       message_specific_payload;

    int                         control_info;

    struct qmi_util_outstanding_request_info* next;
} qmi_util_outstanding_request_info;

static void qmi_util_event_pipe_do_get_event(qmi_util_event_pipe_info* event_pipe_info, qmi_util_event_info ** new_event);

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//static void qmi_util_inication_repost(int service_id, int message_id, void * message_payload, int message_payload_len);
//static void* qmi_util_requester_thread_func(void *param);
static void qmi_util_record_time(struct timeval * recorded_time);
//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void qmi_util_sync_object_init(qmi_util_sync_object_info* sync_object_info)
    {
    ASSERT(sync_object_info != NULL);
    if (sync_object_info)
        {
        memset(sync_object_info, 0, sizeof(*sync_object_info));

        pthread_mutexattr_init(&sync_object_info->mtx_atr);
        pthread_mutexattr_setpshared(&sync_object_info->mtx_atr, PTHREAD_PROCESS_SHARED);

        pthread_mutex_init(&sync_object_info->mutex, &sync_object_info->mtx_atr);

        pthread_cond_init(&sync_object_info->cond_var, NULL);
        }
    }

void qmi_util_sync_object_destroy(qmi_util_sync_object_info* sync_object_info)
    {
    ASSERT(sync_object_info != NULL);
    if (sync_object_info)
        {
        pthread_cond_destroy(&sync_object_info->cond_var);
        pthread_mutexattr_destroy(&sync_object_info->mtx_atr);
        pthread_mutex_destroy(&sync_object_info->mutex);
        }
    }

void qmi_util_sync_object_wait(qmi_util_sync_object_info* sync_object_info)
    {
    ASSERT(sync_object_info != NULL);
    if (sync_object_info)
        {
        pthread_mutex_lock(&sync_object_info->mutex);
        pthread_cond_wait(&sync_object_info->cond_var, &sync_object_info->mutex);
        pthread_mutex_unlock(&sync_object_info->mutex);
        }
    }

void qmi_util_sync_object_signal(qmi_util_sync_object_info* sync_object_info)
    {
    ASSERT(sync_object_info != NULL);
    if (sync_object_info)
        {
        pthread_mutex_lock(&sync_object_info->mutex);
        pthread_cond_signal(&sync_object_info->cond_var);
        pthread_mutex_unlock(&sync_object_info->mutex);
        }
    }

#ifdef FEATURE_QMI_ANDROID
//--------------------------------------------------------------------------------------------------------------------------------------------------------
int qmi_util_set_modem (char *modem)
    {
    if(strlen(modem) >= sizeof(qmi_client_util_port))
        {
        return 1;
        }

    if(0 == strcmp(modem, "local"))
        {
        strlcpy(qmi_client_util_port, QMI_PORT_RMNET_0, sizeof(qmi_client_util_port));
        }
    else if(0 == strcmp(modem, "remote"))
        {
        strlcpy(qmi_client_util_port, QMI_PORT_RMNET_SDIO_0, sizeof(qmi_client_util_port));
        }
    else if(0 == strcmp(modem, "default"))
        {
        char baseband[PROPERTY_VALUE_MAX];
        property_get("ro.baseband", baseband, "msm");
        if ((strcmp(baseband, "csfb") == 0) || (strcmp(baseband,"svlte2a") == 0))
            {
            // Fusion 9K available via sdio0
            strlcpy(qmi_client_util_port, QMI_PORT_RMNET_SDIO_0, sizeof(qmi_client_util_port));
            }
        else
            {
            // default modem available via smd0
            strlcpy(qmi_client_util_port, QMI_PORT_RMNET_0, sizeof(qmi_client_util_port));
            }
        }
    else if(0 == strncmp(modem, "rmnet", strlen("rmnet")))
        {
        strlcpy(qmi_client_util_port, modem, sizeof(qmi_client_util_port));
        }
    else
        {
        return 1;
        }
    return 0;
    }
#endif

void qmi_util_set_modem_port (const char *modem_port)
{
    strlcpy(qmi_client_util_port, modem_port, sizeof(qmi_client_util_port));
}


//--------------------------------------------------------------------------------------------------------------------------------------------------------
// --- event pipe ---
//--------------------------------------------------------------------------------------------------------------------------------------------------------
qmi_util_event_info* qmi_util_create_generic_event( int category_param )
{
    qmi_util_event_info* res = NULL;
    res = malloc( sizeof( *res ) );
    if ( res )
    {
        memset( res, 0, sizeof( *res ) );
        res->category = category_param;
    }
    return res;
}

void qmi_util_destroy_generic_event( qmi_util_event_info* event_info )
{
    if ( event_info )
    {
        free( event_info );
    }
}

void qmi_util_event_pipe_init_obj(qmi_util_event_pipe_info* event_pipe_info)
    {
    if (event_pipe_info)
        {
        memset(event_pipe_info, 0, sizeof(*event_pipe_info));

        pthread_mutexattr_init(&event_pipe_info->mtx_atr);
        pthread_mutexattr_setpshared(&event_pipe_info->mtx_atr, PTHREAD_PROCESS_PRIVATE);

        pthread_mutex_init(&event_pipe_info->event_list_guard, &event_pipe_info->mtx_atr);

        qmi_util_sync_object_init(&event_pipe_info->sync_object);
        }
    }

void qmi_util_event_pipe_destroy_obj(qmi_util_event_pipe_info* event_pipe_info)
    {
    if (event_pipe_info)
        {
        qmi_util_sync_object_destroy(&event_pipe_info->sync_object);
        pthread_mutexattr_destroy(&event_pipe_info->mtx_atr);
        pthread_mutex_destroy(&event_pipe_info->event_list_guard);
        }
    }

void qmi_util_event_pipe_post_event(qmi_util_event_pipe_info* event_pipe_info, qmi_util_event_info * new_event)
    {
    int alert;
    qmi_util_event_info * iter;
    if (event_pipe_info && new_event)
        {
        pthread_mutex_lock(&event_pipe_info->event_list_guard);

        alert = FALSE;
        iter = event_pipe_info->event_list;
        while ( iter != NULL && !alert)
        {
            if ( new_event == iter )
            {
                alert = TRUE;
            }
            else
            {
                iter = iter->next_event;
            }
        }
        if ( !alert )
        {
            new_event->next_event = event_pipe_info->event_list;
            event_pipe_info->event_list = new_event;
        }
        else
        {
            qmi_util_logln1("ALERT: qmi_util_event_pipe_post_event asked to loop", (int)new_event);
        }

        pthread_mutex_unlock(&event_pipe_info->event_list_guard);
        qmi_util_sync_object_signal(&event_pipe_info->sync_object);
        }
    }

void qmi_util_event_pipe_get_event(qmi_util_event_pipe_info* event_pipe_info, qmi_util_event_info ** new_event)
    {
    if (event_pipe_info && new_event)
        {
        pthread_mutex_lock(&event_pipe_info->event_list_guard);
        qmi_util_event_pipe_do_get_event(event_pipe_info, new_event);
        pthread_mutex_unlock(&event_pipe_info->event_list_guard);
        }
    }

void qmi_util_event_pipe_wait_for_event(qmi_util_event_pipe_info* event_pipe_info, qmi_util_event_info ** new_event)
{
    int already_fetched;
    if (event_pipe_info && new_event)
    {
        pthread_mutex_lock( &event_pipe_info->event_list_guard );

        already_fetched = (NULL != event_pipe_info->event_list) ? TRUE : FALSE;
        if ( already_fetched )
        {
            qmi_util_event_pipe_do_get_event(event_pipe_info, new_event);
        }
        pthread_mutex_unlock(&event_pipe_info->event_list_guard);
        if (!already_fetched)
            {
            qmi_util_sync_object_wait(&event_pipe_info->sync_object);
            qmi_util_event_pipe_get_event(event_pipe_info, new_event);
            }
    }
}

void qmi_util_event_pipe_do_get_event(qmi_util_event_pipe_info* event_pipe_info, qmi_util_event_info ** new_event)
{
    qmi_util_event_info * cur;
    qmi_util_event_info * prev;

    cur = event_pipe_info->event_list;

    if ( NULL != cur )
    {
        prev = NULL;
        while ( NULL != cur->next_event )
        {
            prev = cur;
            cur = cur->next_event;
        }
        *new_event = cur;
        if ( NULL != prev )
        {
            prev->next_event = NULL;
        }
        else
        { // no prev
            event_pipe_info->event_list = NULL;
        }

    }
    else
    {
        *new_event = NULL;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// --- strings ---
//--------------------------------------------------------------------------------------------------------------------------------------------------------
char* qmi_util_str_clone(char * str)
    {
    char* res = NULL;
    int len;
    if (str)
        {
        len = strlen(str) + 1;
        res = malloc(len);
        if (res)
            {
            memcpy(res, str, len);
            }
        }
    return res;
    }

void qmi_util_str_destroy(char * str)
{
    if (str)
    {
        free(str);
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
// --- logging ---
//--------------------------------------------------------------------------------------------------------------------------------------------------------
void qmi_util_log_enable ( int enable)
    {
    qmi_client_util_is_logging_enabled = enable;
    }

void qmi_util_logln0 ( char * str)
    {
    if (qmi_client_util_is_logging_enabled)
        {
        qmi_util_log("%s",str);
        }
    }

void qmi_util_logln1 ( char * str, int param1)
    {
    if (qmi_client_util_is_logging_enabled)
        {
        qmi_util_log("%s 1 - %d",str, param1);
        }
    }

void qmi_util_logln2 ( char * str, int param1, int param2)
    {
    if (qmi_client_util_is_logging_enabled)
        {
        qmi_util_log("%s 1 - %d, 2 - %d",str, param1, param2);
        }
    }

void qmi_util_logln1s ( char * str, char * param1)
    {
    char * input;
    if (qmi_client_util_is_logging_enabled)
        {
        input = (NULL != param1) ? param1 : "NULL";
        qmi_util_log("%s 1 - %s", str, input);
        }
    }

void qmi_util_logln2s ( char * str, char * param1, char * param2)
    {
    char * input;
    char * input2;
    if (qmi_client_util_is_logging_enabled)
        {
        input = (NULL != param1) ? param1 : "NULL";
        input2 = (NULL != param2) ? param2 : "NULL";
        qmi_util_log("%s 1 - %s 2 - %s", str, input, input2);
        }
    }

void qmi_util_log(const char * fmt, ...){
    time_t rawtime;
    struct tm * timeinfo;
    char szTime[20];
    FILE * file = qmi_simple_ril_suite_output_handle();

    if(NULL == file){
        file = stdout;
    }

    // get current time string
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    //08/23/01 14:55:02
    strftime(szTime, 20, "%x %X", timeinfo?timeinfo:"??/??/?? ??:??:??");

    pthread_mutex_lock( &log_mutex );
    va_list args;
    va_start(args, fmt);
    fprintf(file, "%s  ", szTime);
    //fprintf(file, "TAG: ");  //TODO add TAG?
    vfprintf(file, fmt, args);
    fprintf(file, "\n");
    fflush(file);

    va_end(args);
    pthread_mutex_unlock( &log_mutex );
}

void qmi_util_record_time(struct timeval * recorded_time)
{
    if(recorded_time)
    {
        gettimeofday(recorded_time,NULL);
    }
}

long long qmi_util_calculate_time_difference(struct timeval * start_time,struct timeval * end_time)
{
    struct timeval difference_time;

    memset(&difference_time,0,sizeof(difference_time));
    if(start_time && end_time)
    {
      difference_time.tv_sec  = end_time->tv_sec - start_time->tv_sec ;
      difference_time.tv_usec = end_time->tv_usec - start_time->tv_usec;

      if(difference_time.tv_usec<0)
      {
        difference_time.tv_usec += 1000000;
        difference_time.tv_sec -= 1;
      }
    }
    return 1000000LL*difference_time.tv_sec+ difference_time.tv_usec;
}

size_t qmi_util_strlcpy(char *dst, char *src, size_t size)
{
  int i = 0;

  if (size)
  {
    for (i  = 0; i < size; i++)
    {
      dst[i] = src[i];
      if (src[i] == 0)
      {
        break;
      }
    }
  }

  if ( i < strlen(src) )
  {
    for(; src[i] != 0; i++) /*EMPTY*/;
  }

  return i;
}

