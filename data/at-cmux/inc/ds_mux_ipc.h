#ifndef DS_MUX_IPC_H
#define DS_MUX_IPC_H
/*===========================================================================

                        D S_ M U X _IPC.H

DESCRIPTION

  This is the header file that will be exposed to the ATFWD Deamon. It is the header file
  of the IPC mechanism between the ATFWD and DS_MUX

Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  11/10/17   sm     Initial version

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SERVER_SOCKET_FR_ATFWD "/var/run/atfwd_socket_server"
#define CLIENT_SOCKET_FR_ATFWD "/var/run/atfwd_socket_client"

#define UNIX_PATH_MAX (108)

#define OP_NA 0x00000001
#define OP_EQ 0x00000002
#define OP_QU 0x00000004
#define OP_AR 0x00000008

#define AT_CMD_NAME_MAX_LEN      20
#define MAX_TOKEN_SIZE           20
#define MAX_TOKENS               44

#define AT_CMD_PARAM_SIZE            100
#define AT_CMD_AND_RESPONSE_SIZE     1024

/*--------------------------------------------------------------------
    ds_mux_at_cmd to store at-cmds.
    Eg:
    AT+CMUX=1,2,3,4;
    Then it will be saved as:
    atcmd_name =AT+CMUX

    opcode may take the following values.

    opcode = AT_MASK_NA_V01 if command is AT+CMUX
    opcode = AT_MASK_NA_V01 | AT_MASK_QU_V01 if command is AT+CMUX?
    opcode = AT_MASK_NA_V01 | AT_MASK_EQ_V01 | AT_MASK_QU_V01 if command is AT+CMUX=?
    opcode = AT_MASK_NA_V01 | AT_MASK_EQ_V01 |AT_MASK_AR_V01 if command is AT+CMUX=0,1,2;

    token = 1,2,3,4

----------------------------------------------------------------------*/

typedef struct {
    int         opcode;
    char        name[AT_CMD_NAME_MAX_LEN+1];
    int         ntokens;
    char        token[MAX_TOKENS][MAX_TOKEN_SIZE+1];
}ds_mux_at_cmd_request;

typedef struct {
  int   errorCode;
  char  resp_buf[AT_CMD_PARAM_SIZE];
}ds_mux_at_cmd_response;


/*--------------------------------------------------------------------------
   Socket descriptor paramters
---------------------------------------------------------------------------*/
typedef struct
{
 int                 sk_fd;       /* socket descriptor */
} ds_mux_sk_info_t;
#endif
