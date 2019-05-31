#ifndef CSR_LOG_FORMATS_EXT_H__
#define CSR_LOG_FORMATS_EXT_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_log.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSR_LOG_EXT_TYPE_TEST (1)

/* Base log format extension structure */
typedef struct
{
    CsrUint8 type;
} CsrLogExt;

/* Test log format extension structure */
typedef struct
{
    CsrUint8 type; /* Must be first */
    void (*ltestverdict)(void *ltHdl,
        CsrUint8 verdict,
        const CsrCharString *text,
        CsrUint32 line,
        const CsrCharString *file);
} CsrLogExtTest;

#ifdef __cplusplus
}
#endif

#endif
