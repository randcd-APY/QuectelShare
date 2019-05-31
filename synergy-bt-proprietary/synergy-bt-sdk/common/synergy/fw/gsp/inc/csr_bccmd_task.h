#ifndef CSR_BCCMD_TASK_H__
#define CSR_BCCMD_TASK_H__

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

/* Queue definition for CSR_BCCMD */
extern CsrSchedQid CSR_BCCMD_IFACEQUEUE;

/* BCCMD */
void CsrBccmdInit(void **gash);
void CsrBccmdHandler(void **gash);
void CsrBccmdDeinit(void **gash);

#define CSR_BCCMD_INIT      CsrBccmdInit
#define CSR_BCCMD_HANDLER   CsrBccmdHandler
#ifdef ENABLE_SHUTDOWN
#define CSR_BCCMD_DEINIT    CsrBccmdDeinit
#else
#define CSR_BCCMD_DEINIT    NULL
#endif

#ifdef __cplusplus
}
#endif

#endif
