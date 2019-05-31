/*! \file pmip6d.c
 * \brief The main PMIP6D file
 * \author OpenAir3 Group
 * \date 12th of October 2010
 * \version 1.0
 * \company Eurecom
 * \project OpenAirInterface
 * \email: openair3@eurecom.fr
 */
#define PMIP
#define PMIP_INIT_C
//---------------------------------------------------------------------------------------------------------------------
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <unistd.h>
//---------------------------------------------------------------------------------------------------------------------
#include "pmip_cache.h"
#include "pmip_fsm.h"
#include "pmip_handler.h"
#include "pmip_hnp_cache.h"
#include "conf.h"
#include "pmip_init.h"
#include "pmip_lma_proc.h"
#include "pmip_mag_proc.h"
#include "pmip_msgs.h"
#include "pmip_pcap.h"
#include "pmip_tunnel.h"
#include "pmip_types.h"
//---------------------------------------------------------------------------------------------------------------------
#include "rtnl.h"
#include "tunnelctl.h"
#ifdef ENABLE_VT
#    include "vt.h"
#endif
#include "debug.h"


#define IPV6_ALL_SOLICITED_MCAST_ADDR 68
//---------------------------------------------------------------------------------------------------------------------
extern struct sock icmp6_sock;
//---------------------------------------------------------------------------------------------------------------------
void init_mag_icmp_sock(void)
	//---------------------------------------------------------------------------------------------------------------------
{
	if (0) {
		int on = 1;
		dbg("Set SOLRAW, IPV6_ALL_SOLICTED_MCAST_ADDR = %d\n", IPV6_ALL_SOLICITED_MCAST_ADDR);
		if (setsockopt(icmp6_sock.fd, SOL_RAW, IPV6_ALL_SOLICITED_MCAST_ADDR, &on, sizeof(on)) < 0) {
			perror("allow all solicited mcast address\n");
		}
	}
}
//---------------------------------------------------------------------------------------------------------------------
static int pmipv4_cache_delete_each(void *data, void *arg)
	//---------------------------------------------------------------------------------------------------------------------
{
	pmip_entry_t *bce = (pmip_entry_t *) data;
	int usercount = 0;
	if (is_mag()) {
		mag_remove_v4_route(bce->mnv4_addr,"pmipv4");
		//Delete existing route & rule for the deleted MN
		if (conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)
		{
			usercount = tunnel_getusers(bce->tunnel);
			//decrement users of old tunnel.
			pmip_tunnel_del(bce->tunnel);
		}
		else if (conf.pmip_tunnel_mode == IP_FAMILY_V4_V01)
		{
			//decrement users of old tunnel.
			pmipv4_tunnel_del(bce->tunnel_v4);
		}
	}
	//Delete existing route for the deleted MN
	if (is_lma()) {
		lma_remove_route(&bce->mn_addr, bce->tunnel);
		//decrement users of old tunnel.
		if (conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)
		{
			pmip_tunnel_del(bce->tunnel);
		}
		else if (conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)
		{
			pmipv4_tunnel_del(bce->tunnel_v4);
		}
	}
	//Delete the Entry.
	free_iov_data((struct iovec *) &bce->mh_vec, bce->iovlen);
	pmipv4_bce_delete(bce);
	return 0;
}


//---------------------------------------------------------------------------------------------------------------------
static int pmip_cache_delete_each(void *data, void *arg)
	//---------------------------------------------------------------------------------------------------------------------
{
	pmip_entry_t *bce = (pmip_entry_t *) data;
	int usercount = 0;
	int force_remove = 1;
	if (is_mag()) {
		//send depricate RA
		dbg ("Sending the deprecate RA\n");
		mag_kickoff_ra(bce, 1);
		//Delete existing route & rule for the deleted MN for v6 tunnel
		if (conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)
		{
			mag_remove_route(&bce->mn_addr, bce->link, force_remove);
			usercount = tunnel_getusers(bce->tunnel);
			if (usercount == 1) {
				route_del(bce->tunnel, RT6_TABLE_PMIP, IP6_RT_PRIO_MIP6_FWD, &in6addr_any, 0, &in6addr_any, 0, NULL);
			}
			//decrement users of old tunnel.
			pmip_tunnel_del(bce->tunnel);
		}
		//Delete existing route & rule for the deleted MN for v4 tunnel
		else if (conf.pmip_tunnel_mode == IP_FAMILY_V4_V01)
		{
			mag_remove_v6_route(bce->mn_addr, "pmipv4", force_remove);

			//decrement users of old tunnel.
			pmipv4_tunnel_del(bce->tunnel_v4);
		}
	}
	//Delete existing route for the deleted MN
	if (is_lma()) {
		lma_remove_route(&bce->mn_addr, bce->tunnel);
		//decrement users of old tunnel.
		if (conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)
		{
			pmip_tunnel_del(bce->tunnel);
		}
		else if (conf.pmip_tunnel_mode == IP_FAMILY_V4_V01)
		{
			pmipv4_tunnel_del(bce->tunnel_v4);
		}
	}
	//Delete the Entry.
	free_iov_data((struct iovec *) &bce->mh_vec, bce->iovlen);
	pmip_bce_delete(bce);
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
void pmip_cleanup(void)
	//---------------------------------------------------------------------------------------------------------------------
{
	char command[MAX_DHCP_COMMAND_STR_LEN] = {0};
	dbg("pmip cleanup...\n");
	if ((conf.pmip_mode_type == IP_FAMILY_V6_V01) || (conf.pmip_mode_type == IP_FAMILY_V4V6_V01))
	{
		//Release the pmip cache ==> deletes the routes and rules and "default route on PMIP" and tunnels created.
		dbg("Release all occupied resources...\n");
		//delete the default rule.
		dbg("Remove default rule...\n");
		rule_del(NULL, RT6_TABLE_MIP6, IP6_RULE_PRIO_MIP6_FWD, RTN_UNICAST, &in6addr_any, 0, &in6addr_any, 0, 0);

		dbg("Release pmip_cache...\n");
		pmip_cache_iterate(pmip_cache_delete_each, NULL);
	}

	if ((conf.pmip_mode_type == IP_FAMILY_V4_V01) || (conf.pmip_mode_type == IP_FAMILY_V4V6_V01))
	{
		//Release the pmip cache ==> deletes the routes and rules and "default route on PMIP" and tunnels created.
		dbg("Release all occupied v4 resources...\n");
		//delete the default rule.
		dbg("Remove default rule...\n");
		rule_del(NULL, RT6_TABLE_MIP6, IP6_RULE_PRIO_MIP6_FWD, RTN_UNICAST, &in6addr_any, 0, &in6addr_any, 0, 0);

		dbg("Release pmipv4 cache...\n");
		pmipv4_cache_iterate(pmipv4_cache_delete_each, NULL);

		//clear DHCP reservation file
		snprintf (command,MAX_DHCP_COMMAND_STR_LEN,"echo > %s",PMIP_DHCP_RESER_FILE);
		pmip_system_call(command,strlen(command),1);
		system("echo 'pmip: Cleared the Pmip reservation file on terminate signal ' > /dev/kmsg");
	}

	//Remove the pmipv4 table from iproute2
	dbg("Remove the pmipv4 table from iproute2\n");
	snprintf (command,MAX_DHCP_COMMAND_STR_LEN,"sed -i /'201 pmipv4'/d %s", PMIP_ROUTE_TABLE);
	pmip_system_call(command,strlen(command),1);

	system("echo 'pmip: termination completed' > /dev/kmsg");

	//#undef HAVE_PCAP_BREAKLOOP
#define HAVE_PCAP_BREAKLOOP
#ifdef HAVE_PCAP_BREAKLOOP
	/*
	 * We have "pcap_breakloop()"; use it, so that we do as little
	 * as possible in the signal handler (it's probably not safe
	 * to do anything with standard I/O streams in a signal handler -
	 * the ANSI C standard doesn't say it is).
	 */
	if (is_mag()) {
		pcap_breakloop(pcap_descr);
	}
#endif
}

//---------------------------------------------------------------------------------------------------------------------
int pmip_common_init(void)
	//---------------------------------------------------------------------------------------------------------------------
{
	/**
	 * Probe for the local address
	 **/
	int probe_fd = socket(AF_INET6, SOCK_DGRAM, 0);
	if (probe_fd < 0) {
		perror("socket");
		exit(2);
	}
	unsigned int alen;
	struct sockaddr_in6 host;
	struct sockaddr_in6 firsthop;

	memset(&firsthop, 0, sizeof(firsthop));
	firsthop.sin6_port = htons(1025);
	firsthop.sin6_family = AF_INET6;
	if (connect(probe_fd, (struct sockaddr *) &firsthop, sizeof(firsthop)) == -1) {
		perror("connect");
		return -1;;
	}
	alen = sizeof(host);
	if (getsockname(probe_fd, (struct sockaddr *) &host, &alen) == -1) {
		perror("probe getsockname");
		return -1;;
	}
	close(probe_fd);

	if ((conf.pmip_mode_type == IP_FAMILY_V6_V01 || conf.pmip_mode_type == IP_FAMILY_V4V6_V01)) {
		/**
		 * Initializes PMIP cache.
		 **/
		if (pmip_cache_init() < 0) {
			dbg("PMIP Binding Cache initialization failed! \n");
			return -1;
		} else {
			dbg("PMIP Binding Cache is initialized!\n");
		}
	}
	if((conf.pmip_mode_type == IP_FAMILY_V4_V01 || conf.pmip_mode_type == IP_FAMILY_V4V6_V01)){
		/**
		 * Initializes PMIPv4 cache.
		 **/
		if (pmipv4_cache_init() < 0) {
			dbg("PMIPV4 Binding Cache initialization failed! \n");
			return -1;
		} else {
			dbg("PMIPV4 Binding Cache is initialized!\n");
		}
	}
	/**
	 * Adds a default rule for RT6_TABLE_MIP6.
	 */
	dbg("Add default rule for RT6_TABLE_MIP6\n");
	if (rule_add(NULL, RT6_TABLE_MIP6, IP6_RULE_PRIO_MIP6_FWD, RTN_UNICAST, &in6addr_any, 0, &in6addr_any, 0, 0) < 0) {
		dbg("Add default rule for RT6_TABLE_MIP6 failed, insufficient privilege/kernel options missing!\n");
		return -1;
	}
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int pmip_mag_init(void)
	//---------------------------------------------------------------------------------------------------------------------
{
	pmip_common_init();
	conf.OurAddress = conf.MagAddressEgress;
	conf.HomeNetworkPrefix = get_node_prefix(&conf.MagAddressIngress); //copy Home network prefix.
	dbg("Running as MAG entity\n");
	dbg("Entity Egress Address: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&conf.OurAddress));
	dbg("Entity Ingress Address: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&conf.MagAddressIngress));
	dbg("Home Network Prefix Address: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&conf.HomeNetworkPrefix));
	if (mag_init_fsm() < 0) {
		dbg("Initialization of FSM failed...exit\n");
		exit(-1);
	}
	if (pending_pbu_seq_list_init() < 0) {
		dbg("PBU seg list init failed...exit\n");
		exit(-1);
	}

	init_pbu_sequence_number();
	if ((conf.pmip_mode_type == IP_FAMILY_V6_V01) ||(conf.pmip_mode_type == IP_FAMILY_V4V6_V01))
	{
		dbg("MAG init request for ipv6 do registration \n");
		init_iface_ra();
		init_mag_icmp_sock();
		dbg("Initializing the NA handler\n");
		// to capture NA message
		icmp6_handler_reg(ND_NEIGHBOR_ADVERT, &pmip_mag_recv_na_handler);
		dbg("Initializing the RS handler\n");
		// to capture RS message
		icmp6_handler_reg(ND_ROUTER_SOLICIT, &pmip_mag_rs_handler);
	}
	else
	{
		dbg("MAG init request is  for pmipv4 mode do not do register icmp handler\n");
	}

	if ((conf.pmip_mode_type == IP_FAMILY_V6_V01 && conf.pmip_tunnel_mode == IP_FAMILY_V6_V01) ||
			((conf.pmip_mode_type == IP_FAMILY_V4_V01 && conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)) ||
			((conf.pmip_mode_type == IP_FAMILY_V4V6_V01 && conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)))
	{
		dbg("Initializing the PBA handler for v6 tunnel\n");
		//To capture PBA message.
		mh_handler_reg(IP6_MH_TYPE_BACK, &pmip_mag_pba_handler);
	}
	if  ((conf.pmip_mode_type == IP_FAMILY_V6_V01 && conf.pmip_tunnel_mode == IP_FAMILY_V4_V01) ||
			(conf.pmip_mode_type == IP_FAMILY_V4_V01 && conf.pmip_tunnel_mode == IP_FAMILY_V4_V01)||
			((conf.pmip_mode_type == IP_FAMILY_V4V6_V01 && conf.pmip_tunnel_mode == IP_FAMILY_V4_V01)))
	{
		dbg("Initializing the PBA handler for v4 tunnel\n");
		//To capture PBA message.
		mhv4_handler_reg(IP6_MH_TYPE_BACK, &pmipv4_mag_pba_handler);
	}

	/**
	 * Deletes the default route for MN prefix so routing is per unicast MN address!
	 **/
	//route_del((int) NULL, RT6_TABLE_MAIN, IP6_RT_PRIO_ADDRCONF, &in6addr_any, 0, &conf.HomeNetworkPrefix, 64, NULL);
	dbg("Initializing the HNP cache\n");
	if (pmip_mn_to_hnp_cache_init() < 0) {
		exit (-1);
	}

	char devname[32];
	int iif;
	dbg("Getting ingress informations\n");
	mag_get_ingress_info(&iif, devname);
	dbg("Got Device name =%s & iif =%d\n",devname,iif);
	dbg("Starting capturing AP messages for incoming MNs detection\n");
	pmip_pcap_loop(devname, iif);
	return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int pmip_lma_init(void)
	//---------------------------------------------------------------------------------------------------------------------
{
	pmip_common_init();
	pmip_lma_mn_to_hnp_cache_init();
	conf.OurAddress = conf.LmaAddress;
	dbg("Entity Address: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&conf.OurAddress));

	//if v6 tunnel
	if (conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)
	{
		dbg("Initializing the PBU handler over v6 tunnel\n");
		//To capture PBU message.
		mh_handler_reg(IP6_MH_TYPE_BU, &pmip_lma_pbu_handler);
	}

	//if v4 tunnel
	if  (conf.pmip_tunnel_mode == IP_FAMILY_V4_V01)
	{
		dbg("Initializing the PBU handler over v4 tunnel\n");
		//To capture PBU message.
		mhv4_handler_reg(IP6_MH_TYPE_BU, &pmipv4_lma_pbu_handler);
	}
	return 0;
}
