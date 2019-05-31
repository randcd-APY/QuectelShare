/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "connx_ipc_common.h"
#include "connx_util.h"


bool ConnxIpcValildRegisterInfo(ConnxIpcRegisterInfo *registerInfo)
{
    return (registerInfo &&
            (registerInfo->size == sizeof(ConnxIpcRegisterInfo)) &&
            registerInfo->name) ? true : false;
}

ConnxIpcRegisterInfo *ConnxIpcDuplicateRegisterInfo(ConnxIpcRegisterInfo *registerInfo)
{
    ConnxIpcRegisterInfo *tempRegisterInfo = NULL;

    if (!registerInfo)
    {
        return NULL;
    }

    tempRegisterInfo = (ConnxIpcRegisterInfo *) malloc(sizeof(ConnxIpcRegisterInfo));

    if (!tempRegisterInfo)
    {
        return NULL;
    }

    memcpy(tempRegisterInfo, registerInfo, sizeof(ConnxIpcRegisterInfo));

    tempRegisterInfo->name   = ConnxStrDup(registerInfo->name);

    return tempRegisterInfo;
}

void ConnxIpcFreeRegisterInfo(ConnxIpcRegisterInfo *registerInfo)
{
    if (!registerInfo)
    {
        return;
    }

    if (registerInfo->name != NULL)
    {
        free(registerInfo->name);
        registerInfo->name = NULL;
    }

    free(registerInfo);
}
