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
* File: tcp-splice-util.h
*
* Description: Provides utility functions for tcp splice.
*
***************************************************************************/

#define MODULE_NAME "Code Aurora TCP Splice"

#define SIZE_OF_TCP_V6_PSEUDO_HDR 40

#define TCPOLEN_SACK_PERBLOCK_SHIFT 3

#define CLI_DIRECTION 0
#define REQ_DIRECTION 1

#define MAX_WINDOW_SIZE 0x0000ffff

#include <net/tcp.h>

#include <uapi/linux/tcp.h>
#include <uapi/linux/ipv6.h>

/***************************************************************************
*
* Function: tcp_fast_parse_options_for_splice
*
* Description: ripped off of kernel mainline 3.18 but stripped of
*              unnecessary information for tcp splice
*
* Parameters: struct tcphdr* tcp_hdr; //ptr to tcp_hdr
*             unsigned int** timestamp; //double ptr for timestamp opt
*             struct tcp_sack_block** sacks; //double ptr for sack opt
*             unsigned char timestamp_ok; //check for timestamp
*             unsigned char sack_ok; //check for sack
*
* Return: 0, success
*        -1, otherwise
*
***************************************************************************/
int qti_tcp_fast_parse_options(struct tcphdr* tcp_hdr, unsigned int** timestamp,
                               struct tcp_sack_block** sacks, unsigned char* num_sacks,
                               unsigned char timestamp_ok, unsigned char sack_ok);

/***************************************************************************
*
* Function: dnat_tcp_splice
*
* Description: change dst IP addr and dst port for another tcp session
*
* Parameters: void* iphdr //the IP header
*             struct tcphdr* tcp_hdr //the TCP header
*             struct tcp_splice_hash_entry* hash_entry //splice session info hash entry
*             int direction //indicates splice for CLI_DIRECTION or REQ_DIRECTION
*
* Return: none
*
* Precondition: it is expected iphdr, tcp_hdr, and hash_entry are not NULL ptrs
*               it is expected this function is called within rcu_read_lock() rcu_read_unlock()
*
***************************************************************************/
void dnat_tcp_splice(void* iphdr, struct tcphdr* tcp_hdr, struct tcp_splice_hash_entry* hash_entry,
                     int direction);

/***************************************************************************
*
* Function: mangle_tcp_splice
*
* Description: mangles tcp header for another tcp session
*
* Parameters: struct tcphdr* tcp_hdr, //the tcp header
*             struct tcp_splice_hash_entry* hash_entry //splice session info hash entry
*             int direction //indicates splice for CLI_DIRECTION or REQ_DIRECTION
*
* Return: none
*
* Precondition: it is expected tcp_hdr and hash_entry are not NULL ptrs
*               it is expected this function is called within rcu_read_lock() rcu_read_unlock()
*
***************************************************************************/
void mangle_tcp_splice(struct tcphdr* tcp_hdr, struct tcp_splice_hash_entry* hash_entry,
                       int direction);

/***************************************************************************
*
* Function: snat_tcp_splice
*
* Description: change src IP addr and src port for another tcp session
*
* Parameters: void* iphdr //the IP header
*             struct tcphdr* tcp_hdr //the TCP header
*             struct tcp_splice_hash_entry* hash_entry //splice session info hash entry
*             int direction //indicates splice for CLI_DIRECTION or REQ_DIRECTION
*
* Return: none
*
* Precondition: it is expected iphdr, tcp_hdr, and hash_entry are not NULL ptrs
*               it is expected this function is called within rcu_read_lock() rcu_read_unlock()
*
***************************************************************************/
void snat_tcp_splice(void* iphdr, struct tcphdr* tcp_hdr, struct tcp_splice_hash_entry* hash_entry,
                     int direction);

/***************************************************************************
*
* Function: set_dst_route
*
* Description: changes skb_dst entry
*
* Parameters: struct sk_buff* skb //the skb
*             struct tcp_splice_hash_entry* hash_entry //splice session info hash entry
*             int direction //indicates splice for CLI_DIRECTION or REQ_DIRECTION
*
* Return: none
*
* Precondition: it is expected skb and hash_entry are not NULL ptrs
*               it is expected this function is called within rcu_read_lock() rcu_read_unlock()
*
***************************************************************************/
void set_dst_route(struct sk_buff* skb, struct tcp_splice_hash_entry* hash_entry, int direction);
