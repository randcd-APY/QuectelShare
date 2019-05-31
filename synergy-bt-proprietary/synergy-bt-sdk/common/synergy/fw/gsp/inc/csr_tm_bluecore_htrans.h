#ifndef CSR_TM_BLUECORE_HTRANS_H__
#define CSR_TM_BLUECORE_HTRANS_H__

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

void CsrTmBlueCoreHtransInit(void **gash);

void CsrTmBlueCoreHtransDeliverMsg(void *msg, CsrUint8 rel);

#ifdef __cplusplus
}
#endif

#endif
