/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifndef _CONNX_LOG_SETTING_H_
#define _CONNX_LOG_SETTING_H_

#include "connx_common_def.h"

#ifdef __cplusplus
extern "C" {
#endif


#define BT_LOG_CONFIG_PATH      "/etc/bluetooth"

#define BT_LOG_CONFIG_FILE      "bt_log.conf"

#define BT_LOG_PATH             "/tmp"

#define BT_LOG_FILE             "bt.log"


typedef struct
{
    bool        enable_log;     /* true: Enable log, false: Disable */
    uint32_t    output_mode;    /* Debug message output mode. */
    uint32_t    output_mask;    /* Debug zone for output message. */
    char       *log_path;       /* Path to store log file. */
    bool        enable_btsnoop; /* true: enable btsnoop log, false: not. */
} ConnxLogSetting;


bool ConnxInitLog(char *config_path, char *config_file, char *log_file, ConnxLogSetting *logSetting);

void ConnxDeinitLog(void);


#ifdef __cplusplus
}
#endif

#endif  /* _CONNX_LOG_SETTING_H_ */