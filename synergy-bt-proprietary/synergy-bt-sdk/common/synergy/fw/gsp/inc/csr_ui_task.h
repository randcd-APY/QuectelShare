#ifndef CSR_UI_TASK_H__
#define CSR_UI_TASK_H__

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

/* Queue definition for CSR_UI */
extern CsrSchedQid CSR_UI_IFACEQUEUE;

/* Task definition for CSR_UI */
void CsrUiInitialise(void **gash);
void CsrUiDeinitialise(void **gash);
void CsrUiHandler(void **gash);

/* UI */
#define CSR_UI_INIT CsrUiInitialise
#define CSR_UI_HANDLER CsrUiHandler
#ifdef ENABLE_SHUTDOWN
#define CSR_UI_DEINIT CsrUiDeinitialise
#else
#define CSR_UI_DEINIT NULL
#endif

#ifdef __cplusplus
}
#endif

#endif /* CSR_UI_TASK_H__ */
