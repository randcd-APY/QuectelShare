#ifndef CSR_VM_PRIM_H__
#define CSR_VM_PRIM_H__
/*****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/
#include "csr_types.h"
#include "csr_prim_defs.h"
#include "csr_sched.h"
#include "csr_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrVmPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrVmPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_VM_PRIM_DOWNSTREAM_LOWEST                  (0x0000)

#define CSR_VM_REGISTER_REQ               ((CsrVmPrim) (0x0000 + CSR_VM_PRIM_DOWNSTREAM_LOWEST))
#define CSR_VM_DATA_REQ                   ((CsrVmPrim) (0x0001 + CSR_VM_PRIM_DOWNSTREAM_LOWEST))

#define CSR_VM_PRIM_DOWNSTREAM_HIGHEST                 (0x0001 + CSR_VM_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_VM_PRIM_UPSTREAM_LOWEST                    (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_VM_REGISTER_CFM               ((CsrVmPrim) (0x0000 + CSR_VM_PRIM_UPSTREAM_LOWEST))
#define CSR_VM_DATA_IND                   ((CsrVmPrim) (0x0001 + CSR_VM_PRIM_UPSTREAM_LOWEST))

#define CSR_VM_PRIM_UPSTREAM_HIGHEST                   (0x0001 + CSR_VM_PRIM_UPSTREAM_LOWEST)

#define CSR_VM_PRIM_DOWNSTREAM_COUNT      (CSR_VM_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_VM_PRIM_DOWNSTREAM_LOWEST)
#define CSR_VM_PRIM_UPSTREAM_COUNT        (CSR_VM_PRIM_UPSTREAM_HIGHEST + 1 - CSR_VM_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrVmPrim   type;               /* Event identifier                                 */
    CsrSchedQid phandle;              /* Handle to application                            */
} CsrVmRegisterReq;

typedef struct
{
    CsrVmPrim type;                 /* Event identifier                                 */
    CsrResult result;               /* Status information about the registration        */
} CsrVmRegisterCfm;

typedef struct
{
    CsrVmPrim type;                 /* Event identifier                                 */
    CsrUint16 payloadLength;         /* The length of the outgoing VM message            */
    CsrUint8 *payload;               /* Pointer to the outgoing VM message               */
} CsrVmDataReq;

typedef struct
{
    CsrVmPrim type;                 /* Event identifier                                 */
    CsrUint16 payloadLength;         /* The length of the incoming VM message            */
    CsrUint8 *payload;               /* Pointer to the incoming VM message               */
} CsrVmDataInd;

#ifdef __cplusplus
}
#endif

#endif /* CSR_VM_PRIM_H__ */
