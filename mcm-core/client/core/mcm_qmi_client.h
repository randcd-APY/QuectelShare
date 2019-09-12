#ifndef MCM_QMI_CLIENT_H
#define MCM_QMI_CLIENT_H

/*************************************************************************************
 Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
**************************************************************************************/
#include "qmi_client.h"
#include "mcm_client.h"
#include "log_util.h"

typedef void (*mcm_qmi_client_ind_cb)
(
    qmi_client_type user_handle,
    unsigned int      msg_id,
    void             *ind_buf,
    unsigned int      ind_buf_len,
    void             *resp_cb_data
);

typedef void (*mcm_qmi_client_async_cb)
(
    qmi_client_type               user_handle,
    unsigned int                  msg_id,
    void                         *resp_c_struct,
    unsigned int                  resp_c_struct_len,
    void                         *resp_cb_data,
    qmi_client_error_type         transp_err
);

void mcm_qmi_ind_cb
(
    qmi_client_type   user_handle,
    unsigned int      msg_id,
    void             *ind_buf,
    unsigned int      ind_buf_len,
    void             *resp_cb_data
);

void mcm_qmi_async_cb
(
    qmi_client_type                user_handle,
    unsigned int                   msg_id,
    void                           *resp_c_struct,
    unsigned int                   resp_c_struct_len,
    void                           *resp_cb_data,
    qmi_client_error_type          transp_err
);

qmi_client_error_type mcm_qmi_client_init
(
    qmi_service_info *service_info,
    qmi_idl_service_object_type service_obj,
    qmi_client_ind_cb ind_cb,
    void *ind_cb_data,
    qmi_client_os_params *os_params,
    qmi_client_type *user_handle
);


qmi_client_error_type mcm_qmi_client_release
(
    qmi_client_type user_handle
);

qmi_client_error_type server_execute_sync
(
    qmi_client_type user_handle,
    unsigned int        msg_id,
    void                *req_c_struct,
    unsigned int        req_c_struct_len,
    void                *resp_c_struct,
    unsigned int        resp_c_struct_len,
    unsigned int        timeout_msecs
);

qmi_client_error_type server_execute_async
(
    qmi_client_type             user_handle,
    unsigned int                msg_id,
    void                        *req_c_struct,
    unsigned int                req_c_struct_len,
    void                        *resp_c_struct,
    unsigned int                resp_c_struct_len,
    qmi_client_recv_msg_async_cb resp_cb,
    void                        *resp_cb_data,
    qmi_txn_handle              *txn_handle
);

mcm_query_qmi_handle(mcm_client_handle_type mcm_clnt_hndl);
uint32_t mcm_qmi_client_mutex_init();

#endif // MCM_QMI_CLIENT_H
