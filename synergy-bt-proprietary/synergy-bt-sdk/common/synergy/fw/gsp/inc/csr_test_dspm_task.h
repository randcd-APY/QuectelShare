#ifndef CSR_TEST_DSPM_TASK_H__
#define CSR_TEST_DSPM_TASK_H__
/*****************************************************************************

Copyright (c) 2012-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "csr_types.h"
#include "csr_sched.h"

extern CsrSchedQid CSR_TEST_DSPM_IFACEQUEUE;

void CsrTestDspmInit(void **arg);
void CsrTestDspmHandler(void **arg);
void CsrTestDspmDeinit(void **arg);

#ifdef __cplusplus
}
#endif

#endif
