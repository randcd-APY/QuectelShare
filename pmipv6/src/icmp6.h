/* $Id: icmp6.h 1.17 06/05/07 21:52:43+03:00 anttit@tcs.hut.fi $ */

#ifndef __ICMP6_H__
#define __ICMP6_H__ 1

#include <netinet/icmp6.h>
#include <linux/types.h>
#include <linux/udp.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <errno.h>
#include <string.h>
#define PMIP_MAC_ADDR_LEN  6


#define BRIDGE_IFACE "bridge0"
#define MAX_DHCP_COMMAND_STR_LEN 4000

struct icmp6_handler {
	struct icmp6_handler *next;
	void (* recv)(const struct icmp6_hdr *ih,
		      ssize_t len,
		      const struct in6_addr *src,
		      const struct in6_addr *dst,
		      int iif,
		      int hoplimit,
		      unsigned char source_mac_addr [PMIP_MAC_ADDR_LEN]);
};

#ifdef FORWARD_DHCPV6_TO_TUNNEL
#define DHCP_SERVER_PORT_V6 547
struct ipv6hdr {
	__u8			priority:4,
				version:4;
	__u8			flow_lbl[3];

	__u16			payload_len;
	__u8			nexthdr;
	__u8			hop_limit;

	struct	in6_addr	saddr;
	struct	in6_addr	daddr;
};
#endif

#define ICMP6_MAIN_SOCK -1

int if_mc_group(int sock, int ifindex, const struct in6_addr *mc_addr,
		int cmd);

void icmp6_handler_reg(uint8_t type, struct icmp6_handler *handler);
void icmp6_handler_dereg(uint8_t type, struct icmp6_handler *handler);

int icmp6_init(void);
void icmp6_cleanup(void);

int icmp6_send(int oif, uint8_t hoplimit, const struct in6_addr *src,
	       const struct in6_addr *dst, struct iovec *datav, size_t iovlen);

ssize_t icmp6_recv(int sock, unsigned char *msg, size_t msglen,
		   struct sockaddr_in6 *addr, struct in6_pktinfo *pkt_info,
		   int *hoplimit, unsigned char source_mac_addr [PMIP_MAC_ADDR_LEN]);

void *icmp6_create(struct iovec *iov, uint8_t type, uint8_t code);

struct ip6_hdr;

int icmp6_parse_data(struct ip6_hdr *ip6h, unsigned int len,
		     struct in6_addr **lhoa, struct in6_addr **rhoa);

#ifdef FORWARD_DHCPV6_TO_TUNNEL
int dhcpv6_info_init(void);
static void *dhcpv6_info_listen(void *arg);
int dhcpv6_info_recv(int sockfd, unsigned char *buf, size_t msglen,
		unsigned char *ether_shost);
int forward_msg_to_tunnel(unsigned char *buf,ssize_t len, char *fname);
int create_sender_sock(int *sock, char *fname);
static void pmip_mag_dhcpv6_info_hand(unsigned char *buf, size_t msglen,
		unsigned char *ether_shost);



#endif

#endif
