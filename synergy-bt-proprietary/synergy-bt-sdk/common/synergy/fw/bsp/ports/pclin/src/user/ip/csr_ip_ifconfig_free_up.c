/*****************************************************************************

Copyright (c) 2012-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

/* Note: this is an auto-generated file. */

#ifndef EXCLUDE_CSR_IP_IFCONFIG_MODULE
#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_mblk.h"
#include "csr_autogen.h"
#include "csr_ip_ifconfig_lib.h"
#include "csr_ip_ifconfig_prim.h"

void CsrIpIfconfigFreeUpstreamMessageContents(CsrUint16 eventClass, void *message)
{
    if (eventClass == CSR_IP_IFCONFIG_PRIM)
    {
        CsrIpIfconfigPrim *prim = (CsrIpIfconfigPrim *) message;
        switch (*prim)
        {
#ifndef EXCLUDE_CSR_IP_IFCONFIG_SUBSCRIBE_CFM
            case CSR_IP_IFCONFIG_SUBSCRIBE_CFM:
            {
                CsrIpIfconfigSubscribeCfm *p = message;
                CsrPmemFree(p->ifDetails);
                p->ifDetails = NULL;
                break;
            }
#endif /* EXCLUDE_CSR_IP_IFCONFIG_SUBSCRIBE_CFM */
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
#endif /* EXCLUDE_CSR_IP_IFCONFIG_MODULE */
