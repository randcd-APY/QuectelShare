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
#include <hardware/camera.h>
#include <system/camera.h>
#include <dlfcn.h>
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <utils/Log.h>
#include <utils/String8.h>
#include <assert.h>
#include <system/graphics.h>
#include <QCamera3VendorTags.h>
#include <iostream>
#include <time.h>
#include <sys/param.h>
#include <map>
#include <vector>

#include "camera_log.h"
#include "camera.h"
#include "qmmf_camera.h"

//#define CAM_DEBUG_ENABLE
#ifdef CAM_DEBUG_ENABLE
#undef CAM_INFO
#define CAM_INFO CAM_PRINT
#undef CAM_ERR
#define CAM_ERR CAM_PRINT
#undef CAM_DBG
#define CAM_DBG CAM_PRINT
#endif

#ifdef LIBCAM_LOG_LEVEL_KPI
uint32_t libcam_kpi_debug_mask = LIBCAM_KPI_DISABLE;
// Dump four frames timestamp in each minute
uint32_t libcam_kpi_round_frame_cnt = 1800;
static uint32_t preview_frame_cnt = 0;
static uint32_t video_frame_cnt = 0;
static uint32_t picture_frame_cnt = 0;
#endif

using namespace std;

namespace camera
{

const char* CAMERA_HAL_LIB_NAME = "/usr/lib/hw/camera.msm8996.so";
const int ID_STR_LEN = 16;

static camera_module_t* g_halModule = NULL;
static pthread_mutex_t halMutex = PTHREAD_MUTEX_INITIALIZER;

static vector<int> g_openCameras;

/* load module, if not already loaded */
static camera_module_t* getCameraHalModule()
{
    /* serialize access to hal module using mutex */
    pthread_mutex_lock(&halMutex);
    if (NULL == g_halModule) {
        /* load the module library */
        void* handle = dlopen(CAMERA_HAL_LIB_NAME, RTLD_NOW);
        if (NULL != handle) {
            camera_module_t* module = (camera_module_t*)
                dlsym(handle, HAL_MODULE_INFO_SYM_AS_STR);
            if (NULL == dlerror()) {
                g_halModule = module;
            } else {
                CAM_ERR("dlsym failed for %s:%s", CAMERA_HAL_LIB_NAME,
                        HAL_MODULE_INFO_SYM_AS_STR);
                dlclose(handle);
            }
        } else {
            CAM_ERR("dlopen failed for %s, %s", CAMERA_HAL_LIB_NAME, dlerror());
        }
    }
    pthread_mutex_unlock(&halMutex);
    return g_halModule;
}

/**
 * check if given camera ID is already open
 *
 * @param id
 *
 * @return bool
 */
static bool isOpen(int id)
{
    bool ret = false;
    pthread_mutex_lock(&halMutex);
    for (unsigned int i=0; i < g_openCameras.size(); i++) {
        if (g_openCameras[i] == id) {
            ret = true;
            break;
        }
    }
    pthread_mutex_unlock(&halMutex);
    return ret;
}

int getNumberOfCameras()
{
    camera_module_t* mod = getCameraHalModule();
    if (!mod) {
        CAM_ERR("failed");
        return -ELIBACC;
    }
    return mod->get_number_of_cameras(); 
}

int getCameraInfo(int id, struct CameraInfo& info)
{
    struct camera_info priv_info;
    camera_module_t* mod = getCameraHalModule();
    if (!mod) {
        CAM_ERR("failed");
        return ELIBACC;
    }
    mod->get_camera_info(id, &priv_info);
    info.func = priv_info.facing;
    return 0;
}

#if 0
inline static EventType toEventType(int32_t msgType)
{
    switch (msgType) {
      case CAMERA_MSG_FOCUS:
          return CAMERA_EVT_FOCUS;
      default:
          return CAMERA_EVT_NONE;
    }
}
#endif 
/* convert camera event bitmask to device message type bitmask */
inline static int32_t toDeviceMsgType(uint32_t eventMask)
{
    int32_t msgType = 0x00;
//    if (eventMask & CAMERA_EVT_FOCUS) {
//        msgType |= CAMERA_MSG_FOCUS;
//    }
    return msgType;
}

std::map<int,qmmfCameraFrame*> qmmfCameraFrame::refs_fd_frame_map_;
std::mutex qmmfCameraFrame::refs_map_mutex_;

qmmfCameraFrame::~qmmfCameraFrame()
{
	status_t ret;

	// Return buffer back to recorder service.
	if (type_ == CAMERA_QMMF_TRACK_PREVIEW || type_ == CAMERA_QMMF_TRACK_VIDEO) {
	   ret = recorder_->ReturnTrackBuffer(this->session_id_,this->track_id_,this->buffers_);
	 } else if (type_ == CAMERA_QMMF_TRACK_PICTURE ) {
		ret = recorder_->ReturnImageCaptureBuffer(this->camera_id_, this->buffer_);
	}
	if (ret != NO_ERROR) {
		CAM_ERR("%s:%d Return image buffer failed - %d",__func__,__LINE__,ret);
	}	

}

uint32_t qmmfCameraFrame::acquireRef()
{
	qmmfCameraFrame::refs_map_mutex_.lock();
	qmmfCameraFrame::refs_fd_frame_map_.emplace(this->fd,this);
	qmmfCameraFrame::refs_map_mutex_.unlock();
	refs_mutex_.lock();
	refs_++;
	refs_mutex_.unlock();
	CAM_DBG("%s:%d TS = %lld ref = %d",__func__,__LINE__,this->timeStamp, refs_);
    return refs_;
}

uint32_t qmmfCameraFrame::releaseRef()
{

	refs_mutex_.lock();
    if (refs_ <= 0) {
        refs_ = 0;
		goto bail;
    }

    refs_--;
    if (0 == refs_) {
		CAM_DBG("%s:%d Delete TS = %lld ref = %d ", __func__, __LINE__, this->timeStamp, refs_);
		goto delete_frame;
    }

bail:
	CAM_DBG("%s:%d TS = %lld ref = %d ",__func__,__LINE__,this->timeStamp, refs_);
	refs_mutex_.unlock();
    return refs_;

delete_frame:
	qmmfCameraFrame::refs_map_mutex_.lock();
	std::map<int,qmmfCameraFrame*>::iterator it = qmmfCameraFrame::refs_fd_frame_map_.find(this->fd);
	if (it != qmmfCameraFrame::refs_fd_frame_map_.end()) {
		qmmfCameraFrame::refs_fd_frame_map_.erase(it);
	}else{
		CAM_DBG("%s:%d - WARN: Frame not found in ref_fd_frame_map ",__func__,__LINE__);
		qmmfCameraFrame::refs_map_mutex_.unlock();
		refs_mutex_.unlock();
		return 0;
	}
	qmmfCameraFrame::refs_map_mutex_.unlock();
	refs_mutex_.unlock();
	delete this;
	return 0;
}

status_t releaseAllFrames(){
 
  size_t refsFDFrameMapSize;
  int wait_count = 0;

  qmmfCameraFrame::refs_map_mutex_.lock();
  refsFDFrameMapSize = qmmfCameraFrame::refs_fd_frame_map_.size();
  qmmfCameraFrame::refs_map_mutex_.unlock();

  while (refsFDFrameMapSize > 0) {
	CAM_DBG("WARN: Waiting for buffers (%d) to be released \n",refsFDFrameMapSize);
	usleep(RELEASE_REF_BUF_WAIT_US);
	wait_count++;
	if (wait_count > RELEASE_REF_BUF_WAIT_MAX_ATTEMPT) {
		qmmfCameraFrame::refs_map_mutex_.lock();
		for ( std::map<int,qmmfCameraFrame*>::iterator it = qmmfCameraFrame::refs_fd_frame_map_.begin(); it != qmmfCameraFrame::refs_fd_frame_map_.end() ; it++ )
		{
			CAM_ERR("WARN: Force release pending buffers : FD = %d", it->first );
			delete it->second;
			qmmfCameraFrame::refs_fd_frame_map_.erase(it);
		}
		qmmfCameraFrame::refs_map_mutex_.unlock();
		break;
	}
	qmmfCameraFrame::refs_map_mutex_.lock();
	refsFDFrameMapSize = qmmfCameraFrame::refs_fd_frame_map_.size();
	qmmfCameraFrame::refs_map_mutex_.unlock();
  } 
	 
  return EXIT_SUCCESS;
}

void qmmfCameraFrame::qmmf_camera_kpi_atrace(uint32_t camera_id,qmmfCameraFrame *frame) {

    char log_buffer[150];
    uint32_t frame_number;

    switch (frame->track_id_) {
      case CAMERA_QMMF_TRACK_PREVIEW:
        frame_number = frame->meta_buffers_[0].cam_meta_frame_number;
        ::snprintf(log_buffer,
                   150,
                   "%s:camera id %d,frame number %d,CAMERA_FRAME_PREVIEW,exposure timestamp %lld",
                   __func__,camera_id,frame_number,frame->timeStamp);
        LIBCAM_KPI_DUMP_FRAME_TS(preview_frame_cnt, log_buffer);
        break;
      case CAMERA_QMMF_TRACK_VIDEO:
        frame_number = frame->meta_buffers_[0].cam_meta_frame_number;
        ::snprintf(log_buffer,
                   150,
                   "%s,:camera id %d,frame number %d,CAMERA_FRAME_VIDEO,exposure timestamp %lld",
                   __func__,camera_id,frame_number,frame->timeStamp);
        LIBCAM_KPI_DUMP_FRAME_TS(video_frame_cnt, log_buffer);
        break;
      case CAMERA_QMMF_TRACK_PICTURE:
        frame_number = frame->meta_buffer_.cam_meta_frame_number;
        ::snprintf(log_buffer,
                   150,
                   "%s:camera id %d,frame number %d,CAMERA_FRAME_SNAPSHOT,exposure timestamp %lld",
                   __func__,camera_id,frame_number,frame->timeStamp);
        LIBCAM_KPI_DUMP_FRAME_TS(picture_frame_cnt, log_buffer);
        break;
      default:
        ::snprintf(log_buffer,
                   150,
                   "%s:camera id %d,frame number %d,unsupported track_id %d",
                   __func__,camera_id,frame_number,frame->track_id_);
        LIBCAM_KPI_DUMP_FRAME_TS(picture_frame_cnt, log_buffer);
    }
}


void qmmfCameraFrame::dispatchFrame(std::vector<ICameraListener *> listeners,
                                  qmmf::recorder::Recorder *recorder,
                                  std::vector<BufferDescriptor> &buffers, 
									std::vector<qmmf::recorder::MetaData> &meta_buffers, 
                                  CameraMetadata &frameMetadata,
								  uint32_t camera_id,
                                  uint32_t  session_id, uint32_t  track_id)
{
    if (meta_buffers.size()==0 || buffers.size()==0 ) {
        CAM_ERR("%s:%d Error : buffer is empty ",__func__, __LINE__);
        return;
    }

    qmmfCameraFrame *frame = new qmmfCameraFrame(buffers.begin()->fd,buffers.begin()->size);
	if (frame == NULL) {
		CAM_ERR("%s:%d new frame memory allocation failed",__func__,__LINE__);
		return;
	}

    frame->recorder_ = recorder;
    frame->buffers_ = buffers;
    frame->meta_buffers_ = meta_buffers;
	frame->frame_info_metadata_ = frameMetadata;
    frame->session_id_ = session_id;
	frame->camera_id_ = camera_id;
    frame->track_id_ = track_id;
	frame->buf_id_ = buffers.begin()->buf_id;


    frame->timeStamp = buffers.begin()->timestamp;;
    frame->data = (uint8_t*)buffers.begin()->data;
    frame->size = buffers.begin()->size;
    frame->fd = buffers.begin()->fd;
    frame->metadata = &(frame->metadata_);

    frame->acquireRef();

#ifdef LIBCAM_LOG_LEVEL_KPI
    qmmfCameraFrame::qmmf_camera_kpi_atrace(camera_id,frame);
#endif

	for (unsigned int i=0; i < listeners.size(); i++) {
		switch (frame->track_id_) {
		  case CAMERA_QMMF_TRACK_PREVIEW:
			  frame->type_ = CAMERA_QMMF_TRACK_PREVIEW;
	#ifdef LIBCAM_LOG_LEVEL_KPI
			  char log_buffer_p[100];
			  ::snprintf(log_buffer_p, 100, "camera id(%d) dispatchFrame:CAMERA_FRAME_PREVIEW ",camera_id);
			  LIBCAM_KPI_DUMP_FRAME_TS(preview_frame_cnt, log_buffer_p);
	#endif
			  listeners[i]->onPreviewFrame(static_cast<ICameraFrame*>(frame));
			  break;
		  case CAMERA_QMMF_TRACK_VIDEO:
			  frame->type_ = CAMERA_QMMF_TRACK_VIDEO;
	#ifdef LIBCAM_LOG_LEVEL_KPI
			  char log_buffer_v[100];
			  ::snprintf(log_buffer_v, 100, "camera id(%d) dispatchFrame:CAMERA_FRAME_VIDEO ",camera_id);
			  LIBCAM_KPI_DUMP_FRAME_TS(video_frame_cnt, log_buffer_v);
	#endif
			  listeners[i]->onVideoFrame(static_cast<ICameraFrame*>(frame));
			  break;
		  case CAMERA_QMMF_TRACK_PICTURE:
			  frame->type_ = CAMERA_QMMF_TRACK_PICTURE;
	#ifdef LIBCAM_LOG_LEVEL_KPI
			  char log_buffer_s[100];
			  ::snprintf(log_buffer_s, 100, "camera id(%d) dispatchFrame:CAMERA_FRAME_SNAPSHOT ",camera_id);
			  LIBCAM_KPI_DUMP_FRAME_TS(picture_frame_cnt, log_buffer_s);
	#endif
			  listeners[i]->onPictureFrame(static_cast<ICameraFrame*>(frame));
			  break;
		  default:
			  CAM_ERR("unsupported track_id %d", track_id);
		}
	}
    frame->releaseRef();
}


void qmmfCameraFrame::dispatchFrame(std::vector<ICameraListener *> listeners,
							   qmmf::recorder::Recorder *recorder,
							   BufferDescriptor &buffer, 
							   qmmf::recorder::MetaData &meta_buffer, 
							   CameraMetadata &frameMetadata,
							   uint32_t camera_id,
							   uint32_t  session_id, uint32_t  track_id)
{
    qmmfCameraFrame *frame = new qmmfCameraFrame(buffer.fd,buffer.size);
	if (frame == NULL) {
		CAM_ERR("%s:%d new frame memory allocation failed",__func__,__LINE__);
		return;
	}
    frame->recorder_ = recorder;
    frame->buffer_ = buffer;
    frame->meta_buffer_ = meta_buffer;
	frame->frame_info_metadata_ = frameMetadata;
	frame->camera_id_ = camera_id;
    frame->session_id_ = session_id;
    frame->track_id_ = track_id; 
	frame->buf_id_ = buffer.buf_id;

    frame->timeStamp = buffer.timestamp;;
    frame->data = (uint8_t*)buffer.data;
    frame->size = buffer.size;
    frame->fd = buffer.fd;
    frame->metadata = &(frame->metadata_);

    frame->acquireRef();

#ifdef LIBCAM_LOG_LEVEL_KPI
    qmmfCameraFrame::qmmf_camera_kpi_atrace(camera_id,frame);
#endif

	for (unsigned int i=0; i < listeners.size(); i++) {
		switch (frame->track_id_) {
		  case CAMERA_QMMF_TRACK_PREVIEW:
			  frame->type_ = CAMERA_QMMF_TRACK_PREVIEW;
	#ifdef LIBCAM_LOG_LEVEL_KPI
			  char log_buffer_p[100];
			  ::snprintf(log_buffer_p, 100, "camera id(%d) dispatchFrame:CAMERA_FRAME_PREVIEW ",camera_id);
			  LIBCAM_KPI_DUMP_FRAME_TS(preview_frame_cnt, log_buffer_p);
	#endif
			  listeners[i]->onPreviewFrame(static_cast<ICameraFrame*>(frame));
			  break;
		  case CAMERA_QMMF_TRACK_VIDEO:
			  frame->type_ = CAMERA_QMMF_TRACK_VIDEO;
	#ifdef LIBCAM_LOG_LEVEL_KPI
			  char log_buffer_v[100];
			  ::snprintf(log_buffer_v, 100, "camera id(%d) dispatchFrame:CAMERA_FRAME_VIDEO ",camera_id);
			  LIBCAM_KPI_DUMP_FRAME_TS(video_frame_cnt, log_buffer_v);
	#endif
			  listeners[i]->onVideoFrame(static_cast<ICameraFrame*>(frame));
			  break;
		  case CAMERA_QMMF_TRACK_PICTURE:
			  frame->type_ = CAMERA_QMMF_TRACK_PICTURE;
	#ifdef LIBCAM_LOG_LEVEL_KPI
			  char log_buffer_s[100];
			  ::snprintf(log_buffer_s, 100, "camera id(%d) dispatchFrame:CAMERA_FRAME_SNAPSHOT ",camera_id);
			  LIBCAM_KPI_DUMP_FRAME_TS(picture_frame_cnt, log_buffer_s);
	#endif
			  listeners[i]->onPictureFrame(static_cast<ICameraFrame*>(frame));
			  break;
		  default:
			  CAM_ERR("unsupported track_id %d", track_id);
		}
	}
    frame->releaseRef();
}

qmmfCAMERA::qmmfCAMERA() :
    isPreviewRunning_(false),
    isVideoRunning_(false),
	session_enabled_(false),
    camera_error_(false)
{
}

int qmmfCAMERA::init(int idx)
{

    status_t ret = EXIT_SUCCESS;

    camera_id_ = idx;

    ret = qmmfRecorderConnect();
    if (ret != NO_ERROR){
        CAM_ERR("%s:%d qmmfRecorderConnect failed - %d ",__func__,__LINE__,ret);
        goto bail;
    }

    ret = qmmfStartCamera();
    if (ret != NO_ERROR){
        CAM_ERR("%s:%d qmmfStartCamera failed - %d ",__func__,__LINE__,ret);
        goto bail;
    }
    ret = qmmfCreateSession();
    if (ret != NO_ERROR){
        CAM_ERR("%s:%d qmmfStartCamera failed - %d ",__func__,__LINE__,ret);
        goto bail;
    }

	for (uint32_t i = CAMERA_QMMF_TRACK_PREVIEW; i < CAMERA_QMMF_TRACK_MAX ; i++  ) {
		qmmfTrackArray_[i].init(this,&recorder_,camera_id_, session_id_,i, \
								&(qmmf_camera_params_.track_params_ptr[i]));
	}

	  //ToDo: Remove starting preview during qmmfCAMERA init and delete track in startPreview function.
	// This is work around for the limitation that we cannot call GetCameraParam in
	// qmmfCAMERA init function without creating a track. 
	qmmfTrackArray_[CAMERA_QMMF_TRACK_PREVIEW].qmmfCreateTrack();

	ret = recorder_.GetCameraParam(camera_id_, qmmf_camera_params_.static_session_meta_info);
	if (NO_ERROR != ret) {
		CAM_ERR("%s:%d Unable to query default capture parameters!\n",__func__,__LINE__);		  
	} 

	qmmfTrackArray_[CAMERA_QMMF_TRACK_PREVIEW].qmmfDeleteTrack();

bail:
    return ret;
}

qmmfCAMERA::~qmmfCAMERA()
{

	status_t ret = qmmfTrackArray_[CAMERA_QMMF_TRACK_PREVIEW].qmmfDeleteTrack();
	if (ret != NO_ERROR){
        CAM_ERR("%s:%d qmmfDeleteTrack failed - %d ",__func__,__LINE__,ret);
	}

    //ToDo: Check how we can notify user about failure
    qmmfDeleteSession();
    qmmfStopCamera();
    qmmfRecorderDisconnect();
}


int qmmfCAMERA::setParameters(const ICameraParameters& params)
{

    status_t ret = EXIT_SUCCESS;
    int32_t status;

	qmmf_camera_params_.mutexLock();
	if (qmmf_camera_params_.static_session_meta_info_updated_flag == false ) {
		goto bail;
	}else{
		qmmf_camera_params_.static_session_meta_info_updated_flag = false;
	}

    status = recorder_.SetCameraParam(camera_id_, qmmf_camera_params_.static_session_meta_info);
    if (NO_ERROR != status) {
        CAM_ERR("%s:%s:%d Failed \n", TAG, __func__,__LINE__);
        ret = EXIT_FAILURE;
    }else{
        CAM_DBG("%s:%s:%d Success \n",TAG,__func__,__LINE__);
    }

bail:
	qmmf_camera_params_.mutexUnlock();
    return ret;
}

int qmmfCAMERA::getParameters(uint8_t* buf, uint32_t bufSize, int* bufSizeRequired)
{
    return EXIT_FAILURE;
}

void* qmmfCAMERA::getParameters()
{
    return &qmmf_camera_params_;
}

void qmmfCAMERA::addListener(ICameraListener* listener)
{
    /* check if this listener is already added, to avoid adding
       duplicates */
    for (unsigned int i=0; i<listeners_.size(); i++) {
        if (listener == listeners_[i]) {
            CAM_ERR("this listener is already added");
            return;
        }
    }
    listeners_.push_back(listener);
}

void qmmfCAMERA::removeListener(ICameraListener* listener)
{
    /* erase if this listener is added */
    for (unsigned int i=0; i<listeners_.size(); i++) {
        if (listener == listeners_[i]) {
            listeners_.erase(listeners_.begin() + i);
            return;
        }
    }
}

void qmmfCAMERA::subscribe(uint32_t eventMask)
{
 
}

void qmmfCAMERA::unsubscribe(uint32_t eventMask)
{
 
}

int qmmfCAMERA::takePicture(uint32_t num_images)
{
	qmmfTakeSnapshot(num_images);
    return EXIT_SUCCESS;
}

int qmmfCAMERA::startPreview()
{
	status_t ret = EXIT_SUCCESS;
	CAM_INFO("%s:%d: Enter : P_on = %d  , V_on = %d ",__func__,__LINE__, isPreviewRunning_,isVideoRunning_);

	if (isPreviewRunning_ == true ) {
		CAM_ERR("%s:%d Preview already running P_on = %d  , V_on = %d ",__func__,__LINE__, isPreviewRunning_,isVideoRunning_);
		return EXIT_FAILURE;
	}

	ret = qmmfStartTrack(CAMERA_QMMF_TRACK_PREVIEW);
    isPreviewRunning_ = true;
	CAM_INFO("%s:%s: Exit ", TAG, __func__);
    return ret;
}

void qmmfCAMERA::stopPreview()
{
	status_t ret;
	CAM_INFO("%s:%d: Enter : P_on = %d  , V_on = %d ",__func__,__LINE__, isPreviewRunning_,isVideoRunning_);

	if (isPreviewRunning_ != true){
		CAM_ERR("%s:%d Error: Preview not started",__func__,__LINE__);
		return;
	}

	if (session_enabled_ == true) {
		qmmfStopSession();
	}else {
		CAM_ERR("%s:%d Error: session not enabled",__func__,__LINE__);
		return;
	}
	ret = qmmfTrackArray_[CAMERA_QMMF_TRACK_PREVIEW].qmmfDeleteTrack();
    if (ret != NO_ERROR){
		CAM_ERR("%s:%d qmmfDeleteTrack failed - %d ",__func__,__LINE__,ret);
    }
	isPreviewRunning_ = false;

	if (isVideoRunning_ == true) {
		qmmfStartSession();
	}
	CAM_INFO("%s:%s: Exit ", TAG, __func__);
    return;
}

int qmmfCAMERA::startRecording()
{
	CAM_INFO("%s:%d: Enter : P_on = %d  , V_on = %d ",__func__,__LINE__, isPreviewRunning_,isVideoRunning_);

	if (isVideoRunning_ == true ) {
		CAM_ERR("%s:%d Video already running P_on = %d  , V_on = %d ",__func__,__LINE__, isPreviewRunning_,isVideoRunning_);
		return EXIT_FAILURE;
	}

	qmmf_camera_params_.track_params_ptr[CAMERA_QMMF_TRACK_VIDEO]->frame_rate = \
		                                          qmmf_camera_params_.video_FPS_XX * \
		                                          qmmf_camera_params_.track_params_ptr[CAMERA_QMMF_TRACK_PREVIEW]->frame_rate;
	qmmfStartTrack(CAMERA_QMMF_TRACK_VIDEO);
    isVideoRunning_ = true;
	CAM_INFO("%s:%s: Exit ", TAG, __func__);
    return EXIT_SUCCESS;
}

void qmmfCAMERA::stopRecording()
{
	status_t ret = EXIT_SUCCESS;
	CAM_INFO("%s:%d: Enter : P_on = %d  , V_on = %d ",__func__,__LINE__, isPreviewRunning_,isVideoRunning_);
	if (isVideoRunning_ != true){
		CAM_ERR("%s:%d Error: Recording not started",__func__,__LINE__);
		return;
	}
	if (session_enabled_ == true) {
		qmmfStopSession();
	}else {
		CAM_ERR("%s:%d Error: session not enabled",__func__,__LINE__);
		return;
	}

	isVideoRunning_ = false;
	ret = qmmfTrackArray_[CAMERA_QMMF_TRACK_VIDEO].qmmfDeleteTrack();
    if (ret != NO_ERROR){
    	CAM_ERR("%s:%d qmmfDeleteTrack failed - %d ",__func__,__LINE__,ret);
    }

	if (isPreviewRunning_ == true) {
		qmmfStartSession();
	}
	CAM_INFO("%s:%s: Exit ", TAG, __func__);
	return;
}

int ICameraDevice::createInstance(int index, ICameraDevice** device)
{
	CAM_INFO("%s:%s: Enter", TAG, __func__);
	int rc = 0;
    qmmfCAMERA* me = NULL;
#ifdef LIBCAM_LOG_LEVEL_KPI
    LIBCAM_KPI_GET_MASK();
#endif
    if (isOpen(index) == true) {
        rc = EBUSY;
        goto bail;
    }

    me = new qmmfCAMERA;
    if (me == NULL) {
        rc = ENOMEM;
        goto bail;
    }
    rc = me->init(index);
    if (rc != 0) {
        rc = ENODEV;
        goto bail;
    }
    /* add entry to openCameras vector */
    pthread_mutex_lock(&halMutex);
    g_openCameras.push_back(index);
    pthread_mutex_unlock(&halMutex);
    *device = static_cast<ICameraDevice*>(me);
	CAM_INFO("%s:%s: Exit", TAG, __func__);
    return 0;
bail:
    delete me;
    CAM_INFO("%s:%s: Exit", TAG, __func__);
    return rc;
}

void ICameraDevice::deleteInstance(ICameraDevice** device)
{
	CAM_INFO("%s:%s: Enter", TAG, __func__);
    qmmfCAMERA* me = static_cast<qmmfCAMERA*>(*device);
    if (me == NULL) {
        return;
    }
   
	/* erase entry from openCamera vector */
    pthread_mutex_lock(&halMutex);
    for (size_t i=0; i < g_openCameras.size(); i++) {
        if (g_openCameras[i] == me->getID()) {
            g_openCameras.erase(g_openCameras.begin() + i);
            break;
        }
    }
    pthread_mutex_unlock(&halMutex);

    delete me;
    *device = NULL;
	CAM_INFO("%s:%s: Exit", TAG, __func__);
}



status_t qmmfCAMERA::qmmfRecorderConnect() {

  CAM_INFO("%s:%s: Enter", TAG, __func__);

  RecorderCb recorder_status_cb;
  recorder_status_cb.event_cb = [&] ( qmmf::recorder::EventType event_type, void *event_data,
      size_t event_data_size) { qmmfCAMERA::RecorderCallbackHandler(event_type, event_data,
      event_data_size); };

  auto ret = recorder_.Connect(recorder_status_cb);
  CAM_INFO("%s:%s: Exit", TAG, __func__);

  return ret;
}

status_t qmmfCAMERA::qmmfRecorderDisconnect() {

  CAM_INFO("%s:%s: Enter", TAG, __func__);
  auto ret = recorder_.Disconnect();
  CAM_INFO("%s:%s: Exit", TAG, __func__);
  return ret;
}

status_t qmmfCAMERA::qmmfStartCamera(){
 
  status_t ret;  
  CAM_INFO("%s:%s: Enter", TAG, __func__);
  CameraResultCb result_cb;
  result_cb = [&] (uint32_t camera_id,
      const CameraMetadata &result) {
    CameraResultCallbackHandler(camera_id, result); };

  ret = recorder_.StartCamera(camera_id_, qmmf_camera_params_.start_params, result_cb);
  if(NO_ERROR != ret) {
      CAM_ERR("%s:%d StartCamera Failed!!",__func__,__LINE__);
      goto bail;
  }

  ret = recorder_.GetDefaultCaptureParam(camera_id_, qmmf_camera_params_.static_default_meta_info);
  if (NO_ERROR != ret) {
    CAM_ERR("%s:%d Unable to query default capture parameters!\n",__func__ , __LINE__);
    goto bail;
  }
  
bail:
  CAM_INFO("%s:%s: Exit", TAG, __func__);
  return ret;
}

status_t qmmfCAMERA::qmmfStopCamera() {

  CAM_INFO("%s:%s: Enter", TAG, __func__);

  auto ret = recorder_.StopCamera(camera_id_);
  if(ret != 0) {
    CAM_ERR("%s:%s StopCamera Failed!!", TAG, __func__);
	return ret;
  }

  qmmf_camera_params_.mutexLock();
  qmmf_camera_params_.static_default_meta_info.clear();
  qmmf_camera_params_.static_session_meta_info.clear();
  qmmf_camera_params_.mutexUnlock();
  CAM_INFO("%s:%s: Exit", TAG, __func__);
  return ret;
}

status_t qmmfCAMERA::qmmfTakeSnapshot(uint32_t num_images) {

	CAM_INFO("%s:%s: Enter", TAG, __func__);
	int32_t ret = EXIT_SUCCESS;

	ImageCaptureCb cb = [&] (uint32_t camera_id_, uint32_t image_count,
					   BufferDescriptor buffer, MetaData meta_data)
					{ qmmfSnapshotCb(camera_id_, image_count, buffer, meta_data); };


	std::vector<CameraMetadata> meta_array;
	for (uint32_t i = 0; i < num_images; i++) {
		meta_array.push_back(qmmf_camera_params_.static_default_meta_info);
	}
	char prop[PROPERTY_VALUE_MAX];
	property_get("persist.qmmf.rec.test.snaptime",prop,"10");
	uint32_t wait_time_secs = atoi (prop);

	int32_t repeat = num_images;
	do {
		camera_error_ = false;
		burst_snapshot_count_ = num_images;
		ret = recorder_.CaptureImage(camera_id_,
									qmmf_camera_params_.snapshot_image_param,
									num_images, meta_array, cb);
	if(ret != NO_ERROR) {
		CAM_ERR("%s:%s CaptureImage Failed!!", TAG, __func__);
	}
	std::unique_lock<std::mutex> lock(snapshot_wait_lock_);
	if (snapshot_wait_signal_.wait_for(lock,
			std::chrono::milliseconds(wait_time_secs * 1000)) ==
			std::cv_status::timeout) {
		CAM_ERR("%s:%s Capture Image Timed out", TAG, __func__);
	}
	if (!camera_error_) {
		CAM_ERR("%s:%s Capture Image Done", TAG, __func__);
		break;
	}

	} while (repeat-- > 0);

	CAM_INFO("%s:%s: Exit", TAG, __func__);
	return ret;
}

status_t qmmfCAMERA::qmmfCreateSession() {

  CAM_INFO("%s:%s: Enter", TAG, __func__);
  SessionCb session_status_cb;
  session_status_cb.event_cb = [&] ( qmmf::recorder::EventType event_type, void *event_data,
      size_t event_data_size) { SessionCallbackHandler(event_type,
      event_data, event_data_size); };

  auto ret = recorder_.CreateSession(session_status_cb, &session_id_);
  CAM_INFO("%s:%s: sessions_id = %d", TAG, __func__, session_id_);
  return ret;
}

status_t qmmfCAMERA::qmmfStartTrack(FrameType trackType)
{
	status_t ret = EXIT_SUCCESS;
	if (session_enabled_ == true) {
	    ret = qmmfStopSession();
	    if (ret != NO_ERROR){
			CAM_ERR("%s:%d StopSession failed - %d ",__func__,__LINE__,ret);
			goto bail;
		}
	}

	ret = qmmfTrackArray_[trackType].qmmfCreateTrack();
    if (NO_ERROR != ret) {
		CAM_ERR("%s:%d qmmfCreateTrack failed\n",__func__,__LINE__);
		goto bail;
	};

    ret = qmmfStartSession();
	if (NO_ERROR != ret) {
		CAM_ERR("%s:%d qmmfStartSession failed\n",__func__,__LINE__);
		goto bail;
	};

bail:
	return ret;
}

status_t QMMFTrack::qmmfCreateTrack()
{
  status_t ret = EXIT_SUCCESS;
  TrackCb video_track_cb;

  CAM_INFO("%s:%s:%d Enter", TAG, __func__,__LINE__);

  video_track_cb.data_cb = [&] (uint32_t track_id,
        std::vector<BufferDescriptor> buffers, std::vector<MetaData>
        meta_buffers) { qmmf_camera_ptr_->qmmfTrackDataCB(track_id, buffers, meta_buffers); };

  video_track_cb.event_cb = [&] (uint32_t track_id, qmmf::recorder::EventType event_type,
        void *event_data, size_t data_size) { qmmf_camera_ptr_->qmmfTrackEventCB(track_id,
        event_type, event_data, data_size); };

  ret = recorder_ptr_->CreateVideoTrack(session_id_,
              track_id_, track_params_, video_track_cb);
  if (NO_ERROR != ret) {
    CAM_ERR("%s:%d CreateVideoTrack failed\n",__func__,__LINE__);
    goto bail;
  }

bail:
  CAM_INFO("%s:%s: Exit", TAG, __func__);
  return ret;
}

status_t QMMFTrack::qmmfDeleteTrack()
{
	status_t ret = EXIT_SUCCESS;
	ret = recorder_ptr_->DeleteVideoTrack(session_id_,track_id_);
	if (ret != NO_ERROR){
		CAM_ERR("%s:%d DeleteVideoTrack failed - %d \n",__func__,__LINE__,ret);
        return ret;
    }
	return ret;
}

status_t qmmfCAMERA::qmmfStartSession() {

  CAM_INFO("%s:%s: Enter", TAG, __func__);

  status_t ret = recorder_.StartSession(session_id_);
  if (NO_ERROR != ret) {
    CAM_ERR("%s:%d StartSession failed\n",__func__,__LINE__);
    return ret;
  }
  session_enabled_ = true;

  ret = recorder_.GetCameraParam(camera_id_, qmmf_camera_params_.static_session_meta_info);
  if (NO_ERROR != ret) {
    CAM_ERR("%s:%d Unable to initialize static session parameters!\n",__func__,__LINE__);
  }
  CAM_INFO("%s:%s: Exit", TAG, __func__);
  return ret;
}

status_t qmmfCAMERA::qmmfStopSession() {

  session_enabled_ = false;

  CAM_INFO("%s:%s: Enter", TAG, __func__);


  releaseAllFrames();
  /* check if any latest buffer is getting released after disabling session */
  while (release_new_frame_in_progress_flag_ == true) {
	 usleep(100);
  }
  status_t ret = recorder_.StopSession(session_id_, false);
  if (ret != NO_ERROR){
   	CAM_ERR("%s:%d StopSession failed - %d ",__func__,__LINE__,ret);
  }

  CAM_INFO("%s:%s: Exit", TAG, __func__);
  return ret;
}

status_t qmmfCAMERA::qmmfPauseSession() {

  CAM_INFO("%s:%s: Enter", TAG, __func__);
  auto ret = recorder_.PauseSession(session_id_);
  if (ret != NO_ERROR){
   	CAM_ERR("%s:%d PauseSession failed - %d ",__func__,__LINE__,ret);
  }
  CAM_INFO("%s:%s: Exit", TAG, __func__);
  return NO_ERROR;
}

status_t qmmfCAMERA::qmmfResumeSession() {

  CAM_INFO("%s:%s: Enter", TAG, __func__);
  auto ret = recorder_.ResumeSession(session_id_);
  if (ret != NO_ERROR){
   	CAM_ERR("%s:%d ResumeSession failed - %d ",__func__,__LINE__,ret);
  }
  CAM_INFO("%s:%s: Exit", TAG, __func__);

  return NO_ERROR;
}

status_t qmmfCAMERA::qmmfDeleteSession() {

  CAM_INFO("%s:%s: Enter", TAG, __func__);
  status_t ret;


  // Once all tracks are deleted successfully delete session.
  ret = recorder_.DeleteSession(session_id_);
  if (ret != NO_ERROR){
	CAM_ERR("%s:%d DeleteSessionfailed - %d ",__func__,__LINE__,ret);
  }


  CAM_INFO("%s:%s: Exit", TAG, __func__);
  return ret;
}

void qmmfCAMERA::CameraResultCallbackHandler(uint32_t camera_id,
                                                const CameraMetadata &result) {
  uint32_t meta_frame_number =
      result.find(ANDROID_REQUEST_FRAME_COUNT).data.i32[0];

    result_metadata_cache_mutex_.lock();
    if (result_metadata_cache_.size() >= 120) {
        result_metadata_cache_.erase(result_metadata_cache_.begin());
    }

    auto ret = result_metadata_cache_.insert((result_metadata_cache_.end())--,std::pair<const uint64_t,CameraMetadata>(meta_frame_number,result));
    if (ret == result_metadata_cache_.end()) 
    {
        CAM_ERR("%s:%d Could not store result metadata : frame_ts = %d",__func__,__LINE__ , meta_frame_number );
    }else{
        CAM_DBG("ResultCB : A_TS = %lld \n", result.find(ANDROID_SENSOR_TIMESTAMP).data.i64[0]);
    }
    result_metadata_cache_mutex_.unlock();
}


void qmmfCAMERA::qmmfSnapshotCb(uint32_t camera_id,
                              uint32_t image_sequence_count,
                              BufferDescriptor buffer, MetaData meta_data) {

  CAM_INFO("%s:%s Enter", TAG, __func__);

  CameraMetadata result;
#ifdef ENABLE_SNAPSHOT_PER_FRAME_METADATA
	int findCameraMetaDataCount = 0;
	while ( findCameraMetaDataCount < FRAME_METADATA_WAIT_MAX_ATTEMPT) {
		result_metadata_cache_mutex_.lock();
		auto it = result_metadata_cache_.find(buffer.timestamp);
		if (it == result_metadata_cache_.end()){
			findCameraMetaDataCount--;
			result_metadata_cache_mutex_.unlock();
			usleep(FRAME_METADATA_WAIT_US);
		}else{
			result = it->second;
			result_metadata_cache_mutex_.unlock();
			break;
		}
	}
	  
   if (findCameraMetaDataCount >= FRAME_METADATA_WAIT_MAX_ATTEMPT)
   {
          CAM_ERR("%s:%d : Error Element not found %lld ",__func__,__LINE__,buffer.timestamp );
    }else{
          /* notify each listener */
          qmmfCameraFrame::dispatchFrame(this->listeners_,&recorder_ ,buffer,
											 meta_data,result,
											 camera_id, 
											 session_id_ ,CAMERA_QMMF_TRACK_PICTURE );
	}
#else 
	  /* notify each listener */
	  qmmfCameraFrame::dispatchFrame(this->listeners_,&recorder_ ,buffer,
										 meta_data,result,
										 camera_id,
										 session_id_ ,CAMERA_QMMF_TRACK_PICTURE );
#endif 
	std::unique_lock<std::mutex> lock(snapshot_wait_lock_);
	if (image_sequence_count == burst_snapshot_count_ - 1) {
		snapshot_wait_signal_.notify_one();
	}

	CAM_INFO("%s:%s Exit", TAG, __func__);
}

void qmmfCAMERA::RecorderCallbackHandler(qmmf::recorder::EventType event_type,
                                           void *event_data,
                                           size_t event_data_size) {
  CAM_INFO("%s:%s: Enter", TAG, __func__);
  if (event_type == EventType::kCameraError) {
    std::unique_lock<std::mutex> lock(snapshot_wait_lock_);
    CAM_INFO("%s:%s: Found CameraError", TAG, __func__);
    camera_error_ = true;
  }
  CAM_INFO("%s:%s: Exit", TAG, __func__);
}

void qmmfCAMERA::SessionCallbackHandler(qmmf::recorder::EventType event_type,
                                          void *event_data,
                                          size_t event_data_size) {
  CAM_INFO("%s:%s: Enter", TAG, __func__);
  CAM_INFO("%s:%s: Exit", TAG, __func__);
}



void qmmfCAMERA::qmmfTrackEventCB(uint32_t track_id, qmmf::recorder::EventType event_type,
                             void *event_data, size_t event_data_size) {

  CAM_DBG("%s:%s: Enter", TAG, __func__);
  CAM_DBG("%s:%s: Exit", TAG, __func__);
}


void qmmfCAMERA::qmmfTrackDataCB(uint32_t track_id, std::vector<BufferDescriptor>
                            buffers, std::vector<MetaData> meta_buffers) {
 release_new_frame_in_progress_flag_ = true;

  if ( session_enabled_ == false ){
	  recorder_.ReturnTrackBuffer(session_id_,track_id,buffers);
	  release_new_frame_in_progress_flag_ = false;
	  return;
  }
  release_new_frame_in_progress_flag_ = false;

  CAM_DBG("%s:%s: Enter \n", TAG, __func__);

  CameraMetadata meta;
  CameraMetadata result;
  int findCameraMetaDataCount = 0;
  uint32_t meta_frame_number = 0;

  for (uint32_t i=0; i< meta_buffers.size();i++){
    MetaData meta_data = meta_buffers[i];
    if (meta_data.meta_flag &
        static_cast<uint32_t>(MetaParamType::kCamMetaFrameNumber)){
      meta_frame_number = meta_buffers[i].cam_meta_frame_number;
    }
  }

  for (uint32_t i = 0; i < meta_buffers.size(); ++i) {
	    while ( findCameraMetaDataCount < FRAME_METADATA_WAIT_MAX_ATTEMPT) {
            result_metadata_cache_mutex_.lock();
            auto it = result_metadata_cache_.find(meta_frame_number);
		    if (it == result_metadata_cache_.end()){
                ++findCameraMetaDataCount;
                result_metadata_cache_mutex_.unlock();
                usleep(FRAME_METADATA_WAIT_US);
            }else{
                result = it->second;
                result_metadata_cache_mutex_.unlock();
                break;
            }
        }
	  
      if (findCameraMetaDataCount >= FRAME_METADATA_WAIT_MAX_ATTEMPT)
      {
          CAM_ERR("%s camera id %d, track id %d,element %d not found meta, return buffer directly",
               __func__,camera_id_,track_id,meta_frame_number);
          recorder_.ReturnTrackBuffer(session_id_,track_id,buffers);
          release_new_frame_in_progress_flag_ = false;
      }else{
		  CAM_DBG("ResultCB : Frame# = %lld  A_TS = %lld Exp_t =  %lld \n",meta_frame_number, \
						 result.find(ANDROID_SENSOR_TIMESTAMP).data.i64[0] , \
                                                 result.find(ANDROID_SENSOR_EXPOSURE_TIME).data.i64[0]);
          /* Here the assumption is that each callback returns only buffer in the BufferDescriptor*/
          qmmfCameraFrame::dispatchFrame(this->listeners_,&recorder_ ,buffers, meta_buffers,result,camera_id_, session_id_ ,track_id);
      }
  }
  CAM_DBG("%s:%s: Exit \n", TAG, __func__);
}

qmmfCameraFrame::qmmfCameraFrame(int fd,uint32_t size) : recorder_(NULL){
		        /* populate buffer metadata */
        metadata_.buffer_type = android::kMetadataBufferTypeCameraSource;
        native_handle_t *nh =  (native_handle_t *)nh_mem_;
        nh->version = sizeof(native_handle_t);
        nh->numFds = NH_NUM_FDS;
        nh->numInts = NH_NUM_INTS;
        nh->data[0] = fd;
        nh->data[1] = 0; /* offset */
        nh->data[2] = size;
        nh->data[3] = 0; /* usage */
        nh->data[4] = 0; /* timestamp */
        nh->data[5] = QOMX_COLOR_FORMATYUV420PackedSemiPlanar32m; /* color_format */

		metadata_.meta_handle = nh;
}

QMMFTrack::QMMFTrack(){
	track_id_ = CAMERA_QMMF_TRACK_PREVIEW;
	session_id_ = 0;

	memset(&track_params_, 0x0, sizeof track_params_);
	track_params_.camera_id   = 0;
	track_params_.width       = DEFAULT_SENSOR_WIDTH;
	track_params_.height      = DEFAULT_SENSOR_HEIGHT;
	track_params_.frame_rate  = DEFAULT_SENSOR_FRAME_RATE;
	track_params_.format_type = VideoFormat::kYUV;

    switch(track_id_){
      case CAMERA_QMMF_TRACK_PREVIEW:
        track_params_.low_power_mode = true;
        break;
      case CAMERA_QMMF_TRACK_VIDEO:
      case CAMERA_QMMF_TRACK_PICTURE:
        track_params_.low_power_mode = false;
        break;
      default:
        CAM_ERR("%s:%d : Error, not support such track type %d",
             __func__,__LINE__,track_id_);
    }
}


int QMMFTrack::init(qmmfCAMERA *qmmf_camera_ptr,  \
		  Recorder *recorder_ptr, uint32_t camera_id, \
		  uint32_t session_id,  uint32_t track_id, \
		  VideoTrackCreateParam **track_params_ptr){

	int ret = EXIT_SUCCESS;
	qmmf_camera_ptr_ = qmmf_camera_ptr;
	if (qmmf_camera_ptr == NULL || recorder_ptr == NULL) {
		CAM_ERR("%s:%d : Error ",__func__,__LINE__);
		ret = EXIT_FAILURE;
		return ret;
	}
	recorder_ptr_ = recorder_ptr;
	track_params_.camera_id = camera_id;
	session_id_ = session_id;
	track_id_ = track_id;
    if (track_id_ != CAMERA_QMMF_TRACK_PREVIEW)
        track_params_.low_power_mode = false;
	*(track_params_ptr) = &track_params_;
	if (track_params_ptr == NULL) {
		CAM_ERR("%s:%d : Error ",__func__,__LINE__);
		ret = EXIT_FAILURE;
		return ret;
	}
	return ret;
}
} /* namespace camera */
