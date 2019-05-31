/*****************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <asm/types.h>
#include <net/if_packet.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <openssl/asn1.h>

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_macro.h"
#include "csr_pmem.h"
#include "csr_result.h"
#include "csr_sched.h"

#include "csr_tls_handler.h"
#include "csr_tls_sef_tls.h"
#include "csr_tls_thread.h"
#include "csr_tls_task.h"
#include "csr_tls_util.h"

#include "csr_log.h"
#include "csr_log_text_2.h"

#define ENABLE_NONBLOCKING_SOCKETS
/* #define ENABLE_NONBLOCKING_SOCKETS_WORKAROUND */

static in_addr_t ipToNetwork(CsrUint8 *ip)
{
    return (ip[0] << 24) +
           (ip[1] << 16) +
           (ip[2] << 8) +
           (ip[3]);
}

static CsrInt32 getNewSocketHandle(CsrTlsThreadInstanceData *instanceData, void *sockInst)
{
    CsrInt32 i;

    for (i = 0; i < CSR_TLS_MAX_SOCKETS; i++)
    {
        if (instanceData->socketInstance[i] == NULL)
        {
            instanceData->socketInstance[i] = sockInst;
            instanceData->maxSocketHandle = CSRMAX(i,
                instanceData->maxSocketHandle);

            return i;
        }
    }
    return -1;
}

static void returnSocketHandle(CsrTlsThreadInstanceData *instanceData, CsrInt32 sockHandle)
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

static CsrTlsSocketInst *getSocketInst(CsrTlsThreadInstanceData *instanceData, CsrInt32 sockHandle)
{
    if ((sockHandle >= 0) && (sockHandle < CSR_TLS_MAX_SOCKETS))
    {
        return instanceData->socketInstance[sockHandle];
    }

    return NULL;
}

static void maxFdSet(CsrTlsThreadInstanceData *instanceData)
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

    instanceData->maxfd = CSRMAX(instanceData->addSocketPipe[0],
        instanceData->closeThreadPipe[0]);
    instanceData->maxfd = CSRMAX(instanceData->maxfd, fd);
}

static void addDescSet(CsrTlsThreadInstanceData *instanceData, int fd, fd_set *fdset)
{
    FD_SET(fd, fdset);
    if (fd > instanceData->maxfd)
    {
        instanceData->maxfd = fd;
    }
}

static void addSocket(CsrTlsThreadInstanceData *instanceData, int fd, fd_set *rset, fd_set *wset)
{
    const char reap = 0;

    if (rset)
    {
        CSR_LOG_TEXT_INFO((CsrTlsLto, 0, "fd %d added to readset", fd));
        addDescSet(instanceData, fd, rset);
    }
    if (wset)
    {
        CSR_LOG_TEXT_INFO((CsrTlsLto, 0, "fd %d added to writeset", fd));
        addDescSet(instanceData, fd, wset);
    }

    if (write(instanceData->addSocketPipe[1], &reap, 1) != 1)
    {
        CSR_LOG_TEXT_WARNING((CsrTlsLto, 0, "Failed to write 0 to addSocketPipe"));
    }
}

static void removeDescSet(CsrTlsThreadInstanceData *instanceData, int fd, fd_set *fdset)
{
    FD_CLR(fd, fdset);
}

static void removeSocket(CsrTlsThreadInstanceData *instanceData, int fd, fd_set *rset, fd_set *wset)
{
    const char reap = 1;

    if (rset)
    {
        CSR_LOG_TEXT_INFO((CsrTlsLto, 0, "fd %d removed from readset", fd));
        removeDescSet(instanceData, fd, rset);
    }
    if (wset)
    {
        CSR_LOG_TEXT_INFO((CsrTlsLto, 0, "fd %d removed from writeset", fd));
        removeDescSet(instanceData, fd, wset);
    }

    if (write(instanceData->addSocketPipe[1], &reap, 1) != 1)
    {
        CSR_LOG_TEXT_WARNING((CsrTlsLto, 0, "Failed to write 1 to addSocketPipe"));
    }
}

static void socketInstFree(CsrTlsThreadInstanceData *instanceData, CsrTlsSocketInst *sockInst)
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
    connectionDestroyReqHandler(sockInst->conn);
    if (sockInst->ssl)
    {
        SSL_free(sockInst->ssl);
    }

    returnSocketHandle(instanceData, sockInst->handle);
    maxFdSet(instanceData);
    removeSocket(instanceData, sockInst->socket, &instanceData->rsetAll, &instanceData->wsetAll);

    close(sockInst->socket);

    CsrPmemFree(sockInst->serverName);
    CsrPmemFree(sockInst->data);
    CsrPmemFree(sockInst);
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

    CSR_LOG_TEXT_INFO((CsrTlsLto, 0, "%s: %d: %d", str, where, SSL_get_error(s, ret)));
}
#endif

static CsrTlsSocketInst *currentSockInst;

/* These should come from the CSR_TLS_CONFIG_CERT_VERIFY signal*/

static int sslCertVerifyCallback(int preverify_ok, X509_STORE_CTX *ctx)
{
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
        CSR_LOG_TEXT_WARNING((CsrTlsLto, 0, "Could not allocate BIO mem buffer"));
        BIO_free(bp);
        return 0;
    }

    if (!PEM_write_bio_X509(bp, x509Cert))
    {
        CSR_LOG_TEXT_WARNING((CsrTlsLto, 0, "Could not extract X509 certificate"));
        BIO_free(bp);
        return 0;
    }

    certLen = BIO_get_mem_data(bp, &cert);
    (void) BIO_set_close(bp, BIO_NOCLOSE);
    BIO_free(bp);

    CSR_LOG_TEXT_BUFFER_INFO((CsrTlsLto, 0, certLen, cert, "Server certificate verify callback, preverify: %u, err: %d", preverify_ok, err));

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
            (currentSockInst->serverName != NULL))
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
                (strcmp(currentSockInst->serverName, commonName) != 0))
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

    CsrTlsConnectionCertIndSend(currentSockInst->conn->session->qid,
        currentSockInst->conn,
        certLen,
        cert,
        returnValue);

    /* Unlock allowing the application to respond */
    pthread_mutex_unlock(&currentSockInst->threadInstanceData->instLock);
    res = CsrEventWait(&currentSockInst->callbackEvent, CSR_EVENT_WAIT_INFINITE, &eventBits);
    pthread_mutex_lock(&currentSockInst->threadInstanceData->instLock);
    CSR_TLS_ASSERT(res == CSR_RESULT_SUCCESS, "could not wait for event");
    CSR_TLS_ASSERT(eventBits == 0x1, "events bits not set");

    return currentSockInst->acceptCert;
}

static void createSSL(CsrTlsThreadInstanceData *instanceData,
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

static void connectSSL(CsrTlsThreadInstanceData *instanceData,
    CsrTlsSocketInst *sockInst)
{
    int res;

    removeSocket(instanceData, sockInst->socket, &instanceData->rsetAll,
        &instanceData->wsetAll);

#ifdef ENABLE_NONBLOCKING_SOCKETS_WORKAROUND
    res = SSL_ERROR_NONE;
#else
    res = SSL_connect(sockInst->ssl);
    res = SSL_get_error(sockInst->ssl, res);
#endif

    switch (res)
    {
        case SSL_ERROR_NONE:
        {
            CSR_LOG_TEXT_INFO((CsrTlsLto, 0,
                               "SSL connected on socket %d", sockInst->socket));
            sockInst->type = CSR_TLS_TYPE_TCP_DATA;
            addSocket(instanceData, sockInst->socket, &instanceData->rsetAll,
                NULL);
            CsrTlsConnectionConnectCfmSend(sockInst->conn->session->qid,
                sockInst->conn, CSR_RESULT_SUCCESS);
            break;
        }

        case SSL_ERROR_WANT_WRITE:
        {
            addSocket(instanceData, sockInst->socket, NULL, &instanceData->wsetAll);
            break;
        }

        case SSL_ERROR_WANT_READ:
        {
            addSocket(instanceData, sockInst->socket, &instanceData->rsetAll, NULL);
            break;
        }

        case SSL_ERROR_SSL:
        {
#ifdef CSR_LOG_ENABLE
            unsigned long error = ERR_get_error();
#endif
            CSR_LOG_TEXT_WARNING((CsrTlsLto, 0,
                                  "SSL connect %s", ERR_error_string(error, NULL)));
            CsrTlsConnectionConnectCfmSend(sockInst->conn->session->qid,
                sockInst->conn, CSR_RESULT_FAILURE);
            socketInstFree(instanceData, sockInst);
            break;
        }

        default:
        {
            CSR_LOG_TEXT_WARNING((CsrTlsLto, 0,
                                  "SSL connect error %d", res));
            CsrTlsConnectionConnectCfmSend(sockInst->conn->session->qid,
                sockInst->conn, CSR_RESULT_FAILURE);
            socketInstFree(instanceData, sockInst);
            break;
        }
    }
}

static void connectSocket(CsrTlsThreadInstanceData *instanceData,
    CsrTlsSocketInst *sockInst)
{
    int error;
    socklen_t len;

    removeSocket(instanceData, sockInst->socket, NULL, &instanceData->wsetAll);
    len = sizeof(error);
    if ((getsockopt(sockInst->socket, SOL_SOCKET, SO_ERROR, &error, &len) == 0) && (error == 0))
    {
        CSR_LOG_TEXT_INFO((CsrTlsLto, 0,
                           "socket %d connected", sockInst->socket));
        sockInst->type = CSR_TLS_TYPE_TCP_SSL_CONNECT;
        createSSL(instanceData, sockInst);
        connectSSL(instanceData, sockInst);
    }
    else
    {
        CsrTlsConnectionConnectCfmSend(sockInst->conn->session->qid,
            sockInst->conn, CSR_RESULT_FAILURE);
        socketInstFree(instanceData, sockInst);
    }
}

static void readSSL(CsrTlsThreadInstanceData *instanceData, CsrTlsSocketInst *sockInst)
{
    CsrBool readBlockedOnRead;

    do
    {
        int res;

        sockInst->readBlockedOnWrite = FALSE;
        readBlockedOnRead = FALSE;

        res = SSL_read(sockInst->ssl, instanceData->rxBuffer, instanceData->rxBufferSize);
        switch (SSL_get_error(sockInst->ssl, res))
        {
            case SSL_ERROR_NONE:
            {
                csrTlsRxElement *rxelem;

                rxelem = CsrPmemAlloc(sizeof(*rxelem));

                rxelem->buf = CsrMemDup(instanceData->rxBuffer, res);
                rxelem->bufLen = res;
                rxelem->next = NULL;

                if (sockInst->rxQueue == NULL)
                {
                    sockInst->rxQueue = rxelem;
                    CsrTlsConnectionDataIndSend(sockInst->conn->session->qid,
                        sockInst->conn,
                        rxelem->bufLen,
                        rxelem->buf);
                }
                else
                {
                    /* Queue indication for later. */

                    csrTlsRxElement *rxprev;

                    for (rxprev = sockInst->rxQueue;
                         rxprev->next != NULL;
                         rxprev = rxprev->next)
                    {
                        /* nothing -- simply iterate to find last element */
                    }

                    rxprev->next = rxelem;
                }
                break;
            }

            case SSL_ERROR_ZERO_RETURN:
            {
                /* The TLS/SSL connection has been closed. If the protocol version is
                    SSL 3.0 or TLS 1.0, this result code is returned only if a
                    closure alert has occurred in the protocol, i.e. if the connection has
                    been closed cleanly. Note that in this case SSL_ERROR_ZERO_RETURN does not
                    necessarily indicate that the underlying transport has been closed. */

                /* peer has performed shutdown */
                if (!sockInst->dead)
                {
                    if (sockInst->rxQueue == NULL)
                    {
                        CsrTlsConnectionDisconnectIndSend(sockInst->conn->session->qid, sockInst->conn);
                    }

                    removeSocket(instanceData, sockInst->socket, &instanceData->rsetAll, &instanceData->wsetAll);
                    sockInst->dead = TRUE;
                }
                break;
            }

            case SSL_ERROR_WANT_READ:
            {
                readBlockedOnRead = TRUE;
                break;
            }

            /* We get a WANT_WRITE if we're trying to rehandshake and we block on
                a write during that rehandshake. We need to wait on the socket to be
                writeable but reinitiate the read when it is */
            case SSL_ERROR_WANT_WRITE:
            {
                sockInst->readBlockedOnWrite = TRUE;
                break;
            }

            default:
            {
                CSR_LOG_TEXT_WARNING((CsrTlsLto, 0, "RX: Could not recv data"));
                break;
            }
        }

        /* We need a check for readBlockedOnRead here because
            SSL_pending() doesn't work properly during the
            handshake. This check prevents a busy-wait
            loop around SSL_read() */
    } while (SSL_pending(sockInst->ssl) && !readBlockedOnRead);
}

static void writeSSL(CsrTlsThreadInstanceData *instanceData, CsrTlsSocketInst *sockInst)
{
    int res;
    int error;

    sockInst->writeBlockedOnRead = FALSE;

    /* Try to write */
    res = SSL_write(sockInst->ssl, &sockInst->data[sockInst->dataOffset], sockInst->dataLen);
    error = SSL_get_error(sockInst->ssl, res);
    switch (error)
    {
        /* "SSL_write() will only return with success, when the complete
            contents of buf of length num has been written." */
        case SSL_ERROR_NONE:
        {
            if ((res > 0) && ((size_t) res == sockInst->dataLen))
            {
                CsrTlsConnectionDataCfmSend(sockInst->conn->session->qid, sockInst->conn, CSR_RESULT_SUCCESS);
                removeSocket(instanceData, sockInst->socket, NULL, &instanceData->wsetAll);
                CsrPmemFree(sockInst->data);
                sockInst->data = NULL;
            }
            else
            {
                CSR_TLS_ASSERT(res > 0 && (size_t) res < sockInst->dataLen, "unrecognized result code");
                sockInst->dataOffset += res;
                sockInst->dataLen -= res;
            }
            break;
        }

        /* We would have blocked. We need to call SSL_write() again with the
           _same_ set of paramters */
        case SSL_ERROR_WANT_WRITE:
        {
            break;
        }

        /* We get a WANT_READ if we're trying to rehandshake and we block on
           write during the current connection. We need to wait on the socket to
           be readable but reinitiate our write when it is */
        case SSL_ERROR_WANT_READ:
        {
            sockInst->writeBlockedOnRead = TRUE;
            break;
        }

        /* Some other error */
        default:
        {
            CSR_LOG_TEXT_WARNING((CsrTlsLto, 0, "TX: Could not send data, error %d", error));
            CsrTlsConnectionDataCfmSend(sockInst->conn->session->qid, sockInst->conn, CSR_RESULT_FAILURE);
            removeSocket(instanceData, sockInst->socket, NULL, &instanceData->wsetAll);
            CsrPmemFree(sockInst->data);
            sockInst->data = NULL;
            break;
        }
    }
}

static void handleSockets(CsrTlsThreadInstanceData *instanceData, fd_set *rset, fd_set *wset)
{
    CsrTlsSocketInst *sockInst;
    int sockno;

    for (sockno = 0; sockno <= instanceData->maxSocketHandle; sockno++)
    {
        if ((sockInst = getSocketInst(instanceData, sockno)) == NULL)
        {
            continue;
        }
        CSR_LOG_TEXT_DEBUG((CsrTlsLto, 0,
                            "socket %d, readset: %u, writeset: %u",
                            sockInst->socket,
                            FD_ISSET(sockInst->socket, rset),
                            FD_ISSET(sockInst->socket, wset)));

        CSR_LOG_TEXT_DEBUG((CsrTlsLto, 0, "DEBUG: SocketInstance Handler:%d", sockInst));
        currentSockInst = sockInst;

        switch (sockInst->type)
        {
            case CSR_TLS_TYPE_TCP_DATA:
            {
                if ((FD_ISSET(sockInst->socket, rset) && !sockInst->writeBlockedOnRead) ||
                    (FD_ISSET(sockInst->socket, wset) && sockInst->readBlockedOnWrite))
                {
                    readSSL(instanceData, sockInst);
                }

                if ((FD_ISSET(sockInst->socket, wset) && !sockInst->readBlockedOnWrite && sockInst->dataLen) ||
                    (FD_ISSET(sockInst->socket, rset) && sockInst->writeBlockedOnRead))
                {
                    writeSSL(instanceData, sockInst);
                }
                break;
            }

            case CSR_TLS_TYPE_TCP_CONNECT:
            {
                if (FD_ISSET(sockInst->socket, wset) || FD_ISSET(sockInst->socket, rset))
                {
                    connectSocket(instanceData, sockInst);
                }
                break;
            }

            case CSR_TLS_TYPE_TCP_SSL_CONNECT:
            {
                if (FD_ISSET(sockInst->socket, wset) || FD_ISSET(sockInst->socket, rset))
                {
                    connectSSL(instanceData, sockInst);
                }
                break;
            }

            default:
            {
                CSR_LOG_TEXT_INFO((CsrTlsLto, 0, "socket thread unhandled socket type: %d", sockInst->type));
                break;
            }
        }
    }
}

static void *socketThreadFunction(void *arg)
{
    CsrTlsThreadInstanceData *instanceData = (CsrTlsThreadInstanceData *) arg;

    CSR_LOG_TEXT_INFO((CsrTlsLto, 0, "socket thread started"));

    while (1)
    {
        int ready, maxfd;
        fd_set rset;
        fd_set wset;

        pthread_mutex_lock(&instanceData->instLock);
        rset = instanceData->rsetAll;
        wset = instanceData->wsetAll;
        maxfd = instanceData->maxfd;
        pthread_mutex_unlock(&instanceData->instLock);

        ready = select(maxfd + 1, &rset, &wset, NULL, NULL);
        if (ready == -1)
        {
            if (errno == EINTR)
            {
                /* Interrupted by a signal. */
                continue;
            }
            else if (errno == ENOMEM)
            {
                /* Temporary out of memory error. */
                continue;
            }
            else
            {
                CSR_LOG_TEXT_ERROR((CsrTlsLto, 0,
                                    "select() error, errno = %d", errno));
                break;
            }
        }

        if (FD_ISSET(instanceData->closeThreadPipe[0], &rset))
        {
            /* exit thread */
            CSR_LOG_TEXT_INFO((CsrTlsLto, 0, "socket thread stopped"));
            break;
        }

        if (FD_ISSET(instanceData->addSocketPipe[0], &rset))
        {
            /* reschedule select() */
            char reap;

            if (read(instanceData->addSocketPipe[0], &reap, 1) != 1)
            {
                CSR_LOG_TEXT_WARNING((CsrTlsLto, 0, "Failed to read from addSocketPipe"));
            }

            if (reap)
            {
                int sockno;

                for (sockno = 0;
                     sockno <= instanceData->maxSocketHandle;
                     sockno++)
                {
                    CsrTlsSocketInst *sockInst;

                    sockInst = getSocketInst(instanceData, sockno);

                    if ((sockInst != NULL) && (sockInst->reap))
                    {
                        CSR_LOG_TEXT_DEBUG((CsrTlsLto, 0,
                                            "reaping socket %d", sockInst->socket));

                        CsrTlsConnectionDisconnectCfmSend(sockInst->conn->session->qid,
                            sockInst->conn, CSR_RESULT_SUCCESS);
                        socketInstFree(instanceData, sockInst);
                    }
                }
            }
            else
            {
                CSR_LOG_TEXT_DEBUG((CsrTlsLto, 0, "socket thread rescheduled"));
            }

            continue;
        }

        pthread_mutex_lock(&instanceData->instLock);
        handleSockets(instanceData, &rset, &wset);
        pthread_mutex_unlock(&instanceData->instLock);
    }

    return NULL;
}

void CsrTlsThreadInit(CsrTlsThreadInstanceData *instanceData, CsrTlsInstanceData *taskInstanceData)
{
    int rc;

    SSL_library_init();
    SSL_load_error_strings();

    instanceData->taskInstanceData = taskInstanceData;
    instanceData->maxSocketHandle = -1;
    instanceData->rxBufferSize = RX_BUFFER_SIZE;
    instanceData->rxBuffer = CsrPmemAlloc(instanceData->rxBufferSize * sizeof(CsrUint8));

    rc = pipe(instanceData->addSocketPipe);
    CSR_TLS_ASSERT(rc == 0, "failed creating add connection pipe");

    rc = pipe(instanceData->closeThreadPipe);
    CSR_TLS_ASSERT(rc == 0, "failed creating close thread pipe");

    FD_ZERO(&instanceData->wsetAll);
    FD_ZERO(&instanceData->rsetAll);

    FD_SET(instanceData->addSocketPipe[0], &instanceData->rsetAll);
    FD_SET(instanceData->closeThreadPipe[0], &instanceData->rsetAll);

    instanceData->maxfd = CSRMAX(instanceData->addSocketPipe[0], instanceData->closeThreadPipe[0]);

    rc = pthread_mutex_init(&instanceData->instLock, NULL);
    CSR_TLS_ASSERT(rc == 0, "failed creating instance lock");

    rc = pthread_create(&instanceData->tid, NULL, socketThreadFunction, (void *) instanceData);
    CSR_TLS_ASSERT(rc == 0, "failed creating ip thread");
}

void CsrTlsThreadDeinit(CsrTlsThreadInstanceData *instanceData)
{
    CsrUint32 i;
    int rc;

    if (write(instanceData->closeThreadPipe[1], "1", 1) != 1)
    {
        CSR_LOG_TEXT_WARNING((CsrTlsLto, 0, "Failed to write 1 to closeTreadPipe"));
    }

    for (i = 0; i < CSR_TLS_MAX_SOCKETS; i++)
    {
        if (instanceData->socketInstance[i] != NULL)
        {
            CsrTlsSocketInst *sockInst;

            sockInst = instanceData->socketInstance[i];

            rc = CsrEventSet(&sockInst->callbackEvent, 0x1);
            CSR_TLS_ASSERT(rc == CSR_RESULT_SUCCESS, "could not set event");
        }
    }

    pthread_join(instanceData->tid, NULL);

    pthread_mutex_destroy(&instanceData->instLock);

    /* Close routing sockets. */
    rc = close(instanceData->closeThreadPipe[0]);
    CSR_TLS_ASSERT(rc == 0, "close closeThreadPipe[0]");

    rc = close(instanceData->closeThreadPipe[1]);
    CSR_TLS_ASSERT(rc == 0, "close closeThreadPipe[1]");

    rc = close(instanceData->addSocketPipe[0]);
    CSR_TLS_ASSERT(rc == 0, "close addSocketPipe[0]");

    rc = close(instanceData->addSocketPipe[1]);
    CSR_TLS_ASSERT(rc == 0, "close addSocketPipe[1]");

    for (i = 0; i < CSR_TLS_MAX_SOCKETS; i++)
    {
        if (instanceData->socketInstance[i] != NULL)
        {
            CsrTlsSocketInst *sockInst;

            sockInst = instanceData->socketInstance[i];

            socketInstFree(instanceData, sockInst);
        }
    }

    /* Deallocate instance data */
    CsrPmemFree(instanceData->rxBuffer);
}

void CsrTlsSessionCreateHandler(CsrTlsInstanceData *taskInstanceData)
{
    CsrTlsThreadInstanceData *instanceData = &taskInstanceData->threadInstanceData;
    CsrTlsSessionCreateReq *msg = (CsrTlsSessionCreateReq *) taskInstanceData->msg;
    csrTlsSession *sess;

    pthread_mutex_lock(&instanceData->instLock);
    sess = sessionCreateHandler(taskInstanceData, msg->qid);
    if (sess != NULL)
    {
        CsrTlsSessionCreateCfmSend(msg->qid, sess, CSR_RESULT_SUCCESS);
    }
    else
    {
        CsrTlsSessionCreateCfmSend(msg->qid, NULL, CSR_RESULT_FAILURE);
    }
    pthread_mutex_unlock(&instanceData->instLock);
}

void CsrTlsSessionDestroyHandler(CsrTlsInstanceData *taskInstanceData)
{
    CsrTlsThreadInstanceData *instanceData = &taskInstanceData->threadInstanceData;
    CsrTlsSessionDestroyReq *msg = (CsrTlsSessionDestroyReq *) taskInstanceData->msg;
    csrTlsSession *sess = (csrTlsSession *) msg->session;

    pthread_mutex_lock(&instanceData->instLock);
    if (sess != NULL)
    {
        CsrTlsSessionDestroyCfmSend(sess->qid, sess, CSR_RESULT_SUCCESS);
        sessionDestroyHandler(taskInstanceData, sess);
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrTlsLto, 0, "Faild to get session"));
    }
    pthread_mutex_unlock(&instanceData->instLock);
}

void CsrTlsConnectionCreateReqHandler(CsrTlsInstanceData *taskInstanceData)
{
    CsrTlsThreadInstanceData *instanceData = &taskInstanceData->threadInstanceData;
    CsrTlsConnectionCreateReq *msg = (CsrTlsConnectionCreateReq *) taskInstanceData->msg;
    csrTlsSession *sess = (csrTlsSession *) msg->session;
    int sock;

    if (msg->socketFamily == CSR_IP_SOCKET_FAMILY_IP4)
    {
        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }
    else
    {
        sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    }

    if (sock != -1)
    {
        int reuseAddr = TRUE;
        int tcpNoDelay = 1;
        CsrTlsSocketInst *sockInst;

        sockInst = CsrPmemZalloc(sizeof(CsrTlsSocketInst));
        sockInst->socket = sock;
        sockInst->handle = CSR_TLS_MAX_SOCKETS;

        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
                (char *) &reuseAddr, sizeof(reuseAddr)) != 0)
        {
            CSR_LOG_TEXT_WARNING((CsrTlsLto, 0,
                                  "setsockopt failed, errno=%d", errno));
        }

        if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
                (char *) &tcpNoDelay, sizeof(tcpNoDelay)) == 0)
        {
            csrTlsConnection *conn;
            CsrInt32 sockHandle;

            pthread_mutex_lock(&instanceData->instLock);
            conn = connectionCreateReqHandler(sess);
            sockHandle = getNewSocketHandle(instanceData, sockInst);
            if ((sockHandle != -1) && (conn != NULL))
            {
                int res;

                sockInst->handle = (CsrUint16) sockHandle;
                sockInst->family = msg->socketFamily;
                sockInst->dead = FALSE;
                sockInst->reap = FALSE;
                sockInst->serverName = msg->serverName;
                sockInst->rxQueue = NULL;

                res = CsrEventCreate(&sockInst->callbackEvent);
                CSR_TLS_ASSERT(res == CSR_RESULT_SUCCESS,
                    "could not create event");

                sockInst->threadInstanceData = instanceData;
                sockInst->ssl = NULL;
                sockInst->readBlockedOnWrite = FALSE;
                sockInst->writeBlockedOnRead = FALSE;
                sockInst->conn = conn;
                conn->socketHandle = sockInst->handle;

                CsrTlsConnectionCreateCfmSend(sess->qid, sess,
                    CSR_RESULT_SUCCESS, conn);
            }
            else
            {
                CsrTlsConnectionCreateCfmSend(sess->qid, sess,
                    CSR_TLS_RESULT_NO_MORE_SOCKETS, NULL);
                CSR_LOG_TEXT_WARNING((CsrTlsLto, 0,
                                      "getNewSocketHandle failed"));
                socketInstFree(instanceData, sockInst);
            }
            pthread_mutex_unlock(&instanceData->instLock);
        }
        else
        {
            CsrTlsConnectionCreateCfmSend(sess->qid, sess,
                CSR_RESULT_FAILURE, NULL);
            CSR_LOG_TEXT_WARNING((CsrTlsLto, 0,
                                  "setsockopt failed, errno=%d", errno));
            socketInstFree(instanceData, sockInst);
        }
    }
    else
    {
        CsrTlsConnectionCreateCfmSend(sess->qid, sess,
            CSR_RESULT_FAILURE, NULL);
    }
}

void CsrTlsConnectionBindReqHandler(CsrTlsInstanceData *taskInstanceData)
{
    CsrTlsThreadInstanceData *instanceData = &taskInstanceData->threadInstanceData;
    CsrTlsConnectionBindReq *msg = (CsrTlsConnectionBindReq *) taskInstanceData->msg;
    csrTlsConnection *conn = (csrTlsConnection *) msg->conn;
    CsrTlsSocketInst *sockInst;

    pthread_mutex_lock(&instanceData->instLock);
    sockInst = getSocketInst(instanceData, conn->socketHandle);
    if (sockInst != NULL)
    {
        int res;
        struct sockaddr_storage sa;

        if (sockInst->family == CSR_IP_SOCKET_FAMILY_IP4)
        {
            struct sockaddr_in *sa4;

            sa4 = (struct sockaddr_in *) &sa;

            CsrMemSet(sa4, 0, sizeof(*sa4));
            sa4->sin_family = AF_INET;
            sa4->sin_port = htons(msg->port);
            sa4->sin_addr.s_addr =
                htonl(ipToNetwork(msg->ipAddress));
        }
        else
        {
            struct sockaddr_in6 *sa6;

            sa6 = (struct sockaddr_in6 *) &sa;

            CsrMemSet(sa6, 0, sizeof(*sa6));
            sa6->sin6_family = AF_INET6;
            sa6->sin6_port = htons(msg->port);
            CsrMemCpy(&sa6->sin6_addr, msg->ipAddress, 16);
        }

        res = bind(sockInst->socket, (struct sockaddr *) &sa,
            sizeof(sa));
        if (res != -1)
        {
            CsrTlsConnectionBindCfmSend(conn->session->qid, conn,
                CSR_RESULT_SUCCESS);
        }
        else
        {
            if (errno == EADDRINUSE)
            {
                CsrTlsConnectionBindCfmSend(conn->session->qid, conn,
                    CSR_TLS_RESULT_PORT_IN_USE);
            }
            else
            {
                CsrTlsConnectionBindCfmSend(conn->session->qid, conn,
                    CSR_RESULT_FAILURE);
            }
        }
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrTlsLto, 0,
                              "failed getting socket inst"));
    }
    pthread_mutex_unlock(&instanceData->instLock);
}

void CsrTlsConnectionDestroyReqHandler(CsrTlsInstanceData *taskInstanceData)
{
    CsrTlsThreadInstanceData *instanceData = &taskInstanceData->threadInstanceData;
    CsrTlsConnectionDestroyReq *msg = (CsrTlsConnectionDestroyReq *) taskInstanceData->msg;
    csrTlsConnection *conn = (csrTlsConnection *) msg->conn;
    CsrTlsSocketInst *sockInst;

    pthread_mutex_lock(&instanceData->instLock);
    sockInst = getSocketInst(instanceData, conn->socketHandle);
    if (sockInst != NULL)
    {
        CsrTlsConnectionDestroyCfmSend(conn->session->qid, conn);
        socketInstFree(instanceData, sockInst);
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrTlsLto, 0,
                              "failed getting socket inst"));
    }
    pthread_mutex_unlock(&instanceData->instLock);
}

void CsrTlsConnectionConnectReqHandler(CsrTlsInstanceData *taskInstanceData)
{
    CsrTlsThreadInstanceData *instanceData = &taskInstanceData->threadInstanceData;
    CsrTlsConnectionConnectReq *msg = (CsrTlsConnectionConnectReq *) taskInstanceData->msg;
    csrTlsConnection *conn = (csrTlsConnection *) msg->conn;
    CsrTlsSocketInst *sockInst;

    pthread_mutex_lock(&instanceData->instLock);
    sockInst = getSocketInst(instanceData, conn->socketHandle);
    if (sockInst != NULL)
    {
#ifdef ENABLE_NONBLOCKING_SOCKETS
        int fdFlags;
#endif
        int res;
        struct sockaddr_storage sa;

#ifdef ENABLE_NONBLOCKING_SOCKETS
        /* Set socket as nonblocking */
        fdFlags = fcntl(sockInst->socket, F_GETFL, 0);
        fcntl(sockInst->socket, F_SETFL, fdFlags | O_NONBLOCK);
#endif

        if (sockInst->family == CSR_IP_SOCKET_FAMILY_IP4)
        {
            struct sockaddr_in *sa4;

            sa4 = (struct sockaddr_in *) &sa;

            CsrMemSet(sa4, 0, sizeof(*sa4));
            sa4->sin_family = AF_INET;
            sa4->sin_port = htons(msg->port);
            sa4->sin_addr.s_addr =
                htonl(ipToNetwork(msg->ipAddress));
        }
        else
        {
            struct sockaddr_in6 *sa6;

            sa6 = (struct sockaddr_in6 *) &sa;

            CsrMemSet(sa6, 0, sizeof(*sa6));
            sa6->sin6_family = AF_INET6;
            sa6->sin6_port = htons(msg->port);
            CsrMemCpy(&sa6->sin6_addr, msg->ipAddress, 16);
        }

        res = connect(sockInst->socket, (struct sockaddr *) &sa,
            sizeof(sa));
        if (res == 0)
        {
            CSR_LOG_TEXT_INFO((CsrTlsLto, 0,
                               "socket %d connected", sockInst->socket));
            sockInst->type = CSR_TLS_TYPE_TCP_SSL_CONNECT;
            createSSL(instanceData, sockInst);
            connectSSL(instanceData, sockInst);
        }
        else if (errno == EINPROGRESS)
        {
            sockInst->type = CSR_TLS_TYPE_TCP_CONNECT;
            addSocket(instanceData, sockInst->socket,
                &instanceData->rsetAll, &instanceData->wsetAll);
        }
        else
        {
            CsrTlsConnectionConnectCfmSend(conn->session->qid, conn,
                CSR_RESULT_FAILURE);
            socketInstFree(instanceData, sockInst);
        }
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrTlsLto, 0,
                              "failed getting socket inst"));
    }
    pthread_mutex_unlock(&instanceData->instLock);
}

void CsrTlsConnectionDataReqHandler(CsrTlsInstanceData *taskInstanceData)
{
    CsrTlsThreadInstanceData *instanceData = &taskInstanceData->threadInstanceData;
    CsrTlsConnectionDataReq *msg = (CsrTlsConnectionDataReq *) taskInstanceData->msg;
    csrTlsConnection *conn = (csrTlsConnection *) msg->conn;
    CsrTlsSocketInst *sockInst;

    pthread_mutex_lock(&instanceData->instLock);
    sockInst = getSocketInst(instanceData, conn->socketHandle);
    if ((sockInst != NULL) && !sockInst->dead)
    {
        sockInst->data = msg->buf;
        sockInst->dataOffset = 0;
        sockInst->dataLen = msg->bufLen;
        addSocket(instanceData, sockInst->socket, NULL, &instanceData->wsetAll);
    }
    else
    {
        CsrPmemFree(msg->buf);
        msg->buf = NULL;
    }
    pthread_mutex_unlock(&instanceData->instLock);
}

void CsrTlsConnectionDataResHandler(CsrTlsInstanceData *taskInstanceData)
{
    CsrTlsThreadInstanceData *instanceData = &taskInstanceData->threadInstanceData;
    CsrTlsConnectionDataRes *msg = (CsrTlsConnectionDataRes *) taskInstanceData->msg;
    csrTlsConnection *conn = (csrTlsConnection *) msg->conn;
    CsrTlsSocketInst *sockInst;
    csrTlsRxElement *rxelem;

    pthread_mutex_lock(&instanceData->instLock);
    sockInst = getSocketInst(instanceData, conn->socketHandle);
    if (sockInst != NULL)
    {
        rxelem = sockInst->rxQueue;
        sockInst->rxQueue = rxelem->next;

        if (sockInst->rxQueue != NULL)
        {
            /* More data to push up */
            CsrTlsConnectionDataIndSend(conn->session->qid, conn,
                sockInst->rxQueue->bufLen,
                sockInst->rxQueue->buf);
        }
        else
        {
            if (sockInst->dead)
            {
                /* The socket is dead, now is the time to inform the app. */
                CsrTlsConnectionDisconnectIndSend(conn->session->qid, conn);
            }
        }

        CsrPmemFree(rxelem);
    }
    pthread_mutex_unlock(&instanceData->instLock);
}

void CsrTlsConnectionDisconnectReqHandler(CsrTlsInstanceData *taskInstanceData)
{
    CsrTlsThreadInstanceData *instanceData = &taskInstanceData->threadInstanceData;
    CsrTlsConnectionDisconnectReq *msg = (CsrTlsConnectionDisconnectReq *) taskInstanceData->msg;
    csrTlsConnection *conn = (csrTlsConnection *) msg->conn;
    CsrTlsSocketInst *sockInst;

    pthread_mutex_lock(&instanceData->instLock);
    sockInst = getSocketInst(instanceData, conn->socketHandle);
    if (sockInst != NULL)
    {
        sockInst->reap = TRUE;
        removeSocket(instanceData, sockInst->socket, &instanceData->rsetAll,
            &instanceData->wsetAll);
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrTlsLto, 0, "failed getting socket inst"));
    }
    pthread_mutex_unlock(&instanceData->instLock);
}

void CsrTlsConnectionDisconnectResHandler(CsrTlsInstanceData *taskInstanceData)
{
    CsrTlsThreadInstanceData *instanceData = &taskInstanceData->threadInstanceData;
    CsrTlsConnectionDisconnectRes *msg = (CsrTlsConnectionDisconnectRes *) taskInstanceData->msg;
    csrTlsConnection *conn = (csrTlsConnection *) msg->conn;
    CsrTlsSocketInst *sockInst;

    pthread_mutex_lock(&instanceData->instLock);
    sockInst = getSocketInst(instanceData, conn->socketHandle);
    if (sockInst != NULL)
    {
        socketInstFree(instanceData, sockInst);
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrTlsLto, 0, "failed getting socket inst"));
    }
    pthread_mutex_unlock(&instanceData->instLock);
}

void CsrTlsConnectionCertResHandler(CsrTlsInstanceData *taskInstanceData)
{
    CsrTlsThreadInstanceData *instanceData = &taskInstanceData->threadInstanceData;
    CsrTlsConnectionCertRes *msg = (CsrTlsConnectionCertRes *) taskInstanceData->msg;
    csrTlsConnection *conn = (csrTlsConnection *) msg->conn;
    CsrTlsSocketInst *sockInst;

    pthread_mutex_lock(&instanceData->instLock);
    sockInst = getSocketInst(instanceData, conn->socketHandle);

    CSR_LOG_TEXT_DEBUG((CsrTlsLto, 0, "DEBUG: certRes %d", sockInst));

    if (sockInst != NULL)
    {
        int rc;
        sockInst->acceptCert = msg->accepted;
        rc = CsrEventSet(&sockInst->callbackEvent, 0x1);
        CSR_TLS_ASSERT(rc == CSR_RESULT_SUCCESS, "could not set event");
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrTlsLto, 0, "failed getting socket inst"));
    }
    pthread_mutex_unlock(&instanceData->instLock);
}
