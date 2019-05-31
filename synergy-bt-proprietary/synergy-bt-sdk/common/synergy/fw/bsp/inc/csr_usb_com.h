#ifndef CSR_USB_COM_H__
#define CSR_USB_COM_H__

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

CsrBool CsrUsbDrvStart(CsrUint8 reset);
CsrBool CsrUsbDrvStop(void);
CsrBool CsrUsbDrvTx(CsrUint8 channel, CsrUint8 *data, CsrUint32 size);

#ifdef __cplusplus
}
#endif

#endif
