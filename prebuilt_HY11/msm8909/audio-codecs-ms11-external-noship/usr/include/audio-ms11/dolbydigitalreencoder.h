/****************************************************************************

 This program is protected under international copyright laws as an
 unpublished work. Do not copy.

                    (C) Copyright Dolby International AB (2011)
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


*******************************************************************************/
/*! \defgroup ddre DDRE - Dolby Digital Reencoder


  This component offers an alternate API interface enabling re-encoding of up to 5.1 mixed
  main and associate PCM from a Dual Decoder Converter, with synchronized metadata. This component will author
  Dolby&reg; Digital style metadata and provide downmix/11dB shift clipping prevention. It will always produce
  a 2-channel downmix output and a Dolby Digital bitstream. Optional (switchable) is a 5.1-channel output
  (<b>virtualizer</b>). In case the input PCM data has a different sample rate than 48000
  (i.e. 32000 or 44100), the Dolby Digital Reencoder shall perform resampling.
  This Dolby Digital Reencoder interface shall be used in addition to encode 2-channel PCM at 48 kHz, coming from
  external sources like decoded Layer-2 broadcast or decoded MP3 files. This operation mode is
  referred to as <b>external PCM mode</b>.

  Typical Calling Sequence

  -# Call DolbyDigitalReencoderGetRequiredBufferSize() to determine the memory requirements
  -# Allocate memory based on the values returned
  -# Create a transcoder handle by calling DolbyDigitalReencoderOpen() with the
  pointers to the allocated memory
  -# Start the reencoding loop consisting of:
   -# Query the reencoder if  DolbyDigitalReencoderFifoEnoughSamples()
   -# Feed an additional frame of PCM data into the input buffer if necessary
   -# Certain processing options may be selected by DolbyDigitalReencoderSetProcessParam()
   -# Call DolbyDigitalReencoderProcess()
   -# Call DolbyDigitalReencoderGetDelay() to determine algorithmic delay to be taken into
      account e.g. for A/V synchronization
   -# Write the decoded time domain data and the generated Dolby Digital frame
      to the output file/device
*/

/*! \ingroup msd */
/*! \addtogroup ddre */
/*! \ingroup ddre */

/*!
  \file   dolbydigitalreencoder.h
  \brief  Dolby Digital reencoder with metadata protection - header file
*/

#ifndef DOLBY_DIGITAL_REENCODER_H
#define DOLBY_DIGITAL_REENCODER_H

#include "ddt_ddre_common.h"

/*!
  \brief Return codes of the Dolby Digital reencoder.

  \anchor DLB_DDRE_RET_VAL
*/
enum {
  DOLBY_DIGITAL_REENCODER_OK = 0,        /*!< No error. */
  DOLBY_DIGITAL_REENCODER_MDC_FAIL,      /*!< The metadata conversion/application has failed. */
  DOLBY_DIGITAL_REENCODER_DDENC_FAIL,    /*!< Failure within the DD encoder. */
  DOLBY_DIGITAL_REENCODER_FIFO_UNDERRUN, /*!< Not enough samples in FIFO */
  DOLBY_DIGITAL_REENCODER_COMPR_FAIL,    /*!< Failure within compressor */
  DOLBY_DIGITAL_REENCODER_INVALID_PARAM, /*!< An invalid parameter has been passed to one of the API functions. */
  DOLBY_DIGITAL_REENCODER_GENERAL_ERROR  /*!< Other unspecified error within the Dolby Digital reencoder. */
};


/*!
  \brief IDs for DolbyDigitalReencoderSetProcessParam()
*/
typedef enum {
  DOLBY_DIGITAL_REENCODER_CTRL_RFMODE_ID,
  /*!< This ID is used to pass a uint16_t value which turns on or off RF mode
    for dynamic rage control invoked for the downmix output. Possible values:\n
      0: DRC shall run in Line Mode (default)\n
      1: DRC shall run in RF mode\n
    The mode may be toggled whenever a change of the DRC type is desired.
    It becomes effective on the next call to DolbyDigitalReencoderProcess().
    The default mode after opening the instance is the Line mode.
  */
  DOLBY_DIGITAL_REENCODER_CTRL_DMXMODE_ID,
  /*!< This ID is used to pass a uint16_t value which controls the downmix
    operation.
    Possible values:\n
      0: Lt/Rt downmix type (default)\n
      1: Lo/Ro downmix type\n
    The mode may be toggled whenever a change of the downmix type is desired.
    It becomes effective on the next call to DolbyDigitalReencoderProcess().
    The default mode after opening the instance is the matrix compatible Lt/Rt
    downmix mode.
  */
  DOLBY_DIGITAL_REENCODER_CTRL_DRCSCALEHIGH_2CH_ID,
  /*!< This ID is used to pass a uint16_t value which scales the impact
    of DRC information for attenuation of high level signals (high level cut).
    The value is given in percent (0-100).
    A partial compression can be achieved between full attenuation (default)
    and no attenuation of loud passages of the audio signal.
    Note that the cut scale factor has no effect on a multichannel downmix
    in order to preserve the clipping protection.
  */
  DOLBY_DIGITAL_REENCODER_CTRL_DRCSCALELOW_2CH_ID,
  /*!< This ID is used to pass a uint16_t value which scales the impact
    of DRC information for amplification of low level signals (low level boost).
    A partial compression can be achieved between full DRC gains (default)
    and no amplification of quiet passages of the audio signal.
  */
  DOLBY_DIGITAL_REENCODER_CTRL_DRCSCALEHIGH_6CH_ID,
  /*!< This ID is used to pass a uint16_t value which scales the impact
    of DRC information for attenuation of high level signals (high level cut).
    The value is given in percent (0-100).
    A partial compression can be achieved between full attenuation (default)
    and no attenuation of loud passages of the audio signal.
  */
  DOLBY_DIGITAL_REENCODER_CTRL_DRCSCALELOW_6CH_ID,
  /*!< This ID is used to pass a uint16_t value which scales the impact
    of DRC information for amplification of low level signals (low level boost).
    A partial compression can be achieved between full DRC gains (default)
    and no amplification of quiet passages of the audio signal.
  */

  DOLBY_DIGITAL_REENCODER_CTRL_MULTICHOUT_ID,
  /*!< This ID is used to enable the multichannel virtualizer PCM output
    to one of the values below:\n
      0: no multichannel output \n
      1: multichannel output
  */

  DOLBY_DIGITAL_REENCODER_CTRL_CMIXLEV_ID,
  /*!< This ID is used to pass a uint16_t value which indicates the center
    mix level for stereo downmix according to the MIXLEV defines below
  */

  DOLBY_DIGITAL_REENCODER_CTRL_SURMIXLEV_ID,
  /*!< This ID is used to pass a uint16_t value which indicates the surround
    mix level for stereo downmix according to the MIXLEV defines below
  */
  DOLBY_DIGITAL_REENCODER_CTRL_DUALMODE_ID,
  /*!< This ID is used to pass a uint16_t value which indicates the dual mono
    reproduction mode. The channels are reproduced according to the DUALMODE
    defines below
  */
} DOLBY_DIGITAL_REENCODER_CTRL_ID;

/*! Compression profile */
#define COMPPROF_NONE (0)       /*!< Compression profile 0: none                       */
#define COMPPROF_FILMSTD (1)    /*!< Compression profile 1: film standard compression  */
#define COMPPROF_FILMLIGHT (2)  /*!< Compression profile 2: film light compression     */
#define COMPPROF_MUSICSTD (3)   /*!< Compression profile 3: music standard compression */
#define COMPPROF_MUSICLIGHT (4) /*!< Compression profile 4: music light compression    */
#define COMPPROF_SPEECH (5)     /*!< Compression profile 5: speech compression         */


/*! mix levels */
#define  MIXLEV_M3  (0)    /*!< -3 dB     */
#define  MIXLEV_M45 (1)    /*!< -4.5 dB   */
#define  MIXLEV_M6  (2)    /*!< -6 dB     */
#define  MIXLEV_RES (3)    /*!< reserved; will be interpreted as -4.5  */

/*! dual mode */
#define DUALMODE_STEREO (0) /*!< both channels are played out  */
#define DUALMODE_LEFT   (1) /*!< only left channel is played out */
#define DUALMODE_RIGHT  (2) /*!< only right channel is played out */


/*!
  \brief Handle to one instance of the Dolby Digital reencoder.
*/
typedef struct DOLBY_DIGITAL_REENCODER_INSTANCE* HANDLE_DOLBY_DIGITAL_REENCODER;

/*!
  \brief Query memory requirements for Dolby Digital reencoder.

  This function should be called by the encapsulating application in
  order to determine the size of the contiguous memory blocks that need
  to be allocated to the current instance of the Dolby Digital reencoder.

  The memory is split into three separate blocks which may be allocated
  in different memory regions, such as internal or external memory.
  Their respective sizes are returned via pointer arguments:

  \li internStaticSize:
  Size of 'internal static' data in bytes. This memory block should be
  located in the processors internal RAM for performance reasons and
  must not be overwritten in other sections of the code (persistent).

  \li internDynamicSize:
  Size of 'internal dynamic' data in bytes. This memory block should be
  located in the processors internal RAM for performance reasons and
  could be reused in other sections of the code (non-persistent).

  \li externStaticSize:
  Size of 'external static' data in bytes. Like the 'internStatic' memory,
  this is a memory block in RAM which must not be overwritten in other
  sections of the code (persistent). However, it is less frequently used
  than the 'intern' memory blocks and will therefore lead to a smaller
  performance hit when located in slower external memory.

  Note that the Dolby Digital reencoder does not perform memory allocation
  on its own.
  Hence, no close() function is provided because there are no resources that
  the decoder library could free.

  \return DOLBY_DIGITAL_REENCODER_OK on success
*/
int32_t
DolbyDigitalReencoderGetRequiredBufferSize(
  uint32_t *pInternStaticSize,  /*!< Pointer to internal static memory */
  uint32_t *pInternDynamicSize, /*!< Pointer to internal dynamic memory */
  uint32_t *pExternStaticSize   /*!< Pointer to external static memory */
);

/*!
  \brief Initialize one Dolby Digital reencoder instance

  This function is used to initialize the Dolby Digital reencoder and to
  retrieve a handle which is required to call DolbyDigitalReencoderProcess().

  DolbyDigitalReencoderGetRequiredBufferSize() should be called prior to
  DolbyDigitalReencoderOpen() to determine the memory requirements of the
  Dolby Digital reencoder.
  Contiguous memory blocks matching (at least) the sizes reported by
  DolbyDigitalReencoderGetRequiredBufferSize shall be passed as pointers via
  internStatic, internDynamic and externStatic.

  \return A handle to the Dolby Digital reencoder instance.
  \return NULL in case of errors (i.e. invalid parameters).
*/
HANDLE_DOLBY_DIGITAL_REENCODER
DolbyDigitalReencoderOpen(
  uint32_t *pInternStatic,  /*!< Pointer to internal static memory */
  uint32_t *pInternDynamic, /*!< Pointer to internal dynamic memory */
  uint32_t *pExternStatic,  /*!< Pointer to external static memory */
  uint16_t  bExternalPcm,   /*!< Flag to signal 'external PCM mode' */
  uint16_t  bDv258,         /*!< Flag indicates that input has been DV-258 processed */
  uint32_t  sampleRate,     /*!< sample rate of incoming PCM */
  uint16_t  compProfile,    /*!< compression profile, only valid for external PCM */
  uint16_t  dialNorm        /*!< dialog level of incoming PCM */
);

/*!
  \brief Adjust metadata and generate DD + PCM output

  This function processes the input multichannel PCM signal and associated
  metadata and generates a DD encoded bit stream plus a stereo downmix of the
  PCM signal.
  The input metadata is adjusted to provide clipping protection for both the DD
  bit stream and the PCM downmix signal.

  \return \ref DLB_DDRE_RET_VAL "Dolby Digital reencoder return value"
*/
int32_t
DolbyDigitalReencoderProcess(
  HANDLE_DOLBY_DIGITAL_REENCODER hReenc,               /*!< IN:  Handle of the Dolby Digital reencoder instance */
                    DLB_SFRACT** ppInputPCM,          /*!< IN:  Array of pointers to non interleaved input samples */
                        int16_t inputFrameSize,      /*!< IN:  number of input samples per channel */
                       int16_t* pInputConsumed,      /*!< OUT: Samples/channel consumed */
       TRANSCODER_CHANNEL_CONFIG inputChannelConfig,   /*!< IN:  Describes the PCM input channel configuration */
            unsigned char *const pOutBitstream,        /*!< IN:  Pointer to the start of bitstream buffer */
                      uint16_t* pAc3BytesWritten,    /*!< OUT: Number of bytes in output buffer after processing */
                       int16_t* pOutputStereoPCM,    /*!< OUT: Stereo output PCM, interleaved */
                      uint16_t* pPcmSamplesWritten,  /*!< OUT: Number of output samples in PCM buffer */
                      uint16_t* pStereoDsurmod,      /*!< OUT: dsurmod of stereo PCM out */
                       int16_t* pVirtualPCM,         /*!< OUT: 6ch output PCM, interleaved */
      TRANSCODER_CHANNEL_CONFIG* pVirtualChannelConfig /*!< OUT: Describes the PCM 6ch output channel configuration */
);


/*!
  \brief Set configuration parameters for an already open instance of the Dolby Digital reencoder

  A number of configuration parameters as defined in #DOLBY_DIGITAL_REENCODER_CTRL_ID
  are available to control the behaviour of DolbyDigitalReencoderProcess().
  This function allows changing a specific parameter by supplying its identification
  number as well as the desired value. The new value is passed by reference to a
  data type as described in #DOLBY_DIGITAL_REENCODER_CTRL_ID.

  \return \ref DLB_DDRE_RET_VAL "Dolby Digital reencoder return value"
*/
int16_t
DolbyDigitalReencoderSetProcessParam(
        HANDLE_DOLBY_DIGITAL_REENCODER hReenc,   /*!< IN: Instance handle obtained by DolbyDigitalReencoderOpen() */
  const DOLBY_DIGITAL_REENCODER_CTRL_ID paramID, /*!< IN: ID of parameter to modify */
                          const void * pParamVal /*!< IN: New value of parameter to modify */
);

/*!
  \brief query status of internal fifo

  In case the fifo contains enough samples for a new outframe, 1 is returned. 0 else.

  \return True if there are enough samples available for a new output frame
*/

uint16_t
DolbyDigitalReencoderFifoEnoughSamples(
  HANDLE_DOLBY_DIGITAL_REENCODER hReenc  /*!< Handle of the Dolby Digital reencoder instance */
);



/*!
  \brief Query algorithmic delay for output data

  The incoming audio signal does not appear directly in the output frame
  generated by DolbyDigitalReencoderProcess().
  There are two kinds of algorithmic delay returned by this function:

  -# A constant latency depending on the current mode of operation returned
     via <i>pStaticDelay</i>.
     There are several sources for this kind of delay:
   \li When the sample rate converter is active,
       its states are considered in the returned value.
   \li There is an additional delay buffer for the stereo and 6 channel PCM
       output which is required for a correct timing of metadata application and for
       synchronization with the decoded Dolby Digital output.
  -# The input frame size maybe different from the Dolby Digital frame size.
     Since DolbyDigitalReencoderProcess() generates one Dolby Digital frame and corresponding
     stereo and 6 channel PCM data on each call, a reframing is performed internally
     using a PCM FIFO buffer. The current amount of valid data in the PCM FIFO
     that has not yet appeared at the output can change periodically,
     depending on the input frame size and sample rate. This value
     is returned via <i>pDynamicDelay</i>.

  Both delay values are measured in samples at the output sample rate of 48kHz
  The result refers to the previous call of DolbyDigitalReencoderProcess(), i.e.
  DolbyDigitalReencoderGetDelay() is called <b>after</b> DolbyDigitalReencoderProcess().

  The amount of audio data that is currently buffered inside the reencoder and
  pending for output is the sum of static and dynamic delay.

  \return \ref DLB_DDRE_RET_VAL "Dolby Digital reencoder return value"
*/
int16_t
DolbyDigitalReencoderGetDelay(HANDLE_DOLBY_DIGITAL_REENCODER hReenc, /*!< IN:  Instance handle obtained by DolbyDigitalReencoderOpen() */
                              int16_t    *pStaticDelay,             /*!< OUT: Constant latency depending on the current mode of operation */
                              int16_t    *pDynamicDelay             /*!< OUT: Dynamically changing amount of decoded audio data in the internal reframing buffer */
                              );


#endif

