/**
 * @file ql_mgtm_network.h
 * @brief Quectel Network interface function declarations.
 *
 * @note
 *
 * @copyright Copyright (c) 2009-2017 @ Quectel Wireless Solutions Co., Ltd.
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
  ------------     -------     -----------------------------------------------
  11/20/2017       Mike        Initial creation.
  10/22/2018       Mike        Adds ethernet mode.
==============================================================================*/
#ifndef __QL_MGMT_NETWORK_H__
#define __QL_MGMT_NETWORK_H__

typedef enum {
	QL_MGMT_CLIENT_COMMAND_NETWORK_DMZ_ADD            = 0x0001,
	QL_MGMT_CLIENT_COMMAND_NETWORK_DMZ_DEL            = 0x0002,
	QL_MGMT_CLIENT_COMMAND_NETWORK_DMZ_GET            = 0x0003,
	QL_MGMT_CLIENT_COMMAND_NETWORK_SGMII_ENABLE       = 0x0004,
	QL_MGMT_CLIENT_COMMAND_NETWORK_SGMII_CONFIG       = 0x0005,
	QL_MGMT_CLIENT_COMMAND_NETWORK_SGMII_INFO         = 0x0006,
	QL_MGMT_CLIENT_COMMAND_NETWORK_LAN_CONFIG         = 0x0007,
	QL_MGMT_CLIENT_COMMAND_NETWORK_SNAT_ADD           = 0x0008,
	QL_MGMT_CLIENT_COMMAND_NETWORK_SNAT_DEL           = 0x0009,
	QL_MGMT_CLIENT_COMMAND_NETWORK_SNAT_GET           = 0x000a,
	QL_MGMT_CLIENT_COMMAND_NETWORK_ETHERNET_MODE      = 0x000b,
} ql_mgmt_client_command_network_e;

struct network_dmz {
	unsigned int addr4;
};

struct network_sgmii_enable {
	bool state;
};

struct network_sgmii_config {
	/*
	 * if autoneg set true, the speed and duplex do not set.
	 * if set speed and duplex, the autoneg must be set false
	 */
	bool autoneg;
	char speed;
	char duplex;
};

struct network_sgmii_info {
	bool state;
	bool running;
	char speed;
	char duplex;
	bool autoneg;
	unsigned long long rx_pkts;
	unsigned long long rx_bytes;
	unsigned long long tx_pkts;
	unsigned long long tx_bytes;
}; 

struct network_lan_config {
	unsigned int gw_ip;
	unsigned int netmask;
	unsigned char enable_dhcp;
	unsigned int  dhcp_start_ip;
	unsigned int  dhcp_end_ip;
	unsigned int  lease_time;
};

struct network_snat {
	unsigned int private_ip;
	unsigned short private_port;
	unsigned short global_port;
	unsigned char protocol;
};

struct network_snat_info {
	unsigned int cnt;
	struct network_snat entry[QMAM_MAX_SNAT_CNT_V01];
};

struct network_ethernet_mode {
	unsigned int mode;
};

typedef struct {
	union {
		struct network_dmz dmz;
		struct network_sgmii_enable sgmii_enable;
		struct network_sgmii_config sgmii_config;
		struct network_sgmii_info sgmii_info;
		struct network_lan_config lan_config;
		struct network_snat snat;
		struct network_snat_info snat_info;
		struct network_ethernet_mode eth_mode;
	};
} ql_mgmt_client_network_s;

#endif /* end of __QL_MGMT_NETWORK_H__ */
