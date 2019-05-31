/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "connx_log_setting.h"
#include "bt_bootstrap_util.h"


void InitBtBootstrapLog(void)
{
    BtBootstrapInstance *inst = GetBtBootstrapInstance();
    BtBootstrapRegisterInfo *registerInfo = &inst->registerInfo;
    ConnxLogSetting *logSetting = &inst->logSetting;

    ConnxInitLog(registerInfo->config_path,
                 BT_LOG_CONFIG_FILE,
                 BT_BOOTSTRAP_LOG_FILE_NAME,
                 logSetting);
}

void DeinitBtBootstrapLog(void)
{
    ConnxDeinitLog();
}
