#ifndef CSR_APP_LOWER_H__
#define CSR_APP_LOWER_H__

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

typedef void (*CsrAppExitFunction)(void);

/* Call to register exit callback function. Must be called before initialising
   the CSR_APP task. The exit callback function will be called to indicate that
   the user has requested the application to exit. */
void CsrAppExitFunctionRegister(CsrAppExitFunction exitFunction);

#ifdef __cplusplus
}
#endif

#endif
