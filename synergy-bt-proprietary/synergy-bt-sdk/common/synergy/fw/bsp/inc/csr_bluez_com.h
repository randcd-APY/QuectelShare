#ifndef CSR_BLUEZ_COM_H__
#define CSR_BLUEZ_COM_H__

/*****************************************************************************

Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"

#ifdef __cplusplus
extern "C" {
#endif

extern CsrBtTransport CsrBtTransportBluez;

void *CsrBtTransportBluezInit(const CsrCharString *device);
void CsrBtTransportBluezDeinit(void *arg);

#ifdef __cplusplus
}
#endif

#endif
