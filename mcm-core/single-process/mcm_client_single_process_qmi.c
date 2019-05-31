
/*************************************************************************************
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
**************************************************************************************/

#include "mcm_msg_queue.h"
#include "mcm_msg_receiver.h"
#include "mcm_dm_v01.h"
#include "mcm_nw_v01.h"
#include "mcm_service_object_v01.h"
#include "mcm_client_single_process_qmi.h"
#include "mcm_srv_internal.h"
#include "hlos_core.h"

mcm_msg_queue *mcm_msg_response_msg_queue;
mcm_signal_data_type signal_response_queue;

mcm_ind_cb_map ind_map[MCM_MAX_CLIENTS];

mcm_req_handle_map request_map[MCM_MAX_REQUESTS];

mcm_async_cb_map async_map[MCM_MAX_REQUESTS];

int mcm_qmi_client_handle = 1;
int mcm_req_handle = 1;
qmi_client_type mcm_dm_clnt_hndl;
qmi_client_type mcm_dm_clnt_notifier;

int init_done = 0;

int mcm_internal_wait_event(int eventid, void** event)
{
    int ret_val = MCM_SUCCESS_V01;
    qmi_util_log("mcm_internal_wait_event Enter\n");

    *event = mcm_msg_queue_dequeue(mcm_msg_response_msg_queue);
    while ( *event == NULL )
    {
        qmi_util_log("mcm_internal_wait_event : No events with event id - %x are currently in queue..\n waiting for event\n", eventid);
        MCM_WAIT_FOR_SIGNAL(signal_response_queue);
        *event = mcm_msg_queue_dequeue(mcm_msg_response_msg_queue);
    }
    return ret_val;
}

void mcm_srv_connect_cb_dummy(void *client_handle,void *mcm_srv_client_hdl,void **connection_handle)
{
    if ( init_done == 0 )
    {
        mcm_srv_qmi_client_init();
        init_done = 1;
    }
    mcm_srv_client_hdl_info *ptr=(mcm_srv_client_hdl_info*)mcm_srv_client_hdl;

    // hack to avoid going to server and getting information.
    *connection_handle=client_handle;
}


qmi_client_error_type mcm_qmi_single_process_init
(
    qmi_service_info                *service_info,
    qmi_idl_service_object_type     service_obj,
    qmi_client_ind_cb               ind_cb,
    void                            *ind_cb_data,
    qmi_client_os_params            *os_params,
    qmi_client_type                 *user_handle
)

{
    int iter=0;
    void* client_handle = mcm_util_memory_alloc(sizeof(int));
    void* connection_handle;

    mcm_srv_connect_cb_dummy(client_handle,mcm_srv_client_hdl,&connection_handle);
    *user_handle = client_handle;

    // ideally this should be called in connect_cb in multi process configuration
    mcm_srv_client_registered(MCM_RIL_SERVICE,client_handle);

    for(iter = 0; iter < MCM_MAX_CLIENTS; iter++ )
    {
        if ( ind_map[iter].ind_cb == NULL )
        {
            ind_map[iter].client_handle = client_handle;
            ind_map[iter].ind_cb = ind_cb;
            break;
        }
    }

    mcm_msg_response_msg_queue = mcm_msg_queue_new();
    MCM_INIT_SIGNAL(signal_response_queue);

    mcm_start_receiver_thread();
}


int mcm_qmi_release(int mcm_client_handle)
{
    int ret_val = MCM_ERROR_GENERIC_V01;

    if ( mcm_client_handle != NULL )
    {
        mcm_srv_client_registered(MCM_RIL_SERVICE,mcm_client_handle);
        ret_val = MCM_SUCCESS_V01;
    }
    return ret_val;
}




void *mcm_internal_get_client_handle_from_req_handle(int req_handle)
{
    int index = 0;
    qmi_client_type client_handle;
    while( index < MCM_MAX_REQUESTS )
    {
        if ( request_map[index].req_handle == req_handle )
        {
            client_handle = request_map[index].user_handle;
            break;
        }
        index++;
    }
    return client_handle;
}


void mcm_internal_update_sync_map
(
    int req_handle,
    qmi_client_type client_handle,
    int msg_id,
    qmi_client_recv_msg_async_cb resp_cb
)
{
    int index = 0;
    while( index < MCM_MAX_REQUESTS )
    {
        if ( request_map[index].req_handle == NULL )
        {
            request_map[index].is_async = FALSE;
            request_map[index].req_handle = req_handle;
            request_map[index].user_handle = client_handle;
            break;
        }
        index++;
    }
}



void mcm_internal_update_async_map
(
    int req_handle,
    int service_id,
    qmi_client_type client_handle,
    int msg_id,
    void *async_cb_data,
    qmi_client_recv_msg_async_cb resp_cb
)
{
    int index = 0;
    while( index < MCM_MAX_REQUESTS)
    {
        if ( request_map[index].req_handle == NULL )
        {
            request_map[index].async_cb = resp_cb;
            request_map[index].is_async = TRUE;
            request_map[index].req_handle = req_handle;
            request_map[index].user_handle = client_handle;
            request_map[index].service_id = service_id;
            request_map[index].msg_id = msg_id;
            request_map[index].async_cb_data = async_cb_data;
            break;
        }
    index++;
    }
}


int mcm_srv_is_request_async
(
    int req_handle,
    qmi_client_type client_handle
)
{
    int index = 0;
    int ret_val = FALSE;

    for(index = 0; index < MCM_MAX_REQUESTS; index++)
    {
        if ( request_map[index].user_handle == client_handle )
        {
            if ( request_map[index].req_handle == req_handle )
            {
                if ( request_map[index].is_async == TRUE )
                {
                    ret_val = TRUE;
                }
                else
                {
                    ret_val = FALSE;
                }
            break;
            }
        }
    }
    return ret_val;

}


int mcm_qmi_get_service_id_from_client_handle(qmi_client_type client_handle)
{

    int iter_srv=0;
    int iter_clnt = 0;
    int service_id = 0;

    for(iter_srv=0;iter_srv<MCM_MAX_SERVICES;iter_srv++)
    {
        if ( mcm_srv_client_hdl[iter_srv].service_id != 0 )
        {
            for(iter_clnt=0;iter_clnt<MCM_MAX_CLIENTS;iter_clnt++)
            {
                if( mcm_srv_client_hdl[iter_srv].client_handles[iter_clnt].client_handle == client_handle)
                {
                    service_id = mcm_srv_client_hdl[iter_srv].service_id;
                break;
                }
            }
        }
        if ( service_id != 0 )
        {
        break;
        }
    }
    return service_id;

}


int mcm_qmi_send_msg_sync(qmi_client_type client_handle,
                        int message_id,
                        void *req_message,
                        int req_message_len,
                        void **resp_message,
                        int resp_message_len,
                        int time_out)
{

    int service_id = 0;
    int request_hdl;

    service_id = mcm_qmi_get_service_id_from_client_handle(client_handle);

    request_hdl = mcm_req_handle++;

    switch(service_id)
    {
        case MCM_RIL_SERVICE:
            qmi_util_log("client_handle:%x\n", client_handle);
                                                mcm_srv_qmi_handle_request(&client_handle,
                                                request_hdl,
                                                message_id,
                                                req_message,
                                                req_message_len,
                                                &mcm_srv_client_hdl);
        break;

        default:
        break;

    }
    mcm_internal_wait_event(message_id,resp_message);

}

int mcm_qmi_send_msg_async(qmi_client_type client_handle,
                            int message_id,
                            void *req_message,
                            int req_message_len,
                            void *resp_message,
                            int resp_message_len,
                            qmi_client_recv_msg_async_cb resp_cb,
                            void *resp_cb_data,
                            qmi_txn_handle *txn_handle)
{

    int service_id = 0;
    int request_hdl;
    int token_id;
    qmi_util_log("mcm_qmi_send_msg_async ENTER:\n");

    service_id = mcm_qmi_get_service_id_from_client_handle(client_handle);

    request_hdl = mcm_req_handle++;

    mcm_internal_update_async_map(request_hdl,service_id,client_handle,message_id,resp_cb_data,resp_cb);
    qmi_util_log("input client_handle:%x\n", client_handle);

    switch(service_id)
    {
        case MCM_RIL_SERVICE:
            mcm_srv_qmi_handle_request(client_handle,
                                        request_hdl,
                                        message_id,
                                        req_message,
                                        req_message_len,
                                        &mcm_srv_client_hdl);
        break;

        default:
        break;
    }

}


void mcm_client_qmi_ind_cb
(
    qmi_client_type                user_handle,
    unsigned long                  msg_id,
    unsigned char                  *ind_buf,
    int                            ind_buf_len,
    void                           *ind_cb_data
)
{
    qmi_util_log("mcm_client_qmi_ind_cb\n");
}

void mcm_srv_qmi_handle_init_request
(
    int service_id,
    qmi_client_handle client_handle
)
{
    if ( init_done == 0 )
    {
        mcm_srv_qmi_client_init();
        init_done = 1;
    }
    mcm_srv_client_registered(service_id,client_handle);
}


void mcm_srv_qmi_handle_request
(
    void                *clnt_info,
    qmi_req_handle      req_handle,
    int                 msg_id,
    void                *req_c_struct,
    int                 req_c_struct_len,
    void                *service_handle
)
{

    qmi_client_error_type cci_err = QMI_ERR_NONE_V01;
    qmi_util_log("mcm_srv_qmi_handle_request");

    switch(msg_id)
    {
        case MCM_DM_SET_POWER_REQ_V01:
        case MCM_DM_GET_STATUS_REQ_V01:
        case MCM_NW_SET_CONFIG_REQ_V01:
        case MCM_NW_GET_CONFIG_REQ_V01:
        case MCM_NW_EVENT_REGISTER_REQ_V01:
        /*      case MCM_NW_SET_PREFERRED_MODE_REQ_V01:
        case MCM_NW_MANUAL_SELECTION_REQ_V01:
        case MCM_NW_AUTO_SELECTION_REQ_V01:
        case MCM_NW_SCAN_REQ_V01:
        case MCM_NW_GET_OPERATOR_NAME_REQ_V01:
        case MCM_NW_GET_REGISTRATION_STATUS_REQ_V01:
        */
            hlos_core_ril_request_handler(clnt_info, req_handle,msg_id,req_c_struct,req_c_struct_len, service_handle);
        break;

        case MCM_DM_EVENT_REGISTER_REQ_V01:
            mcm_srv_registration_event_request_handler(clnt_info, req_handle,msg_id,req_c_struct,req_c_struct_len, service_handle);
        break;
    }
}





