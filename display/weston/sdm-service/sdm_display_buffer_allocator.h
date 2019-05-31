/*
* Copyright (c) 2017, The Linux Foundation. All rights reserved.
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

#ifndef __SDM_DISPLAY_BUFFER_ALLOCATOR_H__
#define __SDM_DISPLAY_BUFFER_ALLOCATOR_H__

#include <core/buffer_allocator.h>
#include <gbm.h>
#include <gbm_priv.h>
#include "sdm_display_connect.h"
#include <layer_stack.h>

namespace sdm {

template <class Type>
inline Type ALIGN(Type x, Type align) {
  return (x + align - 1) & ~(align - 1);
}
class SdmDisplayBufferAllocator : public BufferAllocator {
 public:
  SdmDisplayBufferAllocator();
  ~SdmDisplayBufferAllocator() {
    gbm_device_destroy(gbm_);
    gbm_ = NULL;
  };

  DisplayError AllocateBuffer(BufferInfo *buffer_info);
  DisplayError FreeBuffer(BufferInfo *buffer_info);
  uint32_t GetBufferSize(BufferInfo *buffer_info);

  DisplayError GetAllocatedBufferInfo(const BufferConfig &buffer_config,
                                      AllocatedBufferInfo *allocated_buffer_info);
  int SetBufferInfo(LayerBufferFormat format, uint32_t *target, uint64_t *flags);
  DisplayError GetBufferLayout(const AllocatedBufferInfo &buf_info,
                               uint32_t stride[4], uint32_t offset[4],
                               uint32_t *num_planes);
  void GetGbmDeviceHandle(void **userdata);
 private:
  bool IsFormatVideo(uint32_t fmt);
  bool IsVideoFormatLinear(uint32_t fmt, uint32_t ubwc_status);
  bool IsVideoFormatUBWC(uint32_t fmt, uint32_t ubwc_status);
  struct gbm_device *gbm_ = NULL;
};

}  // namespace sdm
#endif  // __SDM_DISPLAY_BUFFER_ALLOCATOR_H__

