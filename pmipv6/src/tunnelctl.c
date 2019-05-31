/*
 * $Id: tunnelctl.c 1.44 06/04/25 13:24:14+03:00 anttit@tcs.hut.fi $
 *
 * This file is part of the MIPL Mobile IPv6 for Linux.
 *
 * Author: Ville Nuorvala <vnuorval@tcs.hut.fi>
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

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>


#include <asm/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/ip.h>
#include <linux/if_tunnel.h>
#include <linux/ip6_tunnel.h>
#include <pthread.h>

#include "debug.h"
#include "hash.h"
#include "list.h"
#include "util.h"
#include "tunnelctl.h"

#define TUNNEL_DEBUG_LEVEL 1

#if TUNNEL_DEBUG_LEVEL >= 1
#define TDBG dbg
#else
#define TDBG(x...)
#endif

#define IN6ADDRSZ       16
#define INADDRSZ        4


const char basedev[] = "ip6gre0";
const char base4dev[] = "ip4gre1";

static gre_info_type gre_info;

static pthread_mutex_t tnl_lock;
static pthread_mutex_t tnlv4_lock;


static int tnl_fd;
static int tnl_fd4;


struct mip6_tnl {
	struct list_head list;
	struct ip6_tnl_parm2 parm;
	int ifindex;
	int users;
};
struct ip4_tnl_parm2{
	char name[IFNAMSIZ];	/* name of tunnel device */
	int link;		/* ifindex of underlying L2 interface */
	__u8 encap_limit;	/* encapsulation limit for tunnel */
	__u8 hop_limit;		/* hop limit for tunnel */
	__be32 flowinfo;	/* traffic class and flowlabel for tunnel */
	struct in_addr laddr;	/* local tunnel end-point address */
	struct in_addr raddr;	/* remote tunnel end-point address */
	__be32			i_key;
	__be32			o_key;
};


struct mip4_tnl {
	struct list_head list;
	struct ip4_tnl_parm2 parm;
	int ifindex;
	int users;
};

struct mip44_tnl {
	struct list_head list;
	struct ip_tunnel_parm parm;
	//struct ip4_tnl_parm2 parm;
	int ifindex;
	int users;
};


static inline void tnl_dump(struct mip6_tnl *tnl)
{
	TDBG("name: %s\n"
			"link: %d\n"
			"proto: %d\n"
			"encap_limit: %d\n"
			"hop_limit: %d\n"
			"flowinfo: %d\n"
			"flags: %x\n"
			"laddr: %x:%x:%x:%x:%x:%x:%x:%x\n"
			"raddr: %x:%x:%x:%x:%x:%x:%x:%x\n"
			"ifindex: %d\n"
			"users: %d\n",
			tnl->parm.name,
			tnl->parm.link,
			tnl->parm.proto,
			tnl->parm.encap_limit,
			tnl->parm.hop_limit,
			tnl->parm.flowinfo,
			tnl->parm.flags,
			NIP6ADDR(&tnl->parm.laddr),
			NIP6ADDR(&tnl->parm.raddr),
			tnl->ifindex,
			tnl->users);
}

static inline void tnl_parm_dump(struct ip6_tnl_parm2 *parm)
{
	TDBG("name: %s\n"
			"link: %d\n"
			"proto: %d\n"
			"encap_limit: %d\n"
			"hop_limit: %d\n"
			"flowinfo: %d\n"
			"flags: %x\n"
			"laddr: %x:%x:%x:%x:%x:%x:%x:%x\n"
			"raddr: %x:%x:%x:%x:%x:%x:%x:%x\n"
			"ifindex: %d\n"
			"users: %d\n",
			parm->name,
			parm->link,
			parm->proto,
			parm->encap_limit,
			parm->hop_limit,
			parm->flowinfo,
			parm->flags,
			NIP6ADDR(&parm->laddr),
			NIP6ADDR(&parm->raddr));
}

#define TNL_BUCKETS 32

static struct hash tnl_hash;
static struct hash tnlv4_hash;


LIST_HEAD(tnl_list);
LIST_HEAD(tnlv4_list);


static inline struct mip6_tnl *get_tnl(int ifindex)
{
	struct mip6_tnl *tnl = NULL;
	struct list_head *list;
	list_for_each(list, &tnl_list) {
		struct mip6_tnl *tmp;
		tmp = list_entry(list, struct mip6_tnl, list);
		if (tmp->ifindex == ifindex) {
			tnl = tmp;
			break;
		}
	}
	return tnl;
}

static int __tunnel_del(struct mip6_tnl *tnl)
{
	int res = 0;

	tnl->users--;

	TDBG("tunnel %s (%d) from %x:%x:%x:%x:%x:%x:%x:%x "
			"to %x:%x:%x:%x:%x:%x:%x:%x user count decreased to %d\n",
			tnl->parm.name, tnl->ifindex,
			NIP6ADDR(&tnl->parm.laddr), NIP6ADDR(&tnl->parm.raddr),
			tnl->users);

	if (tnl->users == 0) {
		struct ifreq ifr;
		list_del(&tnl->list);
		hash_delete(&tnl_hash, &tnl->parm.laddr, &tnl->parm.raddr);
		strcpy(ifr.ifr_name, tnl->parm.name);
		if ((res = ioctl(tnl_fd, SIOCDELTUNNEL, &ifr)) < 0) {
			TDBG("SIOCDELTUNNEL failed status %d %s\n",
					errno, strerror(errno));
			res = -1;
		} else
			TDBG("tunnel deleted\n");
		free(tnl);
	}
	return res;
}

/**
 * tunnel_del - delete tunnel
 * @ifindex: tunnel interface index
 *
 * Deletes a tunnel identified by @ifindex.  Returns negative if
 * tunnel does not exist, otherwise zero.
 **/
int tunnel_del(int ifindex,
		int (*ext_tunnel_ops)(int request,
			int old_if,
			int new_if,
			void *data),
		void *data)
{
	struct mip6_tnl *tnl;
	int res;

	pthread_mutex_lock(&tnl_lock);
	if ((tnl = get_tnl(ifindex)) == NULL) {
		TDBG("tunnel %d doesn't exist\n", ifindex);
		res = -1;
	} else {
		if (ext_tunnel_ops &&
				ext_tunnel_ops(SIOCDELTUNNEL, tnl->ifindex, 0, data) < 0)
			TDBG("ext_tunnel_ops failed\n");

		if ((res = __tunnel_del(tnl)) < 0)
			TDBG("tunnel %d deletion failed\n", ifindex);
	}
	pthread_mutex_unlock(&tnl_lock);
	return res;
}

static struct mip6_tnl *__tunnel_add(struct in6_addr *local,
		struct in6_addr *remote,
		int link)
{
	struct mip6_tnl *tnl = NULL;
	struct ifreq ifr;

	if ((tnl = malloc(sizeof(struct mip6_tnl))) == NULL)
		return NULL;

	memset(tnl, 0, sizeof(struct mip6_tnl));
	tnl->users = 1;
	tnl->parm.proto = IPPROTO_GRE;
	tnl->parm.flags = IP6_TNL_F_MIP6_DEV|IP6_TNL_F_IGN_ENCAP_LIMIT;
	tnl->parm.hop_limit = 64;
	tnl->parm.laddr = *local;
	tnl->parm.raddr = *remote;
	tnl->parm.link = link;
	if(gre_info.gre_key_valid)
	{
		tnl->parm.i_flags |= GRE_KEY;
		tnl->parm.o_flags |= GRE_KEY;
		tnl->parm.i_key = tnl->parm.o_key = gre_info.gre_key;
	}
	strcpy(ifr.ifr_name, basedev);
	ifr.ifr_ifru.ifru_data = (void *)&tnl->parm;
	if (ioctl(tnl_fd, SIOCADDTUNNEL, &ifr) < 0) {
		TDBG("SIOCADDTUNNEL failed status %d %s\n",
				errno, strerror(errno));
		goto err;
	}
	if (!(tnl->parm.flags & IP6_TNL_F_MIP6_DEV)) {
		TDBG("tunnel exists,but isn't used for MIPv6\n");
		goto err;
	}
	strcpy(ifr.ifr_name, tnl->parm.name);
	if (ioctl(tnl_fd, SIOCGIFFLAGS, &ifr) < 0) {
		TDBG("SIOCGIFFLAGS failed status %d %s\n",
				errno, strerror(errno));
		goto err;
	}
	ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
	if (ioctl(tnl_fd, SIOCSIFFLAGS, &ifr) < 0) {
		TDBG("SIOCSIFFLAGS failed status %d %s\n",
				errno, strerror(errno));
		goto err;
	}
	if (!(tnl->ifindex = if_nametoindex(tnl->parm.name))) {
		TDBG("no device called %s\n", tnl->parm.name);
		goto err;
	}
	if (hash_add(&tnl_hash, tnl, &tnl->parm.laddr, &tnl->parm.raddr) < 0)
		goto err;

	list_add_tail(&tnl->list, &tnl_list);

	TDBG("created tunnel %s (%d) from %x:%x:%x:%x:%x:%x:%x:%x "
			"to %x:%x:%x:%x:%x:%x:%x:%x user count %d\n",
			tnl->parm.name, tnl->ifindex,
			NIP6ADDR(&tnl->parm.laddr), NIP6ADDR(&tnl->parm.raddr),
			tnl->users);

	return tnl;
err:
	free(tnl);
	return NULL;
}

/**
 * tunnel_add - add a tunnel
 * @local: local tunnel address
 * @remote: remote tunnel address
 *
 * Create an IP6-IP6 tunnel between @local and @remote.  Returns
 * interface index of the newly created tunnel, or negative on error.
 **/
int tunnel_add(struct in6_addr *local,
		struct in6_addr *remote,
		int link,
		int (*ext_tunnel_ops)(int request,
			int old_if,
			int new_if,
			void *data),
		void *data)
{
	struct mip6_tnl *tnl;
	int res;
	gre_info_type *gre_data;

	pthread_mutex_lock(&tnl_lock);
	if ((tnl = hash_get(&tnl_hash, local, remote)) != NULL) {
		tnl->users++;
		TDBG("tunnel %s (%d) from %x:%x:%x:%x:%x:%x:%x:%x "
				"to %x:%x:%x:%x:%x:%x:%x:%x user count increased to %d\n",
				tnl->parm.name, tnl->ifindex,
				NIP6ADDR(local), NIP6ADDR(remote), tnl->users);
	} else {

		if(data != NULL && ext_tunnel_ops == NULL){
			gre_data = (gre_info_type *)data;
			gre_info.gre_key_valid = gre_data->gre_key_valid;
			gre_info.gre_key = gre_data->gre_key;
			TDBG("set GRE info to tunnel "
					"gre_valid:%d gre_key:%d\n",gre_info.gre_key_valid, gre_info.gre_key);
		}
		else
		{
			TDBG("Data(GRE) is NULL");
		}
		if ((tnl = __tunnel_add(local, remote, link)) == NULL) {
			TDBG("failed to create tunnel "
					"from %x:%x:%x:%x:%x:%x:%x:%x "
					"to %x:%x:%x:%x:%x:%x:%x:%x\n",
					NIP6ADDR(local), NIP6ADDR(remote));
			pthread_mutex_unlock(&tnl_lock);
			return -1;
		}
	}
	if (ext_tunnel_ops &&
			ext_tunnel_ops(SIOCADDTUNNEL, 0, tnl->ifindex, data) < 0) {
		TDBG("ext_tunnel_ops failed\n");
		__tunnel_del(tnl);
		pthread_mutex_unlock(&tnl_lock);
		return -1;
	}
	res = tnl->ifindex;
	pthread_mutex_unlock(&tnl_lock);
	return res;
}

static int __tunnel_mod(struct mip6_tnl *tnl,
		struct in6_addr *local,
		struct in6_addr *remote,
		int link)
{
	struct ip6_tnl_parm2 parm;
	struct ifreq ifr;

	memset(&parm, 0, sizeof(struct ip6_tnl_parm2));
	parm.proto = IPPROTO_GRE;
	parm.flags = IP6_TNL_F_MIP6_DEV|IP6_TNL_F_IGN_ENCAP_LIMIT;
	parm.hop_limit = 64;
	parm.laddr = *local;
	parm.raddr = *remote;
	parm.link = link;

	strcpy(ifr.ifr_name, tnl->parm.name);
	ifr.ifr_ifru.ifru_data = (void *)&parm;

	if(ioctl(tnl_fd, SIOCCHGTUNNEL, &ifr) < 0) {
		TDBG("SIOCCHGTUNNEL failed status %d %s\n",
				errno, strerror(errno));
		return -1;
	}
	hash_delete(&tnl_hash, &tnl->parm.laddr, &tnl->parm.raddr);
	memcpy(&tnl->parm, &parm, sizeof(struct ip6_tnl_parm2));
	if (hash_add(&tnl_hash, tnl, &tnl->parm.laddr, &tnl->parm.raddr) < 0) {
		free(tnl);
		return -1;
	}
	TDBG("modified tunnel iface %s (%d)"
			"from %x:%x:%x:%x:%x:%x:%x:%x "
			"to %x:%x:%x:%x:%x:%x:%x:%x\n",
			tnl->parm.name, tnl->ifindex, NIP6ADDR(&tnl->parm.laddr),
			NIP6ADDR(&tnl->parm.raddr));
	return tnl->ifindex;

}


/**
 * tunnel_mod - modify tunnel
 * @ifindex: tunnel interface index
 * @local: new local address
 * @remote: new remote address
 *
 * Modifies tunnel end-points.  Returns negative if error, zero on
 * success.
 **/
int tunnel_mod(int ifindex,
		struct in6_addr *local,
		struct in6_addr *remote,
		int link,
		int (*ext_tunnel_ops)(int request,
			int old_if,
			int new_if,
			void *data),
		void *data)
{
	struct mip6_tnl *old, *new;
	int res = -1;

	pthread_mutex_lock(&tnl_lock);

	TDBG("modifying tunnel %d end points with "
			"from %x:%x:%x:%x:%x:%x:%x:%x "
			"to %x:%x:%x:%x:%x:%x:%x:%x\n",
			ifindex, NIP6ADDR(local), NIP6ADDR(remote));

	old = get_tnl(ifindex);
	assert(old != NULL);

	if ((new = hash_get(&tnl_hash, local, remote)) != NULL) {
		if (new != old) {
			new->users++;
			TDBG("tunnel %s (%d) from %x:%x:%x:%x:%x:%x:%x:%x "
					"to %x:%x:%x:%x:%x:%x:%x:%x user count "
					"increased to %d\n",
					new->parm.name, new->ifindex,
					NIP6ADDR(local), NIP6ADDR(remote), new->users);
		}
	} else {
		new = old;

		if (old->users == 1 &&
				(res = __tunnel_mod(old, local, remote, link)) < 0 &&
				(new = __tunnel_add(local, remote, link)) == NULL) {
			pthread_mutex_unlock(&tnl_lock);
			return -1;
		}
	}
	if (ext_tunnel_ops &&
			ext_tunnel_ops(SIOCCHGTUNNEL,
				old->ifindex, new->ifindex, data) < 0) {
		TDBG("ext_tunnel_ops failed\n");
		if (old != new)
			__tunnel_del(new);
		pthread_mutex_unlock(&tnl_lock);
		return -1;
	}
	if (old != new)
		__tunnel_del(old);

	res = new->ifindex;
	pthread_mutex_unlock(&tnl_lock);
	return res;

}

int tunnelctl_init(void)
{
	int res = 0;
	pthread_mutexattr_t mattrs;

	if ((tnl_fd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
		return -1;

	pthread_mutexattr_init(&mattrs);
	pthread_mutexattr_settype(&mattrs, PTHREAD_MUTEX_FAST_NP);
	if (pthread_mutex_init(&tnl_lock, &mattrs))
		return -1;

	pthread_mutex_lock(&tnl_lock);
	res = hash_init(&tnl_hash, DOUBLE_ADDR, TNL_BUCKETS);
	pthread_mutex_unlock(&tnl_lock);
	return res;
}


static int tnl_cleanup(void *data, void *arg)
{
	struct mip6_tnl *tnl = (struct mip6_tnl *) data;
	list_del(&tnl->list);
	hash_delete(&tnl_hash, &tnl->parm.laddr, &tnl->parm.raddr);
	free(tnl);
	return 0;
}

void tunnelctl_cleanup(void)
{
	pthread_mutex_lock(&tnl_lock);
	hash_iterate(&tnl_hash, tnl_cleanup, NULL);
	hash_cleanup(&tnl_hash);
	pthread_mutex_unlock(&tnl_lock);
	close(tnl_fd);
}

int tunnel_getusers(int tun_index)
{
	struct mip6_tnl *tnl;
	int usercount = -1;
	pthread_mutex_lock(&tnl_lock);
	if ((tnl = get_tnl(tun_index)) != NULL) usercount = tnl->users;
	pthread_mutex_unlock(&tnl_lock);
	return usercount;
}
int tunnel_getname(int tun_index,char *fname)
{
	struct mip6_tnl *tnl;
	pthread_mutex_lock(&tnl_lock);
	if ((tnl = get_tnl(tun_index)) != NULL)
	{
		pthread_mutex_unlock(&tnl_lock);
      memcpy(fname,tnl->parm.name,IFNAMSIZ);
      return 1;
	}
	pthread_mutex_unlock(&tnl_lock);
    return 0;
}


static inline struct mip4_tnl *getv4_tnl(int ifindex)
{
	struct mip4_tnl *tnl = NULL;
	struct list_head *list;
	list_for_each(list, &tnlv4_list) {
		struct mip4_tnl *tmp;
		tmp = list_entry(list, struct mip4_tnl, list);
		if (tmp->ifindex == ifindex) {
			tnl = tmp;
			break;
		}
	}
	return tnl;
}

int tunnelv4_getusers(int tun_index)
{
    dbg("in tunnelv4_getusers\n");
	struct mip4_tnl *tnl;
	int usercount = -1;
	pthread_mutex_lock(&tnlv4_lock);
    if ((tnl = getv4_tnl(tun_index)) != NULL) usercount = tnl->users;
	pthread_mutex_unlock(&tnlv4_lock);
    dbg("exit tunnelv4_getusers\n");
	return usercount;
}

int tunnelv4_getname(int tun_index, char *fname)
{
	struct mip4_tnl *tnl;
	pthread_mutex_lock(&tnlv4_lock);
	if ((tnl = getv4_tnl(tun_index)) != NULL)
	{
		pthread_mutex_unlock(&tnlv4_lock);
                memcpy(fname,tnl->parm.name,IFNAMSIZ);
                return 1;
	}
	pthread_mutex_unlock(&tnlv4_lock);
    return 0;
}


int tunnelctlv4_init(void)
{
	int res = 0;
	pthread_mutexattr_t mattrs;
	dbg("V4 Tunnel init\n");
	if ((tnl_fd4 = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		return -1;

	pthread_mutexattr_init(&mattrs);
	pthread_mutexattr_settype(&mattrs, PTHREAD_MUTEX_FAST_NP);
	if (pthread_mutex_init(&tnlv4_lock, &mattrs))
		return -1;

	pthread_mutex_lock(&tnlv4_lock);
	res = hash_init(&tnlv4_hash, DOUBLE_ADDR, TNL_BUCKETS);
	pthread_mutex_unlock(&tnlv4_lock);
	return res;
}

static int tnlv4_cleanup(void *data, void *arg)
{
	struct mip4_tnl *tnl = (struct mip4_tnl *) data;
	memset(&remote_v6, 0 , sizeof(struct in6_addr));
	memset(&local_v6, 0 , sizeof(struct in6_addr));
	
	list_del(&tnl->list);
	ipv4_to_ipv6(&tnl->parm.raddr,&remote_v6);
	ipv4_to_ipv6(&tnl->parm.laddr,&local_v6);
	hash_delete(&tnlv4_hash,&local_v6,&remote_v6);
	free(tnl);
	close(tnl_fd4);
	return 0;
}


void tunnelctlv4_cleanup(void)
{
	pthread_mutex_lock(&tnlv4_lock);
	hash_iterate(&tnlv4_hash, tnlv4_cleanup, NULL);
	hash_cleanup(&tnlv4_hash);
	pthread_mutex_unlock(&tnlv4_lock);
}
static int __tunnelv4_del(struct mip4_tnl *tnl)
{
	int res = 0;
	char command[100];
	memset(&remote_v6, 0 , sizeof(struct in6_addr));
	memset(&local_v6, 0 , sizeof(struct in6_addr));

	tnl->users--;

	TDBG("tunnel %s (%d) from %s "
			"to %s user count decreased to %d\n",
			tnl->parm.name, tnl->ifindex,
			inet_ntoa(tnl->parm.laddr), inet_ntoa(tnl->parm.raddr),
			tnl->users);

	if (tnl->users == 0) {
		struct ifreq ifr;
		list_del(&tnl->list);
		ipv4_to_ipv6(&tnl->parm.raddr,&remote_v6);
		ipv4_to_ipv6(&tnl->parm.laddr,&local_v6);
		hash_delete(&tnlv4_hash, &local_v6, &remote_v6);
		strcpy(ifr.ifr_name, tnl->parm.name);
		snprintf (command,100,"ip tunnel delete %s",base4dev);
		system (command);
		dbg("Tunnel delete comm %s=\n",command);
		TDBG("tunnel deleted\n");
		free(tnl);
	}
	return res;
}


/**
 * tunnelv4_del - delete tunnel
 * @ifindex: tunnel interface index
 *
 * Deletes a tunnel identified by @ifindex.  Returns negative if
 * tunnel does not exist, otherwise zero.
 **/
int tunnelv4_del(int ifindex,
		int (*ext_tunnel_ops)(int request,
			int old_if,
			int new_if,
			void *data),
		void *data)
{
	struct mip4_tnl *tnl;
	int res;

	pthread_mutex_lock(&tnlv4_lock);
	if ((tnl = getv4_tnl(ifindex)) == NULL) {
		TDBG("tunnel %d doesn't exist\n", ifindex);
		res = -1;
	} else {
		if ((res = __tunnelv4_del(tnl)) < 0)
			TDBG("tunnel %d deletion failed\n", ifindex);
	}
	pthread_mutex_unlock(&tnlv4_lock);
	return res;
}

#if 1
static struct mip4_tnl *__tunnel_addv44(struct in_addr *local,
				     struct in_addr *remote,
				     int link)

{
	struct mip44_tnl *tnl = NULL;
	struct ifreq ifr;
	char command[200];
	char *ifac;
	char interfaceName[IFNAMSIZ];
	int ret;
	char tmpstr[INET_ADDRSTRLEN];

	if ((tnl = malloc(sizeof(struct mip44_tnl))) == NULL)
		return NULL;

	memset(tnl, 0, sizeof(struct mip44_tnl));
		tnl->parm.iph.version = 4;
	tnl->parm.iph.ihl = 5;
#ifndef IP_DF
#define IP_DF		0x4000		/* Flag: "Don't Fragment"	*/
#endif
	tnl->parm.iph.frag_off = htons(IP_DF);

	tnl->parm.iph.protocol = IPPROTO_GRE;
	tnl->users = 1;
	//tnl->parm.flags = IP6_TNL_F_MIP6_DEV|IP6_TNL_F_IGN_ENCAP_LIMIT;
	tnl->parm.iph.ttl = 64;
	tnl->parm.iph.saddr = local->s_addr;
	tnl->parm.iph.daddr = remote->s_addr;
	tnl->parm.link = link;
	if(gre_info.gre_key_valid)
	{
	  tnl->parm.i_key = tnl->parm.o_key = gre_info.gre_key;
	  tnl->parm.i_flags |= GRE_KEY;
	  tnl->parm.o_flags |= GRE_KEY;
	}
	//strcpy(tnl->parm.name, "gre0");
	strcpy(ifr.ifr_name, "gre0");

	ifr.ifr_ifru.ifru_data = (void *)&tnl->parm;
	if (ioctl(tnl_fd4, SIOCADDTUNNEL, &ifr) < 0) {
	    TDBG("newn ddddd greeeee SIOCADDTUNNEL failed status %d %s\n",
		 errno, strerror(errno));
	    goto err;
	}
dbg("tun name =%s===",tnl->parm.name);
		strcpy(ifr.ifr_name, tnl->parm.name);
		if (ioctl(tnl_fd4, SIOCGIFFLAGS, &ifr) < 0) {
		TDBG("SIOCGIFFLAGS failed status %d %s name =%s=\n",
		     errno, strerror(errno),ifr.ifr_name);
		goto err;
	}
	ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
	if (ioctl(tnl_fd4, SIOCSIFFLAGS, &ifr) < 0) {
		TDBG("SIOCSIFFLAGS failed status %d %s\n",
		     errno, strerror(errno));
		goto err;
	}

	ifac = if_indextoname(link, &interfaceName);
	if (ifac == NULL)
	{
		dbg("if_indextoname() failed with errno =  %d %s \n",
			errno,strerror(errno));
		return 0;
	}
	inet_ntop(AF_INET, local, tmpstr, INET_ADDRSTRLEN);
	dbg("__tunnel_addv4 Enter WAN dev is %s\n",ifac);
   dbg ("ioctl tunnel add\n");

	if (!(tnl->ifindex = if_nametoindex(tnl->parm.name))) {
		TDBG("no device called %s\n", tnl->parm.name);
		goto err;
	}
	if (hash_add(&tnlv4_hash, tnl, (struct in6_addr *)&tnl->parm.iph.saddr, (struct in6_addr *)&tnl->parm.iph.daddr) < 0)
		goto err;

	list_add_tail(&tnl->list, &tnlv4_list);
	struct in_addr r1;
	r1.s_addr=tnl->parm.iph.saddr;

	TDBG("created tunnel %s (%d) from %s \n",
	     tnl->parm.name, tnl->ifindex,
	     inet_ntoa(r1));

	r1.s_addr=tnl->parm.iph.daddr;
	TDBG("                       to %s user count %d\n",
	     inet_ntoa(r1),
	     tnl->users);

	return tnl;
err:
	free(tnl);
	return NULL;
}

#endif

static struct mip4_tnl *__tunnel_addv4(struct in_addr *local,
		struct in_addr *remote,
		int link)
{
	struct mip4_tnl *tnl = NULL;
	struct ifreq ifr;
	char command[200];
	char *ifac;
	char interfaceName[IFNAMSIZ];
	int ret;
	char tmpstr[INET_ADDRSTRLEN];

	if ((tnl = malloc(sizeof(struct mip4_tnl))) == NULL)
		return NULL;

	memset(tnl, 0, sizeof(struct mip4_tnl));
	tnl->users = 1;
	//tnl->parm.flags = IP6_TNL_F_MIP6_DEV|IP6_TNL_F_IGN_ENCAP_LIMIT;
	tnl->parm.hop_limit = 64;
	tnl->parm.laddr = *local;
	tnl->parm.raddr = *remote;
	tnl->parm.link = link;
	if(gre_info.gre_key_valid)
	{
		tnl->parm.i_key = tnl->parm.o_key = gre_info.gre_key;
	}
	strcpy(tnl->parm.name, base4dev);

	ifac = if_indextoname(link, &interfaceName);
	if (ifac == NULL)
	{
		dbg("if_indextoname() failed with errno =  %d %s \n",
				errno,strerror(errno));
		return 0;
	}
	inet_ntop(AF_INET, local, tmpstr, INET_ADDRSTRLEN);
	dbg("__tunnel_addv4 Enter WAN dev is %s\n",ifac);

	//looks to be some bug in ip tunnel command. Ip tunnel add not providing LLipv6 address to interface
	//creating temperary tunnel & deleting that once main tunnel is created successfully
	pmip_system_call("ip t d ip4gre1",strlen("ip t d ip4gre1"),1);
	//snprintf(command,200,"ip tunnel add %s mode gre remote %s local %s ttl %d dev %s ikey %d okey %d",tnl->parm.name,tmpstr,inet_ntoa(*remote),tnl->parm.hop_limit,ifac,tnl->parm.i_key,tnl->parm.o_key);
	//	ret=system(command);
	snprintf(command,200,"ip tunnel add %s mode gre remote 1.2.3.4 local 3.4.5.6 ttl %d ikey %d okey %d",tnl->parm.name,tnl->parm.hop_limit,tnl->parm.i_key,tnl->parm.o_key);
	pmip_system_call(command,strlen(command),1);
		snprintf(command,200,"ip tunnel change %s remote %s local %s",tnl->parm.name,inet_ntoa(*remote),tmpstr);
	pmip_system_call(command,strlen(command),1);
	snprintf(command,200,"ip link set %s up",tnl->parm.name);
	pmip_system_call(command,strlen(command),1);


	pmip_system_call("ip t s",strlen("ip t s"),1);

	if (!(tnl->ifindex = if_nametoindex(tnl->parm.name))) {
		TDBG("no device called %s\n", tnl->parm.name);
		goto err;
	}
	//convert ipv4 address to ipv6
	ipv4_to_ipv6(&tnl->parm.raddr,&remote_v6);
	ipv4_to_ipv6(&tnl->parm.laddr, &local_v6);
	dbg("Local v4 ip %x,local con v6 ip %x:%x:%x:%x:%x:%x:%x:%x\n",tnl->parm.laddr.s_addr,NIP6ADDR(&local_v6));
	dbg("Remote v4 ip %x,remote v6 ip %x:%x:%x:%x:%x:%x:%x:%x\n",tnl->parm.raddr.s_addr,NIP6ADDR(&remote_v6));
	if (hash_add(&tnlv4_hash, tnl, &local_v6, &remote_v6) < 0)
		goto err;

	list_add_tail(&tnl->list, &tnlv4_list);

	TDBG("created tunnel %s (%d) from %s \n",
			tnl->parm.name, tnl->ifindex,
			inet_ntoa(tnl->parm.laddr));
	TDBG("                       to %s user count %d\n",
			inet_ntoa(tnl->parm.raddr),
			tnl->users);

	return tnl;
err:
	free(tnl);
	return NULL;
}


/**
 * tunnel_add_v4 - add a v4 tunnel
 * @local: local tunnel address
 * @remote: remote tunnel address
 *
 * Create an IP-gre tunnel between @local and @remote.  Returns
 * interface index of the newly created tunnel, or negative on error.
 **/
int tunnel_addv4(struct in_addr *local,
		struct in_addr *remote,
		int link,
		int (*ext_tunnel_ops)(int request,
			int old_if,
			int new_if,
			void *data),
		void *data)
{
	struct mip4_tnl *tnl;
	int res;
	gre_info_type *gre_data;

	memset(&remote_v6, 0 , sizeof(struct in6_addr));
	memset(&local_v6, 0 , sizeof(struct in6_addr));
	ipv4_to_ipv6(remote,&remote_v6);
	ipv4_to_ipv6(local,&local_v6);

	pthread_mutex_lock(&tnlv4_lock);
	if ((tnl = hash_get(&tnlv4_hash, &local_v6, &remote_v6)) != NULL) {
		tnl->users++;
		TDBG("tunnel %s (%d) from %s \n",
				tnl->parm.name, tnl->ifindex,
				inet_ntoa(*local));
		TDBG(" to %s user count increased to %d\n",inet_ntoa(*remote), tnl->users);
	} else {

		if(data != NULL && ext_tunnel_ops == NULL){
			gre_data = (gre_info_type *)data;
			gre_info.gre_key_valid = gre_data->gre_key_valid;
			gre_info.gre_key = gre_data->gre_key;
			TDBG("set GRE info to tunnel "
					"gre_valid:%d gre_key:%d\n",gre_info.gre_key_valid, gre_info.gre_key);
		}
		else
		{
			TDBG("Data(GRE) is NULL");
		}
		if ((tnl = __tunnel_addv4(local, remote, link)) == NULL) {
			TDBG("failed to create tunnel "
					"from %s "
					"to %s\n",
					inet_ntoa(*local), inet_ntoa(*remote));
			pthread_mutex_unlock(&tnlv4_lock);
			return -1;
		}
	}
	res = tnl->ifindex;
	pthread_mutex_unlock(&tnlv4_lock);
	return res;
}


