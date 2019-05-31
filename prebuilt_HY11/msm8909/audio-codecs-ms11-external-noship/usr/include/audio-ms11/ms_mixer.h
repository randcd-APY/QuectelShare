/*
 *        Confidential Information - Limited distribution to authorized persons
 *        only. This material is protected under international copyright laws as
 *        an unpublished work. Do not copy.
 *        Copyright (C) 2011 Dolby Laboratories Inc.
 *        Copyright (C) 2011 Dolby International AB
 *        All rights reserved.
 */

/**
 * \defgroup mixer Multistream Mixer (Optional)
 * \ingroup msd
 *
 * Defines the interface of the Multistream Mixer, which is capable of mixing a
 * main and associated audio program applying the gains derived from the mixing
 * metadata.
 *
 * There are two functions to convert the codec specific mixing metadata defined
 * by the corresponding broadcast standard into a generalized mixing metadata format.
 * The ms_mixer_process() function takes the generalized mixing metadata and does not
 * need to know from which codec the input signal comes from.
 *
 * The typical calling sequence is as follows:
 * \li Call ms_mixer_query_mem() to query the required memory of one instance
 * \li allocate required memory
 * \li Call ms_mixer_open() to initialize the mixer instance and get a handle of the instance
 * \li Convert mixing metadata via ddp_mix_md2gains() or heaac_mix_md2gains() depending on the input format
 * \li Call ms_mixer_process() to mix a main and associated audio program using the supplied mixing metadata
 *
 * @{
 */


/*! \file ms_mixer.h
    \brief Multistream Mixer header file.
 */

#ifndef MS_MIXER_H_
#define MS_MIXER_H_

/** \brief ms_mixer instance handle */
typedef struct MS_MIXER_INSTANCE *MS_MIXER_HANDLE;

/* Define the possible return values for the MS Mixer functions */
#define MS_MIXER_OK                0  /*!< Defines the return value if no error was encountered */
#define MS_MIXER_ERR_INV_CH_MODES -1  /*!< Invalid channel mode combination specified */
#define MS_MIXER_ERR_MIX_MD       -2  /*!< Mixing Metadata is out of the specified range */
#define MS_MIXER_ERR_USER_PREF    -3  /*!< User Preference Balance is out of range */

#define MSMIX_MAX_NUM_CHANNELS     6  /*!< Maximum numner of channels to mix */
#define MSMIX_MAX_NUM_CH_CFGS      8  /*!< Maximum number of channel modes */

#define MSMIX_REGULAR             (0) /*!< Regular operation mode, to be set at the beginning of a new frame */
#define MSMIX_CONTINUE            (1) /*!< Continue interpolation started earlier */
#define MSMIX_RESET               (2) /*!< No interpolation will be performed */


extern const int16_t msmix_chanary[MSMIX_MAX_NUM_CH_CFGS];
extern const int16_t msmix_chantab[MSMIX_MAX_NUM_CH_CFGS][MSMIX_MAX_NUM_CHANNELS];

typedef struct MS_CHANNEL_CONFIG {
  uint16_t acmod; /*!< Audio coding mode as defined for Dolby Digital. */
  int      b_lfe; /*!< Flag indicating presence of the low frequency enhancement channel. */
} MS_CHANNEL_CONFIG;


/**
 * \brief This struct contains a generalized (codec independent) format of mixing metadata
 */
typedef struct MIXER_GAINS{
  DLB_SFRACT p_scl_db[MSMIX_MAX_NUM_CHANNELS];    /*!< Gains per channel (applied on the main audio signal)
                                         The gains are given in a fractional dB representation with 8 bits
                                         before and 7 bits after the decimal point (Q8.7 format) */
  DLB_SFRACT mainscl_db;            /*!< Overall gain of the main audio channels
                                         The gain is given in a fractional dB representation with 8 bits
                                         before and 7 bits after the decimal point (Q8.7 format) */

  DLB_SFRACT a_pangain[MSMIX_MAX_NUM_CHANNELS];   /*!< Linear fractional gains (in Q1.14 format) per channel,
                                                       which are applied to the associated center channel in case of panning */
} MIXER_GAINS;


typedef struct MIXER_PCM{
  DLB_LFRACT       *pp_pcm[MSMIX_MAX_NUM_CHANNELS]; /*!< Array of pointer to the PCM channels */
  int16_t           sample_offset;                  /*!< Sample offset between consecutive samples in one PCM channel */
  MS_CHANNEL_CONFIG channel_config;                 /*!< Channel configuration of the PCM signal */
} MIXER_PCM;

/**
 * \brief This struct contains the relevant mixing metadata for Dolby Digital Plus
 *
 * The metadata is defined in ETSI TS 102366 "Digital Audio Compression (AC-3, Enhanced AC-3) Standard".
 * If one of the parameter does not exist is should be set to a neutral value which has no effect on
 * the output mixed signal or if not possible to -1 which indicates that it is undefined.
 */
typedef struct DDP_MIX_MD{
int16_t extpgmscl;    /*!< External program scale factor (6 bits). Valid values are 0 to 63, with 0 interpreted as mute,
                           and 1-63 interpreted as -50 dB to +12 dB of scaling in 1 dB steps  */
int16_t extpgmlscl;   /*!< External program left scale factor  (4 bits)\n
                           0:   -1dB\n
                           1:   -2dB\n
                           2:   -3dB\n
                           3:   -4dB\n
                           4:   -5dB\n
                           5:   -6dB\n
                           6:   -8dB\n
                           7:  -10dB\n
                           8:  -12dB\n
                           9:  -14dB\n
                           10: -16dB\n
                           11: -19dB\n
                           12: -22dB\n
                           13: -25dB\n
                           14: -28dB\n
                           15: -infinity (mute) */
int16_t extpgmcscl;   /*!< External program center scale factor  (4 bits, interpretation see extpgmlscl) */
int16_t extpgmrscl;   /*!< External program right scale factor  (4 bits, interpretation see extpgmlscl) */
int16_t extpgmlsscl;  /*!< External program left surround scale factor  (4 bits, interpretation see extpgmlscl) */
int16_t extpgmrsscl;  /*!< External program right surround scale factor  (4 bits, interpretation see extpgmlscl) */
int16_t extpgmlfescl; /*!< External program LFE scale factor  (4 bits, interpretation see extpgmlscl) */
int16_t dmixscl;      /*!< Downmix scale factor  (4 bits, interpretation see extpgmlscl) */
int16_t panmean;      /*!< Pan mean direction index - 8 bits
                           This 8-bit word defines the mean angle of rotation index relative to the center position for a panned source in a two
                           dimensional sound field. A value of 0 indicates the panned virtual source points toward the center speaker location
                           (defined as 0 degrees). The index indicates 1,5 degree increments in a clockwise rotation. Values 0 to 239 represent
                           0 to 358,5 degrees, while values 240 to 255 are reserved. */
} DDP_MIX_MD;

/**
 * \brief This struct contains the relevant mixing metadata for HE-AAC
 *
 * The following definition of values is from "ETSI TS 101 154 - Digital Video Broadcasting
 * (DVB); Implementation guidelines for the use of Video and Audio Coding in Broadcasting
 * Applications based on the MPEG-2 Transport Stream, v1.9.1", chapter "E.2 Syntax and semantics".
 *
 * fade_byte: Takes values between 0x00 (representing no fade of the main program
 * sound) and 0xFF (representing a full fade). Over the range 0x00 to 0xFE one lsb
 * represents a step in attenuation of the program sound of 0,3 dB giving a range
 * of 76,2 dB. The fade value of 0xFF represents no program sound at all (i.e. mute).
 * The rate of signaling and the expected behaviour of a decoder to changes in fade byte
 * are described below.
 *
 * e.g. fade byte:\n
 * 0x00:   0,0 dB\n
 * 0x01:  -0,3 dB\n
 * 0xFE: -76,2 dB\n
 * 0xFF:   Mute\n
 *
 * pan_byte: Takes values between 0x00 representing a central forward presentation
 * of the audio description and 0xFF, each increment representing a 360/256 degree
 * step clockwise looking down on the listener (i.e. just over 1,4 degrees, see
 * figure E.2). The rate of signaling and the expected behaviour of a decoder are
 * described below.
 *
 * gain_byte_center: Represents a signed value in dB. Takes values between 0x7F
 * (representing +76,2 dB boost of the main program center) and 0x80 (representing
 * a full fade). Over the range 0x00 to 0x7F one lsb represents a step in boost of
 * the program center of 0,6 dB giving a maximum boost of +76,2 dB. Over the range
 * 0x81 to 0x00 one lsb represents a step in attenuation of the program centre of
 * 0,6 dB giving a maximum attenuation of -76,2 dB. The gain value of 0x80 represents
 * no main centre level at all (i.e. mute). The rate of signaling and the expected
 * behaviour of a decoder to changes in gain byte are described below.
 *
 * gain_byte_front: As gain_byte_center, applied to left and right front channel.
 *
 * gain_byte_surround: As gain_byte_center, applied to all surround channels.
 *
 * e.g. gain byte:\n
 * 0xFF:  -0,6 dB\n
 * 0x81: -76,2 dB\n
 * 0x80:   Mute\n
 * 0x7F: +76,2 dB\n
 * 0x00:   0,0 dB\n
 */
typedef struct HEAAC_MIX_MD{
  uint16_t  fade_byte;
  int16_t   gain_byte_center;
  int16_t   gain_byte_front;
  int16_t   gain_byte_surround;
  uint16_t  pan_byte;
} HEAAC_MIX_MD;


/**
 * \brief Returns the amount of static memory in bytes needed for an instance of the multistream mixer.
 * \returns MS_MIXER_OK on success
 */
//int ms_mixer_query_mem(uint32_t *intern_static_size  /*!< \out: Required static memory in bytes */
//                       );



/**
 * \brief This functions initializes an instance of the multistream mixer.
 *
 * The provided memory pointer needs to be as large as returned by ms_mixer_query_mem().
 *
 * \returns Pointer to an initialized instance handle of the mixer, null pointer in case something went wrong
 */
//MS_MIXER_HANDLE ms_mixer_open(uint32_t *intern_static_mem /*!< [in, out] Points to the memory where the mixer instance will be mapped onto */
//                              );




/**
 * \brief This function mixes a main and associated audio program using the supplied mixing metadata.
 *
 * This function takes n_samples of up to 6 channels of both programs, applies the corresponding
 * gain factors calculated from the mixing metadata and adds them together. The buffer of the mixed
 * signal might be overlayed with the buffer of the main audio program.
 *
 * Only channel mode combinations where every associated channel has a corresponding match in the
 * main program will be accepted for mixing. If an associated LFE channel is present, it is only mixed
 * if a main LFE channel is also present.
 *
 * The gains are adjusted in a way that the dialogue level of the mixed signals are preserved. Besides
 * the mixing metadata a user preference value can be set, which balances the level between the main
 * and the associated audio.
 *
 * Mixing metadata can vary from call to call. In order to avoid discontinuity in
 * the signal, the effective gain values (the factors applied on the signals)
 * transitions smoothly from an old value to a new value by applying an interpolation on the
 * gain over a period of 256 samples.
 *
 * The operation mode determines how the gains of consecutive calls are beeing handled. In regular operation
 * mode (MSMIX_REGULAR) consecutive gains are beeing interpolated. Operation mode MSMIX_RESET indicates that
 * no interpolation should happen (e.g. startup phase). The operation mode MSMIX_CONTINUE is used when the
 * number of samples is less than the interpolation length. This can happen if the decoder frame size and
 * the number of samples to mix do not match. Dolby Pulse dual decoding at 44.1kHz is an example of such a
 * case.
 *
 * \returns MS_MIXER_OK if successful.
 */
//int ms_mixer_process(MS_MIXER_HANDLE h_mixer_state,  /*!< \in and \out:Static memory of an MS Mixer instance */
//                     MIXER_PCM      *p_main_audio,   /*!< \in: Main audio PCM signal related information */
//                     MIXER_PCM      *p_assoc_audio,  /*!< \in: Associated audio PCM signal related information */
//                     MIXER_GAINS    *p_mixer_gains,  /*!< \in: Generalized (codec independent) Mixing Metadata */
//                     int16_t         n_samples,      /*!< \in: Number of samples to process per channel */
//                     int16_t         user_pref,      /*!< \in: User balance adjustment for mixing main and associated audio.
//                                                          -32: associated fully muted\n
//                                                          -1..-31: dB to favor main program (attenuate associate)\n
//                                                         0: neutral (no balance adjustment)\n
//                                                          1..31: dB to favor associated program (attenuate main)\n
//                                                          32: main fully muted\n */
//                     int             operation_mode, /*!< \in: determines how interpolation between consecutive gains is done
//                                                          0: regular op mode: interpolate between old and new gain
//                                                          1: continue: continue interpolation from last call
//                                                          2: reset: don't interpolate at all, apply new gain directly */
//                     MIXER_PCM      *p_mixed_audio   /*!< [in,out] Mixed audio PCM signal related information,
//                                                          buffer may be overlap with the main audio buffer */
//                     );
/**
 * \brief Converts the relevant mixing metadata from the DD+ bitstream format into a generalized codec independant format
 *
 * \returns MS_MIXER_OK if successful.
 */
//int ddp_mix_md2gains(DDP_MIX_MD        *p_ddp_mmd,           /*!< \in: Dolby Digital Plus related mixing metadata */
//                     MS_CHANNEL_CONFIG *p_channel_cfg_main,  /*!< \in: Channel configuration of the main audio program */
//                     MS_CHANNEL_CONFIG *p_channel_cfg_assoc, /*!< \in: Channel configuration of the associated audio program */
//                     int                b_main_downmixed,    /*!< \in: Indicates that the main program has been downmixed in the decoder */
//                     MIXER_GAINS       *p_mixer_gains        /*!< \out: Generalized Mixing Metadata as required by ms_mixer_process() */
//                     );
/**
 * \brief Converts the relevant mixing metadata from the HE-AAC bitstream format into a generalized codec independent format
 *
 * \returns MS_MIXER_OK if successful.
 */
//int heaac_mix_md2gains(HEAAC_MIX_MD      *p_ddp_mmd,           /*!< \in: HEAAC (on PES level) related mixing metadata */
//                       MS_CHANNEL_CONFIG *p_channel_cfg_main,  /*!< \in: Channel configuration of the main audio program */
//                       MS_CHANNEL_CONFIG *p_channel_cfg_assoc, /*!< \in: Channel configuration of the associated audio program */
//                       MIXER_GAINS       *p_mixer_gains        /*!< \out: Generalized Mixing Metadata as required by ms_mixer_process() */
//                       );


#endif /* MS_MIXER_H_ */
/*@}*/
