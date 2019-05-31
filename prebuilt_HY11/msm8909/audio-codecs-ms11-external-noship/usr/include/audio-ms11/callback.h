
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
/*!
  \file   callback.h
  \brief  Typedefs and \#defines used by the example callback functions in callback.c  $Revision: #1 $
*/

#ifndef CALLBACK_H
#define CALLBACK_H

#include "dolbypulsedecodertranscoder.h"
#include "mixcore.h"
#include "ms_mixer.h"

/* #define PRINT_TIME_INFO */

#define MAX_MD_SETS    4    /*!< Maximum number of sections in the bitstream buffer
                                that can be assigned different mixing metadata */
#define MAX_AU_INFOS   3    /*!< Number of access units that are tracked in order to
                                identify AU boundaries in the PCM output */

#define MD_UNUSED   0x7FFF  /*!< Special start offset for metadata in the bitstream buffer
                              indicating that the corresponding metadata set is currently unused.
                              Conceptionally this value corresponds to an offset beyond the end
                              of the bitstream buffer and therefore does not apply. */

#define MD_PAST       -1    /*!< Special start offset for metadata in the bitstream buffer
                              indicating that the corresponding metadata set started in the
                              past but still applies up to the position of the metadata set
                              with the next higher start offset. */

#define  MAX_TIME_SHIFT  512    /*!< Maximum tolerance for main/associated alignment in samples.
                                  Since the tolerance is specified in samples here, the corresponding
                                  time in milliseconds depends on the sample rate. */

/*
  \brief  Access unit properties related to mixing metadata assignment
*/
typedef struct AU_INFO {
  uint32_t packetID;  /*!< Unique identifier of the packet carrying mixing metadata that applies to this AU */
  uint16_t nSamples;  /*!< Length of AU in samples */
} AU_INFO;


/*
  \brief  Simulated PES packet properties

  This struct is used to describe a section of the bitstream input buffer that
  could have been exctracted from a certain PES packet.
  DecInfoCallback() uses this information to assign a packet ID to each decoded access unit.
*/
typedef struct PACKET_INFO {
  uint32_t packetID;       /*!< Unique identifier for this packet. 0 indicates a non-existent packet. */
  int16_t  startOffset;    /*!< Simulated packet border in the elementary stream buffer */
  /* The below members are dummy values which would come from a PES parser in a real system */
  uint16_t bPacketHasPTS;  /*!< Flag to trigger a resynchronization of the running PTS in DecInfoCallback() */
  int32_t  timestamp;      /*!< Presentation timestamp in tics (see ticsPerSample()) */
} PACKET_INFO;


/*! \brief  Collection of data accessible in the decoder information callback function

  A reference to this structure is supplied when setting up the decoder information
  callback function.
  Defining PRINT_TIME_INFO in callback.h invokes some demonstration code that
  calculates and displays timestamps associated with input and output data.
  Due to the lack of real timestamps being evaluated, the input frames
  are assumed to fit together seamlessly.
  The same applies to the generated output, unless the sample rate changes.
  In this case, the internal FIFO is reinitialized and some output data
  may be missing.
*/
typedef struct TIME_INFO {
  int32_t  timestampLatestDecodedAU; /*!< Presentation time of the beginning of the most recently decoded access unit */
  int32_t  timestampNextAU;          /*!< Expected presentation time of the next access unit */
  uint32_t nSkippedAUs;              /*!< Number of access units that have been consumed since the latest PTS,
					 but could not be decoded due to missing ASConfig. This counter is needed
					 for correct synchronization if the first PTS and the first ASConfig do
					 not arrive at the same time. */
  AU_INFO    auInfo[MAX_AU_INFOS];     /*!< History of the most recently decoded access units that may still be
					 present in the internal PCM FIFO buffer. The latest AU corresponds to
                                         index 0 of the array. */
  PACKET_INFO packetInfo[MAX_MD_SETS]; /*!< Simulated PES packets belonging to the current input of the bitstream buffer */
} TIME_INFO;



/*!
  \brief  Time resolution for timestamp calculation

  Specifies the time resolution used when calculating timestamps for input
  and output frames.
  The time resolution of 14.112 MHz corresponds to the smallest common
  clock rate for 32 kHz, 44.1 kHz and 48 kHz, allowing sample exact timestamps
  for these sample rates. Lower clock rates (e.g. 90 kHz) would mean that frame
  durations are not exact and the error would accumulate over time, since
  no transmitted timestamps are evaluated in this example application.
  This code uses 32 bit signed integer variables to represent timestamps
  based on that clock. This means that the counters overflow after approx.
  152 seconds. For a true real time application, the range of the timestamps
  would have to be extended to prevent overflows.
  Note that the timestamps are not really required by this file I/O transcoder,
  but only used to display timing information.
*/
#define  HIRES_CLOCK  14112000




/*! \brief  Static data accessible in the main/associated audio mixer callback function

  A reference to this structure is supplied to the transcoder when setting up the
  mixer callback function.
  It is passed on to the mixer callback function by DolbyPulseDecoderTranscoderProcess().
*/
typedef struct MIXER_INSTANCE {
  HANDLE_DOLBY_PULSE_DECODER_TRANSCODER hTranscoder; /*!< Handle to the main audio Decoder-Transcoder */
  HANDLE_DOLBY_PULSE_DECODER_TRANSCODER hAssDecoder; /*!< Handle to the associated audio decoder */
  TIME_INFO * pTimeInfoMain;                         /*!< Timestamp information for main audio */
  TIME_INFO * pTimeInfoAssociated;                   /*!< Timestamp information for associated audio */
  MIXCORE_HANDLE  hMC;
  uint16_t bMixingMdAvailable;
  MIXING_MD  mixingMetaData[MAX_MD_SETS];            /*!< Mixing meta data related to the latest 3 associated audio access units.
                                                       Index 0 addresses the latest data. */
  uint32_t mixingMetaDataIDs[MAX_MD_SETS];           /*!< Assigns a packet ID to every set of mixing metadata in mixingMetaData[].
                                                       This ID is used to connect the AU_INFO data collected DecInfoCallback()
                                                       to the correct mixing metadata required by MixWithAssociatedAudioCallback(). */

  /* New unified mixer (ms_mixer) related data */
  MS_MIXER_HANDLE h_ms_mixer;                        /*!< Handle to the multistream decoder mixer */
  DLB_LFRACT *p_tmp_buf[2];                          /*!< Temporary buffers for intermediate PCM buffer conversion, each buffer must be at able
                                                          to hold the current number of samples and channels for each program in DLB_LFRACT format */
  int16_t user_pref;                                 /*!< User preference: balance between main and assoc. program */

  int16_t mixStatus;                                 /*!< Contains the latest return value from the mixer callback */

} MIXER_INSTANCE;


typedef struct MIXER_INSTANCE * HANDLE_MIXER;


/* Prototype of callback function for timestamp updates. This must match the DECINFO_CALLBACK type. */
//void DecInfoCallback(TIME_INFO * pTimeInfo,
//                     uint32_t start,
//                     uint32_t stop,
//                     TRANSCODER_STREAMINFO *pStreamInfo,
//                     uint16_t transportFormat,
//                     uint16_t errorflag);

/* Helper functions for processing timing information */
// int32_t ticsPerSample(uint32_t sampleRate);
//int32_t ticsToMilliSec(int32_t tics);


/* Prototype of callback function for main/associated audio mixing. This must match the MIXER_CALLBACK type. */
//int MixWithAssociatedAudioCallback(HANDLE_MIXER  hMixer,
//                                   DLB_SFRACT ** ppInOutPCM,
//                                   uint16_t     inOutLen,
//                                   TRANSCODER_CHANNEL_CONFIG * pChannelConfig);

#endif /* #define CALLBACK_H */
