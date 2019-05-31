/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#define _POSIX_C_SOURCE 200112L

#include <time.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include "connx_util.h"


typedef struct
{
    pthread_cond_t  event;
    pthread_mutex_t mutex;
    uint32_t        eventBits;
} ConnxEvent;


/* -------------------------------------------------------- */

/* Thread API */
ConnxHandle ConnxThreadCreate(void (*threadFunction)(void *pointer),
                              void *pointer,
                              uint32_t stackSize,
                              uint16_t priority,
                              const char *threadName)
{
    pthread_attr_t threadAttr;
    pthread_t *threadHandle = NULL;
    bool res = false;

    CONNX_UNUSED(stackSize);
    CONNX_UNUSED(priority);
    CONNX_UNUSED(threadName);

    if (threadFunction == NULL)
    {
        /* Invalid parameter. */
        return NULL;
    }

    threadHandle = (pthread_t *) malloc(sizeof(pthread_t));

    if (!threadHandle)
        return NULL;

    do
    {
        if (pthread_attr_init(&threadAttr) != 0)
        {
            /* Fail to init thread's attr. */
            break;
        }

        if (pthread_create(threadHandle, &threadAttr, (void * ( *)(void *))threadFunction, pointer) != 0)
        {
            /* Fail to create thread. */
            break;
        }

        pthread_detach(*threadHandle);

        res = true;
    }
    while (0);

    if (!res)
    {
        free(threadHandle);
        threadHandle = NULL;
    }

    return res ? (ConnxHandle) threadHandle : NULL;
}

void ConnxThreadDestroy(ConnxHandle threadHandle)
{
    free(threadHandle);
}

void ConnxThreadSleep(uint16_t sleepTimeInMs)
{
    struct timespec ts;
    uint16_t seconds = (sleepTimeInMs / 1000U);
    uint32_t nanoseconds = (sleepTimeInMs - seconds * 1000U) * 1000000U;

    ts.tv_sec = (time_t) seconds;
    ts.tv_nsec = (long) nanoseconds;

    (void) nanosleep(&ts, NULL);
}

/* Event API */
ConnxHandle ConnxEventCreate()
{
    ConnxEvent *eventHandle = NULL;
    bool res = false;

    eventHandle = (ConnxEvent *) malloc(sizeof(ConnxEvent));

    if (!eventHandle)
        return NULL;

    memset(eventHandle, 0, sizeof(ConnxEvent));

    do
    {
        if (pthread_mutex_init(&(eventHandle->mutex), NULL) != 0)
            break;

        if (pthread_cond_init(&(eventHandle->event), NULL) != 0)
            break;

        eventHandle->eventBits = 0;

        res = true;
    }
    while (0);

    if (!res)
    {
        free(eventHandle);
        eventHandle = NULL;
    }

    return res ? (ConnxHandle) eventHandle : NULL;
}

ConnxResult ConnxEventWait(ConnxHandle eventHandle, uint16_t timeoutInMs, uint32_t *eventBits)
{
    ConnxEvent *tempHandle = (ConnxEvent *) eventHandle;
    struct timespec ts;
    ConnxResult result;
    int rc = 0;

    if (eventHandle == NULL)
    {
        return CONNX_RESULT_INVALID_HANDLE;
    }

    if (eventBits == NULL)
    {
        return CONNX_RESULT_INVALID_POINTER;
    }

    (void) pthread_mutex_lock(&(tempHandle->mutex));

    if ((tempHandle->eventBits == 0) && (timeoutInMs != 0))
    {
        if (timeoutInMs != CONNX_EVENT_WAIT_INFINITE)
        {
            time_t sec;

            /* Get time through "CLOCK_REALTIME", instead of "CLOCK_MONOTONIC". */
            (void) clock_gettime(CLOCK_REALTIME, &ts);

            sec = timeoutInMs / 1000;
            ts.tv_sec = ts.tv_sec + sec;
            ts.tv_nsec = ts.tv_nsec + (timeoutInMs - sec * 1000) * 1000000;

            if (ts.tv_nsec >= 1000000000L)
            {
                ts.tv_nsec -= 1000000000L;
                ts.tv_sec++;
            }

            while (tempHandle->eventBits == 0 && rc == 0)
            {
#ifndef BUILD_FOR_ANDROID
                rc = pthread_cond_timedwait(&(tempHandle->event), &(tempHandle->mutex), &ts);
#else
                rc = pthread_cond_timedwait_monotonic_np(&(tempHandle->event),
                                                         &(tempHandle->mutex), &ts);
#endif
            }
        }
        else
        {
            while (tempHandle->eventBits == 0 && rc == 0)
            {
                rc = pthread_cond_wait(&(tempHandle->event), &(tempHandle->mutex));
            }
        }
    }

    result = (tempHandle->eventBits == 0) ? CONNX_RESULT_TIMEOUT : CONNX_RESULT_SUCCESS;
    /* Indicate to caller which events were triggered and cleared */
    *eventBits = tempHandle->eventBits;
    /* Clear triggered events */
    tempHandle->eventBits = 0;
    (void) pthread_mutex_unlock(&(tempHandle->mutex));

    return result;
}

void ConnxEventSet(ConnxHandle eventHandle, uint32_t eventBits)
{
    ConnxEvent *tempHandle = (ConnxEvent *) eventHandle;

    if (eventHandle == NULL)
        return;

    (void) pthread_mutex_lock(&(tempHandle->mutex));

    tempHandle->eventBits |= eventBits;

    (void) pthread_cond_signal(&(tempHandle->event));

    (void) pthread_mutex_unlock(&(tempHandle->mutex));
}

void ConnxEventDestroy(ConnxHandle eventHandle)
{
    ConnxEvent *tempHandle = (ConnxEvent *) eventHandle;

    if (eventHandle == NULL)
        return;

    (void) pthread_cond_destroy(&(tempHandle->event));

    free(eventHandle);
}

/* Mutex API */
ConnxHandle ConnxMutexCreate()
{
    pthread_mutex_t *mutexHandle = NULL;

    mutexHandle = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));

    if (!mutexHandle)
        return NULL;

    if (pthread_mutex_init(mutexHandle, NULL) != 0)
    {
        /* Error occur during initing mutex. */
        free(mutexHandle);
        return NULL;
    }

    return (ConnxHandle) mutexHandle;
}

void ConnxMutexLock(ConnxHandle mutexHandle)
{
    if (!mutexHandle)
        return;

    pthread_mutex_lock((pthread_mutex_t *) mutexHandle);
}

void ConnxMutexUnlock(ConnxHandle mutexHandle)
{
    if (!mutexHandle)
        return;

    pthread_mutex_unlock((pthread_mutex_t *) mutexHandle);
}

void ConnxMutexDestroy(ConnxHandle mutexHandle)
{
    if (!mutexHandle)
        return;

    pthread_mutex_destroy((pthread_mutex_t *) mutexHandle);

    free(mutexHandle);
}

bool ConnxScriptLaunch(const char *script, const char *cmd)
{
    char str[CONNX_MAX_PATH_LENGTH];

    if (!script || !cmd)
        return false;

    /* [QTI] Fix KW issue#2247126 through using the 'str' in array. */
    snprintf(str, sizeof(str), "%s %s", script, cmd);

    system(str);

    return true;
}

/* Convert string with base 10 to integer */
uint32_t ConnxStrToInt(const char *str)
{
    int16_t i;
    uint32_t res;
    uint32_t digit;

    res = 0;
    digit = 1;

    /* Start from the string end */
    for (i = (uint16_t) (strlen(str) - 1); i >= 0; i--)
    {
        /* Only convert numbers */
        if ((str[i] >= '0') && (str[i] <= '9'))
        {
            res += digit * (str[i] - '0');
            digit = digit * 10;
        }
    }

    return res;
}

char *ConnxStrDup(const char *str)
{
    size_t length = 0;
    char *dst = NULL;

    if (!str)
        return NULL;

    length = strlen(str) + 1;

    dst = (char *)malloc(length);

    if (!dst)
        return NULL;

    ConnxStrLCpy(dst, str, length);

    return dst;
}

char *ConnxStrNCpyZero(char *dst, const char *src, size_t count)
{
    ConnxStrLCpy(dst, src, count);

    return dst;
}

size_t ConnxStrLCpy(char *dst, const char *src, size_t size)
{
    size_t src_len = 0;
    size_t actual_len = 0;

    if (!dst || !src || !size)
        return 0;

    src_len = strlen(src);
    
    actual_len = CONNX_MIN(src_len, size - 1);

    if (actual_len)
    {
        memcpy(dst, src, actual_len);
    }

    dst[actual_len] = '\0';

    /* Return the src length, instead of actual length copied. */
    return src_len;
}

size_t ConnxStrLCat(char *dst, const char *src, size_t size)
{
    size_t dst_len = 0;
    size_t src_len = 0;
    size_t actual_len = 0;

    if (!dst || !src || !size)
        return 0;

    dst_len = strlen(dst);

    src_len = strlen(src);

    if (dst_len + 1 >= size)
        return src_len + dst_len;

    actual_len = CONNX_MIN(src_len, size - dst_len - 1);

    if (actual_len)
    {
        char *tmp_ptr = dst + dst_len;
        memcpy(tmp_ptr, src, actual_len);
    }

    dst[dst_len + actual_len] = '\0';

    return src_len + dst_len;
}

char *ConnxCreateFullFileName(const char *path, const char *file_name)
{
    char *tmp_str = NULL;
    size_t size = CONNX_MAX_PATH_LENGTH;

    if (!path || !file_name)
        return NULL;

    tmp_str = (char *)malloc(size);

    if (!tmp_str)
        return NULL;

    snprintf(tmp_str, size, "%s/%s", path, file_name);

    return tmp_str;
}
