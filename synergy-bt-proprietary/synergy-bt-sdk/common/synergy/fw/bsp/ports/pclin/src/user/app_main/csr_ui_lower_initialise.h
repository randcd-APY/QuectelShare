#ifndef CSR_UI_LOWER_INITIALISE_H__
#define CSR_UI_LOWER_INITIALISE_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

CsrBool CsrUiLowerInitialise(CsrBool guiEnable, CsrBool tuiEnable, CsrBool tuiNoCls);
void CsrUiLowerEventLoop(void);

#ifdef __cplusplus
}
#endif

#endif
