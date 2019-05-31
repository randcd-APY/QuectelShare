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
* File: tcp-splice-util.c
*
* Description: Provides utility functions for tcp splice.
*
***************************************************************************/

#include "tcp-splice-interface.h"
#include "tcp-splice-util.h"

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
                               unsigned char timestamp_ok, unsigned char sack_ok)
{
  unsigned int* fast_ptr;
  unsigned char* slow_ptr;
  int length;
  int opcode;
  int opsize;

  if(NULL == tcp_hdr)
  {
    pr_err(MODULE_NAME": %s tcp_hdr NULL!\n", __func__);
    return -EINVAL;
  } else if(NULL == timestamp)
  {
    pr_err(MODULE_NAME": %s timestamp NULL!\n", __func__);
    return -EINVAL;
  } else if(NULL == sacks)
  {
    pr_err(MODULE_NAME": %s sacks NULL!\n", __func__);
    return -EINVAL;
  } else if(NULL == num_sacks)
  {
    pr_err(MODULE_NAME": %s num_sacks NULL!\n", __func__);
    return -EINVAL;
  }

  *timestamp = NULL;
  *num_sacks = 0;

  if((timestamp_ok) && (tcp_hdr->doff == ((sizeof(struct tcphdr) >> 2) +
                                          (TCPOLEN_TSTAMP_ALIGNED >> 2))))
  {
    fast_ptr = (unsigned int*)(tcp_hdr + 1);
    if (*fast_ptr == htonl((TCPOPT_NOP << 24) | (TCPOPT_NOP << 16) | (TCPOPT_TIMESTAMP << 8) |
                            TCPOLEN_TIMESTAMP))
    {
      ++fast_ptr;
      *timestamp = fast_ptr;
      //pr_debug(MODULE_NAME": took fast timestamp lookup path\n");

      //no point to move on if no SACKs enabled
      if(!(sack_ok))
      {
        return 0;
      }
    }
  }

  //now the slow path
  slow_ptr = (unsigned char*)(tcp_hdr + 1);
  length = ((tcp_hdr->doff) << 2) - sizeof(struct tcphdr);

  //we are slow parsing the brute force way...
  while(length > 0)
  {
    opcode = *slow_ptr++;

    switch(opcode)
    {
      case(TCPOPT_EOL): //we've reached end of options
      {
        return 0;
      }
      case(TCPOPT_NOP): //skip padding
      {
        length--;
        continue;
      }
      default:
      {
        opsize = *slow_ptr++;
        if(opsize < 2)
        {
          pr_err(MODULE_NAME": %s Recevied a bad TCP option size less than 2!\n",
                __func__);
          return -EINVAL;
        }

        if(opsize > length)
        {
          pr_err(MODULE_NAME": %s Recevied a bad TCP option size greater than length!\n",
                 __func__);
          return -EINVAL;
        }

        switch(opcode)
        {
          case(TCPOPT_TIMESTAMP):
          {
            if((timestamp_ok) && (TCPOLEN_TIMESTAMP == opsize))
            {
              *timestamp = (unsigned int*)slow_ptr;
            }
            break;

          }

          case(TCPOPT_SACK):
          {
            if((sack_ok) && (opsize >= (TCPOLEN_SACK_BASE + TCPOLEN_SACK_PERBLOCK)) &&
              !((opsize - TCPOLEN_SACK_BASE) % TCPOLEN_SACK_PERBLOCK))
            {
              *sacks = (struct tcp_sack_block*)slow_ptr;
              *num_sacks = (unsigned char)((opsize - TCPOLEN_SACK_BASE) >>
                                            TCPOLEN_SACK_PERBLOCK_SHIFT);
            }
            break;
          }

          default:
          {
            break;
          }
        }
        slow_ptr += opsize - 2;
        length -= opsize;
      }
    }
  }

  return 0;
}

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
                     int direction)
{
  struct tcp_splice_hash_entry* rcu_hash_entry = rcu_dereference(hash_entry);
  struct iphdr* ip_hdr = NULL;
  struct ipv6hdr* ip6_hdr = NULL;

  //check direction intended for
  switch(direction)
  {
    case(CLI_DIRECTION):
    {
      switch(rcu_hash_entry->splice_tuple.cli_family)
      {
        case AF_INET:
        {
          ip_hdr = (struct iphdr*)iphdr;
          ip_hdr->daddr = rcu_hash_entry->splice_tuple.cli_daddr.v4_addr;
          break;
        }
        case AF_INET6:
        {
          ip6_hdr = (struct ipv6hdr*)iphdr;
          ip6_hdr->daddr = rcu_hash_entry->splice_tuple.cli_daddr.v6_addr;
          break;
        }
        default:
        {
          break;
        }
      }
      tcp_hdr->dest = rcu_hash_entry->splice_tuple.cli_dport;
      break;
    }

    case(REQ_DIRECTION):
    {
      switch(rcu_hash_entry->splice_tuple.req_family)
      {
        case AF_INET:
        {
          ip_hdr = (struct iphdr*)iphdr;
          ip_hdr->daddr = rcu_hash_entry->splice_tuple.req_daddr.v4_addr;
          break;
        }
        case AF_INET6:
        {
          ip6_hdr = (struct ipv6hdr*)iphdr;
          ip6_hdr->daddr = rcu_hash_entry->splice_tuple.req_daddr.v6_addr;
          break;
        }
        default:
        {
          break;
        }
      }
      tcp_hdr->dest = rcu_hash_entry->splice_tuple.req_dport;
      break;
    }

    default:
    {
      pr_err(MODULE_NAME": invalid direction: %d\n", direction);
      break;
    }
  }

  return;
}

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
                       int direction)
{
  struct tcp_splice_hash_entry* rcu_hash_entry = rcu_dereference(hash_entry);
  unsigned int* timestamp;
  unsigned int window_size;
  unsigned char num_sacks;
  unsigned char i;
  struct tcp_sack_block* sacks;

  //check direction intended for
  switch(direction)
  {
    case(CLI_DIRECTION):
    {
      tcp_hdr->seq = htonl((ntohl(tcp_hdr->seq) -
                            rcu_hash_entry->splice_tuple.req_irs) +
                            rcu_hash_entry->splice_tuple.cli_iss);
      tcp_hdr->ack_seq = htonl((ntohl(tcp_hdr->ack_seq) -
                                rcu_hash_entry->splice_tuple.req_iss) +
                                rcu_hash_entry->splice_tuple.cli_irs);

      //handle TCP options: timestamp and sack
      if(rcu_hash_entry->session_info.rx_opt_timestamp_ok ||
         rcu_hash_entry->session_info.rx_opt_sack_ok)
      {
        if(qti_tcp_fast_parse_options(tcp_hdr, &timestamp, &sacks, &num_sacks,
                                     rcu_hash_entry->session_info.rx_opt_timestamp_ok,
                                     rcu_hash_entry->session_info.rx_opt_sack_ok) == 0)
        {
          //are we splicing timestamps?
          if((rcu_hash_entry->session_info.rx_opt_timestamp_ok) && (NULL != timestamp))
          {
            timestamp[0] = htonl((ntohl(timestamp[0]) -
                           rcu_hash_entry->splice_tuple.tstamp_option.req_ircv_tsval) +
                           rcu_hash_entry->splice_tuple.tstamp_option.cli_ircv_tsecr);
            timestamp[1] = htonl((ntohl(timestamp[1]) -
                           rcu_hash_entry->splice_tuple.tstamp_option.req_ircv_tsecr) +
                           rcu_hash_entry->splice_tuple.tstamp_option.cli_ircv_tsval);
          }

          //are we splicing SACKs?
          if((rcu_hash_entry->session_info.rx_opt_sack_ok) && (num_sacks > 0))
          {
            for(i = 0; i < num_sacks; i++)
            {
              sacks[i].start_seq = htonl((ntohl(sacks[i].start_seq) -
                                          rcu_hash_entry->splice_tuple.req_iss) +
                                          rcu_hash_entry->splice_tuple.cli_irs);
              sacks[i].end_seq = htonl((ntohl(sacks[i].end_seq) -
                                        rcu_hash_entry->splice_tuple.req_iss) +
                                        rcu_hash_entry->splice_tuple.cli_irs);
            }
          }
        }
      }

      //are we splicing window sizes?
      if(rcu_hash_entry->session_info.rx_opt_wscale_ok)
      {
        window_size = ((unsigned int)ntohs(tcp_hdr->window)) <<
                        rcu_hash_entry->splice_tuple.wscale_option.req_isnd_wscale;

        //does client supporte window scaling?
        if(rcu_hash_entry->session_info.rx_opt_wscale_ok &
           CLIENT_SUPPORTS_WINDOW_SCALING)
        {
          tcp_hdr->window = htons((unsigned short)(window_size >>
                          rcu_hash_entry->splice_tuple.wscale_option.cli_ircv_wscale));
        } else {

          //check boundaries
          if(window_size >= MAX_WINDOW_SIZE)
          {
            tcp_hdr->window = MAX_WINDOW_SIZE;
          } else {
            tcp_hdr->window = htons((unsigned short)window_size);
          }

        }

      }
      break;
    }

    case(REQ_DIRECTION):
    {
      tcp_hdr->seq = htonl((ntohl(tcp_hdr->seq) -
                          rcu_hash_entry->splice_tuple.cli_irs) +
                          rcu_hash_entry->splice_tuple.req_iss);
      tcp_hdr->ack_seq = htonl((ntohl(tcp_hdr->ack_seq) -
                          rcu_hash_entry->splice_tuple.cli_iss) +
                          rcu_hash_entry->splice_tuple.req_irs);

      //handle TCP options: timestamp and sack
      if(rcu_hash_entry->session_info.rx_opt_timestamp_ok ||
         rcu_hash_entry->session_info.rx_opt_sack_ok)
      {
        if(qti_tcp_fast_parse_options(tcp_hdr, &timestamp, &sacks, &num_sacks,
                                     rcu_hash_entry->session_info.rx_opt_timestamp_ok,
                                     rcu_hash_entry->session_info.rx_opt_sack_ok) == 0)
        {
          //are we splicing timestamps?
          if((rcu_hash_entry->session_info.rx_opt_timestamp_ok) && (NULL != timestamp))
          {
            timestamp[0] = htonl((ntohl(timestamp[0]) -
                           rcu_hash_entry->splice_tuple.tstamp_option.cli_ircv_tsval) +
                           rcu_hash_entry->splice_tuple.tstamp_option.req_ircv_tsecr);
            timestamp[1] = htonl((ntohl(timestamp[1]) -
                           rcu_hash_entry->splice_tuple.tstamp_option.cli_ircv_tsecr) +
                           rcu_hash_entry->splice_tuple.tstamp_option.req_ircv_tsval);
          }

          //are we splicing SACKs?
          if((rcu_hash_entry->session_info.rx_opt_sack_ok) && (num_sacks > 0))
          {
            for(i = 0; i < num_sacks; i++)
            {
                  sacks[i].start_seq = htonl((ntohl(sacks[i].start_seq) -
                                              rcu_hash_entry->splice_tuple.cli_iss) +
                                              rcu_hash_entry->splice_tuple.req_irs);
                  sacks[i].end_seq = htonl((ntohl(sacks[i].end_seq) -
                                            rcu_hash_entry->splice_tuple.cli_iss) +
                                            rcu_hash_entry->splice_tuple.req_irs);
            }
          }
        }
      }

      //are we splicing window sizes?
      if(rcu_hash_entry->session_info.rx_opt_wscale_ok)
      {
        window_size = ((unsigned int)ntohs(tcp_hdr->window)) <<
                        rcu_hash_entry->splice_tuple.wscale_option.cli_isnd_wscale;

        //does remote supporte window scaling?
        if(rcu_hash_entry->session_info.rx_opt_wscale_ok &
           REMOTE_SUPPORTS_WINDOW_SCALING)
        {
          tcp_hdr->window = htons((unsigned short)(window_size >>
                          rcu_hash_entry->splice_tuple.wscale_option.req_ircv_wscale));
        } else {

          //check boundaries
          if(window_size >= MAX_WINDOW_SIZE)
          {
            tcp_hdr->window = MAX_WINDOW_SIZE;
          } else {
            tcp_hdr->window = htons((unsigned short)window_size);
          }

        }

      }
      break;
    }

    default:
    {
      pr_err(MODULE_NAME": invalid direction: %d\n", direction);
      break;
    }
  }

  return;
}

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
                     int direction)
{
  struct tcp_splice_hash_entry* rcu_hash_entry = rcu_dereference(hash_entry);
  struct iphdr* ip_hdr = NULL;
  struct ipv6hdr* ip6_hdr = NULL;

  //check direction intended for
  switch(direction)
  {
    case(CLI_DIRECTION):
    {
      switch(rcu_hash_entry->splice_tuple.cli_family)
      {
        case AF_INET:
        {
          ip_hdr = (struct iphdr*)iphdr;
          ip_hdr->saddr = rcu_hash_entry->splice_tuple.cli_saddr.v4_addr;
          break;
        }
        case AF_INET6:
        {
          ip6_hdr = (struct ipv6hdr*)iphdr;
          ip6_hdr->saddr = rcu_hash_entry->splice_tuple.cli_saddr.v6_addr;
          break;
        }
        default:
        {
          break;
        }
      }
      tcp_hdr->source = rcu_hash_entry->splice_tuple.cli_sk_num_ct;
      break;
    }

    case(REQ_DIRECTION):
    {
      switch(rcu_hash_entry->splice_tuple.req_family)
      {
        case AF_INET:
        {
          ip_hdr = (struct iphdr*)iphdr;
          ip_hdr->saddr = rcu_hash_entry->splice_tuple.req_saddr.v4_addr;
          break;
        }
        case AF_INET6:
        {
          ip6_hdr = (struct ipv6hdr*)iphdr;
          ip6_hdr->saddr = rcu_hash_entry->splice_tuple.req_saddr.v6_addr;
          break;
        }
        default:
        {
          break;
        }
      }
      tcp_hdr->source = rcu_hash_entry->splice_tuple.req_sk_num_ct;
      break;
    }

    default:
    {
      pr_err(MODULE_NAME": invalid direction: %d\n", direction);
      break;
    }
  }

  return;
}

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
void set_dst_route(struct sk_buff* skb, struct tcp_splice_hash_entry* hash_entry, int direction)
{
  struct tcp_splice_hash_entry* rcu_hash_entry = rcu_dereference(hash_entry);

  dst_release(skb_dst(skb)); //release old dst route

  //update with new dst route
  switch(direction)
  {
    case(CLI_DIRECTION):
    {
      skb_dst_set(skb, rcu_hash_entry->splice_tuple.cli_dst);
      break;
    }
    case(REQ_DIRECTION):
    {
      skb_dst_set(skb, rcu_hash_entry->splice_tuple.req_dst);
      break;
    }
    default:
    {
      pr_err(MODULE_NAME": invalid direction: %d\n", direction);
      break;
    }
  }

  if(skb_dst(skb))
  {
    atomic_inc_return(&(skb_dst(skb)->__refcnt));
  }

  return;
}
