#ifndef CSR_IP_SOCKET_LIB_H__
#define CSR_IP_SOCKET_LIB_H__
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/


#include "csr_synergy.h"
#include "csr_sched.h"
#include "csr_pmem.h"
#include "csr_util.h"
#include "csr_types.h"
#include "csr_msg_transport.h"
#include "csr_ip_socket_prim.h"
#include "csr_ip_socket_task.h"


#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *      Free the allocated memory in a CSR_IP_SOCKET upstream message. Does not
 *      free the message itself, and can only be used for upstream messages.
 *
 *  PARAMETERS
 *      Deallocates the resources in a CSR_IP_SOCKET upstream message
 *----------------------------------------------------------------------------*/
void CsrIpSocketFreeUpstreamMessageContents(CsrUint16 eventClass, void *message);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketFreeDownstreamMessageContents
 *
 *  DESCRIPTION
 *      Free the allocated memory in a CSR_IP_SOCKET upstream message. Does not
 *      free the message itself, and can only be used for downstream messages.
 *
 *  PARAMETERS
 *      Deallocates the resources in a CSR_IP_SOCKET upstream message
 *----------------------------------------------------------------------------*/
void CsrIpSocketFreeDownstreamMessageContents(CsrUint16 eventClass, void *message);


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketDnsResolveNameReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketDnsResolveNameReq *CsrIpSocketDnsResolveNameReq_struct(
    CsrSchedQid appHandle, CsrIpSocketFamily familyMax, CsrCharString *name);

#define CsrIpSocketDnsResolveNameReqSend(appHandle, name) \
    { \
        CsrIpSocketDnsResolveNameReq *msg__ = \
            CsrIpSocketDnsResolveNameReq_struct(appHandle, \
                CSR_IP_SOCKET_FAMILY_IP4, name); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketDnsResolveNameReqSend2(appHandle, familyMax, name) \
    { \
        CsrIpSocketDnsResolveNameReq *msg__ = \
            CsrIpSocketDnsResolveNameReq_struct(appHandle, familyMax, name); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketDnsResolveNameCfmSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketDnsResolveNameCfm *CsrIpSocketDnsResolveNameCfm_struct(
    CsrIpSocketFamily family, const CsrUint8 ipAddress[16],
    CsrCharString *name, CsrResult result);

#define CsrIpSocketDnsResolveNameCfmSend(queue, ipAddress, name, result) \
    { \
        CsrIpSocketDnsResolveNameCfm *msg__; \
        CsrUint8 ipAddress__[16]; \
        CsrMemCpy(ipAddress__, ipAddress, 4); \
        CsrMemSet(&ipAddress__[4], 0, 12); \
        msg__ = CsrIpSocketDnsResolveNameCfm_struct(CSR_IP_SOCKET_FAMILY_IP4, \
            ipAddress__, name, result); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketDnsResolveNameCfmSend2(queue, family, ipAddress, name, result) \
    { \
        CsrIpSocketDnsResolveNameCfm *msg__ = \
            CsrIpSocketDnsResolveNameCfm_struct(family, ipAddress, \
                name, result); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketUdpNewReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketUdpNewReq *CsrIpSocketUdpNewReq_struct(CsrSchedQid appHandle,
    CsrIpSocketFamily family);

#define CsrIpSocketUdpNewReqSend(appHandle) \
    { \
        CsrIpSocketUdpNewReq *msg__ = CsrIpSocketUdpNewReq_struct(appHandle, \
            CSR_IP_SOCKET_FAMILY_IP4); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketUdpNewReqSend2(appHandle, family) \
    { \
        CsrIpSocketUdpNewReq *msg__ = CsrIpSocketUdpNewReq_struct(appHandle, \
            family); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketUdpNewCfmSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketUdpNewCfm *CsrIpSocketUdpNewCfm_struct(CsrUint16 socketHandle,
    CsrResult result);

#define CsrIpSocketUdpNewCfmSend(queue, socketHandle, result) \
    { \
        CsrIpSocketUdpNewCfm *msg__ = CsrIpSocketUdpNewCfm_struct(socketHandle, result); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketUdpNewCfmSend2(queue, socketHandle, result) \
    { \
        CsrIpSocketUdpNewCfm *msg__ = CsrIpSocketUdpNewCfm_struct(socketHandle, result); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketUdpBindReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketUdpBindReq *CsrIpSocketUdpBindReq_struct(CsrUint16 socketHandle,
    const CsrUint8 ipAddress[16], CsrUint16 port);

#define CsrIpSocketUdpBindReqSend(socketHandle, ipAddress, port) \
    { \
        CsrIpSocketUdpBindReq *msg__; \
        if (ipAddress != NULL) \
        { \
            CsrUint8 ipAddress__[16]; \
            CsrUint8 *bindaddr__; /* Avoid literal NULL in memcpy() call. */ \
            bindaddr__ = ipAddress; \
            CsrMemCpy(ipAddress__, bindaddr__, 4); \
            CsrMemSet(&ipAddress__[4], 0, 12); \
            msg__ = CsrIpSocketUdpBindReq_struct(socketHandle, ipAddress__, port); \
        } \
        else \
        { \
            msg__ = CsrIpSocketUdpBindReq_struct(socketHandle, NULL, port); \
        } \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketUdpBindReqSend2(socketHandle, ipAddress, port) \
    { \
        CsrIpSocketUdpBindReq *msg__ = CsrIpSocketUdpBindReq_struct( \
            socketHandle, ipAddress, port); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketUdpBindCfmSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketUdpBindCfm *CsrIpSocketUdpBindCfm_struct(CsrUint16 socketHandle,
    CsrResult result, CsrUint16 port);

#define CsrIpSocketUdpBindCfmSend(queue, socketHandle, result, port) \
    { \
        CsrIpSocketUdpBindCfm *msg__ = CsrIpSocketUdpBindCfm_struct(socketHandle, result, port); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketUdpBindCfmSend2(queue, socketHandle, result, port) \
    { \
        CsrIpSocketUdpBindCfm *msg__ = CsrIpSocketUdpBindCfm_struct(socketHandle, result, port); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketUdpDataReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketUdpDataReq *CsrIpSocketUdpDataReq_struct(CsrUint16 socketHandle,
    const CsrUint8 ipAddress[16], CsrUint16 port,
    CsrUint16 dataLength, CsrUint8 *data);

#define CsrIpSocketUdpDataReqSend(socketHandle, ipAddress, port, dataLength, data) \
    { \
        CsrIpSocketUdpDataReq *msg__; \
        CsrUint8 ipAddress__[16]; \
        CsrMemCpy(ipAddress__, ipAddress, 4); \
        CsrMemSet(&ipAddress__[4], 0, 12); \
        msg__ = CsrIpSocketUdpDataReq_struct(socketHandle, ipAddress__, port, \
            dataLength, data); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketUdpDataReqSend2(socketHandle, ipAddress, port, dataLength, data) \
    { \
        CsrIpSocketUdpDataReq *msg__ = CsrIpSocketUdpDataReq_struct(socketHandle, ipAddress, port, dataLength, data); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketUdpDataIndSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketUdpDataInd *CsrIpSocketUdpDataInd_struct(CsrUint16 socketHandle,
    CsrIpSocketFamily family, const CsrUint8 ipAddress[16], CsrUint16 port,
    CsrUint16 dataLength, CsrUint8 *data);

#define CsrIpSocketUdpDataIndSend(queue, \
                                  socketHandle, ipAddress, port, dataLength, data) \
    { \
        CsrIpSocketUdpDataInd *msg__; \
        CsrUint8 ipAddress__[16]; \
        CsrMemCpy(ipAddress__, ipAddress, 4); \
        CsrMemSet(&ipAddress__[4], 0, 12); \
        msg__ = CsrIpSocketUdpDataInd_struct( \
            socketHandle, CSR_IP_SOCKET_FAMILY_IP4, ipAddress__, port, \
            dataLength, data); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketUdpDataIndSend2(queue, socketHandle, family, \
                                   ipAddress, port, dataLength, data) \
    { \
        CsrIpSocketUdpDataInd *msg__ = CsrIpSocketUdpDataInd_struct( \
            socketHandle, family, ipAddress, port, \
            dataLength, data); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketUdpCloseReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketUdpCloseReq *CsrIpSocketUdpCloseReq_struct(CsrUint16 socketHandle);

#define CsrIpSocketUdpCloseReqSend(socketHandle) \
    { \
        CsrIpSocketUdpCloseReq *msg__ = CsrIpSocketUdpCloseReq_struct(socketHandle); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketUdpCloseReqSend2(socketHandle) \
    { \
        CsrIpSocketUdpCloseReq *msg__ = CsrIpSocketUdpCloseReq_struct(socketHandle); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketUdpDataCfmSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketUdpDataCfm *CsrIpSocketUdpDataCfm_struct(CsrUint16 socketHandle);

#define CsrIpSocketUdpDataCfmSend(queue, socketHandle) \
    { \
        CsrIpSocketUdpDataCfm *msg__ = CsrIpSocketUdpDataCfm_struct(socketHandle); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketUdpDataCfmSend2(queue, socketHandle) \
    { \
        CsrIpSocketUdpDataCfm *msg__ = CsrIpSocketUdpDataCfm_struct(socketHandle); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketUdpMulticastSubscribe
 *
 *      Subscribe to multicast group `group'.
 *
 *----------------------------------------------------------------------------*/
CsrIpSocketUdpMulticastSubscribeReq *CsrIpSocketUdpMulticastSubscribeReq_struct(
    CsrUint16 socketHandle,
    CsrUint8 interfaceIp[16],
    CsrUint8 group[16]);

#define CsrIpSocketUdpMulticastSubscribeReqSend(socketHandle, interfaceIp, group) \
    { \
        CsrIpSocketUdpMulticastSubscribeReq *msg__; \
        CsrUint8 interfaceIp__[16], group__[16]; \
        CsrMemCpy(interfaceIp__, interfaceIp, 4); \
        CsrMemSet(&interfaceIp__[4], 0, 12); \
        CsrMemCpy(group__, group, 4); \
        CsrMemSet(&group__[4], 0, 12); \
        msg__ = CsrIpSocketUdpMulticastSubscribeReq_struct(socketHandle, \
            interfaceIp__, group__); \
        CsrSchedMessagePut(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketUdpMulticastSubscribeReqSend2(socketHandle, interfaceIp, group) \
    { \
        CsrIpSocketUdpMulticastSubscribeReq *msg__ = \
            CsrIpSocketUdpMulticastSubscribeReq_struct(socketHandle, interfaceIp, group); \
        CsrSchedMessagePut(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }


CsrIpSocketUdpMulticastSubscribeCfm *CsrIpSocketUdpMulticastSubscribeCfm_struct(
    CsrUint16 socketHandle, CsrIpSocketFamily family,
    CsrUint8 interfaceIp[16], CsrUint8 group[16], CsrResult result);

#define CsrIpSocketUdpMulticastSubscribeCfmSend(queue, socketHandle, \
                                                interfaceIp, group, result) \
    { \
        CsrIpSocketUdpMulticastSubscribeCfm *msg__; \
        CsrUint8 interfaceIp__[16], group__[16]; \
        CsrMemCpy(interfaceIp__, interfaceIp, 4); \
        CsrMemSet(&interfaceIp__[4], 0, 12); \
        CsrMemCpy(group__, group, 4); \
        CsrMemSet(&group__[4], 0, 12); \
        msg__ = CsrIpSocketUdpMulticastSubscribeCfm_struct(socketHandle, \
            CSR_IP_SOCKET_FAMILY_IP4, interfaceIp__, group__, result); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketUdpMulticastSubscribeCfmSend2(queue, socketHandle, family, \
                                                 interfaceIp, group, result) \
    { \
        CsrIpSocketUdpMulticastSubscribeCfm *msg__ = \
            CsrIpSocketUdpMulticastSubscribeCfm_struct(socketHandle, \
                family, interfaceIp, group, result); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketUdpMulticastUnsubscribe
 *
 *      Unsubscribe from multicast group `group'.
 *
 *----------------------------------------------------------------------------*/
CsrIpSocketUdpMulticastUnsubscribeReq *CsrIpSocketUdpMulticastUnsubscribeReq_struct(
    CsrUint16 socketHandle,
    CsrUint8 interfaceIp[16],
    CsrUint8 group[16]);

#define CsrIpSocketUdpMulticastUnsubscribeReqSend(socketHandle, interfaceIp, group) \
    { \
        CsrUint8 interfaceIp__[16], group__[16]; \
        CsrMemCpy(interfaceIp__, interfaceIp, 4); \
        CsrMemSet(&interfaceIp__[4], 0, 12); \
        CsrMemCpy(group__, group, 4); \
        CsrMemSet(&group__[4], 0, 12); \
        msg__ = CsrIpSocketUdpMulticastUnsubscribeReq_struct(socketHandle, \
            interfaceIp__, group__); \
        CsrSchedMessagePut(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketUdpMulticastUnsubscribeReqSend2(socketHandle, interfaceIp, group) \
    { \
        CsrIpSocketUdpMulticastUnsubscribeReq *msg__ = \
            CsrIpSocketUdpMulticastUnsubscribeReq_struct(socketHandle, interfaceIp, group); \
        CsrSchedMessagePut(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }


CsrIpSocketUdpMulticastUnsubscribeCfm *CsrIpSocketUdpMulticastUnsubscribeCfm_struct(
    CsrUint16 socketHandle, CsrIpSocketFamily family,
    CsrUint8 interfaceIp[16], CsrUint8 group[16], CsrResult result);

#define CsrIpSocketUdpMulticastUnsubscribeCfmSend(queue, socketHandle, \
                                                  interfaceIp, group, result) \
    { \
        CsrIpSocketUdpMulticastUnsubscribeCfm *msg__; \
        CsrUint8 interfaceIp__[16], group__[16]; \
        CsrMemCpy(interfaceIp__, interfaceIp, 4); \
        CsrMemSet(&interfaceIp__[4], 0, 12); \
        CsrMemCpy(group__, group, 4); \
        CsrMemSet(&group__[4], 0, 12); \
        msg__ = CsrIpSocketUdpMulticastUnsubscribeCfm_struct(socketHandle, \
            CSR_IP_SOCKET_FAMILY_IP4, interfaceIp__, group__, result); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketUdpMulticastUnsubscribeCfmSend2(queue, socketHandle, \
                                                   family, interfaceIp, group, result) \
    { \
        CsrIpSocketUdpMulticastUnsubscribeCfm *msg__ = \
            CsrIpSocketUdpMulticastUnsubscribeCfm_struct(socketHandle, \
                family, interfaceIp, group, result); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketUdpMulticastInterface
 *
 *      Request multicast traffic to be sent via
 *      the interface with `interfaceIp' as the
 *      address.
 *
 *----------------------------------------------------------------------------*/
CsrIpSocketUdpMulticastInterfaceReq *CsrIpSocketUdpMulticastInterfaceReq_struct(
    CsrUint16 socketHandle,
    CsrUint8 interfaceIp[16]);

#define CsrIpSocketUdpMulticastInterfaceReqSend(socketHandle, interfaceIp) \
    { \
        CsrIpSocketUdpMulticastInterfaceReq *msg__; \
        CsrUint8 interfaceIp__[16]; \
        CsrMemCpy(interfaceIp__, interfaceIp, 4); \
        CsrMemSet(&interfaceIp__[4], 0, 12); \
        msg__ = CsrIpSocketUdpMulticastInterfaceReq_struct(socketHandle, \
            interfaceIp__); \
        CsrSchedMessagePut(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, \
            msg__); \
    }

#define CsrIpSocketUdpMulticastInterfaceReqSend2(socketHandle, interfaceIp) \
    { \
        CsrIpSocketUdpMulticastInterfaceReq *msg__ = \
            CsrIpSocketUdpMulticastInterfaceReq_struct(socketHandle, interfaceIp); \
        CsrSchedMessagePut(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }


CsrIpSocketUdpMulticastInterfaceCfm *CsrIpSocketUdpMulticastInterfaceCfm_struct(
    CsrUint16 socketHandle,
    CsrResult result);

#define CsrIpSocketUdpMulticastInterfaceCfmSend(queue, socketHandle, result) \
    { \
        CsrIpSocketUdpMulticastInterfaceCfm *msg__ = \
            CsrIpSocketUdpMulticastInterfaceCfm_struct(socketHandle, result); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketUdpMulticastInterfaceCfmSend2(queue, socketHandle, result) \
    { \
        CsrIpSocketUdpMulticastInterfaceCfm *msg__ = \
            CsrIpSocketUdpMulticastInterfaceCfm_struct(socketHandle, result); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpNewReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpNewReq *CsrIpSocketTcpNewReq_struct(CsrSchedQid appHandle,
    CsrIpSocketFamily family);

#define CsrIpSocketTcpNewReqSend(appHandle) \
    { \
        CsrIpSocketTcpNewReq *msg__ = CsrIpSocketTcpNewReq_struct(appHandle, \
            CSR_IP_SOCKET_FAMILY_IP4); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketTcpNewReqSend2(appHandle, family) \
    { \
        CsrIpSocketTcpNewReq *msg__ = CsrIpSocketTcpNewReq_struct(appHandle, \
            family); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpNewCfmSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpNewCfm *CsrIpSocketTcpNewCfm_struct(CsrUint16 socketHandle,
    CsrResult result);

#define CsrIpSocketTcpNewCfmSend(queue, socketHandle, result) \
    { \
        CsrIpSocketTcpNewCfm *msg__ = CsrIpSocketTcpNewCfm_struct(socketHandle, result); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketTcpNewCfmSend2(queue, socketHandle, result) \
    { \
        CsrIpSocketTcpNewCfm *msg__ = CsrIpSocketTcpNewCfm_struct(socketHandle, result); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpBindReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpBindReq *CsrIpSocketTcpBindReq_struct(CsrUint16 socketHandle,
    const CsrUint8 ipAddress[16], CsrUint16 port);

#define CsrIpSocketTcpBindReqSend(socketHandle, ipAddress, port) \
    { \
        CsrIpSocketTcpBindReq *msg__; \
        if (ipAddress != NULL) \
        { \
            CsrUint8 ipAddress__[16]; \
            CsrUint8 *bindaddr__; /* Avoid literal NULL in memcpy() call. */ \
            bindaddr__ = ipAddress; \
            CsrMemCpy(ipAddress__, bindaddr__, 4); \
            CsrMemSet(&ipAddress__[4], 0, 12); \
            msg__ = CsrIpSocketTcpBindReq_struct(socketHandle, ipAddress__, port); \
        } \
        else \
        { \
            msg__ = CsrIpSocketTcpBindReq_struct(socketHandle, NULL, port); \
        } \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketTcpBindReqSend2(socketHandle, ipAddress, port) \
    { \
        CsrIpSocketTcpBindReq *msg__ = CsrIpSocketTcpBindReq_struct(socketHandle, ipAddress, port); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpBindCfmSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpBindCfm *CsrIpSocketTcpBindCfm_struct(CsrUint16 socketHandle,
    CsrResult result, CsrUint16 port);

#define CsrIpSocketTcpBindCfmSend(queue, socketHandle, result, port) \
    { \
        CsrIpSocketTcpBindCfm *msg__ = \
            CsrIpSocketTcpBindCfm_struct(socketHandle, result, port); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketTcpBindCfmSend2(queue, socketHandle, result, port) \
    { \
        CsrIpSocketTcpBindCfm *msg__ = CsrIpSocketTcpBindCfm_struct(socketHandle, result, port); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpListenReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpListenReq *CsrIpSocketTcpListenReq_struct(CsrUint16 socketHandle);

#define CsrIpSocketTcpListenReqSend(socketHandle) \
    { \
        CsrIpSocketTcpListenReq *msg__ = \
            CsrIpSocketTcpListenReq_struct(socketHandle); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketTcpListenReqSend2(socketHandle) \
    { \
        CsrIpSocketTcpListenReq *msg__ = CsrIpSocketTcpListenReq_struct(socketHandle); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpListenCfmSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpListenCfm *CsrIpSocketTcpListenCfm_struct(CsrUint16 socketHandle,
    CsrResult result);

#define CsrIpSocketTcpListenCfmSend(queue, socketHandle, result) \
    { \
        CsrIpSocketTcpListenCfm *msg__ = \
            CsrIpSocketTcpListenCfm_struct(socketHandle, result); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketTcpListenCfmSend2(queue, socketHandle, result) \
    { \
        CsrIpSocketTcpListenCfm *msg__ = CsrIpSocketTcpListenCfm_struct(socketHandle, result); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpConnectReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpConnectReq *CsrIpSocketTcpConnectReq_struct(
    CsrUint16 socketHandle, const CsrUint8 ipAddress[16], CsrUint16 port);

#define CsrIpSocketTcpConnectReqSend(socketHandle, ipAddress, port) \
    { \
        CsrIpSocketTcpConnectReq *msg__; \
        CsrUint8 ipAddress__[16]; \
        CsrMemCpy(ipAddress__, ipAddress, 4); \
        CsrMemSet(&ipAddress__[4], 0, 12); \
        msg__ = CsrIpSocketTcpConnectReq_struct(socketHandle, \
            ipAddress__, port); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketTcpConnectReqSend2(socketHandle, ipAddress, port) \
    { \
        CsrIpSocketTcpConnectReq *msg__ = CsrIpSocketTcpConnectReq_struct(socketHandle, ipAddress, port); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpConnectCfmSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpConnectCfm *CsrIpSocketTcpConnectCfm_struct(
    CsrUint16 socketHandle, CsrResult result);

#define CsrIpSocketTcpConnectCfmSend(queue, socketHandle, result) \
    { \
        CsrIpSocketTcpConnectCfm *msg__ = \
            CsrIpSocketTcpConnectCfm_struct(socketHandle, result); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketTcpConnectCfmSend2(queue, socketHandle, result) \
    { \
        CsrIpSocketTcpConnectCfm *msg__ = CsrIpSocketTcpConnectCfm_struct(socketHandle, result); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpDataReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpDataReq *CsrIpSocketTcpDataReq_struct(CsrUint16 socketHandle,
    CsrUint16 dataLength, CsrUint8 *data);

#define CsrIpSocketTcpDataReqSend(socketHandle, dataLength, data) \
    { \
        CsrIpSocketTcpDataReq *msg__ = \
            CsrIpSocketTcpDataReq_struct(socketHandle, dataLength, data); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketTcpDataReqSend2(socketHandle, dataLength, data) \
    { \
        CsrIpSocketTcpDataReq *msg__ = CsrIpSocketTcpDataReq_struct(socketHandle, dataLength, data); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpAcceptIndSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpAcceptInd *CsrIpSocketTcpAcceptInd_struct(CsrUint16 socketHandle,
    CsrIpSocketFamily family, const CsrUint8 ipAddress[16], CsrUint16 port,
    CsrUint16 listenSocketHandle);

#define CsrIpSocketTcpAcceptIndSend(queue, socketHandle, ipAddress, port, listenSocketHandle) \
    { \
        CsrIpSocketTcpAcceptInd *msg__; \
        CsrUint8 ipAddress__[16]; \
        CsrMemCpy(ipAddress__, ipAddress, 4); \
        CsrMemSet(&ipAddress__[4], 0, 12); \
        msg__ = CsrIpSocketTcpAcceptInd_struct( \
            socketHandle, CSR_IP_SOCKET_FAMILY_IP4, ipAddress__, \
            port, listenSocketHandle); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketTcpAcceptIndSend2(queue, socketHandle, family, ipAddress, port, listenSocketHandle) \
    { \
        CsrIpSocketTcpAcceptInd *msg__ = CsrIpSocketTcpAcceptInd_struct( \
            socketHandle, family, ipAddress, \
            port, listenSocketHandle); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpDataResSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpDataRes *CsrIpSocketTcpDataRes_struct(CsrUint16 socketHandle);

#define CsrIpSocketTcpDataResSend(socketHandle) \
    { \
        CsrIpSocketTcpDataRes *msg__ = CsrIpSocketTcpDataRes_struct(socketHandle); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketTcpDataResSend2(socketHandle) \
    { \
        CsrIpSocketTcpDataRes *msg__ = CsrIpSocketTcpDataRes_struct(socketHandle); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpDataCfmSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpDataCfm *CsrIpSocketTcpDataCfm_struct(CsrUint16 socketHandle,
    CsrResult result);

#define CsrIpSocketTcpDataCfmSend(queue, socketHandle, result) \
    { \
        CsrIpSocketTcpDataCfm *msg__ = CsrIpSocketTcpDataCfm_struct(socketHandle, result); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketTcpDataCfmSend2(queue, socketHandle, result) \
    { \
        CsrIpSocketTcpDataCfm *msg__ = CsrIpSocketTcpDataCfm_struct(socketHandle, result); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpCloseReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpCloseReq *CsrIpSocketTcpCloseReq_struct(CsrUint16 socketHandle);

#define CsrIpSocketTcpCloseReqSend(socketHandle) \
    { \
        CsrIpSocketTcpCloseReq *msg__ = CsrIpSocketTcpCloseReq_struct(socketHandle); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketTcpCloseReqSend2(socketHandle) \
    { \
        CsrIpSocketTcpCloseReq *msg__ = CsrIpSocketTcpCloseReq_struct(socketHandle); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpDataIndSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpDataInd *CsrIpSocketTcpDataInd_struct(CsrUint16 socketHandle,
    CsrUint16 dataLength, CsrUint8 *data);

#define CsrIpSocketTcpDataIndSend(queue, socketHandle, dataLength, data) \
    { \
        CsrIpSocketTcpDataInd *msg__ = CsrIpSocketTcpDataInd_struct(socketHandle, dataLength, data); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketTcpDataIndSend2(queue, socketHandle, dataLength, data) \
    { \
        CsrIpSocketTcpDataInd *msg__ = CsrIpSocketTcpDataInd_struct(socketHandle, dataLength, data); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpAbortReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpAbortReq *CsrIpSocketTcpAbortReq_struct(CsrUint16 socketHandle);

#define CsrIpSocketTcpAbortReqSend(socketHandle) \
    { \
        CsrIpSocketTcpAbortReq *msg__ = CsrIpSocketTcpAbortReq_struct(socketHandle); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketTcpAbortReqSend2(socketHandle) \
    { \
        CsrIpSocketTcpAbortReq *msg__ = CsrIpSocketTcpAbortReq_struct(socketHandle); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketTcpCloseIndSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketTcpCloseInd *CsrIpSocketTcpCloseInd_struct(CsrUint16 socketHandle);

#define CsrIpSocketTcpCloseIndSend(queue, socketHandle) \
    { \
        CsrIpSocketTcpCloseInd *msg__ = CsrIpSocketTcpCloseInd_struct(socketHandle); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketTcpCloseIndSend2(queue, socketHandle) \
    { \
        CsrIpSocketTcpCloseInd *msg__ = CsrIpSocketTcpCloseInd_struct(socketHandle); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketRawNewReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketRawNewReq *CsrIpSocketRawNewReq_struct(CsrSchedQid appHandle,
    CsrIpSocketFamily family, CsrUint8 protocolNumber);

#define CsrIpSocketRawNewReqSend(appHandle, protocolNumber) \
    { \
        CsrIpSocketRawNewReq *msg__ = CsrIpSocketRawNewReq_struct(appHandle, \
            CSR_IP_SOCKET_FAMILY_IP4, protocolNumber); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketRawNewReqSend2(appHandle, family, protocolNumber) \
    { \
        CsrIpSocketRawNewReq *msg__ = CsrIpSocketRawNewReq_struct(appHandle, \
            family, protocolNumber); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketRawNewCfmSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketRawNewCfm *CsrIpSocketRawNewCfm_struct(CsrUint16 socketHandle,
    CsrResult result);

#define CsrIpSocketRawNewCfmSend(queue, socketHandle, result) \
    { \
        CsrIpSocketRawNewCfm *msg__ = CsrIpSocketRawNewCfm_struct(socketHandle, result); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketRawNewCfmSend2(queue, socketHandle, result) \
    { \
        CsrIpSocketRawNewCfm *msg__ = CsrIpSocketRawNewCfm_struct(socketHandle, result); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketRawBindReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketRawBindReq *CsrIpSocketRawBindReq_struct(CsrUint16 socketHandle,
    const CsrUint8 ipAddress[16]);

#define CsrIpSocketRawBindReqSend(socketHandle, ipAddress) \
    { \
        CsrIpSocketRawBindReq *msg__; \
        if (ipAddress != NULL) \
        { \
            CsrUint8 ipAddress__[16]; \
            CsrUint8 *bindaddr__; /* Avoid literal NULL in memcpy() call. */ \
            bindaddr__ = ipAddress; \
            CsrMemCpy(ipAddress__, bindaddr__, 4); \
            CsrMemSet(&ipAddress__[4], 0, 12); \
            msg__ = CsrIpSocketRawBindReq_struct(socketHandle, ipAddress__); \
        } \
        else \
        { \
            msg__ = CsrIpSocketRawBindReq_struct(socketHandle, NULL); \
        } \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketRawBindReqSend2(socketHandle, ipAddress) \
    { \
        CsrIpSocketRawBindReq *msg__ = CsrIpSocketRawBindReq_struct(socketHandle, ipAddress); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketRawBindCfmSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketRawBindCfm *CsrIpSocketRawBindCfm_struct(CsrUint16 socketHandle,
    CsrResult result);

#define CsrIpSocketRawBindCfmSend(queue, socketHandle, result) \
    { \
        CsrIpSocketRawBindCfm *msg__ = \
            CsrIpSocketRawBindCfm_struct(socketHandle, result); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketRawBindCfmSend2(queue, socketHandle, result) \
    { \
        CsrIpSocketRawBindCfm *msg__ = CsrIpSocketRawBindCfm_struct(socketHandle, result); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketRawDataReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketRawDataReq *CsrIpSocketRawDataReq_struct(CsrUint16 socketHandle,
    const CsrUint8 ipAddress[16], CsrUint16 dataLength, CsrUint8 *data);

#define CsrIpSocketRawDataReqSend(socketHandle, ipAddress, dataLength, data) \
    { \
        CsrIpSocketRawDataReq *msg__; \
        CsrUint8 ipAddress__[16]; \
        CsrMemCpy(ipAddress__, ipAddress, 4); \
        CsrMemSet(&ipAddress__[4], 0, 12); \
        msg__ = CsrIpSocketRawDataReq_struct(socketHandle, \
            ipAddress__, dataLength, data); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketRawDataReqSend2(socketHandle, ipAddress, dataLength, data) \
    { \
        CsrIpSocketRawDataReq *msg__ = CsrIpSocketRawDataReq_struct(socketHandle, ipAddress, dataLength, data); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketRawDataIndSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketRawDataInd *CsrIpSocketRawDataInd_struct(CsrUint16 socketHandle,
    CsrIpSocketFamily family, const CsrUint8 ipAddress[16], CsrUint16 dataLength,
    CsrUint8 *data);

#define CsrIpSocketRawDataIndSend(queue, socketHandle, ipAddress, dataLength, data) \
    { \
        CsrIpSocketRawDataInd *msg__; \
        CsrUint8 ipAddress__[16]; \
        CsrMemCpy(ipAddress__, ipAddress, 4); \
        CsrMemSet(&ipAddress__[4], 0, 12); \
        msg__ = CsrIpSocketRawDataInd_struct( \
            socketHandle, CSR_IP_SOCKET_FAMILY_IP4, ipAddress__, \
            dataLength, data); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketRawDataIndSend2(queue, socketHandle, family, ipAddress, dataLength, data) \
    { \
        CsrIpSocketRawDataInd *msg__ = CsrIpSocketRawDataInd_struct( \
            socketHandle, family, ipAddress, \
            dataLength, data); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketRawCloseReqSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketRawCloseReq *CsrIpSocketRawCloseReq_struct(CsrUint16 socketHandle);

#define CsrIpSocketRawCloseReqSend(socketHandle) \
    { \
        CsrIpSocketRawCloseReq *msg__ = CsrIpSocketRawCloseReq_struct(socketHandle); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketRawCloseReqSend2(socketHandle) \
    { \
        CsrIpSocketRawCloseReq *msg__ = CsrIpSocketRawCloseReq_struct(socketHandle); \
        CsrMsgTransport(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketRawDataCfmSend
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *----------------------------------------------------------------------------*/
CsrIpSocketRawDataCfm *CsrIpSocketRawDataCfm_struct(CsrUint16 ifHandle);

#define CsrIpSocketRawDataCfmSend(queue, ifHandle) \
    { \
        CsrIpSocketRawDataCfm *msg__ = CsrIpSocketRawDataCfm_struct(ifHandle); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketRawDataCfmSend2(queue, ifHandle) \
    { \
        CsrIpSocketRawDataCfm *msg__ = CsrIpSocketRawDataCfm_struct(ifHandle); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrIpSocketOptions
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
    CsrIpSocketOptionMask optMask);

#define CsrIpSocketOptionsReqSend(socketHandle, txwin, rxwin, nagle, \
                                  keepAlive, broadcast, dscp, optMask) \
    { \
        CsrIpSocketOptionsReq *msg__ = CsrIpSocketOptionsReq_struct(socketHandle, \
            txwin, rxwin, nagle, keepAlive, broadcast, dscp, optMask); \
        CsrSchedMessagePut(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketOptionsReqSend2(socketHandle, txwin, rxwin, nagle, \
                                   keepAlive, broadcast, dscp, optMask) \
    { \
        CsrIpSocketOptionsReq *msg__ = CsrIpSocketOptionsReq_struct(socketHandle, \
            txwin, rxwin, nagle, keepAlive, broadcast, dscp, optMask); \
        CsrSchedMessagePut(CSR_IP_SOCKET_IFACEQUEUE, CSR_IP_SOCKET_PRIM, msg__); \
    }

CsrIpSocketOptionsCfm *CsrIpSocketOptionsCfm_struct(CsrUint16 socketHandle,
    CsrUint32 txwin,
    CsrUint32 rxwin,
    CsrBool nagle,
    CsrBool keepAlive,
    CsrBool broadcast,
    CsrIpSocketPriority dscp,
    CsrIpSocketOptionMask optMask);

#define CsrIpSocketOptionsCfmSend(queue, socketHandle, txwin, rxwin, nagle, \
                                  keepAlive, broadcast, dscp, optMask) \
    { \
        CsrIpSocketOptionsCfm *msg__ = CsrIpSocketOptionsCfm_struct(socketHandle, \
            txwin, rxwin, nagle, keepAlive, broadcast, dscp, optMask); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

#define CsrIpSocketOptionsCfmSend2(queue, socketHandle, txwin, rxwin, nagle, \
                                   keepAlive, broadcast, dscp, optMask) \
    { \
        CsrIpSocketOptionsCfm *msg__ = CsrIpSocketOptionsCfm_struct(socketHandle, \
            txwin, rxwin, nagle, keepAlive, broadcast, dscp, optMask); \
        CsrSchedMessagePut(queue, CSR_IP_SOCKET_PRIM, msg__); \
    }

#ifdef __cplusplus
}
#endif

#endif /* CSR_IP_SOCKET_LIB_H__ */
