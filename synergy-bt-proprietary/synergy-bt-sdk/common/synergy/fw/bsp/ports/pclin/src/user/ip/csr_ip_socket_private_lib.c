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
 *      CsrIpSocketDnsResolveNameCfm
 *----------------------------------------------------------------------------*/
CsrIpSocketDnsResolveNameCfm *CsrIpSocketDnsResolveNameCfm_struct(
    CsrIpSocketFamily family, const CsrUint8 ipAddress[16],
    CsrCharString *name, CsrResult result)
{
    CsrIpSocketDnsResolveNameCfm *prim = CsrPmemAlloc(sizeof(CsrIpSocketDnsResolveNameCfm));
    prim->type = CSR_IP_SOCKET_DNS_RESOLVE_NAME_CFM;
    prim->socketFamily = family;
    CsrMemCpy(prim->ipAddress, ipAddress, sizeof(prim->ipAddress));
    prim->name = name;
    prim->result = result;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketUdpNewCfm
 *----------------------------------------------------------------------------*/
CsrIpSocketUdpNewCfm *CsrIpSocketUdpNewCfm_struct(CsrUint16 socketHandle, CsrResult result)
{
    CsrIpSocketUdpNewCfm *prim = CsrPmemAlloc(sizeof(CsrIpSocketUdpNewCfm));
    prim->type = CSR_IP_SOCKET_UDP_NEW_CFM;
    prim->socketHandle = socketHandle;
    prim->result = result;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketUdpBindCfm
 *----------------------------------------------------------------------------*/
CsrIpSocketUdpBindCfm *CsrIpSocketUdpBindCfm_struct(CsrUint16 socketHandle, CsrResult result, CsrUint16 port)
{
    CsrIpSocketUdpBindCfm *prim = CsrPmemAlloc(sizeof(CsrIpSocketUdpBindCfm));
    prim->type = CSR_IP_SOCKET_UDP_BIND_CFM;
    prim->socketHandle = socketHandle;
    prim->result = result;
    prim->port = port;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketUdpDataInd
 *----------------------------------------------------------------------------*/
CsrIpSocketUdpDataInd *CsrIpSocketUdpDataInd_struct(CsrUint16 socketHandle,
    CsrIpSocketFamily family, const CsrUint8 ipAddress[16], CsrUint16 port,
    CsrUint16 dataLength, CsrUint8 *data)
{
    CsrIpSocketUdpDataInd *prim = CsrPmemAlloc(sizeof(CsrIpSocketUdpDataInd));
    prim->type = CSR_IP_SOCKET_UDP_DATA_IND;
    prim->socketHandle = socketHandle;
    prim->socketFamily = family;
    CsrMemCpy(prim->ipAddress, ipAddress, sizeof(prim->ipAddress));
    prim->port = port;
    prim->dataLength = dataLength;
    prim->data = data;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketUdpDataCfm
 *----------------------------------------------------------------------------*/
CsrIpSocketUdpDataCfm *CsrIpSocketUdpDataCfm_struct(CsrUint16 socketHandle)
{
    CsrIpSocketUdpDataCfm *prim = CsrPmemAlloc(sizeof(CsrIpSocketUdpDataCfm));
    prim->type = CSR_IP_SOCKET_UDP_DATA_CFM;
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
CsrIpSocketUdpMulticastSubscribeCfm *CsrIpSocketUdpMulticastSubscribeCfm_struct(
    CsrUint16 socketHandle, CsrIpSocketFamily family,
    CsrUint8 interfaceIp[16], CsrUint8 group[16], CsrResult result)
{
    CsrIpSocketUdpMulticastSubscribeCfm *prim =
        CsrPmemAlloc(sizeof(CsrIpSocketUdpMulticastSubscribeCfm));

    prim->type = CSR_IP_SOCKET_UDP_MULTICAST_SUBSCRIBE_CFM;
    prim->socketHandle = socketHandle;
    prim->socketFamily = family;
    CsrMemCpy(prim->interfaceIp, interfaceIp, sizeof(prim->interfaceIp));
    CsrMemCpy(prim->group, group, sizeof(prim->group));
    prim->result = result;

    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketUdpMulticastUnsubscribeCfm
 *
 *      Unsubscribe from multicast group `group'.
 *
 *----------------------------------------------------------------------------*/
CsrIpSocketUdpMulticastUnsubscribeCfm *CsrIpSocketUdpMulticastUnsubscribeCfm_struct(
    CsrUint16 socketHandle, CsrIpSocketFamily family,
    CsrUint8 interfaceIp[16], CsrUint8 group[16], CsrResult result)
{
    CsrIpSocketUdpMulticastUnsubscribeCfm *prim =
        CsrPmemAlloc(sizeof(CsrIpSocketUdpMulticastUnsubscribeCfm));

    prim->type = CSR_IP_SOCKET_UDP_MULTICAST_UNSUBSCRIBE_CFM;
    prim->socketHandle = socketHandle;
    prim->socketFamily = family;
    CsrMemCpy(prim->interfaceIp, interfaceIp, sizeof(prim->interfaceIp));
    CsrMemCpy(prim->group, group, sizeof(prim->group));
    prim->result = result;

    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketUdpMulticastInterfaceCfm
 *
 *      Request multicast traffic to be sent via
 *      the interface with `interfaceIp' as the
 *      address.
 *
 *----------------------------------------------------------------------------*/
CsrIpSocketUdpMulticastInterfaceCfm *CsrIpSocketUdpMulticastInterfaceCfm_struct(
    CsrUint16 socketHandle,
    CsrResult result)
{
    CsrIpSocketUdpMulticastInterfaceCfm *prim =
        CsrPmemAlloc(sizeof(CsrIpSocketUdpMulticastInterfaceCfm));

    prim->type = CSR_IP_SOCKET_UDP_MULTICAST_INTERFACE_CFM;
    prim->socketHandle = socketHandle;
    prim->result = result;

    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpNewCfm
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpNewCfm *CsrIpSocketTcpNewCfm_struct(CsrUint16 socketHandle, CsrResult result)
{
    CsrIpSocketTcpNewCfm *prim = CsrPmemAlloc(sizeof(CsrIpSocketTcpNewCfm));
    prim->type = CSR_IP_SOCKET_TCP_NEW_CFM;
    prim->socketHandle = socketHandle;
    prim->result = result;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpBindCfm
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpBindCfm *CsrIpSocketTcpBindCfm_struct(CsrUint16 socketHandle, CsrResult result, CsrUint16 port)
{
    CsrIpSocketTcpBindCfm *prim = CsrPmemAlloc(sizeof(CsrIpSocketTcpBindCfm));
    prim->type = CSR_IP_SOCKET_TCP_BIND_CFM;
    prim->socketHandle = socketHandle;
    prim->result = result;
    prim->port = port;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpListenCfm
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpListenCfm *CsrIpSocketTcpListenCfm_struct(CsrUint16 socketHandle, CsrResult result)
{
    CsrIpSocketTcpListenCfm *prim = CsrPmemAlloc(sizeof(CsrIpSocketTcpListenCfm));
    prim->type = CSR_IP_SOCKET_TCP_LISTEN_CFM;
    prim->socketHandle = socketHandle;
    prim->result = result;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpConnectCfm
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpConnectCfm *CsrIpSocketTcpConnectCfm_struct(CsrUint16 socketHandle, CsrResult result)
{
    CsrIpSocketTcpConnectCfm *prim = CsrPmemAlloc(sizeof(CsrIpSocketTcpConnectCfm));
    prim->type = CSR_IP_SOCKET_TCP_CONNECT_CFM;
    prim->socketHandle = socketHandle;
    prim->result = result;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpAcceptInd
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpAcceptInd *CsrIpSocketTcpAcceptInd_struct(CsrUint16 socketHandle,
    CsrIpSocketFamily family, const CsrUint8 ipAddress[16], CsrUint16 port,
    CsrUint16 listenSocketHandle)
{
    CsrIpSocketTcpAcceptInd *prim = CsrPmemAlloc(sizeof(CsrIpSocketTcpAcceptInd));
    prim->type = CSR_IP_SOCKET_TCP_ACCEPT_IND;
    prim->socketHandle = socketHandle;
    prim->socketFamily = family;
    CsrMemCpy(prim->ipAddress, ipAddress, sizeof(prim->ipAddress));
    prim->port = port;
    prim->listenSocketHandle = listenSocketHandle;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpDataCfm
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpDataCfm *CsrIpSocketTcpDataCfm_struct(CsrUint16 socketHandle, CsrResult result)
{
    CsrIpSocketTcpDataCfm *prim = CsrPmemAlloc(sizeof(CsrIpSocketTcpDataCfm));
    prim->type = CSR_IP_SOCKET_TCP_DATA_CFM;
    prim->socketHandle = socketHandle;
    prim->result = result;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpDataInd
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpDataInd *CsrIpSocketTcpDataInd_struct(CsrUint16 socketHandle, CsrUint16 dataLength, CsrUint8 *data)
{
    CsrIpSocketTcpDataInd *prim = CsrPmemAlloc(sizeof(CsrIpSocketTcpDataInd));
    prim->type = CSR_IP_SOCKET_TCP_DATA_IND;
    prim->socketHandle = socketHandle;
    prim->dataLength = dataLength;
    prim->data = data;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpCloseInd
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpCloseInd *CsrIpSocketTcpCloseInd_struct(CsrUint16 socketHandle)
{
    CsrIpSocketTcpCloseInd *prim = CsrPmemAlloc(sizeof(CsrIpSocketTcpCloseInd));
    prim->type = CSR_IP_SOCKET_TCP_CLOSE_IND;
    prim->socketHandle = socketHandle;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketRawNewCfm
 *----------------------------------------------------------------------------*/
CsrIpSocketRawNewCfm *CsrIpSocketRawNewCfm_struct(CsrUint16 socketHandle,
    CsrResult result)
{
    CsrIpSocketRawNewCfm *prim = CsrPmemAlloc(sizeof(CsrIpSocketRawNewCfm));
    prim->type = CSR_IP_SOCKET_RAW_NEW_CFM;
    prim->socketHandle = socketHandle;
    prim->result = result;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketRawBindCfm
 *----------------------------------------------------------------------------*/
CsrIpSocketRawBindCfm *CsrIpSocketRawBindCfm_struct(CsrUint16 socketHandle,
    CsrResult result)
{
    CsrIpSocketRawBindCfm *prim = CsrPmemAlloc(sizeof(CsrIpSocketRawBindCfm));
    prim->type = CSR_IP_SOCKET_RAW_BIND_CFM;
    prim->socketHandle = socketHandle;
    prim->result = result;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketRawDataInd
 *----------------------------------------------------------------------------*/
CsrIpSocketRawDataInd *CsrIpSocketRawDataInd_struct(CsrUint16 socketHandle,
    CsrIpSocketFamily family, const CsrUint8 ipAddress[16],
    CsrUint16 dataLength, CsrUint8 *data)
{
    CsrIpSocketRawDataInd *prim = CsrPmemAlloc(sizeof(CsrIpSocketRawDataInd));
    prim->type = CSR_IP_SOCKET_RAW_DATA_IND;
    prim->socketHandle = socketHandle;
    prim->socketFamily = family;
    CsrMemCpy(prim->ipAddress, ipAddress, sizeof(prim->ipAddress));
    prim->dataLength = dataLength;
    prim->data = data;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketRawDataCfm
 *----------------------------------------------------------------------------*/
CsrIpSocketRawDataCfm *CsrIpSocketRawDataCfm_struct(CsrUint16 socketHandle)
{
    CsrIpSocketRawDataCfm *prim = CsrPmemAlloc(sizeof(CsrIpSocketRawDataCfm));
    prim->type = CSR_IP_SOCKET_RAW_DATA_CFM;
    prim->socketHandle = socketHandle;
    return prim;
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketOptionsCfm
 *----------------------------------------------------------------------------*/
CsrIpSocketOptionsCfm *CsrIpSocketOptionsCfm_struct(CsrUint16 socketHandle,
    CsrUint32 txwin,
    CsrUint32 rxwin,
    CsrBool nagle,
    CsrBool keepAlive,
    CsrBool broadcast,
    CsrIpSocketPriority dscp,
    CsrIpSocketOptionMask optMask)
{
    CsrIpSocketOptionsCfm *prim = CsrPmemAlloc(sizeof(CsrIpSocketOptionsCfm));
    prim->type = CSR_IP_SOCKET_OPTIONS_CFM;
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
