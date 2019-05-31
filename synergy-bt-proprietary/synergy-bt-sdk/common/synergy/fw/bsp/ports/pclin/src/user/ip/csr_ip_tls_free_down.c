/*****************************************************************************

Copyright (c) 2011-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"

#ifndef EXCLUDE_CSR_TLS_MODULE
#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_mblk.h"
#include "csr_tls_lib.h"
#include "csr_tls_prim.h"

void CsrTlsFreeDownstreamMessageContents(CsrUint16 eventClass, void *message)
{
    if (eventClass == CSR_TLS_PRIM)
    {
        CsrTlsPrim *prim = (CsrTlsPrim *) message;
        switch (*prim)
        {
            case CSR_TLS_CONFIG_CERT_VERIFY_REQ:
            {
                CsrTlsConfigCertVerifyReq *p = message;
                CsrPmemFree(p->trustedCAcertificate);
                p->trustedCAcertificate = NULL;
                break;
            }
            case CSR_TLS_CONNECTION_DATA_REQ:
            {
                CsrTlsConnectionDataReq *p = message;
                CsrPmemFree(p->buf);
                p->buf = NULL;
                break;
            }
            case CSR_TLS_CONFIG_CIPHER_REQ:
            {
                CsrTlsConfigCipherReq *p = message;
                CsrPmemFree(p->cipherSuite);
                p->cipherSuite = NULL;
                break;
            }
            case CSR_TLS_CONFIG_CERT_CLIENT_REQ:
            {
                CsrTlsConfigCertClientReq *p = message;
                CsrPmemFree(p->certificate);
                p->certificate = NULL;
                CsrPmemFree(p->password);
                p->password = NULL;
                break;
            }
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
