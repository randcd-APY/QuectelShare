#ifndef CSR_BLUECORE_MANAGER_TASK_H__
#define CSR_BLUECORE_MANAGER_TASK_H__

#include "csr_synergy.h"
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

extern CsrSchedQid CSR_BLUECORE_MANAGER_IFACEQUEUE;

void CsrBluecoreManagerInit(void **gash);
void CsrBluecoreManagerDeinit(void **gash);
void CsrBluecoreManagerHandler(void **gash);

#define CSR_BLUECORE_MANAGER_INIT      CsrBluecoreManagerInit
#define CSR_BLUECORE_MANAGER_HANDLER   CsrBluecoreManagerHandler
#ifdef ENABLE_SHUTDOWN
#define CSR_BLUECORE_MANAGER_DEINIT    CsrBluecoreManagerDeinit
#else
#define CSR_BLUECORE_MANAGER_DEINIT    NULL
#endif

#ifdef __cplusplus
}
#endif

#endif
