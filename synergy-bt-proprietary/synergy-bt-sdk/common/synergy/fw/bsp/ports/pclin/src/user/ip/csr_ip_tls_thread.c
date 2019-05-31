/*****************************************************************************

Copyright (c) 2011-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"

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

#include "csr_types.h"
#include "csr_macro.h"
#include "csr_pmem.h"
#include "csr_result.h"
#include "csr_sched.h"

#include "csr_tls_lib.h"

#include "csr_ip_tls_handler.h"
#include "csr_ip_tls_sef.h"
#include "csr_ip_tls_thread.h"
#include "csr_ip_tls_util.h"

#include "csr_ip_handler.h"
#include "csr_ip_util.h"

#include "csr_log.h"
#include "csr_log_text_2.h"

/* These should come from the CSR_TLS_CONFIG_CERT_VERIFY signal*/

static void connectSocket(CsrIpTlsInstanceData *instanceData,
    CsrTlsSocketInst *sockInst)
{
    int error;
    socklen_t len;

    CsrIpFdRemove(instanceData->ipInstanceData, sockInst->socket, FALSE, TRUE);
    len = sizeof(error);
    if ((getsockopt(sockInst->socket, SOL_SOCKET, SO_ERROR, &error, &len) == 0) && (error == 0))
    {
        CSR_LOG_TEXT_INFO((CsrIpLto, 4,
                           "socket %d connected", sockInst->socket));
        sockInst->type = CSR_TLS_TYPE_TCP_SSL_CONNECT;
        csrIpTlsCreateSsl(instanceData, sockInst);
        csrIpTlsConnectSsl(instanceData, sockInst);
    }
    else
    {
        CsrTlsConnectionConnectCfmSend(sockInst->conn->session->qid,
            sockInst->conn, CSR_RESULT_FAILURE);
        csrTlsSocketInstFree(instanceData, sockInst);
    }
}

static void readSSL(CsrIpTlsInstanceData *instanceData, CsrTlsSocketInst *sockInst)
{
    CsrBool readBlockedOnRead;

    do
    {
        int res;

        sockInst->readBlockedOnWrite = FALSE;
        readBlockedOnRead = FALSE;

        res = SSL_read(sockInst->ssl, instanceData->rxBuffer, TLS_RX_BUFFER_SIZE);
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

                    CsrIpFdRemove(instanceData->ipInstanceData, sockInst->socket, TRUE, TRUE);
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
                CSR_LOG_TEXT_WARNING((CsrIpLto, 4, "RX: Could not recv data"));
                break;
            }
        }

        /* We need a check for readBlockedOnRead here because
            SSL_pending() doesn't work properly during the
            handshake. This check prevents a busy-wait
            loop around SSL_read() */
    } while (SSL_pending(sockInst->ssl) && !readBlockedOnRead);
}

static void writeSSL(CsrIpTlsInstanceData *instanceData, CsrTlsSocketInst *sockInst)
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
                CsrIpFdRemove(instanceData->ipInstanceData, sockInst->socket, FALSE, TRUE);
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
            CSR_LOG_TEXT_WARNING((CsrIpLto, 4, "TX: Could not send data, error %d", error));
            CsrTlsConnectionDataCfmSend(sockInst->conn->session->qid, sockInst->conn, CSR_RESULT_FAILURE);
            CsrIpFdRemove(instanceData->ipInstanceData, sockInst->socket, FALSE, TRUE);
            CsrPmemFree(sockInst->data);
            sockInst->data = NULL;
            break;
        }
    }
}

void csrIpTlsSocketsHandle(CsrIpTlsInstanceData *instanceData,
    int *ready, fd_set *rset, fd_set *wset)
{
    CsrTlsSocketInst *sockInst;
    int sockno;
    int readyCount;

    readyCount = *ready;

    for (sockno = 0;
         (readyCount > 0) && (sockno <= instanceData->maxSocketHandle);
         sockno++)
    {
        if ((sockInst = csrIpTlsInstGet(instanceData, sockno)) == NULL)
        {
            continue;
        }
        CSR_LOG_TEXT_DEBUG((CsrIpLto, 4,
                            "socket %d, readset: %u, writeset: %u",
                            sockInst->socket,
                            FD_ISSET(sockInst->socket, rset),
                            FD_ISSET(sockInst->socket, wset)));

        CSR_LOG_TEXT_DEBUG((CsrIpLto, 4, "DEBUG: SocketInstance Handler:%d", sockInst));
        globalCsrIpTlsInstanceData->currentSockInst = sockInst;

        if (FD_ISSET(sockInst->socket, rset))
        {
            readyCount--;
        }

        if (FD_ISSET(sockInst->socket, wset))
        {
            readyCount--;
        }

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
                    csrIpTlsConnectSsl(instanceData, sockInst);
                }
                break;
            }

            default:
            {
                CSR_LOG_TEXT_INFO((CsrIpLto, 4, "socket thread unhandled socket type: %d", sockInst->type));
                break;
            }
        }
    }

    *ready = readyCount;
}

void csrIpTlsSocketsReap(CsrIpTlsInstanceData *instanceData)
{
    int sockno;
    csrTlsSession *sess;

    for (sockno = 0;
         sockno <= instanceData->maxSocketHandle;
         sockno++)
    {
        CsrTlsSocketInst *sockInst;

        sockInst = csrIpTlsInstGet(instanceData, sockno);

        if ((sockInst != NULL) && (sockInst->reap))
        {
            CSR_LOG_TEXT_DEBUG((CsrIpLto, 4, "reaping socket %d", sockInst->socket));

            if ((NULL != sockInst->conn) && (NULL != sockInst->conn->session) && !sockInst->conn->session->reap)
            {
                CsrTlsConnectionDisconnectCfmSend(sockInst->conn->session->qid, sockInst->conn, CSR_RESULT_SUCCESS);
            }
            csrTlsSocketInstFree(instanceData, sockInst);
        }
    }
    sess = instanceData->tlsSessionList;
    while (sess)
    {
        if (sess->reap)
        {
            CSR_LOG_TEXT_DEBUG((CsrIpLto, 0, "reaping session %x", sess));
            CsrTlsSessionDestroyCfmSend(sess->qid, sess, CSR_RESULT_SUCCESS);
            csrIpTlsSessionDestroy(instanceData, sess);
            break;
        }
        sess = sess->next;
    }
}

/*
 * This must be done prior to thread join.
 */
void CsrIpTlsThreadDeinit(CsrIpTlsInstanceData *instanceData)
{
    CsrUint32 i;

    for (i = 0; i < CSR_TLS_MAX_SOCKETS; i++)
    {
        if (instanceData->socketInstance[i] != NULL)
        {
            int rc;
            CsrTlsSocketInst *sockInst;

            sockInst = instanceData->socketInstance[i];

            rc = CsrEventSet(&sockInst->callbackEvent, 0x1);
            CSR_TLS_ASSERT(rc == CSR_RESULT_SUCCESS, "could not set event");
        }
    }
}
