#ifndef CSR_VM_TASK_H__
#define CSR_VM_TASK_H__

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

/* Queue definition for CSR_VM */
extern CsrSchedQid CSR_VM_IFACEQUEUE;

/* VM */
void CsrVmInit(void **gash);
void CsrVmHandler(void **gash);
void CsrVmDeinit(void **gash);

#define CSR_VM_INIT      CsrVmInit
#define CSR_VM_HANDLER   CsrVmHandler
#ifdef ENABLE_SHUTDOWN
#define CSR_VM_DEINIT    CsrVmDeinit
#else
#define CSR_VM_DEINIT    NULL
#endif

#ifdef __cplusplus
}
#endif

#endif
