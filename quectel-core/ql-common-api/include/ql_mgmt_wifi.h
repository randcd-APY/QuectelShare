/**
 * @file ql_mgtm_wifi.h
 * @brief Quectel WiFi interface function declarations.
 *
 * @note
 */
/*=============================================================================
  Copyright (c) 2018 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=============================================================================*/

/*============================================================================

                          EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  WHEN             WHO         WHAT, WHERE, WHY
  ------------     -------     ----------------------------------------
  11/20/2017       Mike        Initial creation.
  07/17/2018       Mike        Adds WiFi ap station connected/disconnected event.
  08/10/2018       Mike        Adds WiFi restore defualt configuration.
  08/11/2018       Mike        Adds WiFi Mac address acl.
  10/10/2018       Mike        Adds WiFi station connected/disconnected event.
============================================================================*/
#ifndef __QL_MGMT_WIFI_H__
#define __QL_MGMT_WIFI_H__

typedef enum {
	QL_MGMT_CLIENT_COMMAND_WIFI_WORK_MODE                          = 0x0001,
	QL_MGMT_CLIENT_COMMAND_WIFI_COUNTRY_CODE                       = 0x0002,
	QL_MGMT_CLIENT_COMMAND_WIFI_MODE                               = 0x0003,
	QL_MGMT_CLIENT_COMMAND_WIFI_CHANNEL                            = 0x0004,
	QL_MGMT_CLIENT_COMMAND_WIFI_SSID                               = 0x0005,
	QL_MGMT_CLIENT_COMMAND_WIFI_SSID_HIDE                          = 0x0006,
	QL_MGMT_CLIENT_COMMAND_WIFI_BANDWIDTH                          = 0x0007,
	QL_MGMT_CLIENT_COMMAND_WIFI_MAX_STA                            = 0x0008,
	QL_MGMT_CLIENT_COMMAND_WIFI_11N_ONLY                           = 0x0009,
	QL_MGMT_CLIENT_COMMAND_WIFI_11AC_ONLY                          = 0x000a,
	QL_MGMT_CLIENT_COMMAND_WIFI_AP_ISOLATE                         = 0x000b,
	QL_MGMT_CLIENT_COMMAND_WIFI_DFS                                = 0x000c,
	QL_MGMT_CLIENT_COMMAND_WIFI_AUTH                               = 0x000d,
	QL_MGMT_CLIENT_COMMAND_WIFI_RSSI                               = 0x000e,
	QL_MGMT_CLIENT_COMMAND_WIFI_STA_INFO                           = 0x000f,
	QL_MGMT_CLIENT_COMMAND_WIFI_ENABLE                             = 0x0010,
	QL_MGMT_CLIENT_COMMAND_WIFI_ACTIVE                             = 0x0011,
	QL_MGMT_CLIENT_COMMAND_WIFI_RESTORE_DEFAULT                    = 0x0012,
	QL_MGMT_CLIENT_COMMAND_WIFI_MAC_ACL_RULE                       = 0x0013,	
	QL_MGMT_CLIENT_COMMAND_WIFI_MAC_ACL_ADD                        = 0x0014,
	QL_MGMT_CLIENT_COMMAND_WIFI_MAC_ACL_DEL                        = 0x0015,
	QL_MGMT_CLIENT_COMMAND_WIFI_MAC_ACL_INFO                       = 0x0016,
	QL_MGMT_CLIENT_COMMAND_WIFI_STATION_STATUS                     = 0x0017,
} ql_mgmt_client_command_wifi_e;

#define QL_WIFI_MAX_STA_NUM 16

struct ql_mgmt_client_wifi_work_mode {
	/*
	 * WiFi work mode support AP, STA, AP+STA, AP-AP
	 */
	wifi_work_mode_e_v01 work_mode;
};

struct ql_mgmt_client_wifi_enable {
	bool state;
};

struct ql_mgmt_client_wifi_country_code {
	char country_code[2];
};

struct ql_mgmt_client_wifi_mode {
	wifi_index_e_v01 id;
	wifi_ieee80211_mode_e_v01 mode;
};

struct ql_mgmt_client_wifi_channel {
	wifi_index_e_v01 id;
	int channel;
};

struct ql_mgmt_client_wifi_essid {
	wifi_index_e_v01 id;
	char ssid[32];
};

struct ql_mgmt_client_wifi_ssid_hide {
	wifi_index_e_v01 id;
	bool state;
};

struct ql_mgmt_client_wifi_bandwidth {
	wifi_index_e_v01 id;
	wifi_bandwidth_e_v01 bandwidth;
};

struct ql_mgmt_client_wifi_max_sta {
	wifi_index_e_v01 id;
	int num;
};

struct ql_mgmt_client_wifi_11n_only {
	wifi_index_e_v01 id;
	bool state;
};

struct ql_mgmt_clinet_wifi_11ac_only {
	wifi_index_e_v01 id;
	bool state;
};

struct ql_mgmt_client_wifi_ap_isolate {
	wifi_index_e_v01 id;
	bool state;
};

struct ql_mgmt_client_wifi_dfs {
	wifi_index_e_v01 id;
	bool state;
};

struct ql_mgmt_client_wifi_auth {
	wifi_index_e_v01 id;
	wifi_auth_e_v01 auth;
	union {
		struct {
			int default_index;
			char password[4][64];
		} wep;
		struct {
			wifi_wpa_pairwise_e_v01 pairwise;
			char password[64];
			int group_rekey;
		} wpa_psk;
	};
};

struct ql_mgmt_client_wifi_rssi {
	wifi_index_e_v01 id;
	int rssi;
};

struct ql_mgmt_client_wifi_sta_info {
	int count;
	struct {
		int idx;
		int ip;
		char ipv6[16];
		char mac[6];
		char hostname[256];
		int uptime;           /* not Support */
		long long tx_bytes;   /* not Support */
		long long rx_bytes;   /* not Support */
	} sta[QL_WIFI_MAX_STA_NUM];
};

struct ql_mgmt_client_wifi_restore_default {
	int id;
};

struct ql_mgmt_client_wifi_ap_sta_info_event {
	wifi_ap_sta_action_e_v01 state;
	int idx;
	char mac[6];
};

struct ql_mgmt_client_wifi_mac_acl_rule {
	wifi_index_e_v01 id;
	wifi_mac_acl_rule_e_v01 rule;
};

struct ql_mgmt_client_wifi_mac_acl {
	wifi_index_e_v01 id;
	unsigned char mac[6];
};

struct ql_mgmt_client_wifi_mac_acls {
	wifi_index_e_v01 id;
	unsigned int cnt;
	unsigned char mac[WIFI_MAX_MAC_ACL_CNT_V01][6];
};

struct ql_mgmt_client_wifi_active {
	wifi_index_e_v01 id;
	unsigned char action_type;
};

struct ql_mgmt_client_wifi_station_status {
	wifi_index_e_v01 id;
	char ssid[33];
	unsigned char state;
};

typedef struct {
	union {
		struct ql_mgmt_client_wifi_work_mode work_mode;
		struct ql_mgmt_client_wifi_enable enable;
		struct ql_mgmt_client_wifi_country_code country_code;
		struct ql_mgmt_client_wifi_mode mode;
		struct ql_mgmt_client_wifi_channel channel;
		struct ql_mgmt_client_wifi_essid ssid;
		struct ql_mgmt_client_wifi_ssid_hide ssid_hide;
		struct ql_mgmt_client_wifi_bandwidth bandwidth;
		struct ql_mgmt_client_wifi_max_sta max_sta;
		struct ql_mgmt_client_wifi_11n_only only_11n;
		struct ql_mgmt_clinet_wifi_11ac_only only_11ac;
		struct ql_mgmt_client_wifi_ap_isolate ap_isolate;
		struct ql_mgmt_client_wifi_dfs dfs;
		struct ql_mgmt_client_wifi_auth auth;
		struct ql_mgmt_client_wifi_rssi rssi;
		struct ql_mgmt_client_wifi_sta_info sta_info;
		struct ql_mgmt_client_wifi_restore_default restore;
		struct ql_mgmt_client_wifi_mac_acl_rule mac_acl_rule;
		struct ql_mgmt_client_wifi_mac_acl mac_acl;
		struct ql_mgmt_client_wifi_mac_acls mac_acl_get;
		struct ql_mgmt_client_wifi_active active;
		struct ql_mgmt_client_wifi_station_status sta_status;
	};
} ql_mgmt_client_wifi_s;

#endif /* end of  __QL_MGMT_WIFI_H__ */
