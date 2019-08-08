/**
  @file
  mcm_data_srv.c

  @brief
  This file provides Mobile Connection Manager(mcm) Server functionality.

  @details
  mcm_data_srv is used to handle multiple mcm_data clients

*/

/*===========================================================================

  Copyright (c) 2013-2014, 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/27/13   vb      Initial module.

===========================================================================*/

#include "mcm_data_utils.h"
#include "qmi_client_instance_defs.h"
#include <signal.h>
#include "mcm_client_v01.h"
#include "mcm_ipc.h"
#include "mcm_constants.h"
#include "mcm_ssr_util.h"
#include "data_port_mapper_v01.h"


/*---------------------------------------------------------------------------
  Function declarations of QMI service command handlers
---------------------------------------------------------------------------*/

static qmi_csi_cb_error  mcm_data_start_data_call
(
  qmi_mcm_data_client_info_type  *clnt_info,
  qmi_req_handle                 req_handle,
  int                            msg_id,
  void                           *req_c_struct,
  int                            req_c_struct_len,
  void                           *service_handle,
  unsigned int                   client_index
);

static qmi_csi_cb_error  mcm_data_stop_data_call
(
  qmi_mcm_data_client_info_type  *clnt_info,
  qmi_req_handle                 req_handle,
  int                            msg_id,
  void                           *req_c_struct,
  int                            req_c_struct_len,
  void                           *service_handle,
  unsigned int                   client_index
);

static qmi_csi_cb_error  mcm_data_get_pkt_stats
(
  qmi_mcm_data_client_info_type  *clnt_info,
  qmi_req_handle                 req_handle,
  int                            msg_id,
  void                           *req_c_struct,
  int                            req_c_struct_len,
  void                           *service_handle,
  unsigned int                   client_index
);

static qmi_csi_cb_error  mcm_data_reset_pkt_stats
(
  qmi_mcm_data_client_info_type  *clnt_info,
  qmi_req_handle                 req_handle,
  int                            msg_id,
  void                           *req_c_struct,
  int                            req_c_struct_len,
  void                           *service_handle,
  unsigned int                   client_index
);

static qmi_csi_cb_error  mcm_data_get_device_name
(
  qmi_mcm_data_client_info_type  *clnt_info,
  qmi_req_handle                 req_handle,
  int                            msg_id,
  void                           *req_c_struct,
  int                            req_c_struct_len,
  void                           *service_handle,
  unsigned int                   client_index
);

static qmi_csi_cb_error  mcm_data_addr_count
(
  qmi_mcm_data_client_info_type  *clnt_info,
  qmi_req_handle                 req_handle,
  int                            msg_id,
  void                           *req_c_struct,
  int                            req_c_struct_len,
  void                           *service_handle,
  unsigned int                   client_index
);

static qmi_csi_cb_error  mcm_data_get_call_tech
(
  qmi_mcm_data_client_info_type  *clnt_info,
  qmi_req_handle                 req_handle,
  int                            msg_id,
  void                           *req_c_struct,
  int                            req_c_struct_len,
  void                           *service_handle,
  unsigned int                   client_index
);

static qmi_csi_cb_error  mcm_data_get_call_status
(
  qmi_mcm_data_client_info_type  *clnt_info,
  qmi_req_handle                 req_handle,
  int                            msg_id,
  void                           *req_c_struct,
  int                            req_c_struct_len,
  void                           *service_handle,
  unsigned int                   client_index
);

static qmi_csi_cb_error  mcm_data_get_device_addr
(
  qmi_mcm_data_client_info_type  *clnt_info,
  qmi_req_handle                 req_handle,
  int                            msg_id,
  void                           *req_c_struct,
  int                            req_c_struct_len,
  void                           *service_handle,
  unsigned int                   client_index
);

static qmi_csi_cb_error  mcm_data_get_data_channel_rate
(
  qmi_mcm_data_client_info_type  *clnt_info,
  qmi_req_handle                 req_handle,
  int                            msg_id,
  void                           *req_c_struct,
  int                            req_c_struct_len,
  void                           *service_handle,
  unsigned int                   client_index
);

static qmi_csi_cb_error  mcm_data_event_register
(
  qmi_mcm_data_client_info_type  *clnt_info,
  qmi_req_handle                 req_handle,
  int                            msg_id,
  void                           *req_c_struct,
  int                            req_c_struct_len,
  void                           *service_handle,
  unsigned int                   client_index
);

static qmi_csi_cb_error  mcm_data_get_reg_status
(
  qmi_mcm_data_client_info_type  *clnt_info,
  qmi_req_handle                 req_handle,
  int                            msg_id,
  void                           *req_c_struct,
  int                            req_c_struct_len,
  void                           *service_handle,
  unsigned int                   client_index
);

/*---------------------------------------------------------------------------
  Function to send unsolicited indication to clients
---------------------------------------------------------------------------*/

static qmi_csi_error mcm_data_send_indication
(
  mcm_data_unsol_event_ind_msg_v01 * unsol_event_data
);

void mcm_data_qmi_dsd_ind_cb
(
  qmi_client_type  user_handle,
  unsigned int     msg_id,
  void             *ind_buf,
  unsigned int     ind_buf_len,
  void             *ind_cb_data
);

void mcm_data_qmi_dsd_ind_handler
(
  dsd_system_status_info_type_v01  *dsd_ind_msg
);

void mcm_data_qmi_ssr_ind_handler
(
  mcm_data_modem_ssr_status_type_t *status_info
);

int32 mcm_data_dsd_init
(
  void
);

static void mcm_data_modem_ssr_err_cb
(
  qmi_client_type        user_handle,
  qmi_client_error_type  error,
  void                   *err_cb_data
);

static void mcm_data_modem_ssr_notify_cb
(
  qmi_client_type                user_handle,
  qmi_idl_service_object_type    service_obj,
  qmi_client_notify_event_type   service_event,
  void                           *notify_cb_data
);

static qmi_csi_cb_error qmi_mcm_data_connect_cb
(
  qmi_client_handle  client_handle,
  void               *service_handle,
  void               **connection_handle
);

static void qmi_mcm_data_disconnect_cb
(
  void   *connection_handle,
  void   *service_handle
);

static qmi_csi_cb_error qmi_mcm_data_handle_client_req_cb
(
  void             *connection_handle,
  qmi_req_handle   req_handle,
  unsigned int     msg_id,
  void             *req_c_struct,
  unsigned int     req_c_struct_len,
  void             *service_handle
);

static void mcm_data_sig_hdlr
(
  int signal
);

static void mcm_data_cleanup
(
  void
);

static void mcm_data_release_client
(
    qmi_mcm_data_client_info_type *clnt_info
);

extern unsigned int mcm_data_clt_wds_sockfd;

static ds_dll_el_t *call_table_head;
static ds_dll_el_t *txn_table_head;
static qmi_mcm_data_state_info_type qmi_mcm_data_msgr_state;
static qmi_csi_os_params os_params;

/* Listening sockets */
unsigned int mcm_data_sockfd;

/* Sending sockets */
unsigned int mcm_data_cli_sockfd;

/* Structure used by mcm-framework to execute
   the exit function when mcm_data service stops */
mcm_ipc_srv_mgr_exit_func data_exit;

static uint32 call_id_cntr;

static qmi_csi_cb_error
(
  * const mcm_data_req_handle_table[]
)
(
 qmi_mcm_data_client_info_type  *clnt_info,
 qmi_req_handle                     req_handle,
 int                                msg_id,
 void                               *req_c_struct,
 int                                req_c_struct_len,
 void                               *service_handle,
 unsigned int                    client_index
) =
{
  /* Request handler for message ID 0 */
  mcm_data_start_data_call,
  /* Request handler for message ID 1 */
  mcm_data_stop_data_call,
  /* Request handler for message ID 2 */
  mcm_data_get_pkt_stats,
  /* Request handler for message ID 3 */
  mcm_data_reset_pkt_stats,
  /* Request handler for message ID 4 */
  mcm_data_get_device_name,
  /* Request handler for message ID 5 */
  mcm_data_addr_count,
  /* Request handler for message ID 6 */
  mcm_data_get_call_tech,
  /* Request handler for message ID 7 */
  mcm_data_get_call_status,
  /* Request handler for message ID 8 */
  mcm_data_get_device_addr,
  /* Request handler for message ID 9 */
  mcm_data_get_data_channel_rate,
  /* Request handler for message ID A */
  mcm_data_event_register,
  /* Request handler for message ID B */
  mcm_data_get_reg_status,

  NULL
};

void mcm_data_inited_cb
(
  void *mcm_data_inited_cb_data
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  qmi_mcm_data_msgr_state.mcm_data_inited = TRUE;
  MCM_DATA_LOG_FUNC_EXIT();
}

ds_dll_el_t* get_call_tbl_head
(
  void
)
{
  return call_table_head;
}

ds_dll_el_t* get_txn_tbl_head
(
  void
)
{
  return txn_table_head;
}

int32 mcm_data_create_client_socket
(
  unsigned int *sockfd
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  int32 return_val =  MCM_DATA_SUCCESS;

  if ((*sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == MCM_DATA_ERROR)
  {
    MCM_DATA_LOG_ERROR("Error creating socket, errno: %d", errno);
    return_val = MCM_DATA_ERROR;
    goto BAIL;
  }

  if (fcntl(*sockfd, F_SETFD, FD_CLOEXEC) < 0)
  {
    MCM_DATA_LOG_ERROR("Couldn't set Close on Exec, errno: %d", errno);
    return_val = MCM_DATA_ERROR;
    goto BAIL;
  }

BAIL:
  MCM_DATA_LOG_FUNC_EXIT();
  return return_val;
}

int32 mcm_data_create_listener_socket
(
  void
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  int32 val, return_val, len;
  struct sockaddr_un mcm_data_dsi;
  struct timeval rcv_timeo;

  return_val = mcm_data_create_client_socket(&mcm_data_sockfd);
  if ( return_val != MCM_DATA_SUCCESS)
  {
    MCM_DATA_LOG_ERROR("%s", "mcm_data_create_client_socket failed");
    return_val = MCM_DATA_ERROR;
    goto BAIL;
  }

  rcv_timeo.tv_sec = 0;
  rcv_timeo.tv_usec = 100000;
  setsockopt(mcm_data_sockfd,
             SOL_SOCKET,
             SO_RCVTIMEO,
             (void *)&rcv_timeo,
             sizeof(rcv_timeo));
  val = fcntl(mcm_data_sockfd, F_GETFL, 0);
  fcntl(mcm_data_sockfd, F_SETFL, val | O_NONBLOCK);

  /* To limit the index of __fd_bits internal array to */
  /* its MAX size which is equal to number of bits in unsigned long */
  if (mcm_data_sockfd >= (8*sizeof(unsigned long)))
    goto BAIL;

  /* set bit in os_params */
  FD_SET(mcm_data_sockfd, &os_params.fds);
  os_params.max_fd = MAX(os_params.max_fd, mcm_data_sockfd);

  mcm_data_dsi.sun_family = AF_UNIX;
  std_strlcpy(mcm_data_dsi.sun_path,
              MCM_DATA_DSI_UDS_FILE,
              MCM_DATA_DSI_UDS_FILE_LEN);
  unlink(mcm_data_dsi.sun_path);
  len = strlen(mcm_data_dsi.sun_path) + sizeof(mcm_data_dsi.sun_family);
  if (bind(mcm_data_sockfd,
           (struct sockaddr *)&mcm_data_dsi,
           len) == MCM_DATA_ERROR)
  {
    MCM_DATA_LOG_ERROR("Error binding the socket, errno: %d", errno);
    return_val = MCM_DATA_ERROR;
    goto BAIL;
  }
  return_val = MCM_DATA_SUCCESS;

BAIL:
  MCM_DATA_LOG_FUNC_EXIT();
  return return_val;
}

/* Function to be executed by mcm-framework
   when mcm_data service stops */
void mcm_data_exit_func()
{
  MCM_DATA_LOG_FUNC_ENTRY();
  mcm_data_cleanup();
  MCM_DATA_LOG_FUNC_EXIT();
  exit(0);
}

int main
(
  void
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  mcm_data_event_data_t *event_data;
  fd_set master_fd_set;
  int i, nbytes=0;
  char buf[MAX_BUF_LEN];
  struct sockaddr_storage server_addr;
  socklen_t addr_len = sizeof(server_addr);
  int32 rc;

  mcm_set_service_ready(MCM_DATA_SERVICE, 0);

  /* Set mcm_data_inited to FALSE */
  qmi_mcm_data_msgr_state.mcm_data_inited = FALSE;
  qmi_mcm_data_msgr_state.mcm_data_notifier_inited = FALSE;
  qmi_mcm_data_msgr_state.mcm_data_modem_local_state = MCM_DATA_MODEM_STATUS_IS;

  MCM_DATA_LOG_INFO("%s", "Registering for Signals");
  signal(SIGTERM, mcm_data_sig_hdlr);
  signal(SIGINT, mcm_data_sig_hdlr);
  signal(SIGSEGV, mcm_data_sig_hdlr);

  /* Registering exit function for mcm_data service with mcm-framework */
  data_exit.srv_id = MCM_DATA_V01;
  data_exit.srv_exit_func = mcm_data_exit_func;
  mcm_ipc_srv_mgr_start(&data_exit);

  if ( MCM_DATA_SUCCESS != (rc = mcm_data_init_srv()) )
  {
    MCM_DATA_LOG_ERROR("mcm_data_init_srv: Failed with error %d", rc);
    return rc;
  }

  while(mcm_ipc_get_service_is_ready() == 0) {
    usleep(100000);
  }

  mcm_set_service_ready(MCM_DATA_SERVICE, 1);

  while (1)
  {
    master_fd_set = os_params.fds;
    rc = select(os_params.max_fd+1, &master_fd_set, NULL, NULL, NULL);
    if (rc <= 0)
    {
      MCM_DATA_LOG_ERROR("Error in select, errno:%d", errno);
      continue;
    }

    for (i = 0; i <= os_params.max_fd; i++)
    {
      if (FD_ISSET(i, &master_fd_set))
      {
        if (i == mcm_data_sockfd)
        {
          if ((nbytes = recvfrom(i,
                                  buf,
                                  MAX_BUF_LEN - 1,
                                  0,
                                  (struct sockaddr *)&server_addr,
                                  &addr_len)) > 0)
          {
            event_data = (mcm_data_event_data_t *)buf;
            if (event_data->event_id  == MCM_DATA_DSI_EVENT)
            {
              MCM_DATA_LOG_INFO("Recieved MCM_DATA_DSI_EVENT[%d]",
                                event_data->event_id);
              mcm_data_dsi_net_ev_handler(
                                event_data->evt_data.dsi_event.dsi_nethandle,
                                event_data->evt_data.dsi_event.user_data,
                                event_data->evt_data.dsi_event.evt,
                                event_data->evt_data.dsi_event.payload_ptr );
            }
            else if (event_data->event_id  == MCM_DATA_WDS_EVENT)
            {
              MCM_DATA_LOG_INFO("Recieved MCM_DATA_WDS_EVENT[%d]",
                                event_data->event_id);

              mcm_data_qmi_wds_ind_handler(
                                event_data->evt_data.wds_event.user_data,
                                event_data->evt_data.wds_event.ind_id,
                                &event_data->evt_data.wds_event.ind_data);

            }
            else if (event_data->event_id  == MCM_DATA_DSD_EVENT)
            {
              MCM_DATA_LOG_INFO("Recieved MCM_DATA_DSD_EVENT[%d]",
                                event_data->event_id);
              mcm_data_qmi_dsd_ind_handler(
                          &(event_data->evt_data.dsd_event.status_info));
            }
            else if (event_data->event_id  == MCM_DATA_MODEM_SSR_EVENT)
            {
              MCM_DATA_LOG_HIGH("Recieved MCM_DATA_SSR_EVENT[%d], %s indication",
                                event_data->event_id,
                                event_data->evt_data.ssr_event.status_info
                                     == MCM_DATA_MODEM_STATUS_IS ? "IS":"OOS" );
              mcm_data_qmi_ssr_ind_handler(
                                &(event_data->evt_data.ssr_event.status_info));
            }
          }
          else
          {
            MCM_DATA_LOG_ERROR("recvfrom failed with error %d",
                               errno);
          }
        }
        else /* QMI message from the client */
        {
          MCM_DATA_LOG_INFO("%s", "Received QMI Message");
          qmi_csi_handle_event(qmi_mcm_data_msgr_state.service_handle,
                               &os_params);
        }
      }
    }
  }
  mcm_set_service_ready(MCM_DATA_SERVICE, 0);

  return MCM_DATA_SUCCESS;
}

int32 mcm_data_dsd_init
(
  void
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  qmi_idl_service_object_type        mcm_data_dsd_service_object;
  qmi_idl_service_object_type        mcm_data_dpm_modem_ssr_object;
  qmi_client_error_type              qmi_error = QMI_NO_ERR;
  int32                              rc = MCM_DATA_SUCCESS;
  mcm_data_modem_ssr_thread_info     signal_data;

  mcm_data_dsd_service_object = dsd_get_service_object_v01();
  if (mcm_data_dsd_service_object == NULL)
  {
    MCM_DATA_LOG_ERROR("%s", "DSD service object not found. Continuing without"
                             "DSD Service support");
    rc = MCM_DATA_ERROR;
    goto BAIL;
  }

  qmi_error = qmi_client_init_instance(mcm_data_dsd_service_object,
                                       QMI_CLIENT_INSTANCE_ANY,
                                       mcm_data_qmi_dsd_ind_cb,
                                       NULL,
                                       &qmi_mcm_data_msgr_state.cli_os_params,
                                       MCM_DATA_QMI_DSD_TIMEOUT,
                                       &qmi_mcm_data_msgr_state.qmi_dsd_hndl );
  if (qmi_error != QMI_NO_ERR)
  {
    qmi_client_release(qmi_mcm_data_msgr_state.svc_cli_notifier);
    qmi_mcm_data_msgr_state.svc_cli_notifier = NULL;
    MCM_DATA_LOG_ERROR("qmi_client_init: Failed to initialize mcm_data"
                       " as client of DSD service with error %d",
                       qmi_error);
    rc = MCM_DATA_ERROR;
    goto BAIL;
  }

  qmi_error = qmi_client_register_error_cb(qmi_mcm_data_msgr_state.qmi_dsd_hndl,
                                           mcm_data_modem_ssr_err_cb,
                                           NULL);
  if(qmi_error != QMI_NO_ERR) {
    MCM_DATA_LOG_ERROR("qmi_client_register_error_cb failed with %d",qmi_error);
    rc = MCM_DATA_ERROR;
    goto BAIL;
  }

  if( !qmi_mcm_data_msgr_state.mcm_data_notifier_inited )
  {
    MCM_DATA_LOG_HIGH("%s","Initialize DPM service object and ssr_err_cb. "
                           "Done once during mcm_init.");

    /* If the DSD handle was initialized earlier, implies modem is in service.
     * So Set mcm_data_modem_local_state to MCM_DATA_MODEM_STATUS_IS.*/
    qmi_mcm_data_msgr_state.mcm_data_modem_local_state = MCM_DATA_MODEM_STATUS_IS;

    /* Create dpm object & register for notify callback. */
    mcm_data_dpm_modem_ssr_object = dpm_get_service_object_v01();
    if (mcm_data_dpm_modem_ssr_object == NULL)
    {
      MCM_DATA_LOG_ERROR("%s", "SSR DPM service object not found");
      rc = MCM_DATA_ERROR;
      goto BAIL;
    }
    (void)qmi_client_notifier_init(mcm_data_dpm_modem_ssr_object,
                                   NULL,
                                   &qmi_mcm_data_msgr_state.qmi_dpm_hndl);
    signal_data.thread_id=pthread_self();
    qmi_error = qmi_client_register_notify_cb(qmi_mcm_data_msgr_state.qmi_dpm_hndl,
                                              mcm_data_modem_ssr_notify_cb,
                                              &signal_data);
    if(qmi_error != QMI_NO_ERR){
      MCM_DATA_LOG_ERROR("qmi_client_register_notify_cb failed: %d", qmi_error);
      rc = MCM_DATA_ERROR;
      goto BAIL;
    }

    qmi_mcm_data_msgr_state.mcm_data_notifier_inited = TRUE;
  }

BAIL:
  MCM_DATA_LOG_FUNC_EXIT();
  return rc;
}

static void mcm_data_modem_ssr_err_cb
(
  qmi_client_type        user_handle,
  qmi_client_error_type  error,
  void                   *err_cb_data
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  mcm_data_event_data_t mcm_data_buffer;
  struct sockaddr_un ssr_mcm_data_;
  int numBytes=0, len;

  ssr_mcm_data_.sun_family = AF_UNIX;
  std_strlcpy(ssr_mcm_data_.sun_path,
              MCM_DATA_DSI_UDS_FILE,
              MCM_DATA_DSI_UDS_FILE_LEN);
  len = strlen(ssr_mcm_data_.sun_path)+sizeof(ssr_mcm_data_.sun_family);

  mcm_data_buffer.event_id = MCM_DATA_MODEM_SSR_EVENT;
  mcm_data_buffer.evt_data.ssr_event.status_info = MCM_DATA_MODEM_STATUS_OOS;
  /* Send message */
  if ((numBytes = sendto(mcm_data_cli_sockfd,
                         (void *)&mcm_data_buffer,
                         sizeof(mcm_data_event_data_t),
                         0,
                         (struct sockaddr *)&ssr_mcm_data_,
                         len)) == -1)
  {
    MCM_DATA_LOG_ERROR("%s", "Send Failed from modem_ssr_err_cb context");
  }
  else
  {
    MCM_DATA_LOG_FUNC_EXIT();
  }
  /* Triggered an ssr error event, will be processed in main thread */
  return;
}

static void mcm_data_modem_ssr_notify_cb
(
  qmi_client_type                user_handle,
  qmi_idl_service_object_type    service_obj,
  qmi_client_notify_event_type   service_event,
  void                           *notify_cb_data
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  mcm_data_event_data_t mcm_data_buffer;
  struct sockaddr_un ssr_mcm_data_;
  int numBytes=0, len;

  mcm_data_modem_ssr_thread_info *sig =
                           (mcm_data_modem_ssr_thread_info *)notify_cb_data;

  if (service_event != QMI_CLIENT_SERVICE_COUNT_INC  ||
      sig->thread_id == pthread_self())
  {
    MCM_DATA_LOG_ERROR("Ignore notification, service_event: %d", service_event);
    return;
  }

  ssr_mcm_data_.sun_family = AF_UNIX;
  std_strlcpy(ssr_mcm_data_.sun_path,
              MCM_DATA_DSI_UDS_FILE,
              MCM_DATA_DSI_UDS_FILE_LEN);
  len = strlen(ssr_mcm_data_.sun_path)+sizeof(ssr_mcm_data_.sun_family);
  mcm_data_buffer.event_id = MCM_DATA_MODEM_SSR_EVENT;
  mcm_data_buffer.evt_data.ssr_event.status_info = MCM_DATA_MODEM_STATUS_IS;

  /* Send message */
  if ((numBytes = sendto(mcm_data_cli_sockfd,
                         (void *)&mcm_data_buffer,
                         sizeof(mcm_data_event_data_t),
                         0,
                         (struct sockaddr *)&ssr_mcm_data_,
                         len)) == -1)
  {
    MCM_DATA_LOG_ERROR("%s", "Send Failed from modem_ssr_notify_cb context");
  }
  else
  {
    MCM_DATA_LOG_FUNC_EXIT();
  }
  /* Triggered an ssr notify event, will be processed in main() */
  return;
}

int32 mcm_data_init_srv
(
  void
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  qmi_csi_error                      rc;
  int32                              return_val;
  dsd_get_system_status_resp_msg_v01 sys_resp_msg;
  dsi_data_bearer_tech_t             data_tech;
  unsigned int                       num_services = 0, num_entries = 0;
  qmi_client_error_type              qmi_error = QMI_NO_ERR;
  qmi_service_info                   info[10];
  dsd_system_status_change_resp_msg_v01 sys_reg_resp_msg;

  /*Initialize the Diag for QXDM logs*/
  if (TRUE != Diag_LSM_Init(NULL))
  {
    MCM_DATA_LOG_ERROR("%s", "Diag_LSM_Init: Failed");
  }
  else
  {
    MCM_DATA_LOG_INFO("%s", "Diag_LSM_Init: Success");
  }

  /* Register MCM DATA service with QCSI */
  rc = qmi_csi_register(mcm_data_get_service_object_v01(),
                        qmi_mcm_data_connect_cb,
                        qmi_mcm_data_disconnect_cb,
                        qmi_mcm_data_handle_client_req_cb,
                        &qmi_mcm_data_msgr_state,
                        &os_params,
                        &qmi_mcm_data_msgr_state.service_handle);

  if(rc != QMI_CSI_NO_ERR)
  {
    MCM_DATA_LOG_ERROR("qmi_csi_register: Failed to register mcm_data"
                       " service with QMI Framework with error %d", rc);
    return_val = MCM_DATA_ERROR;
    goto EXIT;
  }

  MCM_DATA_LOG_INFO("%s", "mcm_data service registered with QMI Framework");

  /* Create a listening socket for handling data from DSI */
  return_val = mcm_data_create_listener_socket();
  if (return_val != MCM_DATA_SUCCESS)
  {
    MCM_DATA_LOG_ERROR("mcm_data_create_listener_socket:"
                       " Failed with error %d", return_val);
    return_val = MCM_DATA_ERROR;
    goto EXIT;
  }
  MCM_DATA_LOG_INFO("%s", "Listening socket successfully created for"
                    " handling data from dsi_netctrl");

  return_val = mcm_data_create_client_socket(&mcm_data_cli_sockfd);
  if (return_val != MCM_DATA_SUCCESS)
  {
    MCM_DATA_LOG_ERROR("mcm_data_create_client_socket:"
                       " Failed with error %d", return_val);
    return_val = MCM_DATA_ERROR;
    goto EXIT;
  }
  MCM_DATA_LOG_INFO("%s", "Client socket successfully created");

  qmi_mcm_data_msgr_state.qmi_dsd_hndl = NULL;

  /* Create and Register err and notify cbs */
  if ( mcm_data_dsd_init() == MCM_DATA_ERROR )
  {
    MCM_DATA_LOG_ERROR("mcm_data_dsd_init failed, bailing");
    goto BAIL;
  }

  MCM_DATA_LOG_INFO("%s", "Successfully registered mcm_data as client"
                          " of DSD service, call mcm_ssr_client_init");
  mcm_ssr_client_init();

  memset(&sys_reg_resp_msg, 0, sizeof(sys_reg_resp_msg));

  qmi_error = qmi_client_send_msg_sync(qmi_mcm_data_msgr_state.qmi_dsd_hndl,
                                QMI_DSD_SYSTEM_STATUS_CHANGE_REQ_V01,
                                NULL,
                                0,
                                &sys_reg_resp_msg,
                                sizeof(sys_reg_resp_msg),
                                10000);

  if (QMI_NO_ERR != qmi_error)
  {
    MCM_DATA_LOG_INFO("Failed to register for DSD System Change Status, err=%d",
                      qmi_error);
    goto BAIL;
  }
  if (QMI_NO_ERR != sys_reg_resp_msg.resp.result)
  {
    MCM_DATA_LOG_INFO("Failed to register for DSD System Change Status, err=%d",
                    sys_reg_resp_msg.resp.error);
    goto BAIL;
  }

BAIL:
  //Initializing call_table and txn_table
  call_table_head = ds_dll_init(NULL);
  txn_table_head = ds_dll_init(NULL);

  //Initializing call_id counter to 0
  call_id_cntr = 0;

  if (call_table_head == NULL || txn_table_head == NULL)
  {
    MCM_DATA_LOG_ERROR("%s", "Failed to allocate memory to"
                       " call_table and txn_table");
    return_val = MCM_DATA_ERROR;
    goto EXIT;
  }

  /* Initilize dsi_netctrl library */
  if (DSI_SUCCESS != dsi_init_ex(DSI_MODE_GENERAL, mcm_data_inited_cb, NULL))
  {
    MCM_DATA_LOG_ERROR("%s", "Failed to initilize dsi_netctrl library");
    return_val = MCM_DATA_ERROR;
    goto EXIT;
  }

  MCM_DATA_LOG_INFO("%s", "Successfully initilized dsi_netctrl library");

  return_val = MCM_DATA_SUCCESS;

EXIT:
  MCM_DATA_LOG_FUNC_EXIT();
  return return_val;
}

void mcm_data_qmi_ssr_ind_handler
(
  mcm_data_modem_ssr_status_type_t *status_info
)
{
  MCM_DATA_LOG_FUNC_ENTRY();

  if( *status_info == MCM_DATA_MODEM_STATUS_OOS  &&
       qmi_mcm_data_msgr_state.mcm_data_modem_local_state
                                                   == MCM_DATA_MODEM_STATUS_IS)
  {
    /* Release objects used for notify cb*/
    MCM_DATA_LOG_HIGH("Clean up and Send MCM_NET_DOWN indication");
    qmi_client_release(qmi_mcm_data_msgr_state.qmi_dsd_hndl);
    qmi_mcm_data_msgr_state.qmi_dsd_hndl = NULL;
    dsi_release(DSI_MODE_GENERAL);
    qmi_mcm_data_msgr_state.mcm_data_modem_local_state = MCM_DATA_MODEM_STATUS_OOS;

    /* the following log might not appear in QXDM logs, dont worry. */
    MCM_DATA_LOG_INFO("%s", "Out of Service event Clean up done");
  }

  else if( *status_info == MCM_DATA_MODEM_STATUS_IS  &&
            qmi_mcm_data_msgr_state.mcm_data_modem_local_state
                                                   == MCM_DATA_MODEM_STATUS_OOS)
  {
    /* Create init objects released due to SSR */
    MCM_DATA_LOG_HIGH("%s", "Initialize and send MCM_NET_UP indication");

    if(qmi_mcm_data_msgr_state.qmi_dsd_hndl != NULL ) {
      MCM_DATA_LOG_ERROR("%s","ERROR In Service Indication without cleanup");
      qmi_client_release(qmi_mcm_data_msgr_state.qmi_dsd_hndl);
      qmi_mcm_data_msgr_state.qmi_dsd_hndl = NULL;
      dsi_release(DSI_MODE_GENERAL);
    }
    mcm_data_dsd_init();
    dsi_init(DSI_MODE_SSR);
    qmi_mcm_data_msgr_state.mcm_data_modem_local_state = MCM_DATA_MODEM_STATUS_IS;

    mcm_ssr_client_send_radio_available();
    MCM_DATA_LOG_INFO("%s", "In Service event initialization done");
  }

  else
  {
    MCM_DATA_LOG_HIGH("Indication ignored, local status: %s , indication that %s",
                       qmi_mcm_data_msgr_state.mcm_data_modem_local_state ==
                         MCM_DATA_MODEM_STATUS_OOS ? "modem in SSR":"modem is UP",
                       *status_info ?  "modem is IS" : "modem is OOS" );
  }
  MCM_DATA_LOG_FUNC_EXIT();
}

static qmi_csi_cb_error qmi_mcm_data_connect_cb
(
  qmi_client_handle        client_handle,
  void                     *service_handle,
  void                     **connection_handle
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  qmi_mcm_data_client_info_type    *clnt_info_ptr;
  qmi_mcm_data_state_info_type     *mcm_data_ptr;
  unsigned int                     index;
  qmi_csi_cb_error                 return_val = QMI_CSI_CB_NO_ERR;

  ASSERT(connection_handle);
  ASSERT(service_handle);

  mcm_data_ptr = (qmi_mcm_data_state_info_type *) service_handle;

  for (index=0; index < MCM_DATA_MAX_CLIENT_HANDLES; index++)
  {
    if (NULL == mcm_data_ptr->client_handle_list[index])
    {
      MCM_DATA_LOG_INFO("qmi_mcm_data_connect_cb: index=%d", index);
      clnt_info_ptr = (qmi_mcm_data_client_info_type *)malloc(
                       sizeof(qmi_mcm_data_client_info_type));
      if (!clnt_info_ptr)
      {
        MCM_DATA_LOG_INFO("%s", "qmi_mcm_data_connect_cb: Out of mem");
        return_val = QMI_CSI_CB_NO_MEM;
        goto BAIL;
      }
      else
        break;
    }
  }

  if (index == MCM_DATA_MAX_CLIENT_HANDLES)
  {
    MCM_DATA_LOG_ERROR("%s", "Client handle slots exhausted");
    return_val = QMI_CSI_CB_NO_MEM;
    goto BAIL;
  }

  memset(clnt_info_ptr, 0x00, sizeof(qmi_mcm_data_client_info_type));
  clnt_info_ptr->clnt = client_handle;
  clnt_info_ptr->client_enabled = TRUE;
  mcm_data_ptr->num_clients++;
  *connection_handle = clnt_info_ptr;
  mcm_data_ptr->client_handle_list[index] = *connection_handle;
  MCM_DATA_LOG_INFO("qmi_mcm_data_connect_cb: Alloc client 0x%p",
                    mcm_data_ptr->client_handle_list[index]);

BAIL:
  MCM_DATA_LOG_FUNC_EXIT();
  return return_val;
}

static void qmi_mcm_data_disconnect_cb
(
  void   *connection_handle,
  void   *service_handle
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  qmi_mcm_data_state_info_type     *mcm_data_ptr;
  unsigned int                     client_index;
  unsigned int                     index;
  boolean                          last_client=TRUE;
  qmi_mcm_data_client_info_type    *clnt_info;

  ASSERT(service_handle);
  mcm_data_ptr = (qmi_mcm_data_state_info_type *)service_handle;

  for (client_index=0;
       ((client_index < MCM_DATA_MAX_CLIENT_HANDLES) &&
        (client_index < mcm_data_ptr->num_clients));
       client_index++)
  {
    if (mcm_data_ptr->client_handle_list[client_index] == connection_handle)
      break;
  }

  if (client_index == MCM_DATA_MAX_CLIENT_HANDLES)
  {
    MCM_DATA_LOG_ERROR("qmi_mcm_data_disconnect_cb: Invalid Handle %p",
                       connection_handle);
    goto BAIL;
  }

  clnt_info = (qmi_mcm_data_client_info_type *)
                  (mcm_data_ptr->client_handle_list[client_index]);

  MCM_DATA_LOG_INFO("Received disconnect from client %d with client"
                    " enabled as %d, Clt ref cnt %d",
                    mcm_data_ptr->client_handle_list[client_index],
                    clnt_info->client_enabled,
                    mcm_data_ptr->client_ref_count);

  mcm_data_release_client(clnt_info);
  MCM_DATA_LOG_INFO("qmi_mcm_data_disconnect_cb: Releasing client %p,"
                    " num_clients: %d",
                    mcm_data_ptr->client_handle_list[client_index],
                    mcm_data_ptr->num_clients);

  if (clnt_info->client_enabled)
  {
    if (mcm_data_ptr->client_ref_count > 1)
    {
      mcm_data_ptr->client_ref_count--;
      clnt_info->client_enabled = FALSE;
    }
    else if (mcm_data_ptr->client_ref_count == 1)
    {
      mcm_data_ptr->client_ref_count--;
      clnt_info->client_enabled = FALSE;
      MCM_DATA_LOG_ERROR("%s", "qmi_mcm_data_disconnect_cb:"
                         " No clients available now");
    }
  }

  mcm_data_ptr->num_clients--;
  free(mcm_data_ptr->client_handle_list[client_index]);
  mcm_data_ptr->client_handle_list[client_index] = NULL;

  for (index=0; (index < MCM_DATA_MAX_CLIENT_HANDLES); index++)
  {
    if (mcm_data_ptr->client_handle_list[index] != NULL)
    {
      last_client=FALSE;
      break;
    }
  }

  MCM_DATA_LOG_INFO("qmi_mcm_data_disconnect_cb: Last_client: %d",
                    last_client);
  if (last_client)
  {
    MCM_DATA_LOG_INFO("%s", "qmi_mcm_data_disconnect_cb: Last Client True!!");
  }

BAIL:
  MCM_DATA_LOG_FUNC_EXIT();
  return;
}

static qmi_csi_cb_error qmi_mcm_data_handle_client_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_handle
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  qmi_csi_cb_error                rc;
  qmi_mcm_data_client_info_type   *clnt_info;
  qmi_mcm_data_state_info_type    *mcm_data_ptr;
  unsigned int                    client_index;

  ASSERT(connection_handle);
  ASSERT(service_handle);

  rc = QMI_CSI_CB_INTERNAL_ERR;
  clnt_info = (qmi_mcm_data_client_info_type*)connection_handle;
  mcm_data_ptr = (qmi_mcm_data_state_info_type*) service_handle;

  for (client_index = 0;
       ((client_index < MCM_DATA_MAX_CLIENT_HANDLES) &&
        (client_index < mcm_data_ptr->num_clients));
       client_index++)
  {
    if (mcm_data_ptr->client_handle_list[client_index] == clnt_info)
    {
      MCM_DATA_LOG_INFO("Looking for client %p...Found", clnt_info);
      break;
    }
  }

  if (client_index == MCM_DATA_MAX_CLIENT_HANDLES)
  {
      MCM_DATA_LOG_INFO("qmi_mcm_data_handle_client_req_cb:"
                        " Invalid client handle %p", clnt_info);
      goto BAIL;
  }

  if ((msg_id - MCM_DATA_START_DATA_CALL_REQ_V01) <
      (sizeof(mcm_data_req_handle_table) / sizeof(*mcm_data_req_handle_table)))
  {
    if (mcm_data_req_handle_table[msg_id - MCM_DATA_START_DATA_CALL_REQ_V01])
    {
      rc = mcm_data_req_handle_table[msg_id - MCM_DATA_START_DATA_CALL_REQ_V01]
                                    (clnt_info, req_handle, msg_id,
                                     req_c_struct, req_c_struct_len,
                                     service_handle,client_index);
    }
    else
    {
      MCM_DATA_LOG_INFO("qmi_mcm_data_handle_client_req_cb:"
                        " NULL message ID handler: %d", msg_id);
    }
  }
  else
  {
    MCM_DATA_LOG_INFO("qmi_mcm_data_handle_client_req_cb:"
                      " Invalid message ID: %d", msg_id);
  }

BAIL:
  MCM_DATA_LOG_FUNC_EXIT();
  return rc;
}

void mcm_data_dsi_net_ev_cb
(
  dsi_hndl_t        hndl,
  void              *user_data,
  dsi_net_evt_t     evt,
  dsi_evt_payload_t *payload_ptr
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  mcm_data_event_data_t mcm_data_buffer;
  int numBytes=0, len;
  struct sockaddr_un dsi_mcm_data_;

  dsi_mcm_data_.sun_family = AF_UNIX;
  std_strlcpy(dsi_mcm_data_.sun_path,
              MCM_DATA_DSI_UDS_FILE,
              MCM_DATA_DSI_UDS_FILE_LEN);
  len = strlen(dsi_mcm_data_.sun_path)+sizeof(dsi_mcm_data_.sun_family);

  mcm_data_buffer.event_id = MCM_DATA_DSI_EVENT;

  mcm_data_buffer.evt_data.dsi_event.dsi_nethandle = hndl;
  mcm_data_buffer.evt_data.dsi_event.user_data = user_data;
  mcm_data_buffer.evt_data.dsi_event.evt = evt;
  mcm_data_buffer.evt_data.dsi_event.payload_ptr = payload_ptr;

  if ((numBytes = sendto(mcm_data_cli_sockfd,
                         (void *)&mcm_data_buffer,
                         sizeof(mcm_data_event_data_t),
                         0,
                         (struct sockaddr *)&dsi_mcm_data_,
                         len)) == -1)
  {
    MCM_DATA_LOG_ERROR("%s", "Failed to send dsi event to client");
    goto BAIL;
  }
  MCM_DATA_LOG_INFO("%s", "Successfully sent dsi event to client");

BAIL:
  MCM_DATA_LOG_FUNC_EXIT();
  return;
}

/**
  @brief
  This method handles events generated by dsi_netctrl.

  @params[in] dsi_hndl: DSI Handle associated with the data call.
  @params[in] user_data: User data sent while regitering for the event.
  @params[in] evt: Type of event captured by the handler.
  @params[in] payload: Cached event payload.

  @details
  This method sends request to stop a data call for a given handle.

  @return MCM_SUCCESS indicates the request was issued successfully to modem.
  @return negative value indicates error code
*/
void mcm_data_dsi_net_ev_handler
(
  dsi_hndl_t        hndl,
  void              *user_data,
  dsi_net_evt_t     evt,
  dsi_evt_payload_t *payload_ptr
)
{
  qmi_csi_error   rc;
  dsi_ce_reason_t ce_reason;
  char            buf[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+1] = {0};
  int             qmi_err_code;

  int result, qmi_err;

  mcm_data_unsol_event_ind_msg_v01     unsol_data_ind_msg;
  mcm_data_start_data_call_rsp_msg_v01 start_resp_msg;
  mcm_data_stop_data_call_rsp_msg_v01  stop_resp_msg;
  mcm_data_call_table_entry            *call_entry;
  mcm_data_txn_table_entry             *txn_entry;

  MCM_DATA_LOG_INFO("DSI Event received - %d", evt);
  if (evt > DSI_EVT_INVALID && evt < DSI_EVT_MAX)
  {
    mcm_data_call_table_entry* call_info =
                               (mcm_data_call_table_entry *)user_data;
    MCM_DATA_LOG_INFO("Call table entry - call_id %d, dsi_handle %p"
                      ", QMI handle %d",
                      call_info->call_id,
                      call_info->dsi_handle,
                      call_info->qmi_cli_handle);

    call_entry = (mcm_data_call_table_entry *) get_calltbl_entry_i(
                                   call_info->call_id);
    /* Couldn't find required call ID in the call_table */
    if (call_entry == NULL)
    {
      /* Cleaning the resources attached to unknown call_id. */
      MCM_DATA_LOG_ERROR("Cannot find call_id %d in call_table."
                         " Releasing resources",
                         call_info->call_id);
      del_txntbl_entry_callid_i(call_info->call_id, ALL);
      goto BAIL;
    }

    MCM_DATA_LOG_INFO("Found call_id [%d] for the event [%d]"
          "with dsi_hdnl [%p]",
                      call_info->call_id,
                      evt,
                      call_info->dsi_handle);
    print_txntbl_entries_i();
    switch (evt)
    {
      case DSI_EVT_NET_IS_CONN :
        memset(&buf, 0x00, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+1);
        MCM_DATA_LOG_INFO("Retrieving device_name for dsi_handle %p",
                          call_info->dsi_handle);
        if (DSI_SUCCESS != dsi_get_qmi_port_name(call_info->dsi_handle,
                                         (char *) &buf,
                                         DSI_CALL_INFO_DEVICE_NAME_MAX_LEN + 1))
        {
          MCM_DATA_LOG_ERROR("%s","Failed to get device_name. Cleaning entries");
          del_calltbl_entry_callid_i(call_info->call_id);
          del_txntbl_entry_callid_i(call_info->call_id, ALL);
          goto BAIL;
        }
        else
        {
          buf[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN] = '\0';
          MCM_DATA_LOG_INFO("Success: Retrieved device_name [%s]", buf);
        }

        rc = qmi_client_wds_init_instance(buf,
                                          wds_get_service_object_v01(),
                                          QMI_CLIENT_INSTANCE_ANY,
                                          (qmi_client_ind_cb)mcm_data_qmi_wds_ind_cb,
                                          user_data,
                                          &call_info->wds_cli_os_params,
                                          MCM_DATA_QMI_WDS_TIMEOUT,
                                          &call_info->qmi_wds_hndl);


        if (rc != QMI_NO_ERR)
        {
          MCM_DATA_LOG_ERROR("qmi_client_init: Failed to initialize mcm_data"
                             " as client of WDS service with error %d",
                             rc);
          del_calltbl_entry_callid_i(call_info->call_id);
          del_txntbl_entry_callid_i(call_info->call_id, ALL);
          goto BAIL;
        }
        else
        {
          wds_set_event_report_req_msg_v01 req;
          wds_set_event_report_resp_msg_v01 resp;

          memset(&resp, 0, sizeof(resp));
          memset(&req, 0, sizeof(req));

          req.report_data_bearer_tech_ex = TRUE;
          req.report_data_bearer_tech_ex_valid = TRUE;

          req.report_dormancy_status = TRUE;
          req.report_dormancy_status_valid = TRUE;


          rc = qmi_client_send_msg_sync (call_info->qmi_wds_hndl,
                                         QMI_WDS_SET_EVENT_REPORT_REQ_V01,
                                         (void *)&req,
                                         sizeof(req),
                                         (void*)&resp,
                                         sizeof(resp),
                                         MCM_DATA_QMI_WDS_TIMEOUT);


          if (rc != QMI_NO_ERR || resp.resp.result != QMI_RESULT_SUCCESS_V01)
          {
            MCM_DATA_LOG_ERROR("%s(): failed with rc=%d, qmi_err=%d",
                             __func__, rc, resp.resp.error);
            goto BAIL;
          }
        }

        memset(&unsol_data_ind_msg, 0x00,
               sizeof(mcm_data_unsol_event_ind_msg_v01));
        unsol_data_ind_msg.call_id_valid = TRUE;
        unsol_data_ind_msg.call_id = call_info->call_id;
        unsol_data_ind_msg.call_status_valid = TRUE;
        unsol_data_ind_msg.call_status =
                                  MCM_DATA_CALL_STATE_CONNECTED_V01;
        unsol_data_ind_msg.event_id = MCM_DATA_NET_UP_EV_V01;

        unsol_data_ind_msg.addr_count_valid = TRUE;
        unsol_data_ind_msg.addr_count =
                    dsi_get_ip_addr_count(call_info->dsi_handle);

        if(DSI_SUCCESS == dsi_get_ip_addr(call_info->dsi_handle,
                                (dsi_addr_info_t *)unsol_data_ind_msg.addr_info,
                                unsol_data_ind_msg.addr_count))
        {
          unsol_data_ind_msg.addr_info_valid = TRUE;
          unsol_data_ind_msg.addr_info_len =  unsol_data_ind_msg.addr_count;
        }

        txn_entry = (mcm_data_txn_table_entry *) get_txntbl_entry_i(
                                   call_info->call_id, START);
        MCM_DATA_LOG_INFO("Got Txn entry with callid %d : %p",
                           call_info->call_id,
                           txn_entry);
        if (txn_entry != NULL)
        {
          memset(&start_resp_msg, 0x00,
                 sizeof(mcm_data_start_data_call_rsp_msg_v01));

          start_resp_msg.call_id = txn_entry->call_id;
          start_resp_msg.call_id_valid = TRUE;
          start_resp_msg.resp.error = MCM_SUCCESS_V01;
          start_resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
          dsi_get_call_end_reason(hndl, &ce_reason,DSI_IP_FAMILY_V4);

          if((ce_reason.reason_type == DSI_CE_TYPE_INVALID) ||
              (ce_reason.reason_type == DSI_CE_TYPE_UNINIT))
          {
            dsi_get_call_end_reason(hndl, &ce_reason,DSI_IP_FAMILY_V6);
          }

          if((ce_reason.reason_type != DSI_CE_TYPE_INVALID) &&
             (ce_reason.reason_type != DSI_CE_TYPE_UNINIT))
          {
            start_resp_msg.vce_reason_valid = TRUE;
            start_resp_msg.vce_reason.call_end_reason_code =
                              convert_wds_ce_reason_code_to_mcm(ce_reason);
            start_resp_msg.vce_reason.call_end_reason_type =
                              convert_wds_ce_reason_type_to_mcm(ce_reason);
          }

          MCM_DATA_LOG_INFO("vce_reason_valid [%d] Reason code [%d] Reason type [%d]",
                            start_resp_msg.vce_reason_valid,
                            start_resp_msg.vce_reason.call_end_reason_code,
                            start_resp_msg.vce_reason.call_end_reason_type);

          rc = qmi_csi_send_resp(txn_entry->req_handle,
                                 MCM_DATA_START_DATA_CALL_RSP_V01,
                                 &start_resp_msg,
                                 sizeof(mcm_data_start_data_call_rsp_msg_v01));
          if( rc == QMI_CSI_NO_ERR )
          {
            MCM_DATA_LOG_INFO("Successfully sent async start_data_call "
                              "response to call_id %d",
                               start_resp_msg.call_id);
          }
          else
          {
            MCM_DATA_LOG_ERROR("Failed to send async start_data_call response"
                              "to call_id %d due to error %d",
                               start_resp_msg.call_id,
                               rc);
          }
          MCM_DATA_LOG_INFO("Deleting Start Txn entry with call_id [%d]",
                          txn_entry->call_id,
                          txn_entry);
          del_txntbl_entry_callid_i(txn_entry->call_id, START);

          MCM_DATA_LOG_INFO("%s", "Sending Net Up indication to clients");
          rc = mcm_data_send_indication(&unsol_data_ind_msg);
        }
        else
        {
          // Clearing call table entry with respective to this call_id.
          MCM_DATA_LOG_ERROR("%s", "Start data call entry not found for the"
                             " event received. Terminating call");
          // Releasing dsi_handle
          dsi_rel_data_srvc_hndl(call_info->dsi_handle);
          del_calltbl_entry_callid_i(call_info->call_id);
          del_txntbl_entry_callid_i(call_info->call_id, ALL);
        }
        break;

      case DSI_EVT_NET_NO_NET :
        memset(&unsol_data_ind_msg, 0x00,
               sizeof(mcm_data_unsol_event_ind_msg_v01));
        unsol_data_ind_msg.call_id_valid = TRUE;
        unsol_data_ind_msg.call_id = call_info->call_id;
        unsol_data_ind_msg.call_status_valid = TRUE;
        unsol_data_ind_msg.call_status =
                                  MCM_DATA_CALL_STATE_DISCONNECTED_V01;
        unsol_data_ind_msg.event_id = MCM_DATA_NET_DOWN_EV_V01;

        unsol_data_ind_msg.addr_count_valid = FALSE;
        unsol_data_ind_msg.addr_info_valid = FALSE;

        txn_entry = (mcm_data_txn_table_entry *) get_txntbl_entry_i(
                                   call_info->call_id, START);

        if (txn_entry != NULL)
        {
           MCM_DATA_LOG_INFO("Found a pending start transaction entry with"
                             " call_id [%d]",
                             txn_entry->call_id);
           memset(&start_resp_msg, 0x00,
                         sizeof(mcm_data_start_data_call_rsp_msg_v01));

           start_resp_msg.resp.error = MCM_SUCCESS_V01;
           start_resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;

           dsi_get_call_end_reason(hndl, &ce_reason,DSI_IP_FAMILY_V4);
           if((ce_reason.reason_type == DSI_CE_TYPE_INVALID) ||
               (ce_reason.reason_type == DSI_CE_TYPE_UNINIT))
           {
             dsi_get_call_end_reason(hndl, &ce_reason,DSI_IP_FAMILY_V6);
           }

           if((ce_reason.reason_type != DSI_CE_TYPE_INVALID) &&
              (ce_reason.reason_type != DSI_CE_TYPE_UNINIT))
           {

             start_resp_msg.vce_reason_valid = TRUE;
             start_resp_msg.vce_reason.call_end_reason_code =
                              convert_wds_ce_reason_code_to_mcm(ce_reason);
             start_resp_msg.vce_reason.call_end_reason_type =
                              convert_wds_ce_reason_type_to_mcm(ce_reason);
           }

          MCM_DATA_LOG_INFO("vce_reason_valid %d Reason code %d Reason type %d",
                            start_resp_msg.vce_reason_valid,
                            start_resp_msg.vce_reason.call_end_reason_code,
                            start_resp_msg.vce_reason.call_end_reason_type);

          rc = qmi_csi_send_resp(txn_entry->req_handle,
                                 MCM_DATA_START_DATA_CALL_RSP_V01,
                                 &start_resp_msg,
                                 sizeof(mcm_data_start_data_call_rsp_msg_v01));
          if( rc == QMI_CSI_NO_ERR )
          {
            MCM_DATA_LOG_INFO("Successfully sent async start_data_call "
                              "FAILURE response to call_id %d",
                               start_resp_msg.call_id);
          }
          else
          {
            MCM_DATA_LOG_ERROR("Failed to send async start_data_call FAILURE"
                              "response to call_id %d due to error %d",
                               start_resp_msg.call_id,
                               rc);
          }
        }

        txn_entry = (mcm_data_txn_table_entry *) get_txntbl_entry_i(
                                    call_info->call_id, STOP);
        if (txn_entry != NULL)
        {
          stop_resp_msg.resp.error = MCM_SUCCESS_V01;
          stop_resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
          rc = qmi_csi_send_resp(txn_entry->req_handle,
                            MCM_DATA_STOP_DATA_CALL_RSP_V01 , &stop_resp_msg,
                            sizeof(mcm_data_stop_data_call_rsp_msg_v01));
          if( rc == QMI_CSI_NO_ERR )
          {
            MCM_DATA_LOG_INFO("Successfully sent async stop_data_call "
                              "response to call_id %d", txn_entry->call_id);
          }
          else
          {
            MCM_DATA_LOG_ERROR("Failed to send async stop_data_call "
                               "response to call_id %d due to error %d",
                               txn_entry->call_id,
                               rc);
          }

           /* Deleting all Transaction entries with given callid*/
           MCM_DATA_LOG_INFO("Deleting Txn entries with call_id [%d]"
                              " @ [%p]",
                              txn_entry->call_id,
                              txn_entry);
           del_txntbl_entry_callid_i(txn_entry->call_id, ALL);
         }

        MCM_DATA_LOG_INFO("%s", "Sending Net Down indication to clients");
        rc = mcm_data_send_indication(&unsol_data_ind_msg);

        // Releasing dsi_handle
        MCM_DATA_LOG_INFO("Releasing dsi_handle : %p", call_info->dsi_handle);
        dsi_rel_data_srvc_hndl(call_info->dsi_handle);

        if (MCM_DATA_INVALID_CLT_HNDL != call_entry->qmi_wds_hndl)
        {
          MCM_DATA_LOG_INFO("Releasing qmi_wds_hndl [0x%x]",
                            call_info->qmi_wds_hndl);
          qmi_client_release(call_info->qmi_wds_hndl);
        }

        // Clearing call table entry with respective to this call_id.
        del_calltbl_entry_callid_i(call_info->call_id);
        break;
      case DSI_EVT_NET_NEWADDR :
      case DSI_EVT_NET_DELADDR :

        memset(&unsol_data_ind_msg, 0x00,
               sizeof(mcm_data_unsol_event_ind_msg_v01));
        unsol_data_ind_msg.call_id_valid = TRUE;
        unsol_data_ind_msg.call_id = call_info->call_id;
        unsol_data_ind_msg.call_status_valid = TRUE;
        unsol_data_ind_msg.call_status =
                                  MCM_DATA_CALL_STATE_CONNECTED_V01;
        unsol_data_ind_msg.event_id = ((evt == DSI_EVT_NET_NEWADDR)?
                  MCM_DATA_NET_NEW_ADDR_EV_V01 : MCM_DATA_NET_DEL_ADDR_EV_V01) ;

        unsol_data_ind_msg.addr_count_valid = TRUE;
        unsol_data_ind_msg.addr_count =
                    dsi_get_ip_addr_count(call_info->dsi_handle);

        if(DSI_SUCCESS == dsi_get_ip_addr(call_info->dsi_handle,
                                (dsi_addr_info_t *)unsol_data_ind_msg.addr_info,
                                unsol_data_ind_msg.addr_count))
        {
          unsol_data_ind_msg.addr_info_valid = TRUE;
          unsol_data_ind_msg.addr_info_len =  unsol_data_ind_msg.addr_count;
        }
        MCM_DATA_LOG_INFO("%s", "Sending New/Del Address indication"
                          " to clients");
        rc = mcm_data_send_indication(&unsol_data_ind_msg);

        break;
      default:
        MCM_DATA_LOG_INFO("Received unknown event [%d]", evt);
        goto BAIL;
    }
  }
  BAIL:
    return;
}

static qmi_csi_cb_error  mcm_data_start_data_call
(
  qmi_mcm_data_client_info_type *clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                          *req_c_struct,
  int                           req_c_struct_len,
  void                          *service_handle,
  unsigned int                  client_index
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  mcm_data_start_data_call_rsp_msg_v01 resp_msg;
  mcm_data_start_data_call_req_msg_v01 *req_msg;
  uint32 call_id;
  dsi_call_param_value_t parameter;
  mcm_data_call_table_entry *call_entry;
  mcm_data_txn_table_entry *txn_entry;

  MCM_DATA_LOG_INFO("Client %p requested for Start Data Call", clnt_info);
  memset(&resp_msg, 0, sizeof(mcm_data_start_data_call_rsp_msg_v01));
  req_msg = (mcm_data_start_data_call_req_msg_v01 *)req_c_struct;
  if (!qmi_mcm_data_msgr_state.mcm_data_inited)
  {
    MCM_DATA_LOG_ERROR("%s", "mcm_data not inited");
    resp_msg.resp.error = MCM_ERROR_DEVICE_NOT_READY_V01;
    goto BAIL;
  }

  if (req_msg == NULL)
  {
    MCM_DATA_LOG_ERROR("%s", "NULL req_c_struct received from client");
    resp_msg.resp.error = MCM_ERROR_INVALID_ARG_V01;
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    goto BAIL;
  }

  call_entry = NULL;
  txn_entry = NULL;

  if ((call_entry = (mcm_data_call_table_entry *)
                    malloc(sizeof(mcm_data_call_table_entry))) == NULL)
  {
     resp_msg.resp.error = MCM_ERROR_NO_MEMORY_V01;
     resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
     MCM_DATA_RETURN_ERROR();
  }

  if ((txn_entry = (mcm_data_txn_table_entry *)
                   malloc(sizeof(mcm_data_txn_table_entry))) == NULL)
  {
    resp_msg.resp.error = MCM_ERROR_NO_MEMORY_V01;
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    MCM_DATA_RETURN_ERROR();
  }

  memset(txn_entry, 0x00, sizeof(mcm_data_txn_table_entry));
  memset(call_entry, 0x00, sizeof(mcm_data_call_table_entry));

  /* Get required dsi_handle to bringup the call. */
  if ((call_entry->dsi_handle =
       dsi_get_data_srvc_hndl(&mcm_data_dsi_net_ev_cb,
                              (void*)call_entry)) == NULL)
  {
    /* dsi_handle was not generated. */
    MCM_DATA_LOG_ERROR("%s", "Failed to get dsi_handle");
    resp_msg.resp.error = MCM_ERROR_INTERNAL_V01;
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    MCM_DATA_RETURN_ERROR();
  }

  MCM_DATA_LOG_INFO("Obtained dsi_hndl : %p",
                    call_entry->dsi_handle);

  /* Save client handle into this empty entry. */
  call_entry->qmi_cli_handle = clnt_info->clnt;
  call_entry->self = call_entry;

  memset (&parameter, 0x00, sizeof(dsi_call_param_value_t));
  /* For now we support only CDMA and UMTS*/
  if ((req_msg->tech_pref > DSI_EXT_TECH_INVALID &&
       req_msg->tech_pref < DSI_EXT_TECH_EMBMS) &&
       (req_msg->tech_pref_valid == TRUE))
  {
    /* Setting Tech preference. */
    /* Following technology bitmap defined in dsi_netctrl. */
    parameter.buf_val = NULL;
    parameter.num_val = req_msg->tech_pref;
    if (dsi_set_data_call_param(call_entry->dsi_handle,
                                DSI_CALL_INFO_TECH_PREF,
                                &parameter) == DSI_ERROR)
    {
      MCM_DATA_LOG_ERROR("Failed to set tech preference : ' %d'"
                         "  in set_data_call_param",
                         req_msg->tech_pref);
      resp_msg.resp.error = MCM_ERROR_INVALID_TECH_PREF_V01;
      resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
      MCM_DATA_RETURN_ERROR();
    }
  }

    memset (&parameter, 0x00, sizeof(dsi_call_param_value_t));
    if ((strlen(req_msg->apn_name) > 0) &&
         (req_msg->apn_name_valid == TRUE))
    {
      /* Setting APN Name in case of 3gpp. */
      parameter.buf_val = req_msg->apn_name;
      parameter.num_val = strlen(parameter.buf_val);
      MCM_DATA_LOG_INFO("Setting APN to %s", req_msg->apn_name);
      if (dsi_set_data_call_param(call_entry->dsi_handle,
                                  DSI_CALL_INFO_APN_NAME,
                                  &parameter) == DSI_ERROR)
      {
        MCM_DATA_LOG_ERROR("Failed to set APN name : ' %s'"
                           "  in set_data_call_param",
                           req_msg->apn_name);
        resp_msg.resp.error = MCM_ERROR_INVALID_PDP_TYPE_V01;
        resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
        MCM_DATA_RETURN_ERROR();
      }
    }

    memset (&parameter, 0x00, sizeof(dsi_call_param_value_t));

    /* Setting Profile ID. */
    /* We do not support profile ID '0'. */
    if ((req_msg->umts_profile >= 0) &&
         (req_msg->umts_profile_valid == TRUE))
    {
      parameter.buf_val = NULL;
      parameter.num_val = req_msg->umts_profile;
      MCM_DATA_LOG_INFO("Setting 3GPP PROFILE to %d",
                         parameter.num_val);
      if (dsi_set_data_call_param(call_entry->dsi_handle,
                                  DSI_CALL_INFO_UMTS_PROFILE_IDX,
                                  &parameter) == DSI_ERROR)
      {
        MCM_DATA_LOG_ERROR("%s", "Failed to set 3GPP profile ID");
        resp_msg.resp.error = MCM_ERROR_INVALID_PROFILE_TYPE_V01;
        resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
        MCM_DATA_RETURN_ERROR();
      }
    }
    if ((req_msg->cdma_profile>= 0) &&
         (req_msg->cdma_profile_valid == TRUE))
    {
      parameter.buf_val = NULL;
      parameter.num_val = req_msg->cdma_profile;
      MCM_DATA_LOG_INFO("Setting 3GPP PROFILE to %d",
                         parameter.num_val);
      if (dsi_set_data_call_param(call_entry->dsi_handle,
                                  DSI_CALL_INFO_CDMA_PROFILE_IDX,
                                  &parameter) == DSI_ERROR)
      {
        MCM_DATA_LOG_ERROR("%s", "Failed to set 3GPP profile ID");
        resp_msg.resp.error = MCM_ERROR_INVALID_PROFILE_TYPE_V01;
        resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
        MCM_DATA_RETURN_ERROR();
      }
    }

    if ((req_msg->cdma_profile_valid == FALSE) &&
       (req_msg->umts_profile_valid == FALSE))
    {
      MCM_DATA_LOG_ERROR("%s", "Failed to set Profile ID'");
      resp_msg.resp.error = MCM_ERROR_INVALID_PROFILE_TYPE_V01;
      resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
      MCM_DATA_RETURN_ERROR();
    }

    memset (&parameter, 0x00, sizeof(dsi_call_param_value_t));
    /* Setting Username. */
    if ((strlen(req_msg->user_name) > 0) &&
         (req_msg->user_name_valid == TRUE))
    {
      parameter.buf_val = req_msg->user_name;
      parameter.num_val = strlen(req_msg->user_name);
      if (dsi_set_data_call_param(call_entry->dsi_handle,
                                  DSI_CALL_INFO_USERNAME,
                                  &parameter) == DSI_ERROR)
      {
        MCM_DATA_LOG_ERROR("%s", "Failed to set User Name");
        resp_msg.resp.error = MCM_ERROR_AUTHENTICATION_FAILED_V01;
        resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
        MCM_DATA_RETURN_ERROR();
      }
    }

    memset (&parameter, 0x00, sizeof(dsi_call_param_value_t));
    /* Setting Password. */
    if ((strlen(req_msg->password) > 0) &&
         (req_msg->password_valid == TRUE))
    {
      parameter.buf_val = req_msg->password;
      parameter.num_val = strlen(parameter.buf_val);
      if (dsi_set_data_call_param(call_entry->dsi_handle,
                                  DSI_CALL_INFO_PASSWORD,
                                  &parameter) == DSI_ERROR)
      {
        MCM_DATA_LOG_ERROR("%s", "Failed to set Password");
        resp_msg.resp.error = MCM_ERROR_INCORRECT_PIN_V01;
        resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
        MCM_DATA_RETURN_ERROR();
      }
    }

    memset (&parameter, 0x00, sizeof(dsi_call_param_value_t));
    /* Setting IP Family. */
    /* Following DSI_IP_VERSIONS. */
    /* We support ony IPV4 for now */
    if ((req_msg->ip_family == DSI_IP_VERSION_4 ||
          req_msg->ip_family == DSI_IP_VERSION_6) &&
         (req_msg->ip_family_valid == TRUE))
    {
      parameter.num_val = req_msg->ip_family;
    }
    else
    {
      parameter.num_val = DSI_IP_VERSION_4;
      MCM_DATA_LOG_ERROR("%s", "Invalid IP family. Setting IP"
                         " Family to IPV4");
    }

    parameter.buf_val = NULL;
    MCM_DATA_LOG_INFO("Setting family to %d",
                      parameter.num_val);
    if (dsi_set_data_call_param(call_entry->dsi_handle,
                                DSI_CALL_INFO_IP_VERSION,
                                &parameter) == DSI_ERROR)
    {
      MCM_DATA_LOG_ERROR("%s", "Failed to set IP Family");
      resp_msg.resp.error = MCM_ERROR_INVALID_IP_FAMILY_PREF_V01;
      resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
      MCM_DATA_RETURN_ERROR();
    }

    /* Storing Token information in token table. */

    txn_entry->api_type = MCM_DATA_API_START_DATA_CALL;
    txn_entry->self = txn_entry;
    txn_entry->qmi_cli_handle = clnt_info->clnt;
    txn_entry->req_handle = req_handle;

    /* Setting call_id with index number of the entry. */
    call_entry->call_id = ++call_id_cntr;
    txn_entry->call_id = call_entry->call_id;
    call_entry->qmi_cli_handle = clnt_info->clnt;

    MCM_DATA_LOG_INFO("start data call : call id :"
                      " %d, dsi handle obtained : %p,",
                      call_entry->call_id,
                      call_entry->dsi_handle);

    ds_dll_enq(get_call_tbl_head(), NULL, call_entry);
    ds_dll_enq(get_txn_tbl_head(), NULL, txn_entry);

    /* Starting Data Call. */
    if (dsi_start_data_call(call_entry->dsi_handle) == DSI_ERROR)
    {
       MCM_DATA_LOG_ERROR("%s", "Failed to Start data call");
       resp_msg.resp.error = MCM_ERROR_CALL_FAILED_V01;
       resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
       MCM_DATA_RETURN_ERROR();
    }

  resp_msg.resp.error = MCM_DATA_SUCCESS;
  resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
BAIL:
  if(resp_msg.resp.error != MCM_DATA_SUCCESS)
  {
    MCM_DATA_LOG_INFO("%s", "mcm_data_start_data_call: Failed");
    resp_msg.call_id = 0;
    resp_msg.call_id_valid = FALSE;
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_data_start_data_call_rsp_msg_v01));
  }
  MCM_DATA_LOG_FUNC_EXIT();
  return QMI_CSI_CB_NO_ERR;
}

static qmi_csi_cb_error  mcm_data_stop_data_call(
  qmi_mcm_data_client_info_type *clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                          *req_c_struct,
  int                           req_c_struct_len,
  void                          *service_handle,
  unsigned int                    client_index
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  mcm_data_stop_data_call_rsp_msg_v01 resp_msg;
  mcm_data_stop_data_call_req_msg_v01 *req_msg;
  mcm_data_call_table_entry *call_entry;
  mcm_data_txn_table_entry *txn_entry;
  ds_dll_el_t *call_table_node;
  ds_dll_el_t *txn_table_node;
  ds_dll_el_t *call_table_tail = NULL;
  ds_dll_el_t *txn_table_tail = NULL;
  const void *dummy = NULL;
  int rc;

   /* This variable tracks exhausted token_id or failure in stop data call
      till end of the function.*/
  int end_call_failed = 0;

  req_msg = (mcm_data_stop_data_call_req_msg_v01 *)req_c_struct;

  MCM_DATA_LOG_INFO ("Client %p requested for Stop Data Call on call_id [%d]",
                     clnt_info,
                     req_msg->call_id);

  memset(&resp_msg, 0, sizeof(mcm_data_stop_data_call_rsp_msg_v01));

  if (!qmi_mcm_data_msgr_state.mcm_data_inited)
  {
    MCM_DATA_LOG_ERROR("%s", "mcm_data not inited");
    resp_msg.resp.error = MCM_ERROR_DEVICE_NOT_READY_V01;
    goto BAIL;
  }

  call_entry = NULL;
  txn_entry = NULL;
  call_table_node = NULL;
  txn_table_node = NULL;

  call_entry = (mcm_data_call_table_entry *)
                   malloc(sizeof(mcm_data_call_table_entry));
  txn_entry = (mcm_data_txn_table_entry *)
                   malloc(sizeof(mcm_data_txn_table_entry));

  if (call_entry == NULL || txn_entry == NULL)
  {
    MCM_DATA_LOG_ERROR ("%s", "Failed to allocate memory to"
                        " call/txn table entry");
    resp_msg.resp.error = MCM_ERROR_NO_MEMORY_V01;
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    MCM_DATA_RETURN_ERROR();
  }
  memset(txn_entry, 0x00, sizeof(mcm_data_txn_table_entry));
  memset(call_entry, 0x00, sizeof(mcm_data_call_table_entry));

  call_entry->call_id = req_msg->call_id;
  txn_entry->call_id = req_msg->call_id;

  /* Find the cal_id in the call_table */
  MCM_DATA_LOG_INFO("Searching for call_id [%d] in call_table",
                    req_msg->call_id);
  call_table_node = ds_dll_search(get_call_tbl_head(),
                                  (const void *) call_entry,
                                  isvalid_callid_calltbl_i);

  if (call_table_node == NULL)
  {
    /* Failed to find call_id in call_table */
    MCM_DATA_LOG_ERROR("Unable to find call_id [%d] in call table"
                       " for client [%p]",
                       req_msg->call_id,
                       clnt_info->clnt);

    /*Find tail of transaction table list*/
    txn_table_node = ds_dll_next(get_txn_tbl_head(), &dummy);
    while (txn_table_node != NULL)
    {
      txn_table_tail = txn_table_node;
      txn_table_node = ds_dll_next(txn_table_node, &dummy);
    }

    do
    {
      txn_table_node = ds_dll_delete(get_txn_tbl_head(),
                                     &txn_table_tail,
                                     (const void *) txn_entry,
                                     isvalid_callid_txntbl_i);
      if (txn_table_node != NULL)
      {
        /*Found a transaction table entry wih invalid call_id.*/
        ds_dll_free (txn_table_node);
      }
    }while(txn_table_node != NULL);

    resp_msg.resp.error = MCM_ERROR_NO_ENTRY_V01;
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    goto BAIL;
  }

  /*Re-using call entry pointing to appropriate data*/
  memset(call_entry, 0x00, sizeof(mcm_data_call_table_entry));
  call_entry = (mcm_data_call_table_entry *) call_table_node->data;

  MCM_DATA_LOG_INFO("Found call_id [%d] in call_table with dsi_handle [%p]",
                    call_entry->call_id,
                    call_entry->dsi_handle);

  /* Stop data call with the given handle. */
  if ( (rc = dsi_stop_data_call(call_entry->dsi_handle)) == DSI_ERROR)
  {
    MCM_DATA_LOG_ERROR("dsi_stop_data_call: Failed with error [%d]. "
                       "Cleaning resources and releasing dsi_handle [%p]",
                       rc,
                       call_entry->dsi_handle
                       );
    /* Releasing dsi_handle*/
    dsi_rel_data_srvc_hndl(call_entry->dsi_handle);

    /* Clearing call table entry with respective to this call_id. */
    /*Find tail of call table list*/
    call_table_node = ds_dll_next(get_call_tbl_head(), &dummy);
    while (call_table_node != NULL)
    {
      call_table_tail = call_table_node;
      call_table_node = ds_dll_next(call_table_node, &dummy);
    }

    call_table_node = ds_dll_delete(get_call_tbl_head(),
                                    &call_table_tail,
                                    (const void *) call_entry,
                                    isvalid_callid_calltbl_i);
    if (call_table_node != NULL)
    {
      /*Found a call table entry wih invalid call_id.*/
      ds_dll_free (call_table_node);
    }
    end_call_failed = 1;
  }
  else
  {
    MCM_DATA_LOG_INFO("%s", "dsi_stop_data_call: Success");
    MCM_DATA_LOG_INFO("%s", "Adding txn entries for async response"
                      " of stop_data_call");
    /* Storing Token information in token table. */
    txn_entry->call_id = call_entry->call_id;
    txn_entry->api_type = MCM_DATA_API_END_DATA_CALL;
    txn_entry->self = txn_entry;
    txn_entry->qmi_cli_handle = clnt_info->clnt;
    txn_entry->req_handle = req_handle;
    //Add this transaction entry to list
    ds_dll_enq(get_txn_tbl_head(), NULL, txn_entry);
  }
  /* As we are killing the call, clear any unanswered tokens
     for start data call in the token table. */
  do
  {
    MCM_DATA_LOG_INFO("Clearing any pending start_data_call txns"
                      " on call_id [%d]",
                      txn_entry->call_id);
    txn_table_node = ds_dll_delete(get_txn_tbl_head(),
                                   &txn_table_tail,
                                   (const void *) txn_entry,
                                   txn_entry_callid_start_call_i);
    if (txn_table_node != NULL)
    {
      /*Found a transaction table entry wih invalid/unanswered token.*/
      MCM_DATA_LOG_INFO("Clearing pending start_data_call"
                        " txn entry [%p] with call_id [%d]",
                        txn_table_node,
                        txn_entry->call_id);
      ds_dll_free (txn_table_node);
    }
  } while(txn_table_node != NULL);

  if (end_call_failed == 1)
  {
    MCM_DATA_LOG_ERROR("%s", "mcm_data_stop_data_call: Failed");
    resp_msg.resp.error = MCM_ERROR_INTERNAL_V01;
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    goto BAIL;
  }
  else
  {
    MCM_DATA_LOG_INFO("%s", "mcm_data_stop_data_call: Success");
    resp_msg.resp.error = MCM_SUCCESS_V01;
    resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
    goto BAIL;
  }

BAIL:
  if(resp_msg.resp.error != MCM_SUCCESS_V01)
  {
    MCM_DATA_LOG_INFO("%s", "mcm_data_stop_data_call: Sending "
                      "failure response to client");
    qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_data_stop_data_call_rsp_msg_v01));
  }
  MCM_DATA_LOG_FUNC_EXIT();
  return QMI_CSI_CB_NO_ERR;
}

static qmi_csi_cb_error  mcm_data_get_pkt_stats(
  qmi_mcm_data_client_info_type *clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                          *req_c_struct,
  int                           req_c_struct_len,
  void                          *service_handle,
  unsigned int                    client_index
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  mcm_data_get_pkt_stats_req_msg_v01 *req_msg;
  mcm_data_get_pkt_stats_rsp_msg_v01 resp_msg;
  mcm_data_call_table_entry *call_entry;
  int rc = DSI_ERROR;

  MCM_DATA_LOG_INFO ("Client %p requesting pkt stats", clnt_info);

  memset(&resp_msg, 0, sizeof(mcm_data_get_pkt_stats_rsp_msg_v01));
  req_msg = (mcm_data_get_pkt_stats_req_msg_v01 *)req_c_struct;
  if (!qmi_mcm_data_msgr_state.mcm_data_inited)
  {
    MCM_DATA_LOG_ERROR("%s", "mcm_data not inited");
    resp_msg.resp.error = MCM_ERROR_DEVICE_NOT_READY_V01;
    goto BAIL;
  }

  call_entry = (mcm_data_call_table_entry *) get_calltbl_entry_i(
                                                  req_msg->call_id);
  if (call_entry == NULL)
  {
    resp_msg.resp.error = MCM_ERROR_NO_ENTRY_V01;
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    goto BAIL;
  }
  rc = dsi_get_pkt_stats(call_entry->dsi_handle,
                           (dsi_data_pkt_stats *)&resp_msg.pkt_stats);

BAIL:
  if ( rc == DSI_SUCCESS && resp_msg.resp.error == MCM_DATA_SUCCESS )
  {
    resp_msg.pkt_stats_valid = TRUE;
    resp_msg.resp.error = MCM_SUCCESS_V01;
    resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
  }
  else
  {
    resp_msg.pkt_stats_valid = FALSE;
    resp_msg.resp.error = MCM_ERROR_UNKNOWN_V01;
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_data_get_pkt_stats_rsp_msg_v01));
  MCM_DATA_LOG_FUNC_EXIT();
  return QMI_CSI_CB_NO_ERR;
}

static qmi_csi_cb_error  mcm_data_reset_pkt_stats(
  qmi_mcm_data_client_info_type *clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                          *req_c_struct,
  int                           req_c_struct_len,
  void                          *service_handle,
  unsigned int                  client_index
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  mcm_data_reset_pkt_stats_req_msg_v01 *req_msg;
  mcm_data_reset_pkt_stats_rsp_msg_v01 resp_msg;
  mcm_data_call_table_entry *call_entry;
  int rc;

  MCM_DATA_LOG_INFO ("Client %p resetting pkt stats", clnt_info);

  memset(&resp_msg, 0, sizeof(mcm_data_reset_pkt_stats_rsp_msg_v01));
  req_msg = (mcm_data_reset_pkt_stats_req_msg_v01 *)req_c_struct;

  if (!qmi_mcm_data_msgr_state.mcm_data_inited)
  {
    MCM_DATA_LOG_ERROR("%s", "mcm_data not inited");
    resp_msg.resp.error = MCM_ERROR_DEVICE_NOT_READY_V01;
    goto BAIL;
  }

  call_entry = (mcm_data_call_table_entry *) get_calltbl_entry_i(
                                                  req_msg->call_id);
  if (call_entry == NULL)
  {
    resp_msg.resp.error = MCM_ERROR_NO_ENTRY_V01;
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    goto BAIL;
  }

  rc =  dsi_reset_pkt_stats(call_entry->dsi_handle);

  if (rc == DSI_SUCCESS)
  {
    resp_msg.resp.error = MCM_SUCCESS_V01;
    resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
  }
  else
  {
    resp_msg.resp.error = MCM_ERROR_UNKNOWN_V01;
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
  }
BAIL:
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_data_reset_pkt_stats_rsp_msg_v01));
  MCM_DATA_LOG_FUNC_EXIT();
  return QMI_CSI_CB_NO_ERR;
}

static qmi_csi_cb_error  mcm_data_get_device_name(
  qmi_mcm_data_client_info_type *clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                          *req_c_struct,
  int                           req_c_struct_len,
  void                          *service_handle,
  unsigned int                  client_index
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  mcm_data_get_device_name_req_msg_v01 *req_msg;
  mcm_data_get_device_name_rsp_msg_v01 resp_msg;
  mcm_data_call_table_entry *call_entry;
  int rc = DSI_ERROR;

  MCM_DATA_LOG_INFO ("Client %p requesting device name", clnt_info);

  memset(&resp_msg, 0x0, sizeof(mcm_data_get_device_name_rsp_msg_v01));
  req_msg = (mcm_data_get_device_name_req_msg_v01 *)req_c_struct;

  if (!qmi_mcm_data_msgr_state.mcm_data_inited)
  {
    MCM_DATA_LOG_ERROR("%s", "mcm_data not inited");
    resp_msg.resp.error = MCM_ERROR_DEVICE_NOT_READY_V01;
    goto BAIL;
  }

  call_entry = (mcm_data_call_table_entry *) get_calltbl_entry_i(
                                                  req_msg->call_id);
  if (call_entry == NULL)
  {
    resp_msg.resp.error = MCM_ERROR_NO_ENTRY_V01;
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    goto BAIL;
  }

  rc = dsi_get_device_name(call_entry->dsi_handle,
                             (char *)&resp_msg.device_name,
                             MCM_DATA_MAX_DEVICE_NAME_LEN_V01);

  BAIL:

  if ( rc == DSI_SUCCESS )
  {
    resp_msg.device_name_valid = TRUE;
    resp_msg.device_name_len = MIN(strlen(resp_msg.device_name),
                                MCM_DATA_MAX_DEVICE_NAME_LEN_V01);
    resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
    resp_msg.resp.error = MCM_SUCCESS_V01;
  }
  else
  {
    resp_msg.device_name_valid = FALSE;
    resp_msg.resp.error = MCM_ERROR_UNKNOWN_V01;
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
  }

  MCM_DATA_LOG_INFO ("Server got device name %s with error code %d, "
                     "result %d, resp_msg.device_name_valid %d",
                     resp_msg.device_name,
                     resp_msg.resp.error,
                     resp_msg.resp.result,
                     resp_msg.device_name_valid);

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_data_get_device_name_rsp_msg_v01));
  MCM_DATA_LOG_FUNC_EXIT();
  return QMI_CSI_CB_NO_ERR;
}

static qmi_csi_cb_error mcm_data_addr_count(
  qmi_mcm_data_client_info_type *clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                          *req_c_struct,
  int                           req_c_struct_len,
  void                          *service_handle,
  unsigned int                  client_index
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  mcm_data_get_device_addr_count_req_msg_v01 *req_msg;
  mcm_data_get_device_addr_count_rsp_msg_v01 resp_msg;
  mcm_data_call_table_entry *call_entry;

  MCM_DATA_LOG_INFO ("Client %p requesting Address Count", clnt_info);
  memset(&resp_msg, 0, sizeof(mcm_data_get_device_addr_count_rsp_msg_v01));
  req_msg = (mcm_data_get_device_addr_count_req_msg_v01 *)req_c_struct;

  if (!qmi_mcm_data_msgr_state.mcm_data_inited)
  {
    MCM_DATA_LOG_ERROR("%s", "mcm_data not inited");
    resp_msg.resp.error = MCM_ERROR_DEVICE_NOT_READY_V01;
    goto BAIL;
  }

  call_entry = (mcm_data_call_table_entry *) get_calltbl_entry_i(
                                                  req_msg->call_id);
  if (call_entry == NULL)
  {
    resp_msg.resp.error = MCM_ERROR_NO_ENTRY_V01;
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    goto BAIL;
  }

  resp_msg.addr_count = (uint32)dsi_get_ip_addr_count(call_entry->dsi_handle);
  resp_msg.resp.error = MCM_SUCCESS_V01;
  resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;

BAIL :

  if (resp_msg.resp.error == MCM_SUCCESS_V01)
    resp_msg.addr_count_valid = TRUE;
  else
    resp_msg.addr_count_valid = FALSE;

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_data_get_device_addr_count_rsp_msg_v01));
  MCM_DATA_LOG_FUNC_EXIT();
  return QMI_CSI_CB_NO_ERR;
}

static qmi_csi_cb_error  mcm_data_get_call_tech(
  qmi_mcm_data_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                    *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle,
  unsigned int                    client_index
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  mcm_data_get_call_tech_req_msg_v01 *req_msg;
  mcm_data_get_call_tech_rsp_msg_v01 resp_msg;
  mcm_data_call_table_entry *call_entry;

  MCM_DATA_LOG_INFO ("Client %p requesting Call Tech", clnt_info);
  memset(&resp_msg, 0, sizeof(mcm_data_get_call_tech_rsp_msg_v01));
  req_msg = (mcm_data_get_call_tech_req_msg_v01 *)req_c_struct;

  if (!qmi_mcm_data_msgr_state.mcm_data_inited)
  {
    MCM_DATA_LOG_ERROR("%s", "mcm_data not inited");
    resp_msg.resp.error = MCM_ERROR_DEVICE_NOT_READY_V01;
    goto BAIL;
  }

  resp_msg.call_tech = 0;

  call_entry = get_calltbl_entry_i(req_msg->call_id);
  if (call_entry == NULL)
  {
    resp_msg.resp.error = MCM_ERROR_NO_ENTRY_V01;
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    goto BAIL;
  }

  resp_msg.call_tech =
           (uint32)dsi_get_current_data_bearer_tech(call_entry->dsi_handle);
  resp_msg.resp.error = MCM_SUCCESS_V01;
  resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;

BAIL:

  if (resp_msg.resp.error == MCM_SUCCESS_V01)
    resp_msg.call_tech_valid = TRUE;
  else
    resp_msg.call_tech_valid = FALSE;

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_data_get_call_tech_rsp_msg_v01));
  MCM_DATA_LOG_FUNC_EXIT();
  return QMI_CSI_CB_NO_ERR;
}

static qmi_csi_cb_error  mcm_data_get_call_status(
  qmi_mcm_data_client_info_type *clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                          *req_c_struct,
  int                           req_c_struct_len,
  void                          *service_handle,
  unsigned int                  client_index
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  mcm_data_get_call_status_req_msg_v01 *req_msg;
  mcm_data_get_call_status_rsp_msg_v01 resp_msg;
  mcm_data_call_table_entry *call_entry;

  MCM_DATA_LOG_INFO ("Client %p requesting Call Status", clnt_info);

  memset(&resp_msg, 0, sizeof(mcm_data_get_call_status_rsp_msg_v01));
  req_msg = (mcm_data_get_call_status_req_msg_v01 *)req_c_struct;

  if (!qmi_mcm_data_msgr_state.mcm_data_inited)
  {
    MCM_DATA_LOG_ERROR("%s", "mcm_data not inited");
    resp_msg.resp.error = MCM_ERROR_DEVICE_NOT_READY_V01;
    goto BAIL;
  }

  resp_msg.call_status = MCM_DATA_CALL_STATE_INVALID_V01;
  call_entry = (mcm_data_call_table_entry *) get_calltbl_entry_i(
                                                  req_msg->call_id);
  if (call_entry == NULL)
  {
    resp_msg.resp.error = MCM_ERROR_NO_ENTRY_V01;
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    goto BAIL;
  }
  resp_msg.call_status = MCM_DATA_CALL_STATE_CONNECTED_V01;
  resp_msg.resp.error = MCM_SUCCESS_V01;
  resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;

BAIL:

  if (resp_msg.resp.error == MCM_SUCCESS_V01)
    resp_msg.call_status_valid = TRUE;
  else
    resp_msg.call_status_valid = FALSE;

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                      sizeof(mcm_data_get_call_status_rsp_msg_v01));

  MCM_DATA_LOG_FUNC_EXIT();
  return QMI_CSI_CB_NO_ERR;
}

static qmi_csi_cb_error  mcm_data_get_device_addr
(
  qmi_mcm_data_client_info_type *clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                          *req_c_struct,
  int                           req_c_struct_len,
  void                          *service_handle,
  unsigned int                  client_index
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  mcm_data_get_device_addr_req_msg_v01 *req_msg;
  mcm_data_get_device_addr_rsp_msg_v01 resp_msg;
  uint32 addr_count = 0;
  mcm_data_call_table_entry *call_entry;
  int rc = DSI_ERROR;

  MCM_DATA_LOG_INFO ("Client %p requesting Device Address", clnt_info);

  memset(&resp_msg, 0, sizeof(mcm_data_get_device_addr_rsp_msg_v01));
  req_msg = (mcm_data_get_device_addr_req_msg_v01 *)req_c_struct;

  if (!qmi_mcm_data_msgr_state.mcm_data_inited)
  {
    MCM_DATA_LOG_ERROR("%s", "mcm_data not inited");
    resp_msg.resp.error = MCM_ERROR_DEVICE_NOT_READY_V01;
    goto BAIL;
  }

  call_entry = (mcm_data_call_table_entry *) get_calltbl_entry_i(
                                                  req_msg->call_id);
  if (call_entry == NULL)
  {
    resp_msg.resp.error = MCM_ERROR_NO_ENTRY_V01;
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    goto BAIL;
  }

  addr_count = (uint32)dsi_get_ip_addr_count(call_entry->dsi_handle);

  rc = dsi_get_ip_addr(call_entry->dsi_handle,
                           (dsi_addr_info_t *)&resp_msg.addr_info,
                           addr_count);

  if ( rc == DSI_SUCCESS )
  {
    resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
    resp_msg.resp.error = MCM_SUCCESS_V01;
    resp_msg.addr_info_valid = TRUE;
  }
  else
  {
    resp_msg.resp.error = MCM_ERROR_INTERNAL_V01;
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    resp_msg.addr_info_valid = FALSE;
  }

BAIL:
  resp_msg.addr_info_len = addr_count;
  if (resp_msg.resp.error == MCM_DATA_SUCCESS)
    resp_msg.addr_info_valid = TRUE;
  else
    resp_msg.addr_info_valid = FALSE;

  MCM_DATA_LOG_INFO ("Address Count [%d], addr_info_valid %d",
                     addr_count,
                     resp_msg.addr_info_valid);
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                  sizeof(mcm_data_get_device_addr_rsp_msg_v01));
  MCM_DATA_LOG_FUNC_EXIT();
  return QMI_CSI_CB_NO_ERR;
}

static qmi_csi_cb_error  mcm_data_get_data_channel_rate
(
  qmi_mcm_data_client_info_type *clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                          *req_c_struct,
  int                           req_c_struct_len,
  void                          *service_handle,
  unsigned int                  client_index
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  mcm_data_get_channel_rate_req_msg_v01 *req_msg;
  mcm_data_get_channel_rate_rsp_msg_v01 resp_msg;
  mcm_data_call_table_entry *call_entry;
  int rc = DSI_ERROR;

  MCM_DATA_LOG_INFO ("Client %p requesting Channel Rate", clnt_info);
  memset(&resp_msg, 0, sizeof(mcm_data_get_channel_rate_rsp_msg_v01));
  req_msg = (mcm_data_get_channel_rate_req_msg_v01 *)req_c_struct;
  memset(&resp_msg.channel_rate,
         0x00,
         sizeof(mcm_data_channel_rate_t_v01));

  if (!qmi_mcm_data_msgr_state.mcm_data_inited)
  {
    MCM_DATA_LOG_ERROR("%s", "mcm_data not inited");
    resp_msg.resp.error = MCM_ERROR_DEVICE_NOT_READY_V01;
    goto BAIL;
  }

  call_entry = (mcm_data_call_table_entry *) get_calltbl_entry_i(
                                                      req_msg->call_id);
  if (call_entry == NULL)
  {
    resp_msg.resp.error = MCM_ERROR_NO_ENTRY_V01;
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    goto BAIL;
  }

  rc= dsi_get_current_data_channel_rate(
                           call_entry->dsi_handle,
                           (dsi_data_channel_rate_t *) &resp_msg.channel_rate);

BAIL:

  if ( rc == DSI_SUCCESS )
  {
    resp_msg.channel_rate_valid= TRUE;
    resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
    resp_msg.resp.error = MCM_SUCCESS_V01;
    MCM_DATA_LOG_INFO ("%s", "Received Channel Rate from dsi_netctrl");
    MCM_DATA_LOG_INFO ("current_tx=%lu, current_rx=%lu", resp_msg.channel_rate.current_tx_rate, resp_msg.channel_rate.current_rx_rate);
    MCM_DATA_LOG_INFO ("max_tx=%lu, max_rx=%lu", resp_msg.channel_rate.max_tx_rate, resp_msg.channel_rate.max_rx_rate);
  }
  else
  {
    resp_msg.channel_rate_valid = FALSE;
    resp_msg.resp.error = MCM_ERROR_UNKNOWN_V01;
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    MCM_DATA_LOG_INFO ("dsi_get_current_data_channel_rate:"
                       " Failed with error [%d]",
                       resp_msg.resp.error);
  }

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_data_get_channel_rate_rsp_msg_v01));
  MCM_DATA_LOG_FUNC_EXIT();
  return QMI_CSI_CB_NO_ERR;
}

static qmi_csi_cb_error  mcm_data_event_register
(
  qmi_mcm_data_client_info_type *clnt_info,
  qmi_req_handle                req_handle,
  int                           msg_id,
  void                          *req_c_struct,
  int                           req_c_struct_len,
  void                          *service_handle,
  unsigned int                  client_index
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  mcm_data_event_register_req_msg_v01 *req_msg;
  mcm_data_event_register_resp_msg_v01 resp_msg;
  qmi_mcm_data_state_info_type     *mcm_data_ptr;
  qmi_mcm_data_client_info_type  *client_info;

  MCM_DATA_LOG_INFO ("Client %p Registering for unsol Events", clnt_info);
  memset(&resp_msg, 0, sizeof(mcm_data_event_register_resp_msg_v01));

  if (!qmi_mcm_data_msgr_state.mcm_data_inited)
  {
    MCM_DATA_LOG_ERROR("%s", "mcm_data not inited");
    resp_msg.response.result = MCM_RESULT_FAILURE_V01;
    resp_msg.response.error= MCM_ERROR_DEVICE_NOT_READY_V01;
    goto BAIL;
  }

  if((service_handle != NULL) &&
     (client_index != MCM_DATA_MAX_CLIENT_HANDLES))
  {
    mcm_data_ptr = (qmi_mcm_data_state_info_type*) service_handle;
  }
  else
  {
    resp_msg.response.result = MCM_RESULT_FAILURE_V01;
    resp_msg.response.error = MCM_ERROR_INVALID_CLIENT_ID_V01;
    goto BAIL;
  }

  req_msg = (mcm_data_event_register_req_msg_v01 *)req_c_struct;

  client_info = (qmi_mcm_data_client_info_type *)
                   mcm_data_ptr->client_handle_list[client_index];

  client_info->event_reg_data = *req_msg;

BAIL:
  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_data_event_register_resp_msg_v01));
  MCM_DATA_LOG_FUNC_EXIT();
  return QMI_CSI_CB_NO_ERR;
}

static qmi_csi_cb_error  mcm_data_get_reg_status(
  qmi_mcm_data_client_info_type        *clnt_info,
  qmi_req_handle           req_handle,
  int                      msg_id,
  void                     *req_c_struct,
  int                      req_c_struct_len,
  void                     *service_handle,
  unsigned int             client_index
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  mcm_data_get_reg_status_req_msg_v01 *req_msg;
  mcm_data_get_reg_status_rsp_msg_v01 resp_msg;
  dsd_get_system_status_resp_msg_v01  sys_resp_msg;
  qmi_client_error_type rc;

  MCM_DATA_LOG_INFO ("Client %p requesting Reg Status", clnt_info);
  memset(&resp_msg, 0, sizeof(mcm_data_get_reg_status_rsp_msg_v01));
  memset(&sys_resp_msg, 0, sizeof(dsd_get_system_status_resp_msg_v01));

  if (!qmi_mcm_data_msgr_state.mcm_data_inited)
  {
    MCM_DATA_LOG_ERROR("%s", "mcm_data not inited");
    resp_msg.resp.error = MCM_ERROR_DEVICE_NOT_READY_V01;
    goto BAIL;
  }

  req_msg = (mcm_data_get_reg_status_req_msg_v01 *)req_c_struct;

  if(qmi_mcm_data_msgr_state.qmi_dsd_hndl)
  {
    rc = qmi_client_send_msg_sync(qmi_mcm_data_msgr_state.qmi_dsd_hndl,
                                  QMI_DSD_GET_SYSTEM_STATUS_REQ_V01,
                                  NULL,
                                  0,
                                  &sys_resp_msg,
                                  sizeof(sys_resp_msg),
                                  0);
    if ( sys_resp_msg.avail_sys_valid )
    {
      resp_msg.reg_status_valid = TRUE;
      /* Null bearer system RAT implies modem is out of service */
      if ( DSD_SYS_RAT_EX_NULL_BEARER_V01 == sys_resp_msg.avail_sys[0].rat_value )
      {
        resp_msg.reg_status.srv_status = MCM_DATA_MODEM_STATE_OOS_V01;
      }
      else
      {
        resp_msg.reg_status.srv_status = MCM_DATA_MODEM_STATE_IN_SERVICE_V01;
      }
      resp_msg.reg_status.tech_info =
         (mcm_data_bearer_tech_info_t_v01)
         dsi_translate_qmi_to_dsi_bearer_tech_ex(&sys_resp_msg.avail_sys[0]);
      resp_msg.resp.result = MCM_RESULT_SUCCESS_V01;
      resp_msg.resp.error = MCM_SUCCESS_V01;
    }
    /* We must receive a valid available system even when the modem is
    out of service - we get an available system with NULL bearer as system RAT
    Hence there shouldn't be a case of no valid available systems */
    else
    {
      MCM_DATA_LOG_ERROR("%s", "No systems available");
      resp_msg.reg_status_valid = FALSE;
      resp_msg.resp.error = MCM_ERROR_GENERIC_V01;
      resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
    }
  }
  else
  {
    MCM_DATA_LOG_ERROR ("%s", "Invalid DSD Handle");
    resp_msg.resp.error = MCM_ERROR_INVALID_HANDLE_V01;
    resp_msg.resp.result = MCM_RESULT_FAILURE_V01;
  }

BAIL:

  if (resp_msg.resp.error == MCM_SUCCESS_V01)
    resp_msg.reg_status_valid = TRUE;
  else
    resp_msg.reg_status_valid = FALSE;

  qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                    sizeof(mcm_data_get_reg_status_rsp_msg_v01));
  MCM_DATA_LOG_FUNC_EXIT();
  return QMI_CSI_CB_NO_ERR;
}


static qmi_csi_error mcm_data_send_indication
(
  mcm_data_unsol_event_ind_msg_v01 * unsol_event_data
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  unsigned int i = 0;
  boolean send_ind = FALSE;
  qmi_mcm_data_client_info_type *client_info;
  qmi_csi_error   rc = QMI_CSI_INVALID_ARGS;

  for( i = 0; i < MCM_DATA_MAX_CLIENT_HANDLES ; i++)
  {
    client_info = (qmi_mcm_data_client_info_type *)
                        qmi_mcm_data_msgr_state.client_handle_list[i];
    if(client_info)
    {
      switch(unsol_event_data->event_id)
      {
        case MCM_DATA_NET_UP_EV_V01:
          if((client_info->client_enabled) &&
             (client_info->event_reg_data.register_net_up_event_valid == TRUE) &&
             (client_info->event_reg_data.register_net_up_event == TRUE))
          {
            send_ind = TRUE;
          }
          break;
        case MCM_DATA_NET_DOWN_EV_V01:
          if((client_info->client_enabled) &&
             (client_info->event_reg_data.register_net_down_event_valid == TRUE) &&
             (client_info->event_reg_data.register_net_down_event == TRUE))
          {
            send_ind = TRUE;
          }
          break;
        case MCM_DATA_NET_NEW_ADDR_EV_V01:
          if((client_info->client_enabled) &&
             (client_info->event_reg_data.register_net_new_addr_event_valid == TRUE) &&
             (client_info->event_reg_data.register_net_new_addr_event == TRUE))
          {
            send_ind = TRUE;
          }
          break;
        case MCM_DATA_NET_DEL_ADDR_EV_V01:
          if((client_info->client_enabled) &&
             (client_info->event_reg_data.register_net_del_addr_event_valid == TRUE) &&
             (client_info->event_reg_data.register_net_del_addr_event == TRUE))
          {
            send_ind = TRUE;
          }
          break;
        case MCM_DATA_REG_SRVC_STATUS_EV_V01:
          if((client_info->client_enabled) &&
             (client_info->event_reg_data.register_reg_srvc_status_event_valid == TRUE) &&
             (client_info->event_reg_data.register_reg_srvc_status_event == TRUE))
          {
            send_ind = TRUE;
          }
          break;
        case MCM_DATA_BEARER_TECH_STATUS_EV_V01:
          if((client_info->client_enabled) &&
             (client_info->event_reg_data.register_bearer_tech_status_event_valid == TRUE) &&
             (client_info->event_reg_data.register_bearer_tech_status_event == TRUE))
          {
            send_ind = TRUE;
          }
          break;
        case MCM_DATA_DORMANCY_STATUS_EV_V01:
          if((client_info->client_enabled) &&
             (client_info->event_reg_data.register_dormancy_status_event_valid == TRUE) &&
             (client_info->event_reg_data.register_dormancy_status_event == TRUE))
          {
            send_ind = TRUE;
          }
          break;
        default:
          MCM_DATA_LOG_ERROR("Received unknown event [%d]."
                             " Aborting send_indication",
                             unsol_event_data->event_id);
          send_ind = FALSE;
      }

      if(send_ind == TRUE)
      {
        rc = qmi_csi_send_ind(client_info->clnt,
                      MCM_DATA_UNSOL_EVENT_IND_V01,
                      unsol_event_data,
                      sizeof(mcm_data_unsol_event_ind_msg_v01));
        if ( rc == QMI_CSI_NO_ERR )
        {
          MCM_DATA_LOG_INFO("Successfully sent event [%d] indication to"
                            " client with qmi_client_handle [%d]",
                            unsol_event_data->event_id,
                            client_info->clnt);
        }
        else
        {
          MCM_DATA_LOG_ERROR("Failed to send event [%d] indication to"
                            " client with qmi_client_handle [%d]",
                            unsol_event_data->event_id,
                            client_info->clnt);
        }
      }
    }
  }

  BAIL:
  MCM_DATA_LOG_FUNC_EXIT();
  return QMI_CSI_CB_NO_ERR;
}

void mcm_data_qmi_wds_ind_cb
(
  qmi_client_type                user_handle,
  unsigned long                  msg_id,
  unsigned char                  *ind_buf,
  int                            ind_buf_len,
  void                           *ind_cb_data
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  mcm_data_event_data_t mcm_data_buffer;
  int numBytes=0, len;
  struct sockaddr_un wds_mcm_data_;
  int rc;
  void *ind_data_ptr = NULL;
  int ind_data_len = 0;

  wds_mcm_data_.sun_family = AF_UNIX;
  std_strlcpy(wds_mcm_data_.sun_path,
              MCM_DATA_DSI_UDS_FILE,
              MCM_DATA_DSI_UDS_FILE_LEN);
  len = strlen(wds_mcm_data_.sun_path)+sizeof(wds_mcm_data_.sun_family);

  if(msg_id == QMI_WDS_EVENT_REPORT_IND_V01)
  {
    ind_data_ptr = &mcm_data_buffer.evt_data.wds_event.ind_data;
    ind_data_len = sizeof(mcm_data_buffer.evt_data.wds_event.ind_data);
    mcm_data_buffer.evt_data.wds_event.user_data = ind_cb_data;
    mcm_data_buffer.evt_data.wds_event.ind_id = msg_id;

    mcm_data_buffer.event_id = MCM_DATA_WDS_EVENT;

    rc = qmi_client_message_decode( user_handle,
                                    QMI_IDL_INDICATION,
                                    msg_id,
                                    ind_buf,
                                    ind_buf_len,
                                    ind_data_ptr,
                                    (int)ind_data_len);
    if(rc != QMI_NO_ERR)
    {
      MCM_DATA_LOG_ERROR("%s: failed decode for ind [0x%x], err [%d]",
                       __func__, msg_id, rc);
      goto BAIL;
    }

    if ((numBytes = sendto(mcm_data_cli_sockfd,
                           (void *)&mcm_data_buffer,
                           sizeof(mcm_data_event_data_t),
                           0,
                           (struct sockaddr *)&wds_mcm_data_,
                           len)) == -1)
    {
      MCM_DATA_LOG_ERROR("%s", "Send Failed from dsi callback context");
      goto BAIL;
    }

    MCM_DATA_LOG_INFO("%s", "Send succeeded in dsi callback context");
  }
BAIL:
  MCM_DATA_LOG_FUNC_EXIT();
  return;
}
void mcm_data_qmi_wds_ind_handler
(
  void * user_data,
  unsigned int   ind_id,
  wds_event_report_ind_msg_v01 *ind_data
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  mcm_data_call_table_entry * call_tbl_ptr = NULL;
  mcm_data_unsol_event_ind_msg_v01 unsol_data_ind_msg;
  qmi_csi_error rc;
  mcm_data_dormancy_state_t_v01 dorm_status =
           MCM_DATA_DORMANCY_STATE_T_MAX_ENUM_VAL_V01;
  dsi_data_bearer_tech_t data_tech;

  if (NULL == ind_data)
  {
    MCM_DATA_LOG_ERROR("%s", "NULL ind_data recieved");
    goto BAIL;
  }

  /* if a non NULL user_data is received, validate it */
  if (NULL != user_data)
  {
    call_tbl_ptr = (mcm_data_call_table_entry *)user_data;

    if(is_call_valid_i(call_tbl_ptr) == FALSE)
    {
      goto BAIL;
    }
  }
  else
  {
    MCM_DATA_LOG_ERROR("%s", "Received NULL user_data");
    goto BAIL;
  }

  switch (ind_id)
  {
    case QMI_WDS_EVENT_REPORT_IND_V01:
      if (ind_data->dormancy_status_valid)
      {
        switch(ind_data->dormancy_status)
        {
          case WDS_DORMANCY_STATUS_DORMANT_V01:
            dorm_status = MCM_DATA_DORMANCY_STATE_PHYSLINK_ACTIVE_V01;
          break;
          case WDS_DORMANCY_STATUS_ACTIVE_V01:
            dorm_status = MCM_DATA_DORMANCY_STATE_PHYSLINK_DORMANT_V01;
          break;
          default:
            MCM_DATA_LOG_ERROR("Invalid dorm status [%d] received",
                               ind_data->dormancy_status);
          break;
        }
      }

      if(dorm_status != MCM_DATA_DORMANCY_STATE_T_MAX_ENUM_VAL_V01)
      {
        memset(&unsol_data_ind_msg, 0x00,
               sizeof(mcm_data_unsol_event_ind_msg_v01));
        unsol_data_ind_msg.call_id_valid = TRUE;
        unsol_data_ind_msg.call_id = call_tbl_ptr->call_id;
        unsol_data_ind_msg.call_status_valid = TRUE;
        unsol_data_ind_msg.call_status = MCM_DATA_CALL_STATE_CONNECTED_V01;
        unsol_data_ind_msg.event_id = MCM_DATA_DORMANCY_STATUS_EV_V01;
        unsol_data_ind_msg.dorm_status_valid = TRUE;
        unsol_data_ind_msg.dorm_status = dorm_status;

        rc = mcm_data_send_indication(&unsol_data_ind_msg);

      }

      if (ind_data->bearer_tech_ex_valid)
      {
        data_tech = dsi_translate_qmi_to_dsi_bearer_tech_ex
                         (&ind_data->bearer_tech_ex);
        memset(&unsol_data_ind_msg, 0x00,
               sizeof(mcm_data_unsol_event_ind_msg_v01));
        unsol_data_ind_msg.call_id_valid = TRUE;
        unsol_data_ind_msg.call_id = call_tbl_ptr->call_id;
        unsol_data_ind_msg.call_status_valid = TRUE;
        unsol_data_ind_msg.call_status = MCM_DATA_CALL_STATE_CONNECTED_V01;
        unsol_data_ind_msg.event_id = MCM_DATA_BEARER_TECH_STATUS_EV_V01;
        unsol_data_ind_msg.call_tech_valid = TRUE;
        unsol_data_ind_msg.call_tech =
                 (mcm_data_bearer_tech_info_t_v01) data_tech;

        rc = mcm_data_send_indication(&unsol_data_ind_msg);
      }
      break;
    default:
      MCM_DATA_LOG_INFO("Ignoring wds_ind event [%d]", ind_id);
      break;
  }

  BAIL:
  MCM_DATA_LOG_FUNC_EXIT();
  return;
}

static void mcm_data_sig_hdlr
(
  int signal
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  MCM_DATA_LOG_INFO("Received signal %d", signal);

  switch (signal)
  {
    case SIGTERM:
      MCM_DATA_LOG_ERROR("%s", "SIGTERM");
      break;
    case SIGINT:
      MCM_DATA_LOG_ERROR("%s", "SIGINT");
      break;
    case SIGSEGV :
      MCM_DATA_LOG_ERROR("%s", "SIGSEGV");
      break;
    default:
      MCM_DATA_LOG_ERROR("Received unexpected signal %d", signal);
      break;
  }
  mcm_set_service_ready(MCM_DATA_SERVICE, 0);
  mcm_data_cleanup();
  MCM_DATA_LOG_FUNC_EXIT();
  exit(0);
}

void mcm_data_qmi_dsd_ind_cb
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                           *ind_buf,
 unsigned int                   ind_buf_len,
 void                           *ind_cb_data
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  dsd_system_status_ind_msg_v01  *dsd_ind_msg;
  int  rc = QMI_INTERNAL_ERR;
  mcm_data_event_data_t mcm_data_buffer;
  int numBytes=0, len;
  struct sockaddr_un dsd_mcm_data_;

  if (!user_handle || !ind_buf)
  {
    MCM_DATA_LOG_ERROR("%s", "mcm_data_qmi_dsd_ind_cb: bad param(s)");
    goto BAIL;
  }

  MCM_DATA_LOG_INFO("mcm_data_qmi_dsd_ind_cb: recvd ind=%lu", msg_id);

  dsd_ind_msg = calloc(1, sizeof(dsd_system_status_ind_msg_v01));

  if (!dsd_ind_msg)
  {
    MCM_DATA_LOG_ERROR("%s", "mcm_data_qmi_dsd_ind_cb: Failed to alloc memory");
    goto BAIL;
  }

  switch (msg_id)
  {
    case QMI_DSD_SYSTEM_STATUS_IND_V01:
      /* Decode the QMI indication message to its corresponding C structure */
      rc = qmi_client_message_decode(user_handle,
                                     QMI_IDL_INDICATION,
                                     msg_id,
                                     ind_buf,
                                     ind_buf_len,
                                     dsd_ind_msg,
                                     sizeof(*dsd_ind_msg));
      if(dsd_ind_msg->avail_sys_valid == TRUE)
      {
        MCM_DATA_LOG_INFO("Pref data network=0x%x, rat=0x%08x,"
        " so_mask=0x%016llx",
                          dsd_ind_msg->avail_sys[0].technology,
                          dsd_ind_msg->avail_sys[0].rat_value,
                          dsd_ind_msg->avail_sys[0].so_mask);

        dsd_mcm_data_.sun_family = AF_UNIX;
        std_strlcpy(dsd_mcm_data_.sun_path,
                    MCM_DATA_DSI_UDS_FILE,
                    MCM_DATA_DSI_UDS_FILE_LEN);
        len = strlen(dsd_mcm_data_.sun_path)+sizeof(dsd_mcm_data_.sun_family);

        mcm_data_buffer.event_id = MCM_DATA_DSD_EVENT;

        mcm_data_buffer.evt_data.dsd_event.status_info =
                                        dsd_ind_msg->avail_sys[0];

        if ((numBytes = sendto(mcm_data_cli_sockfd,
                               (void *)&mcm_data_buffer,
                               sizeof(mcm_data_event_data_t),
                               0,
                               (struct sockaddr *)&dsd_mcm_data_,
                               len)) == -1)
        {
          MCM_DATA_LOG_ERROR("%s", "Send Failed from dsd callback context");
          goto BAIL;
        }
      }
      free(dsd_ind_msg);
      break;
    default:
      MCM_DATA_LOG_ERROR("Received unknown msg_id [%d]", msg_id);
  }

BAIL:
  MCM_DATA_LOG_FUNC_EXIT();
}

void mcm_data_qmi_dsd_ind_handler
(
  dsd_system_status_info_type_v01  *dsd_ind_msg
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  mcm_data_unsol_event_ind_msg_v01 unsol_data_ind_msg;
  qmi_csi_error rc;

  memset(&unsol_data_ind_msg, 0x00,
               sizeof(mcm_data_unsol_event_ind_msg_v01));

  unsol_data_ind_msg.event_id = MCM_DATA_REG_SRVC_STATUS_EV_V01;
  unsol_data_ind_msg.reg_status_valid = TRUE;
  unsol_data_ind_msg.reg_status.tech_info =
            (mcm_data_bearer_tech_info_t_v01)
                   dsi_translate_qmi_to_dsi_bearer_tech_ex(dsd_ind_msg);
  if ( DSD_SYS_RAT_EX_NULL_BEARER_V01 == dsd_ind_msg->rat_value )
  {
    unsol_data_ind_msg.reg_status.srv_status = MCM_DATA_MODEM_STATE_OOS_V01;
  }
  else
  {
    unsol_data_ind_msg.reg_status.srv_status = MCM_DATA_MODEM_STATE_IN_SERVICE_V01;
  }
  rc = mcm_data_send_indication(&unsol_data_ind_msg);
  MCM_DATA_LOG_FUNC_EXIT();
}

static void mcm_data_cleanup
(
  void
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  ds_dll_el_t *call_table_node = NULL;
  ds_dll_el_t *txn_table_node  = NULL;
  ds_dll_el_t *call_table_tail = NULL;
  ds_dll_el_t *txn_table_tail  = NULL;
  const void  *dummy           = NULL;
  mcm_data_call_table_entry *call_entry = NULL;

  /*Find tail of call table list*/
  call_table_node = ds_dll_next(get_call_tbl_head(), &dummy);
  while (call_table_node != NULL)
  {
    call_table_tail = call_table_node;
    call_table_node = ds_dll_next(call_table_node, &dummy);
  }

  call_table_node = call_table_tail;
  while(call_table_node !=NULL)
  {
    call_table_tail = call_table_node;
    if (call_table_tail->data != NULL)
    {
      /* Found a valid entry. Releasing dsi_handle*/
      call_entry = (mcm_data_call_table_entry *)call_table_tail->data;
      MCM_DATA_LOG_INFO("Releasing dsi_handle : %p",
                        call_entry->dsi_handle, 0, 0);
      dsi_rel_data_srvc_hndl(call_entry->dsi_handle);
      free(call_table_tail->data);
      call_table_tail->data = NULL;
    }
    call_table_node = call_table_tail->prev;
    ds_dll_free(call_table_tail);
    call_table_tail = NULL;
  }

  /*Find tail of transaction table list*/
  txn_table_node = ds_dll_next(get_txn_tbl_head(), &dummy);
  while (txn_table_node != NULL)
  {
    txn_table_tail = txn_table_node;
    txn_table_node = ds_dll_next(txn_table_node, &dummy);
  }

  txn_table_node = txn_table_tail;
  while(txn_table_node !=NULL)
  {
    txn_table_tail = txn_table_node;
    if (txn_table_tail->data != NULL)
    {
      ds_dll_free(txn_table_tail->data);
      txn_table_tail->data = NULL;
    }
    txn_table_node = txn_table_tail->prev;
    free(txn_table_tail);
    txn_table_tail = NULL;
  }

  qmi_csi_unregister(qmi_mcm_data_msgr_state.service_handle);
  if(qmi_mcm_data_msgr_state.qmi_dpm_hndl != NULL)
    qmi_client_release(qmi_mcm_data_msgr_state.qmi_dpm_hndl);
  if(qmi_mcm_data_msgr_state.qmi_dsd_hndl != NULL)
    qmi_client_release(qmi_mcm_data_msgr_state.qmi_dsd_hndl);
  /* Client sockets */
  close(mcm_data_cli_sockfd);
  MCM_DATA_LOG_FUNC_EXIT();
}

static void mcm_data_release_client
(
    qmi_mcm_data_client_info_type *clnt_info
)
{
  MCM_DATA_LOG_FUNC_ENTRY();
  qmi_client_handle clnt;
  mcm_data_call_table_entry *call_entry;

  clnt = clnt_info->clnt;
  call_entry = get_calltbl_qmi_entry_i(clnt);
  if (call_entry != NULL)
  {
    MCM_DATA_LOG_INFO("Call ID %d, qmi_cli_handle %p, dsi_hdnl %p",
                      call_entry->call_id,
                      call_entry->qmi_cli_handle,
                      call_entry->dsi_handle);
  }
  else
  {
    MCM_DATA_LOG_INFO("Unable to find call entry with qmi_cli_handle %d",
                       clnt);
  }

  while (call_entry != NULL)
  {
    MCM_DATA_LOG_INFO("Cleaning terminated client with Call ID %d,"
                      " QMI CLI HDNL %p, DSI HDNL %p",
                      call_entry->call_id,
                      call_entry->qmi_cli_handle,
                      call_entry->dsi_handle);
    MCM_DATA_LOG_INFO("Releasing dsi_handle : %p",
                      call_entry->dsi_handle, 0, 0);
    dsi_rel_data_srvc_hndl(call_entry->dsi_handle);
    del_calltbl_entry_callid_i(call_entry->call_id);

    del_txntbl_entry_callid_i(call_entry->call_id, ALL);
    call_entry = get_calltbl_qmi_entry_i(clnt);
  }
  MCM_DATA_LOG_FUNC_EXIT();
}
