/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M C M _ D A T A _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the mcm_data service Data structures.

  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.5
   It was generated on: Wed Sep 11 2013 (Spin 0)
   From IDL File: mcm_data_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "mcm_data_v01.h"
#include "mcm_common_v01.h"
#include "mcm_service_object_v01.h"

/*Type Definitions*/
static const uint8_t mcm_data_reg_status_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_reg_status_t_v01, srv_status),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_reg_status_t_v01, tech_info),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_data_pkt_stats_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_pkt_stats_t_v01, pkts_tx),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_pkt_stats_t_v01, pkts_rx),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(mcm_data_pkt_stats_t_v01, bytes_tx),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(mcm_data_pkt_stats_t_v01, bytes_rx),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_pkt_stats_t_v01, pkts_dropped_tx),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_pkt_stats_t_v01, pkts_dropped_rx),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_data_channel_rate_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_channel_rate_t_v01, current_tx_rate),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_channel_rate_t_v01, current_rx_rate),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_channel_rate_t_v01, max_tx_rate),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_channel_rate_t_v01, max_rx_rate),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_data_addr_t_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_data_addr_t_v01, valid_addr),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_data_addr_t_v01, addr),
  MCM_DATA_MAX_ADDR_LEN_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_data_addr_t_info_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_data_addr_t_info_v01, iface_addr_s),
  QMI_IDL_TYPE88(0, 3),
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_addr_t_info_v01, iface_mask),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_data_addr_t_info_v01, gtwy_addr_s),
  QMI_IDL_TYPE88(0, 3),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_data_addr_t_info_v01, gtwy_mask),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(mcm_data_addr_t_info_v01, dnsp_addr_s),
  QMI_IDL_TYPE88(0, 3),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(mcm_data_addr_t_info_v01, dnss_addr_s),
  QMI_IDL_TYPE88(0, 3),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_data_verbose_call_end_reason_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_verbose_call_end_reason_t_v01, call_end_reason_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_verbose_call_end_reason_t_v01, call_end_reason_code),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t mcm_data_start_data_call_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_start_data_call_req_msg_v01, ip_family) - QMI_IDL_OFFSET8(mcm_data_start_data_call_req_msg_v01, ip_family_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_data_start_data_call_req_msg_v01, ip_family),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_start_data_call_req_msg_v01, apn_name) - QMI_IDL_OFFSET8(mcm_data_start_data_call_req_msg_v01, apn_name_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_data_start_data_call_req_msg_v01, apn_name),
  MCM_DATA_MAX_APN_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_start_data_call_req_msg_v01, user_name) - QMI_IDL_OFFSET8(mcm_data_start_data_call_req_msg_v01, user_name_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_data_start_data_call_req_msg_v01, user_name),
  MCM_DATA_MAX_USERNAME_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mcm_data_start_data_call_req_msg_v01, password) - QMI_IDL_OFFSET16RELATIVE(mcm_data_start_data_call_req_msg_v01, password_valid)),
  0x13,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_data_start_data_call_req_msg_v01, password),
  MCM_DATA_MAX_PASSWORD_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mcm_data_start_data_call_req_msg_v01, tech_pref) - QMI_IDL_OFFSET16RELATIVE(mcm_data_start_data_call_req_msg_v01, tech_pref_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_data_start_data_call_req_msg_v01, tech_pref),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mcm_data_start_data_call_req_msg_v01, umts_profile) - QMI_IDL_OFFSET16RELATIVE(mcm_data_start_data_call_req_msg_v01, umts_profile_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_data_start_data_call_req_msg_v01, umts_profile),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mcm_data_start_data_call_req_msg_v01, cdma_profile) - QMI_IDL_OFFSET16RELATIVE(mcm_data_start_data_call_req_msg_v01, cdma_profile_valid)),
  0x16,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(mcm_data_start_data_call_req_msg_v01, cdma_profile)
};

static const uint8_t mcm_data_start_data_call_rsp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_data_start_data_call_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_start_data_call_rsp_msg_v01, call_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_start_data_call_rsp_msg_v01, call_id) - QMI_IDL_OFFSET8(mcm_data_start_data_call_rsp_msg_v01, call_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_start_data_call_rsp_msg_v01, call_id),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_start_data_call_rsp_msg_v01, vce_reason) - QMI_IDL_OFFSET8(mcm_data_start_data_call_rsp_msg_v01, vce_reason_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_data_start_data_call_rsp_msg_v01, vce_reason),
  QMI_IDL_TYPE88(0, 5)
};

static const uint8_t mcm_data_stop_data_call_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_stop_data_call_req_msg_v01, call_id)
};

static const uint8_t mcm_data_stop_data_call_rsp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_data_stop_data_call_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_data_get_pkt_stats_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_get_pkt_stats_req_msg_v01, call_id)
};

static const uint8_t mcm_data_get_pkt_stats_rsp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_data_get_pkt_stats_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_get_pkt_stats_rsp_msg_v01, pkt_stats) - QMI_IDL_OFFSET8(mcm_data_get_pkt_stats_rsp_msg_v01, pkt_stats_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_data_get_pkt_stats_rsp_msg_v01, pkt_stats),
  QMI_IDL_TYPE88(0, 1)
};

static const uint8_t mcm_data_reset_pkt_stats_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_reset_pkt_stats_req_msg_v01, call_id)
};

static const uint8_t mcm_data_reset_pkt_stats_rsp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_data_reset_pkt_stats_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_data_get_device_name_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_get_device_name_req_msg_v01, call_id)
};

static const uint8_t mcm_data_get_device_name_rsp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_data_get_device_name_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_get_device_name_rsp_msg_v01, device_name) - QMI_IDL_OFFSET8(mcm_data_get_device_name_rsp_msg_v01, device_name_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_data_get_device_name_rsp_msg_v01, device_name),
  MCM_DATA_MAX_DEVICE_NAME_LEN_V01,
  QMI_IDL_OFFSET8(mcm_data_get_device_name_rsp_msg_v01, device_name) - QMI_IDL_OFFSET8(mcm_data_get_device_name_rsp_msg_v01, device_name_len)
};

static const uint8_t mcm_data_get_device_addr_count_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_get_device_addr_count_req_msg_v01, call_id)
};

static const uint8_t mcm_data_get_device_addr_count_rsp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_data_get_device_addr_count_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_get_device_addr_count_rsp_msg_v01, addr_count) - QMI_IDL_OFFSET8(mcm_data_get_device_addr_count_rsp_msg_v01, addr_count_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_data_get_device_addr_count_rsp_msg_v01, addr_count)
};

static const uint8_t mcm_data_get_call_tech_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_get_call_tech_req_msg_v01, call_id)
};

static const uint8_t mcm_data_get_call_tech_rsp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_data_get_call_tech_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_get_call_tech_rsp_msg_v01, call_tech) - QMI_IDL_OFFSET8(mcm_data_get_call_tech_rsp_msg_v01, call_tech_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_get_call_tech_rsp_msg_v01, call_tech)
};

static const uint8_t mcm_data_get_call_status_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_get_call_status_req_msg_v01, call_id)
};

static const uint8_t mcm_data_get_call_status_rsp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_data_get_call_status_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_get_call_status_rsp_msg_v01, call_status) - QMI_IDL_OFFSET8(mcm_data_get_call_status_rsp_msg_v01, call_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_get_call_status_rsp_msg_v01, call_status)
};

/*
 * mcm_data_get_reg_status_req_msg is empty
 * static const uint8_t mcm_data_get_reg_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t mcm_data_get_reg_status_rsp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_data_get_reg_status_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_get_reg_status_rsp_msg_v01, reg_status) - QMI_IDL_OFFSET8(mcm_data_get_reg_status_rsp_msg_v01, reg_status_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_data_get_reg_status_rsp_msg_v01, reg_status),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t mcm_data_get_device_addr_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_get_device_addr_req_msg_v01, call_id)
};

static const uint8_t mcm_data_get_device_addr_rsp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_data_get_device_addr_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_get_device_addr_rsp_msg_v01, addr_info) - QMI_IDL_OFFSET8(mcm_data_get_device_addr_rsp_msg_v01, addr_info_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_data_get_device_addr_rsp_msg_v01, addr_info),
  MCM_DATA_MAX_ADDR_COUNT_V01,
  QMI_IDL_OFFSET8(mcm_data_get_device_addr_rsp_msg_v01, addr_info) - QMI_IDL_OFFSET8(mcm_data_get_device_addr_rsp_msg_v01, addr_info_len),
  QMI_IDL_TYPE88(0, 4)
};

static const uint8_t mcm_data_get_channel_rate_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_get_channel_rate_req_msg_v01, call_id)
};

static const uint8_t mcm_data_get_channel_rate_rsp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_data_get_channel_rate_rsp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_get_channel_rate_rsp_msg_v01, channel_rate) - QMI_IDL_OFFSET8(mcm_data_get_channel_rate_rsp_msg_v01, channel_rate_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_data_get_channel_rate_rsp_msg_v01, channel_rate),
  QMI_IDL_TYPE88(0, 2)
};

static const uint8_t mcm_data_event_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_event_register_req_msg_v01, register_net_up_event) - QMI_IDL_OFFSET8(mcm_data_event_register_req_msg_v01, register_net_up_event_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_data_event_register_req_msg_v01, register_net_up_event),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_event_register_req_msg_v01, register_net_down_event) - QMI_IDL_OFFSET8(mcm_data_event_register_req_msg_v01, register_net_down_event_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_data_event_register_req_msg_v01, register_net_down_event),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_event_register_req_msg_v01, register_net_new_addr_event) - QMI_IDL_OFFSET8(mcm_data_event_register_req_msg_v01, register_net_new_addr_event_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_data_event_register_req_msg_v01, register_net_new_addr_event),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_event_register_req_msg_v01, register_net_del_addr_event) - QMI_IDL_OFFSET8(mcm_data_event_register_req_msg_v01, register_net_del_addr_event_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_data_event_register_req_msg_v01, register_net_del_addr_event),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_event_register_req_msg_v01, register_reg_srvc_status_event) - QMI_IDL_OFFSET8(mcm_data_event_register_req_msg_v01, register_reg_srvc_status_event_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_data_event_register_req_msg_v01, register_reg_srvc_status_event),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_event_register_req_msg_v01, register_bearer_tech_status_event) - QMI_IDL_OFFSET8(mcm_data_event_register_req_msg_v01, register_bearer_tech_status_event_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_data_event_register_req_msg_v01, register_bearer_tech_status_event),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_event_register_req_msg_v01, register_dormancy_status_event) - QMI_IDL_OFFSET8(mcm_data_event_register_req_msg_v01, register_dormancy_status_event_valid)),
  0x16,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_data_event_register_req_msg_v01, register_dormancy_status_event)
};

static const uint8_t mcm_data_event_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_data_event_register_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_data_unsol_event_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_unsol_event_ind_msg_v01, event_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_unsol_event_ind_msg_v01, call_id) - QMI_IDL_OFFSET8(mcm_data_unsol_event_ind_msg_v01, call_id_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_unsol_event_ind_msg_v01, call_id),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_unsol_event_ind_msg_v01, call_status) - QMI_IDL_OFFSET8(mcm_data_unsol_event_ind_msg_v01, call_status_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_unsol_event_ind_msg_v01, call_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_unsol_event_ind_msg_v01, call_tech) - QMI_IDL_OFFSET8(mcm_data_unsol_event_ind_msg_v01, call_tech_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_unsol_event_ind_msg_v01, call_tech),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_unsol_event_ind_msg_v01, reg_status) - QMI_IDL_OFFSET8(mcm_data_unsol_event_ind_msg_v01, reg_status_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_data_unsol_event_ind_msg_v01, reg_status),
  QMI_IDL_TYPE88(0, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_unsol_event_ind_msg_v01, dorm_status) - QMI_IDL_OFFSET8(mcm_data_unsol_event_ind_msg_v01, dorm_status_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_data_unsol_event_ind_msg_v01, dorm_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_unsol_event_ind_msg_v01, addr_count) - QMI_IDL_OFFSET8(mcm_data_unsol_event_ind_msg_v01, addr_count_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_data_unsol_event_ind_msg_v01, addr_count),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_data_unsol_event_ind_msg_v01, addr_info) - QMI_IDL_OFFSET8(mcm_data_unsol_event_ind_msg_v01, addr_info_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_data_unsol_event_ind_msg_v01, addr_info),
  MCM_DATA_MAX_ADDR_COUNT_V01,
  QMI_IDL_OFFSET8(mcm_data_unsol_event_ind_msg_v01, addr_info) - QMI_IDL_OFFSET8(mcm_data_unsol_event_ind_msg_v01, addr_info_len),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(mcm_data_unsol_event_ind_msg_v01, vce_reason) - QMI_IDL_OFFSET16RELATIVE(mcm_data_unsol_event_ind_msg_v01, vce_reason_valid)),
  0x17,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(mcm_data_unsol_event_ind_msg_v01, vce_reason),
  QMI_IDL_TYPE88(0, 5)
};

/* Type Table */
static const qmi_idl_type_table_entry  mcm_data_type_table_v01[] = {
  {sizeof(mcm_data_reg_status_t_v01), mcm_data_reg_status_t_data_v01},
  {sizeof(mcm_data_pkt_stats_t_v01), mcm_data_pkt_stats_t_data_v01},
  {sizeof(mcm_data_channel_rate_t_v01), mcm_data_channel_rate_t_data_v01},
  {sizeof(mcm_data_addr_t_v01), mcm_data_addr_t_data_v01},
  {sizeof(mcm_data_addr_t_info_v01), mcm_data_addr_t_info_data_v01},
  {sizeof(mcm_data_verbose_call_end_reason_t_v01), mcm_data_verbose_call_end_reason_t_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry mcm_data_message_table_v01[] = {
  {sizeof(mcm_data_start_data_call_req_msg_v01), mcm_data_start_data_call_req_msg_data_v01},
  {sizeof(mcm_data_start_data_call_rsp_msg_v01), mcm_data_start_data_call_rsp_msg_data_v01},
  {sizeof(mcm_data_stop_data_call_req_msg_v01), mcm_data_stop_data_call_req_msg_data_v01},
  {sizeof(mcm_data_stop_data_call_rsp_msg_v01), mcm_data_stop_data_call_rsp_msg_data_v01},
  {sizeof(mcm_data_get_pkt_stats_req_msg_v01), mcm_data_get_pkt_stats_req_msg_data_v01},
  {sizeof(mcm_data_get_pkt_stats_rsp_msg_v01), mcm_data_get_pkt_stats_rsp_msg_data_v01},
  {sizeof(mcm_data_reset_pkt_stats_req_msg_v01), mcm_data_reset_pkt_stats_req_msg_data_v01},
  {sizeof(mcm_data_reset_pkt_stats_rsp_msg_v01), mcm_data_reset_pkt_stats_rsp_msg_data_v01},
  {sizeof(mcm_data_get_device_name_req_msg_v01), mcm_data_get_device_name_req_msg_data_v01},
  {sizeof(mcm_data_get_device_name_rsp_msg_v01), mcm_data_get_device_name_rsp_msg_data_v01},
  {sizeof(mcm_data_get_device_addr_count_req_msg_v01), mcm_data_get_device_addr_count_req_msg_data_v01},
  {sizeof(mcm_data_get_device_addr_count_rsp_msg_v01), mcm_data_get_device_addr_count_rsp_msg_data_v01},
  {sizeof(mcm_data_get_call_tech_req_msg_v01), mcm_data_get_call_tech_req_msg_data_v01},
  {sizeof(mcm_data_get_call_tech_rsp_msg_v01), mcm_data_get_call_tech_rsp_msg_data_v01},
  {sizeof(mcm_data_get_call_status_req_msg_v01), mcm_data_get_call_status_req_msg_data_v01},
  {sizeof(mcm_data_get_call_status_rsp_msg_v01), mcm_data_get_call_status_rsp_msg_data_v01},
  {sizeof(mcm_data_get_reg_status_req_msg_v01), 0},
  {sizeof(mcm_data_get_reg_status_rsp_msg_v01), mcm_data_get_reg_status_rsp_msg_data_v01},
  {sizeof(mcm_data_get_device_addr_req_msg_v01), mcm_data_get_device_addr_req_msg_data_v01},
  {sizeof(mcm_data_get_device_addr_rsp_msg_v01), mcm_data_get_device_addr_rsp_msg_data_v01},
  {sizeof(mcm_data_get_channel_rate_req_msg_v01), mcm_data_get_channel_rate_req_msg_data_v01},
  {sizeof(mcm_data_get_channel_rate_rsp_msg_v01), mcm_data_get_channel_rate_rsp_msg_data_v01},
  {sizeof(mcm_data_event_register_req_msg_v01), mcm_data_event_register_req_msg_data_v01},
  {sizeof(mcm_data_event_register_resp_msg_v01), mcm_data_event_register_resp_msg_data_v01},
  {sizeof(mcm_data_unsol_event_ind_msg_v01), mcm_data_unsol_event_ind_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object mcm_data_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *mcm_data_qmi_idl_type_table_object_referenced_tables_v01[] =
{&mcm_data_qmi_idl_type_table_object_v01, &mcm_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object mcm_data_qmi_idl_type_table_object_v01 = {
  sizeof(mcm_data_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(mcm_data_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  mcm_data_type_table_v01,
  mcm_data_message_table_v01,
  mcm_data_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry mcm_data_service_command_messages_v01[] = {
  {MCM_DATA_START_DATA_CALL_REQ_V01, QMI_IDL_TYPE16(0, 0), 429},
  {MCM_DATA_STOP_DATA_CALL_REQ_V01, QMI_IDL_TYPE16(0, 2), 7},
  {MCM_DATA_GET_PKT_STATS_REQ_V01, QMI_IDL_TYPE16(0, 4), 7},
  {MCM_DATA_RESET_PKT_STATS_REQ_V01, QMI_IDL_TYPE16(0, 6), 7},
  {MCM_DATA_GET_DEVICE_NAME_REQ_V01, QMI_IDL_TYPE16(0, 8), 7},
  {MCM_DATA_GET_DEVICE_ADDR_COUNT_REQ_V01, QMI_IDL_TYPE16(0, 10), 7},
  {MCM_DATA_GET_CALL_TECH_REQ_V01, QMI_IDL_TYPE16(0, 12), 7},
  {MCM_DATA_GET_CALL_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 14), 7},
  {MCM_DATA_GET_DEVICE_ADDR_REQ_V01, QMI_IDL_TYPE16(0, 18), 7},
  {MCM_DATA_GET_CHANNEL_RATE_REQ_MSG_V01, QMI_IDL_TYPE16(0, 20), 7},
  {MCM_DATA_EVENT_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 22), 28},
  {MCM_DATA_GET_REG_STATUS_REQ_MSG_V01, QMI_IDL_TYPE16(0, 16), 0}
};

static const qmi_idl_service_message_table_entry mcm_data_service_response_messages_v01[] = {
  {MCM_DATA_START_DATA_CALL_RSP_V01, QMI_IDL_TYPE16(0, 1), 36},
  {MCM_DATA_STOP_DATA_CALL_RSP_V01, QMI_IDL_TYPE16(0, 3), 11},
  {MCM_DATA_GET_PKT_STATS_RSP_V01, QMI_IDL_TYPE16(0, 5), 46},
  {MCM_DATA_RESET_PKT_STATS_RSP_V01, QMI_IDL_TYPE16(0, 7), 11},
  {MCM_DATA_GET_DEVICE_NAME_RSP_V01, QMI_IDL_TYPE16(0, 9), 28},
  {MCM_DATA_GET_DEVICE_ADDR_COUNT_RSP_V01, QMI_IDL_TYPE16(0, 11), 15},
  {MCM_DATA_GET_CALL_TECH_RSP_V01, QMI_IDL_TYPE16(0, 13), 18},
  {MCM_DATA_GET_CALL_STATUS_RSP_V01, QMI_IDL_TYPE16(0, 15), 18},
  {MCM_DATA_GET_DEVICE_ADDR_RSP_V01, QMI_IDL_TYPE16(0, 19), 5255},
  {MCM_DATA_GET_CHANNEL_RATE_RSP_MSG_V01, QMI_IDL_TYPE16(0, 21), 46},
  {MCM_DATA_EVENT_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 23), 11},
  {MCM_DATA_GET_REG_STATUS_RSP_MSG_V01, QMI_IDL_TYPE16(0, 17), 22}
};

static const qmi_idl_service_message_table_entry mcm_data_service_indication_messages_v01[] = {
  {MCM_DATA_UNSOL_EVENT_IND_V01, QMI_IDL_TYPE16(0, 24), 5305}
};

/*Service Object*/
struct qmi_idl_service_object mcm_data_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x404,
  5305,
  { sizeof(mcm_data_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(mcm_data_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(mcm_data_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { mcm_data_service_command_messages_v01, mcm_data_service_response_messages_v01, mcm_data_service_indication_messages_v01},
  &mcm_data_qmi_idl_type_table_object_v01,
  0x01,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type mcm_data_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( MCM_DATA_V01_IDL_MAJOR_VERS != idl_maj_version || MCM_DATA_V01_IDL_MINOR_VERS != idl_min_version
       || MCM_DATA_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&mcm_data_qmi_idl_service_object_v01;
}

