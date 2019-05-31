/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

 /**
 *
 * @file qapi_ed25519.h
 *
 * @brief ed25519 Keypair Ggeneration and Message Signing
 *
 * @addtogroup qapi_ed25519
 * @{
 *
  * @details The ED25519 module provides secure generation and storage of the ED25519 keypair,
 *          as well as message signing functionality using the generated ED25519 keypair.
 *
 * @code {.c}
 *
 *     * The code snippet below demonstrates the usage of qapi_ed25519 APIs.
 *     * In the example below, we generate an ED25519 keypair, sign a message
 *     * with it, and delete the keypair when it is no longer needed.
 *
 *     const uint32_t key_id = 0x1234; // unique key ID
 *     const size_t minimum_public_key_buffer_size = 32;
 *     uint8_t public_key_buffer[minimum_public_key_buffer_size];
 *     size_t public_key_size = minimum_public_key_buffer_size;
 *
 *     // generate ed25519 keypair and store it under ID key_id
 *     qapi_Status_t status = qapi_Ed25519_Generate_Key_Pair(key_id, public_key_buffer, &public_key_size);
 *     // at this point, the public_key_buffer (public key of the generated keypair)
 *     // is sent to a peer that needs to verify the message
 *
 *
 *     uint8_t message[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
 *     // message that needs to be signed using ed25519 algorithm
 *     const size_t minimum_signature_buffer_size = 64;
 *     uint8_t signature[minimum_signature_buffer_size];
 *     size_t signature_size = minimum_signature_buffer_size;
 *     // sign message using previously generated keypair
 *     status = qapi_Ed25519_Sign(key_id, message, sizeof(message), signature, &signature_size);
 *
 *
 *     // delete the keypair when it is no longer needed
 *     status = qapi_Ed25519_Reset_Key(key_id);
 *
 * @endcode
 *
 * @}
 *
 */

#ifndef __QAPI_HOMEKIT__H__
#define __QAPI_HOMEKIT__H__

#include <stdint.h>
#include <stddef.h>


/** @addtogroup qapi_ed25519
@{ */


/**
 *  @brief Generates an ED25519 keypair, returning the public key to the caller.
 *
 *  @details This function generates an ED25519 keypair and stores it using TEE
 *           under a specified ID. The corresponding public key of the keypair
 *           is returned to the caller.
 *
 *  @param[in]      id                  ID under which to save the generated keypair.
 *
 *  @param[out]     public_Key          Public key corresponding to the generated keypair.
 *
 *  @param[in,out]  public_Key_Size_Ptr On input, this parameter must point to a size_t type variable indicating
 *                                      the public_Key buffer size. On successful return from this function,
 *                                      the *public_Key_Size_Ptr will contain the real size of the public key.
 *                                      Note that for this function to succeed, the public_Key must
 *                                      be at least 32 bytes, and as a consequence, the *public_Key_Size_Ptr must
 *                                      be set to at least 32 bytes as well.
 *
  *  @return QAPI_OK on success or another error code on error.
 *
 */
int qapi_Ed25519_Generate_Key_Pair(
    uint32_t id,
    uint8_t * public_Key,
    size_t * public_Key_Size_Ptr
    );


/**
 *  @brief Signs a message with the specified previously-generated ED25519 keypair.
 *
 *  @param[in]    id                    ID of the keypair to be used to sign the message.
 *
 *  @param[in]    message               Message to sign.
 *
 *  @param[in]    message_Size          Size of the message to sign.
 *
 *  @param[out]   signature             Buffer where to store the signature.
 *
 *  @param[in,out] signature_Size_Ptr    On input, this must point to a size_t type variable indicating the size of
 *                                      the signature buffer. On successful return from this function, the *signature_Size_Ptr
 *                                      will contain the real size of the generated signature. Note that for this
 *                                      function to succeed, the signature buffer must be at least 64 bytes, and as a consequence,
 *                                      the *signature_Size_Ptr must be set to at least 64 bytes as well.
  *
 *  @return QAPI_OK on success or another error code on error.
 *
 */
int qapi_Ed25519_Sign(
    uint32_t id,
    const uint8_t * message,
    size_t message_Size,
    uint8_t * signature,
    size_t * signature_Size_Ptr
    );


/**
 *  @brief Deletes a previously generated ED25519 keypair from TEE.
 *
 *  @param[in]    id                    ID of the previously generated ED25519 keypair to delete.
 *
 *  @return QAPI_OK on success or another error code on error.
 *
 */
int qapi_Ed25519_Reset_Key(uint32_t id);


/** @} */

#endif // __QAPI_HOMEKIT__H__
