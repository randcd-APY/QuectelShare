/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M C M _ S M S _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the mcm_sms service Data structures.

  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.5
   It was generated on: Wed Nov 20 2013 (Spin 0)
   From IDL File: mcm_sms_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "mcm_sms_v01.h"
#include "mcm_common_v01.h"
#include "mcm_service_object_v01.h"


/*Type Definitions*/
static const uint8_t mcm_cbs_cmae_expire_t_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_cbs_cmae_expire_t_v01, year),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_cbs_cmae_expire_t_v01, month),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_cbs_cmae_expire_t_v01, day),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_cbs_cmae_expire_t_v01, hours),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_cbs_cmae_expire_t_v01, minutes),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_cbs_cmae_expire_t_v01, seconds),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_cbs_cmae_record_type_0_t_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_cbs_cmae_record_type_0_t_v01, message_content),
  MCM_SMS_MAX_MT_MSG_LENGTH_V01,
  QMI_IDL_OFFSET8(mcm_cbs_cmae_record_type_0_t_v01, message_content) - QMI_IDL_OFFSET8(mcm_cbs_cmae_record_type_0_t_v01, message_content_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_cbs_cmae_record_type_1_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_cbs_cmae_record_type_1_t_v01, category),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_cbs_cmae_record_type_1_t_v01, response),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_cbs_cmae_record_type_1_t_v01, severity),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_cbs_cmae_record_type_1_t_v01, urgency),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_cbs_cmae_record_type_1_t_v01, certainty),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_cbs_cmae_record_type_2_t_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_cbs_cmae_record_type_2_t_v01, id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_cbs_cmae_record_type_2_t_v01, alert_handling),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_cbs_cmae_record_type_2_t_v01, expire),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_cbs_cmae_record_type_2_t_v01, language),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t mcm_sms_set_service_center_cfg_type_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_sms_set_service_center_cfg_type_req_msg_v01, service_center_addr),
  MCM_SMS_MAX_ADDR_LENGTH_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mcm_sms_set_service_center_cfg_type_req_msg_v01, validity_time) - QMI_IDL_OFFSET16RELATIVE(mcm_sms_set_service_center_cfg_type_req_msg_v01, validity_time_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_sms_set_service_center_cfg_type_req_msg_v01, validity_time)
};

static const uint8_t mcm_sms_set_service_center_cfg_type_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sms_set_service_center_cfg_type_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * mcm_sms_get_service_center_cfg_type_req_msg is empty
 * static const uint8_t mcm_sms_get_service_center_cfg_type_req_msg_data_v01[] = {
 * };
 */

static const uint8_t mcm_sms_get_service_center_cfg_type_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sms_get_service_center_cfg_type_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sms_get_service_center_cfg_type_resp_msg_v01, service_center_addr) - QMI_IDL_OFFSET8(mcm_sms_get_service_center_cfg_type_resp_msg_v01, service_center_addr_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_sms_get_service_center_cfg_type_resp_msg_v01, service_center_addr),
  MCM_SMS_MAX_ADDR_LENGTH_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mcm_sms_get_service_center_cfg_type_resp_msg_v01, validity_time) - QMI_IDL_OFFSET16RELATIVE(mcm_sms_get_service_center_cfg_type_resp_msg_v01, validity_time_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_sms_get_service_center_cfg_type_resp_msg_v01, validity_time)
};

static const uint8_t mcm_sms_send_mo_msg_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sms_send_mo_msg_req_msg_v01, message_format),

  0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_sms_send_mo_msg_req_msg_v01, message_content),
  ((MCM_SMS_MAX_MO_MSG_LENGTH_V01) & 0xFF), ((MCM_SMS_MAX_MO_MSG_LENGTH_V01) >> 8),

  0x03,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(mcm_sms_send_mo_msg_req_msg_v01, destination),
  MCM_SMS_MAX_ADDR_LENGTH_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mcm_sms_send_mo_msg_req_msg_v01, size_validation) - QMI_IDL_OFFSET16RELATIVE(mcm_sms_send_mo_msg_req_msg_v01, size_validation_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_sms_send_mo_msg_req_msg_v01, size_validation)
};

static const uint8_t mcm_sms_send_mo_msg_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sms_send_mo_msg_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_sms_set_msg_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sms_set_msg_config_req_msg_v01, default_size_validation_mode) - QMI_IDL_OFFSET8(mcm_sms_set_msg_config_req_msg_v01, default_size_validation_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sms_set_msg_config_req_msg_v01, default_size_validation_mode),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sms_set_msg_config_req_msg_v01, enable_cb) - QMI_IDL_OFFSET8(mcm_sms_set_msg_config_req_msg_v01, enable_cb_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sms_set_msg_config_req_msg_v01, enable_cb)
};

static const uint8_t mcm_sms_set_msg_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sms_set_msg_config_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * mcm_sms_get_msg_config_req_msg is empty
 * static const uint8_t mcm_sms_get_msg_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t mcm_sms_get_msg_config_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sms_get_msg_config_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sms_get_msg_config_resp_msg_v01, default_size_validation_mode) - QMI_IDL_OFFSET8(mcm_sms_get_msg_config_resp_msg_v01, default_size_validation_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sms_get_msg_config_resp_msg_v01, default_size_validation_mode),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sms_get_msg_config_resp_msg_v01, enable_cb) - QMI_IDL_OFFSET8(mcm_sms_get_msg_config_resp_msg_v01, enable_cb_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sms_get_msg_config_resp_msg_v01, enable_cb)
};

static const uint8_t mcm_sms_set_reception_mode_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sms_set_reception_mode_req_msg_v01, reception_mode),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sms_set_reception_mode_req_msg_v01, last_absorbed_message_id) - QMI_IDL_OFFSET8(mcm_sms_set_reception_mode_req_msg_v01, last_absorbed_message_id_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(mcm_sms_set_reception_mode_req_msg_v01, last_absorbed_message_id)
};

static const uint8_t mcm_sms_set_reception_mode_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sms_set_reception_mode_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_sms_event_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sms_event_register_req_msg_v01, register_sms_pp_event) - QMI_IDL_OFFSET8(mcm_sms_event_register_req_msg_v01, register_sms_pp_event_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sms_event_register_req_msg_v01, register_sms_pp_event)
};

static const uint8_t mcm_sms_event_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sms_event_register_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_sms_pp_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sms_pp_ind_msg_v01, message_format),

  0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_sms_pp_ind_msg_v01, message_content),
  MCM_SMS_MAX_MT_MSG_LENGTH_V01,

  0x03,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_sms_pp_ind_msg_v01, source_address),
  MCM_SMS_MAX_ADDR_LENGTH_V01,

  0x04,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_sms_pp_ind_msg_v01, message_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mcm_sms_pp_ind_msg_v01, message_class) - QMI_IDL_OFFSET16RELATIVE(mcm_sms_pp_ind_msg_v01, message_class_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_sms_pp_ind_msg_v01, message_class)
};

static const uint8_t mcm_sms_cb_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sms_cb_ind_msg_v01, message_format),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_sms_cb_ind_msg_v01, message_content),
  MCM_SMS_MAX_MT_MSG_LENGTH_V01
};

static const uint8_t mcm_sms_cb_cmas_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sms_cb_cmas_ind_msg_v01, type_0_record) - QMI_IDL_OFFSET8(mcm_sms_cb_cmas_ind_msg_v01, type_0_record_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sms_cb_cmas_ind_msg_v01, type_0_record),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sms_cb_cmas_ind_msg_v01, type_1_record) - QMI_IDL_OFFSET8(mcm_sms_cb_cmas_ind_msg_v01, type_1_record_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sms_cb_cmas_ind_msg_v01, type_1_record),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sms_cb_cmas_ind_msg_v01, type_2_record) - QMI_IDL_OFFSET8(mcm_sms_cb_cmas_ind_msg_v01, type_2_record_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sms_cb_cmas_ind_msg_v01, type_2_record),
  QMI_IDL_TYPE88(0, 3)
};

/* Type Table */
static const qmi_idl_type_table_entry  mcm_sms_type_table_v01[] = {
  {sizeof(mcm_cbs_cmae_expire_t_v01), mcm_cbs_cmae_expire_t_data_v01},
  {sizeof(mcm_cbs_cmae_record_type_0_t_v01), mcm_cbs_cmae_record_type_0_t_data_v01},
  {sizeof(mcm_cbs_cmae_record_type_1_t_v01), mcm_cbs_cmae_record_type_1_t_data_v01},
  {sizeof(mcm_cbs_cmae_record_type_2_t_v01), mcm_cbs_cmae_record_type_2_t_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry mcm_sms_message_table_v01[] = {
  {sizeof(mcm_sms_set_service_center_cfg_type_req_msg_v01), mcm_sms_set_service_center_cfg_type_req_msg_data_v01},
  {sizeof(mcm_sms_set_service_center_cfg_type_resp_msg_v01), mcm_sms_set_service_center_cfg_type_resp_msg_data_v01},
  {sizeof(mcm_sms_get_service_center_cfg_type_req_msg_v01), 0},
  {sizeof(mcm_sms_get_service_center_cfg_type_resp_msg_v01), mcm_sms_get_service_center_cfg_type_resp_msg_data_v01},
  {sizeof(mcm_sms_send_mo_msg_req_msg_v01), mcm_sms_send_mo_msg_req_msg_data_v01},
  {sizeof(mcm_sms_send_mo_msg_resp_msg_v01), mcm_sms_send_mo_msg_resp_msg_data_v01},
  {sizeof(mcm_sms_set_msg_config_req_msg_v01), mcm_sms_set_msg_config_req_msg_data_v01},
  {sizeof(mcm_sms_set_msg_config_resp_msg_v01), mcm_sms_set_msg_config_resp_msg_data_v01},
  {sizeof(mcm_sms_get_msg_config_req_msg_v01), 0},
  {sizeof(mcm_sms_get_msg_config_resp_msg_v01), mcm_sms_get_msg_config_resp_msg_data_v01},
  {sizeof(mcm_sms_set_reception_mode_req_msg_v01), mcm_sms_set_reception_mode_req_msg_data_v01},
  {sizeof(mcm_sms_set_reception_mode_resp_msg_v01), mcm_sms_set_reception_mode_resp_msg_data_v01},
  {sizeof(mcm_sms_event_register_req_msg_v01), mcm_sms_event_register_req_msg_data_v01},
  {sizeof(mcm_sms_event_register_resp_msg_v01), mcm_sms_event_register_resp_msg_data_v01},
  {sizeof(mcm_sms_pp_ind_msg_v01), mcm_sms_pp_ind_msg_data_v01},
  {sizeof(mcm_sms_cb_ind_msg_v01), mcm_sms_cb_ind_msg_data_v01},
  {sizeof(mcm_sms_cb_cmas_ind_msg_v01), mcm_sms_cb_cmas_ind_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
const qmi_idl_type_table_object mcm_sms_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
const qmi_idl_type_table_object *mcm_sms_qmi_idl_type_table_object_referenced_tables_v01[] =
{&mcm_sms_qmi_idl_type_table_object_v01, &mcm_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
const qmi_idl_type_table_object mcm_sms_qmi_idl_type_table_object_v01 = {
  sizeof(mcm_sms_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(mcm_sms_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  mcm_sms_type_table_v01,
  mcm_sms_message_table_v01,
  mcm_sms_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

