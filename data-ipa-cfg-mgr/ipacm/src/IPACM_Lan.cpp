/*
 * Copyright (c) 2013-2018 The Linux Foundation. All rights reserved.
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
	IPACM_Lan.cpp

	@brief
	This file implements the LAN iface functionality.

	@Author
	Skylar Chang

*/
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "IPACM_Netlink.h"
#include "IPACM_Lan.h"
#include "IPACM_Wan.h"
#include "IPACM_IfaceManager.h"
#include "linux/rmnet_ipa_fd_ioctl.h"
#include "linux/ipa_qmi_service_v01.h"
#include "linux/msm_ipa.h"
#include "IPACM_ConntrackListener.h"
#include <sys/ioctl.h>
#include <fcntl.h>

bool IPACM_Lan::odu_up = false;

#ifdef FEATURE_IPACM_PER_CLIENT_STATS
bool IPACM_Lan::lan_stats_inited = false;
ipa_lan_client_idx IPACM_Lan::active_lan_client_index_odu[IPA_MAX_NUM_HW_PATH_CLIENTS];
ipa_lan_client_idx IPACM_Lan::inactive_lan_client_index_odu[IPA_MAX_NUM_HW_PATH_CLIENTS];
#endif

IPACM_Lan::IPACM_Lan(int iface_index) : IPACM_Iface(iface_index)
{
	num_eth_client = 0;
	header_name_count = 0;
	ipv6_set = 0;
	ipv4_header_set = false;
	ipv6_header_set = false;
	odu_route_rule_v4_hdl = NULL;
	odu_route_rule_v6_hdl = NULL;
	eth_client = NULL;
	int i, m_fd_odu, ret = IPACM_SUCCESS;
	eth_client_len = 0;
	is_l2tp_iface = false;
#ifdef FEATURE_IPACM_PER_CLIENT_STATS
	int max_clients = (IPACM_Iface::ipacmcfg->ipacm_lan_stats_enable) ? IPA_MAX_NUM_HW_PATH_CLIENTS:
		IPA_MAX_NUM_ETH_CLIENTS;
	is_odu = false;
#else
	int max_clients = IPA_MAX_NUM_ETH_CLIENTS;
#endif

	Nat_App = NatApp::GetInstance();
	if (Nat_App == NULL)
	{
		IPACMERR("unable to get Nat App instance \n");
		return;
	}

	num_wan_ul_fl_rule_v4 = 0;
	num_wan_ul_fl_rule_v6 = 0;
	hdr_len = 0;
	memset(wan_ul_fl_rule_hdl_v4, 0, MAX_WAN_UL_FILTER_RULES * sizeof(uint32_t));
	memset(wan_ul_fl_rule_hdl_v6, 0, MAX_WAN_UL_FILTER_RULES * sizeof(uint32_t));

	is_active = true;
	memset(ipv4_icmp_flt_rule_hdl, 0, NUM_IPV4_ICMP_FLT_RULE * sizeof(uint32_t));

	is_mode_switch = false;
	if_ipv4_subnet =0;
	memset(private_fl_rule_hdl, 0, IPA_MAX_PRIVATE_SUBNET_ENTRIES * sizeof(uint32_t));
	memset(ipv6_prefix_flt_rule_hdl, 0, IPA_MAX_IPV6_PREFIX_FLT_RULE  * sizeof(uint32_t));
	memset(ipv6_icmp_flt_rule_hdl, 0, NUM_IPV6_ICMP_FLT_RULE * sizeof(uint32_t));
	modem_ul_v4_set = false;
	modem_ul_v6_set = false;
	memset(ipv6_prefix, 0, sizeof(ipv6_prefix));

#ifdef FEATURE_VLAN_MPDN
	dummy_prefix_installed = false;
	memset(v4_mux_up, 0, sizeof(v4_mux_up[0]) * IPA_MAX_NUM_HW_PDNS);
	memset(v6_mux_up, 0, sizeof(v6_mux_up[0]) * IPA_MAX_NUM_HW_PDNS);
#endif

	/* support eth multiple clients */
	if(iface_query != NULL)
	{
		if(ipa_if_cate != WLAN_IF)
		{
			eth_client_len = (sizeof(ipa_eth_client)) + (iface_query->num_tx_props * sizeof(eth_client_rt_hdl));
			eth_client = (ipa_eth_client *)calloc(max_clients, eth_client_len);
			if (eth_client == NULL)
			{
				IPACMERR("unable to allocate memory\n");
				return;
			}
		}

		IPACMDBG_H(" IPACM->IPACM_Lan(%d) constructor: Tx:%d Rx:%d \n", ipa_if_num,
					 iface_query->num_tx_props, iface_query->num_rx_props);

		/* ODU routing table initilization */
		if(ipa_if_cate == ODU_IF)
		{
			odu_route_rule_v4_hdl = (uint32_t *)calloc(iface_query->num_tx_props, sizeof(uint32_t));
			odu_route_rule_v6_hdl = (uint32_t *)calloc(iface_query->num_tx_props, sizeof(uint32_t));
			if ((odu_route_rule_v4_hdl == NULL) || (odu_route_rule_v6_hdl == NULL))
			{
				IPACMERR("unable to allocate memory\n");
				if(odu_route_rule_v4_hdl != NULL)
				{
					free(odu_route_rule_v4_hdl);
				}
				else if(odu_route_rule_v6_hdl != NULL)
				{
					free(odu_route_rule_v6_hdl);
				}
				return;
			}
		}
	}
#ifdef FEATURE_IPACM_PER_CLIENT_STATS
	if (rx_prop)
	{
		if (rx_prop->rx[0].src_pipe == IPA_CLIENT_ODU_PROD)
			is_odu = true;
		else
			is_odu = false;
	}
	IPACMDBG_H ("Is ODU client? %s\n", is_odu?"Yes":"No");

	for (i = 0; i < IPA_MAX_NUM_HW_PATH_CLIENTS; i++)
	{
		active_lan_client_index[i].lan_stats_idx = -1;
		memset(active_lan_client_index[i].mac, 0, IPA_MAC_ADDR_SIZE);
		inactive_lan_client_index[i].lan_stats_idx = -1;
		memset(inactive_lan_client_index[i].mac, 0, IPA_MAC_ADDR_SIZE);
	}
	if (lan_stats_inited == false)
	{
		for (i = 0; i < IPA_MAX_NUM_HW_PATH_CLIENTS; i++)
		{
			active_lan_client_index_odu[i].lan_stats_idx = -1;
			memset(active_lan_client_index_odu[i].mac, 0, IPA_MAC_ADDR_SIZE);
			inactive_lan_client_index_odu[i].lan_stats_idx = -1;
			memset(inactive_lan_client_index_odu[i].mac, 0, IPA_MAC_ADDR_SIZE);
		}
		lan_stats_inited = true;
	}

#endif
	/* ODU routing table initilization */
	if(ipa_if_cate == ODU_IF)
	{
		/* only do one time ioctl to odu-driver to infrom in router or bridge mode*/
		if (IPACM_Lan::odu_up != true)
		{
				m_fd_odu = open(IPACM_Iface::ipacmcfg->DEVICE_NAME_ODU, O_RDWR);
				if (0 == m_fd_odu)
				{
					IPACMERR("Failed opening %s.\n", IPACM_Iface::ipacmcfg->DEVICE_NAME_ODU);
					return ;
				}

				if(IPACM_Iface::ipacmcfg->ipacm_odu_router_mode == true)
				{
					ret = ioctl(m_fd_odu, ODU_BRIDGE_IOC_SET_MODE, ODU_BRIDGE_MODE_ROUTER);
					IPACM_Iface::ipacmcfg->ipacm_odu_enable = true;
				}
				else
				{
					ret = ioctl(m_fd_odu, ODU_BRIDGE_IOC_SET_MODE, ODU_BRIDGE_MODE_BRIDGE);
					IPACM_Iface::ipacmcfg->ipacm_odu_enable = true;
				}

				if (ret)
				{
					IPACMERR("Failed tell odu-driver the mode\n");
				}
				IPACMDBG("Tell odu-driver in router-mode(%d)\n", IPACM_Iface::ipacmcfg->ipacm_odu_router_mode);
				IPACMDBG_H("odu is up: odu-driver in router-mode(%d) \n", IPACM_Iface::ipacmcfg->ipacm_odu_router_mode);
				close(m_fd_odu);
				IPACM_Lan::odu_up = true;
		}
	}

	each_client_rt_rule_count[IPA_IP_v4] = 0;
	each_client_rt_rule_count[IPA_IP_v6] = 0;
	if(iface_query != NULL && tx_prop != NULL)
	{
		for(i=0; i<iface_query->num_tx_props; i++)
			each_client_rt_rule_count[tx_prop->tx[i].ip]++;
	}
	IPACMDBG_H("Need to add %d IPv4 and %d IPv6 routing rules for eth bridge for each client.\n", each_client_rt_rule_count[IPA_IP_v4], each_client_rt_rule_count[IPA_IP_v6]);

#ifdef FEATURE_IPA_ANDROID
	/* set the IPA-client pipe enum */
	if(ipa_if_cate == LAN_IF)
	{
		handle_tethering_client(false, IPACM_CLIENT_USB);
	}
#endif

#ifdef FEATURE_L2TP_E2E
	if(ipa_if_cate == ODU_IF)
	{
		install_l2tp_ul_hdr_proc_ctx();
	}
#endif

	return;
}

IPACM_Lan::~IPACM_Lan()
{
	IPACM_EvtDispatcher::deregistr(this);
	IPACM_IfaceManager::deregistr(this);
	return;
}

/* LAN-iface's callback function */
void IPACM_Lan::event_callback(ipa_cm_event_id event, void *param)
{
	if(is_active == false && event != IPA_LAN_DELETE_SELF)
	{
		IPACMDBG_H("The interface is no longer active, return.\n");
		return;
	}

	int ipa_interface_index;
	ipacm_ext_prop* ext_prop;
	ipacm_event_iface_up* data_wan;
	ipacm_event_iface_up_tehter* data_wan_tether;
	int clnt_indx;

	switch (event)
	{
	case IPA_LINK_DOWN_EVENT:
		{
			ipacm_event_data_fid *data = (ipacm_event_data_fid *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);
			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG_H("Received IPA_LINK_DOWN_EVENT\n");
				handle_down_evt();
				IPACM_Iface::ipacmcfg->DelNatIfaces(dev_name); // delete NAT-iface
				return;
			}
		}
		break;

	case IPA_CFG_CHANGE_EVENT:
		{
			if ( IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_cat != ipa_if_cate)
			{
				IPACMDBG_H("Received IPA_CFG_CHANGE_EVENT and category changed\n");
				/* delete previous instance */
				handle_down_evt();
				IPACM_Iface::ipacmcfg->DelNatIfaces(dev_name); // delete NAT-iface
				is_mode_switch = true; // need post internal usb-link up event
				return;
			}
			/* Add Natting iface to IPACM_Config if there is  Rx/Tx property */
			if (rx_prop != NULL || tx_prop != NULL)
			{
				IPACMDBG_H(" Has rx/tx properties registered for iface %s, add for NATTING \n", dev_name);
				IPACM_Iface::ipacmcfg->AddNatIfaces(dev_name);
#ifdef FEATURE_VLAN_MPDN
				IPACM_Iface::ipacmcfg->restore_vlan_nat_ifaces(dev_name);
#endif
			}
		}
		break;
#ifdef FEATURE_IPACM_UL_FIREWALL
	case IPA_FIREWALL_CHANGE_EVENT:
		IPACMDBG_H("Received IPA_FIREWALL_CHANGE_EVENT\n");

		if(ip_type != IPA_IP_v4)
		{
			IPACMDBG_H ("iface_ul_firewall Addr = (0x%x)\n", &iface_ul_firewall);
			configure_v6_ul_firewall();
		}
		else
		{
			IPACMERR("IP type is not valid.\n");
		}
		break;
#endif
	case IPA_PRIVATE_SUBNET_CHANGE_EVENT:
		{
			ipacm_event_data_fid *data = (ipacm_event_data_fid *)param;
			/* internel event: data->if_index is ipa_if_index */
			if (data->if_index == ipa_if_num)
			{
				IPACMDBG_H("Received IPA_PRIVATE_SUBNET_CHANGE_EVENT from itself posting, ignore\n");
				return;
			}
			else
			{
				IPACMDBG_H("Received IPA_PRIVATE_SUBNET_CHANGE_EVENT from other LAN iface \n");
#if defined(FEATURE_IPA_ANDROID) || defined(FEATURE_VLAN_MPDN)
				handle_private_subnet_android(IPA_IP_v4);
#endif
				IPACMDBG_H(" delete old private subnet rules, use new sets \n");
				return;
			}
		}
		break;

	case IPA_LAN_DELETE_SELF:
	{
		ipacm_event_data_fid *data = (ipacm_event_data_fid *)param;
		if(data->if_index == ipa_if_num)
		{
			IPACMDBG_H("Received IPA_LAN_DELETE_SELF event.\n");
			IPACMDBG_H("ipa_LAN (%s):ipa_index (%d) instance close \n", IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].iface_name, ipa_if_num);
			/* posting link-up event for cradle use-case */
			if(is_mode_switch)
			{
				IPACMDBG_H("Posting IPA_USB_LINK_UP_EVENT event for (%s)\n", dev_name);
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
				//IPACMDBG_H("Posting event:%d\n", evt_data.event);
				IPACM_EvtDispatcher::PostEvt(&evt_data);
			}
#ifndef FEATURE_IPA_ANDROID
			if(rx_prop != NULL)
			{
				if(IPACM_Iface::ipacmcfg->getFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v4) != 0)
				{
					IPACMDBG_DMESG("### WARNING ### num ipv4 flt rules on client %d is not expected: %d expected value: 0",
						rx_prop->rx[0].src_pipe, IPACM_Iface::ipacmcfg->getFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v4));
				}
				if(IPACM_Iface::ipacmcfg->getFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6) != 0)
				{
					IPACMDBG_DMESG("### WARNING ### num ipv6 flt rules on client %d is not expected: %d expected value: 0",
						rx_prop->rx[0].src_pipe, IPACM_Iface::ipacmcfg->getFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6));
				}
			}
#endif

#ifdef FEATURE_ETH_BRIDGE_LE
			if(rx_prop != NULL)
			{
				free(rx_prop);
			}
			if(tx_prop != NULL)
			{
				free(tx_prop);
			}
			if(iface_query != NULL)
			{
				free(iface_query);
			}
#endif
			delete this;
		}
		break;
	}

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
#if defined(FEATURE_L2TP_E2E) || defined(FEATURE_L2TP) || defined(FEATURE_VLAN_MPDN)
			if(is_vlan_event(data->iface_name)) {
				if(data->iptype == IPA_IP_v6
#ifndef FEATURE_VLAN_MPDN
					// for VLAN_MPDN we only have link local addresses
					&& is_unique_local_ipv6_addr(data->ipv6_addr)
#endif
					)
				{
					IPACMDBG_H("Got IPv6 new addr event for a vlan iface %s.\n", data->iface_name);
					IPACM_Iface::ipacmcfg->handle_vlan_iface_info(data);
					return;
				}
			}
#endif
			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG_H("Received IPA_ADDR_ADD_EVENT\n");

				/* only call ioctl for ODU iface with bridge mode */
				if(IPACM_Iface::ipacmcfg->ipacm_odu_enable == true && IPACM_Iface::ipacmcfg->ipacm_odu_router_mode == false
						&& ipa_if_cate == ODU_IF)
				{
					if((data->iptype == IPA_IP_v6) && (num_dft_rt_v6 == 0))
					{
						handle_addr_evt_odu_bridge(data);
					}
#if defined(FEATURE_IPA_ANDROID) || defined(FEATURE_VLAN_MPDN)
					add_dummy_private_subnet_flt_rule(data->iptype);
					handle_private_subnet_android(data->iptype);
#else
					handle_private_subnet(data->iptype);
#endif
				}
				else
				{

					/* check v4 not setup before, v6 can have 2 iface ip */
					if( ((data->iptype != ip_type) && (ip_type != IPA_IP_MAX))
						|| ((data->iptype==IPA_IP_v6) && (num_dft_rt_v6!=MAX_DEFAULT_v6_ROUTE_RULES)))
					{
						IPACMDBG_H("Got IPA_ADDR_ADD_EVENT ip-family:%d, v6 num %d: \n",data->iptype,num_dft_rt_v6);
						if(handle_addr_evt(data) == IPACM_FAILURE)
						{
							return;
						}
#if defined(FEATURE_IPA_ANDROID) || defined(FEATURE_VLAN_MPDN)
						add_dummy_private_subnet_flt_rule(data->iptype);
						handle_private_subnet_android(data->iptype);
#else
#ifdef FEATURE_L2TP_E2E
						if ((num_dft_rt_v6 == 1) && (data->iptype == IPA_IP_v6))
						{
							if(ipa_if_cate == ODU_IF)
							{
								install_l2tp_inner_private_subnet_flt_rule(); /* encapsulated IPv4 private subnet rule */
							}
						}
#endif
						handle_private_subnet(data->iptype);
#endif
#ifdef FEATURE_VLAN_MPDN
						if(data->iptype == IPA_IP_v6)
						{
							add_dummy_ipv6_prefix_flt_rule();
							/* if there are any v6 calls up, update rules */
							modify_ipv6_prefix_flt_rule();
						}

#endif
#ifdef FEATURE_VLAN_MPDN
						/* VLAN IFACES don't care about default route */
						if(!(IPACM_Iface::ipacmcfg->iface_in_vlan_mode(dev_name)))
#endif
						{
							if(IPACM_Wan::isWanUP(ipa_if_num))
							{
								if(data->iptype == IPA_IP_v4 || data->iptype == IPA_IP_MAX)
								{
									if(IPACM_Wan::backhaul_is_sta_mode == false)
									{
										ext_prop = IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v4);
										handle_wan_up_ex(ext_prop, IPA_IP_v4,
											IPACM_Wan::getXlat_Mux_Id());
									}
									else
									{
										handle_wan_up(IPA_IP_v4);
									}
								}
							}
						}
#ifdef FEATURE_VLAN_MPDN
						else
							check_vlan_PDNUp(IPA_IP_v4);

						/* VLAN IFACES don't care about default route */
						if(!(IPACM_Iface::ipacmcfg->iface_in_vlan_mode(dev_name)))
#endif
						{
							if(IPACM_Wan::isWanUP_V6(ipa_if_num)) /* Modem v6 call is UP?*/
							{
#ifdef FEATURE_IPACM_UL_FIREWALL
								if(data->iptype == IPA_IP_v6)
									configure_v6_ul_firewall();
#endif //FEATURE_IPACM_UL_FIREWALL
								if((data->iptype == IPA_IP_v6 || data->iptype == IPA_IP_MAX) && num_dft_rt_v6 == 1)
								{
									memcpy(ipv6_prefix, IPACM_Wan::backhaul_ipv6_prefix, sizeof(ipv6_prefix));
#ifndef FEATURE_VLAN_MPDN
									install_ipv6_prefix_flt_rule(IPACM_Wan::backhaul_ipv6_prefix);
#else
									modify_ipv6_prefix_flt_rule();
#endif
									if(IPACM_Wan::backhaul_is_sta_mode == false)
									{
										ext_prop = IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v6);
										handle_wan_up_ex(ext_prop, IPA_IP_v6, 0);
									}
									else
									{
										handle_wan_up(IPA_IP_v6);
									}
								}
							}
#ifdef FEATURE_IPACM_UL_FIREWALL
							else
								IPACMDBG_H("WAN v6 is not UP\n");
#endif //FEATURE_IPACM_UL_FIREWALL
						}
#ifdef FEATURE_VLAN_MPDN
						else
							check_vlan_PDNUp(IPA_IP_v6);
#endif

						/* Post event to NAT */
						if (post_lan_up_event(data))
						{
							return;
						}

						IPACMDBG_H("Finish handling IPA_ADDR_ADD_EVENT for ip-family(%d)\n", data->iptype);
					}

					IPACMDBG_H("Finish handling IPA_ADDR_ADD_EVENT for ip-family(%d)\n", data->iptype);
					/* checking if SW-RT_enable */
					if (IPACM_Iface::ipacmcfg->ipa_sw_rt_enable == true)
					{
						/* handle software routing enable event*/
						IPACMDBG_H("IPA_SW_ROUTING_ENABLE for iface: %s \n",IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].iface_name);
						handle_software_routing_enable();
					}

				}
			}
		}
		break;
#ifdef FEATURE_IPA_ANDROID
	case IPA_HANDLE_WAN_UP_TETHER:
		IPACMDBG_H("Received IPA_HANDLE_WAN_UP_TETHER event\n");

		data_wan_tether = (ipacm_event_iface_up_tehter*)param;
		if(data_wan_tether == NULL)
		{
			IPACMERR("No event data is found.\n");
			return;
		}
		IPACMDBG_H("Backhaul is sta mode?%d, if_index_tether:%d tether_if_name:%s\n", data_wan_tether->is_sta,
					data_wan_tether->if_index_tether,
					IPACM_Iface::ipacmcfg->iface_table[data_wan_tether->if_index_tether].iface_name);
		if (data_wan_tether->if_index_tether == ipa_if_num)
		{
			if(ip_type == IPA_IP_v4 || ip_type == IPA_IP_MAX)
			{
				if(data_wan_tether->is_sta == false)
				{
					ext_prop = IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v4);
					handle_wan_up_ex(ext_prop, IPA_IP_v4, 0);
				}
				else
				{
					handle_wan_up(IPA_IP_v4);
				}
			}
		}
		break;

	case IPA_HANDLE_WAN_UP_V6_TETHER:
		IPACMDBG_H("Received IPA_HANDLE_WAN_UP_V6_TETHER event\n");

		data_wan_tether = (ipacm_event_iface_up_tehter*)param;
		if(data_wan_tether == NULL)
		{
			IPACMERR("No event data is found.\n");
			return;
		}
		IPACMDBG_H("Backhaul is sta mode?%d, if_index_tether:%d tether_if_name:%s\n", data_wan_tether->is_sta,
					data_wan_tether->if_index_tether,
					IPACM_Iface::ipacmcfg->iface_table[data_wan_tether->if_index_tether].iface_name);
		if (data_wan_tether->if_index_tether == ipa_if_num)
		{
			if(ip_type == IPA_IP_v6 || ip_type == IPA_IP_MAX)
			{
					memcpy(ipv6_prefix, data_wan_tether->ipv6_prefix, sizeof(ipv6_prefix));
					install_ipv6_prefix_flt_rule(data_wan_tether->ipv6_prefix);
					if(data_wan_tether->is_sta == false)
					{
						ext_prop = IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v6);
						handle_wan_up_ex(ext_prop, IPA_IP_v6, 0);
					}
					else
					{
						handle_wan_up(IPA_IP_v6);
					}
			}
		}
		break;

	case IPA_HANDLE_WAN_DOWN_TETHER:
		IPACMDBG_H("Received IPA_HANDLE_WAN_DOWN_TETHER event\n");
		data_wan_tether = (ipacm_event_iface_up_tehter*)param;
		if(data_wan_tether == NULL)
		{
			IPACMERR("No event data is found.\n");
			return;
		}
		IPACMDBG_H("Backhaul is sta mode?%d, if_index_tether:%d tether_if_name:%s\n", data_wan_tether->is_sta,
					data_wan_tether->if_index_tether,
					IPACM_Iface::ipacmcfg->iface_table[data_wan_tether->if_index_tether].iface_name);
		if (data_wan_tether->if_index_tether == ipa_if_num)
		{
			if(ip_type == IPA_IP_v4 || ip_type == IPA_IP_MAX)
			{
				handle_wan_down(data_wan_tether->is_sta);
			}
		}
		break;

	case IPA_HANDLE_WAN_DOWN_V6_TETHER:
		IPACMDBG_H("Received IPA_HANDLE_WAN_DOWN_V6_TETHER event\n");
		data_wan_tether = (ipacm_event_iface_up_tehter*)param;
		if(data_wan_tether == NULL)
		{
			IPACMERR("No event data is found.\n");
			return;
		}
		IPACMDBG_H("Backhaul is sta mode?%d, if_index_tether:%d tether_if_name:%s\n", data_wan_tether->is_sta,
					data_wan_tether->if_index_tether,
					IPACM_Iface::ipacmcfg->iface_table[data_wan_tether->if_index_tether].iface_name);
		if (data_wan_tether->if_index_tether == ipa_if_num)
		{
			/* clean up v6 RT rules*/
			IPACMDBG_H("Received IPA_HANDLE_WAN_DOWN_V6_TETHER in LAN-instance and need clean up client IPv6 address \n");
			/* reset usb-client ipv6 rt-rules */
			handle_lan_client_reset_rt(IPA_IP_v6);

			if(ip_type == IPA_IP_v6 || ip_type == IPA_IP_MAX)
			{
				handle_wan_down_v6(data_wan_tether->is_sta);
			}
		}
		break;
#else
	case IPA_HANDLE_WAN_UP:
		IPACMDBG_H("Received IPA_HANDLE_WAN_UP event\n");

		data_wan = (ipacm_event_iface_up*)param;
		if(data_wan == NULL)
		{
			IPACMERR("No event data is found.\n");
			return;
		}
		IPACMDBG_H("Backhaul is sta mode?%d\n", data_wan->is_sta);
#ifdef FEATURE_VLAN_MPDN
		/* VLAN IFACES don't care about default route */
		if((IPACM_Iface::ipacmcfg->iface_in_vlan_mode(dev_name)))
		{
			IPACMDBG_H("IF %s is vlan IF, ignoring IPA_HANDLE_WAN_UP", dev_name);
			return;
		}
#endif
		if(ip_type == IPA_IP_v4 || ip_type == IPA_IP_MAX)
		{
			if(data_wan->is_sta == false)
			{
				ext_prop = IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v4);
				handle_wan_up_ex(ext_prop, IPA_IP_v4, data_wan->xlat_mux_id);
			}
			else
			{
				handle_wan_up(IPA_IP_v4);
			}
			}
		break;

	case IPA_HANDLE_WAN_UP_V6:
		IPACMDBG_H("Received IPA_HANDLE_WAN_UP_V6 event\n");

		data_wan = (ipacm_event_iface_up*)param;
		if(data_wan == NULL)
		{
			IPACMERR("No event data is found.\n");
			return;
		}
		IPACMDBG_H("Backhaul is sta mode?%d\n", data_wan->is_sta);
#ifdef FEATURE_VLAN_MPDN
		/* VLAN IFACES don't care about default route */
		if((IPACM_Iface::ipacmcfg->iface_in_vlan_mode(dev_name)))
		{
			IPACMDBG_H("IF %s is vlan IF, ignoring IPA_HANDLE_WAN_UP_V6", dev_name);
			return;
		}
#endif
		if(ip_type == IPA_IP_v6 || ip_type == IPA_IP_MAX)
		{
#ifdef FEATURE_IPACM_UL_FIREWALL
			configure_v6_ul_firewall();
#endif //FEATURE_IPACM_UL_FIREWALL
			memcpy(ipv6_prefix, data_wan->ipv6_prefix, sizeof(ipv6_prefix));
#ifdef FEATURE_VLAN_MPDN
			/* new prefix was added - update flt rules */
			modify_ipv6_prefix_flt_rule();
#else
			install_ipv6_prefix_flt_rule(data_wan->ipv6_prefix);
#endif
			if(data_wan->is_sta == false)
			{
				ext_prop = IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v6);
				handle_wan_up_ex(ext_prop, IPA_IP_v6, 0);
			}
			else
			{
				handle_wan_up(IPA_IP_v6);
			}
		}
		break;

	case IPA_HANDLE_WAN_DOWN:
		IPACMDBG_H("Received IPA_HANDLE_WAN_DOWN event\n");
		data_wan = (ipacm_event_iface_up*)param;
		if(data_wan == NULL)
		{
			IPACMERR("No event data is found.\n");
			return;
		}
		IPACMDBG_H("Backhaul is sta mode?%d\n", data_wan->is_sta);
#ifdef FEATURE_VLAN_MPDN
		/* VLAN IFACES don't care about default route */
		if((IPACM_Iface::ipacmcfg->iface_in_vlan_mode(dev_name)))
		{
			IPACMDBG_H("IF %s is vlan IF, ignoring IPA_HANDLE_WAN_DOWN", dev_name);
			return;
		}
#endif
		if(ip_type == IPA_IP_v4 || ip_type == IPA_IP_MAX)
		{
			handle_wan_down(data_wan->is_sta);
		}
		break;

	case IPA_HANDLE_WAN_DOWN_V6:
		IPACMDBG_H("Received IPA_HANDLE_WAN_DOWN_V6 event\n");
		data_wan = (ipacm_event_iface_up*)param;
		if(data_wan == NULL)
		{
			IPACMERR("No event data is found.\n");
			return;
		}
		/* clean up v6 RT rules*/
		IPACMDBG_H("Received IPA_WAN_V6_DOWN in LAN-instance and need clean up client IPv6 address \n");
#ifdef FEATURE_VLAN_MPDN
		/* VLAN IFACES don't care about default route */
		if((IPACM_Iface::ipacmcfg->iface_in_vlan_mode(dev_name)))
		{
			IPACMDBG_H("IF %s is vlan IF, ignoring IPA_HANDLE_WAN_DOWN_V6", dev_name);
			return;
		}
#endif
		/* reset usb-client ipv6 rt-rules */
		handle_lan_client_reset_rt(IPA_IP_v6);

		IPACMDBG_H("Backhaul is sta mode?%d\n", data_wan->is_sta);
		if(ip_type == IPA_IP_v6 || ip_type == IPA_IP_MAX)
		{
#ifdef FEATURE_UL_FIREWALL
			// pdn is down, disable its Q6 UL firewall and reconfigure for all others
			disable_dft_firewall_rules_ul_ex(0);
			configure_v6_ul_firewall();
#endif
			handle_wan_down_v6(data_wan->is_sta);
		}
		break;
#endif

	case IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT:
		{
			ipacm_event_data_all *data = (ipacm_event_data_all *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);
			IPACMDBG_H("Recieved IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT event \n");
			IPACMDBG_H("check iface %s category: %d\n", dev_name, ipa_if_cate);
			if (ipa_interface_index == ipa_if_num && ipa_if_cate == ODU_IF)
			{
				IPACMDBG_H("ODU iface got v4-ip \n");
				/* first construc ODU full header */
				if ((ipv4_header_set == false) && (ipv6_header_set == false))
				{
					/* construct ODU RT tbl */
					handle_odu_hdr_init(data->mac_addr);
					if (IPACM_Iface::ipacmcfg->ipacm_odu_embms_enable == true)
					{
						handle_odu_route_add();
						IPACMDBG_H("construct ODU header and route rules, embms_flag (%d) \n", IPACM_Iface::ipacmcfg->ipacm_odu_embms_enable);
					}
					else
					{
						IPACMDBG_H("construct ODU header only, embms_flag (%d) \n", IPACM_Iface::ipacmcfg->ipacm_odu_embms_enable);
					}
				}
				/* if ODU in bridge mode, directly return */
				if(IPACM_Iface::ipacmcfg->ipacm_odu_router_mode == false)
				{
					IPACMDBG_H("ODU is in bridge mode, no action \n");
					return;
				}
			}

			if(ipa_interface_index == ipa_if_num)
			{
				IPACMDBG_H("ETH iface got client \n");
#ifdef FEATURE_VLAN_MPDN
				if(IPACM_Iface::ipacmcfg->iface_in_vlan_mode(dev_name))
				{
					IPACMDBG_H("physical iface in vlan mode got neighbor event with iptype %d, ip4 0x%X, ip6 pref [0x%X] [0x%X]\n",
						data->iptype, data->ipv4_addr, data->ipv6_addr[0], data->ipv6_addr[1]);
					IPACMDBG_H("ignoring non vlan neighbor event for vlan device\n");
					return;
				}
#endif
				/* first construc ETH full header */
				handle_eth_hdr_init(data->mac_addr);
				IPACMDBG_H("construct ETH header and route rules \n");
				/* Associate with IP and construct RT-rule */
				if (handle_eth_client_ipaddr(data) == IPACM_FAILURE)
				{
					return;
				}
#ifdef FEATURE_IPACM_PER_CLIENT_STATS
				if (IPACM_Iface::ipacmcfg->ipacm_lan_stats_enable == false)
#endif
				{
					handle_eth_client_route_rule(data->mac_addr, data->iptype);

					/* Add NAT rules after RT rules are set */
					HandleNeighIpAddrAddEvt(data);
				}
#ifdef FEATURE_IPACM_PER_CLIENT_STATS
				else
				{
					handle_eth_client_route_rule_ext(data->mac_addr, data->iptype);
				}
#endif
				eth_bridge_post_event(IPA_ETH_BRIDGE_CLIENT_ADD, IPA_IP_MAX, data->mac_addr, NULL, data->iface_name);
			}
#if defined(FEATURE_L2TP_E2E) || defined(FEATURE_L2TP)
			else if(is_l2tp_event(data->iface_name) && ipa_if_cate == ODU_IF)
			{
				handle_l2tp_neigh(data);
			}
#endif
#if defined(FEATURE_L2TP_E2E) || defined(FEATURE_L2TP) || defined(FEATURE_VLAN_MPDN)
			else if(is_vlan_event(data->iface_name))
			{
#ifndef FEATURE_VLAN_MPDN
				IPACMDBG_H("vlan neighbor event for iface %s\n", data->iface_name);
				/* in VLAN_MPDN we handle all VLAN neighbors */
				if(data->iptype == IPA_IP_v6 && is_unique_local_ipv6_addr(data->ipv6_addr))
				{
					IPACM_Iface::ipacmcfg->handle_vlan_client_info(data);
				}
#else
				handle_vlan_neighbor(data);
#endif
			}
#endif
			return;
		}
		break;

	case IPA_NEIGH_CLIENT_IP_ADDR_DEL_EVENT:
		{
			ipacm_event_data_all *data = (ipacm_event_data_all *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);

			IPACMDBG_H("Received IPA_NEIGH_CLIENT_IP_ADDR_DEL_EVENT event. \n");
			IPACMDBG_H("check iface %s category: %d\n", dev_name, ipa_if_cate);
			/* if ODU in bridge mode, directly return */
			if (ipa_if_cate == ODU_IF && IPACM_Iface::ipacmcfg->ipacm_odu_router_mode == false)
			{
				IPACMDBG_H("ODU is in bridge mode, no action \n");
				return;
			}

			if (ipa_interface_index == ipa_if_num
#if defined(FEATURE_L2TP_E2E) || defined(FEATURE_L2TP)
				|| (is_l2tp_event(data->iface_name) && ipa_if_cate == ODU_IF)
#elif defined (FEATURE_VLAN_MPDN)
				|| is_vlan_event(data->iface_name)
#endif
				)
			{
#ifndef FEATURE_VLAN_MPDN
				if(ipa_interface_index == ipa_if_num)
#endif
				{
					uint8_t vlan_id = 0;

					if (data->iptype == IPA_IP_v6)
					{
						handle_del_ipv6_addr(data);
						return;
					}
#ifdef FEATURE_VLAN_MPDN
					if(is_vlan_event(data->iface_name))
					{
						IPACMDBG_H("handling vlan ETH client del v4 ip address for iface %s\n",
							data->iface_name);
						if(IPACM_Iface::ipacmcfg->get_vlan_id(data->iface_name, &vlan_id))
						{
							IPACMERR("failed getting vlan id for iface %s\n",
								data->iface_name);
							return;
						}
					}
#endif
					IPACMDBG_H("LAN iface delete client \n");
					handle_eth_client_down_evt(data->mac_addr, vlan_id);
				}
#if defined(FEATURE_L2TP) || defined(FEATURE_L2TP)
				else
				{
#ifdef FEATURE_L2TP
					eth_bridge_post_event(IPA_ETH_BRIDGE_CLIENT_DEL, IPA_IP_MAX, data->mac_addr, NULL, data->iface_name);
#endif
#ifdef FEATURE_L2TP_E2E

					if(data->iptype == IPA_IP_v4)
					{
						uninstall_l2tp_rules(data);
					}
#endif
				}
#endif
				return;
			}
		}
		break;

	case IPA_SW_ROUTING_ENABLE:
		IPACMDBG_H("Received IPA_SW_ROUTING_ENABLE\n");
		/* handle software routing enable event*/
		handle_software_routing_enable();
		break;

	case IPA_SW_ROUTING_DISABLE:
		IPACMDBG_H("Received IPA_SW_ROUTING_DISABLE\n");
		/* handle software routing disable event*/
		handle_software_routing_disable();
		break;

	case IPA_CRADLE_WAN_MODE_SWITCH:
	{
		IPACMDBG_H("Received IPA_CRADLE_WAN_MODE_SWITCH event.\n");
		ipacm_event_cradle_wan_mode* wan_mode = (ipacm_event_cradle_wan_mode*)param;
		if(wan_mode == NULL)
		{
			IPACMERR("Event data is empty.\n");
			return;
		}

		if(wan_mode->cradle_wan_mode == BRIDGE)
		{
			handle_cradle_wan_mode_switch(true);
		}
		else
		{
			handle_cradle_wan_mode_switch(false);
		}
	}
	break;

	case IPA_TETHERING_STATS_UPDATE_EVENT:
	{
		IPACMDBG_H("Received IPA_TETHERING_STATS_UPDATE_EVENT event.\n");
		if (IPACM_Wan::isWanUP(ipa_if_num) || IPACM_Wan::isWanUP_V6(ipa_if_num))
		{
			if(IPACM_Wan::backhaul_is_sta_mode == false) /* LTE */
			{
				ipa_get_data_stats_resp_msg_v01 *data = (ipa_get_data_stats_resp_msg_v01 *)param;
				IPACMDBG("Received IPA_TETHERING_STATS_UPDATE_STATS ipa_stats_type: %d\n",data->ipa_stats_type);
				IPACMDBG("Received %d UL, %d DL pipe stats\n",data->ul_src_pipe_stats_list_len,
					data->dl_dst_pipe_stats_list_len);
				if (data->ipa_stats_type != QMI_IPA_STATS_TYPE_PIPE_V01)
				{
					IPACMERR("not valid pipe stats enum(%d)\n", data->ipa_stats_type);
					return;
				}
				handle_tethering_stats_event(data);
			}
		}
	}
	break;

#ifdef FEATURE_IPACM_PER_CLIENT_STATS
	/* QCMAP sends this event whenever a client is connected. */
	case IPA_LAN_CLIENT_CONNECT_EVENT:
		{
			ipacm_event_data_mac *data = (ipacm_event_data_mac *)param;
			if (!IPACM_Iface::ipacmcfg->ipacm_lan_stats_enable)
			{
				IPACMDBG_H("LAN stats functionality is not enabled, ignore IPA_LAN_CLIENT_CONNECT_EVENT.\n");
				return;
			}
			ipa_interface_index = iface_ipa_index_query(data->if_index);
			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG_H("Received IPA_LAN_CLIENT_CONNECT_EVENT\n");
				/* Check if we can add this to the active list. */
				/* Active List:- Clients for which index is less than IPA_MAX_NUM_HW_PATH_CLIENTS. */
				if (get_free_active_lan_stats_index(data->mac_addr) == -1)
				{
					IPACMDBG_H("Failed to reserve active lan_stats index, try inactive list. \n");
					/* Try to get the inactive index which can be used later. */
					if (get_free_inactive_lan_stats_index(data->mac_addr) == -1)
					{
						IPACMDBG_H("Failed to reserve inactive lan_stats index, return\n");
					}
					return;
				}
				/* Check if the client is inactive list and remove it*/
				if (reset_inactive_lan_stats_index(data->mac_addr) == -1)
				{
					IPACMDBG_H("Failed to reset inactive lan_stats index, return\n");
				}
				/* Check if the client is already initialized and add filter/routing rules. */
				IPACM_Lan::handle_lan_client_connect(data->mac_addr);
			}
		}
		break;
	/* QCMAP sends this event whenever a client is disconnected. */
	case IPA_LAN_CLIENT_DISCONNECT_EVENT:
		{
			ipacm_event_data_mac *data = (ipacm_event_data_mac *)param;
			if (!IPACM_Iface::ipacmcfg->ipacm_lan_stats_enable)
			{
				IPACMDBG_H("LAN stats functionality is not enabled, ignore IPA_LAN_CLIENT_DISCONNECT_EVENT.\n");
				return;
			}
			ipa_interface_index = iface_ipa_index_query(data->if_index);
			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG_H("Received IPA_LAN_CLIENT_DISCONNECT_EVENT\n");
				IPACM_Lan::handle_lan_client_disconnect(data->mac_addr);
			}
		}
		break;
#endif
#ifdef FEATURE_VLAN_MPDN
	case IPA_PREFIX_CHANGE_EVENT:
		{
			ipacm_event_data_fid *data = (ipacm_event_data_fid *)param;

			IPACMDBG_H("Received IPA_LAN_CLIENT_CONNECT_EVENT\n");
			if(ipa_if_num != data->if_index)
				modify_ipv6_prefix_flt_rule();
			else
				IPACMDBG_H("matching if index, ignoring\n", ipa_if_num);
		}
		break;
	case IPA_HANDLE_WAN_VLAN_PDN_UP:
		{
			ipacm_event_vlan_pdn *data = (ipacm_event_vlan_pdn *)param;

			IPACMDBG_H("Received IPA_HANDLE_WAN_VLAN_PDN_UP for VID %d, iptype %d\n",
				data->VlanID,
				data->iptype);
			if(is_vlan_IF(data->VlanID))
			{
				if(data->iptype == IPA_IP_v6)
				{
					/* new prefix was added - update flt rules */
					modify_ipv6_prefix_flt_rule();
#ifdef FEATURE_IPACM_UL_FIREWALL
					configure_v6_ul_firewall();
#endif
				}
				handle_vlan_pdn_up(data);
			}
		}
		break;
	case IPA_HANDLE_WAN_VLAN_PDN_DOWN:
		{
			ipacm_event_vlan_pdn *data = (ipacm_event_vlan_pdn *)param;

			IPACMDBG_H("Received IPA_HANDLE_WAN_VLAN_PDN_DOWN for VID %d, iptype %d\n",
				data->VlanID,
				data->iptype);
			if(is_vlan_IF(data->VlanID))
			{
#ifdef FEATURE_IPACM_UL_FIREWALL
				if(data->iptype == IPA_IP_v6)
				{
					// vlan pdn is down, disable its Q6 UL firewall and reconfigure
					disable_dft_firewall_rules_ul_ex(data->VlanID);
					configure_v6_ul_firewall();
				}
#endif
				handle_vlan_pdn_down(data);
			}
		}
		break;
#endif

	default:
		break;
	}

	return;
}
#if defined(FEATURE_L2TP_E2E) || defined(FEATURE_L2TP)
int IPACM_Lan::handle_l2tp_neigh(ipacm_event_data_all *data)
{
	if(IPACM_Iface::ipacmcfg->GetIPAVer() >= IPA_HW_None && IPACM_Iface::ipacmcfg->GetIPAVer() < IPA_HW_v4_0)
	{
		/* Add corresponding ipa_rm_resource_name of TX-endpoint up before IPV6 RT-rule set */
		IPACMDBG_H("dev %s add producer dependency\n", dev_name);
		if(tx_prop != NULL)
		{
			IPACMDBG_H("add rm dependency for L2TP interface.\n");
			IPACM_Iface::ipacmcfg->AddRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe], false);
		}
	}
#ifdef FEATURE_L2TP_E2E
	if(data->iptype == IPA_IP_v4)
	{
		int index;

		index = get_eth_client_index(data->mac_addr);
		if(index != IPACM_INVALID_INDEX)
		{
			IPACMERR("eth client is found/attached already with index %d \n", index);
			return IPACM_FAILURE;
		}
		if(num_eth_client >= IPA_MAX_NUM_ETH_CLIENTS)
		{
			IPACMERR("Reached maximum number(%d) of eth clients\n", IPA_MAX_NUM_ETH_CLIENTS);
			return IPACM_FAILURE;
		}

		/* Add NAT rules after ipv4 RT rules are set */
		CtList->HandleNeighIpAddrAddEvt(data);

		index = num_eth_client;
		if(install_l2tp_dl_rules(data, index) != IPACM_SUCCESS)
		{
			IPACMERR("Failed to add l2tp dl rules.\n");
			return IPACM_FAILURE;
		}

		if(install_l2tp_ul_rules(data, index) != IPACM_SUCCESS)
		{
			IPACMERR("Failed to add l2tp ul rules.\n");
			/* delete dl rules */
			m_routing.DeleteRoutingHdl(get_client_memptr(eth_client, index)->dl_first_pass_rt_rule_hdl, IPA_IP_v4);
			m_routing.DeleteRoutingHdl(get_client_memptr(eth_client, index)->dl_second_pass_rt_rule_hdl, IPA_IP_v6);
			m_header.DeleteHeaderProcCtx(get_client_memptr(eth_client, index)->dl_first_pass_hdr_proc_ctx_hdl);
			m_header.DeleteHeaderHdl(get_client_memptr(eth_client, index)->dl_first_pass_hdr_hdl);
			m_header.DeleteHeaderHdl(get_client_memptr(eth_client, index)->dl_second_pass_hdr_hdl);
			return IPACM_FAILURE;
		}
		num_eth_client++;
	}
#endif
#ifdef FEATURE_L2TP
	eth_bridge_post_event(IPA_ETH_BRIDGE_CLIENT_ADD, IPA_IP_MAX, data->mac_addr, NULL, data->iface_name);
#endif
	return 0;
}
#endif

int IPACM_Lan::del_ul_flt_rules(enum ipa_ip_type iptype)
{
	if (rx_prop == NULL)
	{
		IPACMERR("Rx prop is NULL, return\n");
		return IPACM_SUCCESS;
	}

	if(iptype == IPA_IP_v4)
	{
		if (num_wan_ul_fl_rule_v4 == 0)
		{
			IPACMERR("No modem UL rules were installed, return...\n");
			return IPACM_FAILURE;
		}
#ifdef FEATURE_IPACM_PER_CLIENT_STATS
		if(IPACM_Iface::ipacmcfg->ipacm_lan_stats_enable == false)
#endif
		{
			if(num_wan_ul_fl_rule_v4 > MAX_WAN_UL_FILTER_RULES)
			{
				IPACMERR("number of wan_ul_fl_rule_v4 (%d) > MAX_WAN_UL_FILTER_RULES (%d), aborting...\n", num_wan_ul_fl_rule_v4, MAX_WAN_UL_FILTER_RULES);
				return IPACM_FAILURE;
			}

			if(m_filtering.DeleteFilteringHdls(wan_ul_fl_rule_hdl_v4,
				IPA_IP_v4, num_wan_ul_fl_rule_v4) == false)
			{
				IPACMERR("Error Deleting RuleTable(1) to Filtering, aborting...\n");
				return IPACM_FAILURE;
			}
			IPACM_Iface::ipacmcfg->decreaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v4, num_wan_ul_fl_rule_v4);

			memset(wan_ul_fl_rule_hdl_v4, 0, MAX_WAN_UL_FILTER_RULES * sizeof(uint32_t));
			num_wan_ul_fl_rule_v4 = 0;
		}
#ifdef FEATURE_IPACM_PER_CLIENT_STATS
		else {
			if(delete_uplink_filter_rule(IPA_IP_v4) == IPACM_FAILURE)
			{
				IPACMERR("Error delete_uplink_filter_rule, aborting...\n");
				return IPACM_FAILURE;
			}
			num_wan_ul_fl_rule_v4 = 0;
		}
#endif
		modem_ul_v4_set = false;
	}
	else
	{
		if(num_wan_ul_fl_rule_v6 == 0)
		{
			IPACMERR("No modem UL rules were installed, return...\n");
			return IPACM_FAILURE;
		}
#ifdef FEATURE_IPACM_PER_CLIENT_STATS
		if(IPACM_Iface::ipacmcfg->ipacm_lan_stats_enable == false)
#endif
		{
			if(num_wan_ul_fl_rule_v6 > MAX_WAN_UL_FILTER_RULES)
			{
				IPACMERR(" the number of rules (%d) are bigger than array (%d), aborting...\n", num_wan_ul_fl_rule_v6, MAX_WAN_UL_FILTER_RULES);
				return IPACM_FAILURE;
			}

#ifndef FEATURE_L2TP_E2E
			/* When OCU is enabled, no need to delete modem UL IPv6 rules. */
			if(m_filtering.DeleteFilteringHdls(wan_ul_fl_rule_hdl_v6,
				IPA_IP_v6, num_wan_ul_fl_rule_v6) == false)
			{
				IPACMERR("Error Deleting RuleTable(1) to Filtering, aborting...\n");
				return IPACM_FAILURE;
			}
			IPACM_Iface::ipacmcfg->decreaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, num_wan_ul_fl_rule_v6);
			memset(wan_ul_fl_rule_hdl_v6, 0, MAX_WAN_UL_FILTER_RULES * sizeof(uint32_t));
			num_wan_ul_fl_rule_v6 = 0;
#endif
		}
#ifdef FEATURE_IPACM_PER_CLIENT_STATS
		else {
			if(delete_uplink_filter_rule(IPA_IP_v6) == IPACM_FAILURE)
			{
				IPACMERR("Error delete_uplink_filter_rule, aborting...\n");
				return IPACM_FAILURE;
			}
			num_wan_ul_fl_rule_v6 = 0;
		}
#endif
		modem_ul_v6_set = false;
	}

	return IPACM_SUCCESS;
}

#ifdef FEATURE_VLAN_MPDN
int IPACM_Lan::handle_vlan_neighbor(ipacm_event_data_all *data)
{
	ipacm_event_new_neigh_vlan *data_vlan;
	uint8_t vlan_id = 0;
	bool new_prefix = false;

	if (IPACM_Iface::ipacmcfg->get_vlan_id(data->iface_name, &vlan_id))
	{
		if(!IPACM_Iface::ipacmcfg->is_added_vlan_iface(data->iface_name))
		{
			IPACMDBG_H("ignoring neighbor of not added IF %s \n", data->iface_name);
			return 0;
		}
		IPACMERR("failed getting vlan ID of iface %s \n", data->iface_name);
		return IPACM_FAILURE;
	}

	IPACMDBG_H("VLAN IF %s got client, vlan id %d \n", data->iface_name, vlan_id);
	data_vlan = (ipacm_event_new_neigh_vlan *)data;

	if(data_vlan->data_all.iptype == IPA_IP_v6)
	{
		if(IPACM_Wan::is_global_ipv6_addr(data_vlan->data_all.ipv6_addr))
		{
			/* add ipv6 prefix */
			new_prefix = IPACM_Iface::ipacmcfg->add_vlan_ipv6_prefix(data_vlan->data_all.ipv6_addr, ipa_if_num);
		}
	}
	else if(data_vlan->data_all.iptype == IPA_IP_v4)
	{
		add_vlan_private_subnet(data_vlan->bridge);
	}

	/* first construc ETH full header */
	handle_eth_hdr_init(data->mac_addr, data_vlan->bridge, vlan_id, true);
	IPACMDBG_H("construct ETH header and route rules \n");
	/* Associate with IP and construct RT-rule */
	if(handle_eth_client_ipaddr(data) == IPACM_FAILURE)
	{
		return IPACM_FAILURE;
	}
	
	handle_eth_client_route_rule(data->mac_addr, data->iptype, vlan_id);

	/* Add NAT rules after ipv4 RT rules are set */
	HandleNeighIpAddrAddEvt(data);

	/*
	 * if this is the first time we have this global ipv6 prefix (or this
	 * is the default pdn prefix) we can notify WAN that it is a v6 vlan pdn
	 */
	if(new_prefix ||
		((IPACM_Wan::backhaul_ipv6_prefix[0] || IPACM_Wan::backhaul_ipv6_prefix[1]) &&
			(IPACM_Wan::backhaul_ipv6_prefix[0] == data_vlan->data_all.ipv6_addr[0]) &&
			(IPACM_Wan::backhaul_ipv6_prefix[1] == data_vlan->data_all.ipv6_addr[1])))
	{
		ipacm_cmd_q_data evt_data;
		ipacm_event_route_vlan *data;

		IPACMDBG_H("generating IPA_ROUTE_ADD_VLAN_PDN_EVENT, new_prefix %d\n", new_prefix);
		IPACMDBG_H("prefixes 0x[%X][%X], 0x[%X][%X]\n",
			IPACM_Wan::backhaul_ipv6_prefix[0],
			IPACM_Wan::backhaul_ipv6_prefix[1],
			data_vlan->data_all.ipv6_addr[0],
			data_vlan->data_all.ipv6_addr[1])

		evt_data.event = IPA_ROUTE_ADD_VLAN_PDN_EVENT;
		data = (ipacm_event_route_vlan *)malloc(sizeof(ipacm_event_route_vlan));
		if(!data)
		{
			IPACMERR("couldn't allocate memory for new vlan pdn event\n");
			return IPACM_FAILURE;
		}
		data->iptype = IPA_IP_v6;
		data->VlanID = vlan_id;
		data->wan_ipv6_prefix[0] = data_vlan->data_all.ipv6_addr[0];
		data->wan_ipv6_prefix[1] = data_vlan->data_all.ipv6_addr[1];
		evt_data.evt_data = data;
		IPACM_EvtDispatcher::PostEvt(&evt_data);
	}
	return IPACM_SUCCESS;
}

bool IPACM_Lan::is_vlan_IF(uint8_t vlan_id)
{
	char vlan_iface_name[IPA_RESOURCE_NAME_MAX];
	char vlan_suffix[5];

	/* concatenate the vlan id to the IF name and check iface exists */
	snprintf(vlan_suffix, sizeof(vlan_suffix), ".%d", vlan_id);
	strlcpy(vlan_iface_name, dev_name, sizeof(vlan_iface_name));
	if(strlcat(vlan_iface_name, vlan_suffix, sizeof(vlan_iface_name)) > IPA_RESOURCE_NAME_MAX)
	{
		IPACMERR("vlan IF name construction failed exceed length (%d)\n", strlen(vlan_iface_name));
		return false;
	}

	if(IPACM_Iface::ipacmcfg->is_added_vlan_iface(vlan_iface_name))
	{
		IPACMDBG_H("found VLAN IF named %s\n", vlan_iface_name);
		return true;
	}
	else
	{
		IPACMDBG_H("couldn't find VLAN IF named %s\n", vlan_iface_name);
	}

	return false;
}

int IPACM_Lan::check_vlan_PDNUp(enum ipa_ip_type iptype)
{
	int i = 0;
	ipacm_event_vlan_pdn vlan_data;
	uint8_t Ids[IPA_MAX_NUM_HW_PDNS];

	if(IPACM_Iface::ipacmcfg->get_iface_vlan_ids(dev_name, Ids))
	{
		IPACMERR("failed getting vlan ids for iface %s\n", dev_name);
		return IPACM_FAILURE;
	}

	if(iptype == IPA_IP_v4)
	{
		for(i = 0; i < IPA_MAX_NUM_HW_PDNS; i++)
		{
			uint8_t mux_id;

			if(Ids[i] != 0)
			{
				if(IPACM_Wan::GetMuxByVid(Ids[i], &mux_id, iptype))
				{
					IPACMDBG_H("no v4 vlan up PDN for Id %d\n", Ids[i]);
					continue;
				}

				/* create event data and call the handler */
				vlan_data.iptype = iptype;
				vlan_data.mux_id = mux_id;

				if(handle_vlan_pdn_up(&vlan_data))
				{
					IPACMERR("failed handling v4 VLAN up for VID %d, dev %s\n",
						Ids[i],
						dev_name);
				}
				else
				{
					IPACMDBG_H("handled v4 vlan pdn up for VID %d, dev %s\n",
						Ids[i],
						dev_name);
				}
			}
		}
	}
	else if(iptype == IPA_IP_v6)
	{
#ifdef FEATURE_IPACM_UL_FIREWALL
		bool firewall_updated = false;
#endif
		for(i = 0; i < IPA_MAX_NUM_HW_PDNS; i++)
		{
			uint8_t mux_id;

			if(Ids[i] != 0)
			{
				if(IPACM_Wan::GetMuxByVid(Ids[i], &mux_id, iptype))
				{
					IPACMDBG_H("no v6 vlan up PDN for Id %d\n", Ids[i]);
					continue;
				}
#ifdef FEATURE_IPACM_UL_FIREWALL
				if(!firewall_updated)
				{
					configure_v6_ul_firewall();
					firewall_updated = true;
				}
#endif
				modify_ipv6_prefix_flt_rule();

				/* create event data and call the handler */
				vlan_data.iptype = iptype;
				vlan_data.mux_id = mux_id;

				if(handle_vlan_pdn_up(&vlan_data))
				{
					IPACMERR("failed handling v6 VLAN up for VID %d, dev %s\n",
						Ids[i],
						dev_name);
				}
				else
				{
					IPACMDBG_H("handled v6 vlan pdn up for VID %d, dev %s\n",
						Ids[i],
						dev_name);
				}
			}
		}
	}
	else
	{
		IPACMERR("invalid iptype\n");
		return IPACM_FAILURE;
	}

	return IPACM_SUCCESS;
}

int IPACM_Lan::handle_vlan_pdn_up(ipacm_event_vlan_pdn *data)
{
	int ret;

	/* check only add static UL filter rule once */
	if(data->iptype == IPA_IP_v6)
	{
		IPACMDBG_H("IPA_IP_v6 num_dft_rt_v6 %d mux_id: %d modem_ul_v6_set: %d\n", num_dft_rt_v6, data->mux_id, modem_ul_v6_set);
		if(set_mux_up(data->mux_id, data->iptype))
		{
			IPACMERR("couldn't set mux up\n");
			return IPACM_FAILURE;
		}
		/* for the first PDN install UL filtering rules */
		if(num_dft_rt_v6 == 1 && modem_ul_v6_set == FALSE)
		{
			ret = handle_uplink_filter_rule(IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v6), data->iptype, data->mux_id, false);
			modem_ul_v6_set = true;
		}
		/* for the next PDNs only notify modem about new MUX IDs */
		else
		{
			ret = handle_uplink_filter_rule(IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v6), data->iptype, data->mux_id, true);
		}
	}
	else
	{
		IPACMDBG_H("IPA_IP_v4 mux_id: %d, modem_ul_v4_set %d\n", data->mux_id, modem_ul_v4_set);
		if(set_mux_up(data->mux_id, data->iptype))
		{
			IPACMERR("couldn't set mux up\n");
			return IPACM_FAILURE;
		}

		/* for the first PDN install UL filtering rules */
		if(modem_ul_v4_set == false)
		{
			ret = handle_uplink_filter_rule(IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v4), data->iptype, data->mux_id, false);
			modem_ul_v4_set = true;
		}
		/* for the next PDNs only notify modem about new MUX IDs */
		else
		{
			ret = handle_uplink_filter_rule(IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v4), data->iptype, data->mux_id, true);
		}
	}

	return ret;
}

int IPACM_Lan::handle_vlan_pdn_down(ipacm_event_vlan_pdn *data)
{
	bool notif_only = false;

	if(data->iptype == IPA_IP_v4)
	{
		/* if we still have vlan pdns up notify only */
		if(set_mux_down(data->mux_id, data->iptype))
			return IPACM_FAILURE;

		if(is_any_mux_up(data->iptype) == true)
			notif_only = true;

		if(!notif_only)
		{
			if(del_ul_flt_rules(IPA_IP_v4))
			{
				return IPACM_FAILURE;
			}
		}

		if(notify_flt_removed(data->mux_id))
		{
			return IPACM_FAILURE;
		}
	}
	else if (data->iptype == IPA_IP_v6)
	{
		/* if we still have vlan pdns up notify only */
		if(set_mux_down(data->mux_id, data->iptype))
			return IPACM_FAILURE;

		if(is_any_mux_up(data->iptype) == true)
			notif_only = true;

		/* prefixes list updated, install rules accordingly */
		modify_ipv6_prefix_flt_rule();

		if(!notif_only)
		{
			/* reset usb-client ipv6 rt-rules */
			handle_lan_client_reset_rt(IPA_IP_v6);

			if(del_ul_flt_rules(IPA_IP_v6))
			{
				return IPACM_FAILURE;
			}
		}

		if(notify_flt_removed(data->mux_id))
			return IPACM_FAILURE;
	}
	/* v4 and v6 were up and now down (rmnet_dataX is down)*/
	else
	{
		bool notif_only_v6 = false;

		/* if we still have vlan pdns up notify only */
		if(set_mux_down(data->mux_id, IPA_IP_v4))
			return IPACM_FAILURE;

		if(is_any_mux_up(IPA_IP_v4) == true)
			notif_only = true;

		/* if we still have vlan pdns up notify only */
		if(set_mux_down(data->mux_id, IPA_IP_v6))
			return IPACM_FAILURE;

		if(is_any_mux_up(IPA_IP_v6) == true)
			notif_only_v6 = true;

		/* prefixes list updated, install rules accordingly */
		modify_ipv6_prefix_flt_rule();

		if(!notif_only)
		{
			if(del_ul_flt_rules(IPA_IP_v4))
			{
				return IPACM_FAILURE;
			}
		}

		/* need to notify once for v4 */
		if(notify_flt_removed(data->mux_id))
			return IPACM_FAILURE;

		if(!notif_only_v6)
		{
			/* reset usb-client ipv6 rt-rules */
			handle_lan_client_reset_rt(IPA_IP_v6);

			if(del_ul_flt_rules(IPA_IP_v6))
			{
				return IPACM_FAILURE;
			}
		}

		/* need to notify once for v6 */
		if(notify_flt_removed(data->mux_id))
			return IPACM_FAILURE;
	}

	return IPACM_SUCCESS;
}
#endif

int IPACM_Lan::handle_del_ipv6_addr(ipacm_event_data_all *data)
{
	uint32_t tx_index;
	uint32_t rt_hdl;
	int num_v6 =0, clnt_indx;
	uint8_t vlan_id = 0;

#ifdef FEATURE_VLAN_MPDN
	if(is_vlan_event(data->iface_name))
	{
		IPACMDBG_H("handling vlan ETH client del v6 ip address for iface %s\n", data->iface_name);
		if(IPACM_Iface::ipacmcfg->get_vlan_id(data->iface_name, &vlan_id))
		{
			IPACMERR("failed getting vlan id for iface %s\n", data->iface_name);
			return IPACM_FAILURE;
		}
	}
#endif

	clnt_indx = get_eth_client_index(data->mac_addr, vlan_id);
	if (clnt_indx == IPACM_INVALID_INDEX)
	{
		IPACMERR("eth client not found/attached \n");
		return IPACM_FAILURE;
	}

	if(data->iptype == IPA_IP_v6)
	{
		if ((data->ipv6_addr[0] == 0) && (data->ipv6_addr[1] == 0) &&
			(data->ipv6_addr[2] == 0) || (data->ipv6_addr[3] == 0))
		{
			IPACMDBG_H("Received invalid IPv6 address\n");
		}

		IPACMDBG_H("ipv6 address got: 0x%x:%x:%x:%x\n",
			data->ipv6_addr[0], data->ipv6_addr[1], data->ipv6_addr[2], data->ipv6_addr[3]);
		for (num_v6 = 0; num_v6 < get_client_memptr(eth_client, clnt_indx)->ipv6_set; ++num_v6)
		{
			if (data->ipv6_addr[0] == get_client_memptr(eth_client, clnt_indx)->v6_addr[num_v6][0] &&
				data->ipv6_addr[1] == get_client_memptr(eth_client, clnt_indx)->v6_addr[num_v6][1] &&
				data->ipv6_addr[2] == get_client_memptr(eth_client, clnt_indx)->v6_addr[num_v6][2] &&
				data->ipv6_addr[3] == get_client_memptr(eth_client, clnt_indx)->v6_addr[num_v6][3])
			{
				IPACMDBG_H("ipv6 addr is found at position:%d for client:%d\n", num_v6, clnt_indx);
				break;
			}
		}
		if (num_v6 == get_client_memptr(eth_client, clnt_indx)->ipv6_set)
		{
			IPACMDBG_H("ipv6 addr is not found\n");
			return IPACM_FAILURE;
		}

		for(tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
		{
			if((tx_prop->tx[tx_index].ip == IPA_IP_v6) && (get_client_memptr(eth_client, clnt_indx)->route_rule_set_v6 != 0))
			{
				IPACMDBG_H("Delete client index %d ipv6 RT-rules for %d-st ipv6 for tx:%d\n", clnt_indx, num_v6, tx_index);
				rt_hdl = get_client_memptr(eth_client, clnt_indx)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v6[num_v6];
				if(m_routing.DeleteRoutingHdl(rt_hdl, IPA_IP_v6) == false)
				{
					return IPACM_FAILURE;
				}
				rt_hdl = get_client_memptr(eth_client, clnt_indx)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v6_wan[num_v6];
				if(m_routing.DeleteRoutingHdl(rt_hdl, IPA_IP_v6) == false)
				{
					return IPACM_FAILURE;
				}
				get_client_memptr(eth_client, clnt_indx)->ipv6_set--;
				get_client_memptr(eth_client, clnt_indx)->route_rule_set_v6--;

				for(num_v6;num_v6< get_client_memptr(eth_client, clnt_indx)->ipv6_set;num_v6++)
				{
					get_client_memptr(eth_client, clnt_indx)->v6_addr[num_v6][0] =
						get_client_memptr(eth_client, clnt_indx)->v6_addr[num_v6+1][0];
					get_client_memptr(eth_client, clnt_indx)->v6_addr[num_v6][1] =
						get_client_memptr(eth_client, clnt_indx)->v6_addr[num_v6+1][1];
					get_client_memptr(eth_client, clnt_indx)->v6_addr[num_v6][2] =
						get_client_memptr(eth_client, clnt_indx)->v6_addr[num_v6+1][2];
					get_client_memptr(eth_client, clnt_indx)->v6_addr[num_v6][3] =
						get_client_memptr(eth_client, clnt_indx)->v6_addr[num_v6+1][3];
					get_client_memptr(eth_client, clnt_indx)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v6[num_v6] =
						get_client_memptr(eth_client, clnt_indx)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v6[num_v6+1];
					get_client_memptr(eth_client, clnt_indx)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v6_wan[num_v6] =
						get_client_memptr(eth_client, clnt_indx)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v6_wan[num_v6+1];
				}
			}
		}
	}
	return IPACM_SUCCESS;
}

int IPACM_Lan::notify_flt_removed(uint8_t mux_id)
{
	ipa_fltr_installed_notif_req_msg_v01 flt_index;
	int fd;

	fd = open(IPA_DEVICE_NAME, O_RDWR);
	if(0 == fd)
	{
		IPACMERR("Failed opening %s.\n", IPA_DEVICE_NAME);
		return IPACM_FAILURE;
	}

	if (rx_prop == NULL)
	{
		IPACMERR("Rx prop is NULL, return\n");
		return IPACM_SUCCESS;
	}
	memset(&flt_index, 0, sizeof(flt_index));
	flt_index.source_pipe_index = ioctl(fd, IPA_IOC_QUERY_EP_MAPPING, rx_prop->rx[0].src_pipe);
	flt_index.install_status = IPA_QMI_RESULT_SUCCESS_V01;
#ifndef FEATURE_IPA_V3
	flt_index.filter_index_list_len = 0;
#else /* defined (FEATURE_IPA_V3) */
	flt_index.rule_id_valid = 1;
	flt_index.rule_id_len = 0;
#endif
	flt_index.embedded_pipe_index_valid = 1;
	flt_index.embedded_pipe_index = ioctl(fd, IPA_IOC_QUERY_EP_MAPPING, IPA_CLIENT_APPS_LAN_WAN_PROD);
	flt_index.retain_header_valid = 1;
	flt_index.retain_header = 0;
	flt_index.embedded_call_mux_id_valid = 1;
	flt_index.embedded_call_mux_id = mux_id;

	if(false == m_filtering.SendFilteringRuleIndex(&flt_index))
	{
		IPACMERR("Error sending filtering rule index, aborting...\n");
		close(fd);
		return IPACM_FAILURE;
	}

	close(fd);
	return IPACM_SUCCESS;
}

/* delete filter rule for wan_down event for IPv4*/
int IPACM_Lan::handle_wan_down(bool is_sta_mode)
{
	if (rx_prop == NULL)
	{
		IPACMERR("Rx prop is NULL, return\n");
		return IPACM_SUCCESS;
	}
	
	if(is_sta_mode == false)
	{
		if(del_ul_flt_rules(IPA_IP_v4))
			return IPACM_FAILURE;

		if(notify_flt_removed(IPACM_Iface::ipacmcfg->GetQmapId()))
			return IPACM_FAILURE;
	}
	else
	{
		if (m_filtering.DeleteFilteringHdls(&lan_wan_fl_rule_hdl[0], IPA_IP_v4, 1) == false)
		{
			IPACMERR("Error Adding RuleTable(1) to Filtering, aborting...\n");
			return IPACM_FAILURE;
		}
		IPACM_Iface::ipacmcfg->decreaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v4, 1);
	}

	return IPACM_SUCCESS;
}

/* handle new_address event*/
int IPACM_Lan::handle_addr_evt(ipacm_event_data_addr *data)
{
	struct ipa_ioc_add_rt_rule *rt_rule;
	struct ipa_rt_rule_add *rt_rule_entry;
	const int NUM_RULES = 1;
	int num_ipv6_addr;
	int res = IPACM_SUCCESS;

	IPACMDBG_H("set route/filter rule ip-type: %d \n", data->iptype);

/* Add private subnet*/
#ifdef FEATURE_IPA_ANDROID
	if (data->iptype == IPA_IP_v4)
	{
		IPACMDBG_H("current IPACM private subnet_addr number(%d)\n", IPACM_Iface::ipacmcfg->ipa_num_private_subnet);
		if_ipv4_subnet = (data->ipv4_addr >> 8) << 8;
		IPACMDBG_H(" Add IPACM private subnet_addr as: 0x%x \n", if_ipv4_subnet);
		if(IPACM_Iface::ipacmcfg->AddPrivateSubnet(if_ipv4_subnet, ipa_if_num) == false)
		{
			IPACMERR(" can't Add IPACM private subnet_addr as: 0x%x \n", if_ipv4_subnet);
		}
	}
#endif /* defined(FEATURE_IPA_ANDROID)*/

	/* Update the IP Type. */
	config_ip_type(data->iptype);

	if (data->iptype == IPA_IP_v4)
	{
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
		rt_rule_entry->at_rear = false;
		rt_rule_entry->rule.dst = IPA_CLIENT_APPS_LAN_CONS;  //go to A5
		rt_rule_entry->rule.attrib.attrib_mask = IPA_FLT_DST_ADDR;
		strlcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_lan_v4.name, sizeof(rt_rule->rt_tbl_name));
		rt_rule_entry->rule.attrib.u.v4.dst_addr      = data->ipv4_addr;
		rt_rule_entry->rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;
#ifdef FEATURE_IPA_V3
		rt_rule_entry->rule.hashable = true;
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
		IPACMDBG_H("ipv4 iface rt-rule hdl1=0x%x\n", dft_rt_rule_hdl[0]);
		/* initial multicast/broadcast/fragment filter rule */

		init_fl_rule(data->iptype);
#ifdef FEATURE_L2TP
		if(ipa_if_cate == WLAN_IF)
		{
			add_tcp_syn_flt_rule(data->iptype);
		}
#endif
		install_ipv4_icmp_flt_rule();

		/* populate the flt rule offset for eth bridge */
		eth_bridge_flt_rule_offset[data->iptype] = ipv4_icmp_flt_rule_hdl[0];
		eth_bridge_post_event(IPA_ETH_BRIDGE_IFACE_UP, IPA_IP_v4, NULL, NULL, NULL);
	}
	else
	{
	    /* check if see that v6-addr already or not*/
	    for(num_ipv6_addr=0;num_ipv6_addr<num_dft_rt_v6;num_ipv6_addr++)
	    {
            if((ipv6_addr[num_ipv6_addr][0] == data->ipv6_addr[0]) &&
	           (ipv6_addr[num_ipv6_addr][1] == data->ipv6_addr[1]) &&
	           (ipv6_addr[num_ipv6_addr][2] == data->ipv6_addr[2]) &&
	           (ipv6_addr[num_ipv6_addr][3] == data->ipv6_addr[3]))
            {
				return IPACM_FAILURE;
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
		rt_rule_entry->at_rear = false;
		rt_rule_entry->rule.dst = IPA_CLIENT_APPS_LAN_CONS;  //go to A5
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
		rt_rule_entry->rule.hashable = true;
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

		if (num_dft_rt_v6 == 0)
		{
#ifdef FEATURE_L2TP
			if(ipa_if_cate == WLAN_IF)
			{
				add_tcp_syn_flt_rule(data->iptype);
			}
			else if(ipa_if_cate == ODU_IF)
			{
				add_tcp_syn_flt_rule_l2tp(IPA_IP_v4);
				add_tcp_syn_flt_rule_l2tp(IPA_IP_v6);
			}
#endif
			install_ipv6_icmp_flt_rule();

			/* populate the flt rule offset for eth bridge */
			eth_bridge_flt_rule_offset[data->iptype] = ipv6_icmp_flt_rule_hdl[0];
			eth_bridge_post_event(IPA_ETH_BRIDGE_IFACE_UP, IPA_IP_v6, NULL, NULL, NULL);

			init_fl_rule(data->iptype);
		}
		num_dft_rt_v6++;
		IPACMDBG_H("number of default route rules %d\n", num_dft_rt_v6);
	}

	IPACMDBG_H("finish route/filter rule ip-type: %d, res(%d)\n", data->iptype, res);

fail:
	free(rt_rule);
	return res;
}

/* configure private subnet filter rules*/
int IPACM_Lan::handle_private_subnet(ipa_ip_type iptype)
{
	struct ipa_flt_rule_add flt_rule_entry;
	int i;

	ipa_ioc_add_flt_rule *m_pFilteringTable;

	IPACMDBG_H("lan->handle_private_subnet(); set route/filter rule \n");

	if (rx_prop == NULL)
	{
		IPACMDBG_H("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	if (iptype == IPA_IP_v4)
	{

		m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)
			 calloc(1,
							sizeof(struct ipa_ioc_add_flt_rule) +
							(IPACM_Iface::ipacmcfg->ipa_num_private_subnet) * sizeof(struct ipa_flt_rule_add)
							);
		if (!m_pFilteringTable)
		{
			PERROR("Error Locate ipa_flt_rule_add memory...\n");
			return IPACM_FAILURE;
		}
		m_pFilteringTable->commit = 1;
		m_pFilteringTable->ep = rx_prop->rx[0].src_pipe;
		m_pFilteringTable->global = false;
		m_pFilteringTable->ip = IPA_IP_v4;
		m_pFilteringTable->num_rules = (uint8_t)IPACM_Iface::ipacmcfg->ipa_num_private_subnet;

		/* Make LAN-traffic always go A5, use default IPA-RT table */
		if (false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_default_v4))
		{
			IPACMERR("LAN m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_default_v4=0x%p) Failed.\n", &IPACM_Iface::ipacmcfg->rt_tbl_default_v4);
			free(m_pFilteringTable);
			return IPACM_FAILURE;
		}

		for (i = 0; i < (IPACM_Iface::ipacmcfg->ipa_num_private_subnet); i++)
		{
			memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
			flt_rule_entry.at_rear = true;
			flt_rule_entry.rule.retain_hdr = 1;
			flt_rule_entry.flt_rule_hdl = -1;
			flt_rule_entry.status = -1;
			flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
#ifdef FEATURE_IPA_V3
			flt_rule_entry.rule.hashable = true;
#endif
                        /* Support private subnet feature including guest-AP can't talk to primary AP etc */
			flt_rule_entry.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_default_v4.hdl;
			IPACMDBG_H(" private filter rule use table: %s\n",IPACM_Iface::ipacmcfg->rt_tbl_default_v4.name);

			memcpy(&flt_rule_entry.rule.attrib,
						 &rx_prop->rx[0].attrib,
						 sizeof(flt_rule_entry.rule.attrib));
			flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
			flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = IPACM_Iface::ipacmcfg->private_subnet_table[i].subnet_mask;
			flt_rule_entry.rule.attrib.u.v4.dst_addr = IPACM_Iface::ipacmcfg->private_subnet_table[i].subnet_addr;
			memcpy(&(m_pFilteringTable->rules[i]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
			IPACMDBG_H("Loop %d  5\n", i);
		}

		if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
		{
			IPACMERR("Error Adding RuleTable(0) to Filtering, aborting...\n");
			free(m_pFilteringTable);
			return IPACM_FAILURE;
		}
		IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v4, IPACM_Iface::ipacmcfg->ipa_num_private_subnet);

		/* copy filter rule hdls */
		for (i = 0; i < IPACM_Iface::ipacmcfg->ipa_num_private_subnet; i++)
		{
			private_fl_rule_hdl[i] = m_pFilteringTable->rules[i].flt_rule_hdl;
		}
		free(m_pFilteringTable);
	}
	else
	{
		IPACMDBG_H("No private subnet rules for ipv6 iface %s\n", dev_name);
	}
	return IPACM_SUCCESS;
}

#ifdef FEATURE_VLAN_MPDN
int IPACM_Lan::add_vlan_private_subnet(ipacm_bridge *bridge)
{
	int i;
	struct ipa_flt_rule_add flt_rule_entry;
	ipa_ioc_add_flt_rule *m_pFilteringTable;
	ipacm_event_data_fid *data_fid;
	ipacm_cmd_q_data evt_data;

	if(rx_prop == NULL)
	{
		IPACMDBG_H("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	IPACMDBG_H("(%s) handle_vlan_private_subnet (0x%X & 0x%X)\n",
		bridge->bridge_name,
		bridge->bridge_netmask,
		bridge->bridge_ipv4_addr);

	for(i = 0; i < IPACM_Iface::ipacmcfg->ipa_num_private_subnet; i++)
	{
		if((IPACM_Iface::ipacmcfg->private_subnet_table[i].subnet_mask &
			IPACM_Iface::ipacmcfg->private_subnet_table[i].subnet_addr) ==
			(bridge->bridge_netmask & bridge->bridge_ipv4_addr))
		{
			IPACMDBG_H("(%s) private subnet was already added for (0x%X & 0x%X)\n",
				bridge->bridge_name,
				IPACM_Iface::ipacmcfg->private_subnet_table[i].subnet_mask,
				IPACM_Iface::ipacmcfg->private_subnet_table[i].subnet_addr);
			return IPACM_SUCCESS;
		}
	}

	if(IPACM_Iface::ipacmcfg->ipa_num_private_subnet >= IPA_MAX_PRIVATE_SUBNET_ENTRIES)
	{
		IPACMERR("IPACM private subnet_addr overflow, total entry(%d)\n", IPACM_Iface::ipacmcfg->ipa_num_private_subnet);
		return IPACM_FAILURE;
	}

	IPACM_Iface::ipacmcfg->private_subnet_table[IPACM_Iface::ipacmcfg->ipa_num_private_subnet].subnet_mask = bridge->bridge_netmask;
	IPACM_Iface::ipacmcfg->private_subnet_table[IPACM_Iface::ipacmcfg->ipa_num_private_subnet].subnet_addr = bridge->bridge_ipv4_addr & bridge->bridge_netmask;
	IPACM_Iface::ipacmcfg->ipa_num_private_subnet++;

	/* handle private subnet change for this interface first*/
	modify_private_subnet();

	/* notify other ifaces about this subnet */
	data_fid = (ipacm_event_data_fid *)malloc(sizeof(ipacm_event_data_fid));
	if(data_fid == NULL)
	{
		IPACMERR("unable to allocate memory for event data_fid\n");
		return IPACM_FAILURE;
	}
	data_fid->if_index = ipa_if_num; // already ipa index, not fid index
	evt_data.event = IPA_PRIVATE_SUBNET_CHANGE_EVENT;
	evt_data.evt_data = data_fid;

	/* Insert IPA_PRIVATE_SUBNET_CHANGE_EVENT to command queue */
	IPACM_EvtDispatcher::PostEvt(&evt_data);

	return IPACM_SUCCESS;
}
#endif


/* for STA mode wan up:  configure filter rule for wan_up event*/
int IPACM_Lan::handle_wan_up(ipa_ip_type ip_type)
{
	struct ipa_flt_rule_add flt_rule_entry;
	int len = 0;
	ipa_ioc_add_flt_rule *m_pFilteringTable;

	IPACMDBG_H("set WAN interface as default filter rule\n");

	if (rx_prop == NULL)
	{
		IPACMDBG_H("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	if(ip_type == IPA_IP_v4)
	{
		len = sizeof(struct ipa_ioc_add_flt_rule) + (1 * sizeof(struct ipa_flt_rule_add));
		m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)calloc(1, len);
		if (m_pFilteringTable == NULL)
		{
			PERROR("Error Locate ipa_flt_rule_add memory...\n");
			return IPACM_FAILURE;
		}

		m_pFilteringTable->commit = 1;
		m_pFilteringTable->ep = rx_prop->rx[0].src_pipe;
		m_pFilteringTable->global = false;
		m_pFilteringTable->ip = IPA_IP_v4;
		m_pFilteringTable->num_rules = (uint8_t)1;

		IPACMDBG_H("Retrieving routing hanle for table: %s\n",
						 IPACM_Iface::ipacmcfg->rt_tbl_wan_v4.name);
		if (false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_wan_v4))
		{
			IPACMERR("m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_wan_v4=0x%p) Failed.\n",
							 &IPACM_Iface::ipacmcfg->rt_tbl_wan_v4);
			free(m_pFilteringTable);
			return IPACM_FAILURE;
		}
		IPACMDBG_H("Routing hanle for table: %d\n", IPACM_Iface::ipacmcfg->rt_tbl_wan_v4.hdl);


		memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
		flt_rule_entry.at_rear = true;
		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;
		if(IPACM_Wan::isWan_Bridge_Mode())
		{
			flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
		}
		else
		{
			flt_rule_entry.rule.action = IPA_PASS_TO_SRC_NAT;
		}
#ifdef FEATURE_IPA_V3
		flt_rule_entry.rule.hashable = true;
#endif
		flt_rule_entry.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_wan_v4.hdl;

		memcpy(&flt_rule_entry.rule.attrib,
					 &rx_prop->rx[0].attrib,
					 sizeof(flt_rule_entry.rule.attrib));

		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = 0x0;
		flt_rule_entry.rule.attrib.u.v4.dst_addr = 0x0;

		memcpy(&m_pFilteringTable->rules[0], &flt_rule_entry, sizeof(flt_rule_entry));
		if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
		{
			IPACMERR("Error Adding RuleTable(0) to Filtering, aborting...\n");
			free(m_pFilteringTable);
			return IPACM_FAILURE;
		}
		else
		{
			IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v4, 1);
			IPACMDBG_H("flt rule hdl0=0x%x, status=0x%x\n",
							 m_pFilteringTable->rules[0].flt_rule_hdl,
							 m_pFilteringTable->rules[0].status);
		}


		/* copy filter hdls  */
		lan_wan_fl_rule_hdl[0] = m_pFilteringTable->rules[0].flt_rule_hdl;
		free(m_pFilteringTable);
	}
	else if(ip_type == IPA_IP_v6)
	{
		/* add default v6 filter rule */
		m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)
			 calloc(1, sizeof(struct ipa_ioc_add_flt_rule) +
					1 * sizeof(struct ipa_flt_rule_add));

		if (!m_pFilteringTable)
		{
			PERROR("Error Locate ipa_flt_rule_add memory...\n");
			return IPACM_FAILURE;
		}

		m_pFilteringTable->commit = 1;
		m_pFilteringTable->ep = rx_prop->rx[0].src_pipe;
		m_pFilteringTable->global = false;
		m_pFilteringTable->ip = IPA_IP_v6;
		m_pFilteringTable->num_rules = (uint8_t)1;

		if (false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_v6))
		{
			IPACMERR("m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_v6=0x%p) Failed.\n", &IPACM_Iface::ipacmcfg->rt_tbl_v6);
			free(m_pFilteringTable);
			return IPACM_FAILURE;
		}

		memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

		flt_rule_entry.at_rear = true;
		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;
		if (IPACM_Iface::ipacmcfg->IsIpv6CTEnabled() && !IPACM_Wan::isWan_Bridge_Mode())
		{
			flt_rule_entry.rule.action = IPA_PASS_TO_SRC_NAT;
		}
		else
		{
			flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
		}

#ifdef FEATURE_IPA_V3
		flt_rule_entry.rule.hashable = true;
#endif
		flt_rule_entry.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_v6.hdl;

		memcpy(&flt_rule_entry.rule.attrib,
					 &rx_prop->rx[0].attrib,
					 sizeof(flt_rule_entry.rule.attrib));

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
			IPACMERR("Error Adding Filtering rule, aborting...\n");
			free(m_pFilteringTable);
			return IPACM_FAILURE;
		}
		else
		{
			IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, 1);
			IPACMDBG_H("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
		}

		/* copy filter hdls */
		dft_v6fl_rule_hdl[m_ipv6_default_filterting_rules_count] = m_pFilteringTable->rules[0].flt_rule_hdl;
		free(m_pFilteringTable);
	}

	return IPACM_SUCCESS;
}

int IPACM_Lan::handle_wan_up_ex(ipacm_ext_prop *ext_prop, ipa_ip_type iptype, uint8_t xlat_mux_id)
{
	int fd, ret = IPACM_SUCCESS, cnt;
	IPACM_Config* ipacm_config = IPACM_Iface::ipacmcfg;
	struct ipa_ioc_write_qmapid mux;
	int i=0;

	/* not  needed for newer versions since it will be overridden by NAT metadata replacement for IPAv4 and up */
	if((IPACM_Iface::ipacmcfg->GetIPAVer() < IPA_HW_v4_0) && (rx_prop != NULL))
	{
		/* give mux ID of the default PDN to IPA-driver for WLAN/LAN pkts */
		fd = open(IPA_DEVICE_NAME, O_RDWR);
		if (0 == fd)
		{
			IPACMDBG_H("Failed opening %s.\n", IPA_DEVICE_NAME);
			return IPACM_FAILURE;
		}

		mux.qmap_id = ipacm_config->GetQmapId();
		for(cnt=0; cnt<rx_prop->num_rx_props; cnt++)
		{
			mux.client = rx_prop->rx[cnt].src_pipe;
			ret = ioctl(fd, IPA_IOC_WRITE_QMAPID, &mux);
			if (ret)
			{
				IPACMERR("Failed to write mux id %d\n", mux.qmap_id);
				close(fd);
				return IPACM_FAILURE;
			}
		}
		close(fd);
	}

#ifdef FEATURE_IPACM_PER_CLIENT_STATS
	/* Install filter rules for the client. */
	if (IPACM_Iface::ipacmcfg->ipacm_lan_stats_enable == true)
	{
		if (enable_per_client_stats(&IPACM_Iface::ipacmcfg->ipacm_lan_stats_enable))
		{
			IPACMERR("Failed to enable per client stats %d\n", IPACM_Iface::ipacmcfg->ipacm_lan_stats_enable);
			return IPACM_FAILURE;
		}
	}
#endif

	/* check only add static UL filter rule once */
	if(iptype == IPA_IP_v6)
	{
		if(num_dft_rt_v6 == 1 && modem_ul_v6_set == FALSE)
		{
			IPACMDBG_H("IPA_IP_v6 num_dft_rt_v6 %d xlat_mux_id: %d modem_ul_v6_set: %d\n", num_dft_rt_v6, xlat_mux_id, modem_ul_v6_set);
#ifdef FEATURE_L2TP_E2E
			if(ipa_if_cate == ODU_IF)
			{
				IPACMDBG_H("not installing modem UL IPv6 rules in L2TP E2E use case.\n");
				return IPACM_SUCCESS;
			}
#endif
#ifdef FEATURE_VLAN_MPDN
			ret = handle_uplink_filter_rule(ext_prop, iptype, xlat_mux_id, false, true);
#else
			ret = handle_uplink_filter_rule(ext_prop, iptype, xlat_mux_id);
#endif
			modem_ul_v6_set = true;
		}
#ifdef FEATURE_VLAN_MPDN
		else
		{
			ret = handle_uplink_filter_rule(ext_prop, iptype, xlat_mux_id, true, true);
		}
#endif
	}
	else if(iptype == IPA_IP_v4)
	{
		if(modem_ul_v4_set == false)
		{
			IPACMDBG_H("IPA_IP_v4 xlat_mux_id: %d, modem_ul_v4_set %d\n", xlat_mux_id, modem_ul_v4_set);
#ifdef FEATURE_VLAN_MPDN
			ret = handle_uplink_filter_rule(ext_prop, iptype, xlat_mux_id, false, true);
#else
			ret = handle_uplink_filter_rule(ext_prop, iptype, xlat_mux_id);
#endif
			modem_ul_v4_set = true;
		}
#ifdef FEATURE_VLAN_MPDN
		else
		{
			ret = handle_uplink_filter_rule(ext_prop, iptype, xlat_mux_id, true, true);
		}
#endif
	}
	else
	{
		IPACMDBG_H("ip-type: %d modem_ul_v4_set: %d, modem_ul_v6_set %d\n",
			iptype, modem_ul_v4_set, modem_ul_v6_set);
	}

#ifdef FEATURE_IPACM_PER_CLIENT_STATS
	/* Install filter rules for the client. */
	if (IPACM_Iface::ipacmcfg->ipacm_lan_stats_enable == true)
	{
		IPACMDBG_H("xlat_mux_id: %d, iptype %d\n", xlat_mux_id, iptype);
		ret = install_uplink_filter_rule(ext_prop, iptype, xlat_mux_id);
	}
#endif

	return ret;
}

/* handle ETH client initial, construct full headers (tx property) */
int IPACM_Lan::handle_eth_hdr_init(uint8_t *mac_addr, ipacm_bridge *bridge, uint8_t vlan_id, bool isVlan)
{

#define ETH_IFACE_INDEX_LEN 2
#define VLAN_TPID_SIZE 2
#define VLAN_VID_MASK 0x0FFF

	int res = IPACM_SUCCESS, len = 0;
	char index[ETH_IFACE_INDEX_LEN];
	struct ipa_ioc_copy_hdr sCopyHeader;
	struct ipa_ioc_add_hdr *pHeaderDescriptor = NULL;
	uint32_t cnt;
	int clnt_indx;
#ifdef FEATURE_IPACM_PER_CLIENT_STATS
	struct wan_ioctl_lan_client_info *client_info;
	ipacm_ext_prop* ext_prop;
	int max_clients = (IPACM_Iface::ipacmcfg->ipacm_lan_stats_enable) ? IPA_MAX_NUM_HW_PATH_CLIENTS:
		IPA_MAX_NUM_ETH_CLIENTS;
#else
	int max_clients = IPA_MAX_NUM_ETH_CLIENTS;
#endif
#ifdef FEATURE_VLAN_MPDN
	if(isVlan)
	{
		clnt_indx = get_eth_client_index(mac_addr, vlan_id);
	}
	else
#endif
	{
		clnt_indx = get_eth_client_index(mac_addr);
	}

	if (clnt_indx != IPACM_INVALID_INDEX)
	{
		IPACMERR("eth client is found/attached already with index %d \n", clnt_indx);
		return IPACM_FAILURE;
	}

	/* add header to IPA */
	if (num_eth_client >= max_clients)
	{
		IPACMERR("Reached maximum number(%d) of eth clients\n", max_clients);
		return IPACM_FAILURE;
	}

	IPACMDBG_H("ETH client number: %d\n", num_eth_client);

	memcpy(get_client_memptr(eth_client, num_eth_client)->mac,
				 mac_addr,
				 sizeof(get_client_memptr(eth_client, num_eth_client)->mac));
#ifdef FEATURE_VLAN_MPDN
	if (isVlan)
	{
		get_client_memptr(eth_client, num_eth_client)->vlan_id = vlan_id;
	}
#endif
	IPACMDBG_H("Received Client MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					 mac_addr[0], mac_addr[1], mac_addr[2],
					 mac_addr[3], mac_addr[4], mac_addr[5]);

	IPACMDBG_H("stored MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					 get_client_memptr(eth_client, num_eth_client)->mac[0],
					 get_client_memptr(eth_client, num_eth_client)->mac[1],
					 get_client_memptr(eth_client, num_eth_client)->mac[2],
					 get_client_memptr(eth_client, num_eth_client)->mac[3],
					 get_client_memptr(eth_client, num_eth_client)->mac[4],
					 get_client_memptr(eth_client, num_eth_client)->mac[5]);
#ifdef FEATURE_VLAN_MPDN
	IPACMDBG_H("isvlan %d, vlan_id %d\n", isVlan, vlan_id);
#endif

	/* add header to IPA */
	if(tx_prop != NULL)
	{
#ifdef FEATURE_VLAN_MPDN
		if(isVlan && !bridge)
		{
			IPACMERR("vlan with NULL bridge\n");
			return IPACM_FAILURE;
		}
#endif

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

				IPACMDBG_H("header length: %d, partial: %d\n", sCopyHeader.hdr_len, sCopyHeader.is_partial);
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
				if (sCopyHeader.is_eth2_ofst_valid)
				{
					memcpy(&pHeaderDescriptor->hdr[0].hdr[sCopyHeader.eth2_ofst],
					mac_addr,
					IPA_MAC_ADDR_SIZE);
				}
				/* replace src mac to bridge mac_addr if any  */
#ifdef FEATURE_VLAN_MPDN
				/* 802.1Q header (comes after dst and src MAC)
				   --------------------------------------------
				   |    0   |    1   |     2    |    3        |
				   --------------------------------------------
				   |       TPID(2B)  |       TCI(2B)          |
				   --------------------------------------------
				   |                 |   PCP+|  VLAN ID(12b)  |
				   |                 |DEI(4b)|                |
				   --------------------------------------------
				*/
				if(isVlan)
				{
					uint16_t vlan_tci =
						(*((uint16_t *)&(pHeaderDescriptor->hdr[0].hdr[sCopyHeader.eth2_ofst +
							2 * IPA_MAC_ADDR_SIZE +
							VLAN_TPID_SIZE])));
					vlan_tci = (vlan_tci & ~VLAN_VID_MASK) | (vlan_id & VLAN_VID_MASK);
					/* change vlan_tci to HW format */
					vlan_tci = htons(vlan_tci);
					memcpy(&pHeaderDescriptor->hdr[0].hdr[sCopyHeader.eth2_ofst +
						2 * IPA_MAC_ADDR_SIZE +
						VLAN_TPID_SIZE],
						&vlan_tci,
						sizeof(vlan_tci));
					IPACMDBG_H("v4: updated the vlan_tci, now 0x%X, vlan tag is 0x%X\n", vlan_tci,
						*((uint32_t *)&(pHeaderDescriptor->hdr[0].hdr[sCopyHeader.eth2_ofst +
							2 * IPA_MAC_ADDR_SIZE])));
				}

				/* VLAN case */
				if(bridge)
				{
					memcpy(&pHeaderDescriptor->hdr[0].hdr[sCopyHeader.eth2_ofst +
					IPA_MAC_ADDR_SIZE],
					bridge->bridge_mac,
					IPA_MAC_ADDR_SIZE);
					IPACMDBG_H("device is in bridge mode (VLAN), MAC 0x[%X][%X][%X][%X][%X][%X]\n",
						bridge->bridge_mac[0],
						bridge->bridge_mac[1],
						bridge->bridge_mac[2],
						bridge->bridge_mac[3],
						bridge->bridge_mac[4],
						bridge->bridge_mac[5]);
				}
				/* non VLAN case */
				else
#endif
				if(IPACM_Iface::ipacmcfg->ipa_bridge_enable)
				{
					memcpy(&pHeaderDescriptor->hdr[0].hdr[sCopyHeader.eth2_ofst + IPA_MAC_ADDR_SIZE],
						IPACM_Iface::ipacmcfg->bridge_mac,
						IPA_MAC_ADDR_SIZE);
					IPACMDBG_H("device is in bridge mode (XML), MAC 0x[%X][%X][%X][%X][%X][%X]\n",
						IPACM_Iface::ipacmcfg->bridge_mac[0],
						IPACM_Iface::ipacmcfg->bridge_mac[1],
						IPACM_Iface::ipacmcfg->bridge_mac[2],
						IPACM_Iface::ipacmcfg->bridge_mac[3],
						IPACM_Iface::ipacmcfg->bridge_mac[4],
						IPACM_Iface::ipacmcfg->bridge_mac[5]);
				}

				pHeaderDescriptor->commit = true;
				pHeaderDescriptor->num_hdrs = 1;

				memset(pHeaderDescriptor->hdr[0].name, 0,
								sizeof(pHeaderDescriptor->hdr[0].name));

				snprintf(index,sizeof(index), "%d", ipa_if_num);
				strlcpy(pHeaderDescriptor->hdr[0].name, index, sizeof(pHeaderDescriptor->hdr[0].name));
				pHeaderDescriptor->hdr[0].name[IPA_RESOURCE_NAME_MAX-1] = '\0';
				if (strlcat(pHeaderDescriptor->hdr[0].name, IPA_ETH_HDR_NAME_v4, sizeof(pHeaderDescriptor->hdr[0].name)) > IPA_RESOURCE_NAME_MAX)
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
				hdr_len = sCopyHeader.hdr_len;
				pHeaderDescriptor->hdr[0].hdr_hdl = -1;
				pHeaderDescriptor->hdr[0].is_partial = 0;
				pHeaderDescriptor->hdr[0].status = -1;

				if(m_header.AddHeader(pHeaderDescriptor) == false ||
					pHeaderDescriptor->hdr[0].status != 0)
				{
					IPACMERR("ioctl IPA_IOC_ADD_HDR failed: %d\n", pHeaderDescriptor->hdr[0].status);
					res = IPACM_FAILURE;
					goto fail;
				}

				get_client_memptr(eth_client, num_eth_client)->hdr_hdl_v4 = pHeaderDescriptor->hdr[0].hdr_hdl;
				IPACMDBG_H("eth-client(%d) v4 full header name:%s header handle:(0x%x), Len:%d\n",
												num_eth_client,
												pHeaderDescriptor->hdr[0].name,
												get_client_memptr(eth_client, num_eth_client)->hdr_hdl_v4,
												hdr_len);
				get_client_memptr(eth_client, num_eth_client)->ipv4_header_set=true;

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
				if (sCopyHeader.is_eth2_ofst_valid)
				{
					memcpy(&pHeaderDescriptor->hdr[0].hdr[sCopyHeader.eth2_ofst],
						mac_addr,
						IPA_MAC_ADDR_SIZE);
				}
#ifdef FEATURE_VLAN_MPDN
				/* 802.1Q header (comes after dst and src MAC)
				   --------------------------------------------
				   |    0   |    1   |     2    |    3        |
				   --------------------------------------------
				   |       TPID(2B)  |       TCI(2B)          |
				   --------------------------------------------
				   |                 |   PCP+|  VLAN ID(12b)  |
				   |                 |DEI(4b)|                |
				   --------------------------------------------
				*/
				if(isVlan)
				{
					uint16_t vlan_tci =
						(*((uint16_t *)&(pHeaderDescriptor->hdr[0].hdr[sCopyHeader.eth2_ofst +
							2 * IPA_MAC_ADDR_SIZE +
							VLAN_TPID_SIZE])));
					vlan_tci = (vlan_tci & ~VLAN_VID_MASK) | (vlan_id & VLAN_VID_MASK);
					/* change vlan_tci to HW format */
					vlan_tci = htons(vlan_tci);

					memcpy(&pHeaderDescriptor->hdr[0].hdr[sCopyHeader.eth2_ofst +
						2 * IPA_MAC_ADDR_SIZE +
						VLAN_TPID_SIZE],
						&vlan_tci,
						sizeof(vlan_tci));

					IPACMDBG_H("v6 updated the vlan_tci, now 0x%X, vlan tag is 0x%X\n", vlan_tci,
						*((uint32_t *)&(pHeaderDescriptor->hdr[0].hdr[sCopyHeader.eth2_ofst +
							2 * IPA_MAC_ADDR_SIZE])));
				}
				/* VLAN case */
				if(bridge)
				{
					memcpy(&pHeaderDescriptor->hdr[0].hdr[sCopyHeader.eth2_ofst +
						IPA_MAC_ADDR_SIZE],
						bridge->bridge_mac,
						IPA_MAC_ADDR_SIZE);
					IPACMDBG_H("device is in bridge mode (VLAN), MAC 0x[%X][%X][%X][%X][%X][%X]\n",
						bridge->bridge_mac[0],
						bridge->bridge_mac[1],
						bridge->bridge_mac[2],
						bridge->bridge_mac[3],
						bridge->bridge_mac[4],
						bridge->bridge_mac[5]);
				}
				/* non VLAN case */
				else
#endif
				/* replace src mac to bridge mac_addr if any  */
				if (IPACM_Iface::ipacmcfg->ipa_bridge_enable)
				{
					memcpy(&pHeaderDescriptor->hdr[0].hdr[sCopyHeader.eth2_ofst+IPA_MAC_ADDR_SIZE],
							IPACM_Iface::ipacmcfg->bridge_mac,
							IPA_MAC_ADDR_SIZE);
					IPACMDBG_H("device is in bridge mode (XML), MAC 0x[%X][%X][%X][%X][%X][%X]\n",
						IPACM_Iface::ipacmcfg->bridge_mac[0],
						IPACM_Iface::ipacmcfg->bridge_mac[1],
						IPACM_Iface::ipacmcfg->bridge_mac[2],
						IPACM_Iface::ipacmcfg->bridge_mac[3],
						IPACM_Iface::ipacmcfg->bridge_mac[4],
						IPACM_Iface::ipacmcfg->bridge_mac[5]);
				}

				pHeaderDescriptor->commit = true;
				pHeaderDescriptor->num_hdrs = 1;

				memset(pHeaderDescriptor->hdr[0].name, 0,
					 sizeof(pHeaderDescriptor->hdr[0].name));

				snprintf(index,sizeof(index), "%d", ipa_if_num);
				strlcpy(pHeaderDescriptor->hdr[0].name, index, sizeof(pHeaderDescriptor->hdr[0].name));
				pHeaderDescriptor->hdr[0].name[IPA_RESOURCE_NAME_MAX-1] = '\0';
				if (strlcat(pHeaderDescriptor->hdr[0].name, IPA_ETH_HDR_NAME_v6, sizeof(pHeaderDescriptor->hdr[0].name)) > IPA_RESOURCE_NAME_MAX)
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
				hdr_len = sCopyHeader.hdr_len;
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

				get_client_memptr(eth_client, num_eth_client)->hdr_hdl_v6 = pHeaderDescriptor->hdr[0].hdr_hdl;
				IPACMDBG_H("eth-client(%d) v6 full header name:%s header handle:(0x%x) Len:%d\n",
						 num_eth_client,
						 pHeaderDescriptor->hdr[0].name,
									 get_client_memptr(eth_client, num_eth_client)->hdr_hdl_v6,
									 hdr_len);

									get_client_memptr(eth_client, num_eth_client)->ipv6_header_set=true;

				break;

			}
		}
		/* initialize wifi client*/
		get_client_memptr(eth_client, num_eth_client)->route_rule_set_v4 = false;
		get_client_memptr(eth_client, num_eth_client)->route_rule_set_v6 = 0;
		get_client_memptr(eth_client, num_eth_client)->ipv4_set = false;
		get_client_memptr(eth_client, num_eth_client)->ipv6_set = 0;
#ifdef FEATURE_IPACM_PER_CLIENT_STATS
		IPACMDBG_H ("Is ODU client? %s\n", is_odu?"Yes":"No");
		get_client_memptr(eth_client, num_eth_client)->ipv4_ul_rules_set = false;
		get_client_memptr(eth_client, num_eth_client)->ipv4_ul_rules_set = false;
		get_client_memptr(eth_client, num_eth_client)->lan_stats_idx = get_lan_stats_index(get_client_memptr(eth_client, num_eth_client)->mac);
		memset(get_client_memptr(eth_client, num_eth_client)->wan_ul_fl_rule_hdl_v4, 0, MAX_WAN_UL_FILTER_RULES * sizeof(uint32_t));
		memset(get_client_memptr(eth_client, num_eth_client)->wan_ul_fl_rule_hdl_v6, 0, MAX_WAN_UL_FILTER_RULES * sizeof(uint32_t));
#endif
		clnt_indx = num_eth_client;
		num_eth_client++;
#ifdef FEATURE_IPACM_PER_CLIENT_STATS
		if (IPACM_Iface::ipacmcfg->ipacm_lan_stats_enable == true &&
			get_client_memptr(eth_client, clnt_indx)->lan_stats_idx != -1)
		{
			/* Store the client info at WAN driver. */
			client_info = (struct wan_ioctl_lan_client_info *)malloc(sizeof(struct wan_ioctl_lan_client_info));
			if (client_info == NULL)
			{
				IPACMERR("Unable to allocate memory\n");
				res = IPACM_FAILURE;
				goto fail;
			}
			memset(client_info, 0, sizeof(struct wan_ioctl_lan_client_info));
			if (ipa_if_cate == LAN_IF)
			{
				client_info->device_type = IPACM_CLIENT_DEVICE_TYPE_USB;
			}
			else if (ipa_if_cate == ODU_IF && is_odu == true)
			{
				client_info->device_type = IPACM_CLIENT_DEVICE_TYPE_ODU;
			}
			else if (ipa_if_cate == ODU_IF)
			{
				client_info->device_type = IPACM_CLIENT_DEVICE_TYPE_ETH;
			}
			else
			{
				IPACMERR("Unsupported interface category: %d\n", ipa_if_cate);
				res = IPACM_FAILURE;
				goto fail;
			}
			memcpy(client_info->mac,
					mac_addr,
					IPA_MAC_ADDR_SIZE);
			client_info->client_init = 1;
			client_info->client_idx = get_client_memptr(eth_client, clnt_indx)->lan_stats_idx;
			client_info->ul_src_pipe = (enum ipa_client_type) IPA_CLIENT_MAX;
			client_info->hdr_len = hdr_len;
			if (rx_prop)
			{
				client_info->ul_src_pipe = rx_prop->rx[0].src_pipe;
			}
			if (set_lan_client_info(client_info))
			{
				res = IPACM_FAILURE;
				free(client_info);
				/* Reset the mac from active list. */
				reset_active_lan_stats_index(get_client_memptr(eth_client, clnt_indx)->lan_stats_idx, mac_addr);
				/* Add the mac to inactive list. */
				get_free_inactive_lan_stats_index(mac_addr);
				get_client_memptr(eth_client, clnt_indx)->lan_stats_idx = -1;
				goto fail;
			}
			free(client_info);
			if (IPACM_Wan::isWanUP(ipa_if_num))
			{
				if(IPACM_Wan::backhaul_is_sta_mode == false)
				{
					ext_prop = IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v4);
					install_uplink_filter_rule_per_client(ext_prop, IPA_IP_v4, IPACM_Wan::getXlat_Mux_Id(),
						get_client_memptr(eth_client, clnt_indx)->mac);
					get_client_memptr(eth_client, clnt_indx)->ipv4_ul_rules_set = true;
				}
			}
			if(IPACM_Wan::isWanUP_V6(ipa_if_num))
			{
				if(IPACM_Wan::backhaul_is_sta_mode == false)
				{
					ext_prop = IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v6);
					install_uplink_filter_rule_per_client(ext_prop, IPA_IP_v6, 0,
					 get_client_memptr(eth_client, clnt_indx)->mac);
					get_client_memptr(eth_client, clnt_indx)->ipv6_ul_rules_set = true;
				}
			}
		}
#endif
		header_name_count++; //keep increasing header_name_count
		res = IPACM_SUCCESS;
		IPACMDBG_H("eth client number: %d\n", num_eth_client);
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
int IPACM_Lan::handle_eth_client_ipaddr(ipacm_event_data_all *data)
{
	int clnt_indx;
	int v6_num;
	uint32_t ipv6_link_local_prefix = 0xFE800000;
	uint32_t ipv6_link_local_prefix_mask = 0xFFC00000;
	uint8_t vlan_id = 0;

	IPACMDBG_H("number of eth clients: %d\n", num_eth_client);
	IPACMDBG_H("event MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					 data->mac_addr[0],
					 data->mac_addr[1],
					 data->mac_addr[2],
					 data->mac_addr[3],
					 data->mac_addr[4],
					 data->mac_addr[5]);
#ifdef FEATURE_VLAN_MPDN
	if(is_vlan_event(data->iface_name))
	{
		IPACMDBG_H("handling vlan ETH client ip address for iface %s\n", data->iface_name);
		if(IPACM_Iface::ipacmcfg->get_vlan_id(data->iface_name, &vlan_id))
		{
			IPACMERR("failed getting vlan id for iface %s\n", data->iface_name);
			return IPACM_FAILURE;
		}
	}
#endif

	clnt_indx = get_eth_client_index(data->mac_addr, vlan_id);
	if(clnt_indx == IPACM_INVALID_INDEX)
	{
		IPACMERR("eth client not found/attached \n");
		return IPACM_FAILURE;
	}

	IPACMDBG_H("Ip-type received %d\n", data->iptype);
	if (data->iptype == IPA_IP_v4)
	{
		IPACMDBG_H("ipv4 address: 0x%x\n", data->ipv4_addr);
		if (data->ipv4_addr != 0) /* not 0.0.0.0 */
		{
			/* Special handling for Passthrough IP. */
			if (IPACM_Iface::ipacmcfg->ipacm_ip_passthrough_mode)
			{
				/* if the MAC matches or RNDIS/ECM, then IP should not be private subnet. */
				if (!memcmp(data->mac_addr, IPACM_Iface::ipacmcfg->ipacm_ip_passthrough_mac,
					IPA_MAC_ADDR_SIZE))
				{
					/* check if the ip is in private subnet and ignore. */
					if (IPACM_Iface::ipacmcfg->isPrivateSubnet(data->ipv4_addr))
					{
						IPACMDBG_H("Client is in IP passthrough mode, but got private IP: 0x%x\n", data->ipv4_addr);
						return IPACM_FAILURE;
					}
				}
				/* Check if the IP is not in private subnet and ignore. */
				else if (!IPACM_Iface::ipacmcfg->isPrivateSubnet(data->ipv4_addr))
				{
					IPACMDBG_H("Client is not in IP passthrough mode, but got public IP: 0x%x\n", data->ipv4_addr);
					return IPACM_FAILURE;
				}
			}
			else
			{
				/* Check if the IP is not in private subnet and ignore. */
				if (!IPACM_Iface::ipacmcfg->isPrivateSubnet(data->ipv4_addr))
				{
					IPACMDBG_H("Client is not in IP passthrough mode, but got public IP: 0x%x\n", data->ipv4_addr);
					return IPACM_FAILURE;
				}
			}

			if (get_client_memptr(eth_client, clnt_indx)->ipv4_set == false)
			{
				get_client_memptr(eth_client, clnt_indx)->v4_addr = data->ipv4_addr;
				get_client_memptr(eth_client, clnt_indx)->ipv4_set = true;
			}
			else
			{
			   /* check if client got new IPv4 address*/
			   if(data->ipv4_addr == get_client_memptr(eth_client, clnt_indx)->v4_addr)
			   {
				IPACMDBG_H("Already setup ipv4 addr for client:%d, ipv4 address didn't change\n", clnt_indx);
				 return IPACM_FAILURE;
			   }
			   else
			   {
					IPACMDBG_H("ipv4 addr for client:%d is changed \n", clnt_indx);
					/* delete NAT rules first */
					CtList->HandleNeighIpAddrDelEvt(get_client_memptr(eth_client, clnt_indx)->v4_addr);
					delete_eth_rtrules(clnt_indx,IPA_IP_v4);
					get_client_memptr(eth_client, clnt_indx)->route_rule_set_v4 = false;
					get_client_memptr(eth_client, clnt_indx)->v4_addr = data->ipv4_addr;
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
		if ((data->ipv6_addr[0] != 0) || (data->ipv6_addr[1] != 0) ||
				(data->ipv6_addr[2] != 0) || (data->ipv6_addr[3] || 0)) /* check if all 0 not valid ipv6 address */
		{
			IPACMDBG_H("ipv6 address: 0x%x:%x:%x:%x\n", data->ipv6_addr[0], data->ipv6_addr[1], data->ipv6_addr[2], data->ipv6_addr[3]);
			if( (data->ipv6_addr[0] & ipv6_link_local_prefix_mask) != (ipv6_link_local_prefix & ipv6_link_local_prefix_mask) &&
#ifdef FEATURE_VLAN_MPDN
				/* returns true if a VLAN PDN or default PDN should be offloaded */
				IPACM_Iface::ipacmcfg->is_offload_ipv6_prefix(data->ipv6_addr) != true)
#else
				memcmp(ipv6_prefix, data->ipv6_addr, sizeof(ipv6_prefix)) != 0)
#endif
			{
				IPACMDBG_H("This global IPv6 address is not with correct prefix, ignore.\n");
				return IPACM_FAILURE;
			}

			if(get_client_memptr(eth_client, clnt_indx)->ipv6_set < IPV6_NUM_ADDR)
			{
				for(v6_num=0;v6_num < get_client_memptr(eth_client, clnt_indx)->ipv6_set;v6_num++)
				{
					if( data->ipv6_addr[0] == get_client_memptr(eth_client, clnt_indx)->v6_addr[v6_num][0] &&
					data->ipv6_addr[1] == get_client_memptr(eth_client, clnt_indx)->v6_addr[v6_num][1] &&
					data->ipv6_addr[2]== get_client_memptr(eth_client, clnt_indx)->v6_addr[v6_num][2] &&
					data->ipv6_addr[3] == get_client_memptr(eth_client, clnt_indx)->v6_addr[v6_num][3])
					{
						IPACMDBG_H("Already see this ipv6 addr at position: %d for client:%d\n", v6_num, clnt_indx);
						return IPACM_FAILURE; /* not setup the RT rules*/
					}
				}

				/*
				 * The client got new IPv6 address.
				 * NOTE: The new address doesn't replace the existing one but being added (up to IPV6_NUM_ADDR),
				 *       so the previous IPv6 addresses of the client will not be deleted.
				 */
				get_client_memptr(eth_client, clnt_indx)->v6_addr[get_client_memptr(eth_client, clnt_indx)->ipv6_set][0] = data->ipv6_addr[0];
				get_client_memptr(eth_client, clnt_indx)->v6_addr[get_client_memptr(eth_client, clnt_indx)->ipv6_set][1] = data->ipv6_addr[1];
				get_client_memptr(eth_client, clnt_indx)->v6_addr[get_client_memptr(eth_client, clnt_indx)->ipv6_set][2] = data->ipv6_addr[2];
				get_client_memptr(eth_client, clnt_indx)->v6_addr[get_client_memptr(eth_client, clnt_indx)->ipv6_set][3] = data->ipv6_addr[3];
				get_client_memptr(eth_client, clnt_indx)->ipv6_set++;
			}
			else
			{
				IPACMDBG_H("Already got %d ipv6 addr for client:%d\n", IPV6_NUM_ADDR, clnt_indx);
					return IPACM_FAILURE; /* not setup the RT rules*/
			}
		}
	}

	return IPACM_SUCCESS;
}

/*handle eth client routing rule*/
int IPACM_Lan::handle_eth_client_route_rule(uint8_t *mac_addr, ipa_ip_type iptype, uint8_t vlan_id)
{
	struct ipa_ioc_add_rt_rule *rt_rule;
	struct ipa_rt_rule_add *rt_rule_entry;
	uint32_t tx_index;
	int eth_index,v6_num;
	const int NUM = 1;

	if(tx_prop == NULL)
	{
		IPACMDBG_H("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	IPACMDBG_H("Received mac_addr MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					 mac_addr[0], mac_addr[1], mac_addr[2],
					 mac_addr[3], mac_addr[4], mac_addr[5]);

	eth_index = get_eth_client_index(mac_addr, vlan_id);
	if (eth_index == IPACM_INVALID_INDEX)
	{
		IPACMDBG_H("eth client not found/attached \n");
		return IPACM_SUCCESS;
	}

	if (iptype==IPA_IP_v4) {
		IPACMDBG_H("eth client index: %d, ip-type: %d, ipv4_set:%d, ipv4_rule_set:%d \n", eth_index, iptype,
					 get_client_memptr(eth_client, eth_index)->ipv4_set,
					 get_client_memptr(eth_client, eth_index)->route_rule_set_v4);
	} else {
		IPACMDBG_H("eth client index: %d, ip-type: %d, ipv6_set:%d, ipv6_rule_num:%d \n", eth_index, iptype,
					 get_client_memptr(eth_client, eth_index)->ipv6_set,
					 get_client_memptr(eth_client, eth_index)->route_rule_set_v6);
	}
	/* Add default routing rules if not set yet */
	if ((iptype == IPA_IP_v4
			 && get_client_memptr(eth_client, eth_index)->route_rule_set_v4 == false
			 && get_client_memptr(eth_client, eth_index)->ipv4_set == true)
			|| (iptype == IPA_IP_v6
		            && get_client_memptr(eth_client, eth_index)->route_rule_set_v6 < get_client_memptr(eth_client, eth_index)->ipv6_set
					))
	{
		if(IPACM_Iface::ipacmcfg->GetIPAVer() >= IPA_HW_None && IPACM_Iface::ipacmcfg->GetIPAVer() < IPA_HW_v4_0)
		{
			/* Add corresponding ipa_rm_resource_name of TX-endpoint up before IPV6 RT-rule set */
			IPACMDBG_H("dev %s add producer dependency\n", dev_name);
			if (tx_prop != NULL)
			{
				IPACMDBG_H("depend Got pipe %d rm index : %d \n", tx_prop->tx[0].dst_pipe, IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
				IPACM_Iface::ipacmcfg->AddRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe],false);
			}
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
			if (iptype != tx_prop->tx[tx_index].ip)
			{
				IPACMDBG_H("Tx:%d, ip-type: %d conflict ip-type: %d no RT-rule added\n",
					tx_index, tx_prop->tx[tx_index].ip, iptype);
				continue;
			}

			rt_rule_entry = &rt_rule->rules[0];
			rt_rule_entry->at_rear = 0;

			if (iptype == IPA_IP_v4)
			{
				IPACMDBG_H("client index(%d):ipv4 address: 0x%x\n", eth_index,
					get_client_memptr(eth_client, eth_index)->v4_addr);

				IPACMDBG_H("client(%d): v4 header handle:(0x%x)\n",
					eth_index,
					get_client_memptr(eth_client, eth_index)->hdr_hdl_v4);
				strlcpy(rt_rule->rt_tbl_name,
					IPACM_Iface::ipacmcfg->rt_tbl_lan_v4.name,
					sizeof(rt_rule->rt_tbl_name));
				rt_rule->rt_tbl_name[IPA_RESOURCE_NAME_MAX - 1] = '\0';
				rt_rule_entry->rule.dst = tx_prop->tx[tx_index].dst_pipe;
				memcpy(&rt_rule_entry->rule.attrib,
					&tx_prop->tx[tx_index].attrib,
					sizeof(rt_rule_entry->rule.attrib));
				rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
				rt_rule_entry->rule.hdr_hdl = get_client_memptr(eth_client, eth_index)->hdr_hdl_v4;
				rt_rule_entry->rule.attrib.u.v4.dst_addr = get_client_memptr(eth_client, eth_index)->v4_addr;
				rt_rule_entry->rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;

				if (IPACM_Iface::ipacmcfg->GetIPAVer() >= IPA_HW_v4_0)
				{
					rt_rule_entry->rule.hashable = true;
				}

				if (false == m_routing.AddRoutingRule(rt_rule))
				{
					IPACMERR("Routing rule addition failed!\n");
					free(rt_rule);
					return IPACM_FAILURE;
				}

				/* copy ipv4 RT hdl */
				get_client_memptr(eth_client, eth_index)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v4 =
					rt_rule->rules[0].rt_rule_hdl;
				IPACMDBG_H("tx:%d, rt rule hdl=%x ip-type: %d\n", tx_index,
					get_client_memptr(eth_client, eth_index)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v4, iptype);
			}
			else
			{
				for (v6_num = get_client_memptr(eth_client, eth_index)->route_rule_set_v6;
					 v6_num < get_client_memptr(eth_client, eth_index)->ipv6_set;
					 ++v6_num)
				{
					IPACMDBG_H("client(%d): v6 header handle:(0x%x)\n",
						eth_index,
						get_client_memptr(eth_client, eth_index)->hdr_hdl_v6);

					/* v6 LAN_RT_TBL */
					strlcpy(rt_rule->rt_tbl_name,
						IPACM_Iface::ipacmcfg->rt_tbl_v6.name,
						sizeof(rt_rule->rt_tbl_name));
					rt_rule->rt_tbl_name[IPA_RESOURCE_NAME_MAX - 1] = '\0';
					/* Support QCMAP LAN traffic feature, send to A5 */
					rt_rule_entry->rule.dst = IPA_CLIENT_APPS_LAN_CONS;
					memset(&rt_rule_entry->rule.attrib, 0, sizeof(rt_rule_entry->rule.attrib));
					rt_rule_entry->rule.hdr_hdl = 0;
					rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
					rt_rule_entry->rule.attrib.u.v6.dst_addr[0] =
						get_client_memptr(eth_client, eth_index)->v6_addr[v6_num][0];
					rt_rule_entry->rule.attrib.u.v6.dst_addr[1] =
						get_client_memptr(eth_client, eth_index)->v6_addr[v6_num][1];
					rt_rule_entry->rule.attrib.u.v6.dst_addr[2] =
						get_client_memptr(eth_client, eth_index)->v6_addr[v6_num][2];
					rt_rule_entry->rule.attrib.u.v6.dst_addr[3] =
						get_client_memptr(eth_client, eth_index)->v6_addr[v6_num][3];
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

					get_client_memptr(eth_client, eth_index)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v6[v6_num] = rt_rule->rules[0].rt_rule_hdl;
					IPACMDBG_H("tx:%d, rt rule hdl=%x ip-type: %d\n", tx_index,
						get_client_memptr(eth_client, eth_index)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v6[v6_num], iptype);

					/*Copy same rule to v6 WAN RT TBL*/
					strlcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_wan_v6.name, sizeof(rt_rule->rt_tbl_name));
					rt_rule->rt_tbl_name[IPA_RESOURCE_NAME_MAX - 1] = '\0';
					/* Downlink traffic from Wan iface, directly through IPA */
					rt_rule_entry->rule.dst = tx_prop->tx[tx_index].dst_pipe;
					memcpy(&rt_rule_entry->rule.attrib,
						&tx_prop->tx[tx_index].attrib,
						sizeof(rt_rule_entry->rule.attrib));
					rt_rule_entry->rule.hdr_hdl = get_client_memptr(eth_client, eth_index)->hdr_hdl_v6;
					rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
					rt_rule_entry->rule.attrib.u.v6.dst_addr[0] = get_client_memptr(eth_client, eth_index)->v6_addr[v6_num][0];
					rt_rule_entry->rule.attrib.u.v6.dst_addr[1] = get_client_memptr(eth_client, eth_index)->v6_addr[v6_num][1];
					rt_rule_entry->rule.attrib.u.v6.dst_addr[2] = get_client_memptr(eth_client, eth_index)->v6_addr[v6_num][2];
					rt_rule_entry->rule.attrib.u.v6.dst_addr[3] = get_client_memptr(eth_client, eth_index)->v6_addr[v6_num][3];
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

					get_client_memptr(eth_client, eth_index)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v6_wan[v6_num] = rt_rule->rules[0].rt_rule_hdl;
					IPACMDBG_H("tx:%d, rt rule hdl=%x ip-type: %d\n", tx_index,
						get_client_memptr(eth_client, eth_index)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v6_wan[v6_num], iptype);
				}
			}

		} /* end of for loop */

		free(rt_rule);

		if (iptype == IPA_IP_v4)
		{
			get_client_memptr(eth_client, eth_index)->route_rule_set_v4 = true;
		}
		else
		{
			get_client_memptr(eth_client, eth_index)->route_rule_set_v6 = get_client_memptr(eth_client, eth_index)->ipv6_set;
		}
	}
	return IPACM_SUCCESS;
}

#ifdef FEATURE_IPACM_PER_CLIENT_STATS
int IPACM_Lan::handle_lan_client_connect(uint8_t *mac_addr)
{
	int eth_index, res = IPACM_SUCCESS;
	ipacm_ext_prop* ext_prop;
	struct wan_ioctl_lan_client_info *client_info;

	IPACMDBG_H("Received mac_addr MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
			mac_addr[0], mac_addr[1], mac_addr[2],
			mac_addr[3], mac_addr[4], mac_addr[5]);

	eth_index = get_eth_client_index(mac_addr);
	if (eth_index == IPACM_INVALID_INDEX)
	{
		IPACMDBG_H("wlan client not found/attached \n");
		return IPACM_SUCCESS;
	}

	if (get_client_memptr(eth_client, eth_index)->lan_stats_idx != -1)
	{
		IPACMDBG_H("wlan client already has lan_stats index. \n");
		return IPACM_FAILURE;
	}

	IPACMDBG_H ("Is ODU client? %s\n", is_odu?"Yes":"No");
	get_client_memptr(eth_client, eth_index)->lan_stats_idx = get_lan_stats_index(mac_addr);

	if (get_client_memptr(eth_client, eth_index)->lan_stats_idx == -1)
	{
		IPACMDBG_H("No active index..abort \n");
		return IPACM_FAILURE;
	}

	if (IPACM_Iface::ipacmcfg->ipacm_lan_stats_enable == true)
	{
		client_info = (struct wan_ioctl_lan_client_info *)malloc(sizeof(struct wan_ioctl_lan_client_info));
		if (client_info == NULL)
		{
			IPACMERR("Unable to allocate memory\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		memset(client_info, 0, sizeof(struct wan_ioctl_lan_client_info));
		if (ipa_if_cate == LAN_IF)
		{
			client_info->device_type = IPACM_CLIENT_DEVICE_TYPE_USB;
		}
		else if (ipa_if_cate == ODU_IF && is_odu == true)
		{
			client_info->device_type = IPACM_CLIENT_DEVICE_TYPE_ODU;
		}
		else if (ipa_if_cate == ODU_IF)
		{
			client_info->device_type = IPACM_CLIENT_DEVICE_TYPE_ETH;
		}
		else
		{
			IPACMERR("Unsupported interface category: %d\n", ipa_if_cate);
			res = IPACM_FAILURE;
			goto fail;
		}
		memcpy(client_info->mac,
				get_client_memptr(eth_client, eth_index)->mac,
				IPA_MAC_ADDR_SIZE);
		client_info->client_init = 1;
		client_info->client_idx = get_client_memptr(eth_client, eth_index)->lan_stats_idx;
		client_info->ul_src_pipe = (enum ipa_client_type) IPA_CLIENT_MAX;
		client_info->hdr_len = hdr_len;
		if (rx_prop)
		{
			client_info->ul_src_pipe = rx_prop->rx[0].src_pipe;
		}
		if (set_lan_client_info(client_info))
		{
			res = IPACM_FAILURE;
			free(client_info);
			goto fail;
		}
		free(client_info);
		if (IPACM_Wan::isWanUP(ipa_if_num))
		{
			if(IPACM_Wan::backhaul_is_sta_mode == false)
			{
				ext_prop = IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v4);
				install_uplink_filter_rule_per_client(ext_prop, IPA_IP_v4, IPACM_Wan::getXlat_Mux_Id(), get_client_memptr(eth_client, eth_index)->mac);
				get_client_memptr(eth_client, eth_index)->ipv4_ul_rules_set = true;
			}
		}
		if(IPACM_Wan::isWanUP_V6(ipa_if_num))
		{
			if(IPACM_Wan::backhaul_is_sta_mode == false)
			{
				ext_prop = IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v6);
				install_uplink_filter_rule_per_client(ext_prop, IPA_IP_v6, 0, get_client_memptr(eth_client, eth_index)->mac);
				get_client_memptr(eth_client, eth_index)->ipv6_ul_rules_set = true;
			}
		}
		handle_eth_client_route_rule_ext(get_client_memptr(eth_client, eth_index)->mac, IPA_IP_v4);
		handle_eth_client_route_rule_ext(get_client_memptr(eth_client, eth_index)->mac, IPA_IP_v6);
	}
	return IPACM_SUCCESS;
fail:
	/* Reset the mac from active list. */
	reset_active_lan_stats_index(get_client_memptr(eth_client, eth_index)->lan_stats_idx, mac_addr);
	/* Add the mac to inactive list. */
	get_free_inactive_lan_stats_index(mac_addr);
	get_client_memptr(eth_client, eth_index)->lan_stats_idx = -1;
	return IPACM_FAILURE;
}

int IPACM_Lan::handle_lan_client_disconnect(uint8_t *mac_addr)
{
	int i;
	uint8_t mac[IPA_MAC_ADDR_SIZE];

	IPACMDBG_H ("Is ODU client? %s\n", is_odu?"Yes":"No");

	/* Check if the client is in active list and remove it. */
	if (reset_active_lan_stats_index(get_lan_stats_index(mac_addr), mac_addr) == -1)
	{
		IPACMDBG_H("Failed to reset active lan_stats index, try inactive list. \n");
		/* If it is not in active list, check inactive list and remove it. */
		if (reset_inactive_lan_stats_index(mac_addr) == -1)
		{
			IPACMDBG_H("Failed to reserve inactive lan_stats index, return\n");
		}
		return IPACM_SUCCESS;
	}
	/* As we have free lan stats index. */
	/* Go through the inactive list and pick the first available one to add it to active list. */
	if (get_available_inactive_lan_client(mac) == IPACM_FAILURE)
	{
		IPACMDBG_H("Error in getting in active client.\n");
		return IPACM_FAILURE;
	}

	/* Add the mac to the active list. */
	if (get_free_active_lan_stats_index(mac) == -1)
	{
		IPACMDBG_H("Free active index not available. Abort\n");
		return IPACM_FAILURE;
	}

	/* Remove the mac from inactive list. */
	if (reset_inactive_lan_stats_index(mac) == IPACM_FAILURE)
	{
		IPACMDBG_H("Unable to remove the client from inactive list. Check\n");
	}

	/* Process the new lan stats index. */
	return handle_lan_client_connect(mac);
}


/*handle eth client routing rule with rule id*/
int IPACM_Lan::handle_eth_client_route_rule_ext(uint8_t *mac_addr, ipa_ip_type iptype)
{
	struct ipa_ioc_add_rt_rule_ext *rt_rule;
	struct ipa_rt_rule_add_ext *rt_rule_entry;
	uint32_t tx_index;
	int eth_index,v6_num;
	const int NUM = 1;
	ipacm_event_data_all data;

	if(tx_prop == NULL)
	{
		IPACMDBG_H("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	IPACMDBG_H("Received mac_addr MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					 mac_addr[0], mac_addr[1], mac_addr[2],
					 mac_addr[3], mac_addr[4], mac_addr[5]);

	eth_index = get_eth_client_index(mac_addr);
	if (eth_index == IPACM_INVALID_INDEX)
	{
		IPACMDBG_H("eth client not found/attached \n");
		return IPACM_SUCCESS;
	}

	if (get_client_memptr(eth_client, eth_index)->lan_stats_idx == -1)
	{
		IPACMDBG_H("Lan client index not attached. \n");
		return IPACM_SUCCESS;
	}

	if (iptype==IPA_IP_v4) {
		IPACMDBG_H("eth client index: %d, ip-type: %d, ipv4_set:%d, ipv4_rule_set:%d \n", eth_index, iptype,
					 get_client_memptr(eth_client, eth_index)->ipv4_set,
					 get_client_memptr(eth_client, eth_index)->route_rule_set_v4);
	} else {
		IPACMDBG_H("eth client index: %d, ip-type: %d, ipv6_set:%d, ipv6_rule_num:%d \n", eth_index, iptype,
					 get_client_memptr(eth_client, eth_index)->ipv6_set,
					 get_client_memptr(eth_client, eth_index)->route_rule_set_v6);
	}
	/* Add default routing rules if not set yet */
	if ((iptype == IPA_IP_v4
			 && get_client_memptr(eth_client, eth_index)->route_rule_set_v4 == false
			 && get_client_memptr(eth_client, eth_index)->ipv4_set == true)
			|| (iptype == IPA_IP_v6
		            && get_client_memptr(eth_client, eth_index)->route_rule_set_v6 < get_client_memptr(eth_client, eth_index)->ipv6_set
					))
	{
		if(IPACM_Iface::ipacmcfg->GetIPAVer() >= IPA_HW_None && IPACM_Iface::ipacmcfg->GetIPAVer() < IPA_HW_v4_0)
		{
			/* Add corresponding ipa_rm_resource_name of TX-endpoint up before IPV6 RT-rule set */
			IPACMDBG_H("dev %s add producer dependency\n", dev_name);
			if (tx_prop != NULL)
			{
				IPACMDBG_H("depend Got pipe %d rm index : %d \n", tx_prop->tx[0].dst_pipe, IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
				IPACM_Iface::ipacmcfg->AddRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe],false);
			}
		}
		rt_rule = (struct ipa_ioc_add_rt_rule_ext *)
			 calloc(1, sizeof(struct ipa_ioc_add_rt_rule_ext) +
						NUM * sizeof(struct ipa_rt_rule_add_ext));

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
				IPACMDBG_H("client index(%d):ipv4 address: 0x%x\n", eth_index,
					get_client_memptr(eth_client, eth_index)->v4_addr);

				IPACMDBG_H("client(%d): v4 header handle:(0x%x)\n",
						 eth_index,
						 get_client_memptr(eth_client, eth_index)->hdr_hdl_v4);
				strlcpy(rt_rule->rt_tbl_name,
								IPACM_Iface::ipacmcfg->rt_tbl_lan_v4.name,
								sizeof(rt_rule->rt_tbl_name));
				rt_rule->rt_tbl_name[IPA_RESOURCE_NAME_MAX-1] = '\0';
				rt_rule_entry->rule.dst = tx_prop->tx[tx_index].dst_pipe;
				memcpy(&rt_rule_entry->rule.attrib,
						 &tx_prop->tx[tx_index].attrib,
						 sizeof(rt_rule_entry->rule.attrib));
				rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
				rt_rule_entry->rule.hdr_hdl = get_client_memptr(eth_client, eth_index)->hdr_hdl_v4;
				rt_rule_entry->rule.attrib.u.v4.dst_addr = get_client_memptr(eth_client, eth_index)->v4_addr;
				rt_rule_entry->rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;

				if(IPACM_Iface::ipacmcfg->GetIPAVer() >= IPA_HW_v4_0)
				{
					rt_rule_entry->rule.hashable = true;
				}

				rt_rule_entry->rule_id = 0;
				rt_rule_entry->rule_id = (get_client_memptr(eth_client, eth_index)->lan_stats_idx) | 0x200;
			    if (false == m_routing.AddRoutingRuleExt(rt_rule))
				{
					IPACMERR("Routing rule addition failed!\n");
					free(rt_rule);
					return IPACM_FAILURE;
			    }

			    /* copy ipv4 RT hdl */
		        get_client_memptr(eth_client, eth_index)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v4 =
				rt_rule->rules[0].rt_rule_hdl;
		        IPACMDBG_H("tx:%d, rt rule id=%x ip-type: %d\n", tx_index,
				rt_rule_entry->rule_id, iptype);

				get_client_memptr(eth_client, eth_index)->route_rule_set_v4 = true;
				/* Add NAT rules after ipv4 RT rules are set */
				memset(&data, 0, sizeof(data));
				data.if_index = IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].netlink_interface_index;
				data.iptype = IPA_IP_v4;
				data.ipv4_addr = get_client_memptr(eth_client, eth_index)->v4_addr;
				HandleNeighIpAddrAddEvt(&data);
			} else {

		        for(v6_num = get_client_memptr(eth_client, eth_index)->route_rule_set_v6;v6_num < get_client_memptr(eth_client, eth_index)->ipv6_set;v6_num++)
			    {
                    IPACMDBG_H("client(%d): v6 header handle:(0x%x)\n",
		  	    			 eth_index,
		  	    			 get_client_memptr(eth_client, eth_index)->hdr_hdl_v6);

		            /* v6 LAN_RT_TBL */
				strlcpy(rt_rule->rt_tbl_name,
			    					IPACM_Iface::ipacmcfg->rt_tbl_v6.name,
			    					sizeof(rt_rule->rt_tbl_name));
				rt_rule->rt_tbl_name[IPA_RESOURCE_NAME_MAX-1] = '\0';
		            /* Support QCMAP LAN traffic feature, send to A5 */
					rt_rule_entry->rule.dst = IPA_CLIENT_APPS_LAN_CONS;
			        memset(&rt_rule_entry->rule.attrib, 0, sizeof(rt_rule_entry->rule.attrib));
		   	        rt_rule_entry->rule.hdr_hdl = 0;
			        rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		   	        rt_rule_entry->rule.attrib.u.v6.dst_addr[0] = get_client_memptr(eth_client, eth_index)->v6_addr[v6_num][0];
		   	        rt_rule_entry->rule.attrib.u.v6.dst_addr[1] = get_client_memptr(eth_client, eth_index)->v6_addr[v6_num][1];
		   	        rt_rule_entry->rule.attrib.u.v6.dst_addr[2] = get_client_memptr(eth_client, eth_index)->v6_addr[v6_num][2];
		   	        rt_rule_entry->rule.attrib.u.v6.dst_addr[3] = get_client_memptr(eth_client, eth_index)->v6_addr[v6_num][3];
					rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[0] = 0xFFFFFFFF;
					rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[1] = 0xFFFFFFFF;
					rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[2] = 0xFFFFFFFF;
					rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[3] = 0xFFFFFFFF;
#ifdef FEATURE_IPA_V3
					rt_rule_entry->rule.hashable = true;
#endif
					rt_rule_entry->rule_id = 0;
					if (false == m_routing.AddRoutingRuleExt(rt_rule))
					{
						IPACMERR("Routing rule addition failed!\n");
						free(rt_rule);
						return IPACM_FAILURE;
					}

		            get_client_memptr(eth_client, eth_index)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v6[v6_num] = rt_rule->rules[0].rt_rule_hdl;
		            IPACMDBG_H("tx:%d, rt rule id=%x ip-type: %d\n", tx_index,
		            				 rt_rule_entry->rule_id, iptype);
			        /*Copy same rule to v6 WAN RT TBL*/
				strlcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_wan_v6.name, sizeof(rt_rule->rt_tbl_name));
				rt_rule->rt_tbl_name[IPA_RESOURCE_NAME_MAX-1] = '\0';
				/* Downlink traffic from Wan iface, directly through IPA */
					rt_rule_entry->rule.dst = tx_prop->tx[tx_index].dst_pipe;
			        memcpy(&rt_rule_entry->rule.attrib,
						 &tx_prop->tx[tx_index].attrib,
						 sizeof(rt_rule_entry->rule.attrib));
		   	        rt_rule_entry->rule.hdr_hdl = get_client_memptr(eth_client, eth_index)->hdr_hdl_v6;
			        rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		   	        rt_rule_entry->rule.attrib.u.v6.dst_addr[0] = get_client_memptr(eth_client, eth_index)->v6_addr[v6_num][0];
		   	        rt_rule_entry->rule.attrib.u.v6.dst_addr[1] = get_client_memptr(eth_client, eth_index)->v6_addr[v6_num][1];
		   	        rt_rule_entry->rule.attrib.u.v6.dst_addr[2] = get_client_memptr(eth_client, eth_index)->v6_addr[v6_num][2];
		   	        rt_rule_entry->rule.attrib.u.v6.dst_addr[3] = get_client_memptr(eth_client, eth_index)->v6_addr[v6_num][3];
					rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[0] = 0xFFFFFFFF;
					rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[1] = 0xFFFFFFFF;
					rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[2] = 0xFFFFFFFF;
					rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[3] = 0xFFFFFFFF;
#ifdef FEATURE_IPA_V3
					rt_rule_entry->rule.hashable = true;
#endif
					rt_rule_entry->rule_id = get_client_memptr(eth_client, eth_index)->lan_stats_idx | 0x200;
		            if (false == m_routing.AddRoutingRuleExt(rt_rule))
		            {
							IPACMERR("Routing rule addition failed!\n");
							free(rt_rule);
							return IPACM_FAILURE;
		            }
		            get_client_memptr(eth_client, eth_index)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v6_wan[v6_num] = rt_rule->rules[0].rt_rule_hdl;
					IPACMDBG_H("tx:%d, rt rule id=%x ip-type: %d\n", tx_index,
		            				 rt_rule_entry->rule_id, iptype);

					/* Add IPv6CT rules after ipv6 RT rules are set */
					memset(&data, 0, sizeof(data));
					data.if_index = IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].netlink_interface_index;
					data.iptype = IPA_IP_v6;
					memcpy(data.ipv6_addr, get_client_memptr(eth_client, eth_index)->v6_addr[v6_num], sizeof(data.ipv6_addr));
					HandleNeighIpAddrAddEvt(&data);
				}
				get_client_memptr(eth_client, eth_index)->route_rule_set_v6 = get_client_memptr(eth_client, eth_index)->ipv6_set;
			} /* end of for loop */
			free(rt_rule);
		}
	}
	return IPACM_SUCCESS;
}
#endif

/* handle odu client initial, construct full headers (tx property) */
int IPACM_Lan::handle_odu_hdr_init(uint8_t *mac_addr)
{
	int res = IPACM_SUCCESS, len = 0;
	struct ipa_ioc_copy_hdr sCopyHeader;
	struct ipa_ioc_add_hdr *pHeaderDescriptor = NULL;
	uint32_t cnt;

	IPACMDBG("Received Client MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					 mac_addr[0], mac_addr[1], mac_addr[2],
					 mac_addr[3], mac_addr[4], mac_addr[5]);

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
								IPACMDBG("Got partial v4-header name from %d tx props\n", cnt);
								memset(&sCopyHeader, 0, sizeof(sCopyHeader));
								memcpy(sCopyHeader.name,
											tx_prop->tx[cnt].hdr_name,
											 sizeof(sCopyHeader.name));
								IPACMDBG("header name: %s in tx:%d\n", sCopyHeader.name,cnt);
								if (m_header.CopyHeader(&sCopyHeader) == false)
								{
									PERROR("ioctl copy header failed");
									res = IPACM_FAILURE;
									goto fail;
								}
								IPACMDBG("header length: %d, paritial: %d\n", sCopyHeader.hdr_len, sCopyHeader.is_partial);
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
								if (sCopyHeader.is_eth2_ofst_valid)
								{
									memcpy(&pHeaderDescriptor->hdr[0].hdr[sCopyHeader.eth2_ofst],
											 mac_addr,
											 IPA_MAC_ADDR_SIZE);
								}
								/* replace src mac to bridge mac_addr if any  */
								if (IPACM_Iface::ipacmcfg->ipa_bridge_enable)
								{
									memcpy(&pHeaderDescriptor->hdr[0].hdr[sCopyHeader.eth2_ofst+IPA_MAC_ADDR_SIZE],
											IPACM_Iface::ipacmcfg->bridge_mac,
											IPA_MAC_ADDR_SIZE);
									IPACMDBG_H("device is in bridge mode \n");
								}

								pHeaderDescriptor->commit = true;
								pHeaderDescriptor->num_hdrs = 1;

								memset(pHeaderDescriptor->hdr[0].name, 0,
											 sizeof(pHeaderDescriptor->hdr[0].name));
								strlcpy(pHeaderDescriptor->hdr[0].name, IPA_ODU_HDR_NAME_v4, sizeof(pHeaderDescriptor->hdr[0].name));
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

					ODU_hdr_hdl_v4 = pHeaderDescriptor->hdr[0].hdr_hdl;
					ipv4_header_set = true ;
					IPACMDBG(" ODU v4 full header name:%s header handle:(0x%x)\n",
										 pHeaderDescriptor->hdr[0].name,
												 ODU_hdr_hdl_v4);
					break;
				 }
		}


		/* copy partial header for v6*/
		for (cnt=0; cnt<tx_prop->num_tx_props; cnt++)
		{
			if(tx_prop->tx[cnt].ip==IPA_IP_v6)
			{

				IPACMDBG("Got partial v6-header name from %d tx props\n", cnt);
				memset(&sCopyHeader, 0, sizeof(sCopyHeader));
				memcpy(sCopyHeader.name,
						tx_prop->tx[cnt].hdr_name,
							sizeof(sCopyHeader.name));

				IPACMDBG("header name: %s in tx:%d\n", sCopyHeader.name,cnt);
				if (m_header.CopyHeader(&sCopyHeader) == false)
				{
					PERROR("ioctl copy header failed");
					res = IPACM_FAILURE;
					goto fail;
				}

				IPACMDBG("header length: %d, paritial: %d\n", sCopyHeader.hdr_len, sCopyHeader.is_partial);
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
				if (sCopyHeader.is_eth2_ofst_valid)
				{
					memcpy(&pHeaderDescriptor->hdr[0].hdr[sCopyHeader.eth2_ofst],
					 mac_addr,
					 IPA_MAC_ADDR_SIZE);
				}
				/* replace src mac to bridge mac_addr if any  */
				if (IPACM_Iface::ipacmcfg->ipa_bridge_enable)
				{
					memcpy(&pHeaderDescriptor->hdr[0].hdr[sCopyHeader.eth2_ofst+IPA_MAC_ADDR_SIZE],
							IPACM_Iface::ipacmcfg->bridge_mac,
							IPA_MAC_ADDR_SIZE);
					IPACMDBG_H("device is in bridge mode \n");
				}

				pHeaderDescriptor->commit = true;
				pHeaderDescriptor->num_hdrs = 1;

				memset(pHeaderDescriptor->hdr[0].name, 0,
					 sizeof(pHeaderDescriptor->hdr[0].name));

				strlcpy(pHeaderDescriptor->hdr[0].name, IPA_ODU_HDR_NAME_v6, sizeof(pHeaderDescriptor->hdr[0].name));
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
				ODU_hdr_hdl_v6 = pHeaderDescriptor->hdr[0].hdr_hdl;
				ipv6_header_set = true ;
				IPACMDBG(" ODU v4 full header name:%s header handle:(0x%x)\n",
									 pHeaderDescriptor->hdr[0].name,
											 ODU_hdr_hdl_v6);
				break;
			}
		}
	}
fail:
	free(pHeaderDescriptor);
	return res;
}


/* handle odu default route rule configuration */
int IPACM_Lan::handle_odu_route_add()
{
	/* add default WAN route */
	struct ipa_ioc_add_rt_rule *rt_rule;
	struct ipa_rt_rule_add *rt_rule_entry;
	uint32_t tx_index;
	const int NUM = 1;

	if(tx_prop == NULL)
	{
	  IPACMDBG_H("No tx properties, ignore default route setting\n");
	  return IPACM_SUCCESS;
	}

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


	IPACMDBG_H("WAN table created %s \n", rt_rule->rt_tbl_name);
	rt_rule_entry = &rt_rule->rules[0];
	rt_rule_entry->at_rear = true;

	for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
	{

		if (IPA_IP_v4 == tx_prop->tx[tx_index].ip)
		{
			strlcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_odu_v4.name, sizeof(rt_rule->rt_tbl_name));
			rt_rule_entry->rule.hdr_hdl = ODU_hdr_hdl_v4;
			rt_rule->ip = IPA_IP_v4;
		}
		else
		{
			strlcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_odu_v6.name, sizeof(rt_rule->rt_tbl_name));
			rt_rule_entry->rule.hdr_hdl = ODU_hdr_hdl_v6;
			rt_rule->ip = IPA_IP_v6;
		}

		rt_rule_entry->rule.dst = tx_prop->tx[tx_index].dst_pipe;
		memcpy(&rt_rule_entry->rule.attrib,
					 &tx_prop->tx[tx_index].attrib,
					 sizeof(rt_rule_entry->rule.attrib));

		rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		if (IPA_IP_v4 == tx_prop->tx[tx_index].ip)
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
			odu_route_rule_v4_hdl[tx_index] = rt_rule_entry->rt_rule_hdl;
			IPACMDBG_H("Got ipv4 ODU-route rule hdl:0x%x,tx:%d,ip-type: %d \n",
						 odu_route_rule_v4_hdl[tx_index],
						 tx_index,
						 IPA_IP_v4);
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
			odu_route_rule_v6_hdl[tx_index] = rt_rule_entry->rt_rule_hdl;
			IPACMDBG_H("Set ipv6 ODU-route rule hdl for v6_lan_table:0x%x,tx:%d,ip-type: %d \n",
					odu_route_rule_v6_hdl[tx_index],
					tx_index,
					IPA_IP_v6);
		}
	}
	free(rt_rule);
	return IPACM_SUCCESS;
}

/* handle odu default route rule deletion */
int IPACM_Lan::handle_odu_route_del()
{
	uint32_t tx_index;

	if(tx_prop == NULL)
	{
		IPACMDBG_H("No tx properties, ignore delete default route setting\n");
		return IPACM_SUCCESS;
	}

	for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
	{
		if (tx_prop->tx[tx_index].ip == IPA_IP_v4)
		{
			IPACMDBG_H("Tx:%d, ip-type: %d match ip-type: %d, RT-rule deleted\n",
					tx_index, tx_prop->tx[tx_index].ip,IPA_IP_v4);

			if (m_routing.DeleteRoutingHdl(odu_route_rule_v4_hdl[tx_index], IPA_IP_v4)
					== false)
			{
				IPACMERR("IP-family:%d, Routing rule(hdl:0x%x) deletion failed with tx_index %d!\n", IPA_IP_v4, odu_route_rule_v4_hdl[tx_index], tx_index);
				return IPACM_FAILURE;
			}
		}
		else
		{
			IPACMDBG_H("Tx:%d, ip-type: %d match ip-type: %d, RT-rule deleted\n",
					tx_index, tx_prop->tx[tx_index].ip,IPA_IP_v6);

			if (m_routing.DeleteRoutingHdl(odu_route_rule_v6_hdl[tx_index], IPA_IP_v6)
					== false)
			{
				IPACMERR("IP-family:%d, Routing rule(hdl:0x%x) deletion failed with tx_index %d!\n", IPA_IP_v6, odu_route_rule_v6_hdl[tx_index], tx_index);
				return IPACM_FAILURE;
			}
		}
	}

	return IPACM_SUCCESS;
}

/*handle eth client del mode*/
int IPACM_Lan::handle_eth_client_down_evt(uint8_t *mac_addr, uint8_t vlan_id)
{
	int clt_indx;
	uint32_t tx_index;
	int num_eth_client_tmp = num_eth_client;
	int num_v6;
#ifdef FEATURE_IPACM_PER_CLIENT_STATS
	struct wan_ioctl_lan_client_info *client_info;
#endif

	IPACMDBG_H("total client: %d\n", num_eth_client_tmp);

	clt_indx = get_eth_client_index(mac_addr, vlan_id);
	if (clt_indx == IPACM_INVALID_INDEX)
	{
		IPACMDBG_H("eth client not attached\n");
		return IPACM_SUCCESS;
	}

	/* First reset NAT rules and then route rules */
	HandleNeighIpAddrDelEvt(
		get_client_memptr(eth_client, clt_indx)->ipv4_set,
		get_client_memptr(eth_client, clt_indx)->v4_addr,
		get_client_memptr(eth_client, clt_indx)->ipv6_set,
		get_client_memptr(eth_client, clt_indx)->v6_addr);

	if (delete_eth_rtrules(clt_indx, IPA_IP_v4))
	{
		IPACMERR("unbale to delete ecm-client v4 route rules for index: %d\n", clt_indx);
		return IPACM_FAILURE;
	}

	if (delete_eth_rtrules(clt_indx, IPA_IP_v6))
	{
		IPACMERR("unbale to delete ecm-client v6 route rules for index: %d\n", clt_indx);
		return IPACM_FAILURE;
	}

	/* Delete eth client header */
	if(get_client_memptr(eth_client, clt_indx)->ipv4_header_set == true)
	{
		if (m_header.DeleteHeaderHdl(get_client_memptr(eth_client, clt_indx)->hdr_hdl_v4)
				== false)
		{
			return IPACM_FAILURE;
		}
		get_client_memptr(eth_client, clt_indx)->ipv4_header_set = false;
	}

	if(get_client_memptr(eth_client, clt_indx)->ipv6_header_set == true)
	{
		if (m_header.DeleteHeaderHdl(get_client_memptr(eth_client, clt_indx)->hdr_hdl_v6)
				== false)
		{
			return IPACM_FAILURE;
		}
		get_client_memptr(eth_client, clt_indx)->ipv6_header_set = false;
	}

#ifdef FEATURE_IPACM_PER_CLIENT_STATS
	if (get_client_memptr(eth_client, clt_indx)->ipv4_ul_rules_set == true)
	{
		if (delete_uplink_filter_rule_per_client(IPA_IP_v4, get_client_memptr(eth_client, clt_indx)->mac))
		{
			IPACMERR("unbale to delete uplink v4 filter rules for index: %d\n", clt_indx);
			return IPACM_FAILURE;
		}
	}

	if (get_client_memptr(eth_client, clt_indx)->ipv6_ul_rules_set == true)
	{
		if (delete_uplink_filter_rule_per_client(IPA_IP_v6, get_client_memptr(eth_client, clt_indx)->mac))
		{
			IPACMERR("unbale to delete uplink v6 filter rules for index: %d\n", clt_indx);
			return IPACM_FAILURE;
		}
	}
#endif

	/* Reset ip_set to 0*/
	get_client_memptr(eth_client, clt_indx)->ipv4_set = false;
	get_client_memptr(eth_client, clt_indx)->ipv6_set = 0;
	get_client_memptr(eth_client, clt_indx)->ipv4_header_set = false;
	get_client_memptr(eth_client, clt_indx)->ipv6_header_set = false;
	get_client_memptr(eth_client, clt_indx)->route_rule_set_v4 = false;
	get_client_memptr(eth_client, clt_indx)->route_rule_set_v6 = 0;
#ifdef FEATURE_VLAN_MPDN
	get_client_memptr(eth_client, clt_indx)->vlan_id = 0;
#endif
#ifdef FEATURE_IPACM_PER_CLIENT_STATS
	get_client_memptr(eth_client, clt_indx)->ipv4_ul_rules_set = false;
	get_client_memptr(eth_client, clt_indx)->ipv6_ul_rules_set = false;
	if (get_client_memptr(eth_client, clt_indx)->lan_stats_idx != -1)
	{
		/* Clear the lan client info. */
		client_info = (struct wan_ioctl_lan_client_info *)malloc(sizeof(struct wan_ioctl_lan_client_info));
		if (client_info == NULL)
		{
			IPACMERR("Unable to allocate memory\n");
			return IPACM_FAILURE;
		}
		memset(client_info, 0, sizeof(struct wan_ioctl_lan_client_info));
		if (ipa_if_cate == LAN_IF)
		{
			client_info->device_type = IPACM_CLIENT_DEVICE_TYPE_USB;
		}
		else if (ipa_if_cate == ODU_IF && is_odu == true)
		{
				client_info->device_type = IPACM_CLIENT_DEVICE_TYPE_ODU;
		}
		else if (ipa_if_cate == ODU_IF)
		{
			client_info->device_type = IPACM_CLIENT_DEVICE_TYPE_ETH;
		}
		memcpy(client_info->mac,
				get_client_memptr(eth_client, clt_indx)->mac,
				IPA_MAC_ADDR_SIZE);
		client_info->client_init = 0;
		client_info->client_idx = get_client_memptr(eth_client, clt_indx)->lan_stats_idx;
		client_info->ul_src_pipe = (enum ipa_client_type) IPA_CLIENT_MAX;
		if (rx_prop)
		{
			client_info->ul_src_pipe = rx_prop->rx[0].src_pipe;
		}
		clear_lan_client_info(client_info);
		free(client_info);
	}
	get_client_memptr(eth_client, clt_indx)->lan_stats_idx = -1;
	memset(get_client_memptr(eth_client, clt_indx)->wan_ul_fl_rule_hdl_v4, 0, MAX_WAN_UL_FILTER_RULES * sizeof(uint32_t));
	memset(get_client_memptr(eth_client, clt_indx)->wan_ul_fl_rule_hdl_v6, 0, MAX_WAN_UL_FILTER_RULES * sizeof(uint32_t));
#endif

	for (; clt_indx < num_eth_client_tmp - 1; clt_indx++)
	{
		memcpy(get_client_memptr(eth_client, clt_indx)->mac,
					 get_client_memptr(eth_client, (clt_indx + 1))->mac,
					 sizeof(get_client_memptr(eth_client, clt_indx)->mac));

		get_client_memptr(eth_client, clt_indx)->hdr_hdl_v4 = get_client_memptr(eth_client, (clt_indx + 1))->hdr_hdl_v4;
		get_client_memptr(eth_client, clt_indx)->hdr_hdl_v6 = get_client_memptr(eth_client, (clt_indx + 1))->hdr_hdl_v6;
		get_client_memptr(eth_client, clt_indx)->v4_addr = get_client_memptr(eth_client, (clt_indx + 1))->v4_addr;

		get_client_memptr(eth_client, clt_indx)->ipv4_set = get_client_memptr(eth_client, (clt_indx + 1))->ipv4_set;
		get_client_memptr(eth_client, clt_indx)->ipv6_set = get_client_memptr(eth_client, (clt_indx + 1))->ipv6_set;
		get_client_memptr(eth_client, clt_indx)->ipv4_header_set = get_client_memptr(eth_client, (clt_indx + 1))->ipv4_header_set;
		get_client_memptr(eth_client, clt_indx)->ipv6_header_set = get_client_memptr(eth_client, (clt_indx + 1))->ipv6_header_set;

		get_client_memptr(eth_client, clt_indx)->route_rule_set_v4 = get_client_memptr(eth_client, (clt_indx + 1))->route_rule_set_v4;
		get_client_memptr(eth_client, clt_indx)->route_rule_set_v6 = get_client_memptr(eth_client, (clt_indx + 1))->route_rule_set_v6;

#ifdef FEATURE_VLAN_MPDN
		get_client_memptr(eth_client, clt_indx)->vlan_id = get_client_memptr(eth_client, (clt_indx + 1))->vlan_id;
#endif

        for (num_v6=0;num_v6< get_client_memptr(eth_client, clt_indx)->ipv6_set;num_v6++)
	    {
		    get_client_memptr(eth_client, clt_indx)->v6_addr[num_v6][0] = get_client_memptr(eth_client, (clt_indx + 1))->v6_addr[num_v6][0];
		    get_client_memptr(eth_client, clt_indx)->v6_addr[num_v6][1] = get_client_memptr(eth_client, (clt_indx + 1))->v6_addr[num_v6][1];
		    get_client_memptr(eth_client, clt_indx)->v6_addr[num_v6][2] = get_client_memptr(eth_client, (clt_indx + 1))->v6_addr[num_v6][2];
		    get_client_memptr(eth_client, clt_indx)->v6_addr[num_v6][3] = get_client_memptr(eth_client, (clt_indx + 1))->v6_addr[num_v6][3];
        }

		for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
		{
			get_client_memptr(eth_client, clt_indx)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v4 =
				 get_client_memptr(eth_client, (clt_indx + 1))->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v4;

			for(num_v6=0;num_v6< get_client_memptr(eth_client, clt_indx)->route_rule_set_v6;num_v6++)
			{
			  get_client_memptr(eth_client, clt_indx)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v6[num_v6] =
			   	 get_client_memptr(eth_client, (clt_indx + 1))->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v6[num_v6];
			  get_client_memptr(eth_client, clt_indx)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v6_wan[num_v6] =
			   	 get_client_memptr(eth_client, (clt_indx + 1))->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v6_wan[num_v6];
		    }
		}
	}

	IPACMDBG_H(" %d eth client deleted successfully \n", num_eth_client);
	num_eth_client = num_eth_client - 1;
	IPACMDBG_H(" Number of eth client: %d\n", num_eth_client);

	/* Del RM dependency */
	if(num_eth_client == 0)
	{
		if(IPACM_Iface::ipacmcfg->GetIPAVer() >= IPA_HW_None && IPACM_Iface::ipacmcfg->GetIPAVer() < IPA_HW_v4_0)
		{
			/* Delete corresponding ipa_rm_resource_name of TX-endpoint after delete all IPV4V6 RT-rule*/
			IPACMDBG_H("dev %s delete producer dependency\n", dev_name);
			if (tx_prop != NULL)
			{
				IPACMDBG_H("depend Got pipe %d rm index : %d \n", tx_prop->tx[0].dst_pipe, IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
				IPACM_Iface::ipacmcfg->DelRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
			}
		}
	}

	return IPACM_SUCCESS;
}

#ifdef FEATURE_VLAN_MPDN
/* handle LINK DOWN of a physical IF in vlan mode */
int IPACM_Lan::handle_vlan_phys_if_down()
{
	/* delete rules once for each iptype */
	if(is_any_mux_up(IPA_IP_v4))
	{
		if(del_ul_flt_rules(IPA_IP_v4))
		{
			return IPACM_FAILURE;
		}
	}

	if(is_any_mux_up(IPA_IP_v6))
	{
		/* reset usb-client ipv6 rt-rules */
		handle_lan_client_reset_rt(IPA_IP_v6);

		if(del_ul_flt_rules(IPA_IP_v6))
		{
			return IPACM_FAILURE;
		}
	}

	/* notify once per each mux ID per each ip type */
	for(int i = 0; i < IPA_MAX_NUM_HW_PDNS; i++)
	{
		if(v4_mux_up[i])
		{
			IPACMDBG_H("notifying flt removed for mux %d, ipv4\n", v4_mux_up[i]);
			notify_flt_removed(v4_mux_up[i]);
			v4_mux_up[i] = 0;
		}

		if(v6_mux_up[i])
		{
			IPACMDBG_H("notifying flt removed for mux %d, ipv6\n", v6_mux_up[i]);
			notify_flt_removed(v6_mux_up[i]);
			v6_mux_up[i] = 0;
		}
	}

	return IPACM_SUCCESS;
}
#endif

/*handle LAN iface down event*/
int IPACM_Lan::handle_down_evt()
{
	int i;
	int res = IPACM_SUCCESS;
#ifdef FEATURE_IPACM_PER_CLIENT_STATS
	struct wan_ioctl_lan_client_info *client_info;
#endif
	list<l2tp_client_info>::iterator it;
	ipacm_cmd_q_data evt_data;
	ipacm_event_data_all *data_all;

	IPACMDBG_H("lan handle_down_evt\n ");
	if (ipa_if_cate == ODU_IF)
	{
		/* delete ODU default RT rules */
		if (IPACM_Iface::ipacmcfg->ipacm_odu_embms_enable == true)
		{
			IPACMDBG_H("eMBMS enable, delete eMBMS DL RT rule\n");
			handle_odu_route_del();
		}

		/* delete full header */
		if (ipv4_header_set)
		{
			if (m_header.DeleteHeaderHdl(ODU_hdr_hdl_v4)
					== false)
			{
					IPACMERR("ODU ipv4 header delete fail\n");
					res = IPACM_FAILURE;
					goto fail;
			}
			IPACMDBG_H("ODU ipv4 header delete success\n");
		}

		if (ipv6_header_set)
		{
			if (m_header.DeleteHeaderHdl(ODU_hdr_hdl_v6)
					== false)
			{
				IPACMERR("ODU ipv6 header delete fail\n");
				res = IPACM_FAILURE;
				goto fail;
			}
			IPACMERR("ODU ipv6 header delete success\n");
		}
	}

#ifdef FEATURE_L2TP_E2E
	if(ipa_if_cate == ODU_IF)
	{
		if(m_header.DeleteHeaderProcCtx(l2tp_ul_hdr_proc_ctx_hdl) == false)
		{
			IPACMERR("Failed to delete l2tp ul hdr proc ctx.\n");
			res = IPACM_FAILURE;
			goto fail;
		}

		if(m_header.DeleteHeaderHdl(l2tp_ul_dummy_hdr_hdl) == false)
		{
			IPACMERR("Failed to delete l2tp ul dummy hdr.\n");
			res = IPACM_FAILURE;
			goto fail;
		}
	}
#endif

	/* no iface address up, directly close iface*/
	if (ip_type == IPACM_IP_NULL)
	{
		goto fail;
	}

#ifdef FEATURE_VLAN_MPDN
	if(IPACM_Iface::ipacmcfg->iface_in_vlan_mode(dev_name))
	{
		if(handle_vlan_phys_if_down())
		{
			IPACMERR("failed to handle IF down (vlan mode)\n");
			res = IPACM_FAILURE;
			goto fail;
		}
	}
	else
#endif
	{
		/* delete wan filter rule */
		if(IPACM_Wan::isWanUP(ipa_if_num) && rx_prop != NULL)
		{
			IPACMDBG_H("LAN IF goes down, backhaul type %d\n", IPACM_Wan::backhaul_is_sta_mode);
			handle_wan_down(IPACM_Wan::backhaul_is_sta_mode);
		}

		if(IPACM_Wan::isWanUP_V6(ipa_if_num) && rx_prop != NULL)
		{
			IPACMDBG_H("LAN IF goes down, backhaul type %d\n", IPACM_Wan::backhaul_is_sta_mode);
			handle_wan_down_v6(IPACM_Wan::backhaul_is_sta_mode);
		}
	}

	/* delete default filter rules */
	if (ip_type != IPA_IP_v6 && rx_prop != NULL)
	{
		if(m_filtering.DeleteFilteringHdls(ipv4_icmp_flt_rule_hdl, IPA_IP_v4, NUM_IPV4_ICMP_FLT_RULE) == false)
		{
			IPACMERR("Error Deleting ICMPv4 Filtering Rule, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		IPACM_Iface::ipacmcfg->decreaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v4, NUM_IPV4_ICMP_FLT_RULE);

		if(dft_v4fl_rule_hdl[0] != 0)
		{
				if (m_filtering.DeleteFilteringHdls(dft_v4fl_rule_hdl, IPA_IP_v4,
						IPV4_DEFAULT_FILTERTING_RULES) == false)
				{
					IPACMERR("Error Deleting Filtering Rule, aborting...\n");
					res = IPACM_FAILURE;
					goto fail;
				}
				IPACM_Iface::ipacmcfg->decreaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v4, IPV4_DEFAULT_FILTERTING_RULES);
		}

		/* free private-subnet ipv4 filter rules */
		if (IPACM_Iface::ipacmcfg->ipa_num_private_subnet > IPA_PRIV_SUBNET_FILTER_RULE_HANDLES)
		{
			IPACMERR(" the number of rules are bigger than array, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}

#if defined(FEATURE_IPA_ANDROID) || defined(FEATURE_VLAN_MPDN)
		if(m_filtering.DeleteFilteringHdls(private_fl_rule_hdl, IPA_IP_v4, IPA_MAX_PRIVATE_SUBNET_ENTRIES) == false)
		{
			IPACMERR("Error deleting private subnet IPv4 flt rules.\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		IPACM_Iface::ipacmcfg->decreaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v4, IPA_MAX_PRIVATE_SUBNET_ENTRIES);
#else
		if (m_filtering.DeleteFilteringHdls(private_fl_rule_hdl, IPA_IP_v4, IPACM_Iface::ipacmcfg->ipa_num_private_subnet) == false)
		{
			IPACMERR("Error deleting private subnet IPv4 flt rules.\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		IPACM_Iface::ipacmcfg->decreaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v4, IPACM_Iface::ipacmcfg->ipa_num_private_subnet);
#endif
		IPACMDBG_H("Deleted private subnet v4 filter rules successfully.\n");
	}
	IPACMDBG_H("Finished delete default iface ipv4 filtering rules \n ");

	if (ip_type != IPA_IP_v4 && rx_prop != NULL)
	{
		if(m_filtering.DeleteFilteringHdls(ipv6_icmp_flt_rule_hdl, IPA_IP_v6, NUM_IPV6_ICMP_FLT_RULE) == false)
		{
			IPACMERR("Error Deleting ICMPv6 Filtering Rule, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		IPACM_Iface::ipacmcfg->decreaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, NUM_IPV6_ICMP_FLT_RULE);
#ifdef FEATURE_VLAN_MPDN
		if(m_filtering.DeleteFilteringHdls(ipv6_prefix_flt_rule_hdl, IPA_IP_v6,
			IPA_MAX_IPV6_PREFIX_FLT_RULE) == false)
		{
			IPACMERR("Error Deleting Filtering, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		IPACM_Iface::ipacmcfg->decreaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, IPA_MAX_IPV6_PREFIX_FLT_RULE);
		dummy_prefix_installed = false;
#endif

#ifdef FEATURE_L2TP_E2E
		if(ipa_if_cate == ODU_IF)
		{
			if (m_filtering.DeleteFilteringHdls(l2tp_inner_private_subnet_flt_rule_hdl, IPA_IP_v6,
				IPACM_Iface::ipacmcfg->ipa_num_private_subnet) == false)
			{
				IPACMERR("Error Deleting Filtering, aborting...\n");
				res = IPACM_FAILURE;
				goto fail;
			}
			IPACM_Iface::ipacmcfg->decreaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, IPACM_Iface::ipacmcfg->ipa_num_private_subnet);
		}
#endif

		if (dft_v6fl_rule_hdl[0] != 0)
		{
			if (!m_filtering.DeleteFilteringHdls(dft_v6fl_rule_hdl, IPA_IP_v6, m_ipv6_default_filterting_rules_count))
			{
				IPACMERR("Error Adding RuleTable(1) to Filtering, aborting...\n");
				res = IPACM_FAILURE;
				goto fail;
			}
			IPACM_Iface::ipacmcfg->decreaseFltRuleCount(
				rx_prop->rx[0].src_pipe, IPA_IP_v6, m_ipv6_default_filterting_rules_count);
		}
#ifdef FEATURE_L2TP
		if(ipa_if_cate == ODU_IF)
		{
			if(m_filtering.DeleteFilteringHdls(tcp_syn_flt_rule_hdl, IPA_IP_v6, IPA_IP_MAX) == false)
			{
				IPACMERR("Error Deleting TCP SYN L2TP Filtering Rule, aborting...\n");
				res = IPACM_FAILURE;
				goto fail;
			}
		}
#endif
	}
	IPACMDBG_H("Finished delete default iface ipv6 filtering rules \n ");

	if (ip_type != IPA_IP_v6)
	{
		if (m_routing.DeleteRoutingHdl(dft_rt_rule_hdl[0], IPA_IP_v4)
				== false)
		{
			IPACMERR("Routing rule deletion failed!\n");
			res = IPACM_FAILURE;
			goto fail;
		}
	}
	IPACMDBG_H("Finished delete default iface ipv4 rules \n ");

	/* delete default v6 routing rule */
	if (ip_type != IPA_IP_v4)
	{
		/* may have multiple ipv6 iface-RT rules*/
		for (i = 0; i < 2*num_dft_rt_v6; i++)
		{
			if (m_routing.DeleteRoutingHdl(dft_rt_rule_hdl[MAX_DEFAULT_v4_ROUTE_RULES + i], IPA_IP_v6)
					== false)
			{
				IPACMERR("Routing rule deletion failed!\n");
				res = IPACM_FAILURE;
				goto fail;
			}
		}
	}

	IPACMDBG_H("Finished delete default iface ipv6 rules \n ");

	/* free the edm clients cache */
	IPACMDBG_H("Free ecm clients cache\n");

	if(IPACM_Iface::ipacmcfg->GetIPAVer() >= IPA_HW_None && IPACM_Iface::ipacmcfg->GetIPAVer() < IPA_HW_v4_0)
	{
		/* Delete corresponding ipa_rm_resource_name of TX-endpoint after delete all IPV4V6 RT-rule */
		IPACMDBG_H("dev %s delete producer dependency\n", dev_name);
		if (tx_prop != NULL)
		{
			IPACMDBG_H("depend Got pipe %d rm index : %d \n", tx_prop->tx[0].dst_pipe, IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
			IPACM_Iface::ipacmcfg->DelRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
		}
	}
	eth_bridge_post_event(IPA_ETH_BRIDGE_IFACE_DOWN, IPA_IP_MAX, NULL, NULL, NULL);

/* Delete private subnet*/
#ifdef FEATURE_IPA_ANDROID
	if (ip_type != IPA_IP_v6)
	{
		IPACMDBG_H("current IPACM private subnet_addr number(%d)\n", IPACM_Iface::ipacmcfg->ipa_num_private_subnet);
		IPACMDBG_H(" Delete IPACM private subnet_addr as: 0x%x \n", if_ipv4_subnet);
		if(IPACM_Iface::ipacmcfg->DelPrivateSubnet(if_ipv4_subnet, ipa_if_num) == false)
		{
			IPACMERR(" can't Delete IPACM private subnet_addr as: 0x%x \n", if_ipv4_subnet);
		}
	}

	/* reset the IPA-client pipe enum */
	if(ipa_if_cate != WAN_IF)
	{
		handle_tethering_client(true, IPACM_CLIENT_USB);
	}
#endif /* defined(FEATURE_IPA_ANDROID)*/
fail:
	/* clean eth-client header, routing rules */
	IPACMDBG_H("left %d eth clients need to be deleted \n ", num_eth_client);
	for (i = 0; i < num_eth_client; i++)
	{
		if(is_l2tp_iface == false)
		{
			/* First reset NAT/IPv6CT rules and then route rules */
			HandleNeighIpAddrDelEvt(
				get_client_memptr(eth_client, i)->ipv4_set,
				get_client_memptr(eth_client, i)->v4_addr,
				get_client_memptr(eth_client, i)->ipv6_set,
				get_client_memptr(eth_client, i)->v6_addr);

			if (delete_eth_rtrules(i, IPA_IP_v4))
			{
				IPACMERR("unbale to delete ecm-client v4 route rules for index %d\n", i);
				res = IPACM_FAILURE;
			}

			if (delete_eth_rtrules(i, IPA_IP_v6))
			{
				IPACMERR("unbale to delete ecm-client v6 route rules for index %d\n", i);
				res = IPACM_FAILURE;
			}

			IPACMDBG_H("Delete %d client header\n", num_eth_client);

			if(get_client_memptr(eth_client, i)->ipv4_header_set == true)
			{
				if (m_header.DeleteHeaderHdl(get_client_memptr(eth_client, i)->hdr_hdl_v4)
					== false)
				{
					res = IPACM_FAILURE;
				}
			}

			if(get_client_memptr(eth_client, i)->ipv6_header_set == true)
			{
				if (m_header.DeleteHeaderHdl(get_client_memptr(eth_client, i)->hdr_hdl_v6)
						== false)
				{
					res = IPACM_FAILURE;
				}
			}

#ifdef FEATURE_IPACM_PER_CLIENT_STATS
			if (get_client_memptr(eth_client, i)->lan_stats_idx != -1)
			{
				/* Clear the lan client info. */
				client_info = (struct wan_ioctl_lan_client_info *)malloc(sizeof(struct wan_ioctl_lan_client_info));
				if (client_info == NULL)
				{
					IPACMERR("Unable to allocate memory\n");
					res = IPACM_FAILURE;
				}
				else
				{
					memset(client_info, 0, sizeof(struct wan_ioctl_lan_client_info));
					if (ipa_if_cate == LAN_IF)
					{
						client_info->device_type = IPACM_CLIENT_DEVICE_TYPE_USB;
					}
					else if (ipa_if_cate == ODU_IF && is_odu == true)
					{
						client_info->device_type = IPACM_CLIENT_DEVICE_TYPE_ODU;
					}
					else if (ipa_if_cate == ODU_IF)
					{
						client_info->device_type = IPACM_CLIENT_DEVICE_TYPE_ETH;
					}
					memcpy(client_info->mac,
							get_client_memptr(eth_client, i)->mac,
							IPA_MAC_ADDR_SIZE);
					client_info->client_init = 0;
					client_info->client_idx = get_client_memptr(eth_client, i)->lan_stats_idx;
					client_info->ul_src_pipe = (enum ipa_client_type) IPA_CLIENT_MAX;
					if (rx_prop)
					{
						client_info->ul_src_pipe = rx_prop->rx[0].src_pipe;
					}
					if (clear_lan_client_info(client_info))
					{
						res = IPACM_FAILURE;
					}
					free(client_info);
				}
				get_client_memptr(eth_client, i)->lan_stats_idx = -1;
			}
#endif
		}
#ifdef FEATURE_L2TP_E2E
		else
		{
			HandleNeighIpAddrDelEvt(
				get_client_memptr(eth_client, i)->ipv4_set,
				get_client_memptr(eth_client, i)->v4_addr,
				get_client_memptr(eth_client, i)->ipv6_set,
				get_client_memptr(eth_client, i)->v6_addr);

			/* delete dl rules */
			if(m_routing.DeleteRoutingHdl(get_client_memptr(eth_client, i)->dl_first_pass_rt_rule_hdl, IPA_IP_v4) == false)
			{
				IPACMERR("Failed to delete first pass rt rule.\n");
				return IPACM_FAILURE;
			}

			if(m_routing.DeleteRoutingHdl(get_client_memptr(eth_client, i)->dl_second_pass_rt_rule_hdl, IPA_IP_v6) == false)
			{
				IPACMERR("Failed to delete second pass rt rule.\n");
				return IPACM_FAILURE;
			}

			if(m_header.DeleteHeaderProcCtx(get_client_memptr(eth_client, i)->dl_first_pass_hdr_proc_ctx_hdl) == false)
			{
				IPACMERR("Failed to delete first pass hdr proc ctx.\n");
				return IPACM_FAILURE;
			}

			if(m_header.DeleteHeaderHdl(get_client_memptr(eth_client, i)->dl_first_pass_hdr_hdl) == false)
			{
				IPACMERR("Failed to delete first pass hdr.\n");
				return IPACM_FAILURE;
			}

			if(m_header.DeleteHeaderHdl(get_client_memptr(eth_client, i)->dl_second_pass_hdr_hdl) == false)
			{
				IPACMERR("Failed to delete second pass hdr.\n");
				return IPACM_FAILURE;
			}
			/* delete ul rules */
			if(m_filtering.DeleteFilteringHdls(&get_client_memptr(eth_client, i)->ul_first_pass_flt_rule_hdl, IPA_IP_v6, 1) == false)
			{
				IPACMERR("Failed to delete ul flt rule.\n");
				return IPACM_FAILURE;
			}
			IPACM_Iface::ipacmcfg->decreaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, 1);

			if(m_routing.DeleteRoutingHdl(get_client_memptr(eth_client, i)->ul_first_pass_rt_rule_hdl, IPA_IP_v6) == false)
			{
				IPACMERR("Failed to delete ul rt rule.\n");
				return IPACM_FAILURE;
			}
		}
#endif
	} /* end of for loop */
#ifdef FEATURE_L2TP_E2E
	/* post IPA_DEL_L2TP_CLIENT event */
	for(it = IPACM_Iface::ipacmcfg->l2tp_client.begin(); it != IPACM_Iface::ipacmcfg->l2tp_client.end(); it++)
	{
		memset(&evt_data, 0, sizeof(evt_data));
		data_all = (ipacm_event_data_all *)malloc(sizeof(ipacm_event_data_all));
		if(data_all == NULL)
		{
			IPACMERR("Unable to allocate memory for event data.\n");
			return IPACM_FAILURE;
		}
		strlcpy(data_all->iface_name, it->client_iface_name, sizeof(data_all->iface_name));
		evt_data.event = IPA_DEL_L2TP_CLIENT;
		evt_data.evt_data = data_all;
		IPACM_EvtDispatcher::PostEvt(&evt_data);
	}
	IPACM_Iface::ipacmcfg->l2tp_client.clear();
#endif

#ifdef FEATURE_IPACM_PER_CLIENT_STATS
		/* Reset the lan stats indices belonging to this object. */
		if (IPACM_Iface::ipacmcfg->ipacm_lan_stats_enable)
		{
			IPACMDBG_H("Resetting lan stats indices. \n");
			reset_lan_stats_index();
		}
#endif

	/* check software routing fl rule hdl */
	if (softwarerouting_act == true && rx_prop != NULL)
	{
		handle_software_routing_disable();
	}

	if (odu_route_rule_v4_hdl != NULL)
	{
		free(odu_route_rule_v4_hdl);
	}
	if (odu_route_rule_v6_hdl != NULL)
	{
		free(odu_route_rule_v6_hdl);
	}
	if (rx_prop != NULL)
	{
		if(IPACM_Iface::ipacmcfg->GetIPAVer() >= IPA_HW_None && IPACM_Iface::ipacmcfg->GetIPAVer() < IPA_HW_v4_0)
		{
			/* Delete corresponding ipa_rm_resource_name of RX-endpoint after delete all IPV4V6 FT-rule */
			IPACMDBG_H("dev %s delete producer dependency\n", dev_name);
			IPACMDBG_H("depend Got pipe %d rm index : %d \n", rx_prop->rx[0].src_pipe, IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[rx_prop->rx[0].src_pipe]);
			IPACM_Iface::ipacmcfg->DelRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[rx_prop->rx[0].src_pipe]);
			IPACMDBG_H("Finished delete dependency \n ");
		}
#ifndef FEATURE_ETH_BRIDGE_LE
		free(rx_prop);
#endif
	}

	if (eth_client != NULL)
	{
		free(eth_client);
	}
#ifndef FEATURE_ETH_BRIDGE_LE
	if (tx_prop != NULL)
	{
		free(tx_prop);
	}
	if (iface_query != NULL)
	{
		free(iface_query);
	}
#endif
	is_active = false;
	post_del_self_evt();

	return res;
}

/* install UL filter rule from Q6 */
#ifdef FEATURE_VLAN_MPDN
int IPACM_Lan::handle_uplink_filter_rule(ipacm_ext_prop *prop, ipa_ip_type iptype, uint8_t pdn_mux_id, bool notif_only, bool is_xlat)
#else
int IPACM_Lan::handle_uplink_filter_rule(ipacm_ext_prop *prop, ipa_ip_type iptype, uint8_t xlat_mux_id)
#endif
{
	ipa_flt_rule_add flt_rule_entry;
	int len = 0, cnt, ret = IPACM_SUCCESS;
	ipa_ioc_add_flt_rule *pFilteringTable;
	ipa_fltr_installed_notif_req_msg_v01 flt_index;
	int fd;
	int i, index;
	uint32_t value = 0;

	IPACMDBG_H("Set modem UL flt rules\n");

	if (rx_prop == NULL)
	{
		IPACMDBG_H("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	if(prop == NULL || prop->num_ext_props <= 0)
	{
		IPACMDBG_H("No extended property.\n");
		return IPACM_SUCCESS;
	}

	fd = open(IPA_DEVICE_NAME, O_RDWR);
	if (0 == fd)
	{
		IPACMERR("Failed opening %s.\n", IPA_DEVICE_NAME);
		return IPACM_FAILURE;
	}
	if (prop->num_ext_props > MAX_WAN_UL_FILTER_RULES)
	{
		IPACMERR("number of modem UL rules > MAX_WAN_UL_FILTER_RULES, aborting...\n");
		close(fd);
		return IPACM_FAILURE;
	}

	memset(&flt_index, 0, sizeof(flt_index));
	flt_index.source_pipe_index = ioctl(fd, IPA_IOC_QUERY_EP_MAPPING, rx_prop->rx[0].src_pipe);
#ifdef FEATURE_IPACM_PER_CLIENT_STATS
	if (tx_prop && IPACM_Iface::ipacmcfg->ipacm_lan_stats_enable)
	{
		flt_index.dst_pipe_id_valid = 1;
		flt_index.dst_pipe_id_len = tx_prop->num_tx_props;
		for (i = 0; i < tx_prop->num_tx_props && i < QMI_IPA_MAX_CLIENT_DST_PIPES; i++)
		{
			flt_index.dst_pipe_id[i] = ioctl(fd, IPA_IOC_QUERY_EP_MAPPING, tx_prop->tx[i].dst_pipe);
		}
	}
#endif
	flt_index.install_status = IPA_QMI_RESULT_SUCCESS_V01;
#ifndef FEATURE_IPA_V3
	flt_index.filter_index_list_len = prop->num_ext_props;
#else /* defined (FEATURE_IPA_V3) */
	flt_index.rule_id_valid = 1;
	flt_index.rule_id_len = prop->num_ext_props;
#endif
	flt_index.embedded_pipe_index_valid = 1;
	flt_index.embedded_pipe_index = ioctl(fd, IPA_IOC_QUERY_EP_MAPPING, IPA_CLIENT_APPS_LAN_WAN_PROD);
	flt_index.retain_header_valid = 1;
	flt_index.retain_header = 0;
	flt_index.embedded_call_mux_id_valid = 1;
#ifdef FEATURE_VLAN_MPDN
	if (is_xlat)
		flt_index.embedded_call_mux_id = IPACM_Iface::ipacmcfg->GetQmapId();
	else
		flt_index.embedded_call_mux_id = pdn_mux_id;
#else
	flt_index.embedded_call_mux_id = IPACM_Iface::ipacmcfg->GetQmapId();
#endif
#ifndef FEATURE_IPA_V3
	IPACMDBG_H("flt_index: src pipe: %d, num of rules: %d, ebd pipe: %d, mux id: %d\n",
		flt_index.source_pipe_index, flt_index.filter_index_list_len, flt_index.embedded_pipe_index, flt_index.embedded_call_mux_id);
#else /* defined (FEATURE_IPA_V3) */
	IPACMDBG_H("flt_index: src pipe: %d, num of rules: %d, ebd pipe: %d, mux id: %d\n",
		flt_index.source_pipe_index, flt_index.rule_id_len, flt_index.embedded_pipe_index, flt_index.embedded_call_mux_id);
#endif
	len = sizeof(struct ipa_ioc_add_flt_rule) + prop->num_ext_props * sizeof(struct ipa_flt_rule_add);
	pFilteringTable = (struct ipa_ioc_add_flt_rule*)malloc(len);
	if (pFilteringTable == NULL)
	{
		IPACMERR("Error Locate ipa_flt_rule_add memory...\n");
		close(fd);
		return IPACM_FAILURE;
	}
	memset(pFilteringTable, 0, len);

	pFilteringTable->commit = 1;
	pFilteringTable->ep = rx_prop->rx[0].src_pipe;
	pFilteringTable->global = false;
	pFilteringTable->ip = iptype;
	pFilteringTable->num_rules = prop->num_ext_props;

	memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add)); // Zero All Fields
	flt_rule_entry.at_rear = 1;
#ifdef FEATURE_IPA_V3
	if (flt_rule_entry.rule.eq_attrib.ipv4_frag_eq_present)
		flt_rule_entry.at_rear = 0;
#endif
	flt_rule_entry.flt_rule_hdl = -1;
	flt_rule_entry.status = -1;

	flt_rule_entry.rule.retain_hdr = 0;
	flt_rule_entry.rule.to_uc = 0;
	flt_rule_entry.rule.eq_attrib_type = 1;
	if(iptype == IPA_IP_v4)
	{
		if (ipa_if_cate == ODU_IF && IPACM_Wan::isWan_Bridge_Mode())
		{
			IPACMDBG_H("WAN, ODU are in bridge mode \n");
			flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
		}
		else
		{
			flt_rule_entry.rule.action = IPA_PASS_TO_SRC_NAT;

			/* NAT block will set the proper MUX ID in the metadata according to the relevant PDN */
			if (IPACM_Iface::ipacmcfg->GetIPAVer() >= IPA_HW_v4_0)
				flt_rule_entry.rule.set_metadata = true;
		}
	}
	else if(iptype == IPA_IP_v6)
	{
		flt_rule_entry.rule.action = IPACM_Iface::ipacmcfg->IsIpv6CTEnabled()?
			IPA_PASS_TO_SRC_NAT : IPA_PASS_TO_ROUTING;
	}
	else
	{
		IPACMERR("IP type is not expected.\n");
		ret = IPACM_FAILURE;
		goto fail;
	}

	index = IPACM_Iface::ipacmcfg->getFltRuleCount(rx_prop->rx[0].src_pipe, iptype);

	for(cnt=0; cnt<prop->num_ext_props; cnt++)
	{
		memcpy(&flt_rule_entry.rule.eq_attrib,
					 &prop->prop[cnt].eq_attrib,
					 sizeof(prop->prop[cnt].eq_attrib));
		flt_rule_entry.rule.rt_tbl_idx = prop->prop[cnt].rt_tbl_idx;
#ifndef FEATURE_VLAN_MPDN
		/* Handle XLAT configuration */
		if ((iptype == IPA_IP_v4) && prop->prop[cnt].is_xlat_rule && (xlat_mux_id != 0))
		{
			/* fill the value of meta-data */
			value = xlat_mux_id;
			flt_rule_entry.rule.eq_attrib.metadata_meq32_present = 1;
			flt_rule_entry.rule.eq_attrib.metadata_meq32.offset = 0;
			flt_rule_entry.rule.eq_attrib.metadata_meq32.value = (value & 0xFF) << 16;
			flt_rule_entry.rule.eq_attrib.metadata_meq32.mask = 0x00FF0000;
			IPACMDBG_H("xlat meta-data is modified for rule: %d has index %d with xlat_mux_id: %d\n",
					cnt, index, xlat_mux_id);
		}
#else
		/* Handle XLAT configuration */
		if ((iptype == IPA_IP_v4) && prop->prop[cnt].is_xlat_rule && (pdn_mux_id != 0) && is_xlat)
		{
			/* fill the value of meta-data */
			value = pdn_mux_id;
			flt_rule_entry.rule.eq_attrib.metadata_meq32_present = 1;
			flt_rule_entry.rule.eq_attrib.metadata_meq32.offset = 0;
			flt_rule_entry.rule.eq_attrib.metadata_meq32.value = (value & 0xFF) << 16;
			flt_rule_entry.rule.eq_attrib.metadata_meq32.mask = 0x00FF0000;
			IPACMDBG_H("xlat meta-data is modified for rule: %d has index %d with xlat_mux_id: %d\n",
					cnt, index, pdn_mux_id);
		}
#endif
#ifdef FEATURE_IPA_V3
		flt_rule_entry.rule.hashable = prop->prop[cnt].is_rule_hashable;
		flt_rule_entry.rule.rule_id = prop->prop[cnt].rule_id;
		if(rx_prop->rx[0].attrib.attrib_mask & IPA_FLT_META_DATA)	//turn on meta-data equation
		{
			flt_rule_entry.rule.eq_attrib.rule_eq_bitmap |= (1<<9);
			flt_rule_entry.rule.eq_attrib.metadata_meq32_present = 1;
			flt_rule_entry.rule.eq_attrib.metadata_meq32.offset = 0;
			flt_rule_entry.rule.eq_attrib.metadata_meq32.value |= rx_prop->rx[0].attrib.meta_data;
			flt_rule_entry.rule.eq_attrib.metadata_meq32.mask |= rx_prop->rx[0].attrib.meta_data_mask;
		}
#endif
		memcpy(&pFilteringTable->rules[cnt], &flt_rule_entry, sizeof(flt_rule_entry));

		IPACMDBG_H("Modem UL filtering rule %d has index %d\n", cnt, index);
#ifndef FEATURE_IPA_V3
		flt_index.filter_index_list[cnt].filter_index = index;
		flt_index.filter_index_list[cnt].filter_handle = prop->prop[cnt].filter_hdl;
#else /* defined (FEATURE_IPA_V3) */
		flt_index.rule_id[cnt] = prop->prop[cnt].rule_id;
#endif
		index++;
	}

	if(false == m_filtering.SendFilteringRuleIndex(&flt_index))
	{
		IPACMERR("Error sending filtering rule index, aborting...\n");
		ret = IPACM_FAILURE;
		goto fail;
	}
#ifdef FEATURE_VLAN_MPDN
	if(notif_only)
	{
		IPACMDBG_H("UL filtering rules already installed for %s, only sent notification for modem (mux %d)\n",
			dev_name, pdn_mux_id);
		ret = IPACM_SUCCESS;
		goto finish_notif;
	}
	else
	{
		IPACMDBG_H("this is the first PDN for dev %s, commiting modem UL rules, mux %d\n", dev_name, pdn_mux_id);
	}
#endif
#ifdef FEATURE_IPACM_PER_CLIENT_STATS
	if (IPACM_Iface::ipacmcfg->ipacm_lan_stats_enable == false)
#endif
	{
		if(false == m_filtering.AddFilteringRule(pFilteringTable))
		{
			IPACMERR("Error Adding RuleTable to Filtering, aborting...\n");
			ret = IPACM_FAILURE;
			goto fail;
		}
		else
		{
			if(iptype == IPA_IP_v4)
			{
				for(i=0; i<pFilteringTable->num_rules; i++)
				{
					wan_ul_fl_rule_hdl_v4[num_wan_ul_fl_rule_v4] = pFilteringTable->rules[i].flt_rule_hdl;
					num_wan_ul_fl_rule_v4++;
				}
				IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, iptype, pFilteringTable->num_rules);
			}
			else if(iptype == IPA_IP_v6)
			{
				for(i=0; i<pFilteringTable->num_rules; i++)
				{
					wan_ul_fl_rule_hdl_v6[num_wan_ul_fl_rule_v6] = pFilteringTable->rules[i].flt_rule_hdl;
					num_wan_ul_fl_rule_v6++;
				}
				IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, iptype, pFilteringTable->num_rules);
			}
			else
			{
				IPACMERR("IP type is not expected.\n");
				goto fail;
			}
		}
	}
#ifdef FEATURE_IPACM_PER_CLIENT_STATS
	else
	{
		if(iptype == IPA_IP_v4)
		{
			num_wan_ul_fl_rule_v4 = pFilteringTable->num_rules;
		}
		else if(iptype == IPA_IP_v6)
		{
			num_wan_ul_fl_rule_v6 = pFilteringTable->num_rules;
		}
		else
		{
			IPACMERR("IP type is not expected.\n");
			goto fail;
		}

	}
#endif
fail:
finish_notif:
	free(pFilteringTable);
	close(fd);
	return ret;
}

#ifdef FEATURE_IPACM_UL_FIREWALL
void IPACM_Lan::change_to_network_order(ipa_ip_type iptype, ipa_rule_attrib* attrib)
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

/* clean UL firewall filter rules (IPv6 only) from LAN prod pipe, Q6 rules handled separately*/
int IPACM_Lan::delete_uplink_filter_rule_ul(ul_firewall_t *ul_firewall)
{
	uint32_t *flt_rule_hdls = NULL;
	int num_of_rules = 0;
	if (rx_prop == NULL)
	{
		IPACMDBG_H("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	IPACMDBG_H("Deleting UL firewall rules for pipe (%d)\n", rx_prop->rx[0].src_pipe);
#ifdef FEATURE_VLAN_MPDN
	if(ul_firewall->num_ul_frag_installed)
	{
		IPACMDBG_H("deleting %d UL frag flt rules\n", ul_firewall->num_ul_frag_installed);
		if (ul_firewall->num_ul_frag_installed > IPA_MAX_NUM_HW_PDNS)
		{
			IPACMDBG_H("Invalid number of UL fragment rules\n");
			return IPACM_FAILURE;
		}
		flt_rule_hdls = ul_firewall->ul_frag_handle;
		if(m_filtering.DeleteFilteringHdls(flt_rule_hdls, IPA_IP_v6, ul_firewall->num_ul_frag_installed) == false)
		{
			IPACMERR("Error deleting IPv6 UL frag filtering rules.\n");
			return IPACM_FAILURE;
		}
		IPACM_Iface::ipacmcfg->decreaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, ul_firewall->num_ul_frag_installed);
		ul_firewall->num_ul_frag_installed = 0;
	}
	else
	{
		IPACMDBG_H("no UL frag flt rules were installed\n");
	}
#else
	if (true == ul_firewall->ul_frag_installed)
	{
		flt_rule_hdls = &ul_firewall->ul_frag_handle;

		if (m_filtering.DeleteFilteringHdls(flt_rule_hdls, IPA_IP_v6, 1) == false)
		{
			IPACMERR("Error deleting IPv6 UL frag filtering rules.\n");
			return IPACM_FAILURE;
		}
		ul_firewall->ul_frag_installed = false;
		IPACM_Iface::ipacmcfg->decreaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, 1);
		IPACMDBG_H("Frag deleted successfully\n");
	}
#endif

	if (ul_firewall->num_ul_firewall_installed && 
		ul_firewall->num_ul_firewall_installed < IPACM_MAX_FIREWALL_ENTRIES)
	{
		flt_rule_hdls = ul_firewall->ul_firewall_handle;
		if (m_filtering.DeleteFilteringHdls(flt_rule_hdls,
					IPA_IP_v6, ul_firewall->num_ul_firewall_installed) == false)
		{
			IPACMERR("Error Deleting UL Filtering rules, aborting...\n");
			return IPACM_FAILURE;
		}
		IPACM_Iface::ipacmcfg->decreaseFltRuleCount(rx_prop->rx[0].src_pipe,
			IPA_IP_v6,
			ul_firewall->num_ul_firewall_installed);
		IPACMDBG_H("%d num UL rules on pipe (%d) deleted successfully\n",
			ul_firewall->num_ul_firewall_installed,
			rx_prop->rx[0].src_pipe);
	}
	else if (ul_firewall->num_ul_firewall_installed > IPACM_MAX_FIREWALL_ENTRIES)
	{
		IPACMDBG_H("The number of ul firewall rules exceed limit.\n");
	}
	else
	{
		IPACMDBG_H("No UL Firewall filter rule to delete\n");
	}
	memset(ul_firewall, 0, sizeof (ul_firewall_t));
	return IPACM_SUCCESS;
}

/* Send UL firewall WhiteListing rules to Q6 */
int IPACM_Lan::install_wan_firewall_rule_ul(bool enable, int vid, int num_of_ul_rules)
{
	int len, res = IPACM_SUCCESS;
	uint8_t mux_id;
	ipa_ioc_add_flt_rule *pFilteringTable_v6 = NULL;

	mux_id = IPACM_Iface::ipacmcfg->GetQmapId();
	if (rx_prop == NULL)
	{
		IPACMDBG_H("No rx properties registered for iface %s\n", dev_name);
		return IPACM_FAILURE;
	}
#ifdef FEATURE_VLAN_MPDN
	if(vid && IPACM_Wan::GetMuxByVid(vid, &mux_id, IPA_IP_v6))
	{
		IPACMERR("failed getting mux for vid %d\n", vid);
		return IPACM_FAILURE;
	}
#endif

	/* Not considering is_sw_routing and embm is on or off */

	if (num_of_ul_rules >= 0)
	{
		len = sizeof(struct ipa_ioc_add_flt_rule) + num_of_ul_rules * sizeof(struct ipa_flt_rule_add);
		pFilteringTable_v6 = (struct ipa_ioc_add_flt_rule*)malloc(len);

		IPACMDBG_H("Total number of WAN UL filtering rule for IPv6 is %d : mux_id (%d), vid (%d)\n", num_of_ul_rules,
			mux_id, vid);

		if (pFilteringTable_v6 == NULL)
		{
			IPACMERR("Error Locate ipa_flt_rule_add memory...\n");
			return IPACM_FAILURE;
		}
		memset(pFilteringTable_v6, 0, len);
		pFilteringTable_v6->commit = 1;
		pFilteringTable_v6->ep = rx_prop->rx[0].src_pipe;
		pFilteringTable_v6->global = false;
		pFilteringTable_v6->ip = IPA_IP_v6;
		pFilteringTable_v6->num_rules = (uint8_t)num_of_ul_rules;

		memcpy(pFilteringTable_v6->rules, IPACM_Wan::firewall_flt_rule_v6_ul, num_of_ul_rules * sizeof(ipa_flt_rule_add));
	}
	if (false == m_filtering.AddWanULFilteringRule(pFilteringTable_v6, mux_id, enable))
	{
		IPACMERR("Failed to install WAN UL filtering table.\n");
		res = IPACM_FAILURE;
		goto fail;
	}

fail:
	if (pFilteringTable_v6 != NULL)
	{
		free (pFilteringTable_v6);
	}
	return res;
}

/* Config UL frag firewall filter rules */
int IPACM_Lan::config_wan_frag_firewall_rule_ul_ex(ul_firewall_t *ul_firewall, int vid)
{
	struct ipa_flt_rule_add flt_rule_entry;
	int len = 0, index, rule_v6_ul = 0;
	uint32_t *flt_rule_hdls = NULL;

	if (rx_prop == NULL)
	{
		IPACMDBG_H("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	if (ipacmcfg->IsIpv6CTEnabled())
	{
		IPACMDBG_H("The fragment rule already installed. Nothing to do\n");
		return IPACM_SUCCESS;
	}
#ifdef FEATURE_VLAN_MPDN
	uint8_t mux_id = 0;
	if(IPACM_Wan::GetMuxByVid(vid, &mux_id, IPA_IP_v6))
	{
		IPACMERR("couldn't get MUX for VID %d, dev %s\n", vid, dev_name);
		return IPACM_FAILURE;
	}
#endif

	/* Frag rule installation */
	/* construct ipa_ioc_add_flt_rule with 1 frag rule */
	ipa_ioc_add_flt_rule *m_pFilteringTable = NULL;
	len = sizeof(struct ipa_ioc_add_flt_rule) + 1 * sizeof(struct ipa_flt_rule_add);
	m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)calloc(1, len);

	if (!m_pFilteringTable)
	{
		IPACMERR("Error Locate ipa_flt_rule_add memory...\n");
		return IPACM_FAILURE;
	}

	memset(m_pFilteringTable, 0, len);

	m_pFilteringTable->commit = 1;
	m_pFilteringTable->ep = rx_prop->rx[0].src_pipe;
	m_pFilteringTable->global = false;
	m_pFilteringTable->ip = IPA_IP_v6;
	m_pFilteringTable->num_rules = (uint8_t)1;

	memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
	flt_rule_entry.at_rear = false;
	flt_rule_entry.rule.hashable = false;
	flt_rule_entry.flt_rule_hdl = -1;
	flt_rule_entry.status = -1;
	flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;
	flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_FRAGMENT;
#ifdef FEATURE_VLAN_MPDN
	uint32_t v6_prefix[2];
	if(IPACM_Wan::GetV6PrefixByVid(vid, v6_prefix))
	{
		IPACMERR("couldn't get v6 prefix for vid %d\n", vid);
		return IPACM_FAILURE;
	}
	flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_SRC_ADDR;
	flt_rule_entry.rule.attrib.u.v6.src_addr[0] = v6_prefix[0];
	flt_rule_entry.rule.attrib.u.v6.src_addr[1] = v6_prefix[1];
	flt_rule_entry.rule.attrib.u.v6.src_addr[2] = 0x0;
	flt_rule_entry.rule.attrib.u.v6.src_addr[3] = 0x0;
	flt_rule_entry.rule.attrib.u.v6.src_addr_mask[0] = 0xFFFFFFFF;
	flt_rule_entry.rule.attrib.u.v6.src_addr_mask[1] = 0xFFFFFFFF;
	flt_rule_entry.rule.attrib.u.v6.src_addr_mask[2] = 0x0;
	flt_rule_entry.rule.attrib.u.v6.src_addr_mask[3] = 0x0;
#endif

	memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

	if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
	{
		IPACMERR("Error Adding RuleTable(0) to Filtering, aborting...\n");
		free(m_pFilteringTable);
		return IPACM_FAILURE;
	}
	else
	{
		IPACM_Iface::ipacmcfg->increaseFltRuleCount(m_pFilteringTable->ep, IPA_IP_v6, 1);
		IPACMDBG_H("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
	}
#ifdef FEATURE_VLAN_MPDN
	ul_firewall->ul_frag_handle[ul_firewall->num_ul_frag_installed] = m_pFilteringTable->rules[0].flt_rule_hdl;
	ul_firewall->num_ul_frag_installed++;
#else
	ul_firewall->ul_frag_handle = m_pFilteringTable->rules[0].flt_rule_hdl;
	ul_firewall->ul_frag_installed = true;
#endif
	return IPACM_SUCCESS;
}

/* Configure UL firewall rules, to be sent to Q6 side*/
int IPACM_Lan::config_dft_firewall_rules_ul_ex(IPACM_firewall_conf_t* firewall_conf,
	struct ipa_flt_rule_add *rules, int vid)
{
	struct ipa_flt_rule_add flt_rule_entry;
	ipa_ioc_get_rt_tbl_indx rt_tbl_idx;
	ipa_ioc_generate_flt_eq flt_eq;
	int i, len = 0, rule_v6_ul = 0;
	int orig_num_q6_rules = 0;

	if (rx_prop == NULL)
	{
		IPACMDBG_H("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	if(IPACM_Wan::get_pdn_num_fw_rules_by_vid(vid, &orig_num_q6_rules))
	{
		IPACMERR("failed getting num of Q6 rules for VID %d\n", vid);
		return IPACM_FAILURE;
	}

	for (i = 0; i < firewall_conf->num_extd_firewall_entries; i++)
	{
		if (firewall_conf->extd_firewall_entries[i].ip_vsn == 6 &&
			firewall_conf->extd_firewall_entries[i].firewall_direction
			== IPACM_MSGR_UL_FIREWALL)
		{
			memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
			flt_rule_entry.at_rear = true;
			flt_rule_entry.flt_rule_hdl = -1;
			flt_rule_entry.status = -1;
			flt_rule_entry.rule.retain_hdr = 1;
			flt_rule_entry.rule.to_uc = 0;
			flt_rule_entry.rule.eq_attrib_type = 1;
			flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;

			flt_rule_entry.rule.hashable = true;

			memset(&rt_tbl_idx, 0, sizeof(rt_tbl_idx));
			rt_tbl_idx.ip = IPA_IP_v6;
			/* matched rules for v6 go PASS_TO_ROUTE */
			if(firewall_conf->rule_action_accept == true)
			{
				strlcpy(rt_tbl_idx.name, IPACM_Iface::ipacmcfg->rt_tbl_wan_v6.name, IPA_RESOURCE_NAME_MAX);
			}
			else
			{
				strlcpy(rt_tbl_idx.name, IPACM_Iface::ipacmcfg->rt_tbl_wan_dl.name, IPA_RESOURCE_NAME_MAX);
			}
			rt_tbl_idx.name[IPA_RESOURCE_NAME_MAX-1] = '\0';
			if(0 != ioctl(IPACM_Wan::m_fd_ipa_ul, IPA_IOC_QUERY_RT_TBL_INDEX, &rt_tbl_idx))
			{
				IPACMERR("Failed to get routing table index from name\n");
				return IPACM_FAILURE;
			}
			flt_rule_entry.rule.rt_tbl_idx = rt_tbl_idx.idx;
			IPACMDBG_H("Routing table %s has index %d\n", rt_tbl_idx.name, rt_tbl_idx.idx);

			memcpy(&flt_rule_entry.rule.attrib,
			&firewall_conf->extd_firewall_entries[i].attrib,
			sizeof(struct ipa_rule_attrib));
			flt_rule_entry.rule.attrib.attrib_mask |= rx_prop->rx[0].attrib.attrib_mask;
			flt_rule_entry.rule.attrib.attrib_mask &= ~IPA_FLT_META_DATA;
			flt_rule_entry.rule.attrib.meta_data_mask = rx_prop->rx[0].attrib.meta_data_mask;
			flt_rule_entry.rule.attrib.meta_data = rx_prop->rx[0].attrib.meta_data;
			change_to_network_order(IPA_IP_v6, &flt_rule_entry.rule.attrib);

			/* check if the rule is define as TCP/UDP */
			if (firewall_conf->extd_firewall_entries[i].attrib.u.v6.next_hdr == IPACM_FIREWALL_IPPROTO_TCP_UDP)
			{
				/* insert TCP rule*/
				flt_rule_entry.rule.attrib.u.v6.next_hdr = IPACM_FIREWALL_IPPROTO_TCP;
				memset(&flt_eq, 0, sizeof(flt_eq));
				memcpy(&flt_eq.attrib, &flt_rule_entry.rule.attrib, sizeof(flt_eq.attrib));
				flt_eq.ip = IPA_IP_v6;
				if(0 != ioctl(IPACM_Wan::m_fd_ipa_ul, IPA_IOC_GENERATE_FLT_EQ, &flt_eq))
				{
					IPACMERR("Failed to get eq_attrib\n");
					return IPACM_FAILURE;
				}

				memcpy(&flt_rule_entry.rule.eq_attrib,
					&flt_eq.eq_attrib,
					sizeof(flt_rule_entry.rule.eq_attrib));
				memcpy(&(rules[rule_v6_ul]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
				rule_v6_ul++;

				/* insert UDP rule*/
				flt_rule_entry.rule.attrib.u.v6.next_hdr = IPACM_FIREWALL_IPPROTO_UDP;
				memset(&flt_eq, 0, sizeof(flt_eq));
				memcpy(&flt_eq.attrib, &flt_rule_entry.rule.attrib, sizeof(flt_eq.attrib));
				flt_eq.ip = IPA_IP_v6;
				if(0 != ioctl(IPACM_Wan::m_fd_ipa_ul, IPA_IOC_GENERATE_FLT_EQ, &flt_eq))
				{
					IPACMERR("Failed to get eq_attrib\n");
					return IPACM_FAILURE;
				}

				memcpy(&flt_rule_entry.rule.eq_attrib,
					&flt_eq.eq_attrib,
					sizeof(flt_rule_entry.rule.eq_attrib));
				memcpy(&(rules[rule_v6_ul]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
				rule_v6_ul++;
			}
			else
			{
				memset(&flt_eq, 0, sizeof(flt_eq));
				memcpy(&flt_eq.attrib, &flt_rule_entry.rule.attrib, sizeof(flt_eq.attrib));
				flt_eq.ip = IPA_IP_v6;
				if(0 != ioctl(IPACM_Wan::m_fd_ipa_ul, IPA_IOC_GENERATE_FLT_EQ, &flt_eq))
				{
					IPACMERR("Failed to get eq_attrib\n");
					return IPACM_FAILURE;
				}

				memcpy(&flt_rule_entry.rule.eq_attrib,
					&flt_eq.eq_attrib,
					sizeof(flt_rule_entry.rule.eq_attrib));
				memcpy(&(rules[rule_v6_ul]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
				rule_v6_ul++;
			}
		}
	} /* end of firewall ipv6 filter rule add for loop*/

	if(IPACM_Wan::num_firewall_v6_ul - orig_num_q6_rules + rule_v6_ul > IPACM_MAX_FIREWALL_ENTRIES)
	{
		IPACMERR("exceeded overall number of possible Q6 firewall rules for all PDNs, aborting\n");
		return IPACM_FAILURE;
	}

	if(IPACM_Lan::install_wan_firewall_rule_ul(true, vid, rule_v6_ul))
	{
		IPACMERR("failed sending QMI to Q6\n");
		return IPACM_FAILURE;
	}

	if(IPACM_Wan::set_pdn_num_fw_rules_by_vid(vid, rule_v6_ul))
	{
		IPACMERR("failed setting num of Q6 rules for VID %d\n", vid);
	}
	IPACMDBG_H("total %d Q6 UL firewall rules sent to Q6, %d just sent for vid %d\n ",
		IPACM_Wan::num_firewall_v6_ul,
		rule_v6_ul,
		vid);
	return IPACM_SUCCESS;
}

/* delete UL firewall rules, to be sent to Q6 side*/
int IPACM_Lan::disable_dft_firewall_rules_ul_ex(int vid)
{
	if(IPACM_Lan::install_wan_firewall_rule_ul(false, vid, 0))
	{
		IPACMERR("failed sending QMI to Q6\n");
		return IPACM_FAILURE;
	}

	if(IPACM_Wan::set_pdn_num_fw_rules_by_vid(vid, 0))
	{
		IPACMERR("failed setting num of Q6 rules for VID %d\n", vid);
		return IPACM_FAILURE;
	}
	return IPACM_SUCCESS;
}

/* Configure and install UL firewall rules, to be installed on client side */
int IPACM_Lan::config_dft_firewall_rules_ul(IPACM_firewall_conf_t* firewall_conf,
				ul_firewall_t *ul_firewall, int vid)
{
	struct ipa_flt_rule_add flt_rule_entry;
	int len = 0, i;
	int res = IPACM_SUCCESS;

	if (rx_prop == NULL)
	{
		IPACMDBG_H("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	IPACMDBG_H("num rules %d, pdn dev_name %s, accept %d\n",
		firewall_conf->num_extd_firewall_entries,
		firewall_conf->net_dev,
		firewall_conf->rule_action_accept);

#ifdef FEATURE_VLAN_MPDN
	uint32_t v6_prefix[2];
	if(IPACM_Wan::GetV6PrefixByVid(vid, v6_prefix))
	{
		IPACMERR("couldn't get v6 prefix for vid %d\n", vid);
		return IPACM_FAILURE;
	}
#endif

	/* construct ipa_ioc_add_flt_rule with N firewall rules */
	ipa_ioc_add_flt_rule *m_pFilteringTable = NULL;
	len = sizeof(struct ipa_ioc_add_flt_rule) + 1 * sizeof(struct ipa_flt_rule_add);
	m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)calloc(1, len);

	if (!m_pFilteringTable)
	{
		IPACMERR("Error Locate ipa_flt_rule_add memory...\n");
		return IPACM_FAILURE;
	}

	if (false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_wan_v6))
	{
		IPACMERR("m_routing.GetRoutingTable(rt_tbl_wan_v6) Failed.\n");
		res = IPACM_FAILURE;
		goto fail;
	}

	if(ul_firewall->num_ul_firewall_installed >= IPACM_MAX_FIREWALL_ENTRIES)
	{
		IPACMERR("reached MAX num of UL FW rules for ep, skipping pdn firewall (vid %d)\n", vid);
		res = IPACM_FAILURE;
		goto fail;
	}

	/* Catch-all filter rule in case of whitelisting case, redirecting packets to exception path */
	if (firewall_conf->rule_action_accept == true)
	{
		memset(m_pFilteringTable, 0, len);
		m_pFilteringTable->commit = 1;

		m_pFilteringTable->ep = rx_prop->rx[0].src_pipe;
		m_pFilteringTable->global = false;
		m_pFilteringTable->ip = IPA_IP_v6;
		m_pFilteringTable->num_rules = (uint8_t)1;

		IPACMDBG_H ("Catch all rule to drop all in excep path\n");

		memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
	   	flt_rule_entry.at_rear = false;
		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;
		flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;
		flt_rule_entry.rule.hashable = true;
#ifdef FEATURE_VLAN_MPDN
		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_SRC_ADDR;
		flt_rule_entry.rule.attrib.u.v6.src_addr[0] = v6_prefix[0];
		flt_rule_entry.rule.attrib.u.v6.src_addr[1] = v6_prefix[1];
		flt_rule_entry.rule.attrib.u.v6.src_addr[2] = 0x0;
		flt_rule_entry.rule.attrib.u.v6.src_addr[3] = 0x0;
		flt_rule_entry.rule.attrib.u.v6.src_addr_mask[0] = 0xFFFFFFFF;
		flt_rule_entry.rule.attrib.u.v6.src_addr_mask[1] = 0xFFFFFFFF;
		flt_rule_entry.rule.attrib.u.v6.src_addr_mask[2] = 0x0;
		flt_rule_entry.rule.attrib.u.v6.src_addr_mask[3] = 0x0;
#endif
		memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

		if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
		{
			IPACMERR("Error Adding Filtering rules, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else
		{
			IPACM_Iface::ipacmcfg->increaseFltRuleCount(m_pFilteringTable->ep, IPA_IP_v6, 1);
			/* save v6 firewall filter rule handler */
			IPACMDBG_H("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
		}

		ul_firewall->ul_firewall_handle[ul_firewall->num_ul_firewall_installed++] = m_pFilteringTable->rules[0].flt_rule_hdl;
	}

	if (!ipacmcfg->IsIpv6CTEnabled() &&
		(IPACM_Wan::check_dft_firewall_rules_attr_mask_ul(firewall_conf) ||
			firewall_conf->rule_action_accept))
	{
#ifndef FEATURE_VLAN_MPDN
		ul_firewall->ul_frag_installed = true;
#endif
		memset(m_pFilteringTable, 0, len);

		m_pFilteringTable->commit = 1;
		m_pFilteringTable->ep = rx_prop->rx[0].src_pipe;

		memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

		if (firewall_conf->rule_action_accept != true)
			memcpy(&flt_rule_entry.rule.attrib, &rx_prop->rx[0].attrib, sizeof(struct ipa_rule_attrib));

		m_pFilteringTable->global = false;
		m_pFilteringTable->ip = IPA_IP_v6;
		m_pFilteringTable->num_rules = (uint8_t)1;

		flt_rule_entry.at_rear = false;

		flt_rule_entry.rule.hashable = false;

		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;
		flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;

		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_FRAGMENT;
#ifdef FEATURE_VLAN_MPDN
		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_SRC_ADDR;
		flt_rule_entry.rule.attrib.u.v6.src_addr[0] = v6_prefix[0];
		flt_rule_entry.rule.attrib.u.v6.src_addr[1] = v6_prefix[1];
		flt_rule_entry.rule.attrib.u.v6.src_addr[2] = 0x0;
		flt_rule_entry.rule.attrib.u.v6.src_addr[3] = 0x0;
		flt_rule_entry.rule.attrib.u.v6.src_addr_mask[0] = 0xFFFFFFFF;
		flt_rule_entry.rule.attrib.u.v6.src_addr_mask[1] = 0xFFFFFFFF;
		flt_rule_entry.rule.attrib.u.v6.src_addr_mask[2] = 0x0;
		flt_rule_entry.rule.attrib.u.v6.src_addr_mask[3] = 0x0;
#endif
		memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
		if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
		{
			IPACMERR("Error Adding RuleTable(0) to Filtering, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else
		{
			IPACM_Iface::ipacmcfg->increaseFltRuleCount(m_pFilteringTable->ep, IPA_IP_v6, 1);
			IPACMDBG_H("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
#ifdef FEATURE_VLAN_MPDN
			ul_firewall->ul_frag_handle[ul_firewall->num_ul_frag_installed++] = m_pFilteringTable->rules[0].flt_rule_hdl;
#else
			ul_firewall->ul_frag_handle = m_pFilteringTable->rules[0].flt_rule_hdl;
#endif
		}
	}

	memset(m_pFilteringTable, 0, len);
	m_pFilteringTable->commit = 1;
	m_pFilteringTable->ep = rx_prop->rx[0].src_pipe;
	m_pFilteringTable->global = false;
	m_pFilteringTable->ip = IPA_IP_v6;
	m_pFilteringTable->num_rules = (uint8_t)1;

	for (i = 0; i < firewall_conf->num_extd_firewall_entries; i++)
	{
		if(ul_firewall->num_ul_firewall_installed >= (IPACM_MAX_FIREWALL_ENTRIES - 1))
		{
			IPACMERR("reached MAX num of UL FW rules for ep, breaking\n");
			break;
		}
		if (firewall_conf->extd_firewall_entries[i].ip_vsn == 6 &&
			firewall_conf->extd_firewall_entries[i].firewall_direction ==
			IPACM_MSGR_UL_FIREWALL)
		{
			memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
			flt_rule_entry.at_rear = false;
			flt_rule_entry.flt_rule_hdl = -1;
			flt_rule_entry.status = -1;

			if(firewall_conf->rule_action_accept == true)
			{
				flt_rule_entry.rule.action =
					IPACM_Iface::ipacmcfg->IsIpv6CTEnabled() ? IPA_PASS_TO_SRC_NAT : IPA_PASS_TO_ROUTING;
			}
			else
			{
				flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;
			}

			flt_rule_entry.rule.hashable = true;

			flt_rule_entry.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_wan_v6.hdl;
			memcpy(&flt_rule_entry.rule.attrib,
			&firewall_conf->extd_firewall_entries[i].attrib,
			sizeof(struct ipa_rule_attrib));

			flt_rule_entry.rule.attrib.attrib_mask |= rx_prop->rx[0].attrib.attrib_mask;
			flt_rule_entry.rule.attrib.meta_data_mask = rx_prop->rx[0].attrib.meta_data_mask;
			flt_rule_entry.rule.attrib.meta_data = rx_prop->rx[0].attrib.meta_data;
#ifdef FEATURE_VLAN_MPDN
			flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_SRC_ADDR;
			flt_rule_entry.rule.attrib.u.v6.src_addr[0] = v6_prefix[0];
			flt_rule_entry.rule.attrib.u.v6.src_addr[1] = v6_prefix[1];
			flt_rule_entry.rule.attrib.u.v6.src_addr[2] = 0x0;
			flt_rule_entry.rule.attrib.u.v6.src_addr[3] = 0x0;
			flt_rule_entry.rule.attrib.u.v6.src_addr_mask[0] = 0xFFFFFFFF;
			flt_rule_entry.rule.attrib.u.v6.src_addr_mask[1] = 0xFFFFFFFF;
			flt_rule_entry.rule.attrib.u.v6.src_addr_mask[2] = 0x0;
			flt_rule_entry.rule.attrib.u.v6.src_addr_mask[3] = 0x0;
#endif

			/* check if the rule is define as TCP/UDP */
			if (firewall_conf->extd_firewall_entries[i].attrib.u.v6.next_hdr == IPACM_FIREWALL_IPPROTO_TCP_UDP)
			{
				/* insert TCP rule*/
				flt_rule_entry.rule.attrib.u.v6.next_hdr = IPACM_FIREWALL_IPPROTO_TCP;
				memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
				if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
				{
					IPACMERR("Error Adding Filtering rules, aborting...\n");
					res = IPACM_FAILURE;
					goto fail;
				}
				else
				{
					IPACM_Iface::ipacmcfg->increaseFltRuleCount(m_pFilteringTable->ep, IPA_IP_v6, 1);
					/* save v4 firewall filter rule handler */
					IPACMDBG_H("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
					ul_firewall->ul_firewall_handle[ul_firewall->num_ul_firewall_installed++] = m_pFilteringTable->rules[0].flt_rule_hdl;
				}

				/* insert UDP rule*/
				flt_rule_entry.rule.attrib.u.v6.next_hdr = IPACM_FIREWALL_IPPROTO_UDP;
				memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
				if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
				{
					IPACMERR("Error Adding Filtering rules, aborting...\n");
					res = IPACM_FAILURE;
					goto fail;
				}
				else
				{
					IPACM_Iface::ipacmcfg->increaseFltRuleCount(m_pFilteringTable->ep, IPA_IP_v6, 1);
					/* save v6 firewall filter rule handler */
					IPACMDBG_H("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
					ul_firewall->ul_firewall_handle[ul_firewall->num_ul_firewall_installed++] = m_pFilteringTable->rules[0].flt_rule_hdl;
				}
			}
			else
			{
				memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
				if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
				{
					IPACMERR("Error Adding Filtering rules, aborting...\n");
					res = IPACM_FAILURE;
					goto fail;
				}
				else
				{
					IPACM_Iface::ipacmcfg->increaseFltRuleCount(m_pFilteringTable->ep, IPA_IP_v6, 1);
					/* save v6 firewall filter rule handler */
					IPACMDBG_H("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
					ul_firewall->ul_firewall_handle[ul_firewall->num_ul_firewall_installed++] = m_pFilteringTable->rules[0].flt_rule_hdl;
				}
			}
		}
	} /* end of firewall ipv6 filter rule add for loop*/
	IPACMDBG_H ("Configured and installed (%d) UL firewall rules on pipe (%d)\n ",
		ul_firewall->num_ul_firewall_installed,
		(int)m_pFilteringTable->ep);
	IPACMDBG_H ("Firewall Status (%d)\n", firewall_conf->firewall_enable);
fail:
	if(m_pFilteringTable != NULL)
	{
		free(m_pFilteringTable);
	}
	return res;
}

int IPACM_Lan::configure_v6_ul_firewall_one_profile(IPACM_firewall_conf_t* firewall_conf, bool isDefault, int vid)
{
	bool q6_firewall = false;

	if(isDefault)
	{
		/* default profile might be in STA mode */
		if(IPACM_Wan::backhaul_is_sta_mode == false && firewall_conf->rule_action_accept)
			q6_firewall = true;
		IPACMDBG("default: STA %d, action %d\n", IPACM_Wan::backhaul_is_sta_mode, firewall_conf->rule_action_accept);
	}
	else
	{
		if(firewall_conf->rule_action_accept)
			q6_firewall = true;
		IPACMDBG("non default: action %d\n", firewall_conf->rule_action_accept);
	}

	memset(IPACM_Wan::firewall_flt_rule_v6_ul,
		0, (IPACM_MAX_FIREWALL_ENTRIES + 1) * sizeof(ipa_flt_rule_add));

	if(q6_firewall) /* LTE && whitelist ?? */
	{
		IPACMDBG_H("firewall for vid %d shall be installed on Q6 side\n", vid);
		/* Configure and send the firewall filter table to Q6*/
		if(config_dft_firewall_rules_ul_ex(firewall_conf, IPACM_Wan::firewall_flt_rule_v6_ul, vid))
		{
			IPACMERR("failed configuring Q6 firewall for vid %d\n", vid);
			return IPACM_FAILURE;
		}

		/* send fragments to exception since Q6 FW doesn't handle fragments */
		config_wan_frag_firewall_rule_ul_ex(&iface_ul_firewall, vid);
		IPACMDBG_H("New config rules sent to Q6\n");
	}
	else
	{
		IPACMDBG_H("firewall for vid %d shall be installed on %s prod pipe\n", vid, dev_name);
		/* Config and install it on pipes directly, since it is Blacklisted */
		IPACMDBG_H("Send indication to Q6 to disable UL firewall\n");
		disable_dft_firewall_rules_ul_ex(vid);

		config_dft_firewall_rules_ul(firewall_conf, &iface_ul_firewall, vid);
	}

	IPACMDBG_H("finished configuring UL FW for vid %d on %s, is_default %d\n", vid, q6_firewall?"Q6":"LAN prod", isDefault);
	return IPACM_SUCCESS;
}

/*
 * configure IPv6 UL firewall for all PDNs relevant for this LAN from scratch.
 * rules are installed either on this LAN prod pipe or on Q6 routing table
 * depends on the specific PDN configuration.
 */
void IPACM_Lan::configure_v6_ul_firewall(void)
{
	IPACM_firewall_conf_t *firewall_config;
	int default_vid = 0;

	/* first of all clear LAN pipe frag, catch all and FW rules if installed */
	delete_uplink_filter_rule_ul(&iface_ul_firewall);

	/* now read XML and rebuild FW for all PDNs */
	if(IPACM_Wan::read_firewall_filter_rules_ul())
	{
		IPACMERR("failed configuring UL firewall\n");
		return;
	}

	if(IPACM_Wan::isWanUP_V6(ipa_if_num))
	{
		firewall_config = IPACM_Wan::get_default_profile_firewall_conf_ul(&default_vid);
		if(!firewall_config)
		{
			IPACMERR("failed getting default profile config\n");
			return;
		}
		if(firewall_config->firewall_enable)
		{
			if(configure_v6_ul_firewall_one_profile(firewall_config, true, default_vid))
			{
				IPACMERR("failed configuring default profile UL firewall, vid %d\n", default_vid);
			}
		}
		else
		{
			IPACMDBG_H("default profile firewall is disabled, disable Q6 firewall\n");
			disable_dft_firewall_rules_ul_ex(default_vid);
		}
	}
#ifdef FEATURE_VLAN_MPDN
	uint8_t Ids[IPA_MAX_NUM_HW_PDNS];

	if(IPACM_Iface::ipacmcfg->get_iface_vlan_ids(dev_name, Ids))
	{
		IPACMERR("failed getting vlan ids for iface %s\n", dev_name);
		return;
	}

	for(int i = 0; i < IPA_MAX_NUM_HW_PDNS; i++)
	{
		if(Ids[i] != 0)
		{
			if(Ids[i] == default_vid)
			{
				IPACMDBG_H("already handled default pdn, skip...\n");
				continue;
			}

			firewall_config = IPACM_Wan::get_firewall_conf_by_vid_ul(Ids[i]);
			if(!firewall_config)
			{
				IPACMDBG_H("no v6 vlan up PDN for Id %d\n", Ids[i]);
				continue;
			}
			if(firewall_config->firewall_enable)
			{
				if(configure_v6_ul_firewall_one_profile(firewall_config, false, Ids[i]))
				{
					IPACMERR("failed configuring default profile UL firewall, vid %d\n", Ids[i]);
				}
			}
			else
			{
				IPACMDBG_H("firewall is disabled for VID %d, disable Q6 firewall\n",Ids[i]);
				disable_dft_firewall_rules_ul_ex(Ids[i]);
			}
		}
	}
#endif
}
#endif //FEATURE_IPACM_UL_FIREWALL
#ifdef FEATURE_IPACM_PER_CLIENT_STATS
/* install UL filter rule from Q6 per client */
int IPACM_Lan::install_uplink_filter_rule_per_client
(
	ipacm_ext_prop* prop,
	ipa_ip_type iptype,
	uint8_t xlat_mux_id,
	uint8_t *mac_addr
)
{
	ipa_flt_rule_add flt_rule_entry;
	int len = 0, cnt, ret = IPACM_SUCCESS;
	ipa_ioc_add_flt_rule *pFilteringTable;
	int fd;
	int i, index = 0;
	uint32_t value = 0;
	int clnt_indx;
	uint8_t num_offset_meq_128 = 0;
	struct ipa_ipfltr_mask_eq_128 *offset_meq_128 = NULL;

	IPACMDBG_H("Set modem UL flt rules\n");

	if (rx_prop == NULL)
	{
		IPACMDBG_H("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	if(prop == NULL || prop->num_ext_props <= 0)
	{
		IPACMDBG_H("No extended property.\n");
		return IPACM_SUCCESS;
	}

	clnt_indx = get_eth_client_index(mac_addr);

	if (clnt_indx == IPACM_INVALID_INDEX)
	{
		IPACMERR("eth client not found/attached \n");
		return IPACM_FAILURE;
	}

	if (get_client_memptr(eth_client, clnt_indx)->lan_stats_idx == -1)
	{
		IPACMERR("Invalid LAN Stats idx for ethernet client:%d \n", clnt_indx);
		return IPACM_FAILURE;
	}

	fd = open(IPA_DEVICE_NAME, O_RDWR);
	if (0 == fd)
	{
		IPACMERR("Failed opening %s.\n", IPA_DEVICE_NAME);
		return IPACM_FAILURE;
	}
	if (prop->num_ext_props > MAX_WAN_UL_FILTER_RULES)
	{
		IPACMERR("number of modem UL rules > MAX_WAN_UL_FILTER_RULES, aborting...\n");
		close(fd);
		return IPACM_FAILURE;
	}

	len = sizeof(struct ipa_ioc_add_flt_rule) + prop->num_ext_props * sizeof(struct ipa_flt_rule_add);
	pFilteringTable = (struct ipa_ioc_add_flt_rule*)malloc(len);
	if (pFilteringTable == NULL)
	{
		IPACMERR("Error Locate ipa_flt_rule_add memory...\n");
		close(fd);
		return IPACM_FAILURE;
	}
	memset(pFilteringTable, 0, len);

	pFilteringTable->commit = 1;
	pFilteringTable->ep = rx_prop->rx[0].src_pipe;
	pFilteringTable->global = false;
	pFilteringTable->ip = iptype;
	pFilteringTable->num_rules = prop->num_ext_props;

	memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add)); // Zero All Fields
	flt_rule_entry.at_rear = 1;
	if (flt_rule_entry.rule.eq_attrib.ipv4_frag_eq_present)
		flt_rule_entry.at_rear = 0;
	flt_rule_entry.flt_rule_hdl = -1;
	flt_rule_entry.status = -1;

	flt_rule_entry.rule.retain_hdr = 0;
	flt_rule_entry.rule.to_uc = 0;
	flt_rule_entry.rule.eq_attrib_type = 1;
	if(iptype == IPA_IP_v4)
	{
		if (ipa_if_cate == ODU_IF && IPACM_Wan::isWan_Bridge_Mode())
		{
			IPACMDBG_H("WAN, ODU are in bridge mode \n");
			flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
		}
		else
		{
			flt_rule_entry.rule.action = IPA_PASS_TO_SRC_NAT;

			/* NAT block will set the proper MUX ID in the metadata according to the relevant PDN */
			if(IPACM_Iface::ipacmcfg->GetIPAVer() >= IPA_HW_v4_0)
				flt_rule_entry.rule.set_metadata = true;
		}
	}
	else if(iptype == IPA_IP_v6)
	{
		flt_rule_entry.rule.action = IPACM_Iface::ipacmcfg->IsIpv6CTEnabled()?
				IPA_PASS_TO_SRC_NAT : IPA_PASS_TO_ROUTING;
	}
	else
	{
		IPACMERR("IP type is not expected.\n");
		ret = IPACM_FAILURE;
		goto fail;
	}

	for(cnt=0; cnt<prop->num_ext_props; cnt++)
	{
		memcpy(&flt_rule_entry.rule.eq_attrib,
					 &prop->prop[cnt].eq_attrib,
					 sizeof(prop->prop[cnt].eq_attrib));

		/* Check if we can add the MAC address rule. */
		if (flt_rule_entry.rule.eq_attrib.num_offset_meq_128 == IPA_IPFLTR_NUM_MEQ_128_EQNS)
		{
			IPACMERR("128 bit equations not available.\n");
			ret = IPACM_FAILURE;
			goto fail;
		}
		num_offset_meq_128 = flt_rule_entry.rule.eq_attrib.num_offset_meq_128;
		offset_meq_128 = &flt_rule_entry.rule.eq_attrib.offset_meq_128[num_offset_meq_128];
		if(rx_prop->rx[0].hdr_l2_type == IPA_HDR_L2_ETHERNET_II)
		{
			offset_meq_128->offset = -8;
		}
		else
		{
			offset_meq_128->offset = -16;
		}

		for (i = 0; i < 10; i++)
		{
			offset_meq_128->mask[i] = 0;
			offset_meq_128->value[i] = 0;
		}

		memset(&offset_meq_128->mask[10], 0xFF, ETH_ALEN);

		for ( i = 0; i < ETH_ALEN; i++)
			offset_meq_128->value[10+i] = mac_addr[ETH_ALEN-(i+1)];

		if (num_offset_meq_128 == 0)
			flt_rule_entry.rule.eq_attrib.rule_eq_bitmap |= (1<<3);
		else
			flt_rule_entry.rule.eq_attrib.rule_eq_bitmap |= (1<<4);

		flt_rule_entry.rule.eq_attrib.num_offset_meq_128++;


		flt_rule_entry.rule.rt_tbl_idx = prop->prop[cnt].rt_tbl_idx;

		/* Handle XLAT configuration */
		if ((iptype == IPA_IP_v4) && prop->prop[cnt].is_xlat_rule && (xlat_mux_id != 0))
		{
			/* fill the value of meta-data */
			value = xlat_mux_id;
			flt_rule_entry.rule.eq_attrib.metadata_meq32_present = 1;
			flt_rule_entry.rule.eq_attrib.metadata_meq32.offset = 0;
			flt_rule_entry.rule.eq_attrib.metadata_meq32.value = (value & 0xFF) << 16;
			flt_rule_entry.rule.eq_attrib.metadata_meq32.mask = 0x00FF0000;
			IPACMDBG_H("xlat meta-data is modified for rule: %d has rule_id %d with xlat_mux_id: %d\n",
					cnt, prop->prop[cnt].rule_id, xlat_mux_id);
		}
		IPACMDBG_H("rule: %d has rule_id %d\n",
				cnt, prop->prop[cnt].rule_id);
		flt_rule_entry.rule.hashable = prop->prop[cnt].is_rule_hashable;
		flt_rule_entry.rule.rule_id = (prop->prop[cnt].rule_id & 0x1F) |
			(get_client_memptr(eth_client, clnt_indx)->lan_stats_idx << 5) | 0x200;
		IPACMDBG_H("Modified rule: %d has rule_id %d\n",
				cnt, flt_rule_entry.rule.rule_id);
		if(rx_prop->rx[0].attrib.attrib_mask & IPA_FLT_META_DATA)	//turn on meta-data equation
		{
			flt_rule_entry.rule.eq_attrib.rule_eq_bitmap |= (1<<9);
			flt_rule_entry.rule.eq_attrib.metadata_meq32_present = 1;
			flt_rule_entry.rule.eq_attrib.metadata_meq32.offset = 0;
			flt_rule_entry.rule.eq_attrib.metadata_meq32.value |= rx_prop->rx[0].attrib.meta_data;
			flt_rule_entry.rule.eq_attrib.metadata_meq32.mask |= rx_prop->rx[0].attrib.meta_data_mask;
		}
		memcpy(&pFilteringTable->rules[cnt], &flt_rule_entry, sizeof(flt_rule_entry));

		IPACMDBG_H("Modem UL filtering rule %d has rule_id %d\n", cnt, prop->prop[cnt].rule_id);
		index++;
	}

	if(false == m_filtering.AddFilteringRule(pFilteringTable))
	{
		IPACMERR("Error Adding RuleTable to Filtering, aborting...\n");
		ret = IPACM_FAILURE;
		goto fail;
	}
	else
	{
		if(iptype == IPA_IP_v4)
		{
			for(i=0; i < pFilteringTable->num_rules; i++)
			{
				get_client_memptr(eth_client, clnt_indx)->wan_ul_fl_rule_hdl_v4[i] = pFilteringTable->rules[i].flt_rule_hdl;
			}
			get_client_memptr(eth_client, clnt_indx)->ipv4_ul_rules_set = true;
		}
		else if(iptype == IPA_IP_v6)
		{
			for(i=0; i < pFilteringTable->num_rules; i++)
			{
				get_client_memptr(eth_client, clnt_indx)->wan_ul_fl_rule_hdl_v6[i] = pFilteringTable->rules[i].flt_rule_hdl;
			}
			get_client_memptr(eth_client, clnt_indx)->ipv6_ul_rules_set = true;
		}
		else
		{
			IPACMERR("IP type is not expected.\n");
			goto fail;
		}
	}

fail:
	free(pFilteringTable);
	close(fd);
	return ret;
}

/* install UL filter rule from Q6 for all clients */
int IPACM_Lan::install_uplink_filter_rule
(
	ipacm_ext_prop* prop,
	ipa_ip_type iptype,
	uint8_t xlat_mux_id
)
{
	int ret = IPACM_SUCCESS, i=0;

	for (i = 0; i < num_eth_client; i++)
		{
			if (iptype == IPA_IP_v4)
			{
				if (get_client_memptr(eth_client, i)->ipv4_ul_rules_set == false)
				{
					ret = install_uplink_filter_rule_per_client(prop, iptype, xlat_mux_id, get_client_memptr(eth_client, i)->mac);
					IPACMDBG_H("IPA_IP_v4 xlat_mux_id: %d, modem_ul_v4_set %d\n", xlat_mux_id, get_client_memptr(eth_client, i)->ipv4_ul_rules_set);
				}
			}
			else if (iptype == IPA_IP_v6)
			{
				if (num_dft_rt_v6 ==1 && get_client_memptr(eth_client, i)->ipv6_ul_rules_set == false)
				{
					ret = install_uplink_filter_rule_per_client(prop, iptype, xlat_mux_id, get_client_memptr(eth_client, i)->mac);
					IPACMDBG_H("IPA_IP_v6 num_dft_rt_v6 %d xlat_mux_id: %d modem_ul_v6_set: %d\n", num_dft_rt_v6, xlat_mux_id, get_client_memptr(eth_client, i)->ipv6_ul_rules_set);
				}
			} else {
				IPACMDBG_H("ip-type: %d modem_ul_v4_set: %d, modem_ul_v6_set %d\n",
					iptype, get_client_memptr(eth_client, i)->ipv4_ul_rules_set, get_client_memptr(eth_client, i)->ipv6_ul_rules_set);

			}
		} /* end of for loop */

	return ret;
}

/* Delete UL filter rule from Q6 per client */
int IPACM_Lan::delete_uplink_filter_rule_per_client
(
	ipa_ip_type iptype,
	uint8_t *mac_addr
)
{
	int fd;
	int clnt_indx;

	fd = open(IPA_DEVICE_NAME, O_RDWR);
	if (0 == fd)
	{
		IPACMERR("Failed opening %s.\n", IPA_DEVICE_NAME);
		return IPACM_FAILURE;
	}

	clnt_indx = get_eth_client_index(mac_addr);

	if (clnt_indx == IPACM_INVALID_INDEX)
	{
		IPACMERR("eth client not found/attached \n");
		return IPACM_FAILURE;
	}

	if (get_client_memptr(eth_client, clnt_indx)->lan_stats_idx == -1)
	{
		IPACMERR("Invalid LAN Stats idx for ethernet client:%d \n", clnt_indx);
		return IPACM_FAILURE;
	}

	if (((iptype == IPA_IP_v4) && num_wan_ul_fl_rule_v4 > MAX_WAN_UL_FILTER_RULES) ||
		((iptype == IPA_IP_v6) && num_wan_ul_fl_rule_v6 > MAX_WAN_UL_FILTER_RULES))
	{
		IPACMERR("number of wan_ul_fl_rule_v4 (%d)/wan_ul_fl_rule_v6 (%d) > MAX_WAN_UL_FILTER_RULES (%d), aborting...\n",
			num_wan_ul_fl_rule_v4,
			num_wan_ul_fl_rule_v6,
			MAX_WAN_UL_FILTER_RULES);
		return IPACM_FAILURE;
	}

	if ((iptype == IPA_IP_v4) && get_client_memptr(eth_client, clnt_indx)->ipv4_ul_rules_set)
	{
		IPACMDBG_H("Del (%d) num of v4 UL rules for cliend idx:%d\n", num_wan_ul_fl_rule_v4, clnt_indx);
		if (m_filtering.DeleteFilteringHdls(get_client_memptr(eth_client, clnt_indx)->wan_ul_fl_rule_hdl_v4,
				iptype, num_wan_ul_fl_rule_v4) == false)
		{
			IPACMERR("Error Deleting RuleTable(1) to Filtering, aborting...\n");
			close(fd);
			return IPACM_FAILURE;
		}
		memset(get_client_memptr(eth_client, clnt_indx)->wan_ul_fl_rule_hdl_v4, 0, MAX_WAN_UL_FILTER_RULES * sizeof(uint32_t));
		get_client_memptr(eth_client, clnt_indx)->ipv4_ul_rules_set = false;
	}

	if ((iptype == IPA_IP_v6) && get_client_memptr(eth_client, clnt_indx)->ipv6_ul_rules_set)
	{
		IPACMDBG_H("Del (%d) num of v4 UL rules for cliend idx:%d\n", num_wan_ul_fl_rule_v4, clnt_indx);
		if (m_filtering.DeleteFilteringHdls(get_client_memptr(eth_client, clnt_indx)->wan_ul_fl_rule_hdl_v6,
				iptype, num_wan_ul_fl_rule_v6) == false)
		{
			IPACMERR("Error Deleting RuleTable(1) to Filtering, aborting...\n");
			close(fd);
			return IPACM_FAILURE;
		}
		memset(get_client_memptr(eth_client, clnt_indx)->wan_ul_fl_rule_hdl_v6, 0, MAX_WAN_UL_FILTER_RULES * sizeof(uint32_t));
		get_client_memptr(eth_client, clnt_indx)->ipv6_ul_rules_set = false;
	}
	return IPACM_SUCCESS;
}

/* Delete UL filter rule from Q6 for all clients */
int IPACM_Lan::delete_uplink_filter_rule
(
	ipa_ip_type iptype
)
{
	int ret = IPACM_SUCCESS, i=0;

	for (i = 0; i < num_eth_client; i++)
	{
		if (iptype == IPA_IP_v4)
		{
			if (get_client_memptr(eth_client, i)->ipv4_ul_rules_set == true)
			{
				IPACMDBG_H("IPA_IP_v4 Client id: %d, modem_ul_v4_set %d\n", i, get_client_memptr(eth_client, i)->ipv4_ul_rules_set);
				ret = delete_uplink_filter_rule_per_client(iptype, get_client_memptr(eth_client, i)->mac);
			}
		}
		else if (iptype == IPA_IP_v6)
		{
			if (get_client_memptr(eth_client, i)->ipv6_ul_rules_set == true)
			{
				IPACMDBG_H("IPA_IP_v6 Cliend id: %d modem_ul_v6_set: %d\n", i, get_client_memptr(eth_client, i)->ipv6_ul_rules_set);
				ret = delete_uplink_filter_rule_per_client(iptype, get_client_memptr(eth_client, i)->mac);
			}
		} else {
			ret = IPACM_FAILURE;
			IPACMDBG_H("ip-type: %d lan_stats_idx: %d modem_ul_v4_set: %d, modem_ul_v6_set %d\n",
				iptype, get_client_memptr(eth_client, i)->lan_stats_idx, get_client_memptr(eth_client, i)->ipv4_ul_rules_set, get_client_memptr(eth_client, i)->ipv6_ul_rules_set);
		}
	} /* end of for loop */

	return ret;
}

/* Set lan client info. */
int IPACM_Lan::set_lan_client_info(struct wan_ioctl_lan_client_info *client_info)
{
	int ret = IPACM_SUCCESS;
	int fd_wwan_ioctl;

	if (client_info == NULL)
	{
		IPACMERR("Client info NULL.\n");
		return IPACM_FAILURE;
	}

	IPACMDBG_H("Client MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					 client_info->mac[0], client_info->mac[1], client_info->mac[2],
					 client_info->mac[3], client_info->mac[4], client_info->mac[5]);

	fd_wwan_ioctl = open(WWAN_QMI_IOCTL_DEVICE_NAME, O_RDWR);

	if(fd_wwan_ioctl < 0)
	{
		IPACMERR("Failed to open %s.\n",WWAN_QMI_IOCTL_DEVICE_NAME);
		return IPACM_FAILURE;
	}

	ret = ioctl(fd_wwan_ioctl, WAN_IOC_SET_LAN_CLIENT_INFO, client_info);
	if (ret != 0)
	{
		IPACMERR("Failed to set client info %p\n ", client_info);
	}
	IPACMDBG("Set Client info: %p\n", client_info);
	close(fd_wwan_ioctl);
	return ret;
}

/* Clear lan client info. */
int IPACM_Lan::clear_lan_client_info(struct wan_ioctl_lan_client_info *client_info)
{
	int ret = IPACM_SUCCESS;
	int fd_wwan_ioctl;

	if (client_info == NULL)
	{
		IPACMERR("Client info NULL.\n");
		return IPACM_FAILURE;
	}

	IPACMDBG_H("Client MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					 client_info->mac[0], client_info->mac[1], client_info->mac[2],
					 client_info->mac[3], client_info->mac[4], client_info->mac[5]);

	fd_wwan_ioctl = open(WWAN_QMI_IOCTL_DEVICE_NAME, O_RDWR);

	if(fd_wwan_ioctl < 0)
	{
		IPACMERR("Failed to open %s.\n",WWAN_QMI_IOCTL_DEVICE_NAME);
		return IPACM_FAILURE;
	}

	ret = ioctl(fd_wwan_ioctl, WAN_IOC_CLEAR_LAN_CLIENT_INFO, client_info);
	if (ret != 0)
	{
		IPACMERR("Failed to set client info %p\n ", client_info);
	}
	IPACMDBG("Set Client info: %p\n", client_info);
	close(fd_wwan_ioctl);
	return ret;
}

/* Enable per client stats. */
int IPACM_Lan::enable_per_client_stats(bool *status)
{
	int ret = IPACM_SUCCESS;
	int fd_wwan_ioctl;

	if (status == NULL)
	{
		IPACMERR("Status is NULL.\n");
		return IPACM_FAILURE;
	}

	fd_wwan_ioctl = open(WWAN_QMI_IOCTL_DEVICE_NAME, O_RDWR);

	if(fd_wwan_ioctl < 0)
	{
		IPACMERR("Failed to open %s.\n",WWAN_QMI_IOCTL_DEVICE_NAME);
		return IPACM_FAILURE;
	}

	ret = ioctl(fd_wwan_ioctl, WAN_IOC_ENABLE_PER_CLIENT_STATS, status);
	if (ret != 0)
	{
		IPACMERR("Failed to enable per client stats %p\n ", status);
	}
	IPACMDBG("Enabled per client stats: %p\n", status);
	close(fd_wwan_ioctl);
	return ret;
}
#endif
int IPACM_Lan::handle_wan_down_v6(bool is_sta_mode)
{

	if (rx_prop == NULL)
	{
		IPACMERR("Rx prop is NULL, return\n");
		return IPACM_SUCCESS;
	}

#ifdef FEATURE_VLAN_MPDN
	/* prefixes list updated, install rules accordingly */
	modify_ipv6_prefix_flt_rule();
#else
	delete_ipv6_prefix_flt_rule();
#endif
	memset(ipv6_prefix, 0, sizeof(ipv6_prefix));

	if(is_sta_mode == false)
	{
		if(del_ul_flt_rules(IPA_IP_v6))
			return IPACM_FAILURE;

		if(notify_flt_removed(IPACM_Iface::ipacmcfg->GetQmapId()))
			return IPACM_FAILURE;
	}
	else
	{
		if (!m_filtering.DeleteFilteringHdls(&dft_v6fl_rule_hdl[m_ipv6_default_filterting_rules_count], IPA_IP_v6, 1))
		{
			IPACMERR("Error Deleting last default flt rule, aborting...\n");
			return IPACM_FAILURE;
		}
		IPACM_Iface::ipacmcfg->decreaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, 1);
	}
	return IPACM_SUCCESS;
}

int IPACM_Lan::reset_to_dummy_flt_rule(ipa_ip_type iptype, uint32_t rule_hdl)
{
	int len, res = IPACM_SUCCESS;
	struct ipa_flt_rule_mdfy flt_rule;
	struct ipa_ioc_mdfy_flt_rule* pFilteringTable;

	IPACMDBG_H("Reset flt rule to dummy, IP type: %d, hdl: %d\n", iptype, rule_hdl);
	len = sizeof(struct ipa_ioc_mdfy_flt_rule) + sizeof(struct ipa_flt_rule_mdfy);
	pFilteringTable = (struct ipa_ioc_mdfy_flt_rule*)malloc(len);

	if (pFilteringTable == NULL)
	{
		IPACMERR("Error allocate flt rule memory...\n");
		return IPACM_FAILURE;
	}
	memset(pFilteringTable, 0, len);

	pFilteringTable->commit = 1;
	pFilteringTable->ip = iptype;
	pFilteringTable->num_rules = 1;

	memset(&flt_rule, 0, sizeof(struct ipa_flt_rule_mdfy));
	flt_rule.status = -1;
	flt_rule.rule_hdl = rule_hdl;

	flt_rule.rule.retain_hdr = 0;
	flt_rule.rule.action = IPA_PASS_TO_EXCEPTION;

	if(iptype == IPA_IP_v4)
	{
		IPACMDBG_H("Reset IPv4 flt rule to dummy\n");

		flt_rule.rule.attrib.attrib_mask = IPA_FLT_SRC_ADDR | IPA_FLT_DST_ADDR;
		flt_rule.rule.attrib.u.v4.dst_addr = ~0;
		flt_rule.rule.attrib.u.v4.dst_addr_mask = ~0;
		flt_rule.rule.attrib.u.v4.src_addr = ~0;
		flt_rule.rule.attrib.u.v4.src_addr_mask = ~0;

		memcpy(&(pFilteringTable->rules[0]), &flt_rule, sizeof(struct ipa_flt_rule_mdfy));
		if (false == m_filtering.ModifyFilteringRule(pFilteringTable))
		{
			IPACMERR("Error modifying filtering rule.\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else
		{
			IPACMDBG_H("Flt rule reset to dummy, hdl: 0x%x, status: %d\n", pFilteringTable->rules[0].rule_hdl,
						pFilteringTable->rules[0].status);
		}
	}
	else if(iptype == IPA_IP_v6)
	{
		IPACMDBG_H("Reset IPv6 flt rule to dummy\n");

		flt_rule.rule.attrib.attrib_mask = IPA_FLT_SRC_ADDR | IPA_FLT_DST_ADDR;
		flt_rule.rule.attrib.u.v6.src_addr[0] = ~0;
		flt_rule.rule.attrib.u.v6.src_addr[1] = ~0;
		flt_rule.rule.attrib.u.v6.src_addr[2] = ~0;
		flt_rule.rule.attrib.u.v6.src_addr[3] = ~0;
		flt_rule.rule.attrib.u.v6.src_addr_mask[0] = ~0;
		flt_rule.rule.attrib.u.v6.src_addr_mask[1] = ~0;
		flt_rule.rule.attrib.u.v6.src_addr_mask[2] = ~0;
		flt_rule.rule.attrib.u.v6.src_addr_mask[3] = ~0;
		flt_rule.rule.attrib.u.v6.dst_addr[0] = ~0;
		flt_rule.rule.attrib.u.v6.dst_addr[1] = ~0;
		flt_rule.rule.attrib.u.v6.dst_addr[2] = ~0;
		flt_rule.rule.attrib.u.v6.dst_addr[3] = ~0;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[0] = ~0;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[1] = ~0;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[2] = ~0;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[3] = ~0;


		memcpy(&(pFilteringTable->rules[0]), &flt_rule, sizeof(struct ipa_flt_rule_mdfy));
		if (false == m_filtering.ModifyFilteringRule(pFilteringTable))
		{
			IPACMERR("Error modifying filtering rule.\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else
		{
			IPACMDBG_H("Flt rule reset to dummy, hdl: 0x%x, status: %d\n", pFilteringTable->rules[0].rule_hdl,
						pFilteringTable->rules[0].status);
		}
	}
	else
	{
		IPACMERR("IP type is not expected.\n");
		res = IPACM_FAILURE;
		goto fail;
	}

fail:
	free(pFilteringTable);
	return res;
}

void IPACM_Lan::post_del_self_evt()
{
	ipacm_cmd_q_data evt;
	ipacm_event_data_fid* fid;
	fid = (ipacm_event_data_fid*)malloc(sizeof(ipacm_event_data_fid));
	if(fid == NULL)
	{
		IPACMERR("Failed to allocate fid memory.\n");
		return;
	}
	memset(fid, 0, sizeof(ipacm_event_data_fid));
	memset(&evt, 0, sizeof(ipacm_cmd_q_data));

	fid->if_index = ipa_if_num;

	evt.evt_data = (void*)fid;
	evt.event = IPA_LAN_DELETE_SELF;

	IPACMDBG_H("Posting event IPA_LAN_DELETE_SELF\n");
	IPACM_EvtDispatcher::PostEvt(&evt);
}

/*handle reset usb-client rt-rules */
int IPACM_Lan::handle_lan_client_reset_rt(ipa_ip_type iptype)
{
	int i, res = IPACM_SUCCESS;

	/* clean eth-client routing rules */
	IPACMDBG_H("left %d eth clients need to be deleted \n ", num_eth_client);
	for (i = 0; i < num_eth_client; i++)
	{
		res = delete_eth_rtrules(i, iptype);
		if (res != IPACM_SUCCESS)
		{
			IPACMERR("Failed to delete old iptype(%d) rules.\n", iptype);
			return res;
		}
	} /* end of for loop */

	/* Reset ip-address */
	for (i = 0; i < num_eth_client; i++)
	{
		if(iptype == IPA_IP_v4)
		{
			get_client_memptr(eth_client, i)->ipv4_set = false;
		}
		else
		{
			get_client_memptr(eth_client, i)->ipv6_set = 0;
		}
	} /* end of for loop */
	return res;
}

int IPACM_Lan::install_ipv4_icmp_flt_rule()
{
	int len;
	struct ipa_ioc_add_flt_rule* flt_rule;
	struct ipa_flt_rule_add flt_rule_entry;

	if(rx_prop != NULL)
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
		flt_rule->ip = IPA_IP_v4;
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

		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_PROTOCOL;
		flt_rule_entry.rule.attrib.u.v4.protocol = (uint8_t)IPACM_FIREWALL_IPPROTO_ICMP;
		memcpy(&(flt_rule->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

		if (m_filtering.AddFilteringRule(flt_rule) == false)
		{
			IPACMERR("Error Adding Filtering rule, aborting...\n");
			free(flt_rule);
			return IPACM_FAILURE;
		}
		else
		{
			IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v4, 1);
			ipv4_icmp_flt_rule_hdl[0] = flt_rule->rules[0].flt_rule_hdl;
			IPACMDBG_H("IPv4 icmp filter rule HDL:0x%x\n", ipv4_icmp_flt_rule_hdl[0]);
                        free(flt_rule);
		}
	}
	return IPACM_SUCCESS;
}

int IPACM_Lan::install_ipv6_icmp_flt_rule()
{

	int len;
	struct ipa_ioc_add_flt_rule* flt_rule;
	struct ipa_flt_rule_add flt_rule_entry;

	if(rx_prop != NULL)
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
		flt_rule_entry.rule.hashable = false;
#endif
		memcpy(&flt_rule_entry.rule.attrib, &rx_prop->rx[0].attrib, sizeof(flt_rule_entry.rule.attrib));
		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_NEXT_HDR;
		flt_rule_entry.rule.attrib.u.v6.next_hdr = (uint8_t)IPACM_FIREWALL_IPPROTO_ICMP6;
		memcpy(&(flt_rule->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

		if (m_filtering.AddFilteringRule(flt_rule) == false)
		{
			IPACMERR("Error Adding Filtering rule, aborting...\n");
			free(flt_rule);
			return IPACM_FAILURE;
		}
		else
		{
			IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, 1);
			ipv6_icmp_flt_rule_hdl[0] = flt_rule->rules[0].flt_rule_hdl;
			IPACMDBG_H("IPv6 icmp filter rule HDL:0x%x\n", ipv6_icmp_flt_rule_hdl[0]);
			free(flt_rule);
		}
	}
	return IPACM_SUCCESS;
}

#ifdef FEATURE_L2TP_E2E
int IPACM_Lan::install_l2tp_inner_private_subnet_flt_rule()
{
	int i;
	ipa_ioc_add_flt_rule *m_pFilteringTable;
	ipa_flt_rule_add *flt_rule_entry;

	if(rx_prop == NULL)
	{
		IPACMDBG_H("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)
		 calloc(1, sizeof(struct ipa_ioc_add_flt_rule) +
			(IPACM_Iface::ipacmcfg->ipa_num_private_subnet) * sizeof(struct ipa_flt_rule_add));
	if(!m_pFilteringTable)
	{
		IPACMERR("Failed to allocate memory.\n");
		return IPACM_FAILURE;
	}
	m_pFilteringTable->commit = 1;
	m_pFilteringTable->ep = rx_prop->rx[0].src_pipe;
	m_pFilteringTable->global = false;
	m_pFilteringTable->ip = IPA_IP_v6;
	m_pFilteringTable->num_rules = (uint8_t)IPACM_Iface::ipacmcfg->ipa_num_private_subnet;

	for(i = 0; i < (IPACM_Iface::ipacmcfg->ipa_num_private_subnet); i++)
	{
		flt_rule_entry = &m_pFilteringTable->rules[i];
		flt_rule_entry->at_rear = true;
		flt_rule_entry->rule.retain_hdr = 1;
		flt_rule_entry->flt_rule_hdl = -1;
		flt_rule_entry->status = -1;
		flt_rule_entry->rule.action = IPA_PASS_TO_EXCEPTION;

		memcpy(&flt_rule_entry->rule.attrib, &rx_prop->rx[1].attrib,
			sizeof(flt_rule_entry->rule.attrib));
		flt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_NEXT_HDR;
		flt_rule_entry->rule.attrib.u.v6.next_hdr = 0x73;
		flt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_L2TP_INNER_IP_TYPE;
		flt_rule_entry->rule.attrib.type = 0x40;
		flt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_L2TP_INNER_IPV4_DST_ADDR;
		flt_rule_entry->rule.attrib.u.v4.dst_addr_mask = IPACM_Iface::ipacmcfg->private_subnet_table[i].subnet_mask;
		flt_rule_entry->rule.attrib.u.v4.dst_addr = IPACM_Iface::ipacmcfg->private_subnet_table[i].subnet_addr;
	}

	if(false == m_filtering.AddFilteringRule(m_pFilteringTable))
	{
		IPACMERR("Error Adding Filtering, aborting...\n");
		free(m_pFilteringTable);
		return IPACM_FAILURE;
	}
	IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, IPACM_Iface::ipacmcfg->ipa_num_private_subnet);

	/* copy filter rule hdls */
	for (i = 0; i < IPACM_Iface::ipacmcfg->ipa_num_private_subnet; i++)
	{
		l2tp_inner_private_subnet_flt_rule_hdl[i] = m_pFilteringTable->rules[i].flt_rule_hdl;
	}
	free(m_pFilteringTable);
	return IPACM_SUCCESS;
}
#endif

int IPACM_Lan::add_dummy_private_subnet_flt_rule(ipa_ip_type iptype)
{
	if(rx_prop == NULL)
	{
		IPACMDBG_H("There is no rx_prop for iface %s, not able to add dummy private subnet filtering rule.\n", dev_name);
		return 0;
	}

	if(iptype == IPA_IP_v6)
	{
		IPACMDBG_H("There is no ipv6 dummy filter rules needed for iface %s\n", dev_name);
		return 0;
	}
	int i, len, res = IPACM_SUCCESS;
	struct ipa_flt_rule_add flt_rule;
	ipa_ioc_add_flt_rule* pFilteringTable;

	len = sizeof(struct ipa_ioc_add_flt_rule) +	IPA_MAX_PRIVATE_SUBNET_ENTRIES * sizeof(struct ipa_flt_rule_add);

	pFilteringTable = (struct ipa_ioc_add_flt_rule *)malloc(len);
	if (pFilteringTable == NULL)
	{
		IPACMERR("Error allocate flt table memory...\n");
		return IPACM_FAILURE;
	}
	memset(pFilteringTable, 0, len);

	pFilteringTable->commit = 1;
	pFilteringTable->ep = rx_prop->rx[0].src_pipe;
	pFilteringTable->global = false;
	pFilteringTable->ip = iptype;
	pFilteringTable->num_rules = IPA_MAX_PRIVATE_SUBNET_ENTRIES;

	memset(&flt_rule, 0, sizeof(struct ipa_flt_rule_add));

	flt_rule.rule.retain_hdr = 0;
	flt_rule.at_rear = true;
	flt_rule.flt_rule_hdl = -1;
	flt_rule.status = -1;
	flt_rule.rule.action = IPA_PASS_TO_EXCEPTION;
#ifdef FEATURE_IPA_V3
	flt_rule.rule.hashable = true;
#endif
	memcpy(&flt_rule.rule.attrib, &rx_prop->rx[0].attrib,
			sizeof(flt_rule.rule.attrib));

	if(iptype == IPA_IP_v4)
	{
		flt_rule.rule.attrib.attrib_mask = IPA_FLT_SRC_ADDR | IPA_FLT_DST_ADDR;
		flt_rule.rule.attrib.u.v4.src_addr_mask = ~0;
		flt_rule.rule.attrib.u.v4.src_addr = ~0;
		flt_rule.rule.attrib.u.v4.dst_addr_mask = ~0;
		flt_rule.rule.attrib.u.v4.dst_addr = ~0;

		for(i=0; i<IPA_MAX_PRIVATE_SUBNET_ENTRIES; i++)
		{
			memcpy(&(pFilteringTable->rules[i]), &flt_rule, sizeof(struct ipa_flt_rule_add));
		}

		if (false == m_filtering.AddFilteringRule(pFilteringTable))
		{
			IPACMERR("Error adding dummy private subnet v4 flt rule\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else
		{
			IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v4, IPA_MAX_PRIVATE_SUBNET_ENTRIES);
			/* copy filter rule hdls */
			for (int i = 0; i < IPA_MAX_PRIVATE_SUBNET_ENTRIES; i++)
			{
				if (pFilteringTable->rules[i].status == 0)
				{
					private_fl_rule_hdl[i] = pFilteringTable->rules[i].flt_rule_hdl;
					IPACMDBG_H("Private subnet v4 flt rule %d hdl:0x%x\n", i, private_fl_rule_hdl[i]);
				}
				else
				{
					IPACMERR("Failed adding lan2lan v4 flt rule %d\n", i);
					res = IPACM_FAILURE;
					goto fail;
				}
			}
		}
	}
fail:
	free(pFilteringTable);
	return res;
}

int IPACM_Lan::modify_private_subnet()
{
	int i, len, res = IPACM_SUCCESS;
	struct ipa_flt_rule_mdfy flt_rule;
	struct ipa_ioc_mdfy_flt_rule* pFilteringTable;

	for(i = 0; i < IPA_MAX_PRIVATE_SUBNET_ENTRIES; i++)
	{
		reset_to_dummy_flt_rule(IPA_IP_v4, private_fl_rule_hdl[i]);
	}

	len = sizeof(struct ipa_ioc_mdfy_flt_rule) + (IPACM_Iface::ipacmcfg->ipa_num_private_subnet) * sizeof(struct ipa_flt_rule_mdfy);
	pFilteringTable = (struct ipa_ioc_mdfy_flt_rule*)malloc(len);
	if(!pFilteringTable)
	{
		IPACMERR("Failed to allocate ipa_ioc_mdfy_flt_rule memory...\n");
		return IPACM_FAILURE;
	}
	memset(pFilteringTable, 0, len);

	pFilteringTable->commit = 1;
	pFilteringTable->ip = IPA_IP_v4;
	pFilteringTable->num_rules = (uint8_t)IPACM_Iface::ipacmcfg->ipa_num_private_subnet;

	/* Make LAN-traffic always go A5, use default IPA-RT table */
	if(false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_default_v4))
	{
		IPACMERR("Failed to get routing table handle.\n");
		res = IPACM_FAILURE;
		goto fail;
	}

	memset(&flt_rule, 0, sizeof(struct ipa_flt_rule_mdfy));
	flt_rule.status = -1;

	flt_rule.rule.retain_hdr = 1;
	flt_rule.rule.to_uc = 0;
	flt_rule.rule.action = IPA_PASS_TO_ROUTING;
	flt_rule.rule.eq_attrib_type = 0;
	flt_rule.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_default_v4.hdl;
	IPACMDBG_H("Private filter rule use table: %s\n", IPACM_Iface::ipacmcfg->rt_tbl_default_v4.name);

	memcpy(&flt_rule.rule.attrib, &rx_prop->rx[0].attrib, sizeof(flt_rule.rule.attrib));
	flt_rule.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;

	for(i = 0; i < (IPACM_Iface::ipacmcfg->ipa_num_private_subnet); i++)
	{
		flt_rule.rule_hdl = private_fl_rule_hdl[i];
		flt_rule.rule.attrib.u.v4.dst_addr_mask = IPACM_Iface::ipacmcfg->private_subnet_table[i].subnet_mask;
		flt_rule.rule.attrib.u.v4.dst_addr = IPACM_Iface::ipacmcfg->private_subnet_table[i].subnet_addr;
		memcpy(&(pFilteringTable->rules[i]), &flt_rule, sizeof(struct ipa_flt_rule_mdfy));
		IPACMDBG_H(" IPACM private subnet_addr as: 0x%x entry(%d)\n", flt_rule.rule.attrib.u.v4.dst_addr, i);
	}

	if(false == m_filtering.ModifyFilteringRule(pFilteringTable))
	{
		IPACMERR("Failed to modify private subnet filtering rules.\n");
		res = IPACM_FAILURE;
		goto fail;
	}

fail:
	if(pFilteringTable != NULL)
	{
		free(pFilteringTable);
	}
	return res;
}

int IPACM_Lan::handle_private_subnet_android(ipa_ip_type iptype)
{
	if (rx_prop == NULL)
	{
		IPACMDBG_H("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	if(iptype == IPA_IP_v6)
	{
		IPACMDBG_H("There is no ipv6 dummy filter rules needed for iface %s\n", dev_name);
		return 0;
	}
	else
	{
		return modify_private_subnet();
	}

	return IPACM_FAILURE;
}

#ifdef FEATURE_VLAN_MPDN
int IPACM_Lan::add_dummy_ipv6_prefix_flt_rule()
{
	int i, len, res = IPACM_SUCCESS;
	struct ipa_flt_rule_add flt_rule;
	ipa_ioc_add_flt_rule* pFilteringTable;

	if(rx_prop == NULL)
	{
		IPACMDBG_H("There is no rx_prop for iface %s, not able to add dummy ipv6 prefix filtering rule.\n", dev_name);
		return IPACM_SUCCESS;
	}

	if(dummy_prefix_installed)
	{
		IPACMDBG_H("dummy rules already installed\n");
		return IPACM_SUCCESS;
	}

	len = sizeof(struct ipa_ioc_add_flt_rule) + IPA_MAX_IPV6_PREFIX_FLT_RULE  * sizeof(struct ipa_flt_rule_add);

	pFilteringTable = (struct ipa_ioc_add_flt_rule *)malloc(len);
	if(pFilteringTable == NULL)
	{
		IPACMERR("Error allocate flt table memory...\n");
		return IPACM_FAILURE;
	}
	memset(pFilteringTable, 0, len);

	pFilteringTable->commit = 1;
	pFilteringTable->ep = rx_prop->rx[0].src_pipe;
	pFilteringTable->global = false;
	pFilteringTable->ip = IPA_IP_v6;
	pFilteringTable->num_rules = IPA_MAX_IPV6_PREFIX_FLT_RULE;

	memset(&flt_rule, 0, sizeof(struct ipa_flt_rule_add));

	flt_rule.rule.retain_hdr = 0;
	flt_rule.rule.to_uc = 0;
	flt_rule.rule.eq_attrib_type = 0;
	flt_rule.at_rear = true;
	flt_rule.flt_rule_hdl = -1;
	flt_rule.status = -1;
	flt_rule.rule.action = IPA_PASS_TO_EXCEPTION;
#ifdef FEATURE_IPA_V3
	flt_rule.rule.hashable = true;
#endif
	memcpy(&flt_rule.rule.attrib, &rx_prop->rx[0].attrib,
		sizeof(flt_rule.rule.attrib));

	flt_rule.rule.attrib.attrib_mask = IPA_FLT_SRC_ADDR | IPA_FLT_DST_ADDR;
	flt_rule.rule.attrib.u.v6.dst_addr[0] = ~0;
	flt_rule.rule.attrib.u.v6.dst_addr[1] = ~0;
	flt_rule.rule.attrib.u.v6.dst_addr[2] = ~0;
	flt_rule.rule.attrib.u.v6.dst_addr[3] = ~0;
	flt_rule.rule.attrib.u.v6.dst_addr_mask[0] = ~0;
	flt_rule.rule.attrib.u.v6.dst_addr_mask[1] = ~0;
	flt_rule.rule.attrib.u.v6.dst_addr_mask[2] = ~0;
	flt_rule.rule.attrib.u.v6.dst_addr_mask[3] = ~0;
	flt_rule.rule.attrib.u.v6.src_addr[0] = ~0;
	flt_rule.rule.attrib.u.v6.src_addr[1] = ~0;
	flt_rule.rule.attrib.u.v6.src_addr[2] = ~0;
	flt_rule.rule.attrib.u.v6.src_addr[3] = ~0;
	flt_rule.rule.attrib.u.v6.src_addr_mask[0] = ~0;
	flt_rule.rule.attrib.u.v6.src_addr_mask[1] = ~0;
	flt_rule.rule.attrib.u.v6.src_addr_mask[2] = ~0;
	flt_rule.rule.attrib.u.v6.src_addr_mask[3] = ~0;

	for(i = 0; i < IPA_MAX_IPV6_PREFIX_FLT_RULE; i++)
	{
		memcpy(&(pFilteringTable->rules[i]), &flt_rule, sizeof(struct ipa_flt_rule_add));
	}

	if(false == m_filtering.AddFilteringRule(pFilteringTable))
	{
		IPACMERR("Error adding dummy v6 prefix flt rule\n");
		res = IPACM_FAILURE;
		goto fail;
	}
	else
	{
		IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, IPA_MAX_IPV6_PREFIX_FLT_RULE);
		dummy_prefix_installed = true;

		/* copy filter rule hdls */
		for(int i = 0; i < IPA_MAX_IPV6_PREFIX_FLT_RULE; i++)
		{
			if(pFilteringTable->rules[i].status == 0)
			{
				ipv6_prefix_flt_rule_hdl[i] = pFilteringTable->rules[i].flt_rule_hdl;
				IPACMDBG_H("Dummy v6 prefix flt rule %d hdl:0x%x\n", i, ipv6_prefix_flt_rule_hdl[i]);
			}
			else
			{
				IPACMERR("Failed adding dummy v6 prefix flt rule %d\n", i);
				res = IPACM_FAILURE;
				goto fail;
			}
		}
	}

fail:
	free(pFilteringTable);
	return res;
}

int IPACM_Lan::modify_ipv6_prefix_flt_rule()
{
	int i, len, res = IPACM_SUCCESS;
	struct ipa_flt_rule_mdfy flt_rule;
	struct ipa_ioc_mdfy_flt_rule* pFilteringTable;

	if(rx_prop == NULL)
	{
		IPACMERR("no rx props\n");
		return IPACM_FAILURE;
	}

	IPACMDBG_H("modifying prefixes, num %d\n", IPACM_Iface::ipacmcfg->num_ipv6_prefixes);

	for(i = 0; i < IPA_MAX_IPV6_PREFIX_FLT_RULE; i++)
	{
		reset_to_dummy_flt_rule(IPA_IP_v6, ipv6_prefix_flt_rule_hdl[i]);
	}

	len = sizeof(struct ipa_ioc_mdfy_flt_rule) + (IPACM_Iface::ipacmcfg->num_ipv6_prefixes) * sizeof(struct ipa_flt_rule_mdfy);
	pFilteringTable = (struct ipa_ioc_mdfy_flt_rule*)malloc(len);
	if(!pFilteringTable)
	{
		IPACMERR("Failed to allocate ipa_ioc_mdfy_flt_rule memory...\n");
		return IPACM_FAILURE;
	}
	memset(pFilteringTable, 0, len);

	pFilteringTable->commit = 1;
	pFilteringTable->ip = IPA_IP_v6;
	pFilteringTable->num_rules = (uint8_t)IPACM_Iface::ipacmcfg->num_ipv6_prefixes;

	memset(&flt_rule, 0, sizeof(struct ipa_flt_rule_mdfy));
	flt_rule.status = -1;

	flt_rule.rule.retain_hdr = 1;
	flt_rule.rule.to_uc = 0;
	flt_rule.rule.action = IPA_PASS_TO_EXCEPTION;
	flt_rule.rule.eq_attrib_type = 0;

	memcpy(&flt_rule.rule.attrib, &rx_prop->rx[0].attrib, sizeof(flt_rule.rule.attrib));
	flt_rule.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;

	for(i = 0; i < (IPACM_Iface::ipacmcfg->num_ipv6_prefixes); i++)
	{
		flt_rule.rule_hdl = ipv6_prefix_flt_rule_hdl[i];

		flt_rule.rule.attrib.u.v6.dst_addr[0] = IPACM_Iface::ipacmcfg->ipa_ipv6_prefixes[i][0];
		flt_rule.rule.attrib.u.v6.dst_addr[1] = IPACM_Iface::ipacmcfg->ipa_ipv6_prefixes[i][1];
		flt_rule.rule.attrib.u.v6.dst_addr[2] = 0x0;
		flt_rule.rule.attrib.u.v6.dst_addr[3] = 0x0;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[0] = 0xFFFFFFFF;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[1] = 0xFFFFFFFF;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[2] = 0x0;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[3] = 0x0;

		memcpy(&(pFilteringTable->rules[i]), &flt_rule, sizeof(struct ipa_flt_rule_mdfy));
		IPACMDBG_H(" IPACM v6 prefix as: 0x[%X][%X] entry(%d)\n",
			flt_rule.rule.attrib.u.v6.dst_addr[0],
			flt_rule.rule.attrib.u.v6.dst_addr[1], i);
	}

	if(false == m_filtering.ModifyFilteringRule(pFilteringTable))
	{
		IPACMERR("Failed to modify private subnet filtering rules.\n");
		res = IPACM_FAILURE;
		goto fail;
	}

fail:
	if(pFilteringTable != NULL)
	{
		free(pFilteringTable);
	}
	return res;
}
#endif

int IPACM_Lan::install_ipv6_prefix_flt_rule(uint32_t* prefix)
{
	if(prefix == NULL)
	{
		IPACMERR("IPv6 prefix is empty.\n");
		return IPACM_FAILURE;
	}
	IPACMDBG_H("Receive IPv6 prefix: 0x%08x%08x.\n", prefix[0], prefix[1]);

	int len;
	struct ipa_ioc_add_flt_rule* flt_rule;
	struct ipa_flt_rule_add flt_rule_entry;

	if(rx_prop != NULL)
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
		flt_rule_entry.rule.attrib.u.v6.dst_addr[0] = prefix[0];
		flt_rule_entry.rule.attrib.u.v6.dst_addr[1] = prefix[1];
		flt_rule_entry.rule.attrib.u.v6.dst_addr[2] = 0x0;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[3] = 0x0;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[0] = 0xFFFFFFFF;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[1] = 0xFFFFFFFF;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[2] = 0x0;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[3] = 0x0;
		memcpy(&(flt_rule->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

		if (m_filtering.AddFilteringRule(flt_rule) == false)
		{
			IPACMERR("Error Adding Filtering rule, aborting...\n");
			free(flt_rule);
			return IPACM_FAILURE;
		}
		else
		{
			IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, 1);
			ipv6_prefix_flt_rule_hdl[0] = flt_rule->rules[0].flt_rule_hdl;
			IPACMDBG_H("IPv6 prefix filter rule HDL:0x%x\n", ipv6_prefix_flt_rule_hdl[0]);
			free(flt_rule);
		}
	}
	return IPACM_SUCCESS;
}

void IPACM_Lan::delete_ipv6_prefix_flt_rule()
{
	if(m_filtering.DeleteFilteringHdls(&ipv6_prefix_flt_rule_hdl[0], IPA_IP_v6, IPA_MAX_IPV6_PREFIX_FLT_RULE) == false)
	{
		IPACMERR("Failed to delete ipv6 prefix flt rule.\n");
		return;
	}
	IPACM_Iface::ipacmcfg->decreaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, IPA_MAX_IPV6_PREFIX_FLT_RULE);
	return;
}

int IPACM_Lan::handle_addr_evt_odu_bridge(ipacm_event_data_addr* data)
{
	int fd, res = IPACM_SUCCESS;
	struct in6_addr ipv6_addr;
	if(data == NULL)
	{
		IPACMERR("Failed to get interface IP address.\n");
		return IPACM_FAILURE;
	}

	if(data->iptype == IPA_IP_v6)
	{
		fd = open(IPACM_Iface::ipacmcfg->DEVICE_NAME_ODU, O_RDWR);
		if(fd == 0)
		{
			IPACMERR("Failed to open %s.\n", IPACM_Iface::ipacmcfg->DEVICE_NAME_ODU);
			return IPACM_FAILURE;
		}

		memcpy(&ipv6_addr, data->ipv6_addr, sizeof(struct in6_addr));

		if( ioctl(fd, ODU_BRIDGE_IOC_SET_LLV6_ADDR, &ipv6_addr) )
		{
			IPACMERR("Failed to write IPv6 address to odu driver.\n");
			res = IPACM_FAILURE;
		}
		num_dft_rt_v6++;
		close(fd);
	}

	return res;
}

ipa_hdr_proc_type IPACM_Lan::eth_bridge_get_hdr_proc_type(ipa_hdr_l2_type t1, ipa_hdr_l2_type t2)
{
	if(t1 == IPA_HDR_L2_ETHERNET_II)
	{
		if(t2 == IPA_HDR_L2_ETHERNET_II)
		{
			return IPA_HDR_PROC_ETHII_TO_ETHII;
		}
		if(t2 == IPA_HDR_L2_802_3)
		{
			return IPA_HDR_PROC_ETHII_TO_802_3;
		}
	}

	if(t1 == IPA_HDR_L2_802_3)
	{
		if(t2 == IPA_HDR_L2_ETHERNET_II)
		{
			return IPA_HDR_PROC_802_3_TO_ETHII;
		}
		if(t2 == IPA_HDR_L2_802_3)
		{
			return IPA_HDR_PROC_802_3_TO_802_3;
		}
	}

	return IPA_HDR_PROC_NONE;
}

int IPACM_Lan::eth_bridge_get_hdr_template_hdl(uint32_t* hdr_hdl)
{
	if(hdr_hdl == NULL)
	{
		IPACMDBG_H("Hdr handle pointer is empty.\n");
		return IPACM_FAILURE;
	}

	struct ipa_ioc_get_hdr hdr;
	memset(&hdr, 0, sizeof(hdr));

	memcpy(hdr.name, tx_prop->tx[0].hdr_name, sizeof(hdr.name));
	if(m_header.GetHeaderHandle(&hdr) == false)
	{
		IPACMERR("Failed to get template hdr hdl.\n");
		return IPACM_FAILURE;
	}

	*hdr_hdl = hdr.hdl;
	return IPACM_SUCCESS;
}

int IPACM_Lan::handle_cradle_wan_mode_switch(bool is_wan_bridge_mode)
{
	struct ipa_flt_rule_mdfy flt_rule_entry;
	int len = 0;
	ipa_ioc_mdfy_flt_rule *m_pFilteringTable;

	IPACMDBG_H("Handle wan mode swtich: is wan bridge mode?%d\n", is_wan_bridge_mode);

	if (rx_prop == NULL)
	{
		IPACMDBG_H("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	len = sizeof(struct ipa_ioc_mdfy_flt_rule) + (1 * sizeof(struct ipa_flt_rule_mdfy));
	m_pFilteringTable = (struct ipa_ioc_mdfy_flt_rule *)calloc(1, len);
	if (m_pFilteringTable == NULL)
	{
		PERROR("Error Locate ipa_ioc_mdfy_flt_rule memory...\n");
		return IPACM_FAILURE;
	}

	m_pFilteringTable->commit = 1;
	m_pFilteringTable->ip = IPA_IP_v4;
	m_pFilteringTable->num_rules = (uint8_t)1;

	IPACMDBG_H("Retrieving routing hanle for table: %s\n",
					 IPACM_Iface::ipacmcfg->rt_tbl_wan_v4.name);
	if (false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_wan_v4))
	{
		IPACMERR("m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_wan_v4=0x%p) Failed.\n",
						 &IPACM_Iface::ipacmcfg->rt_tbl_wan_v4);
		free(m_pFilteringTable);
		return IPACM_FAILURE;
	}
	IPACMDBG_H("Routing handle for table: %d\n", IPACM_Iface::ipacmcfg->rt_tbl_wan_v4.hdl);


	memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_mdfy)); // Zero All Fields
	flt_rule_entry.status = -1;
	flt_rule_entry.rule_hdl = lan_wan_fl_rule_hdl[0];

	flt_rule_entry.rule.retain_hdr = 0;
	flt_rule_entry.rule.to_uc = 0;
	flt_rule_entry.rule.eq_attrib_type = 0;
	if(is_wan_bridge_mode)
	{
		flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
	}
	else
	{
		flt_rule_entry.rule.action = IPA_PASS_TO_SRC_NAT;
	}
	flt_rule_entry.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_wan_v4.hdl;

	memcpy(&flt_rule_entry.rule.attrib,
				 &rx_prop->rx[0].attrib,
				 sizeof(flt_rule_entry.rule.attrib));

	flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
	flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = 0x0;
	flt_rule_entry.rule.attrib.u.v4.dst_addr = 0x0;

	memcpy(&m_pFilteringTable->rules[0], &flt_rule_entry, sizeof(flt_rule_entry));
	if (false == m_filtering.ModifyFilteringRule(m_pFilteringTable))
	{
		IPACMERR("Error Modifying RuleTable(0) to Filtering, aborting...\n");
		free(m_pFilteringTable);
		return IPACM_FAILURE;
	}
	else
	{
		IPACMDBG_H("flt rule hdl = %d, status = %d\n",
						 m_pFilteringTable->rules[0].rule_hdl,
						 m_pFilteringTable->rules[0].status);
	}
	free(m_pFilteringTable);
	return IPACM_SUCCESS;
}

/*handle reset usb-client rt-rules */
int IPACM_Lan::handle_tethering_stats_event(ipa_get_data_stats_resp_msg_v01 *data)
{
	int cnt, pipe_len, fd;
	uint64_t num_ul_packets, num_ul_bytes;
	uint64_t num_dl_packets, num_dl_bytes;
	bool ul_pipe_found, dl_pipe_found;
	FILE *fp = NULL;

	fd = open(IPA_DEVICE_NAME, O_RDWR);
	if (fd < 0)
	{
		IPACMERR("Failed opening %s.\n", IPA_DEVICE_NAME);
		return IPACM_FAILURE;
	}


	ul_pipe_found = false;
	dl_pipe_found = false;
	num_ul_packets = 0;
	num_dl_packets = 0;
	num_ul_bytes = 0;
	num_dl_bytes = 0;

	if (data->dl_dst_pipe_stats_list_valid)
	{
		if(tx_prop != NULL)
		{
			for (pipe_len = 0; pipe_len < data->dl_dst_pipe_stats_list_len; pipe_len++)
			{
				IPACMDBG_H("Check entry(%d) dl_dst_pipe(%d)\n", pipe_len, data->dl_dst_pipe_stats_list[pipe_len].pipe_index);
				for (cnt=0; cnt<tx_prop->num_tx_props; cnt++)
				{
					IPACMDBG_H("Check Tx_prop_entry(%d) pipe(%d)\n", cnt, ioctl(fd, IPA_IOC_QUERY_EP_MAPPING, tx_prop->tx[cnt].dst_pipe));
					if(ioctl(fd, IPA_IOC_QUERY_EP_MAPPING, tx_prop->tx[cnt].dst_pipe) == data->dl_dst_pipe_stats_list[pipe_len].pipe_index)
					{
						/* update the DL stats */
						dl_pipe_found = true;
						num_dl_packets += data->dl_dst_pipe_stats_list[pipe_len].num_ipv4_packets;
						num_dl_packets += data->dl_dst_pipe_stats_list[pipe_len].num_ipv6_packets;
						num_dl_bytes += data->dl_dst_pipe_stats_list[pipe_len].num_ipv4_bytes;
						num_dl_bytes += data->dl_dst_pipe_stats_list[pipe_len].num_ipv6_bytes;
						IPACMDBG_H("Got matched dst-pipe (%d) from %d tx props\n", data->dl_dst_pipe_stats_list[pipe_len].pipe_index, cnt);
						IPACMDBG_H("DL_packets:(%lu) DL_bytes:(%lu) \n", num_dl_packets, num_dl_bytes);
						break;
					}
				}
			}
		}
	}

	if (data->ul_src_pipe_stats_list_valid)
	{
		if(rx_prop != NULL)
		{
			for (pipe_len = 0; pipe_len < data->ul_src_pipe_stats_list_len; pipe_len++)
			{
				IPACMDBG_H("Check entry(%d) dl_dst_pipe(%d)\n", pipe_len, data->ul_src_pipe_stats_list[pipe_len].pipe_index);
				for (cnt=0; cnt < rx_prop->num_rx_props; cnt++)
				{
					IPACMDBG_H("Check Rx_prop_entry(%d) pipe(%d)\n", cnt, ioctl(fd, IPA_IOC_QUERY_EP_MAPPING, rx_prop->rx[cnt].src_pipe));
					if(ioctl(fd, IPA_IOC_QUERY_EP_MAPPING, rx_prop->rx[cnt].src_pipe) == data->ul_src_pipe_stats_list[pipe_len].pipe_index)
					{
						/* update the UL stats */
						ul_pipe_found = true;
						num_ul_packets += data->ul_src_pipe_stats_list[pipe_len].num_ipv4_packets;
						num_ul_packets += data->ul_src_pipe_stats_list[pipe_len].num_ipv6_packets;
						num_ul_bytes += data->ul_src_pipe_stats_list[pipe_len].num_ipv4_bytes;
						num_ul_bytes += data->ul_src_pipe_stats_list[pipe_len].num_ipv6_bytes;
						IPACMDBG_H("Got matched dst-pipe (%d) from %d tx props\n", data->ul_src_pipe_stats_list[pipe_len].pipe_index, cnt);
						IPACMDBG_H("UL_packets:(%lu) UL_bytes:(%lu) \n", num_ul_packets, num_ul_bytes);
						break;
					}
				}
			}
		}
	}
	close(fd);

	if (ul_pipe_found || dl_pipe_found)
	{
		IPACMDBG_H("Update IPA_TETHERING_STATS_UPDATE_EVENT, TX(P%lu/B%lu) RX(P%lu/B%lu) DEV(%s) to LTE(%s) \n",
					num_ul_packets,
						num_ul_bytes,
							num_dl_packets,
								num_dl_bytes,
									dev_name,
										IPACM_Wan::wan_up_dev_name);
		fp = fopen(IPA_PIPE_STATS_FILE_NAME, "w");
		if ( fp == NULL )
		{
			IPACMERR("Failed to write pipe stats to %s, error is %d - %s\n",
					IPA_PIPE_STATS_FILE_NAME, errno, strerror(errno));
			return IPACM_FAILURE;
		}

		fprintf(fp, PIPE_STATS,
				dev_name,
					IPACM_Wan::wan_up_dev_name,
						num_ul_bytes,
						num_ul_packets,
							    num_dl_bytes,
							num_dl_packets);
		fclose(fp);
	}
	return IPACM_SUCCESS;
}

/*handle tether client */
int IPACM_Lan::handle_tethering_client(bool reset, ipacm_client_enum ipa_client)
{
	int cnt, fd, ret = IPACM_SUCCESS;
	int fd_wwan_ioctl = open(WWAN_QMI_IOCTL_DEVICE_NAME, O_RDWR);
	wan_ioctl_set_tether_client_pipe tether_client;

	if(fd_wwan_ioctl < 0)
	{
		IPACMERR("Failed to open %s.\n",WWAN_QMI_IOCTL_DEVICE_NAME);
		return IPACM_FAILURE;
	}

	fd = open(IPA_DEVICE_NAME, O_RDWR);
	if (fd < 0)
	{
		IPACMERR("Failed opening %s.\n", IPA_DEVICE_NAME);
		close(fd_wwan_ioctl);
		return IPACM_FAILURE;
	}

	memset(&tether_client, 0, sizeof(tether_client));
	tether_client.reset_client = reset;
	tether_client.ipa_client = ipa_client;

	if(tx_prop != NULL)
	{
		tether_client.dl_dst_pipe_len = tx_prop->num_tx_props;
		for (cnt = 0; cnt < tx_prop->num_tx_props; cnt++)
		{
			IPACMDBG_H("Tx(%d), dst_pipe: %d, ipa_pipe: %d\n",
					cnt, tx_prop->tx[cnt].dst_pipe,
						ioctl(fd, IPA_IOC_QUERY_EP_MAPPING, tx_prop->tx[cnt].dst_pipe));
			tether_client.dl_dst_pipe_list[cnt] = ioctl(fd, IPA_IOC_QUERY_EP_MAPPING, tx_prop->tx[cnt].dst_pipe);
		}
	}

	if(rx_prop != NULL)
	{
		tether_client.ul_src_pipe_len = rx_prop->num_rx_props;
		for (cnt = 0; cnt < rx_prop->num_rx_props; cnt++)
		{
			IPACMDBG_H("Rx(%d), src_pipe: %d, ipa_pipe: %d\n",
					cnt, rx_prop->rx[cnt].src_pipe,
						ioctl(fd, IPA_IOC_QUERY_EP_MAPPING, rx_prop->rx[cnt].src_pipe));
			tether_client.ul_src_pipe_list[cnt] = ioctl(fd, IPA_IOC_QUERY_EP_MAPPING, rx_prop->rx[cnt].src_pipe);
		}
	}

	ret = ioctl(fd_wwan_ioctl, WAN_IOC_SET_TETHER_CLIENT_PIPE, &tether_client);
	if (ret != 0)
	{
		IPACMERR("Failed set tether-client-pipe %p with ret %d\n ", &tether_client, ret);
	}
	IPACMDBG("Set tether-client-pipe %p\n", &tether_client);
	close(fd);
	close(fd_wwan_ioctl);
	return ret;
}

/* mac address has to be provided for client related events */
void IPACM_Lan::eth_bridge_post_event(ipa_cm_event_id evt, ipa_ip_type iptype, uint8_t *mac, uint32_t *ipv6_addr, char *iface_name)
{
	ipacm_cmd_q_data eth_bridge_evt;
	ipacm_event_eth_bridge *evt_data_eth_bridge;
	ipacm_event_data_all *evt_data_all;

	memset(&eth_bridge_evt, 0, sizeof(ipacm_cmd_q_data));
	eth_bridge_evt.event = evt;

	evt_data_eth_bridge = (ipacm_event_eth_bridge*)malloc(sizeof(*evt_data_eth_bridge));
	if(evt_data_eth_bridge == NULL)
	{
		IPACMERR("Failed to allocate memory.\n");
		return;
	}
	memset(evt_data_eth_bridge, 0, sizeof(*evt_data_eth_bridge));

	evt_data_eth_bridge->p_iface = this;
	evt_data_eth_bridge->iptype = iptype;
	if(mac)
	{
		IPACMDBG_H("Mac: 0x%02x%02x%02x%02x%02x%02x \n",
			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		memcpy(evt_data_eth_bridge->mac_addr, mac, sizeof(evt_data_eth_bridge->mac_addr));
	}
	if(iface_name)
	{
		IPACMDBG_H("Iface: %s\n", iface_name);
		memcpy(evt_data_eth_bridge->iface_name, iface_name,
			sizeof(evt_data_eth_bridge->iface_name));
	}
	eth_bridge_evt.evt_data = (void*)evt_data_eth_bridge;

	IPACMDBG_H("Posting event %s\n",
		IPACM_Iface::ipacmcfg->getEventName(evt));
	IPACM_EvtDispatcher::PostEvt(&eth_bridge_evt);
}

/* add header processing context and return handle to lan2lan controller */
int IPACM_Lan::eth_bridge_add_hdr_proc_ctx(ipa_hdr_l2_type peer_l2_hdr_type, uint32_t *hdl)
{
	int len, res = IPACM_SUCCESS;
	uint32_t hdr_template;
	ipa_ioc_add_hdr_proc_ctx* pHeaderProcTable = NULL;

	if(tx_prop == NULL)
	{
		IPACMERR("No tx prop.\n");
		return IPACM_FAILURE;
	}

	len = sizeof(struct ipa_ioc_add_hdr_proc_ctx) + sizeof(struct ipa_hdr_proc_ctx_add);
	pHeaderProcTable = (ipa_ioc_add_hdr_proc_ctx*)malloc(len);
	if(pHeaderProcTable == NULL)
	{
		IPACMERR("Cannot allocate header processing context table.\n");
		return IPACM_FAILURE;
	}

	memset(pHeaderProcTable, 0, len);
	pHeaderProcTable->commit = 1;
	pHeaderProcTable->num_proc_ctxs = 1;
	pHeaderProcTable->proc_ctx[0].type = eth_bridge_get_hdr_proc_type(peer_l2_hdr_type, tx_prop->tx[0].hdr_l2_type);
	eth_bridge_get_hdr_template_hdl(&hdr_template);
	pHeaderProcTable->proc_ctx[0].hdr_hdl = hdr_template;
	if (m_header.AddHeaderProcCtx(pHeaderProcTable) == false)
	{
		IPACMERR("Adding hdr proc ctx failed with status: %d\n", pHeaderProcTable->proc_ctx[0].status);
		res = IPACM_FAILURE;
		goto end;
	}

	*hdl = pHeaderProcTable->proc_ctx[0].proc_ctx_hdl;

end:
	free(pHeaderProcTable);
	return res;
}

/* add routing rule and return handle to lan2lan controller */
int IPACM_Lan::eth_bridge_add_rt_rule(uint8_t *mac, char *rt_tbl_name, uint32_t hdr_proc_ctx_hdl,
		ipa_hdr_l2_type peer_l2_hdr_type, ipa_ip_type iptype, uint32_t *rt_rule_hdl, int *rt_rule_count)
{
	int i, len, res = IPACM_SUCCESS;
	struct ipa_ioc_add_rt_rule* rt_rule_table = NULL;
	struct ipa_rt_rule_add rt_rule;
	int position, num_rt_rule;

	IPACMDBG_H("Received client MAC 0x%02x%02x%02x%02x%02x%02x.\n",
			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	num_rt_rule = each_client_rt_rule_count[iptype];

	len = sizeof(ipa_ioc_add_rt_rule) + num_rt_rule * sizeof(ipa_rt_rule_add);
	rt_rule_table = (ipa_ioc_add_rt_rule*)malloc(len);
	if (rt_rule_table == NULL)
	{
		IPACMERR("Failed to allocate memory.\n");
		return IPACM_FAILURE;
	}
	memset(rt_rule_table, 0, len);

	rt_rule_table->commit = 1;
	rt_rule_table->ip = iptype;
	rt_rule_table->num_rules = num_rt_rule;
	strlcpy(rt_rule_table->rt_tbl_name, rt_tbl_name, sizeof(rt_rule_table->rt_tbl_name));
	rt_rule_table->rt_tbl_name[IPA_RESOURCE_NAME_MAX-1] = 0;

	memset(&rt_rule, 0, sizeof(ipa_rt_rule_add));
	rt_rule.at_rear = false;
	rt_rule.status = -1;
	rt_rule.rt_rule_hdl = -1;
#ifdef FEATURE_IPA_V3
	rt_rule.rule.hashable = true;
#endif
	rt_rule.rule.hdr_hdl = 0;
	rt_rule.rule.hdr_proc_ctx_hdl = hdr_proc_ctx_hdl;

	position = 0;
	for(i=0; i<iface_query->num_tx_props; i++)
	{
		if(tx_prop->tx[i].ip == iptype)
		{
			if(position >= num_rt_rule || position >= MAX_NUM_PROP)
			{
				IPACMERR("Number of routing rules already exceeds limit.\n");
				res = IPACM_FAILURE;
				goto end;
			}

			if(ipa_if_cate == WLAN_IF && IPACM_Iface::ipacmcfg->isMCC_Mode)
			{
				IPACMDBG_H("In WLAN MCC mode, use alt dst pipe: %d\n",
						tx_prop->tx[i].alt_dst_pipe);
				rt_rule.rule.dst = tx_prop->tx[i].alt_dst_pipe;
			}
			else
			{
				IPACMDBG_H("It is not WLAN MCC mode, use dst pipe: %d\n",
						tx_prop->tx[i].dst_pipe);
				rt_rule.rule.dst = tx_prop->tx[i].dst_pipe;
			}

			memcpy(&rt_rule.rule.attrib, &tx_prop->tx[i].attrib, sizeof(rt_rule.rule.attrib));
			if(peer_l2_hdr_type == IPA_HDR_L2_ETHERNET_II)
				rt_rule.rule.attrib.attrib_mask |= IPA_FLT_MAC_DST_ADDR_ETHER_II;
			else
				rt_rule.rule.attrib.attrib_mask |= IPA_FLT_MAC_DST_ADDR_802_3;
			memcpy(rt_rule.rule.attrib.dst_mac_addr, mac, sizeof(rt_rule.rule.attrib.dst_mac_addr));
			memset(rt_rule.rule.attrib.dst_mac_addr_mask, 0xFF, sizeof(rt_rule.rule.attrib.dst_mac_addr_mask));

			memcpy(&(rt_rule_table->rules[position]), &rt_rule, sizeof(rt_rule_table->rules[position]));
			position++;
		}
	}
	if(false == m_routing.AddRoutingRule(rt_rule_table))
	{
		IPACMERR("Routing rule addition failed!\n");
		res = IPACM_FAILURE;
		goto end;
	}
	else
	{
		*rt_rule_count = position;
		for(i=0; i<position; i++)
			rt_rule_hdl[i] = rt_rule_table->rules[i].rt_rule_hdl;
	}

end:
	free(rt_rule_table);
	return res;
}

/* modify routing rule*/
int IPACM_Lan::eth_bridge_modify_rt_rule(uint8_t *mac, uint32_t hdr_proc_ctx_hdl,
		ipa_hdr_l2_type peer_l2_hdr_type, ipa_ip_type iptype, uint32_t *rt_rule_hdl, int rt_rule_count)
{
	struct ipa_ioc_mdfy_rt_rule *rt_rule = NULL;
	struct ipa_rt_rule_mdfy *rt_rule_entry;
	int len, index, res = IPACM_SUCCESS;

	if(tx_prop == NULL)
	{
		IPACMDBG_H("No tx properties \n");
		return IPACM_FAILURE;
	}

	if(ipa_if_cate != WLAN_IF)
	{
		IPACMDBG_H("This is not WLAN IF, no need to modify rt rule.\n");
		return IPACM_SUCCESS;
	}

	IPACMDBG_H("Receive WLAN client MAC 0x%02x%02x%02x%02x%02x%02x.\n",
			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	len = sizeof(struct ipa_ioc_mdfy_rt_rule) + rt_rule_count * sizeof(struct ipa_rt_rule_mdfy);
	rt_rule = (struct ipa_ioc_mdfy_rt_rule *)malloc(len);
	if(rt_rule == NULL)
	{
		IPACMERR("Unable to allocate memory for modify rt rule\n");
		return IPACM_FAILURE;
	}
	memset(rt_rule, 0, len);

	rt_rule->commit = 1;
	rt_rule->num_rules = 0;
	rt_rule->ip = iptype;

	for (index = 0; index < tx_prop->num_tx_props; index++)
	{
		if (tx_prop->tx[index].ip == iptype)
		{
			if (rt_rule->num_rules >= rt_rule_count ||
				rt_rule->num_rules >= MAX_NUM_PROP)
			{
				IPACMERR("Number of routing rules exceeds limit.\n");
				res = IPACM_FAILURE;
				goto end;
			}

			rt_rule_entry = &rt_rule->rules[rt_rule->num_rules];

			if (IPACM_Iface::ipacmcfg->isMCC_Mode)
			{
				IPACMDBG_H("In WLAN MCC mode, use alt dst pipe: %d\n",
						tx_prop->tx[index].alt_dst_pipe);
				rt_rule_entry->rule.dst = tx_prop->tx[index].alt_dst_pipe;
			}
			else
			{
				IPACMDBG_H("In WLAN SCC mode, use dst pipe: %d\n",
						tx_prop->tx[index].dst_pipe);
				rt_rule_entry->rule.dst = tx_prop->tx[index].dst_pipe;
			}

			rt_rule_entry->rule.hdr_hdl = 0;
			rt_rule_entry->rule.hdr_proc_ctx_hdl = hdr_proc_ctx_hdl;
#ifdef FEATURE_IPA_V3
			rt_rule_entry->rule.hashable = true;
#endif
			memcpy(&rt_rule_entry->rule.attrib, &tx_prop->tx[index].attrib,
					sizeof(rt_rule_entry->rule.attrib));
			if(peer_l2_hdr_type == IPA_HDR_L2_ETHERNET_II)
				rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_MAC_DST_ADDR_ETHER_II;
			else
				rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_MAC_DST_ADDR_802_3;
			memcpy(rt_rule_entry->rule.attrib.dst_mac_addr, mac,
					sizeof(rt_rule_entry->rule.attrib.dst_mac_addr));
			memset(rt_rule_entry->rule.attrib.dst_mac_addr_mask, 0xFF,
					sizeof(rt_rule_entry->rule.attrib.dst_mac_addr_mask));

			rt_rule_entry->rt_rule_hdl = rt_rule_hdl[rt_rule->num_rules];
			rt_rule->num_rules++;
		}
	}

	if(m_routing.ModifyRoutingRule(rt_rule) == false)
	{
		IPACMERR("Failed to modify routing rules.\n");
		res = IPACM_FAILURE;
		goto end;
	}
	if(m_routing.Commit(iptype) == false)
	{
		IPACMERR("Failed to commit routing rules.\n");
		res = IPACM_FAILURE;
		goto end;
	}
	IPACMDBG("Modified routing rules successfully.\n");

end:
	free(rt_rule);
	return res;
}

int IPACM_Lan::eth_bridge_add_flt_rule(uint8_t *mac, uint32_t rt_tbl_hdl, ipa_ip_type iptype, uint32_t *flt_rule_hdl)
{
	int len, res = IPACM_SUCCESS;
	struct ipa_flt_rule_add flt_rule_entry;
	struct ipa_ioc_add_flt_rule_after *pFilteringTable = NULL;

#ifdef FEATURE_IPA_V3
	if (rx_prop == NULL || tx_prop == NULL)
	{
		IPACMDBG_H("No rx or tx properties registered for iface %s\n", dev_name);
		return IPACM_FAILURE;
	}

	IPACMDBG_H("Received client MAC 0x%02x%02x%02x%02x%02x%02x.\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	len = sizeof(struct ipa_ioc_add_flt_rule_after) + sizeof(struct ipa_flt_rule_add);
	pFilteringTable = (struct ipa_ioc_add_flt_rule_after*)malloc(len);
	if (!pFilteringTable)
	{
		IPACMERR("Failed to allocate ipa_ioc_add_flt_rule_after memory...\n");
		return IPACM_FAILURE;
	}
	memset(pFilteringTable, 0, len);

	/* add mac based rule*/
	pFilteringTable->commit = 1;
	pFilteringTable->ep = rx_prop->rx[0].src_pipe;
	pFilteringTable->ip = iptype;
	pFilteringTable->num_rules = 1;
	pFilteringTable->add_after_hdl = eth_bridge_flt_rule_offset[iptype];

	memset(&flt_rule_entry, 0, sizeof(flt_rule_entry));
	flt_rule_entry.at_rear = 1;

	flt_rule_entry.rule.retain_hdr = 0;
	flt_rule_entry.rule.to_uc = 0;
	flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
	flt_rule_entry.rule.eq_attrib_type = 0;
	flt_rule_entry.rule.rt_tbl_hdl = rt_tbl_hdl;
	flt_rule_entry.rule.hashable = true;

	memcpy(&flt_rule_entry.rule.attrib, &rx_prop->rx[0].attrib, sizeof(flt_rule_entry.rule.attrib));
	if(tx_prop->tx[0].hdr_l2_type == IPA_HDR_L2_ETHERNET_II)
	{
		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_MAC_DST_ADDR_ETHER_II;
	}
	else
	{
		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_MAC_DST_ADDR_802_3;
	}

	memcpy(flt_rule_entry.rule.attrib.dst_mac_addr, mac, sizeof(flt_rule_entry.rule.attrib.dst_mac_addr));
	memset(flt_rule_entry.rule.attrib.dst_mac_addr_mask, 0xFF, sizeof(flt_rule_entry.rule.attrib.dst_mac_addr_mask));

	memcpy(&(pFilteringTable->rules[0]), &flt_rule_entry, sizeof(flt_rule_entry));
	if (false == m_filtering.AddFilteringRuleAfter(pFilteringTable))
	{
		IPACMERR("Failed to add client filtering rules.\n");
		res = IPACM_FAILURE;
		goto end;
	}
	*flt_rule_hdl = pFilteringTable->rules[0].flt_rule_hdl;

end:
	free(pFilteringTable);
#endif
	return res;
}

int IPACM_Lan::eth_bridge_del_flt_rule(uint32_t flt_rule_hdl, ipa_ip_type iptype)
{
	if(m_filtering.DeleteFilteringHdls(&flt_rule_hdl, iptype, 1) == false)
	{
		IPACMERR("Failed to delete the client specific flt rule.\n");
		return IPACM_FAILURE;
	}
	return IPACM_SUCCESS;
}

int IPACM_Lan::eth_bridge_del_rt_rule(uint32_t rt_rule_hdl, ipa_ip_type iptype)
{
	if(m_routing.DeleteRoutingHdl(rt_rule_hdl, iptype) == false)
	{
		IPACMERR("Failed to delete routing rule.\n");
		return IPACM_FAILURE;
	}
	return IPACM_SUCCESS;
}

/* delete header processing context */
int IPACM_Lan::eth_bridge_del_hdr_proc_ctx(uint32_t hdr_proc_ctx_hdl)
{
	if(m_header.DeleteHeaderProcCtx(hdr_proc_ctx_hdl) == false)
	{
		IPACMERR("Failed to delete hdr proc ctx.\n");
		return IPACM_FAILURE;
	}
	return IPACM_SUCCESS;
}

#if defined(FEATURE_L2TP_E2E) || defined(FEATURE_L2TP) || defined(FEATURE_VLAN_MPDN)
/* check if the event is associated with vlan interface */
bool IPACM_Lan::is_vlan_event(char *event_iface_name)
{
	int self_name_len, event_iface_name_len;
	if(event_iface_name == NULL)
	{
		IPACMERR("Invalid input\n");
		return false;
	}

	IPACMDBG_H("Self iface %s, event iface %s\n", dev_name, event_iface_name);
	self_name_len = strlen(dev_name);
	event_iface_name_len = strlen(event_iface_name);

	if(event_iface_name_len > self_name_len && strncmp(dev_name, event_iface_name, self_name_len) == 0)
	{
		IPACMDBG_H("This is vlan event.\n");
		return true;
	}
	return false;
}
#ifndef FEATURE_VLAN_MPDN
/* check if the event is associated with l2tp interface */
bool IPACM_Lan::is_l2tp_event(char *event_iface_name)
{
	if(event_iface_name == NULL)
	{
		IPACMERR("Invalid input\n");
		return false;
	}

	IPACMDBG_H("Self iface %s, event iface %s\n", dev_name, event_iface_name);
	if(strncmp(event_iface_name, "l2tp", 4) == 0)
	{
		IPACMDBG_H("This is l2tp event.\n");
		return true;
	}
	return false;
}
#endif //#ifndef FEATURE_VLAN_MPDN
#endif //#if defined(FEATURE_L2TP_E2E) || defined(FEATURE_L2TP) || defined(FEATURE_VLAN_MPDN)
#ifdef FEATURE_L2TP
/* add l2tp rt rule for l2tp client */
int IPACM_Lan::add_l2tp_rt_rule(ipa_ip_type iptype, uint8_t *dst_mac, ipa_hdr_l2_type peer_l2_hdr_type,
	uint32_t l2tp_session_id, uint32_t vlan_id, uint8_t *vlan_client_mac, uint32_t *vlan_iface_ipv6_addr,
	uint32_t *vlan_client_ipv6_addr, uint32_t *first_pass_hdr_hdl, uint32_t *first_pass_hdr_proc_ctx_hdl,
	uint32_t *second_pass_hdr_hdl, int *num_rt_hdl, uint32_t *first_pass_rt_rule_hdl, uint32_t *second_pass_rt_rule_hdl)
{
	int i, size, position;
	uint32_t vlan_iface_ipv6_addr_network[4], vlan_client_ipv6_addr_network[4];
	ipa_ioc_add_hdr *hdr_table;
	ipa_hdr_add *hdr;
	ipa_ioc_add_hdr_proc_ctx *hdr_proc_ctx_table;
	ipa_hdr_proc_ctx_add *hdr_proc_ctx;
	ipa_ioc_add_rt_rule* rt_rule_table;
	ipa_rt_rule_add *rt_rule;
	ipa_ioc_copy_hdr copy_hdr;

	if(tx_prop == NULL)
	{
		IPACMERR("No tx prop.\n");
		return IPACM_FAILURE;
	}

	/* =========== install first pass hdr template (IPv6 + L2TP + inner ETH header = 62 bytes) ============= */
	if(*first_pass_hdr_hdl != 0)
	{
		IPACMDBG_H("First pass hdr template was added before.\n");
	}
	else
	{
		size = sizeof(ipa_ioc_add_hdr) + sizeof(ipa_hdr_add);
		hdr_table = (ipa_ioc_add_hdr*)malloc(size);
		if(hdr_table == NULL)
		{
			IPACMERR("Failed to allocate memory.\n");
			return IPACM_FAILURE;
		}
		memset(hdr_table, 0, size);

		hdr_table->commit = 1;
		hdr_table->num_hdrs = 1;
		hdr = &hdr_table->hdr[0];

		if(iptype == IPA_IP_v4)
		{
			snprintf(hdr->name, sizeof(hdr->name), "vlan_%d_l2tp_%d_v4", vlan_id, l2tp_session_id);
		}
		else
		{
			snprintf(hdr->name, sizeof(hdr->name), "vlan_%d_l2tp_%d_v6", vlan_id, l2tp_session_id);
		}
		hdr->hdr_len = 62;
		hdr->type = IPA_HDR_L2_ETHERNET_II;
		hdr->is_partial = 0;

		hdr->hdr[0] = 0x60;	/* version */
		hdr->hdr[6] = 0x73; /* next header = L2TP */
		hdr->hdr[7] = 0x40; /* hop limit = 64 */
		for(i = 0; i < 4; i++)
		{
			vlan_iface_ipv6_addr_network[i] = htonl(vlan_iface_ipv6_addr[i]);
			vlan_client_ipv6_addr_network[i] = htonl(vlan_client_ipv6_addr[i]);
		}
		memcpy(hdr->hdr + 8, vlan_iface_ipv6_addr_network, 16); /* source IPv6 addr */
		memcpy(hdr->hdr + 24, vlan_client_ipv6_addr_network, 16); /* dest IPv6 addr */
		hdr->hdr[43] = (uint8_t)(l2tp_session_id & 0xFF); /* l2tp header */
		hdr->hdr[42] = (uint8_t)(l2tp_session_id >> 8 & 0xFF);
		hdr->hdr[41] = (uint8_t)(l2tp_session_id >> 16 & 0xFF);
		hdr->hdr[40] = (uint8_t)(l2tp_session_id >> 24 & 0xFF);

		if(m_header.AddHeader(hdr_table) == false)
		{
			IPACMERR("Failed to add hdr with status: %d\n", hdr_table->hdr[0].status);
			free(hdr_table);
			return IPACM_FAILURE;
		}
		*first_pass_hdr_hdl = hdr_table->hdr[0].hdr_hdl;
		IPACMDBG_H("Installed first pass hdr: hdl %d\n", *first_pass_hdr_hdl);
		free(hdr_table);
	}

	/* =========== install first pass hdr proc ctx (populate src/dst MAC and Ether type) ============= */
	size = sizeof(ipa_ioc_add_hdr_proc_ctx) + sizeof(ipa_hdr_proc_ctx_add);
	hdr_proc_ctx_table = (ipa_ioc_add_hdr_proc_ctx*)malloc(size);
	if(hdr_proc_ctx_table == NULL)
	{
		IPACMERR("Failed to allocate memory.\n");
		return IPACM_FAILURE;
	}
	memset(hdr_proc_ctx_table, 0, size);

	hdr_proc_ctx_table->commit = 1;
	hdr_proc_ctx_table->num_proc_ctxs = 1;
	hdr_proc_ctx = &hdr_proc_ctx_table->proc_ctx[0];

	hdr_proc_ctx->type = IPA_HDR_PROC_L2TP_HEADER_ADD;
	hdr_proc_ctx->hdr_hdl = *first_pass_hdr_hdl;
	hdr_proc_ctx->l2tp_params.hdr_add_param.eth_hdr_retained = 1;
	hdr_proc_ctx->l2tp_params.hdr_add_param.input_ip_version = iptype;
	hdr_proc_ctx->l2tp_params.hdr_add_param.output_ip_version = IPA_IP_v6;
	if(m_header.AddHeaderProcCtx(hdr_proc_ctx_table) == false)
	{
		IPACMERR("Failed to add hdr proc ctx with status: %d\n", hdr_proc_ctx_table->proc_ctx[0].status);
		free(hdr_proc_ctx_table);
		return IPACM_FAILURE;
	}
	*first_pass_hdr_proc_ctx_hdl = hdr_proc_ctx_table->proc_ctx[0].proc_ctx_hdl;
	IPACMDBG_H("Installed first pass hdr proc ctx: hdl %d\n", *first_pass_hdr_proc_ctx_hdl);
	free(hdr_proc_ctx_table);

	/* =========== install first pass rt rules (match dst MAC then doing UCP) ============= */
	*num_rt_hdl = each_client_rt_rule_count[iptype];
	size = sizeof(ipa_ioc_add_rt_rule) + (*num_rt_hdl) * sizeof(ipa_rt_rule_add);
	rt_rule_table = (ipa_ioc_add_rt_rule*)malloc(size);
	if (rt_rule_table == NULL)
	{
		IPACMERR("Failed to allocate memory.\n");
		return IPACM_FAILURE;
	}
	memset(rt_rule_table, 0, size);

	rt_rule_table->commit = 1;
	rt_rule_table->ip = iptype;
	rt_rule_table->num_rules = *num_rt_hdl;
	snprintf(rt_rule_table->rt_tbl_name, sizeof(rt_rule_table->rt_tbl_name), "l2tp");
	rt_rule_table->rt_tbl_name[IPA_RESOURCE_NAME_MAX-1] = 0;

	position = 0;
	for(i = 0; i < iface_query->num_tx_props; i++)
	{
		if(tx_prop->tx[i].ip == iptype)
		{
			if(position >= *num_rt_hdl || position >= MAX_NUM_PROP)
			{
				IPACMERR("Number of routing rules already exceeds limit.\n");
				free(rt_rule_table);
				return IPACM_FAILURE;
			}

			rt_rule = &rt_rule_table->rules[position];
			rt_rule->at_rear = false;
			rt_rule->status = -1;
			rt_rule->rt_rule_hdl = -1;
			rt_rule->rule.hashable = false;	//WLAN->ETH direction rules are set to non-hashable to keep consistent with the other direction
			rt_rule->rule.hdr_hdl = 0;
			rt_rule->rule.hdr_proc_ctx_hdl = *first_pass_hdr_proc_ctx_hdl;
			rt_rule->rule.dst = IPA_CLIENT_DUMMY_CONS;

			memcpy(&rt_rule->rule.attrib, &tx_prop->tx[i].attrib, sizeof(rt_rule->rule.attrib));
			if(peer_l2_hdr_type == IPA_HDR_L2_ETHERNET_II)
				rt_rule->rule.attrib.attrib_mask |= IPA_FLT_MAC_DST_ADDR_ETHER_II;
			else
				rt_rule->rule.attrib.attrib_mask |= IPA_FLT_MAC_DST_ADDR_802_3;
			memcpy(rt_rule->rule.attrib.dst_mac_addr, dst_mac, sizeof(rt_rule->rule.attrib.dst_mac_addr));
			memset(rt_rule->rule.attrib.dst_mac_addr_mask, 0xFF, sizeof(rt_rule->rule.attrib.dst_mac_addr_mask));
			position++;
		}
	}
	if(m_routing.AddRoutingRule(rt_rule_table) == false)
	{
		IPACMERR("Failed to add first pass rt rules.\n");
		free(rt_rule_table);
		return IPACM_FAILURE;
	}
	for(i = 0; i < position; i++)
	{
		first_pass_rt_rule_hdl[i] = rt_rule_table->rules[i].rt_rule_hdl;
	}
	free(rt_rule_table);

	/* =========== install second pass hdr (Ethernet header with L2TP tag = 18 bytes) ============= */
	if(*second_pass_hdr_hdl != 0)
	{
		IPACMDBG_H("Second pass hdr was added before.\n");
	}
	else
	{
		size = sizeof(ipa_ioc_add_hdr) + sizeof(ipa_hdr_add);
		hdr_table = (ipa_ioc_add_hdr*)malloc(size);
		if(hdr_table == NULL)
		{
			IPACMERR("Failed to allocate memory.\n");
			return IPACM_FAILURE;
		}
		memset(hdr_table, 0, size);

		hdr_table->commit = 1;
		hdr_table->num_hdrs = 1;
		hdr = &hdr_table->hdr[0];

		if(iptype == IPA_IP_v4)
		{
			snprintf(hdr->name, sizeof(hdr->name), "vlan_%d_v4", vlan_id);
		}
		else
		{
			snprintf(hdr->name, sizeof(hdr->name), "vlan_%d_v6", vlan_id);
		}
		hdr->type = IPA_HDR_L2_ETHERNET_II;
		hdr->is_partial = 0;
		for(i = 0; i < tx_prop->num_tx_props; i++)
		{
			if(tx_prop->tx[i].ip == IPA_IP_v6)
			{
				memset(&copy_hdr, 0, sizeof(copy_hdr));
				strlcpy(copy_hdr.name, tx_prop->tx[i].hdr_name,
					sizeof(copy_hdr.name));
				IPACMDBG_H("Header name: %s in tx:%d\n", copy_hdr.name, i);
				if(m_header.CopyHeader(&copy_hdr) == false)
				{
					IPACMERR("Failed to get partial header.\n");
					free(hdr_table);
					return IPACM_FAILURE;
				}
				IPACMDBG_H("Header length: %d\n", copy_hdr.hdr_len);
				hdr->hdr_len = copy_hdr.hdr_len;
				memcpy(hdr->hdr, copy_hdr.hdr, hdr->hdr_len);
				break;
			}
		}
		/* copy vlan client mac */
		memcpy(hdr->hdr + hdr->hdr_len - 18, vlan_client_mac, 6);
		hdr->hdr[hdr->hdr_len - 3] = (uint8_t)vlan_id & 0xFF;
		hdr->hdr[hdr->hdr_len - 4] = (uint8_t)(vlan_id >> 8) & 0xFF;

		if(m_header.AddHeader(hdr_table) == false)
		{
			IPACMERR("Failed to add hdr with status: %d\n", hdr->status);
			free(hdr_table);
			return IPACM_FAILURE;
		}
		*second_pass_hdr_hdl = hdr->hdr_hdl;
		IPACMDBG_H("Installed second pass hdr: hdl %d\n", *second_pass_hdr_hdl);
		free(hdr_table);
	}

	/* =========== install second pass rt rules (match VLAN interface IPv6 address at dst client side) ============= */
	if(second_pass_rt_rule_hdl[0] != 0)
	{
		IPACMDBG_H("Second pass rt rule was added before, return.\n");
		return IPACM_SUCCESS;
	}

	*num_rt_hdl = each_client_rt_rule_count[IPA_IP_v6];
	size = sizeof(ipa_ioc_add_rt_rule) + (*num_rt_hdl) * sizeof(ipa_rt_rule_add);
	rt_rule_table = (ipa_ioc_add_rt_rule*)malloc(size);
	if (rt_rule_table == NULL)
	{
		IPACMERR("Failed to allocate memory.\n");
		return IPACM_FAILURE;
	}
	memset(rt_rule_table, 0, size);

	rt_rule_table->commit = 1;
	rt_rule_table->ip = IPA_IP_v6;
	rt_rule_table->num_rules = *num_rt_hdl;
	snprintf(rt_rule_table->rt_tbl_name, sizeof(rt_rule_table->rt_tbl_name), "l2tp");
	rt_rule_table->rt_tbl_name[IPA_RESOURCE_NAME_MAX-1] = 0;

	position = 0;
	for(i = 0; i < iface_query->num_tx_props; i++)
	{
		if(tx_prop->tx[i].ip == IPA_IP_v6)
		{
			if(position >= *num_rt_hdl || position >= MAX_NUM_PROP)
			{
				IPACMERR("Number of routing rules already exceeds limit.\n");
				free(rt_rule_table);
				return IPACM_FAILURE;
			}

			rt_rule = &rt_rule_table->rules[position];
			rt_rule->at_rear = false;
			rt_rule->status = -1;
			rt_rule->rt_rule_hdl = -1;
			rt_rule->rule.hashable = false;	//WLAN->ETH direction rules are set to non-hashable to keep consistent with the other direction
			rt_rule->rule.hdr_hdl = *second_pass_hdr_hdl;
			rt_rule->rule.hdr_proc_ctx_hdl = 0;
			rt_rule->rule.dst = tx_prop->tx[i].dst_pipe;

			memcpy(&rt_rule->rule.attrib, &tx_prop->tx[i].attrib, sizeof(rt_rule->rule.attrib));
			rt_rule->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
			memcpy(rt_rule->rule.attrib.u.v6.dst_addr, vlan_client_ipv6_addr,
				sizeof(rt_rule->rule.attrib.u.v6.dst_addr));
			memset(rt_rule->rule.attrib.u.v6.dst_addr_mask, 0xFF, sizeof(rt_rule->rule.attrib.u.v6.dst_addr_mask));
			position++;
		}
	}
	if(m_routing.AddRoutingRule(rt_rule_table) == false)
	{
		IPACMERR("Failed to add second pass rt rules.\n");
		free(rt_rule_table);
		return IPACM_FAILURE;
	}
	for(i = 0; i < position; i++)
	{
		second_pass_rt_rule_hdl[i] = rt_rule_table->rules[i].rt_rule_hdl;
	}
	free(rt_rule_table);

	return IPACM_SUCCESS;
}

/* delete l2tp rt rule for l2tp client */
int IPACM_Lan::del_l2tp_rt_rule(ipa_ip_type iptype, uint32_t first_pass_hdr_hdl, uint32_t first_pass_hdr_proc_ctx_hdl,
	uint32_t second_pass_hdr_hdl, int num_rt_hdl, uint32_t *first_pass_rt_rule_hdl, uint32_t *second_pass_rt_rule_hdl)
{
	int i;

	if(num_rt_hdl < 0)
	{
		IPACMERR("Invalid num rt rule: %d\n", num_rt_hdl);
		return IPACM_FAILURE;
	}

	for(i = 0; i < num_rt_hdl; i++)
	{
		if(first_pass_rt_rule_hdl != NULL)
		{
			if(m_routing.DeleteRoutingHdl(first_pass_rt_rule_hdl[i], iptype) == false)
			{
				return IPACM_FAILURE;
			}
		}
		if(second_pass_rt_rule_hdl != NULL)
		{
			if(m_routing.DeleteRoutingHdl(second_pass_rt_rule_hdl[i], IPA_IP_v6) == false)
			{
				return IPACM_FAILURE;
			}
		}
	}

	if(first_pass_hdr_proc_ctx_hdl != 0)
	{
		if(m_header.DeleteHeaderProcCtx(first_pass_hdr_proc_ctx_hdl) == false)
		{
			return IPACM_FAILURE;
		}
	}

	if(first_pass_hdr_hdl != 0)
	{
		if(m_header.DeleteHeaderHdl(first_pass_hdr_hdl) == false)
		{
			return IPACM_FAILURE;
		}
	}
	if(second_pass_hdr_hdl != 0)
	{
		if(m_header.DeleteHeaderHdl(second_pass_hdr_hdl) == false)
		{
			return IPACM_FAILURE;
		}
	}

	return IPACM_SUCCESS;
}

/* add l2tp rt rule for non l2tp client */
int IPACM_Lan::add_l2tp_rt_rule(ipa_ip_type iptype, uint8_t *dst_mac, uint32_t *hdr_proc_ctx_hdl,
	int *num_rt_hdl, uint32_t *rt_rule_hdl)
{
	int i, size, position;
	ipa_ioc_add_hdr_proc_ctx *hdr_proc_ctx_table;
	ipa_hdr_proc_ctx_add *hdr_proc_ctx;
	ipa_ioc_add_rt_rule* rt_rule_table;
	ipa_rt_rule_add *rt_rule;
	ipa_ioc_get_hdr hdr;

	if(tx_prop == NULL)
	{
		IPACMERR("No tx prop.\n");
		return IPACM_FAILURE;
	}

	memset(&hdr, 0, sizeof(hdr));
	for(i = 0; i < tx_prop->num_tx_props; i++)
	{
		if(tx_prop->tx[i].ip == iptype)
		{
			strlcpy(hdr.name, tx_prop->tx[i].hdr_name,
				sizeof(hdr.name));
			break;
		}
	}
	if(m_header.GetHeaderHandle(&hdr) == false)
	{
		IPACMERR("Failed to get template hdr hdl.\n");
		return IPACM_FAILURE;
	}

	/* =========== install hdr proc ctx (uc needs to remove IPv6 + L2TP + inner ETH header = 62 bytes) ============= */
	if(*hdr_proc_ctx_hdl != 0)
	{
		IPACMDBG_H("Hdr proc ctx was added before.\n");
	}
	else
	{
		size = sizeof(ipa_ioc_add_hdr_proc_ctx) + sizeof(ipa_hdr_proc_ctx_add);
		hdr_proc_ctx_table = (ipa_ioc_add_hdr_proc_ctx*)malloc(size);
		if(hdr_proc_ctx_table == NULL)
		{
			IPACMERR("Failed to allocate memory.\n");
			return IPACM_FAILURE;
		}
		memset(hdr_proc_ctx_table, 0, size);

		hdr_proc_ctx_table->commit = 1;
		hdr_proc_ctx_table->num_proc_ctxs = 1;
		hdr_proc_ctx = &hdr_proc_ctx_table->proc_ctx[0];

		hdr_proc_ctx->type = IPA_HDR_PROC_L2TP_HEADER_REMOVE;
		hdr_proc_ctx->hdr_hdl = hdr.hdl;
		hdr_proc_ctx->l2tp_params.hdr_remove_param.hdr_len_remove = 62;
		hdr_proc_ctx->l2tp_params.hdr_remove_param.eth_hdr_retained = 1;
		hdr_proc_ctx->l2tp_params.is_dst_pipe_valid = 1;
		hdr_proc_ctx->l2tp_params.dst_pipe = tx_prop->tx[0].dst_pipe;
		IPACMDBG_H("Header_remove: hdr len %d, hdr retained %d, dst client: %d\n",
			hdr_proc_ctx->l2tp_params.hdr_remove_param.hdr_len_remove,
			hdr_proc_ctx->l2tp_params.hdr_remove_param.eth_hdr_retained,
			hdr_proc_ctx->l2tp_params.dst_pipe);
		if(m_header.AddHeaderProcCtx(hdr_proc_ctx_table) == false)
		{
			IPACMERR("Failed to add hdr proc ctx with status: %d\n", hdr_proc_ctx_table->proc_ctx[0].status);
			free(hdr_proc_ctx_table);
			return IPACM_FAILURE;
		}
		*hdr_proc_ctx_hdl = hdr_proc_ctx_table->proc_ctx[0].proc_ctx_hdl;
		IPACMDBG_H("Installed hdr proc ctx: hdl %d\n", *hdr_proc_ctx_hdl);
		free(hdr_proc_ctx_table);
	}

	/* =========== install rt rules (match dst MAC within 62 bytes header) ============= */
	*num_rt_hdl = each_client_rt_rule_count[iptype];
	size = sizeof(ipa_ioc_add_rt_rule) + (*num_rt_hdl) * sizeof(ipa_rt_rule_add);
	rt_rule_table = (ipa_ioc_add_rt_rule*)malloc(size);
	if (rt_rule_table == NULL)
	{
		IPACMERR("Failed to allocate memory.\n");
		return IPACM_FAILURE;
	}
	memset(rt_rule_table, 0, size);

	rt_rule_table->commit = 1;
	rt_rule_table->ip = iptype;
	rt_rule_table->num_rules = *num_rt_hdl;
	snprintf(rt_rule_table->rt_tbl_name, sizeof(rt_rule_table->rt_tbl_name), "l2tp");
	rt_rule_table->rt_tbl_name[IPA_RESOURCE_NAME_MAX-1] = 0;

	position = 0;
	for(i = 0; i < iface_query->num_tx_props; i++)
	{
		if(tx_prop->tx[i].ip == iptype)
		{
			if(position >= *num_rt_hdl || position >= MAX_NUM_PROP)
			{
				IPACMERR("Number of routing rules already exceeds limit.\n");
				free(rt_rule_table);
				return IPACM_FAILURE;
			}

			rt_rule = &rt_rule_table->rules[position];
			rt_rule->at_rear = false;
			rt_rule->status = -1;
			rt_rule->rt_rule_hdl = -1;
			rt_rule->rule.hashable = false;	//ETH->WLAN direction rules need to be non-hashable due to encapsulation

			rt_rule->rule.hdr_hdl = 0;
			rt_rule->rule.hdr_proc_ctx_hdl = *hdr_proc_ctx_hdl;
			rt_rule->rule.dst = tx_prop->tx[i].dst_pipe;

			memcpy(&rt_rule->rule.attrib, &tx_prop->tx[i].attrib, sizeof(rt_rule->rule.attrib));

			rt_rule->rule.attrib.attrib_mask |= IPA_FLT_MAC_DST_ADDR_L2TP;
			memset(rt_rule->rule.attrib.dst_mac_addr_mask, 0xFF, sizeof(rt_rule->rule.attrib.dst_mac_addr_mask));
			memcpy(rt_rule->rule.attrib.dst_mac_addr, dst_mac, sizeof(rt_rule->rule.attrib.dst_mac_addr));

			position++;
		}
	}
	if(m_routing.AddRoutingRule(rt_rule_table) == false)
	{
		IPACMERR("Failed to add first pass rt rules.\n");
		free(rt_rule_table);
		return IPACM_FAILURE;
	}
	for(i = 0; i < position; i++)
		rt_rule_hdl[i] = rt_rule_table->rules[i].rt_rule_hdl;

	free(rt_rule_table);
	return IPACM_SUCCESS;
}

int IPACM_Lan::del_l2tp_rt_rule(ipa_ip_type iptype, int num_rt_hdl, uint32_t *rt_rule_hdl)
{
	int i;

	if(num_rt_hdl < 0)
	{
		IPACMERR("Invalid num rt rule: %d\n", num_rt_hdl);
		return IPACM_FAILURE;
	}

	for(i = 0; i < num_rt_hdl; i++)
	{
		if(m_routing.DeleteRoutingHdl(rt_rule_hdl[i], iptype) == false)
		{
			return IPACM_FAILURE;
		}
	}

	return IPACM_SUCCESS;
}

/* add l2tp flt rule on l2tp interface */
int IPACM_Lan::add_l2tp_flt_rule(uint8_t *dst_mac, uint32_t *flt_rule_hdl)
{
	int len;
	int fd_ipa;
	struct ipa_flt_rule_add flt_rule_entry;
	struct ipa_ioc_add_flt_rule_after *pFilteringTable = NULL;
	ipa_ioc_get_rt_tbl rt_tbl;

#ifdef FEATURE_IPA_V3
	if (rx_prop == NULL || tx_prop == NULL)
	{
		IPACMDBG_H("No rx or tx properties registered for iface %s\n", dev_name);
		return IPACM_FAILURE;
	}

	len = sizeof(struct ipa_ioc_add_flt_rule_after) + sizeof(struct ipa_flt_rule_add);
	pFilteringTable = (struct ipa_ioc_add_flt_rule_after*)malloc(len);
	if (!pFilteringTable)
	{
		IPACMERR("Failed to allocate ipa_ioc_add_flt_rule_after memory...\n");
		return IPACM_FAILURE;
	}
	memset(pFilteringTable, 0, len);

	pFilteringTable->commit = 1;
	pFilteringTable->ep = rx_prop->rx[0].src_pipe;
	pFilteringTable->ip = IPA_IP_v6;
	pFilteringTable->num_rules = 1;
	pFilteringTable->add_after_hdl = eth_bridge_flt_rule_offset[IPA_IP_v6];

	fd_ipa = open(IPA_DEVICE_NAME, O_RDWR);
	if(fd_ipa == 0)
	{
		IPACMERR("Failed to open %s\n",IPA_DEVICE_NAME);
		free(pFilteringTable);
		return IPACM_FAILURE;
	}

	rt_tbl.ip = IPA_IP_v6;
	snprintf(rt_tbl.name, sizeof(rt_tbl.name), "l2tp");
	rt_tbl.name[IPA_RESOURCE_NAME_MAX-1] = '\0';
	IPACMDBG_H("This flt rule points to rt tbl %s.\n", rt_tbl.name);
	if(m_routing.GetRoutingTable(&rt_tbl) == false)
	{
		IPACMERR("Failed to get routing table from name\n");
		free(pFilteringTable);
		close(fd_ipa);
		return IPACM_FAILURE;
	}

	memset(&flt_rule_entry, 0, sizeof(flt_rule_entry));
	flt_rule_entry.at_rear = 1;

	flt_rule_entry.rule.retain_hdr = 0;
	flt_rule_entry.rule.to_uc = 0;
	flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
	flt_rule_entry.rule.eq_attrib_type = 0;
	flt_rule_entry.rule.rt_tbl_hdl = rt_tbl.hdl;
	flt_rule_entry.rule.hashable = false;	//ETH->WLAN direction rules need to be non-hashable due to encapsulation

	memcpy(&flt_rule_entry.rule.attrib, &rx_prop->rx[0].attrib, sizeof(flt_rule_entry.rule.attrib));

	/* flt rule is matching dst MAC within 62 bytes header */
	flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_MAC_DST_ADDR_L2TP;
	memset(flt_rule_entry.rule.attrib.dst_mac_addr_mask, 0xFF, sizeof(flt_rule_entry.rule.attrib.dst_mac_addr_mask));
	memcpy(flt_rule_entry.rule.attrib.dst_mac_addr, dst_mac, sizeof(flt_rule_entry.rule.attrib.dst_mac_addr));

	memcpy(&(pFilteringTable->rules[0]), &flt_rule_entry, sizeof(flt_rule_entry));
	if(m_filtering.AddFilteringRuleAfter(pFilteringTable) == false)
	{
		IPACMERR("Failed to add client filtering rules.\n");
		free(pFilteringTable);
		close(fd_ipa);
		return IPACM_FAILURE;
	}
	*flt_rule_hdl = pFilteringTable->rules[0].flt_rule_hdl;

	free(pFilteringTable);
	close(fd_ipa);
#endif
	return IPACM_SUCCESS;
}

/* delete l2tp flt rule on l2tp interface */
int IPACM_Lan::del_l2tp_flt_rule(uint32_t flt_rule_hdl)
{
	if(m_filtering.DeleteFilteringHdls(&flt_rule_hdl, IPA_IP_v6, 1) == false)
	{
		return IPACM_FAILURE;
	}

	return IPACM_SUCCESS;
}

/* add l2tp flt rule on non l2tp interface */
int IPACM_Lan::add_l2tp_flt_rule(ipa_ip_type iptype, uint8_t *dst_mac, uint32_t *vlan_client_ipv6_addr,
	uint32_t *first_pass_flt_rule_hdl, uint32_t *second_pass_flt_rule_hdl)
{
	int len;
	struct ipa_flt_rule_add flt_rule_entry;
	struct ipa_ioc_add_flt_rule_after *pFilteringTable = NULL;
	ipa_ioc_get_rt_tbl rt_tbl;

#ifdef FEATURE_IPA_V3
	if (rx_prop == NULL || tx_prop == NULL)
	{
		IPACMDBG_H("No rx or tx properties registered for iface %s\n", dev_name);
		return IPACM_FAILURE;
	}

	IPACMDBG_H("Dst client MAC 0x%02x%02x%02x%02x%02x%02x.\n", dst_mac[0], dst_mac[1],
		dst_mac[2], dst_mac[3], dst_mac[4], dst_mac[5]);

	len = sizeof(struct ipa_ioc_add_flt_rule_after) + sizeof(struct ipa_flt_rule_add);
	pFilteringTable = (struct ipa_ioc_add_flt_rule_after*)malloc(len);
	if (!pFilteringTable)
	{
		IPACMERR("Failed to allocate ipa_ioc_add_flt_rule_after memory...\n");
		return IPACM_FAILURE;
	}
	memset(pFilteringTable, 0, len);

	pFilteringTable->commit = 1;
	pFilteringTable->ep = rx_prop->rx[0].src_pipe;
	pFilteringTable->ip = iptype;
	pFilteringTable->num_rules = 1;
	pFilteringTable->add_after_hdl = eth_bridge_flt_rule_offset[iptype];

	/* =========== add first pass flt rule (match dst MAC) ============= */
	rt_tbl.ip = iptype;
	snprintf(rt_tbl.name, sizeof(rt_tbl.name), "l2tp");
	IPACMDBG_H("This flt rule points to rt tbl %s.\n", rt_tbl.name);

	if(m_routing.GetRoutingTable(&rt_tbl) == false)
	{
		IPACMERR("Failed to get routing table.\n");
		return IPACM_FAILURE;
	}

	memset(&flt_rule_entry, 0, sizeof(flt_rule_entry));
	flt_rule_entry.at_rear = 1;

	flt_rule_entry.rule.retain_hdr = 0;
	flt_rule_entry.rule.to_uc = 0;
	flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
	flt_rule_entry.rule.eq_attrib_type = 0;
	flt_rule_entry.rule.rt_tbl_hdl = rt_tbl.hdl;
	flt_rule_entry.rule.hashable = false;	//WLAN->ETH direction rules are set to non-hashable to keep consistent with the other direction

	memcpy(&flt_rule_entry.rule.attrib, &rx_prop->rx[0].attrib, sizeof(flt_rule_entry.rule.attrib));
	if(tx_prop->tx[0].hdr_l2_type == IPA_HDR_L2_ETHERNET_II)
	{
		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_MAC_DST_ADDR_ETHER_II;
	}
	else
	{
		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_MAC_DST_ADDR_802_3;
	}

	memcpy(flt_rule_entry.rule.attrib.dst_mac_addr, dst_mac, sizeof(flt_rule_entry.rule.attrib.dst_mac_addr));
	memset(flt_rule_entry.rule.attrib.dst_mac_addr_mask, 0xFF, sizeof(flt_rule_entry.rule.attrib.dst_mac_addr_mask));

	memcpy(&(pFilteringTable->rules[0]), &flt_rule_entry, sizeof(flt_rule_entry));
	if (false == m_filtering.AddFilteringRuleAfter(pFilteringTable))
	{
		IPACMERR("Failed to add first pass filtering rules.\n");
		free(pFilteringTable);
		return IPACM_FAILURE;
	}
	*first_pass_flt_rule_hdl = pFilteringTable->rules[0].flt_rule_hdl;

	/* =========== add second pass flt rule (match VLAN interface IPv6 address at client side) ============= */
	if(*second_pass_flt_rule_hdl != 0)
	{
		IPACMDBG_H("Second pass flt rule was added before, return.\n");
		free(pFilteringTable);
		return IPACM_SUCCESS;
	}

	rt_tbl.ip = IPA_IP_v6;
	snprintf(rt_tbl.name, sizeof(rt_tbl.name), "l2tp");
	IPACMDBG_H("This flt rule points to rt tbl %s.\n", rt_tbl.name);

	if(m_routing.GetRoutingTable(&rt_tbl) == false)
	{
		IPACMERR("Failed to get routing table.\n");
		return IPACM_FAILURE;
	}

	pFilteringTable->ip = IPA_IP_v6;
	pFilteringTable->add_after_hdl = eth_bridge_flt_rule_offset[IPA_IP_v6];

	memset(&flt_rule_entry, 0, sizeof(flt_rule_entry));
	flt_rule_entry.at_rear = 1;

	flt_rule_entry.rule.retain_hdr = 0;
	flt_rule_entry.rule.to_uc = 0;
	flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
	flt_rule_entry.rule.eq_attrib_type = 0;
	flt_rule_entry.rule.rt_tbl_hdl = rt_tbl.hdl;
	flt_rule_entry.rule.hashable = false;	//WLAN->ETH direction rules are set to non-hashable to keep consistent with the other direction

	memcpy(&flt_rule_entry.rule.attrib, &rx_prop->rx[0].attrib, sizeof(flt_rule_entry.rule.attrib));
	flt_rule_entry.rule.attrib.attrib_mask = IPA_FLT_DST_ADDR;

	memcpy(flt_rule_entry.rule.attrib.u.v6.dst_addr, vlan_client_ipv6_addr, sizeof(flt_rule_entry.rule.attrib.u.v6.dst_addr));
	memset(flt_rule_entry.rule.attrib.u.v6.dst_addr_mask, 0xFF, sizeof(flt_rule_entry.rule.attrib.u.v6.dst_addr_mask));

	memcpy(&(pFilteringTable->rules[0]), &flt_rule_entry, sizeof(flt_rule_entry));
	if (false == m_filtering.AddFilteringRuleAfter(pFilteringTable))
	{
		IPACMERR("Failed to add client filtering rules.\n");
		free(pFilteringTable);
		return IPACM_FAILURE;
	}
	*second_pass_flt_rule_hdl = pFilteringTable->rules[0].flt_rule_hdl;

	free(pFilteringTable);
#endif
	return IPACM_SUCCESS;
}

/* delete l2tp flt rule on non l2tp interface */
int IPACM_Lan::del_l2tp_flt_rule(ipa_ip_type iptype, uint32_t first_pass_flt_rule_hdl, uint32_t second_pass_flt_rule_hdl)
{
	if(first_pass_flt_rule_hdl != 0)
	{
		if(m_filtering.DeleteFilteringHdls(&first_pass_flt_rule_hdl, iptype, 1) == false)
		{
			return IPACM_FAILURE;
		}
	}

	if(second_pass_flt_rule_hdl != 0)
	{
		if(m_filtering.DeleteFilteringHdls(&second_pass_flt_rule_hdl, iptype, 1) == false)
		{
			return IPACM_FAILURE;
		}
	}

	return IPACM_SUCCESS;
}
#endif
bool IPACM_Lan::is_unique_local_ipv6_addr(uint32_t* ipv6_addr)
{
	uint32_t ipv6_unique_local_prefix, ipv6_unique_local_prefix_mask;

	if(ipv6_addr == NULL)
	{
		IPACMERR("IPv6 address is empty.\n");
		return false;
	}
	IPACMDBG_H("Get ipv6 address with first word 0x%08x.\n", ipv6_addr[0]);

	ipv6_unique_local_prefix = 0xFD000000;
	ipv6_unique_local_prefix_mask = 0xFF000000;
	if((ipv6_addr[0] & ipv6_unique_local_prefix_mask) == (ipv6_unique_local_prefix & ipv6_unique_local_prefix_mask))
	{
		IPACMDBG_H("This IPv6 address is unique local IPv6 address.\n");
		return true;
	}
	return false;
}


/* add tcp syn flt rule */
int IPACM_Lan::add_tcp_syn_flt_rule(ipa_ip_type iptype)
{
	int len;
	struct ipa_flt_rule_add flt_rule_entry;
	ipa_ioc_add_flt_rule *m_pFilteringTable;

	if(rx_prop == NULL)
	{
		IPACMDBG_H("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	len = sizeof(struct ipa_ioc_add_flt_rule) + sizeof(struct ipa_flt_rule_add);
	m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)malloc(len);
	if(!m_pFilteringTable)
	{
		PERROR("Not enough memory.\n");
		return IPACM_FAILURE;
	}
	memset(m_pFilteringTable, 0, len);

	m_pFilteringTable->commit = 1;
	m_pFilteringTable->ep = rx_prop->rx[0].src_pipe;
	m_pFilteringTable->global = false;
	m_pFilteringTable->ip = iptype;
	m_pFilteringTable->num_rules = 1;

	memset(&flt_rule_entry, 0, sizeof(flt_rule_entry));
	flt_rule_entry.at_rear = true;
	flt_rule_entry.rule.retain_hdr = 1;
	flt_rule_entry.flt_rule_hdl = -1;
	flt_rule_entry.status = -1;
	flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;

	memcpy(&flt_rule_entry.rule.attrib, &rx_prop->rx[0].attrib,
		sizeof(flt_rule_entry.rule.attrib));
	flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_TCP_SYN;
	if(iptype == IPA_IP_v4)
	{
		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_PROTOCOL;
		flt_rule_entry.rule.attrib.u.v4.protocol = 6;
	}
	else
	{
		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_NEXT_HDR;
		flt_rule_entry.rule.attrib.u.v6.next_hdr = 6;
	}

	memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(flt_rule_entry));

	if(false == m_filtering.AddFilteringRule(m_pFilteringTable))
	{
		IPACMERR("Error Adding RuleTable(0) to Filtering, aborting...\n");
		free(m_pFilteringTable);
		return IPACM_FAILURE;
	}

	tcp_syn_flt_rule_hdl[iptype] = m_pFilteringTable->rules[0].flt_rule_hdl;
	free(m_pFilteringTable);
	return IPACM_SUCCESS;
}

/* add tcp syn flt rule for l2tp interface*/
int IPACM_Lan::add_tcp_syn_flt_rule_l2tp(ipa_ip_type inner_ip_type)
{
	int len;
	struct ipa_flt_rule_add flt_rule_entry;
	ipa_ioc_add_flt_rule *m_pFilteringTable;

	if(rx_prop == NULL)
	{
		IPACMDBG_H("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	len = sizeof(struct ipa_ioc_add_flt_rule) + sizeof(struct ipa_flt_rule_add);
	m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)malloc(len);
	if(!m_pFilteringTable)
	{
		PERROR("Not enough memory.\n");
		return IPACM_FAILURE;
	}
	memset(m_pFilteringTable, 0, len);

	m_pFilteringTable->commit = 1;
	m_pFilteringTable->ep = rx_prop->rx[0].src_pipe;
	m_pFilteringTable->global = false;
	m_pFilteringTable->ip = IPA_IP_v6;
	m_pFilteringTable->num_rules = 1;

	memset(&flt_rule_entry, 0, sizeof(flt_rule_entry));
	flt_rule_entry.at_rear = true;
	flt_rule_entry.rule.retain_hdr = 1;
	flt_rule_entry.flt_rule_hdl = -1;
	flt_rule_entry.status = -1;
	flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;

	memcpy(&flt_rule_entry.rule.attrib, &rx_prop->rx[0].attrib,
		sizeof(flt_rule_entry.rule.attrib));
	flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_TCP_SYN_L2TP;
	if(inner_ip_type == IPA_IP_v4)
	{
		flt_rule_entry.rule.attrib.ether_type = 0x0800;
	}
	else
	{
		flt_rule_entry.rule.attrib.ether_type = 0x86dd;
	}

	memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(flt_rule_entry));

	if(false == m_filtering.AddFilteringRule(m_pFilteringTable))
	{
		IPACMERR("Error Adding RuleTable(0) to Filtering, aborting...\n");
		free(m_pFilteringTable);
		return IPACM_FAILURE;
	}

	tcp_syn_flt_rule_hdl[inner_ip_type] = m_pFilteringTable->rules[0].flt_rule_hdl;
	free(m_pFilteringTable);
	return IPACM_SUCCESS;
}

#ifdef FEATURE_L2TP_E2E
/* install l2tp dl rules */
int IPACM_Lan::install_l2tp_dl_rules(ipacm_event_data_all *data, int index)
{
	int i, size;
	ipa_ioc_add_hdr *hdr_table;
	ipa_hdr_add *hdr;
	ipa_ioc_add_hdr_proc_ctx *hdr_proc_ctx_table;
	ipa_hdr_proc_ctx_add *hdr_proc_ctx;
	ipa_ioc_add_rt_rule* rt_rule_table;
	ipa_rt_rule_add *rt_rule;
	ipa_ioc_copy_hdr copy_hdr;
	l2tp_vlan_mapping_info info;
	uint32_t vlan_iface_ipv6_addr_network[4], vlan_client_ipv6_addr_network[4];
	l2tp_client_info new_client_info;
	ipacm_cmd_q_data evt_data;
	ipacm_event_data_all *data_all;

	if(tx_prop == NULL)
	{
		IPACMERR("No tx prop.\n");
		return IPACM_FAILURE;
	}

	if(IPACM_Iface::ipacmcfg->get_vlan_l2tp_mapping(data->iface_name, info) == IPACM_FAILURE)
	{
		IPACMERR("Fail to get vlan-l2tp mapping.\n");
		return IPACM_FAILURE;
	}

	get_client_memptr(eth_client, index)->v4_addr = data->ipv4_addr;
	is_l2tp_iface = true;
	memcpy(get_client_memptr(eth_client, index)->mac, data->mac_addr,
		sizeof(get_client_memptr(eth_client, index)->mac));

	/* =========== install first pass hdr template (IPv6 + L2TP + inner ETH header = 62 bytes) ============= */
	size = sizeof(ipa_ioc_add_hdr) + sizeof(ipa_hdr_add);
	hdr_table = (ipa_ioc_add_hdr*)malloc(size);
	if(hdr_table == NULL)
	{
		IPACMERR("Failed to allocate memory.\n");
		return IPACM_FAILURE;
	}
	memset(hdr_table, 0, size);

	hdr_table->commit = 1;
	hdr_table->num_hdrs = 1;
	hdr = &hdr_table->hdr[0];

	snprintf(hdr->name, sizeof(hdr->name), "vlan_%d_l2tp_%d_v4", info.vlan_id, info.l2tp_session_id);

	hdr->hdr_len = 62;
	hdr->type = IPA_HDR_L2_ETHERNET_II;
	hdr->is_partial = 0;

	hdr->hdr[0] = 0x60;	/* version */
	hdr->hdr[6] = 0x73; /* next header = L2TP */
	hdr->hdr[7] = 0x40; /* hop limit = 64 */
	for(i = 0; i < 4; i++)
	{
		vlan_iface_ipv6_addr_network[i] = htonl(info.vlan_iface_ipv6_addr[i]);
		vlan_client_ipv6_addr_network[i] = htonl(info.vlan_client_ipv6_addr[i]);
	}
	memcpy(hdr->hdr + 8, vlan_iface_ipv6_addr_network, 16); /* source IPv6 addr */
	memcpy(hdr->hdr + 24, vlan_client_ipv6_addr_network, 16); /* dest IPv6 addr */
	hdr->hdr[43] = (uint8_t)(info.l2tp_session_id & 0xFF); /* l2tp header */
	hdr->hdr[42] = (uint8_t)(info.l2tp_session_id >> 8 & 0xFF);
	hdr->hdr[41] = (uint8_t)(info.l2tp_session_id >> 16 & 0xFF);
	hdr->hdr[40] = (uint8_t)(info.l2tp_session_id >> 24 & 0xFF);
	/* inner ETH header */
	memcpy(hdr->hdr + 48, data->mac_addr, 6); /* dst mac */
	hdr->hdr[60] = 0x08; /* Ether type */
	hdr->hdr[61] = 0x00;

	if(m_header.AddHeader(hdr_table) == false)
	{
		IPACMERR("Failed to add hdr with status: %d\n", hdr_table->hdr[0].status);
		free(hdr_table);
		return IPACM_FAILURE;
	}
	get_client_memptr(eth_client, index)->dl_first_pass_hdr_hdl = hdr_table->hdr[0].hdr_hdl;
	IPACMDBG_H("Installed first pass hdr: hdl %d\n", hdr_table->hdr[0].hdr_hdl);
	free(hdr_table);

	/* =========== install first pass hdr proc ctx ============= */
	size = sizeof(ipa_ioc_add_hdr_proc_ctx) + sizeof(ipa_hdr_proc_ctx_add);
	hdr_proc_ctx_table = (ipa_ioc_add_hdr_proc_ctx*)malloc(size);
	if(hdr_proc_ctx_table == NULL)
	{
		IPACMERR("Failed to allocate memory.\n");
		return IPACM_FAILURE;
	}
	memset(hdr_proc_ctx_table, 0, size);

	hdr_proc_ctx_table->commit = 1;
	hdr_proc_ctx_table->num_proc_ctxs = 1;
	hdr_proc_ctx = &hdr_proc_ctx_table->proc_ctx[0];

	hdr_proc_ctx->type = IPA_HDR_PROC_L2TP_HEADER_ADD;
	hdr_proc_ctx->hdr_hdl = get_client_memptr(eth_client, index)->dl_first_pass_hdr_hdl;
	hdr_proc_ctx->l2tp_params.hdr_add_param.eth_hdr_retained = 0;
	hdr_proc_ctx->l2tp_params.hdr_add_param.input_ip_version = IPA_IP_v4;
	hdr_proc_ctx->l2tp_params.hdr_add_param.output_ip_version = IPA_IP_v6;
	if(m_header.AddHeaderProcCtx(hdr_proc_ctx_table) == false)
	{
		IPACMERR("Failed to add hdr proc ctx with status: %d\n", hdr_proc_ctx_table->proc_ctx[0].status);
		free(hdr_proc_ctx_table);
		return IPACM_FAILURE;
	}
	get_client_memptr(eth_client, index)->dl_first_pass_hdr_proc_ctx_hdl =
		hdr_proc_ctx_table->proc_ctx[0].proc_ctx_hdl;
	IPACMDBG_H("Installed first pass hdr proc ctx: hdl %d\n", hdr_proc_ctx_table->proc_ctx[0].proc_ctx_hdl);
	free(hdr_proc_ctx_table);

	/* =========== install first pass rt rules (match dst MAC then doing UCP) ============= */
	size = sizeof(ipa_ioc_add_rt_rule) + sizeof(ipa_rt_rule_add);
	rt_rule_table = (ipa_ioc_add_rt_rule*)malloc(size);
	if (rt_rule_table == NULL)
	{
		IPACMERR("Failed to allocate memory.\n");
		return IPACM_FAILURE;
	}
	memset(rt_rule_table, 0, size);

	rt_rule_table->commit = 1;
	rt_rule_table->ip = IPA_IP_v4;
	rt_rule_table->num_rules = 1;

	strlcpy(rt_rule_table->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_lan_v4.name,
		sizeof(rt_rule_table->rt_tbl_name));
	rt_rule_table->rt_tbl_name[IPA_RESOURCE_NAME_MAX-1] = 0;

	rt_rule = &rt_rule_table->rules[0];
	for(i = 0; i < iface_query->num_tx_props; i++)
	{
		if(tx_prop->tx[i].ip == IPA_IP_v4)
		{
			rt_rule->at_rear = false;
			rt_rule->status = -1;
			rt_rule->rt_rule_hdl = -1;
			rt_rule->rule.hashable = true;
			rt_rule->rule.hdr_hdl = 0;
			rt_rule->rule.hdr_proc_ctx_hdl =
				get_client_memptr(eth_client, index)->dl_first_pass_hdr_proc_ctx_hdl;
			rt_rule->rule.dst = IPA_CLIENT_DUMMY_CONS;

			memcpy(&rt_rule->rule.attrib, &tx_prop->tx[i].attrib, sizeof(rt_rule->rule.attrib));
			rt_rule->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
			rt_rule->rule.attrib.u.v4.dst_addr = data->ipv4_addr;
			rt_rule->rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;
			break;
		}
	}
	if(m_routing.AddRoutingRule(rt_rule_table) == false)
	{
		IPACMERR("Failed to add first pass rt rules.\n");
		free(rt_rule_table);
		return IPACM_FAILURE;
	}
	get_client_memptr(eth_client, index)->dl_first_pass_rt_rule_hdl =
		rt_rule_table->rules[0].rt_rule_hdl;
	free(rt_rule_table);

	/* =========== install second pass hdr (Ethernet header with L2TP tag = 18 bytes) ============= */
	size = sizeof(ipa_ioc_add_hdr) + sizeof(ipa_hdr_add);
	hdr_table = (ipa_ioc_add_hdr*)malloc(size);
	if(hdr_table == NULL)
	{
		IPACMERR("Failed to allocate memory.\n");
		return IPACM_FAILURE;
	}
	memset(hdr_table, 0, size);

	hdr_table->commit = 1;
	hdr_table->num_hdrs = 1;
	hdr = &hdr_table->hdr[0];

	snprintf(hdr->name, sizeof(hdr->name), "vlan_%d_v6", info.vlan_id);

	hdr->type = IPA_HDR_L2_ETHERNET_II;
	hdr->is_partial = 0;
	for(i = 0; i < tx_prop->num_tx_props; i++)
	{
		if(tx_prop->tx[i].ip == IPA_IP_v6)
		{
			memset(&copy_hdr, 0, sizeof(copy_hdr));
			strlcpy(copy_hdr.name, tx_prop->tx[i].hdr_name,
				sizeof(copy_hdr.name));
			IPACMDBG_H("Header name: %s in tx:%d\n", copy_hdr.name, i);
			if(m_header.CopyHeader(&copy_hdr) == false)
			{
				IPACMERR("Failed to get partial header.\n");
				free(hdr_table);
				return IPACM_FAILURE;
			}
			IPACMDBG_H("Header length: %d\n", copy_hdr.hdr_len);
			hdr->hdr_len = copy_hdr.hdr_len;
			memcpy(hdr->hdr, copy_hdr.hdr, hdr->hdr_len);
			break;
		}
	}
	/* copy vlan client mac */
	memcpy(hdr->hdr, info.vlan_client_mac, 6);
	hdr->hdr[hdr->hdr_len - 3] = (uint8_t)info.vlan_id & 0xFF;
	hdr->hdr[hdr->hdr_len - 4] = (uint8_t)(info.vlan_id >> 8) & 0xFF;

	if(m_header.AddHeader(hdr_table) == false)
	{
		IPACMERR("Failed to add hdr with status: %d\n", hdr->status);
		free(hdr_table);
		return IPACM_FAILURE;
	}
	get_client_memptr(eth_client, index)->dl_second_pass_hdr_hdl = hdr->hdr_hdl;
	IPACMDBG_H("Installed second pass hdr: hdl %d\n",
		get_client_memptr(eth_client, index)->dl_second_pass_hdr_hdl);
	free(hdr_table);

	/* =========== install second pass rt rules (match VLAN interface IPv6 address at dst client side) ============= */
	size = sizeof(ipa_ioc_add_rt_rule) + sizeof(ipa_rt_rule_add);
	rt_rule_table = (ipa_ioc_add_rt_rule*)malloc(size);
	if (rt_rule_table == NULL)
	{
		IPACMERR("Failed to allocate memory.\n");
		return IPACM_FAILURE;
	}
	memset(rt_rule_table, 0, size);

	rt_rule_table->commit = 1;
	rt_rule_table->ip = IPA_IP_v6;
	rt_rule_table->num_rules = 1;

	strlcpy(rt_rule_table->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_wan_v6.name, sizeof(rt_rule_table->rt_tbl_name));
	rt_rule_table->rt_tbl_name[IPA_RESOURCE_NAME_MAX-1] = '\0';

	rt_rule = &rt_rule_table->rules[0];
	for(i = 0; i < iface_query->num_tx_props; i++)
	{
		if(tx_prop->tx[i].ip == IPA_IP_v6)
		{
			rt_rule->at_rear = false;
			rt_rule->status = -1;
			rt_rule->rt_rule_hdl = -1;
			rt_rule->rule.hashable = true;
			rt_rule->rule.hdr_hdl = get_client_memptr(eth_client, index)->dl_second_pass_hdr_hdl;
			rt_rule->rule.hdr_proc_ctx_hdl = 0;
			rt_rule->rule.dst = tx_prop->tx[i].dst_pipe;

			memcpy(&rt_rule->rule.attrib, &tx_prop->tx[i].attrib, sizeof(rt_rule->rule.attrib));
			rt_rule->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
			memcpy(rt_rule->rule.attrib.u.v6.dst_addr, info.vlan_client_ipv6_addr,
				sizeof(rt_rule->rule.attrib.u.v6.dst_addr));
			memset(rt_rule->rule.attrib.u.v6.dst_addr_mask, 0xFF, sizeof(rt_rule->rule.attrib.u.v6.dst_addr_mask));
			break;
		}
	}
	if(m_routing.AddRoutingRule(rt_rule_table) == false)
	{
		IPACMERR("Failed to add second pass rt rules.\n");
		free(rt_rule_table);
		return IPACM_FAILURE;
	}
	get_client_memptr(eth_client, index)->dl_second_pass_rt_rule_hdl =
		rt_rule_table->rules[0].rt_rule_hdl;
	free(rt_rule_table);

	strlcpy(new_client_info.client_iface_name, data->iface_name, sizeof(new_client_info.client_iface_name));
	IPACM_Iface::ipacmcfg->l2tp_client.push_back(new_client_info);

	/* post IPA_ADD_L2TP_CLIENT event */
	memset(&evt_data, 0, sizeof(evt_data));
	data_all = (ipacm_event_data_all *)malloc(sizeof(ipacm_event_data_all));
	if(data_all == NULL)
	{
		IPACMERR("Unable to allocate memory for event data.\n");
		return IPACM_FAILURE;
	}
	strlcpy(data_all->iface_name, data->iface_name, sizeof(data_all->iface_name));
	evt_data.event = IPA_ADD_L2TP_CLIENT;
	evt_data.evt_data = data_all;
	IPACM_EvtDispatcher::PostEvt(&evt_data);

	return IPACM_SUCCESS;
}

/* install l2tp ul rules */
int IPACM_Lan::install_l2tp_ul_rules(ipacm_event_data_all *data, int index)
{
	int i, size;
	ipa_ioc_add_rt_rule* rt_rule_table;
	ipa_rt_rule_add *rt_rule;
	ipa_ioc_add_flt_rule_after *pFilteringTable;
	ipa_flt_rule_add *flt_rule_entry;
	l2tp_vlan_mapping_info info;
	l2tp_client_info new_client_info;

	if(tx_prop == NULL || rx_prop == NULL)
	{
		IPACMERR("No tx/rx prop.\n");
		return IPACM_FAILURE;
	}

	if(IPACM_Iface::ipacmcfg->get_vlan_l2tp_mapping(data->iface_name, info) == IPACM_FAILURE)
	{
		IPACMERR("Fail to get vlan-l2tp mapping.\n");
		return IPACM_FAILURE;
	}

	if(l2tp_ul_hdr_proc_ctx_hdl == 0)
	{
		IPACMERR("Ul hdr proc ctx was not installed.\n");
		return IPACM_FAILURE;
	}

	IPACMDBG_H("Installing l2tp ul rt rules.\n");

	/* =========== install ul rt rule ============= */
	size = sizeof(ipa_ioc_add_rt_rule) + sizeof(ipa_rt_rule_add);
	rt_rule_table = (ipa_ioc_add_rt_rule*)malloc(size);
	if (rt_rule_table == NULL)
	{
		IPACMERR("Failed to allocate memory.\n");
		return IPACM_FAILURE;
	}
	memset(rt_rule_table, 0, size);

	rt_rule_table->commit = 1;
	rt_rule_table->ip = IPA_IP_v6;
	rt_rule_table->num_rules = 1;

	strlcpy(rt_rule_table->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_v6.name,
		sizeof(rt_rule_table->rt_tbl_name));
	rt_rule_table->rt_tbl_name[IPA_RESOURCE_NAME_MAX-1] = '\0';

	rt_rule = &rt_rule_table->rules[0];
	for(i = 0; i < iface_query->num_tx_props; i++)
	{
		if(tx_prop->tx[i].ip == IPA_IP_v6)
		{
			rt_rule->at_rear = false;
			rt_rule->status = -1;
			rt_rule->rt_rule_hdl = -1;
			rt_rule->rule.hashable = false;
			rt_rule->rule.hdr_hdl = 0;
			rt_rule->rule.hdr_proc_ctx_hdl =
				l2tp_ul_hdr_proc_ctx_hdl;
			rt_rule->rule.dst = IPA_CLIENT_DUMMY_CONS;

			memcpy(&rt_rule->rule.attrib, &tx_prop->tx[i].attrib, sizeof(rt_rule->rule.attrib));
			rt_rule->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
			memcpy(rt_rule->rule.attrib.u.v6.dst_addr, info.vlan_iface_ipv6_addr,
				sizeof(rt_rule->rule.attrib.u.v6.dst_addr));
			memset(rt_rule->rule.attrib.u.v6.dst_addr_mask, 0xFF,
				sizeof(rt_rule->rule.attrib.u.v6.dst_addr_mask));
			break;
		}
	}
	if(m_routing.AddRoutingRule(rt_rule_table) == false)
	{
		IPACMERR("Failed to add first pass rt rules.\n");
		free(rt_rule_table);
		return IPACM_FAILURE;
	}
	get_client_memptr(eth_client, index)->ul_first_pass_rt_rule_hdl =
		rt_rule_table->rules[0].rt_rule_hdl;
	free(rt_rule_table);

	IPACMDBG_H("Installing l2tp ul flt rules.\n");

	/* =========== install ul flt rule ============= */
	size = sizeof(struct ipa_ioc_add_flt_rule_after) + sizeof(struct ipa_flt_rule_add);
	pFilteringTable = (struct ipa_ioc_add_flt_rule_after*)malloc(size);
	if (!pFilteringTable)
	{
		IPACMERR("Failed to allocate ipa_ioc_add_flt_rule_after memory...\n");
		return IPACM_FAILURE;
	}
	memset(pFilteringTable, 0, size);

	pFilteringTable->commit = 1;
	pFilteringTable->ep = rx_prop->rx[0].src_pipe;
	pFilteringTable->ip = IPA_IP_v6;
	pFilteringTable->num_rules = 1;
	pFilteringTable->add_after_hdl = ipv6_icmp_flt_rule_hdl[0];

	if(false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_v6))
	{
		IPACMERR("m_routing.GetRoutingTable Failed.\n");
		free(pFilteringTable);
		return IPACM_FAILURE;
	}

	flt_rule_entry = &pFilteringTable->rules[0];
	flt_rule_entry->at_rear = 1;

	flt_rule_entry->rule.retain_hdr = 0;
	flt_rule_entry->rule.to_uc = 0;
	flt_rule_entry->rule.action = IPA_PASS_TO_ROUTING;
	flt_rule_entry->rule.eq_attrib_type = 0;
	flt_rule_entry->rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_v6.hdl;
	flt_rule_entry->rule.hashable = false;

	memcpy(&flt_rule_entry->rule.attrib, &rx_prop->rx[0].attrib, sizeof(flt_rule_entry->rule.attrib));
	flt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
	memcpy(flt_rule_entry->rule.attrib.u.v6.dst_addr, info.vlan_iface_ipv6_addr,
		sizeof(flt_rule_entry->rule.attrib.u.v6.dst_addr));
	memset(flt_rule_entry->rule.attrib.u.v6.dst_addr_mask, 0xFF,
		sizeof(flt_rule_entry->rule.attrib.u.v6.dst_addr_mask));
	flt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_NEXT_HDR;
	flt_rule_entry->rule.attrib.u.v6.next_hdr = 0x73;
	flt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_L2TP_INNER_IP_TYPE;
	flt_rule_entry->rule.attrib.type = 0x40;

	if(m_filtering.AddFilteringRuleAfter(pFilteringTable) == false)
	{
		IPACMERR("Failed to add l2tp ul flt rule.\n");
		free(pFilteringTable);
		return IPACM_FAILURE;
	}

	IPACM_Iface::ipacmcfg->increaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, 1);
	get_client_memptr(eth_client, index)->ul_first_pass_flt_rule_hdl =
		pFilteringTable->rules[0].flt_rule_hdl;
	free(pFilteringTable);
	return IPACM_SUCCESS;
}

/* uninstall l2tp rules */
int IPACM_Lan::uninstall_l2tp_rules(ipacm_event_data_all *data)
{
	int index;
	list<l2tp_client_info>::iterator it;
	ipacm_cmd_q_data evt_data;
	ipacm_event_data_all *data_all;

	index = get_eth_client_index(data->mac_addr);
	if(index == IPACM_INVALID_INDEX)
	{
		IPACMDBG_H("Eth client not attached\n");
		return IPACM_SUCCESS;
	}

	if(is_l2tp_iface == false)
	{
		IPACMDBG_H("This is not L2TP client.\n");
		return IPACM_SUCCESS;
	}

	HandleNeighIpAddrDelEvt(
		get_client_memptr(eth_client, index)->ipv4_set,
		get_client_memptr(eth_client, index)->v4_addr,
		get_client_memptr(eth_client, index)->ipv6_set,
		get_client_memptr(eth_client, index)->v6_addr);

	/* delete dl rules */
	if(m_routing.DeleteRoutingHdl(get_client_memptr(eth_client, index)->dl_first_pass_rt_rule_hdl, IPA_IP_v4) == false)
	{
		IPACMERR("Failed to delete first pass rt rule.\n");
		return IPACM_FAILURE;
	}

	if(m_routing.DeleteRoutingHdl(get_client_memptr(eth_client, index)->dl_second_pass_rt_rule_hdl, IPA_IP_v6) == false)
	{
		IPACMERR("Failed to delete second pass rt rule.\n");
		return IPACM_FAILURE;
	}

	if(m_header.DeleteHeaderProcCtx(get_client_memptr(eth_client, index)->dl_first_pass_hdr_proc_ctx_hdl) == false)
	{
		IPACMERR("Failed to delete first pass hdr proc ctx.\n");
		return IPACM_FAILURE;
	}

	if(m_header.DeleteHeaderHdl(get_client_memptr(eth_client, index)->dl_first_pass_hdr_hdl) == false)
	{
		IPACMERR("Failed to delete first pass hdr.\n");
		return IPACM_FAILURE;
	}

	if(m_header.DeleteHeaderHdl(get_client_memptr(eth_client, index)->dl_second_pass_hdr_hdl) == false)
	{
		IPACMERR("Failed to delete second pass hdr.\n");
		return IPACM_FAILURE;
	}

	/* delete ul rules */
	if(m_filtering.DeleteFilteringHdls(&get_client_memptr(eth_client, index)->ul_first_pass_flt_rule_hdl, IPA_IP_v6, 1) == false)
	{
		IPACMERR("Failed to delete ul flt rule.\n");
		return IPACM_FAILURE;
	}
	IPACM_Iface::ipacmcfg->decreaseFltRuleCount(rx_prop->rx[0].src_pipe, IPA_IP_v6, 1);

	if(m_routing.DeleteRoutingHdl(get_client_memptr(eth_client, index)->ul_first_pass_rt_rule_hdl, IPA_IP_v6) == false)
	{
		IPACMERR("Failed to delete ul rt rule.\n");
		return IPACM_FAILURE;
	}

	for(; index < num_eth_client-1; index++)
	{
		*get_client_memptr(eth_client, index) = *get_client_memptr(eth_client, index+1);
	}
	num_eth_client--;

	for(it = IPACM_Iface::ipacmcfg->l2tp_client.begin(); it != IPACM_Iface::ipacmcfg->l2tp_client.end(); it++)
	{
		if(strncmp(it->client_iface_name, data->iface_name, sizeof(it->client_iface_name)) == 0)
		{
			IPACM_Iface::ipacmcfg->l2tp_client.erase(it);
			break;
		}
	}

	/* post IPA_DEL_L2TP_CLIENT event */
	memset(&evt_data, 0, sizeof(evt_data));
	data_all = (ipacm_event_data_all *)malloc(sizeof(ipacm_event_data_all));
	if(data_all == NULL)
	{
		IPACMERR("Unable to allocate memory for event data.\n");
		return IPACM_FAILURE;
	}
	strlcpy(data_all->iface_name, data->iface_name, sizeof(data_all->iface_name));
	evt_data.event = IPA_DEL_L2TP_CLIENT;
	evt_data.evt_data = data_all;
	IPACM_EvtDispatcher::PostEvt(&evt_data);

	/* Del RM dependency */
	if(num_eth_client == 0)
	{
		IPACMDBG_H("Netdev %s delete dependency\n", dev_name);
		if(tx_prop != NULL)
		{
			IPACMDBG_H("Dependency pipe: %d, rm index: %d\n", tx_prop->tx[0].dst_pipe, IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
			IPACM_Iface::ipacmcfg->DelRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
		}
	}

	return IPACM_SUCCESS;
}

int IPACM_Lan::install_l2tp_ul_hdr_proc_ctx()
{
	int size;
	ipa_ioc_add_hdr *hdr_table;
	ipa_hdr_add *hdr;
	ipa_ioc_add_hdr_proc_ctx *hdr_proc_ctx_table;
	ipa_hdr_proc_ctx_add *hdr_proc_ctx;

	/* =========== install l2tp ul dummy header ============= */
	size = sizeof(ipa_ioc_add_hdr) + sizeof(ipa_hdr_add);
	hdr_table = (ipa_ioc_add_hdr*)malloc(size);
	if(hdr_table == NULL)
	{
		IPACMERR("Failed to allocate memory.\n");
		return IPACM_FAILURE;
	}
	memset(hdr_table, 0, size);

	hdr_table->commit = 1;
	hdr_table->num_hdrs = 1;
	hdr = &hdr_table->hdr[0];
	snprintf(hdr->name, sizeof(hdr->name), "l2tp_ul");

	hdr->hdr_len = 4;
	hdr->type = IPA_HDR_L2_ETHERNET_II;
	hdr->is_partial = 0;

	if(m_header.AddHeader(hdr_table) == false)
	{
		IPACMERR("Failed to add hdr with status: %d\n", hdr_table->hdr[0].status);
		free(hdr_table);
		return IPACM_FAILURE;
	}
	l2tp_ul_dummy_hdr_hdl = hdr_table->hdr[0].hdr_hdl;
	IPACMDBG_H("Installed l2tp ul hdr: hdl %d\n", l2tp_ul_dummy_hdr_hdl);
	free(hdr_table);

	/* =========== install l2tp ul hdr proc ctx ============= */
	size = sizeof(ipa_ioc_add_hdr_proc_ctx) + sizeof(ipa_hdr_proc_ctx_add);
	hdr_proc_ctx_table = (ipa_ioc_add_hdr_proc_ctx*)malloc(size);
	if(hdr_proc_ctx_table == NULL)
	{
		IPACMERR("Failed to allocate memory.\n");
		return IPACM_FAILURE;
	}
	memset(hdr_proc_ctx_table, 0, size);

	hdr_proc_ctx_table->commit = 1;
	hdr_proc_ctx_table->num_proc_ctxs = 1;
	hdr_proc_ctx = &hdr_proc_ctx_table->proc_ctx[0];

	hdr_proc_ctx->type = IPA_HDR_PROC_L2TP_HEADER_REMOVE;
	hdr_proc_ctx->hdr_hdl = l2tp_ul_dummy_hdr_hdl;
	hdr_proc_ctx->l2tp_params.hdr_remove_param.hdr_len_remove = 62;
	hdr_proc_ctx->l2tp_params.hdr_remove_param.eth_hdr_retained = 0;
	if(m_header.AddHeaderProcCtx(hdr_proc_ctx_table) == false)
	{
		IPACMERR("Failed to add hdr proc ctx with status: %d\n", hdr_proc_ctx_table->proc_ctx[0].status);
		free(hdr_proc_ctx_table);
		return IPACM_FAILURE;
	}
	l2tp_ul_hdr_proc_ctx_hdl = hdr_proc_ctx_table->proc_ctx[0].proc_ctx_hdl;
	IPACMDBG_H("Installed l2tp ul hdr proc ctx: hdl %d\n", l2tp_ul_hdr_proc_ctx_hdl);
	free(hdr_proc_ctx_table);

	return IPACM_SUCCESS;
}
#endif

int IPACM_Lan::post_lan_up_event(const ipacm_event_data_addr* data) const
{
	ipacm_cmd_q_data evt_data;
	ipacm_event_iface_up* info;

	evt_data.evt_data = malloc(sizeof(ipacm_event_iface_up));
	if (evt_data.evt_data == NULL)
	{
		IPACMERR("Unable to allocate memory\n");
		return -ENOMEM;
	}

	info = static_cast<ipacm_event_iface_up*>(evt_data.evt_data);
	memcpy(info->ifname, dev_name, IF_NAME_LEN);

	switch (data->iptype)
	{
	case IPA_IP_v4:
		info->ipv4_addr = data->ipv4_addr;
		info->addr_mask = IPACM_Iface::ipacmcfg->private_subnet_table[0].subnet_mask;
		evt_data.event = IPA_HANDLE_LAN_WLAN_UP;

		IPACMDBG_H("posting client interface up for IPv4 with below information\n");
		IPACMDBG_H("IPv4 address:0x%x, IPv4 address mask:0x%x\n", info->ipv4_addr, info->addr_mask);
		break;
	case IPA_IP_v6:
		memcpy(info->ipv6_addr, data->ipv6_addr, sizeof(info->ipv6_addr));
		evt_data.event = IPA_HANDLE_LAN_WLAN_UP_V6;

		IPACMDBG_H("posting client interface up for IPv6 with below information\n");
		IPACMDBG_H("IPv6 address:0x%x%x%x%x\n",
			info->ipv6_addr[0], info->ipv6_addr[1], info->ipv6_addr[2], info->ipv6_addr[3]);
		break;
	default:
		IPACMERR("Unsupported IP type %d\n", data->iptype);
		return -EINVAL;
	}

	IPACM_EvtDispatcher::PostEvt(&evt_data);
	return 0;
}

void IPACM_Lan::HandleNeighIpAddrAddEvt(ipacm_event_data_all *data)
{
	switch (data->iptype)
	{
	case IPA_IP_v4:
		CtList->HandleNeighIpAddrAddEvt(data);
		break;
	case IPA_IP_v6:
	{
		CtList->HandleNeighIpAddrAddEvt_v6(Ipv6IpAddress(data->ipv6_addr, false), data->if_index);
		break;
	}
	default:
		IPACMERR("Not supported IP type %d", data->iptype);
	}
}

void IPACM_Lan::HandleNeighIpAddrDelEvt(bool ipv4_set, uint32_t ipv4_addr,
	int ipv6_set, const uint32_t ipv6_addr[IPV6_NUM_ADDR][IPA_IPV6_ADDR_SIZE_IN_WORDS])
{
	if (ipv4_set)
	{
		CtList->HandleNeighIpAddrDelEvt(ipv4_addr);
	}

	for (int i = 0; i < ipv6_set; ++i)
	{
		CtList->HandleNeighIpAddrDelEvt_v6(Ipv6IpAddress(ipv6_addr[i], false));
	}
}

