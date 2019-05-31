/*! \file pmip_msgs.c
 * \brief creates new options and sends and parses PBU/PBA
 * \author OpenAir3 Group
 * \date 12th of October 2010
 * \version 1.0
 * \company Eurecom
 * \project OpenAirInterface
 * \email: openair3@eurecom.fr
 */
#define PMIP
#define PMIP_MSGS_C
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <pmip_hnp_cache.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <netinet/ip6.h>
#include <net/ethernet.h>
#include <netinet/icmp6.h>
#include <arpa/inet.h>


//---------------------------------------------------------------------------------------------------------------------
#include "pmip_msgs.h"
#include "pmip_pcap.h"
//---------------------------------------------------------------------------------------------------------------------
#ifdef ENABLE_VT
#    include "vt.h"
#endif
#include "debug.h"

extern struct mip6_config conf;
extern int getipforclient(struct in_addr *addr);



//---------------------------------------------------------------------------------------------------------------------
/*! \var struct sock mh_sock
  \brief Global var declared in mipl component
 */
extern struct sock_def mh_sock;
extern struct sock_def mhv4_sock;

/* We can use these safely, since they are only read and never change */
static const uint8_t _pad1[1] = { 0x00 };
static const uint8_t _pad2[2] = { 0x01, 0x00 };
static const uint8_t _pad3[3] = { 0x01, 0x01, 0x00 };
static const uint8_t _pad4[4] = { 0x01, 0x02, 0x00, 0x00 };
static const uint8_t _pad5[5] = { 0x01, 0x03, 0x00, 0x00, 0x00 };
static const uint8_t _pad6[6] = { 0x01, 0x04, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t _pad7[7] = { 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00 };

//---------------------------------------------------------------------------------------------------------------------

static int pmip_cache_get_bce_mac(void *data, void *arg, void *usr_data)
	//---------------------------------------------------------------------------------------------------------------------
{
	pmip_entry_t *bce = (pmip_entry_t *) data;
	assert(bce && arg && usr_data);

	struct in6_addr * rcvd_addr = (struct in6_addr *)arg;
	struct in6_addr src_addr;
	src_addr = get_node_id(rcvd_addr);
	if(IN6_ARE_ADDR_EQUAL(&src_addr, &bce->mn_suffix))
	{
		memcpy(usr_data, (void*)&bce->mn_hw_address, sizeof(struct in6_addr));
		dbg("#pmip_cache_get_bce_mac set mac to  %x:%x:%x:%x:%x:%x:%x:%x \n",NIP6ADDR((struct in6_addr *)usr_data));
		return 1;
	}
	return 0;
}

static int pmip_cache_get_mac_from_iid(void *data, void *arg, void *usr_data)
	//---------------------------------------------------------------------------------------------------------------------
{
	pmip_entry_t *bce = (pmip_entry_t *) data;
	assert(bce && arg && usr_data);

	struct in6_addr * iid = (struct in6_addr *)arg;
	if(IN6_ARE_ADDR_EQUAL(iid, &bce->mn_suffix))
	{
		memcpy(usr_data, (void*)&bce->mn_hw_address, sizeof(struct in6_addr));
		dbg("#pmip_cache_get_bce_mac set mac to  %x:%x:%x:%x:%x:%x:%x:%x \n",NIP6ADDR((struct in6_addr *)usr_data));
		return 1;
	}
	return 0;
}


static int create_opt_pad(struct iovec *iov, int pad)
{
	if (pad == 2) {
		//iov->iov_base = (void *) _pad2;
		iov->iov_base = malloc(sizeof(_pad2));
		memcpy(iov->iov_base, _pad2, sizeof(_pad2));
	}
	else if (pad == 4) {
		//iov->iov_base = (void *) _pad4;
		iov->iov_base = malloc(sizeof(_pad4));
		memcpy(iov->iov_base, _pad4, sizeof(_pad4));
	}
	else if (pad == 6) {
		//iov->iov_base = (void *) _pad6;
		iov->iov_base = malloc(sizeof(_pad6));
		memcpy(iov->iov_base, _pad6, sizeof(_pad6));
	}
	/* Odd pads do not occur with current spec, so test them last */
	else if (pad == 1) {
		iov->iov_base = (void *) _pad1;
		iov->iov_base = malloc(sizeof(_pad1));
		memcpy(iov->iov_base, _pad1, sizeof(_pad1));
	}
	else if (pad == 3) {
		//iov->iov_base = (void *) _pad3;
		iov->iov_base = malloc(sizeof(_pad3));
		memcpy(iov->iov_base, _pad3, sizeof(_pad3));
	}
	else if (pad == 5) {
		//iov->iov_base = (void *) _pad5;
		iov->iov_base = malloc(sizeof(_pad5));
		memcpy(iov->iov_base, _pad5, sizeof(_pad5));
	}
	else if (pad == 7) {
		//iov->iov_base = (void *) _pad7;
		iov->iov_base = malloc(sizeof(_pad7));
		memcpy(iov->iov_base, _pad7, sizeof(_pad7));
	}
	iov->iov_len = pad;
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
static inline int optpad(int xn, int y, int offset)
{
	return ((y - offset) & (xn - 1));
}
//---------------------------------------------------------------------------------------------------------------------
static int mh_try_pad(const struct iovec *in, struct iovec *out, int count)
{
	size_t len = 0;
	int m, n = 1, pad = 0;
	struct ip6_mh_opt *opt;
	out[0].iov_len = in[0].iov_len;
	out[0].iov_base = in[0].iov_base;
	len += in[0].iov_len;
	for (m = 1; m < count; m++) {
		opt = (struct ip6_mh_opt *) in[m].iov_base;
		switch (opt->ip6mhopt_type) {
			case IP6_MHOPT_BREFRESH:
				pad = optpad(2, 0, len);    /* 2n */
				break;
			case IP6_MHOPT_ALTCOA:
				pad = optpad(8, 6, len);    /* 8n+6 */
				break;
			case IP6_MHOPT_NONCEID:
				pad = optpad(2, 0, len);    /* 2n */
				break;
			case IP6_MHOPT_BAUTH:
				pad = optpad(8, 2, len);    /* 8n+2 */
				break;
			case IP6_MHOPT_MOBILE_NODE_IDENTIFIER:
				pad = 0;    /* This option does not have any alignment requirements. */
				break;
			case IP6_MHOPT_HOME_NETWORK_PREFIX:
				pad = optpad(8, 4, len);    /* 8n+4 */
				break;
			case IP6_MHOPT_HANDOFF_INDICATOR:
				pad = 0;    /* The Handoff Indicator option has no alignment requirement. */
				break;
			case IP6_MHOPT_ACCESS_TECHNOLOGY_TYPE:
				pad = 0;    /* The Access Technology Type Option has no alignment requirement. */
				break;
			case IP6_MHOPT_MOBILE_NODE_LINK_LAYER_IDENTIFIER:
				pad = optpad(8, 0, len);    /* 8n */
				break;
			case IP6_MHOPT_LINK_LOCAL_ADDRESS:
				pad = optpad(8, 6, len);    /* 8n+6 */
				break;
			case IP6_MHOPT_TIME_STAMP:
				pad = optpad(8, 2, len);    /* 8n+2 */
				break;

			case IPV4_HOME_ADDR_REQUEST_OPTION:
				pad = optpad(4, 0, len);    /* 4n */
				break;

			case IPV4_HOME_ADDR_REPLY_OPTION:
				pad = optpad(4, 0, len);    /* 4n */
				break;
			case IPV4_HOME_DEFAULT_ROUTER_OPTION:
				pad = optpad(4, 0, len);    /* 4n */
				break;

			case IPV4_DMNP_PREFIX_OPTION:
				pad = optpad(8, 2, len);    /* 8n+2 */
				break;

			case IP6_MHOPT_GRE_KEY:
				pad = optpad(4, 0, len);	/* 4n */
				break;



		}
		if (pad > 0) {
			create_opt_pad(&out[n++], pad);
			len += pad;
		}
		len += in[m].iov_len;
		out[n].iov_len = in[m].iov_len;
		out[n].iov_base = in[m].iov_base;
		n++;
	}
	dbg("making lenght multiple of octate\n");
		pad = optpad(8, 0, len);
		create_opt_pad(&out[n++], pad);
	return n;
}
//---------------------------------------------------------------------------------------------------------------------
static size_t mh_length(struct iovec *vec, int count)
{
	size_t len = 0;
	int i;
	for (i = 0; i < count; i++) {
		len += vec[i].iov_len;
	}
	return len;
}
//---------------------------------------------------------------------------------------------------------------------
void init_pbu_sequence_number(void) {
	g_mag_sequence_number = 0;
}
//---------------------------------------------------------------------------------------------------------------------
int get_new_pbu_sequence_number(void)
{
	// TO DO: should be thread protected
	int seq = g_mag_sequence_number;
	g_mag_sequence_number = g_mag_sequence_number + 1;
	return seq;
}
//---------------------------------------------------------------------------------------------------------------------
int is_pba_is_response_to_valid_pbu_seq_number(msg_info_t * pba_info, pmip_entry_t *bce)
{
	struct pending_pbu_seq_list *tmp = NULL;
	int ret = 0 ;
	if ( get_pbu_seq_node(pba_info->seqno) != NULL) {
		//delete the node from list
		ret = pending_pbu_seq_list_del(pba_info->seqno);
		if (ret < 0)
			dbg("Node with sequence %d failed\n",pba_info->seqno);
		if ((memcmp(pba_info->timestamp.first,bce->timestamp.first,sizeof(bce->timestamp.first)) == 0) && (pba_info->timestamp.second == bce->timestamp.second)) {
			if (IN6_IS_ADDR_UNSPECIFIED(&bce->mn_prefix) || IN6_ARE_ADDR_EQUAL(&pba_info->mn_prefix, &bce->mn_prefix)) {
				if (IN6_ARE_ADDR_EQUAL(&pba_info->mn_iid, &bce->mn_suffix)) {
					dbg("Correct PBA received\n");
					return 1;
				} else {
					dbg("Not identical Mobile Node Link-layer Identifier Option: PBU:%x:%x:%x:%x:%x:%x:%x:%x  PBA:%x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&bce->mn_suffix), NIP6ADDR(&pba_info->mn_iid));
				}
			} else {
				dbg("Not identical Home Network Prefix option: PBU:%x:%x:%x:%x:%x:%x:%x:%x  PBA:%x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&bce->mn_prefix), NIP6ADDR(&pba_info->mn_prefix));
			}
		} else {
			dbg("Not identical Timestamp option: PBU:%08X%08X  PBA:%08X%08X\n", bce->timestamp.first, bce->timestamp.second, pba_info->timestamp.first, pba_info->timestamp.second);
		}
	} else {
		dbg("Sequence Number: PBU:%d does not exits in list\n",pba_info->seqno);
	}
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
struct in6_addr get_node_id(struct in6_addr *mn_addr)
{
	struct in6_addr result;
	result = in6addr_any;
	memcpy(&result.s6_addr32[2], &mn_addr->s6_addr32[2], sizeof(ip6mnid_t));
	return result;
}
//---------------------------------------------------------------------------------------------------------------------
struct in6_addr get_node_prefix(struct in6_addr *mn_addr)
{
	struct in6_addr result;
	result = in6addr_any;
	memcpy(&result.s6_addr32[0], &mn_addr->s6_addr32[0], PREFIX_LENGTH / 8);
	return result;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_home_net_prefix(struct iovec *iov, struct in6_addr *Home_Network_Prefix)
{
	/* From RFC 5213
	 * 8.3. Home Network Prefix Option


	 A new option, Home Network Prefix option is defined for use with the
	 Proxy Binding Update and Proxy Binding Acknowledgement messages
	 exchanged between a local mobility anchor and a mobile access
	 gateway.  This option is used for exchanging the mobile node's home
	 network prefix information.  There can be multiple Home Network
	 Prefix options present in the message.

	 The Home Network Prefix Option has an alignment requirement of 8n+4.
	 Its format is as follows:

	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |      Type     |   Length      |   Reserved    | Prefix Length |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                                                               |
	 +                                                               +
	 |                                                               |
	 +                    Home Network Prefix                        +
	 |                                                               |
	 +                                                               +
	 |                                                               |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	 Type
	 22

	 Length

	 8-bit unsigned integer indicating the length of the option
	 in octets, excluding the type and length fields.  This field
	 MUST be set to 18.

	 Reserved (R)

	 This 8-bit field is unused for now.  The value MUST be
	 initialized to 0 by the sender and MUST be ignored by the
	 receiver.

	 Prefix Length

	 8-bit unsigned integer indicating the prefix length of the
	 IPv6 prefix contained in the option.

	 Home Network Prefix

	 A sixteen-byte field containing the mobile node's IPv6 Home
	 Network Prefix.
	 */
	ip6_mh_opt_home_net_prefix_t *opt;
	size_t optlen = sizeof(ip6_mh_opt_home_net_prefix_t);
	iov->iov_base = malloc(optlen);
	iov->iov_len = optlen;
	if (iov->iov_base == NULL)
		return -ENOMEM;
	opt = (ip6_mh_opt_home_net_prefix_t *) iov->iov_base;
	opt->ip6hnp_type       = IP6_MHOPT_HOME_NETWORK_PREFIX;
	opt->ip6hnp_len        = 18;
	opt->ip6hnp_reserved   = 0;
	opt->ip6hnp_prefix_len = 128;   //128 bits
	opt->ip6hnp_prefix     = *Home_Network_Prefix;
	return 0;
}

__u16 mask_to_int_format(struct in_addr mask)
{
	uint32_t val;
	val = mask.s_addr;
	__u16 i = 0;
	while (val) {
		i += val &1 ;
		val = val >> 1;
	}
	dbg ("Mask for subnet %s is %u\n",inet_ntoa(mask), i);
	return i;
}

//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_v4_home_addr_request_option(struct iovec *iov, struct in_addr requested_ip)
{
	/* From RFC 5544
	 *3.3.1. IPv4 Home Address Request Option


	 A new option, the IPv4 Home Address Request option, is defined for
	 use with the Proxy Binding Update message sent by the mobile access
	 gateway to the local mobility anchor. This option is used to request
	 IPv4 home address assignment for the mobile node.
	 The IPv4 Home Address Request option has an alignment requirement of
	 4n. Its format is as follows:


	 0 1 2 3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 | Type | Length |Prefix-len | Reserved |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 | IPv4 home address |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 Figure 3: IPv4 Home Address Request Option



	 Type
	 36

	 Length
	 An 8-bit unsigned integer indicating the length of the option
	 in octets, excluding the Type and Length fields. This field
	 MUST be set to (6).

	 Prefix-len
	 This 6-bit unsigned integer indicating the prefix length of the
	 mobile node’s IPv4 home network corresponding to the IPv4 home
	 address contained in the option.

	 Reserved
	 This 10-bit field is unused for now. The value MUST be
	 initialized to (0) by the sender and MUST be ignored by the
	 receiver.

	 IPv4 home address
	 This 4-byte field containing the IPv4 home address that is
	 being requested. The value of 0.0.0.0 is used to request that

	 */
	ip_mh_opt_v4_home_addr_request_option_t *opt;
	__u16 i;
	struct in_addr addr;
	addr.s_addr = 0;
	if (requested_ip.s_addr == 0) {
		dbg("requested ip is 0 so get random Ip\n");
		if (getipforclient(&addr)) {
			dbg ("getipforclient success \n");
		} else {
			dbg ("getipforclient failed . Putting 0 in requested IP\n");
			addr.s_addr = 0;
		}

	} else {
		dbg("requested ip is not 0 . dhcp request with ip =%s \n",inet_ntoa(requested_ip));
		addr = requested_ip;
	}
	size_t optlen = sizeof(ip_mh_opt_v4_home_addr_request_option_t);
	iov->iov_base = malloc(optlen);
	iov->iov_len = optlen;
	if (iov->iov_base == NULL)
		return -ENOMEM;
	opt = (ip_mh_opt_v4_home_addr_request_option_t *) iov->iov_base;
	opt->ip4hnp_type= IPV4_HOME_ADDR_REQUEST_OPTION;
	opt->ip4_opt_len = 6;
	i = mask_to_int_format(conf.dhcp_sub_mask);
	opt->plen = (i << 2);
	opt->ip4hnp_prefix= addr; //sending IP
	dbg("added ipv4 home option sizeof struct option =%u & setted the prefix as 0\n",sizeof (struct ip_mh_opt_v4_home_addr_request_option_t));

	dbg("option whole content val opt->var.plen=%d prefix val=%d\n",opt->plen,((opt->plen) >> 2));
	return 0;
}

//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_v4_dmnp_option(struct iovec *iov, struct in_addr *dmnp_prefix, __u8 *pre_len)
{
	/* From RFC 5213
	 * 8.3. DMNP prefix  option


	 0 1 2 3
	 0 1 2 3 4 5 6 7  8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 +-+-+-+-+-+- +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 | Type            | Length |V| Reserved | Prefix Length |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 | |
	 + +
	 | |
	 . .
	 + IPv4 or IPv6 Delegated Mobile Network Prefix +
	 | (DMNP) |
	 + +
	 | |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	 Type
	 55

	 Length
	 8-bit unsigned integer indicating the length of the option in
	 octets, excluding the Type and Length fields.

	 IPv4 Prefix (V)
	 If the IPv4 Prefix (V) flag is set to a value of (1), then it
	 indicates that the prefix that is included in the DMNP field is an
	 IPv4 prefix. If the IPv4 Prefix (V) flag is set to a value of
	 (0), then it indicates that the prefix that is included in the
	 DMNP field is an IPv6 prefix.

	 Reserved
	 This field is unused for now. The value MUST be initialized to 0
	 by the sender and MUST be ignored by the receiver.

	 Prefix Length
	 8-bit unsigned integer indicating the number of leftmost bits
	 covering the network part of the address contained in the Prefix
	 field.

	 Delegated Mobile Network Prefix
	 Contains a mobile router’s 4-byte IPv4 or a 16-byte IPv6 Delegated
	 Mobile Network Prefix.


	 */
	ip_mh_opt_v4_dmnp_prefix_option_t *opt;
	size_t optlen = sizeof(ip_mh_opt_v4_dmnp_prefix_option_t);
	iov->iov_base = malloc(optlen);
	iov->iov_len = optlen;
	if (iov->iov_base == NULL)
		return -ENOMEM;
	opt = (ip_mh_opt_v4_dmnp_prefix_option_t *) iov->iov_base;
	opt->ip4hnp_type= IPV4_DMNP_PREFIX_OPTION;
	opt->ip4_opt_len = optlen - (sizeof(opt->ip4hnp_type) + (sizeof(opt->ip4_opt_len)));
	dbg("DMNP debug whole size=%u , minor =%u\n",optlen,opt->ip4_opt_len );
	//bug Need to fix prefix lenght
	opt->var.isv4dmnp= 1;
	opt->var.res = 0;
	opt->dmnp_prefix= *dmnp_prefix;
	opt->prefix_len= *pre_len;
	dbg("added ipv4 DMNP prefix =%u\n",sizeof (struct ip_mh_opt_v4_dmnp_prefix_option_t));
	return 0;
}



//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_handoff_indicator(struct iovec *iov, int handoff_indicator)
{
	/* From RFC 5213
	   8.4. Handoff Indicator Option


	   A new option, Handoff Indicator option is defined for use with the
	   Proxy Binding Update and Proxy Binding Acknowledgement messages
	   exchanged between a local mobility anchor and a mobile access
	   gateway.  This option is used for exchanging the mobile node's
	   handoff-related hints.

	   The Handoff Indicator option has no alignment requirement.  Its
	   format is as follows:

	   0                   1                   2                   3
	   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |      Type     |   Length      |  Reserved (R) |       HI      |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	   Type
	   23

	   Length

	   8-bit unsigned integer indicating the length of the option
	   in octets, excluding the type and length fields.  This field
	   MUST be set to 2.

	   Reserved (R)

	   This 8-bit field is unused for now.  The value MUST be
	   initialized to 0 by the sender and MUST be ignored by the
	   receiver.

	   Handoff Indicator (HI)

	   An 8-bit field that specifies the type of handoff.  The values
	   (0 - 255) will be allocated and managed by IANA.  The following
	   values are currently defined.

0: Reserved
1: Attachment over a new interface
2: Handoff between two different interfaces of the mobile node
3: Handoff between mobile access gateways for the same interface
4: Handoff state unknown
5: Handoff state not changed (Re-registration)

	 */
	ip6_mh_opt_handoff_indicator_t *opt;
	size_t optlen = sizeof(ip6_mh_opt_handoff_indicator_t);
	iov->iov_base = malloc(optlen);
	iov->iov_len = optlen;
	if (iov->iov_base == NULL)
		return -ENOMEM;
	opt = (ip6_mh_opt_handoff_indicator_t *) iov->iov_base;
	opt->ip6hi_type     = IP6_MHOPT_HANDOFF_INDICATOR;
	opt->ip6hi_len      = 2;  //set to 2 bytes
	opt->ip6hi_reserved = 0;
	opt->ip6hi_hi       = (__u8)handoff_indicator;
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------

int mh_create_opt_gre(struct iovec *iov, uint8_t key_valid, uint32_t key)
{
	/* Custom option for GRE key exchange

	   KEY GRE
	   =========
	   GRE Key Option Follows:
Type: 0x21
Length: 0x06
Reserved: 0x0000
GRE Key Identifier: 0x00000001 (Example)

KEYLESS GRE
===========
GRE Key Option Follows:
Type: 0x21
Length: 0x02
Reserved: 0x0000


The Handoff Indicator option has no alignment requirement.  Its
format is as follows:

0                   1                   2                   4
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|      Type     |   Length      |  Reserved (R) |       GRE Key      |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

Type
33

Length

8-bit unsigned integer indicating the length of the option
in octets, excluding the type and length fields.  This field
MUST be set to 2 and if key is included 6

Reserved (R)

This 16-bit field is unused for now.  The value MUST be
initialized to 0 by the sender and MUST be ignored by the
receiver.

Handoff Indicator (HI)

An 32-bit field that specifies the key to used in data communication

Key value to be used

	 */
	size_t optlen;
	ip6_mh_opt_gre_key_t *opt;
	if(key_valid == 1)
	{
		optlen = sizeof(ip6_mh_opt_gre_key_t);
	}
	else
	{/*do not include key value*/
		optlen = (sizeof(ip6_mh_opt_gre_key_t) - sizeof(__u32));
	}
	iov->iov_base = malloc(optlen);
	iov->iov_len = optlen;
	if (iov->iov_base == NULL)
		return -ENOMEM;
	opt = (ip6_mh_opt_gre_key_t *) iov->iov_base;
	opt->ip6gre_type     = IP6_MHOPT_GRE_KEY;
	opt->ip6gre_len      = (key_valid?IP6_MH_OPT_GRE_KEY_LEN:IP6_MH_OPT_GRE_KEYLESS_LEN);
	opt->ip6gre_reserved = 0;
	if(key_valid)
		opt->ip6gre_key = (__u32)htonl(key);
	return 0;
}

int mh_create_opt_dmnp(struct iovec *iov, struct in_addr *dmnp_pref, uint8_t *dmnp_pref_len)
{
	/* From RFC 5213
	 * 8.3. DMNP prefix	option


	 0 1 2 3
	 0 1 2 3 4 5 6 7  8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 +-+-+-+-+-+- +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 | Type			  | Length |V| Reserved | Prefix Length |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 | |
	 + +
	 | |
	 . .
	 + IPv4 or IPv6 Delegated Mobile Network Prefix +
	 | (DMNP) |
	 + +
	 | |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	 Type
	 55

	 Length
	 8-bit unsigned integer indicating the length of the option in
	 octets, excluding the Type and Length fields.

	 IPv4 Prefix (V)
	 If the IPv4 Prefix (V) flag is set to a value of (1), then it
	 indicates that the prefix that is included in the DMNP field is an
	 IPv4 prefix. If the IPv4 Prefix (V) flag is set to a value of
	 (0), then it indicates that the prefix that is included in the
	 DMNP field is an IPv6 prefix.

	 Reserved
	 This field is unused for now. The value MUST be initialized to 0
	 by the sender and MUST be ignored by the receiver.

	 Prefix Length
	 8-bit unsigned integer indicating the number of leftmost bits
	 covering the network part of the address contained in the Prefix
	 field.

	 Delegated Mobile Network Prefix
	 Contains a mobile router’s 4-byte IPv4 or a 16-byte IPv6 Delegated
	 Mobile Network Prefix.


	 */

	size_t optlen;
	ip_mh_opt_v4_dmnp_prefix_option_t *opt;
	//ip6_mh_opt_gre_key_t *opt;

	optlen = sizeof(ip_mh_opt_v4_dmnp_prefix_option_t);
	iov->iov_base = malloc(optlen);
	iov->iov_len = optlen;
	if (iov->iov_base == NULL)
		return -ENOMEM;
	opt = (ip_mh_opt_v4_dmnp_prefix_option_t *) iov->iov_base;

	opt->ip4hnp_type= IPV4_DMNP_PREFIX_OPTION;
	opt->ip4_opt_len = optlen - (sizeof(opt->ip4hnp_type) + (sizeof(opt->ip4_opt_len)));
	dbg("DMNP debug whole size=%u , minor =%u\n",optlen,opt->ip4_opt_len );
	//bug Need to fix prefix lenght
	opt->var.isv4dmnp= 1;
	opt->var.res = 0;
	opt->dmnp_prefix= *dmnp_pref;
	opt->prefix_len= *dmnp_pref_len;
	dbg("mh_create_opt_dmnp added ipv4 DMNP prefix =%u\n",sizeof (struct ip_mh_opt_v4_dmnp_prefix_option_t));
	return 0;
}

int mh_create_opt_v4_home_adrr_reply(struct iovec *iov, struct in_addr *ip_request,uint8_t *prefix_len)
{
	/* From RFC 5844
	 *    3.3.2. IPv4 Home Address Reply Option


	 A new option, the IPv4 Home Address Reply option, is defined for use
	 in the Proxy Binding Acknowledgement message sent by the local
	 mobility anchor to the mobile access gateway. This option can be
	 used to send the assigned mobile node’s IPv4 home address.
	 The IPv4 Home Address Reply option has an alignment requirement of
	 4n. Its format is as follows:


	 0 1 2 3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 | Type | Length | Status |Pref-len |Res|
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 | IPv4 home address |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+



	 Type
	 37

	 Length
	 An 8-bit unsigned integer indicating the length of the option
	 in octets, excluding the Type and Length fields. This field
	 MUST be set to (6).


	 Status
	 Indicates success or failure for the IPv4 home address
	 assignment. Values from 0 to 127 indicate success. Higher
	 values (128 to 255) indicate failure. The following Status
	 values are currently allocated by this document:

	 0 		Success
	 128 	Failure, reason unspecified
	 129 	Administratively prohibited
	 130 	Incorrect IPv4 home address
	 131 	Invalid IPv4 address
	 132 	Dynamic IPv4 home address assignment not available

	 Prefix-len
	 This 6-bit unsigned integer is used to carry the prefix length
	 of the mobile node’s IPv4 home network corresponding to the
	 IPv4 home address contained in the option.

	 Reserved (Res)
	 This 2-bit field is unused for now. The value MUST be
	 initialized to (0) by the sender and MUST be ignored by the
	 receiver.


	 IPv4 home address
	 This 4-byte field is used to c


	 */


	ip_mh_opt_v4_home_addr_reply_option_t *opt;
	struct in_addr addr;
	addr.s_addr = 0;
	size_t optlen = sizeof(ip_mh_opt_v4_home_addr_reply_option_t);
	iov->iov_base = malloc(optlen);
	iov->iov_len = optlen;
	if (iov->iov_base == NULL)
		return -ENOMEM;
	opt = (ip_mh_opt_v4_home_addr_reply_option_t *) iov->iov_base;
	opt->ip4hnp_type= IPV4_HOME_ADDR_REPLY_OPTION;
	opt->ip4_opt_len = 6;
	opt->status = 0;
	//bug Need to fix prefix lenght
	opt->plen = *prefix_len;
	//opt->var.plen = 24 ;
	//opt->ip4hnp_prefix= *Home_Network_IP;
	//getting Ip address
	if (ip_request->s_addr == 0) {
		dbg("requested IP is 0 . so LMA will create random IP\n");
		memset(&addr, 0, sizeof(struct in_addr));
		//get ip address for client
		getipforclient(&addr);
	} else {
		addr= *ip_request;
		dbg("requested IP is =%s\n",inet_ntoa(*ip_request));
	}
	dbg ("mh_create_opt_v4_home_adrr %s ip for connection \n",inet_ntoa(addr));
	opt->ip4hnp_prefix= addr;

	return 0;
}

int mh_create_opt_v4_default_router_opt(struct iovec *iov)
{
	/* From RFC 5844
	 *3.3.3. IPv4 Default-Router Address Option


	 A new option, the IPv4 Default-Router Address option, is defined for
	 use in the Proxy Binding Acknowledgement message sent by the local
	 mobility anchor to the mobile access gateway. This option can be
	 used to send the mobile node’s IPv4 default router address.
	 The IPv4 Default-Router Address option has an alignment requirement
	 of 4n. Its format is as follows:



	 0 1 2 3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 | Type | Length | Reserved (R) |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 | IPv4 Default-Router Address |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+



	 Type
	 38

	 Length
	 An 8-bit unsigned integer indicating the length of the option
	 in octets, excluding the Type and Length fields. This field
	 MUST be set to (6).

	 Length
	 An 8-bit unsigned integer indicating the length of the option
	 in octets, excluding the Type and Length fields. This field
	 MUST be set to (6).

	 Reserved (R)
	 This 16-bit field is unused for now. The value MUST be
	 initialized to (0) by the sender and MUST be ignored by the
	 receiver.

	 IPv4 Default-Router Address
	 A 4-byte field containing the mobile node’s default router
	 address.




	 */


	ip_mh_opt_v4_default_router_addr_option_t *opt;
	size_t optlen = sizeof(ip_mh_opt_v4_default_router_addr_option_t);
	iov->iov_base = malloc(optlen);
	iov->iov_len = optlen;
	if (iov->iov_base == NULL)
		return -ENOMEM;
	opt = (ip_mh_opt_v4_default_router_addr_option_t *) iov->iov_base;
	opt->ip4hnp_type= IPV4_HOME_DEFAULT_ROUTER_OPTION;
	opt->ip4_opt_len = 6;
	//bug Need to fix prefix lenght
	//opt->var.plen = *pre_len; //24 bit
	opt->res = 0;
	//opt->ip4hnp_prefix= *Home_Network_IP;
	//getting Ip address
	dbg ("mh_create_opt_v4_default_router_opt %s ip for connection \n","0.0.0.0");
	inet_aton("0.0.0.0", &opt->default_router_addr);

	return 0;
}





int mh_create_opt_access_technology_type(struct iovec *iov, int att)
{
	/* From RFC 5213
	   8.5. Access Technology Type Option


	   A new option, Access Technology Type option is defined for use with
	   the Proxy Binding Update and Proxy Binding Acknowledgement messages
	   exchanged between a local mobility anchor and a mobile access
	   gateway.  This option is used for exchanging the type of the access
	   technology by which the mobile node is currently attached to the
	   mobile access gateway.


	   The Access Technology Type Option has no alignment requirement.  Its
	   format is as follows:

	   0                   1                   2                   3
	   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |      Type     |   Length      |  Reserved (R) |      ATT      |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	   Type
	   24

	   Length

	   8-bit unsigned integer indicating the length of the option
	   in octets, excluding the type and length fields.  This field
	   MUST be set to 2.

	   Reserved (R)

	   This 8-bit field is unused for now.  The value MUST be
	   initialized to 0 by the sender and MUST be ignored by the
	   receiver.

	   Access Technology Type (ATT)

	   An 8-bit field that specifies the access technology through
	   which the mobile node is connected to the access link on the
	   mobile access gateway.

	   The values (0 - 255) will be allocated and managed by IANA.  The
	   following values are currently reserved for the below specified
	   access technology types.

0: Reserved         ("Reserved")
1: Virtual          ("Logical Network Interface")
2: PPP              ("Point-to-Point Protocol")
3: IEEE 802.3       ("Ethernet")
4: IEEE 802.11a/b/g ("Wireless LAN")
5: IEEE 802.16e     ("WIMAX")
	 */
	ip6_mh_opt_access_technology_type_t *opt;
	size_t optlen = sizeof(ip6_mh_opt_access_technology_type_t);
	iov->iov_base = malloc(optlen);
	iov->iov_len = optlen;
	if (iov->iov_base == NULL)
		return -ENOMEM;
	opt = (ip6_mh_opt_access_technology_type_t *) iov->iov_base;
	opt->ip6att_type     = IP6_MHOPT_ACCESS_TECHNOLOGY_TYPE;
	opt->ip6att_len      = 2;  //set to 2 bytes
	opt->ip6att_reserved = 0;
	opt->ip6att_att      = (__u8)att;
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_mobile_node_identifier(struct iovec *iov, int subtype, ip6mnid_mac_t * MN_ID)
{
	/* From RFC 4283
	   3. Mobile Node Identifier Option


	   The Mobile Node Identifier option is a new optional data field that
	   is carried in the Mobile IPv6-defined messages that includes the
	   Mobility header.  Various forms of identifiers can be used to
	   identify a Mobile Node (MN).  Two examples are a Network Access
	   Identifier (NAI) [RFC4282] and an opaque identifier applicable to a
	   particular application.  The Subtype field in the option defines the
	   specific type of identifier.

	   This option can be used in mobility messages containing a mobility
	   header.  The subtype field in the option is used to interpret the
	   specific type of identifier.

	   0                   1                   2                   3
	   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |  Option Type  | Option Length |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |  Subtype      |          Identifier ...
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	   Option Type:
	   MN-ID-OPTION-TYPE has been assigned value 8 by the IANA.  It is
	   an  8-bit identifier of the type mobility option.

	   Option Length:
	   8-bit unsigned integer, representing the length in octets of
	   the Subtype and Identifier fields.

Subtype:
Subtype field defines the specific type of identifier included
in the Identifier field.

Identifier:
A variable length identifier of type, as specified by the
Subtype field of this option.


This option does not have any alignment requirements.

	 */
	char macStr[MAC_ADDR_NUM_CHARS] = {0};
	mac_addr_ntop(MN_ID, (char *)&macStr);
	ip6_mh_opt_mobile_node_identifier_t *opt;
	size_t optlen;
	iov->iov_base = malloc(sizeof(ip6_mh_opt_mobile_node_identifier_t));
	if (iov->iov_base == NULL)
		return -ENOMEM;
	opt = (ip6_mh_opt_mobile_node_identifier_t *) iov->iov_base;
	memset(opt, 0, sizeof(ip6_mh_opt_mobile_node_identifier_t));

	if (conf.mobile_node_identifier_type == MOBILE_NODE_IDENTIFIER_TYPE_MAC)
	{
		dbg("MAC address identifier type & mh_create_opt_mobile_node_identifier mac %s\n",macStr);
		optlen = ((sizeof(ip6_mh_opt_mobile_node_identifier_t) - sizeof(opt->ip6mnid_identifier)) + strlen(macStr));
		memcpy (opt->ip6mnid_identifier, macStr, (MAC_ADDR_NUM_CHARS -1));
	}
	else if (conf.mobile_node_identifier_type == MOBILE_NODE_IDENTIFIER_TYPE_STRING)
	{
		dbg("MN Identifier type is string\n");
		optlen = ((sizeof(ip6_mh_opt_mobile_node_identifier_t) - sizeof(opt->ip6mnid_identifier)) + strlen(conf.MnIdString));
		memcpy (opt->ip6mnid_identifier, conf.MnIdString, strlen(conf.MnIdString));
		dbg("Identifier string =%s=\n",opt->ip6mnid_identifier);
	}
	else
	{
		dbg("INVALID identifier type\n");
		return -1;
	}
	iov->iov_len = optlen;
	opt->ip6mnid_type    = IP6_MHOPT_MOBILE_NODE_IDENTIFIER;
	opt->ip6mnid_len = (strlen(opt->ip6mnid_identifier) + 1);
	opt->ip6mnid_subtype = subtype;
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_mobile_node_link_layer_identifier(struct iovec *iov, ip6mnid_t * mnlli)
{
	/* From RFC 5213
	 * 8.6. Mobile Node Link-layer Identifier Option


	 A new option, Mobile Node Link-layer Identifier option is defined for
	 use with the Proxy Binding Update and Proxy Binding Acknowledgement
	 messages exchanged between a local mobility anchor and a mobile
	 access gateway.  This option is used for exchanging the mobile node's
	 link-layer identifier.

	 The format of the Link-layer Identifier option is shown below.  Based
	 on the size of the identifier, the option MUST be aligned
	 appropriately, as per mobility option alignment requirements
	 specified in [RFC3775].

	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |   Type        |    Length     |          Reserved             |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 |                                                               |
	 +                        Link-layer Identifier                  +
	 .                              ...                              .
	 |                                                               |
	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	 Type
	 25

	 Length
	 8-bit unsigned integer indicating the length of the option
	 in octets, excluding the type and length fields.

	 Reserved

	 This field is unused for now.  The value MUST be initialized to
	 0 by the sender and MUST be ignored by the receiver.

	 Link-layer Identifier

	 A variable length field containing the mobile node's link-layer
	 identifier.

	 The content and format of this field (including byte and bit
	 ordering) is as specified in Section 4.6 of [RFC4861] for
	 carrying link-layer addresses.  On certain access links, where
	 the link-layer address is not used or cannot be determined,
	 this option cannot be used.


	 */
	ip6_mh_opt_mobile_node_link_layer_identifier_t *opt;
	size_t optlen = sizeof(ip6_mh_opt_mobile_node_link_layer_identifier_t);
	iov->iov_base = malloc(optlen);
	iov->iov_len = optlen;
	if (iov->iov_base == NULL)
		return -ENOMEM;
	opt = (ip6_mh_opt_mobile_node_link_layer_identifier_t *) iov->iov_base;
	opt->ip6mnllid_type     = IP6_MHOPT_MOBILE_NODE_LINK_LAYER_IDENTIFIER;
	opt->ip6mnllid_len      = 10;  //set to 10 bytes.
	opt->ip6mnllid_reserved = 0;
	opt->ip6mnllid_lli      = *mnlli;
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_time_stamp(struct iovec *iov, ip6ts_t * Timestamp)
{
	/* From RFC 5213
	   8.8. Timestamp Option


	   A new option, Timestamp option is defined for use in the Proxy
	   Binding Update and Proxy Binding Acknowledgement messages.

	   The Timestamp option has an alignment requirement of 8n+2.  Its
	   format is as follows:

	   0                   1                   2                   3
	   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |      Type     |   Length      |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                                                               |
	   +                          Timestamp                            +
	   |                                                               |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	   Type
	   27

	   Length

	   8-bit unsigned integer indicating the length in octets of
	   the option, excluding the type and length fields.  The value
	   for this field MUST be set to 8.

	   Timestamp

	   A 64-bit unsigned integer field containing a timestamp.  The
	   value indicates the number of seconds since January 1, 1970,
00:00 UTC, by using a fixed point format.  In this format, the
integer number of seconds is contained in the first 48 bits of
the field, and the remaining 16 bits indicate the number of
1/65536 fractions of a second.
	 */
	ip6_mh_opt_time_stamp_t *opt;
	size_t optlen = sizeof(ip6_mh_opt_time_stamp_t);
	iov->iov_base = malloc(optlen);
	iov->iov_len  = optlen;
	if (iov->iov_base == NULL)
		return -ENOMEM;
	opt = (ip6_mh_opt_time_stamp_t *) iov->iov_base;
	opt->ip6mots_type = IP6_MHOPT_TIME_STAMP;
	opt->ip6mots_len  = 8;   // set to 8 bytes.
	opt->time_stamp   = *Timestamp;
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_link_local_address(struct iovec *iov, struct in6_addr *lla)
{
	/* From RFC 5213
	   8.7. Link-local Address Option


	   A new option, Link-local Address option is defined for use with the
	   Proxy Binding Update and Proxy Binding Acknowledgement messages
	   exchanged between a local mobility anchor and a mobile access
	   gateway.  This option is used for exchanging the link-local address
	   of the mobile access gateway.

	   The Link-local Address option has an alignment requirement of 8n+6.
	   Its format is as follows:

	   0                   1                   2                   3
	   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |   Type        |    Length     |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                                                               |
	   +                                                               +
	   |                                                               |
	   +                  Link-local Address                           +
	   |                                                               |
	   +                                                               +
	   |                                                               |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	   Type
	   26

	   Length

	   8-bit unsigned integer indicating the length of the option
	   in octets, excluding the type and length fields.  This field
	   MUST be set to 16.

	   Link-local Address

	   A sixteen-byte field containing the link-local address.
	 */
	ip6_mh_opt_link_local_address_t *opt;
	size_t optlen = sizeof(ip6_mh_opt_link_local_address_t);
	iov->iov_base = malloc(optlen);
	iov->iov_len = optlen;
	if (iov->iov_base == NULL)
		return -ENOMEM;
	opt = (ip6_mh_opt_link_local_address_t *) iov->iov_base;
	opt->ip6link_type = IP6_MHOPT_LINK_LOCAL_ADDRESS;
	opt->ip6link_len  = 16;  //set to 16 bytes
	opt->ip6link_addr = *lla;
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_dst_mn_addr(struct iovec *iov, struct in6_addr *dst_mn_addr)
{
	ip6_mh_opt_dst_mn_addr_t *opt;
	size_t optlen = sizeof(ip6_mh_opt_dst_mn_addr_t);
	iov->iov_base = malloc(optlen);
	iov->iov_len = optlen;
	if (iov->iov_base == NULL)
		return -ENOMEM;
	opt = (ip6_mh_opt_dst_mn_addr_t *) iov->iov_base;
	opt->ip6dma_type = IP6_MHOPT_DST_MN_ADDR;
	opt->ip6dma_len = 16;
	opt->dst_mn_addr = *dst_mn_addr;
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_serv_mag_addr(struct iovec *iov, struct in6_addr *Serv_MAG_addr)
{
	ip6_mh_opt_serv_mag_addr_t *opt;
	size_t optlen = sizeof(ip6_mh_opt_serv_mag_addr_t);
	iov->iov_base = malloc(optlen);
	iov->iov_len = optlen;
	if (iov->iov_base == NULL)
		return -ENOMEM;
	opt = (ip6_mh_opt_serv_mag_addr_t *) iov->iov_base;
	opt->ip6sma_type = IP6_MHOPT_SERV_MAG_ADDR;
	opt->ip6sma_len = 16;   //16 bytes
	opt->serv_mag_addr = *Serv_MAG_addr;
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_serv_lma_addr(struct iovec *iov, struct in6_addr *serv_lma_addr)
{
	ip6_mh_opt_serv_lma_addr_t *opt;
	size_t optlen = sizeof(ip6_mh_opt_serv_lma_addr_t);
	iov->iov_base = malloc(optlen);
	iov->iov_len = optlen;
	if (iov->iov_base == NULL)
		return -ENOMEM;
	opt = (ip6_mh_opt_serv_lma_addr_t *) iov->iov_base;
	opt->ip6sla_type = IP6_MHOPT_SERV_LMA_ADDR;
	opt->ip6sla_len = 16;   //16 bytes
	opt->serv_lma_addr = *serv_lma_addr;
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_src_mn_addr(struct iovec *iov, struct in6_addr *src_mn_addr)
{
	struct ip6_mh_opt_src_mn_addr *opt;
	size_t optlen = sizeof(struct ip6_mh_opt_src_mn_addr);
	iov->iov_base = malloc(optlen);
	iov->iov_len = optlen;
	if (iov->iov_base == NULL)
		return -ENOMEM;
	opt = (struct ip6_mh_opt_src_mn_addr *) iov->iov_base;
	opt->ip6sma_type = IP6_MHOPT_SRC_MN_ADDR;
	opt->ip6sma_len = 16;   //16 bytes
	opt->src_mn_addr = *src_mn_addr;
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_src_mag_addr(struct iovec *iov, struct in6_addr *src_mag_addr)
{
	ip6_mh_opt_src_mag_addr_t *opt;
	size_t optlen = sizeof(ip6_mh_opt_src_mag_addr_t);
	iov->iov_base = malloc(optlen);
	iov->iov_len = optlen;
	if (iov->iov_base == NULL)
		return -ENOMEM;
	opt = (ip6_mh_opt_src_mag_addr_t *) iov->iov_base;
	opt->ip6sma_type = IP6_MHOPT_SRC_MAG_ADDR;
	opt->ip6sma_len = 16;   //16 bytes
	opt->src_mag_addr = *src_mag_addr;
	return 0;
}

//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_service_selection(struct iovec *iov)
{
	/* From RFC 5149
	   3.  Service Selection Mobility Option

		  At most one Service Selection Mobility Option MAY be included in any
		  Binding Update message.  If the Binding Update message includes any
		  authorization-related options (such as the Binding Authorization Data
		  option [2]) or authentication related options (such as the Mobility
		  Message Authentication option [8]), then the Service Selection option
		  MUST appear before any mobility message authorization- or
		  authentication-related options.

		  The Service Selection option SHOULD NOT be sent to a correspondent
		  node.  The mobile node cannot assume that the correspondent node has
		  any knowledge about a specific service selection made between the
		  mobile node and the home agent.

		  The Service Selection option has no alignment requirement as such.


		   0				   1				   2				   3
		   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
										  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
										  |  Type = 20	  |   Length	  |
		  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		  | Identifier...
		  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

							Service Selection Mobility Option

	 o	Type: 8-bit identifier set to 20 of the type of the skipable
		  mobility option.

	   o  Length: 8-bit unsigned integer, representing the length of the
		  Service Selection Mobility Option in octets, excluding the Option
		  Type and Option Length fields.  A value of zero (0) is not
		  allowed.

	   o  Identifier: A variable-length encoded service identifier string
		  used to identify the requested service.  The identifier string
		  length is between 1 and 255 octets.  This specification allows
		  international identifier strings that are based on the use of
		  Unicode characters, encoded as UTF-8 [3], and formatted using
		  Normalization Form KC (NFKC) as specified in [4].

	 */
	dbg("service selection string ==%s= strlen%d=\n",conf.ServiceSelectionIdentifier,strlen(conf.ServiceSelectionIdentifier));
	if (strlen(conf.ServiceSelectionIdentifier) == 0)
	{
		dbg("invalid string length \n");
		return -1;
	}
	ip6_mh_opt_service_selection_t *opt;
	size_t optlen = ((sizeof(ip6_mh_opt_service_selection_t) - sizeof(opt->identifier)) + strlen(conf.ServiceSelectionIdentifier));
	dbg("optlen %d\n",optlen);
	dbg("sizeof(ip6_mh_opt_service_selection_t)=%d,, sizeof(opt->identifier)=%d,, strlen(conf.ServiceSelectionIdentifier)=%d\n",sizeof(ip6_mh_opt_service_selection_t),sizeof(opt->identifier),strlen(conf.ServiceSelectionIdentifier));
	iov->iov_base = malloc(sizeof(ip6_mh_opt_service_selection_t));
	iov->iov_len = optlen;
	if (iov->iov_base == NULL)
		return -ENOMEM;
	opt = (ip6_mh_opt_service_selection_t *) iov->iov_base;
	memset(opt, 0, sizeof(ip6_mh_opt_service_selection_t));
	opt->ip6service_sel_type= IP6_MHOPT_SERVICE_SELECTION;
	memcpy (opt->identifier, conf.ServiceSelectionIdentifier, strlen(conf.ServiceSelectionIdentifier));
	dbg("identifier string=%s=, len=%d\n",opt->identifier,strlen(opt->identifier));
	opt->ip6service_sel_len     = strlen(opt->identifier);
	return 0;
}

//---------------------------------------------------------------------------------------------------------------------
int mh_create_opt_vendor_specific_option(struct iovec *iov)
{
	/* From RFC 5094
	 3.  Vendor-Specific Mobility Option

		The Vendor Specific Mobility Option can be included in any Mobility
		Header message and has an alignment requirement of 4n+2.  If the
		Mobility Header message includes a Binding Authorization Data option
		[2], then the Vendor Specific mobility option should appear before
		the Binding Authorization Data option.	Multiple Vendor-Specific
		mobility options MAY be present in a Mobility Header message.

		   0				   1				   2				   3
		   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
										  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
										  | 	Type	  |   Length	  |
		  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		  | 						Vendor ID							  |
		  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		  |   Sub-Type	  | 			Data.......
		  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

		Type

		   An 8-bit field indicating that it is a Vendor-Specific mobility
		   option.

		Length

		   An 8-bit field indicating the length of the option in octets
		   excluding the Type and the Length fields.  All other fields are
		   included.

		Vendor ID

		   The SMI Network Management Private Enterprise Code of the IANA-
		   maintained Private Enterprise Numbers registry [5].

	 Sub-type

		 An 8-bit field indicating the type of vendor-specific information
		 carried in the option.  The administration of the Sub-type is done
		 by the Vendor.


	  Data

		 Vendor-specific data that is carried in this message.
	 */
	dbg("Adding Vendor specific option\n");
	ip6_mh_opt_vendor_specific_option_t *opt;
	size_t optlen = sizeof(ip6_mh_opt_vendor_specific_option_t);
	iov->iov_base = malloc(optlen);
	iov->iov_len = optlen;
	if (iov->iov_base == NULL)
		return -ENOMEM;
	opt = (ip6_mh_opt_vendor_specific_option_t *) iov->iov_base;
	opt->ip6vendor_spec_option_type = IP6_VENDOR_SPECIFIC_OPTION;
	opt->ip6vendor_id[0] = 0;
	opt->ip6vendor_id[1] = (IP6_VENDOR_SPECIFIC_OPTION_VENDOR_ID);
	opt->ip6sub_type = IP6_VENDOR_SPECIFIC_OPTION_SUB_TYPE;
	opt->ven_data.reserve = 0;
	opt->ven_data.extention = 0x80;
	opt->ven_data.proto_info.length = 0;
	opt->ven_data.proto_info.protocol_id[0] = 0;
	opt->ven_data.proto_info.protocol_id[1] = IP6_VENDOR_SPECIFIC_OPTION_DATA_PROTOCOL_ID;
	opt->ip6vendor_spec_option_len = sizeof(ip6_mh_opt_vendor_specific_option_t) - 2;
	dbg("whole size %d,vendor struct size %d,protocol struct size %d,\n",sizeof(ip6_mh_opt_vendor_specific_option_t),sizeof(opt->ven_data),sizeof(opt->ven_data.proto_info));
	return 0;
}


//---------------------------------------------------------------------------------------------------------------------
int mh_pbu_parse(msg_info_t * info, struct ip6_mh_binding_update *pbu, ssize_t len, void *addrs, int iif)
{
	static struct mh_options mh_opts;
	ip6_mh_opt_home_net_prefix_t                     *home_net_prefix_opt;
	ip6_mh_opt_mobile_node_identifier_t              *mobile_node_identifier_opt;
	ip6_mh_opt_mobile_node_link_layer_identifier_t   *mobile_node_link_layer_identifier_opt;
	ip6_mh_opt_time_stamp_t                          *time_stamp_opt;
	ip6_mh_opt_handoff_indicator_t                   *handoff_indicator_opt;
	ip6_mh_opt_access_technology_type_t              *access_technology_type_opt;
	ip6_mh_opt_gre_key_t                             *gre_key_opt;

	ip_mh_opt_v4_home_addr_request_option_t           *v4_home_addr_request_opt;
	ip_mh_opt_v4_dmnp_prefix_option_t                  *dmnp_prefix_opt;
	ip6_mh_opt_service_selection_t                    *service_selection_opt;
	char str[PMIP_MAX_IDENTIFIER_STRING_LEN] = {0};

	int                                              return_code;
	bzero(&mh_opts, sizeof(mh_opts));

	//if v6 tunnel
	if(conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)
	{
		struct in6_addr_bundle *in_addrs = (struct in6_addr_bundle *)addrs;
		info->src = *in_addrs->src;
		info->dst = *in_addrs->dst;
		info->iif = iif;
		info->addrs.src = &info->src;
		info->addrs.dst = &info->dst;
		dbg("Serving MAG Address: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&info->src));
		dbg("Our Address        : %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&info->dst));
	}
	//if v4 tunnel
	else if (conf.pmip_tunnel_mode == IP_FAMILY_V4_V01)
	{
		struct in_addr_bundle *in_addrs = (struct in_addr_bundle *)addrs;
		info->srcv4 = *in_addrs->src;
		info->dstv4 = *in_addrs->dst;
		info->iif = iif;
		info->addrsv4.src = &info->srcv4;
		info->addrsv4.dst = &info->dstv4;
		dbg("Serving MAG Address: %s\n", inet_ntoa(info->srcv4));
		dbg("Our Address        : %s\n", inet_ntoa(info->dstv4));
	}
	else
	{
		dbg("Invalid Tunnel mode %d\n", conf.pmip_tunnel_mode);
	}

	if (len < sizeof(struct ip6_mh_binding_update)) {
		dbg("Bad len of PBU mobility header   : %d versus sizeof(struct ip6_mh_binding_update)= %d\n", len, sizeof(struct ip6_mh_binding_update));
		return 0;
	}
	if ((return_code = mh_opt_parse(&pbu->ip6mhbu_hdr, len, sizeof(struct ip6_mh_binding_update), &mh_opts)) < 0) {
		dbg("Error %d in parsing PBU options\n", return_code);
		return 0;
	}
	info->PBU_flags = ntohs(pbu->ip6mhbu_flags);
	info->lifetime.tv_sec = (ntohs(pbu->ip6mhbu_lifetime) << 2);
	info->seqno = ntohs(pbu->ip6mhbu_seqno);
	dbg("PBU FLAGS          : %04X\n", info->PBU_flags);
	dbg("PBU Lifetime       : %d (%d seconds)\n", pbu->ip6mhbu_lifetime, info->lifetime.tv_sec);
	dbg("PBU Sequence No    : %d\n", info->seqno);

	mobile_node_identifier_opt = mh_opt(&pbu->ip6mhbu_hdr, &mh_opts, IP6_MHOPT_MOBILE_NODE_IDENTIFIER);
	if (mobile_node_identifier_opt)
	{
		//copy
		info->mn_hw_address= in6addr_any;
		memcpy(str, mobile_node_identifier_opt->ip6mnid_identifier, (mobile_node_identifier_opt->ip6mnid_len -1));
		dbg("Mobile NodeIdentifier : %s\n", str);
	}

	service_selection_opt = mh_opt(&pbu->ip6mhbu_hdr, &mh_opts, IP6_MHOPT_SERVICE_SELECTION);
	if (service_selection_opt) {
		bzero(str, sizeof(str));
		memcpy(str, service_selection_opt->identifier, (service_selection_opt->ip6service_sel_len));
		dbg("Service Selection string %s \n", str);
	}


	home_net_prefix_opt = mh_opt(&pbu->ip6mhbu_hdr, &mh_opts, IP6_MHOPT_HOME_NETWORK_PREFIX);
	if (home_net_prefix_opt)
	{
		//copy
		info->mn_prefix = home_net_prefix_opt->ip6hnp_prefix;
		dbg("Mobile Node Home Network Prefix option: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&info->mn_prefix));
	}

	mobile_node_link_layer_identifier_opt = mh_opt(&pbu->ip6mhbu_hdr, &mh_opts, IP6_MHOPT_MOBILE_NODE_LINK_LAYER_IDENTIFIER);
	if (mobile_node_link_layer_identifier_opt) {
		//copy
		info->mn_iid = in6addr_any;
		memcpy(&info->mn_iid.s6_addr32[2], &mobile_node_link_layer_identifier_opt->ip6mnllid_lli, sizeof(ip6mnid_t));
		dbg("Mobile Node Link-layer Identifier Option: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&info->mn_iid));
		info->mn_hw_address= in6addr_any;
		memcpy(&info->mn_hw_address, &info->mn_iid, sizeof(info->mn_iid));
		dbg("hw address %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&info->mn_hw_address));
	}

	time_stamp_opt = mh_opt(&pbu->ip6mhbu_hdr, &mh_opts, IP6_MHOPT_TIME_STAMP);
	if (time_stamp_opt) {
		//copy
		memcpy(info->timestamp.first,time_stamp_opt->time_stamp.first,sizeof(time_stamp_opt->time_stamp.first));
		info->timestamp.second = time_stamp_opt->time_stamp.second;
		uint32_t seconds ;
		memcpy(&seconds,info->timestamp.first+2,sizeof(seconds));
		unsigned int useconds = ((info->timestamp.second) * 1000000 / 65536);
		dbg("Timestamp option: %d sec %d usec\n", ntohl(seconds), ntohl(useconds));
	}

	handoff_indicator_opt = mh_opt(&pbu->ip6mhbu_hdr, &mh_opts, IP6_MHOPT_HANDOFF_INDICATOR);
	if (handoff_indicator_opt) {
		dbg("Handoff Indicator option: %d\n", handoff_indicator_opt->ip6hi_hi);
	}

	access_technology_type_opt = mh_opt(&pbu->ip6mhbu_hdr, &mh_opts, IP6_MHOPT_ACCESS_TECHNOLOGY_TYPE);
	if (access_technology_type_opt) {
		dbg("Access Technology Type option: %d\n", access_technology_type_opt->ip6att_att);
	}

	gre_key_opt = mh_opt(&pbu->ip6mhbu_hdr, &mh_opts, IP6_MHOPT_GRE_KEY);
	if (gre_key_opt) {
		dbg("gre recvd lenght: %d\n", gre_key_opt->ip6gre_len);
	}

	v4_home_addr_request_opt = mh_opt(&pbu->ip6mhbu_hdr, &mh_opts, IPV4_HOME_ADDR_REQUEST_OPTION);
	if (v4_home_addr_request_opt) {
		dbg("v4 home addr recvd lenght: %d\n", v4_home_addr_request_opt->ip4_opt_len);
		info->pmipv4_hnp_req_valid = 1;
		info->v4_home_prefix = v4_home_addr_request_opt->ip4hnp_prefix;
		info->v4_home_prefixlen= v4_home_addr_request_opt->plen;
	}

	dmnp_prefix_opt = mh_opt(&pbu->ip6mhbu_hdr, &mh_opts, IPV4_DMNP_PREFIX_OPTION);
	if (dmnp_prefix_opt) {
		dbg("dmnp prefix recv lenght: %d\n", dmnp_prefix_opt->ip4_opt_len);
		dbg("dmnp ip=%s: len=%d\n", inet_ntoa(dmnp_prefix_opt->dmnp_prefix),dmnp_prefix_opt->prefix_len);
		info->hasipv4_dmnp_prefix_option = 1;
		info->dmnp_prefix = dmnp_prefix_opt->dmnp_prefix;
		info->dmnp_prefix_len= dmnp_prefix_opt->prefix_len;
	}

	info->msg_event = hasPBU;
	dbg("FSM Message Event: %d\n", info->msg_event);
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_pba_parse(msg_info_t * info, struct ip6_mh_binding_ack *pba, ssize_t len, const struct in6_addr_bundle *in_addrs, int iif)
{
	static struct mh_options mh_opts;
	ip6_mh_opt_home_net_prefix_t                     *home_net_prefix_opt;
	ip6_mh_opt_mobile_node_identifier_t              *mobile_node_identifier_opt;
	ip6_mh_opt_mobile_node_link_layer_identifier_t   *mobile_node_link_layer_identifier_opt;
	ip6_mh_opt_time_stamp_t                          *time_stamp_opt;
	ip6_mh_opt_handoff_indicator_t                   *handoff_indicator_opt;
	ip6_mh_opt_access_technology_type_t              *access_technology_type_opt;
	ip6_mh_opt_gre_key_t                             *gre_key_opt;
	ip_mh_opt_v4_home_addr_reply_option_t            *v4_home_addr_reply_opt;
	ip_mh_opt_v4_dmnp_prefix_option_t                 *dmnp_prefix_opt;
	ip_mh_opt_v4_default_router_addr_option_t         *default_router_addr_opt;
	ip6_mh_opt_service_selection_t                    *service_selection_opt;

	char str[PMIP_MAX_IDENTIFIER_STRING_LEN] = {0};

	bzero(&mh_opts, sizeof(mh_opts));
	info->src = *in_addrs->src;
	info->dst = *in_addrs->dst;
	info->iif = iif;
	info->addrs.src = &info->src;
	info->addrs.dst = &info->dst;

	if ((len < sizeof(struct ip6_mh_binding_ack)
				|| mh_opt_parse(&pba->ip6mhba_hdr, len, sizeof(struct ip6_mh_binding_ack), &mh_opts) < 0)) {
		dbg("Error in len:%d", len);
		return 0;
	}

	mobile_node_identifier_opt = mh_opt(&pba->ip6mhba_hdr, &mh_opts, IP6_MHOPT_MOBILE_NODE_IDENTIFIER);
	if (mobile_node_identifier_opt) {
		//copy
		info->mn_hw_address= in6addr_any;
		memcpy(str, mobile_node_identifier_opt->ip6mnid_identifier, (mobile_node_identifier_opt->ip6mnid_len -1));
		dbg("Mobile Node Identifier Option %s \n", str);
	}

	service_selection_opt = mh_opt(&pba->ip6mhba_hdr, &mh_opts, IP6_MHOPT_SERVICE_SELECTION);
	if (service_selection_opt) {
		bzero(str, sizeof(str));
		memcpy(str, service_selection_opt->identifier, (service_selection_opt->ip6service_sel_len));
		dbg("Service Selection string %s \n", str);
	}

	home_net_prefix_opt = mh_opt(&pba->ip6mhba_hdr, &mh_opts, IP6_MHOPT_HOME_NETWORK_PREFIX);
	if (home_net_prefix_opt) {
		info->mn_prefix = home_net_prefix_opt->ip6hnp_prefix;
		dbg("Mobile Node Home Network Prefix option: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&info->mn_prefix));
	}

	mobile_node_link_layer_identifier_opt = mh_opt(&pba->ip6mhba_hdr, &mh_opts, IP6_MHOPT_MOBILE_NODE_LINK_LAYER_IDENTIFIER);
	if (mobile_node_link_layer_identifier_opt) {
		//copy
		info->mn_iid = in6addr_any;
		memcpy(&info->mn_iid.s6_addr32[2], &mobile_node_link_layer_identifier_opt->ip6mnllid_lli, sizeof(ip6mnid_t));
		dbg("Mobile Node Link-layer Identifier Option: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&info->mn_iid));
		info->mn_hw_address= in6addr_any;
		memcpy(&info->mn_hw_address, &info->mn_iid, sizeof(info->mn_iid));
		dbg("hw address %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&info->mn_hw_address));
	}

	time_stamp_opt = mh_opt(&pba->ip6mhba_hdr, &mh_opts, IP6_MHOPT_TIME_STAMP);
	if (time_stamp_opt) {
		//copy
		memcpy(info->timestamp.first,time_stamp_opt->time_stamp.first,sizeof(time_stamp_opt->time_stamp.first));
		info->timestamp.second = time_stamp_opt->time_stamp.second;
		uint32_t seconds ;
		memcpy(&seconds,info->timestamp.first+2,sizeof(seconds));
		unsigned int useconds = (info->timestamp.second) * 1000000 / 65536;
		dbg("Timestamp option: %ld sec %d usec\n", ntohl(seconds), ntohl(useconds));
	}

	handoff_indicator_opt = mh_opt(&pba->ip6mhba_hdr, &mh_opts, IP6_MHOPT_HANDOFF_INDICATOR);
	if (handoff_indicator_opt) {
		dbg("Handoff Indicator option: %d\n", handoff_indicator_opt->ip6hi_hi);
	}

	access_technology_type_opt = mh_opt(&pba->ip6mhba_hdr, &mh_opts, IP6_MHOPT_ACCESS_TECHNOLOGY_TYPE);
	if (access_technology_type_opt) {
		dbg("Access Technology Type option: %d\n", access_technology_type_opt->ip6att_att);
	}

	gre_key_opt = mh_opt(&pba->ip6mhba_hdr, &mh_opts, IP6_MHOPT_GRE_KEY);
	if (gre_key_opt) {
		dbg("gre recvd lenght: %d\n", gre_key_opt->ip6gre_len);
		if(gre_key_opt->ip6gre_len == IP6_MH_OPT_GRE_KEY_LEN)
		{
			dbg("gre recvd key: %d\n", gre_key_opt->ip6gre_key);
			info->gre_key_valid = GRE_KEY_VALID;
			info->gre_key   = ntohl(gre_key_opt->ip6gre_key);
		}
		else
		{
			info->gre_key_valid = GRE_KEY_INVALID;
			info->gre_key   = 0;
		}
	}

	v4_home_addr_reply_opt = mh_opt(&pba->ip6mhba_hdr, &mh_opts, IPV4_HOME_ADDR_REPLY_OPTION);
	if (v4_home_addr_reply_opt) {
		dbg("v4_home_addr_reply_opt recvd lenght: %d\n", v4_home_addr_reply_opt->ip4_opt_len);
		dbg("Got V4 home addr ip %s: as reply\n", inet_ntoa(v4_home_addr_reply_opt->ip4hnp_prefix));
		dbg("Got V4 home addr prefix lenght %d: as reply with status =%d\n", v4_home_addr_reply_opt->plen, v4_home_addr_reply_opt->status);
		info->pmipv4_hnp_reply_valid = 1;
		info->v4_home_prefix   = v4_home_addr_reply_opt->ip4hnp_prefix;
		//expiciltly setting prefix lenght to 24 fix latter
		//info->v4_home_prefixlen= v4_home_addr_reply_opt->plen
		dbg("==Got full 8 byte prefix & reserv info lenght=%u==\n", v4_home_addr_reply_opt->plen);
		info->v4_home_prefixlen = ((v4_home_addr_reply_opt->plen) >> 2);
		dbg("== extrated prefix lenght=%d==\n", info->v4_home_prefixlen);
		info->v4_home_addr_reply_status  =v4_home_addr_reply_opt->status;
	}
	else
	{
		memset(&info->mn_hw_address, 0x0, sizeof(struct in6_addr));
		pmip_cache_iterate_ext(pmip_cache_get_mac_from_iid, (void*)&info->mn_iid, (void *)&info->mn_hw_address);
		dbg("pba got MAC address as: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&info->mn_hw_address));
	}

	dmnp_prefix_opt = mh_opt(&pba->ip6mhba_hdr, &mh_opts, IPV4_DMNP_PREFIX_OPTION);
	if (dmnp_prefix_opt) {
		dbg("dmnp_prefix_opt recvd lenght: %d\n", dmnp_prefix_opt->ip4_opt_len);
		dbg("DMNP received prefix =%s len=%d\n", inet_ntoa(dmnp_prefix_opt->dmnp_prefix),dmnp_prefix_opt->prefix_len);
		info->hasipv4_dmnp_prefix_option= 1;
		info->dmnp_prefix= dmnp_prefix_opt->dmnp_prefix;
		info->dmnp_prefix_len = dmnp_prefix_opt->prefix_len;
		info->dmnp_pba_status = pba->ip6mhba_status;
	}

	default_router_addr_opt = mh_opt(&pba->ip6mhba_hdr, &mh_opts, IPV4_HOME_DEFAULT_ROUTER_OPTION);
	if (default_router_addr_opt) {
		dbg("default_router_addr_opt recvd lenght: %d\n", default_router_addr_opt->ip4_opt_len);
		dbg("default_router_addr_opt received prefix =%s \n", inet_ntoa(default_router_addr_opt->default_router_addr));
		info->hasipv4_default_router_option= 1;
		info->default_router= default_router_addr_opt->default_router_addr;
	}

	info->seqno = ntohs(pba->ip6mhba_seqno);
	info->PBA_flags = ntohs(pba->ip6mhba_flags);
	info->lifetime.tv_sec = ntohs(pba->ip6mhba_lifetime) << 2;
	dbg("PBA FLAGS      : %04X\n", info->PBA_flags);
	dbg("PBA Lifetime   : %d (%d seconds)\n", pba->ip6mhba_lifetime, info->lifetime.tv_sec);
	dbg("PBA Sequence No: %d\n", info->seqno);
	dbg("PBA GRE Key: %d\n", info->gre_key);
	info->msg_event = hasPBA;
	dbg("FSM Message Event: %d\n", info->msg_event);
	return 0;
}


//---------------------------------------------------------------------------------------------------------------------
int icmp_rs_parse(msg_info_t * info, struct nd_router_solicit *rs, const struct in6_addr *saddr, const struct in6_addr *daddr, int iif, int hoplimit)
{
	bzero(info, sizeof(msg_info_t));
	//info->ns_target = ns->nd_ns_target;
	info->hoplimit = hoplimit;
	info->msg_event = hasRS;
	info->src = *saddr;
	info->dst = *daddr;
	info->iif = iif;
	info->addrs.src = &info->src;
	info->addrs.dst = &info->dst;
	//Calculated fields
	info->mn_iid = get_node_id(&info->src);
	dbg("MN IID: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&info->mn_iid));
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int icmp_na_parse(msg_info_t * info, struct nd_neighbor_advert *na, const struct in6_addr *saddr, const struct in6_addr *daddr, int iif, int hoplimit)
{
	struct in6_addr bce_mac_address;

	bzero(info, sizeof(msg_info_t));
	info->na_target = na->nd_na_target;
	info->hoplimit = hoplimit;
	info->msg_event = hasNA;
	info->src = *saddr;
	info->dst = *daddr;
	info->iif = iif;
	info->addrs.src = &info->src;
	info->addrs.dst = &info->dst;
	//Calculated fields
	info->mn_iid = get_node_id(&info->na_target);
	info->mn_addr = info->na_target;
	info->mn_prefix = get_node_prefix(&info->na_target);

	memset(&bce_mac_address, 0x0, sizeof(struct in6_addr));
	pmip_cache_iterate_ext(pmip_cache_get_bce_mac, (void*)saddr, (void *)&bce_mac_address);

	info->mn_hw_address = bce_mac_address;
	dbg("NA Target: %x:%x:%x:%x:%x:%x:%x:%x got MAC address as: %x:%x:%x:%x:%x:%x:%x:%x\n",
			NIP6ADDR(&na->nd_na_target), NIP6ADDR(&info->mn_hw_address));
	return 0;
}

int pmipv4_mh_send(const struct in_addr_bundle *addrs, const struct iovec *mh_vec, int iovlen, int oif, int is_v4_pmip_mode)
{
	struct sockaddr_in daddr;
	struct iovec iov[2 * (IP6_MHOPT_MAX + 1)];
	struct msghdr msg;
	struct cmsghdr *cmsg;
	int cmsglen;
	struct in_pktinfo pinfo;
	int ret = 0, on = 1;
	struct ip6_mh *mh;
	int iov_count;
	int sock = -1;
	int send_v6_to_pmip_port = 0;

	memset((void*)iov , 0, (2 * (IP6_MHOPT_MAX + 1))*sizeof(struct iovec));

	iov_count = mh_try_pad(mh_vec, &iov[0], iovlen);

	mh = (struct ip6_mh *) iov[0].iov_base;
	mh->ip6mh_hdrlen = (mh_length(iov, iov_count) >> 3) - 1;


	dbg("mipv6 type =%d\n",mh->ip6mh_type);
	dbg("Sending MH type %d from %s \n", mh->ip6mh_type, inet_ntoa(*addrs->src));
	dbg(" \t\t\t to \t%s\n", inet_ntoa(*addrs->dst));

	memset(&daddr, 0, sizeof(struct sockaddr_in));
	daddr.sin_family = AF_INET;
	daddr.sin_addr = *(addrs->dst);
	//inet_aton("10.242.28.141",&daddr.sin_addr);
	//daddr.sin_port = htons(IPPROTO_MH);

#ifdef SEND_V6_TO_PMIP_PORT
	send_v6_to_pmip_port = 1;
#endif

	if ((is_v4_pmip_mode == 0) && (send_v6_to_pmip_port == 0))
	{
	daddr.sin_port = htons(IPPROTO_MH);
		sock = mhv4_sock.ip_sock_fd;
	}
	else
	{
		daddr.sin_port = htons(PMIP_RESERVED_PORT);
		sock = mhv4_sock.udp_sock_fd;
	}

	memset(&pinfo, 0, sizeof(pinfo));
	pinfo.ipi_addr = *(addrs->src);
	//inet_aton("10.252.134.239",&pinfo.ipi_addr);
	pinfo.ipi_ifindex = oif;

	dbg("packet info from %s \n",inet_ntoa(pinfo.ipi_addr));
	dbg("index v4 tun= %d with 0 prefix\n",oif);

	cmsglen = CMSG_SPACE(sizeof(pinfo));
	cmsg = malloc(cmsglen);

	if (cmsg == NULL) {
		dbg("malloc failed\n");
		return -ENOMEM;
	}
	memset(cmsg, 0, cmsglen);
	memset(&msg, 0, sizeof(msg));
	msg.msg_control = cmsg;
	msg.msg_controllen = cmsglen;
	msg.msg_iov = iov;
	msg.msg_iovlen = iov_count;
	msg.msg_name = (void *) &daddr;
	msg.msg_namelen = sizeof(daddr);

	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_len = CMSG_LEN(sizeof(pinfo));
	cmsg->cmsg_level = IPPROTO_IP;
	cmsg->cmsg_type = IP_PKTINFO;
	memcpy(CMSG_DATA(cmsg), &pinfo, sizeof(pinfo));

	pthread_mutex_lock(&mhv4_sock.send_mutex);
	setsockopt(sock, IPPROTO_IP, IP_PKTINFO, &on, sizeof(int));
	dbg("Set socket option IP_PKTINFO packet lenght %d\n",msg.msg_iovlen);
	ret = sendmsg(sock, &msg, 0);
	if (ret < 0) {
		dbg("sendmsg:failed Error number %d Error Message '%s'\n", errno, strerror(errno));
	}
	else
		dbg("MH is sent OK....\n");
	pthread_mutex_unlock(&mhv4_sock.send_mutex);
	free(msg.msg_control);
	return ret;
}


//---------------------------------------------------------------------------------------------------------------------
int pmip_mh_send(const struct in6_addr_bundle *addrs, const struct iovec *mh_vec, int iovlen, int oif, int is_v4_pmip_mode)
{
	struct sockaddr_in6 daddr;
	struct iovec iov[2 * (IP6_MHOPT_MAX + 1)];
	struct msghdr msg;
	struct cmsghdr *cmsg;
	int cmsglen;
	struct in6_pktinfo pinfo;
	int ret = 0, on = 1;
	struct ip6_mh *mh;
	int iov_count;
	int sock = -1;
	int send_v6_to_pmip_port = 0;

	iov_count = mh_try_pad(mh_vec, iov, iovlen);

	mh = (struct ip6_mh *) iov[0].iov_base;
	mh->ip6mh_hdrlen = (mh_length(iov, iov_count) >> 3) - 1;
	dbg("Sending MH type %d\n" "from %x:%x:%x:%x:%x:%x:%x:%x\n" "to %x:%x:%x:%x:%x:%x:%x:%x\n", mh->ip6mh_type, NIP6ADDR(addrs->src), NIP6ADDR(addrs->dst));

	memset(&daddr, 0, sizeof(struct sockaddr_in6));
	daddr.sin6_family = AF_INET6;
	daddr.sin6_addr = *addrs->dst;

#ifdef SEND_V6_TO_PMIP_PORT
	send_v6_to_pmip_port = 1;
#endif

	if ((is_v4_pmip_mode == 0) && (send_v6_to_pmip_port == 0))
	{
	daddr.sin6_port = htons(IPPROTO_MH);
		sock = mh_sock.ip_sock_fd;
	}
	else
	{
		daddr.sin6_port = htons(PMIP_RESERVED_PORT);
		sock = mh_sock.udp_sock_fd;
	}

	memset(&pinfo, 0, sizeof(pinfo));
	pinfo.ipi6_addr = *addrs->src;
	pinfo.ipi6_ifindex = oif;

	cmsglen = CMSG_SPACE(sizeof(pinfo));
	cmsg = malloc(cmsglen);

	if (cmsg == NULL) {
		dbg("malloc failed\n");
		return -ENOMEM;
	}
	memset(cmsg, 0, cmsglen);
	memset(&msg, 0, sizeof(msg));
	msg.msg_control = cmsg;
	msg.msg_controllen = cmsglen;
	msg.msg_iov = iov;
	msg.msg_iovlen = iov_count;
	msg.msg_name = (void *) &daddr;
	msg.msg_namelen = sizeof(daddr);

	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_len = CMSG_LEN(sizeof(pinfo));
	cmsg->cmsg_level = IPPROTO_IPV6;
	cmsg->cmsg_type = IPV6_PKTINFO;
	memcpy(CMSG_DATA(cmsg), &pinfo, sizeof(pinfo));

	pthread_mutex_lock(&mh_sock.send_mutex);
	setsockopt(sock, IPPROTO_IPV6, IPV6_PKTINFO, &on, sizeof(int));
	ret = sendmsg(sock, &msg, 0);
	if (ret < 0) {
		dbg("sendmsg: %s\n", strerror(errno));
	}
	pthread_mutex_unlock(&mh_sock.send_mutex);
	free(msg.msg_control);
	dbg("MH is sent....\n");
	return ret;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_send_pbu(void *addrs, pmip_entry_t * bce, struct timespec *lifetime, int oif, int is_v4_pmip_mode)
{
	/*  FROM RFC 5213
	 *
	 * 6.9.1.5. Constructing the Proxy Binding Update Message

	 o  The mobile access gateway, when sending the Proxy Binding Update
	 message to the local mobility anchor, MUST construct the message
	 as specified below.

	 IPv6 header (src=Proxy-CoA, dst=LMAA)
	 Mobility header
	 - BU  P & A flags MUST be set to value 1
	 Mobility Options
	 - Mobile Node Identifier option            (mandatory)
	 - Home Network Prefix option(s)            (mandatory)
	 - Handoff Indicator option                 (mandatory)
	 - Access Technology Type option            (mandatory)
	 - Timestamp option                         (optional)
	 - Mobile Node Link-layer Identifier option (optional)
	 - Link-local Address option                (optional)

	 Figure 12: Proxy Binding Update Message Format


	 o  The Source Address field in the IPv6 header of the message MUST be
	 set to the global address configured on the egress interface of
	 the mobile access gateway.  When there is no Alternate Care-of
	 Address option present in the request, this address will be
	 considered as the Proxy-CoA for this Proxy Binding Update message.
	 However, when there is an Alternate Care-of Address option present
	 in the request, this address will be not be considered as the
	 Proxy-CoA, but the address in the Alternate Care-of Address option
	 will be considered as the Proxy-CoA.

	 o  The Destination Address field in the IPv6 header of the message
	 MUST be set to the local mobility anchor address.

	 o  The Mobile Node Identifier option [RFC4283] MUST be present.

	 o  At least one Home Network Prefix option MUST be present.

	 o  The Handoff Indicator option MUST be present.

	 o  The Access Technology Type option MUST be present.

	 o  The Timestamp option MAY be present.

	 o  The Mobile Node Link-layer Identifier option MAY be present.

	 o  The Link-local Address option MAY be present.

	 o  If IPsec is used for protecting the signaling messages, the
	 message MUST be protected, using the security association existing
	 between the local mobility anchor and the mobile access gateway.

	 o  Unlike in Mobile IPv6 [RFC3775], the Home Address option [RFC3775]
	 MUST NOT be present in the IPv6 Destination Options extension
	 header of the Proxy Binding Update message.
	 */

	int                           iovlen = 1;
	struct ip6_mh_binding_update *pbu;
	static struct iovec           mh_vec[2 * (IP6_MHOPT_MAX + 1)];
	static struct iovec           dummy_mh_vec[2 * (IP6_MHOPT_MAX + 1)];
	struct timeval                tv;
	struct timezone               tz;
	ip6mnid_t                     mn_iid;
	ip6mnid_mac_t                 mn_hardware_address;
	uint16_t                      p_flag = 1;
	int                           iov_count;
	uint32_t                      time_sec = 0;
	int ret = 0;
	struct udphdr *udph = NULL;

	memset((void*)mh_vec       , 0, (2 * (IP6_MHOPT_MAX + 1))*sizeof(struct iovec));
	memset((void*)dummy_mh_vec , 0, (2 * (IP6_MHOPT_MAX + 1))*sizeof(struct iovec));

	pbu = mh_create(&mh_vec[0], IP6_MH_TYPE_BU);
	if (!pbu) {
		dbg("mh_create(&mh_vec[0], IP6_MH_TYPE_BU) failed\n");
		return -ENOMEM;
	}
	bce->seqno_out        = get_new_pbu_sequence_number();
	ret = pending_pbu_seq_list_add(bce->seqno_out);
	if (ret < 0)
	{
		dbg("pbu sequence number addition to list failed\n");
	}
	else
	{
		dbg("pbu sequence number addition to list successed\n");
	}
	pbu->ip6mhbu_seqno    = htons(bce->seqno_out);
	pbu->ip6mhbu_flags    = bce->PBU_flags; // no htons since endianess already taken in account in ip6mh.h
	pbu->ip6mhbu_lifetime = htons(lifetime->tv_sec >> 2);
	dbg("Create PBU with lifetime = %d seconds (config = %d seconds)\n", lifetime->tv_sec, conf.PBULifeTime);
	memcpy(&mn_iid, &bce->mn_suffix.s6_addr32[2], sizeof(ip6mnid_t));
	ipv6_to_mac(&bce->mn_hw_address, mn_hardware_address.mac_addr);

	dbg("Create PBU options...\n");
	dbg("pmip mode %d\n",conf.pmipv4_work_mode_type);
	mh_create_opt_mobile_node_identifier(&mh_vec[iovlen++], p_flag, &mn_hardware_address);
		
	if (conf.is_valid_service_id == 1)
	{
	mh_create_opt_service_selection(&mh_vec[iovlen++]);
	}
	mh_create_opt_vendor_specific_option(&mh_vec[iovlen++]);
	if (is_v4_pmip_mode == 1) {
		dbg("mh_send_pbu is_pmipv4 true\n");
		dbg("adding home addr option\n");
		mh_create_opt_v4_home_addr_request_option(&mh_vec[iovlen++], bce->dhcp_requested_ip);
		if ((conf.pmipv4_work_mode_type == PMIPV4_MODE_SECONDARY_ROUTER_V01) && (conf.is_dmnp_prefix_verified !=1 )) {
			dbg("Secondary case. Since DMNP is Not Verified adding DMNP Prefix\n");
			mh_create_opt_v4_dmnp_option(&mh_vec[iovlen++], &conf.PMIPV4SecRouterDMNPPrefix, &conf.PMIPV4SecRouterdmnpPrefixLen);
		}
		mh_create_opt_v4_default_router_opt(&mh_vec[iovlen++]);
	}
	if (!is_v4_pmip_mode)
	{
		mh_create_opt_home_net_prefix(&mh_vec[iovlen++], &bce->mn_prefix);
	}
	mh_create_opt_handoff_indicator(&mh_vec[iovlen++], IP6_MHOPT_HI_HANDOFF_STATE_UNKNOWN);
	mh_create_opt_access_technology_type(&mh_vec[iovlen++], IP6_MHOPT_ATT_IEEE802_11ABG);
	mh_create_opt_mobile_node_link_layer_identifier(&mh_vec[iovlen++], &mn_iid);
	mh_create_opt_gre(&mh_vec[iovlen++], 0, 0);

	memset(&bce->timestamp,0 ,sizeof(bce->timestamp));
	if (gettimeofday(&tv, NULL) == 0) {
		dbg("Timestamp option success to get time, Add option\n");
		time_sec = htonl(tv.tv_sec) ;
		memcpy((bce->timestamp.first + 2),&time_sec,sizeof(time_sec));
		bce->timestamp.second =  htonl(((tv.tv_usec * 65536)/1000000));
		mh_create_opt_time_stamp(&mh_vec[iovlen++], &bce->timestamp);
	} else {
		memset(&bce->timestamp,0 ,sizeof(bce->timestamp));
		dbg("Timestamp option failed to get time, discard option\n");
	}

	iov_count = mh_try_pad(mh_vec, dummy_mh_vec, iovlen);


	//calculate the length of the message.
	pbu->ip6mhbu_hdr.ip6mh_hdrlen = mh_length(dummy_mh_vec, iov_count);
	dbg("Send PBU with life time...%d\n",pbu->ip6mhbu_lifetime );
	dbg("Send PBU header length...%d, ioc count %d\n",pbu->ip6mhbu_hdr.ip6mh_hdrlen , iov_count);

	dbg("Send PBU....\n");
	if((!is_v4_pmip_mode && conf.pmip_tunnel_mode == IP_FAMILY_V6_V01) ||
			((is_v4_pmip_mode == 1 && conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)))
		pmip_mh_send((struct in6_addr_bundle *)addrs, dummy_mh_vec, iov_count, oif, is_v4_pmip_mode);

	if  ((!is_v4_pmip_mode && conf.pmip_tunnel_mode == IP_FAMILY_V4_V01) ||
			(is_v4_pmip_mode == 1 && conf.pmip_tunnel_mode == IP_FAMILY_V4_V01))
		pmipv4_mh_send((struct in_addr_bundle *)addrs, dummy_mh_vec, iov_count, oif, is_v4_pmip_mode);

	dbg("Copy PBU message into TEMP PMIP entry iovec....\n");
	//copy the PBU message into the mh_vector for the entry for future retransmissions.
	memcpy((void*)(bce->mh_vec), (void*)mh_vec, 2 * (IP6_MHOPT_MAX + 1)* sizeof(struct iovec));
	bce->iovlen = iovlen;
	//do not free, keep for retransmission free_iov_data(dummy_mh_vec, iov_count);
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mh_send_pba(void *addrs, pmip_entry_t * bce, struct timespec *lifetime, int oif, int is_v4_lma_mode)
{
	static struct iovec           mh_vec[2 * (IP6_MHOPT_MAX + 1)];
	static struct iovec           dummy_mh_vec[2 * (IP6_MHOPT_MAX + 1)];
	volatile int                  iovlen = 1;
	struct ip6_mh_binding_ack    *pba;
	uint16_t                      p_flag = 1;
	ip6mnid_t                     mn_iid;
	ip6mnid_mac_t                 mn_hardware_address;
	int                           iov_count;

	//bzero(mh_vec, sizeof(mh_vec));
	memset((void*)mh_vec       , 0, (2 * (IP6_MHOPT_MAX + 1))*sizeof(struct iovec));
	memset((void*)dummy_mh_vec , 0, (2 * (IP6_MHOPT_MAX + 1))*sizeof(struct iovec));
	pba = mh_create(&mh_vec[0], IP6_MH_TYPE_BACK);
	if (!pba) {
		dbg("mh_create() failed\n");
		return -ENOMEM;
	}
	dbg("Create PBA with lifetime = %d seconds\n", lifetime->tv_sec);
	pba->ip6mhba_status = bce->status;
	pba->ip6mhba_flags  = bce->PBA_flags;
	pba->ip6mhba_seqno  = htons(bce->seqno_in);
	pba->ip6mhba_lifetime = htons(lifetime->tv_sec >> 2);
	dbg("Create PBA options....\n");
	memcpy(&mn_iid, &bce->mn_suffix.s6_addr32[2], sizeof(ip6mnid_t));
	ipv6_to_mac(&bce->mn_hw_address, mn_hardware_address.mac_addr);
	mh_create_opt_mobile_node_identifier(&mh_vec[iovlen++], p_flag, &mn_hardware_address);
	if (bce->hasipv4_dmnp_prefix_option)
	{

		dbg("Create PBA adding DMNP before....\n");
		mh_create_opt_dmnp(&mh_vec[iovlen++], &bce->dmnp_prefix, &bce->dmnp_prefix_len);
	}

	if (bce->pmipv4_hnp_req_valid)
	{
		dbg("Create PBA adding home net prefix As PBU has the home v4 prefix request....\n");
		mh_create_opt_v4_home_adrr_reply(&mh_vec[iovlen++], &bce->v4_home_prefix, &bce->v4_home_prefixlen);
	}
	//commenting for this May be need in future
	//mh_create_opt_v4_default_router_opt(&mh_vec[iovlen++]);
	if (!is_v4_lma_mode)
	{
	mh_create_opt_home_net_prefix(&mh_vec[iovlen++], &bce->mn_prefix);
	}
	mh_create_opt_mobile_node_link_layer_identifier(&mh_vec[iovlen++], &mn_iid);
	mh_create_opt_time_stamp(&mh_vec[iovlen++], &bce->timestamp);
	mh_create_opt_gre(&mh_vec[iovlen++], GRE_KEY_VALID, DEFAULT_GRE_KEY);

	iov_count = mh_try_pad(mh_vec, dummy_mh_vec, iovlen);

	dbg("Send PBA with life time...%d\n",pba->ip6mhba_lifetime );

	//calculate the length of the message.
	pba->ip6mhba_hdr.ip6mh_hdrlen = mh_length(dummy_mh_vec, iov_count);
	dbg("Send PBA...\n");
	//if v6 tunnel
	if(conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)
		pmip_mh_send((struct in6_addr_bundle *)addrs, dummy_mh_vec, iov_count, oif, is_v4_lma_mode);
	//if v4 tunnel
	if  (conf.pmip_tunnel_mode == IP_FAMILY_V4_V01)
		pmipv4_mh_send((struct in_addr_bundle *)addrs, dummy_mh_vec, iov_count, oif, is_v4_lma_mode);

	free_iov_data(dummy_mh_vec, iov_count);
	return 0;
}

int pending_pbu_seq_list_del(uint16_t pbu_seq_no)
{
	struct pending_pbu_seq_list *seq_list = NULL;
	//find node
	seq_list = get_pbu_seq_node(pbu_seq_no);
	if (seq_list == NULL)
	{
		dbg("Unable to delete the node .Seq %d does not exits in list\n",pbu_seq_no);
		return -1;
	}
	else
	{
		dbg("Freeing the pbu sequence number:%d \n",pbu_seq_no);
		list_del(&seq_list->list);
		free(seq_list);
	}
	return 0;
}

int pending_pbu_seq_list_add(uint16_t pbu_seq_no)
{
	struct pending_pbu_seq_list *tmp = NULL;
	//find if node already exits
	tmp = get_pbu_seq_node(pbu_seq_no);
	if (tmp != NULL)
	{
		dbg("Duplicate pbu sequence number:%d . will not be added \n",pbu_seq_no);
		return -1;
	}
	tmp = NULL;
	tmp= (struct pending_pbu_seq_list *)malloc(sizeof(struct pending_pbu_seq_list));
	if (tmp == NULL)
	{
		dbg("error: Malloc failed\n");
		return -1;
	}
	tmp->pbu_seq_no = pbu_seq_no;
	INIT_LIST_HEAD(&tmp->list);
	list_add(&(tmp->list), &(conf.pbu_list));

	//list_del(&pbu_list->list);
	dbg("Added sequence number %d to list\n",pbu_seq_no);
	return 0;
	//list_add_tail(&pbu_list->list, &iface->expired_coas);
}

struct pending_pbu_seq_list *get_pbu_seq_node(uint16_t pbu_seq_no)
{
	struct list_head *node = NULL;
	struct pending_pbu_seq_list *seq_list = NULL;
	list_for_each(node, &conf.pbu_list)
	{
		//access the list node through node
		seq_list = list_entry(node, struct pending_pbu_seq_list, list);
		if (pbu_seq_no == seq_list->pbu_seq_no)
		{
			dbg("Match found for pbu seq %d\n",pbu_seq_no);
			return seq_list;
		}

	}
	return NULL;
}

void *delete_pending_pbu_seq_list()
{
	struct list_head *node = NULL;
	struct pending_pbu_seq_list *seq_list = NULL;
	list_for_each(node, &conf.pbu_list)
	{
		//access the list node through node
		seq_list = list_entry(node, struct pending_pbu_seq_list, list);
		if (seq_list != NULL)
		{
			list_del(&seq_list->list);
			free(seq_list);
		}
		else
		{
			dbg("Got NULL pbu sequence list pointer\n");
		}

	}
}

