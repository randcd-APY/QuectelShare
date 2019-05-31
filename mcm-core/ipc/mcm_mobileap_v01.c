/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M C M _ M O B I L E A P _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the mcm_mobileap service Data structures.

  Copyright (c)2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.6
   It requires encode/decode library version 5 or later
   It was generated on: Mon Dec  9 2013 (Spin 0)
   From IDL File: mcm_mobileap_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "mcm_mobileap_v01.h"
#include "mcm_common_v01.h"
#include "mcm_service_object_v01.h"


/*Type Definitions*/
static const uint8_t mcm_mobileap_wwan_call_end_reason_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_wwan_call_end_reason_t_v01, wwan_call_end_reason_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_wwan_call_end_reason_t_v01, wwan_call_end_reason_code),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_mobileap_ip4_addr_subnet_mask_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_ip4_addr_subnet_mask_t_v01, addr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_ip4_addr_subnet_mask_t_v01, subnet_mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_mobileap_ip6_addr_prefix_len_t_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_ip6_addr_prefix_len_t_v01, addr),
  MCM_MOBILEAP_IPV6_ADDR_LEN_V01,

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_ip6_addr_prefix_len_t_v01, prefix_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_mobileap_tcp_udp_port_range_t_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_tcp_udp_port_range_t_v01, port),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_tcp_udp_port_range_t_v01, range),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_mobileap_ip4_tos_t_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_ip4_tos_t_v01, value),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_ip4_tos_t_v01, mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_mobileap_ip6_traffic_class_t_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_ip6_traffic_class_t_v01, value),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_ip6_traffic_class_t_v01, mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_mobileap_static_nat_entry_conf_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_static_nat_entry_conf_t_v01, port_fwding_private_ip),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_static_nat_entry_conf_t_v01, port_fwding_private_port),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_static_nat_entry_conf_t_v01, port_fwding_global_port),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_static_nat_entry_conf_t_v01, port_fwding_protocol),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_mobileap_wwan_statistics_t_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_wwan_statistics_t_v01, bytes_rx),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_wwan_statistics_t_v01, bytes_tx),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_wwan_statistics_t_v01, pkts_rx),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_wwan_statistics_t_v01, pkts_tx),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_wwan_statistics_t_v01, pkts_dropped_rx),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_wwan_statistics_t_v01, pkts_dropped_tx),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_mobileap_dhcpd_config_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_dhcpd_config_t_v01, intf),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_dhcpd_config_t_v01, start),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_dhcpd_config_t_v01, end),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_dhcpd_config_t_v01, leasetime),
  MCM_MOBILEAP_LEASE_TIME_LEN_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_mobileap_dualap_config_t_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_dualap_config_t_v01, enable),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_dualap_config_t_v01, a5_ip_address),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_dualap_config_t_v01, sub_net_mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_mobileap_sta_static_ip_config_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_sta_static_ip_config_t_v01, ip_addr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_sta_static_ip_config_t_v01, gw_ip),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_sta_static_ip_config_t_v01, netmask),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_sta_static_ip_config_t_v01, dns_addr),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t mcm_mobileap_sta_connection_config_t_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_sta_connection_config_t_v01, conn_type),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_sta_connection_config_t_v01, static_ip_config),
  QMI_IDL_TYPE88(0, 10),
  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
/*
 * mcm_mobileap_enable_req_msg is empty
 * static const uint8_t mcm_mobileap_enable_req_msg_data_v01[] = {
 * };
 */

static const uint8_t mcm_mobileap_enable_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_enable_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_enable_resp_msg_v01, mcm_mobileap_handle) - QMI_IDL_OFFSET8(mcm_mobileap_enable_resp_msg_v01, mcm_mobileap_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_enable_resp_msg_v01, mcm_mobileap_handle)
};

static const uint8_t mcm_mobileap_disable_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_disable_req_msg_v01, mcm_mobileap_handle)
};

static const uint8_t mcm_mobileap_disable_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_disable_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_mobileap_bring_up_wwan_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_bring_up_wwan_req_msg_v01, mcm_mobileap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_bring_up_wwan_req_msg_v01, ip_version) - QMI_IDL_OFFSET8(mcm_mobileap_bring_up_wwan_req_msg_v01, ip_version_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_bring_up_wwan_req_msg_v01, ip_version)
};

static const uint8_t mcm_mobileap_bring_up_wwan_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_bring_up_wwan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_mobileap_tear_down_wwan_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_tear_down_wwan_req_msg_v01, mcm_mobileap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_tear_down_wwan_req_msg_v01, ip_version) - QMI_IDL_OFFSET8(mcm_mobileap_tear_down_wwan_req_msg_v01, ip_version_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_tear_down_wwan_req_msg_v01, ip_version)
};

static const uint8_t mcm_mobileap_tear_down_wwan_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_tear_down_wwan_resp_msg_v01, resp) - QMI_IDL_OFFSET8(mcm_mobileap_tear_down_wwan_resp_msg_v01, resp_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_tear_down_wwan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_mobileap_add_static_nat_entry_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_add_static_nat_entry_req_msg_v01, mcm_mobileap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_add_static_nat_entry_req_msg_v01, nat_entry_config),
  QMI_IDL_TYPE88(0, 6)
};

static const uint8_t mcm_mobileap_add_static_nat_entry_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_add_static_nat_entry_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_mobileap_get_static_nat_entry_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_static_nat_entry_req_msg_v01, mcm_mobileap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_static_nat_entry_req_msg_v01, max_entries)
};

static const uint8_t mcm_mobileap_get_static_nat_entry_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_static_nat_entry_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_static_nat_entry_resp_msg_v01, snat_entries) - QMI_IDL_OFFSET8(mcm_mobileap_get_static_nat_entry_resp_msg_v01, snat_entries_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_static_nat_entry_resp_msg_v01, snat_entries),
  MCM_MOBILEAP_MAX_STATIC_NAT_ENTRIES_V01,
  QMI_IDL_OFFSET8(mcm_mobileap_get_static_nat_entry_resp_msg_v01, snat_entries) - QMI_IDL_OFFSET8(mcm_mobileap_get_static_nat_entry_resp_msg_v01, snat_entries_len),
  QMI_IDL_TYPE88(0, 6)
};

static const uint8_t mcm_mobileap_delete_static_nat_entry_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_delete_static_nat_entry_req_msg_v01, mcm_mobileap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_delete_static_nat_entry_req_msg_v01, snat_entry),
  QMI_IDL_TYPE88(0, 6)
};

static const uint8_t mcm_mobileap_delete_static_nat_entry_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_delete_static_nat_entry_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_mobileap_set_nat_timeout_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_nat_timeout_req_msg_v01, mcm_mobileap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_nat_timeout_req_msg_v01, timeout_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_nat_timeout_req_msg_v01, timeout_value)
};

static const uint8_t mcm_mobileap_set_nat_timeout_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_nat_timeout_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_mobileap_get_nat_timeout_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_nat_timeout_req_msg_v01, mcm_mobileap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_nat_timeout_req_msg_v01, timeout_type)
};

static const uint8_t mcm_mobileap_get_nat_timeout_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_nat_timeout_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_nat_timeout_resp_msg_v01, timeout_value) - QMI_IDL_OFFSET8(mcm_mobileap_get_nat_timeout_resp_msg_v01, timeout_value_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_nat_timeout_resp_msg_v01, timeout_value)
};

static const uint8_t mcm_mobileap_set_nat_type_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_nat_type_req_msg_v01, mcm_mobileap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_nat_type_req_msg_v01, nat_type)
};

static const uint8_t mcm_mobileap_set_nat_type_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_nat_type_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_mobileap_get_nat_type_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_nat_type_req_msg_v01, mcm_mobileap_handle)
};

static const uint8_t mcm_mobileap_get_nat_type_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_nat_type_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_nat_type_resp_msg_v01, nat_type) - QMI_IDL_OFFSET8(mcm_mobileap_get_nat_type_resp_msg_v01, nat_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_nat_type_resp_msg_v01, nat_type)
};

static const uint8_t mcm_mobileap_add_firewall_entry_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, mcm_mobileap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, ip_version),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, next_hdr_prot) - QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, next_hdr_prot_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, next_hdr_prot),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, tcp_udp_src) - QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, tcp_udp_src_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, tcp_udp_src),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, tcp_udp_dst) - QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, tcp_udp_dst_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, tcp_udp_dst),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, icmp_type) - QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, icmp_type_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, icmp_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, icmp_code) - QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, icmp_code_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, icmp_code),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, esp_spi) - QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, esp_spi_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, esp_spi),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, ip4_src_addr) - QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, ip4_src_addr_valid)),
  0x16,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, ip4_src_addr),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, ip4_dst_addr) - QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, ip4_dst_addr_valid)),
  0x17,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, ip4_dst_addr),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, ip4_tos) - QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, ip4_tos_valid)),
  0x18,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, ip4_tos),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, ip6_src_addr) - QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, ip6_src_addr_valid)),
  0x19,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, ip6_src_addr),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, ip6_dst_addr) - QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, ip6_dst_addr_valid)),
  0x1A,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, ip6_dst_addr),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, ip6_trf_cls) - QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, ip6_trf_cls_valid)),
  0x1B,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_req_msg_v01, ip6_trf_cls),
  QMI_IDL_TYPE88(0, 5)
};

static const uint8_t mcm_mobileap_add_firewall_entry_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_resp_msg_v01, firewall_handle) - QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_resp_msg_v01, firewall_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_add_firewall_entry_resp_msg_v01, firewall_handle)
};

static const uint8_t mcm_mobileap_get_firewall_entries_handle_list_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entries_handle_list_req_msg_v01, mcm_mobileap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entries_handle_list_req_msg_v01, ip_version)
};

static const uint8_t mcm_mobileap_get_firewall_entries_handle_list_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entries_handle_list_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entries_handle_list_resp_msg_v01, firewall_handle_list) - QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entries_handle_list_resp_msg_v01, firewall_handle_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entries_handle_list_resp_msg_v01, firewall_handle_list),
  MCM_MOBILEAP_MAX_FIREWALL_ENTRIES_V01,
  QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entries_handle_list_resp_msg_v01, firewall_handle_list) - QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entries_handle_list_resp_msg_v01, firewall_handle_list_len)
};

static const uint8_t mcm_mobileap_get_firewall_entry_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_req_msg_v01, mcm_mobileap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_req_msg_v01, firewall_handle)
};

static const uint8_t mcm_mobileap_get_firewall_entry_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, ip_version) - QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, ip_version_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, ip_version),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, next_hdr_prot) - QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, next_hdr_prot_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, next_hdr_prot),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, tcp_udp_src) - QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, tcp_udp_src_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, tcp_udp_src),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, tcp_udp_dst) - QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, tcp_udp_dst_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, tcp_udp_dst),
  QMI_IDL_TYPE88(0, 3),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, icmp_type) - QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, icmp_type_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, icmp_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, icmp_code) - QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, icmp_code_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, icmp_code),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, esp_spi) - QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, esp_spi_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, esp_spi),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, ip4_src_addr) - QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, ip4_src_addr_valid)),
  0x17,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, ip4_src_addr),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, ip4_dst_addr) - QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, ip4_dst_addr_valid)),
  0x18,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, ip4_dst_addr),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, ip4_tos) - QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, ip4_tos_valid)),
  0x19,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, ip4_tos),
  QMI_IDL_TYPE88(0, 4),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, ip6_src_addr) - QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, ip6_src_addr_valid)),
  0x1A,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, ip6_src_addr),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, ip6_dst_addr) - QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, ip6_dst_addr_valid)),
  0x1B,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, ip6_dst_addr),
  QMI_IDL_TYPE88(0, 2),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, ip6_trf_cls) - QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, ip6_trf_cls_valid)),
  0x1C,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_firewall_entry_resp_msg_v01, ip6_trf_cls),
  QMI_IDL_TYPE88(0, 5)
};

static const uint8_t mcm_mobileap_delete_firewall_entry_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_delete_firewall_entry_req_msg_v01, mcm_mobileap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_delete_firewall_entry_req_msg_v01, firewall_handle)
};

static const uint8_t mcm_mobileap_delete_firewall_entry_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_delete_firewall_entry_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_mobileap_set_firewall_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_firewall_config_req_msg_v01, mcm_mobileap_handle),

  0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_firewall_config_req_msg_v01, firewall_enabled),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_set_firewall_config_req_msg_v01, pkts_allowed) - QMI_IDL_OFFSET8(mcm_mobileap_set_firewall_config_req_msg_v01, pkts_allowed_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_firewall_config_req_msg_v01, pkts_allowed)
};

static const uint8_t mcm_mobileap_set_firewall_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_firewall_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_mobileap_add_dmz_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_add_dmz_req_msg_v01, mcm_mobileap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_add_dmz_req_msg_v01, dmz_ip_addr)
};

static const uint8_t mcm_mobileap_add_dmz_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_add_dmz_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_mobileap_get_dmz_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_dmz_req_msg_v01, mcm_mobileap_handle)
};

static const uint8_t mcm_mobileap_get_dmz_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_dmz_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_dmz_resp_msg_v01, dmz_ip_addr) - QMI_IDL_OFFSET8(mcm_mobileap_get_dmz_resp_msg_v01, dmz_ip_addr_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_dmz_resp_msg_v01, dmz_ip_addr)
};

static const uint8_t mcm_mobileap_delete_dmz_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_delete_dmz_req_msg_v01, mcm_mobileap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_delete_dmz_req_msg_v01, dmz_ip_addr)
};

static const uint8_t mcm_mobileap_delete_dmz_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_delete_dmz_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_mobileap_get_ipv4_wwan_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_ipv4_wwan_config_req_msg_v01, mcm_mobileap_handle)
};

static const uint8_t mcm_mobileap_get_ipv4_wwan_config_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_ipv4_wwan_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_ipv4_wwan_config_resp_msg_v01, v4_addr) - QMI_IDL_OFFSET8(mcm_mobileap_get_ipv4_wwan_config_resp_msg_v01, v4_addr_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_ipv4_wwan_config_resp_msg_v01, v4_addr),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_ipv4_wwan_config_resp_msg_v01, v4_prim_dns_addr) - QMI_IDL_OFFSET8(mcm_mobileap_get_ipv4_wwan_config_resp_msg_v01, v4_prim_dns_addr_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_ipv4_wwan_config_resp_msg_v01, v4_prim_dns_addr),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_ipv4_wwan_config_resp_msg_v01, v4_sec_dns_addr) - QMI_IDL_OFFSET8(mcm_mobileap_get_ipv4_wwan_config_resp_msg_v01, v4_sec_dns_addr_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_ipv4_wwan_config_resp_msg_v01, v4_sec_dns_addr)
};

static const uint8_t mcm_mobileap_get_wwan_stats_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_wwan_stats_req_msg_v01, mcm_mobileap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_wwan_stats_req_msg_v01, ip_family)
};

static const uint8_t mcm_mobileap_get_wwan_stats_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_wwan_stats_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_wwan_stats_resp_msg_v01, wwan_stats) - QMI_IDL_OFFSET8(mcm_mobileap_get_wwan_stats_resp_msg_v01, wwan_stats_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_wwan_stats_resp_msg_v01, wwan_stats),
  QMI_IDL_TYPE88(0, 7)
};

static const uint8_t mcm_mobileap_reset_wwan_stats_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_reset_wwan_stats_req_msg_v01, mcm_mobileap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_reset_wwan_stats_req_msg_v01, ip_family)
};

static const uint8_t mcm_mobileap_reset_wwan_stats_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_reset_wwan_stats_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_mobileap_set_dhcpd_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_dhcpd_config_req_msg_v01, mcm_mobileap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_dhcpd_config_req_msg_v01, dhcpd_config),
  QMI_IDL_TYPE88(0, 8)
};

static const uint8_t mcm_mobileap_set_dhcpd_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_dhcpd_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_mobileap_enable_wlan_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_enable_wlan_req_msg_v01, mcm_mobileap_handle)
};

static const uint8_t mcm_mobileap_enable_wlan_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_enable_wlan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_mobileap_disable_wlan_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_disable_wlan_req_msg_v01, mcm_mobileap_handle)
};

static const uint8_t mcm_mobileap_disable_wlan_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_disable_wlan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_mobileap_set_ipsec_vpn_pass_through_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_ipsec_vpn_pass_through_req_msg_v01, mcm_mobileap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_ipsec_vpn_pass_through_req_msg_v01, vpn_pass_through_value)
};

static const uint8_t mcm_mobileap_set_ipsec_vpn_pass_through_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_ipsec_vpn_pass_through_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_mobileap_get_ipsec_vpn_pass_through_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_ipsec_vpn_pass_through_req_msg_v01, mcm_mobileap_handle)
};

static const uint8_t mcm_mobileap_get_ipsec_vpn_pass_through_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_ipsec_vpn_pass_through_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_ipsec_vpn_pass_through_resp_msg_v01, vpn_pass_through_value) - QMI_IDL_OFFSET8(mcm_mobileap_get_ipsec_vpn_pass_through_resp_msg_v01, vpn_pass_through_value_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_ipsec_vpn_pass_through_resp_msg_v01, vpn_pass_through_value)
};

static const uint8_t mcm_mobileap_set_pptp_vpn_pass_through_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_pptp_vpn_pass_through_req_msg_v01, mcm_mobileap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_pptp_vpn_pass_through_req_msg_v01, vpn_pass_through_value)
};

static const uint8_t mcm_mobileap_set_pptp_vpn_pass_through_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_pptp_vpn_pass_through_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_mobileap_get_pptp_vpn_pass_through_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_pptp_vpn_pass_through_req_msg_v01, mcm_mobileap_handle)
};

static const uint8_t mcm_mobileap_get_pptp_vpn_pass_through_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_pptp_vpn_pass_through_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_pptp_vpn_pass_through_resp_msg_v01, vpn_pass_through_value) - QMI_IDL_OFFSET8(mcm_mobileap_get_pptp_vpn_pass_through_resp_msg_v01, vpn_pass_through_value_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_pptp_vpn_pass_through_resp_msg_v01, vpn_pass_through_value)
};

static const uint8_t mcm_mobileap_set_l2tp_vpn_pass_through_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_l2tp_vpn_pass_through_req_msg_v01, mcm_mobileap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_l2tp_vpn_pass_through_req_msg_v01, vpn_pass_through_value)
};

static const uint8_t mcm_mobileap_set_l2tp_vpn_pass_through_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_l2tp_vpn_pass_through_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_mobileap_get_l2tp_vpn_pass_through_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_l2tp_vpn_pass_through_req_msg_v01, mcm_mobileap_handle)
};

static const uint8_t mcm_mobileap_get_l2tp_vpn_pass_through_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_l2tp_vpn_pass_through_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_l2tp_vpn_pass_through_resp_msg_v01, vpn_pass_through_value) - QMI_IDL_OFFSET8(mcm_mobileap_get_l2tp_vpn_pass_through_resp_msg_v01, vpn_pass_through_value_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_l2tp_vpn_pass_through_resp_msg_v01, vpn_pass_through_value)
};

static const uint8_t mcm_mobileap_set_auto_connect_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_auto_connect_req_msg_v01, mcm_mobileap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_auto_connect_req_msg_v01, enable)
};

static const uint8_t mcm_mobileap_set_auto_connect_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_auto_connect_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_mobileap_get_auto_connect_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_auto_connect_req_msg_v01, mcm_mobileap_handle)
};

static const uint8_t mcm_mobileap_get_auto_connect_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_auto_connect_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_auto_connect_resp_msg_v01, auto_conn_flag) - QMI_IDL_OFFSET8(mcm_mobileap_get_auto_connect_resp_msg_v01, auto_conn_flag_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_auto_connect_resp_msg_v01, auto_conn_flag)
};

static const uint8_t mcm_mobileap_set_roaming_pref_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_roaming_pref_req_msg_v01, mcm_mobileap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_roaming_pref_req_msg_v01, allow_wwan_calls_while_roaming)
};

static const uint8_t mcm_mobileap_set_roaming_pref_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_roaming_pref_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_mobileap_get_roaming_pref_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_roaming_pref_req_msg_v01, mcm_mobileap_handle)
};

static const uint8_t mcm_mobileap_get_roaming_pref_resp_msg_data_v01[] = {
  0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_roaming_pref_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_get_roaming_pref_resp_msg_v01, allow_wwan_calls_while_roaming) - QMI_IDL_OFFSET8(mcm_mobileap_get_roaming_pref_resp_msg_v01, allow_wwan_calls_while_roaming_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_get_roaming_pref_resp_msg_v01, allow_wwan_calls_while_roaming)
};

static const uint8_t mcm_mobileap_set_dualap_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_dualap_config_req_msg_v01, mcm_mobileap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_dualap_config_req_msg_v01, dualap_config),
  QMI_IDL_TYPE88(0, 9)
};

static const uint8_t mcm_mobileap_set_dualap_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_set_dualap_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_mobileap_station_mode_enable_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_station_mode_enable_req_msg_v01, mcm_mobileap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_station_mode_enable_req_msg_v01, cfg),
  QMI_IDL_TYPE88(0, 11)
};

static const uint8_t mcm_mobileap_station_mode_enable_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_station_mode_enable_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_mobileap_station_mode_disable_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_station_mode_disable_req_msg_v01, mcm_mobileap_handle)
};

static const uint8_t mcm_mobileap_station_mode_disable_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_station_mode_disable_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_mobileap_event_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_enabled) - QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_enabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_lan_connecting) - QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_lan_connecting_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_lan_connecting),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_lan_connecting_fail) - QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_lan_connecting_fail_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_lan_connecting_fail),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_lan_ipv6_connecting_fail) - QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_lan_ipv6_connecting_fail_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_lan_ipv6_connecting_fail),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_lan_connected) - QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_lan_connected_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_lan_connected),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_sta_connected) - QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_sta_connected_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_sta_connected),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_lan_ipv6_connected) - QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_lan_ipv6_connected_valid)),
  0x16,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_lan_ipv6_connected),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_wan_connecting) - QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_wan_connecting_valid)),
  0x17,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_wan_connecting),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_wan_connecting_fail) - QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_wan_connecting_fail_valid)),
  0x18,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_wan_connecting_fail),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_wan_ipv6_connecting_fail) - QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_wan_ipv6_connecting_fail_valid)),
  0x19,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_wan_ipv6_connecting_fail),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_wan_connected) - QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_wan_connected_valid)),
  0x1A,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_wan_connected),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_wan_ipv6_connected) - QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_wan_ipv6_connected_valid)),
  0x1B,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_wan_ipv6_connected),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_wan_disconnected) - QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_wan_disconnected_valid)),
  0x1C,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_wan_disconnected),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_wan_ipv6_disconnected) - QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_wan_ipv6_disconnected_valid)),
  0x1D,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_wan_ipv6_disconnected),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_lan_disconnected) - QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_lan_disconnected_valid)),
  0x1E,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_lan_disconnected),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_lan_ipv6_disconnected) - QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_lan_ipv6_disconnected_valid)),
  0x1F,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_lan_ipv6_disconnected),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_disabled) - QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_disabled_valid)),
  0x20,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_event_register_req_msg_v01, register_event_disabled)
};

static const uint8_t mcm_mobileap_event_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(mcm_mobileap_event_register_resp_msg_v01, response),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t mcm_mobileap_unsol_event_ind_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(mcm_mobileap_unsol_event_ind_msg_v01, event_id)
};

/* Type Table */
static const qmi_idl_type_table_entry  mcm_mobileap_type_table_v01[] = {
  {sizeof(mcm_mobileap_wwan_call_end_reason_t_v01), mcm_mobileap_wwan_call_end_reason_t_data_v01},
  {sizeof(mcm_mobileap_ip4_addr_subnet_mask_t_v01), mcm_mobileap_ip4_addr_subnet_mask_t_data_v01},
  {sizeof(mcm_mobileap_ip6_addr_prefix_len_t_v01), mcm_mobileap_ip6_addr_prefix_len_t_data_v01},
  {sizeof(mcm_mobileap_tcp_udp_port_range_t_v01), mcm_mobileap_tcp_udp_port_range_t_data_v01},
  {sizeof(mcm_mobileap_ip4_tos_t_v01), mcm_mobileap_ip4_tos_t_data_v01},
  {sizeof(mcm_mobileap_ip6_traffic_class_t_v01), mcm_mobileap_ip6_traffic_class_t_data_v01},
  {sizeof(mcm_mobileap_static_nat_entry_conf_t_v01), mcm_mobileap_static_nat_entry_conf_t_data_v01},
  {sizeof(mcm_mobileap_wwan_statistics_t_v01), mcm_mobileap_wwan_statistics_t_data_v01},
  {sizeof(mcm_mobileap_dhcpd_config_t_v01), mcm_mobileap_dhcpd_config_t_data_v01},
  {sizeof(mcm_mobileap_dualap_config_t_v01), mcm_mobileap_dualap_config_t_data_v01},
  {sizeof(mcm_mobileap_sta_static_ip_config_t_v01), mcm_mobileap_sta_static_ip_config_t_data_v01},
  {sizeof(mcm_mobileap_sta_connection_config_t_v01), mcm_mobileap_sta_connection_config_t_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry mcm_mobileap_message_table_v01[] = {
  {sizeof(mcm_mobileap_enable_req_msg_v01), 0},
  {sizeof(mcm_mobileap_enable_resp_msg_v01), mcm_mobileap_enable_resp_msg_data_v01},
  {sizeof(mcm_mobileap_disable_req_msg_v01), mcm_mobileap_disable_req_msg_data_v01},
  {sizeof(mcm_mobileap_disable_resp_msg_v01), mcm_mobileap_disable_resp_msg_data_v01},
  {sizeof(mcm_mobileap_bring_up_wwan_req_msg_v01), mcm_mobileap_bring_up_wwan_req_msg_data_v01},
  {sizeof(mcm_mobileap_bring_up_wwan_resp_msg_v01), mcm_mobileap_bring_up_wwan_resp_msg_data_v01},
  {sizeof(mcm_mobileap_tear_down_wwan_req_msg_v01), mcm_mobileap_tear_down_wwan_req_msg_data_v01},
  {sizeof(mcm_mobileap_tear_down_wwan_resp_msg_v01), mcm_mobileap_tear_down_wwan_resp_msg_data_v01},
  {sizeof(mcm_mobileap_add_static_nat_entry_req_msg_v01), mcm_mobileap_add_static_nat_entry_req_msg_data_v01},
  {sizeof(mcm_mobileap_add_static_nat_entry_resp_msg_v01), mcm_mobileap_add_static_nat_entry_resp_msg_data_v01},
  {sizeof(mcm_mobileap_get_static_nat_entry_req_msg_v01), mcm_mobileap_get_static_nat_entry_req_msg_data_v01},
  {sizeof(mcm_mobileap_get_static_nat_entry_resp_msg_v01), mcm_mobileap_get_static_nat_entry_resp_msg_data_v01},
  {sizeof(mcm_mobileap_delete_static_nat_entry_req_msg_v01), mcm_mobileap_delete_static_nat_entry_req_msg_data_v01},
  {sizeof(mcm_mobileap_delete_static_nat_entry_resp_msg_v01), mcm_mobileap_delete_static_nat_entry_resp_msg_data_v01},
  {sizeof(mcm_mobileap_set_nat_timeout_req_msg_v01), mcm_mobileap_set_nat_timeout_req_msg_data_v01},
  {sizeof(mcm_mobileap_set_nat_timeout_resp_msg_v01), mcm_mobileap_set_nat_timeout_resp_msg_data_v01},
  {sizeof(mcm_mobileap_get_nat_timeout_req_msg_v01), mcm_mobileap_get_nat_timeout_req_msg_data_v01},
  {sizeof(mcm_mobileap_get_nat_timeout_resp_msg_v01), mcm_mobileap_get_nat_timeout_resp_msg_data_v01},
  {sizeof(mcm_mobileap_set_nat_type_req_msg_v01), mcm_mobileap_set_nat_type_req_msg_data_v01},
  {sizeof(mcm_mobileap_set_nat_type_resp_msg_v01), mcm_mobileap_set_nat_type_resp_msg_data_v01},
  {sizeof(mcm_mobileap_get_nat_type_req_msg_v01), mcm_mobileap_get_nat_type_req_msg_data_v01},
  {sizeof(mcm_mobileap_get_nat_type_resp_msg_v01), mcm_mobileap_get_nat_type_resp_msg_data_v01},
  {sizeof(mcm_mobileap_add_firewall_entry_req_msg_v01), mcm_mobileap_add_firewall_entry_req_msg_data_v01},
  {sizeof(mcm_mobileap_add_firewall_entry_resp_msg_v01), mcm_mobileap_add_firewall_entry_resp_msg_data_v01},
  {sizeof(mcm_mobileap_get_firewall_entries_handle_list_req_msg_v01), mcm_mobileap_get_firewall_entries_handle_list_req_msg_data_v01},
  {sizeof(mcm_mobileap_get_firewall_entries_handle_list_resp_msg_v01), mcm_mobileap_get_firewall_entries_handle_list_resp_msg_data_v01},
  {sizeof(mcm_mobileap_get_firewall_entry_req_msg_v01), mcm_mobileap_get_firewall_entry_req_msg_data_v01},
  {sizeof(mcm_mobileap_get_firewall_entry_resp_msg_v01), mcm_mobileap_get_firewall_entry_resp_msg_data_v01},
  {sizeof(mcm_mobileap_delete_firewall_entry_req_msg_v01), mcm_mobileap_delete_firewall_entry_req_msg_data_v01},
  {sizeof(mcm_mobileap_delete_firewall_entry_resp_msg_v01), mcm_mobileap_delete_firewall_entry_resp_msg_data_v01},
  {sizeof(mcm_mobileap_set_firewall_config_req_msg_v01), mcm_mobileap_set_firewall_config_req_msg_data_v01},
  {sizeof(mcm_mobileap_set_firewall_config_resp_msg_v01), mcm_mobileap_set_firewall_config_resp_msg_data_v01},
  {sizeof(mcm_mobileap_add_dmz_req_msg_v01), mcm_mobileap_add_dmz_req_msg_data_v01},
  {sizeof(mcm_mobileap_add_dmz_resp_msg_v01), mcm_mobileap_add_dmz_resp_msg_data_v01},
  {sizeof(mcm_mobileap_get_dmz_req_msg_v01), mcm_mobileap_get_dmz_req_msg_data_v01},
  {sizeof(mcm_mobileap_get_dmz_resp_msg_v01), mcm_mobileap_get_dmz_resp_msg_data_v01},
  {sizeof(mcm_mobileap_delete_dmz_req_msg_v01), mcm_mobileap_delete_dmz_req_msg_data_v01},
  {sizeof(mcm_mobileap_delete_dmz_resp_msg_v01), mcm_mobileap_delete_dmz_resp_msg_data_v01},
  {sizeof(mcm_mobileap_get_ipv4_wwan_config_req_msg_v01), mcm_mobileap_get_ipv4_wwan_config_req_msg_data_v01},
  {sizeof(mcm_mobileap_get_ipv4_wwan_config_resp_msg_v01), mcm_mobileap_get_ipv4_wwan_config_resp_msg_data_v01},
  {sizeof(mcm_mobileap_get_wwan_stats_req_msg_v01), mcm_mobileap_get_wwan_stats_req_msg_data_v01},
  {sizeof(mcm_mobileap_get_wwan_stats_resp_msg_v01), mcm_mobileap_get_wwan_stats_resp_msg_data_v01},
  {sizeof(mcm_mobileap_reset_wwan_stats_req_msg_v01), mcm_mobileap_reset_wwan_stats_req_msg_data_v01},
  {sizeof(mcm_mobileap_reset_wwan_stats_resp_msg_v01), mcm_mobileap_reset_wwan_stats_resp_msg_data_v01},
  {sizeof(mcm_mobileap_set_dhcpd_config_req_msg_v01), mcm_mobileap_set_dhcpd_config_req_msg_data_v01},
  {sizeof(mcm_mobileap_set_dhcpd_config_resp_msg_v01), mcm_mobileap_set_dhcpd_config_resp_msg_data_v01},
  {sizeof(mcm_mobileap_enable_wlan_req_msg_v01), mcm_mobileap_enable_wlan_req_msg_data_v01},
  {sizeof(mcm_mobileap_enable_wlan_resp_msg_v01), mcm_mobileap_enable_wlan_resp_msg_data_v01},
  {sizeof(mcm_mobileap_disable_wlan_req_msg_v01), mcm_mobileap_disable_wlan_req_msg_data_v01},
  {sizeof(mcm_mobileap_disable_wlan_resp_msg_v01), mcm_mobileap_disable_wlan_resp_msg_data_v01},
  {sizeof(mcm_mobileap_set_ipsec_vpn_pass_through_req_msg_v01), mcm_mobileap_set_ipsec_vpn_pass_through_req_msg_data_v01},
  {sizeof(mcm_mobileap_set_ipsec_vpn_pass_through_resp_msg_v01), mcm_mobileap_set_ipsec_vpn_pass_through_resp_msg_data_v01},
  {sizeof(mcm_mobileap_get_ipsec_vpn_pass_through_req_msg_v01), mcm_mobileap_get_ipsec_vpn_pass_through_req_msg_data_v01},
  {sizeof(mcm_mobileap_get_ipsec_vpn_pass_through_resp_msg_v01), mcm_mobileap_get_ipsec_vpn_pass_through_resp_msg_data_v01},
  {sizeof(mcm_mobileap_set_pptp_vpn_pass_through_req_msg_v01), mcm_mobileap_set_pptp_vpn_pass_through_req_msg_data_v01},
  {sizeof(mcm_mobileap_set_pptp_vpn_pass_through_resp_msg_v01), mcm_mobileap_set_pptp_vpn_pass_through_resp_msg_data_v01},
  {sizeof(mcm_mobileap_get_pptp_vpn_pass_through_req_msg_v01), mcm_mobileap_get_pptp_vpn_pass_through_req_msg_data_v01},
  {sizeof(mcm_mobileap_get_pptp_vpn_pass_through_resp_msg_v01), mcm_mobileap_get_pptp_vpn_pass_through_resp_msg_data_v01},
  {sizeof(mcm_mobileap_set_l2tp_vpn_pass_through_req_msg_v01), mcm_mobileap_set_l2tp_vpn_pass_through_req_msg_data_v01},
  {sizeof(mcm_mobileap_set_l2tp_vpn_pass_through_resp_msg_v01), mcm_mobileap_set_l2tp_vpn_pass_through_resp_msg_data_v01},
  {sizeof(mcm_mobileap_get_l2tp_vpn_pass_through_req_msg_v01), mcm_mobileap_get_l2tp_vpn_pass_through_req_msg_data_v01},
  {sizeof(mcm_mobileap_get_l2tp_vpn_pass_through_resp_msg_v01), mcm_mobileap_get_l2tp_vpn_pass_through_resp_msg_data_v01},
  {sizeof(mcm_mobileap_set_auto_connect_req_msg_v01), mcm_mobileap_set_auto_connect_req_msg_data_v01},
  {sizeof(mcm_mobileap_set_auto_connect_resp_msg_v01), mcm_mobileap_set_auto_connect_resp_msg_data_v01},
  {sizeof(mcm_mobileap_get_auto_connect_req_msg_v01), mcm_mobileap_get_auto_connect_req_msg_data_v01},
  {sizeof(mcm_mobileap_get_auto_connect_resp_msg_v01), mcm_mobileap_get_auto_connect_resp_msg_data_v01},
  {sizeof(mcm_mobileap_set_roaming_pref_req_msg_v01), mcm_mobileap_set_roaming_pref_req_msg_data_v01},
  {sizeof(mcm_mobileap_set_roaming_pref_resp_msg_v01), mcm_mobileap_set_roaming_pref_resp_msg_data_v01},
  {sizeof(mcm_mobileap_get_roaming_pref_req_msg_v01), mcm_mobileap_get_roaming_pref_req_msg_data_v01},
  {sizeof(mcm_mobileap_get_roaming_pref_resp_msg_v01), mcm_mobileap_get_roaming_pref_resp_msg_data_v01},
  {sizeof(mcm_mobileap_set_dualap_config_req_msg_v01), mcm_mobileap_set_dualap_config_req_msg_data_v01},
  {sizeof(mcm_mobileap_set_dualap_config_resp_msg_v01), mcm_mobileap_set_dualap_config_resp_msg_data_v01},
  {sizeof(mcm_mobileap_station_mode_enable_req_msg_v01), mcm_mobileap_station_mode_enable_req_msg_data_v01},
  {sizeof(mcm_mobileap_station_mode_enable_resp_msg_v01), mcm_mobileap_station_mode_enable_resp_msg_data_v01},
  {sizeof(mcm_mobileap_station_mode_disable_req_msg_v01), mcm_mobileap_station_mode_disable_req_msg_data_v01},
  {sizeof(mcm_mobileap_station_mode_disable_resp_msg_v01), mcm_mobileap_station_mode_disable_resp_msg_data_v01},
  {sizeof(mcm_mobileap_event_register_req_msg_v01), mcm_mobileap_event_register_req_msg_data_v01},
  {sizeof(mcm_mobileap_event_register_resp_msg_v01), mcm_mobileap_event_register_resp_msg_data_v01},
  {sizeof(mcm_mobileap_unsol_event_ind_msg_v01), mcm_mobileap_unsol_event_ind_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object mcm_mobileap_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *mcm_mobileap_qmi_idl_type_table_object_referenced_tables_v01[] =
{&mcm_mobileap_qmi_idl_type_table_object_v01, &mcm_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object mcm_mobileap_qmi_idl_type_table_object_v01 = {
  sizeof(mcm_mobileap_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(mcm_mobileap_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  mcm_mobileap_type_table_v01,
  mcm_mobileap_message_table_v01,
  mcm_mobileap_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry mcm_mobileap_service_command_messages_v01[] = {
  {MCM_MOBILEAP_ENABLE_REQ_V01, QMI_IDL_TYPE16(0, 0), 0},
  {MCM_MOBILEAP_DISABLE_REQ_V01, QMI_IDL_TYPE16(0, 2), 7},
  {MCM_MOBILEAP_BRING_UP_WWAN_REQ_V01, QMI_IDL_TYPE16(0, 4), 14},
  {MCM_MOBILEAP_TEAR_DOWN_WWAN_REQ_V01, QMI_IDL_TYPE16(0, 6), 14},
  {MCM_MOBILEAP_ADD_STATIC_NAT_ENTRY_REQ_V01, QMI_IDL_TYPE16(0, 8), 19},
  {MCM_MOBILEAP_GET_STATIC_NAT_ENTRY_REQ_V01, QMI_IDL_TYPE16(0, 10), 14},
  {MCM_MOBILEAP_DELETE_STATIC_NAT_ENTRY_REQ_V01, QMI_IDL_TYPE16(0, 12), 19},
  {MCM_MOBILEAP_SET_NAT_TIMEOUT_REQ_V01, QMI_IDL_TYPE16(0, 14), 21},
  {MCM_MOBILEAP_GET_NAT_TIMEOUT_REQ_V01, QMI_IDL_TYPE16(0, 16), 14},
  {MCM_MOBILEAP_SET_NAT_TYPE_REQ_V01, QMI_IDL_TYPE16(0, 18), 14},
  {MCM_MOBILEAP_GET_NAT_TYPE_REQ_V01, QMI_IDL_TYPE16(0, 20), 7},
  {MCM_MOBILEAP_ADD_FIREWALL_ENTRY_REQ_V01, QMI_IDL_TYPE16(0, 22), 119},
  {MCM_MOBILEAP_GET_FIREWALL_ENTRIES_HANDLE_LIST_REQ_V01, QMI_IDL_TYPE16(0, 24), 14},
  {MCM_MOBILEAP_GET_FIREWALL_ENTRY_REQ_V01, QMI_IDL_TYPE16(0, 26), 14},
  {MCM_MOBILEAP_DELETE_FIREWALL_ENTRY_REQ_V01, QMI_IDL_TYPE16(0, 28), 14},
  {MCM_MOBILEAP_SET_FIREWALL_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 30), 15},
  {MCM_MOBILEAP_SET_DMZ_REQ_V01, QMI_IDL_TYPE16(0, 32), 14},
  {MCM_MOBILEAP_DELETE_DMZ_REQ_V01, QMI_IDL_TYPE16(0, 36), 14},
  {MCM_MOBILEAP_GET_DMZ_REQ_V01, QMI_IDL_TYPE16(0, 34), 7},
  {MCM_MOBILEAP_GET_IPV4_WWAN_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 38), 7},
  {MCM_MOBILEAP_GET_WWAN_STATS_REQ_V01, QMI_IDL_TYPE16(0, 40), 14},
  {MCM_MOBILEAP_RESET_WWAN_STATS_REQ_V01, QMI_IDL_TYPE16(0, 42), 14},
  {MCM_MOBILEAP_SET_DHCPD_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 44), 122},
  {MCM_MOBILEAP_ENABLE_WLAN_REQ_V01, QMI_IDL_TYPE16(0, 46), 7},
  {MCM_MOBILEAP_DISABLE_WLAN_REQ_V01, QMI_IDL_TYPE16(0, 48), 7},
  {MCM_MOBILEAP_GET_IPSEC_VPN_PASS_THROUGH_REQ_V01, QMI_IDL_TYPE16(0, 52), 7},
  {MCM_MOBILEAP_SET_IPSEC_VPN_PASS_THROUGH_REQ_V01, QMI_IDL_TYPE16(0, 50), 11},
  {MCM_MOBILEAP_GET_PPTP_VPN_PASS_THROUGH_REQ_V01, QMI_IDL_TYPE16(0, 56), 7},
  {MCM_MOBILEAP_SET_PPTP_VPN_PASS_THROUGH_REQ_V01, QMI_IDL_TYPE16(0, 54), 11},
  {MCM_MOBILEAP_GET_L2TP_VPN_PASS_THROUGH_REQ_V01, QMI_IDL_TYPE16(0, 60), 7},
  {MCM_MOBILEAP_SET_L2TP_VPN_PASS_THROUGH_REQ_V01, QMI_IDL_TYPE16(0, 58), 11},
  {MCM_MOBILEAP_SET_AUTO_CONNECT_REQ_V01, QMI_IDL_TYPE16(0, 62), 11},
  {MCM_MOBILEAP_GET_AUTO_CONNECT_REQ_V01, QMI_IDL_TYPE16(0, 64), 7},
  {MCM_MOBILEAP_SET_ROAMING_PREF_REQ_V01, QMI_IDL_TYPE16(0, 66), 11},
  {MCM_MOBILEAP_GET_ROAMING_PREF_REQ_V01, QMI_IDL_TYPE16(0, 68), 7},
  {MCM_MOBILEAP_SET_DUALAP_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 70), 19},
  {MCM_MOBILEAP_STATION_MODE_ENABLE_REQ_V01, QMI_IDL_TYPE16(0, 72), 30},
  {MCM_MOBILEAP_STATION_MODE_DISABLE_REQ_V01, QMI_IDL_TYPE16(0, 74), 7},
  {MCM_MOBILEAP_EVENT_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 76), 68}
};

static const qmi_idl_service_message_table_entry mcm_mobileap_service_response_messages_v01[] = {
  {MCM_MOBILEAP_ENABLE_RESP_V01, QMI_IDL_TYPE16(0, 1), 18},
  {MCM_MOBILEAP_DISABLE_RESP_V01, QMI_IDL_TYPE16(0, 3), 11},
  {MCM_MOBILEAP_BRING_UP_WWAN_RESP_V01, QMI_IDL_TYPE16(0, 5), 11},
  {MCM_MOBILEAP_TEAR_DOWN_WWAN_RESP_V01, QMI_IDL_TYPE16(0, 7), 11},
  {MCM_MOBILEAP_ADD_STATIC_NAT_ENTRY_RESP_V01, QMI_IDL_TYPE16(0, 9), 11},
  {MCM_MOBILEAP_GET_STATIC_NAT_ENTRY_RESP_V01, QMI_IDL_TYPE16(0, 11), 465},
  {MCM_MOBILEPA_DELETE_STATIC_NAT_ENTRY_RESP_V01, QMI_IDL_TYPE16(0, 13), 11},
  {MCM_MOBILEAP_SET_NAT_TIMEOUT_RESP_V01, QMI_IDL_TYPE16(0, 15), 11},
  {MCM_MOBILEAP_GET_NAT_TIMEOUT_RESP_V01, QMI_IDL_TYPE16(0, 17), 18},
  {MCM_MOBILEAP_SET_NAT_TYPE_RESP_V01, QMI_IDL_TYPE16(0, 19), 11},
  {MCM_MOBILEAP_GET_NAT_TYPE_RESP_V01, QMI_IDL_TYPE16(0, 21), 18},
  {MCM_MOBILEAP_ADD_FIREWALL_ENTRY_RESP_V01, QMI_IDL_TYPE16(0, 23), 18},
  {MCM_MOBILEAP_GET_FIREWALL_ENTRIES_HANDLE_LIST_RESP_V01, QMI_IDL_TYPE16(0, 25), 215},
  {MCM_MOBILEAP_GET_FIREWALL_ENTRY_RESP_V01, QMI_IDL_TYPE16(0, 27), 123},
  {MCM_MOBILEAP_DELETE_FIREWALL_ENTRY_RESP_V01, QMI_IDL_TYPE16(0, 29), 11},
  {MCM_MOBILEAP_SET_FIREWALL_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 31), 11},
  {MCM_MOBILEAP_SET_DMZ_RESP_V01, QMI_IDL_TYPE16(0, 33), 11},
  {MCM_MOBILEAP_DELETE_DMZ_RESP_V01, QMI_IDL_TYPE16(0, 37), 11},
  {MCM_MOBILEAP_GET_DMZ_RESP_V01, QMI_IDL_TYPE16(0, 35), 18},
  {MCM_MOBILEAP_GET_IPV4_WWAN_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 39), 32},
  {MCM_MOBILEAP_GET_WWAN_STATS_RESP_V01, QMI_IDL_TYPE16(0, 41), 46},
  {MCM_MOBILEAP_RESET_WWAN_STATS_RESP_V01, QMI_IDL_TYPE16(0, 43), 11},
  {MCM_MOBILEAP_SET_DHCPD_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 45), 11},
  {MCM_MOBILEAP_ENABLE_WLAN_RESP_V01, QMI_IDL_TYPE16(0, 47), 11},
  {MCM_MOBILEAP_DISABLE_WLAN_RESP_V01, QMI_IDL_TYPE16(0, 49), 11},
  {MCM_MOBILEAP_GET_IPSEC_VPN_PASS_THROUGH_RESP_V01, QMI_IDL_TYPE16(0, 53), 15},
  {MCM_MOBILEAP_SET_IPSEC_VPN_PASS_THROUGH_RESP_V01, QMI_IDL_TYPE16(0, 51), 11},
  {MCM_MOBILEAP_GET_PPTP_VPN_PASS_THROUGH_RESP_V01, QMI_IDL_TYPE16(0, 57), 15},
  {MCM_MOBILEAP_SET_PPTP_VPN_PASS_THROUGH_RESP_V01, QMI_IDL_TYPE16(0, 55), 11},
  {MCM_MOBILEAP_GET_L2TP_VPN_PASS_THROUGH_RESP_V01, QMI_IDL_TYPE16(0, 61), 15},
  {MCM_MOBILEAP_SET_L2TP_VPN_PASS_THROUGH_RESP_V01, QMI_IDL_TYPE16(0, 59), 11},
  {MCM_MOBILEAP_SET_AUTO_CONNECT_RESP_V01, QMI_IDL_TYPE16(0, 63), 11},
  {MCM_MOBILEAP_GET_AUTO_CONNECT_RESP_V01, QMI_IDL_TYPE16(0, 65), 15},
  {MCM_MOBILEAP_SET_ROAMING_PREF_RESP_V01, QMI_IDL_TYPE16(0, 67), 11},
  {MCM_MOBILEAP_GET_ROAMING_PREF_RESP_V01, QMI_IDL_TYPE16(0, 69), 15},
  {MCM_MOBILEAP_SET_DUALAP_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 71), 11},
  {MCM_MOBILEAP_STATION_MODE_ENABLE_RESP_V01, QMI_IDL_TYPE16(0, 73), 11},
  {MCM_MOBILEAP_STATION_MODE_DISABLE_RESP_V01, QMI_IDL_TYPE16(0, 75), 11},
  {MCM_MOBILEAP_EVENT_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 77), 11}
};

static const qmi_idl_service_message_table_entry mcm_mobileap_service_indication_messages_v01[] = {
  {MCM_MOBILEAP_UNSOL_EVENT_IND_V01, QMI_IDL_TYPE16(0, 78), 7}
};

/*Service Object*/
struct qmi_idl_service_object mcm_mobileap_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x406,
  465,
  { sizeof(mcm_mobileap_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(mcm_mobileap_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(mcm_mobileap_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { mcm_mobileap_service_command_messages_v01, mcm_mobileap_service_response_messages_v01, mcm_mobileap_service_indication_messages_v01},
  &mcm_mobileap_qmi_idl_type_table_object_v01,
  0x02,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type mcm_mobileap_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( MCM_MOBILEAP_V01_IDL_MAJOR_VERS != idl_maj_version || MCM_MOBILEAP_V01_IDL_MINOR_VERS != idl_min_version
       || MCM_MOBILEAP_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&mcm_mobileap_qmi_idl_service_object_v01;
}

