#ifndef CSR_BT_INTERNET_PRIMITIVES_H__
#define CSR_BT_INTERNET_PRIMITIVES_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#define CSR_BT_IP_PRIM                 0xEEE1
#define CSR_BT_ICMP_PRIM               0xEEE2
#define CSR_BT_UDP_PRIM                0xEEE3
#define CSR_BT_DHCP_PRIM               0xEEE4
#define CSR_BT_TFTP_PRIM               0xEEE5
#define CSR_BT_CTRL_PRIM               0xEEE6

#define CSR_BT_LAN_WORD(x) ((((x) & 0x00FF) << 8) | ((x) >> 8))

#ifdef __cplusplus
}
#endif

#endif

