#ifndef CSR_AM_TASK_H__
#define CSR_AM_TASK_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_sched.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Queue definition for CSR_AM */
extern CsrSchedQid CSR_AM_IFACEQUEUE;

/* AM */
void CsrAmInit(void **gash);
void CsrAmHandler(void **gash);
void CsrAmDeinit(void **gash);

#define CSR_AM_INIT      CsrAmInit
#define CSR_AM_HANDLER   CsrAmHandler
#ifdef ENABLE_SHUTDOWN
#define CSR_AM_DEINIT    CsrAmDeinit
#else
#define CSR_AM_DEINIT    NULL
#endif

#ifdef __cplusplus
}
#endif


#endif /* CSR_AM_TASK_H__ */
