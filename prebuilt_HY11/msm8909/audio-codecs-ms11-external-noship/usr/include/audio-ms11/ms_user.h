/*
 *        Confidential Information - Limited distribution to authorized persons
 *        only. This material is protected under international copyright laws as
 *        an unpublished work. Do not copy.
 *        Copyright (C) 2010-2011 Dolby Laboratories Inc.
 *        Copyright (C) 2010-2011 Dolby International AB
 *        All rights reserved.
 *
 */

/*! \addtogroup msd */

/*! \file ms_user.h
 *  \brief User Interaction with the command line frontend - header file
 */

#ifndef MS_USER_H_
#define MS_USER_H_

#include "ms_exec.h"

/* List of defines used in ms_user.c */
#define MS_EXT_LENGTH           4     /*!< Number of characters expected in file extensions */
#define MS_MIN_VERBOSITY        0     /*!< Minimum verbosity mode                           */
#define MS_MAX_VERBOSITY        3     /*!< Maximum verbosity mode                           */
#define MS_MIN_DV               0     /*!< Minimum Dolby Volume mode                        */
#define MS_MAX_DV               1     /*!< Maximum Dolby Volume mode                        */
#define MS_MIN_KT               0     /*!< IDK Test Mode disabled                           */
#define MS_MAX_KT               1     /*!< IDK Test Mode enabled                            */
#define MS_MIN_COMP_PROF        1     /*!< Minimum DDRE compression profile                 */
#define MS_MAX_COMP_PROF        3     /*!< Maximum DDRE compression profile                 */
#define MS_MIN_PROG_REFLEV      0     /*!< Minimum DDRE program reference profile           */
#define MS_MAX_PROG_REFLEV     31     /*!< Maximum DDRE program reference profile           */
#define MS_MIN_DIALNORM         0     /*!< Minimum Pulse dialnorm value                     */
#define MS_MAX_DIALNORM       127     /*!< Maximum Pulse dialnorm value                     */
#define MS_MIN_TRANSFMT         0     /*!< Minimum Pulse transport format value             */
#define MS_MAX_TRANSFMT         3     /*!< Maximum Pulse transport format value             */
#define MS_MIN_MULTCH_MODE      0     /*!< Minimum multichannel mode value                  */
#define MS_MAX_MULTCH_MODE      1     /*!< Minimum multichannel mode value                  */
#define MS_MIN_DUALMONO         0     /*!< Minimum dual-mono mode                           */
#define MS_MAX_DUALMONO         2     /*!< Minimum dula-mono mode                           */
#define MS_MIN_DRC_BOOST        0     /*!< Minimum DRC boost value                          */
#define MS_MAX_DRC_BOOST      100     /*!< Maximum DRC boost value                          */
#define MS_MIN_DRC_CUT          0     /*!< Minimum DRC cut value                            */
#define MS_MAX_DRC_CUT        100     /*!< Maximum DRC cut value                            */
#define MS_MIN_DMX              0     /*!< Minimum stereo downmix value                     */
#define MS_MAX_DMX              2     /*!< Minimum stereo downmix value                     */
#define MS_MIN_DRC_MODE         0     /*!< Minimum DRC mode                                 */
#define MS_MAX_DRC_MODE         1     /*!< Maximum DRC mode                                 */
#define MS_MIN_ASSOC_MIX        0     /*!< Minimum associated mixing mode                   */
#define MS_MAX_ASSOC_MIX        1     /*!< Maximum associated mixing mode                   */
#define MS_MIN_USERBAL        -32     /*!< Minimum user balance value                       */
#define MS_MAX_USERBAL         32     /*!< Maximum user balance value                       */
#define MS_MIN_ASSOC_SUBSTRM    1     /*!< Minimum Associated program substream value       */
#define MS_MAX_ASSOC_SUBSTRM    3     /*!< Maximum Associated program substream value       */

#define MS_DMX_TYPE_LTRT       (0)
#define MS_DMX_TYPE_LORO       (1)
#define MS_DMX_TYPE_ARIB       (2)

//extern const char* acmod2str[];
//extern const char* dsurmod2str[];
//extern const char* instance2str[];

/*! This struct holds all parameters that may change during runtime without re-initialization */
struct MS_RUNTIME_PARAMS{

  /* Parameters common to ddplus, external pcm and pulse */
  int16_t drc_cut_fac_6ch;          /*!< Determines the percentage of application of DRC
                                        attenuation factors for the 6 channel output.                */
  int16_t drc_boost_fac_6ch;        /*!< Determines the percentage of application of DRC
                                        boost factors for the 6 channel output.                      */
  int16_t drc_cut_fac_2ch;          /*!< Determines the percentage of application of DRC
                                        attenuation factors for the 2 channel output.                */
  int16_t drc_boost_fac_2ch;        /*!< Determines the percentage of application of DRC
                                        boost factors for the 2 channel output.                      */
  int16_t downmix_type;             /*!< Indicates which downmix type is used (LtRt,LoRo,ARIB)       */
  int16_t drc_mode;                 /*!< Indicates whether to operate in RF or Line mode             */
  int16_t dual_mono;                /*!< Dual Mono reproduction mode, (both, left, right)            */
  int16_t cmix_lev;                 /*!< Index in center mix level table (-3dB,-4.5dB,-6dB)          */
  int16_t smix_lev;                 /*!< Index in surround mix level table (-3dB,-4.5dB,-6dB)        */
  int16_t multichannel_enable;      /*!< Indicates if the signal in the multichannel output is valid */
  int16_t associated_audio_mixing;  /*!< Enables/Disables mixing in dual decoding use cases          */
  int16_t user_balance_adjustment;  /*!< User defined balance between main and associated signal.
                                         Value range is between -32 and +32 (in dB),
                                         -32dB indicates main only (mute associated)
                                         +32dB indicates associated only (mute main)                 */

  /* External PCM parameters */
  int                       extpcm_num_in_samples;     /*!< Number of input samples to process. */
  TRANSCODER_CHANNEL_CONFIG extpcm_in_channel_config;  /*!< Input channel configuration */

  /* Dolby Pulse parameters */
  int16_t dpulse_mixing_mode;          /*!< Blocking or non-blocking mixing, i.e. does main wait for associated */
  int16_t dpulse_default_dialnorm;     /*!< Default dialnorm (dB/4) that is used until a valid dialnorm is found in the bitstream */
  int16_t dpulse_transport_format;     /*!< Specifies the Dolby Pulse transport format
                                           (can increase robustness compared to auto-detect   */

  /* DDPlus parameters */
  int16_t ddplus_associated_substream; /*!< ID of the substream which should be mixed with the main program */
  int16_t ddplus_outlfe;               /*!< Determines if LFE will be present in the DD+ decoded signal  */
  int16_t ddplus_outmode;              /*!< Determines which acmod will be set in the DD+ decoded signal */

  /* parameter update status variables */
  int        b_init;                   /*!< Indicates the first time variables are set */
  int32_t    frame;                    /*!< Current Frame number (necessary to apply runtime changes) */
  int        b_skip_parse;             /*!< Determines if next line in the command text file should be parsed or not */
  int32_t    skip_update_frame;        /*!< Stores the next update frame in certain scenarios */
  int32_t    next_update_frame;        /*!< Indicates the next frame when parameters shall be updated */
};

/*! This struct holds all parameters that are set at initialization time */
struct MS_INIT_PARAMS{
  /* Input and output filenames */
  const char *input_filename[2];         /*!< Names of the input file(s) - main and associated      */
  const char *pcm_output_filename[2];    /*!< Names of the PCM output files - main and associated   */
  const char *dd_output_filename;        /*!< Name of the DD bitstream output file.                 */
  int         pcm_out_chans[2];          /*!< Number of channels of the 2 PCM output files          */

  /* IDK test mode options */
  int16_t idk_test_mode_enable;             /*!< Activates IDK test modes, 1 decoder output, 2 encoder testmode */
  const char *idk_decoder_output_filename;  /*!< Filename of the decoder output (valid in IDK testmode 1 only) */
  const char *idk_encoder_input_filename;   /*!< Filename of the encoder testmode input files (.pki) (valid in IDK testmode 2 only) */
  const char *idk_encoder_output_filename;  /*!< Filename of the encoder testmode output files (.pko) (valid in IDK testmode 2 only) */

  int16_t verbose_mode;                     /*!< Determines the amout of information that is written to the command line */

#ifdef DEBUG
  /* DDPlus parameters */
  int16_t  ddplus_debug_subroutine;         /*!< Flag that activates the debug output for subroutines */
  int16_t  ddplus_debug_associated_audio;   /*!< Flag that activates the debug output for associated audio */
  int16_t  ddplus_debug_main_audio;         /*!< Flag that activates the debug output for main audio */
  uint32_t ddplus_debug_frame_level;        /*!< Bitmask determining what debug information at frame level will be generated */
  uint32_t ddplus_debug_decode_level;       /*!< Bitmask determining what debug information at decode level will be generated */
#endif

  /* PCM parameters */
  int16_t extpcm_compressor_profile;        /*!< The compressor profile for external PCM reencoding */
  int16_t extpcm_program_ref_level;         /*!< Program Reference Level of the external input to be reencoded */

  /* Dolby Pulse parameters */
  uint16_t dpulse_allow_partial_feed;  /*!< Flag enabling feeding of less input data than necessary to complete an output frame */
};


/*! This struct contains all multistream decoder parameters separated into init time and runtime settable parameters */
struct MS_PARAMS{
  MS_RUNTIME_PARAMS   ms_runtime_args; /*!< Structure containing all runtime dependant arguments */
  MS_INIT_PARAMS      ms_init_args;    /*!< Structure containing all initialization dependant arguments */
};

#endif /* MS_USER_H_ */
