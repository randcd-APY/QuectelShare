/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        Q U A L C O M M _ M O B I L E _ A C C E S S _ P O I N T _ M S G R _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the qcmap_msgr service Data structures.

  Copyright (c) 2012-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Thu Sep 27 2018 (Spin 0)
   From IDL File: qualcomm_mobile_access_point_msgr_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "qualcomm_mobile_access_point_msgr_v01.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t qcmap_msgr_wlan_if_info_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_if_info_v01, wlan_ap_type),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_if_info_v01, if_name),
  QCMAP_MAX_IFACE_NAME_SIZE_V01,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_if_info_v01, state),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_if_info_v01, wlan_dev_type),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_snat_entry_config_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_snat_entry_config_v01, private_ip_addr),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_snat_entry_config_v01, private_port),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_snat_entry_config_v01, global_port),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_snat_entry_config_v01, protocol),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_net_policy_info_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_net_policy_info_v01, tech_pref),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_net_policy_info_v01, ip_family),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_net_policy_info_v01, v4_profile_id_3gpp2),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_net_policy_info_v01, v4_profile_id_3gpp),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_net_policy_info_v01, v6_profile_id_3gpp2),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_net_policy_info_v01, v6_profile_id_3gpp),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_net_profile_and_policy_info_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_net_profile_and_policy_info_v01, profile_handle),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_net_profile_and_policy_info_v01, tech_pref),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_net_profile_and_policy_info_v01, ip_family),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_net_profile_and_policy_info_v01, v4_profile_id_3gpp2),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_net_profile_and_policy_info_v01, v4_profile_id_3gpp),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_net_profile_and_policy_info_v01, v6_profile_id_3gpp2),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_net_profile_and_policy_info_v01, v6_profile_id_3gpp),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_wwan_statistics_type_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_statistics_type_v01, bytes_rx),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_statistics_type_v01, bytes_tx),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_statistics_type_v01, pkts_rx),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_statistics_type_v01, pkts_tx),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_statistics_type_v01, pkts_dropped_rx),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_statistics_type_v01, pkts_dropped_tx),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_ip4_addr_subnet_mask_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_ip4_addr_subnet_mask_v01, addr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_ip4_addr_subnet_mask_v01, subnet_mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_ip6_addr_prefix_len_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_ip6_addr_prefix_len_v01, addr),
  QCMAP_MSGR_IPV6_ADDR_LEN_V01,

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_ip6_addr_prefix_len_v01, prefix_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_tcp_udp_port_range_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qcmap_tcp_udp_port_range_v01, port),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qcmap_tcp_udp_port_range_v01, range),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_ip4_tos_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_ip4_tos_v01, value),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_ip4_tos_v01, mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_ip6_traffic_class_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_ip6_traffic_class_v01, value),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_ip6_traffic_class_v01, mask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_dhcp_config_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_dhcp_config_v01, dhcp_start_ip),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_dhcp_config_v01, dhcp_end_ip),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_dhcp_config_v01, lease_time),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_ip_passthrough_config_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_ip_passthrough_config_v01, device_type),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_ip_passthrough_config_v01, mac_addr),
  QCMAP_MSGR_MAC_ADDR_LEN_V01,

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_ip_passthrough_config_v01, client_device_name),
  QCMAP_MSGR_DEVICE_NAME_MAX_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_lan_config_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_lan_config_v01, gw_ip),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_lan_config_v01, netmask),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_lan_config_v01, enable_dhcp),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_lan_config_v01, dhcp_config),
  QMI_IDL_TYPE88(0, 10),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_wlan_config_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_config_v01, gw_ip),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_config_v01, netmask),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_config_v01, dhcp_config),
  QMI_IDL_TYPE88(0, 10),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_usb_config_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_usb_config_v01, gw_ip),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_usb_config_v01, netmask),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_usb_config_v01, usb_ip),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_usb_config_v01, lease_time),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_ap_mode_wlan_usb_ip_config_data_v01[] = {
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_ap_mode_wlan_usb_ip_config_v01, wlan_config),
  QMI_IDL_TYPE88(0, 13),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_ap_mode_wlan_usb_ip_config_v01, usb_config),
  QMI_IDL_TYPE88(0, 14),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_sta_static_ip_config_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_sta_static_ip_config_v01, ip_addr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_sta_static_ip_config_v01, gw_ip),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_sta_static_ip_config_v01, netmask),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_sta_static_ip_config_v01, dns_addr),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_station_mode_config_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_station_mode_config_v01, conn_type),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_station_mode_config_v01, static_ip_config),
  QMI_IDL_TYPE88(0, 16),
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_station_mode_config_v01, ap_sta_bridge_mode),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_wwan_call_end_reason_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_call_end_reason_v01, wwan_call_end_reason_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_call_end_reason_v01, wwan_call_end_reason_code),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_data_bitrate_data_v01[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_data_bitrate_v01, tx_rate),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_data_bitrate_v01, rx_rate),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_data_bitrate_v01, max_tx_rate),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_data_bitrate_v01, max_rx_rate),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_dhcp_reservation_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_dhcp_reservation_v01, client_mac_addr),
  QCMAP_MSGR_MAC_ADDR_LEN_V01,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_dhcp_reservation_v01, client_reserved_ip),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_dhcp_reservation_v01, client_device_name),
  QCMAP_MSGR_DEVICE_NAME_MAX_V01,

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_dhcp_reservation_v01, enable_reservation),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_sip_server_info_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_sip_server_info_v01, pcscf_info_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_sip_server_info_v01, pcscf_ip_addr),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_sip_server_info_v01, pcscf_fqdn),
  QCMAP_MSGR_PCSCF_FQDN_MAX_LENGTH_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_client_ipv6_addr_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_client_ipv6_addr_v01, addr),
  QCMAP_MSGR_IPV6_ADDR_LEN_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_connected_device_info_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_connected_device_info_v01, client_mac_addr),
  QCMAP_MSGR_MAC_ADDR_LEN_V01,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_connected_device_info_v01, device_type),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_connected_device_info_v01, host_name),
  QCMAP_MSGR_DEVICE_NAME_MAX_V01,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_connected_device_info_v01, lease_expiry_time),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_connected_device_info_v01, ipv4_addr),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_connected_device_info_v01, ll_ipv6_addr),
  QCMAP_MSGR_IPV6_ADDR_LEN_V01,

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_connected_device_info_v01, ipv6),
  QCMAP_MSGR_MAX_IPV6_ADDR_V01,
  QMI_IDL_TYPE88(0, 22),
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_connected_device_info_v01, bytes_rx),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_connected_device_info_v01, bytes_tx),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_connected_device_info_v01, vlan_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_ipv6_sip_server_info_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_ipv6_sip_server_info_v01, pcscf_info_type),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_ipv6_sip_server_info_v01, pcscf_ipv6_addr),
  QCMAP_MSGR_IPV6_ADDR_LEN_V01,

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_ipv6_sip_server_info_v01, pcscf_fqdn),
  QCMAP_MSGR_PCSCF_FQDN_MAX_LENGTH_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_ddns_server_config_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_ddns_server_config_v01, server_url),
  QCMAP_MSGR_DDNS_URL_LENGTH_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_wlan_iface_state_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_iface_state_v01, wlan_iface_state),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_iface_state_v01, wlan_iface_name),
  QCMAP_MSGR_DEVICE_NAME_MAX_V01,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_iface_state_v01, wlan_iface_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_iface_state_v01, ip4_addr),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_iface_state_v01, ip6_addr),
  QCMAP_MSGR_IPV6_ADDR_LEN_V01,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_iface_state_v01, ip_type),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_socksv5_wan_config_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_socksv5_wan_config_v01, wan_iface),
  QCMAP_MAX_IFACE_NAME_SIZE_V01,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_socksv5_wan_config_v01, service_no),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_socksv5_uname_assoc_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_socksv5_uname_assoc_v01, uname),
  QCMAP_SOCKSV5_MAX_UNAME_PASSWD_LEN_V01,

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_socksv5_uname_assoc_v01, service_no),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_socksv5_config_file_paths_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_socksv5_config_file_paths_v01, conf_file),
  QCMAP_SOCKSV5_MAX_UNAME_PASSWD_LEN_V01,

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_socksv5_config_file_paths_v01, auth_file),
  QCMAP_SOCKSV5_MAX_UNAME_PASSWD_LEN_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_gsb_config_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_gsb_config_v01, if_name),
  QCMAP_MAX_IFACE_NAME_SIZE_V01,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_gsb_config_v01, bw_reqd_in_mb),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_gsb_config_v01, if_high_watermark),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_gsb_config_v01, if_low_watermark),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_gsb_config_v01, if_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_gsb_config_v01, ap_ip),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_vlan_config_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_vlan_config_v01, local_iface),
  QCMAP_MAX_IFACE_NAME_SIZE_V01,

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_vlan_config_v01, vlan_id),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_vlan_config_v01, ipa_offload),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_set_unmanaged_l2tp_state_config_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_unmanaged_l2tp_state_config_v01, enable),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_l2tp_session_config_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_l2tp_session_config_v01, session_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_l2tp_session_config_v01, peer_session_id),

  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_l2tp_session_config_v01, v4_addr),
  QMI_IDL_TYPE88(0, 5),
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_l2tp_session_config_v01, v6_addr),
  QMI_IDL_TYPE88(0, 6),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_l2tp_config_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_l2tp_config_v01, local_tunnel_id),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_l2tp_config_v01, peer_tunnel_id),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_l2tp_config_v01, local_iface),
  QCMAP_MAX_IFACE_NAME_SIZE_V01,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_l2tp_config_v01, ip_family),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_l2tp_config_v01, peer_ipv6_addr),
  QCMAP_MSGR_IPV6_ADDR_LEN_V01,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_l2tp_config_v01, peer_ipv4_addr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_l2tp_config_v01, proto),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_l2tp_config_v01, local_udp_port),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_l2tp_config_v01, peer_udp_port),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_l2tp_config_v01, session_config),
  QCMAP_MSGR_L2TP_MAX_SESSION_PER_TUNNEL_V01,
  QMI_IDL_TYPE88(0, 34),
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_l2tp_mtu_config_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_l2tp_mtu_config_v01, enable),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_l2tp_TCP_MSS_config_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_l2tp_TCP_MSS_config_v01, enable),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_delete_l2tp_config_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_l2tp_config_v01, tunnel_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_pdn_to_vlan_mapping_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_pdn_to_vlan_mapping_v01, profile_handle),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_pdn_to_vlan_mapping_v01, vlan_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_wwan_info_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_info_v01, iface_name),
  QCMAP_MAX_IFACE_NAME_SIZE_V01,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_info_v01, v4_addr),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_info_v01, v6_addr),
  QCMAP_MSGR_IPV6_ADDR_LEN_V01,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_info_v01, v4_prim_dns_addr),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_info_v01, v4_sec_dns_addr),

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_info_v01, v6_prim_dns_addr),
  QCMAP_MSGR_IPV6_ADDR_LEN_V01,

  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_info_v01, v6_sec_dns_addr),
  QCMAP_MSGR_IPV6_ADDR_LEN_V01,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_pmipv4_sec_router_param_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_pmipv4_sec_router_param_v01, dmnp_prefix),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_pmipv4_sec_router_param_v01, prefix_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qcmap_msgr_bridge_list_data_v01[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bridge_list_v01, curr_bridge),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bridge_list_v01, num_of_bridges),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_SZ_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bridge_list_v01, bridge_arr),
  ((QCMAP_MSGR_MAX_VLAN_ENTRIES_V01) & 0xFF), ((QCMAP_MSGR_MAX_VLAN_ENTRIES_V01) >> 8),

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t qcmap_msgr_indication_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_indication_register_req_msg_v01, register_backhaul_status_indication) - QMI_IDL_OFFSET8(qcmap_msgr_indication_register_req_msg_v01, register_backhaul_status_indication_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_indication_register_req_msg_v01, register_backhaul_status_indication),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_indication_register_req_msg_v01, register_wwan_roaming_indication) - QMI_IDL_OFFSET8(qcmap_msgr_indication_register_req_msg_v01, register_wwan_roaming_indication_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_indication_register_req_msg_v01, register_wwan_roaming_indication)
};

static const uint8_t qcmap_msgr_indication_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_indication_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_mobile_ap_enable_req_msg is empty
 * static const uint8_t qcmap_msgr_mobile_ap_enable_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_mobile_ap_enable_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_enable_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_enable_resp_msg_v01, mobile_ap_handle) - QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_enable_resp_msg_v01, mobile_ap_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_enable_resp_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_mobile_ap_disable_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_disable_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_mobile_ap_disable_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_disable_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_enable_ipv4_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_ipv4_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_enable_ipv4_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_ipv4_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_disable_ipv4_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_ipv4_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_disable_ipv4_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_ipv4_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_ipv4_state_req_msg is empty
 * static const uint8_t qcmap_msgr_get_ipv4_state_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_ipv4_state_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ipv4_state_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_ipv4_state_resp_msg_v01, ipv4_state) - QMI_IDL_OFFSET8(qcmap_msgr_get_ipv4_state_resp_msg_v01, ipv4_state_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ipv4_state_resp_msg_v01, ipv4_state)
};

static const uint8_t qcmap_msgr_enable_ipv6_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_ipv6_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_enable_ipv6_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_ipv6_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_disable_ipv6_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_ipv6_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_disable_ipv6_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_ipv6_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_ipv6_state_req_msg is empty
 * static const uint8_t qcmap_msgr_get_ipv6_state_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_ipv6_state_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ipv6_state_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_ipv6_state_resp_msg_v01, ipv6_state) - QMI_IDL_OFFSET8(qcmap_msgr_get_ipv6_state_resp_msg_v01, ipv6_state_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ipv6_state_resp_msg_v01, ipv6_state)
};

static const uint8_t qcmap_msgr_bring_up_wwan_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_req_msg_v01, call_type) - QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_req_msg_v01, call_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_req_msg_v01, call_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_req_msg_v01, profile_handle_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_bring_up_wwan_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_resp_msg_v01, conn_status) - QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_resp_msg_v01, conn_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_resp_msg_v01, conn_status)
};

static const uint8_t qcmap_msgr_bring_up_wwan_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_ind_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_ind_msg_v01, conn_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_ind_msg_v01, wwan_call_end_reason) - QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_ind_msg_v01, wwan_call_end_reason_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_ind_msg_v01, wwan_call_end_reason),
  QMI_IDL_TYPE88(0, 18),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_ind_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_ind_msg_v01, profile_handle_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_ind_msg_v01, profile_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_ind_msg_v01, wwan_info) - QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_ind_msg_v01, wwan_info_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_up_wwan_ind_msg_v01, wwan_info),
  QMI_IDL_TYPE88(0, 40)
};

static const uint8_t qcmap_msgr_tear_down_wwan_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_req_msg_v01, call_type) - QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_req_msg_v01, call_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_req_msg_v01, call_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_req_msg_v01, profile_handle_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_tear_down_wwan_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_resp_msg_v01, conn_status) - QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_resp_msg_v01, conn_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_resp_msg_v01, conn_status)
};

static const uint8_t qcmap_msgr_tear_down_wwan_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_ind_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_ind_msg_v01, conn_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_ind_msg_v01, wwan_call_end_reason) - QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_ind_msg_v01, wwan_call_end_reason_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_ind_msg_v01, wwan_call_end_reason),
  QMI_IDL_TYPE88(0, 18),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_ind_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_ind_msg_v01, profile_handle_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_tear_down_wwan_ind_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_wwan_status_ind_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_ind_register_req_msg_v01, register_indication)
};

static const uint8_t qcmap_msgr_wwan_status_ind_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_ind_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_wwan_status_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_ind_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_ind_msg_v01, wwan_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_ind_msg_v01, wwan_call_end_reason) - QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_ind_msg_v01, wwan_call_end_reason_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_ind_msg_v01, wwan_call_end_reason),
  QMI_IDL_TYPE88(0, 18),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_ind_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_ind_msg_v01, profile_handle_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_ind_msg_v01, profile_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_ind_msg_v01, wwan_info) - QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_ind_msg_v01, wwan_info_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_ind_msg_v01, wwan_info),
  QMI_IDL_TYPE88(0, 40)
};

static const uint8_t qcmap_msgr_mobile_ap_status_ind_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_status_ind_register_req_msg_v01, register_indication)
};

static const uint8_t qcmap_msgr_mobile_ap_status_ind_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_status_ind_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_mobile_ap_status_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_status_ind_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_status_ind_msg_v01, mobile_ap_status)
};

static const uint8_t qcmap_msgr_station_mode_status_ind_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_station_mode_status_ind_register_req_msg_v01, register_indication)
};

static const uint8_t qcmap_msgr_station_mode_status_ind_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_station_mode_status_ind_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_station_mode_status_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_station_mode_status_ind_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_station_mode_status_ind_msg_v01, station_mode_status)
};

static const uint8_t qcmap_msgr_enable_wlan_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_wlan_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_enable_wlan_req_msg_v01, privileged_client) - QMI_IDL_OFFSET8(qcmap_msgr_enable_wlan_req_msg_v01, privileged_client_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_wlan_req_msg_v01, privileged_client)
};

static const uint8_t qcmap_msgr_enable_wlan_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_wlan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_disable_wlan_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_wlan_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_disable_wlan_req_msg_v01, privileged_client) - QMI_IDL_OFFSET8(qcmap_msgr_disable_wlan_req_msg_v01, privileged_client_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_wlan_req_msg_v01, privileged_client)
};

static const uint8_t qcmap_msgr_disable_wlan_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_wlan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_add_static_nat_entry_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_static_nat_entry_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_static_nat_entry_req_msg_v01, snat_entry_config),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_static_nat_entry_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_add_static_nat_entry_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_static_nat_entry_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_add_static_nat_entry_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_static_nat_entry_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_delete_static_nat_entry_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_static_nat_entry_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_static_nat_entry_req_msg_v01, snat_entry_config),
  QMI_IDL_TYPE88(0, 1),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_delete_static_nat_entry_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_delete_static_nat_entry_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_static_nat_entry_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_delete_static_nat_entry_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_static_nat_entry_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_static_nat_entries_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_static_nat_entries_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_static_nat_entries_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_static_nat_entries_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_static_nat_entries_resp_msg_v01, snat_config) - QMI_IDL_OFFSET8(qcmap_msgr_get_static_nat_entries_resp_msg_v01, snat_config_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_static_nat_entries_resp_msg_v01, snat_config),
  QCMAP_MSGR_MAX_SNAT_ENTRIES_V01,
  QMI_IDL_OFFSET8(qcmap_msgr_get_static_nat_entries_resp_msg_v01, snat_config) - QMI_IDL_OFFSET8(qcmap_msgr_get_static_nat_entries_resp_msg_v01, snat_config_len),
  QMI_IDL_TYPE88(0, 1)
};

static const uint8_t qcmap_msgr_set_dmz_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dmz_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dmz_req_msg_v01, dmz_ip_addr),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_dmz_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_set_dmz_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dmz_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_set_dmz_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dmz_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_dmz_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dmz_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_dmz_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_get_dmz_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dmz_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_get_dmz_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dmz_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_dmz_resp_msg_v01, dmz_ip_addr) - QMI_IDL_OFFSET8(qcmap_msgr_get_dmz_resp_msg_v01, dmz_ip_addr_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dmz_resp_msg_v01, dmz_ip_addr)
};

static const uint8_t qcmap_msgr_delete_dmz_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_dmz_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_delete_dmz_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_delete_dmz_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_dmz_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_delete_dmz_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_dmz_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_wwan_stats_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_stats_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_stats_req_msg_v01, ip_family),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_stats_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_stats_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_stats_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_get_wwan_stats_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_stats_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_stats_resp_msg_v01, wwan_stats) - QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_stats_resp_msg_v01, wwan_stats_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_stats_resp_msg_v01, wwan_stats),
  QMI_IDL_TYPE88(0, 4)
};

static const uint8_t qcmap_msgr_reset_wwan_stats_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_reset_wwan_stats_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_reset_wwan_stats_req_msg_v01, ip_family),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_reset_wwan_stats_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_reset_wwan_stats_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_reset_wwan_stats_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_reset_wwan_stats_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_reset_wwan_stats_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_set_ipsec_vpn_pass_through_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_ipsec_vpn_pass_through_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_ipsec_vpn_pass_through_req_msg_v01, vpn_pass_through_value),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_ipsec_vpn_pass_through_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_set_ipsec_vpn_pass_through_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_ipsec_vpn_pass_through_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_set_ipsec_vpn_pass_through_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_ipsec_vpn_pass_through_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_ipsec_vpn_pass_through_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ipsec_vpn_pass_through_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_ipsec_vpn_pass_through_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_get_ipsec_vpn_pass_through_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ipsec_vpn_pass_through_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_get_ipsec_vpn_pass_through_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ipsec_vpn_pass_through_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_ipsec_vpn_pass_through_resp_msg_v01, vpn_pass_through_value) - QMI_IDL_OFFSET8(qcmap_msgr_get_ipsec_vpn_pass_through_resp_msg_v01, vpn_pass_through_value_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ipsec_vpn_pass_through_resp_msg_v01, vpn_pass_through_value)
};

static const uint8_t qcmap_msgr_set_pptp_vpn_pass_through_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_pptp_vpn_pass_through_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_pptp_vpn_pass_through_req_msg_v01, vpn_pass_through_value),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_pptp_vpn_pass_through_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_set_pptp_vpn_pass_through_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_pptp_vpn_pass_through_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_set_pptp_vpn_pass_through_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_pptp_vpn_pass_through_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_pptp_vpn_pass_through_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_pptp_vpn_pass_through_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_pptp_vpn_pass_through_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_get_pptp_vpn_pass_through_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_pptp_vpn_pass_through_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_get_pptp_vpn_pass_through_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_pptp_vpn_pass_through_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_pptp_vpn_pass_through_resp_msg_v01, vpn_pass_through_value) - QMI_IDL_OFFSET8(qcmap_msgr_get_pptp_vpn_pass_through_resp_msg_v01, vpn_pass_through_value_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_pptp_vpn_pass_through_resp_msg_v01, vpn_pass_through_value)
};

static const uint8_t qcmap_msgr_set_l2tp_vpn_pass_through_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_l2tp_vpn_pass_through_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_l2tp_vpn_pass_through_req_msg_v01, vpn_pass_through_value),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_l2tp_vpn_pass_through_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_set_l2tp_vpn_pass_through_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_l2tp_vpn_pass_through_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_set_l2tp_vpn_pass_through_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_l2tp_vpn_pass_through_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_l2tp_vpn_pass_through_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_l2tp_vpn_pass_through_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_l2tp_vpn_pass_through_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_get_l2tp_vpn_pass_through_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_l2tp_vpn_pass_through_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_get_l2tp_vpn_pass_through_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_l2tp_vpn_pass_through_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_l2tp_vpn_pass_through_resp_msg_v01, vpn_pass_through_value) - QMI_IDL_OFFSET8(qcmap_msgr_get_l2tp_vpn_pass_through_resp_msg_v01, vpn_pass_through_value_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_l2tp_vpn_pass_through_resp_msg_v01, vpn_pass_through_value)
};

static const uint8_t qcmap_msgr_set_nat_type_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_nat_type_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_nat_type_req_msg_v01, nat_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_nat_type_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_set_nat_type_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_nat_type_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_set_nat_type_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_nat_type_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_nat_type_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_nat_type_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_nat_type_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_get_nat_type_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_nat_type_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_get_nat_type_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_nat_type_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_nat_type_resp_msg_v01, nat_type) - QMI_IDL_OFFSET8(qcmap_msgr_get_nat_type_resp_msg_v01, nat_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_nat_type_resp_msg_v01, nat_type)
};

static const uint8_t qcmap_msgr_set_auto_connect_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_auto_connect_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_auto_connect_req_msg_v01, enable),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_auto_connect_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_set_auto_connect_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_auto_connect_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_set_auto_connect_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_auto_connect_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_auto_connect_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_auto_connect_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_auto_connect_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_get_auto_connect_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_auto_connect_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_get_auto_connect_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_auto_connect_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_auto_connect_resp_msg_v01, auto_conn_flag) - QMI_IDL_OFFSET8(qcmap_msgr_get_auto_connect_resp_msg_v01, auto_conn_flag_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_auto_connect_resp_msg_v01, auto_conn_flag)
};

/*
 * qcmap_msgr_get_active_wlan_if_info_req_msg is empty
 * static const uint8_t qcmap_msgr_get_active_wlan_if_info_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_active_wlan_if_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_active_wlan_if_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_active_wlan_if_info_resp_msg_v01, active_entries) - QMI_IDL_OFFSET8(qcmap_msgr_get_active_wlan_if_info_resp_msg_v01, active_entries_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_active_wlan_if_info_resp_msg_v01, active_entries),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_active_wlan_if_info_resp_msg_v01, wlan_if_info) - QMI_IDL_OFFSET8(qcmap_msgr_get_active_wlan_if_info_resp_msg_v01, wlan_if_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_active_wlan_if_info_resp_msg_v01, wlan_if_info),
  QCMAP_MSGR_MAX_WLAN_IFACE_V01,
  QMI_IDL_OFFSET8(qcmap_msgr_get_active_wlan_if_info_resp_msg_v01, wlan_if_info) - QMI_IDL_OFFSET8(qcmap_msgr_get_active_wlan_if_info_resp_msg_v01, wlan_if_info_len),
  QMI_IDL_TYPE88(0, 0)
};

static const uint8_t qcmap_msgr_set_roaming_pref_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_roaming_pref_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_roaming_pref_req_msg_v01, allow_wwan_calls_while_roaming),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_roaming_pref_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_set_roaming_pref_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_roaming_pref_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_set_roaming_pref_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_roaming_pref_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_roaming_pref_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_roaming_pref_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_roaming_pref_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_get_roaming_pref_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_roaming_pref_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_get_roaming_pref_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_roaming_pref_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_roaming_pref_resp_msg_v01, allow_wwan_calls_while_roaming) - QMI_IDL_OFFSET8(qcmap_msgr_get_roaming_pref_resp_msg_v01, allow_wwan_calls_while_roaming_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_roaming_pref_resp_msg_v01, allow_wwan_calls_while_roaming)
};

static const uint8_t qcmap_msgr_set_loopback_flag_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_loopback_flag_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_loopback_flag_req_msg_v01, loopback_flag)
};

static const uint8_t qcmap_msgr_set_loopback_flag_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_loopback_flag_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_loopback_flag_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_loopback_flag_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_loopback_flag_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_loopback_flag_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_loopback_flag_resp_msg_v01, loopback_flag) - QMI_IDL_OFFSET8(qcmap_msgr_get_loopback_flag_resp_msg_v01, loopback_flag_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_loopback_flag_resp_msg_v01, loopback_flag)
};

static const uint8_t qcmap_msgr_add_firewall_entry_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip_version),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, next_hdr_prot) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, next_hdr_prot_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, next_hdr_prot),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, tcp_udp_src) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, tcp_udp_src_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, tcp_udp_src),
  QMI_IDL_TYPE88(0, 7),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, tcp_udp_dst) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, tcp_udp_dst_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, tcp_udp_dst),
  QMI_IDL_TYPE88(0, 7),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, icmp_type) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, icmp_type_valid)),
  0x13,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, icmp_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, icmp_code) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, icmp_code_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, icmp_code),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, esp_spi) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, esp_spi_valid)),
  0x15,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, esp_spi),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip4_src_addr) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip4_src_addr_valid)),
  0x16,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip4_src_addr),
  QMI_IDL_TYPE88(0, 5),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip4_dst_addr) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip4_dst_addr_valid)),
  0x17,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip4_dst_addr),
  QMI_IDL_TYPE88(0, 5),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip4_tos) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip4_tos_valid)),
  0x18,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip4_tos),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip6_src_addr) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip6_src_addr_valid)),
  0x19,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip6_src_addr),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip6_dst_addr) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip6_dst_addr_valid)),
  0x1A,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip6_dst_addr),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip6_trf_cls) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip6_trf_cls_valid)),
  0x1B,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, ip6_trf_cls),
  QMI_IDL_TYPE88(0, 9),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, upnp_pinhole) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, upnp_pinhole_valid)),
  0x1C,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, upnp_pinhole),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, direction) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, direction_valid)),
  0x1D,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, direction),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, profile_handle_valid)),
  0x1E,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_add_firewall_entry_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_resp_msg_v01, firewall_handle) - QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_resp_msg_v01, firewall_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_firewall_entry_resp_msg_v01, firewall_handle)
};

static const uint8_t qcmap_msgr_get_firewall_entries_handle_list_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entries_handle_list_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entries_handle_list_req_msg_v01, ip_version),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entries_handle_list_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entries_handle_list_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entries_handle_list_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_get_firewall_entries_handle_list_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entries_handle_list_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entries_handle_list_resp_msg_v01, firewall_handle_list) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entries_handle_list_resp_msg_v01, firewall_handle_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entries_handle_list_resp_msg_v01, firewall_handle_list),
  QCMAP_MSGR_MAX_FIREWALL_ENTRIES_V01,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entries_handle_list_resp_msg_v01, firewall_handle_list) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entries_handle_list_resp_msg_v01, firewall_handle_list_len)
};

static const uint8_t qcmap_msgr_get_firewall_entry_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_req_msg_v01, firewall_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_get_firewall_entry_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip_version) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip_version_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip_version),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, next_hdr_prot) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, next_hdr_prot_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, next_hdr_prot),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, tcp_udp_src) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, tcp_udp_src_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, tcp_udp_src),
  QMI_IDL_TYPE88(0, 7),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, tcp_udp_dst) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, tcp_udp_dst_valid)),
  0x13,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, tcp_udp_dst),
  QMI_IDL_TYPE88(0, 7),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, icmp_type) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, icmp_type_valid)),
  0x14,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, icmp_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, icmp_code) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, icmp_code_valid)),
  0x15,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, icmp_code),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, esp_spi) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, esp_spi_valid)),
  0x16,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, esp_spi),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip4_src_addr) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip4_src_addr_valid)),
  0x17,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip4_src_addr),
  QMI_IDL_TYPE88(0, 5),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip4_dst_addr) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip4_dst_addr_valid)),
  0x18,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip4_dst_addr),
  QMI_IDL_TYPE88(0, 5),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip4_tos) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip4_tos_valid)),
  0x19,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip4_tos),
  QMI_IDL_TYPE88(0, 8),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip6_src_addr) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip6_src_addr_valid)),
  0x1A,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip6_src_addr),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip6_dst_addr) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip6_dst_addr_valid)),
  0x1B,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip6_dst_addr),
  QMI_IDL_TYPE88(0, 6),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip6_trf_cls) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip6_trf_cls_valid)),
  0x1C,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, ip6_trf_cls),
  QMI_IDL_TYPE88(0, 9),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, direction) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, direction_valid)),
  0x1D,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_entry_resp_msg_v01, direction)
};

static const uint8_t qcmap_msgr_delete_firewall_entry_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_firewall_entry_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_firewall_entry_req_msg_v01, firewall_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_delete_firewall_entry_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_delete_firewall_entry_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_firewall_entry_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_delete_firewall_entry_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_firewall_entry_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_wwan_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_req_msg_v01, addr_type_op),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_get_wwan_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v4_addr) - QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v4_addr_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v4_addr),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v6_addr) - QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v6_addr_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v6_addr),
  QCMAP_MSGR_IPV6_ADDR_LEN_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v4_prim_dns_addr) - QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v4_prim_dns_addr_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v4_prim_dns_addr),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v4_sec_dns_addr) - QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v4_sec_dns_addr_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v4_sec_dns_addr),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v6_prim_dns_addr) - QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v6_prim_dns_addr_valid)),
  0x14,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v6_prim_dns_addr),
  QCMAP_MSGR_IPV6_ADDR_LEN_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v6_sec_dns_addr) - QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v6_sec_dns_addr_valid)),
  0x15,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_config_resp_msg_v01, v6_sec_dns_addr),
  QCMAP_MSGR_IPV6_ADDR_LEN_V01
};

static const uint8_t qcmap_msgr_mobile_ap_status_req_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_status_req_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_mobile_ap_status_resp_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_status_resp_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_status_resp_v01, mobile_ap_status) - QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_status_resp_v01, mobile_ap_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_mobile_ap_status_resp_v01, mobile_ap_status)
};

static const uint8_t qcmap_msgr_wwan_status_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_req_msg_v01, call_type) - QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_req_msg_v01, call_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_req_msg_v01, call_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_req_msg_v01, profile_handle_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_wwan_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_resp_msg_v01, conn_status) - QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_resp_msg_v01, conn_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_status_resp_msg_v01, conn_status)
};

/*
 * qcmap_msgr_get_station_mode_status_req_msg is empty
 * static const uint8_t qcmap_msgr_get_station_mode_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_station_mode_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_station_mode_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_station_mode_status_resp_msg_v01, conn_status) - QMI_IDL_OFFSET8(qcmap_msgr_get_station_mode_status_resp_msg_v01, conn_status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_station_mode_status_resp_msg_v01, conn_status)
};

static const uint8_t qcmap_msgr_set_nat_timeout_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_nat_timeout_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_nat_timeout_req_msg_v01, nat_timeout_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_nat_timeout_req_msg_v01, timeout_value)
};

static const uint8_t qcmap_msgr_set_nat_timeout_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_nat_timeout_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_nat_timeout_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_nat_timeout_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_nat_timeout_req_msg_v01, nat_timeout_type)
};

static const uint8_t qcmap_msgr_get_nat_timeout_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_nat_timeout_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_nat_timeout_resp_msg_v01, timeout_value) - QMI_IDL_OFFSET8(qcmap_msgr_get_nat_timeout_resp_msg_v01, timeout_value_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_nat_timeout_resp_msg_v01, timeout_value)
};

static const uint8_t qcmap_msgr_set_lan_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_lan_config_req_msg_v01, lan_config) - QMI_IDL_OFFSET8(qcmap_msgr_set_lan_config_req_msg_v01, lan_config_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_lan_config_req_msg_v01, lan_config),
  QMI_IDL_TYPE88(0, 12)
};

static const uint8_t qcmap_msgr_set_lan_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_lan_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_set_wlan_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, wlan_mode) - QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, wlan_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, wlan_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, guest_ap_access_profile) - QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, guest_ap_access_profile_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, guest_ap_access_profile),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, guest_ap_2_access_profile) - QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, guest_ap_2_access_profile_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, guest_ap_2_access_profile),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, guest_ap_3_access_profile) - QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, guest_ap_3_access_profile_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, guest_ap_3_access_profile),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, station_config) - QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, station_config_valid)),
  0x14,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_req_msg_v01, station_config),
  QMI_IDL_TYPE88(0, 17)
};

static const uint8_t qcmap_msgr_set_wlan_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_wlan_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_wlan_status_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_status_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_wlan_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_status_resp_msg_v01, wlan_mode) - QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_status_resp_msg_v01, wlan_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_status_resp_msg_v01, wlan_mode)
};

static const uint8_t qcmap_msgr_activate_wlan_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_activate_wlan_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_activate_wlan_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_activate_wlan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_activate_lan_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_activate_lan_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_activate_lan_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_activate_lan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_lan_config_req_msg is empty
 * static const uint8_t qcmap_msgr_get_lan_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_lan_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_lan_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_lan_config_resp_msg_v01, lan_config) - QMI_IDL_OFFSET8(qcmap_msgr_get_lan_config_resp_msg_v01, lan_config_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_lan_config_resp_msg_v01, lan_config),
  QMI_IDL_TYPE88(0, 12),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_lan_config_resp_msg_v01, ap_wlan_usb_ip_config) - QMI_IDL_OFFSET8(qcmap_msgr_get_lan_config_resp_msg_v01, ap_wlan_usb_ip_config_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_lan_config_resp_msg_v01, ap_wlan_usb_ip_config),
  QMI_IDL_TYPE88(0, 15)
};

/*
 * qcmap_msgr_get_wlan_config_req_msg is empty
 * static const uint8_t qcmap_msgr_get_wlan_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_wlan_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, wlan_mode) - QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, wlan_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, wlan_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, station_config) - QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, station_config_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, station_config),
  QMI_IDL_TYPE88(0, 17),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, guest_ap_access_profile) - QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, guest_ap_access_profile_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, guest_ap_access_profile),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, guest_ap_2_access_profile) - QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, guest_ap_2_access_profile_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, guest_ap_2_access_profile),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, guest_ap_3_access_profile) - QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, guest_ap_3_access_profile_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wlan_config_resp_msg_v01, guest_ap_3_access_profile)
};

static const uint8_t qcmap_msgr_activate_hostapd_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_activate_hostapd_config_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_activate_hostapd_config_req_msg_v01, ap_type) - QMI_IDL_OFFSET8(qcmap_msgr_activate_hostapd_config_req_msg_v01, ap_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_activate_hostapd_config_req_msg_v01, ap_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_activate_hostapd_config_req_msg_v01, action_type) - QMI_IDL_OFFSET8(qcmap_msgr_activate_hostapd_config_req_msg_v01, action_type_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_activate_hostapd_config_req_msg_v01, action_type),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_activate_hostapd_config_req_msg_v01, privileged_client) - QMI_IDL_OFFSET8(qcmap_msgr_activate_hostapd_config_req_msg_v01, privileged_client_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_activate_hostapd_config_req_msg_v01, privileged_client)
};

static const uint8_t qcmap_msgr_activate_hostapd_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_activate_hostapd_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_activate_supplicant_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_activate_supplicant_config_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_activate_supplicant_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_activate_supplicant_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_usb_link_up_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_usb_link_up_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_usb_link_up_req_msg_v01, usb_link)
};

static const uint8_t qcmap_msgr_usb_link_up_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_usb_link_up_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_usb_link_down_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_usb_link_down_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_usb_link_down_req_msg_v01, usb_link)
};

static const uint8_t qcmap_msgr_usb_link_down_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_usb_link_down_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_set_wwan_policy_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_wwan_policy_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_wwan_policy_req_msg_v01, wwan_policy),
  QMI_IDL_TYPE88(0, 2)
};

static const uint8_t qcmap_msgr_set_wwan_policy_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_wwan_policy_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_wwan_policy_resp_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_set_wwan_policy_resp_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_wwan_policy_resp_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_create_wwan_policy_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_create_wwan_policy_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_create_wwan_policy_req_msg_v01, wwan_policy),
  QMI_IDL_TYPE88(0, 2)
};

static const uint8_t qcmap_msgr_create_wwan_policy_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_create_wwan_policy_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_create_wwan_policy_resp_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_create_wwan_policy_resp_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_create_wwan_policy_resp_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_update_wwan_policy_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_update_wwan_policy_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_update_wwan_policy_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_update_wwan_policy_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_update_wwan_policy_req_msg_v01, profile_handle),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_update_wwan_policy_req_msg_v01, update_req) - QMI_IDL_OFFSET8(qcmap_msgr_update_wwan_policy_req_msg_v01, update_req_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_update_wwan_policy_req_msg_v01, update_req),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_update_wwan_policy_req_msg_v01, wwan_policy) - QMI_IDL_OFFSET8(qcmap_msgr_update_wwan_policy_req_msg_v01, wwan_policy_valid)),
  0x12,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_update_wwan_policy_req_msg_v01, wwan_policy),
  QMI_IDL_TYPE88(0, 2)
};

static const uint8_t qcmap_msgr_update_wwan_policy_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_update_wwan_policy_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_wwan_policy_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_policy_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_policy_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_policy_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_get_wwan_policy_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_policy_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_policy_resp_msg_v01, wwan_policy) - QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_policy_resp_msg_v01, wwan_policy_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_policy_resp_msg_v01, wwan_policy),
  QMI_IDL_TYPE88(0, 2)
};

/*
 * qcmap_msgr_enable_upnp_req_msg is empty
 * static const uint8_t qcmap_msgr_enable_upnp_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_enable_upnp_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_upnp_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_disable_upnp_req_msg is empty
 * static const uint8_t qcmap_msgr_disable_upnp_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_disable_upnp_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_upnp_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_enable_dlna_req_msg is empty
 * static const uint8_t qcmap_msgr_enable_dlna_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_enable_dlna_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_dlna_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_disable_dlna_req_msg is empty
 * static const uint8_t qcmap_msgr_disable_dlna_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_disable_dlna_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_dlna_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_set_firewall_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_firewall_config_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_firewall_config_req_msg_v01, firewall_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_firewall_config_req_msg_v01, pkts_allowed) - QMI_IDL_OFFSET8(qcmap_msgr_set_firewall_config_req_msg_v01, pkts_allowed_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_firewall_config_req_msg_v01, pkts_allowed),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_firewall_config_req_msg_v01, upnp_pinhole_flag) - QMI_IDL_OFFSET8(qcmap_msgr_set_firewall_config_req_msg_v01, upnp_pinhole_flag_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_firewall_config_req_msg_v01, upnp_pinhole_flag),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_firewall_config_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_set_firewall_config_req_msg_v01, profile_handle_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_firewall_config_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_set_firewall_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_firewall_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_firewall_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_config_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_config_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_config_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_config_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_get_firewall_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_config_resp_msg_v01, firewall_enabled) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_config_resp_msg_v01, firewall_enabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_config_resp_msg_v01, firewall_enabled),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_config_resp_msg_v01, pkts_allowed) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_config_resp_msg_v01, pkts_allowed_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_config_resp_msg_v01, pkts_allowed),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_config_resp_msg_v01, upnp_pinhole_flag) - QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_config_resp_msg_v01, upnp_pinhole_flag_valid)),
  0x12,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_firewall_config_resp_msg_v01, upnp_pinhole_flag)
};

/*
 * qcmap_msgr_enable_multicast_dns_req_msg is empty
 * static const uint8_t qcmap_msgr_enable_multicast_dns_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_enable_multicast_dns_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_multicast_dns_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_disable_multicast_dns_req_msg is empty
 * static const uint8_t qcmap_msgr_disable_multicast_dns_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_disable_multicast_dns_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_multicast_dns_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_upnp_status_req_msg is empty
 * static const uint8_t qcmap_msgr_get_upnp_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_upnp_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_upnp_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_upnp_status_resp_msg_v01, upnp_mode) - QMI_IDL_OFFSET8(qcmap_msgr_get_upnp_status_resp_msg_v01, upnp_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_upnp_status_resp_msg_v01, upnp_mode)
};

/*
 * qcmap_msgr_get_dlna_status_req_msg is empty
 * static const uint8_t qcmap_msgr_get_dlna_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_dlna_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_status_resp_msg_v01, dlna_mode) - QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_status_resp_msg_v01, dlna_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_status_resp_msg_v01, dlna_mode)
};

/*
 * qcmap_msgr_get_multicast_dns_status_req_msg is empty
 * static const uint8_t qcmap_msgr_get_multicast_dns_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_multicast_dns_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_multicast_dns_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_multicast_dns_status_resp_msg_v01, mdns_mode) - QMI_IDL_OFFSET8(qcmap_msgr_get_multicast_dns_status_resp_msg_v01, mdns_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_multicast_dns_status_resp_msg_v01, mdns_mode)
};

static const uint8_t qcmap_msgr_set_qcmap_bootup_cfg_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_qcmap_bootup_cfg_req_msg_v01, mobileap_bootup_flag) - QMI_IDL_OFFSET8(qcmap_msgr_set_qcmap_bootup_cfg_req_msg_v01, mobileap_bootup_flag_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_qcmap_bootup_cfg_req_msg_v01, mobileap_bootup_flag),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_qcmap_bootup_cfg_req_msg_v01, wlan_bootup_flag) - QMI_IDL_OFFSET8(qcmap_msgr_set_qcmap_bootup_cfg_req_msg_v01, wlan_bootup_flag_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_qcmap_bootup_cfg_req_msg_v01, wlan_bootup_flag)
};

static const uint8_t qcmap_msgr_set_qcmap_bootup_cfg_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_qcmap_bootup_cfg_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_qcmap_bootup_cfg_req_msg is empty
 * static const uint8_t qcmap_msgr_get_qcmap_bootup_cfg_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_v01, mobileap_bootup_flag) - QMI_IDL_OFFSET8(qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_v01, mobileap_bootup_flag_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_v01, mobileap_bootup_flag),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_v01, wlan_bootup_flag) - QMI_IDL_OFFSET8(qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_v01, wlan_bootup_flag_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_v01, wlan_bootup_flag)
};

static const uint8_t qcmap_msgr_set_dlna_media_dir_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dlna_media_dir_req_msg_v01, media_dir),
  ((QCMAP_MSGR_MAX_DLNA_DIR_LEN_V01) & 0xFF), ((QCMAP_MSGR_MAX_DLNA_DIR_LEN_V01) >> 8),
  QMI_IDL_OFFSET8(qcmap_msgr_set_dlna_media_dir_req_msg_v01, media_dir) - QMI_IDL_OFFSET8(qcmap_msgr_set_dlna_media_dir_req_msg_v01, media_dir_len)
};

static const uint8_t qcmap_msgr_set_dlna_media_dir_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dlna_media_dir_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_dlna_media_dir_req_msg is empty
 * static const uint8_t qcmap_msgr_get_dlna_media_dir_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_dlna_media_dir_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_media_dir_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_media_dir_resp_msg_v01, media_dir) - QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_media_dir_resp_msg_v01, media_dir_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_media_dir_resp_msg_v01, media_dir),
  ((QCMAP_MSGR_MAX_DLNA_DIR_LEN_V01) & 0xFF), ((QCMAP_MSGR_MAX_DLNA_DIR_LEN_V01) >> 8),
  QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_media_dir_resp_msg_v01, media_dir) - QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_media_dir_resp_msg_v01, media_dir_len)
};

static const uint8_t qcmap_msgr_get_data_bitrate_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_data_bitrate_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_data_bitrate_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_get_data_bitrate_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_data_bitrate_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_get_data_bitrate_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_data_bitrate_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_data_bitrate_resp_msg_v01, data_rate) - QMI_IDL_OFFSET8(qcmap_msgr_get_data_bitrate_resp_msg_v01, data_rate_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_data_bitrate_resp_msg_v01, data_rate),
  QMI_IDL_TYPE88(0, 19)
};

/*
 * qcmap_msgr_get_upnp_notify_interval_req_msg is empty
 * static const uint8_t qcmap_msgr_get_upnp_notify_interval_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_upnp_notify_interval_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_upnp_notify_interval_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_upnp_notify_interval_resp_msg_v01, notify_interval) - QMI_IDL_OFFSET8(qcmap_msgr_get_upnp_notify_interval_resp_msg_v01, notify_interval_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_upnp_notify_interval_resp_msg_v01, notify_interval)
};

static const uint8_t qcmap_msgr_set_upnp_notify_interval_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_upnp_notify_interval_req_msg_v01, notify_interval) - QMI_IDL_OFFSET8(qcmap_msgr_set_upnp_notify_interval_req_msg_v01, notify_interval_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_upnp_notify_interval_req_msg_v01, notify_interval)
};

static const uint8_t qcmap_msgr_set_upnp_notify_interval_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_upnp_notify_interval_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_dlna_notify_interval_req_msg is empty
 * static const uint8_t qcmap_msgr_get_dlna_notify_interval_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_dlna_notify_interval_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_notify_interval_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_notify_interval_resp_msg_v01, notify_interval) - QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_notify_interval_resp_msg_v01, notify_interval_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_notify_interval_resp_msg_v01, notify_interval)
};

static const uint8_t qcmap_msgr_set_dlna_notify_interval_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_dlna_notify_interval_req_msg_v01, notify_interval) - QMI_IDL_OFFSET8(qcmap_msgr_set_dlna_notify_interval_req_msg_v01, notify_interval_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dlna_notify_interval_req_msg_v01, notify_interval)
};

static const uint8_t qcmap_msgr_set_dlna_notify_interval_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dlna_notify_interval_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_add_dhcp_reservation_record_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_dhcp_reservation_record_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_dhcp_reservation_record_req_msg_v01, dhcp_reservation_record),
  QMI_IDL_TYPE88(0, 20)
};

static const uint8_t qcmap_msgr_add_dhcp_reservation_record_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_dhcp_reservation_record_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_dhcp_reservation_records_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dhcp_reservation_records_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_dhcp_reservation_records_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dhcp_reservation_records_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_dhcp_reservation_records_resp_msg_v01, dhcp_reservation_records) - QMI_IDL_OFFSET8(qcmap_msgr_get_dhcp_reservation_records_resp_msg_v01, dhcp_reservation_records_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dhcp_reservation_records_resp_msg_v01, dhcp_reservation_records),
  QCMAP_MSGR_MAX_DHCP_RESERVATION_ENTRIES_V01,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dhcp_reservation_records_resp_msg_v01, dhcp_reservation_records) - QMI_IDL_OFFSET8(qcmap_msgr_get_dhcp_reservation_records_resp_msg_v01, dhcp_reservation_records_len),
  QMI_IDL_TYPE88(0, 20)
};

static const uint8_t qcmap_msgr_edit_dhcp_reservation_record_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_edit_dhcp_reservation_record_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_edit_dhcp_reservation_record_req_msg_v01, client_reserved_ip) - QMI_IDL_OFFSET8(qcmap_msgr_edit_dhcp_reservation_record_req_msg_v01, client_reserved_ip_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_edit_dhcp_reservation_record_req_msg_v01, client_reserved_ip),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_edit_dhcp_reservation_record_req_msg_v01, dhcp_reservation_record) - QMI_IDL_OFFSET8(qcmap_msgr_edit_dhcp_reservation_record_req_msg_v01, dhcp_reservation_record_valid)),
  0x11,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_edit_dhcp_reservation_record_req_msg_v01, dhcp_reservation_record),
  QMI_IDL_TYPE88(0, 20)
};

static const uint8_t qcmap_msgr_edit_dhcp_reservation_record_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_edit_dhcp_reservation_record_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_delete_dhcp_reservation_record_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_dhcp_reservation_record_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_delete_dhcp_reservation_record_req_msg_v01, client_reserved_ip) - QMI_IDL_OFFSET8(qcmap_msgr_delete_dhcp_reservation_record_req_msg_v01, client_reserved_ip_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_dhcp_reservation_record_req_msg_v01, client_reserved_ip)
};

static const uint8_t qcmap_msgr_delete_dhcp_reservation_record_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_dhcp_reservation_record_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_enable_alg_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_alg_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_alg_req_msg_v01, alg_type_op)
};

static const uint8_t qcmap_msgr_enable_alg_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_alg_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_disable_alg_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_alg_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_alg_req_msg_v01, alg_type_op)
};

static const uint8_t qcmap_msgr_disable_alg_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_alg_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_set_webserver_wwan_access_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_webserver_wwan_access_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_webserver_wwan_access_req_msg_v01, webserver_wwan_access)
};

static const uint8_t qcmap_msgr_set_webserver_wwan_access_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_webserver_wwan_access_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_webserver_wwan_access_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_webserver_wwan_access_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_webserver_wwan_access_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_webserver_wwan_access_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_webserver_wwan_access_resp_msg_v01, webserver_wwan_access) - QMI_IDL_OFFSET8(qcmap_msgr_get_webserver_wwan_access_resp_msg_v01, webserver_wwan_access_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_webserver_wwan_access_resp_msg_v01, webserver_wwan_access)
};

static const uint8_t qcmap_msgr_set_sip_server_info_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_sip_server_info_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_sip_server_info_req_msg_v01, default_sip_server_info),
  QMI_IDL_TYPE88(0, 21)
};

static const uint8_t qcmap_msgr_set_sip_server_info_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_sip_server_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_sip_server_info_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_sip_server_info_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_sip_server_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_sip_server_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_sip_server_info_resp_msg_v01, default_sip_server_info) - QMI_IDL_OFFSET8(qcmap_msgr_get_sip_server_info_resp_msg_v01, default_sip_server_info_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_sip_server_info_resp_msg_v01, default_sip_server_info),
  QMI_IDL_TYPE88(0, 21),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_sip_server_info_resp_msg_v01, network_sip_server_info) - QMI_IDL_OFFSET8(qcmap_msgr_get_sip_server_info_resp_msg_v01, network_sip_server_info_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_sip_server_info_resp_msg_v01, network_sip_server_info),
  QCMAP_MSGR_MAX_SIP_SERVER_ENTRIES_V01,
  QMI_IDL_OFFSET8(qcmap_msgr_get_sip_server_info_resp_msg_v01, network_sip_server_info) - QMI_IDL_OFFSET8(qcmap_msgr_get_sip_server_info_resp_msg_v01, network_sip_server_info_len),
  QMI_IDL_TYPE88(0, 21)
};

static const uint8_t qcmap_msgr_restore_factory_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_restore_factory_config_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_restore_factory_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_restore_factory_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_set_odu_mode_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_odu_mode_req_msg_v01, mode) - QMI_IDL_OFFSET8(qcmap_msgr_set_odu_mode_req_msg_v01, mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_odu_mode_req_msg_v01, mode)
};

static const uint8_t qcmap_msgr_set_odu_mode_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_odu_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_odu_mode_req_msg is empty
 * static const uint8_t qcmap_msgr_get_odu_mode_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_odu_mode_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_odu_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_odu_mode_resp_msg_v01, mode) - QMI_IDL_OFFSET8(qcmap_msgr_get_odu_mode_resp_msg_v01, mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_odu_mode_resp_msg_v01, mode)
};

static const uint8_t qcmap_msgr_set_odu_pswd_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_odu_pswd_req_msg_v01, password) - QMI_IDL_OFFSET8(qcmap_msgr_set_odu_pswd_req_msg_v01, password_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qcmap_msgr_set_odu_pswd_req_msg_v01, password),
  15
};

static const uint8_t qcmap_msgr_set_odu_pswd_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_odu_pswd_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_connected_devices_info_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_connected_devices_info_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_connected_devices_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_connected_devices_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_connected_devices_info_resp_msg_v01, connected_devices_info) - QMI_IDL_OFFSET8(qcmap_msgr_get_connected_devices_info_resp_msg_v01, connected_devices_info_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_connected_devices_info_resp_msg_v01, connected_devices_info),
  QCMAP_MSGR_MAX_CONNECTED_DEVICES_V01,
  QMI_IDL_OFFSET8(qcmap_msgr_get_connected_devices_info_resp_msg_v01, connected_devices_info) - QMI_IDL_OFFSET8(qcmap_msgr_get_connected_devices_info_resp_msg_v01, connected_devices_info_len),
  QMI_IDL_TYPE88(0, 23)
};

static const uint8_t qcmap_msgr_get_ipv6_sip_server_info_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ipv6_sip_server_info_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_ipv6_sip_server_info_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ipv6_sip_server_info_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_ipv6_sip_server_info_resp_msg_v01, network_ipv6_sip_server_info) - QMI_IDL_OFFSET8(qcmap_msgr_get_ipv6_sip_server_info_resp_msg_v01, network_ipv6_sip_server_info_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ipv6_sip_server_info_resp_msg_v01, network_ipv6_sip_server_info),
  QCMAP_MSGR_MAX_SIP_SERVER_ENTRIES_V01,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ipv6_sip_server_info_resp_msg_v01, network_ipv6_sip_server_info) - QMI_IDL_OFFSET8(qcmap_msgr_get_ipv6_sip_server_info_resp_msg_v01, network_ipv6_sip_server_info_len),
  QMI_IDL_TYPE88(0, 24)
};

static const uint8_t qcmap_msgr_set_supplicant_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_supplicant_config_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_supplicant_config_req_msg_v01, supplicant_config_status)
};

static const uint8_t qcmap_msgr_set_supplicant_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_supplicant_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_cradle_mode_req_msg is empty
 * static const uint8_t qcmap_msgr_get_cradle_mode_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_cradle_mode_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_cradle_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_cradle_mode_resp_msg_v01, mode) - QMI_IDL_OFFSET8(qcmap_msgr_get_cradle_mode_resp_msg_v01, mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_cradle_mode_resp_msg_v01, mode)
};

static const uint8_t qcmap_msgr_set_cradle_mode_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_cradle_mode_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_cradle_mode_req_msg_v01, mode)
};

static const uint8_t qcmap_msgr_set_cradle_mode_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_cradle_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_cradle_mode_status_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_cradle_mode_status_ind_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_cradle_mode_status_ind_msg_v01, cradle_status)
};

/*
 * qcmap_msgr_get_prefix_delegation_config_req_msg is empty
 * static const uint8_t qcmap_msgr_get_prefix_delegation_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_prefix_delegation_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_prefix_delegation_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_prefix_delegation_config_resp_msg_v01, prefix_delegation) - QMI_IDL_OFFSET8(qcmap_msgr_get_prefix_delegation_config_resp_msg_v01, prefix_delegation_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_prefix_delegation_config_resp_msg_v01, prefix_delegation)
};

static const uint8_t qcmap_msgr_set_prefix_delegation_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_prefix_delegation_config_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_prefix_delegation_config_req_msg_v01, prefix_delegation)
};

static const uint8_t qcmap_msgr_set_prefix_delegation_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_prefix_delegation_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_prefix_delegation_status_req_msg is empty
 * static const uint8_t qcmap_msgr_get_prefix_delegation_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_prefix_delegation_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_prefix_delegation_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_prefix_delegation_status_resp_msg_v01, prefix_delegation) - QMI_IDL_OFFSET8(qcmap_msgr_get_prefix_delegation_status_resp_msg_v01, prefix_delegation_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_prefix_delegation_status_resp_msg_v01, prefix_delegation)
};

static const uint8_t qcmap_msgr_set_gateway_url_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_gateway_url_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_gateway_url_req_msg_v01, gateway_url),
  ((QCMAP_MSGR_MAX_GATEWAY_URL_V01) & 0xFF), ((QCMAP_MSGR_MAX_GATEWAY_URL_V01) >> 8),
  QMI_IDL_OFFSET8(qcmap_msgr_set_gateway_url_req_msg_v01, gateway_url) - QMI_IDL_OFFSET8(qcmap_msgr_set_gateway_url_req_msg_v01, gateway_url_len)
};

static const uint8_t qcmap_msgr_set_gateway_url_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_gateway_url_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_gateway_url_req_msg is empty
 * static const uint8_t qcmap_msgr_get_gateway_url_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_gateway_url_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_gateway_url_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_gateway_url_resp_msg_v01, gateway_url) - QMI_IDL_OFFSET8(qcmap_msgr_get_gateway_url_resp_msg_v01, gateway_url_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_gateway_url_resp_msg_v01, gateway_url),
  ((QCMAP_MSGR_MAX_GATEWAY_URL_V01) & 0xFF), ((QCMAP_MSGR_MAX_GATEWAY_URL_V01) >> 8),
  QMI_IDL_OFFSET8(qcmap_msgr_get_gateway_url_resp_msg_v01, gateway_url) - QMI_IDL_OFFSET8(qcmap_msgr_get_gateway_url_resp_msg_v01, gateway_url_len)
};

static const uint8_t qcmap_msgr_enable_dynamic_dns_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_dynamic_dns_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_enable_dynamic_dns_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_dynamic_dns_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_disable_dynamic_dns_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_dynamic_dns_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_disable_dynamic_dns_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_dynamic_dns_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_set_dynamic_dns_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dynamic_dns_config_req_msg_v01, mobile_ap_handle),

  0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dynamic_dns_config_req_msg_v01, ddns_server),
  QCMAP_MSGR_DDNS_URL_LENGTH_V01,

  0x03,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_set_dynamic_dns_config_req_msg_v01, hostname),
  QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01,

  0x04,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_set_dynamic_dns_config_req_msg_v01, login),
  QCMAP_MSGR_DDNS_LOGIN_LENGTH_V01,

  0x05,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_set_dynamic_dns_config_req_msg_v01, password),
  QCMAP_MSGR_DDNS_PASSWORD_LENGTH_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_set_dynamic_dns_config_req_msg_v01, timeout) - QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_set_dynamic_dns_config_req_msg_v01, timeout_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_set_dynamic_dns_config_req_msg_v01, timeout)
};

static const uint8_t qcmap_msgr_set_dynamic_dns_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dynamic_dns_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_dynamic_dns_config_req_msg is empty
 * static const uint8_t qcmap_msgr_get_dynamic_dns_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_dynamic_dns_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  0x03,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01, ddns_config),
  QCMAP_MSGR_MAX_DDNS_SERVER_ENTRIES_V01,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01, ddns_config) - QMI_IDL_OFFSET8(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01, ddns_config_len),
  QMI_IDL_TYPE88(0, 25),

  0x04,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01, enable),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01, hostname) - QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01, hostname_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01, hostname),
  QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01, timeout) - QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01, timeout_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01, timeout)
};

/*
 * qcmap_msgr_get_tiny_proxy_status_req_msg is empty
 * static const uint8_t qcmap_msgr_get_tiny_proxy_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_tiny_proxy_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_tiny_proxy_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_tiny_proxy_status_resp_msg_v01, tiny_proxy_mode) - QMI_IDL_OFFSET8(qcmap_msgr_get_tiny_proxy_status_resp_msg_v01, tiny_proxy_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_tiny_proxy_status_resp_msg_v01, tiny_proxy_mode)
};

static const uint8_t qcmap_msgr_enable_tiny_proxy_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_tiny_proxy_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_enable_tiny_proxy_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_tiny_proxy_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_disable_tiny_proxy_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_tiny_proxy_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_disable_tiny_proxy_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_tiny_proxy_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_set_dlna_whitelisting_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dlna_whitelisting_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dlna_whitelisting_req_msg_v01, dlna_whitelist_allow)
};

static const uint8_t qcmap_msgr_set_dlna_whitelisting_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dlna_whitelisting_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_dlna_whitelisting_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_whitelisting_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_dlna_whitelisting_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_whitelisting_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_whitelisting_resp_msg_v01, dlna_whitelist_allow) - QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_whitelisting_resp_msg_v01, dlna_whitelist_allow_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dlna_whitelisting_resp_msg_v01, dlna_whitelist_allow)
};

static const uint8_t qcmap_msgr_add_dlna_whitelistip_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_dlna_whitelistip_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_dlna_whitelistip_req_msg_v01, dlna_whitelist_ip_addr)
};

static const uint8_t qcmap_msgr_add_dlna_whitelistip_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_dlna_whitelistip_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_delete_dlna_whitelist_ip_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_dlna_whitelist_ip_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_dlna_whitelist_ip_req_msg_v01, dlna_whitelist_ip_addr)
};

static const uint8_t qcmap_msgr_delete_dlna_whitelist_ip_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_dlna_whitelist_ip_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_enable_sta_mode_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_sta_mode_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_enable_sta_mode_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_sta_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_disable_sta_mode_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_sta_mode_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_disable_sta_mode_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_sta_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_wlan_status_ind_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_status_ind_register_req_msg_v01, register_indication)
};

static const uint8_t qcmap_msgr_wlan_status_ind_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_status_ind_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_wlan_status_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_status_ind_msg_v01, wlan_status),

  0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_status_ind_msg_v01, wlan_state),
  QCMAP_MSGR_MAX_WLAN_IFACE_V01,
  QMI_IDL_OFFSET8(qcmap_msgr_wlan_status_ind_msg_v01, wlan_state) - QMI_IDL_OFFSET8(qcmap_msgr_wlan_status_ind_msg_v01, wlan_state_len),
  QMI_IDL_TYPE88(0, 26),

  0x03,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_wlan_status_ind_msg_v01, wlan_mode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_wlan_status_ind_msg_v01, scm_ind) - QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_wlan_status_ind_msg_v01, scm_ind_valid)),
  0x10,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_wlan_status_ind_msg_v01, scm_ind),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_wlan_status_ind_msg_v01, bridge_mode) - QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_wlan_status_ind_msg_v01, bridge_mode_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_wlan_status_ind_msg_v01, bridge_mode)
};

static const uint8_t qcmap_msgr_set_backhaul_pref_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_backhaul_pref_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_backhaul_pref_req_msg_v01, first_priority_backhaul),

  0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_backhaul_pref_req_msg_v01, second_priority_backhaul),

  0x04,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_backhaul_pref_req_msg_v01, third_priority_backhaul),

  0x05,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_backhaul_pref_req_msg_v01, fourth_priority_backhaul),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x06,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_backhaul_pref_req_msg_v01, fifth_priority_backhaul)
};

static const uint8_t qcmap_msgr_set_backhaul_pref_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_backhaul_pref_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_backhaul_pref_req_msg is empty
 * static const uint8_t qcmap_msgr_get_backhaul_pref_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_backhaul_pref_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, first_priority_backhaul) - QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, first_priority_backhaul_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, first_priority_backhaul),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, second_priority_backhaul) - QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, second_priority_backhaul_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, second_priority_backhaul),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, third_priority_backhaul) - QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, third_priority_backhaul_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, third_priority_backhaul),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, fourth_priority_backhaul) - QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, fourth_priority_backhaul_valid)),
  0x13,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, fourth_priority_backhaul),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, fifth_priority_backhaul) - QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, fifth_priority_backhaul_valid)),
  0x14,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_pref_resp_msg_v01, fifth_priority_backhaul)
};

static const uint8_t qcmap_msgr_cradle_mode_status_ind_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_cradle_mode_status_ind_register_req_msg_v01, register_indication)
};

static const uint8_t qcmap_msgr_cradle_mode_status_ind_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_cradle_mode_status_ind_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_ethernet_mode_status_ind_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_ethernet_mode_status_ind_register_req_msg_v01, register_indication)
};

static const uint8_t qcmap_msgr_ethernet_mode_status_ind_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_ethernet_mode_status_ind_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_ethernet_mode_req_msg is empty
 * static const uint8_t qcmap_msgr_get_ethernet_mode_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_ethernet_mode_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ethernet_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_ethernet_mode_resp_msg_v01, mode) - QMI_IDL_OFFSET8(qcmap_msgr_get_ethernet_mode_resp_msg_v01, mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ethernet_mode_resp_msg_v01, mode)
};

static const uint8_t qcmap_msgr_set_ethernet_mode_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_ethernet_mode_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_ethernet_mode_req_msg_v01, mode)
};

static const uint8_t qcmap_msgr_set_ethernet_mode_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_ethernet_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_ethernet_mode_status_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_ethernet_mode_status_ind_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_ethernet_mode_status_ind_msg_v01, eth_status)
};

static const uint8_t qcmap_msgr_set_ip_passthrough_flag_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_ip_passthrough_flag_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_ip_passthrough_flag_req_msg_v01, enable_state),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_ip_passthrough_flag_req_msg_v01, passthrough_config) - QMI_IDL_OFFSET8(qcmap_msgr_set_ip_passthrough_flag_req_msg_v01, passthrough_config_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_ip_passthrough_flag_req_msg_v01, passthrough_config),
  QMI_IDL_TYPE88(0, 11)
};

static const uint8_t qcmap_msgr_set_ip_passthrough_flag_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_ip_passthrough_flag_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_ip_passthrough_flag_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ip_passthrough_flag_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_ip_passthrough_flag_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ip_passthrough_flag_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ip_passthrough_flag_resp_msg_v01, enable_state),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x04,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ip_passthrough_flag_resp_msg_v01, passthrough_config),
  QMI_IDL_TYPE88(0, 11)
};

static const uint8_t qcmap_msgr_get_ip_passthrough_state_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ip_passthrough_state_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_ip_passthrough_state_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ip_passthrough_state_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_ip_passthrough_state_resp_msg_v01, state)
};

static const uint8_t qcmap_msgr_bring_up_bt_tethering_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_up_bt_tethering_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_bring_up_bt_tethering_req_msg_v01, bt_mode) - QMI_IDL_OFFSET8(qcmap_msgr_bring_up_bt_tethering_req_msg_v01, bt_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_up_bt_tethering_req_msg_v01, bt_mode)
};

static const uint8_t qcmap_msgr_bring_up_bt_tethering_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_up_bt_tethering_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_bring_down_bt_tethering_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_down_bt_tethering_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_bring_down_bt_tethering_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_bring_down_bt_tethering_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_bt_tethering_status_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_bt_tethering_status_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_bt_tethering_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_bt_tethering_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_bt_tethering_status_resp_msg_v01, bt_tethering_status) - QMI_IDL_OFFSET8(qcmap_msgr_get_bt_tethering_status_resp_msg_v01, bt_tethering_status_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_bt_tethering_status_resp_msg_v01, bt_tethering_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_bt_tethering_status_resp_msg_v01, bt_tethering_mode) - QMI_IDL_OFFSET8(qcmap_msgr_get_bt_tethering_status_resp_msg_v01, bt_tethering_mode_valid)),
  0x11,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_bt_tethering_status_resp_msg_v01, bt_tethering_mode)
};

static const uint8_t qcmap_msgr_bt_tethering_status_ind_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bt_tethering_status_ind_register_req_msg_v01, register_indication)
};

static const uint8_t qcmap_msgr_bt_tethering_status_ind_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_bt_tethering_status_ind_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_bt_tethering_status_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bt_tethering_status_ind_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bt_tethering_status_ind_msg_v01, bt_tethering_status)
};

static const uint8_t qcmap_msgr_bt_tethering_wan_ind_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bt_tethering_wan_ind_register_req_msg_v01, register_indication)
};

static const uint8_t qcmap_msgr_bt_tethering_wan_ind_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_bt_tethering_wan_ind_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_bt_tethering_wan_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bt_tethering_wan_ind_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_bt_tethering_wan_ind_msg_v01, bt_tethering_wan)
};

static const uint8_t qcmap_msgr_set_initial_packet_threshold_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_initial_packet_threshold_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_initial_packet_threshold_req_msg_v01, packet_count)
};

static const uint8_t qcmap_msgr_set_initial_packet_threshold_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_initial_packet_threshold_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_initial_packet_threshold_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_initial_packet_threshold_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_initial_packet_threshold_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_initial_packet_threshold_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_initial_packet_threshold_resp_msg_v01, packet_count)
};

static const uint8_t qcmap_msgr_enable_socksv5_proxy_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_socksv5_proxy_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_enable_socksv5_proxy_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_socksv5_proxy_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_disable_socksv5_proxy_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_socksv5_proxy_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_disable_socksv5_proxy_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_socksv5_proxy_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_socksv5_proxy_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_socksv5_proxy_config_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_socksv5_proxy_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_socksv5_proxy_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  0x03,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_socksv5_proxy_config_resp_msg_v01, config_file_paths),
  QMI_IDL_TYPE88(0, 30),

  0x04,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_get_socksv5_proxy_config_resp_msg_v01, auth_method),

  0x05,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_get_socksv5_proxy_config_resp_msg_v01, lan_iface),
  QCMAP_MAX_IFACE_NAME_SIZE_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x06,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_get_socksv5_proxy_config_resp_msg_v01, socksv5_wan_service),
  QCMAP_MAX_NUM_BACKHAULS_V01,
  QMI_IDL_TYPE88(0, 28)
};

static const uint8_t qcmap_msgr_set_socksv5_proxy_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_socksv5_proxy_config_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_socksv5_proxy_config_req_msg_v01, config_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_socksv5_proxy_config_req_msg_v01, config_file_paths) - QMI_IDL_OFFSET8(qcmap_msgr_set_socksv5_proxy_config_req_msg_v01, config_file_paths_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_socksv5_proxy_config_req_msg_v01, config_file_paths),
  QMI_IDL_TYPE88(0, 30),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_set_socksv5_proxy_config_req_msg_v01, auth_method) - QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_set_socksv5_proxy_config_req_msg_v01, auth_method_valid)),
  0x11,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_set_socksv5_proxy_config_req_msg_v01, auth_method),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_set_socksv5_proxy_config_req_msg_v01, lan_iface) - QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_set_socksv5_proxy_config_req_msg_v01, lan_iface_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_set_socksv5_proxy_config_req_msg_v01, lan_iface),
  QCMAP_MAX_IFACE_NAME_SIZE_V01,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_set_socksv5_proxy_config_req_msg_v01, uname_assoc) - QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_set_socksv5_proxy_config_req_msg_v01, uname_assoc_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_set_socksv5_proxy_config_req_msg_v01, uname_assoc),
  QMI_IDL_TYPE88(0, 29)
};

static const uint8_t qcmap_msgr_set_socksv5_proxy_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_socksv5_proxy_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_enable_packet_stats_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_packet_stats_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_enable_packet_stats_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_packet_stats_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_disable_packet_stats_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_packet_stats_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_disable_packet_stats_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_packet_stats_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_reset_packet_stats_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_reset_packet_stats_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_reset_packet_stats_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_reset_packet_stats_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_packet_stats_status_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_packet_stats_status_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_packet_stats_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_packet_stats_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_packet_stats_status_resp_msg_v01, status) - QMI_IDL_OFFSET8(qcmap_msgr_packet_stats_status_resp_msg_v01, status_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_packet_stats_status_resp_msg_v01, status)
};

static const uint8_t qcmap_msgr_packet_stats_status_ind_register_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_packet_stats_status_ind_register_req_msg_v01, register_indication)
};

static const uint8_t qcmap_msgr_packet_stats_status_ind_register_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_packet_stats_status_ind_register_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_packet_stats_status_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_packet_stats_status_ind_msg_v01, conn_status),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_packet_stats_status_ind_msg_v01, number_of_entries),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_packet_stats_status_ind_msg_v01, info),
  QCMAP_MSGR_MAX_CONNECTED_DEVICES_V01,
  QMI_IDL_TYPE88(0, 23)
};

static const uint8_t qcmap_msgr_delete_wwan_policy_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_wwan_policy_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_delete_wwan_policy_req_msg_v01, profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_delete_wwan_policy_req_msg_v01, profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_wwan_policy_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_delete_wwan_policy_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_wwan_policy_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_wwan_policy_list_req_msg is empty
 * static const uint8_t qcmap_msgr_wwan_policy_list_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_wwan_policy_list_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_policy_list_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_wwan_policy_list_resp_msg_v01, default_profile_handle) - QMI_IDL_OFFSET8(qcmap_msgr_wwan_policy_list_resp_msg_v01, default_profile_handle_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_policy_list_resp_msg_v01, default_profile_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_wwan_policy_list_resp_msg_v01, wwan_policy) - QMI_IDL_OFFSET8(qcmap_msgr_wwan_policy_list_resp_msg_v01, wwan_policy_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_policy_list_resp_msg_v01, wwan_policy),
  QCMAP_MAX_NUM_BACKHAULS_V01,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_policy_list_resp_msg_v01, wwan_policy) - QMI_IDL_OFFSET8(qcmap_msgr_wwan_policy_list_resp_msg_v01, wwan_policy_len),
  QMI_IDL_TYPE88(0, 3)
};

static const uint8_t qcmap_msgr_set_vlan_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_vlan_config_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_vlan_config_req_msg_v01, config),
  QMI_IDL_TYPE88(0, 32)
};

static const uint8_t qcmap_msgr_set_vlan_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_vlan_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_vlan_config_resp_msg_v01, is_ipa_offload_enabled) - QMI_IDL_OFFSET8(qcmap_msgr_set_vlan_config_resp_msg_v01, is_ipa_offload_enabled_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_vlan_config_resp_msg_v01, is_ipa_offload_enabled)
};

static const uint8_t qcmap_msgr_get_vlan_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_vlan_config_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_vlan_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_vlan_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_vlan_config_resp_msg_v01, vlan_config_list) - QMI_IDL_OFFSET8(qcmap_msgr_get_vlan_config_resp_msg_v01, vlan_config_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_vlan_config_resp_msg_v01, vlan_config_list),
  ((QCMAP_MSGR_MAX_VLAN_ENTRIES_V01) & 0xFF), ((QCMAP_MSGR_MAX_VLAN_ENTRIES_V01) >> 8),
  QMI_IDL_OFFSET8(qcmap_msgr_get_vlan_config_resp_msg_v01, vlan_config_list) - QMI_IDL_OFFSET8(qcmap_msgr_get_vlan_config_resp_msg_v01, vlan_config_list_len),
  QMI_IDL_TYPE88(0, 32)
};

static const uint8_t qcmap_msgr_delete_vlan_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_vlan_config_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_vlan_config_req_msg_v01, config),
  QMI_IDL_TYPE88(0, 32)
};

static const uint8_t qcmap_msgr_delete_vlan_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_vlan_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_set_unmanaged_l2tp_state_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_unmanaged_l2tp_state_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_unmanaged_l2tp_state_req_msg_v01, config),
  QMI_IDL_TYPE88(0, 33)
};

static const uint8_t qcmap_msgr_set_unmanaged_l2tp_state_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_unmanaged_l2tp_state_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_set_l2tp_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_l2tp_config_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_l2tp_config_req_msg_v01, mode),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_l2tp_config_req_msg_v01, config),
  QMI_IDL_TYPE88(0, 35)
};

static const uint8_t qcmap_msgr_set_l2tp_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_l2tp_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_set_MTU_for_l2tp_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_MTU_for_l2tp_config_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_MTU_for_l2tp_config_req_msg_v01, config),
  QMI_IDL_TYPE88(0, 36)
};

static const uint8_t qcmap_msgr_set_MTU_for_l2tp_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_MTU_for_l2tp_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_set_TCP_MSS_for_l2tp_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_TCP_MSS_for_l2tp_config_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_TCP_MSS_for_l2tp_config_req_msg_v01, config),
  QMI_IDL_TYPE88(0, 37)
};

static const uint8_t qcmap_msgr_set_TCP_MSS_for_l2tp_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_TCP_MSS_for_l2tp_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_l2tp_config_req_msg is empty
 * static const uint8_t qcmap_msgr_get_l2tp_config_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_l2tp_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_l2tp_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_l2tp_config_resp_msg_v01, mode),

  0x04,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_l2tp_config_resp_msg_v01, mtu_config),
  QMI_IDL_TYPE88(0, 36),

  0x05,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_l2tp_config_resp_msg_v01, tcp_mss_config),
  QMI_IDL_TYPE88(0, 37),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_l2tp_config_resp_msg_v01, l2tp_config_list) - QMI_IDL_OFFSET8(qcmap_msgr_get_l2tp_config_resp_msg_v01, l2tp_config_list_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_l2tp_config_resp_msg_v01, l2tp_config_list),
  QCMAP_MSGR_L2TP_MAX_TUNNELS_V01,
  QMI_IDL_OFFSET8(qcmap_msgr_get_l2tp_config_resp_msg_v01, l2tp_config_list) - QMI_IDL_OFFSET8(qcmap_msgr_get_l2tp_config_resp_msg_v01, l2tp_config_list_len),
  QMI_IDL_TYPE88(0, 35)
};

static const uint8_t qcmap_msgr_delete_l2tp_tunnel_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_l2tp_tunnel_config_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_l2tp_tunnel_config_req_msg_v01, config),
  QMI_IDL_TYPE88(0, 38)
};

static const uint8_t qcmap_msgr_delete_l2tp_tunnel_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_l2tp_tunnel_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_add_pdn_to_vlan_mapping_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_pdn_to_vlan_mapping_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_pdn_to_vlan_mapping_req_msg_v01, profile_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_pdn_to_vlan_mapping_req_msg_v01, vlan_id)
};

static const uint8_t qcmap_msgr_add_pdn_to_vlan_mapping_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_add_pdn_to_vlan_mapping_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_delete_pdn_to_vlan_mapping_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_pdn_to_vlan_mapping_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_pdn_to_vlan_mapping_req_msg_v01, profile_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_pdn_to_vlan_mapping_req_msg_v01, vlan_id)
};

static const uint8_t qcmap_msgr_delete_pdn_to_vlan_mapping_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_pdn_to_vlan_mapping_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_pdn_to_vlan_mappings_req_msg is empty
 * static const uint8_t qcmap_msgr_get_pdn_to_vlan_mappings_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_pdn_to_vlan_mappings_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_pdn_to_vlan_mappings_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_pdn_to_vlan_mappings_resp_msg_v01, pdn_to_vlan_mappings) - QMI_IDL_OFFSET8(qcmap_msgr_get_pdn_to_vlan_mappings_resp_msg_v01, pdn_to_vlan_mappings_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_pdn_to_vlan_mappings_resp_msg_v01, pdn_to_vlan_mappings),
  QCMAP_MAX_NUM_BACKHAULS_V01,
  QMI_IDL_OFFSET8(qcmap_msgr_get_pdn_to_vlan_mappings_resp_msg_v01, pdn_to_vlan_mappings) - QMI_IDL_OFFSET8(qcmap_msgr_get_pdn_to_vlan_mappings_resp_msg_v01, pdn_to_vlan_mappings_len),
  QMI_IDL_TYPE88(0, 39)
};

static const uint8_t qcmap_msgr_enable_gsb_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_gsb_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_enable_gsb_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_enable_gsb_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_disable_gsb_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_gsb_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_disable_gsb_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_disable_gsb_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_get_gsb_config_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_gsb_config_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_gsb_config_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_gsb_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_gsb_config_resp_msg_v01, config) - QMI_IDL_OFFSET8(qcmap_msgr_get_gsb_config_resp_msg_v01, config_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_gsb_config_resp_msg_v01, config),
  QCMAP_MSGR_MAX_IF_SUPPORTED_V01,
  QMI_IDL_OFFSET8(qcmap_msgr_get_gsb_config_resp_msg_v01, config) - QMI_IDL_OFFSET8(qcmap_msgr_get_gsb_config_resp_msg_v01, config_len),
  QMI_IDL_TYPE88(0, 31)
};

static const uint8_t qcmap_msgr_set_gsb_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_gsb_config_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_gsb_config_req_msg_v01, config) - QMI_IDL_OFFSET8(qcmap_msgr_set_gsb_config_req_msg_v01, config_valid)),
  0x10,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_gsb_config_req_msg_v01, config),
  QMI_IDL_TYPE88(0, 31)
};

static const uint8_t qcmap_msgr_set_gsb_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_gsb_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_delete_gsb_config_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_gsb_config_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_gsb_config_req_msg_v01, if_name),
  QCMAP_MAX_IFACE_NAME_SIZE_V01
};

static const uint8_t qcmap_msgr_delete_gsb_config_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_delete_gsb_config_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_set_wwan_profile_preference_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_wwan_profile_preference_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_wwan_profile_preference_req_msg_v01, profile_handle)
};

static const uint8_t qcmap_msgr_set_wwan_profile_preference_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_wwan_profile_preference_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_set_dun_dongle_mode_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dun_dongle_mode_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dun_dongle_mode_req_msg_v01, enable_dun_dongle_mode)
};

static const uint8_t qcmap_msgr_set_dun_dongle_mode_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_dun_dongle_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_dun_dongle_mode_req_msg is empty
 * static const uint8_t qcmap_msgr_get_dun_dongle_mode_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_dun_dongle_mode_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dun_dongle_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_dun_dongle_mode_resp_msg_v01, dun_dongle_mode) - QMI_IDL_OFFSET8(qcmap_msgr_get_dun_dongle_mode_resp_msg_v01, dun_dongle_mode_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_dun_dongle_mode_resp_msg_v01, dun_dongle_mode)
};

static const uint8_t qcmap_msgr_set_data_path_opt_status_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_data_path_opt_status_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_data_path_opt_status_req_msg_v01, data_path_opt_status)
};

static const uint8_t qcmap_msgr_set_data_path_opt_status_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_data_path_opt_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_data_path_opt_status_req_msg is empty
 * static const uint8_t qcmap_msgr_get_data_path_opt_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_data_path_opt_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_data_path_opt_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_data_path_opt_status_resp_msg_v01, data_path_opt_status) - QMI_IDL_OFFSET8(qcmap_msgr_get_data_path_opt_status_resp_msg_v01, data_path_opt_status_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_data_path_opt_status_resp_msg_v01, data_path_opt_status)
};

static const uint8_t qcmap_msgr_get_pmip_mode_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_pmip_mode_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_pmip_mode_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_pmip_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  0x03,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_pmip_mode_resp_msg_v01, pmip_debug_mode),

  0x04,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_pmip_mode_resp_msg_v01, pmip_mode),

  0x05,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_pmip_mode_resp_msg_v01, pmip_mode_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_pmip_mode_resp_msg_v01, pmip_mobile_node_identifier_type) - QMI_IDL_OFFSET8(qcmap_msgr_get_pmip_mode_resp_msg_v01, pmip_mobile_node_identifier_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_pmip_mode_resp_msg_v01, pmip_mobile_node_identifier_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_pmip_mode_resp_msg_v01, pmip_service_selection_string) - QMI_IDL_OFFSET8(qcmap_msgr_get_pmip_mode_resp_msg_v01, pmip_service_selection_string_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qcmap_msgr_get_pmip_mode_resp_msg_v01, pmip_service_selection_string),
  QCMAP_MSGR_PMIP_SERVICE_SELECTION_STRING_LENGTH_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_get_pmip_mode_resp_msg_v01, pmip_mn_id_string) - QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_get_pmip_mode_resp_msg_v01, pmip_mn_id_string_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_get_pmip_mode_resp_msg_v01, pmip_mn_id_string),
  QCMAP_MSGR_PMIP_MN_ID_STRING_LENGTH_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_get_pmip_mode_resp_msg_v01, pmipv4_mode_type) - QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_get_pmip_mode_resp_msg_v01, pmipv4_mode_type_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_get_pmip_mode_resp_msg_v01, pmipv4_mode_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_get_pmip_mode_resp_msg_v01, lma_ipv4_addr) - QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_get_pmip_mode_resp_msg_v01, lma_ipv4_addr_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_get_pmip_mode_resp_msg_v01, lma_ipv4_addr),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_get_pmip_mode_resp_msg_v01, pmipv4_sec_router_param) - QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_get_pmip_mode_resp_msg_v01, pmipv4_sec_router_param_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_get_pmip_mode_resp_msg_v01, pmipv4_sec_router_param),
  QMI_IDL_TYPE88(0, 41),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_get_pmip_mode_resp_msg_v01, lma_ipv6_addr) - QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_get_pmip_mode_resp_msg_v01, lma_ipv6_addr_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_get_pmip_mode_resp_msg_v01, lma_ipv6_addr),
  QCMAP_MSGR_IPV6_ADDR_LEN_V01
};

static const uint8_t qcmap_msgr_set_pmip_mode_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_pmip_mode_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_pmip_mode_req_msg_v01, enable_pmip_debug_mode),

  0x03,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_pmip_mode_req_msg_v01, enable_pmip_mode),

  0x04,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_pmip_mode_req_msg_v01, pmip_mode_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_pmip_mode_req_msg_v01, pmip_mobile_node_identifier_type) - QMI_IDL_OFFSET8(qcmap_msgr_set_pmip_mode_req_msg_v01, pmip_mobile_node_identifier_type_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_pmip_mode_req_msg_v01, pmip_mobile_node_identifier_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_pmip_mode_req_msg_v01, pmip_service_selection_string) - QMI_IDL_OFFSET8(qcmap_msgr_set_pmip_mode_req_msg_v01, pmip_service_selection_string_valid)),
  0x11,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qcmap_msgr_set_pmip_mode_req_msg_v01, pmip_service_selection_string),
  QCMAP_MSGR_PMIP_SERVICE_SELECTION_STRING_LENGTH_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_set_pmip_mode_req_msg_v01, pmip_mn_id_string) - QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_set_pmip_mode_req_msg_v01, pmip_mn_id_string_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_STRING,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_set_pmip_mode_req_msg_v01, pmip_mn_id_string),
  QCMAP_MSGR_PMIP_MN_ID_STRING_LENGTH_V01,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_set_pmip_mode_req_msg_v01, pmipv4_mode_type) - QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_set_pmip_mode_req_msg_v01, pmipv4_mode_type_valid)),
  0x13,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_set_pmip_mode_req_msg_v01, pmipv4_mode_type),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_set_pmip_mode_req_msg_v01, lma_ipv4_addr) - QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_set_pmip_mode_req_msg_v01, lma_ipv4_addr_valid)),
  0x14,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_set_pmip_mode_req_msg_v01, lma_ipv4_addr),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_set_pmip_mode_req_msg_v01, pmipv4_sec_router_param) - QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_set_pmip_mode_req_msg_v01, pmipv4_sec_router_param_valid)),
  0x15,
   QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_set_pmip_mode_req_msg_v01, pmipv4_sec_router_param),
  QMI_IDL_TYPE88(0, 41),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_set_pmip_mode_req_msg_v01, lma_ipv6_addr) - QMI_IDL_OFFSET16RELATIVE(qcmap_msgr_set_pmip_mode_req_msg_v01, lma_ipv6_addr_valid)),
  0x16,
  QMI_IDL_FLAGS_IS_ARRAY |  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qcmap_msgr_set_pmip_mode_req_msg_v01, lma_ipv6_addr),
  QCMAP_MSGR_IPV6_ADDR_LEN_V01
};

static const uint8_t qcmap_msgr_set_pmip_mode_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_pmip_mode_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_backhaul_status_req_msg is empty
 * static const uint8_t qcmap_msgr_get_backhaul_status_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_backhaul_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_status_resp_msg_v01, backhaul_v4_status) - QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_status_resp_msg_v01, backhaul_v4_status_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_status_resp_msg_v01, backhaul_v4_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_status_resp_msg_v01, backhaul_v6_status) - QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_status_resp_msg_v01, backhaul_v6_status_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_status_resp_msg_v01, backhaul_v6_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_status_resp_msg_v01, backhaul_type) - QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_status_resp_msg_v01, backhaul_type_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_backhaul_status_resp_msg_v01, backhaul_type)
};

static const uint8_t qcmap_msgr_backhaul_status_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_backhaul_status_ind_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_backhaul_status_ind_msg_v01, backhaul_v4_status) - QMI_IDL_OFFSET8(qcmap_msgr_backhaul_status_ind_msg_v01, backhaul_v4_status_valid)),
  0x10,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_backhaul_status_ind_msg_v01, backhaul_v4_status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_backhaul_status_ind_msg_v01, backhaul_v6_status) - QMI_IDL_OFFSET8(qcmap_msgr_backhaul_status_ind_msg_v01, backhaul_v6_status_valid)),
  0x11,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_backhaul_status_ind_msg_v01, backhaul_v6_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_backhaul_status_ind_msg_v01, backhaul_type) - QMI_IDL_OFFSET8(qcmap_msgr_backhaul_status_ind_msg_v01, backhaul_type_valid)),
  0x12,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_backhaul_status_ind_msg_v01, backhaul_type)
};

static const uint8_t qcmap_msgr_get_wwan_roaming_status_req_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_roaming_status_req_msg_v01, mobile_ap_handle)
};

static const uint8_t qcmap_msgr_get_wwan_roaming_status_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_roaming_status_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_roaming_status_resp_msg_v01, wwan_roaming_status)
};

static const uint8_t qcmap_msgr_wwan_roaming_status_ind_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_roaming_status_ind_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_wwan_roaming_status_ind_msg_v01, wwan_roaming_status)
};

/*
 * qcmap_msgr_get_lan_bridges_req_msg is empty
 * static const uint8_t qcmap_msgr_get_lan_bridges_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_lan_bridges_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_lan_bridges_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_lan_bridges_resp_msg_v01, bridge_list),
  QMI_IDL_TYPE88(0, 42)
};

static const uint8_t qcmap_msgr_select_lan_bridge_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_select_lan_bridge_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_select_lan_bridge_req_msg_v01, bridge_vlan_id)
};

static const uint8_t qcmap_msgr_select_lan_bridge_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_select_lan_bridge_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

static const uint8_t qcmap_msgr_set_always_on_wlan_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_always_on_wlan_req_msg_v01, mobile_ap_handle),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_always_on_wlan_req_msg_v01, enable_always_on_wlan)
};

static const uint8_t qcmap_msgr_set_always_on_wlan_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_always_on_wlan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_always_on_wlan_req_msg is empty
 * static const uint8_t qcmap_msgr_get_always_on_wlan_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_always_on_wlan_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_always_on_wlan_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_always_on_wlan_resp_msg_v01, always_on_wlan_status)
};

static const uint8_t qcmap_msgr_set_p2p_role_req_msg_data_v01[] = {
  0x01,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_p2p_role_req_msg_v01, mobile_ap_handle),

  0x02,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_p2p_role_req_msg_v01, p2p_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_set_p2p_role_req_msg_v01, p2p_role) - QMI_IDL_OFFSET8(qcmap_msgr_set_p2p_role_req_msg_v01, p2p_role_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_p2p_role_req_msg_v01, p2p_role)
};

static const uint8_t qcmap_msgr_set_p2p_role_resp_msg_data_v01[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_set_p2p_role_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0)
};

/*
 * qcmap_msgr_get_p2p_role_req_msg is empty
 * static const uint8_t qcmap_msgr_get_p2p_role_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_p2p_role_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_p2p_role_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  0x03,
   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_p2p_role_resp_msg_v01, p2p_status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qcmap_msgr_get_p2p_role_resp_msg_v01, p2p_role) - QMI_IDL_OFFSET8(qcmap_msgr_get_p2p_role_resp_msg_v01, p2p_role_valid)),
  0x10,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_p2p_role_resp_msg_v01, p2p_role)
};

/*
 * qcmap_msgr_get_wwan_profile_preference_req_msg is empty
 * static const uint8_t qcmap_msgr_get_wwan_profile_preference_req_msg_data_v01[] = {
 * };
 */

static const uint8_t qcmap_msgr_get_wwan_profile_preference_resp_msg_data_v01[] = {
  0x02,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_profile_preference_resp_msg_v01, resp),
  QMI_IDL_TYPE88(1, 0),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
   QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qcmap_msgr_get_wwan_profile_preference_resp_msg_v01, current_profile_handle)
};

/* Type Table */
static const qmi_idl_type_table_entry  qcmap_msgr_type_table_v01[] = {
  {sizeof(qcmap_msgr_wlan_if_info_v01), qcmap_msgr_wlan_if_info_data_v01},
  {sizeof(qcmap_msgr_snat_entry_config_v01), qcmap_msgr_snat_entry_config_data_v01},
  {sizeof(qcmap_msgr_net_policy_info_v01), qcmap_msgr_net_policy_info_data_v01},
  {sizeof(qcmap_msgr_net_profile_and_policy_info_v01), qcmap_msgr_net_profile_and_policy_info_data_v01},
  {sizeof(qcmap_msgr_wwan_statistics_type_v01), qcmap_msgr_wwan_statistics_type_data_v01},
  {sizeof(qcmap_ip4_addr_subnet_mask_v01), qcmap_ip4_addr_subnet_mask_data_v01},
  {sizeof(qcmap_ip6_addr_prefix_len_v01), qcmap_ip6_addr_prefix_len_data_v01},
  {sizeof(qcmap_tcp_udp_port_range_v01), qcmap_tcp_udp_port_range_data_v01},
  {sizeof(qcmap_ip4_tos_v01), qcmap_ip4_tos_data_v01},
  {sizeof(qcmap_ip6_traffic_class_v01), qcmap_ip6_traffic_class_data_v01},
  {sizeof(qcmap_msgr_dhcp_config_v01), qcmap_msgr_dhcp_config_data_v01},
  {sizeof(qcmap_msgr_ip_passthrough_config_v01), qcmap_msgr_ip_passthrough_config_data_v01},
  {sizeof(qcmap_msgr_lan_config_v01), qcmap_msgr_lan_config_data_v01},
  {sizeof(qcmap_msgr_wlan_config_v01), qcmap_msgr_wlan_config_data_v01},
  {sizeof(qcmap_msgr_usb_config_v01), qcmap_msgr_usb_config_data_v01},
  {sizeof(qcmap_msgr_ap_mode_wlan_usb_ip_config_v01), qcmap_msgr_ap_mode_wlan_usb_ip_config_data_v01},
  {sizeof(qcmap_msgr_sta_static_ip_config_v01), qcmap_msgr_sta_static_ip_config_data_v01},
  {sizeof(qcmap_msgr_station_mode_config_v01), qcmap_msgr_station_mode_config_data_v01},
  {sizeof(qcmap_msgr_wwan_call_end_reason_v01), qcmap_msgr_wwan_call_end_reason_data_v01},
  {sizeof(qcmap_msgr_data_bitrate_v01), qcmap_msgr_data_bitrate_data_v01},
  {sizeof(qcmap_msgr_dhcp_reservation_v01), qcmap_msgr_dhcp_reservation_data_v01},
  {sizeof(qcmap_msgr_sip_server_info_v01), qcmap_msgr_sip_server_info_data_v01},
  {sizeof(qcmap_msgr_client_ipv6_addr_v01), qcmap_msgr_client_ipv6_addr_data_v01},
  {sizeof(qcmap_msgr_connected_device_info_v01), qcmap_msgr_connected_device_info_data_v01},
  {sizeof(qcmap_msgr_ipv6_sip_server_info_v01), qcmap_msgr_ipv6_sip_server_info_data_v01},
  {sizeof(qcmap_msgr_ddns_server_config_v01), qcmap_msgr_ddns_server_config_data_v01},
  {sizeof(qcmap_msgr_wlan_iface_state_v01), qcmap_msgr_wlan_iface_state_data_v01},
  {0, 0},
  {sizeof(qcmap_msgr_socksv5_wan_config_v01), qcmap_msgr_socksv5_wan_config_data_v01},
  {sizeof(qcmap_msgr_socksv5_uname_assoc_v01), qcmap_msgr_socksv5_uname_assoc_data_v01},
  {sizeof(qcmap_msgr_socksv5_config_file_paths_v01), qcmap_msgr_socksv5_config_file_paths_data_v01},
  {sizeof(qcmap_msgr_gsb_config_v01), qcmap_msgr_gsb_config_data_v01},
  {sizeof(qcmap_msgr_vlan_config_v01), qcmap_msgr_vlan_config_data_v01},
  {sizeof(qcmap_msgr_set_unmanaged_l2tp_state_config_v01), qcmap_msgr_set_unmanaged_l2tp_state_config_data_v01},
  {sizeof(qcmap_msgr_l2tp_session_config_v01), qcmap_msgr_l2tp_session_config_data_v01},
  {sizeof(qcmap_msgr_l2tp_config_v01), qcmap_msgr_l2tp_config_data_v01},
  {sizeof(qcmap_msgr_l2tp_mtu_config_v01), qcmap_msgr_l2tp_mtu_config_data_v01},
  {sizeof(qcmap_msgr_l2tp_TCP_MSS_config_v01), qcmap_msgr_l2tp_TCP_MSS_config_data_v01},
  {sizeof(qcmap_msgr_delete_l2tp_config_v01), qcmap_msgr_delete_l2tp_config_data_v01},
  {sizeof(qcmap_msgr_pdn_to_vlan_mapping_v01), qcmap_msgr_pdn_to_vlan_mapping_data_v01},
  {sizeof(qcmap_msgr_wwan_info_v01), qcmap_msgr_wwan_info_data_v01},
  {sizeof(qcmap_pmipv4_sec_router_param_v01), qcmap_pmipv4_sec_router_param_data_v01},
  {sizeof(qcmap_msgr_bridge_list_v01), qcmap_msgr_bridge_list_data_v01}
};

/* Message Table */
static const qmi_idl_message_table_entry qcmap_msgr_message_table_v01[] = {
  {sizeof(qcmap_msgr_indication_register_req_msg_v01), qcmap_msgr_indication_register_req_msg_data_v01},
  {sizeof(qcmap_msgr_indication_register_resp_msg_v01), qcmap_msgr_indication_register_resp_msg_data_v01},
  {sizeof(qcmap_msgr_mobile_ap_enable_req_msg_v01), 0},
  {sizeof(qcmap_msgr_mobile_ap_enable_resp_msg_v01), qcmap_msgr_mobile_ap_enable_resp_msg_data_v01},
  {sizeof(qcmap_msgr_mobile_ap_disable_req_msg_v01), qcmap_msgr_mobile_ap_disable_req_msg_data_v01},
  {sizeof(qcmap_msgr_mobile_ap_disable_resp_msg_v01), qcmap_msgr_mobile_ap_disable_resp_msg_data_v01},
  {sizeof(qcmap_msgr_enable_ipv4_req_msg_v01), qcmap_msgr_enable_ipv4_req_msg_data_v01},
  {sizeof(qcmap_msgr_enable_ipv4_resp_msg_v01), qcmap_msgr_enable_ipv4_resp_msg_data_v01},
  {sizeof(qcmap_msgr_disable_ipv4_req_msg_v01), qcmap_msgr_disable_ipv4_req_msg_data_v01},
  {sizeof(qcmap_msgr_disable_ipv4_resp_msg_v01), qcmap_msgr_disable_ipv4_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_ipv4_state_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_ipv4_state_resp_msg_v01), qcmap_msgr_get_ipv4_state_resp_msg_data_v01},
  {sizeof(qcmap_msgr_enable_ipv6_req_msg_v01), qcmap_msgr_enable_ipv6_req_msg_data_v01},
  {sizeof(qcmap_msgr_enable_ipv6_resp_msg_v01), qcmap_msgr_enable_ipv6_resp_msg_data_v01},
  {sizeof(qcmap_msgr_disable_ipv6_req_msg_v01), qcmap_msgr_disable_ipv6_req_msg_data_v01},
  {sizeof(qcmap_msgr_disable_ipv6_resp_msg_v01), qcmap_msgr_disable_ipv6_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_ipv6_state_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_ipv6_state_resp_msg_v01), qcmap_msgr_get_ipv6_state_resp_msg_data_v01},
  {sizeof(qcmap_msgr_bring_up_wwan_req_msg_v01), qcmap_msgr_bring_up_wwan_req_msg_data_v01},
  {sizeof(qcmap_msgr_bring_up_wwan_resp_msg_v01), qcmap_msgr_bring_up_wwan_resp_msg_data_v01},
  {sizeof(qcmap_msgr_bring_up_wwan_ind_msg_v01), qcmap_msgr_bring_up_wwan_ind_msg_data_v01},
  {sizeof(qcmap_msgr_tear_down_wwan_req_msg_v01), qcmap_msgr_tear_down_wwan_req_msg_data_v01},
  {sizeof(qcmap_msgr_tear_down_wwan_resp_msg_v01), qcmap_msgr_tear_down_wwan_resp_msg_data_v01},
  {sizeof(qcmap_msgr_tear_down_wwan_ind_msg_v01), qcmap_msgr_tear_down_wwan_ind_msg_data_v01},
  {sizeof(qcmap_msgr_wwan_status_ind_register_req_msg_v01), qcmap_msgr_wwan_status_ind_register_req_msg_data_v01},
  {sizeof(qcmap_msgr_wwan_status_ind_register_resp_msg_v01), qcmap_msgr_wwan_status_ind_register_resp_msg_data_v01},
  {sizeof(qcmap_msgr_wwan_status_ind_msg_v01), qcmap_msgr_wwan_status_ind_msg_data_v01},
  {sizeof(qcmap_msgr_mobile_ap_status_ind_register_req_msg_v01), qcmap_msgr_mobile_ap_status_ind_register_req_msg_data_v01},
  {sizeof(qcmap_msgr_mobile_ap_status_ind_register_resp_msg_v01), qcmap_msgr_mobile_ap_status_ind_register_resp_msg_data_v01},
  {sizeof(qcmap_msgr_mobile_ap_status_ind_msg_v01), qcmap_msgr_mobile_ap_status_ind_msg_data_v01},
  {sizeof(qcmap_msgr_station_mode_status_ind_register_req_msg_v01), qcmap_msgr_station_mode_status_ind_register_req_msg_data_v01},
  {sizeof(qcmap_msgr_station_mode_status_ind_register_resp_msg_v01), qcmap_msgr_station_mode_status_ind_register_resp_msg_data_v01},
  {sizeof(qcmap_msgr_station_mode_status_ind_msg_v01), qcmap_msgr_station_mode_status_ind_msg_data_v01},
  {sizeof(qcmap_msgr_enable_wlan_req_msg_v01), qcmap_msgr_enable_wlan_req_msg_data_v01},
  {sizeof(qcmap_msgr_enable_wlan_resp_msg_v01), qcmap_msgr_enable_wlan_resp_msg_data_v01},
  {sizeof(qcmap_msgr_disable_wlan_req_msg_v01), qcmap_msgr_disable_wlan_req_msg_data_v01},
  {sizeof(qcmap_msgr_disable_wlan_resp_msg_v01), qcmap_msgr_disable_wlan_resp_msg_data_v01},
  {sizeof(qcmap_msgr_add_static_nat_entry_req_msg_v01), qcmap_msgr_add_static_nat_entry_req_msg_data_v01},
  {sizeof(qcmap_msgr_add_static_nat_entry_resp_msg_v01), qcmap_msgr_add_static_nat_entry_resp_msg_data_v01},
  {sizeof(qcmap_msgr_delete_static_nat_entry_req_msg_v01), qcmap_msgr_delete_static_nat_entry_req_msg_data_v01},
  {sizeof(qcmap_msgr_delete_static_nat_entry_resp_msg_v01), qcmap_msgr_delete_static_nat_entry_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_static_nat_entries_req_msg_v01), qcmap_msgr_get_static_nat_entries_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_static_nat_entries_resp_msg_v01), qcmap_msgr_get_static_nat_entries_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_dmz_req_msg_v01), qcmap_msgr_set_dmz_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_dmz_resp_msg_v01), qcmap_msgr_set_dmz_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_dmz_req_msg_v01), qcmap_msgr_get_dmz_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_dmz_resp_msg_v01), qcmap_msgr_get_dmz_resp_msg_data_v01},
  {sizeof(qcmap_msgr_delete_dmz_req_msg_v01), qcmap_msgr_delete_dmz_req_msg_data_v01},
  {sizeof(qcmap_msgr_delete_dmz_resp_msg_v01), qcmap_msgr_delete_dmz_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_wwan_stats_req_msg_v01), qcmap_msgr_get_wwan_stats_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_wwan_stats_resp_msg_v01), qcmap_msgr_get_wwan_stats_resp_msg_data_v01},
  {sizeof(qcmap_msgr_reset_wwan_stats_req_msg_v01), qcmap_msgr_reset_wwan_stats_req_msg_data_v01},
  {sizeof(qcmap_msgr_reset_wwan_stats_resp_msg_v01), qcmap_msgr_reset_wwan_stats_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_ipsec_vpn_pass_through_req_msg_v01), qcmap_msgr_set_ipsec_vpn_pass_through_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_ipsec_vpn_pass_through_resp_msg_v01), qcmap_msgr_set_ipsec_vpn_pass_through_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_ipsec_vpn_pass_through_req_msg_v01), qcmap_msgr_get_ipsec_vpn_pass_through_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_ipsec_vpn_pass_through_resp_msg_v01), qcmap_msgr_get_ipsec_vpn_pass_through_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_pptp_vpn_pass_through_req_msg_v01), qcmap_msgr_set_pptp_vpn_pass_through_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_pptp_vpn_pass_through_resp_msg_v01), qcmap_msgr_set_pptp_vpn_pass_through_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_pptp_vpn_pass_through_req_msg_v01), qcmap_msgr_get_pptp_vpn_pass_through_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_pptp_vpn_pass_through_resp_msg_v01), qcmap_msgr_get_pptp_vpn_pass_through_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_l2tp_vpn_pass_through_req_msg_v01), qcmap_msgr_set_l2tp_vpn_pass_through_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_l2tp_vpn_pass_through_resp_msg_v01), qcmap_msgr_set_l2tp_vpn_pass_through_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_l2tp_vpn_pass_through_req_msg_v01), qcmap_msgr_get_l2tp_vpn_pass_through_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_l2tp_vpn_pass_through_resp_msg_v01), qcmap_msgr_get_l2tp_vpn_pass_through_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_nat_type_req_msg_v01), qcmap_msgr_set_nat_type_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_nat_type_resp_msg_v01), qcmap_msgr_set_nat_type_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_nat_type_req_msg_v01), qcmap_msgr_get_nat_type_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_nat_type_resp_msg_v01), qcmap_msgr_get_nat_type_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_auto_connect_req_msg_v01), qcmap_msgr_set_auto_connect_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_auto_connect_resp_msg_v01), qcmap_msgr_set_auto_connect_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_auto_connect_req_msg_v01), qcmap_msgr_get_auto_connect_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_auto_connect_resp_msg_v01), qcmap_msgr_get_auto_connect_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_active_wlan_if_info_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_active_wlan_if_info_resp_msg_v01), qcmap_msgr_get_active_wlan_if_info_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_roaming_pref_req_msg_v01), qcmap_msgr_set_roaming_pref_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_roaming_pref_resp_msg_v01), qcmap_msgr_set_roaming_pref_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_roaming_pref_req_msg_v01), qcmap_msgr_get_roaming_pref_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_roaming_pref_resp_msg_v01), qcmap_msgr_get_roaming_pref_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_loopback_flag_req_msg_v01), qcmap_msgr_set_loopback_flag_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_loopback_flag_resp_msg_v01), qcmap_msgr_set_loopback_flag_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_loopback_flag_req_msg_v01), qcmap_msgr_get_loopback_flag_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_loopback_flag_resp_msg_v01), qcmap_msgr_get_loopback_flag_resp_msg_data_v01},
  {sizeof(qcmap_msgr_add_firewall_entry_req_msg_v01), qcmap_msgr_add_firewall_entry_req_msg_data_v01},
  {sizeof(qcmap_msgr_add_firewall_entry_resp_msg_v01), qcmap_msgr_add_firewall_entry_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_firewall_entries_handle_list_req_msg_v01), qcmap_msgr_get_firewall_entries_handle_list_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_firewall_entries_handle_list_resp_msg_v01), qcmap_msgr_get_firewall_entries_handle_list_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_firewall_entry_req_msg_v01), qcmap_msgr_get_firewall_entry_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_firewall_entry_resp_msg_v01), qcmap_msgr_get_firewall_entry_resp_msg_data_v01},
  {sizeof(qcmap_msgr_delete_firewall_entry_req_msg_v01), qcmap_msgr_delete_firewall_entry_req_msg_data_v01},
  {sizeof(qcmap_msgr_delete_firewall_entry_resp_msg_v01), qcmap_msgr_delete_firewall_entry_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_wwan_config_req_msg_v01), qcmap_msgr_get_wwan_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_wwan_config_resp_msg_v01), qcmap_msgr_get_wwan_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_mobile_ap_status_req_v01), qcmap_msgr_mobile_ap_status_req_data_v01},
  {sizeof(qcmap_msgr_mobile_ap_status_resp_v01), qcmap_msgr_mobile_ap_status_resp_data_v01},
  {sizeof(qcmap_msgr_wwan_status_req_msg_v01), qcmap_msgr_wwan_status_req_msg_data_v01},
  {sizeof(qcmap_msgr_wwan_status_resp_msg_v01), qcmap_msgr_wwan_status_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_station_mode_status_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_station_mode_status_resp_msg_v01), qcmap_msgr_get_station_mode_status_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_nat_timeout_req_msg_v01), qcmap_msgr_set_nat_timeout_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_nat_timeout_resp_msg_v01), qcmap_msgr_set_nat_timeout_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_nat_timeout_req_msg_v01), qcmap_msgr_get_nat_timeout_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_nat_timeout_resp_msg_v01), qcmap_msgr_get_nat_timeout_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_lan_config_req_msg_v01), qcmap_msgr_set_lan_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_lan_config_resp_msg_v01), qcmap_msgr_set_lan_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_wlan_config_req_msg_v01), qcmap_msgr_set_wlan_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_wlan_config_resp_msg_v01), qcmap_msgr_set_wlan_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_wlan_status_req_msg_v01), qcmap_msgr_get_wlan_status_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_wlan_status_resp_msg_v01), qcmap_msgr_get_wlan_status_resp_msg_data_v01},
  {sizeof(qcmap_msgr_activate_wlan_req_msg_v01), qcmap_msgr_activate_wlan_req_msg_data_v01},
  {sizeof(qcmap_msgr_activate_wlan_resp_msg_v01), qcmap_msgr_activate_wlan_resp_msg_data_v01},
  {sizeof(qcmap_msgr_activate_lan_req_msg_v01), qcmap_msgr_activate_lan_req_msg_data_v01},
  {sizeof(qcmap_msgr_activate_lan_resp_msg_v01), qcmap_msgr_activate_lan_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_lan_config_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_lan_config_resp_msg_v01), qcmap_msgr_get_lan_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_wlan_config_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_wlan_config_resp_msg_v01), qcmap_msgr_get_wlan_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_activate_hostapd_config_req_msg_v01), qcmap_msgr_activate_hostapd_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_activate_hostapd_config_resp_msg_v01), qcmap_msgr_activate_hostapd_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_activate_supplicant_config_req_msg_v01), qcmap_msgr_activate_supplicant_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_activate_supplicant_config_resp_msg_v01), qcmap_msgr_activate_supplicant_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_usb_link_up_req_msg_v01), qcmap_msgr_usb_link_up_req_msg_data_v01},
  {sizeof(qcmap_msgr_usb_link_up_resp_msg_v01), qcmap_msgr_usb_link_up_resp_msg_data_v01},
  {sizeof(qcmap_msgr_usb_link_down_req_msg_v01), qcmap_msgr_usb_link_down_req_msg_data_v01},
  {sizeof(qcmap_msgr_usb_link_down_resp_msg_v01), qcmap_msgr_usb_link_down_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_wwan_policy_req_msg_v01), qcmap_msgr_set_wwan_policy_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_wwan_policy_resp_msg_v01), qcmap_msgr_set_wwan_policy_resp_msg_data_v01},
  {sizeof(qcmap_msgr_create_wwan_policy_req_msg_v01), qcmap_msgr_create_wwan_policy_req_msg_data_v01},
  {sizeof(qcmap_msgr_create_wwan_policy_resp_msg_v01), qcmap_msgr_create_wwan_policy_resp_msg_data_v01},
  {sizeof(qcmap_msgr_update_wwan_policy_req_msg_v01), qcmap_msgr_update_wwan_policy_req_msg_data_v01},
  {sizeof(qcmap_msgr_update_wwan_policy_resp_msg_v01), qcmap_msgr_update_wwan_policy_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_wwan_policy_req_msg_v01), qcmap_msgr_get_wwan_policy_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_wwan_policy_resp_msg_v01), qcmap_msgr_get_wwan_policy_resp_msg_data_v01},
  {sizeof(qcmap_msgr_enable_upnp_req_msg_v01), 0},
  {sizeof(qcmap_msgr_enable_upnp_resp_msg_v01), qcmap_msgr_enable_upnp_resp_msg_data_v01},
  {sizeof(qcmap_msgr_disable_upnp_req_msg_v01), 0},
  {sizeof(qcmap_msgr_disable_upnp_resp_msg_v01), qcmap_msgr_disable_upnp_resp_msg_data_v01},
  {sizeof(qcmap_msgr_enable_dlna_req_msg_v01), 0},
  {sizeof(qcmap_msgr_enable_dlna_resp_msg_v01), qcmap_msgr_enable_dlna_resp_msg_data_v01},
  {sizeof(qcmap_msgr_disable_dlna_req_msg_v01), 0},
  {sizeof(qcmap_msgr_disable_dlna_resp_msg_v01), qcmap_msgr_disable_dlna_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_firewall_config_req_msg_v01), qcmap_msgr_set_firewall_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_firewall_config_resp_msg_v01), qcmap_msgr_set_firewall_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_firewall_config_req_msg_v01), qcmap_msgr_get_firewall_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_firewall_config_resp_msg_v01), qcmap_msgr_get_firewall_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_enable_multicast_dns_req_msg_v01), 0},
  {sizeof(qcmap_msgr_enable_multicast_dns_resp_msg_v01), qcmap_msgr_enable_multicast_dns_resp_msg_data_v01},
  {sizeof(qcmap_msgr_disable_multicast_dns_req_msg_v01), 0},
  {sizeof(qcmap_msgr_disable_multicast_dns_resp_msg_v01), qcmap_msgr_disable_multicast_dns_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_upnp_status_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_upnp_status_resp_msg_v01), qcmap_msgr_get_upnp_status_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_dlna_status_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_dlna_status_resp_msg_v01), qcmap_msgr_get_dlna_status_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_multicast_dns_status_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_multicast_dns_status_resp_msg_v01), qcmap_msgr_get_multicast_dns_status_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_qcmap_bootup_cfg_req_msg_v01), qcmap_msgr_set_qcmap_bootup_cfg_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_qcmap_bootup_cfg_resp_msg_v01), qcmap_msgr_set_qcmap_bootup_cfg_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_qcmap_bootup_cfg_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_v01), qcmap_msgr_get_qcmap_bootup_cfg_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_dlna_media_dir_req_msg_v01), qcmap_msgr_set_dlna_media_dir_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_dlna_media_dir_resp_msg_v01), qcmap_msgr_set_dlna_media_dir_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_dlna_media_dir_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_dlna_media_dir_resp_msg_v01), qcmap_msgr_get_dlna_media_dir_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_data_bitrate_req_msg_v01), qcmap_msgr_get_data_bitrate_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_data_bitrate_resp_msg_v01), qcmap_msgr_get_data_bitrate_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_upnp_notify_interval_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_upnp_notify_interval_resp_msg_v01), qcmap_msgr_get_upnp_notify_interval_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_upnp_notify_interval_req_msg_v01), qcmap_msgr_set_upnp_notify_interval_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_upnp_notify_interval_resp_msg_v01), qcmap_msgr_set_upnp_notify_interval_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_dlna_notify_interval_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_dlna_notify_interval_resp_msg_v01), qcmap_msgr_get_dlna_notify_interval_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_dlna_notify_interval_req_msg_v01), qcmap_msgr_set_dlna_notify_interval_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_dlna_notify_interval_resp_msg_v01), qcmap_msgr_set_dlna_notify_interval_resp_msg_data_v01},
  {sizeof(qcmap_msgr_add_dhcp_reservation_record_req_msg_v01), qcmap_msgr_add_dhcp_reservation_record_req_msg_data_v01},
  {sizeof(qcmap_msgr_add_dhcp_reservation_record_resp_msg_v01), qcmap_msgr_add_dhcp_reservation_record_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_dhcp_reservation_records_req_msg_v01), qcmap_msgr_get_dhcp_reservation_records_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_dhcp_reservation_records_resp_msg_v01), qcmap_msgr_get_dhcp_reservation_records_resp_msg_data_v01},
  {sizeof(qcmap_msgr_edit_dhcp_reservation_record_req_msg_v01), qcmap_msgr_edit_dhcp_reservation_record_req_msg_data_v01},
  {sizeof(qcmap_msgr_edit_dhcp_reservation_record_resp_msg_v01), qcmap_msgr_edit_dhcp_reservation_record_resp_msg_data_v01},
  {sizeof(qcmap_msgr_delete_dhcp_reservation_record_req_msg_v01), qcmap_msgr_delete_dhcp_reservation_record_req_msg_data_v01},
  {sizeof(qcmap_msgr_delete_dhcp_reservation_record_resp_msg_v01), qcmap_msgr_delete_dhcp_reservation_record_resp_msg_data_v01},
  {sizeof(qcmap_msgr_enable_alg_req_msg_v01), qcmap_msgr_enable_alg_req_msg_data_v01},
  {sizeof(qcmap_msgr_enable_alg_resp_msg_v01), qcmap_msgr_enable_alg_resp_msg_data_v01},
  {sizeof(qcmap_msgr_disable_alg_req_msg_v01), qcmap_msgr_disable_alg_req_msg_data_v01},
  {sizeof(qcmap_msgr_disable_alg_resp_msg_v01), qcmap_msgr_disable_alg_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_webserver_wwan_access_req_msg_v01), qcmap_msgr_set_webserver_wwan_access_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_webserver_wwan_access_resp_msg_v01), qcmap_msgr_set_webserver_wwan_access_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_webserver_wwan_access_req_msg_v01), qcmap_msgr_get_webserver_wwan_access_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_webserver_wwan_access_resp_msg_v01), qcmap_msgr_get_webserver_wwan_access_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_sip_server_info_req_msg_v01), qcmap_msgr_set_sip_server_info_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_sip_server_info_resp_msg_v01), qcmap_msgr_set_sip_server_info_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_sip_server_info_req_msg_v01), qcmap_msgr_get_sip_server_info_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_sip_server_info_resp_msg_v01), qcmap_msgr_get_sip_server_info_resp_msg_data_v01},
  {sizeof(qcmap_msgr_restore_factory_config_req_msg_v01), qcmap_msgr_restore_factory_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_restore_factory_config_resp_msg_v01), qcmap_msgr_restore_factory_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_odu_mode_req_msg_v01), qcmap_msgr_set_odu_mode_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_odu_mode_resp_msg_v01), qcmap_msgr_set_odu_mode_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_odu_mode_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_odu_mode_resp_msg_v01), qcmap_msgr_get_odu_mode_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_odu_pswd_req_msg_v01), qcmap_msgr_set_odu_pswd_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_odu_pswd_resp_msg_v01), qcmap_msgr_set_odu_pswd_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_connected_devices_info_req_msg_v01), qcmap_msgr_get_connected_devices_info_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_connected_devices_info_resp_msg_v01), qcmap_msgr_get_connected_devices_info_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_ipv6_sip_server_info_req_msg_v01), qcmap_msgr_get_ipv6_sip_server_info_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_ipv6_sip_server_info_resp_msg_v01), qcmap_msgr_get_ipv6_sip_server_info_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_supplicant_config_req_msg_v01), qcmap_msgr_set_supplicant_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_supplicant_config_resp_msg_v01), qcmap_msgr_set_supplicant_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_cradle_mode_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_cradle_mode_resp_msg_v01), qcmap_msgr_get_cradle_mode_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_cradle_mode_req_msg_v01), qcmap_msgr_set_cradle_mode_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_cradle_mode_resp_msg_v01), qcmap_msgr_set_cradle_mode_resp_msg_data_v01},
  {sizeof(qcmap_msgr_cradle_mode_status_ind_msg_v01), qcmap_msgr_cradle_mode_status_ind_msg_data_v01},
  {sizeof(qcmap_msgr_get_prefix_delegation_config_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_prefix_delegation_config_resp_msg_v01), qcmap_msgr_get_prefix_delegation_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_prefix_delegation_config_req_msg_v01), qcmap_msgr_set_prefix_delegation_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_prefix_delegation_config_resp_msg_v01), qcmap_msgr_set_prefix_delegation_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_prefix_delegation_status_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_prefix_delegation_status_resp_msg_v01), qcmap_msgr_get_prefix_delegation_status_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_gateway_url_req_msg_v01), qcmap_msgr_set_gateway_url_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_gateway_url_resp_msg_v01), qcmap_msgr_set_gateway_url_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_gateway_url_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_gateway_url_resp_msg_v01), qcmap_msgr_get_gateway_url_resp_msg_data_v01},
  {sizeof(qcmap_msgr_enable_dynamic_dns_req_msg_v01), qcmap_msgr_enable_dynamic_dns_req_msg_data_v01},
  {sizeof(qcmap_msgr_enable_dynamic_dns_resp_msg_v01), qcmap_msgr_enable_dynamic_dns_resp_msg_data_v01},
  {sizeof(qcmap_msgr_disable_dynamic_dns_req_msg_v01), qcmap_msgr_disable_dynamic_dns_req_msg_data_v01},
  {sizeof(qcmap_msgr_disable_dynamic_dns_resp_msg_v01), qcmap_msgr_disable_dynamic_dns_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_dynamic_dns_config_req_msg_v01), qcmap_msgr_set_dynamic_dns_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_dynamic_dns_config_resp_msg_v01), qcmap_msgr_set_dynamic_dns_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_dynamic_dns_config_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01), qcmap_msgr_get_dynamic_dns_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_tiny_proxy_status_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_tiny_proxy_status_resp_msg_v01), qcmap_msgr_get_tiny_proxy_status_resp_msg_data_v01},
  {sizeof(qcmap_msgr_enable_tiny_proxy_req_msg_v01), qcmap_msgr_enable_tiny_proxy_req_msg_data_v01},
  {sizeof(qcmap_msgr_enable_tiny_proxy_resp_msg_v01), qcmap_msgr_enable_tiny_proxy_resp_msg_data_v01},
  {sizeof(qcmap_msgr_disable_tiny_proxy_req_msg_v01), qcmap_msgr_disable_tiny_proxy_req_msg_data_v01},
  {sizeof(qcmap_msgr_disable_tiny_proxy_resp_msg_v01), qcmap_msgr_disable_tiny_proxy_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_dlna_whitelisting_req_msg_v01), qcmap_msgr_set_dlna_whitelisting_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_dlna_whitelisting_resp_msg_v01), qcmap_msgr_set_dlna_whitelisting_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_dlna_whitelisting_req_msg_v01), qcmap_msgr_get_dlna_whitelisting_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_dlna_whitelisting_resp_msg_v01), qcmap_msgr_get_dlna_whitelisting_resp_msg_data_v01},
  {sizeof(qcmap_msgr_add_dlna_whitelistip_req_msg_v01), qcmap_msgr_add_dlna_whitelistip_req_msg_data_v01},
  {sizeof(qcmap_msgr_add_dlna_whitelistip_resp_msg_v01), qcmap_msgr_add_dlna_whitelistip_resp_msg_data_v01},
  {sizeof(qcmap_msgr_delete_dlna_whitelist_ip_req_msg_v01), qcmap_msgr_delete_dlna_whitelist_ip_req_msg_data_v01},
  {sizeof(qcmap_msgr_delete_dlna_whitelist_ip_resp_msg_v01), qcmap_msgr_delete_dlna_whitelist_ip_resp_msg_data_v01},
  {sizeof(qcmap_msgr_enable_sta_mode_req_msg_v01), qcmap_msgr_enable_sta_mode_req_msg_data_v01},
  {sizeof(qcmap_msgr_enable_sta_mode_resp_msg_v01), qcmap_msgr_enable_sta_mode_resp_msg_data_v01},
  {sizeof(qcmap_msgr_disable_sta_mode_req_msg_v01), qcmap_msgr_disable_sta_mode_req_msg_data_v01},
  {sizeof(qcmap_msgr_disable_sta_mode_resp_msg_v01), qcmap_msgr_disable_sta_mode_resp_msg_data_v01},
  {sizeof(qcmap_msgr_wlan_status_ind_register_req_msg_v01), qcmap_msgr_wlan_status_ind_register_req_msg_data_v01},
  {sizeof(qcmap_msgr_wlan_status_ind_register_resp_msg_v01), qcmap_msgr_wlan_status_ind_register_resp_msg_data_v01},
  {sizeof(qcmap_msgr_wlan_status_ind_msg_v01), qcmap_msgr_wlan_status_ind_msg_data_v01},
  {sizeof(qcmap_msgr_set_backhaul_pref_req_msg_v01), qcmap_msgr_set_backhaul_pref_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_backhaul_pref_resp_msg_v01), qcmap_msgr_set_backhaul_pref_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_backhaul_pref_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_backhaul_pref_resp_msg_v01), qcmap_msgr_get_backhaul_pref_resp_msg_data_v01},
  {sizeof(qcmap_msgr_cradle_mode_status_ind_register_req_msg_v01), qcmap_msgr_cradle_mode_status_ind_register_req_msg_data_v01},
  {sizeof(qcmap_msgr_cradle_mode_status_ind_register_resp_msg_v01), qcmap_msgr_cradle_mode_status_ind_register_resp_msg_data_v01},
  {sizeof(qcmap_msgr_ethernet_mode_status_ind_register_req_msg_v01), qcmap_msgr_ethernet_mode_status_ind_register_req_msg_data_v01},
  {sizeof(qcmap_msgr_ethernet_mode_status_ind_register_resp_msg_v01), qcmap_msgr_ethernet_mode_status_ind_register_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_ethernet_mode_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_ethernet_mode_resp_msg_v01), qcmap_msgr_get_ethernet_mode_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_ethernet_mode_req_msg_v01), qcmap_msgr_set_ethernet_mode_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_ethernet_mode_resp_msg_v01), qcmap_msgr_set_ethernet_mode_resp_msg_data_v01},
  {sizeof(qcmap_msgr_ethernet_mode_status_ind_msg_v01), qcmap_msgr_ethernet_mode_status_ind_msg_data_v01},
  {sizeof(qcmap_msgr_set_ip_passthrough_flag_req_msg_v01), qcmap_msgr_set_ip_passthrough_flag_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_ip_passthrough_flag_resp_msg_v01), qcmap_msgr_set_ip_passthrough_flag_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_ip_passthrough_flag_req_msg_v01), qcmap_msgr_get_ip_passthrough_flag_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_ip_passthrough_flag_resp_msg_v01), qcmap_msgr_get_ip_passthrough_flag_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_ip_passthrough_state_req_msg_v01), qcmap_msgr_get_ip_passthrough_state_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_ip_passthrough_state_resp_msg_v01), qcmap_msgr_get_ip_passthrough_state_resp_msg_data_v01},
  {sizeof(qcmap_msgr_bring_up_bt_tethering_req_msg_v01), qcmap_msgr_bring_up_bt_tethering_req_msg_data_v01},
  {sizeof(qcmap_msgr_bring_up_bt_tethering_resp_msg_v01), qcmap_msgr_bring_up_bt_tethering_resp_msg_data_v01},
  {sizeof(qcmap_msgr_bring_down_bt_tethering_req_msg_v01), qcmap_msgr_bring_down_bt_tethering_req_msg_data_v01},
  {sizeof(qcmap_msgr_bring_down_bt_tethering_resp_msg_v01), qcmap_msgr_bring_down_bt_tethering_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_bt_tethering_status_req_msg_v01), qcmap_msgr_get_bt_tethering_status_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_bt_tethering_status_resp_msg_v01), qcmap_msgr_get_bt_tethering_status_resp_msg_data_v01},
  {sizeof(qcmap_msgr_bt_tethering_status_ind_register_req_msg_v01), qcmap_msgr_bt_tethering_status_ind_register_req_msg_data_v01},
  {sizeof(qcmap_msgr_bt_tethering_status_ind_register_resp_msg_v01), qcmap_msgr_bt_tethering_status_ind_register_resp_msg_data_v01},
  {sizeof(qcmap_msgr_bt_tethering_status_ind_msg_v01), qcmap_msgr_bt_tethering_status_ind_msg_data_v01},
  {sizeof(qcmap_msgr_bt_tethering_wan_ind_register_req_msg_v01), qcmap_msgr_bt_tethering_wan_ind_register_req_msg_data_v01},
  {sizeof(qcmap_msgr_bt_tethering_wan_ind_register_resp_msg_v01), qcmap_msgr_bt_tethering_wan_ind_register_resp_msg_data_v01},
  {sizeof(qcmap_msgr_bt_tethering_wan_ind_msg_v01), qcmap_msgr_bt_tethering_wan_ind_msg_data_v01},
  {sizeof(qcmap_msgr_set_initial_packet_threshold_req_msg_v01), qcmap_msgr_set_initial_packet_threshold_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_initial_packet_threshold_resp_msg_v01), qcmap_msgr_set_initial_packet_threshold_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_initial_packet_threshold_req_msg_v01), qcmap_msgr_get_initial_packet_threshold_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_initial_packet_threshold_resp_msg_v01), qcmap_msgr_get_initial_packet_threshold_resp_msg_data_v01},
  {sizeof(qcmap_msgr_enable_socksv5_proxy_req_msg_v01), qcmap_msgr_enable_socksv5_proxy_req_msg_data_v01},
  {sizeof(qcmap_msgr_enable_socksv5_proxy_resp_msg_v01), qcmap_msgr_enable_socksv5_proxy_resp_msg_data_v01},
  {sizeof(qcmap_msgr_disable_socksv5_proxy_req_msg_v01), qcmap_msgr_disable_socksv5_proxy_req_msg_data_v01},
  {sizeof(qcmap_msgr_disable_socksv5_proxy_resp_msg_v01), qcmap_msgr_disable_socksv5_proxy_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_socksv5_proxy_config_req_msg_v01), qcmap_msgr_get_socksv5_proxy_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_socksv5_proxy_config_resp_msg_v01), qcmap_msgr_get_socksv5_proxy_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_socksv5_proxy_config_req_msg_v01), qcmap_msgr_set_socksv5_proxy_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_socksv5_proxy_config_resp_msg_v01), qcmap_msgr_set_socksv5_proxy_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_enable_packet_stats_req_msg_v01), qcmap_msgr_enable_packet_stats_req_msg_data_v01},
  {sizeof(qcmap_msgr_enable_packet_stats_resp_msg_v01), qcmap_msgr_enable_packet_stats_resp_msg_data_v01},
  {sizeof(qcmap_msgr_disable_packet_stats_req_msg_v01), qcmap_msgr_disable_packet_stats_req_msg_data_v01},
  {sizeof(qcmap_msgr_disable_packet_stats_resp_msg_v01), qcmap_msgr_disable_packet_stats_resp_msg_data_v01},
  {sizeof(qcmap_msgr_reset_packet_stats_req_msg_v01), qcmap_msgr_reset_packet_stats_req_msg_data_v01},
  {sizeof(qcmap_msgr_reset_packet_stats_resp_msg_v01), qcmap_msgr_reset_packet_stats_resp_msg_data_v01},
  {sizeof(qcmap_msgr_packet_stats_status_req_msg_v01), qcmap_msgr_packet_stats_status_req_msg_data_v01},
  {sizeof(qcmap_msgr_packet_stats_status_resp_msg_v01), qcmap_msgr_packet_stats_status_resp_msg_data_v01},
  {sizeof(qcmap_msgr_packet_stats_status_ind_register_req_msg_v01), qcmap_msgr_packet_stats_status_ind_register_req_msg_data_v01},
  {sizeof(qcmap_msgr_packet_stats_status_ind_register_resp_msg_v01), qcmap_msgr_packet_stats_status_ind_register_resp_msg_data_v01},
  {sizeof(qcmap_msgr_packet_stats_status_ind_msg_v01), qcmap_msgr_packet_stats_status_ind_msg_data_v01},
  {sizeof(qcmap_msgr_delete_wwan_policy_req_msg_v01), qcmap_msgr_delete_wwan_policy_req_msg_data_v01},
  {sizeof(qcmap_msgr_delete_wwan_policy_resp_msg_v01), qcmap_msgr_delete_wwan_policy_resp_msg_data_v01},
  {sizeof(qcmap_msgr_wwan_policy_list_req_msg_v01), 0},
  {sizeof(qcmap_msgr_wwan_policy_list_resp_msg_v01), qcmap_msgr_wwan_policy_list_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_vlan_config_req_msg_v01), qcmap_msgr_set_vlan_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_vlan_config_resp_msg_v01), qcmap_msgr_set_vlan_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_vlan_config_req_msg_v01), qcmap_msgr_get_vlan_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_vlan_config_resp_msg_v01), qcmap_msgr_get_vlan_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_delete_vlan_config_req_msg_v01), qcmap_msgr_delete_vlan_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_delete_vlan_config_resp_msg_v01), qcmap_msgr_delete_vlan_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_unmanaged_l2tp_state_req_msg_v01), qcmap_msgr_set_unmanaged_l2tp_state_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_unmanaged_l2tp_state_resp_msg_v01), qcmap_msgr_set_unmanaged_l2tp_state_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_l2tp_config_req_msg_v01), qcmap_msgr_set_l2tp_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_l2tp_config_resp_msg_v01), qcmap_msgr_set_l2tp_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_MTU_for_l2tp_config_req_msg_v01), qcmap_msgr_set_MTU_for_l2tp_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_MTU_for_l2tp_config_resp_msg_v01), qcmap_msgr_set_MTU_for_l2tp_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_TCP_MSS_for_l2tp_config_req_msg_v01), qcmap_msgr_set_TCP_MSS_for_l2tp_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_TCP_MSS_for_l2tp_config_resp_msg_v01), qcmap_msgr_set_TCP_MSS_for_l2tp_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_l2tp_config_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_l2tp_config_resp_msg_v01), qcmap_msgr_get_l2tp_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_delete_l2tp_tunnel_config_req_msg_v01), qcmap_msgr_delete_l2tp_tunnel_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_delete_l2tp_tunnel_config_resp_msg_v01), qcmap_msgr_delete_l2tp_tunnel_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_add_pdn_to_vlan_mapping_req_msg_v01), qcmap_msgr_add_pdn_to_vlan_mapping_req_msg_data_v01},
  {sizeof(qcmap_msgr_add_pdn_to_vlan_mapping_resp_msg_v01), qcmap_msgr_add_pdn_to_vlan_mapping_resp_msg_data_v01},
  {sizeof(qcmap_msgr_delete_pdn_to_vlan_mapping_req_msg_v01), qcmap_msgr_delete_pdn_to_vlan_mapping_req_msg_data_v01},
  {sizeof(qcmap_msgr_delete_pdn_to_vlan_mapping_resp_msg_v01), qcmap_msgr_delete_pdn_to_vlan_mapping_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_pdn_to_vlan_mappings_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_pdn_to_vlan_mappings_resp_msg_v01), qcmap_msgr_get_pdn_to_vlan_mappings_resp_msg_data_v01},
  {sizeof(qcmap_msgr_enable_gsb_req_msg_v01), qcmap_msgr_enable_gsb_req_msg_data_v01},
  {sizeof(qcmap_msgr_enable_gsb_resp_msg_v01), qcmap_msgr_enable_gsb_resp_msg_data_v01},
  {sizeof(qcmap_msgr_disable_gsb_req_msg_v01), qcmap_msgr_disable_gsb_req_msg_data_v01},
  {sizeof(qcmap_msgr_disable_gsb_resp_msg_v01), qcmap_msgr_disable_gsb_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_gsb_config_req_msg_v01), qcmap_msgr_get_gsb_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_gsb_config_resp_msg_v01), qcmap_msgr_get_gsb_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_gsb_config_req_msg_v01), qcmap_msgr_set_gsb_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_gsb_config_resp_msg_v01), qcmap_msgr_set_gsb_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_delete_gsb_config_req_msg_v01), qcmap_msgr_delete_gsb_config_req_msg_data_v01},
  {sizeof(qcmap_msgr_delete_gsb_config_resp_msg_v01), qcmap_msgr_delete_gsb_config_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_wwan_profile_preference_req_msg_v01), qcmap_msgr_set_wwan_profile_preference_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_wwan_profile_preference_resp_msg_v01), qcmap_msgr_set_wwan_profile_preference_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_dun_dongle_mode_req_msg_v01), qcmap_msgr_set_dun_dongle_mode_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_dun_dongle_mode_resp_msg_v01), qcmap_msgr_set_dun_dongle_mode_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_dun_dongle_mode_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_dun_dongle_mode_resp_msg_v01), qcmap_msgr_get_dun_dongle_mode_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_data_path_opt_status_req_msg_v01), qcmap_msgr_set_data_path_opt_status_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_data_path_opt_status_resp_msg_v01), qcmap_msgr_set_data_path_opt_status_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_data_path_opt_status_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_data_path_opt_status_resp_msg_v01), qcmap_msgr_get_data_path_opt_status_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_pmip_mode_req_msg_v01), qcmap_msgr_get_pmip_mode_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_pmip_mode_resp_msg_v01), qcmap_msgr_get_pmip_mode_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_pmip_mode_req_msg_v01), qcmap_msgr_set_pmip_mode_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_pmip_mode_resp_msg_v01), qcmap_msgr_set_pmip_mode_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_backhaul_status_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_backhaul_status_resp_msg_v01), qcmap_msgr_get_backhaul_status_resp_msg_data_v01},
  {sizeof(qcmap_msgr_backhaul_status_ind_msg_v01), qcmap_msgr_backhaul_status_ind_msg_data_v01},
  {sizeof(qcmap_msgr_get_wwan_roaming_status_req_msg_v01), qcmap_msgr_get_wwan_roaming_status_req_msg_data_v01},
  {sizeof(qcmap_msgr_get_wwan_roaming_status_resp_msg_v01), qcmap_msgr_get_wwan_roaming_status_resp_msg_data_v01},
  {sizeof(qcmap_msgr_wwan_roaming_status_ind_msg_v01), qcmap_msgr_wwan_roaming_status_ind_msg_data_v01},
  {sizeof(qcmap_msgr_get_lan_bridges_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_lan_bridges_resp_msg_v01), qcmap_msgr_get_lan_bridges_resp_msg_data_v01},
  {sizeof(qcmap_msgr_select_lan_bridge_req_msg_v01), qcmap_msgr_select_lan_bridge_req_msg_data_v01},
  {sizeof(qcmap_msgr_select_lan_bridge_resp_msg_v01), qcmap_msgr_select_lan_bridge_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_always_on_wlan_req_msg_v01), qcmap_msgr_set_always_on_wlan_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_always_on_wlan_resp_msg_v01), qcmap_msgr_set_always_on_wlan_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_always_on_wlan_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_always_on_wlan_resp_msg_v01), qcmap_msgr_get_always_on_wlan_resp_msg_data_v01},
  {sizeof(qcmap_msgr_set_p2p_role_req_msg_v01), qcmap_msgr_set_p2p_role_req_msg_data_v01},
  {sizeof(qcmap_msgr_set_p2p_role_resp_msg_v01), qcmap_msgr_set_p2p_role_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_p2p_role_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_p2p_role_resp_msg_v01), qcmap_msgr_get_p2p_role_resp_msg_data_v01},
  {sizeof(qcmap_msgr_get_wwan_profile_preference_req_msg_v01), 0},
  {sizeof(qcmap_msgr_get_wwan_profile_preference_resp_msg_v01), qcmap_msgr_get_wwan_profile_preference_resp_msg_data_v01}
};

/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object qcmap_msgr_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *qcmap_msgr_qmi_idl_type_table_object_referenced_tables_v01[] =
{&qcmap_msgr_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object qcmap_msgr_qmi_idl_type_table_object_v01 = {
  sizeof(qcmap_msgr_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(qcmap_msgr_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  qcmap_msgr_type_table_v01,
  qcmap_msgr_message_table_v01,
  qcmap_msgr_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry qcmap_msgr_service_command_messages_v01[] = {
  {QMI_QCMAP_MSGR_INDICATION_REGISTER_REQ_V01, QMI_IDL_TYPE16(0, 0), 8},
  {QMI_QCMAP_MSGR_MOBILE_AP_ENABLE_REQ_V01, QMI_IDL_TYPE16(0, 2), 0},
  {QMI_QCMAP_MSGR_MOBILE_AP_DISABLE_REQ_V01, QMI_IDL_TYPE16(0, 4), 7},
  {QMI_QCMAP_MSGR_BRING_UP_WWAN_REQ_V01, QMI_IDL_TYPE16(0, 18), 21},
  {QMI_QCMAP_MSGR_TEAR_DOWN_WWAN_REQ_V01, QMI_IDL_TYPE16(0, 21), 21},
  {QMI_QCMAP_MSGR_WWAN_STATUS_IND_REG_REQ_V01, QMI_IDL_TYPE16(0, 24), 4},
  {QMI_QCMAP_MSGR_ENABLE_WLAN_REQ_V01, QMI_IDL_TYPE16(0, 33), 11},
  {QMI_QCMAP_MSGR_DISABLE_WLAN_REQ_V01, QMI_IDL_TYPE16(0, 35), 11},
  {QMI_QCMAP_MSGR_MOBILE_AP_STATUS_IND_REG_REQ_V01, QMI_IDL_TYPE16(0, 27), 4},
  {QMI_QCMAP_MSGR_ADD_STATIC_NAT_ENTRY_REQ_V01, QMI_IDL_TYPE16(0, 37), 26},
  {QMI_QCMAP_MSGR_DELETE_STATIC_NAT_ENTRY_REQ_V01, QMI_IDL_TYPE16(0, 39), 26},
  {QMI_QCMAP_MSGR_GET_STATIC_NAT_ENTRIES_REQ_V01, QMI_IDL_TYPE16(0, 41), 7},
  {QMI_QCMAP_MSGR_SET_DMZ_REQ_V01, QMI_IDL_TYPE16(0, 43), 21},
  {QMI_QCMAP_MSGR_DELETE_DMZ_REQ_V01, QMI_IDL_TYPE16(0, 47), 14},
  {QMI_QCMAP_MSGR_GET_DMZ_REQ_V01, QMI_IDL_TYPE16(0, 45), 14},
  {QMI_QCMAP_MSGR_GET_WWAN_STATS_REQ_V01, QMI_IDL_TYPE16(0, 49), 21},
  {QMI_QCMAP_MSGR_RESET_WWAN_STATS_REQ_V01, QMI_IDL_TYPE16(0, 51), 21},
  {QMI_QCMAP_MSGR_GET_IPSEC_VPN_PASS_THROUGH_REQ_V01, QMI_IDL_TYPE16(0, 55), 14},
  {QMI_QCMAP_MSGR_SET_IPSEC_VPN_PASS_THROUGH_REQ_V01, QMI_IDL_TYPE16(0, 53), 18},
  {QMI_QCMAP_MSGR_GET_PPTP_VPN_PASS_THROUGH_REQ_V01, QMI_IDL_TYPE16(0, 59), 14},
  {QMI_QCMAP_MSGR_SET_PPTP_VPN_PASS_THROUGH_REQ_V01, QMI_IDL_TYPE16(0, 57), 18},
  {QMI_QCMAP_MSGR_GET_L2TP_VPN_PASS_THROUGH_REQ_V01, QMI_IDL_TYPE16(0, 63), 14},
  {QMI_QCMAP_MSGR_SET_L2TP_VPN_PASS_THROUGH_REQ_V01, QMI_IDL_TYPE16(0, 61), 18},
  {QMI_QCMAP_MSGR_SET_NAT_TYPE_REQ_V01, QMI_IDL_TYPE16(0, 65), 21},
  {QMI_QCMAP_MSGR_GET_NAT_TYPE_REQ_V01, QMI_IDL_TYPE16(0, 67), 14},
  {QMI_QCMAP_MSGR_SET_AUTO_CONNECT_REQ_V01, QMI_IDL_TYPE16(0, 69), 18},
  {QMI_QCMAP_MSGR_GET_AUTO_CONNECT_REQ_V01, QMI_IDL_TYPE16(0, 71), 14},
  {QMI_QCMAP_MSGR_ADD_FIREWALL_ENTRY_REQ_V01, QMI_IDL_TYPE16(0, 83), 137},
  {QMI_QCMAP_MSGR_GET_FIREWALL_ENTRIES_HANDLE_LIST_REQ_V01, QMI_IDL_TYPE16(0, 85), 21},
  {QMI_QCMAP_MSGR_GET_FIREWALL_ENTRY_REQ_V01, QMI_IDL_TYPE16(0, 87), 21},
  {QMI_QCMAP_MSGR_DELETE_FIREWALL_ENTRY_REQ_V01, QMI_IDL_TYPE16(0, 89), 21},
  {QMI_QCMAP_MSGR_GET_WWAN_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 91), 25},
  {QMI_QCMAP_MSGR_MOBILE_AP_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 93), 7},
  {QMI_QCMAP_MSGR_WWAN_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 95), 21},
  {QMI_QCMAP_MSGR_SET_NAT_TIMEOUT_REQ_V01, QMI_IDL_TYPE16(0, 99), 21},
  {QMI_QCMAP_MSGR_GET_NAT_TIMEOUT_REQ_V01, QMI_IDL_TYPE16(0, 101), 14},
  {QMI_QCMAP_MSGR_SET_LAN_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 103), 24},
  {QMI_QCMAP_MSGR_GET_WLAN_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 107), 7},
  {QMI_QCMAP_MSGR_ACTIVATE_WLAN_REQ_V01, QMI_IDL_TYPE16(0, 109), 7},
  {QMI_QCMAP_MSGR_GET_LAN_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 113), 0},
  {QMI_QCMAP_MSGR_USB_LINK_UP_REQ_V01, QMI_IDL_TYPE16(0, 121), 14},
  {QMI_QCMAP_MSGR_USB_LINK_DOWN_REQ_V01, QMI_IDL_TYPE16(0, 123), 14},
  {QMI_QCMAP_MSGR_ENABLE_IPV6_REQ_V01, QMI_IDL_TYPE16(0, 12), 7},
  {QMI_QCMAP_MSGR_DISABLE_IPV6_REQ_V01, QMI_IDL_TYPE16(0, 14), 7},
  {QMI_QCMAP_MSGR_SET_ROAMING_PREF_REQ_V01, QMI_IDL_TYPE16(0, 75), 18},
  {QMI_QCMAP_MSGR_GET_ROAMING_PREF_REQ_V01, QMI_IDL_TYPE16(0, 77), 14},
  {QMI_QCMAP_MSGR_SET_WWAN_POLICY_REQ_V01, QMI_IDL_TYPE16(0, 125), 26},
  {QMI_QCMAP_MSGR_GET_WWAN_POLICY_REQ_V01, QMI_IDL_TYPE16(0, 131), 7},
  {QMI_QCMAP_MSGR_GET_IPV6_STATE_REQ_V01, QMI_IDL_TYPE16(0, 16), 0},
  {QMI_QCMAP_MSGR_ENABLE_UPNP_REQ_V01, QMI_IDL_TYPE16(0, 133), 0},
  {QMI_QCMAP_MSGR_DISABLE_UPNP_REQ_V01, QMI_IDL_TYPE16(0, 135), 0},
  {QMI_QCMAP_MSGR_ENABLE_DLNA_REQ_V01, QMI_IDL_TYPE16(0, 137), 0},
  {QMI_QCMAP_MSGR_DISABLE_DLNA_REQ_V01, QMI_IDL_TYPE16(0, 139), 0},
  {QMI_QCMAP_MSGR_SET_FIREWALL_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 141), 26},
  {QMI_QCMAP_MSGR_GET_FIREWALL_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 143), 14},
  {QMI_QCMAP_MSGR_ENABLE_MULTICAST_DNS_RESPONDER_REQ_V01, QMI_IDL_TYPE16(0, 145), 0},
  {QMI_QCMAP_MSGR_DISABLE_MULTICAST_DNS_RESPONDER_REQ_V01, QMI_IDL_TYPE16(0, 147), 0},
  {QMI_QCMAP_MSGR_GET_UPNP_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 149), 0},
  {QMI_QCMAP_MSGR_GET_DLNA_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 151), 0},
  {QMI_QCMAP_MSGR_GET_MULTICAST_DNS_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 153), 0},
  {QMI_QCMAP_MSGR_STATION_MODE_STATUS_IND_REG_REQ_V01, QMI_IDL_TYPE16(0, 30), 4},
  {QMI_QCMAP_MSGR_GET_STATION_MODE_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 97), 0},
  {QMI_QCMAP_MSGR_SET_QCMAP_BOOTUP_CFG_REQ_V01, QMI_IDL_TYPE16(0, 155), 14},
  {QMI_QCMAP_MSGR_GET_QCMAP_BOOTUP_CFG_REQ_V01, QMI_IDL_TYPE16(0, 157), 0},
  {QMI_QCMAP_MSGR_SET_DLNA_MEDIA_DIR_REQ_V01, QMI_IDL_TYPE16(0, 159), 1005},
  {QMI_QCMAP_MSGR_GET_DLNA_MEDIA_DIR_REQ_V01, QMI_IDL_TYPE16(0, 161), 0},
  {QMI_QCMAP_MSGR_SET_WLAN_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 105), 59},
  {QMI_QCMAP_MSGR_ACTIVATE_LAN_REQ_V01, QMI_IDL_TYPE16(0, 111), 7},
  {QMI_QCMAP_MSGR_GET_WLAN_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 115), 0},
  {QMI_QCMAP_MSGR_ENABLE_IPV4_REQ_V01, QMI_IDL_TYPE16(0, 6), 7},
  {QMI_QCMAP_MSGR_DISABLE_IPV4_REQ_V01, QMI_IDL_TYPE16(0, 8), 7},
  {QMI_QCMAP_MSGR_GET_IPV4_STATE_REQ_V01, QMI_IDL_TYPE16(0, 10), 0},
  {QMI_QCMAP_MSGR_GET_DATA_BITRATE_REQ_V01, QMI_IDL_TYPE16(0, 163), 14},
  {QMI_QCMAP_MSGR_GET_UPNP_NOTIFY_INTERVAL_REQ_V01, QMI_IDL_TYPE16(0, 165), 0},
  {QMI_QCMAP_MSGR_SET_UPNP_NOTIFY_INTERVAL_REQ_V01, QMI_IDL_TYPE16(0, 167), 7},
  {QMI_QCMAP_MSGR_GET_DLNA_NOTIFY_INTERVAL_REQ_V01, QMI_IDL_TYPE16(0, 169), 0},
  {QMI_QCMAP_MSGR_SET_DLNA_NOTIFY_INTERVAL_REQ_V01, QMI_IDL_TYPE16(0, 171), 7},
  {QMI_QCMAP_MSGR_ADD_DHCP_RESERVATION_RECORD_REQ_V01, QMI_IDL_TYPE16(0, 173), 121},
  {QMI_QCMAP_MSGR_GET_DHCP_RESERVATION_RECORDS_REQ_V01, QMI_IDL_TYPE16(0, 175), 7},
  {QMI_QCMAP_MSGR_EDIT_DHCP_RESERVATION_RECORD_REQ_V01, QMI_IDL_TYPE16(0, 177), 128},
  {QMI_QCMAP_MSGR_DELETE_DHCP_RESERVATION_RECORD_REQ_V01, QMI_IDL_TYPE16(0, 179), 14},
  {QMI_QCMAP_MSGR_ACTIVATE_HOSTAPD_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 117), 25},
  {QMI_QCMAP_MSGR_ACTIVATE_SUPPLICANT_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 119), 7},
  {QMI_QCMAP_MSGR_ENABLE_ALG_REQ_V01, QMI_IDL_TYPE16(0, 181), 18},
  {QMI_QCMAP_MSGR_DISABLE_ALG_REQ_V01, QMI_IDL_TYPE16(0, 183), 18},
  {QMI_QCMAP_MSGR_GET_WEBSERVER_WWAN_ACCESS_REQ_V01, QMI_IDL_TYPE16(0, 187), 7},
  {QMI_QCMAP_MSGR_SET_WEBSERVER_WWAN_ACCESS_REQ_V01, QMI_IDL_TYPE16(0, 185), 11},
  {QMI_QCMAP_MSGR_SET_SIP_SERVER_INFO_REQ_V01, QMI_IDL_TYPE16(0, 189), 118},
  {QMI_QCMAP_MSGR_GET_SIP_SERVER_INFO_REQ_V01, QMI_IDL_TYPE16(0, 191), 7},
  {QMI_QCMAP_MSGR_RESTORE_FACTORY_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 193), 7},
  {QMI_QCMAP_MSGR_SET_ODU_MODE_REQ_V01, QMI_IDL_TYPE16(0, 195), 7},
  {QMI_QCMAP_MSGR_GET_ODU_MODE_REQ_V01, QMI_IDL_TYPE16(0, 197), 0},
  {QMI_QCMAP_MSGR_SET_ODU_PSWD_REQ_V01, QMI_IDL_TYPE16(0, 199), 18},
  {QMI_QCMAP_MSGR_GET_CONNECTED_DEVICES_INFO_REQ_V01, QMI_IDL_TYPE16(0, 201), 7},
  {QMI_QCMAP_MSGR_SET_LOOPBACK_FLAG_REQ_V01, QMI_IDL_TYPE16(0, 79), 11},
  {QMI_QCMAP_MSGR_GET_LOOPBACK_FLAG_REQ_V01, QMI_IDL_TYPE16(0, 81), 7},
  {QMI_QCMAP_MSGR_GET_IPV6_SIP_SERVER_INFO_REQ_V01, QMI_IDL_TYPE16(0, 203), 7},
  {QMI_QCMAP_MSGR_SET_SUPPLICANT_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 205), 11},
  {QMI_QCMAP_MSGR_GET_CRADLE_MODE_REQ_V01, QMI_IDL_TYPE16(0, 207), 0},
  {QMI_QCMAP_MSGR_SET_CRADLE_MODE_REQ_V01, QMI_IDL_TYPE16(0, 209), 14},
  {QMI_QCMAP_MSGR_GET_PREFIX_DELEGATION_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 212), 0},
  {QMI_QCMAP_MSGR_SET_PREFIX_DELEGATION_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 214), 11},
  {QMI_QCMAP_MSGR_GET_PREFIX_DELEGATION_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 216), 0},
  {QMI_QCMAP_MSGR_SET_GATEWAY_URL_REQ_V01, QMI_IDL_TYPE16(0, 218), 312},
  {QMI_QCMAP_MSGR_GET_GATEWAY_URL_REQ_V01, QMI_IDL_TYPE16(0, 220), 0},
  {QMI_QCMAP_MSGR_ENABLE_DYNAMIC_DNS_REQ_V01, QMI_IDL_TYPE16(0, 222), 7},
  {QMI_QCMAP_MSGR_DISABLE_DYNAMIC_DNS_REQ_V01, QMI_IDL_TYPE16(0, 224), 7},
  {QMI_QCMAP_MSGR_SET_DYNAMIC_DNS_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 226), 366},
  {QMI_QCMAP_MSGR_GET_DYNAMIC_DNS_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 228), 0},
  {QMI_QCMAP_MSGR_GET_TINY_PROXY_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 230), 0},
  {QMI_QCMAP_MSGR_ENABLE_TINY_PROXY_REQ_V01, QMI_IDL_TYPE16(0, 232), 7},
  {QMI_QCMAP_MSGR_DISABLE_TINY_PROXY_REQ_V01, QMI_IDL_TYPE16(0, 234), 7},
  {QMI_QCMAP_MSGR_SET_DLNA_WHITELISTING_REQ_V01, QMI_IDL_TYPE16(0, 236), 11},
  {QMI_QCMAP_MSGR_GET_DLNA_WHITELISTING_REQ_V01, QMI_IDL_TYPE16(0, 238), 7},
  {QMI_QCMAP_MSGR_ADD_DLNA_WHITELISTIP_REQ_V01, QMI_IDL_TYPE16(0, 240), 14},
  {QMI_QCMAP_MSGR_DELETE_DLNA_WHITELIST_IP_REQ_V01, QMI_IDL_TYPE16(0, 242), 14},
  {QMI_QCMAP_MSGR_ENABLE_STA_MODE_REQ_V01, QMI_IDL_TYPE16(0, 244), 7},
  {QMI_QCMAP_MSGR_WLAN_STATUS_IND_REG_REQ_V01, QMI_IDL_TYPE16(0, 248), 4},
  {QMI_QCMAP_MSGR_SET_BACKHAUL_PREF_REQ_V01, QMI_IDL_TYPE16(0, 251), 42},
  {QMI_QCMAP_MSGR_GET_BACKHAUL_PREF_REQ_V01, QMI_IDL_TYPE16(0, 253), 0},
  {QMI_QCMAP_MSGR_DISABLE_STA_MODE_REQ_V01, QMI_IDL_TYPE16(0, 246), 7},
  {QMI_QCMAP_MSGR_CRADLE_MODE_STATUS_IND_REG_REQ_V01, QMI_IDL_TYPE16(0, 255), 7},
  {QMI_QCMAP_MSGR_GET_ETHERNET_MODE_REQ_V01, QMI_IDL_TYPE16(0, 259), 0},
  {QMI_QCMAP_MSGR_SET_ETHERNET_MODE_REQ_V01, QMI_IDL_TYPE16(0, 261), 14},
  {QMI_QCMAP_MSGR_ETHERNET_MODE_STATUS_IND_REG_REQ_V01, QMI_IDL_TYPE16(0, 257), 7},
  {QMI_QCMAP_MSGR_SET_IP_PASSTHROUGH_FLAG_REQ_V01, QMI_IDL_TYPE16(0, 264), 127},
  {QMI_QCMAP_MSGR_GET_IP_PASSTHROUGH_FLAG_REQ_V01, QMI_IDL_TYPE16(0, 266), 7},
  {QMI_QCMAP_MSGR_GET_IP_PASSTHROUGH_STATE_REQ_V01, QMI_IDL_TYPE16(0, 268), 7},
  {QMI_QCMAP_MSGR_BRING_UP_BT_TETHERING_REQ_V01, QMI_IDL_TYPE16(0, 270), 14},
  {QMI_QCMAP_MSGR_BRING_DOWN_BT_TETHERING_REQ_V01, QMI_IDL_TYPE16(0, 272), 7},
  {QMI_QCMAP_MSGR_GET_BT_TETHERING_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 274), 7},
  {QMI_QCMAP_MSGR_BT_TETHERING_STATUS_IND_REG_REQ_V01, QMI_IDL_TYPE16(0, 276), 4},
  {QMI_QCMAP_MSGR_BT_TETHERING_WAN_IND_REG_REQ_V01, QMI_IDL_TYPE16(0, 279), 4},
  {QMI_QCMAP_MSGR_SET_INITIAL_PACKET_THRESHOLD_REQ_V01, QMI_IDL_TYPE16(0, 282), 14},
  {QMI_QCMAP_MSGR_GET_INITIAL_PACKET_THRESHOLD_REQ_V01, QMI_IDL_TYPE16(0, 284), 7},
  {QMI_QCMAP_MSGR_ENABLE_SOCKSV5_PROXY_REQ_V01, QMI_IDL_TYPE16(0, 286), 7},
  {QMI_QCMAP_MSGR_DISABLE_SOCKSV5_PROXY_REQ_V01, QMI_IDL_TYPE16(0, 288), 7},
  {QMI_QCMAP_MSGR_GET_SOCKSV5_PROXY_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 290), 7},
  {QMI_QCMAP_MSGR_SET_SOCKSV5_PROXY_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 292), 812},
  {QMI_QCMAP_MSGR_ENABLE_PACKET_STATS_REQ_V01, QMI_IDL_TYPE16(0, 294), 7},
  {QMI_QCMAP_MSGR_DISABLE_PACKET_STATS_REQ_V01, QMI_IDL_TYPE16(0, 296), 7},
  {QMI_QCMAP_MSGR_RESET_PACKET_STATS_REQ_V01, QMI_IDL_TYPE16(0, 298), 7},
  {QMI_QCMAP_MSGR_PACKET_STATS_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 300), 7},
  {QMI_QCMAP_MSGR_PACK_STATS_STATUS_IND_REG_REQ_V01, QMI_IDL_TYPE16(0, 302), 4},
  {QMI_QCMAP_MSGR_DELETE_WWAN_POLICY_REQ_V01, QMI_IDL_TYPE16(0, 305), 14},
  {QMI_QCMAP_MSGR_WWAN_POLICY_LIST_REQ_V01, QMI_IDL_TYPE16(0, 307), 0},
  {QMI_QCMAP_MSGR_SET_VLAN_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 309), 29},
  {QMI_QCMAP_MSGR_GET_VLAN_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 311), 7},
  {QMI_QCMAP_MSGR_DELETE_VLAN_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 313), 29},
  {QMI_QCMAP_MSGR_SET_UNMANAGED_L2TP_STATE_REQ_V01, QMI_IDL_TYPE16(0, 315), 11},
  {QMI_QCMAP_MSGR_SET_L2TP_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 317), 172},
  {QMI_QCMAP_MSGR_SET_MTU_FOR_L2TP_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 319), 11},
  {QMI_QCMAP_MSGR_SET_TCP_MSS_FOR_L2TP_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 321), 11},
  {QMI_QCMAP_MSGR_GET_L2TP_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 323), 0},
  {QMI_QCMAP_MSGR_DELETE_L2TP_TUNNEL_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 325), 14},
  {QMI_QCMAP_MSGR_ADD_PDN_TO_VLAN_MAPPING_REQ_V01, QMI_IDL_TYPE16(0, 327), 19},
  {QMI_QCMAP_MSGR_GET_PDN_TO_VLAN_MAPPINGS_REQ_V01, QMI_IDL_TYPE16(0, 331), 0},
  {QMI_QCMAP_MSGR_CREATE_WWAN_POLICY_REQ_V01, QMI_IDL_TYPE16(0, 127), 26},
  {QMI_QCMAP_MSGR_UPDATE_WWAN_POLICY_REQ_V01, QMI_IDL_TYPE16(0, 129), 40},
  {QMI_QCMAP_MSGR_SET_WWAN_PROFILE_PREFERENCE_REQ_V01, QMI_IDL_TYPE16(0, 343), 14},
  {QMI_QCMAP_MSGR_ENABLE_GSB_REQ_V01, QMI_IDL_TYPE16(0, 333), 7},
  {QMI_QCMAP_MSGR_DISABLE_GSB_REQ_V01, QMI_IDL_TYPE16(0, 335), 7},
  {QMI_QCMAP_MSGR_GET_GSB_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 337), 7},
  {QMI_QCMAP_MSGR_SET_GSB_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 339), 42},
  {QMI_QCMAP_MSGR_DELETE_GSB_CONFIG_REQ_V01, QMI_IDL_TYPE16(0, 341), 26},
  {QMI_QCMAP_MSGR_SET_DUN_DONGLE_MODE_REQ_V01, QMI_IDL_TYPE16(0, 345), 11},
  {QMI_QCMAP_MSGR_GET_DUN_DONGLE_MODE_REQ_V01, QMI_IDL_TYPE16(0, 347), 0},
  {QMI_QCMAP_MSGR_GET_DATA_PATH_OPT_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 351), 0},
  {QMI_QCMAP_MSGR_SET_DATA_PATH_OPT_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 349), 11},
  {QMI_QCMAP_MSGR_GET_PMIP_MODE_REQ_V01, QMI_IDL_TYPE16(0, 353), 7},
  {QMI_QCMAP_MSGR_SET_PMIP_MODE_REQ_V01, QMI_IDL_TYPE16(0, 355), 586},
  {QMI_QCMAP_MSGR_GET_BACKHAUL_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 357), 0},
  {QMI_QCMAP_MSGR_GET_WWAN_ROAMING_STATUS_REQ_V01, QMI_IDL_TYPE16(0, 360), 7},
  {QMI_QCMAP_MSGR_DELETE_PDN_TO_VLAN_MAPPING_REQ_V01, QMI_IDL_TYPE16(0, 329), 19},
  {QMI_QCMAP_MSGR_GET_LAN_BRIDGES_REQ_V01, QMI_IDL_TYPE16(0, 363), 0},
  {QMI_QCMAP_MSGR_SELECT_LAN_BRIDGE_REQ_V01, QMI_IDL_TYPE16(0, 365), 12},
  {QMI_QCMAP_MSGR_GET_ACTIVE_WLAN_IF_INFO_REQ_V01, QMI_IDL_TYPE16(0, 73), 0},
  {QMI_QCMAP_MSGR_SET_AlWAYS_ON_WLAN_REQ_V01, QMI_IDL_TYPE16(0, 367), 11},
  {QMI_QCMAP_MSGR_GET_AlWAYS_ON_WLAN_REQ_V01, QMI_IDL_TYPE16(0, 369), 0},
  {QMI_QCMAP_MSGR_GET_WWAN_PROFILE_PREFERENCE_REQ_V01, QMI_IDL_TYPE16(0, 375), 0},
  {QMI_QCMAP_MSGR_SET_P2P_ROLE_REQ_V01, QMI_IDL_TYPE16(0, 371), 18},
  {QMI_QCMAP_MSGR_GET_P2P_ROLE_REQ_V01, QMI_IDL_TYPE16(0, 373), 0}
};

static const qmi_idl_service_message_table_entry qcmap_msgr_service_response_messages_v01[] = {
  {QMI_QCMAP_MSGR_INDICATION_REGISTER_RESP_V01, QMI_IDL_TYPE16(0, 1), 7},
  {QMI_QCMAP_MSGR_MOBILE_AP_ENABLE_RESP_V01, QMI_IDL_TYPE16(0, 3), 14},
  {QMI_QCMAP_MSGR_MOBILE_AP_DISABLE_RESP_V01, QMI_IDL_TYPE16(0, 5), 7},
  {QMI_QCMAP_MSGR_BRING_UP_WWAN_RESP_V01, QMI_IDL_TYPE16(0, 19), 14},
  {QMI_QCMAP_MSGR_TEAR_DOWN_WWAN_RESP_V01, QMI_IDL_TYPE16(0, 22), 14},
  {QMI_QCMAP_MSGR_WWAN_STATUS_IND_REG_RESP_V01, QMI_IDL_TYPE16(0, 25), 7},
  {QMI_QCMAP_MSGR_ENABLE_WLAN_RESP_V01, QMI_IDL_TYPE16(0, 34), 7},
  {QMI_QCMAP_MSGR_DISABLE_WLAN_RESP_V01, QMI_IDL_TYPE16(0, 36), 7},
  {QMI_QCMAP_MSGR_MOBILE_AP_STATUS_IND_REG_RESP_V01, QMI_IDL_TYPE16(0, 28), 7},
  {QMI_QCMAP_MSGR_ADD_STATIC_NAT_ENTRY_RESP_V01, QMI_IDL_TYPE16(0, 38), 7},
  {QMI_QCMAP_MSGR_DELETE_STATIC_NAT_ENTRY_RESP_V01, QMI_IDL_TYPE16(0, 40), 7},
  {QMI_QCMAP_MSGR_GET_STATIC_NAT_ENTRIES_RESP_V01, QMI_IDL_TYPE16(0, 42), 461},
  {QMI_QCMAP_MSGR_SET_DMZ_RESP_V01, QMI_IDL_TYPE16(0, 44), 7},
  {QMI_QCMAP_MSGR_DELETE_DMZ_RESP_V01, QMI_IDL_TYPE16(0, 48), 7},
  {QMI_QCMAP_MSGR_GET_DMZ_RESP_V01, QMI_IDL_TYPE16(0, 46), 14},
  {QMI_QCMAP_MSGR_GET_WWAN_STATS_RESP_V01, QMI_IDL_TYPE16(0, 50), 42},
  {QMI_QCMAP_MSGR_RESET_WWAN_STATS_RESP_V01, QMI_IDL_TYPE16(0, 52), 7},
  {QMI_QCMAP_MSGR_GET_IPSEC_VPN_PASS_THROUGH_RESP_V01, QMI_IDL_TYPE16(0, 56), 11},
  {QMI_QCMAP_MSGR_SET_IPSEC_VPN_PASS_THROUGH_RESP_V01, QMI_IDL_TYPE16(0, 54), 7},
  {QMI_QCMAP_MSGR_GET_PPTP_VPN_PASS_THROUGH_RESP_V01, QMI_IDL_TYPE16(0, 60), 11},
  {QMI_QCMAP_MSGR_SET_PPTP_VPN_PASS_THROUGH_RESP_V01, QMI_IDL_TYPE16(0, 58), 7},
  {QMI_QCMAP_MSGR_GET_L2TP_VPN_PASS_THROUGH_RESP_V01, QMI_IDL_TYPE16(0, 64), 11},
  {QMI_QCMAP_MSGR_SET_L2TP_VPN_PASS_THROUGH_RESP_V01, QMI_IDL_TYPE16(0, 62), 7},
  {QMI_QCMAP_MSGR_SET_NAT_TYPE_RESP_V01, QMI_IDL_TYPE16(0, 66), 7},
  {QMI_QCMAP_MSGR_GET_NAT_TYPE_RESP_V01, QMI_IDL_TYPE16(0, 68), 14},
  {QMI_QCMAP_MSGR_SET_AUTO_CONNECT_RESP_V01, QMI_IDL_TYPE16(0, 70), 7},
  {QMI_QCMAP_MSGR_GET_AUTO_CONNECT_RESP_V01, QMI_IDL_TYPE16(0, 72), 11},
  {QMI_QCMAP_MSGR_ADD_FIREWALL_ENTRY_RESP_V01, QMI_IDL_TYPE16(0, 84), 14},
  {QMI_QCMAP_MSGR_GET_FIREWALL_ENTRIES_HANDLE_LIST_RESP_V01, QMI_IDL_TYPE16(0, 86), 211},
  {QMI_QCMAP_MSGR_GET_FIREWALL_ENTRY_RESP_V01, QMI_IDL_TYPE16(0, 88), 126},
  {QMI_QCMAP_MSGR_DELETE_FIREWALL_ENTRY_RESP_V01, QMI_IDL_TYPE16(0, 90), 7},
  {QMI_QCMAP_MSGR_GET_WWAN_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 92), 85},
  {QMI_QCMAP_MSGR_MOBILE_AP_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 94), 14},
  {QMI_QCMAP_MSGR_WWAN_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 96), 14},
  {QMI_QCMAP_MSGR_SET_NAT_TIMEOUT_RESP_V01, QMI_IDL_TYPE16(0, 100), 7},
  {QMI_QCMAP_MSGR_GET_NAT_TIMEOUT_RESP_V01, QMI_IDL_TYPE16(0, 102), 14},
  {QMI_QCMAP_MSGR_SET_LAN_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 104), 7},
  {QMI_QCMAP_MSGR_GET_WLAN_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 108), 14},
  {QMI_QCMAP_MSGR_ACTIVATE_WLAN_RESP_V01, QMI_IDL_TYPE16(0, 110), 7},
  {QMI_QCMAP_MSGR_GET_LAN_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 114), 70},
  {QMI_QCMAP_MSGR_USB_LINK_UP_RESP_V01, QMI_IDL_TYPE16(0, 122), 7},
  {QMI_QCMAP_MSGR_USB_LINK_DOWN_RESP_V01, QMI_IDL_TYPE16(0, 124), 7},
  {QMI_QCMAP_MSGR_ENABLE_IPV6_RESP_V01, QMI_IDL_TYPE16(0, 13), 7},
  {QMI_QCMAP_MSGR_DISABLE_IPV6_RESP_V01, QMI_IDL_TYPE16(0, 15), 7},
  {QMI_QCMAP_MSGR_SET_ROAMING_PREF_RESP_V01, QMI_IDL_TYPE16(0, 76), 7},
  {QMI_QCMAP_MSGR_GET_ROAMING_PREF_RESP_V01, QMI_IDL_TYPE16(0, 78), 11},
  {QMI_QCMAP_MSGR_SET_WWAN_POLICY_RESP_V01, QMI_IDL_TYPE16(0, 126), 14},
  {QMI_QCMAP_MSGR_GET_WWAN_POLICY_RESP_V01, QMI_IDL_TYPE16(0, 132), 26},
  {QMI_QCMAP_MSGR_GET_IPV6_STATE_RESP_V01, QMI_IDL_TYPE16(0, 17), 11},
  {QMI_QCMAP_MSGR_ENABLE_UPNP_RESP_V01, QMI_IDL_TYPE16(0, 134), 7},
  {QMI_QCMAP_MSGR_DISABLE_UPNP_RESP_V01, QMI_IDL_TYPE16(0, 136), 7},
  {QMI_QCMAP_MSGR_ENABLE_DLNA_RESP_V01, QMI_IDL_TYPE16(0, 138), 7},
  {QMI_QCMAP_MSGR_DISABLE_DLNA_RESP_V01, QMI_IDL_TYPE16(0, 140), 7},
  {QMI_QCMAP_MSGR_SET_FIREWALL_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 142), 7},
  {QMI_QCMAP_MSGR_GET_FIREWALL_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 144), 19},
  {QMI_QCMAP_MSGR_ENABLE_MULTICAST_DNS_RESPONDER_RESP_V01, QMI_IDL_TYPE16(0, 146), 7},
  {QMI_QCMAP_MSGR_DISABLE_MULTICAST_DNS_RESPONDER_RESP_V01, QMI_IDL_TYPE16(0, 148), 7},
  {QMI_QCMAP_MSGR_GET_UPNP_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 150), 14},
  {QMI_QCMAP_MSGR_GET_DLNA_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 152), 14},
  {QMI_QCMAP_MSGR_GET_MULTICAST_DNS_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 154), 14},
  {QMI_QCMAP_MSGR_STATION_MODE_STATUS_IND_REG_RESP_V01, QMI_IDL_TYPE16(0, 31), 7},
  {QMI_QCMAP_MSGR_GET_STATION_MODE_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 98), 14},
  {QMI_QCMAP_MSGR_SET_QCMAP_BOOTUP_CFG_RESP_V01, QMI_IDL_TYPE16(0, 156), 7},
  {QMI_QCMAP_MSGR_GET_QCMAP_BOOTUP_CFG_RESP_V01, QMI_IDL_TYPE16(0, 158), 21},
  {QMI_QCMAP_MSGR_SET_DLNA_MEDIA_DIR_RESP_V01, QMI_IDL_TYPE16(0, 160), 7},
  {QMI_QCMAP_MSGR_GET_DLNA_MEDIA_DIR_RESP_V01, QMI_IDL_TYPE16(0, 162), 1012},
  {QMI_QCMAP_MSGR_SET_WLAN_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 106), 7},
  {QMI_QCMAP_MSGR_ACTIVATE_LAN_RESP_V01, QMI_IDL_TYPE16(0, 112), 7},
  {QMI_QCMAP_MSGR_GET_WLAN_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 116), 59},
  {QMI_QCMAP_MSGR_ENABLE_IPV4_RESP_V01, QMI_IDL_TYPE16(0, 7), 7},
  {QMI_QCMAP_MSGR_DISABLE_IPV4_RESP_V01, QMI_IDL_TYPE16(0, 9), 7},
  {QMI_QCMAP_MSGR_GET_IPV4_STATE_RESP_V01, QMI_IDL_TYPE16(0, 11), 11},
  {QMI_QCMAP_MSGR_GET_DATA_BITRATE_RESP_V01, QMI_IDL_TYPE16(0, 164), 42},
  {QMI_QCMAP_MSGR_GET_UPNP_NOTIFY_INTERVAL_RESP_V01, QMI_IDL_TYPE16(0, 166), 14},
  {QMI_QCMAP_MSGR_SET_UPNP_NOTIFY_INTERVAL_RESP_V01, QMI_IDL_TYPE16(0, 168), 7},
  {QMI_QCMAP_MSGR_GET_DLNA_NOTIFY_INTERVAL_RESP_V01, QMI_IDL_TYPE16(0, 170), 14},
  {QMI_QCMAP_MSGR_SET_DLNA_NOTIFY_INTERVAL_RESP_V01, QMI_IDL_TYPE16(0, 172), 7},
  {QMI_QCMAP_MSGR_ADD_DHCP_RESERVATION_RECORD_RESP_V01, QMI_IDL_TYPE16(0, 174), 7},
  {QMI_QCMAP_MSGR_GET_DHCP_RESERVATION_RECORDS_RESP_V01, QMI_IDL_TYPE16(0, 176), 2231},
  {QMI_QCMAP_MSGR_EDIT_DHCP_RESERVATION_RECORD_RESP_V01, QMI_IDL_TYPE16(0, 178), 7},
  {QMI_QCMAP_MSGR_DELETE_DHCP_RESERVATION_RECORD_RESP_V01, QMI_IDL_TYPE16(0, 180), 7},
  {QMI_QCMAP_MSGR_ACTIVATE_HOSTAPD_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 118), 7},
  {QMI_QCMAP_MSGR_ACTIVATE_SUPPLICANT_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 120), 7},
  {QMI_QCMAP_MSGR_ENABLE_ALG_RESP_V01, QMI_IDL_TYPE16(0, 182), 7},
  {QMI_QCMAP_MSGR_DISABLE_ALG_RESP_V01, QMI_IDL_TYPE16(0, 184), 7},
  {QMI_QCMAP_MSGR_GET_WEBSERVER_WWAN_ACCESS_RESP_V01, QMI_IDL_TYPE16(0, 188), 11},
  {QMI_QCMAP_MSGR_SET_WEBSERVER_WWAN_ACCESS_RESP_V01, QMI_IDL_TYPE16(0, 186), 7},
  {QMI_QCMAP_MSGR_SET_SIP_SERVER_INFO_RESP_V01, QMI_IDL_TYPE16(0, 190), 7},
  {QMI_QCMAP_MSGR_GET_SIP_SERVER_INFO_RESP_V01, QMI_IDL_TYPE16(0, 192), 2822},
  {QMI_QCMAP_MSGR_RESTORE_FACTORY_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 194), 7},
  {QMI_QCMAP_MSGR_SET_ODU_MODE_RESP_V01, QMI_IDL_TYPE16(0, 196), 7},
  {QMI_QCMAP_MSGR_GET_ODU_MODE_RESP_V01, QMI_IDL_TYPE16(0, 198), 14},
  {QMI_QCMAP_MSGR_SET_ODU_PSWD_RESP_V01, QMI_IDL_TYPE16(0, 200), 7},
  {QMI_QCMAP_MSGR_GET_CONNECTED_DEVICES_INFO_RESP_V01, QMI_IDL_TYPE16(0, 202), 9931},
  {QMI_QCMAP_MSGR_SET_LOOPBACK_FLAG_RESP_V01, QMI_IDL_TYPE16(0, 80), 7},
  {QMI_QCMAP_MSGR_GET_LOOPBACK_FLAG_RESP_V01, QMI_IDL_TYPE16(0, 82), 11},
  {QMI_QCMAP_MSGR_GET_IPV6_SIP_SERVER_INFO_RESP_V01, QMI_IDL_TYPE16(0, 204), 3011},
  {QMI_QCMAP_MSGR_SET_SUPPLICANT_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 206), 7},
  {QMI_QCMAP_MSGR_GET_CRADLE_MODE_RESP_V01, QMI_IDL_TYPE16(0, 208), 14},
  {QMI_QCMAP_MSGR_SET_CRADLE_MODE_RESP_V01, QMI_IDL_TYPE16(0, 210), 7},
  {QMI_QCMAP_MSGR_GET_PREFIX_DELEGATION_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 213), 11},
  {QMI_QCMAP_MSGR_SET_PREFIX_DELEGATION_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 215), 7},
  {QMI_QCMAP_MSGR_GET_PREFIX_DELEGATION_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 217), 11},
  {QMI_QCMAP_MSGR_SET_GATEWAY_URL_RESP_V01, QMI_IDL_TYPE16(0, 219), 7},
  {QMI_QCMAP_MSGR_GET_GATEWAY_URL_RESP_V01, QMI_IDL_TYPE16(0, 221), 312},
  {QMI_QCMAP_MSGR_ENABLE_DYNAMIC_DNS_RESP_V01, QMI_IDL_TYPE16(0, 223), 7},
  {QMI_QCMAP_MSGR_DISABLE_DYNAMIC_DNS_RESP_V01, QMI_IDL_TYPE16(0, 225), 7},
  {QMI_QCMAP_MSGR_SET_DYNAMIC_DNS_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 227), 7},
  {QMI_QCMAP_MSGR_GET_DYNAMIC_DNS_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 229), 2125},
  {QMI_QCMAP_MSGR_GET_TINY_PROXY_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 231), 14},
  {QMI_QCMAP_MSGR_ENABLE_TINY_PROXY_RESP_V01, QMI_IDL_TYPE16(0, 233), 7},
  {QMI_QCMAP_MSGR_DISABLE_TINY_PROXY_RESP_V01, QMI_IDL_TYPE16(0, 235), 7},
  {QMI_QCMAP_MSGR_SET_DLNA_WHITELISTING_RESP_V01, QMI_IDL_TYPE16(0, 237), 7},
  {QMI_QCMAP_MSGR_GET_DLNA_WHITELISTING_RESP_V01, QMI_IDL_TYPE16(0, 239), 11},
  {QMI_QCMAP_MSGR_ADD_DLNA_WHITELISTIP_RESP_V01, QMI_IDL_TYPE16(0, 241), 7},
  {QMI_QCMAP_MSGR_DELETE_DLNA_WHITELIST_IP_RESP_V01, QMI_IDL_TYPE16(0, 243), 7},
  {QMI_QCMAP_MSGR_ENABLE_STA_MODE_RESP_V01, QMI_IDL_TYPE16(0, 245), 7},
  {QMI_QCMAP_MSGR_WLAN_STATUS_IND_REG_RESP_V01, QMI_IDL_TYPE16(0, 249), 7},
  {QMI_QCMAP_MSGR_SET_BACKHAUL_PREF_RESP_V01, QMI_IDL_TYPE16(0, 252), 7},
  {QMI_QCMAP_MSGR_GET_BACKHAUL_PREF_RESP_V01, QMI_IDL_TYPE16(0, 254), 42},
  {QMI_QCMAP_MSGR_DISABLE_STA_MODE_RESP_V01, QMI_IDL_TYPE16(0, 247), 7},
  {QMI_QCMAP_MSGR_CRADLE_MODE_STATUS_IND_REG_RESP_V01, QMI_IDL_TYPE16(0, 256), 7},
  {QMI_QCMAP_MSGR_GET_ETHERNET_MODE_RESP_V01, QMI_IDL_TYPE16(0, 260), 14},
  {QMI_QCMAP_MSGR_SET_ETHERNET_MODE_RESP_V01, QMI_IDL_TYPE16(0, 262), 7},
  {QMI_QCMAP_MSGR_ETHERNET_MODE_STATUS_IND_REG_RESP_V01, QMI_IDL_TYPE16(0, 258), 7},
  {QMI_QCMAP_MSGR_SET_IP_PASSTHROUGH_FLAG_RESP_V01, QMI_IDL_TYPE16(0, 265), 7},
  {QMI_QCMAP_MSGR_GET_IP_PASSTHROUGH_FLAG_RESP_V01, QMI_IDL_TYPE16(0, 267), 127},
  {QMI_QCMAP_MSGR_GET_IP_PASSTHROUGH_STATE_RESP_V01, QMI_IDL_TYPE16(0, 269), 11},
  {QMI_QCMAP_MSGR_BRING_UP_BT_TETHERING_RESP_V01, QMI_IDL_TYPE16(0, 271), 7},
  {QMI_QCMAP_MSGR_BRING_DOWN_BT_TETHERING_RESP_V01, QMI_IDL_TYPE16(0, 273), 7},
  {QMI_QCMAP_MSGR_GET_BT_TETHERING_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 275), 18},
  {QMI_QCMAP_MSGR_BT_TETHERING_STATUS_IND_REG_RESP_V01, QMI_IDL_TYPE16(0, 277), 7},
  {QMI_QCMAP_MSGR_BT_TETHERING_WAN_IND_REG_RESP_V01, QMI_IDL_TYPE16(0, 280), 7},
  {QMI_QCMAP_MSGR_SET_INITIAL_PACKET_THRESHOLD_RESP_V01, QMI_IDL_TYPE16(0, 283), 7},
  {QMI_QCMAP_MSGR_GET_INITIAL_PACKET_THRESHOLD_RESP_V01, QMI_IDL_TYPE16(0, 285), 14},
  {QMI_QCMAP_MSGR_ENABLE_SOCKSV5_PROXY_RESP_V01, QMI_IDL_TYPE16(0, 287), 7},
  {QMI_QCMAP_MSGR_DISABLE_SOCKSV5_PROXY_RESP_V01, QMI_IDL_TYPE16(0, 289), 7},
  {QMI_QCMAP_MSGR_GET_SOCKSV5_PROXY_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 291), 1546},
  {QMI_QCMAP_MSGR_SET_SOCKSV5_PROXY_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 293), 7},
  {QMI_QCMAP_MSGR_ENABLE_PACKET_STATS_RESP_V01, QMI_IDL_TYPE16(0, 295), 7},
  {QMI_QCMAP_MSGR_DISABLE_PACKET_STATS_RESP_V01, QMI_IDL_TYPE16(0, 297), 7},
  {QMI_QCMAP_MSGR_RESET_PACKET_STATS_RESP_V01, QMI_IDL_TYPE16(0, 299), 7},
  {QMI_QCMAP_MSGR_PACKET_STATS_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 301), 14},
  {QMI_QCMAP_MSGR_PACK_STATS_STATUS_IND_REG_RESP_V01, QMI_IDL_TYPE16(0, 303), 7},
  {QMI_QCMAP_MSGR_DELETE_WWAN_POLICY_RESP_V01, QMI_IDL_TYPE16(0, 306), 7},
  {QMI_QCMAP_MSGR_WWAN_POLICY_LIST_RESP_V01, QMI_IDL_TYPE16(0, 308), 1018},
  {QMI_QCMAP_MSGR_SET_VLAN_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 310), 11},
  {QMI_QCMAP_MSGR_GET_VLAN_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 312), 38012},
  {QMI_QCMAP_MSGR_DELETE_VLAN_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 314), 7},
  {QMI_QCMAP_MSGR_SET_UNMANAGED_L2TP_STATE_RESP_V01, QMI_IDL_TYPE16(0, 316), 7},
  {QMI_QCMAP_MSGR_SET_L2TP_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 318), 7},
  {QMI_QCMAP_MSGR_SET_MTU_FOR_L2TP_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 320), 7},
  {QMI_QCMAP_MSGR_SET_TCP_MSS_FOR_L2TP_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 322), 7},
  {QMI_QCMAP_MSGR_GET_L2TP_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 324), 336},
  {QMI_QCMAP_MSGR_DELETE_L2TP_TUNNEL_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 326), 7},
  {QMI_QCMAP_MSGR_ADD_PDN_TO_VLAN_MAPPING_RESP_V01, QMI_IDL_TYPE16(0, 328), 7},
  {QMI_QCMAP_MSGR_GET_PDN_TO_VLAN_MAPPINGS_RESP_V01, QMI_IDL_TYPE16(0, 332), 311},
  {QMI_QCMAP_MSGR_CREATE_WWAN_POLICY_RESP_V01, QMI_IDL_TYPE16(0, 128), 14},
  {QMI_QCMAP_MSGR_UPDATE_WWAN_POLICY_RESP_V01, QMI_IDL_TYPE16(0, 130), 7},
  {QMI_QCMAP_MSGR_SET_WWAN_PROFILE_PREFERENCE_RESP_V01, QMI_IDL_TYPE16(0, 344), 7},
  {QMI_QCMAP_MSGR_ENABLE_GSB_RESP_V01, QMI_IDL_TYPE16(0, 334), 7},
  {QMI_QCMAP_MSGR_DISABLE_GSB_RESP_V01, QMI_IDL_TYPE16(0, 336), 7},
  {QMI_QCMAP_MSGR_GET_GSB_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 338), 1035},
  {QMI_QCMAP_MSGR_SET_GSB_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 340), 7},
  {QMI_QCMAP_MSGR_DELETE_GSB_CONFIG_RESP_V01, QMI_IDL_TYPE16(0, 342), 7},
  {QMI_QCMAP_MSGR_SET_DUN_DONGLE_MODE_RESP_V01, QMI_IDL_TYPE16(0, 346), 7},
  {QMI_QCMAP_MSGR_GET_DUN_DONGLE_MODE_RESP_V01, QMI_IDL_TYPE16(0, 348), 11},
  {QMI_QCMAP_MSGR_GET_DATA_PATH_OPT_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 352), 11},
  {QMI_QCMAP_MSGR_SET_DATA_PATH_OPT_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 350), 7},
  {QMI_QCMAP_MSGR_GET_PMIP_MODE_RESP_V01, QMI_IDL_TYPE16(0, 354), 586},
  {QMI_QCMAP_MSGR_SET_PMIP_MODE_RESP_V01, QMI_IDL_TYPE16(0, 356), 7},
  {QMI_QCMAP_MSGR_GET_BACKHAUL_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 358), 22},
  {QMI_QCMAP_MSGR_GET_WWAN_ROAMING_STATUS_RESP_V01, QMI_IDL_TYPE16(0, 361), 14},
  {QMI_QCMAP_MSGR_DELETE_PDN_TO_VLAN_MAPPING_RESP_V01, QMI_IDL_TYPE16(0, 330), 7},
  {QMI_QCMAP_MSGR_GET_LAN_BRIDGES_RESP_V01, QMI_IDL_TYPE16(0, 364), 4014},
  {QMI_QCMAP_MSGR_SELECT_LAN_BRIDGE_RESP_V01, QMI_IDL_TYPE16(0, 366), 7},
  {QMI_QCMAP_MSGR_GET_ACTIVE_WLAN_IF_INFO_RESP_V01, QMI_IDL_TYPE16(0, 74), 155},
  {QMI_QCMAP_MSGR_SET_AlWAYS_ON_WLAN_RESP_V01, QMI_IDL_TYPE16(0, 368), 7},
  {QMI_QCMAP_MSGR_GET_AlWAYS_ON_WLAN_RESP_V01, QMI_IDL_TYPE16(0, 370), 11},
  {QMI_QCMAP_MSGR_GET_WWAN_PROFILE_PREFERENCE_RESP_V01, QMI_IDL_TYPE16(0, 376), 14},
  {QMI_QCMAP_MSGR_SET_P2P_ROLE_RESP_V01, QMI_IDL_TYPE16(0, 372), 7},
  {QMI_QCMAP_MSGR_GET_P2P_ROLE_RESP_V01, QMI_IDL_TYPE16(0, 374), 18}
};

static const qmi_idl_service_message_table_entry qcmap_msgr_service_indication_messages_v01[] = {
  {QMI_QCMAP_MSGR_BRING_UP_WWAN_IND_V01, QMI_IDL_TYPE16(0, 20), 111},
  {QMI_QCMAP_MSGR_TEAR_DOWN_WWAN_IND_V01, QMI_IDL_TYPE16(0, 23), 32},
  {QMI_QCMAP_MSGR_WWAN_STATUS_IND_V01, QMI_IDL_TYPE16(0, 26), 111},
  {QMI_QCMAP_MSGR_MOBILE_AP_STATUS_IND_V01, QMI_IDL_TYPE16(0, 29), 14},
  {QMI_QCMAP_MSGR_STATION_MODE_STATUS_IND_V01, QMI_IDL_TYPE16(0, 32), 14},
  {QMI_QCMAP_MSGR_CRADLE_MODE_STATUS_IND_V01, QMI_IDL_TYPE16(0, 211), 14},
  {QMI_QCMAP_MSGR_WLAN_STATUS_IND_V01, QMI_IDL_TYPE16(0, 250), 689},
  {QMI_QCMAP_MSGR_ETHERNET_MODE_STATUS_IND_V01, QMI_IDL_TYPE16(0, 263), 14},
  {QMI_QCMAP_MSGR_BT_TETHERING_STATUS_IND_V01, QMI_IDL_TYPE16(0, 278), 14},
  {QMI_QCMAP_MSGR_BT_TETHERING_WAN_IND_V01, QMI_IDL_TYPE16(0, 281), 14},
  {QMI_QCMAP_MSGR_PACKET_STATS_STATUS_IND_V01, QMI_IDL_TYPE16(0, 304), 9937},
  {QMI_QCMAP_MSGR_BACKHAUL_STATUS_IND_V01, QMI_IDL_TYPE16(0, 359), 22},
  {QMI_QCMAP_MSGR_WWAN_ROAMING_STATUS_IND_V01, QMI_IDL_TYPE16(0, 362), 14}
};

/*Service Object*/
struct qmi_idl_service_object qcmap_msgr_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x0400,
  38012,
  { sizeof(qcmap_msgr_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(qcmap_msgr_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(qcmap_msgr_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { qcmap_msgr_service_command_messages_v01, qcmap_msgr_service_response_messages_v01, qcmap_msgr_service_indication_messages_v01},
  &qcmap_msgr_qmi_idl_type_table_object_v01,
  0x45,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type qcmap_msgr_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( QCMAP_MSGR_V01_IDL_MAJOR_VERS != idl_maj_version || QCMAP_MSGR_V01_IDL_MINOR_VERS != idl_min_version
       || QCMAP_MSGR_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&qcmap_msgr_qmi_idl_service_object_v01;
}

