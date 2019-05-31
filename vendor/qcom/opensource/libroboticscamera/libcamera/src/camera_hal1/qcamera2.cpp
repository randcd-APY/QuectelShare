/* Copyright (c) 2015, 2017, The Linux Foundation. All rights reserved.
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
#include <sys/param.h>
#include <pthread.h>

#include <vector>

#include <sstream>

#include "camera_log.h"
#include "camera.h"
#include "camera_memory.h"
#include "qcamera2.h"

using namespace std;

namespace camera
{

const char* CAMERA_HAL_LIB_NAME = "/usr/lib/hw/camera.msm8909.so";
const int ID_STR_LEN = 16;

static camera_module_t* g_halModule = NULL;
static pthread_mutex_t halMutex = PTHREAD_MUTEX_INITIALIZER;
enum stream_type{
    STREAM_PREVIEW,
    STREAM_VIDEO,
    STREAM_NUM
};
static pthread_mutex_t streamMutex[STREAM_NUM];
bool QCamera2::isVideoRunning_ = false;
bool QCamera2::isPreviewRunning_ = false;

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
    for (size_t i=0; i < g_openCameras.size(); i++) {
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

inline static CameraEventType toEventType(int32_t msgType)
{
    switch (msgType) {
      case CAMERA_MSG_FOCUS:
          return CAMERA_EVT_FOCUS;
      default:
          return CAMERA_EVT_NONE;
    }
}

/* convert camera event bitmask to device message type bitmask */
inline static int32_t toDeviceMsgType(uint32_t eventMask)
{
    int32_t msgType = 0x00;
    if (eventMask & CAMERA_EVT_FOCUS) {
        msgType |= CAMERA_MSG_FOCUS;
    }
    return msgType;
}

/* QCamera2Frame implementation */
QCamera2Frame::QCamera2Frame(struct camera_device* dev, int64_t timestamp,
              int32_t msg_type, const camera_memory_t* mem) :
    dev_(dev)
{
    timeStamp = timestamp;
    data      = static_cast<uint8_t*>(mem->data);
    size      = mem->size;

    switch (msg_type) {
      case CAMERA_MSG_PREVIEW_FRAME:
          type = CAMERA_FRAME_PREVIEW;
          break;
      case CAMERA_MSG_VIDEO_FRAME:
          type = CAMERA_FRAME_VIDEO;
          break;
      default:
          CAM_ERR("unsupported msg type, msg=%d", msg_type);
    }
}

QCamera2Frame::~QCamera2Frame()
{
    dev_ = 0;
}

uint32_t QCamera2Frame::acquireRef()
{
    return refs_++;
}

uint32_t QCamera2Frame::releaseRef()
{
    if (refs_ <= 0) {
        return 0;
    }
    refs_--;
    if (0 == refs_) {
        if (type == CAMERA_FRAME_VIDEO) {
            dev_->ops->release_recording_frame(dev_, data);
        }
    }
    return refs_;
}

static void copyFaces(FaceRoi &appFaceData,
    const camera_frame_metadata_t &frameMetadata,
    const camera_face_t *faces)
{
    appFaceData.number_of_faces = frameMetadata.number_of_faces;
    for (int i = 0; (i < appFaceData.number_of_faces) && (i < MAX_FACES); i++) {
        for(int j = 0; j < 4; j++)
            appFaceData.faces[i].rect[j] = faces[i].rect[j];
        appFaceData.faces[i].score = faces[i].score;
        appFaceData.faces[i].id = faces[i].id;
        for(int j = 0; j < 2; j++)
            appFaceData.faces[i].left_eye[j] = faces[i].left_eye[j];
        for(int j = 0; j < 2; j++)
            appFaceData.faces[i].right_eye[j] = faces[i].right_eye[j];
        for(int j = 0; j < 2; j++)
            appFaceData.faces[i].mouth[j] = faces[i].mouth[j];
        appFaceData.faces[i].smile_degree = faces[i].smile_degree;
        appFaceData.faces[i].smile_score = faces[i].smile_score;
        appFaceData.faces[i].blink_detected = faces[i].blink_detected;
        appFaceData.faces[i].face_recognised = faces[i].face_recognised;
        appFaceData.faces[i].gaze_angle = faces[i].gaze_angle;
        appFaceData.faces[i].updown_dir = faces[i].updown_dir;
        appFaceData.faces[i].leftright_dir = faces[i].leftright_dir;
        appFaceData.faces[i].roll_dir = faces[i].roll_dir;
        appFaceData.faces[i].left_right_gaze = faces[i].left_right_gaze;
        appFaceData.faces[i].top_bottom_gaze = faces[i].top_bottom_gaze;
        appFaceData.faces[i].leye_blink = faces[i].leye_blink;
        appFaceData.faces[i].reye_blink = faces[i].reye_blink;
    }
}

void QCamera2Frame::dispatchFrame(ICameraListener* listener,
                          struct camera_device* dev, int64_t timestamp,
                          int32_t msg_type, const camera_memory_t* mem)
{
    QCamera2Frame *frame = &((CameraMemory *)mem->handle)->frame;

    frame->dev_ = dev;
    frame->timeStamp = timestamp;

    frame->acquireRef();
    switch (msg_type) {
      case CAMERA_MSG_PREVIEW_FRAME:
          frame->type = CAMERA_FRAME_PREVIEW;
          listener->onPreviewFrame(static_cast<ICameraFrame*>(frame));
          break;
      case CAMERA_MSG_VIDEO_FRAME:
          frame->type = CAMERA_FRAME_VIDEO;
          listener->onVideoFrame(static_cast<ICameraFrame*>(frame));
          break;
      case CAMERA_MSG_COMPRESSED_IMAGE:
          frame->type = CAMERA_FRAME_PICTURE;
          listener->onPictureFrame(static_cast<ICameraFrame*>(frame));
          break;
      case CAMERA_MSG_PREVIEW_METADATA: {
          unsigned char *pFaceResult = (unsigned char *) mem->data;
          camera_frame_metadata_t *roiData =
              (camera_frame_metadata_t *)pFaceResult;
          camera_face_t *faces = (camera_face_t *)
              (pFaceResult + sizeof(camera_frame_metadata_t));
          FaceRoi *appFaceRoi = new FaceRoi;
          copyFaces(*appFaceRoi, *roiData, faces);
          frame->facedata = appFaceRoi;
          listener->onMetadataFrame(static_cast<ICameraFrame*>(frame));
          break;
      }
      default:
          CAM_ERR("unsupported msg_type %d", msg_type);
          // TODO: add support for other messages using metadata callback
          //  listener->onMetadataFrame(static_cast<ICameraFrame*>(frame));
    }
    frame->releaseRef();
}

QCamera2::QCamera2() :
    dev_(NULL),
    id_(-1),
    isPreviewRequested_(false)
{
    int i;
    for (i = 0; i < STREAM_NUM; i++)
        pthread_mutex_init(&streamMutex[i], NULL);
}

int QCamera2::init(int idx)
{
    camera_module_t* mod = getCameraHalModule();
    int nret = 0;
    char idStr[ID_STR_LEN];

    if (NULL == mod) {
        CAM_ERR("camera HAL module loading failed");
        nret = ELIBACC;
        goto bail;
    }

    snprintf(idStr, ID_STR_LEN, "%d", idx);
    mod->common.methods->open((hw_module_t*)mod, idStr, (hw_device_t**)&dev_);
    if (NULL == dev_) {
        CAM_ERR("camera device open failed");
        nret = EBADR;
        goto bail;
    }

    dev_->ops->set_callbacks(dev_, QCamera2::notify_callback,
                             QCamera2::data_callback,
                             QCamera2::data_timestamp_callback,
                             CameraMemory::requestMemory, this);

    /* enable error events by default */
    dev_->ops->enable_msg_type(dev_, CAMERA_MSG_ERROR);

    id_ = idx;
bail:
    return nret;
}

QCamera2::~QCamera2()
{
    /* close camera device */
    if (NULL != dev_) {
        dev_->common.close(&dev_->common);
        dev_ = NULL;
    }
}

void QCamera2::notify_callback(int32_t msg_type, int32_t ext1, int32_t ext2,
                               void* user)
{
    QCamera2* me = (QCamera2*)user;

    if (NULL == me) {
        CAM_ERR("failed");
        return;
    }
    ControlEvent control;
    control.type = toEventType(msg_type);
    if (control.type == CAMERA_EVT_NONE) {
        CAM_ERR("unsupported msg type");
        return;
    }
    control.ext1 = ext1;
    control.ext2 = ext2;
    /* notify each listener */
    for (size_t i=0; i < me->listeners_.size(); i++) {
        if (CAMERA_MSG_ERROR == msg_type) {
            me->listeners_[i]->onError();
        } else {
            me->listeners_[i]->onControl(control);
        }
    }
}

int32_t QCamera2::msg_to_stream(int32_t msg_type)
{
    switch(msg_type) {
        case CAMERA_MSG_PREVIEW_FRAME:
            return STREAM_PREVIEW;
        case CAMERA_MSG_VIDEO_FRAME:
            return STREAM_VIDEO;
        default:
            return STREAM_PREVIEW;
    }
}

void QCamera2::data_callback(int32_t msg_type,
                             const camera_memory_t* data,
                             unsigned int index,
                             camera_frame_metadata_t* metadata,
                             void* user)
{
    QCamera2* me = (QCamera2*)user;
    if (NULL == me) {
        CAM_ERR("failed");
        return;
    }

    pthread_mutex_lock(&streamMutex[msg_to_stream(msg_type)]);
    if (((CAMERA_MSG_PREVIEW_FRAME == msg_type) ||
        (CAMERA_MSG_PREVIEW_METADATA == msg_type)) && (!isPreviewRunning_))
        goto exit;
    if ((CAMERA_MSG_VIDEO_FRAME == msg_type) && (!isVideoRunning_))
        goto exit;
    /* notify each listener */
    for (size_t i=0; i < me->listeners_.size(); i++) {
        QCamera2Frame::dispatchFrame(me->listeners_[i], me->dev_, 0,
                                     msg_type, data);
    }
exit:
    pthread_mutex_unlock(&streamMutex[msg_to_stream(msg_type)]);
}

void QCamera2::data_timestamp_callback(int64_t timestamp, int32_t msg_type,
                                       const camera_memory_t* data,
                                       unsigned int index, void* user)
{
    QCamera2* me = (QCamera2*)user;
    if (NULL == me) {
        CAM_ERR("failed");
        return;
    }

    pthread_mutex_lock(&streamMutex[msg_to_stream(msg_type)]);
    if (((CAMERA_MSG_PREVIEW_FRAME == msg_type) ||
        (CAMERA_MSG_PREVIEW_METADATA == msg_type)) && (!isPreviewRunning_))
        goto exit;
    if ((CAMERA_MSG_VIDEO_FRAME == msg_type) && (!isVideoRunning_))
        goto exit;
    /* notify each listener */
    for (size_t i=0; i < me->listeners_.size(); i++) {
        QCamera2Frame::dispatchFrame(me->listeners_[i], me->dev_, timestamp,
                                     msg_type, data);
    }
exit:
    pthread_mutex_unlock(&streamMutex[msg_to_stream(msg_type)]);
}

int QCamera2::setParameters(const ICameraParameters& params)
{
    // empty buffer
    std::stringbuf buffer;
    // associate stream buffer to stream
    std::ostream os(&buffer);

    params.writeObject(os);

    return dev_->ops->set_parameters(dev_, buffer.str().c_str());
}

int QCamera2::getParameters(uint8_t* buf, uint32_t bufSize, int* bufSizeRequired)
{
    int rc = 0;
    char* p = dev_->ops->get_parameters(dev_);
    int nlen = strlen(p);

    memmove(buf, p, MIN(nlen, bufSize));
    if (NULL != bufSizeRequired) {
        *bufSizeRequired = nlen;
    }
    dev_->ops->put_parameters(dev_, p);
    return rc;
}

void * QCamera2::getParameters()
{
    CAM_ERR("getParameters not implemented for HAL1");
    return NULL;
}
void QCamera2::addListener(ICameraListener* listener)
{
    /* check if this listener is already added, to avoid adding
       duplicates */
    for (size_t i=0; i<listeners_.size(); i++) {
        if (listener == listeners_[i]) {
            CAM_ERR("this listener is already added");
            return;
        }
    }
    listeners_.push_back(listener);
}

void QCamera2::removeListener(ICameraListener* listener)
{
    /* erase if this listener is added */
    for (size_t i=0; i<listeners_.size(); i++) {
        if (listener == listeners_[i]) {
            listeners_.erase(listeners_.begin() + i);
            return;
        }
    }
}

void QCamera2::subscribe(uint32_t eventMask)
{
    dev_->ops->enable_msg_type(dev_, toDeviceMsgType(eventMask));
}

void QCamera2::unsubscribe(uint32_t eventMask)
{
    dev_->ops->disable_msg_type(dev_, toDeviceMsgType(eventMask));
}

int QCamera2::takePicture(uint32_t num_images)
{
    int rc = 0;
    dev_->ops->enable_msg_type(dev_, CAMERA_MSG_COMPRESSED_IMAGE);
    for (uint32_t i = 0; i < num_images; i++) {
        rc = dev_->ops->take_picture(dev_);
        if (rc != 0) {
            CAM_ERR("takePicture fails for %d, rc %d", i, rc);
            return rc;
        }
    }
    return rc;
}

int QCamera2::startPreview()
{
    int rc = 0;
    pthread_mutex_lock(&streamMutex[msg_to_stream(CAMERA_MSG_PREVIEW_FRAME)]);
    if (isPreviewRequested_ && isPreviewRunning_) {
        CAM_ERR("preview is already started.");
        pthread_mutex_unlock(&streamMutex[msg_to_stream(CAMERA_MSG_PREVIEW_FRAME)]);
        return -1;
    }
    isPreviewRequested_ = true;
    dev_->ops->enable_msg_type(dev_, CAMERA_MSG_PREVIEW_FRAME);
    dev_->ops->enable_msg_type(dev_, CAMERA_MSG_PREVIEW_METADATA);
    if (isPreviewRunning_ == false) {
        rc = dev_->ops->start_preview(dev_);
        if (rc == 0) {
            isPreviewRunning_ = true;
        }
    }
    pthread_mutex_unlock(&streamMutex[msg_to_stream(CAMERA_MSG_PREVIEW_FRAME)]);
    return 0;
}

void QCamera2::sendFaceDetectCommand(bool turn_on)
{
    if (turn_on) {
        dev_->ops->send_command(dev_, CAMERA_CMD_START_FACE_DETECTION, 0,
            0);
    } else {
        dev_->ops->send_command(dev_, CAMERA_CMD_STOP_FACE_DETECTION, 0,
            0);
    }
}

void QCamera2::stopPreview()
{
    pthread_mutex_lock(&streamMutex[msg_to_stream(CAMERA_MSG_PREVIEW_FRAME)]);
    isPreviewRequested_ = false;
    dev_->ops->disable_msg_type(dev_, CAMERA_MSG_PREVIEW_METADATA);
    dev_->ops->disable_msg_type(dev_, CAMERA_MSG_PREVIEW_FRAME);
    /* stop preview only if video is not running */
    if (isPreviewRunning_ == true && isVideoRunning_ == false) {
        dev_->ops->stop_preview(dev_);
        isPreviewRunning_ = false;
    }
    pthread_mutex_unlock(&streamMutex[msg_to_stream(CAMERA_MSG_PREVIEW_FRAME)]);
}

int QCamera2::startRecording()
{
    int rc=0;
    /* start preview internally */
    pthread_mutex_lock(&streamMutex[msg_to_stream(CAMERA_MSG_VIDEO_FRAME)]);
    if (isPreviewRunning_ == false) {
        rc = dev_->ops->start_preview(dev_);
        if (rc != 0) {
            goto bail;
        }
        isPreviewRunning_ = true;
    }
    dev_->ops->enable_msg_type(dev_, CAMERA_MSG_VIDEO_FRAME);
    rc = dev_->ops->start_recording(dev_);
    if (rc == 0) {
        isVideoRunning_ = true;
    }
bail:
    pthread_mutex_unlock(&streamMutex[msg_to_stream(CAMERA_MSG_VIDEO_FRAME)]);
    return rc;
}

void QCamera2::stopRecording()
{
    pthread_mutex_lock(&streamMutex[msg_to_stream(CAMERA_MSG_VIDEO_FRAME)]);
    dev_->ops->disable_msg_type(dev_, CAMERA_MSG_VIDEO_FRAME);
    dev_->ops->stop_recording(dev_);
    isVideoRunning_ = false;
    /* stop preview if it is not requested */
    if (isPreviewRequested_ == false && isPreviewRunning_ == true) {
        dev_->ops->stop_preview(dev_);
    }
    pthread_mutex_unlock(&streamMutex[msg_to_stream(CAMERA_MSG_VIDEO_FRAME)]);
}


int ICameraDevice::createInstance(int index, ICameraDevice** device)
{
    int rc = 0;
    QCamera2* me = NULL;

    if (isOpen(index) == true) {
        rc = EBUSY;
        goto bail;
    }

    me = new QCamera2;
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
    return 0;
bail:
    delete me;
    return rc;
}

void ICameraDevice::deleteInstance(ICameraDevice** device)
{
    QCamera2* me = static_cast<QCamera2*>(*device);
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
}

} /* namespace camera */
