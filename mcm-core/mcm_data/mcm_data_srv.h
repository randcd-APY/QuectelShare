#ifndef MCM_DATA_SRV_H
#define MCM_DATA_SRV_H

/**
  @file
  mcm_data_srv.h

  @brief
  This file provides Mobile Connection Manager(mcm) Server functionality.

  @details
  This file provides various data types and function declarations of the
  Mobile Connection Manager(mcm) Data Services APIs to be defined
  in mcm_data_srv.c

*/

/*===========================================================================

  Copyright (c) 2013, 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/19/13   vb      Initial module.

===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "dsi_netctrl_platform.h"
#include "dsi_netctrl.h"
#include "mcm_data_v01.h"
#include "mcm_service_object_v01.h"
#include "qmi_csi.h"
#include "common_v01.h"
#include <linux/if_addr.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include "string.h"
#include "ds_string.h"
#include "mcm_data_utils.h"
#include "qmi_client.h"
#include "data_system_determination_v01.h"
#include "ds_list.h"
#include "wireless_data_service_v01.h"
#include <pthread.h>


#define MCM_DATA_MAX_CLIENT_HANDLES (5)
#define MCM_DATA_DSI_UDS_FILE       "/data/mcm_data_dsi_uds_file"
#define MCM_DATA_DSI_UDS_FILE_LEN   31

#define MCM_DATA_TOKENID_MAX        1000
#define MAX_BUF_LEN                 256
#define MCM_DATA_CALLID_MAX         DSI_MAX_DATA_CALLS
#define MCM_DATA_INVALID_CALLID     -1
#define MCM_DATA_INVALID_HNDL       -1
#define MCM_DATA_INVALID_CLT_HNDL  NULL

#define MCM_DATA_QMI_DSD_TIMEOUT 30000
#define MCM_DATA_QMI_WDS_TIMEOUT 30000

/*--------------------------------------------------------------------------
  Macro to bailout gracefully clearing memory
---------------------------------------------------------------------------*/

#define MCM_DATA_RETURN_ERROR() \
          if (call_entry != NULL) free(call_entry); \
          if (txn_entry != NULL) free(txn_entry); \
          goto BAIL

/*--------------------------------------------------------------------------
  mcm_data_modem_ssr_signal_t
  will send it to get pthread info, also included <pthread.h>
---------------------------------------------------------------------------*/
typedef struct
{
  pthread_t          thread_id;
} mcm_data_modem_ssr_thread_info;

/*--------------------------------------------------------------------------
  Types of events received by mcm_data_srv
---------------------------------------------------------------------------*/

typedef enum mcm_data_event_type_e
{
  MCM_DATA_DSI_EVENT,
  MCM_DATA_WDS_EVENT,
  MCM_DATA_DSD_EVENT,
  MCM_DATA_MODEM_SSR_EVENT
} mcm_data_event_type_t;

/*---------------------------------------------------------------------------
  Types used to indicate incoming notification state and local state
  MCM_DATA_MODEM_STATUS_OOS : out of service
  MCM_DATA_MODEM_STATUS_IS  : in service
---------------------------------------------------------------------------*/

typedef enum mcm_data_modem_ssr_status_type_e
{
  MCM_DATA_MODEM_STATUS_OOS = 0,
  MCM_DATA_MODEM_STATUS_IS
}mcm_data_modem_ssr_status_type_t;

/*---------------------------------------------------------------------------
  API types used in MCM Data
---------------------------------------------------------------------------*/

typedef enum mcm_data_api_types_e
{
  MCM_DATA_API_MIN = 0,
  MCM_DATA_API_START_DATA_CALL,
  MCM_DATA_API_END_DATA_CALL,
  MCM_DATA_API_MAX
}mcm_data_api_types_t;

/*---------------------------------------------------------------------------
  QMI MCM DATA Service state info
---------------------------------------------------------------------------*/

typedef struct
{
  qmi_csi_service_handle service_handle;
  int                    num_clients;
  void*                  client_handle_list[MCM_DATA_MAX_CLIENT_HANDLES];
  int                    qmi_instance;
  int                    client_ref_count;
  qmi_client_type        qmi_dsd_hndl;
  qmi_client_type        qmi_dpm_hndl;
  qmi_client_type        svc_cli_notifier;
  qmi_cci_os_signal_type cli_os_params;
  boolean                mcm_data_inited;
  boolean                mcm_data_notifier_inited;
  mcm_data_modem_ssr_status_type_t   mcm_data_modem_local_state;
} qmi_mcm_data_state_info_type;

/*---------------------------------------------------------------------------
  Event data structure for dsi events
---------------------------------------------------------------------------*/

typedef struct mcm_data_dsi_event_data
{
  dsi_hndl_t             dsi_nethandle;
  void                   *user_data;
  dsi_net_evt_t          evt;
  dsi_evt_payload_t      *payload_ptr;
} mcm_data_dsi_event_data_t;

/*---------------------------------------------------------------------------
  Event data structure for wds events
---------------------------------------------------------------------------*/

typedef struct mcm_data_wds_event_data
{
  void                         *user_data;
  unsigned int                  ind_id;
  wds_event_report_ind_msg_v01 ind_data;
  void                         *self;
} mcm_data_wds_event_data_t;

/*---------------------------------------------------------------------------
  Event data structure for dsd events
---------------------------------------------------------------------------*/

typedef struct mcm_data_dsd_event_data
{
  dsd_system_status_info_type_v01 status_info;
} mcm_data_dsd_event_data_t;

/*---------------------------------------------------------------------------
  Event data structure for ssr related events
---------------------------------------------------------------------------*/

typedef struct mcm_data_ssr_event_data
{
  mcm_data_modem_ssr_status_type_t status_info;
} mcm_data_ssr_event_data_t;

/*---------------------------------------------------------------------------
  Generic event data structure
---------------------------------------------------------------------------*/

typedef struct mcm_data_event_data
{

  mcm_data_event_type_t event_id;
  union
  {
    mcm_data_wds_event_data_t wds_event;
    mcm_data_dsd_event_data_t dsd_event;
    mcm_data_dsi_event_data_t dsi_event;
    mcm_data_ssr_event_data_t ssr_event;
  } evt_data;

} mcm_data_event_data_t;

/*---------------------------------------------------------------------------
  QMI MCM DATA Client state info
---------------------------------------------------------------------------*/

typedef struct {
  qmi_client_handle                   clnt;
  boolean                             client_enabled;
  mcm_data_event_register_req_msg_v01 event_reg_data;
} qmi_mcm_data_client_info_type;

/*---------------------------------------------------------------------------
    Structure to store relation between token_id, api_type and call_id
---------------------------------------------------------------------------*/

typedef struct mcm_data_transaction_table_entry_s {

  /* Call_id used for generating token */
  uint32_t call_id;

  /* API type for which this token entry is used */
  mcm_data_api_types_t api_type;

  /* Denotes whether entry is valid or not */
  void *self;

  /* QMI client handle */
  qmi_client_handle qmi_cli_handle;

  /* QMI request handle */
  qmi_req_handle req_handle;

} mcm_data_txn_table_entry;

/*---------------------------------------------------------------------------
   Structure to store call_id and handles corresponding to the call_id
---------------------------------------------------------------------------*/
typedef struct mcm_data_call_table_entry_s {

  /* Call_id obtained while registering call in mcm_data */
  uint32_t call_id;

  /* Handle obtained from dsi_netctrl */
  dsi_hndl_t dsi_handle;

  /* QMI client handle */
  qmi_client_handle qmi_cli_handle;

  /* QMI WDS handle*/
  qmi_client_type          qmi_wds_hndl;

  qmi_cci_os_signal_type   wds_cli_os_params;

  /* Denotes whether entry is valid or not */
  void *self;

}mcm_data_call_table_entry;

/*---------------------------------------------------------------------------
  Function declations for mcm_data_srv
---------------------------------------------------------------------------*/

int32_t mcm_data_init_srv
(
  void
);

int32_t create_mcm_data_socket
(
  void
);

int32_t create_mcm_data_wds_socket
(
  void
);

int32_t mcm_data_create_client_socket
(
  unsigned int *sockfd
);

void mcm_data_dsi_net_ev_cb
(
  dsi_hndl_t        hndl,
  void              *user_data,
  dsi_net_evt_t     evt,
  dsi_evt_payload_t *payload_ptr
);

void mcm_data_dsi_net_ev_handler
(
  dsi_hndl_t        hndl,
  void              *user_data,
  dsi_net_evt_t     evt,
  dsi_evt_payload_t *payload_ptr
);

void mcm_data_qmi_wds_ind_cb
(
  qmi_client_type                user_handle,
  unsigned long                  msg_id,
  unsigned char                  *ind_buf,
  int                            ind_buf_len,
  void                           *ind_cb_data
);

void mcm_data_qmi_wds_ind_handler
(
  void * user_data,
  unsigned int   ind_id,
  wds_event_report_ind_msg_v01 *ind_data
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
  dsd_system_status_info_type_v01 *dsd_ind_msg
);

ds_dll_el_t* get_call_tbl_head
(
  void
);

ds_dll_el_t* get_txn_tbl_head
(
  void
);

#endif
