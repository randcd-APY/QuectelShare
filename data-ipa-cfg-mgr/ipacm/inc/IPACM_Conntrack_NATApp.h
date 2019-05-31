/*
Copyright (c) 2013-2018, The Linux Foundation. All rights reserved.

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
#ifndef IPACM_CONNTRACK_NATAPP_H
#define IPACM_CONNTRACK_NATAPP_H

#include <string.h>  /* for stderror */
#include <stdlib.h>
#include <cstdio>  /* for perror */

#include "IPACM_Config.h"
#include "IPACM_Xml.h"

extern "C"
{
#include <libnetfilter_conntrack/libnetfilter_conntrack.h>
}

#define MAX_TEMP_ENTRIES 25

#define IPACM_TCP_FULL_FILE_NAME  "/proc/sys/net/ipv4/netfilter/ip_conntrack_tcp_timeout_established"
#define IPACM_UDP_FULL_FILE_NAME   "/proc/sys/net/ipv4/netfilter/ip_conntrack_udp_timeout_stream"

#ifndef FEATURE_IPA_ANDROID

#define IPACM_TCP_FULL_FILE_NAME_NEW  "/proc/sys/net/netfilter/nf_conntrack_tcp_timeout_established"
#define IPACM_UDP_FULL_FILE_NAME_NEW  "/proc/sys/net/netfilter/nf_conntrack_udp_timeout_stream"

#endif

class IpAddress
{
public:

	virtual ~IpAddress();

	virtual bool Compare(const IpAddress& other) const = 0;
	virtual bool IsSameSubnet(const IpAddress& other) const = 0;
	virtual void Copy(const IpAddress& other) = 0;
	virtual void Clear() = 0;
	virtual bool Valid() const = 0;
	virtual void DebugDump(const char* msg_prefix) const = 0;

	ipa_ip_type GetType() const
	{
		return m_type;
	}

	bool operator==(const IpAddress& other) const
	{
		return Compare(other);
	}

	bool operator!=(const IpAddress& other) const
	{
		return !Compare(other);
	}

	IpAddress& operator=(const IpAddress& other)
	{
		Copy(other);
		return *this;
	}

protected:

	explicit IpAddress(ipa_ip_type type);

	const ipa_ip_type m_type;
};

class Ipv6IpAddress : public IpAddress
{
public:

	Ipv6IpAddress();
	Ipv6IpAddress(const uint32_t* addr, bool inputNetworkEndianness);

	virtual bool Compare(const IpAddress& other) const;
	virtual bool IsSameSubnet(const IpAddress& other) const;
	virtual void Copy(const IpAddress& other);
	virtual void Clear();
	virtual bool Valid() const;
	virtual void DebugDump(const char* msg_prefix) const;

	bool IsSameSubnet(uint32_t* prefix) const;
	void CreateFromArray(const uint32_t* addr, bool inputNetworkEndianness);
	void ToArray(uint32_t* addr, bool outputNetworkEndianness) const;

	uint64_t GetMsb() const
	{
		return m_msb;
	}

	uint64_t GetLsb() const
	{
		return m_lsb;
	}

private:

	static uint64_t Convert2x32to64(const uint32_t* pair32, bool inputNetworkEndianness);
	static void Convert64to2x32(uint64_t in, uint32_t* pair32, bool outputNetworkEndianness);

	uint64_t m_msb;
	uint64_t m_lsb;
};

struct NatEntryBase
{
	enum Direction
	{
		DirectionUnknown,
		DirectionOutbound,
		DirectionInbound,
		DirectionMax
	};

	virtual ~NatEntryBase();
	virtual bool Compare(const NatEntryBase& other) const;
	virtual void Copy(const NatEntryBase& other);
	virtual void Clear();
	virtual bool Valid() const = 0;
	virtual void DebugDump(const char* msg_prefix) const;
	virtual const IpAddress& GetClientIp() const = 0;
	virtual const IpAddress& GetTargetIp() const = 0;

	bool UpdateDirection(const IpAddress& clientIp, bool isStaClientIp);

	bool operator==(const NatEntryBase& other) const
	{
		return Compare(other);
	}

	bool operator!=(const NatEntryBase& other) const
	{
		return !Compare(other);
	}

	NatEntryBase& operator=(const NatEntryBase& other)
	{
		Copy(other);
		return *this;
	}

	const ipa_ip_type m_type;
	uint32_t m_timestamp;
	Direction m_direction;
	uint32_t m_ruleHandle;
	uint8_t m_protocol;
	bool m_enabled;

	/*
	 * Dummy traffic is a traffic that will be sent/received from APPS. Need to install NAT rules to
	 * catch DL traffic that otherwise will NAT miss and go back to Q6. E.g. embedded traffic.
	 */
	bool m_isDummy;

protected:

	explicit NatEntryBase(ipa_ip_type type);

private:

	static const char* DirectionToStr(Direction direction);

	virtual void InvertDirection() = 0;
};

struct Ipv6ctEntry : public NatEntryBase
{
	Ipv6ctEntry();
	virtual bool Compare(const NatEntryBase& other) const;
	virtual void Copy(const NatEntryBase& other);
	virtual void Clear();
	virtual bool Valid() const;
	virtual void DebugDump(const char* msg_prefix) const;
	virtual const IpAddress& GetClientIp() const;
	virtual const IpAddress& GetTargetIp() const;

	Ipv6IpAddress m_srcAddr;
	Ipv6IpAddress m_dstAddr;

	uint16_t m_dstPort;
	uint16_t m_srcPort;

private:

	virtual void InvertDirection();
};

typedef struct _nat_table_entry
{
	uint32_t private_ip;
	uint16_t private_port;

	uint32_t target_ip;
	uint16_t target_port;

	uint32_t public_ip;
	uint16_t public_port;
#ifdef FEATURE_VLAN_MPDN
	uint8_t pdn_index;
#endif

	u_int8_t  protocol;
	uint32_t timestamp;

	bool dst_nat;
	bool enabled;
	uint32_t rule_hdl;

}nat_table_entry;

#define CHK_TBL_HDL()  if(nat_table_hdl == 0){ return -1; }

class CollectionBase
{
public:

	virtual ~CollectionBase();

protected:

	explicit CollectionBase(int max_entries);

	const int m_maxEntries;

private:

	CollectionBase(const CollectionBase&);
	CollectionBase& operator=(const CollectionBase&);
};

template <typename EntryBaseClass>
class GenericCollectionBase : public CollectionBase
{
public:

	virtual EntryBaseClass& Get(int index) = 0;
	virtual const EntryBaseClass& Get(int index) const = 0;

	EntryBaseClass* Find(const EntryBaseClass& other)
	{
		IPACMDBG_H("\n");
		for (int i = 0; i < m_maxEntries; ++i)
		{
			EntryBaseClass& curr = Get(i);
			if (curr == other)
			{
				IPACMDBG_H("return matched entry with index %d\n", i);
				return &curr;
			}
		}

		IPACMDBG_H("Not found\n");
		return NULL;
	}

	const EntryBaseClass* Find(const EntryBaseClass& other) const
	{
		IPACMDBG_H("\n");
		for (int i = 0; i < m_maxEntries; ++i)
		{
			const EntryBaseClass& curr = Get(i);
			if (curr == other)
			{
				IPACMDBG_H("return matched entry with index %d\n", i);
				return &curr;
			}
		}

		IPACMDBG_H("Not found\n");
		return NULL;
	}

	EntryBaseClass* GetFirstEmpty()
	{
		IPACMDBG_H("\n");
		for (int i = 0; i < m_maxEntries; ++i)
		{
			EntryBaseClass& curr = Get(i);
			if (!curr.Valid())
			{
				IPACMDBG_H("return first empty entry with index %d\n", i);
				return &curr;
			}
		}

		IPACMDBG_H("The collection is full, no empty entry was found\n");
		return NULL;
	}

	EntryBaseClass& operator[](int index)
	{
		return Get(index);
	}

	const EntryBaseClass& operator[](int index) const
	{
		return Get(index);
	}

protected:

	explicit GenericCollectionBase(int max_entries) : CollectionBase(max_entries)
	{
	}
};

template <typename Entry, typename EntryBaseClass>
class GenericCollection : public GenericCollectionBase<EntryBaseClass>
{
public:

	explicit GenericCollection(int max_entries) :
		GenericCollectionBase<EntryBaseClass>(max_entries),
		m_arr(new Entry[max_entries])
	{
		memset(m_arr, 0, sizeof(Entry) * max_entries);
	}

	~GenericCollection()
	{
		delete[] m_arr;
	}

	virtual EntryBaseClass& Get(int index)
	{
		return m_arr[index];
	}

	virtual const EntryBaseClass& Get(int index) const
	{
		return m_arr[index];
	}

private:

	Entry* m_arr;
};
typedef GenericCollectionBase<IpAddress> IpAddressesCollectionBase;
typedef GenericCollection<Ipv6IpAddress, IpAddress> Ipv6IpAddressesCollection;

typedef GenericCollectionBase<NatEntryBase> NatEntriesCollectionBase;
typedef GenericCollection<Ipv6ctEntry, NatEntryBase> Ipv6ctEntriesCollection;

class ConntrackTimestampUtil
{
public:

	virtual ~ConntrackTimestampUtil();

	static void Init();

#ifndef FEATURE_IPA_ANDROID
	static void ReadTcpUdpTimeout();
#endif

	int UpdateConntrackTimeStamp(const NatEntryBase& entry);

protected:

	ConntrackTimestampUtil();

	static struct nf_conntrack *ct;

private:

	ConntrackTimestampUtil(const ConntrackTimestampUtil&);
	ConntrackTimestampUtil& operator=(const ConntrackTimestampUtil&);

	virtual void SetConnectionDetails(const NatEntryBase& entry) = 0;

	static struct nfct_handle *ct_hdl;

	static uint32_t tcp_timeout;
	static uint32_t udp_timeout;
};

class Ipv6ctConntrackTimestampUtil : public ConntrackTimestampUtil
{
private:

	virtual void SetConnectionDetails(const NatEntryBase& entry);
};

class NatProxyBase
{
public:

	virtual ~NatProxyBase();
	int AddTable(uint16_t number_of_entries);
	int DeleteTable();
	int AddEntry(NatEntryBase& entry);
	int DelEntry(NatEntryBase& entry);

	virtual int QueryTimestamp(const NatEntryBase& entry, uint32_t& time_stamp) const = 0;
	virtual void DumpTable() = 0;

protected:

	NatProxyBase();

	uint32_t m_tableHandle;

private:

	NatProxyBase(const NatProxyBase&);
	NatProxyBase& operator=(const NatProxyBase&);

	virtual int DoAddTable(uint16_t number_of_entries, uint32_t& table_handle) = 0;
	virtual int DoDeleteTable() = 0;
	virtual int DoAddEntry(const NatEntryBase& entry, uint32_t& entry_handle) = 0;
	virtual int DoDelEntry(const NatEntryBase& entry) = 0;
};

class Ipv6ctProxy : public NatProxyBase
{
public:

	virtual int QueryTimestamp(const NatEntryBase& entry, uint32_t& time_stamp) const;
	virtual void DumpTable();

private:

	virtual int DoAddTable(uint16_t number_of_entries, uint32_t& table_handle);
	virtual int DoDeleteTable();
	virtual int DoAddEntry(const NatEntryBase& entry, uint32_t& entry_handle);
	virtual int DoDelEntry(const NatEntryBase& entry);
};

class NatObjectsGeneratorBase
{
public:

	virtual ~NatObjectsGeneratorBase();
	virtual NatProxyBase& GetProxy() const = 0;
	virtual NatEntriesCollectionBase& GetEntriesCollection(int max_entries) const = 0;
	virtual IpAddressesCollectionBase& GetIpAddressesCollection(int max_entries) const = 0;
	virtual IpAddress& GetIpAddress() const = 0;
	virtual ConntrackTimestampUtil& GetConntrackTimestampUtil() const = 0;

protected:

	NatObjectsGeneratorBase();

private:

	NatObjectsGeneratorBase(const NatObjectsGeneratorBase&);
	NatObjectsGeneratorBase& operator=(const NatObjectsGeneratorBase&);
};

class Ipv6ctObjectsGenerator : public NatObjectsGeneratorBase
{
public:

	virtual NatProxyBase& GetProxy() const;
	virtual NatEntriesCollectionBase& GetEntriesCollection(int max_entries) const;
	virtual IpAddressesCollectionBase& GetIpAddressesCollection(int max_entries) const;
	virtual IpAddress& GetIpAddress() const;
	virtual ConntrackTimestampUtil& GetConntrackTimestampUtil() const;
};

class NatBase
{
public:

	virtual ~NatBase();

	ipa_ip_type GetType() const
	{
		return m_type;
	}

	int AddTable(const IpAddress& wan_ip);
	int DeleteTable(const IpAddress& wan_addr);

	int AddEntry(const NatEntryBase& entry);
	void DeleteEntry(const NatEntryBase& entry);
	void CacheEntry(const NatEntryBase& entry);
	void AddTempEntry(const NatEntryBase& entry);
	void DeleteTempEntry(const NatEntryBase& entry);
	void FlushTempEntries(const IpAddress& clientIp, bool isAdd, bool isDummy, bool isStaClientIp);

	void UpdateTcpUdpTimeStamps(bool& isTcpUdpTimeoutUpToDate);

	int UpdatePwrSaveIf(const IpAddress& client_lan_ip);
	int ResetPwrSaveIf(const IpAddress& client_lan_ip);
	int DelEntriesOnClntDiscon(const IpAddress& client_lan_ip);
	int DelEntriesOnSTAClntDiscon(const IpAddress& client_lan_ip);

protected:

	NatBase(ipa_ip_type type, int max_entries, const NatObjectsGeneratorBase& objectsGenerator);

	const ipa_ip_type m_type;
	NatEntriesCollectionBase& m_temp;
	IpAddressesCollectionBase& m_pwrSaveIfs;

private:

	NatBase(const NatBase&);
	NatBase& operator=(const NatBase&);

	void Reset();

	const int m_maxEntries;
	int m_curCnt;

	NatProxyBase& m_proxy;
	ConntrackTimestampUtil& m_ctTimestampUtil;
	NatEntriesCollectionBase& m_cache;

	IpAddress& m_previousWanAddress;
};

class Ipv6ct : public NatBase
{
public:

	static Ipv6ct* GetInstance();

private:

	explicit Ipv6ct(int max_entries);

	static Ipv6ct* m_instance;
};

class NatApp
{
private:

	static NatApp *pInstance;

	nat_table_entry *cache;
	nat_table_entry temp[MAX_TEMP_ENTRIES];
	uint32_t pub_ip_addr;
	uint32_t pub_ip_addr_pre;
	uint32_t nat_table_hdl;

	int curCnt, max_entries;

	ipacm_alg *pALGPorts;
	uint16_t nALGPort;

	uint32_t tcp_timeout;
	uint32_t udp_timeout;

#ifndef FEATURE_IPA_ANDROID
	static float kernel_ver;
	static bool kernel_ver_updated;
#endif

	uint32_t PwrSaveIfs[IPA_MAX_NUM_WIFI_CLIENTS];

	struct nf_conntrack *ct;
	struct nfct_handle *ct_hdl;

	NatApp();
	int Init();

	void UpdateCTUdpTs(nat_table_entry *, uint32_t);
	bool ChkForDup(const nat_table_entry *);
	void Reset();
	bool isPwrSaveIf(uint32_t);
	uint32_t GenerateMetdata(uint8_t mux_id);

public:
	static NatApp* GetInstance();

#ifdef FEATURE_VLAN_MPDN
	int AddPdn(uint32_t pub_ip, uint8_t mux_id, bool is_sta);
	int RemovePdn(uint32_t pub_ip);
#endif
	bool isAlgPort(uint8_t, uint16_t);

	int AddTable(uint32_t, uint8_t mux_id, bool is_sta);
	int DeleteTable(uint32_t);

	int AddEntry(const nat_table_entry *, bool isVlan = false);
	int DeleteEntry(const nat_table_entry *);

	void UpdateUDPTimeStamp();

	int UpdatePwrSaveIf(uint32_t);
	int ResetPwrSaveIf(uint32_t);
	int DelEntriesOnClntDiscon(uint32_t);
	int DelEntriesOnSTAClntDiscon(uint32_t);

	void Read_TcpUdp_Timeout(void);

	void AddTempEntry(const nat_table_entry *);
	void CacheEntry(const nat_table_entry *);
	void DeleteTempEntry(const nat_table_entry *);
#ifdef FEATURE_VLAN_MPDN
	void FlushAndCacheVlanTempEntries(uint32_t ip_addr, bool *entry_exists, uint32_t *public_ip);
#endif
	void FlushTempEntries(uint32_t, bool, bool isDummy = false);
};

#endif /* IPACM_CONNTRACK_NATAPP_H */
