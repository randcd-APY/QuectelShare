
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
/******************************************************/
/*                    DDT                             */
/******************************************************/
int16_t
(*dlDolbyPulseDecoderTranscoderGetStreamInfo)(HANDLE_DOLBY_PULSE_DECODER_TRANSCODER hTranscoder, /*!< IN:  Instance handle obtained by DolbyPulseDecoderTranscoderOpen() */
					 TRANSCODER_STREAMINFO *tStreamInfo                 /*!< OUT: Stream info structure that is filled by this function. */
					 );


int16_t
(*dlDolbyPulseDecoderTranscoderGetDelay)(HANDLE_DOLBY_PULSE_DECODER_TRANSCODER hTranscoder, /*!< IN:  Instance handle obtained by DolbyPulseDecoderTranscoderOpen() */
				    TRANSCODER_DELAYINFO                  *pDelayInfo  /*!< OUT: Delay information is available in this struct on successful return */
                                    );



int16_t
(*dlDolbyPulseDecoderTranscoderGetPcmFrame)(HANDLE_DOLBY_PULSE_DECODER_TRANSCODER hDecoder,
                                                /*!< IN:  Instance handle of associated audio decoder obtained by DolbyPulseDecoderTranscoderOpen() */
                                                DLB_SFRACT ** apPCM,     /*!< OUT: Pointers to output channels. The channel layout is the same as describe in MIXER_CALLBACK. */
                                                uint16_t frameLen,      /*!< IN:  Length of requested frame in samples per channel */
                                                TRANSCODER_CHANNEL_CONFIG * pChannelConfig
                                                /*!< OUT: Channel configuration of the supplied audio data, consisting of acmod and a flag for the LFE channel.
                                                  Valid acmod values for mixing: are DD_MONO=1, DD_STEREO=2, and DD_3_2=7. */
                                                );


int32_t
(*dlDolbyPulseDecoderTranscoderGetRequiredBufferSize)(uint32_t *pInternStaticSize, /*!< OUT: Size of internal static memory in bytes  */
						 uint32_t *pInternDynamicSize,/*!< OUT: Size of internal dynamic memory in bytes */
						 uint32_t *pExternStaticSize, /*!< OUT: Size of external static memory in bytes */
						 uint16_t  operationMode      /*!< IN:  Determines the operation mode of an instance
                                                                                  (see \ref DLB_DDT_OP_MODE_VAL) */
                                                 ,uint16_t  bMS10Mode         /*!< IN:  Flag indicating MS10 mode is used */
						 );


HANDLE_DOLBY_PULSE_DECODER_TRANSCODER
(*dlDolbyPulseDecoderTranscoderOpen)(uint32_t * pInternStatic,    /*!< IN: Pointer to static internal memory */
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


int16_t
(*dlDolbyPulseDecoderTranscoderConfigure)(HANDLE_DOLBY_PULSE_DECODER_TRANSCODER hTranscoder, /*!< IN:   Instance handle obtained by DolbyPulseDecoderTranscoderOpen() */
                                     unsigned char *ascData,  /*!< IN:   Pointer to audio specific config */
                                     int ascLen  /*!< IN:   Length of audio specific config in bytes*/
                                    );


int16_t
(*dlDolbyPulseDecoderTranscoderProcess)(HANDLE_DOLBY_PULSE_DECODER_TRANSCODER hTranscoder, /*!< IN:   Instance handle obtained by DolbyPulseDecoderTranscoderOpen() */
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


int16_t
(*dlDolbyPulseDecoderTranscoderSetProcessParam)(HANDLE_DOLBY_PULSE_DECODER_TRANSCODER hTranscoder,   /*!< IN: Instance handle obtained by DolbyPulseDecoderTranscoderOpen() */
					   const DOLBY_PULSE_DECODER_TRANSCODER_CTRL_ID paramID, /*!< IN: ID of parameter to modify */
					   const void * pParamVal                               /*!< IN: New value of parameter to modify */
					   );


int16_t
(*dlDolbyPulseDecoderTranscoderInstallDecInfoCallback)(HANDLE_DOLBY_PULSE_DECODER_TRANSCODER hTranscoder, /*!< IN:  Instance handle obtained by DolbyPulseDecoderTranscoderOpen() */
                                                  DECINFO_CALLBACK   pDecInfoCallbackFunc, /*!< IN: Pointer to user defined function. Passing a NULL pointer disables the callback. */
                                                  void             * pDecInfoCallbackArg   /*!< IN: Pointer argument that will be passed on to the callback function */
                                                  );


int16_t
(*dlDolbyPulseDecoderTranscoderInstallMixerCallback)(HANDLE_DOLBY_PULSE_DECODER_TRANSCODER hTranscoder,
                                                /*!< IN:  Instance handle obtained by DolbyPulseDecoderTranscoderOpen() */
                                                MIXER_CALLBACK   pMixerCallbackFunc, /*!< IN: Pointer to user defined function. Passing a NULL pointer is not allowed; DolbyPulseDecoderTranscoderSetProcessParam() can be used to disable the mixer callback again. */
                                                void           * pMixerCallbackArg   /*!< IN: Pointer argument that will be passed on to the callback function */
                                                );


int16_t
(*dlDolbyPulseDecoderTranscoderInstallDolbyVolumeCallback)(HANDLE_DOLBY_PULSE_DECODER_TRANSCODER hTranscoder, /*!< IN: Instance handle obtained by DolbyPulseDecoderTranscoderOpen() */
                                                      DOLBY_VOLUME_CALLBACK  pDolbyVolumeCallbackFunc,   /*!< IN: Pointer to user defined function. Passing a NULL pointer disables the callback. */
                                                      void           * pDolbyVolumeCallbackArg           /*!< IN: Pointer argument that will be passed on to the callback function */
                                                      );


int16_t
(*dlDolbyPulseDecoderTranscoderGetMaxDelay)(void);


int16_t
(*dlDolbyPulseDecoderTranscoderGetStartupDelay)(void);

/******************************************************/
/*                    DDRE                            */
/******************************************************/
HANDLE_DOLBY_DIGITAL_REENCODER
(*dlDolbyDigitalReencoderOpen)(
  uint32_t *pInternStatic,  /*!< Pointer to internal static memory */
  uint32_t *pInternDynamic, /*!< Pointer to internal dynamic memory */
  uint32_t *pExternStatic,  /*!< Pointer to external static memory */
  uint16_t  bExternalPcm,   /*!< Flag to signal 'external PCM mode' */
  uint16_t  bDv258,         /*!< Flag indicates that input has been DV-258 processed */
  uint32_t  sampleRate,     /*!< sample rate of incoming PCM */
  uint16_t  compProfile,    /*!< compression profile, only valid for external PCM */
  uint16_t  dialNorm        /*!< dialog level of incoming PCM */
);

int32_t
(*dlDolbyDigitalReencoderGetRequiredBufferSize)(
  uint32_t *pInternStaticSize,  /*!< Pointer to internal static memory */
  uint32_t *pInternDynamicSize, /*!< Pointer to internal dynamic memory */
  uint32_t *pExternStaticSize   /*!< Pointer to external static memory */
);



int32_t
(*dlDolbyDigitalReencoderProcess)(
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


int16_t
(*dlDolbyDigitalReencoderSetProcessParam)(
        HANDLE_DOLBY_DIGITAL_REENCODER hReenc,   /*!< IN: Instance handle obtained by DolbyDigitalReencoderOpen() */
  const DOLBY_DIGITAL_REENCODER_CTRL_ID paramID, /*!< IN: ID of parameter to modify */
                          const void * pParamVal /*!< IN: New value of parameter to modify */
);


uint16_t
(*dlDolbyDigitalReencoderFifoEnoughSamples)(
  HANDLE_DOLBY_DIGITAL_REENCODER hReenc  /*!< Handle of the Dolby Digital reencoder instance */
);

int16_t
(*dlDolbyDigitalReencoderGetDelay)(HANDLE_DOLBY_DIGITAL_REENCODER hReenc, /*!< IN:  Instance handle obtained by DolbyDigitalReencoderOpen() */
                              int16_t    *pStaticDelay,             /*!< OUT: Constant latency depending on the current mode of operation */
                              int16_t    *pDynamicDelay             /*!< OUT: Dynamically changing amount of decoded audio data in the internal reframing buffer */
                              );

/******************************************************/
/*                    DDC                             */
/******************************************************/
DSPerr (*dlddpi_ddc_query)(
        DDPI_DDC_QUERY_OP           *p_outparams        /*!< \out: Query output parameters */
);

DSPerr (*dlddpi_ddc_open)(
        void                        *p_ddchdl,          /*!< \mod: Pointer to subroutine memory */
        const DDPI_DDC_INPUTMODE    inputmode           /*!< \in: Indicates DDC input mode */
);

DSPerr (*dlddpi_ddc_setprocessparam)(
	void                        *p_ddchdl,          /*!< \mod: Pointer to subroutine memory */
	const DDPI_DDC_CTL_ID       paramid,            /*!< \in: Control parameter identification */
	const void                  *p_paramval         /*!< \in: Pointer to parameter value */
);


DSPerr (*dlddpi_ddc_getprocessparam)(
	const void                  *p_ddchdl,          /*!< \mod: Pointer to subroutine memory */
	const DDPI_DDC_CTL_ID       paramid,            /*!< \in: Control parameter identification */
	void                        *p_paramval         /*!< \out: Pointer to parameter value */
);


DSPerr (*dlddpi_ddc_getddpframesize)(
	BFD_BUFDESC_DDPFRM          *p_ddinbfd,         /*!< \in: Buffer descriptor containing partial frame */
	int16_t                    *p_nwords           /*!< \out: Number of 16-bit words in the complete frame */
);


DSPerr (*dlddpi_ddc_getddpsamplerate)(
	BFD_BUFDESC_DDPFRM          *p_ddinbfd,         /*!< \in: Buffer descriptor containing partial frame */
	int32_t                     *p_samplerate       /*!< \out: Sample rate of the frame */
);


DSPerr (*dlddpi_ddc_addframe)(
	void                        *p_ddchdl,          /*!< \in: Pointer to subroutine memory */
	const DDPI_DDC_AF_IP        *p_inparams,        /*!< \in: Add frame input parameters */
	DDPI_DDC_AF_OP              *p_outparams        /*!< \out: Add frame output parameters */
);


DSPerr (*dlddpi_ddc_processtimeslice)(
	void                        *p_ddchdl,          /*!< \in: Pointer to subroutine memory */
	DDPI_DDC_PT_OP              *p_outparams        /*!< \out: Process timeslice output parameters */
);


DSPerr (*dlddpi_ddc_close)(
	void                        *p_ddchdl   /*!< \mod: Pointer to subroutine memory */
);


/******************************************************/
/*                    BFD                             */
/******************************************************/
DLB_API_IMPORT
DSPerr (*dlbfd_clearddpfrmbuf)(
	BFD_BUFDESC_DDPFRM			*p_ddpfrmbfd		/*!< \mod: Pointer to an initialized DDP frame buffer */
	);


DLB_API_IMPORT
DSPerr (*dlbfd_clearpcmchbuf)(
	BFD_BUFDESC_PCMCH			*p_pcmchbfd		/*!< \mod: Pointer to an initialized PCM channel buffer	*/
	);
