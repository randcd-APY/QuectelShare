/******************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifndef _CONNX_SOCKET_COMMON_H_
#define _CONNX_SOCKET_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "connx_common_def.h"

#define INVALID_CONNX_SOCKET              -1

#define IS_VALID_CONNX_SOCKET(sock_fd)    ((sock_fd) >= 0)

#define RESET_SOCKET(sock_fd)             ((sock_fd) = INVALID_CONNX_SOCKET)

#define IS_SUCCESS(res)                   ((res) == CONNX_RESULT_SUCCESS ? true : false)

#define DEFAULT_SOCKET_BUFFER_SIZE        8192

#define CONNX_SOCKET_RESULT_ERROR                 ((ConnxResult) 0x8000)

#define CONNX_SOCKET_RESULT_INVALID_PARAMETER     ((ConnxResult) (0x0001 + CONNX_SOCKET_RESULT_ERROR))
#define CONNX_SOCKET_RESULT_ALREADY_USED          ((ConnxResult) (0x0002 + CONNX_SOCKET_RESULT_ERROR))
#define CONNX_SOCKET_RESULT_INTERNAL_ERROR        ((ConnxResult) (0x0003 + CONNX_SOCKET_RESULT_ERROR))
#define CONNX_SOCKET_RESULT_INVALID_SOCKET        ((ConnxResult) (0x0004 + CONNX_SOCKET_RESULT_ERROR))
#define CONNX_SOCKET_RESULT_PIPE_BROKEN           ((ConnxResult) (0x0005 + CONNX_SOCKET_RESULT_ERROR))
#define CONNX_SOCKET_RESULT_OUT_OF_MEMORY         ((ConnxResult) (0x0006 + CONNX_SOCKET_RESULT_ERROR))


#ifdef __cplusplus
}
#endif

#endif  /* _CONNX_SOCKET_COMMON_H_ */

