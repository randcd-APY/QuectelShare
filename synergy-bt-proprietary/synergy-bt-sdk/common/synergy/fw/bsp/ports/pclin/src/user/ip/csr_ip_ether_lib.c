/*****************************************************************************

Copyright (c) 2012-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_sched.h"
#include "csr_ip_ether_prim.h"
#include "csr_ip_ether_lib.h"

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfAddReq
 *----------------------------------------------------------------------------*/
CsrIpEtherIfAddReq *CsrIpEtherIfAddReq_struct(CsrSchedQid appHandle, CsrIpEtherIfType ifType, CsrUint8 *mac, CsrUint8 encapsulation, CsrUint16 maxTxUnit, CsrIpEtherFrameTxFunction frameTxFunction, CsrIpEtherIfContext ifContext)
{
    CsrIpEtherIfAddReq *prim;

    prim = (CsrIpEtherIfAddReq *) CsrPmemZalloc(sizeof(CsrIpEtherIfAddReq));
    prim->type = CSR_IP_ETHER_IF_ADD_REQ;
    prim->appHandle = appHandle;
    prim->ifType = ifType;
    if (mac != NULL)
    {
        prim->mac[0] = mac[0];
        prim->mac[1] = mac[1];
        prim->mac[2] = mac[2];
        prim->mac[3] = mac[3];
        prim->mac[4] = mac[4];
        prim->mac[5] = mac[5];
    }
    prim->encapsulation = encapsulation;
    prim->maxTxUnit = maxTxUnit;
    prim->frameTxFunction = frameTxFunction;
    prim->ifContext = ifContext;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfRemoveReq
 *----------------------------------------------------------------------------*/
CsrIpEtherIfRemoveReq *CsrIpEtherIfRemoveReq_struct(CsrSchedQid appHandle, CsrUint32 ifHandle)
{
    CsrIpEtherIfRemoveReq *prim = (CsrIpEtherIfRemoveReq *) CsrPmemAlloc(sizeof(CsrIpEtherIfRemoveReq));
    prim->type = CSR_IP_ETHER_IF_REMOVE_REQ;
    prim->appHandle = appHandle;
    prim->ifHandle = ifHandle;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfLinkUpReq
 *----------------------------------------------------------------------------*/
CsrIpEtherIfLinkUpReq *CsrIpEtherIfLinkUpReq_struct(CsrSchedQid appHandle, CsrUint32 ifHandle)
{
    CsrIpEtherIfLinkUpReq *prim = (CsrIpEtherIfLinkUpReq *) CsrPmemAlloc(sizeof(CsrIpEtherIfLinkUpReq));
    prim->type = CSR_IP_ETHER_IF_LINK_UP_REQ;
    prim->appHandle = appHandle;
    prim->ifHandle = ifHandle;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfLinkDownReq
 *----------------------------------------------------------------------------*/
CsrIpEtherIfLinkDownReq *CsrIpEtherIfLinkDownReq_struct(CsrSchedQid appHandle, CsrUint32 ifHandle)
{
    CsrIpEtherIfLinkDownReq *prim = (CsrIpEtherIfLinkDownReq *) CsrPmemAlloc(sizeof(CsrIpEtherIfLinkDownReq));
    prim->type = CSR_IP_ETHER_IF_LINK_DOWN_REQ;
    prim->appHandle = appHandle;
    prim->ifHandle = ifHandle;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfMulticastAddrAddRes
 *----------------------------------------------------------------------------*/
CsrIpEtherIfMulticastAddrAddRes *CsrIpEtherIfMulticastAddrAddRes_struct(CsrUint32 ifHandle, CsrResult result)
{
    CsrIpEtherIfMulticastAddrAddRes *prim = (CsrIpEtherIfMulticastAddrAddRes *) CsrPmemAlloc(sizeof(CsrIpEtherIfMulticastAddrAddRes));
    prim->type = CSR_IP_ETHER_IF_MULTICAST_ADDR_ADD_RES;
    prim->ifHandle = ifHandle;
    prim->result = result;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfMulticastAddrRemoveRes
 *----------------------------------------------------------------------------*/
CsrIpEtherIfMulticastAddrRemoveRes *CsrIpEtherIfMulticastAddrRemoveRes_struct(CsrUint32 ifHandle, CsrResult result)
{
    CsrIpEtherIfMulticastAddrRemoveRes *prim = (CsrIpEtherIfMulticastAddrRemoveRes *) CsrPmemAlloc(sizeof(CsrIpEtherIfMulticastAddrRemoveRes));
    prim->type = CSR_IP_ETHER_IF_MULTICAST_ADDR_REMOVE_RES;
    prim->ifHandle = ifHandle;
    prim->result = result;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfMulticastAddrFlushRes
 *----------------------------------------------------------------------------*/
CsrIpEtherIfMulticastAddrFlushRes *CsrIpEtherIfMulticastAddrFlushRes_struct(CsrUint32 ifHandle, CsrResult result)
{
    CsrIpEtherIfMulticastAddrFlushRes *prim = (CsrIpEtherIfMulticastAddrFlushRes *) CsrPmemAlloc(sizeof(CsrIpEtherIfMulticastAddrFlushRes));
    prim->type = CSR_IP_ETHER_IF_MULTICAST_ADDR_FLUSH_RES;
    prim->ifHandle = ifHandle;
    prim->result = result;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfFlowControlPauseReq
 *----------------------------------------------------------------------------*/
CsrIpEtherIfFlowControlPauseReq *CsrIpEtherIfFlowControlPauseReq_struct(CsrUint32 ifHandle, CsrIpEtherPriority priority)
{
    CsrIpEtherIfFlowControlPauseReq *prim = (CsrIpEtherIfFlowControlPauseReq *) CsrPmemAlloc(sizeof(CsrIpEtherIfFlowControlPauseReq));
    prim->type = CSR_IP_ETHER_IF_FLOW_CONTROL_PAUSE_REQ;
    prim->ifHandle = ifHandle;
    prim->priority = priority;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfFlowControlResumeReq
 *----------------------------------------------------------------------------*/
CsrIpEtherIfFlowControlResumeReq *CsrIpEtherIfFlowControlResumeReq_struct(CsrUint32 ifHandle, CsrIpEtherPriority priority)
{
    CsrIpEtherIfFlowControlResumeReq *prim = (CsrIpEtherIfFlowControlResumeReq *) CsrPmemAlloc(sizeof(CsrIpEtherIfFlowControlResumeReq));
    prim->type = CSR_IP_ETHER_IF_FLOW_CONTROL_RESUME_REQ;
    prim->ifHandle = ifHandle;
    prim->priority = priority;
    return prim;
}
