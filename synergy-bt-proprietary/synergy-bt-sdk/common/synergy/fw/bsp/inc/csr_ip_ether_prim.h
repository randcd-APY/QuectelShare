#ifndef CSR_IP_ETHER_PRIM_H__
#define CSR_IP_ETHER_PRIM_H__
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

/* search_string="CsrIpEtherPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrIpEtherPrim;

typedef CsrUint8 CsrIpEtherPriority;
#define CSR_IP_ETHER_PRIORITY_BE_0   ((CsrIpEtherPriority) 0x01)
#define CSR_IP_ETHER_PRIORITY_BK_1   ((CsrIpEtherPriority) 0x02)
#define CSR_IP_ETHER_PRIORITY_BK_2   ((CsrIpEtherPriority) 0x04)
#define CSR_IP_ETHER_PRIORITY_BE_3   ((CsrIpEtherPriority) 0x08)
#define CSR_IP_ETHER_PRIORITY_VI_4   ((CsrIpEtherPriority) 0x10)
#define CSR_IP_ETHER_PRIORITY_VI_5   ((CsrIpEtherPriority) 0x20)
#define CSR_IP_ETHER_PRIORITY_VO_6   ((CsrIpEtherPriority) 0x40)
#define CSR_IP_ETHER_PRIORITY_VO_7   ((CsrIpEtherPriority) 0x80)
#define CSR_IP_ETHER_PRIORITY_ALL    ((CsrIpEtherPriority) 0xFF)

/* Opaque pointer to the context of the ethernet interface, passed in the
   CSR_IP_ETHER_IF_ADD_REQ to the IP Stack, which will pass it in every call to
   the corresponding CsrIpEtherFrameTxFunction. */
typedef void *CsrIpEtherIfContext;

/* Opaque pointer to the context of the IP Stack, passed in the
   CSR_IP_ETHER_IF_ADD_CFM to the ethernet interface, which pass it in every call
   to the corresponding CsrIpEtherFrameRxFunction. */
typedef void *CsrIpEtherIpContext;

typedef void (*CsrIpEtherFrameRxFunction)(CsrUint8 *destinationMac, CsrUint8 *sourceMac, CsrInt16 rssi, CsrUint16 frameLength, CsrUint8 *frame, CsrIpEtherIpContext ipContext);
typedef CsrResult (*CsrIpEtherFrameTxFunction)(CsrUint8 *destinationMac, CsrUint8 *sourceMac, CsrUint16 frameLength, CsrUint8 *frame, CsrIpEtherIfContext ifContext);

typedef CsrUint8 CsrIpEtherEncapsulation;
#define CSR_IP_ETHER_ENCAPSULATION_ETHERNET               ((CsrIpEtherEncapsulation) 0x00)
#define CSR_IP_ETHER_ENCAPSULATION_LLC_SNAP               ((CsrIpEtherEncapsulation) 0x01)

/* Result codes. */
#define CSR_IP_ETHER_RESULT_NOT_SENT                      ((CsrResult) 1)
#define CSR_IP_ETHER_RESULT_INVALID_HANDLE                ((CsrResult) 2)
#define CSR_IP_ETHER_RESULT_NO_MORE_INTERFACES            ((CsrResult) 3)

typedef CsrUint8 CsrIpEtherIfType;
#define CSR_IP_ETHER_IF_TYPE_WIRED                        ((CsrIpEtherIfType) 0x00)
#define CSR_IP_ETHER_IF_TYPE_WIFI                         ((CsrIpEtherIfType) 0x01)

/* Downstream */
#define CSR_IP_ETHER_PRIM_DOWNSTREAM_LOWEST               (0x0000)

#define CSR_IP_ETHER_IF_ADD_REQ                           ((CsrIpEtherPrim) (0x0000 + CSR_IP_ETHER_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_ETHER_IF_REMOVE_REQ                        ((CsrIpEtherPrim) (0x0001 + CSR_IP_ETHER_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_ETHER_IF_LINK_UP_REQ                       ((CsrIpEtherPrim) (0x0002 + CSR_IP_ETHER_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_ETHER_IF_LINK_DOWN_REQ                     ((CsrIpEtherPrim) (0x0003 + CSR_IP_ETHER_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_ETHER_IF_MULTICAST_ADDR_ADD_RES            ((CsrIpEtherPrim) (0x0004 + CSR_IP_ETHER_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_ETHER_IF_MULTICAST_ADDR_REMOVE_RES         ((CsrIpEtherPrim) (0x0005 + CSR_IP_ETHER_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_ETHER_IF_MULTICAST_ADDR_FLUSH_RES          ((CsrIpEtherPrim) (0x0006 + CSR_IP_ETHER_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_ETHER_IF_FLOW_CONTROL_PAUSE_REQ            ((CsrIpEtherPrim) (0x0007 + CSR_IP_ETHER_PRIM_DOWNSTREAM_LOWEST))
#define CSR_IP_ETHER_IF_FLOW_CONTROL_RESUME_REQ           ((CsrIpEtherPrim) (0x0008 + CSR_IP_ETHER_PRIM_DOWNSTREAM_LOWEST))

#define CSR_IP_ETHER_PRIM_DOWNSTREAM_HIGHEST              (0x0008 + CSR_IP_ETHER_PRIM_DOWNSTREAM_LOWEST)

/* Upstream */
#define CSR_IP_ETHER_PRIM_UPSTREAM_LOWEST                 (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_IP_ETHER_IF_ADD_CFM                           ((CsrIpEtherPrim) (0x0000 + CSR_IP_ETHER_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_ETHER_IF_REMOVE_CFM                        ((CsrIpEtherPrim) (0x0001 + CSR_IP_ETHER_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_ETHER_IF_LINK_UP_CFM                       ((CsrIpEtherPrim) (0x0002 + CSR_IP_ETHER_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_ETHER_IF_LINK_DOWN_CFM                     ((CsrIpEtherPrim) (0x0003 + CSR_IP_ETHER_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_ETHER_IF_MULTICAST_ADDR_ADD_IND            ((CsrIpEtherPrim) (0x0004 + CSR_IP_ETHER_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_ETHER_IF_MULTICAST_ADDR_REMOVE_IND         ((CsrIpEtherPrim) (0x0005 + CSR_IP_ETHER_PRIM_UPSTREAM_LOWEST))
#define CSR_IP_ETHER_IF_MULTICAST_ADDR_FLUSH_IND          ((CsrIpEtherPrim) (0x0006 + CSR_IP_ETHER_PRIM_UPSTREAM_LOWEST))

#define CSR_IP_ETHER_PRIM_UPSTREAM_HIGHEST                (0x0006 + CSR_IP_ETHER_PRIM_UPSTREAM_LOWEST)

#define CSR_IP_ETHER_PRIM_DOWNSTREAM_COUNT                (CSR_IP_ETHER_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_IP_ETHER_PRIM_DOWNSTREAM_LOWEST)
#define CSR_IP_ETHER_PRIM_UPSTREAM_COUNT                  (CSR_IP_ETHER_PRIM_UPSTREAM_HIGHEST + 1 - CSR_IP_ETHER_PRIM_UPSTREAM_LOWEST)

typedef struct
{
    CsrIpEtherPrim            type;
    CsrSchedQid               appHandle;
    CsrIpEtherIfType          ifType;
    CsrUint8                  mac[6];
    CsrIpEtherEncapsulation   encapsulation;
    CsrUint16                 maxTxUnit;
    CsrIpEtherFrameTxFunction frameTxFunction;
    CsrIpEtherIfContext       ifContext;
} CsrIpEtherIfAddReq;

typedef struct
{
    CsrIpEtherPrim            type;
    CsrUint32                 ifHandle;
    CsrResult                 result;
    CsrIpEtherEncapsulation   encapsulation;
    CsrIpEtherFrameRxFunction frameRxFunction;
    CsrIpEtherIpContext       ipContext;
} CsrIpEtherIfAddCfm;

typedef struct
{
    CsrIpEtherPrim type;
    CsrSchedQid    appHandle;
    CsrUint32      ifHandle;
} CsrIpEtherIfRemoveReq;

typedef struct
{
    CsrIpEtherPrim type;
    CsrUint32      ifHandle;
    CsrResult      result;
} CsrIpEtherIfRemoveCfm;

typedef struct
{
    CsrIpEtherPrim type;
    CsrSchedQid    appHandle;
    CsrUint32      ifHandle;
} CsrIpEtherIfLinkUpReq;

typedef struct
{
    CsrIpEtherPrim type;
    CsrUint32      ifHandle;
    CsrResult      result;
} CsrIpEtherIfLinkUpCfm;

typedef struct
{
    CsrIpEtherPrim type;
    CsrSchedQid    appHandle;
    CsrUint32      ifHandle;
} CsrIpEtherIfLinkDownReq;

typedef struct
{
    CsrIpEtherPrim type;
    CsrUint32      ifHandle;
    CsrResult      result;
} CsrIpEtherIfLinkDownCfm;

typedef struct
{
    CsrIpEtherPrim type;
    CsrUint32      ifHandle;
    CsrUint8       multicastMacAddr[6];                      /* Multicast MAC address to be joined  */
} CsrIpEtherIfMulticastAddrAddInd;

typedef struct
{
    CsrIpEtherPrim type;
    CsrUint32      ifHandle;
    CsrResult      result;
} CsrIpEtherIfMulticastAddrAddRes;

typedef struct
{
    CsrIpEtherPrim type;
    CsrUint32      ifHandle;
    CsrUint8       multicastMacAddr[6];                      /* Multicast MAC address to be removed  */
} CsrIpEtherIfMulticastAddrRemoveInd;

typedef struct
{
    CsrIpEtherPrim type;
    CsrUint32      ifHandle;
    CsrResult      result;
} CsrIpEtherIfMulticastAddrRemoveRes;

typedef struct
{
    CsrIpEtherPrim type;
    CsrUint32      ifHandle;
} CsrIpEtherIfMulticastAddrFlushInd;

typedef struct
{
    CsrIpEtherPrim type;
    CsrUint32      ifHandle;
    CsrResult      result;
} CsrIpEtherIfMulticastAddrFlushRes;

typedef struct
{
    CsrIpEtherPrim     type;
    CsrUint32          ifHandle;
    CsrIpEtherPriority priority;
} CsrIpEtherIfFlowControlPauseReq;

typedef struct
{
    CsrIpEtherPrim     type;
    CsrUint32          ifHandle;
    CsrIpEtherPriority priority;
} CsrIpEtherIfFlowControlResumeReq;

#ifdef __cplusplus
}
#endif

#endif
