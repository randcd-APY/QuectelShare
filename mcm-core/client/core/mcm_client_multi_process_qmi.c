
/*************************************************************************************
 Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
**************************************************************************************/

#include "comdef.h"
#include "mcm_constants.h"
#include "mcm_client_multi_process_qmi.h"

#define MCM_MAX_RETRY_COUNT 1
#define MCM_MAX_FIRST_RETRY_TIMEOUT 100
#define MCM_MAX_RETRY_TIMEOUT 1000

#ifdef MULTI_PROCESS_CONFIG
qmi_client_error_type mcm_qmi_multi_process_init
(
    qmi_service_info                          *service_info,
    qmi_idl_service_object_type               service_obj,
    qmi_client_ind_cb                         ind_cb,
    void                                      *ind_cb_data,
    qmi_client_os_params 	                    *os_params,
    qmi_client_type                         *user_handle
)
{
    uint32_t               num_services, num_entries;
    qmi_client_type  mcm_ril_client, client_notifier;
    uint32_t               retry_count = 0;
    uint32_t               retry_timeout = MCM_MAX_FIRST_RETRY_TIMEOUT;
    uint32_t               is_service_up = FALSE;
    int rc = QMI_NO_ERR;

    LOG_MSG_INFO("mcm_qmi_multi_process_init ENTER");
    memset(&mcm_ril_client, 0, sizeof(mcm_ril_client));

    /* Validate input */
    if (NULL == service_info ||
    NULL == service_obj  ||
    NULL == os_params)
    {
        LOG_MSG_INFO("Invalid input parameter exit");
        rc = QMI_SERVICE_ERR;
        return rc;
    }

    /* init notifier for dm_service */
    //rc = qmi_client_notifier_init(service_obj, os_params, &client_notifier);

    //if (rc != QMI_NO_ERR)
    //{
    //    LOG_MSG_INFO("Notifier initialization failed exit rc:%d", rc);
    //    rc = QMI_SERVICE_ERR;
    //    return rc;
    //}

    /* Check if the service is up, if not wait on a signal */
    while( retry_count < MCM_MAX_RETRY_COUNT )
    {
        rc = qmi_client_get_service_list( service_obj, NULL, NULL, &num_services);
        LOG_MSG_INFO("qmi_client_get_service_list rc:%d, num_services:%d", rc, num_services);

        if(rc == QMI_NO_ERR)
            break;

        /* wait for server to come up */
        LOG_MSG_INFO("wait for %d milli seconds for server to come up",retry_timeout);
        QMI_CCI_OS_SIGNAL_WAIT(os_params, retry_timeout);
        retry_count++;
        retry_timeout = MCM_MAX_RETRY_TIMEOUT;
        LOG_MSG_INFO("Get service list retry - %d",retry_count);
    }

    if ( rc == QMI_NO_ERR )
    {
        is_service_up = TRUE;
    }


    if ( is_service_up )
    {

        num_entries = num_services;
        /* The server has come up, store the information in info variable */
        rc = qmi_client_get_service_list( service_obj, service_info, &num_entries, &num_services);
        LOG_MSG_INFO("qmi_client_get_service_list() returned %d num_entries = %d num_services = %d", rc, num_entries, num_services);

        rc = qmi_client_init(service_info, service_obj, ind_cb, NULL, NULL, &mcm_ril_client);

      //  qmi_client_release(client_notifier);
      //  client_notifier = NULL;

        if ( QMI_NO_ERR != rc)
        {
            LOG_MSG_INFO("qmi_client_init failed exit rc:%d", rc);
            mcm_ril_client = NULL;
            return rc;
        }
        *user_handle = mcm_ril_client;
    }

    LOG_MSG_INFO("qmi_client_init rc:%d mcm_ril_client:%x", rc, mcm_ril_client);
    LOG_MSG_INFO("mcm_qmi_multi_process_init EXIT");

    return rc;
}
#endif //MULTI_PROCESS_CONFIG



