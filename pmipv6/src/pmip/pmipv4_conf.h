/*
   Copyright (c) 2017, The Linux Foundation. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2 and
   only version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
 */

#ifndef HEARDER_G
#define HEARDER_G

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <linux/filter.h>
#include <linux/types.h>


#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <errno.h>
#include <pthread.h>
#include "debug.h"


//dnsmasq options
#define OPTION_PAD               0
#define OPTION_REQUESTED_IP      50
#define OPTION_OVERLOAD          52
#define OPTION_MESSAGE_TYPE      53
#define OPTION_REQUESTED_OPTIONS 55
#define OPTION_MESSAGE           56
#define OPTION_END               255



//dnsmasq Message type
#define DHCPDISCOVER             1
#define DHCPREQUEST              3
#define IN6ADDRSZ       16
#define INADDRSZ        4

#define MAC_ADDR_LEN  6

#define option_len(opt) ((int)(((unsigned char *)(opt))[1]))
#define option_ptr(opt) ((void *)&(((unsigned char *)(opt))[2]))

#define option_len1(opt) ((int)(((unsigned char *)(opt))[1]))
#define option_ptr1(opt, i) ((void *)&(((unsigned char *)(opt))[2u+(unsigned int)(i)]))


#define DHCP_CHADDR_MAX          16


struct dhcp_packet {
	uint8_t op, htype, hlen, hops;
	uint32_t xid;
	uint16_t secs, flags;
	struct in_addr ciaddr, yiaddr, siaddr, giaddr;
	uint8_t chaddr[DHCP_CHADDR_MAX], sname[64], file[128];
	uint32_t cookie;
	unsigned char options[308];
};


unsigned char *option_find1(unsigned char *p, unsigned char *end, int opt, int minsize);

unsigned char *option_find(struct dhcp_packet *mess, size_t size, int opt_type, int minsize);
void print_dnsmasq_message_type(unsigned int mess_type);
unsigned int option_uint(unsigned char *opt, int size);
unsigned int option_uint1(unsigned char *opt, int offset, int size);
struct in_addr option_addr(unsigned char *opt);

typedef enum {
	TYPE_NOT_SET,
	DHCP_PACKET=1,
	ARP_PACKET
}packet_type_t;

struct arp_hdr_info {
	uint16_t htype;
	uint16_t ptype;
	uint8_t hlen;
	uint8_t plen;
	uint16_t opcode;
	uint8_t sender_mac[MAC_ADDR_LEN];
	uint8_t sender_ip[4];
	uint8_t target_mac[MAC_ADDR_LEN];
	uint8_t target_ip[4];
};


struct pktinfo {
	packet_type_t packet_type;
	struct sockaddr_in ipsrc;
	struct sockaddr_in ipdst;
	unsigned char 	ether_shost [MAC_ADDR_LEN];
	struct dhcp_packet dhcpinfo;
	struct arp_hdr_info arp_info;
};

#define hasDHCPREQ        0x00000001  /* Has a dhcp request */
#define hasDHCPDIS        0x00000002  /* Has a dhcp discover */
#define hasARPReply       0x00000003  /* Has a dhcp discover */

#define ETH_ALEN	6		/* Octets in one ethernet addr	 */
#define BUF_SIZE 1024
#define MAX_PKT_LEN 1024
#define DHCP_SERVER_PORT 67


#define BRIDGE_IFACE "bridge0"


#endif
