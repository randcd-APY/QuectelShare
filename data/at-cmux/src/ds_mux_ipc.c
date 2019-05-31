/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     D S   M U X   IPC

GENERAL DESCRIPTION
  This file contains the IPC mechanism to connect and talk from DS_MUX Deamon
  to the ATFWD Deamon.

Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  11/10/17   sm     Initial version

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES

===========================================================================*/
#include "ds_mux_main.h"
#include "ds_mux_ipc.h"

char                           ds_mux_at_cmd_buf[AT_CMD_AND_RESPONSE_SIZE];
ds_mux_sk_info_t               sk_info_bw_atfwd_ds_mux;

extern ds_mux_sk_fd_set_info_t sk_fdset;
struct sockaddr_un             cl_address;


int ds_mux_recv_msg (int fd)
{
  int bytes_rcvd;
  ds_mux_at_cmd_request* at_cmd_ptr = NULL;
  unsigned int i = 0;
  socklen_t len = sizeof(struct sockaddr_un);

  memset(ds_mux_at_cmd_buf, 0, AT_CMD_AND_RESPONSE_SIZE);
  bytes_rcvd = recvfrom(fd, ds_mux_at_cmd_buf, AT_CMD_AND_RESPONSE_SIZE, 0, (struct sockaddr *)&cl_address, &len);

  if(bytes_rcvd <= 0)
  {
    LOG_MSG_ERROR(" Error in receiving message bytes received = %d. errno= %d", bytes_rcvd, errno,0);
    return DS_MUX_FAILURE_MACRO;
  }

  at_cmd_ptr = (ds_mux_at_cmd_request *)ds_mux_at_cmd_buf;

  LOG_MSG_INFO1("opcode: %d  name: %s  ntokens:%d  ", at_cmd_ptr->opcode, at_cmd_ptr->name, at_cmd_ptr->ntokens);

  LOG_MSG_INFO1("at_cmd_ptr->token[0] = %s", at_cmd_ptr->token[0], 0, 0);
  for(i = 0 ; i< at_cmd_ptr->ntokens ; i++)
  {
    LOG_MSG_INFO1("at_cmd_ptr->token[%d] = %s", i, at_cmd_ptr->token[i], 0);
  }

  ds_mux_validate_atfwd_cmux_params(at_cmd_ptr);

  return DS_MUX_SUCCESS_MACRO;
}

int ds_mux_send_msg(int fd, char* buf)
{
  int ret ;
  LOG_MSG_INFO1("Sending response to ATFWD", 0, 0, 0);
  ret = sendto(fd, buf, AT_CMD_AND_RESPONSE_SIZE, 0, (struct sockaddr *)&cl_address, sizeof(struct sockaddr_un));
  if(ret == 0 || ret==-1)
  {
    LOG_MSG_ERROR("error in sending. Bytes sent: %d, errno: %d", ret, errno, 0);
    return DS_MUX_FAILURE_MACRO;
  }
  return DS_MUX_SUCCESS_MACRO;
}

/*===========================================================================

FUNCTION QCMAP_NL_OPEN_SOCKET()

DESCRIPTION

  This function
  - opens ds_mux sockets
  - binds the socket to listen to the messages from ATFWD

DEPENDENCIES
  None.

RETURN VALUE
  DS_MUX_SUCCESS_MACRO on success
  DS_MUX_FAILURE_MACRO on failure


SIDE EFFECTS
  None

==========================================================================*/

 int ds_mux_open_server_socket
(
  ds_mux_sk_info_t   *sk_info
)
{
  int                  *p_sk_fd;
  int                   len;
  struct                sockaddr_un server;

   ds_assert(sk_info != NULL);

   p_sk_fd = &(sk_info->sk_fd);

/*--------------------------------------------------------------------------
  Open socket
---------------------------------------------------------------------------*/
  if ((*p_sk_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
  {
    LOG_MSG_ERROR("Cannot open socket errno: %d", errno,0,0);
    return DS_MUX_FAILURE_MACRO;
  }
  memset (&server, 0, sizeof (server));
  server.sun_family = AF_UNIX;
  strlcpy (server.sun_path, SERVER_SOCKET_FR_ATFWD, sizeof(server.sun_path));
  unlink(SERVER_SOCKET_FR_ATFWD);

/*-------------------------------------------------------------------------
  Bind socket to receive messages from atfwd
--------------------------------------------------------------------------*/
  len = strlen(server.sun_path) + sizeof(server.sun_family);
  if (bind (*p_sk_fd, (struct sockaddr *)&server, len) <0)
  {
    LOG_MSG_ERROR("Cannot bind socket errno: %d", errno,0,0);
    close(*p_sk_fd);
    return DS_MUX_FAILURE_MACRO;
  }
  return DS_MUX_SUCCESS_MACRO;
}


int ds_mux_atfwd_listener_init
(
  ds_mux_sk_fd_set_info_t*   sk_fdset,
  ds_mux_sock_thrd_fd_read_f read_f,
  ds_mux_sk_info_t*          sk_info,
  int                        max_fds
)
{
  memset(sk_info, 0, sizeof(ds_mux_sk_info_t));

  /*---------------------------------------------------------------------------
    Open sockets
  ----------------------------------------------------------------------------*/

  if( ds_mux_open_server_socket( sk_info) == DS_MUX_FAILURE_MACRO)
  {
    LOG_MSG_ERROR("Open: DMUX socket for atfwd failed",0,0,0);
    return DS_MUX_FAILURE_MACRO;
  }

  if( ds_mux_addfd_map(sk_fdset,sk_info->sk_fd,read_f, max_fds) ==
                                                            DS_MUX_FAILURE_MACRO)
  {
    LOG_MSG_ERROR("cannot add DSMUX atfwd socket for reading",0,0,0);
    close(sk_info->sk_fd);
    return DS_MUX_FAILURE_MACRO;
  }

return DS_MUX_SUCCESS_MACRO;
}



int ds_mux_listen_frm_atfwd(void)
{

  int ret_val;

  if(ds_mux_atfwd_listener_init(&sk_fdset, ds_mux_recv_msg, &sk_info_bw_atfwd_ds_mux, DS_MUX_MAX_NUM_OF_FD) == DS_MUX_SUCCESS_MACRO)
  {
    LOGI("Socket to receive ATFWD msgs succeeds");
  }
  else
  {
    LOGI("Socket to receive ATFWD msgs Fails");
    return DS_MUX_FAILURE_MACRO;
  }

  return DS_MUX_SUCCESS_MACRO;

}

int ds_mux_listen_fr_msgs_from_atfwd(void)
{
  int ret;
  ret = ds_mux_listen_frm_atfwd();
  if (DS_MUX_SUCCESS_MACRO != ret)
  {
     LOG_MSG_ERROR("unable to listen msgs from atfwd, errno: %d",ret, 0, 0);
  }
  return ret;
}

