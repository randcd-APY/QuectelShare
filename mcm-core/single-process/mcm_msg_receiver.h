
/*************************************************************************************
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
**************************************************************************************/


#ifndef MCM_MSG_RECEIVER_H
#define MCM_MSG_RECEIVER_H

#include "common_v01.h"
#include "qmi_client.h"

typedef struct mcm_async_cb_map
{
    int token_id;
    qmi_client_recv_msg_async_cb async_cb;
}mcm_async_cb_map;

void mcm_start_receiver_thread(void);

int mcm_ind_receiver_thread_func(void *param );

int mcm_async_receiver_thread_func(void *param );


void mcm_receiver_async_response_handler
(
    void *resp_event
);

#endif // MCM_MSG_RECEIVER_H

