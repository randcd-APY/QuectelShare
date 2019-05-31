#ifndef CSR_SSD_PRIM_H__
#define CSR_SSD_PRIM_H__
/*****************************************************************************

Copyright (c) 2013-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_result.h"
#include "csr_sched.h"
#include "csr_prim_defs.h"
#include "csr_mblk.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Downstream Primitives */
#define CSR_SSD_PRIM_DOWNSTREAM_LOWEST                    (0x0000)

#define CSR_SSD_SERVICE_NOTIFICATION                   ((CsrPrim) (0x0000 + CSR_SSD_PRIM_DOWNSTREAM_LOWEST))
#define CSR_SSD_BTSS_SHUTDOWN_REQ                      ((CsrPrim) (0x0001 + CSR_SSD_PRIM_DOWNSTREAM_LOWEST))

#define CSR_SSD_PRIM_DOWNSTREAM_HIGHEST                   (0x0001 + CSR_SSD_PRIM_DOWNSTREAM_LOWEST)

/* Upstream Primitives */
#define CSR_SSD_PRIM_UPSTREAM_LOWEST                      (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_SSD_PRIM_UPSTREAM_HIGHEST                     (0x0000 + CSR_SSD_PRIM_UPSTREAM_LOWEST)

#define CSR_SSD_PRIM_DOWNSTREAM_COUNT      (CSR_SSD_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_SSD_PRIM_DOWNSTREAM_LOWEST)
#define CSR_SSD_PRIM_UPSTREAM_COUNT        (CSR_SSD_PRIM_UPSTREAM_HIGHEST + 1 - CSR_SSD_PRIM_UPSTREAM_LOWEST)

/* Primitive Structures */
typedef struct
{
    CsrPrim type;
    CsrSchedQid phandle;
    void *ssdinst;
}CsrSsdNotifyService;


typedef struct
{
    CsrPrim type;
}CsrSsdBtssShutdownReq;

#ifdef __cplusplus
}
#endif

#endif
