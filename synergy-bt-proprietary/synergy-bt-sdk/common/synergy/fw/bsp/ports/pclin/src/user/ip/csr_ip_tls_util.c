/*****************************************************************************

Copyright (c) 2011-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"

#include <unistd.h>

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_result.h"
#include "csr_sched.h"

#include "csr_log.h"
#include "csr_log_text_2.h"



#include "csr_tls_prim.h"
#include "csr_tls_lib.h"

#include "csr_ip_tls_handler.h"
#include "csr_ip_tls_util.h"

#include "csr_ip_util.h"

/* Internal helpers. */
static void returnSocketHandle(CsrIpTlsInstanceData *instanceData, CsrInt32 sockHandle)
{
    if ((sockHandle >= 0) && (sockHandle < CSR_TLS_MAX_SOCKETS))
    {
        instanceData->socketInstance[sockHandle] = NULL;

        if (sockHandle == instanceData->maxSocketHandle)
        {
            CsrInt32 i;

            for (i = instanceData->maxSocketHandle - 1;
                 i >= 0;
                 i--)
            {
                if (instanceData->socketInstance[i] != NULL)
                {
                    break;
                }
            }

            instanceData->maxSocketHandle = i;
        }
    }
}

#ifdef CSR_LOG_ENABLE
static void sslInfoCallback(const SSL *s, int where, int ret)
{
    const char *str;
    int w;

    w = where & ~SSL_ST_MASK;

    if (w & SSL_ST_CONNECT)
    {
        str = "SSL_connect";
    }
    else if (w & SSL_ST_ACCEPT)
    {
        str = "SSL_accept";
    }
    else
    {
        str = "undefined";
    }

    CSR_LOG_TEXT_INFO((CsrIpLto, 4, "%s: %d: %d", str, where, SSL_get_error(s, ret)));
}
#endif

static int sslCertVerifyCallback(int preverify_ok, X509_STORE_CTX *ctx)
{
    CsrTlsSocketInst *sockInst;
    X509 *x509Cert;
    int err, depth;
    CsrUint32 eventBits;
    CsrResult res;
    CsrSize certLen;
    CsrUint8 *cert;
    BIO *bp;
    CsrResult returnValue;

    returnValue = CSR_RESULT_SUCCESS;

    x509Cert = X509_STORE_CTX_get_current_cert(ctx);
    err = X509_STORE_CTX_get_error(ctx);
    depth = X509_STORE_CTX_get_error_depth(ctx);

    /* Read the server certificate for sending to user app.*/
    bp = BIO_new(BIO_s_mem());
    if (bp == NULL)
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 4, "Could not allocate BIO mem buffer"));
        BIO_free(bp);
        return 0;
    }

    if (!PEM_write_bio_X509(bp, x509Cert))
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 4, "Could not extract X509 certificate"));
        BIO_free(bp);
        return 0;
    }

    certLen = BIO_get_mem_data(bp, &cert);
    (void) BIO_set_close(bp, BIO_NOCLOSE);
    BIO_free(bp);

    CSR_LOG_TEXT_BUFFER_INFO((CsrIpLto, 4, certLen, cert, "Server certificate verify callback, preverify: %u, err: %d", preverify_ok, err));

    sockInst = globalCsrIpTlsInstanceData->currentSockInst;

    /*
     * If the cert was generally ok, take a closer look at it.
     */
    if (preverify_ok == 1)
    {
        /*
         * If this is the server certificate and the application gave us a
         * server name to check, ... check it.
         */
        if ((depth == 0) &&
            
            (sockInst->serverName != NULL))
        {
            X509_NAME *subjectName;
            int commonNameLen;
            char commonName[256];

            subjectName = X509_get_subject_name(x509Cert);
            commonNameLen = X509_NAME_get_text_by_NID(subjectName,
                NID_commonName, commonName, sizeof(commonName));

            /*
             * If the name is missing (let's be cautious) or doesn't
             * match what was requested by the application, say so.
             */
            if ((commonNameLen == -1) ||
                (strcmp(sockInst->serverName, commonName) != 0))
            {
                preverify_ok = 0;
                returnValue = CSR_TLS_CERT_CN_MISMATCH;
            }
        }
    }
    else
    {
        /* Translating OpenSSL error code to CSR TLS error code */
        switch (err)
        {
            case X509_V_ERR_CERT_NOT_YET_VALID:
                returnValue = CSR_TLS_CERT_NOT_YET_VALID;
                break;
            case X509_V_ERR_CERT_HAS_EXPIRED:
                returnValue = CSR_TLS_CERT_HAS_EXPIRED;
                break;
            case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT:
                returnValue = CSR_TLS_CERT_SELFSIGNED;
                break;
            case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY:
                returnValue = CSR_TLS_CERT_MISSING_ISSUER_CERT;
                break;
            case X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE:
                returnValue = CSR_TLS_CERT_UNABLE_TO_VERIFY_NO_SELFSIGNED_CERT;
                break;
            case X509_V_ERR_INVALID_CA:
                returnValue = CSR_TLS_CERT_CA_INVALID;
                break;
            default:
                returnValue = CSR_RESULT_FAILURE;
        }
    }

    CsrTlsConnectionCertIndSend(sockInst->conn->session->qid,
        sockInst->conn,
        certLen,
        cert,
        returnValue);

    /* Unlock allowing the application to respond */
    CsrIpUnlock(globalCsrIpTlsInstanceData->ipInstanceData);
    res = CsrEventWait(&sockInst->callbackEvent, CSR_EVENT_WAIT_INFINITE, &eventBits);
    CsrIpLock(globalCsrIpTlsInstanceData->ipInstanceData);
    CSR_TLS_ASSERT(res == CSR_RESULT_SUCCESS, "could not wait for event");
    CSR_TLS_ASSERT(eventBits == 0x1, "events bits not set");

    return sockInst->acceptCert;
}

/* External API. */
CsrTlsSocketInst *csrIpTlsInstGet(CsrIpTlsInstanceData *instanceData,
    CsrInt32 sockHandle)
{
    if ((sockHandle >= 0) && (sockHandle < CSR_TLS_MAX_SOCKETS))
    {
        return instanceData->socketInstance[sockHandle];
    }

    return NULL;
}

void csrIpTlsConnectionDestroy(csrTlsConnection *conn)
{
    if (conn)
    {
        csrTlsSession *sess;

        sess = conn->session;

        /*
        * Remove connection from session connection list.
        * Trivial if connection is at head of list, otherwise
        * find previous element.
        */
        if (conn == sess->connList)
        {
            sess->connList = conn->next;
        }
        else
        {
            csrTlsConnection *prevConn;

            for (prevConn = sess->connList;
                 prevConn;
                 prevConn = prevConn->next)
            {
                if (prevConn->next == conn)
                {
                    break;
                }
            }

            prevConn->next = conn->next;
        }
        sess->connections--;

        CsrPmemFree(conn);
    }
}

void csrIpTlsSessionDestroy(CsrIpTlsInstanceData *instanceData, csrTlsSession *sess)
{
    if (sess != NULL)
    {
        csrTlsSession *prevSess, *iterSess;

        for (prevSess = NULL, iterSess = instanceData->tlsSessionList;
             iterSess;
             prevSess = iterSess, iterSess = iterSess->next)
        {
            if (iterSess == sess)
            {
                break;
            }
        }

        if (iterSess != NULL)
        {
            csrTlsConnection *conn;

            if (prevSess != NULL)
            {
                prevSess->next = iterSess->next;
            }
            else
            {
                instanceData->tlsSessionList = iterSess->next;
            }

            conn = iterSess->connList;
            while (conn && iterSess->connections)
            {
                csrIpTlsConnectionDestroy(conn);
                conn = iterSess->connList;
            }

            SSL_CTX_free(iterSess->ctx);
#ifdef CSR_TLS_SUPPORT_PSK
            CsrPmemFree(iterSess->pskKey);
#endif
            CsrPmemFree(iterSess);
            instanceData->tlsSessions--;
        }
        /*
        * If we don't find it, someone's violating the API.
        * Since we don't have the session, we don't have a
        * qid to respond to so we just drop the request.
        */
    }
}

void csrTlsSocketInstFree(CsrIpTlsInstanceData *instanceData, CsrTlsSocketInst *sockInst)
{
    csrTlsRxElement *rxelem, *rxnext;

    if (sockInst->rxQueue != NULL)
    {
        /*
         * Free receive queue.  The first element has already been
         * sent to the application so it owns the data buffer.
         * Free element and data buffer of subsequent elements.
         */
        rxelem = sockInst->rxQueue;
        rxnext = rxelem->next;

        CsrPmemFree(rxelem);

        for (rxelem = rxnext;
             rxelem;
             rxelem = rxnext)
        {
            rxnext = rxelem->next;
            CsrPmemFree(rxelem->buf);
            CsrPmemFree(rxelem);
        }
    }

    CsrEventDestroy(&sockInst->callbackEvent);
    ERR_clear_error();
    csrIpTlsConnectionDestroy(sockInst->conn);
    if (sockInst->ssl)
    {
        SSL_free(sockInst->ssl);
    }

    returnSocketHandle(instanceData, sockInst->handle);
    CsrIpFdRemove(instanceData->ipInstanceData, sockInst->socket, TRUE, TRUE);

    close(sockInst->socket);

    CsrPmemFree(sockInst->serverName);
    CsrPmemFree(sockInst->data);
    CsrPmemFree(sockInst);
}

int CsrIpTlsMaxFdGet(CsrIpTlsInstanceData *instanceData)
{
    CsrInt32 i;
    int fd;

    for (fd = -1, i = 0;
         i < CSR_TLS_MAX_SOCKETS;
         i++)
    {
        if (instanceData->socketInstance[i] != NULL)
        {
            if (instanceData->socketInstance[i]->socket > fd)
            {
                fd = instanceData->socketInstance[i]->socket;
            }
        }
    }

    return fd;
}

void csrIpTlsCreateSsl(CsrIpTlsInstanceData *instanceData,
    CsrTlsSocketInst *sockInst)
{
    int res;
    BIO *sbio;

    sockInst->ssl = SSL_new(sockInst->conn->session->ctx);
    CSR_TLS_ASSERT(sockInst->ssl != NULL, "SSL connection is NULL");

#ifdef CSR_LOG_ENABLE
    SSL_set_info_callback(sockInst->ssl, sslInfoCallback);
#endif

    SSL_set_verify(sockInst->ssl, SSL_VERIFY_PEER, sslCertVerifyCallback);

    sbio = BIO_new_socket(sockInst->socket, BIO_NOCLOSE);
    CSR_TLS_ASSERT(sbio != NULL, "SSL socket BIO failed");

    SSL_set_bio(sockInst->ssl, sbio, sbio);

    res = ERR_get_error();
    CSR_TLS_ASSERT(res == 0, "SSL error queue not empty");

    if (sockInst->serverName != NULL)
    {
        SSL_set_tlsext_host_name(sockInst->ssl, sockInst->serverName);
    }

    SSL_set_connect_state(sockInst->ssl);
}

void csrIpTlsConnectSsl(CsrIpTlsInstanceData *instanceData,
    CsrTlsSocketInst *sockInst)
{
    int res;

    CsrIpFdRemove(instanceData->ipInstanceData, sockInst->socket, TRUE, TRUE);

    res = SSL_connect(sockInst->ssl);
    res = SSL_get_error(sockInst->ssl, res);

    switch (res)
    {
        case SSL_ERROR_NONE:
        {
            CSR_LOG_TEXT_INFO((CsrIpLto, 4,
                               "SSL connected on socket %d", sockInst->socket));
            sockInst->type = CSR_TLS_TYPE_TCP_DATA;
            CsrIpFdAdd(instanceData->ipInstanceData, sockInst->socket, TRUE, FALSE);
            CsrTlsConnectionConnectCfmSend(sockInst->conn->session->qid,
                sockInst->conn, CSR_RESULT_SUCCESS);
            break;
        }

        case SSL_ERROR_WANT_WRITE:
        {
            CsrIpFdAdd(instanceData->ipInstanceData, sockInst->socket, FALSE, TRUE);
            break;
        }

        case SSL_ERROR_WANT_READ:
        {
            CsrIpFdAdd(instanceData->ipInstanceData, sockInst->socket, TRUE, FALSE);
            break;
        }

        case SSL_ERROR_SSL:
        {
#ifdef CSR_LOG_ENABLE
            unsigned long error = ERR_get_error();
#endif
            CSR_LOG_TEXT_WARNING((CsrIpLto, 4,
                                  "SSL connect %s", ERR_error_string(error, NULL)));
            CsrTlsConnectionConnectCfmSend(sockInst->conn->session->qid,
                sockInst->conn, CSR_RESULT_FAILURE);
            csrTlsSocketInstFree(instanceData, sockInst);
            break;
        }

        default:
        {
            CSR_LOG_TEXT_WARNING((CsrIpLto, 4,
                                  "SSL connect error %d", res));
            CsrTlsConnectionConnectCfmSend(sockInst->conn->session->qid,
                sockInst->conn, CSR_RESULT_FAILURE);
            csrTlsSocketInstFree(instanceData, sockInst);
            break;
        }
    }
}

/*
 * Some messages are sent directly to the intended task, and some
 * are sent indirectly and forwarded from CsrTlsHandler().  The
 * ones sent indirectly are the ones that are sent from the TLS
 * worker thread.
 */

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

    CsrIpMessageForward(qid, CSR_TLS_PRIM, tlsCfm);
}

void CsrTlsConnectionDisconnectIndSend(CsrSchedQid qid,
    CsrTlsConnection conn)
{
    CsrTlsConnectionDisconnectInd *tlsInd;

    tlsInd = CsrPmemAlloc(sizeof(*tlsInd));
    tlsInd->type = CSR_TLS_CONNECTION_DISCONNECT_IND;
    tlsInd->conn = conn;

    CsrIpMessageForward(qid, CSR_TLS_PRIM, tlsInd);
}

void CsrTlsConnectionDisconnectCfmSend(CsrSchedQid qid,
    CsrTlsConnection conn, CsrResult result)
{
    CsrTlsConnectionDisconnectCfm *tlsCfm;

    tlsCfm = CsrPmemAlloc(sizeof(*tlsCfm));
    tlsCfm->type = CSR_TLS_CONNECTION_DISCONNECT_CFM;
    tlsCfm->conn = conn;
    tlsCfm->result = result;

    CsrIpMessageForward(qid, CSR_TLS_PRIM, tlsCfm);
}

void CsrTlsConnectionDataCfmSend(CsrSchedQid qid,
    CsrTlsConnection conn, CsrResult result)
{
    CsrTlsConnectionDataCfm *tlsCfm;

    tlsCfm = CsrPmemAlloc(sizeof(*tlsCfm));
    tlsCfm->type = CSR_TLS_CONNECTION_DATA_CFM;
    tlsCfm->conn = conn;
    tlsCfm->result = result;

    CsrIpMessageForward(qid, CSR_TLS_PRIM, tlsCfm);
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

    CsrIpMessageForward(qid, CSR_TLS_PRIM, tlsInd);
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

    CsrIpMessageForward(qid, CSR_TLS_PRIM, tlsInd);
}
