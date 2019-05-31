/* Copyright (c) 2015-2017 The Linux Foundataion. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <camera/CameraParameters.h>
#include "camera_parameters.h"
#include "camera_log.h"
#include "qmmf_camera.h"

#include <string>
#include <vector>
#include <cstdlib>
#include <errno.h>
#include <sstream>

using namespace std;
using namespace android;
using namespace qcamera;

#define NANOSEC_PER_SEC 1000000000

namespace camera
{

/* helper function to cast the private params to CameraParameters */
inline static QMMFCameraParams* params_cast(void *priv)
{
    return static_cast<QMMFCameraParams*> (priv);
}

/* writes serialized version of parameters to ostream object
   provided by the caller*/
int CameraParams::writeObject(std::ostream& ps) const

{
	CAM_PRINT("%s API is deprecated for this target \n",__func__);
    return EXIT_FAILURE;
}

CameraParams::CameraParams()
{
    priv_ = NULL;
}

int CameraParams::init(ICameraDevice* device)
{
    int rc = EXIT_SUCCESS;
	if (device == NULL) {
		CAM_ERR("%s:%d Error : NULL device provided ",__func__,__LINE__);
		rc = EXIT_FAILURE;
		goto bail;
	}
	priv_ = device->getParameters();
	if (priv_  == NULL) {
	 CAM_ERR("%s:%d Error : Params init failed ",__func__,__LINE__);
	 rc = EXIT_FAILURE;
	 goto bail;
	}

    device_ = device;
bail:
   return rc;
}

CameraParams::~CameraParams()
{
}

vector<ImageSize> CameraParams::getSupportedPreviewSizes() const
{
	vector<ImageSize> imgSizes;
	uint32_t width,height;
	camera_metadata_entry_t entry;

	params_cast(priv_)->mutexLock();
	if ((params_cast(priv_)->static_default_meta_info.exists(ANDROID_SCALER_AVAILABLE_PROCESSED_SIZES))){
		entry = params_cast(priv_)->static_default_meta_info.find(ANDROID_SCALER_AVAILABLE_PROCESSED_SIZES);
		CAM_DBG("%s:%d ANDROID_SCALER_AVAILABLE_PROCESSED_SIZES count = %d",__func__,__LINE__,entry.count);
		for (uint32_t i = 0 ; i < entry.count; i += 2) {
			width = entry.data.i32[i+0];
			height = entry.data.i32[i+1];
			imgSizes.emplace_back(width,height);
			CAM_DBG("Preview Sizes : %d x %d",width ,height);
		}
	}else{
		CAM_ERR("%s:%d ANDROID_SCALER_AVAILABLE_PROCESSED_SIZES does not exist",__func__,__LINE__);
	}
	params_cast(priv_)->mutexUnlock();
	return imgSizes;
}

void CameraParams::setPreviewSize(const ImageSize& size)
{
    params_cast(priv_)->mutexLock();
    params_cast(priv_)->track_params_ptr[CAMERA_QMMF_TRACK_PREVIEW]->width = size.width;
    params_cast(priv_)->track_params_ptr[CAMERA_QMMF_TRACK_PREVIEW]->height = size.height;
    params_cast(priv_)->mutexUnlock();

}

ImageSize CameraParams::getPreviewSize() const
{
    ImageSize size;
	size.width = params_cast(priv_)->track_params_ptr[CAMERA_QMMF_TRACK_PREVIEW]->width;
	size.height = params_cast(priv_)->track_params_ptr[CAMERA_QMMF_TRACK_PREVIEW]->height;
    return size;
}


vector<ImageSize> CameraParams::getSupportedVideoSizes() const
{
	vector<ImageSize> imgSizes;
	uint32_t width,height;
	camera_metadata_entry_t entry;

	params_cast(priv_)->mutexLock();
	if ((params_cast(priv_)->static_default_meta_info.exists(ANDROID_SCALER_AVAILABLE_PROCESSED_SIZES))){
		entry = params_cast(priv_)->static_default_meta_info.find(ANDROID_SCALER_AVAILABLE_PROCESSED_SIZES);
		CAM_DBG("%s:%d ANDROID_SCALER_AVAILABLE_PROCESSED_SIZES count = %d",__func__,__LINE__,entry.count);
		for (uint32_t i = 0 ; i < entry.count; i += 2) {
			width = entry.data.i32[i+0];
			height = entry.data.i32[i+1];
			imgSizes.emplace_back(width,height);
			CAM_DBG("Preview Sizes : %d x %d \n",width ,height);
		}
	}else{
		CAM_ERR("%s:%d ANDROID_SCALER_AVAILABLE_PROCESSED_SIZES does not exist",__func__,__LINE__);
	}
	params_cast(priv_)->mutexUnlock();
	return imgSizes;
}

void CameraParams::setVideoSize(const ImageSize& size)
{
	params_cast(priv_)->mutexLock();
    params_cast(priv_)->track_params_ptr[CAMERA_QMMF_TRACK_VIDEO]->width = size.width;
    params_cast(priv_)->track_params_ptr[CAMERA_QMMF_TRACK_VIDEO]->height = size.height;
    params_cast(priv_)->mutexUnlock();
}

ImageSize CameraParams::getVideoSize() const
{
    ImageSize size;
	size.width = params_cast(priv_)->track_params_ptr[CAMERA_QMMF_TRACK_VIDEO]->width;
	size.height = params_cast(priv_)->track_params_ptr[CAMERA_QMMF_TRACK_VIDEO]->height;
    return size;
}

vector<ImageSize> CameraParams::getSupportedPictureSizes(std::string format) const
{
	vector<ImageSize> imgSizes;
	uint32_t width,height;
	camera_metadata_entry_t entry;
	camera_metadata_tag metaTag;
	params_cast(priv_)->mutexLock();

	if (format == FORMAT_JPEG) {
	   metaTag = ANDROID_SCALER_AVAILABLE_PROCESSED_SIZES;
	} else if ( format == FORMAT_RAW10 ) {
	   metaTag = ANDROID_SCALER_AVAILABLE_RAW_SIZES;
	} else {
	  CAM_ERR("%s:%d invalid picture format %s",__func__,__LINE__, format.c_str());
	  goto bail;
	}

	if ((params_cast(priv_)->static_default_meta_info.exists(metaTag))) {
		entry = params_cast(priv_)->static_default_meta_info.find(metaTag);
		CAM_DBG("%s:%d ANDROID_SCALER_AVAILABLE_PROCESSED_SIZES count = %d ",__func__,__LINE__,entry.count);
		for (uint32_t i = 0 ; i < entry.count; i += 2) {
			width = entry.data.i32[i+0];
			height = entry.data.i32[i+1];
			imgSizes.emplace_back(width,height);
			CAM_DBG("Preview Sizes : %d x %d \n",width ,height);
		}
	}else{
		CAM_ERR("%s:%d metatag = %d  does not exist",__func__,__LINE__, metaTag );
	}
bail:
	params_cast(priv_)->mutexUnlock();
	return imgSizes;
}

void CameraParams::setPictureSize(const ImageSize& size)
{
	params_cast(priv_)->mutexLock();
    params_cast(priv_)->snapshot_image_param.width = size.width;
    params_cast(priv_)->snapshot_image_param.height = size.height;
    params_cast(priv_)->mutexUnlock();
    return;
}

ImageSize CameraParams::getPictureSize() const
{
    ImageSize size;
	size.width = params_cast(priv_)->snapshot_image_param.width;
	size.height = params_cast(priv_)->snapshot_image_param.height;
    return size;
}

ImageSize CameraParams::getPictureThumbNailSize() const
{
    ImageSize size;
	CAM_PRINT("%s API is not supported for this target \n",__func__);
    return size;
}

void CameraParams::setPictureThumbNailSize(const ImageSize& size)
{
	CAM_PRINT("%s API is not supported for this target \n",__func__);
}

int CameraParams::commit()
{
    /* set the current state of paramters in camera device */
    return device_->setParameters(*this);
}

string CameraParams::get(const string& key) const
{
    string str;
	CAM_PRINT("%s API is deprecated for this target \n",__func__);
    return str;
}

void CameraParams::set(const string& key, const string& value)
{
	//CAM_PRINT("%s API is deprecated for this target \n",__func__);
    return;
}

std::map<camera_metadata_enum_android_control_af_mode,std::string> camAFModeEnum2StringMap = 
{
	{ANDROID_CONTROL_AF_MODE_OFF, FOCUS_MODE_OFF },
    {ANDROID_CONTROL_AF_MODE_AUTO, FOCUS_MODE_AUTO },
    {ANDROID_CONTROL_AF_MODE_MACRO, FOCUS_MODE_MACRO },
    {ANDROID_CONTROL_AF_MODE_CONTINUOUS_VIDEO, FOCUS_MODE_CONTINUOUS_VIDEO } ,
    {ANDROID_CONTROL_AF_MODE_CONTINUOUS_PICTURE, FOCUS_MODE_CONTINUOUS_PICTURE },
    {ANDROID_CONTROL_AF_MODE_EDOF, FOCUS_MODE_EDOF }
};

std::map<std::string,camera_metadata_enum_android_control_af_mode> camString2AFModeEnumMap = 
{
	{FOCUS_MODE_OFF, ANDROID_CONTROL_AF_MODE_OFF },
    {FOCUS_MODE_AUTO , ANDROID_CONTROL_AF_MODE_AUTO},
    {FOCUS_MODE_MACRO , ANDROID_CONTROL_AF_MODE_MACRO},
    {FOCUS_MODE_CONTINUOUS_VIDEO , ANDROID_CONTROL_AF_MODE_CONTINUOUS_VIDEO,  } ,
    {FOCUS_MODE_CONTINUOUS_PICTURE , ANDROID_CONTROL_AF_MODE_CONTINUOUS_PICTURE},
    {FOCUS_MODE_EDOF , ANDROID_CONTROL_AF_MODE_EDOF}
};

std::map<camera_metadata_enum_android_control_awb_mode,std::string> camAWBModeEnum2StringMap =
{
    {ANDROID_CONTROL_AWB_MODE_OFF, WHITE_BALANCE_OFF },
    {ANDROID_CONTROL_AWB_MODE_AUTO, WHITE_BALANCE_AUTO },
    {ANDROID_CONTROL_AWB_MODE_INCANDESCENT, WHITE_BALANCE_INCANDESCENT },
    {ANDROID_CONTROL_AWB_MODE_FLUORESCENT, WHITE_BALANCE_FLUORESCENT },
    {ANDROID_CONTROL_AWB_MODE_WARM_FLUORESCENT, WHITE_BALANCE_WARM_FLUORESCENT } ,
    {ANDROID_CONTROL_AWB_MODE_DAYLIGHT, WHITE_BALANCE_DAYLIGHT },
    {ANDROID_CONTROL_AWB_MODE_CLOUDY_DAYLIGHT, WHITE_BALANCE_CLOUDY_DAYLIGHT } ,
    {ANDROID_CONTROL_AWB_MODE_TWILIGHT, WHITE_BALANCE_TWILIGHT},
    {ANDROID_CONTROL_AWB_MODE_SHADE, WHITE_BALANCE_SHADE }
};

std::map<std::string,camera_metadata_enum_android_control_awb_mode> camString2AWBModeEnumMap =
{
    {WHITE_BALANCE_OFF, ANDROID_CONTROL_AWB_MODE_OFF },
    {WHITE_BALANCE_AUTO, ANDROID_CONTROL_AWB_MODE_AUTO },
    {WHITE_BALANCE_INCANDESCENT, ANDROID_CONTROL_AWB_MODE_INCANDESCENT },
    {WHITE_BALANCE_FLUORESCENT, ANDROID_CONTROL_AWB_MODE_FLUORESCENT },
    {WHITE_BALANCE_WARM_FLUORESCENT, ANDROID_CONTROL_AWB_MODE_WARM_FLUORESCENT } ,
    {WHITE_BALANCE_DAYLIGHT, ANDROID_CONTROL_AWB_MODE_DAYLIGHT },
    {WHITE_BALANCE_CLOUDY_DAYLIGHT, ANDROID_CONTROL_AWB_MODE_CLOUDY_DAYLIGHT } ,
    {WHITE_BALANCE_TWILIGHT, ANDROID_CONTROL_AWB_MODE_TWILIGHT },
    {WHITE_BALANCE_SHADE, ANDROID_CONTROL_AWB_MODE_SHADE }
};

std::map<camera_metadata_enum_android_edge_mode,std::string> camEDGEModeEnum2StringMap =
{
    {ANDROID_EDGE_MODE_OFF, SHARPNESS_MODE_EDGE_OFF },
    {ANDROID_EDGE_MODE_FAST, SHARPNESS_MODE_EDGE_FAST },
    {ANDROID_EDGE_MODE_HIGH_QUALITY, SHARPNESS_MODE_EDGE_HIGH_QUALITY },
    {ANDROID_EDGE_MODE_ZERO_SHUTTER_LAG, SHARPNESS_MODE_EDGE_ZERO_SHUTTER_LAG }
};

std::map<std::string,camera_metadata_enum_android_edge_mode> camString2EDGEModeEnumMap =
{
    {SHARPNESS_MODE_EDGE_OFF, ANDROID_EDGE_MODE_OFF },
    {SHARPNESS_MODE_EDGE_FAST, ANDROID_EDGE_MODE_FAST },
    {SHARPNESS_MODE_EDGE_HIGH_QUALITY, ANDROID_EDGE_MODE_HIGH_QUALITY },
    {SHARPNESS_MODE_EDGE_ZERO_SHUTTER_LAG, ANDROID_EDGE_MODE_ZERO_SHUTTER_LAG }
};

std::map<camera_metadata_enum_android_tonemap_mode,std::string> camToneMapModeEnum2StringMap =
{
    {ANDROID_TONEMAP_MODE_CONTRAST_CURVE, TONEMAP_CONTRAST_CURVE },
    {ANDROID_TONEMAP_MODE_FAST, TONEMAP_FAST },
    {ANDROID_TONEMAP_MODE_HIGH_QUALITY, TONEMAP_HIGH_QUALITY },
    {ANDROID_TONEMAP_MODE_GAMMA_VALUE, TONEMAP_GAMMA_VALUE },
    {ANDROID_TONEMAP_MODE_PRESET_CURVE, TONEMAP_PRESET_CURVE }
};

std::map<std::string,camera_metadata_enum_android_tonemap_mode> camString2ToneMapModeEnumMap =
{
    {TONEMAP_CONTRAST_CURVE, ANDROID_TONEMAP_MODE_CONTRAST_CURVE },
    {TONEMAP_FAST, ANDROID_TONEMAP_MODE_FAST },
    {TONEMAP_HIGH_QUALITY, ANDROID_TONEMAP_MODE_HIGH_QUALITY },
    {TONEMAP_GAMMA_VALUE, ANDROID_TONEMAP_MODE_GAMMA_VALUE },
    {TONEMAP_PRESET_CURVE, ANDROID_TONEMAP_MODE_PRESET_CURVE }
};


std::map<qcamera3_ext_iso_mode,std::string> camISOModeEnum2StringMap =
{
    {QCAMERA3_ISO_MODE_AUTO,  ISO_AUTO },
    {QCAMERA3_ISO_MODE_DEBLUR, ISO_HJR },
    {QCAMERA3_ISO_MODE_100,    ISO_100 },
    {QCAMERA3_ISO_MODE_200,    ISO_200 },
    {QCAMERA3_ISO_MODE_400,    ISO_400 },
    {QCAMERA3_ISO_MODE_800,    ISO_800 },
    {QCAMERA3_ISO_MODE_1600,   ISO_1600 },
    {QCAMERA3_ISO_MODE_3200,   ISO_3200 }
};

std::map<std::string,qcamera3_ext_iso_mode> camString2ISOModeEnumMap =
{
    {ISO_AUTO, QCAMERA3_ISO_MODE_AUTO },
    {ISO_HJR, QCAMERA3_ISO_MODE_DEBLUR },
    {ISO_100, QCAMERA3_ISO_MODE_100 },
    {ISO_200, QCAMERA3_ISO_MODE_200 },
    {ISO_400, QCAMERA3_ISO_MODE_400 } ,
    {ISO_800, QCAMERA3_ISO_MODE_800 },
    {ISO_1600, QCAMERA3_ISO_MODE_1600 } ,
    {ISO_3200, QCAMERA3_ISO_MODE_3200 }
};

vector<string> CameraParams::getSupportedFocusModes() const
{
    vector<string> focusModes;

	params_cast(priv_)->mutexLock();
	camera_metadata_entry_t entry;
	if (params_cast(priv_)->static_default_meta_info.exists(ANDROID_CONTROL_AF_AVAILABLE_MODES)) {
	   entry = params_cast(priv_)->static_default_meta_info.find(ANDROID_CONTROL_AF_AVAILABLE_MODES);
	   for (uint32_t i = 0 ; i < entry.count; i++) {
		 auto it = camAFModeEnum2StringMap.find(static_cast<camera_metadata_enum_android_control_af_mode>(entry.data.u8[i]));
		 if (it!=camAFModeEnum2StringMap.end()) {
			 focusModes.emplace_back(it->second);
		 }else{
			 CAM_ERR("%s:%d Focus mode %d currently not supported.",__func__,__LINE__,entry.data.u8[i]);
		 }
	   }
	 }else{
	   CAM_ERR("%s:%d ANDROID_CONTROL_AF_AVAILABLE_MODES does not exist",__func__,__LINE__);
	}
	params_cast(priv_)->mutexUnlock();
    return focusModes;
}

string CameraParams::getFocusMode() const
{
    string str;
	camera_metadata_enum_android_control_af_mode af_mode;

      params_cast(priv_)->mutexLock();
	 /* Can do some camera params initialization here */
	 if ( (params_cast(priv_)->static_session_meta_info.exists(ANDROID_CONTROL_AF_MODE)) ){
	   af_mode = static_cast<camera_metadata_enum_android_control_af_mode>(params_cast(priv_)->static_session_meta_info.find(ANDROID_CONTROL_AF_MODE).data.u8[0]);
	   	auto it = camAFModeEnum2StringMap.find(af_mode);
		if (it != camAFModeEnum2StringMap.end()) {
			str = it->second;
			CAM_DBG("%s:%s:%d Get focus mode : %d - %s   \n",TAG,__func__,__LINE__,af_mode,str.c_str());
		}else{
			CAM_ERR("%s:%d Unknown focus mode: %d \n",__func__,__LINE__,af_mode);
		}
	 }
	 params_cast(priv_)->mutexUnlock();
    return str;
}

void CameraParams::setFocusMode(const string& value)
{
	uint8_t af_mode;
	auto it = camString2AFModeEnumMap.find(value);
	if (it != camString2AFModeEnumMap.end()) {
		af_mode = it->second;
		params_cast(priv_)->mutexLock();
		/* Can do some camera params initialization here */
		if ( (params_cast(priv_)->static_session_meta_info.exists(ANDROID_CONTROL_AF_MODE)) ){
		  //af_mode = params_cast(priv_)->static_session_meta_info.find(ANDROID_CONTROL_AF_MODE).data.u8[0];
		  params_cast(priv_)->static_session_meta_info.update(ANDROID_CONTROL_AF_MODE, &af_mode, 1);
		  CAM_INFO("%s:%s:%d Set  auto focus mode : %d - %s \n",TAG,__func__,__LINE__,af_mode,value.c_str());
		  params_cast(priv_)->static_session_meta_info_updated_flag = true;
		}
		params_cast(priv_)->mutexUnlock();
	}else{
		CAM_ERR("%s:%d Invalid Focus mode: %s \n",__func__,__LINE__, value.c_str());
	}
	return;
}

vector<string> CameraParams::getSupportedWhiteBalance() const
{
    vector<string> awb_modes;

	params_cast(priv_)->mutexLock();
	camera_metadata_entry_t entry;
	if (params_cast(priv_)->static_default_meta_info.exists(ANDROID_CONTROL_AWB_AVAILABLE_MODES)) {
	   entry = params_cast(priv_)->static_default_meta_info.find(ANDROID_CONTROL_AWB_AVAILABLE_MODES);
	   for (uint32_t i = 0 ; i < entry.count; i++) {
		 auto it = camAWBModeEnum2StringMap.find(static_cast<camera_metadata_enum_android_control_awb_mode>(entry.data.u8[i]));
				 if (it!=camAWBModeEnum2StringMap.end()) {
					CAM_DBG("%s:%d AWB mode %d currently supported.",__func__,__LINE__,entry.data.u8[i]);
			 awb_modes.emplace_back(it->second);
		 }else{
			 CAM_ERR("%s:%d AWB mode %d currently not supported.",__func__,__LINE__,entry.data.u8[i]);
		 }
	   }
	 }else{
	   CAM_ERR("%s:%d ANDROID_CONTROL_AWB_AVAILABLE_MODES does not exist",__func__,__LINE__);
	}
	params_cast(priv_)->mutexUnlock();
    return awb_modes;
}

string CameraParams::getWhiteBalance() const
{
    string str;
	camera_metadata_enum_android_control_awb_mode awb_mode;

      params_cast(priv_)->mutexLock();
	 /* Can do some camera params initialization here */
	 if ( (params_cast(priv_)->static_session_meta_info.exists(ANDROID_CONTROL_AWB_MODE)) ){
	   awb_mode = static_cast<camera_metadata_enum_android_control_awb_mode>(params_cast(priv_)->static_session_meta_info.find(ANDROID_CONTROL_AWB_MODE).data.u8[0]);
		auto it = camAWBModeEnum2StringMap.find(awb_mode);
		if (it != camAWBModeEnum2StringMap.end()) {
			str = it->second;
			CAM_DBG("%s:%s:%d Get AWB mode : %d - %s   \n",TAG,__func__,__LINE__,awb_mode,str.c_str());
		}else{
			CAM_ERR("%s:%d Unknown AWB mode: %d \n",__func__,__LINE__,awb_mode);
		}
	 }
	 params_cast(priv_)->mutexUnlock();
    return str;
}

void CameraParams::setWhiteBalance(const string& value)
{
	uint8_t awb_mode;
	auto it = camString2AWBModeEnumMap.find(value);
	if (it != camString2AWBModeEnumMap.end()) {
		awb_mode = it->second;
		params_cast(priv_)->mutexLock();
		/* Can do some camera params initialization here */
		if ( (params_cast(priv_)->static_session_meta_info.exists(ANDROID_CONTROL_AWB_MODE)) ){
		  //awb_mode = params_cast(priv_)->static_session_meta_info.find(ANDROID_CONTROL_AWB_MODE).data.u8[0];
		  params_cast(priv_)->static_session_meta_info.update(ANDROID_CONTROL_AWB_MODE, &awb_mode, 1);
		  CAM_DBG("%s:%s:%d Set AWB mode : %d - %s \n",TAG,__func__,__LINE__,awb_mode,value.c_str());
		  params_cast(priv_)->static_session_meta_info_updated_flag = true;
		}
		params_cast(priv_)->mutexUnlock();
	}else{
		CAM_ERR("%s:%d Invalid Focus mode: %s \n",__func__,__LINE__, value.c_str());
	}
	return;
}

vector<string> CameraParams::getSupportedISO() const
{
    vector<string> iso_modes;
	qcamera3_ext_iso_mode iso_mode;

	params_cast(priv_)->mutexLock();
	camera_metadata_entry_t entry;
	if (params_cast(priv_)->static_default_meta_info.exists(QCAMERA3_ISO_AVAILABLE_MODES)) {
	   entry = params_cast(priv_)->static_default_meta_info.find(QCAMERA3_ISO_AVAILABLE_MODES);
	   for (uint32_t i = 0 ; i < entry.count; i++) {
			CAM_DBG("%s:%d QCAMERA3_ISO_AVAILABLE_MODES %d currently supported.",__func__,__LINE__,entry.data.i32[i]);
			iso_mode = (qcamera3_ext_iso_mode)entry.data.i32[i];
			auto it = camISOModeEnum2StringMap.find(iso_mode);
			if (it != camISOModeEnum2StringMap.end()) {
				CAM_DBG("%s:%d Get ISO mode : %d - %s   \n",__func__,__LINE__,iso_mode,it->second.c_str());
				iso_modes.emplace_back(it->second);
			}else{
				CAM_ERR("%s:%d Unknown ISO mode: %d \n",__func__,__LINE__,iso_mode);
			}
	   }
	 }else{
	   CAM_ERR("%s:%d QCAMERA3_ISO_AVAILABLE_MODES does not exist",__func__,__LINE__);
	}
	params_cast(priv_)->mutexUnlock();
    return iso_modes;
}

string CameraParams::getISO() const
{
    string str;
	qcamera3_ext_iso_mode iso_mode;

      params_cast(priv_)->mutexLock();

	 if ( (params_cast(priv_)->static_session_meta_info.exists(QCAMERA3_USE_ISO_EXP_PRIORITY)) ){
	   iso_mode = static_cast<qcamera3_ext_iso_mode>(params_cast(priv_)->static_session_meta_info.find(QCAMERA3_USE_ISO_EXP_PRIORITY).data.i64[0]);
		auto it = camISOModeEnum2StringMap.find(iso_mode);
		if (it != camISOModeEnum2StringMap.end()) {
			str = it->second;
			CAM_DBG("%s:%s:%d Get ISO mode : %d - %s   \n",TAG,__func__,__LINE__,iso_mode,str.c_str());
		}else{
			CAM_ERR("%s:%d Unknown ISO mode: %d \n",__func__,__LINE__,iso_mode);
		}
	 }
    params_cast(priv_)->mutexUnlock();
    return str;
}

void CameraParams::setISO(const string& value)
{
	int32_t use_exp_priority = 0;
	int64_t iso_mode;

	auto it = camString2ISOModeEnumMap.find(value);
	if (it != camString2ISOModeEnumMap.end()) {
	 iso_mode = it->second;
	 params_cast(priv_)->mutexLock();
	 params_cast(priv_)->static_session_meta_info.update(QCAMERA3_SELECT_PRIORITY, &use_exp_priority, 4);
	 params_cast(priv_)->static_session_meta_info.update(QCAMERA3_USE_ISO_EXP_PRIORITY, &iso_mode, 8);
	 CAM_DBG("%s:%s:%d Set ISO mode : %lld - %s \n",TAG,__func__,__LINE__,iso_mode,value.c_str());
	 params_cast(priv_)->static_session_meta_info_updated_flag = true;
	 params_cast(priv_)->mutexUnlock();
	}else{
	 CAM_ERR("%s:%d Invalid ISO mode: %s \n",__func__,__LINE__, value.c_str());
	}

    return;
}

vector<string> CameraParams::getSupportedSharpnessMode() const
{
    vector<string> edge_modes;

	params_cast(priv_)->mutexLock();
	camera_metadata_entry_t entry;
	if (params_cast(priv_)->static_default_meta_info.exists(ANDROID_EDGE_AVAILABLE_EDGE_MODES)) {
	   entry = params_cast(priv_)->static_default_meta_info.find(ANDROID_EDGE_AVAILABLE_EDGE_MODES);
	   for (uint32_t i = 0 ; i < entry.count; i++) {
		 auto it = camEDGEModeEnum2StringMap.find(static_cast<camera_metadata_enum_android_edge_mode>(entry.data.u8[i]));
				 if (it!=camEDGEModeEnum2StringMap.end()) {
					CAM_DBG("%s:%d EDGE mode %d currently supported.",__func__,__LINE__,entry.data.u8[i]);
			 edge_modes.emplace_back(it->second);
		 }else{
			 CAM_ERR("%s:%d EDGE mode %d currently not supported.",__func__,__LINE__,entry.data.u8[i]);
		 }
	   }
	 }else{
	   CAM_ERR("%s:%d ANDROID_EDGE_MODE does not exist",__func__,__LINE__);
	}
	params_cast(priv_)->mutexUnlock();
    return edge_modes;
}

string CameraParams::getSharpnessMode() const
{
    string str;
	camera_metadata_enum_android_edge_mode edge_mode;

      params_cast(priv_)->mutexLock();
	 /* Can do some camera params initialization here */
	 if ( (params_cast(priv_)->static_session_meta_info.exists(ANDROID_EDGE_MODE)) ){
	   edge_mode = static_cast<camera_metadata_enum_android_edge_mode>(params_cast(priv_)->static_session_meta_info.find(ANDROID_EDGE_MODE).data.u8[0]);
		auto it = camEDGEModeEnum2StringMap.find(edge_mode);
		if (it != camEDGEModeEnum2StringMap.end()) {
			str = it->second;
			CAM_DBG("%s:%s:%d Get EDGE mode : %d - %s   \n",TAG,__func__,__LINE__,edge_mode,str.c_str());
		}else{
			CAM_ERR("%s:%d Unknown EDGE mode: %d \n",__func__,__LINE__,edge_mode);
		}
	 }
	 params_cast(priv_)->mutexUnlock();
    return str;
}

void CameraParams::setSharpnessMode(const string& value)
{
	uint8_t edge_mode;
	auto it = camString2EDGEModeEnumMap.find(value);
	if (it != camString2EDGEModeEnumMap.end()) {
		edge_mode = it->second;
		params_cast(priv_)->mutexLock();
		/* Can do some camera params initialization here */
		if ( (params_cast(priv_)->static_session_meta_info.exists(ANDROID_EDGE_MODE)) ){
		  //edge_mode = params_cast(priv_)->static_session_meta_info.find(ANDROID_EDGE_MODE).data.u8[0];
		  params_cast(priv_)->static_session_meta_info.update(ANDROID_EDGE_MODE, &edge_mode, 1);
		  CAM_DBG("%s:%s:%d Set EDGE mode : %d - %s \n",TAG,__func__,__LINE__,edge_mode,value.c_str());
		  params_cast(priv_)->static_session_meta_info_updated_flag = true;
		}
		params_cast(priv_)->mutexUnlock();
	}else{
		CAM_ERR("%s:%d Invalid EDGE mode: %s \n",__func__,__LINE__, value.c_str());
	}
	return;
}

Range CameraParams::getSupportedSharpness() const
{
	Range range;

	params_cast(priv_)->mutexLock();
	camera_metadata_entry_t entry;
	if (static_cast<bool>(params_cast(priv_)->static_default_meta_info.find(ANDROID_EDGE_MODE).data.u8[0]) != ANDROID_EDGE_MODE_OFF) {
		   /* Switching EDGE mode to OFF for sharpness to take effect*/
		const uint8_t edge_mode = ANDROID_EDGE_MODE_OFF;
		params_cast(priv_)->static_session_meta_info.update(ANDROID_EDGE_MODE, &edge_mode, 1);

		entry = params_cast(priv_)->static_default_meta_info.find(QCAMERA3_SHARPNESS_RANGE);
		range.min = entry.data.i32[0];
		range.max = entry.data.i32[1];
		CAM_ERR("%s:%d range.min:%d range.max:%d ",__func__,__LINE__,range.min,range.max);
	}else{
		CAM_ERR("%s:%d ANDROID_CONTROL_AWB_AVAILABLE_MODES does not exist",__func__,__LINE__);
	}
	params_cast(priv_)->mutexUnlock();
	return range;
}

int CameraParams::getSharpness() const
{
    int32_t strength;

      params_cast(priv_)->mutexLock();
	 /* Can do some camera params initialization here */
	 //if ( (params_cast(priv_)->static_session_meta_info.exists(ANDROID_EDGE_MODE).data.u8[0] != ANDROID_EDGE_MODE_OFF) ) {
     if (static_cast<bool>(params_cast(priv_)->static_default_meta_info.find(ANDROID_EDGE_MODE).data.u8[0]) != ANDROID_EDGE_MODE_OFF) {

		/* Switching EDGE mode to OFF for sharpness to take effect*/
		const uint8_t edge_mode = ANDROID_EDGE_MODE_OFF;
		params_cast(priv_)->static_session_meta_info.update(ANDROID_EDGE_MODE, &edge_mode, 1);

		if (params_cast(priv_)->static_default_meta_info.exists(QCAMERA3_SHARPNESS_STRENGTH)) {
		    strength = params_cast(priv_)->static_default_meta_info.find(QCAMERA3_SHARPNESS_STRENGTH).data.i32[0];
		  }  else {
		    // In case camera didn't set default.
		    // Setting the value to MIN possible by default.
		    strength = params_cast(priv_)->static_default_meta_info.find(QCAMERA3_SHARPNESS_RANGE).data.i32[0];
         }
	 }else if (static_cast<bool>(params_cast(priv_)->static_default_meta_info.find(ANDROID_EDGE_MODE).data.u8[0]) == ANDROID_EDGE_MODE_OFF) {
		strength = static_cast<int32_t>(params_cast(priv_)->static_session_meta_info.find(QCAMERA3_SHARPNESS_STRENGTH).data.i32[0]);
		}else{
		CAM_ERR("%s:%d failed to get sharpness range",__func__,__LINE__);
	 }
	 params_cast(priv_)->mutexUnlock();
	CAM_DBG("%s:%d Sharpness strength currently at %d",__func__,__LINE__,strength);
    return strength;
}

void CameraParams::setSharpness(int value)
{
	uint8_t edge_mode;

	edge_mode = static_cast<camera_metadata_enum_android_edge_mode>(params_cast(priv_)->static_session_meta_info.find(ANDROID_EDGE_MODE).data.u8[0]);
	CAM_DBG("%s:%s:%d Sharpness Mode: %d %d\n",TAG,__func__,__LINE__,edge_mode,ANDROID_EDGE_MODE_OFF);
	params_cast(priv_)->mutexLock();
		/* Can do some camera params initialization here */
		if (edge_mode != ANDROID_EDGE_MODE_OFF) {
		  params_cast(priv_)->static_session_meta_info.update(QCAMERA3_SHARPNESS_STRENGTH, &value, 1);
		  CAM_INFO("%s:%s:%d Set Sharpness: %d \n",TAG,__func__,__LINE__,value);
		  params_cast(priv_)->static_session_meta_info_updated_flag = true;
		}else{
		CAM_ERR("%s:%d Failed to set sharpness_strength, turn Off ANDROID_EDGE_MODE_OFF\n",__func__,__LINE__);
	}
	params_cast(priv_)->mutexUnlock();
	return;
}

Range CameraParams::getSupportedBrightness() const
{
    Range range;
	CAM_PRINT("%s API is deprecated for this target \n",__func__);
    return range;
}

int CameraParams::getBrightness() const
{
	CAM_PRINT("%s API is deprecated for this target \n",__func__);
    return EXIT_FAILURE;

}

void CameraParams::setBrightness(int value)
{
	CAM_PRINT("%s API is deprecated for this target \n",__func__);
	return;
}

vector<string> CameraParams::getSupportedToneMapMode() const
{
    vector<string> tonemap_modes;

	params_cast(priv_)->mutexLock();
	camera_metadata_entry_t entry;
	if (params_cast(priv_)->static_default_meta_info.exists(ANDROID_TONEMAP_AVAILABLE_TONE_MAP_MODES)) {
	   entry = params_cast(priv_)->static_default_meta_info.find(ANDROID_TONEMAP_AVAILABLE_TONE_MAP_MODES);
	   for (uint32_t i = 0 ; i < entry.count; i++) {
		 auto it = camToneMapModeEnum2StringMap.find(static_cast<camera_metadata_enum_android_tonemap_mode>(entry.data.u8[i]));
		if (it!=camToneMapModeEnum2StringMap.end()) {
			 CAM_ERR("%s:%d ToneMap mode %d currently supported.",__func__,__LINE__,entry.data.u8[i]);
			 tonemap_modes.emplace_back(it->second);
		 }else{
			 CAM_ERR("%s:%d ToneMap mode %d currently not supported.",__func__,__LINE__,entry.data.u8[i]);
		 }
	   }
	 }else{
	   CAM_ERR("%s:%d ANDROID_TONEMAP_AVAILABLE_TONE_MAP_MODES does not exist",__func__,__LINE__);
	}
	params_cast(priv_)->mutexUnlock();
    return tonemap_modes;
}

string CameraParams::getToneMapMode() const
{
    string str;
	camera_metadata_enum_android_tonemap_mode tonemap_mode;

      params_cast(priv_)->mutexLock();
	 /* Can do some camera params initialization here */
	 if ( (params_cast(priv_)->static_session_meta_info.exists(ANDROID_TONEMAP_MODE)) ){
	   tonemap_mode = static_cast<camera_metadata_enum_android_tonemap_mode >(params_cast(priv_)->static_session_meta_info.find(ANDROID_TONEMAP_MODE).data.u8[0]);
		auto it = camToneMapModeEnum2StringMap.find(tonemap_mode);
		if (it != camToneMapModeEnum2StringMap.end()) {
			str = it->second;
			CAM_ERR("%s:%s:%d Get ToneMap mode : %d - %s   \n",TAG,__func__,__LINE__,tonemap_mode,str.c_str());
		}else{
			CAM_ERR("%s:%d Unknown ToneMap mode: %d \n",__func__,__LINE__,tonemap_mode);
		}
	 }
	 params_cast(priv_)->mutexUnlock();
    return str;
}

void CameraParams::setToneMapMode(const string& value)
{
	uint8_t tonemap_mode;
	auto it = camString2ToneMapModeEnumMap.find(value);
	if (it != camString2ToneMapModeEnumMap.end()) {
		tonemap_mode = it->second;
		params_cast(priv_)->mutexLock();
		/* Can do some camera params initialization here */
		if ( (params_cast(priv_)->static_session_meta_info.exists(ANDROID_TONEMAP_MODE)) ){
		  //tonemap_mode = params_cast(priv_)->static_session_meta_info.find(ANDROID_TONEMAP_MODE).data.u8[0];
		  params_cast(priv_)->static_session_meta_info.update(ANDROID_TONEMAP_MODE, &tonemap_mode, 1);
		  CAM_ERR("%s:%s:%d Set ToneMap mode : %d - %s \n",TAG,__func__,__LINE__,tonemap_mode,value.c_str());
		  params_cast(priv_)->static_session_meta_info_updated_flag = true;
		}
		params_cast(priv_)->mutexUnlock();
	}else{
		CAM_ERR("%s:%d Invalid ToneMap mode: %s \n",__func__,__LINE__, value.c_str());
	}
	return;
}

Range CameraParams::getSupportedContrast() const
{
    Range range;
	CAM_PRINT("%s API is deprecated for this target \n",__func__);
    return range;
}
int CameraParams::getContrast() const
{
	CAM_PRINT("%s API is deprecated for this target \n",__func__);
	return EXIT_FAILURE;
}

void CameraParams::setContrast(int value)
{
	CAM_PRINT("%s API is deprecated for this target \n",__func__);
}

Tonemap_RBG CameraParams::getContrastTone() const
{
	params_cast(priv_)->mutexLock();
	Tonemap_RBG tonemapCurves;
    memset(&tonemapCurves,0,sizeof(Tonemap_RBG));

	if(params_cast(priv_)->static_session_meta_info.exists(ANDROID_TONEMAP_CURVE_BLUE) &&
		params_cast(priv_)->static_session_meta_info.exists(ANDROID_TONEMAP_CURVE_RED) &&
		params_cast(priv_)->static_session_meta_info.exists(ANDROID_TONEMAP_CURVE_GREEN))
	{

		CAM_ERR("%s:%d In ANDROID_TONEMAP_CURVE_GREEN\n",__func__,__LINE__);
		tonemapCurves.tonemap_points_cnt = (params_cast(priv_)->static_session_meta_info.find(ANDROID_TONEMAP_CURVE_GREEN).count)/2;
		CAM_ERR("%s:%d tonemap_points_cnt %d \n",__func__,__LINE__,tonemapCurves.tonemap_points_cnt);

		if (tonemapCurves.tonemap_points_cnt > CAM_MAX_TONEMAP_CURVE_SIZE) {

			CAM_ERR("%s:%d iWARN:tonemap_points_cnt %d exceeds max value of %d",
							__func__,__LINE__,
							tonemapCurves.tonemap_points_cnt,
							CAM_MAX_TONEMAP_CURVE_SIZE);
			tonemapCurves.tonemap_points_cnt = CAM_MAX_TONEMAP_CURVE_SIZE;
		}

		/* ch0 = G */
		size_t point = 0;
		cam_tonemap_curve_t tonemapCurveGreen;
		for (size_t i = 0; i < tonemapCurves.tonemap_points_cnt; i++) {
			for (size_t j = 0; j < 2; j++) {
			   tonemapCurveGreen.tonemap_points[i][j] =
				  params_cast(priv_)->static_session_meta_info.find(ANDROID_TONEMAP_CURVE_GREEN).data.f[point];
			   point++;
			}
		}
		tonemapCurves.curves[0] = tonemapCurveGreen;

		/* ch 1 = B */
		point = 0;
		cam_tonemap_curve_t tonemapCurveBlue;
		for (size_t i = 0; i < tonemapCurves.tonemap_points_cnt; i++) {
			for (size_t j = 0; j < 2; j++) {
			   tonemapCurveBlue.tonemap_points[i][j] =
				  params_cast(priv_)->static_session_meta_info.find(ANDROID_TONEMAP_CURVE_BLUE).data.f[point];
			   point++;
			}
		}
		tonemapCurves.curves[1] = tonemapCurveBlue;

		/* ch 2 = R */
		point = 0;
		cam_tonemap_curve_t tonemapCurveRed;
		for (size_t i = 0; i < tonemapCurves.tonemap_points_cnt; i++) {
			for (size_t j = 0; j < 2; j++) {
			   tonemapCurveRed.tonemap_points[i][j] =
				  params_cast(priv_)->static_session_meta_info.find(ANDROID_TONEMAP_CURVE_RED).data.f[point];
			   point++;
			}
		}
		tonemapCurves.curves[2] = tonemapCurveRed;
	} else {
		CAM_ERR("%s:%d WARN: No previous ANDROID_TONEMAP_MODE_CONTRAST_CURVE settings \n",__func__,__LINE__);
    }

	params_cast(priv_)->mutexUnlock();
	return tonemapCurves;
}

void CameraParams::setContrastTone(Tonemap_RBG& tonemap)
{
	params_cast(priv_)->mutexLock();

	if (tonemap.tonemap_points_cnt > CAM_MAX_TONEMAP_CURVE_SIZE) {
		CAM_ERR("%s:%d Error: tonemap_points_cnt %d exceeds max value of %d \n",
						__func__,__LINE__,
						tonemap.tonemap_points_cnt,
						CAM_MAX_TONEMAP_CURVE_SIZE);
		tonemap.tonemap_points_cnt = CAM_MAX_TONEMAP_CURVE_SIZE;
	}else if (tonemap.tonemap_points_cnt < 2 ) {
		CAM_ERR("%s:%d Error: tonemap_points_cnt %d < 2 ( min) \n",
				  __func__,__LINE__,
				  tonemap.tonemap_points_cnt);
	}else{
			params_cast(priv_)->static_session_meta_info.update(ANDROID_TONEMAP_CURVE_GREEN,
                        &tonemap.curves[0].tonemap_points[0][0],
                        (tonemap.tonemap_points_cnt * 2));

			params_cast(priv_)->static_session_meta_info.update(ANDROID_TONEMAP_CURVE_BLUE,
                        &tonemap.curves[1].tonemap_points[0][0],
                        (tonemap.tonemap_points_cnt * 2));

			params_cast(priv_)->static_session_meta_info.update(ANDROID_TONEMAP_CURVE_RED,
                        &tonemap.curves[2].tonemap_points[0][0],
                        (tonemap.tonemap_points_cnt * 2));
			params_cast(priv_)->static_session_meta_info_updated_flag = true;
	}
	params_cast(priv_)->mutexUnlock();
}

std::vector<Range> CameraParams::getSupportedPreviewFpsRanges() const
{
    std::vector<Range> ranges;

    camera_metadata_entry_t entry;
	 params_cast(priv_)->mutexLock();
	 if ((params_cast(priv_)->static_default_meta_info.exists(ANDROID_CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES))){
		 entry = params_cast(priv_)->static_default_meta_info.find(ANDROID_CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES);
		 CAM_DBG("%s:%d ANDROID_CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES count = %d ",__func__,__LINE__,entry.count);
	     for (size_t i = 0 ; i < entry.count; i += 2) {
	      CAM_DBG("%s:%d fps range = %d %d ",__func__,__LINE__,entry.data.i32[i], entry.data.i32[i+1]);
		  /* Multiply FPS * 1000 for backward compatibility of HAL1 */
		  ranges.push_back(Range((entry.data.i32[i]*1000),(entry.data.i32[i+1]*1000),1));
		 }
     }else{
		 CAM_ERR("%s:%d ANDROID_CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES does not exist",__func__,__LINE__);
	 }
    std::vector<Range> ranges2 = getHFRFpsRange();
    ranges.insert( ranges.end(), ranges2.begin(), ranges2.end() );
	 params_cast(priv_)->mutexUnlock();
    return ranges;
}

Range CameraParams::getPreviewFpsRange() const
{
    Range range;
    camera_metadata_entry_t entry;
	params_cast(priv_)->mutexLock();
	if ((params_cast(priv_)->static_default_meta_info.exists(ANDROID_CONTROL_AE_TARGET_FPS_RANGE))){
	 entry = params_cast(priv_)->static_default_meta_info.find(ANDROID_CONTROL_AE_TARGET_FPS_RANGE);
	 CAM_DBG("%s:%d ANDROID_CONTROL_AE_TARGET_FPS_RANGE count = %d ",__func__,__LINE__,entry.count);
	 if (entry.count >= 2) {
		 range.min = entry.data.i32[0] * 1000;   /* Multiply FPS * 1000 for backward compatibility of HAL1 */
		 range.max = entry.data.i32[0+1] * 1000;
		 range.step = 1;
	 }else{
		 CAM_ERR("%s:%d Error - Invalid FPS range",__func__,__LINE__);
	 }
	}else{
	  CAM_ERR("%s:%d ANDROID_CONTROL_AE_TARGET_FPS_RANGE does not exist",__func__,__LINE__);
	}
	params_cast(priv_)->mutexUnlock();
    return range;
}

std::vector<Range> CameraParams::getHFRFpsRange() const
{
    std::vector<Range> ranges;
    uint32_t last_fps = 0;
    uint32_t width_offset = 0;
    uint32_t height_offset = 1;
    uint32_t min_fps_offset = 2;
    uint32_t max_fps_offset = 3;
    uint32_t batch_size_offset = 4;
    uint32_t hfr_size = 5;

    bool hfr_supported_ = false;
    camera_metadata_entry meta_entry =
      params_cast(priv_)->static_default_meta_info.find(ANDROID_REQUEST_AVAILABLE_CAPABILITIES);
    for (uint32_t i = 0; i < meta_entry.count; ++i) {
    uint8_t caps = meta_entry.data.u8[i];
    if (ANDROID_REQUEST_AVAILABLE_CAPABILITIES_CONSTRAINED_HIGH_SPEED_VIDEO ==
        caps) {
      hfr_supported_ = true;
      break;
    }
    }
    if (!hfr_supported_) {
    return ranges;
    }

    meta_entry = params_cast(priv_)->static_default_meta_info.find(
      ANDROID_CONTROL_AVAILABLE_HIGH_SPEED_VIDEO_CONFIGURATIONS);
    for (uint32_t i = 0; i < meta_entry.count; i += hfr_size) {
        uint32_t width = meta_entry.data.i32[i + width_offset];
        uint32_t height = meta_entry.data.i32[i + height_offset];
        uint32_t min_fps = meta_entry.data.i32[i + min_fps_offset];
        uint32_t max_fps = meta_entry.data.i32[i + max_fps_offset];
        uint32_t batch = meta_entry.data.i32[i + batch_size_offset];
        if (min_fps == max_fps) { //Only constant framerates are supported
            CAM_ERR("%d, %d, %d, w %d, h %d, b %d", min_fps, max_fps, meta_entry.count, width, height, batch);
            if (last_fps != min_fps) {
                ranges.push_back(Range(min_fps*1000, max_fps*1000, 1));
                last_fps = min_fps;
            }
        }
    }
    return ranges;
}
void CameraParams::setPreviewFpsRange(const Range& value)
{
    bool success = false;
    camera_metadata_entry_t entry;
	params_cast(priv_)->mutexLock();
	if ((params_cast(priv_)->static_default_meta_info.exists(ANDROID_CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES))){
		entry = params_cast(priv_)->static_default_meta_info.find(ANDROID_CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES);
		CAM_INFO("%s:%d ANDROID_CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES count = %d ",__func__,__LINE__,entry.count);
		size_t i;
		for ( i = 0 ; i < entry.count; i += 2) {
		    if (value.min/1000 == entry.data.i32[i] && value.max/1000 == entry.data.i32[i+1]){
                params_cast(priv_)->track_params_ptr[CAMERA_QMMF_TRACK_PREVIEW]->frame_rate = value.max/1000;
                CAM_INFO("%s:%d Setting  fps range = %d %d ",__func__,__LINE__,entry.data.i32[i], entry.data.i32[i+1]);
                success = true;
			  break;
		  }
		}
		if (i >= entry.count) {
			CAM_ERR("%s:%d Error: preview range not valid for regular fps",__func__,__LINE__);
		}
	} else {
		CAM_ERR("%s:%d ANDROID_CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES does not exist",__func__,__LINE__);
	}
    if (! success) {
        std::vector<Range> ranges = getHFRFpsRange();
        for (size_t i = 0; i < ranges.size(); i ++)
        {
            if ((ranges[i].min == value.min) && (ranges[i].max == value.max))
            {
                success = true;
                params_cast(priv_)->track_params_ptr[CAMERA_QMMF_TRACK_PREVIEW]->frame_rate = value.max/1000;
                CAM_INFO("%s:%d Setting  fps(HFR) range = %d %d ",__func__,__LINE__,value.max/1000, value.max/1000);
                break;
            }
        }
    }

	params_cast(priv_)->mutexUnlock();
	return;
}

std::vector<VideoFPS> CameraParams::getSupportedVideoFps() const
{
	camera_metadata_entry_t entry;
	std::vector<Range> ranges;
	
	vector<VideoFPS> values;
	values.push_back(VIDEO_FPS_1X);

	params_cast(priv_)->mutexLock();
	int32_t currentPreviewFPS = params_cast(priv_)->track_params_ptr[CAMERA_QMMF_TRACK_PREVIEW]->frame_rate;
	if ((params_cast(priv_)->static_default_meta_info.exists(ANDROID_CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES))){
		 entry = params_cast(priv_)->static_default_meta_info.find(ANDROID_CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES);
		 CAM_INFO("%s:%d ANDROID_CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES count = %d ",__func__,__LINE__,entry.count);
	     for (size_t i = 0 ; i < entry.count; i += 2) {
			  if (entry.data.i32[i] == entry.data.i32[i+1]) {

				  CAM_INFO("%s:%d Possible fps = %d ",__func__,__LINE__,entry.data.i32[i]);
                  ranges.push_back(Range(entry.data.i32[i], entry.data.i32[i], 1));
              }
         }
    }else
	    CAM_ERR("%s:%d ANDROID_CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES does not exist",__func__,__LINE__);

    std::vector<Range> ranges2 = getHFRFpsRange();
    for (size_t i = 0; i < ranges2.size(); i ++) {
        ranges2[i].min = ranges2[i].min/1000;
        ranges2[i].max = ranges2[i].max/1000;
		CAM_INFO("%s:%d Possible fps = %d ",__func__,__LINE__, ranges2[i].max);
    }
    ranges.insert( ranges.end(), ranges2.begin(), ranges2.end());
    for (size_t i = 0; i < ranges.size(); i ++) {
          int fps = ranges[i].max;
   	      CAM_INFO("%s:%d currentPreviewFPS %d, fps %d",__func__,__LINE__, currentPreviewFPS, fps);
          if (fps < currentPreviewFPS) continue;
    	  VideoFPS possibleVideoFPSXX = static_cast<VideoFPS>( fps / currentPreviewFPS);

				  /* check if element exist */
		  size_t j;
		  for (j = 0 ; j < values.size() ; j++) {
					  if (values[i] == possibleVideoFPSXX  ) {
						  break;
					  }
				  }
		  if ( j < values.size()) {
					  continue;
				  }
				  
				  switch (possibleVideoFPSXX) {
     		  CAM_INFO("%s:%d Add fps = %d ",__func__,__LINE__,entry.data.i32[i]);
					  case VIDEO_FPS_1X:
						  values.push_back(VIDEO_FPS_1X);
						  break;
					  case VIDEO_FPS_2X:
						  values.push_back(VIDEO_FPS_2X);
						  break;
					  case VIDEO_FPS_3X:
						  values.push_back(VIDEO_FPS_3X);
						  break;
					  case VIDEO_FPS_4X:
						  values.push_back(VIDEO_FPS_4X);
						  break;
					  case VIDEO_FPS_5X:
						  values.push_back(VIDEO_FPS_5X);
						  break;
					  default:
						break;
				  }
			  }

    params_cast(priv_)->mutexUnlock(); 
    return values;
}

VideoFPS CameraParams::getVideoFPS() const
{
    VideoFPS ret;
	params_cast(priv_)->mutexLock();
	ret = params_cast(priv_)->video_FPS_XX;
	params_cast(priv_)->mutexUnlock();
    return ret;
}

void CameraParams::setVideoFPS(VideoFPS value)
{
    CAM_INFO("setVideoFPS %d", value);
	params_cast(priv_)->mutexLock();
	params_cast(priv_)->video_FPS_XX = value;
	params_cast(priv_)->mutexUnlock();
	return;
}

string CameraParams::toString() const
{
    string tempReturn;
	CAM_PRINT("%s API is deprecated for this target \n",__func__);
    return tempReturn;
}

vector<string> CameraParams::getSupportedPreviewFormats() const
{
    vector<string> formats;

	params_cast(priv_)->mutexLock();
	camera_metadata_entry_t entry;
	if (params_cast(priv_)->static_default_meta_info.exists(ANDROID_SCALER_AVAILABLE_FORMATS)) {
	   entry = params_cast(priv_)->static_default_meta_info.find(ANDROID_SCALER_AVAILABLE_FORMATS);
	   for (uint32_t i = 0 ; i < entry.count; i++) {

		 CAM_DBG(" previewFormat : %d - %x \n",i,entry.data.i32[i]);
         /* Check only format supported by qmmf-sdk */
		 if (entry.data.i32[i] == ANDROID_SCALER_AVAILABLE_FORMATS_YCbCr_420_888 ) {
			 formats.emplace_back(FORMAT_NV12_VENUS);
		 }else if (entry.data.i32[i] == HAL_PIXEL_FORMAT_RAW10){
			  formats.emplace_back(FORMAT_RAW10);
		 }else if (entry.data.i32[i] == HAL_PIXEL_FORMAT_RAW12){
			  formats.emplace_back(FORMAT_RAW12);
		 }

	   }
	 }else{
	   CAM_ERR("%s:%d ANDROID_SCALER_AVAILABLE_FORMATS does not exist",__func__,__LINE__);
	}
	params_cast(priv_)->mutexUnlock();
    return formats;
}

string CameraParams::getPreviewFormat() const
{
    string format;

	if (params_cast(priv_)->track_params_ptr[CAMERA_QMMF_TRACK_PREVIEW]->format_type == VideoFormat::kBayerRDI12BIT) {
		format = FORMAT_RAW12;
	 } else if (params_cast(priv_)->track_params_ptr[CAMERA_QMMF_TRACK_PREVIEW]->format_type == VideoFormat::kBayerRDI10BIT) {
		 format = FORMAT_RAW10;
	 } else if (params_cast(priv_)->track_params_ptr[CAMERA_QMMF_TRACK_PREVIEW]->format_type == qmmf::VideoFormat::kYUV) {
		 format = FORMAT_NV12_VENUS;
	 } else{
		 CAM_ERR("%s:%d Error : format not in list \n",__func__,__LINE__);
	 }

    return format;
}

void CameraParams::setPreviewFormat(const string& value)
{
	params_cast(priv_)->mutexLock();
	if (FORMAT_RAW12 == value) {
		params_cast(priv_)->track_params_ptr[CAMERA_QMMF_TRACK_PREVIEW]->format_type = VideoFormat::kBayerRDI12BIT;
	} else if (FORMAT_RAW10 == value) {
		params_cast(priv_)->track_params_ptr[CAMERA_QMMF_TRACK_PREVIEW]->format_type = VideoFormat::kBayerRDI10BIT;
	} else if (FORMAT_NV12_VENUS == value) {
		params_cast(priv_)->track_params_ptr[CAMERA_QMMF_TRACK_PREVIEW]->format_type = qmmf::VideoFormat::kYUV;
	} else{
		CAM_ERR("%s:%d Error : unsupport format \n",__func__,__LINE__);
	}
	params_cast(priv_)->mutexUnlock();
	return;
}

void CameraParams::setPictureFormat(const string& value)
{
	params_cast(priv_)->mutexLock();
	if (FORMAT_RAW10 == value) {
		params_cast(priv_)->snapshot_image_param.image_format = ImageFormat::kBayerRDI10BIT;
	} else if (FORMAT_JPEG == value) {
		params_cast(priv_)->snapshot_image_param.image_format = ImageFormat::kJPEG;
	} else if (FORMAT_NV12 == value) {
		params_cast(priv_)->snapshot_image_param.image_format = ImageFormat::kNV12;
	} else{
		CAM_ERR("%s:%d Error : unsupport format \n",__func__,__LINE__);
	}
	params_cast(priv_)->mutexUnlock();
	return;
}

void CameraParams::setManualExposure(int value)
{
    static uint8_t aeMode;
    static int64_t exposure_time;

	params_cast(priv_)->mutexLock();
    /* Can do some camera params initialization here */
    if ( (params_cast(priv_)->static_session_meta_info.exists(ANDROID_CONTROL_AE_MODE)) && 
         (params_cast(priv_)->static_session_meta_info.exists(ANDROID_SENSOR_EXPOSURE_TIME))
        ) {
          aeMode = params_cast(priv_)->static_session_meta_info.find(ANDROID_CONTROL_AE_MODE).data.u8[0];
          exposure_time = params_cast(priv_)->static_session_meta_info.find(ANDROID_SENSOR_EXPOSURE_TIME).data.i64[0];

          aeMode = 0;
          exposure_time = static_cast<int64_t>(value);

          params_cast(priv_)->static_session_meta_info.update(ANDROID_CONTROL_AE_MODE, &aeMode, 1);
          params_cast(priv_)->static_session_meta_info.update(ANDROID_SENSOR_EXPOSURE_TIME, &exposure_time, 1);
          CAM_INFO("%s:%s:%d Set manual exposure : %lld   ",TAG,__func__,__LINE__,exposure_time);
		  params_cast(priv_)->static_session_meta_info_updated_flag = true;
    }else{
       CAM_ERR("%s:%s:%d Failed enabling manual exposure\n", TAG, __func__,__LINE__);
    }
	params_cast(priv_)->mutexUnlock();
	return;
}

int CameraParams::getExposureTime() const
{
    CAM_ERR("%s:%d getExposureTime is not implemented\n",__func__,__LINE__);
    return 0;
}

void CameraParams::setExposureTime(const string& value)
{
    CAM_DBG("%s:%d setExposureTime is not implemented\n",__func__,__LINE__);
    return;
}


void CameraParams::setManualGain(int value)
{
    static uint8_t aeMode;
    static int32_t sensitivity;

	params_cast(priv_)->mutexLock();
	/* Can do some camera params initialization here */
    if ( (params_cast(priv_)->static_session_meta_info.exists(ANDROID_CONTROL_AE_MODE)) && 
       (params_cast(priv_)->static_session_meta_info.exists(ANDROID_SENSOR_SENSITIVITY))
      ) {
        aeMode = params_cast(priv_)->static_session_meta_info.find(ANDROID_CONTROL_AE_MODE).data.u8[0];
        sensitivity = params_cast(priv_)->static_session_meta_info.find(ANDROID_SENSOR_SENSITIVITY).data.i32[0];

        aeMode = 0;
        sensitivity = static_cast<int32_t>(value) ; 
        
        params_cast(priv_)->static_session_meta_info.update(ANDROID_CONTROL_AE_MODE, &aeMode, 1);
        params_cast(priv_)->static_session_meta_info.update(ANDROID_SENSOR_SENSITIVITY, &sensitivity, 1);
        CAM_INFO("%s:%s:%d Set manual gain : %d   ",TAG,__func__,__LINE__,sensitivity);

		params_cast(priv_)->static_session_meta_info_updated_flag = true;

  }else{
     CAM_ERR("%s:%s:%d Failed enabling manual gain \n", TAG, __func__,__LINE__);
  }
  params_cast(priv_)->mutexUnlock();
}

Range64 CameraParams::getManualExposureRange(ImageSize size, int fps){
	Range64 range;

	params_cast(priv_)->mutexLock();
	camera_metadata_entry_t  entry = params_cast(priv_)->static_default_meta_info.find(ANDROID_SENSOR_INFO_EXPOSURE_TIME_RANGE);
	if (entry.count >= 2) {
		range.min = entry.data.i64[0];
		range.max = entry.data.i64[1];
		CAM_DBG("%s:%d ANDROID_SENSOR_INFO_EXPOSURE_TIME_RANGE min = %lld , max = %lld \n",__func__,__LINE__, range.min,range.max]);
	}else{
		CAM_ERR("%s:%d ANDROID_SENSOR_INFO_EXPOSURE_TIME_RANGE does not exist \n",__func__,__LINE__);
	}

	uint64_t  maxPossibleExposurse = NANOSEC_PER_SEC/ fps;
	if ( range.max > maxPossibleExposurse ){
		range.max  = maxPossibleExposurse;
	}

	params_cast(priv_)->mutexUnlock();
	return range;
}

Range CameraParams::getManualGainRange(ImageSize size, int fps){
	Range range;
	params_cast(priv_)->mutexLock();
	camera_metadata_entry_t  entry = params_cast(priv_)->static_default_meta_info.find(ANDROID_SENSOR_INFO_SENSITIVITY_RANGE);
	if (entry.count >= 2) {
		range.min = entry.data.i32[0];
		range.max = entry.data.i32[1];
		CAM_DBG("%s:%d ANDROID_SENSOR_INFO_SENSITIVITY_RANGE min = %lld , max = %lld \n",__func__,__LINE__, range.min,range.max]);
	}else{
		CAM_ERR("%s:%d ANDROID_SENSOR_INFO_SENSITIVITY_RANGE does not exist \n",__func__,__LINE__);
	}
	params_cast(priv_)->mutexUnlock();
	return range;
}

void CameraParams::setVerticalFlip(bool value)
{
	CAM_PRINT("%s API is deprecated for this target \n",__func__);
}

void CameraParams::setHorizontalMirror(bool value)
{
	CAM_PRINT("%s API is deprecated for this target \n",__func__);
}

void CameraParams::setAntibanding(const string& value)
{
    CAM_PRINT("%s API is not available for this target \n",__func__);
    return;
}

void CameraParams::setStatsLoggingMask(int value)
{
	CAM_PRINT("%s API is deprecated for this target \n",__func__);
}

uint64_t CameraParams::getFrameExposureTime(ICameraFrame* frame)
{

   CameraMetadata *result = static_cast<qmmfCameraFrame *>(frame)->getFrameInfoMetadataPtr();

    auto entry = result->find(ANDROID_SENSOR_EXPOSURE_TIME);
    if (0 < entry.count) {
        return *entry.data.i64;
    }else{
        CAM_ERR("%s:%d ANDROID_SENSOR_EXPOSURE_TIME does not exist \n",__func__,__LINE__);
        return 0;
    }
}

int32_t CameraParams::getFrameGainValue(ICameraFrame* frame)
{

   CameraMetadata *result = static_cast<qmmfCameraFrame *>(frame)->getFrameInfoMetadataPtr();

    auto entry = result->find(ANDROID_SENSOR_SENSITIVITY);
    if (0 < entry.count) {
        return static_cast<int32_t>(*entry.data.i32);
    }else{
        CAM_ERR("%s:%d ANDROID_SENSOR_SENSITIVITY does not exist \n",__func__,__LINE__);
        return 0;
    }
}

uint64_t CameraParams::getFrameRollingShutterSkew(ICameraFrame* frame)
{

   CameraMetadata *result = static_cast<qmmfCameraFrame *>(frame)->getFrameInfoMetadataPtr();

    auto entry = result->find(ANDROID_SENSOR_ROLLING_SHUTTER_SKEW);
    if (0 < entry.count) {
        return *entry.data.i64;
    }else{
        CAM_ERR("%s:%d ANDROID_SENSOR_ROLLING_SHUTTER_SKEW does not exist \n",__func__,__LINE__);
        return 0;
    }
}

uint64_t CameraParams::getFrameReadoutTimestamp(ICameraFrame* frame)
{

   CameraMetadata *result = static_cast<qmmfCameraFrame *>(frame)->getFrameInfoMetadataPtr();

    auto entry = result->find(QCAMERA3_SENSOR_START_FRAME_READOUT);
    if (0 < entry.count) {
        return *entry.data.i64;
    }else{
        CAM_ERR("%s:%d ANDROID_SENSOR_ROLLING_SHUTTER_SKEW does not exist \n",__func__,__LINE__);
        return 0;
	}
}

uint64_t CameraParams::getFrameReadoutDuration(ICameraFrame* frame)
{

   CameraMetadata *result = static_cast<qmmfCameraFrame *>(frame)->getFrameInfoMetadataPtr();

    auto entry = result->find(QCAMERA3_SENSOR_FRAME_READOUT_DURATION);
    if (0 < entry.count) {
        return *entry.data.i64;
    }else{
		CAM_ERR("%s:%d QCAMERA3_SENSOR_FRAME_READOUT_DURATION does not exist \n",__func__,__LINE__);
		return 0;
	}
}

} /* namespace camera */

