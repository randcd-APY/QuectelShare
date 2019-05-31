/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * 2016 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_crypto.h
 *
 * @addtogroup qapi_crypto_op
 * @{
 *
 * @details The crypto library provides a collection of API functions that
 * allow the application to perform various cryptographic operations, such as:
 * - Digests (SHA1, SHA256, SHA384, SHA512, MD5)
 * - Ciphers (AES-CBC, AES-CTR, RSA V1.5, RSA no padding)
 * - Key exchange (DH, ECDH, SRP, CURVE25519)
 * - Signature (ECDSA, RSA, ED25519)
 * - Authenticated encryption (AES-CCM, AES-GCM, ChaCha20Poly1305)
 * - Message authentication (HMAC)
 *
 * The crypto QAPIs are designed to be compatible with those defined by Global
 * Platform, a cross industry, non-profit association that publishes
 * specifications for the deployment of secure applications. 
 *
 * "Objects" are the keys used in the crypto operations and "operations" are the
 * cryptographic functions performed on the keys (sign, key generation, etc.).
 *
 * @}
 */

#ifndef __QAPI_CRYPTO_H__
#define __QAPI_CRYPTO_H__

#include <qapi/qapi_types.h>
#include <qapi/qapi_status.h>

/** @addtogroup qapi_crypto_op
@{ */

/** Crypto library-specific error code. An error is returned when authentication fails in
 * authenticated encryption algorithms, such as AES CCM, AES GCM, and ChaCha20Poly1305. */
#define QAPI_ERR_CRYPTO_INVALID_MAC \
	__QAPI_ERROR(QAPI_MOD_CRYPTO, 1)

/** Crypto library-specific error code. Gives an error when signature verification fails. */
#define QAPI_ERR_CRYPTO_INVALID_SIG \
	__QAPI_ERROR(QAPI_MOD_CRYPTO, 2)

/** Crypto library specific error code. Access conflict error when accessing persistent object. */
#define QAPI_ERR_CRYPTO_ACCESS_CONFLICT \
	__QAPI_ERROR(QAPI_MOD_CRYPTO, 3)

/** Crypto library specific error code. Persistent or transient object type does not matched expected type for operation. */
#define QAPI_ERR_CRYPTO_INVALID_OBJECT_TYPE \
	__QAPI_ERROR(QAPI_MOD_CRYPTO, 4)

/** Object handle. This can be a cryptographic keypair, public key or simply data */
typedef uint32_t qapi_Crypto_Obj_Hdl_t;

/** Handle to the crypto operation */
typedef uint32_t qapi_Crypto_Op_Hdl_t;

#define QAPI_CRYPTO_PERSISTENT_OBJECT_ID_MAX_LEN 64

/** Persistent object storage IDs. */
#define QAPI_CRYPTO_PERSISTENT_OBJ_DATA_TEE_STORAGE_PRIVATE 0x00000001

/** Handle to the crypto persistent object enumerator. */
typedef uint32_t qapi_Crypto_Enumerator_Hdl_t;

/** 
 * @brief Crypto object information.
 *
 * See qapi_Crypto_Obj_Info_Get().
 */
typedef struct {
    uint32_t object_Type;
    /**< See qapi_Crypto_Obj_Type_t. */

    uint32_t key_Size;
    /**< Current key size in bits. */

    uint32_t max_Key_Size;
    /**< Maximum key size in bits passed to qapi_Crypto_Transient_Obj_Alloc(). */

    uint32_t object_Usage;
    /**< Bit vector of the #qapi_Crypto_Usage_Flags_t bits. */

    uint32_t handle_Flags;
    /**< See #qapi_Crypto_Handle_Flags_t. */
} qapi_Crypto_Obj_Info_t;

/** 
 * Crypto operation information.
 *
 * See qapi_Crypto_Op_Info_Get().
 **/
typedef struct { 
    uint32_t algorithm;
    /**< Parameter passed to qapi_Crypto_Op_Alloc(). */

    uint32_t mode;
    /**< Parameter passed to qapi_Crypto_Op_Alloc(). */

    uint32_t digest_Len;
    /**< For a MAC, AE, or Digest digest, describes the number of bytes in the digest or tag. */

    uint32_t max_Key_Size;
    /**< Parameter passed to qapi_Crypto_Op_Alloc(). */

    uint32_t key_Size;
    /**< Actual size of the key if a key is programmed in the operation. */

    uint32_t handle_State;
    /**< Bit vector describing the current state of the operation. */
} qapi_Crypto_Op_Info_t;

/**
 * @brief Object type. Denotes the contents of the object.
 */

#define QAPI_CRYPTO_OBJ_TYPE_AES_E 0xA0000010
    /**< Object is AES. */
#define QAPI_CRYPTO_OBJ_TYPE_HMAC_MD5_E 0xA0000001
    /**< Object is HMAC MD5. */
#define QAPI_CRYPTO_OBJ_TYPE_HMAC_SHA1_E 0xA0000002
    /**< Object is HMAC SHA1. */
#define QAPI_CRYPTO_OBJ_TYPE_HMAC_SHA224_E 0xA0000003
    /**< Object is HMAC SHA224. */
#define QAPI_CRYPTO_OBJ_TYPE_HMAC_SHA256_E 0xA0000004
    /**< Object is HMAC SHA256. */
#define QAPI_CRYPTO_OBJ_TYPE_HMAC_SHA384_E 0xA0000005
    /**< Object is HMAC SHA384. */
#define QAPI_CRYPTO_OBJ_TYPE_HMAC_SHA512_E 0xA0000006
    /**< Object is HMAC SHA512. */
#define QAPI_CRYPTO_OBJ_TYPE_RSA_PUBLIC_KEY_E 0xA0000030
    /**< Object is RSA public key. */
#define QAPI_CRYPTO_OBJ_TYPE_RSA_KEYPAIR_E 0xA1000030
    /**< Object is RSA key pair. */
#define QAPI_CRYPTO_OBJ_TYPE_DH_KEYPAIR_E 0xA1000032
    /**< Object is Diffie-Helman key pair. */
#define QAPI_CRYPTO_OBJ_TYPE_ECDSA_PUBLIC_KEY_E 0xA0000041
    /**< Object is ECDSA public key. */
#define QAPI_CRYPTO_OBJ_TYPE_ECDSA_KEYPAIR_E 0xA1000041
    /**< Object is ECDSA key pair. */
#define QAPI_CRYPTO_OBJ_TYPE_ECDH_KEYPAIR_E  0xA1000042
    /**< Object is ECDH key pair. */
#define QAPI_CRYPTO_OBJ_TYPE_GENERIC_SECRET_E 0xA0000000
    /**< Object is generic secret. */
#define QAPI_CRYPTO_OBJ_TYPE_ED25519_PUBLIC_KEY_E 0xA00000C0
    /**< Object is ED25519 public key. */
#define QAPI_CRYPTO_OBJ_TYPE_ED25519_KEYPAIR_E 0xA10000C0
    /**< Object is ED25519 key pair. */
#define QAPI_CRYPTO_OBJ_TYPE_CURVE25519_KEYPAIR_E 0xA10000C1
    /**< Object is Curve25519 key pair. */
#define QAPI_CRYPTO_OBJ_TYPE_SRP_KEYPAIR_E 0xA10000C2
    /**< Object is SRP key pair. */
#define QAPI_CRYPTO_OBJ_TYPE_CHACHA20_E 0xA00000C3
    /**< Object is ChaCha20. */
#define QAPI_CRYPTO_OBJ_TYPE_ECJPAKE_ROUND1_PUBLIC_KEY_AND_ZKP_PAIR_E 0xA10000C3
    /**< Object is round1 ECJPAKE public key and zero knowledge proof pair. */
#define QAPI_CRYPTO_OBJ_TYPE_ECJPAKE_ROUND2_PUBLIC_KEY_AND_ZKP_E 0xA10000C4
    /**< Object is round2 ECJPAKE public key and zero knowledge proof. */

#define QAPI_CRYPTO_OBJ_TYPE_ECDH_PUBLIC_KEY_E  QAPI_CRYPTO_OBJ_TYPE_ECDSA_PUBLIC_KEY_E
   /**< Alias to QAPI_CRYPTO_OBJ_TYPE_ECDH_PUBLIC_KEY for qapi_Crypto_Transient_Obj_Import_From_Pem. */

typedef uint32_t qapi_Crypto_Obj_Type_t;

/** 
 * @brief Algorithm type.
 */

#define QAPI_CRYPTO_ALG_AES_CBC_NOPAD_E    0x10000110
    /**< Algorithm is AES CBC. */
#define QAPI_CRYPTO_ALG_AES_CTR_E  0x10000210
    /**< Algorithm is AES CTR. */
#define QAPI_CRYPTO_ALG_AES_CCM_E  0x40000710
    /**< Algorithm is AES CCM. */
#define QAPI_CRYPTO_ALG_AES_GCM_E  0x40000810
    /**< Algorithm is AES GCM. */
#define QAPI_CRYPTO_ALG_RSASSA_PKCS1_V1_5_SHA1_E   0x70002830
    /**< Algorithm is RSA SHA1. */
#define QAPI_CRYPTO_ALG_RSASSA_PKCS1_V1_5_SHA256_E     0x70004830
    /**< Algorithm is RSA SHA256. */
#define QAPI_CRYPTO_ALG_RSASSA_PKCS1_V1_5_SHA384_E     0x70005830
    /**< Algorithm is RSA SHA384. */
#define QAPI_CRYPTO_ALG_RSASSA_PKCS1_V1_5_SHA512_E     0x70006830
    /**< Algorithm is RSA SHA512. */
#define QAPI_CRYPTO_ALG_RSAES_PKCS1_V1_5_E     0x60000130
    /**< Algorithm is RSA. */
#define QAPI_CRYPTO_ALG_RSA_NOPAD_E    0x60000030
    /**< Algorithm is RSA with no pad. */
#define QAPI_CRYPTO_ALG_DH_DERIVE_SHARED_SECRET_E  0x80000032
    /**< Algorithm is DH Derive Shared Secret. */
#define QAPI_CRYPTO_ALG_MD5_E  0x50000001
    /**< Algorithm is MD5. */
#define QAPI_CRYPTO_ALG_SHA1_E     0x50000002
    /**< Algorithm is SHA1. */
#define QAPI_CRYPTO_ALG_SHA224_E   0x50000003
    /**< Algorithm is SHA224. */
#define QAPI_CRYPTO_ALG_SHA256_E   0x50000004
    /**< Algorithm is SHA256. */
#define QAPI_CRYPTO_ALG_SHA384_E   0x50000005
    /**< Algorithm is SHA384. */
#define QAPI_CRYPTO_ALG_SHA512_E   0x50000006
    /**< Algorithm is SHA512. */
#define QAPI_CRYPTO_ALG_HMAC_MD5_E     0x30000001
    /**< Algorithm is HMAC MD5. */
#define QAPI_CRYPTO_ALG_HMAC_SHA1_E    0x30000002
    /**< Algorithm is HMAC SHA1. */
#define QAPI_CRYPTO_ALG_HMAC_SHA224_E  0x30000003
    /**< Algorithm is HMAC SHA224. */
#define QAPI_CRYPTO_ALG_HMAC_SHA256_E  0x30000004
    /**< Algorithm is HMAC SHA256. */
#define QAPI_CRYPTO_ALG_HMAC_SHA384_E  0x30000005
    /**< Algorithm is HMAC SHA384. */
#define QAPI_CRYPTO_ALG_HMAC_SHA512_E  0x30000006
    /**< Algorithm is HMAC SHA512. */
#define QAPI_CRYPTO_ALG_ECDSA_P192_E   0x70001042
    /**< Algorithm is ECDSA P192. */
#define QAPI_CRYPTO_ALG_ECDSA_P224_E   0x70002042
    /**< Algorithm is ECDSA P224. */
#define QAPI_CRYPTO_ALG_ECDSA_P256_E   0x70003042
    /**< Algorithm is ECDSA P256. */
#define QAPI_CRYPTO_ALG_ECDSA_P384_E   0x70004042
    /**< Algorithm is ECDSA P384. */
#define QAPI_CRYPTO_ALG_ECDSA_P521_E   0x70005042
    /**< Algorithm is ECDSA P521. */
#define QAPI_CRYPTO_ALG_ECDH_P192_E    0x80001042
    /**< Algorithm is ECDH P192. */
#define QAPI_CRYPTO_ALG_ECDH_P224_E    0x80002042
    /**< Algorithm is ECDH P224. */
#define QAPI_CRYPTO_ALG_ECDH_P256_E    0x80003042
    /**< Algorithm is ECDH P256. */
#define QAPI_CRYPTO_ALG_ECDH_P384_E    0x80004042
    /**< Algorithm is ECDH P384. */
#define QAPI_CRYPTO_ALG_ECDH_P521_E    0x80005042
    /**< Algorithm is ECDH P521. */
#define QAPI_CRYPTO_ALG_ED25519_E  0x700000C0
    /**< Algorithm is ED25519. */
#define QAPI_CRYPTO_ALG_CURVE25519_DERIVE_SHARED_SECRET_E  0x800000C1
    /**< Algorithm is Curve25519. */
#define QAPI_CRYPTO_ALG_SRP_DERIVE_SHARED_SECRET_E     0x800000C2
    /**< Algorithm is SRP. */
#define QAPI_CRYPTO_ALG_CHACHA20_POLY1305_E    0x400000C3
    /**< Algorithm is ChaCha20. */
#define QAPI_CRYPTO_ALG_ECJPAKE_E  0x800000C3
    /**< Algorithm is ECJPAKE. */

typedef uint32_t qapi_Crypto_Alg_t;
 
/** 
 * @brief SRP mode.
 */
typedef enum {
    QAPI_CRYPTO_SRP_CLIENT_E,
    /**< SRIP Client mode. */
    QAPI_CRYPTO_SRP_SERVER_E,
    /**< SRIP Server mode. */
} qapi_Crypto_SRP_Type_t;

/** 
 * @brief Operation mode.
 */
typedef enum {
    QAPI_CRYPTO_MODE_ENCRYPT_E,
    /**< Operation Encrypt. */
    QAPI_CRYPTO_MODE_DECRYPT_E,
    /**< Operation Decrypt. */
    QAPI_CRYPTO_MODE_SIGN_E,
    /**< Operation Sign. */
    QAPI_CRYPTO_MODE_VERIFY_E,
    /**< Operation Verify. */
    QAPI_CRYPTO_MODE_MAC_E,
    /**< Operation MAC. */
    QAPI_CRYPTO_MODE_DIGEST_E,
    /**< Operation Digest. */
    QAPI_CRYPTO_MODE_DERIVE_E,
    /**< Operation Key derivation. */
} qapi_Crypto_Mode_t;

/**
 * @brief Attribute type. Used in #qapi_Crypto_Attrib_t.
 */

#define QAPI_CRYPTO_ATTR_SECRET_VALUE_E 0xC0000000
    /**< Attribute secret value. */
#define QAPI_CRYPTO_ATTR_ED25519_PUBLIC_VALUE_E 0xD00001C0
    /**< Attribute ED25519 public value. */
#define QAPI_CRYPTO_ATTR_ED25519_PRIVATE_VALUE_E 0xC00001C0
    /**< Attribute ED25519 private value. */
#define QAPI_CRYPTO_ATTR_CURVE25519_PUBLIC_VALUE_E 0xD00001C1
    /**< Attribute Curve25519 public value. */
#define QAPI_CRYPTO_ATTR_CURVE25519_PRIVATE_VALUE_E 0xC00002C1
    /**< Attribute Curve15519 private value. */
#define QAPI_CRYPTO_ATTR_ECC_PUBLIC_VALUE_X_E 0xD0000141
    /**< Attribute ECC public value x .*/
#define QAPI_CRYPTO_ATTR_ECC_PUBLIC_VALUE_Y_E 0xD0000241
    /**< Attribute ECC public value y. */
#define QAPI_CRYPTO_ATTR_ECC_PRIVATE_VALUE_E 0xC0000341
    /**< Attribute ECC private value. */
#define QAPI_CRYPTO_ATTR_ECC_CURVE_E 0xF0000441
    /**< Attribute ECC curve. */
#define QAPI_CRYPTO_ATTR_DH_PRIME_E 0xD0001032
    /**< Attribute DH prime. */
#define QAPI_CRYPTO_ATTR_DH_BASE_E 0xD0001232
    /**< Attribute DH base. */
#define QAPI_CRYPTO_ATTR_DH_PUBLIC_VALUE_E 0xD0000132
    /**< Attribute DH public value. */
#define QAPI_CRYPTO_ATTR_DH_PRIVATE_VALUE_E 0xC0000232
    /**< Attribute DH private value. */
#define QAPI_CRYPTO_ATTR_RSA_MODULUS_E 0xD0000130
    /**< Attribute RSA modulus. */
#define QAPI_CRYPTO_ATTR_RSA_PUBLIC_EXPONENT_E 0xD0000230
    /**< Attribute RSA public exponent. */
#define QAPI_CRYPTO_ATTR_RSA_PRIVATE_EXPONENT_E 0xC0000330
    /**< Attribute RSA private exponent. */
#define QAPI_CRYPTO_ATTR_RSA_PRIME1_E 0xC0000430
    /**< Attribute RSA prime1. */
#define QAPI_CRYPTO_ATTR_RSA_PRIME2_E 0xC0000530
    /**< Attribute RSA prime2. */
#define QAPI_CRYPTO_ATTR_RSA_EXPONENT1_E 0xC0000630
    /**< Attribute RSA exponent1. */
#define QAPI_CRYPTO_ATTR_RSA_EXPONENT2_E 0xC0000730
    /**< Attribute RSA exponent2. */
#define QAPI_CRYPTO_ATTR_RSA_COEFFICIENT_E 0xC0000830
    /**< Attribute RSA coefficient. */
#define QAPI_CRYPTO_ATTR_SRP_PRIME_E 0xD00001C2
    /**< Attribute SRP prime. */
#define QAPI_CRYPTO_ATTR_SRP_GEN_E   0xD00002C2
    /**< Attribute SRP gen. */
#define QAPI_CRYPTO_ATTR_SRP_VERIFIER_E  0xC00003C2
    /**< Attribute SRP verifier. */
#define QAPI_CRYPTO_ATTR_SRP_USERNAME_E  0xD00004C2
    /**< Attribute SRP user name. */
#define QAPI_CRYPTO_ATTR_SRP_PASSWORD_E  0xC00005C2
    /**< Attribute SRP password. */
#define QAPI_CRYPTO_ATTR_SRP_TYPE_E  0xD00006C2
    /**< Attribute SRP type. */
#define QAPI_CRYPTO_ATTR_SRP_HASH_E  0xD00007C2
    /**< Attribute SRP hash. */
#define QAPI_CRYPTO_ATTR_SRP_SALT_E  0xD00008C2
    /**< Attribute SRP salt. */
#define QAPI_CRYPTO_ATTR_SRP_PUBLIC_VALUE_E  0xD00009C2
    /**< Attribute SRP public value. */
#define QAPI_CRYPTO_ATTR_SRP_PRIVATE_VALUE_E 0xC0000AC2
    /**< Attribute SRP private value. */
#define QAPI_CRYPTO_ATTR_ECJPAKE_LOCAL_IDENTITY_E 0xD00010C2
    /**< Attribute ECJPAKE local identity. */
#define QAPI_CRYPTO_ATTR_ECJPAKE_REMOTE_IDENTITY_E 0xD00020C2
    /**< Attribute ECJPAKE remote identity. */
#define QAPI_CRYPTO_ATTR_ECJPAKE_ROUND1_PUBLIC_KEY_1_E 0xD00030C2
    /**< Attribute ECJPAKE public key. */
#define QAPI_CRYPTO_ATTR_ECJPAKE_ROUND1_ZKP_EPHEMERAL_PUBLIC_KEY_1_E 0xD00040C2
    /**< Attribute ECJPAKE zero knowledge proof ephemeral public key. */
#define QAPI_CRYPTO_ATTR_ECJPAKE_ROUND1_ZKP_SIGNATURE_1_E 0xD00050C2
    /**< Attribute ECJPAKE zero knowledge proof signature. */
#define QAPI_CRYPTO_ATTR_ECJPAKE_ROUND1_PUBLIC_KEY_2_E 0xD00060C2
    /**< Attribute ECJPAKE public key. */
#define QAPI_CRYPTO_ATTR_ECJPAKE_ROUND1_ZKP_EPHEMERAL_PUBLIC_KEY_2_E 0xD00070C2
    /**< Attribute ECJPAKE zero knowledge proof ephemeral public key. */
#define QAPI_CRYPTO_ATTR_ECJPAKE_ROUND1_ZKP_SIGNATURE_2_E 0xD00080C2
    /**< Attribute ECJPAKE zero knowledge proof signature. */
#define QAPI_CRYPTO_ATTR_ECJPAKE_ROUND2_PUBLIC_KEY_E 0xD00090C2
    /**< Attribute ECJPAKE public key. */
#define QAPI_CRYPTO_ATTR_ECJPAKE_ROUND2_ZKP_EPHEMERAL_PUBLIC_KEY_E 0xD00100C2
    /**< Attribute ECJPAKE zero knowledge proof ephemeral public key. */
#define QAPI_CRYPTO_ATTR_ECJPAKE_ROUND2_ZKP_SIGNATURE_E 0xD00110C2
    /**< Attribute ECJPAKE zero knowledge proof signature. */

typedef uint32_t qapi_Crypto_Attr_Type_t;

/**
 * @brief An attribute can be either a buffer attribute or a value attribute. This is
 * determined by bit [29] of the attribute identifier. If this bit is set to 0,
 * then the attribute is a buffer attribute and the field ref must be selected.
 * If the bit is set to 1, it is a value attribute and the field val
 * must be selected.
 */
typedef struct {
    uint32_t attrib_id;
    /**< Attribute ID. See qapi_Crypto_Attr_Type_t. */

    union {
        struct {
            void *buf;
            /**< Buffer pointer. */
            uint32_t len;
            /**< Buffer length. */
        } ref;
        /**< Reference. */
        struct {
            uint32_t a;
            /**< Value attribute a. */
            uint32_t b;
            /**< Value attribute b. */
        } val;
        /**< Value. */
    } u;
    /**< Reference/value union. */
} qapi_Crypto_Attrib_t;

/** 
 * @brief Object usage flags.
 */
typedef enum {
    QAPI_CRYPTO_USAGE_EXTRACTABLE_E = 0x1,
    /**< A protected attribute cannot be extracted unless it has the
     * *_USAGE_EXTRACTABLE_E flag. */

    QAPI_CRYPTO_USAGE_ENCRYPT_E = 0x2,
    /**< Used in encryption operations. */

    QAPI_CRYPTO_USAGE_DECRYPT_E = 0x4,
    /**< Used in decryption operations. */

    QAPI_CRYPTO_USAGE_MAC_E = 0x8,
    /**< Used in MAC operations. */

    QAPI_CRYPTO_USAGE_SIGN_E = 0x10,
    /**< Used in signature operations. */

    QAPI_CRYPTO_USAGE_VERIFY_E = 0x20,
    /**< Used in verify signature operations. */

    QAPI_CRYPTO_USAGE_DERIVE_E = 0x40,
    /**< Used in key derive operations. */

    QAPI_CRYPTO_USAGE_ALL_E = (~0),
    /**< Used in all operations. */
} qapi_Crypto_Usage_Flags_t;

/** 
 * @brief Operation class.
 */
typedef enum {
    QAPI_CRYPTO_OP_CIPHER_E,
    /**< Used in cipher operations. */

    QAPI_CRYPTO_OP_MAC_E,
    /**< Used in MAC operations. */

    QAPI_CRYPTO_OP_AE_E,
    /**< Used in AE operations. */

    QAPI_CRYPTO_OP_DIGEST_E,
    /**< Used in digest operations. */

    QAPI_CRYPTO_OP_ASYMMETRIC_CIPHER_E,
    /**< Used in asymmetric cipher operations. */

    QAPI_CRYPTO_OP_ASYMMETRIC_SIGNATURE_E,
    /**< Used in asymmetric signature operations. */

    QAPI_CRYPTO_OP_KEY_DERIVATION_E,
    /**< Used in key derivation. */
} qapi_Crypto_Op_Class_t;

/**
 * @brief Handle flags.
 */
typedef enum {
    QAPI_CRYPTO_HANDLE_INITIALIZED_E = 0x20000,
    /**< Indicates that a key has been generated/populated in an object handle. */

    QAPI_CRYPTO_HANDLE_KEY_SET_E = 0x40000,
    /**< Indicates that a key has been set in an operation handle. */

	QAPI_CRYPTO_HANDLE_PERSISTENT_E = 0x10000,
	/**< Indicates that an object handle corresponds to a persistent object. */

} qapi_Crypto_Handle_Flags_t;

/**
 * @brief Data flags for persistent objects.
 */
typedef enum {

	QAPI_CRYPTO_DATA_ACCESS_READ = 0x00000001,
	/**< Read access right for data only (non-cryptographic) persistent objects. */

	QAPI_CRYPTO_DATA_ACCESS_WRITE = 0x00000002,
	/**< Write access right for data only (non-cryptographic) persistent objects. */

	QAPI_CRYPTO_DATA_ACCESS_WRITE_META = 0x00000004,
	/**< Access right for deleting and renaming persistent objects. */

	QAPI_CRYPTO_DATA_SHARE_READ = 0x00000010,
	/**< (Not supported) Shared read access right for data only persistent objects. */

	QAPI_CRYPTO_DATA_SHARE_WRITE = 0x00000020,
	/**< (Not supported) Shared write access right for data only persistent objects. */

	QAPI_CRYPTO_DATA_EXCLUSIVE = 0x00000400,
	/**< Flag used when creating a persistent object. Does not create it if it already exists. */

} qapi_Crypto_Data_Flags_t;

/* ECC curve types and key sizes */

/** To use with ECC operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P192_KEYPAIR_BITS (192 * 2)

/** To use with ECC operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P224_KEYPAIR_BITS (224 * 2)

/** To use with ECC operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P256_KEYPAIR_BITS (256 * 2)

/** To use with ECC operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P384_KEYPAIR_BITS (384 * 2)

/** To use with ECC operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P521_KEYPAIR_BITS (528 * 2)

/** To use with ECC operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P192_KEYPAIR_BYTES \
	(QAPI_CRYPTO_ECC_P192_KEYPAIR_BITS / 8)

/** To use with ECC operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P224_KEYPAIR_BYTES \
	(QAPI_CRYPTO_ECC_P224_KEYPAIR_BITS / 8)

/** To use with ECC operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P256_KEYPAIR_BYTES \
	(QAPI_CRYPTO_ECC_P256_KEYPAIR_BITS / 8)

/** To use with ECC operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P384_KEYPAIR_BYTES \
	(QAPI_CRYPTO_ECC_P384_KEYPAIR_BITS / 8)

/** To use with ECC operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P521_KEYPAIR_BYTES \
	(QAPI_CRYPTO_ECC_P521_KEYPAIR_BITS / 8)


/** To use with ECDH shared secret operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P192_SHARED_SECRET_BITS (192)

/** To use with ECDH shared secret operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P224_SHARED_SECRET_BITS (224)

/** To use with ECDH shared secret operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P256_SHARED_SECRET_BITS (256)

/** To use with ECDH shared secret operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P384_SHARED_SECRET_BITS (384)

/** To use with ECDH shared secret operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P521_SHARED_SECRET_BITS (528)

/** To use with ECDH shared secret operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P192_SHARED_SECRET_BYTES \
	(QAPI_CRYPTO_ECC_P192_SHARED_SECRET_BITS/8)

/** To use with ECDH shared secret operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P224_SHARED_SECRET_BYTES \
	(QAPI_CRYPTO_ECC_P224_SHARED_SECRET_BITS/8)

/** To use with ECDH shared secret operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P256_SHARED_SECRET_BYTES \
	(QAPI_CRYPTO_ECC_P256_SHARED_SECRET_BITS/8)

/** To use with ECDH shared secret operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P384_SHARED_SECRET_BYTES \
	(QAPI_CRYPTO_ECC_P384_SHARED_SECRET_BITS/8)

/** To use with ECDH shared secret operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P521_SHARED_SECRET_BYTES \
	(QAPI_CRYPTO_ECC_P521_SHARED_SECRET_BITS/8)


/** To use with ECC public key operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P192_PUBLIC_KEY_BITS (192 * 2)

/** To use with ECC public key operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P224_PUBLIC_KEY_BITS (224 * 2)

/** To use with ECC public key operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P256_PUBLIC_KEY_BITS (256 * 2)

/** To use with ECC public key operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P384_PUBLIC_KEY_BITS (384 * 2)

/** To use with ECC public key operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P521_PUBLIC_KEY_BITS (528 * 2)


/** To use with ECC public key operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P192_PUB_VAL_X_BYTES (192/8)

/** To use with ECC public key operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P224_PUB_VAL_X_BYTES (224/8)

/** To use with ECC public key operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P256_PUB_VAL_X_BYTES (256/8)

/** To use with ECC public key operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P384_PUB_VAL_X_BYTES (384/8)

/** To use with ECC public key operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P521_PUB_VAL_X_BYTES (528/8)


/** To use with ECC public key operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P192_PUB_VAL_Y_BYTES \
	(QAPI_CRYPTO_ECC_P192_PUB_VAL_X_BYTES)

/** To use with ECC public key operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P224_PUB_VAL_Y_BYTES \
	(QAPI_CRYPTO_ECC_P224_PUB_VAL_X_BYTES)

/** To use with ECC public key operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P256_PUB_VAL_Y_BYTES \
	(QAPI_CRYPTO_ECC_P256_PUB_VAL_X_BYTES)

/** To use with ECC public key operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P384_PUB_VAL_Y_BYTES \
	(QAPI_CRYPTO_ECC_P384_PUB_VAL_X_BYTES)

/** To use with ECC public key operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P521_PUB_VAL_Y_BYTES \
	(QAPI_CRYPTO_ECC_P521_PUB_VAL_X_BYTES)


/** To use with ECC sign operation APIs. */
#define QAPI_CRYPTO_ECC_P192_PRIVATE_KEY_BITS (192)

/** To use with ECC sign operation APIs. */
#define QAPI_CRYPTO_ECC_P224_PRIVATE_KEY_BITS (224)

/** To use with ECC sign operation APIs. */
#define QAPI_CRYPTO_ECC_P256_PRIVATE_KEY_BITS (256)

/** To use with ECC sign operation APIs. */
#define QAPI_CRYPTO_ECC_P384_PRIVATE_KEY_BITS (384)

/** To use with ECC sign operation APIs. */
#define QAPI_CRYPTO_ECC_P521_PRIVATE_KEY_BITS (528)


/** To use with ECC sign operation APIs. */
#define QAPI_CRYPTO_ECC_P192_PRIVATE_KEY_BYTES \
	(QAPI_CRYPTO_ECC_P192_PRIVATE_KEY_BITS/8)

/** To use with ECC sign operation APIs. */
#define QAPI_CRYPTO_ECC_P224_PRIVATE_KEY_BYTES \
	(QAPI_CRYPTO_ECC_P224_PRIVATE_KEY_BITS/8)

/** To use with ECC sign operation APIs. */
#define QAPI_CRYPTO_ECC_P256_PRIVATE_KEY_BYTES \
	(QAPI_CRYPTO_ECC_P256_PRIVATE_KEY_BITS/8)

/** To use with ECC sign operation APIs. */
#define QAPI_CRYPTO_ECC_P384_PRIVATE_KEY_BYTES \
	(QAPI_CRYPTO_ECC_P384_PRIVATE_KEY_BITS/8)

/** To use with ECC sign operation APIs. */
#define QAPI_CRYPTO_ECC_P521_PRIVATE_KEY_BYTES \
	(QAPI_CRYPTO_ECC_P521_PRIVATE_KEY_BITS/8)

/** To use with ECC sign operation APIs. */
#define QAPI_CRYPTO_ECDSA_SIGNATURE_BUFFER_SIZE_BYTES(key_Pair_Size_In_Bytes) \
    (key_Pair_Size_In_Bytes + 9)

/** To use with RSA sign operation APIs. */
#define QAPI_CRYPTO_RSA_SIGNATURE_BUFFER_SIZE_BYTES(key_Size_In_Bytes) \
    (key_Size_In_Bytes)

/** To use with RSA encrypt operation APIs. */
#define QAPI_CRYPTO_RSA_ENCRYPT_CIPHER_TEXT_BUFFER_SIZE_BYTES(key_Size_In_Bytes) \
    (key_Size_In_Bytes)

/** To use with RSA decrypt operation APIs. */
#define QAPI_CRYPTO_RSA_DECRYPT_PLAIN_TEXT_BUFFER_SIZE_BYTES(key_Size_In_Bytes) \
    (key_Size_In_Bytes)

/** To use with ED25519 sign operation APIs. */
#define QAPI_CRYPTO_ED25519_PUBLIC_KEY_BYTES (32)

/** To use with ED25519 sign operation APIs. */
#define QAPI_CRYPTO_ED25519_PRIVATE_KEY_BYTES (64)

/** To use with ED25519 sign operation APIs. */
#define QAPI_CRYPTO_ED25519_PUBLIC_KEY_BITS \
	(QAPI_CRYPTO_ED25519_PUBLIC_KEY_BYTES * 8)

/** To use with ED25519 sign operation APIs. */
#define QAPI_CRYPTO_ED25519_PRIVATE_KEY_BITS \
	(QAPI_CRYPTO_ED25519_PRIVATE_KEY_BYTES * 8)


/** To use with curve25519 sign operation APIs. */
#define QAPI_CRYPTO_CURVE25519_PUBLIC_KEY_BYTES (32)

/** To use with curve25519 sign operation APIs. */
#define QAPI_CRYPTO_CURVE25519_PRIVATE_KEY_BYTES (32)

/** To use with curve25519 sign operation APIs. */
#define QAPI_CRYPTO_CURVE25519_SHARED_SECRET_BYTES (32)

/** To use with curve25519 sign operation APIs. */
#define QAPI_CRYPTO_CURVE25519_KEYPAIR_BYTES (32)


/** To use with curve25519 sign operation APIs. */
#define QAPI_CRYPTO_CURVE25519_PUBLIC_KEY_BITS \
	(QAPI_CRYPTO_CURVE25519_PUBLIC_KEY_BYTES * 8)

/** To use with curve25519 sign operation APIs. */
#define QAPI_CRYPTO_CURVE25519_PRIVATE_KEY_BITS \
	(QAPI_CRYPTO_CURVE25519_PRIVATE_KEY_BYTES * 8)

/** To use with curve25519 sign operation APIs. */
#define QAPI_CRYPTO_CURVE25519_SHARED_SECRET_BITS \
	(QAPI_CRYPTO_CURVE25519_SHARED_SECRET_BYTES * 8)

/** To use with curve25519 sign operation APIs. */
#define QAPI_CRYPTO_CURVE25519_KEYPAIR_BITS \
	(QAPI_CRYPTO_CURVE25519_KEYPAIR_BYTES * 8)

/** To use with ECC public key operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P192_PUBLIC_KEY_UNCOMPRESSED_FORMAT_BYTES (2 * QAPI_CRYPTO_ECC_P192_PUB_VAL_X_BYTES) + 1

/** To use with ECC public key operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P244_PUBLIC_KEY_UNCOMPRESSED_FORMAT_BYTES (2 * QAPI_CRYPTO_ECC_P244_PUB_VAL_X_BYTES) + 1

/** To use with ECC public key operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P256_PUBLIC_KEY_UNCOMPRESSED_FORMAT_BYTES (2 * QAPI_CRYPTO_ECC_P256_PUB_VAL_X_BYTES) + 1

/** To use with ECC public key operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P384_PUBLIC_KEY_UNCOMPRESSED_FORMAT_BYTES (2 * QAPI_CRYPTO_ECC_P384_PUB_VAL_X_BYTES) + 1

/** To use with ECC public key operations and objects APIs. */
#define QAPI_CRYPTO_ECC_P521_PUBLIC_KEY_UNCOMPRESSED_FORMAT_BYTES (2 * QAPI_CRYPTO_ECC_P521_PUB_VAL_X_BYTES) + 1


/** Number of attributes that must be provided in _key_derive API. */
#define QAPI_CRYPTO_OP_ATTRIB_COUNT_ECJPAKE_DERIVE_ROUND1_KEYS 2
/** Number of attributes that must be provided in _key_derive API. */
#define QAPI_CRYPTO_OP_ATTRIB_COUNT_ECJPAKE_DERIVE_ROUND2_KEYS 7
/** Number of attributes that must be provided in _key_derive API. */
#define QAPI_CRYPTO_OP_ATTRIB_COUNT_ECJPAKE_DERIVE_PREMASTER_KEY 3

/*
 * these values MUST be the same as TLS_NAMEDCURVE_SECPXXXR1 
 * in SharkSslCon.h -- see RFC 4492 sect. 5.1.1
 */
/** Supported ECC NIST curves. */
#define QAPI_CRYPTO_ECC_CURVE_NIST_P192 19
/** Supported ECC NIST curves. */
#define QAPI_CRYPTO_ECC_CURVE_NIST_P224 21
/** Supported ECC NIST curves. */
#define QAPI_CRYPTO_ECC_CURVE_NIST_P256 23
/** Supported ECC NIST curves. */
#define QAPI_CRYPTO_ECC_CURVE_NIST_P384 24
/** Unsupported ECC NIST curves. */
#define QAPI_CRYPTO_ECC_CURVE_NIST_P521 25

/** Number of attributes that must be provided in *_populate API. */
#define QAPI_CRYPTO_OBJ_ATTRIB_COUNT_ECC_KEYPAIR 4
/** Number of attributes that must be provided in *_populate API. */
#define QAPI_CRYPTO_OBJ_ATTRIB_COUNT_ECC_PUBLIC_KEY 3
/** Number of attributes that must be provided in *_populate API. */
#define QAPI_CRYPTO_OBJ_ATTRIB_COUNT_ED25519_KEYPAIR 2
/** Number of attributes that must be provided in *_populate API. */
#define QAPI_CRYPTO_OBJ_ATTRIB_COUNT_ED25519_PUBLIC_KEY 1
/** Number of attributes that must be provided in *_populate API. */
#define QAPI_CRYPTO_OBJ_ATTRIB_COUNT_CURVE25519_KEYPAIR 2
/** Number of attributes that must be provided in *_populate API. */
#define QAPI_CRYPTO_OBJ_ATTRIB_COUNT_CURVE25519_PUBLIC_KEY 1
/** Number of attributes that must be provided in *_populate API. */
#define QAPI_CRYPTO_OBJ_ATTRIB_COUNT_GENERIC_SECRET 1
/** Number of attributes that must be provided in *_populate API. */
#define QAPI_CRYPTO_OBJ_ATTRIB_COUNT_ECDH 2
/** Number of attributes that must be provided in *_populate API. */
#define QAPI_CRYPTO_OBJ_ATTRIB_COUNT_DH_POP 4
/** Number of attributes that must be provided in *_populate API. */
#define QAPI_CRYPTO_OBJ_ATTRIB_COUNT_RSA_KEYPAIR 8
/** Number of attributes that must be provided in *_populate API. */
#define QAPI_CRYPTO_OBJ_ATTRIB_COUNT_RSA_PUBLIC_KEY 2
/** Number of attributes that must be provided in *_populate API. */
#define QAPI_CRYPTO_OBJ_ATTRIB_COUNT_SRP_CLIENT 3
/** Number of attributes that must be provided in *_populate API.
 *  Does not include optional attributes. */
#define QAPI_CRYPTO_OBJ_ATTRIB_COUNT_SRP_SERVER 4
/** Number of attributes that must be provided in *_keygen API. */
#define QAPI_CRYPTO_OBJ_ATTRIB_COUNT_DH_KEYGEN 2

/** Cipher AES key size. */
#define QAPI_CRYPTO_AES128_KEY_BYTES (16)
/** Cipher AES key size. */
#define QAPI_CRYPTO_AES256_KEY_BYTES (32)
/** Cipher AES IV size. */
#define QAPI_CRYPTO_AES_IV_BYTES (16)
/** Cipher AES block size. */
#define QAPI_CRYPTO_AES_BLOCK_BYTES (16)

/** Cipher AES key size. */
#define QAPI_CRYPTO_AES128_KEY_BITS \
	(QAPI_CRYPTO_AES128_KEY_BYTES*8)

/** Cipher AES key size. */
#define QAPI_CRYPTO_AES256_KEY_BITS \
	(QAPI_CRYPTO_AES256_KEY_BYTES*8)

/** Cipher AES encrypt buffer size. */
#define QAPI_CRYPTO_AES_ENCRYPT_CIPHER_TEXT_BUFFER_SIZE_BYTES(plain_Text_Bytes) \
	( ( ( plain_Text_Bytes + (QAPI_CRYPTO_AES_BLOCK_BYTES - 1) ) / QAPI_CRYPTO_AES_BLOCK_BYTES ) * QAPI_CRYPTO_AES_BLOCK_BYTES )

/** Cipher AES decrypt buffer size. */
#define QAPI_CRYPTO_AES_DECRYPT_PLAIN_TEXT_BUFFER_SIZE_BYTES(cipher_Text_Bytes) (cipher_Text_Bytes)

/** Cipher CHACHA20_POLY key size. */
#define QAPI_CRYPTO_CHACHA20_POLY1305_KEY_BYTES (32)
/** Cipher CHACHA20_POLY key size. */
#define QAPI_CRYPTO_CHACHA20_POLY1305_KEY_BITS \
	(QAPI_CRYPTO_CHACHA20_POLY1305_KEY_BYTES*8)
/** Cipher CHACHA20_POLY nonce size. */
#define QAPI_CRYPTO_CHACHA20_POLY1305_NONCE_BYTES (12)


/** Cipher CHACHA20 POLY1305 encrypt buffer size. */
#define QAPI_CRYPTO_CHACHA20_POLY1305_ENCRYPT_CIPHER_TEXT_BUFFER_SIZE_BYTES(plain_Text_Bytes) \
	( ( ( plain_Text_Bytes + (64 - 1) ) / 64 ) * 64 )

/** Cipher CHACHA20 POLY1305 decrypt buffer size. */
#define QAPI_CRYPTO_CHACHA20_POLY1305_DECRYPT_PLAIN_TEXT_BUFFER_SIZE_BYTES(cipher_Text_Bytes) (cipher_Text_Bytes)

/** HMAC-SHA1 minimum key size. */
#define QAPI_CRYPTO_HMAC_SHA1_MIN_KEY_BYTES (10)
/** HMAC-SHA1 maximum key size. */
#define QAPI_CRYPTO_HMAC_SHA1_MAX_KEY_BYTES (64)
/** HMAC-SHA1 block size. */
#define QAPI_CRYPTO_HMAC_SHA1_BLOCK_BYTES (64)
/** HMAC-SHA1 digest size. */
#define QAPI_CRYPTO_HMAC_SHA1_MAC_BYTES (20)

/** HMAC-SHA1 minimum key size. */
#define QAPI_CRYPTO_HMAC_SHA1_MIN_KEY_BITS \
	(QAPI_CRYPTO_HMAC_SHA1_MIN_KEY_BYTES*8)
/** HMAC-SHA1 maximum key size. */
#define QAPI_CRYPTO_HMAC_SHA1_MAX_KEY_BITS \
	(QAPI_CRYPTO_HMAC_SHA1_MAX_KEY_BYTES*8)
/** HMAC-SHA1 block size. */
#define QAPI_CRYPTO_HMAC_SHA1_BLOCK_BITS \
	(QAPI_CRYPTO_HMAC_SHA1_BLOCK_BYTES*8)
/** HMAC-SHA1 digest size. */
#define QAPI_CRYPTO_HMAC_SHA1_MAC_BITS \
	(QAPI_CRYPTO_HMAC_SHA1_MAC_BYTES*8)

/** HMAC-SHA256 minimum key size. */
#define QAPI_CRYPTO_HMAC_SHA256_MIN_KEY_BYTES (24)
/** HMAC-SHA256 maximum key size. */
#define QAPI_CRYPTO_HMAC_SHA256_MAX_KEY_BYTES (128)
/** HMAC-SHA256 block size. */
#define QAPI_CRYPTO_HMAC_SHA256_BLOCK_BYTES (64)
/** HMAC-SHA256 digest size. */
#define QAPI_CRYPTO_HMAC_SHA256_MAC_BYTES (32)

/** HMAC-SHA256 minimum key size. */
#define QAPI_CRYPTO_HMAC_SHA256_MIN_KEY_BITS \
	(QAPI_CRYPTO_HMAC_SHA256_MIN_KEY_BYTES*8)
/** HMAC-SHA256 maximum key size. */
#define QAPI_CRYPTO_HMAC_SHA256_MAX_KEY_BITS \
	(QAPI_CRYPTO_HMAC_SHA256_MAX_KEY_BYTES*8)
/** HMAC-SHA256 block size. */
#define QAPI_CRYPTO_HMAC_SHA256_BLOCK_BITS \
	(QAPI_CRYPTO_HMAC_SHA256_BLOCK_BYTES*8)
/** HMAC-SHA256 digest size. */
#define QAPI_CRYPTO_HMAC_SHA256_MAC_BITS \
	(QAPI_CRYPTO_HMAC_SHA256_MAC_BYTES*8)

/** HMAC-SHA384 minimum key size. */
#define QAPI_CRYPTO_HMAC_SHA384_MIN_KEY_BYTES (32)
/** HMAC-SHA384 maximum key size. */
#define QAPI_CRYPTO_HMAC_SHA384_MAX_KEY_BYTES (128)
/** HMAC-SHA384 block size. */
#define QAPI_CRYPTO_HMAC_SHA384_BLOCK_BYTES (128)
/** HMAC-SHA384 digest size. */
#define QAPI_CRYPTO_HMAC_SHA384_MAC_BYTES (48)

/** HMAC-SHA384 minimum key size. */
#define QAPI_CRYPTO_HMAC_SHA384_MIN_KEY_BITS \
	(QAPI_CRYPTO_HMAC_SHA384_MIN_KEY_BYTES*8)
/** HMAC-SHA384 maximum key size. */
#define QAPI_CRYPTO_HMAC_SHA384_MAX_KEY_BITS \
	(QAPI_CRYPTO_HMAC_SHA384_MAX_KEY_BYTES*8)
/** HMAC-SHA384 block size. */
#define QAPI_CRYPTO_HMAC_SHA384_BLOCK_BITS \
	(QAPI_CRYPTO_HMAC_SHA384_BLOCK_BYTES*8)
/** HMAC-SHA384 digest size. */
#define QAPI_CRYPTO_HMAC_SHA384_MAC_BITS \
	(QAPI_CRYPTO_HMAC_SHA384_MAC_BYTES*8)

/** HMAC-SHA512 minimum key size. */
#define QAPI_CRYPTO_HMAC_SHA512_MIN_KEY_BYTES (32)
/** HMAC-SHA512 maximum key size. */
#define QAPI_CRYPTO_HMAC_SHA512_MAX_KEY_BYTES (128)
/** HMAC-SHA512 block size. */
#define QAPI_CRYPTO_HMAC_SHA512_BLOCK_BYTES (128)
/** HMAC-SHA512 digest size. */
#define QAPI_CRYPTO_HMAC_SHA512_MAC_BYTES (64)

/** HMAC-SHA512 minimum key size. */
#define QAPI_CRYPTO_HMAC_SHA512_MIN_KEY_BITS \
	(QAPI_CRYPTO_HMAC_SHA512_MIN_KEY_BYTES*8)
/** HMAC-SHA512 maximum key size. */
#define QAPI_CRYPTO_HMAC_SHA512_MAX_KEY_BITS \
	(QAPI_CRYPTO_HMAC_SHA512_MAX_KEY_BYTES*8)
/** HMAC-SHA512 block size. */
#define QAPI_CRYPTO_HMAC_SHA512_BLOCK_BITS \
	(QAPI_CRYPTO_HMAC_SHA512_BLOCK_BYTES*8)
/** HMAC-SHA512 digest size. */
#define QAPI_CRYPTO_HMAC_SHA512_MAC_BITS \
	(QAPI_CRYPTO_HMAC_SHA512_MAC_BYTES*8)

/** HMAC-MD5 minimum key size. */
#define QAPI_CRYPTO_HMAC_MD5_MIN_KEY_BYTES (8)
/** HMAC-MD5 maximum key size. */
#define QAPI_CRYPTO_HMAC_MD5_MAX_KEY_BYTES (64)
/** HMAC-MD5 block size. */
#define QAPI_CRYPTO_HMAC_MD5_BLOCK_BYTES (64)
/** HMAC-MD5 digest size. */
#define QAPI_CRYPTO_HMAC_MD5_MAC_BYTES (16)

/** HMAC-MD5 minimum key size. */
#define QAPI_CRYPTO_HMAC_MD5_MIN_KEY_BITS \
	(QAPI_CRYPTO_HMAC_MD5_MIN_KEY_BYTES*8)
/** HMAC-MD5 maximum key size. */
#define QAPI_CRYPTO_HMAC_MD5_MAX_KEY_BITS \
	(QAPI_CRYPTO_HMAC_MD5_MAX_KEY_BYTES*8)
/** HMAC-MD5 block size. */
#define QAPI_CRYPTO_HMAC_MD5_BLOCK_BITS \
	(QAPI_CRYPTO_HMAC_MD5_BLOCK_BYTES*8)
/** HMAC-MD5 digest size. */
#define QAPI_CRYPTO_HMAC_MD5_MAC_BITS \
	(QAPI_CRYPTO_HMAC_MD5_MAC_BYTES*8)

/** SHA1 digest size. */
#define QAPI_CRYPTO_SHA1_DIGEST_BYTES (20)
/** SHA256 digest size. */
#define QAPI_CRYPTO_SHA256_DIGEST_BYTES (32)
/** SHA384 digest size. */
#define QAPI_CRYPTO_SHA384_DIGEST_BYTES (48)
/** SHA512 digest size. */
#define QAPI_CRYPTO_SHA512_DIGEST_BYTES (64)
/** MD5 digest size. */
#define QAPI_CRYPTO_MD5_DIGEST_BYTES (16)

/** SRP user name maximum length. */
#define QAPI_CRYPTO_SRP_UNAME_MAX_BYTES (64)
/** SRP password maximum length. */
#define QAPI_CRYPTO_SRP_PWD_MAX_BYTES (64)
/** SRP salt length. */
#define QAPI_CRYPTO_SRP_SALT_BYTES (16)

/** ECJPAKE password maximum size. */
#define QAPI_CRYPTO_ECJPAKE_PASSWORD_MAX_BYTES 256
/** ECJPAKE generated premaster secret size. */
#define QAPI_CRYPTO_ECJPAKE_PREMASTER_SECRET_BYTES 32
/** ECJPAKE identity maximum size. @newpage */
#define QAPI_CRYPTO_ECJPAKE_IDENTITY_MAX_BYTES 512

/** RSA minimum key size. */
#define QAPI_CRYPTO_RSA_MIN_KEY_SIZE_BITS (1024)

/** RSA maximum key size. */
#define QAPI_CRYPTO_RSA_MAX_KEY_SIZE_BITS (4096)

/** 
 * @brief Restricts the object usage
 * flags of an object handle to contain at most the flags passed in the
 * obj_usage parameter.
 *
 * @details 
 * For each bit in the parameter obj_usage:
 * - If the bit is set to 1, the corresponding usage flag in the object is
 * left unchanged.
 * - If the bit is set to 0, the corresponding usage flag in the object is
 * cleared.
 * For example, if the usage flags of the object are set to
 * QAPI_CRYPTO_USAGE_ENCRYPT | QAPI_CRYPTO_USAGE_DECRYPT and if obj_usage is set
 * to QAPI_CRYPTO_USAGE_ENCRYPT | QAPI_CRYPTO_USAGE_EXTRACTABLE, the only
 * remaining usage flag in the object after calling the function
 * qapi_Crypto_Obj_Usage_Restrict is QAPI_CRYPTO_USAGE_ENCRYPT.
 *
 * Note that an object usage flag can only be cleared. 
 *
 * A transient object's object usage flags are reset to 1 using the
 * qapi_Crypto_Transient_Obj_Reset function. 
 *
 * @param[in] hdl Object handle
 * @param[in] obj_usage Object usage. See #qapi_Crypto_Usage_Flags_t.
 * @return See QAPI error codes.
 */
int qapi_Crypto_Obj_Usage_Restrict(qapi_Crypto_Obj_Hdl_t hdl, uint32_t obj_usage);


/** 
 * @brief Resets a transient
 * object to its initial state after allocation.
 *
 * @details If the object is currently initialized, the function clears the
 * object of all its material. The object is then uninitialized again.  In any
 * case, the function resets the key usage of the container to 0xFFFFFFFFF. 
 *
 * @param[in] hdl Object handle.
 * @return int.
 */
int qapi_Crypto_Transient_Obj_Reset(qapi_Crypto_Obj_Hdl_t hdl);


/** 
 * @brief Returns the characteristics of
 * an object. 
 *
 * @details The function fills in the following fields in the structure
 * #qapi_Crypto_Obj_Info_t: 
 * - object_type: The parameter object_type passed when the object was
 * created.
 * - key_size: The current size in bits of the object as determined by its
 * attributes. This will always be less than or equal to max_key_size. Set to 0
 * for uninitialized objects.
 * - max_key_size: The maximum key_size that this object can represent.
 * Set to the parameter max_key_size passed to qapi_Crypto_Transient_Obj_Alloc().
 * - obj_usage: A bit vector of the #qapi_Crypto_Usage_Flags_t bits.
 * - handle_flags: Has QAPI_CRYPTO_HANDLE_INITIALIZED_E set when the transient object
 * becomes initialized using key_gen or populate API.
 *
 * @param[in]  hdl  Object handle.
 * @param[out] info Buffer to hold object information.
 * @return int.
 */
int qapi_Crypto_Obj_Info_Get(qapi_Crypto_Obj_Hdl_t hdl, qapi_Crypto_Obj_Info_t *info);


/** 
 * @brief Allocates a transient object.
 *
 * @details The function allocates an uninitialized
 * transient object, i.e., a container for attributes. Transient objects are used
 * to hold a cryptographic object (key or key-pair). The object type and the
 * maximum key size must be specified so that all the container resources can be
 * preallocated.
 * As allocated, the container is uninitialized. It can be initialized by
 * subsequently importing the object material, generating an object, deriving an
 * object, or loading an object from the Trusted Storage.
 * The initial value of the key usage associated with the container is
 * 0xFFFFFFFF,  which  means  that it contains all usage flags. The user can use the
 * function qapi_Crypto_Obj_Usage_Restrict() to restrict the usage of the
 * container.
 * The returned handle is used to refer to the newly-created container in all
 * subsequent functions that require an object container: key management and
 * operation functions. The handle remains valid until the  container is
 * deallocated using the function qapi_Crypto_Transient_Obj_Free().
 *
 * @param[in]  obj_type Object type (see qapi_Crypto_Obj_Type_t).
 * @param[in]  max_key_size Maximum key size in bits.
 * @param[out] obj Handle to the allocated object.
 * @return int.
 */
int qapi_Crypto_Transient_Obj_Alloc(uint32_t obj_type, uint32_t max_key_size, 
        qapi_Crypto_Obj_Hdl_t *obj);


/** 
 * @brief Generates a random key or a key pair and 
 * populates  a transient  key object with the generated key material.
 * 
 * @details The size of the intended key is passed in the key_size parameter and must be 
 * less than or equal to the maximum key size specified when the transient object
 * was created.
 *
 * @param[in] hdl Object handle.
 * @param[in] key_size Key size to be generated.
 * @param[in] attrs Key attributes.
 * @param[in] attr_count Number of attributes.
 * @return int.
 */
int qapi_Crypto_Transient_Obj_Key_Gen(qapi_Crypto_Obj_Hdl_t hdl, uint32_t key_size, 
        qapi_Crypto_Attrib_t *attrs, uint32_t attr_count);


/** 
 * @brief Populates an
 * uninitialized object container with object attributes passed by the TA in the
 * attrs parameter.
 *
 * @details When this function is called, the object must be uninitialized. If
 * the object is initialized, the caller must first clear it using the function
 * qapi_Crypto_Transient_Obj_Reset(). Note that if the object type is a key pair,
 * this function sets both the private and public parts of the key pair.
 *
 * @param[in] hdl Object handle.
 * @param[in] attrs Key attributes
 * @param[in] attr_count Number of attributes.
 *
 * @return int.
 */
int qapi_Crypto_Transient_Obj_Populate(qapi_Crypto_Obj_Hdl_t hdl, qapi_Crypto_Attrib_t *attrs, uint32_t attr_count);

/** 
 * @brief Deallocates a transient
 * object previously allocated with qapi_Crypto_Transient_Obj_Alloc(). 
 *
 * @details After this function has been called, the object handle is no longer
 * valid and all resources associated with the transient object must have been
 * reclaimed.  If the object is initialized, the object attributes are cleared
 * before the object is deallocated. 
 *
 * @param[in] hdl Object handle.
 * @return int.
 */
int qapi_Crypto_Transient_Obj_Free(qapi_Crypto_Obj_Hdl_t hdl);


/**
 * @brief Returns information about an
 * operation handle. 
 *
 * @details This function fills the following fields in the structure
 * #qapi_Crypto_Op_Info_t:
 * - algorithm, mode, max_key_size: The parameters passed to the function qapi_Crypto_Op_Alloc().
 * - key_size: The actual size of the key if a key is programmed in the operation. 
 * - digestLength: For a MAC, AE, or Digest digest, describes the number of bytes in the digest or tag.
 * - handleState: A bit vector describing the current state of the operation. 
 * Can contain any combination of the following flags or 0 if no flags are appropriate:
 *    -- QAPI_CRYPTO_HANDLE_FLAG_KEY_SET: Set if the operation key has been set. Always set for digest operations.
 *    -- QAPI_CRYPTO_HANDLE_FLAG_INITIALIZED: For multistage operations, whether the operation has been initialized 
 *    using one of the qti_crypto_op_XXX_init functions. This flag is always set for Digest operations.
 *
 * @param[in]  hdl Operation handle.
 * @param[out] info Crypto operation information.
 * @return int.
 */
int qapi_Crypto_Op_Info_Get(qapi_Crypto_Obj_Hdl_t hdl, qapi_Crypto_Op_Info_t *info);


/**
 * @brief Allocates a handle for a new
 * cryptographic operation and sets the mode and algorithm type. If this
 * function does not return with A_CRYPTO_SUCESS, there is no valid handle
 * value.
 *
 * @param[in]  alg Algorithm (see qapi_Crypto_Alg_t).
 * @param[in]  mode Crypto mode (see #qapi_Crypto_Mode_t).
 * @param[in]  max_key_size  Maximum key size used in bits in the operation.
 * @param[out] hdl Operation handle.
 * @return int.
 */
int qapi_Crypto_Op_Alloc(uint32_t alg, uint32_t mode, uint32_t max_key_size, qapi_Crypto_Op_Hdl_t *hdl);

/**
 * @brief Deallocates all resources associated
 * with an operation handle. After this function is called, the operation handle
 * is no longer valid. All cryptographic material in the operation is destroyed.
 *
 * @param[in] hdl Operation handle.
 * @return int.
 */
int qapi_Crypto_Op_Free(qapi_Crypto_Op_Hdl_t hdl);

/**
 * @brief Resets the operation to the initial
 * state before initialization, but after the key has been set.
 *
 * @details This function can be called on any operation and at any time after
 * the key is set, but is meaningful only for multistage operations, i.e.
 * symmetric ciphers, MACs, AEs, and digests.
 *
 * @param[in] hdl Operation handle.
 * @return int.
 */
int qapi_Crypto_Op_Reset(qapi_Crypto_Op_Hdl_t hdl);

/**
 * @brief Programs the key of an operation, i.e.,
 * it associates an operation with a key.
 *
 * @details The key material is copied from the key object handle into the
 * operation.  After the key has been set, there is no longer a link between
 * the operation and the key object. The object handle can be closed or reset
 * and this will not affect the operation. This copied material exists until the
 * operation is freed using qapi_Crypto_Op_Free() or another key is set into the
 * operation.
 *
 * The key object type and size must be compatible with the type and size of the
 * operation. The operation mode must be compatible with key usage.
 *
 * @param[in] op_hdl Operation handle
 * @param[in] obj_hdl Key handle
 * @return int.
 */
int qapi_Crypto_Op_Key_Set(qapi_Crypto_Op_Hdl_t op_hdl, qapi_Crypto_Obj_Hdl_t obj_hdl);

/**
 * @brief Copies an operation state from one
 * operation handle into another operation handle. This also copies the key
 * material associated with the source operation.
 *
 * @details This function is useful in the following use cases:
 * - "Forking" a digest operation after feeding some amount of initial data
 * - Computing intermediate digests
 *
 * @param[in] dst_hdl Handle on the destination operation.
 * @param[in] src_hdl Handle on the source operation.
 * @return int.
 */
int qapi_Crypto_Op_Copy(qapi_Crypto_Op_Hdl_t dst_hdl, qapi_Crypto_Op_Hdl_t src_hdl);

/**
 * @brief Verifies a message digest
 * signature within an asymmetric operation. ECDSA signatures should be in
 * ASN.1 format.
 *
 * @param[in] hdl Operation handle.
 * @param[in] params Attributes.
 * @param[in] param_count Number of parameters.
 * @param[in] digest Message digest.
 * @param[in] digest_len Digest length.
 * @param[in] sign Signature.
 * @param[in] sign_len Signature length.
 *
 * @return int.
 */
int qapi_Crypto_Op_Verify_Digest(qapi_Crypto_Op_Hdl_t hdl, qapi_Crypto_Attrib_t *params, uint32_t param_count,
                    void *digest, uint32_t digest_len, void *sign,  uint32_t sign_len);

/**
 * @brief Signs a message digest within an
 * asymmetric operation. Note that only an already-hashed message can be signed.
 * ECDSA signatures are in ASN.1 format. The signature buffer should be allocated
 * to a minimum size given by the macro #QAPI_CRYPTO_ECDSA_SIGNATURE_BUFFER_SIZE_BYTES
 * for ECDSA and #QAPI_CRYPTO_RSA_SIGNATURE_BUFFER_SIZE_BYTES for RSA.
 *
 * @param[in] hdl Operation handle.
 * @param[in] params Attributes.
 * @param[in] param_count Number of parameters.
 * @param[in] digest Message digest.
 * @param[in] digest_len Digest length.
 * @param[in] sign Signature.
 * @param[in,out] sign_len Size of the sign buffer in bytes (in). Signature length (out).
 *
 * @return int.
 */
int qapi_Crypto_Op_Sign_Digest(qapi_Crypto_Op_Hdl_t hdl, qapi_Crypto_Attrib_t *params, uint32_t param_count,
                    void *digest, uint32_t digest_len, void *sign,  uint32_t *sign_len);


/**
 * @brief Starts the symmetric cipher
 * operation. The operation must have been associated with a key.
 *
 * @details The supported Symmetric Cipher algorithms are:
 * - QAPI_CRYPTO_ALG_AES_CBC_NOPAD
 * - QAPI_CRYPTO_ALG_AES_CTR
 * 
 * @param[in] hdl Operation handle.
 * @param[in] IV Buffer containing the operation initialization vector (IV) or the initial counter value, as appropriate.
 * @param[in] IVLen  Length of the IV in bytes.
 *
 * @return int.
 */
 int qapi_Crypto_Op_Cipher_Init(qapi_Crypto_Op_Hdl_t hdl, void *IV, uint32_t IVLen);


/**
 * @brief Encrypts or decrypts input data.
 *
 * @details Input data must be a multiple of the block size.
 *
 * The supported Symmetric Cipher algorithms are:
 * - QAPI_CRYPTO_ALG_AES_CBC_NOPAD
 *
 * @param[in]  hdl Operation handle.
 * @param[in]  srcData  Input data buffer to be encrypted or decrypted.
 * @param[in]  srcLen   Length of the input data.
 * @param[out] destData Output buffer.
 * @param[in,out] destLen  Size of destData buffer in bytes (in). Length of the data in destData in bytes (out).
 *
 * @return int.
 */
int qapi_Crypto_Op_Cipher_Update(qapi_Crypto_Op_Hdl_t hdl, void *srcData, uint32_t srcLen, void *destData, uint32_t *destLen);


/** 
 * @brief Finalizes the cipher
 * operation, processing data that has not been processed by previous calls to
 * qapi_Crypto_Op_Cipher_Update() as well as data supplied in srcData. The
 * operation handle can be reused or reinitialized. 
 *
 * @details Note that for the *_NOPAD symmetric algorithms, it is the
 * responsibility of the application to do the padding, so input data must be a
 * multiple of block size.
 *
 * Also, for QAPI_CRYPTO_ALG_AES_CTR, if one of the chunks supplied to
 * the qapi_Crypto_Op_Cipher_Update() function was not a multiple of 16 bytes, the
 * destination buffer length must be at least ( ceil(srcLen/16) * 16 ) bytes.
 * long.
 *
 * The macro QAPI_CRYPTO_AES_ENCRYPT_CIPHER_TEXT_BUFFER_SIZE_BYTES should be used to determine
 * the size of the destData buffer to allocate.
 *
 * The supported Symmetric Cipher algorithms are:
 * - QAPI_CRYPTO_ALG_AES_CBC_NOPAD
 * - QAPI_CRYPTO_ALG_AES_CTR
 * 
 * @param[in]  hdl Operation handle.
 * @param[in]  srcData  Input data buffer to be encrypted or decrypted.
 * @param[in]  srcLen   Legnth of the input data.
 * @param[out] destData Output buffer.
 * @param[in,out] destLen  Size of destData buffer in bytes (in). Length of the data in destData in bytes (out).
 *
 * @return int.
 */
int qapi_Crypto_Op_Cipher_Final(qapi_Crypto_Op_Hdl_t hdl, void *srcData, uint32_t srcLen, void *destData, uint32_t *destLen);

/**
 * @brief Extracts one buffer attribute from an object.
 *
 * @details The attribute is identified by the argument attribute_id.  
 *
 * There are two kinds of object attributes, which are identified by bit 28 in
 * their handle value. If bit 28 is set to 1, the attribute is public. If set to 0,
 * the attribute is protected.  Public object attributes can always be extracted,
 * whatever the status of the container is. Protected attributes can be extracted
 * only if the object's key usage contains the QAPI_CRYPTO_USAGE_EXTRACTABLE
 * flag. QAPI_CRYPTO_USAGE_EXTRACTABLE is not supported for persistent objects, i.e.,
 * protected attributes cannot be extracted for persistent objects.
 * 
 * @param[in]  hdl Object handle.
 * @param[in]  attrib_id Attribute ID (see qapi_Crypto_Attr_Type_t).
 * @param[out] buffer  Output buffer that is to hold the attribute.
 * @param[in] size  Size of the output buffer.
 * @return int.
 */
int qapi_Crypto_Obj_Buf_Attrib_Get(qapi_Crypto_Obj_Hdl_t hdl, uint32_t attrib_id, void *buffer, uint32_t size);

/**
 * @brief Extracts a value attribute from an object. 
 *
 * @details The attribute is identified by the argument attribute_id.  
 *
 * There are two kinds of object attributes, which are identified by bit 28 in
 * their handle value. If bit 28 is set to 1, the attribute is public. If set to 0,
 * the attribute is protected. Public object attributes can always be extracted,
 * whatever the status of the container is. Protected attributes can be extracted
 * only if the object's key usage contains the QAPI_CRYPTO_USAGE_EXTRACTABLE
 * flag.
 *
 * @param[in]  hdl Object handle.
 * @param[in]  attrib_id Attribute ID (see qapi_Crypto_Attr_Type_t).
 * @param[out] a Attribute value.
 * @param[out] b Attribute value if size > 4 bytes.
 * @return int.
 */
int qapi_Crypto_Obj_Val_Attrib_Get(qapi_Crypto_Obj_Hdl_t hdl, uint32_t attrib_id, uint32_t *a, uint32_t *b);

/** 
 * @brief Retrieves len random bytes into *ptr. Uses a hardware random number
 * generator.
 *
 * @param[out] buffer Pointer to a buffer to which to return random bytes.
 * @param[in] len Number of random bytes to return.
 * @return int.
 */
int qapi_Crypto_Random_Get(void *buffer, uint16_t len);

/**
 * @brief Initializes an Authentication Encryption operation.
 *
 * @details The supported Authenticated Encryption algorithms are:
 * - QAPI_CRYPTO_ALG_AES_CCM
 * - QAPI_CRYPTO_ALG_AES_GCM
 * - QAPI_CRYPTO_ALG_CHACHA20_POLY1305
 *
 * @param[in] hdl        Operation handle.
 * @param[in] nonce      Operation nonce or IV.
 * @param[in] nonceLen   Length in bytes of the nonce. \n
 *                       For AES-GCM and ChaCha20, the length must be 12. \n
 *                       For AES-CCM, the length must be >= 7 and <= 13.
 * @param[in] tagLen     Size of the tag in bits. \n
 *                       For AES-GCM and ChaCha20, the length must be 128. \n
 *                       For AES-CCM, the length must be a multiple of 8, >= 32, and <= 128.
 * @param[in] AADLen     Length of the AAD in bytes.
 * @param[in] payloadLen Length of the payload in bytes.
 *
 * @return int.
 */
 int qapi_Crypto_Op_AE_Init(qapi_Crypto_Op_Hdl_t hdl, void *nonce, uint32_t nonceLen, uint32_t tagLen, uint32_t AADLen, uint32_t payloadLen);

/**
 * @brief Feeds a new chunk of
 * Additional Authentication Data (AAD) to the AE operation.  
 *
 * @details Note that subsequent calls to this function are available only for
 * the QAPI_CRYPTO_ALG_CHACHA20_POLY1305 algorithm.
 *
 * The supported Authenticated Encryption algorithms are:
 * - QAPI_CRYPTO_ALG_AES_CCM
 * - QAPI_CRYPTO_ALG_AES_GCM
 * - QAPI_CRYPTO_ALG_CHACHA20_POLY1305
 *
 * @param[in] hdl       Operation handle.
 * @param[in] AAData    Input buffer containing the chunk of AAD.
 * @param[in] AADataLen Length of the AAD in bytes.
 *
 * @return int.
 */
 int qapi_Crypto_Op_AE_AAD_Update(qapi_Crypto_Op_Hdl_t hdl, void *AAData, uint32_t AADataLen);

/**
 * @brief Accumulates data for an
 * Authentication Encryption operation.
 *
 * @details Input data does not have to be a multiple of block size. Subsequent
 * calls to this function are possible. Unless one or more calls of this
 * function have supplied sufficient input data, no output is generated.
 
 * @note1hang When using this routine to decrypt, the returned data may be corrupt,
 * since the integrity check is not performed until all the data has been
 * processed. If this is a concern, use only the qti_crypto_op_ae_*_final
 * routine. 
 *
 * Note that this function is only supported for the
 * QAPI_CRYPTO_ALG_CHACHA20_POLY1305 algorithm.
 *
 * Notes related to the QAPI_CRYPTO_ALG_CHACHA20_POLY1305 algorithm:
 *  - For optimal performance, it is preferable that the input data length is a
 *  multiple of 64 bytes.  If the input data length of one of the chunks is not
 *  a multiple of 64 bytes, an internal buffer will be used, and this will
 *  slightly impact the performance.
 *  - If the input data length of one of the chunks is not a multiple of 64
 *  bytes, the destination buffer length for this chunk and subsequent chunks
 *  must be at least ( ceil(srcLen/64) * 64 ) bytes long.
 *
 * @param[in]  hdl Operation handle.
 * @param[in]  srcData Input data buffer to be encrypted or decrypted.
 * @param[in]  srcLen  Length of srcData in bytes.
 * @param[out] destData Output buffer.
 * @param[in,out] destLen  Size of the destData buffer in bytes (in). Length of the data in destData in bytes (out).
 * @return int.
 */
int qapi_Crypto_Op_AE_Update(qapi_Crypto_Op_Hdl_t hdl, void *srcData, uint32_t srcLen, void *destData, uint32_t *destLen);

/**
 * @brief Processes data that has
 * not been processed by previous calls to qapi_Crypto_Op_AE_Update(), as well as
 * data supplied in srcData. It completes the AE operation and computes the tag.
 *
 * @details The supported Authenticated Encryption algorithms are:
 * - QAPI_CRYPTO_ALG_AES_CCM 
 * - QAPI_CRYPTO_ALG_AES_GCM 
 * - QAPI_CRYPTO_ALG_CHACHA20_POLY1305
 *
 * The macro QAPI_CRYPTO_AES_ENCRYPT_CIPHER_TEXT_BUFFER_SIZE_BYTES should be used to 
 * determine the length of the destData buffer to allocate for AES CCM and GCM.
 *
 * Notes related to the QAPI_CRYPTO_ALG_CHACHA20_POLY1305 algorithm:
 *  - If one of the chunks supplied to the qapi_Crypto_Op_AE_Update() function was
 *  not a multiple of 64 bytes, the destination buffer length must be at
 *  least ( ceil(srcLen/64) * 64 ) bytes long.
 *  - If the operation is to be reused after qapi_Crypto_Op_AE_Encrypt_Final() is
 *  called, the qapi_Crypto_Op_AE_Init() and the qapi_Crypto_Op_AE_AAD_Update()
 *  functions must be called before calling qapi_Crypto_Op_AE_Update() or
 *  qti_crypto_op_ae*final()
 *  - The macro QAPI_CRYPTO_CHACHA20_POLY1305_ENCRYPT_CIPHER_TEXT_BUFFER_SIZE_BYTES
 *  should be used to determine the size of the destData buffer to allocate.
 *
 * @param[in]  hdl      Operation handle
 * @param[in]  srcData  Reference to the final chunk of input data to be encrypted.
 * @param[in]  srcLen   Length of srcData in bytes.
 * @param[out] destData Output buffer. Can be omitted if the output is to be discarded, e.g., because it is known to be empty.
 * @param[in,out] destLen  Size of the destData buffer in bytes (in). Length of the cipher text in destData in bytes (out).
 * @param[out] tag      Output buffer filled with the computed tag.
 * @param[in,out] tagLen   Size of the tag buffer in bits (in).  Length of the computed tag in bits (out).
 * @return int.
 */
int qapi_Crypto_Op_AE_Encrypt_Final(qapi_Crypto_Op_Hdl_t hdl, void *srcData, uint32_t srcLen, void *destData, uint32_t *destLen, void *tag, uint32_t *tagLen);

/**
 * @brief Processes data that has
 * not been processed by previous calls to qti_crypto_ae_update, as well as data
 * supplied in srcData. It completes the AE operation and compares the computed
 * tag with the tag supplied in the parameter tag. 
 *
 * @details The supported Authenticated Encryption algorithms are:
 * - QAPI_CRYPTO_ALG_AES_CCM
 * - QAPI_CRYPTO_ALG_AES_GCM
 * - QAPI_CRYPTO_ALG_CHACHA20_POLY1305
 *
 * Notes related to the QAPI_CRYPTO_ALG_CHACHA20_POLY1305 algorithm:
 *  - If one of the chunks supplied to the qapi_Crypto_Op_AE_Update() function was
 *  not a multiple of 64 bytes, the destination buffer length must be at
 *  least ( ceil(srcLen/64) * 64 ) bytes long.
 *  - If the operation is to be reused after qapi_Crypto_Op_AE_Decrypt_Final() is
 *  called, the qapi_Crypto_Op_AE_Init() and the qapi_Crypto_Op_AE_AAD_Update()
 *  functions must be called before calling qapi_Crypto_Op_AE_Update() or
 *  qti_crypto_op_ae*final()
 *
 * @param[in]  hdl      Operation handle.
 * @param[in]  srcData  Reference to the final chunk of input data to be encrypted.
 * @param[in]  srcLen   Length of srcData in bytes.
 * @param[out] destData Output buffer. Can be omitted if the output is to be discarded, e.g., because it is known to be empty.
 * @param[in,out] destLen  Size of the destData buffer in bytes (in). Length of the plain text in destData in bytes (out).
 * @param[in]  tag      Input buffer containing the tag to compare.
 * @param[in,out] tagLen   Size of the tag buffer in bits (in). Length of the computed tag in bits (out).
 * @return int.
 */
int qapi_Crypto_Op_AE_Decrypt_Final(qapi_Crypto_Op_Hdl_t hdl, void *srcData, uint32_t srcLen, void *destData, uint32_t *destLen, void *tag, uint32_t tagLen);

/**
 * @brief Used to derive a key using a shared secret algorithm. 
 *
 * @details The derived_key_hdl must refer to an object of the type
 * QAPI_CRYPTO_OBJ_TYPE_GENERIC_SECRET. On completion, the derived key is placed
 * into the QAPI_CRYPTO_ATTR_SECRET_VALUE attribute of the derivedKey handle.
 * 
 * The supported shared secret algorithms and the corresponding attributes are:	\n
 * Algorithm: QAPI_CRYPTO_ALG_CURVE25519_DERIVE_SHARED_SECRET \n
 *     Attributes:
 *     - QAPI_CRYPTO_ATTR_CURVE25519_PUBLIC_VALUE
 * Algorithm: QAPI_CRYPTO_ALG_ECDH_P \n
 *     Attributes:
 *     - QAPI_CRYPTO_ATTR_ECC_PUBLIC_VALUE_X
 *     - QAPI_CRYPTO_ATTR_ECC_PUBLIC_VALUE_Y
 * Algorithm: QAPI_CRYPTO_ALG_DH \n
 *     Attributes:
 *     - QAPI_CRYPTO_ATTR_DH_PUBLIC_VALUE
 * Algorithm: QAPI_CRYPTO_ALG_ECJPAKE \n
 *     Attributes:
 *     - QAPI_CRYPTO_ATTR_ECJPAKE_ROUND2_PUBLIC_KEY
 *     - QAPI_CRYPTO_ATTR_ECJPAKE_ROUND2_ZKP_EPHEMERAL_PUBLIC_KEY
 *     - QAPI_CRYPTO_ATTR_ECJPAKE_ROUND2_ZKP_SIGNATURE
 *
 * @param[in] op_hdl  Operation handle.
 * @param[in] attrs  Attributes (see the list above).
 * @param[in] attr_count  Number of attributes.
 * @param[in] derived_key_hdl  Transient object where the derived key value is to be stored.
 *
 * @return int.
 */
int qapi_Crypto_Op_Key_Derive(qapi_Crypto_Op_Hdl_t op_hdl, qapi_Crypto_Attrib_t *attrs, 
        uint32_t attr_count, qapi_Crypto_Obj_Hdl_t derived_key_hdl);

/**
 * @brief Used to derive intermediate
 * public keys that are shared with the remote party in the key exchange.
 *
 * @details This function is useful for key exchange algorithms
 * that involve multiple rounds between two parties where intermediate public keys are
 * shared between the parties in each round. The final round of the key exchange should
 * call qapi_Crypto_Op_Key_Derive() to derive the shared secret key.
 *
 * The supported shared secret algorithms and the corresponding attributes are:	\n
 * Algorithm: QAPI_CRYPTO_ALG_ECJPAKE \n
 *     Derived key object:
 *     QAPI_CRYPTO_OBJ_TYPE_ECJPAKE_ROUND1_PUBLIC_KEY_AND_ZKP_PAIR \n
 *         Attributes:
 *         - QAPI_CRYPTO_ATTR_ECJPAKE_IDENTITY
 *         - QAPI_CRYPTO_ATTR_ECC_CURVE
 *
 *     Derived key object:
 *     QAPI_CRYPTO_OBJ_TYPE_ECJPAKE_ROUND2_PUBLIC_KEY_AND_ZKP \n
 *         Attributes:
 *         - QAPI_CRYPTO_ATTR_ECJPAKE_REMOTE_IDENTITY
 *	       - QAPI_CRYPTO_ATTR_ECJPAKE_ROUND1_PUBLIC_KEY_1
 *	       - QAPI_CRYPTO_ATTR_ECJPAKE_ROUND1_ZKP_EPHEMERAL_PUBLIC_KEY_1
 *	       - QAPI_CRYPTO_ATTR_ECJPAKE_ROUND1_ZKP_SIGNATURE_1
 *	       - QAPI_CRYPTO_ATTR_ECJPAKE_ROUND1_PUBLIC_KEY_2
 *	       - QAPI_CRYPTO_ATTR_ECJPAKE_ROUND1_ZKP_EPHEMERAL_PUBLIC_KEY_2
 *	       - QAPI_CRYPTO_ATTR_ECJPAKE_ROUND1_ZKP_SIGNATURE_2
 *
 * @param[in] op_hdl  Operation handle.
 * @param[in] attrs  Attributes (see the list above).
 * @param[in] attr_count  Number of attributes.
 * @param[in] intermediate_derived_key_hdl  Object where the intermediate key value is to be stored.
 *
 * @return int.
 */
int qapi_Crypto_Op_Intermediate_Key_Derive(qapi_Crypto_Op_Hdl_t op_hdl, qapi_Crypto_Attrib_t *attrs,
        uint32_t attr_count, qapi_Crypto_Obj_Hdl_t intermediate_derived_key_hdl);
		
/**
 * @brief Initializes a MAC operation. The
 * operation must have been associated with a key. Note that if the IV for a MAC
 * operation is constant, the IV is ignored and can be NULL.
 *
 * @details The supported MAC algorithms are:
 * - QAPI_CRYPTO_ALG_HMAC_SHA1
 * - QAPI_CRYPTO_ALG_HMAC_SHA256
 * - QAPI_CRYPTO_ALG_HMAC_SHA384
 * - QAPI_CRYPTO_ALG_HMAC_SHA512
 * - QAPI_CRYPTO_ALG_HMAC_MD5
 * - QAPI_CRYPTO_ALG_AES_CMAC
 *
 * @param[in] hdl  Operation handle.
 * @param[in] IV   Buffer containing the operation IV, if applicable.
 * @param[in] IVLen   Length of the IV in bytes.
 *
 * @return int.
 */
 int qapi_Crypto_Op_Mac_Init(qapi_Crypto_Op_Hdl_t hdl, void *IV, uint32_t IVLen);


/**
 * @brief Accumulates data for a MAC calculation.
 *
 * @details Input data does not have to be a multiple of the block size.
 * Subsequent calls to this function are possible.
 * qapi_Crypto_Op_Mac_Final_Compute() or qapi_Crypto_Op_Mac_Final_Compare() are
 * called to complete the MAC operation.
 *
 * Note that this function can be called only with an operation of the following
 * algorithms:
 * - QAPI_CRYPTO_ALG_HMAC_SHA1
 * - QAPI_CRYPTO_ALG_HMAC_SHA256
 * - QAPI_CRYPTO_ALG_HMAC_SHA384
 * - QAPI_CRYPTO_ALG_HMAC_SHA512
 * - QAPI_CRYPTO_ALG_HMAC_MD5
 * - QAPI_CRYPTO_ALG_AES_CMAC
 *
 * @param[in] hdl  Operation handle.
 * @param[in] chunk    Chunk of the message to be MACed.
 * @param[in] chunkSize   Length of the chunk in bytes.
 *
 * @return int.
 */
int qapi_Crypto_Op_Mac_Update(qapi_Crypto_Op_Hdl_t hdl, void *chunk, uint32_t chunkSize);


/**
 * @brief Finalizes the MAC
 * operation with a last chunk of message, and computes the MAC.
 *
 * @details The supported MAC algorithms are:
 * - QAPI_CRYPTO_ALG_HMAC_SHA1
 * - QAPI_CRYPTO_ALG_HMAC_SHA256
 * - QAPI_CRYPTO_ALG_HMAC_SHA384
 * - QAPI_CRYPTO_ALG_HMAC_SHA512
 * - QAPI_CRYPTO_ALG_HMAC_MD5
 * - QAPI_CRYPTO_ALG_AES_CMAC
 *
 * @param[in] hdl  Operation handle.
 * @param[in] message    Input buffer containing a last message chunk to MAC.
 * @param[in] messageLen   Length of the message in bytes.
 * @param[out] mac    Output buffer filled with the computed MAC.
 * @param[in,out] macLen   Size of the MAC buffer in bytes (in). Length of computed MAC in bytes (out).
 *
 * @return int.
 */
int qapi_Crypto_Op_Mac_Final_Compute(qapi_Crypto_Op_Hdl_t hdl, void *message, uint32_t messageLen, void *mac, uint32_t *macLen);

/**
 * @brief Finalizes the MAC
 * operation and compares the MAC with the buffer passed to the function.
 *
 * @details The supported MAC algorithms are:
 * - QAPI_CRYPTO_ALG_HMAC_SHA1
 * - QAPI_CRYPTO_ALG_HMAC_SHA256
 * - QAPI_CRYPTO_ALG_HMAC_SHA384
 * - QAPI_CRYPTO_ALG_HMAC_SHA512
 * - QAPI_CRYPTO_ALG_HMAC_MD5
 * - QAPI_CRYPTO_ALG_AES_CMAC
 *
 * @param[in] hdl  Operation handle.
 * @param[in] message    Input buffer containing a last message chunk to MAC.
 * @param[in] messageLen   Length of the message in bytes.
 * @param[in] mac    Input buffer containing the MAC to check.
 * @param[in] macLen   Length of MAC in bytes.
 *
 * @return int.
 */
int qapi_Crypto_Op_Mac_Final_Compare(qapi_Crypto_Op_Hdl_t hdl, void *message, uint32_t messageLen, void *mac, uint32_t macLen);

/**
 * @brief Accumulates message data for hashing. 
 *
 * @details The message does not have to be block aligned. Subsequent calls to
 * this function are possible.
 *
 * The supported digest algorithms are:
 * - QAPI_CRYPTO_ALG_SHA1
 * - QAPI_CRYPTO_ALG_SHA256
 * - QAPI_CRYPTO_ALG_SHA384
 * - QAPI_CRYPTO_ALG_SHA512
 * - QAPI_CRYPTO_ALG_MD5
 *
 * @param[in] hdl  Operation handle.
 * @param[in] chunk    Chunk of data to be hashed.
 * @param[in] chunkSize   Length of the chunk in bytes.
 *
 * @return int.
 */
int qapi_Crypto_Op_Digest_Update(qapi_Crypto_Op_Hdl_t hdl, void *chunk, uint32_t chunkSize);

/**
 * @brief Finalizes the message digest
 * operation and produces the message hash.
 *
 * @details The supported digest algorithms are:
 * - QAPI_CRYPTO_ALG_SHA1
 * - QAPI_CRYPTO_ALG_SHA256
 * - QAPI_CRYPTO_ALG_SHA384
 * - QAPI_CRYPTO_ALG_SHA512
 * - QAPI_CRYPTO_ALG_MD5
 *
 * @param[in] hdl  Operation handle.
 * @param[in] chunk    Last chunk of data to be hashed.
 * @param[in] chunkSize   Length of the chunk in bytes.
 * @param[out] hash    Output buffer filled with the message hash.
 * @param[in,out] hashLen   Size of the hash buffer in bytes (in). Length of the computed hash in bytes (out).
 *
 * @return int.
 */
int qapi_Crypto_Op_Digest_Final(qapi_Crypto_Op_Hdl_t hdl, void *chunk, uint32_t chunkSize, void *hash, uint32_t *hashLen);

/**
 * @brief Encrypts a message within an
 * asymmetric operation.
 *
 * @details The supported asymmetirc encrypt algorithms are:
 * - QAPI_CRYPTO_ALG_RSAES_PKCS1_V1_5
 * - QAPI_CRYPTO_ALG_RSA_NOPAD
 *
 * @param[in] hdl  Operation handle.
 * @param[in] params  Attributes.
 * @param[in] param_count Number of parameters.
 * @param[in] srcData  Input buffer.
 * @param[in] srcLen Length of srcData in bytes.
 * @param[out] destData   Output buffer.
 * @param[in,out] destLen Size of destData buffer in bytes, which should at least the RSA modulus size (in). Length of the cipher text in bytes (out).
 *
 * @return int.
 */
int qapi_Crypto_Op_Asym_Encrypt(qapi_Crypto_Op_Hdl_t hdl, qapi_Crypto_Attrib_t *params, uint32_t param_count,
                    void *srcData, uint32_t srcLen, void *destData,  uint32_t* destLen);

/**
 * @brief Decrypts a message within an
 * asymmetric operation.
 *
 * @details The supported asymmetirc decrypt algorithms are:
 * - QAPI_CRYPTO_ALG_RSAES_PKCS1_V1_5
 * - QAPI_CRYPTO_ALG_RSA_NOPAD
 *
 * @param[in] hdl  Operation handle.
 * @param[in] params  Attributes.
 * @param[in] param_count Number of parameters.
 * @param[in] srcData  Input buffer.
 * @param[in] srcLen Length of srcData in bytes.
 * @param[out] destData   Output buffer.
 * @param[in,out] destLen Size of the destData buffer in bytes, which should at least the RSA modulus size (in). Length of plain text in bytes (out).
 *
 * @return int.
 */
int qapi_Crypto_Op_Asym_Decrypt(qapi_Crypto_Op_Hdl_t hdl, qapi_Crypto_Attrib_t *params, uint32_t param_count,
                    void *srcData, uint32_t srcLen, void *destData,  uint32_t* destLen);


/**
 * @brief Zeroes the buffer securely.
 *
 * @details Secures memzero, which voids being optimized out by the compiler.
 * Should be used to zero passwords and other secret data.
 *
 * @param[out] buffer   Buffer to zero.
 * @param[out] bufferLen Length of buffer in bytes.
 */
void qapi_Crypto_Secure_Memzero(void *buffer, uint32_t bufferLen);

/**
 * @brief Creates a persistent object.
 *
 * @details Creates a persistent object using a transient object or
 * another persistent object as a template. The handle to this
 * template object is provided in the attributes parameter. The
 * object type, size, usage, and object attributes are initialized
 * from this object.
 *
 * @param[in] storage_Id   Storage ID.
 * Must be QAPI_CRYPTO_PERSISTENT_OBJ_DATA_TEE_STORAGE_PRIVATE.
 * @param[in] object_Id Object identifier. At most 64 characters.
 * @param[in] object_Id_Len Length of the object identifier.
 * @param[in] flags
 * Supported flags: \n
 * QAPI_CRYPTO_DATA_EXCLUSIVE -- Creates an object if it does not already
 * exist, otherwise returns an error.
 * @param[in] attributes Handle to s transient or persistent object.
 * @param[in] initial_Data Reserved for future use.
 * @param[in] initial_Data_Len Reserved for future use.
 * @param[out] hdl Handle to the newly created object.
 *
 * @return QAPI_OK on success or another error code on failure.
 */
qapi_Status_t qapi_Crypto_Persistent_Obj_Create(
		uint32_t storage_Id,
		uint8_t * object_Id,
		uint32_t object_Id_Len,
		uint32_t flags,
		qapi_Crypto_Obj_Hdl_t attributes,
		uint8_t *initial_Data,
		uint32_t initial_Data_Len,
		qapi_Crypto_Obj_Hdl_t *hdl);

/**
 * @brief Reads the key pair or public associated with a certificate
 * in the certificate store and saves it to a new persistent object.
 *
 * @param[in] storage_Id   Storage ID.
 * Must be QAPI_CRYPTO_PERSISTENT_OBJ_DATA_TEE_STORAGE_PRIVATE.
 * @param[in] object_Id Object identifier. At most 64 characters.
 * @param[in] object_Id_Len Length of the object identifier.
 * @param[in] certificate_Name Name of the certificate file in the
 * certificate store.
 * @param[in] flag
 * Supported flags: \n
 * QAPI_CRYPTO_DATA_EXCLUSIVE -- Creates an object if it does not already
 * exist, otherwise returns an error.
 * @param[in] object_Type (Optional) Type of the persistent object to create or 0
 * if the type should be inferred from the certificate.  The type if
 * specified must be compatible with the certificate (ECC or RSA).
 * If the type is 0 and the certificate has an associated ECC key pair,
 * it is ambiguous whether the type is QAPI_CRYPTO_OBJ_TYPE_ECDSA_KEYPAIR_E or
 * QAPI_CRYPTO_OBJ_TYPE_ECDH_KEYPAIR_E.  By default it is assumed to be 
 * QAPI_CRYPTO_OBJ_TYPE_ECDSA_KEYPAIR_E unless the object_Usage is 
 * QAPI_CRYPTO_USAGE_DERIVE_E in which case the type will be inferred as
 * QAPI_CRYPTO_OBJ_TYPE_ECDH_KEYPAIR_E.
 * @param[in] object_Usage Object usage (see qapi_Crypto_Usage_Flags_t).
 * @param[out] hdl Handle to the newly created object.
 *
 * @return QAPI_OK on success or another error code on failure.
 */
qapi_Status_t qapi_Crypto_Persistent_Obj_Import_From_Cert_Store(
		uint32_t storage_Id,
		uint8_t * object_Id,
		uint32_t object_Id_Len,
		const char *certificate_Name,
		uint32_t flags,
		uint32_t object_Type,
		uint32_t object_Usage,
		qapi_Crypto_Obj_Hdl_t *hdl);

/**
 * @brief Opens and returns a handle to
 * a previously created persistent object.
 *
 * @param[in] storage_Id   Storage ID.
 * Must be QAPI_CRYPTO_PERSISTENT_OBJ_DATA_TEE_STORAGE_PRIVATE.
 * @param[in] object_Id Object identifier. At most 64 characters.
 * @param[in] object_Id_Len Length of the object identifier.
 * @param[in] flags
 * Supported flags: \n
 * QAPI_CRYPTO_DATA_ACCESS_WRITE_META -- Opens a handle suitable for
 * renaming or deleting the persistent object.
 * @param[out] hdl Handle to the opened object.
 *
 * @return QAPI_OK on success or another error code on failure.
 */
qapi_Status_t qapi_Crypto_Persistent_Obj_Open(
		uint32_t storage_Id,
		uint8_t * object_Id,
		uint32_t object_Id_Len,
		uint32_t flags,
		qapi_Crypto_Obj_Hdl_t *hdl);

/**
 * @brief Closes the handle to an object.
 *
 * @param[in] hdl   Handle to the persistent object.
 *
 * @return None.
 */
void qapi_Crypto_Persistent_Obj_Close(qapi_Crypto_Obj_Hdl_t hdl);

/**
 * @brief Renames a persistent object to a new object ID.
 *
 * @param[in] hdl   Handle to the persistent object
 * @param[in] new_object_Id New object identifier. At most 64 characters.
 * @param[in] new_object_Id_Len Length of the new object identifier.
 *
 * @return QAPI_OK on success or another error code on failure.
 */
qapi_Status_t qapi_Crypto_Persistent_Obj_Rename (qapi_Crypto_Obj_Hdl_t hdl,
		uint8_t* new_object_Id,
		uint32_t new_object_Id_Len);

/**
 * @brief Closes the handle to an
 * object and deletes it from persistent storage.
 *
 * @param[in] hdl   Handle to the persistent object.
 *
 * @return QAPI_OK on success or another error code on failure.
 */
qapi_Status_t qapi_Crypto_Persistent_Obj_Close_and_Delete( qapi_Crypto_Obj_Hdl_t hdl );

/*
 * @brief Allocates an enumerator
 * for listing the persistent objects on the device.
 *
 * @param[out] hdl   Handle to the object enumerator.
 *
 * @return QAPI_OK on success or another error code on failure.
 */
qapi_Status_t qapi_Crypto_Persistent_Obj_Enumerator_Alloc(
		qapi_Crypto_Enumerator_Hdl_t *hdl);

/*
 * @brief Frees the object enumerator.
 *
 * @param[in] hdl   Handle to the object enumerator.
 *
 * @return None.
 */
void qapi_Crypto_Persistent_Obj_Enumerator_Free(
		qapi_Crypto_Enumerator_Hdl_t hdl);

/*
 * @brief Must be called
 * before enumerating objects using qapi_Crypto_Persistent_Obj_Enumerator_Get_Next().
 *
 * @param[in] hdl   Handle to the object enumerator.
 * @param[in] storage_Id   Storage ID.
 * Must be QAPI_CRYPTO_PERSISTENT_OBJ_DATA_TEE_STORAGE_PRIVATE.
 *
 * @return QAPI_OK on success or another error code on failure.
 */
qapi_Status_t qapi_Crypto_Persistent_Obj_Enumerator_Start(
		qapi_Crypto_Enumerator_Hdl_t hdl,
		uint32_t storage_Id);
/*
 * @brief Gets the object
 * information of the next object.
 *
 * @details Changes on the file system (e.g., new objects or deleted
 * objects) cause opened enumerators to become invalid. The behavior is
 * undefined in this case.
 *
 * @param[in] hdl   Handle to the object enumerator.
 * @param[out] object_Info Object infomation for the next object.
 * @param[out] object_Id Object identifier of the next object. The size of the buffer must be QAPI_CRYPTO_PERSISTENT_OBJECT_ID_MAX_LEN.
 * @param[out] object_Id_Len Object identifier length.
 *
 * @return QAPI_OK on success or another error code on failure. \n
 * QAPI_ERR_NO_ENTRY is returned when there are no more objects
 * to enumerate.
*/
qapi_Status_t qapi_Crypto_Persistent_Obj_Enumerator_Get_Next(
		qapi_Crypto_Enumerator_Hdl_t hdl,
		qapi_Crypto_Obj_Info_t *object_Info,
		uint8_t *object_Id,
		uint32_t *object_Id_Len);

/**
 * @brief Resets an object
 * enumerator to its original state.
 *
 * @param[in] hdl   Handle to the object enumerator.
 *
 * @return QAPI_OK on success or another error code on failure.
 */
qapi_Status_t qapi_Crypto_Persistent_Obj_Enumerator_Reset(
		qapi_Crypto_Enumerator_Hdl_t hdl);

/**
 * @brief Creates a transient
 * object from a PEM file.
 *
 * @param[in] obj_Type Object type.
 * Supported object types are: \n
 *    - QAPI_CRYPTO_OBJ_TYPE_ECDSA_PUBLIC_KEY_E
 *    - QAPI_CRYPTO_OBJ_TYPE_ECDSA_KEYPAIR_E
 *    - QAPI_CRYPTO_OBJ_TYPE_ECDH_KEYPAIR_E
 *    - QAPI_CRYPTO_OBJ_TYPE_ECDH_PUBLIC_KEY_E
 *    - QAPI_CRYPTO_OBJ_TYPE_RSA_PUBLIC_KEY_E
 *    - QAPI_CRYPTO_OBJ_TYPE_RSA_KEYPAIR_E
 * @param[in] pem String containing contents of the PEM.
 * @param[in] pass_Phrase Optional PEM pass phrase. Can be NULL if the PEM file
 *            is not password protected.
 * @param[out] hdl   Handle to the created object.
 *
 * @return QAPI_OK on success or another error code on failure.
 */
int qapi_Crypto_Transient_Obj_Import_From_Pem(uint32_t obj_Type,
		const char *pem,
		const char* pass_Phrase,
		qapi_Crypto_Obj_Hdl_t *hdl);

/**
 * @brief Get the size of the buffer needed to hold the PEM
 * representation of the public key.
 *
 * @details  Use in conjunction with
 * qapi_Crypto_Obj_Export_Public_Key_To_Pem
 *
 * @param[in] obj_hdl   Handle to the object containing the public key.
 * @param[out] pem_Buffer_Size Set to the PEM output size.
 *
 * @return QAPI_OK on success or another error code on failure.
 */
qapi_Status_t qapi_Crypto_Obj_Get_Public_Key_Pem_Size(qapi_Crypto_Obj_Hdl_t obj_hdl, size_t *pem_Buffer_Size);

/**
 * @brief Export object's public key to a PEM file.
 *
 * @param[in] obj_hdl Handle to the object containing the public key.
 * @param[in/out] pem_Buffer String set to the contents of the PEM output.
 * @param[in] pem_Buffer_Size Size of pem_Buffer.
 *
 * @return QAPI_OK on success or another error code on failure.
 */
qapi_Status_t qapi_Crypto_Obj_Export_Public_Key_To_Pem(
		qapi_Crypto_Obj_Hdl_t obj_hdl,
		char *pem_Buffer,
		size_t pem_Buffer_Size);

/** @} */

#endif
