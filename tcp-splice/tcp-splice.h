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
* File: tcp-splice.h
*
* Description: TCP splice implementation through netfilter.
*
***************************************************************************/

//IPv6 hdr = 40 bytes, IPv4 hdr = 20 bytes w/ no options; 40 - 20 = 20
#define IPV6_IPV4_HEADER_DIFF 20

#define IPV4_HDR_LEN_BYTES(ihl) (ihl << 2)

#define PROC_FILE_NAME "tcpsplice"

#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/types.h>

#include <asm/checksum.h>

#include <net/sock.h>
#include <net/ip6_checksum.h>

#include <uapi/linux/netfilter_ipv4.h>
#include <uapi/linux/ip.h>

/***************************************************************************
*
* Function: tcp_splice_proc_read_cb
*
* Description: callback for proc entry
*
* Parameters: struct file* fp; //file ptr to proc entry
*             char __user* user; //buffer filled from userspace
*             size_t size; size of buffer from userspace
*             loff_t* offset; //offset
*
* Return: ssize_t, the number of bytes needed to process in userspace buffer
*
***************************************************************************/
ssize_t tcp_splice_proc_read_cb(struct file* fp, char __user* user, size_t size, loff_t* offset);

/***************************************************************************
*
* Function: tcp_splice_proc_write_cb
*
* Description: callback for proc entry
*
* Parameters: struct file* fp; //file ptr to proc entry
*             char __user* user; //buffer filled from userspace
*             size_t size; size of buffer from userspace
*             loff_t* offset; //offset
*
* Return: ssize_t, the number of bytes processed from userspace buffer
*
***************************************************************************/
ssize_t tcp_splice_proc_write_cb(struct file* fp, const char __user* user, size_t size,
                                 loff_t* offset);

/***************************************************************************
*
* Function: pre_route_v4
*
* Description: callback for IPv4 skb in preroute chain
*
* Parameters: unsigned int hooknum, //hook number
*              struct sk_buff* skb, //the skb
*              const struct net_device* in, //net_device the skb arrived ingress
*              const struct net_device* out, //net_device the skb should egress
*              int (*okfn)(struct sk_buff*), //fcn ptr to okfn after all hooks done processing
*
* Return: NF_DROP, discard the skb
*          NF_ACCEPT, continue traversal in kernel network stack
*          NF_STOLEN, do not continue traversal
*          NF_QUEUE, queue skb for userspace
*           NF_REPEAT, call this callback again
*
***************************************************************************/
unsigned int pre_route_v4(unsigned int hooknum, struct sk_buff* skb, const struct net_device* in,
                          const struct net_device* out, int (*okfn)(struct sk_buff*));

/***************************************************************************
*
* Function: local_out_v4
*
* Description: calls ip_local_out after checksumming skb
*
* Parameters: struct sk_buff* skb, //the skb
*
* Return: 0, success
*         -1, otherwise
*
***************************************************************************/
unsigned int local_out_v4(struct sk_buff* skb);

/***************************************************************************
*
* Function: forward_v4
*
* Description: callback for IPv4 skb in forward chain
*
* Parameters: unsigned int hooknum, //hook number
*              struct sk_buff* skb, //the skb
*              const struct net_device* in, //net_device the skb arrived ingress
*              const struct net_device* out, //net_device the skb should egress
*              int (*okfn)(struct sk_buff*), //fcn ptr to okfn after all hooks done processing
*
* Return: NF_DROP, discard the skb
*          NF_ACCEPT, continue traversal in kernel network stack
*          NF_STOLEN, do not continue traversal
*          NF_QUEUE, queue skb for userspace
*           NF_REPEAT, call this callback again
*
***************************************************************************/
unsigned int forward_v4(unsigned int hooknum, struct sk_buff* skb, const struct net_device* in,
                        const struct net_device* out, int (*okfn)(struct sk_buff*));

/***************************************************************************
*
* Function: post_route_v4
*
* Description: callback for IPv4 skb in postroute chain
*
* Parameters: unsigned int hooknum, //hook number
*              struct sk_buff* skb, //the skb
*              const struct net_device* in, //net_device the skb arrived ingress
*              const struct net_device* out, //net_device the skb should egress
*              int (*okfn)(struct sk_buff*), //fcn ptr to okfn after all hooks done processing
*
* Return: NF_DROP, discard the skb
*          NF_ACCEPT, continue traversal in kernel network stack
*          NF_STOLEN, do not continue traversal
*          NF_QUEUE, queue skb for userspace
*           NF_REPEAT, call this callback again
*
***************************************************************************/
unsigned int post_route_v4(unsigned int hooknum, struct sk_buff* skb, const struct net_device* in,
                            const struct net_device* out, int (*okfn)(struct sk_buff*));

/***************************************************************************
*
* Function: pre_route_v6
*
* Description: callback for IPv6 skb in preroute chain
*
* Parameters: unsigned int hooknum, //hook number
*              struct sk_buff* skb, //the skb
*              const struct net_device* in, //net_device the skb arrived ingress
*              const struct net_device* out, //net_device the skb should egress
*              int (*okfn)(struct sk_buff*), //fcn ptr to okfn after all hooks done processing
*
* Return: NF_DROP, discard the skb
*          NF_ACCEPT, continue traversal in kernel network stack
*          NF_STOLEN, do not continue traversal
*          NF_QUEUE, queue skb for userspace
*           NF_REPEAT, call this callback again
*
***************************************************************************/
unsigned int pre_route_v6(unsigned int hooknum, struct sk_buff* skb, const struct net_device* in,
                          const struct net_device* out, int (*okfn)(struct sk_buff*));

/***************************************************************************
*
* Function: local_out_v6
*
* Description: calls ip6_local_out after checksumming skb
*
* Parameters: struct sk_buff* skb, //the skb
*
* Return: 0, success
*         -1, otherwise
*
***************************************************************************/
unsigned int local_out_v6(struct sk_buff* skb);

/***************************************************************************
*
* Function: ip_splice_v4_to_v6
*
* Description: splices a v4 skb into a v6 skb
*
* Parameters:  struct in6_addr v6_daddr, // IPv6 daddr to push
*              struct in6_addr v6_saddr, // IPv6 saddr to push
*              unsigned short dest_port, // TCP dest port to push
*              unsigned short src_port, // TCP src port to push
*              struct sk_buff* skb, //the skb
*
* Return: none
*
***************************************************************************/
void ip_splice_v4_to_v6(struct in6_addr v6_daddr, struct in6_addr v6_saddr,
                        unsigned short dest_port, unsigned short src_port, struct sk_buff* skb);

/***************************************************************************
*
* Function: ip_splice_v6_to_v4
*
* Description: splices a v6 skb into a v4 skb
*
* Parameters:  unsigned int v4_daddr, // IPv4 daddr to push
*              unsigned int v4_saddr, // IPv4 saddr to push
*              unsigned short dest_port, // TCP dest port to push
*              unsigned short src_port, // TCP src port to push
*              struct sk_buff* skb, //the skb
*
* Return: none
*
***************************************************************************/
void ip_splice_v6_to_v4(unsigned int v4_daddr, unsigned int v4_saddr, unsigned short dest_port,
                        unsigned short src_port, struct sk_buff* skb);

/***************************************************************************
*
* Function: check_for_fin_rst_tcp_splice
*
* Description: checks tcp hdr for FIN or RST and notifies userspace
*
* Parameters:  struct tcphdr* tcp_hdr //the TCP hdr
*              struct tcp_splice_hash_entry* hash_entry //splice session info hash entry
*
* Return: none
*
***************************************************************************/
void check_for_fin_rst_tcp_splice(struct tcphdr* tcp_hdr, struct tcp_splice_hash_entry* hash_entry);

/***************************************************************************
*
* Function: tcp_splice_ht_free_entry
*
* Description: frees struct tcp_splice_hash_entry* hash_entry, is a call_rcu callback
*
* Parameters:  struct rcu_head* head //ptr to rcu_head member in hash_entry
*
* Return: none
*
***************************************************************************/
void tcp_splice_ht_free_entry(struct rcu_head* head);
