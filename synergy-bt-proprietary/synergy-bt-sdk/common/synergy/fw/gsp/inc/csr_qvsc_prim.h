#ifndef CSR_QVSC_PRIM_H__
#define CSR_QVSC_PRIM_H__
/******************************************************************************
Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_types.h"
#include "csr_prim_defs.h"
#include "csr_sched.h"
#include "csr_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrQvscPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrQvscPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_QVSC_PRIM_DOWNSTREAM_LOWEST                     (0x0000)

#define CSR_QVSC_REQ                         ((CsrQvscPrim) (0x0000 + CSR_QVSC_PRIM_DOWNSTREAM_LOWEST))

#define CSR_QVSC_PRIM_DOWNSTREAM_HIGHEST                    (0x0000 + CSR_QVSC_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_QVSC_PRIM_UPSTREAM_LOWEST                       (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_QVSC_CFM                         ((CsrQvscPrim) (0x0000 + CSR_QVSC_PRIM_UPSTREAM_LOWEST))

#define CSR_QVSC_PRIM_UPSTREAM_HIGHEST                      (0x0000 + CSR_QVSC_PRIM_UPSTREAM_LOWEST)

#define CSR_QVSC_PRIM_DOWNSTREAM_COUNT     (CSR_QVSC_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_QVSC_PRIM_DOWNSTREAM_LOWEST)
#define CSR_QVSC_PRIM_UPSTREAM_COUNT       (CSR_QVSC_PRIM_UPSTREAM_HIGHEST + 1 - CSR_QVSC_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrQvscPrim  type;                      /* Event identifier                 */
    CsrSchedQid  phandle;                   /* Handle to application            */
    CsrUint16    ocf;                       /* Opcode comand field              */
    CsrUint16    payloadLength;             /* The length of the QVSC message   */
    CsrUint8    *payload;                   /* Pointer to the QVSC message      */
} CsrQvscReq;


typedef struct
{
    CsrQvscPrim  type;                      /* Event identifier                                 */
    CsrUint16    payloadLength;              /* The length of the payload of the QVSC message   */
    CsrUint8    *payload;                    /* Pointer to the payload of the QVSC message      */
} CsrQvscCfm;

#ifdef __cplusplus
}
#endif

#endif /* CSR_QVSC_PRIM_H__ */

