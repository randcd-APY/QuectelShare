/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include <stdio.h>
#include <unistd.h>

#include "connx_ipc.h"
#include "connx_util.h"
#include "connx_callback.h"
#include "connx_log.h"
#include "connx_common_def.h"
#include "connx_socket_client.h"
#include "connx_socket_server.h"

typedef ConnxResult(* ConnxSocketFuncOpen)(ConnxSocketRegisterInfo *registerInfo, ConnxSocketHandle *socketHandle);

typedef ConnxResult(* ConnxSocketFuncClose)(ConnxSocketHandle socketHandle);

typedef ConnxResult(* ConnxSocketFuncSend)(ConnxSocketHandle socketHandle, const void *buf, size_t len, size_t *bytesSent);

typedef ConnxContext(* ConnxSocketFuncGetContext)(ConnxSocketHandle socketHandle);

#define GET_SOCKET_ROLE(flag)        ((flag) & IPC_FLAG_MASK_BITS_SOCKET_ROLE)

typedef struct
{
    ConnxSocketFuncOpen          open;
    ConnxSocketFuncClose         close;
    ConnxSocketFuncSend          send;
    ConnxSocketFuncGetContext    getContext;
} ConnxIpcSocketFuncTab;

typedef struct
{
    size_t                    size;
    uint16_t                  type;
    uint32_t                  flag;
} ConnxIpcSocketInstance;

static const ConnxIpcSocketFuncTab ipcSocketFuncTable[MAX_SOCKET_ROLE] =
{
    {ConnxSocketServerOpen, ConnxSocketServerClose, ConnxSocketServerSend, ConnxSocketServerGetContext},
    {ConnxSocketClientOpen, ConnxSocketClientClose, ConnxSocketClientSend, ConnxSocketClientGetContext}
};

static int count;

static void IPC_SocketDataInd(ConnxSocketHandle socketHandle, ConnxContext context, void *data, size_t size);
static void IPC_SocketClose(ConnxSocketHandle socketHandle, ConnxContext context);
static void IPC_SocketAccept(ConnxSocketHandle socketHandle, ConnxContext context);


static void IPC_SocketInitRegisterInfo(ConnxSocketRegisterInfo *registerInfo, ConnxContext context, char *name);


static void IPC_SocketDataInd(ConnxSocketHandle socketHandle, ConnxContext context, void *data, size_t size)
{
    ConnxHandle handle = (ConnxHandle) socketHandle;
    ConnxIpcRegisterInfo *registerInfo = (ConnxIpcRegisterInfo *) context;

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> socketHandle: %p, context: %p, data: %p, size: %d"), __FUNCTION__, socketHandle, context, data, size));

    if (!socketHandle || !context || !data || !size)
    {
        return;
    }

    CONNX_CALLBACK(registerInfo, dataIndCb, handle, context, data, size);
}

static void IPC_SocketClose(ConnxSocketHandle socketHandle, ConnxContext context)
{
    ConnxHandle handle = (ConnxHandle) socketHandle;
    ConnxIpcRegisterInfo *registerInfo = (ConnxIpcRegisterInfo *) context;

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> socketHandle: %p, context: %p"), __FUNCTION__, socketHandle, context));

    if (!socketHandle)
    {
        return;
    }

    CONNX_CALLBACK(registerInfo, closeCb, handle, context);
}

static void IPC_SocketAccept(ConnxSocketHandle socketHandle, ConnxContext context)
{
    ConnxHandle handle = (ConnxHandle) socketHandle;
    ConnxIpcRegisterInfo *registerInfo = (ConnxIpcRegisterInfo *) context;

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> socketHandle: %p, context: %p"), __FUNCTION__, socketHandle, context));

    if (!socketHandle)
    {
        return;
    }

    CONNX_CALLBACK(registerInfo, reservedCb, handle, context);
}

static void IPC_SocketInitRegisterInfo(ConnxSocketRegisterInfo *registerInfo, ConnxContext context, char *name)
{
    ConnxSocketInitRegisterInfo(registerInfo,
                                context,
                                name,
                                IPC_SocketDataInd,
                                IPC_SocketClose,
                                IPC_SocketAccept);
}

/* --------------------------------------------------------------------------------------- */

ConnxHandle ConnxIpcSocketOpen(ConnxContext context)
{
    ConnxSocketHandle socketHandle = (ConnxSocketHandle)malloc(sizeof(ConnxSocketHandle));
    ConnxIpcRegisterInfo *tempRegisterInfo = (ConnxIpcRegisterInfo *) context;
    ConnxIpcRegisterInfo *registerInfo = NULL;
    ConnxSocketRegisterInfo socketRegisterInfo;
    ConnxSocketRole role;
    bool res = false;

    count = 0;
    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> context: %p"), __FUNCTION__, context));

    if (!ConnxIpcValildRegisterInfo(tempRegisterInfo))
    {
        return NULL;
    }

    /* [QTI] Fix KW issue#266965. */
    memset(&socketRegisterInfo, 0, sizeof(ConnxSocketRegisterInfo));

    do
    {
        IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Duplicate IPC register info"), __FUNCTION__));

        registerInfo = ConnxIpcDuplicateRegisterInfo(tempRegisterInfo);

        if (!registerInfo)
        {
            break;
        }

        /* Switch here dbus and socket has different process */
        IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Init socket register info"), __FUNCTION__));

        IPC_SocketInitRegisterInfo(&socketRegisterInfo,
                                   (ConnxContext) registerInfo,
                                   registerInfo->name);

        IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Open socket"), __FUNCTION__));

        role = GET_SOCKET_ROLE(registerInfo->flag);
     
        if (!IS_SUCCESS(ipcSocketFuncTable[role].open(&socketRegisterInfo, &socketHandle)))
        {
            IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Open socket fail"), __FUNCTION__));
            break;
        }

        res = true;
    }
    while (0);

    if (socketRegisterInfo.pathName != NULL)
    {
        free(socketRegisterInfo.pathName);
        socketRegisterInfo.pathName = NULL;
    }

    if (!res)
    {
        ConnxIpcFreeRegisterInfo(registerInfo);
        registerInfo = NULL;
    }

    /* [MDM] It's a workaround for an unsolved segmentation fault, it is hard to trace even the backtrace is introduced */
    while (1)
    {
        usleep(1000);

        if (count++ == 1)
            break;
    }

    return res ? (ConnxHandle) socketHandle : NULL;
}

void ConnxIpcSocketClose(ConnxHandle handle)
{
    ConnxSocketHandle socketHandle = (ConnxSocketHandle) handle;
    ConnxIpcSocketInstance *inst = (ConnxIpcSocketInstance *)handle;
    ConnxIpcRegisterInfo *registerInfo = NULL;
	ConnxSocketRole role;

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> socketHandle: %p"), __FUNCTION__, socketHandle));

    if (!socketHandle || !inst)
    {
        return;
    }

    role = GET_SOCKET_ROLE(inst->flag);

    registerInfo = ipcSocketFuncTable[role].getContext(socketHandle);

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> registerInfo: %p"), __FUNCTION__, registerInfo));

    if (registerInfo != NULL)
    {
        ConnxIpcFreeRegisterInfo(registerInfo);
        registerInfo = NULL;
    }

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> ConnxSocketClientClose, socketHandle: %p"), __FUNCTION__, socketHandle));

    ipcSocketFuncTable[role].close(socketHandle);
}

int32_t ConnxIpcSocketDataSend(ConnxHandle handle, void *data, size_t size)
{
    ConnxSocketHandle socketHandle = (ConnxSocketHandle) handle;
    ConnxIpcSocketInstance *inst = (ConnxIpcSocketInstance *)handle;
	ConnxSocketRole role;
    size_t bytesSent = 0;

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> handle: %p, data: %p, size: %d"), __FUNCTION__, handle, data, size));

    if (!socketHandle || !data || !size || !inst)
    {
        return -1;
    }

	role = GET_SOCKET_ROLE(inst->flag);

    return IS_SUCCESS(ipcSocketFuncTable[role].send(socketHandle, data, size, &bytesSent)) ? (int32_t) bytesSent : -1;
}

ConnxContext ConnxIpcSocketGetContext(ConnxHandle handle)
{
    ConnxSocketHandle socketHandle = (ConnxSocketHandle) handle;
    ConnxIpcSocketInstance *inst = (ConnxIpcSocketInstance *)handle;
    ConnxSocketRole role;
    ConnxIpcRegisterInfo *ipcRegisterInfo = NULL;
    
    role = GET_SOCKET_ROLE(inst->flag);
    ipcRegisterInfo = (ConnxIpcRegisterInfo *) ipcSocketFuncTable[role].getContext(socketHandle);
    
    return (ipcRegisterInfo != NULL) ? ipcRegisterInfo->appContext : NULL;
}

