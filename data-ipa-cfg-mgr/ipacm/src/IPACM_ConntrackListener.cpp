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
#include <sys/ioctl.h>
#include <net/if.h>

#include "IPACM_ConntrackListener.h"
#include "IPACM_ConntrackClient.h"
#include "IPACM_EvtDispatcher.h"
#include "IPACM_Iface.h"
#include "IPACM_Wan.h"

IPACM_ConntrackListener::IPACM_ConntrackListener() :
	WanUp_v6(false),
	ipv6ct_inst(Ipv6ct::GetInstance()),
	StaClntCnt_v6(0),
	nat_iface_ipv6_addr(*(new Ipv6IpAddressesCollection(MAX_IFACE_ADDRESS))),
	nonnat_iface_ipv6_addr(*(new Ipv6IpAddressesCollection(MAX_IFACE_ADDRESS))),
	sta_clnt_ipv6_addr(*(new Ipv6IpAddressesCollection(MAX_STA_CLNT_IFACES))),
	wan_ipaddr_v6(*(new Ipv6IpAddress))
{
	 IPACMDBG("\n");

	 isNatThreadStart = false;
	 isCTReg = false;
	 WanUp = false;
	 nat_inst = NatApp::GetInstance();

	 NatIfaceCnt = 0;
	 StaClntCnt = 0;
	 pNatIfaces = NULL;
	 pConfig = IPACM_Config::GetInstance();;
	 memset(nat_clients, 0, sizeof(nat_clients));
#ifdef FEATURE_VLAN_MPDN
	 memset(vlan_pdns, 0, sizeof(vlan_pdns));
	 num_vlan_pdns = 0;
#endif
	 memset(nonnat_iface_ipv4_addr, 0, sizeof(nonnat_iface_ipv4_addr));
	 memset(sta_clnt_ipv4_addr, 0, sizeof(sta_clnt_ipv4_addr));

	 IPACM_EvtDispatcher::registr(IPA_HANDLE_WAN_UP, this);
	 IPACM_EvtDispatcher::registr(IPA_HANDLE_WAN_DOWN, this);
	if (IsIpv6CTEnabled())
	{
		IPACM_EvtDispatcher::registr(IPA_HANDLE_WAN_UP_V6, this);
		IPACM_EvtDispatcher::registr(IPA_HANDLE_WAN_DOWN_V6, this);
	}
#ifdef FEATURE_VLAN_MPDN
	 IPACM_EvtDispatcher::registr(IPA_HANDLE_WAN_VLAN_PDN_UP, this);
	 IPACM_EvtDispatcher::registr(IPA_HANDLE_WAN_VLAN_PDN_DOWN, this);
#endif
	 IPACM_EvtDispatcher::registr(IPA_PROCESS_CT_MESSAGE, this);
	 IPACM_EvtDispatcher::registr(IPA_PROCESS_CT_MESSAGE_V6, this);
	 IPACM_EvtDispatcher::registr(IPA_HANDLE_LAN_WLAN_UP, this);
	 IPACM_EvtDispatcher::registr(IPA_HANDLE_LAN_WLAN_UP_V6, this);
	 IPACM_EvtDispatcher::registr(IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT, this);
	 IPACM_EvtDispatcher::registr(IPA_NEIGH_CLIENT_IP_ADDR_DEL_EVENT, this);

#ifdef CT_OPT
	 p_lan2lan = IPACM_LanToLan::getLan2LanInstance();
#endif
}

IPACM_ConntrackListener::~IPACM_ConntrackListener()
{
	delete &nat_iface_ipv6_addr;
	delete &nonnat_iface_ipv6_addr;
	delete &sta_clnt_ipv6_addr;
	delete &wan_ipaddr_v6;
}

void IPACM_ConntrackListener::event_callback(ipa_cm_event_id evt,
						void *data)
{
	const ipacm_event_iface_up *wan_data = NULL;

	 if(data == NULL)
	 {
		 IPACMERR("Invalid Data\n");
		 return;
	 }

	 switch(evt)
	 {
	 case IPA_PROCESS_CT_MESSAGE:
			IPACMDBG("Received IPA_PROCESS_CT_MESSAGE event\n");
			ProcessCTMessage(data);
			break;

	case IPA_PROCESS_CT_MESSAGE_V6:
	{
		if (IsIpv6CTEnabled())
		{
			IPACMDBG_H("Received IPA_PROCESS_CT_MESSAGE_V6 event\n");
			const ipacm_ct_evt_data* evt_data = static_cast<const ipacm_ct_evt_data*>(data);
			Ipv6ctEntry entry;
			CreateIpv6ctEntryFromCtEventData(evt_data, entry);
			ProcessCTMessage_v6(evt_data, entry);
		}
#ifdef CT_OPT
			ProcessCTV6Message(data);
#endif
		break;
	}
	 case IPA_HANDLE_WAN_UP:
			IPACMDBG_H("Received IPA_HANDLE_WAN_UP event\n");
			if (!WanUp)
			{
				TriggerWANUp(data);
			}
			break;
#ifdef FEATURE_VLAN_MPDN
	 case IPA_HANDLE_WAN_VLAN_PDN_UP:
			IPACMDBG_H("Received IPA_HANDLE_WAN_VLAN_PDN_UP event\n");
			HandleVlanUp(data);
			break;

	 case IPA_HANDLE_WAN_VLAN_PDN_DOWN:
			IPACMDBG_H("Received IPA_HANDLE_WAN_VLAN_PDN_DOWN event\n");
			HandleVlanDown(data);
			break;
#endif

	 case IPA_HANDLE_WAN_DOWN:
			IPACMDBG_H("Received IPA_HANDLE_WAN_DOWN event\n");
			wan_data = (ipacm_event_iface_up *)data;
			if (WanUp)
			{
				TriggerWANDown(wan_data->ipv4_addr);
			}
			break;

	case IPA_HANDLE_WAN_UP_V6:
		IPACMDBG_H("Received IPA_HANDLE_WAN_UP_V6 event\n");
		if (!WanUp_v6)
		{
			wan_data = static_cast<const ipacm_event_iface_up*>(data);
			static_cast<Ipv6IpAddress&>(wan_ipaddr_v6).CreateFromArray(wan_data->ipv6_addr, false);
			TriggerWANUp_v6(wan_data);
		}
		break;

	case IPA_HANDLE_WAN_DOWN_V6:
		IPACMDBG_H("Received IPA_HANDLE_WAN_DOWN_V6 event\n");
		if (WanUp_v6)
		{
			wan_data = static_cast<const ipacm_event_iface_up*>(data);
			Ipv6IpAddress wan_addr;
			wan_addr.CreateFromArray(wan_data->ipv6_addr, false);
			TriggerWANDown_v6(wan_addr);
		}
		break;

	/* modify TCP/UDP filters to ignore local WLAN or LAN IPv4 connections */
	case IPA_HANDLE_LAN_WLAN_UP:
			IPACMDBG_H("Received event: %d with ifname: %s and address: 0x%x\n",
							 evt, ((ipacm_event_iface_up *)data)->ifname,
							 ((ipacm_event_iface_up *)data)->ipv4_addr);
			IPACM_ConntrackClient::UpdateUDPFilters(data, false);
			IPACM_ConntrackClient::UpdateTCPFilters(data, false);
			break;

	/* modify TCP/UDP filters to ignore local WLAN or LAN IPv6 connections */
	case IPA_HANDLE_LAN_WLAN_UP_V6:
		IPACM_ConntrackClient::UpdateFilters_v6(static_cast<ipacm_event_iface_up*>(data));
		break;

	 case IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT:
		 IPACMDBG("Received IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT event\n");
		 HandleNonNatIPAddr(data, true);
		 break;

	 case IPA_NEIGH_CLIENT_IP_ADDR_DEL_EVENT:
		 IPACMDBG("Received IPA_NEIGH_CLIENT_IP_ADDR_DEL_EVENT event\n");
		 HandleNonNatIPAddr(data, false);
		 break;

	 default:
			IPACMDBG("Ignore cmd %d\n", evt);
			break;
	 }
}

uint32_t IPACM_ConntrackListener::GetPacketThreshhold(void)
{
	uint32_t pkt_thrshld = 0;
	FILE *cmd = NULL;
	int acct = 0;
	const char acct_proc[] = "cat /proc/sys/net/netfilter/nf_conntrack_acct";
	const char pkt_thresh_proc[] = "cat /proc/sys/net/netfilter/nf_conntrack_pkt_threshold";
	char input_value[MAX_CMD_SIZE] = {0};

	cmd = popen(acct_proc, "r");
	if(cmd)
	{
		fgets(input_value, MAX_CMD_SIZE, cmd);
		acct = atoi(input_value);
		pclose(cmd);
		if ( acct == 1 )
		{
			IPACMDBG_H("Accounting is enabled.\n");
			cmd = popen(pkt_thresh_proc, "r");
			if(cmd)
			{
				memset(input_value, 0, MAX_CMD_SIZE);
				fgets(input_value, MAX_CMD_SIZE, cmd);
				pkt_thrshld = strtoul(input_value, NULL, 0);
				IPACMDBG_H("Configured packet threshold: %d\n", pkt_thrshld);
				pclose(cmd);
			}
			else
			{
				IPACMDBG_H("Packet threshold is not enabled.\n");
			}
		}
		else
		{
			IPACMDBG_H("Accounting is not enabled.\n");
		}
	}
	return pkt_thrshld;
}

int IPACM_ConntrackListener::CheckNatIface(int if_index, bool *NatIface)
{
	int fd = 0, len = 0, cnt, i;
	struct ifreq ifr;
	*NatIface = false;

	IPACMDBG("Received interface index %d", if_index);

	if (pConfig == NULL)
	{
		pConfig = IPACM_Config::GetInstance();
		if (pConfig == NULL)
		{
			IPACMERR("Unable to get Config instance\n");
			return IPACM_FAILURE;
		}
	}

	cnt = pConfig->GetNatIfacesCnt();
	NatIfaceCnt = cnt;
	IPACMDBG("Total Nat ifaces: %d\n", NatIfaceCnt);
	if (pNatIfaces != NULL)
	{
		free(pNatIfaces);
		pNatIfaces = NULL;
	}

	len = (sizeof(NatIfaces) * NatIfaceCnt);
	pNatIfaces = (NatIfaces *)malloc(len);
	if (pNatIfaces == NULL)
	{
		IPACMERR("Unable to allocate memory for non nat ifaces\n");
		return IPACM_FAILURE;
	}

	memset(pNatIfaces, 0, len);
	if (pConfig->GetNatIfaces(NatIfaceCnt, pNatIfaces) != 0)
	{
		IPACMERR("Unable to retrieve non nat ifaces\n");
		return IPACM_FAILURE;
	}

	/* Search/Configure linux interface-index and map it to IPA interface-index */
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		PERROR("get interface name socket create failed");
		return IPACM_FAILURE;
	}

	memset(&ifr, 0, sizeof(struct ifreq));
	ifr.ifr_ifindex = if_index;
	if (ioctl(fd, SIOCGIFNAME, &ifr) < 0)
	{
		PERROR("call_ioctl_on_dev: ioctl failed:");
		close(fd);
		return IPACM_FAILURE;
	}
	close(fd);

	for (i = 0; i < NatIfaceCnt; i++)
	{
		if (strncmp(ifr.ifr_name,
					pNatIfaces[i].iface_name,
					sizeof(pNatIfaces[i].iface_name)) == 0)
		{
			IPACMDBG_H("Nat iface (%s), entry (%d), dont cache",
						pNatIfaces[i].iface_name, i);
			*NatIface = true;
			return IPACM_SUCCESS;
		}
	}

	return IPACM_SUCCESS;
}

void IPACM_ConntrackListener::HandleNonNatIPAddr(void* inParam, bool AddOp)
{
	const ipacm_event_data_all *data = (ipacm_event_data_all *)inParam;

	switch (data->iptype)
	{
	case IPA_IP_v4:
		/* For IPv4 legacy HandleNonNatIPAddr renamed to HandleNonNatIPAddr_v4 */
		HandleNonNatIPAddr_v4(inParam, AddOp);
		break;
	case IPA_IP_v6:
	{
		if (IsIpv6CTEnabled())
		{
			Ipv6IpAddress ipv6Addr(data->ipv6_addr, false);
			HandleNonNatIPAddr_v6(ipv6Addr, data->if_index, AddOp);
		}
		break;
	}
	default:
		IPACMERR("Not supported IP type %d", data->iptype);
	}
}

void IPACM_ConntrackListener::HandleNonNatIPAddr_v4(
   void *inParam, bool AddOp)
{
	ipacm_event_data_all *data = (ipacm_event_data_all *)inParam;
	bool NatIface = false;
	int cnt, ret;

	if (isStaMode)
	{
		IPACMDBG("In STA mode, don't add dummy rules for non nat ifaces\n");
		return;
	}

	if (data->ipv4_addr == 0)
	{
		IPACMDBG("Ignoring\n");
		return;
	}

	/* Handle only non nat ifaces, NAT iface should be handle
	   separately to avoid race conditions between route/nat
	   rules add/delete operations */
	if (AddOp)
	{
		ret = CheckNatIface(data->if_index, &NatIface);
		if (!NatIface && ret == IPACM_SUCCESS)
		{
			/* Cache the non nat iface ip address */
			for (cnt = 0; cnt < MAX_IFACE_ADDRESS; cnt++)
			{
				if (nonnat_iface_ipv4_addr[cnt] == 0)
				{
					nonnat_iface_ipv4_addr[cnt] = data->ipv4_addr;
					IPACMDBG("Add ip addr to non nat list (%d) ", cnt);
					iptodot("with ipv4 address", nonnat_iface_ipv4_addr[cnt]);

					/* Add dummy nat rule for non nat ifaces */
					nat_inst->FlushTempEntries(data->ipv4_addr, true, true);
					return;
				}
			}
		}
	}
	else
	{
		/* for delete operation */
		for (cnt = 0; cnt < MAX_IFACE_ADDRESS; cnt++)
		{
			if (nonnat_iface_ipv4_addr[cnt] == data->ipv4_addr)
			{
				IPACMDBG("Reseting ct filters, entry (%d) ", cnt);
				iptodot("with ipv4 address", nonnat_iface_ipv4_addr[cnt]);
				nonnat_iface_ipv4_addr[cnt] = 0;
				nat_inst->FlushTempEntries(data->ipv4_addr, false);
				nat_inst->DelEntriesOnClntDiscon(data->ipv4_addr);
				return;
			}
		}

	}

	return;
}

void IPACM_ConntrackListener::HandleNonNatIPAddr_v6(const IpAddress& ip, int if_index, bool AddOp)
{
	IPACMDBG_H("\n");
	if (isStaMode)
	{
		IPACMDBG("In STA mode, don't add dummy rules for non nat ifaces\n");
		return;
	}

	ip.DebugDump("Handle nonnat interface with following address\n");

	/*
	 * Handle only non NAT intefraces, NAT interfaces should be handle separately to avoid race conditions between
	 * route/NAT rules add/delete operations
	 */
	if (AddOp)
	{
		bool NatIface = false;
		int ret = CheckNatIface(if_index, &NatIface);
		if (NatIface || ret != IPACM_SUCCESS)
		{
			return;
		}

		if (nonnat_iface_ipv6_addr.Find(ip) != NULL)
		{
			IPACMDBG_H("IP duplication. Ignore\n");
			return;
		}

		IpAddress* entry = nonnat_iface_ipv6_addr.GetFirstEmpty();
		if (entry == NULL)
		{
			IPACMERR("Unable to add, reached maximum nonnat_interfaces\n");
			return;
		}

		*entry = ip;

		/* Add dummy NAT rule for non NAT interfaces */
		ipv6ct_inst->FlushTempEntries(ip, true, true, false);

		IPACMDBG_H("Successfully added nonnat interface\n");
	}
	else
	{
		/* for delete operation */
		IpAddress* entry = nonnat_iface_ipv6_addr.Find(ip);
		if (entry == NULL)
		{
			IPACMDBG_H("The interface is not in nonnat interfaces\n");
			return;
		}

		entry->Clear();
		ipv6ct_inst->FlushTempEntries(ip, false, true, false);
		ipv6ct_inst->DelEntriesOnClntDiscon(ip);
		IPACMDBG("Successfully deleted nonnat interface\n");
	}
}

void IPACM_ConntrackListener::HandleNeighIpAddrAddEvt(
   ipacm_event_data_all *data)
{
	bool NatIface = false;
	int j, ret;

	ret = CheckNatIface(data->if_index, &NatIface);
	if (NatIface && ret == IPACM_SUCCESS)
	{
		for (j = 0; j < MAX_IFACE_ADDRESS; j++)
		{
			/* check if duplicate NAT ip */
			if (nat_clients[j].nat_iface_ipv4_addr == data->ipv4_addr)
				break;

			/* Cache the new nat iface address */
			if (nat_clients[j].nat_iface_ipv4_addr == 0)
			{
				nat_clients[j].nat_iface_ipv4_addr = data->ipv4_addr;
#ifdef FEATURE_VLAN_MPDN
				if (pConfig == NULL)
				{
					pConfig = IPACM_Config::GetInstance();
					if (pConfig == NULL)
					{
						IPACMERR("Unable to get Config instance\n");
						return;
					}
				}

				if(pConfig->get_vlan_id(data->iface_name, &nat_clients[j].vlan_id) == IPACM_SUCCESS)
				{
					nat_clients[j].is_vlan_client = true;
					IPACMDBG_H("client %d: vlan iface %s has vlan id %d ", j, data->iface_name, nat_clients[j].vlan_id);
					iptodot("and ip data->ipv4_addr", data->ipv4_addr);
				}
				else
				{
					nat_clients[j].is_vlan_client = false;
					nat_clients[j].vlan_id = 0;
					IPACMDBG_H("client %d: iface %s is not a vlan iface\n", j, data->iface_name);
				}
#endif
				IPACMDBG_H("for iface %s: ", data->iface_name);
				iptodot("Nating connections of iface addr: ", nat_clients[j].nat_iface_ipv4_addr);
				break;
			}
		}

		/* Add the cached temp entries to NAT table */
		if(j != MAX_IFACE_ADDRESS)
		{
			nat_inst->ResetPwrSaveIf(data->ipv4_addr);
#ifdef FEATURE_VLAN_MPDN
			uint32_t public_ip;
			bool entry_exists;
			int i;

			IPACMDBG_H("client %d is_vlan_client %d\n", j, nat_clients[j].is_vlan_client);
			if (nat_clients[j].is_vlan_client)
			{
				IPACMDBG("handling VLAN clients temp entries\n");
				iptodot("vlan client ip", data->ipv4_addr);

				nat_inst->FlushAndCacheVlanTempEntries(data->ipv4_addr, &entry_exists, &public_ip);
				if(entry_exists)
				{
					for(i = 0; i < IPA_MAX_NUM_HW_PDNS; i++)
					{
						/* check if we already got vlan_pdn_up event for this ip */
						if(vlan_pdns[i].public_ip == public_ip)
						{
							IPACMDBG_H("vlan pdn already up for ");
							iptodot("ip", public_ip);
							return;
						}
					}

					if((i >= IPA_MAX_NUM_HW_PDNS) && (num_vlan_pdns >= IPA_MAX_NUM_HW_PDNS))
					{
						iptodot("vlan client ip", data->ipv4_addr);
						iptodot("pdn ip", public_ip);
						IPACMERR("can't add more PDN, already got max \n");
						return;
					}

					ipacm_cmd_q_data evt_data;
					ipacm_event_route_vlan *vlan_data;

					evt_data.event = IPA_ROUTE_ADD_VLAN_PDN_EVENT;
					vlan_data = (ipacm_event_route_vlan *)malloc(sizeof(ipacm_event_route_vlan));
					if(!vlan_data)
					{
						IPACMERR("couldn't allocate memory for new vlan pdn event\n");
						return;
					}
					vlan_data->iptype = IPA_IP_v4;
					vlan_data->VlanID = nat_clients[j].vlan_id;
					vlan_data->wan_ipv4_addr = public_ip;
					evt_data.evt_data = vlan_data;
					IPACMDBG("sending IPA_ROUTE_ADD_VLAN_PDN_EVENT vlan id %d, iptype %d,\n",
						vlan_data->VlanID,
						vlan_data->iptype);
					iptodot("pdn ip", public_ip);

					IPACM_EvtDispatcher::PostEvt(&evt_data);
				}
			}
			else
#endif
			{
				IPACMDBG("Flushing temp entries client %d\n", j);
				iptodot("client ip", data->ipv4_addr);
				nat_inst->FlushTempEntries(data->ipv4_addr, true);
			}
		}
	}
	return;
}

void IPACM_ConntrackListener::HandleNeighIpAddrAddEvt_v6(const IpAddress& ip, int if_index)
{
	IPACMDBG_H("\n");

	if (!IsIpv6CTEnabled() || !ip.Valid())
	{
		IPACMDBG("Ignoring\n");
		return;
	}

	ip.DebugDump("Add NAT interface with following address\n");

	bool NatIface = false;
	int ret = CheckNatIface(if_index, &NatIface);
	if (!NatIface || ret != IPACM_SUCCESS)
	{
		return;
	}

	if (nat_iface_ipv6_addr.Find(ip) == NULL)
	{
		IpAddress* entry = nat_iface_ipv6_addr.GetFirstEmpty();
		if (entry == NULL)
		{
			IPACMERR("Unable to add, reached maximum nat_interfaces\n");
			return;
		}

		*entry = ip;
	}

	ipv6ct_inst->ResetPwrSaveIf(ip);
	ipv6ct_inst->FlushTempEntries(ip, true, false, false);

	IPACMDBG_H("Successfully added NAT interface\n");
}

#ifdef FEATURE_VLAN_MPDN
bool IPACM_ConntrackListener::IsVlanIPv4(uint32_t ipv4_address, uint8_t *VlanId)
{
	iptodot("checking ipv4_address", ipv4_address);

	for(int i = 0; i < MAX_IFACE_ADDRESS; i++)
	{
		if(nat_clients[i].nat_iface_ipv4_addr == ipv4_address)
		{
			if(nat_clients[i].is_vlan_client)
			{
				IPACMDBG_H("ipv4 address belong to vlan iface with id %d\n", nat_clients[i].vlan_id)
				*VlanId = nat_clients[i].vlan_id;
				return true;
			}
			else
			{
				IPACMDBG_H("not vlan v4 address\n");
				return false;
			}
			return false;
		}
	}
	IPACMDBG("couldn't match IP\n");
	return false;
}
#endif

void IPACM_ConntrackListener::HandleNeighIpAddrDelEvt(
   uint32_t ipv4_addr)
{
	int cnt;

	if(ipv4_addr == 0)
	{
		IPACMDBG("Ignoring\n");
		return;
	}

	iptodot("HandleNeighIpAddrDelEvt(): Received ip addr", ipv4_addr);
	for(cnt = 0; cnt<MAX_IFACE_ADDRESS; cnt++)
	{
		if (nat_clients[cnt].nat_iface_ipv4_addr == ipv4_addr)
		{
			IPACMDBG("Reseting ct nat iface, entry (%d) ", cnt);
			iptodot("with ipv4 address", nat_clients[cnt].nat_iface_ipv4_addr);
			nat_clients[cnt].nat_iface_ipv4_addr = 0;
#ifdef FEATURE_VLAN_MPDN
			nat_clients[cnt].is_vlan_client = false;
			nat_clients[cnt].vlan_id = 0;
#endif
			nat_inst->FlushTempEntries(ipv4_addr, false);
			nat_inst->DelEntriesOnClntDiscon(ipv4_addr);
		}
	}

	return;
}

void IPACM_ConntrackListener::HandleNeighIpAddrDelEvt_v6(const IpAddress& ip)
{
	IPACMDBG_H("\n");

	if (!IsIpv6CTEnabled() || !ip.Valid())
	{
		IPACMDBG("Ignoring\n");
		return;
	}

	ip.DebugDump("Delete NAT interface with following address\n");

	IpAddress* entry = nat_iface_ipv6_addr.Find(ip);
	if (entry == NULL)
	{
		IPACMDBG_H("The interface is not NAT interface\n");
		return;
	}

	entry->Clear();
	ipv6ct_inst->FlushTempEntries(ip, false, false, false);
	ipv6ct_inst->DelEntriesOnClntDiscon(ip);

	IPACMDBG_H("Successfully deleted NAT interface\n");
}

#ifdef FEATURE_VLAN_MPDN
void IPACM_ConntrackListener::HandleVlanUp(void *in_param)
{
	ipacm_event_vlan_pdn *vlanup_data = (ipacm_event_vlan_pdn *)in_param;
	IPACMDBG_H("Recevied below information during VLAN PDN up,\n");
	IPACMDBG_H("IPType: %d, vlan_id:%d, mux id %d\n",
		vlanup_data->iptype,
		vlanup_data->VlanID,
		vlanup_data->mux_id);
	if(nat_inst == NULL)
	{
		IPACMERR(" no nat_inst\n");
		return;
	}

	if(vlanup_data->iptype == IPA_IP_v4)
	{
		/* we exceeded max num pdns */
		if(num_vlan_pdns >= IPA_MAX_NUM_HW_PDNS)
			return;

		IPACMDBG_H("ipv4 address for new PDN 0x%X\n", vlanup_data->ipv4_addr);
		if(nat_inst->AddPdn(vlanup_data->ipv4_addr, vlanup_data->mux_id, false))
		{
			IPACMERR("failed adding pdn\n");
		}
		else
		{
			for(int i = 0; i < IPA_MAX_NUM_HW_PDNS; i++)
			{
				if(vlan_pdns[i].public_ip == 0)
				{
					IPACMDBG_H("found empty PDN entry in %d\n", i);
					vlan_pdns[i].public_ip = vlanup_data->ipv4_addr;
					vlan_pdns[i].vlan_id = vlanup_data->VlanID;
					num_vlan_pdns++;
					break;
				}
			}
			if(!isNatThreadStart)
			{
				IPACMDBG("creating nat threads\n");
				CreateNatThreads();
			}
		}
	}
}
#endif

void IPACM_ConntrackListener::TriggerWANUp(void *in_param)
{
	 ipacm_event_iface_up *wanup_data = (ipacm_event_iface_up *)in_param;
	 uint8_t mux_id;

	 IPACMDBG_H("Recevied below information during wanup,\n");
	 IPACMDBG_H("if_name:%s, ipv4_address:0x%x mux_id:%d, xlat_mux_id:%d\n",
						wanup_data->ifname, wanup_data->ipv4_addr, wanup_data->mux_id,
						wanup_data->xlat_mux_id);

	 if(wanup_data->ipv4_addr == 0)
	 {
		 IPACMERR("Invalid ipv4 address,ignoring IPA_HANDLE_WAN_UP event\n");
		 return;
	 }

	 WanUp = true;
	 isStaMode = wanup_data->is_sta;
	 IPACMDBG("isStaMode: %d\n", isStaMode);

	 wan_ipaddr = wanup_data->ipv4_addr;
	 memcpy(wan_ifname, wanup_data->ifname, sizeof(wan_ifname));

	 if(nat_inst != NULL)
	 {
	   if (wanup_data->mux_id == 0)
	   	 mux_id = wanup_data->xlat_mux_id;
	   else
	   	 mux_id = wanup_data->mux_id;
#ifdef FEATURE_VLAN_MPDN
		 nat_inst->AddPdn(wanup_data->ipv4_addr, mux_id, isStaMode);
#else
		 nat_inst->AddTable(wanup_data->ipv4_addr, mux_id, isStaMode);
#endif
	 }

	 IPACMDBG("creating nat threads\n");
	 CreateNatThreads();
}

void IPACM_ConntrackListener::TriggerWANUp_v6(const ipacm_event_iface_up* evt_data)
{
	IPACMDBG_H("\n");
	if (!IsIpv6CTEnabled())
	{
		IPACMDBG("Ignoring\n");
		return;
	}

	if (!wan_ipaddr_v6.Valid())
	{
		IPACMERR("Invalid WAN address,ignoring WAN UP event\n");
		return;
	}

	IPACMDBG_H("Recevied below information during wanup\n");
	IPACMDBG_H("if_name: %s", evt_data->ifname);
	wan_ipaddr_v6.DebugDump("WAN");

	isStaMode = evt_data->is_sta;
	IPACMDBG_H("isStaMode: %d\n", isStaMode);

	memcpy(wan_ifname, evt_data->ifname, sizeof(wan_ifname));

	ipv6ct_inst->AddTable(wan_ipaddr_v6);

	IPACMDBG_H("creating nat threads\n");
	CreateNatThreads();

	WanUp_v6 = true;

	IPACMDBG_H("return\n");
}

int IPACM_ConntrackListener::CreateConnTrackThreads(void)
{
	int ret;
	pthread_t tcp_thread = 0, udp_thread = 0;

	if(isCTReg == false)
	{
		ret = pthread_create(&tcp_thread, NULL, IPACM_ConntrackClient::TCPRegisterWithConnTrack, NULL);
		if(0 != ret)
		{
			IPACMERR("unable to create TCP conntrack event listner thread\n");
			PERROR("unable to create TCP conntrack\n");
			goto error;
		}

		IPACMDBG("created TCP conntrack event listner thread\n");
		if(pthread_setname_np(tcp_thread, "tcp ct listener") != 0)
		{
			IPACMERR("unable to set thread name\n");
		}

		ret = pthread_create(&udp_thread, NULL, IPACM_ConntrackClient::UDPRegisterWithConnTrack, NULL);
		if(0 != ret)
		{
			IPACMERR("unable to create UDP conntrack event listner thread\n");
			PERROR("unable to create UDP conntrack\n");
			goto error;
		}

		IPACMDBG("created UDP conntrack event listner thread\n");
		if(pthread_setname_np(udp_thread, "udp ct listener") != 0)
		{
			IPACMERR("unable to set thread name\n");
		}

		isCTReg = true;
	}

	return 0;

error:
	return -1;
}
int IPACM_ConntrackListener::CreateNatThreads(void)
{
	int ret;
	pthread_t udpcto_thread = 0;

	if(isNatThreadStart == false)
	{
		ret = pthread_create(&udpcto_thread, NULL, IPACM_ConntrackClient::UDPConnTimeoutUpdate, NULL);
		if(0 != ret)
		{
			IPACMERR("unable to create udp conn timeout thread\n");
			PERROR("unable to create udp conn timeout\n");
			goto error;
		}

		IPACMDBG("created upd conn timeout thread\n");
		if(pthread_setname_np(udpcto_thread, "udp conn timeout") != 0)
		{
			IPACMERR("unable to set thread name\n");
		}

		isNatThreadStart = true;
	}
	return 0;

error:
	return -1;
}

#ifdef FEATURE_VLAN_MPDN
void IPACM_ConntrackListener::HandleVlanDown(void *in_param)
{
	ipacm_event_vlan_pdn *vlanup_data = (ipacm_event_vlan_pdn *)in_param;
	IPACMDBG_H("Recevied below information during VLAN DOWN up,\n");
	IPACMDBG_H("IPType: %d, vlan_id:%d, mux id %d\n",
		vlanup_data->iptype,
		vlanup_data->VlanID,
		vlanup_data->mux_id);
	if(nat_inst == NULL)
	{
		IPACMERR(" no nat_inst\n");
		return;
	}

	if((vlanup_data->iptype == IPA_IP_v4) ||
		(vlanup_data->iptype == IPA_IP_MAX))
	{
		/* VLAN PDN down is triggered only on LINK_DOWN, we can safely remove the PDN */
		IPACMDBG_H("removing PDN ipv4 address 0x%X\n", vlanup_data->ipv4_addr);
		nat_inst->RemovePdn(vlanup_data->ipv4_addr);

		for(int i = 0; i < IPA_MAX_NUM_HW_PDNS; i++)
		{
			if(vlan_pdns[i].public_ip == vlanup_data->ipv4_addr)
			{
				IPACMDBG_H("removing pdn entry in %d\n", i);
				vlan_pdns[i].public_ip = 0;
				vlan_pdns[i].vlan_id = 0;
				num_vlan_pdns--;
				break;
			}
		}
	}
}
#endif
void IPACM_ConntrackListener::TriggerWANDown(uint32_t wan_addr)
{
#ifdef FEATURE_VLAN_MPDN
	IPACMDBG_H("Removing default ipv4 pdn with");
#else
	IPACMDBG_H("Deleting ipv4 nat table with");
#endif
	IPACMDBG_H(" public ip address(0x%x): %d.%d.%d.%d\n", wan_addr,
		    ((wan_addr>>24) & 0xFF), ((wan_addr>>16) & 0xFF),
		    ((wan_addr>>8) & 0xFF), (wan_addr & 0xFF));

	WanUp = false;

	if(nat_inst != NULL)
	{
#ifdef FEATURE_VLAN_MPDN
		nat_inst->RemovePdn(wan_addr);
#else
		nat_inst->DeleteTable(wan_addr);
#endif
	}
}

void IPACM_ConntrackListener::TriggerWANDown_v6(const IpAddress& wan_addr)
{
	IPACMDBG_H("\n");
	if (!IsIpv6CTEnabled())
	{
		IPACMDBG("Ignoring\n");
		return;
	}

	WanUp_v6 = false;

	if (wan_addr != wan_ipaddr_v6)
	{
		IPACMDBG_H("WAN IP address is not matching\n");
		return;
	}

	wan_addr.DebugDump("Deleting the table with");
	ipv6ct_inst->DeleteTable(wan_addr);
	IPACMDBG_H("return\n");
}

void ParseCTMessage(struct nf_conntrack *ct)
{
	 uint32_t status, timeout;
	 IPACMDBG("Printing conntrack parameters\n");

	 iptodot("ATTR_IPV4_SRC = ATTR_ORIG_IPV4_SRC:", nfct_get_attr_u32(ct, ATTR_ORIG_IPV4_SRC));
	 iptodot("ATTR_IPV4_DST = ATTR_ORIG_IPV4_DST:", nfct_get_attr_u32(ct, ATTR_ORIG_IPV4_DST));
	 IPACMDBG("ATTR_PORT_SRC = ATTR_ORIG_PORT_SRC: 0x%x\n", nfct_get_attr_u16(ct, ATTR_ORIG_PORT_SRC));
	 IPACMDBG("ATTR_PORT_DST = ATTR_ORIG_PORT_DST: 0x%x\n", nfct_get_attr_u16(ct, ATTR_ORIG_PORT_DST));

	 iptodot("ATTR_REPL_IPV4_SRC:", nfct_get_attr_u32(ct, ATTR_REPL_IPV4_SRC));
	 iptodot("ATTR_REPL_IPV4_DST:", nfct_get_attr_u32(ct, ATTR_REPL_IPV4_DST));
	 IPACMDBG("ATTR_REPL_PORT_SRC: 0x%x\n", nfct_get_attr_u16(ct, ATTR_REPL_PORT_SRC));
	 IPACMDBG("ATTR_REPL_PORT_DST: 0x%x\n", nfct_get_attr_u16(ct, ATTR_REPL_PORT_DST));

	 iptodot("ATTR_SNAT_IPV4:", nfct_get_attr_u32(ct, ATTR_SNAT_IPV4));
	 iptodot("ATTR_DNAT_IPV4:", nfct_get_attr_u32(ct, ATTR_DNAT_IPV4));
	 IPACMDBG("ATTR_SNAT_PORT: 0x%x\n", nfct_get_attr_u16(ct, ATTR_SNAT_PORT));
	 IPACMDBG("ATTR_DNAT_PORT: 0x%x\n", nfct_get_attr_u16(ct, ATTR_DNAT_PORT));

	 IPACMDBG("ATTR_MARK: 0x%x\n", nfct_get_attr_u32(ct, ATTR_MARK));
	 IPACMDBG("ATTR_USE: 0x%x\n", nfct_get_attr_u32(ct, ATTR_USE));
	 IPACMDBG("ATTR_ID: 0x%x\n", nfct_get_attr_u32(ct, ATTR_ID));

	 status = nfct_get_attr_u32(ct, ATTR_STATUS);
	 IPACMDBG("ATTR_STATUS: 0x%x\n", status);

	 timeout = nfct_get_attr_u32(ct, ATTR_TIMEOUT);
	 IPACMDBG("ATTR_TIMEOUT: 0x%x\n", timeout);

	 if(IPS_SRC_NAT & status)
	 {
			IPACMDBG("IPS_SRC_NAT set\n");
	 }

	 if(IPS_DST_NAT & status)
	 {
			IPACMDBG("IPS_DST_NAT set\n");
	 }

	 if(IPS_SRC_NAT_DONE & status)
	 {
			IPACMDBG("IPS_SRC_NAT_DONE set\n");
	 }

	 if(IPS_DST_NAT_DONE & status)
	 {
			IPACMDBG(" IPS_DST_NAT_DONE set\n");
	 }

	 IPACMDBG("\n");
	 return;
}

void ParseCTV6Message(struct nf_conntrack *ct)
{
	 uint32_t status, timeout;
	 struct nfct_attr_grp_ipv6 orig_params;
	 uint8_t l4proto, tcp_flags, tcp_state;

	 IPACMDBG("Printing conntrack parameters\n");

	 nfct_get_attr_grp(ct, ATTR_GRP_ORIG_IPV6, (void *)&orig_params);
	 IPACMDBG("Orig src_v6_addr: 0x%08x%08x%08x%08x\n", orig_params.src[0], orig_params.src[1],
                	orig_params.src[2], orig_params.src[3]);
	IPACMDBG("Orig dst_v6_addr: 0x%08x%08x%08x%08x\n", orig_params.dst[0], orig_params.dst[1],
                	orig_params.dst[2], orig_params.dst[3]);

	 IPACMDBG("ATTR_PORT_SRC = ATTR_ORIG_PORT_SRC: 0x%x\n", nfct_get_attr_u16(ct, ATTR_ORIG_PORT_SRC));
	 IPACMDBG("ATTR_PORT_DST = ATTR_ORIG_PORT_DST: 0x%x\n", nfct_get_attr_u16(ct, ATTR_ORIG_PORT_DST));

	 IPACMDBG("ATTR_MARK: 0x%x\n", nfct_get_attr_u32(ct, ATTR_MARK));
	 IPACMDBG("ATTR_USE: 0x%x\n", nfct_get_attr_u32(ct, ATTR_USE));
	 IPACMDBG("ATTR_ID: 0x%x\n", nfct_get_attr_u32(ct, ATTR_ID));

	 timeout = nfct_get_attr_u32(ct, ATTR_TIMEOUT);
	 IPACMDBG("ATTR_TIMEOUT: 0x%x\n", timeout);

	 status = nfct_get_attr_u32(ct, ATTR_STATUS);
	 IPACMDBG("ATTR_STATUS: 0x%x\n", status);

	 l4proto = nfct_get_attr_u8(ct, ATTR_ORIG_L4PROTO);
	 IPACMDBG("ATTR_ORIG_L4PROTO: 0x%x\n", l4proto);
	 if(l4proto == IPPROTO_TCP)
	 {
		tcp_state = nfct_get_attr_u8(ct, ATTR_TCP_STATE);
		IPACMDBG("ATTR_TCP_STATE: 0x%x\n", tcp_state);

		tcp_flags =  nfct_get_attr_u8(ct, ATTR_TCP_FLAGS_ORIG);
		IPACMDBG("ATTR_TCP_FLAGS_ORIG: 0x%x\n", tcp_flags);
	 }

	 IPACMDBG("\n");
	 return;
}

#ifdef CT_OPT
void IPACM_ConntrackListener::ProcessCTV6Message(void *param)
{
	ipacm_ct_evt_data *evt_data = (ipacm_ct_evt_data *)param;
	u_int8_t l4proto = 0;
	uint32_t status = 0;
	struct nf_conntrack *ct = evt_data->ct;

#ifdef IPACM_DEBUG
	 char buf[1024];

	 /* Process message and generate ioctl call to kernel thread */
	 nfct_snprintf(buf, sizeof(buf), evt_data->ct,
								 evt_data->type, NFCT_O_PLAIN, NFCT_OF_TIME);
	 IPACMDBG("%s\n", buf);
	 IPACMDBG("\n");
	 ParseCTV6Message(ct);
#endif

	if(p_lan2lan == NULL)
	{
		IPACMERR("Lan2Lan Instance is null\n");
		goto IGNORE;
	}

	status = nfct_get_attr_u32(ct, ATTR_STATUS);
	if((IPS_DST_NAT & status) || (IPS_SRC_NAT & status))
	{
		IPACMDBG("Either Destination or Source nat flag Set\n");
		goto IGNORE;
	}

	l4proto = nfct_get_attr_u8(ct, ATTR_ORIG_L4PROTO);
	if(IPPROTO_UDP != l4proto && IPPROTO_TCP != l4proto)
	{
		 IPACMDBG("Received unexpected protocl %d conntrack message\n", l4proto);
		 goto IGNORE;
	}

	IPACMDBG("Neither Destination nor Source nat flag Set\n");
	struct nfct_attr_grp_ipv6 orig_params;
	nfct_get_attr_grp(ct, ATTR_GRP_ORIG_IPV6, (void *)&orig_params);

	ipacm_event_connection lan2lan_conn;
	lan2lan_conn.iptype = IPA_IP_v6;
	memcpy(lan2lan_conn.src_ipv6_addr, orig_params.src,
				 sizeof(lan2lan_conn.src_ipv6_addr));
    IPACMDBG("Before convert, src_v6_addr: 0x%08x%08x%08x%08x\n", lan2lan_conn.src_ipv6_addr[0], lan2lan_conn.src_ipv6_addr[1],
                	lan2lan_conn.src_ipv6_addr[2], lan2lan_conn.src_ipv6_addr[3]);
    for(int cnt=0; cnt<4; cnt++)
	{
	   lan2lan_conn.src_ipv6_addr[cnt] = ntohl(lan2lan_conn.src_ipv6_addr[cnt]);
	}
	IPACMDBG("After convert src_v6_addr: 0x%08x%08x%08x%08x\n", lan2lan_conn.src_ipv6_addr[0], lan2lan_conn.src_ipv6_addr[1],
                	lan2lan_conn.src_ipv6_addr[2], lan2lan_conn.src_ipv6_addr[3]);

	memcpy(lan2lan_conn.dst_ipv6_addr, orig_params.dst,
				 sizeof(lan2lan_conn.dst_ipv6_addr));
	IPACMDBG("Before convert, dst_ipv6_addr: 0x%08x%08x%08x%08x\n", lan2lan_conn.dst_ipv6_addr[0], lan2lan_conn.dst_ipv6_addr[1],
                	lan2lan_conn.dst_ipv6_addr[2], lan2lan_conn.dst_ipv6_addr[3]);
    for(int cnt=0; cnt<4; cnt++)
	{
	   lan2lan_conn.dst_ipv6_addr[cnt] = ntohl(lan2lan_conn.dst_ipv6_addr[cnt]);
	}
	IPACMDBG("After convert, dst_ipv6_addr: 0x%08x%08x%08x%08x\n", lan2lan_conn.dst_ipv6_addr[0], lan2lan_conn.dst_ipv6_addr[1],
                	lan2lan_conn.dst_ipv6_addr[2], lan2lan_conn.dst_ipv6_addr[3]);

	if(((IPPROTO_UDP == l4proto) && (NFCT_T_NEW == evt_data->type)) ||
		 ((IPPROTO_TCP == l4proto) &&
			(nfct_get_attr_u8(ct, ATTR_TCP_STATE) == TCP_CONNTRACK_ESTABLISHED))
		 )
	{
			p_lan2lan->handle_new_connection(&lan2lan_conn);
	}
	else if((IPPROTO_UDP == l4proto && NFCT_T_DESTROY == evt_data->type) ||
					(IPPROTO_TCP == l4proto &&
					 nfct_get_attr_u8(ct, ATTR_TCP_STATE) == TCP_CONNTRACK_FIN_WAIT))
	{
			p_lan2lan->handle_del_connection(&lan2lan_conn);
	}

IGNORE:
	/* Cleanup item that was allocated during the original CT callback */
	nfct_destroy(ct);
	return;
}
#endif

void IPACM_ConntrackListener::ProcessCTMessage(void *param)
{
	 ipacm_ct_evt_data *evt_data = (ipacm_ct_evt_data *)param;
	 u_int8_t l4proto = 0;

#ifdef IPACM_DEBUG
	 char buf[1024];
	 unsigned int out_flags;

	 /* Process message and generate ioctl call to kernel thread */
	 out_flags = (NFCT_OF_SHOW_LAYER3 | NFCT_OF_TIME | NFCT_OF_ID);
	 nfct_snprintf(buf, sizeof(buf), evt_data->ct,
								 evt_data->type, NFCT_O_PLAIN, out_flags);
	 IPACMDBG_H("%s\n", buf);

	 ParseCTMessage(evt_data->ct);
#endif

	 l4proto = nfct_get_attr_u8(evt_data->ct, ATTR_ORIG_L4PROTO);
	 if(IPPROTO_UDP != l4proto && IPPROTO_TCP != l4proto)
	 {
			IPACMDBG("Received unexpected protocl %d conntrack message\n", l4proto);
	 }
	 else
	 {
			ProcessTCPorUDPMsg(evt_data->ct, evt_data->type, l4proto);
	 }

	 /* Cleanup item that was allocated during the original CT callback */
	 nfct_destroy(evt_data->ct);
	 return;
}

void IPACM_ConntrackListener::ProcessCTMessage_v6(const ipacm_ct_evt_data* evt_data, const NatEntryBase& entry)
{
	IPACMDBG_H("\n");
#ifdef IPACM_DEBUG
	char buf[1024];

	/* Process message and generate ioctl call to kernel thread */
	nfct_snprintf(buf, sizeof(buf), evt_data->ct, evt_data->type, NFCT_O_PLAIN, NFCT_OF_TIME);
	IPACMDBG("%s\n", buf);
#endif

	if (entry.Valid())
	{
		ProcessTCPorUDPMsg_v6(evt_data, entry);
	}

	/* Cleanup item that was allocated during the original CT callback */
	nfct_destroy(evt_data->ct);
	IPACMDBG_H("return\n");
}

bool IPACM_ConntrackListener::AddIface(
   nat_table_entry *rule, bool *isTempEntry)
{
	int cnt;

	*isTempEntry = false;

	/* Special handling for Passthrough IP. */
	if (IPACM_Iface::ipacmcfg->ipacm_ip_passthrough_mode)
	{
		if (rule->private_ip == IPACM_Wan::getWANIP())
		{
			IPACMDBG("In Passthrough mode and entry matched with Wan IP (0x%x)\n",
				rule->private_ip);
			return true;
		}
	}

	if (nat_inst == NULL)
	{
		IPACMERR("Nat instance is NULL, unable to check ALG\n");
		return false;
	}

	/* Handle ALG port traffic */
	if(nat_inst->isAlgPort(rule->protocol, rule->private_port) ||
		 nat_inst->isAlgPort(rule->protocol, rule->target_port)) {

		IPACMDBG("ALG port connection, prot=%u, private_port=%u, target_port=%u\n",
			rule->protocol, rule->private_port, rule->target_port);

		if (isStaMode) {
			IPACMDBG("ALG port in STA mode, ignore the event\n");
			return false;
		}

		if (!rule->dst_nat) {
			IPACMDBG("ALG port with src NAT event, ignore it\n");
			return false;
		}

		IPACMDBG("Install dummy NAT rule for ALG port DL flow public_ip=%u public_port=%u\n",
			rule->public_ip, rule->public_port);
		rule->private_ip = rule->public_ip;
		rule->private_port = rule->public_port;
		return true;
	}

	/* check whether nat iface or not */
	for (cnt = 0; cnt < MAX_IFACE_ADDRESS; cnt++)
	{
		if (nat_clients[cnt].nat_iface_ipv4_addr != 0)
		{
			if (rule->private_ip == nat_clients[cnt].nat_iface_ipv4_addr ||
				rule->target_ip == nat_clients[cnt].nat_iface_ipv4_addr)
			{
				IPACMDBG("matched nat_clients[%d].nat_iface_ipv4_addr\n", cnt);
				iptodot("AddIface(): Nat entry match with ip addr",
					nat_clients[cnt].nat_iface_ipv4_addr);
				return true;
			}
		}
	}

	if (!isStaMode)
	{
		/* check whether non nat iface or not, on Non Nat iface
		   add dummy rule by copying public ip to private ip */
		for (cnt = 0; cnt < MAX_IFACE_ADDRESS; cnt++)
		{
			if (nonnat_iface_ipv4_addr[cnt] != 0)
			{
				if (rule->private_ip == nonnat_iface_ipv4_addr[cnt] ||
					rule->target_ip == nonnat_iface_ipv4_addr[cnt])
				{
					IPACMDBG("matched non_nat_iface_ipv4_addr entry(%d)\n", cnt);
					iptodot("AddIface(): Non Nat entry match with ip addr",
							nonnat_iface_ipv4_addr[cnt]);

					rule->private_ip = rule->public_ip;
					rule->private_port = rule->public_port;
					return true;
				}
			}
		}
		IPACMDBG_H("Not mtaching with non-nat ifaces\n");
	}
	else
		IPACMDBG("In STA mode, don't compare against non nat ifaces\n");

	if(pConfig == NULL)
	{
		pConfig = IPACM_Config::GetInstance();
		if(pConfig == NULL)
		{
			IPACMERR("Unable to get Config instance\n");
			return false;
		}
	}

	if (pConfig->isPrivateSubnet(rule->private_ip) ||
		pConfig->isPrivateSubnet(rule->target_ip))
	{
		IPACMDBG("Matching with Private subnet\n");
		*isTempEntry = true;
		return true;
	}

	return false;
}

int IPACM_ConntrackListener::AddORDeleteNatEntry(const nat_entry_bundle *input, bool *sendVlanEvent)
{
	u_int8_t tcp_state;
	u_int64_t pkt_count = 0;
	uint32_t pkt_threshld = GetPacketThreshhold();

	if (nat_inst == NULL)
	{
		IPACMERR("Nat instance is NULL, unable to add or delete\n");
		return IPACM_FAILURE;
	}
#ifdef FEATURE_VLAN_MPDN
	if(!sendVlanEvent)
	{
		IPACMERR("sendVlanEvent is NULL\n");
		return IPACM_FAILURE;
	}
#endif

	IPACMDBG_H("Below Nat Entry will either be added or deleted\n");
	iptodot("AddORDeleteNatEntry(): target ip or dst ip",
			input->rule->target_ip);
	IPACMDBG("target port or dst port: 0x%x Decimal:%d\n",
			 input->rule->target_port, input->rule->target_port);
	iptodot("AddORDeleteNatEntry(): private ip or src ip",
			input->rule->private_ip);
	IPACMDBG("private port or src port: 0x%x, Decimal:%d\n",
			 input->rule->private_port, input->rule->private_port);
	IPACMDBG("public port or reply dst port: 0x%x, Decimal:%d\n",
			 input->rule->public_port, input->rule->public_port);
	IPACMDBG("Protocol: %d, destination nat flag: %d\n",
			 input->rule->protocol, input->rule->dst_nat);
#ifdef FEATURE_VLAN_MPDN
	IPACMDBG("isVlan %d, IsVlanUp %d\n", input->isVlan, input->IsVlanUp);
#endif

	pkt_count = nfct_get_attr_u64(input->ct, ATTR_ORIG_COUNTER_PACKETS) +
				nfct_get_attr_u64(input->ct, ATTR_REPL_COUNTER_PACKETS);

	if (IPPROTO_TCP == input->rule->protocol)
	{
		tcp_state = nfct_get_attr_u8(input->ct, ATTR_TCP_STATE);
		if ((TCP_CONNTRACK_ESTABLISHED == tcp_state) &&
                    (((pkt_threshld != 0) && (pkt_count >= pkt_threshld)) ||
                    (pkt_threshld == 0)))
		{
			IPACMDBG("TCP state TCP_CONNTRACK_ESTABLISHED(%d)\n", tcp_state);
#ifdef FEATURE_VLAN_MPDN
			if(input->isVlan)
			{
				if(!input->IsVlanUp)
				{
					IPACMDBG_H("Detected VLAN WAN UP\n");
					*sendVlanEvent = true;
					IPACMDBG_H("vlan Wan is not up, cache connections\n");
					nat_inst->CacheEntry(input->rule);
				}
				else if(input->isTempEntry)
				{
					IPACMDBG("TCP: adding temp for vlan\n");
					nat_inst->AddTempEntry(input->rule);
				}
				else
				{
					IPACMDBG("TCP: adding entry for vlan\n");
					nat_inst->AddEntry(input->rule);
				}
			} else
#endif
			if (!WanUp)
			{
				IPACMDBG("Wan is not up, cache connections\n");
				nat_inst->CacheEntry(input->rule);
			}
			else if (input->isTempEntry)
			{
				nat_inst->AddTempEntry(input->rule);
			}
			else
			{
				nat_inst->AddEntry(input->rule);
			}
		}
		else if (TCP_CONNTRACK_FIN_WAIT == tcp_state ||
				   input->type == NFCT_T_DESTROY)
		{
			IPACMDBG("TCP state TCP_CONNTRACK_FIN_WAIT(%d) "
					 "or type NFCT_T_DESTROY(%d)\n", tcp_state, input->type);

			nat_inst->DeleteEntry(input->rule);
			nat_inst->DeleteTempEntry(input->rule);
		}
		else
		{
			IPACMDBG("Ignore tcp state: %d and type: %d\n",
					 tcp_state, input->type);
		}

	}
	else if (IPPROTO_UDP == input->rule->protocol)
	{
		if (((NFCT_T_NEW == input->type || NFCT_T_UPDATE == input->type)
			&& (pkt_threshld == 0)) ||
			((pkt_threshld != 0) && (pkt_count >= pkt_threshld)
			&& (NFCT_T_UPDATE == input->type)))
		{
			IPACMDBG("New UDP connection at time %ld\n", time(NULL));
#ifdef FEATURE_VLAN_MPDN
			if(input->isVlan)
			{
				if(!input->IsVlanUp)
				{
					IPACMDBG_H("Detected VLAN WAN UP\n");
					*sendVlanEvent = true;
					IPACMDBG_H("vlan Wan is not up, cache connections\n");
					nat_inst->CacheEntry(input->rule);
				}
				else if(input->isTempEntry)
				{
					IPACMDBG("UDP: adding temp for vlan\n");
					nat_inst->AddTempEntry(input->rule);
				}
				else
				{
					IPACMDBG("UDP: adding entry for vlan\n");
					nat_inst->AddEntry(input->rule);
				}
			}
			else
#endif
			if (!WanUp)
			{
				IPACMDBG("Wan is not up, cache connections\n");
				nat_inst->CacheEntry(input->rule);
			}
			else if (input->isTempEntry)
			{
				nat_inst->AddTempEntry(input->rule);
			}
			else
			{
				nat_inst->AddEntry(input->rule);
			}
		}
		else if (NFCT_T_DESTROY == input->type)
		{
			IPACMDBG("UDP connection close at time %ld\n", time(NULL));
			nat_inst->DeleteEntry(input->rule);
			nat_inst->DeleteTempEntry(input->rule);
		}
		else
		{
			IPACMDBG("Ignore udp, count: %d and type: %d\n",
				pkt_count, input->type);
		}
	}

	return IPACM_SUCCESS;
}

void IPACM_ConntrackListener::AddORDeleteNatEntry_v6(const ipacm_ct_evt_data* evt_data,
	const NatEntryBase& entry, bool isTempEntry)
{
	IPACMDBG_H("\n");

	uint32_t pkt_threshld = GetPacketThreshhold();
	uint64_t pkt_count = nfct_get_attr_u64(evt_data->ct, ATTR_ORIG_COUNTER_PACKETS) +
		nfct_get_attr_u64(evt_data->ct, ATTR_REPL_COUNTER_PACKETS);

	if (IPPROTO_TCP == entry.m_protocol)
	{
		uint8_t tcp_state = nfct_get_attr_u8(evt_data->ct, ATTR_TCP_STATE);
		if (TCP_CONNTRACK_ESTABLISHED == tcp_state && pkt_count >= pkt_threshld)
		{
			IPACMDBG_H("TCP state TCP_CONNTRACK_ESTABLISHED(%d)\n", tcp_state);
			if (!WanUp_v6)
			{
				IPACMDBG_H("Wan is not up, cache connections\n");
				ipv6ct_inst->CacheEntry(entry);
			}
			else if (isTempEntry)
			{
				ipv6ct_inst->AddTempEntry(entry);
			}
			else
			{
				ipv6ct_inst->AddEntry(entry);
			}
		}
		else if (TCP_CONNTRACK_FIN_WAIT == tcp_state || evt_data->type == NFCT_T_DESTROY)
		{
			IPACMDBG_H("TCP state TCP_CONNTRACK_FIN_WAIT(%d) or type NFCT_T_DESTROY(%d)\n", tcp_state, evt_data->type);

			ipv6ct_inst->DeleteEntry(entry);
			ipv6ct_inst->DeleteTempEntry(entry);
		}
		else
		{
			IPACMDBG_H("Ignore tcp state: %d and type: %d\n",
				tcp_state, evt_data->type);
		}

	}
	else if (IPPROTO_UDP == entry.m_protocol)
	{
		if (((NFCT_T_NEW == evt_data->type || NFCT_T_UPDATE == evt_data->type)
			&& (pkt_threshld == 0)) ||
			((pkt_threshld != 0) && (pkt_count >= pkt_threshld)
			&& (NFCT_T_UPDATE == evt_data->type)))
		{
			IPACMDBG_H("New UDP connection at time %ld\n", time(NULL));
			if (!WanUp_v6)
			{
				IPACMDBG_H("Wan is not up, cache connections\n");
				ipv6ct_inst->CacheEntry(entry);
			}
			else if (isTempEntry)
			{
				ipv6ct_inst->AddTempEntry(entry);
			}
			else
			{
				ipv6ct_inst->AddEntry(entry);
			}
		}
		else if (NFCT_T_DESTROY == evt_data->type)
		{
			IPACMDBG_H("UDP connection close at time %ld\n", time(NULL));
			ipv6ct_inst->DeleteEntry(entry);
			ipv6ct_inst->DeleteTempEntry(entry);
		}
	}
	IPACMDBG_H("return\n");
}

void IPACM_ConntrackListener::PopulateTCPorUDPEntry(
	 struct nf_conntrack *ct,
	 uint32_t status,
	 nat_table_entry *rule)
{
	if (IPS_DST_NAT == status)
	{
		IPACMDBG("Destination NAT\n");
		rule->dst_nat = true;

		IPACMDBG("Parse reply tuple\n");
		rule->target_ip = nfct_get_attr_u32(ct, ATTR_ORIG_IPV4_SRC);
		rule->target_ip = ntohl(rule->target_ip);
		iptodot("PopulateTCPorUDPEntry(): target ip", rule->target_ip);

		/* Retrieve target/dst port */
		rule->target_port = nfct_get_attr_u16(ct, ATTR_ORIG_PORT_SRC);
		rule->target_port = ntohs(rule->target_port);
		if (0 == rule->target_port)
		{
			IPACMDBG("unable to retrieve target port\n");
		}

		/* Retrieve public port */
		rule->public_port = nfct_get_attr_u16(ct, ATTR_ORIG_PORT_DST);
		rule->public_port = ntohs(rule->public_port);

		/* Retrieve src/private ip address */
		rule->private_ip = nfct_get_attr_u32(ct, ATTR_REPL_IPV4_SRC);
		rule->private_ip = ntohl(rule->private_ip);
		iptodot("PopulateTCPorUDPEntry(): private ip", rule->private_ip);
		if (0 == rule->private_ip)
		{
			IPACMDBG("unable to retrieve private ip address\n");
		}

		/* Retrieve src/private port */
		rule->private_port = nfct_get_attr_u16(ct, ATTR_REPL_PORT_SRC);
		rule->private_port = ntohs(rule->private_port);
		if (0 == rule->private_port)
		{
			IPACMDBG("unable to retrieve private port\n");
		}
	}
	else if (IPS_SRC_NAT == status)
	{
		IPACMDBG("Source NAT\n");
		rule->dst_nat = false;

		/* Retrieve target/dst ip address */
		IPACMDBG("Parse source tuple\n");
		rule->target_ip = nfct_get_attr_u32(ct, ATTR_ORIG_IPV4_DST);
		rule->target_ip = ntohl(rule->target_ip);
		iptodot("PopulateTCPorUDPEntry(): target ip", rule->target_ip);
		if (0 == rule->target_ip)
		{
			IPACMDBG("unable to retrieve target ip address\n");
		}
		/* Retrieve target/dst port */
		rule->target_port = nfct_get_attr_u16(ct, ATTR_ORIG_PORT_DST);
		rule->target_port = ntohs(rule->target_port);
		if (0 == rule->target_port)
		{
			IPACMDBG("unable to retrieve target port\n");
		}

		/* Retrieve public port */
		rule->public_port = nfct_get_attr_u16(ct, ATTR_REPL_PORT_DST);
		rule->public_port = ntohs(rule->public_port);
		if (0 == rule->public_port)
		{
			IPACMDBG("unable to retrieve public port\n");
		}

		/* Retrieve src/private ip address */
		rule->private_ip = nfct_get_attr_u32(ct, ATTR_ORIG_IPV4_SRC);
		rule->private_ip = ntohl(rule->private_ip);
		iptodot("PopulateTCPorUDPEntry(): private ip", rule->private_ip);
		if (0 == rule->private_ip)
		{
			IPACMDBG("unable to retrieve private ip address\n");
		}

		/* Retrieve src/private port */
		rule->private_port = nfct_get_attr_u16(ct, ATTR_ORIG_PORT_SRC);
		rule->private_port = ntohs(rule->private_port);
		if (0 == rule->private_port)
		{
			IPACMDBG("unable to retrieve private port\n");
		}
	}

	return;
}

#ifdef CT_OPT
void IPACM_ConntrackListener::HandleLan2Lan(struct nf_conntrack *ct,
	enum nf_conntrack_msg_type type,
	 nat_table_entry *rule)
{
	ipacm_event_connection lan2lan_conn = { 0 };

	if (p_lan2lan == NULL)
	{
		IPACMERR("Lan2Lan Instance is null\n");
		return;
	}

	lan2lan_conn.iptype = IPA_IP_v4;
	lan2lan_conn.src_ipv4_addr = orig_src_ip;
	lan2lan_conn.dst_ipv4_addr = orig_dst_ip;

	if (((IPPROTO_UDP == rule->protocol) && (NFCT_T_NEW == type)) ||
		((IPPROTO_TCP == rule->protocol) && (nfct_get_attr_u8(ct, ATTR_TCP_STATE) == TCP_CONNTRACK_ESTABLISHED)))
	{
		p_lan2lan->handle_new_connection(&lan2lan_conn);
	}
	else if ((IPPROTO_UDP == rule->protocol && NFCT_T_DESTROY == type) ||
			   (IPPROTO_TCP == rule->protocol &&
				nfct_get_attr_u8(ct, ATTR_TCP_STATE) == TCP_CONNTRACK_FIN_WAIT))
	{
		p_lan2lan->handle_del_connection(&lan2lan_conn);
	}
}
#endif

void IPACM_ConntrackListener::CheckSTAClient(
   const nat_table_entry *rule, bool *isTempEntry)
{
	int nCnt;

	/* Check whether target is in STA client list or not if not ignore the connection */
	if (!isStaMode || !StaClntCnt)
	{
		return;
	}

	for (nCnt = 0; nCnt < MAX_STA_CLNT_IFACES; ++nCnt)
	{
		if (sta_clnt_ipv4_addr[nCnt])
		{
			break;
		}
	}

	if (nCnt == MAX_STA_CLNT_IFACES)
	{
		IPACMERR("The STA client IP addresses collection is inconsistent with STA client counter\n");
		return;
	}

	if ((sta_clnt_ipv4_addr[nCnt] & STA_CLNT_SUBNET_MASK) != (rule->target_ip & STA_CLNT_SUBNET_MASK))
	{
		IPACMDBG("STA client subnet mask not matching\n");
		return;
	}

	IPACMDBG("StaClntCnt %d\n", StaClntCnt);
	for (; nCnt < MAX_STA_CLNT_IFACES; ++nCnt)
	{
		if (!sta_clnt_ipv4_addr[nCnt])
		{
			continue;
		}

		IPACMDBG("Comparing trgt_ip 0x%x with sta clnt ip: 0x%x\n", rule->target_ip, sta_clnt_ipv4_addr[nCnt]);
		if (rule->target_ip == sta_clnt_ipv4_addr[nCnt])
		{
			IPACMDBG("Match index %d\n", nCnt);
			return;
		}
	}

	IPACMDBG_H("Not matching with STA Clnt Ip Addrs 0x%x\n", rule->target_ip);
	*isTempEntry = true;
}

void IPACM_ConntrackListener::CheckSTAClient_v6(const NatEntryBase& entry, bool& isTempEntry)
{
	IPACMDBG_H("\n");

	if (!isStaMode || !StaClntCnt_v6)
	{
		return;
	}

	int i;
	for (i = 0; i < MAX_STA_CLNT_IFACES; ++i)
	{
		if (sta_clnt_ipv6_addr[i].Valid())
		{
			break;
		}
	}

	if (i == MAX_STA_CLNT_IFACES)
	{
		IPACMERR("The STA client IP addresses collection is inconsistent with STA client counter\n");
		return;
	}

	const IpAddress& target_ip = entry.GetTargetIp();
	if (!target_ip.IsSameSubnet(sta_clnt_ipv6_addr[i]))
	{
		IPACMDBG("Not STA client\n");
		return;
	}

	if (sta_clnt_ipv6_addr.Find(target_ip) != NULL)
	{
		IPACMDBG_H("The target is in STA client list\n");
		return;
	}

	entry.GetTargetIp().DebugDump("Not matching with STA Clnt Ip Addrs");
	isTempEntry = true;
	IPACMDBG_H("return\n");
}

/* conntrack send in host order and ipa expects in host order */
void IPACM_ConntrackListener::ProcessTCPorUDPMsg(
	 struct nf_conntrack *ct,
	 enum nf_conntrack_msg_type type,
	 u_int8_t l4proto)
{
	 nat_table_entry rule;
	 uint32_t status = 0;
	 uint32_t orig_src_ip, orig_dst_ip;
#ifdef FEATURE_VLAN_MPDN
	 uint32_t public_ip;
	 uint32_t repl_src_ip, repl_dst_ip;
	 bool SendVlanEvent = false;
	 uint8_t VlanID = 0;
	 bool embedded_vlan = false;
#endif
	 bool isAdd = false;

	 nat_entry_bundle nat_entry;
	 nat_entry.isTempEntry = false;
	 nat_entry.ct = ct;
	 nat_entry.type = type;
#ifdef FEATURE_VLAN_MPDN
	 nat_entry.isVlan = false;
	 nat_entry.IsVlanUp = false;
#endif

 	 memset(&rule, 0, sizeof(rule));
	 IPACMDBG("Received type:%d with proto:%d\n", type, l4proto);
	 status = nfct_get_attr_u32(ct, ATTR_STATUS);

	 /* Retrieve Protocol */
	 rule.protocol = nfct_get_attr_u8(ct, ATTR_REPL_L4PROTO);

	 orig_src_ip = nfct_get_attr_u32(ct, ATTR_ORIG_IPV4_SRC);
	 orig_src_ip = ntohl(orig_src_ip);
	 if(orig_src_ip == 0)
	 {
		 IPACMERR("unable to retrieve orig src ip address\n");
		 return;
	 }

	 orig_dst_ip = nfct_get_attr_u32(ct, ATTR_ORIG_IPV4_DST);
	 orig_dst_ip = ntohl(orig_dst_ip);
	 if(orig_dst_ip == 0)
	 {
		 IPACMERR("unable to retrieve orig dst ip address\n");
		 return;
	 }
#ifdef FEATURE_VLAN_MPDN
	 repl_src_ip = nfct_get_attr_u32(ct, ATTR_REPL_IPV4_SRC);
	 repl_src_ip = ntohl(repl_src_ip);
	 if(repl_src_ip == 0)
	 {
		 IPACMERR("unable to retrieve repl src ip address\n");
		 return;
	 }

	 repl_dst_ip = nfct_get_attr_u32(ct, ATTR_REPL_IPV4_DST);
	 repl_dst_ip = ntohl(repl_dst_ip);
	 if(repl_dst_ip == 0)
	 {
		 IPACMERR("unable to retrieve repl dst ip address\n");
		 return;
	 }
#endif

	 if(IPS_DST_NAT & status)
	 {
		 status = IPS_DST_NAT;
#ifdef FEATURE_VLAN_MPDN
		 nat_entry.isVlan = IsVlanIPv4(repl_src_ip, &VlanID);
		 if(nat_entry.isVlan)
		 {
			 int i;

			 nat_entry.IsVlanUp = false;
			 for(i = 0; i < IPA_MAX_NUM_HW_PDNS; i++)
			 {
				 /* check if we already got vlan_pdn_up event for this ip */
				 if(vlan_pdns[i].public_ip == orig_dst_ip)
				 {
					 IPACMDBG_H("DST_NAT: vlan pdn already up for ");
					 iptodot("ip", orig_dst_ip);
					 nat_entry.IsVlanUp = true;
					 break;
				 }
			 }

			 if((i >= IPA_MAX_NUM_HW_PDNS) && (num_vlan_pdns >= IPA_MAX_NUM_HW_PDNS) && (!nat_entry.IsVlanUp))
			 {
				 iptodot("vlan client ip", repl_src_ip);
				 iptodot("pdn ip",orig_dst_ip)
				 IPACMERR("src NAT: can't add more PDN, already got max \n");
				 return;
			 }
			 iptodot("vlan client ip", repl_src_ip);
			 iptodot("pdn ip", orig_dst_ip);
			 IPACMDBG_H("IsVlanUp %d\n", nat_entry.IsVlanUp);
		 }
		 public_ip = orig_dst_ip;
#endif
	 }
	 else if(IPS_SRC_NAT & status)
	 {
		 status = IPS_SRC_NAT;
#ifdef FEATURE_VLAN_MPDN
		 nat_entry.isVlan = IsVlanIPv4(orig_src_ip, &VlanID);
		 if(nat_entry.isVlan)
		 {
			 int i = 0;

			nat_entry.IsVlanUp = false;
			for(int i = 0; i < IPA_MAX_NUM_HW_PDNS; i++)
			{
				/* check if we already got vlan_pdn_up event for this ip */
				if(vlan_pdns[i].public_ip == repl_dst_ip)
				{
					IPACMDBG_H("SRC_NAT: vlan pdn already up for ");
					iptodot("ip", repl_dst_ip);
					nat_entry.IsVlanUp = true;
					break;
				}
			}

			if((i >= IPA_MAX_NUM_HW_PDNS) && (num_vlan_pdns >= IPA_MAX_NUM_HW_PDNS) && (!nat_entry.IsVlanUp))
			{
				iptodot("vlan client ip", orig_src_ip);
				iptodot("pdn ip",repl_dst_ip)
					IPACMERR("dst NAT: can't add more PDN, already got max \n");
				return;
			}
			iptodot("vlan client ip ", orig_src_ip);
			iptodot("pdn ip ", repl_dst_ip)
			IPACMDBG_H("IsVlanUp %d\n", nat_entry.IsVlanUp);
		 }
		 public_ip = repl_dst_ip;
#endif
	 }
	 else
	 {
		 IPACMDBG("Neither Destination nor Source nat flag Set\n");

		if(orig_src_ip == wan_ipaddr)
		{
			IPACMDBG("orig src ip:0x%x equal to wan ip\n",orig_src_ip);
			status = IPS_SRC_NAT;
#ifdef FEATURE_VLAN_MPDN
			public_ip = wan_ipaddr;
#endif
		}
		else if(orig_dst_ip == wan_ipaddr)
		{
			IPACMDBG("orig Dst IP:0x%x equal to wan ip\n",orig_dst_ip);
			status = IPS_DST_NAT;
#ifdef FEATURE_VLAN_MPDN
			public_ip = wan_ipaddr;
#endif
		}
		else
		{
#ifdef FEATURE_VLAN_MPDN
			status = 0;
			/* check if this is an embedded traffic to a secondary PDN */
			for(int i = 0; i < IPA_MAX_NUM_HW_PDNS; i++)
			{
				/* check if we already got vlan_pdn_up event for this ip */
				if(vlan_pdns[i].public_ip == orig_src_ip)
				{
					IPACMDBG("orig src ip:0x%x equal to vlan wan ip\n", orig_src_ip);
					status = IPS_SRC_NAT;
					public_ip = orig_src_ip;
					embedded_vlan = true;
					break;
				}
				else if(vlan_pdns[i].public_ip == orig_dst_ip)
				{
					IPACMDBG("orig Dst IP:0x%x equal to wan ip\n", orig_dst_ip);
					status = IPS_DST_NAT;
					public_ip = orig_dst_ip;
					embedded_vlan = true;
					break;
				}
			}
			if (!status)
#endif
			{
				IPACMDBG_H("Neither orig src ip:0x%x Nor orig Dst IP:0x%x equal to wan ip:0x%x\n",
					orig_src_ip, orig_dst_ip, wan_ipaddr);

#ifdef CT_OPT
				HandleLan2Lan(ct, type, &rule);
#endif
				return;
			}
		}
	 }

	 if(IPS_DST_NAT == status || IPS_SRC_NAT == status)
	 {
		 PopulateTCPorUDPEntry(ct, status, &rule);
#ifdef FEATURE_VLAN_MPDN
		 rule.public_ip = public_ip;
#else
		 rule.public_ip = wan_ipaddr;
#endif
	 }
	 else
	 {
		 IPACMDBG("Neither source Nor destination nat\n");
		 goto IGNORE;
	 }

	 if ((rule.private_ip != wan_ipaddr)
#ifdef FEATURE_VLAN_MPDN
		&& (!embedded_vlan)
#endif
		 )
	 {
		 isAdd = AddIface(&rule, &nat_entry.isTempEntry);
		 if (!isAdd)
		 {
			 goto IGNORE;
		 }
	 }
	 else
	 {
		 if (isStaMode)
		 {
			 IPACMDBG("In STA mode, ignore connections destinated to STA interface\n");
			 goto IGNORE;
		 }

		 IPACMDBG("For embedded connections add dummy nat rule\n");
		 IPACMDBG("Change private port %d to %d\n",
				  rule.private_port, rule.public_port);
		 rule.private_port = rule.public_port;
	 }

	 CheckSTAClient(&rule, &nat_entry.isTempEntry);
	 nat_entry.rule = &rule;
#ifdef FEATURE_VLAN_MPDN
	 AddORDeleteNatEntry(&nat_entry, &SendVlanEvent);
	 if(SendVlanEvent)
	 {
		 ipacm_cmd_q_data evt_data;
		 ipacm_event_route_vlan *data;

		 evt_data.event = IPA_ROUTE_ADD_VLAN_PDN_EVENT;
		 data = (ipacm_event_route_vlan *)malloc(sizeof(ipacm_event_route_vlan));
		 if(!data)
		 {
			 IPACMERR("couldn't allocate memory for new vlan pdn event\n");
			 return;
		 }
		 data->iptype = IPA_IP_v4;
		 data->VlanID = VlanID;
		 data->wan_ipv4_addr = public_ip;
		 evt_data.evt_data = data;
		 IPACMDBG("sending IPA_ROUTE_ADD_VLAN_PDN_EVENT vlan id %d, iptype %d,\n",
			 data->VlanID,
			 data->iptype);
		 iptodot("pdn ip", public_ip);

		 IPACM_EvtDispatcher::PostEvt(&evt_data);
	 }
#else
	 AddORDeleteNatEntry(&nat_entry, NULL);
#endif
	 return;

IGNORE:
	IPACMDBG_H("ignoring below Nat Entry\n");
	iptodot("ProcessTCPorUDPMsg(): target ip or dst ip", rule.target_ip);
	IPACMDBG("target port or dst port: 0x%x Decimal:%d\n", rule.target_port, rule.target_port);
	iptodot("ProcessTCPorUDPMsg(): private ip or src ip", rule.private_ip);
	IPACMDBG("private port or src port: 0x%x, Decimal:%d\n", rule.private_port, rule.private_port);
	IPACMDBG("public port or reply dst port: 0x%x, Decimal:%d\n", rule.public_port, rule.public_port);
	IPACMDBG("Protocol: %d, destination nat flag: %d\n", rule.protocol, rule.dst_nat);
	return;
}

void IPACM_ConntrackListener::ProcessTCPorUDPMsg_v6(const ipacm_ct_evt_data* evt_data, const NatEntryBase& entry)
{
	IPACMDBG_H("Received conntrack event with type: %d\n", evt_data->type);
	entry.DebugDump("with");

	bool isTempEntry = false;
	if (entry.m_direction == NatEntryBase::DirectionUnknown)
	{
		if (!IsIpv6PrivateSubnet(entry.GetClientIp()) && !IsIpv6PrivateSubnet(entry.GetTargetIp()))
		{
			IPACMDBG_H("Ignore the entry\n");
			return;
		}
		isTempEntry = true;
	}
	else
	{
		CheckSTAClient_v6(entry, isTempEntry);
	}

	AddORDeleteNatEntry_v6(evt_data, entry, isTempEntry);
	IPACMDBG_H("return\n");
}

void IPACM_ConntrackListener::HandleSTAClientAddEvt(uint32_t clnt_ip_addr)
{
	 int cnt;
	 IPACMDBG_H("Received STA client 0x%x\n", clnt_ip_addr);

	 if(StaClntCnt >= MAX_STA_CLNT_IFACES)
	 {
		IPACMDBG("Max STA client reached, ignore 0x%x\n", clnt_ip_addr);
		return;
	 }

	 for(cnt=0; cnt<MAX_STA_CLNT_IFACES; cnt++)
	 {
		if(sta_clnt_ipv4_addr[cnt] != 0 &&
		 sta_clnt_ipv4_addr[cnt] == clnt_ip_addr)
		{
			IPACMDBG("Ignoring duplicate one 0x%x\n", clnt_ip_addr);
			break;
		}

		if(sta_clnt_ipv4_addr[cnt] == 0)
		{
			IPACMDBG("Adding STA client 0x%x at Index: %d\n",
					clnt_ip_addr, cnt);
			sta_clnt_ipv4_addr[cnt] = clnt_ip_addr;
			StaClntCnt++;
			IPACMDBG("STA client cnt %d\n", StaClntCnt);
			break;
		}

	 }

	 nat_inst->FlushTempEntries(clnt_ip_addr, true);
	 return;
}

void IPACM_ConntrackListener::HandleSTAClientAddEvt_v6(const IpAddress& ip)
{
	IPACMDBG_H("\n");

	if (!IsIpv6CTEnabled() || !ip.Valid())
	{
		IPACMDBG("Ignoring\n");
		return;
	}

	ip.DebugDump("Add STA client with following address\n");

	if (StaClntCnt_v6 >= MAX_STA_CLNT_IFACES)
	{
		IPACMDBG("Max STA client reached, ignore\n");
		return;
	}

	if (sta_clnt_ipv6_addr.Find(ip) != NULL)
	{
		IPACMDBG("Ignoring duplicate\n");
	}
	else
	{
		++StaClntCnt_v6;
		IpAddress* entry = sta_clnt_ipv6_addr.GetFirstEmpty();
		if (entry)
		{
			*entry = ip;
			IPACMDBG("STA client cnt %d\n", StaClntCnt_v6);
		}
		else
			IPACMDBG_H("Entry is NULL\n");
	}

	ipv6ct_inst->FlushTempEntries(ip, true, false, true);

	IPACMDBG_H("Successfully added STA client\n");
}

void IPACM_ConntrackListener::HandleSTAClientDelEvt(uint32_t clnt_ip_addr)
{
	 int cnt;
	 IPACMDBG_H("Received STA client 0x%x\n", clnt_ip_addr);

	 for(cnt=0; cnt<MAX_STA_CLNT_IFACES; cnt++)
	 {
		if(sta_clnt_ipv4_addr[cnt] != 0 &&
		 sta_clnt_ipv4_addr[cnt] == clnt_ip_addr)
		{
			IPACMDBG("Deleting STA client 0x%x at index: %d\n",
					clnt_ip_addr, cnt);
			sta_clnt_ipv4_addr[cnt] = 0;
			nat_inst->DelEntriesOnSTAClntDiscon(clnt_ip_addr);
			StaClntCnt--;
			IPACMDBG("STA client cnt %d\n", StaClntCnt);
			break;
		}
	 }

	 nat_inst->FlushTempEntries(clnt_ip_addr, false);
   return;
}

void IPACM_ConntrackListener::HandleSTAClientDelEvt_v6(const IpAddress& ip)
{
	IPACMDBG_H("\n");

	if (!IsIpv6CTEnabled() || !ip.Valid())
	{
		IPACMDBG("Ignoring\n");
		return;
	}

	ip.DebugDump("Delete STA client with following address\n");

	IpAddress* entry = sta_clnt_ipv6_addr.Find(ip);
	if (entry == NULL)
	{
		IPACMDBG_H("The received IP is not an STA client\n");
	}
	else
	{
		--StaClntCnt_v6;
		entry->Clear();
		ipv6ct_inst->DelEntriesOnSTAClntDiscon(ip);
		IPACMDBG("STA client cnt %d\n", StaClntCnt_v6);
	}

	ipv6ct_inst->FlushTempEntries(ip, false, false, true);

	IPACMDBG_H("Successfully deleted STA client\n");
}

void IPACM_ConntrackListener::CreateIpv6ctEntryFromCtEventData(const ipacm_ct_evt_data* evt_data,
	Ipv6ctEntry& entry) const
{
	IPACMDBG_H("\n");
	struct nfct_attr_grp_ipv6 orig_params;
	nfct_get_attr_grp(evt_data->ct, ATTR_GRP_ORIG_IPV6, (void *)&orig_params);
	const Ipv6IpAddress srcAddr(orig_params.src, true), dstAddr(orig_params.dst, true);

	uint16_t srcPort = nfct_get_attr_u16(evt_data->ct, ATTR_ORIG_PORT_SRC);
	uint16_t dstPort = nfct_get_attr_u16(evt_data->ct, ATTR_ORIG_PORT_DST);

	entry.m_protocol = nfct_get_attr_u8(evt_data->ct, ATTR_ORIG_L4PROTO);
	if (entry.m_protocol == IPPROTO_UDP)
	{
		IPACMDBG("Received UDP packet\n");
	}
	else if (entry.m_protocol == IPPROTO_TCP)
	{
		IPACMDBG("Received TCP packet\n");
	}
	else
	{
		IPACMDBG("Received unexpected protocl %d conntrack message\n", entry.m_protocol);
		goto bail;
	}

	if (nat_iface_ipv6_addr.Find(srcAddr) != NULL)
	{
		entry.m_direction = NatEntryBase::DirectionOutbound;
	}
	else if (nat_iface_ipv6_addr.Find(dstAddr) != NULL)
	{
		entry.m_direction = NatEntryBase::DirectionInbound;
	}
	else if (srcAddr == wan_ipaddr_v6 || nonnat_iface_ipv6_addr.Find(srcAddr) != NULL)
	{
		if (isStaMode)
		{
			IPACMDBG("Don't install dummy rules in STA mode\n");
			goto bail;
		}

		entry.m_isDummy = true;
		entry.m_direction = NatEntryBase::DirectionOutbound;
	}
	else if (dstAddr == wan_ipaddr_v6 || nonnat_iface_ipv6_addr.Find(dstAddr) != NULL)
	{
		if (isStaMode)
		{
			IPACMDBG("Don't install dummy rules in STA mode\n");
			goto bail;
		}

		entry.m_isDummy = true;
		entry.m_direction = NatEntryBase::DirectionInbound;
	}
	else
	{
		IPACMDBG("Neither source Nor destination NAT. Should be decided during adding the client\n");
		entry.m_direction = NatEntryBase::DirectionUnknown;
	}

	if (entry.m_direction == NatEntryBase::DirectionOutbound || entry.m_direction == NatEntryBase::DirectionUnknown)
	{
		entry.m_srcAddr = srcAddr;
		entry.m_srcPort = srcPort;
		entry.m_dstAddr = dstAddr;
		entry.m_dstPort = dstPort;
	}
	else if (entry.m_direction == NatEntryBase::DirectionInbound)
	{
		entry.m_srcAddr = dstAddr;
		entry.m_srcPort = dstPort;
		entry.m_dstAddr = srcAddr;
		entry.m_dstPort = srcPort;
	}
	else
	{
		IPACMERR("Bad direction %d\n", entry.m_direction);
		goto bail;
	}

	IPACMDBG_H("return\n");
	return;

bail:
	entry.Clear();
}

bool IPACM_ConntrackListener::IsIpv6PrivateSubnet(const IpAddress& ip)
{
	bool ret = false;

	IPACMDBG_H("\n");
#ifdef FEATURE_VLAN_MPDN
	if (pConfig == NULL)
	{
		pConfig = IPACM_Config::GetInstance();
	}

	if (pConfig != NULL)
	{
		const Ipv6IpAddress& ipv6 = static_cast<const Ipv6IpAddress&>(ip);
		for (int i = 0; i < pConfig->num_ipv6_prefixes; ++i)
		{
			if (ipv6.IsSameSubnet(pConfig->ipa_ipv6_prefixes[i]))
			{
				ret = true;
				break;
			}
		}
	}
	else
#endif
	{
		ret = ip.IsSameSubnet(wan_ipaddr_v6);
	}

	IPACMDBG_H("return\n");
	return ret;
}

