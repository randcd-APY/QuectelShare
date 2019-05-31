#ifndef CSR_IP_IFCONFIG_SEF_H__
#define CSR_IP_IFCONFIG_SEF_H__

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

void CsrIpIfconfigRequestDefer(CsrIpInstanceData *instanceData);

/* These are called from thread context. */
void csrIpIfconfigReadRoute(CsrIpInstanceData *instanceData);
void csrIpIfconfigDeferredProcess(CsrIpInstanceData *instanceData);
void CsrIpIfconfigFreeUpstreamMessage(void *message);

#ifdef __cplusplus
}
#endif

#endif
