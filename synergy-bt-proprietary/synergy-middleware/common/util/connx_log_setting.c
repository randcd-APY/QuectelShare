/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "connx_log_setting.h"
#include "connx_setting.h"
#include "connx_log.h"
#include "connx_util.h"


#define MAX_LOG_SETTING_SIZE            2048

#define LOG_SETTING_FORMAT              "%s = %x \n\n"
#define LOG_SETTING_VAL_STR_FORMAT      "%s = %s \n\n"

#define LOG_SETTING_KEY_ENABLE_LOG      "enable_log"
#define LOG_SETTING_KEY_OUTPUT_MODE     "output_mode"
#define LOG_SETTING_KEY_OUTPUT_MASK     "output_mask"
#define LOG_SETTING_KEY_LOG_PATH        "log_path"
#define LOG_SETTING_KEY_ENABLE_BTSNOOP  "enable_btsnoop"

#define ENABLE_LOG_COMMENT              CONNX_COMMENT "0: Disable log, 1: Enable" CONNX_NEW_LINE
#define OUTPUT_MODE_COMMENT             CONNX_COMMENT "0: No debug message output, 1: Output through console, 2: Output into file" CONNX_NEW_LINE
#define OUTPUT_MASK_COMMENT             CONNX_COMMENT "Debug zone to control output message " CONNX_NEW_LINE
#define LOG_PATH_COMMENT                CONNX_COMMENT "Path to store log file" CONNX_NEW_LINE
#define ENABLE_BTSNOOP_COMMENT          CONNX_COMMENT "0: Disable BT snoop log, 1: Enable" CONNX_NEW_LINE

#define LOG_SETTING_FILE_FORMAT         ENABLE_LOG_COMMENT \
                                        LOG_SETTING_FORMAT \
                                        OUTPUT_MODE_COMMENT \
                                        LOG_SETTING_FORMAT \
                                        OUTPUT_MASK_COMMENT \
                                        LOG_SETTING_FORMAT \
                                        LOG_PATH_COMMENT \
                                        LOG_SETTING_VAL_STR_FORMAT \
                                        ENABLE_BTSNOOP_COMMENT \
                                        LOG_SETTING_FORMAT


static void ParseLogSetting(ConnxContext context, char *key, char *val)
{
    ConnxLogSetting *logSetting = (ConnxLogSetting *)context;
    uint32_t tmp_val = 0;

    if (!context || !key || !val)
        return;

    if (!strcmp(key, LOG_SETTING_KEY_ENABLE_LOG))
    {
        if (ConnxSettingGetUint32(val, &tmp_val))
        {
            logSetting->enable_log = tmp_val ? true : false;
        }
    }
    else if (!strcmp(key, LOG_SETTING_KEY_OUTPUT_MODE))
    {
        if (ConnxSettingGetUint32(val, &tmp_val))
        {
            logSetting->output_mode = tmp_val;
        }
    }
    else if (!strcmp(key, LOG_SETTING_KEY_OUTPUT_MASK))
    {
        if (ConnxSettingGetUint32(val, &tmp_val))
        {
            logSetting->output_mask = tmp_val;
        }
    }
    else if (!strcmp(key, LOG_SETTING_KEY_LOG_PATH))
    {
        logSetting->log_path = ConnxStrDup(val);
    }
    else if (!strcmp(key, LOG_SETTING_KEY_ENABLE_BTSNOOP))
    {
        if (ConnxSettingGetUint32(val, &tmp_val))
        {
            logSetting->enable_btsnoop = tmp_val ? true : false;
        }
    }
    else
    {
        /* Unknown property. */
    }
}

static void GetDefaultLogSetting(ConnxLogSetting *logSetting)
{
    if (!logSetting)
        return;

    logSetting->enable_log     = false;
    logSetting->output_mode    = DEFAULT_OUTPUT_MODE;
    logSetting->output_mask    = DEFAULT_OUTPUT_MASK;
    logSetting->log_path       = BT_LOG_PATH;
    logSetting->enable_btsnoop = true;
}

static char *CreateLogSettingString(ConnxLogSetting *logSetting)
{
    char *total_str = NULL;

    if (!logSetting)
        return NULL;

    total_str = (char *)malloc(MAX_LOG_SETTING_SIZE);

    if (!total_str)
        return NULL;

    memset(total_str, 0, MAX_LOG_SETTING_SIZE);

    snprintf(total_str, MAX_LOG_SETTING_SIZE, LOG_SETTING_FILE_FORMAT,
            LOG_SETTING_KEY_ENABLE_LOG, logSetting->enable_log,
            LOG_SETTING_KEY_OUTPUT_MODE, logSetting->output_mode,
            LOG_SETTING_KEY_OUTPUT_MASK, logSetting->output_mask,
            LOG_SETTING_KEY_LOG_PATH, logSetting->log_path,
            LOG_SETTING_KEY_ENABLE_BTSNOOP, logSetting->enable_btsnoop);

    return total_str;
}

static bool ReadLogSetting(char *file_name, ConnxLogSetting *logSetting)
{
    ConnxSettingRegisterInfo registerInfo;
    ConnxSettingRegisterInfo *ri = &registerInfo;
    ConnxHandle handle = NULL;
    ConnxLogSetting defaultLogSetting;
    char *defaultSetting = NULL;

    if (!file_name || !logSetting)
        return false;

    GetDefaultLogSetting(&defaultLogSetting);

    defaultSetting = CreateLogSettingString(&defaultLogSetting);

    ri->fileName       = file_name;
    ri->defaultSetting = defaultSetting;
    ri->context        = logSetting;
    ri->parseCb        = ParseLogSetting;

    handle = ConnxSettingOpen(ri);

    if (!handle)
    {
        free(defaultSetting);
        return false;
    }

    ConnxSettingParse(handle);

    ConnxSettingClose(handle);

    free(defaultSetting);

    return true;
}

static bool GetLogSetting(char *config_path, char *file_name, ConnxLogSetting *logSetting)
{
    char *full_file_name;

    if (!config_path || !file_name || !logSetting)
        return false;

    GetDefaultLogSetting(logSetting);

    full_file_name = ConnxCreateFullFileName(config_path, file_name);

    ReadLogSetting(full_file_name, logSetting);

    free(full_file_name);

    return true;
}

bool ConnxInitLog(char *config_path, char *config_file, char *log_file, ConnxLogSetting *logSetting)
{
    bool enable_log = false;
    uint32_t output_mode = 0;
    uint32_t output_mask = 0;
    char *output_file = NULL;

    if (!config_path || !config_file || !log_file || !logSetting)
        return false;

    GetLogSetting(config_path, config_file, logSetting);

    enable_log  = logSetting->enable_log;
    output_mode = logSetting->output_mode;
    output_mask = logSetting->output_mask;
    output_file = ConnxCreateFullFileName(logSetting->log_path, log_file);

    if (enable_log)
    {
        IFLOG(DebugInitialize(output_mode, output_mask, output_file));
    }

    IFLOG(DebugOut(DEBUG_OUTPUT, TEXT("<%s> enable_log: %x, output_mode: 0x%x, output_mask: 0x%x, log_path: %s"),
                   __FUNCTION__, enable_log, output_mode, output_mask, logSetting->log_path));

    free(output_file);

    return true;
}

void ConnxDeinitLog(void)
{
    IFLOG(DebugUninitialize());
}
