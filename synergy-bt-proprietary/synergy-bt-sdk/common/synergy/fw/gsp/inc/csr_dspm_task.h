#ifndef CSR_DSPM_TASK_H__
#define CSR_DSPM_TASK_H__
/*****************************************************************************

Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_sched.h"

#ifdef __cplusplus
extern "C" {
#endif

extern CsrSchedQid CSR_DSPM_IFACEQUEUE;

void CsrDspmInit(void **gash);
void CsrDspmHandler(void **gash);
void CsrDspmDeinit(void **gash);

#define CSR_DSPM_INIT      CsrDspmInit
#define CSR_DSPM_HANDLER   CsrDspmHandler
#ifdef ENABLE_SHUTDOWN
#define CSR_DSPM_DEINIT    CsrDspmDeinit
#else
#define CSR_DSPM_DEINIT    NULL
#endif

#ifdef __cplusplus
}
#endif

#endif
