/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include <stdio.h>
#include <unistd.h>

#include "connx_ipc_common.h"
#include "connx_ipc_socket.h"
#include "connx_util.h"
#include "connx_callback.h"
#include "connx_log.h"

#define INST_GET_IPC_TYPE(inst)       ((inst)->type)

typedef struct
{
    size_t                    size;
    uint16_t                  type;
    uint32_t                  flag;
} ConnxIpcInstance;

ConnxHandle ConnxIpcOpen(ConnxContext context)
{
    ConnxIpcRegisterInfo *registerInfo = (ConnxIpcRegisterInfo *) context;

    if (!IS_VALID_IPC_TYPE(registerInfo->type))
    {
        return NULL;
    }

    return ConnxIpcSocketOpen(context);
}

void ConnxIpcClose(ConnxHandle handle)
{
    ConnxIpcInstance *inst = (ConnxIpcInstance *)handle;

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> handle: %p, inst: %p"), __FUNCTION__, handle, inst));

    if (!inst)
    {
        return;
    }

	ConnxIpcSocketClose(handle);
}

void ConnxIpcDataSend(ConnxHandle handle, void *data, size_t size)
{
    ConnxIpcInstance *inst = (ConnxIpcInstance *)handle;

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> handle: %p, data: %p, size: %d, inst: %p"), __FUNCTION__, handle, data, size, inst));

    if (!handle || !data || !size || !inst)
    {
        return;
    }

	ConnxIpcSocketDataSend(handle, data, size);
}

ConnxContext ConnxIpcGetContext(ConnxHandle handle)
{
    ConnxIpcRegisterInfo *ipcRegisterInfo = NULL;
    ConnxIpcInstance *inst = (ConnxIpcInstance *)handle;

    if (!inst)
    {
        return NULL;
    }

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> handle: %p, inst: %p"), __FUNCTION__, handle, inst));

    ipcRegisterInfo = ConnxIpcSocketGetContext(handle);

    return (ipcRegisterInfo != NULL) ? ipcRegisterInfo->appContext : NULL;
}

