#ifndef CSR_FSAL_TASK_H__
#define CSR_FSAL_TASK_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Queue definition for CSR_FSAL */
extern CsrUint16 CSR_FSAL_IFACEQUEUE;

/* Task definition for CSR_FSAL */
void CsrFsalInit(void **gash);
void CsrFsalDeinit(void **gash);
void CsrFsalHandler(void **gash);

/* CSR_FSAL */
#define CSR_FSAL_INIT CsrFsalInit
#define CSR_FSAL_HANDLER CsrFsalHandler
#ifdef ENABLE_SHUTDOWN
#define CSR_FSAL_DEINIT CsrFsalDeinit
#else
#define CSR_FSAL_DEINIT NULL
#endif


#ifdef __cplusplus
}
#endif

#endif
