#ifndef CSR_BT_ICMP_PRIM_H__
#define CSR_BT_ICMP_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_sched.h"
#include "csr_bt_profiles.h"
#include "csr_bt_ip_prim.h"
#include "csr_bt_internet_primitives.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtIcmpPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrBtIcmpPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_ICMP_PRIM_DOWNSTREAM_LOWEST                        (0x0000)

#define CSR_BT_ICMP_PING_REQ                        ((CsrBtIcmpPrim) (0x0000 + CSR_BT_ICMP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_ICMP_DESTINATION_UNREACHABLE_REQ     ((CsrBtIcmpPrim) (0x0001 + CSR_BT_ICMP_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_ICMP_PRIM_DOWNSTREAM_HIGHEST                       (0x0001 + CSR_BT_ICMP_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_ICMP_PRIM_UPSTREAM_LOWEST                          (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_ICMP_PING_CFM                        ((CsrBtIcmpPrim) (0x0000 + CSR_BT_ICMP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_ICMP_PORT_UNREACHABLE_IND            ((CsrBtIcmpPrim) (0x0001 + CSR_BT_ICMP_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_ICMP_PRIM_UPSTREAM_HIGHEST                         (0x0001 + CSR_BT_ICMP_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_ICMP_PRIM_DOWNSTREAM_COUNT           (CSR_BT_ICMP_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_ICMP_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_ICMP_PRIM_UPSTREAM_COUNT             (CSR_BT_ICMP_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_ICMP_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

#define CSR_BT_ICMP_RESULT_SUCCESS                  0x0000
#define CSR_BT_ICMP_RESULT_BUSY                     0x0001
#define CSR_BT_ICMP_RESULT_UNREACHABLE              0x0002
#define CSR_BT_ICMP_RESULT_SHUTDOWN                 0x0003

typedef struct
{
    CsrBtIcmpPrim type;
    CsrSchedQid applicationQueue;
    CsrUint16 destinationAddress[2];
    CsrUint16 dataLength;
} CsrBtIcmpPingReq;

typedef struct
{
    CsrBtIcmpPrim type;
    CsrUint16 result;
    CsrTime responseTime;
} CsrBtIcmpPingCfm;

#define CSR_BT_NET_UNREACHABLE_CODE                 0x00
#define CSR_BT_HOST_UNREACHABLE_CODE                0x01
#define CSR_BT_PROTOCOL_UNREACHABLE_CODE            0x02
#define CSR_BT_PORT_UNREACHABLE_CODE                0x03
#define CSR_BT_FRAGMENT_NEEDED_AND_DF_SET_CODE      0x04
#define CSR_BT_SOURCE_ROUTE_FAILED_CODE             0x05

typedef struct
{
    CsrBtIcmpPrim type;
    CsrUint8 code;
    CsrUint16 ipHeaderLength;
    CsrUint16 ipHeader[32];    /* 4 bit IHL -> 64 bytes */
    CsrUint16 ipDataLength;
    CsrUint16 *ipData;
} CsrBtIcmpDestinationUnreachableReq;

typedef CsrBtIpDataInd CsrBtIcmpPortUnreachableInd;

#ifdef __cplusplus
}
#endif

#endif
