
/*************************************************************************************

 Copyright (c) 2013-2014, 2017, 2018 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

**************************************************************************************/

#include "mcm_utils.h"
#include "mcm_dm_v01.h"
#include "mcm_nw_v01.h"
#include "mcm_sms_v01.h"
#include "mcm_voice_v01.h"
#include "mcm_mobileap_v01.h"
#include "mcm_data_v01.h"
#include "mcm_loc_v01.h"
#include "mcm_constants.h"
#include "mcm_client.h"
#include "mcm_service_object_v01.h"
#include "mcm_client_internal.h"
#include "mcm_sim_v01.h"
#include "mcm_client_v01.h"

#define MCM_QMI_CLIENT_MUTEX_UNLOCK()     pthread_mutex_unlock(&qmi_client_mutex)
#define MCM_QMI_CLIENT_MUTEX_LOCK()                           \
{                                                             \
    int ret;                                                  \
    ret = pthread_mutex_lock(&qmi_client_mutex);              \
    if(EOWNERDEAD == ret)                                     \
    {                                                         \
        pthread_mutex_consistent(&qmi_client_mutex);          \
    }                                                         \
}

pthread_mutex_t qmi_client_mutex;
pthread_mutexattr_t qmi_client_mutexattr;

uint32 mcm_qmi_client_mutex_init()
{
    int ret_val = MCM_SUCCESS_V01;

    LOG_MSG_INFO("mcm_qmi_client_mutex_init Enter");

    do
    {
        ret_val = pthread_mutexattr_init(&qmi_client_mutexattr);
        if(ret_val)
        {
            LOG_MSG_INFO("QMI Client Mutex Attribute Initialization error - %d, %s",ret_val, strerror(ret_val));
            ret_val = MCM_ERROR_GENERIC_V01;
            break;
        }

        ret_val = pthread_mutexattr_setrobust(&qmi_client_mutexattr, PTHREAD_MUTEX_ROBUST);
        if(ret_val)
        {
            LOG_MSG_INFO("QMI Client Mutex Set robust error - %d, %s",ret_val, strerror(ret_val));
            ret_val = MCM_ERROR_GENERIC_V01;
            break;
        }

        ret_val = pthread_mutex_init(&qmi_client_mutex, &qmi_client_mutexattr);
        if(ret_val)
        {
            LOG_MSG_INFO("QMI Client Mutex Init error - %d, %s",ret_val, strerror(ret_val));
            ret_val = MCM_ERROR_GENERIC_V01;
            break;
        }
    }while(FALSE);

    LOG_MSG_INFO("mcm_qmi_client_mutex_init Exit with ret: %d", ret_val);

    return ret_val;
}

void mcm_qmi_ind_cb
(
    qmi_client_type  user_handle,
    unsigned int      msg_id,
    void             *ind_buf,
    unsigned int      ind_buf_len,
    void             *resp_cb_data
)
{
    mcm_client_ind_cb                ind_cb = NULL;
    qmi_client_error_type            qmi_err;
    void                            *ind_data = NULL;
    int                              ind_data_len = 0;
    mcm_client_handle_type           mcm_clnt_hndl = 0;
    uint32                           ret_val = MCM_SUCCESS_V01;
    mcm_dm_radio_mode_changed_event_ind_msg_v01 *mcm_radio_status = NULL;

    LOG_MSG_INFO("mcm_qmi_ind_cb ENTER msg_id:%x", msg_id);

    do
    {
        /* get mcm_client_handle */
        if(MCM_SUCCESS_V01 !=
            (ret_val = mcm_client_internal_get_mcm_handle_for_qmi_handle(user_handle, &mcm_clnt_hndl)))
        {
            ret_val = MCM_ERROR_GENERAL_V01;
            break;
        }
        /* get indication_cb */
        if(MCM_SUCCESS_V01 !=
            (ret_val = mcm_client_internal_get_ind_cb_for_mcm_handle(mcm_clnt_hndl, &ind_cb)))
        {
            ret_val = MCM_ERROR_GENERAL_V01;
            break;
        }

        if (NULL == ind_cb )
        {
            LOG_MSG_INFO("NULL ind_cb");
            ret_val = MCM_ERROR_GENERAL_V01;
            break;
        }

        if (NULL == ind_buf ||
            0 == ind_buf_len)
        {
            /* zero payload, no process for decoding break out */
            LOG_MSG_INFO("Indication has no payload");
            ret_val = MCM_ERROR_GENERAL_V01;
            break;
        }
        switch (msg_id)
        {
            // RIL - DM
            case MCM_DM_RADIO_MODE_CHANGED_EVENT_IND_V01:
                ind_data_len = sizeof(mcm_dm_radio_mode_changed_event_ind_msg_v01);
                break;

            // RIL - SMS
            case MCM_SMS_PP_IND_V01:
                ind_data_len = sizeof(mcm_sms_pp_ind_msg_v01);
                break;
            case MCM_SMS_CB_IND_V01:
                ind_data_len = sizeof(mcm_sms_cb_ind_msg_v01);
                break;
            case MCM_SMS_CB_CMAS_IND_V01:
                ind_data_len = sizeof(mcm_sms_cb_cmas_ind_msg_v01);
                break;

            // RIL - NW
            case MCM_NW_VOICE_REGISTRATION_EVENT_IND_V01:
                ind_data_len = sizeof(mcm_nw_voice_registration_event_ind_msg_v01);
                break;
            case MCM_NW_DATA_REGISTRATION_EVENT_IND_V01:
                ind_data_len = sizeof(mcm_nw_data_registration_event_ind_msg_v01);
                break;
            case MCM_NW_SIGNAL_STRENGTH_EVENT_IND_V01:
                ind_data_len = sizeof(mcm_nw_signal_strength_event_ind_msg_v01);
                break;
            case MCM_NW_CELL_ACCESS_STATE_CHANGE_EVENT_IND_V01:
                ind_data_len = sizeof(mcm_nw_cell_access_state_change_event_ind_msg_v01);
                break;
            case MCM_NW_NITZ_TIME_IND_V01:
                ind_data_len = sizeof(mcm_nw_nitz_time_ind_msg_v01);
                break;

            // RIL - voice
            case MCM_VOICE_CALL_IND_V01:
                ind_data_len = sizeof(mcm_voice_call_ind_msg_v01);
                break;
            case MCM_VOICE_MUTE_IND_V01:
                ind_data_len = sizeof(mcm_voice_mute_ind_msg_v01);
                break;
            case MCM_VOICE_E911_STATE_IND_V01:
                ind_data_len = sizeof(mcm_voice_e911_state_ind_msg_v01);
                break;

            // mobileap
            case MCM_MOBILEAP_UNSOL_EVENT_IND_V01:
                ind_data_len = sizeof(mcm_mobileap_unsol_event_ind_msg_v01);
                break;

            // Data
            case MCM_DATA_UNSOL_EVENT_IND_V01:
                ind_data_len = sizeof(mcm_data_unsol_event_ind_msg_v01);
                break;

            // UIM
            case MCM_SIM_CARD_STATUS_EVENT_IND_V01:
                ind_data_len = sizeof(mcm_sim_card_status_event_ind_msg_v01);
                break;
            case MCM_SIM_REFRESH_EVENT_IND_V01:
                ind_data_len = sizeof(mcm_sim_refresh_event_ind_msg_v01);
                break;

            // LOC
            case MCM_LOC_LOCATION_INFO_IND_V01:
                ind_data_len = sizeof(mcm_loc_location_info_ind_msg_v01);
                break;

            case MCM_LOC_STATUS_INFO_IND_V01:
                ind_data_len = sizeof(mcm_loc_status_info_ind_msg_v01);
                break;

            case MCM_LOC_SV_INFO_IND_V01:
                ind_data_len = sizeof(mcm_loc_sv_info_ind_msg_v01);
                break;

            case MCM_LOC_NMEA_INFO_IND_V01:
                ind_data_len = sizeof(mcm_loc_nmea_info_ind_msg_v01);
                break;

            case MCM_LOC_CAPABILITIES_INFO_IND_V01:
                ind_data_len = sizeof(mcm_loc_capabilities_info_ind_msg_v01);
                break;

            case MCM_LOC_UTC_TIME_REQ_IND_V01:
                ind_data_len = sizeof(mcm_loc_utc_time_req_ind_msg_v01);
                break;

            case MCM_LOC_XTRA_DATA_REQ_IND_V01:
                ind_data_len = sizeof(mcm_loc_xtra_data_req_ind_msg_v01);
                break;

            case MCM_LOC_AGPS_STATUS_IND_V01:
                ind_data_len = sizeof(mcm_loc_agps_status_ind_msg_v01);
                break;

            case MCM_LOC_NI_NOTIFICATION_IND_V01:
                ind_data_len = sizeof(mcm_loc_ni_notification_ind_msg_v01);
                break;

            case MCM_LOC_XTRA_REPORT_SERVER_IND_V01:
                ind_data_len = sizeof(mcm_loc_xtra_report_server_ind_msg_v01);
                break;

            default:
                ret_val = MCM_ERROR_GENERIC_V01;
                break;
        }
        if (MCM_SUCCESS_V01 != ret_val)
        {
            break;
        }

#ifdef MULTI_PROCESS_CONFIG

        if (NULL == (ind_data = (void *)mcm_util_memory_alloc(ind_data_len)))
        {
            ret_val = MCM_ERROR_GENERAL_V01;
            break;
        }
        qmi_err = qmi_client_message_decode(user_handle,
                                            QMI_IDL_INDICATION,
                                            msg_id,
                                            ind_buf,
                                            ind_buf_len,
                                            ind_data,
                                            ind_data_len);
         /* TODO check qmi_err, if QMI_ERROR_X_FAIL break */
        LOG_MSG_INFO("decode qmi_error:%d", qmi_err);
        if (MCM_SUCCESS_V01 != (ret_val = convert_qmi_err_to_mcm(qmi_err)))
        {
            break;
        }

#else // SINGLE_PROCESS_CONFIG
                memcpy(&ind_data,ind_buf,sizeof(ind_data));
#endif


    }while(0);

    if ( ret_val == MCM_SUCCESS_V01 ||
         (ret_val == MCM_ERROR_GENERAL_V01 &&
         (msg_id == MCM_LOC_UTC_TIME_REQ_IND_V01 ||
          msg_id == MCM_LOC_XTRA_DATA_REQ_IND_V01)) )
    {
        if ( NULL != ind_cb )
        {
            (*ind_cb)(mcm_clnt_hndl, msg_id, ind_data, ind_data_len);
        }
    }

    if(msg_id == MCM_DM_RADIO_MODE_CHANGED_EVENT_IND_V01)
    {
        mcm_radio_status = (mcm_dm_radio_mode_changed_event_ind_msg_v01*)ind_data;
        if((NULL != mcm_radio_status) && (mcm_radio_status->radio_mode_valid))
        {
            if(MCM_DM_RADIO_MODE_UNAVAILABLE_V01 == mcm_radio_status->radio_mode)
            {
                mcm_update_ssr_status(TRUE);
            }
            else
            {
                mcm_update_ssr_status(FALSE);
            }
        }
    }

    if (NULL != ind_data)
    {
        mcm_util_memory_free(&ind_data);
    }

}
void mcm_query_qmi_handle(mcm_client_handle_type mcm_clnt_hndl)
{
   qmi_client_type                    n_user_handle;
   client_handle_info_type            qmi_clients;

   MCM_QMI_CLIENT_MUTEX_LOCK();

   mcm_client_internal_get_qmi_handles_for_mcm_handle(mcm_clnt_hndl, &qmi_clients);

   n_user_handle = mcm_atcop_init();
   if ( NULL != n_user_handle && NULL != qmi_clients.mcm_atcop_handle)
   {
       /* service exists already, release new queried handle */
       LOG_MSG_INFO("service exists already");
       qmi_client_release(n_user_handle);
   }
   else
   {
       /* update with new queried handle */
       LOG_MSG_INFO("qmi_handle for ATCOP:%x", n_user_handle);
       mcm_client_internal_update_qmi_handle(mcm_clnt_hndl, MCM_ATCOP_SERVICE, n_user_handle);
   }

   n_user_handle = mcm_data_init();
   if(NULL != n_user_handle && NULL != qmi_clients.mcm_data_handle)
   {
       /* service exists already, release new queried handle */
       LOG_MSG_INFO("service exists already");
       qmi_client_release(n_user_handle);
   }
   else
   {
       /* update with new queried handle */
       LOG_MSG_INFO("qmi_handle for DATA:%x", n_user_handle);
       mcm_client_internal_update_qmi_handle(mcm_clnt_hndl, MCM_DATA_SERVICE, n_user_handle);
   }

   n_user_handle = mcm_loc_init();
   if( NULL != n_user_handle && NULL != qmi_clients.mcm_loc_handle)
   {
       /* service exists already, release new queried handle */
       LOG_MSG_INFO("service exists already");
       qmi_client_release(n_user_handle);
   }
   else
   {
       /* update with new queried handle */
       LOG_MSG_INFO("qmi_handle for LOC:%x", n_user_handle);
       mcm_client_internal_update_qmi_handle(mcm_clnt_hndl, MCM_LOC_SERVICE, n_user_handle);
   }

   n_user_handle = mcm_mobileap_init();
   if( NULL != n_user_handle && NULL != qmi_clients.mcm_mobileap_handle)
   {
       /* service exists already, release new queried handle */
       LOG_MSG_INFO("service exists already");
       qmi_client_release(n_user_handle);
   }
   else
   {
       /* update with new queried handle */
       LOG_MSG_INFO("qmi_handle for MOBILEAP:%x", n_user_handle);
       mcm_client_internal_update_qmi_handle(mcm_clnt_hndl, MCM_MOBILEAP_SERVICE, n_user_handle);
   }

   n_user_handle = mcm_sim_init();
   if( NULL != n_user_handle && NULL != qmi_clients.mcm_sim_handle)
   {
       /* service exists already, release new queried handle */
       LOG_MSG_INFO("service exists already");
       qmi_client_release(n_user_handle);
   }
   else
   {
       /* update with new queried handle */
       LOG_MSG_INFO("qmi_handle for MOBILEAP:%x", n_user_handle);
       mcm_client_internal_update_qmi_handle(mcm_clnt_hndl, MCM_SIM_SERVICE, n_user_handle);
   }

   client_handle_info_type qmi_handles;
   memset(&qmi_handles, 0, sizeof(client_handle_info_type));

   mcm_client_internal_get_qmi_handles_for_mcm_handle(mcm_clnt_hndl, &qmi_handles);
   LOG_MSG_INFO("ril_hndl:%d data_hndl:%d atcop_hndl:%d, mobileap_hndl:%d sim_hndl:%d loc_hndl:%d",
          qmi_handles.mcm_ril_handle,
          qmi_handles.mcm_data_handle,
          qmi_handles.mcm_atcop_handle,
          qmi_handles.mcm_mobileap_handle,
          qmi_handles.mcm_sim_handle,
          qmi_handles.mcm_loc_handle);
   MCM_QMI_CLIENT_MUTEX_UNLOCK();

}
void mcm_qmi_async_cb
(
    qmi_client_type                user_handle,
    unsigned int                   msg_id,
    void                           *resp_c_struct,
    unsigned int                   resp_c_struct_len,
    void                           *resp_cb_data,
    qmi_client_error_type          transp_err
)
{
    mcm_client_handle_type   mcm_clnt_hndl = 0;
    uint32                   ret_val       = MCM_ERROR_GENERIC_V01;
    int                     *token_id_ptr  = NULL;
    mcm_client_async_cb      async_cb      = NULL;

    LOG_MSG_INFO("mcm_qmi_async_cb ENTER msg_id:%d transp_err:%d",
                 msg_id, transp_err);

    do
    {
        /* validate input params */
        if (MCM_SUCCESS_V01 !=
            (ret_val = convert_qmi_err_to_mcm(transp_err)))
        {
            LOG_MSG_INFO("Async callback failed error:%d", ret_val);
            break;
        }
        if (NULL == resp_c_struct ||
            0   == resp_c_struct_len)
        {
            LOG_MSG_INFO("Invalid input EXIT ");
            break;
        }

        token_id_ptr = (int *)resp_cb_data;
        LOG_MSG_INFO("token_id:%d", *token_id_ptr);

        /* get mcm_client_handle */
        if(MCM_SUCCESS_V01 !=
            (ret_val = mcm_client_internal_get_mcm_handle_for_qmi_handle(user_handle, &mcm_clnt_hndl)))
        {
            break;
        }

        /* get async_cb */
        if(MCM_SUCCESS_V01 !=
            (ret_val = mcm_client_internal_get_async_cb_for_mcm_handle(mcm_clnt_hndl, *token_id_ptr, &async_cb)))
        {
            break;
        }

        if (NULL == async_cb)
        {
            LOG_MSG_INFO("NULL callback");
            break;
        }


        if (msg_id == MCM_CLIENT_REQUIRE_REQ_V01 ||
            msg_id == MCM_CLIENT_NOT_REQUIRE_REQ_V01)
        {
            mcm_query_qmi_handle(mcm_clnt_hndl);
        }
        (*async_cb)(mcm_clnt_hndl, msg_id, resp_c_struct, resp_c_struct_len, token_id_ptr);

    }while(0);

    /*Free memory only in error case else memory is freed in callback function*/
    if (MCM_SUCCESS_V01 != ret_val && NULL != resp_c_struct)
    {
      free(resp_c_struct);
      resp_c_struct = NULL;
    }

    if (NULL != token_id_ptr)
    {
      free(token_id_ptr);
      token_id_ptr = NULL;
    }

    return ret_val;
}

qmi_client_error_type mcm_qmi_client_init
(
    qmi_service_info *service_info,
    qmi_idl_service_object_type service_obj,
    qmi_client_ind_cb ind_cb,
    void *ind_cb_data,
    qmi_client_os_params *os_params,
    qmi_client_type *user_handle
)
{

    qmi_client_error_type ret_val;

#ifdef SINGLE_PROCESS_CONFIG
    ret_val = mcm_qmi_single_process_init(service_info,
                                        service_obj,
                                        ind_cb,
                                        ind_cb_data,
                                        os_params,
                                        user_handle);

#else // MULTI_PROCESS_CONFIG

    ret_val = mcm_qmi_multi_process_init(service_info,
                                        service_obj,
                                        ind_cb,
                                        ind_cb_data,
                                        os_params,
                                        user_handle);

    LOG_MSG_INFO("user_handle:%x", *user_handle);

#endif

    ret_val = convert_qmi_err_to_mcm(ret_val);
    return ret_val;

}

qmi_client_error_type mcm_qmi_client_release
(
    qmi_client_type user_handle
)
    {
    qmi_client_error_type ret_val;

#ifdef SINGLE_PROCESS_CONFIG

    ret_val = mcm_qmi_release(user_handle);

#else //MULTI_PROCESS_CONFIG

    ret_val = qmi_client_release(user_handle);

#endif

    ret_val = convert_qmi_err_to_mcm(ret_val);
    return ret_val;

}



qmi_client_error_type server_execute_sync
(
    qmi_client_type     user_handle,
    unsigned int        msg_id,
    void                *req_c_struct,
    unsigned int        req_c_struct_len,
    void                *resp_c_struct,
    unsigned int        resp_c_struct_len,
    unsigned int        timeout_msecs
)
{
    qmi_client_error_type ret_val;
    LOG_MSG_INFO("server_execute_sync Enter");

#ifdef SINGLE_PROCESS_CONFIG
    void *temp_resp;

    ret_val = mcm_qmi_send_msg_sync( user_handle,
                                    msg_id,
                                    req_c_struct,
                                    req_c_struct_len,
                                    &temp_resp,
                                    resp_c_struct_len,
                                    timeout_msecs);
    memcpy(resp_c_struct,temp_resp,resp_c_struct_len);

#else
    LOG_MSG_INFO("user_handle:%x, msg_id:%x, req_c_struct:%x, req_c_struct_len:%x, resp_c_struct:%x, resp_c_struct_len:%x",
    user_handle, msg_id, req_c_struct, req_c_struct_len, resp_c_struct, resp_c_struct_len);
    ret_val = qmi_client_send_msg_sync( user_handle,
                                        msg_id,
                                        req_c_struct,
                                        req_c_struct_len,
                                        resp_c_struct,
                                        resp_c_struct_len,
                                        timeout_msecs);
    qmi_util_log("ret_val:%d", ret_val);
#endif

    return ret_val;

}


qmi_client_error_type server_execute_async
(
    qmi_client_type user_handle,
    unsigned int msg_id,
    void *req_c_struct,
    unsigned int req_c_struct_len,
    void *resp_c_struct,
    unsigned int resp_c_struct_len,
    qmi_client_recv_msg_async_cb resp_cb,
    void *resp_cb_data,
    qmi_txn_handle *txn_handle
)
{

    qmi_client_error_type ret_val;
    LOG_MSG_INFO("server_execute_async Enter");

#ifdef SINGLE_PROCESS_CONFIG

    ret_val = mcm_qmi_send_msg_async(user_handle,
                                    msg_id,
                                    req_c_struct,
                                    req_c_struct_len,
                                    resp_c_struct,
                                    resp_c_struct_len,
                                    resp_cb,
                                    resp_cb_data,
                                    txn_handle);


#else

    ret_val = qmi_client_send_msg_async(user_handle,
                                        msg_id,
                                        req_c_struct,
                                        req_c_struct_len,
                                        resp_c_struct,
                                        resp_c_struct_len,
                                        resp_cb,
                                        resp_cb_data,
                                        txn_handle);
    qmi_util_log("ret_val:%d", ret_val);
#endif

    return ret_val;

}



