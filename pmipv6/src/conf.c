/*
 * $Id: conf.c 1.50 06/05/12 11:48:36+03:00 vnuorval@tcs.hut.fi $
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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

#include <netinet/in.h>
#include <netinet/ip6mh.h>
#include <arpa/inet.h>
#include "defpath.h"
#include "conf.h"
#include "debug.h"
#include "util.h"
#include "mipv6.h"
#ifdef ENABLE_VT
#include "vt.h"
#endif

static void conf_usage(char *exec_name)
{
	fprintf(stderr,
			"Usage: %s [options]\nOptions:\n"
			"  -V, --version                Display version information and copyright\n"
			"  -?, -h, --help               Display this help text\n"
			"  -c <file>                    Read configuration from <file>\n"
#ifdef ENABLE_VT
			"      --vt-service <serv>      Set VT service (default=" VT_DEFAULT_SERVICE ")\n"
#endif
			"\n These options override values read from config file:\n"
			"  -d <number>                  Set debug level (0-10)\n"
			"  -l <file>                    Write debug log to <file> instead of stderr\n"
			"  -C, --correspondent-node     Node is CN\n"
			"  -H, --home-agent             Node is HA\n"
			"  -M, --mobile-node            Node is MN\n\n"
			"  -m, --pmip-mode              pmip running mode\n"
			"  -w,  --pmipv4-mode            pmipv4 working mode\n"
			"  -t   --pmipv4-tunnel-mode    pmipv4 Tunnel mode\n"
			"  -s,  --dhcp-start-ip         dhcp LAN start ip\n"
			"  -e,  --dhcp-end-ip           dhcp LAN end ip\n"
			"  -j,   --subnet-mask          dhcp LAN subnet mask\n"
			"  -i,  --mobile-node-identifier-type mobile node identifier type"
			"For bug reporting, see %s.\n", exec_name, PACKAGE_BUGREPORT);
}

static void conf_version(void)
{
	fprintf(stderr,
			"%s (%s) %s\n"
			"%s\n"
			"This is free software; see the source for copying conditions.  There is NO\n"
			"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n",
			PACKAGE, PACKAGE_NAME, PACKAGE_VERSION, PACKAGE_COPYRIGHT);
}

static int conf_alt_file(char *filename, int argc, char **argv)
{
	int args_left = argc;
	char **cur_arg = argv;

	while (args_left--) {
		if (strcmp(*cur_arg, "-c") == 0 && args_left > 0) {
			cur_arg++;
			if (**cur_arg == '-')
				return -EINVAL;
			if (strlen(*cur_arg) >= MAXPATHLEN)
				return -ENAMETOOLONG;
			strcpy(filename, *cur_arg);
			return 0;
		}
		cur_arg++;
	}

	return 1;
}
int pmip_system_call
(
 const char   *command,
 unsigned int  cmdlen,
 int print_command
 )
{
	int result = -1;
	FILE *stream = NULL;
	unsigned int vallen = strlen( command );

	if( vallen != cmdlen ) {
		dbg( "system call length mismatch: %d != %d\n", cmdlen, vallen );
		return -1;
	}
	if (print_command)
		dbg("system call command: %s\n", command);

	stream = popen( command, "w" );
	if( stream == NULL )
	{
		dbg("system command failed error num %d error msg is %s\n",errno, strerror(errno));
		result = -1;
	}
	else
	{
		result = pclose( stream );
		if (WIFEXITED(result))
		{
			result = WIFEXITED(result);
		}
		else
	{
		dbg("pclose command failed\n");
	}
	}
	if (result < 0)
		dbg("system command failed, return=%d\n",result);

	return result;
}


static int conf_file(struct mip6_config *c, char *filename)
{
	extern FILE *yyin;
	int ret;

	yyin = fopen(filename, "r");
	if (yyin == NULL)
	{
		system("echo 'pmip: conf file open failed:error -ENOENT ' > /dev/kmsg");
		return -ENOENT;
	}

	c->config_file = malloc(strlen(filename) + 1);
	if (c->config_file == NULL)
	{
		system("echo 'pmip: No mem:error -ENOMEM ' > /dev/kmsg");
		return -ENOMEM;
	}
	strcpy(c->config_file, filename);

	ret = yyparse();

	fclose(yyin);
	if (ret)
	{
		//print message in dmesg if file parse failed
		system("echo 'pmip: file parse failed' > /dev/kmsg");
		return -EINVAL;
	}

	system("echo 'pmip: file parse ok' > /dev/kmsg");

	return 0;
}

static int conf_cmdline(struct mip6_config *cfg, int argc, char **argv)
{
	static struct option long_opts[] = {
		{"version", 0, 0, 'V'},
		{"help", 0, 0, 'h'},
		{"correspondent-node", 0, 0, 'C'},
		{"home-agent", 0, 0, 'H'},
		{"mobile-node", 0, 0, 'M'},
		{"pmip-mode", 0, 0, 'm'},
		{"pmipv4-mode", 0, 0, 'w'},
		{"pmip-tunnel-mode", 0, 0, 't'},
		{"dhcp-start-ip", 0, 0, 's'},
		{"dhcp-end-ip", 0, 0, 'e'},
		{"subnet-mask", 0, 0, 'j'},
		{"mobile-node-identifier-type", 0, 0, 'i'},
		{"show-config", 0, 0, 0},
#ifdef ENABLE_VT
		{"vt-service", 1, 0, 0 },
#endif
		{0, 0, 0, 0}
	};

	/* parse all other cmd line parameters than -c */
	while (1) {
		int idx, c;
		c = getopt_long(argc, argv, "t:j:e:s:w:m:i:c:d:l:Vh?CMH", long_opts, &idx);
		if (c == -1) break;

		switch (c) {
			case 0:
#ifdef ENABLE_VT
				if (strcmp(long_opts[idx].name, "vt-service") == 0) {
					cfg->vt_service = optarg;
					break;
				}
#endif
				if (idx == 5)
					conf_show(cfg);
				return -1;
			case 'V':
				conf_version();
				return -1;
			case '?':
			case 'h':
				conf_usage(basename(argv[0]));
				return -1;
			case 'd':
				cfg->debug_level = atoi(optarg);
				break;
			case 'i':
				cfg->mobile_node_identifier_type = atoi(optarg);
					//checking validity
				if ((cfg->mobile_node_identifier_type == MOBILE_NODE_IDENTIFIER_TYPE_MAC) ||
					(cfg->mobile_node_identifier_type == MOBILE_NODE_IDENTIFIER_TYPE_STRING))
				{
					system("echo 'pmip: valid mobile node identifier type ' > /dev/kmsg");
					break;
				}
				else
				{
					system("echo 'pmip: Invalid mobile node identifier type ' > /dev/kmsg");
					return -1;
				}
				break;
			case 'm':
				cfg->pmip_mode_type= atoi(optarg);
				//checking valid pmip mode
				if ((cfg->pmip_mode_type == IP_FAMILY_V4_V01) ||
					(cfg->pmip_mode_type == IP_FAMILY_V6_V01) ||
					(cfg->pmip_mode_type == IP_FAMILY_V4V6_V01))
				{
					system("echo 'pmip: valid pmip mode ' > /dev/kmsg");
				break;

				}
				else
				{
					system("echo 'pmip: Invalid pmip mode ' > /dev/kmsg");
					return -1;
				}
			case 'w':
				cfg->pmipv4_work_mode_type= atoi(optarg);
				//checking if pmipv4 work mode is valid
				if ((cfg->pmipv4_work_mode_type == PMIPV4_MODE_CPE_V01) ||
					(cfg->pmipv4_work_mode_type == PMIPV4_MODE_SECONDARY_ROUTER_V01))
				{
					system("echo 'pmip: valid pmip v4 work mode ' > /dev/kmsg");
				break;
				}
				else
				{
					system("echo 'pmip: Invalid pmip v4 work mode ' > /dev/kmsg");
					return -1;
				}
			case 't':
				cfg->pmip_tunnel_mode= atoi(optarg);
				//checking valid pmip tunnel mode
				if ((cfg->pmip_tunnel_mode == IP_FAMILY_V4_V01) ||
					(cfg->pmip_tunnel_mode == IP_FAMILY_V6_V01))
				{
					system("echo 'pmip: valid pmip Tunnel mode ' > /dev/kmsg");
				break;
				}
				else
				{
					system("echo 'pmip: Invalid pmip Tunnel mode ' > /dev/kmsg");
					return -1;
				}
			case 's':
				inet_aton(optarg, &cfg->dhcp_start_ip);
				break;
			case 'e':
				inet_aton(optarg , &cfg->dhcp_end_ip);
				conf.current_ip =conf.dhcp_end_ip.s_addr;
				dbg ("current ip range start is %p\n",conf.current_ip);
				break;
			case 'j':
				inet_aton(optarg, &cfg->dhcp_sub_mask);
				break;
			case 'l':
				cfg->debug_log_file = optarg;
				break;
			case 'C':
				cfg->mip6_entity = MIP6_ENTITY_CN;
				break;
			case 'H':
				cfg->mip6_entity = MIP6_ENTITY_HA;
				break;
			case 'M':
				cfg->mip6_entity = MIP6_ENTITY_MN;
				break;
			default:
				break;
		};
	}
	return 0;
}

static void conf_default(struct mip6_config *c)
{
	memset(c, 0, sizeof(*c));
	// Common options
#ifdef ENABLE_VT
	c->vt_hostname = VT_DEFAULT_HOSTNAME;
	c->vt_service = VT_DEFAULT_SERVICE;
#endif
	c->mip6_entity = MIP6_ENTITY_CN;
	pmgr_init(NULL, &conf.pmgr);
	INIT_LIST_HEAD(&c->net_ifaces);
	INIT_LIST_HEAD(&c->bind_acl);
	c->DefaultBindingAclPolicy = IP6_MH_BAS_ACCEPTED;

	// IPsec options
	c->UseMnHaIPsec = 0;
	INIT_LIST_HEAD(&c->ipsec_policies);

	// MN options
	c->MnMaxHaBindingLife = MAX_BINDING_LIFETIME;
	c->MnMaxCnBindingLife = MAX_RR_BINDING_LIFETIME;
	tssetdsec(c->InitialBindackTimeoutFirstReg_ts, 1.5);//seconds
	tssetsec(c->InitialBindackTimeoutReReg_ts, INITIAL_BINDACK_TIMEOUT);//seconds
	INIT_LIST_HEAD(&c->home_addrs);
	c->MoveModulePath = NULL; // internal
	c->DoRouteOptimizationMN = 1;
	c->SendMobPfxSols = 1;
	c->OptimisticHandoff = 0;

	/* PMIP global options */
	c->RFC5213TimestampBasedApproachInUse = 1;
	c->RFC5213MobileNodeGeneratedTimestampInUse = 1;
	c->RFC5213FixedMAGLinkLocalAddressOnAllAccessLinks = in6addr_any;
	c->RFC5213FixedMAGLinkLayerAddressOnAllAccessLinks = in6addr_any;

	/* PMIP LMA options */
	struct timespec lifetime1;
	lifetime1.tv_sec = 10;
	lifetime1.tv_nsec = 0;
	c->RFC5213MinDelayBeforeBCEDelete         = lifetime1; // 10000 milliseconds
	struct timespec lifetime2;
	lifetime2.tv_sec = 1;
	lifetime2.tv_nsec = 500000000;
	c->RFC5213MaxDelayBeforeNewBCEAssign      = lifetime2; // 1500 milliseconds
	struct timespec lifetime3;
	lifetime3.tv_sec = 0;
	lifetime3.tv_nsec = 300000000;
	c->RFC5213TimestampValidityWindow         = lifetime3; // 300 milliseconds

	// HA options
	c->SendMobPfxAdvs       = 1;
	c->SendUnsolMobPfxAdvs  = 1;
	c->MaxMobPfxAdvInterval = 86400; // seconds
	c->MinMobPfxAdvInterval = 600; // seconds
	c->HaMaxBindingLife     = MAX_BINDING_LIFETIME;

	// CN bindings
	c->DoRouteOptimizationCN = 1;

	/* PMIP MAG options */
	c->RFC5213EnableMAGLocalRouting = 0;
	c->HomeNetworkPrefix             = in6addr_any;
	c->MagAddressIngress             = in6addr_loopback;
	c->MagAddressEgress              = in6addr_loopback;
	c->MagDeviceIngress              = "";
	c->ServiceSelectionIdentifier    = "";
	c->MnIdString             = "";
	c->MagDeviceEgress               = "";
	c->LmaAddress                    = in6addr_loopback;
	c->OurAddress                    = in6addr_loopback;
	memset(&c->PrimaryV6Dns,0,sizeof(c->PrimaryV6Dns));
	memset(&c->SecondaryV6Dns,0,sizeof(c->SecondaryV6Dns));
	memset(&c->PrimaryV4Dns,0,sizeof(c->PrimaryV4Dns));
	memset(&c->SecondaryV4Dns,0,sizeof(c->SecondaryV4Dns));
	//Lifetime for PB entry
	struct timespec lifetime4;
	lifetime4.tv_sec  = 40;
	lifetime4.tv_nsec = 0;
	c->PBULifeTime    = lifetime4;
	struct timespec lifetime5;
	lifetime5.tv_sec  = 40;
	lifetime5.tv_nsec = 0;
	c->PBALifeTime    = lifetime5;
	//Time for N_Retransmissions
	struct timespec lifetime6;
	lifetime6.tv_sec  = 1;
	lifetime6.tv_nsec = 0;
	c->RetransmissionTimeOut = lifetime6;
	//Define the maximum # of message retransmissions.
	int Max_rets = 5;
	c->MaxMessageRetransmissions = Max_rets;
	c->TunnelingEnabled          = 0;
	c->DynamicTunnelingEnabled   = 0;
	c->RadiusClientConfigFile    = "";
	c->RadiusPassword            = "";

	//set default pmip mode
	c->pmip_mode_type = IP_FAMILY_V6_V01;
	c->pmipv4_work_mode_type = 0;
	c->mobile_node_identifier_type = MOBILE_NODE_IDENTIFIER_TYPE_MAC;
	c->is_dnsmasq_started_on_pba = 0;
    c->pmip_tunnel_mode = IP_FAMILY_V6_V01;

}


int conf_parse(struct mip6_config *c, int argc, char **argv)
{
	char cfile[MAXPATHLEN];
	int ret;

	/* set config defaults */
	conf_default(c);

	if ((ret = conf_alt_file(cfile, argc, argv)) != 0) {
		if (ret == -EINVAL) {
			fprintf(stderr,
					"%s: option requires an argument -- c\n",
					argv[0]);
			conf_usage(basename(argv[0]));
			return -1;
		} else if (ret == -ENAMETOOLONG) {
			fprintf(stderr,
					"%s: argument too long -- c <file>\n",
					argv[0]);
			return -1;
		}
		strcpy(cfile, DEFAULT_CONFIG_FILE);
	}

	if (conf_file(c, cfile) < 0 && ret == 0)
		return -1;

	if (conf_cmdline(c, argc, argv) < 0)
		return -1;

	return 0;
}

#define CONF_BOOL_STR(x) ((x) ? "enabled" : "disabled")

void conf_show(struct mip6_config *c)
{
	/* Common options */
	dbg("config_file = %s\n",     c->config_file);
#ifdef ENABLE_VT
	dbg("vt_hostname = %s\n",     c->vt_hostname);
	dbg("vt_service = %s\n",      c->vt_service);
#endif
	dbg("mip6_entity = %u\n",     c->mip6_entity);
	dbg("debug_level = %u\n",     c->debug_level);
	dbg("debug_log_file = %s\n", (c->debug_log_file ? c->debug_log_file :"stderr"));
	if (c->pmgr.so_path)
		dbg("PolicyModulePath = %s\n",    c->pmgr.so_path);
	dbg("DefaultBindingAclPolicy = %u\n", c->DefaultBindingAclPolicy);
	dbg("NonVolatileBindingCache = %s\n", CONF_BOOL_STR(c->NonVolatileBindingCache));

	/* IPsec options */
	dbg("KeyMngMobCapability = %s\n",     CONF_BOOL_STR(c->KeyMngMobCapability));
	dbg("UseMnHaIPsec = %s\n",            CONF_BOOL_STR(c->UseMnHaIPsec));

	/* MN options */
	dbg("MnMaxHaBindingLife = %u\n",            c->MnMaxHaBindingLife);
	dbg("MnMaxCnBindingLife = %u\n",            c->MnMaxCnBindingLife);
	dbg("MnRouterProbes = %u\n",                c->MnRouterProbes);
	dbg("MnRouterProbeTimeout = %f\n",          tstodsec(c->MnRouterProbeTimeout_ts));
	dbg("InitialBindackTimeoutFirstReg = %f\n", tstodsec(c->InitialBindackTimeoutFirstReg_ts));
	dbg("InitialBindackTimeoutReReg = %f\n",	tstodsec(c->InitialBindackTimeoutReReg_ts));
	if (c->MoveModulePath)
		dbg("MoveModulePath = %s\n",   c->MoveModulePath);
	dbg("UseCnBuAck = %s\n",           CONF_BOOL_STR(c->CnBuAck));
	dbg("DoRouteOptimizationMN = %s\n",CONF_BOOL_STR(c->DoRouteOptimizationMN));
	dbg("MnUseAllInterfaces = %s\n",   CONF_BOOL_STR(c->MnUseAllInterfaces));
	dbg("MnDiscardHaParamProb = %s\n", CONF_BOOL_STR(c->MnDiscardHaParamProb));
	dbg("SendMobPfxSols = %s\n",       CONF_BOOL_STR(c->SendMobPfxSols));
	dbg("OptimisticHandoff = %s\n",    CONF_BOOL_STR(c->OptimisticHandoff));

	/* HA options */
	dbg("SendMobPfxAdvs = %s\n",       CONF_BOOL_STR(c->SendMobPfxAdvs));
	dbg("SendUnsolMobPfxAdvs = %s\n",  CONF_BOOL_STR(c->SendUnsolMobPfxAdvs));
	dbg("MaxMobPfxAdvInterval = %u\n", c->MaxMobPfxAdvInterval);
	dbg("MinMobPfxAdvInterval = %u\n", c->MinMobPfxAdvInterval);
	dbg("HaMaxBindingLife = %u\n",     c->HaMaxBindingLife);

	/* CN options */
	dbg("DoRouteOptimizationCN = %s\n",CONF_BOOL_STR(c->DoRouteOptimizationCN));

	/* PMIP options */
	dbg("RFC5213TimestampBasedApproachInUse                = %s\n",CONF_BOOL_STR(c->RFC5213TimestampBasedApproachInUse));
	dbg("RFC5213MobileNodeGeneratedTimestampInUse          = %s\n",CONF_BOOL_STR(c->RFC5213MobileNodeGeneratedTimestampInUse));
	dbg("RFC5213FixedMAGLinkLocalAddressOnAllAccessLinks   = %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&c->RFC5213FixedMAGLinkLocalAddressOnAllAccessLinks));
	dbg("RFC5213FixedMAGLinkLayerAddressOnAllAccessLinks   = %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&c->RFC5213FixedMAGLinkLayerAddressOnAllAccessLinks));

	/* PMIP LMA options */
	dbg("RFC5213MinDelayBeforeBCEDelete                    = %u.%9u seconds\n",c->RFC5213MinDelayBeforeBCEDelete.tv_sec,c->RFC5213MinDelayBeforeBCEDelete.tv_nsec);
	dbg("RFC5213MaxDelayBeforeNewBCEAssign                 = %u.%9u seconds\n",c->RFC5213MaxDelayBeforeNewBCEAssign.tv_sec,c->RFC5213MaxDelayBeforeNewBCEAssign.tv_nsec);
	dbg("RFC5213TimestampValidityWindow                    = %u.%9u seconds\n",c->RFC5213TimestampValidityWindow.tv_sec,c->RFC5213TimestampValidityWindow.tv_nsec);

	/* PMIP MAG options */
	dbg("RFC5213EnableMAGLocalRouting = %s\n", CONF_BOOL_STR(c->RFC5213EnableMAGLocalRouting));

	dbg("AllLmaMulticastAddress            = %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&c->AllLmaMulticastAddress));
	dbg("Pmip MAx Log file size in MB      = %d\n", c->PmipMaxLogFileSize);
	dbg("LmaAddress                        = %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&c->LmaAddress));
	dbg("LmaAddress v4 addr                       = %s\n", inet_ntoa(c->LmaV4Address));
	dbg("Pmipv4 DMNP prefix                       = %s\n", inet_ntoa(c->PMIPV4SecRouterDMNPPrefix));
	dbg("Pmipv4 DMNP prefix len                       = %d\n", c->PMIPV4SecRouterdmnpPrefixLen);
	dbg("Primary v4 DNS Server                       = %s\n", inet_ntoa(c->PrimaryV4Dns));
	dbg("Secondary v4 DNS Server                       = %s\n", inet_ntoa(c->SecondaryV4Dns));
	dbg("Primary v6 DNS Server                       = %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&c->PrimaryV6Dns));
	dbg("Primary v6 DNS Server                       = %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&c->SecondaryV6Dns));
	if (is_mag()) {
		dbg("MagAddressIngress                 = %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&c->MagAddressIngress));
		dbg("MagAddressEgress                  = %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&c->MagAddressEgress));
		dbg("Magv4AddressIngress                 = %s\n", inet_ntoa(c->Magv4AddressIngress));
		dbg("Magv4AddressEgress                  = %s\n", inet_ntoa(c->Magv4AddressEgress));
		dbg("MagDeviceIngress                  = %s\n", (c->MagDeviceIngress ? c->MagDeviceIngress : "No device"));
		dbg("MagDeviceEgress                   = %s\n", (c->MagDeviceEgress ? c->MagDeviceEgress : "No device"));
		dbg("ServiceSelectionIdentifier        = %s\n", c->ServiceSelectionIdentifier);
		dbg("MnIdString                 = %s\n", c->MnIdString);
		dbg("PBULifeTime                       = %u.%9u seconds\n",c->PBULifeTime.tv_sec,c->PBULifeTime.tv_nsec);
		dbg("RetransmissionTimeOut             = %u.%9u seconds\n",c->RetransmissionTimeOut.tv_sec,c->RetransmissionTimeOut.tv_nsec);
		dbg("RadiusClientConfigFile            = %s\n", (c->RadiusClientConfigFile ? c->RadiusClientConfigFile : "No Config file"));
		dbg("RadiusPassword                    = %s\n", (c->RadiusPassword ? c->RadiusPassword : "No password"));
		dbg("PcapSyslogAssociationGrepString   = %s\n", (c->PcapSyslogAssociationGrepString ? c->PcapSyslogAssociationGrepString : "No syslog association grep string"));
		dbg("PcapSyslogDeAssociationGrepString = %s\n", (c->PcapSyslogDeAssociationGrepString ? c->PcapSyslogDeAssociationGrepString : "No syslog de-association grep string"));
		dbg("mobile_node_identifier_type                 = %d\n", c->mobile_node_identifier_type);
	}
	dbg("OurAddress                        = %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&c->OurAddress));
	dbg("HomeNetworkPrefix                 = %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&c->HomeNetworkPrefix));

	dbg("MaxMessageRetransmissions         = %u\n", c->MaxMessageRetransmissions);
	dbg("TunnelingEnabled                  = %s\n", CONF_BOOL_STR(c->TunnelingEnabled));
	dbg("DynamicTunnelingEnabled           = %s\n", CONF_BOOL_STR(c->DynamicTunnelingEnabled));

}
