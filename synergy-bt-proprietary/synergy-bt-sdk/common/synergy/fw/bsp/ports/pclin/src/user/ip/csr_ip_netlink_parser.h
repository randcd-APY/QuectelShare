#ifndef CSR_IP_NETLINK_PARSER_H__
#define CSR_IP_NETLINK_PARSER_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2010-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include <sys/socket.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if_packet.h>

#include "csr_types.h"
#include "csr_sched.h"

#include "csr_ip_ifconfig_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

CsrUint32 csrIfAddrMsgIndexGet(struct nlmsghdr *nh);
CsrUint32 csrIfInfoMsgIndexGet(struct nlmsghdr *nh);
void csrIpIfconfigDevLookup(int fd,
    CsrUint32 ifIdx,
    CsrCharString **ifName,
    CsrIpIfconfigIfDetails *ifd);

#ifdef __cplusplus
}
#endif

#endif
