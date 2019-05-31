#ifndef CSR_HYDRA_TRANSPORT_H__
#define CSR_HYDRA_TRANSPORT_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2013-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/
#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

CsrBool CsrTransportHtransStart(void *ssdinst);
CsrBool CsrTransportHtransStop(void);
void *CsrTransportHtransOpen(const CsrUint8 *token);
void CsrTransportHtransClose(void);
CsrUint16 Send2Htrans(CsrUint8 channel, CsrUint8 *msg, CsrUint32 size);

#ifdef __cplusplus
}
#endif


#endif /* CSR_HYDRA_TRANSPORT_H__ */

