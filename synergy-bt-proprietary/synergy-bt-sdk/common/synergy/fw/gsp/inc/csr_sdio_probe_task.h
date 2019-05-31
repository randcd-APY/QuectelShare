#ifndef CSR_SDIO_PROBE_TASK_H__
#define CSR_SDIO_PROBE_TASK_H__

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

extern CsrSchedQid CSR_SDIO_PROBE_IFACEQUEUE;
void CsrSdioProbeInit(void **gash);
void CsrSdioProbeDeinit(void **gash);
void CsrSdioProbeHandler(void **gash);

#define CSR_SDIO_PROBE_INIT      CsrSdioProbeInit
#define CSR_SDIO_PROBE_HANDLER   CsrSdioProbeHandler
#ifdef ENABLE_SHUTDOWN
#define CSR_SDIO_PROBE_DEINIT    CsrSdioProbeDeinit
#else
#define CSR_SDIO_PROBE_DEINIT    NULL
#endif

#ifdef __cplusplus
}
#endif

#endif
