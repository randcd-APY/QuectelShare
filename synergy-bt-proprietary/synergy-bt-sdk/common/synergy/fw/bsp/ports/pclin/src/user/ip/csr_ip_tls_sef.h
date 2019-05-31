#ifndef CSR_IP_TLS_SEF_H__
#define CSR_IP_TLS_SEF_H__

#include "csr_synergy.h"

/*****************************************************************************

Copyright (c) 2011-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_ip_tls_handler.h"

#ifdef __cplusplus
extern "C" {
#endif

void CsrIpTlsHandler(CsrIpTlsInstanceData *, void *);
void csrIpTlsSocketsHandle(CsrIpTlsInstanceData *,
    int *, fd_set *, fd_set *);
void csrIpTlsSocketsReap(CsrIpTlsInstanceData *);

#ifdef __cplusplus
}
#endif

#endif
