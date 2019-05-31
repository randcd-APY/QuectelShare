#ifndef CSR_RESULT_H__
#define CSR_RESULT_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef CsrUint16 CsrResult;
#define CSR_RESULT_SUCCESS  ((CsrResult) 0x0000)
#define CSR_RESULT_FAILURE  ((CsrResult) 0xFFFF)

#ifdef __cplusplus
}
#endif

#endif
