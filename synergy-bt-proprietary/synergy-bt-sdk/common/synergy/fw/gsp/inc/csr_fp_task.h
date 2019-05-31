#ifndef CSR_FP_TASK_H__
#define CSR_FP_TASK_H__

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

/* Queue definition for CSR_FP */
extern CsrSchedQid CSR_FP_IFACEQUEUE;

/* FP */
void CsrFpInit(void **gash);
void CsrFpHandler(void **gash);
void CsrFpDeinit(void **gash);

#define CSR_FP_INIT         CsrFpInit
#define CSR_FP_HANDLER      CsrFpHandler
#ifdef ENABLE_SHUTDOWN
#define CSR_FP_DEINIT       CsrFpDeinit
#else
#define CSR_FP_DEINIT       NULL
#endif

#ifdef __cplusplus
}
#endif

#endif
