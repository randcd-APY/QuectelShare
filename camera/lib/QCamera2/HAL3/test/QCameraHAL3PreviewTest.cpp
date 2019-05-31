/* Copyright (c) 2016, The Linux Foundation. All rights reserved.
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

#include "QCameraHAL3PreviewTest.h"
#include "QCameraHAL3MainTestContext.h"
#include "gralloc_priv.h"
#define LOG_TAG "QCameraHAL3PreviewTest"

namespace qcamera {
extern hal3_camera_lib_test *CamObj_handle;
int req_sent;
extern pthread_cond_t mRequestAppCond;
int test_case_end;
bool thread_exit;
extern std::list<uint32_t> PreviewQueue;
int preview_buffer_allocated;
extern pthread_mutex_t TestAppLock, mCaptureRequestLock;
int snapshot_buffer = -1;

#define MAX_JPEG_SIZE     (12*1024*1024)
#define FRAME_DUMP_RATE   (30)            // 30 means 1 in 30 frames will be saved
                                          // 0 means dump disabled
#define DUMP_EVERY_SNAP   (1)             // if 1 dumps all snapshot, otherwise
                                          // uses FRAME_DUMP_RATE
QCameraHAL3ConfigTest::QCameraHAL3ConfigTest(int cameraId,
                                             CameraHAL3Base* HAL3Base):
                                                      QCameraHAL3Test(cameraId),
                                                      mBuffs(NULL),
                                                      mHAL3Base(HAL3Base),
                                                      mCamId(cameraId),
                                                      mCaseStopFlag(false),
                                                      mQueued(0),
                                                      mRequestedSnapshots(0),
                                                      mFPS(30.0),
                                                      mPipelineDepth(0),
                                                      mTestSnapshotStream(NULL),
                                                      mNumSnapshotBuffers(0),
                                                      mRuntimeSnapshot(0)
{
  camera3_callback_ops::notify = &Notify;
  camera3_callback_ops::process_capture_result = &processCaptureResult;
  pthread_mutex_init(&mLock, NULL);
}

void QCameraHAL3ConfigTest::Initialize(int cameraId,
      const std::vector<HAL3TestStreamInfo>& infos,
      const HAL3TestSnapshotInfo *snapshotInfo)
{

    printf("%s:%d initializing cam %d\n", __func__, __LINE__, cameraId);
    bool hasSnapshot = false;
    for (std::vector<HAL3TestStreamInfo>::const_iterator it = infos.begin();
        it != infos.end(); it++) {
        if (it->camera_id != cameraId)
            continue;
        HAL3TestCaseStreamInfo *TestCaseStream = new HAL3TestCaseStreamInfo;
        int format = HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED;
        switch (it->type) {
            case HAL3_TEST_STREAM_TYPE_RAW:
                format = HAL_PIXEL_FORMAT_RAW10;
                break;
            default:
                LOGE("%s:%d Unexpected - Number of captures is > 0 but format is None",
                      __func__, __LINE__);
                break;
        }
        TestCaseStream->mStream = initStream(CAMERA3_STREAM_OUTPUT,
          it->camera_id, it->width, it->height, 0,
          format, HAL3_DATASPACE_UNKNOWN);
        TestCaseStream->isVideo = it->isvideo;
        TestCaseStream->mWidth = it->width;
        TestCaseStream->mHeight = it->height;
        vTestStreams.push_back(TestCaseStream);
        mFPS = it->fps;
    }
    camera3_device_t *device_handle = mHAL3Base->mLibHandle->test_obj.device;
    if (snapshotInfo) {
        if (snapshotInfo->count &&
            snapshotInfo->type != HAL3_TEST_STREAM_TYPE_MAX &&
            snapshotInfo->camera_id == cameraId)
            hasSnapshot = true;
        LOGE("%s:%d Requested %d snapshots with camera %d", __func__, __LINE__,
            snapshotInfo->count, snapshotInfo->camera_id);
    }
    if (hasSnapshot) {
        mNumSnapshotBuffers = (snapshotInfo->count > 8) ? 8 : snapshotInfo->count;
        mRequestedSnapshots = snapshotInfo->count;
        mTestSnapshotStream = new HAL3TestCaseStreamInfo;
        int format = HAL_PIXEL_FORMAT_BLOB;
        int dataspace = HAL3_DATASPACE_JFIF;
        switch (snapshotInfo->type) {
            case HAL3_TEST_STREAM_TYPE_YUV:
                format = HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED;
                dataspace =  HAL3_DATASPACE_UNKNOWN;
                break;
            case HAL3_TEST_STREAM_TYPE_RAW:
                format = HAL_PIXEL_FORMAT_RAW10;
                dataspace =  HAL3_DATASPACE_UNKNOWN;
                break;
            case HAL3_TEST_STREAM_TYPE_JPEG:
                break;
            default:
                LOGE("%s:%d Unexpected - Number of captures is > 0 but format is None",
                      __func__, __LINE__);
                break;
        }
        mTestSnapshotStream->mStream = initStream(CAMERA3_STREAM_OUTPUT,
            snapshotInfo->camera_id, snapshotInfo->width, snapshotInfo->height,
            0, format, dataspace);
        mTestSnapshotStream->isVideo = false;
        mTestSnapshotStream->mWidth = snapshotInfo->width;
        mTestSnapshotStream->mHeight = snapshotInfo->height;
    } else {
        mNumSnapshotBuffers = 0;
    }

    LOGE("%s:%d streams to start %d\n", __func__, __LINE__, vTestStreams.size()
        + (hasSnapshot ? 1 : 0));
    mConfig = configureStream(CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE,
        vTestStreams.size() + (hasSnapshot ? 1 : 0));
    // Assumption - if first stream is RAW, using RAW only mode
    if (vTestStreams[0]->mStream->format !=
            HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED) {
        mConfig.operation_mode =
            QCAMERA3_VENDOR_STREAM_CONFIGURATION_RAW_ONLY_MODE;
    }
    mPipelineDepth = 0;
    for (int i = 0; i < vTestStreams.size(); i++) {
        mConfig.streams[i] = vTestStreams[i]->mStream;
    }
    if (hasSnapshot) {
        mConfig.streams[vTestStreams.size()] = mTestSnapshotStream->mStream;
    }

    // Initially configure only non-snapshot streams
    mConfig.num_streams = vTestStreams.size();
    device_handle->ops->configure_streams(device_handle, &(mConfig));
    for (int i = 0; i < vTestStreams.size(); i++) {
        if (mPipelineDepth < mConfig.streams[i]->max_buffers) {
            mPipelineDepth = mConfig.streams[i]->max_buffers;
        }
    }
    mPipelineDepth += 8;
    for (int i = 0; i < vTestStreams.size(); i++) {
        vTestStreams[i]->mMetaDataPtr =
            device_handle->ops->construct_default_request_settings(
                device_handle,
                vTestStreams[i]->isVideo ? CAMERA3_TEMPLATE_VIDEO_RECORD :
                                           CAMERA3_TEMPLATE_PREVIEW);

        vTestStreams[i]->mHandle = new native_handle_t *[mPipelineDepth];
        vTestStreams[i]->mMeminfo = new hal3_camtest_meminfo_t[mPipelineDepth];
        for (int j = 0; j < mPipelineDepth; j++) {
            vTestStreams[i]->mHandle[j] = allocateBuffers(
                vTestStreams[i]->mWidth, vTestStreams[i]->mHeight,
                &vTestStreams[i]->mMeminfo[j]);
            vTestStreams[i]->mQueue.push_back(j);
        }
    }

    if (hasSnapshot) {
        mTestSnapshotStream->mMetaDataPtr =
            device_handle->ops->construct_default_request_settings(
                device_handle, CAMERA3_TEMPLATE_STILL_CAPTURE);
        mTestSnapshotStream->mHandle =
            new native_handle_t *[mNumSnapshotBuffers];

        mTestSnapshotStream->mMeminfo =
            new hal3_camtest_meminfo_t[mNumSnapshotBuffers];
        int snapWidth = snapshotInfo->width;
        int snapHeight = snapshotInfo->width;
        if (snapshotInfo->type == HAL3_TEST_STREAM_TYPE_JPEG) {
            snapWidth = MAX_JPEG_SIZE;
            snapHeight = 1;
        }
        for (int j = 0; j < mNumSnapshotBuffers; j++) {
            mTestSnapshotStream->mHandle[j] = allocateBuffers(
                snapWidth, snapHeight, &mTestSnapshotStream->mMeminfo[j]);
            mTestSnapshotStream->mQueue.push_back(j);
        }
    }

    mBuffs = new camera3_stream_buffer_t[vTestStreams.size() +
                                          (hasSnapshot ? 1 : 0)];
};

void QCameraHAL3ConfigTest::Notify(const camera3_callback_ops *cb,
                                    const camera3_notify_msg *msg)
{
    if(cb == NULL) {
        LOGD("callback returned is NULL");
    }
    QCameraHAL3ConfigTest *cfgTest = const_cast<QCameraHAL3ConfigTest *>(
      static_cast<const QCameraHAL3ConfigTest *>(cb));
    cfgTest->handleNotify(cb, msg);
}

void QCameraHAL3ConfigTest::handleNotify(const camera3_callback_ops *cb,
        const camera3_notify_msg *msg)
{
    switch (msg->type) {
      case CAMERA3_MSG_ERROR: {
          pthread_mutex_lock(&mLock);
          if (mNumSnapshotBuffers && msg->message.error.error_stream ==
                      mTestSnapshotStream->mStream) {
              mRuntimeSnapshot++;
              pthread_mutex_unlock(&mLock);
              LOGE("%s:%d, Notify error %d for snapshot %u, retrying",
                  __func__, __LINE__,
                  msg->message.error.error_code, msg->message.error.frame_number);
          } else {
              pthread_mutex_unlock(&mLock);
              LOGE("%s:%d, Notify error %d for frame %u", __func__, __LINE__,
                  msg->message.error.error_code, msg->message.error.frame_number);
          }
      }
          break;
      case CAMERA3_MSG_SHUTTER: {
          LOGE("%s:%d, Notify shutter for frame %u time %llu", __func__, __LINE__,
              msg->message.shutter.frame_number, msg->message.shutter.timestamp);
      }
          break;
      default: {
                 LOGE("UNHANDLED");
      }
          break;
    }
}

void QCameraHAL3ConfigTest::processCaptureResult(
        const camera3_callback_ops *cb,
        const camera3_capture_result *result)
{

    if(cb == NULL) {
        LOGD("callback returned is NULL");
    }
    QCameraHAL3ConfigTest *cfgTest = const_cast<QCameraHAL3ConfigTest *>(
      static_cast<const QCameraHAL3ConfigTest *>(cb));
    cfgTest->handleCaptureResult(cb, result);
}

uint32_t getJpegSize(uint8_t *jpegBuffer, uint32_t bufferSize) {

  uint32_t ret = bufferSize;
  uint32_t jpegBlobSize = sizeof(struct camera3_jpeg_blob);

  if (bufferSize > jpegBlobSize) {
    uint8_t *footer = jpegBuffer + (bufferSize - jpegBlobSize - 1);
    struct camera3_jpeg_blob *jpegBlob = (struct camera3_jpeg_blob *)footer;

    if (CAMERA3_JPEG_BLOB_ID == jpegBlob->jpeg_blob_id) {
      ret = jpegBlob->jpeg_size;
    }
  }
  return ret;
}

void generateFileName(char *name, int format, uint32_t frame)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    switch (format) {
        case HAL_PIXEL_FORMAT_BLOB: {
            sprintf(name, "snp_%06u_%010u.jpg", frame,
                tv.tv_sec*1000 + tv.tv_usec/1000);
        }
            break;
        case HAL_PIXEL_FORMAT_RAW10: {
            sprintf(name, "frm_%06u_%010u.raw", frame,
                tv.tv_sec*1000 + tv.tv_usec/1000);
        }
            break;
        default: {
            sprintf(name, "frm_%06u_%010u.yuv", frame,
                tv.tv_sec*1000 + tv.tv_usec/1000);
        }
            break;
    }
}

void saveBuffer(char *name, HAL3TestCaseStreamInfo *testCaseStream, uint32_t bufIndex)
{
    FILE *f = fopen(name, "wb");
    if (f) {
        uint32_t bufferSize = testCaseStream->mMeminfo[bufIndex].size;
        if (testCaseStream->mStream->format == HAL_PIXEL_FORMAT_BLOB) {
            // Buffer contains JPEG extract the exact size from buffer footer
            bufferSize = getJpegSize((uint8_t*)testCaseStream->mMeminfo[bufIndex].vaddr, bufferSize);
        }
        fwrite(testCaseStream->mMeminfo[bufIndex].vaddr, 1, bufferSize, f);
        LOGE("Frame saved in %s with size %u", name, bufferSize);
        fclose(f);
    } else {
        LOGE("%s:%d File open unsuccessful", __func__, __LINE__);
    }
}

void QCameraHAL3ConfigTest::handleCaptureResult(
        const camera3_callback_ops *cb,
        const camera3_capture_result *result)
{
    buffer_thread_msg_t msg;
    int frame_num;
    int num;
    pthread_mutex_lock(&mLock);
        for (int i = 0; i < result->num_output_buffers; i++) {
            bool valid = false;
            frame_num = result->frame_number;
            (mHAL3Base->mFrameCount)++;
            int sIdx,bIdx;
            for(sIdx = 0; sIdx < vTestStreams.size(); sIdx++) {

                if (vTestStreams[sIdx]->mStream == result->output_buffers[i].stream) {
                    valid = true;
                    break;
                }
            }
            if (sIdx < vTestStreams.size()) {
                for(bIdx = 0; bIdx < mPipelineDepth; bIdx++) {
                    private_handle_t *lh =
                          private_handle_t::dynamicCast(vTestStreams[sIdx]->mHandle[bIdx]);
                    private_handle_t *rh =
                          private_handle_t::dynamicCast(*result->output_buffers[i].buffer);
                    if (lh->fd == rh->fd) {
                        if (FRAME_DUMP_RATE) {
                            if (frame_num % FRAME_DUMP_RATE == 0) {
                                char name[32];
                                generateFileName(name, vTestStreams[sIdx]->mStream->format, frame_num);
                                saveBuffer(name, vTestStreams[sIdx], bIdx);
                            }
                        }
                        vTestStreams[sIdx]->mQueue.push_back(bIdx);
                        LOGE("Matching Stream %d buffer %d", sIdx, bIdx);
                        break;
                    }
                }
            }
            if(mNumSnapshotBuffers) {
                if (mTestSnapshotStream->mStream ==
                        result->output_buffers[i].stream) {
                    LOGE("%s:%d Capture frame received!", __func__, __LINE__);
                    valid = true;
                    for(bIdx = 0; bIdx < mPipelineDepth; bIdx++) {
                        private_handle_t *lh =
                              private_handle_t::dynamicCast(mTestSnapshotStream->mHandle[bIdx]);
                        private_handle_t *rh =
                              private_handle_t::dynamicCast(*result->output_buffers[i].buffer);
                        if (lh->fd == rh->fd) {
                            if (FRAME_DUMP_RATE || DUMP_EVERY_SNAP) {
                                if (DUMP_EVERY_SNAP || frame_num % FRAME_DUMP_RATE == 0) {
                                    char name[32];
                                    generateFileName(name, mTestSnapshotStream->mStream->format, frame_num);
                                    saveBuffer(name, mTestSnapshotStream, bIdx);
                                }
                            }
                            mTestSnapshotStream->mQueue.push_back(bIdx);
                            LOGE("Matching capture buffer %d", bIdx);
                            break;
                        }
                    }
                }
            }

            if (!valid) {
                printf ("\nWRONG %d %d\n", i, sIdx);
                continue;
            }

            pthread_mutex_lock(&mThread.mutex);
            mQueued--;
            pthread_mutex_unlock(&mThread.mutex);
            LOGD("Preview/Video Capture Result %d and fcount: %d and /*req_Sent:%d*/ and %d \n",
            result->num_output_buffers, mHAL3Base->mFrameCount, /*req_sent,*/ result->frame_number);

        }
        int flag = 1;
        for (int j = 0; j < vTestStreams.size(); j++) {
          if (vTestStreams[j]->mQueue.size() == 0) {
            flag = 0;
            break;
          }
        }
        pthread_mutex_unlock(&mLock);
        if((flag == 1) && result->num_output_buffers) {
            pthread_mutex_lock(&mThread.mutex);
            pthread_cond_signal(&mThread.cond);
            pthread_mutex_unlock(&mThread.mutex);
            memset(&msg, 0, sizeof(buffer_thread_msg_t));
        }
   /* }
    else {
        extern int fcount_captured;
        if (result->num_output_buffers == 1) {
            LOGD("snapshot/Raw Capture1 Result Callback %d and %d",
                    result->num_output_buffers, fcount_captured);
            (mHAL3Base->mFrameCount)++;
            fcount_captured++;
            LOGD("\n Capture %d done preparing for capture ", fcount_captured);
            memset(&msg, 0, sizeof(buffer_thread_msg_t));
            write(pfd[1], &msg, sizeof(buffer_thread_msg_t));
        }
    }*/

}

void QCameraHAL3ConfigTest::Deinitialize()
{
    // Cleanup snapshot buffers;
    if (mNumSnapshotBuffers) {
        for (int j = 0; j < mNumSnapshotBuffers; j++) {
            mTestSnapshotStream->mQueue.clear();
            freeBuffers(mTestSnapshotStream->mHandle[j],
                &mTestSnapshotStream->mMeminfo[j]);
        }
        delete[] mTestSnapshotStream->mMeminfo;
        delete[] mTestSnapshotStream->mHandle;
        delete mTestSnapshotStream->mStream;
        delete mTestSnapshotStream;
    }


    // Cleanup preview/video buffers;
    delete [] mBuffs;
    for (int i = 0; i < vTestStreams.size(); i++) {
        for (int j = 0; j < mPipelineDepth; j++) {
            vTestStreams[i]->mQueue.clear();
            freeBuffers(vTestStreams[i]->mHandle[j],
                &vTestStreams[i]->mMeminfo[j]);
        }
        delete[] vTestStreams[i]->mMeminfo;
        delete[] vTestStreams[i]->mHandle;
        delete vTestStreams[i]->mStream;
        delete vTestStreams[i];
    }
    vTestStreams.clear();
}

QCameraHAL3ConfigTest::~QCameraHAL3ConfigTest()
{

    pthread_cond_destroy(&mThread.cond);
    pthread_mutex_destroy(&mThread.mutex);
    pthread_mutex_destroy(&mLock);
};

void QCameraHAL3ConfigTest::testStart()
{

  mRequest.frame_number = 0;
  testThreadCreate();
};


void QCameraHAL3ConfigTest::testMakeSnapshot()
{
    if (mNumSnapshotBuffers) {
        printf("\nTAKING SNAPSHOT\n");
        LOGE("%s:%d Taking %d Snapshot(s)", __func__, __LINE__, mRequestedSnapshots);
        mConfig.num_streams = vTestStreams.size() + 1;
        camera3_device_t *device_handle = mHAL3Base->mLibHandle->test_obj.device;
        device_handle->ops->configure_streams(device_handle, &(mConfig));
        pthread_mutex_lock(&mLock);
        mRuntimeSnapshot = mRequestedSnapshots;
        pthread_mutex_unlock(&mLock);
    }
};

void QCameraHAL3ConfigTest::testEnd()
{
    pthread_mutex_lock(&mThread.mutex);
    mCaseStopFlag = true;
    pthread_mutex_unlock(&mThread.mutex);
    pthread_join(mThread.td, NULL);
};

int QCameraHAL3ConfigTest::testThreadCreate()
{
    int32_t ret = 0;
    pthread_attr_t attr;
    if (pipe(pfd) < 0) {
        LOGE("%s: Error in creating the pipe", __func__);
    }
    pthread_mutex_init(&mThread.mutex, NULL);
    pthread_cond_init(&mThread.cond, NULL);
    mThread.is_thread_started = 0;
    mThread.readfd = pfd[0];
    mThread.writefd = pfd[1];
    mThread.data_obj = this;
    mThread.camera_obj = mHAL3Base->mLibHandle;
    mThread.stop_flag = &mCaseStopFlag;
    mThread.queued = &mQueued;
    mThread.testcase = MENU_INVALID;
    ret = pthread_create(&mThread.td, NULL, processBuffers, &mThread );
    /*pthread_setname_np(mThread.td, "TestApp_Thread");*/
    if (ret < 0) {
        LOGE("Failed to create status mThread");
        return 0;
    }
    return 1;
};

void QCameraHAL3ConfigTest::testProcessBuffers()
{

};


void QCameraHAL3ConfigTest::captureRequestRepeat(hal3_camera_lib_test *my_hal3test_obj, int camid, int testcase)
{
    int num1, num2;
    hal3_camera_test_obj_t *my_test_obj = &(mHAL3Base->mLibHandle->test_obj);
    camera3_device_t *device_handle = my_test_obj->device;
    int cnt = 0;

    pthread_mutex_lock(&mLock);
    for (int i = 0; i < vTestStreams.size(); i++) {
        if (vTestStreams[i]->mQueue.empty()) {
        } else {
            num2 = vTestStreams[i]->mQueue.front();
            vTestStreams[i]->mQueue.pop_front();
            num1 = mRequest.frame_number;
            if (num1 < 1) {
                (mRequest).settings = vTestStreams[i]->mMetaDataPtr;

            } else {
                (mRequest).settings = NULL;
            }

            mBuffs[cnt].stream = vTestStreams[i]->mStream;
            mBuffs[cnt].status = 0;
            mBuffs[cnt].buffer =
                    (const native_handle_t**)&vTestStreams[i]->mHandle[num2];
            mBuffs[cnt].release_fence = -1;
            mBuffs[cnt].acquire_fence = -1;
            cnt++;

        }
    }
    if (mRuntimeSnapshot) {
        if (mTestSnapshotStream->mQueue.empty()) {
            LOGE("%s:%d no capture buffer for CamID : %d", __func__, __LINE__, mCamId);
        } else {
            num2 = mTestSnapshotStream->mQueue.front();
            mTestSnapshotStream->mQueue.pop_front();
            (mRequest).settings = mTestSnapshotStream->mMetaDataPtr;

            mBuffs[cnt].stream = mTestSnapshotStream->mStream;
            mBuffs[cnt].status = 0;
            mBuffs[cnt].buffer =
                    (const native_handle_t**)&mTestSnapshotStream->mHandle[num2];
            mBuffs[cnt].release_fence = -1;
            mBuffs[cnt].acquire_fence = -1;
            cnt++;
            mRuntimeSnapshot--;
        }
    } else {
    }
    if ((mRequest).settings) {
        int32_t fps_range[2];
        fps_range[0] = mFPS;
        fps_range[1] = mFPS;
        android::CameraMetadata hal3app_preview_settings;
        hal3app_preview_settings = vTestStreams[0]->mMetaDataPtr;

        LOGE("Setting FPS to %3.1f", mFPS);
        hal3app_preview_settings.update(
            ANDROID_CONTROL_AE_TARGET_FPS_RANGE, fps_range, 2);
        (mRequest).settings = hal3app_preview_settings.release();
    }
    pthread_mutex_unlock(&mLock);
    if (cnt) {
        (mRequest).input_buffer = NULL;
        (mRequest).num_output_buffers = cnt;
        (mRequest).output_buffers = mBuffs;
        device_handle->ops->process_capture_request(my_test_obj->device, &(mRequest));
        pthread_mutex_lock(&mThread.mutex);
        mQueued += cnt;
        pthread_mutex_unlock(&mThread.mutex);
        (mRequest.frame_number)++;
    }
}

QCameraHAL3PreviewTest::QCameraHAL3PreviewTest(int camid) :
    QCameraHAL3Test(0),
    mPreviewHandle(NULL),
    mCaptureHandle(NULL),
    mPreviewStream(NULL),
    nobuffer(0),
    mCamId(camid),
    ir_mode(0),
    svhdr_mode(0)
{

}

void QCameraHAL3PreviewTest::initTest(hal3_camera_lib_test *handle,
                                int testcase, int camid, int w, int h)
{
    int i;
    CamObj_handle = handle; thread_exit = 0; test_case_end = 0;
    configurePreviewStream(&(handle->test_obj) , camid, w, h, testcase);
    constructDefaultRequest(&(handle->test_obj), camid);
    mPreviewHandle = new native_handle_t *[mPipelineDepthPreview];
    for (i = 0; i < mPipelineDepthPreview; i++)
        mPreviewHandle[i] = new native_handle_t;
    for (i = 0, req_sent = 1; i < mPipelineDepthPreview; i++, req_sent++) {
        previewAllocateBuffers(width, height, i);
        PreviewQueue.push_back(i);
    }
    mRequest.frame_number = 0;
    previewProcessThreadCreate(handle,testcase);
}

void QCameraHAL3PreviewTest::snapshotCaptureRequest(hal3_camera_lib_test *handle,
        int testcase, int camid, int w, int h)
{
    LOGD("Requested Capture Sizes for testcase:%d are :%d  X %d",testcase, w, h);
    captureRequestRepeat(handle, camid, MENU_START_CAPTURE);
    pthread_mutex_unlock(&mCaptureRequestLock);
}

void QCameraHAL3PreviewTest::configurePreviewStream(hal3_camera_test_obj_t *my_test_obj,
                                int camid, int w, int h, int testcase)
{
    camera3_device_t *device_handle = my_test_obj->device;
    mPreviewStream =  new camera3_stream_t;
    memset(mPreviewStream, 0, sizeof(camera3_stream_t));
    if (testcase != MENU_START_RDI_PREVIEW) {
      mPreviewStream = initStream(CAMERA3_STREAM_OUTPUT, camid, w, h, 0,
            HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, HAL3_DATASPACE_UNKNOWN);

      mPreviewConfig = configureStream(CAMERA3_STREAM_CONFIGURATION_NORMAL_MODE, 1);
    }
    else {
      mPreviewStream = initStream(CAMERA3_STREAM_OUTPUT, camid, w, h, 0,
          HAL_PIXEL_FORMAT_RAW10, HAL3_DATASPACE_ARBITRARY);

      mPreviewConfig = configureStream(QCAMERA3_VENDOR_STREAM_CONFIGURATION_RAW_ONLY_MODE, 1);
    }
    mPreviewConfig.streams[0] = mPreviewStream;
    device_handle->ops->configure_streams(my_test_obj->device, &(mPreviewConfig));
    mPipelineDepthPreview = mPreviewConfig.streams[0]->max_buffers;
    preview_buffer_allocated = mPipelineDepthPreview;
}

void QCameraHAL3PreviewTest::constructDefaultRequest(
        hal3_camera_test_obj_t *my_test_obj, int camid)
{
    camera3_device_t *device_handle = my_test_obj->device;
    LOGD("Camera ID : %d",camid);
    mMetaDataPtr[0]= device_handle->ops->construct_default_request_settings(my_test_obj->device,
            CAMERA3_TEMPLATE_PREVIEW);
    mMetaDataPtr[1] = device_handle->ops->construct_default_request_settings(my_test_obj->device,
            CAMERA3_TEMPLATE_STILL_CAPTURE);
}

void QCameraHAL3PreviewTest::captureRequestRepeat(
        hal3_camera_lib_test *my_hal3test_obj, int camid, int testcase)
{
    int num1, num2;
    int ir_mode_changed;
    int svhdr_mode_changed;
    int32_t set_svhdr_mode;
    int32_t set_ir_mode;
    hal3_camera_test_obj_t *my_test_obj = &(my_hal3test_obj->test_obj);
    camera3_device_t *device_handle = my_test_obj->device;

    if (testcase == MENU_START_PREVIEW || testcase == MENU_START_RDI_PREVIEW) {
        if (PreviewQueue.empty()) {
            LOGE("no preview buffer for CamID : %d", camid);
        }
        else {
            if (test_case_end == 0) {
                LOGD(" Request Number is preview : %d ",mRequest.frame_number);
                pthread_mutex_lock(&mCaptureRequestLock);
                num2 = PreviewQueue.front();
                PreviewQueue.pop_front();
                num1 = mRequest.frame_number;
                ir_mode_changed = get_ir_mode(ir_mode);
                svhdr_mode_changed = get_svhdr_mode(svhdr_mode);
                ALOGE("setting IR mode :%d",ir_mode_changed);
                ALOGE("setting SVHDR mode :%d",svhdr_mode_changed);
                if (num1 < 1) {
                    (mRequest).settings = mMetaDataPtr[0];
                }
                else if(ir_mode_changed == 1) {
                    hal3app_preview_settings = mMetaDataPtr[0];
                    if(ir_mode == 0) {
                        ALOGE("setting IR mode off");
                        set_ir_mode = (int32_t)QCAMERA3_IR_MODE_OFF;
                    }
                    else {
                        ALOGE("setting IR mode On");
                        set_ir_mode = (int32_t)QCAMERA3_IR_MODE_ON;
                    }
                    hal3app_preview_settings.update(QCAMERA3_IR_MODE, &set_ir_mode, 1);
                    (mRequest).settings = hal3app_preview_settings.release();
                }
                else if(svhdr_mode_changed == 1) {
                    hal3app_preview_settings = mMetaDataPtr[0];
                    if(svhdr_mode == 0) {
                        ALOGE("setting HDR mode off");
                        set_svhdr_mode = (int32_t)QCAMERA3_VIDEO_HDR_MODE_OFF;
                    }
                    else {
                        ALOGE("setting HDR mode On");
                        set_svhdr_mode = (int32_t)QCAMERA3_VIDEO_HDR_MODE_ON;
                    }
                    hal3app_preview_settings.update(QCAMERA3_VIDEO_HDR_MODE, &set_svhdr_mode, 1);
                    (mRequest).settings = hal3app_preview_settings.release();
                }
                else {
                    (mRequest).settings = NULL;
                }
                (mRequest).input_buffer = NULL;
                (mRequest).num_output_buffers = 1;
                mPreviewStreamBuffs.stream = mPreviewStream;
                mPreviewStreamBuffs.status = 0;
                mPreviewStreamBuffs.buffer =
                        (const native_handle_t**)&mPreviewHandle[num2];
                mPreviewStreamBuffs.release_fence = -1;
                mPreviewStreamBuffs.acquire_fence = -1;
                (mRequest).output_buffers = &(mPreviewStreamBuffs);
                LOGD("Calling HAL3APP repeat capture request %d and %d and free buffer :%d "
                        , num1, num2, PreviewQueue.size());

                device_handle->ops->process_capture_request(my_test_obj->device, &(mRequest));
                (mRequest.frame_number)++;
                pthread_mutex_unlock(&mCaptureRequestLock);
            }
        }
    }
    else {
        snapshot_buffer = mRequest.frame_number;
        (mRequest).settings = mMetaDataPtr[1];
        mSnapshotStreamBuffs = hal3appGetStreamBuffs(mSnapshotStream);
        mSnapshotStreamBuffs.buffer = (const native_handle_t**)&mCaptureHandle;
        mRequest = hal3appGetRequestSettings(&mSnapshotStreamBuffs, 1);
        LOGD("Calling snap HAL3APP repeat capture request repeat %d  ", snapshot_buffer);
        device_handle->ops->process_capture_request(my_test_obj->device, &(mRequest));
        (mRequest.frame_number)++;
    }
}

void QCameraHAL3PreviewTest::previewTestEnd(
        hal3_camera_lib_test *my_hal3test_obj, int camid)
{
    test_case_end = 1;
    hal3_camera_test_obj_t *my_test_obj = &(my_hal3test_obj->test_obj);
    camera3_device_t *device_handle = my_test_obj->device;
    device_handle->ops->flush(my_test_obj->device);
    LOGD("%s Closing Camera %d", __func__, camid);
    ioctl(mPreviewMeminfo.ion_fd, ION_IOC_FREE, &mPreviewMeminfo.ion_handle);
    close(mPreviewMeminfo.ion_fd);
    mPreviewMeminfo.ion_fd = -1;
    LOGD("%s Closing thread", __func__);
    thread_exit = 1;
}

void QCameraHAL3PreviewTest::previewAllocateBuffers(int width, int height, int num)
{
    mPreviewHandle[num] = allocateBuffers(width, height, &mPreviewMeminfo);
}

void QCameraHAL3PreviewTest::snapshotAllocateBuffers(int width, int height)
{
    mCaptureHandle = allocateBuffers(width, height, &mCaptureMemInfo);
}

bool QCameraHAL3PreviewTest::previewProcessThreadCreate(
        hal3_camera_lib_test *handle, int testcase)
{
    if(handle == NULL) {
        LOGD("Camera Hanle is NULL");
    }
    processThreadCreate(this, testcase);
    return 1;
}

int QCameraHAL3PreviewTest::get_ir_mode(int ir_mode)
{
    static int prev_irmode;
    if(ir_mode == prev_irmode)
        return 0;
    else {
        prev_irmode = ir_mode;
        return 1;
    }
}

int QCameraHAL3PreviewTest::get_svhdr_mode(int hdr_mode)
{
    static int prev_hdrmode;
    if(hdr_mode == prev_hdrmode)
        return 0;
    else {
        prev_hdrmode = hdr_mode;
        return 1;
    }
}

QCameraHAL3PreviewTest::~QCameraHAL3PreviewTest()
{

}

}
