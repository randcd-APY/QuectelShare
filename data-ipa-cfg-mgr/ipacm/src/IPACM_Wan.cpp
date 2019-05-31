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
		IPACM_Wan.cpp

		@brief
		This file implements the WAN iface functionality.

		@Author
		Skylar Chang

*/
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <IPACM_Wan.h>
#include <IPACM_Xml.h>
#include <IPACM_Log.h>
#include "IPACM_EvtDispatcher.h"
#include <IPACM_IfaceManager.h>
#include "linux/rmnet_ipa_fd_ioctl.h"
#include "IPACM_Config.h"
#include "IPACM_Defs.h"
#include <IPACM_ConntrackListener.h>
#include "linux/ipa_qmi_service_v01.h"

bool IPACM_Wan::wan_up = false;
bool IPACM_Wan::wan_up_v6 = false;
uint8_t IPACM_Wan::xlat_mux_id = 0;

uint32_t IPACM_Wan::curr_wan_ip = 0;
int IPACM_Wan::num_v4_flt_rule = 0;
int IPACM_Wan::num_v6_flt_rule = 0;
#ifdef FEATURE_VLAN_MPDN
int IPACM_Wan::ipv6_mpdn_default_filterting_rules_count = 0;
#endif

int IPACM_Wan::ipa_pm_q6_check = 0;

#ifdef FEATURE_IPACM_UL_FIREWALL
int IPACM_Wan::num_firewall_v6_ul = 0;
#endif //FEATURE_IPACM_UL_FIREWALL

#ifdef FEATURE_VLAN_MPDN
struct ipacm_pdn_flt_rule IPACM_Wan::pdn_flt_rule_v4[IPA_MAX_FLT_RULE];
struct ipacm_pdn_flt_rule IPACM_Wan::pdn_flt_rule_v6[IPA_MAX_FLT_RULE];
#else
struct ipa_flt_rule_add IPACM_Wan::flt_rule_v4[IPA_MAX_FLT_RULE];
struct ipa_flt_rule_add IPACM_Wan::flt_rule_v6[IPA_MAX_FLT_RULE];
#endif

#ifdef FEATURE_IPACM_UL_FIREWALL
struct ipa_flt_rule_add IPACM_Wan::firewall_flt_rule_v6_ul[IPACM_MAX_FIREWALL_ENTRIES+1];
#endif //FEATURE_IPACM_UL_FIREWALL

char IPACM_Wan::wan_up_dev_name[IF_NAME_LEN];

bool IPACM_Wan::backhaul_is_sta_mode = false;
bool IPACM_Wan::is_ext_prop_set = false;

uint32_t IPACM_Wan::wan_route_rule_v6_hdl_a5;

int IPACM_Wan::num_ipv4_modem_pdn = 0;
int IPACM_Wan::num_ipv6_modem_pdn = 0;

bool IPACM_Wan::embms_is_on = false;
bool IPACM_Wan::backhaul_is_wan_bridge = false;

uint32_t IPACM_Wan::backhaul_ipv6_prefix[2];

#ifdef FEATURE_IPACM_UL_FIREWALL
#ifdef FEATURE_VLAN_MPDN
IPACM_firewall_t IPACM_Wan::firewall_mpdn_config_ul;
#endif
IPACM_firewall_conf_t IPACM_Wan::firewall_config_ul;

int IPACM_Wan::m_fd_ipa_ul = 0;
#endif //FEATURE_IPACM_UL_FIREWALL

#ifdef FEATURE_IPA_ANDROID
int	IPACM_Wan::ipa_if_num_tether_v4_total = 0;
int	IPACM_Wan::ipa_if_num_tether_v6_total = 0;

int	IPACM_Wan::ipa_if_num_tether_v4[IPA_MAX_IFACE_ENTRIES];
int	IPACM_Wan::ipa_if_num_tether_v6[IPA_MAX_IFACE_ENTRIES];
#endif
#ifdef FEATURE_VLAN_MPDN
ipacm_ipv4_wan_iface IPACM_Wan::ipv4_to_iface[IPA_MAX_NUM_SW_PDNS];
ipacm_ipv6_wan_iface IPACM_Wan::ipv6_to_iface[IPA_MAX_NUM_SW_PDNS];
#endif

#define MOBILE_FIREWALL_FILE "/etc/data/mobileap_firewall.xml"

IPACM_Wan::IPACM_Wan(int iface_index,
	ipacm_wan_iface_type is_sta_mode,
	uint8_t *mac_addr) : IPACM_Iface(iface_index)
{
	num_firewall_v4 = 0;
	num_firewall_v6 = 0;
	wan_route_rule_v4_hdl = NULL;
	wan_route_rule_v6_hdl = NULL;
	wan_client = NULL;
	wan_client_len = 0;
	is_default_gateway = true;

	if(iface_query != NULL)
	{
		wan_route_rule_v4_hdl = (uint32_t *)calloc(iface_query->num_tx_props, sizeof(uint32_t));
		wan_route_rule_v6_hdl = (uint32_t *)calloc(iface_query->num_tx_props, sizeof(uint32_t));
		IPACMDBG_H("IPACM->IPACM_Wan(%d) constructor: Tx:%d\n", ipa_if_num, iface_query->num_tx_props);
	}
	m_is_sta_mode = is_sta_mode;

	/* Used to store the Public IP info in IP passthrough mode. */
	wan_v4_addr = 0;
	wan_v4_addr_gw = 0;
	public_wan_v4_addr = 0;
	public_wan_v4_addr_set = false;
	wan_v4_addr_set = false;
	wan_v4_addr_gw_set = false;
	wan_v6_addr_gw_set = false;
	active_v4 = false;
	active_v6 = false;
	header_set_v4 = false;
	header_set_v6 = false;
	header_partial_default_wan_v4 = false;
	header_partial_default_wan_v6 = false;
	hdr_hdl_sta_v4 = 0;
	hdr_hdl_sta_v6 = 0;
	num_ipv6_dest_flt_rule = 0;
	memset(ipv6_dest_flt_rule_hdl, 0, MAX_DEFAULT_v6_ROUTE_RULES*sizeof(uint32_t));
	memset(ipv6_prefix, 0, sizeof(ipv6_prefix));
	memset(m_ipv6_addr, 0, sizeof(m_ipv6_addr));
	memset(wan_v6_addr_gw, 0, sizeof(wan_v6_addr_gw));
	ext_prop = NULL;
	is_ipv6_frag_firewall_flt_rule_installed = false;

#ifdef FEATURE_IPACM_UL_FIREWALL
#ifdef FEATURE_VLAN_MPDN
	num_firewall_v6_ul_pdn = 0;
#endif
#endif //FEATURE_IPACM_UL_FIREWALL
	ipv6_frag_firewall_flt_rule_hdl = 0;

	num_wan_client = 0;
	header_name_count = 0;
	memset(invalid_mac, 0, sizeof(invalid_mac));

	is_xlat = false;
	hdr_hdl_dummy_v6 = 0;
	hdr_proc_hdl_dummy_v6 = 0;

	modem_ipv6_pdn_index = -1;
	modem_ipv4_pdn_index = -1;

#ifdef FEATURE_VLAN_MPDN
	associated_VID = 0;
#endif

	if(m_is_sta_mode == Q6_WAN)
	{
		IPACMDBG_H("The new WAN interface is modem.\n");
		is_default_gateway = false;
		query_ext_prop();
	}
	else
	{
		IPACMDBG_H("The new WAN interface is WLAN STA.\n");
	}

	m_fd_ipa = open(IPA_DEVICE_NAME, O_RDWR);
	if(0 == m_fd_ipa)
	{
		IPACMERR("Failed to open %s\n",IPA_DEVICE_NAME);
	}
	if(iface_query != NULL)
	{
		wan_client_len = (sizeof(ipa_wan_client)) + (iface_query->num_tx_props * sizeof(wan_client_rt_hdl));
		wan_client = (ipa_wan_client *)calloc(IPA_MAX_NUM_WAN_CLIENTS, wan_client_len);
		if (wan_client == NULL)
		{
			IPACMERR("unable to allocate memory\n");
			close(m_fd_ipa);
			return;
		}
		IPACMDBG_H("index:%d constructor: Tx properties:%d\n", iface_index, iface_query->num_tx_props);
	}
#ifdef FEATURE_IPACM_UL_FIREWALL
	m_fd_ipa_ul = m_fd_ipa;
#endif //FEATURE_IPACM_UL_FIREWALL
	if(IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_cat == EMBMS_IF)
	{
		IPACMDBG(" IPACM->IPACM_Wan_eMBMS(%d)\n", ipa_if_num);
		embms_is_on = true;
		install_wan_filtering_rule(false);
		if(IPACM_Iface::ipacmcfg->GetIPAVer() >= IPA_HW_None && IPACM_Iface::ipacmcfg->GetIPAVer() < IPA_HW_v4_0)
		{
			/* Add corresponding ipa_rm_resource_name of TX-endpoint up before IPV6 RT-rule set */
			if(tx_prop != NULL)
			{
				IPACMDBG_H("dev %s add producer dependency\n", dev_name);
				IPACMDBG_H("depend Got pipe %d rm index : %d \n", tx_prop->tx[0].dst_pipe, IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
				IPACM_Iface::ipacmcfg->AddRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe],false);
			}
		}
	}
	else
	{
		IPACMDBG(" IPACM->IPACM_Wan(%d)\n", ipa_if_num);
	}

	return;
}

IPACM_Wan::~IPACM_Wan()
{
	IPACM_EvtDispatcher::deregistr(this);
	IPACM_IfaceManager::deregistr(this);
	return;
}

#ifdef FEATURE_VLAN_MPDN

int IPACM_Wan::GetMuxByVid(uint8_t vlan_id, uint8_t *mux_id, ipa_ip_type iptype)
{
	for(int i = 0; i < IPA_MAX_NUM_SW_PDNS; i++)
	{
		if(iptype == IPA_IP_v4)
		{
			if(IPACM_Wan::ipv4_to_iface[i].ipv4_addr)
			{
				if(IPACM_Wan::ipv4_to_iface[i].pIface->associated_VID == vlan_id)
				{
					*mux_id = IPACM_Wan::ipv4_to_iface[i].pIface->ext_prop->ext[0].mux_id;
					return IPACM_SUCCESS;
				}
			}
		}
		else
		{
			if(IPACM_Wan::ipv6_to_iface[i].ipv6_prefix[0] || IPACM_Wan::ipv6_to_iface[i].ipv6_prefix[1])
			{
				if(IPACM_Wan::ipv6_to_iface[i].pIface->associated_VID == vlan_id)
				{
					*mux_id = IPACM_Wan::ipv6_to_iface[i].pIface->ext_prop->ext[0].mux_id;
					return IPACM_SUCCESS;
				}
			}
		}
	}
	IPACMERR("couldn't find MUX for VID %d\n", vlan_id);
	return IPACM_FAILURE;
}
#endif
/* handle new_address event */
int IPACM_Wan::handle_addr_evt(ipacm_event_data_addr *data)
{
	struct ipa_ioc_add_rt_rule *rt_rule;
	struct ipa_rt_rule_add *rt_rule_entry;
	struct ipa_ioc_add_flt_rule *flt_rule;
	struct ipa_flt_rule_add flt_rule_entry;
	struct ipa_ioc_get_hdr hdr;

	const int NUM_RULES = 1;
	int num_ipv6_addr, len;
	int res = IPACM_SUCCESS;

	memset(&hdr, 0, sizeof(hdr));
	if(tx_prop == NULL || rx_prop == NULL)
	{
		IPACMDBG_H("Either tx or rx property is NULL, return.\n");
		return IPACM_SUCCESS;
	}

	/* Update the IP Type. */
	config_ip_type(data->iptype);

	if (data->iptype == IPA_IP_v6)
	{
		for(num_ipv6_addr=0;num_ipv6_addr<num_dft_rt_v6;num_ipv6_addr++)
		{
			if((ipv6_addr[num_ipv6_addr][0] == data->ipv6_addr[0]) &&
			   (ipv6_addr[num_ipv6_addr][1] == data->ipv6_addr[1]) &&
			   (ipv6_addr[num_ipv6_addr][2] == data->ipv6_addr[2]) &&
			   (ipv6_addr[num_ipv6_addr][3] == data->ipv6_addr[3]))
			{
				IPACMDBG_H("find matched ipv6 address, index:%d \n", num_ipv6_addr);
				return IPACM_SUCCESS;
				break;
			}
		}
		rt_rule = (struct ipa_ioc_add_rt_rule *)
			calloc(1, sizeof(struct ipa_ioc_add_rt_rule) +
				NUM_RULES * sizeof(struct ipa_rt_rule_add));

		if (!rt_rule)
		{
			IPACMERR("Error Locate ipa_ioc_add_rt_rule memory...\n");
			return IPACM_FAILURE;
		}

		rt_rule->commit = 1;
		rt_rule->num_rules = NUM_RULES;
		rt_rule->ip = data->iptype;
		strlcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_v6.name, sizeof(rt_rule->rt_tbl_name));

		rt_rule_entry = &rt_rule->rules[0];
		if(m_is_sta_mode == Q6_WAN)
		{
			strlcpy(hdr.name, tx_prop->tx[0].hdr_name, sizeof(hdr.name));
			hdr.name[IPA_RESOURCE_NAME_MAX-1] = '\0';
			if(m_header.GetHeaderHandle(&hdr) == false)
			{
				IPACMERR("Failed to get QMAP header.\n");
				return IPACM_FAILURE;
			}
			rt_rule_entry->rule.hdr_hdl = hdr.hdl;
		}
		rt_rule_entry->at_rear = false;
		if(m_is_sta_mode == Q6_WAN)
		{
			rt_rule_entry->rule.dst = IPA_CLIENT_APPS_WAN_CONS;
		}
		else
		{
			rt_rule_entry->rule.dst = IPA_CLIENT_APPS_LAN_CONS;
		}
		rt_rule_entry->rule.attrib.attrib_mask = IPA_FLT_DST_ADDR;
		rt_rule_entry->rule.attrib.u.v6.dst_addr[0] = data->ipv6_addr[0];
		rt_rule_entry->rule.attrib.u.v6.dst_addr[1] = data->ipv6_addr[1];
		rt_rule_entry->rule.attrib.u.v6.dst_addr[2] = data->ipv6_addr[2];
		rt_rule_entry->rule.attrib.u.v6.dst_addr[3] = data->ipv6_addr[3];
		rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[0] = 0xFFFFFFFF;
		rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[1] = 0xFFFFFFFF;
		rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[2] = 0xFFFFFFFF;
		rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[3] = 0xFFFFFFFF;
		ipv6_addr[num_dft_rt_v6][0] = data->ipv6_addr[0];
		ipv6_addr[num_dft_rt_v6][1] = data->ipv6_addr[1];
		ipv6_addr[num_dft_rt_v6][2] = data->ipv6_addr[2];
		ipv6_addr[num_dft_rt_v6][3] = data->ipv6_addr[3];
#ifdef FEATURE_IPA_V3
		rt_rule_entry->rule.hashable = false;
#endif
		if (false == m_routing.AddRoutingRule(rt_rule))
		{
			IPACMERR("Routing rule addition failed!\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else if (rt_rule_entry->status)
		{
			IPACMERR("rt rule adding failed. Result=%d\n", rt_rule_entry->status);
			res = rt_rule_entry->status;
			goto fail;
		}
		dft_rt_rule_hdl[MAX_DEFAULT_v4_ROUTE_RULES + 2*num_dft_rt_v6] = rt_rule_entry->rt_rule_hdl;

		/* setup same rule for v6_wan table*/
		strlcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_wan_v6.name, sizeof(rt_rule->rt_tbl_name));
		if (false == m_routing.AddRoutingRule(rt_rule))
		{
			IPACMERR("Routing rule addition failed!\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else if (rt_rule_entry->status)
		{
			IPACMERR("rt rule adding failed. Result=%d\n", rt_rule_entry->status);
			res = rt_rule_entry->status;
			goto fail;
		}
		dft_rt_rule_hdl[MAX_DEFAULT_v4_ROUTE_RULES + 2*num_dft_rt_v6+1] = rt_rule_entry->rt_rule_hdl;

		IPACMDBG_H("ipv6 wan iface rt-rule hdl=0x%x hdl=0x%x, num_dft_rt_v6: %d \n",
				dft_rt_rule_hdl[MAX_DEFAULT_v4_ROUTE_RULES + 2*num_dft_rt_v6],
				dft_rt_rule_hdl[MAX_DEFAULT_v4_ROUTE_RULES + 2*num_dft_rt_v6+1],num_dft_rt_v6);

		/* add default filtering rules when wan-iface get global v6-prefix */
		if (num_dft_rt_v6 == 1)
		{
			if(m_is_sta_mode == Q6_WAN)
			{
				modem_ipv6_pdn_index = getFreePDNIndex_V6();
				if (modem_ipv6_pdn_index == -1)
				{
					IPACMERR("No Free index available.!\n");
					res = IPACM_FAILURE;
					goto fail;
				}
				num_ipv6_modem_pdn++;
				IPACMDBG_H("Now the number of modem ipv6 pdn is %d.\n", num_ipv6_modem_pdn);
				init_fl_rule_ex(data->iptype);
			}
			else
			{
				init_fl_rule(data->iptype);
			}
		}

		/* add WAN DL interface IP specific flt rule for IPv6 when backhaul is not Q6 */
		if(m_is_sta_mode != Q6_WAN)
		{
			if(rx_prop != NULL && is_global_ipv6_addr(data->ipv6_addr)
				&& num_ipv6_dest_flt_rule < MAX_DEFAULT_v6_ROUTE_RULES)
			{
				len = sizeof(struct ipa_ioc_add_flt_rule) + sizeof(struct ipa_flt_rule_add);

				flt_rule = (struct ipa_ioc_add_flt_rule *)calloc(1, len);
				if (!flt_rule)
				{
					IPACMERR("Error Locate ipa_flt_rule_add memory...\n");
					return IPACM_FAILURE;
				}

				flt_rule->commit = 1;
				flt_rule->ep = rx_prop->rx[0].src_pipe;
				flt_rule->global = false;
				flt_rule->ip = IPA_IP_v6;
				flt_rule->num_rules = 1;

				memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

				flt_rule_entry.rule.retain_hdr = 1;
				flt_rule_entry.rule.to_uc = 0;
				flt_rule_entry.rule.eq_attrib_type = 0;
				flt_rule_entry.at_rear = true;
				flt_rule_entry.flt_rule_hdl = -1;
				flt_rule_entry.status = -1;
				flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;
#ifdef FEATURE_IPA_V3
				flt_rule_entry.rule.hashable = true;
#endif
				memcpy(&flt_rule_entry.rule.attrib, &rx_prop->rx[0].attrib, sizeof(flt_rule_entry.rule.attrib));

				flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
				memcpy(flt_rule_entry.rule.attrib.u.v6.dst_addr, data->ipv6_addr, sizeof(flt_rule_entry.rule.attrib.u.v6.dst_addr));
				flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[0] = 0xFFFFFFFF;
				flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[1] = 0xFFFFFFFF;
				flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[2] = 0xFFFFFFFF;
				flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[3] = 0xFFFFFFFF;
				memcpy(&(flt_rule->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

				if (m_filtering.AddFilteringRule(flt_rule) == false)
				{
					IPACMERR("Error Adding Filtering rule, aborting...\n");
					free(flt_rule);
					res = IPACM_FAILURE;
					goto fail;
				}
				else
				{
					IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, 1);
					ipv6_dest_flt_rule_hdl[num_ipv6_dest_flt_rule] = flt_rule->rules[0].flt_rule_hdl;
					IPACMDBG_H("IPv6 dest filter rule %d HDL:0x%x\n", num_ipv6_dest_flt_rule, ipv6_dest_flt_rule_hdl[num_ipv6_dest_flt_rule]);
					num_ipv6_dest_flt_rule++;
					free(flt_rule);
				}
			}
		}
		/* store ipv6 prefix if the ipv6 address is not link local */
		if(is_global_ipv6_addr(data->ipv6_addr))
		{
			memcpy(ipv6_prefix, data->ipv6_addr, sizeof(ipv6_prefix));
			memcpy(m_ipv6_addr, data->ipv6_addr, sizeof(m_ipv6_addr));
#ifdef FEATURE_VLAN_MPDN
			if(m_is_sta_mode == Q6_WAN)
			{
				memcpy(ipv6_to_iface[modem_ipv6_pdn_index].ipv6_prefix, data->ipv6_addr, sizeof(uint32_t) * 2);
				ipv6_to_iface[modem_ipv6_pdn_index].pIface = this;
			}
#endif
		}
	    num_dft_rt_v6++;
    }
	else
	{
		if(wan_v4_addr_set)
		{
			/* check iface ipv4 same or not */
			if(data->ipv4_addr == wan_v4_addr)
			{
				IPACMDBG_H("Already setup device (%s) ipv4 and it didn't change(0x%x)\n", dev_name, data->ipv4_addr);
				return IPACM_SUCCESS;
			}
			else
			{
				IPACMDBG_H(" device (%s) ipv4 addr is changed\n", dev_name);
				/* Delete default v4 RT rule */
				IPACMDBG_H("Delete default v4 routing rules\n");
				if (m_routing.DeleteRoutingHdl(dft_rt_rule_hdl[0], IPA_IP_v4) == false)
				{
					IPACMERR("Routing old RT rule deletion failed!\n");
					res = IPACM_FAILURE;
					goto fail;
				}
			}
		}

		rt_rule = (struct ipa_ioc_add_rt_rule *)
			 calloc(1, sizeof(struct ipa_ioc_add_rt_rule) +
							NUM_RULES * sizeof(struct ipa_rt_rule_add));

		if (!rt_rule)
		{
			IPACMERR("Error Locate ipa_ioc_add_rt_rule memory...\n");
			return IPACM_FAILURE;
		}

		rt_rule->commit = 1;
		rt_rule->num_rules = NUM_RULES;
		rt_rule->ip = data->iptype;
		rt_rule_entry = &rt_rule->rules[0];
		if(m_is_sta_mode == Q6_WAN)
		{
			strlcpy(hdr.name, tx_prop->tx[0].hdr_name, sizeof(hdr.name));
			hdr.name[IPA_RESOURCE_NAME_MAX-1] = '\0';
			if(m_header.GetHeaderHandle(&hdr) == false)
			{
				IPACMERR("Failed to get QMAP header.\n");
				return IPACM_FAILURE;
			}
			rt_rule_entry->rule.hdr_hdl = hdr.hdl;
			rt_rule_entry->rule.dst = IPA_CLIENT_APPS_WAN_CONS;
		}
		else
		{
			rt_rule_entry->rule.dst = IPA_CLIENT_APPS_LAN_CONS;
		}
		rt_rule_entry->at_rear = false;
		rt_rule_entry->rule.attrib.attrib_mask = IPA_FLT_DST_ADDR;
		/* still need setup v4 default routing rule to A5*/
		strlcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_lan_v4.name, sizeof(rt_rule->rt_tbl_name));
		rt_rule_entry->rule.attrib.u.v4.dst_addr      = data->ipv4_addr;
		rt_rule_entry->rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;
#ifdef FEATURE_IPA_V3
		rt_rule_entry->rule.hashable = false;
#endif
		if (false == m_routing.AddRoutingRule(rt_rule))
		{
			IPACMERR("Routing rule addition failed!\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else if (rt_rule_entry->status)
		{
			IPACMERR("rt rule adding failed. Result=%d\n", rt_rule_entry->status);
			res = rt_rule_entry->status;
			goto fail;
		}
		dft_rt_rule_hdl[0] = rt_rule_entry->rt_rule_hdl;
		IPACMDBG_H("ipv4 wan iface rt-rule hdll=0x%x\n", dft_rt_rule_hdl[0]);
			/* initial multicast/broadcast/fragment filter rule */

		/* only do one time */
		if(!wan_v4_addr_set)
		{
			/* initial multicast/broadcast/fragment filter rule */
			if(m_is_sta_mode == Q6_WAN)
			{
				modem_ipv4_pdn_index = getFreePDNIndex_V4();
				if (modem_ipv4_pdn_index == -1)
				{
					IPACMERR("No Free index available.!\n");
					res = IPACM_FAILURE;
					goto fail;
				}
#ifdef FEATURE_VLAN_MPDN
				ipv4_to_iface[modem_ipv4_pdn_index].ipv4_addr = data->ipv4_addr;
				ipv4_to_iface[modem_ipv4_pdn_index].pIface = this;
#endif
				num_ipv4_modem_pdn++;
				IPACMDBG_H("Now the number of modem ipv4 pdn is %d.\n", num_ipv4_modem_pdn);
				init_fl_rule_ex(data->iptype);
			}
			else
			{
				init_fl_rule(data->iptype);
			}
		}

		/* Store the public ip address when in passthrough mode which will be used when wan is down. */
		if ((m_is_sta_mode == Q6_WAN) && (is_default_gateway == true) &&
			data->ipv4_addr == inet_network(IPACM_IPPASSTHROUGH_WAN_IP))
		{
			curr_wan_ip = data->ipv4_addr;
			public_wan_v4_addr = wan_v4_addr;
			public_wan_v4_addr_set = true;
			IPACMDBG_H("In Passthrough mode, Storing previous wan ipv4-addr:0x%x\n",public_wan_v4_addr);
		}
		else
		{
			IPACMDBG_H("Not in passthrough mode, reset previous wan ipv4-addr:0x%x\n",public_wan_v4_addr);
			public_wan_v4_addr = 0;
			public_wan_v4_addr_set = false;
		}

		wan_v4_addr = data->ipv4_addr;
		wan_v4_addr_set = true;

		IPACMDBG_H("Receved wan ipv4-addr:0x%x\n",wan_v4_addr);
	}

	IPACMDBG_H("number of default route rules %d\n", num_dft_rt_v6);

fail:
	free(rt_rule);

	return res;
}

void IPACM_Wan::event_callback(ipa_cm_event_id event, void *param)
{
	int ipa_interface_index, cnt;

	switch (event)
	{
	case IPA_WLAN_LINK_DOWN_EVENT:
		{
			if(m_is_sta_mode == WLAN_WAN)
			{
				ipacm_event_data_fid *data = (ipacm_event_data_fid *)param;
				ipa_interface_index = iface_ipa_index_query(data->if_index);
				if (ipa_interface_index == ipa_if_num)
				{
					IPACMDBG_H("Received IPA_WLAN_LINK_DOWN_EVENT\n");
					handle_down_evt();
					/* reset the STA-iface category to unknown */
					IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_cat = UNKNOWN_IF;
					IPACMDBG_H("IPA_WAN_STA (%s):ipa_index (%d) instance close \n", IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].iface_name, ipa_if_num);
					IPACM_Iface::ipacmcfg->DelNatIfaces(dev_name); // delete NAT-iface
					delete this;
					return;
				}
			}
		}
		break;

	case IPA_WAN_XLAT_CONNECT_EVENT:
		{
			IPACMDBG_H("Recieved IPA_WAN_XLAT_CONNECT_EVENT\n");
			ipacm_event_data_fid *data = (ipacm_event_data_fid *)param;
			ipa_interface_index = IPACM_Iface::iface_ipa_index_query(data->if_index);
			if ((ipa_interface_index == ipa_if_num) && (m_is_sta_mode == Q6_WAN))
			{
				is_xlat = true;
				IPACMDBG_H("WAN-LTE (%s) link up, iface: %d is_xlat: \n",
						IPACM_Iface::ipacmcfg->iface_table[ipa_interface_index].iface_name,data->if_index, is_xlat);
			}
			break;
		}
	case IPA_CFG_CHANGE_EVENT:
		{
			if ( (IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_cat == ipa_if_cate) &&
					(m_is_sta_mode ==ECM_WAN))
			{
				IPACMDBG_H("Received IPA_CFG_CHANGE_EVENT and category did not change(wan_mode:%d)\n", m_is_sta_mode);
				IPACMDBG_H("Now the cradle wan mode is %d.\n", IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_mode);
				if(is_default_gateway == true)
				{
					if(backhaul_is_wan_bridge == false && IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_mode == BRIDGE)
					{
						IPACMDBG_H("Cradle wan mode switch to bridge mode.\n");
						backhaul_is_wan_bridge = true;
					}
					else if(backhaul_is_wan_bridge == true && IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_mode == ROUTER)
					{
						IPACMDBG_H("Cradle wan mode switch to router mode.\n");
						backhaul_is_wan_bridge = false;
					}
					else
					{
						IPACMDBG_H("No cradle mode switch, return.\n");
						return;
					}
					/* post wan mode change event to LAN/WLAN */
					if(IPACM_Wan::wan_up == true)
					{
						IPACMDBG_H("This interface is default GW.\n");
						ipacm_cmd_q_data evt_data;
						memset(&evt_data, 0, sizeof(evt_data));

						ipacm_event_cradle_wan_mode *data_wan_mode = NULL;
						data_wan_mode = (ipacm_event_cradle_wan_mode *)malloc(sizeof(ipacm_event_cradle_wan_mode));
						if(data_wan_mode == NULL)
						{
							IPACMERR("unable to allocate memory.\n");
							return;
						}
						data_wan_mode->cradle_wan_mode = IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_mode;
						evt_data.event = IPA_CRADLE_WAN_MODE_SWITCH;
						evt_data.evt_data = data_wan_mode;
						IPACMDBG_H("Posting IPA_CRADLE_WAN_MODE_SWITCH event.\n");
						IPACM_EvtDispatcher::PostEvt(&evt_data);
					}
					/* update the firewall flt rule actions */
					if(active_v4)
					{
						del_dft_firewall_rules(IPA_IP_v4);
						config_dft_firewall_rules(IPA_IP_v4);
					}
					if(active_v6)
					{
						del_dft_firewall_rules(IPA_IP_v6);
						config_dft_firewall_rules(IPA_IP_v6);
					}
				}
				else
				{
					IPACMDBG_H("This interface is not default GW, ignore.\n");
				}
			}
			else if ( (IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_cat != ipa_if_cate) &&
					(m_is_sta_mode ==ECM_WAN))
			{
				IPACMDBG_H("Received IPA_CFG_CHANGE_EVENT and category changed(wan_mode:%d)\n", m_is_sta_mode);
				/* posting link-up event for cradle use-case */
				ipacm_cmd_q_data evt_data;
				memset(&evt_data, 0, sizeof(evt_data));

				ipacm_event_data_fid *data_fid = NULL;
				data_fid = (ipacm_event_data_fid *)malloc(sizeof(ipacm_event_data_fid));
				if(data_fid == NULL)
				{
					IPACMERR("unable to allocate memory for IPA_USB_LINK_UP_EVENT data_fid\n");
					return;
				}
				if(IPACM_Iface::ipa_get_if_index(dev_name, &(data_fid->if_index)))
				{
					IPACMERR("Error while getting interface index for %s device", dev_name);
				}
				evt_data.event = IPA_USB_LINK_UP_EVENT;
				evt_data.evt_data = data_fid;
				IPACMDBG_H("Posting event:%d\n", evt_data.event);
				IPACM_EvtDispatcher::PostEvt(&evt_data);

				/* delete previous instance */
				handle_down_evt();
				IPACM_Iface::ipacmcfg->DelNatIfaces(dev_name); // delete NAT-iface
				delete this;
				return;
			}
		}
		break;

	case IPA_LINK_DOWN_EVENT:
		{
			ipacm_event_data_fid *data = (ipacm_event_data_fid *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);
			if (ipa_interface_index == ipa_if_num)
			{
				if(m_is_sta_mode == Q6_WAN)
				{
						IPACMDBG_H("Received IPA_LINK_DOWN_EVENT\n");
						handle_down_evt_ex();
						IPACMDBG_H("IPA_WAN_Q6 (%s):ipa_index (%d) instance close \n", IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].iface_name, ipa_if_num);
						IPACM_Iface::ipacmcfg->DelNatIfaces(dev_name); // delete NAT-iface
						delete this;
						return;
				}
				else if (m_is_sta_mode == ECM_WAN)
				{
					IPACMDBG_H("Received IPA_LINK_DOWN_EVENT(wan_mode:%d)\n", m_is_sta_mode);
					/* delete previous instance */
					handle_down_evt();
					IPACMDBG_H("IPA_WAN_CRADLE (%s):ipa_index (%d) instance close \n", IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].iface_name, ipa_if_num);
					IPACM_Iface::ipacmcfg->DelNatIfaces(dev_name); // delete NAT-iface
					delete this;
					return;
				}
			}
		}
		break;

	case IPA_ADDR_ADD_EVENT:
		{
			ipacm_event_data_addr *data = (ipacm_event_data_addr *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);

			if ( (data->iptype == IPA_IP_v4 && data->ipv4_addr == 0) ||
					 (data->iptype == IPA_IP_v6 &&
						data->ipv6_addr[0] == 0 && data->ipv6_addr[1] == 0 &&
					  data->ipv6_addr[2] == 0 && data->ipv6_addr[3] == 0) )
			{
				IPACMDBG_H("Invalid address, ignore IPA_ADDR_ADD_EVENT event\n");
				return;
			}

			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG_H("Get IPA_ADDR_ADD_EVENT: IF ip type %d, incoming ip type %d\n", ip_type, data->iptype);
				/* check v4 not setup before, v6 can have 2 iface ip */
				if( (data->iptype == IPA_IP_v4)
				    || ((data->iptype==IPA_IP_v6) && (num_dft_rt_v6!=MAX_DEFAULT_v6_ROUTE_RULES)))
				{
					IPACMDBG_H("Got IPA_ADDR_ADD_EVENT ip-family:%d, v6 num %d: \n",data->iptype,num_dft_rt_v6);
					handle_addr_evt(data);
					/* checking if SW-RT_enable */
					if (IPACM_Iface::ipacmcfg->ipa_sw_rt_enable == true &&
							m_is_sta_mode != Q6_WAN)
					{
						/* handle software routing enable event*/
						IPACMDBG_H("IPA_SW_ROUTING_ENABLE for iface: %s \n",IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].iface_name);
						handle_software_routing_enable();
					}

				}
			}
		}
		break;

	case IPA_WAN_UPSTREAM_ROUTE_ADD_EVENT:
		{
			ipacm_event_data_iptype *data = (ipacm_event_data_iptype *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);
			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG_H("Received IPA_WAN_UPSTREAM_ROUTE_ADD_EVENT (Android) for ip-type (%d)\n", data->iptype);
				/* The special below condition is to handle default gateway */
				if ((data->iptype == IPA_IP_v4) && (ip_type == IPA_IP_v4 || ip_type == IPA_IP_MAX))
				{
					if (active_v4 == false)
					{
						IPACMDBG_H("adding routing table(upstream), dev (%s) ip-type(%d)\n", dev_name,data->iptype);
						handle_route_add_evt(data->iptype);
					}
#ifdef FEATURE_IPA_ANDROID
					/* using ipa_if_index, not netdev_index */
					post_wan_up_tether_evt(data->iptype, iface_ipa_index_query(data->if_index_tether));
#endif
				}
				else if ((data->iptype == IPA_IP_v6) && (ip_type == IPA_IP_v6 || ip_type == IPA_IP_MAX))
				{
					if(ipv6_prefix[0] == 0 && ipv6_prefix[1] == 0)
					{
						IPACMDBG_H("IPv6 default route comes earlier than global IP, ignore.\n");
						return;
					}

					if (active_v6 == false)
					{
						IPACMDBG_H("\n get default v6 route (dst:00.00.00.00) upstream\n");
						handle_route_add_evt(data->iptype);
					}
#ifdef FEATURE_IPA_ANDROID
					/* using ipa_if_index, not netdev_index */
					post_wan_up_tether_evt(data->iptype, iface_ipa_index_query(data->if_index_tether));
#endif
				}
			}
			else /* double check if current default iface is not itself */
			{
				if ((data->iptype == IPA_IP_v4) && (active_v4 == true))
				{
					IPACMDBG_H("Received v4 IPA_WAN_UPSTREAM_ROUTE_ADD_EVENT for other iface (%s)\n", IPACM_Iface::ipacmcfg->iface_table[ipa_interface_index].iface_name);
					IPACMDBG_H("need clean default v4 route (dst:0.0.0.0) for old iface (%s)\n", dev_name);
//					wan_v4_addr_gw_set = false; /* android requires CnE change too */
					if(m_is_sta_mode == Q6_WAN)
					{
						del_wan_firewall_rule(IPA_IP_v4);
						install_wan_filtering_rule(false);
						handle_route_del_evt_ex(IPA_IP_v4);
					}
					else
					{
						del_dft_firewall_rules(IPA_IP_v4);
						handle_route_del_evt(IPA_IP_v4);
					}
				}
				else if ((data->iptype == IPA_IP_v6) && (active_v6 == true))
				{
				    IPACMDBG_H("Received v6 IPA_WAN_UPSTREAM_ROUTE_ADD_EVENT for other iface (%s)\n", IPACM_Iface::ipacmcfg->iface_table[ipa_interface_index].iface_name);
					IPACMDBG_H("need clean default v6 route for old iface (%s)\n", dev_name);
					if(m_is_sta_mode == Q6_WAN)
					{
						del_wan_firewall_rule(IPA_IP_v6);
						install_wan_filtering_rule(false);
						handle_route_del_evt_ex(IPA_IP_v6);
					}
					else
					{
						del_dft_firewall_rules(IPA_IP_v6);
						handle_route_del_evt(IPA_IP_v6);
					}
				}
			}
		}
		break;

	case IPA_WAN_UPSTREAM_ROUTE_DEL_EVENT:
		{
			ipacm_event_data_iptype *data = (ipacm_event_data_iptype *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);
			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG_H("Received IPA_WAN_UPSTREAM_ROUTE_DEL_EVENT\n");
				if ((data->iptype == IPA_IP_v4) && (active_v4 == true))
				{
					IPACMDBG_H("get del default v4 route (dst:0.0.0.0)\n");
//					wan_v4_addr_gw_set = false; /* android requires CnE change too */
#ifdef FEATURE_IPA_ANDROID
					/* using ipa_if_index, not netdev_index */
					post_wan_down_tether_evt(data->iptype, iface_ipa_index_query(data->if_index_tether));
					/* no any ipv4 tether iface support*/
					if(IPACM_Wan::ipa_if_num_tether_v4_total != 0)
					{
						IPACMDBG_H("still have tether ipv4 client on upsteam iface\n");
						return;
					}
#endif
					if(m_is_sta_mode == Q6_WAN)
					{
						del_wan_firewall_rule(IPA_IP_v4);
						install_wan_filtering_rule(false);
						handle_route_del_evt_ex(IPA_IP_v4);
					}
					else
					{
						del_dft_firewall_rules(IPA_IP_v4);
						handle_route_del_evt(IPA_IP_v4);
					}
				}
				else if ((data->iptype == IPA_IP_v6) && (active_v6 == true))
				{
#ifdef FEATURE_IPA_ANDROID
					/* using ipa_if_index, not netdev_index */
					post_wan_down_tether_evt(data->iptype, iface_ipa_index_query(data->if_index_tether));
					/* no any ipv6 tether iface support*/
					if(IPACM_Wan::ipa_if_num_tether_v6_total != 0)
					{
						IPACMDBG_H("still have tether ipv6 client on upsteam iface\n");
						return;
					}
#endif
					if(m_is_sta_mode == Q6_WAN)
					{
						del_wan_firewall_rule(IPA_IP_v6);
						install_wan_filtering_rule(false);
						handle_route_del_evt_ex(IPA_IP_v6);
					}
					else
					{
						del_dft_firewall_rules(IPA_IP_v6);
						handle_route_del_evt(IPA_IP_v6);
					}
				}
			}
		}
		break;
	case IPA_NETWORK_STATS_UPDATE_EVENT:
		{
			ipa_get_apn_data_stats_resp_msg_v01 *data = (ipa_get_apn_data_stats_resp_msg_v01 *)param;
			if (!data->apn_data_stats_list_valid)
			{
				IPACMERR("not valid APN\n");
				return;
			}
			else
			{
				handle_network_stats_update(data);
			}
		}
		break;
	case IPA_ROUTE_ADD_EVENT:
		{
			ipacm_event_data_addr *data = (ipacm_event_data_addr *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);
			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG_H("Received IPA_ROUTE_ADD_EVENT\n");
				IPACMDBG_H("ipv4 addr 0x%x\n", data->ipv4_addr);
				IPACMDBG_H("ipv4 addr mask 0x%x\n", data->ipv4_addr_mask);

				/* The special below condition is to handle default gateway */
				if ((data->iptype == IPA_IP_v4) && (!data->ipv4_addr) && (!data->ipv4_addr_mask) && (active_v4 == false)
					&& (ip_type == IPA_IP_v4 || ip_type == IPA_IP_MAX))
				{
					wan_v4_addr_gw = data->ipv4_addr_gw;
					wan_v4_addr_gw_set = true;
					IPACMDBG_H("adding routing table, dev (%s) ip-type(%d), default gw (%x)\n", dev_name,data->iptype, wan_v4_addr_gw);
					/* Check & construct STA header */
					handle_sta_header_add_evt();
					handle_route_add_evt(data->iptype);
					/* Add IPv6 routing table if XLAT is enabled */
					if(is_xlat && (m_is_sta_mode == Q6_WAN) && (active_v6 == false))
					{
						IPACMDBG_H("XLAT enabled: adding IPv6 routing table dev (%s)\n", dev_name);
						handle_route_add_evt(IPA_IP_v6);
					}
				}
				else if ((data->iptype == IPA_IP_v6) &&
						(!data->ipv6_addr[0]) && (!data->ipv6_addr[1]) && (!data->ipv6_addr[2]) && (!data->ipv6_addr[3]) &&
						(active_v6 == false) &&	(ip_type == IPA_IP_v6 || ip_type == IPA_IP_MAX))
				{
					if(ipv6_prefix[0] == 0 && ipv6_prefix[1] == 0)
					{
						IPACMDBG_H("IPv6 default route comes earlier than global IP, ignore.\n");
						return;
					}

					IPACMDBG_H("\n get default v6 route (dst:00.00.00.00)\n");
					IPACMDBG_H(" IPV6 dst: %08x:%08x:%08x:%08x \n",
							data->ipv6_addr[0], data->ipv6_addr[1], data->ipv6_addr[2], data->ipv6_addr[3]);
					IPACMDBG_H(" IPV6 gateway: %08x:%08x:%08x:%08x \n",
							data->ipv6_addr_gw[0], data->ipv6_addr_gw[1], data->ipv6_addr_gw[2], data->ipv6_addr_gw[3]);
					wan_v6_addr_gw[0] = data->ipv6_addr_gw[0];
					wan_v6_addr_gw[1] = data->ipv6_addr_gw[1];
					wan_v6_addr_gw[2] = data->ipv6_addr_gw[2];
					wan_v6_addr_gw[3] = data->ipv6_addr_gw[3];
					wan_v6_addr_gw_set = true;
					/* Check & construct STA header */
					handle_sta_header_add_evt();
					handle_route_add_evt(data->iptype);
				}
			}
			else /* double check if current default iface is not itself */
			{
				if ((data->iptype == IPA_IP_v4) && (!data->ipv4_addr) && (!data->ipv4_addr_mask) && (active_v4 == true))
				{
					IPACMDBG_H("Received v4 IPA_ROUTE_ADD_EVENT for other iface (%s)\n", IPACM_Iface::ipacmcfg->iface_table[ipa_interface_index].iface_name);
					IPACMDBG_H("ipv4 addr 0x%x\n", data->ipv4_addr);
					IPACMDBG_H("ipv4 addr mask 0x%x\n", data->ipv4_addr_mask);
					IPACMDBG_H("need clean default v4 route (dst:0.0.0.0) for old iface (%s)\n", dev_name);
					wan_v4_addr_gw_set = false;
					if(m_is_sta_mode == Q6_WAN)
					{
						del_wan_firewall_rule(IPA_IP_v4);
						install_wan_filtering_rule(false);
						handle_route_del_evt_ex(IPA_IP_v4);
					}
					else
					{
						del_dft_firewall_rules(IPA_IP_v4);
						handle_route_del_evt(IPA_IP_v4);
					}
				}
				else if ((data->iptype == IPA_IP_v6) && (!data->ipv6_addr[0]) && (!data->ipv6_addr[1]) && (!data->ipv6_addr[2]) && (!data->ipv6_addr[3]) && (active_v6 == true))
				{
				    IPACMDBG_H("Received v6 IPA_ROUTE_ADD_EVENT for other iface (%s)\n", IPACM_Iface::ipacmcfg->iface_table[ipa_interface_index].iface_name);
					IPACMDBG_H("need clean default v6 route for old iface (%s)\n", dev_name);
					if(m_is_sta_mode == Q6_WAN)
					{
						del_wan_firewall_rule(IPA_IP_v6);
						install_wan_filtering_rule(false);
						handle_route_del_evt_ex(IPA_IP_v6);
					}
					else
					{
						del_dft_firewall_rules(IPA_IP_v6);
						handle_route_del_evt(IPA_IP_v6);
					}
				}
			}
		}
		break;

#ifdef FEATURE_VLAN_MPDN
	case IPA_ROUTE_ADD_VLAN_PDN_EVENT:
		{
			ipacm_event_route_vlan *data = (ipacm_event_route_vlan *)param;

			if(data->iptype == IPA_IP_v4)
			{
				if(data->wan_ipv4_addr == wan_v4_addr)
				{
					IPACMDBG_H("received v4 IPA_ROUTE_ADD_VLAN_PDN_EVENT for VID %d, wan %s, %d\n", data->VlanID, dev_name, ipa_if_num);
					if(ipv4_to_iface[modem_ipv4_pdn_index].wan_up_vlan)
					{
						IPACMERR("v4 vlan wan is already up for %s, ignoring\n", dev_name);
						return;
					}
					if((modem_ipv6_pdn_index >= 0) && ipv6_to_iface[modem_ipv6_pdn_index].wan_up_vlan_v6 && (data->VlanID != associated_VID))
					{
						IPACMERR("inconsistency on new v4 VID (%d) and exisiting v6 VID (%d) ignoring\n", data->VlanID, associated_VID);
						return;
					}
					handle_route_add_vlan_pdn_evt(IPA_IP_v4, data->VlanID);
				}
			}
			else
			{
				if((data->wan_ipv6_prefix[0] == ipv6_prefix[0]) &&
					(data->wan_ipv6_prefix[1] == ipv6_prefix[1]))
				{
					IPACMDBG_H("received v6 IPA_ROUTE_ADD_VLAN_PDN_EVENT for VID %d, wan %s, %d\n", data->VlanID, dev_name, ipa_if_num);
					if(ipv6_to_iface[modem_ipv6_pdn_index].wan_up_vlan_v6)
					{
						IPACMERR("v6 vlan wan is already up for %s, ignoring\n", dev_name);
						return;
					}
					if((modem_ipv4_pdn_index >= 0) && ipv4_to_iface[modem_ipv4_pdn_index].wan_up_vlan && (data->VlanID != associated_VID))
					{
						IPACMERR("inconsistency on new v6 VID (%d) and exisiting v4 VID (%d) ignoring\n", data->VlanID, associated_VID);
						return;
					}
					handle_route_add_vlan_pdn_evt(IPA_IP_v6, data->VlanID);
				}
			}
		}
		break;
#endif

	case IPA_ROUTE_DEL_EVENT:
		{
			ipacm_event_data_addr *data = (ipacm_event_data_addr *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);
			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG_H("Received IPA_ROUTE_DEL_EVENT\n");
				if ((data->iptype == IPA_IP_v4) && (!data->ipv4_addr) && (!data->ipv4_addr_mask) && (active_v4 == true))
				{
					IPACMDBG_H("get del default v4 route (dst:0.0.0.0)\n");
					wan_v4_addr_gw_set = false;
					if(m_is_sta_mode == Q6_WAN)
					{
						del_wan_firewall_rule(IPA_IP_v4);
						install_wan_filtering_rule(false);
						handle_route_del_evt_ex(IPA_IP_v4);

						if(is_xlat && active_v6 == true)
						{
							IPACMDBG_H("XLAT enabled: Delete IPv6 routing table dev (%s)\n", dev_name);
							del_wan_firewall_rule(IPA_IP_v6);
							install_wan_filtering_rule(false);
							handle_route_del_evt_ex(IPA_IP_v6);
						}
					}
					else
					{
						del_dft_firewall_rules(IPA_IP_v4);
						handle_route_del_evt(IPA_IP_v4);
					}
				}
				else if ((data->iptype == IPA_IP_v6) && (!data->ipv6_addr[0]) && (!data->ipv6_addr[1]) && (!data->ipv6_addr[2]) && (!data->ipv6_addr[3]) && (active_v6 == true))
				{

					IPACMDBG_H("get del default v6 route (dst:00.00.00.00)\n");
					if(m_is_sta_mode == Q6_WAN)
					{
						del_wan_firewall_rule(IPA_IP_v6);
						install_wan_filtering_rule(false);
						handle_route_del_evt_ex(IPA_IP_v6);
					}
					else
					{
						del_dft_firewall_rules(IPA_IP_v6);
						handle_route_del_evt(IPA_IP_v6);
					}
				}
			}
		}
		break;

	case IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT:
		{
			ipacm_event_data_all *data = (ipacm_event_data_all *)param;
			bool gw_addr = false;
			ipa_interface_index = iface_ipa_index_query(data->if_index);

			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG_H("Received IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT in STA mode\n");

				if (m_is_sta_mode == WLAN_WAN)
				{
					if (data->iptype == IPA_IP_v4 && data->ipv4_addr == wan_v4_addr)
					{
						IPACMDBG_H("Ignore IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT in STA mode\n");
						IPACMDBG_H("for its own ipv4 address\n");
						return;
					}
					else if (data->iptype == IPA_IP_v6)
					{
						for (int num_ipv6_addr = 0; num_ipv6_addr < num_dft_rt_v6; num_ipv6_addr++)
						{
							if ((ipv6_addr[num_ipv6_addr][0] == data->ipv6_addr[0]) &&
								(ipv6_addr[num_ipv6_addr][1] == data->ipv6_addr[1]) &&
								(ipv6_addr[num_ipv6_addr][2] == data->ipv6_addr[2]) &&
								(ipv6_addr[num_ipv6_addr][3] == data->ipv6_addr[3]))
							{
								IPACMDBG_H("Ignore IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT in STA mode\n");
								IPACMDBG_H("for its own ipv6 address\n");
								return;
							}
						}
					}
				}

				IPACMDBG_H("wan-iface got client \n");
				/* first construc WAN-client full header */
				if(memcmp(data->mac_addr,
						invalid_mac,
						sizeof(data->mac_addr)) == 0)
				{
					IPACMDBG_H("Received invalid Client MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					 data->mac_addr[0], data->mac_addr[1], data->mac_addr[2],
					 data->mac_addr[3], data->mac_addr[4], data->mac_addr[5]);
					return;
				}

				if ((data->iptype == IPA_IP_v4) && wan_v4_addr_gw_set && (data->ipv4_addr == wan_v4_addr_gw))
					gw_addr = true;

				if ((data->iptype == IPA_IP_v6) && wan_v6_addr_gw_set)
				{
					if(data->ipv6_addr[0] == wan_v6_addr_gw[0] &&
					   data->ipv6_addr[1] == wan_v6_addr_gw[1] &&
					   data->ipv6_addr[2] == wan_v6_addr_gw[2] &&
					   data->ipv6_addr[3] == wan_v6_addr_gw[3])
					   	gw_addr = true;
				}

				handle_wan_hdr_init(data->mac_addr, gw_addr);
				IPACMDBG_H("construct wan-client header and route rules \n");
				/* Associate with IP and construct RT-rule */
				if (handle_wan_client_ipaddr(data) == IPACM_FAILURE)
				{
					return;
				}
				handle_wan_client_route_rule(data->mac_addr, data->iptype);
				/* Check & construct STA header */
				handle_sta_header_add_evt();
				return;
			}
		}
		break;

	case IPA_SW_ROUTING_ENABLE:
		IPACMDBG_H("Received IPA_SW_ROUTING_ENABLE\n");
		/* handle software routing enable event */
		if(m_is_sta_mode == Q6_WAN)
		{
			install_wan_filtering_rule(true);
		}
		else
		{
			handle_software_routing_enable();
		}
		break;

	case IPA_SW_ROUTING_DISABLE:
		IPACMDBG_H("Received IPA_SW_ROUTING_DISABLE\n");
		/* handle software routing disable event */
		if(m_is_sta_mode == Q6_WAN)
		{
			/* send current DL rules to modem */
			install_wan_filtering_rule(false);
			softwarerouting_act = false;
		}
		else
		{
			handle_software_routing_disable();
		}
		break;

	case IPA_FIREWALL_CHANGE_EVENT:
		IPACMDBG_H("Received IPA_FIREWALL_CHANGE_EVENT\n");

		if(m_is_sta_mode == Q6_WAN)
		{
#ifdef FEATURE_VLAN_MPDN
			if (ipacmcfg->vlan_firewall_change_handle)
			{
				ipacmcfg->vlan_firewall_change_handle = false;
			}
			else
			{
				break;
			}

			del_wan_firewall_rule(IPA_IP_v4);
			config_wan_firewall_rule(IPA_IP_v4);

			del_wan_firewall_rule(IPA_IP_v6);
			config_wan_firewall_rule(IPA_IP_v6);
			install_wan_filtering_rule(false);
#else
			if (is_default_gateway == false)
			{
				IPACMDBG_H("Interface %s is not default gw, return.\n", dev_name);
				return;
			}

			if(ip_type == IPA_IP_v4)
			{
				del_wan_firewall_rule(IPA_IP_v4);
				config_wan_firewall_rule(IPA_IP_v4);
				install_wan_filtering_rule(false);
			}
			else if(ip_type == IPA_IP_v6)
			{
				del_wan_firewall_rule(IPA_IP_v6);
				config_wan_firewall_rule(IPA_IP_v6);
				install_wan_filtering_rule(false);
			}
			else if(ip_type == IPA_IP_MAX)
			{
				del_wan_firewall_rule(IPA_IP_v4);
				config_wan_firewall_rule(IPA_IP_v4);

				del_wan_firewall_rule(IPA_IP_v6);
				config_wan_firewall_rule(IPA_IP_v6);
				install_wan_filtering_rule(false);
			}
			else
			{
				IPACMERR("IP type is not expected.\n");
			}
#endif
		}
		else
		{
			if (active_v4)
			{
				del_dft_firewall_rules(IPA_IP_v4);
				config_dft_firewall_rules(IPA_IP_v4);
			}
			if (active_v6)
			{

				del_dft_firewall_rules(IPA_IP_v6);
				config_dft_firewall_rules(IPA_IP_v6);
			}
		}
		break;

		case IPA_WLAN_SWITCH_TO_SCC:
			if(IPACM_Wan::backhaul_is_sta_mode == true)
			{
				IPACMDBG_H("Received IPA_WLAN_SWITCH_TO_SCC\n");
				if(ip_type == IPA_IP_MAX)
				{
					handle_wlan_SCC_MCC_switch(true, IPA_IP_v4);
					handle_wlan_SCC_MCC_switch(true, IPA_IP_v6);
					handle_wan_client_SCC_MCC_switch(true, IPA_IP_v4);
					handle_wan_client_SCC_MCC_switch(true, IPA_IP_v6);
				}
				else
				{
					handle_wlan_SCC_MCC_switch(true, ip_type);
					handle_wan_client_SCC_MCC_switch(true, ip_type);
				}
			}
			break;

		case IPA_WLAN_SWITCH_TO_MCC:
			/* check if alt_dst_pipe set or not */
			for (cnt = 0; cnt < tx_prop->num_tx_props; cnt++)
			{
				if (tx_prop->tx[cnt].alt_dst_pipe == 0)
				{
					IPACMERR("Tx(%d): wrong tx property: alt_dst_pipe: 0. \n", cnt);
					return;
				}
			}

			if(IPACM_Wan::backhaul_is_sta_mode == true)
			{
				IPACMDBG_H("Received IPA_WLAN_SWITCH_TO_MCC\n");
				if(ip_type == IPA_IP_MAX)
				{
					handle_wlan_SCC_MCC_switch(false, IPA_IP_v4);
					handle_wlan_SCC_MCC_switch(false, IPA_IP_v6);
					handle_wan_client_SCC_MCC_switch(false, IPA_IP_v4);
					handle_wan_client_SCC_MCC_switch(false, IPA_IP_v6);
				}
				else
				{
					handle_wlan_SCC_MCC_switch(false, ip_type);
					handle_wan_client_SCC_MCC_switch(false, ip_type);
				}
			}
			break;
#ifdef FEATURE_L2TP_E2E
	case IPA_ADD_L2TP_CLIENT:
		if(active_v4)
		{
			ipacm_event_data_all *data = (ipacm_event_data_all *)param;
			handle_l2tp_client_add(data->iface_name);
			install_wan_filtering_rule(false);
		}
		break;

	case IPA_DEL_L2TP_CLIENT:
		if(active_v4)
		{
			ipacm_event_data_all *data = (ipacm_event_data_all *)param;
			handle_l2tp_client_del(data->iface_name);
			install_wan_filtering_rule(false);
		}
		break;
#endif
	default:
		break;
	}

	return;
}

#ifdef FEATURE_VLAN_MPDN
int IPACM_Wan::handle_route_add_vlan_pdn_evt(ipa_ip_type iptype, uint8_t vlan_id)
{
	struct ipa_ioc_add_rt_rule *rt_rule = NULL;
	struct ipa_rt_rule_add *rt_rule_entry;
	struct ipa_ioc_get_hdr hdr;
	const int NUM = 1;
	ipacm_cmd_q_data evt_data;
	bool FullConfig = false;

	/* copy header from tx-property, see if partial or not */
	/* assume all tx-property uses the same header name for v4 or v6*/
	if(tx_prop == NULL)
	{
		IPACMDBG_H("No tx properties, ignore new vlan PDN event\n");
		return IPACM_FAILURE;
	}

	if(m_is_sta_mode != Q6_WAN)
	{
		IPACMERR("sta mode is not Q6_WAN, not supported for VLAN PDNs");
		return IPACM_FAILURE;
	}

	if(iptype == IPA_IP_v6)
	{
		if(wan_up_v6 || isVlanWanUP_V6())
		{
			IPACMDBG_H("a v6 PDN is already up, don't create default rt rule\n");
		}
		else
		{
			FullConfig = true;

			rt_rule = (struct ipa_ioc_add_rt_rule *)
				calloc(1, sizeof(struct ipa_ioc_add_rt_rule) +
					NUM * sizeof(struct ipa_rt_rule_add));

			if(!rt_rule)
			{
				IPACMERR("Error Locate ipa_ioc_add_rt_rule memory...\n");
				return IPACM_FAILURE;
			}

			rt_rule->commit = 1;
			rt_rule->num_rules = (uint8_t)NUM;
			rt_rule->ip = iptype;


			IPACMDBG_H(" WAN table created %s \n", rt_rule->rt_tbl_name);
			rt_rule_entry = &rt_rule->rules[0];
			rt_rule_entry->at_rear = true;

			/* add a catch-all rule in wan dl routing table */
			strlcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_wan_v6.name, sizeof(rt_rule->rt_tbl_name));
			memset(rt_rule_entry, 0, sizeof(struct ipa_rt_rule_add));
			rt_rule_entry->at_rear = true;

			memset(&hdr, 0, sizeof(hdr));
			strlcpy(hdr.name, tx_prop->tx[0].hdr_name, sizeof(hdr.name));
			hdr.name[IPA_RESOURCE_NAME_MAX - 1] = '\0';
			if(m_header.GetHeaderHandle(&hdr) == false)
			{
				IPACMERR("Failed to get QMAP header.\n");
				return IPACM_FAILURE;
			}
			rt_rule_entry->rule.hdr_hdl = hdr.hdl;
			rt_rule_entry->rule.dst = IPA_CLIENT_APPS_WAN_CONS;

			rt_rule_entry->rule.attrib.attrib_mask = IPA_FLT_DST_ADDR;
			rt_rule_entry->rule.attrib.u.v6.dst_addr[0] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr[1] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr[2] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr[3] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[0] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[1] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[2] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[3] = 0;
#ifdef FEATURE_IPA_V3
			rt_rule_entry->rule.hashable = true;
#endif
			if(false == m_routing.AddRoutingRule(rt_rule))
			{
				IPACMERR("Routing rule addition failed!\n");
				free(rt_rule);
				return IPACM_FAILURE;
			}
			wan_route_rule_v6_hdl_a5 = rt_rule_entry->rt_rule_hdl;
			IPACMDBG_H("Set ipv6 wan-route rule hdl for v6_wan_table:0x%x,tx:%d,ip-type: %d \n",
				wan_route_rule_v6_hdl_a5, 0, iptype);

			free(rt_rule);
		}

		ipv6_to_iface[modem_ipv6_pdn_index].wan_up_vlan_v6 = true;
		/*config_wan_firewall_rule traverses all active wan IF and configures them*/
		config_wan_firewall_rule(IPA_IP_v6);
		install_wan_filtering_rule(false);

		IPACMDBG_H("new VLAN PDN prefix is 0x%08x%08x.\n", ipv6_prefix[0], ipv6_prefix[1]);

		ipacm_event_vlan_pdn *wanup_vlan_data;
		wanup_vlan_data = (ipacm_event_vlan_pdn *)malloc(sizeof(ipacm_event_vlan_pdn));
		if(wanup_vlan_data == NULL)
		{
			IPACMERR("Unable to allocate memory\n");
			return IPACM_FAILURE;
		}
		memset(wanup_vlan_data, 0, sizeof(ipacm_event_vlan_pdn));

		wanup_vlan_data->iptype = IPA_IP_v6;
		wanup_vlan_data->VlanID = vlan_id;
		wanup_vlan_data->mux_id = ext_prop->ext[0].mux_id;

		IPACMDBG_H("Posting IPA_HANDLE_WAN_VLAN_PDN_UP (v6) with below information:\n");
		IPACMDBG_H("iptype IPA_IP_v6, VlanID %d, mux_id %d, if num %d\n", vlan_id, ext_prop->ext[0].mux_id, ipa_if_num);

		evt_data.event = IPA_HANDLE_WAN_VLAN_PDN_UP;
		evt_data.evt_data = (void *)wanup_vlan_data;
		IPACM_EvtDispatcher::PostEvt(&evt_data);
	}
	else
	{
		if(wan_up || isVlanWanUP())
		{
			IPACMDBG_H("a v4 PDN is already up, minimal configuration is needed\n");
			FullConfig = true;
		}

		ipv4_to_iface[modem_ipv4_pdn_index].wan_up_vlan = true;
		config_wan_firewall_rule(IPA_IP_v4);
		install_wan_filtering_rule(false);

		ipacm_event_vlan_pdn *wanup_vlan_data;
		wanup_vlan_data = (ipacm_event_vlan_pdn *)malloc(sizeof(ipacm_event_vlan_pdn));
		if(wanup_vlan_data == NULL)
		{
			IPACMERR("Unable to allocate memory\n");
			return IPACM_FAILURE;
		}
		memset(wanup_vlan_data, 0, sizeof(ipacm_event_vlan_pdn));

		wanup_vlan_data->mux_id = ext_prop->ext[0].mux_id;
		wanup_vlan_data->iptype = IPA_IP_v4;
		wanup_vlan_data->VlanID = vlan_id;
		wanup_vlan_data->ipv4_addr = wan_v4_addr;

		IPACMDBG_H("Posting IPA_HANDLE_WAN_VLAN_PDN_UP with below information:\n");
		IPACMDBG_H("iptype IPA_IP_v4, VlanID %d, mux_id %d, if num %d\n", vlan_id, ext_prop->ext[0].mux_id, ipa_if_num);

		evt_data.event = IPA_HANDLE_WAN_VLAN_PDN_UP;
		evt_data.evt_data = (void *)wanup_vlan_data;
		IPACM_EvtDispatcher::PostEvt(&evt_data);
	}

	associated_VID = vlan_id;

	if(FullConfig)
	{
		/* Add corresponding ipa_rm_resource_name of TX-endpoint up before IPV6 RT-rule set */
		if(IPACM_Iface::ipacmcfg->GetIPAVer() >= IPA_HW_None && IPACM_Iface::ipacmcfg->GetIPAVer() < IPA_HW_v4_0)
		{
			IPACMDBG_H("dev %s add producer dependency\n", dev_name);
			IPACMDBG_H("depend Got pipe %d rm index : %d \n", tx_prop->tx[0].dst_pipe, IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
			IPACM_Iface::ipacmcfg->AddRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe], false);
		}
		else
		{
			if(m_is_sta_mode == Q6_WAN && ipa_pm_q6_check == 0)
			{
				struct wan_ioctl_notify_wan_state wan_state;

				memset(&wan_state, 0, sizeof(wan_state));

				int fd_wwan_ioctl = open(WWAN_QMI_IOCTL_DEVICE_NAME, O_RDWR);
				if(fd_wwan_ioctl < 0)
				{
					IPACMERR("Failed to open %s.\n", WWAN_QMI_IOCTL_DEVICE_NAME);
					return false;
				}
				IPACMDBG_H("send WAN_IOC_NOTIFY_WAN_STATE up to IPA_PM\n");
				wan_state.up = true;
				if(ioctl(fd_wwan_ioctl, WAN_IOC_NOTIFY_WAN_STATE, &wan_state))
				{
					IPACMERR("Failed to send WAN_IOC_NOTIFY_WAN_STATE as up %d\n ", wan_state.up);
				}
				close(fd_wwan_ioctl);
			}
			ipa_pm_q6_check++;
			IPACMDBG_H("update ipa_pm_q6_check to %d\n", ipa_pm_q6_check);
		}
	}
	else
	{
		IPACMDBG_H("don't AddRmDepend, a PDN is already up\n");
	}

	return IPACM_SUCCESS;
}

IPACM_firewall_conf_t* IPACM_Wan::get_curr_pdn_firewall_config(IPACM_firewall_t &firewall_configs, const char* curr_dev_name)
{
	if (!firewall_configs.pdn_count)
	{
		return NULL;
	}

	if (!firewall_configs.default_profile)
	{
		if (firewall_configs.pdn_count != 1)
		{
			IPACMERR("The XML %s is not valid. Please add %s tag\n", MOBILE_FIREWALL_FILE, DefaultProfile_TAG);
			return NULL;
		}
		return &firewall_configs.pdns[0];
	}

	size_t len = strlen(curr_dev_name);

	IPACMDBG_H("looking for dev %s\n", curr_dev_name);
	for (uint8_t i = 0; i < firewall_configs.pdn_count; ++i)
	{
		if (strncmp(firewall_configs.pdns[i].net_dev, curr_dev_name, len) == 0)
		{
			IPACMDBG_H("found dev %s, entry %d\n", curr_dev_name, i);
			return &firewall_configs.pdns[i];
		}
		else
		{
			IPACMDBG("dev %s doesn't match\n", firewall_configs.pdns[i].net_dev);
		}
	}

	if(firewall_configs.pdn_count == 1)
	{
		if(!strncmp(firewall_configs.pdns[0].net_dev, "UNKNOWN", strlen("UNKNOWN")))
		{
			IPACMDBG_H("only one profile in file and no name, return it");
			return &firewall_configs.pdns[0];
		}
		IPACMERR("one pdn with a differnet name (%s) != (%s), return it\n",
			firewall_configs.pdns[0].net_dev,
			curr_dev_name);
		return &firewall_configs.pdns[0];
	}

	return NULL;
}
#endif

/* wan default route/filter rule configuration */
int IPACM_Wan::handle_route_add_evt(ipa_ip_type iptype)
{

	/* add default WAN route */
	struct ipa_ioc_add_rt_rule *rt_rule = NULL;
	struct ipa_rt_rule_add *rt_rule_entry;
	struct ipa_ioc_get_hdr sRetHeader;
	uint32_t cnt, tx_index = 0;
	const int NUM = 1;
	ipacm_cmd_q_data evt_data;
	struct ipa_ioc_copy_hdr sCopyHeader; /* checking if partial header*/
	struct ipa_ioc_get_hdr hdr;
#ifdef FEATURE_VLAN_MPDN
	bool FullConfig = true;
#endif
	struct wan_ioctl_notify_wan_state wan_state;
	int fd_wwan_ioctl;
	memset(&wan_state, 0, sizeof(wan_state));
	IPACMDBG_H("ip-type:%d\n", iptype);

	/* copy header from tx-property, see if partial or not */
	/* assume all tx-property uses the same header name for v4 or v6*/

	if(tx_prop == NULL)
	{
		IPACMDBG_H("No tx properties, ignore default route setting\n");
		return IPACM_SUCCESS;
	}

	is_default_gateway = true;
	IPACMDBG_H("Default route is added to iface %s.\n", dev_name);

	if(IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_mode == BRIDGE)
	{
		IPACM_Wan::backhaul_is_wan_bridge = true;
	}
	else
	{
		IPACM_Wan::backhaul_is_wan_bridge = false;
	}
	IPACMDBG_H("backhaul_is_wan_bridge ?: %d \n", IPACM_Wan::backhaul_is_wan_bridge);

	if (m_is_sta_mode !=Q6_WAN)
	{
		IPACM_Wan::backhaul_is_sta_mode	= true;
		if((iptype==IPA_IP_v4) && (header_set_v4 != true))
		{
			header_partial_default_wan_v4 = true;
			IPACMDBG_H("STA ipv4-header haven't constructed \n");
			return IPACM_SUCCESS;
		}
		else if((iptype==IPA_IP_v6) && (header_set_v6 != true))
		{
			header_partial_default_wan_v6 = true;
			IPACMDBG_H("STA ipv6-header haven't constructed \n");
			return IPACM_SUCCESS;
		}
	}
	else
	{
		IPACM_Wan::backhaul_is_sta_mode	= false;
		IPACMDBG_H("reset backhaul to LTE \n");

		if (iface_query != NULL && iface_query->num_ext_props > 0)
		{
			if(ext_prop == NULL)
			{
				IPACMERR("Extended property is empty.\n");
				return IPACM_FAILURE;
			}
			else
			{
				IPACM_Iface::ipacmcfg->SetQmapId(ext_prop->ext[0].mux_id);
				IPACMDBG_H("Setting up QMAP ID %d.\n", ext_prop->ext[0].mux_id);
			}
		}
		else
		{
			IPACMERR("iface_query is empty.\n");
			return IPACM_FAILURE;
		}
	}
#if 0
    for (cnt=0; cnt<tx_prop->num_tx_props; cnt++)
	{
		if(tx_prop->tx[cnt].ip==iptype)
		break;
	}

	if(tx_prop->tx[cnt].hdr_name != NULL)
	{
	    memset(&sCopyHeader, 0, sizeof(sCopyHeader));
	    memcpy(sCopyHeader.name,
	    			 tx_prop->tx[cnt].hdr_name,
	    			 sizeof(sCopyHeader.name));

	    IPACMDBG_H("header name: %s\n", sCopyHeader.name);
	    if (m_header.CopyHeader(&sCopyHeader) == false)
	    {
	    	IPACMERR("ioctl copy header failed");
	    	return IPACM_FAILURE;
	    }
	    IPACMDBG_H("header length: %d, paritial: %d\n", sCopyHeader.hdr_len, sCopyHeader.is_partial);
	    if(sCopyHeader.is_partial)
	    {
		IPACMDBG_H("Not setup default WAN routing rules cuz the header is not complete\n");
            if(iptype==IPA_IP_v4)
			{
				header_partial_default_wan_v4 = true;
            }
			else
			{
				header_partial_default_wan_v6 = true;
			}
			return IPACM_SUCCESS;
	    }
	    else
	    {
            if(iptype==IPA_IP_v4)
			{
				header_partial_default_wan_v4 = false;
            }
			else
			{
				header_partial_default_wan_v6 = false;
			}
	    }
    }
#endif

	rt_rule = (struct ipa_ioc_add_rt_rule *)
		 calloc(1, sizeof(struct ipa_ioc_add_rt_rule) +
						NUM * sizeof(struct ipa_rt_rule_add));

	if (!rt_rule)
	{
		IPACMERR("Error Locate ipa_ioc_add_rt_rule memory...\n");
		return IPACM_FAILURE;
	}

	rt_rule->commit = 1;
	rt_rule->num_rules = (uint8_t)NUM;
	rt_rule->ip = iptype;


	IPACMDBG_H(" WAN table created %s \n", rt_rule->rt_tbl_name);
	rt_rule_entry = &rt_rule->rules[0];
	rt_rule_entry->at_rear = true;

	if(m_is_sta_mode != Q6_WAN)
	{
		IPACMDBG_H(" WAN instance is in STA mode \n");
		for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
		{
			if(iptype != tx_prop->tx[tx_index].ip)
			{
				IPACMDBG_H("Tx:%d, ip-type: %d conflict ip-type: %d no RT-rule added\n",
									tx_index, tx_prop->tx[tx_index].ip,iptype);
				continue;
			}

			/* use the STA-header handler */
			if (iptype == IPA_IP_v4)
			{
				strlcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_wan_v4.name, sizeof(rt_rule->rt_tbl_name));
				rt_rule_entry->rule.hdr_hdl = hdr_hdl_sta_v4;
			}
			else
			{
				strlcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_v6.name, sizeof(rt_rule->rt_tbl_name));
				rt_rule_entry->rule.hdr_hdl = hdr_hdl_sta_v6;
			}

			if(IPACM_Iface::ipacmcfg->isMCC_Mode == true)
			{
				IPACMDBG_H("In MCC mode, use alt dst pipe: %d\n",
						tx_prop->tx[tx_index].alt_dst_pipe);
				rt_rule_entry->rule.dst = tx_prop->tx[tx_index].alt_dst_pipe;
			}
			else
			{
				rt_rule_entry->rule.dst = tx_prop->tx[tx_index].dst_pipe;
			}
			memcpy(&rt_rule_entry->rule.attrib,
						 &tx_prop->tx[tx_index].attrib,
						 sizeof(rt_rule_entry->rule.attrib));

			rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
			if (iptype == IPA_IP_v4)
			{
				rt_rule_entry->rule.attrib.u.v4.dst_addr      = 0;
				rt_rule_entry->rule.attrib.u.v4.dst_addr_mask = 0;
#ifdef FEATURE_IPA_V3

				rt_rule_entry->rule.hashable = true;
#endif
				if (false == m_routing.AddRoutingRule(rt_rule))
				{
		    		IPACMERR("Routing rule addition failed!\n");
		    		free(rt_rule);
		    		return IPACM_FAILURE;
				}
				wan_route_rule_v4_hdl[tx_index] = rt_rule_entry->rt_rule_hdl;
				IPACMDBG_H("Got ipv4 wan-route rule hdl:0x%x,tx:%d,ip-type: %d \n",
							 wan_route_rule_v4_hdl[tx_index],
							 tx_index,
							 iptype);
			}
			else
			{
				rt_rule_entry->rule.attrib.u.v6.dst_addr[0] = 0;
				rt_rule_entry->rule.attrib.u.v6.dst_addr[1] = 0;
				rt_rule_entry->rule.attrib.u.v6.dst_addr[2] = 0;
				rt_rule_entry->rule.attrib.u.v6.dst_addr[3] = 0;
				rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[0] = 0;
				rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[1] = 0;
				rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[2] = 0;
				rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[3] = 0;
#ifdef FEATURE_IPA_V3
				rt_rule_entry->rule.hashable = true;
#endif
				if (false == m_routing.AddRoutingRule(rt_rule))
				{
		    		IPACMERR("Routing rule addition failed!\n");
		    		free(rt_rule);
		    		return IPACM_FAILURE;
				}
				wan_route_rule_v6_hdl[tx_index] = rt_rule_entry->rt_rule_hdl;
				IPACMDBG_H("Set ipv6 wan-route rule hdl for v6_lan_table:0x%x,tx:%d,ip-type: %d \n",
							 wan_route_rule_v6_hdl[tx_index],
							 tx_index,
							 iptype);
			}
		}
	}

	/* add a catch-all rule in wan dl routing table */

	if (iptype == IPA_IP_v6)
	{
		strlcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_wan_v6.name, sizeof(rt_rule->rt_tbl_name));
		memset(rt_rule_entry, 0, sizeof(struct ipa_rt_rule_add));
		rt_rule_entry->at_rear = true;
		if(m_is_sta_mode == Q6_WAN)
		{
			memset(&hdr, 0, sizeof(hdr));
			strlcpy(hdr.name, tx_prop->tx[0].hdr_name, sizeof(hdr.name));
			hdr.name[IPA_RESOURCE_NAME_MAX-1] = '\0';
			if(m_header.GetHeaderHandle(&hdr) == false)
			{
				IPACMERR("Failed to get QMAP header.\n");
				return IPACM_FAILURE;
			}
			rt_rule_entry->rule.hdr_hdl = hdr.hdl;
			rt_rule_entry->rule.dst = IPA_CLIENT_APPS_WAN_CONS;
		}
		else
		{
			/* create dummy ethernet header for v6 RX path */
			IPACMDBG_H("Construct dummy ethernet_header\n");
			if (add_dummy_rx_hdr())
			{
				IPACMERR("Construct dummy ethernet_header failed!\n");
				free(rt_rule);
				return IPACM_FAILURE;
			}
			rt_rule_entry->rule.hdr_proc_ctx_hdl = hdr_proc_hdl_dummy_v6;
			rt_rule_entry->rule.dst = IPA_CLIENT_APPS_LAN_CONS;
		}
		rt_rule_entry->rule.attrib.attrib_mask = IPA_FLT_DST_ADDR;
		rt_rule_entry->rule.attrib.u.v6.dst_addr[0] = 0;
		rt_rule_entry->rule.attrib.u.v6.dst_addr[1] = 0;
		rt_rule_entry->rule.attrib.u.v6.dst_addr[2] = 0;
		rt_rule_entry->rule.attrib.u.v6.dst_addr[3] = 0;
		rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[0] = 0;
		rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[1] = 0;
		rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[2] = 0;
		rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[3] = 0;
#ifdef FEATURE_IPA_V3
		rt_rule_entry->rule.hashable = true;
#endif
#ifdef FEATURE_VLAN_MPDN
		if (((m_is_sta_mode == Q6_WAN) && (!isVlanWanUP_V6()))
			|| (m_is_sta_mode != Q6_WAN))
#endif
		{
			if(false == m_routing.AddRoutingRule(rt_rule))
			{
				IPACMERR("Routing rule addition failed!\n");
				free(rt_rule);
				return IPACM_FAILURE;
			}
			wan_route_rule_v6_hdl_a5 = rt_rule_entry->rt_rule_hdl;
			IPACMDBG_H("Set ipv6 wan-route rule hdl for v6_wan_table:0x%x,tx:%d,ip-type: %d \n",
				wan_route_rule_v6_hdl_a5, 0, iptype);
		}
#ifdef FEATURE_VLAN_MPDN
		else
		{
			IPACMDBG_H("a v6 PDN is already up, don't create default rt rule\n");
			FullConfig = false;
		}
#endif
	}

	ipacm_event_iface_up *wanup_data;
	wanup_data = (ipacm_event_iface_up *)malloc(sizeof(ipacm_event_iface_up));
	if (wanup_data == NULL)
	{
		IPACMERR("Unable to allocate memory\n");
		free(rt_rule);
		return IPACM_FAILURE;
	}
	memset(wanup_data, 0, sizeof(ipacm_event_iface_up));

	if (iptype == IPA_IP_v4)
	{
		IPACM_Wan::wan_up = true;
		active_v4 = true;
		memcpy(IPACM_Wan::wan_up_dev_name,
			dev_name,
				sizeof(IPACM_Wan::wan_up_dev_name));

		if(m_is_sta_mode == Q6_WAN)
		{
			config_wan_firewall_rule(IPA_IP_v4);
			install_wan_filtering_rule(false);
#ifdef FEATURE_VLAN_MPDN
			if(isVlanWanUP())
				FullConfig = false;
#endif
		}
		else
		{
			config_dft_firewall_rules(IPA_IP_v4);
		}

		memcpy(wanup_data->ifname, dev_name, sizeof(wanup_data->ifname));
		wanup_data->ipv4_addr = wan_v4_addr;
		if (m_is_sta_mode!=Q6_WAN)
		{
			wanup_data->is_sta = true;
		}
		else
		{
			wanup_data->is_sta = false;
		}
		IPACMDBG_H("Posting IPA_HANDLE_WAN_UP with below information:\n");
		IPACMDBG_H("if_name:%s, ipv4_address:0x%x, is sta mode:%d\n",
				wanup_data->ifname, wanup_data->ipv4_addr, wanup_data->is_sta);
		memset(&evt_data, 0, sizeof(evt_data));

		/* send xlat configuration for installing uplink rules */
		if(is_xlat && (m_is_sta_mode == Q6_WAN))
		{
			IPACM_Wan::xlat_mux_id = ext_prop->ext[0].mux_id;
			wanup_data->xlat_mux_id = IPACM_Wan::xlat_mux_id;
			IPACMDBG_H("Set xlat configuraiton with below information:\n");
			IPACMDBG_H("xlat_enabled:  xlat_mux_id: %d \n",
					is_xlat, xlat_mux_id);
		}
		else
		{
			IPACM_Wan::xlat_mux_id = 0;
			wanup_data->xlat_mux_id = 0;
			if(m_is_sta_mode == Q6_WAN)
				wanup_data->mux_id = ext_prop->ext[0].mux_id;
			else
				wanup_data->mux_id = 0;
			IPACMDBG_H("No xlat configuration\n");
		}
		evt_data.event = IPA_HANDLE_WAN_UP;
		evt_data.evt_data = (void *)wanup_data;
		IPACM_EvtDispatcher::PostEvt(&evt_data);
	}
	else
	{
		memcpy(backhaul_ipv6_prefix, ipv6_prefix, sizeof(backhaul_ipv6_prefix));
		IPACMDBG_H("Setup backhaul ipv6 prefix to be 0x%08x%08x.\n", backhaul_ipv6_prefix[0], backhaul_ipv6_prefix[1]);

		IPACM_Wan::wan_up_v6 = true;
		active_v6 = true;
		memcpy(IPACM_Wan::wan_up_dev_name,
			dev_name,
				sizeof(IPACM_Wan::wan_up_dev_name));

		if(m_is_sta_mode == Q6_WAN)
		{
			config_wan_firewall_rule(IPA_IP_v6);
			install_wan_filtering_rule(false);
		}
		else
		{
			config_dft_firewall_rules(IPA_IP_v6);
		}

		memcpy(wanup_data->ifname, dev_name, sizeof(wanup_data->ifname));
		if (m_is_sta_mode!=Q6_WAN)
		{
			wanup_data->is_sta = true;
		}
		else
		{
			wanup_data->is_sta = false;
		}
		memcpy(wanup_data->ipv6_prefix, ipv6_prefix, sizeof(wanup_data->ipv6_prefix));
		memcpy(wanup_data->ipv6_addr, m_ipv6_addr, sizeof(wanup_data->ipv6_addr));
#ifdef FEATURE_VLAN_MPDN
		IPACM_Iface::ipacmcfg->add_vlan_ipv6_prefix(wanup_data->ipv6_prefix, ipa_if_num);
#endif
		IPACMDBG_H("Posting IPA_HANDLE_WAN_UP_V6 with below information:\n");
		IPACMDBG_H("if_name:%s, is sta mode: %d\n", wanup_data->ifname, wanup_data->is_sta);
		IPACMDBG_H("ipv6 prefix: 0x%08x%08x.\n", ipv6_prefix[0], ipv6_prefix[1]);
		IPACMDBG_H("ipv6 addr: 0x%08x%08x%08x%08x\n", m_ipv6_addr[0], m_ipv6_addr[1], m_ipv6_addr[2], m_ipv6_addr[3]);
		memset(&evt_data, 0, sizeof(evt_data));
		evt_data.event = IPA_HANDLE_WAN_UP_V6;
		evt_data.evt_data = (void *)wanup_data;
		IPACM_EvtDispatcher::PostEvt(&evt_data);
	}
#ifdef FEATURE_VLAN_MPDN
	if(FullConfig)
#endif
	{
		if(IPACM_Iface::ipacmcfg->GetIPAVer() >= IPA_HW_None && IPACM_Iface::ipacmcfg->GetIPAVer() < IPA_HW_v4_0)
		{
			/* Add corresponding ipa_rm_resource_name of TX-endpoint up before IPV6 RT-rule set */
			IPACMDBG_H("dev %s add producer dependency\n", dev_name);
			IPACMDBG_H("depend Got pipe %d rm index : %d \n", tx_prop->tx[0].dst_pipe, IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
			IPACM_Iface::ipacmcfg->AddRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe],false);
		} else {
			if (m_is_sta_mode == Q6_WAN && ipa_pm_q6_check == 0)
			{
				fd_wwan_ioctl = open(WWAN_QMI_IOCTL_DEVICE_NAME, O_RDWR);
				if(fd_wwan_ioctl < 0)
				{
					IPACMERR("Failed to open %s.\n",WWAN_QMI_IOCTL_DEVICE_NAME);
					return false;
				}
				IPACMDBG_H("send WAN_IOC_NOTIFY_WAN_STATE up to IPA_PM\n");
				wan_state.up = true;
				if(ioctl(fd_wwan_ioctl, WAN_IOC_NOTIFY_WAN_STATE, &wan_state))
				{
					IPACMERR("Failed to send WAN_IOC_NOTIFY_WAN_STATE as up %d\n ", wan_state.up);
				}
				close(fd_wwan_ioctl);
			}
			ipa_pm_q6_check++;
			IPACMDBG_H("update ipa_pm_q6_check to %d\n", ipa_pm_q6_check);

		}
	}
#ifdef FEATURE_VLAN_MPDN
	else
	{
		IPACMDBG_H("don't AddRmDepend, vlan PDN already up, iptype\n", iptype);
	}
#endif
	if(rt_rule != NULL)
	{
		free(rt_rule);
	}
	return IPACM_SUCCESS;
}

#ifdef FEATURE_IPA_ANDROID
/* wan default route/filter rule configuration */
int IPACM_Wan::post_wan_up_tether_evt(ipa_ip_type iptype, int ipa_if_num_tether)
{
	ipacm_cmd_q_data evt_data;
	ipacm_event_iface_up_tehter *wanup_data;

	wanup_data = (ipacm_event_iface_up_tehter *)malloc(sizeof(ipacm_event_iface_up_tehter));
	if (wanup_data == NULL)
	{
		IPACMERR("Unable to allocate memory\n");
		return IPACM_FAILURE;
	}
	memset(wanup_data, 0, sizeof(ipacm_event_iface_up_tehter));

	wanup_data->if_index_tether = ipa_if_num_tether;
	if (m_is_sta_mode!=Q6_WAN)
	{
		wanup_data->is_sta = true;
	}
	else
	{
		wanup_data->is_sta = false;
	}
	IPACMDBG_H("Posting IPA_HANDLE_WAN_UP_TETHER with below information:\n");
	IPACMDBG_H("tether_if_name:%s, is sta mode:%d\n",
			IPACM_Iface::ipacmcfg->iface_table[ipa_if_num_tether].iface_name, wanup_data->is_sta);
	memset(&evt_data, 0, sizeof(evt_data));

	if (iptype == IPA_IP_v4)
	{
		evt_data.event = IPA_HANDLE_WAN_UP_TETHER;
		/* Add support tether ifaces to its array*/
		IPACM_Wan::ipa_if_num_tether_v4[IPACM_Wan::ipa_if_num_tether_v4_total] = ipa_if_num_tether;
		IPACMDBG_H("adding tether iface(%s) ipa_if_num_tether_v4_total(%d) on wan_iface(%s)\n",
			IPACM_Iface::ipacmcfg->iface_table[ipa_if_num_tether].iface_name,
			IPACM_Wan::ipa_if_num_tether_v4_total,
			IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].iface_name);
		IPACM_Wan::ipa_if_num_tether_v4_total++;
	}
	else
	{
		evt_data.event = IPA_HANDLE_WAN_UP_V6_TETHER;
		memcpy(wanup_data->ipv6_prefix, ipv6_prefix, sizeof(wanup_data->ipv6_prefix));
		/* Add support tether ifaces to its array*/
		IPACM_Wan::ipa_if_num_tether_v6[IPACM_Wan::ipa_if_num_tether_v6_total] = ipa_if_num_tether;
		IPACMDBG_H("adding tether iface(%s) ipa_if_num_tether_v6_total(%d) on wan_iface(%s)\n",
			IPACM_Iface::ipacmcfg->iface_table[ipa_if_num_tether].iface_name,
			IPACM_Wan::ipa_if_num_tether_v6_total,
			IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].iface_name);
		IPACM_Wan::ipa_if_num_tether_v6_total++;
	}
		evt_data.evt_data = (void *)wanup_data;
		IPACM_EvtDispatcher::PostEvt(&evt_data);

	return IPACM_SUCCESS;
}

/* wan default route/filter rule configuration */
int IPACM_Wan::post_wan_down_tether_evt(ipa_ip_type iptype, int ipa_if_num_tether)
{
	ipacm_cmd_q_data evt_data;
	ipacm_event_iface_up_tehter *wandown_data;
	int i, j;

	wandown_data = (ipacm_event_iface_up_tehter *)malloc(sizeof(ipacm_event_iface_up_tehter));
	if (wandown_data == NULL)
	{
		IPACMERR("Unable to allocate memory\n");
		return IPACM_FAILURE;
	}
	memset(wandown_data, 0, sizeof(ipacm_event_iface_up_tehter));

	wandown_data->if_index_tether = ipa_if_num_tether;
	if (m_is_sta_mode!=Q6_WAN)
	{
		wandown_data->is_sta = true;
	}
	else
	{
		wandown_data->is_sta = false;
	}
	IPACMDBG_H("Posting IPA_HANDLE_WAN_DOWN_TETHER with below information:\n");
	IPACMDBG_H("tether_if_name:%s, is sta mode:%d\n",
			IPACM_Iface::ipacmcfg->iface_table[ipa_if_num_tether].iface_name, wandown_data->is_sta);
	memset(&evt_data, 0, sizeof(evt_data));

	if (iptype == IPA_IP_v4)
	{
		evt_data.event = IPA_HANDLE_WAN_DOWN_TETHER;
		/* delete support tether ifaces to its array*/
		for (i=0; i < IPACM_Wan::ipa_if_num_tether_v4_total; i++)
		{
			if(IPACM_Wan::ipa_if_num_tether_v4[i] == ipa_if_num_tether)
			{
				IPACMDBG_H("Found tether client at position %d name(%s)\n", i,
				IPACM_Iface::ipacmcfg->iface_table[ipa_if_num_tether].iface_name);
				break;
			}
		}
		if(i == IPACM_Wan::ipa_if_num_tether_v4_total)
		{
			IPACMDBG_H("Not finding the tether client.\n");
			free(wandown_data);
			return IPACM_SUCCESS;
		}
		for(j = i+1; j < IPACM_Wan::ipa_if_num_tether_v4_total; j++)
		{
			IPACM_Wan::ipa_if_num_tether_v4[j-1] = IPACM_Wan::ipa_if_num_tether_v4[j];
		}
		IPACM_Wan::ipa_if_num_tether_v4_total--;
		IPACMDBG_H("Now the total num of ipa_if_num_tether_v4_total is %d on wan-iface(%s)\n",
			IPACM_Wan::ipa_if_num_tether_v4_total,
			IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].iface_name);
	}
	else
	{
		evt_data.event = IPA_HANDLE_WAN_DOWN_V6_TETHER;
		/* delete support tether ifaces to its array*/
		for (i=0; i < IPACM_Wan::ipa_if_num_tether_v6_total; i++)
		{
			if(IPACM_Wan::ipa_if_num_tether_v6[i] == ipa_if_num_tether)
			{
				IPACMDBG_H("Found tether client at position %d name(%s)\n", i,
				IPACM_Iface::ipacmcfg->iface_table[ipa_if_num_tether].iface_name);
				break;
			}
		}
		if(i == IPACM_Wan::ipa_if_num_tether_v6_total)
		{
			IPACMDBG_H("Not finding the tether client.\n");
			free(wandown_data);
			return IPACM_SUCCESS;
		}
		for(j = i+1; j < IPACM_Wan::ipa_if_num_tether_v6_total; j++)
		{
			IPACM_Wan::ipa_if_num_tether_v6[j-1] = IPACM_Wan::ipa_if_num_tether_v6[j];
		}
		IPACM_Wan::ipa_if_num_tether_v6_total--;
		IPACMDBG_H("Now the total num of ipa_if_num_tether_v6_total is %d on wan-iface(%s)\n",
			IPACM_Wan::ipa_if_num_tether_v6_total,
			IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].iface_name);
	}
		evt_data.evt_data = (void *)wandown_data;
		IPACM_EvtDispatcher::PostEvt(&evt_data);
	return IPACM_SUCCESS;
}
#endif

/* construct complete ethernet header */
int IPACM_Wan::handle_sta_header_add_evt()
{
	int res = IPACM_SUCCESS, index = IPACM_INVALID_INDEX;
	if((header_set_v4 == true) || (header_set_v6 == true))
	{
		IPACMDBG_H("Already add STA full header\n");
		return IPACM_SUCCESS;
	}

	/* checking if the ipv4 same as default route */
	if(wan_v4_addr_gw_set)
	{
		index = get_wan_client_index_ipv4(wan_v4_addr_gw);
		if (index != IPACM_INVALID_INDEX)
		{
			IPACMDBG_H("Matched client index: %d\n", index);
			IPACMDBG_H("Received Client MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					 get_client_memptr(wan_client, index)->mac[0],
					 get_client_memptr(wan_client, index)->mac[1],
					 get_client_memptr(wan_client, index)->mac[2],
					 get_client_memptr(wan_client, index)->mac[3],
					 get_client_memptr(wan_client, index)->mac[4],
					 get_client_memptr(wan_client, index)->mac[5]);

			if(get_client_memptr(wan_client, index)->ipv4_header_set)
			{
				hdr_hdl_sta_v4 = get_client_memptr(wan_client, index)->hdr_hdl_v4;
				header_set_v4 = true;
				IPACMDBG_H("add full ipv4 header hdl: (%x)\n", get_client_memptr(wan_client, index)->hdr_hdl_v4);
				/* store external_ap's MAC */
				memcpy(ext_router_mac_addr, get_client_memptr(wan_client, index)->mac, sizeof(ext_router_mac_addr));
			}
			else
			{
				IPACMERR(" wan-client got ipv4 however didn't construct complete ipv4 header \n");
				return IPACM_FAILURE;
			}

			if(get_client_memptr(wan_client, index)->ipv6_header_set)
			{
				hdr_hdl_sta_v6 = get_client_memptr(wan_client, index)->hdr_hdl_v6;
				header_set_v6 = true;
				IPACMDBG_H("add full ipv6 header hdl: (%x)\n", get_client_memptr(wan_client, index)->hdr_hdl_v6);
			}
			else
			{
				IPACMERR(" wan-client got ipv6 however didn't construct complete ipv6 header \n");
				return IPACM_FAILURE;
			}
		}
		else
		{
			IPACMDBG_H(" currently can't find matched wan-client's MAC-addr, waiting for header construction\n");
			return IPACM_SUCCESS;
		}
	}

	/* see if default routes are setup before constructing full header */
	if(header_partial_default_wan_v4 == true)
	{
	   handle_route_add_evt(IPA_IP_v4);
	}

	/* checking if the ipv6 same as default route */
	if(wan_v6_addr_gw_set)
	{
		index = get_wan_client_index_ipv6(wan_v6_addr_gw);
		if (index != IPACM_INVALID_INDEX)
		{
			IPACMDBG_H("Matched client index: %d\n", index);
			IPACMDBG_H("Received Client MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					 get_client_memptr(wan_client, index)->mac[0],
					 get_client_memptr(wan_client, index)->mac[1],
					 get_client_memptr(wan_client, index)->mac[2],
					 get_client_memptr(wan_client, index)->mac[3],
					 get_client_memptr(wan_client, index)->mac[4],
					 get_client_memptr(wan_client, index)->mac[5]);

			if(get_client_memptr(wan_client, index)->ipv6_header_set)
			{
				hdr_hdl_sta_v6 = get_client_memptr(wan_client, index)->hdr_hdl_v6;
				header_set_v6 = true;
				IPACMDBG_H("add full ipv6 header hdl: (%x)\n", get_client_memptr(wan_client, index)->hdr_hdl_v6);
				/* store external_ap's MAC */
				memcpy(ext_router_mac_addr, get_client_memptr(wan_client, index)->mac, sizeof(ext_router_mac_addr));
			}
			else
			{
				IPACMERR(" wan-client got ipv6 however didn't construct complete ipv4 header \n");
				return IPACM_FAILURE;
			}

			if(get_client_memptr(wan_client, index)->ipv4_header_set)
			{
				hdr_hdl_sta_v4 = get_client_memptr(wan_client, index)->hdr_hdl_v4;
				header_set_v4 = true;
				IPACMDBG_H("add full ipv4 header hdl: (%x)\n", get_client_memptr(wan_client, index)->hdr_hdl_v4);
			}
			else
			{
				IPACMERR(" wan-client got ipv4 however didn't construct complete ipv4 header \n");
				return IPACM_FAILURE;
			}
		}
		else
		{
			IPACMDBG_H(" currently can't find matched wan-client's MAC-addr, waiting for header construction\n");
			return IPACM_SUCCESS;
		}
	}

	/* see if default routes are setup before constructing full header */

	if(header_partial_default_wan_v6 == true)
	{
	   handle_route_add_evt(IPA_IP_v6);
	}
	return res;
}

/* For checking attribute mask field in firewall rules for IPv6 only */
bool IPACM_Wan::check_dft_firewall_rules_attr_mask(IPACM_firewall_conf_t *firewall_config)
{
	uint32_t attrib_mask = 0ul;
	attrib_mask =	IPA_FLT_SRC_PORT_RANGE |
			IPA_FLT_DST_PORT_RANGE |
			IPA_FLT_TYPE |
			IPA_FLT_CODE |
			IPA_FLT_SPI |
			IPA_FLT_SRC_PORT |
			IPA_FLT_DST_PORT;

	for (int i = 0; i < firewall_config->num_extd_firewall_entries; i++)
	{
#ifndef FEATURE_IPACM_UL_FIREWALL
		if (firewall_config->extd_firewall_entries[i].ip_vsn == 6)
#else //n FEATURE_IPACM_UL_FIREWALL
		if (firewall_config->extd_firewall_entries[i].ip_vsn == 6 &&
			firewall_config->extd_firewall_entries[i].firewall_direction
			!= IPACM_MSGR_UL_FIREWALL)
#endif //n FEATURE_IPACM_UL_FIREWALL
		{
			if (firewall_config->extd_firewall_entries[i].attrib.attrib_mask & attrib_mask)
			{
				IPACMDBG_H("IHL based attribute mask is found: install IPv6 frag firewall rule \n");
				return true;
			}
		}
	}
	IPACMDBG_H("IHL based attribute mask is not found: no IPv6 frag firewall rule \n");
	return false;
}

#ifdef FEATURE_IPACM_UL_FIREWALL
/* For checking attribute mask field in firewall rules for IPv6 UL only */
bool IPACM_Wan::check_dft_firewall_rules_attr_mask_ul(IPACM_firewall_conf_t *firewall_config)
{
	uint32_t attrib_mask = 0ul;
	attrib_mask =	IPA_FLT_SRC_PORT_RANGE |
			IPA_FLT_DST_PORT_RANGE |
			IPA_FLT_TYPE |
			IPA_FLT_CODE |
			IPA_FLT_SPI |
			IPA_FLT_SRC_PORT |
			IPA_FLT_DST_PORT;

	for (int i = 0; i < firewall_config->num_extd_firewall_entries; i++)
	{
		if (firewall_config->extd_firewall_entries[i].ip_vsn == 6 &&
			firewall_config->extd_firewall_entries[i].firewall_direction
			== IPACM_MSGR_UL_FIREWALL)
		{
			if (firewall_config->extd_firewall_entries[i].attrib.attrib_mask & attrib_mask)
			{
				IPACMDBG_H("IHL based attribute mask is found: install IPv6 frag firewall rule \n");
				return true;
			}
		}
	}
	IPACMDBG_H("IHL based attribute mask is not found: no IPv6 frag firewall rule \n");
	return false;
}
#endif //FEATURE_IPACM_UL_FIREWALL

/* for STA mode: add firewall rules */
int IPACM_Wan::config_dft_firewall_rules(ipa_ip_type iptype)
{
	struct ipa_flt_rule_add flt_rule_entry;
	int i, rule_v4 = 0, rule_v6 = 0, len;
#ifdef FEATURE_IPACM_UL_FIREWALL
	int rule_v4_ul = 0, rule_v6_ul = 0;
#endif //FEATURE_IPACM_UL_FIREWALL
	IPACMDBG_H("ip-family: %d; \n", iptype);

	if (rx_prop == NULL)
	{
		IPACMDBG_H("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	/* default firewall is disable and the rule action is drop */
	IPACM_firewall_conf_t firewall_config;
	IPACMDBG_H("Firewall XML file is %s\n", MOBILE_FIREWALL_FILE);
	if (IPACM_read_firewall_xml(MOBILE_FIREWALL_FILE, firewall_config) == IPACM_SUCCESS)
	{
		IPACMDBG_H("QCMAP Firewall XML read OK \n");
		/* find the number of v4/v6 firewall rules */
		for (i = 0; i < firewall_config.num_extd_firewall_entries; i++)
		{
#ifdef FEATURE_IPACM_UL_FIREWALL
			if (firewall_config.extd_firewall_entries[i].ip_vsn == 4 &&
				firewall_config.extd_firewall_entries[i].firewall_direction
				== IPACM_MSGR_UL_FIREWALL)
			{
				rule_v4_ul++;
			}
			else if (firewall_config.extd_firewall_entries[i].ip_vsn == 6 &&
				firewall_config.extd_firewall_entries[i].firewall_direction
				== IPACM_MSGR_UL_FIREWALL)
			{
				rule_v6_ul++;
			}
			else if (firewall_config.extd_firewall_entries[i].ip_vsn == 6 &&
				firewall_config.extd_firewall_entries[i].firewall_direction
				!= IPACM_MSGR_UL_FIREWALL)
			{
				rule_v6++;
			}
#else //FEATURE_IPACM_UL_FIREWALL
			if (firewall_config.extd_firewall_entries[i].ip_vsn == 4)
			{
				rule_v4++;
			}
			else
			{
				rule_v6++;
			}
#endif //n FEATURE_IPACM_UL_FIREWALL
		}
#ifdef FEATURE_IPACM_UL_FIREWALL
		IPACMDBG_H("UL firewall rule cnt v4ul:%d v6ul:%d\n",
			rule_v4_ul, rule_v6_ul);
#endif //FEATURE_IPACM_UL_FIREWALL
		IPACMDBG_H("firewall rule v4:%d v6:%d total:%d\n",
			rule_v4, rule_v6, firewall_config.num_extd_firewall_entries);
	}
	else
	{
		IPACMERR("QCMAP Firewall XML read failed, no such file, use default configuration \n");
	}

	/* construct ipa_ioc_add_flt_rule with N firewall rules */
	ipa_ioc_add_flt_rule *m_pFilteringTable = NULL;
	len = sizeof(struct ipa_ioc_add_flt_rule) + 1 * sizeof(struct ipa_flt_rule_add);
	m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)calloc(1, len);
	if (!m_pFilteringTable)
	{
		IPACMERR("Error Locate ipa_flt_rule_add memory...\n");
		return IPACM_FAILURE;
	}

	if (iptype == IPA_IP_v6 && !ipacmcfg->IsIpv6CTEnabled() && firewall_config.firewall_enable &&
		check_dft_firewall_rules_attr_mask(&firewall_config))
	{
		m_pFilteringTable->commit = 1;
		m_pFilteringTable->ep = rx_prop->rx[0].src_pipe;
		m_pFilteringTable->global = false;
		m_pFilteringTable->ip = IPA_IP_v6;
		m_pFilteringTable->num_rules = (uint8_t)1;

		memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
		flt_rule_entry.at_rear = true;
#ifdef FEATURE_IPA_V3
		flt_rule_entry.at_rear = false;
		flt_rule_entry.rule.hashable = false;
#endif
		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;
		flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;
		memcpy(&flt_rule_entry.rule.attrib, &rx_prop->rx[0].attrib, sizeof(struct ipa_rule_attrib));
		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_FRAGMENT;
		memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
		if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
		{
			IPACMERR("Error Adding RuleTable(0) to Filtering, aborting...\n");
			free(m_pFilteringTable);
			return IPACM_FAILURE;
		}
		else
		{
			IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, 1);
			ipv6_frag_firewall_flt_rule_hdl = m_pFilteringTable->rules[0].flt_rule_hdl;
			is_ipv6_frag_firewall_flt_rule_installed = true;
			IPACMDBG_H("Installed IPv6 frag firewall rule, handle %d.\n", ipv6_frag_firewall_flt_rule_hdl);
		}
	}

	if (iptype == IPA_IP_v4)
	{
		if (rule_v4 == 0)
		{
			memset(m_pFilteringTable, 0, len);

			m_pFilteringTable->commit = 1;
			m_pFilteringTable->ep = rx_prop->rx[0].src_pipe;
			m_pFilteringTable->global = false;
			m_pFilteringTable->ip = IPA_IP_v4;
			m_pFilteringTable->num_rules = (uint8_t)1;

			memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

			if (false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_lan_v4))
			{
				IPACMERR("m_routing.GetRoutingTable(rt_tbl_lan_v4) Failed.\n");
				free(m_pFilteringTable);
				return IPACM_FAILURE;
			}

			flt_rule_entry.flt_rule_hdl = -1;
			flt_rule_entry.status = -1;

#ifndef FEATURE_IPACM_UL_FIREWALL

			/* firewall disable, all traffic are allowed */
			if(firewall_config.firewall_enable == true)
			{
				flt_rule_entry.at_rear = true;

				/* default action for v4 is go DST_NAT unless user set to exception*/
				if(firewall_config.rule_action_accept == true)
				{
					flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;
				}
				else
				{
					if(IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_mode == ROUTER)
					{
						flt_rule_entry.rule.action = IPA_PASS_TO_DST_NAT;
					}
					else
					{
						flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
					}
				}
			}
			else
#endif //FEATURE_IPACM_UL_FIREWALL
			{
				flt_rule_entry.at_rear = true;
				if(IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_mode == ROUTER)
				{
					flt_rule_entry.rule.action = IPA_PASS_TO_DST_NAT;
				}
				else
				{
					flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
				}
            }
#ifdef FEATURE_IPA_V3
			flt_rule_entry.at_rear = true;
			flt_rule_entry.rule.hashable = true;
#endif
			flt_rule_entry.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_lan_v4.hdl;
			memcpy(&flt_rule_entry.rule.attrib,
						 &rx_prop->rx[0].attrib,
						 sizeof(struct ipa_rule_attrib));
			flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
			flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = 0x00000000;
			flt_rule_entry.rule.attrib.u.v4.dst_addr = 0x00000000;

			memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

			if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
			{
				IPACMERR("Error Adding RuleTable(0) to Filtering, aborting...\n");
				free(m_pFilteringTable);
				return IPACM_FAILURE;
			}
			else
			{
				IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v4, 1);
				IPACMDBG_H("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
			}

			/* copy filter hdls */
			dft_wan_fl_hdl[0] = m_pFilteringTable->rules[0].flt_rule_hdl;
		}
		else
		{
			memset(m_pFilteringTable, 0, len);

			m_pFilteringTable->commit = 1;
			m_pFilteringTable->ep = rx_prop->rx[0].src_pipe;
			m_pFilteringTable->global = false;
			m_pFilteringTable->ip = IPA_IP_v4;
			m_pFilteringTable->num_rules = (uint8_t)1;

			IPACMDBG_H("Retreiving Routing handle for routing table name:%s\n",
							 IPACM_Iface::ipacmcfg->rt_tbl_lan_v4.name);
			if (false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_lan_v4))
			{
				IPACMERR("m_routing.GetRoutingTable(&rt_tbl_lan_v4=0x%p) Failed.\n", &IPACM_Iface::ipacmcfg->rt_tbl_lan_v4);
				free(m_pFilteringTable);
				return IPACM_FAILURE;
			}
			IPACMDBG_H("Routing handle for wan routing table:0x%x\n", IPACM_Iface::ipacmcfg->rt_tbl_lan_v4.hdl);

            if(firewall_config.firewall_enable == true)
            {
			rule_v4 = 0;
			for (i = 0; i < firewall_config.num_extd_firewall_entries; i++)
			{
				if (firewall_config.extd_firewall_entries[i].ip_vsn == 4)
				{
					memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

		    			flt_rule_entry.at_rear = true;
					flt_rule_entry.flt_rule_hdl = -1;
					flt_rule_entry.status = -1;
					flt_rule_entry.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_lan_v4.hdl;

					/* Accept v4 matched rules*/
                    if(firewall_config.rule_action_accept == true)
			        {
						if(IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_mode == ROUTER)
						{
							flt_rule_entry.rule.action = IPA_PASS_TO_DST_NAT;
						}
						else
						{
							flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
						}
			        }
			        else
			        {
			            flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;
                    }
#ifdef FEATURE_IPA_V3
					flt_rule_entry.rule.hashable = true;
#endif
					memcpy(&flt_rule_entry.rule.attrib,
								 &firewall_config.extd_firewall_entries[i].attrib,
								 sizeof(struct ipa_rule_attrib));

					IPACMDBG_H("rx property attrib mask: 0x%x\n", rx_prop->rx[0].attrib.attrib_mask);
					flt_rule_entry.rule.attrib.attrib_mask |= rx_prop->rx[0].attrib.attrib_mask;
					flt_rule_entry.rule.attrib.meta_data_mask = rx_prop->rx[0].attrib.meta_data_mask;
					flt_rule_entry.rule.attrib.meta_data = rx_prop->rx[0].attrib.meta_data;

					/* check if the rule is define as TCP_UDP, split into 2 rules, 1 for TCP and 1 UDP */
					if (firewall_config.extd_firewall_entries[i].attrib.u.v4.protocol
							== IPACM_FIREWALL_IPPROTO_TCP_UDP)
					{
						/* insert TCP rule*/
						flt_rule_entry.rule.attrib.u.v4.protocol = IPACM_FIREWALL_IPPROTO_TCP;
						memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

						IPACMDBG_H("Filter rule attrib mask: 0x%x\n",
										 m_pFilteringTable->rules[0].rule.attrib.attrib_mask);
						if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
						{
							IPACMERR("Error Adding RuleTable(0) to Filtering, aborting...\n");
							free(m_pFilteringTable);
							return IPACM_FAILURE;
						}
						else
						{
							IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v4, 1);
							/* save v4 firewall filter rule handler */
							IPACMDBG_H("flt rule hdl0=0x%x, status=0x%x\n",
											 m_pFilteringTable->rules[rule_v4].flt_rule_hdl,
											 m_pFilteringTable->rules[rule_v4].status);
							firewall_hdl_v4[rule_v4] = m_pFilteringTable->rules[0].flt_rule_hdl;
							num_firewall_v4++;
							rule_v4++;
						}

						/* insert UDP rule*/
						flt_rule_entry.rule.attrib.u.v4.protocol = IPACM_FIREWALL_IPPROTO_UDP;
						memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

						IPACMDBG_H("Filter rule attrib mask: 0x%x\n",
										 m_pFilteringTable->rules[0].rule.attrib.attrib_mask);
						if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
						{
							IPACMERR("Error Adding RuleTable(0) to Filtering, aborting...\n");
							free(m_pFilteringTable);
							return IPACM_FAILURE;
						}
						else
						{
							IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v4, 1);
							/* save v4 firewall filter rule handler */
							IPACMDBG_H("flt rule hdl0=0x%x, status=0x%x\n",
											 m_pFilteringTable->rules[rule_v4].flt_rule_hdl,
											 m_pFilteringTable->rules[rule_v4].status);
							firewall_hdl_v4[rule_v4] = m_pFilteringTable->rules[0].flt_rule_hdl;
							num_firewall_v4++;
							rule_v4++;
						}
					}
					else
					{
						memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

						IPACMDBG_H("Filter rule attrib mask: 0x%x\n",
										 m_pFilteringTable->rules[0].rule.attrib.attrib_mask);
						if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
						{
							IPACMERR("Error Adding RuleTable(0) to Filtering, aborting...\n");
							free(m_pFilteringTable);
							return IPACM_FAILURE;
						}
						else
						{
							IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v4, 1);
							/* save v4 firewall filter rule handler */
							IPACMDBG_H("flt rule hdl0=0x%x, status=0x%x\n",
											 m_pFilteringTable->rules[rule_v4].flt_rule_hdl,
											 m_pFilteringTable->rules[rule_v4].status);
							firewall_hdl_v4[rule_v4] = m_pFilteringTable->rules[0].flt_rule_hdl;
							num_firewall_v4++;
							rule_v4++;
						}
					}
				}
			} /* end of firewall ipv4 filter rule add for loop*/
            }

			/* configure default filter rule */
			memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

			flt_rule_entry.flt_rule_hdl = -1;
			flt_rule_entry.status = -1;

			/* firewall disable, all traffic are allowed */
            if(firewall_config.firewall_enable == true)
			{
			     flt_rule_entry.at_rear = true;

			     /* default action for v4 is go DST_NAT unless user set to exception*/
                             if(firewall_config.rule_action_accept == true)
			     {
			        flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;
			     }
			     else
			     {
					if(IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_mode == ROUTER)
					{
						flt_rule_entry.rule.action = IPA_PASS_TO_DST_NAT;
					}
					else
					{
						flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
					}
				}
		    }
			else
			{
			    flt_rule_entry.at_rear = true;
				if(IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_mode == ROUTER)
				{
					flt_rule_entry.rule.action = IPA_PASS_TO_DST_NAT;
				}
				else
				{
					flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
				}
            }
#ifdef FEATURE_IPA_V3
			flt_rule_entry.rule.hashable = true;
#endif
			flt_rule_entry.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_lan_v4.hdl;
			memcpy(&flt_rule_entry.rule.attrib,
						 &rx_prop->rx[0].attrib,
						 sizeof(struct ipa_rule_attrib));
			flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
			flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = 0x00000000;
			flt_rule_entry.rule.attrib.u.v4.dst_addr = 0x00000000;

			memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

			IPACMDBG_H("Filter rule attrib mask: 0x%x\n",
							 m_pFilteringTable->rules[0].rule.attrib.attrib_mask);
			if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
			{
				IPACMERR("Error Adding RuleTable(0) to Filtering, aborting...\n");
				free(m_pFilteringTable);
				return IPACM_FAILURE;
			}
			else
			{
				IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v4, 1);
				IPACMDBG_H("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
			}

			/* copy filter hdls */
			dft_wan_fl_hdl[0] = m_pFilteringTable->rules[0].flt_rule_hdl;
		}

	}
	else
	{
		if (rule_v6 == 0)
		{
			memset(m_pFilteringTable, 0, len);

			m_pFilteringTable->commit = 1;
			m_pFilteringTable->ep = rx_prop->rx[0].src_pipe;
			m_pFilteringTable->global = false;
			m_pFilteringTable->ip = IPA_IP_v6;
			m_pFilteringTable->num_rules = (uint8_t)1;

			/* Construct ICMP rule */
			memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
			flt_rule_entry.at_rear = true;
			flt_rule_entry.flt_rule_hdl = -1;
			flt_rule_entry.status = -1;
			flt_rule_entry.rule.retain_hdr = 1;
			flt_rule_entry.rule.eq_attrib_type = 0;
			flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;
#ifdef FEATURE_IPA_V3
			flt_rule_entry.rule.hashable = true;
#endif
			memcpy(&flt_rule_entry.rule.attrib,
					 &rx_prop->rx[0].attrib,
					 sizeof(struct ipa_rule_attrib));
			flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_NEXT_HDR;
			flt_rule_entry.rule.attrib.u.v6.next_hdr = (uint8_t)IPACM_FIREWALL_IPPROTO_ICMP6;
			memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

			if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
			{
				IPACMERR("Error Adding Filtering rules, aborting...\n");
				free(m_pFilteringTable);
				return IPACM_FAILURE;
			}
			else
			{
				IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, 1);
				IPACMDBG_H("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
			}
			/* copy filter hdls */
			dft_wan_fl_hdl[2] = m_pFilteringTable->rules[0].flt_rule_hdl;

			/* End of construct ICMP rule */

			/* v6 default route */
			memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
			if (false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_wan_v6)) //rt_tbl_wan_v6 rt_tbl_v6
			{
				IPACMERR("m_routing.GetRoutingTable(rt_tbl_wan_v6) Failed.\n");
				free(m_pFilteringTable);
				return IPACM_FAILURE;
			}

			flt_rule_entry.flt_rule_hdl = -1;
			flt_rule_entry.status = -1;
			flt_rule_entry.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_wan_v6.hdl;

			/* firewall disable, all traffic are allowed */
			if (firewall_config.firewall_enable == true)
			{
				flt_rule_entry.at_rear = true;

				if (firewall_config.rule_action_accept == true)
				{
					flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;
				}
				else
				{
					if (IPACM_Iface::ipacmcfg->IsIpv6CTEnabled() &&
						IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_mode == ROUTER)
					{
						flt_rule_entry.rule.action = IPA_PASS_TO_DST_NAT;
					}
					else
					{
						flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
					}
				}
			}
			else
			{
				flt_rule_entry.at_rear = true;
				if (IPACM_Iface::ipacmcfg->IsIpv6CTEnabled() &&
					IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_mode == ROUTER)
				{
					flt_rule_entry.rule.action = IPA_PASS_TO_DST_NAT;
				}
				else
				{
					flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
				}
			}
#ifdef FEATURE_IPA_V3
			flt_rule_entry.rule.hashable = true;
#endif
			memcpy(&flt_rule_entry.rule.attrib,
				&rx_prop->rx[0].attrib,
				sizeof(struct ipa_rule_attrib));
			flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
			flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[0] = 0x00000000;
			flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[1] = 0x00000000;
			flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[2] = 0x00000000;
			flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[3] = 0x00000000;
			flt_rule_entry.rule.attrib.u.v6.dst_addr[0] = 0X00000000;
			flt_rule_entry.rule.attrib.u.v6.dst_addr[1] = 0x00000000;
			flt_rule_entry.rule.attrib.u.v6.dst_addr[2] = 0x00000000;
			flt_rule_entry.rule.attrib.u.v6.dst_addr[3] = 0X00000000;

			memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

			if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
			{
				IPACMERR("Error Adding Filtering rules, aborting...\n");
				free(m_pFilteringTable);
				return IPACM_FAILURE;
			}
			else
			{
				IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, 1);
				IPACMDBG_H("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
			}

			/* copy filter hdls */
			dft_wan_fl_hdl[1] = m_pFilteringTable->rules[0].flt_rule_hdl;
		}
		else
		{
			memset(m_pFilteringTable, 0, len);

			m_pFilteringTable->commit = 1;
			m_pFilteringTable->ep = rx_prop->rx[0].src_pipe;
			m_pFilteringTable->global = false;
			m_pFilteringTable->ip = IPA_IP_v6;
			m_pFilteringTable->num_rules = (uint8_t)1;

			if (false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_wan_v6))
			{
				IPACMERR("m_routing.GetRoutingTable(rt_tbl_wan_v6) Failed.\n");
				free(m_pFilteringTable);
				return IPACM_FAILURE;
			}

			if (firewall_config.firewall_enable == true)
			{
				rule_v6 = 0;
				for (i = 0; i < firewall_config.num_extd_firewall_entries; i++)
				{
#ifndef FEATURE_IPACM_UL_FIREWALL
					if (firewall_config.extd_firewall_entries[i].ip_vsn == 6)
#else //n FEATURE_IPACM_UL_FIREWALL
					if (firewall_config.extd_firewall_entries[i].ip_vsn == 6 &&
						firewall_config.extd_firewall_entries[i].firewall_direction !=
						IPACM_MSGR_UL_FIREWALL)
#endif //n FEATURE_IPACM_UL_FIREWALL
					{
						memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

						flt_rule_entry.at_rear = true;
						flt_rule_entry.flt_rule_hdl = -1;
						flt_rule_entry.status = -1;

						if (firewall_config.rule_action_accept == true)
						{
							if (IPACM_Iface::ipacmcfg->IsIpv6CTEnabled() &&
								IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_mode == ROUTER)
							{
								flt_rule_entry.rule.action = IPA_PASS_TO_DST_NAT;
							}
							else
							{
								flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
							}
						}
						else
						{
							flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;
						}
#ifdef FEATURE_IPA_V3
						flt_rule_entry.rule.hashable = true;
#endif
						flt_rule_entry.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_wan_v6.hdl;
						memcpy(&flt_rule_entry.rule.attrib,
							&firewall_config.extd_firewall_entries[i].attrib,
							sizeof(struct ipa_rule_attrib));
						flt_rule_entry.rule.attrib.attrib_mask |= rx_prop->rx[0].attrib.attrib_mask;
						flt_rule_entry.rule.attrib.meta_data_mask = rx_prop->rx[0].attrib.meta_data_mask;
						flt_rule_entry.rule.attrib.meta_data = rx_prop->rx[0].attrib.meta_data;

						/* check if the rule is define as TCP/UDP */
						if (firewall_config.extd_firewall_entries[i].attrib.u.v6.next_hdr == IPACM_FIREWALL_IPPROTO_TCP_UDP)
						{
							/* insert TCP rule*/
							flt_rule_entry.rule.attrib.u.v6.next_hdr = IPACM_FIREWALL_IPPROTO_TCP;
							memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
							if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
							{
								IPACMERR("Error Adding Filtering rules, aborting...\n");
								free(m_pFilteringTable);
								return IPACM_FAILURE;
							}
							else
							{
								IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, 1);
								/* save v4 firewall filter rule handler */
								IPACMDBG_H("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
								firewall_hdl_v6[rule_v6] = m_pFilteringTable->rules[0].flt_rule_hdl;
								num_firewall_v6++;
								rule_v6++;
							}

							/* insert UDP rule*/
							flt_rule_entry.rule.attrib.u.v6.next_hdr = IPACM_FIREWALL_IPPROTO_UDP;
							memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
							if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
							{
								IPACMERR("Error Adding Filtering rules, aborting...\n");
								free(m_pFilteringTable);
								return IPACM_FAILURE;
							}
							else
							{
								IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, 1);
								/* save v6 firewall filter rule handler */
								IPACMDBG_H("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
								firewall_hdl_v6[rule_v6] = m_pFilteringTable->rules[0].flt_rule_hdl;
								num_firewall_v6++;
								rule_v6++;
							}
						}
						else
						{
							memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
							if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
							{
								IPACMERR("Error Adding Filtering rules, aborting...\n");
								free(m_pFilteringTable);
								return IPACM_FAILURE;
							}
							else
							{
								IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, 1);
								/* save v6 firewall filter rule handler */
								IPACMDBG_H("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
								firewall_hdl_v6[rule_v6] = m_pFilteringTable->rules[0].flt_rule_hdl;
								num_firewall_v6++;
								rule_v6++;
							}
						}
					}
				} /* end of firewall ipv6 filter rule add for loop*/
			}

			/* Construct ICMP rule */
			memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
			flt_rule_entry.at_rear = true;
			flt_rule_entry.flt_rule_hdl = -1;
			flt_rule_entry.status = -1;
			flt_rule_entry.rule.retain_hdr = 1;
			flt_rule_entry.rule.eq_attrib_type = 0;
			flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;
#ifdef FEATURE_IPA_V3
			flt_rule_entry.rule.hashable = true;
#endif
			memcpy(&flt_rule_entry.rule.attrib,
				&rx_prop->rx[0].attrib,
				sizeof(struct ipa_rule_attrib));
			flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_NEXT_HDR;
			flt_rule_entry.rule.attrib.u.v6.next_hdr = (uint8_t)IPACM_FIREWALL_IPPROTO_ICMP6;
			memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

			if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
			{
				IPACMERR("Error Adding Filtering rules, aborting...\n");
				free(m_pFilteringTable);
				return IPACM_FAILURE;
			}
			else
			{
				IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, 1);
				IPACMDBG_H("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
			}
			/* copy filter hdls */
			dft_wan_fl_hdl[2] = m_pFilteringTable->rules[0].flt_rule_hdl;
			/* End of construct ICMP rule */

			/* setup default wan filter rule */
			memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

			flt_rule_entry.flt_rule_hdl = -1;
			flt_rule_entry.status = -1;
			flt_rule_entry.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_wan_v6.hdl;

			/* firewall disable, all traffic are allowed */
			if (firewall_config.firewall_enable == true)
			{
				flt_rule_entry.at_rear = true;

				if (firewall_config.rule_action_accept == true)
				{
					flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;
				}
				else
				{
					if (IPACM_Iface::ipacmcfg->IsIpv6CTEnabled() &&
						IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_mode == ROUTER)
					{
						flt_rule_entry.rule.action = IPA_PASS_TO_DST_NAT;
					}
					else
					{
						flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
					}
				}
			}
			else
			{
				flt_rule_entry.at_rear = true;
				if (IPACM_Iface::ipacmcfg->IsIpv6CTEnabled() &&
					IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_mode == ROUTER)
				{
					flt_rule_entry.rule.action = IPA_PASS_TO_DST_NAT;
				}
				else
				{
					flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
				}
			}
#ifdef FEATURE_IPA_V3
			flt_rule_entry.rule.hashable = true;
#endif
			memcpy(&flt_rule_entry.rule.attrib,
				&rx_prop->rx[0].attrib,
				sizeof(struct ipa_rule_attrib));
			flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
			flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[0] = 0x00000000;
			flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[1] = 0x00000000;
			flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[2] = 0x00000000;
			flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[3] = 0x00000000;
			flt_rule_entry.rule.attrib.u.v6.dst_addr[0] = 0X00000000;
			flt_rule_entry.rule.attrib.u.v6.dst_addr[1] = 0x00000000;
			flt_rule_entry.rule.attrib.u.v6.dst_addr[2] = 0x00000000;
			flt_rule_entry.rule.attrib.u.v6.dst_addr[3] = 0X00000000;

			memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

			if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
			{
				IPACMERR("Error Adding Filtering rules, aborting...\n");
				free(m_pFilteringTable);
				return IPACM_FAILURE;
			}
			else
			{
				IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, 1);
				IPACMDBG_H("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
			}
			/* copy filter hdls*/
			dft_wan_fl_hdl[1] = m_pFilteringTable->rules[0].flt_rule_hdl;
		}
	}

	if (m_pFilteringTable != NULL)
	{
		free(m_pFilteringTable);
	}
	return IPACM_SUCCESS;
}

#ifdef FEATURE_VLAN_MPDN
int IPACM_Wan::get_v6_pdn_firewall_configs(
	std::pair<IPACM_firewall_conf_t*, ipacm_ipv6_wan_iface*> wan_firewall_pair[],
	IPACM_firewall_t &firewall_configs)
{
	int num_v6_pdns = 0;

	for(uint32_t i = 0;
	i < IPA_MAX_NUM_SW_PDNS && num_v6_pdns < IPA_MAX_NUM_HW_PDNS;
		++i)
	{
		if(ipv6_to_iface[i].pIface &&
			(ipv6_to_iface[i].wan_up_vlan_v6 || isDefaultGatewayIfaceUp_v6(ipv6_to_iface[i].pIface)))
		{
			IPACMDBG_H("identified v6 pdn (%s): wan_up_v6: %d, wan_up_vlan_v6: %d, getting FW config\n",
				ipv6_to_iface[i].pIface->dev_name,
				isDefaultGatewayIfaceUp_v6(ipv6_to_iface[i].pIface),
				ipv6_to_iface[i].wan_up_vlan_v6);

			IPACM_firewall_conf_t* curr_pdn_firewall_config =
				get_curr_pdn_firewall_config(firewall_configs, ipv6_to_iface[i].pIface->dev_name);
			if(curr_pdn_firewall_config != NULL)
			{
				std::pair<IPACM_firewall_conf_t*, ipacm_ipv6_wan_iface*>* curr =
					&wan_firewall_pair[num_v6_pdns++];
				curr->first = curr_pdn_firewall_config;
				curr->second = &ipv6_to_iface[i];
			}
		}
	}
	IPACMDBG_H("found %d v6 pdns in firewall file\n", num_v6_pdns);
	return num_v6_pdns;
}
#endif

/* configure the initial firewall filter rules */
#ifdef FEATURE_VLAN_MPDN
int IPACM_Wan::config_dft_firewall_rules_ex(struct ipacm_pdn_flt_rule* rules, int rule_offset, ipa_ip_type iptype)
{
	IPACM_firewall_t firewall_config;
#else
int IPACM_Wan::config_dft_firewall_rules_ex(struct ipa_flt_rule_add *rules, int rule_offset, ipa_ip_type iptype)
{
	IPACM_firewall_conf_t firewall_config;
#endif
	int num_rules = 0, original_num_rules = 0, res, pos = rule_offset;

	IPACMDBG_H("ip-family: %d; \n", iptype);

	if (rx_prop == NULL)
	{
		IPACMDBG_H("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	if (rules == NULL || rule_offset < 0)
	{
		IPACMERR("No filtering table is available.\n");
		return IPACM_FAILURE;
	}

	IPACMDBG_H("Firewall XML file is %s\n", MOBILE_FIREWALL_FILE);
	if (IPACM_read_firewall_xml(MOBILE_FIREWALL_FILE, firewall_config) == IPACM_SUCCESS)
	{
		IPACMDBG_H("QCMAP Firewall XML read OK \n");
	}
	else
	{
		IPACMERR("QCMAP Firewall XML read failed, no such file, use default configuration \n");
	}

#ifdef FEATURE_VLAN_MPDN
	uint32_t offloaded_pdns_count_v4 = 0;
	std::pair<IPACM_firewall_conf_t*, ipacm_ipv4_wan_iface*> offloaded_pdns_v4[IPA_MAX_NUM_HW_PDNS];
	for (uint32_t i = 0;
		(i < IPA_MAX_NUM_SW_PDNS) && (offloaded_pdns_count_v4 < IPA_MAX_NUM_HW_PDNS);
		++i)
	{
		if (ipv4_to_iface[i].pIface &&
			(ipv4_to_iface[i].wan_up_vlan || isDefaultGatewayIfaceUp(ipv4_to_iface[i].pIface)))
		{
			IPACMDBG_H("identified pdn (%s): wan_up: %d, wan_up_vlan: %d, getting FW config\n",
				ipv4_to_iface[i].pIface->dev_name,
				isDefaultGatewayIfaceUp(ipv4_to_iface[i].pIface),
				ipv4_to_iface[i].wan_up_vlan);

			IPACM_firewall_conf_t* curr_pdn_firewall_config =
				get_curr_pdn_firewall_config(firewall_config, ipv4_to_iface[i].pIface->dev_name);
			if (curr_pdn_firewall_config != NULL)
			{
				std::pair<IPACM_firewall_conf_t*, ipacm_ipv4_wan_iface*>* curr =
					&offloaded_pdns_v4[offloaded_pdns_count_v4++];
				curr->first = curr_pdn_firewall_config;
				curr->second = &ipv4_to_iface[i];
			}
			else
			{
				IPACMERR("couldn't find pdn %s firewall config\n", ipv4_to_iface[i].pIface->dev_name);
			}
		}
	}
	IPACMDBG_H("found %d v4 pdns in firewall file\n", offloaded_pdns_count_v4);

	uint32_t offloaded_pdns_count_v6 = 0;
	std::pair<IPACM_firewall_conf_t*, ipacm_ipv6_wan_iface*> offloaded_pdns_v6[IPA_MAX_NUM_HW_PDNS];
	for (uint32_t i = 0;
		i < IPA_MAX_NUM_SW_PDNS && offloaded_pdns_count_v6 < IPA_MAX_NUM_HW_PDNS;
		++i)
	{
		if (ipv6_to_iface[i].pIface &&
			(ipv6_to_iface[i].wan_up_vlan_v6 || isDefaultGatewayIfaceUp_v6(ipv6_to_iface[i].pIface)))
		{
			IPACMDBG_H("identified v6 pdn (%s): wan_up_v6: %d, wan_up_vlan_v6: %d, getting FW config\n",
				ipv6_to_iface[i].pIface->dev_name,
				isDefaultGatewayIfaceUp_v6(ipv6_to_iface[i].pIface),
				ipv6_to_iface[i].wan_up_vlan_v6);

			IPACM_firewall_conf_t* curr_pdn_firewall_config =
				get_curr_pdn_firewall_config(firewall_config, ipv6_to_iface[i].pIface->dev_name);
			if (curr_pdn_firewall_config != NULL)
			{
				std::pair<IPACM_firewall_conf_t*, ipacm_ipv6_wan_iface*>* curr =
					&offloaded_pdns_v6[offloaded_pdns_count_v6++];
				curr->first = curr_pdn_firewall_config;
				curr->second = &ipv6_to_iface[i];
			}
		}
	}
	IPACMDBG_H("found %d v6 pdns in firewall file\n", offloaded_pdns_count_v6);
#endif

	/* add IPv6 frag rule when firewall is enabled*/
	if (iptype == IPA_IP_v6 && !ipacmcfg->IsIpv6CTEnabled())
	{
#ifdef FEATURE_VLAN_MPDN
		for (uint32_t i = 0; i < offloaded_pdns_count_v6; ++i)
		{
			IPACM_firewall_conf_t *curr_firewall_config = offloaded_pdns_v6[i].first;
			if (curr_firewall_config->firewall_enable && check_dft_firewall_rules_attr_mask(curr_firewall_config))
			{
				IPACM_Wan* curr_interface = offloaded_pdns_v6[i].second->pIface;
				IPACMDBG_H("adding frag rule for iface %s\n", curr_interface->dev_name);
				res = add_ipv6_frag_filtering_rule_ex(curr_interface->rx_prop->rx[0].attrib, rules[pos].flt_rule, pos);
				if (res != IPACM_SUCCESS)
				{
					return res;
				}
				rules[pos].mux_id = curr_interface->ext_prop->ext[0].mux_id;
				++pos;
			}
		}
#else
		if (firewall_config.firewall_enable && check_dft_firewall_rules_attr_mask(&firewall_config))
		{
			res = add_ipv6_frag_filtering_rule_ex(rx_prop->rx[0].attrib, rules[pos], pos);
			if (res != IPACM_SUCCESS)
			{
				return res;
			}
			++pos;
		}
#endif
	}

	if (iptype == IPA_IP_v4)
	{
		original_num_rules = IPACM_Wan::num_v4_flt_rule;
		IPACM_Wan::num_firewall_v4 = 0;
/* only install ipv4 DL firewall on modem endpoint when UL_FIREWALL FR not there */
#ifndef FEATURE_IPACM_UL_FIREWALL
#ifdef FEATURE_VLAN_MPDN
		/* firewall rules for all PDNs which are up */
		for (uint32_t i = 0; i < offloaded_pdns_count_v4; ++i)
		{
			IPACM_Wan* curr_interface = offloaded_pdns_v4[i].second->pIface;
			IPACMDBG_H("adding firewall rules for iface %s\n", curr_interface->dev_name);
			res = add_firewall_rules_ex(*offloaded_pdns_v4[i].first, iptype, curr_interface->ext_prop->ext[0].mux_id,
				curr_interface->rx_prop->rx[0].attrib, rules, IPA_MAX_FLT_RULE - offloaded_pdns_count_v4, pos);
			if (res != IPACM_SUCCESS)
			{
				return res;
			}
		}
#else
		res = add_firewall_rules_ex(firewall_config, iptype, rx_prop->rx[0].attrib, rules, IPA_MAX_FLT_RULE - 1, pos);
		if (res != IPACM_SUCCESS)
		{
			return res;
		}
#endif
#endif //FEATURE_IPACM_UL_FIREWALL

#ifdef FEATURE_VLAN_MPDN
		/* default rule for all PDNs which are up */
		for (uint32_t i = 0; i < offloaded_pdns_count_v4; ++i)
		{
			IPACM_Wan* curr_interface = offloaded_pdns_v4[i].second->pIface;
			IPACMDBG_H("adding default rule for iface %s\n", curr_interface->dev_name);
			res = add_catchup_all_filtering_rule_each_pdn(*offloaded_pdns_v4[i].first, iptype,
				curr_interface->rx_prop->rx[0].attrib, rules[pos].flt_rule, pos);
			if (res != IPACM_SUCCESS)
			{
				return res;
			}
			rules[pos].mux_id = curr_interface->ext_prop->ext[0].mux_id;
			++pos;
		}
		if(offloaded_pdns_count_v4)
			num_rules = IPACM_Wan::num_v4_flt_rule - original_num_rules - 1;
		else
			num_rules = 0;
#else
		res = add_catchup_all_filtering_rule_each_pdn(firewall_config, iptype, rx_prop->rx[0].attrib, rules[pos], pos);
		if (res != IPACM_SUCCESS)
		{
			return res;
		}
		++pos;
		if(wan_up)
			num_rules = IPACM_Wan::num_v4_flt_rule - original_num_rules - 1;
		else
			num_rules = 0;
#endif
	}
	else
	{
		original_num_rules = IPACM_Wan::num_v6_flt_rule;
		IPACM_Wan::num_firewall_v6 = 0;

#ifdef FEATURE_VLAN_MPDN
		/* firewall rules for all PDNs which are up */
		for (uint32_t i = 0; i < offloaded_pdns_count_v6; ++i)
		{
			IPACM_Wan* curr_interface = offloaded_pdns_v6[i].second->pIface;
			IPACMDBG_H("adding firewall rules for iface %s\n", curr_interface->dev_name);
			res = add_firewall_rules_ex(*offloaded_pdns_v6[i].first, iptype, curr_interface->ext_prop->ext[0].mux_id,
				curr_interface->rx_prop->rx[0].attrib, rules, IPA_MAX_FLT_RULE - offloaded_pdns_count_v6, pos);
			if (res != IPACM_SUCCESS)
			{
				return res;
			}
		}
#else
		res = add_firewall_rules_ex(firewall_config, iptype, rx_prop->rx[0].attrib, rules, IPA_MAX_FLT_RULE - 1, pos);
		if (res != IPACM_SUCCESS)
		{
			return res;
		}
#endif

#ifdef FEATURE_VLAN_MPDN
		/* default rule for all PDNs which are up */
		for (uint32_t i = 0; i < offloaded_pdns_count_v6; ++i)
		{
			IPACM_Wan* curr_interface = offloaded_pdns_v6[i].second->pIface;
			IPACMDBG_H("adding default rule for iface %s\n", curr_interface->dev_name);
			res = add_catchup_all_filtering_rule_each_pdn(*offloaded_pdns_v6[i].first, iptype,
				curr_interface->rx_prop->rx[0].attrib, rules[pos].flt_rule, pos);
			if (res != IPACM_SUCCESS)
			{
				return res;
			}
			rules[pos].mux_id = curr_interface->ext_prop->ext[0].mux_id;
			++pos;
		}

		if(offloaded_pdns_count_v6)
			num_rules = IPACM_Wan::num_v6_flt_rule - original_num_rules - 1;
		else
			num_rules = 0;
#else
		res = add_catchup_all_filtering_rule_each_pdn(firewall_config, iptype, rx_prop->rx[1].attrib, rules[pos], pos);
		if (res != IPACM_SUCCESS)
		{
			return res;
		}
		++pos;
		if(wan_up_v6)
			num_rules = IPACM_Wan::num_v6_flt_rule - original_num_rules - 1;
		else
			num_rules = 0;
#endif
	}
	IPACMDBG_H("Constructed %d firewall rules for ip type %d\n", num_rules, iptype);
	return IPACM_SUCCESS;
}

#ifdef FEATURE_IPACM_UL_FIREWALL

int IPACM_Wan::read_firewall_filter_rules_ul(void)
{
	int i = 0;
#ifdef FEATURE_VLAN_MPDN
	std::pair<IPACM_firewall_conf_t*, ipacm_ipv6_wan_iface*> offloaded_pdns_v6[IPA_MAX_NUM_HW_PDNS];
	int firewall_num_v6_pdns_ul = 0;
	int num_mpdn_firewall_v6_ul[IPA_MAX_NUM_HW_PDNS];
#endif
	IPACMDBG_H("Firewall XML file is %s\n", MOBILE_FIREWALL_FILE);
#ifdef FEATURE_VLAN_MPDN
	if(IPACM_read_firewall_xml(MOBILE_FIREWALL_FILE, firewall_mpdn_config_ul) == IPACM_SUCCESS)
#else
	if(IPACM_read_firewall_xml(MOBILE_FIREWALL_FILE, firewall_config_ul) == IPACM_SUCCESS)
#endif
	{
		IPACMDBG_H("QCMAP Firewall XML read OK \n");
	}
	else
	{
		IPACMERR("QCMAP Firewall XML read failed, no such file, use default configuration \n");
		return IPACM_FAILURE;
	}
#ifdef FEATURE_VLAN_MPDN
	firewall_num_v6_pdns_ul = get_v6_pdn_firewall_configs(offloaded_pdns_v6, firewall_mpdn_config_ul);

	for(int j = 0; j < firewall_num_v6_pdns_ul; j++)
	{
		IPACM_firewall_conf_t* curr_conf = offloaded_pdns_v6[j].first;
		char *dev = offloaded_pdns_v6[j].second->pIface->dev_name;
		/* find the number of IPv6 UL firewall rules */
		if(curr_conf->firewall_enable)
		{
			num_mpdn_firewall_v6_ul[j] = 0;
			for(int i = 0; i < curr_conf->num_extd_firewall_entries; i++)
			{
				if(curr_conf->extd_firewall_entries[i].ip_vsn == 6 &&
					curr_conf->extd_firewall_entries[i].firewall_direction ==
					IPACM_MSGR_UL_FIREWALL)
				{
					num_mpdn_firewall_v6_ul[j]++;
				}
				/* limit the total number of firewall entries for this pdn,
				 * another limitation is applied when we install the rules on LAN prod or send to Q6
				 */
				if(num_mpdn_firewall_v6_ul[j] == IPACM_MAX_FIREWALL_ENTRIES)
				{
					IPACMERR("reached MAX num firewall rules, dev %s, j %d, num pdns %d\n",
						dev,
						j, firewall_num_v6_pdns_ul);
					break;
				}
			}
			IPACMDBG_H("dev %s, num ul firewall %d\n",
				dev,
				num_mpdn_firewall_v6_ul[j]);
		}
		else
		{
			IPACMDBG_H("firewall disabled, dev %s\n",
				dev);
			num_mpdn_firewall_v6_ul[j] = 0;
		}
	}
#else
	int total_num_firewall_v6_ul = 0;

	/* find the number of IPv6 UL firewall rules */
	for(i = 0; i < firewall_config_ul.num_extd_firewall_entries; i++)
	{
		if(firewall_config_ul.extd_firewall_entries[i].ip_vsn == 6 &&
			firewall_config_ul.extd_firewall_entries[i].firewall_direction ==
			IPACM_MSGR_UL_FIREWALL)
		{
			total_num_firewall_v6_ul++;
		}

		/* limit the total number of firewall entries for all pdns */
		if(total_num_firewall_v6_ul == IPACM_MAX_FIREWALL_ENTRIES)
		{
			IPACMDBG_H("reached maximal number of FW rules\n");
			break;
		}
	}
	IPACMDBG_H("firewall rule v6_ul:%d total:%d\n", total_num_firewall_v6_ul, firewall_config_ul.num_extd_firewall_entries);
#endif
	return IPACM_SUCCESS;
}

int IPACM_Wan::set_pdn_num_fw_rules_by_vid(int vid, int num_fw_rules)
{
#ifdef FEATURE_VLAN_MPDN
	for(int i = 0; i < IPA_MAX_NUM_SW_PDNS; i++)
	{
		if(ipv6_to_iface[i].pIface &&
			(ipv6_to_iface[i].wan_up_vlan_v6 || isDefaultGatewayIfaceUp_v6(ipv6_to_iface[i].pIface)))
		{
			if((ipv6_to_iface[i].pIface->associated_VID == vid) ||
				(isDefaultGatewayIfaceUp_v6(ipv6_to_iface[i].pIface) && !vid))
			{
				IPACMDBG_H("found dev %s, vid %d, num_ul_fw_rules %d update to %d\n",
					ipv6_to_iface[i].pIface->dev_name,
					ipv6_to_iface[i].pIface->associated_VID,
					ipv6_to_iface[i].pIface->num_firewall_v6_ul_pdn,
					num_fw_rules);
				int orig_num = IPACM_Wan::num_firewall_v6_ul;
				IPACM_Wan::num_firewall_v6_ul -= ipv6_to_iface[i].pIface->num_firewall_v6_ul_pdn;
				IPACM_Wan::num_firewall_v6_ul += num_fw_rules;
				IPACMDBG_H("num_firewall_v6_ul (%d)->(%d)\n", orig_num, IPACM_Wan::num_firewall_v6_ul);

				ipv6_to_iface[i].pIface->num_firewall_v6_ul_pdn = num_fw_rules;
				return IPACM_SUCCESS;
			}
		}
	}
#else
	if(vid == 0)
	{
		num_firewall_v6_ul = num_fw_rules;
		return IPACM_SUCCESS;
	}
#endif
	IPACMERR("couldn't find match for vid %d\n", vid);
	return IPACM_FAILURE;
}
int IPACM_Wan::get_pdn_num_fw_rules_by_vid(int vid, int *num_fw_rules)
{
#ifdef FEATURE_VLAN_MPDN
	for(int i = 0; i < IPA_MAX_NUM_SW_PDNS; i++)
	{
		if(ipv6_to_iface[i].pIface &&
			(ipv6_to_iface[i].wan_up_vlan_v6 || isDefaultGatewayIfaceUp_v6(ipv6_to_iface[i].pIface)))
		{
			if((ipv6_to_iface[i].pIface->associated_VID == vid) ||
				(isDefaultGatewayIfaceUp_v6(ipv6_to_iface[i].pIface) && !vid))
			{
				IPACMDBG_H("found dev %s, vid %d, num_ul_fw_rules %d\n",
					ipv6_to_iface[i].pIface->dev_name,
					ipv6_to_iface[i].pIface->associated_VID,
					ipv6_to_iface[i].pIface->num_firewall_v6_ul_pdn);
				*num_fw_rules = ipv6_to_iface[i].pIface->num_firewall_v6_ul_pdn;
				return IPACM_SUCCESS;
			}
		}
	}
#else
	if(vid == 0)
	{
		*num_fw_rules = num_firewall_v6_ul;
		return IPACM_SUCCESS;
	}
#endif
	IPACMERR("couldn't find match for vid %d\n", vid);
	return IPACM_FAILURE;
}

#ifdef FEATURE_VLAN_MPDN
int IPACM_Wan::GetV6PrefixByVid(int vid, uint32_t *v6_prefix)
{
	if(!vid)
	{
		v6_prefix[0] = backhaul_ipv6_prefix[0];
		v6_prefix[1] = backhaul_ipv6_prefix[1];
		return IPACM_SUCCESS;
	}

	for(int i = 0; i < IPA_MAX_NUM_SW_PDNS; i++)
	{
		if(ipv6_to_iface[i].pIface && ipv6_to_iface[i].wan_up_vlan_v6)
		{
			if((ipv6_to_iface[i].pIface->associated_VID == vid))
			{
				IPACMDBG_H("found dev %s, vid %d, v6_prefix 0x[%X][%X]\n",
					ipv6_to_iface[i].pIface->dev_name,
					ipv6_to_iface[i].pIface->associated_VID,
					ipv6_to_iface[i].pIface->ipv6_prefix[0],
					ipv6_to_iface[i].pIface->ipv6_prefix[1]);
				v6_prefix[0] = ipv6_to_iface[i].pIface->ipv6_prefix[0];
				v6_prefix[1] = ipv6_to_iface[i].pIface->ipv6_prefix[1];
				return IPACM_SUCCESS;
			}
		}
	}
	IPACMERR("couldn't find match for vid %d\n", vid);
	return IPACM_FAILURE;
}
#endif //FEATURE_VLAN_MPDN

IPACM_firewall_conf_t* IPACM_Wan::get_default_profile_firewall_conf_ul(int *default_vid)
{
	IPACM_firewall_conf_t* firewall_conf = NULL;
#ifdef FEATURE_VLAN_MPDN
	int idx = 0;
	int num_pdns = firewall_mpdn_config_ul.pdn_count;

	if(firewall_mpdn_config_ul.default_profile == 0)
	{
		if(firewall_mpdn_config_ul.pdn_count != 1)
		{
			IPACMERR("can't identify default pdn\n");
			return NULL;
		}
		idx = 0;
	}
	else
	{
		for(idx = 0; idx < num_pdns; ++idx)
		{
			if(firewall_mpdn_config_ul.default_profile == firewall_mpdn_config_ul.pdns[idx].profile)
			{
				break;
			}
		}
		if(idx == num_pdns)
		{
			IPACMERR("The XML is not valid. The default profile %d wasn't located\n",
				firewall_mpdn_config_ul.default_profile);
			return NULL;
		}
	}
	firewall_conf = &firewall_mpdn_config_ul.pdns[idx];
	*default_vid = 0;
	for(int i = 0; i < IPA_MAX_NUM_SW_PDNS; i++)
	{
		if(ipv6_to_iface[i].pIface &&
			(ipv6_to_iface[i].wan_up_vlan_v6 || isDefaultGatewayIfaceUp_v6(ipv6_to_iface[i].pIface)))
		{
			if(!strcmp(firewall_mpdn_config_ul.pdns[idx].net_dev,
				ipv6_to_iface[i].pIface->dev_name))
			{
				IPACMDBG("found %s dev in index %d, VID %d\n",
					firewall_mpdn_config_ul.pdns[idx].net_dev,
					i,
					ipv6_to_iface[i].pIface->associated_VID);
				*default_vid = ipv6_to_iface[i].pIface->associated_VID;
			}
		}
	}
#else
	firewall_conf = &firewall_config_ul;
	*default_vid = 0;
#endif
	return firewall_conf;
}

#ifdef FEATURE_VLAN_MPDN
IPACM_firewall_conf_t* IPACM_Wan::get_firewall_conf_by_vid_ul(int vid)
{
	int num_pdns = firewall_mpdn_config_ul.pdn_count;

	for(int i = 0; i < IPA_MAX_NUM_SW_PDNS; i++)
	{
		if(ipv6_to_iface[i].pIface &&
			(ipv6_to_iface[i].wan_up_vlan_v6 || isDefaultGatewayIfaceUp_v6(ipv6_to_iface[i].pIface)))
		{
			if(vid == ipv6_to_iface[i].pIface->associated_VID)
			{
				IPACMDBG("found %s dev in index %d, VID %d\n",
					ipv6_to_iface[i].pIface->dev_name,
					i,
					ipv6_to_iface[i].pIface->associated_VID);
				for(int j = 0; j < num_pdns; j++)
				{
					if(!strcmp(firewall_mpdn_config_ul.pdns[j].net_dev,
						ipv6_to_iface[i].pIface->dev_name))
					{
						IPACMDBG("found %s dev in index %d\n",
							firewall_mpdn_config_ul.pdns[j].net_dev, j);
						return &firewall_mpdn_config_ul.pdns[j];
					}
				}
			}
		}
	}
	return NULL;
}
#endif //FEATURE_VLAN_MPDN

#endif //FEATURE_IPACM_UL_FIREWALL
int IPACM_Wan::init_fl_rule_ex(ipa_ip_type iptype)
{
	int res = IPACM_SUCCESS;

	char *dev_wlan0="wlan0";
	char *dev_wlan1="wlan1";
	char *dev_ecm0="ecm0";

	/* ADD corresponding ipa_rm_resource_name of RX-endpoint before adding all IPV4V6 FT-rules */
	IPACMDBG_H(" dun add producer dependency from %s with registered rx-prop\n", dev_name);

	if(iptype == IPA_IP_v4)
	{
		if(modem_ipv4_pdn_index == 0)	/* install ipv4 default modem DL filtering rules only once */
		{
			/* reset the num_v4_flt_rule*/
			IPACM_Wan::num_v4_flt_rule = 0;
#ifdef FEATURE_VLAN_MPDN
			add_dft_filtering_rule(pdn_flt_rule_v4, IPACM_Wan::num_v4_flt_rule, IPA_IP_v4);
#else
			add_dft_filtering_rule(flt_rule_v4, IPACM_Wan::num_v4_flt_rule, IPA_IP_v4);
#endif
		}
	}
	else if(iptype == IPA_IP_v6)
	{
		if(modem_ipv6_pdn_index == 0)	/* install ipv6 default modem DL filtering rules only once */
		{
			/* reset the num_v6_flt_rule*/
			IPACM_Wan::num_v6_flt_rule = 0;
#ifdef FEATURE_VLAN_MPDN
			add_dft_filtering_rule(pdn_flt_rule_v6, IPACM_Wan::num_v6_flt_rule, IPA_IP_v6);
#else
			add_dft_filtering_rule(flt_rule_v6, IPACM_Wan::num_v6_flt_rule, IPA_IP_v6);
#endif
		}
	}
	else
	{
		IPACMERR("IP type is not expected.\n");
		res = IPACM_FAILURE;
		goto fail;
	}
	install_wan_filtering_rule(false);

fail:
	return res;
}

#ifdef FEATURE_VLAN_MPDN
int IPACM_Wan::add_icmp_alg_rules(struct ipacm_pdn_flt_rule *rules, int rule_offset, ipa_ip_type iptype)
#else
int IPACM_Wan::add_icmp_alg_rules(struct ipa_flt_rule_add *rules, int rule_offset, ipa_ip_type iptype)
#endif
{
	int res = IPACM_SUCCESS, i, original_num_rules = 0, num_rules = 0;
#ifdef FEATURE_VLAN_MPDN
	int num_icmp_rules = 0;
#endif
	struct ipa_flt_rule_add flt_rule_entry;
	IPACM_Config* ipacm_config = IPACM_Iface::ipacmcfg;
	ipa_ioc_generate_flt_eq flt_eq;
	ipa_ioc_get_rt_tbl_indx rt_tbl_idx;

	if(rules == NULL || rule_offset < 0)
	{
		IPACMERR("No filtering table is available.\n");
		return IPACM_FAILURE;
	}

	if(iptype == IPA_IP_v4)
	{
		original_num_rules = IPACM_Wan::num_v4_flt_rule;

		memset(&rt_tbl_idx, 0, sizeof(rt_tbl_idx));
		strlcpy(rt_tbl_idx.name, IPACM_Iface::ipacmcfg->rt_tbl_wan_dl.name, IPA_RESOURCE_NAME_MAX);
		rt_tbl_idx.name[IPA_RESOURCE_NAME_MAX-1] = '\0';
		rt_tbl_idx.ip = iptype;
		if(0 != ioctl(m_fd_ipa, IPA_IOC_QUERY_RT_TBL_INDEX, &rt_tbl_idx))
		{
			IPACMERR("Failed to get routing table index from name\n");
			res = IPACM_FAILURE;
			goto fail;
		}

		IPACMDBG_H("WAN DL routing table %s has index %d\n", IPACM_Iface::ipacmcfg->rt_tbl_wan_dl.name, rt_tbl_idx.idx);

		memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

		flt_rule_entry.at_rear = true;
		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;

		flt_rule_entry.rule.retain_hdr = 1;
		flt_rule_entry.rule.to_uc = 0;
		flt_rule_entry.rule.eq_attrib_type = 1;
		flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
#ifdef FEATURE_IPA_V3
		flt_rule_entry.rule.hashable = true;
#endif
		flt_rule_entry.rule.rt_tbl_idx = rt_tbl_idx.idx;

		/* Configuring ICMP filtering rule */
		memcpy(&flt_rule_entry.rule.attrib,
					 &rx_prop->rx[0].attrib,
					 sizeof(flt_rule_entry.rule.attrib));
		/* Multiple PDNs may exist so keep meta-data */
		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_PROTOCOL;
		flt_rule_entry.rule.attrib.u.v4.protocol = (uint8_t)IPACM_FIREWALL_IPPROTO_ICMP;

		memset(&flt_eq, 0, sizeof(flt_eq));
		memcpy(&flt_eq.attrib, &flt_rule_entry.rule.attrib, sizeof(flt_eq.attrib));
		flt_eq.ip = iptype;
		if(0 != ioctl(m_fd_ipa, IPA_IOC_GENERATE_FLT_EQ, &flt_eq))
		{
			IPACMERR("Failed to get eq_attrib\n");
			res = IPACM_FAILURE;
			goto fail;
		}

		memcpy(&flt_rule_entry.rule.eq_attrib,
					 &flt_eq.eq_attrib,
					 sizeof(flt_rule_entry.rule.eq_attrib));
#ifdef FEATURE_VLAN_MPDN
		num_icmp_rules = 0;
		for(i = 0; i < IPA_MAX_NUM_SW_PDNS; i++)
		{
			if(ipv4_to_iface[i].pIface &&
				(ipv4_to_iface[i].wan_up_vlan || isDefaultGatewayIfaceUp(ipv4_to_iface[i].pIface)))
			{
				IPACMDBG_H("adding ICMP rule for IF %s \n", ipv4_to_iface[i].pIface->dev_name);
				rules[rule_offset + i].mux_id = ipv4_to_iface[i].pIface->ext_prop->ext[0].mux_id;
				memcpy(&(rules[rule_offset + i].flt_rule), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
				IPACM_Wan::num_v4_flt_rule++;
				num_icmp_rules++;
			}
		}
#else
		memcpy(&(rules[rule_offset]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
		IPACM_Wan::num_v4_flt_rule++;
#endif

		/*
		 * Removed from here the code to general filtering rules for
		 * ALG ports. ALG ports are now handled by NAT exception for UL
		 * and NAT dummy rules for DL.
		 *
		 */

		num_rules = IPACM_Wan::num_v4_flt_rule - original_num_rules;
	}
	else /* IPv6 case */
	{
		original_num_rules = IPACM_Wan::num_v6_flt_rule;

		memset(&rt_tbl_idx, 0, sizeof(rt_tbl_idx));
		strlcpy(rt_tbl_idx.name, IPACM_Iface::ipacmcfg->rt_tbl_wan_dl.name, IPA_RESOURCE_NAME_MAX);
		rt_tbl_idx.name[IPA_RESOURCE_NAME_MAX-1] = '\0';
		rt_tbl_idx.ip = iptype;
		if(0 != ioctl(m_fd_ipa, IPA_IOC_QUERY_RT_TBL_INDEX, &rt_tbl_idx))
		{
			IPACMERR("Failed to get routing table index from name\n");
			res = IPACM_FAILURE;
			goto fail;
		}

		IPACMDBG_H("WAN DL routing table %s has index %d\n", IPACM_Iface::ipacmcfg->rt_tbl_wan_dl.name, rt_tbl_idx.idx);

		memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

		flt_rule_entry.at_rear = true;
		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;

		flt_rule_entry.rule.retain_hdr = 1;
		flt_rule_entry.rule.to_uc = 0;
		flt_rule_entry.rule.eq_attrib_type = 1;
		flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
#ifdef FEATURE_IPA_V3
		flt_rule_entry.rule.hashable = true;
#endif
		flt_rule_entry.rule.rt_tbl_idx = rt_tbl_idx.idx;

		/* Configuring ICMP filtering rule */
		memcpy(&flt_rule_entry.rule.attrib,
					 &rx_prop->rx[1].attrib,
					 sizeof(flt_rule_entry.rule.attrib));
		/* Multiple PDNs may exist so keep meta-data */
		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_NEXT_HDR;
		flt_rule_entry.rule.attrib.u.v6.next_hdr = (uint8_t)IPACM_FIREWALL_IPPROTO_ICMP6;

		memset(&flt_eq, 0, sizeof(flt_eq));
		memcpy(&flt_eq.attrib, &flt_rule_entry.rule.attrib, sizeof(flt_eq.attrib));
		flt_eq.ip = iptype;
		if(0 != ioctl(m_fd_ipa, IPA_IOC_GENERATE_FLT_EQ, &flt_eq))
		{
			IPACMERR("Failed to get eq_attrib\n");
			res = IPACM_FAILURE;
			goto fail;
		}

		memcpy(&flt_rule_entry.rule.eq_attrib,
					 &flt_eq.eq_attrib,
					 sizeof(flt_rule_entry.rule.eq_attrib));
#ifdef FEATURE_VLAN_MPDN
		num_icmp_rules = 0;
		for(i = 0; i < IPA_MAX_NUM_SW_PDNS; i++)
		{
			if(ipv6_to_iface[i].pIface && (ipv6_to_iface[i].wan_up_vlan_v6 || isDefaultGatewayIfaceUp_v6(ipv6_to_iface[i].pIface)))
			{
				IPACMDBG_H("adding ICMPv6 rule for IF %s \n", ipv6_to_iface[i].pIface->dev_name);
				rules[rule_offset + i].mux_id = ipv6_to_iface[i].pIface->ext_prop->ext[0].mux_id;
				memcpy(&(rules[rule_offset + i].flt_rule), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
				IPACM_Wan::num_v6_flt_rule++;
				num_icmp_rules++;
			}
		}
#else
		memcpy(&(rules[rule_offset]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
		IPACM_Wan::num_v6_flt_rule++;
#endif

		num_rules = IPACM_Wan::num_v6_flt_rule - original_num_rules;
	}

fail:
	IPACMDBG_H("Constructed %d ICMP/ALG rules for ip type %d\n", num_rules, iptype);
		return res;
}

int IPACM_Wan::query_ext_prop()
{
	int fd, ret = IPACM_SUCCESS, cnt;

	if (iface_query->num_ext_props > 0)
	{
		fd = open(IPA_DEVICE_NAME, O_RDWR);
		IPACMDBG_H("iface query-property \n");
		if (0 == fd)
		{
			IPACMERR("Failed opening %s.\n", IPA_DEVICE_NAME);
			return IPACM_FAILURE;
		}

		ext_prop = (struct ipa_ioc_query_intf_ext_props *)
			 calloc(1, sizeof(struct ipa_ioc_query_intf_ext_props) +
							iface_query->num_ext_props * sizeof(struct ipa_ioc_ext_intf_prop));
		if(ext_prop == NULL)
		{
			IPACMERR("Unable to allocate memory.\n");
			return IPACM_FAILURE;
		}
		memcpy(ext_prop->name, dev_name,
					 sizeof(dev_name));
		ext_prop->num_ext_props = iface_query->num_ext_props;

		IPACMDBG_H("Query extended property for iface %s\n", ext_prop->name);

		ret = ioctl(fd, IPA_IOC_QUERY_INTF_EXT_PROPS, ext_prop);
		if (ret < 0)
		{
			IPACMERR("ioctl IPA_IOC_QUERY_INTF_EXT_PROPS failed\n");
			/* ext_prop memory will free when iface-down*/
			free(ext_prop);
			close(fd);
			return ret;
		}

		IPACMDBG_H("Wan interface has %d tx props, %d rx props and %d ext props\n",
				iface_query->num_tx_props, iface_query->num_rx_props, iface_query->num_ext_props);

		for (cnt = 0; cnt < ext_prop->num_ext_props; cnt++)
		{
#ifndef FEATURE_IPA_V3
			IPACMDBG_H("Ex(%d): ip-type: %d, mux_id: %d, flt_action: %d\n, rt_tbl_idx: %d, is_xlat_rule: %d flt_hdl: %d\n",
				cnt, ext_prop->ext[cnt].ip, ext_prop->ext[cnt].mux_id, ext_prop->ext[cnt].action,
				ext_prop->ext[cnt].rt_tbl_idx, ext_prop->ext[cnt].is_xlat_rule, ext_prop->ext[cnt].filter_hdl);
#else /* defined (FEATURE_IPA_V3) */
			IPACMDBG_H("Ex(%d): ip-type: %d, mux_id: %d, flt_action: %d\n, rt_tbl_idx: %d, is_xlat_rule: %d rule_id: %d\n",
				cnt, ext_prop->ext[cnt].ip, ext_prop->ext[cnt].mux_id, ext_prop->ext[cnt].action,
				ext_prop->ext[cnt].rt_tbl_idx, ext_prop->ext[cnt].is_xlat_rule, ext_prop->ext[cnt].rule_id);
#endif
		}

		if(IPACM_Wan::is_ext_prop_set == false)
		{
			IPACM_Iface::ipacmcfg->SetExtProp(ext_prop);
			IPACM_Wan::is_ext_prop_set = true;
		}
		close(fd);
	}
	return IPACM_SUCCESS;
}

int IPACM_Wan::config_wan_firewall_rule(ipa_ip_type iptype)
{
	list<l2tp_client_info>::iterator it;
	int res = IPACM_SUCCESS;

	IPACMDBG_H("Configure WAN DL firewall rules.\n");

	if(iptype == IPA_IP_v4)
	{
		IPACM_Wan::num_v4_flt_rule = IPA_V2_NUM_DEFAULT_WAN_FILTER_RULE_IPV4;
#ifdef FEATURE_VLAN_MPDN
		if(IPACM_FAILURE == add_icmp_alg_rules(pdn_flt_rule_v4, IPACM_Wan::num_v4_flt_rule, IPA_IP_v4))
#else
		if(IPACM_FAILURE == add_icmp_alg_rules(flt_rule_v4, IPACM_Wan::num_v4_flt_rule, IPA_IP_v4))
#endif
		{
			IPACMERR("Failed to add ICMP and ALG port filtering rules.\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		IPACMDBG_H("Succeded in constructing ICMP/ALG rules for ip type %d\n", iptype);

#ifdef FEATURE_VLAN_MPDN
		if(IPACM_FAILURE == config_dft_firewall_rules_ex(pdn_flt_rule_v4, IPACM_Wan::num_v4_flt_rule, IPA_IP_v4))
#else
		if(IPACM_FAILURE == config_dft_firewall_rules_ex(flt_rule_v4, IPACM_Wan::num_v4_flt_rule, IPA_IP_v4))
#endif
		{
			IPACMERR("Failed to add firewall filtering rules.\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		IPACMDBG_H("Succeded in constructing firewall rules for ip type %d\n", iptype);
#ifdef FEATURE_L2TP_E2E
		for(it = IPACM_Iface::ipacmcfg->l2tp_client.begin(); it != IPACM_Iface::ipacmcfg->l2tp_client.end(); it++)
		{
			handle_l2tp_client_add(it->client_iface_name);
		}
#endif
	}
	else if(iptype == IPA_IP_v6)
	{
#ifdef FEATURE_VLAN_MPDN
		IPACM_Wan::num_v6_flt_rule = IPACM_Wan::ipv6_mpdn_default_filterting_rules_count;
#else
		IPACM_Wan::num_v6_flt_rule = m_ipv6_default_filterting_rules_count;
#endif
#ifdef FEATURE_L2TP_E2E
		if(active_v4)
		{
			IPACM_Wan::num_v6_flt_rule += IPACM_Iface::ipacmcfg->l2tp_client.size();
		}
#endif
#ifdef FEATURE_VLAN_MPDN
		if(IPACM_FAILURE == add_icmp_alg_rules(pdn_flt_rule_v6, IPACM_Wan::num_v6_flt_rule, IPA_IP_v6))
#else
		if(IPACM_FAILURE == add_icmp_alg_rules(flt_rule_v6, IPACM_Wan::num_v6_flt_rule, IPA_IP_v6))
#endif
		{
			IPACMERR("Failed to add ICMP and ALG port filtering rules.\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		IPACMDBG_H("Succeded in constructing ICMP/ALG rules for ip type %d\n", iptype);

#ifdef FEATURE_VLAN_MPDN
		if(IPACM_FAILURE == config_dft_firewall_rules_ex(pdn_flt_rule_v6, IPACM_Wan::num_v6_flt_rule, IPA_IP_v6))
#else
		if(IPACM_FAILURE == config_dft_firewall_rules_ex(flt_rule_v6, IPACM_Wan::num_v6_flt_rule, IPA_IP_v6))
#endif
		{
			IPACMERR("Failed to add firewall filtering rules.\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		IPACMDBG_H("Succeded in constructing firewall rules for ip type %d\n", iptype);
	}
	else
	{
		IPACMERR("IP type is not expected.\n");
		return IPACM_FAILURE;
	}

fail:
	return res;
}

#ifdef FEATURE_VLAN_MPDN
int IPACM_Wan::add_dft_filtering_rule(ipacm_pdn_flt_rule *rules, int rule_offset, ipa_ip_type iptype)
#else
int IPACM_Wan::add_dft_filtering_rule(struct ipa_flt_rule_add *rules, int rule_offset, ipa_ip_type iptype)
#endif
{
	struct ipa_ioc_get_rt_tbl_indx rt_tbl_idx;
	struct ipa_flt_rule_add flt_rule_entry;
	struct ipa_ioc_generate_flt_eq flt_eq;
	int res = IPACM_SUCCESS;

	if(rules == NULL)
	{
		IPACMERR("No filtering table available.\n");
		return IPACM_FAILURE;
	}
	if(rx_prop == NULL)
	{
		IPACMERR("No tx property.\n");
		return IPACM_FAILURE;
	}

	if (iptype == IPA_IP_v4)
	{
		memset(&rt_tbl_idx, 0, sizeof(rt_tbl_idx));
		strlcpy(rt_tbl_idx.name, IPACM_Iface::ipacmcfg->rt_tbl_wan_dl.name, IPA_RESOURCE_NAME_MAX);
		rt_tbl_idx.name[IPA_RESOURCE_NAME_MAX-1] = '\0';
		rt_tbl_idx.ip = iptype;
		if(0 != ioctl(m_fd_ipa, IPA_IOC_QUERY_RT_TBL_INDEX, &rt_tbl_idx))
		{
			IPACMERR("Failed to get routing table index from name\n");
			res = IPACM_FAILURE;
			goto fail;
		}

		IPACMDBG_H("Routing table %s has index %d\n", rt_tbl_idx.name, rt_tbl_idx.idx);

		memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

		flt_rule_entry.at_rear = true;
		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;

		flt_rule_entry.rule.retain_hdr = 1;
		flt_rule_entry.rule.to_uc = 0;
		flt_rule_entry.rule.eq_attrib_type = 1;
		flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
#ifdef FEATURE_IPA_V3
		flt_rule_entry.rule.hashable = true;
#endif
		flt_rule_entry.rule.rt_tbl_idx = rt_tbl_idx.idx;

		IPACMDBG_H("rx property attrib mask:0x%x\n", rx_prop->rx[0].attrib.attrib_mask);

		/* Configuring Multicast Filtering Rule */
		memcpy(&flt_rule_entry.rule.attrib,
					 &rx_prop->rx[0].attrib,
					 sizeof(flt_rule_entry.rule.attrib));
		/* remove meta data mask since we only install default flt rules once for all modem PDN*/
		flt_rule_entry.rule.attrib.attrib_mask &= ~((uint32_t)IPA_FLT_META_DATA);
		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = 0xF0000000;
		flt_rule_entry.rule.attrib.u.v4.dst_addr = 0xE0000000;

		change_to_network_order(IPA_IP_v4, &flt_rule_entry.rule.attrib);

		memset(&flt_eq, 0, sizeof(flt_eq));
		memcpy(&flt_eq.attrib, &flt_rule_entry.rule.attrib, sizeof(flt_eq.attrib));
		flt_eq.ip = iptype;
		if(0 != ioctl(m_fd_ipa, IPA_IOC_GENERATE_FLT_EQ, &flt_eq))
		{
			IPACMERR("Failed to get eq_attrib\n");
			res = IPACM_FAILURE;
			goto fail;
		}

		memcpy(&flt_rule_entry.rule.eq_attrib,
					 &flt_eq.eq_attrib,
					 sizeof(flt_rule_entry.rule.eq_attrib));
#ifdef FEATURE_VLAN_MPDN
		/* default rules are not metadata dependant - mux_id is not relevant */
		rules[rule_offset].mux_id = 0;
		memcpy(&(rules[rule_offset].flt_rule), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
#else
		memcpy(&(rules[rule_offset]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
#endif

		/* Configuring Broadcast Filtering Rule */
		flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;
		flt_rule_entry.rule.attrib.u.v4.dst_addr = 0xFFFFFFFF;

		change_to_network_order(IPA_IP_v4, &flt_rule_entry.rule.attrib);

		memset(&flt_eq, 0, sizeof(flt_eq));
		memcpy(&flt_eq.attrib, &flt_rule_entry.rule.attrib, sizeof(flt_eq.attrib));
		flt_eq.ip = iptype;
		if(0 != ioctl(m_fd_ipa, IPA_IOC_GENERATE_FLT_EQ, &flt_eq))
		{
			IPACMERR("Failed to get eq_attrib\n");
			res = IPACM_FAILURE;
			goto fail;
		}

		memcpy(&flt_rule_entry.rule.eq_attrib,
					 &flt_eq.eq_attrib,
					 sizeof(flt_rule_entry.rule.eq_attrib));
#ifdef FEATURE_VLAN_MPDN
		/* default rules are not metadata dependant - mux_id is not relevant */
		rules[rule_offset + 1].mux_id = 0;
		memcpy(&(rules[rule_offset + 1].flt_rule), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
#else
		memcpy(&(rules[rule_offset + 1]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
#endif

		IPACM_Wan::num_v4_flt_rule += IPA_V2_NUM_DEFAULT_WAN_FILTER_RULE_IPV4;
		IPACMDBG_H("Constructed %d default filtering rules for ip type %d\n", IPA_V2_NUM_DEFAULT_WAN_FILTER_RULE_IPV4, iptype);
	}
	else	/*insert rules for ipv6*/
	{
		m_ipv6_default_filterting_rules_count = 0;
		memset(&rt_tbl_idx, 0, sizeof(rt_tbl_idx));
		strlcpy(rt_tbl_idx.name, IPACM_Iface::ipacmcfg->rt_tbl_wan_dl.name, IPA_RESOURCE_NAME_MAX);
		rt_tbl_idx.name[IPA_RESOURCE_NAME_MAX-1] = '\0';
		rt_tbl_idx.ip = iptype;
		if(0 != ioctl(m_fd_ipa, IPA_IOC_QUERY_RT_TBL_INDEX, &rt_tbl_idx))
		{
			IPACMERR("Failed to get routing table index from name\n");
			res = IPACM_FAILURE;
			goto fail;
		}

		IPACMDBG_H("Routing table %s has index %d\n", rt_tbl_idx.name, rt_tbl_idx.idx);

		memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

		flt_rule_entry.at_rear = true;
		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;

		flt_rule_entry.rule.retain_hdr = 1;
		flt_rule_entry.rule.to_uc = 0;
		flt_rule_entry.rule.eq_attrib_type = 1;
		flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;

		IPACMDBG_H("rx property attrib mask:0x%x\n", rx_prop->rx[0].attrib.attrib_mask);

		if (ipacmcfg->IsIpv6CTEnabled())
		{
			/* Configuring Fragment Filtering Rule */
#ifdef FEATURE_IPA_V3
			flt_rule_entry.rule.hashable = false;
#endif
			memcpy(&flt_rule_entry.rule.attrib, &rx_prop->rx[0].attrib, sizeof(flt_rule_entry.rule.attrib));
			/* remove meta data mask since we only install default flt rules once for all modem PDN*/
			flt_rule_entry.rule.attrib.attrib_mask &= ~((uint32_t)IPA_FLT_META_DATA);
			flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_FRAGMENT;

			change_to_network_order(IPA_IP_v6, &flt_rule_entry.rule.attrib);

			memset(&flt_eq, 0, sizeof(flt_eq));
			memcpy(&flt_eq.attrib, &flt_rule_entry.rule.attrib, sizeof(flt_eq.attrib));
			flt_eq.ip = iptype;
			if (ioctl(m_fd_ipa, IPA_IOC_GENERATE_FLT_EQ, &flt_eq))
			{
				IPACMERR("Failed to get eq_attrib\n");
				res = IPACM_FAILURE;
				goto fail;
			}

			memcpy(&flt_rule_entry.rule.eq_attrib, &flt_eq.eq_attrib, sizeof(flt_rule_entry.rule.eq_attrib));
#ifdef FEATURE_VLAN_MPDN
			/* default rules are not metadata dependant - mux_id is not relevant */
			rules[rule_offset + m_ipv6_default_filterting_rules_count].mux_id = 0;
			memcpy(&(rules[rule_offset + m_ipv6_default_filterting_rules_count++].flt_rule),
				&flt_rule_entry, sizeof(struct ipa_flt_rule_add));
#else
			memcpy(&(rules[rule_offset + m_ipv6_default_filterting_rules_count++]),
				&flt_rule_entry, sizeof(struct ipa_flt_rule_add));
#endif
		}

#ifdef FEATURE_IPA_V3
		flt_rule_entry.rule.hashable = true;
#endif
		flt_rule_entry.rule.rt_tbl_idx = rt_tbl_idx.idx;

		/* Configuring Multicast Filtering Rule */
		memcpy(&flt_rule_entry.rule.attrib,
					 &rx_prop->rx[0].attrib,
					 sizeof(flt_rule_entry.rule.attrib));
		/* remove meta data mask since we only install default flt rules once for all modem PDN*/
		flt_rule_entry.rule.attrib.attrib_mask &= ~((uint32_t)IPA_FLT_META_DATA);
		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[0] = 0xFF000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[1] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[2] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[3] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[0] = 0xFF000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[1] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[2] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[3] = 0x00000000;

		change_to_network_order(IPA_IP_v6, &flt_rule_entry.rule.attrib);

		memset(&flt_eq, 0, sizeof(flt_eq));
		memcpy(&flt_eq.attrib, &flt_rule_entry.rule.attrib, sizeof(flt_eq.attrib));
		flt_eq.ip = iptype;
		if(0 != ioctl(m_fd_ipa, IPA_IOC_GENERATE_FLT_EQ, &flt_eq))
		{
			IPACMERR("Failed to get eq_attrib\n");
			res = IPACM_FAILURE;
			goto fail;
		}

		memcpy(&flt_rule_entry.rule.eq_attrib,
					 &flt_eq.eq_attrib,
					 sizeof(flt_rule_entry.rule.eq_attrib));
#ifdef FEATURE_VLAN_MPDN
		/* default rules are not metadata dependant - mux_id is not relevant */
		rules[rule_offset + m_ipv6_default_filterting_rules_count].mux_id = 0;
		memcpy(&(rules[rule_offset + m_ipv6_default_filterting_rules_count++].flt_rule),
			&flt_rule_entry, sizeof(struct ipa_flt_rule_add));
#else
		memcpy(&(rules[rule_offset + m_ipv6_default_filterting_rules_count++]),
			&flt_rule_entry, sizeof(struct ipa_flt_rule_add));
#endif

		/* Configuring fe80::/10 Link-Scoped Unicast Filtering Rule */
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[0] = 0xFFC00000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[1] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[2] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[3] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[0] = 0xFE800000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[1] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[2] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[3] = 0x00000000;

		change_to_network_order(IPA_IP_v6, &flt_rule_entry.rule.attrib);

		memset(&flt_eq, 0, sizeof(flt_eq));
		memcpy(&flt_eq.attrib, &flt_rule_entry.rule.attrib, sizeof(flt_eq.attrib));
		flt_eq.ip = iptype;
		if(0 != ioctl(m_fd_ipa, IPA_IOC_GENERATE_FLT_EQ, &flt_eq))
		{
			IPACMERR("Failed to get eq_attrib\n");
			res = IPACM_FAILURE;
			goto fail;
		}

		memcpy(&flt_rule_entry.rule.eq_attrib,
					 &flt_eq.eq_attrib,
					 sizeof(flt_rule_entry.rule.eq_attrib));

#ifdef FEATURE_VLAN_MPDN
		/* default rules are not metadata dependant - mux_id is not relevant */
		rules[rule_offset + m_ipv6_default_filterting_rules_count].mux_id = 0;
		memcpy(&(rules[rule_offset + m_ipv6_default_filterting_rules_count++].flt_rule),
			&flt_rule_entry, sizeof(struct ipa_flt_rule_add));
#else
		memcpy(&(rules[rule_offset + m_ipv6_default_filterting_rules_count++]),
			&flt_rule_entry, sizeof(struct ipa_flt_rule_add));
#endif

		/* Configuring fec0::/10 Reserved by IETF Filtering Rule */
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[0] = 0xFFC00000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[1] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[2] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[3] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[0] = 0xFEC00000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[1] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[2] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[3] = 0x00000000;

		change_to_network_order(IPA_IP_v6, &flt_rule_entry.rule.attrib);

		memset(&flt_eq, 0, sizeof(flt_eq));
		memcpy(&flt_eq.attrib, &flt_rule_entry.rule.attrib, sizeof(flt_eq.attrib));
		flt_eq.ip = iptype;
		if(0 != ioctl(m_fd_ipa, IPA_IOC_GENERATE_FLT_EQ, &flt_eq))
		{
			IPACMERR("Failed to get eq_attrib\n");
			res = IPACM_FAILURE;
			goto fail;
		}

		memcpy(&flt_rule_entry.rule.eq_attrib,
					 &flt_eq.eq_attrib,
					 sizeof(flt_rule_entry.rule.eq_attrib));

#ifdef FEATURE_VLAN_MPDN
		/* default rules are not metadata dependant - mux_id is not relevant */
		rules[rule_offset + m_ipv6_default_filterting_rules_count].mux_id = 0;
		memcpy(&(rules[rule_offset + m_ipv6_default_filterting_rules_count++].flt_rule),
			&flt_rule_entry, sizeof(struct ipa_flt_rule_add));
#else
		memcpy(&(rules[rule_offset + m_ipv6_default_filterting_rules_count++]),
			&flt_rule_entry, sizeof(struct ipa_flt_rule_add));
#endif

#ifdef FEATURE_IPA_ANDROID
		IPACMDBG_H("Add TCP ctrl rules\n");
		memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

		flt_rule_entry.at_rear = true;
		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;

		flt_rule_entry.rule.retain_hdr = 1;
		flt_rule_entry.rule.to_uc = 0;
		flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
		flt_rule_entry.rule.rt_tbl_idx = rt_tbl_idx.idx;
		flt_rule_entry.rule.eq_attrib_type = 1;

		flt_rule_entry.rule.eq_attrib.rule_eq_bitmap = 0;

		flt_rule_entry.rule.eq_attrib.rule_eq_bitmap |= (1<<1);
		flt_rule_entry.rule.eq_attrib.protocol_eq_present = 1;
		flt_rule_entry.rule.eq_attrib.protocol_eq = IPACM_FIREWALL_IPPROTO_TCP;

		flt_rule_entry.rule.eq_attrib.rule_eq_bitmap |= (1<<8);
		flt_rule_entry.rule.eq_attrib.num_ihl_offset_meq_32 = 1;
		flt_rule_entry.rule.eq_attrib.ihl_offset_meq_32[0].offset = 12;

		/* add TCP FIN rule*/
		flt_rule_entry.rule.eq_attrib.ihl_offset_meq_32[0].value = (((uint32_t)1)<<TCP_FIN_SHIFT);
		flt_rule_entry.rule.eq_attrib.ihl_offset_meq_32[0].mask = (((uint32_t)1)<<TCP_FIN_SHIFT);
		memcpy(&(rules[rule_offset + m_ipv6_default_filterting_rules_count++]),
			&flt_rule_entry, sizeof(struct ipa_flt_rule_add));

		/* add TCP SYN rule*/
		flt_rule_entry.rule.eq_attrib.ihl_offset_meq_32[0].value = (((uint32_t)1)<<TCP_SYN_SHIFT);
		flt_rule_entry.rule.eq_attrib.ihl_offset_meq_32[0].mask = (((uint32_t)1)<<TCP_SYN_SHIFT);
		memcpy(&(rules[rule_offset + m_ipv6_default_filterting_rules_count++]),
			&flt_rule_entry, sizeof(struct ipa_flt_rule_add));

		/* add TCP RST rule*/
		flt_rule_entry.rule.eq_attrib.ihl_offset_meq_32[0].value = (((uint32_t)1)<<TCP_RST_SHIFT);
		flt_rule_entry.rule.eq_attrib.ihl_offset_meq_32[0].mask = (((uint32_t)1)<<TCP_RST_SHIFT);
		memcpy(&(rules[rule_offset + m_ipv6_default_filterting_rules_count++]),
			&flt_rule_entry, sizeof(struct ipa_flt_rule_add));
#endif

		IPACM_Wan::num_v6_flt_rule += m_ipv6_default_filterting_rules_count;
#ifdef FEATURE_VLAN_MPDN
		/**
		 * store the default filtering rules count since on each WAN IFACE construction
		 * this variable is zeroed, but it is incremented only at this init function which is called with the first pdn
		 */
		IPACM_Wan::ipv6_mpdn_default_filterting_rules_count = m_ipv6_default_filterting_rules_count;
#endif
		IPACMDBG_H("Constructed %d default filtering rules for ip type %d\n",
			m_ipv6_default_filterting_rules_count, iptype);
	}

fail:
	return res;
}

int IPACM_Wan::del_wan_firewall_rule(ipa_ip_type iptype)
{
	list<l2tp_client_info>::iterator it;
	if(iptype == IPA_IP_v4)
	{
		IPACM_Wan::num_v4_flt_rule = IPA_V2_NUM_DEFAULT_WAN_FILTER_RULE_IPV4;
#ifdef FEATURE_VLAN_MPDN
		memset(&IPACM_Wan::pdn_flt_rule_v4[IPA_V2_NUM_DEFAULT_WAN_FILTER_RULE_IPV4], 0,
			(IPA_MAX_FLT_RULE - IPA_V2_NUM_DEFAULT_WAN_FILTER_RULE_IPV4) * sizeof(struct ipacm_pdn_flt_rule));
#else
		memset(&IPACM_Wan::flt_rule_v4[IPA_V2_NUM_DEFAULT_WAN_FILTER_RULE_IPV4], 0,
			(IPA_MAX_FLT_RULE - IPA_V2_NUM_DEFAULT_WAN_FILTER_RULE_IPV4) * sizeof(struct ipa_flt_rule_add));
#endif
#ifdef FEATURE_L2TP_E2E
		for(it = IPACM_Iface::ipacmcfg->l2tp_client.begin(); it != IPACM_Iface::ipacmcfg->l2tp_client.end(); it++)
		{
			handle_l2tp_client_del(it->client_iface_name);
		}
#endif
	}
	else if(iptype == IPA_IP_v6)
	{
#ifdef FEATURE_VLAN_MPDN
		IPACM_Wan::num_v6_flt_rule = IPACM_Wan::ipv6_mpdn_default_filterting_rules_count;
#else
		IPACM_Wan::num_v6_flt_rule = m_ipv6_default_filterting_rules_count;
#endif
#ifdef FEATURE_L2TP_E2E
		if(active_v4)
		{
			IPACM_Wan::num_v6_flt_rule += IPACM_Iface::ipacmcfg->l2tp_client.size();
		}
#endif
#ifdef FEATURE_VLAN_MPDN
		memset(&IPACM_Wan::pdn_flt_rule_v6[IPACM_Wan::num_v6_flt_rule], 0,
			(IPA_MAX_FLT_RULE - IPACM_Wan::num_v6_flt_rule) * sizeof(struct ipacm_pdn_flt_rule));
#else
		memset(&IPACM_Wan::flt_rule_v6[IPACM_Wan::num_v6_flt_rule], 0,
			(IPA_MAX_FLT_RULE - IPACM_Wan::num_v6_flt_rule) * sizeof(struct ipa_flt_rule_add));
#endif
	}
	else
	{
		IPACMERR("IP type is not expected.\n");
		return IPACM_FAILURE;
	}

	return IPACM_SUCCESS;
}

/*for STA mode: clean firewall filter rules */
int IPACM_Wan::del_dft_firewall_rules(ipa_ip_type iptype)
{
	/* free v4 firewall filter rule */
	if (rx_prop == NULL)
	{
		IPACMDBG_H("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	if ((iptype == IPA_IP_v4) && (active_v4 == true))
	{
		if (num_firewall_v4 > IPACM_MAX_FIREWALL_ENTRIES)
		{
			IPACMERR("the number of v4 firewall entries overflow, aborting...\n");
			return IPACM_FAILURE;
		}
		if (num_firewall_v4 != 0)
		{
			if (m_filtering.DeleteFilteringHdls(firewall_hdl_v4,
																					IPA_IP_v4, num_firewall_v4) == false)
			{
				IPACMERR("Error Deleting Filtering rules, aborting...\n");
				return IPACM_FAILURE;
			}
			IPACM_Iface::ipacmcfg->decreaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v4, num_firewall_v4);
		}
		else
		{
			IPACMDBG_H("No ipv4 firewall rules, no need deleted\n");
		}

		if (m_filtering.DeleteFilteringHdls(dft_wan_fl_hdl,
																				IPA_IP_v4, 1) == false)
		{
			IPACMERR("Error Deleting Filtering rules, aborting...\n");
			return IPACM_FAILURE;
		}
		IPACM_Iface::ipacmcfg->decreaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v4, 1);

		num_firewall_v4 = 0;
	}

	/* free v6 firewall filter rule */
	if ((iptype == IPA_IP_v6) && (active_v6 == true))
	{
		if (num_firewall_v6 > IPACM_MAX_FIREWALL_ENTRIES)
		{
			IPACMERR("the number of v6 firewall entries overflow, aborting...\n");
			return IPACM_FAILURE;
		}
		if (num_firewall_v6 != 0)
		{
			if (m_filtering.DeleteFilteringHdls(firewall_hdl_v6,
																					IPA_IP_v6, num_firewall_v6) == false)
			{
				IPACMERR("Error Deleting Filtering rules, aborting...\n");
				return IPACM_FAILURE;
			}
			IPACM_Iface::ipacmcfg->decreaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, num_firewall_v6);
		}
		else
		{
			IPACMDBG_H("No ipv6 firewall rules, no need deleted\n");
		}

		if (m_filtering.DeleteFilteringHdls(&dft_wan_fl_hdl[1],
																				IPA_IP_v6, 1) == false)
		{
			IPACMERR("Error Deleting Filtering rules, aborting...\n");
			return IPACM_FAILURE;
		}
		IPACM_Iface::ipacmcfg->decreaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, 1);
		if (m_filtering.DeleteFilteringHdls(&dft_wan_fl_hdl[2],
																				IPA_IP_v6, 1) == false)
		{
			IPACMERR("Error Deleting Filtering rules, aborting...\n");
			return IPACM_FAILURE;
		}
		IPACM_Iface::ipacmcfg->decreaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, 1);

		if (is_ipv6_frag_firewall_flt_rule_installed)
		{
			if (m_filtering.DeleteFilteringHdls(&ipv6_frag_firewall_flt_rule_hdl, IPA_IP_v6, 1) == false)
			{
				IPACMERR("Error deleting IPv6 frag filtering rules.\n");
				return IPACM_FAILURE;
			}
			is_ipv6_frag_firewall_flt_rule_installed = false;
			IPACM_Iface::ipacmcfg->decreaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, 1);
		}
		num_firewall_v6 = 0;
	}

	return IPACM_SUCCESS;
}

/* for STA mode: wan default route/filter rule delete */
int IPACM_Wan::handle_route_del_evt(ipa_ip_type iptype)
{
	uint32_t tx_index;
	ipacm_cmd_q_data evt_data;

	IPACMDBG_H("got handle_route_del_evt for STA-mode with ip-family:%d \n", iptype);

	if(tx_prop == NULL)
	{
		IPACMDBG_H("No tx properties, ignore delete default route setting\n");
		return IPACM_SUCCESS;
	}
	is_default_gateway = false;
	IPACMDBG_H("Default route is deleted to iface %s.\n", dev_name);

	if (((iptype == IPA_IP_v4) && (active_v4 == true)) ||
			((iptype == IPA_IP_v6) && (active_v6 == true)))
	{
		if(IPACM_Iface::ipacmcfg->GetIPAVer() >= IPA_HW_None && IPACM_Iface::ipacmcfg->GetIPAVer() < IPA_HW_v4_0)
		{
			/* Delete corresponding ipa_rm_resource_name of TX-endpoint after delete IPV4/V6 RT-rule */
			IPACMDBG_H("dev %s delete producer dependency\n", dev_name);
			IPACMDBG_H("depend Got pipe %d rm index : %d \n", tx_prop->tx[0].dst_pipe, IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
			IPACM_Iface::ipacmcfg->DelRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
		}
		for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
		{
		    if(iptype != tx_prop->tx[tx_index].ip)
		    {
		    	IPACMDBG_H("Tx:%d, ip-type: %d conflict ip-type: %d, no RT-rule deleted\n",
		    					    tx_index, tx_prop->tx[tx_index].ip,iptype);
		    	continue;
		    }

			if (iptype == IPA_IP_v4)
			{
		    	IPACMDBG_H("Tx:%d, ip-type: %d match ip-type: %d, RT-rule deleted\n", tx_index, tx_prop->tx[tx_index].ip,iptype);

				if (m_routing.DeleteRoutingHdl(wan_route_rule_v4_hdl[tx_index], IPA_IP_v4) == false)
				{
					IPACMDBG_H("IP-family:%d, Routing rule(hdl:0x%x) deletion failed with tx_index %d!\n", IPA_IP_v4, wan_route_rule_v4_hdl[tx_index], tx_index);
					return IPACM_FAILURE;
				}
			}
			else
			{
		    	IPACMDBG_H("Tx:%d, ip-type: %d match ip-type: %d, RT-rule deleted\n", tx_index, tx_prop->tx[tx_index].ip,iptype);

				if (m_routing.DeleteRoutingHdl(wan_route_rule_v6_hdl[tx_index], IPA_IP_v6) == false)
				{
					IPACMDBG_H("IP-family:%d, Routing rule(hdl:0x%x) deletion failed with tx_index %d!\n", IPA_IP_v6, wan_route_rule_v6_hdl[tx_index], tx_index);
					return IPACM_FAILURE;
				}
			}
		}

		/* Delete the default wan route*/
		if (iptype == IPA_IP_v6)
		{
		   	IPACMDBG_H("ip-type %d: default v6 wan RT-rule deleted\n",iptype);
			if (m_routing.DeleteRoutingHdl(wan_route_rule_v6_hdl_a5, IPA_IP_v6) == false)
			{
			IPACMDBG_H("IP-family:%d, Routing rule(hdl:0x%x) deletion failed!\n",IPA_IP_v6,wan_route_rule_v6_hdl_a5);
				return IPACM_FAILURE;
			}
		}
		ipacm_event_iface_up *wandown_data;
		wandown_data = (ipacm_event_iface_up *)malloc(sizeof(ipacm_event_iface_up));
		if (wandown_data == NULL)
		{
			IPACMERR("Unable to allocate memory\n");
			return IPACM_FAILURE;
		}
		memset(wandown_data, 0, sizeof(ipacm_event_iface_up));

		if (iptype == IPA_IP_v4)
		{
			wandown_data->ipv4_addr = wan_v4_addr;
			if (m_is_sta_mode!=Q6_WAN)
			{
				wandown_data->is_sta = true;
			}
			else
			{
				wandown_data->is_sta = false;
			}
			evt_data.event = IPA_HANDLE_WAN_DOWN;
			evt_data.evt_data = (void *)wandown_data;
			/* Insert IPA_HANDLE_WAN_DOWN to command queue */
			IPACMDBG_H("posting IPA_HANDLE_WAN_DOWN for IPv4 (%d.%d.%d.%d) \n",
					(unsigned char)(wandown_data->ipv4_addr),
					(unsigned char)(wandown_data->ipv4_addr >> 8),
					(unsigned char)(wandown_data->ipv4_addr >> 16),
					(unsigned char)(wandown_data->ipv4_addr >> 24));

			IPACM_EvtDispatcher::PostEvt(&evt_data);
			IPACMDBG_H("setup wan_up/active_v4= false \n");
			IPACM_Wan::wan_up = false;
			active_v4 = false;
			if(IPACM_Wan::wan_up_v6)
			{
				IPACMDBG_H("modem v6-call still up(%s), not reset\n", IPACM_Wan::wan_up_dev_name);
			}
			else
			{
				memset(IPACM_Wan::wan_up_dev_name, 0, sizeof(IPACM_Wan::wan_up_dev_name));
			}
		}
		else
		{
			if (m_is_sta_mode!=Q6_WAN)
			{
				wandown_data->is_sta = true;
			}
			else
			{
				wandown_data->is_sta = false;
			}
			memcpy(wandown_data->ipv6_prefix, ipv6_prefix, sizeof(wandown_data->ipv6_prefix));
			evt_data.event = IPA_HANDLE_WAN_DOWN_V6;
			evt_data.evt_data = (void *)wandown_data;
			/* Insert IPA_HANDLE_WAN_DOWN to command queue */
			IPACMDBG_H("posting IPA_HANDLE_WAN_DOWN for IPv6 with prefix 0x%08x%08x\n", ipv6_prefix[0], ipv6_prefix[1]);
			IPACM_EvtDispatcher::PostEvt(&evt_data);
			IPACMDBG_H("setup wan_up_v6/active_v6= false \n");
			IPACM_Wan::wan_up_v6 = false;
			active_v6 = false;
			if(IPACM_Wan::wan_up)
			{
				IPACMDBG_H("modem v4-call still up(%s), not reset\n", IPACM_Wan::wan_up_dev_name);
			}
			else
			{
				memset(IPACM_Wan::wan_up_dev_name, 0, sizeof(IPACM_Wan::wan_up_dev_name));
			}
		}
	}
	else
	{
		IPACMDBG_H(" The default WAN routing rules are deleted already \n");
	}

	return IPACM_SUCCESS;
}

int IPACM_Wan::handle_route_del_evt_ex(ipa_ip_type iptype)
{
	ipacm_cmd_q_data evt_data;
	struct wan_ioctl_notify_wan_state wan_state;
	int fd_wwan_ioctl;
	memset(&wan_state, 0, sizeof(wan_state));

	IPACMDBG_H("got handle_route_del_evt_ex with ip-family:%d \n", iptype);

	if(tx_prop == NULL)
	{
		IPACMDBG_H("No tx properties, ignore delete default route setting\n");
		return IPACM_SUCCESS;
	}

	IPACMDBG_H("Default route is deleted to iface %s.\n", dev_name);

	if (((iptype == IPA_IP_v4) && (active_v4 == true)) ||
		((iptype == IPA_IP_v6) && (active_v6 == true)))
	{
#ifdef FEATURE_VLAN_MPDN
		if( !isVlanWanUP() && !isVlanWanUP_V6() )
#endif
	{
		if(IPACM_Iface::ipacmcfg->GetIPAVer() >= IPA_HW_None && IPACM_Iface::ipacmcfg->GetIPAVer() < IPA_HW_v4_0)
		{
			/* Delete corresponding ipa_rm_resource_name of TX-endpoint after delete IPV4/V6 RT-rule */
			IPACMDBG_H("dev %s delete producer dependency\n", dev_name);
			IPACMDBG_H("depend Got pipe %d rm index : %d \n", tx_prop->tx[0].dst_pipe, IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
			IPACM_Iface::ipacmcfg->DelRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
		} else {
			IPACMDBG_H("ipa_pm_q6_check to %d\n", ipa_pm_q6_check);
			if(ipa_pm_q6_check == 1)
			{
				fd_wwan_ioctl = open(WWAN_QMI_IOCTL_DEVICE_NAME, O_RDWR);
				if(fd_wwan_ioctl < 0)
				{
					IPACMERR("Failed to open %s.\n",WWAN_QMI_IOCTL_DEVICE_NAME);
					return false;
				}
				IPACMDBG_H("send WAN_IOC_NOTIFY_WAN_STATE down to IPA_PM\n");
				if(ioctl(fd_wwan_ioctl, WAN_IOC_NOTIFY_WAN_STATE, &wan_state))
				{
					IPACMERR("Failed to send WAN_IOC_NOTIFY_WAN_STATE as up %d\n ", wan_state.up);
				}
				close(fd_wwan_ioctl);
			}
			if (ipa_pm_q6_check > 0)
				ipa_pm_q6_check--;
			else
				IPACMERR(" ipa_pm_q6_check becomes negative !!!\n");
		}
	}
#ifdef FEATURE_VLAN_MPDN
	else
	{
		IPACMDBG_H("not deleting rm depend for default rt, a VLAN PDN is still up, iptype %d\n", iptype);
	}
#endif
		/* Delete the default route*/
		if (iptype == IPA_IP_v6)
		{
#ifdef FEATURE_VLAN_MPDN
			if(!isVlanWanUP_V6())
#endif
			{
				IPACMDBG_H("ip-type %d: default v6 wan RT-rule deleted\n",iptype);
				if (m_routing.DeleteRoutingHdl(wan_route_rule_v6_hdl_a5, IPA_IP_v6) == false)
				{
					IPACMDBG_H("IP-family:%d, Routing rule(hdl:0x%x) deletion failed!\n",IPA_IP_v6,wan_route_rule_v6_hdl_a5);
					return IPACM_FAILURE;
				}
			}
#ifdef FEATURE_VLAN_MPDN
			else
			{
				IPACMDBG_H("not deleting default v6 RT rule, vlan v6 PDN is up\n");
			}
#endif
		}
		ipacm_event_iface_up *wandown_data;
		wandown_data = (ipacm_event_iface_up *)malloc(sizeof(ipacm_event_iface_up));
		if (wandown_data == NULL)
		{
			IPACMERR("Unable to allocate memory\n");
			return IPACM_FAILURE;
		}
		memset(wandown_data, 0, sizeof(ipacm_event_iface_up));

		if (iptype == IPA_IP_v4)
		{
			if (public_wan_v4_addr_set)
				wandown_data->ipv4_addr = public_wan_v4_addr;
			else
				wandown_data->ipv4_addr = wan_v4_addr;
			if (m_is_sta_mode!=Q6_WAN)
			{
				wandown_data->is_sta = true;
			}
			else
			{
				wandown_data->is_sta = false;
			}
			evt_data.event = IPA_HANDLE_WAN_DOWN;
			evt_data.evt_data = (void *)wandown_data;
			/* Insert IPA_HANDLE_WAN_DOWN to command queue */
			IPACMDBG_H("posting IPA_HANDLE_WAN_DOWN for IPv4 with address: 0x%x\n", wan_v4_addr);
			IPACM_EvtDispatcher::PostEvt(&evt_data);

			IPACMDBG_H("setup wan_up/active_v4= false \n");
			IPACM_Wan::wan_up = false;
			active_v4 = false;
			if(IPACM_Wan::wan_up_v6)
			{
				IPACMDBG_H("modem v6-call still up(%s), not reset\n", IPACM_Wan::wan_up_dev_name);
			}
			else
			{
				memset(IPACM_Wan::wan_up_dev_name, 0, sizeof(IPACM_Wan::wan_up_dev_name));
				is_default_gateway = false;
			}
		}
		else
		{
			if (m_is_sta_mode!=Q6_WAN)
			{
				wandown_data->is_sta = true;
			}
			else
			{
				wandown_data->is_sta = false;
			}
			memcpy(wandown_data->ipv6_prefix, ipv6_prefix, sizeof(wandown_data->ipv6_prefix));
#ifdef FEATURE_VLAN_MPDN
			IPACM_Iface::ipacmcfg->del_vlan_ipv6_prefix(ipv6_prefix, -1);
#endif
			evt_data.event = IPA_HANDLE_WAN_DOWN_V6;
			evt_data.evt_data = (void *)wandown_data;
			IPACMDBG_H("posting IPA_HANDLE_WAN_DOWN_V6 for IPv6 with prefix 0x%08x%08x\n", ipv6_prefix[0], ipv6_prefix[1]);
			IPACM_EvtDispatcher::PostEvt(&evt_data);

			IPACMDBG_H("setup wan_up_v6/active_v6= false \n");
			IPACM_Wan::wan_up_v6 = false;
			active_v6 = false;
			if(IPACM_Wan::wan_up)
			{
				IPACMDBG_H("modem v4-call still up(%s), not reset\n", IPACM_Wan::wan_up_dev_name);
			}
			else
			{
				memset(IPACM_Wan::wan_up_dev_name, 0, sizeof(IPACM_Wan::wan_up_dev_name));
				is_default_gateway = false;
			}
		}
	}
	else
	{
		IPACMDBG_H(" The default WAN routing rules are deleted already \n");
	}

	return IPACM_SUCCESS;
}

/* configure the initial embms filter rules */
int IPACM_Wan::config_dft_embms_rules(ipa_ioc_add_flt_rule *pFilteringTable_v4, ipa_ioc_add_flt_rule *pFilteringTable_v6)
{
	struct ipa_flt_rule_add flt_rule_entry;
	struct ipa_ioc_get_rt_tbl_indx rt_tbl_idx;
	struct ipa_ioc_generate_flt_eq flt_eq;

	if (rx_prop == NULL)
	{
		IPACMDBG("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	if(pFilteringTable_v4 == NULL || pFilteringTable_v6 == NULL)
	{
		IPACMERR("Either v4 or v6 filtering table is empty.\n");
		return IPACM_FAILURE;
	}

	/* set up ipv4 odu rule*/
	memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

	/* get eMBMS ODU tbl index*/
	memset(&rt_tbl_idx, 0, sizeof(rt_tbl_idx));
	strlcpy(rt_tbl_idx.name, IPACM_Iface::ipacmcfg->rt_tbl_odu_v4.name, IPA_RESOURCE_NAME_MAX);
	rt_tbl_idx.name[IPA_RESOURCE_NAME_MAX-1] = '\0';
	rt_tbl_idx.ip = IPA_IP_v4;
	if(0 != ioctl(m_fd_ipa, IPA_IOC_QUERY_RT_TBL_INDEX, &rt_tbl_idx))
	{
		IPACMERR("Failed to get routing table index from name\n");
		return IPACM_FAILURE;
	}
	IPACMDBG_H("Odu routing table %s has index %d\n", rt_tbl_idx.name, rt_tbl_idx.idx);

	memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
	flt_rule_entry.flt_rule_hdl = -1;
	flt_rule_entry.status = -1;
	flt_rule_entry.at_rear = false;

	flt_rule_entry.rule.retain_hdr = 0;
	flt_rule_entry.rule.to_uc = 0;
	flt_rule_entry.rule.eq_attrib_type = 1;
	flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
#ifdef FEATURE_IPA_V3
	flt_rule_entry.rule.hashable = true;
#endif
	flt_rule_entry.rule.rt_tbl_idx = rt_tbl_idx.idx;

	memcpy(&flt_rule_entry.rule.attrib,
				 &rx_prop->rx[0].attrib,
				 sizeof(struct ipa_rule_attrib));
	flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
	flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = 0x00000000;
	flt_rule_entry.rule.attrib.u.v4.dst_addr = 0x00000000;

	memset(&flt_eq, 0, sizeof(flt_eq));
	memcpy(&flt_eq.attrib, &flt_rule_entry.rule.attrib, sizeof(flt_eq.attrib));
	flt_eq.ip = IPA_IP_v4;
	if(0 != ioctl(m_fd_ipa, IPA_IOC_GENERATE_FLT_EQ, &flt_eq))
	{
		IPACMERR("Failed to get eq_attrib\n");
		return IPACM_FAILURE;
	}
	memcpy(&flt_rule_entry.rule.eq_attrib,
				 &flt_eq.eq_attrib,
				 sizeof(flt_rule_entry.rule.eq_attrib));

	memcpy(&(pFilteringTable_v4->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

	/* construc v6 rule */
	memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
	/* get eMBMS ODU tbl*/
	memset(&rt_tbl_idx, 0, sizeof(rt_tbl_idx));
	strlcpy(rt_tbl_idx.name, IPACM_Iface::ipacmcfg->rt_tbl_odu_v6.name, IPA_RESOURCE_NAME_MAX);
	rt_tbl_idx.name[IPA_RESOURCE_NAME_MAX-1] = '\0';
	rt_tbl_idx.ip = IPA_IP_v6;
	if(0 != ioctl(m_fd_ipa, IPA_IOC_QUERY_RT_TBL_INDEX, &rt_tbl_idx))
	{
		IPACMERR("Failed to get routing table index from name\n");
		return IPACM_FAILURE;
	}
	IPACMDBG_H("Odu routing table %s has index %d\n", rt_tbl_idx.name, rt_tbl_idx.idx);

	memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
	flt_rule_entry.flt_rule_hdl = -1;
	flt_rule_entry.status = -1;
	flt_rule_entry.at_rear = false;

	flt_rule_entry.rule.retain_hdr = 0;
	flt_rule_entry.rule.to_uc = 0;
	flt_rule_entry.rule.eq_attrib_type = 1;
	flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
#ifdef FEATURE_IPA_V3
	flt_rule_entry.rule.hashable = true;
#endif
	flt_rule_entry.rule.rt_tbl_idx = rt_tbl_idx.idx;

	memcpy(&flt_rule_entry.rule.attrib,
				 &rx_prop->rx[0].attrib,
				 sizeof(struct ipa_rule_attrib));
	flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
	flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[0] = 0x00000000;
	flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[1] = 0x00000000;
	flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[2] = 0x00000000;
	flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[3] = 0x00000000;
	flt_rule_entry.rule.attrib.u.v6.dst_addr[0] = 0X00000000;
	flt_rule_entry.rule.attrib.u.v6.dst_addr[1] = 0x00000000;
	flt_rule_entry.rule.attrib.u.v6.dst_addr[2] = 0x00000000;
	flt_rule_entry.rule.attrib.u.v6.dst_addr[3] = 0X00000000;

	memset(&flt_eq, 0, sizeof(flt_eq));
	memcpy(&flt_eq.attrib, &flt_rule_entry.rule.attrib, sizeof(flt_eq.attrib));
	flt_eq.ip = IPA_IP_v6;
	if(0 != ioctl(m_fd_ipa, IPA_IOC_GENERATE_FLT_EQ, &flt_eq))
	{
		IPACMERR("Failed to get eq_attrib\n");
		return IPACM_FAILURE;
	}
	memcpy(&flt_rule_entry.rule.eq_attrib,
				 &flt_eq.eq_attrib,
				 sizeof(flt_rule_entry.rule.eq_attrib));

	memcpy(&(pFilteringTable_v6->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

	return IPACM_SUCCESS;
}


/*for STA mode: handle wan-iface down event */
int IPACM_Wan::handle_down_evt()
{
	int res = IPACM_SUCCESS;
	int i;

	IPACMDBG_H(" wan handle_down_evt \n");
	if(IPACM_Iface::ipacmcfg->GetIPAVer() >= IPA_HW_None && IPACM_Iface::ipacmcfg->GetIPAVer() < IPA_HW_v4_0)
	{
		/* Delete corresponding ipa_rm_resource_name of TX-endpoint after delete IPV4/V6 RT-rule */
		IPACMDBG_H("dev %s delete producer dependency\n", dev_name);
		if (tx_prop != NULL)
		{
			IPACMDBG_H("depend Got pipe %d rm index : %d \n", tx_prop->tx[0].dst_pipe, IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
			IPACM_Iface::ipacmcfg->DelRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
		}
	}
	/* no iface address up, directly close iface*/
	if (ip_type == IPACM_IP_NULL)
	{
		goto fail;
	}

	/* make sure default routing rules and firewall rules are deleted*/
	if (active_v4)
	{
	   	if (rx_prop != NULL)
	    {
			del_dft_firewall_rules(IPA_IP_v4);
		}
		handle_route_del_evt(IPA_IP_v4);
		IPACMDBG_H("Delete default v4 routing rules\n");
	}

	if (active_v6)
	{
	   	if (rx_prop != NULL)
	    {
			del_dft_firewall_rules(IPA_IP_v6);
		}
		handle_route_del_evt(IPA_IP_v6);
		IPACMDBG_H("Delete default v6 routing rules\n");
	}

	/* Delete default v4 RT rule */
	if (ip_type != IPA_IP_v6)
	{
		IPACMDBG_H("Delete default v4 routing rules\n");
		if (m_routing.DeleteRoutingHdl(dft_rt_rule_hdl[0], IPA_IP_v4) == false)
		{
		   IPACMERR("Routing rule deletion failed!\n");
			res = IPACM_FAILURE;
			goto fail;
		}
	}

	/* delete default v6 RT rule */
	if (ip_type != IPA_IP_v4)
	{
		IPACMDBG_H("Delete default v6 routing rules\n");
		/* May have multiple ipv6 iface-routing rules*/
		for (i = 0; i < 2*num_dft_rt_v6; i++)
		{
			if (m_routing.DeleteRoutingHdl(dft_rt_rule_hdl[MAX_DEFAULT_v4_ROUTE_RULES+i], IPA_IP_v6) == false)
			{
				IPACMERR("Routing rule deletion failed!\n");
				res = IPACM_FAILURE;
				goto fail;
			}
		}

		IPACMDBG_H("finished delete default v6 RT rules\n ");
	}


	/* clean wan-client header, routing rules */
	IPACMDBG_H("left %d wan clients need to be deleted \n ", num_wan_client);
	for (i = 0; i < num_wan_client; i++)
	{
		/* Del NAT rules before RT rules are delete */
		HandleSTAClientDelEvt(get_client_memptr(wan_client, i));

		if (delete_wan_rtrules(i, IPA_IP_v4))
		{
			IPACMERR("unbale to delete wan-client v4 route rules for index %d\n", i);
			res = IPACM_FAILURE;
			goto fail;
		}

		if (delete_wan_rtrules(i, IPA_IP_v6))
		{
			IPACMERR("unbale to delete ecm-client v6 route rules for index %d\n", i);
			res = IPACM_FAILURE;
			goto fail;
		}

		IPACMDBG_H("Delete %d client header\n", num_wan_client);


		if (get_client_memptr(wan_client, i)->ipv4_header_set == true)
		{
			if (m_header.DeleteHeaderHdl(get_client_memptr(wan_client, i)->hdr_hdl_v4)
				== false)
			{
				res = IPACM_FAILURE;
				goto fail;
			}
		}

		if (get_client_memptr(wan_client, i)->ipv6_header_set == true)
		{
			if (m_header.DeleteHeaderHdl(get_client_memptr(wan_client, i)->hdr_hdl_v6)
				== false)
			{
				res = IPACM_FAILURE;
				goto fail;
			}
		}
	} /* end of for loop */

	/* free the edm clients cache */
	IPACMDBG_H("Free wan clients cache\n");

	/* check software routing fl rule hdl */
	if (softwarerouting_act == true)
	{
		handle_software_routing_disable();
	}

	/* free dft ipv4 filter rule handlers if any */
	if (ip_type != IPA_IP_v6 && rx_prop != NULL)
	{
		if (dft_v4fl_rule_hdl[0] != 0)
		{
			if (m_filtering.DeleteFilteringHdls(dft_v4fl_rule_hdl,
				IPA_IP_v4,
				IPV4_DEFAULT_FILTERTING_RULES) == false)
			{
				IPACMERR("Error Delete Filtering rules, aborting...\n");
				res = IPACM_FAILURE;
				goto fail;
			}
			IPACM_Iface::ipacmcfg->decreaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v4, IPV4_DEFAULT_FILTERTING_RULES);
			IPACMDBG_H("finished delete default v4 filtering rules\n ");
		}
	}

	/* free dft ipv6 filter rule handlers if any */
	if (ip_type != IPA_IP_v4 && rx_prop != NULL)
	{
		if (dft_v6fl_rule_hdl[0] != 0)
		{
			if (!m_filtering.DeleteFilteringHdls(dft_v6fl_rule_hdl, IPA_IP_v6, m_ipv6_default_filterting_rules_count))
			{
				IPACMERR("ErrorDeleting Filtering rule, aborting...\n");
				res = IPACM_FAILURE;
				goto fail;
			}
			IPACM_Iface::ipacmcfg->decreaseFltRuleCount(
				rx_prop->rx[0].src_pipe, IPA_IP_v6, m_ipv6_default_filterting_rules_count);
		}

		if(num_ipv6_dest_flt_rule > 0 && num_ipv6_dest_flt_rule <= MAX_DEFAULT_v6_ROUTE_RULES)
		{
			if(m_filtering.DeleteFilteringHdls(ipv6_dest_flt_rule_hdl,  IPA_IP_v6, num_ipv6_dest_flt_rule) == false)
			{
				IPACMERR("Failed to delete ipv6 dest flt rules.\n");
				res = IPACM_FAILURE;
				goto fail;
			}
			IPACM_Iface::ipacmcfg->decreaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, num_ipv6_dest_flt_rule);
		}
		IPACMDBG_H("finished delete default v6 filtering rules\n ");
	}
	if(hdr_proc_hdl_dummy_v6)
	{
		if(m_header.DeleteHeaderProcCtx(hdr_proc_hdl_dummy_v6) == false)
		{
			IPACMERR("Failed to delete hdr_proc_hdl_dummy_v6\n");
			res = IPACM_FAILURE;
			goto fail;
		}
	}
	if(hdr_hdl_dummy_v6)
	{
		if (m_header.DeleteHeaderHdl(hdr_hdl_dummy_v6) == false)
		{
			IPACMERR("Failed to delete hdr_hdl_dummy_v6\n");
			res = IPACM_FAILURE;
			goto fail;
		}
	}
fail:
	if (tx_prop != NULL)
	{
		free(tx_prop);
	}
	if (rx_prop != NULL)
	{
		free(rx_prop);
	}
	if (iface_query != NULL)
	{
		free(iface_query);
	}
	if (wan_route_rule_v4_hdl != NULL)
	{
		free(wan_route_rule_v4_hdl);
	}
	if (wan_route_rule_v6_hdl != NULL)
	{
		free(wan_route_rule_v6_hdl);
	}
	if (wan_client != NULL)
	{
		free(wan_client);
	}
	close(m_fd_ipa);
	return res;
}

int IPACM_Wan::handle_down_evt_ex()
{
	int res = IPACM_SUCCESS;
	int i, tether_total;
	int ipa_if_num_tether_tmp[IPA_MAX_IFACE_ENTRIES];

	IPACMDBG_H(" wan handle_down_evt \n");

	/* free ODU filter rule handlers */
	if(IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_cat == EMBMS_IF)
	{
		embms_is_on = false;
		if(IPACM_Iface::ipacmcfg->GetIPAVer() >= IPA_HW_None && IPACM_Iface::ipacmcfg->GetIPAVer() < IPA_HW_v4_0)
		{
			/* Delete corresponding ipa_rm_resource_name of TX-endpoint after delete IPV4/V6 RT-rule */
			IPACMDBG_H("dev %s delete producer dependency\n", dev_name);
			if (tx_prop != NULL)
			{
				IPACMDBG_H("depend Got pipe %d rm index : %d \n", tx_prop->tx[0].dst_pipe, IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
				IPACM_Iface::ipacmcfg->DelRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
			}
		}
		if (rx_prop != NULL)
		{
			install_wan_filtering_rule(false);
			IPACMDBG("finished delete embms filtering rule\n ");
		}
		goto fail;
	}

	/* no iface address up, directly close iface*/
	if (ip_type == IPACM_IP_NULL)
	{
		goto fail;
	}

	if(ip_type == IPA_IP_v4)
	{
		num_ipv4_modem_pdn--;
		IPACMDBG_H("Now the number of ipv4 modem pdn is %d.\n", num_ipv4_modem_pdn);
#ifdef FEATURE_VLAN_MPDN
		if(ipv4_to_iface[modem_ipv4_pdn_index].wan_up_vlan)
		{
			ipacm_cmd_q_data evt_data;
			ipacm_event_vlan_pdn *vlandown_data;

			ipv4_to_iface[modem_ipv4_pdn_index].wan_up_vlan = false;

			vlandown_data = (ipacm_event_vlan_pdn *)malloc(sizeof(ipacm_event_vlan_pdn));
			if(vlandown_data == NULL)
			{
				IPACMERR("Unable to allocate memory\n");
				res = IPACM_FAILURE;
				goto fail;
			}
			memset(vlandown_data, 0, sizeof(ipacm_event_vlan_pdn));

			vlandown_data->iptype = IPA_IP_v4;
			vlandown_data->VlanID = associated_VID;
			vlandown_data->ipv4_addr = wan_v4_addr;
			vlandown_data->mux_id = ext_prop->ext[0].mux_id;

			IPACMDBG_H("Posting IPA_HANDLE_WAN_VLAN_PDN_DOWN with below information:\n");
			IPACMDBG_H("iptype IPA_IP_v4, VlanID %d, mux_id %d, if num %d\n", associated_VID, ext_prop->ext[0].mux_id, ipa_if_num);

			evt_data.event = IPA_HANDLE_WAN_VLAN_PDN_DOWN;
			evt_data.evt_data = (void *)vlandown_data;

			IPACM_EvtDispatcher::PostEvt(&evt_data);

			/* in also default gateway, DL filtering rules will be reconfigured later */
			if(!is_default_gateway)
			{
				del_wan_firewall_rule(IPA_IP_v4);
				/* if there are still PDNs up we need to reconfigure firewall */
				if(isVlanWanUP() || wan_up)
				{
					config_wan_firewall_rule(IPA_IP_v4);
				}
				install_wan_filtering_rule(false);
			}
		}

		ipv4_to_iface[modem_ipv4_pdn_index].ipv4_addr = 0;
		ipv4_to_iface[modem_ipv4_pdn_index].pIface = NULL;

		/* if no PDN is up, remove rm dependencies */
		if(!isVlanWanUP() && !isVlanWanUP_V6() && !wan_up && !wan_up_v6)
		{
			if(IPACM_Iface::ipacmcfg->GetIPAVer() >= IPA_HW_None && IPACM_Iface::ipacmcfg->GetIPAVer() < IPA_HW_v4_0)
			{
				/* Delete corresponding ipa_rm_resource_name of TX-endpoint after delete IPV4/V6 RT-rule */
				IPACMDBG_H("dev %s delete producer dependency\n", dev_name);
				IPACMDBG_H("depend Got pipe %d rm index : %d \n", tx_prop->tx[0].dst_pipe, IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
				IPACM_Iface::ipacmcfg->DelRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
			}
			else {
				IPACMDBG_H("ipa_pm_q6_check to %d\n", ipa_pm_q6_check);
				if(ipa_pm_q6_check == 1)
				{
					struct wan_ioctl_notify_wan_state wan_state;

					memset(&wan_state, 0, sizeof(wan_state));

					int fd_wwan_ioctl = open(WWAN_QMI_IOCTL_DEVICE_NAME, O_RDWR);
					if(fd_wwan_ioctl < 0)
					{
						IPACMERR("Failed to open %s.\n", WWAN_QMI_IOCTL_DEVICE_NAME);
						return false;
					}
					IPACMDBG_H("send WAN_IOC_NOTIFY_WAN_STATE down to IPA_PM\n");
					if(ioctl(fd_wwan_ioctl, WAN_IOC_NOTIFY_WAN_STATE, &wan_state))
					{
						IPACMERR("Failed to send WAN_IOC_NOTIFY_WAN_STATE as up %d\n ", wan_state.up);
					}
					close(fd_wwan_ioctl);
				}
				if(ipa_pm_q6_check > 0)
					ipa_pm_q6_check--;
				else
					IPACMERR(" ipa_pm_q6_check becomes negative !!!\n");
			}
		}
		else
		{
			IPACMDBG_H("not deleting rm depend for default rt, a v4 VLAN PDN is still up, iptype %d\n", ip_type);
		}
#endif
		/* only when default gw goes down we post WAN_DOWN event*/
		if(is_default_gateway == true)
		{
			IPACM_Wan::wan_up = false;
			del_wan_firewall_rule(IPA_IP_v4);
#ifdef FEATURE_VLAN_MPDN
			/* if there are still secondary PDNs up we need to reconfigure firewall */
			if(isVlanWanUP())
			{
				config_wan_firewall_rule(IPA_IP_v4);
			}
#endif
			install_wan_filtering_rule(false);
			handle_route_del_evt_ex(IPA_IP_v4);
#ifdef FEATURE_IPA_ANDROID
			/* posting wan_down_tether for all lan clients */
			for (i=0; i < IPACM_Wan::ipa_if_num_tether_v4_total; i++)
			{
				ipa_if_num_tether_tmp[i] = IPACM_Wan::ipa_if_num_tether_v4[i];
			}
			tether_total = IPACM_Wan::ipa_if_num_tether_v4_total;
			for (i=0; i < tether_total; i++)
			{
				post_wan_down_tether_evt(IPA_IP_v4, ipa_if_num_tether_tmp[i]);
				IPACMDBG_H("post_wan_down_tether_v4 iface(%d: %s)\n",
					i, IPACM_Iface::ipacmcfg->iface_table[ipa_if_num_tether_tmp[i]].iface_name);
			}
#endif
			if(IPACM_Wan::wan_up_v6)
			{
				IPACMDBG_H("modem v6-call still up(%s), not reset\n", IPACM_Wan::wan_up_dev_name);
			}
			else
			{
				memset(IPACM_Wan::wan_up_dev_name, 0, sizeof(IPACM_Wan::wan_up_dev_name));
			}
		}

		/* only when the last ipv4 modem interface goes down, delete ipv4 default flt rules*/
		if(num_ipv4_modem_pdn == 0)
		{
			IPACMDBG_H("Now the number of modem ipv4 interface is 0, delete default flt rules.\n");
			IPACM_Wan::num_v4_flt_rule = 0;
#ifdef FEATURE_VLAN_MPDN
			memset(IPACM_Wan::pdn_flt_rule_v4, 0, IPA_MAX_FLT_RULE * sizeof(struct ipacm_pdn_flt_rule));
#else
			memset(IPACM_Wan::flt_rule_v4, 0, IPA_MAX_FLT_RULE * sizeof(struct ipa_flt_rule_add));
#endif
			install_wan_filtering_rule(false);
		}

		if (m_routing.DeleteRoutingHdl(dft_rt_rule_hdl[0], IPA_IP_v4) == false)
		{
			IPACMERR("Routing rule deletion failed!\n");
			res = IPACM_FAILURE;
			goto fail;
		}
	}
	else if(ip_type == IPA_IP_v6)
	{
		if(num_dft_rt_v6 > 1)
		{
			num_ipv6_modem_pdn--;
		}
		IPACMDBG_H("Now the number of ipv6 modem pdn is %d.\n", num_ipv6_modem_pdn);
		/* only when default gw goes down we post WAN_DOWN event*/

#ifdef FEATURE_VLAN_MPDN
		IPACM_Iface::ipacmcfg->del_vlan_ipv6_prefix(ipv6_prefix, -1);

		if(ipv6_to_iface[modem_ipv6_pdn_index].wan_up_vlan_v6)
		{
			ipacm_cmd_q_data evt_data;
			ipacm_event_vlan_pdn *vlandown_data;

			ipv6_to_iface[modem_ipv6_pdn_index].wan_up_vlan_v6 = false;
			memset(&ipv6_to_iface[modem_ipv6_pdn_index].ipv6_prefix, 0, sizeof(uint32_t) * 2);
			ipv6_to_iface[modem_ipv6_pdn_index].pIface = NULL;

			if(!isVlanWanUP_V6())
			{
				IPACMDBG_H("ip-type %d: default v6 wan RT-rule deleted\n", ip_type);
				if(m_routing.DeleteRoutingHdl(wan_route_rule_v6_hdl_a5, IPA_IP_v6) == false)
				{
					IPACMDBG_H("IP-family:%d, Routing rule(hdl:0x%x) deletion failed!\n", IPA_IP_v6, wan_route_rule_v6_hdl_a5);
					return IPACM_FAILURE;
				}
			}
			else
			{
				IPACMDBG_H("not deleting default v6 RT rule, vlan v6 PDN is up\n");
			}

			vlandown_data = (ipacm_event_vlan_pdn *)malloc(sizeof(ipacm_event_vlan_pdn));
			if(vlandown_data == NULL)
			{
				IPACMERR("Unable to allocate memory\n");
				res = IPACM_FAILURE;
				goto fail;
			}
			memset(vlandown_data, 0, sizeof(ipacm_event_vlan_pdn));

			vlandown_data->iptype = IPA_IP_v6;
			vlandown_data->VlanID = associated_VID;
			vlandown_data->mux_id = ext_prop->ext[0].mux_id;

			IPACMDBG_H("Posting IPA_HANDLE_WAN_VLAN_PDN_DOWN (v6) with below information:\n");
			IPACMDBG_H("iptype IPA_IP_v6, VlanID %d, mux_id %d, if num %d\n", associated_VID, ext_prop->ext[0].mux_id, ipa_if_num);

			evt_data.event = IPA_HANDLE_WAN_VLAN_PDN_DOWN;
			evt_data.evt_data = (void *)vlandown_data;

			IPACM_EvtDispatcher::PostEvt(&evt_data);

			/* in also default gateway, DL filtering rules will be reconfigured later */
			if(!is_default_gateway)
			{
				del_wan_firewall_rule(IPA_IP_v6);
				/* if there are still PDNs up we need to reconfigure firewall */
				if(isVlanWanUP_V6() || wan_up_v6)
				{
					config_wan_firewall_rule(IPA_IP_v6);
				}
				install_wan_filtering_rule(false);
			}
		}

		/* if no PDN is up, remove rm dependencies */
		if(!isVlanWanUP() && !isVlanWanUP_V6() && !wan_up && !wan_up_v6)
		{
			if(IPACM_Iface::ipacmcfg->GetIPAVer() >= IPA_HW_None && IPACM_Iface::ipacmcfg->GetIPAVer() < IPA_HW_v4_0)
			{
				/* Delete corresponding ipa_rm_resource_name of TX-endpoint after delete IPV4/V6 RT-rule */
				IPACMDBG_H("dev %s delete producer dependency\n", dev_name);
				IPACMDBG_H("depend Got pipe %d rm index : %d \n", tx_prop->tx[0].dst_pipe, IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
				IPACM_Iface::ipacmcfg->DelRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
			}
			else {
				IPACMDBG_H("ipa_pm_q6_check to %d\n", ipa_pm_q6_check);
				if(ipa_pm_q6_check == 1)
				{
					struct wan_ioctl_notify_wan_state wan_state;

					memset(&wan_state, 0, sizeof(wan_state));

					int fd_wwan_ioctl = open(WWAN_QMI_IOCTL_DEVICE_NAME, O_RDWR);
					if(fd_wwan_ioctl < 0)
					{
						IPACMERR("Failed to open %s.\n", WWAN_QMI_IOCTL_DEVICE_NAME);
						return false;
					}
					IPACMDBG_H("send WAN_IOC_NOTIFY_WAN_STATE down to IPA_PM\n");
					if(ioctl(fd_wwan_ioctl, WAN_IOC_NOTIFY_WAN_STATE, &wan_state))
					{
						IPACMERR("Failed to send WAN_IOC_NOTIFY_WAN_STATE as up %d\n ", wan_state.up);
					}
					close(fd_wwan_ioctl);
				}
				if(ipa_pm_q6_check > 0)
					ipa_pm_q6_check--;
				else
					IPACMERR(" ipa_pm_q6_check becomes negative !!!\n");
			}
		}
		else
		{
			IPACMDBG_H("not deleting rm depend for default rt, a v6 VLAN PDN is still up, iptype %d\n", ip_type);
		}
#endif
		if(is_default_gateway == true)
		{
			IPACM_Wan::wan_up_v6 = false;
			del_wan_firewall_rule(IPA_IP_v6);
#ifdef FEATURE_VLAN_MPDN
			/* if there are still secondary PDNs up we need to reconfigure firewall */
			if(isVlanWanUP_V6())
			{
				config_wan_firewall_rule(IPA_IP_v4);
			}
#endif
			install_wan_filtering_rule(false);
			handle_route_del_evt_ex(IPA_IP_v6);
#ifdef FEATURE_IPA_ANDROID //sky
			/* posting wan_down_tether for all lan clients */
			for (i=0; i < IPACM_Wan::ipa_if_num_tether_v6_total; i++)
			{
				ipa_if_num_tether_tmp[i] = IPACM_Wan::ipa_if_num_tether_v6[i];
			}
			tether_total = IPACM_Wan::ipa_if_num_tether_v6_total;
			for (i=0; i < tether_total; i++)
			{
				post_wan_down_tether_evt(IPA_IP_v6, ipa_if_num_tether_tmp[i]);
				IPACMDBG_H("post_wan_down_tether_v6 iface(%d: %s)\n",
					i, IPACM_Iface::ipacmcfg->iface_table[ipa_if_num_tether_tmp[i]].iface_name);
			}
#endif
			if(IPACM_Wan::wan_up)
			{
				IPACMDBG_H("modem v4-call still up(%s), not reset\n", IPACM_Wan::wan_up_dev_name);
			}
			else
			{
				memset(IPACM_Wan::wan_up_dev_name, 0, sizeof(IPACM_Wan::wan_up_dev_name));
			}
		}

		/* only when the last ipv6 modem interface goes down, delete ipv6 default flt rules*/
		if(num_ipv6_modem_pdn == 0)
		{
			IPACMDBG_H("Now the number of modem ipv6 interface is 0, delete default flt rules.\n");
			IPACM_Wan::num_v6_flt_rule = 0;
#ifdef FEATURE_VLAN_MPDN
			IPACM_Wan::ipv6_mpdn_default_filterting_rules_count = 0;
			memset(IPACM_Wan::pdn_flt_rule_v6, 0, IPA_MAX_FLT_RULE * sizeof(struct ipacm_pdn_flt_rule));
#else
			memset(IPACM_Wan::flt_rule_v6, 0, IPA_MAX_FLT_RULE * sizeof(struct ipa_flt_rule_add));
#endif
			install_wan_filtering_rule(false);
		}

		for (i = 0; i < 2*num_dft_rt_v6; i++)
		{
			if (m_routing.DeleteRoutingHdl(dft_rt_rule_hdl[MAX_DEFAULT_v4_ROUTE_RULES+i], IPA_IP_v6) == false)
			{
				IPACMERR("Routing rule deletion failed!\n");
				res = IPACM_FAILURE;
				goto fail;
			}
		}
	}
	else
	{
		num_ipv4_modem_pdn--;
		IPACMDBG_H("Now the number of ipv4 modem pdn is %d.\n", num_ipv4_modem_pdn);
		if (num_dft_rt_v6 > 1)
			num_ipv6_modem_pdn--;
		IPACMDBG_H("Now the number of ipv6 modem pdn is %d.\n", num_ipv6_modem_pdn);

#ifdef FEATURE_VLAN_MPDN
		IPACM_Iface::ipacmcfg->del_vlan_ipv6_prefix(ipv6_prefix, -1);

		if(ipv4_to_iface[modem_ipv4_pdn_index].wan_up_vlan ||
			ipv6_to_iface[modem_ipv6_pdn_index].wan_up_vlan_v6)
		{
			ipacm_cmd_q_data evt_data;
			ipacm_event_vlan_pdn *vlandown_data;

			vlandown_data = (ipacm_event_vlan_pdn *)malloc(sizeof(ipacm_event_vlan_pdn));
			if(vlandown_data == NULL)
			{
				IPACMERR("Unable to allocate memory\n");
				res = IPACM_FAILURE;
				goto fail;
			}
			memset(vlandown_data, 0, sizeof(ipacm_event_vlan_pdn));

			if(ipv4_to_iface[modem_ipv4_pdn_index].wan_up_vlan &&
				ipv6_to_iface[modem_ipv6_pdn_index].wan_up_vlan_v6)
			{
				vlandown_data->iptype = IPA_IP_MAX;
				ipv4_to_iface[modem_ipv4_pdn_index].wan_up_vlan = false;
				ipv4_to_iface[modem_ipv4_pdn_index].ipv4_addr = 0;
				ipv4_to_iface[modem_ipv4_pdn_index].pIface = NULL;

				ipv6_to_iface[modem_ipv6_pdn_index].wan_up_vlan_v6 = false;
				memset(&ipv6_to_iface[modem_ipv6_pdn_index].ipv6_prefix, 0, sizeof(uint32_t) * 2);
				ipv6_to_iface[modem_ipv6_pdn_index].pIface = NULL;
			}
			else if(ipv4_to_iface[modem_ipv4_pdn_index].wan_up_vlan)
			{
				vlandown_data->iptype = IPA_IP_v4;
				vlandown_data->ipv4_addr = wan_v4_addr;
				ipv4_to_iface[modem_ipv4_pdn_index].wan_up_vlan = false;
				ipv4_to_iface[modem_ipv4_pdn_index].ipv4_addr = 0;
				ipv4_to_iface[modem_ipv4_pdn_index].pIface = NULL;
			}
			else
			{
				vlandown_data->iptype = IPA_IP_v6;
				ipv6_to_iface[modem_ipv6_pdn_index].wan_up_vlan_v6 = false;
				memset(&ipv6_to_iface[modem_ipv6_pdn_index].ipv6_prefix, 0, sizeof(uint32_t) * 2);
				ipv6_to_iface[modem_ipv6_pdn_index].pIface = NULL;
			}

			vlandown_data->VlanID = associated_VID;
			vlandown_data->mux_id = ext_prop->ext[0].mux_id;

			IPACMDBG_H("Posting IPA_HANDLE_WAN_VLAN_PDN_DOWN (v6) with below information:\n");
			IPACMDBG_H("iptype IPA_IP_v6, VlanID %d, mux_id %d, if num %d\n", associated_VID, ext_prop->ext[0].mux_id, ipa_if_num);

			evt_data.event = IPA_HANDLE_WAN_VLAN_PDN_DOWN;
			evt_data.evt_data = (void *)vlandown_data;

			IPACM_EvtDispatcher::PostEvt(&evt_data);
		}

		if(!isVlanWanUP())
		{
			if(IPACM_Iface::ipacmcfg->GetIPAVer() >= IPA_HW_None && IPACM_Iface::ipacmcfg->GetIPAVer() < IPA_HW_v4_0)
			{
				/* Delete corresponding ipa_rm_resource_name of TX-endpoint after delete IPV4/V6 RT-rule */
				IPACMDBG_H("dev %s delete producer dependency\n", dev_name);
				IPACMDBG_H("depend Got pipe %d rm index : %d \n", tx_prop->tx[0].dst_pipe, IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
				IPACM_Iface::ipacmcfg->DelRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
			}
			else {
				IPACMDBG_H("ipa_pm_q6_check to %d\n", ipa_pm_q6_check);
				if(ipa_pm_q6_check == 1)
				{
					struct wan_ioctl_notify_wan_state wan_state;

					memset(&wan_state, 0, sizeof(wan_state));

					int fd_wwan_ioctl = open(WWAN_QMI_IOCTL_DEVICE_NAME, O_RDWR);
					if(fd_wwan_ioctl < 0)
					{
						IPACMERR("Failed to open %s.\n", WWAN_QMI_IOCTL_DEVICE_NAME);
						return false;
					}
					IPACMDBG_H("send WAN_IOC_NOTIFY_WAN_STATE down to IPA_PM\n");
					if(ioctl(fd_wwan_ioctl, WAN_IOC_NOTIFY_WAN_STATE, &wan_state))
					{
						IPACMERR("Failed to send WAN_IOC_NOTIFY_WAN_STATE as up %d\n ", wan_state.up);
					}
					close(fd_wwan_ioctl);
				}
				if(ipa_pm_q6_check > 0)
					ipa_pm_q6_check--;
				else
					IPACMERR(" ipa_pm_q6_check becomes negative !!!\n");
			}
		}
		else
		{
			IPACMDBG_H("not deleting rm depend for default rt, a v4 VLAN PDN is still up, iptype %d\n", ip_type);
		}

		if(!isVlanWanUP_V6())
		{
			if(IPACM_Iface::ipacmcfg->GetIPAVer() >= IPA_HW_None && IPACM_Iface::ipacmcfg->GetIPAVer() < IPA_HW_v4_0)
			{
				/* Delete corresponding ipa_rm_resource_name of TX-endpoint after delete IPV4/V6 RT-rule */
				IPACMDBG_H("dev %s delete producer dependency\n", dev_name);
				IPACMDBG_H("depend Got pipe %d rm index : %d \n", tx_prop->tx[0].dst_pipe, IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
				IPACM_Iface::ipacmcfg->DelRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
			}
			else {
				IPACMDBG_H("ipa_pm_q6_check to %d\n", ipa_pm_q6_check);
				if(ipa_pm_q6_check == 1)
				{
					struct wan_ioctl_notify_wan_state wan_state;

					memset(&wan_state, 0, sizeof(wan_state));

					int fd_wwan_ioctl = open(WWAN_QMI_IOCTL_DEVICE_NAME, O_RDWR);
					if(fd_wwan_ioctl < 0)
					{
						IPACMERR("Failed to open %s.\n", WWAN_QMI_IOCTL_DEVICE_NAME);
						return false;
					}
					IPACMDBG_H("send WAN_IOC_NOTIFY_WAN_STATE down to IPA_PM\n");
					if(ioctl(fd_wwan_ioctl, WAN_IOC_NOTIFY_WAN_STATE, &wan_state))
					{
						IPACMERR("Failed to send WAN_IOC_NOTIFY_WAN_STATE as up %d\n ", wan_state.up);
					}
					close(fd_wwan_ioctl);
				}
				if(ipa_pm_q6_check > 0)
					ipa_pm_q6_check--;
				else
					IPACMERR(" ipa_pm_q6_check becomes negative !!!\n");
			}
		}
		else
		{
			IPACMDBG_H("not deleting rm depend for default rt, a v6 VLAN PDN is still up, iptype %d\n", ip_type);
		}
#endif
		/* only when default gw goes down we post WAN_DOWN event*/
		if(is_default_gateway == true)
		{
			IPACM_Wan::wan_up = false;
			del_wan_firewall_rule(IPA_IP_v4);
#ifdef FEATURE_VLAN_MPDN
			/* if there are still secondary PDNs up we need to reconfigure firewall */
			if(isVlanWanUP())
			{
				config_wan_firewall_rule(IPA_IP_v4);
			}
#endif
			handle_route_del_evt_ex(IPA_IP_v4);
#ifdef FEATURE_IPA_ANDROID
			/* posting wan_down_tether for all lan clients */
			for (i=0; i < IPACM_Wan::ipa_if_num_tether_v4_total; i++)
			{
				ipa_if_num_tether_tmp[i] = IPACM_Wan::ipa_if_num_tether_v4[i];
			}
			tether_total = IPACM_Wan::ipa_if_num_tether_v4_total;
			for (i=0; i < tether_total; i++)
			{
				post_wan_down_tether_evt(IPA_IP_v4, ipa_if_num_tether_tmp[i]);
				IPACMDBG_H("post_wan_down_tether_v4 iface(%d: %s)\n",
					i, IPACM_Iface::ipacmcfg->iface_table[ipa_if_num_tether_tmp[i]].iface_name);
			}
#endif
			IPACM_Wan::wan_up_v6 = false;
			del_wan_firewall_rule(IPA_IP_v6);
#ifdef FEATURE_VLAN_MPDN
			/* if there are still secondary PDNs up we need to reconfigure firewall */
			if(isVlanWanUP_V6())
			{
				config_wan_firewall_rule(IPA_IP_v4);
			}
#endif
			handle_route_del_evt_ex(IPA_IP_v6);
#ifdef FEATURE_IPA_ANDROID
			/* posting wan_down_tether for all lan clients */
			for (i=0; i < IPACM_Wan::ipa_if_num_tether_v6_total; i++)
			{
				ipa_if_num_tether_tmp[i] = IPACM_Wan::ipa_if_num_tether_v6[i];
			}
			tether_total = IPACM_Wan::ipa_if_num_tether_v6_total;
			for (i=0; i < tether_total; i++)
			{
				post_wan_down_tether_evt(IPA_IP_v6, ipa_if_num_tether_tmp[i]);
				IPACMDBG_H("post_wan_down_tether_v6 iface(%d: %s)\n",
					i, IPACM_Iface::ipacmcfg->iface_table[ipa_if_num_tether_tmp[i]].iface_name);
			}
#endif
			memset(IPACM_Wan::wan_up_dev_name, 0, sizeof(IPACM_Wan::wan_up_dev_name));

			install_wan_filtering_rule(false);
		}

		/* only when the last ipv4 modem interface goes down, delete ipv4 default flt rules*/
		if(num_ipv4_modem_pdn == 0)
		{
			IPACMDBG_H("Now the number of modem ipv4 interface is 0, delete default flt rules.\n");
			IPACM_Wan::num_v4_flt_rule = 0;
#ifdef FEATURE_VLAN_MPDN
			memset(IPACM_Wan::pdn_flt_rule_v4, 0, IPA_MAX_FLT_RULE * sizeof(struct ipacm_pdn_flt_rule));
#else
			memset(IPACM_Wan::flt_rule_v4, 0, IPA_MAX_FLT_RULE * sizeof(struct ipa_flt_rule_add));
#endif
			install_wan_filtering_rule(false);
		}
		/* only when the last ipv6 modem interface goes down, delete ipv6 default flt rules*/
		if(num_ipv6_modem_pdn == 0)
		{
			IPACMDBG_H("Now the number of modem ipv6 interface is 0, delete default flt rules.\n");
			IPACM_Wan::num_v6_flt_rule = 0;
#ifdef FEATURE_VLAN_MPDN
			memset(IPACM_Wan::pdn_flt_rule_v6, 0, IPA_MAX_FLT_RULE * sizeof(struct ipacm_pdn_flt_rule));
#else
			memset(IPACM_Wan::flt_rule_v6, 0, IPA_MAX_FLT_RULE * sizeof(struct ipa_flt_rule_add));
#endif
			install_wan_filtering_rule(false);
		}

		if (m_routing.DeleteRoutingHdl(dft_rt_rule_hdl[0], IPA_IP_v4) == false)
		{
			IPACMERR("Routing rule deletion failed!\n");
			res = IPACM_FAILURE;
			goto fail;
		}

		for (i = 0; i < 2*num_dft_rt_v6; i++)
		{
			if (m_routing.DeleteRoutingHdl(dft_rt_rule_hdl[MAX_DEFAULT_v4_ROUTE_RULES+i], IPA_IP_v6) == false)
			{
				IPACMERR("Routing rule deletion failed!\n");
				res = IPACM_FAILURE;
				goto fail;
			}
		}
	}

	/* check software routing fl rule hdl */
	if (softwarerouting_act == true)
	{
		handle_software_routing_disable();
	}

fail:
	if (tx_prop != NULL)
	{
		free(tx_prop);
	}
	if (rx_prop != NULL)
	{
		free(rx_prop);
	}
	if (ext_prop != NULL)
	{
		free(ext_prop);
	}
	if (iface_query != NULL)
	{
		free(iface_query);
	}
	if (wan_route_rule_v4_hdl != NULL)
	{
		free(wan_route_rule_v4_hdl);
	}
	if (wan_route_rule_v6_hdl != NULL)
	{
		free(wan_route_rule_v6_hdl);
	}
	if (wan_client != NULL)
	{
		free(wan_client);
	}
	close(m_fd_ipa);
	return res;
}

int IPACM_Wan::install_wan_filtering_rule(bool is_sw_routing)
{
	int len, res = IPACM_SUCCESS;
	uint8_t mux_id;
	ipa_ioc_add_flt_rule *pFilteringTable_v4 = NULL;
	ipa_ioc_add_flt_rule *pFilteringTable_v6 = NULL;
#ifdef FEATURE_VLAN_MPDN
	uint8_t *mux_id_v4 = NULL;
	uint8_t *mux_id_v6 = NULL;
#endif

	mux_id = IPACM_Iface::ipacmcfg->GetQmapId();
	if(rx_prop == NULL)
	{
		IPACMDBG_H("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}
	if (is_sw_routing == true ||
			IPACM_Iface::ipacmcfg->ipa_sw_rt_enable == true)
	{
		/* contruct SW-RT rules to Q6*/
		struct ipa_flt_rule_add flt_rule_entry;
		struct ipa_ioc_get_rt_tbl_indx rt_tbl_idx;
		ipa_ioc_generate_flt_eq flt_eq;

		IPACMDBG("\n");
		if (softwarerouting_act == true)
		{
			IPACMDBG("already setup software_routing rule for (%s)iface ip-family %d\n",
								IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].iface_name, ip_type);
			return IPACM_SUCCESS;
		}

		len = sizeof(struct ipa_ioc_add_flt_rule) + sizeof(struct ipa_flt_rule_add);
		pFilteringTable_v4 = (struct ipa_ioc_add_flt_rule*)malloc(len);
		if (pFilteringTable_v4 == NULL)
		{
			IPACMERR("Error Locate ipa_flt_rule_add memory...\n");
			return IPACM_FAILURE;
		}
		memset(pFilteringTable_v4, 0, len);
#ifdef FEATURE_VLAN_MPDN
		mux_id_v4 = (uint8_t*)malloc(sizeof(uint8_t));
		if(mux_id_v4 == NULL)
		{
			IPACMERR("Error Locate mux_id_v4 memory...\n");
			return IPACM_FAILURE;
		}
		/* use the default PDN mux ID */
		mux_id_v4[0] = IPACM_Iface::ipacmcfg->GetQmapId();
#endif
		IPACMDBG_H("Total number of WAN DL filtering rule for IPv4 is 1\n");

		pFilteringTable_v4->commit = 1;
		pFilteringTable_v4->ep = rx_prop->rx[0].src_pipe;
		pFilteringTable_v4->global = false;
		pFilteringTable_v4->ip = IPA_IP_v4;
		pFilteringTable_v4->num_rules = (uint8_t)1;

		/* Configuring Software-Routing Filtering Rule */
		memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
		memset(&rt_tbl_idx, 0, sizeof(rt_tbl_idx));
		strlcpy(rt_tbl_idx.name, IPACM_Iface::ipacmcfg->rt_tbl_wan_dl.name, IPA_RESOURCE_NAME_MAX);
		rt_tbl_idx.name[IPA_RESOURCE_NAME_MAX-1] = '\0';
		rt_tbl_idx.ip = IPA_IP_v4;
		if(ioctl(m_fd_ipa, IPA_IOC_QUERY_RT_TBL_INDEX, &rt_tbl_idx) < 0)
		{
			IPACMERR("Failed to get routing table index from name\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		IPACMDBG_H("Routing table %s has index %d\n", rt_tbl_idx.name, rt_tbl_idx.idx);

		flt_rule_entry.at_rear = false;
		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;
		flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
#ifdef FEATURE_IPA_V3
		flt_rule_entry.rule.hashable = true;
#endif

		flt_rule_entry.rule.rt_tbl_idx = rt_tbl_idx.idx;

		memcpy(&flt_rule_entry.rule.attrib,
					&rx_prop->rx[0].attrib,
					sizeof(flt_rule_entry.rule.attrib));
		flt_rule_entry.rule.retain_hdr = 0;
		flt_rule_entry.rule.to_uc = 0;
		flt_rule_entry.rule.eq_attrib_type = 1;

		memset(&flt_eq, 0, sizeof(flt_eq));
		memcpy(&flt_eq.attrib, &flt_rule_entry.rule.attrib, sizeof(flt_eq.attrib));
		flt_eq.ip = IPA_IP_v4;
		if(0 != ioctl(m_fd_ipa, IPA_IOC_GENERATE_FLT_EQ, &flt_eq))
		{
			IPACMERR("Failed to get eq_attrib\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		memcpy(&flt_rule_entry.rule.eq_attrib,
			&flt_eq.eq_attrib,
			sizeof(flt_rule_entry.rule.eq_attrib));
		memcpy(&(pFilteringTable_v4->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

		len = sizeof(struct ipa_ioc_add_flt_rule) + sizeof(struct ipa_flt_rule_add);
		pFilteringTable_v6 = (struct ipa_ioc_add_flt_rule*)malloc(len);
		if (pFilteringTable_v6 == NULL)
		{
			IPACMERR("Error Locate ipa_flt_rule_add memory...\n");
			free(pFilteringTable_v4);
			return IPACM_FAILURE;
		}
		memset(pFilteringTable_v6, 0, len);
#ifdef FEATURE_VLAN_MPDN
		mux_id_v6 = (uint8_t*)malloc(sizeof(uint8_t));
		if(mux_id_v6 == NULL)
		{
			IPACMERR("Error Locate mux_id_v6 memory...\n");
			return IPACM_FAILURE;
		}
		/* use the default PDN mux ID */
		mux_id_v6[0] = IPACM_Iface::ipacmcfg->GetQmapId();
#endif
		IPACMDBG_H("Total number of WAN DL filtering rule for IPv6 is 1\n");

		pFilteringTable_v6->commit = 1;
		pFilteringTable_v6->ep = rx_prop->rx[0].src_pipe;
		pFilteringTable_v6->global = false;
		pFilteringTable_v6->ip = IPA_IP_v6;
		pFilteringTable_v6->num_rules = (uint8_t)1;

		/* Configuring Software-Routing Filtering Rule */
		memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
		memset(&rt_tbl_idx, 0, sizeof(rt_tbl_idx));
		strlcpy(rt_tbl_idx.name, IPACM_Iface::ipacmcfg->rt_tbl_wan_dl.name, IPA_RESOURCE_NAME_MAX);
		rt_tbl_idx.name[IPA_RESOURCE_NAME_MAX-1] = '\0';
		rt_tbl_idx.ip = IPA_IP_v6;
		if(ioctl(m_fd_ipa, IPA_IOC_QUERY_RT_TBL_INDEX, &rt_tbl_idx) < 0)
		{
			IPACMERR("Failed to get routing table index from name\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		IPACMDBG_H("Routing table %s has index %d\n", rt_tbl_idx.name, rt_tbl_idx.idx);

		flt_rule_entry.at_rear = false;
		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;
		flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
#ifdef FEATURE_IPA_V3
		flt_rule_entry.rule.hashable = true;
#endif
		flt_rule_entry.rule.rt_tbl_idx = rt_tbl_idx.idx;
		memcpy(&flt_rule_entry.rule.attrib,
					&rx_prop->rx[0].attrib,
					sizeof(flt_rule_entry.rule.attrib));
		flt_rule_entry.rule.retain_hdr = 0;
		flt_rule_entry.rule.to_uc = 0;
		flt_rule_entry.rule.eq_attrib_type = 1;

		memset(&flt_eq, 0, sizeof(flt_eq));
		memcpy(&flt_eq.attrib, &flt_rule_entry.rule.attrib, sizeof(flt_eq.attrib));
		flt_eq.ip = IPA_IP_v6;
		if(0 != ioctl(m_fd_ipa, IPA_IOC_GENERATE_FLT_EQ, &flt_eq))
		{
			IPACMERR("Failed to get eq_attrib\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		memcpy(&flt_rule_entry.rule.eq_attrib,
			&flt_eq.eq_attrib,
			sizeof(flt_rule_entry.rule.eq_attrib));
		memcpy(&(pFilteringTable_v6->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
		softwarerouting_act = true;
		/* end of contruct SW-RT rules to Q6*/
	}
	else
	{
		if(embms_is_on == false)
		{
			if(IPACM_Wan::num_v4_flt_rule > 0)
			{
				len = sizeof(struct ipa_ioc_add_flt_rule) + IPACM_Wan::num_v4_flt_rule * sizeof(struct ipa_flt_rule_add);
				pFilteringTable_v4 = (struct ipa_ioc_add_flt_rule*)malloc(len);

				IPACMDBG_H("Total number of WAN DL filtering rule for IPv4 is %d\n", IPACM_Wan::num_v4_flt_rule);

				if (pFilteringTable_v4 == NULL)
				{
					IPACMERR("Error Locate ipa_flt_rule_add memory...\n");
					return IPACM_FAILURE;
				}
				memset(pFilteringTable_v4, 0, len);
#ifdef FEATURE_VLAN_MPDN
				mux_id_v4 = (uint8_t*)malloc(IPACM_Wan::num_v4_flt_rule * sizeof(uint8_t));
				if(mux_id_v4 == NULL)
				{
					IPACMERR("Error allocate mux_id_v4 memory...\n");
					return IPACM_FAILURE;
				}
#endif
				pFilteringTable_v4->commit = 1;
				pFilteringTable_v4->ep = rx_prop->rx[0].src_pipe;
				pFilteringTable_v4->global = false;
				pFilteringTable_v4->ip = IPA_IP_v4;
				pFilteringTable_v4->num_rules = (uint8_t)IPACM_Wan::num_v4_flt_rule;

#ifdef FEATURE_VLAN_MPDN
				for(int i = 0; i < IPACM_Wan::num_v4_flt_rule; i++)
				{
					mux_id_v4[i] = IPACM_Wan::pdn_flt_rule_v4[i].mux_id;
					memcpy(&pFilteringTable_v4->rules[i],
						&IPACM_Wan::pdn_flt_rule_v4[i].flt_rule,
						sizeof(ipa_flt_rule_add));
				}
#else
				memcpy(pFilteringTable_v4->rules, IPACM_Wan::flt_rule_v4, IPACM_Wan::num_v4_flt_rule * sizeof(ipa_flt_rule_add));
#endif
			}

			if(IPACM_Wan::num_v6_flt_rule > 0)
			{
				len = sizeof(struct ipa_ioc_add_flt_rule) + IPACM_Wan::num_v6_flt_rule * sizeof(struct ipa_flt_rule_add);
				pFilteringTable_v6 = (struct ipa_ioc_add_flt_rule*)malloc(len);

				IPACMDBG_H("Total number of WAN DL filtering rule for IPv6 is %d\n", IPACM_Wan::num_v6_flt_rule);

				if (pFilteringTable_v6 == NULL)
				{
					IPACMERR("Error Locate ipa_flt_rule_add memory...\n");
					free(pFilteringTable_v4);
					return IPACM_FAILURE;
				}
				memset(pFilteringTable_v6, 0, len);
#ifdef FEATURE_VLAN_MPDN
				mux_id_v6 = (uint8_t*)malloc(IPACM_Wan::num_v6_flt_rule * sizeof(uint8_t));
				if(mux_id_v6 == NULL)
				{
					IPACMERR("Error allocate mux_id_v6 memory...\n");
					return IPACM_FAILURE;
				}
#endif
				pFilteringTable_v6->commit = 1;
				pFilteringTable_v6->ep = rx_prop->rx[0].src_pipe;
				pFilteringTable_v6->global = false;
				pFilteringTable_v6->ip = IPA_IP_v6;
				pFilteringTable_v6->num_rules = (uint8_t)IPACM_Wan::num_v6_flt_rule;

#ifdef FEATURE_VLAN_MPDN
				for(int i = 0; i < IPACM_Wan::num_v6_flt_rule; i++)
				{
					mux_id_v6[i] = IPACM_Wan::pdn_flt_rule_v6[i].mux_id;
					memcpy(&pFilteringTable_v6->rules[i],
						&IPACM_Wan::pdn_flt_rule_v6[i].flt_rule,
						sizeof(ipa_flt_rule_add));
				}
#else
				memcpy(pFilteringTable_v6->rules, IPACM_Wan::flt_rule_v6, IPACM_Wan::num_v6_flt_rule * sizeof(ipa_flt_rule_add));
#endif
			}
		}
		else	//embms is on, always add 1 embms rule on top of WAN DL flt table
		{
			/* allocate ipv4 filtering table */
			len = sizeof(struct ipa_ioc_add_flt_rule) + (1 + IPACM_Wan::num_v4_flt_rule) * sizeof(struct ipa_flt_rule_add);
			pFilteringTable_v4 = (struct ipa_ioc_add_flt_rule*)malloc(len);
			IPACMDBG_H("Total number of WAN DL filtering rule for IPv4 is %d\n", IPACM_Wan::num_v4_flt_rule + 1);
			if (pFilteringTable_v4 == NULL)
			{
				IPACMERR("Error Locate ipa_flt_rule_add memory...\n");
				return IPACM_FAILURE;
			}
			memset(pFilteringTable_v4, 0, len);
#ifdef FEATURE_VLAN_MPDN
			mux_id_v4 = (uint8_t*)malloc((IPACM_Wan::num_v4_flt_rule + 1) * sizeof(uint8_t));
			if(mux_id_v4 == NULL)
			{
				IPACMERR("Error allocate mux_id_v4 memory...\n");
				return IPACM_FAILURE;
			}
#endif
			pFilteringTable_v4->commit = 1;
			pFilteringTable_v4->ep = rx_prop->rx[0].src_pipe;
			pFilteringTable_v4->global = false;
			pFilteringTable_v4->ip = IPA_IP_v4;
			pFilteringTable_v4->num_rules = (uint8_t)IPACM_Wan::num_v4_flt_rule + 1;

			/* allocate ipv6 filtering table */
			len = sizeof(struct ipa_ioc_add_flt_rule) + (1 + IPACM_Wan::num_v6_flt_rule) * sizeof(struct ipa_flt_rule_add);
			pFilteringTable_v6 = (struct ipa_ioc_add_flt_rule*)malloc(len);
			IPACMDBG_H("Total number of WAN DL filtering rule for IPv6 is %d\n", IPACM_Wan::num_v6_flt_rule + 1);
			if (pFilteringTable_v6 == NULL)
			{
				IPACMERR("Error Locate ipa_flt_rule_add memory...\n");
				free(pFilteringTable_v4);
				return IPACM_FAILURE;
			}
			memset(pFilteringTable_v6, 0, len);
			pFilteringTable_v6->commit = 1;
			pFilteringTable_v6->ep = rx_prop->rx[0].src_pipe;
			pFilteringTable_v6->global = false;
			pFilteringTable_v6->ip = IPA_IP_v6;
			pFilteringTable_v6->num_rules = (uint8_t)IPACM_Wan::num_v6_flt_rule + 1;

			config_dft_embms_rules(pFilteringTable_v4, pFilteringTable_v6);
			if(IPACM_Wan::num_v4_flt_rule > 0)
			{
#ifdef FEATURE_VLAN_MPDN
				/* embms get's the mux ID of the default PDN */
				mux_id_v4[0] = IPACM_Iface::ipacmcfg->GetQmapId();
				for(int i = 1; i < IPACM_Wan::num_v4_flt_rule + 1; i++)
				{
					mux_id_v4[i] = IPACM_Wan::pdn_flt_rule_v4[i].mux_id;
					memcpy(&pFilteringTable_v4->rules[i],
						&IPACM_Wan::pdn_flt_rule_v4[i].flt_rule,
						sizeof(ipa_flt_rule_add));
				}
#else
				memcpy(&(pFilteringTable_v4->rules[1]), IPACM_Wan::flt_rule_v4, IPACM_Wan::num_v4_flt_rule * sizeof(ipa_flt_rule_add));
#endif
			}

			if(IPACM_Wan::num_v6_flt_rule > 0)
			{
#ifdef FEATURE_VLAN_MPDN
				/* embms get's the mux ID of the default PDN */
				if (mux_id_v6 != NULL) {
					mux_id_v6[0] = IPACM_Iface::ipacmcfg->GetQmapId();
					for(int i = 1; i < IPACM_Wan::num_v6_flt_rule + 1; i++)
					{
						mux_id_v6[i] = IPACM_Wan::pdn_flt_rule_v6[i].mux_id;
						memcpy(&pFilteringTable_v6->rules[i],
							&IPACM_Wan::pdn_flt_rule_v6[i].flt_rule,
							sizeof(ipa_flt_rule_add));
					}
#else
					memcpy(&(pFilteringTable_v6->rules[1]), IPACM_Wan::flt_rule_v6, IPACM_Wan::num_v6_flt_rule * sizeof(ipa_flt_rule_add));
#endif
				}
			}
		}
	}

#ifdef FEATURE_VLAN_MPDN
	if(false == m_filtering.AddWanDLFilteringRule(pFilteringTable_v4, pFilteringTable_v6, mux_id_v4, mux_id_v6))
#else
	if(false == m_filtering.AddWanDLFilteringRule(pFilteringTable_v4, pFilteringTable_v6, mux_id))
#endif
	{
		IPACMERR("Failed to install WAN DL filtering table.\n");
		res = IPACM_FAILURE;
		goto fail;
	}

fail:
	if(pFilteringTable_v4 != NULL)
	{
		free(pFilteringTable_v4);
	}
	if(pFilteringTable_v6 != NULL)
	{
		free(pFilteringTable_v6);
	}
#ifdef FEATURE_VLAN_MPDN
	if(mux_id_v4)
	{
		free(mux_id_v4);
	}
	if(mux_id_v6)
	{
		free(mux_id_v6);
	}
#endif
	return res;
}

void IPACM_Wan::change_to_network_order(ipa_ip_type iptype, ipa_rule_attrib* attrib)
{
	if(attrib == NULL)
	{
		IPACMERR("Attribute pointer is NULL.\n");
		return;
	}

	if(iptype == IPA_IP_v6)
	{
		int i;
		for(i=0; i<4; i++)
		{
			attrib->u.v6.src_addr[i] = htonl(attrib->u.v6.src_addr[i]);
			attrib->u.v6.src_addr_mask[i] = htonl(attrib->u.v6.src_addr_mask[i]);
			attrib->u.v6.dst_addr[i] = htonl(attrib->u.v6.dst_addr[i]);
			attrib->u.v6.dst_addr_mask[i] = htonl(attrib->u.v6.dst_addr_mask[i]);
		}
	}
	else
	{
		IPACMDBG_H("IP type is not IPv6, do nothing: %d\n", iptype);
	}

	return;
}

bool IPACM_Wan::is_global_ipv6_addr(uint32_t* ipv6_addr)
{
	if(ipv6_addr == NULL)
	{
		IPACMERR("IPv6 address is empty.\n");
		return false;
	}
	IPACMDBG_H("Get ipv6 address with first word 0x%08x.\n", ipv6_addr[0]);

	uint32_t ipv6_link_local_prefix, ipv6_link_local_prefix_mask;
	ipv6_link_local_prefix = 0xFE800000;
	ipv6_link_local_prefix_mask = 0xFFC00000;
	if((ipv6_addr[0] & ipv6_link_local_prefix_mask) == (ipv6_link_local_prefix & ipv6_link_local_prefix_mask))
	{
		IPACMDBG_H("This IPv6 address is link local.\n");
		return false;
	}
	else
	{
		IPACMDBG_H("This IPv6 address is not link local.\n");
		return true;
	}
}

/* handle STA WAN-client */
/* handle WAN client initial, construct full headers (tx property) */
int IPACM_Wan::handle_wan_hdr_init(uint8_t *mac_addr, bool gw_addr)
{

#define WAN_IFACE_INDEX_LEN 2

	int res = IPACM_SUCCESS, len = 0;
	char index[WAN_IFACE_INDEX_LEN];
	struct ipa_ioc_copy_hdr sCopyHeader;
	struct ipa_ioc_add_hdr *pHeaderDescriptor = NULL;
	uint32_t cnt;
	int clnt_indx;

	clnt_indx = get_wan_client_index(mac_addr);

	if (clnt_indx != IPACM_INVALID_INDEX)
	{
		IPACMERR("eth client is found/attached already with index %d \n", clnt_indx);
		return IPACM_FAILURE;
	}

	/* add header to IPA */
	if (num_wan_client >= IPA_MAX_NUM_WAN_CLIENTS)
	{
		IPACMERR("Reached maximum number(%d) of eth clients\n", IPA_MAX_NUM_WAN_CLIENTS);
		return IPACM_FAILURE;
	}

	/* Reserve entry for storing the GW address. */
	if ((num_wan_client >= (IPA_MAX_NUM_WAN_CLIENTS - 1)) && !gw_addr)
	{
		IPACMERR("Reached maximum number(%d) of eth clients without GW address\n", num_wan_client);
		return IPACM_FAILURE;
	}

	IPACMDBG_H("WAN client number: %d\n", num_wan_client);

	memcpy(get_client_memptr(wan_client, num_wan_client)->mac,
				 mac_addr,
				 sizeof(get_client_memptr(wan_client, num_wan_client)->mac));

	IPACMDBG_H("Received Client MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					 mac_addr[0], mac_addr[1], mac_addr[2],
					 mac_addr[3], mac_addr[4], mac_addr[5]);

	IPACMDBG_H("stored MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					 get_client_memptr(wan_client, num_wan_client)->mac[0],
					 get_client_memptr(wan_client, num_wan_client)->mac[1],
					 get_client_memptr(wan_client, num_wan_client)->mac[2],
					 get_client_memptr(wan_client, num_wan_client)->mac[3],
					 get_client_memptr(wan_client, num_wan_client)->mac[4],
					 get_client_memptr(wan_client, num_wan_client)->mac[5]);

	/* add header to IPA */
	if(tx_prop != NULL)
	{
		len = sizeof(struct ipa_ioc_add_hdr) + (1 * sizeof(struct ipa_hdr_add));
		pHeaderDescriptor = (struct ipa_ioc_add_hdr *)calloc(1, len);
		if (pHeaderDescriptor == NULL)
		{
			IPACMERR("calloc failed to allocate pHeaderDescriptor\n");
			return IPACM_FAILURE;
		}

		/* copy partial header for v4*/
		for (cnt=0; cnt<tx_prop->num_tx_props; cnt++)
		{
				 if(tx_prop->tx[cnt].ip==IPA_IP_v4)
				 {
								IPACMDBG_H("Got partial v4-header name from %d tx props\n", cnt);
								memset(&sCopyHeader, 0, sizeof(sCopyHeader));
								memcpy(sCopyHeader.name,
											 tx_prop->tx[cnt].hdr_name,
											 sizeof(sCopyHeader.name));

								IPACMDBG_H("header name: %s in tx:%d\n", sCopyHeader.name,cnt);
								if (m_header.CopyHeader(&sCopyHeader) == false)
								{
									PERROR("ioctl copy header failed");
									res = IPACM_FAILURE;
									goto fail;
								}

								IPACMDBG_H("header length: %d, paritial: %d\n", sCopyHeader.hdr_len, sCopyHeader.is_partial);
								IPACMDBG_H("header eth2_ofst_valid: %d, eth2_ofst: %d\n", sCopyHeader.is_eth2_ofst_valid, sCopyHeader.eth2_ofst);
								if (sCopyHeader.hdr_len > IPA_HDR_MAX_SIZE)
								{
									IPACMERR("header oversize\n");
									res = IPACM_FAILURE;
									goto fail;
								}
								else
								{
									memcpy(pHeaderDescriptor->hdr[0].hdr,
												 sCopyHeader.hdr,
												 sCopyHeader.hdr_len);
								}

								/* copy client mac_addr to partial header */
								IPACMDBG_H("header eth2_ofst_valid: %d, eth2_ofst: %d\n",
										sCopyHeader.is_eth2_ofst_valid, sCopyHeader.eth2_ofst);

								/* only copy 6 bytes mac-address */
								if(sCopyHeader.is_eth2_ofst_valid == false)
								{
									memcpy(&pHeaderDescriptor->hdr[0].hdr[0],
											mac_addr, IPA_MAC_ADDR_SIZE);
								}
								else
								{
									memcpy(&pHeaderDescriptor->hdr[0].hdr[sCopyHeader.eth2_ofst],
											mac_addr, IPA_MAC_ADDR_SIZE);
								}


								pHeaderDescriptor->commit = true;
								pHeaderDescriptor->num_hdrs = 1;

								memset(pHeaderDescriptor->hdr[0].name, 0,
											 sizeof(pHeaderDescriptor->hdr[0].name));

								snprintf(index,sizeof(index), "%d", ipa_if_num);
								strlcpy(pHeaderDescriptor->hdr[0].name, index, sizeof(pHeaderDescriptor->hdr[0].name));
								pHeaderDescriptor->hdr[0].name[IPA_RESOURCE_NAME_MAX-1] = '\0';
								if (strlcat(pHeaderDescriptor->hdr[0].name, IPA_WAN_PARTIAL_HDR_NAME_v4, sizeof(pHeaderDescriptor->hdr[0].name)) > IPA_RESOURCE_NAME_MAX)
								{
									IPACMERR(" header name construction failed exceed length (%d)\n", strlen(pHeaderDescriptor->hdr[0].name));
									res = IPACM_FAILURE;
									goto fail;
								}

								snprintf(index,sizeof(index), "%d", header_name_count);
								if (strlcat(pHeaderDescriptor->hdr[0].name, index, sizeof(pHeaderDescriptor->hdr[0].name)) > IPA_RESOURCE_NAME_MAX)
								{
									IPACMERR(" header name construction failed exceed length (%d)\n", strlen(pHeaderDescriptor->hdr[0].name));
									res = IPACM_FAILURE;
									goto fail;
								}

								pHeaderDescriptor->hdr[0].hdr_len = sCopyHeader.hdr_len;
								pHeaderDescriptor->hdr[0].hdr_hdl = -1;
								pHeaderDescriptor->hdr[0].is_partial = 0;
								pHeaderDescriptor->hdr[0].status = -1;

					 if (m_header.AddHeader(pHeaderDescriptor) == false ||
							pHeaderDescriptor->hdr[0].status != 0)
					 {
						IPACMERR("ioctl IPA_IOC_ADD_HDR failed: %d\n", pHeaderDescriptor->hdr[0].status);
						res = IPACM_FAILURE;
						goto fail;
					 }

					get_client_memptr(wan_client, num_wan_client)->hdr_hdl_v4 = pHeaderDescriptor->hdr[0].hdr_hdl;
					IPACMDBG_H("eth-client(%d) v4 full header name:%s header handle:(0x%x)\n",
												 num_wan_client,
												 pHeaderDescriptor->hdr[0].name,
												 get_client_memptr(wan_client, num_wan_client)->hdr_hdl_v4);
									get_client_memptr(wan_client, num_wan_client)->ipv4_header_set=true;

					break;
				 }
		}


		/* copy partial header for v6*/
		for (cnt=0; cnt<tx_prop->num_tx_props; cnt++)
		{
			if(tx_prop->tx[cnt].ip==IPA_IP_v6)
			{

				IPACMDBG_H("Got partial v6-header name from %d tx props\n", cnt);
				memset(&sCopyHeader, 0, sizeof(sCopyHeader));
				memcpy(sCopyHeader.name,
						tx_prop->tx[cnt].hdr_name,
							sizeof(sCopyHeader.name));

				IPACMDBG_H("header name: %s in tx:%d\n", sCopyHeader.name,cnt);
				if (m_header.CopyHeader(&sCopyHeader) == false)
				{
					PERROR("ioctl copy header failed");
					res = IPACM_FAILURE;
					goto fail;
				}

				IPACMDBG_H("header length: %d, paritial: %d\n", sCopyHeader.hdr_len, sCopyHeader.is_partial);
				IPACMDBG_H("header eth2_ofst_valid: %d, eth2_ofst: %d\n", sCopyHeader.is_eth2_ofst_valid, sCopyHeader.eth2_ofst);
				if (sCopyHeader.hdr_len > IPA_HDR_MAX_SIZE)
				{
					IPACMERR("header oversize\n");
					res = IPACM_FAILURE;
					goto fail;
				}
				else
				{
					memcpy(pHeaderDescriptor->hdr[0].hdr,
							sCopyHeader.hdr,
							sCopyHeader.hdr_len);
				}

				/* copy client mac_addr to partial header */
				if(sCopyHeader.is_eth2_ofst_valid == false)
				{
					memcpy(&pHeaderDescriptor->hdr[0].hdr[0],
								 mac_addr, IPA_MAC_ADDR_SIZE); /* only copy 6 bytes mac-address */
				}
				else
				{
					memcpy(&pHeaderDescriptor->hdr[0].hdr[sCopyHeader.eth2_ofst],
								 mac_addr, IPA_MAC_ADDR_SIZE); /* only copy 6 bytes mac-address */
				}


				pHeaderDescriptor->commit = true;
				pHeaderDescriptor->num_hdrs = 1;

				memset(pHeaderDescriptor->hdr[0].name, 0,
					 sizeof(pHeaderDescriptor->hdr[0].name));

				snprintf(index,sizeof(index), "%d", ipa_if_num);
				strlcpy(pHeaderDescriptor->hdr[0].name, index, sizeof(pHeaderDescriptor->hdr[0].name));
				pHeaderDescriptor->hdr[0].name[IPA_RESOURCE_NAME_MAX-1] = '\0';
				if (strlcat(pHeaderDescriptor->hdr[0].name, IPA_WAN_PARTIAL_HDR_NAME_v6, sizeof(pHeaderDescriptor->hdr[0].name)) > IPA_RESOURCE_NAME_MAX)
				{
					IPACMERR(" header name construction failed exceed length (%d)\n", strlen(pHeaderDescriptor->hdr[0].name));
					res = IPACM_FAILURE;
					goto fail;
				}
				snprintf(index,sizeof(index), "%d", header_name_count);
				if (strlcat(pHeaderDescriptor->hdr[0].name, index, sizeof(pHeaderDescriptor->hdr[0].name)) > IPA_RESOURCE_NAME_MAX)
				{
					IPACMERR(" header name construction failed exceed length (%d)\n", strlen(pHeaderDescriptor->hdr[0].name));
					res = IPACM_FAILURE;
					goto fail;
				}

				pHeaderDescriptor->hdr[0].hdr_len = sCopyHeader.hdr_len;
				pHeaderDescriptor->hdr[0].hdr_hdl = -1;
				pHeaderDescriptor->hdr[0].is_partial = 0;
				pHeaderDescriptor->hdr[0].status = -1;

				if (m_header.AddHeader(pHeaderDescriptor) == false ||
						pHeaderDescriptor->hdr[0].status != 0)
				{
					IPACMERR("ioctl IPA_IOC_ADD_HDR failed: %d\n", pHeaderDescriptor->hdr[0].status);
					res = IPACM_FAILURE;
					goto fail;
				}

				get_client_memptr(wan_client, num_wan_client)->hdr_hdl_v6 = pHeaderDescriptor->hdr[0].hdr_hdl;
				IPACMDBG_H("eth-client(%d) v6 full header name:%s header handle:(0x%x)\n",
						 num_wan_client,
						 pHeaderDescriptor->hdr[0].name,
									 get_client_memptr(wan_client, num_wan_client)->hdr_hdl_v6);

									get_client_memptr(wan_client, num_wan_client)->ipv6_header_set=true;

				break;

			}
		}
		/* initialize wifi client*/
		get_client_memptr(wan_client, num_wan_client)->route_rule_set_v4 = false;
		get_client_memptr(wan_client, num_wan_client)->route_rule_set_v6 = 0;
		get_client_memptr(wan_client, num_wan_client)->ipv4_set = false;
		get_client_memptr(wan_client, num_wan_client)->ipv6_set = 0;
		num_wan_client++;
		header_name_count++; //keep increasing header_name_count
		res = IPACM_SUCCESS;
		IPACMDBG_H("eth client number: %d\n", num_wan_client);
	}
	else
	{
		return res;
	}
fail:
	free(pHeaderDescriptor);

	return res;
}

/*handle eth client */
int IPACM_Wan::handle_wan_client_ipaddr(ipacm_event_data_all *data)
{
	int clnt_indx;
	int v6_num;

	IPACMDBG_H("number of wan clients: %d\n", num_wan_client);
	IPACMDBG_H(" event MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					 data->mac_addr[0],
					 data->mac_addr[1],
					 data->mac_addr[2],
					 data->mac_addr[3],
					 data->mac_addr[4],
					 data->mac_addr[5]);

	clnt_indx = get_wan_client_index(data->mac_addr);

		if (clnt_indx == IPACM_INVALID_INDEX)
		{
			IPACMERR("wan client not found/attached \n");
			return IPACM_FAILURE;
		}

	IPACMDBG_H("Ip-type received %d\n", data->iptype);
	if (data->iptype == IPA_IP_v4)
	{
		IPACMDBG_H("ipv4 address: 0x%x\n", data->ipv4_addr);
		if (data->ipv4_addr != 0) /* not 0.0.0.0 */
		{
			if (get_client_memptr(wan_client, clnt_indx)->ipv4_set == false)
			{
				get_client_memptr(wan_client, clnt_indx)->v4_addr = data->ipv4_addr;
				get_client_memptr(wan_client, clnt_indx)->ipv4_set = true;
				/* Add NAT rules after ipv4 RT rules are set */
				CtList->HandleSTAClientAddEvt(data->ipv4_addr);
			}
			else
			{
			   /* check if client got new IPv4 address*/
			   if(data->ipv4_addr == get_client_memptr(wan_client, clnt_indx)->v4_addr)
			   {
			     IPACMDBG_H("Already setup ipv4 addr for client:%d, ipv4 address didn't change\n", clnt_indx);
				 return IPACM_FAILURE;
			   }
			   else
			   {
					IPACMDBG_H("ipv4 addr for client:%d is changed \n", clnt_indx);
					/* Del NAT rules before ipv4 RT rules are delete */
					CtList->HandleSTAClientDelEvt(get_client_memptr(wan_client, clnt_indx)->v4_addr);
					delete_wan_rtrules(clnt_indx,IPA_IP_v4);
					get_client_memptr(wan_client, clnt_indx)->route_rule_set_v4 = false;
					get_client_memptr(wan_client, clnt_indx)->v4_addr = data->ipv4_addr;
					/* Add NAT rules after ipv4 RT rules are set */
					CtList->HandleSTAClientAddEvt(data->ipv4_addr);
				}
			}
		}
		else
		{
				IPACMDBG_H("Invalid client IPv4 address \n");
				return IPACM_FAILURE;
		}
	}
	else
	{
		/* check if all 0 not valid ipv6 address */
		if (data->ipv6_addr[0] || data->ipv6_addr[1] || data->ipv6_addr[2] || data->ipv6_addr[3])
		{
			IPACMDBG_H("ipv6 address: 0x%x:%x:%x:%x\n", data->ipv6_addr[0], data->ipv6_addr[1], data->ipv6_addr[2], data->ipv6_addr[3]);
			if (get_client_memptr(wan_client, clnt_indx)->ipv6_set < IPV6_NUM_ADDR)
			{

				for (v6_num = 0; v6_num < get_client_memptr(wan_client, clnt_indx)->ipv6_set; v6_num++)
				{
					if (data->ipv6_addr[0] == get_client_memptr(wan_client, clnt_indx)->v6_addr[v6_num][0] &&
						data->ipv6_addr[1] == get_client_memptr(wan_client, clnt_indx)->v6_addr[v6_num][1] &&
						data->ipv6_addr[2] == get_client_memptr(wan_client, clnt_indx)->v6_addr[v6_num][2] &&
						data->ipv6_addr[3] == get_client_memptr(wan_client, clnt_indx)->v6_addr[v6_num][3])
					{
						IPACMDBG_H("Already see this ipv6 addr for client:%d\n", clnt_indx);
						return IPACM_FAILURE; /* not setup the RT rules*/
					}
				}

				/*
				 * The client got new IPv6 address.
				 * NOTE: The new address doesn't replace the existing one but being added (up to IPV6_NUM_ADDR),
				 *       so the previous IPv6 addresses of the client will not be deleted.
				 */
				get_client_memptr(wan_client, clnt_indx)->v6_addr[get_client_memptr(wan_client, clnt_indx)->ipv6_set][0] = data->ipv6_addr[0];
				get_client_memptr(wan_client, clnt_indx)->v6_addr[get_client_memptr(wan_client, clnt_indx)->ipv6_set][1] = data->ipv6_addr[1];
				get_client_memptr(wan_client, clnt_indx)->v6_addr[get_client_memptr(wan_client, clnt_indx)->ipv6_set][2] = data->ipv6_addr[2];
				get_client_memptr(wan_client, clnt_indx)->v6_addr[get_client_memptr(wan_client, clnt_indx)->ipv6_set][3] = data->ipv6_addr[3];
				get_client_memptr(wan_client, clnt_indx)->ipv6_set++;

				CtList->HandleSTAClientAddEvt_v6(Ipv6IpAddress(data->ipv6_addr, false));
			}
			else
			{
				IPACMDBG_H("Already got 3 ipv6 addr for client:%d\n", clnt_indx);
				return IPACM_FAILURE; /* not setup the RT rules*/
			}
		}
	}

	return IPACM_SUCCESS;
}

/*handle wan client routing rule*/
int IPACM_Wan::handle_wan_client_route_rule(uint8_t *mac_addr, ipa_ip_type iptype)
{
	struct ipa_ioc_add_rt_rule *rt_rule;
	struct ipa_rt_rule_add *rt_rule_entry;
	uint32_t tx_index;
	int wan_index,v6_num;
	const int NUM = 1;

	if(tx_prop == NULL)
	{
		IPACMDBG_H("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	IPACMDBG_H("Received mac_addr MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
			mac_addr[0], mac_addr[1], mac_addr[2],
			mac_addr[3], mac_addr[4], mac_addr[5]);

	wan_index = get_wan_client_index(mac_addr);
	if (wan_index == IPACM_INVALID_INDEX)
	{
		IPACMDBG_H("wan client not found/attached \n");
		return IPACM_SUCCESS;
	}

	if (iptype==IPA_IP_v4) {
		IPACMDBG_H("wan client index: %d, ip-type: %d, ipv4_set:%d, ipv4_rule_set:%d \n", wan_index, iptype,
				get_client_memptr(wan_client, wan_index)->ipv4_set,
				get_client_memptr(wan_client, wan_index)->route_rule_set_v4);
	} else {
		IPACMDBG_H("wan client index: %d, ip-type: %d, ipv6_set:%d, ipv6_rule_num:%d \n", wan_index, iptype,
				get_client_memptr(wan_client, wan_index)->ipv6_set,
				get_client_memptr(wan_client, wan_index)->route_rule_set_v6);
	}

	/* Add default routing rules if not set yet */
	if ((iptype == IPA_IP_v4
				&& get_client_memptr(wan_client, wan_index)->route_rule_set_v4 == false
				&& get_client_memptr(wan_client, wan_index)->ipv4_set == true)
			|| (iptype == IPA_IP_v6
				&& get_client_memptr(wan_client, wan_index)->route_rule_set_v6 < get_client_memptr(wan_client, wan_index)->ipv6_set
			   ))
	{
		if(IPACM_Iface::ipacmcfg->GetIPAVer() >= IPA_HW_None && IPACM_Iface::ipacmcfg->GetIPAVer() < IPA_HW_v4_0)
		{
			/* Add corresponding ipa_rm_resource_name of TX-endpoint up before IPV6 RT-rule set */
			IPACMDBG_H("dev %s add producer dependency\n", dev_name);
			IPACMDBG_H("depend Got pipe %d rm index : %d \n", tx_prop->tx[0].dst_pipe, IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
			IPACM_Iface::ipacmcfg->AddRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe],false);
		}
		rt_rule = (struct ipa_ioc_add_rt_rule *)
			calloc(1, sizeof(struct ipa_ioc_add_rt_rule) +
					NUM * sizeof(struct ipa_rt_rule_add));

		if (rt_rule == NULL)
		{
			PERROR("Error Locate ipa_ioc_add_rt_rule memory...\n");
			return IPACM_FAILURE;
		}

		rt_rule->commit = 1;
		rt_rule->num_rules = (uint8_t)NUM;
		rt_rule->ip = iptype;

		for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
		{
			if(iptype != tx_prop->tx[tx_index].ip)
			{
				IPACMDBG_H("Tx:%d, ip-type: %d conflict ip-type: %d no RT-rule added\n",
						tx_index, tx_prop->tx[tx_index].ip,iptype);
				continue;
			}

			rt_rule_entry = &rt_rule->rules[0];
			rt_rule_entry->at_rear = 0;

			if (iptype == IPA_IP_v4)
			{
				IPACMDBG_H("client index(%d):ipv4 address: 0x%x\n", wan_index,
						get_client_memptr(wan_client, wan_index)->v4_addr);

				IPACMDBG_H("client(%d): v4 header handle:(0x%x)\n",
						wan_index,
						get_client_memptr(wan_client, wan_index)->hdr_hdl_v4);
				strlcpy(rt_rule->rt_tbl_name,
						IPACM_Iface::ipacmcfg->rt_tbl_wan_v4.name,
						sizeof(rt_rule->rt_tbl_name));
				rt_rule->rt_tbl_name[IPA_RESOURCE_NAME_MAX-1] = '\0';
				if (IPACM_Iface::ipacmcfg->isMCC_Mode == true)
				{
					IPACMDBG_H("In MCC mode, use alt dst pipe: %d\n",
							tx_prop->tx[tx_index].alt_dst_pipe);
					rt_rule_entry->rule.dst = tx_prop->tx[tx_index].alt_dst_pipe;
				}
				else
				{
					rt_rule_entry->rule.dst = tx_prop->tx[tx_index].dst_pipe;
				}
				memcpy(&rt_rule_entry->rule.attrib,
						&tx_prop->tx[tx_index].attrib,
						sizeof(rt_rule_entry->rule.attrib));
				rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
				rt_rule_entry->rule.hdr_hdl = get_client_memptr(wan_client, wan_index)->hdr_hdl_v4;
				rt_rule_entry->rule.attrib.u.v4.dst_addr = get_client_memptr(wan_client, wan_index)->v4_addr;
				rt_rule_entry->rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;
#ifdef FEATURE_IPA_V3
				rt_rule_entry->rule.hashable = true;
#endif
				if (false == m_routing.AddRoutingRule(rt_rule))
				{
					IPACMERR("Routing rule addition failed!\n");
					free(rt_rule);
					return IPACM_FAILURE;
				}

				/* copy ipv4 RT hdl */
				get_client_memptr(wan_client, wan_index)->wan_rt_hdl[tx_index].wan_rt_rule_hdl_v4 =
					rt_rule->rules[0].rt_rule_hdl;
				IPACMDBG_H("tx:%d, rt rule hdl=%x ip-type: %d\n", tx_index,
						get_client_memptr(wan_client, wan_index)->wan_rt_hdl[tx_index].wan_rt_rule_hdl_v4, iptype);
			} else {

				for(v6_num = get_client_memptr(wan_client, wan_index)->route_rule_set_v6;v6_num < get_client_memptr(wan_client, wan_index)->ipv6_set;v6_num++)
				{
					IPACMDBG_H("client(%d): v6 header handle:(0x%x)\n",
							wan_index,
							get_client_memptr(wan_client, wan_index)->hdr_hdl_v6);

					/* v6 LAN_RT_TBL */
					strlcpy(rt_rule->rt_tbl_name,
							IPACM_Iface::ipacmcfg->rt_tbl_v6.name,
							sizeof(rt_rule->rt_tbl_name));
					rt_rule->rt_tbl_name[IPA_RESOURCE_NAME_MAX-1] = '\0';
					/* Uplink going to wan clients should go to IPA */
					if (IPACM_Iface::ipacmcfg->isMCC_Mode == true)
					{
						IPACMDBG_H("In MCC mode, use alt dst pipe: %d\n",
								tx_prop->tx[tx_index].alt_dst_pipe);
						rt_rule_entry->rule.dst = tx_prop->tx[tx_index].alt_dst_pipe;
					}
					else
					{
						rt_rule_entry->rule.dst = tx_prop->tx[tx_index].dst_pipe;
					}
					memset(&rt_rule_entry->rule.attrib, 0, sizeof(rt_rule_entry->rule.attrib));
					rt_rule_entry->rule.hdr_hdl = get_client_memptr(wan_client, wan_index)->hdr_hdl_v6;;
					rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
					rt_rule_entry->rule.attrib.u.v6.dst_addr[0] = get_client_memptr(wan_client, wan_index)->v6_addr[v6_num][0];
					rt_rule_entry->rule.attrib.u.v6.dst_addr[1] = get_client_memptr(wan_client, wan_index)->v6_addr[v6_num][1];
					rt_rule_entry->rule.attrib.u.v6.dst_addr[2] = get_client_memptr(wan_client, wan_index)->v6_addr[v6_num][2];
					rt_rule_entry->rule.attrib.u.v6.dst_addr[3] = get_client_memptr(wan_client, wan_index)->v6_addr[v6_num][3];
					rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[0] = 0xFFFFFFFF;
					rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[1] = 0xFFFFFFFF;
					rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[2] = 0xFFFFFFFF;
					rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[3] = 0xFFFFFFFF;
#ifdef FEATURE_IPA_V3
					rt_rule_entry->rule.hashable = true;
#endif
					if (false == m_routing.AddRoutingRule(rt_rule))
					{
						IPACMERR("Routing rule addition failed!\n");
						free(rt_rule);
						return IPACM_FAILURE;
					}

					get_client_memptr(wan_client, wan_index)->wan_rt_hdl[tx_index].wan_rt_rule_hdl_v6[v6_num] = rt_rule->rules[0].rt_rule_hdl;
					IPACMDBG_H("tx:%d, rt rule hdl=%x ip-type: %d\n", tx_index,
							get_client_memptr(wan_client, wan_index)->wan_rt_hdl[tx_index].wan_rt_rule_hdl_v6[v6_num], iptype);

					/*Copy same rule to v6 WAN RT TBL*/
					strlcpy(rt_rule->rt_tbl_name,
							IPACM_Iface::ipacmcfg->rt_tbl_wan_v6.name,
							sizeof(rt_rule->rt_tbl_name));
					rt_rule->rt_tbl_name[IPA_RESOURCE_NAME_MAX-1] = '\0';
					/* Downlink traffic from Wan clients, should go exception */
					rt_rule_entry->rule.dst = iface_query->excp_pipe;
					memcpy(&rt_rule_entry->rule.attrib,
							&tx_prop->tx[tx_index].attrib,
							sizeof(rt_rule_entry->rule.attrib));
					rt_rule_entry->rule.hdr_hdl = 0;
					rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
					rt_rule_entry->rule.attrib.u.v6.dst_addr[0] = get_client_memptr(wan_client, wan_index)->v6_addr[v6_num][0];
					rt_rule_entry->rule.attrib.u.v6.dst_addr[1] = get_client_memptr(wan_client, wan_index)->v6_addr[v6_num][1];
					rt_rule_entry->rule.attrib.u.v6.dst_addr[2] = get_client_memptr(wan_client, wan_index)->v6_addr[v6_num][2];
					rt_rule_entry->rule.attrib.u.v6.dst_addr[3] = get_client_memptr(wan_client, wan_index)->v6_addr[v6_num][3];
					rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[0] = 0xFFFFFFFF;
					rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[1] = 0xFFFFFFFF;
					rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[2] = 0xFFFFFFFF;
					rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[3] = 0xFFFFFFFF;
					if (false == m_routing.AddRoutingRule(rt_rule))
					{
						IPACMERR("Routing rule addition failed!\n");
						free(rt_rule);
						return IPACM_FAILURE;
					}

					get_client_memptr(wan_client, wan_index)->wan_rt_hdl[tx_index].wan_rt_rule_hdl_v6_wan[v6_num] = rt_rule->rules[0].rt_rule_hdl;
					IPACMDBG_H("tx:%d, rt rule hdl=%x ip-type: %d\n", tx_index,
							get_client_memptr(wan_client, wan_index)->wan_rt_hdl[tx_index].wan_rt_rule_hdl_v6_wan[v6_num], iptype);
				}
			}

		} /* end of for loop */

		free(rt_rule);

		if (iptype == IPA_IP_v4)
		{
			get_client_memptr(wan_client, wan_index)->route_rule_set_v4 = true;
		}
		else
		{
			get_client_memptr(wan_client, wan_index)->route_rule_set_v6 = get_client_memptr(wan_client, wan_index)->ipv6_set;
		}
	}

	return IPACM_SUCCESS;
}

/* TODO Handle wan client routing rules also */
void IPACM_Wan::handle_wlan_SCC_MCC_switch(bool isSCCMode, ipa_ip_type iptype)
{
	struct ipa_ioc_mdfy_rt_rule *rt_rule = NULL;
	struct ipa_rt_rule_mdfy *rt_rule_entry;
	uint32_t tx_index = 0;

	IPACMDBG("\n");
	if (tx_prop == NULL || is_default_gateway == false)
	{
		IPACMDBG_H("No tx properties or no default route set yet\n");
		return;
	}

	const int NUM = tx_prop->num_tx_props;

	for (tx_index = 0; tx_index < tx_prop->num_tx_props; tx_index++)
	{
		if (tx_prop->tx[tx_index].ip != iptype)
		{
			IPACMDBG_H("Tx:%d, ip-type: %d ip-type not matching: %d Ignore\n",
					tx_index, tx_prop->tx[tx_index].ip, iptype);
			continue;
		}

		if (rt_rule == NULL)
		{
			rt_rule = (struct ipa_ioc_mdfy_rt_rule *)
				calloc(1, sizeof(struct ipa_ioc_mdfy_rt_rule) +
						NUM * sizeof(struct ipa_rt_rule_mdfy));

			if (rt_rule == NULL)
			{
				IPACMERR("Unable to allocate memory for modify rt rule\n");
				return;
			}
			IPACMDBG("Allocated memory for %d rules successfully\n", NUM);

			rt_rule->commit = 1;
			rt_rule->num_rules = 0;
			rt_rule->ip = iptype;
		}

		rt_rule_entry = &rt_rule->rules[rt_rule->num_rules];

		memcpy(&rt_rule_entry->rule.attrib,
				&tx_prop->tx[tx_index].attrib,
				sizeof(rt_rule_entry->rule.attrib));
		rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;

		if (iptype == IPA_IP_v4)
		{
			rt_rule_entry->rule.attrib.u.v4.dst_addr      = 0;
			rt_rule_entry->rule.attrib.u.v4.dst_addr_mask = 0;
			rt_rule_entry->rule.hdr_hdl = hdr_hdl_sta_v4;
			rt_rule_entry->rt_rule_hdl = wan_route_rule_v4_hdl[tx_index];
		}
		else
		{
			rt_rule_entry->rule.attrib.u.v6.dst_addr[0] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr[1] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr[2] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr[3] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[0] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[1] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[2] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[3] = 0;

			rt_rule_entry->rule.hdr_hdl = hdr_hdl_sta_v6;
			rt_rule_entry->rt_rule_hdl = wan_route_rule_v6_hdl[tx_index];
		}
		IPACMDBG_H("Header handle: 0x%x\n", rt_rule_entry->rule.hdr_hdl);

		if (isSCCMode)
		{
			rt_rule_entry->rule.dst = tx_prop->tx[tx_index].dst_pipe;
		}
		else
		{
			IPACMDBG_H("In MCC mode, use alt dst pipe: %d\n",
					tx_prop->tx[tx_index].alt_dst_pipe);
			rt_rule_entry->rule.dst = tx_prop->tx[tx_index].alt_dst_pipe;
		}

		rt_rule->num_rules++;
	}

	if (rt_rule != NULL)
	{

		if (rt_rule->num_rules > 0)
		{
			if (false == m_routing.ModifyRoutingRule(rt_rule))
			{
				IPACMERR("Routing rule modify failed!\n");
				free(rt_rule);
				return;
			}

			IPACMDBG("Routing rule modified successfully \n");
		}

		free(rt_rule);
	}

	return;
}

void IPACM_Wan::handle_wan_client_SCC_MCC_switch(bool isSCCMode, ipa_ip_type iptype)
{
	struct ipa_ioc_mdfy_rt_rule *rt_rule = NULL;
	struct ipa_rt_rule_mdfy *rt_rule_entry;

	uint32_t tx_index = 0, clnt_index =0;
	int v6_num = 0;
	const int NUM_RULES = 1;

	int size = sizeof(struct ipa_ioc_mdfy_rt_rule) +
		NUM_RULES * sizeof(struct ipa_rt_rule_mdfy);

	IPACMDBG("\n");

	if (tx_prop == NULL || is_default_gateway == false)
	{
		IPACMDBG_H("No tx properties or no default route set yet\n");
		return;
	}

	rt_rule = (struct ipa_ioc_mdfy_rt_rule *)calloc(1, size);
	if (rt_rule == NULL)
	{
		IPACMERR("Unable to allocate memory for modify rt rule\n");
		return;
	}


	for (clnt_index = 0; clnt_index < num_wan_client; clnt_index++)
	{
		if (iptype == IPA_IP_v4)
		{
			IPACMDBG_H("wan client index: %d, ip-type: %d, ipv4_set:%d, ipv4_rule_set:%d \n",
					clnt_index, iptype,
					get_client_memptr(wan_client, clnt_index)->ipv4_set,
					get_client_memptr(wan_client, clnt_index)->route_rule_set_v4);

			if( get_client_memptr(wan_client, clnt_index)->route_rule_set_v4 == false ||
					get_client_memptr(wan_client, clnt_index)->ipv4_set == false)
			{
				continue;
			}

			for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
			{
				if (iptype != tx_prop->tx[tx_index].ip)
				{
					IPACMDBG_H("Tx:%d, ip-type: %d conflict ip-type: %d skip\n",
							tx_index, tx_prop->tx[tx_index].ip, iptype);
					continue;
				}

				memset(rt_rule, 0, size);
				rt_rule->commit = 1;
				rt_rule->num_rules = NUM_RULES;
				rt_rule->ip = iptype;
				rt_rule_entry = &rt_rule->rules[0];

				IPACMDBG_H("client index(%d):ipv4 address: 0x%x\n", clnt_index,
						get_client_memptr(wan_client, clnt_index)->v4_addr);

				IPACMDBG_H("client(%d): v4 header handle:(0x%x)\n",
						clnt_index,
						get_client_memptr(wan_client, clnt_index)->hdr_hdl_v4);

				if (IPACM_Iface::ipacmcfg->isMCC_Mode == true)
				{
					IPACMDBG_H("In MCC mode, use alt dst pipe: %d\n",
							tx_prop->tx[tx_index].alt_dst_pipe);
					rt_rule_entry->rule.dst = tx_prop->tx[tx_index].alt_dst_pipe;
				}
				else
				{
					rt_rule_entry->rule.dst = tx_prop->tx[tx_index].dst_pipe;
				}

				memcpy(&rt_rule_entry->rule.attrib,
						&tx_prop->tx[tx_index].attrib,
						sizeof(rt_rule_entry->rule.attrib));
				rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;

				rt_rule_entry->rule.hdr_hdl = get_client_memptr(wan_client, clnt_index)->hdr_hdl_v4;
				rt_rule_entry->rule.attrib.u.v4.dst_addr = get_client_memptr(wan_client, clnt_index)->v4_addr;
				rt_rule_entry->rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;

				/* copy ipv4 RT rule hdl */
				IPACMDBG_H("rt rule hdl=%x\n",
						get_client_memptr(wan_client, clnt_index)->wan_rt_hdl[tx_index].wan_rt_rule_hdl_v4);

				rt_rule_entry->rt_rule_hdl =
					get_client_memptr(wan_client, clnt_index)->wan_rt_hdl[tx_index].wan_rt_rule_hdl_v4;

				if (false == m_routing.ModifyRoutingRule(rt_rule))
				{
					IPACMERR("Routing rule modify failed!\n");
					free(rt_rule);
					return;
				}
			}
		}
		else
		{
			IPACMDBG_H("wan client index: %d, ip-type: %d, ipv6_set:%d, ipv6_rule_num:%d \n", clnt_index, iptype,
					get_client_memptr(wan_client, clnt_index)->ipv6_set,
					get_client_memptr(wan_client, clnt_index)->route_rule_set_v6);

			if( get_client_memptr(wan_client, clnt_index)->route_rule_set_v6 == 0)
			{
				continue;
			}

			for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
			{
				if (iptype != tx_prop->tx[tx_index].ip)
				{
					IPACMDBG_H("Tx:%d, ip-type: %d conflict ip-type: %d skip\n",
							tx_index, tx_prop->tx[tx_index].ip, iptype);
					continue;
				}

				memset(rt_rule, 0, size);
				rt_rule->commit = 1;
				rt_rule->num_rules = NUM_RULES;
				rt_rule->ip = iptype;
				rt_rule_entry = &rt_rule->rules[0];

				/* Modify only rules in v6 WAN RT TBL*/
				for (v6_num = 0;
						v6_num < get_client_memptr(wan_client, clnt_index)->route_rule_set_v6;
						v6_num++)
				{
					IPACMDBG_H("client(%d): v6 header handle:(0x%x)\n",
							clnt_index,
							get_client_memptr(wan_client, clnt_index)->hdr_hdl_v6);

					/* Downlink traffic from Wan iface, directly through IPA */
					if (IPACM_Iface::ipacmcfg->isMCC_Mode == true)
					{
						IPACMDBG_H("In MCC mode, use alt dst pipe: %d\n",
								tx_prop->tx[tx_index].alt_dst_pipe);
						rt_rule_entry->rule.dst = tx_prop->tx[tx_index].alt_dst_pipe;
					}
					else
					{
						rt_rule_entry->rule.dst = tx_prop->tx[tx_index].dst_pipe;
					}

					memcpy(&rt_rule_entry->rule.attrib,
							&tx_prop->tx[tx_index].attrib,
							sizeof(rt_rule_entry->rule.attrib));

					rt_rule_entry->rule.hdr_hdl = get_client_memptr(wan_client, clnt_index)->hdr_hdl_v6;
					rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
					rt_rule_entry->rule.attrib.u.v6.dst_addr[0] = get_client_memptr(wan_client, clnt_index)->v6_addr[v6_num][0];
					rt_rule_entry->rule.attrib.u.v6.dst_addr[1] = get_client_memptr(wan_client, clnt_index)->v6_addr[v6_num][1];
					rt_rule_entry->rule.attrib.u.v6.dst_addr[2] = get_client_memptr(wan_client, clnt_index)->v6_addr[v6_num][2];
					rt_rule_entry->rule.attrib.u.v6.dst_addr[3] = get_client_memptr(wan_client, clnt_index)->v6_addr[v6_num][3];
					rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[0] = 0xFFFFFFFF;
					rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[1] = 0xFFFFFFFF;
					rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[2] = 0xFFFFFFFF;
					rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[3] = 0xFFFFFFFF;

					IPACMDBG_H("rt rule hdl=%x\n",
							get_client_memptr(wan_client, clnt_index)->wan_rt_hdl[tx_index].wan_rt_rule_hdl_v6_wan[v6_num]);

					rt_rule_entry->rt_rule_hdl =
						get_client_memptr(wan_client, clnt_index)->wan_rt_hdl[tx_index].wan_rt_rule_hdl_v6_wan[v6_num];

					if (false == m_routing.ModifyRoutingRule(rt_rule))
					{
						IPACMERR("Routing rule Modify failed!\n");
						free(rt_rule);
						return;
					}
				}
			} /* end of for loop */
		}

	}

	free(rt_rule);
	return;
}

/*handle eth client */
int IPACM_Wan::handle_network_stats_update(ipa_get_apn_data_stats_resp_msg_v01 *data)
{
	FILE *fp = NULL;

	for (int apn_index =0; apn_index < data->apn_data_stats_list_len; apn_index++)
	{
		if(data->apn_data_stats_list[apn_index].mux_id == ext_prop->ext[0].mux_id)
		{
			IPACMDBG_H("Received IPA_TETHERING_STATS_UPDATE_NETWORK_STATS, MUX ID %d TX (P%lu/B%lu) RX (P%lu/B%lu)\n",
				data->apn_data_stats_list[apn_index].mux_id,
					data->apn_data_stats_list[apn_index].num_ul_packets,
						data->apn_data_stats_list[apn_index].num_ul_bytes,
							data->apn_data_stats_list[apn_index].num_dl_packets,
								data->apn_data_stats_list[apn_index].num_dl_bytes);
			fp = fopen(IPA_NETWORK_STATS_FILE_NAME, "w");
			if ( fp == NULL )
			{
				IPACMERR("Failed to write pipe stats to %s, error is %d - %s\n",
						IPA_NETWORK_STATS_FILE_NAME, errno, strerror(errno));
				return IPACM_FAILURE;
			}

			fprintf(fp, NETWORK_STATS,
				dev_name,
					data->apn_data_stats_list[apn_index].num_ul_packets,
						data->apn_data_stats_list[apn_index].num_ul_bytes,
							data->apn_data_stats_list[apn_index].num_dl_packets,
								data->apn_data_stats_list[apn_index].num_dl_bytes);
			fclose(fp);
			break;
		};
	}
	return IPACM_SUCCESS;
}

int IPACM_Wan::add_dummy_rx_hdr()
{

#define IFACE_INDEX_LEN 2
	char index[IFACE_INDEX_LEN];
	struct ipa_ioc_add_hdr *pHeaderDescriptor = NULL;
	int len = 0;
	struct ipa_ioc_copy_hdr sCopyHeader;
	struct ipa_hdr_add *ipv6_hdr;
	struct ethhdr *eth_ipv6;
	struct ipa_ioc_add_hdr_proc_ctx* pHeaderProcTable = NULL;
	uint32_t cnt;

	/* get netdev-mac */
	if(tx_prop != NULL)
	{
		/* copy partial header for v6 */
		for (cnt=0; cnt<tx_prop->num_tx_props; cnt++)
		{
				 if(tx_prop->tx[cnt].ip==IPA_IP_v6)
				 {
								IPACMDBG_H("Got partial v6-header name from %d tx props\n", cnt);
								memset(&sCopyHeader, 0, sizeof(sCopyHeader));
								memcpy(sCopyHeader.name,
											 tx_prop->tx[cnt].hdr_name,
											 sizeof(sCopyHeader.name));

								IPACMDBG_H("header name: %s in tx:%d\n", sCopyHeader.name,cnt);
								if (m_header.CopyHeader(&sCopyHeader) == false)
								{
									PERROR("ioctl copy header failed");
									return IPACM_FAILURE;
								}

								IPACMDBG_H("header length: %d, paritial: %d\n", sCopyHeader.hdr_len, sCopyHeader.is_partial);
								IPACMDBG_H("header eth2_ofst_valid: %d, eth2_ofst: %d\n", sCopyHeader.is_eth2_ofst_valid, sCopyHeader.eth2_ofst);
								if (sCopyHeader.hdr_len > IPA_HDR_MAX_SIZE)
								{
									IPACMERR("header oversize\n");
									return IPACM_FAILURE;
								}
								else
								{
									/* copy client mac_addr to partial header */
									IPACMDBG_H("header eth2_ofst_valid: %d, eth2_ofst: %d\n",
											sCopyHeader.is_eth2_ofst_valid, sCopyHeader.eth2_ofst);
									/* only copy 6 bytes mac-address */
									if(sCopyHeader.is_eth2_ofst_valid == false)
									{
										memcpy(netdev_mac, &sCopyHeader.hdr[0+IPA_MAC_ADDR_SIZE],
												sizeof(netdev_mac));
									}
									else
									{
										memcpy(netdev_mac, &sCopyHeader.hdr[sCopyHeader.eth2_ofst+IPA_MAC_ADDR_SIZE],
												sizeof(netdev_mac));
									}
								}
					break;
				}
		}
	}

	len = sizeof(struct ipa_ioc_add_hdr) + (1 * sizeof(struct ipa_hdr_add));
	pHeaderDescriptor = (struct ipa_ioc_add_hdr *)calloc(1, len);
	if (pHeaderDescriptor == NULL)
	{
		IPACMERR("calloc failed to allocate pHeaderDescriptor\n");
		return IPACM_FAILURE;
	}
	ipv6_hdr = &pHeaderDescriptor->hdr[0];
	/* copy ethernet type to header */
	eth_ipv6 = (struct ethhdr *) (ipv6_hdr->hdr +2);
	memcpy(eth_ipv6->h_dest, netdev_mac, ETH_ALEN);
	memcpy(eth_ipv6->h_source, ext_router_mac_addr, ETH_ALEN);
	eth_ipv6->h_proto = htons(ETH_P_IPV6);
	pHeaderDescriptor->commit = true;
	pHeaderDescriptor->num_hdrs = 1;

	memset(ipv6_hdr->name, 0,
			 sizeof(pHeaderDescriptor->hdr[0].name));

	snprintf(index,sizeof(index), "%d", ipa_if_num);
	strlcpy(ipv6_hdr->name, index, sizeof(ipv6_hdr->name));
	ipv6_hdr->name[IPA_RESOURCE_NAME_MAX-1] = '\0';

	if (strlcat(ipv6_hdr->name, IPA_DUMMY_ETH_HDR_NAME_v6, sizeof(ipv6_hdr->name)) > IPA_RESOURCE_NAME_MAX)
	{
		IPACMERR(" header name construction failed exceed length (%d)\n", strlen(ipv6_hdr->name));
		return IPACM_FAILURE;
	}

	ipv6_hdr->hdr_len = ETH_HLEN + 2;
	ipv6_hdr->hdr_hdl = -1;
	ipv6_hdr->is_partial = 0;
	ipv6_hdr->status = -1;
	ipv6_hdr->type = IPA_HDR_L2_ETHERNET_II;

	if (m_header.AddHeader(pHeaderDescriptor) == false ||
			ipv6_hdr->status != 0)
	{
		IPACMERR("ioctl IPA_IOC_ADD_HDR failed: %d\n", ipv6_hdr->status);
		return IPACM_FAILURE;
	}

	hdr_hdl_dummy_v6 = ipv6_hdr->hdr_hdl;
	IPACMDBG_H("dummy v6 full header name:%s header handle:(0x%x)\n",
								 ipv6_hdr->name,
								 hdr_hdl_dummy_v6);
	/* add dummy hdr_proc_hdl */
	len = sizeof(struct ipa_ioc_add_hdr_proc_ctx) + sizeof(struct ipa_hdr_proc_ctx_add);
	pHeaderProcTable = (ipa_ioc_add_hdr_proc_ctx*)malloc(len);
	if(pHeaderProcTable == NULL)
	{
		IPACMERR("Cannot allocate header processing table.\n");
		return IPACM_FAILURE;
	}

	memset(pHeaderProcTable, 0, len);
	pHeaderProcTable->commit = 1;
	pHeaderProcTable->num_proc_ctxs = 1;
	pHeaderProcTable->proc_ctx[0].hdr_hdl = hdr_hdl_dummy_v6;
	if (m_header.AddHeaderProcCtx(pHeaderProcTable) == false)
	{
		IPACMERR("Adding dummy hhdr_proc_hdl failed with status: %d\n", pHeaderProcTable->proc_ctx[0].status);
		return IPACM_FAILURE;
	}
	else
	{
		hdr_proc_hdl_dummy_v6 = pHeaderProcTable->proc_ctx[0].proc_ctx_hdl;
		IPACMDBG_H("dummy hhdr_proc_hdl is added successfully. (0x%x)\n", hdr_proc_hdl_dummy_v6);
	}
	return IPACM_SUCCESS;
}
#ifdef FEATURE_L2TP_E2E
void IPACM_Wan::handle_l2tp_client_add(char *iface_name)
{
	int i;

	if(IPACM_Wan::num_v4_flt_rule >= IPA_MAX_FLT_RULE)
	{
		IPACMERR("Model DL flt rule has reached cap.\n");
		return;
	}

	for (i = IPACM_Wan::num_v4_flt_rule - 1; i >= m_ipv6_default_filterting_rules_count; --i)
	{
		flt_rule_v6[i+1] = flt_rule_v6[i];
	}

	install_l2tp_flt_rule(flt_rule_v6, m_ipv6_default_filterting_rules_count, iface_name);
	IPACM_Wan::num_v6_flt_rule++;
	IPACMDBG_H("Now num of v6 dl flt rule is %d.\n", IPACM_Wan::num_v6_flt_rule);
	return;
}

void IPACM_Wan::handle_l2tp_client_del(char *iface_name)
{
	int i;
	l2tp_vlan_mapping_info info;
	uint32_t ipv6_addr[4];

	if(IPACM_Iface::ipacmcfg->get_vlan_l2tp_mapping(iface_name, info) == IPACM_FAILURE)
	{
		IPACMERR("Failed to find vlan-l2tp mapping.\n");
		return;
	}

	memcpy(ipv6_addr, info.vlan_client_ipv6_addr, sizeof(ipv6_addr));
	for(i=0; i<4; i++)
	{
		ipv6_addr[i] = htonl(ipv6_addr[i]);
	}

	for (i = m_ipv6_default_filterting_rules_count; i < IPACM_Wan::num_v6_flt_rule; ++i)
	{
		if( (flt_rule_v6[i].rule.attrib.attrib_mask | IPA_FLT_DST_ADDR)
			&& memcmp(flt_rule_v6[i].rule.attrib.u.v6.dst_addr, ipv6_addr,
				sizeof(flt_rule_v6[i].rule.attrib.u.v6.dst_addr)) == 0)
		{
			IPACMDBG_H("Found modem DL flt rule at position %d.\n", i);
			break;
		}
	}

	if(i == IPACM_Wan::num_v6_flt_rule)
	{
		IPACMERR("Failed to find the flt rule.\n");
		return;
	}

	for(; i < IPACM_Wan::num_v6_flt_rule - 1; i++)
	{
		flt_rule_v6[i] = flt_rule_v6[i+1];
	}

	IPACM_Wan::num_v6_flt_rule--;
	IPACMDBG_H("Now the num of v6 dl flt rule is %d.\n", IPACM_Wan::num_v6_flt_rule);
	return;
}

void IPACM_Wan::install_l2tp_flt_rule(ipa_flt_rule_add* rules, int rule_offset, char *iface_name)
{
	l2tp_vlan_mapping_info info;
	ipa_flt_rule_add flt_rule_entry;
	ipa_ioc_generate_flt_eq flt_eq;
	ipa_ioc_get_rt_tbl_indx rt_tbl_idx;

	if(IPACM_Iface::ipacmcfg->get_vlan_l2tp_mapping(iface_name, info) == IPACM_FAILURE)
	{
		IPACMERR("Failed to find vlan-l2tp mapping.\n");
		return;
	}

	memset(&rt_tbl_idx, 0, sizeof(rt_tbl_idx));
	strlcpy(rt_tbl_idx.name, IPACM_Iface::ipacmcfg->rt_tbl_wan_v6.name, IPA_RESOURCE_NAME_MAX);
	rt_tbl_idx.name[IPA_RESOURCE_NAME_MAX-1] = '\0';
	rt_tbl_idx.ip = IPA_IP_v6;
	if(ioctl(m_fd_ipa, IPA_IOC_QUERY_RT_TBL_INDEX, &rt_tbl_idx) != 0)
	{
		IPACMERR("Failed to get routing table index from name\n");
		return;
	}

	IPACMDBG_H("WAN DL routing table %s has index %d\n", IPACM_Iface::ipacmcfg->rt_tbl_wan_v6.name, rt_tbl_idx.idx);

	memset(&flt_rule_entry, 0, sizeof(ipa_flt_rule_add));

	flt_rule_entry.at_rear = true;
	flt_rule_entry.flt_rule_hdl = -1;
	flt_rule_entry.status = -1;

	flt_rule_entry.rule.retain_hdr = 0;
	flt_rule_entry.rule.to_uc = 0;
	flt_rule_entry.rule.eq_attrib_type = 1;
	flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
#ifdef FEATURE_IPA_V3
	flt_rule_entry.rule.hashable = true;
#endif
	flt_rule_entry.rule.rt_tbl_idx = rt_tbl_idx.idx;

	/* Configuring dest IP based filtering rule */
	memcpy(&flt_rule_entry.rule.attrib, &rx_prop->rx[1].attrib,
		sizeof(flt_rule_entry.rule.attrib));
	flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
	memcpy(flt_rule_entry.rule.attrib.u.v6.dst_addr, info.vlan_client_ipv6_addr,
		sizeof(flt_rule_entry.rule.attrib.u.v6.dst_addr));
	memset(flt_rule_entry.rule.attrib.u.v6.dst_addr_mask, 0xFF,
		sizeof(flt_rule_entry.rule.attrib.u.v6.dst_addr_mask));

	change_to_network_order(IPA_IP_v6, &flt_rule_entry.rule.attrib);

	memset(&flt_eq, 0, sizeof(flt_eq));
	memcpy(&flt_eq.attrib, &flt_rule_entry.rule.attrib, sizeof(flt_eq.attrib));
	flt_eq.ip = IPA_IP_v6;
	if(ioctl(m_fd_ipa, IPA_IOC_GENERATE_FLT_EQ, &flt_eq) != 0)
	{
		IPACMERR("Failed to get eq_attrib\n");
		return;
	}

	memcpy(&flt_rule_entry.rule.eq_attrib, &flt_eq.eq_attrib,
		sizeof(flt_rule_entry.rule.eq_attrib));

	memcpy(&(rules[rule_offset]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

	return;
}
#endif

void IPACM_Wan::HandleSTAClientDelEvt(const ipa_wan_client* client)
{
	if (client->ipv4_set == true)
	{
		CtList->HandleSTAClientDelEvt(client->v4_addr);
	}

	for (int i = 0; i < client->ipv6_set; ++i)
	{
		CtList->HandleSTAClientDelEvt_v6(Ipv6IpAddress(client->v6_addr[i], false));
	}
}


int IPACM_Wan::add_catchup_all_filtering_rule_each_pdn(const IPACM_firewall_conf_t& firewall_config, ipa_ip_type iptype,
	const struct ipa_rule_attrib& rx_prop_attrib, struct ipa_flt_rule_add& flt_rule_add, int fltr_rule_number)
{
	/* Check for "out of boundary" failure before adding a rule */
	if (fltr_rule_number >= IPA_MAX_FLT_RULE)
	{
		IPACMERR("Filtering table is full. Number of rules %d allowed %d\n", fltr_rule_number + 1, IPA_MAX_FLT_RULE);
		return IPACM_FAILURE;
	}

	struct ipa_flt_rule_add flt_rule_entry;
	memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

	flt_rule_entry.at_rear = true;
	flt_rule_entry.flt_rule_hdl = -1;
	flt_rule_entry.status = -1;

	flt_rule_entry.rule.retain_hdr = 1;
	flt_rule_entry.rule.to_uc = 0;
	flt_rule_entry.rule.eq_attrib_type = 1;
#ifdef FEATURE_IPA_V3
	flt_rule_entry.rule.hashable = true;
#endif
	memcpy(&flt_rule_entry.rule.attrib, &rx_prop_attrib, sizeof(struct ipa_rule_attrib));
	flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;

	int *num_firewall, *num_flt_rule;
	const char* rt_tbl_name;
	if (iptype == IPA_IP_v4)
	{
		num_firewall = &num_firewall_v4;
		num_flt_rule = &num_v4_flt_rule;

		flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = 0x00000000;
		flt_rule_entry.rule.attrib.u.v4.dst_addr = 0x00000000;

#if !defined FEATURE_IPACM_UL_FIREWALL
		/* firewall disable, all traffic are allowed */
		if (firewall_config.firewall_enable)
		{
			/* default action for v4 is go DST_NAT unless user set to exception*/
			if (firewall_config.rule_action_accept)
			{
				flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
				rt_tbl_name = ipacmcfg->rt_tbl_wan_dl.name;
			}
			else
			{
				flt_rule_entry.rule.action = IPA_PASS_TO_DST_NAT;
				rt_tbl_name = ipacmcfg->rt_tbl_lan_v4.name;
			}
		}
		else
#endif //FEATURE_IPACM_UL_FIREWALL
		{
			if (isWan_Bridge_Mode())
			{
				IPACMDBG_H("ODU is in bridge mode. \n");
				flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
				rt_tbl_name = ipacmcfg->rt_tbl_wan_dl.name;
			}
			else
			{
				flt_rule_entry.rule.action = IPA_PASS_TO_DST_NAT;
				rt_tbl_name = ipacmcfg->rt_tbl_lan_v4.name;
			}
		}
	}
	else if (iptype == IPA_IP_v6)
	{
		num_firewall = &num_firewall_v6;
		num_flt_rule = &num_v6_flt_rule;

		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[0] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[1] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[2] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[3] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[0] = 0X00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[1] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[2] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[3] = 0X00000000;

		/* firewall disable, all traffic are allowed */
		if (firewall_config.firewall_enable)
		{
			if (firewall_config.rule_action_accept)
			{
				flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
				rt_tbl_name = ipacmcfg->rt_tbl_wan_dl.name;
			}
			else
			{
				flt_rule_entry.rule.action = IPACM_Iface::ipacmcfg->IsIpv6CTEnabled() ?
					IPA_PASS_TO_DST_NAT : IPA_PASS_TO_ROUTING;
				rt_tbl_name = ipacmcfg->rt_tbl_wan_v6.name;
			}
		}
		else
		{
			flt_rule_entry.rule.action = IPACM_Iface::ipacmcfg->IsIpv6CTEnabled() ?
				IPA_PASS_TO_DST_NAT : IPA_PASS_TO_ROUTING;
			rt_tbl_name = ipacmcfg->rt_tbl_wan_v6.name;
		}
	}
	else
	{
		IPACMERR("IP type is not expected.\n");
		return IPACM_FAILURE;
	}

	ipa_ioc_get_rt_tbl_indx rt_tbl_idx;
	memset(&rt_tbl_idx, 0, sizeof(rt_tbl_idx));
	rt_tbl_idx.ip = iptype;
	strlcpy(rt_tbl_idx.name, rt_tbl_name, IPA_RESOURCE_NAME_MAX);
	rt_tbl_idx.name[IPA_RESOURCE_NAME_MAX - 1] = '\0';
	if (ioctl(m_fd_ipa, IPA_IOC_QUERY_RT_TBL_INDEX, &rt_tbl_idx))
	{
		IPACMERR("Failed to get routing table index from name\n");
		return IPACM_FAILURE;
	}
	flt_rule_entry.rule.rt_tbl_idx = rt_tbl_idx.idx;
	IPACMDBG_H("Routing table %s has index %d\n", rt_tbl_idx.name, rt_tbl_idx.idx);

	change_to_network_order(iptype, &flt_rule_entry.rule.attrib);

	ipa_ioc_generate_flt_eq flt_eq;
	memset(&flt_eq, 0, sizeof(flt_eq));
	memcpy(&flt_eq.attrib, &flt_rule_entry.rule.attrib, sizeof(flt_eq.attrib));
	flt_eq.ip = iptype;
	if (ioctl(m_fd_ipa, IPA_IOC_GENERATE_FLT_EQ, &flt_eq))
	{
		IPACMERR("Failed to get eq_attrib\n");
		return IPACM_FAILURE;
	}

	memcpy(&flt_rule_entry.rule.eq_attrib, &flt_eq.eq_attrib, sizeof(flt_rule_entry.rule.eq_attrib));
	memcpy(&flt_rule_add, &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
	IPACMDBG_H("Filter rule attrib mask: 0x%x\n", flt_rule_add.rule.attrib.attrib_mask);

	++(*num_firewall);
	++(*num_flt_rule);

	return IPACM_SUCCESS;
}

int IPACM_Wan::add_ipv6_frag_filtering_rule_ex(const struct ipa_rule_attrib& rx_prop_attrib,
	struct ipa_flt_rule_add& flt_rule_add, int fltr_rule_number)
{
	/* Check for "out of boundary" failure before adding a rule */
	if (fltr_rule_number >= IPA_MAX_FLT_RULE)
	{
		IPACMERR("Filtering table is full. Number of rules %d allowed %d\n", fltr_rule_number + 1, IPA_MAX_FLT_RULE);
		return IPACM_FAILURE;
	}

	struct ipa_flt_rule_add flt_rule_entry;
	memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
	flt_rule_entry.at_rear = true;
#ifdef FEATURE_IPA_V3
	flt_rule_entry.at_rear = false;
#endif
	flt_rule_entry.flt_rule_hdl = -1;
	flt_rule_entry.status = -1;

	flt_rule_entry.rule.retain_hdr = 1;
	flt_rule_entry.rule.to_uc = 0;
	flt_rule_entry.rule.eq_attrib_type = 1;
	flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
#ifdef FEATURE_IPA_V3
	flt_rule_entry.at_rear = false;
	flt_rule_entry.rule.hashable = false;
#endif

	ipa_ioc_get_rt_tbl_indx rt_tbl_idx;
	memset(&rt_tbl_idx, 0, sizeof(rt_tbl_idx));
	rt_tbl_idx.ip = IPA_IP_v6;
	strlcpy(rt_tbl_idx.name, IPACM_Iface::ipacmcfg->rt_tbl_wan_dl.name, IPA_RESOURCE_NAME_MAX);
	rt_tbl_idx.name[IPA_RESOURCE_NAME_MAX - 1] = '\0';
	if (ioctl(m_fd_ipa, IPA_IOC_QUERY_RT_TBL_INDEX, &rt_tbl_idx))
	{
		IPACMERR("Failed to get routing table index from name\n");
		return IPACM_FAILURE;
	}

	flt_rule_entry.rule.rt_tbl_idx = rt_tbl_idx.idx;
	IPACMDBG_H("IPv6 frag flt rule uses routing table index %d\n", rt_tbl_idx.idx);

	flt_rule_entry.rule.attrib.attrib_mask |= rx_prop_attrib.attrib_mask;
	flt_rule_entry.rule.attrib.meta_data_mask = rx_prop_attrib.meta_data_mask;
	flt_rule_entry.rule.attrib.meta_data = rx_prop_attrib.meta_data;
	flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_FRAGMENT;

	change_to_network_order(IPA_IP_v6, &flt_rule_entry.rule.attrib);

	ipa_ioc_generate_flt_eq flt_eq;
	memset(&flt_eq, 0, sizeof(flt_eq));
	memcpy(&flt_eq.attrib, &flt_rule_entry.rule.attrib, sizeof(flt_eq.attrib));
	flt_eq.ip = IPA_IP_v6;
	if (ioctl(m_fd_ipa, IPA_IOC_GENERATE_FLT_EQ, &flt_eq))
	{
		IPACMERR("Failed to get eq_attrib\n");
		return IPACM_FAILURE;
	}

	memcpy(&flt_rule_entry.rule.eq_attrib, &flt_eq.eq_attrib, sizeof(flt_rule_entry.rule.eq_attrib));
	memcpy(&flt_rule_add, &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

	++IPACM_Wan::num_v6_flt_rule;

	return IPACM_SUCCESS;
}

#ifdef FEATURE_VLAN_MPDN
int IPACM_Wan::add_firewall_rules_ex(
	const IPACM_firewall_conf_t& firewall_config,
	ipa_ip_type iptype,
	uint8_t curr_mux_id,
	const struct ipa_rule_attrib& rx_prop_attrib,
	ipacm_pdn_flt_rule* rules,
	int rules_size, int& pos)
#else
int IPACM_Wan::add_firewall_rules_ex(const IPACM_firewall_conf_t& firewall_config, ipa_ip_type iptype,
	const struct ipa_rule_attrib& rx_prop_attrib, struct ipa_flt_rule_add *rules, int rules_size, int& pos)
#endif
{
	if (!firewall_config.firewall_enable)
	{
		return IPACM_SUCCESS;
	}

	for (uint8_t i = 0; i < firewall_config.num_extd_firewall_entries; ++i)
	{
		struct ipa_flt_rule_add flt_rule_entry;
		memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

		flt_rule_entry.at_rear = true;
		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;

		flt_rule_entry.rule.retain_hdr = 1;
		flt_rule_entry.rule.to_uc = 0;
		flt_rule_entry.rule.eq_attrib_type = 1;

#ifdef FEATURE_IPA_V3
		flt_rule_entry.rule.hashable = true;
#endif

		int *num_firewall, *num_flt_rule;
		const char* rt_tbl_name;
		uint8_t* rule_protocol;
		const uint8_t* firewall_config_protocol;
		if (iptype == IPA_IP_v4)
		{
			if (firewall_config.extd_firewall_entries[i].ip_vsn != 4)
			{
				continue;
			}

			num_firewall = &num_firewall_v4;
			num_flt_rule = &num_v4_flt_rule;
			rule_protocol = &flt_rule_entry.rule.attrib.u.v4.protocol;
			firewall_config_protocol = &firewall_config.extd_firewall_entries[i].attrib.u.v4.protocol;

			if (firewall_config.rule_action_accept)
			{
				flt_rule_entry.rule.action = IPA_PASS_TO_DST_NAT;
				rt_tbl_name = ipacmcfg->rt_tbl_lan_v4.name;
			}
			else
			{
				flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
				rt_tbl_name = ipacmcfg->rt_tbl_wan_dl.name;
			}
		}
		else if (iptype == IPA_IP_v6)
		{
#ifndef FEATURE_IPACM_UL_FIREWALL
			if (firewall_config.extd_firewall_entries[i].ip_vsn != 6)
#else // n FEATURE_IPACM_UL_FIREWALL
			if (firewall_config.extd_firewall_entries[i].ip_vsn != 6 ||
				firewall_config.extd_firewall_entries[i].firewall_direction == IPACM_MSGR_UL_FIREWALL)
#endif //n FEATURE_IPACM_UL_FIREWALL
			{
				continue;
			}

			num_firewall = &num_firewall_v6;
			num_flt_rule = &num_v6_flt_rule;
			rule_protocol = &flt_rule_entry.rule.attrib.u.v6.next_hdr;
			firewall_config_protocol = &firewall_config.extd_firewall_entries[i].attrib.u.v6.next_hdr;

			if (firewall_config.rule_action_accept)
			{
				flt_rule_entry.rule.action =
					IPACM_Iface::ipacmcfg->IsIpv6CTEnabled() ? IPA_PASS_TO_DST_NAT : IPA_PASS_TO_ROUTING;
				rt_tbl_name = ipacmcfg->rt_tbl_wan_v6.name;
			}
			else
			{
				flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
				rt_tbl_name = ipacmcfg->rt_tbl_wan_dl.name;
			}
		}
		else
		{
			IPACMERR("IP type is not expected.\n");
			return IPACM_FAILURE;
		}

		ipa_ioc_get_rt_tbl_indx rt_tbl_idx;
		memset(&rt_tbl_idx, 0, sizeof(rt_tbl_idx));
		rt_tbl_idx.ip = iptype;
		strlcpy(rt_tbl_idx.name, rt_tbl_name, IPA_RESOURCE_NAME_MAX);
		rt_tbl_idx.name[IPA_RESOURCE_NAME_MAX - 1] = '\0';
		if (ioctl(m_fd_ipa, IPA_IOC_QUERY_RT_TBL_INDEX, &rt_tbl_idx))
		{
			IPACMERR("Failed to get routing table index from name\n");
			return IPACM_FAILURE;
		}
		flt_rule_entry.rule.rt_tbl_idx = rt_tbl_idx.idx;
		IPACMDBG_H("Routing table %s has index %d\n", rt_tbl_idx.name, rt_tbl_idx.idx);

		memcpy(&flt_rule_entry.rule.attrib, &firewall_config.extd_firewall_entries[i].attrib,
			sizeof(struct ipa_rule_attrib));

		flt_rule_entry.rule.attrib.attrib_mask |= rx_prop_attrib.attrib_mask;
		flt_rule_entry.rule.attrib.meta_data_mask = rx_prop_attrib.meta_data_mask;
		flt_rule_entry.rule.attrib.meta_data = rx_prop_attrib.meta_data;

		change_to_network_order(iptype, &flt_rule_entry.rule.attrib);

		ipa_ioc_generate_flt_eq flt_eq;

		/* check if the rule is define as TCP_UDP, split into 2 rules, 1 for TCP and 1 UDP */
		if (*firewall_config_protocol == IPACM_FIREWALL_IPPROTO_TCP_UDP)
		{
			/* Check for "out of boundary" failure before adding two rules */
			if (pos + 1 >= rules_size)
			{
				IPACMERR("Filtering table is full. Number of rules %d allowed %d\n", pos + 2, rules_size);
				return IPACM_SUCCESS;
			}

			/* insert TCP rule*/
			*rule_protocol = IPACM_FIREWALL_IPPROTO_TCP;

			memset(&flt_eq, 0, sizeof(flt_eq));
			memcpy(&flt_eq.attrib, &flt_rule_entry.rule.attrib, sizeof(flt_eq.attrib));
			flt_eq.ip = iptype;
			if (ioctl(m_fd_ipa, IPA_IOC_GENERATE_FLT_EQ, &flt_eq))
			{
				IPACMERR("Failed to get eq_attrib\n");
				return IPACM_FAILURE;
			}
			memcpy(&flt_rule_entry.rule.eq_attrib, &flt_eq.eq_attrib, sizeof(flt_rule_entry.rule.eq_attrib));

#ifdef FEATURE_VLAN_MPDN
			rules[pos].mux_id = curr_mux_id;
			memcpy(&(rules[pos].flt_rule), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
			IPACMDBG_H("Filter rule attrib mask: 0x%x\n", rules[pos].flt_rule.rule.attrib.attrib_mask);
#else
			memcpy(&(rules[pos]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
			IPACMDBG_H("Filter rule attrib mask: 0x%x\n", rules[pos].rule.attrib.attrib_mask);
#endif
			++pos;
			++(*num_firewall);
			++(*num_flt_rule);

			/* insert UDP rule*/
			*rule_protocol = IPACM_FIREWALL_IPPROTO_UDP;

			memset(&flt_eq, 0, sizeof(flt_eq));
			memcpy(&flt_eq.attrib, &flt_rule_entry.rule.attrib, sizeof(flt_eq.attrib));
			flt_eq.ip = iptype;
			if (ioctl(m_fd_ipa, IPA_IOC_GENERATE_FLT_EQ, &flt_eq))
			{
				IPACMERR("Failed to get eq_attrib\n");
				return IPACM_FAILURE;
			}
			memcpy(&flt_rule_entry.rule.eq_attrib, &flt_eq.eq_attrib, sizeof(flt_rule_entry.rule.eq_attrib));

#ifdef FEATURE_VLAN_MPDN
			rules[pos].mux_id = curr_mux_id;
			memcpy(&(rules[pos].flt_rule), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
			IPACMDBG_H("Filter rule attrib mask: 0x%x\n", rules[pos].flt_rule.rule.attrib.attrib_mask);
#else
			memcpy(&(rules[pos]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
			IPACMDBG_H("Filter rule attrib mask: 0x%x\n", rules[pos].rule.attrib.attrib_mask);
#endif
			++pos;
			++(*num_firewall);
			++(*num_flt_rule);
		}
		else
		{
			/* Check for "out of boundary" failure before adding a rule */
			if (pos >= rules_size)
			{
				IPACMERR("Filtering table is full. Number of rules %d allowed %d\n", pos + 1, rules_size);
				return IPACM_SUCCESS;
			}

			memset(&flt_eq, 0, sizeof(flt_eq));
			memcpy(&flt_eq.attrib, &flt_rule_entry.rule.attrib, sizeof(flt_eq.attrib));
			flt_eq.ip = iptype;
			if (ioctl(m_fd_ipa, IPA_IOC_GENERATE_FLT_EQ, &flt_eq))
			{
				IPACMERR("Failed to get eq_attrib\n");
				return IPACM_FAILURE;
			}
			memcpy(&flt_rule_entry.rule.eq_attrib, &flt_eq.eq_attrib, sizeof(flt_rule_entry.rule.eq_attrib));

#ifdef FEATURE_VLAN_MPDN
			rules[pos].mux_id = curr_mux_id;
			memcpy(&(rules[pos].flt_rule), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
			IPACMDBG_H("Filter rule attrib mask: 0x%x\n", rules[pos].flt_rule.rule.attrib.attrib_mask);
#else
			memcpy(&(rules[pos]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
			IPACMDBG_H("Filter rule attrib mask: 0x%x\n", rules[pos].rule.attrib.attrib_mask);
#endif
			++pos;
			++(*num_firewall);
			++(*num_flt_rule);
		}
	} /* end of firewall filter rule add for loop*/
	return IPACM_SUCCESS;
}
