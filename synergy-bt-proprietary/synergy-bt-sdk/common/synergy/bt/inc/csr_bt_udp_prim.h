#ifndef CSR_BT_UDP_PRIM_H__
#define CSR_BT_UDP_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_prim_defs.h"
#include "csr_bt_internet_primitives.h"
#include "csr_bt_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtUdpPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrBtUdpPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_UDP_PRIM_DOWNSTREAM_LOWEST                  (0x0000)

#define CSR_BT_UDP_REGISTER_PORT_REQ          ((CsrBtUdpPrim) (0x0000 + CSR_BT_UDP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_UDP_DATA_REQ                   ((CsrBtUdpPrim) (0x0001 + CSR_BT_UDP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_UDP_DEREGISTER_PORT_REQ        ((CsrBtUdpPrim) (0x0002 + CSR_BT_UDP_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_UDP_PRIM_DOWNSTREAM_HIGHEST                 (0x0002 + CSR_BT_UDP_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_UDP_PRIM_UPSTREAM_LOWEST                    (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_UDP_REGISTER_PORT_CFM          ((CsrBtUdpPrim) (0x0000 + CSR_BT_UDP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_UDP_DATA_IND                   ((CsrBtUdpPrim) (0x0001 + CSR_BT_UDP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_UDP_PORT_UNREACHABLE_IND       ((CsrBtUdpPrim) (0x0002 + CSR_BT_UDP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_UDP_SHUTDOWN_IND               ((CsrBtUdpPrim) (0x0003 + CSR_BT_UDP_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_UDP_PRIM_UPSTREAM_HIGHEST                   (0x0003 + CSR_BT_UDP_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_UDP_PRIM_DOWNSTREAM_COUNT      (CSR_BT_UDP_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_UDP_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_UDP_PRIM_UPSTREAM_COUNT        (CSR_BT_UDP_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_UDP_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

#define CSR_BT_UDP_SUCCESS                    0x00
#define CSR_BT_UDP_PORT_ALREADY_IN_USE        0x01

typedef struct
{
    CsrBtUdpPrim type;
    CsrUint16 port;                     /* in network order! If zero, chosen by UDP (next free) from the dynamic range (C000h-FFFFh)! */
    CsrSchedQid destinationQueue;
} CsrBtUdpRegisterPortReq;

typedef struct
{
    CsrBtUdpPrim type;
    CsrUint16 port;                     /* in network order! */
    CsrUint8 result;
} CsrBtUdpRegisterPortCfm;

typedef struct
{
    CsrBtUdpPrim type;
    CsrUint16 port;                     /* in network order! */
} CsrBtUdpDeregisterPortReq;

typedef struct
{
    CsrBtUdpPrim type;
    CsrUint8 typeOfService;
    CsrUint8 timeToLive;
    CsrUint16 destinationAddress[2];    /* destination IP address */
    CsrUint16 udpDataLength;            /* including the 8 octets udp header */
    CsrUint16 * udpData;                /* length and check-sum fields are filled out by udp */
} CsrBtUdpDataReq;

typedef struct
{
    CsrBtUdpPrim type;
    CsrUint16 sourceAddress[2];         /* source IP address */
    CsrUint16 udpDataLength;            /* including the 8 octets udp header */
    CsrUint16 * udpData;                /* check-sum is checked by udp */
} CsrBtUdpDataInd;

typedef struct
{
    CsrBtUdpPrim type;
    CsrUint16 localPort;                /* in network order! */
    CsrUint16 remotePort;               /* in network order! */
} CsrBtUdpPortUnreachableInd;

typedef struct
{
    CsrBtUdpPrim type;
} CsrBtUdpShutdownInd;

#ifdef __cplusplus
}
#endif

#endif

