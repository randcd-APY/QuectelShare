/*************************************************************************************
 Copyright (c) 2013-2014, 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**************************************************************************************/
#include "mcm_client.h"
#include "mcm_client_internal.h"
#include "mcm_constants.h"
#include <pthread.h>

#define MCM_CLIENT_INTERNAL_MUTEX_UNLOCK()     pthread_mutex_unlock(&client_mutex)
#define MCM_CLIENT_INTERNAL_MUTEX_LOCK()                           \
{                                                                  \
    int ret;                                                       \
    ret = pthread_mutex_lock(&client_mutex);                       \
    if(EOWNERDEAD == ret)                                          \
    {                                                              \
        pthread_mutex_consistent(&client_mutex);                   \
    }                                                              \
}
#define MCM_MOBILEAP_SERVICE_INDEX    6

pthread_mutex_t client_mutex;
pthread_mutexattr_t client_mutexattr;

int mcm_global_client_handle = 0;
uint8_t mcm_ssr_status = FALSE;

typedef struct mcm_async_token_type_t
{
    mcm_client_async_cb async_cb;
    int                 token_id;
}mcm_async_token_type;

typedef struct mcm_handle_map_type
{
    int mcm_handle;
    mcm_client_async_cb     default_async_cb;
    mcm_client_ind_cb       ind_cb;
    mcm_async_token_type    async_token[MCM_MAX_NUM_OF_ASYNC_CB];
    client_handle_info_type handle_info;
}mcm_handle_map_type;

mcm_handle_map_type mcm_handle_map[MCM_MAX_CLIENTS] = {{0}};

typedef struct mcm_msg_service_id_map_t
{
    int mcm_msg_id_min;
    int mcm_msg_id_max;
    int mcm_service_id;
}mcm_msg_service_id_map;

mcm_msg_service_id_map mcm_msg_srv_id_map_tbl[] =
{
    {MCM_RIL_DMS_SERVICE_MIN,   MCM_RIL_DMS_SERVICE_MAX,   MCM_RIL_SERVICE},      /* DMS      */
    {MCM_RIL_NAS_SERVICE_MIN,   MCM_RIL_NAS_SERVICE_MAX,   MCM_RIL_SERVICE},      /* NAS      */
    {MCM_RIL_SMS_SERVCIE_MIN,   MCM_RIL_SMS_SERVICE_MAX,   MCM_RIL_SERVICE},      /* SMS      */
    {MCM_RIL_VOICE_SERVICE_MIN, MCM_RIL_VOICE_SERVICE_MAX, MCM_RIL_SERVICE},      /* VOICE    */
    {MCM_SIM_SERVICE_MIN,       MCM_SIM_SERVICE_MAX,       MCM_SIM_SERVICE},      /* SIM      */
    {MCM_DATA_SERVICE_MIN,      MCM_DATA_SERVICE_MAX,      MCM_DATA_SERVICE},     /* DATA     */
    {MCM_MOBILEAP_SERVICE_MIN,  MCM_MOBILEAP_SERVICE_MAX,  MCM_MOBILEAP_SERVICE}, /* MOBILEAP */
    {MCM_LOC_SERVICE_MIN,       MCM_LOC_SERVICE_MAX,       MCM_LOC_SERVICE},       /* LOC     */
    {MCM_ATCOP_SERVICE_MIN,     MCM_ATCOP_SERVICE_MAX,     MCM_ATCOP_SERVICE},     /* ATCOP   */
    {MCM_SRV_MANAGER_MIN,       MCM_SRV_MANAGER_MAX,       MCM_SRV_MANAGER}        /* CLIENT  */
};

uint32_t mcm_client_internal_mutex_init()
{
    int ret_val = MCM_SUCCESS_V01;

    LOG_MSG_INFO("mcm_client_internal_mutex_init Enter");

    do
    {
        ret_val = pthread_mutexattr_init(&client_mutexattr);
        if(ret_val)
        {
            LOG_MSG_INFO("Client internal Mutex Attribute Initialization error - %d, %s",ret_val, strerror(ret_val));
            ret_val = MCM_ERROR_GENERIC_V01;
            break;
        }

        ret_val = pthread_mutexattr_setrobust(&client_mutexattr, PTHREAD_MUTEX_ROBUST);
        if(ret_val)
        {
            LOG_MSG_INFO("Client internal Mutex Set robust error - %d, %s",ret_val, strerror(ret_val));
            ret_val = MCM_ERROR_GENERIC_V01;
            break;
        }

        ret_val = pthread_mutex_init(&client_mutex, &client_mutexattr);
        if(ret_val)
        {
            LOG_MSG_INFO("Client internal Mutex Init error - %d, %s",ret_val, strerror(ret_val));
            ret_val = MCM_ERROR_GENERIC_V01;
            break;
        }
    }while(FALSE);

    LOG_MSG_INFO("mcm_client_internal_mutex_init Exit with ret: %d", ret_val);

    return ret_val;
}

//=============================================================================
// FUNCTION: mcm_client_internal_get_mcm_client_handle <fd> <out:token>
//
// DESCRIPTION:
// returns a fd to identify a client. fd can also be used by apps to call select, poll
//
// @ingroup
//
// @return
//    event ptr
//=============================================================================

uint32_t mcm_client_internal_get_mcm_client_handle
(
    mcm_client_handle_type *mcm_handle
)
{
    MCM_CLIENT_INTERNAL_MUTEX_LOCK();
    *mcm_handle = ++mcm_global_client_handle;
    MCM_CLIENT_INTERNAL_MUTEX_UNLOCK();
    return MCM_SUCCESS_V01;
}
uint32_t mcm_client_internal_update_cb_info
(
    mcm_client_handle_type   mcm_handle,
    mcm_client_async_cb      client_resp_cb,
    mcm_client_ind_cb        client_ind_cb
)
{
    int ret_val = MCM_SUCCESS_V01;
    int iter=0;

    MCM_CLIENT_INTERNAL_MUTEX_LOCK();
    for ( iter = 0; iter<MCM_MAX_CLIENTS; iter++)
    {
        if ( mcm_handle_map[iter].mcm_handle == mcm_handle )
        {
            LOG_MSG_INFO("mcm_client_internal_update_cb_info: found slot %d with mcm handle - %d", iter, mcm_handle);

            mcm_handle_map[iter].default_async_cb = client_resp_cb;
            mcm_handle_map[iter].ind_cb           = client_ind_cb;
            break;
        }
    }
    if (iter == MCM_MAX_CLIENTS)
    {
        LOG_MSG_INFO("mcm_client_internal_update_cb_info: check for empty slot and fill cb info");
        for (iter = 0; iter < MCM_MAX_CLIENTS; iter ++)
        {
            if (mcm_handle_map[iter].mcm_handle == 0)
            {
                LOG_MSG_INFO("mcm_client_internal_update_cb_info: found slot %d", iter);
                mcm_handle_map[iter].mcm_handle       = mcm_handle;
                mcm_handle_map[iter].default_async_cb = client_resp_cb;
                mcm_handle_map[iter].ind_cb           = client_ind_cb;
                memset(mcm_handle_map[iter].async_token, 0, sizeof(mcm_async_token_type)* MCM_MAX_NUM_OF_ASYNC_CB);
                memset(&(mcm_handle_map[iter].handle_info), 0, sizeof(mcm_handle_map[iter].handle_info));
                break;
            }
        }
        if (iter == MCM_MAX_CLIENTS)
        {
            LOG_MSG_INFO("mcm_client_internal_update_cb_info: slot exhaused..");
            ret_val = MCM_ERROR_GENERIC_V01;
        }
    }
    MCM_CLIENT_INTERNAL_MUTEX_UNLOCK();
    return ret_val;
}

uint32_t mcm_client_internal_update_async_cb_info
(
    mcm_client_handle_type    handle,
    mcm_client_async_cb       async_resp_cb,
    int                      *clnt_token
)
{
    int ret_val = MCM_SUCCESS_V01;
    int iter = 0;
    int iter_async =0;
    int async_token_len;
    mcm_async_token_type *async_token_ptr = NULL;

    MCM_CLIENT_INTERNAL_MUTEX_LOCK();
    do
    {
        for (iter = 0; iter < MCM_MAX_CLIENTS; iter ++)
        {
            if (mcm_handle_map[iter].mcm_handle == handle)
            {
                LOG_MSG_INFO("mcm_client_internal_update_async_cb_info: found slot %d with mcm handle - %d", iter, handle);
                break;
            }
        }
        if (iter == MCM_MAX_CLIENTS)
        {
            ret_val = MCM_ERROR_INVALID_HANDLE_V01;
            break;
        }

        for (iter_async = 0; iter_async < MCM_MAX_NUM_OF_ASYNC_CB; iter_async ++)
        {
            if ( mcm_handle_map[iter].async_token[iter_async].token_id == MCM_TOKEN_ID_INVALID)
            {
                mcm_handle_map[iter].async_token[iter_async].async_cb = async_resp_cb;
                mcm_handle_map[iter].async_token[iter_async].token_id = *clnt_token;
                break;
            }
        }
        if (iter_async == MCM_MAX_NUM_OF_ASYNC_CB)
        {
            ret_val= MCM_ERROR_GENERIC_V01;
            break;
        }

    }while(0);
    MCM_CLIENT_INTERNAL_MUTEX_UNLOCK();

    return ret_val;
}

uint32_t mcm_client_internal_update_qmi_handle
(
    mcm_client_handle_type    mcm_handle,
    int                       service_id,
    qmi_client_type           user_handle
)
{
    int iter;
    int ret_val = MCM_SUCCESS_V01;

    MCM_CLIENT_INTERNAL_MUTEX_LOCK();
    for (iter = 0; iter < MCM_MAX_CLIENTS; iter++)
    {
        if (mcm_handle_map[iter].mcm_handle == mcm_handle)
        {
            LOG_MSG_INFO("mcm_client_internal_update_qmi_handle: found slot %d with mcm handle - %d", iter, mcm_handle);
            if( MCM_RIL_SERVICE == service_id)
            {
                mcm_handle_map[iter].handle_info.mcm_ril_handle = user_handle;
                break;
            }
            else if (MCM_DATA_SERVICE == service_id)
            {
                mcm_handle_map[iter].handle_info.mcm_data_handle = user_handle;
                break;
            }
            else if (MCM_SIM_SERVICE == service_id)
            {
                mcm_handle_map[iter].handle_info.mcm_sim_handle = user_handle;
                break;
            }
            else if (MCM_MOBILEAP_SERVICE == service_id)
            {
                mcm_handle_map[iter].handle_info.mcm_mobileap_handle = user_handle;
                break;
            }
            else if (MCM_LOC_SERVICE == service_id)
            {
                mcm_handle_map[iter].handle_info.mcm_loc_handle = user_handle;
                break;
            }
            else if (MCM_ATCOP_SERVICE == service_id)
            {
                mcm_handle_map[iter].handle_info.mcm_atcop_handle = user_handle;
                break;
            }
            else if (MCM_SRV_MANAGER == service_id)
            {
                mcm_handle_map[iter].handle_info.mcm_srv_mgr_handle = user_handle;
                break;
            }

        }
    }
    if (iter == MCM_MAX_CLIENTS)
    {
        LOG_MSG_INFO("mcm_client_internal_update_qmi_handle: not found mcm_handle EXIT");
        ret_val = MCM_ERROR_GENERIC_V01;
    }
    MCM_CLIENT_INTERNAL_MUTEX_UNLOCK();
    return ret_val;
}

uint32_t mcm_client_internal_get_async_cb_for_mcm_handle
(
    mcm_client_handle_type    mcm_handle,
    int                       token_id,
    mcm_client_async_cb      *async_cb
)
{
    int iter;
    int iter_list;
    uint32_t ret_val = MCM_SUCCESS_V01;

    MCM_CLIENT_INTERNAL_MUTEX_LOCK();
    do
    {
        for (iter = 0; iter < MCM_MAX_CLIENTS; iter ++)
        {
            if (mcm_handle_map[iter].mcm_handle == mcm_handle)
            {
                LOG_MSG_INFO("found iter:%d", iter);
                break;
            }
        }

        if (iter == MCM_MAX_CLIENTS)
        {
            ret_val = MCM_ERROR_GENERIC_V01;
            break;
        }

        for (iter_list = 0; iter_list < MCM_MAX_NUM_OF_ASYNC_CB; iter_list ++)
        {
            if (mcm_handle_map[iter].async_token[iter_list].token_id == token_id)
            {
                if ( NULL == mcm_handle_map[iter].async_token[iter_list].async_cb)
                {
                    *async_cb = mcm_handle_map[iter].default_async_cb;
                }
                else
                {
                    *async_cb = mcm_handle_map[iter].async_token[iter_list].async_cb;
                }
                mcm_handle_map[iter].async_token[iter_list].token_id = MCM_TOKEN_ID_INVALID;
                break;
            }
        }
        if (MCM_MAX_NUM_OF_ASYNC_CB == iter_list)
        {
            ret_val = MCM_ERROR_INVALID_ARG_V01;
            break;
        }
    }while(0);
    MCM_CLIENT_INTERNAL_MUTEX_UNLOCK();

    return ret_val;
}

uint32_t mcm_client_internal_get_ind_cb_for_mcm_handle
(
    mcm_client_handle_type    mcm_handle,
    mcm_client_ind_cb        *ind_cb
)
{
    int iter = 0;
    uint32_t ret_val = MCM_SUCCESS_V01;

    MCM_CLIENT_INTERNAL_MUTEX_LOCK();
    do
    {
        for(iter = 0; iter < MCM_MAX_CLIENTS; iter ++)
        {
            if (mcm_handle_map[iter].mcm_handle == mcm_handle)
            {
                LOG_MSG_INFO("found iter:%d", iter);
                break;
            }
        }

        if (iter == MCM_MAX_CLIENTS)
        {
            LOG_MSG_INFO("NOT FOUND");
            ret_val = MCM_ERROR_INVALID_HANDLE_V01;
            break;
        }

        *ind_cb = mcm_handle_map[iter].ind_cb;
    }while(0);
    MCM_CLIENT_INTERNAL_MUTEX_UNLOCK();

    return ret_val;
}

uint32_t mcm_internal_get_srv_id_for_msg_id
(
    int msg_id,
    int *srv_id
)
{
    int iter;
    uint32_t ret_val = MCM_SUCCESS_V01;

    LOG_MSG_INFO("alpha MAX: %x",MCM_RIL_DMS_SERVICE_MAX);
    int tbl_size = sizeof(mcm_msg_srv_id_map_tbl)/sizeof(mcm_msg_service_id_map);
    for (iter = 0; iter < tbl_size; iter ++)
    {
        LOG_MSG_INFO("*** msg_id: %x | MIN: %x | MAX: %x",
                      msg_id,
                      mcm_msg_srv_id_map_tbl[iter].mcm_msg_id_min,
                      mcm_msg_srv_id_map_tbl[iter].mcm_msg_id_max);
        if (msg_id >= mcm_msg_srv_id_map_tbl[iter].mcm_msg_id_min &&
        msg_id <= mcm_msg_srv_id_map_tbl[iter].mcm_msg_id_max)
        {
            LOG_MSG_INFO("found service_id iter:%d", iter);
            break;
        }
    }
    if (iter == tbl_size)
    {
        LOG_MSG_INFO("No service found");
        ret_val = MCM_ERROR_GENERIC_V01;
    }

    if(ret_val == MCM_SUCCESS_V01)
    {
        *srv_id = mcm_msg_srv_id_map_tbl[iter].mcm_service_id;
    }

    return ret_val;

}
//=============================================================================
// FUNCTION: mcm_client_internal_get_qmi_handle_for_mcm_handle <fd> <out:token>
//
// DESCRIPTION:
// get user handle from fd.
//
// @ingroup
//
// @return
//
//=============================================================================
uint32_t mcm_client_internal_get_qmi_handle_for_mcm_handle
(
    mcm_client_handle_type     handle,
    int                        msg_id,
    qmi_client_type           *mcm_client_handle
)
{
    int iter;
    int ret_val = MCM_SUCCESS_V01;
    int reti_val = MCM_SUCCESS_V01;
    int srv_id;
    int clnt_found = FALSE;

    mcm_internal_get_srv_id_for_msg_id(msg_id, &srv_id);

    MCM_CLIENT_INTERNAL_MUTEX_LOCK();
    for(iter = 0; iter < MCM_MAX_CLIENTS; iter ++)
    {
        if (mcm_handle_map[iter].mcm_handle == handle)
        {
            switch (srv_id)
            {
                case MCM_RIL_SERVICE:
                    *mcm_client_handle = mcm_handle_map[iter].handle_info.mcm_ril_handle;
                    clnt_found = TRUE;
                    break;
                case MCM_DATA_SERVICE:
                    *mcm_client_handle = mcm_handle_map[iter].handle_info.mcm_data_handle;
                    clnt_found = TRUE;
                    break;
                case MCM_SIM_SERVICE:
                    *mcm_client_handle = mcm_handle_map[iter].handle_info.mcm_sim_handle;
                    clnt_found = TRUE;
                    break;
                case MCM_MOBILEAP_SERVICE:
                    *mcm_client_handle = mcm_handle_map[iter].handle_info.mcm_mobileap_handle;
                    clnt_found = TRUE;
                    break;
                case MCM_LOC_SERVICE:
                    *mcm_client_handle = mcm_handle_map[iter].handle_info.mcm_loc_handle;
                    clnt_found = TRUE;
                    break;
                case MCM_ATCOP_SERVICE:
                    *mcm_client_handle = mcm_handle_map[iter].handle_info.mcm_atcop_handle;
                    clnt_found = TRUE;
                    break;
                case MCM_SRV_MANAGER:
                    *mcm_client_handle = mcm_handle_map[iter].handle_info.mcm_srv_mgr_handle;
                    clnt_found = TRUE;
                    break;
                default:
                    reti_val = MCM_ERROR_GENERIC_V01;
                    break;
            }/* switch */
        }/* if */
        if (TRUE == clnt_found || MCM_SUCCESS_V01 != reti_val)
        {
            /* get out of for loop */
            break;
        }
        /* continue with for loop */

    }/* for */
    MCM_CLIENT_INTERNAL_MUTEX_UNLOCK();

    if (FALSE == clnt_found || MCM_SUCCESS_V01 != reti_val)
    {
        ret_val = MCM_ERROR_INVALID_HANDLE_V01;
    }

    return ret_val;
}

//=============================================================================
// FUNCTION: mcm_client_internal_get_mcm_handle_for_qmi_handle
//
// DESCRIPTION:
// get fd from user handle
//
// @ingroup
//
// @return
//
//=============================================================================

uint32_t mcm_client_internal_get_mcm_handle_for_qmi_handle
(
    qmi_client_type          mcm_client_handle,
    mcm_client_handle_type  *handle
)
{

    int iter = 0;
    int ret_val = MCM_ERROR_GENERIC_V01;

    MCM_CLIENT_INTERNAL_MUTEX_LOCK();
    for(iter = 0; iter < MCM_MAX_CLIENTS; iter++)
    {
        if ( mcm_handle_map[iter].mcm_handle != 0 )
        {
            if ( mcm_handle_map[iter].handle_info.mcm_ril_handle == mcm_client_handle )
            {
                *handle = mcm_handle_map[iter].mcm_handle;
                ret_val = MCM_SUCCESS_V01;
                break;
            }
            else if ( mcm_handle_map[iter].handle_info.mcm_data_handle == mcm_client_handle )
            {
                *handle = mcm_handle_map[iter].mcm_handle;
                ret_val = MCM_SUCCESS_V01;
                break;
            }
            else if ( mcm_handle_map[iter].handle_info.mcm_loc_handle == mcm_client_handle )
            {
                *handle = mcm_handle_map[iter].mcm_handle;
                ret_val = MCM_SUCCESS_V01;
                break;
            }
            else if ( mcm_handle_map[iter].handle_info.mcm_sim_handle == mcm_client_handle )
            {
                *handle = mcm_handle_map[iter].mcm_handle;
                ret_val = MCM_SUCCESS_V01;
                break;
            }
            else if ( mcm_handle_map[iter].handle_info.mcm_mobileap_handle== mcm_client_handle )
            {
                *handle = mcm_handle_map[iter].mcm_handle;
                ret_val = MCM_SUCCESS_V01;
                break;
            }
            else if ( mcm_handle_map[iter].handle_info.mcm_mm_handle == mcm_client_handle )
            {
                *handle = mcm_handle_map[iter].mcm_handle;
                ret_val = MCM_SUCCESS_V01;
                break;
            }
            else if ( mcm_handle_map[iter].handle_info.mcm_atcop_handle == mcm_client_handle )
            {
                *handle = mcm_handle_map[iter].mcm_handle;
                ret_val = MCM_SUCCESS_V01;
                break;
            }
            else if ( mcm_handle_map[iter].handle_info.mcm_srv_mgr_handle == mcm_client_handle )
            {
                *handle = mcm_handle_map[iter].mcm_handle;
                ret_val = MCM_SUCCESS_V01;
                break;
            }
        }
    }
    MCM_CLIENT_INTERNAL_MUTEX_UNLOCK();

    return ret_val;

}

//=============================================================================
// FUNCTION: mcm_internal_client_release
//
// DESCRIPTION:
// get fd from user handle
//
// @ingroup
//
// @return
//
//=============================================================================


uint32_t mcm_internal_client_release
(
    mcm_client_handle_type handle
)
{
    int iter = 0;

    uint32_t ret_val = MCM_SUCCESS_V01;

    MCM_CLIENT_INTERNAL_MUTEX_LOCK();
    for(iter=0; iter<MCM_MAX_CLIENTS; iter++)
    {
        if ( mcm_handle_map[iter].mcm_handle == handle )
        {
            if ( mcm_handle_map[iter].handle_info.mcm_ril_handle != NULL )
            {
                mcm_qmi_client_release(mcm_handle_map[iter].handle_info.mcm_ril_handle);
                mcm_handle_map[iter].handle_info.mcm_ril_handle = NULL;
            }
            if( mcm_handle_map[iter].handle_info.mcm_data_handle != NULL )
            {
                mcm_qmi_client_release(mcm_handle_map[iter].handle_info.mcm_data_handle);
                mcm_handle_map[iter].handle_info.mcm_data_handle = NULL;
            }

            if ( mcm_handle_map[iter].handle_info.mcm_mobileap_handle != NULL )
            {
                mcm_qmi_client_release(mcm_handle_map[iter].handle_info.mcm_mobileap_handle);
                mcm_handle_map[iter].handle_info.mcm_mobileap_handle = NULL;
            }
            if ( mcm_handle_map[iter].handle_info.mcm_sim_handle != NULL )
            {
                mcm_qmi_client_release(mcm_handle_map[iter].handle_info.mcm_sim_handle);
                mcm_handle_map[iter].handle_info.mcm_sim_handle = NULL;
            }
            if (mcm_handle_map[iter].handle_info.mcm_loc_handle != NULL)
            {
                mcm_qmi_client_release(mcm_handle_map[iter].handle_info.mcm_loc_handle);
                mcm_handle_map[iter].handle_info.mcm_loc_handle = NULL;
            }
            if (mcm_handle_map[iter].handle_info.mcm_atcop_handle != NULL)
            {
                mcm_qmi_client_release(mcm_handle_map[iter].handle_info.mcm_atcop_handle);
                mcm_handle_map[iter].handle_info.mcm_atcop_handle = NULL;
            }
            // free handle itself.
            mcm_handle_map[iter].mcm_handle = NIL;
            break;
        }
    }
    MCM_CLIENT_INTERNAL_MUTEX_UNLOCK();

    if (MCM_MAX_CLIENTS == iter)
    {
        ret_val = MCM_ERROR_GENERIC_V01;
    }
    return ret_val;
}
uint32_t mcm_client_internal_get_qmi_handles_for_mcm_handle
(
    mcm_client_handle_type mcm_handle,
    client_handle_info_type *qmi_handles_ptr
)
{
    int iter;
    int iter_clnt;
    int ret_val = MCM_SUCCESS_V01;
    int reti = MCM_SUCCESS_V01;
    int srv_id;

    //mcm_internal_get_srv_id_for_msg_id(msg_id, &srv_id);
    for(iter_clnt = 0; iter_clnt < MCM_MAX_CLIENTS; iter_clnt ++)
    {
        if (mcm_handle_map[iter_clnt].mcm_handle == mcm_handle)
        {
            qmi_handles_ptr->mcm_atcop_handle = mcm_handle_map[iter_clnt].handle_info.mcm_atcop_handle;
            qmi_handles_ptr->mcm_data_handle = mcm_handle_map[iter_clnt].handle_info.mcm_data_handle;
            qmi_handles_ptr->mcm_loc_handle = mcm_handle_map[iter_clnt].handle_info.mcm_loc_handle;
            qmi_handles_ptr->mcm_mobileap_handle = mcm_handle_map[iter_clnt].handle_info.mcm_mobileap_handle;
            qmi_handles_ptr->mcm_sim_handle = mcm_handle_map[iter_clnt].handle_info.mcm_sim_handle;
            break;
        }
    }
    if (iter_clnt == MCM_MAX_CLIENTS)
    {
        ret_val = MCM_ERROR_GENERIC_V01;
    }
    return ret_val;
}

void mcm_update_ssr_status(uint8_t ssr_status)
{
    mcm_ssr_status = ssr_status;
}

uint8_t mcm_is_device_in_ssr(void)
{
    return mcm_ssr_status;
}

uint8_t mcm_is_msg_for_mobileap_service(int msg_id)
{
    uint8_t status = FALSE;

    if ((msg_id >= mcm_msg_srv_id_map_tbl[MCM_MOBILEAP_SERVICE_INDEX].mcm_msg_id_min) &&
        (msg_id <= mcm_msg_srv_id_map_tbl[MCM_MOBILEAP_SERVICE_INDEX].mcm_msg_id_max))
    {
        status = TRUE;
    }

    return status;
}
