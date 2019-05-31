/*****************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/
#include "csr_synergy.h"

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_result.h"
#include "csr_sched.h"

#include "csr_tls_prim.h"
#include "csr_tls_task.h"
#include "csr_tls_util.h"

/*
 * Some messages are sent directly to the intended task, and some
 * are sent indirectly and forwarded from CsrTlsHandler().  The
 * ones sent indirectly are the ones that are sent from the TLS
 * worker thread.
 */

static void tlsMessageForward(CsrSchedQid queue, CsrPrim prim, void *message)
{
    csrTlsMessageContainer *container;

    container = CsrPmemAlloc(sizeof(*container));
    container->queue = queue;
    container->prim = prim;
    container->message = message;

    CsrSchedMessagePut(CSR_TLS_IFACEQUEUE, CSR_TLS_INTERNAL_PRIM, container);
}

void CsrTlsSessionCreateCfmSend(CsrSchedQid qid,
    CsrTlsSession session, CsrResult result)
{
    CsrTlsSessionCreateCfm *tlsCfm;

    tlsCfm = CsrPmemAlloc(sizeof(*tlsCfm));
    tlsCfm->type = CSR_TLS_SESSION_CREATE_CFM;
    tlsCfm->session = session;
    tlsCfm->result = result;

    CsrSchedMessagePut(qid, CSR_TLS_PRIM, tlsCfm);
}

void CsrTlsSessionDestroyCfmSend(CsrSchedQid qid,
    CsrTlsSession session, CsrResult result)
{
    CsrTlsSessionDestroyCfm *tlsCfm;

    tlsCfm = CsrPmemAlloc(sizeof(*tlsCfm));
    tlsCfm->type = CSR_TLS_SESSION_DESTROY_CFM;
    tlsCfm->session = session;
    tlsCfm->result = result;

    CsrSchedMessagePut(qid, CSR_TLS_PRIM, tlsCfm);
}

void CsrTlsConnectionCreateCfmSend(CsrSchedQid qid,
    CsrTlsSession session, CsrResult result, CsrTlsConnection conn)
{
    CsrTlsConnectionCreateCfm *tlsCfm;

    tlsCfm = CsrPmemAlloc(sizeof(*tlsCfm));
    tlsCfm->type = CSR_TLS_CONNECTION_CREATE_CFM;
    tlsCfm->session = session;
    tlsCfm->result = result;
    tlsCfm->conn = conn;

    CsrSchedMessagePut(qid, CSR_TLS_PRIM, tlsCfm);
}

void CsrTlsConnectionDestroyCfmSend(CsrSchedQid qid,
    CsrTlsConnection conn)
{
    CsrTlsConnectionDestroyCfm *tlsCfm;

    tlsCfm = CsrPmemAlloc(sizeof(*tlsCfm));
    tlsCfm->type = CSR_TLS_CONNECTION_DESTROY_CFM;
    tlsCfm->conn = conn;

    CsrSchedMessagePut(qid, CSR_TLS_PRIM, tlsCfm);
}

void CsrTlsConnectionBindCfmSend(CsrSchedQid qid,
    CsrTlsConnection conn, CsrResult result)
{
    CsrTlsConnectionBindCfm *tlsCfm;

    tlsCfm = CsrPmemAlloc(sizeof(*tlsCfm));
    tlsCfm->type = CSR_TLS_CONNECTION_BIND_CFM;
    tlsCfm->conn = conn;
    tlsCfm->result = result;

    CsrSchedMessagePut(qid, CSR_TLS_PRIM, tlsCfm);
}

void CsrTlsConnectionConnectCfmSend(CsrSchedQid qid,
    CsrTlsConnection conn, CsrResult result)
{
    CsrTlsConnectionConnectCfm *tlsCfm;

    tlsCfm = CsrPmemAlloc(sizeof(*tlsCfm));
    tlsCfm->type = CSR_TLS_CONNECTION_CONNECT_CFM;
    tlsCfm->conn = conn;
    tlsCfm->result = result;

    tlsMessageForward(qid, CSR_TLS_PRIM, tlsCfm);
}

void CsrTlsConnectionDisconnectIndSend(CsrSchedQid qid,
    CsrTlsConnection conn)
{
    CsrTlsConnectionDisconnectInd *tlsInd;

    tlsInd = CsrPmemAlloc(sizeof(*tlsInd));
    tlsInd->type = CSR_TLS_CONNECTION_DISCONNECT_IND;
    tlsInd->conn = conn;

    tlsMessageForward(qid, CSR_TLS_PRIM, tlsInd);
}

void CsrTlsConnectionDisconnectCfmSend(CsrSchedQid qid,
    CsrTlsConnection conn, CsrResult result)
{
    CsrTlsConnectionDisconnectCfm *tlsCfm;

    tlsCfm = CsrPmemAlloc(sizeof(*tlsCfm));
    tlsCfm->type = CSR_TLS_CONNECTION_DISCONNECT_CFM;
    tlsCfm->conn = conn;
    tlsCfm->result = result;

    tlsMessageForward(qid, CSR_TLS_PRIM, tlsCfm);
}

void CsrTlsConnectionDataCfmSend(CsrSchedQid qid,
    CsrTlsConnection conn, CsrResult result)
{
    CsrTlsConnectionDataCfm *tlsCfm;

    tlsCfm = CsrPmemAlloc(sizeof(*tlsCfm));
    tlsCfm->type = CSR_TLS_CONNECTION_DATA_CFM;
    tlsCfm->conn = conn;
    tlsCfm->result = result;

    tlsMessageForward(qid, CSR_TLS_PRIM, tlsCfm);
}

void CsrTlsConnectionDataIndSend(CsrSchedQid qid,
    CsrTlsConnection conn, CsrSize bufLen, CsrUint8 *buf)
{
    CsrTlsConnectionDataInd *tlsInd;

    tlsInd = CsrPmemAlloc(sizeof(*tlsInd));
    tlsInd->type = CSR_TLS_CONNECTION_DATA_IND;
    tlsInd->conn = conn;
    tlsInd->bufLen = bufLen;
    tlsInd->buf = buf;

    tlsMessageForward(qid, CSR_TLS_PRIM, tlsInd);
}

void CsrTlsConnectionCertIndSend(CsrSchedQid qid,
    CsrTlsConnection conn, CsrSize certLen, CsrUint8 *cert, CsrResult result)
{
    CsrTlsConnectionCertInd *tlsInd;

    tlsInd = CsrPmemAlloc(sizeof(*tlsInd));
    tlsInd->type = CSR_TLS_CONNECTION_CERT_IND;
    tlsInd->conn = conn;
    tlsInd->certLen = certLen;
    tlsInd->cert = cert;
    tlsInd->result = result;

    tlsMessageForward(qid, CSR_TLS_PRIM, tlsInd);
}
