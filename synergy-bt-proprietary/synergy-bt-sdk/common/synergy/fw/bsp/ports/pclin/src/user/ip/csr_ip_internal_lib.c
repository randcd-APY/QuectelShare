/*****************************************************************************

Copyright (c) 2012-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_sched.h"
#include "csr_util.h"

#include "csr_ip_ifconfig_prim.h"
#include "csr_ip_internal_prim.h"
#include "csr_ip_internal_lib.h"
#include "csr_ip_handler.h"
#include "csr_ip_task.h"

#include "lwip/netif.h"
#include "lwip/pbuf.h"

void CsrIpInternalEtherPacketReqSend(CsrUint16 ethType, struct pbuf *buffer, struct netif **netif)
{
    CsrIpInternalEtherPacketReq *request = CsrPmemAlloc(sizeof(CsrIpInternalEtherPacketReq));
    request->type = CSR_IP_INTERNAL_ETHER_PACKET_REQ;
    request->ethType = ethType;
    request->buffer = buffer;
    request->netif = netif;
    CsrSchedMessagePut(CSR_IP_IFACEQUEUE, CSR_IP_INTERNAL_PRIM, request);
}
