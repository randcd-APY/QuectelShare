#ifndef CSR_HTRANS_INIT_H__
#define CSR_HTRANS_INIT_H__

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

void CsrHtransDrvConfigure(const char *device);
void *CsrHtransDrvOpen(const CsrUint8 *token);
#ifdef __cplusplus
}
#endif

#endif /* ifndef CSR_HTRANS_INIT_H */
