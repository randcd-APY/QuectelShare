/*
 * Copyright (c) 2013 - 2018 The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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

#ifndef IPACM_CONNTRACK_LISTENER
#define IPACM_CONNTRACK_LISTENER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#include "IPACM_CmdQueue.h"
#include "IPACM_Conntrack_NATApp.h"
#include "IPACM_Listener.h"
#ifdef CT_OPT
#include "IPACM_LanToLan.h"
#endif

#define MAX_IFACE_ADDRESS 50
#define MAX_STA_CLNT_IFACES 10
#define STA_CLNT_SUBNET_MASK 0xFFFFFF00

using namespace std;

typedef struct _nat_entry_bundle
{
	struct nf_conntrack *ct;
	enum nf_conntrack_msg_type type;
	nat_table_entry *rule;
	bool isTempEntry;
#ifdef FEATURE_VLAN_MPDN
	bool isVlan;
	bool IsVlanUp;
#endif
}nat_entry_bundle;

typedef struct __nat_client_info
{
	uint32_t nat_iface_ipv4_addr;
#ifdef FEATURE_VLAN_MPDN
	bool is_vlan_client;
	uint8_t vlan_id;
#endif
}nat_client_info;

#ifdef FEATURE_VLAN_MPDN
typedef struct _nat_pdn_entry
{
	uint32_t public_ip;
	uint8_t vlan_id;
}nat_pdn_entry;
#endif

class IPACM_ConntrackListener : public IPACM_Listener
{

private:
	bool isCTReg;
	bool isNatThreadStart;
	bool WanUp;
	bool WanUp_v6;
	NatApp *nat_inst;
	NatBase* const ipv6ct_inst;

	int NatIfaceCnt;
	int StaClntCnt;
	int StaClntCnt_v6;
	NatIfaces *pNatIfaces;
	nat_client_info nat_clients[MAX_IFACE_ADDRESS];
	IpAddressesCollectionBase& nat_iface_ipv6_addr;
#ifdef FEATURE_VLAN_MPDN
	nat_pdn_entry vlan_pdns[IPA_MAX_NUM_HW_PDNS];
	int num_vlan_pdns;
#endif
	uint32_t nonnat_iface_ipv4_addr[MAX_IFACE_ADDRESS];
	IpAddressesCollectionBase& nonnat_iface_ipv6_addr;
	uint32_t sta_clnt_ipv4_addr[MAX_STA_CLNT_IFACES];
	IpAddressesCollectionBase& sta_clnt_ipv6_addr;
	IPACM_Config *pConfig;
#ifdef CT_OPT
	IPACM_LanToLan *p_lan2lan;
#endif

	void ProcessCTMessage(void *);
	void ProcessCTMessage_v6(const ipacm_ct_evt_data* evt_data, const NatEntryBase& entry);
	void ProcessTCPorUDPMsg(struct nf_conntrack *,
		enum nf_conntrack_msg_type, u_int8_t);
	void ProcessTCPorUDPMsg_v6(const ipacm_ct_evt_data* evt_data, const NatEntryBase& entry);
	void CreateIpv6ctEntryFromCtEventData(const ipacm_ct_evt_data* evt_data, Ipv6ctEntry& entry) const;
#ifdef FEATURE_VLAN_MPDN
	void HandleVlanUp(void *);
	void HandleVlanDown(void *);
#endif
	void TriggerWANUp(void *);
	void TriggerWANUp_v6(const ipacm_event_iface_up* evt_data);
	void TriggerWANDown(uint32_t);
	void TriggerWANDown_v6(const IpAddress& wan_addr);
	int  CreateNatThreads(void);
	bool AddIface(nat_table_entry *, bool *);
	int AddORDeleteNatEntry(const nat_entry_bundle *, bool *sendVlanEvent);
	void AddORDeleteNatEntry_v6(const ipacm_ct_evt_data* evt_data, const NatEntryBase& entry, bool isTempEntry);
	void PopulateTCPorUDPEntry(struct nf_conntrack *, uint32_t, nat_table_entry *);
	void CheckSTAClient(const nat_table_entry *rule, bool *isTempEntry);
	void CheckSTAClient_v6(const NatEntryBase& entry, bool& isTempEntry);
	int CheckNatIface(int if_index, bool *NatIface);
	void HandleNonNatIPAddr(void *, bool);
	void HandleNonNatIPAddr_v4(void* inParam, bool AddOp);
	void HandleNonNatIPAddr_v6(const IpAddress& ip, int if_index, bool AddOp);
	uint32_t GetPacketThreshhold(void);
	bool IsIpv6PrivateSubnet(const IpAddress& ip);

#ifdef CT_OPT
	void ProcessCTV6Message(void *);
	void HandleLan2Lan(struct nf_conntrack *,
		enum nf_conntrack_msg_type, nat_table_entry* );
#endif

	bool IsIpv6CTEnabled() const
	{
		return ipv6ct_inst != NULL;
	}

public:
	char wan_ifname[IPA_IFACE_NAME_LEN];
	uint32_t wan_ipaddr;
	IpAddress& wan_ipaddr_v6;
	bool isStaMode;
	IPACM_ConntrackListener();
	~IPACM_ConntrackListener();
	void event_callback(ipa_cm_event_id, void *data);
	int  CreateConnTrackThreads(void);

	void HandleNeighIpAddrAddEvt(ipacm_event_data_all *);
	void HandleNeighIpAddrAddEvt_v6(const IpAddress& ip, int if_index);
	void HandleNeighIpAddrDelEvt(uint32_t);
	void HandleNeighIpAddrDelEvt_v6(const IpAddress& ip);
	void HandleSTAClientAddEvt(uint32_t);
	void HandleSTAClientAddEvt_v6(const IpAddress& ip);
	void HandleSTAClientDelEvt(uint32_t);
	void HandleSTAClientDelEvt_v6(const IpAddress& ip);
#ifdef FEATURE_VLAN_MPDN
	bool IsVlanIPv4(uint32_t ipv4_address, uint8_t *VlanId);
#endif
};

extern IPACM_ConntrackListener *CtList;

#endif /* IPACM_CONNTRACK_LISTENER */
