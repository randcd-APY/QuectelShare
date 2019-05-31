/**********************************************************************
*  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

#include "QCameraBufferMap.h"
#include "QCameraPPEisCore.h"


// EGLImage extension function pointers
PFNGLEGLIMAGETARGETTEXTURE2DOESPROC                     p_glEGLImageTargetTexture2DOES = NULL;
PFNEGLCREATEIMAGEKHRPROC                                p_eglCreateImageKHR            = NULL;
PFNEGLDESTROYIMAGEKHRPROC                               p_eglDestroyImageKHR           = NULL;
PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC p_glEGLImageTargetRenderbufferStorageOES = NULL;


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
QCameraBufferMap::QCameraBufferMap()
{

}

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
QCameraBufferMap::~QCameraBufferMap()
{

}

void QCameraBufferMap::Init()
{
  //IDBG_LOW("E");
  mFlags = 0x8 | GRALLOC_USAGE_PRIVATE_CAMERA_HEAP;
  mUsage = GRALLOC_USAGE_HW_TEXTURE | GRALLOC_USAGE_HW_RENDER;
  mBufferType = BUFFER_TYPE_UI;
  mFormat = HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS;

  p_glEGLImageTargetTexture2DOES = reinterpret_cast<PFNGLEGLIMAGETARGETTEXTURE2DOESPROC>(eglGetProcAddress("glEGLImageTargetTexture2DOES"));
  p_eglCreateImageKHR  = reinterpret_cast<PFNEGLCREATEIMAGEKHRPROC>(eglGetProcAddress("eglCreateImageKHR"));
  p_eglDestroyImageKHR = reinterpret_cast<PFNEGLDESTROYIMAGEKHRPROC>(eglGetProcAddress("eglDestroyImageKHR"));
  p_glEGLImageTargetRenderbufferStorageOES = reinterpret_cast<PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC>(eglGetProcAddress("glEGLImageTargetRenderbufferStorageOES"));
}

void QCameraBufferMap::DeInit()
{
  IDBG_LOW("E");
  mFlags = 0;
  mUsage = 0;
  mBufferType = 0;
  mFormat = 0;

  FlushMaps();
  p_glEGLImageTargetTexture2DOES = NULL;
  p_eglCreateImageKHR  = NULL;
  p_eglDestroyImageKHR = NULL;
  p_glEGLImageTargetRenderbufferStorageOES = NULL;

}

void QCameraBufferMap::FlushMaps()
{
  IDBG_LOW("Flushing Source Texture Map");
  std::map<uint32_t ,bufMapInfo>::iterator itr = mSourceTextureMap.begin();
  while (itr != mSourceTextureMap.end()) {
    bufMapInfo info = itr->second;
    IDBG_LOW("Flushing element for idx %d ", info.index);
    glDeleteTextures(1, &info.hColorTexture);
    if(info.hEGLImage) {
      p_eglDestroyImageKHR(eglGetCurrentDisplay(), info.hEGLImage);
      checkEglError("Destroy image");
    }

    //Free metadata buffer
    img_buffer_release(&info.meta_buffer);

    //Free handle
    delete info.privateHandle;

    itr = mSourceTextureMap.erase(itr);

  }
  IDBG_LOW("Flushing Frame Buffer Map");
  itr = mFrameBufferMap.begin();
  while (itr != mFrameBufferMap.end()) {
    bufMapInfo info = itr->second;

    IDBG_LOW("Flushing element for idx %d ", info.index);
    glDeleteFramebuffers(1, &info.hFBO);
    glDeleteRenderbuffers(1, &info.hRBO);

    if(info.hEGLImage){
      p_eglDestroyImageKHR(eglGetCurrentDisplay(), info.hEGLImage);
      checkEglError("Destroy image");
    }
    //Free metadata buffer
    img_buffer_release(&info.meta_buffer);

    //Free handle
    delete info.privateHandle;

    itr = mFrameBufferMap.erase(itr);

  }
}

GLuint QCameraBufferMap::GetSourceTexture(uint32_t index, uint32_t fd,
  uint32_t size, int32_t width, int32_t height, int32_t stride, int32_t scanline)
{
  IDBG_LOW("E");
  int rc = 0;
  GLint error = 0;
  std::map<uint32_t ,bufMapInfo> ::iterator it;
  EGLint vEGLAttribs[] = {EGL_WIDTH,  width, EGL_HEIGHT, height, EGL_NONE};

  it = mSourceTextureMap.find(index);
  if (it != mSourceTextureMap.end()) {
    IDBG_LOW("Map found!");
    bufMapInfo info = it->second;

    IDBG_LOW("index %d tex %d!", index, info.hColorTexture);

    return info.hColorTexture;
  } else {

    IDBG_LOW("Map miss! Need to add mapping!");

    //bufMapInfo *mapInfo = (bufMapInfo *)malloc(sizeof(bufMapInfo));
    bufMapInfo mapInfo;
    memset(&mapInfo, 0, sizeof(bufMapInfo));
    mapInfo.index = index;
    rc = img_buffer_get(IMG_BUFFER_ION_IOMMU, ION_HEAP(ION_IOMMU_HEAP_ID), 0, ROUND_UP_PAGESIZE(sizeof(MetaData_t)),
      &mapInfo.meta_buffer);

    if (IMG_ERROR(rc)) {
      IDBG_ERROR(" cannot alloc meta buffer");
      //goto end;
    }

    MetaData_t *p_meta_data = (MetaData_t *)mapInfo.meta_buffer.vaddr;

    mapInfo.privateHandle = new private_handle_t(fd, size, mFlags, mBufferType,  mFormat,
                    width,  height, mapInfo.meta_buffer.fd, 0, (uint64_t)p_meta_data);

    if (mapInfo.privateHandle == NULL)
    {
        IDBG_ERROR("create private_handle_t failed!!");
        return (GLuint)0;
    }

    BufferDim_t dim;
    dim.sliceWidth  = stride;
    dim.sliceHeight = scanline;
    int ret = setMetaData(mapInfo.privateHandle, UPDATE_BUFFER_GEOMETRY, &dim);
    if (ret < 0) {
      IDBG_LOW("setMetaData(UPDATE_BUFFER_GEOMETRY) failed %d", ret);
      return (GLuint)0;
    }
    IDBG_LOW(" wxh %dX%d strXsca %dX%d ret %d", width, height, dim.sliceWidth, dim.sliceHeight, ret);

    ColorSpace_t colorSpace = ITU_R_601_FR;
    ret = setMetaData(mapInfo.privateHandle, UPDATE_COLOR_SPACE, &colorSpace);
    if (ret < 0) {
      IDBG_LOW("setMetaData(UPDATE_COLOR_SPACE) failed %d", ret);
      return (GLuint)0;
    }


    mapInfo.pNativeBuffer = new GraphicBuffer(width, height, mFormat, mUsage, stride, mapInfo.privateHandle, 0);
    if (mapInfo.pNativeBuffer == NULL)
    {
        IDBG_ERROR("GraphicBuffer Creation Failed \n");
        return (GLuint)0;
    }

    EGLClientBuffer pNativeBuffer = (EGLClientBuffer)mapInfo.pNativeBuffer->getNativeBuffer();

    mapInfo.hEGLImage = p_eglCreateImageKHR(eglGetCurrentDisplay(), EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_ANDROID, pNativeBuffer, vEGLAttribs);
    if (!mapInfo.hEGLImage)
    {
        IDBG_ERROR("EGLImage creation failed");
        return (GLuint)0;
    }

    glGenTextures(1, &mapInfo.hColorTexture);
    error = glGetError();
    if(error != GL_NO_ERROR) {
      IDBG_ERROR("after glGenTextures eglError (0x%x)", error);
      return 0;
    }
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, mapInfo.hColorTexture);
    checkglError("glBindTexture");
    p_glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, mapInfo.hEGLImage);
    checkEglError("p_glEGLImageTargetTexture2DOES");
    checkglError("p_glEGLImageTargetTexture2DOES");
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    checkglError("glTexParameteri");
    mapInfo.isInputBuffer = TRUE;
    mapInfo.hRBO = 0;
    mapInfo.hFBO = 0;
    //Insert into the map
    IDBG_LOW("Inserting index %d tex %d!", index, mapInfo.hColorTexture);
    mSourceTextureMap[index] = mapInfo;
    return mapInfo.hColorTexture;
  }

  IDBG_LOW("X");
}

GLuint QCameraBufferMap::GetFrameBuffer(uint32_t index, uint32_t fd,
  uint32_t size, int32_t width, int32_t height, int32_t stride, int32_t scanline)
{
  IDBG_LOW("E");
  int rc = 0;
  GLint error = 0;
  std::map<uint32_t ,bufMapInfo> ::iterator it;
  EGLint vEGLAttribs[] = {EGL_WIDTH,  width, EGL_HEIGHT, height, EGL_NONE};

  it = mFrameBufferMap.find(index);
  if (it != mFrameBufferMap.end()) {
    IDBG_LOW("Map found!");
    bufMapInfo info = it->second;

    IDBG_LOW("index %d fbo %d!", index, info.hFBO);

    return info.hFBO;
  } else {

    IDBG_LOW("Map miss! Need to add mapping!");

    bufMapInfo mapInfo;
    memset(&mapInfo, 0, sizeof(bufMapInfo));
    mapInfo.index = index;
    rc = img_buffer_get(IMG_BUFFER_ION_IOMMU, ION_HEAP(ION_IOMMU_HEAP_ID), 0, ROUND_UP_PAGESIZE(sizeof(MetaData_t)),
      &mapInfo.meta_buffer);

    if (IMG_ERROR(rc)) {
      IDBG_ERROR(" cannot alloc meta buffer");
      //goto end;
    }

    MetaData_t *p_meta_data = (MetaData_t *)mapInfo.meta_buffer.vaddr;

    mapInfo.privateHandle = new private_handle_t(fd, size, mFlags,
      mBufferType,  mFormat, width,  height, mapInfo.meta_buffer.fd,
      0, (uint64_t)p_meta_data);

    if (mapInfo.privateHandle == NULL)
    {
        IDBG_ERROR("create private_handle_t failed!!");
        return (GLuint)0;
    }

    BufferDim_t dim;
    dim.sliceWidth  = stride;
    dim.sliceHeight = scanline;
    int ret = setMetaData(mapInfo.privateHandle, UPDATE_BUFFER_GEOMETRY, &dim);
    if (ret < 0) {
      IDBG_ERROR("setMetaData(UPDATE_BUFFER_GEOMETRY) failed %d", ret);
      return (GLuint)0;
    }
    IDBG_LOW(" wxh %dX%d strXsca %dX%d ret %d", width, height, dim.sliceWidth, dim.sliceHeight, ret);

    ColorSpace_t colorSpace = ITU_R_601_FR;
    ret = setMetaData(mapInfo.privateHandle, UPDATE_COLOR_SPACE, &colorSpace);
    if (ret < 0) {
      IDBG_ERROR("setMetaData(UPDATE_COLOR_SPACE) failed %d", ret);
      return (GLuint)0;
    }


    mapInfo.pNativeBuffer = new GraphicBuffer(width, height, mFormat, mUsage, stride, mapInfo.privateHandle, 0);

    if (mapInfo.pNativeBuffer == NULL)
    {
        IDBG_ERROR("GraphicBuffer Creation Failed \n");
        return (GLuint)0;
    }

    EGLClientBuffer pNativeBuffer = (EGLClientBuffer)mapInfo.pNativeBuffer->getNativeBuffer();

    mapInfo.hEGLImage = p_eglCreateImageKHR(eglGetCurrentDisplay(), EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_ANDROID, pNativeBuffer, vEGLAttribs);
    if (!mapInfo.hEGLImage)
    {
        IDBG_ERROR("EGLImage creation failed");
        return (GLuint)0;
    }

    glGenFramebuffers(1, &mapInfo.hFBO);
    error = glGetError();
    if(error != GL_NO_ERROR) {
      IDBG_ERROR("after glGenFramebuffers eglError (0x%x)", error);
      return 0;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, mapInfo.hFBO);
    checkglError("glBindFramebuffer");
    glGenRenderbuffers(1, &mapInfo.hRBO);
    checkglError("glGenRenderbuffers");
    glBindRenderbuffer(GL_RENDERBUFFER, mapInfo.hRBO);
    checkglError("glBindRenderbuffer");
    p_glEGLImageTargetRenderbufferStorageOES(GL_RENDERBUFFER, (GLeglImageOES)mapInfo.hEGLImage);
    checkglError("p_glEGLImageTargetRenderbufferStorageOES");
    checkEglError("p_glEGLImageTargetRenderbufferStorageOES");
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mapInfo.hRBO);
    checkglError("glFramebufferRenderbuffer");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    checkglError("glBindFramebuffer");
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    checkglError("glBindRenderbuffer");

    mapInfo.isInputBuffer = FALSE;
    mapInfo.hColorTexture = 0;
    //Insert into the map
    IDBG_LOW("Inserting index %d fbo %d!", index, mapInfo.hFBO);
    mFrameBufferMap[index] = mapInfo;

    return mapInfo.hFBO;
  }

  IDBG_LOW("X");
}


