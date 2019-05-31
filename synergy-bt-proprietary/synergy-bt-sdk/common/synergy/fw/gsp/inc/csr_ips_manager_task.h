#ifndef CSR_IPS_MANAGER_TASK_H__
#define CSR_IPS_MANAGER_TASK_H__
/*****************************************************************************

Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_sched.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Queue definition for CSR_IPS_MANAGER */
extern CsrSchedQid CSR_IPS_MANAGER_IFACEQUEUE;

/* Task definition for CSR_IPS_MANAGER */
void CsrIpsManagerInit(void **gash);
void CsrIpsManagerDeinit(void **gash);
void CsrIpsManagerHandler(void **gash);

#define CSR_IPS_MANAGER_INIT      CsrIpsManagerInit
#define CSR_IPS_MANAGER_HANDLER   CsrIpsManagerHandler
#ifdef ENABLE_SHUTDOWN
#define CSR_IPS_MANAGER_DEINIT    CsrIpsManagerDeinit
#else
#define CSR_IPS_MANAGER_DEINIT    NULL
#endif

#ifdef __cplusplus
}
#endif

#endif
