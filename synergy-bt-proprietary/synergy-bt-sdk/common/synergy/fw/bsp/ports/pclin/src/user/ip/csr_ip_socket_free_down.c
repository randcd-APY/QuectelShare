/*****************************************************************************

Copyright (c) 2011-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

/* Note: this is an auto-generated file. */

#ifndef EXCLUDE_CSR_IP_SOCKET_MODULE
#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_mblk.h"
#include "csr_autogen.h"
#include "csr_ip_socket_lib.h"
#include "csr_ip_socket_prim.h"

void CsrIpSocketFreeDownstreamMessageContents(CsrUint16 eventClass, void *message)
{
    if (eventClass == CSR_IP_SOCKET_PRIM)
    {
        CsrIpSocketPrim *prim = (CsrIpSocketPrim *) message;
        switch (*prim)
        {
#ifndef EXCLUDE_CSR_IP_SOCKET_UDP_DATA_REQ
            case CSR_IP_SOCKET_UDP_DATA_REQ:
            {
                CsrIpSocketUdpDataReq *p = message;
                CsrPmemFree(p->data);
                p->data = NULL;
                break;
            }
#endif /* EXCLUDE_CSR_IP_SOCKET_UDP_DATA_REQ */
#ifndef EXCLUDE_CSR_IP_SOCKET_DNS_RESOLVE_NAME_REQ
            case CSR_IP_SOCKET_DNS_RESOLVE_NAME_REQ:
            {
                CsrIpSocketDnsResolveNameReq *p = message;
                CsrPmemFree(p->name);
                p->name = NULL;
                break;
            }
#endif /* EXCLUDE_CSR_IP_SOCKET_DNS_RESOLVE_NAME_REQ */
#ifndef EXCLUDE_CSR_IP_SOCKET_TCP_DATA_REQ
            case CSR_IP_SOCKET_TCP_DATA_REQ:
            {
                CsrIpSocketTcpDataReq *p = message;
                CsrPmemFree(p->data);
                p->data = NULL;
                break;
            }
#endif /* EXCLUDE_CSR_IP_SOCKET_TCP_DATA_REQ */
#ifndef EXCLUDE_CSR_IP_SOCKET_RAW_DATA_REQ
            case CSR_IP_SOCKET_RAW_DATA_REQ:
            {
                CsrIpSocketRawDataReq *p = message;
                CsrPmemFree(p->data);
                p->data = NULL;
                break;
            }
#endif /* EXCLUDE_CSR_IP_SOCKET_RAW_DATA_REQ */
            default:
            {
                break;
            }
        } /* End switch */
    } /* End if */
    else
    {
        /* Unknown primitive type, exception handling */
    }
}
#endif /* EXCLUDE_CSR_IP_SOCKET_MODULE */
