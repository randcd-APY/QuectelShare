/* $Id: conf.h 1.39 06/05/12 11:48:36+03:00 vnuorval@tcs.hut.fi $ */

#ifndef __CONF_H__
#define __CONF_H__ 1

#include <time.h>
#include <net/if.h>
#include "list.h"
#include "pmgr.h"
#define BASE4DEV "ip4gre1"
#define PMIP_RESERVED_PORT 5436
#define MAX_COMMAND_STRING_LEN 500
#define PMIP_INVALID_ID_STRING "-1"
#define PMIP_DHCP_RESER_FILE "/data/pmip_dhcp_hosts"
#define PMIP_ROUTE_TABLE "/data/iproute2/rt_tables"


typedef enum {
	IP_FAMILY_V4_V01 = 0x04, /**<  IPv4 version  */
	IP_FAMILY_V6_V01 = 0x06, /**<  IPv6 version  */
	IP_FAMILY_V4V6_V01 = 0x0A /**<  Dual mode version  */
}ip_family_enum_v01;

typedef enum {
	PMIPV4_MODE_CPE_V01 = 1, /**<  MIPV4 MODE is CPE  */
	PMIPV4_MODE_SECONDARY_ROUTER_V01 = 2 /**<  PMIPV4 MODE is SECONDARY ROUTER  */
}pmipv4_mode_enum_v01;

typedef enum {
	MOBILE_NODE_IDENTIFIER_TYPE_MAC = 0,     /**<  MOBILE NODE IDENTIFIER TYPE MAC  */
	MOBILE_NODE_IDENTIFIER_TYPE_STRING = 1   /**<  MOBILE NODE IDENTIFIER TYPE custom string  */
}mobile_node_identifier_type_enum_v01;


struct pending_pbu_seq_list {
	struct list_head list;
	uint16_t pbu_seq_no;
};


struct mip6_config {
	/* Common options */
	char            *config_file;
	ip_family_enum_v01              pmip_mode_type;
	pmipv4_mode_enum_v01              pmipv4_work_mode_type;
	ip_family_enum_v01    pmip_tunnel_mode;
	struct in_addr dhcp_start_ip;
	struct in_addr dhcp_end_ip;
	uint32_t current_ip;
	struct in_addr dhcp_sub_mask;
	int is_dnsmasq_started_on_pba;
	int is_dmnp_prefix_verified;
#ifdef ENABLE_VT
	char            *vt_hostname;
	char            *vt_service;
#endif
	unsigned int    mip6_entity;
	unsigned int    debug_level;
	char *          debug_log_file;
	mobile_node_identifier_type_enum_v01    mobile_node_identifier_type;
	struct pmgr_cb  pmgr;
	struct list_head net_ifaces;
	struct list_head bind_acl;
	uint8_t         DefaultBindingAclPolicy;
	char            NonVolatileBindingCache;

	/* IPsec options */
	char            KeyMngMobCapability;
	char            UseMnHaIPsec;
	struct list_head ipsec_policies;

	/* MN options */
	unsigned int    MnMaxHaBindingLife;
	unsigned int    MnMaxCnBindingLife;
	unsigned int    MnRouterProbes;
	struct timespec MnRouterProbeTimeout_ts;
	struct timespec InitialBindackTimeoutFirstReg_ts;
	struct timespec InitialBindackTimeoutReReg_ts;
	struct list_head home_addrs;
	char *          MoveModulePath;
	uint16_t        CnBuAck;
	char            DoRouteOptimizationMN;
	char            MnUseAllInterfaces;
	char            MnDiscardHaParamProb;
	char            SendMobPfxSols;
	char            OptimisticHandoff;

	/* HA options */
	char            SendMobPfxAdvs;
	char            SendUnsolMobPfxAdvs;
	unsigned int    MaxMobPfxAdvInterval;
	unsigned int    MinMobPfxAdvInterval;
	unsigned int    HaMaxBindingLife;

	/* CN options */
	char            DoRouteOptimizationCN;

	/* PMIP global options */
	unsigned int    RFC5213TimestampBasedApproachInUse;
	unsigned int    RFC5213MobileNodeGeneratedTimestampInUse;
	struct in6_addr RFC5213FixedMAGLinkLocalAddressOnAllAccessLinks;
	struct in6_addr RFC5213FixedMAGLinkLayerAddressOnAllAccessLinks;

	/* PMIP LMA options */
	struct timespec RFC5213MinDelayBeforeBCEDelete;
	struct timespec RFC5213MaxDelayBeforeNewBCEAssign;
	struct timespec RFC5213TimestampValidityWindow;

	/* PMIP MAG options */
	unsigned int    RFC5213EnableMAGLocalRouting;
	struct in6_addr AllLmaMulticastAddress;     // All-LMA Multicast Address (Eurecom' Extension for SPMIPv6).
	struct in6_addr LmaAddress;                 // address of LMA, PMIP network side.
	struct in_addr LmaV4Address;                 // address of LMA, PMIP network side.
	struct in_addr PrimaryV4Dns;                   // primary v4 dns server.
	struct in_addr SecondaryV4Dns;                   // Secondary v4 dns server
	struct in6_addr PrimaryV6Dns;                 // primary v6 dns server
	struct in6_addr SecondaryV6Dns;                 // Secondary v6 dns server
	struct in_addr PMIPV4SecRouterDMNPPrefix;
	int PMIPV4SecRouterdmnpPrefixLen;
	char*           LmaPmipNetworkDevice;       // PMIP LMA device, PMIP network side.
	struct in6_addr LmaCoreNetworkAddress;      // address of LMA, core network side.
	char*           LmaCoreNetworkDevice;       // PMIP LMA device, core network side.
	struct in6_addr MagAddressIngress;          // ingress address of MAG.
	struct in6_addr MagAddressEgress;           // egress address of MAG.
	struct in_addr  Magv4AddressIngress;          // ingress v4 address of MAG.
	struct in_addr  Magv4AddressEgress;           // egres v4s address of MAG.
	struct in6_addr Mag1AddressIngress;         // ingress address of MAG1.
	struct in6_addr Mag1AddressEgress;          // egress address of MAG1.
	struct in6_addr Mag2AddressIngress;         // ingress address of MAG1.
	struct in6_addr Mag2AddressEgress;          // egress address of MAG1.
	struct in6_addr Mag3AddressIngress;         // ingress address of MAG1.
	struct in6_addr Mag3AddressEgress;          // egress address of MAG1.
	char*           MagDeviceIngress;           // ingress device.
	char*           MagDeviceEgress;            // egress device.
	char*           ServiceSelectionIdentifier; //Service selection identifier
	int             is_valid_service_id;        //check for valid service identifier string
	char*           MnIdString;                 // Mobile node identifier string
	uint16_t        PmipMaxLogFileSize;         //Max pmip log file size
	struct in6_addr OurAddress;
	struct in6_addr HomeNetworkPrefix;          // home network address common for domain!
	struct timespec PBULifeTime;                // Life time of Proxy Binding Update.
	struct timespec PBALifeTime;                // Life time MR side.
	struct timespec RetransmissionTimeOut;      // Time-out before retransmission of a message.
	int             MaxMessageRetransmissions;  //indicates the maximum number of message retransmissions
	char            TunnelingEnabled;
	char            DynamicTunnelingEnabled;
	char*           RadiusClientConfigFile;
	char*           RadiusPassword;
	char*           PcapSyslogAssociationGrepString;
	char*           PcapSyslogDeAssociationGrepString;
	struct list_head pbu_list;                //list to store pbu sequence number
};

struct net_iface {
	struct list_head list;
	char name[IF_NAMESIZE];
	int ifindex;
	int is_rtr;
	int mip6_if_entity;
	int mn_if_preference;
};

extern struct mip6_config conf;

#define MIP6_ENTITY_NO -1
#define MIP6_ENTITY_CN 0
#define MIP6_ENTITY_MN 1
#define MIP6_ENTITY_HA 2
#define MIP6_ENTITY_MAG 3
#define MIP6_ENTITY_LMA 4

int pmip_system_call
(
 const char   *command,
 unsigned int  cmdlen,
 int print_command
);



static inline int is_cn(void)
{
	return conf.mip6_entity == MIP6_ENTITY_CN;
}

static inline int is_mn(void)
{
	return conf.mip6_entity == MIP6_ENTITY_MN;
}

static inline int is_ha(void)
{
	return conf.mip6_entity == MIP6_ENTITY_HA;
}

static inline int is_mag(void)
{
	return conf.mip6_entity == MIP6_ENTITY_MAG;
}

static inline int is_lma(void)
{
	return conf.mip6_entity == MIP6_ENTITY_LMA;
}

static inline int is_if_entity_set(struct net_iface *i)
{
	return i->mip6_if_entity != MIP6_ENTITY_NO;

}

static inline int is_if_cn(struct net_iface *i)
{
	return (is_cn() &&
			(!is_if_entity_set(i) || i->mip6_if_entity == MIP6_ENTITY_CN));

}

static inline int is_if_mn(struct net_iface *i)
{
	return (is_mn() &&
			(!is_if_entity_set(i) || i->mip6_if_entity == MIP6_ENTITY_MN));
}

static inline int is_if_ha(struct net_iface *i)
{
	return (is_ha() &&
			(!is_if_entity_set(i) || i->mip6_if_entity == MIP6_ENTITY_HA));
}

static inline int is_if_lma(struct net_iface *i)
{
	return (is_lma() &&
			(!is_if_entity_set(i) || i->mip6_if_entity == MIP6_ENTITY_LMA));
}

static inline int is_if_mag(struct net_iface *i)
{
	return (is_mag() &&
			(!is_if_entity_set(i) || i->mip6_if_entity == MIP6_ENTITY_MAG));
}

int conf_parse(struct mip6_config *c, int argc, char **argv);

void conf_show(struct mip6_config *c);

int yyparse(void);

int yylex(void);

#endif
