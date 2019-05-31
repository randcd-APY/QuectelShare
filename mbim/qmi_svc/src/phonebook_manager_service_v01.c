/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P H O N E B O O K _ M A N A G E R _ S E R V I C E _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the pbm service Data structures.

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.

  $Header: //source/qcom/qct/interfaces/qmi/pbm/main/latest/src/phonebook_manager_service_v01.c#5 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 4.3
   It was generated on: Tue Feb 28 2012
   From IDL File: phonebook_manager_service_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "phonebook_manager_service_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t pbm_phonebook_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_phonebook_info_type_v01, session_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_phonebook_info_type_v01, pb_type),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_capability_basic_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_type_v01, session_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_type_v01, pb_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_type_v01, used_records),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_type_v01, max_records),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_type_v01, max_num_len),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_type_v01, max_name_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_group_capability_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_group_capability_type_v01, max_grp),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_group_capability_type_v01, max_grp_tag_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_ad_num_capability_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_ad_num_capability_type_v01, max_ad_num),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_ad_num_capability_type_v01, max_ad_num_len),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_ad_num_capability_type_v01, max_ad_num_tag_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_email_capability_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_email_capability_type_v01, max_email),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_email_capability_type_v01, max_email_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_gas_capability_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_gas_capability_type_v01, max_records),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_gas_capability_type_v01, used_records),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_gas_capability_type_v01, max_gas_string_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_aas_capability_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_capability_type_v01, max_records),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_capability_type_v01, used_records),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_capability_type_v01, max_aas_string_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_capability_basic_info_array_elem_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_array_elem_type_v01, pb_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_array_elem_type_v01, used_records),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_array_elem_type_v01, max_records),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_array_elem_type_v01, max_num_len),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_array_elem_type_v01, max_name_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_capability_basic_info_array_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_array_type_v01, session_type),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_array_type_v01, capability_basic_info),
  QMI_PBM_MAX_NUM_PBS_V01,
  QMI_IDL_OFFSET8(pbm_capability_basic_info_array_type_v01, capability_basic_info) - QMI_IDL_OFFSET8(pbm_capability_basic_info_array_type_v01, capability_basic_info_len),
 7, 0,
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_group_capability_with_session_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_group_capability_with_session_type_v01, session_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_group_capability_with_session_type_v01, max_grp),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_group_capability_with_session_type_v01, max_grp_tag_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_ad_num_capability_with_session_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_ad_num_capability_with_session_type_v01, session_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_ad_num_capability_with_session_type_v01, max_ad_num),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_ad_num_capability_with_session_type_v01, max_ad_num_len),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_ad_num_capability_with_session_type_v01, max_ad_num_tag_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_email_capability_with_session_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_email_capability_with_session_type_v01, session_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_email_capability_with_session_type_v01, max_email),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_email_capability_with_session_type_v01, max_email_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_second_name_capability_with_session_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_second_name_capability_with_session_type_v01, session_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_second_name_capability_with_session_type_v01, max_second_name_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_hidden_records_capability_with_session_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_hidden_records_capability_with_session_type_v01, session_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_hidden_records_capability_with_session_type_v01, is_hidden_entry_supported),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_gas_capability_with_session_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_gas_capability_with_session_type_v01, session_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_gas_capability_with_session_type_v01, max_records),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_gas_capability_with_session_type_v01, used_records),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_gas_capability_with_session_type_v01, max_gas_string_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_aas_capability_with_session_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_aas_capability_with_session_type_v01, session_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_capability_with_session_type_v01, max_records),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_capability_with_session_type_v01, used_records),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_capability_with_session_type_v01, max_aas_string_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_record_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_record_info_type_v01, session_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_record_info_type_v01, pb_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_record_info_type_v01, record_start_id),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_record_info_type_v01, record_end_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_record_information_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_record_information_type_v01, session_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_record_information_type_v01, phonebook_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_record_information_type_v01, record_id),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_record_information_type_v01, num_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_record_information_type_v01, num_plan),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_record_information_type_v01, number),
  QMI_PBM_NUMBER_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_record_information_type_v01, number) - QMI_IDL_OFFSET8(pbm_record_information_type_v01, number_len),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_record_information_type_v01, name),
  QMI_PBM_NAME_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_record_information_type_v01, name) - QMI_IDL_OFFSET8(pbm_record_information_type_v01, name_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_ad_num_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_ad_num_info_type_v01, num_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_ad_num_info_type_v01, num_plan),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_ad_num_info_type_v01, ad_number),
  QMI_PBM_NUMBER_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_ad_num_info_type_v01, ad_number) - QMI_IDL_OFFSET8(pbm_ad_num_info_type_v01, ad_number_len),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_ad_num_info_type_v01, ad_num_tag_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_email_info_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_email_info_type_v01, email_address),
  QMI_PBM_EMAIL_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_email_info_type_v01, email_address) - QMI_IDL_OFFSET8(pbm_email_info_type_v01, email_address_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_record_id_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_record_id_type_v01, session_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_record_id_type_v01, pb_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_record_id_type_v01, record_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_record_update_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_record_update_info_type_v01, session_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_record_update_info_type_v01, pb_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_record_update_info_type_v01, operation),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_record_update_info_type_v01, record_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_refresh_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_refresh_info_type_v01, session_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_refresh_info_type_v01, pb_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_refresh_info_type_v01, status),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_emer_num_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_emer_num_type_v01, emer_num),
  QMI_PBM_EMER_NUM_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_emer_num_type_v01, emer_num) - QMI_IDL_OFFSET8(pbm_emer_num_type_v01, emer_num_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_ecc_emer_num_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_ecc_emer_num_type_v01, cat),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_ecc_emer_num_type_v01, emer_num),
  QMI_PBM_EMER_NUM_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_ecc_emer_num_type_v01, emer_num) - QMI_IDL_OFFSET8(pbm_ecc_emer_num_type_v01, emer_num_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_card_emer_num_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_card_emer_num_type_v01, session_type),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_card_emer_num_type_v01, emer_nums),
  QMI_PBM_MAX_NUM_CARD_ECC_EMER_NUM_V01,
  QMI_IDL_OFFSET8(pbm_card_emer_num_type_v01, emer_nums) - QMI_IDL_OFFSET8(pbm_card_emer_num_type_v01, emer_nums_len),
 24, 0,
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_network_emer_num_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_network_emer_num_type_v01, session_type),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_network_emer_num_type_v01, emer_nums),
  QMI_PBM_MAX_NUM_NETWORK_ECC_EMER_NUM_V01,
  QMI_IDL_OFFSET8(pbm_network_emer_num_type_v01, emer_nums) - QMI_IDL_OFFSET8(pbm_network_emer_num_type_v01, emer_nums_len),
 24, 0,
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_phonebook_ready_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_phonebook_ready_info_type_v01, session_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_phonebook_ready_info_type_v01, pb_bit_mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_record_instance_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_record_instance_type_v01, record_id),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_record_instance_type_v01, num_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_record_instance_type_v01, num_plan),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_record_instance_type_v01, number),
  QMI_PBM_NUMBER_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_record_instance_type_v01, number) - QMI_IDL_OFFSET8(pbm_record_instance_type_v01, number_len),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_record_instance_type_v01, name),
  QMI_PBM_NAME_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_record_instance_type_v01, name) - QMI_IDL_OFFSET8(pbm_record_instance_type_v01, name_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_basic_record_data_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_basic_record_data_type_v01, seq_num),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_basic_record_data_type_v01, session_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_basic_record_data_type_v01, pb_type),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_basic_record_data_type_v01, record_instances),
  QMI_PBM_MAX_NUM_BASIC_RECORD_INSTANCE_V01,
  QMI_IDL_OFFSET8(pbm_basic_record_data_type_v01, record_instances) - QMI_IDL_OFFSET8(pbm_basic_record_data_type_v01, record_instances_len),
 28, 0,
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_sname_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_sname_info_type_v01, record_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_sname_info_type_v01, sname),
  QMI_PBM_NAME_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_sname_info_type_v01, sname) - QMI_IDL_OFFSET8(pbm_sname_info_type_v01, sname_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_ad_num_record_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_ad_num_record_info_type_v01, record_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_ad_num_record_info_type_v01, ad_nums),
  QMI_PBM_MAX_NUM_AD_NUM_COUNT_V01,
  QMI_IDL_OFFSET8(pbm_ad_num_record_info_type_v01, ad_nums) - QMI_IDL_OFFSET8(pbm_ad_num_record_info_type_v01, ad_nums_len),
 18, 0,
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_grp_id_record_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_grp_id_record_info_type_v01, record_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_grp_id_record_info_type_v01, grp_id),
  QMI_PBM_GRP_ID_MAX_V01,
  QMI_IDL_OFFSET8(pbm_grp_id_record_info_type_v01, grp_id) - QMI_IDL_OFFSET8(pbm_grp_id_record_info_type_v01, grp_id_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_email_record_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_email_record_info_type_v01, record_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_email_record_info_type_v01, email),
  QMI_PBM_EMAIL_INFO_MAX_V01,
  QMI_IDL_OFFSET8(pbm_email_record_info_type_v01, email) - QMI_IDL_OFFSET8(pbm_email_record_info_type_v01, email_len),
 19, 0,
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_hidden_record_info_type_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_hidden_record_info_type_v01, record_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_hidden_record_info_type_v01, is_hidden),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_groups_instance_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_groups_instance_type_v01, grp_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_groups_instance_type_v01, grp_name),
  QMI_PBM_GROUP_NAME_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_groups_instance_type_v01, grp_name) - QMI_IDL_OFFSET8(pbm_groups_instance_type_v01, grp_name_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_groups_data_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_groups_data_type_v01, session_type),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_groups_data_type_v01, grp_array),
  QMI_PBM_MAX_NUM_GROUP_COUNT_V01,
  QMI_IDL_OFFSET8(pbm_groups_data_type_v01, grp_array) - QMI_IDL_OFFSET8(pbm_groups_data_type_v01, grp_array_len),
 35, 0,
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_grp_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_grp_info_type_v01, session_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_grp_info_type_v01, operation),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_grp_info_type_v01, group_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_grp_info_type_v01, grp_name),
  QMI_PBM_GROUP_NAME_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_grp_info_type_v01, grp_name) - QMI_IDL_OFFSET8(pbm_grp_info_type_v01, grp_name_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_grp_id_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_grp_id_type_v01, session_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_grp_id_type_v01, group_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_phonebook_state_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_phonebook_state_type_v01, session_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_phonebook_state_type_v01, pb_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_phonebook_state_type_v01, state),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_hidden_status_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_hidden_status_type_v01, session_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_hidden_status_type_v01, status),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_base_record_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_base_record_info_type_v01, session_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_base_record_info_type_v01, pb_type),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_base_record_info_type_v01, record_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_aas_instance_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_instance_type_v01, aas_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_instance_type_v01, alpha),
  QMI_PBM_AAS_NAME_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_aas_instance_type_v01, alpha) - QMI_IDL_OFFSET8(pbm_aas_instance_type_v01, alpha_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_aas_data_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_aas_data_type_v01, session_type),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_aas_data_type_v01, aas_array),
  QMI_PBM_MAX_NUM_AAS_COUNT_V01,
  QMI_IDL_OFFSET8(pbm_aas_data_type_v01, aas_array) - QMI_IDL_OFFSET8(pbm_aas_data_type_v01, aas_array_len),
 42, 0,
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_aas_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_aas_info_type_v01, session_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_aas_info_type_v01, operation),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_info_type_v01, aas_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_info_type_v01, alpha),
  QMI_PBM_AAS_NAME_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_aas_info_type_v01, alpha) - QMI_IDL_OFFSET8(pbm_aas_info_type_v01, alpha_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_aas_id_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_aas_id_type_v01, session_type),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_id_type_v01, aas_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_aas_update_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_aas_update_info_type_v01, session_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_aas_update_info_type_v01, operation),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_update_info_type_v01, aas_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_aas_update_info_type_v01, alpha),
  QMI_PBM_AAS_NAME_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_aas_update_info_type_v01, alpha) - QMI_IDL_OFFSET8(pbm_aas_update_info_type_v01, alpha_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_gas_update_info_type_data_v01[] = {
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_gas_update_info_type_v01, session_type),

  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_gas_update_info_type_v01, operation),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_gas_update_info_type_v01, gas_id),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_gas_update_info_type_v01, grp_name),
  QMI_PBM_GROUP_NAME_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_gas_update_info_type_v01, grp_name) - QMI_IDL_OFFSET8(pbm_gas_update_info_type_v01, grp_name_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t pbm_access_info_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(pbm_access_info_type_v01, protection_method),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(pbm_access_info_type_v01, pin_mask),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t pbm_indication_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(pbm_indication_register_req_msg_v01, reg_mask)
};

static const uint8_t pbm_indication_register_resp_msg_data_v01[] = {
  0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_indication_register_resp_msg_v01, resp),
  0, 1,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_indication_register_resp_msg_v01, reg_mask) - QMI_IDL_OFFSET8(pbm_indication_register_resp_msg_v01, reg_mask_valid)),
  0x10,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(pbm_indication_register_resp_msg_v01, reg_mask)
};

static const uint8_t pbm_get_pb_capabilities_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_pb_capabilities_req_msg_v01, phonebook_info),
  0, 0
};

static const uint8_t pbm_get_pb_capabilities_resp_msg_data_v01[] = {
  0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, resp),
  0, 1,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, capability_basic_info) - QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, capability_basic_info_valid)),
  0x10,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, capability_basic_info),
  1, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, group_capability) - QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, group_capability_valid)),
  0x11,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, group_capability),
  2, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, ad_num_capability) - QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, ad_num_capability_valid)),
  0x12,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, ad_num_capability),
  3, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, email_capability) - QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, email_capability_valid)),
  0x13,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, email_capability),
  4, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, max_second_name_len) - QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, max_second_name_len_valid)),
  0x14,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, max_second_name_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, is_hidden_entry_supported) - QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, is_hidden_entry_supported_valid)),
  0x15,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, is_hidden_entry_supported),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, gas_capability) - QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, gas_capability_valid)),
  0x16,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, gas_capability),
  5, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, aas_capability) - QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, aas_capability_valid)),
  0x17,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, aas_capability),
  6, 0,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, write_access_info) - QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, write_access_info_valid)),
  0x18,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_pb_capabilities_resp_msg_v01, write_access_info),
  48, 0
};

/*
 * pbm_get_all_pb_capabilities_req_msg is empty
 * static const uint8_t pbm_get_all_pb_capabilities_req_msg_data_v01[] = {
 * };
 */

static const uint8_t pbm_get_all_pb_capabilities_resp_msg_data_v01[] = {
  0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_all_pb_capabilities_resp_msg_v01, resp),
  0, 1,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_all_pb_capabilities_resp_msg_v01, capability_basic_info_array) - QMI_IDL_OFFSET8(pbm_get_all_pb_capabilities_resp_msg_v01, capability_basic_info_array_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_all_pb_capabilities_resp_msg_v01, capability_basic_info_array),
  QMI_PBM_MAX_NUM_SESSIONS_V01,
  QMI_IDL_OFFSET8(pbm_get_all_pb_capabilities_resp_msg_v01, capability_basic_info_array) - QMI_IDL_OFFSET8(pbm_get_all_pb_capabilities_resp_msg_v01, capability_basic_info_array_len),
  8, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, group_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, group_capability_array_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_get_all_pb_capabilities_resp_msg_v01, group_capability_array),
  QMI_PBM_MAX_NUM_SESSIONS_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, group_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, group_capability_array_len),
  9, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, ad_num_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, ad_num_capability_array_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_get_all_pb_capabilities_resp_msg_v01, ad_num_capability_array),
  QMI_PBM_MAX_NUM_SESSIONS_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, ad_num_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, ad_num_capability_array_len),
  10, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, email_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, email_capability_array_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_get_all_pb_capabilities_resp_msg_v01, email_capability_array),
  QMI_PBM_MAX_NUM_SESSIONS_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, email_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, email_capability_array_len),
  11, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, second_name_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, second_name_capability_array_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_get_all_pb_capabilities_resp_msg_v01, second_name_capability_array),
  QMI_PBM_MAX_NUM_SESSIONS_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, second_name_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, second_name_capability_array_len),
  12, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, hidden_records_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, hidden_records_capability_array_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_get_all_pb_capabilities_resp_msg_v01, hidden_records_capability_array),
  QMI_PBM_MAX_NUM_SESSIONS_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, hidden_records_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, hidden_records_capability_array_len),
  13, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, gas_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, gas_capability_array_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_get_all_pb_capabilities_resp_msg_v01, gas_capability_array),
  QMI_PBM_MAX_NUM_SESSIONS_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, gas_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, gas_capability_array_len),
  14, 0,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, aas_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, aas_capability_array_valid)),
  0x17,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_get_all_pb_capabilities_resp_msg_v01, aas_capability_array),
  QMI_PBM_MAX_NUM_SESSIONS_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, aas_capability_array) - QMI_IDL_OFFSET16RELATIVE(pbm_get_all_pb_capabilities_resp_msg_v01, aas_capability_array_len),
  15, 0
};

static const uint8_t pbm_read_records_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_read_records_req_msg_v01, record_info),
  16, 0
};

static const uint8_t pbm_read_records_resp_msg_data_v01[] = {
  0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_read_records_resp_msg_v01, resp),
  0, 1,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_read_records_resp_msg_v01, num_of_recs) - QMI_IDL_OFFSET8(pbm_read_records_resp_msg_v01, num_of_recs_valid)),
  0x10,
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_read_records_resp_msg_v01, num_of_recs)
};

static const uint8_t pbm_write_record_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_write_record_req_msg_v01, record_information),
  17, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, sname) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, sname_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(pbm_write_record_req_msg_v01, sname),
  QMI_PBM_NAME_MAX_LENGTH_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, sname) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, sname_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, ad_num_info) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, ad_num_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_write_record_req_msg_v01, ad_num_info),
  QMI_PBM_AD_NUM_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, ad_num_info) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, ad_num_info_len),
  18, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, grp_id) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, grp_id_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(pbm_write_record_req_msg_v01, grp_id),
  QMI_PBM_GRP_ID_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, grp_id) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, grp_id_len),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, email_info) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, email_info_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_write_record_req_msg_v01, email_info),
  QMI_PBM_EMAIL_INFO_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, email_info) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, email_info_len),
  19, 0,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, is_hidden) - QMI_IDL_OFFSET16RELATIVE(pbm_write_record_req_msg_v01, is_hidden_valid)),
  0x14,
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(pbm_write_record_req_msg_v01, is_hidden)
};

static const uint8_t pbm_write_record_resp_msg_data_v01[] = {
  0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_write_record_resp_msg_v01, resp),
  0, 1,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_write_record_resp_msg_v01, record_id) - QMI_IDL_OFFSET8(pbm_write_record_resp_msg_v01, record_id_valid)),
  0x10,
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_write_record_resp_msg_v01, record_id)
};

static const uint8_t pbm_delete_record_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_delete_record_req_msg_v01, record_info),
  20, 0
};

static const uint8_t pbm_delete_record_resp_msg_data_v01[] = {
  0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_delete_record_resp_msg_v01, resp),
  0, 1,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_delete_record_resp_msg_v01, record_id) - QMI_IDL_OFFSET8(pbm_delete_record_resp_msg_v01, record_id_valid)),
  0x10,
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_delete_record_resp_msg_v01, record_id)
};

static const uint8_t pbm_delete_all_pb_records_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_delete_all_pb_records_req_msg_v01, phonebook_info),
  0, 0
};

static const uint8_t pbm_delete_all_pb_records_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_delete_all_pb_records_resp_msg_v01, resp),
  0, 1
};

static const uint8_t pbm_search_records_req_msg_data_v01[] = {
  0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_search_records_req_msg_v01, search_info),
  0, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_search_records_req_msg_v01, number) - QMI_IDL_OFFSET8(pbm_search_records_req_msg_v01, number_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_search_records_req_msg_v01, number),
  QMI_PBM_NUMBER_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_search_records_req_msg_v01, number) - QMI_IDL_OFFSET8(pbm_search_records_req_msg_v01, number_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_search_records_req_msg_v01, name) - QMI_IDL_OFFSET8(pbm_search_records_req_msg_v01, name_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(pbm_search_records_req_msg_v01, name),
  QMI_PBM_NAME_MAX_LENGTH_V01,
  QMI_IDL_OFFSET8(pbm_search_records_req_msg_v01, name) - QMI_IDL_OFFSET8(pbm_search_records_req_msg_v01, name_len)
};

static const uint8_t pbm_search_records_resp_msg_data_v01[] = {
  0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_search_records_resp_msg_v01, resp),
  0, 1,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_search_records_resp_msg_v01, record_id) - QMI_IDL_OFFSET8(pbm_search_records_resp_msg_v01, record_id_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_search_records_resp_msg_v01, record_id),
  ((QMI_PBM_REC_IDS_MAX_V01) & 0xFF), ((QMI_PBM_REC_IDS_MAX_V01) >> 8),
  QMI_IDL_OFFSET8(pbm_search_records_resp_msg_v01, record_id) - QMI_IDL_OFFSET8(pbm_search_records_resp_msg_v01, record_id_len)
};

static const uint8_t pbm_record_update_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_record_update_ind_msg_v01, record_update_info),
  21, 0
};

static const uint8_t pbm_refresh_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_refresh_ind_msg_v01, refresh_info),
  22, 0
};

static const uint8_t pbm_pb_ready_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_pb_ready_ind_msg_v01, phonebook_ready_info),
  0, 0
};

static const uint8_t pbm_emergency_list_ind_msg_data_v01[] = {
  0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_emergency_list_ind_msg_v01, emer_nums),
  QMI_PBM_MAX_NUM_EMER_NUM_V01,
  QMI_IDL_OFFSET8(pbm_emergency_list_ind_msg_v01, emer_nums) - QMI_IDL_OFFSET8(pbm_emergency_list_ind_msg_v01, emer_nums_len),
  23, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_emergency_list_ind_msg_v01, nv_emer_nums) - QMI_IDL_OFFSET8(pbm_emergency_list_ind_msg_v01, nv_emer_nums_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_emergency_list_ind_msg_v01, nv_emer_nums),
  QMI_PBM_MAX_NUM_NV_EMER_NUM_V01,
  QMI_IDL_OFFSET8(pbm_emergency_list_ind_msg_v01, nv_emer_nums) - QMI_IDL_OFFSET8(pbm_emergency_list_ind_msg_v01, nv_emer_nums_len),
  23, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_emergency_list_ind_msg_v01, card_emer_nums) - QMI_IDL_OFFSET8(pbm_emergency_list_ind_msg_v01, card_emer_nums_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_emergency_list_ind_msg_v01, card_emer_nums),
  QMI_PBM_MAX_NUM_CARD_EMER_NUM_V01,
  QMI_IDL_OFFSET8(pbm_emergency_list_ind_msg_v01, card_emer_nums) - QMI_IDL_OFFSET8(pbm_emergency_list_ind_msg_v01, card_emer_nums_len),
  25, 0,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, network_emer_nums) - QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, network_emer_nums_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_emergency_list_ind_msg_v01, network_emer_nums),
  QMI_PBM_MAX_NUM_NETWORK_EMER_NUM_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, network_emer_nums) - QMI_IDL_OFFSET16RELATIVE(pbm_emergency_list_ind_msg_v01, network_emer_nums_len),
  26, 0
};

static const uint8_t pbm_all_pb_init_done_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_all_pb_init_done_ind_msg_v01, phonebook_ready_info),
  QMI_PBM_MAX_NUM_PHONEBOOK_READY_INFO_V01,
  QMI_IDL_OFFSET8(pbm_all_pb_init_done_ind_msg_v01, phonebook_ready_info) - QMI_IDL_OFFSET8(pbm_all_pb_init_done_ind_msg_v01, phonebook_ready_info_len),
  27, 0
};

static const uint8_t pbm_record_read_ind_msg_data_v01[] = {
  0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_record_read_ind_msg_v01, basic_record_data),
  29, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, sname_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, sname_info_array_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_record_read_ind_msg_v01, sname_info_array),
  QMI_PBM_MAX_NUM_SNAME_INFO_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, sname_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, sname_info_array_len),
  30, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, ad_num_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, ad_num_info_array_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_record_read_ind_msg_v01, ad_num_info_array),
  QMI_PBM_MAX_NUM_AD_NUM_INSTANCE_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, ad_num_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, ad_num_info_array_len),
  31, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, grp_id_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, grp_id_info_array_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_record_read_ind_msg_v01, grp_id_info_array),
  QMI_PBM_MAX_NUM_GRP_ID_INSTANCE_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, grp_id_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, grp_id_info_array_len),
  32, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, email_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, email_info_array_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_record_read_ind_msg_v01, email_info_array),
  QMI_PBM_MAX_NUM_EMAIL_INSTANCE_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, email_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, email_info_array_len),
  33, 0,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, hidden_record_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, hidden_record_info_array_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_record_read_ind_msg_v01, hidden_record_info_array),
  QMI_PBM_MAX_NUM_HIDDEN_INFO_INSTANCE_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, hidden_record_info_array) - QMI_IDL_OFFSET16RELATIVE(pbm_record_read_ind_msg_v01, hidden_record_info_array_len),
  34, 0
};

/*
 * pbm_get_emergency_list_req_msg is empty
 * static const uint8_t pbm_get_emergency_list_req_msg_data_v01[] = {
 * };
 */

static const uint8_t pbm_get_emergency_list_resp_msg_data_v01[] = {
  0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, resp),
  0, 1,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, pbm_emer_nums) - QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, pbm_emer_nums_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, pbm_emer_nums),
  QMI_PBM_MAX_NUM_EMER_NUM_V01,
  QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, pbm_emer_nums) - QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, pbm_emer_nums_len),
  23, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, pbm_nv_emer_nums) - QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, pbm_nv_emer_nums_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, pbm_nv_emer_nums),
  QMI_PBM_MAX_NUM_NV_EMER_NUM_V01,
  QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, pbm_nv_emer_nums) - QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, pbm_nv_emer_nums_len),
  23, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, card_emer_nums) - QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, card_emer_nums_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, card_emer_nums),
  QMI_PBM_MAX_NUM_CARD_EMER_NUM_V01,
  QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, card_emer_nums) - QMI_IDL_OFFSET8(pbm_get_emergency_list_resp_msg_v01, card_emer_nums_len),
  25, 0,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, network_emer_nums) - QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, network_emer_nums_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(pbm_get_emergency_list_resp_msg_v01, network_emer_nums),
  QMI_PBM_MAX_NUM_NETWORK_EMER_NUM_V01,
  QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, network_emer_nums) - QMI_IDL_OFFSET16RELATIVE(pbm_get_emergency_list_resp_msg_v01, network_emer_nums_len),
  26, 0
};

/*
 * pbm_get_all_groups_req_msg is empty
 * static const uint8_t pbm_get_all_groups_req_msg_data_v01[] = {
 * };
 */

static const uint8_t pbm_get_all_groups_resp_msg_data_v01[] = {
  0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_all_groups_resp_msg_v01, resp),
  0, 1,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_all_groups_resp_msg_v01, groups_data) - QMI_IDL_OFFSET8(pbm_get_all_groups_resp_msg_v01, groups_data_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_all_groups_resp_msg_v01, groups_data),
  QMI_PBM_MAX_NUM_GROUPS_DATA_V01,
  QMI_IDL_OFFSET8(pbm_get_all_groups_resp_msg_v01, groups_data) - QMI_IDL_OFFSET8(pbm_get_all_groups_resp_msg_v01, groups_data_len),
  36, 0
};

static const uint8_t pbm_set_group_info_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_set_group_info_req_msg_v01, grp_info),
  37, 0
};

static const uint8_t pbm_set_group_info_resp_msg_data_v01[] = {
  0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_set_group_info_resp_msg_v01, resp),
  0, 1,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_set_group_info_resp_msg_v01, grp_id) - QMI_IDL_OFFSET8(pbm_set_group_info_resp_msg_v01, grp_id_valid)),
  0x10,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_set_group_info_resp_msg_v01, grp_id),
  38, 0
};

static const uint8_t pbm_get_pb_state_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_pb_state_req_msg_v01, phonebook_info),
  0, 0
};

static const uint8_t pbm_get_pb_state_resp_msg_data_v01[] = {
  0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_pb_state_resp_msg_v01, resp),
  0, 1,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_pb_state_resp_msg_v01, phonebook_state) - QMI_IDL_OFFSET8(pbm_get_pb_state_resp_msg_v01, phonebook_state_valid)),
  0x10,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_pb_state_resp_msg_v01, phonebook_state),
  39, 0
};

static const uint8_t pbm_read_all_hidden_records_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_read_all_hidden_records_req_msg_v01, session_type)
};

static const uint8_t pbm_read_all_hidden_records_resp_msg_data_v01[] = {
  0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_read_all_hidden_records_resp_msg_v01, resp),
  0, 1,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_read_all_hidden_records_resp_msg_v01, num_of_recs) - QMI_IDL_OFFSET8(pbm_read_all_hidden_records_resp_msg_v01, num_of_recs_valid)),
  0x10,
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_read_all_hidden_records_resp_msg_v01, num_of_recs)
};

static const uint8_t pbm_hidden_record_status_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_hidden_record_status_ind_msg_v01, status_info),
  40, 0
};

static const uint8_t pbm_get_next_empty_record_id_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_next_empty_record_id_req_msg_v01, record_info),
  41, 0
};

static const uint8_t pbm_get_next_empty_record_id_resp_msg_data_v01[] = {
  0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_next_empty_record_id_resp_msg_v01, resp),
  0, 1,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_next_empty_record_id_resp_msg_v01, record_id) - QMI_IDL_OFFSET8(pbm_get_next_empty_record_id_resp_msg_v01, record_id_valid)),
  0x10,
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_get_next_empty_record_id_resp_msg_v01, record_id)
};

static const uint8_t pbm_get_next_non_empty_record_id_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_next_non_empty_record_id_req_msg_v01, record_info),
  41, 0
};

static const uint8_t pbm_get_next_non_empty_record_id_resp_msg_data_v01[] = {
  0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_next_non_empty_record_id_resp_msg_v01, resp),
  0, 1,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_next_non_empty_record_id_resp_msg_v01, record_id) - QMI_IDL_OFFSET8(pbm_get_next_non_empty_record_id_resp_msg_v01, record_id_valid)),
  0x10,
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(pbm_get_next_non_empty_record_id_resp_msg_v01, record_id)
};

/*
 * pbm_get_all_aas_req_msg is empty
 * static const uint8_t pbm_get_all_aas_req_msg_data_v01[] = {
 * };
 */

static const uint8_t pbm_get_all_aas_resp_msg_data_v01[] = {
  0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_all_aas_resp_msg_v01, resp),
  0, 1,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_all_aas_resp_msg_v01, aas_data) - QMI_IDL_OFFSET8(pbm_get_all_aas_resp_msg_v01, aas_data_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_all_aas_resp_msg_v01, aas_data),
  QMI_PBM_MAX_NUM_SESSIONS_V01,
  QMI_IDL_OFFSET8(pbm_get_all_aas_resp_msg_v01, aas_data) - QMI_IDL_OFFSET8(pbm_get_all_aas_resp_msg_v01, aas_data_len),
  43, 0
};

static const uint8_t pbm_set_aas_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_set_aas_req_msg_v01, aas_info),
  44, 0
};

static const uint8_t pbm_set_aas_resp_msg_data_v01[] = {
  0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_set_aas_resp_msg_v01, resp),
  0, 1,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_set_aas_resp_msg_v01, aas_id) - QMI_IDL_OFFSET8(pbm_set_aas_resp_msg_v01, aas_id_valid)),
  0x10,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_set_aas_resp_msg_v01, aas_id),
  45, 0
};

static const uint8_t pbm_aas_update_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_aas_update_ind_msg_v01, aas_update_info),
  46, 0
};

static const uint8_t pbm_gas_update_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_gas_update_ind_msg_v01, gas_update_info),
  47, 0
};

static const uint8_t pbm_bind_subscription_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_bind_subscription_req_msg_v01, subs_type)
};

static const uint8_t pbm_bind_subscription_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_bind_subscription_resp_msg_v01, resp),
  0, 1
};

/*
 * pbm_get_subscription_binding_req_msg is empty
 * static const uint8_t pbm_get_subscription_binding_req_msg_data_v01[] = {
 * };
 */

static const uint8_t pbm_get_subscription_binding_resp_msg_data_v01[] = {
  0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(pbm_get_subscription_binding_resp_msg_v01, resp),
  0, 1,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(pbm_get_subscription_binding_resp_msg_v01, subs_type) - QMI_IDL_OFFSET8(pbm_get_subscription_binding_resp_msg_v01, subs_type_valid)),
  0x10,
  QMI_IDL_1_BYTE_ENUM,
  QMI_IDL_OFFSET8(pbm_get_subscription_binding_resp_msg_v01, subs_type)
};

/* Type Table */
static const qmi_idl_type_table_entry  pbm_type_table_v01[] = {
  {sizeof(pbm_phonebook_info_type_v01), pbm_phonebook_info_type_data_v01},
  {sizeof(pbm_capability_basic_info_type_v01), pbm_capability_basic_info_type_data_v01},
  {sizeof(pbm_group_capability_type_v01), pbm_group_capability_type_data_v01},
  {sizeof(pbm_ad_num_capability_type_v01), pbm_ad_num_capability_type_data_v01},
  {sizeof(pbm_email_capability_type_v01), pbm_email_capability_type_data_v01},
  {sizeof(pbm_gas_capability_type_v01), pbm_gas_capability_type_data_v01},
  {sizeof(pbm_aas_capability_type_v01), pbm_aas_capability_type_data_v01},
  {sizeof(pbm_capability_basic_info_array_elem_type_v01), pbm_capability_basic_info_array_elem_type_data_v01},
  {sizeof(pbm_capability_basic_info_array_type_v01), pbm_capability_basic_info_array_type_data_v01},
  {sizeof(pbm_group_capability_with_session_type_v01), pbm_group_capability_with_session_type_data_v01},
  {sizeof(pbm_ad_num_capability_with_session_type_v01), pbm_ad_num_capability_with_session_type_data_v01},
  {sizeof(pbm_email_capability_with_session_type_v01), pbm_email_capability_with_session_type_data_v01},
  {sizeof(pbm_second_name_capability_with_session_type_v01), pbm_second_name_capability_with_session_type_data_v01},
  {sizeof(pbm_hidden_records_capability_with_session_type_v01), pbm_hidden_records_capability_with_session_type_data_v01},
  {sizeof(pbm_gas_capability_with_session_type_v01), pbm_gas_capability_with_session_type_data_v01},
  {sizeof(pbm_aas_capability_with_session_type_v01), pbm_aas_capability_with_session_type_data_v01},
  {sizeof(pbm_record_info_type_v01), pbm_record_info_type_data_v01},
  {sizeof(pbm_record_information_type_v01), pbm_record_information_type_data_v01},
  {sizeof(pbm_ad_num_info_type_v01), pbm_ad_num_info_type_data_v01},
  {sizeof(pbm_email_info_type_v01), pbm_email_info_type_data_v01},
  {sizeof(pbm_record_id_type_v01), pbm_record_id_type_data_v01},
  {sizeof(pbm_record_update_info_type_v01), pbm_record_update_info_type_data_v01},
  {sizeof(pbm_refresh_info_type_v01), pbm_refresh_info_type_data_v01},
  {sizeof(pbm_emer_num_type_v01), pbm_emer_num_type_data_v01},
  {sizeof(pbm_ecc_emer_num_type_v01), pbm_ecc_emer_num_type_data_v01},
  {sizeof(pbm_card_emer_num_type_v01), pbm_card_emer_num_type_data_v01},
  {sizeof(pbm_network_emer_num_type_v01), pbm_network_emer_num_type_data_v01},
  {sizeof(pbm_phonebook_ready_info_type_v01), pbm_phonebook_ready_info_type_data_v01},
  {sizeof(pbm_record_instance_type_v01), pbm_record_instance_type_data_v01},
  {sizeof(pbm_basic_record_data_type_v01), pbm_basic_record_data_type_data_v01},
  {sizeof(pbm_sname_info_type_v01), pbm_sname_info_type_data_v01},
  {sizeof(pbm_ad_num_record_info_type_v01), pbm_ad_num_record_info_type_data_v01},
  {sizeof(pbm_grp_id_record_info_type_v01), pbm_grp_id_record_info_type_data_v01},
  {sizeof(pbm_email_record_info_type_v01), pbm_email_record_info_type_data_v01},
  {sizeof(pbm_hidden_record_info_type_v01), pbm_hidden_record_info_type_data_v01},
  {sizeof(pbm_groups_instance_type_v01), pbm_groups_instance_type_data_v01},
  {sizeof(pbm_groups_data_type_v01), pbm_groups_data_type_data_v01},
  {sizeof(pbm_grp_info_type_v01), pbm_grp_info_type_data_v01},
  {sizeof(pbm_grp_id_type_v01), pbm_grp_id_type_data_v01},
  {sizeof(pbm_phonebook_state_type_v01), pbm_phonebook_state_type_data_v01},
  {sizeof(pbm_hidden_status_type_v01), pbm_hidden_status_type_data_v01},
  {sizeof(pbm_base_record_info_type_v01), pbm_base_record_info_type_data_v01},
  {sizeof(pbm_aas_instance_type_v01), pbm_aas_instance_type_data_v01},
  {sizeof(pbm_aas_data_type_v01), pbm_aas_data_type_data_v01},
  {sizeof(pbm_aas_info_type_v01), pbm_aas_info_type_data_v01},
  {sizeof(pbm_aas_id_type_v01), pbm_aas_id_type_data_v01},
  {sizeof(pbm_aas_update_info_type_v01), pbm_aas_update_info_type_data_v01},
  {sizeof(pbm_gas_update_info_type_v01), pbm_gas_update_info_type_data_v01},
  {sizeof(pbm_access_info_type_v01), pbm_access_info_type_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry pbm_message_table_v01[] = {
  {sizeof(pbm_indication_register_req_msg_v01), pbm_indication_register_req_msg_data_v01},
  {sizeof(pbm_indication_register_resp_msg_v01), pbm_indication_register_resp_msg_data_v01},
  {sizeof(pbm_get_pb_capabilities_req_msg_v01), pbm_get_pb_capabilities_req_msg_data_v01},
  {sizeof(pbm_get_pb_capabilities_resp_msg_v01), pbm_get_pb_capabilities_resp_msg_data_v01},
  {0, 0},
  {sizeof(pbm_get_all_pb_capabilities_resp_msg_v01), pbm_get_all_pb_capabilities_resp_msg_data_v01},
  {sizeof(pbm_read_records_req_msg_v01), pbm_read_records_req_msg_data_v01},
  {sizeof(pbm_read_records_resp_msg_v01), pbm_read_records_resp_msg_data_v01},
  {sizeof(pbm_write_record_req_msg_v01), pbm_write_record_req_msg_data_v01},
  {sizeof(pbm_write_record_resp_msg_v01), pbm_write_record_resp_msg_data_v01},
  {sizeof(pbm_delete_record_req_msg_v01), pbm_delete_record_req_msg_data_v01},
  {sizeof(pbm_delete_record_resp_msg_v01), pbm_delete_record_resp_msg_data_v01},
  {sizeof(pbm_delete_all_pb_records_req_msg_v01), pbm_delete_all_pb_records_req_msg_data_v01},
  {sizeof(pbm_delete_all_pb_records_resp_msg_v01), pbm_delete_all_pb_records_resp_msg_data_v01},
  {sizeof(pbm_search_records_req_msg_v01), pbm_search_records_req_msg_data_v01},
  {sizeof(pbm_search_records_resp_msg_v01), pbm_search_records_resp_msg_data_v01},
  {sizeof(pbm_record_update_ind_msg_v01), pbm_record_update_ind_msg_data_v01},
  {sizeof(pbm_refresh_ind_msg_v01), pbm_refresh_ind_msg_data_v01},
  {sizeof(pbm_pb_ready_ind_msg_v01), pbm_pb_ready_ind_msg_data_v01},
  {sizeof(pbm_emergency_list_ind_msg_v01), pbm_emergency_list_ind_msg_data_v01},
  {sizeof(pbm_all_pb_init_done_ind_msg_v01), pbm_all_pb_init_done_ind_msg_data_v01},
  {sizeof(pbm_record_read_ind_msg_v01), pbm_record_read_ind_msg_data_v01},
  {0, 0},
  {sizeof(pbm_get_emergency_list_resp_msg_v01), pbm_get_emergency_list_resp_msg_data_v01},
  {0, 0},
  {sizeof(pbm_get_all_groups_resp_msg_v01), pbm_get_all_groups_resp_msg_data_v01},
  {sizeof(pbm_set_group_info_req_msg_v01), pbm_set_group_info_req_msg_data_v01},
  {sizeof(pbm_set_group_info_resp_msg_v01), pbm_set_group_info_resp_msg_data_v01},
  {sizeof(pbm_get_pb_state_req_msg_v01), pbm_get_pb_state_req_msg_data_v01},
  {sizeof(pbm_get_pb_state_resp_msg_v01), pbm_get_pb_state_resp_msg_data_v01},
  {sizeof(pbm_read_all_hidden_records_req_msg_v01), pbm_read_all_hidden_records_req_msg_data_v01},
  {sizeof(pbm_read_all_hidden_records_resp_msg_v01), pbm_read_all_hidden_records_resp_msg_data_v01},
  {sizeof(pbm_hidden_record_status_ind_msg_v01), pbm_hidden_record_status_ind_msg_data_v01},
  {sizeof(pbm_get_next_empty_record_id_req_msg_v01), pbm_get_next_empty_record_id_req_msg_data_v01},
  {sizeof(pbm_get_next_empty_record_id_resp_msg_v01), pbm_get_next_empty_record_id_resp_msg_data_v01},
  {sizeof(pbm_get_next_non_empty_record_id_req_msg_v01), pbm_get_next_non_empty_record_id_req_msg_data_v01},
  {sizeof(pbm_get_next_non_empty_record_id_resp_msg_v01), pbm_get_next_non_empty_record_id_resp_msg_data_v01},
  {0, 0},
  {sizeof(pbm_get_all_aas_resp_msg_v01), pbm_get_all_aas_resp_msg_data_v01},
  {sizeof(pbm_set_aas_req_msg_v01), pbm_set_aas_req_msg_data_v01},
  {sizeof(pbm_set_aas_resp_msg_v01), pbm_set_aas_resp_msg_data_v01},
  {sizeof(pbm_aas_update_ind_msg_v01), pbm_aas_update_ind_msg_data_v01},
  {sizeof(pbm_gas_update_ind_msg_v01), pbm_gas_update_ind_msg_data_v01},
  {sizeof(pbm_bind_subscription_req_msg_v01), pbm_bind_subscription_req_msg_data_v01},
  {sizeof(pbm_bind_subscription_resp_msg_v01), pbm_bind_subscription_resp_msg_data_v01},
  {0, 0},
  {sizeof(pbm_get_subscription_binding_resp_msg_v01), pbm_get_subscription_binding_resp_msg_data_v01}
};

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object pbm_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *pbm_qmi_idl_type_table_object_referenced_tables_v01[] =
{&pbm_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object pbm_qmi_idl_type_table_object_v01 = {
  sizeof(pbm_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(pbm_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  pbm_type_table_v01,
  pbm_message_table_v01,
  pbm_qmi_idl_type_table_object_referenced_tables_v01
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry pbm_service_command_messages_v01[] = {
  {QMI_PBM_INDICATION_REGISTER_REQ_V01, TYPE16(0, 0), 7},
  {QMI_PBM_GET_PB_CAPABILITIES_REQ_V01, TYPE16(0, 2), 6},
  {QMI_PBM_GET_ALL_PB_CAPABILITIES_REQ_V01, TYPE16(0, 4), 0},
  {QMI_PBM_READ_RECORDS_REQ_V01, TYPE16(0, 6), 10},
  {QMI_PBM_WRITE_RECORD_REQ_V01, TYPE16(0, 8), 1224},
  {QMI_PBM_DELETE_RECORD_REQ_V01, TYPE16(0, 10), 8},
  {QMI_PBM_DELETE_ALL_PB_RECORDS_REQ_V01, TYPE16(0, 12), 6},
  {QMI_PBM_SEARCH_RECORDS_REQ_V01, TYPE16(0, 14), 295},
  {QMI_PBM_GET_EMERGENCY_LIST_REQ_V01, TYPE16(0, 22), 0},
  {QMI_PBM_GET_ALL_GROUPS_REQ_V01, TYPE16(0, 24), 0},
  {QMI_PBM_SET_GROUP_INFO_REQ_V01, TYPE16(0, 26), 248},
  {QMI_PBM_GET_PB_STATE_REQ_V01, TYPE16(0, 28), 6},
  {QMI_PBM_READ_ALL_HIDDEN_RECORDS_REQ_V01, TYPE16(0, 30), 4},
  {QMI_PBM_GET_NEXT_EMPTY_RECORD_ID_REQ_V01, TYPE16(0, 33), 8},
  {QMI_PBM_GET_NEXT_NON_EMPTY_RECORD_ID_REQ_V01, TYPE16(0, 35), 8},
  {QMI_PBM_GET_ALL_AAS_REQ_V01, TYPE16(0, 37), 0},
  {QMI_PBM_SET_AAS_REQ_V01, TYPE16(0, 39), 248},
  {QMI_PBM_BIND_SUBSCRIPTION_REQ_V01, TYPE16(0, 43), 4},
  {QMI_PBM_GET_SUBSCRIPTION_BINDING_REQ_V01, TYPE16(0, 45), 0}
};

static const qmi_idl_service_message_table_entry pbm_service_response_messages_v01[] = {
  {QMI_PBM_INDICATION_REGISTER_RESP_V01, TYPE16(0, 1), 14},
  {QMI_GET_PB_CAPABILITIES_RESP_V01, TYPE16(0, 3), 70},
  {QMI_PBM_GET_ALL_PB_CAPABILITIES_RESP_V01, TYPE16(0, 5), 567},
  {QMI_PBM_READ_RECORDS_RESP_V01, TYPE16(0, 7), 12},
  {QMI_PBM_WRITE_RECORD_RESP_V01, TYPE16(0, 9), 12},
  {QMI_PBM_DELETE_RECORD_RESP_V01, TYPE16(0, 11), 12},
  {QMI_PBM_DELETE_ALL_PB_RECORDS_RESP_V01, TYPE16(0, 13), 7},
  {QMI_PBM_SEARCH_RECORDS_RESP_V01, TYPE16(0, 15), 2012},
  {QMI_PBM_GET_EMERGENCY_LIST_RESP_V01, TYPE16(0, 23), 13202},
  {QMI_PBM_GET_ALL_GROUPS_RESP_V01, TYPE16(0, 25), 58343},
  {QMI_PBM_SET_GROUP_INFO_RESP_V01, TYPE16(0, 27), 12},
  {QMI_PBM_GET_PB_STATE_RESP_V01, TYPE16(0, 29), 14},
  {QMI_PBM_READ_ALL_HIDDEN_RECORDS_RESP_V01, TYPE16(0, 31), 12},
  {QMI_PBM_GET_NEXT_EMPTY_RECORD_ID_RESP_V01, TYPE16(0, 34), 12},
  {QMI_PBM_GET_NEXT_NON_EMPTY_RECORD_ID_RESP_V01, TYPE16(0, 36), 12},
  {QMI_PBM_GET_ALL_AAS_RESP_V01, TYPE16(0, 38), 58343},
  {QMI_PBM_SET_AAS_RESP_V01, TYPE16(0, 40), 12},
  {QMI_PBM_BIND_SUBSCRIPTION_RESP_V01, TYPE16(0, 44), 7},
  {QMI_PBM_GET_SUBSCRIPTION_BINDING_RESP_V01, TYPE16(0, 46), 11}
};

static const qmi_idl_service_message_table_entry pbm_service_indication_messages_v01[] = {
  {QMI_PBM_RECORD_READ_IND_V01, TYPE16(0, 21), 12159},
  {QMI_PBM_RECORD_UPDATE_IND_V01, TYPE16(0, 16), 9},
  {QMI_PBM_REFRESH_IND_V01, TYPE16(0, 17), 7},
  {QMI_PBM_PB_READY_IND_V01, TYPE16(0, 18), 6},
  {QMI_PBM_EMERGENCY_LIST_IND_V01, TYPE16(0, 19), 13195},
  {QMI_PBM_ALL_PB_INIT_DONE_IND_V01, TYPE16(0, 20), 22},
  {QMI_PBM_HIDDEN_RECORD_STATUS_IND_V01, TYPE16(0, 32), 5},
  {QMI_PBM_AAS_UPDATE_IND_V01, TYPE16(0, 41), 248},
  {QMI_PBM_GAS_UPDATE_IND_V01, TYPE16(0, 42), 248}
};

/*Service Object*/
const struct qmi_idl_service_object pbm_qmi_idl_service_object_v01 = {
  0x04,
  0x01,
  0x0C,
  58343,
  { sizeof(pbm_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(pbm_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(pbm_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { pbm_service_command_messages_v01, pbm_service_response_messages_v01, pbm_service_indication_messages_v01},
  &pbm_qmi_idl_type_table_object_v01
};

/* Service Object Accessor */
qmi_idl_service_object_type pbm_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( PBM_V01_IDL_MAJOR_VERS != idl_maj_version || PBM_V01_IDL_MINOR_VERS != idl_min_version
       || PBM_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&pbm_qmi_idl_service_object_v01;
}

