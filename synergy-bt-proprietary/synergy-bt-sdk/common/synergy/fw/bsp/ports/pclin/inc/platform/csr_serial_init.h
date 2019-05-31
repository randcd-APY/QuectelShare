#ifndef CSR_SERIAL_INIT_H__
#define CSR_SERIAL_INIT_H__

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

typedef CsrUint8 CsrUartDrvParity;
#define CSR_UART_DRV_PARITY_NONE    1
#define CSR_UART_DRV_PARITY_EVEN    0

typedef CsrUint8 CsrUartDrvDataBits;
#define CSR_UART_DRV_DATA_BITS_7    1
#define CSR_UART_DRV_DATA_BITS_8    0

typedef CsrUint8 CsrUartDrvStopBits;
#define CSR_UART_DRV_STOP_BITS_1    1
#define CSR_UART_DRV_STOP_BITS_2    0

/* DEPRECATED: This function is provided for backward compatibility, please
   use CsrUartDrvOpen in new code. */
void *CsrUartDrvInitialise(const CsrCharString *device,
    CsrUint32 baud, CsrUint32 resetBaud,
    CsrUartDrvDataBits dataBits,
    CsrUartDrvParity parity,
    CsrUartDrvStopBits stopBits,
    CsrBool flowControl,
    const CsrUint8 *token);

/* DEPRECATED: This function is provided for backward compatibility, please
   use CsrUartDrvClose in new code */
void CsrUartDrvDeinitialise(void *handle);

CsrUint32 CsrUartDrvBaudrateActualGet(CsrUint32 wantedBaudrate);

#ifdef __cplusplus
}
#endif

#endif
