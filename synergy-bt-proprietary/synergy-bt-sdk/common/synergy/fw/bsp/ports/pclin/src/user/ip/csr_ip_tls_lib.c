/*****************************************************************************

Copyright (c) 2011-2015 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_tls_prim.h"
#include "csr_tls_lib.h"
#include "csr_pmem.h"

CsrTlsSessionCreateReq *CsrTlsSessionCreateReq_struct(
    CsrSchedQid qid)
{
    CsrTlsSessionCreateReq *prim;

    prim = CsrPmemAlloc(sizeof(*prim));
    prim->type = CSR_TLS_SESSION_CREATE_REQ;
    prim->qid = qid;

    return prim;
}

CsrTlsSessionDestroyReq *CsrTlsSessionDestroyReq_struct(
    CsrTlsSession session)
{
    CsrTlsSessionDestroyReq *prim;

    prim = CsrPmemAlloc(sizeof(*prim));
    prim->type = CSR_TLS_SESSION_DESTROY_REQ;
    prim->session = session;

    return prim;
}

CsrTlsConfigCipherReq *CsrTlsConfigCipherReq_struct(
    CsrTlsSession session,
    CsrUint16 *cipherSuite,
    CsrUint16 cipherSuiteLength,
    CsrUint16 keybitsMin,
    CsrUint16 keybitsMax)
{
    CsrTlsConfigCipherReq *prim;

    prim = CsrPmemAlloc(sizeof(*prim));
    prim->type = CSR_TLS_CONFIG_CIPHER_REQ;
    prim->session = session;
    prim->cipherSuite = cipherSuite;
    prim->cipherSuiteLength = cipherSuiteLength;
    prim->keybitsMin = keybitsMin;
    prim->keybitsMax = keybitsMax;

    return prim;
}

CsrTlsConfigCertClientReq *CsrTlsConfigCertClientReq_struct(
    CsrTlsSession session,
    CsrUint8 certificateType,
    CsrUint8 *certificate,
    CsrSize certificateLength,
    CsrUint8 *password,
    CsrSize passwordLength)
{
    CsrTlsConfigCertClientReq *prim;

    prim = CsrPmemAlloc(sizeof(*prim));
    prim->type = CSR_TLS_CONFIG_CERT_CLIENT_REQ;
    prim->session = session;
    prim->certificateType = certificateType;
    prim->certificate = certificate;
    prim->certificateLength = certificateLength;
    prim->password = password;
    prim->passwordLength = passwordLength;
    return prim;
}

CsrTlsConfigCertVerifyReq *CsrTlsConfigCertVerifyReq_struct(
    CsrTlsSession session,
    CsrUint8 *trustedCAcertificate,
    CsrSize trustedCAcertificateLength)
{
    CsrTlsConfigCertVerifyReq *prim;

    prim = CsrPmemAlloc(sizeof(*prim));
    prim->type = CSR_TLS_CONFIG_CERT_VERIFY_REQ;
    prim->session = session;
    prim->trustedCAcertificate = trustedCAcertificate;
    prim->trustedCAcertificateLength = trustedCAcertificateLength;

    return prim;
}

CsrTlsConfigCompressionReq *CsrTlsConfigCompressionReq_struct(
    CsrTlsSession session,
    CsrUint8 compression)
{
    CsrTlsConfigCompressionReq *prim;

    prim = CsrPmemAlloc(sizeof(*prim));
    prim->type = CSR_TLS_CONFIG_COMPRESSION_REQ;
    prim->session = session;
    prim->compression = compression;

    return prim;
}

CsrTlsConnectionCreateReq *CsrTlsConnectionCreateReq_struct(
    CsrTlsSession session, CsrIpSocketFamily socketFamily,
    CsrCharString *serverName)
{
    CsrTlsConnectionCreateReq *prim;

    prim = CsrPmemAlloc(sizeof(*prim));
    prim->type = CSR_TLS_CONNECTION_CREATE_REQ;
    prim->session = session;
    prim->socketFamily = socketFamily;
    prim->serverName = serverName;

    return prim;
}

CsrTlsConnectionDestroyReq *CsrTlsConnectionDestroyReq_struct(
    CsrTlsConnection conn)
{
    CsrTlsConnectionDestroyReq *prim;

    prim = CsrPmemAlloc(sizeof(*prim));
    prim->type = CSR_TLS_CONNECTION_DESTROY_REQ;
    prim->conn = conn;

    return prim;
}

CsrTlsConnectionBindReq *CsrTlsConnectionBindReq_struct(
    CsrTlsConnection conn,
    CsrUint8 ip[16],
    CsrUint16 port)
{
    CsrTlsConnectionBindReq *prim;

    prim = CsrPmemAlloc(sizeof(*prim));
    prim->type = CSR_TLS_CONNECTION_BIND_REQ;
    prim->conn = conn;
    CsrMemCpy(prim->ipAddress, ip, 16);
    prim->port = port;

    return prim;
}

CsrTlsConnectionConnectReq *CsrTlsConnectionConnectReq_struct(
    CsrTlsConnection conn,
    CsrUint8 ip[16],
    CsrUint16 port)
{
    CsrTlsConnectionConnectReq *prim;

    prim = CsrPmemAlloc(sizeof(*prim));
    prim->type = CSR_TLS_CONNECTION_CONNECT_REQ;
    prim->conn = conn;
    CsrMemCpy(prim->ipAddress, ip, 16);
    prim->port = port;

    return prim;
}

CsrTlsConnectionDisconnectReq *CsrTlsConnectionDisconnectReq_struct(
    CsrTlsConnection conn)
{
    CsrTlsConnectionDisconnectReq *prim;

    prim = CsrPmemAlloc(sizeof(*prim));
    prim->type = CSR_TLS_CONNECTION_DISCONNECT_REQ;
    prim->conn = conn;

    return prim;
}

CsrTlsConnectionDisconnectRes *CsrTlsConnectionDisconnectRes_struct(
    CsrTlsConnection conn)
{
    CsrTlsConnectionDisconnectRes *prim;

    prim = CsrPmemAlloc(sizeof(*prim));
    prim->type = CSR_TLS_CONNECTION_DISCONNECT_RES;
    prim->conn = conn;

    return prim;
}

CsrTlsConnectionCertRes *CsrTlsConnectionCertRes_struct(
    CsrTlsConnection conn,
    CsrBool accepted)
{
    CsrTlsConnectionCertRes *prim;

    prim = CsrPmemAlloc(sizeof(*prim));
    prim->type = CSR_TLS_CONNECTION_CERT_RES;
    prim->conn = conn;
    prim->accepted = accepted;

    return prim;
}

CsrTlsConnectionDataReq *CsrTlsConnectionDataReq_struct(
    CsrTlsConnection conn,
    CsrUint8 *buf,
    CsrSize buflen)
{
    CsrTlsConnectionDataReq *prim;

    prim = CsrPmemAlloc(sizeof(*prim));
    prim->type = CSR_TLS_CONNECTION_DATA_REQ;
    prim->conn = conn;
    prim->bufLen = buflen;
    prim->buf = buf;

    return prim;
}

CsrTlsConnectionDataRes *CsrTlsConnectionDataRes_struct(
    CsrTlsConnection conn)
{
    CsrTlsConnectionDataRes *prim;

    prim = CsrPmemAlloc(sizeof(*prim));
    prim->type = CSR_TLS_CONNECTION_DATA_RES;
    prim->conn = conn;

    return prim;
}
