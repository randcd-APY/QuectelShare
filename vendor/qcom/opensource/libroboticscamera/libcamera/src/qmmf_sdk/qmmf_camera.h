/* Copyright (c) 2015, The Linux Foundataion. All rights reserved.
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
#ifndef __QMMFCAMERA_H__
#define __QMMFCAMERA_H__

#define TAG "LIBCAM"

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
#include <mutex> 
#include <map>
#include <vector>
#include <condition_variable>

#include <qmmf-sdk/qmmf_recorder.h>
#include <qmmf-sdk/qmmf_recorder_params.h>
#include <qmmf-sdk/qmmf_codec.h>
#include <camera/CameraMetadata.h>

#include <OMX_QCOMExtns.h>
#include <libstagefrighthw/QComOMXMetadata.h>



#include "camera_log.h"
#include "camera.h"
#include "qmmf_camera.h"
#include "camera_parameters.h"

// Sleep for specified seconds to allow settling after parameter change
#define PARAMETER_SETTLE_INTERVAL(x) sleep(x)

using namespace qmmf;
using namespace recorder;
using namespace android;

#define DEFAULT_SENSOR_FRAME_RATE 30
#define DEFAULT_SENSOR_WIDTH 640
#define DEFAULT_SENSOR_HEIGHT 480

namespace camera
{

enum FrameType
{
    CAMERA_QMMF_TRACK_PREVIEW = 1,
    CAMERA_QMMF_TRACK_VIDEO = 2,
    CAMERA_QMMF_TRACK_PICTURE = 3,
    CAMERA_QMMF_TRACK_MAX = 4,
};

class QMMFCameraParams{

public:
    CameraStartParam start_params;
    CameraMetadata static_default_meta_info;
    CameraMetadata static_session_meta_info;
    bool static_session_meta_info_updated_flag;
	VideoTrackCreateParam *track_params_ptr[CAMERA_QMMF_TRACK_MAX];
	ImageParam snapshot_image_param;

	/*Local Bookkeeping Parameters*/
	camera::VideoFPS video_FPS_XX;

    std::mutex params_mutex;  

    QMMFCameraParams()
    {
        memset(&start_params, 0x0, sizeof start_params);
        start_params.zsl_mode            = false;
        start_params.zsl_queue_depth     = 10;
        start_params.zsl_width           = DEFAULT_SENSOR_WIDTH;
        start_params.zsl_height          = DEFAULT_SENSOR_HEIGHT;
        start_params.frame_rate          = DEFAULT_SENSOR_FRAME_RATE;
        start_params.flags               = 0x0;

		for (int i = 0; i < CAMERA_QMMF_TRACK_MAX ; i++) {
			track_params_ptr[i] = NULL;
		}
		static_session_meta_info_updated_flag = false;
		memset(&snapshot_image_param, 0x0, sizeof(snapshot_image_param));
		snapshot_image_param.image_format  = ImageFormat::kJPEG;
	    snapshot_image_param.image_quality = 95;

		video_FPS_XX = VIDEO_FPS_1X;
    }

	void mutexLock()
	{
	   params_mutex.lock();
	}

	void mutexUnlock()
	{
	   params_mutex.unlock();
	}
};


#define NH_NUM_FDS 1
#define NH_NUM_INTS 5
#define FRAME_METADATA_WAIT_US 250
#define FRAME_METADATA_WAIT_MAX_ATTEMPT 60
#define RELEASE_REF_BUF_WAIT_US 33000
#define RELEASE_REF_BUF_WAIT_MAX_ATTEMPT 30

class qmmfCameraFrame : public ICameraFrame
{
private:
    qmmf::recorder::Recorder *recorder_; 

    std::vector<BufferDescriptor> buffers_;
	BufferDescriptor buffer_;


	std::vector<qmmf::recorder::MetaData> meta_buffers_;
	qmmf::recorder::MetaData meta_buffer_;
    
	CameraMetadata frame_info_metadata_;
    uint32_t  camera_id_;
    uint32_t  session_id_;
    uint32_t  track_id_;
    FrameType type_;
    uint32_t buf_id_;

	android::encoder_media_buffer_type metadata_;
	uint8_t nh_mem_[sizeof(native_handle_t) +
        (NH_NUM_FDS + NH_NUM_INTS) * sizeof(int)];
    static std::map<int,qmmfCameraFrame*> refs_fd_frame_map_;
    static std::mutex refs_map_mutex_;

    std::mutex refs_mutex_;
    
public:
	qmmfCameraFrame(int fd,uint32_t size);

    virtual ~qmmfCameraFrame();

    virtual uint32_t acquireRef();

    virtual uint32_t releaseRef();

    static void dispatchFrame(std::vector<ICameraListener *> listeners,
                                  qmmf::recorder::Recorder *recorder,
                                  std::vector<BufferDescriptor> &buffers, 
								  std::vector<qmmf::recorder::MetaData> &meta_buffers, 
                                  CameraMetadata &frameMetadata,
								  uint32_t  camera_id,
                                  uint32_t  session_id, uint32_t  track_id);

	static void dispatchFrame(std::vector<ICameraListener *> listeners,
							   qmmf::recorder::Recorder *recorder,
							   BufferDescriptor &buffers, 
							   qmmf::recorder::MetaData &meta_buffers, 
							   CameraMetadata &frameMetadata,
							   uint32_t camera_id,
							   uint32_t  session_id, uint32_t  track_id);


    virtual CameraMetadata* getFrameInfoMetadataPtr(){
		return static_cast<CameraMetadata*>(&frame_info_metadata_);
    }

    friend status_t releaseAllFrames();
    static void qmmf_camera_kpi_atrace(uint32_t camera_id,
                                       qmmfCameraFrame *frame);
};


class qmmfCAMERA;

class QMMFTrack{
private:
	uint32_t track_id_;
	uint32_t session_id_;
    VideoTrackCreateParam track_params_;
	Recorder *recorder_ptr_;
	qmmfCAMERA *qmmf_camera_ptr_;
public:

	status_t qmmfCreateTrack();
	status_t qmmfDeleteTrack();

    QMMFTrack();
	
	int init(qmmfCAMERA *qmmf_camera_ptr,  \
		  Recorder *recorder_ptr, uint32_t camera_id, \
		  uint32_t session_id,  uint32_t track_id, \
		  VideoTrackCreateParam **track_params_ptr);

};


class qmmfCAMERA : public ICameraDevice
{
    ICameraParameters* params_;
    std::vector<ICameraListener *> listeners_;

    bool isPreviewRunning_;
    bool isVideoRunning_;

    status_t qmmfRecorderConnect();
    status_t qmmfRecorderDisconnect();
	void RecorderCallbackHandler(EventType event_type, void *event_data,
							   size_t event_data_size);


    status_t qmmfStartCamera();
    status_t qmmfStopCamera();
	void CameraResultCallbackHandler(uint32_t camera_id,
								   const CameraMetadata &result);

	status_t qmmfCreateSession(); 
    status_t qmmfDeleteSession();
    status_t qmmfStartSession();
    status_t qmmfStopSession();
    status_t qmmfPauseSession();
    status_t qmmfResumeSession();
	void SessionCallbackHandler(EventType event_type,
							  void *event_data, size_t event_data_size);

	status_t qmmfStartTrack(FrameType trackType);
	status_t qmmfTakeSnapshot(uint32_t num_images = 1);

    void qmmfSnapshotCb(uint32_t camera_id, uint32_t image_sequence_count,
                  BufferDescriptor buffer, MetaData meta_data);

    Recorder recorder_;
    QMMFCameraParams qmmf_camera_params_;

    uint32_t camera_id_;
	uint32_t session_id_;
	bool session_enabled_;

	QMMFTrack qmmfTrackArray_[CAMERA_QMMF_TRACK_MAX];

    std::map<uint64_t,CameraMetadata> result_metadata_cache_;
	std::mutex result_metadata_cache_mutex_;

	/* Cannot use mutex because:
	   Trackdata callback can occur during stop session.
	   And stop session will wait for all buffers to be released */
	bool release_new_frame_in_progress_flag_;

	std::mutex               snapshot_wait_lock_;
	std::condition_variable  snapshot_wait_signal_;
	uint32_t                 burst_snapshot_count_;
	bool                     camera_error_;
public:
    qmmfCAMERA();

    virtual ~qmmfCAMERA();

    int init(int index);

    int getID() { return camera_id_; }

    /* Implementation of virtual methods of ICameraDevice interface */
    virtual void addListener(ICameraListener* listener);
    virtual void removeListener(ICameraListener* listener);

    virtual void subscribe(uint32_t eventMask);
    virtual void unsubscribe(uint32_t eventMask);

    virtual int setParameters(const ICameraParameters& params);
    virtual int getParameters(uint8_t* buf, uint32_t bufSize,
                              int* bufSizeRequired);
    virtual void* getParameters();
                              
    virtual int takePicture(uint32_t num_images = 1);

    virtual int startPreview();
    virtual void stopPreview();

    virtual int startRecording();
    virtual void stopRecording();

    virtual void cancelPicture() {}
    virtual void sendFaceDetectCommand(bool turn_on) {};


	void qmmfTrackEventCB(uint32_t track_id, EventType event_type, void *event_data,
					 size_t event_data_size);
	void qmmfTrackDataCB(uint32_t track_id, std::vector<BufferDescriptor> buffers,
				std::vector<MetaData> meta_buffers);
};

} /* namespace camera */

#endif /* __QMMFCAMERA_H__ */

