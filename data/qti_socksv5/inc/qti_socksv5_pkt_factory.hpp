#ifndef QTI_SOCKSV5_PKT_FACTORY_HPP_
#define QTI_SOCKSV5_PKT_FACTORY_HPP_

/*==========================================================================

  FILE:  qti_socksv5_pkt_factory.hpp

  SERVICES:

  SOCKSv5 packet factory mallocs SOCKSv5 headers on to heap, must call free
  SOCKSv5 packet afterwards.

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

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

/* SOCKS version */
#define SOCKSV5_VERSION 0x05


/* SOCKS command request values */
enum Cmd_Request
{
  CONNECT       = 0x01,
  BIND          = 0x02,
  UDP_ASSOCIATE = 0x03,
};

/* Server reply codes */
enum Server_Reply
{
  SUCCEEDED              = 0x00,
  SERVER_FAILURE         = 0x01,
  CONNECTION_NOT_ALLOWED = 0x02,
  NETWORK_UNREACHABLE    = 0x03,
  HOST_UNREACHABLE       = 0x04,
  CONNECTION_REFUSED     = 0x05,
  TTL_EXPIRED            = 0x06,
  COMMAND_NOT_SUPPORTED  = 0x07,
  ADDR_TYPE_UNSUPPORTED  = 0x08,
};

/* Address type codes */
enum Addr_Type
{
  IPV4        = 0x01,
  DOMAIN_NAME = 0x03,
  IPV6        = 0x04,
};

struct socksv5_packet
{
  unsigned int payload_size;
  void* payload;

};

namespace QC_SOCKSv5_Pkt_Factory
{
  struct socksv5_packet* createMethodSelectionReply(unsigned char method);
  struct socksv5_packet* createUnamePasswdReply(unsigned char status);
  struct socksv5_packet* createCmdReply(unsigned char reply, unsigned char addr_type,
                                        const char* bind_addr, unsigned short bind_port);
  void freeSocksv5Pkt(struct socksv5_packet* pkt_to_free);
}

#endif
