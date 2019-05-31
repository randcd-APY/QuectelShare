/*****************************************************************************

Copyright (c) 2012-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_ip_ifconfig_prim.h"
#include "csr_ip_ifconfig_lib.h"


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigSubscribeCfm
 *----------------------------------------------------------------------------*/
CsrIpIfconfigSubscribeCfm *CsrIpIfconfigSubscribeCfm_struct(CsrUint16 ifDetailsCount, CsrIpIfconfigIfDetails *ifDetails)
{
    CsrIpIfconfigSubscribeCfm *prim = (CsrIpIfconfigSubscribeCfm *) CsrPmemAlloc(sizeof(CsrIpIfconfigSubscribeCfm));
    prim->type = CSR_IP_IFCONFIG_SUBSCRIBE_CFM;
    prim->ifDetailsCount = ifDetailsCount;
    prim->ifDetails = ifDetails;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigUnsubscribeCfm
 *----------------------------------------------------------------------------*/
CsrIpIfconfigUnsubscribeCfm *CsrIpIfconfigUnsubscribeCfm_struct(void)
{
    CsrIpIfconfigUnsubscribeCfm *prim = (CsrIpIfconfigUnsubscribeCfm *) CsrPmemAlloc(sizeof(CsrIpIfconfigUnsubscribeCfm));
    prim->type = CSR_IP_IFCONFIG_UNSUBSCRIBE_CFM;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigUpdateInd
 *----------------------------------------------------------------------------*/
CsrIpIfconfigUpdateInd *CsrIpIfconfigUpdateInd_struct(CsrIpIfconfigIfDetails ifDetails)
{
    CsrIpIfconfigUpdateInd *prim = (CsrIpIfconfigUpdateInd *) CsrPmemAlloc(sizeof(CsrIpIfconfigUpdateInd));
    prim->type = CSR_IP_IFCONFIG_UPDATE_IND;
    prim->ifDetails = ifDetails;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigUpCfm
 *----------------------------------------------------------------------------*/
CsrIpIfconfigUpCfm *CsrIpIfconfigUpCfm_struct(CsrUint32 ifHandle, CsrResult result)
{
    CsrIpIfconfigUpCfm *prim = (CsrIpIfconfigUpCfm *) CsrPmemAlloc(sizeof(CsrIpIfconfigUpCfm));
    prim->type = CSR_IP_IFCONFIG_UP_CFM;
    prim->ifHandle = ifHandle;
    prim->result = result;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigDownCfm
 *----------------------------------------------------------------------------*/
CsrIpIfconfigDownCfm *CsrIpIfconfigDownCfm_struct(CsrUint32 ifHandle, CsrResult result)
{
    CsrIpIfconfigDownCfm *prim = (CsrIpIfconfigDownCfm *) CsrPmemAlloc(sizeof(CsrIpIfconfigDownCfm));
    prim->type = CSR_IP_IFCONFIG_DOWN_CFM;
    prim->ifHandle = ifHandle;
    prim->result = result;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigNatCfm
 *----------------------------------------------------------------------------*/
CsrIpIfconfigNatCfm *CsrIpIfconfigNatCfm_struct(CsrUint32 ifHandle, CsrResult result)
{
    CsrIpIfconfigNatCfm *prim = (CsrIpIfconfigNatCfm *) CsrPmemAlloc(sizeof(CsrIpIfconfigNatCfm));
    prim->type = CSR_IP_IFCONFIG_NAT_CFM;
    prim->ifHandle = ifHandle;
    prim->result = result;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigDownCfm
 *----------------------------------------------------------------------------*/
CsrIpIfconfigForwardCfm *CsrIpIfconfigForwardCfm_struct(CsrResult result)
{
    CsrIpIfconfigForwardCfm *prim = (CsrIpIfconfigForwardCfm *) CsrPmemAlloc(sizeof(CsrIpIfconfigForwardCfm));
    prim->type = CSR_IP_IFCONFIG_FORWARD_CFM;
    prim->result = result;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigUnsubscribeCfm
 *----------------------------------------------------------------------------*/
CsrIpIfconfigArpEntryAddCfm *CsrIpIfconfigArpEntryAddCfm_struct(CsrResult result)
{
    CsrIpIfconfigArpEntryAddCfm *prim = (CsrIpIfconfigArpEntryAddCfm *) CsrPmemAlloc(sizeof(CsrIpIfconfigArpEntryAddCfm));
    prim->type = CSR_IP_IFCONFIG_ARP_ENTRY_ADD_CFM;
    prim->result = result;
    return prim;
}
