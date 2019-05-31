#ifndef CSR_PANIC_INIT_H__
#define CSR_PANIC_INIT_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

typedef void (*CsrPanicHandler)(CsrUint8 tech, CsrUint16 reason, const char *p);

/* Initial registration of external panic handler */
void CsrPanicInit(CsrPanicHandler cb);

#ifdef __cplusplus
}
#endif

#endif /* CSR_PANIC_INIT_H__ */
