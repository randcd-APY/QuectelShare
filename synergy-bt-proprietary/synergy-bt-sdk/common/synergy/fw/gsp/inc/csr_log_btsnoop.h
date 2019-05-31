#ifndef CSR_LOG_BTSNOOP_H__
#define CSR_LOG_BTSNOOP_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_log_formats.h"

#ifdef __cplusplus
extern "C" {
#endif

CsrLog *CsrLogBtsnoopCreate(void *ltHdl);
void CsrLogBtsnoopDestroy(CsrLog *l);

#ifdef __cplusplus
}
#endif

#endif
