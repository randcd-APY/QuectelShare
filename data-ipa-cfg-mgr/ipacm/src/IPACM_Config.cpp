/*
 * Copyright (c) 2013-2018, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *    * Neither the name of The Linux Foundation nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*!
		@file
		IPACM_Config.cpp

		@brief
		This file implements the IPACM Configuration from XML file

		@Author
		Skylar Chang

*/
#include <IPACM_Config.h>
#include <IPACM_Log.h>
#include <IPACM_Iface.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <fcntl.h>
#include <string.h>

IPACM_Config *IPACM_Config::pInstance = NULL;
const char *IPACM_Config::DEVICE_NAME = "/dev/ipa";
const char *IPACM_Config::DEVICE_NAME_ODU = "/dev/odu_ipa_bridge";

#define __stringify(x...) #x

#ifdef FEATURE_IPA_ANDROID
#define IPACM_CONFIG_FILE "/etc/IPACM_cfg.xml"
#else
#define IPACM_CONFIG_FILE "/etc/data/ipa/IPACM_cfg.xml"
#endif

const char *ipacm_event_name[] = {
	__stringify(IPA_CFG_CHANGE_EVENT),                     /* NULL */
	__stringify(IPA_PRIVATE_SUBNET_CHANGE_EVENT),          /* ipacm_event_data_fid */
	__stringify(IPA_FIREWALL_CHANGE_EVENT),                /* NULL */
	__stringify(IPA_LINK_UP_EVENT),                        /* ipacm_event_data_fid */
	__stringify(IPA_LINK_DOWN_EVENT),                      /* ipacm_event_data_fid */
	__stringify(IPA_USB_LINK_UP_EVENT),                    /* ipacm_event_data_fid */
	__stringify(IPA_BRIDGE_LINK_UP_EVENT),                 /* ipacm_event_data_all */
	__stringify(IPA_WAN_EMBMS_LINK_UP_EVENT),              /* ipacm_event_data_mac */
	__stringify(IPA_ADDR_ADD_EVENT),                       /* ipacm_event_data_addr */
	__stringify(IPA_ADDR_DEL_EVENT),                       /* no use */
	__stringify(IPA_ROUTE_ADD_EVENT),                      /* ipacm_event_data_addr */
	__stringify(IPA_ROUTE_DEL_EVENT),                      /* ipacm_event_data_addr */
	__stringify(IPA_WAN_UPSTREAM_ROUTE_ADD_EVENT),         /* ipacm_event_data_fid */
	__stringify(IPA_WAN_UPSTREAM_ROUTE_DEL_EVENT),         /* ipacm_event_data_fid */
	__stringify(IPA_WLAN_AP_LINK_UP_EVENT),                /* ipacm_event_data_mac */
	__stringify(IPA_WLAN_STA_LINK_UP_EVENT),               /* ipacm_event_data_mac */
	__stringify(IPA_WLAN_LINK_DOWN_EVENT),                 /* ipacm_event_data_mac */
	__stringify(IPA_WLAN_CLIENT_ADD_EVENT),                /* ipacm_event_data_mac */
	__stringify(IPA_WLAN_CLIENT_ADD_EVENT_EX),             /* ipacm_event_data_wlan_ex */
	__stringify(IPA_WLAN_CLIENT_DEL_EVENT),                /* ipacm_event_data_mac */
	__stringify(IPA_WLAN_CLIENT_POWER_SAVE_EVENT),         /* ipacm_event_data_mac */
	__stringify(IPA_WLAN_CLIENT_RECOVER_EVENT),            /* ipacm_event_data_mac */
	__stringify(IPA_NEW_NEIGH_EVENT),                      /* ipacm_event_data_all */
	__stringify(IPA_DEL_NEIGH_EVENT),                      /* ipacm_event_data_all */
	__stringify(IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT),       /* ipacm_event_data_all */
	__stringify(IPA_NEIGH_CLIENT_IP_ADDR_DEL_EVENT),       /* ipacm_event_data_all */
	__stringify(IPA_SW_ROUTING_ENABLE),                    /* NULL */
	__stringify(IPA_SW_ROUTING_DISABLE),                   /* NULL */
	__stringify(IPA_PROCESS_CT_MESSAGE),                   /* ipacm_ct_evt_data */
	__stringify(IPA_PROCESS_CT_MESSAGE_V6),                /* ipacm_ct_evt_data */
	__stringify(IPA_LAN_TO_LAN_NEW_CONNECTION),            /* ipacm_event_connection */
	__stringify(IPA_LAN_TO_LAN_DEL_CONNECTION),            /* ipacm_event_connection */
	__stringify(IPA_WLAN_SWITCH_TO_SCC),                   /* No Data */
	__stringify(IPA_WLAN_SWITCH_TO_MCC),                   /* No Data */
	__stringify(IPA_CRADLE_WAN_MODE_SWITCH),               /* ipacm_event_cradle_wan_mode */
	__stringify(IPA_WAN_XLAT_CONNECT_EVENT),               /* ipacm_event_data_fid */
	__stringify(IPA_TETHERING_STATS_UPDATE_EVENT),         /* ipacm_event_data_fid */
	__stringify(IPA_NETWORK_STATS_UPDATE_EVENT),           /* ipacm_event_data_fid */
#ifdef FEATURE_IPACM_PER_CLIENT_STATS
	__stringify(IPA_LAN_CLIENT_CONNECT_EVENT),             /* ipacm_event_data_mac */
	__stringify(IPA_LAN_CLIENT_DISCONNECT_EVENT),          /* ipacm_event_data_mac */
	__stringify(IPA_LAN_CLIENT_UPDATE_EVENT),              /* ipacm_event_data_mac */
#endif
	__stringify(IPA_EXTERNAL_EVENT_MAX),
	__stringify(IPA_HANDLE_WAN_UP),                        /* ipacm_event_iface_up  */
	__stringify(IPA_HANDLE_WAN_DOWN),                      /* ipacm_event_iface_up  */
	__stringify(IPA_HANDLE_WAN_UP_V6),                     /* NULL */
	__stringify(IPA_HANDLE_WAN_DOWN_V6),                   /* NULL */
	__stringify(IPA_HANDLE_WAN_UP_TETHER),                 /* ipacm_event_iface_up_tehter */
	__stringify(IPA_HANDLE_WAN_DOWN_TETHER),               /* ipacm_event_iface_up_tehter */
	__stringify(IPA_HANDLE_WAN_UP_V6_TETHER),              /* ipacm_event_iface_up_tehter */
	__stringify(IPA_HANDLE_WAN_DOWN_V6_TETHER),            /* ipacm_event_iface_up_tehter */
	__stringify(IPA_HANDLE_LAN_WLAN_UP),                   /* ipacm_event_iface_up */
	__stringify(IPA_HANDLE_LAN_WLAN_UP_V6),                /* ipacm_event_iface_up */
	__stringify(IPA_ETH_BRIDGE_IFACE_UP),                  /* ipacm_event_eth_bridge*/
	__stringify(IPA_ETH_BRIDGE_IFACE_DOWN),                /* ipacm_event_eth_bridge*/
	__stringify(IPA_ETH_BRIDGE_CLIENT_ADD),                /* ipacm_event_eth_bridge*/
	__stringify(IPA_ETH_BRIDGE_CLIENT_DEL),                /* ipacm_event_eth_bridge*/
	__stringify(IPA_ETH_BRIDGE_WLAN_SCC_MCC_SWITCH),       /* ipacm_event_eth_bridge*/
	__stringify(IPA_LAN_DELETE_SELF),                      /* ipacm_event_data_fid */
#ifdef FEATURE_L2TP_E2E
	__stringify(IPA_ADD_L2TP_CLIENT),                      /* ipacm_event_data_all */
	__stringify(IPA_DEL_L2TP_CLIENT),                      /* ipacm_event_data_all */
#endif
#ifdef FEATURE_VLAN_MPDN
	__stringify(IPA_PREFIX_CHANGE_EVENT),                 /* ipacm_event_data_fid */
	__stringify(IPA_ROUTE_ADD_VLAN_PDN_EVENT),            /* ipacm_event_route_vlan */
	__stringify(IPA_HANDLE_WAN_VLAN_PDN_UP),                  /* ipacm_event_vlan_pdn */
	__stringify(IPA_HANDLE_WAN_VLAN_PDN_DOWN),                /* ipacm_event_vlan_pdn */
#endif
	__stringify(IPACM_EVENT_MAX),
};

IPACM_Config::IPACM_Config()
{
	iface_table = NULL;
	alg_table = NULL;
	pNatIfaces = NULL;
	memset(&ipa_client_rm_map_tbl, 0, sizeof(ipa_client_rm_map_tbl));
	memset(&ipa_rm_tbl, 0, sizeof(ipa_rm_tbl));
	ipa_rm_a2_check=0;
	ipacm_odu_enable = false;
#ifdef FEATURE_IPACM_PER_CLIENT_STATS
	ipacm_lan_stats_enable = false;
	ipacm_lan_stats_enable_set = false;
#endif
	ipacm_odu_router_mode = false;
	ipa_num_wlan_guest_ap = 0;

	ipa_num_ipa_interfaces = 0;
	ipa_num_private_subnet = 0;
	ipa_num_alg_ports = 0;
	ipa_nat_max_entries = 0;
	ipa_ipv6ct_max_entries = 0;
	ipa_nat_iface_entries = 0;
	ipa_sw_rt_enable = false;
	ipa_bridge_enable = false;
	isMCC_Mode = false;
	ipa_max_valid_rm_entry = 0;

	memset(&rt_tbl_default_v4, 0, sizeof(rt_tbl_default_v4));
	memset(&rt_tbl_lan_v4, 0, sizeof(rt_tbl_lan_v4));
	memset(&rt_tbl_wan_v4, 0, sizeof(rt_tbl_wan_v4));
	memset(&rt_tbl_v6, 0, sizeof(rt_tbl_v6));
	memset(&rt_tbl_wan_v6, 0, sizeof(rt_tbl_wan_v6));
	memset(&rt_tbl_wan_dl, 0, sizeof(rt_tbl_wan_dl));
	memset(&rt_tbl_odu_v4, 0, sizeof(rt_tbl_odu_v4));
	memset(&rt_tbl_odu_v6, 0, sizeof(rt_tbl_odu_v6));

	memset(&ext_prop_v4, 0, sizeof(ext_prop_v4));
	memset(&ext_prop_v6, 0, sizeof(ext_prop_v6));

	qmap_id = ~0;

	memset(flt_rule_count_v4, 0, IPA_CLIENT_MAX*sizeof(int));
	memset(flt_rule_count_v6, 0, IPA_CLIENT_MAX*sizeof(int));
	memset(bridge_mac, 0, IPA_MAC_ADDR_SIZE*sizeof(uint8_t));
#ifdef FEATURE_VLAN_MPDN
	num_ipv6_prefixes = 0;
	memset(ipa_ipv6_prefixes, 0, sizeof(ipa_ipv6_prefixes));
	memset(vlan_bridges, 0, IPA_MAX_NUM_BRIDGES * sizeof(vlan_bridges[0]));
	memset(vlan_devices, 0, IPA_VLAN_IF_MAX * sizeof(vlan_devices[0]));
#endif
#if defined(FEATURE_L2TP_E2E) || defined(FEATURE_L2TP) || defined(FEATURE_VLAN_MPDN)
	pthread_mutex_init(&vlan_l2tp_lock, NULL);
#endif
	IPACMDBG_H(" create IPACM_Config constructor\n");
	return;
}

int IPACM_Config::Init(void)
{
	/* Read IPACM Config file */
	char	IPACM_config_file[IPA_MAX_FILE_LEN];
	IPACM_conf_t	*cfg;
	cfg = (IPACM_conf_t *)malloc(sizeof(IPACM_conf_t));
	if(cfg == NULL)
	{
		IPACMERR("Unable to allocate cfg memory.\n");
		return IPACM_FAILURE;
	}
	uint32_t subnet_addr;
	uint32_t subnet_mask;
	int i, ret = IPACM_SUCCESS;
	struct in_addr in_addr_print;

	m_fd = open(DEVICE_NAME, O_RDWR);
	if (0 > m_fd)
	{
		IPACMERR("Failed opening %s.\n", DEVICE_NAME);
	}
	ver = GetIPAVer(true);
#ifdef FEATURE_VLAN_MPDN
	get_vlan_mode_ifaces();
#endif

	strlcpy(IPACM_config_file, IPACM_CONFIG_FILE, sizeof(IPACM_config_file));

	IPACMDBG_H("\n IPACM XML file is %s \n", IPACM_config_file);
	if (IPACM_SUCCESS == ipacm_read_cfg_xml(IPACM_config_file, cfg))
	{
		IPACMDBG_H("\n IPACM XML read OK \n");
	}
	else
	{
		IPACMERR("\n IPACM XML read failed \n");
		ret = IPACM_FAILURE;
		goto fail;
	}

	/* Construct IPACM Iface table */
	ipa_num_ipa_interfaces = cfg->iface_config.num_iface_entries;
	if (iface_table != NULL)
	{
		free(iface_table);
		iface_table = NULL;
		IPACMDBG_H("RESET IPACM_Config::iface_table\n");
	}
	iface_table = (ipa_ifi_dev_name_t *)calloc(ipa_num_ipa_interfaces,
					sizeof(ipa_ifi_dev_name_t));
	if(iface_table == NULL)
	{
		IPACMERR("Unable to allocate iface_table memory.\n");
		ret = IPACM_FAILURE;
		goto fail;
	}

	for (i = 0; i < cfg->iface_config.num_iface_entries; i++)
	{
		strlcpy(iface_table[i].iface_name, cfg->iface_config.iface_entries[i].iface_name, sizeof(iface_table[i].iface_name));
		iface_table[i].if_cat = cfg->iface_config.iface_entries[i].if_cat;
		iface_table[i].if_mode = cfg->iface_config.iface_entries[i].if_mode;
		iface_table[i].wlan_mode = cfg->iface_config.iface_entries[i].wlan_mode;
		IPACMDBG_H("IPACM_Config::iface_table[%d] = %s, cat=%d, mode=%d wlan-mode=%d \n", i, iface_table[i].iface_name,
				iface_table[i].if_cat, iface_table[i].if_mode, iface_table[i].wlan_mode);
		/* copy bridge interface name to ipacmcfg */
		if( iface_table[i].if_cat == VIRTUAL_IF)
		{
			strlcpy(ipa_virtual_iface_name, iface_table[i].iface_name, sizeof(ipa_virtual_iface_name));
			IPACMDBG_H("ipa_virtual_iface_name(%s) \n", ipa_virtual_iface_name);
		}
	}

	/* Construct IPACM Private_Subnet table */
	memset(&private_subnet_table, 0, sizeof(private_subnet_table));
	ipa_num_private_subnet = cfg->private_subnet_config.num_subnet_entries;

	for (i = 0; i < cfg->private_subnet_config.num_subnet_entries; i++)
	{
		memcpy(&private_subnet_table[i].subnet_addr,
					 &cfg->private_subnet_config.private_subnet_entries[i].subnet_addr,
					 sizeof(cfg->private_subnet_config.private_subnet_entries[i].subnet_addr));

		memcpy(&private_subnet_table[i].subnet_mask,
					 &cfg->private_subnet_config.private_subnet_entries[i].subnet_mask,
					 sizeof(cfg->private_subnet_config.private_subnet_entries[i].subnet_mask));

		subnet_addr = htonl(private_subnet_table[i].subnet_addr);
		memcpy(&in_addr_print,&subnet_addr,sizeof(in_addr_print));
		IPACMDBG_H("%dst::private_subnet_table= %s \n ", i,
						 inet_ntoa(in_addr_print));

		subnet_mask =  htonl(private_subnet_table[i].subnet_mask);
		memcpy(&in_addr_print,&subnet_mask,sizeof(in_addr_print));
		IPACMDBG_H("%dst::private_subnet_table= %s \n ", i,
						 inet_ntoa(in_addr_print));
	}

	/* Construct IPACM ALG table */
	ipa_num_alg_ports = cfg->alg_config.num_alg_entries;
	if (alg_table != NULL)
	{
		free(alg_table);
		alg_table = NULL;
		IPACMDBG_H("RESET IPACM_Config::alg_table \n");
	}
	alg_table = (ipacm_alg *)calloc(ipa_num_alg_ports,
				sizeof(ipacm_alg));
	if(alg_table == NULL)
	{
		IPACMERR("Unable to allocate alg_table memory.\n");
		ret = IPACM_FAILURE;
		free(iface_table);
		goto fail;;
	}
	for (i = 0; i < cfg->alg_config.num_alg_entries; i++)
	{
		alg_table[i].protocol = cfg->alg_config.alg_entries[i].protocol;
		alg_table[i].port = cfg->alg_config.alg_entries[i].port;
		IPACMDBG_H("IPACM_Config::ipacm_alg[%d] = %d, port=%d\n", i, alg_table[i].protocol, alg_table[i].port);
	}

	ipa_nat_max_entries = cfg->nat_max_entries;
	IPACMDBG_H("Nat Maximum Entries %d\n", ipa_nat_max_entries);

	if (cfg->ipv6ct_enable > 0)
	{
		ipa_ipv6ct_max_entries = (cfg->ipv6ct_max_entries > 0) ? cfg->ipv6ct_max_entries : DEFAULT_IPV6CT_MAX_ENTRIES;
		IPACMDBG_H("IPv6CT Maximum Entries %d\n", ipa_ipv6ct_max_entries);
	}
	else
	{
		ipa_ipv6ct_max_entries = 0;
		IPACMDBG_H("IPv6CT is disabled\n");
	}

	/* Find ODU is either router mode or bridge mode*/
	ipacm_odu_enable = cfg->odu_enable;
	ipacm_odu_router_mode = cfg->router_mode_enable;
	ipacm_odu_embms_enable = cfg->odu_embms_enable;
	IPACMDBG_H("ipacm_odu_enable %d\n", ipacm_odu_enable);
	IPACMDBG_H("ipacm_odu_mode %d\n", ipacm_odu_router_mode);
	IPACMDBG_H("ipacm_odu_embms_enable %d\n", ipacm_odu_embms_enable);

	ipacm_ip_passthrough_mode = cfg->ip_passthrough_mode;
	IPACMDBG_H("ipacm_ip_passthrough_mode %d. \n", ipacm_ip_passthrough_mode);

	memcpy(ipacm_ip_passthrough_mac, cfg->ip_passthrough_mac.ether_addr_octet, IPA_MAC_ADDR_SIZE);

#ifdef FEATURE_IPACM_PER_CLIENT_STATS
	if (!ipacm_lan_stats_enable_set)
	{
		/* Read the configuration only once. */
		ipacm_lan_stats_enable = cfg->lan_stats_enable;
		ipacm_lan_stats_enable_set = true;
		IPACMDBG_H("ipacm_lan_stats_enable %d. \n", ipacm_lan_stats_enable);
	}
#endif

	ipa_num_wlan_guest_ap = cfg->num_wlan_guest_ap;
	IPACMDBG_H("ipa_num_wlan_guest_ap %d\n",ipa_num_wlan_guest_ap);

	/* Allocate more non-nat entries if the monitored iface dun have Tx/Rx properties */
	if (pNatIfaces != NULL)
	{
		free(pNatIfaces);
		pNatIfaces = NULL;
		IPACMDBG_H("RESET IPACM_Config::pNatIfaces \n");
	}
	ipa_nat_iface_entries = 0;
	pNatIfaces = (NatIfaces *)calloc(ipa_num_ipa_interfaces, sizeof(NatIfaces));
	if (pNatIfaces == NULL)
	{
		IPACMERR("unable to allocate nat ifaces\n");
		ret = IPACM_FAILURE;
		free(iface_table);
		free(alg_table);
		goto fail;
	}

	/* Construct the routing table ictol name in iface static member*/
	rt_tbl_default_v4.ip = IPA_IP_v4;
	strlcpy(rt_tbl_default_v4.name, V4_DEFAULT_ROUTE_TABLE_NAME, sizeof(rt_tbl_default_v4.name));

	rt_tbl_lan_v4.ip = IPA_IP_v4;
	strlcpy(rt_tbl_lan_v4.name, V4_LAN_ROUTE_TABLE_NAME, sizeof(rt_tbl_lan_v4.name));

	rt_tbl_wan_v4.ip = IPA_IP_v4;
	strlcpy(rt_tbl_wan_v4.name, V4_WAN_ROUTE_TABLE_NAME, sizeof(rt_tbl_wan_v4.name));

	rt_tbl_v6.ip = IPA_IP_v6;
	strlcpy(rt_tbl_v6.name, V6_COMMON_ROUTE_TABLE_NAME, sizeof(rt_tbl_v6.name));

	rt_tbl_wan_v6.ip = IPA_IP_v6;
	strlcpy(rt_tbl_wan_v6.name, V6_WAN_ROUTE_TABLE_NAME, sizeof(rt_tbl_wan_v6.name));

	rt_tbl_odu_v4.ip = IPA_IP_v4;
	strlcpy(rt_tbl_odu_v4.name, V4_ODU_ROUTE_TABLE_NAME, sizeof(rt_tbl_odu_v4.name));

	rt_tbl_odu_v6.ip = IPA_IP_v6;
	strlcpy(rt_tbl_odu_v6.name, V6_ODU_ROUTE_TABLE_NAME, sizeof(rt_tbl_odu_v6.name));

	rt_tbl_wan_dl.ip = IPA_IP_MAX;
	strlcpy(rt_tbl_wan_dl.name, WAN_DL_ROUTE_TABLE_NAME, sizeof(rt_tbl_wan_dl.name));

	/* Construct IPACM ipa_client map to rm_resource table */
	ipa_client_rm_map_tbl[IPA_CLIENT_WLAN1_PROD]= IPA_RM_RESOURCE_WLAN_PROD;
	ipa_client_rm_map_tbl[IPA_CLIENT_USB_PROD]= IPA_RM_RESOURCE_USB_PROD;
	ipa_client_rm_map_tbl[IPA_CLIENT_A5_WLAN_AMPDU_PROD]= IPA_RM_RESOURCE_HSIC_PROD;
	ipa_client_rm_map_tbl[IPA_CLIENT_A2_EMBEDDED_PROD]= IPA_RM_RESOURCE_Q6_PROD;
	ipa_client_rm_map_tbl[IPA_CLIENT_A2_TETHERED_PROD]= IPA_RM_RESOURCE_Q6_PROD;
	ipa_client_rm_map_tbl[IPA_CLIENT_APPS_LAN_WAN_PROD]= IPA_RM_RESOURCE_Q6_PROD;
	ipa_client_rm_map_tbl[IPA_CLIENT_WLAN1_CONS]= IPA_RM_RESOURCE_WLAN_CONS;
	ipa_client_rm_map_tbl[IPA_CLIENT_WLAN2_CONS]= IPA_RM_RESOURCE_WLAN_CONS;
	ipa_client_rm_map_tbl[IPA_CLIENT_WLAN3_CONS]= IPA_RM_RESOURCE_WLAN_CONS;
	ipa_client_rm_map_tbl[IPA_CLIENT_WLAN4_CONS]= IPA_RM_RESOURCE_WLAN_CONS;
	ipa_client_rm_map_tbl[IPA_CLIENT_USB_CONS]= IPA_RM_RESOURCE_USB_CONS;
	ipa_client_rm_map_tbl[IPA_CLIENT_A2_EMBEDDED_CONS]= IPA_RM_RESOURCE_Q6_CONS;
	ipa_client_rm_map_tbl[IPA_CLIENT_A2_TETHERED_CONS]= IPA_RM_RESOURCE_Q6_CONS;
	ipa_client_rm_map_tbl[IPA_CLIENT_APPS_WAN_CONS]= IPA_RM_RESOURCE_Q6_CONS;
	ipa_client_rm_map_tbl[IPA_CLIENT_ODU_PROD]= IPA_RM_RESOURCE_ODU_ADAPT_PROD;
	ipa_client_rm_map_tbl[IPA_CLIENT_ODU_EMB_CONS]= IPA_RM_RESOURCE_ODU_ADAPT_CONS;
	ipa_client_rm_map_tbl[IPA_CLIENT_ODU_TETH_CONS]= IPA_RM_RESOURCE_ODU_ADAPT_CONS;
	ipa_client_rm_map_tbl[IPA_CLIENT_ETHERNET_PROD]= IPA_RM_RESOURCE_ETHERNET_PROD;
	ipa_client_rm_map_tbl[IPA_CLIENT_ETHERNET_CONS]= IPA_RM_RESOURCE_ETHERNET_CONS;

	/* Create the entries which IPACM wants to add dependencies on */
	ipa_rm_tbl[0].producer_rm1 = IPA_RM_RESOURCE_WLAN_PROD;
	ipa_rm_tbl[0].consumer_rm1 = IPA_RM_RESOURCE_Q6_CONS;
	ipa_rm_tbl[0].producer_rm2 = IPA_RM_RESOURCE_Q6_PROD;
	ipa_rm_tbl[0].consumer_rm2 = IPA_RM_RESOURCE_WLAN_CONS;

	ipa_rm_tbl[1].producer_rm1 = IPA_RM_RESOURCE_USB_PROD;
	ipa_rm_tbl[1].consumer_rm1 = IPA_RM_RESOURCE_Q6_CONS;
	ipa_rm_tbl[1].producer_rm2 = IPA_RM_RESOURCE_Q6_PROD;
	ipa_rm_tbl[1].consumer_rm2 = IPA_RM_RESOURCE_USB_CONS;

	ipa_rm_tbl[2].producer_rm1 = IPA_RM_RESOURCE_WLAN_PROD;
	ipa_rm_tbl[2].consumer_rm1 = IPA_RM_RESOURCE_USB_CONS;
	ipa_rm_tbl[2].producer_rm2 = IPA_RM_RESOURCE_USB_PROD;
	ipa_rm_tbl[2].consumer_rm2 = IPA_RM_RESOURCE_WLAN_CONS;

	ipa_rm_tbl[3].producer_rm1 = IPA_RM_RESOURCE_ODU_ADAPT_PROD;
	ipa_rm_tbl[3].consumer_rm1 = IPA_RM_RESOURCE_Q6_CONS;
	ipa_rm_tbl[3].producer_rm2 = IPA_RM_RESOURCE_Q6_PROD;
	ipa_rm_tbl[3].consumer_rm2 = IPA_RM_RESOURCE_ODU_ADAPT_CONS;

	ipa_rm_tbl[4].producer_rm1 = IPA_RM_RESOURCE_WLAN_PROD;
	ipa_rm_tbl[4].consumer_rm1 = IPA_RM_RESOURCE_ODU_ADAPT_CONS;
	ipa_rm_tbl[4].producer_rm2 = IPA_RM_RESOURCE_ODU_ADAPT_PROD;
	ipa_rm_tbl[4].consumer_rm2 = IPA_RM_RESOURCE_WLAN_CONS;

	ipa_rm_tbl[5].producer_rm1 = IPA_RM_RESOURCE_ODU_ADAPT_PROD;
	ipa_rm_tbl[5].consumer_rm1 = IPA_RM_RESOURCE_USB_CONS;
	ipa_rm_tbl[5].producer_rm2 = IPA_RM_RESOURCE_USB_PROD;
	ipa_rm_tbl[5].consumer_rm2 = IPA_RM_RESOURCE_ODU_ADAPT_CONS;

	ipa_rm_tbl[6].producer_rm1 = IPA_RM_RESOURCE_ETHERNET_PROD;
	ipa_rm_tbl[6].consumer_rm1 = IPA_RM_RESOURCE_Q6_CONS;
	ipa_rm_tbl[6].producer_rm2 = IPA_RM_RESOURCE_Q6_PROD;
	ipa_rm_tbl[6].consumer_rm2 = IPA_RM_RESOURCE_ETHERNET_CONS;

	ipa_rm_tbl[7].producer_rm1 = IPA_RM_RESOURCE_ETHERNET_PROD;
	ipa_rm_tbl[7].consumer_rm1 = IPA_RM_RESOURCE_USB_CONS;
	ipa_rm_tbl[7].producer_rm2 = IPA_RM_RESOURCE_USB_PROD;
	ipa_rm_tbl[7].consumer_rm2 = IPA_RM_RESOURCE_ETHERNET_CONS;

	ipa_rm_tbl[8].producer_rm1 = IPA_RM_RESOURCE_WLAN_PROD;
	ipa_rm_tbl[8].consumer_rm1 = IPA_RM_RESOURCE_ETHERNET_CONS;
	ipa_rm_tbl[8].producer_rm2 = IPA_RM_RESOURCE_ETHERNET_PROD;
	ipa_rm_tbl[8].consumer_rm2 = IPA_RM_RESOURCE_WLAN_CONS;
	ipa_max_valid_rm_entry = 9; /* max is IPA_MAX_RM_ENTRY (9)*/

	IPACMDBG_H(" depend MAP-0 rm index %d to rm index: %d \n", IPA_RM_RESOURCE_WLAN_PROD, IPA_RM_RESOURCE_Q6_CONS);
	IPACMDBG_H(" depend MAP-1 rm index %d to rm index: %d \n", IPA_RM_RESOURCE_USB_PROD, IPA_RM_RESOURCE_Q6_CONS);
	IPACMDBG_H(" depend MAP-2 rm index %d to rm index: %d \n", IPA_RM_RESOURCE_WLAN_PROD, IPA_RM_RESOURCE_USB_CONS);
	IPACMDBG_H(" depend MAP-3 rm index %d to rm index: %d \n", IPA_RM_RESOURCE_ODU_ADAPT_PROD, IPA_RM_RESOURCE_Q6_CONS);
	IPACMDBG_H(" depend MAP-4 rm index %d to rm index: %d \n", IPA_RM_RESOURCE_WLAN_PROD, IPA_RM_RESOURCE_ODU_ADAPT_CONS);
	IPACMDBG_H(" depend MAP-5 rm index %d to rm index: %d \n", IPA_RM_RESOURCE_ODU_ADAPT_PROD, IPA_RM_RESOURCE_USB_CONS);
	IPACMDBG_H(" depend MAP-6 rm index %d to rm index: %d \n", IPA_RM_RESOURCE_ETHERNET_PROD, IPA_RM_RESOURCE_Q6_CONS);
	IPACMDBG_H(" depend MAP-7 rm index %d to rm index: %d \n", IPA_RM_RESOURCE_ETHERNET_PROD, IPA_RM_RESOURCE_USB_CONS);
	IPACMDBG_H(" depend MAP-8 rm index %d to rm index: %d \n", IPA_RM_RESOURCE_WLAN_PROD, IPA_RM_RESOURCE_ETHERNET_CONS);

fail:
	if (cfg != NULL)
	{
		free(cfg);
		cfg = NULL;
	}

	return ret;
}

IPACM_Config* IPACM_Config::GetInstance()
{
	int res = IPACM_SUCCESS;

	if (pInstance == NULL)
	{
		pInstance = new IPACM_Config();

		res = pInstance->Init();
		if (res != IPACM_SUCCESS)
		{
			delete pInstance;
			IPACMERR("unable to initialize config instance\n");
			return NULL;
		}
	}

	return pInstance;
}

int IPACM_Config::GetAlgPorts(int nPorts, ipacm_alg *pAlgPorts)
{
	if (nPorts <= 0 || pAlgPorts == NULL)
	{
		IPACMERR("Invalid input\n");
		return -1;
	}

	for (int cnt = 0; cnt < nPorts; cnt++)
	{
		pAlgPorts[cnt].protocol = alg_table[cnt].protocol;
		pAlgPorts[cnt].port = alg_table[cnt].port;
	}

	return 0;
}

int IPACM_Config::GetNatIfaces(int nIfaces, NatIfaces *pIfaces)
{
	if (nIfaces <= 0 || pIfaces == NULL)
	{
		IPACMERR("Invalid input\n");
		return -1;
	}

	for (int cnt=0; cnt<nIfaces; cnt++)
	{
		memcpy(pIfaces[cnt].iface_name,
					 pNatIfaces[cnt].iface_name,
					 sizeof(pIfaces[cnt].iface_name));
	}

	return 0;
}


int IPACM_Config::AddNatIfaces(char *dev_name)
{
	int i;
	/* Check if this iface already in NAT-iface*/
	for(i = 0; i < ipa_nat_iface_entries; i++)
	{
		if(strncmp(dev_name,
							 pNatIfaces[i].iface_name,
							 sizeof(pNatIfaces[i].iface_name)) == 0)
		{
			IPACMDBG("Interface (%s) is add to nat iface already\n", dev_name);
				return 0;
		}
	}

	IPACMDBG_H("Add iface %s to NAT-ifaces, origin it has %d nat ifaces\n",
					          dev_name, ipa_nat_iface_entries);
	ipa_nat_iface_entries++;

	if (ipa_nat_iface_entries < ipa_num_ipa_interfaces)
	{
		strlcpy(pNatIfaces[ipa_nat_iface_entries - 1].iface_name,dev_name,
				IPA_IFACE_NAME_LEN);
		IPACMDBG_H("Add Nat IfaceName: %s ,update nat-ifaces number: %d\n",
				pNatIfaces[ipa_nat_iface_entries - 1].iface_name,
				ipa_nat_iface_entries);
	}

	return 0;
}

int IPACM_Config::DelNatIfaces(char *dev_name)
{
	int i = 0;
	IPACMDBG_H("Del iface %s from NAT-ifaces, origin it has %d nat ifaces\n",
					 dev_name, ipa_nat_iface_entries);

	for (i = 0; i < ipa_nat_iface_entries; i++)
	{
		if (strcmp(dev_name, pNatIfaces[i].iface_name) == 0)
		{
			IPACMDBG_H("Find Nat IfaceName: %s ,previous nat-ifaces number: %d\n",
							 pNatIfaces[i].iface_name, ipa_nat_iface_entries);

			/* Reset the matched entry */
			memset(pNatIfaces[i].iface_name, 0, IPA_IFACE_NAME_LEN);

			for (; i < ipa_nat_iface_entries - 1; i++)
			{
				memcpy(pNatIfaces[i].iface_name,
							 pNatIfaces[i + 1].iface_name, IPA_IFACE_NAME_LEN);

				/* Reset the copied entry */
				memset(pNatIfaces[i + 1].iface_name, 0, IPA_IFACE_NAME_LEN);
			}
			ipa_nat_iface_entries--;
			IPACMDBG_H("Update nat-ifaces number: %d\n", ipa_nat_iface_entries);
			return 0;
		}
	}

	IPACMDBG_H("Can't find Nat IfaceName: %s with total nat-ifaces number: %d\n",
					    dev_name, ipa_nat_iface_entries);
	return 0;
}

/* for IPACM resource manager dependency usage
   add either Tx or Rx ipa_rm_resource_name and
   also indicate that endpoint property if valid */
void IPACM_Config::AddRmDepend(ipa_rm_resource_name rm1,bool rx_bypass_ipa)
{
	int retval = 0;
	struct ipa_ioc_rm_dependency dep;

	IPACMDBG_H(" Got rm add-depend index : %d \n", rm1);
	/* ipa_rm_a2_check: IPA_RM_RESOURCE_Q6_CONS*/
	if(rm1 == IPA_RM_RESOURCE_Q6_CONS)
	{
		ipa_rm_a2_check+=1;
		IPACMDBG_H("got %d times default RT routing from A2 \n", ipa_rm_a2_check);
	}

	for(int i=0;i<ipa_max_valid_rm_entry;i++)
	{
		if(rm1 == ipa_rm_tbl[i].producer_rm1)
		{
			ipa_rm_tbl[i].producer1_up = true;
			/* entry1's producer actually dun have registered Rx-property */
			ipa_rm_tbl[i].rx_bypass_ipa = rx_bypass_ipa;
			IPACMDBG_H("Matched RM_table entry: %d's producer_rm1 with non_rx_prop: %d \n", i,ipa_rm_tbl[i].rx_bypass_ipa);

			if(ipa_rm_tbl[i].consumer1_up == true && ipa_rm_tbl[i].rm_set == false)
			{
				IPACMDBG_H("SETUP RM_table entry %d's bi-direction dependency  \n", i);
				/* add bi-directional dependency*/
				if(ipa_rm_tbl[i].rx_bypass_ipa)
				{
					IPACMDBG_H("Skip ADD entry %d's dependency between WLAN-Pro: %d, Con: %d \n", i, ipa_rm_tbl[i].producer_rm1,ipa_rm_tbl[i].consumer_rm1);
				}
				else
				{
					memset(&dep, 0, sizeof(dep));
					dep.resource_name = ipa_rm_tbl[i].producer_rm1;
					dep.depends_on_name = ipa_rm_tbl[i].consumer_rm1;
					retval = ioctl(m_fd, IPA_IOC_RM_ADD_DEPENDENCY, &dep);
					IPACMDBG_H("ADD entry %d's dependency between Pro: %d, Con: %d \n", i,dep.resource_name,dep.depends_on_name);
					if (retval)
					{
						IPACMERR("Failed adding dependecny for RM_table entry %d's bi-direction dependency (error:%d) \n", i,retval);
					}
				}
				memset(&dep, 0, sizeof(dep));
				dep.resource_name = ipa_rm_tbl[i].producer_rm2;
				dep.depends_on_name = ipa_rm_tbl[i].consumer_rm2;
				retval = ioctl(m_fd, IPA_IOC_RM_ADD_DEPENDENCY, &dep);
				IPACMDBG_H("ADD entry %d's dependency between Pro: %d, Con: %d \n", i,dep.resource_name,dep.depends_on_name);
				if (retval)
				{
					IPACMERR("Failed adding dependecny for RM_table entry %d's bi-direction dependency (error:%d)  \n", i,retval);
				}
				ipa_rm_tbl[i].rm_set = true;
			}
			else
			{
				IPACMDBG_H("Not SETUP RM_table entry %d: prod_up:%d, cons_up:%d, rm_set: %d \n", i,ipa_rm_tbl[i].producer1_up, ipa_rm_tbl[i].consumer1_up, ipa_rm_tbl[i].rm_set);
			}
		}

		if(rm1 == ipa_rm_tbl[i].consumer_rm1)
		{
			ipa_rm_tbl[i].consumer1_up = true;
			IPACMDBG_H("Matched RM_table entry: %d's consumer_rm1 \n", i);

			if(ipa_rm_tbl[i].producer1_up == true && ipa_rm_tbl[i].rm_set == false)
			{
				IPACMDBG_H("SETUP RM_table entry %d's bi-direction dependency  \n", i);
				/* add bi-directional dependency*/
				if(ipa_rm_tbl[i].rx_bypass_ipa)
				{
					IPACMDBG_H("Skip ADD entry %d's dependency between WLAN-Pro: %d, Con: %d \n", i, ipa_rm_tbl[i].producer_rm1,ipa_rm_tbl[i].consumer_rm1);
				}
				else
				{
					memset(&dep, 0, sizeof(dep));
					dep.resource_name = ipa_rm_tbl[i].producer_rm1;
					dep.depends_on_name = ipa_rm_tbl[i].consumer_rm1;
					retval = ioctl(m_fd, IPA_IOC_RM_ADD_DEPENDENCY, &dep);
					IPACMDBG_H("ADD entry %d's dependency between Pro: %d, Con: %d \n", i,dep.resource_name,dep.depends_on_name);
					if (retval)
					{
						IPACMERR("Failed adding dependecny for RM_table entry %d's bi-direction dependency (error:%d)  \n", i,retval);
					}
				}

				memset(&dep, 0, sizeof(dep));
				dep.resource_name = ipa_rm_tbl[i].producer_rm2;
				dep.depends_on_name = ipa_rm_tbl[i].consumer_rm2;
				retval = ioctl(m_fd, IPA_IOC_RM_ADD_DEPENDENCY, &dep);
				IPACMDBG_H("ADD entry %d's dependency between Pro: %d, Con: %d \n", i,dep.resource_name,dep.depends_on_name);
				if (retval)
				{
					IPACMERR("Failed adding dependecny for RM_table entry %d's bi-direction dependency (error:%d)  \n", i,retval);
				}
				ipa_rm_tbl[i].rm_set = true;
			}
			else
			{
				IPACMDBG_H("Not SETUP RM_table entry %d: prod_up:%d, cons_up:%d, rm_set: %d \n", i,ipa_rm_tbl[i].producer1_up, ipa_rm_tbl[i].consumer1_up, ipa_rm_tbl[i].rm_set);
			}
	   }
   }
   return ;
}

/* for IPACM resource manager dependency usage
   delete either Tx or Rx ipa_rm_resource_name */

void IPACM_Config::DelRmDepend(ipa_rm_resource_name rm1)
{
	int retval = 0;
	struct ipa_ioc_rm_dependency dep;

	IPACMDBG_H(" Got rm del-depend index : %d \n", rm1);
	/* ipa_rm_a2_check: IPA_RM_RESOURCE_Q6_CONS*/
	if(rm1 == IPA_RM_RESOURCE_Q6_CONS)
	{
		ipa_rm_a2_check-=1;
		IPACMDBG_H("Left %d times default RT routing from A2 \n", ipa_rm_a2_check);
	}

	for(int i=0;i<ipa_max_valid_rm_entry;i++)
	{

		if(rm1 == ipa_rm_tbl[i].producer_rm1)
		{
			if(ipa_rm_tbl[i].rm_set == true)
			{
				IPACMDBG_H("Matched RM_table entry: %d's producer_rm1 and dependency is up \n", i);
				ipa_rm_tbl[i].rm_set = false;

				/* delete bi-directional dependency*/
				if(ipa_rm_tbl[i].rx_bypass_ipa)
				{
					IPACMDBG_H("Skip DEL entry %d's dependency between WLAN-Pro: %d, Con: %d \n", i, ipa_rm_tbl[i].producer_rm1,ipa_rm_tbl[i].consumer_rm1);
				}
				else
				{
					memset(&dep, 0, sizeof(dep));
					dep.resource_name = ipa_rm_tbl[i].producer_rm1;
					dep.depends_on_name = ipa_rm_tbl[i].consumer_rm1;
					retval = ioctl(m_fd, IPA_IOC_RM_DEL_DEPENDENCY, &dep);
					IPACMDBG_H("Delete entry %d's dependency between Pro: %d, Con: %d \n", i,dep.resource_name,dep.depends_on_name);
					if (retval)
					{
						IPACMERR("Failed deleting dependecny for RM_table entry %d's bi-direction dependency (error:%d) \n", i,retval);
					}
				}
				memset(&dep, 0, sizeof(dep));
				dep.resource_name = ipa_rm_tbl[i].producer_rm2;
				dep.depends_on_name = ipa_rm_tbl[i].consumer_rm2;
				retval = ioctl(m_fd, IPA_IOC_RM_DEL_DEPENDENCY, &dep);
				IPACMDBG_H("Delete entry %d's dependency between Pro: %d, Con: %d \n", i,dep.resource_name,dep.depends_on_name);
				if (retval)
				{
					IPACMERR("Failed deleting dependecny for RM_table entry %d's bi-direction dependency (error:%d) \n", i,retval);
				}
			}
			ipa_rm_tbl[i].producer1_up = false;
			ipa_rm_tbl[i].rx_bypass_ipa = false;
		}
		if(rm1 == ipa_rm_tbl[i].consumer_rm1)
		{
			/* ipa_rm_a2_check: IPA_RM_RESOURCE_!6_CONS*/
			if(ipa_rm_tbl[i].consumer_rm1 == IPA_RM_RESOURCE_Q6_CONS && ipa_rm_a2_check == 1)
			{
				IPACMDBG_H(" still have %d default RT routing from A2 \n", ipa_rm_a2_check);
				continue;
			}

			if(ipa_rm_tbl[i].rm_set == true)
			{
				IPACMDBG_H("Matched RM_table entry: %d's consumer_rm1 and dependency is up \n", i);
				ipa_rm_tbl[i].rm_set = false;
				/* delete bi-directional dependency*/
				if(ipa_rm_tbl[i].rx_bypass_ipa)
				{
					IPACMDBG_H("Skip DEL entry %d's dependency between WLAN-Pro: %d, Con: %d \n", i, ipa_rm_tbl[i].producer_rm1,ipa_rm_tbl[i].consumer_rm1);
				}
				else
				{
					memset(&dep, 0, sizeof(dep));
					dep.resource_name = ipa_rm_tbl[i].producer_rm1;
					dep.depends_on_name = ipa_rm_tbl[i].consumer_rm1;
					retval = ioctl(m_fd, IPA_IOC_RM_DEL_DEPENDENCY, &dep);
					IPACMDBG_H("Delete entry %d's dependency between Pro: %d, Con: %d \n", i,dep.resource_name,dep.depends_on_name);
					if (retval)
					{
						IPACMERR("Failed deleting dependecny for RM_table entry %d's bi-direction dependency (error:%d) \n", i,retval);
					}
				}

				memset(&dep, 0, sizeof(dep));
				dep.resource_name = ipa_rm_tbl[i].producer_rm2;
				dep.depends_on_name = ipa_rm_tbl[i].consumer_rm2;
				retval = ioctl(m_fd, IPA_IOC_RM_DEL_DEPENDENCY, &dep);
				IPACMDBG_H("Delete entry %d's dependency between Pro: %d, Con: %d \n", i,dep.resource_name,dep.depends_on_name);
				if (retval)
				{
					IPACMERR("Failed deleting dependecny for RM_table entry %d's bi-direction dependency (error:%d) \n", i,retval);
				}
			}
			ipa_rm_tbl[i].consumer1_up = false;
		}
	}
	return ;
}

int IPACM_Config::SetExtProp(ipa_ioc_query_intf_ext_props *prop)
{
	int i, num;

	if(prop == NULL || prop->num_ext_props <= 0)
	{
		IPACMERR("There is no extended property!\n");
		return IPACM_FAILURE;
	}

	num = prop->num_ext_props;
	for(i=0; i<num; i++)
	{
		if(prop->ext[i].ip == IPA_IP_v4)
		{
			if(ext_prop_v4.num_ext_props >= MAX_NUM_EXT_PROPS)
			{
				IPACMERR("IPv4 extended property table is full!\n");
				continue;
			}
			memcpy(&ext_prop_v4.prop[ext_prop_v4.num_ext_props], &prop->ext[i], sizeof(struct ipa_ioc_ext_intf_prop));
			ext_prop_v4.num_ext_props++;
		}
		else if(prop->ext[i].ip == IPA_IP_v6)
		{
			if(ext_prop_v6.num_ext_props >= MAX_NUM_EXT_PROPS)
			{
				IPACMERR("IPv6 extended property table is full!\n");
				continue;
			}
			memcpy(&ext_prop_v6.prop[ext_prop_v6.num_ext_props], &prop->ext[i], sizeof(struct ipa_ioc_ext_intf_prop));
			ext_prop_v6.num_ext_props++;
		}
		else
		{
			IPACMERR("The IP type is not expected!\n");
			return IPACM_FAILURE;
		}
	}

	IPACMDBG_H("Set extended property succeeded.\n");

	return IPACM_SUCCESS;
}

ipacm_ext_prop* IPACM_Config::GetExtProp(ipa_ip_type ip_type)
{
	if(ip_type == IPA_IP_v4)
		return &ext_prop_v4;
	else if(ip_type == IPA_IP_v6)
		return &ext_prop_v6;
	else
	{
		IPACMERR("Failed to get extended property: the IP version is neither IPv4 nor IPv6!\n");
		return NULL;
	}
}

int IPACM_Config::DelExtProp(ipa_ip_type ip_type)
{
	if(ip_type != IPA_IP_v6)
	{
		memset(&ext_prop_v4, 0, sizeof(ext_prop_v4));
	}

	if(ip_type != IPA_IP_v4)
	{
		memset(&ext_prop_v6, 0, sizeof(ext_prop_v6));
	}

	return IPACM_SUCCESS;
}

const char* IPACM_Config::getEventName(ipa_cm_event_id event_id)
{
	if(event_id >= sizeof(ipacm_event_name)/sizeof(ipacm_event_name[0]))
	{
		IPACMERR("Event name array is not consistent with event array!\n");
		return NULL;
	}

	return ipacm_event_name[event_id];
}

enum ipa_hw_type IPACM_Config::GetIPAVer(bool get)
{
	int ret;

	if(!get)
		return ver;

	ret = ioctl(m_fd, IPA_IOC_GET_HW_VERSION, &ver);
	if(ret != 0)
	{
		IPACMERR("Failed to get IPA version with error %d.\n", ret);
		ver = IPA_HW_None;
		return IPA_HW_None;
	}
	IPACMDBG_H("IPA version is %d.\n", ver);
	return ver;
}

#ifdef FEATURE_VLAN_MPDN
void IPACM_Config::add_bridge_vlan_mapping(ipa_ioc_bridge_vlan_mapping_info *data)
{
	list<bridge_vlan_mapping_info>::iterator it_mapping;
	bridge_vlan_mapping_info new_mapping;
	ipacm_bridge *bridge = NULL;

	if(pthread_mutex_lock(&vlan_l2tp_lock) != 0)
	{
		IPACMERR("Unable to lock the mutex\n");
		return;
	}

	IPACMDBG_H("trying to add bridge %s -> VID %d mapping, subnet 0x%X & 0x%X\n",
		data->bridge_name,
		data->vlan_id,
		data->bridge_ipv4,
		data->subnet_mask);

	for(it_mapping = m_bridge_vlan_mapping.begin(); it_mapping != m_bridge_vlan_mapping.end(); it_mapping++)
	{
		if(strncmp(data->bridge_name, it_mapping->bridge_iface_name, sizeof(data->bridge_name)) == 0)
		{
			IPACMERR("The bridge %s was added before with vlan id %d\n", data->bridge_name,
				it_mapping->bridge_associated_VID);
			goto fail;
		}
	}

	memset(&new_mapping, 0, sizeof(new_mapping));
	strlcpy(new_mapping.bridge_iface_name, data->bridge_name,
		sizeof(new_mapping.bridge_iface_name));
	new_mapping.bridge_associated_VID = data->vlan_id;
	new_mapping.bridge_ipv4 = data->bridge_ipv4;
	new_mapping.subnet_mask = data->subnet_mask;

	m_bridge_vlan_mapping.push_front(new_mapping);
	IPACMDBG_H("added bridge %s with VID %d\n", data->bridge_name, data->vlan_id);

	pthread_mutex_unlock(&vlan_l2tp_lock);

	bridge = get_vlan_bridge(data->bridge_name);
	if(bridge)
	{
		IPACMDBG_H("bridge %s already added, update data\n",
			data->bridge_name);
		bridge->associate_VID = data->vlan_id;
		bridge->bridge_ipv4_addr = data->bridge_ipv4;
		bridge->bridge_netmask = data->subnet_mask;
	}
	return;
fail:
	pthread_mutex_unlock(&vlan_l2tp_lock);
	return;
}

void IPACM_Config::del_bridge_vlan_mapping(ipa_ioc_bridge_vlan_mapping_info *data)
{
	list<bridge_vlan_mapping_info>::iterator it_mapping;
	ipacm_bridge *bridge = NULL;

	IPACMDBG_H("deleting bridge vlan mapping (%s)->(%d)\n",
		data->bridge_name,
		data->vlan_id);

	if(pthread_mutex_lock(&vlan_l2tp_lock) != 0)
	{
		IPACMERR("Unable to lock the mutex\n");
		return;
	}

	for(it_mapping = m_bridge_vlan_mapping.begin(); it_mapping != m_bridge_vlan_mapping.end(); it_mapping++)
	{
		if(strncmp(data->bridge_name, it_mapping->bridge_iface_name, sizeof(data->bridge_name)) == 0)
		{
			IPACMDBG_H("Found the bridge mapping (%s->%d)\n",
				data->bridge_name,
				it_mapping->bridge_associated_VID);
			m_bridge_vlan_mapping.erase(it_mapping);

			bridge = get_vlan_bridge(data->bridge_name);
			if(bridge)
			{
				IPACMDBG_H("bridge %s - remove vlan id\n",
					data->bridge_name);
				bridge->associate_VID = 0;
			}
			break;
		}
	}

	pthread_mutex_unlock(&vlan_l2tp_lock);
	return;
}

int IPACM_Config::get_bridge_vlan_mapping(ipa_ioc_bridge_vlan_mapping_info *data)
{
	list<bridge_vlan_mapping_info>::iterator it_mapping;
	int ret = IPACM_FAILURE;

	if(pthread_mutex_lock(&vlan_l2tp_lock) != 0)
	{
		IPACMERR("Unable to lock the mutex\n");
		return IPACM_FAILURE;
	}

	for(it_mapping = m_bridge_vlan_mapping.begin(); it_mapping != m_bridge_vlan_mapping.end(); it_mapping++)
	{
		if(strncmp(data->bridge_name, it_mapping->bridge_iface_name, sizeof(data->bridge_name)) == 0)
		{
			IPACMDBG_H("Found the bridge mapping (%s->%d)\n",
				data->bridge_name,
				it_mapping->bridge_associated_VID);

			data->vlan_id = it_mapping->bridge_associated_VID;
			data->bridge_ipv4 = it_mapping->bridge_ipv4;
			data->subnet_mask = it_mapping->subnet_mask;
			ret = IPACM_SUCCESS;
			break;
		}
	}

	pthread_mutex_unlock(&vlan_l2tp_lock);
	return ret;
}
#endif

#if defined(FEATURE_L2TP_E2E) || defined(FEATURE_L2TP) || defined(FEATURE_VLAN_MPDN)
void IPACM_Config::add_vlan_iface(ipa_ioc_vlan_iface_info *data)
{
	list<vlan_iface_info>::iterator it_vlan;
	vlan_iface_info new_vlan_info;

	if(pthread_mutex_lock(&vlan_l2tp_lock) != 0)
	{
		IPACMERR("Unable to lock the mutex\n");
		return;
	}

	IPACMDBG_H("Vlan iface: %s vlan id: %d\n", data->name, data->vlan_id);
	for(it_vlan = m_vlan_iface.begin(); it_vlan != m_vlan_iface.end(); it_vlan++)
	{
		if(strncmp(it_vlan->vlan_iface_name, data->name, sizeof(it_vlan->vlan_iface_name)) == 0)
		{
			IPACMERR("The vlan iface was added before with id %d\n", it_vlan->vlan_id);
			pthread_mutex_unlock(&vlan_l2tp_lock);
			return;
		}
	}
#ifndef FEATURE_VLAN_MPDN
	list<l2tp_vlan_mapping_info>::iterator it_mapping;
	for(it_mapping = m_l2tp_vlan_mapping.begin(); it_mapping != m_l2tp_vlan_mapping.end(); it_mapping++)
	{
		if(strncmp(data->name, it_mapping->vlan_iface_name, sizeof(data->name)) == 0)
		{
			IPACMDBG_H("Found a mapping: l2tp iface %s.\n", it_mapping->l2tp_iface_name);
			it_mapping->vlan_id = data->vlan_id;
		}
	}
#endif
#ifdef FEATURE_VLAN_MPDN
	AddNatIfaces(data->name);
	IPACMDBG_H("Add VLAN iface %s to nat ifaces.\n", data->name);
#endif
	memset(&new_vlan_info, 0 , sizeof(new_vlan_info));
	strlcpy(new_vlan_info.vlan_iface_name, data->name, sizeof(new_vlan_info.vlan_iface_name));
	new_vlan_info.vlan_id = data->vlan_id;
	m_vlan_iface.push_front(new_vlan_info);
	pthread_mutex_unlock(&vlan_l2tp_lock);

	return;
}

void IPACM_Config::restore_vlan_nat_ifaces(const char *phys_iface_name)
{
	list<vlan_iface_info>::iterator it_vlan;

	if(!phys_iface_name)
	{
		IPACMERR("got NULL iface_name\n");
		return;
	}

	if(pthread_mutex_lock(&vlan_l2tp_lock) != 0)
	{
		IPACMERR("Unable to lock the mutex\n");
		return;
	}

	IPACMDBG_H("searching iface %s vlan interfaces to add to NAT devices\n", phys_iface_name)

	for(it_vlan = m_vlan_iface.begin(); it_vlan != m_vlan_iface.end(); it_vlan++)
	{
		if(strstr(it_vlan->vlan_iface_name, phys_iface_name))
		{
			AddNatIfaces(it_vlan->vlan_iface_name);
			IPACMDBG_H("restored VLAN iface %s to nat ifaces.\n", it_vlan->vlan_iface_name);
		}
	}

	pthread_mutex_unlock(&vlan_l2tp_lock);
	return;
}

void IPACM_Config::del_vlan_iface(ipa_ioc_vlan_iface_info *data)
{
	list<vlan_iface_info>::iterator it_vlan;

	if(pthread_mutex_lock(&vlan_l2tp_lock) != 0)
	{
		IPACMERR("Unable to lock the mutex\n");
		return;
	}

	IPACMDBG_H("Vlan iface: %s vlan id: %d\n", data->name, data->vlan_id);
	for(it_vlan = m_vlan_iface.begin(); it_vlan != m_vlan_iface.end(); it_vlan++)
	{
		if(strncmp(it_vlan->vlan_iface_name, data->name, sizeof(it_vlan->vlan_iface_name)) == 0)
		{
			IPACMDBG_H("Found the vlan interface\n");
			m_vlan_iface.erase(it_vlan);
			break;
		}
	}
#ifdef FEATURE_VLAN_MPDN
	DelNatIfaces(data->name);
	IPACMDBG_H("Del VLAN iface %s to nat ifaces.\n", data->name);
#endif

#ifndef FEATURE_VLAN_MPDN
	list<l2tp_vlan_mapping_info>::iterator it_mapping;
	it_mapping = m_l2tp_vlan_mapping.begin();
	while(it_mapping != m_l2tp_vlan_mapping.end())
	{
		if(strncmp(data->name, it_mapping->vlan_iface_name, sizeof(data->name)) == 0)
		{
			IPACMDBG_H("Delete mapping with l2tp iface %s\n", it_mapping->l2tp_iface_name);
			it_mapping = m_l2tp_vlan_mapping.erase(it_mapping);
		}
		else
		{
			it_mapping++;
		}
	}
#endif
	pthread_mutex_unlock(&vlan_l2tp_lock);

	return;
}

void IPACM_Config::handle_vlan_iface_info(ipacm_event_data_addr *data)
{
	list<vlan_iface_info>::iterator it_vlan;

	if(pthread_mutex_lock(&vlan_l2tp_lock) != 0)
	{
		IPACMERR("Unable to lock the mutex\n");
		return;
	}

	IPACMDBG_H("Incoming vlan iface: %s IPv6 address: 0x%08x%08x%08x%08x\n", data->iface_name,
		data->ipv6_addr[0], data->ipv6_addr[1], data->ipv6_addr[2], data->ipv6_addr[3]);

	for(it_vlan = m_vlan_iface.begin(); it_vlan != m_vlan_iface.end(); it_vlan++)
	{
		if(strncmp(it_vlan->vlan_iface_name, data->iface_name,
			sizeof(it_vlan->vlan_iface_name)) == 0)
		{
			IPACMDBG_H("Found vlan iface: %s\n", it_vlan->vlan_iface_name);
			memcpy(it_vlan->vlan_iface_ipv6_addr, data->ipv6_addr,
				sizeof(it_vlan->vlan_iface_ipv6_addr));

#ifndef FEATURE_VLAN_MPDN
			list<l2tp_vlan_mapping_info>::iterator it_mapping;

			for(it_mapping = m_l2tp_vlan_mapping.begin(); it_mapping != m_l2tp_vlan_mapping.end(); it_mapping++)
			{
				if(strncmp(it_mapping->vlan_iface_name, it_vlan->vlan_iface_name,
					sizeof(it_mapping->vlan_iface_name)) == 0)
				{
					IPACMDBG_H("Found the l2tp-vlan mapping: l2tp %s\n", it_mapping->l2tp_iface_name);
					memcpy(it_mapping->vlan_iface_ipv6_addr, data->ipv6_addr,
						sizeof(it_mapping->vlan_iface_ipv6_addr));
				}
			}
			break;
#endif
		}
	}

	if(it_vlan == m_vlan_iface.end())
	{
		IPACMDBG_H("Failed to find the vlan iface: %s\n", data->iface_name);
	}
	pthread_mutex_unlock(&vlan_l2tp_lock);

	return;
}

void IPACM_Config::handle_vlan_client_info(ipacm_event_data_all *data)
{
	list<l2tp_vlan_mapping_info>::iterator it_mapping;
	list<vlan_iface_info>::iterator it_vlan;

	if(pthread_mutex_lock(&vlan_l2tp_lock) != 0)
	{
		IPACMERR("Unable to lock the mutex\n");
		return;
	}

	IPACMDBG_H("Incoming vlan client iface: %s IPv6 address: 0x%08x%08x%08x%08x\n", data->iface_name,
		data->ipv6_addr[0], data->ipv6_addr[1], data->ipv6_addr[2], data->ipv6_addr[3]);
	IPACMDBG_H("MAC address: 0x%02x%02x%02x%02x%02x%02x\n", data->mac_addr[0], data->mac_addr[1],
		data->mac_addr[2], data->mac_addr[3], data->mac_addr[4], data->mac_addr[5]);

	for(it_vlan = m_vlan_iface.begin(); it_vlan != m_vlan_iface.end(); it_vlan++)
	{
		if(strncmp(it_vlan->vlan_iface_name, data->iface_name, sizeof(it_vlan->vlan_iface_name)) == 0)
		{
			IPACMDBG_H("Found vlan iface in vlan list: %s\n", it_vlan->vlan_iface_name);
			if(it_vlan->vlan_client_ipv6_addr[0] > 0 || it_vlan->vlan_client_ipv6_addr[1] > 0 ||
				it_vlan->vlan_client_ipv6_addr[2] > 0 || it_vlan->vlan_client_ipv6_addr[3] > 0)
			{
				IPACMDBG_H("Vlan client info has been populated before, return.\n");
				pthread_mutex_unlock(&vlan_l2tp_lock);
				return;
			}
			memcpy(it_vlan->vlan_client_mac, data->mac_addr, sizeof(it_vlan->vlan_client_mac));
			memcpy(it_vlan->vlan_client_ipv6_addr, data->ipv6_addr, sizeof(it_vlan->vlan_client_ipv6_addr));
			break;
		}
	}
#ifndef FEATURE_VLAN_MPDN
	for(it_mapping = m_l2tp_vlan_mapping.begin(); it_mapping != m_l2tp_vlan_mapping.end(); it_mapping++)
	{
		if(strncmp(it_mapping->vlan_iface_name, data->iface_name, sizeof(it_mapping->vlan_iface_name)) == 0)
		{
			IPACMDBG_H("Found vlan iface in l2tp mapping list: %s, l2tp iface: %s\n", it_mapping->vlan_iface_name,
				it_mapping->l2tp_iface_name);
			memcpy(it_mapping->vlan_client_mac, data->mac_addr, sizeof(it_mapping->vlan_client_mac));
			memcpy(it_mapping->vlan_client_ipv6_addr, data->ipv6_addr, sizeof(it_mapping->vlan_client_ipv6_addr));
		}
	}
#endif
	pthread_mutex_unlock(&vlan_l2tp_lock);

	return;
}
#endif

#ifdef FEATURE_VLAN_MPDN

void IPACM_Config::get_vlan_mode_ifaces()
{
	struct ipa_ioc_get_vlan_mode vlan_mode;
	int retval;

	for(int i = 0; i < IPA_VLAN_IF_MAX; i++)
	{
		vlan_mode.iface = static_cast<ipa_vlan_ifaces>(i);
		retval = ioctl(m_fd, IPA_IOC_GET_VLAN_MODE, &vlan_mode);
		if(retval)
		{
			IPACMERR("failed reading vlan mode for %d, error %d\n", i ,retval);
			vlan_devices[i] = 0;
		}
		vlan_devices[i] = vlan_mode.is_vlan_mode;
	}

	IPACMDBG("modes are EMAC %d, RNDIS %d, ECM %d\n",
		vlan_devices[IPA_VLAN_IF_EMAC],
		vlan_devices[IPA_VLAN_IF_RNDIS],
		vlan_devices[IPA_VLAN_IF_ECM]);
}

void IPACM_Config::add_vlan_bridge(ipacm_event_data_all *data_all)
{
	uint8_t testmac[IPA_MAC_ADDR_SIZE];
	ipa_ioc_bridge_vlan_mapping_info mapping_info;

	memset(testmac, 0, IPA_MAC_ADDR_SIZE * sizeof(uint8_t));
	memset(&mapping_info, 0, sizeof(mapping_info));

	strlcpy(mapping_info.bridge_name, data_all->iface_name, IF_NAME_LEN);

	for(int i = 0; i < IPA_MAX_NUM_BRIDGES; i++)
	{
		if(strcmp(data_all->iface_name, IPACM_Iface::ipacmcfg->vlan_bridges[i].bridge_name) == 0)
		{
			IPACMDBG_H("bridge %s already exist with MAC %02x:%02x:%02x:%02x:%02x:%02x\n ignoring\n",
				data_all->iface_name, IPACM_Iface::ipacmcfg->vlan_bridges[i].bridge_mac[0],
				IPACM_Iface::ipacmcfg->vlan_bridges[i].bridge_mac[1],
				IPACM_Iface::ipacmcfg->vlan_bridges[i].bridge_mac[2],
				IPACM_Iface::ipacmcfg->vlan_bridges[i].bridge_mac[3],
				IPACM_Iface::ipacmcfg->vlan_bridges[i].bridge_mac[4],
				IPACM_Iface::ipacmcfg->vlan_bridges[i].bridge_mac[5]);
			return;
		}
		/* no MAC was assigned before i.e. this is the first unused entry*/
		else if(!memcmp(IPACM_Iface::ipacmcfg->vlan_bridges[i].bridge_mac, testmac, sizeof(uint8_t) * IPA_MAC_ADDR_SIZE))
		{
			bool default_bridge = false;

			if(strcmp(ipa_virtual_iface_name, data_all->iface_name) == 0)
			{
				default_bridge = true;
			}

			if(get_bridge_vlan_mapping(&mapping_info))
			{
				if(default_bridge)
				{
					IPACMDBG_H("default bridge doesn't have vlan mapping\n");
				}
				else
				{
					/* mapping may arrive later and information will be updated then */
					IPACMERR("no bridge vlan mapping found for bridge %s, not adding\n", data_all->iface_name);
					return;
				}
			}

			vlan_bridges[i].bridge_netmask = mapping_info.subnet_mask;
			vlan_bridges[i].bridge_ipv4_addr = mapping_info.bridge_ipv4;
			strlcpy(vlan_bridges[i].bridge_name, data_all->iface_name, IF_NAME_LEN);
			vlan_bridges[i].associate_VID = mapping_info.vlan_id;
			IPACMDBG("bridge (%s) mask 0x%X, address 0x%X, VID %d\n", data_all->iface_name,
				mapping_info.subnet_mask,
				mapping_info.bridge_ipv4,
				mapping_info.vlan_id);

			struct ifreq ifr;
			int fd;

			fd = socket(AF_INET, SOCK_DGRAM, 0);
			memset(&ifr, 0, sizeof(struct ifreq));
			ifr.ifr_addr.sa_family = AF_INET;
			strlcpy(ifr.ifr_name, data_all->iface_name, sizeof(ifr.ifr_name));
			if(ioctl(fd, SIOCGIFHWADDR, &ifr) < 0)
			{
				IPACMERR("unable to retrieve (%s) bridge MAC\n", ifr.ifr_name);
				vlan_bridges[i].bridge_netmask = 0;
				vlan_bridges[i].bridge_ipv4_addr = 0;
				vlan_bridges[i].associate_VID = 0;
				close(fd);
				return;
			}
			memcpy(vlan_bridges[i].bridge_mac,
				ifr.ifr_hwaddr.sa_data,
				sizeof(vlan_bridges[i].bridge_mac));
			IPACMDBG("got bridge MAC using IOCTL\n");
			if(default_bridge)
			{
				memcpy(IPACM_Iface::ipacmcfg->bridge_mac,
					ifr.ifr_hwaddr.sa_data,
					sizeof(IPACM_Iface::ipacmcfg->bridge_mac));

				IPACM_Iface::ipacmcfg->ipa_bridge_enable = true;

				IPACMDBG("set default bridge flag dev %s\n",
					data_all->iface_name);
			}
			close(fd);
			IPACMDBG_H("added bridge named %s, MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
				IPACM_Iface::ipacmcfg->vlan_bridges[i].bridge_name,
				IPACM_Iface::ipacmcfg->vlan_bridges[i].bridge_mac[0],
				IPACM_Iface::ipacmcfg->vlan_bridges[i].bridge_mac[1],
				IPACM_Iface::ipacmcfg->vlan_bridges[i].bridge_mac[2],
				IPACM_Iface::ipacmcfg->vlan_bridges[i].bridge_mac[3],
				IPACM_Iface::ipacmcfg->vlan_bridges[i].bridge_mac[4],
				IPACM_Iface::ipacmcfg->vlan_bridges[i].bridge_mac[5]);
			return;
		}
	}
	IPACMERR("couldn't find an empty cell for new bridge\n");
}

ipacm_bridge *IPACM_Config::get_vlan_bridge(char *name)
{
	for(int i = 0; i < IPA_MAX_NUM_BRIDGES; i++)
	{
		if(strcmp(name, IPACM_Iface::ipacmcfg->vlan_bridges[i].bridge_name) == 0)
		{
			IPACMDBG_H("found bridge %s with MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
				IPACM_Iface::ipacmcfg->vlan_bridges[i].bridge_name,
				IPACM_Iface::ipacmcfg->vlan_bridges[i].bridge_mac[0],
				IPACM_Iface::ipacmcfg->vlan_bridges[i].bridge_mac[1],
				IPACM_Iface::ipacmcfg->vlan_bridges[i].bridge_mac[2],
				IPACM_Iface::ipacmcfg->vlan_bridges[i].bridge_mac[3],
				IPACM_Iface::ipacmcfg->vlan_bridges[i].bridge_mac[4],
				IPACM_Iface::ipacmcfg->vlan_bridges[i].bridge_mac[5]);

			return &IPACM_Iface::ipacmcfg->vlan_bridges[i];
		}
	}

	IPACMDBG_H("no bridge %s exists\n", name);
	return NULL;
}

bool IPACM_Config::is_added_vlan_iface(char *iface_name)
{
	list<vlan_iface_info>::iterator it_vlan;
	bool ret = false;

	if(pthread_mutex_lock(&vlan_l2tp_lock) != 0)
	{
		IPACMERR("Unable to lock the mutex\n");
		return false;
	}

	for(it_vlan = m_vlan_iface.begin(); it_vlan != m_vlan_iface.end(); it_vlan++)
	{
		if(strncmp(it_vlan->vlan_iface_name, iface_name, sizeof(it_vlan->vlan_iface_name)) == 0)
		{
			IPACMDBG_H("Found vlan iface in vlan list: %s\n", it_vlan->vlan_iface_name);
			ret = true;
			break;
		}
	}

	pthread_mutex_unlock(&vlan_l2tp_lock);

	return ret;
}

bool IPACM_Config::iface_in_vlan_mode(const char *phys_iface_name)
{
	if(strstr(phys_iface_name, "eth"))
	{
		IPACMDBG("eth vlan mode %d\n", vlan_devices[IPA_VLAN_IF_EMAC]);
		return vlan_devices[IPA_VLAN_IF_EMAC];
	}

	if(strstr(phys_iface_name, "rndis"))
	{
		IPACMDBG("rndis vlan mode %d\n", vlan_devices[IPA_VLAN_IF_RNDIS]);
		return vlan_devices[IPA_VLAN_IF_RNDIS];
	}

	if(strstr(phys_iface_name, "ecm"))
	{
		IPACMDBG("ecm vlan mode %d\n", vlan_devices[IPA_VLAN_IF_RNDIS]);
		return vlan_devices[IPA_VLAN_IF_ECM];
	}

	IPACMDBG("iface %s did not match any known ifaces\n", phys_iface_name);
	return false;
}

int IPACM_Config::get_iface_vlan_ids(char *phys_iface_name, uint8_t *Ids)
{
	list<vlan_iface_info>::iterator it_vlan;
	int cnt = 0;

	if(!Ids)
	{
		IPACMERR("got NULL Ids array\n");
		return IPACM_FAILURE;
	}

	if(pthread_mutex_lock(&vlan_l2tp_lock) != 0)
	{
		IPACMERR("Unable to lock the mutex\n");
		return false;
	}

	for(it_vlan = m_vlan_iface.begin(); it_vlan != m_vlan_iface.end() && cnt < IPA_MAX_NUM_HW_PDNS; it_vlan++)
	{
		if(strstr(it_vlan->vlan_iface_name, phys_iface_name))
		{
			IPACMDBG_H("Found vlan iface in vlan list: %s\n", it_vlan->vlan_iface_name);
			Ids[cnt] = it_vlan->vlan_id;
			cnt++;
		}
	}

	pthread_mutex_unlock(&vlan_l2tp_lock);

	IPACMDBG_H("found %d vlan interfaces for dev %s\n", cnt, phys_iface_name);

	while(cnt < IPA_MAX_NUM_HW_PDNS)
	{
		Ids[cnt] = 0;
		cnt++;
	}

	return IPACM_SUCCESS;
}

int IPACM_Config::get_vlan_id(char *iface_name, uint8_t *vlan_id)
{
	list<vlan_iface_info>::iterator it_vlan;
	int ret = IPACM_FAILURE;

	if(pthread_mutex_lock(&vlan_l2tp_lock) != 0)
	{
		IPACMERR("Unable to lock the mutex\n");
		return IPACM_FAILURE;
	}

	for(it_vlan = m_vlan_iface.begin(); it_vlan != m_vlan_iface.end(); it_vlan++)
	{
		if(strncmp(it_vlan->vlan_iface_name, iface_name, sizeof(it_vlan->vlan_iface_name)) == 0)
		{
			IPACMDBG_H("Found vlan iface in vlan list: %s\n", it_vlan->vlan_iface_name);
			*vlan_id = it_vlan->vlan_id;
			ret = IPACM_SUCCESS;
			break;
		}
	}

	pthread_mutex_unlock(&vlan_l2tp_lock);

	return ret;
}
#endif

#if defined(FEATURE_L2TP_E2E) || defined(FEATURE_L2TP)
void IPACM_Config::add_l2tp_vlan_mapping(ipa_ioc_l2tp_vlan_mapping_info *data)
{
	list<l2tp_vlan_mapping_info>::iterator it_mapping;
	list<vlan_iface_info>::iterator it_vlan;
	l2tp_vlan_mapping_info new_mapping;

	if(pthread_mutex_lock(&vlan_l2tp_lock) != 0)
	{
		IPACMERR("Unable to lock the mutex\n");
		return;
	}

	IPACMDBG_H("L2tp iface: %s session id: %d vlan iface: %s \n",
		data->l2tp_iface_name, data->l2tp_session_id, data->vlan_iface_name);
	for(it_mapping = m_l2tp_vlan_mapping.begin(); it_mapping != m_l2tp_vlan_mapping.end(); it_mapping++)
	{
		if(strncmp(data->l2tp_iface_name, it_mapping->l2tp_iface_name,
			sizeof(data->l2tp_iface_name)) == 0)
		{
			IPACMERR("L2tp mapping was added before mapped to vlan %s.\n", it_mapping->vlan_iface_name);
			pthread_mutex_unlock(&vlan_l2tp_lock);
			return;
		}
	}

	AddNatIfaces(data->l2tp_iface_name);
	IPACMDBG_H("Add l2tp iface %s to nat ifaces.\n", data->l2tp_iface_name);

	memset(&new_mapping, 0, sizeof(new_mapping));
	strlcpy(new_mapping.l2tp_iface_name, data->l2tp_iface_name,
		sizeof(new_mapping.l2tp_iface_name));
	strlcpy(new_mapping.vlan_iface_name, data->vlan_iface_name,
		sizeof(new_mapping.vlan_iface_name));
	new_mapping.l2tp_session_id = data->l2tp_session_id;

	for(it_vlan = m_vlan_iface.begin(); it_vlan != m_vlan_iface.end(); it_vlan++)
	{
		if(strncmp(it_vlan->vlan_iface_name, data->vlan_iface_name, sizeof(it_vlan->vlan_iface_name)) == 0)
		{
			IPACMDBG_H("Found vlan iface with id %d\n", it_vlan->vlan_id);
			new_mapping.vlan_id = it_vlan->vlan_id;
			memcpy(new_mapping.vlan_iface_ipv6_addr, it_vlan->vlan_iface_ipv6_addr,
				sizeof(new_mapping.vlan_iface_ipv6_addr));
			memcpy(new_mapping.vlan_client_mac, it_vlan->vlan_client_mac,
				sizeof(new_mapping.vlan_client_mac));
			memcpy(new_mapping.vlan_client_ipv6_addr, it_vlan->vlan_client_ipv6_addr,
				sizeof(new_mapping.vlan_client_ipv6_addr));
			break;
		}
	}
	m_l2tp_vlan_mapping.push_front(new_mapping);
	pthread_mutex_unlock(&vlan_l2tp_lock);

	return;
}

void IPACM_Config::del_l2tp_vlan_mapping(ipa_ioc_l2tp_vlan_mapping_info *data)
{
	list<l2tp_vlan_mapping_info>::iterator it;

	if(pthread_mutex_lock(&vlan_l2tp_lock) != 0)
	{
		IPACMERR("Unable to lock the mutex\n");
		return;
	}

	IPACMDBG_H("L2tp iface: %s session id: %d vlan iface: %s \n",
		data->l2tp_iface_name, data->l2tp_session_id, data->vlan_iface_name);
	for(it = m_l2tp_vlan_mapping.begin(); it != m_l2tp_vlan_mapping.end(); it++)
	{
		if(strncmp(data->l2tp_iface_name, it->l2tp_iface_name,
			sizeof(data->l2tp_iface_name)) == 0)
		{
			IPACMDBG_H("Found l2tp iface mapped to vlan %s.\n", it->vlan_iface_name);
			if(strncmp(data->vlan_iface_name, it->vlan_iface_name,
				sizeof(data->vlan_iface_name)) == 0)
			{
				m_l2tp_vlan_mapping.erase(it);
				DelNatIfaces(data->l2tp_iface_name);
				IPACMDBG_H("Del l2tp iface %s to nat ifaces.\n", data->l2tp_iface_name);
			}
			else
			{
				IPACMERR("Incoming mapping is incorrect.\n");
			}
			break;
		}
	}
	pthread_mutex_unlock(&vlan_l2tp_lock);

	return;
}

int IPACM_Config::get_vlan_l2tp_mapping(char *client_iface, l2tp_vlan_mapping_info& info)
{
	list<l2tp_vlan_mapping_info>::iterator it_mapping;

	if(pthread_mutex_lock(&vlan_l2tp_lock) != 0)
	{
		IPACMERR("Unable to lock the mutex\n");
		return IPACM_FAILURE;
	}

	IPACMDBG_H("Incoming client iface name: %s\n", client_iface);

	for(it_mapping = m_l2tp_vlan_mapping.begin(); it_mapping != m_l2tp_vlan_mapping.end(); it_mapping++)
	{
		if(strncmp(client_iface, it_mapping->l2tp_iface_name,
			strlen(client_iface)) == 0)
		{
			IPACMDBG_H("Found vlan-l2tp mapping.\n");
			info = *it_mapping;
			pthread_mutex_unlock(&vlan_l2tp_lock);
			return IPACM_SUCCESS;
		}
	}

	pthread_mutex_unlock(&vlan_l2tp_lock);
	return IPACM_FAILURE;
}
#endif
