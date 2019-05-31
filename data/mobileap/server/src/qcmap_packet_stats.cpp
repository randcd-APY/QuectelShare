/*===========================================================================
FILE:  QCMAP_PACKET_STATS.CPP

SERVICES:
   The Data Services packet stats functionality source file

=============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
/*===========================================================================

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/05/17    gs     Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>

#include "qcmap_packet_stats.h"
#ifdef FEATURE_MOBILEAP_PER_CLIENT_STATS
#include "linux/rmnet_ipa_fd_ioctl.h"
#endif


struct msghdr nl_msg;
int sfev6_nl_sock_fd = -1;
int sfev4_nl_sock_fd = -1;
uint8 *ipv4_tag_pool = NULL;
uint8 *ipv6_tag_pool = NULL;
struct sockaddr_nl sfe_addr;
struct iovec iov;

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
void SendNLMsgToSFE(int sock, int cmd, char* ip_addr)
{
  nl_ipv4_tx_buffer_t nlv4databuf;
  nl_ipv6_tx_buffer_t nlv6databuf;
  struct nlmsghdr *nlh = NULL;

  if (sock <0)
  {
    LOG_MSG_ERROR("Invalid Socket Passed",0,0,0);
    return;
  }

  if (ip_addr == NULL)
  {
    ip_addr = "::";
  }
  memset(&sfe_addr, 0, sizeof(sfe_addr));
  sfe_addr.nl_family = AF_NETLINK;
  sfe_addr.nl_pid = 0;
  sfe_addr.nl_groups = NL_UNICAST_GRP;
  memset(&nlv6databuf,0, sizeof(nlv6databuf));
  memset(&nlv4databuf,0, sizeof(nlv4databuf));

  nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(NL_MAX_BUF));
  if (nlh == NULL)
  {
    LOG_MSG_ERROR("Malloc failure",0,0,0);
    return;
  }
  memset(nlh, 0, NLMSG_SPACE(NL_MAX_BUF));
  nlh->nlmsg_len = NLMSG_SPACE(NL_MAX_BUF);
  nlh->nlmsg_pid = (getpid() & NL_PID_MASK);
  nlh->nlmsg_flags = 0;
  nlh->nlmsg_type = NL_MESSAGE_TYPE;

  switch (cmd)
  {
  case SFE_IPV6_RESET_PACKET_STATS_COUNTERS:
      nlv6databuf.command = cmd;
      /* Copy the Buffer */
      memcpy(NLMSG_DATA(nlh), &nlv6databuf, sizeof(nlv6databuf));
    break;
  case SFE_IPV4_RESET_PACKET_STATS_COUNTERS:
      nlv4databuf.command = cmd;
      /* Copy the Buffer */
      memcpy(NLMSG_DATA(nlh), &nlv4databuf, sizeof(nlv4databuf));
    break;
  case SFE_IPV6_DELETE_PACKET_STATS_NODE:
      nlv6databuf.command = cmd;
      inet_pton(AF_INET6,ip_addr , &nlv6databuf.client_src_addr);
      /* Copy the Buffer */
      memcpy(NLMSG_DATA(nlh), &nlv6databuf, sizeof(nlv6databuf));

    break;
  case SFE_IPV4_DELETE_PACKET_STATS_NODE:
      nlv4databuf.command = cmd;
      inet_pton(AF_INET, ip_addr, &nlv4databuf.client_src_addr);
      /* Copy the Buffer */
      memcpy(NLMSG_DATA(nlh), &nlv4databuf, sizeof(nlv4databuf));

    break;
  default:
    break;
  }

  iov.iov_base = (void *)nlh;
  iov.iov_len = nlh->nlmsg_len;
  nl_msg.msg_name = (void *)&sfe_addr;
  nl_msg.msg_namelen = sizeof(sfe_addr);
  nl_msg.msg_iov = &iov;
  nl_msg.msg_iovlen = 1;
  sendmsg(sock, &nl_msg, 0);
}

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
void ResetExceptionDataStats(void)
{
  ds_system_call( RESET_EXCEPTION_DATA_STATS,
                   strlen(RESET_EXCEPTION_DATA_STATS));


}

/*=====================================================
  FUNCTION AddIpv4IPtableRule
======================================================*/
/*!
@brief
  - Add the ipv4 rule to iptables
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
int AddIpv4IPtableRule(char* addr)
{
  char command[MAX_COMMAND_STR_LEN]={0};
  unsigned int mark_tag_pool_ix = 0;
  unsigned int mark_hash = 0;
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int qcmap_cm_error;
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();


  if (addr == NULL)
  {
    LOG_MSG_INFO1("NULL addr passed",0,0,0);
    return -1;
  }
  if(!QcMapBackhaulWWANMgr)
  {
    LOG_MSG_ERROR("AddIpv4IPtableRule: Null QcMapBackhaulWWANMgr pointer",0,0,0);
    return -1;
  }

  //get mark value from mark tag pool
  for (mark_tag_pool_ix = 0; mark_tag_pool_ix < MAX_MARK_VALUE; mark_tag_pool_ix++)
  {
    if(!ipv4_tag_pool[mark_tag_pool_ix])
    {
      break;
    }
  }
  if (mark_tag_pool_ix+1 >= MAX_MARK_VALUE)
  {
    LOG_MSG_INFO1("Pool Tag ran out of memory",0,0,0);
    return -1;
  }

  if(QcMapBackhaulWWANMgr->GetDeviceName(QCMAP_ConnectionManager::qcmap_cm_handle,
                       QCMAP_MSGR_IP_FAMILY_V4_V01,
                       devname, &qcmap_cm_error) != QCMAP_CM_SUCCESS )
  {
    LOG_MSG_ERROR("Couldn't get ipv4 rmnet name. error %d\n", qcmap_cm_error,0,0);
    return false;
  }

  ipv4_tag_pool[mark_tag_pool_ix] = 1; // for UL rule
  ipv4_tag_pool[mark_tag_pool_ix+1] = 1; // for DL rule

  mark_hash = mark_tag_pool_ix+MARK_BASE;

  snprintf(command, MAX_COMMAND_STR_LEN,
           "iptables -t mangle -A FORWARD  -s %s -o %s -j MARK --set-xmark 0x%X", addr,devname,mark_hash);
  ds_system_call( command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,
           "iptables -t mangle -A FORWARD  -d %s -i %s -j MARK --set-xmark 0x%X", addr,devname,mark_hash+1);
  ds_system_call( command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
           "iptables -t mangle  -A TRAFFIC_ACCT -m mark --mark %d -s %s -j ACCEPT",
           mark_hash,addr);
  ds_system_call( command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,
           "iptables -t mangle  -A TRAFFIC_ACCT -m mark --mark %d -d %s -j ACCEPT",
           mark_hash+1,addr);
  ds_system_call( command, strlen(command));
  return 1;
}


/*=====================================================
  FUNCTION IsIPv4RuleAlreadyPresent
======================================================*/
/*!
@brief
  - Checks if IPv4 address rule already present
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
int IsIPv4RuleAlreadyPresent(char* addr)
{
  char command[MAX_COMMAND_STR_LEN]={0};
  char tmp[MAX_COMMAND_STR_LEN]={0};
  FILE *fd = NULL;
  char *bytes = NULL,*line_number = NULL,*saddr=NULL, *daddr=NULL, *mark=NULL,*ptr;
  int addr_found = 0;
  int bufferSize = MAX_COMMAND_STR_LEN;
  char stringline[MAX_COMMAND_STR_LEN] = {0};
  uint32_t mark_val = 0;
  uint64_t byte_val = 0;

  if (addr == NULL)
  {
    LOG_MSG_INFO1("NULL addr passed",0,0,0);
    return -1;
  }

  snprintf(command, MAX_COMMAND_STR_LEN,
           "iptables -t mangle -L FORWARD -n -v -x  --line-numbers");
  strlcat(command, IPTABLES_AWK_REG_EXP_FORWARD, MAX_COMMAND_STR_LEN);
  snprintf(tmp, MAX_COMMAND_STR_LEN,
            " | grep -i %s", addr);
  strlcat(command, tmp, MAX_COMMAND_STR_LEN);
  memset(tmp,0,MAX_COMMAND_STR_LEN);
  snprintf(tmp, MAX_COMMAND_STR_LEN, " > %s", IPV4_PACK_STAT_IPTABLES_DUMP_FILE);
  strlcat(command, tmp, MAX_COMMAND_STR_LEN);

  FILE *access_fd = NULL;
  access_fd = fopen(IPV4_PACK_STAT_IPTABLES_SHELL_FILE,"w");
  /* File could not be opened for writing/append*/
  if (access_fd == NULL)
  {
    LOG_MSG_INFO1("could not open shell file",0,0,0);
    return -1;
  }
  fprintf( access_fd,"#!/bin/sh \n");
  fprintf( access_fd,"%s",command);
  fprintf(access_fd,"\n");
  fclose(access_fd);
  snprintf( command, MAX_COMMAND_STR_LEN,"chmod 755 %s",IPV4_PACK_STAT_IPTABLES_SHELL_FILE);
  ds_system_call( command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,IPV4_PACK_STAT_IPTABLES_SHELL_FILE);
  ds_system_call( command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"rm -f %s",IPV4_PACK_STAT_IPTABLES_SHELL_FILE);
  ds_system_call( command, strlen(command));


  fd = fopen(IPV4_PACK_STAT_IPTABLES_DUMP_FILE,"r");

  if(fd == NULL)
  {
    LOG_MSG_INFO1("Error in opening iptable dump file ",0,0,0);
    return -1;
  }

  while(fgets( stringline, bufferSize, fd) != NULL)
  {
    line_number = strtok_r(stringline, ",", &ptr);
    bytes = strtok_r(NULL, ",", &ptr);
    saddr = strtok_r(NULL, ",", &ptr);
    daddr = strtok_r(NULL, ",", &ptr);
    mark = strtok_r(NULL, ",", &ptr);

    if(mark != NULL)
      mark_val = strtoull(mark,&ptr,16);

    if(bytes != NULL)
      byte_val = strtoull(bytes,&ptr,10);

    if ((mark_val & MARK_MASK)  == MARK_BASE)
    {
      addr_found = 1;
      break;
    }
  }

  fclose(fd);
  snprintf(command,MAX_COMMAND_STR_LEN,"rm -rf %s",IPV4_PACK_STAT_IPTABLES_DUMP_FILE);
  ds_system_call(command, strlen(command));

  return addr_found;

}



/*=====================================================
  FUNCTION AddRuletoIptables
======================================================*/
/*!
@brief
  - Match the MAC address
  - If a match is found, check if given IP already exist if not add them to entry
@parameters
  pointer to ipaddress string
  Rule type ipv4 or ipv6
@return
  none
@note
- Dependencies
- packet stats need to be enabled
- Side Effects
- None
*/
/*=====================================================*/

void AddRuletoIptables(char* ip_addr, boolean rule_type)
{
  qmi_error_type_v01 qmi_err_num;
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWAN = GET_DEFAULT_BACKHAUL_WWAN_OBJECT();

  if (ip_addr == NULL)
  {
    LOG_MSG_ERROR("Null ip passed to iptables",0,0,0);
    return;
  }
  switch (rule_type)
  {
  case IPV4_RULE:

      if (QcMapBackhaulWWAN && (QcMapBackhaulWWAN->GetState() == QCMAP_CM_WAN_CONNECTED) &&
           (0==IsIPv4RuleAlreadyPresent(ip_addr)))
      {
        AddIpv4IPtableRule(ip_addr);
      }
      break;

    case IPV6_RULE:
      if (QcMapBackhaulWWAN && (QcMapBackhaulWWAN->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED) &&
          (0==IsIPv6RuleAlreadyPresent(ip_addr)))
      {
        AddIpv6IPtableRule(ip_addr);
      }
      break;

    default:
      break;
  }

}

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
int DeleteIPv4IptableRule(char* ip_addr)
{
  char command[MAX_COMMAND_STR_LEN]={0};
  unsigned int mark_tag_pool_ix = 0;
  unsigned int mark_hash = 0;
  char tmp[MAX_COMMAND_STR_LEN]={0};
  FILE *fd = NULL;
  FILE *access_fd = NULL;
  char *line_number = NULL,*bytes=NULL,*saddr=NULL, *daddr=NULL, *mark=NULL,*ptr;
  boolean addr_found = false;
  int bufferSize = MAX_COMMAND_STR_LEN;
  char stringline[MAX_COMMAND_STR_LEN] = {0};
  unsigned int line = 0;
  unsigned int mark_val = 0;
  uint64_t byte_val = 0;
  char mark_match[MAX_COMMAND_STR_LEN] = {0};

  if (ip_addr == NULL)
  {
    LOG_MSG_ERROR("NULL addr passed",0,0,0);
    return FAILURE;
  }

  snprintf(command, MAX_COMMAND_STR_LEN,
         "iptables -t mangle -L FORWARD -n -v -x  --line-numbers");
  strlcat(command, IPTABLES_AWK_REG_EXP_FORWARD, MAX_COMMAND_STR_LEN);

  snprintf(tmp, MAX_COMMAND_STR_LEN,
            " | grep -i %s", ip_addr);
  strlcat(command, tmp, MAX_COMMAND_STR_LEN);
  memset(tmp,0,MAX_COMMAND_STR_LEN);
  snprintf(tmp, MAX_COMMAND_STR_LEN, " > %s", IPV4_PACK_STAT_IPTABLES_DUMP_FILE);
  strlcat(command, tmp, MAX_COMMAND_STR_LEN);


  access_fd = fopen(IPV4_PACK_STAT_IPTABLES_SHELL_FILE,"w");
  /* File could not be opened for writing/append*/
  if (access_fd == NULL)
  {
    LOG_MSG_ERROR("could not open shell file",0,0,0);
    return FAILURE;
  }

  fprintf( access_fd,"#!/bin/sh \n");
  fprintf( access_fd,"%s",command);
  fprintf(access_fd,"\n");
  fclose(access_fd);
  snprintf( command, MAX_COMMAND_STR_LEN,"chmod 755 %s",IPV4_PACK_STAT_IPTABLES_SHELL_FILE);
  ds_system_call( command, strlen(command));

  for (int i = 0; i < 2; i++)
  {
    snprintf( command, MAX_COMMAND_STR_LEN,IPV4_PACK_STAT_IPTABLES_SHELL_FILE);
    ds_system_call( command, strlen(command));

    fd = fopen(IPV4_PACK_STAT_IPTABLES_DUMP_FILE,"r");
    if(fd == NULL)
    {
      LOG_MSG_ERROR("Error in opening iptable dump file ",0,0,0);
      return FAILURE;
    }

    while(fgets( stringline, bufferSize, fd) != NULL)
    {

      line_number = strtok_r(stringline, ",", &ptr);
      bytes = strtok_r(NULL, ",", &ptr);
      saddr = strtok_r(NULL, ",", &ptr);
      daddr = strtok_r(NULL, ",", &ptr);
      mark = strtok_r(NULL, ",", &ptr);
      memset(mark_match,0,MAX_COMMAND_STR_LEN);

      if(mark != NULL)
      {
        strlcat(mark_match, mark, MAX_COMMAND_STR_LEN);
        mark_val = strtoull(mark,&ptr,16);
      }
      //check if this is our rule..
      if ((mark_val & MARK_MASK)  == MARK_BASE)
      {
        if(line_number != NULL)
          line = strtoull(line_number, &ptr,10);

        if (mark_val%2 == 0)
        {
          memset(tmp,0,MAX_COMMAND_STR_LEN);
          snprintf(tmp, MAX_COMMAND_STR_LEN,
                   "iptables -t mangle -D FORWARD %d",line );
        }
        else
        {
          memset(tmp,0,MAX_COMMAND_STR_LEN);
          snprintf(tmp, MAX_COMMAND_STR_LEN,
                   "iptables -t mangle -D FORWARD %d",line );
        }
        DeleteStatsIPv4Rule(mark_match);
        ds_system_call( tmp, strlen(tmp));

        break;
      }
    }

    fclose(fd);
    snprintf(command,MAX_COMMAND_STR_LEN,"rm -rf %s",IPV4_PACK_STAT_IPTABLES_DUMP_FILE);
    ds_system_call(command, strlen(command));
  }

  snprintf( command, MAX_COMMAND_STR_LEN,"rm -f %s",IPV4_PACK_STAT_IPTABLES_SHELL_FILE);
  ds_system_call( command, strlen(command));
  return SUCCESS;
}

/*=====================================================
  FUNCTION DeleteStatsIPv6Rule
======================================================*/
/*!
@brief
  - Deletes a rule in ip6tables using line number
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
int DeleteStatsIPv6Rule(char* mark_match)
{
  char command[MAX_COMMAND_STR_LEN]={0};
  unsigned int mark_hash = 0;
  char tmp[MAX_COMMAND_STR_LEN]={0};
  FILE *fd = NULL;
  FILE *access_fd = NULL;
  char *line_number = NULL,*bytes=NULL,*saddr=NULL, *daddr=NULL, *mark=NULL,*ptr;
  boolean addr_found = false;
  int bufferSize = MAX_COMMAND_STR_LEN;
  char stringline[MAX_COMMAND_STR_LEN] = {0};
  unsigned int line = 0;
  unsigned int mark_val = 0;

  if (mark_match == NULL)
  {
    LOG_MSG_ERROR("NULL mark passed", 0, 0, 0);
    return FAILURE;
  }


  snprintf(command, MAX_COMMAND_STR_LEN,
         "ip6tables -t mangle -L TRAFFIC_ACCT -n -v -x  --line-numbers");
  strlcat(command, IP6TABLES_AWK_REG_EXP_STATS, MAX_COMMAND_STR_LEN);
  snprintf(tmp, MAX_COMMAND_STR_LEN,
            " | grep -i %s", mark_match);
  strlcat(command, tmp, MAX_COMMAND_STR_LEN);
  memset(tmp,0,MAX_COMMAND_STR_LEN);
  snprintf(tmp, MAX_COMMAND_STR_LEN, " > %s", IPV6_PACK_STAT_IPTABLES_STATS_DUMP_FILE);
  strlcat(command, tmp, MAX_COMMAND_STR_LEN);

  access_fd = fopen(IPV6_PACK_STAT_IPTABLES_STATS_SHELL_FILE,"w");
  /* File could not be opened for writing/append*/
  if (access_fd == NULL)
  {
    LOG_MSG_ERROR("could not open shell file",0,0,0);
    return FAILURE;
  }

  fprintf( access_fd,"#!/bin/sh \n");
  fprintf( access_fd,"%s",command);
  fprintf(access_fd,"\n");
  fclose(access_fd);
  snprintf( command, MAX_COMMAND_STR_LEN,"chmod 755 %s",IPV6_PACK_STAT_IPTABLES_STATS_SHELL_FILE);
  ds_system_call( command, strlen(command));

  snprintf( command, MAX_COMMAND_STR_LEN,IPV6_PACK_STAT_IPTABLES_STATS_SHELL_FILE);
  ds_system_call( command, strlen(command));

  fd = fopen(IPV6_PACK_STAT_IPTABLES_STATS_DUMP_FILE,"r");
  if(fd == NULL)
  {
    LOG_MSG_ERROR("Error in opening iptable dump file ",0,0,0);
    return FAILURE;
  }

  while(fgets( stringline, bufferSize, fd) != NULL)
  {
    line_number = strtok_r(stringline, ",", &ptr);
    bytes = strtok_r(NULL, ",", &ptr);
    saddr = strtok_r(NULL, ",", &ptr);
    daddr = strtok_r(NULL, ",", &ptr);
    mark = strtok_r(NULL, ",", &ptr);

    if(mark != NULL)
      mark_val = strtoull(mark,&ptr,16);
    //check if this is our rule..
    if ((mark_val & MARK_MASK)  == MARK_BASE)
    {
      if(line_number != NULL)
      {
        line = strtoull(line_number, &ptr,10);
      }
      if (mark_val%2 == 0)
      {
        memset(tmp,0,MAX_COMMAND_STR_LEN);
        snprintf(tmp, MAX_COMMAND_STR_LEN,
                 "ip6tables -t mangle -D TRAFFIC_ACCT %d",line );
      }
      else
      {
        memset(tmp,0,MAX_COMMAND_STR_LEN);
        snprintf(tmp, MAX_COMMAND_STR_LEN,
                 "ip6tables -t mangle -D TRAFFIC_ACCT %d",line );
      }
      ds_system_call( tmp, strlen(tmp));
      ipv6_tag_pool[mark_val - MARK_BASE] = 0;
      break;
    }
  }
  fclose(fd);
  snprintf(command,MAX_COMMAND_STR_LEN,"rm -rf %s",IPV6_PACK_STAT_IPTABLES_STATS_DUMP_FILE);
  ds_system_call(command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"rm -f %s",IPV6_PACK_STAT_IPTABLES_STATS_SHELL_FILE);
  ds_system_call( command, strlen(command));

  return SUCCESS;

}


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
int DeleteIPv6IptableRule(char* ip_addr)
{
  char command[MAX_COMMAND_STR_LEN]={0};
  unsigned int mark_hash = 0;
  char tmp[MAX_COMMAND_STR_LEN]={0};
  FILE *fd = NULL;
  FILE *access_fd = NULL;
  char *line_number = NULL,*bytes=NULL,*saddr=NULL, *daddr=NULL, *mark=NULL,*ptr;
  boolean addr_found = false;
  int bufferSize = MAX_COMMAND_STR_LEN;
  char stringline[MAX_COMMAND_STR_LEN] = {0};
  unsigned int line = 0;
  unsigned int mark_val = 0;
  uint64_t byte_val = 0;
  char mark_match[MAX_COMMAND_STR_LEN] = {0};

  if (ip_addr == NULL)
  {
    LOG_MSG_ERROR("NULL addr passed",0,0,0);
    return FAILURE;
  }

  snprintf(command, MAX_COMMAND_STR_LEN,
         "ip6tables -t mangle -L FORWARD -n -v -x  --line-numbers");
  strlcat(command, IP6TABLES_AWK_REG_EXP_FORWARD, MAX_COMMAND_STR_LEN);

  snprintf(tmp, MAX_COMMAND_STR_LEN,
            " | grep -i %s", ip_addr);
  strlcat(command, tmp, MAX_COMMAND_STR_LEN);
  memset(tmp,0,MAX_COMMAND_STR_LEN);
  snprintf(tmp, MAX_COMMAND_STR_LEN, " > %s", IPV6_PACK_STAT_IPTABLES_DUMP_FILE);
  strlcat(command, tmp, MAX_COMMAND_STR_LEN);


  access_fd = fopen(IPV6_PACK_STAT_IPTABLES_SHELL_FILE,"w");
  /* File could not be opened for writing/append*/
  if (access_fd == NULL)
  {
    LOG_MSG_ERROR("could not open shell file",0,0,0);
    return FAILURE;
  }

  fprintf( access_fd,"#!/bin/sh \n");
  fprintf( access_fd,"%s",command);
  fprintf(access_fd,"\n");
  fclose(access_fd);
  snprintf( command, MAX_COMMAND_STR_LEN,"chmod 755 %s",IPV6_PACK_STAT_IPTABLES_SHELL_FILE);
  ds_system_call( command, strlen(command));

  for (int i = 0; i < 2; i++)
  {
    snprintf( command, MAX_COMMAND_STR_LEN,IPV6_PACK_STAT_IPTABLES_SHELL_FILE);
    ds_system_call( command, strlen(command));

    fd = fopen(IPV6_PACK_STAT_IPTABLES_DUMP_FILE,"r");
    if(fd == NULL)
    {
      LOG_MSG_ERROR("Error in opening iptable dump file ",0,0,0);
      return FAILURE;
    }

    while(fgets( stringline, bufferSize, fd) != NULL)
    {

      line_number = strtok_r(stringline, ",", &ptr);
      bytes = strtok_r(NULL, ",", &ptr);
      saddr = strtok_r(NULL, ",", &ptr);
      daddr = strtok_r(NULL, ",", &ptr);
      mark = strtok_r(NULL, ",", &ptr);
      memset(mark_match,0,MAX_COMMAND_STR_LEN);
      if(mark)
      {
        strlcat(mark_match, mark, MAX_COMMAND_STR_LEN);
        mark_val = strtoull(mark,&ptr,16);
      }
      //check if this is our rule..
      if ((mark_val & MARK_MASK)  == MARK_BASE)
      {
        if(line_number !=NULL)
        {
          line = strtoull(line_number, &ptr,10);
        }
        if (mark_val%2 == 0)
        {
          memset(tmp,0,MAX_COMMAND_STR_LEN);
          snprintf(tmp, MAX_COMMAND_STR_LEN,
                   "ip6tables -t mangle -D FORWARD %d",line );
        }
        else
        {
          memset(tmp,0,MAX_COMMAND_STR_LEN);
          snprintf(tmp, MAX_COMMAND_STR_LEN,
                   "ip6tables -t mangle -D FORWARD %d",line );
        }
        DeleteStatsIPv6Rule(mark_match);
        ds_system_call( tmp, strlen(tmp));

        break;
      }
    }

    fclose(fd);
    snprintf(command,MAX_COMMAND_STR_LEN,"rm -rf %s",IPV6_PACK_STAT_IPTABLES_DUMP_FILE);
    ds_system_call(command, strlen(command));
  }

  snprintf( command, MAX_COMMAND_STR_LEN,"rm -f %s",IPV6_PACK_STAT_IPTABLES_SHELL_FILE);
  ds_system_call( command, strlen(command));
  return SUCCESS;
}

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
int AddIpv6IPtableRule(char* addr)
{
  char command[MAX_COMMAND_STR_LEN]={0};
  unsigned int mark_tag_pool_ix = 0;
  unsigned int mark_hash = 0;
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int qcmap_cm_error;
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=GET_DEFAULT_BACKHAUL_WWAN_OBJECT();


  if (addr == NULL)
  {
    LOG_MSG_ERROR("NULL addr passed",0,0,0);
    return FAILURE;
  }

  //get mark value from mark tag pool
  for (mark_tag_pool_ix = 0; mark_tag_pool_ix < MAX_MARK_VALUE; mark_tag_pool_ix++)
  {
    if(!ipv6_tag_pool[mark_tag_pool_ix])
    {
      break;
    }
  }
  if (mark_tag_pool_ix+1 >= MAX_MARK_VALUE)
  {
    LOG_MSG_ERROR("Pool Tag ran out of memory",0,0,0);
    return FAILURE;
  }

  if(!QcMapBackhaulWWANMgr)
  {
    LOG_MSG_ERROR("AddIpv6IPtableRule: Null QcMapBackhaulWWANMgr pointer",0,0,0);
    return false;
  }

  if(QcMapBackhaulWWANMgr->GetDeviceName(QCMAP_ConnectionManager::qcmap_cm_handle,
                       QCMAP_MSGR_IP_FAMILY_V6_V01,
                       devname, &qcmap_cm_error) != QCMAP_CM_SUCCESS )
  {
    LOG_MSG_ERROR("Couldn't get ipv6 rmnet name. error %d\n", qcmap_cm_error,0,0);
    return FAILURE;
  }

  ipv6_tag_pool[mark_tag_pool_ix] = 1; // for UL rule
  ipv6_tag_pool[mark_tag_pool_ix+1] = 1; // for DL rule

  mark_hash = mark_tag_pool_ix+MARK_BASE;


  snprintf(command, MAX_COMMAND_STR_LEN,
           "ip6tables -t mangle -A FORWARD  -s %s -o %s -j MARK --set-xmark 0x%X", addr,devname,mark_hash);
  ds_system_call( command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,
           "ip6tables -t mangle -A FORWARD  -d %s -i %s -j MARK --set-xmark 0x%X", addr,devname,mark_hash+1);
  ds_system_call( command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
           "ip6tables -t mangle  -A TRAFFIC_ACCT -m mark --mark %d -s %s -j ACCEPT",
           mark_hash,addr);
  ds_system_call( command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,
           "ip6tables -t mangle  -A TRAFFIC_ACCT -m mark --mark %d -d %s -j ACCEPT",
           mark_hash+1,addr);
  ds_system_call( command, strlen(command));
  return SUCCESS;
}


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
int IsIPv6RuleAlreadyPresent(char* addr)
{
  char command[MAX_COMMAND_STR_LEN]={0};
  char tmp[MAX_COMMAND_STR_LEN]={0};
  FILE *fd = NULL;
  char *bytes = NULL,*line_number = NULL,*saddr=NULL, *daddr=NULL, *mark=NULL,*ptr;
  int rule_found = 0;
  int bufferSize = MAX_COMMAND_STR_LEN;
  char stringline[MAX_COMMAND_STR_LEN] = {0};
  uint32_t mark_val = 0;
  uint64_t byte_val = 0;

  if (addr == NULL)
  {
    LOG_MSG_ERROR("NULL addr passed",0,0,0);
    return FAILURE;
  }

  snprintf(command, MAX_COMMAND_STR_LEN,
           "ip6tables -t mangle -L FORWARD -n -v -x  --line-numbers");
  strlcat(command, IP6TABLES_AWK_REG_EXP_FORWARD, MAX_COMMAND_STR_LEN);
  snprintf(tmp, MAX_COMMAND_STR_LEN,
            " | grep -i %s", addr);
  strlcat(command, tmp, MAX_COMMAND_STR_LEN);
  memset(tmp,0,MAX_COMMAND_STR_LEN);
  snprintf(tmp, MAX_COMMAND_STR_LEN, " > %s", IPV6_PACK_STAT_IPTABLES_DUMP_FILE);
  strlcat(command, tmp, MAX_COMMAND_STR_LEN);

  FILE *access_fd = NULL;
  access_fd = fopen(IPV6_PACK_STAT_IPTABLES_SHELL_FILE,"w");
  /* File could not be opened for writing/append*/
  if (access_fd == NULL)
  {
    LOG_MSG_ERROR("could not open shell file",0,0,0);
    return FAILURE;
  }
  fprintf( access_fd,"#!/bin/sh \n");
  fprintf( access_fd,"%s",command);
  fprintf(access_fd,"\n");
  fclose(access_fd);
  snprintf( command, MAX_COMMAND_STR_LEN,"chmod 755 %s",IPV6_PACK_STAT_IPTABLES_SHELL_FILE);
  ds_system_call( command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,IPV6_PACK_STAT_IPTABLES_SHELL_FILE);
  ds_system_call( command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"rm -f %s",IPV6_PACK_STAT_IPTABLES_SHELL_FILE);
  ds_system_call( command, strlen(command));


  fd = fopen(IPV6_PACK_STAT_IPTABLES_DUMP_FILE,"r");

  if(fd == NULL)
  {
    LOG_MSG_ERROR("Error in opening iptable dump file ",0,0,0);
    return FAILURE;
  }

  while(fgets( stringline, bufferSize, fd) != NULL)
  {
    line_number = strtok_r(stringline, ",", &ptr);
    bytes = strtok_r(NULL, ",", &ptr);
    saddr = strtok_r(NULL, ",", &ptr);
    daddr = strtok_r(NULL, ",", &ptr);
    mark = strtok_r(NULL, ",", &ptr);
    if(mark)
      mark_val = strtoull(mark,&ptr,16);
    if ((mark_val & MARK_MASK)  == MARK_BASE)
    {
      rule_found = 1;
      break;
    }
  }

  fclose(fd);
  snprintf(command,MAX_COMMAND_STR_LEN,"rm -rf %s",IPV6_PACK_STAT_IPTABLES_DUMP_FILE);
  ds_system_call(command, strlen(command));

  return rule_found;

}

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
void UpdateIPV4ExceptionStats(qcmap_cm_client_data_info_t* cdiNode, char* ipv4addr)
{
  char command[MAX_COMMAND_STR_LEN]={0};
  char tmp[MAX_COMMAND_STR_LEN]={0};
  int bufferSize = MAX_COMMAND_STR_LEN;
  char stringline[MAX_COMMAND_STR_LEN] = {0};
  uint64_t rx_data = 0;
  uint64_t tx_data = 0;
  FILE *fd = NULL;
  FILE *access_fd = NULL;
  char *line_number = NULL,*bytes=NULL,*saddr=NULL, *daddr=NULL, *mark=NULL,*ptr;
  unsigned int mark_val = 0;
  unsigned int line = 0;
  char mac_addr_string[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];/*char array of mac addr*/

  ds_mac_addr_ntop(cdiNode->mac_addr, mac_addr_string);

  snprintf(command, MAX_COMMAND_STR_LEN,
         "iptables -t mangle -L TRAFFIC_ACCT -n -v -x  --line-numbers");
  strlcat(command, IPTABLES_AWK_REG_EXP_STATS, MAX_COMMAND_STR_LEN);

  snprintf(tmp, MAX_COMMAND_STR_LEN,
           " | grep -i %s", ipv4addr);
  strlcat(command, tmp, MAX_COMMAND_STR_LEN);
  memset(tmp,0,MAX_COMMAND_STR_LEN);
  snprintf(tmp, MAX_COMMAND_STR_LEN, " > %s", IPV4_PACK_STAT_IPTABLES_DUMP_FILE);
  strlcat(command, tmp, MAX_COMMAND_STR_LEN);


  access_fd = fopen(IPV4_PACK_STAT_IPTABLES_SHELL_FILE,"w");
  /* File could not be opened for writing/append*/
  if (access_fd == NULL)
  {
    LOG_MSG_ERROR("could not open shell file",0,0,0);
    return;
  }

  fprintf( access_fd,"#!/bin/sh \n");
  fprintf( access_fd,"%s",command);
  fprintf(access_fd,"\n");
  fclose(access_fd);
  snprintf( command, MAX_COMMAND_STR_LEN,"chmod 755 %s",IPV4_PACK_STAT_IPTABLES_SHELL_FILE);
  ds_system_call( command, strlen(command));

  snprintf( command, MAX_COMMAND_STR_LEN,IPV4_PACK_STAT_IPTABLES_SHELL_FILE);
  ds_system_call( command, strlen(command));

  fd = fopen(IPV4_PACK_STAT_IPTABLES_DUMP_FILE,"r");
  if(fd == NULL)
  {
    LOG_MSG_ERROR("Error in opening iptable dump file ",0,0,0);
    return;
  }

  while(fgets( stringline, bufferSize, fd) != NULL)
  {
    line_number = strtok_r(stringline, ",", &ptr);
    bytes = strtok_r(NULL, ",", &ptr);
    saddr = strtok_r(NULL, ",", &ptr);
    daddr = strtok_r(NULL, ",", &ptr);
    mark = strtok_r(NULL, ",", &ptr);
    if(mark != NULL)
    {
      mark_val = strtoull(mark,&ptr,16);
    }
    if ((mark_val & MARK_MASK)  == MARK_BASE)
    {
      if(line_number != NULL)
      {
        line = strtoull(line_number, &ptr,10);
      }
      if( bytes !=NULL )
      {
        if (mark_val%2 == 0)
        {
          tx_data = strtoull(bytes,&ptr,10);
        }
        else
        {
          rx_data = strtoull(bytes,&ptr,10);
        }
      }
    }
  }

  cdiNode->exception_data.rx_data += rx_data;
  cdiNode->exception_data.tx_data += tx_data;
  LOG_MSG_INFO1("IPv4 Exception Stats: RX: %llu TX: %llu", cdiNode->exception_data.rx_data, cdiNode->exception_data.tx_data, 0);
  fclose(fd);
}



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
void UpdateIPV6ExceptionStats(qcmap_cm_client_data_info_t* cdiNode, char* ipv6addr)
{
  char command[MAX_COMMAND_STR_LEN]={0};
  char tmp[MAX_COMMAND_STR_LEN]={0};
  int bufferSize = MAX_COMMAND_STR_LEN;
  char stringline[MAX_COMMAND_STR_LEN] = {0};
  uint64_t rx_data = 0;
  uint64_t tx_data = 0;
  FILE *fd = NULL;
  FILE *access_fd = NULL;
  char *line_number = NULL,*bytes=NULL,*saddr=NULL, *daddr=NULL, *mark=NULL,*ptr;
  unsigned int mark_val = 0;
  unsigned int line = 0;
  char mac_addr_string[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];/*char array of mac addr*/

  ds_mac_addr_ntop(cdiNode->mac_addr, mac_addr_string);

  snprintf(command, MAX_COMMAND_STR_LEN,
         "ip6tables -t mangle -L TRAFFIC_ACCT -n -v -x  --line-numbers");
  strlcat(command, IP6TABLES_AWK_REG_EXP_STATS, MAX_COMMAND_STR_LEN);

  snprintf(tmp, MAX_COMMAND_STR_LEN,
           " | grep -i %s", ipv6addr);
  strlcat(command, tmp, MAX_COMMAND_STR_LEN);
  memset(tmp,0,MAX_COMMAND_STR_LEN);
  snprintf(tmp, MAX_COMMAND_STR_LEN, " > %s", IPV6_PACK_STAT_IPTABLES_DUMP_FILE);
  strlcat(command, tmp, MAX_COMMAND_STR_LEN);


  access_fd = fopen(IPV6_PACK_STAT_IPTABLES_SHELL_FILE,"w");
  /* File could not be opened for writing/append*/
  if (access_fd == NULL)
  {
    LOG_MSG_ERROR("could not open shell file",0,0,0);
    return;
  }

  fprintf( access_fd,"#!/bin/sh \n");
  fprintf( access_fd,"%s",command);
  fprintf(access_fd,"\n");
  fclose(access_fd);
  snprintf( command, MAX_COMMAND_STR_LEN,"chmod 755 %s",IPV6_PACK_STAT_IPTABLES_SHELL_FILE);
  ds_system_call( command, strlen(command));

  snprintf( command, MAX_COMMAND_STR_LEN,IPV6_PACK_STAT_IPTABLES_SHELL_FILE);
  ds_system_call( command, strlen(command));

  fd = fopen(IPV6_PACK_STAT_IPTABLES_DUMP_FILE,"r");
  if(fd == NULL)
  {
    LOG_MSG_ERROR("Error in opening iptable dump file ", 0, 0, 0);
    return;
  }

  while(fgets( stringline, bufferSize, fd) != NULL)
  {
    line_number = strtok_r(stringline, ",", &ptr);
    bytes = strtok_r(NULL, ",", &ptr);
    saddr = strtok_r(NULL, ",", &ptr);
    daddr = strtok_r(NULL, ",", &ptr);
    mark = strtok_r(NULL, ",", &ptr);
    if( mark !=NULL)
      mark_val = strtoull(mark,&ptr,16);

    if ((mark_val & MARK_MASK)  == MARK_BASE)
    {
      if(line_number !=NULL)
      {
        line = strtoull(line_number, &ptr,10);
      }
      if( bytes != NULL)
      {
        if (mark_val%2 == 0)
        {
          tx_data = strtoull(bytes,&ptr,10);
        }
        else
        {
          rx_data = strtoull(bytes,&ptr,10);
        }
      }
    }
  }

  LOG_MSG_INFO1("IPv6 Exception Stats: RX: %llu TX: %llu", rx_data, tx_data, 0);
  cdiNode->exception_data.rx_data += rx_data;
  cdiNode->exception_data.tx_data += tx_data;
  LOG_MSG_INFO1("Exception Stats: RX: %llu TX: %llu", cdiNode->exception_data.rx_data, cdiNode->exception_data.tx_data, 0);
  close(fd);
}

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
int DeleteStatsIPv4Rule(char* mark_match)
{
  char command[MAX_COMMAND_STR_LEN]={0};
  unsigned int mark_tag_pool_ix = 0;
  unsigned int mark_hash = 0;
  char tmp[MAX_COMMAND_STR_LEN]={0};
  FILE *fd = NULL;
  FILE *access_fd = NULL;
  char *line_number = NULL,*bytes=NULL,*saddr=NULL, *daddr=NULL, *mark=NULL,*ptr;
  boolean addr_found = false;
  int bufferSize = MAX_COMMAND_STR_LEN;
  char stringline[MAX_COMMAND_STR_LEN] = {0};
  unsigned int line = 0;
  unsigned int mark_val = 0;

  if (mark_match == NULL)
  {
    LOG_MSG_ERROR("NULL mark passed",0,0,0);
    return FAILURE;
  }


  snprintf(command, MAX_COMMAND_STR_LEN,
         "iptables -t mangle -L TRAFFIC_ACCT -n -v -x  --line-numbers");
  strlcat(command, IPTABLES_AWK_REG_EXP_STATS, MAX_COMMAND_STR_LEN);
  snprintf(tmp, MAX_COMMAND_STR_LEN,
            " | grep -i %s", mark_match);
  strlcat(command, tmp, MAX_COMMAND_STR_LEN);
  memset(tmp,0,MAX_COMMAND_STR_LEN);
  snprintf(tmp, MAX_COMMAND_STR_LEN, " > %s", IPV4_PACK_STAT_IPTABLES_STATS_DUMP_FILE);
  strlcat(command, tmp, MAX_COMMAND_STR_LEN);

  access_fd = fopen(IPV4_PACK_STAT_IPTABLES_STATS_SHELL_FILE,"w");
  /* File could not be opened for writing/append*/
  if (access_fd == NULL)
  {
    LOG_MSG_ERROR("could not open shell file",0,0,0);
    return FAILURE;
  }

  fprintf( access_fd,"#!/bin/sh \n");
  fprintf( access_fd,"%s",command);
  fprintf(access_fd,"\n");
  fclose(access_fd);
  snprintf( command, MAX_COMMAND_STR_LEN,"chmod 755 %s",IPV4_PACK_STAT_IPTABLES_STATS_SHELL_FILE);
  ds_system_call( command, strlen(command));

  snprintf( command, MAX_COMMAND_STR_LEN,IPV4_PACK_STAT_IPTABLES_STATS_SHELL_FILE);
  ds_system_call( command, strlen(command));

  fd = fopen(IPV4_PACK_STAT_IPTABLES_STATS_DUMP_FILE,"r");
  if(fd == NULL)
  {
    LOG_MSG_ERROR("Error in opening iptable dump file ",0,0,0);
    return FAILURE;
  }

  while(fgets( stringline, bufferSize, fd) != NULL)
  {
    line_number = strtok_r(stringline, ",", &ptr);
    bytes = strtok_r(NULL, ",", &ptr);
    saddr = strtok_r(NULL, ",", &ptr);
    daddr = strtok_r(NULL, ",", &ptr);
    mark = strtok_r(NULL, ",", &ptr);
    if( mark != NULL)
      mark_val = strtoull(mark,&ptr,16);
    //check if this is our rule..
    if ((mark_val & MARK_MASK)  == MARK_BASE)
    {
      if(line_number != NULL)
        line = strtoull(line_number, &ptr,10);
      if (mark_val%2 == 0)
      {
        memset(tmp,0,MAX_COMMAND_STR_LEN);
        snprintf(tmp, MAX_COMMAND_STR_LEN,
                 "iptables -t mangle -D TRAFFIC_ACCT %d",line );
      }
      else
      {
        memset(tmp,0,MAX_COMMAND_STR_LEN);
        snprintf(tmp, MAX_COMMAND_STR_LEN,
                 "iptables -t mangle -D TRAFFIC_ACCT %d",line );
      }
      ds_system_call( tmp, strlen(tmp));
      ipv4_tag_pool[mark_val - MARK_BASE] = 0;
      break;
    }
  }
  fclose(fd);
  snprintf(command,MAX_COMMAND_STR_LEN,"rm -rf %s",IPV4_PACK_STAT_IPTABLES_STATS_DUMP_FILE);
  ds_system_call(command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,"rm -f %s",IPV4_PACK_STAT_IPTABLES_STATS_SHELL_FILE);
  ds_system_call( command, strlen(command));

  return SUCCESS;

}
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
void NotifyIPACM(struct wan_ioctl_send_lan_client_msg  *ioctl_buffer)
{
  int ret;
   //open fd for IPACM ioctl communication
  int ipa_ioctl_fd = open(IPA_IOCTL_DEVICE_NAME, O_RDWR);

  if(ipa_ioctl_fd < 0)
  {
    LOG_MSG_INFO3("Failed to open IPA device node: %d\n",errno, 0, 0);
    return;
  }

  LOG_MSG_INFO1("Sending Event to IPA: %d, Iface %s", ioctl_buffer->client_event, ioctl_buffer->lan_client.lanIface, 0);
  ret = ioctl(ipa_ioctl_fd, WAN_IOC_SEND_LAN_CLIENT_MSG, ioctl_buffer);
  if (ret != 0)
  {
    LOG_MSG_INFO3("Failed to send WAN_IOC_SEND_LAN_CLIENT_MSG: %d\n", errno, 0, 0);
    close(ipa_ioctl_fd);
    return;
  }
  close(ipa_ioctl_fd);
}

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
void UpdateIPAStats(struct wan_ioctl_query_per_client_stats  *ioctl_buffer)
{
  int ret, retries = 0;
   //open fd for IPACM ioctl communication
  int ipa_ioctl_fd = open(IPA_IOCTL_DEVICE_NAME, O_RDWR);

  if(ipa_ioctl_fd < 0)
  {
    LOG_MSG_INFO3("Failed to open IPA device node: %d\n",errno, 0, 0);
    return;
  }

LOG_MSG_INFO1("Send WAN_IOC_QUERY_PER_CLIENT_STATS: num_clients: %d, Type: %d, Disconnect flag: %d\n",
   ioctl_buffer->num_clients, ioctl_buffer->device_type,
   ioctl_buffer->disconnect_clnt);
LOG_MSG_INFO1("Send WAN_IOC_QUERY_PER_CLIENT_STATS: reset flag: %d\n",
   ioctl_buffer->reset_stats, 0, 0);
ioctl_exec:
  ret = ioctl(ipa_ioctl_fd, WAN_IOC_QUERY_PER_CLIENT_STATS, ioctl_buffer);
  if (ret != 0)
  {
    LOG_MSG_INFO3("Failed to fetch and reset IPA stats: %d\n", errno, 0, 0);
    if (ioctl_buffer->disconnect_clnt && errno == EAGAIN)
    {
      retries++;
      if (retries < QCMAP_PER_CLIENT_STATS_DISCONNECT_RETRY)
      {
        sleep(QCMAP_PER_CLIENT_STATS_DISCONNECT_RETRY_TIMEOUT);
        goto ioctl_exec;
      }
    }
  }
  close(ipa_ioctl_fd);
}
#endif
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
void ResetSfeDataStats(void)
{
  SendNLMsgToSFE(sfev6_nl_sock_fd, SFE_IPV6_RESET_PACKET_STATS_COUNTERS, NULL);
  SendNLMsgToSFE(sfev4_nl_sock_fd, SFE_IPV4_RESET_PACKET_STATS_COUNTERS, NULL);
}


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
void UpdateSFEStats(qcmap_cm_client_data_info_t* cdiNode)
{
  qcmap_cm_sfe_ipv4_dump_t *sfe_ipv4_dump = NULL;
  qcmap_cm_sfe_ipv6_dump_t *sfe_ipv6_dump = NULL;
  char command[MAX_COMMAND_STR_LEN]={0};
  char tmp[MAX_COMMAND_STR_LEN]={0};
  uint32_t ipv4_entries_num = 0;
  uint32_t ipv6_entries_num = 0;
  uint64_t rx_data = 0;
  uint64_t tx_data = 0;
  uint8 zero_buff[QCMAP_MSGR_IPV6_ADDR_LEN_V01] = {0};
  char mac_addr_string[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];/*char array of mac addr*/

  ds_mac_addr_ntop(cdiNode->mac_addr, mac_addr_string);

  snprintf( command, MAX_COMMAND_STR_LEN,IPV4_PACK_STAT_SFE_SHELL_FILE);
  snprintf(tmp, MAX_COMMAND_STR_LEN, " > %s", IPV4_PACK_STAT_SFE_XML_FILE);
  strlcat(command, tmp, MAX_COMMAND_STR_LEN);
  ds_system_call( command, strlen(command));

  snprintf( command, MAX_COMMAND_STR_LEN,IPV6_PACK_STAT_SFE_SHELL_FILE);
  snprintf(tmp, MAX_COMMAND_STR_LEN, " > %s", IPV6_PACK_STAT_SFE_XML_FILE);
  strlcat(command, tmp, MAX_COMMAND_STR_LEN);
  ds_system_call( command, strlen(command));

  sfe_ipv4_dump = extractSFEv4data(IPV4_PACK_STAT_SFE_XML_FILE,&ipv4_entries_num);
  sfe_ipv6_dump = extractSFEv6data(IPV6_PACK_STAT_SFE_XML_FILE,&ipv6_entries_num);

  if (cdiNode != NULL)
  {
    if (sfe_ipv4_dump != NULL)
    {
      for (int i = 0; i < ipv4_entries_num; i++)
      {
        if (cdiNode->ip_addr == sfe_ipv4_dump[i].sa.sin_addr.s_addr)
        {
          rx_data += sfe_ipv4_dump[i].rx_data;
          tx_data += sfe_ipv4_dump[i].tx_data;
          break;
        }
      }
      ds_free(sfe_ipv4_dump);
    }
    if (sfe_ipv6_dump !=NULL)
    {
      for (int j = 0; j < QCMAP_MSGR_MAX_IPV6_ADDR_V01; j++)
      {
        // if no more ipv6 left for CDI, break the loop
        if (!memcmp(cdiNode->ipv6[j].addr, zero_buff,QCMAP_MSGR_IPV6_ADDR_LEN_V01))
          break;
        // iterate thru all sfe nodes and check if we have a match for cdi ipv6
        for (int i = 0; i < ipv6_entries_num; i++)
        {
          if (!memcmp(cdiNode->ipv6[j].addr, &sfe_ipv6_dump[i].sa.sin6_addr, sizeof(struct in6_addr)))
          {
             rx_data += sfe_ipv6_dump[i].rx_data;
             tx_data += sfe_ipv6_dump[i].tx_data;
             break;
          }
        }
      }
      ds_free(sfe_ipv6_dump);
    }

    cdiNode->sfe_data.rx_data = rx_data;
    cdiNode->sfe_data.tx_data = tx_data;
    LOG_MSG_INFO1("SFE Stats: RX: %llu TX: %llu", cdiNode->sfe_data.rx_data, cdiNode->sfe_data.tx_data, 0);
  }
  else
  {
    if (sfe_ipv4_dump != NULL)
    {
      ds_free(sfe_ipv4_dump);
    }

    if (sfe_ipv6_dump != NULL)
    {
      ds_free(sfe_ipv6_dump);
    }
  }

}

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
qcmap_cm_sfe_ipv4_dump_t* extractSFEv4data(char* file_path, uint32_t * num_entries)
{
  pugi::xml_node root, child, conn_root, conn_child;
  pugi::xml_attribute attr;
  pugi::xml_document xml_file;
  char* ptr = NULL;
  int i = 0;
  char command[MAX_COMMAND_STR_LEN]={0};
  qcmap_cm_sfe_ipv4_dump_t* node = NULL;

  if (!xml_file.load_file(file_path))
  {
    LOG_MSG_ERROR("error loading XML file",0,0,0);
    return NULL;
  }

  root = xml_file.child(PACKET_STATS_TAG);

  if (root)
  {
    for (child = root.first_child(); child; child = child.next_sibling())
    {
      if (!strcmp(child.name(), STATS_TAG))
      {
        //print its attribute..
        for (attr = child.first_attribute(); attr; attr = attr.next_attribute())
        {
          *num_entries = atoi(attr.value());
        }
      }
      if (*num_entries > 0)
      {
        node = (qcmap_cm_sfe_ipv4_dump_t*)ds_malloc(sizeof(qcmap_cm_sfe_ipv4_dump_t)*(*num_entries));
        if (node == NULL)
        {
          LOG_MSG_ERROR("cannot allocate memory",0,0,0);
          return NULL;
        }
      }
      else
      {
        return NULL;
      }

      if (!strcmp(child.name(), CONNECTIONS_TAG))
      {
        conn_root = child;
        for (conn_child = conn_root.first_child(), i = 0; conn_child; conn_child = conn_child.next_sibling(),i++)
        {
          //print its attribute..
          for (attr = conn_child.first_attribute(); attr; attr = attr.next_attribute())
          {
            if (!strcmp(attr.name(), CLIENT_ADDR_ATTR_TAG))
            {
              inet_pton(AF_INET, attr.value(), &(node[i].sa.sin_addr));
            }

            if (!strcmp(attr.name(), RX_BYTE_ATTR_TAG))
            {
              node[i].rx_data = strtoull(attr.value(), &ptr, 10);
            }

            if (!strcmp(attr.name(), TX_BYTE_ATTR_TAG))
            {
              node[i].tx_data = strtoull(attr.value(), &ptr, 10);
            }
          }
        }
      }
    }
  }

  snprintf(command,MAX_COMMAND_STR_LEN,"rm -rf %s",
           file_path);
  ds_system_call(command, strlen(command));
  return node;
}

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
qcmap_cm_sfe_ipv6_dump_t* extractSFEv6data(char* file_path, uint32_t * num_entries)
{
  pugi::xml_node root, child, conn_root, conn_child;
  pugi::xml_attribute attr;
  pugi::xml_document xml_file;
  int i =0;
  char* ptr = NULL;
  char command[MAX_COMMAND_STR_LEN]={0};
  qcmap_cm_sfe_ipv6_dump_t* node = NULL;


  if (!xml_file.load_file(file_path))
  {
    LOG_MSG_ERROR("error loading XML file",0,0,0);
    return NULL;
  }

  root = xml_file.child(PACKET_STATS_TAG);

  if (root)
  {
    for (child = root.first_child(); child; child = child.next_sibling())
    {
      if (!strcmp(child.name(), STATS_TAG))
      {
        //print its attribute..
        for (attr = child.first_attribute(); attr; attr = attr.next_attribute())
        {
          *num_entries = atoi(attr.value());
        }
      }

      if (*num_entries > 0)
      {
        node = (qcmap_cm_sfe_ipv6_dump_t*)ds_malloc(sizeof(qcmap_cm_sfe_ipv6_dump_t)*(*num_entries));

        if (node == NULL)
        {
          LOG_MSG_ERROR("cannot allocate memory",0,0,0);
          return NULL;
        }
      }
      else
      {
        return NULL;
      }

      if (!strcmp(child.name(), CONNECTIONS_TAG))
      {
        conn_root = child;
        for (conn_child = conn_root.first_child(),i = 0; conn_child; conn_child = conn_child.next_sibling(),i++)
        {
          //print its attribute..
          for (attr = conn_child.first_attribute(); attr; attr = attr.next_attribute())
          {
            if (node != NULL)
            {
              if (!strcmp(attr.name(), CLIENT_ADDR_ATTR_TAG))
              {
                inet_pton(AF_INET6, attr.value(), &node[i].sa.sin6_addr);
              }

              if (!strcmp(attr.name(), RX_BYTE_ATTR_TAG))
              {
                node[i].rx_data = strtoull(attr.value(), &ptr, 10);
              }

              if (!strcmp(attr.name(), TX_BYTE_ATTR_TAG))
              {
                node[i].tx_data = strtoull(attr.value(), &ptr, 10);
              }
            }
          }
        }
      }
    }
  }
  snprintf(command,MAX_COMMAND_STR_LEN,"rm -rf %s",
           file_path);
  ds_system_call(command, strlen(command));
  return node;
}



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

int createSFEDumpFile(void)
{
  char command[MAX_COMMAND_STR_LEN]={0};
  FILE *access_fd = NULL;

  access_fd = fopen(IPV4_PACK_STAT_SFE_SHELL_FILE,"w");
  /* File could not be opened for writing/append*/
  if (access_fd == NULL)
  {
    LOG_MSG_ERROR("could not open shell file",0,0,0);
    return FAILURE;
  }
  fprintf( access_fd,"#!/bin/sh \n");
  snprintf(command, MAX_COMMAND_STR_LEN,
           SFE_IPV4_DUMP_TEXT1);
  fprintf( access_fd,"%s",command);
  memset(command,0,MAX_COMMAND_STR_LEN);
  snprintf(command, MAX_COMMAND_STR_LEN,
           SFE_IPV4_DUMP_TEXT2);
  fprintf( access_fd,"%s",command);
  memset(command,0,MAX_COMMAND_STR_LEN);
  snprintf(command, MAX_COMMAND_STR_LEN,
           SFE_IPV4_DUMP_TEXT3);
  fprintf( access_fd,"%s",command);
  memset(command,0,MAX_COMMAND_STR_LEN);
  snprintf(command, MAX_COMMAND_STR_LEN,
           SFE_IPV4_DUMP_TEXT4);
  fprintf( access_fd,"%s",command);
  memset(command,0,MAX_COMMAND_STR_LEN);
  snprintf(command, MAX_COMMAND_STR_LEN,
           SFE_IPV4_DUMP_TEXT5);
  fprintf( access_fd,"%s",command);
  memset(command,0,MAX_COMMAND_STR_LEN);
  snprintf(command, MAX_COMMAND_STR_LEN,
           SFE_IPV4_DUMP_TEXT6);
  fprintf( access_fd,"%s",command);
  memset(command,0,MAX_COMMAND_STR_LEN);
  snprintf(command, MAX_COMMAND_STR_LEN,
           SFE_IPV4_DUMP_TEXT7);
  fprintf( access_fd,"%s",command);
  memset(command,0,MAX_COMMAND_STR_LEN);
  snprintf(command, MAX_COMMAND_STR_LEN,
           SFE_IPV4_DUMP_TEXT8);
  fprintf( access_fd,"%s",command);
  memset(command,0,MAX_COMMAND_STR_LEN);
  fclose(access_fd);
  memset(command,0,MAX_COMMAND_STR_LEN);
  snprintf( command, MAX_COMMAND_STR_LEN,"chmod 755 %s",
            IPV4_PACK_STAT_SFE_SHELL_FILE);
  ds_system_call( command, strlen(command));


  access_fd = fopen(IPV6_PACK_STAT_SFE_SHELL_FILE,"w");
  /* File could not be opened for writing/append*/
  if (access_fd == NULL)
  {
    LOG_MSG_ERROR("could not open shell file",0,0,0);
    return FAILURE;
  }
  fprintf( access_fd,"#!/bin/sh \n");
  snprintf(command, MAX_COMMAND_STR_LEN,
           SFE_IPV6_DUMP_TEXT1);
  fprintf( access_fd,"%s",command);
  memset(command,0,MAX_COMMAND_STR_LEN);
  snprintf(command, MAX_COMMAND_STR_LEN,
           SFE_IPV6_DUMP_TEXT2);
  fprintf( access_fd,"%s",command);
  memset(command,0,MAX_COMMAND_STR_LEN);
  snprintf(command, MAX_COMMAND_STR_LEN,
           SFE_IPV6_DUMP_TEXT3);
  fprintf( access_fd,"%s",command);
  memset(command,0,MAX_COMMAND_STR_LEN);
  snprintf(command, MAX_COMMAND_STR_LEN,
           SFE_IPV6_DUMP_TEXT4);
  fprintf( access_fd,"%s",command);
  memset(command,0,MAX_COMMAND_STR_LEN);
  snprintf(command, MAX_COMMAND_STR_LEN,
           SFE_IPV6_DUMP_TEXT5);
  fprintf( access_fd,"%s",command);
  memset(command,0,MAX_COMMAND_STR_LEN);
  snprintf(command, MAX_COMMAND_STR_LEN,
           SFE_IPV6_DUMP_TEXT6);
  fprintf( access_fd,"%s",command);
  memset(command,0,MAX_COMMAND_STR_LEN);
  snprintf(command, MAX_COMMAND_STR_LEN,
           SFE_IPV6_DUMP_TEXT7);
  fprintf( access_fd,"%s",command);
  memset(command,0,MAX_COMMAND_STR_LEN);
  snprintf(command, MAX_COMMAND_STR_LEN,
           SFE_IPV6_DUMP_TEXT8);
  fprintf( access_fd,"%s",command);
  memset(command,0,MAX_COMMAND_STR_LEN);
  fclose(access_fd);
  memset(command,0,MAX_COMMAND_STR_LEN);
  snprintf( command, MAX_COMMAND_STR_LEN,"chmod 755 %s",
            IPV6_PACK_STAT_SFE_SHELL_FILE);
  ds_system_call( command, strlen(command));
  return SUCCESS;
}

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

void removeSFEDumpFile(void)
{
  char command[MAX_COMMAND_STR_LEN]={0};
  snprintf(command,MAX_COMMAND_STR_LEN,"rm -rf %s",
           IPV4_PACK_STAT_SFE_SHELL_FILE);
  ds_system_call(command, strlen(command));
  memset(command,0,MAX_COMMAND_STR_LEN);
  snprintf(command,MAX_COMMAND_STR_LEN,"rm -rf %s",
           IPV6_PACK_STAT_SFE_SHELL_FILE);
  ds_system_call(command, strlen(command));

}


/*=====================================================
  FUNCTION LookUpCDIAndUpdate
======================================================*/
/*!
@brief
  - Match the MAC address
  - If a match is found, check if given IP already exist if not add them to entry
@parameters
  Pointer to pkt_buf
@return
  none
@note
- Dependencies
- None
- Side Effects
- None
*/
/*=====================================================*/
bool LookUpCDIAndUpdate
(
  qcmap_nl_sock_msg_t* pktbuf
)
{
  ds_dll_el_t * node = NULL;
  qcmap_cm_client_data_info_t* list_data = NULL;
  qmi_error_type_v01 qmi_err_num;
  int qcmap_cm_error;
  in_addr addr;
  int ret;
  char ipv6addr[INET6_ADDRSTRLEN];
  char ipv4addr[INET_ADDRSTRLEN];
  uint8 mac_addr_char[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  uint8 zero_buff[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Backhaul* QcMapBackhaulMgr=GET_DEFAULT_BACKHAUL_OBJECT();
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWAN = GET_DEFAULT_BACKHAUL_WWAN_OBJECT();
  struct ps_in6_addr *nl_prefix_ptr;
  struct ps_in6_addr *prefix_ptr;
  struct ps_in6_addr iid_addr, list_iid_addr;
  uint8 origIPv6[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  memset(devname, 0, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2);
  char ip6_addr[INET6_ADDRSTRLEN];
  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  memset(ipv6addr, 0, INET6_ADDRSTRLEN);
  memset(ipv4addr,0,INET_ADDRSTRLEN);
  memset(ip6_addr,0,INET_ADDRSTRLEN);
  memset(mac_addr_char, 0, QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01);
  memset(zero_buff,0,QCMAP_MSGR_IPV6_ADDR_LEN_V01);
  qcmap_mac_addr_vlan_info_t mac_addr_vlan_info;
  qcmap_nl_addr_t  *nl_addr = NULL;


  if (pktbuf == NULL)
  {
    LOG_MSG_ERROR("Input pktbuf is NULL \n",0,0,0);
    return FALSE;
  }

  nl_addr = &(pktbuf->nl_addr);

  if( QcMapBackhaulWWAN && ((QcMapBackhaulWWAN->GetState() == QCMAP_CM_WAN_CONNECTED)||
     (QcMapBackhaulWWAN->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED))&&
      (QcMapBackhaulMgr != NULL && QCMAP_Backhaul::current_backhaul == BACKHAUL_TYPE_WWAN))
  {

    if ( addrList->addrListHead == NULL)
    {
      LOG_MSG_ERROR("Linked list head is NULL \n",0,0,0);
      return false;
    }

    ds_mac_addr_ntop(nl_addr->mac_addr, mac_addr_char);


    memcpy(mac_addr_vlan_info.mac_addr,nl_addr->mac_addr,QCMAP_MSGR_MAC_ADDR_LEN_V01);
    mac_addr_vlan_info.vlan_id = pktbuf->vlan_id;

    node = ds_dll_search (addrList->addrListHead ,(void*)&mac_addr_vlan_info,
                          QcMapMgr->qcmap_match_mac_addr_vlan_id_pair);
    if( node == NULL)
    {
      return false;
    }
    else
    {
      //When a node is found, update the IP address from the netlink event
      list_data = (qcmap_cm_client_data_info_t*)(node->data);
      if (list_data == NULL)
      {
        LOG_MSG_INFO1("Error in fetching node data ", 0, 0, 0);
        return false;
      }
      ds_mac_addr_ntop(list_data->mac_addr, mac_addr_char);
      if (nl_addr->isValidIPv4address == true && nl_addr->ip_addr != 0)
      {
       /*ip will be added to cdi list by legacy framework. In this code we add this
           ipv4 to iptables for accounting*/
        if(inet_ntop(AF_INET,
                       (void *)&nl_addr->ip_addr,ipv4addr,
                       INET_ADDRSTRLEN))
        {
          AddRuletoIptables(ipv4addr, IPV4_RULE);
        }
      }
    }

    if (nl_addr->isValidIPv6address &&
        (QcMapBackhaulWWAN->GetIPv6State() == QCMAP_CM_V6_WAN_CONNECTED))
    {
      memcpy(origIPv6, nl_addr->ip_v6_addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01);
      //check if the address is link local
      nl_prefix_ptr = (struct ps_in6_addr*)(nl_addr->ip_v6_addr);
      if((QCMAP_IN6_IS_PREFIX_LINKLOCAL(nl_prefix_ptr->ps_s6_addr32)))
      {
        //convert Linklocal to global using prefix and IID info -- assuming unicast addresses
        if (QcMapBackhaulMgr)
        {
          ret = QcMapBackhaulMgr->GetDeviceName(devname,
                                                QCMAP_MSGR_IP_FAMILY_V6_V01,
                                                &qmi_err_num);

          memset(&QcMapBackhaulMgr->ipv6_prefix_info, 0, sizeof(qcmap_cm_nl_prefix_info_t));
          QcMapBackhaulMgr->ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
          QcMapBackhaulMgr->ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
          QcMapBackhaulMgr->GetIPV6PrefixInfo(devname,&(QcMapBackhaulMgr->ipv6_prefix_info));

          inet_ntop(AF_INET6,
                    (struct sockaddr_in6 *)origIPv6,
                    ip6_addr, INET6_ADDRSTRLEN);
          LOG_MSG_INFO1("LookUpCDIAndUpdate - Received IP %s",ip6_addr,0,0);

          //placement of ipv6 is big endian meaning iid in higher memory
          prefix_ptr = (struct ps_in6_addr *) ((struct sockaddr_in6 *)&
                                               (QcMapBackhaulMgr->ipv6_prefix_info.prefix_addr))->sin6_addr.s6_addr;
          prefix_ptr->ps_s6_addr64[1] <<=
              QcMapBackhaulMgr->ipv6_prefix_info.prefix_len;
          prefix_ptr->ps_s6_addr64[1] >>=
              QcMapBackhaulMgr->ipv6_prefix_info.prefix_len;

          //get the IID
          memcpy(&iid_addr, nl_addr->ip_v6_addr, sizeof (struct ps_in6_addr));
          iid_addr.ps_s6_addr64[0] >>=
              (IPV6_MAX_PREFIX_LENGTH - QcMapBackhaulMgr->ipv6_prefix_info.prefix_len);
          iid_addr.ps_s6_addr64[0] <<=
              (IPV6_MAX_PREFIX_LENGTH - QcMapBackhaulMgr->ipv6_prefix_info.prefix_len);
          //get iid now ..
          prefix_ptr->ps_s6_addr64[1] = iid_addr.ps_s6_addr64[1];
          //prefix_ptr has complete IP address now
         //look up CDI ipv6 list and check if the address exists or not?if not then add it.
          for (int i = 0; i < QCMAP_MSGR_MAX_IPV6_ADDR_V01; i++)
          {
            if (!memcmp (list_data->ipv6[i].addr, prefix_ptr, sizeof(struct in6_addr)))
            {
              /*ipv6 address already exist on CDI but we still need to check
                if rules are installed properly */
              if(inet_ntop(AF_INET6,
                          (void *)prefix_ptr,ipv6addr,
                          INET6_ADDRSTRLEN))
              {
                AddRuletoIptables(ipv6addr, IPV6_RULE);
              }
              break;
            }
            else
            {
              if (!memcmp(list_data->ipv6[i].addr, zero_buff, QCMAP_MSGR_IPV6_ADDR_LEN_V01))
              {
                memcpy(list_data->ipv6[i].addr, prefix_ptr, sizeof(struct in6_addr));
                inet_ntop(AF_INET6,
                          (void *)prefix_ptr,
                          ip6_addr, INET6_ADDRSTRLEN);
                LOG_MSG_INFO1("LookUpCDIAndUpdate - New V6 Address %s added at index:%d\n",ip6_addr,i,0);

                //QcMapMgr->SendPacketStatsInd(QCMAP_MSGR_PACKET_STATS_IPV6_UPDATED_V01,1,list_data);

                /* this is where we add the client to ip6tables*/
                if(inet_ntop(AF_INET6,
                            (void *)prefix_ptr,ipv6addr,
                            INET6_ADDRSTRLEN))
                {
                  AddRuletoIptables(ipv6addr, IPV6_RULE);
                }
                break;
              }
              else
              {
                //Compare the IID part - if equal or zero then break, handled in legacy path (MatchMacAddrInList)
                /*if only prefix is updated not IID or only IID is updated not PREFIX for list_data->ipv6[i], just break*/
                memcpy(&list_iid_addr, list_data->ipv6[i].addr, sizeof (struct ps_in6_addr));
                list_iid_addr.ps_s6_addr64[0] >>=
                    (IPV6_MAX_PREFIX_LENGTH - QcMapBackhaulMgr->ipv6_prefix_info.prefix_len);
                list_iid_addr.ps_s6_addr64[0] <<=
                    (IPV6_MAX_PREFIX_LENGTH - QcMapBackhaulMgr->ipv6_prefix_info.prefix_len);
                if(!memcmp(&list_iid_addr, &iid_addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01) ||
                   !memcmp(&list_iid_addr, zero_buff, QCMAP_MSGR_IPV6_ADDR_LEN_V01))
                {
                  inet_ntop(AF_INET6,
                            (void *)&list_iid_addr,
                             ip6_addr, INET6_ADDRSTRLEN);
                  LOG_MSG_INFO1("LookUpCDIAndUpdate-IID already exist or NULL break %s",ip6_addr,0,0);
                  break;
                }

                if (i+1 == QCMAP_MSGR_MAX_IPV6_ADDR_V01)
                {
                  LOG_MSG_INFO1("Max IPV6 reached",0,0,0);
                }
                else
                  continue;
              }
            }
          }
        }
      }
      else
      {
     //look up CDI ipv6 list and check if the address exists or not?if not then add it.
        for (int i = 0; i < QCMAP_MSGR_MAX_IPV6_ADDR_V01; i++)
        {
          if (!memcmp (list_data->ipv6[i].addr, origIPv6, sizeof(struct in6_addr)))
          {
            //ipv6 already exist in CDI
            if(inet_ntop(AF_INET6,
                            (void *)origIPv6,ipv6addr,
                            INET6_ADDRSTRLEN))
            {
              AddRuletoIptables(ipv6addr, IPV6_RULE);
            }
            break;
          }
          else
          {
           if (!memcmp(list_data->ipv6[i].addr, zero_buff, QCMAP_MSGR_IPV6_ADDR_LEN_V01))
            {
              memcpy(list_data->ipv6[i].addr, origIPv6, sizeof(struct in6_addr));
              inet_ntop(AF_INET6,
                        (void *)origIPv6,
                        ip6_addr, INET6_ADDRSTRLEN);
              LOG_MSG_INFO1("LookUpCDIAndUpdate - New V6 Address %s added at index:%d",ip6_addr,i,0);

              //QcMapMgr->SendPacketStatsInd(QCMAP_MSGR_PACKET_STATS_IPV6_UPDATED_V01,1,list_data);
              if(inet_ntop(AF_INET6,
                          (void *)origIPv6,ipv6addr,
                          INET6_ADDRSTRLEN))
              {
                AddRuletoIptables(ipv6addr, IPV6_RULE);
              }
              break;
            }
            else
            {
              if (i+1 == QCMAP_MSGR_MAX_IPV6_ADDR_V01)
              {
                LOG_MSG_INFO1("Max IPV6 reached",0,0,0);
              }
              else
                continue;
            }
          }
        }
      }
    }
    return true;
  }
  else
  {
    if (QcMapBackhaulMgr != NULL && QCMAP_Backhaul::current_backhaul == NO_BACKHAUL)
    {
      //No BH available do nothing
    }
    return true;
  }
}


