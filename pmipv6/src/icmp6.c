/*
 * $Id: icmp6.c 1.42 06/05/06 15:15:47+03:00 anttit@tcs.hut.fi $
 *
 * This file is part of the MIPL Mobile IPv6 for Linux.
 *
 * Authors: Antti Tuominen <anttit@tcs.hut.fi>
 *          Ville Nuorvala <vnuorval@tcs.hut.fi>
 *
 * Copyright 2003-2005 Go-Core Project
 * Copyright 2003-2006 Helsinki University of Technology
 *
 * MIPL Mobile IPv6 for Linux is free software; you can redistribute
 * it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; version 2 of
 * the License.
 *
 * MIPL Mobile IPv6 for Linux is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MIPL Mobile IPv6 for Linux; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.
 */
/*
 * This file is part of the PMIP, Proxy Mobile IPv6 for Linux.
 *
 * Authors: OPENAIR3 <openair_tech@eurecom.fr>
 *
 * Copyright 2010-2011 EURECOM (Sophia-Antipolis, FRANCE)
 * 
 * Proxy Mobile IPv6 (or PMIPv6, or PMIP) is a network-based mobility 
 * management protocol standardized by IETF. It is a protocol for building 
 * a common and access technology independent of mobile core networks, 
 * accommodating various access technologies such as WiMAX, 3GPP, 3GPP2 
 * and WLAN based access architectures. Proxy Mobile IPv6 is the only 
 * network-based mobility management protocol standardized by IETF.
 * 
 * PMIP Proxy Mobile IPv6 for Linux has been built above MIPL free software;
 * which it involves that it is under the same terms of GNU General Public
 * License version 2. See MIPL terms condition if you need more details. 
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <pthread.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <netinet/icmp6.h>
#include "icmp6.h"
#include "util.h"
#include "debug.h"
#include "conf.h"
#include "pmip_cache.h"


enum {
    ICMP6_DU = 0,
    ICMP6_PP = 1,
    ICMP6_RA = 3,
    ICMP6_NA = 4,
    ICMP6_DRQ = 5,
    ICMP6_DRP = 6,
    ICMP6_MPS = 7,
    ICMP6_MPA = 8,
    ICMP6_NS  = 9,  // ADDED EURECOM
    ICMP6_RS  = 10, // ADDED EURECOM
    __ICMP6_SENTINEL = 11
};


static pthread_rwlock_t handler_lock;
static struct icmp6_handler *handlers[__ICMP6_SENTINEL + 1];

struct sock icmp6_sock;
static pthread_t icmp6_listener;

static pthread_rwlock_t handler_lock_dhcpv6_info;

#ifdef FORWARD_DHCPV6_TO_TUNNEL
struct sock dhcpv6_info_sock;
static pthread_t dhcpv6_info_sock_listener;
#endif

static inline int icmp6_type_map(uint8_t type)
{
	switch (type) {
	case ICMP6_DST_UNREACH:
		return ICMP6_DU;
	case ICMP6_PARAM_PROB:
		return ICMP6_PP;
	case ND_ROUTER_ADVERT:
		return ICMP6_RA;
	case ND_NEIGHBOR_ADVERT:
		return ICMP6_NA;
	case MIP_HA_DISCOVERY_REQUEST:
		return ICMP6_DRQ;
	case MIP_HA_DISCOVERY_REPLY:
		return ICMP6_DRP;
	case MIP_PREFIX_SOLICIT:
		return ICMP6_MPS;
	case MIP_PREFIX_ADVERT:
		return ICMP6_MPA;
    // Modified by EURECOM
    case ND_NEIGHBOR_SOLICIT:
        return ICMP6_NS;
    // added by EURECOM
    case ND_ROUTER_SOLICIT:
        return ICMP6_RS;
	default:
		return __ICMP6_SENTINEL;
	}
}

static inline struct icmp6_handler *icmp6_handler_get(uint8_t type)
{
	return handlers[icmp6_type_map(type)];
}

void icmp6_handler_reg(uint8_t type, struct icmp6_handler *handler)
{
	int i = icmp6_type_map(type);

	assert(handler->next == NULL);

	pthread_rwlock_wrlock(&handler_lock);
	handler->next = handlers[i];
	handlers[i] = handler;
	pthread_rwlock_unlock(&handler_lock);
}

void icmp6_handler_dereg(uint8_t type, struct icmp6_handler *handler)
{
	struct icmp6_handler **h;
	int i = icmp6_type_map(type);
	pthread_rwlock_wrlock(&handler_lock);
	h = &handlers[i];
	while (*h) {
		if (*h == handler) {
			*h = handler->next;
			handler->next = NULL;
			break;
		}
		h = &(*h)->next;
	}
	pthread_rwlock_unlock(&handler_lock);
}

/**
 * if_mc_group - join or leave multicast group
 * @sock: socket
 * @ifindex: interface to join/leave
 * @mc_addr: multicast address
 * @cmd: join/leave command
 *
 * Join/leave multicast group on interface.  cmd must be either
 * IPV6_JOIN_GROUP or IPV6_LEAVE_GROUP.  Also turns off local
 * multicast loopback.
 **/
int if_mc_group(int sock, int ifindex, const struct in6_addr *mc_addr, int cmd)
{
	unsigned int val = 0;
	struct ipv6_mreq mreq;
	int ret = 0;

	if (sock == ICMP6_MAIN_SOCK)
		sock = icmp6_sock.fd;

	memset(&mreq, 0, sizeof(mreq));
	mreq.ipv6mr_interface = ifindex;
	mreq.ipv6mr_multiaddr = *mc_addr;

	ret = setsockopt(sock, IPPROTO_IPV6, IPV6_MULTICAST_LOOP,
			 &val, sizeof(int));

	if (ret < 0) return ret;

	return setsockopt(sock, IPPROTO_IPV6, cmd, &mreq, sizeof(mreq));
}

static void *icmp6_listen(void *arg)
{
	uint8_t msg[MAX_PKT_LEN];
	struct sockaddr_in6 addr;
	struct in6_addr *saddr, *daddr;
	struct in6_pktinfo pkt_info;
	struct icmp6_hdr *ih;
	int iif, hoplimit;
	unsigned char source_mac_addr [PMIP_MAC_ADDR_LEN];
	ssize_t len;
	struct icmp6_handler *h;

	pthread_dbg("thread started");

	while (1) {
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		len = icmp6_recv(icmp6_sock.fd, msg, sizeof(msg),
				 &addr, &pkt_info, &hoplimit, &source_mac_addr);
		/* check if socket has closed */
		if (len == -EBADF)
			break;
		/* common validity check */
		if (len < sizeof(struct icmp6_hdr))
			continue;
		if (len < 0)
		{
			dbg("Invalid length %d . ignore icmpv6 packet\n");
			continue;
		}
		saddr = &addr.sin6_addr;
		daddr = &pkt_info.ipi6_addr;
		iif = pkt_info.ipi6_ifindex;

		ih = (struct icmp6_hdr *)msg;
		/* multiplex to right handler */
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
		pthread_rwlock_rdlock(&handler_lock);
		if ((h = icmp6_handler_get(ih->icmp6_type)) != NULL)
			h->recv(ih, len, saddr, daddr, iif, hoplimit, source_mac_addr);
		pthread_rwlock_unlock(&handler_lock);
	}
	pthread_exit(NULL);
}

int icmp6_init(void)
{
	struct icmp6_filter filter;
	pthread_mutexattr_t mattrs;
	int val;

	icmp6_sock.fd = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6);
	if (icmp6_sock.fd < 0) {
		syslog(LOG_ERR,
		       "Unable to open ICMPv6 socket! "
		       "Do you have root permissions?");
		return icmp6_sock.fd;
	}
	val = 1;
	if (setsockopt(icmp6_sock.fd, IPPROTO_IPV6, IPV6_RECVPKTINFO,
		       &val, sizeof(val)) < 0)
		return -1;
	if (setsockopt(icmp6_sock.fd, IPPROTO_IPV6, IPV6_RECVHOPLIMIT,
		       &val, sizeof(val)) < 0)
		return -1;
	//ICMP6_FILTER_SETBLOCKALL(&filter);
	//ICMP6_FILTER_SETPASS(ICMP6_DST_UNREACH, &filter);

	if (is_ha()) {
		ICMP6_FILTER_SETPASS(MIP_PREFIX_SOLICIT, &filter);
		ICMP6_FILTER_SETPASS(MIP_HA_DISCOVERY_REQUEST, &filter);
		ICMP6_FILTER_SETPASS(ND_ROUTER_ADVERT, &filter);
	}

	if (is_mn()) {
		ICMP6_FILTER_SETPASS(ND_ROUTER_ADVERT, &filter);
		ICMP6_FILTER_SETPASS(ND_NEIGHBOR_ADVERT, &filter);
		ICMP6_FILTER_SETPASS(MIP_PREFIX_ADVERT, &filter);
		ICMP6_FILTER_SETPASS(MIP_HA_DISCOVERY_REPLY, &filter);
		ICMP6_FILTER_SETPASS(ICMP6_PARAM_PROB, &filter);
	}
    //Added by EURECOM/Nghia for PMIP
    if (is_mag()) {
        ICMP6_FILTER_SETPASS(ND_NEIGHBOR_SOLICIT, &filter);
        ICMP6_FILTER_SETPASS(ND_NEIGHBOR_ADVERT, &filter);
        ICMP6_FILTER_SETPASS(ND_ROUTER_SOLICIT, &filter);
    }

	if (setsockopt(icmp6_sock.fd, IPPROTO_ICMPV6, ICMP6_FILTER,
		       &filter, sizeof(struct icmp6_filter)) < 0)
		return -1;
	val = 2;
	if (setsockopt(icmp6_sock.fd, IPPROTO_RAW, IPV6_CHECKSUM,
		       &val, sizeof(val)) < 0)
		return -1;
	/* create ICMP listener thread */
	pthread_mutexattr_init(&mattrs);
	pthread_mutexattr_settype(&mattrs, PTHREAD_MUTEX_FAST_NP);
	if (pthread_mutex_init(&icmp6_sock.send_mutex, &mattrs) ||
	    pthread_rwlock_init(&handler_lock, NULL) ||
	    pthread_create(&icmp6_listener, NULL, icmp6_listen, NULL))
		return -1;
	return 0;
}

void *icmp6_create(struct iovec *iov, uint8_t type, uint8_t code)
{
	struct icmp6_hdr *hdr;
	int msglen;

	switch (type) {
	case ICMP6_DST_UNREACH:
	case ICMP6_PACKET_TOO_BIG:
	case ICMP6_TIME_EXCEEDED:
	case ICMP6_PARAM_PROB:
		msglen = sizeof(struct icmp6_hdr);
		break;
	case ND_ROUTER_SOLICIT:
		msglen = sizeof(struct nd_router_solicit);
		break;
	case ND_ROUTER_ADVERT:
		msglen = sizeof(struct nd_router_advert);
		break;
	case ND_NEIGHBOR_SOLICIT:
		msglen = sizeof(struct nd_neighbor_solicit);
		break;
	case ND_NEIGHBOR_ADVERT:
		msglen = sizeof(struct nd_neighbor_advert);
		break;
	case ND_REDIRECT:
		msglen = sizeof(struct nd_redirect);
		break;
	default:
		msglen = sizeof(struct icmp6_hdr);
	}
	hdr = malloc(msglen);
	if (hdr == NULL)
		return NULL;

	memset(hdr, 0, msglen);
	hdr->icmp6_type = type;
	hdr->icmp6_code = code;
	iov->iov_base = hdr;
	iov->iov_len = msglen;

	return hdr;
}

int icmp6_send(int oif, uint8_t hoplimit,
	       const struct in6_addr *src, const struct in6_addr *dst,
	       struct iovec *datav, size_t iovlen)
{
	struct sockaddr_in6 daddr;
	struct msghdr msg;
	struct cmsghdr *cmsg;
	struct in6_pktinfo pinfo;
	int cmsglen, ret = 0, on = 1, hops;

	hops = (hoplimit == 0) ? 64 : hoplimit;

	memset(&daddr, 0, sizeof(struct sockaddr_in6));
	daddr.sin6_family = AF_INET6;
	daddr.sin6_addr = *dst;
	daddr.sin6_port = htons(IPPROTO_ICMPV6);

	memset(&pinfo, 0, sizeof(pinfo));
	pinfo.ipi6_addr = *src;
	if (oif > 0)
		pinfo.ipi6_ifindex = oif;

	cmsglen = CMSG_SPACE(sizeof(pinfo));
	cmsg = malloc(cmsglen);
	if (cmsg == NULL) {
		dbg("out of memory\n");
		return -ENOMEM;
	}
	cmsg->cmsg_len = CMSG_LEN(sizeof(pinfo));
	cmsg->cmsg_level = IPPROTO_IPV6;
	cmsg->cmsg_type = IPV6_PKTINFO;
	memcpy(CMSG_DATA(cmsg), &pinfo, sizeof(pinfo));

	msg.msg_control = cmsg;
	msg.msg_controllen = cmsglen;
	msg.msg_iov = datav;
	msg.msg_iovlen = iovlen;
	msg.msg_name = (void *)&daddr;
	msg.msg_namelen = CMSG_SPACE(sizeof(struct in6_pktinfo));

	pthread_mutex_lock(&icmp6_sock.send_mutex);
	setsockopt(icmp6_sock.fd, IPPROTO_IPV6, IPV6_PKTINFO,
		   &on, sizeof(int));
	setsockopt(icmp6_sock.fd, IPPROTO_IPV6, IPV6_UNICAST_HOPS,
		   &hops, sizeof(hops));
	setsockopt(icmp6_sock.fd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS,
		   &hops, sizeof(hops));

	ret = sendmsg(icmp6_sock.fd, &msg, 0);
	if (ret < 0)
		dbg("sendmsg: %s\n", strerror(errno));

	pthread_mutex_unlock(&icmp6_sock.send_mutex);

	free(cmsg);

	return ret;
}

#define CMSG_BUF_LEN 128
#define QCMAP_MSGR_MAC_ADDR_LEN_V01 6

ssize_t icmp6_recv(int sockfd, unsigned char *msg, size_t msglen,
		   struct sockaddr_in6 *addr, struct in6_pktinfo *pkt_info,
		   int *hoplimit, unsigned char source_mac_addr [PMIP_MAC_ADDR_LEN])
{
	struct msghdr mhdr;
	struct cmsghdr *cmsg;
	struct iovec iov;
	static unsigned char chdr[CMSG_BUF_LEN];
	ssize_t len;
	struct nd_opt_hdr *opt_hdr;
	int payload_len;
	int packet_size;
	struct icmp6_hdr *ih;
	struct ether_addr *mac_addr;
	int is_rs = 0 ;
	int is_rs_mac_successfully_parsed = 0;

	memset(source_mac_addr,0,sizeof(source_mac_addr));

	iov.iov_len = msglen;
	iov.iov_base = (unsigned char *) msg;

	mhdr.msg_name = (void *)addr;
	mhdr.msg_namelen = sizeof(struct sockaddr_in6);
	mhdr.msg_iov = &iov;
	mhdr.msg_iovlen = 1;
	mhdr.msg_control = (void *)chdr;
	mhdr.msg_controllen = CMSG_BUF_LEN;

	if ((len = recvmsg(sockfd, &mhdr, 0)) < 0)
		return -errno;

        for (cmsg = CMSG_FIRSTHDR(&mhdr); cmsg;
	     cmsg = CMSG_NXTHDR(&mhdr, cmsg)) {
		if (cmsg->cmsg_level != IPPROTO_IPV6)
			continue;
		switch(cmsg->cmsg_type) {
		case IPV6_HOPLIMIT:
			*hoplimit = *(int *)CMSG_DATA(cmsg);
			break;
		case IPV6_PKTINFO:
			memcpy(pkt_info, CMSG_DATA(cmsg), sizeof(*pkt_info));
			break;
		}
	}

	ih = (struct icmp6_hdr *) msg;
	if (ih->icmp6_type == ND_ROUTER_SOLICIT)
	{
		dbg("Router solicitation received\n");
		is_rs = 1;
		opt_hdr = (struct nd_opt_hdr *) (msg + sizeof(struct nd_router_solicit));
		payload_len = len;
		packet_size = sizeof(struct nd_router_solicit);
		while(opt_hdr != NULL && opt_hdr->nd_opt_len != 0 && payload_len > 0)
		{
			packet_size += (opt_hdr->nd_opt_len * 8);

			if (opt_hdr->nd_opt_type == ND_OPT_SOURCE_LINKADDR)
			{
				mac_addr = (struct ether_addr*) (msg  + (packet_size - QCMAP_MSGR_MAC_ADDR_LEN_V01));
				//stroing mac
				memcpy(source_mac_addr,mac_addr,PMIP_MAC_ADDR_LEN);
				dbg("Router solicitation sender MAC %s\n", ether_ntoa(mac_addr));
				is_rs_mac_successfully_parsed = 1;
				break;
			}

			opt_hdr = (struct nd_opt_hdr *) (opt_hdr + opt_hdr->nd_opt_len);
			payload_len -= opt_hdr->nd_opt_len;
		}
	}
	if (is_rs && (is_rs_mac_successfully_parsed != 1))
	{
		dbg("Router solicitation Do Not have Sender MAc option.Ignore RS\n");
		return -1;
	}
	return len;
}

struct ip6_subopt_hdr {
	u_int8_t	opttype;
	u_int8_t	optlen;
};

int icmp6_parse_data(struct ip6_hdr *ip6h, unsigned int len,
		     struct in6_addr **lhoa, struct in6_addr **rhoa)
{
	uint8_t *data = (uint8_t *)ip6h;
	unsigned int hoff = sizeof(struct ip6_hdr);
	uint8_t htype;

	if (len < hoff)
		return -1;

	htype = ip6h->ip6_nxt;

	/* The minumum length of an extension header is eight octets,
	   so check that we at least have room for that */
	while (hoff + 8 < len) {
		struct ip6_ext *h = (struct ip6_ext *) (data + hoff);
		unsigned int hlen = (h->ip6e_len + 1) << 3;

		if (htype != IPPROTO_DSTOPTS &&
		    htype != IPPROTO_ROUTING &&
		    htype != IPPROTO_HOPOPTS)
			return 0;

		if (len < hoff + hlen)
			return -1;

		if (htype == IPPROTO_DSTOPTS) {
			uint8_t *odata = (uint8_t *) h;
			uint32_t ooff = 2;
			while (ooff < hlen) {
				struct ip6_subopt_hdr *o;
				o = (struct ip6_subopt_hdr *) (odata + ooff);
				if (o->opttype == IP6OPT_PAD0) {
					ooff++;
					continue;
				}
				/* invalid TLV option length */
				if (hlen < ooff + 2 ||
				    hlen < ooff + 2 + o->optlen)
					break;
				if (o->opttype == IP6OPT_HOME_ADDRESS &&
				    o->optlen == sizeof(struct in6_addr)) {
					*lhoa = (struct in6_addr *) (o + 1);
					dbg("HAO %x:%x:%x:%x:%x:%x:%x:%x\n",
					    NIP6ADDR(*lhoa));
					break;
				}
				ooff += 2 + o->optlen;
			}
		} else if (htype == IPPROTO_ROUTING) {
			struct ip6_rthdr2 *rth = (struct ip6_rthdr2 *) h;
			if (rth->ip6r2_type == 2 &&
			rth->ip6r2_len == 2 && rth->ip6r2_segleft == 1)
				*rhoa = &rth->ip6r2_homeaddr;
			dbg("RTH2 %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(*rhoa));
		} else if (htype != IPPROTO_HOPOPTS) {
			return 0;
		}
		hoff += hlen;
		htype = h->ip6e_nxt;
	}
	return 0;
}

void icmp6_cleanup(void)
{
	close(icmp6_sock.fd);
	pthread_cancel(icmp6_listener);
	pthread_join(icmp6_listener, NULL);
}
int pmip_add_dnsmasq_v6dns_option(char * dhcp_command)
{
	char command[200] = {0};
	char buff[200] = {0};
	char base[200] = {0};
	char pridnsaddr[INET6_ADDRSTRLEN] = {0};
	char secdnsaddr[INET6_ADDRSTRLEN] = {0};
	char *dnsv6_option = "option6:dns-server";
	int haspridns = 0;
	int hassecdns = 0;
	dbg(":Enter\n");
	//check if dhcp dns option is alreday there
	if((strstr(dhcp_command, dnsv6_option)) != NULL)
	{
		dbg ("error: dhcpv6 dns option already there\n");
		return -1;
	}
	inet_ntop(AF_INET6, &conf.PrimaryV6Dns, pridnsaddr, INET6_ADDRSTRLEN);

	snprintf(base, 200," --dhcp-range=%s,%s " , "::","::");
	if (strncmp (pridnsaddr, "::", sizeof("::")))
	{
		haspridns = 1;
		snprintf(command, 200,"%s --dhcp-option=option6:dns-server,[%s]",base,pridnsaddr);
		dbg ("Added primary dns option\n");
	}
	inet_ntop(AF_INET6, &conf.SecondaryV6Dns, secdnsaddr, INET6_ADDRSTRLEN);
	if (strncmp (secdnsaddr, "::", sizeof("::")))
	{
		hassecdns = 1;
		if (haspridns)
		{
			dbg ("Added secondary dns option\n");
			snprintf(buff, 200,",[%s] ",secdnsaddr);
			strcat(command,buff);
		}
		else
		{
			dbg ("Adding only secondary dns option\n");
			snprintf(command, 200,"%s --dhcp-option=option6:dns-server,[%s] ",base,secdnsaddr);
		}

	}
	if (!pridnsaddr && !hassecdns)
	{
		dbg("Error: Both dns is zero. dnsmasq will not provide the v6 dns\n");
		return -1;
	}

	strcat(dhcp_command, command);
	return 1;
}
int pmipv6_start_dnsmasq_with_dhcpv6_option()
{
	FILE* fp;
	const char* check_cmd = "ps -o args | grep dnsmasq | grep -v grep";
	char dhcp_command[MAX_DHCP_COMMAND_STR_LEN] = {0};

	//Read current dnsmasq options
	if((fp = popen(check_cmd, "r")) == NULL)
	{
		dbg("Error checking dnsmasq \n", errno, 0, 0);
		pclose(fp);
		return -1;
	}

	if(fgets(dhcp_command, sizeof(dhcp_command), fp))
	{
		dbg("Reading dnsmasq command from ps succesed=%s=\n",dhcp_command);
		pclose(fp);
		dhcp_command[strlen(dhcp_command) - 1] = '\0';
		if (pmip_add_dnsmasq_v6dns_option(dhcp_command) < 0)
		{
			dbg("error: pmip_add_dnsmasq_v6dns_option failed\n");
			return -1;
		}
		pmip_system_call("killall -9 dnsmasq",strlen("killall -9 dnsmasq"),1);
		pmip_system_call(dhcp_command,strlen(dhcp_command),1);
		return 1;
	}
	dbg("Error Reading dnsmasq command from ps failed\n");
	pclose(fp);
	return -1;
}

//comment code to forward dhcpv6 packet to tunnel may be needed in future
#ifdef FORWARD_DHCPV6_TO_TUNNEL
static void pmip_mag_dhcpv6_info_hand(unsigned char *buf, size_t msglen,
		unsigned char *ether_shost)
{
	int ret;
	char command[100];
	dbg("in pmip_mag_dhcpv6_info_hand\n");
	pmip_entry_t *bce;
	struct in6_addr hw_address;
	char fname[IFNAMSIZ]={0};
	int type;

	msg_info_t info;
	bzero(&info, sizeof(info));

	mac_to_ipv6(ether_shost,&hw_address);
	if ((conf.pmip_mode_type== IP_FAMILY_V6_V01 || conf.pmip_mode_type== IP_FAMILY_V4V6_V01))
	{
		type = pmip_cache_exists(&conf.OurAddress, &hw_address);
		if (type == BCE_NO_ENTRY)
		{
			dbg("ipv6 entry not created return\n");
			return;
		}
		bce = pmip_cache_get(&conf.OurAddress, &hw_address);
		pmipcache_release_entry(bce);
		if (bce == NULL)
		{
			dbg("ipv6 bce entry not found return\n");
			return;
		}
		if (conf.pmip_tunnel_mode== IP_FAMILY_V4_V01)
		{
			ret = tunnelv4_getname(bce->tunnel_v4,fname);
			if (ret == 0)
			{
				dbg("ipv6 Get Tunnel name failed for v4 tunnel do not forward packet\n");
				return;
			}
			else
			{
				dbg("ipv6 got tunnel name = %s\n",fname);
				//forward packet to tunnel interface from ip header
				forward_msg_to_tunnel((buf + sizeof(struct ether_header)),(msglen-sizeof(struct ether_header)),fname);
			}
		}
		else if (conf.pmip_tunnel_mode== IP_FAMILY_V6_V01)
		{
			ret = tunnel_getname(bce->tunnel,fname);
			if (ret == 0)
			{
				dbg("ipv6 Get Tunnel name failed for v6 tunnel do not forward packet\n");
				return;
			}
			else
			{
				dbg("ipv6 got tunnel name = %s\n",fname);
				//forward packet to tunnel interface from ip header
				forward_msg_to_tunnel((buf + sizeof(struct ether_header)),(msglen-sizeof(struct ether_header)),fname);
			}
		}
	}
	else
	{
	}
}

int create_sender_sock(int *sock, char *fname)
{
	struct sockaddr_ll daddr;
	*sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (*sock < 0)
	{
		dbg("ipv6 Socket cteation failed\n");
		return -1;;
	}
	//get tunnel name

	memset(&daddr, 0, sizeof(struct sockaddr_ll));
	daddr.sll_family = AF_PACKET;
	daddr.sll_protocol = htons(ETHERTYPE_IPV6);;
	daddr.sll_ifindex = if_nametoindex(fname);
	if (bind(*sock, (struct sockaddr*) &daddr, sizeof(daddr)) < 0) {
		dbg("ipv6 bind failed\n");
		close(*sock);
	}

	if (setsockopt(*sock, SOL_SOCKET, SO_BINDTODEVICE, fname, (socklen_t)strlen(fname)) < 0)
	{
		dbg("ipv6  %s: Unable to bind socket to interface %s errnum=%d,msg=%s\n", __func__, fname,errno,strerror(errno));
		return -1;
	}
	dbg("ipv6 create sucess\n");
}

int forward_msg_to_tunnel(unsigned char *buf,ssize_t len, char *fname)
{
	struct msghdr mhdr;
	struct cmsghdr *cmsg;
	struct iovec iov;
	static unsigned char chdr[CMSG_BUF_LEN];
	struct sockaddr_in6 sdaddr;
	struct ipv6hdr *iph6;
	int send_sock;
	create_sender_sock(&send_sock, fname);
	int bytes_sent;
	dbg("ipv6 Got len=%d\n",len);
	bytes_sent=write(send_sock,buf,len);
	dbg("ipv6 Sent %d bytes\n",bytes_sent);
	if (bytes_sent < 0) {
		dbg("ipv6 write failed errnum=%d,msg=%s\n",errno,strerror(errno));
	}
	else
	{
		dbg("ipv6 write ok to iface=%s except eth header=====\n",fname);
	}
	close(send_sock);

}

int dhcpv6_info_recv(int sockfd, unsigned char *buf, size_t msglen,
		unsigned char *ether_shost)
{
	/* Header structures */
	struct ether_header *eh;
	struct ipv6hdr *iph6;
	struct udphdr *udph;
	struct arp_hdr_info *arphdr;
	ssize_t numbytes;
	struct sockaddr_in ipsrc,ipdst;
	struct sockaddr_in dhcp_server_ip;
	int i= 0 ,ret;
	unsigned int lease_time = 6000;
	char command [200];
	struct dhcp_packet *dhcpinfo1;

	ssize_t len;

	len = recvfrom(sockfd, buf, msglen, 0, NULL, NULL);
	eh = (struct ether_header *) buf;

	eh = (struct ether_header *) buf;
	//cheching packet type
	if ((conf.pmip_mode_type == IP_FAMILY_V6_V01 || conf.pmip_mode_type == IP_FAMILY_V4V6_V01)
			&& ntohs (eh->ether_type) == ETHERTYPE_IPV6)
	{
		dbg("ip ipv6 packet\n");
		//stroing mac
		memcpy(ether_shost,eh->ether_shost,PMIP_MAC_ADDR_LEN);
		iph6 = (struct ip6hdr *) (buf + sizeof(struct ether_header));
		udph = (struct udphdr *) (buf + sizeof(struct ipv6hdr) + sizeof(struct ether_header));
		if (ntohs(udph->dest) == DHCP_SERVER_PORT_V6)
		{
			dbg ("ipv6 Got multicast for port %d will send to tunnel\n", ntohs(udph->dest));
			return len;
		}
		else
		{
			//ipv6 Invalid UDP packet received Not DHCPv6 packet
			return -1;
		}
	}

	return -1;
}


static void *dhcpv6_info_listen(void *arg)
{
	uint8_t msg[MAX_PKT_LEN];
	unsigned char ether_shost [PMIP_MAC_ADDR_LEN];

	int iif;
	ssize_t len;
	int ret;

	dbg("dhcpv6_info_listen thread started\n");

	while (1) {
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		len = dhcpv6_info_recv(dhcpv6_info_sock.fd, msg, sizeof(msg),ether_shost);
		if (len == -1)
		{
			//dbg ("Invalid packet received\n");
			continue;
		}

		/* multiplex to right handler */
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
		pthread_rwlock_rdlock(&handler_lock_dhcpv6_info);
		pmip_mag_dhcpv6_info_hand(msg , len ,ether_shost);
		pthread_rwlock_unlock(&handler_lock_dhcpv6_info);
	}
	pthread_exit(NULL);
}

int dhcpv6_info_init(void)
{
	pthread_mutexattr_t mattrs;
	int val=1;
	int sockopt;

	dhcpv6_info_sock.fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (dhcpv6_info_sock.fd < 0) {
		syslog(LOG_ERR,
				"Unable to open Multicast socket! "
				"Do you have root permissions?");
		return dhcpv6_info_sock.fd;

	}
	/* Bind to bridge0 */
	if (setsockopt(dhcpv6_info_sock.fd, SOL_SOCKET, SO_BINDTODEVICE, BRIDGE_IFACE, strlen(BRIDGE_IFACE) + 1) == -1)
	{
		dbg("SO_BINDTODEVICE failed\n");
		close(dhcpv6_info_sock.fd);
		return -1;
	}
	else
	{
		dbg("bind to bridge succesed\n");
	}

	/* create muticast listener thread */
	pthread_mutexattr_init(&mattrs);
	//pthread_mutexattr_settype(&mattrs, PTHREAD_MUTEX_FAST_NP);
	if (pthread_mutex_init(&dhcpv6_info_sock.send_mutex, &mattrs) ||
			pthread_rwlock_init(&handler_lock_dhcpv6_info, NULL) ||
			pthread_create(&dhcpv6_info_sock_listener, NULL, dhcpv6_info_listen, NULL))
		return -1;
	return 0;
}
#endif
