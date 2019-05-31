/******************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifndef _CONNX_SOCKET_H_
#define _CONNX_SOCKET_H_

#include "connx_socket_common.h"
#include "connx_common_def.h"

#include <sys/socket.h>
#include <sys/un.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef void *ConnxSocketHandle;

/* Callback to handle data received through socket. */
typedef void (* ConnxSocketCbDataInd)(ConnxSocketHandle socketHandle, ConnxContext context, void *data, size_t size);

/* Callback to handle socket close/shutdown event. */
typedef void (* ConnxSocketCbClose)(ConnxSocketHandle socketHandle, ConnxContext context);

/* Callback to handle new socket connection accepted in socket server. */
typedef void (* ConnxSocketCbAccept)(ConnxSocketHandle socketHandle, ConnxContext context);

typedef struct
{
    uint32_t              size;               /* Total size in byte for connx socket server register info. */
    ConnxContext          context;            /* Context for socket server register info. */
    char                 *pathName;           /* Path name for socket. */
    ConnxSocketCbDataInd  socketDataIndCb;    /* Callback for socket data received. */
    ConnxSocketCbClose    socketCloseCb;      /* Callback for socket close. */
    ConnxSocketCbAccept   socketAcceptCb;     /* Callback for socket accept, which is specific with socket server. */
} ConnxSocketRegisterInfo;


bool ConnxSocketInitRegisterInfo(ConnxSocketRegisterInfo *registerInfo, ConnxContext context, char *name, 
                                  ConnxSocketCbDataInd socketDataIndCb, ConnxSocketCbClose socketCloseCb, 
                                  ConnxSocketCbAccept socketAcceptCb);

char *ConnxSocketCreatePath(char *pathName, char *serverName, char *clientName);

ConnxResult ConnxSocketSend(int32_t sock_fd, const void *buf, size_t len, size_t *bytesSent);

ConnxResult ConnxSocketClose(int32_t sock_fd);

ConnxResult ConnxSocketShutdown(int32_t sock_fd);


#ifdef __cplusplus
}
#endif

#endif  /* _CONNX_SOCKET_H_ */

