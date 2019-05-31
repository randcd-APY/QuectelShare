#ifndef CSR_HTRANS_COM_H__
#define CSR_HTRANS_COM_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2013-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_sched.h"
#ifdef __cplusplus
extern "C" {
#endif

#define inline

/*****************************************************************************

    NAME
        CsrHtransDrvDataRx

    DESCRIPTION
        This callback type is used for the callback that is registerd in the
        CsrHtransDrvRegister function and called by the CsrHtransDrvRx function.
        Please refer to the description of these functions for further
        information.

    PARAMETERS
        data - pointer to the data.
        dataLength - the number of contiguous data bytes available.

    RETURNS
        The number of data bytes consumed from the data pointer.

*****************************************************************************/
typedef CsrUint32 (*CsrHtransDrvDataRx)(const CsrUint8 *data, CsrUint32 dataLength);


CsrBool CsrHtransDrvStart(CsrUint8 reset,void *ssdinst);
CsrBool CsrHtransDrvStop(void *ssdinst);
void *CsrHtransDrvOpen(const CsrUint8 *token);
CsrBool CsrHtransDrvTx(CsrUint8 channel, CsrUint8 *data, CsrUint32 size, void *ssdinst);
void CsrHtransDrvRx(void *unused);

void CsrHtransDrvRegister(void *unused_handle, CsrHtransDrvDataRx rxDataFn, CsrSchedBgint rxBgintHdl);




#ifdef __cplusplus
}
#endif

#endif
