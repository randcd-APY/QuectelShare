/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "iotd_context.h"

#define DEFAULT_MAX_BUF_COUNT (20)

uint16_t buf_count = 0;
uint16_t buf_max_num = DEFAULT_MAX_BUF_COUNT;

pthread_mutex_t pool_lock = PTHREAD_MUTEX_INITIALIZER;

int bufpool_alloc(void* pCxt)
{
    return IOTD_OK;
}

int bufpool_free(void* pCxt)
{
    return IOTD_OK;
}

int buf_init(void* pCxt)
{
    IOTD_CXT_T* iotdCxt = (IOTD_CXT_T*)pCxt;
    cfg_ini* cfg;

    if(iotdCxt == NULL)
    {
        return IOTD_ERROR;
    }

    pthread_mutex_lock(&pool_lock);
    buf_count = 0;   
    cfg = &(iotdCxt->cfg);    
    buf_max_num = cfg->config_system.num_buffer;
    pthread_mutex_unlock(&pool_lock);
    
    return IOTD_OK;
}

void* buf_alloc(uint32_t size)
{
    char *buf = NULL;
    
    if(buf_count < buf_max_num){
        pthread_mutex_lock(&pool_lock);
        buf_count++;
        pthread_mutex_unlock(&pool_lock);
        buf = (char *)malloc(size);
        memset(buf, 0, size);
        return buf;
    }else
        return NULL;

}

void buf_free(void* buf)
{
    pthread_mutex_lock(&pool_lock);
    buf_count--;
    pthread_mutex_unlock(&pool_lock);
    free(buf);
}
