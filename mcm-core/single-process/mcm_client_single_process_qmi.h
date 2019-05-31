/*************************************************************************************
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
**************************************************************************************/

#ifndef MCM_CLIENT_QMI_H
#define MCM_CLIENT_QMI_H

#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include "common_v01.h"
#include "qmi_client.h"
#include "mcm_constants.h"

typedef struct mcm_ind_cb_map
{
    qmi_client_type client_handle;
    qmi_client_ind_cb ind_cb;
}mcm_ind_cb_map;


typedef struct mcm_req_handle_map
{
    int req_handle;
    int service_id;
    int msg_id;
    qmi_client_type user_handle;
    int is_async;
    qmi_client_recv_msg_async_cb async_cb;
    void *async_cb_data;
}mcm_req_handle_map;

qmi_client_error_type mcm_qmi_single_process_init
(
    qmi_service_info                          *service_info,
    qmi_idl_service_object_type               service_obj,
    qmi_client_ind_cb                         ind_cb,
    void                                      *ind_cb_data,
    qmi_client_os_params                      *os_params,
    qmi_client_type                           *user_handle
);

void mcm_srv_qmi_handle_init_request(int service_id,qmi_client_handle client_handle);

int mcm_qmi_release(int mcm_client_handle);

int mcm_qmi_send_msg_sync(qmi_client_type client_handle,
                        int message_id,
                        void *req_message,
                        int req_message_len,
                        void **resp_message,
                        int resp_message_len,
                        int time_out);


void mcm_srv_qmi_handle_request
(
    void               *clnt_info,
    qmi_req_handle     req_handle,
    int                msg_id,
    void               *req_c_struct,
    int                req_c_struct_len,
    void               *service_handle
);

#endif //MCM_CLIENT_QMI_H

