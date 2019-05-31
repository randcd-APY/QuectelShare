/*! \file pmip_fsm.c
 * \brief
 * \author OpenAir3 Group
 * \date 12th of October 2010
 * \version 1.0
 * \company Eurecom
 * \project OpenAirInterface
 * \email: openair3@eurecom.fr
 */
#define PMIP
#define PMIP_FSM_C
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
//---------------------------------------------------------------------------------------------------------------------
#include "pmip_fsm.h"
#include "pmip_hnp_cache.h"
#include "pmip_lma_proc.h"
#include "pmip_mag_proc.h"
#include "pmip_pcap.h"
#include <stdint.h>
#include <string.h>
#include "pmipv4_conf.h"



//#include "pugixml/pugixml.hpp"

//---------------------------------------------------------------------------------------------------------------------
#ifdef ENABLE_VT
#    include "vt.h"
#endif
#include "debug.h"
#include "conf.h"

#define INET_ADDRSTRLEN        16
#define MAC_ADDR_NUM_CHARS 18

extern struct mip6_config conf;

//---------------------------------------------------------------------------------------------------------------------
int pending_pbu_seq_list_init(void)
{
	dbg(":pending_pbu_seq_list_init\n");
	//memset(&conf->pbu_list, 0, sizeof(conf->pbu_list));
	//INIT_LIST_HEAD(&conf->pbu_list.list);
	INIT_LIST_HEAD(&conf.pbu_list);
	return 1;
}


//---------------------------------------------------------------------------------------------------------------------
int mag_init_fsm(void)
{
	if (conf.pmip_mode_type == IP_FAMILY_V6_V01 || conf.pmip_mode_type == IP_FAMILY_V4V6_V01) {
		if (pthread_rwlock_init(&fsm_lock, NULL))
			return -1;
		else
			return 0;
	}
	if (conf.pmip_mode_type == IP_FAMILY_V4_V01 || conf.pmip_mode_type == IP_FAMILY_V4V6_V01) {
		if (pthread_rwlock_init(&fsm_v4_lock, NULL))
			return -1;
		else
			return 0;
	}
}
//---------------------------------------------------------------------------------------------------------------------
int mag_fsm(msg_info_t * info)
{
	int result = 0;
	int aaa_result = 0;
	pmip_entry_t *bce;
	struct in6_addr prefix;
	struct in6_addr hw_address = info->mn_hw_address;
	int type = pmip_cache_exists(&conf.OurAddress, &hw_address);
	int mutex_return_code;
	uint8_t       mac_addr[PMIP_MAC_ADDR_LEN];


	mutex_return_code = pthread_rwlock_wrlock(&fsm_lock);
	if (mutex_return_code != 0) {
		dbg("pthread_rwlock_wrlock(&fsm_lock) %s\n", strerror(mutex_return_code));
	}
	switch (type) {
		//--------------------------------------
		case BCE_NO_ENTRY:
			dbg("BCE_NO_ENTRY\n");
			if (info->msg_event == hasRS) {
				dbg("New MN is found sending RS, start new registration ...\n\n");

				ipv6_to_mac(&hw_address, mac_addr);
				dbg("---fsm v6 mac to send: %02x:%02x:%02x:%02x:%02x:%02x\n",mac_addr[0],mac_addr[1],mac_addr[2],mac_addr[3],mac_addr[4],mac_addr[5]);
				if (pmip_send_qcmap_dev_query(info->iif, &hw_address, 0)){
					dbg("QCMAP Acknowledged Device. Add BCE and send PBU\n");
					bce = pmip_cache_alloc(BCE_TEMP);
					prefix = mnid_hnp_map(hw_address, &aaa_result);
					if (aaa_result >= 0) {
						bce->mn_prefix = prefix;
						bce->mn_suffix = info->mn_iid;
						bce->mn_hw_address = hw_address;
						info->mn_prefix = prefix;
						result = mag_pmip_md(info, bce, 0);
						dbg("Movement detection is finished, now going to add an entry into the cache\n\n");
						pmip_cache_add(bce);
						dbg("pmip_cache_add is done \n\n");
					} else {
						dbg("Authentication failed\n");
					}
				} else {
					dbg("QCMAP did not detect Device. Ignore RS\n");
				}
				//yet to process
			} else if (info->msg_event == hasWLCCP) {
				dbg("Incoming MN is detected by Wireless Access Point, start new registration ...\n\n");
				bce = pmip_cache_alloc(BCE_TEMP);
				prefix = mnid_hnp_map(hw_address, &aaa_result);
				if (aaa_result >= 0) {
					bce->mn_prefix = prefix;
					bce->mn_suffix = info->mn_iid;
					bce->mn_hw_address = hw_address;
					info->mn_prefix = prefix;
					result = mag_pmip_md(info, bce, 0);
					dbg("Movement detection is finished, now going to add an entry into the cache\n\n");
					pmip_cache_add(bce);
					dbg("pmip_cache_add is done \n\n");
				} else {
					dbg("Authentication failed\n");
				}
				//yet to process
			} else if (info->msg_event == hasDEREG) {
				dbg("Received DEREG message\n");
				dbg("No action for this event (%d) at current state (%d) !\n", info->msg_event, type);
			}
			break;
			//--------------------------------------
		case BCE_TEMP:
			dbg("BCE_TEMP\n");
			if (info->msg_event == hasPBA) {
				bce = pmip_cache_get(&conf.OurAddress, &hw_address);
				if (is_pba_is_response_to_valid_pbu_seq_number(info, bce)) {
					dbg("Handling PBA. Moving from BCE_TEMP to BCE_PMIP\n");
					dbg("Finish Location Registration\n");
					//Modify the entry with additional info.
					del_task(&bce->tqe);    //Delete timer retransmission PBU (if any)
					bce->PBA_flags = info->PBA_flags;
					bce->mn_prefix = info->mn_prefix;
					bce->gre_key_valid = info->gre_key_valid;
					if(info->gre_key_valid)
						bce->gre_key = info->gre_key;
					// trick not to let LMA break the tunnel a few milli-seconds before MAG send a new PBU -> loss of traffic
					if (info->lifetime.tv_sec > 10) {
						bce->lifetime.tv_sec = info->lifetime.tv_sec - 8;
						dbg("Set BCE lifetime to PBA lifetime (%d seconds) - 8 second = %d seconds\n", info->lifetime.tv_sec, bce->lifetime.tv_sec);
					} else if (info->lifetime.tv_sec > 5) {
						bce->lifetime.tv_sec = info->lifetime.tv_sec - 3;
						dbg("Set BCE lifetime to PBA lifetime (%d seconds) - 3 second = %d seconds\n", info->lifetime.tv_sec, bce->lifetime.tv_sec);
					} else if (info->lifetime.tv_sec > 1) {
						bce->lifetime.tv_sec = info->lifetime.tv_sec - 1;
						dbg("Set BCE lifetime to PBA lifetime (%d seconds) - 1 second = %d seconds\n", info->lifetime.tv_sec, bce->lifetime.tv_sec);
					} else {
						bce->lifetime.tv_sec = info->lifetime.tv_sec;
						dbg("Set BCE lifetime to PBA lifetime (%d seconds) =  %d seconds\n", info->lifetime.tv_sec, bce->lifetime.tv_sec);
					}
					dbg("Prefix before ending registration : %x:%x:%x:%x:%x:%x:%x:%x \n", NIP6ADDR(&bce->mn_prefix));
					// LG COMMENT GOT PREFIX BY RADIUS - bce->mn_prefix = info->mn_prefix;   //adding the hn prefix value receive in PBA to MAG cache
					mag_end_registration(bce, info->iif, 0);
				}
				pmipcache_release_entry(bce);
			}
			break;
			//--------------------------------------
		case BCE_PMIP:
			dbg("BCE_PMIP\n");
			if (info->msg_event == hasRS) {
				dbg("Router solicitation received for existing MN\n");
				bce = pmip_cache_get(&conf.OurAddress, &hw_address);
				dbg("prefix before entering kickoff_ra : %x:%x:%x:%x:%x:%x:%x:%x \n", NIP6ADDR(&bce->mn_prefix));
				mag_kickoff_ra(bce, 0);
				pmipcache_release_entry(bce);
				dbg("RA sent after router solicitation ...\n");
			} else if (info->msg_event == hasPBA) {
				bce = pmip_cache_get(&conf.OurAddress, &hw_address);
				if (is_pba_is_response_to_valid_pbu_seq_number(info, bce)) {
					dbg("Finish Location Registration\n");
					//Modify the entry with additional info.
					del_task(&bce->tqe);    //Delete timer retransmission PBU (if any)
					bce->PBA_flags = info->PBA_flags;
					// trick not to let LMA break the tunnel a few milli-seconds before MAG send a new PBU -> loss of traffic
					bce->lifetime.tv_nsec = 0;
					if (info->lifetime.tv_sec > 10) {
						bce->lifetime.tv_sec = info->lifetime.tv_sec - 8;
						dbg("Set BCE lifetime to PBA lifetime (%d seconds) - 8 second = %d seconds\n", info->lifetime.tv_sec, bce->lifetime.tv_sec);
					} else if (info->lifetime.tv_sec > 5) {
						bce->lifetime.tv_sec = info->lifetime.tv_sec - 3;
						dbg("Set BCE lifetime to PBA lifetime (%d seconds) - 3 second = %d seconds\n", info->lifetime.tv_sec, bce->lifetime.tv_sec);
					} else if (info->lifetime.tv_sec > 1) {
						bce->lifetime.tv_sec = info->lifetime.tv_sec - 1;
						dbg("Set BCE lifetime to PBA lifetime (%d seconds) - 1 second = %d seconds\n", info->lifetime.tv_sec, bce->lifetime.tv_sec);
					} else {
						bce->lifetime.tv_sec = info->lifetime.tv_sec;
						dbg("Set BCE lifetime to PBA lifetime (%d seconds) =  %d seconds\n", info->lifetime.tv_sec, bce->lifetime.tv_sec);
					}
					dbg("Prefix before ending registration : %x:%x:%x:%x:%x:%x:%x:%x \n", NIP6ADDR(&bce->mn_prefix));
					// LG COMMENT GOT PREFIX BY RADIUS - bce->mn_prefix = info->mn_prefix;   //adding the hn prefix value receive in PBA to MAG cache
					mag_end_registration_no_new_tunnel(bce, info->iif, 0);
				}
				pmipcache_release_entry(bce);
			} else if (info->msg_event == hasWLCCP) {
				dbg("Incomming MN is detected by Wireless Access Point, existing MN\n");
				bce = pmip_cache_get(&conf.OurAddress, &hw_address);
				dbg("Prefix before entering kickoff_ra : %x:%x:%x:%x:%x:%x:%x:%x \n", NIP6ADDR(&bce->mn_prefix));
				mag_kickoff_ra(bce, 0);
				// Some case where the access point did not detect the departure of the mobile node
				// so we have to register again to the LMA
				mag_force_update_registration(bce, info->iif, 0);
				pmipcache_release_entry(bce);
				dbg("RA sent after MN AP detection ...\n");
			} else if (info->msg_event == hasDEREG) {
				dbg("Deregistration procedure detected by Wireless Access Point for a registered MN\n");
				dbg("Start Location Deregistration\n");
				bce = pmip_cache_get(&conf.OurAddress, &hw_address);
				mag_dereg(bce, 1, 0);
				pmipcache_release_entry(bce);
				pmip_bce_delete(bce);
			} else if (info->msg_event == hasNA) {
				//Reset counter, Delete task for entry deletion  & Add a new task for NS expiry.
				bce = pmip_cache_get(&conf.OurAddress, &hw_address);
				bce->n_rets_counter = conf.MaxMessageRetransmissions;    //Reset the Retransmissions Counter.
				dbg("Reset the Reachability Counter = %d for %x:%x:%x:%x:%x:%x:%x:%x\n", bce->n_rets_counter, NIP6ADDR(&info->mn_iid));
				del_task(&bce->tqe);
				mag_force_update_registration(bce, info->iif, 0);
				//pmip_cache_start(bce);
				pmipcache_release_entry(bce);
			}
			break;
		default:
			dbg("No action for this event (%d) at current state (%d) !\n", info->msg_event, type);
	}
	mutex_return_code = pthread_rwlock_unlock(&fsm_lock);
	if (mutex_return_code != 0) {
		dbg("pthread_rwlock_unlock(&fsm_lock) %s\n", strerror(mutex_return_code));
	}
	return result;
}

int getipforclient(struct in_addr *addr)
{
	uint32_t	 sub_net_mask;
	uint32_t	 dhcp_start_address = conf.dhcp_start_ip.s_addr;
	uint32_t	 dhcp_end_address = conf.dhcp_end_ip.s_addr;
	char *ip;
	struct in_addr temp;
	temp.s_addr = conf.current_ip;
	dbg ("current avail ip address start %s\n",inet_ntoa(temp));

	if (conf.current_ip < conf.dhcp_start_ip.s_addr)
	{
		dbg("Ip range exhausted no ip available\n");
		//current_ip = dhcp_end_address;
		return 0;
	}
	addr->s_addr = conf.current_ip;
	ip = inet_ntoa(*addr);
	while (isrervationexits(PMIP_DHCP_RESER_FILE,ip) !=0 ){
		dbg("ip %s exits in %s picking next ip\n",ip, PMIP_DHCP_RESER_FILE);
		conf.current_ip = htonl(ntohl(conf.current_ip) - 1);
		if (conf.current_ip < conf.dhcp_start_ip.s_addr) {
			dbg("Ip range exhausted no ip available\n");
			return 0;
		}
		addr->s_addr = conf.current_ip;
		ip = inet_ntoa(*addr);
	}
	temp.s_addr=conf.current_ip;
	dbg ("Going to reserve %s ip for connection\n",ip);
	dbg("the new available current ip %s\n",inet_ntoa(temp));

	return 1;
}

int isrervationexits(char *filename ,char *str)
{
	FILE *file = fopen ( filename, "r" );
	char command[MAX_DHCP_COMMAND_STR_LEN] = {0};
	dbg("in isrervationexits file=%s, check str=%s=\n",filename,str);
	if ( file != NULL )
	{
		char line [ 128 ];
		while ( fgets ( line, sizeof line, file ) != NULL ) /* read a line */
		{
			if (strstr(line, str))
			{
				dbg("entry '%s' matching passed return match success\n",str);
				snprintf (command, MAX_DHCP_COMMAND_STR_LEN, "echo 'pmip: entry exits for ip %s' /dev/kmsg",str);
				pmip_system_call(command,strlen(command),1);
				fclose ( file );
				return 1;
			}
		}
		fclose ( file );
	}
	else
	{
		dbg ("open failed for file %s\n" ,filename );
	}
	dbg("match does not exits\n");
	return 0;
}

#if 0 //May be use in future if default router address Need to be added on bridge
in_addr_t  prefix_to_mask(int prefix)
{
	if ( prefix == 0 )
		return htonl( ~((in_addr_t) -1) );
	else
		return htonl( ~((1 << (32 - prefix)) - 1) );

}

struct in_addr get_same_network_ip(struct in_addr ip, struct in_addr sub_mask)
{
	struct in_addr network_ip ;
	network_ip.s_addr = ip.s_addr & sub_mask.s_addr;
	return network_ip;
}
#endif

int is_requested_ip_in_same_range(struct in_addr request_ip)
{
	dbg ("Ip address to check=%s\n",inet_ntoa(request_ip));
	dbg ("Lan start ip=%s\n",inet_ntoa(conf.dhcp_start_ip));
	dbg ("Lan end ip=%s\n",inet_ntoa(conf.dhcp_end_ip));
	//first check if requested Ip is in same network
	if ((request_ip.s_addr & conf.dhcp_sub_mask.s_addr) == (conf.dhcp_start_ip.s_addr & conf.dhcp_sub_mask.s_addr))
	{
		dbg ("requested Ip is in same network . Now will check if in range\n");
	} else {
		dbg ("requested Ip is Not in same network . Dont Send PBU\n");
		return 0;
	}
	if (((request_ip.s_addr) >= (conf.dhcp_start_ip.s_addr)) && ((request_ip.s_addr) <= (conf.dhcp_end_ip.s_addr))) {
		dbg("Requested Ip is in Lan range provided during pmip run .\n");
		return 1;
	} else {
		dbg("Requested Ip is not Lan range provided during pmip run. Dont Send PBU\n");
		return 0;
	}
	return 0;
}


//---------------------------------------------------------------------------------------------------------------------
int mag_fsm_v4(msg_info_t * info)
{
	dbg("Enter mag_fsm_v4\n");

	int result = 0;
	pmip_entry_t *bce;
	struct in_addr prefix;
	struct in6_addr hw_address = info->mn_hw_address;
	struct in_addr mask;
	struct in_addr netwok_ip;
	char ip_addr[INET_ADDRSTRLEN];
	int mutex_return_code;
	char home_prefix[INET_ADDRSTRLEN];

	uint8_t mac[PMIP_MAC_ADDR_LEN];

	char macStr[MAC_ADDR_NUM_CHARS];

	memset (&prefix,0,sizeof (struct in_addr));
	char command[MAX_DHCP_COMMAND_STR_LEN] = {0};
	char buff[200]= {0};
	char temp[100]={0};
	int isprimdnsvalid = 0;
	int issecdnsvalid = 0;

	int type = pmipv4_cache_exists(&conf.OurAddress, &hw_address);

	dbg("mag_fsm_v4 our address: %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(&conf.OurAddress));
	dbg("pmipv4_cache_exists mag_fsm_v4 get type =%d \n",type);
	dbg("mag v4 fsm print conf work mode = %d \n",conf.pmipv4_work_mode_type);

	dbg("mag_fsm_v4 MN id : %x:%x:%x:%x:%x:%x:%x:%x \n", NIP6ADDR(&info->mn_iid));

	mutex_return_code = pthread_rwlock_wrlock(&fsm_v4_lock);
	if (mutex_return_code != 0) {
		dbg("pthread_rwlock_wrlock(&fsm_v4_lock) %s\n", strerror(mutex_return_code));
	}
	switch (type) {
		//--------------------------------------
		case BCE_NO_ENTRY:
			dbg("BCE_NO_ENTRY\n");
			if (info->msg_event == hasDHCPDIS) {
				if (pmip_send_qcmap_dev_query(info->iif, &hw_address, 1)) {
					dbg("QCMAP Acknowledged Device. Add BCE and send PBU for ipv4\n");
					dbg("New MN is found dhcp discover, start new registration \n");
					//convert  mac to string
					ipv6_to_mac(&info->mn_hw_address, mac);
					dbg("registartion mac : %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
					mac_addr_ntop(mac, (char *)&macStr);

					bce = pmipv4_cache_alloc(BCE_TEMP);
					bce->mn_suffix = info->mn_iid;
					bce->mn_hw_address = hw_address;
					result = mag_pmip_md(info, bce, 1);
					dbg("Movement detection is finished, now going to add an entry into the cache for v4\n\n");
					pmipv4_cache_add(bce);
					dbg("pmip_cache_add is done \n\n");


				}
				else
				{
					dbg("QCMAP did not detect Device. Ignore DHCP DISCOVER\n");
				}
			}
			else if (info->msg_event == hasDHCPREQ) {
				if (pmip_send_qcmap_dev_query(info->iif, &hw_address, 1)) {
					dbg("BCE_NO_ENTRY New MN dhcp Request ...\n");
					dbg ("BCE_NO_ENTRY client requested ip =%s\n",inet_ntoa(info->dhcp_request_ip));
					if (!is_requested_ip_in_same_range(info->dhcp_request_ip)) {
						snprintf (command, MAX_DHCP_COMMAND_STR_LEN, "echo 'pmip: TBCE_NO_ENTRY dhcp request Will be discarded as request Ip is outside range' /dev/kmsg ");
						pmip_system_call(command,strlen(command),1);
						dbg("BCE_NO_ENTRY dhcp request Will be discarded as request Ip is outside range\n");
						break;
					}
					if (isrervationexits(PMIP_DHCP_RESER_FILE,inet_ntoa(info->dhcp_request_ip))) {
						snprintf (command, MAX_DHCP_COMMAND_STR_LEN, "echo 'pmip: The requested Ip %s is "
									" already reserved for some client discard dhcp request' /dev/kmsg",inet_ntoa(info->dhcp_request_ip));
						pmip_system_call(command,strlen(command),1);
						dbg ("The requested Ip %s is already reserved for some client discard dhcp request\n",inet_ntoa(info->dhcp_request_ip));
						break;
					}
					dbg("All checking Ok .Ready to send PBU\n");
					bce = pmipv4_cache_alloc(BCE_TEMP);
					bce->mn_suffix = info->mn_iid;
					bce->mn_hw_address = hw_address;
					bce->dhcp_requested_ip = info->dhcp_request_ip;
					result = mag_pmip_md(info, bce, 1);
					dbg("DHCP REQUEST Movement detection is finished, now going to add an entry into the cache for v4\n\n");
					pmipv4_cache_add(bce);
				} else {
					dbg("QCMAP did not detect Device. Ignore DHCP REQUEST\n");
				}
			}
			else if (info->msg_event == hasDEREG) {
				dbg("Received DEREG message\n");
				dbg("No action for this event (%d) at current state (%d) !\n", info->msg_event, type);
			}
			break;
			//--------------------------------------
		case BCE_TEMP:
			dbg("BCE_TEMP\n");
			memset(command,0,sizeof(command));
			memset(buff,0,sizeof(buff));
			if (info->msg_event == hasDHCPDIS) {
				dbg("Received DHCP discover message\n");
				dbg("No action for this event (%d) at current state (%d) !\n", info->msg_event, type);
			}
			if (info->msg_event == hasPBA) {
				dbg("Recieved PBA from LMA for ipv4 after sending PBU\n");
				bce = pmipv4_cache_get(&conf.OurAddress, &hw_address);
				if (is_pba_is_response_to_valid_pbu_seq_number(info, bce)) {
					//Checking for DMNP prefix verification
					//Skiping for now
					#if 0
					if (conf.pmipv4_work_mode_type == PMIPV4_MODE_SECONDARY_ROUTER_V01) {
						if ((conf.is_dmnp_prefix_verified !=1 ) && info->dmnp_pba_status !=0) {
							dbg("DMNP prefix rejected by LMA . Send PBU again\n");
							mag_start_registration(bce);
							break;
						} else {
							dbg("DMNP prefix Accepted by LMA\n");
							conf.is_dmnp_prefix_verified =1;
						}
					} else {
						dbg ("CPE Mode DMNP prefix validation not required\n");
					}
					#endif
					dbg("Handling PBA. Moving from BCE_TEMP to BCE_PMIP for ipv4\n");
					dbg("Finish Location Registration\n");
					//Modify the entry with additional info.
					del_task(&bce->tqe);    //Delete timer retransmission PBU (if any)
					bce->PBA_flags = info->PBA_flags;
					bce->mn_prefix = info->mn_prefix;
					bce->gre_key_valid = info->gre_key_valid;
					if(info->gre_key_valid)
						bce->gre_key = info->gre_key;
					//staring dnsmasq
					if (!conf.is_dnsmasq_started_on_pba) {
						dbg("starting dnsmasq As it is first PBA received\n");
						if (info->pmipv4_hnp_reply_valid){
							dbg("BCE_TEMP Got pmipv4_hnp_reply_valid \n");
							if (info->v4_home_addr_reply_status != 0 ) {
								dbg("BCE_TEMP Invalid status code =%d for reply \n",info->v4_home_addr_reply_status);
								dbg("sending PBU again with different ip request\n");
								mag_start_registration(bce, 1);
								break;
							} else {
								memcpy(home_prefix, inet_ntoa(info->v4_home_prefix), INET_ADDRSTRLEN);
								dbg("BCE_TEMP Got ip in home prefix reply =%s prefix len=%d\n",home_prefix,info->v4_home_prefixlen);
								//clear DHCP reservation file
								snprintf (command,MAX_DHCP_COMMAND_STR_LEN,"echo > %s",PMIP_DHCP_RESER_FILE);
								pmip_system_call(command,strlen(command),1);

								snprintf (command,MAX_DHCP_COMMAND_STR_LEN ,"dnsmasq -i %s -I lo -z --dhcp-range=%s,%s,static,%s,43200 --dhcp-hostsfile=%s ",BRIDGE_IFACE,BRIDGE_IFACE,home_prefix,inet_ntoa(conf.dhcp_sub_mask), PMIP_DHCP_RESER_FILE);
								if (conf.PrimaryV4Dns.s_addr != 0)
								{
									isprimdnsvalid = 1;
									dbg("primary ipv4 dns is configured\n");
									snprintf(buff,200, " --dhcp-option-force=6,%s",inet_ntoa(conf.PrimaryV4Dns) );
								}
								if (conf.SecondaryV4Dns.s_addr != 0)
								{
									issecdnsvalid= 1;
									if (isprimdnsvalid) //have primary & secondary dns
									{
										dbg("Secondary ipv4 dns is also configured\n");
										snprintf(temp,200, ",%s ",inet_ntoa(conf.SecondaryV4Dns) );
										strcat(buff,temp);
									}
									else //only secondary dns
									{
										dbg("Only Secondary ipv4 dns is configured\n");
										snprintf(buff,200, " --dhcp-option-force=6,%s  ",inet_ntoa(conf.SecondaryV4Dns) );
									}
								}
								if (!isprimdnsvalid && !issecdnsvalid)
								{
									dbg("Primary and secondary dns are not configured.dns option will not be added\n");
								}
								else
								{
									dbg("adding dnsmasq dns option\n");
								strcat(command,buff);
								}
								if (conf.pmip_mode_type == IP_FAMILY_V4V6_V01)
								{
									pmip_add_dnsmasq_v6dns_option(command);
								}
								system("killall -1 dnsmasq");
								system("killall -15 dnsmasq");
								system("killall -9 dnsmasq");
								pmip_system_call(command,strlen(command),1);
								conf.is_dnsmasq_started_on_pba = 1;
								#if 0 //commenting this Code sine currenly DMNP prefix not being used
								if (info->hasipv4_default_router_option) {
									dbg("BCE_TEMP Got hasipv4_default_router_option router %s \n",inet_ntoa(info->default_router));
									snprintf(buff,200, " --dhcp-option-force=6,%s  ",inet_ntoa(conf.PrimaryDns) );
									strcat(command,buff);
									dbg("dnamsq command=%s= for starting dnsmasq\n",command);
									system("killall -1 dnsmasq");
									system("killall -15 dnsmasq");
									system("killall -9 dnsmasq");
									system (command);

									//add same network Ip to bridge to all dhcp range to be working
									netwok_ip= get_same_network_ip(conf.dhcp_start_ip, conf.dhcp_sub_mask);
									snprintf (command,200,"ip a a %s/%d dev %s",inet_ntoa(netwok_ip),info->v4_home_prefixlen,BRIDGE_IFACE);
									ds_system_call(command,strlen(command));
									dbg("IP addr command=%s=\n",command);
									//delete route
									snprintf (command,200,"ip r d %s/%d dev %s src %s",inet_ntoa(netwok_ip),info->v4_home_prefixlen,BRIDGE_IFACE,inet_ntoa(netwok_ip));
									system (command);
									dbg("IP route del command=%s=\n",command);
									conf.is_dnsmasq_started_on_pba = 1;
									//adding Route
									snprintf (command,200 ,"ip r a %s/%d dev %s",inet_ntoa(netwok_ip),info->v4_home_prefixlen,BRIDGE_IFACE);
									system(command);
									dbg("adding Route command =%s=\n",command);
									snprintf(command, 100, "sed -i 's/Magv4AddressIngress.*\;/Magv4AddressIngress        %s\;/g' %s",inet_ntoa(netwok_ip),conf.config_file);
									system(command);
									conf.Magv4AddressIngress = netwok_ip;
								}
								#endif
							}

						}

					}
					else {
						dbg("BCE_TEMP dnsmasq already started\n");
					}

					if (conf.is_dnsmasq_started_on_pba != 1) {
						dbg(" BCE_TEMP dnsmasq havent started . ignore the PBA . break");
						break;
					}

					if (info->pmipv4_hnp_reply_valid) {
						dbg("BCE_TEMP Got pmipv4_hnp_reply_valid \n\n");
						if (info->v4_home_addr_reply_status != 0 ) {
							dbg("BCE_TEMP Invalid status code for reply \n\n");
							dbg("sending PBU again with different ip request");
							mag_start_registration(bce, 1);
							break;
						}
						else {
							dbg("BCE_TEMP Got ip in home prefix reply =%s prefix len=%d\n",inet_ntoa(info->v4_home_prefix),info->v4_home_prefixlen);
							//convert  mac to string
							ipv6_to_mac(&info->mn_hw_address, mac);
							dbg("BCE_TEMP mac to registration : %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
							mac_addr_ntop(mac, (char *)&macStr);

							dbg("Saving MN v4 address in cache\n");
							bce->mnv4_addr = info->v4_home_prefix;
							dbg("Saving MN v4 ip so that while Sending next PBU this can be used\n");
							bce->dhcp_requested_ip = bce->mnv4_addr;
							snprintf(command, MAX_DHCP_COMMAND_STR_LEN,"echo %s,%s >> %s",macStr,inet_ntoa(info->v4_home_prefix),PMIP_DHCP_RESER_FILE);
							pmip_system_call(command,strlen(command),1);
							//sending signal to dnsmasq to re-read hosts file
							system("killall -1 dnsmasq");
						}
					}
					if (info->hasipv4_default_router_option) {
						dbg("BCE_TEMP Got hasipv4_default_router_option router %s \n",inet_ntoa(info->default_router));

					}
					// trick not to let LMA break the tunnel a few milli-seconds before MAG send a new PBU -> loss of traffic
					if (info->lifetime.tv_sec > 10) {
						bce->lifetime.tv_sec = info->lifetime.tv_sec - 8;
						dbg("Set BCE lifetime to PBA lifetime (%d seconds) - 8 second = %d seconds\n", info->lifetime.tv_sec, bce->lifetime.tv_sec);
					} else if (info->lifetime.tv_sec > 5) {
						bce->lifetime.tv_sec = info->lifetime.tv_sec - 3;
						dbg("Set BCE lifetime to PBA lifetime (%d seconds) - 3 second = %d seconds\n", info->lifetime.tv_sec, bce->lifetime.tv_sec);
					} else if (info->lifetime.tv_sec > 1) {
						bce->lifetime.tv_sec = info->lifetime.tv_sec - 1;
						dbg("Set BCE lifetime to PBA lifetime (%d seconds) - 1 second = %d seconds\n", info->lifetime.tv_sec, bce->lifetime.tv_sec);
					} else {
						bce->lifetime.tv_sec = info->lifetime.tv_sec;
						dbg("Set BCE lifetime to PBA lifetime (%d seconds) =  %d seconds\n", info->lifetime.tv_sec, bce->lifetime.tv_sec);
					}
					//dbg("Prefix before ending registration : %x:%x:%x:%x:%x:%x:%x:%x \n", NIP6ADDR(&bce->mn_prefix));
					// LG COMMENT GOT PREFIX BY RADIUS - bce->mn_prefix = info->mn_prefix;   //adding the hn prefix value receive in PBA to MAG cache
					mag_end_registration(bce, info->iif, 1);
					mag_update_binding_entry_with_DMNP_Home_addr(bce, info);
				}
				pmipv4cache_release_entry(bce);
			}
			break;
			//--------------------------------------
		case BCE_PMIP:
			dbg("BCE_PMIP event %d\n",info->msg_event);
			if (info->msg_event == hasDHCPREQ) {
				dbg ("BCE_PMIP client requested ip =%s\n",inet_ntoa(info->dhcp_request_ip));
				dbg("DHCP request received for existing MN\n");
				dbg("No action for this event (%d) at current state (%d) !\n", info->msg_event, type);
			}
			else if (info->msg_event == hasPBA) {
				dbg("BCE_PMIP has PBA\n");

				bce = pmipv4_cache_get(&conf.OurAddress, &hw_address);
				if (is_pba_is_response_to_valid_pbu_seq_number(info, bce)) {
					dbg("Finish Location Registration\n");
					//Modify the entry with additional info.
					del_task(&bce->tqe);    //Delete timer retransmission PBU (if any)
					bce->PBA_flags = info->PBA_flags;
					// trick not to let LMA break the tunnel a few milli-seconds before MAG send a new PBU -> loss of traffic
					bce->lifetime.tv_nsec = 0;
					if (info->lifetime.tv_sec > 10) {
						bce->lifetime.tv_sec = info->lifetime.tv_sec - 8;
						dbg("Set BCE lifetime to PBA lifetime (%d seconds) - 8 second = %d seconds\n", info->lifetime.tv_sec, bce->lifetime.tv_sec);
					} else if (info->lifetime.tv_sec > 5) {
						bce->lifetime.tv_sec = info->lifetime.tv_sec - 3;
						dbg("Set BCE lifetime to PBA lifetime (%d seconds) - 3 second = %d seconds\n", info->lifetime.tv_sec, bce->lifetime.tv_sec);
					} else if (info->lifetime.tv_sec > 1) {
						bce->lifetime.tv_sec = info->lifetime.tv_sec - 1;
						dbg("Set BCE lifetime to PBA lifetime (%d seconds) - 1 second = %d seconds\n", info->lifetime.tv_sec, bce->lifetime.tv_sec);
					} else {
						bce->lifetime.tv_sec = info->lifetime.tv_sec;
						dbg("Set BCE lifetime to PBA lifetime (%d seconds) =  %d seconds\n", info->lifetime.tv_sec, bce->lifetime.tv_sec);
					}
					//dbg("Prefix before ending registration : %x:%x:%x:%x:%x:%x:%x:%x \n", NIP6ADDR(&bce->mn_prefix));
					// LG COMMENT GOT PREFIX BY RADIUS - bce->mn_prefix = info->mn_prefix;   //adding the hn prefix value receive in PBA to MAG cache
					mag_end_registration_no_new_tunnel(bce, info->iif, 1);
				}
				pmipv4cache_release_entry(bce);
			}

			else if (info->msg_event == hasDEREG) {
				dbg("Deregistration procedure detected by Wireless Access Point for a registered MN\n");
				dbg("Start Location Deregistration\n");

				bce = pmipv4_cache_get(&conf.OurAddress, &hw_address);
				mag_dereg(bce, 1, 1);
				pmipv4cache_release_entry(bce);
				pmipv4_bce_delete(bce);

			}  else if (info->msg_event == hasARPReply) {
				dbg("Has ARP reply\n");
				//Reset counter, Delete task for entry deletion  & Add a new task for ARP expiry.

				bce = pmipv4_cache_get(&conf.OurAddress, &hw_address);
				if (bce->Is_arp_sent == 1)
				{
					dbg("Got Arp reply requested By PMIP\n");
					bce->n_rets_counter = conf.MaxMessageRetransmissions;    //Reset the Retransmissions Counter.
					bce->Is_arp_sent = 0; //Reset arp flag
					dbg("Reset the Reachability Counter = %d for %s\n", bce->n_rets_counter, inet_ntoa(bce->mnv4_addr));
					del_task(&bce->tqe);
					mag_force_update_registration(bce, info->iif, 1);
					//pmip_cache_start(bce);
				}
				else
				{
					dbg("Ignore Arp reply Since It is not requested By PMIP\n");
				}
				pmipv4cache_release_entry(bce);
			}
			break;
		default:
			dbg("No action for this event (%d) at current state (%d) !\n", info->msg_event, type);
	}

	mutex_return_code = pthread_rwlock_unlock(&fsm_v4_lock);
	if (mutex_return_code != 0) {
		dbg("pthread_rwlock_unlock(&fsm_v4_lock) %s\n", strerror(mutex_return_code));
	}
	return result;
}


//---------------------------------------------------------------------------------------------------------------------
int lma_fsm(msg_info_t * info)
{
	struct in6_addr r_tmp1;
	int result = 0;
	pmip_entry_t *bce = NULL;
	struct in6_addr hw_address = info->mn_hw_address;
	int type = pmip_cache_exists(&conf.OurAddress, &hw_address);
	switch (type) {
		//--------------------------------------
		case BCE_NO_ENTRY:
			dbg("No PMIP entry found for %x:%x:%x:%x:%x:%x:%x:%x ... \n",
					NIP6ADDR(&info->mn_hw_address));
			if (info->msg_event == hasPBU && (info->lifetime.tv_sec > 0)) {
				//Create New Proxy Binding Entry storing information
				dbg("PBU for a new MN ... Location Registration starting now...\n");
				bce = pmip_cache_alloc(BCE_PMIP);
				if (bce != NULL) {
					lma_get_prefix(hw_address,&(info->mn_prefix));
					dbg("lma assigned  prefix %x:%x:%x:%x:%x:%x:%x:%x \n",NIP6ADDR(&info->mn_prefix));
					pmip_insert_into_hnp_cache(hw_address, info->mn_prefix);
					lma_update_binding_entry(bce, info, 0);   //Save information into bce
					//lma_update_binding_entry_with_DMNP_Home_addr(bce, info);
					lma_reg(bce, 0);
					pmip_cache_add(bce);
				}
			} else if ((info->msg_event == hasPBU) && (info->lifetime.tv_sec == 0) && (info->lifetime.tv_nsec == 0)) {
				dbg("PBU with Lifetime = 0 for a not-registered MN... \n");
				// LG Comment lma_dereg(bce, info, 0);
				// LG Comment pmipcache_release_entry(bce);
			}

			break;
			//--------------------------------------
		case BCE_PMIP:
			if ((info->msg_event == hasPBU) && (info->lifetime.tv_sec > 0)) {
				dbg("PBU for an existing MN ... update serving MAG\n");
				bce = pmip_cache_get(&conf.OurAddress, &hw_address);
				memset(&r_tmp1, 0, sizeof(struct in6_addr));
				//if v6 serving Mag
				if(conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)
				{
				if (IN6_ARE_ADDR_EQUAL(&r_tmp1, &bce->mn_serv_mag_addr)) //Received PBU from serving MAG
				{
					lma_update_binding_entry(bce, info, 0);
					lma_reg(bce, 0);
				} if (IN6_ARE_ADDR_EQUAL(&info->src, &bce->mn_serv_mag_addr) ) //Received PBU from serving MAG
				{
					lma_update_binding_entry(bce, info, 0);
					lma_reg_no_new_tunnel (bce, 0);
				} else { //Received PBU from an already unregistered MAG
					dbg("Deregistration for previously registered MAG\n");
					lma_dereg(bce, info, 0, 0);
					bce->type = BCE_PMIP;
					lma_update_binding_entry(bce, info, 0);
					lma_reg(bce, 0);
				}
				}
				//if v4 serving mag
				if  (conf.pmip_tunnel_mode == IP_FAMILY_V4_V01)
				{
				if (bce->mn_serv_mag_v4_addr.s_addr == 0) //Received PBU from serving MAG
				{
					lma_update_binding_entry(bce, info, 0);
					lma_reg(bce, 0);
				} if (info->srcv4.s_addr == bce->mn_serv_mag_v4_addr.s_addr ) //Received PBU from serving MAG
				{
					lma_update_binding_entry(bce, info, 0);
					lma_reg_no_new_tunnel (bce, 0);
				} else { //Received PBU from an already unregistered MAG
					dbg("Deregistration for previously registered MAG\n");
					lma_dereg(bce, info, 0, 0);
					bce->type = BCE_PMIP;
					lma_update_binding_entry(bce, info, 0);
					lma_reg(bce, 0);
				}
				}
				pmipcache_release_entry(bce);
			}
			else if (info->msg_event == hasPBU && info->lifetime.tv_sec == 0 && info->lifetime.tv_nsec == 0) {
				dbg("PBU with Lifetime = 0... start Location Deregistration\n");
				bce = pmip_cache_get(&conf.OurAddress, &hw_address);
				if (IN6_ARE_ADDR_EQUAL(&info->src, &bce->mn_serv_mag_addr)) //Received PBU from serving MAG
				{
					dbg("Deregistration case...\n");
					lma_dereg(bce, info, 1, 0);
					pmipcache_release_entry(bce);
					pmip_bce_delete(bce);
				} else { //Received PBU from an already unregistered MAG
					dbg("Deregistration for not registered MAG, doing nothing\n");
					//lma_dereg(bce, info, 0);
					pmipcache_release_entry(bce);
				}
			}
			break;
			//--------------------------------------
		default:
			dbg("No action for this event (%d) at current state (%d) !\n", info->msg_event, type);
	}
	return result;
}

//---------------------------------------------------------------------------------------------------------------------
int lma_fsm_v4(msg_info_t * info)
{
	struct in6_addr r_tmp1;
	int result = 0;
	pmip_entry_t *bce = NULL;

	struct in6_addr hw_address = info->mn_hw_address;
	int type = pmipv4_cache_exists(&conf.OurAddress, &hw_address);
	switch (type) {
		//--------------------------------------
		case BCE_NO_ENTRY:
			dbg("No PMIP entry found for %x:%x:%x:%x:%x:%x:%x:%x ... \n",
					NIP6ADDR(&info->mn_hw_address));
			if (info->msg_event == hasPBU && (info->lifetime.tv_sec > 0)) {
				//Create New Proxy Binding Entry storing information
				dbg("PBU for a new MN ... Location Registration starting now...\n");
				bce = pmip_cache_alloc(BCE_PMIP);
				if (bce != NULL) {
					lma_get_prefix(hw_address,&(info->mn_prefix));
					dbg("\nlma assigned  prefix %x:%x:%x:%x:%x:%x:%x:%x \n",NIP6ADDR(&info->mn_prefix));
					pmip_insert_into_hnp_cache(hw_address, info->mn_prefix);
					lma_update_binding_entry(bce, info, 1);   //Save information into bce
					lma_update_binding_entry_with_DMNP_Home_addr(bce, info);
					lma_reg(bce, 1);
					pmipv4_cache_add(bce);
				}
			} else if ((info->msg_event == hasPBU) && (info->lifetime.tv_sec == 0) && (info->lifetime.tv_nsec == 0)) {
				dbg("PBU with Lifetime = 0 for a not-registered MN... \n");
				// LG Comment lma_dereg(bce, info, 0);
				// LG Comment pmipcache_release_entry(bce);
			}

			break;
			//--------------------------------------
		case BCE_PMIP:
			if ((info->msg_event == hasPBU) && (info->lifetime.tv_sec > 0)) {
				dbg("PBU for an existing MN ... update serving MAG\n");
				bce = pmipv4_cache_get(&conf.OurAddress, &hw_address);
				memset(&r_tmp1, 0, sizeof(struct in6_addr));
				//if v6 serving Mag
				if(conf.pmip_tunnel_mode == IP_FAMILY_V6_V01)
				{
				if (IN6_ARE_ADDR_EQUAL(&r_tmp1, &bce->mn_serv_mag_addr)) //Received PBU from serving MAG
				{
					lma_update_binding_entry(bce, info, 1);
					lma_reg(bce, 1);
				} if (IN6_ARE_ADDR_EQUAL(&info->src, &bce->mn_serv_mag_addr) ) //Received PBU from serving MAG
				{
					lma_update_binding_entry(bce, info, 1);
					lma_reg_no_new_tunnel (bce, 1);
				} else { //Received PBU from an already unregistered MAG
					dbg("Deregistration for previously registered MAG\n");
					lma_dereg(bce, info, 0, 1);
					bce->type = BCE_PMIP;
					lma_update_binding_entry(bce, info, 1);
					lma_reg(bce, 1);
				}
				}
				//if v4 serving mag
				if  (conf.pmip_tunnel_mode == IP_FAMILY_V4_V01)
				{
				if (bce->mn_serv_mag_v4_addr.s_addr == 0) //Received PBU from serving MAG
				{
					lma_update_binding_entry(bce, info, 1);
					lma_reg(bce, 1);
				} if (info->srcv4.s_addr == bce->mn_serv_mag_v4_addr.s_addr ) //Received PBU from serving MAG
				{
					lma_update_binding_entry(bce, info, 1);
					lma_reg_no_new_tunnel (bce, 1);
				} else { //Received PBU from an already unregistered MAG
					dbg("Deregistration for previously registered MAG\n");
					lma_dereg(bce, info, 0, 1);
					bce->type = BCE_PMIP;
					lma_update_binding_entry(bce, info, 1);
					lma_reg(bce, 1);
				}
				}
				pmipv4cache_release_entry(bce);
			}
			else if (info->msg_event == hasPBU && info->lifetime.tv_sec == 0 && info->lifetime.tv_nsec == 0) {
				dbg("PBU with Lifetime = 0... start Location Deregistration\n");
				bce = pmipv4_cache_get(&conf.OurAddress, &hw_address);
				if (IN6_ARE_ADDR_EQUAL(&info->src, &bce->mn_serv_mag_addr)) //Received PBU from serving MAG
				{
					dbg("Deregistration case...\n");
					lma_dereg(bce, info, 1, 1);
					pmipv4cache_release_entry(bce);
					pmipv4_bce_delete(bce);
				} else { //Received PBU from an already unregistered MAG
					dbg("Deregistration for not registered MAG, doing nothing\n");
					//lma_dereg(bce, info, 0);
					pmipv4cache_release_entry(bce);
				}
			}
			break;
			//--------------------------------------
		default:
			dbg("No action for this event (%d) at current state (%d) !\n", info->msg_event, type);
	}
	return result;
}

