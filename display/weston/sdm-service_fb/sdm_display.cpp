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
/*
 * Copyright © 2008-2011 Kristian Høgsberg
 * Copyright © 2011 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <unistd.h>
#include "sdm_display.h"
//#include "compositor-sdm-output.h"
#ifdef __cplusplus
extern "C" {
#endif


#include "../src/linux-dmabuf.h"

#define __CLASS__ "SdmDisplay"




namespace sdm {

SdmDisplay::SdmDisplay(DisplayType type, CoreInterface *core_intf,
                                         SdmDisplayBufferAllocator *buffer_allocator) {
    display_type_ = type;
    core_intf_    = core_intf;
    buffer_allocator_ = buffer_allocator;
}

SdmDisplay::~SdmDisplay() {
}


DisplayError SdmDisplay::CreateDisplay() {
    DisplayError error = kErrorNone;

    error = core_intf_->CreateDisplay(display_type_, this, &display_intf_);

    if (error != kErrorNone) {
        DLOGE("Display creation failed. Error = %d", error);
        return error;
    }
    display_intf_->GetFrameBufferConfig(&fb_config_);
    display_intf_->SetDisplayState(kStateOn);
    //display_intf_->SetVSyncState(true);
    display_intf_->SetPanelBrightness(255);
    display_intf_->Flush();
    layer_stack_.layers.clear();
    gpu_target_layer_ = new Layer();
    dummy_layer_ = new Layer();
    PrepareFbLayerGeometry();
    *dummy_layer_ = *gpu_target_layer_;
    dummy_layer_->flags.skip = 1;
    dummy_layer_->composition = kCompositionSDE;
    gpu_target_layer_->composition = sdm::kCompositionGPUTarget;
    layer_stack_.layers.push_back(dummy_layer_);
    layer_stack_.layers.push_back(gpu_target_layer_);
    return kErrorNone;
}

DisplayError SdmDisplay::DestroyDisplay() {
    
    return kErrorNone;
}

void SdmDisplay::PrepareFbLayerGeometry()
{
    LayerBuffer &buffer = gpu_target_layer_->input_buffer;
    buffer.height = fb_config_.y_pixels;
    buffer.unaligned_width = fb_config_.x_pixels;
    buffer.unaligned_height = fb_config_.y_pixels;
    buffer.planes[0].offset = 0;
    buffer.format = kFormatRGBA8888;

    if (buffer.acquire_fence_fd >= 0) {
        close(buffer.acquire_fence_fd);
        buffer.acquire_fence_fd = -1;
     }

    gpu_target_layer_->src_rect.left = 0.0f;
    gpu_target_layer_->src_rect.top = 0.0f;
    gpu_target_layer_->src_rect.right = fb_config_.x_pixels;
    gpu_target_layer_->src_rect.bottom = fb_config_.y_pixels;

    gpu_target_layer_->dst_rect.left = 0.0f;
    gpu_target_layer_->dst_rect.top = 0.0f;
    gpu_target_layer_->dst_rect.right = fb_config_.x_pixels;
    gpu_target_layer_->dst_rect.bottom = fb_config_.y_pixels;
    gpu_target_layer_->blending = kBlendingOpaque;
}

DisplayError SdmDisplay::Commit(int fd)
{
    DisplayError ret = kErrorNone;
    gpu_target_layer_->input_buffer.planes[0].fd = fd;
    ret = display_intf_->Prepare(&layer_stack_);
    ret = display_intf_->Commit(&layer_stack_);
    PostCommit();
    return ret;
}

void SdmDisplay::PostCommit()
{

 //Iterate through the layer buffer and close release fences
    for (uint32_t i = 0; i < layer_stack_.layers.size(); i++) {
        Layer *layer = layer_stack_.layers.at(i);
        LayerBuffer *layer_buffer = &layer->input_buffer;

        if (layer_buffer->release_fence_fd > 0) {
          close(layer_buffer->release_fence_fd);
          layer_buffer->release_fence_fd = -1;
        }
    }
    //close release fence fds
    if (layer_stack_.retire_fence_fd > 0) {
      close(layer_stack_.retire_fence_fd);
      layer_stack_.retire_fence_fd = -1;
    }
}
 DisplayError SdmDisplay::VSync(const DisplayEventVSync &vsync)
{
    return kErrorNone;
}

 DisplayError SdmDisplay::Refresh() 
{
    return kErrorNone;
}
 DisplayError SdmDisplay::CECMessage(char *message)
{
    return kErrorNone;
}

DisplayError SdmDisplay::VSync(int fd, unsigned int sequence,
                             unsigned int tv_sec, unsigned int tv_usec,
                             void *data)
{
    return kErrorNone;
}


DisplayError SdmDisplay::PFlip(int fd, unsigned int sequence,
                             unsigned int tv_sec, unsigned int tv_usec,
                             void *data)
{
    return kErrorNone;
}

void SdmDisplay::SetFBTStride(int stride)
{
    LayerBuffer &buffer = gpu_target_layer_->input_buffer;
    buffer.width = stride / 4;
    buffer.planes[0].stride = stride;
}

#ifdef __cplusplus
}
#endif

}  // namespace sdm
