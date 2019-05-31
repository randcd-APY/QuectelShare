/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        Q T I _ W L A N _ S C M _ M S G R _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the scm_msgr service Data structures.

  Copyright (c) 2016,2018 Qualcomm Technologies, Inc.  All Rights Reserved. 
 Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //source/qcom/qct/interfaces/qmi/rel/deploy/scm_msgr/src/qti_wlan_scm_msgr_v01.c#5 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Fri Apr 13 2018 (Spin 0)
   From IDL File: qti_wlan_scm_msgr_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "qti_wlan_scm_msgr_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t scm_msgr_wlan_if_data_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_if_data_v01, wlan_if_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_if_data_v01, wlan_if_num),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_if_data_v01, wlan_dev_type),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t scm_msgr_wlan_cfg_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_cfg_v01, wlan_if_type),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_cfg_v01, conf_file),
  SCM_MSGR_WLAN_CONF_FILE_PATH_MAX_V01,

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_cfg_v01, pid_file),
  SCM_MSGR_WLAN_CONF_FILE_PATH_MAX_V01,

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_cfg_v01, entropy_file),
  SCM_MSGR_WLAN_CONF_FILE_PATH_MAX_V01,

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(scm_msgr_wlan_cfg_v01, extra_cmd_line_args),
  SCM_MSGR_WLAN_CMD_LINE_ARG_MAX_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t scm_msgr_wlan_dyn_ind_notify_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_dyn_ind_notify_v01, wlan_if_control),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_dyn_ind_notify_v01, wlan_if_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_dyn_ind_notify_v01, wlan_if_num),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_dyn_ind_notify_v01, wlan_dev_type),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t scm_msgr_wlan_sta_con_notify_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_sta_con_notify_v01, wlan_sta_status),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_sta_con_notify_v01, wlan_if_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_sta_con_notify_v01, wlan_if_num),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_sta_con_notify_v01, wlan_dev_type),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t scm_msgr_wlan_enable_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_enable_req_msg_v01, wlan_concurrency_cfg)
};

static const uint8_t scm_msgr_wlan_enable_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_enable_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(scm_msgr_wlan_enable_resp_msg_v01, wlan_concurrency_cfg) - QMI_IDL_OFFSET8(scm_msgr_wlan_enable_resp_msg_v01, wlan_concurrency_cfg_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_enable_resp_msg_v01, wlan_concurrency_cfg),
  SCM_MSGR_WLAN_CONCURRENT_IF_MAX_V01,
  QMI_IDL_OFFSET8(scm_msgr_wlan_enable_resp_msg_v01, wlan_concurrency_cfg) - QMI_IDL_OFFSET8(scm_msgr_wlan_enable_resp_msg_v01, wlan_concurrency_cfg_len),
  QMI_IDL_TYPE88(0, 0)
};

/*
 * scm_msgr_wlan_disable_req_msg is empty
 * static const uint8_t scm_msgr_wlan_disable_req_msg_data_v01[] = {
 * };
 */

static const uint8_t scm_msgr_wlan_disable_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_disable_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t scm_msgr_wlan_if_reconfig_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_if_reconfig_req_msg_v01, wlan_if_type),
  SCM_MSGR_WLAN_CONCURRENT_IF_MAX_V01,
  QMI_IDL_OFFSET8(scm_msgr_wlan_if_reconfig_req_msg_v01, wlan_if_type) - QMI_IDL_OFFSET8(scm_msgr_wlan_if_reconfig_req_msg_v01, wlan_if_type_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_if_reconfig_req_msg_v01, wlan_if_control),
  SCM_MSGR_WLAN_CONCURRENT_IF_MAX_V01,
  QMI_IDL_OFFSET8(scm_msgr_wlan_if_reconfig_req_msg_v01, wlan_if_control) - QMI_IDL_OFFSET8(scm_msgr_wlan_if_reconfig_req_msg_v01, wlan_if_control_len)
};

static const uint8_t scm_msgr_wlan_if_reconfig_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_if_reconfig_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(scm_msgr_wlan_if_reconfig_resp_msg_v01, wlan_concurrency_cfg) - QMI_IDL_OFFSET8(scm_msgr_wlan_if_reconfig_resp_msg_v01, wlan_concurrency_cfg_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_if_reconfig_resp_msg_v01, wlan_concurrency_cfg),
  SCM_MSGR_WLAN_CONCURRENT_IF_MAX_V01,
  QMI_IDL_OFFSET8(scm_msgr_wlan_if_reconfig_resp_msg_v01, wlan_concurrency_cfg) - QMI_IDL_OFFSET8(scm_msgr_wlan_if_reconfig_resp_msg_v01, wlan_concurrency_cfg_len),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t scm_msgr_wlan_setup_ind_register_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_setup_ind_register_req_msg_v01, register_indication),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(scm_msgr_wlan_setup_ind_register_req_msg_v01, wlan_cfg) - QMI_IDL_OFFSET8(scm_msgr_wlan_setup_ind_register_req_msg_v01, wlan_cfg_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_setup_ind_register_req_msg_v01, wlan_cfg),
  SCM_MSGR_WLAN_IF_TYPES_MAX_V01,
  QMI_IDL_OFFSET8(scm_msgr_wlan_setup_ind_register_req_msg_v01, wlan_cfg) - QMI_IDL_OFFSET8(scm_msgr_wlan_setup_ind_register_req_msg_v01, wlan_cfg_len),
  QMI_IDL_TYPE88(0, 1)
};

static const uint8_t scm_msgr_wlan_setup_ind_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_setup_ind_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t scm_msgr_wlan_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_ind_msg_v01, wlan_ind_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(scm_msgr_wlan_ind_msg_v01, wlan_dyn_ind) - QMI_IDL_OFFSET8(scm_msgr_wlan_ind_msg_v01, wlan_dyn_ind_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_ind_msg_v01, wlan_dyn_ind),
  SCM_MSGR_WLAN_CONCURRENT_IF_MAX_V01,
  QMI_IDL_OFFSET8(scm_msgr_wlan_ind_msg_v01, wlan_dyn_ind) - QMI_IDL_OFFSET8(scm_msgr_wlan_ind_msg_v01, wlan_dyn_ind_len),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(scm_msgr_wlan_ind_msg_v01, wlan_sta_ind) - QMI_IDL_OFFSET8(scm_msgr_wlan_ind_msg_v01, wlan_sta_ind_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_ind_msg_v01, wlan_sta_ind),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(scm_msgr_wlan_ind_msg_v01, wlan_sys_control) - QMI_IDL_OFFSET8(scm_msgr_wlan_ind_msg_v01, wlan_sys_control_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(scm_msgr_wlan_ind_msg_v01, wlan_sys_control)
};

/* Type Table */
static const qmi_idl_type_table_entry  scm_msgr_type_table_v01[] = {
  {sizeof(scm_msgr_wlan_if_data_v01), scm_msgr_wlan_if_data_data_v01},
  {sizeof(scm_msgr_wlan_cfg_v01), scm_msgr_wlan_cfg_data_v01},
  {sizeof(scm_msgr_wlan_dyn_ind_notify_v01), scm_msgr_wlan_dyn_ind_notify_data_v01},
  {sizeof(scm_msgr_wlan_sta_con_notify_v01), scm_msgr_wlan_sta_con_notify_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry scm_msgr_message_table_v01[] = {
  {sizeof(scm_msgr_wlan_enable_req_msg_v01), scm_msgr_wlan_enable_req_msg_data_v01},
  {sizeof(scm_msgr_wlan_enable_resp_msg_v01), scm_msgr_wlan_enable_resp_msg_data_v01},
  {sizeof(scm_msgr_wlan_disable_req_msg_v01), 0},
  {sizeof(scm_msgr_wlan_disable_resp_msg_v01), scm_msgr_wlan_disable_resp_msg_data_v01},
  {sizeof(scm_msgr_wlan_if_reconfig_req_msg_v01), scm_msgr_wlan_if_reconfig_req_msg_data_v01},
  {sizeof(scm_msgr_wlan_if_reconfig_resp_msg_v01), scm_msgr_wlan_if_reconfig_resp_msg_data_v01},
  {sizeof(scm_msgr_wlan_setup_ind_register_req_msg_v01), scm_msgr_wlan_setup_ind_register_req_msg_data_v01},
  {sizeof(scm_msgr_wlan_setup_ind_register_resp_msg_v01), scm_msgr_wlan_setup_ind_register_resp_msg_data_v01},
  {sizeof(scm_msgr_wlan_ind_msg_v01), scm_msgr_wlan_ind_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object scm_msgr_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *scm_msgr_qmi_idl_type_table_object_referenced_tables_v01[] =
{&scm_msgr_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object scm_msgr_qmi_idl_type_table_object_v01 = {
  sizeof(scm_msgr_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(scm_msgr_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  scm_msgr_type_table_v01,
  scm_msgr_message_table_v01,
  scm_msgr_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry scm_msgr_service_command_messages_v01[] = {
  {QMI_SCM_MSGR_WLAN_ENABLE_REQ_V01, QMI_IDL_TYPE16(0, 0), 7},
  {QMI_SCM_MSGR_WLAN_DISABLE_REQ_V01, QMI_IDL_TYPE16(0, 2), 0},
  {QMI_SCM_MSGR_WLAN_IF_RECONFIG_REQ_V01, QMI_IDL_TYPE16(0, 4), 32},
  {QMI_SCM_MSGR_WLAN_SETUP_IND_REG_REQ_V01, QMI_IDL_TYPE16(0, 6), 2416}
};

static const qmi_idl_service_message_table_entry scm_msgr_service_response_messages_v01[] = {
  {QMI_SCM_MSGR_WLAN_ENABLE_RESP_V01, QMI_IDL_TYPE16(0, 1), 38},
  {QMI_SCM_MSGR_WLAN_DISABLE_RESP_V01, QMI_IDL_TYPE16(0, 3), 7},
  {QMI_SCM_MSGR_WLAN_IF_RECONFIG_RESP_V01, QMI_IDL_TYPE16(0, 5), 38},
  {QMI_SCM_MSGR_WLAN_SETUP_IND_REG_RESP_V01, QMI_IDL_TYPE16(0, 7), 7}
};

static const qmi_idl_service_message_table_entry scm_msgr_service_indication_messages_v01[] = {
  {QMI_SCM_MSGR_WLAN_IND_V01, QMI_IDL_TYPE16(0, 8), 73}
};

/*Service Object*/
struct qmi_idl_service_object scm_msgr_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x41b,
  2416,
  { sizeof(scm_msgr_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(scm_msgr_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(scm_msgr_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { scm_msgr_service_command_messages_v01, scm_msgr_service_response_messages_v01, scm_msgr_service_indication_messages_v01},
  &scm_msgr_qmi_idl_type_table_object_v01,
  0x03,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type scm_msgr_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( SCM_MSGR_V01_IDL_MAJOR_VERS != idl_maj_version || SCM_MSGR_V01_IDL_MINOR_VERS != idl_min_version
       || SCM_MSGR_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&scm_msgr_qmi_idl_service_object_v01;
}

