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
#include "iotd_context.h"
#include "iotd_dbg.h"

static int iotd_dbg_level =  LOG_LEVEL_DEFAULT;  

int32_t dbg_init(void* pCxt)
{
    IOTD_CXT_T* iotdCxt = (IOTD_CXT_T*)pCxt;
    cfg_ini* cfg;

    if(iotdCxt == NULL)
    {
        return IOTD_ERROR;
    }

    cfg = &(iotdCxt->cfg);
    iotd_dbg_level = cfg->config_system.dbg_lvl;

    return IOTD_OK;
}

int32_t dbg_deinit(void* pCxt)
{
    return IOTD_OK;
}

void iotd_log(int level, char* format, ...)
{
    va_list args;

    va_start(args, format);
#ifdef IOTD_SYSLOG
    vsyslog(LOG_INFO, format, args);
#endif
#ifdef DBG_STDOUT
    if (iotd_dbg_level >= level)
        vprintf(format,args);
#endif
    va_end(args);
}


