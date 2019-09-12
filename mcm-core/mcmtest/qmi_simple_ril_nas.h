/******************************************************************************
  @file    qmi_simple_ril_nas.h
  @brief   Sample simple RIL voice

  DESCRIPTION
  Sample Radio Interface Layer (telephony adaptation layer) voice subsystem

  ---------------------------------------------------------------------------

  Copyright (c) 2010 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.  ---------------------------------------------------------------------------
******************************************************************************/

#include "qmi_simple_ril_core.h"

#ifndef QMI_SIMPLE_RIL_NAS_H
#define QMI_SIMPLE_RIL_NAS_H

int qmi_simple_ril_mcm_nw_set_config(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);

int qmi_simple_ril_mcm_nw_get_config(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);

int qmi_simple_ril_mcm_nw_event_register(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);

int qmi_simple_ril_mcm_nw_status(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);

int simple_ril_nas_ind_handler(uint32_t msg_id, void *ind_c_struct);

int qmi_simple_ril_perform_network_scan_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);

int qmi_simple_ril_get_operator_name(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);

int qmi_simple_ril_initiate_network_manual_register_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);

int qmi_simple_ril_initiate_network_auto_register_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);

int qmi_simple_ril_mcm_nw_screen_on_off(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);

int qmi_simple_ril_get_signal_strength_handler(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);

int qmi_simple_ril_nw_cell_access_state_event(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);

int qmi_simple_ril_mcm_nw_get_nitz_time_info(qmi_simple_ril_cmd_input_info* cmd_params, qmi_simple_ril_cmd_ack_info* ack_info);

#endif // QMI_SIMPLE_RIL_VOICE_H
