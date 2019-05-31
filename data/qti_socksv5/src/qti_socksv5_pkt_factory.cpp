/*==========================================================================

  FILE:  qti_socksv5_pkt_factory.cpp

  SERVICES:

  Packet factory QC_SOCKSv5 Proxy uses produce SOCKSv5 headers and payloads
  onto heap.  Must call freeSOCKSv5Pkt after sending packet.

==========================================================================*/

/*==========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

==========================================================================*/

/*==========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        ----------------------------------------------------
  03/20/17   jt         SOCKSv5 support.
==========================================================================*/

#include "qti_socksv5_pkt_factory.hpp"
#include "qti_socksv5_log_msg.h"

namespace QC_SOCKSv5_Pkt_Factory
{

  /*==========================================================================
    FUNCTION createMethodSelectionReply
  ==========================================================================*/
  /*!
  @brief
    creates method selection reply socksv5 packet

  @parameters
    RFC 1928 specified authentication method selected

  @return
    ptr to the socksv5 packet on heap
  */
  /*========================================================================*/
  struct socksv5_packet* createMethodSelectionReply(unsigned char method)
  {
    struct socksv5_packet* method_selection_reply;
    unsigned char* byte;

    if((method_selection_reply = (struct socksv5_packet*)malloc(sizeof(struct socksv5_packet)))
        == NULL)
    {
      return NULL;
    }

    method_selection_reply->payload_size = 2 * sizeof(unsigned char);
    if((method_selection_reply->payload = malloc(method_selection_reply->payload_size))
        == NULL)
    {
      free(method_selection_reply);
      return NULL;
    }

    byte = (unsigned char*)method_selection_reply->payload;

    //socks version 5
    *byte = SOCKSV5_VERSION; //socks version 5
    byte++;

    //fill in selected method
    *byte = method;

    //after server sends, server must free both the payload and socksv5 packet
    return method_selection_reply;
  }

  /*==========================================================================
    FUNCTION createCmdReply
  ==========================================================================*/
  /*!
  @brief
    creates command reply socksv5 packet

  @parameters
    RFC 1928 specified command reply status
    address type (IPv4, IPv6, or domain name)
    bind address
    bind port

  @return
    ptr to the socksv5 packet on heap
  */
  /*========================================================================*/
  struct socksv5_packet* createCmdReply
  (
    unsigned char reply,
    unsigned char addr_type,
    const char* bind_addr,
    unsigned short bind_port
  )
  {
    unsigned char* byte;
    struct socksv5_packet* cmd_reply;

    if(bind_addr == NULL)
    {
      LOG_MSG_INFO1("given a null bind addr when creating cmd reply", 0, 0, 0);
      return NULL;
    }

    if((cmd_reply = (struct socksv5_packet*)malloc(sizeof(struct socksv5_packet))) == NULL)
    {
      LOG_MSG_INFO1("failed to malloc in creating cmd reply", 0, 0, 0);
      return NULL;
    }

    switch(addr_type)
    {
      case(Addr_Type::IPV4):
      {
        cmd_reply->payload_size = 10 * sizeof(unsigned char);

        if((cmd_reply->payload = calloc(1, cmd_reply->payload_size)) == NULL)
        {
          free(cmd_reply);
          LOG_MSG_INFO1("failed to calloc in creating cmd reply", 0, 0, 0);
          return NULL;
        }

        byte = (unsigned char*)cmd_reply->payload;

        //version
        *byte = SOCKSV5_VERSION;
        byte++;

        //Reply
        *byte = reply;
        byte += 2; //skip over reserved field

        //Address Type
        *byte = addr_type;
        byte++;

        //IPV4 address
        if(inet_pton(AF_INET, bind_addr, byte) <= 0)
        {
          LOG_MSG_INFO1("error converting bind IP into network order: %s", strerror(errno), 0, 0);
          free(cmd_reply);
          return NULL;
        }
        byte += 4; //increment over the IPv4 field

        //fill out port htons order
        *byte = bind_port >> 8;
        byte++;
        *byte = bind_port;
        break;
      }
      case(Addr_Type::DOMAIN_NAME):
      {
        unsigned char bind_addr_len = (unsigned char)strlen(bind_addr);

        cmd_reply->payload_size = 5 * sizeof(unsigned char) + bind_addr_len +
                                  sizeof(unsigned short);

        if((cmd_reply->payload = calloc(1, cmd_reply->payload_size)) == NULL)
        {
          free(cmd_reply);
          LOG_MSG_INFO1("failed to calloc in creating cmd reply", 0, 0, 0);
          return NULL;
        }

        byte = (unsigned char*)cmd_reply->payload;

        //version
        *byte = SOCKSV5_VERSION;
        byte++;

        //CMD
        *byte = reply;
        byte += 2; //skip over reserved field

        //Address Type
        *byte = addr_type;
        byte++;

        //Domain Name Length
        *byte = bind_addr_len;
        byte++;

        //Domain Name
        for(int i = 0; i < (unsigned char)bind_addr_len; i++)
        {
          *byte = bind_addr[i];
          byte++;
        }

        //Port in htons order
        *byte = bind_port >> 8;
        byte++;
        *byte = bind_port;
        break;
      }
      case(Addr_Type::IPV6):
      {
        cmd_reply->payload_size = 22 * sizeof(unsigned char);

        if((cmd_reply->payload = calloc(1, cmd_reply->payload_size)) == NULL)
        {
          free(cmd_reply);
          LOG_MSG_INFO1("failed to calloc in creating cmd reply", 0, 0, 0);
          return NULL;
        }

        byte = (unsigned char*)cmd_reply->payload;

        //version
        *byte = SOCKSV5_VERSION;
        byte++;

        //Reply
        *byte = reply;
        byte += 2; //skip over reserved field

        //Address Type
        *byte = addr_type;
        byte++;

        //IPV6 address
        if(inet_pton(AF_INET6, bind_addr, byte) <= 0)
        {
          LOG_MSG_INFO1("error converting bind IP into network order: %s", strerror(errno), 0, 0);
          free(cmd_reply);
          return NULL;
        }
        byte += 16; //increment over the IPv6 field

        //fill out port htons order
        *byte = bind_port >> 8;
        byte++;
        *byte = bind_port;
        break;
      }
      default:
      {
        LOG_MSG_INFO1("Address type: %u not supported.", addr_type, 0, 0);
        break;

      }
    }

    return cmd_reply; //after client sends, client must free
  }

  /*==========================================================================
    FUNCTION createUnamePasswdReply
  ==========================================================================*/
  /*!
  @brief
    creates username password reply socksv5 packet

  @parameters
    RFC 1929 specified command reply status

  @return
    ptr to the socksv5 packet on heap
  */
  /*========================================================================*/
  struct socksv5_packet* createUnamePasswdReply(unsigned char status)
  {
    unsigned char* byte;
    struct socksv5_packet* uname_passwd_reply;

    if((uname_passwd_reply = (struct socksv5_packet*)malloc(sizeof(struct socksv5_packet)))
        == NULL)
    {
      LOG_MSG_INFO1("failed to malloc in creating uname passwd reply", 0, 0, 0);
      return NULL;
    }

    uname_passwd_reply->payload_size = 2 * sizeof(unsigned char);

    if((uname_passwd_reply->payload = malloc(uname_passwd_reply->payload_size)) == NULL)
    {
      free(uname_passwd_reply);
      LOG_MSG_INFO1("failed to malloc in creating uname passwd reply", 0, 0, 0);
      return NULL;
    }

    byte = (unsigned char*)uname_passwd_reply->payload;

    //version
    *byte = 0x01;
    byte++;

    //Reply
    *byte = status;

    return uname_passwd_reply; //reminder after sending to free
  }

  /*==========================================================================
    FUNCTION freeSocksv5Pkt
  ==========================================================================*/
  /*!
  @brief
    frees the socksv5 packet on heap

  @parameters
    ptr to socksv5 packet to free

  @return
  */
  /*========================================================================*/
  void freeSocksv5Pkt(struct socksv5_packet* pkt_to_free)
  {
    free(pkt_to_free->payload);
    free(pkt_to_free);
    return;
  }
}
