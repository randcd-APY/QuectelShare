/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#include "connx_socket.h"
#include "connx_util.h"
#include "connx_common_def.h"
#include "connx_log.h"


bool ConnxSocketInitRegisterInfo(ConnxSocketRegisterInfo *registerInfo, ConnxContext context, char *name,
                                 ConnxSocketCbDataInd socketDataIndCb, ConnxSocketCbClose socketCloseCb,
                                 ConnxSocketCbAccept socketAcceptCb)
{
    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> "), __FUNCTION__));

    if (!registerInfo || !name)
    {
        IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Invalid parameter"), __FUNCTION__));
        return false;
    }

    memset(registerInfo, 0, sizeof(ConnxSocketRegisterInfo));

    registerInfo->size            = sizeof(ConnxSocketRegisterInfo);
    registerInfo->context         = context;
    registerInfo->pathName        = ConnxStrDup(name);
    registerInfo->socketDataIndCb = socketDataIndCb;
    registerInfo->socketCloseCb   = socketCloseCb;
    registerInfo->socketAcceptCb  = socketAcceptCb;

    return true;
}

char *ConnxSocketCreatePath(char *pathName, char *serverName, char *clientName)
{
    char fullPath[MAX_BUF_SIZE];

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> "), __FUNCTION__));

    if (!pathName || !serverName || !clientName)
    {
        IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Invalid parameter"), __FUNCTION__));
        return NULL;
    }

    /* E.g. "/data/synergy/ConnxSocketSynergyDaemon-SynergyService" */
    snprintf(fullPath, sizeof(fullPath), "%s%s%s%s%s", pathName, CONNX_SOCKET, serverName, CONNX_HYPHEN, clientName);

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> full path: '%s' "), __FUNCTION__, fullPath));

    return ConnxStrDup(fullPath);
}

ConnxResult ConnxSocketSend(int32_t sock_fd, const void *buf, size_t len, size_t *bytesSent)
{
    int32_t ret = 0;
    int32_t flags = 0;

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> sock_fd: %d, buf: %p, len: %d"), __FUNCTION__, sock_fd, buf, len));

    if (!IS_VALID_CONNX_SOCKET(sock_fd) || !buf || !len)
    {
        IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Invalid parameter"), __FUNCTION__));
        return CONNX_SOCKET_RESULT_INVALID_PARAMETER;
    }

    IFLOG(DumpBuff(DEBUG_IPC_MESSAGE, (const uint8_t *) buf, (uint32_t) len));

    ret = send(sock_fd, buf, len, flags);

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> bytes sent: %d"), __FUNCTION__, ret));

    if (bytesSent)
    {
        *bytesSent = ret;
    }

    return (ret > 0) ? CONNX_RESULT_SUCCESS : CONNX_SOCKET_RESULT_INTERNAL_ERROR;
}

ConnxResult ConnxSocketClose(int32_t sock_fd)
{
    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> sock_fd: %d"), __FUNCTION__, sock_fd));

    if (!IS_VALID_CONNX_SOCKET(sock_fd))
    {
        IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Invalid parameter"), __FUNCTION__));
        return CONNX_SOCKET_RESULT_INVALID_PARAMETER;
    }

    close(sock_fd);

    return CONNX_RESULT_SUCCESS;
}

ConnxResult ConnxSocketShutdown(int32_t sock_fd)
{
    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> sock_fd: %d"), __FUNCTION__, sock_fd));

    if (!IS_VALID_CONNX_SOCKET(sock_fd))
    {
        IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Invalid parameter"), __FUNCTION__));
        return CONNX_SOCKET_RESULT_INVALID_PARAMETER;
    }

    shutdown(sock_fd, SHUT_RDWR);

    close(sock_fd);

    return CONNX_RESULT_SUCCESS;
}
