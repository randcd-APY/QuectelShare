/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "bt_bootstrap.h"
#include "bt_bootstrap_util.h"
#include "connx_serial_com.h"
#ifndef EXCLUDE_BOOT_QCA
#include "boot_qca.h"
#endif
#ifndef EXCLUDE_BOOT_BLUECORE
#include "boot_bluecore.h"
#endif


static BtBootstrapInstance bootstrapInst;


BtBootstrapInstance *GetBtBootstrapInstance(void)
{
    return &bootstrapInst;
}

bool QcomBtBootstrap(BtBootstrapRegisterInfo *registerInfo)
{
    BtBootstrapInstance *inst = &bootstrapInst;
    BtBootstrapRegisterInfo *ri = &inst->registerInfo;
    uint16_t chipType = 0;
    ConnxResult result = 0;

    if (!IsValidRegisterInfo(registerInfo))
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Invalid register information"), __FUNCTION__));
        return false;
    }

    CpyRegisterInfo(ri, registerInfo);

    InitBtBootstrapLog();

    chipType = BRI_GET_CHIP_TYPE(ri);

    IFLOG(DebugOut(DEBUG_BT_OUTPUT, TEXT("<%s> BT chip type: 0x%x"), __FUNCTION__, chipType));

    if (IS_QCA_CHIP(chipType))
    {
#ifndef EXCLUDE_BOOT_QCA
        IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("<%s> Boot QCA chip"), __FUNCTION__));

        result = BootQca();
#endif
    }
    else if (IS_BLUECORE_CHIP(chipType))
    {
#ifndef EXCLUDE_BOOT_BLUECORE
        IFLOG(DebugOut(DEBUG_BT_MESSAGE, TEXT("<%s> Boot BlueCore chip"), __FUNCTION__));

        result = BootBlueCore();
#endif
    }
    else
    {
        IFLOG(DebugOut(DEBUG_BT_ERROR, TEXT("<%s> Invalid BT chip: 0x%x"), __FUNCTION__, chipType));

        result = CONNX_RESULT_SERVICE_UNSUPPORTED;
    }

    DeinitBtBootstrapLog();

    return IS_CONNX_RESULT_SUCCESS(result) ? true : false;
}
