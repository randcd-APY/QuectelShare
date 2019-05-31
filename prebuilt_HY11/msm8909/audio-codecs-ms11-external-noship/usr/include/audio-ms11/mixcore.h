/****************************************************************************

 This program is protected under international copyright laws as an
 unpublished work. Do not copy.

                  (C) Copyright Dolby International AB (2007 - 2011)
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


/*! \defgroup DLB_MCLIB Mixer Core


\section introMC Introduction

 This package contains an Associate Mixing module for usage in the MS11 DDT component.
 This Mixer module is suitable for fulfilling the requirements of Associate Mixing in DVB.

 The source code is based on the \ref DLB_INTRINSICSLIB,
 a generic set of arithmetic operators that enables easy porting to any 16/32 bit
 fixed point processor.

 \section mcCallingSequence Calling Sequence for the Mixer Core

 This section illustrates how to use Mixer Core (MC) interface.
 Your applications main source file shall include mixcore.h and dlb_types.h.

 -# Call MixCore_QueryMem() to determine the memory size required by the MixCore.
 -# Allocate memory based on the values returned.
 -# Call MixCore_Open()
 -# loop over input frames
  -# Call MixCore_Process() to mix main and associate signal together while applying mixing metadata.
*/
/*@{*/

/*!
  \file   mixcore.h
  \brief  Associate Mixing module for usage in the MS11 DDT component $Revision: $
*/

#ifndef _MIXCORE_H_
#define _MIXCORE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_NSAMPLES (2048)
#define MAX_DD_BLOCKS_PER_FRAME (6)
#define MAX_DD_FRAMESIZE (MAX_DD_BLOCKS_PER_FRAME * MAX_NSAMPLES)
#define MAX_CHANNELS (6)


/*!
  \brief channel mode

  Representation of mono, stereo, 3/2 channel mode.
*/
typedef enum {
  MC_MONO   = 1,
  MC_STEREO = 2,
  MC_3_2    = 7
} MC_ACMOD;

/*!
  \brief acmod channel mask coding

  Representation of acmod channels as summed bit mask values.
  Channels in the PCM streams are coded as [l c r ls rs lfe].
  The corresponding bit mask is            [1 2 4 8  16  32].
*/
typedef enum {
  MC_MONO_MASK   = 2,
  MC_STEREO_MASK = 5,
  MC_3_2_MASK    = 31,
  MC_LFE_MASK    = 32
} MC_ACMOD_MASK;

/*!
  \brief Mixer Core return values

  Return values for MixCore_Process(), MixCore_QueryMem()
  These enum shall be used when evaluating the MixCore return value.
*/
typedef enum
{
  MC_OK              =  0,
  MC_ERR_NSAMPLES    = -1, /*!< number of samples supplied is greater than MAX_NSAMPLES */
  MC_ERR_INV_CH_COMB = -2, /*!< invalid channel combination */
  MC_ERR_RST_CNTE    = -3, /*!< reset and continue are set at the same time */
  MC_ERR_MIX_MD_FB   = -4, /*!< invalid value in mixing metadata: fade_byte */
  MC_ERR_MIX_MD_GBC  = -5, /*!< invalid value in mixing metadata: gain_byte_center */
  MC_ERR_MIX_MD_GBF  = -6, /*!< invalid value in mixing metadata: gain_byte_front */
  MC_ERR_MIX_MD_GBS  = -7, /*!< invalid value in mixing metadata: gain_byte_surround */
  MC_ERR_MIX_MD_PB   = -8, /*!< invalid value in mixing metadata: pan_byte */
  MC_ERR_MIX_MD_FMU  = -9, /*!< invalid value in mixing metadata: fade_main_usr */
  MC_ERR_MIX_MD_FAU  = -10, /*!< invalid value in mixing metadata: fade_assoc_usr */
  MC_ERR_UNDEF       = -11, /*!< unspecified error */
} MC_SUCCESS;

/*! \brief Channel configuration for Mixer Core channel mode, 6 channel output
*/
typedef struct CHANNEL_CONFIG {
  uint16_t acmod;       /*!< Audio coding mode as defined for Dolby Digital.
                               The audio coding mode associated with the output from the Dolby Pulse Decoder-Transcoder
                               can be DD_ONEPLUSONE=0, DD_MONO=1, DD_STEREO=2, or DD_3_2=7. */
  uint16_t bLfePresent; /*!< Flag indicating presence of the low frequency enhancement channel. */
} CHANNEL_CONFIG;

/*! \brief Mixing metadata struct

The following definition of values is from "ETSI TS 101 154 - Digital Video Broadcasting
(DVB); Implementation guidelines for the use of Video and Audio Coding in Broadcasting
Applications based on the MPEG-2 Transport Stream, v1.9.1", chapter "E.2 Syntax and semantics".

AD_fade_byte: Takes values between 0x00 (representing no fade of the main programme
sound) and 0xFF (representing a full fade). Over the range 0x00 to 0xFE one lsb
represents a step in attenuation of the programme sound of 0,3 dB giving a range
of 76,2 dB. The fade value of 0xFF represents no programme sound at all (i.e. mute).
The rate of signalling and the expected behaviour of a decoder to changes in fade byte
are described below.

e.g. fade byte:\n
0x00:   0,0 dB\n
0x01:  -0,3 dB\n
0xFE: -76,2 dB\n
0xFF:   Mute\n

AD_pan_byte: Takes values between 0x00 representing a central forward presentation
of the audio description and 0xFF, each increment representing a 360/256 degree
step clockwise looking down on the listener (i.e. just over 1,4 degrees, see
figure E.2). The rate of signalling and the expected behaviour of a decoder are
described below.

AD_gain_byte_center: Represents a signed value in dB. Takes values between 0x7F
(representing +76,2 dB boost of the main programme centre) and 0x80 (representing
a full fade). Over the range 0x00 to 0x7F one lsb represents a step in boost of
the programme centre of 0,6 dB giving a maximum boost of +76,2 dB. Over the range
0x81 to 0x00 one lsb represents a step in attenuation of the programme centre of
0,6 dB giving a maximum attenuation of -76,2 dB. The gain value of 0x80 represents
no main centre level at all (i.e. mute). The rate of signalling and the expected
behaviour of a decoder to changes in gain byte are described below.

AD_gain_byte_front: As AD_gain_byte_center, applied to left and right front channel.

AD_gain_byte_surround: As AD_gain_byte_center, applied to all surround channels.

e.g. gain byte:\n
0xFF:  -0,6 dB\n
0x81: -76,2 dB\n
0x80:   Mute\n
0x7F: +76,2 dB\n
0x00:   0,0 dB\n

*/
typedef struct MIXING_MD {
  uint16_t  fade_byte;
  int16_t   gain_byte_center;
  int16_t   gain_byte_front;
  int16_t   gain_byte_surround;
  uint16_t  pan_byte;
  DLB_SFRACT fade_main_usr;
  DLB_SFRACT fade_assoc_usr;
} MIXING_MD;


/*! \brief MixCore handle
*/
typedef struct MIXCORE_INSTANCE *MIXCORE_HANDLE;

/*!
  \brief Calculate required buffer size for MixCore instance

  This function should be called by the encapsulating application in
  order to determine the size of the contiguous memory block that needs
  to be allocated to the current instance of the MixCore.

  The memory block size is returned via pointer arguments:

  \li internStaticSize:
  Size of 'intern static' data in bytes. This is a memory block in RAM
  which must not be overwritten in other sections of the code (persistent).

  Note that the MixCore does not perform any internal dynamic memory allocation.
  Hence, no MixCore_Close() function is provided because there are no resources
  that the MixCore library could free.

  \return #MC_OK if successful
*/

int32_t MixCore_QueryMem(uint32_t *internStaticSize     /*!< OUT: Size of internal static memory in bytes */
                );
/*!
  \brief Open one MixCore instance

  This function is used to open the MixCore. Some basic parameters
  are initialized. MixCore_Open() has to be called once prior to a
  call to MixCore_Process().

  MixCore_QueryMem() should be called prior to MixCore_Open().
  A memory block matching (at least) the requested size shall be passed
  via internStatic.

  \return A pointer to a MixCore instance; NULL in case of errors.
*/
MIXCORE_HANDLE MixCore_Open(DLB_LFRACT *internStatic      /*!< IN:  Pointer to internal memory used statically */
            );
/*!
  \brief Mix together one frame (up to 2048 samples) of main and associate audio
  stream (each containing up to 6 channels of audio).

  This is done by taking PCM samples from main (ppPcmMainIn) and associate in
  (ppPcmAssocIn) and writing the result to the main buffer (ppPcmMainIn).
  This is done over the range of nSamples. The Mixer Core mixes these incoming
  PCM samples by adding the respective channels.

  Mixing metadata in pMixingMD is applied to main and assoc before mixing.
  Gain and fade values are applied on the main signal, pan value on the
  associate signal. No read or write operations happens on PCM buffers that
  are not valid according to the channel mode set with pChannelModeMain and
  pChannelModeAssoc.

  Mixing metadata can vary from call to call. In order to avoid discontinuity in
  the signal, the effective gain values (the factors applied on the signals)
  transitions smoothly from an old value to a new value by applying a ramp on the
  gain over a period of 256 samples.

  If the bReset flag is set on call to MixCore_Process(), metadata is applied over
  the whole PCM signal from the first sample, i.e. no ramp is applied.

  If the bContinue flag is set on call to MixCore_Process(), no new metadata set
  is accepted. Instead, ramping is continued until the gains corresponding to the
  last passed metadata set are reached.

  \return #MC_OK if successful, other value of #MC_SUCCESS for any error.
*/
int32_t MixCore_Process(MIXCORE_HANDLE  hMC,               /*!< IN: Handle to the MIXCORE instance obtained by MixCore_Open() */
                DLB_SFRACT   **ppPcmMainIn,       /*!< IN: Main audio in PCM buffer (chMask: [l c r ls rs lfe]) */
                CHANNEL_CONFIG *pChannelModeMain,  /*!< IN: Main audio in channel mode */
                DLB_SFRACT   **ppPcmAssocIn,      /*!< IN: Associate audio in PCM buffer (chMask: [l c r ls rs lfe]) */
                CHANNEL_CONFIG *pChannelModeAssoc, /*!< IN: Associate audio in channel mode */
                int16_t       bReset,            /*!< IN: Flag indicating reset of ramp; no ramp applied */
                int16_t       bContinue,         /*!< IN: Flag indicating continue of ramp; no new metadata is accepted */
                MIXING_MD      *pMixingMD,         /*!< IN: Mixing metadata */
                uint16_t      nSamples           /*!< IN: Number of audio samples */
               );


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MIXCORE_H_ */
/*@}*/
