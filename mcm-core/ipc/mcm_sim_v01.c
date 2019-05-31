/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M C M _ S I M _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the mcm_sim service Data structures.

  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.2
   It was generated on: Tue Dec  3 2013 (Spin 0)
   From IDL File: mcm_sim_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "mcm_sim_v01.h"
#include "mcm_common_v01.h"
#include "mcm_service_object_v01.h"


/*Type Definitions*/
static const uint8_t mcm_sim_application_identification_info_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_application_identification_info_t_v01, slot_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_application_identification_info_t_v01, app_t),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_sim_plmn_t_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_plmn_t_v01, mcc),
  MCM_SIM_MCC_LEN_V01,

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_plmn_t_v01, mnc),
  MCM_SIM_MNC_MAX_V01,
  QMI_IDL_OFFSET8(mcm_sim_plmn_t_v01, mnc) - QMI_IDL_OFFSET8(mcm_sim_plmn_t_v01, mnc_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_sim_file_access_t_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_file_access_t_v01, offset),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_file_access_t_v01, record_num),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_file_access_t_v01, path),
  MCM_SIM_CHAR_PATH_MAX_V01,
  QMI_IDL_OFFSET8(mcm_sim_file_access_t_v01, path) - QMI_IDL_OFFSET8(mcm_sim_file_access_t_v01, path_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_sim_card_result_t_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_card_result_t_v01, sw1),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_card_result_t_v01, sw2),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_sim_file_info_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_file_info_t_v01, file_t),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_file_info_t_v01, file_size),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_file_info_t_v01, record_size),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_file_info_t_v01, record_count),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_sim_depersonalization_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_depersonalization_t_v01, feature),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_depersonalization_t_v01, operation),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_depersonalization_t_v01, ck_value),
  MCM_SIM_CK_MAX_V01,
  QMI_IDL_OFFSET8(mcm_sim_depersonalization_t_v01, ck_value) - QMI_IDL_OFFSET8(mcm_sim_depersonalization_t_v01, ck_value_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_sim_network_perso_t_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_network_perso_t_v01, mcc),
  MCM_SIM_MCC_LEN_V01,

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_network_perso_t_v01, mnc),
  MCM_SIM_MNC_MAX_V01,
  QMI_IDL_OFFSET8(mcm_sim_network_perso_t_v01, mnc) - QMI_IDL_OFFSET8(mcm_sim_network_perso_t_v01, mnc_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_sim_gw_network_subset_perso_t_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_gw_network_subset_perso_t_v01, network),
  QMI_IDL_TYPE88(0, 6),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_gw_network_subset_perso_t_v01, digit6),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_gw_network_subset_perso_t_v01, digit7),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_sim_gw_sp_perso_t_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_gw_sp_perso_t_v01, network),
  QMI_IDL_TYPE88(0, 6),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_gw_sp_perso_t_v01, gid1),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_sim_gw_corporate_perso_t_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_gw_corporate_perso_t_v01, network),
  QMI_IDL_TYPE88(0, 6),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_gw_corporate_perso_t_v01, gid1),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_gw_corporate_perso_t_v01, gid2),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_sim_sim_perso_t_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_sim_perso_t_v01, network),
  QMI_IDL_TYPE88(0, 6),
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_sim_perso_t_v01, msin),
  MCM_SIM_MSIN_MAX_V01,
  QMI_IDL_OFFSET8(mcm_sim_sim_perso_t_v01, msin) - QMI_IDL_OFFSET8(mcm_sim_sim_perso_t_v01, msin_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_sim_1x_network_type2_perso_t_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_1x_network_type2_perso_t_v01, irm_code),
  MCM_SIM_IRM_CODE_LEN_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_sim_perso_retries_left_t_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_perso_retries_left_t_v01, verify_left),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_perso_retries_left_t_v01, unblock_left),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_sim_app_info_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_app_info_t_v01, subscription),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_app_info_t_v01, app_state),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_app_info_t_v01, perso_feature),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_app_info_t_v01, perso_retries),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_app_info_t_v01, perso_unblock_retries),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_app_info_t_v01, pin1_state),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_app_info_t_v01, pin1_num_retries),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_app_info_t_v01, puk1_num_retries),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_app_info_t_v01, pin2_state),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_app_info_t_v01, pin2_num_retries),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_app_info_t_v01, puk2_num_retries),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_sim_card_app_info_t_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_card_app_info_t_v01, app_3gpp),
  QMI_IDL_TYPE88(0, 13),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_card_app_info_t_v01, app_3gpp2),
  QMI_IDL_TYPE88(0, 13),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_card_app_info_t_v01, app_isim),
  QMI_IDL_TYPE88(0, 13),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_sim_card_info_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_card_info_t_v01, card_state),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_card_info_t_v01, card_t),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_card_info_t_v01, card_app_info),
  QMI_IDL_TYPE88(0, 14),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_sim_refresh_file_list_t_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_refresh_file_list_t_v01, path_value),
  MCM_SIM_CHAR_PATH_MAX_V01,
  QMI_IDL_OFFSET8(mcm_sim_refresh_file_list_t_v01, path_value) - QMI_IDL_OFFSET8(mcm_sim_refresh_file_list_t_v01, path_value_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_sim_refresh_event_t_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_refresh_event_t_v01, app_info),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_refresh_event_t_v01, refresh_mode),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_refresh_event_t_v01, refresh_files),
  MCM_SIM_MAX_REFRESH_FILES_V01,
  QMI_IDL_OFFSET8(mcm_sim_refresh_event_t_v01, refresh_files) - QMI_IDL_OFFSET8(mcm_sim_refresh_event_t_v01, refresh_files_len),
  QMI_IDL_TYPE88(0, 16),
  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t mcm_sim_get_subscriber_id_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_get_subscriber_id_req_msg_v01, app_info),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t mcm_sim_get_subscriber_id_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_get_subscriber_id_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sim_get_subscriber_id_resp_msg_v01, imsi) - QMI_IDL_OFFSET8(mcm_sim_get_subscriber_id_resp_msg_v01, imsi_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_get_subscriber_id_resp_msg_v01, imsi),
  MCM_SIM_IMSI_LEN_V01,
  QMI_IDL_OFFSET8(mcm_sim_get_subscriber_id_resp_msg_v01, imsi) - QMI_IDL_OFFSET8(mcm_sim_get_subscriber_id_resp_msg_v01, imsi_len)
};

static const uint8_t mcm_sim_get_card_id_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_get_card_id_req_msg_v01, slot_id)
};

static const uint8_t mcm_sim_get_card_id_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_get_card_id_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sim_get_card_id_resp_msg_v01, iccid) - QMI_IDL_OFFSET8(mcm_sim_get_card_id_resp_msg_v01, iccid_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_get_card_id_resp_msg_v01, iccid),
  MCM_SIM_ICCID_LEN_V01,
  QMI_IDL_OFFSET8(mcm_sim_get_card_id_resp_msg_v01, iccid) - QMI_IDL_OFFSET8(mcm_sim_get_card_id_resp_msg_v01, iccid_len)
};

static const uint8_t mcm_sim_get_device_phone_number_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_get_device_phone_number_req_msg_v01, app_info),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t mcm_sim_get_device_phone_number_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_get_device_phone_number_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sim_get_device_phone_number_resp_msg_v01, phone_number) - QMI_IDL_OFFSET8(mcm_sim_get_device_phone_number_resp_msg_v01, phone_number_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_get_device_phone_number_resp_msg_v01, phone_number),
  MCM_SIM_PHONE_NUMBER_MAX_V01,
  QMI_IDL_OFFSET8(mcm_sim_get_device_phone_number_resp_msg_v01, phone_number) - QMI_IDL_OFFSET8(mcm_sim_get_device_phone_number_resp_msg_v01, phone_number_len)
};

static const uint8_t mcm_sim_get_preferred_operator_list_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_get_preferred_operator_list_req_msg_v01, slot_id)
};

static const uint8_t mcm_sim_get_preferred_operator_list_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_get_preferred_operator_list_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sim_get_preferred_operator_list_resp_msg_v01, preferred_operator_list) - QMI_IDL_OFFSET8(mcm_sim_get_preferred_operator_list_resp_msg_v01, preferred_operator_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_get_preferred_operator_list_resp_msg_v01, preferred_operator_list),
  MCM_SIM_NUM_PLMN_MAX_V01,
  QMI_IDL_OFFSET8(mcm_sim_get_preferred_operator_list_resp_msg_v01, preferred_operator_list) - QMI_IDL_OFFSET8(mcm_sim_get_preferred_operator_list_resp_msg_v01, preferred_operator_list_len),
  QMI_IDL_TYPE88(0, 1)
};

static const uint8_t mcm_sim_read_file_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_read_file_req_msg_v01, app_info),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_read_file_req_msg_v01, file_access),
  QMI_IDL_TYPE88(0, 2)
};

static const uint8_t mcm_sim_read_file_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_read_file_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sim_read_file_resp_msg_v01, card_result) - QMI_IDL_OFFSET8(mcm_sim_read_file_resp_msg_v01, card_result_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_read_file_resp_msg_v01, card_result),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sim_read_file_resp_msg_v01, data) - QMI_IDL_OFFSET8(mcm_sim_read_file_resp_msg_v01, data_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_read_file_resp_msg_v01, data),
  ((MCM_SIM_DATA_MAX_V01) & 0xFF), ((MCM_SIM_DATA_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(mcm_sim_read_file_resp_msg_v01, data) - QMI_IDL_OFFSET8(mcm_sim_read_file_resp_msg_v01, data_len)
};

static const uint8_t mcm_sim_write_file_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_write_file_req_msg_v01, app_info),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_write_file_req_msg_v01, file_access),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_write_file_req_msg_v01, data),
  ((MCM_SIM_DATA_MAX_V01) & 0xFF), ((MCM_SIM_DATA_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(mcm_sim_write_file_req_msg_v01, data) - QMI_IDL_OFFSET8(mcm_sim_write_file_req_msg_v01, data_len)
};

static const uint8_t mcm_sim_write_file_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_write_file_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sim_write_file_resp_msg_v01, card_result) - QMI_IDL_OFFSET8(mcm_sim_write_file_resp_msg_v01, card_result_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_write_file_resp_msg_v01, card_result),
  QMI_IDL_TYPE88(0, 3)
};

static const uint8_t mcm_sim_get_file_size_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_get_file_size_req_msg_v01, app_info),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_get_file_size_req_msg_v01, path),
  MCM_SIM_CHAR_PATH_MAX_V01,
  QMI_IDL_OFFSET8(mcm_sim_get_file_size_req_msg_v01, path) - QMI_IDL_OFFSET8(mcm_sim_get_file_size_req_msg_v01, path_len)
};

static const uint8_t mcm_sim_get_file_size_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_get_file_size_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sim_get_file_size_resp_msg_v01, card_result) - QMI_IDL_OFFSET8(mcm_sim_get_file_size_resp_msg_v01, card_result_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_get_file_size_resp_msg_v01, card_result),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sim_get_file_size_resp_msg_v01, file_info) - QMI_IDL_OFFSET8(mcm_sim_get_file_size_resp_msg_v01, file_info_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_get_file_size_resp_msg_v01, file_info),
  QMI_IDL_TYPE88(0, 4)
};

static const uint8_t mcm_sim_verify_pin_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_verify_pin_req_msg_v01, app_info),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_verify_pin_req_msg_v01, pin_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_verify_pin_req_msg_v01, pin_value),
  MCM_SIM_PIN_MAX_V01,
  QMI_IDL_OFFSET8(mcm_sim_verify_pin_req_msg_v01, pin_value) - QMI_IDL_OFFSET8(mcm_sim_verify_pin_req_msg_v01, pin_value_len)
};

static const uint8_t mcm_sim_verify_pin_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_verify_pin_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sim_verify_pin_resp_msg_v01, retries_left) - QMI_IDL_OFFSET8(mcm_sim_verify_pin_resp_msg_v01, retries_left_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_verify_pin_resp_msg_v01, retries_left)
};

static const uint8_t mcm_sim_change_pin_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_change_pin_req_msg_v01, app_info),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_change_pin_req_msg_v01, pin_id),

  0x03,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_change_pin_req_msg_v01, old_pin_value),
  MCM_SIM_PIN_MAX_V01,
  QMI_IDL_OFFSET8(mcm_sim_change_pin_req_msg_v01, old_pin_value) - QMI_IDL_OFFSET8(mcm_sim_change_pin_req_msg_v01, old_pin_value_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x04,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_change_pin_req_msg_v01, new_pin_value),
  MCM_SIM_PIN_MAX_V01,
  QMI_IDL_OFFSET8(mcm_sim_change_pin_req_msg_v01, new_pin_value) - QMI_IDL_OFFSET8(mcm_sim_change_pin_req_msg_v01, new_pin_value_len)
};

static const uint8_t mcm_sim_change_pin_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_change_pin_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sim_change_pin_resp_msg_v01, retries_left) - QMI_IDL_OFFSET8(mcm_sim_change_pin_resp_msg_v01, retries_left_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_change_pin_resp_msg_v01, retries_left)
};

static const uint8_t mcm_sim_unblock_pin_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_unblock_pin_req_msg_v01, app_info),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_unblock_pin_req_msg_v01, pin_id),

  0x03,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_unblock_pin_req_msg_v01, puk_value),
  MCM_SIM_PIN_MAX_V01,
  QMI_IDL_OFFSET8(mcm_sim_unblock_pin_req_msg_v01, puk_value) - QMI_IDL_OFFSET8(mcm_sim_unblock_pin_req_msg_v01, puk_value_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x04,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_unblock_pin_req_msg_v01, new_pin_value),
  MCM_SIM_PIN_MAX_V01,
  QMI_IDL_OFFSET8(mcm_sim_unblock_pin_req_msg_v01, new_pin_value) - QMI_IDL_OFFSET8(mcm_sim_unblock_pin_req_msg_v01, new_pin_value_len)
};

static const uint8_t mcm_sim_unblock_pin_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_unblock_pin_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sim_unblock_pin_resp_msg_v01, retries_left) - QMI_IDL_OFFSET8(mcm_sim_unblock_pin_resp_msg_v01, retries_left_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_unblock_pin_resp_msg_v01, retries_left)
};

static const uint8_t mcm_sim_enable_pin_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_enable_pin_req_msg_v01, app_info),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_enable_pin_req_msg_v01, pin_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_enable_pin_req_msg_v01, pin_value),
  MCM_SIM_PIN_MAX_V01,
  QMI_IDL_OFFSET8(mcm_sim_enable_pin_req_msg_v01, pin_value) - QMI_IDL_OFFSET8(mcm_sim_enable_pin_req_msg_v01, pin_value_len)
};

static const uint8_t mcm_sim_enable_pin_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_enable_pin_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sim_enable_pin_resp_msg_v01, retries_left) - QMI_IDL_OFFSET8(mcm_sim_enable_pin_resp_msg_v01, retries_left_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_enable_pin_resp_msg_v01, retries_left)
};

static const uint8_t mcm_sim_disable_pin_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_disable_pin_req_msg_v01, app_info),
  QMI_IDL_TYPE88(0, 0),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_disable_pin_req_msg_v01, pin_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_disable_pin_req_msg_v01, pin_value),
  MCM_SIM_PIN_MAX_V01,
  QMI_IDL_OFFSET8(mcm_sim_disable_pin_req_msg_v01, pin_value) - QMI_IDL_OFFSET8(mcm_sim_disable_pin_req_msg_v01, pin_value_len)
};

static const uint8_t mcm_sim_disable_pin_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_disable_pin_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sim_disable_pin_resp_msg_v01, retries_left) - QMI_IDL_OFFSET8(mcm_sim_disable_pin_resp_msg_v01, retries_left_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_disable_pin_resp_msg_v01, retries_left)
};

static const uint8_t mcm_sim_depersonalization_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_depersonalization_req_msg_v01, depersonalization),
  QMI_IDL_TYPE88(0, 5)
};

static const uint8_t mcm_sim_depersonalization_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_depersonalization_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sim_depersonalization_resp_msg_v01, retries_left) - QMI_IDL_OFFSET8(mcm_sim_depersonalization_resp_msg_v01, retries_left_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_depersonalization_resp_msg_v01, retries_left),
  QMI_IDL_TYPE88(0, 12)
};

static const uint8_t mcm_sim_personalization_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_personalization_req_msg_v01, ck_value),
  MCM_SIM_CK_MAX_V01,
  QMI_IDL_OFFSET8(mcm_sim_personalization_req_msg_v01, ck_value) - QMI_IDL_OFFSET8(mcm_sim_personalization_req_msg_v01, ck_value_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sim_personalization_req_msg_v01, feature_gw_network_perso) - QMI_IDL_OFFSET8(mcm_sim_personalization_req_msg_v01, feature_gw_network_perso_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_personalization_req_msg_v01, feature_gw_network_perso),
  MCM_SIM_PERSO_NUM_NW_MAX_V01,
  QMI_IDL_OFFSET8(mcm_sim_personalization_req_msg_v01, feature_gw_network_perso) - QMI_IDL_OFFSET8(mcm_sim_personalization_req_msg_v01, feature_gw_network_perso_len),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_gw_network_subset_perso) - QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_gw_network_subset_perso_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(mcm_sim_personalization_req_msg_v01, feature_gw_network_subset_perso),
  MCM_SIM_PERSO_NUM_NS_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_gw_network_subset_perso) - QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_gw_network_subset_perso_len),
  QMI_IDL_TYPE88(0, 7),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_gw_sp_perso) - QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_gw_sp_perso_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(mcm_sim_personalization_req_msg_v01, feature_gw_sp_perso),
  MCM_SIM_PERSO_NUM_GW_SP_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_gw_sp_perso) - QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_gw_sp_perso_len),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_gw_corporate_perso) - QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_gw_corporate_perso_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(mcm_sim_personalization_req_msg_v01, feature_gw_corporate_perso),
  MCM_SIM_PERSO_NUM_GW_CP_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_gw_corporate_perso) - QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_gw_corporate_perso_len),
  QMI_IDL_TYPE88(0, 9),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_gw_sim_perso) - QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_gw_sim_perso_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(mcm_sim_personalization_req_msg_v01, feature_gw_sim_perso),
  MCM_SIM_PERSO_NUM_SIM_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_gw_sim_perso) - QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_gw_sim_perso_len),
  QMI_IDL_TYPE88(0, 10),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_1x_network1_perso) - QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_1x_network1_perso_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(mcm_sim_personalization_req_msg_v01, feature_1x_network1_perso),
  MCM_SIM_PERSO_NUM_NW_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_1x_network1_perso) - QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_1x_network1_perso_len),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_1x_network2_perso) - QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_1x_network2_perso_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(mcm_sim_personalization_req_msg_v01, feature_1x_network2_perso),
  MCM_SIM_PERSO_NUM_1X_NW2_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_1x_network2_perso) - QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_1x_network2_perso_len),
  QMI_IDL_TYPE88(0, 11),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_1x_ruim_perso) - QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_1x_ruim_perso_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(mcm_sim_personalization_req_msg_v01, feature_1x_ruim_perso),
  MCM_SIM_PERSO_NUM_SIM_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_1x_ruim_perso) - QMI_IDL_OFFSET16RELATIVE(mcm_sim_personalization_req_msg_v01, feature_1x_ruim_perso_len),
  QMI_IDL_TYPE88(0, 10)
};

static const uint8_t mcm_sim_personalization_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_personalization_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sim_personalization_resp_msg_v01, retries_left) - QMI_IDL_OFFSET8(mcm_sim_personalization_resp_msg_v01, retries_left_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_personalization_resp_msg_v01, retries_left),
  QMI_IDL_TYPE88(0, 12)
};

static const uint8_t mcm_sim_get_card_status_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_get_card_status_req_msg_v01, slot_id)
};

static const uint8_t mcm_sim_get_card_status_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_get_card_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sim_get_card_status_resp_msg_v01, card_info) - QMI_IDL_OFFSET8(mcm_sim_get_card_status_resp_msg_v01, card_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_get_card_status_resp_msg_v01, card_info),
  QMI_IDL_TYPE88(0, 15)
};

static const uint8_t mcm_sim_event_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sim_event_register_req_msg_v01, register_card_status_event) - QMI_IDL_OFFSET8(mcm_sim_event_register_req_msg_v01, register_card_status_event_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_event_register_req_msg_v01, register_card_status_event),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sim_event_register_req_msg_v01, register_refresh_event) - QMI_IDL_OFFSET8(mcm_sim_event_register_req_msg_v01, register_refresh_event_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_event_register_req_msg_v01, register_refresh_event)
};

static const uint8_t mcm_sim_event_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_event_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_sim_card_status_event_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sim_card_status_event_ind_msg_v01, slot_id) - QMI_IDL_OFFSET8(mcm_sim_card_status_event_ind_msg_v01, slot_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_sim_card_status_event_ind_msg_v01, slot_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sim_card_status_event_ind_msg_v01, card_info) - QMI_IDL_OFFSET8(mcm_sim_card_status_event_ind_msg_v01, card_info_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_card_status_event_ind_msg_v01, card_info),
  QMI_IDL_TYPE88(0, 15)
};

static const uint8_t mcm_sim_refresh_event_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_sim_refresh_event_ind_msg_v01, refresh_event) - QMI_IDL_OFFSET8(mcm_sim_refresh_event_ind_msg_v01, refresh_event_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_sim_refresh_event_ind_msg_v01, refresh_event),
  QMI_IDL_TYPE88(0, 17)
};

/* Type Table */
static const qmi_idl_type_table_entry  mcm_sim_type_table_v01[] = {
  {sizeof(mcm_sim_application_identification_info_t_v01), mcm_sim_application_identification_info_t_data_v01},
  {sizeof(mcm_sim_plmn_t_v01), mcm_sim_plmn_t_data_v01},
  {sizeof(mcm_sim_file_access_t_v01), mcm_sim_file_access_t_data_v01},
  {sizeof(mcm_sim_card_result_t_v01), mcm_sim_card_result_t_data_v01},
  {sizeof(mcm_sim_file_info_t_v01), mcm_sim_file_info_t_data_v01},
  {sizeof(mcm_sim_depersonalization_t_v01), mcm_sim_depersonalization_t_data_v01},
  {sizeof(mcm_sim_network_perso_t_v01), mcm_sim_network_perso_t_data_v01},
  {sizeof(mcm_sim_gw_network_subset_perso_t_v01), mcm_sim_gw_network_subset_perso_t_data_v01},
  {sizeof(mcm_sim_gw_sp_perso_t_v01), mcm_sim_gw_sp_perso_t_data_v01},
  {sizeof(mcm_sim_gw_corporate_perso_t_v01), mcm_sim_gw_corporate_perso_t_data_v01},
  {sizeof(mcm_sim_sim_perso_t_v01), mcm_sim_sim_perso_t_data_v01},
  {sizeof(mcm_sim_1x_network_type2_perso_t_v01), mcm_sim_1x_network_type2_perso_t_data_v01},
  {sizeof(mcm_sim_perso_retries_left_t_v01), mcm_sim_perso_retries_left_t_data_v01},
  {sizeof(mcm_sim_app_info_t_v01), mcm_sim_app_info_t_data_v01},
  {sizeof(mcm_sim_card_app_info_t_v01), mcm_sim_card_app_info_t_data_v01},
  {sizeof(mcm_sim_card_info_t_v01), mcm_sim_card_info_t_data_v01},
  {sizeof(mcm_sim_refresh_file_list_t_v01), mcm_sim_refresh_file_list_t_data_v01},
  {sizeof(mcm_sim_refresh_event_t_v01), mcm_sim_refresh_event_t_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry mcm_sim_message_table_v01[] = {
  {sizeof(mcm_sim_get_subscriber_id_req_msg_v01), mcm_sim_get_subscriber_id_req_msg_data_v01},
  {sizeof(mcm_sim_get_subscriber_id_resp_msg_v01), mcm_sim_get_subscriber_id_resp_msg_data_v01},
  {sizeof(mcm_sim_get_card_id_req_msg_v01), mcm_sim_get_card_id_req_msg_data_v01},
  {sizeof(mcm_sim_get_card_id_resp_msg_v01), mcm_sim_get_card_id_resp_msg_data_v01},
  {sizeof(mcm_sim_get_device_phone_number_req_msg_v01), mcm_sim_get_device_phone_number_req_msg_data_v01},
  {sizeof(mcm_sim_get_device_phone_number_resp_msg_v01), mcm_sim_get_device_phone_number_resp_msg_data_v01},
  {sizeof(mcm_sim_get_preferred_operator_list_req_msg_v01), mcm_sim_get_preferred_operator_list_req_msg_data_v01},
  {sizeof(mcm_sim_get_preferred_operator_list_resp_msg_v01), mcm_sim_get_preferred_operator_list_resp_msg_data_v01},
  {sizeof(mcm_sim_read_file_req_msg_v01), mcm_sim_read_file_req_msg_data_v01},
  {sizeof(mcm_sim_read_file_resp_msg_v01), mcm_sim_read_file_resp_msg_data_v01},
  {sizeof(mcm_sim_write_file_req_msg_v01), mcm_sim_write_file_req_msg_data_v01},
  {sizeof(mcm_sim_write_file_resp_msg_v01), mcm_sim_write_file_resp_msg_data_v01},
  {sizeof(mcm_sim_get_file_size_req_msg_v01), mcm_sim_get_file_size_req_msg_data_v01},
  {sizeof(mcm_sim_get_file_size_resp_msg_v01), mcm_sim_get_file_size_resp_msg_data_v01},
  {sizeof(mcm_sim_verify_pin_req_msg_v01), mcm_sim_verify_pin_req_msg_data_v01},
  {sizeof(mcm_sim_verify_pin_resp_msg_v01), mcm_sim_verify_pin_resp_msg_data_v01},
  {sizeof(mcm_sim_change_pin_req_msg_v01), mcm_sim_change_pin_req_msg_data_v01},
  {sizeof(mcm_sim_change_pin_resp_msg_v01), mcm_sim_change_pin_resp_msg_data_v01},
  {sizeof(mcm_sim_unblock_pin_req_msg_v01), mcm_sim_unblock_pin_req_msg_data_v01},
  {sizeof(mcm_sim_unblock_pin_resp_msg_v01), mcm_sim_unblock_pin_resp_msg_data_v01},
  {sizeof(mcm_sim_enable_pin_req_msg_v01), mcm_sim_enable_pin_req_msg_data_v01},
  {sizeof(mcm_sim_enable_pin_resp_msg_v01), mcm_sim_enable_pin_resp_msg_data_v01},
  {sizeof(mcm_sim_disable_pin_req_msg_v01), mcm_sim_disable_pin_req_msg_data_v01},
  {sizeof(mcm_sim_disable_pin_resp_msg_v01), mcm_sim_disable_pin_resp_msg_data_v01},
  {sizeof(mcm_sim_depersonalization_req_msg_v01), mcm_sim_depersonalization_req_msg_data_v01},
  {sizeof(mcm_sim_depersonalization_resp_msg_v01), mcm_sim_depersonalization_resp_msg_data_v01},
  {sizeof(mcm_sim_personalization_req_msg_v01), mcm_sim_personalization_req_msg_data_v01},
  {sizeof(mcm_sim_personalization_resp_msg_v01), mcm_sim_personalization_resp_msg_data_v01},
  {sizeof(mcm_sim_get_card_status_req_msg_v01), mcm_sim_get_card_status_req_msg_data_v01},
  {sizeof(mcm_sim_get_card_status_resp_msg_v01), mcm_sim_get_card_status_resp_msg_data_v01},
  {sizeof(mcm_sim_event_register_req_msg_v01), mcm_sim_event_register_req_msg_data_v01},
  {sizeof(mcm_sim_event_register_resp_msg_v01), mcm_sim_event_register_resp_msg_data_v01},
  {sizeof(mcm_sim_card_status_event_ind_msg_v01), mcm_sim_card_status_event_ind_msg_data_v01},
  {sizeof(mcm_sim_refresh_event_ind_msg_v01), mcm_sim_refresh_event_ind_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object mcm_sim_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *mcm_sim_qmi_idl_type_table_object_referenced_tables_v01[] =
{&mcm_sim_qmi_idl_type_table_object_v01, &mcm_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object mcm_sim_qmi_idl_type_table_object_v01 = {
  sizeof(mcm_sim_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(mcm_sim_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  mcm_sim_type_table_v01,
  mcm_sim_message_table_v01,
  mcm_sim_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry mcm_sim_service_command_messages_v01[] = {
  {MCM_SIM_GET_SUBSCRIBER_ID_REQ_V01, QMI_IDL_TYPE16(0, 0), 11},
  {MCM_SIM_GET_CARD_ID_REQ_V01, QMI_IDL_TYPE16(0, 2), 7},
  {MCM_SIM_GET_DEVICE_PHONE_NUMBER_REQ_V01, QMI_IDL_TYPE16(0, 4), 11},
  {MCM_SIM_GET_PREFERRED_OPERATOR_LIST_REQ_V01, QMI_IDL_TYPE16(0, 6), 7},
  {MCM_SIM_READ_FILE_REQ_V01, QMI_IDL_TYPE16(0, 8), 38},
  {MCM_SIM_WRITE_FILE_REQ_V01, QMI_IDL_TYPE16(0, 10), 4139},
  {MCM_SIM_GET_FILE_SIZE_REQ_V01, QMI_IDL_TYPE16(0, 12), 35},
  {MCM_SIM_VERIFY_PIN_REQ_V01, QMI_IDL_TYPE16(0, 14), 30},
  {MCM_SIM_CHANGE_PIN_REQ_V01, QMI_IDL_TYPE16(0, 16), 42},
  {MCM_SIM_UNBLOCK_PIN_REQ_V01, QMI_IDL_TYPE16(0, 18), 42},
  {MCM_SIM_ENABLE_PIN_REQ_V01, QMI_IDL_TYPE16(0, 20), 30},
  {MCM_SIM_DISABLE_PIN_REQ_V01, QMI_IDL_TYPE16(0, 22), 30},
  {MCM_SIM_GET_CARD_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 28), 7},
  {MCM_SIM_DEPERSONALIZATION_REQ_V01, QMI_IDL_TYPE16(0, 24), 28},
  {MCM_SIM_PERSONALIZATION_REQ_V01, QMI_IDL_TYPE16(0, 26), 4453},
  {MCM_SIM_EVENT_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 30), 8}
};

static const qmi_idl_service_message_table_entry mcm_sim_service_response_messages_v01[] = {
  {MCM_SIM_GET_SUBSCRIBER_ID_RESP_V01, QMI_IDL_TYPE16(0, 1), 31},
  {MCM_SIM_GET_CARD_ID_RESP_V01, QMI_IDL_TYPE16(0, 3), 35},
  {MCM_SIM_GET_DEVICE_PHONE_NUMBER_RESP_V01, QMI_IDL_TYPE16(0, 5), 97},
  {MCM_SIM_GET_PREFERRED_OPERATOR_LIST_RESP_V01, QMI_IDL_TYPE16(0, 7), 183},
  {MCM_SIM_READ_FILE_RESP_V01, QMI_IDL_TYPE16(0, 9), 4117},
  {MCM_SIM_WRITE_FILE_RESP_V01, QMI_IDL_TYPE16(0, 11), 16},
  {MCM_SIM_GET_FILE_SIZE_RESP_V01, QMI_IDL_TYPE16(0, 13), 29},
  {MCM_SIM_VERIFY_PIN_RESP_V01, QMI_IDL_TYPE16(0, 15), 15},
  {MCM_SIM_CHANGE_PIN_RESP_V01, QMI_IDL_TYPE16(0, 17), 15},
  {MCM_SIM_UNBLOCK_PIN_RESP_V01, QMI_IDL_TYPE16(0, 19), 15},
  {MCM_SIM_ENABLE_PIN_RESP_V01, QMI_IDL_TYPE16(0, 21), 15},
  {MCM_SIM_DISABLE_PIN_RESP_V01, QMI_IDL_TYPE16(0, 23), 15},
  {MCM_SIM_GET_CARD_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 29), 100},
  {MCM_SIM_DEPERSONALIZATION_RESP_V01, QMI_IDL_TYPE16(0, 25), 16},
  {MCM_SIM_PERSONALIZATION_RESP_V01, QMI_IDL_TYPE16(0, 27), 16},
  {MCM_SIM_EVENT_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 31), 11}
};

static const qmi_idl_service_message_table_entry mcm_sim_service_indication_messages_v01[] = {
  {MCM_SIM_CARD_STATUS_EVENT_IND_V01, QMI_IDL_TYPE16(0, 32), 96},
  {MCM_SIM_REFRESH_EVENT_IND_V01, QMI_IDL_TYPE16(0, 33), 751}
};

/*Service Object*/
struct qmi_idl_service_object mcm_sim_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x402,
  4453,
  { sizeof(mcm_sim_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(mcm_sim_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(mcm_sim_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { mcm_sim_service_command_messages_v01, mcm_sim_service_response_messages_v01, mcm_sim_service_indication_messages_v01},
  &mcm_sim_qmi_idl_type_table_object_v01,
  0x01,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type mcm_sim_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( MCM_SIM_V01_IDL_MAJOR_VERS != idl_maj_version || MCM_SIM_V01_IDL_MINOR_VERS != idl_min_version
       || MCM_SIM_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&mcm_sim_qmi_idl_service_object_v01;
}

