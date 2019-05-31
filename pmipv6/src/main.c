/*
 * $Id: main.c 1.67 06/05/05 19:40:57+03:00 anttit@tcs.hut.fi $
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
 * 02111-1307 USA
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
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <syslog.h>
#include <netinet/icmp6.h>

#include "conf.h"
#include "cn.h"
#include "ha.h"
#include "mn.h"
#include "mh.h"
#include "keygen.h"
#include "debug.h"
#include "bcache.h"
#include "policy.h"
#include "xfrm.h"
#include "icmp6.h"
#ifdef ENABLE_VT
#include "vt.h"
#endif
#include "tunnelctl.h"
#include "pmip.h"
#include "pmipv4_conf.h"


static void sig_child(int unused)
{
	int pid, status;

	while ((pid = waitpid(0, &status, WNOHANG)) > 0);
}

static void reinit(void)
{
	/* got SIGHUP, reread configuration and reinitialize */
	dbg("got SIGHUP, reinitilize\n");
	return;
}


struct mip6_config conf;

static void terminate(void)
{
	/* got SIGINT, cleanup and exit */
	syslog(LOG_INFO, "terminated (SIGINT)");
	dbg("got SIGINT, exiting\n");
	system("echo 'pmip: Got Pmip Terminate signal ' > /dev/kmsg");
	pmip_cleanup();
	debug_close();
	pthread_exit(NULL);
}

/*
 * Detach from any tty.
 */
static void daemon_start(int ignsigcld)
{
	register int childpid, fd;

	if (getppid() == 1) goto out;

#ifdef SIGTTOU
	signal(SIGTTOU, SIG_IGN);
#endif
#ifdef SIGTTIN
	signal(SIGTTIN, SIG_IGN);
#endif
#ifdef SIGTSTP
	signal(SIGTSTP, SIG_IGN);
#endif

	if ((childpid = fork()) < 0)
		fprintf(stderr, "can't fork first child\n");
	else if (childpid > 0)
		exit(0);

	if (setpgrp() == -1)
		fprintf(stderr, "can't change process group\n");
	if ((fd = open("/dev/tty", O_RDWR)) >= 0) {
		ioctl(fd, TIOCNOTTY, (char *)NULL);
		close(fd);
	}

out:
	for (fd = 0; fd < NOFILE; fd++) close(fd);
	errno = 0;

	chdir("/tmp");
	umask(0);

	if (ignsigcld) {
#ifdef SIGTSTP
		signal(SIGCLD, sig_child);
#else
		signal(SIGCLD, SIG_IGN);
#endif
	}
}


const char *entity_string[5] = {
	"Correspondent Node",
	"Mobile Node",
	"Home Agent",
	"Mobile Access Gateway",
	"Local Mobility Anchor" };

int getifaceip(struct in_addr *addr, char *iface)
{

	int s, ret;
	struct ifreq buffer;
	struct sockaddr_in *sa;

	if ( addr== NULL)
	{
		dbg("Null arguements passed.\n");
		return -1;
	}

	/* Open a socket */
	s = socket(PF_INET, SOCK_DGRAM, 0);
	if ( s < 0 )
	{
		dbg("Unable to open socket to get IP address.\n");
		return -1;
	}
	/* Set up the interface request buffer for wlan0. */
	memset(&buffer, 0x00, sizeof(buffer));
	strcpy(buffer.ifr_name, iface);

	/* Call the ioctl to get the address. */
	ret = ioctl(s, SIOCGIFADDR, &buffer);

	if ( ret < 0 )
	{
		dbg("Unable to call ioctl to get IP address.\n");
		/* Close the socket handle. */
		close(s);
		return -1;
	}
	/* Copy out the ip address for the interface. */
	sa = (struct sockaddr_in *)&(buffer.ifr_addr);

	*addr = sa->sin_addr;

	//dbg("req Iface %s,result Interface ip %s\n",iface,inet_ntoa(*addr));


	/* Close the socket handle. */
	close(s);

	return 0;
}


int main(int argc, char **argv)
{
	int logflags = 0;
	int ret = 1;
	struct in_addr sa;
	char command[MAX_DHCP_COMMAND_STR_LEN] = {0};

	debug_init();

	//handling signal
	signal (SIGHUP,reinit);
	signal(SIGTERM, terminate);
	signal(SIGINT, terminate);
	signal(SIGPIPE, terminate);

	if (conf_parse(&conf, argc, argv))
	{
		system("echo 'pmip: conf parse failed. Exit ' > /dev/kmsg");
		printf("\npmip: conf parse failed. Exit . Check dmesg for more detail\n");
		return 1;
	}

	system("echo 'pmip: conf file/argument parse succeeded ' > /dev/kmsg");

	if (conf.debug_level > 0)
		logflags = LOG_PERROR;

	dbg("------ pmip mode %d-------------\n",conf.pmip_mode_type);

	dbg("------ pmipv4 work mode %d-------------\n",conf.pmipv4_work_mode_type);
	dbg("------ pmip Tunnel Mode %d-------------\n",conf.pmip_tunnel_mode);

	openlog(basename(argv[0]), LOG_PID|logflags, LOG_DAEMON);

	syslog(LOG_INFO, "%s v%s started (%s)", PACKAGE_NAME, PACKAGE_VERSION,
			entity_string[conf.mip6_entity]);
#ifdef ENABLE_VT
	if (vt_init() < 0)
		goto vt_failed;
#endif

	/* if not debugging, detach from tty */
	if (conf.debug_level == 0)
	{
		daemon_start(1);
	}
	else {
		/* if debugging with debug log file, detach from tty */
		if (conf.debug_log_file) {
			daemon_start(1);

			ret = debug_open(conf.debug_log_file);
			if (ret < 0) {
				fprintf(stderr, "can't init debug log:%s\n",
						strerror(-ret));
				goto debug_failed;
			}
			dbg("%s started in debug mode\n", PACKAGE_NAME);
		} else {
			dbg("%s started in debug mode, not detaching from terminal\n",
					PACKAGE_NAME);
		}
		conf_show(&conf);
	}
	if (strncmp(conf.ServiceSelectionIdentifier,
		PMIP_INVALID_ID_STRING,strlen(PMIP_INVALID_ID_STRING)) != 0)
	{
		dbg("Note: Service selection string is valid(%s). PBU will have Service Selection Identifier\n", conf.ServiceSelectionIdentifier);
		conf.is_valid_service_id = 1;
	}
	else
	{
		dbg("Note: Service selection string is invalid(-1). PBU will not have Service Selection Identifier\n", conf.ServiceSelectionIdentifier);
		conf.is_valid_service_id = 0;
	}

	srandom(time(NULL));

	if (rr_cn_init() < 0)
		goto rr_cn_failed;
	if (policy_init() < 0)
		goto policy_failed;
	if (taskqueue_init() < 0)
		goto taskqueue_failed;
	if (bcache_init() < 0)
		goto bcache_failed;

	//If tunnel mode v6 or Pmip mode is v6 listen for IPv6 traffic
	if ((conf.pmip_mode_type == IP_FAMILY_V4_V01 && conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)
		 || (conf.pmip_mode_type == IP_FAMILY_V6_V01 && conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)
		 || (conf.pmip_mode_type == IP_FAMILY_V4V6_V01 && conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)
		 || (conf.pmip_mode_type == IP_FAMILY_V6_V01) ||(conf.pmip_mode_type == IP_FAMILY_V4V6_V01))
	{
	if (mh_init() < 0)
		goto mh_failed;
	}else {
		dbg("Tunnel mode is not v6/Pmip mode is not v6. so dont do mh_init\n");
	}
	//If tunnel mode v4 listen for Mipv6 packet over IPv4 traffic
	if ((conf.pmip_mode_type == IP_FAMILY_V4_V01 && conf.pmip_tunnel_mode == IP_FAMILY_V4_V01)
		 || (conf.pmip_mode_type == IP_FAMILY_V6_V01 && conf.pmip_tunnel_mode == IP_FAMILY_V4_V01)
		 || (conf.pmip_mode_type == IP_FAMILY_V4V6_V01 && conf.pmip_tunnel_mode == IP_FAMILY_V4_V01))
	{
	if (mhv4_init() < 0)
			goto mhv4_failed;
	}else {
		dbg("Tunnel mode is not v4 so dont do mhv4_init\n");
	}
	//based on pmip mode listen for traffic
	if (is_mag() && ((conf.pmip_mode_type == IP_FAMILY_V6_V01) ||(conf.pmip_mode_type == IP_FAMILY_V4V6_V01)))
	{
		dbg(" pmip mode is v6 so enable icmpv6 deamon\n");
		if (icmp6_init() < 0)
			goto icmp6_failed;

		//comment code for forwading dhcpv6 packet to tunnel may be need in future
#ifdef FORWARD_DHCPV6_TO_TUNNEL
		dbg("All dhcpv6 packet will not be forwading to Tunnel\n");
		if (dhcpv6_info_init() < 0) {
			dbg ("dhcpv6_info_init failed\n");
			//goto fail;;
		}
		else
			dbg ("Multicast init passed\n");
#endif
		if (conf.pmip_mode_type != IP_FAMILY_V4V6_V01) //in case of V4V6 it will be handle in pmipv4 case
		{
			//Add dnsmasq option to provide dhcpv6 info
			if (pmipv6_start_dnsmasq_with_dhcpv6_option() < 0)
				dbg ("dnsmasq dhcpv6 option addition failed\n");
			else
				dbg ("dnsmasq dhcpv6 option addition successful\n");
		}
		else
		{
			dbg ("Pmip mode is v4v6 . dnsmasq dhcpv6 dns option will be added in pmipv4\n");
		}
	}
	else
	{
		dbg("pmip mode is v4 so dont enable icmpv6 deamon\n");
	}
	if (is_mag() && (conf.pmip_mode_type == IP_FAMILY_V4_V01 || conf.pmip_mode_type == IP_FAMILY_V4V6_V01)) {
		dbg("pmip mode is v4 start dhcp deamon\n");
		//start dnsmasq for ipv4
		dbg("Kiiling dnsmasq . & start dnsmasq to not provide any ip\n");
		system("killall -1 dnsmasq");
		system("killall -15 dnsmasq");
		system("killall -9 dnsmasq");
		ret = getifaceip(&sa, BRIDGE_IFACE);
		if (ret == 0) {
			//clear DHCP reservation file
			snprintf (command,MAX_DHCP_COMMAND_STR_LEN,"echo > %s",PMIP_DHCP_RESER_FILE);
			pmip_system_call(command,strlen(command),1);

			snprintf (command,MAX_DHCP_COMMAND_STR_LEN,"dnsmasq -i %s -I lo -z --dhcp-range=%s,%s,static,255.255.255.0,43200 --dhcp-hostsfile=%s --dhcp-option-force=6,%s --dhcp-option-force=3,%s",BRIDGE_IFACE,BRIDGE_IFACE,inet_ntoa(sa),PMIP_DHCP_RESER_FILE,inet_ntoa(sa),inet_ntoa(sa));
			if (conf.pmip_mode_type == IP_FAMILY_V4V6_V01)
			{
				dbg ("Add dnsmasq ipv6 dnsoption\n");
				if (pmip_add_dnsmasq_v6dns_option(command) < 0)
				{
					dbg("pmip_add_dnsmasq_v6dns_option failed\n");
				}
			}
			pmip_system_call(command,strlen(command),1);
		} else {

			dbg("getting ip failed for %s\n", BRIDGE_IFACE);
		}

		if (dhcp_init() < 0) {
			dbg ("dhcp init failed failed\n");
			goto dhcp_failed;
		}
		else
			dbg ("dhcp init passed\n");
	}
	else
	{
		dbg("not pmip v4 mode do not start dhcp deamon mode=%d\n",conf.pmip_mode_type);
	}

	//If tunnel mode v6 or Pmip mode is v6 listen for IPv6 traffic
	if ((conf.pmip_mode_type == IP_FAMILY_V4_V01 && conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)
		 || (conf.pmip_mode_type == IP_FAMILY_V6_V01 && conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)
		 || (conf.pmip_mode_type == IP_FAMILY_V4V6_V01 && conf.pmip_tunnel_mode == IP_FAMILY_V6_V01))
	{
		if ((is_ha() || is_mn() || is_lma() || is_mag()) && ((tunnelctl_init() < 0) ))
		goto tunnelctl_failed;
	}
	else
		dbg("Do not Init v6 tunnel As v6 tunnel not needed\n");
	//If tunnel mode v4 init v4 tunnel
	if ((conf.pmip_mode_type == IP_FAMILY_V4_V01 && conf.pmip_tunnel_mode == IP_FAMILY_V4_V01)
		 || (conf.pmip_mode_type == IP_FAMILY_V6_V01 && conf.pmip_tunnel_mode == IP_FAMILY_V4_V01)
		 || (conf.pmip_mode_type == IP_FAMILY_V4V6_V01 && conf.pmip_tunnel_mode == IP_FAMILY_V4_V01))
	{
		if((is_lma() || is_mag()) && tunnelctlv4_init() < 0)
			goto tunnelctlv4_failed;
	}
	else
		dbg("Do not Init v4 tunnel As v4 tunnel not needed\n");
	if (is_ha() && ha_init() < 0)
		goto ha_failed;
	if (is_mn() && mn_init() < 0)
		goto mn_failed;
	if (is_lma() && pmip_lma_init() < 0)
		goto pmip_failed;
	if (is_mag() && pmip_mag_init() < 0)
		goto pmip_failed;
	dbg("Waiting.....\n");
	while(1);

#ifdef ENABLE_VT
	if (vt_start(conf.vt_hostname, conf.vt_service) < 0)
		goto vt_start_failed;
#endif
#ifdef ENABLE_VT
	vt_fini();
vt_start_failed:
#endif
	if (is_mn())
		mn_cleanup();
pmip_failed:
	pmip_cleanup();
mn_failed:
	if (is_mn())
		mn_cleanup();

ha_failed:
	if (is_ha())
		ha_cleanup();
tunnelctl_failed:
	if (is_ha() || is_mn())
	{
		tunnelctl_cleanup();
		tunnelctlv4_cleanup();
	}
tunnelctlv4_failed:
	if (is_ha() || is_mn())
		{
		tunnelctlv4_cleanup();
		}

icmp6_failed:
	icmp6_cleanup();
mh_failed:
	mh_cleanup();
mhv4_failed:
	mhv4_cleanup();
bcache_failed:
	bcache_cleanup();
taskqueue_failed:
	taskqueue_destroy();
policy_failed:
	policy_cleanup();
rr_cn_failed:

debug_failed:
	debug_close();
#ifdef ENABLE_VT
vt_failed:
#endif

dhcp_failed:
	dhcp_cleanup();

	syslog(LOG_INFO, "%s v%s stopped (%s)", PACKAGE_NAME, PACKAGE_VERSION,
			entity_string[conf.mip6_entity]);
	closelog();
	return ret;
}
