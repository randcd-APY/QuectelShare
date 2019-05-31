/*! \file pmip_handler.c
 * \brief
 * \author OpenAir3 Group
 * \date 12th of October 2010
 * \version 1.0
 * \company Eurecom
 * \project OpenAirInterface
 * \email: openair3@eurecom.fr
 */
#define PMIP
#define PMIP_HANDLER_C
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
//---------------------------------------------------------------------------------------------------------------------
#include "pmip_fsm.h"
#include "pmip_handler.h"
#include "pmip_lma_proc.h"
#include "pmip_mag_proc.h"
#include "pmip_msgs.h"
//---------------------------------------------------------------------------------------------------------------------
#include "ndisc.h"
#ifdef ENABLE_VT
#    include "vt.h"
#endif
#include "debug.h"
#include "conf.h"
#include "pmipv4_conf.h"

//---------------------------------------------------------------------------------------------------------------------
struct in6_addr *link_local_addr(struct in6_addr *id)
{
	static struct in6_addr ADDR;
	ADDR = in6addr_any;
	ADDR.s6_addr32[0] = htonl(0xfe800000);
	//copy the MN_ID.
	memcpy(&ADDR.s6_addr32[2], &id->s6_addr32[2], sizeof(ip6mnid_t));
	return &ADDR;
}
//---------------------------------------------------------------------------------------------------------------------
struct in6_addr *CONVERT_ID2ADDR(struct in6_addr *result, struct in6_addr *prefix, struct in6_addr *id)
{
	*result = in6addr_any;
	memcpy(&result->s6_addr32[0], &prefix->s6_addr32[0], sizeof(ip6mnid_t));
	memcpy(&result->s6_addr32[2], &id->s6_addr32[2], sizeof(ip6mnid_t));
	return result;
}
//---------------------------------------------------------------------------------------------------------------------
struct in6_addr *get_mn_addr(pmip_entry_t * bce)
{
	CONVERT_ID2ADDR(&bce->mn_addr, &bce->mn_prefix, &bce->mn_suffix);
	return &bce->mn_addr;
}
//---------------------------------------------------------------------------------------------------------------------
struct in6_addr *solicited_mcast(struct in6_addr *id)
{
	//NUD_ADDR converts an ID into a Multicast Address for NS Unreachability!
	static struct in6_addr ADDR2;
	ADDR2 = in6addr_any;
	ADDR2.s6_addr32[0] = htonl(0xff020000);
	ADDR2.s6_addr32[1] = htonl(0x00000000);
	ADDR2.s6_addr32[2] = htonl(0x00000001);
	ADDR2.s6_addr[12] = 0xff;
	//copy the least 24 bits from the MN_ID.
	memcpy(&ADDR2.s6_addr[13], &id->s6_addr[13], 3 * sizeof(ADDR2.s6_addr[0]));
	return &ADDR2;
}
//---------------------------------------------------------------------------------------------------------------------
void pmip_timer_retrans_pbu_handler(struct tq_elem *tqe)
{
	int mutex_return_code;
	int ret = -1;

	dbg("pmip_timer_retrans_pbu_handler : %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&conf.OurAddress));
		dbg("pmip_timer_retrans_pbu_handler v6 lock\n");
		mutex_return_code = pthread_rwlock_wrlock(&pmip_lock);

	if (mutex_return_code != 0) {
		dbg("pthread_rwlock_wrlock(&pmip_lock) %s\n", strerror(mutex_return_code));
	}
	printf("-------------------------------------\n");
	if (!task_interrupted()) {
		pmip_entry_t *e = tq_data(tqe, pmip_entry_t, tqe);
		mutex_return_code = pthread_rwlock_wrlock(&e->lock);
		if (mutex_return_code != 0) {
			dbg("pthread_rwlock_wrlock(&e->lock) %s\n", strerror(mutex_return_code));
		}
		dbg("Retransmissions counter : %d\n", e->n_rets_counter);
		if (e->n_rets_counter == 0) {
			free_iov_data((struct iovec *) &e->mh_vec, e->iovlen);
			dbg("No PBA received from LMA....\n");
			dbg("Abort Trasmitting the PBU....\n");
			mutex_return_code = pthread_rwlock_unlock(&e->lock);
			if (mutex_return_code != 0) {
				dbg("pthread_rwlock_unlock(&e->lock) %s\n", strerror(mutex_return_code));
			}
			dbg ("pmipv6 pmip_bce_delete\n");
			pmip_bce_delete(e);
		} else {
			//Decrement the N trasnmissions counter.
			e->n_rets_counter--;
			struct in6_addr_bundle addrs;
			struct in_addr_bundle addrs1;
			memset(&addrs, 0, sizeof(addrs));
			memset(&addrs1, 0, sizeof(addrs1));
			//sends a PBU
			dbg("Send PBU again....\n");

			// INCREMENT SEQ NUMBER OF PBU
			e->seqno_out        = get_new_pbu_sequence_number();
			ret = pending_pbu_seq_list_add(e->seqno_out);
			if (ret < 0)
			{
				dbg("pbu sequence number addition to list failed\n");
			}
			else
			{
				dbg("pbu sequence number addition to list successed\n");
			}
			((struct ip6_mh_binding_update *)(e->mh_vec[0].iov_base))->ip6mhbu_seqno = htons(e->seqno_out);
			if (conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)
			{
				addrs.src = &conf.OurAddress;
				addrs.dst = &conf.LmaAddress;
				pmip_mh_send(&addrs, e->mh_vec, e->iovlen, 0, 0);

			}
			if  (conf.pmip_tunnel_mode == IP_FAMILY_V4_V01)
			{
				//store v4 address in v6 format will parse back v4 while sending packet
				addrs1.src = &conf.Magv4AddressEgress;
				addrs1.dst = &conf.LmaV4Address;
				pmipv4_mh_send(&addrs1, e->mh_vec, e->iovlen, 0, 0);
			}

			//add a new task for PBU retransmission.
			struct timespec expires;
			clock_gettime(CLOCK_REALTIME, &e->add_time);
			tsadd(e->add_time, conf.RetransmissionTimeOut, expires);
			add_task_abs(&expires, &e->tqe, pmip_timer_retrans_pbu_handler);
			dbg("PBU Retransmissions timer is triggered again....\n");
			mutex_return_code = pthread_rwlock_unlock(&e->lock);
			if (mutex_return_code != 0) {
				dbg("pthread_rwlock_unlock(&e->lock) %s\n", strerror(mutex_return_code));
	}
		}
	}
	dbg("pmip_timer_retrans_pbu_handler v6 unlock\n");
	mutex_return_code = pthread_rwlock_unlock(&pmip_lock);
	if (mutex_return_code != 0) {
		dbg("pthread_rwlock_unlock(&pmip_lock) %s\n", strerror(mutex_return_code));
	}
}

//---------------------------------------------------------------------------------------------------------------------
void pmip_timer_retrans_pbu_handlerv4(struct tq_elem *tqe)
{
	int mutex_return_code;
	int ret = 0;

	dbg("pmip_timer_retrans_pbu_handlerv4 : %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&conf.OurAddress));

		dbg("pmip_timer_retrans_pbu_handler v4 lock\n");
		mutex_return_code = pthread_rwlock_wrlock(&pmip_v4_lock);

	if (mutex_return_code != 0) {
		dbg("pthread_rwlock_wrlock(&pmip_lock) %s\n", strerror(mutex_return_code));
	}
	printf("-------------------------------------\n");
	if (!task_interrupted()) {
		pmip_entry_t *e = tq_data(tqe, pmip_entry_t, tqe);
		mutex_return_code = pthread_rwlock_wrlock(&e->lock);
		if (mutex_return_code != 0) {
			dbg("pthread_rwlock_wrlock(&e->lock) %s\n", strerror(mutex_return_code));
		}
		dbg("Retransmissions counter : %d\n", e->n_rets_counter);
		if (e->n_rets_counter == 0) {
			free_iov_data((struct iovec *) &e->mh_vec, e->iovlen);
			dbg("No PBA received from LMA....\n");
			dbg("Abort Trasmitting the PBU....\n");
			mutex_return_code = pthread_rwlock_unlock(&e->lock);
			if (mutex_return_code != 0) {
				dbg("pthread_rwlock_unlock(&e->lock) %s\n", strerror(mutex_return_code));
			}
				dbg ("pmipv4 pmipv4_bce_delete\n");
				pmipv4_bce_delete(e);

		} else {
			//Decrement the N trasnmissions counter.
			e->n_rets_counter--;
			struct in6_addr_bundle addrs;
			struct in_addr_bundle addrs1;
			memset(&addrs, 0, sizeof(addrs));
			memset(&addrs1, 0, sizeof(addrs1));

			//sends a PBU
			dbg("Send PBU again....\n");

			// INCREMENT SEQ NUMBER OF PBU
			e->seqno_out        = get_new_pbu_sequence_number();
			ret = pending_pbu_seq_list_add(e->seqno_out);
			if (ret < 0)
			{
				dbg("pbu sequence number addition to list failed\n");
			}
			else
			{
				dbg("pbu sequence number addition to list successed\n");
			}
			((struct ip6_mh_binding_update *)(e->mh_vec[0].iov_base))->ip6mhbu_seqno = htons(e->seqno_out);
			if (conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)
			{
				addrs.src = &conf.OurAddress;
				addrs.dst = &conf.LmaAddress;
				pmip_mh_send(&addrs, e->mh_vec, e->iovlen, 0, 1);

			}
			if  (conf.pmip_tunnel_mode == IP_FAMILY_V4_V01)
			{
				//store v4 address in v6 format will parse back v4 while sending packet
				addrs1.src = &conf.Magv4AddressEgress;
				addrs1.dst = &conf.LmaV4Address;
				pmipv4_mh_send(&addrs1, e->mh_vec, e->iovlen, 0, 1);
			}

			//add a new task for PBU retransmission.
			struct timespec expires;
			clock_gettime(CLOCK_REALTIME, &e->add_time);
			tsadd(e->add_time, conf.RetransmissionTimeOut, expires);
			add_task_abs(&expires, &e->tqe, pmip_timer_retrans_pbu_handlerv4);
			dbg("PBU Retransmissions timer is triggered again....\n");
			mutex_return_code = pthread_rwlock_unlock(&e->lock);
			if (mutex_return_code != 0) {
				dbg("pthread_rwlock_unlock(&e->lock) %s\n", strerror(mutex_return_code));
			}
		}
	}


		dbg("pmip_timer_retrans_pbu_handler v4 unlock\n");
		mutex_return_code = pthread_rwlock_unlock(&pmip_v4_lock);
	if (mutex_return_code != 0) {
		dbg("pthread_rwlock_unlock(&pmip_lock) %s\n", strerror(mutex_return_code));
	}

}

int send_arp_from_bridge(struct in_addr arp_ip)
{
	int fd, ret;
	struct ifreq ifr;
	uint32_t own_ip;
	int if_index;
	struct sockaddr_ll sa;
	struct packet pkt;
	dbg ("send_arp_from_bridge enter\n");
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if( fd < 0 ) {
		dbg("IPv4 Socket creation failed\n");
		return -1;
	}

	// get Bridge Ip address
	memcpy(ifr.ifr_name, BRIDGE_IFACE, IF_NAMESIZE);
	ret = ioctl(fd, SIOCGIFADDR, &ifr, sizeof(ifr));
	if( ret < 0 ) {
		dbg("SIOCGIFADDR failed\n");
		return -1;
	}
	struct sockaddr_in *sin = (struct sockaddr_in *)&ifr.ifr_addr;
	own_ip = ntohl(sin->sin_addr.s_addr);


	// get bridge0 index
	ret = ioctl(fd, SIOCGIFINDEX, &ifr, sizeof(ifr));
	if( ret < 0 ) {
		dbg("IOCTL index getting failed\n");
		return -1;
	}
	if_index = ifr.ifr_ifindex;


	// get bridge0 MAC address
	ret = ioctl(fd, SIOCGIFHWADDR, &ifr, sizeof(ifr));
	if( ret < 0 ) {
		dbg("IOCTL mac adress getting failed\n");
		return -1;
	}

	close(fd);

	// Socket  for arp packet
	fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
	if( fd == -1 ) {
		dbg("RAW Socket creation failed\n");
		return -1;
	}
	memset(pkt.ether.ether_dhost, 0xFF, sizeof(pkt.ether.ether_dhost));
	memcpy(pkt.ether.ether_shost, ifr.ifr_hwaddr.sa_data, sizeof(pkt.ether.ether_dhost));

	pkt.ether.ether_type = htons(ETHERTYPE_ARP);
	pkt.arp.ar_hrd = htons(ARPHRD_ETHER);
	pkt.arp.ar_pro = htons(ETHERTYPE_IP);
	pkt.arp.ar_hln = PMIP_MAC_ADDR_LEN;
	pkt.arp.ar_pln = sizeof(pkt.sender_ip);
	pkt.arp.ar_op = htons(ARPOP_REQUEST);

	memcpy(pkt.sender_mac, ifr.ifr_hwaddr.sa_data, sizeof(pkt.sender_mac));
	pkt.sender_ip = htonl(own_ip);
	memset(pkt.arp_mac, 0 , sizeof(pkt.arp_mac));
	pkt.arp_ip = arp_ip.s_addr;   //request ip for arp
	memset(pkt.padding, 0 , sizeof(pkt.padding));

	sa.sll_family   = AF_PACKET;
	sa.sll_protocol = htons(ETH_P_ARP);
	sa.sll_ifindex  = if_index;
	sa.sll_hatype   = ARPHRD_ETHER;
	sa.sll_pkttype  = PACKET_BROADCAST;
	sa.sll_halen	  = 0;
	ret = sendto(fd, &pkt, sizeof(pkt), 0,(struct sockaddr *)&sa, sizeof(sa));
	if( ret < 0 ) {
		dbg("arp sending failed for ip %s \n", inet_ntoa(arp_ip));
		return -1;
	}
	dbg("arp send for ip %s suceeded\n", inet_ntoa(arp_ip));

	close(fd);
	return 0;

}

//---------------------------------------------------------------------------------------------------------------------
void pmip_timer_bce_expired_handlerv4(struct tq_elem *tqe)

{
	dbg("pmip_timer_bce_expired_handlerv4 Enter\n");
	int mutex_return_code;
	int ret;

		dbg("pmip_timer_bce_expired_handlerv4 v4 lock\n");
		mutex_return_code = pthread_rwlock_wrlock(&pmip_v4_lock);

	if (mutex_return_code != 0) {
		dbg("pthread_rwlock_wrlock(&pmip_lock) %s\n", strerror(mutex_return_code));
	}
	dbg("-------------------------------------\n");
	if (!task_interrupted()) {
		pmip_entry_t *e = tq_data(tqe, pmip_entry_t, tqe);
		mutex_return_code = pthread_rwlock_wrlock(&e->lock);
		if (mutex_return_code != 0) {
			dbg("pthread_rwlock_wrlock(&e->lock) %s\n", strerror(mutex_return_code));
		}
		dbg("Retransmissions counter : %d\n", e->n_rets_counter);
		if (e->n_rets_counter == 0) {
			dbg("Retransmissions counter expired\n");
			free_iov_data((struct iovec *) &e->mh_vec, e->iovlen);
			if (is_mag()) {
				//++e->seqno_out;
				dbg("Calling deregistration\n");
				mag_dereg(e, 1, 1);
					pmipv4cache_release_entry(e);
					pmipv4_bce_delete(e);

				return;
			}
			//Delete existing route for the deleted MN
			if (is_lma()) {
				lma_dereg(e, 0, 0, 1);
				pmipv4cache_release_entry(e);
				pmipv4_bce_delete(e);
				return;
			}
				mutex_return_code = pthread_rwlock_unlock(&pmip_v4_lock);

			if (mutex_return_code != 0) {
				dbg("pthread_rwlock_unlock(&pmip_lock) %s\n", strerror(mutex_return_code));
			}
			return;
		}
		if (is_mag()) {
				dbg("Send ARP for Neighbour Reachability for:%x:%x:%x:%x:%x:%x:%x:%x ipv4 address %s\n", NIP6ADDR(&e->mn_hw_address), inet_ntoa(e->mnv4_addr));
				//Create ARP for Reachability test!
				ret = send_arp_from_bridge(e->mnv4_addr);
				if (ret < 0)
				{
					dbg("Arp Sent failed. Do not set flag\n");
				}
				else
				{
					dbg("Arp Sent Ok. set flag\n");
					e->Is_arp_sent =1 ;
				}

			struct timespec expires;
			clock_gettime(CLOCK_REALTIME, &e->add_time);
			tsadd(e->add_time, conf.RetransmissionTimeOut, expires);
			// Add a new task for deletion of entry if No Na is received.
			add_task_abs(&expires, &e->tqe, pmip_timer_bce_expired_handlerv4);
			dbg("Start the Timer for Retransmission/Deletion ....\n");
			//Decrements the Retransmissions counter.
			e->n_rets_counter--;
			mutex_return_code = pthread_rwlock_unlock(&e->lock);
			if (mutex_return_code != 0) {
				dbg("pthread_rwlock_unlock(&e->lock) %s\n", strerror(mutex_return_code));
			}
		}
		if (is_lma()) {
			lma_dereg(e, 0, 0, 1);
			pmipv4cache_release_entry(e);
			pmipv4_bce_delete(e);
			return;
		}
			}

		dbg("pmip_timer_bce_expired_handlerv4 v4 unlock\n");
		mutex_return_code = pthread_rwlock_unlock(&pmip_v4_lock);

	if (mutex_return_code != 0) {
		dbg("pthread_rwlock_unlock(&pmip_lock) %s\n", strerror(mutex_return_code));
	}
}


//---------------------------------------------------------------------------------------------------------------------
void pmip_timer_bce_expired_handler(struct tq_elem *tqe)
{
	dbg("pmip_timer_bce_expired_handler Enter\n");
	int mutex_return_code;
	int ret;
	dbg("pmip_timer_bce_expired_handler v6 lock\n");
	mutex_return_code = pthread_rwlock_wrlock(&pmip_lock);

	if (mutex_return_code != 0) {
		dbg("pthread_rwlock_wrlock(&pmip_lock) %s\n", strerror(mutex_return_code));
	}
	dbg("-------------------------------------\n");
	if (!task_interrupted()) {
		pmip_entry_t *e = tq_data(tqe, pmip_entry_t, tqe);
		mutex_return_code = pthread_rwlock_wrlock(&e->lock);
		if (mutex_return_code != 0) {
			dbg("pthread_rwlock_wrlock(&e->lock) %s\n", strerror(mutex_return_code));
		}
		dbg("Retransmissions counter : %d\n", e->n_rets_counter);
		if (e->n_rets_counter == 0) {
			dbg("Retransmissions counter expired\n");
			free_iov_data((struct iovec *) &e->mh_vec, e->iovlen);
			if (is_mag()) {
				//++e->seqno_out;
				dbg("Calling deregistration\n");
				mag_dereg(e, 1, 0);
				pmipcache_release_entry(e);
				pmip_bce_delete(e);
				return;
			}
			//Delete existing route for the deleted MN
			if (is_lma()) {
				lma_dereg(e, 0, 0, 0);
				pmipcache_release_entry(e);
				pmip_bce_delete(e);
				return;
			}
			mutex_return_code = pthread_rwlock_unlock(&pmip_lock);

			if (mutex_return_code != 0) {
				dbg("pthread_rwlock_unlock(&pmip_lock) %s\n", strerror(mutex_return_code));
			}
			return;
		}
		if (is_mag()) {
			dbg("Send NS for Neighbour Reachability for:%x:%x:%x:%x:%x:%x:%x:%x iif=%d\n", NIP6ADDR(&e->mn_hw_address), e->link);

			dbg("mcast:%x:%x:%x:%x:%x:%x:%x:%x mn addr=%x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(solicited_mcast(&e->mn_suffix)),NIP6ADDR(get_mn_addr(e)));
			//Create NS for Reachability test!
			ndisc_send_ns(e->link, &conf.MagAddressIngress, solicited_mcast(&e->mn_suffix), get_mn_addr(e));

			struct timespec expires;
			clock_gettime(CLOCK_REALTIME, &e->add_time);
			tsadd(e->add_time, conf.RetransmissionTimeOut, expires);
			// Add a new task for deletion of entry if No Na is received.
			add_task_abs(&expires, &e->tqe, pmip_timer_bce_expired_handler);
			dbg("Start the Timer for Retransmission/Deletion ....\n");
			//Decrements the Retransmissions counter.
			e->n_rets_counter--;
			mutex_return_code = pthread_rwlock_unlock(&e->lock);
			if (mutex_return_code != 0) {
				dbg("pthread_rwlock_unlock(&e->lock) %s\n", strerror(mutex_return_code));
			}
		}
		if (is_lma()) {
			lma_dereg(e, 0, 0, 0);
			pmipcache_release_entry(e);
			pmip_bce_delete(e);
			return;
		}
	}
		dbg("pmip_timer_bce_expired_handler v6 unlock\n");
		mutex_return_code = pthread_rwlock_unlock(&pmip_lock);
	if (mutex_return_code != 0) {
		dbg("pthread_rwlock_unlock(&pmip_lock) %s\n", strerror(mutex_return_code));
	}
}


/**
 * Handlers defined for MH and ICMP messages.
 **/

/*!
 * check if address is solicited multicast
 * \param addr
 * \return value <> 0 if true
 */
static inline int ipv6_addr_is_solicited_mcast(const struct in6_addr *addr)
{
	return (addr->s6_addr32[0] == htonl(0xff020000)
			&& addr->s6_addr32[1] == htonl(0x00000000)
			&& addr->s6_addr32[2] == htonl(0x00000001)
			&& addr->s6_addr[12] == 0xff);
}

/*!
 * check if address is multicast
 * \param addr
 * \return value <> 0 if true
 */
static inline int ipv6_addr_is_multicast(const struct in6_addr *addr)
{
	return (addr->s6_addr32[0] & htonl(0xFF000000)) == htonl(0xFF000000);
}

/*!
 * check if address is linklocal
 * \param addr
 * \return value <> 0 if true
 */
static inline int ipv6_addr_is_linklocal(const struct in6_addr *addr)
{
	return IN6_IS_ADDR_LINKLOCAL(addr);
}


/*!
 * handler called when receiving a router solicitation
 */
//hip

static void pmip_mag_recv_rs
(
	const struct icmp6_hdr *ih,
	ssize_t len,
	const struct in6_addr *saddr,
	const struct in6_addr *daddr,
	int iif,
	int hoplimit,
	unsigned char source_mac_addr [PMIP_MAC_ADDR_LEN]
)
{
	dbg("\n");
	dbg("Router Solicitation received \n");
	printf("-------------------------------------\n");
	dbg("Router Solicitation (RS) Received iif %d\n", iif);
	dbg("Received RS Src Addr: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(saddr));
	dbg("Received RS Dst addr: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(daddr));
	msg_info_t rs_info;
	bzero(&rs_info, sizeof(rs_info));
	icmp_rs_parse(&rs_info, (struct nd_router_solicit *) ih, saddr, daddr, iif, hoplimit);
	mac_to_ipv6(source_mac_addr,&rs_info.mn_hw_address);
	mag_fsm(&rs_info);
}

/*!
 * handler called when receiving a proxy binding acknowledgment over v6 tunnel
 */
static void pmip_mag_recv_pba(const struct ip6_mh *mh, ssize_t len, const struct in6_addr_bundle *in_addrs, int iif)
{
    dbg("=====================================\n");
	dbg("Proxy Binding Acknowledgement (PBA) Received\n");
	dbg("Received PBA Src Addr: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(in_addrs->src));
	dbg("Received PBA Dst addr: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(in_addrs->dst));
	//define the values for calling the parsing function
	//call the parsing function
	struct ip6_mh_binding_ack *pba;
	//call the fsm function.
	msg_info_t info;
	bzero(&info, sizeof(info));
	pba = (struct ip6_mh_binding_ack *) ((void *) mh);
	mh_pba_parse(&info, pba, len, in_addrs, iif);

	if (info.pmipv4_hnp_reply_valid == 1)
	{
	//calling for ipv4 handling
	if (conf.pmip_mode_type ==  IP_FAMILY_V4_V01 || conf.pmip_mode_type ==  IP_FAMILY_V4V6_V01)
		{
			dbg("Received PBA with ipv4 HNP reply\n");
    mag_fsm_v4(&info);
		}
	}
	else
	{
		if (conf.pmip_mode_type ==  IP_FAMILY_V6_V01 || conf.pmip_mode_type ==  IP_FAMILY_V4V6_V01)
		{
			dbg("Received PBA with ipv6 Home Network prefix\n");
			mag_fsm(&info);
		}
	}
}

/*!
 * handler called when receiving a proxy binding acknowledgment over v4 tunnel
 */
static void pmipv4_mag_recv_pba(const struct ip6_mh *mh, ssize_t len, const struct in_addr_bundle *in_addrs, int iif)
{
	printf("=====================================\n");
	dbg("Proxy Binding Acknowledgement (PBA) Received\n");
	dbg("Received PBA Src Addr: %s\n", inet_ntoa(*in_addrs->src));
	dbg("Received PBA Dst addr: %s\n", inet_ntoa(*in_addrs->dst));
	//define the values for calling the parsing function
	//call the parsing function
	struct ip6_mh_binding_ack *pba;
	//call the fsm function.
	msg_info_t info;
	bzero(&info, sizeof(info));
	pba = (struct ip6_mh_binding_ack *) ((void *) mh);
	mh_pba_parse(&info, pba, len, in_addrs, iif);
	if (info.pmipv4_hnp_reply_valid == 1)
	{
	//calling for ipv4 handling
	if (conf.pmip_mode_type ==  IP_FAMILY_V4_V01 || conf.pmip_mode_type ==  IP_FAMILY_V4V6_V01)
		{
			dbg("Received PBA with ipv4 HNP reply\n");
    mag_fsm_v4(&info);
		}
	}
	else
	{
		if (conf.pmip_mode_type ==  IP_FAMILY_V6_V01 || conf.pmip_mode_type ==  IP_FAMILY_V4V6_V01)
		{
			dbg("Received PBA with ipv6 Home Network prefix\n");
			mag_fsm(&info);
		}
	}

}



/*!
 * handler called when receiving a proxy binding update over v6 tunnel
 */
static void pmip_lma_recv_pbu(const struct ip6_mh *mh, ssize_t len, const struct in6_addr_bundle *in_addrs, int iif)
{
	printf("=====================================\n");
	dbg("Proxy Binding Update (PBU) Received\n");
	dbg("Received PBU Src Addr: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(in_addrs->src));
	dbg("Received PBU Dst addr: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(in_addrs->dst));
	//define the values for the parsing function
	//call the parsing function
	struct ip6_mh_binding_update *pbu = (struct ip6_mh_binding_update *) mh;
	//call the fsm function.
	msg_info_t info;
	bzero(&info, sizeof(info));
	mh_pbu_parse(&info, pbu, len, in_addrs, iif);
	if (info.pmipv4_hnp_req_valid == 1)
	{
		lma_fsm_v4(&info);
	}
	else
	{
	lma_fsm(&info);
	}
}

/*!
 * handler called when receiving a proxy binding update over v4 tunnel
 */
static void pmipv4_lma_recv_pbu(const struct ip6_mh *mh, ssize_t len, const struct in_addr_bundle *in_addrs, int iif)
{
	printf("=====================================\n");
	dbg("Proxy Binding Update (PBU) Received\n");
	dbg("Received PBU Src Addr: %s\n", inet_ntoa(*in_addrs->src));
	dbg("Received PBU Dst addr: %s\n", inet_ntoa(*in_addrs->dst));
	//define the values for the parsing function
	//call the parsing function
	struct ip6_mh_binding_update *pbu = (struct ip6_mh_binding_update *) mh;
	//call the fsm function.
	msg_info_t info;
	bzero(&info, sizeof(info));
	mh_pbu_parse(&info, pbu, len, in_addrs, iif);
	if (info.pmipv4_hnp_req_valid == 1)
	{
		lma_fsm_v4(&info);
	}
	else
	{
	lma_fsm(&info);
	}
}


/*!
 * handler called when MAG receive a neighbor advertisement
 */
static void pmip_mag_recv_na
(
	const struct icmp6_hdr *ih,
	ssize_t len,
	const struct in6_addr *saddr,
	const struct in6_addr *daddr,
	int iif,
	int hoplimit,
	unsigned char source_mac_addr [PMIP_MAC_ADDR_LEN]
)
{
	// define the MN identifier
	//struct in6_addr id = in6addr_any;
	struct nd_neighbor_advert *msg = (struct nd_neighbor_advert *) ih;
	//Check target is not link local address.
	if (ipv6_addr_is_linklocal(&msg->nd_na_target)) {
		return;
	}
	//Check target is not multicast.
	if (ipv6_addr_is_multicast(&msg->nd_na_target)) {
		return;
	}
	if (len - sizeof(struct nd_neighbor_advert) > 0) {
		printf("-------------------------------------\n");
		dbg("Neighbor Advertisement (NA) Received\n");
		dbg("Received NA Src Addr: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(saddr));
		dbg("Received NA Dst addr: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(daddr));
		msg_info_t na_info;
		bzero(&na_info, sizeof(na_info));
		icmp_na_parse(&na_info, (struct nd_neighbor_advert *) ih, saddr, daddr, iif, hoplimit);
		mag_fsm(&na_info);
	}
	return;
}


struct icmp6_handler pmip_mag_rs_handler = {
	.recv = pmip_mag_recv_rs
};

struct mh_handler pmip_mag_pba_handler = {
	.recv = pmip_mag_recv_pba
};
struct mh_handler pmip_lma_pbu_handler = {
	.recv = pmip_lma_recv_pbu
};
struct icmp6_handler pmip_mag_recv_na_handler = {
	.recv = pmip_mag_recv_na
};

struct mhv4_handler pmipv4_mag_pba_handler = {
	.recv = pmipv4_mag_recv_pba
};
struct mhv4_handler pmipv4_lma_pbu_handler = {
	.recv = pmipv4_lma_recv_pbu
};


