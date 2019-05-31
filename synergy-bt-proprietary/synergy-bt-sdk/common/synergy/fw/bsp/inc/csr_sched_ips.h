#ifndef CSR_SCHED_IPS_H__
#define CSR_SCHED_IPS_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef CsrUint32 CsrSchedIpsContext;

#define CSR_SCHED_IPS_CONTEXT_CURRENT             ((CsrSchedIpsContext) ((0 << 16) | ((0) & 0xFFFF)))
#define CSR_SCHED_IPS_CONTEXT_SYSTEM(instance)    ((CsrSchedIpsContext) ((1 << 16) | ((instance) & 0xFFFF)))
#define CSR_SCHED_IPS_CONTEXT_TASK(queue)         ((CsrSchedIpsContext) ((2 << 16) | ((queue) & 0xFFFF)))
#define CSR_SCHED_IPS_CONTEXT_BGINT(handle)       ((CsrSchedIpsContext) ((3 << 16) | ((handle) & 0xFFFF)))

CsrUint32 *CsrSchedIpsInsert(CsrSchedIpsContext context, CsrUint32 *measurements);
CsrUint32 *CsrSchedIpsMeasurementsGet(CsrSchedIpsContext context);

#ifdef __cplusplus
}
#endif

#endif
