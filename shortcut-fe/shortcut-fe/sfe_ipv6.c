/*
 * sfe_ipv6.c
 *	Shortcut forwarding engine - IPv6 support.
 *
 * Copyright (c) 2015 The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <linux/module.h>
#include <linux/sysfs.h>
#include <linux/skbuff.h>
#include <linux/icmp.h>
#include <net/tcp.h>
#include <linux/etherdevice.h>
#include <linux/netdevice.h>
#include <linux/netlink.h>
#include <linux/hashtable.h>
#include <linux/proc_fs.h>

#include "sfe.h"
#include "sfe_cm.h"

#define PKT_THRESHOLD 10
#define TIMEOUT 100
#define PACKETS_STATS_ENABLED 0

int var_timeout = TIMEOUT;
int var_thresh = PKT_THRESHOLD;
int threshold_count;
int timeout_count;
bool iface;
int packet_stats_enabled = PACKETS_STATS_ENABLED;


#define XDBG_ADD_PROC_ENTRY(it, name, xdata)             \
{                                                 \
	.procname       = (name),                 \
	.data           = (xdata),                \
	.maxlen         = sizeof(int),            \
	.mode           = 0666,                   \
	.proc_handler   = &proc_dointvec,         \
}

enum {
	XDBG_TIMER_STEP_DBG,
	XDBG_THRESHOLD_STEP_DBG,
	XDBG_THRESHOLD_NUM_DBG,
	XDBG_TIMER_NUM_DBG,
	XDBG_MAX
};

static struct ctl_table sfe_sysctl_debug[] = 
{
	XDBG_ADD_PROC_ENTRY(XDBG_TIMER_STEP_DBG, "v6_timeout_value", &var_timeout),
	XDBG_ADD_PROC_ENTRY(XDBG_THRESHOLD_STEP_DBG, "v6_threshold", &var_thresh),
	XDBG_ADD_PROC_ENTRY(XDBG_THRESHOLD_STEP_DBG, "v6_threshold_count", &threshold_count),
	XDBG_ADD_PROC_ENTRY(XDBG_THRESHOLD_STEP_DBG, "v6_timeout_count", &timeout_count),
	XDBG_ADD_PROC_ENTRY(XDBG_THRESHOLD_STEP_DBG, "packet_stats_on", &packet_stats_enabled),
	{},
};

static int sfe_v6_enable_ipc_low;

typedef struct sfe_proc_sys_db
{
	struct ctl_table debug_root[2];
	struct ctl_table_header * debug_ctl_header;

	struct ctl_path sfe_debug_ctl_path[2];
}sfe_proc_sys_db_t;

/*
 * By default Linux IP header and transport layer header structures are
 * unpacked, assuming that such headers should be 32-bit aligned.
 * Unfortunately some wireless adaptors can't cope with this requirement and
 * some CPUs can't handle misaligned accesses.  For those platforms we
 * define SFE_IPV6_UNALIGNED_IP_HEADER and mark the structures as packed.
 * When we do this the compiler will generate slightly worse code than for the
 * aligned case (on most platforms) but will be much quicker than fixing
 * things up in an unaligned trap handler.
 */
#define SFE_IPV6_UNALIGNED_IP_HEADER 1
#if SFE_IPV6_UNALIGNED_IP_HEADER
#define SFE_IPV6_UNALIGNED_STRUCT __attribute__((packed))
#else
#define SFE_IPV6_UNALIGNED_STRUCT
#endif

#define CHAR_DEV_MSG_SIZE 768

/*
 * An Ethernet header, but with an optional "packed" attribute to
 * help with performance on some platforms (see the definition of
 * SFE_IPV6_UNALIGNED_STRUCT)
 */
struct sfe_ipv6_eth_hdr {
	__be16 h_dest[ETH_ALEN / 2];
	__be16 h_source[ETH_ALEN / 2];
	__be16 h_proto;
} SFE_IPV6_UNALIGNED_STRUCT;

/*
 * An IPv6 header, but with an optional "packed" attribute to
 * help with performance on some platforms (see the definition of
 * SFE_IPV6_UNALIGNED_STRUCT)
 */
struct sfe_ipv6_ip_hdr {
#if defined(__LITTLE_ENDIAN_BITFIELD)
	__u8 priority:4,
	     version:4;
#elif defined(__BIG_ENDIAN_BITFIELD)
	__u8 version:4,
	     priority:4;
#else
#error	"Please fix <asm/byteorder.h>"
#endif
	__u8 flow_lbl[3];
	__be16 payload_len;
	__u8 nexthdr;
	__u8 hop_limit;
	struct sfe_ipv6_addr saddr;
	struct sfe_ipv6_addr daddr;

	/*
	 * The extension header start here.
	 */
} SFE_IPV6_UNALIGNED_STRUCT;

#define SFE_IPV6_EXT_HDR_HOP 0
#define SFE_IPV6_EXT_HDR_ROUTING 43
#define SFE_IPV6_EXT_HDR_FRAG 44
#define SFE_IPV6_EXT_HDR_ESP 50
#define SFE_IPV6_EXT_HDR_AH 51
#define SFE_IPV6_EXT_HDR_NONE 59
#define SFE_IPV6_EXT_HDR_DST 60
#define SFE_IPV6_EXT_HDR_MH 135

/*
 * fragmentation header
 */

struct sfe_ipv6_frag_hdr {
	__u8	nexthdr;
	__u8	reserved;
	__be16	frag_off;
	__be32	identification;
};

#define	SFE_IPV6_FRAG_OFFSET	0xfff8

/*
 * generic IPv6 extension header
 */
struct sfe_ipv6_ext_hdr {
	__u8 next_hdr;
	__u8 hdr_len;
	__u8 padding[6];
} SFE_IPV6_UNALIGNED_STRUCT;

/*
 * A UDP header, but with an optional "packed" attribute to
 * help with performance on some platforms (see the definition of
 * SFE_IPV6_UNALIGNED_STRUCT)
 */
struct sfe_ipv6_udp_hdr {
	__be16 source;
	__be16 dest;
	__be16 len;
	__sum16 check;
} SFE_IPV6_UNALIGNED_STRUCT;

/*
 * A TCP header, but with an optional "packed" attribute to
 * help with performance on some platforms (see the definition of
 * SFE_IPV6_UNALIGNED_STRUCT)
 */
struct sfe_ipv6_tcp_hdr {
	__be16 source;
	__be16 dest;
	__be32 seq;
	__be32 ack_seq;
#if defined(__LITTLE_ENDIAN_BITFIELD)
	__u16 res1:4,
	      doff:4,
	      fin:1,
	      syn:1,
	      rst:1,
	      psh:1,
	      ack:1,
	      urg:1,
	      ece:1,
	      cwr:1;
#elif defined(__BIG_ENDIAN_BITFIELD)
	__u16 doff:4,
	      res1:4,
	      cwr:1,
	      ece:1,
	      urg:1,
	      ack:1,
	      psh:1,
	      rst:1,
	      syn:1,
	      fin:1;
#else
#error	"Adjust your <asm/byteorder.h> defines"
#endif
	__be16 window;
	__sum16	check;
	__be16 urg_ptr;
} SFE_IPV6_UNALIGNED_STRUCT;

/*
 * Specifies the lower bound on ACK numbers carried in the TCP header
 */
#define SFE_IPV6_TCP_MAX_ACK_WINDOW 65520

/*
 * IPv6 TCP connection match additional data.
 */
struct sfe_ipv6_tcp_connection_match {
	uint8_t win_scale;		/* Window scale */
	uint32_t max_win;		/* Maximum window size seen */
	uint32_t end;			/* Sequence number of the next byte to send (seq + segment length) */
	uint32_t max_end;		/* Sequence number of the last byte to ack */
};

/*
 * Bit flags for IPv6 connection matching entry.
 */
#define SFE_IPV6_CONNECTION_MATCH_FLAG_XLATE_SRC 0x1
/* Perform source translation */
#define SFE_IPV6_CONNECTION_MATCH_FLAG_XLATE_DEST 0x2
/* Perform destination translation */
#define SFE_IPV6_CONNECTION_MATCH_FLAG_NO_SEQ_CHECK 0x4
/* Ignore TCP sequence numbers */
#define SFE_IPV6_CONNECTION_MATCH_FLAG_WRITE_FAST_ETH_HDR 0x8
/* Fast Ethernet header write */
#define SFE_IPV6_CONNECTION_MATCH_FLAG_WRITE_L2_HDR 0x10
/* Fast Ethernet header write */

/*
 * IPv6 connection matching structure.
 */
struct sfe_ipv6_connection_match {
	/*
	 * References to other objects.
	 */
	struct sfe_ipv6_connection_match *next;
	/* Next connection match entry in a list */
	struct sfe_ipv6_connection_match *prev;
	/* Previous connection match entry in a list */
	struct sfe_ipv6_connection *connection;
	/* Pointer to our connection */
	struct sfe_ipv6_connection_match *counter_match;
	/* Pointer to the connection match in the "counter" direction to this one */
	struct sfe_ipv6_connection_match *active_next;
	/* Pointer to the next connection in the active list */
	struct sfe_ipv6_connection_match *active_prev;
	/* Pointer to the previous connection in the active list */
	bool active;			/* Flag to indicate if we're on the active list */

	/*
	 * Characteristics that identify flows that match this rule.
	 */
	struct net_device *match_dev;	/* Network device */
	uint8_t match_protocol;		/* Protocol */
	struct sfe_ipv6_addr match_src_ip[1];	/* Source IP address */
	struct sfe_ipv6_addr match_dest_ip[1];	/* Destination IP address */
	__be16 match_src_port;		/* Source port/connection ident */
	__be16 match_dest_port;		/* Destination port/connection ident */

	/*
	 * Control the operations of the match.
	 */
	uint32_t flags;			/* Bit flags */
#ifdef CONFIG_NF_FLOW_COOKIE
	uint32_t flow_cookie;		/* used flow cookie, for debug */
#endif
#ifdef CONFIG_XFRM
	uint32_t flow_accel;            /* The flow accelerated or not */
#endif

	/*
	 * Connection state that we track once we match.
	 */
	union {				/* Protocol-specific state */
		struct sfe_ipv6_tcp_connection_match tcp;
	} protocol_state;
	uint32_t rx_packet_count;	/* Number of packets RX'd */
	uint32_t rx_byte_count;		/* Number of bytes RX'd */
	uint64_t rx_pack_stat_byte_count;		/* Number of bytes RX'd used  with packet stats */

	/*
	 * Packet translation information.
	 */
	struct sfe_ipv6_addr xlate_src_ip[1];	/* Address after source translation */
	__be16 xlate_src_port;	/* Port/connection ident after source translation */
	uint16_t xlate_src_csum_adjustment;
	/* Transport layer checksum adjustment after source translation */
	struct sfe_ipv6_addr xlate_dest_ip[1];	/* Address after destination translation */
	__be16 xlate_dest_port;	/* Port/connection ident after destination translation */
	uint16_t xlate_dest_csum_adjustment;
	/* Transport layer checksum adjustment after destination translation */

	/*
	 * Packet transmit information.
	 */
	struct net_device *xmit_dev;	/* Network device on which to transmit */
	unsigned short int xmit_dev_mtu;
	/* Interface MTU */
	uint16_t xmit_dest_mac[ETH_ALEN / 2];
	/* Destination MAC address to use when forwarding */
	uint16_t xmit_src_mac[ETH_ALEN / 2];
	/* Source MAC address to use when forwarding */

	/*
	 * Summary stats.
	 */
	uint64_t rx_packet_count64;	/* Number of packets RX'd */
	uint64_t rx_byte_count64;	/* Number of bytes RX'd */
	bool addEthMAC;                 /* Add ethernet header if set */
	sfe_wlan_index_type index;      /* WLAN Interface index. */
	bool expand_head;               /* Extra headroom needed */
	bool pad_removal_require;
};

/*
 * Per-connection data structure.
 */
struct sfe_ipv6_connection {
	struct sfe_ipv6_connection *next;
	/* Pointer to the next entry in a hash chain */
	struct sfe_ipv6_connection *prev;
	/* Pointer to the previous entry in a hash chain */
	int protocol;			/* IP protocol number */
	struct sfe_ipv6_addr src_ip[1];		/* Source IP address */
	struct sfe_ipv6_addr src_ip_xlate[1];	/* NAT-translated source IP address */
	struct sfe_ipv6_addr dest_ip[1];	/* Destination IP address */
	struct sfe_ipv6_addr dest_ip_xlate[1];	/* NAT-translated destination IP address */
	__be16 src_port;		/* Source port */
	__be16 src_port_xlate;		/* NAT-translated source port */
	__be16 dest_port;		/* Destination port */
	__be16 dest_port_xlate;		/* NAT-translated destination port */
	struct sfe_ipv6_connection_match *original_match;
	/* Original direction matching structure */
	struct net_device *original_dev;
	/* Original direction source device */
	struct sfe_ipv6_connection_match *reply_match;
	/* Reply direction matching structure */
	struct net_device *reply_dev;	/* Reply direction source device */
	uint64_t last_sync_jiffies;	/* Jiffies count for the last sync */
	struct sfe_ipv6_connection *all_connections_next;
	/* Pointer to the next entry in the list of all connections */
	struct sfe_ipv6_connection *all_connections_prev;
	/* Pointer to the previous entry in the list of all connections */
	uint32_t mark;			/* mark for outgoing packet */
	uint32_t debug_read_seq;	/* sequence number for debug dump */
	bool use_destMac;		/*Add ethernet header if set*/
};

/*
 * IPv6 connections and hash table size information.
 */
#define SFE_IPV6_CONNECTION_HASH_SHIFT 12
#define SFE_IPV6_CONNECTION_HASH_SIZE (1 << SFE_IPV6_CONNECTION_HASH_SHIFT)
#define SFE_IPV6_CONNECTION_HASH_MASK (SFE_IPV6_CONNECTION_HASH_SIZE - 1)

/*
 * IPv6 hash table size information for packet stats.
 */
#define SFE_IPV6_PACKET_STATS_HASH_SHIFT 7
#define SFE_IPV6_PACKET_STATS_HASH_SIZE (1 << SFE_IPV6_PACKET_STATS_HASH_SHIFT)

#ifdef CONFIG_NF_FLOW_COOKIE
#define SFE_FLOW_COOKIE_SIZE 2048
#define SFE_FLOW_COOKIE_MASK 0x7ff

struct sfe_ipv6_flow_cookie_entry {
	struct sfe_ipv6_connection_match *match;
	unsigned long last_clean_time;
};
#endif

enum sfe_ipv6_exception_events {
	SFE_IPV6_EXCEPTION_EVENT_UDP_HEADER_INCOMPLETE,
	SFE_IPV6_EXCEPTION_EVENT_UDP_NO_CONNECTION,
	SFE_IPV6_EXCEPTION_EVENT_UDP_IP_OPTIONS_OR_INITIAL_FRAGMENT,
	SFE_IPV6_EXCEPTION_EVENT_UDP_SMALL_TTL,
	SFE_IPV6_EXCEPTION_EVENT_UDP_NEEDS_FRAGMENTATION,
	SFE_IPV6_EXCEPTION_EVENT_TCP_HEADER_INCOMPLETE,
	SFE_IPV6_EXCEPTION_EVENT_TCP_NO_CONNECTION_SLOW_FLAGS,
	SFE_IPV6_EXCEPTION_EVENT_TCP_NO_CONNECTION_FAST_FLAGS,
	SFE_IPV6_EXCEPTION_EVENT_TCP_IP_OPTIONS_OR_INITIAL_FRAGMENT,
	SFE_IPV6_EXCEPTION_EVENT_TCP_SMALL_TTL,
	SFE_IPV6_EXCEPTION_EVENT_TCP_NEEDS_FRAGMENTATION,
	SFE_IPV6_EXCEPTION_EVENT_TCP_FLAGS,
	SFE_IPV6_EXCEPTION_EVENT_TCP_SEQ_EXCEEDS_RIGHT_EDGE,
	SFE_IPV6_EXCEPTION_EVENT_TCP_SMALL_DATA_OFFS,
	SFE_IPV6_EXCEPTION_EVENT_TCP_BAD_SACK,
	SFE_IPV6_EXCEPTION_EVENT_TCP_BIG_DATA_OFFS,
	SFE_IPV6_EXCEPTION_EVENT_TCP_SEQ_BEFORE_LEFT_EDGE,
	SFE_IPV6_EXCEPTION_EVENT_TCP_ACK_EXCEEDS_RIGHT_EDGE,
	SFE_IPV6_EXCEPTION_EVENT_TCP_ACK_BEFORE_LEFT_EDGE,
	SFE_IPV6_EXCEPTION_EVENT_ICMP_HEADER_INCOMPLETE,
	SFE_IPV6_EXCEPTION_EVENT_ICMP_UNHANDLED_TYPE,
	SFE_IPV6_EXCEPTION_EVENT_ICMP_IPV6_HEADER_INCOMPLETE,
	SFE_IPV6_EXCEPTION_EVENT_ICMP_IPV6_NON_V6,
	SFE_IPV6_EXCEPTION_EVENT_ICMP_IPV6_IP_OPTIONS_INCOMPLETE,
	SFE_IPV6_EXCEPTION_EVENT_ICMP_IPV6_UDP_HEADER_INCOMPLETE,
	SFE_IPV6_EXCEPTION_EVENT_ICMP_IPV6_TCP_HEADER_INCOMPLETE,
	SFE_IPV6_EXCEPTION_EVENT_ICMP_IPV6_UNHANDLED_PROTOCOL,
	SFE_IPV6_EXCEPTION_EVENT_ICMP_NO_CONNECTION,
	SFE_IPV6_EXCEPTION_EVENT_ICMP_FLUSHED_CONNECTION,
	SFE_IPV6_EXCEPTION_EVENT_HEADER_INCOMPLETE,
	SFE_IPV6_EXCEPTION_EVENT_BAD_TOTAL_LENGTH,
	SFE_IPV6_EXCEPTION_EVENT_NON_V6,
	SFE_IPV6_EXCEPTION_EVENT_NON_INITIAL_FRAGMENT,
	SFE_IPV6_EXCEPTION_EVENT_DATAGRAM_INCOMPLETE,
	SFE_IPV6_EXCEPTION_EVENT_IP_OPTIONS_INCOMPLETE,
	SFE_IPV6_EXCEPTION_EVENT_UNHANDLED_PROTOCOL,
	SFE_IPV6_EXCEPTION_EVENT_FLOW_COOKIE_ADD_FAIL,
	SFE_IPV6_EXCEPTION_EVENT_LAST
};

static char *sfe_ipv6_exception_events_string[SFE_IPV6_EXCEPTION_EVENT_LAST] = {
	"UDP_HEADER_INCOMPLETE",
	"UDP_NO_CONNECTION",
	"UDP_IP_OPTIONS_OR_INITIAL_FRAGMENT",
	"UDP_SMALL_TTL",
	"UDP_NEEDS_FRAGMENTATION",
	"TCP_HEADER_INCOMPLETE",
	"TCP_NO_CONNECTION_SLOW_FLAGS",
	"TCP_NO_CONNECTION_FAST_FLAGS",
	"TCP_IP_OPTIONS_OR_INITIAL_FRAGMENT",
	"TCP_SMALL_TTL",
	"TCP_NEEDS_FRAGMENTATION",
	"TCP_FLAGS",
	"TCP_SEQ_EXCEEDS_RIGHT_EDGE",
	"TCP_SMALL_DATA_OFFS",
	"TCP_BAD_SACK",
	"TCP_BIG_DATA_OFFS",
	"TCP_SEQ_BEFORE_LEFT_EDGE",
	"TCP_ACK_EXCEEDS_RIGHT_EDGE",
	"TCP_ACK_BEFORE_LEFT_EDGE",
	"ICMP_HEADER_INCOMPLETE",
	"ICMP_UNHANDLED_TYPE",
	"ICMP_IPV6_HEADER_INCOMPLETE",
	"ICMP_IPV6_NON_V6",
	"ICMP_IPV6_IP_OPTIONS_INCOMPLETE",
	"ICMP_IPV6_UDP_HEADER_INCOMPLETE",
	"ICMP_IPV6_TCP_HEADER_INCOMPLETE",
	"ICMP_IPV6_UNHANDLED_PROTOCOL",
	"ICMP_NO_CONNECTION",
	"ICMP_FLUSHED_CONNECTION",
	"HEADER_INCOMPLETE",
	"BAD_TOTAL_LENGTH",
	"NON_V6",
	"NON_INITIAL_FRAGMENT",
	"DATAGRAM_INCOMPLETE",
	"IP_OPTIONS_INCOMPLETE",
	"UNHANDLED_PROTOCOL",
	"FLOW_COOKIE_ADD_FAIL"
};

/*
 * Per-module structure.
 */
struct sfe_ipv6 {
	spinlock_t lock;		/* Lock for SMP correctness */
	struct sfe_ipv6_connection_match *active_head;
	/* Head of the list of recently active connections */
	struct sfe_ipv6_connection_match *active_tail;
	/* Tail of the list of recently active connections */
	struct sfe_ipv6_connection *all_connections_head;
	/* Head of the list of all connections */
	struct sfe_ipv6_connection *all_connections_tail;
	/* Tail of the list of all connections */
	unsigned int num_connections;	/* Number of connections */
	struct timer_list timer;	/* Timer used for periodic sync ops */
	sfe_sync_rule_callback_t __rcu sync_rule_callback;
	/* Callback function registered by a connection manager for stats syncing */
	struct sfe_ipv6_connection *conn_hash[SFE_IPV6_CONNECTION_HASH_SIZE];
	/* Connection hash table */
	struct sfe_ipv6_connection_match *conn_match_hash[SFE_IPV6_CONNECTION_HASH_SIZE];
	/* Connection match hash table */
#ifdef CONFIG_NF_FLOW_COOKIE
	struct sfe_ipv6_flow_cookie_entry sfe_flow_cookie_table[SFE_FLOW_COOKIE_SIZE];
	/* flow cookie table*/
	sfe_ipv6_flow_cookie_set_func_t flow_cookie_set_func;
	/* function used to configure flow cookie in hardware*/
#endif

	/*
	 * Statistics.
	 */
	uint32_t connection_create_requests;
	/* Number of IPv6 connection create requests */
	uint32_t connection_create_collisions;
	/* Number of IPv6 connection create requests that collided with existing hash table entries */
	uint32_t connection_destroy_requests;
	/* Number of IPv6 connection destroy requests */
	uint32_t connection_destroy_misses;
	/* Number of IPv6 connection destroy requests that missed our hash table */
	uint32_t connection_match_hash_hits;
	/* Number of IPv6 connection match hash hits */
	uint32_t connection_match_hash_reorders;
	/* Number of IPv6 connection match hash reorders */
	uint32_t connection_flushes;	/* Number of IPv6 connection flushes */
	uint32_t packets_forwarded;	/* Number of IPv6 packets forwarded */
	uint32_t packets_not_forwarded;	/* Number of IPv6 packets not forwarded */
	uint32_t exception_events[SFE_IPV6_EXCEPTION_EVENT_LAST];

	/*
	 * Summary tatistics.
	 */
	uint64_t connection_create_requests64;
	/* Number of IPv6 connection create requests */
	uint64_t connection_create_collisions64;
	/* Number of IPv6 connection create requests that collided with existing hash table entries */
	uint64_t connection_destroy_requests64;
	/* Number of IPv6 connection destroy requests */
	uint64_t connection_destroy_misses64;
	/* Number of IPv6 connection destroy requests that missed our hash table */
	uint64_t connection_match_hash_hits64;
	/* Number of IPv6 connection match hash hits */
	uint64_t connection_match_hash_reorders64;
	/* Number of IPv6 connection match hash reorders */
	uint64_t connection_flushes64;	/* Number of IPv6 connection flushes */
	uint64_t packets_forwarded64;	/* Number of IPv6 packets forwarded */
	uint64_t packets_not_forwarded64;
	/* Number of IPv6 packets not forwarded */
	uint64_t exception_events64[SFE_IPV6_EXCEPTION_EVENT_LAST];
	sfe_proc_sys_db_t proc1;

	/*
	 * Control state here
	 */
	struct kobject *sys_sfe_ipv6;	/* sysfs linkage */
	struct kobject *sys_sfe_ipv6_packet_stats;	/* sysfs linkage */
	int debug_dev;			/* Major number of the debug char device */
	int packet_stats_dev; /*packet stats char device*/
	uint32_t debug_read_seq;	/* sequence number for debug dump */
	uint32_t pack_stats_read_seq;	/* sequence number for packet stats dump */

	/*
	 *  Proc entry for Interface name
	 */
	char ipv6_iface[MAX_INTF_LEN];
	int iface_length;

	/*
	 *  packet stats list
	 */
	struct hlist_head packet_stats_htable[SFE_IPV6_PACKET_STATS_HASH_SIZE]; //hash table
	uint32_t num_of_pack_stat_nodes;/* Number of nodes in pack stats list */
};

/*
 * Enumeration of the XML output.
 */
enum sfe_ipv6_debug_xml_states {
	SFE_IPV6_DEBUG_XML_STATE_START,
	SFE_IPV6_DEBUG_XML_STATE_CONNECTIONS_START,
	SFE_IPV6_DEBUG_XML_STATE_CONNECTIONS_CONNECTION,
	SFE_IPV6_DEBUG_XML_STATE_CONNECTIONS_END,
	SFE_IPV6_DEBUG_XML_STATE_EXCEPTIONS_START,
	SFE_IPV6_DEBUG_XML_STATE_EXCEPTIONS_EXCEPTION,
	SFE_IPV6_DEBUG_XML_STATE_EXCEPTIONS_END,
	SFE_IPV6_DEBUG_XML_STATE_STATS,
	SFE_IPV6_DEBUG_XML_STATE_END,
	SFE_IPV6_DEBUG_XML_STATE_DONE
};

/*
 * XML write state.
 */
struct sfe_ipv6_debug_xml_write_state {
	enum sfe_ipv6_debug_xml_states state;
	/* XML output file state machine state */
	int iter_exception;		/* Next exception iterator */
};

typedef bool (*sfe_ipv6_debug_xml_write_method_t)(struct sfe_ipv6 *si, char *buffer, char *msg, size_t *length,
		int *total_read, struct sfe_ipv6_debug_xml_write_state *ws);

struct sfe_ipv6 __si6;
/*
 * sfe_ipv6_get_debug_dev()
 */
static ssize_t sfe_ipv6_get_debug_dev(struct device *dev, struct device_attribute *attr, char *buf);

/*
 * sysfs attributes.
 */
static const struct device_attribute sfe_ipv6_debug_dev_attr =
__ATTR(debug_dev, 0664, sfe_ipv6_get_debug_dev, NULL);

/*
 * sfe_ipv6_debug_level_show
 * dump the current debug level value
 */
static ssize_t sfe_ipv6_debug_level_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n", debug_level);
}

/*
 * sfe_ipv6_debug_level_store
 * change the debug level value by valu provided by user
 */
static ssize_t sfe_ipv6_debug_level_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	int tmp = 0;

	if (sscanf(buf, "%du", &tmp) < 0)
		pr_err("sscanf failed\n");
	else
		debug_level = tmp;
	return count;
}


/*
 * sysfs attributes.
 */
static const struct device_attribute sfe_debug_level =
__ATTR(debug_level, 0660, sfe_ipv6_debug_level_show,
	sfe_ipv6_debug_level_store);

/*
 * sfe_ipv6_debug_level_low_show
 * dump the current value
 */
static ssize_t sfe_ipv6_debug_level_low_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n", sfe_v6_enable_ipc_low);
}

/*
 * sfe_ipv6_debug_level_low_store
 * Enable/disable sfe ipv6 low level logging
 */
static ssize_t sfe_ipv6_debug_level_low_store(struct device *dev,
			struct device_attribute *attr, const char *buf,
			size_t count)
{
	int tmp = 0;

	if (sscanf(buf, "%du", &tmp) < 0)
		pr_err("sscanf failed\n");
	else {
		if (tmp) {
			if (!ipc_sfe_log_ctxt_low) {
				ipc_sfe_log_ctxt_low =
						ipc_log_context_create(
						IPCLOG_STATE_PAGES,
						"sfe_ipv4_low", 0);
			}
			if (!ipc_sfe_log_ctxt_low) {
				pr_err("failed to create ipc sfe low context\n");
				return -EFAULT;
			}
		} else {
			if (ipc_sfe_log_ctxt_low)
				ipc_log_context_destroy(ipc_sfe_log_ctxt_low);
				ipc_sfe_log_ctxt_low = NULL;
		}
	}
	sfe_v6_enable_ipc_low = tmp;
	return count;
}


/*
 * sysfs attributes.
 */
static const struct device_attribute sfe_debug_level_low =
__ATTR(sfe_v6_enable_ipc_low, 0660,
	sfe_ipv6_debug_level_low_show, sfe_ipv6_debug_level_low_store);


/*
 * Packet stats framework
 */


static inline void sfe_ipv6_connection_match_update_summary_stats(struct sfe_ipv6_connection_match *cm);
static inline int sfe_ipv6_addr_equal(struct sfe_ipv6_addr *a,
		struct sfe_ipv6_addr *b);

#define PACKET_STATS_MSG 30
#define NL_MESSAGE_TYPE PACKET_STATS_MSG
#define NL_MAX_BUF 1024
#define NL_UNICAST_GRP 0
#define NL_IPV6_PROTO_ID 25
#define SFE_IPV6_RESET_PACKET_STATS_COUNTERS 0xAA
#define SFE_IPV6_DELETE_PACKET_STATS_NODE 0xAB
struct sock *nl_socket = NULL;
uint32_t gPID = 0;
struct sfe_ipv6_packet_stats_node
{
	struct sfe_ipv6_addr client_src_addr[1];
	uint64_t packet_stat_node_rx_byte_count;
	uint64_t packet_stat_node_tx_byte_count;
};
struct nl_rx_buffer
{
	uint8_t command;
	__be32 client_src_addr[4];
};
struct sfe_ipv6_packet_stats_list
{
	uint32_t packet_stat_node_read_seq;
	/* Next  entry in a list */
	struct hlist_node sfe_ipv6_packet_hash_list;
	struct sfe_ipv6_packet_stats_node packet_stats_node;
};
static void sfe_ipv6_nl_receive(struct sk_buff *skb);

static struct netlink_kernel_cfg nl_ipv6_cfg = {
	.input = sfe_ipv6_nl_receive,
	.groups = NL_UNICAST_GRP,
	.flags = 0,
};
/*
 * sfe_ipv6_get_packet_stats_dev()
 */
static ssize_t sfe_ipv6_get_packet_stats_dev(struct device *dev, struct device_attribute *attr, char *buf);
static void sfe_ipv6_insert_packet_stats_connection(struct sfe_ipv6 *si, struct sfe_ipv6_packet_stats_list* node);


/*
 * sysfs attributes.
 */
static const struct device_attribute sfe_ipv6_packet_stats_dev_attr =
__ATTR(packet_stats_dev, 0664, sfe_ipv6_get_packet_stats_dev, NULL);


/*
 * sfe_ipv6_destroy_packet_stats_list()
 *	destroy the list and reclaim memory
 *
 *
 */

static void sfe_ipv6_destroy_packet_stats_list(void)
{
	struct sfe_ipv6 *si = &__si6;

	struct sfe_ipv6_packet_stats_list* curr;
	struct hlist_node *tmp;
	int bkt;

	hash_for_each_safe(si->packet_stats_htable, bkt, tmp, curr, sfe_ipv6_packet_hash_list) {
		DEBUG_INFO("deleting node %pI6 , conn %d\n",
				&curr->packet_stats_node.client_src_addr, si->num_of_pack_stat_nodes);
		hash_del(&curr->sfe_ipv6_packet_hash_list);
		kfree(curr);
		si->num_of_pack_stat_nodes--;
	}

}

static u32 ht_conn_hash(unsigned long *saddr)
{
	return (saddr[0] ^ saddr[1] ^ saddr[2] ^ saddr[3] ^ ( 5 << 16));
}


/*
 * sfe_ipv6_remove_packet_stats_connection()
 *	remove a connection from the list for packet stats.
 *
 * On entry we must be holding the lock that protects the list.
 */

static void sfe_ipv6_remove_packet_stats_connection(struct sfe_ipv6_addr * client_addr)
{
	struct sfe_ipv6 *si = &__si6;
	struct sfe_ipv6_packet_stats_list* curr;
	int bkt;
	struct hlist_node *tmp;
	u32 key;
	key = ht_conn_hash((unsigned long *) client_addr);

	spin_lock_bh(&si->lock);
	hash_for_each_possible(si->packet_stats_htable, curr, sfe_ipv6_packet_hash_list, key) {
		if (sfe_ipv6_addr_equal(client_addr,
				(struct sfe_ipv6_addr *)&curr->
				packet_stats_node.client_src_addr)) {
			DEBUG_INFO("Connection found");
			hash_del(&curr->sfe_ipv6_packet_hash_list);
			kfree(curr);
			si->num_of_pack_stat_nodes--;
			spin_unlock_bh(&si->lock);
			return;
		}
	}
	DEBUG_INFO("Connection not found\n");
	spin_unlock_bh(&si->lock);
}


/*
 * sfe_ipv6_insert_packet_stats_connection()
 *	Insert a connection into the list for packet stats.
 *
 * On entry we must be holding the lock that protects the list.
 */

static void sfe_ipv6_insert_packet_stats_connection(struct sfe_ipv6 *si, struct sfe_ipv6_packet_stats_list* node)
{
	struct sfe_ipv6_packet_stats_list* curr;
	int bkt;
	struct hlist_node *tmp;
	u32 key;

	key = ht_conn_hash((unsigned long *)
				node->packet_stats_node.client_src_addr);
	hash_for_each_possible(si->packet_stats_htable, curr, sfe_ipv6_packet_hash_list, key) {
		if (sfe_ipv6_addr_equal(
				(struct sfe_ipv6_addr *)
				&node->packet_stats_node.client_src_addr,
				(struct sfe_ipv6_addr *)
				&curr->packet_stats_node.client_src_addr)) {
			return;
		}
	}
	key = ht_conn_hash((unsigned long *)
			node->packet_stats_node.client_src_addr);
	hash_add(si->packet_stats_htable , &node->sfe_ipv6_packet_hash_list, key);
	si->num_of_pack_stat_nodes++;
}

/*
 * sfe_ipv6_update_packet_stats_connections()
 *	update  the  connections
 *
 * On entry we must be holding the lock that protects the list.
 */

static bool sfe_ipv6_update_packet_stats_connection(struct sfe_ipv6* sic,struct sfe_ipv6_addr * client_addr, uint64_t rx_bytes, uint64_t tx_bytes )
{
	struct sfe_ipv6_packet_stats_list* curr;
	int bkt;
	struct hlist_node *tmp;
	u32 key;

	key = ht_conn_hash((unsigned long *)client_addr);
	hash_for_each_possible(sic->packet_stats_htable, curr, sfe_ipv6_packet_hash_list, key) {
		if ((sfe_ipv6_addr_equal(client_addr, (
					struct sfe_ipv6_addr *)&curr->
					packet_stats_node.client_src_addr)))
		{
			curr->packet_stats_node.packet_stat_node_rx_byte_count += rx_bytes;
			curr->packet_stats_node.packet_stat_node_tx_byte_count += tx_bytes;
			DEBUG_INFO("updating client %pI6, rx %llu, tx %llu \n", client_addr,
					curr->packet_stats_node.packet_stat_node_rx_byte_count,
					curr->packet_stats_node.packet_stat_node_rx_byte_count);

			return true;
		}
	}

	return false;
}

/*
 * sfe_ipv6_reset_packet_stats_counters()
 *	reset counters of all connection in the packet stats list.
 *
 * On entry we must be holding the lock that protects the list.
 */

static void sfe_ipv6_reset_packet_stats_counters(struct sfe_ipv6* sic)
{
	struct sfe_ipv6_packet_stats_list* curr;
	int bkt;
	struct hlist_node *tmp;

	hash_for_each_safe(sic->packet_stats_htable, bkt, tmp, curr, sfe_ipv6_packet_hash_list) {
		DEBUG_INFO("reseting counters for client %pI6 \n", &curr->packet_stats_node.client_src_addr[0]);
		spin_lock_bh(&sic->lock);
		curr->packet_stats_node.packet_stat_node_rx_byte_count = 0;
		curr->packet_stats_node.packet_stat_node_tx_byte_count = 0;
		spin_unlock_bh(&sic->lock);
	}
}

/*
 * sfe_ipv6_get_packet_stats_dev()
 */
static ssize_t sfe_ipv6_get_packet_stats_dev(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
	struct sfe_ipv6 *si = &__si6;
	ssize_t count;
	int num;

	spin_lock_bh(&si->lock);
	num = si->packet_stats_dev;
	spin_unlock_bh(&si->lock);

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "%d\n", num);
	return count;
}
/*
 * Enumeration of the XML output for packet stats.
 */
enum sfe_ipv6_packet_stats_xml_states {
	SFE_IPV6_PACKET_STATS_XML_HEADER_START,
	SFE_IPV6_PACKET_STATS_XML_STATE_START,
	SFE_IPV6_PACKET_STATS_XML_STATS_STATE_START,
	SFE_IPV6_PACKET_STATS_XML_STATE_CONNECTIONS_START,
	SFE_IPV6_PACKET_STATS_XML_STATE_CONNECTIONS_CONNECTION,
	SFE_IPV6_PACKET_STATS_XML_STATE_DISPLAY_CONNECTIONS_CONNECTION,
	SFE_IPV6_PACKET_STATS_XML_STATE_CONNECTIONS_END,
	SFE_IPV6_PACKET_STATS_XML_STATE_END,
	SFE_IPV6_PACKET_STATS_XML_STATE_DONE
};
/*
 * XML write state for packet stats..
 */
struct sfe_ipv6_packet_stats_xml_write_state {
	enum sfe_ipv6_packet_stats_xml_states state;
	/* XML output file state machine state */
	int iter_exception;		/* Next exception iterator */
};

typedef bool (*sfe_ipv6_packet_stats_xml_write_method_t)(struct sfe_ipv6 *si, char *buffer, char *msg, size_t *length,
		int *total_read, struct sfe_ipv6_packet_stats_xml_write_state *ws);

/*
 * sfe_ipv6_packet_stats_read_xml_header_start()
 *	Generate part of the XML output.
 */
static bool sfe_ipv6_packet_stats_read_xml_header_start(struct sfe_ipv6 *si, char *buffer, char *msg, size_t *length,
		int *total_read, struct sfe_ipv6_packet_stats_xml_write_state *ws)
{
	int bytes_read;

	si->debug_read_seq++;
	si->pack_stats_read_seq++;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "<?xml version = '1.0' encoding = 'UTF-8'?>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	ws->state++;
	return true;
}
/*
 * sfe_ipv6_packet_stats_read_xml_state_start()
 *	Generate part of the XML output.
 */
static bool sfe_ipv6_packet_stats_read_xml_state_start(struct sfe_ipv6 *si, char *buffer, char *msg, size_t *length,
		int *total_read, struct sfe_ipv6_packet_stats_xml_write_state *ws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t<packet_stats>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	ws->state++;
	return true;
}
/*
 * sfe_ipv6_packet_stats_read_stats_start()
 *	Generate part of the XML output.
 */
static bool sfe_ipv6_packet_stats_read_stats_start(struct sfe_ipv6 *si, char *buffer, char *msg, size_t *length,
		int *total_read, struct sfe_ipv6_packet_stats_xml_write_state *ws)
{
	int bytes_read;
	unsigned int num_connections;
	spin_lock_bh(&si->lock);
	num_connections = si->num_of_pack_stat_nodes;
	spin_unlock_bh(&si->lock);
	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t\t<stats "
			"num_connections=\"%u\" />\n",
			num_connections);
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	ws->state++;
	return true;

}
/*
 * sfe_ipv6_packet_stats_read_connections_start()
 *	Generate part of the XML output.
 */
static bool sfe_ipv6_packet_stats_read_connections_start(struct sfe_ipv6 *si, char *buffer, char *msg, size_t *length,
		int *total_read, struct sfe_ipv6_packet_stats_xml_write_state *ws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t\t<connections>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	ws->state++;
	return true;
}
/*
 * sfe_ipv6_packet_stats_read_connections_connection()
 *	Generate part of the XML output.
 */
static bool sfe_ipv6_packet_stats_read_connections_connection(struct sfe_ipv6 *si, char *buffer, char *msg, size_t *length,
		int *total_read, struct sfe_ipv6_packet_stats_xml_write_state *ws)
{
	struct sfe_ipv6_connection *c;
	struct sfe_ipv6_connection_match *original_cm;
	struct sfe_ipv6_connection_match *reply_cm;
	int bytes_read = 0;
	struct sfe_ipv6_addr client_ip;
	uint64_t tx_bytes;
	uint64_t rx_bytes;
	bool dest_dev_valid_for_pack_stats = false;
	bool src_dev_valid_for_pack_stats = false;
	bool ret = false;

	spin_lock_bh(&si->lock);

	for (c = si->all_connections_head; c; c = c->all_connections_next) {
		if (c->debug_read_seq < si->debug_read_seq) {
			c->debug_read_seq = si->debug_read_seq;
			break;
		}
	}

	/*
	 * If there were no connections then move to the next state.
	 */
	if (!c) {
		spin_unlock_bh(&si->lock);
		ws->state++;
		return true;
	}

	if (strncmp(c->original_dev->name, si->ipv6_iface, strlen(si->ipv6_iface) - 1) == 0)
	{
		src_dev_valid_for_pack_stats = true;
		if (!sfe_ipv6_addr_equal(c->dest_ip, c->dest_ip_xlate))
		{
			client_ip = c->dest_ip_xlate[0];
		}
		else
		{
			client_ip = c->dest_ip[0];
		}
	}
	else if (strncmp(c->reply_dev->name, si->ipv6_iface, strlen(si->ipv6_iface) - 1) == 0)
	{
		dest_dev_valid_for_pack_stats = true;
		client_ip = c->src_ip[0];
	}
	else
	{
		//the connection is not valid for packet stats
		spin_unlock_bh(&si->lock);
		goto exit1;
	}
	original_cm = c->original_match;
	reply_cm = c->reply_match;


	sfe_ipv6_connection_match_update_summary_stats(original_cm);
	sfe_ipv6_connection_match_update_summary_stats(reply_cm);

	//find connection in packet stat list..
	if (src_dev_valid_for_pack_stats)
	{
		DEBUG_INFO("updating  for %pI6, orig(rx) %d, reply(tx) %d \n", &client_ip,
				original_cm->rx_pack_stat_byte_count,reply_cm->rx_pack_stat_byte_count);
		/*in this case rmnet_data is src of data so src_rx_bytes is data in
		  uplink dir so they need to be added to tx_bytes of packet stats list*/
		tx_bytes = reply_cm->rx_pack_stat_byte_count;
		reply_cm->rx_pack_stat_byte_count = 0;
		rx_bytes = original_cm->rx_pack_stat_byte_count;
		original_cm->rx_pack_stat_byte_count = 0;
		ret = sfe_ipv6_update_packet_stats_connection(si, &client_ip, rx_bytes, tx_bytes);
	}
	else if (dest_dev_valid_for_pack_stats)
	{
		DEBUG_INFO("updating  for %pI6, orig(tx) %d, reply(rx) %d \n", &client_ip,
				original_cm->rx_pack_stat_byte_count,reply_cm->rx_pack_stat_byte_count);
		tx_bytes = original_cm->rx_pack_stat_byte_count;
		original_cm->rx_pack_stat_byte_count = 0;
		rx_bytes = reply_cm->rx_pack_stat_byte_count;
		reply_cm->rx_pack_stat_byte_count = 0;
		/*in this case rmnet_data is receiver  of data so src_rx_bytes is data in
		  downlink dir so they need to be added to rx_bytes of packet stats list*/
		ret = sfe_ipv6_update_packet_stats_connection(si, &client_ip, rx_bytes, tx_bytes);
	}

	if (!ret)
	{
		DEBUG_INFO("pack stat node not found\n");
	}


	spin_unlock_bh(&si->lock);

exit1:
	*length -= bytes_read;
	*total_read += bytes_read;

	return true;
}

/*
 * sfe_ipv6_packet_stats_display_connections_connection()
 *	Generate part of the XML output.
 */
static bool sfe_ipv6_packet_stats_display_connections_connection(struct sfe_ipv6 *si, char *buffer, char *msg, size_t *length,
		int *total_read, struct sfe_ipv6_packet_stats_xml_write_state *ws)
{
	struct sfe_ipv6_packet_stats_list* curr;
	int bkt;
	struct hlist_node *tmp;
	uint32_t bytes_read;
	int valid_conn = 0;

	spin_lock_bh(&si->lock);

	hash_for_each_safe(si->packet_stats_htable, bkt, tmp, curr, sfe_ipv6_packet_hash_list) {
		if (curr->packet_stat_node_read_seq < si->pack_stats_read_seq) {
			curr->packet_stat_node_read_seq = si->pack_stats_read_seq;
			valid_conn = 1;
			break;
		}
	}

	/*
	 * If there were no connections then move to the next state.
	 */
	if (hash_empty(si->packet_stats_htable)) {
		spin_unlock_bh(&si->lock);
		ws->state++;
		return true;
	}

	spin_unlock_bh(&si->lock);
	if (!valid_conn) {
		ws->state++;
		return true;
	}

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t\t\t<connection "
			"client_addr=\"%pI6\" "
			"rx_bytes=\"%llu\" tx_bytes=\"%llu\" />\n",
			&curr->packet_stats_node.client_src_addr,
			curr->packet_stats_node.packet_stat_node_rx_byte_count,
			curr->packet_stats_node.packet_stat_node_tx_byte_count);

	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}


	*length -= bytes_read;
	*total_read += bytes_read;

	return true;
}

/*
 * sfe_ipv6_packet_stats_read_connections_end()
 *	Generate part of the XML output.
 */
static bool sfe_ipv6_packet_stats_read_connections_end(struct sfe_ipv6 *si, char *buffer, char *msg, size_t *length,
		int *total_read, struct sfe_ipv6_packet_stats_xml_write_state *ws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t\t</connections>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	ws->state++;
	return true;
}
/*
 * sfe_ipv6_packet_stats_read_xml_state_end()
 *	Generate part of the XML output.
 */
static bool sfe_ipv6_packet_stats_read_xml_state_end(struct sfe_ipv6 *si, char *buffer, char *msg, size_t *length,
		int *total_read, struct sfe_ipv6_packet_stats_xml_write_state *ws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t</packet_stats>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	ws->state++;
	return true;
}
/*
 * Array of write functions that write various XML elements that correspond to
 * our XML output state machine.
 */
static sfe_ipv6_packet_stats_xml_write_method_t sfe_ipv6_packet_stats_xml_write_methods[SFE_IPV6_PACKET_STATS_XML_STATE_DONE] = {
	sfe_ipv6_packet_stats_read_xml_header_start,
	sfe_ipv6_packet_stats_read_xml_state_start,
	sfe_ipv6_packet_stats_read_stats_start,
	sfe_ipv6_packet_stats_read_connections_start,
	sfe_ipv6_packet_stats_read_connections_connection,
	sfe_ipv6_packet_stats_display_connections_connection,
	sfe_ipv6_packet_stats_read_connections_end,
	sfe_ipv6_packet_stats_read_xml_state_end,

};
/*
 * sfe_ipv6_packet_stats_read()
 *	Send info to userspace upon read request from user
 */
static ssize_t sfe_ipv6_packet_stats_read(struct file *filp, char *buffer, size_t length, loff_t *offset)
{
	char msg[CHAR_DEV_MSG_SIZE];
	int total_read = 0;
	struct sfe_ipv6_packet_stats_xml_write_state *ws;
	struct sfe_ipv6 *si = &__si6;
	if (packet_stats_enabled)
	{
		ws = (struct sfe_ipv6_packet_stats_xml_write_state *)filp->private_data;
		while ((ws->state != SFE_IPV6_PACKET_STATS_XML_STATE_DONE) && (length > CHAR_DEV_MSG_SIZE)) {
			if ((sfe_ipv6_packet_stats_xml_write_methods[ws->state])(si, buffer, msg, &length, &total_read, ws)) {
				continue;
			}
		}
	}

	return total_read;
}
/*
 * sfe_ipv6_packet_stats_write()
 *	Write to packet stats char device
 */
static ssize_t sfe_ipv6_packet_stats_write(struct file *filp, const char *buffer, size_t length, loff_t *offset)
{


	return length;
}

/*
 * sfe_ipv6_packet_stats_open()
 */
static int sfe_ipv6_packet_stats_open(struct inode *inode, struct file *file)
{
	struct sfe_ipv6_packet_stats_xml_write_state *ws;

	ws = (struct sfe_ipv6_packet_stats_xml_write_state *)file->private_data;
	if (ws) {
		return 0;
	}

	ws = kzalloc(sizeof(struct sfe_ipv6_packet_stats_xml_write_state), GFP_KERNEL);
	if (!ws) {
		return -ENOMEM;
	}

	ws->state = SFE_IPV6_PACKET_STATS_XML_HEADER_START;
	file->private_data = ws;

	return 0;
}

/*
 * sfe_ipv6_packet_stats_release()
 */
static int sfe_ipv6_packet_stats_release(struct inode *inode, struct file *file)
{
	struct sfe_ipv6_packet_stats_xml_write_state *ws;

	ws = (struct sfe_ipv6_packet_stats_xml_write_state *)file->private_data;
	if (ws) {
		/*
		 * We've finished with our output so free the write state.
		 */
		kfree(ws);
	}

	return 0;
}
/*
 * File operations used in the packet stats char device
 */
static struct file_operations sfe_ipv6_packet_stats_fops = {
	.read = sfe_ipv6_packet_stats_read,
	.write = sfe_ipv6_packet_stats_write,
	.open = sfe_ipv6_packet_stats_open,
	.release = sfe_ipv6_packet_stats_release
};

static void sfe_ipv6_nl_receive(struct sk_buff *skb)
{
	struct nlmsghdr *nlheader;
	struct nl_rx_buffer *nl_data_ptr = NULL;

	if (packet_stats_enabled)
	{
		DEBUG_INFO("Entering: %s\n", __FUNCTION__);
		nl_data_ptr = (struct nl_rx_buffer *)kmalloc(sizeof(struct nl_rx_buffer), GFP_KERNEL);

		if (nl_data_ptr == NULL)
		{
			DEBUG_INFO("Cannot allocate memmory for NL msg\n");
			return;
		}

		nlheader = (struct nlmsghdr *)skb->data;
		memcpy(nl_data_ptr, (struct nl_rx_buffer *)nlmsg_data(nlheader), sizeof(struct nl_rx_buffer));
		gPID = nlheader->nlmsg_pid;
		switch (nl_data_ptr->command)
		{
			case SFE_IPV6_RESET_PACKET_STATS_COUNTERS:
				DEBUG_INFO("Reset Counters cmd received\n");
				sfe_ipv6_reset_packet_stats_counters(&__si6);
				break;
			case SFE_IPV6_DELETE_PACKET_STATS_NODE:
				DEBUG_INFO("deleting addr %pI6\n", &nl_data_ptr->client_src_addr);
				sfe_ipv6_remove_packet_stats_connection((struct sfe_ipv6_addr *)&nl_data_ptr->client_src_addr);
				break;
			default:
				DEBUG_INFO("IPV6 Invalid NL command %d  received from pid %d\n",nl_data_ptr->command, gPID);
				break;
		}

		kfree(nl_data_ptr);
	}
	else
	{
		DEBUG_INFO("packet stats not enabled\n");
	}

}


/*
 * Packet stats end framework
 */



/*
 * sfe_ipv6_addr_equal()
 *	compare ipv6 address
 *
 * return: 1, equal; 0, no equal
 */
static inline int sfe_ipv6_addr_equal(struct sfe_ipv6_addr *a,
		struct sfe_ipv6_addr *b)
{
	return a->addr[0] == b->addr[0] &&
		a->addr[1] == b->addr[1] &&
		a->addr[2] == b->addr[2] &&
		a->addr[3] == b->addr[3];
}

/*
 * sfe_ipv6_is_ext_hdr()
 *	check if we recognize ipv6 extension header
 */
static inline bool sfe_ipv6_is_ext_hdr(uint8_t hdr)
{
	return (hdr == SFE_IPV6_EXT_HDR_HOP) ||
		(hdr == SFE_IPV6_EXT_HDR_ROUTING) ||
		(hdr == SFE_IPV6_EXT_HDR_FRAG) ||
		(hdr == SFE_IPV6_EXT_HDR_AH) ||
		(hdr == SFE_IPV6_EXT_HDR_DST) ||
		(hdr == SFE_IPV6_EXT_HDR_MH);
}

/*
 * sfe_ipv6_get_connection_match_hash()
 * Generate the hash used in connection match lookups.
 */
static inline unsigned int sfe_ipv6_get_connection_match_hash(struct net_device *dev, uint8_t protocol,
		struct sfe_ipv6_addr *src_ip, __be16 src_port,
		struct sfe_ipv6_addr *dest_ip, __be16 dest_port)
{
	uint32_t idx, hash = 0;
	size_t dev_addr = (size_t)dev;

	for (idx = 0; idx < 4; idx++) {
		hash ^= src_ip->addr[idx] ^ dest_ip->addr[idx];
	}
	hash = ((uint32_t)dev_addr) ^ hash ^ protocol ^ ntohs(src_port ^ dest_port);
	return ((hash >> SFE_IPV6_CONNECTION_HASH_SHIFT) ^ hash) & SFE_IPV6_CONNECTION_HASH_MASK;
}

/*
 * sfe_ipv6_find_connection_match()
 *	Get the IPv6 flow match info that corresponds to a particular 5-tuple.
 *
 * On entry we must be holding the lock that protects the hash table.
 */
	static struct sfe_ipv6_connection_match *
sfe_ipv6_find_connection_match(struct sfe_ipv6 *si, struct net_device *dev, uint8_t protocol,
		struct sfe_ipv6_addr *src_ip, __be16 src_port,
		struct sfe_ipv6_addr *dest_ip, __be16 dest_port)
{
	struct sfe_ipv6_connection_match *cm;
	struct sfe_ipv6_connection_match *head;
	unsigned int conn_match_idx;

	conn_match_idx = sfe_ipv6_get_connection_match_hash(dev, protocol, src_ip, src_port, dest_ip, dest_port);
	cm = si->conn_match_hash[conn_match_idx];

	/*
	 * If we don't have anything in this chain then bale.
	 */
	if (unlikely(!cm)) {
		return cm;
	}

	/*
	 * Hopefully the first entry is the one we want.
	 */
	if (likely(cm->match_src_port == src_port)
			&& likely(cm->match_dest_port == dest_port)
			&& likely(sfe_ipv6_addr_equal(cm->match_src_ip, src_ip))
			&& likely(sfe_ipv6_addr_equal(cm->match_dest_ip, dest_ip))
			&& likely(cm->match_protocol == protocol)
			&& likely(cm->match_dev == dev)) {
		si->connection_match_hash_hits++;
		return cm;
	}

	/*
	 * We may or may not have a matching entry but if we do then we want to
	 * move that entry to the top of the hash chain when we get to it.  We
	 * presume that this will be reused again very quickly.
	 */
	head = cm;
	do {
		cm = cm->next;
	} while (cm && (cm->match_src_port != src_port
				|| cm->match_dest_port != dest_port
				|| !sfe_ipv6_addr_equal(cm->match_src_ip, src_ip)
				|| !sfe_ipv6_addr_equal(cm->match_dest_ip, dest_ip)
				|| cm->match_protocol != protocol
				|| cm->match_dev != dev));

	/*
	 * Not found then we're done.
	 */
	if (unlikely(!cm)) {
		return cm;
	}

	/*
	 * We found a match so move it.
	 */
	if (cm->next) {
		cm->next->prev = cm->prev;
	}
	cm->prev->next = cm->next;
	cm->prev = NULL;
	cm->next = head;
	head->prev = cm;
	si->conn_match_hash[conn_match_idx] = cm;
	si->connection_match_hash_reorders++;

	return cm;
}

/*
 * sfe_ipv6_connection_match_update_summary_stats()
 *	Update the summary stats for a connection match entry.
 */
static inline void sfe_ipv6_connection_match_update_summary_stats(struct sfe_ipv6_connection_match *cm)
{
	cm->rx_packet_count64 += cm->rx_packet_count;
	cm->rx_packet_count = 0;
	cm->rx_byte_count64 += cm->rx_byte_count;
	cm->rx_byte_count = 0;
}

/*
 * sfe_ipv6_connection_match_compute_translations()
 *	Compute port and address translations for a connection match entry.
 */
static void sfe_ipv6_connection_match_compute_translations(struct sfe_ipv6_connection_match *cm)
{
	uint32_t diff[9];
	uint32_t *idx_32;
	uint16_t *idx_16;

	/*
	 * Before we insert the entry look to see if this is tagged as doing address
	 * translations.  If it is then work out the adjustment that we need to apply
	 * to the transport checksum.
	 */
	if (cm->flags & SFE_IPV6_CONNECTION_MATCH_FLAG_XLATE_SRC) {
		uint32_t adj = 0;
		uint32_t carry = 0;

		/*
		 * Precompute an incremental checksum adjustment so we can
		 * edit packets in this stream very quickly.  The algorithm is from RFC1624.
		 */
		idx_32 = diff;
		*(idx_32++) = cm->match_src_ip->addr[0];
		*(idx_32++) = cm->match_src_ip->addr[1];
		*(idx_32++) = cm->match_src_ip->addr[2];
		*(idx_32++) = cm->match_src_ip->addr[3];

		idx_16 = (uint16_t *)idx_32;
		*(idx_16++) = cm->match_src_port;
		*(idx_16++) = ~cm->xlate_src_port;
		idx_32 = (uint32_t *)idx_16;

		*(idx_32++) = ~cm->xlate_src_ip->addr[0];
		*(idx_32++) = ~cm->xlate_src_ip->addr[1];
		*(idx_32++) = ~cm->xlate_src_ip->addr[2];
		*(idx_32++) = ~cm->xlate_src_ip->addr[3];

		/*
		 * When we compute this fold it down to a 16-bit offset
		 * as that way we can avoid having to do a double
		 * folding of the twos-complement result because the
		 * addition of 2 16-bit values cannot cause a double
		 * wrap-around!
		 */
		for (idx_32 = diff; idx_32 < diff + 9; idx_32++) {
			uint32_t w = *idx_32;
			adj += carry;
			adj += w;
			carry = (w > adj);
		}
		adj += carry;
		adj = (adj & 0xffff) + (adj >> 16);
		adj = (adj & 0xffff) + (adj >> 16);
		cm->xlate_src_csum_adjustment = (uint16_t)adj;
	}

	if (cm->flags & SFE_IPV6_CONNECTION_MATCH_FLAG_XLATE_DEST) {
		uint32_t adj = 0;
		uint32_t carry = 0;

		/*
		 * Precompute an incremental checksum adjustment so we can
		 * edit packets in this stream very quickly.  The algorithm is from RFC1624.
		 */
		idx_32 = diff;
		*(idx_32++) = cm->match_dest_ip->addr[0];
		*(idx_32++) = cm->match_dest_ip->addr[1];
		*(idx_32++) = cm->match_dest_ip->addr[2];
		*(idx_32++) = cm->match_dest_ip->addr[3];

		idx_16 = (uint16_t *)idx_32;
		*(idx_16++) = cm->match_dest_port;
		*(idx_16++) = ~cm->xlate_dest_port;
		idx_32 = (uint32_t *)idx_16;

		*(idx_32++) = ~cm->xlate_dest_ip->addr[0];
		*(idx_32++) = ~cm->xlate_dest_ip->addr[1];
		*(idx_32++) = ~cm->xlate_dest_ip->addr[2];
		*(idx_32++) = ~cm->xlate_dest_ip->addr[3];

		/*
		 * When we compute this fold it down to a 16-bit offset
		 * as that way we can avoid having to do a double
		 * folding of the twos-complement result because the
		 * addition of 2 16-bit values cannot cause a double
		 * wrap-around!
		 */
		for (idx_32 = diff; idx_32 < diff + 9; idx_32++) {
			uint32_t w = *idx_32;
			adj += carry;
			adj += w;
			carry = (w > adj);
		}
		adj += carry;
		adj = (adj & 0xffff) + (adj >> 16);
		adj = (adj & 0xffff) + (adj >> 16);
		cm->xlate_dest_csum_adjustment = (uint16_t)adj;
	}
}

/*
 * sfe_ipv6_update_summary_stats()
 *	Update the summary stats.
 */
static void sfe_ipv6_update_summary_stats(struct sfe_ipv6 *si)
{
	int i;

	si->connection_create_requests64 += si->connection_create_requests;
	si->connection_create_requests = 0;
	si->connection_create_collisions64 += si->connection_create_collisions;
	si->connection_create_collisions = 0;
	si->connection_destroy_requests64 += si->connection_destroy_requests;
	si->connection_destroy_requests = 0;
	si->connection_destroy_misses64 += si->connection_destroy_misses;
	si->connection_destroy_misses = 0;
	si->connection_match_hash_hits64 += si->connection_match_hash_hits;
	si->connection_match_hash_hits = 0;
	si->connection_match_hash_reorders64 += si->connection_match_hash_reorders;
	si->connection_match_hash_reorders = 0;
	si->connection_flushes64 += si->connection_flushes;
	si->connection_flushes = 0;
	si->packets_forwarded64 += si->packets_forwarded;
	si->packets_forwarded = 0;
	si->packets_not_forwarded64 += si->packets_not_forwarded;
	si->packets_not_forwarded = 0;

	for (i = 0; i < SFE_IPV6_EXCEPTION_EVENT_LAST; i++) {
		si->exception_events64[i] += si->exception_events[i];
		si->exception_events[i] = 0;
	}
}

/*
 * sfe_ipv6_insert_connection_match()
 *	Insert a connection match into the hash.
 *
 * On entry we must be holding the lock that protects the hash table.
 */
static inline void sfe_ipv6_insert_connection_match(struct sfe_ipv6 *si, struct sfe_ipv6_connection_match *cm)
{
	struct sfe_ipv6_connection_match **hash_head;
	struct sfe_ipv6_connection_match *prev_head;
	unsigned int conn_match_idx
		= sfe_ipv6_get_connection_match_hash(cm->match_dev, cm->match_protocol,
				cm->match_src_ip, cm->match_src_port,
				cm->match_dest_ip, cm->match_dest_port);
	hash_head = &si->conn_match_hash[conn_match_idx];
	prev_head = *hash_head;
	cm->prev = NULL;
	if (prev_head) {
		prev_head->prev = cm;
	}

	cm->next = prev_head;
	*hash_head = cm;

#ifdef CONFIG_NF_FLOW_COOKIE
	if (!(cm->flags & (SFE_IPV6_CONNECTION_MATCH_FLAG_XLATE_SRC | SFE_IPV6_CONNECTION_MATCH_FLAG_XLATE_DEST)))
		return;

	/*
	 * Configure hardware to put a flow cookie in packet of this flow,
	 * then we can accelerate the lookup process when we received this packet.
	 */
	for (conn_match_idx = 1; conn_match_idx < SFE_FLOW_COOKIE_SIZE; conn_match_idx++) {
		struct sfe_ipv6_flow_cookie_entry *entry = &si->sfe_flow_cookie_table[conn_match_idx];

		if ((NULL == entry->match) && time_is_before_jiffies(entry->last_clean_time + HZ)) {
			sfe_ipv6_flow_cookie_set_func_t func;

			rcu_read_lock();
			func = rcu_dereference(si->flow_cookie_set_func);
			if (func) {
				if (!func(cm->match_protocol, cm->match_src_ip->addr, cm->match_src_port,
							cm->match_dest_ip->addr, cm->match_dest_port, conn_match_idx)) {
					entry->match = cm;
					cm->flow_cookie = conn_match_idx;
				} else {
					si->exception_events[SFE_IPV6_EXCEPTION_EVENT_FLOW_COOKIE_ADD_FAIL]++;
				}
			}
			rcu_read_unlock();

			break;
		}
	}
#endif

}

/*
 * sfe_ipv6_remove_connection_match()
 *	Remove a connection match object from the hash.
 *
 * On entry we must be holding the lock that protects the hash table.
 */
static inline void sfe_ipv6_remove_connection_match(struct sfe_ipv6 *si, struct sfe_ipv6_connection_match *cm)
{
#ifdef CONFIG_NF_FLOW_COOKIE
	/*
	 * Tell hardware that we no longer need a flow cookie in packet of this flow
	 */
	unsigned int conn_match_idx;

	for (conn_match_idx = 1; conn_match_idx < SFE_FLOW_COOKIE_SIZE; conn_match_idx++) {
		struct sfe_ipv6_flow_cookie_entry *entry = &si->sfe_flow_cookie_table[conn_match_idx];

		if (cm == entry->match) {
			sfe_ipv6_flow_cookie_set_func_t func;

			rcu_read_lock();
			func = rcu_dereference(si->flow_cookie_set_func);
			if (func) {
				func(cm->match_protocol, cm->match_src_ip->addr, cm->match_src_port,
						cm->match_dest_ip->addr, cm->match_dest_port, 0);
			}
			rcu_read_unlock();

			cm->flow_cookie = 0;
			entry->match = NULL;
			entry->last_clean_time = jiffies;
			break;
		}
	}
#endif

	/*
	 * Unlink the connection match entry from the hash.
	 */
	if (cm->prev) {
		cm->prev->next = cm->next;
	} else {
		unsigned int conn_match_idx
			= sfe_ipv6_get_connection_match_hash(cm->match_dev, cm->match_protocol,
					cm->match_src_ip, cm->match_src_port,
					cm->match_dest_ip, cm->match_dest_port);
		si->conn_match_hash[conn_match_idx] = cm->next;
	}

	if (cm->next) {
		cm->next->prev = cm->prev;
	}

	/*
	 * If the connection match entry is in the active list remove it.
	 */
	if (cm->active) {
		if (likely(cm->active_prev)) {
			cm->active_prev->active_next = cm->active_next;
		} else {
			si->active_head = cm->active_next;
		}

		if (likely(cm->active_next)) {
			cm->active_next->active_prev = cm->active_prev;
		} else {
			si->active_tail = cm->active_prev;
		}
	}
}

/*
 * sfe_ipv6_get_connection_hash()
 *	Generate the hash used in connection lookups.
 */
static inline unsigned int sfe_ipv6_get_connection_hash(uint8_t protocol, struct sfe_ipv6_addr *src_ip, __be16 src_port,
		struct sfe_ipv6_addr *dest_ip, __be16 dest_port)
{
	uint32_t idx, hash = 0;

	for (idx = 0; idx < 4; idx++) {
		hash ^= src_ip->addr[idx] ^ dest_ip->addr[idx];
	}
	hash = hash ^ protocol ^ ntohs(src_port ^ dest_port);
	return ((hash >> SFE_IPV6_CONNECTION_HASH_SHIFT) ^ hash) & SFE_IPV6_CONNECTION_HASH_MASK;
}

/*
 * sfe_ipv6_find_connection()
 *	Get the IPv6 connection info that corresponds to a particular 5-tuple.
 *
 * On entry we must be holding the lock that protects the hash table.
 */
static inline struct sfe_ipv6_connection *sfe_ipv6_find_connection(struct sfe_ipv6 *si, uint32_t protocol,
		struct sfe_ipv6_addr *src_ip, __be16 src_port,
		struct sfe_ipv6_addr *dest_ip, __be16 dest_port)
{
	struct sfe_ipv6_connection *c;
	unsigned int conn_idx = sfe_ipv6_get_connection_hash(protocol, src_ip, src_port, dest_ip, dest_port);
	c = si->conn_hash[conn_idx];

	/*
	 * If we don't have anything in this chain then bale.
	 */
	if (unlikely(!c)) {
		return c;
	}

	/*
	 * Hopefully the first entry is the one we want.
	 */
	if (likely(c->src_port == src_port)
			&& likely(c->dest_port == dest_port)
			&& likely(sfe_ipv6_addr_equal(c->src_ip, src_ip))
			&& likely(sfe_ipv6_addr_equal(c->dest_ip, dest_ip))
			&& likely(c->protocol == protocol)) {
		return c;
	}

	/*
	 * We may or may not have a matching entry but if we do then we want to
	 * move that entry to the top of the hash chain when we get to it.  We
	 * presume that this will be reused again very quickly.
	 */
	do {
		c = c->next;
	} while (c && (c->src_port != src_port
				|| c->dest_port != dest_port
				|| !sfe_ipv6_addr_equal(c->src_ip, src_ip)
				|| !sfe_ipv6_addr_equal(c->dest_ip, dest_ip)
				|| c->protocol != protocol));

	/*
	 * Will need connection entry for next create/destroy metadata,
	 * So no need to re-order entry for these requests
	 */
	return c;
}

/*
 * sfe_ipv6_mark_rule()
 *	Updates the mark for a current offloaded connection
 *
 * Will take hash lock upon entry
 */
void sfe_ipv6_mark_rule(struct sfe_connection_mark *mark)
{
	struct sfe_ipv6 *si = &__si6;
	struct sfe_ipv6_connection *c;

	spin_lock_bh(&si->lock);
	c = sfe_ipv6_find_connection(si, mark->protocol,
			mark->src_ip.ip6, mark->src_port,
			mark->dest_ip.ip6, mark->dest_port);
	if (c) {
		DEBUG_TRACE("Matching connection found for mark, "
				"setting from %08x to %08x\n",
				c->mark, mark->mark);
		WARN_ON((0 != c->mark) && (0 == mark->mark));
		c->mark = mark->mark;
	}
	spin_unlock_bh(&si->lock);
}

/*
 * sfe_ipv6_insert_connection()
 *	Insert a connection into the hash.
 *
 * On entry we must be holding the lock that protects the hash table.
 */
static void sfe_ipv6_insert_connection(struct sfe_ipv6 *si, struct sfe_ipv6_connection *c)
{
	struct sfe_ipv6_connection **hash_head;
	struct sfe_ipv6_connection *prev_head;
	unsigned int conn_idx;

	/*
	 * Insert entry into the connection hash.
	 */
	conn_idx = sfe_ipv6_get_connection_hash(c->protocol, c->src_ip, c->src_port,
			c->dest_ip, c->dest_port);
	hash_head = &si->conn_hash[conn_idx];
	prev_head = *hash_head;
	c->prev = NULL;
	if (prev_head) {
		prev_head->prev = c;
	}

	c->next = prev_head;
	*hash_head = c;

	/*
	 * Insert entry into the "all connections" list.
	 */
	if (si->all_connections_tail) {
		c->all_connections_prev = si->all_connections_tail;
		si->all_connections_tail->all_connections_next = c;
	} else {
		c->all_connections_prev = NULL;
		si->all_connections_head = c;
	}

	si->all_connections_tail = c;
	c->all_connections_next = NULL;
	si->num_connections++;

	/*
	 * Insert the connection match objects too.
	 */
	sfe_ipv6_insert_connection_match(si, c->original_match);
	sfe_ipv6_insert_connection_match(si, c->reply_match);
}

/*
 * sfe_ipv6_remove_connection()
 *	Remove a sfe_ipv6_connection object from the hash.
 *
 * On entry we must be holding the lock that protects the hash table.
 */
static void sfe_ipv6_remove_connection(struct sfe_ipv6 *si, struct sfe_ipv6_connection *c)
{
	uint64_t tx_bytes;
	uint64_t rx_bytes;
	struct sfe_ipv6_addr client_ip;
	bool ret;
	/*
	 * Remove the connection match objects.
	 */
	if (packet_stats_enabled)
	{
		DEBUG_INFO("removing connection ipv6\n");

		/*we need to update pack stat list before destroying
		  we can use connection c whihc we are abt to destroy to update
		  packet stat list*/
		if (strncmp(c->original_dev->name, si->ipv6_iface, strlen(si->ipv6_iface) - 1) == 0)
		{
			if (!sfe_ipv6_addr_equal(c->dest_ip, c->dest_ip_xlate))
			{
				client_ip = c->dest_ip_xlate[0];
			}
			else
			{
				client_ip = c->dest_ip[0];
			}
			IPC_DEBUG(
				"Destroyed updating  for %pI6, orig(rx) %d, reply(tx) %d\n",
				&client_ip,
				c->original_match->rx_pack_stat_byte_count,
				c->reply_match->rx_pack_stat_byte_count);
			tx_bytes = c->reply_match->rx_pack_stat_byte_count;
			c->reply_match->rx_pack_stat_byte_count = 0;
			rx_bytes = c->original_match->rx_pack_stat_byte_count;
			c->original_match->rx_pack_stat_byte_count = 0;
		}
		else if (strncmp(c->reply_dev->name, si->ipv6_iface, strlen(si->ipv6_iface) - 1) == 0)
		{
			client_ip = c->src_ip[0];
			IPC_DEBUG(
				"Destroyed updating  for %pI6, orig(tx) %d, reply(rx) %d\n",
				&client_ip,
				c->original_match->rx_pack_stat_byte_count,
				c->reply_match->rx_pack_stat_byte_count);
			rx_bytes = c->reply_match->rx_pack_stat_byte_count;
			c->reply_match->rx_pack_stat_byte_count = 0;
			tx_bytes = c->original_match->rx_pack_stat_byte_count;
			c->original_match->rx_pack_stat_byte_count = 0;
		}
		else
		{
			//the connection is not valid for packet stats
			goto exit1;
		}
		ret = sfe_ipv6_update_packet_stats_connection(si, &client_ip, rx_bytes, tx_bytes );

		if (!ret)
		{
			DEBUG_INFO("pack stat node not found\n");
		}
	}
exit1:

	sfe_ipv6_remove_connection_match(si, c->reply_match);
	sfe_ipv6_remove_connection_match(si, c->original_match);

	/*
	 * Unlink the connection.
	 */
	if (c->prev) {
		c->prev->next = c->next;
	} else {
		unsigned int conn_idx = sfe_ipv6_get_connection_hash(c->protocol, c->src_ip, c->src_port,
				c->dest_ip, c->dest_port);
		si->conn_hash[conn_idx] = c->next;
	}

	if (c->next) {
		c->next->prev = c->prev;
	}

	/*
	 * Unlink connection from all_connections list
	 */
	if (c->all_connections_prev) {
		c->all_connections_prev->all_connections_next = c->all_connections_next;
	} else {
		si->all_connections_head = c->all_connections_next;
	}

	if (c->all_connections_next) {
		c->all_connections_next->all_connections_prev = c->all_connections_prev;
	} else {
		si->all_connections_tail = c->all_connections_prev;
	}

	si->num_connections--;
}

/*
 * sfe_ipv6_gen_sync_connection()
 *	Sync a connection.
 *
 * On entry to this function we expect that the lock for the connection is either
 * already held or isn't required.
 */
static void sfe_ipv6_gen_sync_connection(struct sfe_ipv6 *si, struct sfe_ipv6_connection *c,
		struct sfe_connection_sync *sis, sfe_sync_reason_t reason,
		uint64_t now_jiffies)
{
	struct sfe_ipv6_connection_match *original_cm;
	struct sfe_ipv6_connection_match *reply_cm;

	/*
	 * Fill in the update message.
	 */
	sis->protocol = c->protocol;
	sis->src_ip.ip6[0] = c->src_ip[0];
	sis->src_ip_xlate.ip6[0] = c->src_ip_xlate[0];
	sis->dest_ip.ip6[0] = c->dest_ip[0];
	sis->dest_ip_xlate.ip6[0] = c->dest_ip_xlate[0];
	sis->src_port = c->src_port;
	sis->src_port_xlate = c->src_port_xlate;
	sis->dest_port = c->dest_port;
	sis->dest_port_xlate = c->dest_port_xlate;

	original_cm = c->original_match;
	reply_cm = c->reply_match;
	sis->src_td_max_window = original_cm->protocol_state.tcp.max_win;
	sis->src_td_end = original_cm->protocol_state.tcp.end;
	sis->src_td_max_end = original_cm->protocol_state.tcp.max_end;
	sis->dest_td_max_window = reply_cm->protocol_state.tcp.max_win;
	sis->dest_td_end = reply_cm->protocol_state.tcp.end;
	sis->dest_td_max_end = reply_cm->protocol_state.tcp.max_end;

	sis->src_new_packet_count = original_cm->rx_packet_count;
	sis->src_new_byte_count = original_cm->rx_byte_count;
	sis->dest_new_packet_count = reply_cm->rx_packet_count;
	sis->dest_new_byte_count = reply_cm->rx_byte_count;

	sfe_ipv6_connection_match_update_summary_stats(original_cm);
	sfe_ipv6_connection_match_update_summary_stats(reply_cm);

	sis->src_dev = original_cm->match_dev;
	sis->src_packet_count = original_cm->rx_packet_count64;
	sis->src_byte_count = original_cm->rx_byte_count64;

	sis->dest_dev = reply_cm->match_dev;
	sis->dest_packet_count = reply_cm->rx_packet_count64;
	sis->dest_byte_count = reply_cm->rx_byte_count64;

	sis->reason = reason;

	/*
	 * Get the time increment since our last sync.
	 */
	sis->delta_jiffies = now_jiffies - c->last_sync_jiffies;
	c->last_sync_jiffies = now_jiffies;
}

/*
 * sfe_ipv6_flush_connection()
 *	Flush a connection and free all associated resources.
 *
 * We need to be called with bottom halves disabled locally as we need to acquire
 * the connection hash lock and release it again.  In general we're actually called
 * from within a BH and so we're fine, but we're also called when connections are
 * torn down.
 */
static void sfe_ipv6_flush_connection(struct sfe_ipv6 *si, struct sfe_ipv6_connection *c, sfe_sync_reason_t reason)
{
	struct sfe_connection_sync sis;
	uint64_t now_jiffies;
	sfe_sync_rule_callback_t sync_rule_callback;

	rcu_read_lock();
	spin_lock_bh(&si->lock);
	si->connection_flushes++;
	sync_rule_callback = rcu_dereference(si->sync_rule_callback);
	spin_unlock_bh(&si->lock);

	if (sync_rule_callback) {
		/*
		 * Generate a sync message and then sync.
		 */
		now_jiffies = get_jiffies_64();
		sfe_ipv6_gen_sync_connection(si, c, &sis, reason, now_jiffies);
		sync_rule_callback(&sis);
	}

	rcu_read_unlock();

	/*
	 * Release our hold of the source and dest devices and free the memory
	 * for our connection objects.
	 */
	dev_put(c->original_dev);
	dev_put(c->reply_dev);
	kfree(c->original_match);
	kfree(c->reply_match);
	kfree(c);
}

/*
 * sfe_ipv6_recv_udp()
 *	Handle UDP packet receives and forwarding.
 */
static int sfe_ipv6_recv_udp(struct sfe_ipv6 *si, struct sk_buff *skb, struct net_device *dev,
		unsigned int len, struct sfe_ipv6_ip_hdr *iph, unsigned int ihl, bool flush_on_find)
{
	struct sfe_ipv6_udp_hdr *udph;
	struct sfe_ipv6_addr *src_ip;
	struct sfe_ipv6_addr *dest_ip;
	__be16 src_port;
	__be16 dest_port;
	struct sfe_ipv6_connection_match *cm;
	struct net_device *xmit_dev;
	struct sk_buff *new_skb;
	const struct net_device_ops *ops;
	int queue_index = 0, ret = 0;
	unsigned int skb_trim_len, trim_len = 0;
	struct sfe_ipv6_eth_hdr *eth;
	struct sfe_ipv6_connection *c;

	/*
	 * Is our packet too short to contain a valid UDP header?
	 */
	if (!pskb_may_pull(skb, (sizeof(struct sfe_ipv6_udp_hdr) + ihl))) {
		spin_lock_bh(&si->lock);
		si->exception_events[SFE_IPV6_EXCEPTION_EVENT_UDP_HEADER_INCOMPLETE]++;
		si->packets_not_forwarded++;
		spin_unlock_bh(&si->lock);

		DEBUG_TRACE_LOW("packet too short for UDP header\n");
		return 0;
	}

	/*
	 * Read the IP address and port information.  Read the IP header data first
	 * because we've almost certainly got that in the cache.  We may not yet have
	 * the UDP header cached though so allow more time for any prefetching.
	 */
	src_ip = &iph->saddr;
	dest_ip = &iph->daddr;

	udph = (struct sfe_ipv6_udp_hdr *)(skb->data + ihl);
	src_port = udph->source;
	dest_port = udph->dest;

	spin_lock_bh(&si->lock);

	/*
	 * Look for a connection match.
	 */
#ifdef CONFIG_NF_FLOW_COOKIE
	cm = si->sfe_flow_cookie_table[skb->flow_cookie & SFE_FLOW_COOKIE_MASK].match;
	if (unlikely(!cm)) {
		cm = sfe_ipv6_find_connection_match(si, dev, IPPROTO_UDP, src_ip, src_port, dest_ip, dest_port);
	}
#else
	cm = sfe_ipv6_find_connection_match(si, dev, IPPROTO_UDP, src_ip, src_port, dest_ip, dest_port);
#endif
	if (unlikely(!cm)) {
		si->exception_events[SFE_IPV6_EXCEPTION_EVENT_UDP_NO_CONNECTION]++;
		si->packets_not_forwarded++;
		spin_unlock_bh(&si->lock);

		DEBUG_TRACE_LOW("no connection found\n");
		return 0;
	}

	/*
	 * If our packet has beern marked as "flush on find" we can't actually
	 * forward it in the fast path, but now that we've found an associated
	 * connection we can flush that out before we process the packet.
	 */
	if (unlikely(flush_on_find)) {
		struct sfe_ipv6_connection *c = cm->connection;
		sfe_ipv6_remove_connection(si, c);
		si->exception_events[SFE_IPV6_EXCEPTION_EVENT_UDP_IP_OPTIONS_OR_INITIAL_FRAGMENT]++;
		si->packets_not_forwarded++;
		spin_unlock_bh(&si->lock);

		DEBUG_TRACE_LOW("flush on find\n");
		sfe_ipv6_flush_connection(si, c, SFE_SYNC_REASON_FLUSH);
		return 0;
	}

#ifdef CONFIG_XFRM
	/*
	 * We can't accelerate the flow on this direction, just let it go
	 * through the slow path.
	 */
	if (unlikely(!cm->flow_accel)) {
		si->packets_not_forwarded++;
		spin_unlock_bh(&si->lock);
		return 0;
	}
#endif

	/*
	 * Does our hop_limit allow forwarding?
	 */
	if (unlikely(iph->hop_limit < 2)) {
		struct sfe_ipv6_connection *c = cm->connection;
		sfe_ipv6_remove_connection(si, c);
		si->exception_events[SFE_IPV6_EXCEPTION_EVENT_UDP_SMALL_TTL]++;
		si->packets_not_forwarded++;
		spin_unlock_bh(&si->lock);

		DEBUG_TRACE_LOW("hop_limit too low\n");
		sfe_ipv6_flush_connection(si, c, SFE_SYNC_REASON_FLUSH);
		return 0;
	}

	/*
	 * If our packet is larger than the MTU of the transmit interface then
	 * we can't forward it easily.
	 */
	if (unlikely(len > cm->xmit_dev_mtu)) {
		struct sfe_ipv6_connection *c = cm->connection;
		sfe_ipv6_remove_connection(si, c);
		si->exception_events[SFE_IPV6_EXCEPTION_EVENT_UDP_NEEDS_FRAGMENTATION]++;
		si->packets_not_forwarded++;
		spin_unlock_bh(&si->lock);

		DEBUG_TRACE_LOW("larger than mtu\n");
		sfe_ipv6_flush_connection(si, c, SFE_SYNC_REASON_FLUSH);
		return 0;
	}

	/*
	 * From this point on we're good to modify the packet.
	 */

	/*
	 * Decrement our hop_limit.
	 */
	iph->hop_limit -= 1;

	/*
	 * Do we have to perform translations of the source address/port?
	 */
	if (unlikely(cm->flags & SFE_IPV6_CONNECTION_MATCH_FLAG_XLATE_SRC)) {
		uint16_t udp_csum;

		iph->saddr = cm->xlate_src_ip[0];
		udph->source = cm->xlate_src_port;

		/*
		 * Do we have a non-zero UDP checksum?  If we do then we need
		 * to update it.
		 */
		udp_csum = udph->check;
		if (likely(udp_csum)) {
			uint32_t sum = udp_csum + cm->xlate_src_csum_adjustment;
			sum = (sum & 0xffff) + (sum >> 16);
			udph->check = (uint16_t)sum;
		}
	}

	/*
	 * Do we have to perform translations of the destination address/port?
	 */
	if (unlikely(cm->flags & SFE_IPV6_CONNECTION_MATCH_FLAG_XLATE_DEST)) {
		uint16_t udp_csum;

		iph->daddr = cm->xlate_dest_ip[0];
		udph->dest = cm->xlate_dest_port;

		/*
		 * Do we have a non-zero UDP checksum?  If we do then we need
		 * to update it.
		 */
		udp_csum = udph->check;
		if (likely(udp_csum)) {
			uint32_t sum = udp_csum + cm->xlate_dest_csum_adjustment;
			sum = (sum & 0xffff) + (sum >> 16);
			udph->check = (uint16_t)sum;
		}
	}

	/*
	 * Update traffic stats.
	 */
	cm->rx_packet_count++;
	cm->rx_byte_count += len;
	if (packet_stats_enabled)
	{
		cm->rx_pack_stat_byte_count += len;
	}

	/*
	 * If we're not already on the active list then insert ourselves at the tail
	 * of the current list.
	 */
	if (unlikely(!cm->active)) {
		cm->active = true;
		cm->active_prev = si->active_tail;
		if (likely(si->active_tail)) {
			si->active_tail->active_next = cm;
		} else {
			si->active_head = cm;
		}
		si->active_tail = cm;
	}

	xmit_dev = cm->xmit_dev;
	skb->dev = xmit_dev;
	c = cm->connection;
	/*
	 * Check to see if we need to write a header.
	 */
	if (likely(c->use_destMac || cm->addEthMAC)) {
		if (likely(cm->flags & SFE_IPV6_CONNECTION_MATCH_FLAG_WRITE_L2_HDR)) {
			if (unlikely(!(cm->flags & SFE_IPV6_CONNECTION_MATCH_FLAG_WRITE_FAST_ETH_HDR))) {
				if (skb_headroom(skb) <
					xmit_dev->hard_header_len) {
					ret = pskb_expand_head(skb,
								HH_DATA_ALIGN(
						xmit_dev->hard_header_len -
						skb_headroom(skb)),
						0, GFP_ATOMIC);
					if (ret) {
						kfree_skb(skb);
						IPC_DEBUG_LOW(
							"pskb_expand_head failed = %d",
							ret);
						return 0;
					}
				}
				dev_hard_header(skb, xmit_dev, ETH_P_IPV6,
						cm->xmit_dest_mac, cm->xmit_src_mac, len);
				trim_len = xmit_dev->hard_header_len;
			} else {
				/*
				 * For the simple case we write this really fast.
				 */
				if (skb_headroom(skb) <
					xmit_dev->hard_header_len)
					ret = pskb_expand_head(skb, ETH_HLEN, 0,
							GFP_ATOMIC);
					if (ret) {
						kfree_skb(skb);
						IPC_DEBUG_LOW(
							"pskb_expand_head failed = %d",
							ret);
						return 0;
					}

				eth = (struct sfe_ipv6_eth_hdr *)__skb_push(skb, ETH_HLEN);
				eth->h_proto = htons(ETH_P_IPV6);
				eth->h_dest[0] = cm->xmit_dest_mac[0];
				eth->h_dest[1] = cm->xmit_dest_mac[1];
				eth->h_dest[2] = cm->xmit_dest_mac[2];
				eth->h_source[0] = cm->xmit_src_mac[0];
				eth->h_source[1] = cm->xmit_src_mac[1];
				eth->h_source[2] = cm->xmit_src_mac[2];
				trim_len = ETH_HLEN;
			}
		}
	}

#ifdef SFE_CONFIG_MARK
	/*
	 * Mark outgoing packet.
	 */
	skb->mark = cm->connection->mark;
	if (skb->mark) {
		DEBUG_TRACE_LOW("SKB MARK is NON ZERO %x\n", skb->mark);
	}
#endif

	si->packets_forwarded++;
	spin_unlock_bh(&si->lock);

	/*
	 * We're going to check for GSO flags when we transmit the packet so
	 * start fetching the necessary cache line now.
	 */
	prefetch(skb_shinfo(skb));

	IPC_DEBUG_LOW("UDP_v6-Uplink. No Aggregation. ");
	if (cm->pad_removal_require) {
		skb_trim_len = ntohs(iph->payload_len) +
						sizeof(struct sfe_ipv6_ip_hdr);
		if (pskb_trim_rcsum(skb, skb_trim_len +
			trim_len)) {
			DEBUG_TRACE_LOW("padding removal failed\n");
		}
	}
	dev_queue_xmit(skb);
	return 1;
}

/*
 * sfe_ipv6_process_tcp_option_sack()
 *	Parse TCP SACK option and update ack according
 */
static bool sfe_ipv6_process_tcp_option_sack(const struct sfe_ipv6_tcp_hdr *th, const uint32_t data_offs,
		uint32_t *ack)
{
	uint32_t length = sizeof(struct sfe_ipv6_tcp_hdr);
	uint8_t *ptr = (uint8_t *)th + length;

	/*
	 * If option is TIMESTAMP discard it.
	 */
	if (likely(data_offs == length + TCPOLEN_TIMESTAMP + 1 + 1)
			&& likely(ptr[0] == TCPOPT_NOP)
			&& likely(ptr[1] == TCPOPT_NOP)
			&& likely(ptr[2] == TCPOPT_TIMESTAMP)
			&& likely(ptr[3] == TCPOLEN_TIMESTAMP)) {
		return true;
	}

	/*
	 * TCP options. Parse SACK option.
	 */
	while (length < data_offs) {
		uint8_t size;
		uint8_t kind;

		ptr = (uint8_t *)th + length;
		kind = *ptr;

		/*
		 * NOP, for padding
		 * Not in the switch because to fast escape and to not calculate size
		 */
		if (kind == TCPOPT_NOP) {
			length++;
			continue;
		}

		if (kind == TCPOPT_SACK) {
			uint32_t sack = 0;
			uint8_t re = 1 + 1;

			size = *(ptr + 1);
			if ((size < (1 + 1 + TCPOLEN_SACK_PERBLOCK))
					|| ((size - (1 + 1)) % (TCPOLEN_SACK_PERBLOCK))
					|| (size > (data_offs - length))) {
				return false;
			}

			re += 4;
			while (re < size) {
				uint32_t sack_re;
				uint8_t *sptr = ptr + re;
				sack_re = (sptr[0] << 24) | (sptr[1] << 16) | (sptr[2] << 8) | sptr[3];
				if (sack_re > sack) {
					sack = sack_re;
				}
				re += TCPOLEN_SACK_PERBLOCK;
			}
			if (sack > *ack) {
				*ack = sack;
			}
			length += size;
			continue;
		}
		if (kind == TCPOPT_EOL) {
			return true;
		}
		size = *(ptr + 1);
		if (size < 2) {
			return false;
		}
		length += size;
	}

	return true;
}

/*
 * sfe_ipv6_recv_tcp()
 *	Handle TCP packet receives and forwarding.
 */
static int sfe_ipv6_recv_tcp(struct sfe_ipv6 *si, struct sk_buff *skb, struct net_device *dev,
		unsigned int len, struct sfe_ipv6_ip_hdr *iph, unsigned int ihl, bool flush_on_find)
{
	struct sfe_ipv6_tcp_hdr *tcph;
	struct sfe_ipv6_addr *src_ip;
	struct sfe_ipv6_addr *dest_ip;
	__be16 src_port;
	__be16 dest_port;
	struct sfe_ipv6_connection_match *cm;
	struct sfe_ipv6_connection_match *counter_cm;
	uint32_t flags;
	struct net_device *xmit_dev;
	struct sk_buff *new_skb ;
	const struct net_device_ops *ops;
	int queue_index = 0, ret = 0;
	struct sfe_ipv6_eth_hdr *eth;
	struct sfe_ipv6_connection *c;
	uint32_t data_offs;
	unsigned int trim_len = 0;

	/*
	 * Is our packet too short to contain a valid UDP header?
	 */
	if (!pskb_may_pull(skb, (sizeof(struct sfe_ipv6_tcp_hdr) + ihl))) {
		spin_lock_bh(&si->lock);
		si->exception_events[SFE_IPV6_EXCEPTION_EVENT_TCP_HEADER_INCOMPLETE]++;
		si->packets_not_forwarded++;
		spin_unlock_bh(&si->lock);

		DEBUG_TRACE_LOW("packet too short for TCP header\n");
		return 0;
	}

	/*
	 * Read the IP address and port information.  Read the IP header data first
	 * because we've almost certainly got that in the cache.  We may not yet have
	 * the TCP header cached though so allow more time for any prefetching.
	 */
	src_ip = &iph->saddr;
	dest_ip = &iph->daddr;

	tcph = (struct sfe_ipv6_tcp_hdr *)(skb->data + ihl);
	src_port = tcph->source;
	dest_port = tcph->dest;
	flags = tcp_flag_word(tcph);

	spin_lock_bh(&si->lock);

	/*
	 * Look for a connection match.
	 */
#ifdef CONFIG_NF_FLOW_COOKIE
	cm = si->sfe_flow_cookie_table[skb->flow_cookie & SFE_FLOW_COOKIE_MASK].match;
	if (unlikely(!cm)) {
		cm = sfe_ipv6_find_connection_match(si, dev, IPPROTO_TCP, src_ip, src_port, dest_ip, dest_port);
	}
#else
	cm = sfe_ipv6_find_connection_match(si, dev, IPPROTO_TCP, src_ip, src_port, dest_ip, dest_port);
#endif
	if (unlikely(!cm)) {
		/*
		 * We didn't get a connection but as TCP is connection-oriented that
		 * may be because this is a non-fast connection (not running established).
		 * For diagnostic purposes we differentiate this here.
		 */
		if (likely((flags & (TCP_FLAG_SYN | TCP_FLAG_RST | TCP_FLAG_FIN | TCP_FLAG_ACK)) == TCP_FLAG_ACK)) {
			si->exception_events[SFE_IPV6_EXCEPTION_EVENT_TCP_NO_CONNECTION_FAST_FLAGS]++;
			si->packets_not_forwarded++;
			spin_unlock_bh(&si->lock);

			DEBUG_TRACE_LOW("no connection found - fast flags\n");
			return 0;
		}
		si->exception_events[SFE_IPV6_EXCEPTION_EVENT_TCP_NO_CONNECTION_SLOW_FLAGS]++;
		si->packets_not_forwarded++;
		spin_unlock_bh(&si->lock);

		DEBUG_TRACE_LOW("no connection found - slow flags: 0x%x\n",
				flags & (TCP_FLAG_SYN | TCP_FLAG_RST | TCP_FLAG_FIN | TCP_FLAG_ACK));
		return 0;
	}

	/*
	 * If our packet has beern marked as "flush on find" we can't actually
	 * forward it in the fast path, but now that we've found an associated
	 * connection we can flush that out before we process the packet.
	 */
	if (unlikely(flush_on_find)) {
		struct sfe_ipv6_connection *c = cm->connection;
		sfe_ipv6_remove_connection(si, c);
		si->exception_events[SFE_IPV6_EXCEPTION_EVENT_TCP_IP_OPTIONS_OR_INITIAL_FRAGMENT]++;
		si->packets_not_forwarded++;
		spin_unlock_bh(&si->lock);

		DEBUG_TRACE_LOW("flush on find\n");
		sfe_ipv6_flush_connection(si, c, SFE_SYNC_REASON_FLUSH);
		return 0;
	}

#ifdef CONFIG_XFRM
	/*
	 * We can't accelerate the flow on this direction, just let it go
	 * through the slow path.
	 */
	if (unlikely(!cm->flow_accel)) {
		si->packets_not_forwarded++;
		spin_unlock_bh(&si->lock);
		return 0;
	}
#endif

	/*
	 * Does our hop_limit allow forwarding?
	 */
	if (unlikely(iph->hop_limit < 2)) {
		struct sfe_ipv6_connection *c = cm->connection;
		sfe_ipv6_remove_connection(si, c);
		si->exception_events[SFE_IPV6_EXCEPTION_EVENT_TCP_SMALL_TTL]++;
		si->packets_not_forwarded++;
		spin_unlock_bh(&si->lock);

		DEBUG_TRACE_LOW("hop_limit too low\n");
		sfe_ipv6_flush_connection(si, c, SFE_SYNC_REASON_FLUSH);
		return 0;
	}

	/*
	 * If our packet is larger than the MTU of the transmit interface then
	 * we can't forward it easily.
	 */
	if (unlikely((len > cm->xmit_dev_mtu) && !skb_is_gso(skb))) {
		struct sfe_ipv6_connection *c = cm->connection;
		sfe_ipv6_remove_connection(si, c);
		si->exception_events[SFE_IPV6_EXCEPTION_EVENT_TCP_NEEDS_FRAGMENTATION]++;
		si->packets_not_forwarded++;
		spin_unlock_bh(&si->lock);

		DEBUG_TRACE_LOW("larger than mtu\n");
		sfe_ipv6_flush_connection(si, c, SFE_SYNC_REASON_FLUSH);
		return 0;
	}

	/*
	 * Look at our TCP flags.  Anything missing an ACK or that has RST, SYN or FIN
	 * set is not a fast path packet.
	 */
	if (unlikely((flags & (TCP_FLAG_SYN | TCP_FLAG_RST | TCP_FLAG_FIN | TCP_FLAG_ACK)) != TCP_FLAG_ACK)) {
		struct sfe_ipv6_connection *c = cm->connection;
		sfe_ipv6_remove_connection(si, c);
		si->exception_events[SFE_IPV6_EXCEPTION_EVENT_TCP_FLAGS]++;
		si->packets_not_forwarded++;
		spin_unlock_bh(&si->lock);

		DEBUG_TRACE_LOW("TCP flags: 0x%x are not fast\n",
				flags & (TCP_FLAG_SYN | TCP_FLAG_RST | TCP_FLAG_FIN | TCP_FLAG_ACK));
		sfe_ipv6_flush_connection(si, c, SFE_SYNC_REASON_FLUSH);
		return 0;
	}

	counter_cm = cm->counter_match;

	/*
	 * Are we doing sequence number checking?
	 */
	if (unlikely(!(cm->flags & SFE_IPV6_CONNECTION_MATCH_FLAG_NO_SEQ_CHECK))) {
		uint32_t seq;
		uint32_t ack;
		uint32_t sack;
		uint32_t data_offs;
		uint32_t end;
		uint32_t left_edge;
		uint32_t scaled_win;
		uint32_t max_end;

		/*
		 * Is our sequence fully past the right hand edge of the window?
		 */
		seq = ntohl(tcph->seq);
		if (unlikely((int32_t)(seq - (cm->protocol_state.tcp.max_end + 1)) > 0)) {
			struct sfe_ipv6_connection *c = cm->connection;
			sfe_ipv6_remove_connection(si, c);
			si->exception_events[SFE_IPV6_EXCEPTION_EVENT_TCP_SEQ_EXCEEDS_RIGHT_EDGE]++;
			si->packets_not_forwarded++;
			spin_unlock_bh(&si->lock);

			DEBUG_TRACE_LOW("seq: %u exceeds right edge: %u\n",
					seq, cm->protocol_state.tcp.max_end + 1);
			sfe_ipv6_flush_connection(si, c, SFE_SYNC_REASON_FLUSH);
			return 0;
		}

		/*
		 * Check that our TCP data offset isn't too short.
		 */
		data_offs = tcph->doff << 2;
		if (unlikely(data_offs < sizeof(struct sfe_ipv6_tcp_hdr))) {
			struct sfe_ipv6_connection *c = cm->connection;
			sfe_ipv6_remove_connection(si, c);
			si->exception_events[SFE_IPV6_EXCEPTION_EVENT_TCP_SMALL_DATA_OFFS]++;
			si->packets_not_forwarded++;
			spin_unlock_bh(&si->lock);

			DEBUG_TRACE_LOW("TCP data offset: %u, too small\n",
				data_offs);
			sfe_ipv6_flush_connection(si, c, SFE_SYNC_REASON_FLUSH);
			return 0;
		}

		/*
		 * Update ACK according to any SACK option.
		 */
		ack = ntohl(tcph->ack_seq);
		sack = ack;
		if (unlikely(!sfe_ipv6_process_tcp_option_sack(tcph, data_offs, &sack))) {
			struct sfe_ipv6_connection *c = cm->connection;
			sfe_ipv6_remove_connection(si, c);
			si->exception_events[SFE_IPV6_EXCEPTION_EVENT_TCP_BAD_SACK]++;
			si->packets_not_forwarded++;
			spin_unlock_bh(&si->lock);

			DEBUG_TRACE_LOW("TCP option SACK size is wrong\n");
			sfe_ipv6_flush_connection(si, c, SFE_SYNC_REASON_FLUSH);
			return 0;
		}

		/*
		 * Check that our TCP data offset isn't past the end of the packet.
		 */
		data_offs += sizeof(struct sfe_ipv6_ip_hdr);
		if (unlikely(len < data_offs)) {
			struct sfe_ipv6_connection *c = cm->connection;
			sfe_ipv6_remove_connection(si, c);
			si->exception_events[SFE_IPV6_EXCEPTION_EVENT_TCP_BIG_DATA_OFFS]++;
			si->packets_not_forwarded++;
			spin_unlock_bh(&si->lock);

			DEBUG_TRACE_LOW(
				"TCP data offset: %u, past end of packet: %u\n",
				data_offs, len);
			sfe_ipv6_flush_connection(si, c, SFE_SYNC_REASON_FLUSH);
			return 0;
		}

		end = seq + len - data_offs;

		/*
		 * Is our sequence fully before the left hand edge of the window?
		 */
		if (unlikely((int32_t)(end - (cm->protocol_state.tcp.end
							- counter_cm->protocol_state.tcp.max_win - 1)) < 0)) {
			struct sfe_ipv6_connection *c = cm->connection;
			sfe_ipv6_remove_connection(si, c);
			si->exception_events[SFE_IPV6_EXCEPTION_EVENT_TCP_SEQ_BEFORE_LEFT_EDGE]++;
			si->packets_not_forwarded++;
			spin_unlock_bh(&si->lock);

			DEBUG_TRACE_LOW("seq: %u before left edge: %u\n",
					end, cm->protocol_state.tcp.end - counter_cm->protocol_state.tcp.max_win - 1);
			sfe_ipv6_flush_connection(si, c, SFE_SYNC_REASON_FLUSH);
			return 0;
		}

		/*
		 * Are we acking data that is to the right of what has been sent?
		 */
		if (unlikely((int32_t)(sack - (counter_cm->protocol_state.tcp.end + 1)) > 0)) {
			struct sfe_ipv6_connection *c = cm->connection;
			sfe_ipv6_remove_connection(si, c);
			si->exception_events[SFE_IPV6_EXCEPTION_EVENT_TCP_ACK_EXCEEDS_RIGHT_EDGE]++;
			si->packets_not_forwarded++;
			spin_unlock_bh(&si->lock);

			DEBUG_TRACE("ack: %u exceeds right edge: %u\n",
					sack, counter_cm->protocol_state.tcp.end + 1);
			sfe_ipv6_flush_connection(si, c, SFE_SYNC_REASON_FLUSH);
			return 0;
		}

		/*
		 * Is our ack too far before the left hand edge of the window?
		 */
		left_edge = counter_cm->protocol_state.tcp.end
			- cm->protocol_state.tcp.max_win
			- SFE_IPV6_TCP_MAX_ACK_WINDOW
			- 1;
		if (unlikely((int32_t)(sack - left_edge) < 0)) {
			struct sfe_ipv6_connection *c = cm->connection;
			sfe_ipv6_remove_connection(si, c);
			si->exception_events[SFE_IPV6_EXCEPTION_EVENT_TCP_ACK_BEFORE_LEFT_EDGE]++;
			si->packets_not_forwarded++;
			spin_unlock_bh(&si->lock);

			DEBUG_TRACE_LOW("ack: %u before left edge: %u\n",
				sack, left_edge);
			sfe_ipv6_flush_connection(si, c, SFE_SYNC_REASON_FLUSH);
			return 0;
		}

		/*
		 * Have we just seen the largest window size yet for this connection?  If yes
		 * then we need to record the new value.
		 */
		scaled_win = ntohs(tcph->window) << cm->protocol_state.tcp.win_scale;
		scaled_win += (sack - ack);
		if (unlikely(cm->protocol_state.tcp.max_win < scaled_win)) {
			cm->protocol_state.tcp.max_win = scaled_win;
		}

		/*
		 * If our sequence and/or ack numbers have advanced then record the new state.
		 */
		if (likely((int32_t)(end - cm->protocol_state.tcp.end) >= 0)) {
			cm->protocol_state.tcp.end = end;
		}

		max_end = sack + scaled_win;
		if (likely((int32_t)(max_end - counter_cm->protocol_state.tcp.max_end) >= 0)) {
			counter_cm->protocol_state.tcp.max_end = max_end;
		}
	}

	/*
	 * From this point on we're good to modify the packet.
	 */

	/*
	 * Decrement our hop_limit.
	 */
	iph->hop_limit -= 1;

	/*
	 * Do we have to perform translations of the source address/port?
	 */
	if (unlikely(cm->flags & SFE_IPV6_CONNECTION_MATCH_FLAG_XLATE_SRC)) {
		uint16_t tcp_csum;
		uint32_t sum;

		iph->saddr = cm->xlate_src_ip[0];
		tcph->source = cm->xlate_src_port;

		/*
		 * Do we have a non-zero UDP checksum?  If we do then we need
		 * to update it.
		 */
		tcp_csum = tcph->check;
		sum = tcp_csum + cm->xlate_src_csum_adjustment;
		sum = (sum & 0xffff) + (sum >> 16);
		tcph->check = (uint16_t)sum;
	}

	/*
	 * Do we have to perform translations of the destination address/port?
	 */
	if (unlikely(cm->flags & SFE_IPV6_CONNECTION_MATCH_FLAG_XLATE_DEST)) {
		uint16_t tcp_csum;
		uint32_t sum;

		iph->daddr = cm->xlate_dest_ip[0];
		tcph->dest = cm->xlate_dest_port;

		/*
		 * Do we have a non-zero UDP checksum?  If we do then we need
		 * to update it.
		 */
		tcp_csum = tcph->check;
		sum = tcp_csum + cm->xlate_dest_csum_adjustment;
		sum = (sum & 0xffff) + (sum >> 16);
		tcph->check = (uint16_t)sum;
	}

	/*
	 * Update traffic stats.
	 */
	cm->rx_packet_count++;
	cm->rx_byte_count += len;
	if (packet_stats_enabled)
	{
		cm->rx_pack_stat_byte_count += len;
	}
	/*
	 * If we're not already on the active list then insert ourselves at the tail
	 * of the current list.
	 */
	if (unlikely(!cm->active)) {
		cm->active = true;
		cm->active_prev = si->active_tail;
		if (likely(si->active_tail)) {
			si->active_tail->active_next = cm;
		} else {
			si->active_head = cm;
		}
		si->active_tail = cm;
	}

	xmit_dev = cm->xmit_dev;
	skb->dev = xmit_dev;
	c = cm->connection;
	/*
	 * Check to see if we need to write a header.
	 */
	if (likely(c->use_destMac || cm->addEthMAC)) {
		if (likely(cm->flags & SFE_IPV6_CONNECTION_MATCH_FLAG_WRITE_L2_HDR)) {
			if (unlikely(!(cm->flags & SFE_IPV6_CONNECTION_MATCH_FLAG_WRITE_FAST_ETH_HDR))) {
				if (skb_headroom(skb) <
					xmit_dev->hard_header_len) {
					ret = pskb_expand_head(skb ,
								HH_DATA_ALIGN(
						xmit_dev->hard_header_len -
						skb_headroom(skb)) ,
						0, GFP_ATOMIC);
					if (ret) {
						kfree_skb(skb);
						IPC_DEBUG_LOW(
							"pskb_expand_head failed = %d",
							ret);
						return 0;
					}
				}
				dev_hard_header(skb, xmit_dev, ETH_P_IPV6,
						cm->xmit_dest_mac, cm->xmit_src_mac, len);
				trim_len = xmit_dev->hard_header_len;
			} else {
				/*
				 * For the simple case we write this really fast.
				 */
				if (skb_headroom(skb) <
					xmit_dev->hard_header_len)
					ret = pskb_expand_head(skb, ETH_HLEN, 0,
							GFP_ATOMIC);
					if (ret) {
						kfree_skb(skb);
						IPC_DEBUG_LOW(
							"pskb_expand_head failed = %d",
							ret);
						return 0;
					}

				eth = (struct sfe_ipv6_eth_hdr *)__skb_push(skb, ETH_HLEN);
				eth->h_proto = htons(ETH_P_IPV6);
				eth->h_dest[0] = cm->xmit_dest_mac[0];
				eth->h_dest[1] = cm->xmit_dest_mac[1];
				eth->h_dest[2] = cm->xmit_dest_mac[2];
				eth->h_source[0] = cm->xmit_src_mac[0];
				eth->h_source[1] = cm->xmit_src_mac[1];
				eth->h_source[2] = cm->xmit_src_mac[2];
				trim_len = ETH_HLEN;
			}
		}
	}

#ifdef SFE_CONFIG_MARK
	/*
	 * Mark outgoing packet
	 */
	skb->mark = cm->connection->mark;
	if (skb->mark) {
		DEBUG_TRACE_LOW("SKB MARK is NON ZERO %x\n", skb->mark);
	}
#endif

	si->packets_forwarded++;
	spin_unlock_bh(&si->lock);

	/*
	 * We're going to check for GSO flags when we transmit the packet so
	 * start fetching the necessary cache line now.
	 */
	prefetch(skb_shinfo(skb));

	/*
	 * Send the packet on its way.
	 */

	IPC_DEBUG_LOW("TCP_v6-UpLink. No Aggregation. ");
	if (cm->pad_removal_require) {
		if (pskb_trim_rcsum(skb, ntohs(iph->payload_len)+
			sizeof(struct sfe_ipv6_ip_hdr)+trim_len))
			DEBUG_TRACE_LOW("padding removal failed\n");
	}
	dev_queue_xmit(skb);
	return 1;
}

/*
 * sfe_ipv6_recv_icmp()
 *	Handle ICMP packet receives.
 *
 * ICMP packets aren't handled as a "fast path" and always have us process them
 * through the default Linux stack.  What we do need to do is look for any errors
 * about connections we are handling in the fast path.  If we find any such
 * connections then we want to flush their state so that the ICMP error path
 * within Linux has all of the correct state should it need it.
 */
static int sfe_ipv6_recv_icmp(struct sfe_ipv6 *si, struct sk_buff *skb, struct net_device *dev,
		unsigned int len, struct sfe_ipv6_ip_hdr *iph, unsigned int ihl)
{
	struct icmp6hdr *icmph;
	struct sfe_ipv6_ip_hdr *icmp_iph;
	struct sfe_ipv6_udp_hdr *icmp_udph;
	struct sfe_ipv6_tcp_hdr *icmp_tcph;
	struct sfe_ipv6_addr *src_ip;
	struct sfe_ipv6_addr *dest_ip;
	__be16 src_port;
	__be16 dest_port;
	struct sfe_ipv6_connection_match *cm;
	struct sfe_ipv6_connection *c;
	uint8_t next_hdr;

	/*
	 * Is our packet too short to contain a valid UDP header?
	 */
	len -= ihl;
	if (!pskb_may_pull(skb, ihl + sizeof(struct icmp6hdr))) {
		spin_lock_bh(&si->lock);
		si->exception_events[SFE_IPV6_EXCEPTION_EVENT_ICMP_HEADER_INCOMPLETE]++;
		si->packets_not_forwarded++;
		spin_unlock_bh(&si->lock);

		DEBUG_TRACE_LOW("packet too short for ICMP header\n");
		return 0;
	}

	/*
	 * We only handle "destination unreachable" and "time exceeded" messages.
	 */
	icmph = (struct icmp6hdr *)(skb->data + ihl);
	if ((icmph->icmp6_type != ICMPV6_DEST_UNREACH)
			&& (icmph->icmp6_type != ICMPV6_TIME_EXCEED)) {
		spin_lock_bh(&si->lock);
		si->exception_events[SFE_IPV6_EXCEPTION_EVENT_ICMP_UNHANDLED_TYPE]++;
		si->packets_not_forwarded++;
		spin_unlock_bh(&si->lock);

		DEBUG_TRACE_LOW("unhandled ICMP type: 0x%x\n",
			icmph->icmp6_type);
		return 0;
	}

	/*
	 * Do we have the full embedded IP header?
	 * We should have 8 bytes of next L4 header - that's enough to identify
	 * the connection.
	 */
	len -= sizeof(struct icmp6hdr);
	ihl += sizeof(struct icmp6hdr);
	if (!pskb_may_pull(skb, ihl + sizeof(struct sfe_ipv6_ip_hdr) + sizeof(struct sfe_ipv6_ext_hdr))) {
		spin_lock_bh(&si->lock);
		si->exception_events[SFE_IPV6_EXCEPTION_EVENT_ICMP_IPV6_HEADER_INCOMPLETE]++;
		si->packets_not_forwarded++;
		spin_unlock_bh(&si->lock);

		DEBUG_TRACE_LOW("Embedded IP header not complete\n");
		return 0;
	}

	/*
	 * Is our embedded IP version wrong?
	 */
	icmp_iph = (struct sfe_ipv6_ip_hdr *)(icmph + 1);
	if (unlikely(icmp_iph->version != 6)) {
		spin_lock_bh(&si->lock);
		si->exception_events[SFE_IPV6_EXCEPTION_EVENT_ICMP_IPV6_NON_V6]++;
		si->packets_not_forwarded++;
		spin_unlock_bh(&si->lock);

		DEBUG_TRACE_LOW("IP version: %u\n", icmp_iph->version);
		return 0;
	}

	len -= sizeof(struct sfe_ipv6_ip_hdr);
	ihl += sizeof(struct sfe_ipv6_ip_hdr);
	next_hdr = icmp_iph->nexthdr;
	while (unlikely(sfe_ipv6_is_ext_hdr(next_hdr))) {
		struct sfe_ipv6_ext_hdr *ext_hdr;
		unsigned int ext_hdr_len;

		ext_hdr = (struct sfe_ipv6_ext_hdr *)(skb->data + ihl);
		if (next_hdr == SFE_IPV6_EXT_HDR_FRAG) {
			struct sfe_ipv6_frag_hdr *frag_hdr = (struct sfe_ipv6_frag_hdr *)ext_hdr;
			unsigned int frag_off = ntohs(frag_hdr->frag_off);

			if (frag_off & SFE_IPV6_FRAG_OFFSET) {
				spin_lock_bh(&si->lock);
				si->exception_events[SFE_IPV6_EXCEPTION_EVENT_NON_INITIAL_FRAGMENT]++;
				si->packets_not_forwarded++;
				spin_unlock_bh(&si->lock);

				DEBUG_TRACE_LOW("non-initial fragment\n");
				return 0;
			}
		}

		ext_hdr_len = ext_hdr->hdr_len;
		ext_hdr_len <<= 3;
		ext_hdr_len += sizeof(struct sfe_ipv6_ext_hdr);
		len -= ext_hdr_len;
		ihl += ext_hdr_len;
		/*
		 * We should have 8 bytes of next header - that's enough to identify
		 * the connection.
		 */
		if (!pskb_may_pull(skb, ihl + sizeof(struct sfe_ipv6_ext_hdr))) {
			spin_lock_bh(&si->lock);
			si->exception_events[SFE_IPV6_EXCEPTION_EVENT_HEADER_INCOMPLETE]++;
			si->packets_not_forwarded++;
			spin_unlock_bh(&si->lock);

			DEBUG_TRACE_LOW("extension header %d not completed\n",
				next_hdr);
			return 0;
		}

		next_hdr = ext_hdr->next_hdr;
	}

	/*
	 * Handle the embedded transport layer header.
	 */
	switch (next_hdr) {
		case IPPROTO_UDP:
			icmp_udph = (struct sfe_ipv6_udp_hdr *)(skb->data + ihl);
			src_port = icmp_udph->source;
			dest_port = icmp_udph->dest;
			break;

		case IPPROTO_TCP:
			icmp_tcph = (struct sfe_ipv6_tcp_hdr *)(skb->data + ihl);
			src_port = icmp_tcph->source;
			dest_port = icmp_tcph->dest;
			break;

		default:
			spin_lock_bh(&si->lock);
			si->exception_events[SFE_IPV6_EXCEPTION_EVENT_ICMP_IPV6_UNHANDLED_PROTOCOL]++;
			si->packets_not_forwarded++;
			spin_unlock_bh(&si->lock);

			DEBUG_TRACE_LOW("Unhandled embedded IP protocol: %u\n",
				next_hdr);
			return 0;
	}

	src_ip = &icmp_iph->saddr;
	dest_ip = &icmp_iph->daddr;

	spin_lock_bh(&si->lock);

	/*
	 * Look for a connection match.  Note that we reverse the source and destination
	 * here because our embedded message contains a packet that was sent in the
	 * opposite direction to the one in which we just received it.  It will have
	 * been sent on the interface from which we received it though so that's still
	 * ok to use.
	 */
	cm = sfe_ipv6_find_connection_match(si, dev, icmp_iph->nexthdr, dest_ip, dest_port, src_ip, src_port);
	if (unlikely(!cm)) {
		si->exception_events[SFE_IPV6_EXCEPTION_EVENT_ICMP_NO_CONNECTION]++;
		si->packets_not_forwarded++;
		spin_unlock_bh(&si->lock);

		DEBUG_TRACE_LOW("no connection found\n");
		return 0;
	}

	/*
	 * We found a connection so now remove it from the connection list and flush
	 * its state.
	 */
	c = cm->connection;
	sfe_ipv6_remove_connection(si, c);
	si->exception_events[SFE_IPV6_EXCEPTION_EVENT_ICMP_FLUSHED_CONNECTION]++;
	si->packets_not_forwarded++;
	spin_unlock_bh(&si->lock);

	sfe_ipv6_flush_connection(si, c, SFE_SYNC_REASON_FLUSH);
	return 0;
}

/*
 * sfe_ipv6_recv()
 *	Handle packet receives and forwaring.
 *
 * Returns 1 if the packet is forwarded or 0 if it isn't.
 */
int sfe_ipv6_recv(struct net_device *dev, struct sk_buff *skb, struct packet_type *pt_prev)
{
	struct sfe_ipv6 *si = &__si6;
	unsigned int len;
	unsigned int payload_len;
	unsigned int ihl = sizeof(struct sfe_ipv6_ip_hdr);
	bool flush_on_find = false;
	struct sfe_ipv6_ip_hdr *iph;
	uint8_t next_hdr;

	/*
	 * Check that we have space for an IP header and an uplayer header here.
	 */
	len = skb->len;
	if (!pskb_may_pull(skb, ihl + sizeof(struct sfe_ipv6_ext_hdr))) {
		spin_lock_bh(&si->lock);
		si->exception_events[SFE_IPV6_EXCEPTION_EVENT_HEADER_INCOMPLETE]++;
		si->packets_not_forwarded++;
		spin_unlock_bh(&si->lock);

		DEBUG_TRACE_LOW("len: %u is too short\n", len);
		return 0;
	}

	/*
	 * Is our IP version wrong?
	 */
	iph = (struct sfe_ipv6_ip_hdr *)skb->data;
	if (unlikely(iph->version != 6)) {
		spin_lock_bh(&si->lock);
		si->exception_events[SFE_IPV6_EXCEPTION_EVENT_NON_V6]++;
		si->packets_not_forwarded++;
		spin_unlock_bh(&si->lock);

		DEBUG_TRACE_LOW("IP version: %u\n", iph->version);
		return 0;
	}

	/*
	 * Does our datagram fit inside the skb?
	 */
	payload_len = ntohs(iph->payload_len);
	if (unlikely(payload_len > (len - ihl))) {
		spin_lock_bh(&si->lock);
		si->exception_events[SFE_IPV6_EXCEPTION_EVENT_DATAGRAM_INCOMPLETE]++;
		si->packets_not_forwarded++;
		spin_unlock_bh(&si->lock);

		DEBUG_TRACE_LOW("payload_len: %u, exceeds len: %u\n",
			payload_len, (len - sizeof(struct sfe_ipv6_ip_hdr)));
		return 0;
	}

	next_hdr = iph->nexthdr;
	while (unlikely(sfe_ipv6_is_ext_hdr(next_hdr))) {
		struct sfe_ipv6_ext_hdr *ext_hdr;
		unsigned int ext_hdr_len;

		ext_hdr = (struct sfe_ipv6_ext_hdr *)(skb->data + ihl);
		if (next_hdr == SFE_IPV6_EXT_HDR_FRAG) {
			struct sfe_ipv6_frag_hdr *frag_hdr = (struct sfe_ipv6_frag_hdr *)ext_hdr;
			unsigned int frag_off = ntohs(frag_hdr->frag_off);

			if (frag_off & SFE_IPV6_FRAG_OFFSET) {
				spin_lock_bh(&si->lock);
				si->exception_events[SFE_IPV6_EXCEPTION_EVENT_NON_INITIAL_FRAGMENT]++;
				si->packets_not_forwarded++;
				spin_unlock_bh(&si->lock);

				DEBUG_TRACE_LOW("non-initial fragment\n");
				return 0;
			}
		}

		ext_hdr_len = ext_hdr->hdr_len;
		ext_hdr_len <<= 3;
		ext_hdr_len += sizeof(struct sfe_ipv6_ext_hdr);
		ihl += ext_hdr_len;
		if (!pskb_may_pull(skb, ihl + sizeof(struct sfe_ipv6_ext_hdr))) {
			spin_lock_bh(&si->lock);
			si->exception_events[SFE_IPV6_EXCEPTION_EVENT_HEADER_INCOMPLETE]++;
			si->packets_not_forwarded++;
			spin_unlock_bh(&si->lock);

			DEBUG_TRACE_LOW("extension header %d not completed\n",
				next_hdr);
			return 0;
		}

		flush_on_find = true;
		next_hdr = ext_hdr->next_hdr;
	}
	if (unlikely(sfe_tcpdump_enable)) {
		sfe_tcpdump_log(skb,pt_prev);
	}
	if (IPPROTO_TCP == next_hdr) {
		return sfe_ipv6_recv_tcp(si, skb, dev, len, iph, ihl, flush_on_find);
	}

	if (IPPROTO_UDP == next_hdr) {
		return sfe_ipv6_recv_udp(si, skb, dev, len, iph, ihl, flush_on_find);
	}

	if (IPPROTO_ICMPV6 == next_hdr) {
		return sfe_ipv6_recv_icmp(si, skb, dev, len, iph, ihl);
	}

	spin_lock_bh(&si->lock);
	si->exception_events[SFE_IPV6_EXCEPTION_EVENT_UNHANDLED_PROTOCOL]++;
	si->packets_not_forwarded++;
	spin_unlock_bh(&si->lock);

	DEBUG_TRACE_LOW("not UDP, TCP or ICMP: %u\n", next_hdr);
	return 0;
}

/*
 * sfe_ipv6_update_tcp_state()
 *	update TCP window variables.
 */
	static void
sfe_ipv6_update_tcp_state(struct sfe_ipv6_connection *c,
		struct sfe_connection_create *sic)
{
	struct sfe_ipv6_connection_match *orig_cm;
	struct sfe_ipv6_connection_match *repl_cm;
	struct sfe_ipv6_tcp_connection_match *orig_tcp;
	struct sfe_ipv6_tcp_connection_match *repl_tcp;

	orig_cm = c->original_match;
	repl_cm = c->reply_match;
	orig_tcp = &orig_cm->protocol_state.tcp;
	repl_tcp = &repl_cm->protocol_state.tcp;

	/* update orig */
	if (orig_tcp->max_win < sic->src_td_max_window) {
		orig_tcp->max_win = sic->src_td_max_window;
	}
	if ((int32_t)(orig_tcp->end - sic->src_td_end) < 0) {
		orig_tcp->end = sic->src_td_end;
	}
	if ((int32_t)(orig_tcp->max_end - sic->src_td_max_end) < 0) {
		orig_tcp->max_end = sic->src_td_max_end;
	}

	/* update reply */
	if (repl_tcp->max_win < sic->dest_td_max_window) {
		repl_tcp->max_win = sic->dest_td_max_window;
	}
	if ((int32_t)(repl_tcp->end - sic->dest_td_end) < 0) {
		repl_tcp->end = sic->dest_td_end;
	}
	if ((int32_t)(repl_tcp->max_end - sic->dest_td_max_end) < 0) {
		repl_tcp->max_end = sic->dest_td_max_end;
	}

	/* update match flags */
	orig_cm->flags &= ~SFE_IPV6_CONNECTION_MATCH_FLAG_NO_SEQ_CHECK;
	repl_cm->flags &= ~SFE_IPV6_CONNECTION_MATCH_FLAG_NO_SEQ_CHECK;
	if (sic->flags & SFE_CREATE_FLAG_NO_SEQ_CHECK) {
		orig_cm->flags |= SFE_IPV6_CONNECTION_MATCH_FLAG_NO_SEQ_CHECK;
		repl_cm->flags |= SFE_IPV6_CONNECTION_MATCH_FLAG_NO_SEQ_CHECK;
	}
}

/*
 * sfe_ipv6_update_protocol_state()
 *	update protocol specified state machine.
 */
	static void
sfe_ipv6_update_protocol_state(struct sfe_ipv6_connection *c,
		struct sfe_connection_create *sic)
{
	switch (sic->protocol) {
		case IPPROTO_TCP:
			sfe_ipv6_update_tcp_state(c, sic);
			break;
	}
}

/*
 * sfe_ipv6_update_rule()
 *	update forwarding rule after rule is created.
 */
void sfe_ipv6_update_rule(struct sfe_connection_create *sic)
{
	struct sfe_ipv6_connection *c;
	struct sfe_ipv6 *si = &__si6;

	spin_lock_bh(&si->lock);

	c = sfe_ipv6_find_connection(si,
			sic->protocol,
			sic->src_ip.ip6,
			sic->src_port,
			sic->dest_ip.ip6,
			sic->dest_port);
	if (c != NULL) {
		sfe_ipv6_update_protocol_state(c, sic);
	}

	spin_unlock_bh(&si->lock);
}

/*
 * sfe_ipv6_create_rule()
 *	Create a forwarding rule.
 */
int sfe_ipv6_create_rule(struct sfe_connection_create *sic)
{
	struct sfe_ipv6 *si = &__si6;
	struct sfe_ipv6_connection *c;
	struct sfe_ipv6_connection_match *original_cm;
	struct sfe_ipv6_connection_match *reply_cm;
	struct net_device *dest_dev;
	struct net_device *src_dev;

	bool dest_dev_valid_for_pack_stats = false;
	bool src_dev_valid_for_pack_stats = false;
	struct sfe_ipv6_packet_stats_list *packet_list;



	dest_dev = sic->dest_dev;
	src_dev = sic->src_dev;

	if (unlikely((dest_dev->reg_state != NETREG_REGISTERED) ||
				(src_dev->reg_state != NETREG_REGISTERED))) {
		return -EINVAL;
	}

	spin_lock_bh(&si->lock);
	si->connection_create_requests++;

	/*
	 * Check to see if there is already a flow that matches the rule we're
	 * trying to create.  If there is then we can't create a new one.
	 */
	c = sfe_ipv6_find_connection(si,
			sic->protocol,
			sic->src_ip.ip6,
			sic->src_port,
			sic->dest_ip.ip6,
			sic->dest_port);
	if (c != NULL) {
		si->connection_create_collisions++;

		/*
		 * If we already have the flow then it's likely that this
		 * request to create the connection rule contains more
		 * up-to-date information. Check and update accordingly.
		 */
		sfe_ipv6_update_protocol_state(c, sic);
		spin_unlock_bh(&si->lock);

		DEBUG_TRACE("connection already exists - mark: %08x, p: %d\n"
				"  s: %s:%pM:%pI6:%u, d: %s:%pM:%pI6:%u\n",
				sic->mark, sic->protocol,
				sic->src_dev->name, sic->src_mac, sic->src_ip.ip6, ntohs(sic->src_port),
				sic->dest_dev->name, sic->dest_mac, sic->dest_ip.ip6, ntohs(sic->dest_port));
		return -EADDRINUSE;
	}

	/*
	 * Allocate the various connection tracking objects.
	 */
	c = (struct sfe_ipv6_connection *)kmalloc(sizeof(struct sfe_ipv6_connection), GFP_ATOMIC);
	if (unlikely(!c)) {
		spin_unlock_bh(&si->lock);
		return -ENOMEM;
	}

	original_cm = (struct sfe_ipv6_connection_match *)kmalloc(sizeof(struct sfe_ipv6_connection_match), GFP_ATOMIC);
	if (unlikely(!original_cm)) {
		spin_unlock_bh(&si->lock);
		kfree(c);
		return -ENOMEM;
	}

	reply_cm = (struct sfe_ipv6_connection_match *)kmalloc(sizeof(struct sfe_ipv6_connection_match), GFP_ATOMIC);
	if (unlikely(!reply_cm)) {
		spin_unlock_bh(&si->lock);
		kfree(original_cm);
		kfree(c);
		return -ENOMEM;
	}

	if (packet_stats_enabled)
	{
		packet_list  = (struct sfe_ipv6_packet_stats_list *)kmalloc(sizeof(struct sfe_ipv6_packet_stats_list)
				, GFP_ATOMIC);
		if (unlikely(!packet_list)) {
			spin_unlock_bh(&si->lock);
			kfree(reply_cm);
			kfree(original_cm);
			kfree(c);
			return -ENOMEM;
		}
	}
	/*
	 * Fill in the "original" direction connection matching object.
	 * Note that the transmit MAC address is "dest_mac_xlate" because
	 * we always know both ends of a connection by their translated
	 * addresses and not their public addresses.
	 */
	original_cm->match_dev = src_dev;
	original_cm->match_protocol = sic->protocol;
	original_cm->match_src_ip[0] = sic->src_ip.ip6[0];
	original_cm->match_src_port = sic->src_port;
	original_cm->match_dest_ip[0] = sic->dest_ip.ip6[0];
	original_cm->match_dest_port = sic->dest_port;
	original_cm->xlate_src_ip[0] = sic->src_ip_xlate.ip6[0];
	original_cm->xlate_src_port = sic->src_port_xlate;
	original_cm->xlate_dest_ip[0] = sic->dest_ip_xlate.ip6[0];
	original_cm->xlate_dest_port = sic->dest_port_xlate;
	original_cm->rx_packet_count = 0;
	original_cm->rx_pack_stat_byte_count = 0;
	original_cm->rx_packet_count64 = 0;
	original_cm->rx_byte_count = 0;
	original_cm->rx_byte_count64 = 0;
	original_cm->xmit_dev = dest_dev;
	original_cm->xmit_dev_mtu = sic->dest_mtu;
	memcpy(original_cm->xmit_src_mac, dest_dev->dev_addr, ETH_ALEN);
	memcpy(original_cm->xmit_dest_mac, sic->dest_mac_xlate, ETH_ALEN);
	original_cm->connection = c;
	original_cm->counter_match = reply_cm;
	original_cm->flags = 0;
	original_cm->pad_removal_require = false;
#ifdef CONFIG_NF_FLOW_COOKIE
	original_cm->flow_cookie = 0;
#endif
#ifdef CONFIG_XFRM
	original_cm->flow_accel = sic->original_accel;
#endif
	original_cm->active_next = NULL;
	original_cm->active_prev = NULL;
	original_cm->active = false;
	original_cm->expand_head = true;

	/*
	 * For PPP links we don't write an L2 header.  For everything else we do.
	 */
	if (!(dest_dev->flags & IFF_POINTOPOINT)) {
		original_cm->flags |= SFE_IPV6_CONNECTION_MATCH_FLAG_WRITE_L2_HDR;

		/*
		 * If our dev writes Ethernet headers then we can write a really fast
		 * version.
		 */
		if (dest_dev->header_ops) {
			if (dest_dev->header_ops->create == eth_header) {
				original_cm->flags |= SFE_IPV6_CONNECTION_MATCH_FLAG_WRITE_FAST_ETH_HDR;
			}
		}
	}

	/*
	 * Fill in the "reply" direction connection matching object.
	 */
	reply_cm->match_dev = dest_dev;
	reply_cm->match_protocol = sic->protocol;
	reply_cm->match_src_ip[0] = sic->dest_ip_xlate.ip6[0];
	reply_cm->match_src_port = sic->dest_port_xlate;
	reply_cm->match_dest_ip[0] = sic->src_ip_xlate.ip6[0];
	reply_cm->match_dest_port = sic->src_port_xlate;
	reply_cm->xlate_src_ip[0] = sic->dest_ip.ip6[0];
	reply_cm->xlate_src_port = sic->dest_port;
	reply_cm->xlate_dest_ip[0] = sic->src_ip.ip6[0];
	reply_cm->xlate_dest_port = sic->src_port;
	reply_cm->rx_packet_count = 0;
	reply_cm->rx_packet_count64 = 0;
	reply_cm->rx_byte_count = 0;
	reply_cm->rx_pack_stat_byte_count = 0;
	reply_cm->rx_byte_count64 = 0;
	reply_cm->xmit_dev = src_dev;
	reply_cm->xmit_dev_mtu = sic->src_mtu;
	memcpy(reply_cm->xmit_src_mac, src_dev->dev_addr, ETH_ALEN);
	memcpy(reply_cm->xmit_dest_mac, sic->src_mac, ETH_ALEN);
	reply_cm->connection = c;
	reply_cm->counter_match = original_cm;
	reply_cm->flags = 0;
	reply_cm->pad_removal_require = false;
#ifdef CONFIG_NF_FLOW_COOKIE
	reply_cm->flow_cookie = 0;
#endif
#ifdef CONFIG_XFRM
	reply_cm->flow_accel = sic->reply_accel;
#endif
	reply_cm->active_next = NULL;
	reply_cm->active_prev = NULL;
	reply_cm->active = false;
	reply_cm->expand_head = true;

	/*
	 * For PPP links we don't write an L2 header.  For everything else we do.
	 */
	if (!(src_dev->flags & IFF_POINTOPOINT)) {
		reply_cm->flags |= SFE_IPV6_CONNECTION_MATCH_FLAG_WRITE_L2_HDR;

		/*
		 * If our dev writes Ethernet headers then we can write a really fast
		 * version.
		 */
		if (src_dev->header_ops) {
			if (src_dev->header_ops->create == eth_header) {
				reply_cm->flags |= SFE_IPV6_CONNECTION_MATCH_FLAG_WRITE_FAST_ETH_HDR;
			}
		}
	}


	if (!sfe_ipv6_addr_equal(sic->dest_ip.ip6, sic->dest_ip_xlate.ip6) || sic->dest_port != sic->dest_port_xlate) {
		original_cm->flags |= SFE_IPV6_CONNECTION_MATCH_FLAG_XLATE_DEST;
		reply_cm->flags |= SFE_IPV6_CONNECTION_MATCH_FLAG_XLATE_SRC;
		original_cm->expand_head = false;
		reply_cm->expand_head = false;
	}

	if (!sfe_ipv6_addr_equal(sic->src_ip.ip6, sic->src_ip_xlate.ip6) || sic->src_port != sic->src_port_xlate) {
		original_cm->flags |= SFE_IPV6_CONNECTION_MATCH_FLAG_XLATE_SRC;
		reply_cm->flags |= SFE_IPV6_CONNECTION_MATCH_FLAG_XLATE_DEST;
		original_cm->expand_head = false;
		reply_cm->expand_head = false;
	}

	c->protocol = sic->protocol;
	c->src_ip[0] = sic->src_ip.ip6[0];
	c->src_ip_xlate[0] = sic->src_ip_xlate.ip6[0];
	c->src_port = sic->src_port;
	c->src_port_xlate = sic->src_port_xlate;
	c->original_dev = src_dev;
	c->original_match = original_cm;
	c->dest_ip[0] = sic->dest_ip.ip6[0];
	c->dest_ip_xlate[0] = sic->dest_ip_xlate.ip6[0];
	c->dest_port = sic->dest_port;
	c->dest_port_xlate = sic->dest_port_xlate;
	c->reply_dev = dest_dev;
	c->reply_match = reply_cm;
	c->mark = sic->mark;
	c->debug_read_seq = 0;
	c->last_sync_jiffies = get_jiffies_64();

	if (strncmp(dest_dev->name, si->ipv6_iface, strlen(si->ipv6_iface) - 1)== 0) {
		c->use_destMac = false;
		original_cm->addEthMAC = false;
		reply_cm->addEthMAC = true;
		dest_dev_valid_for_pack_stats = true;
	}
	else if (strncmp(src_dev->name, si->ipv6_iface, strlen(si->ipv6_iface) - 1)== 0) {
		c->use_destMac = false;
		reply_cm->addEthMAC = false;
		original_cm->addEthMAC = true;
		src_dev_valid_for_pack_stats = true;
	}
	else {
		c->use_destMac = true;
		reply_cm->addEthMAC = false;
		original_cm->addEthMAC = false;
	}

	/* Skip headroom in case dest is wlan0 or wlan1*/
	if ((strncmp(dest_dev->name, WLAN_INTF1, WLAN_INTF_LEN)  == 0) ||
			(strncmp(dest_dev->name, WLAN_INTF2, WLAN_INTF_LEN)  == 0 ) ||
			(strncmp(dest_dev->name, WLAN_INTF3, WLAN_INTF_LEN)  == 0 ) ||
			(strncmp(dest_dev->name, WLAN_INTF4, WLAN_INTF_LEN)  == 0 ))
	{
		/* For LAN-LAN communication make sure enough headroom is available. */
		original_cm->expand_head = false;
		reply_cm->expand_head = false;
	}
	if ((strncmp(dest_dev->name, WLAN_INTF1, WLAN_INTF_LEN)  == 0)) 
	{
		original_cm->index = SFE_WLAN_LINK_INDEX0;
		reply_cm->index = SFE_WLAN_LINK_INDEX_NONE;
		/* For LAN-LAN communication make sure enough headroom is available. */
		original_cm->expand_head = false;
		reply_cm->expand_head = false;
	}
	else if ((strncmp(dest_dev->name, WLAN_INTF2, WLAN_INTF_LEN)  == 0 ))
	{
		original_cm->index = SFE_WLAN_LINK_INDEX1;
		reply_cm->index = SFE_WLAN_LINK_INDEX_NONE;
	}
	else if ((strncmp(dest_dev->name, WLAN_INTF3, WLAN_INTF_LEN)  == 0))
	{
		original_cm->index = SFE_WLAN_LINK_INDEX2;
		reply_cm->index = SFE_WLAN_LINK_INDEX_NONE;
	}
	else if ((strncmp(dest_dev->name, WLAN_INTF4, WLAN_INTF_LEN)  == 0 ))
	{
		original_cm->index = SFE_WLAN_LINK_INDEX3;
		reply_cm->index = SFE_WLAN_LINK_INDEX_NONE;
	}

	else if((strncmp(src_dev->name, WLAN_INTF1,WLAN_INTF_LEN) == 0 ))
	{
		original_cm->index = SFE_WLAN_LINK_INDEX_NONE;
		reply_cm->index = SFE_WLAN_LINK_INDEX0;
	}
	else if ((strncmp(src_dev->name, WLAN_INTF2, WLAN_INTF_LEN)  == 0 ))
	{
		original_cm->index = SFE_WLAN_LINK_INDEX_NONE;
		reply_cm->index = SFE_WLAN_LINK_INDEX1;
	}
	else if((strncmp(src_dev->name, WLAN_INTF3,WLAN_INTF_LEN) == 0 ))
	{
		original_cm->index = SFE_WLAN_LINK_INDEX_NONE;
		reply_cm->index = SFE_WLAN_LINK_INDEX2;
	}
	else if((strncmp(src_dev->name, WLAN_INTF4,WLAN_INTF_LEN) == 0 ))
	{
		original_cm->index = SFE_WLAN_LINK_INDEX_NONE;
		reply_cm->index = SFE_WLAN_LINK_INDEX3;
	}
	else
	{
		original_cm->index = SFE_WLAN_LINK_INDEX_NONE;
		reply_cm->index = SFE_WLAN_LINK_INDEX_NONE;
	}
	if ((strncmp(src_dev->name, ETH_INTF, ETH_INTF_LEN)  == 0))
		original_cm->pad_removal_require = true;

	if ((strncmp(dest_dev->name, ETH_INTF, ETH_INTF_LEN)  == 0))
		reply_cm->pad_removal_require= true;

	/*
	 * Take hold of our source and dest devices for the duration of the connection.
	 */
	dev_hold(c->original_dev);
	dev_hold(c->reply_dev);

	/*
	 * Initialize the protocol-specific information that we track.
	 */
	switch (sic->protocol) {
		case IPPROTO_TCP:
			original_cm->protocol_state.tcp.win_scale = sic->src_td_window_scale;
			original_cm->protocol_state.tcp.max_win = sic->src_td_max_window ? sic->src_td_max_window : 1;
			original_cm->protocol_state.tcp.end = sic->src_td_end;
			original_cm->protocol_state.tcp.max_end = sic->src_td_max_end;
			reply_cm->protocol_state.tcp.win_scale = sic->dest_td_window_scale;
			reply_cm->protocol_state.tcp.max_win = sic->dest_td_max_window ? sic->dest_td_max_window : 1;
			reply_cm->protocol_state.tcp.end = sic->dest_td_end;
			reply_cm->protocol_state.tcp.max_end = sic->dest_td_max_end;
			if (sic->flags & SFE_CREATE_FLAG_NO_SEQ_CHECK) {
				original_cm->flags |= SFE_IPV6_CONNECTION_MATCH_FLAG_NO_SEQ_CHECK;
				reply_cm->flags |= SFE_IPV6_CONNECTION_MATCH_FLAG_NO_SEQ_CHECK;
			}
			break;
	}

	sfe_ipv6_connection_match_compute_translations(original_cm);
	sfe_ipv6_connection_match_compute_translations(reply_cm);
	sfe_ipv6_insert_connection(si, c);

	if (packet_stats_enabled)
	{

		if (src_dev_valid_for_pack_stats)
		{
			if (!sfe_ipv6_addr_equal(sic->dest_ip.ip6, sic->dest_ip_xlate.ip6))
			{
				packet_list->packet_stats_node.client_src_addr[0] = sic->dest_ip_xlate.ip6[0];
			}
			else
			{
				packet_list->packet_stats_node.client_src_addr[0] = sic->dest_ip.ip6[0];
			}
			packet_list->packet_stats_node.packet_stat_node_rx_byte_count = 0;
			packet_list->packet_stats_node.packet_stat_node_tx_byte_count = 0;
			packet_list->packet_stat_node_read_seq = 0;
			INIT_HLIST_NODE(&packet_list->sfe_ipv6_packet_hash_list);
			sfe_ipv6_insert_packet_stats_connection(si, packet_list);
		}
		if (dest_dev_valid_for_pack_stats)
		{
			packet_list->packet_stats_node.client_src_addr[0] = sic->src_ip.ip6[0];
			packet_list->packet_stats_node.packet_stat_node_rx_byte_count = 0;
			packet_list->packet_stats_node.packet_stat_node_tx_byte_count = 0;
			packet_list->packet_stat_node_read_seq = 0;
			INIT_HLIST_NODE(&packet_list->sfe_ipv6_packet_hash_list);
			sfe_ipv6_insert_packet_stats_connection(si, packet_list);
		}
	}
	spin_unlock_bh(&si->lock);

	/*
	 * We have everything we need!
	 */
	DEBUG_INFO("new connection - mark: %08x, p: %d\n"
			"  s: %s:%pM(%pM):%pI6(%pI6):%u(%u)\n"
			"  d: %s:%pM(%pM):%pI6(%pI6):%u(%u)\n",
			sic->mark, sic->protocol,
			sic->src_dev->name, sic->src_mac, sic->src_mac_xlate,
			sic->src_ip.ip6, sic->src_ip_xlate.ip6, ntohs(sic->src_port), ntohs(sic->src_port_xlate),
			dest_dev->name, sic->dest_mac, sic->dest_mac_xlate,
			sic->dest_ip.ip6, sic->dest_ip_xlate.ip6, ntohs(sic->dest_port), ntohs(sic->dest_port_xlate));

	return 0;
}

/*
 * sfe_ipv6_destroy_rule()
 *	Destroy a forwarding rule.
 */
void sfe_ipv6_destroy_rule(struct sfe_connection_destroy *sid)
{
	struct sfe_ipv6 *si = &__si6;
	struct sfe_ipv6_connection *c;

	spin_lock_bh(&si->lock);
	si->connection_destroy_requests++;

	/*
	 * Check to see if we have a flow that matches the rule we're trying
	 * to destroy.  If there isn't then we can't destroy it.
	 */
	c = sfe_ipv6_find_connection(si, sid->protocol, sid->src_ip.ip6, sid->src_port,
			sid->dest_ip.ip6, sid->dest_port);
	if (!c) {
		si->connection_destroy_misses++;
		spin_unlock_bh(&si->lock);

		DEBUG_TRACE_LOW(
			"connection does not exist - p: %d, s: %pI6:%u, d: %pI6:%u\n",
				sid->protocol, sid->src_ip.ip6, ntohs(sid->src_port),
				sid->dest_ip.ip6, ntohs(sid->dest_port));
		return;
	}

	/*
	 * Remove our connection details from the hash tables.
	 */
	sfe_ipv6_remove_connection(si, c);
	spin_unlock_bh(&si->lock);

	sfe_ipv6_flush_connection(si, c, SFE_SYNC_REASON_DESTROY);

	DEBUG_INFO("connection destroyed - p: %d, s: %pI6:%u, d: %pI6:%u\n",
			sid->protocol, sid->src_ip.ip6, ntohs(sid->src_port),
			sid->dest_ip.ip6, ntohs(sid->dest_port));
}

/*
 * sfe_ipv6_register_sync_rule_callback()
 *	Register a callback for rule synchronization.
 */
void sfe_ipv6_register_sync_rule_callback(sfe_sync_rule_callback_t sync_rule_callback)
{
	struct sfe_ipv6 *si = &__si6;

	spin_lock_bh(&si->lock);
	rcu_assign_pointer(si->sync_rule_callback, sync_rule_callback);
	spin_unlock_bh(&si->lock);
}

/*
 * sfe_ipv6_get_debug_dev()
 */
static ssize_t sfe_ipv6_get_debug_dev(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
	struct sfe_ipv6 *si = &__si6;
	ssize_t count;
	int num;

	spin_lock_bh(&si->lock);
	num = si->debug_dev;
	spin_unlock_bh(&si->lock);

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "%d\n", num);
	return count;
}

/*
 * sfe_ipv6_destroy_all_rules_for_dev()
 *	Destroy all connections that match a particular device.
 *
 * If we pass dev as NULL then this destroys all connections.
 */
void sfe_ipv6_destroy_all_rules_for_dev(struct net_device *dev)
{
	struct sfe_ipv6 *si = &__si6;
	struct sfe_ipv6_connection *c;

another_round:
	spin_lock_bh(&si->lock);

	for (c = si->all_connections_head; c; c = c->all_connections_next) {
		/*
		 * Does this connection relate to the device we are destroying?
		 */
		if (!dev
				|| (dev == c->original_dev)
				|| (dev == c->reply_dev)) {
			break;
		}
	}

	if (c) {
		sfe_ipv6_remove_connection(si, c);
	}

	spin_unlock_bh(&si->lock);

	if (c) {
		sfe_ipv6_flush_connection(si, c, SFE_SYNC_REASON_DESTROY);
		goto another_round;
	}
}

/*
 * sfe_ipv6_periodic_sync()
 */
static void sfe_ipv6_periodic_sync(unsigned long arg)
{
	struct sfe_ipv6 *si = (struct sfe_ipv6 *)arg;
	uint64_t now_jiffies;
	int quota;
	sfe_sync_rule_callback_t sync_rule_callback;

	now_jiffies = get_jiffies_64();

	rcu_read_lock();
	sync_rule_callback = rcu_dereference(si->sync_rule_callback);
	if (!sync_rule_callback) {
		rcu_read_unlock();
		goto done;
	}

	spin_lock_bh(&si->lock);
	sfe_ipv6_update_summary_stats(si);

	/*
	 * Get an estimate of the number of connections to parse in this sync.
	 */
	quota = (si->num_connections + 63) / 64;

	/*
	 * Walk the "active" list and sync the connection state.
	 */
	while (quota--) {
		struct sfe_ipv6_connection_match *cm;
		struct sfe_ipv6_connection_match *counter_cm;
		struct sfe_ipv6_connection *c;
		struct sfe_connection_sync sis;

		cm = si->active_head;
		if (!cm) {
			break;
		}

		/*
		 * There's a possibility that our counter match is in the active list too.
		 * If it is then remove it.
		 */
		counter_cm = cm->counter_match;
		if (counter_cm->active) {
			counter_cm->active = false;

			/*
			 * We must have a connection preceding this counter match
			 * because that's the one that got us to this point, so we don't have
			 * to worry about removing the head of the list.
			 */
			counter_cm->active_prev->active_next = counter_cm->active_next;

			if (likely(counter_cm->active_next)) {
				counter_cm->active_next->active_prev = counter_cm->active_prev;
			} else {
				si->active_tail = counter_cm->active_prev;
			}

			counter_cm->active_next = NULL;
			counter_cm->active_prev = NULL;
		}

		/*
		 * Now remove the head of the active scan list.
		 */
		cm->active = false;
		si->active_head = cm->active_next;
		if (likely(cm->active_next)) {
			cm->active_next->active_prev = NULL;
		} else {
			si->active_tail = NULL;
		}
		cm->active_next = NULL;

		/*
		 * Sync the connection state.
		 */
		c = cm->connection;
		sfe_ipv6_gen_sync_connection(si, c, &sis, SFE_SYNC_REASON_STATS, now_jiffies);

		/*
		 * We don't want to be holding the lock when we sync!
		 */
		spin_unlock_bh(&si->lock);
		sync_rule_callback(&sis);
		spin_lock_bh(&si->lock);
	}

	spin_unlock_bh(&si->lock);
	rcu_read_unlock();

done:
	mod_timer(&si->timer, jiffies + ((HZ + 99) / 100));
}

/*
 * sfe_ipv6_debug_dev_read_start()
 *	Generate part of the XML output.
 */
static bool sfe_ipv6_debug_dev_read_start(struct sfe_ipv6 *si, char *buffer, char *msg, size_t *length,
		int *total_read, struct sfe_ipv6_debug_xml_write_state *ws)
{
	int bytes_read;

	si->debug_read_seq++;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "<sfe_ipv6>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	ws->state++;
	return true;
}

/*
 * sfe_ipv6_debug_dev_read_connections_start()
 *	Generate part of the XML output.
 */
static bool sfe_ipv6_debug_dev_read_connections_start(struct sfe_ipv6 *si, char *buffer, char *msg, size_t *length,
		int *total_read, struct sfe_ipv6_debug_xml_write_state *ws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t<connections>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	ws->state++;
	return true;
}

/*
 * sfe_ipv6_debug_dev_read_connections_connection()
 *	Generate part of the XML output.
 */
static bool sfe_ipv6_debug_dev_read_connections_connection(struct sfe_ipv6 *si, char *buffer, char *msg, size_t *length,
		int *total_read, struct sfe_ipv6_debug_xml_write_state *ws)
{
	struct sfe_ipv6_connection *c;
	struct sfe_ipv6_connection_match *original_cm;
	struct sfe_ipv6_connection_match *reply_cm;
	int bytes_read;
	int protocol;
	struct net_device *src_dev;
	struct sfe_ipv6_addr src_ip;
	struct sfe_ipv6_addr src_ip_xlate;
	__be16 src_port;
	__be16 src_port_xlate;
	uint64_t src_rx_packets;
	uint64_t src_rx_bytes;
	struct net_device *dest_dev;
	struct sfe_ipv6_addr dest_ip;
	struct sfe_ipv6_addr dest_ip_xlate;
	__be16 dest_port;
	__be16 dest_port_xlate;
	uint64_t dest_rx_packets;
	uint64_t dest_rx_bytes;
	uint64_t last_sync_jiffies;
	uint32_t mark;
#ifdef CONFIG_NF_FLOW_COOKIE
	int src_flow_cookie, dst_flow_cookie;
#endif

	spin_lock_bh(&si->lock);

	for (c = si->all_connections_head; c; c = c->all_connections_next) {
		if (c->debug_read_seq < si->debug_read_seq) {
			c->debug_read_seq = si->debug_read_seq;
			break;
		}
	}

	/*
	 * If there were no connections then move to the next state.
	 */
	if (!c) {
		spin_unlock_bh(&si->lock);
		ws->state++;
		return true;
	}

	original_cm = c->original_match;
	reply_cm = c->reply_match;

	protocol = c->protocol;
	src_dev = c->original_dev;
	src_ip = c->src_ip[0];
	src_ip_xlate = c->src_ip_xlate[0];
	src_port = c->src_port;
	src_port_xlate = c->src_port_xlate;

	sfe_ipv6_connection_match_update_summary_stats(original_cm);
	sfe_ipv6_connection_match_update_summary_stats(reply_cm);

	src_rx_packets = original_cm->rx_packet_count64;
	src_rx_bytes = original_cm->rx_byte_count64;
	dest_dev = c->reply_dev;
	dest_ip = c->dest_ip[0];
	dest_ip_xlate = c->dest_ip_xlate[0];
	dest_port = c->dest_port;
	dest_port_xlate = c->dest_port_xlate;
	dest_rx_packets = reply_cm->rx_packet_count64;
	dest_rx_bytes = reply_cm->rx_byte_count64;
	last_sync_jiffies = get_jiffies_64() - c->last_sync_jiffies;
	mark = c->mark;
#ifdef CONFIG_NF_FLOW_COOKIE
	src_flow_cookie = original_cm->flow_cookie;
	dst_flow_cookie = reply_cm->flow_cookie;
#endif
	spin_unlock_bh(&si->lock);

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t\t<connection "
			"protocol=\"%u\" "
			"src_dev=\"%s\" "
			"src_ip=\"%pI6\" src_ip_xlate=\"%pI6\" "
			"src_port=\"%u\" src_port_xlate=\"%u\" "
			"src_rx_pkts=\"%llu\" src_rx_bytes=\"%llu\" "
			"dest_dev=\"%s\" "
			"dest_ip=\"%pI6\" dest_ip_xlate=\"%pI6\" "
			"dest_port=\"%u\" dest_port_xlate=\"%u\" "
			"dest_rx_pkts=\"%llu\" dest_rx_bytes=\"%llu\" "
#ifdef CONFIG_NF_FLOW_COOKIE
			"src_flow_cookie=\"%d\" dst_flow_cookie=\"%d\" "
#endif
			"last_sync=\"%llu\" "
			"mark=\"%08x\" />\n",
			protocol,
			src_dev->name,
			&src_ip, &src_ip_xlate,
			ntohs(src_port), ntohs(src_port_xlate),
			src_rx_packets, src_rx_bytes,
			dest_dev->name,
			&dest_ip, &dest_ip_xlate,
			ntohs(dest_port), ntohs(dest_port_xlate),
			dest_rx_packets, dest_rx_bytes,
#ifdef CONFIG_NF_FLOW_COOKIE
			src_flow_cookie, dst_flow_cookie,
#endif
			last_sync_jiffies, mark);

	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	return true;
}

/*
 * sfe_ipv6_debug_dev_read_connections_end()
 *	Generate part of the XML output.
 */
static bool sfe_ipv6_debug_dev_read_connections_end(struct sfe_ipv6 *si, char *buffer, char *msg, size_t *length,
		int *total_read, struct sfe_ipv6_debug_xml_write_state *ws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t</connections>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	ws->state++;
	return true;
}

/*
 * sfe_ipv6_debug_dev_read_exceptions_start()
 *	Generate part of the XML output.
 */
static bool sfe_ipv6_debug_dev_read_exceptions_start(struct sfe_ipv6 *si, char *buffer, char *msg, size_t *length,
		int *total_read, struct sfe_ipv6_debug_xml_write_state *ws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t<exceptions>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	ws->state++;
	return true;
}

/*
 * sfe_ipv6_debug_dev_read_exceptions_exception()
 *	Generate part of the XML output.
 */
static bool sfe_ipv6_debug_dev_read_exceptions_exception(struct sfe_ipv6 *si, char *buffer, char *msg, size_t *length,
		int *total_read, struct sfe_ipv6_debug_xml_write_state *ws)
{
	uint64_t ct;

	spin_lock_bh(&si->lock);
	ct = si->exception_events64[ws->iter_exception];
	spin_unlock_bh(&si->lock);

	if (ct) {
		int bytes_read;

		bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE,
				"\t\t<exception name=\"%s\" count=\"%llu\" />\n",
				sfe_ipv6_exception_events_string[ws->iter_exception],
				ct);
		if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
			return false;
		}

		*length -= bytes_read;
		*total_read += bytes_read;
	}

	ws->iter_exception++;
	if (ws->iter_exception >= SFE_IPV6_EXCEPTION_EVENT_LAST) {
		ws->iter_exception = 0;
		ws->state++;
	}

	return true;
}

/*
 * sfe_ipv6_debug_dev_read_exceptions_end()
 *	Generate part of the XML output.
 */
static bool sfe_ipv6_debug_dev_read_exceptions_end(struct sfe_ipv6 *si, char *buffer, char *msg, size_t *length,
		int *total_read, struct sfe_ipv6_debug_xml_write_state *ws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t</exceptions>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	ws->state++;
	return true;
}

/*
 * sfe_ipv6_debug_dev_read_stats()
 *	Generate part of the XML output.
 */
static bool sfe_ipv6_debug_dev_read_stats(struct sfe_ipv6 *si, char *buffer, char *msg, size_t *length,
		int *total_read, struct sfe_ipv6_debug_xml_write_state *ws)
{
	int bytes_read;
	unsigned int num_connections;
	uint64_t packets_forwarded;
	uint64_t packets_not_forwarded;
	uint64_t connection_create_requests;
	uint64_t connection_create_collisions;
	uint64_t connection_destroy_requests;
	uint64_t connection_destroy_misses;
	uint64_t connection_flushes;
	uint64_t connection_match_hash_hits;
	uint64_t connection_match_hash_reorders;

	spin_lock_bh(&si->lock);
	sfe_ipv6_update_summary_stats(si);

	num_connections = si->num_connections;
	packets_forwarded = si->packets_forwarded64;
	packets_not_forwarded = si->packets_not_forwarded64;
	connection_create_requests = si->connection_create_requests64;
	connection_create_collisions = si->connection_create_collisions64;
	connection_destroy_requests = si->connection_destroy_requests64;
	connection_destroy_misses = si->connection_destroy_misses64;
	connection_flushes = si->connection_flushes64;
	connection_match_hash_hits = si->connection_match_hash_hits64;
	connection_match_hash_reorders = si->connection_match_hash_reorders64;
	spin_unlock_bh(&si->lock);

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "\t<stats "
			"num_connections=\"%u\" "
			"pkts_forwarded=\"%llu\" pkts_not_forwarded=\"%llu\" "
			"create_requests=\"%llu\" create_collisions=\"%llu\" "
			"destroy_requests=\"%llu\" destroy_misses=\"%llu\" "
			"flushes=\"%llu\" "
			"hash_hits=\"%llu\" hash_reorders=\"%llu\" />\n",
			num_connections,
			packets_forwarded,
			packets_not_forwarded,
			connection_create_requests,
			connection_create_collisions,
			connection_destroy_requests,
			connection_destroy_misses,
			connection_flushes,
			connection_match_hash_hits,
			connection_match_hash_reorders);
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	ws->state++;
	return true;
}

/*
 * sfe_ipv6_debug_dev_read_end()
 *	Generate part of the XML output.
 */
static bool sfe_ipv6_debug_dev_read_end(struct sfe_ipv6 *si, char *buffer, char *msg, size_t *length,
		int *total_read, struct sfe_ipv6_debug_xml_write_state *ws)
{
	int bytes_read;

	bytes_read = snprintf(msg, CHAR_DEV_MSG_SIZE, "</sfe_ipv6>\n");
	if (copy_to_user(buffer + *total_read, msg, CHAR_DEV_MSG_SIZE)) {
		return false;
	}

	*length -= bytes_read;
	*total_read += bytes_read;

	ws->state++;
	return true;
}

/*
 * Array of write functions that write various XML elements that correspond to
 * our XML output state machine.
 */
static sfe_ipv6_debug_xml_write_method_t sfe_ipv6_debug_xml_write_methods[SFE_IPV6_DEBUG_XML_STATE_DONE] = {
	sfe_ipv6_debug_dev_read_start,
	sfe_ipv6_debug_dev_read_connections_start,
	sfe_ipv6_debug_dev_read_connections_connection,
	sfe_ipv6_debug_dev_read_connections_end,
	sfe_ipv6_debug_dev_read_exceptions_start,
	sfe_ipv6_debug_dev_read_exceptions_exception,
	sfe_ipv6_debug_dev_read_exceptions_end,
	sfe_ipv6_debug_dev_read_stats,
	sfe_ipv6_debug_dev_read_end,
};

/*
 * sfe_ipv6_debug_dev_read()
 *	Send info to userspace upon read request from user
 */
static ssize_t sfe_ipv6_debug_dev_read(struct file *filp, char *buffer, size_t length, loff_t *offset)
{
	char msg[CHAR_DEV_MSG_SIZE];
	int total_read = 0;
	struct sfe_ipv6_debug_xml_write_state *ws;
	struct sfe_ipv6 *si = &__si6;

	ws = (struct sfe_ipv6_debug_xml_write_state *)filp->private_data;
	while ((ws->state != SFE_IPV6_DEBUG_XML_STATE_DONE) && (length > CHAR_DEV_MSG_SIZE)) {
		if ((sfe_ipv6_debug_xml_write_methods[ws->state])(si, buffer, msg, &length, &total_read, ws)) {
			continue;
		}
	}

	return total_read;
}

/*
 * sfe_ipv6_debug_dev_write()
 *	Write to char device resets some stats
 */
static ssize_t sfe_ipv6_debug_dev_write(struct file *filp, const char *buffer, size_t length, loff_t *offset)
{
	struct sfe_ipv6 *si = &__si6;

	spin_lock_bh(&si->lock);
	sfe_ipv6_update_summary_stats(si);

	si->packets_forwarded64 = 0;
	si->packets_not_forwarded64 = 0;
	si->connection_create_requests64 = 0;
	si->connection_create_collisions64 = 0;
	si->connection_destroy_requests64 = 0;
	si->connection_destroy_misses64 = 0;
	si->connection_flushes64 = 0;
	si->connection_match_hash_hits64 = 0;
	si->connection_match_hash_reorders64 = 0;
	spin_unlock_bh(&si->lock);

	return length;
}

/*
 * sfe_ipv6_debug_dev_open()
 */
static int sfe_ipv6_debug_dev_open(struct inode *inode, struct file *file)
{
	struct sfe_ipv6_debug_xml_write_state *ws;

	ws = (struct sfe_ipv6_debug_xml_write_state *)file->private_data;
	if (ws) {
		return 0;
	}

	ws = kzalloc(sizeof(struct sfe_ipv6_debug_xml_write_state), GFP_KERNEL);
	if (!ws) {
		return -ENOMEM;
	}

	ws->state = SFE_IPV6_DEBUG_XML_STATE_START;
	file->private_data = ws;

	return 0;
}

/*
 * sfe_ipv6_debug_dev_release()
 */
static int sfe_ipv6_debug_dev_release(struct inode *inode, struct file *file)
{
	struct sfe_ipv6_debug_xml_write_state *ws;

	ws = (struct sfe_ipv6_debug_xml_write_state *)file->private_data;
	if (ws) {
		/*
		 * We've finished with our output so free the write state.
		 */
		kfree(ws);
	}

	return 0;
}

/*
 * File operations used in the debug char device
 */
static struct file_operations sfe_ipv6_debug_dev_fops = {
	.read = sfe_ipv6_debug_dev_read,
	.write = sfe_ipv6_debug_dev_write,
	.open = sfe_ipv6_debug_dev_open,
	.release = sfe_ipv6_debug_dev_release
};

static ssize_t
read_from_v6_iface_proc_entry(
				struct file *filp, char *buf,
					size_t count, loff_t *offp)
{
	struct sfe_ipv6 *si = &__si6;

	if ( count < si->iface_length )
		return -EINVAL;

	if (*offp != 0)
		return 0;

	if (copy_to_user(buf, si->ipv6_iface, si->iface_length))
		return -EINVAL;

	*offp = si->iface_length;

	return si->iface_length;
}

static ssize_t write_to_v6_iface_proc_entry(struct file *file,const char *buf,size_t count,loff_t *data )
{
	struct sfe_ipv6 *si = &__si6;

	if (count == 0) {
		DEBUG_ERROR("Iface_length zero \n");
		return 0;
	}
	else if(count > MAX_INTF_LEN)
	{
		DEBUG_ERROR("Iface_length too big \n");
		return 0;
	}

	memset(si->ipv6_iface, 0, MAX_INTF_LEN);

	if(copy_from_user(si->ipv6_iface, buf, count-1))
		return -EFAULT;

	si->iface_length = strlen(si->ipv6_iface);
	DEBUG_INFO( "Iface length = %d , iface_name = %s \n", si->iface_length, si->ipv6_iface);

	return count;
}


static struct file_operations ipv6_iface_proc_fops = {
	.owner = THIS_MODULE,
	.read = read_from_v6_iface_proc_entry,
	.write = write_to_v6_iface_proc_entry,
};


#ifdef CONFIG_NF_FLOW_COOKIE
/*
 * sfe_ipv6_register_flow_cookie_cb
 *	register a function in SFE to let SFE use this function to configure flow cookie for a flow
 *
 * Hardware driver which support flow cookie should register a callback function in SFE. Then SFE
 * can use this function to configure flow cookie for a flow.
 * return: 0, success; !=0, fail
 */
int sfe_ipv6_register_flow_cookie_cb(sfe_ipv6_flow_cookie_set_func_t cb)
{
	struct sfe_ipv6 *si = &__si6;

	BUG_ON(!cb);

	if (si->flow_cookie_set_func) {
		return -1;
	}

	rcu_assign_pointer(si->flow_cookie_set_func, cb);
	return 0;
}

/*
 * sfe_ipv6_unregister_flow_cookie_cb
 *	unregister function which is used to configure flow cookie for a flow
 *
 * return: 0, success; !=0, fail
 */
int sfe_ipv6_unregister_flow_cookie_cb(sfe_ipv6_flow_cookie_set_func_t cb)
{
	struct sfe_ipv6 *si = &__si6;

	RCU_INIT_POINTER(si->flow_cookie_set_func, NULL);
	return 0;
}
#endif /*CONFIG_NF_FLOW_COOKIE*/

/*
 * sfe_ipv6_init()
 */
static int __init sfe_ipv6_init(void)
{
	struct sfe_ipv6 *si = &__si6;
	int result = -1;

	ipc_sfe_log_ctxt = ipc_log_context_create(IPCLOG_STATE_PAGES,
							"sfe_ipv6", 0);
	if (!ipc_sfe_log_ctxt)
		pr_err("error creating logging context for sfe ipv6 connection\n");
	else
		pr_info("IPC logging has been enabled for sfe ipv6 connection\n");

	DEBUG_INFO("SFE IPv6 init\n");

	/*register proc sys*/
	si->proc1.sfe_debug_ctl_path[0].procname = "debug_v6";
	si->proc1.debug_root[0].procname = "sfe_v6";
	si->proc1.debug_root[0].mode = 0555;
	si->proc1.debug_root[0].child = sfe_sysctl_debug;
	si->proc1.debug_ctl_header = register_sysctl_paths(si->proc1.sfe_debug_ctl_path, si->proc1.debug_root);

	DEBUG_INFO("SFE IPv6 init\n");

	nl_socket = netlink_kernel_create(&init_net, NL_IPV6_PROTO_ID, &nl_ipv6_cfg);

	if (!nl_socket)
	{
		DEBUG_ERROR("Error creating SFE IPV6 NL socket");
		goto exit1;
	}




	/*
	 * Create sys/sfe_ipv6
	 */
	si->sys_sfe_ipv6 = kobject_create_and_add("sfe_ipv6", NULL);
	if (!si->sys_sfe_ipv6) {
		DEBUG_ERROR("failed to register sfe_ipv6\n");
		goto exit1;
	}

	/*
	 * Create files, one for each parameter supported by this module.
	 */
	result = sysfs_create_file(si->sys_sfe_ipv6, &sfe_ipv6_debug_dev_attr.attr);
	if (result) {
		DEBUG_ERROR("failed to register debug dev file: %d\n", result);
		goto exit2;
	}

	/*
	 * Create sys/sfe_ipv6/debug_level
	 */
	result = sysfs_create_file(si->sys_sfe_ipv6, &sfe_debug_level.attr);
	if (result) {
		DEBUG_ERROR("failed debug level file: %d for ipv6 connection",
			result);
		goto exit2;
	}

	/*
	 * Create sys/sfe_ipv4/sfe_v6_enable_ipc_low
	 */
	result = sysfs_create_file(si->sys_sfe_ipv6, &sfe_debug_level_low.attr);
	if (result) {
		DEBUG_ERROR(
			"failed debug level low file: %d for ipv6 connection\n",
			result);
		goto exit2;
	}

	/*
	 * Register our debug char device.
	 */
	result = register_chrdev(0, "sfe_ipv6", &sfe_ipv6_debug_dev_fops);
	if (result < 0) {
		DEBUG_ERROR("Failed to register chrdev: %d\n", result);
		goto exit3;
	}

	si->debug_dev = result;

	/*
	 * Create sys/sfe_packet_stats_ipv6
	 */
	si->sys_sfe_ipv6_packet_stats = kobject_create_and_add("sfe_packet_stats_ipv6", NULL);
	if (!si->sys_sfe_ipv6_packet_stats) {
		DEBUG_ERROR("failed to register sfe_packet_stats_ipv6\n");
		goto exit1;
	}

	/*
	 * Create files, one for each parameter supported by this module.
	 */
	result = sysfs_create_file(si->sys_sfe_ipv6_packet_stats, &sfe_ipv6_packet_stats_dev_attr.attr);
	if (result) {
		DEBUG_ERROR("failed to register packet stat dev file: %d\n", result);
		goto exit2;
	}

	/*
	 * Register our packet stats char device.
	 */
	result = register_chrdev(0, "sfe_packet_stats_ipv6", &sfe_ipv6_packet_stats_fops);
	if (result < 0) {
		DEBUG_ERROR("Failed to register packet stats chrdev: %d\n", result);
		goto exit3;
	}

	si->packet_stats_dev = result;
	//create Hash table
	hash_init(si->packet_stats_htable);

	proc_create("ipv6_iface_name",0,NULL,&ipv6_iface_proc_fops);
	memset(si->ipv6_iface,0,MAX_INTF_LEN);
	si->iface_length=strlen(si->ipv6_iface);

	/*
	 * Create a timer to handle periodic statistics.
	 */
	setup_timer(&si->timer, sfe_ipv6_periodic_sync, (unsigned long)si);
	mod_timer(&si->timer, jiffies + ((HZ + 99) / 100));

	spin_lock_init(&si->lock);

	return 0;

exit3:
	sysfs_remove_file(si->sys_sfe_ipv6, &sfe_ipv6_debug_dev_attr.attr);
	sysfs_remove_file(si->sys_sfe_ipv6_packet_stats, &sfe_ipv6_packet_stats_dev_attr.attr);

exit2:
	kobject_put(si->sys_sfe_ipv6);
	kobject_put(si->sys_sfe_ipv6_packet_stats);
exit1:
	return result;
}

/*
 * sfe_ipv6_exit()
 */
static void __exit sfe_ipv6_exit(void)
{
	struct sfe_ipv6 *si = &__si6;

	unregister_sysctl_table(si->proc1.debug_ctl_header);
	netlink_kernel_release(nl_socket);

	DEBUG_INFO("SFE IPv6 exit\n");

	/*
	 * Destroy all connections.
	 */
	sfe_ipv6_destroy_all_rules_for_dev(NULL);
	sfe_ipv6_destroy_packet_stats_list();

	del_timer_sync(&si->timer);

	unregister_chrdev(si->debug_dev, "sfe_ipv6");

	unregister_chrdev(si->packet_stats_dev, "sfe_packet_stats_ipv6");
	sysfs_remove_file(si->sys_sfe_ipv6_packet_stats, &sfe_ipv6_packet_stats_dev_attr.attr);
	kobject_put(si->sys_sfe_ipv6_packet_stats);

	remove_proc_entry("ipv6_iface_name",NULL);

	sysfs_remove_file(si->sys_sfe_ipv6, &sfe_ipv6_debug_dev_attr.attr);

	kobject_put(si->sys_sfe_ipv6);
	if (ipc_sfe_log_ctxt != NULL)
		ipc_log_context_destroy(ipc_sfe_log_ctxt);

	if (ipc_sfe_log_ctxt_low != NULL)
		ipc_log_context_destroy(ipc_sfe_log_ctxt_low);

}

	module_init(sfe_ipv6_init)
module_exit(sfe_ipv6_exit)
	EXPORT_SYMBOL(sfe_ipv6_recv);
	EXPORT_SYMBOL(sfe_ipv6_create_rule);
	EXPORT_SYMBOL(sfe_ipv6_destroy_rule);
	EXPORT_SYMBOL(sfe_ipv6_destroy_all_rules_for_dev);
	EXPORT_SYMBOL(sfe_ipv6_register_sync_rule_callback);
	EXPORT_SYMBOL(sfe_ipv6_mark_rule);
	EXPORT_SYMBOL(sfe_ipv6_update_rule);
#ifdef CONFIG_NF_FLOW_COOKIE
	EXPORT_SYMBOL(sfe_ipv6_register_flow_cookie_cb);
	EXPORT_SYMBOL(sfe_ipv6_unregister_flow_cookie_cb);
#endif

	MODULE_AUTHOR("Qualcomm Atheros Inc.");
	MODULE_DESCRIPTION("Shortcut Forwarding Engine - IPv6 support");
	MODULE_LICENSE("Dual BSD/GPL");
