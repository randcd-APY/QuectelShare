
/*************************************************************************************
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
**************************************************************************************/

#include <pthread.h>

#include "mcm_msg_queue.h"
#include "mcm_constants.h"
#include "mcm_msg_receiver.h"
#include "mcm_single_process.h"
#include "mcm_client_single_process_qmi.h"

extern mcm_req_handle_map request_map[MCM_MAX_REQUESTS];

extern mcm_ind_cb_map ind_map[MCM_MAX_CLIENTS];


mcm_async_cb_map async_map[MCM_MAX_REQUESTS];

mcm_msg_queue *mcm_msg_ind_msg_queue;
mcm_signal_data_type signal_ind_queue;

mcm_msg_queue *mcm_msg_asyncresp_msg_queue;
mcm_signal_data_type signal_asyncresp_queue;


pthread_t ind_receiver_tid = 0;
pthread_t async_receiver_tid = 0;

#define UTIL_ARR_SIZE( arr )  (sizeof( ( arr ) ) / sizeof( ( arr[ 0 ])))

void mcm_start_receiver_thread(void)
{
    mcm_msg_ind_msg_queue = mcm_msg_queue_new();
    MCM_INIT_SIGNAL(signal_ind_queue);
    mcm_msg_asyncresp_msg_queue = mcm_msg_queue_new();
    MCM_INIT_SIGNAL(signal_asyncresp_queue);
    pthread_create(&ind_receiver_tid, NULL, mcm_ind_receiver_thread_func, NULL);
    pthread_create(&async_receiver_tid, NULL, mcm_async_receiver_thread_func, NULL);
}

int mcm_async_receiver_thread_func(void *param )
{
    int ret = MCM_SUCCESS_V01;
    qmi_util_log("mcm receiver thread started\n");
    void *resp_event;

    while(1)
    {
        resp_event = mcm_msg_queue_dequeue(mcm_msg_asyncresp_msg_queue);
        if ( resp_event == NULL )
        {
            qmi_util_log("Currently no events in receiver queue..\n waiting for events\n");
            MCM_WAIT_FOR_SIGNAL(signal_asyncresp_queue);
            continue;
        }
        qmi_util_log("receiver thread: received async cb:\n");
        mcm_receiver_async_response_handler(resp_event);
    }
    return ret;
}



int mcm_ind_receiver_thread_func(	void *resp_event)
{
    int ret = MCM_SUCCESS_V01;
    qmi_util_log("mcm receiver thread started\n");

    while(1)
    {
        resp_event = mcm_msg_queue_dequeue(mcm_msg_ind_msg_queue);
        if ( resp_event == NULL )
        {
            qmi_util_log("Currently no events in receiver queue..\n waiting for events\n");
            MCM_WAIT_FOR_SIGNAL(signal_ind_queue);
            continue;
        }
        qmi_util_log("receiver thread: received async cb:\n");
        mcm_receiver_ind_handler(resp_event);
    }
    return ret;
}


void mcm_receiver_ind_handler(void *resp_event)
{

    int iter=0;
    mcm_ind_resp *event;
    event = (mcm_ind_resp*)resp_event;
    qmi_client_type client_handle;
    client_handle = event->client_handle;

    qmi_util_log("mcm_receiver_ind_handler Enter\n");

    for(iter=0;iter<MCM_MAX_CLIENTS;iter++)
    {
        if ( ind_map[iter].client_handle == client_handle )
        {
            (ind_map[iter].ind_cb)(event->client_handle,
                                event->msg_id,
                                event->ind_c_struct,
                                event->ind_c_struct_len,
                                NULL);
            break;
        }
    }

}



void mcm_receiver_async_response_handler
(
    void *resp_event
)
{
    int index = 0;
    qmi_client_type user_handle;
    mcm_async_resp *async_resp = (mcm_async_resp*)resp_event;

    for ( index=0; index<MCM_MAX_REQUESTS;index++)
    {
        if ( request_map[index].req_handle == async_resp->req_handle )
        {
            break;
        }
    }
    user_handle = request_map[index].user_handle;
    index = 0;

    for(index = 0; index < MCM_MAX_REQUESTS; index++)
    {
        if ( request_map[index].user_handle == user_handle )
        {
            if ( request_map[index].req_handle == async_resp->req_handle )
            {
                (request_map[index].async_cb)(user_handle,
                                            async_resp->msg_id,
                                            async_resp->resp_c_struct,
                                            async_resp->resp_c_struct_len,
                                            request_map[index].async_cb_data,
                                            NULL);
                break;
            }

        }

    }

}




