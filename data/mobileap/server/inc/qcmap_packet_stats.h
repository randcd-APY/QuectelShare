/******************************************************************************

                          QCMAP_PACKET_STATS . H

******************************************************************************/
/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/******************************************************************************

  @file    qcmap_packet_stats.h
  @brief   QCMAP packet stats functionality header file

  DESCRIPTION
  Header file for QCMAP packet stats functionality.

******************************************************************************/

/******************************************************************************

                      EDIT HISTORY FOR FILE

  $Id:$

when       who        what, where, why
--------   ---        -------------------------------------------------------
04/05/2017  gs         created module

******************************************************************************/

#ifndef __QCMAP_PACKET_STATS_H__
#define __QCMAP_PACKET_STATS_H__

#include <glib.h>
#include <stdbool.h>
#include "comdef.h"

#include <signal.h>
#include <time.h>
#include <linux/if_addr.h>
#include <netinet/ether.h>
#include <stdint.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <netinet/icmp6.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

#include <assert.h>
#include <sys/time.h>
#include <sys/select.h>
#include <pthread.h>

#include "QCMAP_ConnectionManager.h"
#include "QCMAP_Backhaul.h"


#ifdef __cplusplus
extern "C"
{
#endif
/* packet stats framework*/

extern int sfev6_nl_sock_fd;
extern int sfev4_nl_sock_fd;
extern uint8 *ipv4_tag_pool;
extern uint8 *ipv6_tag_pool;

#define SUCCESS 0
#define FAILURE -1

#define IPA_IOCTL_DEVICE_NAME "/dev/wwan_ioctl"
#define IPV6_MAX_PREFIX_LENGTH 128
#define PACKET_STATS_MSG 30
#define NL_MESSAGE_TYPE PACKET_STATS_MSG
#define NL_MAX_BUF 1024
#define NL_UNICAST_GRP 0
#define NL_IPV6_PROTO_ID 25
#define NL_IPV4_PROTO_ID 26
#define NL_PID_MASK (0x7FFFFFFF)
#define SFE_IPV6_RESET_PACKET_STATS_COUNTERS 0xAA
#define SFE_IPV6_DELETE_PACKET_STATS_NODE 0xAB
#define SFE_IPV4_RESET_PACKET_STATS_COUNTERS 0xAC
#define SFE_IPV4_DELETE_PACKET_STATS_NODE 0xAD
#define IPV4_RULE 0
#define IPV6_RULE 1
#define IPV4_PACK_STAT_IPTABLES_DUMP_FILE "/tmp/iptabledump.txt"
#define IPV4_PACK_STAT_IPTABLES_SHELL_FILE "/tmp/iptabledump.sh"
#define IPV6_PACK_STAT_IPTABLES_DUMP_FILE "/tmp/ip6tabledump.txt"
#define IPV6_PACK_STAT_IPTABLES_SHELL_FILE "/tmp/ip6tabledump.sh"
#define IPV4_PACK_STAT_IPTABLES_STATS_SHELL_FILE "/tmp/iptablestatsdump.sh"
#define IPV6_PACK_STAT_IPTABLES_STATS_SHELL_FILE "/tmp/ip6tablestatsdump.sh"
#define IPV4_PACK_STAT_SFE_SHELL_FILE "/tmp/sfeipv4dump.sh"
#define IPV6_PACK_STAT_SFE_SHELL_FILE "/tmp/sfeipv6dump.sh"
#define IPV4_PACK_STAT_SFE_XML_FILE "/tmp/sfeipv4dump.xml"
#define IPV6_PACK_STAT_SFE_XML_FILE "/tmp/sfeipv6dump.xml"
#define IPV4_PACK_STAT_IPTABLES_STATS_DUMP_FILE "/tmp/iptablestatsdump.txt"
#define IPV6_PACK_STAT_IPTABLES_STATS_DUMP_FILE "/tmp/ip6tablestatsdump.txt"

#define IPTABLES_AWK_REG_EXP_FORWARD " | awk '$1 ~ /^[0-9]+$/ {printf \"%s,%s,%s,%s,%s,\\n\",$1,$3,$9,$10,$13}'"
#define IPTABLES_AWK_REG_EXP_STATS " | awk '$1 ~ /^[0-9]+$/ {printf \"%s,%s,%s,%s,%s,\\n\",$1,$3,$9,$10,$13}'"
#define IP6TABLES_AWK_REG_EXP_FORWARD " | awk '$1 ~ /^[0-9]+$/ {printf \"%s,%s,%s,%s,%s,\\n\",$1,$3,$8,$9,$12}'"
#define IP6TABLES_AWK_REG_EXP_STATS " | awk '$1 ~ /^[0-9]+$/ {printf \"%s,%s,%s,%s,%s,\\n\",$1,$3,$8,$9,$12}'"

#define CREATE_IPTABLES_CHAIN "ip6tables -t mangle -N TRAFFIC_ACCT;" \
                              "iptables -t mangle -N TRAFFIC_ACCT;" \
                              "ip6tables -t mangle -A POSTROUTING -j TRAFFIC_ACCT;" \
                              "iptables -t mangle -A POSTROUTING -j TRAFFIC_ACCT"
#define CLEANUP_IPTABLES_CHAIN  "ip6tables  -t mangle -F TRAFFIC_ACCT;" \
                                "ip6tables  -t mangle -D POSTROUTING -j TRAFFIC_ACCT;" \
                                "ip6tables  -t mangle  -X TRAFFIC_ACCT;" \
                                "iptables  -t mangle -F TRAFFIC_ACCT;" \
                                "iptables  -t mangle -D POSTROUTING -j TRAFFIC_ACCT;" \
                                "iptables  -t mangle  -X TRAFFIC_ACCT"
#define RESET_EXCEPTION_DATA_STATS "iptables -t mangle -Z TRAFFIC_ACCT;" \
                                   "ip6tables -t mangle -Z TRAFFIC_ACCT"

#define PACKET_STATS_TAG "packet_stats"
#define STATS_TAG "stats"
#define CONNECTIONS_TAG "connections"
#define CONNECTION "connection"

#define CLIENT_ADDR_ATTR_TAG "client_addr"
#define RX_BYTE_ATTR_TAG "rx_bytes"
#define TX_BYTE_ATTR_TAG "tx_bytes"

#define MARK_MASK 0xFF000
#define MARK_BASE 0xAA000
#define MAX_MARK_VALUE 0x4B0 //1200
#define MIN_MARK_VALUE 0

 #define SFE_IPV4_DUMP_TEXT1 "sfe_dump(){\n"
 #define SFE_IPV4_DUMP_TEXT2 "\t[ -e \"/dev/sfe_packet_stats_ipv4\" ] || {\n"
 #define SFE_IPV4_DUMP_TEXT3 "\t\tdev_num=$(cat /sys/sfe_packet_stats_ipv4/packet_stats_dev)\n"
 #define SFE_IPV4_DUMP_TEXT4 "\t\tmknod /dev/sfe_packet_stats_ipv4 c $dev_num 0\n"
 #define SFE_IPV4_DUMP_TEXT5 "\t}\n"
 #define SFE_IPV4_DUMP_TEXT6 "\tcat /dev/sfe_packet_stats_ipv4\n"
 #define SFE_IPV4_DUMP_TEXT7 "}\n"
 #define SFE_IPV4_DUMP_TEXT8 "sfe_dump\n"

 #define SFE_IPV6_DUMP_TEXT1 "sfe_dump(){\n"
 #define SFE_IPV6_DUMP_TEXT2 "\t[ -e \"/dev/sfe_packet_stats_ipv6\" ] || {\n"
 #define SFE_IPV6_DUMP_TEXT3 "\t\tdev_num=$(cat /sys/sfe_packet_stats_ipv6/packet_stats_dev)\n"
 #define SFE_IPV6_DUMP_TEXT4 "\t\tmknod /dev/sfe_packet_stats_ipv6 c $dev_num 0\n"
 #define SFE_IPV6_DUMP_TEXT5 "\t}\n"
 #define SFE_IPV6_DUMP_TEXT6 "\tcat /dev/sfe_packet_stats_ipv6\n"
 #define SFE_IPV6_DUMP_TEXT7 "}\n"
 #define SFE_IPV6_DUMP_TEXT8 "sfe_dump\n"

typedef struct nl_ipv6_tx_buffer
{
  uint8_t command;
  __be32 client_src_addr[4];
}nl_ipv6_tx_buffer_t;

typedef struct nl_ipv4_tx_buffer
{
  uint8_t command;
  __be32 client_src_addr;
}nl_ipv4_tx_buffer_t;


typedef struct
{
  struct sockaddr_in sa;
  uint64_t tx_data;
  uint64_t rx_data;
}qcmap_cm_sfe_ipv4_dump_t;

typedef struct
{
  struct sockaddr_in6 sa;
  uint64_t tx_data;
  uint64_t rx_data;
}qcmap_cm_sfe_ipv6_dump_t;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
  FUNCTION QcmapSFEv4NLInit
==========================================================================*/
/*!
@brief
  Initialize the NL socket for QCMAP to communicate with SFE IPV4 Module

@parameters
None

@return

@note
- Dependencies
- packet stats need to be enabled
- SFE need to be loaded

- Side Effects
- None
*/
/*=========================================================================*/
int QcmapSFEv4NLInit(void);

/*=====================================================
  FUNCTION IsIPv6RuleAlreadyPresent
======================================================*/
/*!
@brief
  - Checks if IPv6 address rule already present
@parameters
  pointer to ipaddress string
  awk expression for parsing iptables
@return
  none
@note
- Dependencies
- packet stats need to be enabled
- Side Effects
- None
*/
/*=====================================================*/
int IsIPv6RuleAlreadyPresent(char* addr);

/*=====================================================
  FUNCTION AddIpv6IPtableRule
======================================================*/
/*!
@brief
  - Add the ipv6 rule to ip6tables
@parameters
  pointer to ipaddress string
@return
  none
@note
- Dependencies
- packet stats need to be enabled
- Side Effects
- None
*/
/*=====================================================*/
int AddIpv6IPtableRule(char* addr);

/*=====================================================
  FUNCTION DeleteStatsIPv4Rule
======================================================*/
/*!
@brief
  - Deletes a rule in iptables using line number
@parameters
  pointer to mark string

@return
  none
@note
- Dependencies
- packet stats need to be enabled
- Side Effects
- None
*/
/*=====================================================*/
int DeleteStatsIPv4Rule(char* mark_match);


/*=====================================================
  FUNCTION ResetExceptionDataStats
======================================================*/
/*!
@brief
  - reset exception data stats in iptables and ip6tables
@parameters
  none
@return
  none
@note
- Dependencies
- packet stats need to be enabled
- Side Effects
- None
*/
/*=====================================================*/
void ResetExceptionDataStats(void);


/*=====================================================
  FUNCTION ResetSfeDataStats
======================================================*/
/*!
@brief
  - reset sfe data stats
@parameters
  none

@return
  none
@note
- Dependencies
- packet stats need to be enabled
- Side Effects
- None
*/
/*=====================================================*/
void ResetSfeDataStats(void);


/*===========================================================================
  FUNCTION QcmapSFEv6NLInit
==========================================================================*/
/*!
@brief
  Initialize the NL socket for QCMAP to communicate with SFE IPV6 Module

@parameters
None

@return

@note
- Dependencies
- packet stats need to be enabled
 - SFE need to be loaded

- Side Effects
- None
*/
/*=========================================================================*/
int QcmapSFEv6NLInit(void);

/*=====================================================
  FUNCTION DeleteIPv6IptableRule
======================================================*/
/*!
@brief
  - Deletes a rule in ip6tables using line number
@parameters
  pointer to ipaddress string

@return
  none
@note
- Dependencies
- packet stats need to be enabled
- Side Effects
- None
*/
/*=====================================================*/
int DeleteIPv6IptableRule(char* ip_addr);

/*=====================================================
  FUNCTION DeleteIPv4IptableRule
======================================================*/
/*!
@brief
  - Deletes a rule in iptables using line number
@parameters
  pointer to ipaddress string

@return
  none
@note
- Dependencies
- packet stats need to be enabled
- Side Effects
- None
*/
/*=====================================================*/
int DeleteIPv4IptableRule(char* ip_addr);

/*===========================================================================
  FUNCTION createSFEDumpFile
==========================================================================*/
/*!
@brief
Creates IPV4 and IPV6 sh script to dump sfe connections
@parameters
- None


@return

@note
- Dependencies
- packet stats need to be enabled

- Side Effects
- None
*/
/*=========================================================================*/
int createSFEDumpFile(void);

/*===========================================================================
  FUNCTION removeSFEDumpFile
==========================================================================*/
/*!
@brief
removes IPV4 and IPV6 sh script to dump sfe connections
@parameters
- None


@return

@note
- Dependencies
- packet stats need to be enabled

- Side Effects
- None
*/
/*=========================================================================*/
void removeSFEDumpFile(void);

/*===========================================================================
  FUNCTION UpdateIPV4ExceptionStats
==========================================================================*/
/*!
@brief
  Update the packets stats from ipv4 exception path

@parameters
  - pointer to Connected device
  - Ipv4 address


@return

@note
- Dependencies
- packet stats need to be enabled

- Side Effects
- None
*/
/*=========================================================================*/
void UpdateIPV4ExceptionStats(qcmap_cm_client_data_info_t* cdiNode, char* ipv4addr);

/*===========================================================================
  FUNCTION UpdateIPV6ExceptionStats
==========================================================================*/
/*!
@brief
  Update the packets stats from ipv6 exception path

@parameters
  - pointer to Connected device
  - Ipv6 address

@return

@note
- Dependencies
- packet stats need to be enabled

- Side Effects
- None
*/
/*=========================================================================*/
void UpdateIPV6ExceptionStats(qcmap_cm_client_data_info_t* cdiNode, char* ipv6addr);

#ifdef FEATURE_MOBILEAP_PER_CLIENT_STATS
/*===========================================================================
  FUNCTION NotifyIPACM
==========================================================================*/
/*!
@brief
  Notify IPACM about the client connect/disconnect.
@parameters
  - pointer to Connected device node


@return

@note
- Dependencies
- packet stats need to be enabled

- Side Effects
- None
*/
/*=========================================================================*/
void NotifyIPACM(struct wan_ioctl_send_lan_client_msg  *ioctl_buffer);

/*===========================================================================
  FUNCTION UpdateIPAStats
==========================================================================*/
/*!
@brief
  Update the packets stats from IPA connections
@parameters
  - pointer to Connected device node


@return

@note
- Dependencies
- packet stats need to be enabled

- Side Effects
- None
*/
/*=========================================================================*/
void UpdateIPAStats(struct wan_ioctl_query_per_client_stats  *ioctl_buffer);
#endif
/*===========================================================================
  FUNCTION UpdateSFEStats
==========================================================================*/
/*!
@brief
  Update the packets stats from sfe connections
@parameters
  - pointer to Connected device


@return

@note
- Dependencies
- packet stats need to be enabled

- Side Effects
- None
*/
/*=========================================================================*/
void UpdateSFEStats(qcmap_cm_client_data_info_t* cdiNode);

/*===========================================================================
  FUNCTION extractSFEv4data
==========================================================================*/
/*!
@brief
Extracts IPV4 SFE stats and populate a local array.
@parameters
  - file path from which connections are extracted
  -[out] number of entries extracted


@return
- pointer to ipv4 arrary of connections

@note
- Dependencies
- packet stats need to be enabled

- Side Effects
- None
*/
/*=========================================================================*/
qcmap_cm_sfe_ipv4_dump_t* extractSFEv4data(char* file_path, uint32_t * num_entries);

/*===========================================================================
  FUNCTION extractSFEv6data
==========================================================================*/
/*!
@brief
Extracts IPV6 SFE stats and populate a local array.
@parameters
  - file path from which connections are extracted
  -[out] number of entries extracted


@return
- pointer to ipv6 arrary of connections
@note
- Dependencies
- packet stats need to be enabled

- Side Effects
- None
*/
/*=========================================================================*/
qcmap_cm_sfe_ipv6_dump_t* extractSFEv6data(char* file_path, uint32_t * num_entries);

/*===========================================================================
  FUNCTION SendNLMsgToSFE
==========================================================================*/
/*!
@brief
  Sends the NL msg to SFE

@parameters
  - socket for ipv6 or ipv4
  - command (reset counters or delete node)
  - ipaddress of the client node need to be deleted

@return

@note
- Dependencies
- packet stats need to be enabled

- Side Effects
- None
*/
/*=========================================================================*/
void SendNLMsgToSFE(int sock, int cmd, char* ip_addr);

/*=====================================================
  FUNCTION LookUpCDIAndUpdate
======================================================*/
/*!
@brief
  - Match the MAC address
  - If a match is found, check if given IP already exist if not add them to entry
@parameters
  Pointer to pkt_buf to fetch address and vlan_id
@return
  none
@note
- Dependencies
- None
- Side Effects
- None
*/
/*=====================================================*/
bool LookUpCDIAndUpdate(qcmap_nl_sock_msg_t* pktbuf);



/* packet stats framework end*/


#ifdef __cplusplus
}
#endif

#endif /* __QCMAP_PACKET_STATS_H__ */
