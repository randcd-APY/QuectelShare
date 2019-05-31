/*****************************************************************************

Copyright (c) 2011-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <openssl/ssl.h>

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_prim_defs.h"
#include "csr_result.h"
#include "csr_sched.h"

#include "csr_tls_lib.h"
#include "csr_tls_prim.h"

#include "csr_ip_tls_sef.h"

#include "csr_ip_tls_handler.h"
#include "csr_ip_tls_thread.h"
#include "csr_ip_tls_util.h"

#include "csr_ip_util.h"

#include "csr_log.h"
#include "csr_log_text_2.h"

#define CSR_TLS_SESSIONLIMIT 8
#define CSR_TLS_CONNECTIONLIMIT 8

static void sessionCreateHandler(CsrIpTlsInstanceData *);
void sessionDestroyHandler(CsrIpTlsInstanceData *);
static void configCipherReqHandler(CsrIpTlsInstanceData *);
static void configCertClientReqHandler(CsrIpTlsInstanceData *);
static void configCertVerifyReqHandler(CsrIpTlsInstanceData *);
static void connectionCreateReqHandler(CsrIpTlsInstanceData *);
static void connectionBindReqHandler(CsrIpTlsInstanceData *);
static void connectionDestroyReqHandler(CsrIpTlsInstanceData *);
static void connectionConnectReqHandler(CsrIpTlsInstanceData *);
static void connectionDisconnectReqHandler(CsrIpTlsInstanceData *);
static void connectionDisconnectResHandler(CsrIpTlsInstanceData *);
static void connectionCertResHandler(CsrIpTlsInstanceData *);
static void connectionDataReqHandler(CsrIpTlsInstanceData *);
static void connectionDataResHandler(CsrIpTlsInstanceData *);

/* Dispatch table */
static const CsrTlsEventHandler tlsHandler[] =
{
    sessionCreateHandler,           /* CSR_TLS_SESSION_CREATE_REQ */
    sessionDestroyHandler,          /* CSR_TLS_SESSION_DESTROY_REQ */
    configCipherReqHandler,         /* CSR_TLS_CONFIG_CIPHER_REQ */
    configCertClientReqHandler,     /* CSR_TLS_CONFIG_CERT_CLIENT_REQ */
    configCertVerifyReqHandler,     /* CSR_TLS_CONFIG_CERT_VERIFY_REQ */
    NULL,                           /* CSR_TLS_CONFIG_COMPRESSION_REQ */
    connectionCreateReqHandler,     /* CSR_TLS_CONNECTION_CREATE_REQ */
    connectionDestroyReqHandler,    /* CSR_TLS_CONNECTION_DESTROY_REQ */
    connectionBindReqHandler,       /* CSR_TLS_CONNECTION_BIND_REQ */
    connectionConnectReqHandler,    /* CSR_TLS_CONNECTION_CONNECT_REQ */
    connectionDisconnectReqHandler, /* CSR_TLS_CONNECTION_DISCONNECT_REQ */
    connectionDisconnectResHandler, /* CSR_TLS_CONNECTION_DISCONNECT_RES */
    connectionCertResHandler,       /* CSR_TLS_CONNECTION_CERT_RES */
    connectionDataReqHandler,       /* CSR_TLS_CONNECTION_DATA_REQ */
    connectionDataResHandler        /* CSR_TLS_CONNECTION_DATA_RES */
};

static CsrInt32 getNewSocketHandle(CsrIpTlsInstanceData *instanceData, void *sockInst)
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

static in_addr_t ipToNetwork(CsrUint8 *ip)
{
    return (ip[0] << 24) +
           (ip[1] << 16) +
           (ip[2] << 8) +
           (ip[3]);
}

#ifdef CSR_TLS_SUPPORT_PSK
/* The following define can be used to define an external psk client call-back function if the application needs more functionality
    that is provided by this standard function */
#ifndef CSR_TLS_EXTERNAL_PSK_CLIENT_CALLBACK_FUNCTION
static unsigned int pskClientCallback(SSL *ssl, const char *hint, char *identity,
    unsigned int maxIdentityLen, unsigned char *psk,
    unsigned int maxPskLen)
{
    unsigned int pskLen = 0;
    CsrUint8 count;
    csrTlsSession *session = NULL;

    if (globalCsrIpTlsInstanceData)
    {
        CSR_LOG_TEXT_INFO((CsrIpLto, 4, "pskCLientCallback: Sesssions stored %d", globalCsrIpTlsInstanceData->tlsSessions));

        /*find right psk in store*/
        session = globalCsrIpTlsInstanceData->tlsSessionList;
        for (count = 0; count < globalCsrIpTlsInstanceData->tlsSessions && session; count++)
        {
            if (session->ctx == ssl->ctx)
            {
                /*identical ctx -> right session found*/
                CSR_LOG_TEXT_INFO((CsrIpLto, 4, "pskCLientCallback: Session found %d", count));
                break;
            }
            session = session->next;
        }

        if (session)
        {
            /*add a default identity*/
            CsrStrNCpy(identity, CSR_TLS_PSK_CLIENT_IDENTITY, maxIdentityLen);
            CSR_LOG_TEXT_INFO((CsrIpLto, 4, "pskCLientCallback: identity %s", identity));

            /*the identity-hint is not added/used here*/

            if ((unsigned int) session->pskKeyLength > maxPskLen)
            {
                CSR_LOG_TEXT_WARNING((CsrIpLto, 4, "pskCLientCallback: Stored psk key length larger than supported %d, %d", session->pskKeyLength, maxPskLen));
            }
            else
            {
                pskLen = (unsigned int) session->pskKeyLength;
                CsrMemCpy(psk, session->pskKey, pskLen);
                CSR_LOG_TEXT_INFO((CsrIpLto, 4, "pskCLientCallback: psk key of length %d copied to ssl store", pskLen));
            }
        }
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 4, "pskCLientCallback: No globalCsrIpTlsInstanceData"));
    }

    return pskLen;
}

#else
unsigned int pskClientCallback(SSL *ssl, const char *hint, char *identity,
    unsigned int maxIdentityLen, unsigned char *psk,
    unsigned int maxPskLen);
#endif
#endif

/*
 * Dummy function which need to triger a wrong set password.
 * Please see caller for full explanation.
 */
int dummyPasswordCB(char *buf, int size, int rwflag, void *userdata)
{
    CsrUint8 len;
    CsrCharString *tmp;

    CSR_LOG_TEXT_WARNING((CsrIpLto, 4,
                          "Dummy password CB executed. Password probably not correct"));

    /* assign dummy password */
    tmp = "asdf";
    len = CsrStrLen(tmp);

    /* if too long, truncate */
    if (len > size)
    {
        len = size;
    }

    CsrMemCpy(buf, tmp, len);

    return (int) len;
}

/* Translating between RFC (& CSR) cipher suite code to OpenSSL name */
static CsrCharString *cipherSuiteInterpreter(CsrUint16 cipherSuite)
{
    /* It is important that all returns are null terminated */
    switch (cipherSuite)
    {
        case CSR_TLS_NULL_WITH_NULL_NULL:
            return NULL; /* Not suppported */

        case CSR_TLS_RSA_WITH_NULL_MD5:
            return "NULL-MD5,";
        case CSR_TLS_RSA_WITH_NULL_SHA:
            return "NULL-SHA,";
        case CSR_TLS_RSA_WITH_NULL_SHA256:
            return NULL; /* Not suppported */
        case CSR_TLS_RSA_WITH_RC4_128_MD5:
            return "RC4-MD5,";
        case CSR_TLS_RSA_WITH_RC4_128_SHA:
            return "RC4-SHA,";
        case CSR_TLS_RSA_WITH_3DES_EDE_CBC_SHA:
            return "DES-CBC3-SHA,";
        case CSR_TLS_RSA_WITH_AES_128_CBC_SHA:
            return "AES128-SHA,";
        case CSR_TLS_RSA_WITH_AES_128_CBC_SHA256:
            return NULL; /* Not suppported */
        case CSR_TLS_RSA_WITH_AES_256_CBC_SHA:
            return "AES256-SHA,";
        case CSR_TLS_RSA_WITH_AES_256_CBC_SHA256:
            return NULL; /* Not suppported */

        case CSR_TLS_DH_DSS_WITH_3DES_EDE_CBC_SHA:
            return NULL; /* Not implemented in openSSL*/
        case CSR_TLS_DH_DSS_WITH_AES_128_CBC_SHA:
            return NULL; /* Not implemented in openSSL*/
        case CSR_TLS_DH_DSS_WITH_AES_128_CBC_SHA256:
            return NULL; /* Not suppported */
        case CSR_TLS_DH_DSS_WITH_AES_256_CBC_SHA:
            return NULL; /* Not implemented in openSSL*/
        case CSR_TLS_DH_DSS_WITH_AES_256_CBC_SHA256:
            return NULL; /* Not suppported */
        case CSR_TLS_DH_RSA_WITH_3DES_EDE_CBC_SHA:
        case CSR_TLS_DH_RSA_WITH_AES_128_CBC_SHA:
            return NULL; /* Not implemented in openSSL*/
        case CSR_TLS_DH_RSA_WITH_AES_128_CBC_SHA256:
            return NULL; /* Not suppported */
        case CSR_TLS_DH_RSA_WITH_AES_256_CBC_SHA:
            return NULL; /* Not implemented in openSSL*/
        case CSR_TLS_DH_RSA_WITH_AES_256_CBC_SHA256:
            return NULL; /* Not suppported */

        case CSR_TLS_DHE_DSS_WITH_3DES_EDE_CBC_SHA:
            return "EDH-DSS-DES-CBC3-SHA,";
        case CSR_TLS_DHE_DSS_WITH_AES_128_CBC_SHA:
            return "DHE-DSS-AES128-SHA,";
        case CSR_TLS_DHE_DSS_WITH_AES_128_CBC_SHA256:
            return NULL; /* Not suppported */
        case CSR_TLS_DHE_DSS_WITH_AES_256_CBC_SHA:
            return "DHE-DSS-AES256-SHA,";
        case CSR_TLS_DHE_DSS_WITH_AES_256_CBC_SHA256:
            return NULL; /* Not suppported */
        case CSR_TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA:
            return "EDH-RSA-DES-CBC3-SHA,";
        case CSR_TLS_DHE_RSA_WITH_AES_128_CBC_SHA:
            return "DHE-RSA-AES128-SHA,";
        case CSR_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256:
            return NULL; /* Not suppported */
        case CSR_TLS_DHE_RSA_WITH_AES_256_CBC_SHA:
            return "DHE-RSA-AES256-SHA,";
        case CSR_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256:
            return NULL; /* Not suppported */

        case CSR_TLS_DH_anon_WITH_3DES_EDE_CBC_SHA:
            return "ADH-DES-CBC3-SHA,";
        case CSR_TLS_DH_anon_WITH_AES_128_CBC_SHA:
            return "ADH-AES128-SHA,";
        case CSR_TLS_DH_anon_WITH_AES_128_CBC_SHA256:
            return NULL; /* Not suppported */
        case CSR_TLS_DH_anon_WITH_AES_256_CBC_SHA:
            return "ADH-AES256-SHA,";
        case CSR_TLS_DH_anon_WITH_AES_256_CBC_SHA256:
            return NULL; /* Not suppported */
        case CSR_TLS_DH_anon_WITH_RC4_128_MD5:
            return "ADH-RC4-MD5,";

#ifdef CSR_TLS_SUPPORT_PSK
        /* First available when upgrading to OpenSSL v 1.0.0 */
        case CSR_TLS_PSK_WITH_RC4_128_SHA:
            return "PSK-RC4-SHA,";
        case CSR_TLS_PSK_WITH_3DES_EDE_CBC_SHA:
            return "PSK-3DES-EDE-CBC-SHA,";
        case CSR_TLS_PSK_WITH_AES_128_CBC_SHA:
            return "PSK-AES128-CBC-SHA,";
        case CSR_TLS_PSK_WITH_AES_256_CBC_SHA:
            return "PSK-AES256-CBC-SHA,";
        case CSR_TLS_DHE_PSK_WITH_RC4_128_SHA:
            return NULL; /* Not suppported */
        case CSR_TLS_DHE_PSK_WITH_3DES_EDE_CBC_SHA:
            return NULL; /* Not suppported */
        case CSR_TLS_DHE_PSK_WITH_AES_128_CBC_SHA:
            return NULL; /* Not suppported */
        case CSR_TLS_DHE_PSK_WITH_AES_256_CBC_SHA:
            return NULL; /* Not suppported */
        case CSR_TLS_RSA_PSK_WITH_RC4_128_SHA:
            return NULL; /* Not suppported */
        case CSR_TLS_RSA_PSK_WITH_3DES_EDE_CBC_SHA:
            return NULL; /* Not suppported */
        case CSR_TLS_RSA_PSK_WITH_AES_128_CBC_SHA:
            return NULL; /* Not suppported */
        case CSR_TLS_RSA_PSK_WITH_AES_256_CBC_SHA:
            return NULL; /* Not suppported */
#endif

        default:
            return NULL;
    }
}

/* Dispatcher */

void CsrIpTlsHandler(CsrIpTlsInstanceData *instanceData, void *msg)
{
    CsrTlsPrim *prim;

    instanceData->msg = msg;

    prim = (CsrTlsPrim *) msg;

    if (*prim <= CSR_TLS_PRIM_DOWNSTREAM_HIGHEST)
    {
        tlsHandler[*prim](instanceData);
    }
}

/*
* Internal interfaces
*/

static void configCipherReqHandler(CsrIpTlsInstanceData *instanceData)
{
    CsrTlsConfigCipherReq *req;
    CsrTlsConfigCipherCfm *cfm;
    csrTlsSession *session;
    SSL_CTX *ctx;
    CsrCharString *cipherList;
    CsrCharString *cipherTemp;
    CsrUint8 cipherListLength;                /*Is limited to 255 in openSSL*/
    CsrUint16 ii;

    req = (CsrTlsConfigCipherReq *) instanceData->msg;
    session = req->session;
    ctx = session->ctx;

    CSR_LOG_TEXT_WARNING((CsrIpLto, 4,
                          "Handler: session id %x\n", session));

    CSR_TLS_ASSERT(ctx, "Invalid SSL ctx");

    cfm = CsrPmemAlloc(sizeof(*cfm));
    cfm->type = CSR_TLS_CONFIG_CIPHER_CFM;
    cfm->session = req->session;
    cfm->result = CSR_RESULT_SUCCESS;

    /* Construct comma seperate list of ciphers */
    cipherListLength = 255;
    cipherList = CsrPmemZalloc(sizeof(*cipherList) * cipherListLength);

    /* Translate from the RFC(CSR) CipherSuite code into OpenSSL strings*/
    for (ii = 0;
         ii < req->cipherSuiteLength;
         ii++)
    {
        if ((cipherTemp = cipherSuiteInterpreter(req->cipherSuite[ii])) == NULL)
        {
            CSR_LOG_TEXT_ERROR((CsrIpLto, 4,
                                "Error CipherSuite (%x) Unknown or not supportet.\n",
                                req->cipherSuite[ii]));
            cfm->result = CSR_RESULT_FAILURE;
        }
        else
        {
            cipherList = CsrStrNCat(cipherList, cipherTemp, cipherListLength - 1);
        }
    }

    /*Set cipher list*/
    if ((SSL_CTX_set_cipher_list(ctx, (char *) cipherList) <= 0))
    {
        CSR_LOG_TEXT_ERROR((CsrIpLto, 4,
                            "Error setting the cipher list.\n"));
        cfm->result = CSR_RESULT_FAILURE;
    }


    CsrPmemFree(cipherList);
    CsrPmemFree(req->cipherSuite);
    req->cipherSuite = NULL;

    CsrSchedMessagePut(session->qid, CSR_TLS_PRIM, cfm);
}

static void configCertClientReqHandler(CsrIpTlsInstanceData *instanceData)
{
    CsrTlsConfigCertClientReq *req;
    CsrTlsConfigCertClientCfm *cfm;
    csrTlsSession *session;
    SSL_CTX *ctx;
    BIO *in = NULL;
    X509 *certificate;
    EVP_PKEY *keyPair = NULL;
    CsrUint8 *passPhrase;

    req = (CsrTlsConfigCertClientReq *) instanceData->msg;
    session = req->session;
    ctx = session->ctx;
    passPhrase = req->password;

    cfm = CsrPmemAlloc(sizeof(*cfm));
    cfm->type = CSR_TLS_CONFIG_CERT_CLIENT_CFM;
    cfm->session = req->session;
    cfm->result = CSR_RESULT_SUCCESS;

#ifdef CSR_TLS_SUPPORT_PSK
    if (req->certificateType != CSR_TLS_PSK)
#endif
    {
        /* Registrate client certifcate in CTX*/
        if (!(in = BIO_new_mem_buf(req->certificate, req->certificateLength)))
        {
            CSR_LOG_TEXT_ERROR((CsrIpLto, 4,
                                "Couldn't place certificate in BIO buffer"));

            cfm->result = CSR_RESULT_FAILURE;
        }
    }

    if (req->certificateType == CSR_TLS_CERTIFICATE)
    {
        if (!(certificate = PEM_read_bio_X509(in, NULL, NULL,
                  ctx->default_passwd_callback_userdata)))
        {
            CSR_LOG_TEXT_WARNING((CsrIpLto, 4,
                                  "Couldn't transform into X509 object"));
            cfm->result = CSR_RESULT_FAILURE;
        }

        if (!SSL_CTX_use_certificate(ctx, certificate))
        {
            CSR_LOG_TEXT_ERROR((CsrIpLto, 4,
                                "Error loading client certificate into CTX"));
            cfm->result = CSR_RESULT_FAILURE;
        }
        else
        {
            CSR_LOG_TEXT_INFO((CsrIpLto, 4,
                               "Client certificate loaded into CTX"));
        }
    }
    /* Loading client private key */

    /* The certificate can also include the private key, so this signal
        do not need to be repeated. */
    if ((req->certificateType == CSR_TLS_PRIVATE_KEY) ||
        (req->certificateType == CSR_TLS_CERTIFICATE))
    {
        /* When passing a NULL password to the OpenSSL library the library
            will automatic prompt for a password if the private key is
            password protected. If the user by mistake pass a NULL password
            to the task, it will end up in waiting for a propt input which
            will never occurre. A dummy password CB is therefor assigned which
            will make the loading return a failure.*/

        if (passPhrase == NULL)
        {
            /* Assign dummy CB */
            if (!(keyPair = PEM_read_bio_PrivateKey(in, NULL, &dummyPasswordCB,
                      passPhrase)))
            {
                CSR_LOG_TEXT_WARNING((CsrIpLto, 4,
                                      "Couldn't transform key pair into EVP_PKEY object. Key may be password protected or no private key is in certifcate"));

                /* Error handling only if it is a private key msg since
                        certificate not always include a private key */
                if (req->certificateType == CSR_TLS_PRIVATE_KEY)
                {
                    cfm->result = CSR_RESULT_FAILURE;
                }
            }
        }
        else
        {
            if (!(keyPair = PEM_read_bio_PrivateKey(in, NULL, 0, passPhrase)))
            {
                CSR_LOG_TEXT_WARNING((CsrIpLto, 4,
                                      "Couldn't transform key pair into EVP_PKEY object"));

                /* Error handling only if it is a private key msg since
                        certificate not always include a private key */
                if (req->certificateType == CSR_TLS_PRIVATE_KEY)
                {
                    cfm->result = CSR_RESULT_FAILURE;
                }
            }
        }
        if (!SSL_CTX_use_PrivateKey(ctx, keyPair))
        {
            CSR_LOG_TEXT_ERROR((CsrIpLto, 4,
                                "Error loading client key pair into CTX"));
            /* Error handling only if it is a private key msg since
                certificate not always include a private key */
            if (req->certificateType == CSR_TLS_PRIVATE_KEY)
            {
                cfm->result = CSR_RESULT_FAILURE;
            }
        }
        else
        {
            CSR_LOG_TEXT_INFO((CsrIpLto, 4,
                               "Private key added to CTX"));
        }
    }
    else if (req->certificateType == CSR_TLS_PSK)
    {
#ifdef CSR_TLS_SUPPORT_PSK
        CsrPmemFree(session->pskKey);
        session->pskKey = req->certificate;
        session->pskKeyLength = req->certificateLength;
        req->certificate = NULL;

        SSL_CTX_set_psk_client_callback(ctx, pskClientCallback);

        CSR_LOG_TEXT_INFO((CsrIpLto, 4,
                           "PSK key stored in session"));
#else
        CSR_LOG_TEXT_ERROR((CsrIpLto, 4,
                            "PSK key are currently not supported"));
        CsrPmemFree(req->certificate);
        req->certificate = NULL;
        cfm->result = CSR_RESULT_FAILURE;
#endif
    }

    EVP_PKEY_free(keyPair);
    BIO_free(in);
    CsrSchedMessagePut(session->qid, CSR_TLS_PRIM, cfm);
}

static void configCertVerifyReqHandler(CsrIpTlsInstanceData *instanceData)
{
    CsrTlsConfigCertVerifyReq *req;
    CsrTlsConfigCertVerifyCfm *cfm;
    csrTlsSession *session;
    SSL_CTX *ctx;
    BIO *in;
    STACK_OF(X509_INFO) * inf;
    X509_INFO *itmp;

    req = (CsrTlsConfigCertVerifyReq *) instanceData->msg;
    session = req->session;
    ctx = session->ctx;

    cfm = CsrPmemAlloc(sizeof(*cfm));
    cfm->type = CSR_TLS_CONFIG_CERT_VERIFY_CFM;
    cfm->session = req->session;

    /* In order to add the certificate verification chain to the ctx it is
    *  need to do add them to the internal store in the ctx */
    if (!(in = BIO_new_mem_buf(req->trustedCAcertificate,
              req->trustedCAcertificateLength)))
    {
        CSR_LOG_TEXT_ERROR((CsrIpLto, 4,
                            "Couldn't place CA certificate in BIO buffer"));

        cfm->result = CSR_RESULT_FAILURE;
    }
    else if (!(inf = PEM_X509_INFO_read_bio(in, NULL, NULL, NULL)))
    {
        BIO_free(in);

        CSR_LOG_TEXT_ERROR((CsrIpLto, 4,
                            "BIO buffer does not contain any certificate"));

        cfm->result = CSR_RESULT_FAILURE;
    }
    else
    {
        int num;

        BIO_free(in);

        num = sk_X509_INFO_num(inf);

        if (num >= 0)
        {
            CsrSize count, ii;

            for (ii = 0, count = 0; ii < (CsrSize) num; ii++)
            {
                itmp = sk_X509_INFO_value(inf, ii);
                if (itmp->x509)
                {
                    if (!X509_STORE_add_cert(ctx->cert_store, itmp->x509))
                    {
                        CSR_LOG_TEXT_ERROR((CsrIpLto, 4,
                                            "Certificate %d could not be added to X509 store: %d",
                                            ii,
                                            ERR_get_error()));
                    }
                    else
                    {
                        count++;
                    }
                }
                /* Currently we do not support CRL */
                /*if(itmp->crl)
                {
                    X509_STORE_add_crl(ctx->store_ctx, itmp->crl);
                    count++;
                }*/
            }

            if (count == (CsrSize) num)
            {
                cfm->result = CSR_RESULT_SUCCESS;
            }
            else
            {
                cfm->result = CSR_RESULT_FAILURE;
            }

            CSR_LOG_TEXT_INFO((CsrIpLto, 4,
                               "%d certificates added in certificate chain",
                               count));
        }
        else
        {
            CSR_LOG_TEXT_WARNING((CsrIpLto, 4,
                                  "no certificates in certificate chain"));
            cfm->result = CSR_RESULT_FAILURE;
        }

        sk_X509_INFO_pop_free(inf, X509_INFO_free);
    }

    CsrSchedMessagePut(session->qid, CSR_TLS_PRIM, cfm);
}

static void sessionCreateHandler(CsrIpTlsInstanceData *instanceData)
{
    CsrTlsSessionCreateReq *msg = instanceData->msg;

    CsrIpLock(instanceData->ipInstanceData);

    if (instanceData->tlsSessions < CSR_TLS_SESSIONLIMIT)
    {
        csrTlsSession *sess;
        SSL_CTX *ctx;
        const SSL_METHOD *method;

        /* Create ctx context
        *
        * If both SSLv2 SSLv3 and TLSv1 should be supported,
        * method creation should be changed to SSLv23_method();
        * TLSv1_1_client_method() for TLS 1.1.
        */
        method = TLSv1_client_method();
        ctx = SSL_CTX_new((SSL_METHOD *) method);

        sess = CsrPmemAlloc(sizeof(*sess));
        sess->qid = msg->qid;
        sess->connections = 0;
        sess->connList = NULL;
        sess->ctx = ctx;

#ifdef CSR_TLS_SUPPORT_PSK
        sess->pskKey = NULL;
        sess->pskKeyLength = 0;
#endif

        sess->reap = FALSE;

        sess->next = instanceData->tlsSessionList;
        instanceData->tlsSessionList = sess;
        instanceData->tlsSessions++;

        CSR_LOG_TEXT_WARNING((CsrIpLto, 4,
                              "Handler: session id %x\n", sess));

        CsrTlsSessionCreateCfmSend(msg->qid, sess, CSR_RESULT_SUCCESS);
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 4,
                              "Number of session limit achieve"));

        CsrTlsSessionCreateCfmSend(msg->qid, NULL, CSR_RESULT_FAILURE);
    }

    CsrIpUnlock(instanceData->ipInstanceData);
}

void sessionDestroyHandler(CsrIpTlsInstanceData *instanceData)
{
    CsrTlsSessionDestroyReq *msg = (CsrTlsSessionDestroyReq *) instanceData->msg;
    csrTlsSession *sess = (csrTlsSession *) msg->session;
    CsrTlsSocketInst *sockInst;
    csrTlsConnection *conn;

    if (sess != NULL)
    {
        CsrIpLock(instanceData->ipInstanceData);
        conn = sess->connList;
        while (conn)
        {
            sockInst = csrIpTlsInstGet(instanceData, conn->socketHandle);
            sockInst->reap = TRUE;
            sess->reap = TRUE;
            CsrIpFdRemove(instanceData->ipInstanceData, sockInst->socket, TRUE, TRUE);
            conn = conn->next;
        }
        if (!sess->reap)
        {
            CsrTlsSessionDestroyCfmSend(sess->qid, sess, CSR_RESULT_SUCCESS);
            csrIpTlsSessionDestroy(instanceData, sess);
        }
        CsrIpUnlock(instanceData->ipInstanceData);
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 4, "Faild to get session"));
    }
}

static void connectionCreateReqHandler(CsrIpTlsInstanceData *instanceData)
{
    CsrTlsConnectionCreateReq *msg = (CsrTlsConnectionCreateReq *) instanceData->msg;
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
            CSR_LOG_TEXT_WARNING((CsrIpLto, 4,
                                  "setsockopt failed, errno=%d", errno));
        }

        if (fcntl(sock, F_SETFD, FD_CLOEXEC) == -1)
        {
            CSR_LOG_TEXT_WARNING((CsrIpLto, 0, "fcntl(F_SETFD, FD_CLOEXEC) failed, errno=%d", errno));
        }

        if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
                (char *) &tcpNoDelay, sizeof(tcpNoDelay)) == 0)
        {
            CsrInt32 sockHandle;

            CsrIpLock(instanceData->ipInstanceData);

            sockHandle = getNewSocketHandle(instanceData, sockInst);

            if ((sess->connections < CSR_TLS_CONNECTIONLIMIT) &&
                (sockHandle != -1))
            {
                csrTlsConnection *conn;
                CsrResult res;

                conn = CsrPmemAlloc(sizeof(*conn));

                /* socketHandle will be filled in later */
                conn->session = sess;

                conn->next = sess->connList;
                sess->connList = conn;
                sess->connections++;

                sockInst->handle = (CsrUint16) sockHandle;
                sockInst->family = msg->socketFamily;
                sockInst->dead = FALSE;
                sockInst->reap = FALSE;
                sockInst->serverName = msg->serverName;
                sockInst->rxQueue = NULL;

                res = CsrEventCreate(&sockInst->callbackEvent);
                CSR_TLS_ASSERT(res == CSR_RESULT_SUCCESS,
                    "could not create event");

                sockInst->instanceData = instanceData;
                sockInst->ssl = NULL;
                sockInst->readBlockedOnWrite = FALSE;
                sockInst->writeBlockedOnRead = FALSE;
                sockInst->conn = conn;
                conn->socketHandle = sockInst->handle;

                CsrIpFdNew(instanceData->ipInstanceData, sock);
                CsrTlsConnectionCreateCfmSend(sess->qid, sess,
                    CSR_RESULT_SUCCESS, conn);
            }
            else
            {
                CSR_LOG_TEXT_WARNING((CsrIpLto, 4,
                                      "getNewSocketHandle failed"));

                CsrTlsConnectionCreateCfmSend(sess->qid, sess,
                    CSR_TLS_RESULT_NO_MORE_SOCKETS, NULL);
                csrTlsSocketInstFree(instanceData, sockInst);
            }
            CsrIpUnlock(instanceData->ipInstanceData);
        }
        else
        {
            CsrTlsConnectionCreateCfmSend(sess->qid, sess,
                CSR_RESULT_FAILURE, NULL);
            CSR_LOG_TEXT_WARNING((CsrIpLto, 4,
                                  "setsockopt failed, errno=%d", errno));
            csrTlsSocketInstFree(instanceData, sockInst);
        }
    }
    else
    {
        CsrTlsConnectionCreateCfmSend(sess->qid, sess,
            CSR_RESULT_FAILURE, NULL);
    }
}

static void connectionBindReqHandler(CsrIpTlsInstanceData *instanceData)
{
    CsrTlsConnectionBindReq *msg = (CsrTlsConnectionBindReq *) instanceData->msg;
    csrTlsConnection *conn = (csrTlsConnection *) msg->conn;
    CsrTlsSocketInst *sockInst;

    CsrIpLock(instanceData->ipInstanceData);
    sockInst = csrIpTlsInstGet(instanceData, conn->socketHandle);
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
        CSR_LOG_TEXT_WARNING((CsrIpLto, 4,
                              "failed getting socket inst"));
    }
    CsrIpUnlock(instanceData->ipInstanceData);
}

static void connectionDestroyReqHandler(CsrIpTlsInstanceData *instanceData)
{
    CsrTlsConnectionDestroyReq *msg = (CsrTlsConnectionDestroyReq *) instanceData->msg;
    csrTlsConnection *conn = (csrTlsConnection *) msg->conn;
    CsrTlsSocketInst *sockInst;

    CsrIpLock(instanceData->ipInstanceData);
    sockInst = csrIpTlsInstGet(instanceData, conn->socketHandle);
    if (sockInst != NULL)
    {
        CsrTlsConnectionDestroyCfmSend(conn->session->qid, conn);
        csrTlsSocketInstFree(instanceData, sockInst);
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 4,
                              "failed getting socket inst"));
    }
    CsrIpUnlock(instanceData->ipInstanceData);
}

static void connectionConnectReqHandler(CsrIpTlsInstanceData *instanceData)
{
    CsrTlsConnectionConnectReq *msg = (CsrTlsConnectionConnectReq *) instanceData->msg;
    csrTlsConnection *conn = (csrTlsConnection *) msg->conn;
    CsrTlsSocketInst *sockInst;

    CsrIpLock(instanceData->ipInstanceData);
    sockInst = csrIpTlsInstGet(instanceData, conn->socketHandle);
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
            CSR_LOG_TEXT_INFO((CsrIpLto, 4,
                               "socket %d connected", sockInst->socket));
            sockInst->type = CSR_TLS_TYPE_TCP_SSL_CONNECT;
            csrIpTlsCreateSsl(instanceData, sockInst);
            csrIpTlsConnectSsl(instanceData, sockInst);
        }
        else if (errno == EINPROGRESS)
        {
            sockInst->type = CSR_TLS_TYPE_TCP_CONNECT;
            CsrIpFdAdd(instanceData->ipInstanceData, sockInst->socket, TRUE, TRUE);
        }
        else
        {
            CsrTlsConnectionConnectCfmSend(conn->session->qid, conn,
                CSR_RESULT_FAILURE);
            csrTlsSocketInstFree(instanceData, sockInst);
        }
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 4,
                              "failed getting socket inst"));
    }
    CsrIpUnlock(instanceData->ipInstanceData);
}

static void connectionDataReqHandler(CsrIpTlsInstanceData *instanceData)
{
    CsrTlsConnectionDataReq *msg = (CsrTlsConnectionDataReq *) instanceData->msg;
    csrTlsConnection *conn = (csrTlsConnection *) msg->conn;
    CsrTlsSocketInst *sockInst;

    CsrIpLock(instanceData->ipInstanceData);
    sockInst = csrIpTlsInstGet(instanceData, conn->socketHandle);
    if ((sockInst != NULL) && !sockInst->dead)
    {
        sockInst->data = msg->buf;
        sockInst->dataOffset = 0;
        sockInst->dataLen = msg->bufLen;
        CsrIpFdAdd(instanceData->ipInstanceData, sockInst->socket, FALSE, TRUE);
    }
    else
    {
        CsrPmemFree(msg->buf);
        msg->buf = NULL;
    }
    CsrIpUnlock(instanceData->ipInstanceData);
}

static void connectionDataResHandler(CsrIpTlsInstanceData *instanceData)
{
    CsrTlsConnectionDataRes *msg = (CsrTlsConnectionDataRes *) instanceData->msg;
    csrTlsConnection *conn = (csrTlsConnection *) msg->conn;
    CsrTlsSocketInst *sockInst;
    csrTlsRxElement *rxelem;

    CsrIpLock(instanceData->ipInstanceData);
    sockInst = csrIpTlsInstGet(instanceData, conn->socketHandle);
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
    CsrIpUnlock(instanceData->ipInstanceData);
}

static void connectionDisconnectReqHandler(CsrIpTlsInstanceData *instanceData)
{
    CsrTlsConnectionDisconnectReq *msg = (CsrTlsConnectionDisconnectReq *) instanceData->msg;
    csrTlsConnection *conn = (csrTlsConnection *) msg->conn;
    CsrTlsSocketInst *sockInst;

    CsrIpLock(instanceData->ipInstanceData);
    sockInst = csrIpTlsInstGet(instanceData, conn->socketHandle);
    if (sockInst != NULL)
    {
        sockInst->reap = TRUE;
        CsrIpFdRemove(instanceData->ipInstanceData, sockInst->socket, TRUE, TRUE);
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 4, "failed getting socket inst"));
    }
    CsrIpUnlock(instanceData->ipInstanceData);
}

static void connectionDisconnectResHandler(CsrIpTlsInstanceData *instanceData)
{
    CsrTlsConnectionDisconnectRes *msg = (CsrTlsConnectionDisconnectRes *) instanceData->msg;
    csrTlsConnection *conn = (csrTlsConnection *) msg->conn;
    CsrTlsSocketInst *sockInst;

    CsrIpLock(instanceData->ipInstanceData);
    sockInst = csrIpTlsInstGet(instanceData, conn->socketHandle);
    if (sockInst != NULL)
    {
        csrTlsSocketInstFree(instanceData, sockInst);
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 4, "failed getting socket inst"));
    }
    CsrIpUnlock(instanceData->ipInstanceData);
}

static void connectionCertResHandler(CsrIpTlsInstanceData *instanceData)
{
    CsrTlsConnectionCertRes *msg = (CsrTlsConnectionCertRes *) instanceData->msg;
    csrTlsConnection *conn = (csrTlsConnection *) msg->conn;
    CsrTlsSocketInst *sockInst;

    CsrIpLock(instanceData->ipInstanceData);
    sockInst = csrIpTlsInstGet(instanceData, conn->socketHandle);

    CSR_LOG_TEXT_DEBUG((CsrIpLto, 4, "DEBUG: certRes %d", sockInst));

    if (sockInst != NULL)
    {
        int rc;
        sockInst->acceptCert = msg->accepted;
        rc = CsrEventSet(&sockInst->callbackEvent, 0x1);
        CSR_TLS_ASSERT(rc == CSR_RESULT_SUCCESS, "could not set event");
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrIpLto, 4, "failed getting socket inst"));
    }
    CsrIpUnlock(instanceData->ipInstanceData);
}
