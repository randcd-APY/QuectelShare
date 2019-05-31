/*****************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include <openssl/ssl.h>
#include <stdio.h>

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_pmem.h"
#include "csr_prim_defs.h"
#include "csr_result.h"
#include "csr_sched.h"

#include "csr_tls_task.h"
#include "csr_tls_prim.h"

#include "csr_tls_sef_tls.h"

#include "csr_tls_handler.h"
#include "csr_tls_thread.h"
#include "csr_tls_util.h"

#include "csr_log.h"
#include "csr_log_text_2.h"

#define CSR_TLS_SESSIONLIMIT 8
#define CSR_TLS_CONNECTIONLIMIT 8

static void configCipherReqHandler(CsrTlsInstanceData *);
static void configCertClientReqHandler(CsrTlsInstanceData *);
static void configCertVerifyReqHandler(CsrTlsInstanceData *);

/* Dispatch table */
static const CsrTlsEventHandler tlsHandler[] =
{
    CsrTlsSessionCreateHandler,             /* CSR_TLS_SESSION_CREATE_REQ */
    CsrTlsSessionDestroyHandler,            /* CSR_TLS_SESSION_DESTROY_REQ */
    configCipherReqHandler,                 /* CSR_TLS_CONFIG_CIPHER_REQ */
    configCertClientReqHandler,             /* CSR_TLS_CONFIG_CERT_CLIENT_REQ */
    configCertVerifyReqHandler,             /* CSR_TLS_CONFIG_CERT_VERIFY_REQ */
    NULL,                                   /* CSR_TLS_CONFIG_COMPRESSION_REQ */
    CsrTlsConnectionCreateReqHandler,       /* CSR_TLS_CONNECTION_CREATE_REQ */
    CsrTlsConnectionDestroyReqHandler,      /* CSR_TLS_CONNECTION_DESTROY_REQ */
    CsrTlsConnectionBindReqHandler,         /* CSR_TLS_CONNECTION_BIND_REQ */
    CsrTlsConnectionConnectReqHandler,      /* CSR_TLS_CONNECTION_CONNECT_REQ */
    CsrTlsConnectionDisconnectReqHandler,   /* CSR_TLS_CONNECTION_DISCONNECT_REQ */
    CsrTlsConnectionDisconnectResHandler,   /* CSR_TLS_CONNECTION_DISCONNECT_RES */
    CsrTlsConnectionCertResHandler,         /* CSR_TLS_CONNECTION_CERT_RES */
    CsrTlsConnectionDataReqHandler,         /* CSR_TLS_CONNECTION_DATA_REQ */
    CsrTlsConnectionDataResHandler          /* CSR_TLS_CONNECTION_DATA_RES */
};

#ifdef CSR_TLS_SUPPORT_PSK
/* The following define can be used to define an external psk client call-back function if the application needs more functionality
    that is provided by this standard function */
#ifndef CSR_TLS_EXTERNAL_PSK_CLIENT_CALLBACK_FUNCTION
unsigned int pskClientCallback(SSL *ssl, const char *hint, char *identity,
    unsigned int maxIdentityLen, unsigned char *psk,
    unsigned int maxPskLen)
{
    unsigned int pskLen = 0;
    CsrUint8 count;
    csrTlsSession *session = NULL;

    if (globalCsrTlsInstanceData)
    {
        CSR_LOG_TEXT_INFO((CsrTlsLto, 0, "pskCLientCallback: Sesssions stored %d", globalCsrTlsInstanceData->sessions));
        /*find right psk in store*/
        session = globalCsrTlsInstanceData->sessionList;
        for (count = 0; count < globalCsrTlsInstanceData->sessions && session; count++)
        {
            if (session->ctx == ssl->ctx)
            {
                /*identical ctx -> right session found*/
                CSR_LOG_TEXT_INFO((CsrTlsLto, 0, "pskCLientCallback: Session found %d", count));
                break;
            }
            session = session->next;
        }
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrTlsLto, 0, "pskCLientCallback: No globalCsrTlsInstanceData"));
    }

    if (session)
    {
        /*add a default identity*/
        CsrStrLCpy(identity, CSR_TLS_PSK_CLIENT_IDENTITY, maxIdentityLen);
        CSR_LOG_TEXT_INFO((CsrTlsLto, 0, "pskCLientCallback: identity %s", identity));

        /*the identity-hint is not added/used here*/

        if ((unsigned int) session->pskKeyLength > maxPskLen)
        {
            CSR_LOG_TEXT_WARNING((CsrTlsLto, 0, "pskCLientCallback: Stored psk key length larger than supported %d, %d", session->pskKeyLength, maxPskLen));
        }
        else
        {
            pskLen = (unsigned int) session->pskKeyLength;
            CsrMemCpy(psk, session->pskKey, pskLen);
            CSR_LOG_TEXT_INFO((CsrTlsLto, 0, "pskCLientCallback: psk key of length %d copied to ssl store", pskLen));
        }
    }

    return pskLen;
}

#else
unsigned int pskClientCallback(SSL *ssl, const char *hint, char *identity,
    unsigned int maxIdentityLen, unsigned char *psk,
    unsigned int maxPskLen);
#endif
#endif

/* Dummy function which need to tricker a wrong set password.
    Please see caller for full explanation. */
int dummyPasswordCB(char *buf, int size, int rwflag, void *userdata)
{
    CsrUint8 len;
    CsrCharString *tmp;

    CSR_LOG_TEXT_WARNING((CsrTlsLto, 0,
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
CsrCharString *cipherSuiteInterpreter(CsrUint16 cipherSuite)
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

void CsrTlsTlsHandler(CsrTlsInstanceData *instanceData)
{
    CsrTlsPrim *prim;

    prim = (CsrTlsPrim *) instanceData->msg;

    if (*prim <= CSR_TLS_PRIM_DOWNSTREAM_HIGHEST)
    {
        tlsHandler[*prim](instanceData);
    }
}

/*
* Internal interfaces
*/

/* Message handler functions. */
csrTlsSession *sessionCreateHandler(CsrTlsInstanceData *instanceData, CsrSchedQid qid)
{
    if (instanceData->sessions < CSR_TLS_SESSIONLIMIT)
    {
        csrTlsSession *sess;
        SSL_CTX *ctx;
        const SSL_METHOD *method;

        /* Create ctx context
        *
        * If both SSLv2 SSLv3 and TLSv1 should be supported,
        * method creation should be changed to SSLv23_method();
        */
        method = TLSv1_client_method();
        ctx = SSL_CTX_new((SSL_METHOD *) method);

        sess = CsrPmemAlloc(sizeof(*sess));
        sess->qid = qid;
        sess->connections = 0;
        sess->connList = NULL;
        sess->ctx = ctx;

#ifdef CSR_TLS_SUPPORT_PSK
        sess->pskKey = NULL;
        sess->pskKeyLength = 0;
#endif

        sess->next = instanceData->sessionList;
        instanceData->sessionList = sess;
        instanceData->sessions++;

        CSR_LOG_TEXT_WARNING((CsrTlsLto, 0,
                              "Handler: session id %x\n", sess));

        return sess;
    }
    else
    {
        CSR_LOG_TEXT_WARNING((CsrTlsLto, 0,
                              "Number of session limit achieve"));
        return NULL;
    }
}

void sessionDestroyHandler(CsrTlsInstanceData *instanceData, csrTlsSession *haystackSess)
{
    csrTlsSession *prevSess;
    csrTlsSession *sess;
    csrTlsConnection *conn;

    for (prevSess = NULL, sess = instanceData->sessionList;
         sess;
         prevSess = sess, sess = sess->next)
    {
        if (sess == haystackSess)
        {
            break;
        }
    }

    if (sess != NULL)
    {
        if (prevSess != NULL)
        {
            prevSess->next = sess->next;
        }
        else
        {
            instanceData->sessionList = sess->next;
        }

        conn = sess->connList;
        while (conn && sess->connections)
        {
            connectionDestroyReqHandler(conn);
            conn = sess->connList;
        }

        SSL_CTX_free(sess->ctx);
#ifdef CSR_TLS_SUPPORT_PSK
        CsrPmemFree(sess->pskKey);
#endif
        CsrPmemFree(sess);
        instanceData->sessions--;
    }
    /*
    * If we don't find it, someone's violating the API.
    * Since we don't have the session, we don't have a
    * qid to respond to so we just drop the request.
    */
}

static void configCipherReqHandler(CsrTlsInstanceData *instanceData)
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

    CSR_LOG_TEXT_WARNING((CsrTlsLto, 0,
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
            CSR_LOG_TEXT_ERROR((CsrTlsLto, 0,
                                "Error CipherSuite (%x) Unknown or not supportet.\n",
                                req->cipherSuite[ii]));
            cfm->result = CSR_RESULT_FAILURE;
        }
        else
        {
            cipherList = CsrStrLCat(cipherList, cipherTemp, cipherListLength);
        }
    }

    /*Set cipher list*/
    if ((SSL_CTX_set_cipher_list(ctx, (char *) cipherList) <= 0))
    {
        CSR_LOG_TEXT_ERROR((CsrTlsLto, 0,
                            "Error setting the cipher list.\n"));
        cfm->result = CSR_RESULT_FAILURE;
    }


    CsrPmemFree(cipherList);
    CsrPmemFree(req->cipherSuite);
    req->cipherSuite = NULL;

    CsrSchedMessagePut(session->qid, CSR_TLS_PRIM, cfm);
}

static void configCertClientReqHandler(CsrTlsInstanceData *instanceData)
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
            CSR_LOG_TEXT_ERROR((CsrTlsLto, 0,
                                "Couldn't place certificate in BIO buffer"));

            cfm->result = CSR_RESULT_FAILURE;
        }
    }

    if (req->certificateType == CSR_TLS_CERTIFICATE)
    {
        if (!(certificate = PEM_read_bio_X509(in, NULL, NULL,
                  ctx->default_passwd_callback_userdata)))
        {
            CSR_LOG_TEXT_WARNING((CsrTlsLto, 0,
                                  "Couldn't transform into X509 object"));
            cfm->result = CSR_RESULT_FAILURE;
        }

        if (!SSL_CTX_use_certificate(ctx, certificate))
        {
            CSR_LOG_TEXT_ERROR((CsrTlsLto, 0,
                                "Error loading client certificate into CTX"));
            cfm->result = CSR_RESULT_FAILURE;
        }
        else
        {
            CSR_LOG_TEXT_INFO((CsrTlsLto, 0,
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
                CSR_LOG_TEXT_WARNING((CsrTlsLto, 0,
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
                CSR_LOG_TEXT_WARNING((CsrTlsLto, 0,
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
            CSR_LOG_TEXT_ERROR((CsrTlsLto, 0,
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
            CSR_LOG_TEXT_INFO((CsrTlsLto, 0,
                               "Private key added to CTX"));
        }
    }
    else if (req->certificateType == CSR_TLS_PSK)
    {
#ifdef CSR_TLS_SUPPORT_PSK
        /* PSK key loading */
        CsrPmemFree(session->pskKey);
        session->pskKey = req->certificate;
        session->pskKeyLength = req->certificateLength;
        req->certificate = NULL;

        SSL_CTX_set_psk_client_callback(ctx, pskClientCallback);

        CSR_LOG_TEXT_INFO((CsrTlsLto, 0,
                           "PSK key stored in session"));
#else
        CSR_LOG_TEXT_ERROR((CsrTlsLto, 0,
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

static void configCertVerifyReqHandler(CsrTlsInstanceData *instanceData)
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
        CSR_LOG_TEXT_ERROR((CsrTlsLto, 0,
                            "Couldn't place CA certificate in BIO buffer"));

        cfm->result = CSR_RESULT_FAILURE;
    }
    else if (!(inf = PEM_X509_INFO_read_bio(in, NULL, NULL, NULL)))
    {
        BIO_free(in);

        CSR_LOG_TEXT_ERROR((CsrTlsLto, 0,
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
                        CSR_LOG_TEXT_ERROR((CsrTlsLto, 0,
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

            CSR_LOG_TEXT_INFO((CsrTlsLto, 0,
                               "%d certificates added in certificate chain",
                               count));
        }
        else
        {
            CSR_LOG_TEXT_WARNING((CsrTlsLto, 0,
                                  "no certificates in certificate chain"));
            cfm->result = CSR_RESULT_FAILURE;
        }

        sk_X509_INFO_pop_free(inf, X509_INFO_free);
    }

    CsrSchedMessagePut(session->qid, CSR_TLS_PRIM, cfm);
}

csrTlsConnection *connectionCreateReqHandler(csrTlsSession *session)
{
    if (session->connections < CSR_TLS_CONNECTIONLIMIT)
    {
        csrTlsConnection *conn;

        conn = CsrPmemAlloc(sizeof(*conn));

        /* socketHandle will be filled in later */
        conn->session = session;

        conn->next = session->connList;
        session->connList = conn;
        session->connections++;

        return conn;
    }
    else
    {
        return NULL;
    }
}

void connectionDestroyReqHandler(csrTlsConnection *conn)
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
