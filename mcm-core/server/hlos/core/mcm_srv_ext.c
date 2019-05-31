
/*************************************************************************************
   Copyright (c) 2013, 2017 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
**************************************************************************************/


#include "mcm_srv.h"
#include "utils_common.h"
#include "util_timer.h"

//=============================================================================
// FUNCTION: mcm_srv_qmi_client_init
//
// DESCRIPTION:
// initializes crf services, start crf core handler threads
// @ingroup mcm_dm_api
//
// @param[in]
// @param[out]
//
// @return
//    mcm msg
//=============================================================================

int mcm_srv_qmi_client_init()
{
    UTIL_LOG_MSG( "mcm_srv_qmi_client_init:");

    hlos_core_mcm_ssr_init();

    hlos_core_ril_client_init();

    hlos_core_flow_control_init();

    core_handler_start();

    cri_core_start();

    util_timer_start();

    return 0;
}

//=============================================================================
// FUNCTION: mcm_srv_post_response
//
// DESCRIPTION:
// wrapped around qmi_qcs_post_response
// @ingroup mcm_dm_api
//
// @param[in]
// @param[out]
//
// @return
//    mcm msg
//=============================================================================

int mcm_srv_post_response
(
    qmi_req_handle req_handle,
    unsigned int msg_id,
    void *resp_c_struct,
    unsigned int resp_c_struct_len
)
{
    return mcm_srv_adaptor_post_response(req_handle,msg_id, resp_c_struct, resp_c_struct_len);
}


//=============================================================================
// FUNCTION: mcm_srv_post_indication
//
// DESCRIPTION:
// wrapped around qmi_qcs_post_ind
// @ingroup mcm_dm_api
//
// @param[in]
// @param[out]
//
// @return
//    mcm msg
//=============================================================================

int mcm_srv_post_indication
(
    qmi_client_handle client_handle,
    unsigned int msg_id,
    void *ind_c_struct,
    unsigned int ind_c_struct_len
)
{
    return mcm_srv_adaptor_post_indication(client_handle, msg_id, ind_c_struct, ind_c_struct_len);
}

