#ifndef CSR_BT_IP_PRIM_H__
#define CSR_BT_IP_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_profiles.h"
#include "csr_bt_internet_primitives.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtIpPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrBtIpPrim;

#define CSR_BT_IP_PROTOCOL_ICMP         0x0001
#define CSR_BT_IP_PROTOCOL_TCP          0x0006
#define CSR_BT_IP_PROTOCOL_UDP          0x0011

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_BT_IP_PRIM_DOWNSTREAM_LOWEST                     (0x0000)

#define CSR_BT_IP_DATA_REQ                       ((CsrBtIpPrim) (0x0000 + CSR_BT_IP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_IP_ADDRESS_REQ                    ((CsrBtIpPrim) (0x0001 + CSR_BT_IP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_IP_MAC_ADDRESS_REQ                ((CsrBtIpPrim) (0x0002 + CSR_BT_IP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_IP_SHUTDOWN_REQ                   ((CsrBtIpPrim) (0x0003 + CSR_BT_IP_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_IP_PRIM_DOWNSTREAM_HIGHEST                    (0x0003 + CSR_BT_IP_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_BT_IP_PRIM_UPSTREAM_LOWEST                       (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_BT_IP_DATA_IND                       ((CsrBtIpPrim) (0x0000 + CSR_BT_IP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_IP_ADDRESS_IND                    ((CsrBtIpPrim) (0x0001 + CSR_BT_IP_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_IP_SHUTDOWN_IND                   ((CsrBtIpPrim) (0x0002 + CSR_BT_IP_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_IP_PRIM_UPSTREAM_HIGHEST                      (0x0002 + CSR_BT_IP_PRIM_UPSTREAM_LOWEST)

#define CSR_BT_IP_PRIM_DOWNSTREAM_COUNT          (CSR_BT_IP_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_BT_IP_PRIM_DOWNSTREAM_LOWEST)
#define CSR_BT_IP_PRIM_UPSTREAM_COUNT            (CSR_BT_IP_PRIM_UPSTREAM_HIGHEST + 1 - CSR_BT_IP_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrBtIpPrim type;
    CsrUint16 ipHeaderLength;
    CsrUint16 ipHeader[32];    /* 4 bit IHL -> 64 bytes */
    CsrUint16 ipDataLength;
    CsrUint16 * ipData;
} CsrBtIpDataInd;

typedef struct
{
    CsrBtIpPrim type;
    CsrUint16 ipAddress[2];
} CsrBtIpAddressInd;

typedef struct
{
    CsrBtIpPrim type;
    CsrUint16 ipAddress[2];
    CsrUint16 subnetMask[2];
    CsrUint16 gatewayIpAddress[2];
} CsrBtIpAddressReq;

typedef struct
{
    CsrBtIpPrim type;
    CsrUint16 macAddress[3];
} CsrBtIpMacAddressReq;

typedef struct
{
    CsrBtIpPrim type;
    CsrUint8 typeOfService;
    CsrUint8 timeToLive;
    CsrUint8 protocol;
    CsrUint16 destinationAddress[2];
    CsrUint16 sourceAddress[2];        /* will be set by IP layer if set to zero by higher layer! */
    CsrUint16 ipDataLength;
    CsrUint16 * ipData;
} CsrBtIpDataReq;

typedef struct
{
    CsrBtIpPrim type;
} CsrBtIpShutdownReq;

typedef struct
{
    CsrBtIpPrim type;
} CsrBtIpShutdownInd;

#ifdef __cplusplus
}
#endif

#endif

