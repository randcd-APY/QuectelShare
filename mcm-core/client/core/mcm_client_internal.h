/*************************************************************************************
 Copyright (c) 2013-2014, 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**************************************************************************************/

#ifndef MCM_CLIENT_INTERNAL_H
#define MCM_CLIENT_INTERNAL_H

#include <stdint.h>
#include "qmi_client.h"
#include "mcm_constants.h"
#include "log_util.h"

typedef struct client_handle_info_type
{
    qmi_client_type mcm_ril_handle;
    qmi_client_type mcm_data_handle;
    qmi_client_type mcm_sim_handle;
    qmi_client_type mcm_loc_handle;
    qmi_client_type mcm_mobileap_handle;
    qmi_client_type mcm_mm_handle;
    qmi_client_type mcm_atcop_handle;
    qmi_client_type mcm_srv_mgr_handle;
}client_handle_info_type;

uint32_t mcm_internal_get_srv_id_for_msg_id
(
    int msg_id,
    int *srv_id
);
uint32_t mcm_client_internal_get_mcm_client_handle
(
    mcm_client_handle_type *mcm_handle
);

uint32_t mcm_internal_client_release
(
    mcm_client_handle_type handle
);
uint32_t mcm_client_internal_update_cb_info
(
    mcm_client_handle_type   mcm_handle,
    mcm_client_async_cb      client_resp_cb,
    mcm_client_ind_cb        client_ind_cb
);

uint32_t mcm_client_internal_update_async_cb_info
(
    mcm_client_handle_type   handle,
    mcm_client_async_cb      async_resp_cb,
    int                     *clnt_token
);

uint32_t mcm_client_internal_update_qmi_handle
(
    mcm_client_handle_type    mcm_handle,
    int                       service_id,
    qmi_client_type           user_handle
);
uint32_t mcm_client_internal_get_async_cb_for_mcm_handle
(
    mcm_client_handle_type    mcm_handle,
    int                       token_id,
    mcm_client_async_cb      *async_cb
);

uint32_t mcm_client_internal_get_ind_cb_for_mcm_handle
(
    mcm_client_handle_type    mcm_handle,
    mcm_client_ind_cb        *ind_cb
);

uint32_t mcm_client_internal_get_qmi_handle_for_mcm_handle
(
    mcm_client_handle_type     handle,
    int                        msg_id,
    qmi_client_type           *mcm_client_handle
);

uint32_t mcm_client_internal_get_mcm_handle_for_qmi_handle
(
    qmi_client_type         mcm_client_handle,
    mcm_client_handle_type *handle
);

uint32_t mcm_client_internal_mutex_init();


void mcm_update_ssr_status(uint8_t ssr_status);
uint8_t mcm_is_device_in_ssr(void);
uint8_t mcm_is_msg_for_mobileap_service(int msg_id);
#endif //MCM_CLIENT_INTERNAL_H

