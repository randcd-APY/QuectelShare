/*************************************************************************************
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
**************************************************************************************/


#ifndef MCM_SRV_H
#define MCM_SRV_H

//#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "qmi_csi.h"

//#include "cri_core.h"

typedef struct qmi_mcm_svc_info_type
{
    int service_id;
    qmi_client_handle client_handle;
} qmi_mcm_svc_info_type;

int mcm_srv_qmi_client_init();
#if 0
void mcm_client_qmi_register_services
(
    qmi_client_type *clnt_handle
);

void mcm_srv_sys_event_handler
(
    qmi_sys_event_type        event_id,
    const qmi_sys_event_info_type   *event_info,
    void                      *user_data
);
#endif

int mcm_srv_post_response
(
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *resp_c_struct,
    unsigned int resp_c_struct_len
);

int mcm_srv_post_indication
(
    qmi_client_handle client_handle,
    unsigned int msg_id,
    void *ind_c_struct,
    unsigned int ind_c_struct_len
);

#if 0
int mcm_srv_post_async_response
(
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *resp_c_struct,
    unsigned int resp_c_struct_len
);
#endif
#endif // MCM_SRV_H
