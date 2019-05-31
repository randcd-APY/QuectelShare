/*****************************************************************************

      Copyright (c) 2017 Qualcomm Technologies International, Ltd.

      All Rights Reserved.

      Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#ifndef CSR_TM_BLUECORE_H4IBS_H__
#define CSR_TM_BLUECORE_H4IBS_H__

#include "csr_synergy.h"
#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif


void CsrTmBlueCoreH4ibsInit(void **gash);

void CsrTmBlueCoreRegisterUartHandleH4ibs(void *handle);

/* Enable/disable H4-IBS and UART LPM(LowPowerMode). */
void CsrTmBlueCoreH4ibsSetLowPowerMode(CsrBool ibs, CsrBool lpm);


#ifdef __cplusplus
}
#endif

#endif /* CSR_TM_BLUECORE_H4IBS_H__ */
