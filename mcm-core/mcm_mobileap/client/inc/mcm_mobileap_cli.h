/*====================================================

FILE:  mcm_mobileap_cli.h

SERVICES:
mcm_mobileap_cli.c

=====================================================

  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

===========================================================================*/
#include "mcm_client.h"



typedef struct {
   /* MCM_MOBILEAP service info */
   mcm_client_handle_type           mcm_client_handle;
   uint32_t                  mcm_mobileap_handle;
   /* QCMAP CM Config */
   uint8_t                     mcm_mobileap_enable;
} mcm_mobileap_cb_t;

#define MCM_MOBILEAP_MAX_FILE_PATH_LEN 100
#define TRUE 1
#define FALSE 0

#define LOG_MSG_INFO1( fmtString, x, y, z)                            \
{                                                                     \
  printf(fmtString, x, y, z);                \
}
#define LOG_MSG_INFO2( fmtString, x, y, z)                            \
{                                                                     \
  printf(fmtString, x, y, z);                \
}
#define LOG_MSG_INFO3( fmtString, x, y, z)                            \
{                                                                     \
  printf(fmtString, x, y, z);                \
}
#define LOG_MSG_ERROR( fmtString, x, y, z)                            \
{                                                                     \
  printf(fmtString, x, y, z);                \
}


#define MCM_MOBILEAP_LOG(...)                         \
 LOG_MSG_INFO1( "%s %d:", __FILE__, __LINE__,0); \
 LOG_MSG_INFO1( __VA_ARGS__ ,0,0)

#define MCM_MOBILEAP_LOG_FUNC_ENTRY()  \
 MCM_MOBILEAP_LOG                   \
(                              \
       "Entering function %s\n",  \
       __FUNCTION__               \
)

#define MCM_MOBILEAP_LOG_FUNC_EXIT()   \
 MCM_MOBILEAP_LOG                   \
(                              \
       "Exiting function %s\n",   \
       __FUNCTION__ \
)

#define IPPROTO_TCP_UDP 253
#define IPPROTO_ICMP6 58

/*===========================================================================
MACRO IPV4_ADDR_MSG()

DESCRIPTION
  This macro prints an IPV4 address to F3.

PARAMETERS
  ip_addr: The IPV4 address in host byte order.

RETURN VALUE
  none
===========================================================================*/
#define IPV4_ADDR_MSG(ip_addr) printf( \
                        "IPV4 Address is %d.%d.%d.%d", \
                        (unsigned char)(ip_addr), \
                        (unsigned char)(ip_addr >> 8), \
                        (unsigned char)(ip_addr >> 16) , \
                        (unsigned char)(ip_addr >> 24))

/*===========================================================================
MACRO IPV6_ADDR_MSG()

DESCRIPTION
  This macro prints an IPV6 address to F3.

PARAMETERS
  ip_addr: The IPV6 address in network byte order.

RETURN VALUE
  none
===========================================================================*/
#define IPV6_ADDR_MSG(ip_addr) printf( \
                        "IPV6 Address %x:%x:%x:%x:%x:%x:%x:%x", \
                        (uint16_t)(ntohs(ip_addr[0])), \
                        (uint16_t)(ntohs(ip_addr[0] >> 16)), \
                        (uint16_t)(ntohs(ip_addr[0] >> 32)) , \
                        (uint16_t)(ntohs(ip_addr[0] >> 48)), \
                        (uint16_t)(ntohs(ip_addr[1])), \
                        (uint16_t)(ntohs(ip_addr[1] >> 16)), \
                        (uint16_t)(ntohs(ip_addr[1] >> 32)) , \
                        (uint16_t)(ntohs(ip_addr[1] >> 48)))

#define MAX_PROTO_VALUE          255
#define MAX_TOS_VALUE          255


