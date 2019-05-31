/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        C O N T R O L _ S E R V I C E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the ctl service Data structures.

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.

  $Header: //source/qcom/qct/interfaces/qmi/ctl/main/latest/src/control_service_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 5.1
   It requires encode/decode library version 4 or later
   It was generated on: Fri Mar 16 2012
   From IDL File: control_service_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "control_service_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t ctl_service_version_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ctl_service_version_type_v01, qmi_svc_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ctl_service_version_type_v01, major_ver),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ctl_service_version_type_v01, minor_ver),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ctl_addendum_version_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ctl_addendum_version_type_v01, qmi_svc_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ctl_addendum_version_type_v01, addendum_major_ver),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ctl_addendum_version_type_v01, addendum_minor_ver),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ctl_addendum_ver_list_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_STRING,
  QMI_IDL_OFFSET8(ctl_addendum_ver_list_type_v01, addendum_label),
  QMI_CTL_MAX_ADDENDUM_LABEL_V01,

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ctl_addendum_ver_list_type_v01, addendum_version_list),
  QMI_CTL_MAX_SERVICES_V01,
  QMI_IDL_OFFSET16RELATIVE(ctl_addendum_ver_list_type_v01, addendum_version_list) - QMI_IDL_OFFSET16RELATIVE(ctl_addendum_ver_list_type_v01, addendum_version_list_len),
 1, 0,
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ctl_svc_clid_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ctl_svc_clid_type_v01, qmi_svc_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ctl_svc_clid_type_v01, client_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ctl_pwr_save_state_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ctl_pwr_save_state_type_v01, current_state),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ctl_pwr_save_state_type_v01, previous_state),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t ctl_pwr_save_desc_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ctl_pwr_save_desc_type_v01, pwrsave_state),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ctl_pwr_save_desc_type_v01, qmi_service),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t ctl_set_instance_id_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ctl_set_instance_id_req_msg_v01, host_driver_instance)
};

static const uint8_t ctl_set_instance_id_resp_msg_data_v01[] = {
  0x01,
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ctl_set_instance_id_resp_msg_v01, qmi_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ctl_set_instance_id_resp_msg_v01, resp),
  0, 1
};

/*
 * ctl_get_version_info_req_msg is empty
 * static const uint8_t ctl_get_version_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ctl_get_version_info_resp_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ctl_get_version_info_resp_msg_v01, service_version_list),
  QMI_CTL_MAX_SERVICES_V01,
  QMI_IDL_OFFSET8(ctl_get_version_info_resp_msg_v01, service_version_list) - QMI_IDL_OFFSET8(ctl_get_version_info_resp_msg_v01, service_version_list_len),
  0, 0,

  0x02,
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ctl_get_version_info_resp_msg_v01, resp),
  0, 1,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(ctl_get_version_info_resp_msg_v01, addendum_ver_list) - QMI_IDL_OFFSET16RELATIVE(ctl_get_version_info_resp_msg_v01, addendum_ver_list_valid)),
  0x10,
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(ctl_get_version_info_resp_msg_v01, addendum_ver_list),
  2, 0
};

static const uint8_t ctl_get_client_id_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ctl_get_client_id_req_msg_v01, qmi_svc_type)
};

static const uint8_t ctl_get_client_id_resp_msg_data_v01[] = {
  0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ctl_get_client_id_resp_msg_v01, svc_client_id),
  3, 0,

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ctl_get_client_id_resp_msg_v01, resp),
  0, 1
};

static const uint8_t ctl_release_client_id_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ctl_release_client_id_req_msg_v01, svc_client_id),
  3, 0
};

static const uint8_t ctl_release_client_id_resp_msg_data_v01[] = {
  0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ctl_release_client_id_resp_msg_v01, svc_client_id),
  3, 0,

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ctl_release_client_id_resp_msg_v01, resp),
  0, 1
};

static const uint8_t ctl_revoke_client_id_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ctl_revoke_client_id_ind_msg_v01, svc_client_id),
  3, 0
};

static const uint8_t ctl_invalid_client_id_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ctl_invalid_client_id_ind_msg_v01, svc_client_id),
  3, 0
};

static const uint8_t ctl_set_data_format_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ctl_set_data_format_req_msg_v01, data_format),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ctl_set_data_format_req_msg_v01, link_prot) - QMI_IDL_OFFSET8(ctl_set_data_format_req_msg_v01, link_prot_valid)),
  0x10,
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ctl_set_data_format_req_msg_v01, link_prot),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ctl_set_data_format_req_msg_v01, ul_data_agg_setting) - QMI_IDL_OFFSET8(ctl_set_data_format_req_msg_v01, ul_data_agg_setting_valid)),
  0x11,
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ctl_set_data_format_req_msg_v01, ul_data_agg_setting)
};

static const uint8_t ctl_set_data_format_resp_msg_data_v01[] = {
  0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ctl_set_data_format_resp_msg_v01, resp),
  0, 1,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ctl_set_data_format_resp_msg_v01, link_prot) - QMI_IDL_OFFSET8(ctl_set_data_format_resp_msg_v01, link_prot_valid)),
  0x10,
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(ctl_set_data_format_resp_msg_v01, link_prot),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ctl_set_data_format_resp_msg_v01, ul_data_agg_setting) - QMI_IDL_OFFSET8(ctl_set_data_format_resp_msg_v01, ul_data_agg_setting_valid)),
  0x11,
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(ctl_set_data_format_resp_msg_v01, ul_data_agg_setting)
};

/*
 * ctl_sync_req_msg is empty
 * static const uint8_t ctl_sync_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ctl_sync_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ctl_sync_resp_msg_v01, resp),
  0, 1
};

/*
 * ctl_sync_ind_msg is empty
 * static const uint8_t ctl_sync_ind_msg_data_v01[] = {
 * };
 */

static const uint8_t ctl_reg_pwr_save_mode_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(ctl_reg_pwr_save_mode_req_msg_v01, report_pwrsnormal_state)
};

static const uint8_t ctl_reg_pwr_save_mode_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ctl_reg_pwr_save_mode_resp_msg_v01, resp),
  0, 1
};

static const uint8_t ctl_pwr_save_mode_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ctl_pwr_save_mode_ind_msg_v01, pwr_save_state) - QMI_IDL_OFFSET8(ctl_pwr_save_mode_ind_msg_v01, pwr_save_state_valid)),
  0x10,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ctl_pwr_save_mode_ind_msg_v01, pwr_save_state),
  4, 0
};

static const uint8_t ctl_config_pwr_save_settings_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ctl_config_pwr_save_settings_req_msg_v01, pwr_save_desc),
  5, 0,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(ctl_config_pwr_save_settings_req_msg_v01, indication_set) - QMI_IDL_OFFSET8(ctl_config_pwr_save_settings_req_msg_v01, indication_set_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_FIRST_EXTENDED |   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_FLAGS_ARRAY_IS_LENGTHLESS,
  QMI_IDL_OFFSET8(ctl_config_pwr_save_settings_req_msg_v01, indication_set),
  QMI_CTL_MAX_IND_SET_V01,
  QMI_IDL_OFFSET8(ctl_config_pwr_save_settings_req_msg_v01, indication_set) - QMI_IDL_OFFSET8(ctl_config_pwr_save_settings_req_msg_v01, indication_set_len)
};

static const uint8_t ctl_config_pwr_save_settings_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ctl_config_pwr_save_settings_resp_msg_v01, resp),
  0, 1
};

static const uint8_t ctl_set_pwr_save_mode_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ctl_set_pwr_save_mode_req_msg_v01, pwrsave_state)
};

static const uint8_t ctl_set_pwr_save_mode_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ctl_set_pwr_save_mode_resp_msg_v01, resp),
  0, 1
};

/*
 * ctl_get_pwr_save_mode_req_msg is empty
 * static const uint8_t ctl_get_pwr_save_mode_req_msg_data_v01[] = {
 * };
 */

static const uint8_t ctl_get_pwr_save_mode_resp_msg_data_v01[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(ctl_get_pwr_save_mode_resp_msg_v01, pwrsave_state),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(ctl_get_pwr_save_mode_resp_msg_v01, resp),
  0, 1
};

/* Type Table */
static const qmi_idl_type_table_entry  ctl_type_table_v01[] = {
  {sizeof(ctl_service_version_type_v01), ctl_service_version_type_data_v01},
  {sizeof(ctl_addendum_version_type_v01), ctl_addendum_version_type_data_v01},
  {sizeof(ctl_addendum_ver_list_type_v01), ctl_addendum_ver_list_type_data_v01},
  {sizeof(ctl_svc_clid_type_v01), ctl_svc_clid_type_data_v01},
  {sizeof(ctl_pwr_save_state_type_v01), ctl_pwr_save_state_type_data_v01},
  {sizeof(ctl_pwr_save_desc_type_v01), ctl_pwr_save_desc_type_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry ctl_message_table_v01[] = {
  {sizeof(ctl_set_instance_id_req_msg_v01), ctl_set_instance_id_req_msg_data_v01},
  {sizeof(ctl_set_instance_id_resp_msg_v01), ctl_set_instance_id_resp_msg_data_v01},
  {0, 0},
  {sizeof(ctl_get_version_info_resp_msg_v01), ctl_get_version_info_resp_msg_data_v01},
  {sizeof(ctl_get_client_id_req_msg_v01), ctl_get_client_id_req_msg_data_v01},
  {sizeof(ctl_get_client_id_resp_msg_v01), ctl_get_client_id_resp_msg_data_v01},
  {sizeof(ctl_release_client_id_req_msg_v01), ctl_release_client_id_req_msg_data_v01},
  {sizeof(ctl_release_client_id_resp_msg_v01), ctl_release_client_id_resp_msg_data_v01},
  {sizeof(ctl_revoke_client_id_ind_msg_v01), ctl_revoke_client_id_ind_msg_data_v01},
  {sizeof(ctl_invalid_client_id_ind_msg_v01), ctl_invalid_client_id_ind_msg_data_v01},
  {sizeof(ctl_set_data_format_req_msg_v01), ctl_set_data_format_req_msg_data_v01},
  {sizeof(ctl_set_data_format_resp_msg_v01), ctl_set_data_format_resp_msg_data_v01},
  {0, 0},
  {sizeof(ctl_sync_resp_msg_v01), ctl_sync_resp_msg_data_v01},
  {0, 0},
  {sizeof(ctl_reg_pwr_save_mode_req_msg_v01), ctl_reg_pwr_save_mode_req_msg_data_v01},
  {sizeof(ctl_reg_pwr_save_mode_resp_msg_v01), ctl_reg_pwr_save_mode_resp_msg_data_v01},
  {sizeof(ctl_pwr_save_mode_ind_msg_v01), ctl_pwr_save_mode_ind_msg_data_v01},
  {sizeof(ctl_config_pwr_save_settings_req_msg_v01), ctl_config_pwr_save_settings_req_msg_data_v01},
  {sizeof(ctl_config_pwr_save_settings_resp_msg_v01), ctl_config_pwr_save_settings_resp_msg_data_v01},
  {sizeof(ctl_set_pwr_save_mode_req_msg_v01), ctl_set_pwr_save_mode_req_msg_data_v01},
  {sizeof(ctl_set_pwr_save_mode_resp_msg_v01), ctl_set_pwr_save_mode_resp_msg_data_v01},
  {0, 0},
  {sizeof(ctl_get_pwr_save_mode_resp_msg_v01), ctl_get_pwr_save_mode_resp_msg_data_v01}
};

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object ctl_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *ctl_qmi_idl_type_table_object_referenced_tables_v01[] =
{&ctl_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object ctl_qmi_idl_type_table_object_v01 = {
  sizeof(ctl_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(ctl_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  ctl_type_table_v01,
  ctl_message_table_v01,
  ctl_qmi_idl_type_table_object_referenced_tables_v01
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry ctl_service_command_messages_v01[] = {
  {QMI_CTL_SET_INSTANCE_ID_REQ_V01, TYPE16(0, 0), 4},
  {QMI_CTL_GET_VERSION_INFO_REQ_V01, TYPE16(0, 2), 0},
  {QMI_CTL_GET_CLIENT_ID_REQ_V01, TYPE16(0, 4), 4},
  {QMI_CTL_RELEASE_CLIENT_ID_REQ_V01, TYPE16(0, 6), 5},
  {QMI_CTL_SET_DATA_FORMAT_REQ_V01, TYPE16(0, 10), 13},
  {QMI_CTL_SYNC_REQ_V01, TYPE16(0, 12), 0},
  {QMI_CTL_REG_PWR_SAVE_MODE_REQ_V01, TYPE16(0, 15), 4},
  {QMI_CTL_CONFIG_PWR_SAVE_SETTINGS_REQ_V01, TYPE16(0, 18), 32},
  {QMI_CTL_SET_PWR_SAVE_MODE_REQ_V01, TYPE16(0, 20), 7},
  {QMI_CTL_GET_PWR_SAVE_MODE_REQ_V01, TYPE16(0, 22), 0}
};

static const qmi_idl_service_message_table_entry ctl_service_response_messages_v01[] = {
  {QMI_CTL_SET_INSTANCE_ID_RESP_V01, TYPE16(0, 1), 12},
  {QMI_CTL_GET_VERSION_INFO_RESP_V01, TYPE16(0, 3), 2821},
  {QMI_CTL_GET_CLIENT_ID_RESP_V01, TYPE16(0, 5), 12},
  {QMI_CTL_RELEASE_CLIENT_ID_RESP_V01, TYPE16(0, 7), 12},
  {QMI_CTL_SET_DATA_FORMAT_RESP_V01, TYPE16(0, 11), 16},
  {QMI_CTL_SYNC_RESP_V01, TYPE16(0, 13), 7},
  {QMI_CTL_REG_PWR_SAVE_MODE_RESP_V01, TYPE16(0, 16), 7},
  {QMI_CTL_CONFIG_PWR_SAVE_SETTINGS_RESP_V01, TYPE16(0, 19), 7},
  {QMI_CTL_SET_PWR_SAVE_MODE_RESP_V01, TYPE16(0, 21), 7},
  {QMI_CTL_GET_PWR_SAVE_MODE_RESP_V01, TYPE16(0, 23), 14}
};

static const qmi_idl_service_message_table_entry ctl_service_indication_messages_v01[] = {
  {QMI_CTL_REVOKE_CLIENT_ID_IND_V01, TYPE16(0, 8), 5},
  {QMI_CTL_INVALID_CLIENT_ID_IND_V01, TYPE16(0, 9), 5},
  {QMI_CTL_SYNC_IND_V01, TYPE16(0, 14), 0},
  {QMI_CTL_PWR_SAVE_MODE_IND_V01, TYPE16(0, 17), 11}
};

/*Service Object*/
const struct qmi_idl_service_object ctl_qmi_idl_service_object_v01 = {
  0x04,
  0x01,
  0x00,
  2821,
  { sizeof(ctl_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(ctl_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(ctl_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { ctl_service_command_messages_v01, ctl_service_response_messages_v01, ctl_service_indication_messages_v01},
  &ctl_qmi_idl_type_table_object_v01
};

/* Service Object Accessor */
qmi_idl_service_object_type ctl_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( CTL_V01_IDL_MAJOR_VERS != idl_maj_version || CTL_V01_IDL_MINOR_VERS != idl_min_version
       || CTL_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&ctl_qmi_idl_service_object_v01;
}

