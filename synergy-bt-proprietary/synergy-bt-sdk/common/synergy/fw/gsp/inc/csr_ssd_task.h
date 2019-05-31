#ifndef CSR_SSD_TASK_H__
#define CSR_SSD_TASK_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2013-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_sched.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Queue definition for CSR_SSD */
extern CsrSchedQid CSR_SSD_IFACEQUEUE;

/* SSD */
void CsrSsdInit(void **gash);
void CsrSsdHandler(void **gash);
void CsrSsdDeinit(void **gash);

void CsrTmBlueCoreHtransInit(void **gash);

#define CSR_SSD_INIT      CsrSsdInit
#define CSR_SSD_HANDLER   CsrSsdHandler
#ifdef ENABLE_SHUTDOWN
#define CSR_SSD_DEINIT    CsrSsdDeinit
#else
#define CSR_SSD_DEINIT    NULL
#endif

#ifdef __cplusplus
}
#endif

#endif
