/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/
#include <stdio.h>

#include "bt_audio_service.h"
#include "bt_audio_service_main.h"
#include "bt_audio_service_sef.h"
#include "bt_audio_hw.h"

#include "connx_ipc_common.h"
#include "connx_arg_search.h"
#include "connx_log.h"
#include "connx_util.h"

static BtAudioServiceInstance basInst =
{
    .init = false,
};

BtAudioServiceInstance *BasGetMainInstance()
{
    return &basInst;
}

static bool InitRegisterInfo(BasRegisterInfo *registerInfo)
{
    if (!registerInfo)
        return false;

    memset(registerInfo, 0, sizeof(BasRegisterInfo));

    registerInfo->size = sizeof(BasRegisterInfo);

    return true;
}

int main(int argc, char *argv[])
{
    BtAudioServiceInstance *inst = BasGetMainInstance();
    BasRegisterInfo registerInfo;
    uint32_t eventBits = 0;

    ConnxArgSearchInit((uint32_t) argc, (char **) argv);
    
    InitRegisterInfo(&registerInfo);
    
    InitPlatformDeviceInfo();
    
    InitBtAudioServiceLog();
    
    BtAudioServiceOpen(&registerInfo);
    
    /* Block infinitely. */
    ConnxEventWait(BAS_GET_EVENT_HANDLE(inst), CONNX_EVENT_WAIT_INFINITE, &eventBits);

    BtAudioServiceClose();
    
    DeinitBtAudioServiceLog();

    return 0;
}

