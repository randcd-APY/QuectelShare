/*
Copyright (c) 2013, 2018, The Linux Foundation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*!
	@file
	IPACM_Wan.cpp

	@brief
	This file implements the WAN iface functionality.

	@Author
	Skylar Chang

*/
#ifndef IPACM_WAN_H
#define IPACM_WAN_H

#include <stdio.h>
#include <IPACM_CmdQueue.h>
#include <linux/msm_ipa.h>
#include "IPACM_Routing.h"
#include "IPACM_Filtering.h"
#include <IPACM_Iface.h>
#include <IPACM_Defs.h>
#include <IPACM_Xml.h>

#define IPA_NUM_DEFAULT_WAN_FILTER_RULES 3 /*1 for v4, 2 for v6*/
#define IPA_V2_NUM_DEFAULT_WAN_FILTER_RULE_IPV4 2

#define NETWORK_STATS "%s %lu %lu %lu %lu"
#ifdef FEATURE_IPA_ANDROID
#define IPA_NETWORK_STATS_FILE_NAME "/data/misc/ipa/network_stats"
#else
#define IPA_NETWORK_STATS_FILE_NAME "/tmp/network_stats"
#endif

typedef struct _wan_client_rt_hdl
{
	uint32_t wan_rt_rule_hdl_v4;
	uint32_t wan_rt_rule_hdl_v6[IPV6_NUM_ADDR];
	uint32_t wan_rt_rule_hdl_v6_wan[IPV6_NUM_ADDR];
}wan_client_rt_hdl;

typedef struct _ipa_wan_client
{
	ipacm_event_data_wlan_ex* p_hdr_info;
	uint8_t mac[IPA_MAC_ADDR_SIZE];
	uint32_t v4_addr;
	uint32_t v6_addr[IPV6_NUM_ADDR][4];
	uint32_t hdr_hdl_v4;
	uint32_t hdr_hdl_v6;
	bool route_rule_set_v4;
	int route_rule_set_v6;
	bool ipv4_set;
	int ipv6_set;
	bool ipv4_header_set;
	bool ipv6_header_set;
	bool power_save_set;
	wan_client_rt_hdl wan_rt_hdl[0]; /* depends on number of tx properties */
}ipa_wan_client;

class IPACM_Wan;

typedef struct
{
	uint32_t ipv4_addr;
	bool wan_up_vlan;
	IPACM_Wan *pIface;
}ipacm_ipv4_wan_iface;

typedef struct
{
	uint32_t ipv6_prefix[2];
	bool wan_up_vlan_v6;
	IPACM_Wan *pIface;
}ipacm_ipv6_wan_iface;

struct ipacm_pdn_flt_rule
{
	struct ipa_flt_rule_add flt_rule;
	uint8_t mux_id;
};

/* wan iface */
class IPACM_Wan : public IPACM_Iface
{

public:
	/* IPACM pm_depency q6 check*/
	static int ipa_pm_q6_check;
	static bool wan_up;
	static bool wan_up_v6;
	static uint8_t xlat_mux_id;
#ifdef FEATURE_VLAN_MPDN
#ifdef FEATURE_IPACM_UL_FIREWALL
	int num_firewall_v6_ul_pdn;
#endif
	uint8_t associated_VID;
#endif
	/* IPACM interface name */
	static char wan_up_dev_name[IF_NAME_LEN];
	static uint32_t curr_wan_ip;
	IPACM_Wan(int, ipacm_wan_iface_type, uint8_t *);
	virtual ~IPACM_Wan();
#ifdef FEATURE_IPACM_UL_FIREWALL
	/* IPACM firewall Configuration file*/
	static IPACM_firewall_conf_t firewall_config_ul;
#ifdef FEATURE_VLAN_MPDN
	static IPACM_firewall_t firewall_mpdn_config_ul;
#endif //FEATURE_VLAN_MPDN
	static int read_firewall_filter_rules_ul(void);

	static bool check_dft_firewall_rules_attr_mask_ul(IPACM_firewall_conf_t *firewall_config);

#ifdef FEATURE_VLAN_MPDN
	static int get_v6_pdn_firewall_configs(
		std::pair<IPACM_firewall_conf_t*, ipacm_ipv6_wan_iface*> wan_firewall_pair[],
		IPACM_firewall_t &firewall_configs);

	static IPACM_firewall_conf_t* get_firewall_conf_by_vid_ul(int vid);
#endif //FEATURE_VLAN_MPDN
	static IPACM_firewall_conf_t* get_default_profile_firewall_conf_ul(int *default_vid);

	static int set_pdn_num_fw_rules_by_vid(int vid, int num_fw_rules);

	static int get_pdn_num_fw_rules_by_vid(int vid, int *num_fw_rules);
#endif //FEATURE_IPACM_UL_FIREWALL
#ifdef FEATURE_VLAN_MPDN
	static int GetV6PrefixByVid(int vid, uint32_t *v6_prefix);
	static IPACM_firewall_conf_t* get_curr_pdn_firewall_config(IPACM_firewall_t &firewall_configs, const char* dev_name);
#endif
	static bool isWanUP(int ipa_if_num_tether)
	{
#ifdef FEATURE_IPA_ANDROID
		int i;
		for (i=0; i < ipa_if_num_tether_v4_total;i++)
		{
			if (ipa_if_num_tether_v4[i] == ipa_if_num_tether)
			{
				IPACMDBG_H("support ipv4 tether_iface(%s)\n",
					IPACM_Iface::ipacmcfg->iface_table[ipa_if_num_tether].iface_name);
				return wan_up;
				break;
			}
		}
		return false;
#else
		return wan_up;
#endif
	}

#ifdef FEATURE_VLAN_MPDN
	static bool isVlanWanUP()
	{
		for(int i = 0; i < IPA_MAX_NUM_SW_PDNS; i++)
		{
			if(ipv4_to_iface[i].ipv4_addr && ipv4_to_iface[i].wan_up_vlan)
			{
				IPACMDBG_H("iface %s is vlan up\n", ipv4_to_iface[i].pIface->dev_name);
				return true;
			}
		}
		return false;
	}

	static bool isVlanWanUP_V6()
	{
		for(int i = 0; i < IPA_MAX_NUM_SW_PDNS; i++)
		{
			if(ipv6_to_iface[i].wan_up_vlan_v6)
			{
				IPACMDBG_H("iface %s is vlan up v6\n", ipv6_to_iface[i].pIface->dev_name);
				return true;
			}
		}
		return false;
	}

	static int getFreePDNIndex_V4()
	{
		for(int i = 0; i < IPA_MAX_NUM_SW_PDNS; i++)
		{
			if(!ipv4_to_iface[i].pIface)
			{
				IPACMDBG_H("iface index %d is free\n", i);
				return i;
			}
		}
		return -1;
	}

	static int getFreePDNIndex_V6()
	{
		for(int i = 0; i < IPA_MAX_NUM_SW_PDNS; i++)
		{
			if(!ipv6_to_iface[i].pIface)
			{
				IPACMDBG_H("iface index %d is free\n", i);
				return i;
			}
		}
		return -1;
	}

	static bool isDefaultGatewayIfaceUp(IPACM_Wan *iface)
	{
		if(wan_up && iface->is_default_gateway)
		{
			IPACMDBG("iface %s, wan_up %d, is_default_gateway %d\n",
				iface->dev_name, wan_up, iface->is_default_gateway);
			return true;
		}
		return false;
	}

	static bool isDefaultGatewayIfaceUp_v6(IPACM_Wan *iface)
	{
		if(wan_up_v6 && iface->is_default_gateway)
		{
			IPACMDBG("iface %s, wan_up_v6 %d, is_default_gateway %d\n",
				iface->dev_name, wan_up_v6, iface->is_default_gateway);
			return true;
		}
		return false;
	}
#endif

	static bool isWanUP_V6(int ipa_if_num_tether)
	{
#ifdef FEATURE_IPA_ANDROID
		int i;
		for (i=0; i < ipa_if_num_tether_v6_total;i++)
		{
			if (ipa_if_num_tether_v6[i] == ipa_if_num_tether)
			{
				IPACMDBG_H("support ipv6 tether_iface(%s)\n",
					IPACM_Iface::ipacmcfg->iface_table[ipa_if_num_tether].iface_name);
				return wan_up_v6;
				break;
			}
		}
		return false;
#else
		return wan_up_v6;
#endif
	}

	static uint32_t getWANIP()
	{
		return curr_wan_ip;
	}

	static bool getXlat_Mux_Id()
	{
		return xlat_mux_id;
	}

	void event_callback(ipa_cm_event_id event, void *data);

#ifdef FEATURE_VLAN_MPDN
	static struct ipacm_pdn_flt_rule pdn_flt_rule_v4[IPA_MAX_FLT_RULE];
	static struct ipacm_pdn_flt_rule pdn_flt_rule_v6[IPA_MAX_FLT_RULE];
#else
	static struct ipa_flt_rule_add flt_rule_v4[IPA_MAX_FLT_RULE];
	static struct ipa_flt_rule_add flt_rule_v6[IPA_MAX_FLT_RULE];
#endif
#ifdef FEATURE_IPACM_UL_FIREWALL
	static struct ipa_flt_rule_add firewall_flt_rule_v6_ul[IPACM_MAX_FIREWALL_ENTRIES+1];
#endif

	static int num_v4_flt_rule;
	static int num_v6_flt_rule;
#ifdef FEATURE_VLAN_MPDN
	static int ipv6_mpdn_default_filterting_rules_count;
#endif
#ifdef FEATURE_IPACM_UL_FIREWALL
	static int num_firewall_v6_ul;
#endif

	ipacm_wan_iface_type m_is_sta_mode;
	static bool backhaul_is_sta_mode;
	static bool is_ext_prop_set;
	static uint32_t backhaul_ipv6_prefix[2];
#ifdef FEATURE_IPACM_UL_FIREWALL
	static int m_fd_ipa_ul;
#endif

	static bool embms_is_on;
	static bool backhaul_is_wan_bridge;

	static bool isWan_Bridge_Mode()
	{
		return backhaul_is_wan_bridge;
	}
#ifdef FEATURE_IPA_ANDROID
	/* IPACM interface id */
	static int ipa_if_num_tether_v4_total;
	static int ipa_if_num_tether_v4[IPA_MAX_IFACE_ENTRIES];
	static int ipa_if_num_tether_v6_total;
	static int ipa_if_num_tether_v6[IPA_MAX_IFACE_ENTRIES];
#endif

	static bool is_global_ipv6_addr(uint32_t* ipv6_addr);
#ifdef FEATURE_VLAN_MPDN
	static ipacm_ipv4_wan_iface ipv4_to_iface[IPA_MAX_NUM_SW_PDNS];
	static ipacm_ipv6_wan_iface ipv6_to_iface[IPA_MAX_NUM_SW_PDNS];
	static int GetMuxByVid(uint8_t vlan_id, uint8_t *mux_id, ipa_ip_type iptype);
#endif
private:

	bool is_ipv6_frag_firewall_flt_rule_installed;
	uint32_t ipv6_frag_firewall_flt_rule_hdl;
	uint32_t *wan_route_rule_v4_hdl;
	uint32_t *wan_route_rule_v6_hdl;
	uint32_t hdr_hdl_sta_v4;
	uint32_t hdr_hdl_sta_v6;
	uint32_t firewall_hdl_v4[IPACM_MAX_FIREWALL_ENTRIES];
	uint32_t firewall_hdl_v6[IPACM_MAX_FIREWALL_ENTRIES];
	uint32_t dft_wan_fl_hdl[IPA_NUM_DEFAULT_WAN_FILTER_RULES];
	uint32_t ipv6_dest_flt_rule_hdl[MAX_DEFAULT_v6_ROUTE_RULES];
	int num_ipv6_dest_flt_rule;
	uint32_t ODU_fl_hdl[IPA_NUM_DEFAULT_WAN_FILTER_RULES];
	int num_firewall_v4,num_firewall_v6;
	uint32_t wan_v4_addr;
	uint32_t public_wan_v4_addr;
	uint32_t wan_v4_addr_gw;
	uint32_t wan_v6_addr_gw[4];
	bool wan_v4_addr_set;
	bool public_wan_v4_addr_set;
	bool wan_v4_addr_gw_set;
	bool wan_v6_addr_gw_set;
	bool active_v4;
	bool active_v6;
	bool header_set_v4;
	bool header_set_v6;
	bool header_partial_default_wan_v4;
	bool header_partial_default_wan_v6;
	uint8_t ext_router_mac_addr[IPA_MAC_ADDR_SIZE];
	uint8_t netdev_mac[IPA_MAC_ADDR_SIZE];

	static uint32_t wan_route_rule_v6_hdl_a5;

	static int num_ipv4_modem_pdn;

	static int num_ipv6_modem_pdn;

	int modem_ipv4_pdn_index;

	int modem_ipv6_pdn_index;

	bool is_default_gateway;

	uint32_t ipv6_prefix[2];
	uint32_t m_ipv6_addr[IPA_IPV6_ADDR_SIZE_IN_WORDS];

	/* STA mode wan-client*/
	int wan_client_len;
	ipa_wan_client *wan_client;
	int header_name_count;
	int num_wan_client;
	uint8_t invalid_mac[IPA_MAC_ADDR_SIZE];
	bool is_xlat;

	/* update network stats for CNE */
	uint32_t hdr_hdl_dummy_v6;
	uint32_t hdr_proc_hdl_dummy_v6;

	inline ipa_wan_client* get_client_memptr(ipa_wan_client *param, int cnt)
	{
	    char *ret = ((char *)param) + (wan_client_len * cnt);
		return (ipa_wan_client *)ret;
	}

	inline int get_wan_client_index(uint8_t *mac_addr)
	{
		int cnt;
		int num_wan_client_tmp = num_wan_client;

		IPACMDBG_H("Passed MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
						 mac_addr[0], mac_addr[1], mac_addr[2],
						 mac_addr[3], mac_addr[4], mac_addr[5]);

		for(cnt = 0; cnt < num_wan_client_tmp; cnt++)
		{
			IPACMDBG_H("stored MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
							 get_client_memptr(wan_client, cnt)->mac[0],
							 get_client_memptr(wan_client, cnt)->mac[1],
							 get_client_memptr(wan_client, cnt)->mac[2],
							 get_client_memptr(wan_client, cnt)->mac[3],
							 get_client_memptr(wan_client, cnt)->mac[4],
							 get_client_memptr(wan_client, cnt)->mac[5]);

			if(memcmp(get_client_memptr(wan_client, cnt)->mac,
								mac_addr,
								sizeof(get_client_memptr(wan_client, cnt)->mac)) == 0)
			{
				IPACMDBG_H("Matched client index: %d\n", cnt);
				return cnt;
			}
		}

		return IPACM_INVALID_INDEX;
	}

	inline int get_wan_client_index_ipv4(uint32_t ipv4_addr)
	{
		int cnt;
		int num_wan_client_tmp = num_wan_client;

		IPACMDBG_H("Passed IPv4 %x\n", ipv4_addr);

		for(cnt = 0; cnt < num_wan_client_tmp; cnt++)
		{
			if (get_client_memptr(wan_client, cnt)->ipv4_set)
			{
				IPACMDBG_H("stored IPv4 %x\n", get_client_memptr(wan_client, cnt)->v4_addr);

				if(ipv4_addr == get_client_memptr(wan_client, cnt)->v4_addr)
				{
					IPACMDBG_H("Matched client index: %d\n", cnt);
					IPACMDBG_H("The MAC is %02x:%02x:%02x:%02x:%02x:%02x\n",
							get_client_memptr(wan_client, cnt)->mac[0],
							get_client_memptr(wan_client, cnt)->mac[1],
							get_client_memptr(wan_client, cnt)->mac[2],
							get_client_memptr(wan_client, cnt)->mac[3],
							get_client_memptr(wan_client, cnt)->mac[4],
							get_client_memptr(wan_client, cnt)->mac[5]);
					IPACMDBG_H("header set ipv4(%d) ipv6(%d)\n",
							get_client_memptr(wan_client, cnt)->ipv4_header_set,
							get_client_memptr(wan_client, cnt)->ipv6_header_set);
					return cnt;
				}
			}
		}
		return IPACM_INVALID_INDEX;
	}

	inline int get_wan_client_index_ipv6(uint32_t* ipv6_addr)
	{
		int cnt, v6_num;
		int num_wan_client_tmp = num_wan_client;

		IPACMDBG_H("Get ipv6 address 0x%08x.0x%08x.0x%08x.0x%08x\n", ipv6_addr[0], ipv6_addr[1], ipv6_addr[2], ipv6_addr[3]);

		for(cnt = 0; cnt < num_wan_client_tmp; cnt++)
		{
			if (get_client_memptr(wan_client, cnt)->ipv6_set)
			{
			    for(v6_num=0;v6_num < get_client_memptr(wan_client, cnt)->ipv6_set;v6_num++)
	            {

					IPACMDBG_H("stored IPv6 0x%08x.0x%08x.0x%08x.0x%08x\n", get_client_memptr(wan_client, cnt)->v6_addr[v6_num][0],
						get_client_memptr(wan_client, cnt)->v6_addr[v6_num][1],
						get_client_memptr(wan_client, cnt)->v6_addr[v6_num][2],
						get_client_memptr(wan_client, cnt)->v6_addr[v6_num][3]);

					if(ipv6_addr[0] == get_client_memptr(wan_client, cnt)->v6_addr[v6_num][0] &&
					   ipv6_addr[1] == get_client_memptr(wan_client, cnt)->v6_addr[v6_num][1] &&
					   ipv6_addr[2]== get_client_memptr(wan_client, cnt)->v6_addr[v6_num][2] &&
					   ipv6_addr[3] == get_client_memptr(wan_client, cnt)->v6_addr[v6_num][3])
					{
						IPACMDBG_H("Matched client index: %d\n", cnt);
						IPACMDBG_H("The MAC is %02x:%02x:%02x:%02x:%02x:%02x\n",
								get_client_memptr(wan_client, cnt)->mac[0],
								get_client_memptr(wan_client, cnt)->mac[1],
								get_client_memptr(wan_client, cnt)->mac[2],
								get_client_memptr(wan_client, cnt)->mac[3],
								get_client_memptr(wan_client, cnt)->mac[4],
								get_client_memptr(wan_client, cnt)->mac[5]);
						IPACMDBG_H("header set ipv4(%d) ipv6(%d)\n",
								get_client_memptr(wan_client, cnt)->ipv4_header_set,
								get_client_memptr(wan_client, cnt)->ipv6_header_set);
						return cnt;
					}
				}
			}
		}
		return IPACM_INVALID_INDEX;
	}

	inline int delete_wan_rtrules(int clt_indx, ipa_ip_type iptype)
	{
		uint32_t tx_index;
		uint32_t rt_hdl;
		int num_v6;

		if(iptype == IPA_IP_v4)
		{
		     for(tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
		     {
		        if((tx_prop->tx[tx_index].ip == IPA_IP_v4) && (get_client_memptr(wan_client, clt_indx)->route_rule_set_v4==true)) /* for ipv4 */
			{
				IPACMDBG_H("Delete client index %d ipv4 Qos rules for tx:%d \n",clt_indx,tx_index);
				rt_hdl = get_client_memptr(wan_client, clt_indx)->wan_rt_hdl[tx_index].wan_rt_rule_hdl_v4;

				if(m_routing.DeleteRoutingHdl(rt_hdl, IPA_IP_v4) == false)
				{
					return IPACM_FAILURE;
				}
			}
		     } /* end of for loop */

		     /* clean the 4 Qos ipv4 RT rules for client:clt_indx */
		     if(get_client_memptr(wan_client, clt_indx)->route_rule_set_v4==true) /* for ipv4 */
		     {
				get_client_memptr(wan_client, clt_indx)->route_rule_set_v4 = false;
		     }
		}

		if(iptype == IPA_IP_v6)
		{
		    for(tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
		    {

				if((tx_prop->tx[tx_index].ip == IPA_IP_v6) && (get_client_memptr(wan_client, clt_indx)->route_rule_set_v6 != 0)) /* for ipv6 */
				{
					for(num_v6 =0;num_v6 < get_client_memptr(wan_client, clt_indx)->route_rule_set_v6;num_v6++)
					{
						IPACMDBG_H("Delete client index %d ipv6 Qos rules for %d-st ipv6 for tx:%d\n", clt_indx,num_v6,tx_index);
						rt_hdl = get_client_memptr(wan_client, clt_indx)->wan_rt_hdl[tx_index].wan_rt_rule_hdl_v6[num_v6];
						if(m_routing.DeleteRoutingHdl(rt_hdl, IPA_IP_v6) == false)
						{
							return IPACM_FAILURE;
						}

						rt_hdl = get_client_memptr(wan_client, clt_indx)->wan_rt_hdl[tx_index].wan_rt_rule_hdl_v6_wan[num_v6];
						if(m_routing.DeleteRoutingHdl(rt_hdl, IPA_IP_v6) == false)
						{
							return IPACM_FAILURE;
						}
					}

				}
			} /* end of for loop */

		    /* clean the 4 Qos ipv6 RT rules for client:clt_indx */
		    if(get_client_memptr(wan_client, clt_indx)->route_rule_set_v6 != 0) /* for ipv6 */
		    {
		                 get_client_memptr(wan_client, clt_indx)->route_rule_set_v6 = 0;
                    }
		}

		return IPACM_SUCCESS;
	}

	int handle_wan_hdr_init(uint8_t *mac_addr, bool gw_addr);
	int handle_wan_client_ipaddr(ipacm_event_data_all *data);
	int handle_wan_client_route_rule(uint8_t *mac_addr, ipa_ip_type iptype);

	/* handle new_address event */
	int handle_addr_evt(ipacm_event_data_addr *data);

	/* wan default route/filter rule configuration */
	int handle_route_add_evt(ipa_ip_type iptype);

#ifdef FEATURE_VLAN_MPDN
	int handle_route_add_vlan_pdn_evt(ipa_ip_type iptype, uint8_t vlan_id);
#endif

	/* construct complete STA ethernet header */
	int handle_sta_header_add_evt();

	bool check_dft_firewall_rules_attr_mask(IPACM_firewall_conf_t *firewall_config);

#ifdef FEATURE_IPA_ANDROID
	/* wan posting supported tether_iface */
	int post_wan_up_tether_evt(ipa_ip_type iptype, int ipa_if_num_tether);

	int post_wan_down_tether_evt(ipa_ip_type iptype, int ipa_if_num_tether);
#endif
	int config_dft_firewall_rules(ipa_ip_type iptype);

	/* configure the initial firewall filter rules */
	int config_dft_embms_rules(ipa_ioc_add_flt_rule *pFilteringTable_v4, ipa_ioc_add_flt_rule *pFilteringTable_v6);

	int handle_route_del_evt(ipa_ip_type iptype);

	int del_dft_firewall_rules(ipa_ip_type iptype);

	int handle_down_evt();

	/*handle wan-iface down event */
	int handle_down_evt_ex();

	/* wan default route/filter rule delete */
	int handle_route_del_evt_ex(ipa_ip_type iptype);

	/* configure the initial firewall filter rules */
#ifdef FEATURE_VLAN_MPDN
	int config_dft_firewall_rules_ex(struct ipacm_pdn_flt_rule* rules, int rule_offset,
		ipa_ip_type iptype);
#else
	int config_dft_firewall_rules_ex(struct ipa_flt_rule_add* rules, int rule_offset,
		ipa_ip_type iptype);
#endif
	/* init filtering rule in wan dl filtering table */
	int init_fl_rule_ex(ipa_ip_type iptype);

	/* add ICMP and ALG rules in wan dl filtering table */
#ifdef FEATURE_VLAN_MPDN
	int add_icmp_alg_rules(struct ipacm_pdn_flt_rule *rules, int rule_offset, ipa_ip_type iptype);
#else
	int add_icmp_alg_rules(struct ipa_flt_rule_add* rules, int rule_offset, ipa_ip_type iptype);
#endif

	/* query extended property */
	int query_ext_prop();

	ipa_ioc_query_intf_ext_props *ext_prop;

	int config_wan_firewall_rule(ipa_ip_type iptype);

	int del_wan_firewall_rule(ipa_ip_type iptype);

#ifdef FEATURE_VLAN_MPDN
	int add_dft_filtering_rule(struct ipacm_pdn_flt_rule *rules, int rule_offset, ipa_ip_type iptype);
#else
	int add_dft_filtering_rule(struct ipa_flt_rule_add* rules, int rule_offset, ipa_ip_type iptype);
#endif

	int install_wan_filtering_rule(bool is_sw_routing);

	void change_to_network_order(ipa_ip_type iptype, ipa_rule_attrib* attrib);

	void handle_wlan_SCC_MCC_switch(bool, ipa_ip_type);

	void handle_wan_client_SCC_MCC_switch(bool, ipa_ip_type);
#ifdef FEATURE_L2TP_E2E
	void handle_l2tp_client_add(char *iface_name);

	void handle_l2tp_client_del(char *iface_name);

	void install_l2tp_flt_rule(ipa_flt_rule_add* rules, int rule_offset, char *iface_name);
#endif
	int handle_network_stats_evt();

	int m_fd_ipa;

	int handle_network_stats_update(ipa_get_apn_data_stats_resp_msg_v01 *data);

	/* construct dummy ethernet header */
	int add_dummy_rx_hdr();

	void HandleSTAClientDelEvt(const ipa_wan_client* client);
	
	int add_catchup_all_filtering_rule_each_pdn(const IPACM_firewall_conf_t& firewall_config, ipa_ip_type iptype,
		const struct ipa_rule_attrib& rx_prop_attrib, struct ipa_flt_rule_add& flt_rule_add, int fltr_rule_number);
	int add_ipv6_frag_filtering_rule_ex(const struct ipa_rule_attrib& rx_prop_attrib,
		struct ipa_flt_rule_add& flt_rule_add, int fltr_rule_number);
#ifndef FEATURE_VLAN_MPDN
	int add_firewall_rules_ex(const IPACM_firewall_conf_t& firewall_config, ipa_ip_type iptype,
		const struct ipa_rule_attrib& rx_prop_attrib, struct ipa_flt_rule_add *rules, int rules_size, int& pos);
#else
	int add_firewall_rules_ex(const IPACM_firewall_conf_t& firewall_config, ipa_ip_type iptype, uint8_t curr_mux_id,
		const struct ipa_rule_attrib& rx_prop_attrib, ipacm_pdn_flt_rule* rules, int rules_size, int& pos);
#endif
};

#endif /* IPACM_WAN_H */
