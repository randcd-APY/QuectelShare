#ifndef CSR_IP_IFCONFIG_PRIM_H__
#define CSR_IP_IFCONFIG_PRIM_H__
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
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

/* search_string="CsrIpIfconfigPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrIpIfconfigPrim;


typedef CsrUint8 CsrIpIfconfigConfigMethod;
#define CSR_IP_IFCONFIG_CONFIG_METHOD_STATIC              ((CsrIpIfconfigConfigMethod) 0x00)
#define CSR_IP_IFCONFIG_CONFIG_METHOD_AUTOIP              ((CsrIpIfconfigConfigMethod) 0x01)
#define CSR_IP_IFCONFIG_CONFIG_METHOD_DHCP                ((CsrIpIfconfigConfigMethod) 0x02)

typedef CsrUint8 CsrIpIfconfigIfState;
#define CSR_IP_IFCONFIG_IF_STATE_UNAVAILABLE              ((CsrIpIfconfigIfState) 0x00)
#define CSR_IP_IFCONFIG_IF_STATE_MEDIA_DISCONNECTED       ((CsrIpIfconfigIfState) 0x01)
#define CSR_IP_IFCONFIG_IF_STATE_DOWN                     ((CsrIpIfconfigIfState) 0x02)
#define CSR_IP_IFCONFIG_IF_STATE_UP                       ((CsrIpIfconfigIfState) 0x03)

typedef CsrUint8 CsrIpIfconfigIfType;
#define CSR_IP_IFCONFIG_IF_TYPE_WIRED                     ((CsrIpIfconfigIfType) 0x00)
#define CSR_IP_IFCONFIG_IF_TYPE_WIFI                      ((CsrIpIfconfigIfType) 0x01)


/* IP ifconfig specific error codes. */
#define CSR_IP_IFCONFIG_RESULT_INVALID_HANDLE             ((CsrResult) 0x02)
#define CSR_IP_IFCONFIG_RESULT_NOT_SUPPORTED              ((CsrResult) 0x03)
#define CSR_IP_IFCONFIG_RESULT_UNKNOWN_NETWORK            ((CsrResult) 0x04)

typedef struct
{
    CsrUint32            ifHandle;
    CsrIpIfconfigIfType  ifType;
    CsrIpIfconfigIfState ifState;
    CsrUint8             mac[6];
    CsrUint8             ipAddress[4];
    CsrUint8             gatewayIpAddress[4];
    CsrUint8             networkMask[4];
    CsrUint8             dnsPrimaryIpAddress[4];
    CsrUint8             dnsSecondaryIpAddress[4];
} CsrIpIfconfigIfDetails;


/* Downstream */
#define CSR_IP_IFCONFIG_PRIM_DOWNSTREAM_LOWEST            (0x0000)

#define CSR_IP_IFCONFIG_SUBSCRIBE_REQ                     ((CsrIpIfconfigPrim) (0x0000 + CSR_IP_IFCONFIG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_IFCONFIG_UNSUBSCRIBE_REQ                   ((CsrIpIfconfigPrim) (0x0001 + CSR_IP_IFCONFIG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_IFCONFIG_UP_REQ                            ((CsrIpIfconfigPrim) (0x0002 + CSR_IP_IFCONFIG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_IFCONFIG_DOWN_REQ                          ((CsrIpIfconfigPrim) (0x0003 + CSR_IP_IFCONFIG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_IFCONFIG_NAT_REQ                           ((CsrIpIfconfigPrim) (0x0004 + CSR_IP_IFCONFIG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_IFCONFIG_FORWARD_REQ                       ((CsrIpIfconfigPrim) (0x0005 + CSR_IP_IFCONFIG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_IFCONFIG_ARP_ENTRY_ADD_REQ                 ((CsrIpIfconfigPrim) (0x0006 + CSR_IP_IFCONFIG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_IFCONFIG_PRIM_DOWNSTREAM_HIGHEST           (0x0006 + CSR_IP_IFCONFIG_PRIM_DOWNSTREAM_LOWEST)

/* Upstream */
#define CSR_IP_IFCONFIG_PRIM_UPSTREAM_LOWEST              (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_IP_IFCONFIG_SUBSCRIBE_CFM                     ((CsrIpIfconfigPrim) (0x0000 + CSR_IP_IFCONFIG_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_IFCONFIG_UNSUBSCRIBE_CFM                   ((CsrIpIfconfigPrim) (0x0001 + CSR_IP_IFCONFIG_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_IFCONFIG_UPDATE_IND                        ((CsrIpIfconfigPrim) (0x0002 + CSR_IP_IFCONFIG_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_IFCONFIG_NAT_CFM                           ((CsrIpIfconfigPrim) (0x0003 + CSR_IP_IFCONFIG_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_IFCONFIG_FORWARD_CFM                       ((CsrIpIfconfigPrim) (0x0004 + CSR_IP_IFCONFIG_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_IFCONFIG_UP_CFM                            ((CsrIpIfconfigPrim) (0x0005 + CSR_IP_IFCONFIG_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_IFCONFIG_DOWN_CFM                          ((CsrIpIfconfigPrim) (0x0006 + CSR_IP_IFCONFIG_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_IFCONFIG_ARP_ENTRY_ADD_CFM                 ((CsrIpIfconfigPrim) (0x0007 + CSR_IP_IFCONFIG_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_IFCONFIG_PRIM_UPSTREAM_HIGHEST             (0x0007 + CSR_IP_IFCONFIG_PRIM_UPSTREAM_LOWEST)

#define CSR_IP_IFCONFIG_PRIM_DOWNSTREAM_COUNT             (CSR_IP_IFCONFIG_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_IP_IFCONFIG_PRIM_DOWNSTREAM_LOWEST)
#define CSR_IP_IFCONFIG_PRIM_UPSTREAM_COUNT               (CSR_IP_IFCONFIG_PRIM_UPSTREAM_HIGHEST + 1 - CSR_IP_IFCONFIG_PRIM_UPSTREAM_LOWEST)

typedef struct
{
    CsrIpIfconfigPrim type;
    CsrSchedQid       appHandle;
} CsrIpIfconfigSubscribeReq;

typedef struct
{
    CsrIpIfconfigPrim       type;
    CsrUint16               ifDetailsCount;        /* Number of elements in ifDetails array  */
    CsrIpIfconfigIfDetails *ifDetails;              /* One element for each ethernet interface  */
} CsrIpIfconfigSubscribeCfm;

typedef struct
{
    CsrIpIfconfigPrim type;
    CsrSchedQid       appHandle;
} CsrIpIfconfigUnsubscribeReq;

typedef struct
{
    CsrIpIfconfigPrim type;
} CsrIpIfconfigUnsubscribeCfm;

typedef struct
{
    CsrIpIfconfigPrim         type;
    CsrSchedQid               appHandle;
    CsrUint32                 ifHandle;
    CsrIpIfconfigConfigMethod configMethod;
    CsrUint8                  ipAddress[4];
    CsrUint8                  gatewayIpAddress[4];
    CsrUint8                  networkMask[4];
    CsrUint8                  dnsPrimaryIpAddress[4];
    CsrUint8                  dnsSecondaryIpAddress[4];
} CsrIpIfconfigUpReq;

typedef struct
{
    CsrIpIfconfigPrim type;
    CsrUint32         ifHandle;
    CsrResult         result;
} CsrIpIfconfigUpCfm;

typedef struct
{
    CsrIpIfconfigPrim      type;
    CsrIpIfconfigIfDetails ifDetails;
} CsrIpIfconfigUpdateInd;

typedef struct
{
    CsrIpIfconfigPrim type;
    CsrSchedQid       appHandle;
    CsrUint32         ifHandle;
} CsrIpIfconfigDownReq;

typedef struct
{
    CsrIpIfconfigPrim type;
    CsrUint32         ifHandle;
    CsrResult         result;
} CsrIpIfconfigDownCfm;

typedef struct
{
    CsrPrim     type;
    CsrSchedQid appHandle;
    CsrUint32   ifHandle;
    CsrBool     enable;
} CsrIpIfconfigNatReq;

typedef struct
{
    CsrPrim   type;
    CsrUint32 ifHandle;
    CsrResult result;
} CsrIpIfconfigNatCfm;

typedef struct
{
    CsrPrim     type;
    CsrSchedQid appHandle;
    CsrBool     enable;
} CsrIpIfconfigForwardReq;

typedef struct
{
    CsrPrim   type;
    CsrResult result;
} CsrIpIfconfigForwardCfm;

typedef struct
{
    CsrPrim     type;
    CsrSchedQid appHandle;
    CsrUint8    mac[6];
    CsrUint8    ipAddress[4];
} CsrIpIfconfigArpEntryAddReq;

typedef struct
{
    CsrPrim   type;
    CsrResult result;
} CsrIpIfconfigArpEntryAddCfm;


#ifdef __cplusplus
}
#endif

#endif
