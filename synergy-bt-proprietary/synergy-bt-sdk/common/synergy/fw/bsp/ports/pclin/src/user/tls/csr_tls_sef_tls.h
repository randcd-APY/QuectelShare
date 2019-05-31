#ifndef CSR_TLS_SEF_TLS_H__
#define CSR_TLS_SEF_TLS_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_tls_handler.h"

#ifdef __cplusplus
extern "C" {
#endif

void CsrTlsTlsHandler(CsrTlsInstanceData *);
csrTlsSession *sessionCreateHandler(CsrTlsInstanceData *instanceData, CsrSchedQid qid);
void sessionDestroyHandler(CsrTlsInstanceData *instanceData, csrTlsSession *session);

csrTlsConnection *connectionCreateReqHandler(csrTlsSession *session);

void connectionDestroyReqHandler(csrTlsConnection *conn);


#ifdef __cplusplus
}
#endif

#endif
