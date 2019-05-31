/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "connx_socket_server.h"
#include "connx_util.h"
#include "connx_callback.h"
#include "connx_log.h"
#include "connx_ipc_debug.h"
#include "connx_ipc_common.h"

#define MAX_SOCKET_NUMBER                     1

#define IS_VALID_CONNX_SOCKET_SERVER(inst)    (((inst) != NULL) && ((inst)->size == sizeof(ConnxSocketServerInstance)) && (inst)->inUse)

#define SSI_GET_SIZE(p)                       ((p)->size)
#define SSI_GET_TYPE(p)                       ((p)->type)
#define SSI_GET_FLAG(p)                       ((p)->flag)
#define SSI_GET_IN_USE(p)                     ((p)->inUse)
#define SSI_GET_REGISTER_INFO(p)              ((p)->registerInfo)
#define SSI_GET_EVENT_HANDLE(p)               ((p)->eventHandle)
#define SSI_GET_MUTEX_HANDLE(p)               ((p)->mutexHandle)
#define SSI_GET_QUIT(p)                       ((p)->quit)
#define SSI_GET_BUFFER(p)                     ((p)->buffer)
#define SSI_GET_BUFFER_SIZE(p)                ((p)->bufferSize)
#define SSI_GET_THREAD_HANDLE(p)              ((p)->threadHandle)
#define SSI_GET_SERVER_FD(p)                  ((p)->socketServer)
#define SSI_GET_CLIENT_FD(p)                  ((p)->socketClient)

#define SSI_LOCK(inst)                        ConnxMutexLock(SSI_GET_MUTEX_HANDLE(inst))
#define SSI_UNLOCK(inst)                      ConnxMutexUnlock(SSI_GET_MUTEX_HANDLE(inst))

#define CONNX_SOCKET_SERVER_THREAD_NAME       "CONNX Socket Server Thread"

#define CONNX_SOCKET_SERVER_START_CFM         0x00000001
#define CONNX_SOCKET_SERVER_STOP_CFM          0x00000002


typedef struct
{
    size_t                         size;               /* Total size in byte for socket server instance. */
    uint16_t                       type;
    uint32_t					   flag;
    bool                           inUse;              /* Flag indicating whether socket server is used or not. */
    bool                           useNetLink;         /* Flag indicating whether netlink is created or not. */
    ConnxSocketRegisterInfo        registerInfo;       /* Socket server register information. */
    ConnxHandle                    eventHandle;        /* Event handle. */
    ConnxHandle                    mutexHandle;        /* Mutex handle. */
    bool                           quit;               /* Flag indicating whether to quit socket server thread. */
    uint8_t                       *buffer;             /* Buffer used to receive data from client socket. */
    uint32_t                       bufferSize;         /* Buffer size in byte. */
    ConnxHandle                    threadHandle;       /* Thread handle for socket server. */
    int32_t                        socketServer;       /* Socket server fd. */
    int32_t                        socketClient;       /* Socket client fd accepted by server. Note: limit to only support 1 socket client. */
} ConnxSocketServerInstance;


static int32_t ConnxSocketServerCreate(ConnxSocketServerInstance *inst);
static void ConnxSocketServerThreadLoop(void *data);


static int32_t ConnxSocketServerCreate(ConnxSocketServerInstance *inst)
{
    int32_t server_fd = INVALID_CONNX_SOCKET;
    struct sockaddr_un server_addr; /* server address information */
    int32_t server_addr_len = 0;
    size_t str_len = 0;
    char *pathName = NULL;
    ConnxSocketRegisterInfo *registerInfo = NULL;
    int32_t ret = 0;

    if (!inst)
    {
        return INVALID_CONNX_SOCKET;
    }

    registerInfo = &SSI_GET_REGISTER_INFO(inst);

    pathName = registerInfo->pathName;

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> pathName: '%s' "), __FUNCTION__, pathName));

    do
    {
        IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Create socket"), __FUNCTION__));

        server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        CHK(IS_VALID_CONNX_SOCKET(server_fd), "Create socket");

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sun_family = AF_UNIX;

        str_len = CONNX_MIN(sizeof(server_addr.sun_path), strlen(pathName) + 1);
        strncpy(server_addr.sun_path, pathName, str_len);
        /* [QTI] Fix KW issue#267000. */
        server_addr.sun_path[str_len] = '\0';

        unlink(server_addr.sun_path);
        server_addr_len = strlen(server_addr.sun_path) + sizeof(server_addr.sun_family);

        IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Bind socket"), __FUNCTION__));

        /* [TODO] Whether need to try sometimes to guarantee that IPC socket is created normally for socket client connection. */
        ret = bind(server_fd, (struct sockaddr *)&server_addr, server_addr_len);
        CHK(ret >= 0, "Bind socket");

        IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Listen socket"), __FUNCTION__));

        chmod(server_addr.sun_path, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

        ret = listen(server_fd, MAX_SOCKET_NUMBER);
        CHK(ret >= 0, "Listen socket");

        ret = 0;
    }
    while (0);

    if (ret < 0)
    {
        close(server_fd);
        return INVALID_CONNX_SOCKET;
    }

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Socket server created, server_fd: %d"), __FUNCTION__, server_fd));

    return server_fd;
}

static void ConnxSocketServerThreadLoop(void *data)
{
    ConnxSocketServerInstance *inst = (ConnxSocketServerInstance *) data;
    ConnxSocketHandle socketHandle = (ConnxSocketHandle) inst;
    ConnxSocketRegisterInfo *registerInfo = NULL;
    ConnxContext context = NULL;
    uint8_t *buffer = NULL;
    uint32_t bufferSize = 0;
    int32_t server_fd = INVALID_CONNX_SOCKET;
    int32_t new_fd = INVALID_CONNX_SOCKET;
    int32_t maxsock, ret;
    uint32_t conn_amount = 0;  /* socket connection count. */
    struct sockaddr_un client_addr; /* connector's address information */
    socklen_t sin_size = sizeof(client_addr);
    fd_set fdsr;

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Wait CONNX_SOCKET_SERVER_START_CFM"), __FUNCTION__));

    ConnxEventSet(SSI_GET_EVENT_HANDLE(inst), CONNX_SOCKET_SERVER_START_CFM);

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Socket server started"), __FUNCTION__));

    registerInfo = &SSI_GET_REGISTER_INFO(inst);
    buffer       = SSI_GET_BUFFER(inst);
    bufferSize   = (size_t) SSI_GET_BUFFER_SIZE(inst);
    server_fd    = SSI_GET_SERVER_FD(inst);

    context = registerInfo->context;

    maxsock = server_fd;

    while (!SSI_GET_QUIT(inst))
    {
        /* initialize file descriptor set */
        FD_ZERO(&fdsr);
        FD_SET(SSI_GET_SERVER_FD(inst), &fdsr);

        if (IS_VALID_CONNX_SOCKET(SSI_GET_CLIENT_FD(inst)))
        {
            FD_SET(SSI_GET_CLIENT_FD(inst), &fdsr);
        }

        IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> + select, maxsock: %d"), __FUNCTION__, maxsock));

        ret = select(maxsock + 1, &fdsr, NULL, NULL, NULL);

        IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> - select, ret: %d"), __FUNCTION__, ret));

        if (SSI_GET_QUIT(inst))
        {
            IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Quit"), __FUNCTION__));
            break;
        }

        if (ret < 0)
        {
            IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> select fail"), __FUNCTION__));

            break;
        }

        if (SSI_GET_CLIENT_FD(inst) != INVALID_CONNX_SOCKET)
        {
            if (FD_ISSET(SSI_GET_CLIENT_FD(inst), &fdsr))
            {
                IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> recv on socket client: %d"), __FUNCTION__, SSI_GET_CLIENT_FD(inst)));

                ret = recv(SSI_GET_CLIENT_FD(inst), buffer, bufferSize, 0);

                if (ret > 0)
                {
                    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Call socketDataIndCb on socket client: %d, socketHandle: %p, buffer: %p, length: %d"),
                                   __FUNCTION__, SSI_GET_CLIENT_FD(inst), socketHandle, buffer, ret));

                    IFLOG(DumpBuff(DEBUG_IPC_MESSAGE, (const uint8_t *) buffer, (uint32_t) ret));

                    /* Generic socket packet. */
                    CONNX_CALLBACK(registerInfo, socketDataIndCb, socketHandle, context, buffer, ret);
                }
                else
                {
                    IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Call socketCloseCb due to broken socket client: %d, socketHandle: %p, conn_amount: %d"),
                                   __FUNCTION__, SSI_GET_CLIENT_FD(inst), socketHandle, conn_amount));

                    CONNX_CALLBACK(registerInfo, socketCloseCb, socketHandle, context);

                    FD_CLR(SSI_GET_CLIENT_FD(inst), &fdsr);

                    ConnxSocketShutdown(SSI_GET_CLIENT_FD(inst));

                    RESET_SOCKET(SSI_GET_CLIENT_FD(inst));

                    if (conn_amount > 0)
                    {
                        --conn_amount;
                    }
                }
            }
        }
        /* check whether a new connection comes */
        if (FD_ISSET(SSI_GET_SERVER_FD(inst), &fdsr))
        {
            IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> accept on socket server: %d"), __FUNCTION__, SSI_GET_SERVER_FD(inst)));

            new_fd = accept(SSI_GET_SERVER_FD(inst), (struct sockaddr *)&client_addr, &sin_size);

            IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> new socket client: %d is accepted"), __FUNCTION__, new_fd));

            if (new_fd < 0)
            {
                IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Invalid new socket fd"), __FUNCTION__));
                continue;
            }

            if (conn_amount >= MAX_SOCKET_NUMBER)
            {
                IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Exceed max socket number: %d, shut down new socket fd: %d"),
                               __FUNCTION__, MAX_SOCKET_NUMBER, new_fd));

                continue;
            }

            ++conn_amount;

            if (new_fd > maxsock)
            {
                maxsock = new_fd;
            }

            SSI_GET_CLIENT_FD(inst) = new_fd;

            IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Call socketAcceptCb for new socket client: %d, socketHandle: %p"),
                           __FUNCTION__, SSI_GET_CLIENT_FD(inst), socketHandle));

            CONNX_CALLBACK(registerInfo, socketAcceptCb, socketHandle, context);
        }
    }

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Notify CONNX_SOCKET_SERVER_STOP_CFM"), __FUNCTION__));

    ConnxEventSet(SSI_GET_EVENT_HANDLE(inst), CONNX_SOCKET_SERVER_STOP_CFM);
}

/* ------------------------------------------------------------------------- */

ConnxResult ConnxSocketServerOpen(ConnxSocketRegisterInfo *registerInfo, ConnxSocketHandle *socketHandle)
{
    ConnxResult res = CONNX_SOCKET_RESULT_INTERNAL_ERROR;
    ConnxSocketServerInstance *inst = NULL;
    ConnxSocketRegisterInfo *tempInfo = NULL;
    uint32_t eventBits = 0;
    uint8_t *buffer = NULL;
    uint32_t bufferSize = DEFAULT_SOCKET_BUFFER_SIZE;
    int32_t server_fd = INVALID_CONNX_SOCKET;
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
        if (registerInfo->size != sizeof(ConnxSocketRegisterInfo) || !registerInfo->pathName)
        {
            IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Invalid registerInfo"), __FUNCTION__));

            res = CONNX_SOCKET_RESULT_INVALID_PARAMETER;
            break;
        }

        inst = malloc(sizeof(ConnxSocketServerInstance));

        if (!inst)
        {
            IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Alloc socket server instance fail"), __FUNCTION__));

            res = CONNX_SOCKET_RESULT_OUT_OF_MEMORY;
            break;
        }

        memset(inst, 0, sizeof(ConnxSocketServerInstance));

        SSI_GET_SERVER_FD(inst) = INVALID_CONNX_SOCKET;
        SSI_GET_CLIENT_FD(inst) = INVALID_CONNX_SOCKET;

        tempInfo = &SSI_GET_REGISTER_INFO(inst);

        memcpy(tempInfo, registerInfo, sizeof(ConnxSocketRegisterInfo));

        tempInfo->pathName = ConnxStrDup(registerInfo->pathName);

        SSI_GET_EVENT_HANDLE(inst) = ConnxEventCreate();
        CHK((SSI_GET_EVENT_HANDLE(inst) != NULL), "Create socket event");

        SSI_GET_MUTEX_HANDLE(inst) = ConnxMutexCreate();
        CHK((SSI_GET_MUTEX_HANDLE(inst) != NULL), "Create socket mutex");

        buffer = malloc(bufferSize);

        if (!buffer)
        {
            IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Allocate socket buffer fail"), __FUNCTION__));

            res = CONNX_SOCKET_RESULT_OUT_OF_MEMORY;
            break;
        }

        SSI_GET_BUFFER(inst) = buffer;
        SSI_GET_BUFFER_SIZE(inst) = bufferSize;
        SSI_GET_TYPE(inst) = IPC_SOCKET;
        SSI_GET_FLAG(inst) = SOCKET_SERVER;

        server_fd = ConnxSocketServerCreate(inst);

        if (!IS_VALID_CONNX_SOCKET(server_fd))
        {
            IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Create socket server fail"), __FUNCTION__));

            res = CONNX_SOCKET_RESULT_INTERNAL_ERROR;
            break;
        }

        SSI_GET_SERVER_FD(inst) = server_fd;

        SSI_GET_THREAD_HANDLE(inst) = ConnxThreadCreate(ConnxSocketServerThreadLoop, inst, 0,
                                                        CONNX_THREAD_PRIORITY_NORMAL,
                                                        CONNX_SOCKET_SERVER_THREAD_NAME);

        res = ConnxEventWait(SSI_GET_EVENT_HANDLE(inst), timeoutInMs, &eventBits);
        CHK(IS_SUCCESS(res) && (eventBits == CONNX_SOCKET_SERVER_START_CFM), "Wait socket server thread started");

        SSI_GET_IN_USE(inst) = true;

        SSI_GET_SIZE(inst) = sizeof(ConnxSocketServerInstance);

        res = CONNX_RESULT_SUCCESS;
    }
    while (0);

    if (IS_SUCCESS(res))
    {
        IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Done, socket handle: %p"), __FUNCTION__, inst));

        *socketHandle = inst;
    }
    else
    {
        IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Open fail, res: 0x%04x"), __FUNCTION__, res));

        ConnxSocketServerClose(inst);
    }

    return res;
}

ConnxResult ConnxSocketServerClose(ConnxSocketHandle socketHandle)
{
    ConnxSocketServerInstance *inst = (ConnxSocketServerInstance *) socketHandle;
    ConnxSocketRegisterInfo *registerInfo = NULL;
    int32_t sock_fd = 0;
    uint32_t eventBits = 0;
    uint16_t timeoutInMs = 1000;

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> socket handle: %p"), __FUNCTION__, socketHandle));

    if (!IS_VALID_CONNX_SOCKET_SERVER(inst))
    {
        IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Invalid parameter"), __FUNCTION__));
        return CONNX_SOCKET_RESULT_INVALID_PARAMETER;
    }

    SSI_GET_QUIT(inst) = true;

    sock_fd = SSI_GET_CLIENT_FD(inst);

    if (IS_VALID_CONNX_SOCKET(sock_fd))
    {
        IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Shutdown client sock_fd: %d"), __FUNCTION__, sock_fd));

        ConnxSocketShutdown(sock_fd);

        RESET_SOCKET(SSI_GET_CLIENT_FD(inst));
    }

    sock_fd = SSI_GET_SERVER_FD(inst);

    if (IS_VALID_CONNX_SOCKET(sock_fd))
    {
        IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Shutdown server sock_fd: %d"), __FUNCTION__, sock_fd));

        ConnxSocketShutdown(sock_fd);

        RESET_SOCKET(SSI_GET_SERVER_FD(inst));
    }

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Wait thread exit"), __FUNCTION__));

    ConnxEventWait(SSI_GET_EVENT_HANDLE(inst), timeoutInMs, &eventBits);

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Thread exit, eventBits: 0x%x"), __FUNCTION__, eventBits));

    if (SSI_GET_BUFFER(inst) != NULL)
    {
        free(SSI_GET_BUFFER(inst));
        SSI_GET_BUFFER(inst) = NULL;
    }

    SSI_GET_BUFFER_SIZE(inst) = 0;

    ConnxMutexDestroy(SSI_GET_MUTEX_HANDLE(inst));

    ConnxEventDestroy(SSI_GET_EVENT_HANDLE(inst));

    registerInfo = &SSI_GET_REGISTER_INFO(inst);

    if (registerInfo->pathName != NULL)
    {
        free(registerInfo->pathName);
        registerInfo->pathName = NULL;
    }

    SSI_GET_IN_USE(inst) = false;

    SSI_GET_SIZE(inst) = 0;

    free(inst);

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Done"), __FUNCTION__));

    return CONNX_RESULT_SUCCESS;
}

ConnxResult ConnxSocketServerSend(ConnxSocketHandle socketHandle, const void *buf, size_t len, size_t *bytesSent)
{
    ConnxSocketServerInstance *inst = (ConnxSocketServerInstance *) socketHandle;
    int32_t sock_fd = 0;

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> socket handle: %p, buf: %p, len: %d"), __FUNCTION__, socketHandle, buf, len));

    if (!IS_VALID_CONNX_SOCKET_SERVER(inst) || !buf || !len)
    {
        IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Invalid parameter"), __FUNCTION__));
        return CONNX_SOCKET_RESULT_INVALID_PARAMETER;
    }

    sock_fd = SSI_GET_CLIENT_FD(inst);

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> Client sock_fd: %d"), __FUNCTION__, sock_fd));

    return ConnxSocketSend(sock_fd, buf, len, bytesSent);
}

ConnxContext ConnxSocketServerGetContext(ConnxSocketHandle socketHandle)
{
    ConnxSocketServerInstance *inst = (ConnxSocketServerInstance *) socketHandle;
    ConnxSocketRegisterInfo *registerInfo = NULL;

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> socket handle: %p"), __FUNCTION__, socketHandle));

    if (!IS_VALID_CONNX_SOCKET_SERVER(inst))
    {
        IFLOG(DebugOut(DEBUG_IPC_ERROR, TEXT("<%s> Invalid parameter"), __FUNCTION__));
        return NULL;
    }

    registerInfo = &SSI_GET_REGISTER_INFO(inst);

    IFLOG(DebugOut(DEBUG_IPC_MESSAGE, TEXT("<%s> context: %p"), __FUNCTION__, registerInfo->context));

    return registerInfo->context;
}
