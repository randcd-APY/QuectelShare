/**********************************************************************
*  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

#ifndef __QCAMERA_BUFFERMAP_H__
#define __QCAMERA_BUFFERMAP_H__

extern "C" {
#include "img_common.h"
#include "img_mem_ops.h"
#include "img_buffer.h"

}

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#ifndef _LE_CAMERA_
#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>
#endif

#ifdef _LE_CAMERA_
#include <ui/GraphicBuffer.h>
#endif

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>

#include <system/graphics.h>
#include <gralloc_priv.h>
#include <android/log.h>
#include <stdio.h>
#include <vector>
#include <pthread.h>
#include <map>

#include <qdMetaData.h>


//#include "mm_jpeg_ionbuf.h"
using namespace android;



typedef struct {
  uint8_t* addr;
  uint32_t length;
  int32_t fd;
} bufInfo_t;

typedef struct {
  uint32_t index;
  uint32_t isInputBuffer;
  img_mem_handle_t meta_buffer;
  private_handle_t *privateHandle;
  sp<GraphicBuffer> pNativeBuffer;
  EGLImageKHR hEGLImage;
  GLuint hColorTexture;
  GLuint hRBO;  // Only for output buffers
  GLuint hFBO;  // Only for output buffers
}bufMapInfo;
/*========QCameraBufferMap=======================================
 * Class: QCameraBufferMap
 *
 * Description: This class provides functionality to map native buffers to graphics buffer handles
 *
 * Notes: none
 *====================================================*/
class QCameraBufferMap
{
public:
 /**
  * Function: QCameraBufferMap
  *
  * Description: constructor of class
  *
  * Arguments:
  *   none
  *
  * Return values:
  *     none
  *
  * Notes: none
  **/
  QCameraBufferMap();

 /**
  * Function: ~QCameraBufferMap
  *
  * Description: destructor of class
  *
  * Arguments:
  *   none
  *
  * Return values:
  *     none
  *
  * Notes: none
  **/
  ~QCameraBufferMap();

 //void Init(size_t size, int32_t flags, uint32_t inUsage,
  //int32_t bufferType, int32_t format, int32_t width, int32_t height);
 void Init();
 void DeInit();
 void FlushMaps();
 GLuint GetSourceTexture(uint32_t index, uint32_t fd,uint32_t size,
  int32_t width, int32_t height, int32_t stride, int32_t scanline);
 GLuint GetFrameBuffer(uint32_t index, uint32_t fd, uint32_t size,
  int32_t width, int32_t height, int32_t stride, int32_t scanline);

 private:
  int32_t mFlags;
  uint32_t mUsage;
  int32_t mBufferType;
  int32_t mFormat;
  #if 0
  size_t mSize;
  int32_t mWidth;
  int32_t mHeight;
  int32_t mStride;
  int32_t mScanline;
  #endif
  std::map<uint32_t ,bufMapInfo > mSourceTextureMap;
  std::map<uint32_t ,bufMapInfo > mFrameBufferMap;
};
#endif
