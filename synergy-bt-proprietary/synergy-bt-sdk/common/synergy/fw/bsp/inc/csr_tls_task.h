#ifndef CSR_TLS_TASK_H__
#define CSR_TLS_TASK_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_sched.h"
#include "csr_prim_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * If CSR_IP_SUPPORT_TLS is defined, the TLS API
 * is implemented by the IP task.
 */
#ifdef CSR_IP_SUPPORT_TLS
#error csr_tls_task.h may not be included when CSR_IP_SUPPORT_TLS is defined!
#endif

/* Queue definition for CSR_TLS */
extern CsrSchedQid CSR_TLS_IFACEQUEUE;

/* Task definition for CSR_TLS */
void CsrTlsInit(void **gash);
void CsrTlsDeinit(void **gash);
void CsrTlsHandler(void **gash);

/* DATA_STORE */
#define CSR_TLS_INIT    CsrTlsInit
#define CSR_TLS_HANDLER CsrTlsHandler
#ifdef ENABLE_SHUTDOWN
#define CSR_TLS_DEINIT  CsrTlsDeinit
#else
#define CSR_TLS_DEINIT  NULL
#endif


#ifdef __cplusplus
}
#endif


#endif /* CSR_TLS_TASK_H__ */
