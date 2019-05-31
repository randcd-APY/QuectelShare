/*
* Copyright (c) 2018 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_attestation.h
 *
 * @addtogroup qapi_crypto_attestation
 * @{
 *
 * @brief QAPI to generate an attestation token.
 *
 * @}
 */

#ifndef _QAPI_ATTESTATION_H_
#define _QAPI_ATTESTATION_H_
/** @cond */
#include "stdint.h"
/** @endcond */
#include "qapi/qapi_status.h"

/** @addtogroup qapi_crypto_attestation
@{ */


/** Size of the input buffer to the qapi_Crypto_Attestation_Generate_Token() function. */
#define QAPI_CRYPTO_ATTESTATION_INPUT_BUFFER_SIZE   16
/** Size of the output buffer to the qapi_Crypto_Attestation_Generate_Token() function. */
#define QAPI_CRYPTO_ATTESTATION_OUTPUT_BUFFER_SIZE  16


/**
 * @brief Generates an attestation token.
 *
 * @param[in]  user_Input      Input buffer for which to generate attestation.
 * @param[out] output          Output buffer to which to write the attestation token.
 *
 * @return QAPI_OK on success or an error code on failure.
 */
qapi_Status_t
qapi_Crypto_Attestation_Generate_Token(
    const uint8_t user_Input[QAPI_CRYPTO_ATTESTATION_INPUT_BUFFER_SIZE],
    uint8_t output[QAPI_CRYPTO_ATTESTATION_OUTPUT_BUFFER_SIZE]
    );


/** @} */

#endif /* _QAPI_ATTESTATION_H_ */
