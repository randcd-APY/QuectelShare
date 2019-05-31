/*!
 * Copyright (c) 2017, The Linux Foundation. All rights reserved.
 * \file pmip_pcap.c
 * \brief Analyse of captured packets
 * \author OpenAir3 Group
 * \date 12th of October 2010
 * \version 1.0
 * \company Eurecom
 * \project OpenAirInterface
 * \email: openair3@eurecom.fr
 */
#define PMIP
#define PMIP_PCAP_C
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
//---------------------------------------------------------------------------------------------------------------------
#include "pmip_fsm.h"
#include "pmip_hnp_cache.h"
#include "pmip_pcap.h"
#include "pmip_msgs.h"
#include "pmip_types.h"
//---------------------------------------------------------------------------------------------------------------------
#include "pmipv4_conf.h"

#ifdef ENABLE_VT
#    include "vt.h"
#endif
#include "debug.h"
#include "conf.h"

#define QCMAP_PMIP_FILE "/data/qcmap_pmip_file"
#define PMIP_QCMAP_FILE "/data/pmip_qcmap_file"
#define PMIP_QCMAP_IND_FILE "/data/pmip_qcmap_ind_file"
#define false 0
#define true  1

#define MAX_BUF_LEN       256
#define INET6_ADDRSTRLEN  46
#define MAC_ADDR_NUM_CHARS 18

unsigned int qcmap_pmip_sockfd;
unsigned int pmip_qcmap_sockfd;

unsigned int pmip_qcmap_ind_sockfd;
struct mac_addr
{
	uint8_t mac_addr[PMIP_MAC_ADDR_LEN];
};

/*---------------------------------------------------------------------------
  QCMAP PMIP SOCK TYPES
  ----------------------------------------------------------------------------*/
typedef enum
{
	/* QCMAP_PMIP_ADD_DEVICE, - Dont need this for IPV6 as PMIP is dependent on RS for now*/
	QCMAP_PMIP_DEL_DEVICE = 1,
	QCMAP_PMIP_QUERY_DEVICE_REQ,
	QCMAP_PMIP_QUERY_DEVICE_RESP
}qcmap_pmip_event_t;

typedef struct
{
	uint8_t       mac_addr[PMIP_MAC_ADDR_LEN];
	uint8_t       if_id;
	ip_family_enum_v01       pmip_mode;
	uint8_t       isValidMac;

}qcmap_pmip_info_t;

typedef struct
{
	qcmap_pmip_event_t    pmip_event;
	qcmap_pmip_info_t     dev_info;
} qcmap_pmip_sock_msg_t;


//---------------------------------------------------------------------------------------------------------------------

/*!
 *  Construct message event and send it to the MAG FSM
 * \param
 */
void pmip_pcap_msg_handler_associate(struct in6_addr mn_iidP, int iifP)
{
	dbg("pmip_pcap_msg_handler_associate()\n");
	msg_info_t msg;
	memset(&msg, 0, sizeof(msg_info_t));
	//  msg.mn_iid = EUI48_to_EUI64(mn_iidP);
	msg.iif = iifP;
	msg.msg_event = hasWLCCP;
	mag_fsm(&msg);
}
//---------------------------------------------------------------------------------------------------------------------
void pmip_pcap_msg_handler_deassociate(struct in6_addr mn_iidP, int iifP)
{
	char command[200];
	uint8_t mac[PMIP_MAC_ADDR_LEN];
	char macStr[MAC_ADDR_NUM_CHARS];
	dbg("pmip_pcap_msg_handler_desassociate()\n");
	msg_info_t msg;
	memset(&msg, 0, sizeof(msg_info_t));
	//   msg.mn_iid = EUI48_to_EUI64(mn_iidP);
	msg.mn_hw_address = mn_iidP;
	msg.iif = iifP;
	msg.msg_event = hasDEREG;
	if ((conf.pmip_mode_type ==  IP_FAMILY_V6_V01) || (conf.pmip_mode_type ==  IP_FAMILY_V4V6_V01))
		mag_fsm(&msg);
	if ((conf.pmip_mode_type ==  IP_FAMILY_V4_V01) || (conf.pmip_mode_type ==  IP_FAMILY_V4V6_V01)) {
		dbg("pmip_pcap_msg_handler_deassociate mag_fsm_v4 called\n");
		//remove dhcp registartion from /etc/dhcp_hosts
		//convert  mac to string
		ipv6_to_mac(&msg.mn_hw_address, mac);
		dbg("-------------------pmip_pcap_msg_handler_deassociate-----------------------\n");
		dbg("pmip_pcap_msg_handler_deassociate mac address : %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
		mac_addr_ntop(mac, (char *)&macStr);
		snprintf(command, 200,"sed -i '/%s/d' %s",macStr,PMIP_DHCP_RESER_FILE);
		pmip_system_call(command,strlen(command),1);
		dbg("SIGUP signal send to dnsmasq\n");
		system ("killall -1 dnsmasq");
    mag_fsm_v4(&msg);
	}
}

void mac_to_ipv6(const uint8_t *mac, struct in6_addr * mac_v6)
{
	assert(mac && mac_v6);
	memset(mac_v6,0,sizeof(struct in6_addr));
	//  *mac_v6 = in6addr_any;
	mac_v6->s6_addr[0]  = 0;
	mac_v6->s6_addr[1]  = 0;
	mac_v6->s6_addr[2]  = 0;
	mac_v6->s6_addr[3]  = 0;
	mac_v6->s6_addr[4]  = 0;
	mac_v6->s6_addr[5]  = 0;
	mac_v6->s6_addr[6]  = 0;
	mac_v6->s6_addr[7]  = 0;
	mac_v6->s6_addr[8]  = 0;
	mac_v6->s6_addr[9]  = 0;
	mac_v6->s6_addr[10] = mac[0];
	mac_v6->s6_addr[11] = mac[1];
	mac_v6->s6_addr[12] = mac[2];
	mac_v6->s6_addr[13] = mac[3];
	mac_v6->s6_addr[14] = mac[4];
	mac_v6->s6_addr[15] = mac[5];
	return;
}

void ipv6_to_mac(const struct in6_addr * mac_v6, uint8_t *mac)
{
	assert(mac && mac_v6);
	memset(mac,0,PMIP_MAC_ADDR_LEN);
	//  *mac_v6 = in6addr_any;
	mac[0] = mac_v6->s6_addr[10];
	mac[1] = mac_v6->s6_addr[11];
	mac[2] = mac_v6->s6_addr[12];
	mac[3] = mac_v6->s6_addr[13];
	mac[4] = mac_v6->s6_addr[14];
	mac[5] = mac_v6->s6_addr[15];
	return;
}

void mac_addr_ntop(const uint8_t *mac, char *mac_addr_str)
{
	memset(mac_addr_str,0,(size_t)MAC_ADDR_NUM_CHARS);
	snprintf(mac_addr_str,(size_t)MAC_ADDR_NUM_CHARS,"%02x:%02x:%02x:%02x:%02x:%02x",
			mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	return;
}

void ipv4_to_ipv6(struct in_addr *v4_ip, struct in6_addr *v6_ip)
{
	memset(v6_ip,0,sizeof(struct in6_addr));
	memcpy(&v6_ip->s6_addr[12],&v4_ip->s_addr,4);
	return;
}

void ipv6_to_ipv4(const struct in6_addr * v6_ip, struct in_addr *v4_ip)
{
	memcpy(&v4_ip->s_addr,&v6_ip->s6_addr[12],4);
	return;
}


int create_socket(unsigned int *sockfd)
{

	if ((*sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
	{
		dbg("create_socket: Error creating socket, errno: %d, errmsg:%s;, sockfd:%d\n", errno, strerror(errno),*sockfd);
		return -1;
	}

	if(fcntl(*sockfd, F_SETFD, FD_CLOEXEC) < 0)
	{
		dbg("Couldn't set Close on Exec, errno: %d\n", errno, 0, 0);
	}
	dbg("create_socket: sucess sockfd:%d\n", *sockfd);
	return 0;
}

int create_server_socket(unsigned int* sockfd, char* file_path)
{
	int val, rval;
	struct sockaddr_un qcmap_un;
	int len;
	struct timeval rcv_timeo;

	rval = create_socket(sockfd);
	if( rval == -1 || *sockfd < 0 )
	{
		dbg("failed to create server sockfd. errno:%d errmsg:%s\n", errno, strerror(errno) );
		return -1;
	}
	else if (*sockfd >= FD_SETSIZE)
	{
		dbg(" create_server_socket fd=%d is exceeding range, errno:%d\n", *sockfd, errno, 0);
		return -1;
	}

	rcv_timeo.tv_sec = 0;
	rcv_timeo.tv_usec = 100000;
	setsockopt(*sockfd, SOL_SOCKET, SO_RCVTIMEO, (void *)&rcv_timeo, sizeof(rcv_timeo));
	val = fcntl(*sockfd, F_GETFL, 0);
	fcntl(*sockfd, F_SETFL, val | O_NONBLOCK);
	qcmap_un.sun_family = AF_UNIX;
	strncpy(qcmap_un.sun_path, file_path, sizeof(qcmap_un.sun_path));
	unlink(qcmap_un.sun_path);
	len = strlen(qcmap_un.sun_path) + sizeof(qcmap_un.sun_family);

	if (bind(*sockfd, (struct sockaddr *)&qcmap_un, len) == -1)
	{
		dbg("Error binding the server socket, errno: %d\n", errno, 0, 0);
		return -1;
	}
	return 0;
}

int pmip_send_qcmap_dev_query
(
 int iifP,
  struct in6_addr *hw_addr,
  int is_v4_pmip_mode
)
{
	qcmap_pmip_sock_msg_t  send_dev_info;
	qcmap_pmip_sock_msg_t  *send_dev_info_ptr;
	struct sockaddr_un qcmap_pmip;
	int numBytes=0, len;
	struct sockaddr_storage their_addr;
	socklen_t addr_len = sizeof(struct sockaddr_storage);
	int  nbytes=0;
	char buf[MAX_BUF_LEN];
	struct in6_addr  mn_iidP_1;
	//  struct in6_addr             mn_iid_1;
	//  struct in6_addr mn_iid_2;
	/*struct in6_addr mn_iid_2;*/
	char ip6_addr_str[INET6_ADDRSTRLEN];
	char mac_addr_str[PMIP_MAC_ADDR_LEN];
	int type;

	qcmap_pmip.sun_family = AF_UNIX;
	strncpy(qcmap_pmip.sun_path, QCMAP_PMIP_FILE, sizeof(QCMAP_PMIP_FILE));
	len = strlen(qcmap_pmip.sun_path) + sizeof(qcmap_pmip.sun_family);

	memset(ip6_addr_str, 0x0, INET6_ADDRSTRLEN);
	memset((void*)&send_dev_info, 0x0, sizeof(qcmap_pmip_sock_msg_t));

	//set pmip mode
	if(is_v4_pmip_mode == 1)
		send_dev_info.dev_info.pmip_mode = IP_FAMILY_V4_V01;
	else if(is_v4_pmip_mode == 0)
		send_dev_info.dev_info.pmip_mode = IP_FAMILY_V6_V01;
	else
  send_dev_info.dev_info.pmip_mode = conf.pmip_mode_type;

	dbg("Pmip is using Mac address for authentication\n", 0, 0, 0);
		dbg("pmip_send_qcmap_dev_query enter\n", 0, 0, 0);
		send_dev_info.dev_info.isValidMac= true;
		ipv6_to_mac(hw_addr, send_dev_info.dev_info.mac_addr);
		dbg("---pmip_send_qcmap_dev_query mac to send: %02x:%02x:%02x:%02x:%02x:%02x\n", send_dev_info.dev_info.mac_addr[0],send_dev_info.dev_info.mac_addr[1],send_dev_info.dev_info.mac_addr[2],send_dev_info.dev_info.mac_addr[3],send_dev_info.dev_info.mac_addr[4],send_dev_info.dev_info.mac_addr[5]);

	send_dev_info.pmip_event = QCMAP_PMIP_QUERY_DEVICE_REQ;
	if ((numBytes = sendto(qcmap_pmip_sockfd, (void *)&send_dev_info, sizeof(qcmap_pmip_sock_msg_t), 0,
					(struct sockaddr *)&qcmap_pmip, len)) == -1)
	{
		dbg("Send Failed from pmip context\n", 0, 0, 0);
		return false;
	}
	memset((void*)&send_dev_info, 0x0, sizeof(qcmap_pmip_sock_msg_t));
	while(1)
	{
		nbytes = recvfrom(pmip_qcmap_sockfd, buf, MAX_BUF_LEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len);
		dbg("No data from socket errno:%d\n",errno, 0, 0);
		if (nbytes <= 0)
		{
			if(errno == EAGAIN)
				sleep(1);
			continue;
			dbg("Failed to receive data socket errno:%d\n",errno, 0, 0);
			return false;
		}
		else
			break;
	}
	dbg("Completed full recv from PMIP context Recvd bytes# %d \n",nbytes, 0, 0);

	if(nbytes > sizeof(qcmap_pmip_sock_msg_t) || nbytes < sizeof(qcmap_pmip_sock_msg_t))
	{
		dbg("Bad number of bytes received\n",0, 0, 0);
	}
	send_dev_info_ptr = (qcmap_pmip_sock_msg_t*)buf;

	dbg("rcvd envent: %d \n",send_dev_info_ptr->pmip_event, 0, 0);

	if(send_dev_info_ptr->pmip_event == QCMAP_PMIP_QUERY_DEVICE_RESP)
	{
		if (is_v4_pmip_mode == 0)
		{

			type = pmip_cache_exists(&conf.OurAddress, hw_addr);
		}
		else if(is_v4_pmip_mode == 1)
		{
			type = pmipv4_cache_exists(&conf.OurAddress, hw_addr);
		}
		if(type != BCE_NO_ENTRY)
		{
			dbg("Entry Already exists with type %d. Ignore do nothing\n",type, 0, 0);
			return false;
		}

		dbg("All good add this mac %s \n",ip6_addr_str, 0, 0);
		return true;
	}
	else
		dbg("invalid event received\n", 0, 0, 0);
	return false;
}


//---------------------------------------------------------------------------------------------------------------------
void pmip_pcap_loop(char *devname, int iif)
{
	//    char addrmac[16];
	//   char addrmacByte[16];
	//    struct in6_addr mn_iid;
	struct sockaddr_storage their_addr;
	socklen_t addr_len = sizeof(struct sockaddr_storage);
	int  nbytes=0;
	char buf[MAX_BUF_LEN];
	struct in6_addr hw_address;
	char mac_addr_str[PMIP_MAC_ADDR_LEN];

	int ret;
	fd_set fds;
	qcmap_pmip_sock_msg_t  *send_dev_info;
	char ip6_addr_str[INET6_ADDRSTRLEN];
	memset(ip6_addr_str, 0x0, INET6_ADDRSTRLEN);

	/*create server and client sockets
	  wait on the Server socket...
	  once FD set call diassociate for DEL event
	  write function to send to qcmap*/
	qcmap_pmip_sockfd = 0;
	pmip_qcmap_ind_sockfd = 0;
	pmip_qcmap_sockfd = 0;

	if(create_socket(&qcmap_pmip_sockfd) == -1)
	{
		dbg("Unable to create qcmap_pmip_sockfd socket!: errno:%d\n", errno,0,0);
	}

	/* Create pmip -> qcmap server socket */
	if (create_server_socket(&pmip_qcmap_ind_sockfd, PMIP_QCMAP_IND_FILE) != 0)
	{
		dbg("Unable to create pmip_qcmap_ind_sockfd socket! errno:%d\n", errno,0,0);
	}

	/* Create pmip -> qcmap server socket */
	if (create_server_socket(&pmip_qcmap_sockfd, PMIP_QCMAP_FILE) != 0)
	{
		dbg("Unable to create pmip_qcmap_ind_sockfd socket! errno:%d\n", errno,0,0);
	}

	FD_ZERO(&fds);
	FD_SET(pmip_qcmap_ind_sockfd, &fds);

	while (1)
	{
		ret = select(pmip_qcmap_ind_sockfd+1, &fds, NULL, NULL, NULL);
		if (ret < 0) {
			if (errno == EINTR)
				continue;
		}

		if ( FD_ISSET(pmip_qcmap_ind_sockfd, &fds) )
		{
			dbg("RECEIVED PACKET\n", 0,0);

			nbytes = recvfrom(pmip_qcmap_ind_sockfd, buf, MAX_BUF_LEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len);
			if (nbytes <= 0)
			{
				dbg("No data from socket errno:%d\n",errno, 0, 0);
			}
			dbg("Completed full recv from PMIP context Recvd bytes# %d \n",nbytes, 0, 0);
			if(nbytes > sizeof(qcmap_pmip_sock_msg_t) || nbytes < sizeof(qcmap_pmip_sock_msg_t))
			{
				dbg("Bad number of bytes received\n",0, 0, 0);
			}
			send_dev_info = (qcmap_pmip_sock_msg_t*)buf;
			dbg("rcvd envent: %d \n",send_dev_info->pmip_event, 0, 0);

			if(send_dev_info->pmip_event != QCMAP_PMIP_DEL_DEVICE)
			{
				continue;
			}

			mac_addr_ntop((uint8_t *)send_dev_info->dev_info.mac_addr, (char *)&mac_addr_str);

			mac_to_ipv6((uint8_t *)(send_dev_info->dev_info.mac_addr),&hw_address);
			inet_ntop(AF_INET6, (uint8_t*)&hw_address, ip6_addr_str, INET6_ADDRSTRLEN);
			dbg("hw_address %s \n",ip6_addr_str, 0, 0);

			/*    pmip_pcap_msg_handler_associate(mn_iid, iif);*/
			pmip_pcap_msg_handler_deassociate(hw_address, iif);
		} /*fd set*/
	}/*while 1*/
}/*pmip_pcap_loop*/

