#ifndef CSR_USB_INIT_H__
#define CSR_USB_INIT_H__

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

void CsrUsbDrvConfigure(const char *device);
#ifdef __cplusplus
}
#endif

#endif /* ifndef CSR_USB_INIT_H */
