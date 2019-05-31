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
* File: tcp-splice.c
*
* Description: TCP splice implementation through netfilter.
*
***************************************************************************/

#include "tcp-splice-interface.h"
#include "tcp-splice-util.h"
#include "tcp-splice.h"

static struct proc_dir_entry* proc_file = NULL;
static struct file_operations proc_file_ops;

static struct nf_sockopt_ops tcp_splice_sockopt_hook;

static struct nf_hook_ops pre_hook_v4;
static struct nf_hook_ops fwd_hook_v4;
static struct nf_hook_ops post_hook_v4;

static struct nf_hook_ops pre_hook_v6;

int debug_mode = 0;
struct sock* nl_socket = NULL;
struct sockaddr_nl userspace_addr;

DEFINE_HASHTABLE(tcp_splice_ht, TCP_SPLICE_HASH_INDEX_SHIFT_SIZE);
spinlock_t ht_lock;

/***************************************************************************
*
* Function: tcp_splice_init
*
* Description: insmod
*
* Parameters: none
*
* Return: 0, success
*         -1, otherwise
*
***************************************************************************/
static int __init tcp_splice_init(void)
{
  struct netlink_kernel_cfg cfg;
  enum nf_ip_hook_priorities hook_priority;

  //define proc file and operations
  memset(&proc_file_ops, 0, sizeof(struct file_operations));
  proc_file_ops.owner = THIS_MODULE;
  proc_file_ops.read =  tcp_splice_proc_read_cb;
  proc_file_ops.write = tcp_splice_proc_write_cb;
  if((proc_file = proc_create(PROC_FILE_NAME, 0, init_net.proc_net, &proc_file_ops)) == NULL)
  {
    pr_err(MODULE_NAME": error creating proc entry!\n");
    return -EINVAL;
  }

  spin_lock_init(&ht_lock);

  hash_init(tcp_splice_ht);

  //necessary for the zeroing out of the struct list_head list member
  memset(&pre_hook_v4, 0, sizeof(struct nf_hook_ops));
  memset(&fwd_hook_v4, 0, sizeof(struct nf_hook_ops));
  memset(&post_hook_v4, 0, sizeof(struct nf_hook_ops));
  memset(&pre_hook_v6, 0, sizeof(struct nf_hook_ops));
  memset(&tcp_splice_sockopt_hook, 0, sizeof(struct nf_sockopt_ops));
  memset(&cfg, 0, sizeof(struct netlink_kernel_cfg));
  memset(&userspace_addr, 0, sizeof(struct sockaddr_nl));

  cfg.input = listenFromUserspace; //the callback
  if((nl_socket = netlink_kernel_create(&init_net, NETLINK_QC_SOCKSIFY, &cfg)) == 0)
  {
    pr_err(MODULE_NAME": failed to create netlink socket...\n");
    return -EINVAL;
  }
  userspace_addr.nl_family = AF_NETLINK; //prepping the userspace_addr
  pr_info(MODULE_NAME": netlink socket created\n");

  //fill out the nf_hook_ops struct
  pre_hook_v4.hook = (nf_hookfn*)pre_route_v4;
  pre_hook_v4.owner = THIS_MODULE;
  pre_hook_v4.pf = NFPROTO_IPV4; //match on IPV4 pkts
  pre_hook_v4.hooknum = NF_INET_PRE_ROUTING; //hook into the PRE_ROUTING
  hook_priority = NF_IP_PRI_NAT_DST; //specifying that we are DNAT priority
  //hook_priority = NF_IP_PRI_FIRST;
  pre_hook_v4.priority = hook_priority;
  if(nf_register_hook(&pre_hook_v4) != 0)
  {
    pr_err(MODULE_NAME": error registering the pre_hook_v4 callback\n");
    netlink_kernel_release(nl_socket);
    return -EINVAL;
  }

  //fill out the nf_hook_ops struct
  fwd_hook_v4.hook = (nf_hookfn*)forward_v4;
  fwd_hook_v4.owner = THIS_MODULE;
  fwd_hook_v4.pf = NFPROTO_IPV4; //match on IPV4 pkts
  fwd_hook_v4.hooknum = NF_INET_FORWARD; //NF_INET_FORWARD
  hook_priority = NF_IP_PRI_MANGLE; //specifying that we are MANGLE priority
  fwd_hook_v4.priority = hook_priority;
  if(nf_register_hook(&fwd_hook_v4) != 0)
  {
    pr_err(MODULE_NAME": error registering the fwd_hook_v4 callback\n");
    netlink_kernel_release(nl_socket);
    return -EINVAL;
  }

  //fill out the nf_hook_ops struct
  post_hook_v4.hook = (nf_hookfn*)post_route_v4;
  post_hook_v4.owner = THIS_MODULE;
  post_hook_v4.pf = NFPROTO_IPV4; //match on IPV4 pkts
  post_hook_v4.hooknum = NF_INET_POST_ROUTING; //NF_INET_POST_ROUTING
  hook_priority = NF_IP_PRI_LAST;
  post_hook_v4.priority = hook_priority;
  if(nf_register_hook(&post_hook_v4) != 0)
  {
    pr_err(MODULE_NAME": error registering the post_hook_v4 callback\n");
    netlink_kernel_release(nl_socket);
    return -EINVAL;
  }

  //fill out the nf_hook_ops struct
  pre_hook_v6.hook = (nf_hookfn*)pre_route_v6;
  pre_hook_v6.owner = THIS_MODULE;
  pre_hook_v6.pf = NFPROTO_IPV6; //match on IPV6 pkts
  pre_hook_v6.hooknum = NF_INET_PRE_ROUTING; //hook into the PRE_ROUTING
  hook_priority = NF_IP_PRI_NAT_DST; //specifying that we are DNAT priority
  pre_hook_v6.priority = hook_priority;
  if(nf_register_hook(&pre_hook_v6) != 0)
  {
    pr_err(MODULE_NAME": error registering the pre_hook_v6 callback\n");
    netlink_kernel_release(nl_socket);
    return -EINVAL;
  }

  pr_info(MODULE_NAME": netfilter hook callbacks registered...\n");


  //fill out the nf_sockopt_ops struct
  tcp_splice_sockopt_hook.pf = PF_INET;
  tcp_splice_sockopt_hook.set = tcp_splice_socket;
  tcp_splice_sockopt_hook.set_optmin = SO_TCP_SPLICE;
  tcp_splice_sockopt_hook.set_optmax = SO_TCP_SPLICE + 1;
  tcp_splice_sockopt_hook.owner = THIS_MODULE;
  if(nf_register_sockopt(&tcp_splice_sockopt_hook) != 0)
  {
    pr_err(MODULE_NAME": error registering the setsockopt callback\n");
    netlink_kernel_release(nl_socket);
    return -EINVAL;
  }

  pr_info(MODULE_NAME": netfilter hook sockopt registered...\n");

  return 0;
}

/***************************************************************************
*
* Function: tcp_splice_exit
*
* Description: rmmod
*
* Parameters: none
*
* Return: none
*
***************************************************************************/
static void __exit tcp_splice_exit(void)
{
  int i;
  struct tcp_splice_hash_entry* hash_entry = NULL;

  pr_info(MODULE_NAME": cleaning up...\n");

  nf_unregister_hook(&pre_hook_v4);
  nf_unregister_hook(&fwd_hook_v4);
  nf_unregister_hook(&post_hook_v4);
  nf_unregister_hook(&pre_hook_v6);
  nf_unregister_sockopt(&tcp_splice_sockopt_hook);
  netlink_kernel_release(nl_socket);

  spin_lock_bh(&ht_lock);
  hash_for_each_rcu(tcp_splice_ht, i, hash_entry, hash_node)
  {
    hash_del_rcu(&(hash_entry->hash_node));
    call_rcu(&(hash_entry->rcu), tcp_splice_ht_free_entry);
  }
  spin_unlock_bh(&ht_lock);

  rcu_barrier(); //wait for all call_rcu callbacks to complete before exiting

  proc_remove(proc_file);

  pr_info(MODULE_NAME": finished clean up...\n");
  return;
}

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
ssize_t tcp_splice_proc_read_cb(struct file* fp, char __user* user, size_t size, loff_t* offset)
{
  if(NULL == fp)
  {
    pr_err(MODULE_NAME": %s fp NULL!\n", __func__);
    return 0;
  } else if(NULL == user) {
    pr_err(MODULE_NAME": %s user NULL!\n", __func__);
    return 0;
  } else if(NULL == offset) {
    pr_err(MODULE_NAME": %s offset NULL!\n", __func__);
    return 0;
  }

  switch(debug_mode)
  {
    case(0):
    {
      pr_info(MODULE_NAME": debug mode is disabled...\n");
      break;
    }

    default:
    {
      pr_info(MODULE_NAME": debug mode is enabled...\n");
      break;
    }
  }

  return 0;
}

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
                                 loff_t* offset)
{
  char *buffer = NULL;

  if(NULL == fp)
  {
    pr_err(MODULE_NAME": %s fp NULL!\n", __func__);
    return size;
  } else if(NULL == user) {
    pr_err(MODULE_NAME": %s user NULL!\n", __func__);
    return size;
  } else if(NULL == offset) {
    pr_err(MODULE_NAME": %s offset NULL!\n", __func__);
    return size;
  }

  if((buffer = kzalloc(size + 1, GFP_ATOMIC)) == NULL)
  {
    pr_err(MODULE_NAME": %s kzalloc failed!\n", __func__);
    return size;
  }

  if(copy_from_user(buffer, user, size) != 0)
  {
    pr_err(MODULE_NAME": %s error copying from userspace\n", __func__);
    kfree(buffer);
    return size;
  }

  //echo command will have '\n' character
  if((size > 0) && (buffer[size - 1] == '\n'))
  {
    buffer[size - 1] = '\0';
  }

  if(!strcmp(buffer, "1"))
  {
    pr_info(MODULE_NAME": enabling debug mode...\n");
    debug_mode = 1;

  } else if(!strcmp(buffer, "0")) {

    pr_info(MODULE_NAME": disabling debug mode...\n");
    debug_mode = 0;

  } else {
    pr_info(MODULE_NAME": echo 1 or 0 for enable/disable debug mode respectively\n");
  }

  kfree(buffer);
  return size;
}

/***************************************************************************
*
* Function: pre_route_v4
*
* Description: callback for IPv4 skb in preroute chain
*
* Parameters: unsigned int hooknum, //hook number
*             struct sk_buff* skb, //the skb
*             const struct net_device* in, //net_device the skb arrived ingress
*             const struct net_device* out, //net_device the skb should egress
*             int (*okfn)(struct sk_buff*), //fcn ptr to okfn after all hooks done processing
*
* Return: NF_DROP, discard the skb
*          NF_ACCEPT, continue traversal in kernel network stack
*         NF_STOLEN, do not continue traversal
*         NF_QUEUE, queue skb for userspace
*         NF_REPEAT, call this callback again
*
***************************************************************************/
unsigned int pre_route_v4(unsigned int hooknum, struct sk_buff* skb, const struct net_device* in,
                          const struct net_device* out, int (*okfn)(struct sk_buff*))
{
  struct iphdr* ip_hdr = (struct iphdr*)skb_network_header(skb); //get L3 info;
  struct tcphdr* tcp_hdr;
  struct tcp_splice_hash_entry* hash_entry;

  //match on TCP, and IPv4 should already be matched on when registering
  if(ip_hdr->protocol == IPPROTO_TCP)
  {
    //sanity check to prevent overflow
    if((skb->data + (IPV4_HDR_LEN_BYTES(ip_hdr->ihl))) >= skb->tail)
    {
      pr_debug(MODULE_NAME": %s bad ip header\n", __func__);
      return NF_DROP;
    }

    tcp_hdr = (struct tcphdr*)(skb->data + (IPV4_HDR_LEN_BYTES(ip_hdr->ihl))); //get L4 info

    //lookup in hashtable
    rcu_read_lock();
    hash_for_each_possible_rcu(tcp_splice_ht, hash_entry, hash_node,
                               ip_hdr->saddr ^ tcp_hdr->source ^ tcp_hdr->dest)
    {
      //check direction
      if(tcp_hdr->source == rcu_dereference(hash_entry)->splice_tuple.cli_dport)
      {
        //change dest IP
        switch(rcu_dereference(hash_entry)->splice_tuple.req_family)
        {
          case AF_INET:
          {
            ip_hdr->daddr = rcu_dereference(hash_entry)->splice_tuple.req_daddr.v4_addr;
            break;
          }
          case AF_INET6: //IPv4 to IPv6
          {
            ip_splice_v4_to_v6(rcu_dereference(hash_entry)->splice_tuple.req_daddr.v6_addr,
                               rcu_dereference(hash_entry)->splice_tuple.req_saddr.v6_addr,
                               rcu_dereference(hash_entry)->splice_tuple.req_dport,
                               rcu_dereference(hash_entry)->splice_tuple.req_sk_num, skb);

            //for req
            mangle_tcp_splice(tcp_hdr, rcu_dereference(hash_entry), REQ_DIRECTION);

            //set new dst route
            set_dst_route(skb, rcu_dereference(hash_entry), REQ_DIRECTION);

            //replace with actual source port
            tcp_hdr->source = rcu_dereference(hash_entry)->splice_tuple.req_sk_num_ct;

            //check for fin or rst
            check_for_fin_rst_tcp_splice(tcp_hdr, rcu_dereference(hash_entry));

            if(local_out_v6(skb))
            {
              pr_debug(MODULE_NAME": %s, error with local_out_v6\n", __func__);
            }
            rcu_read_unlock();

            return NF_STOLEN;
            break;
          }
          default:
          {
            rcu_read_unlock();
            return NF_DROP;
            break;
          }
        }

        rcu_read_unlock();
        goto checksum;

      } else if(tcp_hdr->source == rcu_dereference(hash_entry)->splice_tuple.req_dport)
      {
        //change dest IP
        switch(rcu_dereference(hash_entry)->splice_tuple.cli_family)
        {
          case AF_INET:
          {
            ip_hdr->daddr = rcu_dereference(hash_entry)->splice_tuple.cli_daddr.v4_addr;
            break;
          }
          case AF_INET6: //IPv4 to IPv6
          {
            ip_splice_v4_to_v6(rcu_dereference(hash_entry)->splice_tuple.cli_daddr.v6_addr,
                               rcu_dereference(hash_entry)->splice_tuple.cli_saddr.v6_addr,
                               rcu_dereference(hash_entry)->splice_tuple.cli_dport,
                               rcu_dereference(hash_entry)->splice_tuple.cli_sk_num, skb);

            //for cli
            mangle_tcp_splice(tcp_hdr, rcu_dereference(hash_entry), CLI_DIRECTION);

            //set new dst route
            set_dst_route(skb, rcu_dereference(hash_entry), CLI_DIRECTION);

            //replace with actual source port
            tcp_hdr->source = rcu_dereference(hash_entry)->splice_tuple.cli_sk_num_ct;

            //check for fin or rst
            check_for_fin_rst_tcp_splice(tcp_hdr, rcu_dereference(hash_entry));

            if(local_out_v6(skb))
            {
              pr_debug(MODULE_NAME": %s, error with local_out_v6\n", __func__);
            }
            rcu_read_unlock();

            return NF_STOLEN;
            break;
          }
          default:
          {
            rcu_read_unlock();
            return NF_DROP;
            break;
          }
        }

        rcu_read_unlock();
        goto checksum;
      }

    }
    rcu_read_unlock();
  }

  return NF_ACCEPT; //pass pkt along its traversal

  checksum:

    //recompute L3 checksum
    ip_hdr->check = 0x0000;
    ip_hdr->check = ip_fast_csum(ip_hdr, ip_hdr->ihl);

    //recompute L4 checksum is in post route chain, because we only want to goto routing
    return NF_ACCEPT; //pass pkt along its traversal
}

/***************************************************************************
*
* Function: local_out_v4
*
* Description: calls ip_local_out after checksumming skb
*
* Parameters: struct sk_buff* skb, //the skb
*
* Return: 0, success
*        -1, otherwise
*
***************************************************************************/
unsigned int local_out_v4(struct sk_buff* skb)
{
  struct iphdr* ip_hdr = (struct iphdr*)skb_network_header(skb); //get L3 info;
  struct tcphdr* tcp_hdr;

  //sanity check to prevent overflow
  if((skb->data + (IPV4_HDR_LEN_BYTES(ip_hdr->ihl))) >= skb->tail)
  {
    pr_debug(MODULE_NAME": %s bad ip header\n", __func__);
    kfree_skb(skb);
    return -1;
  }

  tcp_hdr = (struct tcphdr*)(skb->data + (IPV4_HDR_LEN_BYTES(ip_hdr->ihl))); //get L4 info

  if(((struct nf_conn*)skb->nfct))
  {
    //tell conntrack we already NAT'ed
    ((struct nf_conn*)skb->nfct)->status |= IPS_NAT_DONE_MASK;
  } else {
    pr_debug(MODULE_NAME": skb->nfct is NULL!\n");
    kfree_skb(skb);
    return -1;
  }

  if(NULL == skb_dst(skb))
  {
    pr_debug(MODULE_NAME": skb_dst(skb) is NULL! (%d)\n", __LINE__);
    kfree_skb(skb);
    return -1;
  }

  //recompute L3 checksum
  ip_hdr->check = 0x0000;
  ip_hdr->check = ip_fast_csum(ip_hdr, ip_hdr->ihl);

  //recompute L4 checksum
  tcp_hdr->check = 0x0000;
  tcp_hdr->check = tcp_v4_check(skb->len - (IPV4_HDR_LEN_BYTES(ip_hdr->ihl)),
                                ip_hdr->saddr, ip_hdr->daddr,
                                csum_partial((unsigned char*)tcp_hdr,
                                skb->len - (IPV4_HDR_LEN_BYTES(ip_hdr->ihl)), 0));

  return ip_local_out(skb); //pass pkt along its traversal
}

/***************************************************************************
*
* Function: forward_v4
*
* Description: callback for IPv4 skb in forward chain
*
* Parameters: unsigned int hooknum, //hook number
*             struct sk_buff* skb, //the skb
*             const struct net_device* in, //net_device the skb arrived ingress
*             const struct net_device* out, //net_device the skb should egress
*             int (*okfn)(struct sk_buff*), //fcn ptr to okfn after all hooks done processing
*
* Return: NF_DROP, discard the skb
*         NF_ACCEPT, continue traversal in kernel network stack
*         NF_STOLEN, do not continue traversal
*         NF_QUEUE, queue skb for userspace
*         NF_REPEAT, call this callback again
*
***************************************************************************/
unsigned int forward_v4(unsigned int hooknum, struct sk_buff* skb, const struct net_device* in,
                        const struct net_device* out, int (*okfn)(struct sk_buff*))
{
  struct iphdr* ip_hdr = (struct iphdr*)skb_network_header(skb); //get L3 info;
  struct tcphdr* tcp_hdr;
  struct tcp_splice_hash_entry* hash_entry;

  //match on TCP, and IPv4 should already be matched on when registering
  if(ip_hdr->protocol == IPPROTO_TCP)
  {
    //sanity check to prevent overflow
    if((skb->data + (IPV4_HDR_LEN_BYTES(ip_hdr->ihl))) >= skb->tail)
    {
      pr_debug(MODULE_NAME": %s bad ip header\n", __func__);
      return NF_DROP;
    }

    tcp_hdr = (struct tcphdr*)(skb->data + (IPV4_HDR_LEN_BYTES(ip_hdr->ihl)));

    //lookup in hashtable
    rcu_read_lock();
    hash_for_each_possible_rcu(tcp_splice_ht, hash_entry, hash_node,
                               ip_hdr->saddr ^ tcp_hdr->source ^ tcp_hdr->dest)
    {
      //check direction
      if(tcp_hdr->source == rcu_dereference(hash_entry)->splice_tuple.cli_dport)
      {
        //for req
        mangle_tcp_splice(tcp_hdr, rcu_dereference(hash_entry), REQ_DIRECTION);
        //don't need to compute checksum just yet, post_route will checksum

      } else if(tcp_hdr->source == rcu_dereference(hash_entry)->splice_tuple.req_dport)
      {
        //for cli
        mangle_tcp_splice(tcp_hdr, rcu_dereference(hash_entry), CLI_DIRECTION);
        //don't need to compute checksum just yet, post_route will checksum
      }
    }
    rcu_read_unlock();
  }

  return NF_ACCEPT; //pass pkt along its traversal

}

/***************************************************************************
*
* Function: post_route_v4
*
* Description: callback for IPv4 skb in postroute chain
*
* Parameters: unsigned int hooknum, //hook number
*             struct sk_buff* skb, //the skb
*             const struct net_device* in, //net_device the skb arrived ingress
*             const struct net_device* out, //net_device the skb should egress
*             int (*okfn)(struct sk_buff*), //fcn ptr to okfn after all hooks done processing
*
* Return: NF_DROP, discard the skb
*         NF_ACCEPT, continue traversal in kernel network stack
*         NF_STOLEN, do not continue traversal
*         NF_QUEUE, queue skb for userspace
*          NF_REPEAT, call this callback again
*
***************************************************************************/
unsigned int post_route_v4(unsigned int hooknum, struct sk_buff* skb, const struct net_device* in,
                           const struct net_device* out, int (*okfn)(struct sk_buff*))
{
  struct iphdr* ip_hdr = (struct iphdr*)skb_network_header(skb); //get L3 info;
  struct tcphdr* tcp_hdr;
  struct tcp_splice_hash_entry* hash_entry;

  //match on TCP, and IPv4 should already be matched on when registering
  if(ip_hdr->protocol == IPPROTO_TCP)
  {
    //sanity check to prevent overflow
    if((skb->data + (IPV4_HDR_LEN_BYTES(ip_hdr->ihl))) >= skb->tail)
    {
      pr_debug(MODULE_NAME": %s bad ip header\n", __func__);
      return NF_DROP;
    }

    tcp_hdr = (struct tcphdr*)(skb->data + (IPV4_HDR_LEN_BYTES(ip_hdr->ihl)));

    //lookup in hashtable
    rcu_read_lock();
    hash_for_each_possible_rcu(tcp_splice_ht, hash_entry, hash_node,
                               ip_hdr->saddr ^ tcp_hdr->source ^ tcp_hdr->dest)
    {
      //check direction
      if(tcp_hdr->source == rcu_dereference(hash_entry)->splice_tuple.cli_dport)
      {
        //change src IP, and src port
        snat_tcp_splice(ip_hdr, tcp_hdr, rcu_dereference(hash_entry), REQ_DIRECTION);

        //change dst port
        tcp_hdr->dest = rcu_dereference(hash_entry)->splice_tuple.req_dport;

        rcu_read_unlock();
        goto checksum;

      } else if(tcp_hdr->source == rcu_dereference(hash_entry)->splice_tuple.req_dport)
      {
        //change src IP, and src port
        snat_tcp_splice(ip_hdr, tcp_hdr, rcu_dereference(hash_entry), CLI_DIRECTION);

        //change dst port
        tcp_hdr->dest = rcu_dereference(hash_entry)->splice_tuple.cli_dport;

        rcu_read_unlock();
        goto checksum;
      }
    }
    rcu_read_unlock();
  }

  return NF_ACCEPT; //pass pkt along its traversal

  checksum:
    //recompute L3 checksum
    ip_hdr->check = 0x0000;
    ip_hdr->check = ip_fast_csum(ip_hdr, ip_hdr->ihl);

    //recompute L4 checksum
    tcp_hdr->check = 0x0000;
    tcp_hdr->check = tcp_v4_check(skb->len - (IPV4_HDR_LEN_BYTES(ip_hdr->ihl)), ip_hdr->saddr,
                                  ip_hdr->daddr, csum_partial((unsigned char*)tcp_hdr,
                                  skb->len - (IPV4_HDR_LEN_BYTES(ip_hdr->ihl)), 0));

    //check for fin or rst
    check_for_fin_rst_tcp_splice(tcp_hdr, hash_entry);

    return NF_ACCEPT; //pass pkt along its traversal
}

/***************************************************************************
*
* Function: pre_route_v6
*
* Description: callback for IPv6 skb in preroute chain
*
* Parameters: unsigned int hooknum, //hook number
*             struct sk_buff* skb, //the skb
*             const struct net_device* in, //net_device the skb arrived ingress
*             const struct net_device* out, //net_device the skb should egress
*             int (*okfn)(struct sk_buff*), //fcn ptr to okfn after all hooks done processing
*
* Return: NF_DROP, discard the skb
*         NF_ACCEPT, continue traversal in kernel network stack
*         NF_STOLEN, do not continue traversal
*         NF_QUEUE, queue skb for userspace
*         NF_REPEAT, call this callback again
*
***************************************************************************/
unsigned int pre_route_v6(unsigned int hooknum, struct sk_buff* skb, const struct net_device* in,
                          const struct net_device* out, int (*okfn)(struct sk_buff*))
{
  struct ipv6hdr* ip_hdr = (struct ipv6hdr*)skb_network_header(skb); //get L3 info;
  struct tcphdr* tcp_hdr;
  struct tcp_splice_hash_entry* hash_entry;

  //match on TCP, and IPv6 should already be matched on when registering
  if(ip_hdr->nexthdr == IPPROTO_TCP)
  {
    //sanity check to prevent overflow
    if((skb->data + sizeof(struct ipv6hdr)) >= skb->tail)
    {
      pr_debug(MODULE_NAME": %s bad ip header\n", __func__);
      return NF_DROP;
    }

    tcp_hdr = (struct tcphdr*)(skb->data + sizeof(struct ipv6hdr));

    //lookup in hashtable
    rcu_read_lock();
    hash_for_each_possible_rcu(tcp_splice_ht, hash_entry, hash_node,
                               ip_hdr->saddr.in6_u.u6_addr32[0] ^ tcp_hdr->source ^ tcp_hdr->dest)
    {
      //check direction
      if((tcp_hdr->source == rcu_dereference(hash_entry)->splice_tuple.cli_dport))
      {
        //change dest IP, dest port
        switch(rcu_dereference(hash_entry)->splice_tuple.req_family)
        {
          case AF_INET: //IPv6 to IPv4
          {
            ip_splice_v6_to_v4(rcu_dereference(hash_entry)->splice_tuple.req_daddr.v4_addr,
                               rcu_dereference(hash_entry)->splice_tuple.req_saddr.v4_addr,
                               rcu_dereference(hash_entry)->splice_tuple.req_dport,
                               rcu_dereference(hash_entry)->splice_tuple.req_sk_num, skb);

            //for req
            mangle_tcp_splice(tcp_hdr, rcu_dereference(hash_entry), REQ_DIRECTION);

            //set new dst route
            set_dst_route(skb, rcu_dereference(hash_entry), REQ_DIRECTION);

            //replace with actual source port
            tcp_hdr->source = rcu_dereference(hash_entry)->splice_tuple.req_sk_num_ct;

            //check for fin or rst
            check_for_fin_rst_tcp_splice(tcp_hdr, rcu_dereference(hash_entry));

            if(local_out_v4(skb))
            {
              pr_debug(MODULE_NAME": %s, error with local_out_v4 (%d)\n", __func__, __LINE__);
            }
            rcu_read_unlock();

            return NF_STOLEN; //I'll make sure skb arrives to dev_queue_xmit
            break;
          }
          case AF_INET6:
          {
            //change dst IP and dst port
            dnat_tcp_splice(ip_hdr, tcp_hdr, rcu_dereference(hash_entry), REQ_DIRECTION);

            //change tcp hdr
            mangle_tcp_splice(tcp_hdr, rcu_dereference(hash_entry), REQ_DIRECTION);

            //change src IP, and src port
            snat_tcp_splice(ip_hdr, tcp_hdr, rcu_dereference(hash_entry), REQ_DIRECTION);

            //set new dst route
            set_dst_route(skb, rcu_dereference(hash_entry), REQ_DIRECTION);

            break;
          }
          default:
          {
            rcu_read_unlock();
            return NF_DROP;
            break;
          }
        }

        rcu_read_unlock();
        goto checksum;

      } else if(tcp_hdr->source == rcu_dereference(hash_entry)->splice_tuple.req_dport)
      {
        //change dest IP, dest port
        switch(rcu_dereference(hash_entry)->splice_tuple.cli_family)
        {
          case AF_INET: //IPv6 to IPv4
          {
            ip_splice_v6_to_v4(rcu_dereference(hash_entry)->splice_tuple.cli_daddr.v4_addr,
                               rcu_dereference(hash_entry)->splice_tuple.cli_saddr.v4_addr,
                               rcu_dereference(hash_entry)->splice_tuple.cli_dport,
                               rcu_dereference(hash_entry)->splice_tuple.cli_sk_num, skb);

            //for cli
            mangle_tcp_splice(tcp_hdr, rcu_dereference(hash_entry), CLI_DIRECTION);

            //set new dst route
            set_dst_route(skb, rcu_dereference(hash_entry), CLI_DIRECTION);

            //replace with actual source port
            tcp_hdr->source = rcu_dereference(hash_entry)->splice_tuple.cli_sk_num_ct;

            //check for fin or rst
            check_for_fin_rst_tcp_splice(tcp_hdr, rcu_dereference(hash_entry));

            if(local_out_v4(skb))
            {
              pr_debug(MODULE_NAME": %s, error with local_out_v4 (%d)\n", __func__, __LINE__);
            }
            rcu_read_unlock();

            return NF_STOLEN; //I'll make sure skb arrives to dev_queue_xmit
            break;
          }
          case AF_INET6:
          {
            //change dst IP and dst port
            dnat_tcp_splice(ip_hdr, tcp_hdr, rcu_dereference(hash_entry), CLI_DIRECTION);

            //change tcp hdr
            mangle_tcp_splice(tcp_hdr, rcu_dereference(hash_entry), CLI_DIRECTION);

            //change src IP, and src port
            snat_tcp_splice(ip_hdr, tcp_hdr, rcu_dereference(hash_entry), CLI_DIRECTION);

            //set new dst route
            set_dst_route(skb, rcu_dereference(hash_entry), CLI_DIRECTION);

            break;
          }
          default:
          {
            rcu_read_unlock();
            return NF_DROP;
            break;
          }
        }

        rcu_read_unlock();
        goto checksum;
      }

    }
    rcu_read_unlock();
  }

  return NF_ACCEPT;

  checksum:

    //recompute L4 checksum
    tcp_hdr->check = 0x000;
    tcp_hdr->check = csum_ipv6_magic(&ip_hdr->saddr, &ip_hdr->daddr,
                     ntohs(ip_hdr->payload_len), IPPROTO_TCP,
                     csum_partial(tcp_hdr, ntohs(ip_hdr->payload_len), 0));

    //check for fin or rst
    check_for_fin_rst_tcp_splice(tcp_hdr, hash_entry);

    if(NULL == skb_dst(skb))
    {
      pr_debug(MODULE_NAME": skb_dst(skb) is NULL! (%d)\n", __LINE__);
      kfree_skb(skb);
      return -1;
    }

    ip6_local_out(skb);
    return NF_STOLEN;
}

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
unsigned int local_out_v6(struct sk_buff* skb)
{
  struct ipv6hdr* ip_hdr = (struct ipv6hdr*)skb_network_header(skb); //get L3 info;
  struct tcphdr* tcp_hdr;

  //sanity check to prevent overflow
  if((skb->data + sizeof(struct ipv6hdr)) >= skb->tail)
  {
    pr_debug(MODULE_NAME": %s bad ip header\n", __func__);
    kfree_skb(skb);
    return -1;
  }

  tcp_hdr = (struct tcphdr*)(skb->data + sizeof(struct ipv6hdr));

  if(NULL == skb_dst(skb))
  {
    pr_debug(MODULE_NAME": skb_dst(skb) is NULL! (%d)\n", __LINE__);
    kfree_skb(skb);
    return -1;
  }

  //recompute L4 checksum
  tcp_hdr->check = 0x000;
  tcp_hdr->check = csum_ipv6_magic(&ip_hdr->saddr, &ip_hdr->daddr,
                   ntohs(ip_hdr->payload_len), IPPROTO_TCP,
                   csum_partial(tcp_hdr, ntohs(ip_hdr->payload_len), 0));

  return ip6_local_out(skb); //pass pkt along its traversal
}

/***************************************************************************
*
* Function: ip_splice_v4_to_v6
*
* Description: splices a v4 skb into a v6 skb
*
* Parameters: struct in6_addr v6_daddr, // IPv6 daddr to push
*             struct in6_addr v6_saddr, // IPv6 saddr to push
*             unsigned short dest_port, // TCP dest port to push
*             unsigned short src_port, // TCP src port to push
*             struct sk_buff* skb, //the skb
*
* Return: none
*
***************************************************************************/
void ip_splice_v4_to_v6(struct in6_addr v6_daddr, struct in6_addr v6_saddr,
                        unsigned short dest_port, unsigned short src_port, struct sk_buff* skb)
{
  struct ipv6hdr* ip6_hdr;
  struct iphdr* ip_hdr;
  struct tcphdr* tcp_hdr;
  unsigned short payload_len;
  unsigned char ttl;

  if(NULL == skb)
  {
    pr_debug(MODULE_NAME": %s skb NULL!\n", __func__);
    return;
  }

  ip_hdr = (struct iphdr*)skb_network_header(skb); //get location of L3 hdr;

  //sanity check to prevent overflow
  if((skb->data + (IPV4_HDR_LEN_BYTES(ip_hdr->ihl))) >= skb->tail)
  {
    pr_debug(MODULE_NAME": %s bad ip header\n", __func__);
    return;
  }
  tcp_hdr = (struct tcphdr*)(skb->data + (IPV4_HDR_LEN_BYTES(ip_hdr->ihl)));
  payload_len = (unsigned short)(ntohs(ip_hdr->tot_len) - (IPV4_HDR_LEN_BYTES(ip_hdr->ihl)) -
                                      (tcp_hdr->doff << 2));
  ttl = ip_hdr->ttl;

  if(skb_headroom(skb) > IPV6_IPV4_HEADER_DIFF)
  {
    skb_push(skb, IPV6_IPV4_HEADER_DIFF);
    skb_reset_network_header(skb);

    ip6_hdr = (struct ipv6hdr*)skb_network_header(skb);
    memset(ip6_hdr, 0, sizeof(struct ipv6hdr));

    ip6_hdr->version = 6; //IPv6
    ip6_hdr->payload_len = htons((tcp_hdr->doff << 2) + payload_len);
    ip6_hdr->nexthdr = IPPROTO_TCP;
    if((--ttl) <= 0)
    {
      pr_info(MODULE_NAME": %s TTL will be zero, dropping skb\n", __func__);
      return;
    } else {
      ip6_hdr->hop_limit = ttl;
    }
    ip6_hdr->saddr = v6_saddr;
    ip6_hdr->daddr = v6_daddr;

    tcp_hdr->source = src_port;
    tcp_hdr->dest = dest_port;

    //skb_data must be pointing at ip_hdr
    skb->protocol = htons(ETH_P_IPV6);

    skb->ip_summed = CHECKSUM_UNNECESSARY;

  } else {
    pr_debug(MODULE_NAME": Uh oh, we don't have enough headroom for IPv6 hdr...\n");
    return;
  }

  return;
}

/***************************************************************************
*
* Function: ip_splice_v6_to_v4
*
* Description: splices a v6 skb into a v4 skb
*
* Parameters: unsigned int v4_daddr, // IPv4 daddr to push
*             unsigned int v4_saddr, // IPv4 saddr to push
*             unsigned short dest_port, // TCP dest port to push
*             unsigned short src_port, // TCP src port to push
*             struct sk_buff* skb, //the skb
*
* Return: none
*
***************************************************************************/
void ip_splice_v6_to_v4(unsigned int v4_daddr, unsigned int v4_saddr, unsigned short dest_port,
                        unsigned short src_port, struct sk_buff* skb)
{
  struct iphdr* ip_hdr;
  struct ipv6hdr* ip6_hdr;
  struct tcphdr* tcp_hdr;
  unsigned short payload_len;
  unsigned char ttl;

  if(NULL == skb)
  {
    pr_debug(MODULE_NAME": %s skb NULL!\n", __func__);
    return;
  }

  //sanity check to prevent overflow
  if((skb->data + sizeof(struct ipv6hdr)) >= skb->tail)
  {
    pr_debug(MODULE_NAME": %s bad ip header\n", __func__);
    return;
  }

  ip6_hdr = (struct ipv6hdr*)skb_network_header(skb); //get location of L3 hdr
  tcp_hdr = (struct tcphdr*)(skb->data + sizeof(struct ipv6hdr));
  payload_len = (unsigned short)ntohs(ip6_hdr->payload_len);
  ttl = ip6_hdr->hop_limit;

  skb_pull(skb, IPV6_IPV4_HEADER_DIFF);
  skb_reset_network_header(skb);

  ip_hdr = (struct iphdr*)skb_network_header(skb);
  memset(ip_hdr, 0, sizeof(struct iphdr));

  ip_hdr->version = 4; //IPv4
  ip_hdr->ihl = 5; //standard IPv4 hdr len
  ip_hdr->tot_len = htons(payload_len + (tcp_hdr->doff << 2) + (IPV4_HDR_LEN_BYTES(ip_hdr->ihl)));

  if((--ttl) <= 0)
  {
    pr_info(MODULE_NAME": TTL will be zero, dropping skb\n");
    return;
  } else {
    ip_hdr->ttl = ttl;
  }
  ip_hdr->protocol = IPPROTO_TCP;

  ip_hdr->saddr = v4_saddr;
  ip_hdr->daddr = v4_daddr;

  tcp_hdr->source = src_port;
  tcp_hdr->dest = dest_port;

  skb->protocol = htons(ETH_P_IP);

  skb->ip_summed = CHECKSUM_UNNECESSARY;

  //skb_data must be pointing at ip_hdr
  return;
}

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
void check_for_fin_rst_tcp_splice(struct tcphdr* tcp_hdr, struct tcp_splice_hash_entry* hash_entry)
{
  struct tcp_splice_hash_entry* temp_node = NULL;

  if(tcp_hdr->fin || tcp_hdr->rst)
  {
    pr_debug(MODULE_NAME": received FIN or RST!\n");

    spin_lock(&ht_lock);

    //notify userpsace to close the fds
    sendToUserspace(hash_entry);

    //delete the hash entry for opposite direction
    hash_for_each_possible_rcu(tcp_splice_ht, temp_node, hash_node, hash_entry->opp_key)
    {
      if((hash_entry->splice_tuple.cli_dport == temp_node->splice_tuple.cli_dport) &&
         (hash_entry->splice_tuple.cli_sk_num_ct == temp_node->splice_tuple.cli_sk_num_ct) &&
         (hash_entry->splice_tuple.req_dport == temp_node->splice_tuple.req_dport) &&
         (hash_entry->splice_tuple.req_sk_num_ct == temp_node->splice_tuple.req_sk_num_ct))
      {
        hash_del_rcu(&(temp_node->hash_node));
        break;
      }
    }

    //delete the hash entry for this direction
    hash_del_rcu(&(hash_entry->hash_node));
    spin_unlock(&ht_lock);

    if(NULL != temp_node)
    {
      call_rcu(&(temp_node->rcu), tcp_splice_ht_free_entry);
    }

    call_rcu(&(hash_entry->rcu), tcp_splice_ht_free_entry);
  }

  return;
}

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
void tcp_splice_ht_free_entry(struct rcu_head* head)
{
  kfree(container_of(head, struct tcp_splice_hash_entry, rcu));
  return;
}

module_init(tcp_splice_init)
module_exit(tcp_splice_exit)
MODULE_LICENSE("GPL");
