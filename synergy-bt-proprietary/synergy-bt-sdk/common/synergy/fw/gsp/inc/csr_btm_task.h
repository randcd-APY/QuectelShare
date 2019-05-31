#ifndef CSR_BTM_TASK_H__
#define CSR_BTM_TASK_H__
/*****************************************************************************

Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_prim_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Queue definition for CSR_BTM */
extern CsrSchedQid CSR_BTM_IFACEQUEUE;

/* Task definition for CSR_BTM */
void CsrBtmInit(void **gash);
void CsrBtmDeinit(void **gash);
void CsrBtmHandler(void **gash);

#define CSR_BTM_INIT    CsrBtmInit
#define CSR_BTM_HANDLER CsrBtmHandler
#ifdef ENABLE_SHUTDOWN
#define CSR_BTM_DEINIT  CsrBtmDeinit
#else
#define CSR_BTM_DEINIT  NULL
#endif


#ifdef __cplusplus
}
#endif


#endif /* CSR_BTM_TASK_H__ */
