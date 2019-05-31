/*************************************************************************************
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
**************************************************************************************/


#ifndef MCM_SINGLE_PROCESS_H
#define MCM_SINGLE_PROCESS_H

#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "qmi_csi.h"

typedef struct {
    int req_handle;
    qmi_client_handle client_handle;
    unsigned int msg_id;
    void *resp_c_struct;
    unsigned int resp_c_struct_len;
}mcm_async_resp;


typedef struct {
    qmi_client_handle client_handle;
    unsigned int msg_id;
    void *ind_c_struct;
    unsigned int ind_c_struct_len;
}mcm_ind_resp;


#endif // MCM_MSG_RECEIVER_H

