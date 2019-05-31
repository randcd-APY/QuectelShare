#ifndef CSR_DHCP_SERVER_PRIM_H__
#define CSR_DHCP_SERVER_PRIM_H__
/*****************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_prim_defs.h"
#include "csr_sched.h"
#include "csr_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrDhcpServerPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrDhcpServerPrim;

#define CSR_DHCP_SERVER_PRIM_DOWNSTREAM_LOWEST                  (0x0000)

#define CSR_DHCP_SERVER_CREATE_REQ                              ((CsrDhcpServerPrim) (0x0000 + CSR_DHCP_SERVER_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DHCP_SERVER_DESTROY_REQ                             ((CsrDhcpServerPrim) (0x0001 + CSR_DHCP_SERVER_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DHCP_SERVER_START_REQ                               ((CsrDhcpServerPrim) (0x0002 + CSR_DHCP_SERVER_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DHCP_SERVER_STOP_REQ                                ((CsrDhcpServerPrim) (0x0003 + CSR_DHCP_SERVER_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DHCP_SERVER_LEASES_GET_REQ                          ((CsrDhcpServerPrim) (0x0004 + CSR_DHCP_SERVER_PRIM_DOWNSTREAM_LOWEST))
#define CSR_DHCP_SERVER_LEASES_SET_REQ                          ((CsrDhcpServerPrim) (0x0005 + CSR_DHCP_SERVER_PRIM_DOWNSTREAM_LOWEST))

#define CSR_DHCP_SERVER_PRIM_DOWNSTREAM_HIGHEST                 (0x0005 + CSR_DHCP_SERVER_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_DHCP_SERVER_PRIM_UPSTREAM_LOWEST                    (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_DHCP_SERVER_CREATE_CFM                              ((CsrDhcpServerPrim) (0x0000 + CSR_DHCP_SERVER_PRIM_UPSTREAM_LOWEST))
#define CSR_DHCP_SERVER_DESTROY_CFM                             ((CsrDhcpServerPrim) (0x0001 + CSR_DHCP_SERVER_PRIM_UPSTREAM_LOWEST))
#define CSR_DHCP_SERVER_START_CFM                               ((CsrDhcpServerPrim) (0x0002 + CSR_DHCP_SERVER_PRIM_UPSTREAM_LOWEST))
#define CSR_DHCP_SERVER_STOP_CFM                                ((CsrDhcpServerPrim) (0x0003 + CSR_DHCP_SERVER_PRIM_UPSTREAM_LOWEST))
#define CSR_DHCP_SERVER_LEASES_GET_CFM                          ((CsrDhcpServerPrim) (0x0004 + CSR_DHCP_SERVER_PRIM_UPSTREAM_LOWEST))
#define CSR_DHCP_SERVER_LEASES_SET_CFM                          ((CsrDhcpServerPrim) (0x0005 + CSR_DHCP_SERVER_PRIM_UPSTREAM_LOWEST))
#define CSR_DHCP_SERVER_ROGUE_IP_ADDRESS_IND                    ((CsrDhcpServerPrim) (0x0006 + CSR_DHCP_SERVER_PRIM_UPSTREAM_LOWEST))

#define CSR_DHCP_SERVER_PRIM_UPSTREAM_HIGHEST                   (0x0006 + CSR_DHCP_SERVER_PRIM_UPSTREAM_LOWEST)

#define CSR_DHCP_SERVER_PRIM_DOWNSTREAM_COUNT                   (CSR_DHCP_SERVER_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_DHCP_SERVER_PRIM_DOWNSTREAM_LOWEST)
#define CSR_DHCP_SERVER_PRIM_UPSTREAM_COUNT                     (CSR_DHCP_SERVER_PRIM_UPSTREAM_HIGHEST + 1 - CSR_DHCP_SERVER_PRIM_UPSTREAM_LOWEST)

typedef void *CsrDhcpServerHandle;

typedef struct
{
    CsrUint8   ip[4];
    CsrUint8   mac[6];
    CsrTimeUtc expiryTime;  /* If sec == 0xFFFFFFFF and msec == 0xFFFF the lease is infinite */
} CsrDhcpServerLease;

typedef struct
{
    CsrDhcpServerPrim type;

    CsrSchedQid qid;                     /* Control task ID */
    CsrUint8    ipAddress[4];            /* Address to listen to -- if part of network it won't be announced */

    CsrUint8 network[4];                 /* Network to announce, e.g. 192.168.0.0 */
    CsrUint8 networkMask[4];             /* Subnet Mask e.g. 255.255.255.0 */
    CsrUint8 gateway[4];
    CsrUint8 dns1[4];
    CsrUint8 dns2[4];
    CsrUint8 leaseTime;                  /* Lease lifetime in hours, 0 == infinite */
} CsrDhcpServerCreateReq;

typedef struct
{
    CsrDhcpServerPrim   type;
    CsrDhcpServerHandle serverHandle;
    CsrResult           result;
} CsrDhcpServerCreateCfm;

typedef struct
{
    CsrDhcpServerPrim   type;
    CsrDhcpServerHandle serverHandle;
} CsrDhcpServerDestroyReq;

typedef struct
{
    CsrDhcpServerPrim   type;
    CsrDhcpServerHandle serverHandle;
    CsrResult           result;
} CsrDhcpServerDestroyCfm;

/*
 * Enable/disable handing out releases
 *
 * The server starts disabled.
 */
typedef struct
{
    CsrDhcpServerPrim   type;
    CsrDhcpServerHandle serverHandle;
} CsrDhcpServerStartReq;

typedef struct
{
    CsrDhcpServerPrim   type;
    CsrDhcpServerHandle serverHandle;
    CsrResult           result;
} CsrDhcpServerStartCfm;

typedef struct
{
    CsrDhcpServerPrim   type;
    CsrDhcpServerHandle serverHandle;
} CsrDhcpServerStopReq;

typedef struct
{
    CsrDhcpServerPrim   type;
    CsrDhcpServerHandle serverHandle;
    CsrResult           result;
} CsrDhcpServerStopCfm;

/*
 * Reqeust a dump of the current leases.
 */
typedef struct
{
    CsrDhcpServerPrim   type;
    CsrDhcpServerHandle serverHandle;
} CsrDhcpServerLeasesGetReq;

typedef struct
{
    CsrDhcpServerPrim   type;
    CsrDhcpServerHandle serverHandle;
    CsrUint16           leasesCount;
    CsrDhcpServerLease *leases;
} CsrDhcpServerLeasesGetCfm;

/*
 * Set the current lease database.
 *
 * Should only be done when server is stopped to
 * ensure integrity.
 */
typedef struct
{
    CsrDhcpServerPrim   type;
    CsrDhcpServerHandle serverHandle;
    CsrUint16           leasesCount;
    CsrDhcpServerLease *leases;
} CsrDhcpServerLeasesSetReq;

typedef struct
{
    CsrDhcpServerPrim   type;
    CsrDhcpServerHandle serverHandle;
    CsrResult           result;
} CsrDhcpServerLeasesSetCfm;

typedef struct
{
    CsrDhcpServerPrim   type;
    CsrDhcpServerHandle serverHandle;
    CsrUint8            ipAddress[4];
} CsrDhcpServerRogueIpAddressInd;

#ifdef __cplusplus
}
#endif

#endif /* CSR_DHCP_SERVER_PRIM_H__ */
