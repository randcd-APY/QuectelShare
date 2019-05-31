/***************************************************************************
*
* Copyright (c) 2017, The Linux Foundation. All rights reserved.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 and
* only version 2 as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
***************************************************************************/

/***************************************************************************
*
* Copyright (C) 2002 Wensong Zhang <wensong@linux-vs.org>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version
* 2 of the License, or (at your option) any later version.
*
***************************************************************************/

/***************************************************************************
*
* File: tcp-splice-interface.h
*
* Description: Provides functions to interfacing with userspace.
*
***************************************************************************/

#define MODULE_NAME "Code Aurora TCP Splice"

#define NETLINK_QC_SOCKSIFY 24

#define SO_TCP_SPLICE 8345 //for setsockopt

#define TCP_SPLICE_HASH_INDEX_SHIFT_SIZE 12

#define CLIENT_SUPPORTS_WINDOW_SCALING 0b10
#define REMOTE_SUPPORTS_WINDOW_SCALING 0b01

#define IPV6_MAX_PAYLOAD 1412

#define PRINTK_IPV6_ADDR(v6_addr) \
        pr_info(MODULE_NAME \
        ": 0x%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\n", \
        v6_addr.in6_u.u6_addr8[0], v6_addr.in6_u.u6_addr8[1], v6_addr.in6_u.u6_addr8[2], \
        v6_addr.in6_u.u6_addr8[3], v6_addr.in6_u.u6_addr8[4], v6_addr.in6_u.u6_addr8[5], \
        v6_addr.in6_u.u6_addr8[6], v6_addr.in6_u.u6_addr8[7],  v6_addr.in6_u.u6_addr8[8], \
        v6_addr.in6_u.u6_addr8[9], v6_addr.in6_u.u6_addr8[10], v6_addr.in6_u.u6_addr8[11], \
        v6_addr.in6_u.u6_addr8[12], v6_addr.in6_u.u6_addr8[13], v6_addr.in6_u.u6_addr8[14], \
        v6_addr.in6_u.u6_addr8[15]);

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/tcp.h>
#include <linux/hash.h>
#include <linux/net.h>
#include <linux/file.h>
#include <linux/hashtable.h>

#include <net/netfilter/nf_conntrack_tuple.h>
#include <net/netfilter/nf_conntrack_core.h>
#include <net/netfilter/nf_conntrack_zones.h>

#include <uapi/asm-generic/errno-base.h>


struct userspace_relay_session
{
  int cli_fd;
  int req_fd;
};

struct tcp_splice_session_info
{
  struct userspace_relay_session r_session;

  struct socket* cli_socket;
  struct socket* req_socket;

  //0b00 means don't window size splice
  //0b01 means cli doesn't support window scaling, and req does support window scaling
  //0b10 means cli supports window scaling, and req doesn't support window scaling
  //0b11 means cli and req support window scaling
  unsigned char rx_opt_wscale_ok:2,
                rx_opt_timestamp_ok:1,
                rx_opt_sack_ok:1; //leave the rest for zero pads
};

struct timestamp_splice_tuple
{
  unsigned int cli_ircv_tsval;
  unsigned int req_ircv_tsval;
  unsigned int cli_ircv_tsecr;
  unsigned int req_ircv_tsecr;
};

struct window_scale_splice_tuple
{
  unsigned short cli_isnd_wscale:4, //window scale received from cli
                 req_isnd_wscale:4, //window scale received from req
                 cli_ircv_wscale:4, //window scale sent to cli
                 req_ircv_wscale:4; //window scale sent to req
};

union addr
{
  unsigned int v4_addr;
  struct in6_addr v6_addr;
};

struct tcp_splice_tuple
{
  unsigned short cli_family;
  unsigned short req_family;
  union addr cli_daddr;
  union addr req_daddr;
  unsigned short cli_dport;
  unsigned short req_dport;
  unsigned int cli_iss;
  unsigned int req_iss;
  unsigned int cli_irs;
  unsigned int req_irs;
  union addr cli_saddr;
  union addr req_saddr;
  unsigned short cli_sk_num;
  unsigned short req_sk_num;
  unsigned short cli_sk_num_ct;
  unsigned short req_sk_num_ct;
  struct timestamp_splice_tuple tstamp_option;
  struct window_scale_splice_tuple wscale_option;
  struct dst_entry* cli_dst;
  struct dst_entry* req_dst;
};

struct tcp_splice_hash_entry
{
  struct hlist_node hash_node;
  struct rcu_head rcu;
  struct tcp_splice_session_info session_info;
  struct tcp_splice_tuple splice_tuple;
  unsigned int opp_key; //key to the opposing direction hash entry
};


/***************************************************************************
*
* Function: listenFromUserspace
*
* Description: callback for listening to netlink msgs from userspace
*
* Parameters: struct sk_buff* skb; //skb for netlink msg
*
* Return: none
*
***************************************************************************/
void listenFromUserspace(struct sk_buff* skb);

/***************************************************************************
*
* Function: sendToUserspace
*
* Description: send netlink msg to userspace that the socket pair fd has
*                seen a FIN or RST
*
* Parameters: struct tcp_splice_hash_entry* hash_entry; //socket pair fd
*
* Return: none
*
***************************************************************************/
void sendToUserspace(struct tcp_splice_hash_entry* hash_entry);

/***************************************************************************
*
* Function: tcp_splice_socket
*
* Description: callback for setsockopt SO_TCP_SPLICE
*
* Parameters: struct sock* sk; //socket fd
*               int optval; option value
*              void __user *user; user who called this function
*              unsigned int len; option length
*
* Return: 0, success
*         -1, otherwise
*
***************************************************************************/
int tcp_splice_socket(struct sock* sk, int optval, void __user *user, unsigned int len);

/***************************************************************************
*
* Function: insertSpliceInfoTuple
*
* Description: inserts tcp splice info into global hash table
*
* Parameters: struct tcp_splice_session_info* sp_session; //ptr to tcp splice session
*
* Return: 0, success
*         -1, otherwise
*
***************************************************************************/
int insertSpliceInfoTuple(struct tcp_splice_session_info* sp_session);

/***************************************************************************
*
* Function: printSessionInfo
*
* Description: prints tcp splice info
*
* Parameters: struct tcp_splice_session_info* sp_session; //ptr to tcp splice session
*
* Return: none
*
***************************************************************************/
void printSessionInfo(struct tcp_splice_session_info* sp_session);
