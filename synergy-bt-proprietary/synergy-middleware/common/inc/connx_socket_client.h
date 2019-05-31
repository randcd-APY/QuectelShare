/******************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifndef _CONNX_SOCKET_CLIENT_H_
#define _CONNX_SOCKET_CLIENT_H_

#include "connx_socket.h"

#ifdef __cplusplus
extern "C" {
#endif


ConnxResult ConnxSocketClientOpen(ConnxSocketRegisterInfo *registerInfo, ConnxSocketHandle *socketHandle);

ConnxResult ConnxSocketClientClose(ConnxSocketHandle socketHandle);

ConnxResult ConnxSocketClientSend(ConnxSocketHandle socketHandle, const void *buf, size_t len, size_t *bytesSent);

ConnxContext ConnxSocketClientGetContext(ConnxSocketHandle socketHandle);


#ifdef __cplusplus
}
#endif

#endif  /* _CONNX_SOCKET_CLIENT_H_ */
