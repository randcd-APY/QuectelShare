#ifndef CSR_BT_DHCP_PRIM_H__
#define CSR_BT_DHCP_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_sched.h"
#include "csr_bt_internet_primitives.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtDhcpPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrUint16 CsrBtDhcpPrim;

#define CSR_BT_DHCP_RESULT_CONFIGURED                     0x0000
#define CSR_BT_DHCP_RESULT_ALREADY_ACTIVE                 0x0001
#define CSR_BT_DHCP_RESULT_FAILED                         0x0002
#define CSR_BT_DHCP_RESULT_PORT_UNAVAILABLE               0x0003

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_DHCP_PRIM_DOWNSTREAM_LOWEST                       (0x0000)

#define CSR_BT_DHCP_ACTIVATE_CLIENT_REQ            ((CsrBtDhcpPrim) (0x0000 + CSR_BT_DHCP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_DHCP_DEACTIVATE_CLIENT_REQ          ((CsrBtDhcpPrim) (0x0001 + CSR_BT_DHCP_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_DHCP_PRIM_DOWNSTREAM_HIGHEST                      (0x0001 + CSR_BT_DHCP_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_DHCP_PRIM_UPSTREAM_LOWEST                         (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_DHCP_ACTIVATE_CLIENT_CFM            ((CsrBtDhcpPrim) (0x0000 + CSR_BT_DHCP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_DHCP_DEACTIVATE_CLIENT_IND          ((CsrBtDhcpPrim) (0x0001 + CSR_BT_DHCP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_DHCP_DEACTIVATE_CLIENT_CFM          ((CsrBtDhcpPrim) (0x0002 + CSR_BT_DHCP_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_DHCP_PRIM_UPSTREAM_HIGHEST                        (0x0002 + CSR_BT_DHCP_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_DHCP_PRIM_DOWNSTREAM_COUNT          (CSR_BT_DHCP_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_DHCP_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_DHCP_PRIM_UPSTREAM_COUNT            (CSR_BT_DHCP_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_DHCP_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrBtDhcpPrim type;
    CsrSchedQid applicationQueue;
    CsrUint16 ethernetAddress[3]; /* in network order! */
} CsrBtDhcpActivateClientReq;

typedef struct
{
    CsrBtDhcpPrim type;
    CsrUint16 result;
    CsrUint16 ipAddress[2];
} CsrBtDhcpActivateClientCfm;

typedef struct
{
    CsrBtDhcpPrim type;
} CsrBtDhcpDeactivateClientInd;

typedef struct
{
    CsrBtDhcpPrim type;
} CsrBtDhcpDeactivateClientReq;

typedef struct
{
    CsrBtDhcpPrim type;
} CsrBtDhcpDeactivateClientCfm;

#ifdef __cplusplus
}
#endif

#endif
