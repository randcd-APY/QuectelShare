#ifndef CSR_TM_BLUECORE_H4DS_H__
#define CSR_TM_BLUECORE_H4DS_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void CsrTmBlueCoreH4dsInit(void **gash);
void CsrTmBlueCoreRegisterUartHandleH4ds(void *handle);

#ifdef __cplusplus
}
#endif

#endif /* CSR_TM_BLUECORE_H4DS_H__ */
