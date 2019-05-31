#ifndef CSR_TM_BLUECORE_H4_H__
#define CSR_TM_BLUECORE_H4_H__

#include "csr_synergy.h"
/******************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void CsrTmBlueCoreH4Init(void **gash);
void CsrTmBlueCoreRegisterUartHandleH4(void *handle);

#ifdef __cplusplus
}
#endif

#endif /* CSR_TM_BLUECORE_H4_H__ */
