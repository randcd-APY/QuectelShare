#ifndef QTI_SOCKSV5_UTIL_HPP_
#define QTI_SOCKSV5_UTIL_HPP_

/*==========================================================================

  FILE:  qti_socksv5_util.hpp

  SERVICES:

  SOCKSv5 utlity header file for handling SOCKSv5 requests

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

//MTU - IPV6_HDR - TCP_HDR_WITH_OPT = 1500 - 40 - 60 = 1400
#define MAX_MSS_IPV6_IPV4_SPLICE_SESSION 1400

#include <stdio.h>
#include <unistd.h>
#include <netinet/tcp.h>

namespace QC_SOCKSv5_Util
{
  int handleUnamePasswdAuth(unsigned char* buffer, int cli_sock,
                            const QC_SOCKSv5_Proxy_Configuration* configuration);
  int handleUnameWANMapAuth(unsigned char* buffer, int cli_sock, char** egress_wan_iface,
                            const QC_SOCKSv5_Proxy_Configuration* configuration);
  int handleConnectRequest(unsigned char* buffer, int cli_sock, const char* wan_iface,
                           const QC_SOCKSv5_Proxy_Configuration* configuration);
  void handleUnsupportedRequest(unsigned char* buffer, int cli_sock);
}

//static helper functions
static int connectToWAN(int sock_req, void* req_addr, int af_family, bool setMSS,
                        unsigned int cli_mss);
static int respondToConnectRequest(int cli_sock, int reply_errno, int wan_req_type);
static char* executeDig(const char* name_to_resolve, unsigned char wan_ip_ver,
                        const char* dns_ip_addr, unsigned char auth_method);

#endif
