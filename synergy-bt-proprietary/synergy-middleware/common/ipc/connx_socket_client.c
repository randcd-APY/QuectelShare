/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "connx_socket_client.h"
#include "connx_util.h"
#include "connx_callback.h"
#include "connx_common_def.h"
#include "connx_log.h"
#include "connx_ipc_debug.h"
#include "connx_ipc_common.h"


#define IS_VALID_CONNX_SOCKET_CLIENT(inst)    (((inst) != NULL) && ((inst)->size == sizeof(ConnxSocketClientInstance)) && (inst)->inUse)

#define SCI_GET_SIZE(p)                     ((p)->size)
#define SCI_GET_TYPE(p)                     ((p)->type)
#define SCI_GET_FLAG(p)                     ((p)->flag)
#define SCI_GET_IN_USE(p)                   ((p)->inUse)
#define SCI_GET_REGISTER_INFO(p)            ((p)->registerInfo)
#define SCI_GET_EVENT_HANDLE(p)             ((p)->eventHandle)
#define SCI_GET_MUTEX_HANDLE(p)             ((p)->mutexHandle)
#define SCI_GET_QUIT(p)                     ((p)->quit)
#define SCI_GET_BUFFER(p)                   ((p)->buffer)
#define SCI_GET_BUFFER_SIZE(p)              ((p)->bufferSize)
#define SCI_GET_THREAD_HANDLE(p)            ((p)->threadHandle)
#define SCI_GET_CLIENT_FD(p)                ((p)->socketClient)

#define SCI_LOCK(inst)                      ConnxMutexLock(SCI_GET_MUTEX_HANDLE(inst))
#define SCI_UNLOCK(inst)                    ConnxMutexUnlock(SCI_GET_MUTEX_HANDLE(inst))

/* Timeout in ms. */
#define MAX_CONNX_SOCKET_CLIENT_TIMEOUT       30000

#define CONNX_SOCKET_CLIENT_THREAD_NAME       "CONNX Socket Client Thread"

#define CONNX_SOCKET_CLIENT_START_CFM         0x00000001
#define CONNX_SOCKET_CLIENT_STOP_CFM          0x00000002


typedef struct
{
    size_t                         size;               /* Total size in byte for socket client instance. */
    uint16_t                       type;
    uint32_t                       flag;
    bool                           inUse;              /* Flag indicating whether socket client is used or not. */
    ConnxSocketRegisterInfo        registerInfo;       /* Socket client register information. */
    ConnxHandle                    eventHandle;        /* Event handle. */
    ConnxHandle                    mutexHandle;        /* Mutex handle. */
    bool                           quit;               /* Flag indicating whether to quit socket client thread. */
    uint8_t                       *buffer;             /* Buffer used to receive data from client socket. */
    uint32_t                       bufferSize;         /* Buffer size in byte. */
    ConnxHandle                    threadHandle;       /* Thread handle for socket client. */
    int32_t                        socketClient;       /* Socket client fd. */
} ConnxSocketClientInstance;


static int32_t ConnxSocketClientCreate(char *pathName);
static void ConnxSocketClientThreadLoop(void *data);

static int32_t ConnxSocketClientCreate(char *pathName)
{
    int32_t sock_fd = INVALID_CONNX_SOCKET;
    struct sockaddr_un server_addr;
    int32_t server_addr_len = 0;
    size_t str_len = 0;
    int32_t ret = 0;
    uint16_t sleepTimeInMs = 1000;
    uint16_t totalTimeout = 0;

    if (!pathName)
    {
        IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Invalid parameter"), __FUNCTION__));
        return INVALID_CONNX_SOCKET;
    }

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> pathName: '%s' "), __FUNCTION__, pathName));

    do
    {
        sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);

        if (sock_fd < 0)
        {
            IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Init socket fail"), __FUNCTION__));
            return INVALID_CONNX_SOCKET;
        }

        IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> sock_fd: %d"), __FUNCTION__, sock_fd));

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sun_family = AF_UNIX;

        str_len = CONNX_MIN(sizeof(server_addr.sun_path), strlen(pathName) + 1);
        strncpy(server_addr.sun_path, pathName, str_len);
        /* [QTI] Fix KW issue#266997, 277460. */
        server_addr.sun_path[str_len - 1] = '\0';

        server_addr_len = strlen(server_addr.sun_path) + sizeof(server_addr.sun_family);

        ret = connect(sock_fd, (struct sockaddr *) &server_addr, server_addr_len);

        if (ret >= 0)
        {
            IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Socket connected, sock_fd: %d"), __FUNCTION__, sock_fd));

            break;
        }
        else
        {
            if (totalTimeout < MAX_CONNX_SOCKET_CLIENT_TIMEOUT)
            {
                ConnxThreadSleep(sleepTimeInMs);

                totalTimeout += sleepTimeInMs;

                close(sock_fd);

                sock_fd = INVALID_CONNX_SOCKET;
            }
            else
            {
                IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Create socket fail in %d ms, err: %d (%s)"),
                               __FUNCTION__, totalTimeout, errno, strerror(errno)));
                close(sock_fd);

                sock_fd = INVALID_CONNX_SOCKET;

                break;
            }
        }
    }
    while (1);

    return sock_fd;
}

static void ConnxSocketClientThreadLoop(void *data)
{
    ConnxSocketClientInstance *inst = (ConnxSocketClientInstance *) data;
    ConnxSocketHandle socketHandle = (ConnxSocketHandle) inst;
    ConnxSocketRegisterInfo *registerInfo = NULL;
    ConnxContext context = NULL;
    int32_t sock_fd = INVALID_CONNX_SOCKET;
    uint8_t *buffer = NULL;
    size_t bufferSize = 0;
    int32_t flags = 0;
    int32_t ret = 0;

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Wait CONNX_SOCKET_CLIENT_START_CFM, inst: %p"), __FUNCTION__, inst));

    ConnxEventSet(SCI_GET_EVENT_HANDLE(inst), CONNX_SOCKET_CLIENT_START_CFM);
    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Socket client started"), __FUNCTION__));

    registerInfo = &SCI_GET_REGISTER_INFO(inst);
    buffer       = SCI_GET_BUFFER(inst);
    bufferSize   = (size_t) SCI_GET_BUFFER_SIZE(inst);

    sock_fd = ConnxSocketClientCreate(registerInfo->pathName);

    if (IS_VALID_CONNX_SOCKET(sock_fd))
    {
        /* Store sock_fd. */
        SCI_GET_CLIENT_FD(inst) = sock_fd;
    }
    else
    {
        /* Exit thread. */
        IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Exit thread because socket fail to connect"), __FUNCTION__));
        return;
    }

    context = registerInfo->context;

    while (!SCI_GET_QUIT(inst))
    {
        ret = recv(sock_fd, buffer, bufferSize, flags);

        /* If server close the socket, client will receive 0 byte correspondingly. */
        if (ret <= 0)
        {
            CONNX_CALLBACK(registerInfo, socketCloseCb, socketHandle, context);

            break;
        }

        if (SCI_GET_QUIT(inst))
        {
            break;
        }

        CONNX_CALLBACK(registerInfo, socketDataIndCb, socketHandle, context, buffer, ret);
    }

    ConnxEventSet(SCI_GET_EVENT_HANDLE(inst), CONNX_SOCKET_CLIENT_STOP_CFM);
    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Return"), __FUNCTION__));
}

/* ------------------------------------------------------------------------- */

ConnxResult ConnxSocketClientOpen(ConnxSocketRegisterInfo *registerInfo, ConnxSocketHandle *socketHandle)
{
    ConnxResult res = CONNX_SOCKET_RESULT_INTERNAL_ERROR;
    ConnxSocketClientInstance *inst = NULL;
    ConnxSocketRegisterInfo *tempInfo = NULL;
    uint32_t eventBits = 0;
    uint8_t *buffer = NULL;
    uint32_t bufferSize = DEFAULT_SOCKET_BUFFER_SIZE;
    uint16_t timeoutInMs = 1000;

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> "), __FUNCTION__));

    if (!registerInfo || !socketHandle)
    {
        IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Invalid parameter"), __FUNCTION__));
        return CONNX_SOCKET_RESULT_INVALID_PARAMETER;
    }

    *socketHandle = NULL;

    do
    {
        if ((registerInfo->size < sizeof(ConnxSocketRegisterInfo)) ||
            !registerInfo->pathName ||
            !registerInfo->socketDataIndCb ||
            !registerInfo->socketCloseCb)
        {
            res = CONNX_SOCKET_RESULT_INVALID_PARAMETER;
            break;
        }

        inst = (ConnxSocketClientInstance *) malloc(sizeof(ConnxSocketClientInstance));

        if (!inst)
        {
            IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Alloc socket client instance fail"), __FUNCTION__));
            res = CONNX_SOCKET_RESULT_OUT_OF_MEMORY;
            break;
        }

        memset(inst, 0, sizeof(ConnxSocketClientInstance));

        SCI_GET_CLIENT_FD(inst) = INVALID_CONNX_SOCKET;

        tempInfo = &SCI_GET_REGISTER_INFO(inst);

        memcpy(tempInfo, registerInfo, sizeof(ConnxSocketRegisterInfo));

        tempInfo->pathName = ConnxStrDup(registerInfo->pathName);

        SCI_GET_EVENT_HANDLE(inst) = ConnxEventCreate();
        CHK((SCI_GET_EVENT_HANDLE(inst) != NULL), "Create socket event");

        SCI_GET_MUTEX_HANDLE(inst) = ConnxMutexCreate();
        CHK((SCI_GET_MUTEX_HANDLE(inst) != NULL), "Create socket mutex");

        buffer = malloc(bufferSize);

        if (!buffer)
        {
            IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Allocate socket buffer fail"), __FUNCTION__));
            res = CONNX_SOCKET_RESULT_OUT_OF_MEMORY;
            break;
        }

        SCI_GET_BUFFER(inst) = buffer;
        SCI_GET_BUFFER_SIZE(inst) = bufferSize;
		SCI_GET_TYPE(inst) = IPC_SOCKET;
        SCI_GET_FLAG(inst) = SOCKET_CLIENT;

        /* Postpone to connect socket in the thread, so as to have the chance to retry. */
        SCI_GET_THREAD_HANDLE(inst) = ConnxThreadCreate(ConnxSocketClientThreadLoop,
                                      inst,
                                      0,
                                      CONNX_THREAD_PRIORITY_NORMAL,
                                      CONNX_SOCKET_CLIENT_THREAD_NAME);

        res = ConnxEventWait(SCI_GET_EVENT_HANDLE(inst), timeoutInMs, &eventBits);
        CHK(IS_SUCCESS(res) && (eventBits == CONNX_SOCKET_CLIENT_START_CFM), "Wait socket client thread started");

        SCI_GET_IN_USE(inst) = true;

        SCI_GET_SIZE(inst) = sizeof(ConnxSocketClientInstance);

        res = CONNX_RESULT_SUCCESS;
    }
    while (0);

    if (IS_SUCCESS(res))
    {
        IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Done, socket handle: %p"), __FUNCTION__, inst));

        *socketHandle = (ConnxSocketHandle) inst;
    }
    else
    {
        IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Open fail, res: 0x%04x"), __FUNCTION__, res));

        ConnxSocketClientClose(inst);
    }

    return res;
}

ConnxResult ConnxSocketClientClose(ConnxSocketHandle socketHandle)
{
    ConnxSocketClientInstance *inst = (ConnxSocketClientInstance *) socketHandle;
    ConnxSocketRegisterInfo *registerInfo = NULL;
    int32_t sock_fd = 0;
    uint32_t eventBits = 0;
    uint16_t timeoutInMs = 1000;

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> socket handle: %p"), __FUNCTION__, socketHandle));

    if (!IS_VALID_CONNX_SOCKET_CLIENT(inst))
    {
        IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Invalid parameter"), __FUNCTION__));
        return CONNX_SOCKET_RESULT_INVALID_PARAMETER;
    }

    SCI_GET_QUIT(inst) = true;

    sock_fd = SCI_GET_CLIENT_FD(inst);

    if (IS_VALID_CONNX_SOCKET(sock_fd))
    {
        IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Shutdown sock_fd: %d"), __FUNCTION__, sock_fd));

        ConnxSocketShutdown(sock_fd);

        RESET_SOCKET(SCI_GET_CLIENT_FD(inst));
    }

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Wait thread exit"), __FUNCTION__));

    ConnxEventWait(SCI_GET_EVENT_HANDLE(inst), timeoutInMs, &eventBits);

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Thread exit, eventBits: 0x%x"), __FUNCTION__, eventBits));

    if (SCI_GET_BUFFER(inst) != NULL)
    {
        free(SCI_GET_BUFFER(inst));
        SCI_GET_BUFFER(inst) = NULL;
    }

    SCI_GET_BUFFER_SIZE(inst) = 0;

    ConnxMutexDestroy(SCI_GET_MUTEX_HANDLE(inst));

    ConnxEventDestroy(SCI_GET_EVENT_HANDLE(inst));

    registerInfo = &SCI_GET_REGISTER_INFO(inst);

    if (registerInfo->pathName != NULL)
    {
        free(registerInfo->pathName);
        registerInfo->pathName = NULL;
    }

    SCI_GET_IN_USE(inst) = false;

    SCI_GET_SIZE(inst) = 0;

    free(inst);

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Done"), __FUNCTION__));

    return CONNX_RESULT_SUCCESS;
}

ConnxResult ConnxSocketClientSend(ConnxSocketHandle socketHandle, const void *buf, size_t len, size_t *bytesSent)
{
    ConnxSocketClientInstance *inst = (ConnxSocketClientInstance *) socketHandle;
    int32_t sock_fd = 0;

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> socket handle: %p, buf: %p, len: %d"), __FUNCTION__, socketHandle, buf, len));

    if (!IS_VALID_CONNX_SOCKET_CLIENT(inst) || !buf || !len)
    {
        IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Invalid parameter"), __FUNCTION__));

        return CONNX_SOCKET_RESULT_INVALID_PARAMETER;
    }

    sock_fd = SCI_GET_CLIENT_FD(inst);

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Client sock_fd: %d"), __FUNCTION__, sock_fd));

    return ConnxSocketSend(sock_fd, buf, len, bytesSent);
}

ConnxContext ConnxSocketClientGetContext(ConnxSocketHandle socketHandle)
{
    ConnxSocketClientInstance *inst = (ConnxSocketClientInstance *) socketHandle;
    ConnxSocketRegisterInfo *registerInfo = NULL;

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> socket handle: %p"), __FUNCTION__, socketHandle));

    if (!IS_VALID_CONNX_SOCKET_CLIENT(inst))
    {
        IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Invalid parameter"), __FUNCTION__));

        return NULL;
    }

    registerInfo = &SCI_GET_REGISTER_INFO(inst);

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> context: %p"), __FUNCTION__, registerInfo->context));

    return registerInfo->context;
}
