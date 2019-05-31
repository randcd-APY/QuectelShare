#ifndef CSR_TLS_PRIM_H__
#define CSR_TLS_PRIM_H__
/*****************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/
#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_prim_defs.h"
#include "csr_sched.h"
#include "csr_result.h"

#include "csr_ip_socket_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrTlsPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrTlsPrim;

typedef void *CsrTlsSession;
typedef void *CsrTlsConnection;

/* TLS specific error codes. */
#define CSR_TLS_RESULT_INVALID_HANDLE               ((CsrResult) 0x01)
#define CSR_TLS_RESULT_NO_MORE_SOCKETS              ((CsrResult) 0x02)
#define CSR_TLS_RESULT_PORT_IN_USE                  ((CsrResult) 0x03)
#define CSR_TLS_RESULT_IP6_NOT_SUPPORTED            ((CsrResult) 0x04)

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_TLS_PRIM_DOWNSTREAM_LOWEST                               (0x0000)

#define CSR_TLS_SESSION_CREATE_REQ          ((CsrTlsPrim) (0x0000 + CSR_TLS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_TLS_SESSION_DESTROY_REQ         ((CsrTlsPrim) (0x0001 + CSR_TLS_PRIM_DOWNSTREAM_LOWEST))

#define CSR_TLS_CONFIG_CIPHER_REQ           ((CsrTlsPrim) (0x0002 + CSR_TLS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_TLS_CONFIG_CERT_CLIENT_REQ      ((CsrTlsPrim) (0x0003 + CSR_TLS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_TLS_CONFIG_CERT_VERIFY_REQ      ((CsrTlsPrim) (0x0004 + CSR_TLS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_TLS_CONFIG_COMPRESSION_REQ      ((CsrTlsPrim) (0x0005 + CSR_TLS_PRIM_DOWNSTREAM_LOWEST))

#define CSR_TLS_CONNECTION_CREATE_REQ       ((CsrTlsPrim) (0x0006 + CSR_TLS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_TLS_CONNECTION_DESTROY_REQ      ((CsrTlsPrim) (0x0007 + CSR_TLS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_TLS_CONNECTION_BIND_REQ         ((CsrTlsPrim) (0x0008 + CSR_TLS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_TLS_CONNECTION_CONNECT_REQ      ((CsrTlsPrim) (0x0009 + CSR_TLS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_TLS_CONNECTION_DISCONNECT_REQ   ((CsrTlsPrim) (0x000a + CSR_TLS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_TLS_CONNECTION_DISCONNECT_RES   ((CsrTlsPrim) (0x000b + CSR_TLS_PRIM_DOWNSTREAM_LOWEST))

#define CSR_TLS_CONNECTION_CERT_RES         ((CsrTlsPrim) (0x000c + CSR_TLS_PRIM_DOWNSTREAM_LOWEST))

#define CSR_TLS_CONNECTION_DATA_REQ         ((CsrTlsPrim) (0x000d + CSR_TLS_PRIM_DOWNSTREAM_LOWEST))
#define CSR_TLS_CONNECTION_DATA_RES         ((CsrTlsPrim) (0x000e + CSR_TLS_PRIM_DOWNSTREAM_LOWEST))

#define CSR_TLS_PRIM_DOWNSTREAM_HIGHEST     (0x000e + CSR_TLS_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_TLS_PRIM_UPSTREAM_LOWEST        (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_TLS_SESSION_CREATE_CFM          ((CsrTlsPrim) (0x0000 + CSR_TLS_PRIM_UPSTREAM_LOWEST))
#define CSR_TLS_SESSION_DESTROY_CFM         ((CsrTlsPrim) (0x0001 + CSR_TLS_PRIM_UPSTREAM_LOWEST))

#define CSR_TLS_CONFIG_CIPHER_CFM           ((CsrTlsPrim) (0x0002 + CSR_TLS_PRIM_UPSTREAM_LOWEST))
#define CSR_TLS_CONFIG_CERT_CLIENT_CFM      ((CsrTlsPrim) (0x0003 + CSR_TLS_PRIM_UPSTREAM_LOWEST))
#define CSR_TLS_CONFIG_CERT_VERIFY_CFM      ((CsrTlsPrim) (0x0004 + CSR_TLS_PRIM_UPSTREAM_LOWEST))
#define CSR_TLS_CONFIG_COMPRESSION_CFM      ((CsrTlsPrim) (0x0005 + CSR_TLS_PRIM_UPSTREAM_LOWEST))

#define CSR_TLS_CONNECTION_CREATE_CFM       ((CsrTlsPrim) (0x0006 + CSR_TLS_PRIM_UPSTREAM_LOWEST))
#define CSR_TLS_CONNECTION_DESTROY_CFM      ((CsrTlsPrim) (0x0007 + CSR_TLS_PRIM_UPSTREAM_LOWEST))
#define CSR_TLS_CONNECTION_BIND_CFM         ((CsrTlsPrim) (0x0008 + CSR_TLS_PRIM_UPSTREAM_LOWEST))
#define CSR_TLS_CONNECTION_CONNECT_CFM      ((CsrTlsPrim) (0x0009 + CSR_TLS_PRIM_UPSTREAM_LOWEST))
#define CSR_TLS_CONNECTION_DISCONNECT_CFM   ((CsrTlsPrim) (0x000a + CSR_TLS_PRIM_UPSTREAM_LOWEST))
#define CSR_TLS_CONNECTION_DISCONNECT_IND   ((CsrTlsPrim) (0x000b + CSR_TLS_PRIM_UPSTREAM_LOWEST))

#define CSR_TLS_CONNECTION_CERT_IND         ((CsrTlsPrim) (0x000c + CSR_TLS_PRIM_UPSTREAM_LOWEST))

#define CSR_TLS_CONNECTION_DATA_CFM         ((CsrTlsPrim) (0x000d + CSR_TLS_PRIM_UPSTREAM_LOWEST))
#define CSR_TLS_CONNECTION_DATA_IND         ((CsrTlsPrim) (0x000e + CSR_TLS_PRIM_UPSTREAM_LOWEST))


#define CSR_TLS_PRIM_UPSTREAM_HIGHEST       (0x000e + CSR_TLS_PRIM_UPSTREAM_LOWEST)

#define CSR_TLS_PRIM_DOWNSTREAM_COUNT       (CSR_TLS_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_TLS_PRIM_DOWNSTREAM_LOWEST)
#define CSR_TLS_PRIM_UPSTREAM_COUNT         (CSR_TLS_PRIM_UPSTREAM_HIGHEST + 1 - CSR_TLS_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrTlsPrim  type;
    CsrSchedQid qid;
} CsrTlsSessionCreateReq;

typedef struct
{
    CsrTlsPrim    type;
    CsrTlsSession session;
    CsrResult     result;
} CsrTlsSessionCreateCfm;

typedef struct
{
    CsrTlsPrim    type;
    CsrTlsSession session;
} CsrTlsSessionDestroyReq;

typedef struct
{
    CsrTlsPrim    type;
    CsrTlsSession session;
    CsrResult     result;
} CsrTlsSessionDestroyCfm;

#define CSR_TLS_NULL_WITH_NULL_NULL             0x0000

#define CSR_TLS_RSA_WITH_NULL_MD5               0x0001
#define CSR_TLS_RSA_WITH_NULL_SHA               0x0002
#define CSR_TLS_RSA_WITH_NULL_SHA256            0x003B
#define CSR_TLS_RSA_WITH_RC4_128_MD5            0x0004
#define CSR_TLS_RSA_WITH_RC4_128_SHA            0x0005
#define CSR_TLS_RSA_WITH_3DES_EDE_CBC_SHA       0x000A
#define CSR_TLS_RSA_WITH_AES_128_CBC_SHA        0x002F
#define CSR_TLS_RSA_WITH_AES_128_CBC_SHA256     0x003C
#define CSR_TLS_RSA_WITH_AES_256_CBC_SHA        0x0035
#define CSR_TLS_RSA_WITH_AES_256_CBC_SHA256     0x003D


#define CSR_TLS_DH_DSS_WITH_3DES_EDE_CBC_SHA    0x000D
#define CSR_TLS_DH_DSS_WITH_AES_128_CBC_SHA     0x0030
#define CSR_TLS_DH_DSS_WITH_AES_128_CBC_SHA256  0x003E
#define CSR_TLS_DH_DSS_WITH_AES_256_CBC_SHA     0x0036
#define CSR_TLS_DH_DSS_WITH_AES_256_CBC_SHA256  0x0068
#define CSR_TLS_DH_RSA_WITH_3DES_EDE_CBC_SHA    0x0010
#define CSR_TLS_DH_RSA_WITH_AES_128_CBC_SHA     0x0031
#define CSR_TLS_DH_RSA_WITH_AES_128_CBC_SHA256  0x003F
#define CSR_TLS_DH_RSA_WITH_AES_256_CBC_SHA     0x0037
#define CSR_TLS_DH_RSA_WITH_AES_256_CBC_SHA256  0x0069

#define CSR_TLS_DHE_DSS_WITH_3DES_EDE_CBC_SHA   0x0013
#define CSR_TLS_DHE_DSS_WITH_AES_128_CBC_SHA    0x0032
#define CSR_TLS_DHE_DSS_WITH_AES_128_CBC_SHA256 0x0040
#define CSR_TLS_DHE_DSS_WITH_AES_256_CBC_SHA    0x0038
#define CSR_TLS_DHE_DSS_WITH_AES_256_CBC_SHA256 0x006A
#define CSR_TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA   0x0016
#define CSR_TLS_DHE_RSA_WITH_AES_128_CBC_SHA    0x0033
#define CSR_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256 0x0067
#define CSR_TLS_DHE_RSA_WITH_AES_256_CBC_SHA    0x0039
#define CSR_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256 0x006B

#define CSR_TLS_DH_anon_WITH_3DES_EDE_CBC_SHA   0x001B
#define CSR_TLS_DH_anon_WITH_AES_128_CBC_SHA    0x0034
#define CSR_TLS_DH_anon_WITH_AES_128_CBC_SHA256 0x006C
#define CSR_TLS_DH_anon_WITH_AES_256_CBC_SHA    0x003A
#define CSR_TLS_DH_anon_WITH_AES_256_CBC_SHA256 0x006D
#define CSR_TLS_DH_anon_WITH_RC4_128_MD5        0x0018

#define CSR_TLS_PSK_WITH_RC4_128_SHA            0x008A
#define CSR_TLS_PSK_WITH_3DES_EDE_CBC_SHA       0x008B
#define CSR_TLS_PSK_WITH_AES_128_CBC_SHA        0x008C
#define CSR_TLS_PSK_WITH_AES_256_CBC_SHA        0x008D
#define CSR_TLS_DHE_PSK_WITH_RC4_128_SHA        0x008E
#define CSR_TLS_DHE_PSK_WITH_3DES_EDE_CBC_SHA   0x008F
#define CSR_TLS_DHE_PSK_WITH_AES_128_CBC_SHA    0x0090
#define CSR_TLS_DHE_PSK_WITH_AES_256_CBC_SHA    0x0091
#define CSR_TLS_RSA_PSK_WITH_RC4_128_SHA        0x0092
#define CSR_TLS_RSA_PSK_WITH_3DES_EDE_CBC_SHA   0x0093
#define CSR_TLS_RSA_PSK_WITH_AES_128_CBC_SHA    0x0094
#define CSR_TLS_RSA_PSK_WITH_AES_256_CBC_SHA    0x0095

typedef struct
{
    CsrTlsPrim    type;
    CsrTlsSession session;
    CsrUint16    *cipherSuite;
    CsrUint16     cipherSuiteLength;
    CsrUint16     keybitsMin;
    CsrUint16     keybitsMax;
} CsrTlsConfigCipherReq;

typedef struct
{
    CsrTlsPrim    type;
    CsrTlsSession session;
    CsrResult     result;
} CsrTlsConfigCipherCfm;

#define CSR_TLS_CERTIFICATE 0
#define CSR_TLS_PRIVATE_KEY 1
#define CSR_TLS_PSK         2

typedef struct
{
    CsrTlsPrim    type;
    CsrTlsSession session;
    CsrUint8      certificateType;
    CsrSize       certificateLength;
    CsrUint8     *certificate;
    CsrSize       passwordLength;
    CsrUint8     *password;
} CsrTlsConfigCertClientReq;

typedef struct
{
    CsrTlsPrim    type;
    CsrTlsSession session;
    CsrResult     result;
} CsrTlsConfigCertClientCfm;

typedef struct
{
    CsrTlsPrim    type;
    CsrTlsSession session;
    CsrUint8     *trustedCAcertificate;
    CsrSize       trustedCAcertificateLength;
} CsrTlsConfigCertVerifyReq;

typedef struct
{
    CsrTlsPrim    type;
    CsrTlsSession session;
    CsrResult     result;
} CsrTlsConfigCertVerifyCfm;

typedef struct
{
    CsrTlsPrim    type;
    CsrTlsSession session;
    CsrUint8      compression;
} CsrTlsConfigCompressionReq;

typedef struct
{
    CsrTlsPrim    type;
    CsrTlsSession session;
    CsrResult     result;
} CsrTlsConfigCompressionCfm;

typedef struct
{
    CsrTlsPrim        type;
    CsrTlsSession     session;
    CsrIpSocketFamily socketFamily;
    CsrCharString    *serverName;
} CsrTlsConnectionCreateReq;

typedef struct
{
    CsrTlsPrim       type;
    CsrTlsSession    session;
    CsrResult        result;
    CsrTlsConnection conn;
} CsrTlsConnectionCreateCfm;

typedef struct
{
    CsrTlsPrim       type;
    CsrTlsConnection conn;
} CsrTlsConnectionDestroyReq;

typedef struct
{
    CsrTlsPrim       type;
    CsrTlsConnection conn;
    CsrResult        result;
} CsrTlsConnectionDestroyCfm;

typedef struct
{
    CsrTlsPrim       type;
    CsrTlsConnection conn;
    CsrUint8         ipAddress[16];
    CsrUint16        port;
} CsrTlsConnectionBindReq;

typedef struct
{
    CsrTlsPrim       type;
    CsrTlsConnection conn;
    CsrResult        result;
} CsrTlsConnectionBindCfm;

typedef struct
{
    CsrTlsPrim       type;
    CsrTlsConnection conn;
    CsrUint8         ipAddress[16];
    CsrUint16        port;
} CsrTlsConnectionConnectReq;

typedef struct
{
    CsrTlsPrim       type;
    CsrTlsConnection conn;
    CsrResult        result;
} CsrTlsConnectionConnectCfm;

typedef struct
{
    CsrTlsPrim       type;
    CsrTlsConnection conn;
} CsrTlsConnectionDisconnectReq;

typedef struct
{
    CsrTlsPrim       type;
    CsrTlsConnection conn;
    CsrResult        result;
} CsrTlsConnectionDisconnectCfm;

typedef struct
{
    CsrTlsPrim       type;
    CsrTlsConnection conn;
} CsrTlsConnectionDisconnectInd;

typedef struct
{
    CsrTlsPrim       type;
    CsrTlsConnection conn;
} CsrTlsConnectionDisconnectRes;

/* CSR_RESULT_SUCCESS */
#define CSR_TLS_CERT_NOT_YET_VALID                          0x0009
#define CSR_TLS_CERT_HAS_EXPIRED                            0x0010
#define CSR_TLS_CERT_SELFSIGNED                             0x0018
#define CSR_TLS_CERT_MISSING_ISSUER_CERT                    0x0020
#define CSR_TLS_CERT_UNABLE_TO_VERIFY_NO_SELFSIGNED_CERT    0x0021
#define CSR_TLS_CERT_CA_INVALID                             0x0024
#define CSR_TLS_CERT_CN_MISMATCH                            0x0666
/* CSR_RESULT_FAILURE */

typedef struct
{
    CsrTlsPrim       type;
    CsrTlsConnection conn;
    CsrSize          certLen;
    CsrUint8        *cert;
    CsrResult        result;
} CsrTlsConnectionCertInd;

typedef struct
{
    CsrTlsPrim       type;
    CsrTlsConnection conn;
    CsrBool          accepted;
} CsrTlsConnectionCertRes;

typedef struct
{
    CsrTlsPrim       type;
    CsrTlsConnection conn;
} CsrTlsConnectionPasswdreqInd;

typedef struct
{
    CsrTlsPrim       type;
    CsrTlsConnection conn;
    CsrSize          pwdLen;
    CsrUint8        *pwd;
} CsrTlsConnectionPasswdreqRes;

typedef struct
{
    CsrTlsPrim       type;
    CsrTlsConnection conn;
    CsrSize          bufLen;
    CsrUint8        *buf;
} CsrTlsConnectionDataReq;

typedef struct
{
    CsrTlsPrim       type;
    CsrTlsConnection conn;
    CsrResult        result;
} CsrTlsConnectionDataCfm;

typedef struct
{
    CsrTlsPrim       type;
    CsrTlsConnection conn;
    CsrSize          bufLen;
    CsrUint8        *buf;
} CsrTlsConnectionDataInd;

typedef struct
{
    CsrTlsPrim       type;
    CsrTlsConnection conn;
} CsrTlsConnectionDataRes;

#ifdef __cplusplus
}
#endif

#endif /* CSR_TLS_PRIM_H__ */
