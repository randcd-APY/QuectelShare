/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_ssl.h
 *
 * @addtogroup qapi_networking_ssl
 * @{
 *
 * @brief This section describes the QAPIs used for Transport Layer Security
 *        (TLS) and the Secure Socket Layer (SSL).
 *
 * @details TLS/SSL is used to provide security and data integrity between two
 * peers communicating over TCP. After a TCP connection is established, the two
 * peers use a handshake mechanism to establish the keys used for
 * encryption/decryption and data verification. Once the handshake is
 * successful, data can be transmitted/received over the SSL connection.
 *
 * @}
 */
#ifndef _QAPI_SSL_H_
#define _QAPI_SSL_H_

#include <stdint.h>
#include <qapi/qapi_net_status.h>
#include <qapi/qapi_ssl_cert.h>

struct sockaddr;

/** @addtogroup qapi_networking_ssl
@{ */


/** Maximum number of cipher suites that can be configured. */
#define QAPI_NET_SSL_CIPHERSUITE_LIST_DEPTH 8

/** Invalid handle. */
#define QAPI_NET_SSL_INVALID_HANDLE (0)

/*
 * SSL protocol version, to be used with qapi_Net_SSL_Configure.
 * SSL 3.0 is deprecated.
 */

/** Unknown SSL protocol version. */
#define QAPI_NET_SSL_PROTOCOL_UNKNOWN                  0x00

/** TLS version 1.0. */
#define QAPI_NET_SSL_PROTOCOL_TLS_1_0                  0x31

/** TLS version 1.1. */
#define QAPI_NET_SSL_PROTOCOL_TLS_1_1                  0x32

/** TLS version 1.2. */
#define QAPI_NET_SSL_PROTOCOL_TLS_1_2                  0x33

/** DTLS version 1.0. */
#define QAPI_NET_SSL_PROTOCOL_DTLS_1_0                 0xEF

/** DTLS version 1.2. */
#define QAPI_NET_SSL_PROTOCOL_DTLS_1_2                 0xED

/*
 * SSL cipher suites. To be used with qapi_Net_SSL_Configure
 */

/** Cipher TLS_RSA_WITH_AES_128_CBC_SHA. */
#define QAPI_NET_TLS_RSA_WITH_AES_128_CBC_SHA               0x002F
/** Cipher TLS_DHE_RSA_WITH_AES_128_CBC_SHA. */
#define QAPI_NET_TLS_DHE_RSA_WITH_AES_128_CBC_SHA           0x0033
/** Cipher TLS_RSA_WITH_AES_256_CBC_SHA. */
#define QAPI_NET_TLS_RSA_WITH_AES_256_CBC_SHA               0x0035
/** Cipher TLS_DHE_RSA_WITH_AES_256_CBC_SHA. */
#define QAPI_NET_TLS_DHE_RSA_WITH_AES_256_CBC_SHA           0x0039
/** Cipher TLS_RSA_WITH_AES_128_CBC_SHA256. */
#define QAPI_NET_TLS_RSA_WITH_AES_128_CBC_SHA256            0x003C
/** Cipher TLS_RSA_WITH_AES_256_CBC_SHA256. */
#define QAPI_NET_TLS_RSA_WITH_AES_256_CBC_SHA256            0x003D
/** Cipher TLS_DHE_RSA_WITH_AES_128_CBC_SHA256. */
#define QAPI_NET_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256        0x0067
/** Cipher TLS_DHE_RSA_WITH_AES_256_CBC_SHA256. */
#define QAPI_NET_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256        0x006B
/** Cipher TLS_RSA_WITH_AES_128_GCM_SHA256. */
#define QAPI_NET_TLS_RSA_WITH_AES_128_GCM_SHA256            0x009C
/** Cipher TLS_RSA_WITH_AES_256_GCM_SHA384. */
#define QAPI_NET_TLS_RSA_WITH_AES_256_GCM_SHA384            0x009D
/** Cipher TLS_DHE_RSA_WITH_AES_128_GCM_SHA256. */
#define QAPI_NET_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256        0x009E
/** Cipher TLS_DHE_RSA_WITH_AES_256_GCM_SHA384. */
#define QAPI_NET_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384        0x009F
/** Cipher TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA. */
#define QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA        0xC004
/** Cipher TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA. */
#define QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA        0xC005
/** Cipher TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA. */
#define QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA       0xC009
/** Cipher TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA. */
#define QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA       0xC00A
/** Cipher TLS_ECDH_RSA_WITH_AES_128_CBC_SHA. */
#define QAPI_NET_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA          0xC00E
/** Cipher TLS_ECDH_RSA_WITH_AES_256_CBC_SHA. */
#define QAPI_NET_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA          0xC00F
/** Cipher TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA. */
#define QAPI_NET_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA         0xC013
/** Cipher TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA. */
#define QAPI_NET_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA         0xC014
/** Cipher TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256. */
#define QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256    0xC023
/** Cipher TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384. */
#define QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384    0xC024
/** Cipher TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256. */
#define QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256     0xC025
/** Cipher TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384. */
#define QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384     0xC026
/** Cipher TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256. */
#define QAPI_NET_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256      0xC027
/** Cipher TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384. */
#define QAPI_NET_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384      0xC028
/** Cipher TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256. */
#define QAPI_NET_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256       0xC029
/** Cipher TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384. */
#define QAPI_NET_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384       0xC02A
/** Cipher TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256. */
#define QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256    0xC02B
/** Cipher TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384. */
#define QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384    0xC02C
/** Cipher TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256. */
#define QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256     0xC02D
/** Cipher TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384. */
#define QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384     0xC02E
/** Cipher TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256. */
#define QAPI_NET_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256      0xC02F
/** Cipher TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384. */
#define QAPI_NET_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384      0xC030
/** Cipher TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256. */
#define QAPI_NET_TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256       0xC031
/** Cipher TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384. */
#define QAPI_NET_TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384       0xC032
/** Cipher TLS_RSA_WITH_AES_128_CCM. */
#define QAPI_NET_TLS_RSA_WITH_AES_128_CCM                   0xC09C
/** Cipher TLS_RSA_WITH_AES_256_CCM. */
#define QAPI_NET_TLS_RSA_WITH_AES_256_CCM                   0xC09D
/** Cipher TLS_DHE_RSA_WITH_AES_128_CCM. */
#define QAPI_NET_TLS_DHE_RSA_WITH_AES_128_CCM               0xC09E
/** Cipher TLS_DHE_RSA_WITH_AES_256_CCM. */
#define QAPI_NET_TLS_DHE_RSA_WITH_AES_256_CCM               0xC09F
/** Cipher TLS_RSA_WITH_AES_128_CCM_8. */
#define QAPI_NET_TLS_RSA_WITH_AES_128_CCM_8                 0xC0A0
/** Cipher TLS_RSA_WITH_AES_256_CCM_8. */
#define QAPI_NET_TLS_RSA_WITH_AES_256_CCM_8                 0xC0A1
/** Cipher TLS_DHE_RSA_WITH_AES_128_CCM_8. */
#define QAPI_NET_TLS_DHE_RSA_WITH_AES_128_CCM_8             0xC0A2
/** Cipher TLS_DHE_RSA_WITH_AES_256_CCM_8. */
#define QAPI_NET_TLS_DHE_RSA_WITH_AES_256_CCM_8             0xC0A3
/** Cipher TLS_PSK_WITH_AES_128_CCM_8. */
#define QAPI_NET_TLS_PSK_WITH_AES_128_CCM_8                    0xC0A8
/** Cipher TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256. */
#define QAPI_NET_TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256   0xCC13
/** Cipher TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256. */
#define QAPI_NET_TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256 0xCC14
/** Cipher TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256. */
#define QAPI_NET_TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256     0xCC15
/** Cipher TLS_ECJPAKE_WITH_AES_128_CCM_8. */
#define QAPI_NET_TLS_ECJPAKE_WITH_AES_128_CCM_8             0xC0FF

/** Cipher TLS_PSK_WITH_AES_128_GCM_SHA256. */
#define QAPI_NET_TLS_PSK_WITH_AES_128_GCM_SHA256               0x00A8
/** Cipher TLS_PSK_WITH_AES_256_GCM_SHA384. */
#define QAPI_NET_TLS_PSK_WITH_AES_256_GCM_SHA384               0x00A9
/** Cipher TLS_PSK_WITH_AES_128_CBC_SHA256. */
#define QAPI_NET_TLS_PSK_WITH_AES_128_CBC_SHA256               0x00AE
/** Cipher TLS_PSK_WITH_AES_256_CBC_SHA384. */
#define QAPI_NET_TLS_PSK_WITH_AES_256_CBC_SHA384               0x00AF
/** Cipher TLS_PSK_WITH_AES_128_CBC_SHA. */
#define QAPI_NET_TLS_PSK_WITH_AES_128_CBC_SHA                  0x008C
/** Cipher TLS_PSK_WITH_AES_256_CBC_SHA. */
#define QAPI_NET_TLS_PSK_WITH_AES_256_CBC_SHA                  0x008D

/** Cipher TLS_PSK_WITH_AES_256_CBC_SHA. */
#define QAPI_NET_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256         0xC037

/** QAPI_NET_TLS_ALERT_CLOSE_NOTIFY */
#define QAPI_NET_TLS_ALERT_CLOSE_NOTIFY                0
/** QAPI_NET_TLS_ALERT_UNEXPECTED_MESSAGE */
#define QAPI_NET_TLS_ALERT_UNEXPECTED_MESSAGE          10
/** QAPI_NET_TLS_ALERT_BAD_RECORD_MAC */
#define QAPI_NET_TLS_ALERT_BAD_RECORD_MAC              20
/** QAPI_NET_TLS_ALERT_DECRYPTION_FAILED */
#define QAPI_NET_TLS_ALERT_DECRYPTION_FAILED           21
/** QAPI_NET_TLS_ALERT_RECORD_OVERFLOW */
#define QAPI_NET_TLS_ALERT_RECORD_OVERFLOW             22
/** QAPI_NET_TLS_ALERT_DECOMPRESSION_FAILURE */
#define QAPI_NET_TLS_ALERT_DECOMPRESSION_FAILURE       30
/** QAPI_NET_TLS_ALERT_HANDSHAKE_FAILURE */
#define QAPI_NET_TLS_ALERT_HANDSHAKE_FAILURE           40
/** QAPI_NET_TLS_ALERT_NO_CERTIFICATE */
#define QAPI_NET_TLS_ALERT_NO_CERTIFICATE              41  /* SSL 3.0 only */
/** QAPI_NET_TLS_ALERT_BAD_CERTIFICATE */
#define QAPI_NET_TLS_ALERT_BAD_CERTIFICATE             42
/** QAPI_NET_TLS_ALERT_UNSUPPORTED_CERTIFICATE */
#define QAPI_NET_TLS_ALERT_UNSUPPORTED_CERTIFICATE     43
/** QAPI_NET_TLS_ALERT_CERTIFICATE_REVOKED */
#define QAPI_NET_TLS_ALERT_CERTIFICATE_REVOKED         44
/** QAPI_NET_TLS_ALERT_CERTIFICATE_EXPIRED */
#define QAPI_NET_TLS_ALERT_CERTIFICATE_EXPIRED         45
/** QAPI_NET_TLS_ALERT_CERTIFICATE_UNKNOWN */
#define QAPI_NET_TLS_ALERT_CERTIFICATE_UNKNOWN         46
/** QAPI_NET_TLS_ALERT_ILLEGAL_PARAMETER */
#define QAPI_NET_TLS_ALERT_ILLEGAL_PARAMETER           47
/** QAPI_NET_TLS_ALERT_UNKNOWN_CA */
#define QAPI_NET_TLS_ALERT_UNKNOWN_CA                  48
/** QAPI_NET_TLS_ALERT_ACCESS_DENIED */
#define QAPI_NET_TLS_ALERT_ACCESS_DENIED               49
/** QAPI_NET_TLS_ALERT_DECODE_ERROR */
#define QAPI_NET_TLS_ALERT_DECODE_ERROR                50
/** QAPI_NET_TLS_ALERT_DECRYPT_ERROR */
#define QAPI_NET_TLS_ALERT_DECRYPT_ERROR               51
/** QAPI_NET_TLS_ALERT_EXPORT_RESTRICTION */
#define QAPI_NET_TLS_ALERT_EXPORT_RESTRICTION          60
/** QAPI_NET_TLS_ALERT_PROTOCOL_VERSION */
#define QAPI_NET_TLS_ALERT_PROTOCOL_VERSION            70
/** QAPI_NET_TLS_ALERT_INSUFFICIENT_SECURITY */
#define QAPI_NET_TLS_ALERT_INSUFFICIENT_SECURITY       71
/** QAPI_NET_TLS_ALERT_INTERNAL_ERROR */
#define QAPI_NET_TLS_ALERT_INTERNAL_ERROR              80
/** QAPI_NET_TLS_ALERT_USER_CANCELED */
#define QAPI_NET_TLS_ALERT_USER_CANCELED               90
/** QAPI_NET_TLS_ALERT_NO_RENEGOTIATION */
#define QAPI_NET_TLS_ALERT_NO_RENEGOTIATION            100
/** QAPI_NET_TLS_ALERT_UNKNOWN_PSK_IDENTITY */
#define QAPI_NET_TLS_ALERT_UNKNOWN_PSK_IDENTITY        115
/** QAPI_NET_TLS_ALERT_NO_APPLICATION_PROTOCOL */
#define QAPI_NET_TLS_ALERT_NO_APPLICATION_PROTOCOL     120

/** SSL/TLS warning message */
#define QAPI_NET_TLS_ALERT_LEVEL_WARNING               1

/** Fatal error message. Connection must be closed. */
#define QAPI_NET_TLS_ALERT_LEVEL_FATAL                 2

/*
 * Please refer to qapi_net_status.h for SSL QAPI return values.
 * See qapi_status.h for common error codes.
 */

/**
 * @brief Structure to specify the certificate verification policy.
 */
typedef struct __qapi_Net_SSL_Verify_Policy_s {
    uint8_t domain ;
    /**< TRUE to verify certificate commonName against the peer's domain name. */

    uint8_t time_Validity ;
    /**< TRUE to verify certificate time validity. */

    uint8_t send_Alert ;
    /**< TRUE to immediately send a fatal alert on detection of an untrusted certificate. */

    char match_Name[QAPI_NET_SSL_MAX_CERT_NAME_LEN];
    /**< Name to match against the common name or altDNSNames of a certificate. */
}  qapi_Net_SSL_Verify_Policy_t;

/**
 * @brief Structure to configure an SSL connection. 
 */
typedef  struct __qapi_Net_SSL_Config_s {
    uint16_t protocol ;
    /**< Protocol to use. See QAPI_NET_SSL_PROTOCOL_*. */

    uint64_t set_Of_Selected_Ciphers_As_Bitmap;
    /**< Set of ciphers to use. This is populated via the qapi_Net_SSL_Cipher_Add() function. */

    qapi_Net_SSL_Verify_Policy_t verify ;
    /**< Certificate verification policy. */

    uint16_t max_Frag_Len;
    /**< Maximum fragment length in bytes. */

    uint16_t max_Frag_Len_Neg_Disable;
    /**< Is maximum fragment length negotiation allowed? See RFC 6066. */

    uint16_t sni_Name_Size;
    /**< Length of the SNI server name. */

    char *sni_Name;
    /**< Server name for SNI. */

    void *alpn;
    /**< ALPN parameters. */

    void *extensions;
    /**< Additional extensions. */
}  qapi_Net_SSL_Config_t;

/**
 * @brief Structure to configure Password Authenticated
 * Key Exchange (PAKE) ciphers. Currently, ECJAPKE is
 * the only supported PAKE cipher.
 */
typedef  struct __qapi_Net_SSL_ECJPAKE_Params_s {
	uint8_t *password;
    /**< Password used for ECJPAKE. */

    uint32_t password_Size;
    /**< Length of the password field. */

    uint8_t *identity;
    /**< Identity used for ECJPAKE. For future use, currently not supported. */

    uint32_t identity_Size;
    /**< Length of the identity field. For future use, currently not supported. */

    uint16_t ecjpake_Curve_Id;
    /**< Curve ID used by ECJPAKE. QAPI_CRYPTO_ECC_CURVE_NIST_P256 is the only curve currently supported. */

} qapi_Net_SSL_ECJPAKE_Params_t;

/**
 * @brief SSL object role.
 */
typedef enum
{
  QAPI_NET_SSL_SERVER_E = 1,
  /**< Server role. */

  QAPI_NET_SSL_CLIENT_E = 2
  /**< Client role. */
} qapi_Net_SSL_Role_t;

/**
 * @brief SSL protocol.
 */
typedef enum
{
  QAPI_NET_SSL_TLS_E  = 1,
  /**< TLS protocol. */

  QAPI_NET_SSL_DTLS_E = 2,
  /**< DTLS protocol. */
} qapi_Net_SSL_Protocol_t;

/**
 * @brief SSL alert information.
 */
typedef struct qapi_Net_SSL_Alert_Info_s
{
	qbool_t alert_Set;
	/**< True if alert level and type set.  False if no alert sent or received. */

	uint8_t alert_Level;
	/**< TLS alert level (see QAPI_NET_TLS_ALERT_LEVEL_* ). */

	uint32_t alert_Description;
	/**< TLS alert type (see QAPI_NET_TLS_ALERT_* ). */

} qapi_Net_SSL_Alert_Info_t;

/**
 * Handle to an SSL connection.
 * This is obtained from a call to qapi_Net_SSL_Con_New(). The handle is
 * freed with a call to qapi_Net_SSL_Shutdown().
 */
typedef uint32_t qapi_Net_SSL_Con_Hdl_t;

/**
* @brief SSL keys provided to the key export callback.
*/
typedef struct {
    const uint8_t *master_Secret;
    /**< SSL session master secret. */
    uint32_t  master_Secret_Length;
    /**< Master secret length. */
    const uint8_t *key_Block;
    /**< SSL key block (defined in section 6.3 of RFC 5246). */
    uint32_t key_Block_Length;
    /**< Key block length.  */
} qapi_Net_SSL_Key_Data;

/**
* @brief SSL key export callback.
*/
typedef void (*qapi_Net_SSL_Key_Export_CB_t)(
              qapi_Net_SSL_Con_Hdl_t handle,
			  /**< SSL connection handle. */

              const struct sockaddr *peer_Addr,
			  /**< IP address and port of the SSL peer. */

              int peer_Addr_Length,
			  /**< Length of the peer address (length depends on whether the address is IPv4 or IPv6). */

              qapi_Net_SSL_Key_Data *key_Data,
			  /**< Exported keys. */

              void *arg
			  /**< User defined data. */
);

/**
 * @brief Creates a new SSL object (server or client). 
 * This function must be called before using any other SSL function.
 *  
 * @param[in] role  Server or client role.
 *
 * @return
 * SSL object handle on success. \n
 * QAPI_NET_SSL_HDL_NULL on error (out of memory).
 */
qapi_Net_SSL_Obj_Hdl_t qapi_Net_SSL_Obj_New(qapi_Net_SSL_Role_t role);

/**
 * @brief Creates an SSL connection handle for an SSL object.
 *
 * @param[in] hdl   SSL object handle.
 * @param[in] prot  Protocol to be used for this connection.
 *
 * @return
 * SSL connection handle on success. \n
 * QAPI_NET_SSL_HDL_NULL on error (out of memory).
 */
qapi_Net_SSL_Con_Hdl_t qapi_Net_SSL_Con_New(qapi_Net_SSL_Obj_Hdl_t hdl, qapi_Net_SSL_Protocol_t prot);

/**
 * @brief Adds a cipher to the SSL configuration.
 *
 * @details
 * This function is called to select a set of ciphers that should be enabled for
 * a particular SSL instance.  This function should be called once for each cipher that
 * is to be selected. Internally, this function sets a bit in the
 * cft->set_Of_Selected_Ciphers_As_Bitmap corresponding to the selected cipher.
 *
 * @param[in] cfg     Configuration parameters.
 * @param[in] cipher Code of the cipher to be selected.  Cipher codes are
 *                   found in the QAPI_NET_TLS_* defines.
 *
 * @return
 * QAPI_OK on success. \n
 * QAPI_ERR_INVALID_PARAM_SSL on error.
 */
qapi_Status_t qapi_Net_SSL_Cipher_Add(qapi_Net_SSL_Config_t * cfg, uint16_t cipher);

/**
 * @brief Configures an SSL connection regarding protocol and cipher, certificate
 * validation criteria, maximum fragment length, and disable fragment length
 * negotiation.
 *
 * @details 
 * The SSL protocol and selected ciphers can be configured in client context.
 *
 * The SSL_VERIFY_POLICY verify structure (and the matchName) specify how the
 * SSL certificate will be verified during the SSL handshake:
 * - If verify.domain = 1, the certificate domain name will be
 *   checked against matchName.
 * - If verify.timeValidity = 1, the certificate will be checked
 *   for expiration.
 *   - The certificate itself will always be checked against the CAList.  If a
 *   CAList is not present in the SSL context, the certificate is implicitly
 *   trusted.
 *   - If verify.sendAlert = 1, an SSL alert is sent if the certificate fails
 *   any of the tests. An error is also returned to the application, which should
 *   subsequently close the connection.  If verify.sendAlert = 0, an error will
 *   be returned by SSL_connect() and it is up to the application to decide
 *   what to do.
 *
 * In SSL, a smaller fragment length helps in efficient memory utilization and
 * to minimize latency. In client mode, a maximum fragment length of 1 KB is
 * negotiated during handshake using TLS extensions. If the peer server does not
 * support the extension, the default maximum size of 16 KB is used. 
 * 
 * SSL_configure provides two fields, max_frag_len and
 * max_frag_len_neg_disable to override the above behavior.
 * max_frag_len_neg_disable applies only in Client mode.
 *
 * If negotiation is allowed (i.e, max_frag_len_neg_disable = 0), max_frag_len
 * must be set to one of these four values, according to RFC 6066:
 * - 512
 * - 1024
 * - 2048
 * - 4096
 *
 * Other values are not permitted.
 *
 * max_frag_len is applicable in Client or Server mode. Server mode does not
 * support a maximum fragment length TLS extension.
 *
 * There can be scenarios where the peer does not support the maximum fragment length
 * TLS extension, but the maximum fragment length is known a-priori. In that case,
 * the user may choose to configure max_frag_len and set max_frag_len_neg_disable to
 * 1 to disable negotiation but still get the benefits of a smaller fragment
 * length. When negotiation is disabled, any value < 16 KB can be configured for
 * max_frag_len. The above limitations do not apply.
 *
 * An error is returned and the connection is closed if any incoming record exceeds
 * max_frag_len.
 *
 * Note that the set of ciphers can be explicitly selected for the SSL context
 * by calling the qapi_Net_SSL_Cipher_Add() function.
 *
 * @param[in] ssl   Connection handle.
 * @param[in] cfg   Configuration parameters.
 *
 * @return
 * QAPI_OK on success, \n
 * QAPI_ERR_INVALID_PARAM_SSL if an error occurred (configuration is invalid).
 */
qapi_Status_t qapi_Net_SSL_Configure(qapi_Net_SSL_Con_Hdl_t ssl, qapi_Net_SSL_Config_t *cfg);

/**
 * @brief Attaches a given socket descriptor to the SSL connection.
 *
 * @details The SSL connection will inherit the behavior of the socket
 * descriptor (zero-copy/nonzero-copy, blocking/nonblocking, etc.).
 *
 * @param[in] ssl   SSL connection handle.
 * @param[in] fd    Socket descriptor.
 *
 * @return
 * QAPI_OK on success, \n
 * QAPI_ERR_INVALID_PARAM_SSL on error.
 */
qapi_Status_t qapi_Net_SSL_Fd_Set(qapi_Net_SSL_Con_Hdl_t ssl, uint32_t fd);

/**
 * @brief Accepts an incoming SSL connection from the client.
 *
 * @details This should be called only by a server SSL object. This will respond
 * to the incoming client hello message and complete the SSL handshake. 
 *
 * @param[in] ssl   SSL connection handle.
 *
 * @return
 * QAPI_SSL_OK_HS on success. \n
 * QAPI_ERR_* on error.
 */
qapi_Status_t qapi_Net_SSL_Accept(qapi_Net_SSL_Con_Hdl_t ssl);

/**
 * @brief Initiates an SSL handshake. Called only by a client SSL object.
 *
 * @param[in] ssl   SSL connection handle.
 *
 * @return
 * QAPI_SSL_OK_HS on success. \n
 * QAPI_ERR_* on error.
 */
qapi_Status_t qapi_Net_SSL_Connect(qapi_Net_SSL_Con_Hdl_t ssl);

/**
 * @brief Closes an SSL connection.
 *
 * @details The connection handle is freed with this API. 
 * The socket must be closed explicitly after this call. See socket QAPIs.
 *
 * @param[in] ssl   SSL connection handle.
 *
 * @return
 * QAPI_OK on success. \n
 * QAPI_ERR_INVALID_PARAM_SSL on error (invalid connection handle).
 */
qapi_Status_t qapi_Net_SSL_Shutdown(qapi_Net_SSL_Con_Hdl_t ssl);

/**
 * @brief Frees the SSL object handle. 
 * All connections belonging to this handle must be closed prior to calling this
 * API. 
 *
 * @param[in] hdl    SSL object handle.
 *
 * @return
 * QAPI_OK on success, or QAPI error value on error.
 */
qapi_Status_t qapi_Net_SSL_Obj_Free(qapi_Net_SSL_Obj_Hdl_t hdl);

/**
 * @brief Reads data received over the SSL connection.
 *
 * @details
 * The SSL handshake must be completed successfully prior to calling this API. 
 * Depending on the underlying socket associated with the SSL connection, the
 * API will be blocking/nonblocking, zero-copy/nonzero-copy, etc.
 * The select API can be used to check if there is any data available.
 *
 * @param[in] hdl      Connection handle.
 * @param[in,out] buf  Buffer to hold received data. Must be allocated by the application.
 * @param[in] size     Size of the buffer in bytes.
 *
 * @return
 * The number of bytes available in the buffer. \n
 * QAPI_ERR_* on error. 
 */
int32_t qapi_Net_SSL_Read(qapi_Net_SSL_Con_Hdl_t hdl, void *buf, uint32_t size);

/**
 * @brief Sends data over the SSL connection.
 *
 * @details
 * The SSL handshake must be completed successfully prior to calling this API. 
 * Depending on the underlying socket associated with the SSL connection, the
 * API will be blocking/nonblocking, zero-copy/nonzero-copy, etc.
 *
 * @param[in] hdl    Connection handle.
 * @param[in] buf    Buffer with the data to send.
 * @param[in] size   Size of the buffer in bytes.
 *
 * @return
 * The number of bytes sent. \n
 * QAPI_ERR_* on error.
 */
int32_t qapi_Net_SSL_Write(qapi_Net_SSL_Con_Hdl_t hdl, void *buf, uint32_t size);

/**
 * @brief Close a DTLS client SSL connection object.
 *
 * @details
 * Closes an SSL connection object for a peer. This should be called
 * to explicitly reset a client connection from the application
 * layer. Used only for DTLS servers.
 *
 * @param[in] ssl   SSL connection handle.
 * @param[in] peer_Addr Address of the connection to reset.
 * @param[in] peer_Addr_Len Address length.
 *
 * @return QAPI_ERR_* on error.
 */
qapi_Status_t qapi_Net_SSL_Close_Client_Con(qapi_Net_SSL_Con_Hdl_t ssl, struct sockaddr *peer_Addr, int peer_Addr_Len);

/**
 * @brief Returns whether the client connection was closed by the server.
 * @details
 * Returns if the server closed a client connection with a fatal alert
 * or close notify.
 *
 * @param[in] hdl SSL object handle
 * @return 1 if the client connection was closed by peer (the server). \n
 *         0 if connection was not closed or is not a client connection.
 */
qapi_Status_t qapi_Net_SSL_Client_Closed_By_Peer(qapi_Net_SSL_Obj_Hdl_t hdl);

/**
 * @brief Reads data received over the SSL connection.
 *
 * @details
 * The SSL handshake must be completed successfully before calling this API.
 * Depending on the underlying socket associated with the SSL connection, the
 * API will be blocking/nonblocking, zero-copy/non-zero-copy, etc.
 * The selected API can be used to check whether there is any data available.
 *
 * @param[in] ssl SSL connection handle
 * @param[inout] buf Buffer to hold received data. Must be allocated by the application.
 * @param[in] num Size of the buffer in bytes.
 * @param[out] from From address.
 * @param[out] from_Len From address length.
 *
 * @return Number of bytes available in the buffer. \n
 *         QAPI_ERR_* on error.
 */
int32_t qapi_Net_SSL_Read_From(qapi_Net_SSL_Con_Hdl_t ssl, void *buf, uint32_t num,
		struct sockaddr *from, int32_t *from_Len);

/**
 * @brief Sends data over SSL connection.
 *
 * @details
 * The SSL handshake must be completed successfully before calling this API.
 * Depending on the underlying socket associated with the SSL connection, the
 * API will be blocking/nonblocking, zero-copy/non-zero-copy, etc.
 *
 * @param[in] ssl SSL connection handle.
 * @param[in] buf Buffer with data to send.
 * @param[in] num Size of the buffer in bytes.
 * @param[in] to To address.
 * @param[in] to_Len To address length.
 *
 * @return Number of bytes sent. \n
 *         QAPI_ERR_* on error.
 */
int32_t qapi_Net_SSL_Write_To(qapi_Net_SSL_Con_Hdl_t ssl, void *buf, uint32_t num,
		struct sockaddr *to, int32_t to_Len);


/**
 * @brief Returns the status of an SSL connection.
 * @details
 * Returns the status of an SSL connection when a nonblocking socket is used
 * and there is a need to poll the status of a connection.
 *
 * @param[in] ssl SSL connection handle.
 *
 * @return Connection status.
 */
qapi_Status_t qapi_Net_SSL_Con_Get_Status(qapi_Net_SSL_Con_Hdl_t ssl);

/**
 * @brief Gets the DTLS maximum payload size without causing fragmentation.
 *
 * @details
 * The SSL handshake must be completed successfully before calling this API.
 * Returns the maximum DTLS data payload size that can be sent by the application
 * without causing IP layer fragmentation.
 *
 * @param[in] hdl SSL client connection handle.
 *
 * @return The maximum DTLS payload size that can be sent without causing fragmentation
 *         at the IP layer.
 */
uint32_t qapi_Net_SSL_DTLS_Client_Get_Data_MTU(qapi_Net_SSL_Con_Hdl_t hdl);

/**
 * @brief Gets the DTLS maximum payload size without causing fragmentation.
 *
 * @details
 * The SSL handshake must be completed successfully before calling this API.
 * Returns the maximum DTLS data payload size that can be sent by the application
 * without causing IP layer fragmentation.
 *
 * @param[in] hdl SSL server connection handle.
 * @param[in] peer_addr Server peer address.
 * @param[in] peer_addr_len Server peer address length.
 *
 * @return The maximum DTLS payload size that can be sent without causing fragmentation
 *         at the IP layer.
 */
uint32_t qapi_Net_SSL_DTLS_Server_Get_Data_MTU(qapi_Net_SSL_Con_Hdl_t hdl, struct sockaddr *peer_addr, int peer_addr_len);


/**
 * @brief Returns the selected ALPN protocol for a given SSL connection.
 * @details
 * Returns the negotiated ALPN protocol for a given SSL connection after a
 * successful SSL handshake.
 *
 * @param[in] ssl SSL connection handle.
 *
 * @return Negotiated protocol string if the peer supports ALPN, NULL otherwise.
 */
const char* qapi_Net_SSL_ALPN_Protocol_Get(qapi_Net_SSL_Con_Hdl_t ssl);

/**
 * @brief Adds the given protocol to the list of supported ALPN protocols.
 * @details
 * Adds the given application layer protocol to the list of supported protocols.
 * The list of supported protocols will be published in the ALPN TLS extension.
 *
 * @param[in] hdl SSL object handle.
 * @param[in] protocol Protocol string to be added to the list of protocols.
 *
 * @return 0 for success or a nonzero value for failure.
 */
int qapi_Net_SSL_ALPN_Protocol_Add(qapi_Net_SSL_Obj_Hdl_t hdl, const char *protocol);

/**
 * @brief Returns the selected ALPN protocol for a given peer address.
 * @details
 * Returns the negotiated ALPN protocol for a given peer address after a
 * successful SSL handshake.
 *
 * @param[in] ssl SSL connection handle.
 * @param[in] peer_addr Server peer address.
 * @param[in] peer_addr_len Server peer address length.
 *
 * @return Negotiated protocol string if the peer supports ALPN, NULL otherwise.
 */
const char* qapi_Net_SSL_ALPN_Get_Protocol_For_Peer(qapi_Net_SSL_Con_Hdl_t ssl, struct sockaddr *peer_addr, int peer_addr_len);

/**
 * @brief Sets a key export callback.
 *
 * @details Sets a callback that gets the master secret and key block
 * for an SSL session.  IMPORTANT: the callback should keep its own
 * copy of the master secret and key block.  These are only valid
 * during the duration of the SSL handshake, after which point they are
 * freed.
 *
 * @param[in] hdl SSL object handle.
 * @param[in] key_Export_Callback Callback that receives the exported keys.
 * @param[in] arg Caller supplied data that is passed to the callback.
 *
 * @return QAPI_OK on success, or a QAPI error value on error.
 */
qapi_Status_t qapi_Net_SSL_Set_Key_Export_Callback(qapi_Net_SSL_Obj_Hdl_t hdl, qapi_Net_SSL_Key_Export_CB_t key_Export_Callback, void *arg);

/**
 * @brief Configures an idle timer for the DTLS server.
 *
 * @details If a client establishes a
 * DTLS connection and completes the handshake but does not send any traffic
 * within idle_Timeout seconds, the connection to the client is closed. Setting
 * this to 0 disables the idle timer.
 *
 * @param[in] hdl SSL object handle.
 * @param[in] idle_Timeout Idle timeout in seconds.
 *
 * @return QAPI_OK on success, or a QAPI error value on error.
 */
qapi_Status_t qapi_Net_SSL_Idle_Timeout_Set(qapi_Net_SSL_Obj_Hdl_t hdl, uint32_t idle_Timeout);

/**
 * @brief Configures the maximum number of clients allowed per DTLS server
 * connection.  Defaults to 1 if this API is not called.
 *
 * @param[in] hdl SSL object handle.
 * @param[in] max_Clients Maximum number of clients.
 *
 * @return QAPI_OK on success, or a QAPI error value on error.
 */
qapi_Status_t qapi_Net_SSL_Max_Clients_Set(qapi_Net_SSL_Obj_Hdl_t hdl, uint32_t max_Clients);

/**
 * @brief Configures parameters for an ECJPAKE cipher suite.
 *
 * @param[in] hdl SSL object handle.
 * @param[in] cfg ECPAKE parameters.
 *
 * @return QAPI_OK on success, or a QAPI error value on error.
 */
qapi_Status_t qapi_Net_SSL_ECJPAKE_Parameters_Set(qapi_Net_SSL_Obj_Hdl_t hdl, qapi_Net_SSL_ECJPAKE_Params_t *cfg);

/**
 * @brief Get the last alert sent or received for a TLS connection.  Cannot
 * be used for DTLS server.
 *
 * @param[in] ssl SSL connection handle.
 * @param[out] alert Information about last alert.
 *
 * @return QAPI_OK on success, or a QAPI error value on error.
 */
qapi_Status_t qapi_Net_SSL_Get_Alert_Info(qapi_Net_SSL_Con_Hdl_t ssl, qapi_Net_SSL_Alert_Info_t *alert);

/** @} */

#endif /* _QAPI_SSL_H_ */
