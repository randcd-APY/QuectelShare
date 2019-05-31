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
* File: tcp-splice-interface.c
*
* Description: Provides functions to interfacing with userspace.
*
***************************************************************************/

#include "tcp-splice-interface.h"

extern int debug_mode;
extern struct sock* nl_socket;
extern struct sockaddr_nl userspace_addr;
extern struct hlist_head tcp_splice_ht[1 << TCP_SPLICE_HASH_INDEX_SHIFT_SIZE];
extern spinlock_t ht_lock;

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
void listenFromUserspace(struct sk_buff* skb)
{
  struct nlmsghdr* nlh;

  if(NULL == skb)
  {
    pr_debug(MODULE_NAME": %s skb NULL!\n", __func__);
    return;
  }

  nlh = (struct nlmsghdr*)skb->data;

  pr_info(MODULE_NAME": %s received netlink msg from userspace pid: %u\n", __func__,
          nlh->nlmsg_pid);

  userspace_addr.nl_pid = nlh->nlmsg_pid;

  return;
}

/***************************************************************************
*
* Function: sendToUserspace
*
* Description: send netlink msg to userspace that the socket pair fd has
*              seen a FIN or RST
*
* Parameters: struct tcp_splice_hash_entry* hash_entry; //socket pair fd
*
* Return: none
*
***************************************************************************/
void sendToUserspace(struct tcp_splice_hash_entry* hash_entry)
{
  struct nlmsghdr* nlmhdr;
  struct sk_buff* skb;
  struct userspace_relay_session* data_ptr;

  if(NULL == hash_entry)
  {
    pr_debug(MODULE_NAME": %s hash_entry NULL!\n", __func__);
    return;
  }

  //allocate new skb, nlmsg_unicast takes care of the freeing
  if((skb = nlmsg_new(sizeof(struct userspace_relay_session), GFP_ATOMIC)) == NULL)
  {
    pr_debug(MODULE_NAME": %s skb NULL!\n", __func__);
    return;
  }

  //attach the payload to the skb
  if((nlmhdr = nlmsg_put(skb, 0, 0, NLMSG_DONE, sizeof(struct userspace_relay_session), 0)) == NULL)
  {
    pr_debug(MODULE_NAME": %s nlmhdr NULL!\n", __func__);
    kfree_skb(skb);
    return;
  }

  //we're not multicast
  NETLINK_CB(skb).dst_group = 0;

  //ptr to the head of the payload
  if((data_ptr = (struct userspace_relay_session*)NLMSG_DATA(nlmhdr)) == NULL)
  {
    pr_debug(MODULE_NAME": %s data_ptr NULL!\n", __func__);
    kfree_skb(skb);
    return;
  }

  *data_ptr = hash_entry->session_info.r_session;

  if(nlmsg_unicast(nl_socket, skb, userspace_addr.nl_pid) < 0)
  {
    pr_err(MODULE_NAME": %s error sending netlink msg to userspace\n", __func__);
  }

  pr_debug(MODULE_NAME": %s notification of FIN socket pair sent to userspace\n",__func__);

  return;
}

/***************************************************************************
*
* Function: tcp_splice_socket
*
* Description: callback for setsockopt SO_TCP_SPLICE
*
* Parameters: struct sock* sk; //socket fd
*             int optval; option value
*             void __user *user; user data
*             unsigned int len; option length
*
* Return: 0, success
*        -1, otherwise
*
***************************************************************************/
int tcp_splice_socket(struct sock* sk, int optval, void __user *user, unsigned int len)
{
  int err = 0;
  struct tcp_splice_session_info splice_session_info;

  if(NULL == sk)
  {
    pr_err(MODULE_NAME": %s sock NULL!\n", __func__);
    return -EINVAL;
  } else if(NULL == user)
  {
    pr_err(MODULE_NAME": %s user NULL!\n", __func__);
    return -EINVAL;
  }

  //not checking for capabilities of user
  switch(optval)
  {
    case SO_TCP_SPLICE:
    {
      //copy_from_user and not using netlink socket because filling out the socket
      //splice info needs to be scheduled now
      if(copy_from_user(&(splice_session_info.r_session), user, len) != 0)
      {
        pr_err(MODULE_NAME": %s error copying from userspace\n", __func__);
        return -EINVAL;
      }

      if((splice_session_info.cli_socket = sockfd_lookup(splice_session_info.r_session.cli_fd,
                                                          &err)) == NULL)
      {
        pr_err(MODULE_NAME": %s error looking up client socket from given userspace fd: "
              "%d\n", __func__, err);
        return -EBADF;
      }
      sockfd_put(splice_session_info.cli_socket);

      if((splice_session_info.req_socket = sockfd_lookup(splice_session_info.r_session.req_fd,
                                                          &err)) == NULL)
      {

        pr_err(MODULE_NAME": %s error looking up req socket from given userspace fd: "
              "%d\n", __func__, err);
        return -EBADF;
      }
      sockfd_put(splice_session_info.req_socket);

      if(debug_mode)
      {
        printSessionInfo(&splice_session_info);
      }

      if(insertSpliceInfoTuple(&splice_session_info) != 0)
      {
        pr_debug(MODULE_NAME": %s error with inserting tcp splice info tuple setsockopt\n",
                 __func__);
        return -EBADF;
      }

      pr_debug(MODULE_NAME": %s setsockopt tcpsplice successful\n", __func__);
      break;
    }
    default:
    {
      pr_err(MODULE_NAME": %s error, wrong tcpsplice setsockopt\n",__func__);
      return -EBADF;
    }
  }

  return 0;

}

/***************************************************************************
*
* Function: insertSpliceInfoTuple
*
* Description: inserts tcp splice info into global hash table
*
* Parameters: struct tcp_splice_session_info* sp_session; //ptr to tcp splice session
*
* Return: 0, success
*        -1, otherwise
*
***************************************************************************/
int insertSpliceInfoTuple(struct tcp_splice_session_info* sp_session)
{
  unsigned char i;
  int j;
  unsigned int rcv_queue_bytes;
  unsigned int key[2]; //first key will be for client, second key for req
  struct nf_conntrack_tuple conn_tuple[2]; //index 0 for client, index 1 for req
  struct sk_buff* skb;
  struct nf_conntrack_tuple_hash* cli_conn_hash = NULL;
  struct nf_conntrack_tuple_hash* req_conn_hash = NULL;
  struct nf_conn* cli_ct = NULL;
  struct nf_conn* req_ct = NULL;
  struct tcp_sock* cli_tcp_sock = NULL;
  struct tcp_sock* req_tcp_sock = NULL;
  struct tcp_splice_hash_entry* hash_entry = NULL;

  if(NULL == sp_session)
  {
    pr_err(MODULE_NAME": %s splice session NULL!\n", __func__);
    return -EINVAL;
  }

  cli_tcp_sock = tcp_sk(sp_session->cli_socket->sk);
  req_tcp_sock = tcp_sk(sp_session->req_socket->sk);

  memset(conn_tuple, 0, sizeof(conn_tuple));

  //generate cli direction key
  switch(sp_session->cli_socket->sk->sk_family)
  {
    case AF_INET:
    {
      conn_tuple[0].src.l3num = AF_INET;
      conn_tuple[0].src.u3.ip = sp_session->cli_socket->sk->sk_rcv_saddr;
      conn_tuple[0].src.u.tcp.port = htons(sp_session->cli_socket->sk->sk_num);
      conn_tuple[0].dst.u3.ip = sp_session->cli_socket->sk->sk_daddr;
      conn_tuple[0].dst.u.tcp.port = sp_session->cli_socket->sk->sk_dport;
      conn_tuple[0].dst.protonum = sp_session->cli_socket->sk->sk_protocol;
      conn_tuple[0].dst.dir = IP_CT_DIR_ORIGINAL;
      if((cli_conn_hash = nf_conntrack_find_get(sock_net(sp_session->cli_socket->sk),
                                                NF_CT_DEFAULT_ZONE, &conn_tuple[0])) == NULL)
      {
        pr_debug(MODULE_NAME": %s couldn't find get the client socket conntrack hash\n",
                 __func__);
        return -EBADF;
      }

      if((cli_ct = nf_ct_tuplehash_to_ctrack(cli_conn_hash)) == NULL)
      {
        pr_debug(MODULE_NAME": %s couldn't convert client hash to "
                 "conntrack entry\n", __func__);
        return -EBADF;
      }

      key[0] = sp_session->cli_socket->sk->sk_daddr ^ sp_session->cli_socket->sk->sk_dport ^
               htons(sp_session->cli_socket->sk->sk_num);

      break;
    }
    case AF_INET6:
    {
      conn_tuple[0].src.l3num = AF_INET6;
      memcpy((void*)&conn_tuple[0].src.u3.in6,
             (void*)&sp_session->cli_socket->sk->sk_v6_rcv_saddr.in6_u, sizeof(struct in6_addr));
      conn_tuple[0].src.u.tcp.port = htons(sp_session->cli_socket->sk->sk_num);
      memcpy((void*)&conn_tuple[0].dst.u3.in6,
             (void*)&sp_session->cli_socket->sk->sk_v6_daddr.in6_u, sizeof(struct in6_addr));
      conn_tuple[0].dst.u.tcp.port = sp_session->cli_socket->sk->sk_dport;
      conn_tuple[0].dst.protonum = sp_session->cli_socket->sk->sk_protocol;
      conn_tuple[0].dst.dir = IP_CT_DIR_ORIGINAL;
      if((cli_conn_hash = nf_conntrack_find_get(sock_net(sp_session->cli_socket->sk),
                                            NF_CT_DEFAULT_ZONE, &conn_tuple[0])) == NULL)
      {
        pr_debug(MODULE_NAME": %s couldn't find get the client socket conntrack hash\n",
                 __func__);
        return -EBADF;
      }

      if((cli_ct = nf_ct_tuplehash_to_ctrack(cli_conn_hash)) == NULL)
      {
        pr_debug(MODULE_NAME": %s couldn't convert client hash to conntrack entry\n",
                 __func__);
        return -EBADF;
      }

      key[0] = sp_session->cli_socket->sk->sk_v6_daddr.in6_u.u6_addr32[0] ^
               sp_session->cli_socket->sk->sk_dport ^ htons(sp_session->cli_socket->sk->sk_num);
      break;
    }
    default:
    {
      pr_err(MODULE_NAME": %s cli socket not AF_INET or AF_INET6 family\n", __func__);
      break;
    }
  }

  //generate req direction key
  switch(sp_session->req_socket->sk->sk_family)
  {
    case AF_INET:
    {
      conn_tuple[1].src.l3num = AF_INET;
      conn_tuple[1].src.u3.ip = sp_session->req_socket->sk->sk_rcv_saddr;
      conn_tuple[1].src.u.tcp.port = htons(sp_session->req_socket->sk->sk_num);
      conn_tuple[1].dst.u3.ip = sp_session->req_socket->sk->sk_daddr;
      conn_tuple[1].dst.u.tcp.port = sp_session->req_socket->sk->sk_dport;
      conn_tuple[1].dst.protonum = sp_session->req_socket->sk->sk_protocol;
      conn_tuple[1].dst.dir = IP_CT_DIR_ORIGINAL;
      if((req_conn_hash = nf_conntrack_find_get(sock_net(sp_session->req_socket->sk),
                                                NF_CT_DEFAULT_ZONE, &conn_tuple[1])) == NULL)
      {
        pr_debug(MODULE_NAME": %s couldn't find get the req socket conntrack hash\n",
                 __func__);
        nf_ct_put(cli_ct); //clean up
        return -EBADF;
      }

      if((req_ct = nf_ct_tuplehash_to_ctrack(req_conn_hash)) == NULL)
      {
        pr_debug(MODULE_NAME": %s couldn't convert req hash to conntrack entry\n",
                 __func__);
        nf_ct_put(cli_ct); //clean up
        return -EBADF;
      }

      key[1] = sp_session->req_socket->sk->sk_daddr ^ sp_session->req_socket->sk->sk_dport ^
               htons(sp_session->req_socket->sk->sk_num);
      break;
    }
    case AF_INET6:
    {
      conn_tuple[1].src.l3num = AF_INET6;
      memcpy((void*)&conn_tuple[1].src.u3.in6,
             (void*)&sp_session->req_socket->sk->sk_v6_rcv_saddr.in6_u, sizeof(struct in6_addr));
      conn_tuple[1].src.u.tcp.port = htons(sp_session->req_socket->sk->sk_num);
      memcpy((void*)&conn_tuple[1].dst.u3.in6,
             (void*)&sp_session->req_socket->sk->sk_v6_daddr.in6_u, sizeof(struct in6_addr));
      conn_tuple[1].dst.u.tcp.port = sp_session->req_socket->sk->sk_dport;
      conn_tuple[1].dst.protonum = sp_session->req_socket->sk->sk_protocol;
      conn_tuple[1].dst.dir = IP_CT_DIR_ORIGINAL;
      if((req_conn_hash = nf_conntrack_find_get(sock_net(sp_session->req_socket->sk),
                                            NF_CT_DEFAULT_ZONE, &conn_tuple[1])) == NULL)
      {
        pr_debug(MODULE_NAME": %s couldn't find get the req socket conntrack hash\n",
                 __func__);
        nf_ct_put(cli_ct); //clean up
        return -EBADF;
      }

      if((req_ct = nf_ct_tuplehash_to_ctrack(req_conn_hash)) == NULL)
      {
        pr_debug(MODULE_NAME": %s couldn't convert req hash to conntrack entry\n",
                 __func__);
        nf_ct_put(cli_ct); //clean up
        return -EBADF;
      }

      key[1] = sp_session->req_socket->sk->sk_v6_daddr.in6_u.u6_addr32[0] ^
               sp_session->req_socket->sk->sk_dport ^ htons(sp_session->req_socket->sk->sk_num);
      break;
    }
    default:
    {
      pr_err(MODULE_NAME": %s req socket not AF_INET or AF_INET6 family\n", __func__);
      break;
    }
  }

  pr_debug(MODULE_NAME": insertSpliceInfoTuple: hash_key[0] = %u\n", key[0]);
  pr_debug(MODULE_NAME": insertSpliceInfoTuple: hash_key[1] = %u\n", key[1]);

  //adding two hastable entries: one for cli->req and req->cli
  for(i = 0; i < 2; i++)
  {
    hash_entry = NULL;

    if((hash_entry = kzalloc(sizeof(struct tcp_splice_hash_entry), GFP_ATOMIC)) == NULL)
    {
      pr_err(MODULE_NAME": %s kzalloc failed!\n", __func__);
      if(i)
      {
        //delete and free the other previous hash_entry
        pr_debug(MODULE_NAME": %s deleting previous hash_entry\n", __func__);
        spin_lock_bh(&ht_lock);
        hash_for_each_possible_rcu(tcp_splice_ht, hash_entry, hash_node, key[0])
        {
          if((hash_entry->splice_tuple.cli_dport == sp_session->cli_socket->sk->sk_dport) &&
            (hash_entry->splice_tuple.cli_sk_num_ct == cli_ct->tuplehash[0].tuple.dst.u.tcp.port) &&
            (hash_entry->splice_tuple.cli_iss == cli_tcp_sock->snd_nxt) &&
            (hash_entry->splice_tuple.req_dport == sp_session->req_socket->sk->sk_dport) &&
            (hash_entry->splice_tuple.req_sk_num_ct == req_ct->tuplehash[1].tuple.dst.u.tcp.port))
          {
            hash_del_rcu(&(hash_entry->hash_node));
          }
        }
        spin_unlock_bh(&ht_lock);
        synchronize_rcu();
        kfree(hash_entry);
      }

      return -ENOMEM;
    }

    //load client socket FD info
    //depending on IPv4 or IPv6 socket
    switch(sp_session->cli_socket->sk->sk_family)
    {
      case AF_INET:
      {
        hash_entry->splice_tuple.cli_daddr.v4_addr = sp_session->cli_socket->sk->sk_daddr;
        hash_entry->splice_tuple.cli_saddr.v4_addr = sp_session->cli_socket->sk->sk_rcv_saddr;
        break;
      }
      case AF_INET6:
      {
        hash_entry->splice_tuple.cli_daddr.v6_addr = sp_session->cli_socket->sk->sk_v6_daddr;
        hash_entry->splice_tuple.cli_saddr.v6_addr = sp_session->cli_socket->sk->sk_v6_rcv_saddr;
        break;

      }
      default:
      {
        pr_debug(MODULE_NAME": %s cli_sk_family = 0x%x\n", __func__,
              sp_session->cli_socket->sk->sk_family);
        break;
      }
    }
    hash_entry->splice_tuple.cli_family = sp_session->cli_socket->sk->sk_family;
    hash_entry->splice_tuple.cli_dport = sp_session->cli_socket->sk->sk_dport;
    hash_entry->splice_tuple.cli_sk_num = htons(sp_session->cli_socket->sk->sk_num);
    hash_entry->splice_tuple.cli_sk_num_ct = cli_ct->tuplehash[0].tuple.dst.u.tcp.port;
    hash_entry->splice_tuple.cli_iss = cli_tcp_sock->snd_nxt;
    hash_entry->splice_tuple.cli_irs = cli_tcp_sock->rcv_nxt;

    //do a routing table lookup now
    if((hash_entry->splice_tuple.cli_dst = sk_dst_get(sp_session->cli_socket->sk)) == NULL)
    {
      pr_err(MODULE_NAME": sk_dst_get returned NULL for cli socket\n");
      nf_ct_put(cli_ct); //clean up
      nf_ct_put(req_ct); //clean up

      if(i)
      {
        //delete and free the other previous hash_entry
        pr_debug(MODULE_NAME": %s deleting previous hash_entry\n", __func__);
        spin_lock_bh(&ht_lock);
        hash_for_each_possible_rcu(tcp_splice_ht, hash_entry, hash_node, key[0])
        {
          if((hash_entry->splice_tuple.cli_dport == sp_session->cli_socket->sk->sk_dport) &&
            (hash_entry->splice_tuple.cli_sk_num_ct == cli_ct->tuplehash[0].tuple.dst.u.tcp.port) &&
            (hash_entry->splice_tuple.cli_iss == cli_tcp_sock->snd_nxt) &&
            (hash_entry->splice_tuple.req_dport == sp_session->req_socket->sk->sk_dport) &&
            (hash_entry->splice_tuple.req_sk_num_ct == req_ct->tuplehash[1].tuple.dst.u.tcp.port))
          {
            hash_del_rcu(&(hash_entry->hash_node));
          }
        }
        spin_unlock_bh(&ht_lock);
        synchronize_rcu();
        kfree(hash_entry);
      } else {
        kfree(hash_entry);
      }
      return -EBADF;
    }

    //load req socket FD info
    //depending on IPv4 or IPv6 socket
    switch(sp_session->req_socket->sk->sk_family)
    {
      case AF_INET:
      {
        hash_entry->splice_tuple.req_daddr.v4_addr = sp_session->req_socket->sk->sk_daddr;
        hash_entry->splice_tuple.req_saddr.v4_addr = sp_session->req_socket->sk->sk_rcv_saddr;
        break;
      }
      case AF_INET6:
      {
        hash_entry->splice_tuple.req_daddr.v6_addr = sp_session->req_socket->sk->sk_v6_daddr;
        hash_entry->splice_tuple.req_saddr.v6_addr = sp_session->req_socket->sk->sk_v6_rcv_saddr;
        break;

      }
      default:
      {
        pr_debug(MODULE_NAME": %s req_sk_family = 0x%x\n", __func__,
                sp_session->req_socket->sk->sk_family);
        break;
      }
    }
    hash_entry->splice_tuple.req_family = sp_session->req_socket->sk->sk_family;

    hash_entry->splice_tuple.req_dport = sp_session->req_socket->sk->sk_dport;
    hash_entry->splice_tuple.req_sk_num = htons(sp_session->req_socket->sk->sk_num);
    hash_entry->splice_tuple.req_sk_num_ct = req_ct->tuplehash[1].tuple.dst.u.tcp.port;

    j = sp_session->cli_socket->sk->sk_receive_queue.qlen;
    rcv_queue_bytes = 0;
    skb = sp_session->cli_socket->sk->sk_receive_queue.next;
    //iterate through the receive queue
    while((NULL != skb) && (j > 0))
    {
      rcv_queue_bytes += skb->len;
      skb = skb->next;
      j--;
    }

    hash_entry->splice_tuple.req_iss = req_tcp_sock->snd_nxt + rcv_queue_bytes;
    //hash_entry->splice_tuple.req_iss = req_tcp_sock->snd_nxt;
    hash_entry->splice_tuple.req_irs = req_tcp_sock->rcv_nxt;

    //do a routing table lookup now
    if((hash_entry->splice_tuple.req_dst = sk_dst_get(sp_session->req_socket->sk)) == NULL)
    {
      pr_err(MODULE_NAME": %s sk_dst_get returned NULL for req socket\n", __func__);
      nf_ct_put(cli_ct); //clean up
      nf_ct_put(req_ct); //clean up

      if(i)
      {
        //delete and free the other previous hash_entry
        pr_debug(MODULE_NAME": %s deleting previous hash_entry\n", __func__);
        spin_lock_bh(&ht_lock);
        hash_for_each_possible_rcu(tcp_splice_ht, hash_entry, hash_node, key[0])
        {
          if((hash_entry->splice_tuple.cli_dport == sp_session->cli_socket->sk->sk_dport) &&
            (hash_entry->splice_tuple.cli_sk_num_ct == cli_ct->tuplehash[0].tuple.dst.u.tcp.port) &&
            (hash_entry->splice_tuple.cli_iss == cli_tcp_sock->snd_nxt) &&
            (hash_entry->splice_tuple.req_dport == sp_session->req_socket->sk->sk_dport) &&
            (hash_entry->splice_tuple.req_sk_num_ct == req_ct->tuplehash[1].tuple.dst.u.tcp.port))
          {
            hash_del_rcu(&(hash_entry->hash_node));
          }
        }
        spin_unlock_bh(&ht_lock);
        synchronize_rcu();
        kfree(hash_entry);
      } else {
        kfree(hash_entry);
      }
      return -EBADF;
    }

    //handling TCP options

    //Timestamp
    if(cli_tcp_sock->rx_opt.tstamp_ok && req_tcp_sock->rx_opt.tstamp_ok)
    {
      //we must splice Timestamps
      hash_entry->session_info.rx_opt_timestamp_ok = 1;

      hash_entry->splice_tuple.tstamp_option.cli_ircv_tsval = cli_tcp_sock->rx_opt.rcv_tsval;
      hash_entry->splice_tuple.tstamp_option.cli_ircv_tsecr = cli_tcp_sock->rx_opt.rcv_tsecr;

      hash_entry->splice_tuple.tstamp_option.req_ircv_tsval = req_tcp_sock->rx_opt.rcv_tsval;
      hash_entry->splice_tuple.tstamp_option.req_ircv_tsecr = req_tcp_sock->rx_opt.rcv_tsecr;

    } else {
      hash_entry->session_info.rx_opt_timestamp_ok = 0;
    }

    //Window Scaling
    memset(&hash_entry->splice_tuple.wscale_option, 0, sizeof(struct window_scale_splice_tuple));

    if(cli_tcp_sock->rx_opt.wscale_ok)
    {
      //we need to window size splice
      hash_entry->session_info.rx_opt_wscale_ok |= CLIENT_SUPPORTS_WINDOW_SCALING;
      hash_entry->splice_tuple.wscale_option.cli_isnd_wscale = cli_tcp_sock->rx_opt.snd_wscale;
      hash_entry->splice_tuple.wscale_option.cli_ircv_wscale = cli_tcp_sock->rx_opt.rcv_wscale;
    }

    if(req_tcp_sock->rx_opt.wscale_ok)
    {
      //we need to window size splice
      hash_entry->session_info.rx_opt_wscale_ok |= REMOTE_SUPPORTS_WINDOW_SCALING;
      hash_entry->splice_tuple.wscale_option.req_isnd_wscale = req_tcp_sock->rx_opt.snd_wscale;
      hash_entry->splice_tuple.wscale_option.req_ircv_wscale = req_tcp_sock->rx_opt.rcv_wscale;
    }

    //SACK
    if(cli_tcp_sock->rx_opt.sack_ok & req_tcp_sock->rx_opt.sack_ok & TCP_SACK_SEEN)
    {
      //we must splice SACKs
      hash_entry->session_info.rx_opt_sack_ok = 1;

    } else {
      hash_entry->session_info.rx_opt_sack_ok = 0;
    }

    //load session info
    hash_entry->session_info.r_session.cli_fd = sp_session->r_session.cli_fd;
    hash_entry->session_info.r_session.req_fd = sp_session->r_session.req_fd;
    hash_entry->session_info.cli_socket = sp_session->cli_socket;
    hash_entry->session_info.req_socket = sp_session->req_socket;

    //add keys to opposing directions
    if(i)
    {
      hash_entry->opp_key = key[0];
    } else {
      hash_entry->opp_key = key[1];
    }

    spin_lock_bh(&ht_lock);
    hash_add_rcu(tcp_splice_ht, &(hash_entry->hash_node), key[i]);
    pr_debug(MODULE_NAME": inserted for key = 0x%x\n", key[i]);
    spin_unlock_bh(&ht_lock);

  }

  //clean up
  nf_ct_put(cli_ct);
  nf_ct_put(req_ct);

  return 0;
}

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
void printSessionInfo(struct tcp_splice_session_info* sp_session)
{
  struct tcp_sock* cli_tcp_sock = NULL;
  struct tcp_sock* req_tcp_sock = NULL;

  if(NULL == sp_session)
  {
    pr_err(MODULE_NAME": %s : %d sp_session is NULL\n", __func__, __LINE__);
    return;
  }

  cli_tcp_sock = tcp_sk(sp_session->cli_socket->sk);
  req_tcp_sock = tcp_sk(sp_session->req_socket->sk);

  //depending on IPv4 or IPv6 socket
  switch(sp_session->cli_socket->sk->sk_family)
  {
    case AF_INET:
    {
      pr_info(MODULE_NAME": cli_sk_family = AF_INET\n");
      pr_info(MODULE_NAME": cli_daddr = 0x%x\n", ntohl(sp_session->cli_socket->sk->sk_daddr));
      pr_info(MODULE_NAME": cli_saddr = 0x%x\n", ntohl(sp_session->cli_socket->sk->sk_rcv_saddr));
      break;
    }
    case AF_INET6:
    {
      pr_info(MODULE_NAME": cli_sk_family = AF_INET6\n");
      pr_info(MODULE_NAME": cli_v6_daddr = ");
      PRINTK_IPV6_ADDR(sp_session->cli_socket->sk->sk_v6_daddr);
      pr_info(MODULE_NAME": cli_v6_rcv_saddr = ");
      PRINTK_IPV6_ADDR(sp_session->cli_socket->sk->sk_v6_rcv_saddr);
      break;
    }
    default:
    {
      pr_info(MODULE_NAME": cli_sk_family = 0x%x\n", sp_session->cli_socket->sk->sk_family);
      break;
    }
  }

  pr_info(MODULE_NAME": cli_dport = 0x%x\n", ntohs(sp_session->cli_socket->sk->sk_dport));
  pr_info(MODULE_NAME": cli_sk_num = 0x%x\n", sp_session->cli_socket->sk->sk_num);
  pr_info(MODULE_NAME": cli_splice_iss = 0x%x\n", cli_tcp_sock->snd_nxt);
  pr_info(MODULE_NAME": cli_splice_irs = 0x%x\n", cli_tcp_sock->rcv_nxt);
  pr_info(MODULE_NAME": cli_rx_opt.tstamp_ok = %u\n", cli_tcp_sock->rx_opt.tstamp_ok);
  pr_info(MODULE_NAME": cli_rx_opt.splice_rcv_tsval = %u\n", cli_tcp_sock->rx_opt.rcv_tsval);
  pr_info(MODULE_NAME": cli_rx_opt.splice_rcv_tsecr = %u\n", cli_tcp_sock->rx_opt.rcv_tsecr);
  pr_info(MODULE_NAME": cli_rx_opt.wscale_ok = %u\n", cli_tcp_sock->rx_opt.wscale_ok);
  pr_info(MODULE_NAME": cli_rx_opt.splice_snd_wscale = %u\n", cli_tcp_sock->rx_opt.snd_wscale);
  pr_info(MODULE_NAME": cli_rx_opt.splice_rcv_wscale = %u\n", cli_tcp_sock->rx_opt.rcv_wscale);
  pr_info(MODULE_NAME": cli_rx_opt.sack_ok = %u\n", cli_tcp_sock->rx_opt.sack_ok & TCP_SACK_SEEN);

  //depending on IPv4 or IPv6 socket
  switch(sp_session->req_socket->sk->sk_family)
  {
    case AF_INET:
    {
      pr_info(MODULE_NAME": req_sk_family = AF_INET\n");
      pr_info(MODULE_NAME": req_daddr = 0x%x\n", ntohl(sp_session->req_socket->sk->sk_daddr));
      pr_info(MODULE_NAME": req_saddr = 0x%x\n", ntohl(sp_session->req_socket->sk->sk_rcv_saddr));
      break;
    }
    case AF_INET6:
    {
      pr_info(MODULE_NAME": req_sk_family = AF_INET6\n");
      pr_info(MODULE_NAME": req_v6_daddr = ");
      PRINTK_IPV6_ADDR(sp_session->req_socket->sk->sk_v6_daddr);
      pr_info(MODULE_NAME": req_v6_rcv_saddr = ");
      PRINTK_IPV6_ADDR(sp_session->req_socket->sk->sk_v6_rcv_saddr);
      break;
    }
    default:
    {
      pr_info(MODULE_NAME": req_sk_family = 0x%x\n", sp_session->req_socket->sk->sk_family);
      break;
    }
  }

  pr_info(MODULE_NAME": req_dport = 0x%x\n", ntohs(sp_session->req_socket->sk->sk_dport));
  pr_info(MODULE_NAME": req_sk_num = 0x%x\n", sp_session->req_socket->sk->sk_num);
  pr_info(MODULE_NAME": req_splice_iss = 0x%x\n", req_tcp_sock->snd_nxt);
  pr_info(MODULE_NAME": req_splice_irs = 0x%x\n", req_tcp_sock->rcv_nxt);
  pr_info(MODULE_NAME": req_rx_opt.tstamp_ok = %u\n", req_tcp_sock->rx_opt.tstamp_ok);
  pr_info(MODULE_NAME": req_rx_opt.splice_rcv_tsval = %u\n", req_tcp_sock->rx_opt.rcv_tsval);
  pr_info(MODULE_NAME": req_rx_opt.splice_rcv_tsecr = %u\n", req_tcp_sock->rx_opt.rcv_tsecr);
  pr_info(MODULE_NAME": req_rx_opt.wscale_ok = %u\n", req_tcp_sock->rx_opt.wscale_ok);
  pr_info(MODULE_NAME": req_rx_opt.splice_snd_wscale = %u\n", req_tcp_sock->rx_opt.snd_wscale);
  pr_info(MODULE_NAME": req_rx_opt.splice_rcv_wscale = %u\n", req_tcp_sock->rx_opt.rcv_wscale);
  pr_info(MODULE_NAME": req_rx_opt.sack_ok = %u\n", req_tcp_sock->rx_opt.sack_ok & TCP_SACK_SEEN);
}
