/***************************************************************************************************
    @file
    hlos_sms_core.h

    @brief
    Supports functions for handling HLOS CSVT requests.

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
***************************************************************************************************/

#ifndef HLOS_SMS_CORE_H
#define HLOS_SMS_CORE_H

#include "utils_common.h"
#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include "cri_core.h"


#define MCM_SMS_MAX_MSG_LENGTH 160
#define MCM_SMS_MAX_ADDR_LENGTH 252



void hlos_sms_send_mo_msg_request_handler(void *event_data);

void hlos_sms_send_ack_request_handler(void *ind_data);


void hlos_sms_send_mo_msg_response_handler(cri_core_context_type context,
                                        cri_core_error_type cri_core_error,
                                        void *hlos_cb_data,
                                        void *cri_resp_data);

void hlos_sms_send_ack_response_handler(cri_core_context_type context,
                                        cri_core_error_type cri_core_error,
                                        void *hlos_cb_data,
                                        void *cri_resp_data);


/***************************************************************************************************
    @function
    hlos_sms_unsol_ind_handler

    @brief
    Handles CRI WMS indications.

    @param[in]
        message_id
           message id of the indication
        ind_data
           pointer to the indication data that was received
        ind_data_len
           length of the indication data that was received

    @param[out]
        none

    @retval
    none
***************************************************************************************************/
void hlos_sms_unsol_ind_handler(unsigned long message_id,
                                void *ind_data,
                                int ind_data_len);


#endif

