/*
 * Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
 * Not a Contribution.
 *
 * Copyright 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "EGLImageBufferLE.h"
#include <map>
#include "EGLImageWrapperLE.h"
#include "glengine.h"
#include "drm_master.h"
#include <EGL/eglwaylandext.h>
#include <wayland-server.h>
using namespace drm_utils;

//-----------------------------------------------------------------------------
EGLImageKHR EGLImageBufferLE::create_eglImage(struct gbm_buf_info *gbo_info, void *userdata)
//-----------------------------------------------------------------------------
{
  unsigned int gbm_ret;
  unsigned int secure_status = 0;
  EGLImageKHR eglImage;
  PFNEGLCREATEIMAGEKHRPROC create_image;
  create_image = reinterpret_cast<PFNEGLCREATEIMAGEKHRPROC>
                                  (eglGetProcAddress("eglCreateImageKHR"));

  EGLint attribs[20];
  struct gbm_bo *bo = NULL;
  int i=0;

  bo = gbm_bo_import(gbm_, GBM_BO_IMPORT_GBM_BUF_TYPE, gbo_info,
                        GBM_BO_USE_SCANOUT|GBM_BO_USE_RENDERING);

  gbm_perform(GBM_PERFORM_GET_SECURE_BUFFER_STATUS, bo, &secure_status);
  //We need to pass wl_resource to create egl image to support TP10_UBWC and NV12 formats in
  // Forward tone mapper
  if(gbo_info->format == GBM_FORMAT_YCbCr_420_TP10_UBWC || gbo_info->format == GBM_FORMAT_NV12) {
   attribs[i++] = EGL_WAYLAND_PLANE_WL;
   attribs[i++] = 0;
   attribs[i++] = EGL_PROTECTED_CONTENT_EXT;
   attribs[i++] = secure_status;
   attribs[i++] = EGL_NONE;

   EGLClientBuffer buffer = reinterpret_cast<EGLClientBuffer>(userdata);
   eglImage = create_image(eglGetCurrentDisplay(), (EGLContext)EGL_NO_CONTEXT,
                                      EGL_WAYLAND_BUFFER_WL, buffer, attribs);
   }
  else {
   attribs[i++] = EGL_WIDTH;
   attribs[i++] = gbm_bo_get_width(bo);
   attribs[i++] = EGL_HEIGHT;
   attribs[i++] = gbm_bo_get_height(bo);
   attribs[i++] = EGL_LINUX_DRM_FOURCC_EXT;
   attribs[i++] = gbm_bo_get_format(bo);
   attribs[i++] = EGL_DMA_BUF_PLANE0_FD_EXT;
   attribs[i++] = gbm_bo_get_fd(bo);
   attribs[i++] = EGL_DMA_BUF_PLANE0_OFFSET_EXT;
   attribs[i++] = 0;
   attribs[i++] = EGL_DMA_BUF_PLANE0_PITCH_EXT;
   attribs[i++] = gbm_bo_get_stride(bo);
   attribs[i++] = EGL_PROTECTED_CONTENT_EXT;
   attribs[i++] = secure_status;
   attribs[i++] = EGL_NONE;

   eglImage = create_image(eglGetCurrentDisplay(), (EGLContext)EGL_NO_CONTEXT,
                                           EGL_LINUX_DMA_BUF_EXT, NULL, attribs);
  }
   // we no longer need the bo
  if (bo) {
    gbm_bo_destroy(bo);
  }

  return eglImage;
}

//-----------------------------------------------------------------------------
EGLImageBufferLE::EGLImageBufferLE(struct gbm_buf_info *gbuf_info, void *userdata, void *userdata2)
//-----------------------------------------------------------------------------
{

    struct gbm_buf_info *gbo_info = gbuf_info;

    DRMMaster *master = nullptr;
    int ret = DRMMaster::GetInstance(&master);

    if (ret < 0) {
        fprintf(stderr, "Failed to acquire DRMMaster instance\n");
    }

    master->GetHandle(&fd);

    gbm_ = (gbm_device*) userdata2;

    this->eglImageID = create_eglImage(gbo_info, userdata);
    this->width = gbo_info->width;
    this->height = gbo_info->height;

    textureID = 0;
    renderbufferID = 0;
    framebufferID = 0;
}

//-----------------------------------------------------------------------------
EGLImageBufferLE::~EGLImageBufferLE() {
//-----------------------------------------------------------------------------
  if (textureID != 0) {
    GL(glDeleteTextures(1, &textureID));
    textureID = 0;
  }

  if (renderbufferID != 0) {
    GL(glDeleteRenderbuffers(1, &renderbufferID));
    renderbufferID = 0;
  }

  if (framebufferID != 0) {
    GL(glDeleteFramebuffers(1, &framebufferID));
    framebufferID = 0;
  }

  // Delete the eglImage
  if (eglImageID != 0)
  {
      eglDestroyImageKHR(eglGetCurrentDisplay(), eglImageID);
      eglImageID = 0;
  }

  /* static variable initialized is for 2 purpose: */
  /* 1: to help initialize by getting master fd and opening gbm device first time */
  /* 2: On every buffer creation instance, a reference count is added to it to    */
  /*    keep track of how many times this object has been instantiation. It is    */
  /*    decremented in destructor. But if the object to be destroyed is with      */
  /*    reference count = 1, then fd is set to invalid number and gbm device is   */
  /*    destroyed */

  fd = -1;
};

static EGLImageBufferLE EGLImageBufferLE::*from(const void *src) {

}
