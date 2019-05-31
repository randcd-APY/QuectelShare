/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef _QAPI_MP3_DECODER_H_
#define _QAPI_MP3_DECODER_H_

#include <stdint.h>
#include "qapi/qapi_status.h"    /* qapi_Status_t */

#ifdef __cplusplus
extern "C" {
#endif

/** @file qapi_mp3_decoder.h
*
*/

/** @addtogroup qapi_mp3_decoder
@{ */

/**
 * @brief MP3 frame header
 *
 * @details Please refer to http://www.mp3-tech.org/programmer/frame_header.html.
 */
typedef struct
{
    uint32_t frame_sync : 11;

    uint32_t version : 2;
    /**< MPEG Audio version id. */

    uint32_t layer : 2;
    /**< Layer description. */

    uint32_t error_protection : 1;
    /**< Protection bit. */

    uint32_t bitrate : 4;
    /**< Bitrate index. */

    uint32_t sampling_frequency : 2;
    /**< Sampling frequency index. */

    uint32_t padding : 1;

    uint32_t extension : 1;
    /**< Private bit. */

    uint32_t channel_mode : 2;
    /**< Channel mode. */

    uint32_t mode_extension : 2;
    /**< Mode extension (only used when channel_mode = 1. */

    uint32_t copyright : 1;

    uint32_t original : 1;

    uint32_t emphasis : 2;
} qapi_Mp3_Frame_Header_t;

/**
 * @brief MP3 decoder output callback.
 * @return On success, should return 0.
 *         On error, should return -1.
 */
typedef int32_t (* qapi_MP3_Decoder_Output_CB_t)(
    void *param,
    /**< Same param as in qapi_MP3_Decoder_Run() */ 

    qapi_Mp3_Frame_Header_t header,
    /**< Frame header info. */

    uint32_t framenum,
    /**< Frame number. It starts from 1. */

    uint32_t nsamples,
    /**< Number of PCM samples per channel for this frame. */

    int16_t *left,
    /**< Pointer to the start of 16-bit PCM samples for left channel. */

    int16_t *right
    /**< Pointer to the start of 16-bit PCM samples for right channel. NULL if this is mono. */
    );

/**
 * @brief Reason code for inputting more data.
 */
typedef enum {
    QAPI_MP3_DECODER_START_NEW_FRAME_E,
    /**< Start decoding a new frame. */

    QAPI_MP3_DECODER_INVALID_SYNC_WORD_E,
    /**< The entire input buffer does not contain a valid sync word. */

    QAPI_MP3_DECODER_INVALID_HEADER_INFO_E,
    /**< Invalid header info, e.g. bitrate index = 15, etc. */

    QAPI_MP3_DECODER_INVALID_SIDE_INFO_E,
    /**< Invalid side info. */
} qapi_Mp3_Decoder_Input_Reason_t;

/**
 * @brief MP3 decoder input callback.
 * @return On success, should return number of bytes written to the decoder.
 *         On error, should return -1.
 */
typedef int32_t (* qapi_MP3_Decoder_Input_CB_t)(
    void *param,
    /**< Same param as in qapi_MP3_Decoder_Run(). */ 

    uint32_t maxbytes,
    /**< Max number of bytes the decoder can accept. */

    qapi_Mp3_Decoder_Input_Reason_t reason
    /**< Reason code. */
    );

/**
 * @brief Initialize MP3 decoder.
 *
 * @return  On success, a decoder handle is returned; on error, -1 is returned.
 *
 */
int32_t qapi_MP3_Decoder_Open(void);

/**
 * @brief Shutdown MP3 decoder.
 *
 * @param[in] handle    The decoder handle returned from qapi_MP3_Decoder_Open().
 *
 * @return  On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_MP3_Decoder_Close(int32_t handle);

/**
 * @brief   Copy MP3 data to decoder
 *
 * @param[in] handle        The decoder handle returned from qapi_MP3_Decoder_Open().
 * @param[in] buf           The address of buffer containing MP3 data.
 * @param[in] count         The maximum length (in bytes) of MP3 data to be copied.
 *
 * @return  On success, the number of bytes copied is returned; on error, -1 is returned.
 */
int32_t qapi_MP3_Decoder_Write_Data(int32_t handle, uint8_t *buf, uint32_t count);

/**
 * @brief   Decode MP3 data.
 *
 * @param[in] handle        The decoder handle returned from qapi_MP3_Decoder_Open().
 * @param[in] input         The callback which is invoked by the decoder to get MP3 data.
 * @param[in] output        The callback which is invoked by the decoder to return decoded samples for an MP3 frame.
 * @param[in] param         When the decoder invokes input/output, param is returned as the first argument \n
 *                          of input/output.
 *
 * @return  On success, 0 is returned; on error, -1 is returned.
 */
int32_t qapi_MP3_Decoder_Run(
                int32_t handle,
                qapi_MP3_Decoder_Input_CB_t input,
                qapi_MP3_Decoder_Output_CB_t output,
                void *param
                );

/** @} */ /* end_addtogroup */

#ifdef __cplusplus
}
#endif

#endif /* _QAPI_MP3_DECODER_H_ */
