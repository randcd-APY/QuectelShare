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


****************************************************************************/
/*!
  \file   dolbypulsedecodertranscoder.h
  \brief  Dolby Pulse Decoder-Transcoder interface $Revision: #2 $
*/

#ifndef DOLBYPULSEDECODERTRANSCODER_H
#define DOLBYPULSEDECODERTRANSCODER_H

#include "ddt_ddre_common.h"

/*

\section mainpageStructure Document Structure

This documentation describes the following two major components and their sub-components
\li The \subpage ddtDocuPage which is capable of decoding Dolby&reg; Pulse bitstreams
and transcode them to Dolby&reg; Digital.
\li The \subpage ddreDocuPage which is capable of reencoding PCM signals coming from
either a Dolby&reg; Digital Decoder or an external audio decoder


Browse through this documentation by using the navigation pane on the
left. A good starting point is "Modules", where the documentation of
sub-components (e.g. Dolby&reg; Pulse Decoder) can be found.
Additionally all software modules have their own documentation
e.g. "Fixed Point Dolby Pulse Decoder", "Dolby Intrinsics Library" etc.
"Data structures", "Data fields", "File List" and "Globals" give an overview of the
structs/unions, files and variables used in the project. "Data
structures" shows a brief description of the data structures. "Data
fields" shows a list of all struct and union fields with links to the
structures/unions they belong to. "File List" gives a brief description
of all files. "Globals" shows a list of all functions, variables,
defines, enums, and typedefs with links to the files they belong to.

*/

/*! \defgroup pulse DDT - Dolby Pulse Decoder-Transcoder

\section sectionSoftwareStructure Software structure

This component contains an optimized
fixedpoint reference source code written in ANSI C, implementing a
Dolby Pulse Decoder-Transcoder.
 The source code is based on the \ref dlb_intrinsics_lib,
 a generic set of arithmetic operators that enables easy porting to any 16/32 bit
 fixed point processor.

The primary use case of this software is to be built into set top boxes
or integrated in digital TVs with the purpose of offering a link (S/PDIF)
to a Dolby Digital 5.1 capable A/V receiver. At the same time, a two-channel downmix
for analog (and digital) output is provided.
Additionally, the decoded multichannel audio is available to a virtualizer.
When the incoming bitstream contains audio metadata (dynamic range control,
program reference level, downmix coefficients), the metadata is converted to corresponding
Dolby Digital metadata. The audio metadata is applied on the downmix output and
embedded in the outgoing Dolby Digital bitstream. When no audio metadata is present,
dynamic range coefficients are generated that provide clipping prevention
when downmixing.

\image html ddt-single.png "Block Diagram for the Dolby Pulse Decoder-Transcoder in single-instance operation"

While a single instance of the Dolby Pulse Decoder-Transcoder can decode only
one Dolby Pulse bitstream, an application may allocate two Dolby Pulse Decoder-Transcoder
instances to deploy the full functionality of the Dual Decoder-Transcoder (DDT).
This allows parallel decoding of a main audio stream and an associated audio stream.
Using DolbyPulseDecoderTranscoderOpen(), the second instance shall be configured to run
without the resampler and Dolby Digital encoding. Additionally the associate stream decoding
instance can be restricted to two channels in order to reduce memory and computational demands.
Please see the supplied example application (main.c) for details.

As illustrated in the below figures, an external mixer component which is not part
of the Dual Decoder-Transcoder shall be used to combine the decoded main audio signal
and the decoded associated audio signal. The external mixer is invoked by the main
DDT instance via a callback function (see DolbyPulseDecoderTranscoderInstallMixerCallback()).
During mixing operation, dynamic range control coefficients are recalculated for the
mixed audio signal by the DRCC block shown in the block diagram. The mixed audio signal
is available at the PCM outputs (multichannel and stereo downmix) as well as encoded
to Dolby Digital.
\image html ddt-main.png "Block Diagram for the main DDT instance in mixing operation"
\image html ddt-aa.png "Block Diagram for the associated DDT instance in mixing operation"


\section inputFormats Input Formats

The Dolby Pulse Decoder-Transcoder decodes various types of AAC content to PCM,
and simultaneously transcodes this content to Dolby Digital for multichannel bitstream output.
AAC formats supported include:
\li Dolby Pulse
\li aacPlus
\li MPEG-2/4 AAC
\li MPEG-4 HE AAC v1
\li MPEG-4 HE AAC v2

Dolby Pulse is an optimized implementation of the MPEG-4 AAC family of audio codecs.
Dolby Pulse is designed by Dolby to bring additional functionality, flexibility,
and reliability to the format while remaining fully backward and forward compatible
with MPEG-4 AAC, MPEG-4 HE AAC v1 and MPEG-4 HE AAC v2 as defined in ISO/IEC 14496-3:2005.

The input buffer shall contain elementary stream access units encapsulated
in the LATM multiplex format and LOAS transmission format as defined by DVB.
Alternatively, the buffer may contain an ADTS stream as defined by ARIB.
For other transport formats, it is suggested to extract single AAC access units
and use the file playback feature of the Dual Decoder-Transcoder in push mode.
Each access unit shall contain exactly one frame of audio (i.e. 1024 samples at
the AAC sample rate).

Supported input sample rates are 32 kHz, 44.1 kHz
and 48 kHz. Other sample rates between 7.35 kHz and 96 kHz can also be
processed with some restrictions:
\li No Dolby Digital output is generated for AAC sample rates below 16 kHz.
    The downmix clipping protection is not tuned for these sample rates and
    might result in slow gain control.
\li If the AAC or HE-AAC sample rate is higher than 48 kHz, downsampling by
    a factor of 2 is applied.

For performance reasons, the Dolby Pulse decoder is limited to 6 fully decoded channels.
7.1 bitstreams can still be decoded, but only a 5.1 subset is passed on by dropping the
side surround channels.


\section ddOut Dolby Digital Output Bitstream

Provided no input buffer underrun occurs, every call to DolbyPulseDecoderTranscoderProcess()
generates exactly one frame of a Dolby Digital bitstream covering 32 ms at a sample rate
of 48 kHz.
Like the input data, the bit stream is stored in an array of bytes.
The bit rate is 640 kbps so that 2560 bytes are written on every call.
In case of a 32 kHz
or 44.1 kHz
input stream, a sample rate converter to 48 kHz is involved that requires an
audio bandwidth limitation to 14 kHz
resp. 16 kHz
for aliasing prevention.


\section transcoderCallingSequence Calling Sequence for the transcoder

The transcoder can be operated in two modes, denoted 'pull mode' and 'push mode'.
This mode is selected on DolbyPulseDecoderTranscoderOpen() via the
'bAllowPartialFeed' flag (0: 'pull'; 1: 'push'). <br>

\subsection pullmode Pull Mode

In 'pull mode', every call to DolbyPulseDecoderTranscoderProcess() will result in one
frame of output data. Dependent on the fill level of the internal FiFo module, this
means that either 0, 1, or 2 access units are consumed from the input. In turn, the
surrounding application has to cater for availability of at least 2 access units in
the input buffer (pInBitstream) for every DolbyPulseDecoderTranscoderProcess() call. In case not enough data is
available at the input, the transcoder will conceal, i.e. insert silence as a replacement
for the missing input data.

The basic calling sequence for running in 'pull mode' is outlined below:

 -# Call DolbyPulseDecoderTranscoderGetRequiredBufferSize() to determine the memory requirements
 -# Allocate memory based on the values returned
 -# Create a transcoder handle by calling DolbyPulseDecoderTranscoderOpen() with the
    pointers to the allocated memory
 -# Start the transcoding loop consisting of:
  -# Fill buffer for input bitstream
  -# Certain processing options may be selected by DolbyPulseDecoderTranscoderSetProcessParam()
  -# Generate one output frame by calling DolbyPulseDecoderTranscoderProcess()
  -# Query stream properties by calling DolbyPulseDecoderTranscoderGetStreamInfo().
  -# Call DolbyPulseDecoderTranscoderGetDelay() to determine algorithmic delay to be taken into
     account e.g. for A/V synchronization
  -# Write the decoded time domain data and the generated Dolby Digital frame
     to the output file/device

The usage of the functions provided by the dolbypulsedecodertranscoder.h API is demonstrated in the
supplied example application (main.c).

\subsection pushmode Push Mode

Different than in \ref pullmode "pull mode", not every call to DolbyPulseDecoderTranscoderProcess()
necessarily leads to valid output data. Instead, if not enough data was found at the input,
DolbyPulseDecoderTranscoderProcess() will return with the status set to DOLBY_PULSE_DECODER_TRANSCODER_UNDERRUN.
The amount of consumed data is indicated by numBytesRead; in consequence the data not taken
by this call has to be re-fed in a subsequent next call, concatenated with additional input data. <br>
A successfully transcoded output frame is indicated by the returned transcoder status and by a pOutLen value > 0.

There is a possible error condition where DolbyPulseDecoderTranscoderProcess()
returns with DOLBY_PULSE_DECODER_TRANSCODER_UNDERRUN without having read any
data, but the provided input buffer was already of maximum size. This can
happen when the buffer starts with bytes resembling a LOAS or ADTS header, and
a size field larger than the buffer size the caller is providing.

In this case, to prevent an endless loop, discard some data (one byte suffices)
and call DolbyPulseDecoderTranscoderProcess() again.

The basic calling sequence for running in 'push mode' is slightly different
from \ref pullmode "pull mode":

 -# Call DolbyPulseDecoderTranscoderGetRequiredBufferSize() to determine the memory requirements
 -# Allocate memory based on the values returned
 -# Create a transcoder handle by calling DolbyPulseDecoderTranscoderOpen() with the
    pointers to the allocated memory
 -# Start the transcoding loop consisting of:
  -# Repeat the following until one output frame is generated:
   -# Fill additional data into bitstream input buffer
   -# Certain processing options may be selected by DolbyPulseDecoderTranscoderSetProcessParam()
   -# Call DolbyPulseDecoderTranscoderProcess()
  -# Query stream properties by calling DolbyPulseDecoderTranscoderGetStreamInfo().
  -# Call DolbyPulseDecoderTranscoderGetDelay() to determine algorithmic delay to be taken into
     account e.g. for A/V synchronization
  -# Write the decoded time domain data and the generated Dolby Digital frame
     to the output file/device


\section Unittests Unit tests

Some parts of the source code may be tested separately using the frontend from one of the
<i>test</i> subdirectories. Unit tests are available for
\li \ref DLB_PULSEDEC
\li \ref DLB_DDENCLIB
\li \ref resamplerTest "Rational resampler"
\li \ref SBRDEC_QMF

*/

/*! \ingroup msd */
/*! \addtogroup pulse */
/*! \ingroup pulse */

      #define MS11


/*!
  Handle for one instance of the Dolby Pulse Decoder-Transcoder
*/
typedef struct TRANSCODER_INSTANCE * HANDLE_DOLBY_PULSE_DECODER_TRANSCODER;


/*!
  Transcoder stream info struct
*/
typedef struct TRANSCODER_STREAMINFO {
  uint16_t    sbrEnabled;   /*!< Flag indicating activity of Spectral Band Replication */
  uint16_t    psEnabled;    /*!< Flag indicating activity of Parametric Stereo processing */
  uint32_t    samplingRate; /*!< Sample rate of the PCM data generated by the previous call to DolbyPulseDecoderTranscoderProcess() */
  uint16_t    frameLength;  /*!< Number of samples generated by the previous call to DolbyPulseDecoderTranscoderProcess() */
  uint16_t    channels;     /*!< Number of decoded Dolby Pulse audio channels including Low Frequency Enhancement channels.
                               When the input bitstream contains more than 6 channels, only the number actually decoded channels is reported. */
  uint16_t    dolbyPulseID; /*!< Indicates that a Dolby Pulse bitstream has been detected */
} TRANSCODER_STREAMINFO;


/*!
  \brief Transcoder delay information provided by DolbyPulseDecoderTranscoderGetDelay()

  All delay values within this struct are relative to the presentation time
  that refers to the <b>end</b> of the most recently consumed AAC/HE-AAC access unit.
  Assuming a seamless stream, this is the equivalent to the expected presentation
  time associated with the <b>next</b> access unit.
  The reported delay tells the difference between that presentation time and the
  presentation time that refers to the end of the most recently generated output block,
  which is equivalent to the <b>beginning</b> of the output block generated next.

  There are two kinds of algorithmic delay represented in this struct:

  -# A delay denoted as <i>static</i> which is constant for a specific mode of operation.
     There are several sources for this kind of delay:
   \li When the SBR tool is used by the Dolby Pulse bitstream, it amounts
       to 961 samples of additonal algorithmic delay compared to plain AAC.
       This  delay is caused by QMF filter states and the time grid overlap buffer.
       Note that this delay is not considered by presentation time stamps
       generated by the encoder because the encoder side does not know
       whether an SBR-enabled decoder is used for playback or not.
   \li When the sample rate converter is active,
       its states are considered in the returned value.
   \li There is an additional delay buffer for the stereo output which
       is required for a correct timing of metadata application and for
       synchronization with the decoded Dolby Digital output.
   \li When enabled, the Dolby Volume 258 algorithm adds 256 samples to this delay value.
  -# A varying latency denoted as <i>dynamic</i> which adds on top of the constant
     delay. The varying latency is caused by the internal PCM FIFO buffer used
     for reframing the Dolby Pulse Decoder's output to the Dolby Digital input.
     The current amount of valid data in the PCM FIFO that has not yet appeared
     at the output can change periodically, depending on the input frame size
     and sample rate.

  The input-to-output delay applies to the stereo PCM output,
  the multichannel PCM output,
  and the decoded Dolby Digital output (assuming immediate decoding of the generated
  Dolby Digital frames).
  It is measured in samples at the transcoder's PCM output sample rate indicated by
  the parameter <i>pOutRate</i> set by DolbyPulseDecoderTranscoderProcess().

  The input-to-mixer delay applies to the PCM data fed into the mixer callback function.
  It is measured in samples at the Dolby Pulse decoder's output sample rate indicated by
  DolbyPulseDecoderTranscoderGetStreamInfo().
  Since there is only one source of <i>dynamic</i> delay, <i>dynamicInput2Output</i>
  and <i>dynamicInput2Mixer</i>
  mean the same time period, however given at potentially different sample rates.
*/
typedef struct TRANSCODER_DELAYINFO {
  int16_t    staticInput2Output;   /*!< Non-varying part of the latency between the input and the output of the transcoder,
                                       reported in samples at the output sample rate. */
  int16_t    dynamicInput2Output;  /*!< Varying part of the latency between the input and the output of the transcoder,
                                       reported in samples at the output sample rate. */
  int16_t    staticInput2Mixer;    /*!< Non-varying part of the latency between the input and the mixer feed of the transcoder,
                                       reported in samples at the sample rate the mixer operates at. */
  int16_t    dynamicInput2Mixer;   /*!< Varying part of the latency between the input and the mixer feed of the transcoder,
                                       reported in samples at the sample rate the mixer operates at. */
} TRANSCODER_DELAYINFO;


#define INPUT_BUFFER_LEN (7*2*6144/8)        /*!< Size of the Dolby Pulse input buffers in bytes.
                                               Up to 2 frames may be consumed by DolbyPulseDecoderTranscoderProcess(),
                                               therefore the input buffer must be large enough for 2 maximum sized
                                               access units. The buffer size used here is suitable for up to 7.1
                                               channels (the LFE is not counted according to MPEG).
                                               Even in push mode (bAllowPartialFeed=1), the buffer size should not be reduced.
                                               Otherwise, a partial frame may prevent the LOAS parser from demanding more
                                               input and result in muted frames being inserted.
                                             */

/*!
  \name Dolby Pulse Decoder-Transcoder operation modes

  \anchor DLB_DDT_OP_MODE_VAL
  Possible values for the operation mode of a DDT instance provided in the DolbyPulseDecoderTranscoderOpen() and
  DolbyPulseDecoderTranscoderGetRequiredBufferSize() functions. For the single decode use case there is only the
  main instance. An associated instance is only present in the dual decode use case and generates only PCM output
  to be fed into the mixer. A restricted associated instance will reject multi-channel
  input bitstreams and has therefore reduced memory demands.
*/
/* @{ */
#define DDT_MAIN                  0  /*!< DDT instance with full functionality, drives the mixing process in dual decode
                                       and generates all activated output signals */
#define DDT_ASSOCIATED            1  /*!< DDT instance for decoding the associated audio stream in a dual decode use case. */
#define DDT_RESTRICTED_ASSOCIATED 2  /*!< DDT instance for decoding an associated audio stream with up to two channels in a
                                       dual decode use case. */
/* @} */

/*!
  \name Dolby Pulse Decoder-Transcoder return values

  \anchor DLB_DDT_RET_VAL
  Return values for functions of the API, e.g. DolbyPulseDecoderTranscoderProcess().
  These defines shall be used when evaluating return values from the converter.
*/
/* @{ */
#define DOLBY_PULSE_DECODER_TRANSCODER_OK            (int16_t)0x0000 /*!< Return value in case of normal operation */
#define DOLBY_PULSE_DECODER_TRANSCODER_DEC_ERROR     (int16_t)0x0001 /*!< Return value indicating a corrupt bitstream detected by the Dolby Pulse decoder.
									The generated output signal is faded out smoothly and may still be
									played back, however, there may be undetected frame losses. */
#define DOLBY_PULSE_DECODER_TRANSCODER_UNDERRUN      (int16_t)0x0002 /*!< Return value indicating that the input buffer did not contain
									enough access units for generating one output frame. This happens <br>
                                                                        1.) for a LOAS stream as long as no audio specific config is received.
                                                                        Like for a corrupt input bitstream, the generated output signal is
									faded out smoothly and may still be played back. <br>
                                                                        2.) if push mode is enabled (bAllowPartialFeed=1). In this case, this value indicates
                                                                        that not enough data was found at the input to generate an output frame.
                                                                      */
#define DOLBY_PULSE_DECODER_TRANSCODER_RESET         (int16_t)0x0004 /*!< Return value indicating that the decoder has been internally reset
                                                                        because either the sample rate or the channel configuration has changed.
                                                                        The new configuration can be be queried using DolbyPulseDecoderTranscoderGetStreamInfo(). */
#define DOLBY_PULSE_DECODER_TRANSCODER_FLUSHING_COMPLETE (int16_t)0x0008 /*!< Return value indicating that no more audio is pending for output. */
#define DOLBY_PULSE_DECODER_TRANSCODER_UNSUPPORTED_CHANNEL_MODE (int16_t)0x0010 /*!< Return value indicating that the associated instance has decoded a channel mode
                                                                                   which is not allowed in the current use case (e.g. multichannel in restricted
                                                                                   associated instance or dual mono in the mixing use case) */
#define DOLBY_PULSE_DECODER_TRANSCODER_INVALID_PARAM (int16_t)0x8000 /*!< Return value indicating a parameter being out of range or an invalid combination of settings */
/* @} */


/*!
  \brief IDs for DolbyPulseDecoderTranscoderSetProcessParam()
*/
typedef enum {
  DOLBY_PULSE_DECODER_TRANSCODER_CTRL_RFMODE_ID,
  /*!< This ID is used to pass a uint16_t value which turns on or off RF mode
    for dynamic range control invoked for the downmix output. Possible values:\n
      0: DRC runs in Line Mode (default)\n
      1: DRC runs in RF mode\n
    The mode may be toggled whenever a change of the DRC type is desired.
    It becomes effective on the next call to DolbyPulseDecoderTranscoderProcess().
    The default mode after opening the instance is the Line mode.
  */
  DOLBY_PULSE_DECODER_TRANSCODER_CTRL_DMXMODE_ID,
  /*!< This ID is used to pass a uint16_t value which controls the downmix operation.
    Possible values:\n
      0: Lt/Rt downmix type (default)\n
      1: Lo/Ro downmix type\n
      2: ARIB downmix type\n
    The mode may be toggled whenever a change of the downmix type is desired.
    It becomes effective on the next call to DolbyPulseDecoderTranscoderProcess().
    The default mode after opening the instance is the matrix compatible Lt/Rt
    downmix mode.
  */
  DOLBY_PULSE_DECODER_TRANSCODER_CTRL_DRCSCALEHIGH_ID,
  /*!< This ID is used to pass a uint16_t value which scales the impact
    of DRC information for attenuation of high level signals (high level cut).
    The value is given in percent (0-100).
    A partial compression can be achieved between full attenuation (default)
    and no attenuation of loud passages of the audio signal.
    Note that in order to preserve the clipping protection, the cut scale factor
    has no effect on a multichannel downmix.
  */
  DOLBY_PULSE_DECODER_TRANSCODER_CTRL_DRCSCALELOW_ID,
  /*!< This ID is used to pass a uint16_t value which scales the impact
    of DRC information for amplification of low level signals (low level boost).
    A partial compression can be achieved between full DRC gains (default)
    and no amplification of quiet passages of the audio signal.
  */
  DOLBY_PULSE_DECODER_TRANSCODER_CTRL_DRCSCALEHIGHMULTI_ID,
  /*!< This ID is used to pass a uint16_t value which scales the impact
    of DRC information for attenuation of high level signals (high level cut).
    The value is given in percent (0-100).
    A partial compression can be achieved between full attenuation (default)
    and no attenuation of loud passages of the audio signal.
    Note that this cut scale factor has effect only on multichannel output for
    main audio.
  */
  DOLBY_PULSE_DECODER_TRANSCODER_CTRL_DRCSCALELOWMULTI_ID,
  /*!< This ID is used to pass a uint16_t value which scales the impact
    of DRC information for amplification of low level signals (low level boost).
    A partial compression can be achieved between full DRC gains (default)
    and no amplification of quiet passages of the audio signal.
    Note that this boost scale factor has effect only on multichannel output for
    main audio.
  */
  DOLBY_PULSE_DECODER_TRANSCODER_CTRL_MULTICHOUT_ID,
  /*!< This ID is used to set the multichannel PCM output for main audio
    to one of the below values:\n
      0: no multichannel output \n
      1: multichannel output (default) \n
    this multichannel output is with drc application
  */
  DOLBY_PULSE_DECODER_TRANSCODER_CTRL_MIXER_ENABLED_ID,
  /*!< This ID is used to pass a uint16_t value turning on (1) or off (0)
    mixing of main and associated audio. When mixing is on, the main audio
    instance invokes an external mixer from DolbyPulseDecoderTranscoderProcess()
    via a callback function. The callback function pointer must have been
    set up using DolbyPulseDecoderTranscoderInstallMixerCallback().
    The mixer adds the output from the associated audio instance
    (having been processed first) to the current block of audio prior to Dolby Digital encoding.
    In order to allow mixing streams with different Program Reference Levels,
    the Dolby Pulse Decoder is configured to adjust the PRL to a level of
    -31 dB (exactly 31.1064 dB) when mixing is enabled.
  */
  DOLBY_PULSE_DECODER_TRANSCODER_CTRL_MIXER_MODE_ID,
  /*!< This ID is used to pass a uint16_t value which distinguishs between blocking (1)
    and non-blocking mixing operation mode. In blocking mode the main instance returns
    right after the mixer if not enough samples from the associated instance are available.
    In non-blocking mode, the main instance will not wait for the associated samples, but
    will continue processing without an actual mixing taking place.
  */
  DOLBY_PULSE_DECODER_TRANSCODER_CTRL_DUALMODE_ID,
  /*!< This ID is used to set the dual mono reproduction mode for the PCM output
    to one of the following values:\n
      0: Stereo (default) \n
      1: Left mono signal at both output channels, attenuated by 3 dB \n
      2: Right mono signal at both output channels, attenuated by 3 dB \n
    A Dolby Pulse stream containing two single channel elements (SCEs) is
    transcoded into a dual mono Dolby Digital stream. Note that a common program
    reference level and the same compression values are used for both channels,
    independent of the chosen dual mono reproduction mode.
  */
   DOLBY_PULSE_DECODER_TRANSCODER_CTRL_TRANSPORTFORMAT_ID,
  /*!< This ID is used to set the transport format
    to one of the following values:\n
      0: Autodetect (default)\n
      1: ADTS \n
      2: LOAS \n
      3: RAW \n
   This can be used to DLB_RESTRICT the decoder to a specific transport format
   (for example, if it is known that the implementation is only used in
   DVB markets, ADTS need not be recognized). This will increase transport robustness,
   while preserving runtime versatility/diversity. If the input is raw AUs (as from an MP4 file),
   use of this function is mandatory (with format raw).
   In this case, use of DolbyPulseDecoderTranscoderConfigure() is also mandatory.
  */
  DOLBY_PULSE_DECODER_TRANSCODER_CTRL_DIALNORM_ID
  /*!< This ID is used to pass a uint16_t value to control the default
  dialnorm. It is scaled to dB/4 resolution and the valid range of this parameter is
  0 to 127, corresponding to approx. 0 to -31.75 dBFS.
  The default would be used until the first time that dialnorm is received in the ES
  (and at that time be overwritten with that dialnorm).
  If the default dialnorm is not explicitly set, a value of 108 (-27 dBFS) applies.
 */
  ,DOLBY_PULSE_DECODER_TRANSCODER_CTRL_CONFORMANCE_MODE_ID
  /*!< This ID is used to trigger an operation mode, where the decoder output is
    directly available in the 6ch virtualizer output without any processing afterwards.
    This allows an easy method to measure the MPEG conformance of the decoder. Note that
    the other outputs are not filled with meaningful data.
  */
} DOLBY_PULSE_DECODER_TRANSCODER_CTRL_ID;


/*!
  \brief Query memory requirements for transcoder instance

  This function should be called by the encapsulating application in
  order to determine the size of the contiguous memory blocks that need
  to be allocated to the current instance of the transcoder.

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
  this is a memory block in RAM that must not be overwritten in other
  sections of the code (persistent). However, it is less frequently used
  than the 'intern' memory blocks and will therefore lead to a smaller
  performance hit when located in slower external memory.

  Note that the transcoder does not perform memory allocation on its own.
  Hence, no DolbyPulseDecoderTranscoderClose() function is provided because there are no
  resources that the decoder library could free.

  \return \ref DLB_DDT_RET_VAL "Dolby Pulse Decoder-Transcoder return value"
*/
int32_t
DolbyPulseDecoderTranscoderGetRequiredBufferSize(uint32_t *pInternStaticSize, /*!< OUT: Size of internal static memory in bytes  */
						 uint32_t *pInternDynamicSize,/*!< OUT: Size of internal dynamic memory in bytes */
						 uint32_t *pExternStaticSize, /*!< OUT: Size of external static memory in bytes */
						 uint16_t  operationMode      /*!< IN:  Determines the operation mode of an instance
                                                                                  (see \ref DLB_DDT_OP_MODE_VAL) */
                                                 ,uint16_t  bMS10Mode         /*!< IN:  Flag indicating MS10 mode is used */
						 );


/*!
  \brief Initialize one transcoder instance

  This function is used to initialize the transcoder and to
  retrieve a handle required for DolbyPulseDecoderTranscoderProcess().

  DolbyPulseDecoderTranscoderGetRequiredBufferSize() may be called prior to DolbyPulseDecoderTranscoderOpen()
  to determine the memory requirements.
  Memory blocks matching (at least) the requested sizes shall be passed
  via internStatic, internDynamic and externStatic.

  \return A pointer to a transcoder instance; NULL in case of errors
  (i.e. invalid parameters).
*/
HANDLE_DOLBY_PULSE_DECODER_TRANSCODER
DolbyPulseDecoderTranscoderOpen(uint32_t * pInternStatic,    /*!< IN: Pointer to static internal memory */
				uint32_t * pInternDynamic,   /*!< IN: Pointer to dynamic internal memory */
				uint32_t * pExternStatic,    /*!< IN: Pointer to static external memory */
                                uint16_t  bAllowPartialFeed, /*!< IN: Flag enabling feeding of less input data
                                                                than necessary to complete an output frame */
                                uint16_t   bMS10Mode,        /*!< IN: Flag that disables multichannel mixing for main and associated audio
                                                                and preserves the original sample rate for the PCM outputs.
                                                                I.e. upsampling to 48 kHz affects the Dolby Digital output only. */
				uint16_t   operationMode,    /*!< IN:  Determines the operation mode of an instance
                                                                 (see \ref DLB_DDT_OP_MODE_VAL) */
                                uint16_t   bFilePlaybackMode /*!< IN: Flag enabling file playback mode (raw data frames) */
				);

/*!
  \brief Configure decdoder with audio specific config (ASC)
  The function will take an ASC, and configure the decoder correspondingly.
  It will always reset the decoder

  return \ref DLB_DDT_RET_VAL "Dolby Pulse Decoder-Transcoder return value"
*/
int16_t
DolbyPulseDecoderTranscoderConfigure(HANDLE_DOLBY_PULSE_DECODER_TRANSCODER hTranscoder, /*!< IN:   Instance handle obtained by DolbyPulseDecoderTranscoderOpen() */
                                     unsigned char *ascData,  /*!< IN:   Pointer to audio specific config */
                                     int ascLen  /*!< IN:   Length of audio specific config in bytes*/
                                    );
/*!
  \brief Decode input bitstream and generate one output frame

  The function decodes the supplied Dolby Pulse bitstream and, depending on the mode of operation,
  converts it to a Dolby Digital bitstream.
  Due to the different frame rates for AAC, HE-AAC and Dolby Digital, every call to DolbyPulseDecoderTranscoderProcess()
  may consume 0, 1 or 2 input frames, while creating one output frame.

  \return \ref DLB_DDT_RET_VAL "Dolby Pulse Decoder-Transcoder return value"
*/
int16_t
DolbyPulseDecoderTranscoderProcess(HANDLE_DOLBY_PULSE_DECODER_TRANSCODER hTranscoder, /*!< IN:   Instance handle obtained by DolbyPulseDecoderTranscoderOpen() */
				   unsigned char * const pInBitstream,                /*!< IN:   Pointer to input bitstream buffer (Dolby Pulse).
											When the bitstream has ended (typically in case of file playback), a NULL pointer
											can be passed in order to flush the audio buffers inside the transcoder.
											It may take multiple calls until the final (non-silent) audio becomes available at the output.
											This situation is indicated by the return value DOLBY_PULSE_DECODER_TRANSCODER_FLUSHING_COMPLETE. */
				   const int             inBufferLen,                 /*!< IN:   Length of input buffer in bytes */
				   uint16_t     *       numBytesRead,               /*!< OUT:  Number of bytes consumed from the input buffer */
				   unsigned char * const pOutBitstream,               /*!< IN:   Pointer to output bitstream buffer (Dolby Digital) */
				   uint16_t     *       numBytesWritten,            /*!< OUT:  Number of bytes written to the output bitstream buffer */
				   int16_t      *       pOutPCM,                    /*!< OUT:  Decoded and downmixed stereo output signal (sample interleaved) */
				   int16_t      *       pOutPCM6ch,                 /*!< OUT:  Decoded 6ch output signal (sample interleaved) */
                                   uint32_t     *       pOutRate,                   /*!< OUT:  Sample rate of PCM data in pOutPCM */
				   uint16_t     *       pOutLen,                    /*!< OUT:  Number of samples per channel in pOutPCM */
                                   uint16_t*            pStereoDsurmod              /*!< OUT:  dsurmod of stereo PCM out, indicates Dolby Surround encoding */
                             ,TRANSCODER_CHANNEL_CONFIG* pVirtualChannelConfig        /*!< OUT:  Descibes the PCM 6ch output channel configuration */
				   );


/*!
  \brief Set configuration parameter for an open instance

  A number of configuration parameters as defined in #DOLBY_PULSE_DECODER_TRANSCODER_CTRL_ID
  are available to control the behaviour of DolbyPulseDecoderTranscoderProcess().
  This function allows a specific parameter to be changed by supplying its identification
  number as well as the desired value. The new value is passed by reference to a
  data type as described in #DOLBY_PULSE_DECODER_TRANSCODER_CTRL_ID.

  \return \ref DLB_DDT_RET_VAL "Dolby Pulse Decoder-Transcoder return value"
*/
int16_t
DolbyPulseDecoderTranscoderSetProcessParam(HANDLE_DOLBY_PULSE_DECODER_TRANSCODER hTranscoder,   /*!< IN: Instance handle obtained by DolbyPulseDecoderTranscoderOpen() */
					   const DOLBY_PULSE_DECODER_TRANSCODER_CTRL_ID paramID, /*!< IN: ID of parameter to modify */
					   const void * pParamVal                               /*!< IN: New value of parameter to modify */
					   );


/*!
  \brief Retrieve parameters of the bit stream that is currently decoded in the Dolby Pulse decoder.

  \return \ref DLB_DDT_RET_VAL "Dolby Pulse Decoder-Transcoder return value"

*/
int16_t
DolbyPulseDecoderTranscoderGetStreamInfo(HANDLE_DOLBY_PULSE_DECODER_TRANSCODER hTranscoder, /*!< IN:  Instance handle obtained by DolbyPulseDecoderTranscoderOpen() */
					 TRANSCODER_STREAMINFO *tStreamInfo                 /*!< OUT: Stream info structure that is filled by this function. */
					 );

/*!
  \brief  Callback function pointer definition for DolbyPulseDecoderTranscoderInstallDecInfoCallback()

  The transcoder calls an externally defined function of this type every time an access unit (AU)
  has been decoded. The function arguments supply some details about the AU that allow
  timing information to be derived.<br>
  <i>pDecInfoCallbackArg</i> specifies a pointer to custom data which can be used to store the
  result of the processing done inside the callback function. Since a void pointer is used,
  the actual data type can be defined by the licensee to fit the needs of his implementation
  of the callback function (see main.c for an example callback function implementation). <br>
  <i>start</i> identifies the beginning of the AU, i.e. the byte position of the syncword (ADTS or LOAS)
  inside the bitstream input buffer.<br>
  <i>stop</i> identifies the end of the AU, or more precisely addresses the next byte after its end.<br>
  <i>pStreamInfo</i> contains information about the current stream like the sample rate of the audio signal
  rendered by the decoder or the current frame length, i.e. the number of audio samples per channel
  rendered by decoding the AU. This may be either 1024 (AAC) or 2048 (HE-AAC or upsampled AAC).
  Dividing the number of output samples by the sample rate yields the frame duration in seconds.<br>
  <i>transportFormat</i> indicates the transport format of the bitstream like ADTS or LOAS (see trnsptGetFormat()).<br>
  <i>errorflag</i> indicates a syntactically incorrect AU. Especially at high bit error rates,
  frame losses or decoder misconfigurations may have occurred, resulting in potentially unreliable
  values in the function parameters.
*/
typedef void (*DECINFO_CALLBACK)(void     * pDecInfoCallbackArg,
                                 uint32_t  start,
                                 uint32_t  stop,
                                 TRANSCODER_STREAMINFO *pStreamInfo,
                                 uint16_t  transportFormat,
                                 uint16_t  errorflag);


/*!
  \brief  Set up a callback function notifying about decoded access units

  A callback function pointer can optionally be supplied to the transcoder which helps keeping
  track of decoded access units (AU). If a callback function is installed, it will be called
  on every AU decoded by the transcoder.
  Especially when DolbyPulseDecoderTranscoderProcess() consumes two AUs at a time, this
  mechanism allows boundaries of every single AU to be analyzed. This information can be used
  to identify which AU is the first one starting in a specific PES packet. A presentation
  timestamp conveyed with the packet can then be assigned to the correct AU.

  \return  DOLBY_PULSE_DECODER_TRANSCODER_OK on success, DOLBY_PULSE_DECODER_TRANSCODER_INVALID_PARAM otherwise.
*/
int16_t
DolbyPulseDecoderTranscoderInstallDecInfoCallback(HANDLE_DOLBY_PULSE_DECODER_TRANSCODER hTranscoder, /*!< IN:  Instance handle obtained by DolbyPulseDecoderTranscoderOpen() */
                                                  DECINFO_CALLBACK   pDecInfoCallbackFunc, /*!< IN: Pointer to user defined function. Passing a NULL pointer disables the callback. */
                                                  void             * pDecInfoCallbackArg   /*!< IN: Pointer argument that will be passed on to the callback function */
                                                  );


/*!
  \brief  Callback function pointer definition for DolbyPulseDecoderTranscoderInstallMixerCallback()

  When mixing of main audio and associated audio is enabled, the transcoder calls an
  externally defined function of this type every time an output frame is generated.<br>
  <i>pMixerCallbackArg</i> specifies a pointer to custom data which is used to pass through
  control information to the mixer. Since a void pointer is used, the actual data type can be
  defined by the licensee to fit the needs of his implementation of the callback function
  (see mix.c for an example callback function implementation).<br>
  <i>inOutLen</i> specifies the size in samples per channel of the block to be mixed.
  Note that the block size depends on the sample rate and varies when the sample rate converter
  upsamples from 44.1 kHz to 48 kHz.
  <TABLE border=1 cellspacing=3 cellpadding=3>
  <TR><TH> Sample rate </TH><TH> nSamplesMix  </TH></TR>
  <TR><TD>    32 kHz   </TD><TD> 1024         </TD></TR>
  <TR><TD>  44.1 kHz   </TD><TD> 1411 or 1412 </TD></TR>
  <TR><TD>    48 kHz   </TD><TD> 1536         </TD></TR>
  </TABLE>
  <i>pChannelConfig</i> specifies the channel configuration of the supplied audio data
  described by audio coding mode (acmod) and a flag indicating the presence of an LFE channel.
  Valid audio coding modes for mixing are DD_MONO=1, DD_STEREO=2, and DD_3_2=7.
  The PCM buffers are supplied using a fixed layout for all channel configurations:
  The 6 pointers in <i>ppInOutPCM</i> refer to separate PCM buffers for all channels,
  using the cinema channel order L, C, R, Ls, Rs, LFE. E.g. in case of DD_MONO, only
  <i>ppInOutPCM[1]</i> is relevant.
  Note that this format differs from (sample-interleaved) 6 channel output supplied by
  DolbyPulseDecoderTranscoderProcess().<br>

  \return \ref DLB_DDT_MIXER_RET_VAL "Mixer return value"
*/
typedef int (*MIXER_CALLBACK)(void                       * pMixerCallbackArg,
                              DLB_SFRACT                ** ppInOutPCM,
                              uint16_t                    inOutLen,
                              TRANSCODER_CHANNEL_CONFIG  * pChannelConfig);


/*!
  \brief  Set up a callback function for main/associated audio mixing

  A callback function pointer must be supplied to the main audio transcoder instance
  if the associated audio signal shall be mixed in prior to Dolby Digital encoding.
  The standard procedure is to install the mixer callback function at startup time, e.g.
  directly after calling DolbyPulseDecoderTranscoderOpen() for the main audio instance.
  No mixer callback needs to be installed for the associated audio decoder instance.
  To actually switch on the mixing mode, DolbyPulseDecoderTranscoderSetProcessParam()
  must be called in addition for the main audio instance
  (see DOLBY_PULSE_DECODER_TRANSCODER_CTRL_MIXER_ENABLED_ID).
  Note that this function is not intended to be called for the associated instance and
  will return DOLBY_PULSE_DECODER_TRANSCODER_INVALID_PARAM in this case.

  \return  DOLBY_PULSE_DECODER_TRANSCODER_OK on success, DOLBY_PULSE_DECODER_TRANSCODER_INVALID_PARAM otherwise.
*/
int16_t
DolbyPulseDecoderTranscoderInstallMixerCallback(HANDLE_DOLBY_PULSE_DECODER_TRANSCODER hTranscoder,
                                                /*!< IN:  Instance handle obtained by DolbyPulseDecoderTranscoderOpen() */
                                                MIXER_CALLBACK   pMixerCallbackFunc, /*!< IN: Pointer to user defined function. Passing a NULL pointer is not allowed; DolbyPulseDecoderTranscoderSetProcessParam() can be used to disable the mixer callback again. */
                                                void           * pMixerCallbackArg   /*!< IN: Pointer argument that will be passed on to the callback function */
                                                );


/*!
  \brief  Pull audio samples from the internal PCM FIFO buffer

  This function is intended to be used for mixing main audio with associated audio.
  As demonstrated in MixWithAssociatedAudioCallback(), the mixer callback function gets
  a block of main audio via its arguments and needs to fetch the corresponding block
  of associated audio from the associated audio decoder instance.
  The function provides a pointer for each audio channel that represents the current
  start position of decoded audio buffered in the internal PCM FIFO.
  The FIFO output pointer is advanced by the requested frame length, i.e. the audio
  data is consumed from the internal buffer.
  Note that this function is not intended to be called for the main audio transcoder instance.

  \return DOLBY_PULSE_DECODER_TRANSCODER_OK if the requested amount of samples is available, DOLBY_PULSE_DECODER_TRANSCODER_UNDERRUN otherwise.
  However, if the instance handle is configured for main audio processing, the function returns DOLBY_PULSE_DECODER_TRANSCODER_INVALID_PARAM.
*/
int16_t DolbyPulseDecoderTranscoderGetPcmFrame(HANDLE_DOLBY_PULSE_DECODER_TRANSCODER hDecoder,
                                                /*!< IN:  Instance handle of associated audio decoder obtained by DolbyPulseDecoderTranscoderOpen() */
                                                DLB_SFRACT ** apPCM,     /*!< OUT: Pointers to output channels. The channel layout is the same as describe in MIXER_CALLBACK. */
                                                uint16_t frameLen,      /*!< IN:  Length of requested frame in samples per channel */
                                                TRANSCODER_CHANNEL_CONFIG * pChannelConfig
                                                /*!< OUT: Channel configuration of the supplied audio data, consisting of acmod and a flag for the LFE channel.
                                                  Valid acmod values for mixing: are DD_MONO=1, DD_STEREO=2, and DD_3_2=7. */
                                                );


/*!
  \name Expected return values from mixer callback function

  \anchor DLB_DDT_MIXER_RET_VAL

  This set of \#defines lists possible return values that the mixer callback function
  (see also MIXER_CALLBACK) could generate.
*/
/* @{ */
#define MIX_OK                    0x0000 /*!< Return value indicating successful mixing */
#define MIX_UNINITIALIZED         0x0100 /*!< Return value indicating that no mixing was done due to missing stream information,
                                           usually because no valid access unit was decoded yet */
#define MIX_SAMPLE_RATE_MISMATCH  0x0200 /*!< Return value indicating that no mixing was done because the sample rate of the
                                           associated audio stream does not match the sample rate of the main audio stream
                                           (which is no allowed in the context of DVB). */
#define MIX_NO_TIMESTAMP          0x0400 /*!< Return value indicating that no mixing was done due to missing timing information
                                          to synchronize main and associated audio. */
#define MIX_ASSO_UNDERRUN         0x0800 /*!< Return value indicating that no mixing was done because the available amount of
                                           associated audio does not cover the complete time frame to be mixed. */
#define MIX_ASSO_LATE             0x1000 /*!< Return value indicating that no mixing was done because all available decoded
                                           associated audio samples are outdated in relation to the time frame to be mixed. */
#define MIX_ASSO_AHEAD            0x2000 /*!< Return value indicating that no mixing was done because the timestamps indicate
                                           that the decoded associated data is too new to be mixed into the current block.
                                           If there is an overlap (e.g. associated audio is available for the 2nd half
                                           of the mixing time frame), still no mixing is suggested to allow a fade-in
                                           at the start of the next mixing time frame. */
#define MIX_CORE_PARAM_INVALID    0x4000 /*!< Return value indicating that an invalid parameter has been passed to the mixer core */
/* @} */




/*!
  \brief  Callback function pointer definition for DolbyPulseDecoderTranscoderInstallDolbyVolumeCallback()

  \return DOLBY_PULSE_DECODER_TRANSCODER_OK on success, DOLBY_PULSE_DECODER_TRANSCODER_INVALID_PARAM otherwise.
*/
typedef int (*DOLBY_VOLUME_CALLBACK)(void                       * pDolbyVolumeCallbackArg,
                                     DLB_SFRACT                ** ppInOutPCM,
                                     uint16_t                    inOutLen,
                                     TRANSCODER_CHANNEL_CONFIG  * pChannelConfig);


/*!
  \brief  Set up a callback function for leveling the decoded signal

  A callback function pointer must be supplied to the main audio transcoder instance
  if the decoded and potentially mixed signal shall be leveled to achieve a consistent
  audio level. The callback will not be called if passing a NULL pointer instead
  of a function pointer is passed to the DolbyPulseDecoderTranscoderInstallDolbyVolumeCallback
  function.

  \return  DOLBY_PULSE_DECODER_TRANSCODER_OK on success, DOLBY_PULSE_DECODER_TRANSCODER_INVALID_PARAM otherwise.
*/
int16_t
DolbyPulseDecoderTranscoderInstallDolbyVolumeCallback(HANDLE_DOLBY_PULSE_DECODER_TRANSCODER hTranscoder, /*!< IN: Instance handle obtained by DolbyPulseDecoderTranscoderOpen() */
                                                      DOLBY_VOLUME_CALLBACK  pDolbyVolumeCallbackFunc,   /*!< IN: Pointer to user defined function. Passing a NULL pointer disables the callback. */
                                                      void           * pDolbyVolumeCallbackArg           /*!< IN: Pointer argument that will be passed on to the callback function */
                                                      );




/*!
  \brief Query algorithmic delay for output data

  The audio signal from a decoded Dolby Pulse frame does not appear
  directly in the output frame
  or at the mixer breakout
  provided by DolbyPulseDecoderTranscoderProcess().
  The function reports <i>static</i> and <i>dynamic</i> delay values described by the
  TRANSCODER_DELAYINFO struct.
  The result refers to the previous call of DolbyPulseDecoderTranscoderProcess(), i. e.
  DolbyPulseDecoderTranscoderGetDelay() is called <b>after</b> DolbyPulseDecoderTranscoderProcess().

  The amount of audio data that is currently buffered inside the transcoder and
  pending for output (except for the overlap-add buffer of the AAC decoder, which is already
  considered by the PTS) is the sum of static and dynamic delay.

  Static and dynamic delay can be used to calculate a presentation time stamp
  for the output data from a presentation time stamp associated with the AAC input:

  \li Call DolbyPulseDecoderTranscoderProcess(). This consumes up to two audio access units
      which can be monitored by the calling instance by setting up a callback function
      using DolbyPulseDecoderTranscoderInstallDecInfoCallback().
  \li After the processing function has returned, query static and dynamic delay by
      using DolbyPulseDecoderTranscoderGetDelay().
  \li Call DolbyPulseDecoderTranscoderGetStreamInfo() to query
      the input frame size in samples. Alternatively, this information is available
      inside the callback function and could be saved using the user-defined pointer
      argument.
      The duration of one AAC access unit can now be derived from these values.
      The duration of one output frame is calculated using the number of output samples
      as indicated by DolbyPulseDecoderTranscoderProcess().
  \li Get a presentation time stamp \f$PTS_{IN}\f$ associated with the AAC input that
      DolbyPulseDecoderTranscoderProcess() has just consumed. Please see the code
      under <tt>\#ifdef PRINT_TIME_INFO</tt> in main.c for details.
  \li The presentation timestamp corresponding to the beginning of the output block is
      then calculated as
    \f[ PTS_{OUT} = PTS_{IN} + \mbox{input frame duration} - (\mbox{output frame duration} + \mbox{dynamic delay} + \mbox{static delay}) \f].

  Notes:
  \li \f$PTS_{OUT}\f$ is lower than \f$PTS_{IN}\f$, which simply means that the generated
      output block has to be presented earlier because it (partly) originates from earlier
      access units that were still pending for output in the transcoder's internal buffers.
  \li PCM and Dolby Digital outputs are synchronized, hence the same presentation time applies to both.

  The graph below illustrates the periodically varying latency between input and output of the
  Dolby Pulse Decoder-Transcoder for 6 modes of operation. For every mode, the upper row shows a box
  for each AAC access unit that can be considered to contain the audio signal, which a plain AAC decoder
  (without SBR or other tools invoking delay) would render from that access unit.
  The corresponding PTS refers to the left end side of the box.
  The lower row shows how the output blocks from the transcoder (Dolby Digital or PCM) need to be
  placed in time in order to align the contained audio signal with the audio signal corresponding to the upper row.
  The static delay queried by this function is visible in the diagram as the amount of time
  the output bar starts before the input bar (e.g. 35.7 ms in case of 32 kHz HE-AAC).
  Input and output blocks consumed and generated during the same call to DolbyPulseDecoderTranscoderProcess()
  are displayed using the same colour. Consecuteve output blocks of the same colour indicate two calls to
  DolbyPulseDecoderTranscoderProcess() where no input block is consumed in the 2nd call, i.e. the data
  is taken from the internal PCM FIFO.
  For every mode, the maximum input-to-output latency is indicated by an arrow labelled with
  the delay value in milliseconds.
  The graph does not include the delay of the Dolby Volume processing that may be
  enabled by DolbyPulseDecoderTranscoderInstallDolbyVolumeCallback(). Dolby Volume 258
  adds 5.3 ms (256 samples at 48 kHz) to all delay values shown in the graph.

  \image html timing_rs44.png "Visualization of periodic latency for different input frame durations"

  \return  DOLBY_PULSE_DECODER_TRANSCODER_OK on success or -1 in case no stream information is available
*/
int16_t
DolbyPulseDecoderTranscoderGetDelay(HANDLE_DOLBY_PULSE_DECODER_TRANSCODER hTranscoder, /*!< IN:  Instance handle obtained by DolbyPulseDecoderTranscoderOpen() */
				    TRANSCODER_DELAYINFO                  *pDelayInfo  /*!< OUT: Delay information is available in this struct on successful return */
                                    );


/*!
  \brief Query maximum lag between input and output

  Indicates the maximum difference between the PTS associated with the latest decoded
  AAC/HE-AAC access unit and the newest block of output data generated by DolbyPulseDecoderTranscoderProcess().
  The specified value constitutes the maximum input-to-output latency over all operating modes.
  It needs to be considered for setting up a delay buffer used to achieve A/V synchronization.

  The returned value is a constant which corresponds to the maximum latency
  the transcoder may induce.
  This worst case occurs for HE-AAC at 44.1 kHz and consists of 4 components:
  \li The FIFO containing one output frame but 1 sample (1411 samples at 44.1 kHz = 1535.78 samples at 48 kHz)
  \li Algorithmic delay of SBR (961 samples at 44.1 kHz = 1046 samples at 48 kHz)
  \li Algorithmic delay of sample rate converter (9 samples)
  \li Dolby Digital encoding/decoding delay compensation (256 samples)

  The total maximum latency amounts to 2845 samples or 59.27 ms. Note that the latency
  is returned in samples at 48 kHz although this latency can only occur with a 44.1 kHz
  input stream.

  \return   Maximum input-to-output latency in samples at 48 kHz
*/
int16_t
DolbyPulseDecoderTranscoderGetMaxDelay(void);


/*!
  \brief Query startup delay for just-in-time processing

  Before generating the first output data, the system must typically wait
  for the stream PCR to reach the PTS of the first packet.
  Due to internal reframing that is necessary for feeding the Dolby Digital encoder,
  the transcoder needs an additional startup delay. Depending on the maximum fullness
  of the internal FIFO, incoming frames may be required that correspond to a later
  point in time as if the FIFO was empty.
  For an input sample rate of 44.1 kHz where a conversion to 48 kHz takes place,
  the FIFO fullness after Dolby Digital encoding can get close to one output
  frame of 32 ms.
  For an input sample rate of 48 kHz the FIFO fullness after Dolby Digital encoding
  can reach 1024 samples which corresponds to 21.3 ms.

  Conceptually, the system must delay each incoming packet
  by this additional offset to make sure that data is not consumed before
  it has arrived in the input buffer.

  The figures below illustrate the startup delay for AAC resp. HE-AAC
  input at 48 kHz. The uppermost timeline indicates the equally spaced
  presentation time stamps of incoming frames. The bottom row represents
  the points in time when the processing function DolbyPulseDecoderTranscoderProcess()
  is called. This happens once per Dolby Digital frame, i. e. every 32 ms.
  The figures do not include the processing delay (computing time),
  but assume immediate processing for decoder and encoder.
  For AAC, the first call to the processing function implies
  2 input frames being decoded in order to generate enough samples for one
  Dolby Digital output frame. Hence the system cannot present the first input
  frame at its PTS, but only 1024 samples later, when the 2nd input frame
  arrives.
  For HE-AAC, 3 out of 4 calls to DolbyPulseDecoderTranscoderProcess()
  imply Dolby Pulse decoding. The 3rd input frame arrives just in time when
  the system takes the pre-delay into account (which is again 1024 samples).

  \image html predelayAac.png "Timing diagram for 48 kHz AAC transcoding"

  \image html predelaySBR.png "Timing diagram for 48 kHz HE-AAC transcoding"

  Since the startup delay returned by this function must be known before the
  first call to DolbyPulseDecoderTranscoderProcess(), DolbyPulseDecoderTranscoderGetStartupDelay()
  cannot consider the current input sample rate, but returns a fixed value that
  is just enough for the worst case
  input.
  The returned number of samples assume an input sample rate of 48 kHz.

  \return  Constant startup delay in samples at 48 kHz
*/
int16_t
DolbyPulseDecoderTranscoderGetStartupDelay(void);


#endif /* #ifndef DOLBY_PULSE_DECODER_TRANSCODER_H */
