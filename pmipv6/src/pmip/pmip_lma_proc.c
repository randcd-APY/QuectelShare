/*! \file pmip_lma_proc.c
 * \brief
 * \author OpenAir3 Group
 * \date 12th of October 2010
 * \version 1.0
 * \company Eurecom
 * \project OpenAirInterface
 * \email: openair3@eurecom.fr
 */
#define PMIP
#define PMIP_LMA_PROC_C
#ifdef HAVE_CONFIG_H
#    include <config.h>
#endif
//---------------------------------------------------------------------------------------------------------------------
#include "pmip_handler.h"
#include "pmip_hnp_cache.h"
#include "pmip_lma_proc.h"
#include "pmip_tunnel.h"
//---------------------------------------------------------------------------------------------------------------------
#include "rtnl.h"
#ifdef ENABLE_VT
#    include "vt.h"
#endif
#include "debug.h"
#include "conf.h"
//---------------------------------------------------------------------------------------------------------------------
int lma_setup_route(struct in6_addr *pmip6_addr, int tunnel)
{
	int res = 0;
	if (conf.TunnelingEnabled) {
		dbg("Forward: Add new route for %x:%x:%x:%x:%x:%x:%x:%x in table %d\n", NIP6ADDR(pmip6_addr), RT6_TABLE_MIP6);
		res = route_add(tunnel, RT6_TABLE_MIP6, RTPROT_MIP, 0, IP6_RT_PRIO_MIP6_FWD, &in6addr_any, 0, pmip6_addr, 64, NULL);
	}
	return res;
}
//---------------------------------------------------------------------------------------------------------------------
int lma_remove_route(struct in6_addr *pmip6_addr, int tunnel)
{
	int res = 0;
	if (conf.TunnelingEnabled) {
		//Delete existing rule for the deleted MN
		dbg("Delete old route for: %x:%x:%x:%x:%x:%x:%x:%x from table %d\n", NIP6ADDR(pmip6_addr), RT6_TABLE_MIP6);
		res = route_del(tunnel, RT6_TABLE_MIP6, IP6_RT_PRIO_MIP6_FWD, &in6addr_any, 0, pmip6_addr, 64, NULL);
	}
	return res;
}
void enable_forwading(char *fname)
{
	char command[200];
	//enable forwading
	snprintf (command,200,"echo 2 > /proc/sys/net/ipv6/conf/%s/forwarding ",fname);
	system (command);
	snprintf (command,200,"echo 1 > /proc/sys/net/ipv4/conf/%s/forwarding ",fname);
	system (command);

}
//---------------------------------------------------------------------------------------------------------------------
int lma_reg(pmip_entry_t * bce, int is_v4_lma_mode)
{
	char command[200];
	struct in6_addr *v6_ip;
	int ret;
	char addr_buf[INET6_ADDRSTRLEN];
	char fname[IFNAMSIZ]={0};
	if (bce != NULL) {
	if (conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)
		{
			//create a tunnel between MAG and LMA && add a route for peer address.
			bce->tunnel = pmip_tunnel_add(&conf.OurAddress, &bce->mn_serv_mag_addr, bce->link, bce->gre_key_valid, bce->gre_key);

	}
		if  (conf.pmip_tunnel_mode == IP_FAMILY_V4_V01)
		{
			//create a tunnel between MAG and LMA && add a route for peer address.
			bce->tunnel_v4 = pmipv4_tunnel_add(&conf.LmaV4Address, &bce->mn_serv_mag_v4_addr, bce->link, bce->gre_key_valid, bce->gre_key);
		}
		//ading Route for Ipv6 mn ip in v6 tunnel
		if (!is_v4_lma_mode && conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)
		{
			lma_setup_route(get_mn_addr(bce), bce->tunnel);
		}
			//ading Route for Ipv6 ip in v4 tunnel
		if (!is_v4_lma_mode && conf.pmip_tunnel_mode == IP_FAMILY_V4_V01)
		{
			ret = tunnelv4_getname(bce->tunnel_v4,fname);
			if (ret == 0)
			{
				dbg("Get Tunnel name failed for v4 ipv4 tunnel do not add route\n");
			}
			else
			{
				//enable forwading
				//disble enable forwading As it is removing the default route
				//enable_forwading(fname);
			dbg("Got current Tunnel name =%s\n",fname);
				v6_ip = get_mn_addr(bce);
				//add Route
				inet_ntop(AF_INET6, v6_ip, addr_buf, INET6_ADDRSTRLEN);
				snprintf (command,200,"ip -6 route add %s/%d dev %s ", addr_buf, PREFIX_LENGTH, fname);
				system (command);
				dbg("ip v6 Route table com=%s=\n",command);
			}
		}
		//ading Route for Ipv4 mn ip in v6 tunnel
		if ((is_v4_lma_mode == 1) && conf.pmip_tunnel_mode == IP_FAMILY_V6_V01 )
		{
			//get tunnel Iface name
			ret = tunnel_getname(bce->tunnel, fname);
			if (ret == 0)
			{
				dbg("Get Tunnel name failed for v4 ipv6 tunnel\n");
			}
			else
			{
				//enable forwading
				//disble enable forwading As it is removing the default route
				//enable_forwading(fname);
				dbg("Got current Tunnel name =%s\n",fname);
				//add Route
				snprintf (command,200,"ip route add %s dev %s ",inet_ntoa(bce->mnv4_addr), fname);
				system (command);
				dbg("Route table com=%s=\n",command);
			}

		}
		//ading Route for Ipv4 mn ip in v4 tunnel
		if ((is_v4_lma_mode == 1) && conf.pmip_tunnel_mode == IP_FAMILY_V4_V01 )
		{
			ret = tunnelv4_getname(bce->tunnel_v4, fname);
			if (ret == 0)
			{
				dbg("Get Tunnel name failed for v4 ipv4 tunnel\n");
			}
			else
			{
				//enable forwading
				//disble enable forwading As it is removing the default route
				//enable_forwading(fname);
				dbg("Got current Tunnel name =%s\n",fname);
				//add Route
				snprintf (command,200,"ip route add %s dev %s ",inet_ntoa(bce->mnv4_addr), fname);
			system (command);
				dbg("Route table com=%s=\n",command);
		}
		}

		bce->status = 0;        //PBU was Accepted!
		//Add task for entry expiry.
		if (!is_v4_lma_mode)
		{
			dbg("lma_reg pmip_cache_start\n");
		pmip_cache_start(bce);
		}
		else if (is_v4_lma_mode == 1)
		{
			dbg("lma_reg pmipv4_cache_start\n");
			pmipv4_cache_start(bce);
		}
		//Send a PBA to ack new serving MAG
		dbg("Create PBA to new Serving MAG...\n");
		struct in6_addr_bundle addrs;
		struct in_addr_bundle addrs1;
		memset(&addrs, 0, sizeof(addrs));
		memset(&addrs1, 0, sizeof(addrs1));

		if (conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)
		{
			addrs.src = &conf.OurAddress;
			addrs.dst = &bce->mn_serv_mag_addr;
			mh_send_pba(&addrs, bce, &bce->lifetime, 0, is_v4_lma_mode);
		}
		else if (conf.pmip_tunnel_mode == IP_FAMILY_V4_V01)
		{
        addrs1.src = &conf.LmaV4Address;
			addrs1.dst = &bce->mn_serv_mag_v4_addr;
			mh_send_pba(&addrs1, bce, &bce->lifetime, 0, is_v4_lma_mode);
		}
		else
		{
			dbg("invalid Tunnel mode\n");
		}
		return 0;
	} else {
		dbg("WARNING parameter pmip_entry_t * bce is NULL\n");
		return -1;
	}
}
//---------------------------------------------------------------------------------------------------------------------
int lma_reg_no_new_tunnel(pmip_entry_t * bce, int is_v4_lma_mode)
{
	if (bce != NULL) {
		bce->status = 0;        //PBU was Accepted!
		//Add task for entry expiry.
		if (!is_v4_lma_mode)
		{
			dbg("lma_reg_no_new_tunnel pmip_cache_start\n");
		pmip_cache_start(bce);
		}
		else if (is_v4_lma_mode == 1)
		{
			dbg("lma_reg_no_new_tunnel pmipv4_cache_start\n");
			pmipv4_cache_start(bce);
		}
		//Send a PBA to ack new serving MAG
		dbg("Create PBA to new Serving MAG...\n");
		struct in6_addr_bundle addrs;
		struct in_addr_bundle addrs1;
		dbg("done memset\n");
		memset(&addrs, 0, sizeof(addrs));
		memset(&addrs1, 0, sizeof(addrs1));
		if (conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)
		{
			addrs.src = &conf.OurAddress;
			addrs.dst = &bce->mn_serv_mag_addr;
			mh_send_pba(&addrs, bce, &bce->lifetime, 0, is_v4_lma_mode);
		}
		else if (conf.pmip_tunnel_mode == IP_FAMILY_V4_V01)
		{
			addrs1.src = &conf.LmaV4Address;
			addrs1.dst = &bce->mn_serv_mag_v4_addr;
			mh_send_pba(&addrs1, bce, &bce->lifetime, 0, is_v4_lma_mode);
		}
		else
		{
			dbg("Invalid tunnel mode\n");
		}
		return 0;
	} else {
		dbg("WARNING parameter pmip_entry_t * bce is NULL\n");
		return -1;
	}
}
//---------------------------------------------------------------------------------------------------------------------
int lma_dereg(pmip_entry_t * bce, msg_info_t * info, int propagate, int is_v4_lma_mode)
{
	int usercount = 0;
	char command[200];
	int ret;
	struct in6_addr *v6_ip;
	char addr_buf[INET6_ADDRSTRLEN];
	char fname[IFNAMSIZ]={0};

	if (bce != NULL) {
		//Delete the Task
		del_task(&bce->tqe);

		//v6 route delete if v6 tunnel
		if (!is_v4_lma_mode && conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)
		{
		dbg("lma_dereg v6 route delete if v6 tunnel\n");
		lma_remove_route(get_mn_addr(bce), bce->tunnel);
		}

		//v6 route delete if v4 tunnel
		if (!is_v4_lma_mode && conf.pmip_tunnel_mode == IP_FAMILY_V4_V01)
		{
			dbg("lma_dereg v6 route delete if v4 tunnel\n");
			ret = tunnelv4_getname(bce->tunnel_v4, fname);
			if (ret == 0)
			{
				dbg("Get Tunnel name failed for v4 ipv4 tunnel do not add route\n");
			}
			else
			{
				v6_ip = get_mn_addr(bce);
				//delete Route
				inet_ntop(AF_INET6, v6_ip, addr_buf, INET6_ADDRSTRLEN);
				snprintf (command,200,"ip -6 route del %s/%d dev %s ", addr_buf,PREFIX_LENGTH, fname);
				system (command);
				dbg("ip v6 Route table com=%s=\n",command);
			}
		}

		//Delete Route for Ipv4 mn ip in v6 tunnel
		if ((is_v4_lma_mode == 1) && conf.pmip_tunnel_mode == IP_FAMILY_V6_V01 )
		{
			//get tunnel Iface name
			ret = tunnel_getname(bce->tunnel,fname);
			if (ret == 0)
			{
				dbg("Get Tunnel name failed for v4 ipv6 tunnel\n");
			}
			else
			{
				//Delete  Route
				snprintf (command,200,"ip route del %s dev %s ",inet_ntoa(bce->mnv4_addr), fname);
				system (command);
				dbg("Route table com=%s=\n",command);
			}

		}
		//Delete Route for Ipv4 mn ip in v4 tunnel
		if ((is_v4_lma_mode == 1) && conf.pmip_tunnel_mode == IP_FAMILY_V4_V01 )
		{
			ret = tunnelv4_getname(bce->tunnel_v4,fname);
			if (ret == 0)
			{
				dbg("Get Tunnel name failed for v4 ipv4 tunnel\n");
			}
			else
			{
				dbg("Got current Tunnel name =%s\n",fname);
				//Delete Route
				snprintf (command,200,"ip route del %s dev %s ",inet_ntoa(bce->mnv4_addr), fname);
				system (command);
				dbg("Route table com=%s=\n",command);
			}
		}

		//if v6 Tunnel
		if (conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)
		{
			//delete old route to old tunnel.
			usercount = tunnel_getusers(bce->tunnel);
			if(usercount <= 1)
				lma_remove_route(get_mn_addr(bce), bce->tunnel);

			//decrement users of old tunnel.
			pmip_tunnel_del(bce->tunnel);
		}

		//if v4 tunnel
		if  (conf.pmip_tunnel_mode == IP_FAMILY_V4_V01)
		{
			//delete old route to old tunnel.
			usercount = tunnelv4_getusers(bce->tunnel_v4);

			//decrement users of old tunnel.
			pmipv4_tunnel_del(bce->tunnel_v4);
		}
		if (propagate) {
			dbg("Create PBA for deregistration for MAG (%x:%x:%x:%x:%x:%x:%x:%x)\n", NIP6ADDR(&bce->mn_serv_mag_addr));
			struct in6_addr_bundle addrs;
			struct in_addr_bundle addrs1;
			memset(&addrs, 0, sizeof(addrs));
			memset(&addrs1, 0, sizeof(addrs1));
			struct timespec lifetime = { 0, 0 };
			if (conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)
			{
				addrs.src = &conf.LmaAddress;
				addrs.dst = &bce->mn_serv_mag_addr;
				bce->seqno_in = info->seqno;
				mh_send_pba(&addrs, bce, &lifetime, 0, is_v4_lma_mode);
			}
			else if  (conf.pmip_tunnel_mode == IP_FAMILY_V4_V01)
			{
			addrs1.src = &conf.LmaV4Address;
				addrs1.dst = &bce->mn_serv_mag_v4_addr;
				bce->seqno_in = info->seqno;
				mh_send_pba(&addrs1, bce, &lifetime, 0, is_v4_lma_mode);
			}
			else
			{
				dbg("invalid tunnel mode\n");
			}
		} else {
			dbg("Doing nothing....\n");
		}
		bce->type = BCE_NO_ENTRY;
	} else {
		dbg("WARNING parameter pmip_entry_t * bce is NULL\n");
		return -1;
	}
	return 0;
}

int lma_update_binding_entry_with_DMNP_Home_addr(pmip_entry_t * bce, msg_info_t * info)
{
	dbg("lma_update_binding_entry_with_DMNP_Home_addr enter \n");
	if (info->pmipv4_hnp_req_valid)
	{
		bce->pmipv4_hnp_req_valid = 1;
		bce->v4_home_prefix = info->v4_home_prefix;
		bce->v4_home_prefixlen= info->v4_home_prefixlen;
		bce->mnv4_addr = bce->v4_home_prefix;
		dbg("Has Home_addr\n");

	}
	if (info->hasipv4_dmnp_prefix_option)
	{
		bce->hasipv4_dmnp_prefix_option = 1;
		bce->dmnp_prefix= info->dmnp_prefix;
		bce->dmnp_prefix_len= info->dmnp_prefix_len;
		dbg("Has DMNP \n");
	}
}

//---------------------------------------------------------------------------------------------------------------------
int lma_update_binding_entry(pmip_entry_t * bce, msg_info_t * info, int is_v4_lma_mode)
{
	int result = 0;
	int usercount = 0;
	char fname[IFNAMSIZ]={0};
	int ret;
	char command[200];
	char addr_buf[INET6_ADDRSTRLEN];
	if (bce != NULL) {
		if (info != NULL) {
			int result;
			struct in6_addr r_tmp, r_tmp1;
			struct in6_addr zero_v6;
			memset(&r_tmp1, 0, sizeof(struct in6_addr));
			memset(&zero_v6, 0, sizeof(struct in6_addr));
			dbg("Store Binding Entry\n");
			bce->our_addr = conf.OurAddress;
			bce->mn_suffix = info->mn_iid;
			bce->mn_hw_address = info->mn_hw_address;
			if (conf.pmip_tunnel_mode== IP_FAMILY_V4_V01)
			{
				{
					dbg("checking the mag  %s in lma_update_binding entry \n", inet_ntoa(bce->mn_serv_mag_v4_addr));
					if (bce->mn_serv_mag_v4_addr.s_addr == 0) {
						dbg("First new serving MAG:  %s  \n", inet_ntoa(info->srcv4));
						result = 1;
					}
					else if (bce->mn_serv_mag_v4_addr.s_addr != info->srcv4.s_addr) {
						dbg("New serving MAG:  %s \n", inet_ntoa(info->srcv4));
						dbg("(Old MAG:         %s)\n", inet_ntoa(bce->mn_serv_mag_v4_addr));

							//get tunnel Iface name
							ret = tunnelv4_getname(bce->tunnel_v4,fname);
							if (ret == 0)
							{
								dbg("Get Tunnel name failed for v4 ipv6 tunnel\n");
							}
							else
							{
								if (is_v4_lma_mode == 1)
								{
																//Delete  Route
								snprintf (command,200,"ip route del %s dev %s ",inet_ntoa(bce->mnv4_addr), fname);
								system (command);
								dbg("Route table com=%s=\n",command);
								}
								if (!is_v4_lma_mode)
								{
								inet_ntop(AF_INET6, &bce->mn_addr, addr_buf, INET6_ADDRSTRLEN);
								//Delete  Route
								snprintf (command,200,"ip -6 route del %s/%d dev %s ",addr_buf,PREFIX_LENGTH, fname);
								system (command);
								dbg("Route table com=%s=\n",command);
								}
							}
							//decrement users of old tunnel.
							pmipv4_tunnel_del(bce->tunnel_v4);
							dbg("Deleting the old v4 tunnel \n");

						result = 1;
					} else {
						dbg("Same serving MAG: %s, No need to delete tunnel\n", inet_ntoa(info->srcv4));
						// to do here in future: cancel possible delete timer on this tunnel
						result = 0;
					}
				}


			}
			if ((conf.pmip_tunnel_mode == IP_FAMILY_V6_V01))
			{
				if (!is_v4_lma_mode)
				{
					dbg("searching for the prefix for a new BCE entry for pmip v6 mode...\n");
			r_tmp = lma_mnid_hnp_map(bce->mn_hw_address, &result);
			if (result >= 0) {
				if (IN6_ARE_ADDR_EQUAL(&r_tmp, &info->mn_prefix)) {
					bce->mn_prefix = r_tmp;
					dbg("found the prefix  %x:%x:%x:%x:%x:%x:%x:%x in lma_update_binding entry \n", NIP6ADDR(&bce->mn_prefix));

					if (IN6_ARE_ADDR_EQUAL(&bce->mn_serv_mag_addr, &r_tmp1)) {
						dbg("First new serving MAG:  %x:%x:%x:%x:%x:%x:%x:%x  \n", NIP6ADDR(&info->src));
						result = 1;
					} else if (!(IN6_ARE_ADDR_EQUAL(&bce->mn_serv_mag_addr, &info->src))) {
						dbg("New serving MAG:  %x:%x:%x:%x:%x:%x:%x:%x  (Old MAG: %x:%x:%x:%x:%x:%x:%x:%x)\n", NIP6ADDR(&info->src), NIP6ADDR(&bce->mn_serv_mag_addr));
						//delete old route to old tunnel.
						usercount = tunnel_getusers(bce->tunnel);
						if(usercount <= 1)
							lma_remove_route(get_mn_addr(bce), bce->tunnel);
						//decrement users of old tunnel.
						pmip_tunnel_del(bce->tunnel);
						dbg("Deleting the old tunnel \n");
						result = 1;
					} else {
						dbg("Same serving MAG: %x:%x:%x:%x:%x:%x:%x:%x, No need to delete tunnel\n", NIP6ADDR(&info->src));
						// to do here in future: cancel possible delete timer on this tunnel
						result = 0;
					}
				} else {
					dbg("Mobine node prefix changed, delete route, tunnel\n");
					//delete old route to old tunnel.
					usercount = tunnel_getusers(bce->tunnel);
					if(usercount <= 1)
						lma_remove_route(get_mn_addr(bce), bce->tunnel);
					//decrement users of old tunnel.
					pmip_tunnel_del(bce->tunnel);
					dbg("Deleting the old tunnel \n");
					result = 1;
				}


			}

				}
			if (is_v4_lma_mode == 1)
				{
				dbg("pmip v4 mode checking mag\n");
				if (IN6_ARE_ADDR_EQUAL(&zero_v6, &bce->mn_serv_mag_addr))
				{
					dbg("First new serving MAG:  %x:%x:%x:%x:%x:%x:%x:%x  \n", NIP6ADDR(&info->src));
					result = 1;
				}
				else if (!(IN6_ARE_ADDR_EQUAL(&bce->mn_serv_mag_addr, &info->src))) {
							dbg("New serving MAG:  %x:%x:%x:%x:%x:%x:%x:%x  (Old MAG: %x:%x:%x:%x:%x:%x:%x:%x)\n", NIP6ADDR(&info->src), NIP6ADDR(&bce->mn_serv_mag_addr));
							//delete old route to old tunnel.
							usercount = tunnel_getusers(bce->tunnel);
							//if(usercount <= 1)
								//lma_remove_route(get_mn_addr(bce), bce->tunnel);
											//get tunnel Iface name
			ret = tunnel_getname(bce->tunnel,fname);
			if (ret == 0)
			{
				dbg("Get Tunnel name failed for v4 ipv6 tunnel\n");
			}
			else
			{
				//Delete  Route
				snprintf (command,200,"ip route del %s dev %s ",inet_ntoa(bce->mnv4_addr), fname);
				system (command);
				dbg("Route table com=%s=\n",command);
			}
							//decrement users of old tunnel.
							pmip_tunnel_del(bce->tunnel);
							dbg("Deleting the old tunnel \n");
							result = 1;
						} else {
							dbg("Same serving MAG: %x:%x:%x:%x:%x:%x:%x:%x, No need to delete tunnel\n", NIP6ADDR(&info->src));
							// to do here in future: cancel possible delete timer on this tunnel
							result = 0;
						}
				}
			}
			memcpy(bce->timestamp.first,info->timestamp.first, sizeof(info->timestamp.first));
			bce->timestamp.second   = info->timestamp.second;
			if (!is_v4_lma_mode)
			{
			if(!IN6_IS_ADDR_UNSPECIFIED(&info->mn_prefix))
				bce->mn_prefix          = info->mn_prefix;
			bce->mn_addr            = info->mn_addr;
					dbg("MN v6 address: %x:%x:%x:%x:%x:%x:%x:%x, \n", NIP6ADDR(&bce->mn_addr));
			}
			else if (is_v4_lma_mode == 1)
			{
				dbg("MN v4 address: %s, \n", inet_ntoa(bce->mnv4_addr));
			}

			if (conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)
			{
				dbg("Pmip Tunnel mode is v6 store ipv6 address of MAG\n");
			bce->mn_link_local_addr = info->mn_link_local_addr;
			bce->mn_serv_mag_addr   = info->src;
			}
			else if (conf.pmip_tunnel_mode == IP_FAMILY_V4_V01)
			{
				dbg("Pmip Tunnel mode is v4 store ipv4 address of MAG\n");
			bce->mn_serv_mag_v4_addr = info->srcv4;
			}
			else
				dbg("Pmip Tunnel mode is invalid\n");
			bce->lifetime.tv_sec    = info->lifetime.tv_sec;
			bce->lifetime.tv_nsec   = 0;
			bce->n_rets_counter     = conf.MaxMessageRetransmissions;
			bce->seqno_in           = info->seqno;
			bce->link               = info->iif;
			bce->gre_key_valid      = GRE_KEY_VALID;
			bce->gre_key            = DEFAULT_GRE_KEY;
			dbg("Finished updating the binding cache\n");
			return result;
		} else {
			dbg("WARNING parameter msg_info_t* info is NULL\n");
			result = -1;
			return result;
		}
	} else {
		dbg("WARNING parameter pmip_entry_t * bce is NULL\n");
		result = -1;
		return result;
	}
}
