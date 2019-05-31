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
 *      CsrIpIfconfigSubscribeReq
 *----------------------------------------------------------------------------*/
CsrIpIfconfigSubscribeReq *CsrIpIfconfigSubscribeReq_struct(CsrSchedQid appHandle)
{
    CsrIpIfconfigSubscribeReq *prim = (CsrIpIfconfigSubscribeReq *) CsrPmemAlloc(sizeof(CsrIpIfconfigSubscribeReq));
    prim->type = CSR_IP_IFCONFIG_SUBSCRIBE_REQ;
    prim->appHandle = appHandle;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigUnsubscribeReq
 *----------------------------------------------------------------------------*/
CsrIpIfconfigUnsubscribeReq *CsrIpIfconfigUnsubscribeReq_struct(CsrSchedQid appHandle)
{
    CsrIpIfconfigUnsubscribeReq *prim = (CsrIpIfconfigUnsubscribeReq *) CsrPmemAlloc(sizeof(CsrIpIfconfigUnsubscribeReq));
    prim->type = CSR_IP_IFCONFIG_UNSUBSCRIBE_REQ;
    prim->appHandle = appHandle;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigUpReq
 *----------------------------------------------------------------------------*/
CsrIpIfconfigUpReq *CsrIpIfconfigUpReq_struct(CsrSchedQid appHandle, CsrUint32 ifHandle, CsrIpIfconfigConfigMethod configMethod, CsrUint8 *ipAddress, CsrUint8 *gatewayIpAddress, CsrUint8 *networkMask, CsrUint8 *dnsPrimaryIpAddress, CsrUint8 *dnsSecondaryIpAddress)
{
    CsrIpIfconfigUpReq *prim;

    prim = (CsrIpIfconfigUpReq *) CsrPmemZalloc(sizeof(CsrIpIfconfigUpReq));
    prim->type = CSR_IP_IFCONFIG_UP_REQ;
    prim->appHandle = appHandle;
    prim->ifHandle = ifHandle;
    prim->configMethod = configMethod;

    if (ipAddress != NULL)
    {
        prim->ipAddress[0] = ipAddress[0];
        prim->ipAddress[1] = ipAddress[1];
        prim->ipAddress[2] = ipAddress[2];
        prim->ipAddress[3] = ipAddress[3];
    }

    if (gatewayIpAddress != NULL)
    {
        prim->gatewayIpAddress[0] = gatewayIpAddress[0];
        prim->gatewayIpAddress[1] = gatewayIpAddress[1];
        prim->gatewayIpAddress[2] = gatewayIpAddress[2];
        prim->gatewayIpAddress[3] = gatewayIpAddress[3];
    }

    if (networkMask != NULL)
    {
        prim->networkMask[0] = networkMask[0];
        prim->networkMask[1] = networkMask[1];
        prim->networkMask[2] = networkMask[2];
        prim->networkMask[3] = networkMask[3];
    }

    if (dnsPrimaryIpAddress != NULL)
    {
        prim->dnsPrimaryIpAddress[0] = dnsPrimaryIpAddress[0];
        prim->dnsPrimaryIpAddress[1] = dnsPrimaryIpAddress[1];
        prim->dnsPrimaryIpAddress[2] = dnsPrimaryIpAddress[2];
        prim->dnsPrimaryIpAddress[3] = dnsPrimaryIpAddress[3];
    }

    if (dnsSecondaryIpAddress != NULL)
    {
        prim->dnsSecondaryIpAddress[0] = dnsSecondaryIpAddress[0];
        prim->dnsSecondaryIpAddress[1] = dnsSecondaryIpAddress[1];
        prim->dnsSecondaryIpAddress[2] = dnsSecondaryIpAddress[2];
        prim->dnsSecondaryIpAddress[3] = dnsSecondaryIpAddress[3];
    }

    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigDownReq
 *----------------------------------------------------------------------------*/
CsrIpIfconfigDownReq *CsrIpIfconfigDownReq_struct(CsrSchedQid appHandle, CsrUint32 ifHandle)
{
    CsrIpIfconfigDownReq *prim = (CsrIpIfconfigDownReq *) CsrPmemAlloc(sizeof(CsrIpIfconfigDownReq));
    prim->type = CSR_IP_IFCONFIG_DOWN_REQ;
    prim->appHandle = appHandle;
    prim->ifHandle = ifHandle;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigNatReq
 *----------------------------------------------------------------------------*/
CsrIpIfconfigNatReq *CsrIpIfconfigNatReq_struct(CsrSchedQid appHandle, CsrUint32 ifHandle, CsrBool enable)
{
    CsrIpIfconfigNatReq *prim = (CsrIpIfconfigNatReq *) CsrPmemAlloc(sizeof(CsrIpIfconfigNatReq));
    prim->type = CSR_IP_IFCONFIG_NAT_REQ;
    prim->appHandle = appHandle;
    prim->ifHandle = ifHandle;
    prim->enable = enable;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigForwardReq
 *----------------------------------------------------------------------------*/
CsrIpIfconfigForwardReq *CsrIpIfconfigForwardReq_struct(CsrSchedQid appHandle, CsrBool enable)
{
    CsrIpIfconfigForwardReq *prim = (CsrIpIfconfigForwardReq *) CsrPmemAlloc(sizeof(CsrIpIfconfigForwardReq));
    prim->type = CSR_IP_IFCONFIG_FORWARD_REQ;
    prim->appHandle = appHandle;
    prim->enable = enable;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpIfconfigArpEntryAddReq
 *----------------------------------------------------------------------------*/
CsrIpIfconfigArpEntryAddReq *CsrIpIfconfigArpEntryAddReq_struct(CsrSchedQid appHandle, CsrUint8 mac[6], CsrUint8 ipAddress[4])
{
    CsrIpIfconfigArpEntryAddReq *prim = (CsrIpIfconfigArpEntryAddReq *) CsrPmemAlloc(sizeof(CsrIpIfconfigArpEntryAddReq));
    prim->type = CSR_IP_IFCONFIG_ARP_ENTRY_ADD_REQ;
    prim->appHandle = appHandle;
    prim->mac[0] = mac[0];
    prim->mac[1] = mac[1];
    prim->mac[2] = mac[2];
    prim->mac[3] = mac[3];
    prim->mac[4] = mac[4];
    prim->mac[5] = mac[5];
    prim->ipAddress[0] = ipAddress[0];
    prim->ipAddress[1] = ipAddress[1];
    prim->ipAddress[2] = ipAddress[2];
    prim->ipAddress[3] = ipAddress[3];

    return prim;
}
