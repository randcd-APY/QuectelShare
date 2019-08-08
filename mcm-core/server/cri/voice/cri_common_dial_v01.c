/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        C R I _ C O M M O N _ D I A L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the cri_common_dial service Data structures.

  Copyright (c) 2014 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.5
   It was generated on: Mon Dec 16 2013 (Spin )
   From IDL File: cri_common_dial_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "cri_common_dial_v01.h"


/*Type Definitions*/
static const uint8_t cri_common_dial_ss_call_forwarding_info_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(cri_common_dial_ss_call_forwarding_info_t_v01, type),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(cri_common_dial_ss_call_forwarding_info_t_v01, number),
  CRI_MAX_DIAL_STRING_LENGTH_V01,

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t cri_common_dial_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(cri_common_dial_req_msg_v01, dialstring),
  CRI_MAX_DIAL_STRING_LENGTH_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(cri_common_dial_req_msg_v01, request_type) - QMI_IDL_OFFSET8(cri_common_dial_req_msg_v01, request_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(cri_common_dial_req_msg_v01, request_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(cri_common_dial_req_msg_v01, sub_address) - QMI_IDL_OFFSET8(cri_common_dial_req_msg_v01, sub_address_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(cri_common_dial_req_msg_v01, sub_address),
  CRI_MAX_SUBADDRESS_LENGTH_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ss_type) - QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ss_type_valid)),
  0x12,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(cri_common_dial_req_msg_v01, ss_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ussd_type) - QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ussd_type_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(cri_common_dial_req_msg_v01, ussd_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ss_get_cf_reason) - QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ss_get_cf_reason_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(cri_common_dial_req_msg_v01, ss_get_cf_reason),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ss_set_cf_service) - QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ss_set_cf_service_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(cri_common_dial_req_msg_v01, ss_set_cf_service),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ss_set_cf_reason) - QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ss_set_cf_reason_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(cri_common_dial_req_msg_v01, ss_set_cf_reason),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ss_set_cf_number) - QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ss_set_cf_number_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(cri_common_dial_req_msg_v01, ss_set_cf_number),
  CRI_MAX_DIAL_STRING_LENGTH_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ss_set_cw_service) - QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ss_set_cw_service_valid)),
  0x18,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(cri_common_dial_req_msg_v01, ss_set_cw_service),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ss_set_fac_lock_code) - QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ss_set_fac_lock_code_valid)),
  0x19,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(cri_common_dial_req_msg_v01, ss_set_fac_lock_code),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ss_set_fac_lock_status) - QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ss_set_fac_lock_status_valid)),
  0x1A,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(cri_common_dial_req_msg_v01, ss_set_fac_lock_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ss_set_fac_lock_password) - QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ss_set_fac_lock_password_valid)),
  0x1B,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(cri_common_dial_req_msg_v01, ss_set_fac_lock_password),
  CRI_MAX_PASSWORD_LENGTH_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ss_change_cb_pw_reason) - QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ss_change_cb_pw_reason_valid)),
  0x1C,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(cri_common_dial_req_msg_v01, ss_change_cb_pw_reason),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ss_change_cb_pw_old_password) - QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ss_change_cb_pw_old_password_valid)),
  0x1D,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(cri_common_dial_req_msg_v01, ss_change_cb_pw_old_password),
  CRI_MAX_PASSWORD_LENGTH_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ss_change_cb_pw_new_password) - QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ss_change_cb_pw_new_password_valid)),
  0x1E,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(cri_common_dial_req_msg_v01, ss_change_cb_pw_new_password),
  CRI_MAX_PASSWORD_LENGTH_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ussd_send_ussd_encoding) - QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ussd_send_ussd_encoding_valid)),
  0x1F,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(cri_common_dial_req_msg_v01, ussd_send_ussd_encoding),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ussd_send_ussd_string) - QMI_IDL_OFFSET16RELATIVE(cri_common_dial_req_msg_v01, ussd_send_ussd_string_valid)),
  0x20,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(cri_common_dial_req_msg_v01, ussd_send_ussd_string),
  CRI_MAX_USSD_LENGTH_V01
};

static const uint8_t cri_common_dial_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(cri_common_dial_resp_msg_v01, response_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(cri_common_dial_resp_msg_v01, ss_type) - QMI_IDL_OFFSET8(cri_common_dial_resp_msg_v01, ss_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(cri_common_dial_resp_msg_v01, ss_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(cri_common_dial_resp_msg_v01, ussd_type) - QMI_IDL_OFFSET8(cri_common_dial_resp_msg_v01, ussd_type_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(cri_common_dial_resp_msg_v01, ussd_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(cri_common_dial_resp_msg_v01, ss_get_cf_status) - QMI_IDL_OFFSET8(cri_common_dial_resp_msg_v01, ss_get_cf_status_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(cri_common_dial_resp_msg_v01, ss_get_cf_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(cri_common_dial_resp_msg_v01, ss_get_cf_status_info) - QMI_IDL_OFFSET8(cri_common_dial_resp_msg_v01, ss_get_cf_status_info_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(cri_common_dial_resp_msg_v01, ss_get_cf_status_info),
  CRI_MAX_CALL_FORWARDING_INFO_V01,
  QMI_IDL_OFFSET8(cri_common_dial_resp_msg_v01, ss_get_cf_status_info) - QMI_IDL_OFFSET8(cri_common_dial_resp_msg_v01, ss_get_cf_status_info_len),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(cri_common_dial_resp_msg_v01, ss_get_cw_service) - QMI_IDL_OFFSET16RELATIVE(cri_common_dial_resp_msg_v01, ss_get_cw_service_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(cri_common_dial_resp_msg_v01, ss_get_cw_service),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(cri_common_dial_resp_msg_v01, ss_get_clir_action) - QMI_IDL_OFFSET16RELATIVE(cri_common_dial_resp_msg_v01, ss_get_clir_action_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(cri_common_dial_resp_msg_v01, ss_get_clir_action),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(cri_common_dial_resp_msg_v01, ss_get_clir_presentation) - QMI_IDL_OFFSET16RELATIVE(cri_common_dial_resp_msg_v01, ss_get_clir_presentation_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(cri_common_dial_resp_msg_v01, ss_get_clir_presentation)
};

/* Type Table */
static const qmi_idl_type_table_entry  cri_common_dial_type_table_v01[] = {
  {sizeof(cri_common_dial_ss_call_forwarding_info_t_v01), cri_common_dial_ss_call_forwarding_info_t_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry cri_common_dial_message_table_v01[] = {
  {sizeof(cri_common_dial_req_msg_v01), cri_common_dial_req_msg_data_v01},
  {sizeof(cri_common_dial_resp_msg_v01), cri_common_dial_resp_msg_data_v01}
};

/* Range Table */
/* Predefine the Type Table Object */
const qmi_idl_type_table_object cri_common_dial_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *cri_common_dial_qmi_idl_type_table_object_referenced_tables_v01[] =
{&cri_common_dial_qmi_idl_type_table_object_v01};

/*Type Table Object*/
const qmi_idl_type_table_object cri_common_dial_qmi_idl_type_table_object_v01 = {
  sizeof(cri_common_dial_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(cri_common_dial_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  cri_common_dial_type_table_v01,
  cri_common_dial_message_table_v01,
  cri_common_dial_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

