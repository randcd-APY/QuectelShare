/* Copyright (c) 2009-2012 by Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef SOFTMS11_H_
#define SOFTMS11_H_

#include <errno.h>
#include <utils/List.h>

#include "ms_intrinsics.h"
#include "gbl.h"
#include "gbl_type.h"
#include "err.h"
#include "bfd.h"

#include "dolbypulsedecodertranscoder.h"
#include "ddc_api.h"
#include "ddt_ddre_common.h"
#include "ddenc.h"
#include "dolbydigitalreencoder.h"


#include "ms_user.h"
#include "ms_exec.h"
#include "ms_err.h"
#include "ms_fio.h"
#include "ms_mixer.h"
#include "callback.h"
#include "mixcore.h"
#include "callback_258.h"

#define DRC_CUT_FAC_6CH                 0
#define DRC_BOOST_FAC_6CH               1
#define DRC_CUT_FAC_2CH                 2
#define DRC_BOOST_FAC_2CH               3
#define DOWNMIX_TYPE                    4
#define DUAL_MONO                       5
#define MULTICHANNEL_ENABLE             6
#define ASSOCIATED_AUDIO_MIXING         7
#define USER_BALANCE_ADJUSTMENT         8
#define DDPLUS_ASSOCIATED_SUBSTREAM     9


#define FORMAT_DOLBY_DIGITAL_PLUS_MAIN        0
#define FORMAT_DOLBY_DIGITAL_PLUS_ASSOC       1
#define FORMAT_DOLBY_DIGITAL_PLUS_MAIN_ASSOC  2
#define FORMAT_DOLBY_PULSE_MAIN               3
#define FORMAT_DOLBY_PULSE_ASSOC              4
#define FORMAT_EXTERNAL_PCM                   5

#define INPUT_TYPE_DOLBY_DIGITAL_PLUS         0
#define INPUT_TYPE_DOLBY_PULSE                1
#define INPUT_TYPE_EXTERNAL_PCM               2

#define PCM_BLOCK_SIZE_PER_CHANNEL_MS11       1536*2
#define STEREO_CHANNELS                       2
#define PCM_5POINT1_CHANNELS                  6

#define NO_ERROR                              0
#define ERROR_AD_CONFIG_PRIOR_TO_MAIN         -10001
#define ERROR_SAMP_RATE_AD_MISMATCH_MAIN      -10002
#define ERROR_SAMP_RATE_MISMATCH              -10003
#define ERROR_UNSUPPORTED_CHANNELS            -10004
#define ERROR_STREAM_OPEN                     -20000

#define PCM_2CH_OUT                           0
#define PCM_MCH_OUT                           1
#define SPDIF_OUT                             2

#define DDSYNCWORD                            (0x0B77)
#define DDSYNCWORDREV                         (0x770B)

namespace android_audio_legacy {

extern "C"
{
#include "dlb_mixer_api.h"

int32_t ticsPerSample(uint32_t sampleRate);
void    DecInfoCallback(TIME_INFO * pTimeInfo,
                        uint32_t start,
                        uint32_t stop,
                        TRANSCODER_STREAMINFO *pStreamInfo,
                        uint16_t transportFormat,
                        uint16_t errorflag);

int     MixWithAssociatedAudioCallback(HANDLE_MIXER  hMixer,
                                   DLB_SFRACT ** ppInOutPCM,
                                   uint16_t     inOutLen,
                                   TRANSCODER_CHANNEL_CONFIG * pChannelConfig);
int     Dv258AudioCallback(HANDLE_DV_258  hDv258,
                                  /*!< Callback data (e.g. instance handles) */
                           DLB_SFRACT ** ppInOutPCM,
                                  /*!< Pointer to an array of PCM buffers in cinema order */
                           uint16_t     inOutLen,
                                  /*!< Number of samples per channel */
                           TRANSCODER_CHANNEL_CONFIG * pChannelConfig);
                                  /*!< Current channel configuration */

}

class SoftMS11 {

public:
    SoftMS11();
    ~SoftMS11();

    //Resolves the symbols from the dolby library for API calls
    bool initializeMS11FunctionPointers();

    // Open the stream handle based on the usecase requested for
    // MS11 decoding
    MS_RETVAL  streamOpen();

    // Sets the run time parameters required for DDT/DDP Single/dual decoding
    MS_RETVAL  setParameters(int16_t param_id, int16_t val);

    // decode the stream and output sample rate, number of channels and
    // bytes consumed
    size_t     streamDecode(uint32_t *out_sample_rate, uint32_t *channels);

    // close the stream handle and free the input and output buffer allocated
    MS_RETVAL  streamClose();

    // sets the use case and open the stream based on the format
    int32_t    setUseCaseAndOpenStream(int , uint32_t , uint32_t );

    // sets the use case and open the stream based on the format
    int32_t    setUseCaseAndOpenStream(int , uint32_t , uint32_t, bool );

    // Copy the bitstream from HAL to MS11 input buffer. Typically,
    // this buffer would have a complete frame to decode
    void       copyBitstreamToMS11InpBuf(void *buffer, size_t bytes);

    // Copy the output samples from MS11 buffer to HAL based on the output
    // mode
    size_t     copyOutputFromMS11Buf(int outputMode, void *buffer);

    // sets the AAC config sent for DDT use case and flags if the config
    // is valid
    bool       setAACConfig(unsigned char *, size_t);

    void       flush();
private:
    void      ms_init_params(MS_PARAMS *);
    MS_RETVAL ms_query_memory(MS_USE_CASE *, MS_MEM_SIZES *);
    MS_RETVAL ms_allocate_memory(MS_MEM_SIZES *,  MS_MEM_PTR *);
    MS_RETVAL ms_free_memory(MS_MEM_PTR *);
    MS_RETVAL ms_open_components(MS_MEM_PTR *, MS_USE_CASE *,
                      MS_INIT_PARAMS *, MS_COMPONENTS_HANDLE *);
    MS_RETVAL ms_allocate_iobuffer(MS_USE_CASE *, MS_IOBUFFER *);
    MS_RETVAL ms_free_iobuffer(MS_IOBUFFER *);

    MS_RETVAL ms_handle_ddre_retval(int ,  const char *);
    MS_RETVAL ms_handle_ddc_retval(int , const char *);
    MS_RETVAL ms_handle_ddt_retval(int , int , const char *);
    int       ms_errorlookup(const MS_ERR *, int , const char **);
    MS_RETVAL ms_my_alloc(void **, uint32_t , uint16_t *);
    MS_RETVAL ms_set_all_process_params(MS_USE_CASE *, MS_RUNTIME_PARAMS *,
                                MS_COMPONENTS_HANDLE *);
    MS_RETVAL ms_set_ddre_process_params(MS_RUNTIME_PARAMS *,
                                 HANDLE_DOLBY_DIGITAL_REENCODER);
    MS_RETVAL ms_process(MS_USE_CASE *, MS_RUNTIME_PARAMS *,
                     MS_PROCESSING_STATE *, MS_COMPONENTS_HANDLE *,
                     MS_IOBUFFER *);
    void      ms_convert_pcm_L2S(MS_PROCESSING_STATE *, MS_IOBUFFER *);
    void      ms_pack_channels(MS_PROCESSING_STATE *);
    MS_RETVAL ms_process_ddre_after_ddc(MS_PROCESSING_STATE *,
                     MS_COMPONENTS_HANDLE *, MS_IOBUFFER *);
    MS_RETVAL ms_fill_input_buffer(MS_USE_CASE *p_use_case,
                               MS_COMPONENTS_HANDLE *p_comp_handles,
                               MS_PROCESSING_STATE *p_proc_state,
                               MS_IOBUFFER *p_iobuffer, int16_t *buffer, size_t bytes
                               );
    MS_RETVAL ms_read_input_frame(MS_USE_CASE *p_use_case, MS_IOBUFFER *p_iobuffer,
                            DDPI_DDC_AF_IP *p_af_ip, int16_t *buffer, size_t bytes,
                            int16_t *bytes_consumed_to_lock_buf);
    int16_t   ms_ddc_get_free_inbuf(
                     const LOCKINGDDPFRMBFD     p_ddpinbfds[DDPI_DDC_TOTALINBUFS],  /* input    */
                     const BFD_BUFDESC_DDPFRM **pp_ddbufdesc);              /* output   */
    int16_t   ms_ddc_read_frame(
                     int16_t      *buffer, /* modify  */ size_t bytes,
                     BFD_BUFDESC_DDPFRM *const p_ddpinbfd   /* modify  */,
                     int16_t *bytes_consumed_to_lock_buf);
    int16_t   ms_ddc_lock_inbuf(
	             LOCKINGDDPFRMBFD   p_ddpinbfds[DDPI_DDC_TOTALINBUFS], /* modify */
                     const BFD_BUFDESC_DDPFRM *const p_ddbufdesc);         /* input  */
    int16_t   ms_ddc_unlock_inbuf(
	             LOCKINGDDPFRMBFD          p_ddpinbfds[DDPI_DDC_TOTALINBUFS], /* modify */
                     const BFD_BUFDESC_DDPFRM *const p_ddbufdesc);                /* input  */
    size_t    writepcmout(int16_t *buffer, const uint16_t nblocks, int16_t channels, const BFD_BUFDESC_PCMCH p_pcmchbfds[]);
    int16_t   detectsyncword(int16_t *buffer, size_t bytes, bool *byterev, int16_t *byteskip);
    DSPerr    read_inp_buf_update_bfd(
                     int16_t	     *buffer,		/* modify	*/
                     const uint16_t  length,		/* input	*/
                     const uint16_t  offset,		/* input	*/
                     bool            byterev,           /* input        */
                     BFD_BUFDESC_DDPFRM	  *p_ddpfrmbuf);/* modify	*/
    DSPerr    performbyterev(const uint32_t length, int16_t *p_buffer);
    bool      validate_complete_timeslice(int16_t *buffer, int16_t bytes);
    int32_t   setUseCaseAndOpen(int , uint32_t , uint32_t, bool);

    #include "dlb_api.h"

    MS_USE_CASE           use_case;
    MS_PARAMS             ms_args;
    MS_IOBUFFER           io_buffer;
    MS_MEM_SIZES          mem_sizes;
    MS_MEM_PTR            mem_ptr;
    MS_COMPONENTS_HANDLE  comp_handles;
    MS_PROCESSING_STATE   ms_process_state;
    MS_RETVAL             retval;
    bool                  mStartDDPDecoding;
    int                   mBytesConsumedDDC;
    int                   mBytesStoredDDC;
    void                  *handle;
    int                   mUnableToDecode;

protected:

};



} //namespace android

#endif
