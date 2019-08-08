/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M C M _ V O I C E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the mcm_voice service Data structures.

  Copyright (c) 2013, 2015 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Thu Oct 29 2015 (Spin 0)
   From IDL File: mcm_voice_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "mcm_voice_v01.h"
#include "mcm_common_v01.h"
#include "mcm_service_object_v01.h"


/*Type Definitions*/
static const uint8_t mcm_voice_uusdata_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_uusdata_t_v01, type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_uusdata_t_v01, dcs),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_uusdata_t_v01, uus_data),
  MCM_MAX_UUS_DATA_V01,
  QMI_IDL_OFFSET8(mcm_voice_uusdata_t_v01, uus_data) - QMI_IDL_OFFSET8(mcm_voice_uusdata_t_v01, uus_data_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_voice_call_record_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_call_record_t_v01, call_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_call_record_t_v01, state),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_call_record_t_v01, tech),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_voice_call_record_t_v01, number),
  MCM_MAX_PHONE_NUMBER_V01,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_call_record_t_v01, number_presentation),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_call_record_t_v01, direction),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_call_record_t_v01, uusdata_valid),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_call_record_t_v01, uusdata),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_voice_dtmf_info_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_dtmf_info_t_v01, call_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_dtmf_info_t_v01, dtmf_event),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_dtmf_info_t_v01, digit),
  MCM_MAX_DTMF_LENGTH_V01,
  QMI_IDL_OFFSET8(mcm_voice_dtmf_info_t_v01, digit) - QMI_IDL_OFFSET8(mcm_voice_dtmf_info_t_v01, digit_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_voice_call_forwarding_info_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_call_forwarding_info_t_v01, type),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_voice_call_forwarding_info_t_v01, number),
  MCM_MAX_PHONE_NUMBER_V01,

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
/*
 * mcm_voice_get_calls_req_msg is empty
 * static const uint8_t mcm_voice_get_calls_req_msg_data_v01[] = {
 * };
 */

static const uint8_t mcm_voice_get_calls_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_get_calls_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_get_calls_resp_msg_v01, calls) - QMI_IDL_OFFSET8(mcm_voice_get_calls_resp_msg_v01, calls_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_get_calls_resp_msg_v01, calls),
  MCM_MAX_VOICE_CALLS_V01,
  QMI_IDL_OFFSET8(mcm_voice_get_calls_resp_msg_v01, calls) - QMI_IDL_OFFSET8(mcm_voice_get_calls_resp_msg_v01, calls_len),
  QMI_IDL_TYPE88(0, 1)
};

static const uint8_t mcm_voice_dial_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_dial_req_msg_v01, address) - QMI_IDL_OFFSET8(mcm_voice_dial_req_msg_v01, address_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_voice_dial_req_msg_v01, address),
  MCM_MAX_PHONE_NUMBER_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_dial_req_msg_v01, call_type) - QMI_IDL_OFFSET8(mcm_voice_dial_req_msg_v01, call_type_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_dial_req_msg_v01, call_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_dial_req_msg_v01, uusdata) - QMI_IDL_OFFSET8(mcm_voice_dial_req_msg_v01, uusdata_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_dial_req_msg_v01, uusdata),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_dial_req_msg_v01, emergency_cat) - QMI_IDL_OFFSET8(mcm_voice_dial_req_msg_v01, emergency_cat_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_dial_req_msg_v01, emergency_cat),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_dial_req_msg_v01, ecall_msd) - QMI_IDL_OFFSET8(mcm_voice_dial_req_msg_v01, ecall_msd_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_dial_req_msg_v01, ecall_msd),
  MCM_MAX_ECALL_MSD_V01,
  QMI_IDL_OFFSET8(mcm_voice_dial_req_msg_v01, ecall_msd) - QMI_IDL_OFFSET8(mcm_voice_dial_req_msg_v01, ecall_msd_len)
};

static const uint8_t mcm_voice_dial_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_dial_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_dial_resp_msg_v01, call_id) - QMI_IDL_OFFSET8(mcm_voice_dial_resp_msg_v01, call_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_dial_resp_msg_v01, call_id)
};

static const uint8_t mcm_voice_get_call_status_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_get_call_status_req_msg_v01, call_id)
};

static const uint8_t mcm_voice_get_call_status_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_get_call_status_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_get_call_status_resp_msg_v01, status) - QMI_IDL_OFFSET8(mcm_voice_get_call_status_resp_msg_v01, status_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_get_call_status_resp_msg_v01, status),
  QMI_IDL_TYPE88(0, 1)
};

static const uint8_t mcm_voice_dtmf_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_voice_dtmf_req_msg_v01, dtmf),
  MCM_MAX_DTMF_LENGTH_V01
};

static const uint8_t mcm_voice_dtmf_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_dtmf_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_voice_start_dtmf_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_start_dtmf_req_msg_v01, call_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_start_dtmf_req_msg_v01, digit)
};

static const uint8_t mcm_voice_start_dtmf_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_start_dtmf_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_start_dtmf_resp_msg_v01, call_id) - QMI_IDL_OFFSET8(mcm_voice_start_dtmf_resp_msg_v01, call_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_start_dtmf_resp_msg_v01, call_id)
};

static const uint8_t mcm_voice_stop_dtmf_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_stop_dtmf_req_msg_v01, call_id)
};

static const uint8_t mcm_voice_stop_dtmf_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_stop_dtmf_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_stop_dtmf_resp_msg_v01, call_id) - QMI_IDL_OFFSET8(mcm_voice_stop_dtmf_resp_msg_v01, call_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_stop_dtmf_resp_msg_v01, call_id)
};

static const uint8_t mcm_voice_mute_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_mute_req_msg_v01, call_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_mute_req_msg_v01, mute_type)
};

static const uint8_t mcm_voice_mute_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_mute_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_voice_flash_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_voice_flash_req_msg_v01, sflash_string),
  MCM_MAX_PHONE_NUMBER_V01
};

static const uint8_t mcm_voice_flash_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_flash_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_voice_hangup_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_hangup_req_msg_v01, call_id)
};

static const uint8_t mcm_voice_hangup_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_hangup_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_voice_command_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_command_req_msg_v01, call_operation),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_command_req_msg_v01, call_id) - QMI_IDL_OFFSET8(mcm_voice_command_req_msg_v01, call_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_command_req_msg_v01, call_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_command_req_msg_v01, cause) - QMI_IDL_OFFSET8(mcm_voice_command_req_msg_v01, cause_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_command_req_msg_v01, cause)
};

static const uint8_t mcm_voice_command_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_command_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_voice_auto_answer_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_auto_answer_req_msg_v01, auto_answer_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_auto_answer_req_msg_v01, anto_answer_timer) - QMI_IDL_OFFSET8(mcm_voice_auto_answer_req_msg_v01, anto_answer_timer_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_auto_answer_req_msg_v01, anto_answer_timer)
};

static const uint8_t mcm_voice_auto_answer_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_auto_answer_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_voice_event_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_event_register_req_msg_v01, register_voice_call_event) - QMI_IDL_OFFSET8(mcm_voice_event_register_req_msg_v01, register_voice_call_event_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_event_register_req_msg_v01, register_voice_call_event),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_event_register_req_msg_v01, register_mute_event) - QMI_IDL_OFFSET8(mcm_voice_event_register_req_msg_v01, register_mute_event_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_event_register_req_msg_v01, register_mute_event),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_event_register_req_msg_v01, register_dtmf_event) - QMI_IDL_OFFSET8(mcm_voice_event_register_req_msg_v01, register_dtmf_event_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_event_register_req_msg_v01, register_dtmf_event),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_event_register_req_msg_v01, register_e911_state_event) - QMI_IDL_OFFSET8(mcm_voice_event_register_req_msg_v01, register_e911_state_event_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_event_register_req_msg_v01, register_e911_state_event)
};

static const uint8_t mcm_voice_event_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_event_register_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_voice_call_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_call_ind_msg_v01, calls),
  MCM_MAX_VOICE_CALLS_V01,
  QMI_IDL_OFFSET8(mcm_voice_call_ind_msg_v01, calls) - QMI_IDL_OFFSET8(mcm_voice_call_ind_msg_v01, calls_len),
  QMI_IDL_TYPE88(0, 1)
};

static const uint8_t mcm_voice_mute_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_mute_ind_msg_v01, is_mute)
};

static const uint8_t mcm_voice_dtmf_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_dtmf_ind_msg_v01, dtmf_info),
  QMI_IDL_TYPE88(0, 2)
};

static const uint8_t mcm_voice_get_call_forwarding_status_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_get_call_forwarding_status_req_msg_v01, reason)
};

static const uint8_t mcm_voice_get_call_forwarding_status_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_get_call_forwarding_status_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_get_call_forwarding_status_resp_msg_v01, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_get_call_forwarding_status_resp_msg_v01, info) - QMI_IDL_OFFSET8(mcm_voice_get_call_forwarding_status_resp_msg_v01, info_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_get_call_forwarding_status_resp_msg_v01, info),
  MCM_MAX_CALL_FORWARDING_INFO_V01,
  QMI_IDL_OFFSET8(mcm_voice_get_call_forwarding_status_resp_msg_v01, info) - QMI_IDL_OFFSET8(mcm_voice_get_call_forwarding_status_resp_msg_v01, info_len),
  QMI_IDL_TYPE88(0, 3)
};

static const uint8_t mcm_voice_set_call_forwarding_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_set_call_forwarding_req_msg_v01, fwdservice),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_set_call_forwarding_req_msg_v01, reason),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_set_call_forwarding_req_msg_v01, forwarding_number) - QMI_IDL_OFFSET8(mcm_voice_set_call_forwarding_req_msg_v01, forwarding_number_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_voice_set_call_forwarding_req_msg_v01, forwarding_number),
  MCM_MAX_PHONE_NUMBER_V01
};

static const uint8_t mcm_voice_set_call_forwarding_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_set_call_forwarding_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * mcm_voice_get_call_waiting_status_req_msg is empty
 * static const uint8_t mcm_voice_get_call_waiting_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t mcm_voice_get_call_waiting_status_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_get_call_waiting_status_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_get_call_waiting_status_resp_msg_v01, status)
};

static const uint8_t mcm_voice_set_call_waiting_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_set_call_waiting_req_msg_v01, cwservice)
};

static const uint8_t mcm_voice_set_call_waiting_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_set_call_waiting_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * mcm_voice_get_clir_req_msg is empty
 * static const uint8_t mcm_voice_get_clir_req_msg_data_v01[] = {
 * };
 */

static const uint8_t mcm_voice_get_clir_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_get_clir_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_get_clir_resp_msg_v01, action),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_get_clir_resp_msg_v01, presentation)
};

static const uint8_t mcm_voice_set_clir_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_set_clir_req_msg_v01, clir_action)
};

static const uint8_t mcm_voice_set_clir_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_set_clir_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_voice_set_facility_lock_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_set_facility_lock_req_msg_v01, code),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_set_facility_lock_req_msg_v01, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_voice_set_facility_lock_req_msg_v01, password),
  MCM_MAX_PASSWORD_LENGTH_V01
};

static const uint8_t mcm_voice_set_facility_lock_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_set_facility_lock_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_voice_change_call_barring_password_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_change_call_barring_password_req_msg_v01, reason),

  0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_voice_change_call_barring_password_req_msg_v01, old_password),
  MCM_MAX_PASSWORD_LENGTH_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_voice_change_call_barring_password_req_msg_v01, new_password),
  MCM_MAX_PASSWORD_LENGTH_V01
};

static const uint8_t mcm_voice_change_call_barring_password_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_change_call_barring_password_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_voice_send_ussd_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_send_ussd_req_msg_v01, type),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_send_ussd_req_msg_v01, encoding),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_voice_send_ussd_req_msg_v01, ussd_string),
  MCM_MAX_USSD_LENGTH_V01
};

static const uint8_t mcm_voice_send_ussd_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_send_ussd_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * mcm_voice_cancel_ussd_req_msg is empty
 * static const uint8_t mcm_voice_cancel_ussd_req_msg_data_v01[] = {
 * };
 */

static const uint8_t mcm_voice_cancel_ussd_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_cancel_ussd_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_voice_receive_ussd_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_receive_ussd_ind_msg_v01, notification),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_voice_receive_ussd_ind_msg_v01, ussd),
  MCM_MAX_USSD_LENGTH_V01
};

static const uint8_t mcm_voice_common_dial_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_voice_common_dial_req_msg_v01, request),
  MCM_MAX_PHONE_NUMBER_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_common_dial_req_msg_v01, call_type) - QMI_IDL_OFFSET8(mcm_voice_common_dial_req_msg_v01, call_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_common_dial_req_msg_v01, call_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_common_dial_req_msg_v01, uusdata) - QMI_IDL_OFFSET8(mcm_voice_common_dial_req_msg_v01, uusdata_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_common_dial_req_msg_v01, uusdata),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_common_dial_req_msg_v01, emergency_cat) - QMI_IDL_OFFSET8(mcm_voice_common_dial_req_msg_v01, emergency_cat_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_common_dial_req_msg_v01, emergency_cat),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_common_dial_req_msg_v01, ecall_msd) - QMI_IDL_OFFSET8(mcm_voice_common_dial_req_msg_v01, ecall_msd_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_common_dial_req_msg_v01, ecall_msd),
  MCM_MAX_ECALL_MSD_V01,
  QMI_IDL_OFFSET8(mcm_voice_common_dial_req_msg_v01, ecall_msd) - QMI_IDL_OFFSET8(mcm_voice_common_dial_req_msg_v01, ecall_msd_len)
};

static const uint8_t mcm_voice_common_dial_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_common_dial_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_common_dial_resp_msg_v01, req_changed_to_type) - QMI_IDL_OFFSET8(mcm_voice_common_dial_resp_msg_v01, req_changed_to_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_common_dial_resp_msg_v01, req_changed_to_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_common_dial_resp_msg_v01, call_id) - QMI_IDL_OFFSET8(mcm_voice_common_dial_resp_msg_v01, call_id_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_common_dial_resp_msg_v01, call_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_common_dial_resp_msg_v01, ss_get_cf_status) - QMI_IDL_OFFSET8(mcm_voice_common_dial_resp_msg_v01, ss_get_cf_status_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_common_dial_resp_msg_v01, ss_get_cf_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_common_dial_resp_msg_v01, ss_get_cf_info) - QMI_IDL_OFFSET8(mcm_voice_common_dial_resp_msg_v01, ss_get_cf_info_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_common_dial_resp_msg_v01, ss_get_cf_info),
  MCM_MAX_CALL_FORWARDING_INFO_V01,
  QMI_IDL_OFFSET8(mcm_voice_common_dial_resp_msg_v01, ss_get_cf_info) - QMI_IDL_OFFSET8(mcm_voice_common_dial_resp_msg_v01, ss_get_cf_info_len),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mcm_voice_common_dial_resp_msg_v01, ss_get_cw_status) - QMI_IDL_OFFSET16RELATIVE(mcm_voice_common_dial_resp_msg_v01, ss_get_cw_status_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_voice_common_dial_resp_msg_v01, ss_get_cw_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mcm_voice_common_dial_resp_msg_v01, ss_get_clir_action) - QMI_IDL_OFFSET16RELATIVE(mcm_voice_common_dial_resp_msg_v01, ss_get_clir_action_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_voice_common_dial_resp_msg_v01, ss_get_clir_action),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mcm_voice_common_dial_resp_msg_v01, ss_get_clir_presentation) - QMI_IDL_OFFSET16RELATIVE(mcm_voice_common_dial_resp_msg_v01, ss_get_clir_presentation_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_voice_common_dial_resp_msg_v01, ss_get_clir_presentation)
};

static const uint8_t mcm_voice_update_msd_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_update_msd_req_msg_v01, ecall_msd) - QMI_IDL_OFFSET8(mcm_voice_update_msd_req_msg_v01, ecall_msd_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_update_msd_req_msg_v01, ecall_msd),
  MCM_MAX_ECALL_MSD_V01,
  QMI_IDL_OFFSET8(mcm_voice_update_msd_req_msg_v01, ecall_msd) - QMI_IDL_OFFSET8(mcm_voice_update_msd_req_msg_v01, ecall_msd_len)
};

static const uint8_t mcm_voice_update_msd_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_update_msd_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_voice_e911_state_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_e911_state_ind_msg_v01, e911_state)
};

/*
 * mcm_voice_get_e911_state_req_msg is empty
 * static const uint8_t mcm_voice_get_e911_state_req_msg_data_v01[] = {
 * };
 */

static const uint8_t mcm_voice_get_e911_state_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_voice_get_e911_state_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_voice_get_e911_state_resp_msg_v01, e911_state) - QMI_IDL_OFFSET8(mcm_voice_get_e911_state_resp_msg_v01, e911_state_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_voice_get_e911_state_resp_msg_v01, e911_state)
};

/* Type Table */
static const qmi_idl_type_table_entry  mcm_voice_type_table_v01[] = {
  {sizeof(mcm_voice_uusdata_t_v01), mcm_voice_uusdata_t_data_v01},
  {sizeof(mcm_voice_call_record_t_v01), mcm_voice_call_record_t_data_v01},
  {sizeof(mcm_voice_dtmf_info_t_v01), mcm_voice_dtmf_info_t_data_v01},
  {sizeof(mcm_voice_call_forwarding_info_t_v01), mcm_voice_call_forwarding_info_t_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry mcm_voice_message_table_v01[] = {
  {sizeof(mcm_voice_get_calls_req_msg_v01), 0},
  {sizeof(mcm_voice_get_calls_resp_msg_v01), mcm_voice_get_calls_resp_msg_data_v01},
  {sizeof(mcm_voice_dial_req_msg_v01), mcm_voice_dial_req_msg_data_v01},
  {sizeof(mcm_voice_dial_resp_msg_v01), mcm_voice_dial_resp_msg_data_v01},
  {sizeof(mcm_voice_get_call_status_req_msg_v01), mcm_voice_get_call_status_req_msg_data_v01},
  {sizeof(mcm_voice_get_call_status_resp_msg_v01), mcm_voice_get_call_status_resp_msg_data_v01},
  {sizeof(mcm_voice_dtmf_req_msg_v01), mcm_voice_dtmf_req_msg_data_v01},
  {sizeof(mcm_voice_dtmf_resp_msg_v01), mcm_voice_dtmf_resp_msg_data_v01},
  {sizeof(mcm_voice_start_dtmf_req_msg_v01), mcm_voice_start_dtmf_req_msg_data_v01},
  {sizeof(mcm_voice_start_dtmf_resp_msg_v01), mcm_voice_start_dtmf_resp_msg_data_v01},
  {sizeof(mcm_voice_stop_dtmf_req_msg_v01), mcm_voice_stop_dtmf_req_msg_data_v01},
  {sizeof(mcm_voice_stop_dtmf_resp_msg_v01), mcm_voice_stop_dtmf_resp_msg_data_v01},
  {sizeof(mcm_voice_mute_req_msg_v01), mcm_voice_mute_req_msg_data_v01},
  {sizeof(mcm_voice_mute_resp_msg_v01), mcm_voice_mute_resp_msg_data_v01},
  {sizeof(mcm_voice_flash_req_msg_v01), mcm_voice_flash_req_msg_data_v01},
  {sizeof(mcm_voice_flash_resp_msg_v01), mcm_voice_flash_resp_msg_data_v01},
  {sizeof(mcm_voice_hangup_req_msg_v01), mcm_voice_hangup_req_msg_data_v01},
  {sizeof(mcm_voice_hangup_resp_msg_v01), mcm_voice_hangup_resp_msg_data_v01},
  {sizeof(mcm_voice_command_req_msg_v01), mcm_voice_command_req_msg_data_v01},
  {sizeof(mcm_voice_command_resp_msg_v01), mcm_voice_command_resp_msg_data_v01},
  {sizeof(mcm_voice_auto_answer_req_msg_v01), mcm_voice_auto_answer_req_msg_data_v01},
  {sizeof(mcm_voice_auto_answer_resp_msg_v01), mcm_voice_auto_answer_resp_msg_data_v01},
  {sizeof(mcm_voice_event_register_req_msg_v01), mcm_voice_event_register_req_msg_data_v01},
  {sizeof(mcm_voice_event_register_resp_msg_v01), mcm_voice_event_register_resp_msg_data_v01},
  {sizeof(mcm_voice_call_ind_msg_v01), mcm_voice_call_ind_msg_data_v01},
  {sizeof(mcm_voice_mute_ind_msg_v01), mcm_voice_mute_ind_msg_data_v01},
  {sizeof(mcm_voice_dtmf_ind_msg_v01), mcm_voice_dtmf_ind_msg_data_v01},
  {sizeof(mcm_voice_get_call_forwarding_status_req_msg_v01), mcm_voice_get_call_forwarding_status_req_msg_data_v01},
  {sizeof(mcm_voice_get_call_forwarding_status_resp_msg_v01), mcm_voice_get_call_forwarding_status_resp_msg_data_v01},
  {sizeof(mcm_voice_set_call_forwarding_req_msg_v01), mcm_voice_set_call_forwarding_req_msg_data_v01},
  {sizeof(mcm_voice_set_call_forwarding_resp_msg_v01), mcm_voice_set_call_forwarding_resp_msg_data_v01},
  {sizeof(mcm_voice_get_call_waiting_status_req_msg_v01), 0},
  {sizeof(mcm_voice_get_call_waiting_status_resp_msg_v01), mcm_voice_get_call_waiting_status_resp_msg_data_v01},
  {sizeof(mcm_voice_set_call_waiting_req_msg_v01), mcm_voice_set_call_waiting_req_msg_data_v01},
  {sizeof(mcm_voice_set_call_waiting_resp_msg_v01), mcm_voice_set_call_waiting_resp_msg_data_v01},
  {sizeof(mcm_voice_get_clir_req_msg_v01), 0},
  {sizeof(mcm_voice_get_clir_resp_msg_v01), mcm_voice_get_clir_resp_msg_data_v01},
  {sizeof(mcm_voice_set_clir_req_msg_v01), mcm_voice_set_clir_req_msg_data_v01},
  {sizeof(mcm_voice_set_clir_resp_msg_v01), mcm_voice_set_clir_resp_msg_data_v01},
  {sizeof(mcm_voice_set_facility_lock_req_msg_v01), mcm_voice_set_facility_lock_req_msg_data_v01},
  {sizeof(mcm_voice_set_facility_lock_resp_msg_v01), mcm_voice_set_facility_lock_resp_msg_data_v01},
  {sizeof(mcm_voice_change_call_barring_password_req_msg_v01), mcm_voice_change_call_barring_password_req_msg_data_v01},
  {sizeof(mcm_voice_change_call_barring_password_resp_msg_v01), mcm_voice_change_call_barring_password_resp_msg_data_v01},
  {sizeof(mcm_voice_send_ussd_req_msg_v01), mcm_voice_send_ussd_req_msg_data_v01},
  {sizeof(mcm_voice_send_ussd_resp_msg_v01), mcm_voice_send_ussd_resp_msg_data_v01},
  {sizeof(mcm_voice_cancel_ussd_req_msg_v01), 0},
  {sizeof(mcm_voice_cancel_ussd_resp_msg_v01), mcm_voice_cancel_ussd_resp_msg_data_v01},
  {sizeof(mcm_voice_receive_ussd_ind_msg_v01), mcm_voice_receive_ussd_ind_msg_data_v01},
  {sizeof(mcm_voice_common_dial_req_msg_v01), mcm_voice_common_dial_req_msg_data_v01},
  {sizeof(mcm_voice_common_dial_resp_msg_v01), mcm_voice_common_dial_resp_msg_data_v01},
  {sizeof(mcm_voice_update_msd_req_msg_v01), mcm_voice_update_msd_req_msg_data_v01},
  {sizeof(mcm_voice_update_msd_resp_msg_v01), mcm_voice_update_msd_resp_msg_data_v01},
  {sizeof(mcm_voice_e911_state_ind_msg_v01), mcm_voice_e911_state_ind_msg_data_v01},
  {sizeof(mcm_voice_get_e911_state_req_msg_v01), 0},
  {sizeof(mcm_voice_get_e911_state_resp_msg_v01), mcm_voice_get_e911_state_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
const qmi_idl_type_table_object mcm_voice_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *mcm_voice_qmi_idl_type_table_object_referenced_tables_v01[] =
{&mcm_voice_qmi_idl_type_table_object_v01, &mcm_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
const qmi_idl_type_table_object mcm_voice_qmi_idl_type_table_object_v01 = {
  sizeof(mcm_voice_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(mcm_voice_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  mcm_voice_type_table_v01,
  mcm_voice_message_table_v01,
  mcm_voice_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

