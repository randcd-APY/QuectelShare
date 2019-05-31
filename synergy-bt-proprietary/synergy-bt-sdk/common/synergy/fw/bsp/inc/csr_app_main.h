#ifndef CSR_APP_MAIN_H__
#define CSR_APP_MAIN_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/* This function must be supplied by the application. It is called before the
   scheduler is started to allow the application to perform any required
   initialisation or argument parsing. */
CsrResult CsrAppMain(void);

/* This function must be supplied by the application and will be called if the
   --help argument is given, allowing the application may print additional usage
   information. */
void CsrAppMainUsage(void);

/* The following functions can be called by the application to retrieve
   Bluecore related parameters that have been extracted from the
   command line arguments. */
#ifdef CSR_USE_QCA_CHIP
CsrBool CsrAppMainQcFrwHciLogGet(void);
#endif
CsrUint16 CsrAppMainBluecoreTransportGet(void);
CsrUint32 CsrAppMainBluecorePreBootstrapBaudrateGet(void);
CsrUint32 CsrAppMainBluecorePostBootstrapBaudrateGet(void);
CsrUint32 CsrAppMainBluecoreXtalFrequencyGet(void);
void CsrAppMainBluecoreLocalDeviceBluetoothAddressGet(CsrUint8 address[6]);
const CsrCharString *CsrAppMainBluecoreLocalDeviceNameGet(void);
const CsrCharString *CsrAppMainBluecorePsrFileGet(void);
const CsrCharString *CsrAppMainBluecorePsrStringGet(void);

#ifdef __cplusplus
}
#endif

#endif
