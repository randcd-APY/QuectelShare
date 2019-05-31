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
/*!
	@file
	IPACM_Defs.h

	@brief
	This file implements the common definitions amon all ifaces.

	@Author
	Skylar Chang

*/
#ifndef IPA_CM_DEFS_H
#define IPA_CM_DEFS_H

#include <unistd.h>
#include <fcntl.h>
#include <linux/msm_ipa.h>
#include "IPACM_Log.h"

#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#define strlcat g_strlcat
#endif

extern "C"
{
#include <libnetfilter_conntrack/libnetfilter_conntrack.h>
#include <libnetfilter_conntrack/libnetfilter_conntrack_tcp.h>
}

#define IF_NAME_LEN 16
#define IPA_MAX_FILE_LEN  64
#define IPA_IFACE_NAME_LEN 16
#define IPA_ALG_PROTOCOL_NAME_LEN  10

#define IPA_WLAN_PARTIAL_HDR_OFFSET  0 // dst mac first then src mac
#define IPA_ODU_PARTIAL_HDR_OFFSET  8 // dst mac first then src mac
#define IPA_WLAN_PARTIAL_HDR_NAME_v4  "IEEE802_3_v4"
#define IPA_WLAN_PARTIAL_HDR_NAME_v6  "IEEE802_3_v6"
#define IPA_DUMMY_ETH_HDR_NAME_v6     "ETH_dummy_v6"
#define IPA_WAN_PARTIAL_HDR_NAME_v4  "IEEE802_3_STA_v4"
#define IPA_WAN_PARTIAL_HDR_NAME_v6  "IEEE802_3_STA_v6"
#define IPA_ETH_HDR_NAME_v4  "IPACM_ETH_v4"
#define IPA_ETH_HDR_NAME_v6  "IPACM_ETH_v6"
#define IPA_ODU_HDR_NAME_v4  "IPACM_ODU_v4"
#define IPA_ODU_HDR_NAME_v6  "IPACM_ODU_v6"


#define IPA_MAX_IFACE_ENTRIES 20
#ifdef FEATURE_VLAN_MPDN
#define IPA_MAX_PRIVATE_SUBNET_ENTRIES 4
#else
#define IPA_MAX_PRIVATE_SUBNET_ENTRIES 3
#endif
#define IPA_MAX_ALG_ENTRIES 20
#define IPA_MAX_RM_ENTRY 9

#define IPV4_ADDR_LINKLOCAL 0xA9FE0000
#define IPV4_ADDR_LINKLOCAL_MASK 0xFFFF0000

#define V4_DEFAULT_ROUTE_TABLE_NAME  "ipa_dflt_rt"
#define V4_LAN_ROUTE_TABLE_NAME  "COMRTBLLANv4"
#define V4_WAN_ROUTE_TABLE_NAME  "WANRTBLv4"
#define WAN_DL_ROUTE_TABLE_NAME "ipa_dflt_wan_rt"
#define V6_COMMON_ROUTE_TABLE_NAME  "COMRTBLv6"
#define V6_WAN_ROUTE_TABLE_NAME  "WANRTBLv6"
#define V4_ODU_ROUTE_TABLE_NAME  "ODURTBLv4"
#define V6_ODU_ROUTE_TABLE_NAME  "ODURTBLv6"

#define WWAN_QMI_IOCTL_DEVICE_NAME "/dev/wwan_ioctl"
#define IPA_DEVICE_NAME "/dev/ipa"
#define MAX_NUM_PROP 2

#ifndef FEATURE_IPA_V3
#define IPA_MAX_FLT_RULE 50
#else
#define IPA_MAX_FLT_RULE 100
#endif

#define TCP_FIN_SHIFT 16
#define TCP_SYN_SHIFT 17
#define TCP_RST_SHIFT 18
#ifdef FEATURE_VLAN_MPDN
/* support default PDN+3 VLAN PDNs */
#define IPA_MAX_IPV6_PREFIX_FLT_RULE  4
#else
/* support only default PDN */
#define IPA_MAX_IPV6_PREFIX_FLT_RULE  1
#endif
#define MAX_CMD_SIZE 100

/* WAN IP address in IP Passthrough mode. */
#define IPACM_IPPASSTHROUGH_WAN_IP "169.254.5.1"

/*---------------------------------------------------------------------------
										Return values indicating error status
---------------------------------------------------------------------------*/

#define IPACM_SUCCESS                0         /* Successful operation   */
#define IPACM_FAILURE               -1         /* Unsuccessful operation */

#define IPACM_IP_NULL (ipa_ip_type)0xFF
#define IPACM_INVALID_INDEX (ipa_ip_type)0xFF

#define IPA_MAX_NUM_WIFI_CLIENTS  32
#define IPA_MAX_NUM_WAN_CLIENTS  10
#define IPA_MAX_NUM_ETH_CLIENTS  15
#define IPA_MAX_NUM_AMPDU_RULE  15
#define IPA_MAC_ADDR_SIZE  6
#define IPA_IPV6_ADDR_SIZE_IN_WORDS 4
#define IPA_MAX_NUM_BRIDGES 8
#define IPA_MAX_NUM_SW_PDNS 15
#define IPA_MAX_NUM_HW_PDNS 4
/*===========================================================================
										 GLOBAL DEFINITIONS AND DECLARATIONS
===========================================================================*/
typedef enum
{
	IPA_CFG_CHANGE_EVENT,                 /* NULL */
	IPA_PRIVATE_SUBNET_CHANGE_EVENT,          /* ipacm_event_data_fid */
	IPA_FIREWALL_CHANGE_EVENT,                /* NULL */
	IPA_LINK_UP_EVENT,                        /* ipacm_event_data_fid */
	IPA_LINK_DOWN_EVENT,                      /* ipacm_event_data_fid */
	IPA_USB_LINK_UP_EVENT,                    /* ipacm_event_data_fid */
	IPA_BRIDGE_LINK_UP_EVENT,                 /* ipacm_event_data_all */
	IPA_WAN_EMBMS_LINK_UP_EVENT,              /* ipacm_event_data_mac */
	IPA_ADDR_ADD_EVENT,                       /* ipacm_event_data_addr */
	IPA_ADDR_DEL_EVENT,                       /* no use */
	IPA_ROUTE_ADD_EVENT,                      /* ipacm_event_data_addr */
	IPA_ROUTE_DEL_EVENT,                      /* ipacm_event_data_addr */
	IPA_WAN_UPSTREAM_ROUTE_ADD_EVENT,         /* ipacm_event_data_fid */
	IPA_WAN_UPSTREAM_ROUTE_DEL_EVENT,         /* ipacm_event_data_fid */
	IPA_WLAN_AP_LINK_UP_EVENT,                /* ipacm_event_data_mac */
	IPA_WLAN_STA_LINK_UP_EVENT,               /* ipacm_event_data_mac */
	IPA_WLAN_LINK_DOWN_EVENT,                 /* ipacm_event_data_mac */
	IPA_WLAN_CLIENT_ADD_EVENT,                /* ipacm_event_data_mac */
	IPA_WLAN_CLIENT_ADD_EVENT_EX,             /* ipacm_event_data_wlan_ex */
	IPA_WLAN_CLIENT_DEL_EVENT,                /* ipacm_event_data_mac */
	IPA_WLAN_CLIENT_POWER_SAVE_EVENT,         /* ipacm_event_data_mac */
	IPA_WLAN_CLIENT_RECOVER_EVENT,            /* ipacm_event_data_mac */
	IPA_NEW_NEIGH_EVENT,                      /* ipacm_event_data_all */
	IPA_DEL_NEIGH_EVENT,                      /* ipacm_event_data_all */
	IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT,       /* ipacm_event_data_all */
	IPA_NEIGH_CLIENT_IP_ADDR_DEL_EVENT,       /* ipacm_event_data_all */
	IPA_SW_ROUTING_ENABLE,                    /* NULL */
	IPA_SW_ROUTING_DISABLE,                   /* NULL */
	IPA_PROCESS_CT_MESSAGE,                   /* ipacm_ct_evt_data */
	IPA_PROCESS_CT_MESSAGE_V6,                /* ipacm_ct_evt_data */
	IPA_LAN_TO_LAN_NEW_CONNECTION,            /* ipacm_event_connection */
	IPA_LAN_TO_LAN_DEL_CONNECTION,            /* ipacm_event_connection */
	IPA_WLAN_SWITCH_TO_SCC,                   /* No Data */
	IPA_WLAN_SWITCH_TO_MCC,                   /* No Data */
	IPA_CRADLE_WAN_MODE_SWITCH,               /* ipacm_event_cradle_wan_mode */
	IPA_WAN_XLAT_CONNECT_EVENT,               /* ipacm_event_data_fid */
	IPA_TETHERING_STATS_UPDATE_EVENT,         /* ipacm_event_data_fid */
	IPA_NETWORK_STATS_UPDATE_EVENT,           /* ipacm_event_data_fid */
#ifdef FEATURE_IPACM_PER_CLIENT_STATS
	IPA_LAN_CLIENT_CONNECT_EVENT,             /* ipacm_event_data_mac */
	IPA_LAN_CLIENT_DISCONNECT_EVENT,          /* ipacm_event_data_mac */
	IPA_LAN_CLIENT_UPDATE_EVENT,              /* ipacm_event_data_mac */
#endif
	IPA_EXTERNAL_EVENT_MAX,

	IPA_HANDLE_WAN_UP,                        /* ipacm_event_iface_up  */
	IPA_HANDLE_WAN_DOWN,                      /* ipacm_event_iface_up  */
	IPA_HANDLE_WAN_UP_V6,                     /* NULL */
	IPA_HANDLE_WAN_DOWN_V6,                   /* NULL */
	IPA_HANDLE_WAN_UP_TETHER,                 /* ipacm_event_iface_up_tehter */
	IPA_HANDLE_WAN_DOWN_TETHER,               /* ipacm_event_iface_up_tehter */
	IPA_HANDLE_WAN_UP_V6_TETHER,              /* ipacm_event_iface_up_tehter */
	IPA_HANDLE_WAN_DOWN_V6_TETHER,            /* ipacm_event_iface_up_tehter */
	IPA_HANDLE_LAN_WLAN_UP,                   /* ipacm_event_iface_up */
	IPA_HANDLE_LAN_WLAN_UP_V6,                /* ipacm_event_iface_up */
	IPA_ETH_BRIDGE_IFACE_UP,                  /* ipacm_event_eth_bridge*/
	IPA_ETH_BRIDGE_IFACE_DOWN,                /* ipacm_event_eth_bridge*/
	IPA_ETH_BRIDGE_CLIENT_ADD,                /* ipacm_event_eth_bridge */
	IPA_ETH_BRIDGE_CLIENT_DEL,                /* ipacm_event_eth_bridge*/
	IPA_ETH_BRIDGE_WLAN_SCC_MCC_SWITCH,       /* ipacm_event_eth_bridge*/
	IPA_LAN_DELETE_SELF,                      /* ipacm_event_data_fid */
#ifdef FEATURE_L2TP_E2E
	IPA_ADD_L2TP_CLIENT,                      /* ipacm_event_data_all */
	IPA_DEL_L2TP_CLIENT,                      /* ipacm_event_data_all */
#endif
#ifdef FEATURE_VLAN_MPDN
	IPA_PREFIX_CHANGE_EVENT,                  /* ipacm_event_data_fid */
	IPA_ROUTE_ADD_VLAN_PDN_EVENT,             /* ipacm_event_route_vlan */
	IPA_HANDLE_WAN_VLAN_PDN_UP,                   /* ipacm_event_vlan_pdn */
	IPA_HANDLE_WAN_VLAN_PDN_DOWN,                 /* ipacm_event_vlan_pdn */
#endif
	IPACM_EVENT_MAX
} ipa_cm_event_id;

typedef struct
{
	uint8_t num_rule;
	uint32_t rule_hdl[MAX_NUM_PROP];
} lan_to_lan_rt_rule_hdl;

typedef enum
{
	LAN_IF = 0,
	WLAN_IF,
	WAN_IF,
	VIRTUAL_IF,
	ETH_IF,
	EMBMS_IF,
	ODU_IF,
	UNKNOWN_IF
} ipacm_iface_type;

typedef enum
{
	ROUTER = 0,
	BRIDGE
} ipacm_cradle_iface_mode;

typedef enum
{
	FULL,
	INTERNET
} ipacm_wlan_access_mode;

typedef struct
{
	char bridge_name[IF_NAME_LEN];
	uint32_t bridge_netmask;
	uint32_t bridge_ipv4_addr;
	uint8_t bridge_mac[IPA_MAC_ADDR_SIZE];
	uint32_t associate_VID;
}ipacm_bridge;

typedef struct
{
	struct nf_conntrack *ct;
	enum nf_conntrack_msg_type type;
}ipacm_ct_evt_data;

typedef struct
{
	char iface_name[IPA_IFACE_NAME_LEN];
	ipacm_iface_type if_cat;
	ipacm_cradle_iface_mode if_mode;
	ipacm_wlan_access_mode wlan_mode;
	int netlink_interface_index;
} ipa_ifi_dev_name_t;

typedef struct
{
	uint32_t subnet_addr;
	uint32_t subnet_mask;
} ipa_private_subnet;


typedef struct _ipacm_event_data_all
{
	enum ipa_ip_type iptype;
	int if_index;
	uint32_t  ipv4_addr;
	uint32_t  ipv6_addr[4];
	uint8_t mac_addr[IPA_MAC_ADDR_SIZE];
	char iface_name[IPA_IFACE_NAME_LEN];
} ipacm_event_data_all;

typedef struct _ipacm_event_new_neigh_vlan
{
	/* must be first since might be treated as data_all event */
	ipacm_event_data_all data_all;
	ipacm_bridge *bridge;
} ipacm_event_new_neigh_vlan;

class IPACM_Lan;

typedef struct
{
	ipacm_cradle_iface_mode cradle_wan_mode;
} ipacm_event_cradle_wan_mode;

typedef struct
{
	IPACM_Lan *p_iface;
	ipa_ip_type iptype;
	uint8_t mac_addr[6];
	char iface_name[IPA_IFACE_NAME_LEN];
} ipacm_event_eth_bridge;

typedef struct
{
	enum ipa_ip_type iptype;
	uint32_t src_ipv4_addr;
	uint32_t dst_ipv4_addr;
	uint32_t src_ipv6_addr[4];
	uint32_t dst_ipv6_addr[4];
} ipacm_event_connection;

typedef struct _ipacm_event_data_fid
{
	int if_index;
} ipacm_event_data_fid;

typedef struct
{
	ipacm_iface_type if_cat;
} ipacm_event_data_if_cat;

typedef struct _ipacm_event_data_iptype
{
	int if_index;
	int if_index_tether;
	enum ipa_ip_type iptype;
} ipacm_event_data_iptype;


typedef struct _ipacm_event_data_addr
{
	enum ipa_ip_type iptype;
	char iface_name[IPA_IFACE_NAME_LEN];
	int if_index;
	uint32_t  ipv4_addr_gw;
	uint32_t  ipv4_addr;
	uint32_t  ipv4_addr_mask;
	uint32_t  ipv6_addr[4];
	uint32_t  ipv6_addr_mask[4];
	uint32_t  ipv6_addr_gw[4];
} ipacm_event_data_addr;

typedef struct _ipacm_event_data_mac
{
	int if_index;
	int ipa_if_cate;
	uint8_t mac_addr[IPA_MAC_ADDR_SIZE];
} ipacm_event_data_mac;

typedef struct
{
	int if_index;
	uint8_t num_of_attribs;
	struct ipa_wlan_hdr_attrib_val attribs[0];
} ipacm_event_data_wlan_ex;

typedef struct _ipacm_event_iface_up
{
	char ifname[IPA_IFACE_NAME_LEN];
	uint32_t ipv4_addr;
	uint32_t addr_mask;
	uint32_t ipv6_prefix[2];
	uint32_t ipv6_addr[4];
	bool is_sta;
	uint8_t xlat_mux_id;
	uint8_t mux_id;
}ipacm_event_iface_up;

typedef struct _ipacm_event_iface_up_tether
{
	uint32_t if_index_tether;
	uint32_t ipv6_prefix[2];
	bool is_sta;
}ipacm_event_iface_up_tehter;

typedef struct
{
	enum ipa_ip_type iptype;
	uint16_t VlanID;
	uint32_t wan_ipv4_addr;
	uint32_t wan_ipv6_prefix[2];
}ipacm_event_route_vlan;

typedef struct
{
	enum ipa_ip_type iptype;
	uint8_t VlanID;
	int mux_id;
	int ipv4_addr;
}ipacm_event_vlan_pdn;

typedef enum
{
	Q6_WAN = 0,
	WLAN_WAN,
	ECM_WAN
} ipacm_wan_iface_type;

typedef struct _ipacm_ifacemgr_data
{
	int if_index;
	ipacm_wan_iface_type if_type;
	uint8_t mac_addr[IPA_MAC_ADDR_SIZE];
}ipacm_ifacemgr_data;

struct vlan_iface_info
{
	char vlan_iface_name[IPA_RESOURCE_NAME_MAX];
	uint8_t vlan_id;
	uint32_t vlan_iface_ipv6_addr[4];
	uint8_t vlan_client_mac[6];
	uint32_t vlan_client_ipv6_addr[4];
};

struct l2tp_vlan_mapping_info
{
	/* the following are l2tp iface info (name, session id) */
	char l2tp_iface_name[IPA_RESOURCE_NAME_MAX];
	uint8_t l2tp_session_id;
	/* the following are mdm vlan iface info (name, vlan id, ipv6 addr) */
	char vlan_iface_name[IPA_RESOURCE_NAME_MAX];
	uint8_t vlan_id;
	uint32_t vlan_iface_ipv6_addr[4];
	/* the following are MIB3 vlan client info (mac, ipv6 addr) */
	uint8_t vlan_client_mac[6];
	uint32_t vlan_client_ipv6_addr[4];
	/* the following is MIB3 l2tp client info (mac) */
	uint8_t l2tp_client_mac[6];
};

struct bridge_vlan_mapping_info
{
	char bridge_iface_name[IPA_RESOURCE_NAME_MAX];
	uint32_t bridge_associated_VID;
	uint32_t bridge_ipv4;
	uint32_t subnet_mask;
};

struct l2tp_client_info
{
	char client_iface_name[IPA_IFACE_NAME_LEN];
};

#endif /* IPA_CM_DEFS_H */
