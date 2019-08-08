/**************************************************************************************
Copyright (c) 2013-2014, 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
**************************************************************************************/
#include "mcm_constants.h"
#include "mcm_qmi_client.h"
#include "mcm_client_internal.h"
#include "mcm_service_object_v01.h"
#include "mcm_client.h"
#include <pthread.h>
#include "log_util.h"

#define MCM_CLIENT_MUTEX_UNLOCK()     pthread_mutex_unlock(&client_token_mutex)
#define MCM_CLIENT_MUTEX_LOCK()                                    \
{                                                                  \
    int ret;                                                       \
    ret = pthread_mutex_lock(&client_token_mutex);                 \
    if(EOWNERDEAD == ret)                                          \
    {                                                              \
        pthread_mutex_consistent(&client_token_mutex);             \
    }                                                              \
}


int                 client_token = 10;
pthread_mutex_t     client_token_mutex;
pthread_mutexattr_t client_token_mutexattr;

//=============================================================================
// FUNCTION: mcm_init <fd>
//
// DESCRIPTION:
// Initialization of mcm client ( RIL ).
// @ingroup
//
// @param[in] fd
// @return
//       user_handle for the client
//
//=============================================================================
uint32 mcm_client_mutex_init()
{
    int ret_val = MCM_SUCCESS_V01;

    LOG_MSG_INFO("mcm_client_mutex_init Enter");

    do
    {
        ret_val = pthread_mutexattr_init(&client_token_mutexattr);
        if(ret_val)
        {
            LOG_MSG_INFO("Client Token Mutex Attribute Initialization error - %d, %s",ret_val, strerror(ret_val));
            ret_val = MCM_ERROR_GENERIC_V01;
            break;
        }

        ret_val = pthread_mutexattr_setrobust(&client_token_mutexattr, PTHREAD_MUTEX_ROBUST);
        if(ret_val)
        {
            LOG_MSG_INFO("Client Token Mutex Set robust error - %d, %s",ret_val, strerror(ret_val));
            ret_val = MCM_ERROR_GENERIC_V01;
            break;
        }

        ret_val = pthread_mutex_init(&client_token_mutex, &client_token_mutexattr);
        if(ret_val)
        {
            LOG_MSG_INFO("Client Token Mutex Init error - %d, %s",ret_val, strerror(ret_val));
            ret_val = MCM_ERROR_GENERIC_V01;
            break;
        }
    }while(FALSE);

    LOG_MSG_INFO("mcm_client_mutex_init Exit with ret: %d", ret_val);

    return ret_val;
}

void *mcm_srv_manger_init()
{

    qmi_client_error_type           client_err = QMI_NO_ERR;
    qmi_idl_service_object_type     service_obj = NULL;
    qmi_client_type                 user_handle;
    qmi_service_info                info[10];
    qmi_client_os_params            os_params;

    memset(&user_handle, 0, sizeof(qmi_client_type));
    memset(&os_params, 0, sizeof(qmi_client_os_params));

    service_obj = mcm_client_get_service_object_v01();

    client_err = mcm_qmi_client_init(&info[0],
                                    service_obj,
                                    mcm_qmi_ind_cb,
                                    NULL,
                                    &os_params,
                                    &user_handle);

    LOG_MSG_INFO("mcm_srv_manger_init: client_err - %d",client_err);

    return user_handle;

}

void *mcm_ril_init()
{

    qmi_client_error_type           client_err = QMI_NO_ERR;
    qmi_idl_service_object_type     service_obj = NULL;
    qmi_client_type                 user_handle;
    qmi_service_info                info[10];
    qmi_client_os_params            os_params;

    memset(&user_handle, 0, sizeof(qmi_client_type));
    memset(&os_params, 0, sizeof(qmi_client_os_params));

    service_obj = mcm_ril_service_get_service_object_v01();

    client_err = mcm_qmi_client_init(&info[0],
                                    service_obj,
                                    mcm_qmi_ind_cb,
                                    NULL,
                                    &os_params,
                                    &user_handle);

    LOG_MSG_INFO("mcm_ril_init: client_err - %d",client_err);

    return user_handle;

}


void *mcm_data_init()
{

    qmi_client_error_type client_err = QMI_NO_ERR;
    qmi_idl_service_object_type	service_obj = NULL;
    qmi_client_type                 user_handle;
    qmi_service_info                info[10];
    qmi_client_os_params            os_params;

    memset(&user_handle, 0, sizeof(qmi_client_type));
    memset(&os_params, 0, sizeof(qmi_client_os_params));

    service_obj = mcm_data_get_service_object_v01();

    client_err = mcm_qmi_client_init(&info[0],
                                    service_obj,
                                    mcm_qmi_ind_cb,
                                    NULL,
                                    &os_params,
                                    &user_handle);

    LOG_MSG_INFO("mcm_data_init: client_err - %d",client_err);

    return user_handle;

}

void *mcm_sim_init()
{

    qmi_client_error_type client_err = QMI_NO_ERR;
    qmi_idl_service_object_type	service_obj = NULL;
    qmi_client_type                 user_handle;
    qmi_service_info                info[10];
    qmi_client_os_params            os_params;

    memset(&user_handle, 0, sizeof(qmi_client_type));
    memset(&os_params, 0, sizeof(qmi_client_os_params));

    service_obj = mcm_sim_get_service_object_v01();

    client_err = mcm_qmi_client_init(&info[0],
                                    service_obj,
                                    mcm_qmi_ind_cb,
                                    NULL,
                                    &os_params,
                                    &user_handle);

    LOG_MSG_INFO("mcm_sim_init: client_err - %d",client_err);

    return user_handle;

}


void *mcm_mobileap_init()
{

    qmi_client_error_type client_err = QMI_NO_ERR;
    qmi_idl_service_object_type	service_obj = NULL;
    qmi_client_type                 user_handle;
    qmi_service_info                info[10];
    qmi_client_os_params            os_params;

    memset(&user_handle, 0, sizeof(qmi_client_type));
    memset(&os_params, 0, sizeof(qmi_client_os_params));

    service_obj = mcm_mobileap_get_service_object_v01();

    client_err = mcm_qmi_client_init(&info[0],
                                    service_obj,
                                    mcm_qmi_ind_cb,
                                    NULL,
                                    &os_params,
                                    &user_handle);

    LOG_MSG_INFO("mcm_mobileap_init: client_err - %d",client_err);

    return user_handle;

}

void *mcm_atcop_init()
{

    qmi_client_error_type client_err = QMI_NO_ERR;
    qmi_idl_service_object_type service_obj = NULL;
    qmi_client_type                 user_handle;
    qmi_service_info                info[10];
    qmi_client_os_params            os_params;

    memset(&user_handle, 0, sizeof(qmi_client_type));
    memset(&os_params, 0, sizeof(qmi_client_os_params));

    service_obj = mcm_atcop_get_service_object_v01();

    client_err = mcm_qmi_client_init(&info[0],
                                    service_obj,
                                    mcm_qmi_ind_cb,
                                    NULL,
                                    &os_params,
                                    &user_handle);

    LOG_MSG_INFO("mcm_atcop_init: client_err - %d",client_err);

    return user_handle;

}

void *mcm_loc_init()
{

    qmi_client_error_type client_err = QMI_NO_ERR;
    qmi_idl_service_object_type service_obj = NULL;
    qmi_client_type                 user_handle;
    qmi_service_info                info[10];
    qmi_client_os_params            os_params;

    memset(&user_handle, 0, sizeof(qmi_client_type));
    memset(&os_params, 0, sizeof(qmi_client_os_params));

    service_obj = mcm_loc_get_service_object_v01();

    client_err = mcm_qmi_client_init(&info[0],
                                    service_obj,
                                    mcm_qmi_ind_cb,
                                    NULL,
                                    &os_params,
                                    &user_handle);

    LOG_MSG_INFO("mcm_loc_init: client_err - %d",client_err);

    return user_handle;

}

//=============================================================================
// FUNCTION: mcm_client_init <fd>
//
// DESCRIPTION:
// Initialize the MCM interface
// This function should be called at startup and before any other functions
// are called. It returns a file descriptor associated with this client and
// an associated event queue which can be used as a regular file descirptor
// in system calls such as select(2) and poll(2)
// @ingroup mcm_client_api
//
// @param[out] fd file descriptor associated with this instance and event queue
//
// @return
//    MCM_SUCCESS -- 0 is success\n
//    MCM_ERROR_TBD...
//=============================================================================
uint32 mcm_client_init
(
    mcm_client_handle_type      *client_handle,
    mcm_client_ind_cb            client_ind_cb,
    mcm_client_async_cb          client_resp_cb
)
{
    uint32            ret_val = MCM_SUCCESS_V01;
    int               mcm_handle;
    uint32            fail_count = 0;
    qmi_client_type   user_handle;
    log_util_type_e local_log_status = LOG_UTIL_NONE;

    local_log_status = log_util_init();
    LOG_MSG_INFO("log_status = %d",local_log_status);
    LOG_MSG_INFO("mcm_client_int ENTER");

    memset(&user_handle, NIL, sizeof(qmi_client_type));

    do {

        if((MCM_SUCCESS_V01 != mcm_client_mutex_init())
            || (MCM_SUCCESS_V01 != mcm_client_internal_mutex_init())
                || (MCM_SUCCESS_V01 != mcm_qmi_client_mutex_init()))
        {
            ret_val = MCM_ERROR_GENERIC_V01;
            LOG_MSG_INFO("Failed to initialize client robust mutexes ret_val:%d", ret_val);
            break;
        }


        if(MCM_SUCCESS_V01 !=
            (ret_val = mcm_client_internal_get_mcm_client_handle(&mcm_handle)))
        {
            LOG_MSG_INFO("failed in mcm_client_internal_get_mcm_client_handle ret_val:%d", ret_val);
            break;
        }

        if(MCM_SUCCESS_V01 !=
            (ret_val = mcm_client_internal_update_cb_info(mcm_handle, client_resp_cb, client_ind_cb)))
        {
            LOG_MSG_INFO("failed in mcm_client_internal_update_cb_info ret_val:%d", ret_val);
            break;
        }

        user_handle = mcm_ril_init();
        if (user_handle != NULL)
        {
            mcm_client_internal_update_qmi_handle(mcm_handle, MCM_RIL_SERVICE, user_handle);
        }
        else
        {
            LOG_MSG_INFO("failed to initialize RIL service ret_val:%d", ret_val);
            /* break out without counting fail_count since RIL/SRV_MGR is mandatory */
            ret_val = MCM_ERROR_MCM_SERVICES_NOT_AVAILABLE_V01;
            break;
        }

        user_handle = mcm_srv_manger_init();
        if (user_handle != NULL)
        {
            mcm_client_internal_update_qmi_handle(mcm_handle, MCM_SRV_MANAGER, user_handle);
        }
        else
        {
            LOG_MSG_INFO("failed to initialize SRV_MGR ret_val:%d", ret_val);
            /* break out without counting fail_count since RIL/SRV_MGR is mandatory */
            ret_val = MCM_ERROR_MCM_SERVICES_NOT_AVAILABLE_V01;
            break;
        }
        user_handle = mcm_data_init();
        if (user_handle != NULL)
        {
            mcm_client_internal_update_qmi_handle(mcm_handle, MCM_DATA_SERVICE, user_handle);
        }
        else
        {
            fail_count++;
            LOG_MSG_INFO("failed to initialize DATA service ret_val:%d", ret_val);
        }

        user_handle = mcm_sim_init();
        if (user_handle != NULL)
        {
            mcm_client_internal_update_qmi_handle(mcm_handle, MCM_SIM_SERVICE, user_handle);
        }
        else
        {
            fail_count++;
            LOG_MSG_INFO("failed to initialize SIM service ret_val:%d", ret_val);
        }

        user_handle = mcm_mobileap_init();
        if (user_handle != NULL)
        {
            mcm_client_internal_update_qmi_handle(mcm_handle, MCM_MOBILEAP_SERVICE, user_handle);
        }
        else
        {
            fail_count++;
            LOG_MSG_INFO("failed to initialize MobileAP service ret_val:%d", ret_val);
        }

        user_handle = mcm_loc_init();
        if (user_handle != NULL)
        {
            mcm_client_internal_update_qmi_handle(mcm_handle, MCM_LOC_SERVICE, user_handle);
        }
        else
        {
            fail_count++;
            LOG_MSG_INFO("failed to initialize LOC service ret_val:%d", ret_val);
        }

        user_handle = mcm_atcop_init();
        if (user_handle != NULL)
        {
            mcm_client_internal_update_qmi_handle(mcm_handle, MCM_ATCOP_SERVICE, user_handle);
        }
        else
        {
            fail_count++;
            LOG_MSG_INFO("failed to initialize ATCOP service ret_val:%d", ret_val);
        }

        if ( fail_count > 0 )
        {
            ret_val = MCM_SUCCESS_CONDITIONAL_SUCCESS_V01;
            LOG_MSG_INFO("failed to initialize few services required for mcm client : ret_val:%d", ret_val);
            LOG_MSG_INFO("Only Limited functionality available for client");
        }
        else
        {
            ret_val = MCM_SUCCESS_V01;
            LOG_MSG_INFO("mcm_client_init SUCCESS EXIT");
        }
        *client_handle = mcm_handle;
    }
    while (0);

    LOG_MSG_INFO("mcm_client_init EXIT - ret_val - %d", ret_val);
    return ret_val;
}


uint32 mcm_client_execute_command_async
(
    mcm_client_handle_type        client_handle,
    int                           msg_id,
    void                         *req_c_struct,
    int                           req_c_struct_len,
    void                         *resp_c_struct,
    int                           resp_c_struct_len,
    mcm_client_async_cb           async_resp_cb,
    void                          *token_id
)
{

    qmi_client_type    user_handle = NULL;
    int                ret_val = MCM_SUCCESS_V01;
    int               *token_ptr;
    qmi_txn_handle     txn_handle;

    LOG_MSG_INFO("mcm_client_execute_command_async ENTER msg_id:%d", msg_id);

    do {
        token_ptr = (int *)mcm_util_memory_alloc(sizeof(int));
        if (NULL == token_ptr)
        {
            LOG_MSG_INFO("memory allocate for token_ptr failed");
            break;
        }

        if((TRUE == mcm_is_device_in_ssr()) && (FALSE == mcm_is_msg_for_mobileap_service(msg_id)))
        {
            ret_val = MCM_ERROR_RADIO_RESET_V01;
            break;
        }

        MCM_CLIENT_MUTEX_LOCK();
        *token_ptr = client_token ++;
        MCM_CLIENT_MUTEX_UNLOCK();

        if(MCM_SUCCESS_V01 !=
            (ret_val = mcm_client_internal_update_async_cb_info(client_handle, async_resp_cb, token_ptr)))
        {
            LOG_MSG_INFO("failed in mcm_client_internal_update_async_cb_info ret_val:%d", ret_val);
            break;
        }
        if (MCM_SUCCESS_V01 !=
            (ret_val = mcm_client_internal_get_qmi_handle_for_mcm_handle(client_handle, msg_id, &user_handle)))
        {
            LOG_MSG_INFO("failed in mcm_client_internal_get_qmi_handle_for_mcm_handle ret_val:%d", ret_val);
            break;
        }

        ret_val = server_execute_async( user_handle,
                                        msg_id,
                                        req_c_struct,
                                        req_c_struct_len,
                                        resp_c_struct,
                                        resp_c_struct_len,
                                        mcm_qmi_async_cb,
                                        token_ptr,
                                        &txn_handle);

        qmi_util_log("server_execute_async ret_val:%d", ret_val);
        ret_val = convert_qmi_err_to_mcm(ret_val);

        if (MCM_SUCCESS_V01 != ret_val)
        {
            break;
        }

        *((int*)token_id) = *token_ptr;
    }while (0);

    if (MCM_SUCCESS_V01 == ret_val)
    {
        LOG_MSG_INFO("mcm_client_execute_command_async SUCCESS EXIT");
    }
    else
    {
        if (NULL != token_ptr)
        {
            mcm_util_memory_free(&token_ptr);
        }
        LOG_MSG_INFO("mcm_client_execute_command_async FAIL EXIT ret_val:%d", ret_val);
    }

    return ret_val;

}

uint32 mcm_client_execute_command_sync
(
    mcm_client_handle_type      client_handle,
    int                         msg_id,
    void                       *req_c_struct,
    int                         req_c_struct_len,
    void                       *resp_c_struct,
    int                         resp_c_struct_len
)
{
    qmi_client_type user_handle;
    int             ret_val = MCM_SUCCESS_V01;

    LOG_MSG_INFO("mcm_client_execute_command_sync ENTER msg_id:%d", msg_id);

    ret_val = mcm_client_execute_command_sync_ex(client_handle,
                                                 msg_id,
                                                 req_c_struct,
                                                 req_c_struct_len,
                                                 resp_c_struct,
                                                 resp_c_struct_len,
                                                 MCM_QMI_TIMEOUT);
    if (msg_id == MCM_CLIENT_REQUIRE_REQ_V01 ||
        msg_id == MCM_CLIENT_NOT_REQUIRE_REQ_V01)
    {
       mcm_query_qmi_handle(client_handle);
    }
    LOG_MSG_INFO("mcm_client_execute_command_sync EXIT");
    return ret_val;
}


uint32 mcm_client_execute_command_sync_ex
(
    mcm_client_handle_type      client_handle,
    int                         msg_id,
    void                       *req_c_struct,
    int                         req_c_struct_len,
    void                       *resp_c_struct,
    int                         resp_c_struct_len,
    uint32                      timeout
)
{
    qmi_client_type user_handle;
    int             ret_val = MCM_SUCCESS_V01;

    LOG_MSG_INFO("mcm_client_execute_command_sync_ex ENTER msg_id:%d", msg_id);

    do {
        if((TRUE == mcm_is_device_in_ssr()) && (FALSE == mcm_is_msg_for_mobileap_service(msg_id)))
        {
            ret_val = MCM_ERROR_RADIO_RESET_V01;
            break;
        }

        if(MCM_SUCCESS_V01 !=
            (ret_val = mcm_client_internal_get_qmi_handle_for_mcm_handle(client_handle, msg_id, &user_handle)))
        {
            break;
        }

        ret_val = server_execute_sync(user_handle,
                                            msg_id,
                                            req_c_struct,
                                            req_c_struct_len,
                                            resp_c_struct,
                                            resp_c_struct_len,
                                            timeout);

        qmi_util_log("server_execute_sync ret_val:%d", ret_val);
        ret_val = convert_qmi_err_to_mcm(ret_val);
        if(MCM_SUCCESS_V01 != ret_val)
        {
            break;
        }

    }while (0);

    if (MCM_SUCCESS_V01 == ret_val)
    {
        LOG_MSG_INFO("mcm_client_execute_command_sync_ex SUCCESS EXIT");
    }
    else
    {
        LOG_MSG_INFO("mcm_client_execute_command_sync_ex FAIL EXIT ret_val:%d", ret_val);
    }

    return ret_val;

}

//=============================================================================
// FUNCTION: mcm_client_release <fd>
//
// DESCRIPTION:
// Release the subscription to the client
// @ingroup mcm_client_api
//
// @param[in] fd file descriptor associated with this instance and event queue
//
// @return
//    MCM_SUCCESS -- 0 is success\n
//    MCM_ERROR_TBD...
//=============================================================================
uint32 mcm_client_release(mcm_client_handle_type handle)
{
    uint32 ret_val = MCM_SUCCESS_V01;
    ret_val = mcm_internal_client_release(handle);
    log_util_close();
    return ret_val;
}
