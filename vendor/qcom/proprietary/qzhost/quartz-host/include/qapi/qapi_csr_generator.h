/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_csr_generator.h
 *
 * @details QAPI to generate a Certificate Signing Request (CSR).
 *
 */

#ifndef __QAPI_CSR_GENERATOR_H__
#define __QAPI_CSR_GENERATOR_H__

#include <stdint.h>


/** @addtogroup qapi_networking_ssl_cert
@{ */


/** ECC type key-pair to use for the CSR. */
#define QAPI_KEYPAIR_PARAM_KEY_TYPE_ECC_E 1
/** RSA type key-pair to use for the CSR.  The RSA key-pair generation is not yet supported. */
#define QAPI_KEYPAIR_PARAM_KEY_TYPE_RSA_E 2

/** SECP 192R1 curve type to use for the key-pair generation of the CSR. */
#define QAPI_EC_CURVE_ID_SECP192R1  19
/** SECP 224R1 curve type to use for the key-pair generation of the CSR. */
#define QAPI_EC_CURVE_ID_SECP224R1  21
/** SECP 256R1 curve type to use for the key-pair generation of the CSR. */
#define QAPI_EC_CURVE_ID_SECP256R1  23
/** SECP 384R1 curve type to use for the key-pair generation of the CSR. */
#define QAPI_EC_CURVE_ID_SECP384R1  24


/** Maximum size of the nonce to be used for the encryption of the CSR. */
#define QAPI_PROTECTED_CSR_NONCE_MAX_SIZE 13
/** Size of the tag to be used for authentication of the encrypted CSR. */
#define QAPI_PROTECTED_CSR_TAG_SIZE 16

/**
 * @brief Parameters for the ECC key-pair.
 */
typedef struct qapi_Crypto_Keypair_ECC_Params_s {
    uint16_t curve_Type;
    /**< Curve type to use for the key-pair generation of the Certificate Signing Request (CSR).
         Currently, the only supported curve type is QAPI_EC_CURVE_ID_SECP224R1. */
} qapi_Crypto_Keypair_ECC_Params_t;

/**
 * @brief Parameters for the RSA key-pair.
 */
typedef struct qapi_Crypto_Keypair_RSA_Params_s {
    uint32_t key_Length_In_Bits;
    /**< Length of the key to use for the key-pair generation of the CSR.
         The RSA key-pair generation is not yet supported. */
} qapi_Crypto_Keypair_RSA_Params_t;

/**
 * @brief Crypto key-pair parameters.
 */
typedef struct qapi_Crypto_Keypair_Params_s {
    uint32_t key_Type;
    /**< Key type to use. Currently, only QAPI_KEYPAIR_PARAM_KEY_TYPE_ECC_E is supported. */
    union {
        qapi_Crypto_Keypair_ECC_Params_t ecc;
        /**< ECC parameters specification if an ECC key type is to be used for key-pair generation. */
        qapi_Crypto_Keypair_RSA_Params_t rsa;
        /**< RSA parameters specification if an RSA key type is to be used for key-pair generation. */
    } params;
    /**< Parameters for the key-pair generation. */
} qapi_Crypto_Keypair_Params_t;

/** @} */

/** @addtogroup qapi_networking_ssl
@{ */

/**
* @brief Generates a CSR and encrypts it using a device-specific key.
*
* @param[in]       keypair_Name                Name to use for key-pair associated with the generated CSR.
* @param[in]       keypair_Params              Key-pair parameters.
* @param[in]       service_Provider_ID         Service provider ID buffer to use in CSR.  This ID will be converted to hex string and be put in common name field.
* @param[in]       service_Provider_ID_Size    Size of the service provider ID buffer.
* @param[out]      CSR                         Pointer to the CSR buffer in binary format.
* @param[out]      CSR_Size                    Set to the CSR buffer size.
* @param[out]      CSR_Protected               Pointer to the encrypted CSR buffer.
* @param[out]      CSR_Protected_Size          Set to the encrypted CSR buffer size.
* @param[in]       nonce                       Pointer to a nonce buffer that will be used for encrypting the CSR.
* @param[in]       nonce_Size                  Size of the nonce buffer.
* @param[in,out]   tag                         Set to authentication tag associated with the encrypted CSR.
* @param[in]       aad                         (Optional) Additional authenticated data.
* @param[in]       aad_Length                  (Optional) Set to the AAD buffer size.
*
* @return QAPI_OK on success, or an error code on failure.
*/
int
qapi_Net_SSL_CSR_Generate(
    const char *keypair_Name,
    const qapi_Crypto_Keypair_Params_t * keypair_Params,
    const uint8_t *service_Provider_ID,
    size_t service_Provider_ID_Size,
    uint8_t **CSR,
    size_t *CSR_Size,
    uint8_t **CSR_Protected,
    size_t *CSR_Protected_Size,
    const uint8_t *nonce,
    size_t nonce_Size,
    uint8_t tag[QAPI_PROTECTED_CSR_TAG_SIZE],
    const uint8_t *aad,
    size_t aad_Length
    );

/** @} */

#endif /* __QAPI_CSR_GENERATOR_H__ */
