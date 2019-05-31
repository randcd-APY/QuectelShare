/*****************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/
#include "csr_synergy.h"

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_sched.h"
#include "csr_prim_defs.h"
#include "csr_log_text_2.h"

#include "csr_tls_prim.h"
#include "csr_tls_task.h"
#include "csr_tls_lib.h"
#include "csr_tls_thread.h"

#include "csr_tls_handler.h"
#include "csr_tls_util.h"
#include "csr_tls_sef_tls.h"

#include <openssl/crypto.h>

CSR_LOG_TEXT_HANDLE_DEFINE(CsrTlsLto);

#ifdef CSR_TLS_SUPPORT_PSK
CsrTlsInstanceData *globalCsrTlsInstanceData = NULL;
#endif

void CsrTlsInit(void **gash)
{
    CsrTlsInstanceData *instanceData;

    instanceData = (CsrTlsInstanceData *) CsrPmemZalloc(sizeof(*instanceData));
    *gash = (void *) instanceData;

    CSR_LOG_TEXT_REGISTER(&CsrTlsLto, "CSR_TLS", 0, NULL);

#ifdef CSR_TLS_SUPPORT_PSK
    /*added to be able to find psk key in callback*/
    globalCsrTlsInstanceData = instanceData;
#endif

    instanceData->qid = CsrSchedTaskQueueGet();

    CsrTlsThreadInit(&instanceData->threadInstanceData, instanceData);
}

void CsrTlsHandler(void **gash)
{
    CsrTlsInstanceData *instanceData;
    CsrUint16 event;

    instanceData = (CsrTlsInstanceData *) *gash;

    if (CsrSchedMessageGet(&event, &instanceData->msg) == TRUE)
    {
        switch (event)
        {
            case CSR_TLS_PRIM:
            {
                CsrTlsTlsHandler(instanceData);
                break;
            }

            case CSR_TLS_INTERNAL_PRIM:
            {
                /* This is an upstream signal */
                csrTlsMessageContainer *container = instanceData->msg;

                CsrSchedMessagePut(container->queue, container->prim,
                    container->message);

                break;
            }
            default:
                break;
        }

        CsrPmemFree(instanceData->msg);
    }
}

#ifdef ENABLE_SHUTDOWN
void CsrTlsDeinit(void **gash)
{
    CsrTlsInstanceData *instanceData;
    csrTlsSession *session;
    void *message;
    CsrUint16 event;

    instanceData = (CsrTlsInstanceData *) *gash;
    session = instanceData->sessionList;

#ifdef CSR_TLS_SUPPORT_PSK
    globalCsrTlsInstanceData = NULL;
#endif

    CsrTlsThreadDeinit(&instanceData->threadInstanceData);

    /* Deallocate instance data */

    while (session && instanceData->sessions)
    {
        sessionDestroyHandler(instanceData, session);
        session = instanceData->sessionList;
    }
    CsrPmemFree(instanceData);
    CRYPTO_cleanup_all_ex_data();

    /* Clean queue */
    while (CsrSchedMessageGet(&event, &message))
    {
        switch (event)
        {
            case CSR_TLS_PRIM:
            {
                CsrTlsFreeDownstreamMessageContents(event, message);
                break;
            }
            case CSR_TLS_INTERNAL_PRIM:
            {
                csrTlsMessageContainer *container;

                container = message;

                /* We only encapsulate upstream TLS primitives. */
                CsrTlsFreeUpstreamMessageContents(container->prim,
                    container->message);
                break;
            }
            default:
                break;
        }
        CsrPmemFree(message);
    }
}

#endif
