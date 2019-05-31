/****************************************************************************

 This program is protected under international copyright laws as an
 unpublished work. Do not copy.

                    (C) Copyright Dolby International AB (%YEAR%)
                             All Rights Reserved

 This software is company confidential information and the property of
 Dolby International AB, and can not be reproduced or disclosed in any form
 without written authorization of Dolby International AB.

 Those intending to use this software module for other purposes are advised
 that this infringe existing or pending patents. Dolby International AB has no
 liability for use of this software module or derivatives thereof in any
 implementation. Copyright is not released for any means. Dolby International AB
 retains full right to use the code for its own purpose, assign or sell the
 code to a third party and to inhibit any user or third party from using the
 code. This copyright notice must be included in all copies or derivative
 works.


****************************************************************************/
/*! \defgroup DLB_DDENCLIB Fixed Point Dolby Digital Encoder Library

\par  Introduction

 This package contains an encoder for the Dolby Digital format. It is based
 on the Dolby DDCE encoder package version 3.2.0, with the following
 differences:

 -# Support for DRC metadata (dialnorm, compr, dynrng) inputs
 -# Support for new acmod/datarate combinations: 640kbps 1/0, 640kbps 2/0, 640kbps 1+1
 -# Restriction on input datarates so that only 640kbps is allowed
 -# No support for AGC and LFE lowpass filters
 -# No support for coupling

#ifdef SRC_DIST
 The source code is based on the \ref dlb_intrinsics_lib,
 a generic set of arithmetic operators that enables easy porting to any 16/32 bit
 fixed point processor.
#endif

 \par Calling sequence for the encoder

 This section illustrates how to use the encoder interface.
 Your applications main source file shall include ddenc.h and dlb_types.h.

 -# Call DDEncGetRequiredBufferSize() to determine the memory
 size required by the encoder.
 -# Allocate memory based on the values returned.
 -# Call DDEncOpen()
 -# loop over input data
  -# provide #NBLOCKS*#N input samples
  -# Call DDEncEncode() to encoder one frame of #NBLOCKS*#N input samples
  -# Save output of max. #NOUTWORDS words
 -# DDEncConfigure() can optionally be used to reconfigure an already opened encoder for seamless switching

#ifdef DOLBY_PULSE_DECODER_TRANSCODER
 \par Dolby Digital Encoder unit test

 While the Dolby Digital Encoder is a part of the Dolby Pulse Decoder-Transcoder,
 it can also be run separately using a different main program.
 The Unix Makefile of the Dolby Pulse Decoder-Transcoder contains a dedicated target
 for that purpose called <i>DolbyPulseDecoderTranscoder_ddenc_main</i>.
 There is also a separate MSVC workspace available in the dlb_ddenclib/test subdirectory.
 The respective main program provides a command line interface for the test mode
 to the encoder verifier (see usage message of the program itself for details).

#endif
#ifdef INCLUDE_DDENC_TEST_MODE
 \par Test mode for the encoder

 This section illustrates how to use the encoder test mode interface.
 Your applications main source file shall include ddenc.h and dlb_types.h.

 -# Call DDEncGetRequiredBufferSize() to determine the memory
 size required by the encoder.
 -# Allocate memory based on the values returned.
 -# Call DDEncOpen()
 -# loop over test mode data
  -# provide test package data block without sync word 0xAC3C
  -# Call DDEncTest to process test data block
  -# Save output of max #N words
#endif
*/

/*! \ingroup pulse */
/*! \ingroup ddre */
/*! \addtogroup DLB_DDENCLIB */

/*@{*/

/*!
  \file   ddenc.h
  \brief  Dolby Digital Encoder - header file
*/

#ifndef _ddenc_h_
#define _ddenc_h_

#ifdef __cplusplus
extern "C"
{
#endif

/**** General system equates ****/
#define NBLOCKS     6       /* # of time blocks per frame   */
#define NCHANS      6       /* max # of discrete channels   */
#define N         256       /* # of samples per time block    */
#ifdef DDCE_V3_2_1
#define LFELP
#endif
/* Allow LFE filter code in Exec  */
#define NOUTWORDS   1280    /* max # words per frame @ 48kHz  */



/*
  error codes
*/

enum
{
  DDENC_OK =0,
  DDENC_ERROR_GENERAL      /* unspecified error */
};

/*-------------------- structure definitions ------------------------------*/
struct DD_ENCODER;

typedef struct DD_ENCODER *DDENC_HANDLE;
typedef  struct {
  int debug_arg;   /*!< debug argument */
  int sampratecod; /*!< sample rate 0 = 48 kHz, 1 = 44.1 kHz, 2 = 32 kHz, */
#if defined(DDCE_V3_2_1)
  int compchar;    /*!< compression characteristic */
  int compchar2;   /*!< compression characteristic for 1+1 */
#endif
  int lofreqeffon; /*!< LFE on/off */
  int audcodmod;   /*!< Encoder channel mode, default: 3/2 mode , 0 = 1+1 (L, R)
                                                                  1 = 1/0 (C)
                                                                  2 = 2/0 (L, R)
                                                                  7 = 3/2 (L, C, R, l, r) */
  int datarate;    /*!< Encoder data rate, default 448 kbps, 4 = 64 kbps 10 = 192 kbps 15 = 448 kbps
                                                             6 = 96 kbps 12 = 256 kbps 18 = 640 kbps
                                                             8 = 128 kbps 14 = 384 kbps */
#if defined(DDCE_V3_2_1)
  int lfefiltinuse;/* LFE low pass filter on/off */
#endif
} DDENC_CONFIG;

typedef struct {
  unsigned char version;       /*!< Version number */
  unsigned char majorRevision; /*!< Major revision number */
  unsigned char minorRevision; /*!< Minor revision number */
  char buildDate[21];          /*!< Build date */
} DDENC_VERSIONINFO;

/*!
  \brief Metadata Input interface

  \note the combination of compre==1 and compr ==0 is not allowed
  and may lead to bitstreams that do not obey an intended clipping protection.
*/
typedef struct {
  int16_t cmixlev;
  int16_t surmixlev;
  int16_t dsurmod;
  int16_t comprflag;
  int16_t compr[2];
  int16_t dynrngflag;
  int16_t dynrng[2][NBLOCKS];
  int16_t dialnorm[2];
} DDENC_META_DATA;

/*!
  \brief Get version number of encoder library

  This function returns the version information of the library into a
  user-supplied structure.

  \return DDENC_OK if success
*/
int32_t DDEncGetVersionInfo( DDENC_VERSIONINFO *encapiVersion /*!< Version info handle */
                          );

/*!
  \brief Calculate required buffer size for encoder instance

  This function should be called by the encapsulating application in
  order to determine the sizes of the contiguous memory blocks that needs
  to be allocated to the current instance of the encoder.

  The encoder might require memory blocks in multiple memory regions
  such as internal or external memory.

  The encoder does not perform any internal dynamic memory allocation.

  \return DDENC_OK if success
*/
int32_t DDEncGetRequiredBufferSize( uint32_t numChannels,       /*!< Number of channels */
                                     uint32_t *internStaticSize, /*!< OUT: Size of internal static memory in Byte  */
                                     uint32_t *internDynamicSize,/*!< OUT: Size of internal dynamic memory in Byte */
                                     uint32_t *externStaticSize  /*!< OUT: Size of external static memory in Byte */
                                     );

/*!
  \brief Gives a reasonable default configuration for the
         encoder open function

  \return
*/
void DDInitDefaultConfig(DDENC_CONFIG *config /*!< pointer to  config struct */
                         );

/*!
  \brief Allocate and initialize a new encoder instance

  \return DDENC_OK if success
*/

int32_t  DDEncOpen
(  struct DD_ENCODER**     phDDEnc,        /*!< Pointer to an encoder handle, initialized on return */
   const  DDENC_CONFIG     config,         /*!< Pre-initialized config struct */
   uint32_t               *internStatic,  /*!< Pointer to static internal memory */
   uint32_t               *internDynamic, /*!< Pointer to dynamic internal memory */
   uint32_t               *externStatic   /*!< Pointer to static external memory */
);


/*!
  \brief Configure encoder instance with pre-initialized config struct

  \return DDENC_OK if success
*/

int32_t  DDEncConfigure
(  struct DD_ENCODER*      hDDEnc,        /*!< Pointer to an encoder handle */
   const  DDENC_CONFIG     config         /*!< Pre-initialized config struct */
);

/*!
  \brief  Encodes one frame

  \return DDENC_OK if success
*/

int32_t DDEncEncode(struct DD_ENCODER  *hDDEnc,               /*!< Pointer to an encoder handle */
                     DLB_SFRACT** pSamples,                    /*!< NBLOCKS*N*nChannels audio samples, non interleaved */
#ifdef READBUF_SHORT
                     uint16_t     *outBytes,                  /*!< Pointer to the start of the bitstream buffer */
#else
                     unsigned char *outBytes,                  /*!< Pointer to the start of the bitstream buffer */
#endif
                     uint16_t           maxBytesToWrite,      /*!< Maximum number of bytes the encoder should write in this frame */
                     uint32_t           offset,               /*!< Bit offset into the bitstream at which to start writing */
                     uint16_t           *numOutBytes,         /*!< Number of bytes in the output buffer */
                     DDENC_META_DATA     *metaData             /*!< Points to metadata to be encoded in the current frame, NULL if not available */
                     );

#ifdef INCLUDE_DDENC_TEST_MODE
/*!
  \brief  Performs Encoder test on one test package data block,
          test package without sync word 0xAC3C

  \return DDENC_OK if success
*/

int32_t DDEncTest(struct DD_ENCODER  *hDDEnc,      /*!< Pointer to an encoder handle */
                     DLB_SFRACT** pSamples,                    /*!< NBLOCKS*N*nChannels audio samples, non interleaved */
#ifdef READBUF_SHORT
                     uint16_t     *outBytes,                  /*!< Pointer to the start of the bitstream buffer */
#else
                     unsigned char *outBytes,                  /*!< Pointer to the start of the bitstream buffer */
#endif
                    uint16_t           *numOutBytes,          /*!< Number of bytes in the output buffer after processing */

                   uint16_t*         pPKI,         /*!< Pointer to test package input data, max size N      */
                   uint16_t*         pPKO          /*!< Pointer to test package output data max size N      */
                    );
#endif

/*!
  \brief  Deallocate an encoder instance
  \return
*/

void DDEncClose (struct DD_ENCODER* hDDEnc   /*!< Pointer to an encoder handle */
                 );


#ifdef __cplusplus
}
#endif


#endif /* _ddenc_h_ */
/*@}*/
