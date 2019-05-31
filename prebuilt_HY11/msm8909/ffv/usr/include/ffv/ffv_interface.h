/*============================================================================
* Copyright (c) 2017, 2018 Qualcomm Technologies, Inc.                       *
* All Rights Reserved.                                                       *
* Confidential and Proprietary - Qualcomm Technologies, Inc.                 *
* ===========================================================================*/
#ifndef _FARFIELD_VOICE_FRONTEND_INTERFACE_H_
#define _FARFIELD_VOICE_FRONTEND_INTERFACE_H_

/**
 * @file ffv_interface.h
 *
 * This header file declares the interface to the far-filed voice
 * front-end algorithm.
 */
#include <stddef.h>
#ifndef _INT16_DEFINED
typedef  signed short       int16;       /* Signed 16 bit value */
#define _INT16_DEFINED
#endif

#ifndef _UINT8_DEFINED
typedef  unsigned char      uint8;       /* Unsigned 8  bit value */
#define _UINT8_DEFINED
#endif

#ifndef _UINT32_DEFINED
typedef  unsigned int   uint32;      // Unsigned 32 bit value
#define _UINT32_DEFINED
#endif

#define MAX_SECTOR_NUM (10)

/* Error codes */
typedef enum
{
   FFV_SUCCESS             =  0,   /* Success */
   FFV_ERR_INVALID_PARAM         = -1,   /* Invalid function parameter, unspecified */
   FFV_ERR_INVALID_INCHANNUM_PARAM = -2,
   FFV_ERR_INVALID_OUTCHANNUM_PARAM = -3,
   FFV_ERR_INVALID_REFCHANNUM_PARAM = -4,
   FFV_ERR_INVALID_FRAMELEN_PARAM = -5,
   FFV_ERR_INVALID_CONFIGFILE_PARAM = -6,
   FFV_ERR_PARSE_CONFIGFILE = -7,
   FFV_ERR_PARSE_CONFIGFILE_BFCOEFF = -8,
   FFV_ERR_PARSE_CONFIGFILE_BFCOEFF_MAS = -9,
   FFV_ERR_INIT_GETSIZE = -10,
   FFV_ERR_INIT = -11,
   FFV_ERR_GETPARAM = -12,
   FFV_ERR_SETPARAM = -13,
   FFV_ERR_REGISTER_EVENT_CALLBACK = -14,
   FFV_ERR_LICENSE_CHECK_FAILED = -15,
   FFV_ERR_NOMEM    = -100  /*  */
}FfvStatusType;


/*Event type */
typedef enum
{
   EVENT_KEYWORD_DETECTION  = 0
}FfvEventType;
typedef void (*ffv_event_callback_fn_t)(void *callback_data, void* handle, FfvEventType ffv_event, void* event_payload, size_t event_payload_size);
// declare a funciton prototype
/*======================================================================

  FUNCTION      ffv_register_event_callback

  DESCRIPTION

  DEPENDENCIES  ffv_init must be called prior to calling this function.

  PARAMETERS    handle: [in] Pointer to the library instance
                fun_ptr [in]: Event callback function pointer passed by STHAL


  RETURN VALUE  FFV_SUCCESS if successful,
                FFV_ERR_REGISTER_EVENT_CALLBACK otherwise.

  SIDE EFFECTS  None

======================================================================*/

FfvStatusType ffv_register_event_callback(void *handle, ffv_event_callback_fn_t fun_ptr,void *callback_data);

/*KEYWORD_DETECTION EVENT status and payload descriptions */
typedef enum {
    KWD_STATUS_NONE         = 0x00, // nop
    KWD_STATUS_IN_PROCESS   = 0x01, // running
    KWD_STATUS_DETECTED     = 0x02, // detected
    KWD_STATUS_END_SPEECH   = 0x03, // epd detected
    KWD_STATUS_REJECTED     = 0x04, // rejected
    KWD_STATUS_IN_DISCOVERY = 0x05, // discovery keyword end
} FfvKwdStatus;

#define     FFV_KW_DETECTION_EVENT_PAYLOAD_VERSION      0x1
#define     KEY1_VERSION                                0X1
#define     KEY2_VERSION                                0X1

typedef enum
{
    DET_EVENT_CONF_LEVELS        = 0,        //  confidence level
    DET_EVENT_KWD_INDEX          = 1,        //  indices
}kwdPayloadType;
#if defined(__arm__) || defined(__NEON_OPT__)
typedef struct __attribute__((packed)) keyword_detection_status
#else
typedef struct keyword_detection_status
#endif
{
  uint32 timestamp_lsw; //-> lsw of detection event timestamp (system clock to be used? )
  uint32 timestamp_msw; //-> msw of detection event timestamp
  uint8  detection_status; // see FfvKwdStatus
  uint8  payload_size;//  size of payload to be followed after this
}ffv_keyword_detection_status_t;



/* Define GEG/SET Parms */

/*======================================================================

  FUNCTION      ffv_get_param

  DESCRIPTION   Get FFV param, copying from FFV library to a block of memory query for the param

  DEPENDENCIES  params_buffer_ptr must not be NULL.
                ffv_init must be called prior to calling this function.

  PARAMETERS    handle: [in/out] Pointer to the library instance
                param_id: [in]: PARAMETER ID
                params_buffer_ptr: [in] Pointer to a memory to save the payload for getting FFV parameter
                buffer_size: [in]: memory size
                param_size_ptr: [out] copied parameter memory size

  RETURN VALUE  FFV_SUCCESS if successful,
                FFV_ERR_GETPARAM otherwise.

  SIDE EFFECTS  None

======================================================================*/

FfvStatusType ffv_get_param(
        void *handle,
        char *params_buffer_ptr,
        int param_id,
        int buffer_size,
        int *param_size_ptr
        );


/*======================================================================

  FUNCTION      ffv_set_param

  DESCRIPTION

  DEPENDENCIES
                FfvInit must be called prior to calling this function.

  PARAMETERS    handle: [in/out] Pointer to the library instance
                param_id: [in]: PARAMETER ID
                params_buffer_ptr: [in] Pointer to a memory contains payload for setting FFV parameter
                param_size: [in] parameter memory size
  RETURN VALUE  FFV_SUCCESS if successful,
                FFV_ERR_SETPARAM otherwise.
  SIDE EFFECTS  None

======================================================================*/
FfvStatusType ffv_set_param(
        void *handle,
        char *params_buffer_ptr,
        int param_id,
        int param_size
        );


#define FFV_VER_PARAM  (0x0)
typedef struct ffv_ver_param_t {
   int ver;
   /*
      Rest of the parameters
    */
}ffv_ver_param_t;

#define FFV_HISTORY_BUFFER_LENGTH_PARAM    0x0001 // get only
typedef struct ffv_history_buffer_length_param
{
   size_t bufSize;
 }ffv_history_buffer_length_param_t;

#define FFV_SRC_TRACKING_PARAM  0x0002 // get only
typedef struct ffv_src_tracking_param
{
        int target_angle_L16[2];  // provide the dominant target angle boundaries  (in degree)
                                    // [-1 -1]: undefined target
                                    // [a   b]: target direction is within [a,b] degree
        int interf_angle_L16[2];        // provide the dominant interference boundaries  (in degree)
                                    // [-1 -1]: undefined interference
                                    // [a   b]: interference direction is within [a,b] degree
        char polarActivityGUI[360]; // TO BE COMPATIBLE WITH SSR, NOT SUPPORTED YET:  to provide indication of sound source strength in the 360 degree plane;
}ffv_src_tracking_param_t;

#define FFV_START_VOICE_RECOGNITION_PARAM 0x0003 // set only

#define FFV_STOP_VOICE_RECOGNITION_PARAM 0x0004 // set only

#define FFV_RESET_AEC_PARAM 0x0005 // set only

#define FFV_SOUND_FOCUS_PARAM 0x0006 // set only to be used after Soud Focus Feature is developed
typedef struct ffv_sound_focus_param
{
        int numSectors;
        char target_sec_en[MAX_SECTOR_NUM];
        int sect[MAX_SECTOR_NUM]; //
}ffv_sound_focus_param_t;

#define FFV_TARGET_CHANNEL_INDEX_PARAM 0x0007 // set and get
typedef struct ffv_target_channel_index_param
{
        int target_chan_idx;  // provide the target sector index based on KW detection result
}ffv_target_channel_index_param_t;

#define FFV_STEREORX_USE_DWNMIX_PARAM 0x0008 // downmix stereo rx ref to mono ref
typedef struct ffv_stereorx_use_dwnmix_param
{
    int stereorx_dwnmix_enable;  // downmix enable/disable flag
}ffv_stereorx_use_dwnmix_param_t;

/**
 * Initialize farfield front-end (ECNS + SVA) algorithm.
 *
 * @param[out] handle
 *    Pointer to dynamically allocated ffvfe library
 *
 * @param[in] num_in_chan
 *    Number of input channels
 *
 * @param[in] num_out_chan
 *    Number of output channels
 *
 * @param[in] num_ec_ref_chan
 *    Number of channels for echo cancellation reference
 *
 * @param[in] frame_len
 *    Number of samples per frame for one channel
 *
 * @param[in] sample_rate
 *    Sampling rate in Hz
 *
 * @param[in] config_file_name
 *    Path to config file
 *
 * @param[in] svaModelBuffer
 *    buffer for SVA sound model
 *
 * @param[in] svaModelSize
 *    SVA Model size in bytes

 * @param[out] totMemSize
 *    total memory size allocated for the library

 * @return 0, or error code
 */
FfvStatusType ffv_init(void **handle,
            int num_in_chan,
            int num_out_chan,
            int num_ec_ref_chan,
            int frame_len,
            int sample_rate,
            const char *config_file_name,
            const char* svaModelBuffer,
            size_t svaModelSize,
            int* totMemSize,
            int product_id,
             const char* license
             );
/**
* far-field voice front end process.
*
* @param[in/out] handle (from surround_rec_init function)
* @param in_pcm[in] interleaved 16-bit PCM data
* @param out_pcm[out] interleaved 16-bit PCM output
* @param ec_ref_pcm[in] interleaved 16-bit PCM echo cancellation reference
*/
void ffv_process(void *handle, const int16 *in_pcm,  int16 *out_pcm, const int16 *ec_ref_pcm);
/**
 * far-field voice front end process, v2.
*
* @param[in/out] handle (from surround_rec_init function)
* @param in_pcm[in] interleaved 16-bit PCM data
* @param out_pcm[out] interleaved 16-bit PCM output
 * @param bsp_out_pcm[out] interleaved 16-bit PCM output for basic signal processing (e.g.., HPF, Gain, LEC)
* @param ec_ref_pcm[in] interleaved 16-bit PCM echo cancellation reference
*/
void ffv_process_v2(void *handle, const int16 *in_pcm, int16 *out_pcm, int16 *bsp_out_pcm, const int16 *ec_ref_pcm);


/**
* far-field voice front end read function
*
* @param[in] handle (from ffv_init function)
* @param pOutBuf[out] pointer to the buffer for reading
* @param bytes[in] number of bytes to read
*
* @return actual number of bytes read, or -1 if there is an error
*/
int ffv_read(void* handle, char* pOutBuf, int bytes);

/**
* Deinit algorithm, freeing allocated memory.
*
* @param[out] handle
*/

void ffv_deinit(void *handle);

#endif /* #ifndef _FFV_INTERFACE_H_ */
