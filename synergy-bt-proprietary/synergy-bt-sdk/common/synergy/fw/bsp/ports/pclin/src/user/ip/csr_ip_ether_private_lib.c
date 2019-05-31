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
 *      CsrIpEtherIfAddCfm
 *----------------------------------------------------------------------------*/
CsrIpEtherIfAddCfm *CsrIpEtherIfAddCfm_struct(CsrUint32 ifHandle, CsrResult result, CsrIpEtherEncapsulation encapsulation, CsrIpEtherFrameRxFunction frameRxFunction, CsrIpEtherIpContext ipContext)
{
    CsrIpEtherIfAddCfm *prim = (CsrIpEtherIfAddCfm *) CsrPmemAlloc(sizeof(CsrIpEtherIfAddCfm));
    prim->type = CSR_IP_ETHER_IF_ADD_CFM;
    prim->ifHandle = ifHandle;
    prim->result = result;
    prim->encapsulation = encapsulation;
    prim->frameRxFunction = frameRxFunction;
    prim->ipContext = ipContext;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfRemoveCfm
 *----------------------------------------------------------------------------*/
CsrIpEtherIfRemoveCfm *CsrIpEtherIfRemoveCfm_struct(CsrUint32 ifHandle, CsrResult result)
{
    CsrIpEtherIfRemoveCfm *prim = (CsrIpEtherIfRemoveCfm *) CsrPmemAlloc(sizeof(CsrIpEtherIfRemoveCfm));
    prim->type = CSR_IP_ETHER_IF_REMOVE_CFM;
    prim->ifHandle = ifHandle;
    prim->result = result;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfLinkUpCfm
 *----------------------------------------------------------------------------*/
CsrIpEtherIfLinkUpCfm *CsrIpEtherIfLinkUpCfm_struct(CsrUint32 ifHandle, CsrResult result)
{
    CsrIpEtherIfLinkUpCfm *prim = (CsrIpEtherIfLinkUpCfm *) CsrPmemAlloc(sizeof(CsrIpEtherIfLinkUpCfm));
    prim->type = CSR_IP_ETHER_IF_LINK_UP_CFM;
    prim->ifHandle = ifHandle;
    prim->result = result;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfLinkDownCfm
 *----------------------------------------------------------------------------*/
CsrIpEtherIfLinkDownCfm *CsrIpEtherIfLinkDownCfm_struct(CsrUint32 ifHandle, CsrResult result)
{
    CsrIpEtherIfLinkDownCfm *prim = (CsrIpEtherIfLinkDownCfm *) CsrPmemAlloc(sizeof(CsrIpEtherIfLinkDownCfm));
    prim->type = CSR_IP_ETHER_IF_LINK_DOWN_CFM;
    prim->ifHandle = ifHandle;
    prim->result = result;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfMulticastAddrAddInd
 *----------------------------------------------------------------------------*/
CsrIpEtherIfMulticastAddrAddInd *CsrIpEtherIfMulticastAddrAddInd_struct(CsrUint32 ifHandle, CsrUint8 *multicastMacAddr)
{
    CsrIpEtherIfMulticastAddrAddInd *prim = (CsrIpEtherIfMulticastAddrAddInd *) CsrPmemAlloc(sizeof(CsrIpEtherIfMulticastAddrAddInd));
    prim->type = CSR_IP_ETHER_IF_MULTICAST_ADDR_ADD_IND;
    prim->ifHandle = ifHandle;
    CsrMemCpy(prim->multicastMacAddr, multicastMacAddr, 6);
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfMulticastAddrRemoveInd
 *----------------------------------------------------------------------------*/
CsrIpEtherIfMulticastAddrRemoveInd *CsrIpEtherIfMulticastAddrRemoveInd_struct(CsrUint32 ifHandle, CsrUint8 *multicastMacAddr)
{
    CsrIpEtherIfMulticastAddrRemoveInd *prim = (CsrIpEtherIfMulticastAddrRemoveInd *) CsrPmemAlloc(sizeof(CsrIpEtherIfMulticastAddrRemoveInd));
    prim->type = CSR_IP_ETHER_IF_MULTICAST_ADDR_REMOVE_IND;
    prim->ifHandle = ifHandle;
    CsrMemCpy(prim->multicastMacAddr, multicastMacAddr, 6);
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpEtherIfMulticastAddrFlushInd
 *----------------------------------------------------------------------------*/
CsrIpEtherIfMulticastAddrFlushInd *CsrIpEtherIfMulticastAddrFlushInd_struct(CsrUint32 ifHandle)
{
    CsrIpEtherIfMulticastAddrFlushInd *prim = (CsrIpEtherIfMulticastAddrFlushInd *) CsrPmemAlloc(sizeof(CsrIpEtherIfMulticastAddrFlushInd));
    prim->type = CSR_IP_ETHER_IF_MULTICAST_ADDR_FLUSH_IND;
    prim->ifHandle = ifHandle;
    return prim;
}
