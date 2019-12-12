/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M C M _ N W _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the mcm_nw service Data structures.

  Copyright (c) 2013, 2017 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Mon Oct 23 2017 (Spin 0)
   From IDL File: mcm_nw_v01.idl */


/*============================================================================== 
 
                        EDIT HISTORY FOR MODULE 
 
This section contains comments describing changes made to the module. 
Notice that changes are listed in reverse chronological order. 
 
 
WHEN                WHO          WHAT, WHERE, WHY 
----------         ----------    ------------------------------------------------
24/05/2019         Nebula Li     Fix error:The time message got by QL_MCM_NW_GetNitzTimeInfo is incomplete.
                                 NO event report after registering event by QL_MCM_NW_EventRegister.

================================================================================*/ 



#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "mcm_nw_v01.h"
#include "mcm_common_v01.h"
#include "mcm_service_object_v01.h"

/*Type Definitions*/
static const uint8_t mcm_nw_operator_name_t_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_nw_operator_name_t_v01, long_eons),
  ((512) & 0xFF), ((512) >> 8),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(mcm_nw_operator_name_t_v01, short_eons),
  ((512) & 0xFF), ((512) >> 8),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(mcm_nw_operator_name_t_v01, mcc),
  3,

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(mcm_nw_operator_name_t_v01, mnc),
  3,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_nw_scan_entry_t_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_scan_entry_t_v01, operator_name),
  QMI_IDL_TYPE88(0, 0),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_nw_scan_entry_t_v01, network_status),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_nw_scan_entry_t_v01, rat),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_nw_common_registration_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_common_registration_t_v01, tech_domain),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_common_registration_t_v01, radio_tech),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_common_registration_t_v01, roaming),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_common_registration_t_v01, deny_reason),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_common_registration_t_v01, registration_state),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_nw_3gpp_registration_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_3gpp_registration_t_v01, tech_domain),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_3gpp_registration_t_v01, radio_tech),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_nw_3gpp_registration_t_v01, mcc),
  3,

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_nw_3gpp_registration_t_v01, mnc),
  3,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_3gpp_registration_t_v01, roaming),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_3gpp_registration_t_v01, forbidden),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_3gpp_registration_t_v01, cid),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_3gpp_registration_t_v01, lac),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_3gpp_registration_t_v01, psc),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_3gpp_registration_t_v01, tac),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_nw_3gpp2_registration_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_3gpp2_registration_t_v01, tech_domain),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_3gpp2_registration_t_v01, radio_tech),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_nw_3gpp2_registration_t_v01, mcc),
  3,

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_nw_3gpp2_registration_t_v01, mnc),
  3,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_3gpp2_registration_t_v01, roaming),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_3gpp2_registration_t_v01, forbidden),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_3gpp2_registration_t_v01, inPRL),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_3gpp2_registration_t_v01, css),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_3gpp2_registration_t_v01, sid),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_3gpp2_registration_t_v01, nid),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_3gpp2_registration_t_v01, bsid),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_nw_selection_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_selection_t_v01, nw_selection_type),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_nw_selection_t_v01, mcc),
  3,

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_nw_selection_t_v01, mnc),
  3,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_selection_t_v01, rat),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_nw_gsm_info_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_gsm_info_t_v01, cid),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_gsm_info_t_v01, plmn),
  3,

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_gsm_info_t_v01, lac),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_gsm_info_t_v01, arfcn),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_gsm_info_t_v01, bsic),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_nw_umts_info_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_umts_info_t_v01, cid),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_umts_info_t_v01, lcid),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_umts_info_t_v01, plmn),
  3,

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_umts_info_t_v01, lac),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_umts_info_t_v01, uarfcn),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_umts_info_t_v01, psc),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_nw_lte_info_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_lte_info_t_v01, cid),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_lte_info_t_v01, plmn),
  3,

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_lte_info_t_v01, tac),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_lte_info_t_v01, pci),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_lte_info_t_v01, earfcn),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_nw_gsm_sig_info_t_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_gsm_sig_info_t_v01, rssi),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_nw_wcdma_sig_info_t_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_wcdma_sig_info_t_v01, rssi),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_wcdma_sig_info_t_v01, ecio),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_nw_tdscdma_sig_info_t_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_tdscdma_sig_info_t_v01, rssi),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_tdscdma_sig_info_t_v01, rscp),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_tdscdma_sig_info_t_v01, ecio),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_tdscdma_sig_info_t_v01, sinr),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_nw_lte_sig_info_t_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_lte_sig_info_t_v01, rssi),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_lte_sig_info_t_v01, rsrq),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_lte_sig_info_t_v01, rsrp),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_lte_sig_info_t_v01, snr),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_nw_cdma_sig_info_t_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_cdma_sig_info_t_v01, rssi),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_cdma_sig_info_t_v01, ecio),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_nw_hdr_sig_info_t_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_hdr_sig_info_t_v01, rssi),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_hdr_sig_info_t_v01, ecio),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_hdr_sig_info_t_v01, sinr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_hdr_sig_info_t_v01, io),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_nw_nitz_time_t_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |QMI_IDL_STRING,
  QMI_IDL_OFFSET8(mcm_nw_nitz_time_t_v01, nitz_time),
  MCM_NW_NITZ_STR_BUF_MAX_V01,

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t mcm_nw_set_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_set_config_req_msg_v01, preferred_nw_mode) - QMI_IDL_OFFSET8(mcm_nw_set_config_req_msg_v01, preferred_nw_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_set_config_req_msg_v01, preferred_nw_mode),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_set_config_req_msg_v01, roaming_pref) - QMI_IDL_OFFSET8(mcm_nw_set_config_req_msg_v01, roaming_pref_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_set_config_req_msg_v01, roaming_pref)
};

static const uint8_t mcm_nw_set_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_set_config_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * mcm_nw_get_config_req_msg is empty
 * static const uint8_t mcm_nw_get_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t mcm_nw_get_config_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_get_config_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_get_config_resp_msg_v01, preferred_nw_mode) - QMI_IDL_OFFSET8(mcm_nw_get_config_resp_msg_v01, preferred_nw_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_get_config_resp_msg_v01, preferred_nw_mode),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_get_config_resp_msg_v01, roaming_pref) - QMI_IDL_OFFSET8(mcm_nw_get_config_resp_msg_v01, roaming_pref_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_get_config_resp_msg_v01, roaming_pref)
};

/*
 * mcm_nw_get_registration_status_req_msg is empty
 * static const uint8_t mcm_nw_get_registration_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t mcm_nw_get_registration_status_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_get_registration_status_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_get_registration_status_resp_msg_v01, voice_registration) - QMI_IDL_OFFSET8(mcm_nw_get_registration_status_resp_msg_v01, voice_registration_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_get_registration_status_resp_msg_v01, voice_registration),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_get_registration_status_resp_msg_v01, data_registration) - QMI_IDL_OFFSET8(mcm_nw_get_registration_status_resp_msg_v01, data_registration_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_get_registration_status_resp_msg_v01, data_registration),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_get_registration_status_resp_msg_v01, voice_registration_details_3gpp) - QMI_IDL_OFFSET8(mcm_nw_get_registration_status_resp_msg_v01, voice_registration_details_3gpp_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_get_registration_status_resp_msg_v01, voice_registration_details_3gpp),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_get_registration_status_resp_msg_v01, data_registration_details_3gpp) - QMI_IDL_OFFSET8(mcm_nw_get_registration_status_resp_msg_v01, data_registration_details_3gpp_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_get_registration_status_resp_msg_v01, data_registration_details_3gpp),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_get_registration_status_resp_msg_v01, voice_registration_details_3gpp2) - QMI_IDL_OFFSET8(mcm_nw_get_registration_status_resp_msg_v01, voice_registration_details_3gpp2_valid)),
  0x14,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_get_registration_status_resp_msg_v01, voice_registration_details_3gpp2),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_get_registration_status_resp_msg_v01, data_registration_details_3gpp2) - QMI_IDL_OFFSET8(mcm_nw_get_registration_status_resp_msg_v01, data_registration_details_3gpp2_valid)),
  0x15,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_get_registration_status_resp_msg_v01, data_registration_details_3gpp2),
  QMI_IDL_TYPE88(0, 4)
};

/*
 * mcm_nw_scan_req_msg is empty
 * static const uint8_t mcm_nw_scan_req_msg_data_v01[] = {
 * };
 */

static const uint8_t mcm_nw_scan_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_scan_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_scan_resp_msg_v01, entry) - QMI_IDL_OFFSET8(mcm_nw_scan_resp_msg_v01, entry_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_scan_resp_msg_v01, entry),
  MCM_NW_SCAN_LIST_MAX_V01,
  QMI_IDL_OFFSET8(mcm_nw_scan_resp_msg_v01, entry) - QMI_IDL_OFFSET8(mcm_nw_scan_resp_msg_v01, entry_len),
  QMI_IDL_TYPE88(0, 1)
};

/*
 * mcm_nw_get_operator_name_req_msg is empty
 * static const uint8_t mcm_nw_get_operator_name_req_msg_data_v01[] = {
 * };
 */

static const uint8_t mcm_nw_get_operator_name_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_get_operator_name_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_get_operator_name_resp_msg_v01, operator_name) - QMI_IDL_OFFSET8(mcm_nw_get_operator_name_resp_msg_v01, operator_name_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_get_operator_name_resp_msg_v01, operator_name),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t mcm_nw_screen_on_off_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_screen_on_off_req_msg_v01, turn_off_screen)
};

static const uint8_t mcm_nw_screen_on_off_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_screen_on_off_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_nw_selection_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_selection_req_msg_v01, nw_selection_info),
  QMI_IDL_TYPE88(0, 5)
};

static const uint8_t mcm_nw_selection_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_selection_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * mcm_nw_get_signal_strength_req_msg is empty
 * static const uint8_t mcm_nw_get_signal_strength_req_msg_data_v01[] = {
 * };
 */

static const uint8_t mcm_nw_get_signal_strength_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_get_signal_strength_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_get_signal_strength_resp_msg_v01, gsm_sig_info) - QMI_IDL_OFFSET8(mcm_nw_get_signal_strength_resp_msg_v01, gsm_sig_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_get_signal_strength_resp_msg_v01, gsm_sig_info),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_get_signal_strength_resp_msg_v01, wcdma_sig_info) - QMI_IDL_OFFSET8(mcm_nw_get_signal_strength_resp_msg_v01, wcdma_sig_info_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_get_signal_strength_resp_msg_v01, wcdma_sig_info),
  QMI_IDL_TYPE88(0, 7),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_get_signal_strength_resp_msg_v01, tdscdma_sig_info) - QMI_IDL_OFFSET8(mcm_nw_get_signal_strength_resp_msg_v01, tdscdma_sig_info_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_get_signal_strength_resp_msg_v01, tdscdma_sig_info),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_get_signal_strength_resp_msg_v01, lte_sig_info) - QMI_IDL_OFFSET8(mcm_nw_get_signal_strength_resp_msg_v01, lte_sig_info_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_get_signal_strength_resp_msg_v01, lte_sig_info),
  QMI_IDL_TYPE88(0, 9),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_get_signal_strength_resp_msg_v01, cdma_sig_info) - QMI_IDL_OFFSET8(mcm_nw_get_signal_strength_resp_msg_v01, cdma_sig_info_valid)),
  0x14,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_get_signal_strength_resp_msg_v01, cdma_sig_info),
  QMI_IDL_TYPE88(0, 10),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_get_signal_strength_resp_msg_v01, hdr_sig_info) - QMI_IDL_OFFSET8(mcm_nw_get_signal_strength_resp_msg_v01, hdr_sig_info_valid)),
  0x15,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_get_signal_strength_resp_msg_v01, hdr_sig_info),
  QMI_IDL_TYPE88(0, 11)
};

/*
 * mcm_nw_get_cell_access_state_req_msg is empty
 * static const uint8_t mcm_nw_get_cell_access_state_req_msg_data_v01[] = {
 * };
 */

static const uint8_t mcm_nw_get_cell_access_state_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_get_cell_access_state_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_get_cell_access_state_resp_msg_v01, nw_cell_access_state) - QMI_IDL_OFFSET8(mcm_nw_get_cell_access_state_resp_msg_v01, nw_cell_access_state_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_get_cell_access_state_resp_msg_v01, nw_cell_access_state)
};

/*
 * mcm_nw_get_nitz_time_info_req_msg is empty
 * static const uint8_t mcm_nw_get_nitz_time_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t mcm_nw_get_nitz_time_info_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_get_nitz_time_info_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_get_nitz_time_info_resp_msg_v01, nw_nitz_time) - QMI_IDL_OFFSET8(mcm_nw_get_nitz_time_info_resp_msg_v01, nw_nitz_time_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_get_nitz_time_info_resp_msg_v01, nw_nitz_time),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_get_nitz_time_info_resp_msg_v01, abs_time) - QMI_IDL_OFFSET8(mcm_nw_get_nitz_time_info_resp_msg_v01, abs_time_valid)),
  0x11,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_get_nitz_time_info_resp_msg_v01, abs_time),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_get_nitz_time_info_resp_msg_v01, leap_sec) - QMI_IDL_OFFSET8(mcm_nw_get_nitz_time_info_resp_msg_v01, leap_sec_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_get_nitz_time_info_resp_msg_v01, leap_sec)
};

static const uint8_t mcm_nw_event_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_event_register_req_msg_v01, register_voice_registration_event) - QMI_IDL_OFFSET8(mcm_nw_event_register_req_msg_v01, register_voice_registration_event_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_event_register_req_msg_v01, register_voice_registration_event),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_event_register_req_msg_v01, register_data_registration_event) - QMI_IDL_OFFSET8(mcm_nw_event_register_req_msg_v01, register_data_registration_event_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_event_register_req_msg_v01, register_data_registration_event),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_event_register_req_msg_v01, register_signal_strength_event) - QMI_IDL_OFFSET8(mcm_nw_event_register_req_msg_v01, register_signal_strength_event_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_event_register_req_msg_v01, register_signal_strength_event),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_event_register_req_msg_v01, register_cell_access_state_change_event) - QMI_IDL_OFFSET8(mcm_nw_event_register_req_msg_v01, register_cell_access_state_change_event_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_event_register_req_msg_v01, register_cell_access_state_change_event),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_event_register_req_msg_v01, register_nitz_time_update_event) - QMI_IDL_OFFSET8(mcm_nw_event_register_req_msg_v01, register_nitz_time_update_event_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_event_register_req_msg_v01, register_nitz_time_update_event)
};

static const uint8_t mcm_nw_event_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_event_register_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * mcm_nw_get_cell_info_req_msg is empty
 * static const uint8_t mcm_nw_get_cell_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t mcm_nw_get_cell_info_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_get_cell_info_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_get_cell_info_resp_msg_v01, serving_rat),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_get_cell_info_resp_msg_v01, gsm_info) - QMI_IDL_OFFSET8(mcm_nw_get_cell_info_resp_msg_v01, gsm_info_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_get_cell_info_resp_msg_v01, gsm_info),
  MCM_CELL_INFO_GSM_MAX_V01,
  QMI_IDL_OFFSET8(mcm_nw_get_cell_info_resp_msg_v01, gsm_info) - QMI_IDL_OFFSET8(mcm_nw_get_cell_info_resp_msg_v01, gsm_info_len),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mcm_nw_get_cell_info_resp_msg_v01, umts_info) - QMI_IDL_OFFSET16RELATIVE(mcm_nw_get_cell_info_resp_msg_v01, umts_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(mcm_nw_get_cell_info_resp_msg_v01, umts_info),
  MCM_CELL_INFO_UMTS_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(mcm_nw_get_cell_info_resp_msg_v01, umts_info) - QMI_IDL_OFFSET16RELATIVE(mcm_nw_get_cell_info_resp_msg_v01, umts_info_len),
  QMI_IDL_TYPE88(0, 7),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mcm_nw_get_cell_info_resp_msg_v01, lte_info) - QMI_IDL_OFFSET16RELATIVE(mcm_nw_get_cell_info_resp_msg_v01, lte_info_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(mcm_nw_get_cell_info_resp_msg_v01, lte_info),
  MCM_CELL_INFO_LTE_MAX_V01,
  QMI_IDL_OFFSET16RELATIVE(mcm_nw_get_cell_info_resp_msg_v01, lte_info) - QMI_IDL_OFFSET16RELATIVE(mcm_nw_get_cell_info_resp_msg_v01, lte_info_len),
  QMI_IDL_TYPE88(0, 8)
};

static const uint8_t mcm_nw_voice_registration_event_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_voice_registration_event_ind_msg_v01, voice_registration) - QMI_IDL_OFFSET8(mcm_nw_voice_registration_event_ind_msg_v01, voice_registration_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_voice_registration_event_ind_msg_v01, voice_registration),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_voice_registration_event_ind_msg_v01, voice_registration_details_3gpp) - QMI_IDL_OFFSET8(mcm_nw_voice_registration_event_ind_msg_v01, voice_registration_details_3gpp_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_voice_registration_event_ind_msg_v01, voice_registration_details_3gpp),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_voice_registration_event_ind_msg_v01, voice_registration_details_3gpp2) - QMI_IDL_OFFSET8(mcm_nw_voice_registration_event_ind_msg_v01, voice_registration_details_3gpp2_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_voice_registration_event_ind_msg_v01, voice_registration_details_3gpp2),
  QMI_IDL_TYPE88(0, 4)
};

static const uint8_t mcm_nw_data_registration_event_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_data_registration_event_ind_msg_v01, data_registration) - QMI_IDL_OFFSET8(mcm_nw_data_registration_event_ind_msg_v01, data_registration_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_data_registration_event_ind_msg_v01, data_registration),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_data_registration_event_ind_msg_v01, data_registration_details_3gpp) - QMI_IDL_OFFSET8(mcm_nw_data_registration_event_ind_msg_v01, data_registration_details_3gpp_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_data_registration_event_ind_msg_v01, data_registration_details_3gpp),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_data_registration_event_ind_msg_v01, data_registration_details_3gpp2) - QMI_IDL_OFFSET8(mcm_nw_data_registration_event_ind_msg_v01, data_registration_details_3gpp2_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_data_registration_event_ind_msg_v01, data_registration_details_3gpp2),
  QMI_IDL_TYPE88(0, 4)
};

static const uint8_t mcm_nw_signal_strength_event_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_signal_strength_event_ind_msg_v01, gsm_sig_info) - QMI_IDL_OFFSET8(mcm_nw_signal_strength_event_ind_msg_v01, gsm_sig_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_signal_strength_event_ind_msg_v01, gsm_sig_info),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_signal_strength_event_ind_msg_v01, wcdma_sig_info) - QMI_IDL_OFFSET8(mcm_nw_signal_strength_event_ind_msg_v01, wcdma_sig_info_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_signal_strength_event_ind_msg_v01, wcdma_sig_info),
  QMI_IDL_TYPE88(0, 7),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_signal_strength_event_ind_msg_v01, tdscdma_sig_info) - QMI_IDL_OFFSET8(mcm_nw_signal_strength_event_ind_msg_v01, tdscdma_sig_info_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_signal_strength_event_ind_msg_v01, tdscdma_sig_info),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_signal_strength_event_ind_msg_v01, lte_sig_info) - QMI_IDL_OFFSET8(mcm_nw_signal_strength_event_ind_msg_v01, lte_sig_info_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_signal_strength_event_ind_msg_v01, lte_sig_info),
  QMI_IDL_TYPE88(0, 9),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_signal_strength_event_ind_msg_v01, cdma_sig_info) - QMI_IDL_OFFSET8(mcm_nw_signal_strength_event_ind_msg_v01, cdma_sig_info_valid)),
  0x14,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_signal_strength_event_ind_msg_v01, cdma_sig_info),
  QMI_IDL_TYPE88(0, 10),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_signal_strength_event_ind_msg_v01, hdr_sig_info) - QMI_IDL_OFFSET8(mcm_nw_signal_strength_event_ind_msg_v01, hdr_sig_info_valid)),
  0x15,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_signal_strength_event_ind_msg_v01, hdr_sig_info),
  QMI_IDL_TYPE88(0, 11)
};

static const uint8_t mcm_nw_cell_access_state_change_event_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_cell_access_state_change_event_ind_msg_v01, nw_cell_access_state)
};

static const uint8_t mcm_nw_nitz_time_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_nitz_time_ind_msg_v01, nw_nitz_time) - QMI_IDL_OFFSET8(mcm_nw_nitz_time_ind_msg_v01, nw_nitz_time_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_nw_nitz_time_ind_msg_v01, nw_nitz_time),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_nitz_time_ind_msg_v01, abs_time) - QMI_IDL_OFFSET8(mcm_nw_nitz_time_ind_msg_v01, abs_time_valid)),
  0x11,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_nitz_time_ind_msg_v01, abs_time),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_nw_nitz_time_ind_msg_v01, leap_sec) - QMI_IDL_OFFSET8(mcm_nw_nitz_time_ind_msg_v01, leap_sec_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_nw_nitz_time_ind_msg_v01, leap_sec)
};

/* Type Table */
static const qmi_idl_type_table_entry  mcm_nw_type_table_v01[] = {
  {sizeof(mcm_nw_operator_name_t_v01), mcm_nw_operator_name_t_data_v01},
  {sizeof(mcm_nw_scan_entry_t_v01), mcm_nw_scan_entry_t_data_v01},
  {sizeof(mcm_nw_common_registration_t_v01), mcm_nw_common_registration_t_data_v01},
  {sizeof(mcm_nw_3gpp_registration_t_v01), mcm_nw_3gpp_registration_t_data_v01},
  {sizeof(mcm_nw_3gpp2_registration_t_v01), mcm_nw_3gpp2_registration_t_data_v01},
  {sizeof(mcm_nw_selection_t_v01), mcm_nw_selection_t_data_v01},

/**
* Nebula Li -2019/05/24: [delete]
* Fix error: The time message got by QL_MCM_NW_GetNitzTimeInfo is incomplete.
* These message can not be here. They will break index of the follow messeage.

    @{
*/

  {sizeof(mcm_nw_gsm_info_t_v01), mcm_nw_gsm_info_t_data_v01},
  {sizeof(mcm_nw_umts_info_t_v01), mcm_nw_umts_info_t_data_v01},
  {sizeof(mcm_nw_lte_info_t_v01), mcm_nw_lte_info_t_data_v01},

/** 
    @}
*/

  {sizeof(mcm_nw_gsm_sig_info_t_v01), mcm_nw_gsm_sig_info_t_data_v01},
  {sizeof(mcm_nw_wcdma_sig_info_t_v01), mcm_nw_wcdma_sig_info_t_data_v01},
  {sizeof(mcm_nw_tdscdma_sig_info_t_v01), mcm_nw_tdscdma_sig_info_t_data_v01},
  {sizeof(mcm_nw_lte_sig_info_t_v01), mcm_nw_lte_sig_info_t_data_v01},
  {sizeof(mcm_nw_cdma_sig_info_t_v01), mcm_nw_cdma_sig_info_t_data_v01},
  {sizeof(mcm_nw_hdr_sig_info_t_v01), mcm_nw_hdr_sig_info_t_data_v01},
  {sizeof(mcm_nw_nitz_time_t_v01), mcm_nw_nitz_time_t_data_v01},

/**
* Nebula Li -2019/05/24: [add]
* Fix error: The time message got by QL_MCM_NW_GetNitzTimeInfo is incomplete.
* 
    @{
*/

 // {sizeof(mcm_nw_gsm_info_t_v01), mcm_nw_gsm_info_t_data_v01},
 // {sizeof(mcm_nw_umts_info_t_v01), mcm_nw_umts_info_t_data_v01},
 // {sizeof(mcm_nw_lte_info_t_v01), mcm_nw_lte_info_t_data_v01},

/** 
    @}
*/
};

/* Message Table */
static const qmi_idl_message_table_entry mcm_nw_message_table_v01[] = {
  {sizeof(mcm_nw_set_config_req_msg_v01), mcm_nw_set_config_req_msg_data_v01},
  {sizeof(mcm_nw_set_config_resp_msg_v01), mcm_nw_set_config_resp_msg_data_v01},
  {sizeof(mcm_nw_get_config_req_msg_v01), 0},
  {sizeof(mcm_nw_get_config_resp_msg_v01), mcm_nw_get_config_resp_msg_data_v01},
  {sizeof(mcm_nw_get_registration_status_req_msg_v01), 0},
  {sizeof(mcm_nw_get_registration_status_resp_msg_v01), mcm_nw_get_registration_status_resp_msg_data_v01},
  {sizeof(mcm_nw_scan_req_msg_v01), 0},
  {sizeof(mcm_nw_scan_resp_msg_v01), mcm_nw_scan_resp_msg_data_v01},
  {sizeof(mcm_nw_get_operator_name_req_msg_v01), 0},
  {sizeof(mcm_nw_get_operator_name_resp_msg_v01), mcm_nw_get_operator_name_resp_msg_data_v01},
  {sizeof(mcm_nw_screen_on_off_req_msg_v01), mcm_nw_screen_on_off_req_msg_data_v01},
  {sizeof(mcm_nw_screen_on_off_resp_msg_v01), mcm_nw_screen_on_off_resp_msg_data_v01},
  {sizeof(mcm_nw_selection_req_msg_v01), mcm_nw_selection_req_msg_data_v01},
  {sizeof(mcm_nw_selection_resp_msg_v01), mcm_nw_selection_resp_msg_data_v01},
  {sizeof(mcm_nw_get_signal_strength_req_msg_v01), 0},
  {sizeof(mcm_nw_get_signal_strength_resp_msg_v01), mcm_nw_get_signal_strength_resp_msg_data_v01},
  {sizeof(mcm_nw_get_cell_access_state_req_msg_v01), 0},
  {sizeof(mcm_nw_get_cell_access_state_resp_msg_v01), mcm_nw_get_cell_access_state_resp_msg_data_v01},
  {sizeof(mcm_nw_get_nitz_time_info_req_msg_v01), 0},
  {sizeof(mcm_nw_get_nitz_time_info_resp_msg_v01), mcm_nw_get_nitz_time_info_resp_msg_data_v01},
  {sizeof(mcm_nw_event_register_req_msg_v01), mcm_nw_event_register_req_msg_data_v01},
  {sizeof(mcm_nw_event_register_resp_msg_v01), mcm_nw_event_register_resp_msg_data_v01},
/**
* Nebula Li -2019/05/24: [delete]
* Fix error: NO event report after registering event by QL_MCM_NW_EventRegister.
* These message can not be here. They will break index of the follow messeage.
    @{
*/
  //{sizeof(mcm_nw_get_cell_info_req_msg_v01), 0},
  //{sizeof(mcm_nw_get_cell_info_resp_msg_v01), mcm_nw_get_cell_info_resp_msg_data_v01},
/** 
    @}
*/
  
  {sizeof(mcm_nw_voice_registration_event_ind_msg_v01), mcm_nw_voice_registration_event_ind_msg_data_v01},
  {sizeof(mcm_nw_data_registration_event_ind_msg_v01), mcm_nw_data_registration_event_ind_msg_data_v01},
  {sizeof(mcm_nw_signal_strength_event_ind_msg_v01), mcm_nw_signal_strength_event_ind_msg_data_v01},
  {sizeof(mcm_nw_cell_access_state_change_event_ind_msg_v01), mcm_nw_cell_access_state_change_event_ind_msg_data_v01},
  {sizeof(mcm_nw_nitz_time_ind_msg_v01), mcm_nw_nitz_time_ind_msg_data_v01},

/**
* Nebula Li -2019/05/24: [add]
* Fix error: NO event report after registering event by QL_MCM_NW_EventRegister.

    @{
*/
  
  {sizeof(mcm_nw_get_cell_info_req_msg_v01), 0},
  {sizeof(mcm_nw_get_cell_info_resp_msg_v01), mcm_nw_get_cell_info_resp_msg_data_v01}
   
/** 
    @}
*/
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
const qmi_idl_type_table_object mcm_nw_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *mcm_nw_qmi_idl_type_table_object_referenced_tables_v01[] =
{&mcm_nw_qmi_idl_type_table_object_v01, &mcm_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
const qmi_idl_type_table_object mcm_nw_qmi_idl_type_table_object_v01 = {
  sizeof(mcm_nw_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(mcm_nw_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  mcm_nw_type_table_v01,
  mcm_nw_message_table_v01,
  mcm_nw_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

