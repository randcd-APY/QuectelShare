#ifndef CSR_TM_BLUECORE_H4I_H__
#define CSR_TM_BLUECORE_H4I_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void CsrTmBlueCoreH4iInit(void **gash);
void CsrTmBlueCoreRegisterUartHandleH4i(void *handle);

#ifdef __cplusplus
}
#endif

#endif /* CSR_TM_BLUECORE_H4I_H__ */
