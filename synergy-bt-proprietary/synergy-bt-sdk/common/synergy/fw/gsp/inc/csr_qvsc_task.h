#ifndef CSR_QVSC_TASK_H__
#define CSR_QVSC_TASK_H__

#include "csr_synergy.h"
/******************************************************************************
Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_types.h"
#include "csr_sched.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Queue definition for CSR_QVSC */
extern CsrSchedQid CSR_QVSC_IFACEQUEUE;

/* QVSC */
void CsrQvscInit(void **gash);
void CsrQvscHandler(void **gash);
void CsrQvscDeinit(void **gash);

#define CSR_QVSC_INIT      CsrQvscInit
#define CSR_QVSC_HANDLER   CsrQvscHandler
#ifdef ENABLE_SHUTDOWN
#define CSR_QVSC_DEINIT    CsrQvscDeinit
#else
#define CSR_QVSC_DEINIT    NULL
#endif

#ifdef __cplusplus
}
#endif

#endif

