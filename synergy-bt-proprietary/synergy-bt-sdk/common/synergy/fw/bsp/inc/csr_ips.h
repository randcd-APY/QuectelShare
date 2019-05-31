#ifndef CSR_IPS_H__
#define CSR_IPS_H__

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

typedef void *CsrIpsContext;

#define CSR_IPS_CONTEXT_CURRENT         ((CsrIpsContext) 0)
#define CSR_IPS_CONTEXT_SYSTEM          ((CsrIpsContext) 1)
#define CSR_IPS_CONTEXT_EVERYTHINGELSE  ((CsrIpsContext) 2)
#define CSR_IPS_CONTEXT_THREAD(thread)  ((CsrIpsContext) (thread))

void CsrIpsConfigure(CsrUint32 currentInterval, CsrUint32 maximumInterval, CsrUint32 intervalLength);
CsrUint32 *CsrIpsInsert(CsrIpsContext context, CsrUint32 *measurements);
CsrUint32 *CsrIpsMeasurementsGet(CsrIpsContext context);
void CsrIpsEnable(void);
void CsrIpsDisable(void);
CsrUint32 CsrIpsResolutionGet(void);
CsrUint32 CsrIpsCurrentIntervalGet(void);

#ifdef __cplusplus
}
#endif

#endif
