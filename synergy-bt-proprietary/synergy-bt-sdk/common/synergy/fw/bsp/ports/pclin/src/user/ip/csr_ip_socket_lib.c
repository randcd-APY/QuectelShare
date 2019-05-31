/*****************************************************************************

Copyright (c) 2009-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_ip_socket_prim.h"
#include "csr_ip_socket_lib.h"

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketDnsResolveNameReq
 *----------------------------------------------------------------------------*/
CsrIpSocketDnsResolveNameReq *CsrIpSocketDnsResolveNameReq_struct(
    CsrSchedQid appHandle, CsrIpSocketFamily familyMax, CsrCharString *name)
{
    CsrIpSocketDnsResolveNameReq *prim = CsrPmemAlloc(sizeof(CsrIpSocketDnsResolveNameReq));
    prim->type = CSR_IP_SOCKET_DNS_RESOLVE_NAME_REQ;
    prim->appHandle = appHandle;
    prim->socketFamilyMax = familyMax;
    prim->name = name;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketUdpNewReq
 *----------------------------------------------------------------------------*/
CsrIpSocketUdpNewReq *CsrIpSocketUdpNewReq_struct(CsrSchedQid appHandle,
    CsrIpSocketFamily family)
{
    CsrIpSocketUdpNewReq *prim = CsrPmemAlloc(sizeof(CsrIpSocketUdpNewReq));
    prim->type = CSR_IP_SOCKET_UDP_NEW_REQ;
    prim->appHandle = appHandle;
    prim->socketFamily = family;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketUdpBindReq
 *----------------------------------------------------------------------------*/
CsrIpSocketUdpBindReq *CsrIpSocketUdpBindReq_struct(CsrUint16 socketHandle,
    const CsrUint8 ipAddress[16], CsrUint16 port)
{
    CsrIpSocketUdpBindReq *prim;

    prim = CsrPmemAlloc(sizeof(CsrIpSocketUdpBindReq));
    prim->type = CSR_IP_SOCKET_UDP_BIND_REQ;
    prim->socketHandle = socketHandle;
    if (ipAddress != NULL)
    {
        CsrMemCpy(prim->ipAddress, ipAddress, sizeof(prim->ipAddress));
    }
    else
    {
        CsrMemSet(prim->ipAddress, 0, sizeof(prim->ipAddress));
    }
    prim->port = port;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketUdpDataReq
 *----------------------------------------------------------------------------*/
CsrIpSocketUdpDataReq *CsrIpSocketUdpDataReq_struct(CsrUint16 socketHandle,
    const CsrUint8 ipAddress[16], CsrUint16 port,
    CsrUint16 dataLength, CsrUint8 *data)
{
    CsrIpSocketUdpDataReq *prim;

    prim = CsrPmemAlloc(sizeof(CsrIpSocketUdpDataReq));
    prim->type = CSR_IP_SOCKET_UDP_DATA_REQ;
    prim->socketHandle = socketHandle;
    if (ipAddress != NULL)
    {
        CsrMemCpy(prim->ipAddress, ipAddress, sizeof(prim->ipAddress));
    }
    else
    {
        CsrMemSet(prim->ipAddress, 0, sizeof(prim->ipAddress));
    }
    prim->port = port;
    prim->dataLength = dataLength;
    prim->data = data;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketUdpCloseReq
 *----------------------------------------------------------------------------*/
CsrIpSocketUdpCloseReq *CsrIpSocketUdpCloseReq_struct(CsrUint16 socketHandle)
{
    CsrIpSocketUdpCloseReq *prim = CsrPmemAlloc(sizeof(CsrIpSocketUdpCloseReq));
    prim->type = CSR_IP_SOCKET_UDP_CLOSE_REQ;
    prim->socketHandle = socketHandle;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketUdpMulticastSubscribeReq
 *
 *      Subscribe to multicast group `group'.
 *
 *----------------------------------------------------------------------------*/
CsrIpSocketUdpMulticastSubscribeReq *CsrIpSocketUdpMulticastSubscribeReq_struct(
    CsrUint16 socketHandle,
    CsrUint8 interfaceIp[16],
    CsrUint8 group[16])
{
    CsrIpSocketUdpMulticastSubscribeReq *prim =
        CsrPmemAlloc(sizeof(CsrIpSocketUdpMulticastSubscribeReq));

    prim->type = CSR_IP_SOCKET_UDP_MULTICAST_SUBSCRIBE_REQ;
    prim->socketHandle = socketHandle;
    CsrMemCpy(prim->interfaceIp, interfaceIp, sizeof(prim->interfaceIp));
    CsrMemCpy(prim->group, group, sizeof(prim->group));

    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketUdpMulticastUnsubscribeReq
 *
 *      Unsubscribe from multicast group `group'.
 *
 *----------------------------------------------------------------------------*/
CsrIpSocketUdpMulticastUnsubscribeReq *CsrIpSocketUdpMulticastUnsubscribeReq_struct(
    CsrUint16 socketHandle,
    CsrUint8 interfaceIp[16],
    CsrUint8 group[16])
{
    CsrIpSocketUdpMulticastUnsubscribeReq *prim =
        CsrPmemAlloc(sizeof(CsrIpSocketUdpMulticastUnsubscribeReq));

    prim->type = CSR_IP_SOCKET_UDP_MULTICAST_UNSUBSCRIBE_REQ;
    prim->socketHandle = socketHandle;
    CsrMemCpy(prim->interfaceIp, interfaceIp, sizeof(prim->interfaceIp));
    CsrMemCpy(prim->group, group, sizeof(prim->group));

    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketUdpMulticastInterfaceReq
 *
 *      Request multicast traffic to be sent via
 *      the interface with `interfaceIp' as the
 *      address.
 *
 *----------------------------------------------------------------------------*/
CsrIpSocketUdpMulticastInterfaceReq *CsrIpSocketUdpMulticastInterfaceReq_struct(
    CsrUint16 socketHandle,
    CsrUint8 interfaceIp[16])
{
    CsrIpSocketUdpMulticastInterfaceReq *prim =
        CsrPmemAlloc(sizeof(CsrIpSocketUdpMulticastInterfaceReq));

    prim->type = CSR_IP_SOCKET_UDP_MULTICAST_INTERFACE_REQ;
    prim->socketHandle = socketHandle;
    CsrMemCpy(prim->interfaceIp, interfaceIp, sizeof(prim->interfaceIp));

    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpNewReq
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpNewReq *CsrIpSocketTcpNewReq_struct(CsrSchedQid appHandle,
    CsrIpSocketFamily family)
{
    CsrIpSocketTcpNewReq *prim = CsrPmemAlloc(sizeof(CsrIpSocketTcpNewReq));
    prim->type = CSR_IP_SOCKET_TCP_NEW_REQ;
    prim->appHandle = appHandle;
    prim->socketFamily = family;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpBindReq
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpBindReq *CsrIpSocketTcpBindReq_struct(CsrUint16 socketHandle,
    const CsrUint8 ipAddress[16], CsrUint16 port)
{
    CsrIpSocketTcpBindReq *prim;

    prim = CsrPmemAlloc(sizeof(CsrIpSocketTcpBindReq));
    prim->type = CSR_IP_SOCKET_TCP_BIND_REQ;
    prim->socketHandle = socketHandle;
    if (ipAddress != NULL)
    {
        CsrMemCpy(prim->ipAddress, ipAddress, sizeof(prim->ipAddress));
    }
    else
    {
        CsrMemSet(prim->ipAddress, 0, sizeof(prim->ipAddress));
    }
    prim->port = port;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpListenReq
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpListenReq *CsrIpSocketTcpListenReq_struct(CsrUint16 socketHandle)
{
    CsrIpSocketTcpListenReq *prim = CsrPmemAlloc(sizeof(CsrIpSocketTcpListenReq));
    prim->type = CSR_IP_SOCKET_TCP_LISTEN_REQ;
    prim->socketHandle = socketHandle;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpConnectReq
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpConnectReq *CsrIpSocketTcpConnectReq_struct(
    CsrUint16 socketHandle, const CsrUint8 ipAddress[16], CsrUint16 port)
{
    CsrIpSocketTcpConnectReq *prim;

    prim = CsrPmemAlloc(sizeof(CsrIpSocketTcpConnectReq));
    prim->type = CSR_IP_SOCKET_TCP_CONNECT_REQ;
    prim->socketHandle = socketHandle;
    if (ipAddress != NULL)
    {
        CsrMemCpy(prim->ipAddress, ipAddress, sizeof(prim->ipAddress));
    }
    else
    {
        CsrMemSet(prim->ipAddress, 0, sizeof(prim->ipAddress));
    }
    prim->port = port;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpDataReq
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpDataReq *CsrIpSocketTcpDataReq_struct(CsrUint16 socketHandle,
    CsrUint16 dataLength, CsrUint8 *data)
{
    CsrIpSocketTcpDataReq *prim = CsrPmemAlloc(sizeof(CsrIpSocketTcpDataReq));
    prim->type = CSR_IP_SOCKET_TCP_DATA_REQ;
    prim->socketHandle = socketHandle;
    prim->dataLength = dataLength;
    prim->data = data;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpDataRes
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpDataRes *CsrIpSocketTcpDataRes_struct(CsrUint16 socketHandle)
{
    CsrIpSocketTcpDataRes *prim = CsrPmemAlloc(sizeof(CsrIpSocketTcpDataRes));
    prim->type = CSR_IP_SOCKET_TCP_DATA_RES;
    prim->socketHandle = socketHandle;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpCloseReq
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpCloseReq *CsrIpSocketTcpCloseReq_struct(CsrUint16 socketHandle)
{
    CsrIpSocketTcpCloseReq *prim = CsrPmemAlloc(sizeof(CsrIpSocketTcpCloseReq));
    prim->type = CSR_IP_SOCKET_TCP_CLOSE_REQ;
    prim->socketHandle = socketHandle;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpAbortReq
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpAbortReq *CsrIpSocketTcpAbortReq_struct(CsrUint16 socketHandle)
{
    CsrIpSocketTcpAbortReq *prim = CsrPmemAlloc(sizeof(CsrIpSocketTcpAbortReq));
    prim->type = CSR_IP_SOCKET_TCP_ABORT_REQ;
    prim->socketHandle = socketHandle;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketRawNewReq
 *----------------------------------------------------------------------------*/
CsrIpSocketRawNewReq *CsrIpSocketRawNewReq_struct(CsrSchedQid appHandle,
    CsrIpSocketFamily family, CsrUint8 protocolNumber)
{
    CsrIpSocketRawNewReq *prim = CsrPmemAlloc(sizeof(CsrIpSocketRawNewReq));
    prim->type = CSR_IP_SOCKET_RAW_NEW_REQ;
    prim->appHandle = appHandle;
    prim->socketFamily = family;
    prim->protocolNumber = protocolNumber;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketRawBindReq
 *----------------------------------------------------------------------------*/
CsrIpSocketRawBindReq *CsrIpSocketRawBindReq_struct(CsrUint16 socketHandle,
    const CsrUint8 ipAddress[16])
{
    CsrIpSocketRawBindReq *prim;

    prim = CsrPmemAlloc(sizeof(CsrIpSocketRawBindReq));
    prim->type = CSR_IP_SOCKET_RAW_BIND_REQ;
    prim->socketHandle = socketHandle;
    if (ipAddress != NULL)
    {
        CsrMemCpy(prim->ipAddress, ipAddress, sizeof(prim->ipAddress));
    }
    else
    {
        CsrMemSet(prim->ipAddress, 0, sizeof(prim->ipAddress));
    }
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketRawDataReq
 *----------------------------------------------------------------------------*/
CsrIpSocketRawDataReq *CsrIpSocketRawDataReq_struct(CsrUint16 socketHandle,
    const CsrUint8 ipAddress[16], CsrUint16 dataLength, CsrUint8 *data)
{
    CsrIpSocketRawDataReq *prim;

    prim = CsrPmemAlloc(sizeof(CsrIpSocketRawDataReq));
    prim->type = CSR_IP_SOCKET_RAW_DATA_REQ;
    prim->socketHandle = socketHandle;
    if (ipAddress != NULL)
    {
        CsrMemCpy(prim->ipAddress, ipAddress, sizeof(prim->ipAddress));
    }
    else
    {
        CsrMemSet(prim->ipAddress, 0, sizeof(prim->ipAddress));
    }
    prim->dataLength = dataLength;
    prim->data = data;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketRawCloseReq
 *----------------------------------------------------------------------------*/
CsrIpSocketRawCloseReq *CsrIpSocketRawCloseReq_struct(CsrUint16 socketHandle)
{
    CsrIpSocketRawCloseReq *prim = CsrPmemAlloc(sizeof(CsrIpSocketRawCloseReq));
    prim->type = CSR_IP_SOCKET_RAW_CLOSE_REQ;
    prim->socketHandle = socketHandle;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketOptionsReq
 *
 *      Set socket options.
 *
 *----------------------------------------------------------------------------*/
CsrIpSocketOptionsReq *CsrIpSocketOptionsReq_struct(CsrUint16 socketHandle,
    CsrUint32 txwin,
    CsrUint32 rxwin,
    CsrBool nagle,
    CsrBool keepAlive,
    CsrBool broadcast,
    CsrIpSocketPriority dscp,
    CsrIpSocketOptionMask optMask)
{
    CsrIpSocketOptionsReq *prim = CsrPmemAlloc(sizeof(CsrIpSocketOptionsReq));
    prim->type = CSR_IP_SOCKET_OPTIONS_REQ;
    prim->socketHandle = socketHandle;
    prim->txWindow = txwin;
    prim->rxWindow = rxwin;
    prim->nagle = nagle;
    prim->keepAlive = keepAlive;
    prim->broadcast = broadcast;
    prim->dscp = dscp;
    prim->validOptions = optMask;
    return prim;
}
