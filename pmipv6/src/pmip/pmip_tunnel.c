/*! \file pmip_tunnel.c
* \brief
* \author OpenAir3 Group
* \date 12th of October 2010
* \version 1.0
* \company Eurecom
* \project OpenAirInterface
* \email: openair3@eurecom.fr
*/
#define PMIP
#define PMIP_TUNNEL_C
#ifdef HAVE_CONFIG_H
#include <config.h>
#include <arpa/inet.h>

#endif
//---------------------------------------------------------------------------------------------------------------------
#include "pmip_tunnel.h"
//---------------------------------------------------------------------------------------------------------------------
#include "tunnelctl.h"
#include "util.h"
#ifdef ENABLE_VT
#    include "vt.h"
#endif
#include "debug.h"
#include "conf.h"
//---------------------------------------------------------------------------------------------------------------------
int pmip_tunnel_add(struct in6_addr *local, struct in6_addr *remote, int link, uint8_t gre_key_valid, uint32_t gre_key)
{
	gre_info_type gre_info;
	gre_info.gre_key_valid = gre_key_valid;
	gre_info.gre_key = gre_key;

	if (conf.TunnelingEnabled) {
		dbg("Creating IP-in-IP tunnel link %d from %x:%x:%x:%x:%x:%x:%x:%x to %x:%x:%x:%x:%x:%x:%x:%x...\n", link, NIP6ADDR(local), NIP6ADDR(remote));
		dbg("gre key valid:%d, key:%d\n",gre_key_valid, gre_key );
		int tunnel = tunnel_add(local, remote, link, 0, (gre_key_valid?(void*)&gre_info:NULL)); // -1 if error
		return tunnel;
	} else {
		dbg("IP-in-IP tunneling is disabled, no tunnel is created\n");
		return 0;
	}
}
//---------------------------------------------------------------------------------------------------------------------
int pmip_tunnel_del(int ifindex)
{
	int res = 0;
	if (conf.TunnelingEnabled) {
		dbg("Decrease reference number of tunnel %d\n", ifindex);
		if (ifindex > 0) {
			int usercount = tunnel_getusers(ifindex);
			dbg("current v6 tunnel # of binding entries %d \n", usercount);
			if (usercount > 1 || conf.DynamicTunnelingEnabled) {
				res = tunnel_del(ifindex, 0, 0);
			} else if (usercount == 1) {
				dbg("Completely delete the tunnel.... \n");
				res = tunnel_del(ifindex, 0, 0);
				//TODO: Put the tunnel  in to the pool
				//TODO: Set timer to delete the tunnel after a long stalled period
			}
		} else {
			res = -1;
		}
	} else {
		dbg("IP-in-IP tunneling is disabled, no tunnel is deleted\n");
	}
	return res;
}

//---------------------------------------------------------------------------------------------------------------------
int pmipv4_tunnel_add(struct in_addr *local, struct in_addr *remote, int link, uint8_t gre_key_valid, uint32_t gre_key)
{
	gre_info_type gre_info;
	gre_info.gre_key_valid = gre_key_valid;
	gre_info.gre_key = gre_key;

	if (conf.TunnelingEnabled) {
		dbg("Creating IP-in-GRE tunnel link %d from %s \n", link, inet_ntoa(*local));
		dbg("                                  to %s\n", inet_ntoa(*remote));
		dbg("gre key valid:%d, key:%d\n",gre_key_valid, gre_key );
		int tunnel = tunnel_addv4(local, remote, link, 0, (gre_key_valid?(void*)&gre_info:NULL)); // -1 if error
		return tunnel;
	} else {
		dbg("IP-in-IP tunneling is disabled, no tunnel is created\n");
		return 0;
	}
}

//---------------------------------------------------------------------------------------------------------------------
int pmipv4_tunnel_del(int ifindex)
{
	int res = 0;
	if (conf.TunnelingEnabled) {
		dbg("Decrease reference number of tunnel %d\n", ifindex);
		if (ifindex > 0) {
			int usercount = tunnelv4_getusers(ifindex);
			dbg("current v4 tunnel # of binding entries %d \n", usercount);
			if (usercount > 1 || conf.DynamicTunnelingEnabled) {
				res = tunnelv4_del(ifindex, 0, 0);
			} else if (usercount == 1) {
				dbg("Completely delete the tunnel.... \n");
				res = tunnelv4_del(ifindex, 0, 0);
				//TODO: Put the tunnel  in to the pool
				//TODO: Set timer to delete the tunnel after a long stalled period
			}
		} else {
			res = -1;
		}
	} else {
		dbg("IP-in-IP tunneling is disabled, no tunnel is deleted\n");
	}
	return res;
}

