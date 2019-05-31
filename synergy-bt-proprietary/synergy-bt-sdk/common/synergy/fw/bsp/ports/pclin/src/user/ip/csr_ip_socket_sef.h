#ifndef CSR_IP_SOCKET_SEF_H__
#define CSR_IP_SOCKET_SEF_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2009-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_ip_handler.h"

#ifdef __cplusplus
extern "C" {
#endif

void csrIpSocketHandleSockets(CsrIpInstanceData *,
    int *, fd_set *, fd_set *);
void csrIpSocketInstFree(CsrIpInstanceData *, CsrIpSocketInst *);


void CsrIpSocketDnsResolveNameReqHandler(CsrIpInstanceData *instanceData);
void CsrIpSocketUdpNewReqHandler(CsrIpInstanceData *instanceData);
void CsrIpSocketUdpBindReqHandler(CsrIpInstanceData *instanceData);
void CsrIpSocketUdpDataReqHandler(CsrIpInstanceData *instanceData);
void CsrIpSocketUdpCloseReqHandler(CsrIpInstanceData *instanceData);
void CsrIpSocketUdpMulticastSubscribeReqHandler(CsrIpInstanceData *instanceData);
void CsrIpSocketUdpMulticastUnsubscribeReqHandler(CsrIpInstanceData *instanceData);
void CsrIpSocketUdpMulticastInterfaceReqHandler(CsrIpInstanceData *instanceData);
void CsrIpSocketTcpNewReqHandler(CsrIpInstanceData *instanceData);
void CsrIpSocketTcpBindReqHandler(CsrIpInstanceData *instanceData);
void CsrIpSocketTcpListenReqHandler(CsrIpInstanceData *instanceData);
void CsrIpSocketTcpConnectReqHandler(CsrIpInstanceData *instanceData);
void CsrIpSocketTcpDataReqHandler(CsrIpInstanceData *instanceData);
void CsrIpSocketTcpDataResHandler(CsrIpInstanceData *instanceData);
void CsrIpSocketTcpCloseReqHandler(CsrIpInstanceData *instanceData);
void CsrIpSocketTcpAbortReqHandler(CsrIpInstanceData *instanceData);
void CsrIpSocketRawNewReqHandler(CsrIpInstanceData *instanceData);
void CsrIpSocketRawBindReqHandler(CsrIpInstanceData *instanceData);
void CsrIpSocketRawDataReqHandler(CsrIpInstanceData *instanceData);
void CsrIpSocketRawCloseReqHandler(CsrIpInstanceData *instanceData);
void CsrIpSocketOptionsReqHandler(CsrIpInstanceData *instanceData);

#ifdef __cplusplus
}
#endif

#endif
