/**
* @file QCamera2App.h
* @brief encode and decode api, the api is packeted based on OMX API
* the api differ from JAVA API, it is writen with C++
* @author chapin, chapin.fang@quectel.com
* @version 2.0
* @date 2018-08-28
*/

#define GL_Decoder_Init omx_decoder_init
#define GL_Decoder_Process omx_decoder_process
#define GL_Decoder_Release omx_decoder_release
#define QL_Encoder_Init omx_encoder_init
#define GL_Encoder_Process omx_encoder_process
#define GL_Encoder_Release omx_encoder_release
#define GL_Encoder_SetIntraperiod omx_encoder_setintraperiod

#ifdef __cplusplus
extern "C" {
#endif
	
typedef unsigned char OMX_U8;
typedef signed long long OMX_TICKS;
typedef signed int OMX_S32;
typedef int OMX_U32;
/**
 @brief enum with video codec profile type
*/ 	
  enum VideoCodecProfileType
  {
  //======add by les codec profile format, it should in keeping with struct CodecProfileType in "qcamera-omx-lib/common/src/QCamera2ComDef.h"====
    CODECPROFILE_MPEG4_Simple,
    CODECPROFILE_MPEG4_AdvancedSimple,
    CODECPROFILE_H263_Baseline,
    CODECPROFILE_AVC_Baseline,
    CODECPROFILE_AVC_High,
    CODECPROFILE_AVC_Main,
    CODECPROFILE_VP8_Main,
    //CODECPROFILE_HEVC_Main,
    //CODECPROFILE_HEVC_Main10,    
  };
  
/**
 @brief enum with video codec type
*/  
   enum VideoCodecType
   {
  //======add by les codec format, it should in keeping with struct CodecProfileType in "hardware/qcom/media/mm-core/inc/OMX_Video.h"====
    CODEC_Unused,     /**< Value when coding is N/A */
    CODEC_AutoDetect, /**< Autodetection of coding type */
    CODEC_MPEG2,      /**< AKA: H.262 */
    CODEC_H263,       /**< H.263 */
    CODEC_MPEG4,      /**< MPEG-4 */
    CODEC_WMV,        /**< all versions of Windows Media Video */
    CODEC_RV,         /**< all versions of Real Video */
    CODEC_AVC,        /**< H.264/AVC */
    CODEC_MJPEG,      /**< Motion JPEG */
    CODEC_VP8,        /**< Google VP8, formerly known as On2 VP8 */
    CODEC_VP9,        /**< Google VP9 */
    //CODEC_HEVC,       /**< HEVC */
    CODEC_KhronosExtensions = 0x6F000000, /**< Reserved region for introducing Khronos Standard Extensions */ 
    CODEC_VendorStartUnused = 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
    CODEC_Max = 0x7FFFFFFF ,
  };

/**
 @brief parameter for initializing encoder
*/
  struct EncoderInitParam
  {		
	/**width */ 
	int width;
	/**height*/
	int height; 
	/**bitrate*/
	int bitrate; 
	/**frame rate*/ 
	int framerate; 
	/**0-5*/ 
	int ratetype; 
	/**i frame interval, unit: sec*/
	int intraPeriod; 
	/**min quality 0-100, this param will be used in ratetype 4*/
	int minQP; 
	/**max quality 0-100, this param will be used in ratetype 4*/
	int maxQP;
	/**video codec type: if CODEC_CodingHEVC was set, it will using HEVC codec **/
	VideoCodecType codecType;
	/**video codec profile type: if CODEC_HECV_Main was set, it will using HEVC Main Proifle codec **/
	VideoCodecProfileType codecProfileType;
  };

/**
 @brief PostProc param
*/
  struct PostProcInitParam
  {
    /**source frame width*/
	int srcWidth; 
    /**source frame height*/
	int srcHeight;
    /**dest frame width*/
	int dstWidth;
    /**dest frame height*/
	int dstHeight;
    /**source frame color*/
	int inputColorFormat;
    /**dest frame color*/
	int outputColorFormat;
  };
	
  
/**
 @brief decoder init, create decoder handle
 
 @param  eCodecType
	vido codec Type, exp: CODEC_AVC means H.264/AVC

 @returns the decoder handle, if the handle is null, it mean init fail
*/
  void* omx_decoder_init(VideoCodecType eCodecType);
  

/**
 @brief input the buffer that you will decode, it will return the yuv buffer

 @param  pDecoder
	decoder handle
 @param  pInputData
	the buffer that need to decode
 @param  nInsize
	the input buffer size
 @param  ifakeTimeUs
	rtp timestamp
 @param  pWidth
	resolution width
 @param  pHeight
	resolution height
 @param  nStride
	add the aligning data
 @param  nSliceHeight
	add the aligning data

 @returns: yuv buffer -- NV12
*/

  OMX_U8* omx_decoder_process(void* pDecoder, OMX_U8* pInputData, OMX_S32 nInsize,
                             OMX_TICKS ifakeTimeUs, OMX_U32* pWidth, OMX_U32* pHeight,
                             OMX_U32* nStride, OMX_U32* nSliceHeight);
							 

/**
 @brief  release decode

 @param pDecoder
	 success or fail

 @returns success or fail
**/
  int omx_decoder_release(void* pDecoder);

/**
 @brief encoder init

 @param struct EncoderInitParam
	width  video width
    height video height
	bitrate bit rate
 	framerate frame rat
	ratetype 0~5
   	 	OMX_Video_ControlRateConstant;
		OMX_Video_ControlRateConstantSkipFrames;
     	OMX_Video_ControlRateVariable;
		OMX_Video_ControlRateVariableSkipFrames;
		OMX_Video_ControlRateDisable;

 @returns encoder handle 
**/
  void* omx_encoder_init(struct EncoderInitParam eInitParam);
	
							 
/**
 @brief encode yuv buffer

 @param pEncoder
	 encoder handle
 @param pInputData
	 input data
 @param nInsize
	 input data size
 @param ifakeTimeUs
	 rtp timestamp
 @param nLength
 	 encoded data length

 @returns encoded data
**/
  OMX_U8* omx_encoder_process(void* pEncoder, OMX_U8* pInputData, OMX_U32 nInsize,
                             OMX_TICKS ifakeTimeUs, int* nLength);
							
  
 /**
@brief set i frame interval

@param pEncoder
	encoder handle
@param nIntraperiod
	num of i frame interval

@returns success or fail
**/


  int omx_encoder_setintraperiod(void* pEncoder, int nIntraperiod);
  

  
  /**
  @brief release
	 encoder handle
  @param pEncoder 
	encoder handle

  @returns success of fail
**/

  int omx_encoder_release(void* pEncoder);
  
#ifdef __cplusplus
}
#endif
