/*
* Copyright (c) 2016 - 2017, The Linux Foundation. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*  * Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  * Redistributions in binary form must reproduce the above
*    copyright notice, this list of conditions and the following
*    disclaimer in the documentation and/or other materials provided
*    with the distribution.
*  * Neither the name of The Linux Foundation nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
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
*/

#include <TonemapFactoryLE.h>
#include <utils/constants.h>
#include <utils/debug.h>
#include <utils/formats.h>
#include <utils/rect.h>
#include <utils/utils.h>
#include <gbm.h>
#include <gbm_priv.h>
#include <vector>
#include <string.h>
#include "sdm_display_debugger.h"
#include "sdm_display_tonemapper.h"

using std::string;
#define __CLASS__ "SdmDisplayToneMapper"

namespace sdm {

ToneMapSession::ToneMapSession(SdmDisplayBufferAllocator *buffer_allocator) :
                buffer_allocator_(buffer_allocator) {
  buffer_info_.resize(kNumIntermediateBuffers);
}

ToneMapSession::~ToneMapSession() {
  delete gpu_tone_mapper_;
  gpu_tone_mapper_ = nullptr;
  FreeIntermediateBuffers();
  buffer_info_.clear();
  buffer_allocator_ = nullptr;
}

DisplayError ToneMapSession::AllocateIntermediateBuffers(const Layer *layer) {
  DisplayError error = kErrorNone;
  for (uint8_t i = 0; i < kNumIntermediateBuffers; i++) {
    BufferInfo &buffer_info = buffer_info_[i];
    buffer_info.buffer_config.width = layer->request.width;
    buffer_info.buffer_config.height = layer->request.height;
    buffer_info.buffer_config.format = layer->request.format;
    buffer_info.buffer_config.secure = layer->request.flags.secure;
    error = buffer_allocator_->AllocateBuffer(&buffer_info);
    if (error != kErrorNone) {
      FreeIntermediateBuffers();
      return error;
    }
  }

  return kErrorNone;
}

void ToneMapSession::FreeIntermediateBuffers() {
  for (uint8_t i = 0; i < kNumIntermediateBuffers; i++) {
    // Free the valid fence
    if (release_fence_fd_[i] >= 0) {
      CloseFd(&release_fence_fd_[i]);
    }
    BufferInfo &buffer_info = buffer_info_[i];
    buffer_allocator_->FreeBuffer(&buffer_info);
  }
}

void ToneMapSession::UpdateBuffer(int acquire_fence, LayerBuffer *buffer) {
  // Acquire fence will be closed by HDMI Display.
  // Fence returned by GPU will be closed in PostCommit.
  buffer->acquire_fence_fd = acquire_fence;
  buffer->size = buffer_info_[current_buffer_index_].alloc_buffer_info.size;
  buffer->planes[0].fd = buffer_info_[current_buffer_index_].alloc_buffer_info.fd;
}

void ToneMapSession::SetReleaseFence(int fd) {
  CloseFd(&release_fence_fd_[current_buffer_index_]);
  // Used to give to GPU tonemapper along with input layer fd
  release_fence_fd_[current_buffer_index_] = dup(fd);
}

void ToneMapSession::SetToneMapConfig(Layer *layer) {
  // HDR -> SDR is FORWARD and SDR - > HDR is INVERSE
  tone_map_config_.type = layer->input_buffer.flags.hdr ? TONEMAP_FORWARD : TONEMAP_INVERSE;
  tone_map_config_.colorPrimaries = layer->input_buffer.color_metadata.colorPrimaries;
  tone_map_config_.transfer = layer->input_buffer.color_metadata.transfer;
  tone_map_config_.secure = layer->request.flags.secure;
  tone_map_config_.format = layer->request.format;
}

bool ToneMapSession::IsSameToneMapConfig(Layer *layer) {
  LayerBuffer& buffer = layer->input_buffer;

  int tonemap_type = buffer.flags.hdr ? TONEMAP_FORWARD : TONEMAP_INVERSE;

  return ((tonemap_type == tone_map_config_.type) &&
          (buffer.color_metadata.colorPrimaries == tone_map_config_.colorPrimaries) &&
          (buffer.color_metadata.transfer == tone_map_config_.transfer) &&
          (layer->request.flags.secure == tone_map_config_.secure) &&
          (layer->request.format == tone_map_config_.format) &&
          (layer->request.width == UINT32(buffer.unaligned_width)) &&
          (layer->request.height == UINT32(buffer.unaligned_height)));
}

int SdmDisplayToneMapper::HandleToneMap(LayerStack *layer_stack) {
  uint32_t gpu_count = 0;
  DisplayError error = kErrorNone;
  for (uint32_t i = 0; i < layer_stack->layers.size(); i++) {
    uint32_t session_index = 0;
    Layer *temp1;
    Layer *layer = layer_stack->layers.at(i);
    if (layer->composition == kCompositionGPU) {
      gpu_count++;
    }

    if (layer->request.flags.tone_map) {
      switch (layer->composition) {
         case kCompositionGPUTarget:
           if (!gpu_count) {
             // When all layers are on FrameBuffer and if they do not update in the next draw cycle,
             // then SDM marks them for SDE Composition because the cached FB layer gets displayed.
             // GPU count will be 0 in this case. Try to use the existing tone-mapped frame buffer.
             // No ToneMap/Blit is required. Just update the buffer & acquire fence fd of FB layer.
             if (!tone_map_sessions_.empty()) {
               ToneMapSession *fb_tone_map_session = tone_map_sessions_.at(fb_session_index_);
               fb_tone_map_session->UpdateBuffer(-1 /* acquire_fence */, &layer->input_buffer);
               fb_tone_map_session->layer_index_ = INT(i);
               fb_tone_map_session->acquired_ = true;
               return 0;
             }
           }
           temp1 = layer;

           error = AcquireToneMapSession(layer, &session_index);

           layer = temp1;
           layer = layer_stack->layers.at(i);
           fb_session_index_ = session_index;
           break;
         default:
           error = AcquireToneMapSession(layer, &session_index);
           break;
      }

      if (error != kErrorNone) {
        Terminate();
        return -1;
      }

      ToneMapSession *session = tone_map_sessions_.at(session_index);
      ToneMap(layer, session);
      session->layer_index_ = INT(i);
    }
  }
  return 0;
}

void SdmDisplayToneMapper::ToneMap(Layer* layer, ToneMapSession *session) {

  int fence_fd = -1;
  int acquire_fd = -1;
  int merged_fd = -1;

  uint8_t buffer_index = session->current_buffer_index_;
  BufferInfo buffer_info = session->buffer_info_[buffer_index];

  // invoke perform call to get metadata
  int metadata_fd = -1;

  struct gbm_bo *bo = reinterpret_cast<struct gbm_bo *>(buffer_info.private_data);
  gbm_perform(GBM_PERFORM_GET_METADATA_ION_FD, bo, &metadata_fd);

  struct gbm_buf_info gbo_info;
  gbo_info.fd = buffer_info.alloc_buffer_info.fd;
  gbo_info.metadata_fd = metadata_fd;
  gbo_info.width = buffer_info.buffer_config.width;
  gbo_info.height = buffer_info.buffer_config.height;

  uint32_t gbm_format;
  uint64_t alloc_flags = 0;
  int error = buffer_allocator_->SetBufferInfo(buffer_info.alloc_buffer_info.format,
                                               &gbm_format, &alloc_flags);
  gbo_info.format = gbm_format;

  void *dst_hnd = static_cast<void *>(&gbo_info);

  error = buffer_allocator_->SetBufferInfo(layer->input_buffer.format, &gbm_format, &alloc_flags);

  struct gbm_buf_info gbuf_info;

  gbuf_info.fd = layer->input_buffer.planes[0].fd;
  gbuf_info.metadata_fd  = -1;
  gbuf_info.width = layer->input_buffer.width;
  gbuf_info.height = layer->input_buffer.height;
  gbuf_info.format = gbm_format;

  void *src_hnd = static_cast<void *>(&gbuf_info);
  buffer_allocator_->GetGbmDeviceHandle(&layer->userdata2);
  fence_fd = session->gpu_tone_mapper_->blit(dst_hnd, src_hnd, merged_fd, layer->userdata,
                                              layer->userdata2);
  DumpToneMapOutput(session, &fence_fd);

  session->UpdateBuffer(fence_fd, &layer->input_buffer);
}

void SdmDisplayToneMapper::PostCommit(LayerStack *layer_stack) {
  auto it = tone_map_sessions_.begin();
  while (it != tone_map_sessions_.end()) {
    uint32_t session_index = UINT32(std::distance(tone_map_sessions_.begin(), it));
    ToneMapSession *session = tone_map_sessions_.at(session_index);
    if (session->acquired_) {
      Layer *layer = layer_stack->layers.at(UINT32(session->layer_index_));
      // Close the fd returned by GPU ToneMapper and set release fence.
      LayerBuffer &layer_buffer = layer->input_buffer;
      CloseFd(&layer_buffer.acquire_fence_fd);
      session->SetReleaseFence(layer_buffer.release_fence_fd);
      session->acquired_ = false;
      it++;
    } else {
      delete session;
      it = tone_map_sessions_.erase(it);
    }
  }
}

void SdmDisplayToneMapper::Terminate() {
  if (tone_map_sessions_.size()) {
    while (!tone_map_sessions_.empty()) {
      delete tone_map_sessions_.back();
      tone_map_sessions_.pop_back();
    }
    TonemapperFactory_Destroy();
    fb_session_index_ = 0;
  }
}

void SdmDisplayToneMapper::SetFrameDumpConfig(uint32_t count) {
  dump_frame_count_ = count;
  dump_frame_index_ = 0;
}

void SdmDisplayToneMapper::DumpToneMapOutput(ToneMapSession *session, int *acquire_fd) {

  DLOGD("Not supported");
}

DisplayError SdmDisplayToneMapper::AcquireToneMapSession(Layer *layer, uint32_t *session_index) {
  Color10Bit *grid_entries = NULL;
  int grid_size = 0;

  if (layer->lut_3d.validGridEntries) {
    grid_entries = layer->lut_3d.gridEntries;
    grid_size = INT(layer->lut_3d.gridSize);
  }

  // When the property sdm.disable_hdr_lut_gen is set, the lutEntries and gridEntries in
  // the Lut3d will be NULL, clients needs to allocate the memory and set correct 3D Lut
  // for Tonemapping.
  if (!layer->lut_3d.lutEntries || !layer->lut_3d.dim) {
    // Atleast lutEntries must be valid for GPU Tonemapper.
    return kErrorParameters;
  }
  // Check if we can re-use an existing tone map session.
  for (uint32_t i = 0; i < tone_map_sessions_.size(); i++) {
    ToneMapSession *tonemap_session = tone_map_sessions_.at(i);
    if (!tonemap_session->acquired_ && tonemap_session->IsSameToneMapConfig(layer)) {
      tonemap_session->current_buffer_index_ = (tonemap_session->current_buffer_index_ + 1) %
                                                ToneMapSession::kNumIntermediateBuffers;
      tonemap_session->acquired_ = true;
      *session_index = i;
      return kErrorNone;
    }
  }

  ToneMapSession *session = new ToneMapSession(buffer_allocator_);

  session->SetToneMapConfig(layer);
  session->gpu_tone_mapper_ = TonemapperFactory_GetInstance(session->tone_map_config_.type,
                                                            layer->lut_3d.lutEntries,
                                                            layer->lut_3d.dim,
                                                            grid_entries, grid_size,
                                                            layer->request.flags.secure);
  if (session->gpu_tone_mapper_ == NULL) {
    delete session;
    return kErrorNotSupported;
  }

  DisplayError error = session->AllocateIntermediateBuffers(layer);
  if (error != kErrorNone) {
    delete session;
    return error;
  }

  session->acquired_ = true;
  tone_map_sessions_.push_back(session);
  *session_index = UINT32(tone_map_sessions_.size() - 1);

  return kErrorNone;
}

}  // namespace sdm
