#ifndef CSR_HQ_TASK_H__
#define CSR_HQ_TASK_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_sched.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Queue definition for CSR_HQ */
extern CsrSchedQid CSR_HQ_IFACEQUEUE;

/* HQ */
void CsrHqInit(void **gash);
void CsrHqHandler(void **gash);
void CsrHqDeinit(void **gash);

#define CSR_HQ_INIT      CsrHqInit
#define CSR_HQ_HANDLER   CsrHqHandler
#ifdef ENABLE_SHUTDOWN
#define CSR_HQ_DEINIT    CsrHqDeinit
#else
#define CSR_HQ_DEINIT    NULL
#endif

#ifdef __cplusplus
}
#endif

#endif
