#ifndef CSR_TCP_TRANSCEIVER_TASK_H__
#define CSR_TCP_TRANSCEIVER_TASK_H__

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

extern CsrSchedQid CSR_TCP_TRANSCEIVER_IFACEQUEUE;
void CsrTcpTransceiverInit(void **gash);
void CsrTcpTransceiverDeinit(void **gash);
void CsrTcpTransceiverHandler(void **gash);

#define CSR_TCP_TRANSCEIVER_INIT      CsrTcpTransceiverInit
#define CSR_TCP_TRANSCEIVER_HANDLER   CsrTcpTransceiverHandler
#ifdef ENABLE_SHUTDOWN
#define CSR_TCP_TRANSCEIVER_DEINIT    CsrTcpTransceiverDeinit
#else
#define CSR_TCP_TRANSCEIVER_DEINIT    NULL
#endif

#ifdef __cplusplus
}
#endif

#endif
