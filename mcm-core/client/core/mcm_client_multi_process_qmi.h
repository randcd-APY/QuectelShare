
#ifndef MCM_CLIENT_MULTI_PROCESS_QMI_H
#define MCM_CLIENT_MULTI_PROCESS_QMI_H

/*************************************************************************************
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
**************************************************************************************/
#include "log_util.h"

#ifdef MULTI_PROCESS_CONFIG

#include "qmi_client.h"

qmi_client_error_type mcm_qmi_multi_process_init
(
    qmi_service_info                          *service_info,
    qmi_idl_service_object_type               service_obj,
    qmi_client_ind_cb                         ind_cb,
    void                                      *ind_cb_data,
    qmi_client_os_params                      *os_params,
    qmi_client_type                           *user_handle
);
#endif // MULTI_PROCESS_CONFIG

#endif //MCM_CLIENT_MULTI_PROCESS_QMI_H

