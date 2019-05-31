/*
* Copyright (c) 2017-2018, The Linux Foundation. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted
* provided that the following conditions are met:
*    * Redistributions of source code must retain the above copyright notice, this list of
*      conditions and the following disclaimer.
*    * Redistributions in binary form must reproduce the above copyright notice, this list of
*      conditions and the following disclaimer in the documentation and/or other materials provided
*      with the distribution.
*    * Neither the name of The Linux Foundation nor the names of its contributors may be used to
*      endorse or promote products derived from this software without specific prior written
*      permission.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "sdm_display_debugger.h"
#include "sdm_display_buffer_allocator.h"

#define __CLASS__ "SdmDisplayBufferAllocator"

#ifdef __cplusplus
extern "C" {
#endif
namespace sdm {

SdmDisplayBufferAllocator::SdmDisplayBufferAllocator() {

}

LayerBufferFormat GetLayerBufferFormat(uint32_t format, uint32_t ubwc_status) {
   LayerBufferFormat layer_buffer_format = kFormatInvalid;

   switch (format) {
       case GBM_FORMAT_ABGR8888:
          layer_buffer_format = ubwc_status ? kFormatRGBA8888Ubwc: kFormatRGBA8888;
          break;
       case GBM_FORMAT_XBGR8888:
          layer_buffer_format = ubwc_status ? kFormatRGBX8888Ubwc: kFormatRGBX8888;
          break;
       case GBM_FORMAT_BGR888:
          layer_buffer_format = kFormatRGB888;
          break;
       case GBM_FORMAT_BGR565:
          layer_buffer_format = kFormatRGB565;
          break;
       case GBM_FORMAT_RGB565:
          layer_buffer_format = ubwc_status ? kFormatBGR565Ubwc: kFormatBGR565;
          break;
       case GBM_FORMAT_ARGB8888:
          layer_buffer_format = kFormatBGRA8888;
          break;
       case GBM_FORMAT_XRGB8888:
          layer_buffer_format = kFormatBGRX8888;
          break;
       case GBM_FORMAT_NV12:
          layer_buffer_format =
            ubwc_status ? kFormatYCbCr420SPVenusUbwc: kFormatYCbCr420SemiPlanarVenus;
          break;
       case GBM_FORMAT_YCbCr_420_TP10_UBWC:
          layer_buffer_format = kFormatYCbCr420TP10Ubwc;
          break;
       case GBM_FORMAT_P010:
          layer_buffer_format = kFormatYCbCr420P010;
          break;
       case GBM_FORMAT_YCbCr_420_P010_UBWC:
          layer_buffer_format = kFormatYCbCr420P010Ubwc;
          break;
       case GBM_FORMAT_ABGR2101010:
         layer_buffer_format = ubwc_status ? kFormatRGBA1010102Ubwc: kFormatRGBA1010102;
         break;
       default:
         layer_buffer_format = kFormatInvalid;
         break;
   }

   return layer_buffer_format;
}

DisplayError SdmDisplayBufferAllocator::AllocateBuffer(BufferInfo *buffer_info) {
  const BufferConfig &buffer_config = buffer_info->buffer_config;
  AllocatedBufferInfo *alloc_buffer_info = &buffer_info->alloc_buffer_info;
  uint32_t width = buffer_config.width;
  uint32_t height = buffer_config.height;
  uint32_t format;
  int metadata_fd = -1;
  uint64_t alloc_flags = 0;
  uint32_t ubwc_status;
  int error = SetBufferInfo(buffer_config.format, &format, &alloc_flags);
  if (error != 0) {
    return kErrorParameters;
  }

  if (buffer_info->buffer_config.secure) {
     alloc_flags |= GBM_BO_USAGE_PROTECTED_QTI;
  }
  // CreateBuffer
  struct gbm_bo *bo = gbm_bo_create(gbm_, width, height, format, alloc_flags);

  if (bo) {
      alloc_buffer_info->fd = gbm_bo_get_fd(bo);
      alloc_buffer_info->stride = gbm_bo_get_stride(bo);
      uint32_t alignedWidth = 0;
      gbm_perform(GBM_PERFORM_GET_BO_ALIGNED_WIDTH, bo, &alignedWidth);
      alloc_buffer_info->aligned_width = alignedWidth;
      uint32_t alignedHeight = 0;
      gbm_perform(GBM_PERFORM_GET_BO_ALIGNED_HEIGHT, bo, &alignedHeight);
      alloc_buffer_info->aligned_height = alignedHeight;
      size_t bo_size = 0;
      gbm_perform(GBM_PERFORM_GET_BO_SIZE, bo, &bo_size);
      gbm_perform(GBM_PERFORM_GET_UBWC_STATUS, bo, &ubwc_status);
      alloc_buffer_info->size = (uint32_t)bo_size;
      alloc_buffer_info->format = GetLayerBufferFormat(gbm_bo_get_format(bo), ubwc_status);

      gbm_perform(GBM_PERFORM_GET_METADATA_ION_FD, bo, &metadata_fd);

  } else {
      DLOGE("Failed to allocate memory");

      return kErrorMemory;
  }

  buffer_info->private_data = reinterpret_cast<void *>(bo);

  return kErrorNone;
}

DisplayError SdmDisplayBufferAllocator::FreeBuffer(BufferInfo *buffer_info) {
  DisplayError err = kErrorNone;
  struct gbm_bo *bo = reinterpret_cast<struct gbm_bo *>(buffer_info->private_data);
  if (bo)
      gbm_bo_destroy(bo);
  else {
      DLOGE("Unable to destroy bo = NULL.\n");
      err = kErrorParameters;
  }
  buffer_info->private_data = NULL;
  if (err == kErrorNone) {
      AllocatedBufferInfo *alloc_buffer_info = &buffer_info->alloc_buffer_info;
      alloc_buffer_info->fd = -1;
      alloc_buffer_info->stride = 0;
      alloc_buffer_info->size = 0;
      alloc_buffer_info->aligned_height = 0;
      alloc_buffer_info->aligned_width = 0;
      alloc_buffer_info->format = kFormatInvalid;
      buffer_info->private_data = NULL;
      buffer_info->buffer_config = {};
  }
  return err;
}

uint32_t SdmDisplayBufferAllocator::GetBufferSize(BufferInfo *buffer_info) {

  uint32_t size = 0;
  const BufferConfig &bufferConfig = buffer_info->buffer_config;
  uint64_t usageFlags = 0;
  uint32_t gbmFormat = 0;
  struct gbm_buf_info bufInfo;

  if (SetBufferInfo(bufferConfig.format, &gbmFormat, &usageFlags) < 0) {
     return 0;
  }
  if (bufferConfig.secure) {
     usageFlags |= GBM_BO_USAGE_PROTECTED_QTI;
  }
  bufInfo.width = INT(bufferConfig.width);
  bufInfo.height = INT(bufferConfig.height);
  bufInfo.format = gbmFormat;

  uint32_t alignedWidth = 0;
  uint32_t alignedHeight = 0;

  gbm_perform(GBM_PERFORM_GET_BUFFER_SIZE_DIMENSIONS, &bufInfo, usageFlags,
              &alignedWidth, &alignedHeight, &size);

  return size;
}

int SdmDisplayBufferAllocator::SetBufferInfo(LayerBufferFormat format, uint32_t *target, uint64_t *flags) {
  switch (format) {
  case kFormatRGBA8888:                 *target = GBM_FORMAT_ABGR8888;
                                        break;
  case kFormatRGBX8888:                 *target = GBM_FORMAT_XBGR8888;
                                        break;
  case kFormatRGB888:                   *target = GBM_FORMAT_BGR888;               break;
  case kFormatRGB565:                   *target = GBM_FORMAT_BGR565;
                                        break;
  case kFormatBGR565:                   *target = GBM_FORMAT_RGB565;               break;
  case kFormatBGRA8888:                 *target = GBM_FORMAT_ARGB8888;             break;
  case kFormatBGRX8888:                 *target = GBM_FORMAT_XRGB8888;             break;
  case kFormatYCbCr420SemiPlanarVenus:  *target = GBM_FORMAT_NV12;                 break;
  case kFormatYCbCr420SPVenusUbwc:      *target = GBM_FORMAT_NV12;
                                        *flags = GBM_BO_USAGE_UBWC_ALIGNED_QTI;
                                        break;
  case kFormatRGBA8888Ubwc:             *target = GBM_FORMAT_ABGR8888;
                                        *flags = GBM_BO_USAGE_UBWC_ALIGNED_QTI |
                                                 GBM_BO_USAGE_HW_RENDERING_QTI;
                                        break;
  case kFormatRGBX8888Ubwc:             *target = GBM_FORMAT_XBGR8888;
                                        *flags = GBM_BO_USAGE_UBWC_ALIGNED_QTI |
                                                 GBM_BO_USAGE_HW_RENDERING_QTI;
                                        break;
  case kFormatBGR565Ubwc:              *target = GBM_FORMAT_RGB565;
                                        *flags = GBM_BO_USAGE_UBWC_ALIGNED_QTI |
                                                 GBM_BO_USAGE_HW_RENDERING_QTI;
                                        break;
  case kFormatABGR2101010:              *target = GBM_FORMAT_ABGR2101010;
                                        break;
  case kFormatRGBA1010102:              *target = GBM_FORMAT_ABGR2101010;
                                        break;
  case kFormatRGBA1010102Ubwc:          *target = GBM_FORMAT_ABGR2101010;
                                        *flags = GBM_BO_USAGE_UBWC_ALIGNED_QTI |
                                                 GBM_BO_USAGE_HW_RENDERING_QTI;
                                        break;
  case kFormatYCbCr420TP10Ubwc:         *target = GBM_FORMAT_YCbCr_420_TP10_UBWC;  break;
  case kFormatYCbCr420P010Ubwc:         *target = GBM_FORMAT_YCbCr_420_P010_UBWC;  break;
  case kFormatYCbCr420P010:             *target = GBM_FORMAT_P010;  break;
  default:
    DLOGE("Unsupported format = 0x%x", format);
    return -1;
  }

  return 0;
}

void SdmDisplayBufferAllocator::GetGbmDeviceHandle(void **userdata) {
  *userdata = (void *) gbm_;
}

DisplayError SdmDisplayBufferAllocator::GetAllocatedBufferInfo(const BufferConfig \
                                                               &buffer_config,
                                                               AllocatedBufferInfo \
                                                               *allocated_buffer_info) {
  /* This API does not fill or provide stride to the caller in AllocatedBufferInfo structure */
  uint64_t usageFlags = 0;
  uint32_t gbmFormat = 0;
  struct gbm_buf_info bufInfo;
  uint32_t alignedWidth = 0;
  uint32_t alignedHeight = 0;
  uint32_t size = 0;

  if (SetBufferInfo(buffer_config.format, &gbmFormat, &usageFlags) < 0) {
     return kErrorParameters;
  }
  if (buffer_config.secure) {
     usageFlags |= GBM_BO_USAGE_PROTECTED_QTI;
  }
  bufInfo.width = INT(buffer_config.width);
  bufInfo.height = INT(buffer_config.height);
  bufInfo.format = gbmFormat;

  gbm_perform(GBM_PERFORM_GET_BUFFER_SIZE_DIMENSIONS, &bufInfo, usageFlags,
              &alignedWidth, &alignedHeight, &size);
  DLOGI("aligned_width:%d, aligned_height:%d, size:%d, gbm_format:0x%x usage_flags:0x%x",
         alignedWidth, alignedHeight, size, gbmFormat, usageFlags);

  allocated_buffer_info->aligned_width = alignedWidth;
  allocated_buffer_info->aligned_height = alignedHeight;
  allocated_buffer_info->size = size;
  allocated_buffer_info->format = buffer_config.format;

  return kErrorNone;
}

bool SdmDisplayBufferAllocator::IsFormatVideo(uint32_t fmt)
{
   bool is_video_present = false;

   switch (fmt) {
      case GBM_FORMAT_NV12:
      case GBM_FORMAT_YCbCr_420_TP10_UBWC:
      case GBM_FORMAT_P010:
           is_video_present = true;
           break;
      default:
           is_video_present = false;
           break;
   }

   return is_video_present;
}

bool SdmDisplayBufferAllocator::IsVideoFormatLinear(uint32_t fmt, uint32_t ubwc_status)
{
   bool is_videofmt_linear = false;

   if (!ubwc_status) {
     switch (fmt) {
      case GBM_FORMAT_NV12:
      case GBM_FORMAT_P010:
        is_videofmt_linear = true;
        break;
      default:
        DLOGW("Unsupported format 0x%x\n", fmt);
        is_videofmt_linear = false;
        break;
     }
   }

  return is_videofmt_linear;
}

bool SdmDisplayBufferAllocator::IsVideoFormatUBWC(uint32_t fmt, uint32_t ubwc_status)
{
   bool is_videofmt_ubwc = false;

   if (ubwc_status) {
     switch (fmt) {
      case GBM_FORMAT_NV12:
      case GBM_FORMAT_YCbCr_420_TP10_UBWC:
        is_videofmt_ubwc = true;
        break;
      default:
        DLOGW("Unsupported format 0x%x\n", fmt);
        is_videofmt_ubwc = false;
        break;
     }
   }

  return is_videofmt_ubwc;
}

DisplayError SdmDisplayBufferAllocator::GetBufferLayout(const AllocatedBufferInfo &buf_info,
                                                 uint32_t stride[4], uint32_t offset[4],
                                                 uint32_t *num_planes) {
    struct gbm_bo *bo;
    struct gbm_import_fd_data import_fd_data;
    uint32_t format1 = GBM_FORMAT_ARGB8888;
    uint64_t flags = 0;
    uint32_t ubwc_status = 0;
    generic_buf_layout_t buf_layout;

    SetBufferInfo(buf_info.format, &format1, &flags);

    import_fd_data.fd = buf_info.fd;
    import_fd_data.format = format1;
    import_fd_data.width = buf_info.aligned_width;
    import_fd_data.height = buf_info.aligned_height;

    // Import gbm bo from buf_info
    bo = gbm_bo_import(gbm_, GBM_BO_IMPORT_FD, &import_fd_data, flags);

    if (bo == NULL) {
        return kErrorNone;
    }

    uint32_t width, height;
    uint32_t *fbid;
    uint32_t fb_id, stride1, handle, size, format;
    uint32_t fb_id1;
    width = gbm_bo_get_width(bo);
    height = gbm_bo_get_height(bo);
    stride1 = gbm_bo_get_stride(bo);
    handle = gbm_bo_get_handle(bo).u32;
    format = gbm_bo_get_format(bo);

    if (IsFormatVideo(format) == false) {
      stride[0] = gbm_bo_get_stride(bo);
      offset[0] = 0;
      *num_planes++;
      gbm_bo_destroy(bo);
      return kErrorNone;
    }

    // for NV12 format
    *num_planes = 2;
    gbm_perform(GBM_PERFORM_GET_PLANE_INFO, bo, &buf_layout);
    gbm_perform(GBM_PERFORM_GET_UBWC_STATUS, bo, &ubwc_status);

    if (IsVideoFormatLinear(format, ubwc_status)) {
      stride[0] = buf_layout.planes[0].v_increment;
      offset[0] = 0;

      stride[1] = stride[0];
      offset[1] = stride[0]*height;
    }
    if (IsVideoFormatUBWC(format, ubwc_status)) {
      stride[0] = buf_layout.planes[0].v_increment;
      stride[1] = buf_layout.planes[1].v_increment;
      offset[0] = 0;
      offset[1] = 0;
    }

  gbm_bo_destroy(bo);

  return kErrorNone;
}

}  // namespace sdm
#ifdef __cplusplus
}
#endif
