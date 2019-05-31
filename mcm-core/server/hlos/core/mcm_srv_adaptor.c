/*************************************************************************************
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
**************************************************************************************/


#include "mcm_srv_adaptor.h"
#include "util_log.h"

#ifdef SINGLE_PROCESS_CONFIG
#include "mcm_msg_queue.h"
#include "mcm_single_process.h"
#endif

#ifdef SINGLE_PROCESS_CONFIG
extern mcm_msg_queue *mcm_msg_response_msg_queue;
extern mcm_signal_data_type signal_response_queue;

extern mcm_msg_queue *mcm_msg_ind_msg_queue;
extern mcm_signal_data_type signal_ind_queue;

extern mcm_msg_queue *mcm_msg_asyncresp_msg_queue;
extern mcm_signal_data_type signal_asyncresp_queue;

extern int mcm_srv_is_request_async(int req_handle, qmi_client_type client_handle);

#endif //SINGLE_PROCESS_CONFIG
#if 0
int mcm_srv_get_client_request_msg(void *req_msg)
{

}

int mcm_srv_send_response_msg_to_client(void *resp_msg)
{

}
#endif
int mcm_srv_adaptor_post_response
(
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *resp_c_struct,
    unsigned int resp_c_struct_len
)
{
    UTIL_LOG_MSG("mcm_srv_adaptor_post_response Enter");
    qmi_csi_error   rc = QMI_CSI_NO_ERR;

#ifdef SINGLE_PROCESS_CONFIG
    int is_async = FALSE;
    mcm_async_resp *msg;
    qmi_client_type client_handle;

    client_handle = mcm_internal_get_client_handle_from_req_handle(req_handle);

    is_async = mcm_srv_is_request_async(req_handle, client_handle);

    if ( is_async )
    {
        msg = (mcm_async_resp*)malloc(sizeof(mcm_async_resp));
        msg->req_handle = req_handle;
        msg->msg_id = msg_id;
        msg->resp_c_struct = resp_c_struct;
        msg->resp_c_struct_len = resp_c_struct_len;
        mcm_msg_queue_enqueue(mcm_msg_asyncresp_msg_queue,msg);
        MCM_SEND_SIGNAL(signal_asyncresp_queue);
    }
    else
    {
        mcm_msg_queue_enqueue(mcm_msg_response_msg_queue,resp_c_struct);
        MCM_SEND_SIGNAL(signal_response_queue);
    }

#else // MULTI_PROCESS_CONFIG
    rc = qmi_csi_send_resp(req_handle,msg_id,resp_c_struct,resp_c_struct_len);
    UTIL_LOG_MSG("qmi_csi_send_resp req_handle:%x msg_id:%x rc:%d", req_handle, msg_id, rc);

#endif // SINGLE_PROCESS_CONFIG
    return 0;
}

int mcm_srv_adaptor_post_indication
(
    qmi_client_handle client_handle,
    unsigned int msg_id,
    void *ind_c_struct,
    unsigned int ind_c_struct_len
)
{
    qmi_csi_error rc = QMI_CSI_NO_ERR;
    UTIL_LOG_MSG("mcm_srv_adaptor_post_indication Enter");

#ifdef SINGLE_PROCESS_CONFIG

    mcm_ind_resp *msg = (mcm_ind_resp*)malloc(sizeof(mcm_ind_resp));
    msg->client_handle = client_handle;
    msg->msg_id = msg_id;
    msg->ind_c_struct = ind_c_struct;
    msg->ind_c_struct_len = ind_c_struct_len;

    mcm_msg_queue_enqueue(mcm_msg_ind_msg_queue,msg);
    MCM_SEND_SIGNAL(signal_ind_queue);

#else // MULTI_PROCESS_CONFIG

    rc = qmi_csi_send_ind(client_handle,msg_id,ind_c_struct,ind_c_struct_len);
    UTIL_LOG_MSG("qmi_csi_send_ind rc:%d", rc);

#endif //SINGLE_PROCESS_CONFIG
    return 0;
}

