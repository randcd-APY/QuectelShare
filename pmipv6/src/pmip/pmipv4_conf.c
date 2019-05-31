/*
   Copyright (c) 2017, The Linux Foundation. All rights reserved.
 */

/* dnsmasq is Copyright (c) 2000-2015 Simon Kelley

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 dated June, 1991, or
   (at your option) version 3 dated 29 June, 2007.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//attach filter & create thread to listen packet
#include "pmipv4_conf.h"
#include "pmip_types.h"
#include "debug.h"
#include "util.h"


/* filter to capture dhcp port 67 & ARP packet:   tcpdump "dst port 67 or arp" -dd   */
struct sock_filter code[] = {
	{ 0x28, 0, 0, 0x0000000c },
	{ 0x15, 0, 6, 0x000086dd },
	{ 0x30, 0, 0, 0x00000014 },
	{ 0x15, 2, 0, 0x00000084 },
	{ 0x15, 1, 0, 0x00000006 },
	{ 0x15, 0, 14, 0x00000011 },
	{ 0x28, 0, 0, 0x00000038 },
	{ 0x15, 11, 12, 0x00000043 },
	{ 0x15, 0, 9, 0x00000800 },
	{ 0x30, 0, 0, 0x00000017 },
	{ 0x15, 2, 0, 0x00000084 },
	{ 0x15, 1, 0, 0x00000006 },
	{ 0x15, 0, 7, 0x00000011 },
	{ 0x28, 0, 0, 0x00000014 },
	{ 0x45, 5, 0, 0x00001fff },
	{ 0xb1, 0, 0, 0x0000000e },
	{ 0x48, 0, 0, 0x00000010 },
	{ 0x15, 1, 2, 0x00000043 },
	{ 0x15, 0, 1, 0x00000806 },
	{ 0x6, 0, 0, 0x0000ffff },
	{ 0x6, 0, 0, 0x00000000 }
};

struct sock_fprog bpf = {
	.len = sizeof(code)/sizeof(struct sock_filter),
	.filter = code,
};

struct sock_dhcp_arp {
	pthread_mutex_t send_mutex;
	int fd;
};

static pthread_rwlock_t handler_lock;
static pthread_rwlock_t handlerv4_lock;

//static struct icmp6_handler *handlers[__ICMP6_SENTINEL + 1];

struct sock_dhcp_arp dhcp_arp_sock;
static pthread_t dhcp_arp_listener;


int create_send_socket_from_bridge0()
{
	int sock = socket (PF_PACKET,SOCK_RAW,IPPROTO_RAW);
	struct sockaddr_ll saddr, daddr;
	int saddr_size , data_size, daddr_size, bytes_sent;
	memset(&daddr, 0, sizeof(struct sockaddr_ll));
	daddr.sll_family = AF_PACKET;
	daddr.sll_protocol = htons(ETH_P_ALL);
	daddr.sll_ifindex = if_nametoindex(BRIDGE_IFACE);
	if (bind(sock, (struct sockaddr*) &daddr, sizeof(daddr)) < 0) {
		dbg("bind failed\n");
		close(sock);
	}

}
int create_dhcp_arp_listen_socket()
{
	int sock,ret;
	int sockopt;
	struct ifreq ifopts;	/* set promiscuous mode */
	sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sock < 0)
	{
		dbg("\nSocket creation failed\n");
		return -1;
	}
	else
	{
		//attach filter for dhcp pakcket
		ret = setsockopt(sock, SOL_SOCKET, SO_ATTACH_FILTER, &bpf, sizeof(bpf));
		if (ret < 0)
		{
			dbg ("\nSetting socket filter failed\n");
			close(sock);
			return -1;
		}

		//Set interface to promiscuous mode
		strncpy(ifopts.ifr_name, BRIDGE_IFACE, strlen(BRIDGE_IFACE)+1);
		ioctl(sock, SIOCGIFFLAGS, &ifopts);
		ifopts.ifr_flags |= IFF_PROMISC;
		ioctl(sock, SIOCSIFFLAGS, &ifopts);

		/* Allow the socket to be reused - incase connection is closed prematurely */
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof sockopt) == -1)
		{
			dbg("setsockopt failed for reuse socket");
			close(sock);
			return -1;
		}


		/* Bind to bridge0 */
		if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, BRIDGE_IFACE, strlen(BRIDGE_IFACE) + 1) == -1)
		{
			dbg("\nSO_BINDTODEVICE failed\n");
			close(sock);
			return -1;
		}
		return sock;
	}
}

int ipv4_arp_parse(msg_info_t * info, struct pktinfo *pkt_info)
{
	bzero(info, sizeof(msg_info_t));
	int hoplimit =20;
	info->hoplimit = hoplimit;
	char command[100];
	struct in_addr addr,target;
	int ret;

	if ((ntohs (pkt_info->arp_info.opcode) == ARPOP_REPLY) )
	{
		//dbg ("Got ARP Reply packet\n");
		ret = getifaceip(&addr,BRIDGE_IFACE);
		if (ret < 0)
		{
			dbg ("getting bridge ip failed\n");
			return -1;
		}
		snprintf(command,100,"%d.%d.%d.%d",pkt_info->arp_info.target_ip[0],pkt_info->arp_info.target_ip[1],pkt_info->arp_info.target_ip[2],pkt_info->arp_info.target_ip[3]);
		//dbg ("Target Ip %s\n",command);
		inet_aton(command,&target);
		if (target.s_addr == addr.s_addr)
		{
			//dbg ("Got ARP Reply packet target as bridge0 setting event hasARPReply\n");
			info->msg_event = hasARPReply;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		//dbg ("Not ARP reply packet\n");
		return -1;
	}
	mac_to_ipv6(pkt_info->ether_shost,&info->mn_hw_address);

	mac_to_ipv6(pkt_info->ether_shost,&info->mn_iid);
	//dbg("hwww address after pars: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&info->mn_hw_address));
	//dbg("MN id after pars: %x:%x:%x:%x:%x:%x:%x:%x ooo\n", NIP6ADDR(&info->mn_iid));

	return 0;
}


//---------------------------------------------------------------------------------------------------------------------
int ipv4_dhcp_parse(msg_info_t * info, struct pktinfo *pkt_info)
{
	bzero(info, sizeof(msg_info_t));
	int hoplimit =20;
	info->hoplimit = hoplimit;
	unsigned int mess_type = 0;
	unsigned char *opt;
	char command[100];
	/*print dhcp info*/
	//Getting dnsmasq message type
	if ((opt = option_find(&pkt_info->dhcpinfo, sizeof(struct dhcp_packet), OPTION_MESSAGE_TYPE, 1)))
	{
		mess_type = option_uint1(opt, 0, 1);
		print_dnsmasq_message_type(mess_type);
		if (mess_type == DHCPDISCOVER)
		{
			//dbg("--------DHCP discover-----------------------------\n");
		}
		else if (mess_type == DHCPREQUEST)
		{
			//dbg("--------DHCP Request-----------------------------\n");
			if ((opt = option_find(&pkt_info->dhcpinfo,  sizeof(struct dhcp_packet), OPTION_REQUESTED_IP, INADDRSZ)))
			{
				pkt_info->dhcpinfo.yiaddr = option_addr(opt);
				dbg ("Requested Ip for client =%s\n",inet_ntoa(pkt_info->dhcpinfo.yiaddr));
			}
			else
			{
				dbg ("Getting dnsmasq requsted Ip failed\n");
				return -1;
			}
		}

	}
	else
	{
		dbg ("Getting dnsmasq message type failed\n");
		return -1;
	}
	if (mess_type == DHCPDISCOVER )
		info->msg_event = hasDHCPDIS;
	else if (mess_type == DHCPREQUEST )
		info->msg_event = hasDHCPREQ;
	else
	{
		//dbg ("Invalid dhcp message type\n");
		return -1;
	}

	mac_to_ipv6(pkt_info->ether_shost,&info->mn_hw_address);

	mac_to_ipv6(pkt_info->ether_shost,&info->mn_iid);
	//storing requested Ip on Info
	info->dhcp_request_ip = pkt_info->dhcpinfo.yiaddr;
	//dbg("hwww address after pars: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&info->mn_hw_address));
	//dbg("MN id after pars: %x:%x:%x:%x:%x:%x:%x:%x ooo\n", NIP6ADDR(&info->mn_iid));

	return 0;
}

int update_arp_info(struct arp_hdr_info *arphdr,struct pktinfo *pkt_info)
{
	pkt_info->packet_type = ARP_PACKET;
	memcpy(&pkt_info->arp_info,arphdr,sizeof(struct arp_hdr_info ));
	return 0;
}

int update_dhcp_info(struct dhcp_packet *dhcpinfo1,struct pktinfo *pkt_info)
{
	unsigned char *opt;
	unsigned int mess_type = 0;
	unsigned char *end;
	memcpy(&pkt_info->dhcpinfo,dhcpinfo1,sizeof(struct dhcp_packet ));

	//Getting dnsmasq message type
	if ((opt = option_find(&pkt_info->dhcpinfo, sizeof(struct dhcp_packet), OPTION_MESSAGE_TYPE, 1)))
	{
		mess_type = option_uint1(opt, 0, 1);
		print_dnsmasq_message_type(mess_type);
		if ((mess_type != DHCPDISCOVER) && (mess_type != DHCPREQUEST))
		{
			//dbg ("Not DHCP Discover/Request packet\n");
			return -1;
		}
		else
		{
			pkt_info->packet_type = DHCP_PACKET;
			return 0;
		}
	}
	else
	{
		dbg ("Getting dnsmasq message type failed\n");
		return -1;
	}
	return 0;

}


int dhcp_arp_recv(int sock, unsigned char *buf, size_t msglen,
		struct pktinfo *pkt_info,
		int *hoplimit)
{
	/* Header structures */
	struct ether_header *eh;
	struct iphdr *iph;
	struct udphdr *udph;
	struct arp_hdr_info *arphdr;
	ssize_t numbytes;
	struct sockaddr_in ipsrc,ipdst;
	struct sockaddr_in dhcp_server_ip;
	int i= 0 ,ret;
	unsigned int lease_time = 6000;
	char command [200];
	struct dhcp_packet *dhcpinfo1;


//	dbg ("received on dnsmasq_ARP filter before receive from\n");
	numbytes = recvfrom(sock, buf, msglen, 0, NULL, NULL);
	eh = (struct ether_header *) buf;

	//stroing mac
	memcpy(pkt_info->ether_shost,eh->ether_shost,PMIP_MAC_ADDR_LEN);

	//checking  packet Type
	if (ntohs (eh->ether_type) == ETHERTYPE_ARP) {
		//dbg("Got ARP packet\n");
		arphdr = (struct arp_hdr_info *) (buf + sizeof(struct ether_header));
		ret = update_arp_info(arphdr, pkt_info);
		return ret;
	} else if (ntohs (eh->ether_type) == ETHERTYPE_IP) {
		//dbg("Got IP packet\n");
		iph = (struct iphdr *) (buf + sizeof(struct ether_header));

		//stroing IP
		pkt_info->ipsrc.sin_addr.s_addr = iph->saddr;
		pkt_info->ipdst.sin_addr.s_addr = iph->daddr;
		udph = (struct udphdr *) (buf + sizeof(struct iphdr) + sizeof(struct ether_header));
		if (ntohs(udph->dest) == DHCP_SERVER_PORT) {
			//dbg ("Got dhcp packet for port %d extract dnsmasq info\n", ntohs(udph->dest));
			dhcpinfo1 = (struct dhcp_packet *) (buf + sizeof(struct iphdr)+ sizeof(struct udphdr) + sizeof(struct ether_header));
			ret = update_dhcp_info(dhcpinfo1, pkt_info);
			return ret;
		} else {
			//dbg ("Invalid UDP packet received Not DHCP packet\n");
			return -1;
		}
	} else {
		//dbg ("Invalid packet received . Not DHCP/ARP\n");
		return -1;
	}
	//dhcpinfo1 = (struct dhcp_packet *) (buf + sizeof(struct iphdr)+ sizeof(struct udphdr) + sizeof(struct ether_header));

	return 0;
}


/*!
 * handler called when receiving a router solicitation
 */
//hip
static void pmip_mag_recv_dhcp_arp(struct pktinfo *pkt_info)
{
	int ret;
	char command[100];
	//dbg("\n");
	//dbg("Main function recieived ARP packet\n");
	//dbg("-------------------------------------\n");
	//dbg("Received : ARP from ip: %d.%d.%d.%d \n",
	//	 pkt_info->arp_info.sender_ip[0],pkt_info->arp_info.sender_ip[1],pkt_info->arp_info.sender_ip[2],pkt_info->arp_info.sender_ip[3]);
	//dbg("Received : ARP Target ip %d.%d.%d.%d \n",
	//	pkt_info->arp_info.target_ip[0],pkt_info->arp_info.target_ip[1],pkt_info->arp_info.target_ip[2],pkt_info->arp_info.target_ip[3]);
	msg_info_t info;
	bzero(&info, sizeof(info));
	if (pkt_info->packet_type == ARP_PACKET)
	{
		ret = ipv4_arp_parse(&info, pkt_info);
	}
	else if (pkt_info->packet_type == DHCP_PACKET)
	{
		ret = ipv4_dhcp_parse(&info, pkt_info);
	}
	if (ret ==0) {
		dbg("--------mag_fsm_v4-----------------------------\n");
		mag_fsm_v4(&info);
	} else {
		//dbg("pmip_mag_recv_dhcp Do not process dhcp packet . As it is not DHCP discover/Request/ARP\n");
	}
}


static void *dhcp_arp_listen(void *arg)
{
	uint8_t msg[MAX_PKT_LEN];
	struct sockaddr_in addr;

	struct pktinfo pkt_info;
	int iif, hoplimit;
	ssize_t len;
	int ret;

	dbg("dhcp thread started\n");

	while (1) {
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		ret = dhcp_arp_recv(dhcp_arp_sock.fd, msg, sizeof(msg),
				&pkt_info, &hoplimit);
		if (ret == -1)
		{
			//dbg ("Invalid packet received\n");
			continue;
		}

		/* multiplex to right handler */
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
		pthread_rwlock_rdlock(&handler_lock);
		pmip_mag_recv_dhcp_arp(&pkt_info);
		pthread_rwlock_unlock(&handler_lock);
	}
	pthread_exit(NULL);
}



int dhcp_init(void)
{
	pthread_mutexattr_t mattrs;
	int val;

	dhcp_arp_sock.fd = create_dhcp_arp_listen_socket();
	if (dhcp_arp_sock.fd < 0) {
		dbg("Unable to open dhcp arp socket! "
				"Do you have root permissions\n");
		return dhcp_arp_sock.fd;
	}

	/* create dhcp listener thread */
	pthread_mutexattr_init(&mattrs);
	//pthread_mutexattr_settype(&mattrs, PTHREAD_MUTEX_FAST_NP);
	if (pthread_mutex_init(&dhcp_arp_sock.send_mutex, &mattrs) ||
			pthread_rwlock_init(&handler_lock, NULL) ||
			pthread_create(&dhcp_arp_listener, NULL, dhcp_arp_listen, NULL))
		return -1;
	return 0;
}

int dhcp_cleanup()
{
	close(dhcp_arp_sock.fd);
	pthread_cancel(dhcp_arp_listener);
	pthread_join(dhcp_arp_listener, NULL);

}


unsigned char *option_find1(unsigned char *p, unsigned char *end, int opt, int minsize)
{
	while (*p != OPTION_END)
	{
		if (p >= end)
			return NULL; /* malformed packet */
		else if (*p == OPTION_PAD)
			p++;
		else
		{
			int opt_len;
			if (p >= end - 2)
				return NULL; /* malformed packet */
			opt_len = option_len(p);
			if (p >= end - (2 + opt_len))
				return NULL; /* malformed packet */
			if (*p == opt && opt_len >= minsize)
				return p;
			p += opt_len + 2;
		}
	}

	return opt == OPTION_END ? p : NULL;
}

unsigned char *option_find(struct dhcp_packet *mess, size_t size, int opt_type, int minsize)

{
	unsigned char *ret, *overload;

	/* skip over DHCP cookie; */
	if ((ret = option_find1(&mess->options[0], ((unsigned char *)mess) + size, opt_type, minsize)))
		return ret;

	/* look for overload option. */
	if (!(overload = option_find1(&mess->options[0], ((unsigned char *)mess) + size, OPTION_OVERLOAD, 1)))
		return NULL;

	/* Can we look in filename area ? */
	if ((overload[2] & 1) &&
			(ret = option_find1(&mess->file[0], &mess->file[128], opt_type, minsize)))
		return ret;

	/* finally try sname area */
	if ((overload[2] & 2) &&
			(ret = option_find1(&mess->sname[0], &mess->sname[64], opt_type, minsize)))
		return ret;

	return NULL;
}

struct in_addr option_addr(unsigned char *opt)
{
	/* this worries about unaligned data in the option. */
	/* struct in_addr is network byte order */
	struct in_addr ret;

	memcpy(&ret, option_ptr1(opt, 0), INADDRSZ);

	return ret;
}


unsigned int option_uint(unsigned char *opt, int size)
{
	/* this worries about unaligned data and byte order */
	unsigned int ret = 0;
	int i;
	unsigned char *p = option_ptr(opt);

	for (i = 0; i < size; i++)
		ret = (ret << 8) | *p++;

	return ret;
}

unsigned int option_uint1(unsigned char *opt, int offset, int size)
{
	/* this worries about unaligned data and byte order */
	unsigned int ret = 0;
	int i;
	unsigned char *p = option_ptr1(opt, offset);

	for (i = 0; i < size; i++)
		ret = (ret << 8) | *p++;

	return ret;
}

void print_dnsmasq_message_type(unsigned int mess_type)
{
	switch (mess_type)
	{
		case DHCPDISCOVER:
			dbg ("--------Got DHCP discover-----------------------------\n\n");
			break;
		case DHCPREQUEST:
			dbg ("--------Got DHCP Request-----------------------------\n");
			break;
		default:
			//dbg ("Invalid type packet not dhcp request/discover\n");
			break;


	}
}

