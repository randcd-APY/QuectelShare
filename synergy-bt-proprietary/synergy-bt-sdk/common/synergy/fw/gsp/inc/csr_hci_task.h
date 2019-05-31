#ifndef CSR_HCI_TASK_H__
#define CSR_HCI_TASK_H__

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

/* Queue definition for CSR_HCI */
extern CsrUint16 CSR_HCI_IFACEQUEUE;

/* Task definition for CSR_HCI */
void CsrHciInit(void **gash);
void CsrHciDeinit(void **gash);
void CsrHciHandler(void **gash);

/* HCI */
#define CSR_HCI_INIT    CsrHciInit
#define CSR_HCI_HANDLER CsrHciHandler
#ifdef ENABLE_SHUTDOWN
#define CSR_HCI_DEINIT  CsrHciDeinit
#else
#define CSR_HCI_DEINIT  NULL
#endif


#ifdef __cplusplus
}
#endif


#endif /* CSR_HCI_TASK_H__ */
