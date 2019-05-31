#ifndef CSR_ENV_PRIM_H__
#define CSR_ENV_PRIM_H__
/*****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_prim_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrEnvPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrEnvPrim;

#define CSR_CLEANUP_IND        ((CsrEnvPrim) 0x0000)

typedef struct
{
    CsrEnvPrim type;
    CsrUint16  phandle;
} CsrCleanupInd;

#ifdef __cplusplus
}
#endif

#endif
