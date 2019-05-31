#ifndef CSR_LOG_TEST_H__
#define CSR_LOG_TEST_H__

#include "csr_synergy.h"
/******************************************************************************
Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/


#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void CsrLogTestVerdict(CsrUint8 verdict,
    const CsrCharString *text,
    CsrUint32 line,
    const CsrCharString *file);

#ifdef CSR_LOG_ENABLE
#define CSR_LOG_TEST_VERDICT(verdict, text) CsrLogTestVerdict(verdict, text, __LINE__, __FILE__)
#define CSR_LOG_TEST_TEXT(text) CSR_LOG_TEXT(text)
#else
#define CSR_LOG_TEST_VERDICT(verdict, text)
#define CSR_LOG_TEST_TEXT(text)
#endif

#ifdef __cplusplus
}
#endif

#endif
