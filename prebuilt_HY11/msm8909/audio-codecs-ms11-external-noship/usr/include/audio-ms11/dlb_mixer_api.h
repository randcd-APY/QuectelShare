
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
*/
#ifndef DOLBY_MIXER_API_H
#define DOLBY_MIXER_API_H
/******************************************************/
/*                    MS_MIXER                        */
/******************************************************/
int heaac_mix_md2gains(HEAAC_MIX_MD      *p_ddp_mmd,           /*!< \in: HEAAC (on PES level) related mixing metadata */
                       MS_CHANNEL_CONFIG *p_channel_cfg_main,  /*!< \in: Channel configuration of the main audio program */
                       MS_CHANNEL_CONFIG *p_channel_cfg_assoc, /*!< \in: Channel configuration of the associated audio program */
                       MIXER_GAINS       *p_mixer_gains        /*!< \out: Generalized Mixing Metadata as required by ms_mixer_process() */
                       );



int ms_mixer_process(MS_MIXER_HANDLE h_mixer_state,  /*!< \in and \out:Static memory of an MS Mixer instance */
                     MIXER_PCM      *p_main_audio,   /*!< \in: Main audio PCM signal related information */
                     MIXER_PCM      *p_assoc_audio,  /*!< \in: Associated audio PCM signal related information */
                     MIXER_GAINS    *p_mixer_gains,  /*!< \in: Generalized (codec independent) Mixing Metadata */
                     int16_t         n_samples,      /*!< \in: Number of samples to process per channel */
                     int16_t         user_pref,      /*!< \in: User balance adjustment for mixing main and associated audio.
                                                          -32: associated fully muted\n
                                                          -1..-31: dB to favor main program (attenuate associate)\n
                                                          0: neutral (no balance adjustment)\n
                                                          1..31: dB to favor associated program (attenuate main)\n
                                                          32: main fully muted\n */
                     int             operation_mode, /*!< \in: determines how interpolation between consecutive gains is done
                                                          0: regular op mode: interpolate between old and new gain
                                                          1: continue: continue interpolation from last call
                                                          2: reset: don't interpolate at all, apply new gain directly */
                     MIXER_PCM      *p_mixed_audio   /*!< [in,out] Mixed audio PCM signal related information,
                                                          buffer may be overlap with the main audio buffer */
                     );


int ms_mixer_query_mem(uint32_t *intern_static_size  /*!< \out: Required static memory in bytes */
                       );


MS_MIXER_HANDLE ms_mixer_open(uint32_t *intern_static_mem /*!< [in, out] Points to the memory where the mixer instance will be mapped onto */
                              );

int ddp_mix_md2gains(DDP_MIX_MD        *p_ddp_mmd,           /*!< \in: Dolby Digital Plus related mixing metadata */
                     MS_CHANNEL_CONFIG *p_channel_cfg_main,  /*!< \in: Channel configuration of the main audio program */
                     MS_CHANNEL_CONFIG *p_channel_cfg_assoc, /*!< \in: Channel configuration of the associated audio program */
                     int                b_main_downmixed,    /*!< \in: Indicates that the main program has been downmixed in the decoder */
                     MIXER_GAINS       *p_mixer_gains        /*!< \out: Generalized Mixing Metadata as required by ms_mixer_process() */
                     );

#endif
