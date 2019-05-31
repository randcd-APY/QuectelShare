/******************************************************************************
  @file    qmi_simple_ril_voice.h
  @brief   Sample simple RIL voice

  DESCRIPTION
  Sample Radio Interface Layer (telephony adaptation layer) voice subsystem

  ---------------------------------------------------------------------------

  Copyright (c) 2010, 2013-2014 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#include "qmi_simple_ril_core.h"

#ifndef QMI_SIMPLE_RIL_VOICE_H
#define QMI_SIMPLE_RIL_VOICE_H
/*
extern void qmi_simple_ril_voice_init();
extern int qmi_simple_ril_voice_unsolicited_qmi_message_handler (qmi_util_service_message_info* unsolicited_msg_info,
                                                          qmi_simple_ril_cmd_completion_info* uplink_message);
*/
extern int qmi_simple_ril_dial_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);
extern int qmi_simple_ril_call_status_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);
extern int qmi_simple_ril_dtmf_start_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);

extern int qmi_simple_ril_dtmf_stop_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);
//extern int qmi_simple_ril_voice_answer_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);
//extern int qmi_simple_ril_voice_reject_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);
extern int qmi_simple_ril_voice_end_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);
//extern int qmi_simple_ril_voice_hold_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);
//extern int qmi_simple_ril_voice_resume_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);
//extern int qmi_simple_ril_send_cdma_burst_dtmf_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);
//extern int qmi_simple_ril_send_cdma_burst_dtmf_callback (qmi_util_service_message_info* callback_msg_info,qmi_simple_ril_cmd_completion_info* uplink_message);
//extern int qmi_simple_ril_voice_call_state_resp_handler (qmi_util_service_message_info* unsolicited_msg_info,
//                                                          qmi_simple_ril_cmd_completion_info* uplink_message);

extern int qmi_simple_ril_ss_get_call_forwarding_status_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);
extern int qmi_simple_ril_ss_set_call_forwarding_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);
extern int qmi_simple_ril_ss_get_call_waiting_status_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);
extern int qmi_simple_ril_ss_set_call_waiting_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);
extern int qmi_simple_ril_ss_get_clir_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);
extern int qmi_simple_ril_ss_set_clir_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);
extern int qmi_simple_ril_ss_change_call_barring_password_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);
extern int qmi_simple_ril_ussd_send_ussd(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);
extern int qmi_simple_ril_ussd_cancel_ussd(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);
int qmi_simple_ril_voice_ecall_msd_handler
(
    qmi_simple_ril_cmd_input_info *cmd_params,
    qmi_simple_ril_cmd_ack_info *ack_info
);

int qmi_simple_ril_voice_get_e911_state_info_handler
(
    qmi_simple_ril_cmd_input_info *cmd_params,
    qmi_simple_ril_cmd_ack_info *ack_info
);
#endif // QMI_SIMPLE_RIL_VOICE_H
