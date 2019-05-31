#ifndef CSR_DATA_STORE_TASK_H__
#define CSR_DATA_STORE_TASK_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_prim_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Queue definition for CSR_DATA_STORE */
extern CsrUint16 CSR_DATA_STORE_IFACEQUEUE;

/* Task definition for CSR_DATA_STORE */
void CsrDataStoreInit(void **gash);
void CsrDataStoreDeinit(void **gash);
void CsrDataStoreHandler(void **gash);

/* DATA_STORE */
#define CSR_DATA_STORE_INIT    CsrDataStoreInit
#define CSR_DATA_STORE_HANDLER CsrDataStoreHandler
#ifdef ENABLE_SHUTDOWN
#define CSR_DATA_STORE_DEINIT  CsrDataStoreDeinit
#else
#define CSR_DATA_STORE_DEINIT  NULL
#endif


#ifdef __cplusplus
}
#endif


#endif /* CSR_DATA_STORE_TASK_H__ */
