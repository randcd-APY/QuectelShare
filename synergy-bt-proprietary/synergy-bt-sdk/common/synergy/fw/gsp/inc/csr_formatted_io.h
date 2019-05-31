#ifndef CSR_FORMATTED_IO_H__
#define CSR_FORMATTED_IO_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "csr_types.h"

CsrInt32 CsrSnprintf(CsrCharString *dest, CsrSize n, const CsrCharString *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
