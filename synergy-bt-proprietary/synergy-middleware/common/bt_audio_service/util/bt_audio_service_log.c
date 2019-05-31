/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "connx_log_setting.h"
#include "bt_audio_service_main.h"

void InitBtAudioServiceLog(void)
{
    BtAudioServiceInstance *inst = BasGetMainInstance();
    ConnxLogSetting *logSetting = &inst->logSetting;

    ConnxInitLog(BT_LOG_CONFIG_PATH, BT_LOG_CONFIG_FILE, BT_LOG_FILE, logSetting);
}

void DeinitBtAudioServiceLog(void)
{
    ConnxDeinitLog();
}

