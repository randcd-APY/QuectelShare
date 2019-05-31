/***************************************************************************************************
    @file
    hlos_dms_core.h

    @brief
    Supports functions for handling HLOS CSVT requests.

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
***************************************************************************************************/

#ifndef HLOS_VOICE_CORE_H
#define HLOS_VOICE_CORE_H

#include "utils_common.h"
#include "cri_core.h"
#include "cri_voice.h"


void hlos_voice_dial_request_handler(void *event_data);

void hlos_voice_hangup_request_handler(void *event_data);

void hlos_voice_command_request_handler(void *event_data);

void hlos_voice_get_calls_request_handler(void *event_data);
void hlos_voice_get_call_status_request_handler(void *event_data);
void hlos_voice_mute_request_handler(void *event_data);
void hlos_voice_auto_answer_request_handler(void *event_data);
void hlos_voice_start_dtmf_request_handler(void *event_data);
void hlos_voice_stop_dtmf_request_handler(void *event_data);

void hlos_voice_get_call_forwarding_status_request_handler(void *event_data);
void hlos_voice_set_call_forwarding_request_handler(void *event_data);
void hlos_voice_get_call_waiting_status_request_handler(void *event_data);
void hlos_voice_set_call_waiting_request_handler(void *event_data);
void hlos_voice_get_clir_request_handler(void *event_data);
void hlos_voice_set_clir_request_handler(void *event_data);
void hlos_voice_change_call_barring_password_request_handler(void *event_data);
void hlos_voice_send_ussd_request_handler(void *event_data);
void hlos_voice_cancel_ussd_request_handler(void *event_data);
void hlos_voice_update_ecall_msd_request_handler(void *event_data);

void hlos_voice_unsol_ind_handler
(
    unsigned long message_id,
    void *ind_data,
    int ind_data_len
);

void hlos_voice_get_e911_state_info_handler
(
    void *event_data
);

#endif

