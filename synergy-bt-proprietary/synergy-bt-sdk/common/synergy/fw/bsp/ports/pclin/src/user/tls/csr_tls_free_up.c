/*****************************************************************************

Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

/* Note: this is an auto-generated file. */

#ifndef EXCLUDE_CSR_TLS_MODULE
#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_mblk.h"
#include "csr_autogen.h"
#include "csr_tls_lib.h"
#include "csr_tls_prim.h"

void CsrTlsFreeUpstreamMessageContents(CsrUint16 eventClass, void *message)
{
    if (eventClass == CSR_TLS_PRIM)
    {
        CsrTlsPrim *prim = (CsrTlsPrim *) message;
        switch (*prim)
        {
#ifndef EXCLUDE_CSR_TLS_CONNECTION_CERT_IND
            case CSR_TLS_CONNECTION_CERT_IND:
            {
                CsrTlsConnectionCertInd *p = message;
                CsrPmemFree(p->cert);
                p->cert = NULL;
                break;
            }
#endif /* EXCLUDE_CSR_TLS_CONNECTION_CERT_IND */
#ifndef EXCLUDE_CSR_TLS_CONNECTION_DATA_IND
            case CSR_TLS_CONNECTION_DATA_IND:
            {
                CsrTlsConnectionDataInd *p = message;
                CsrPmemFree(p->buf);
                p->buf = NULL;
                break;
            }
#endif /* EXCLUDE_CSR_TLS_CONNECTION_DATA_IND */
            default:
            {
                break;
            }
        } /* End switch */
    } /* End if */
    else
    {
        /* Unknown primitive type, exception handling */
    }
}
#endif /* EXCLUDE_CSR_TLS_MODULE */
