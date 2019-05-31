#ifndef CSR_TM_BLUECORE_BCSP_H__
#define CSR_TM_BLUECORE_BCSP_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void CsrTmBlueCoreBcspInit(void **gash);
void CsrTmBlueCoreRegisterUartHandleBcsp(void *handle);

#ifdef __cplusplus
}
#endif

#endif
