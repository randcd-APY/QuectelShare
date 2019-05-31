
/*************************************************************************************
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
**************************************************************************************/


#ifndef MCM_SERVER_ADAPTOR_H
#define MCM_SERVER_ADAPTOR_H

#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include "qmi_client.h"

int mcm_srv_get_client_request_msg(void *req_msg);

int mcm_srv_send_response_msg_to_client(void *resp_msg);

int mcm_srv_adaptor_post_async_response
(
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *resp_c_struct,
    unsigned int resp_c_struct_len
);

int mcm_srv_adaptor_post_response
(
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *resp_c_struct,
    unsigned int resp_c_struct_len
);

int mcm_srv_adaptor_post_indication
(
    qmi_client_handle client_handle,
    unsigned int msg_id,
    void *ind_c_struct,
    unsigned int ind_c_struct_len
);


#endif //MCM_CLIENT_ADAPTOR_H
