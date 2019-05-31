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
#include <assert.h>
#include <stdarg.h>
#include <linux/limits.h>
#include <string>
#include <vector>
#include <map>
#include <ostream>
#include <fstream>
#include <utility>
#include <bitset>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include "sdm_display.h"
#include "uevent.h"

#ifdef __cplusplus
extern "C" {
#endif


#include "../src/linux-dmabuf.h"

#define __CLASS__ "SdmDisplay"

struct drm_output *drm_output_;
vblank_cb_t vblank_cb_;
int tone_mapper_disable = 0; /* (user): enable this flag once  */
                             /* To disable tone mapping functionality. */

namespace sdm {
#define GET_GPU_TARGET_SLOT(max_layers) ((max_layers) - 1)
/* Cursor is fixed in (gpu_target_index-1) slot in SDM */
#define GET_CURSOR_SLOT(max_layers) ((max_layers) - 2)
#define LEN_LOCAL 2048

#define SDM_DISPLAY_DEBUG 0
#define SDM_DISPLAY_DUMP_LAYER_STACK 0

#define SDM_DEAFULT_NULL_DISPLAY_WIDTH 1920
#define SDM_DEAFULT_NULL_DISPLAY_HEIGHT 1080
#define SDM_DEAFULT_NULL_DISPLAY_FPS 60
#define SDM_DEAFULT_NULL_DISPLAY_X_DPI 25.4
#define SDM_DEAFULT_NULL_DISPLAY_Y_DPI 25.4
#define SDM_DEAFULT_NULL_DISPLAY_IS_YUV false

#define MAX_PROP_STR_SIZE 64
#define SDM_NULL_DISPLAY_RESOLUTON_PROP_NAME "weston.sdm.default.resolution"

SdmDisplay::SdmDisplay(DisplayType type, CoreInterface *core_intf,
                                         SdmDisplayBufferAllocator *buffer_allocator) {
    display_type_ = type;
    core_intf_    = core_intf;
    buffer_allocator_ = buffer_allocator;
    drm_output_   = NULL;
    vblank_cb_    = NULL;
}

SdmDisplay::~SdmDisplay() {
    FreeLayerStack();
}

const char * SdmDisplay::FourccToString(uint32_t fourcc)
{
    static __thread char s[4];
    uint32_t fmt = htole32(fourcc);

    memcpy(s, &fmt, 4);

    return s;
}

DisplayError SdmDisplay::CreateDisplay() {
    DisplayError error = kErrorNone;
    struct DisplayHdrInfo display_hdr_info;
    struct DisplayHdcpProtocol display_hdcp_protocol;

    error = core_intf_->CreateDisplay(display_type_, this, &display_intf_);

    if (error != kErrorNone) {
        DLOGE("Display creation failed. Error = %d", error);

        return error;
    }

    SdmDisplayDebugger::Get()->GetProperty("sys.weston_disable_hdr", &disable_hdr_handling_);
    if (disable_hdr_handling_) {
        DLOGI("HDR Handling disabled");
    }

    SdmDisplayDebugger::Get()->GetProperty("sys.weston_disable_hdr_tm", &tone_mapper_disable);
    if (!tone_mapper_disable && !disable_hdr_handling_) {
        DLOGI("Tone Mapper Enabled");
        tone_mapper_ = new SdmDisplayToneMapper(buffer_allocator_);

        if (!tone_mapper_)
            DLOGI("Failed to create tone_mapper instance");
    }

    GetHdrInfo(&display_hdr_info);

    if (hdr_supported_) {
        DLOGI("Display Device supports HDR functionality");
    } else {
        DLOGI("Display Device doesn't support HDR functionality");
    }

    GetHdcpProtocol(&display_hdcp_protocol);
    /* TODO: */
    if (hdcp_version_) {
        DLOGI("Display Device supports HDCP functionality");
    } else {
        DLOGI("Display Device doesn't support HDCP functionality");
    }

    return kErrorNone;
}

DisplayError SdmDisplay::DestroyDisplay() {
    DisplayError error;

    error = core_intf_->DestroyDisplay(display_intf_);
    display_intf_ = NULL;

    delete tone_mapper_;
    tone_mapper_ = nullptr;

    return error;
}

DisplayError SdmDisplay::VSync(const DisplayEventVSync &vsync) {
    DLOGW("Not implemented");

    return kErrorNone;
}

DisplayError SdmDisplay::VSync(int fd, unsigned int sequence, unsigned int tv_sec,
                               unsigned int tv_usec, void *data) {

    vblank_cb_(sequence, tv_sec, tv_usec, drm_output_);

    return kErrorNone;
}

DisplayError SdmDisplay::PFlip(int fd, unsigned int sequence, unsigned int tv_sec,
                               unsigned int tv_usec, void *data) {

    DLOGW("Not implemented");
    return kErrorNone;
}

DisplayError SdmDisplay::Refresh() {
    if (client_event_handler_) {
        client_event_handler_->Refresh();
    }

    return kErrorNone;
}

DisplayError SdmDisplay::CECMessage(char *message) {
    DLOGW("Not implemented");

    return kErrorNone;
}

DisplayError SdmDisplay::SetDisplayState(DisplayState state) {
    DisplayError error;

    error = display_intf_->SetDisplayState(state);
    if (error != kErrorNone) {
        DLOGE("function failed. Error = %d", error);
     return error;
    }

    return kErrorNone;
}

DisplayError SdmDisplay::SetVSyncState(bool VSyncState, struct drm_output *output) {
    DisplayError error;

    drm_output_ = output;
    error = display_intf_->SetVSyncState(VSyncState);
    if (error != kErrorNone) {
        DLOGE("VSync state setting failed. Error = %d", error);
        return error;
    }

    return kErrorNone;
}

DisplayError SdmDisplay::GetDisplayConfiguration(struct DisplayConfigInfo *display_config) {
    DisplayError error = kErrorNone;
    DisplayConfigVariableInfo disp_config;
    uint32_t active_index = 0;

    error = display_intf_->GetActiveConfig(&active_index);

    if (error != kErrorNone) {
        DLOGE("Active Index not found. Error = %d", error);
        return error;
    }

    error = display_intf_->GetConfig(active_index, &disp_config);

    if (error != kErrorNone) {
        DLOGE("Display Configuration failed. Error = %d", error);
        return error;
    }

    display_config->x_pixels     = disp_config.x_pixels;
    display_config->y_pixels     = disp_config.y_pixels;
    display_config->x_dpi        = disp_config.x_dpi;
    display_config->y_dpi        = disp_config.y_dpi;
    display_config->fps          = disp_config.fps;
    display_config->vsync_period_ns = disp_config.vsync_period_ns;
    display_config->is_yuv       = disp_config.is_yuv;
    fps_                         = disp_config.fps;

    return kErrorNone;
}

DisplayError SdmDisplay::RegisterCb(int display_id,       vblank_cb_t vbcb) {
    DisplayError error = kErrorNone;

    vblank_cb_   = vbcb;
    display_id_  = display_id;

    return error;
}

DisplayError SdmDisplay::FreeLayerStack() {
    for (Layer *layer : layer_stack_.layers) {

         layer->visible_regions.erase(layer->visible_regions.begin(),
                                       layer->visible_regions.end());
         layer->dirty_regions.erase(layer->dirty_regions.begin(),
                                       layer->dirty_regions.end());

         delete layer;
    }
    layer_stack_ = {};

    return kErrorNone;
}

DisplayError SdmDisplay::FreeLayerGeometry(struct LayerGeometry *glayer) {
    if (glayer->dirty_regions.count)
        free(glayer->dirty_regions.rects);
    if (glayer->visible_regions.count)
        free(glayer->visible_regions.rects);

    free(glayer);

    return kErrorNone;
}

DisplayError SdmDisplay::AllocLayerStackMemory(struct drm_output *output) {
    uint32_t num_layers = output->view_count;

    for (size_t i = 0; i < output->view_count; i++) {
         Layer *layer = new Layer();
         layer_stack_.layers.push_back(layer);
    }

    return kErrorNone;
}

static void SetRect(sdm::LayerRect *dst, struct Rect *src)
{
    dst->left = src->left;
    dst->top = src->top;
    dst->right = src->right;
    dst->bottom = src->bottom;
}

static void SetRectArray(sdm::LayerRectArray *dst, struct RectArray *src)
{
    for (uint32_t i = 0; i < src->count; i++)
         SetRect(&dst->rect[i], &src->rects[i]);
}

static uint32_t GetComposition(sdm::LayerComposition composition)
{
    uint32_t ret;

    switch (composition) {
     case sdm::kCompositionGPUTarget:
          ret = SDM_COMPOSITION_FB_TARGET;
          break;
     case sdm::kCompositionGPU:
          ret = SDM_COMPOSITION_GPU;
          break;
     case sdm::kCompositionHWCursor:
          ret = SDM_COMPOSITION_HW_CURSOR;
          break;
     default:
          ret = SDM_COMPOSITION_OVERLAY;
          break;
    }

    return ret;
}

DisplayError SdmDisplay::PopulateLayerGeometryOnToLayerStack(struct drm_output *output,
                                                             uint32_t index,
                                                             struct LayerGeometry *glayer,
                                                             bool is_skip) {
    DisplayError error = kErrorNone;
    sdm::Layer *layer = layer_stack_.layers.at(index);
    struct LayerGeometry *layer_geometry = glayer;
    LayerBuffer *layer_buffer, &layer_buffer_ = layer->input_buffer;
    layer_buffer = &layer_buffer_;

    /* 1. Fill buffer information */
    *layer_buffer = sdm::LayerBuffer();
    layer_buffer->format = GetSDMFormat(layer_geometry->format, layer_geometry->flags);
    layer_buffer->width = layer_geometry->width;
    layer_buffer->height = layer_geometry->height;
    layer_buffer->unaligned_width = layer_geometry->unaligned_width;
    layer_buffer->unaligned_height = layer_geometry->unaligned_height;
    /* TODO (user): Obtain metadata and then */
    // TODO: (user)  if (SetMetaData(metadatar layer) != kErrorNone) {
    // TODO: (user)      return kErrorUndefined;
    // TODO: (user)  }

    if (index != layer_stack_.layers.size()-1)
        layer_buffer->planes[0].fd = layer_geometry->ion_fd;

    /* TODO: Below information should be set according to the real user scenario */
    layer_buffer->flags.secure = layer_geometry->flags.secure_present;
    layer_buffer->flags.video = layer_geometry->flags.video_present;
    layer_buffer->flags.hdr = layer_geometry->flags.hdr_present;

    if (layer_buffer->flags.hdr) {
      layer_buffer->color_metadata = layer_geometry->color_metadata;

       DLOGI("color_metadata: ColorPrimaries: %d", layer_buffer->color_metadata.colorPrimaries);
       DLOGI("color_metadata: Transfer: %d", layer_buffer->color_metadata.transfer);
    }

    layer_buffer->flags.macro_tile = false;
    layer_buffer->flags.interlace = false;
    layer_buffer->flags.secure_display = false;
    layer_buffer->flags.secure_camera = false;
    /* 2. Fill layer information */
    if (layer_geometry->composition == SDM_COMPOSITION_FB_TARGET)
        layer->composition = sdm::kCompositionGPUTarget;
    else
        layer->composition = sdm::kCompositionGPU;

    SetRect(&layer->src_rect, &layer_geometry->src_rect);
    SetRect(&layer->dst_rect, &layer_geometry->dst_rect);

    for (uint32_t i = 0; i < layer_geometry->visible_regions.count; i++) {
         SetRect(&layer->visible_regions.at(i), &layer_geometry->visible_regions.rects[i]);
    }

    for (uint32_t i = 0; i < layer_geometry->dirty_regions.count; i++) {
         SetRect(&layer->dirty_regions.at(i), &layer_geometry->dirty_regions.rects[i]);
    }

    layer->blending = GetSDMBlending(layer_geometry->blending);
    layer->plane_alpha = layer_geometry->plane_alpha;

    layer->transform.flip_horizontal =
              ((layer_geometry->transform & SDM_TRANSFORM_FLIP_H) > 0);
    layer->transform.flip_vertical =
              ((layer_geometry->transform & SDM_TRANSFORM_FLIP_V) > 0);
    layer->transform.rotation =
              ((layer_geometry->transform & SDM_TRANSFORM_90) ? 90.0f : 0.0f);

    layer->frame_rate = fps_;
    layer->solid_fill_color = 0;

    layer->flags.skip = is_skip;
    if (is_skip)
       layer_stack_.flags.skip_present = is_skip;
    if (layer_buffer->flags.video)
       layer_stack_.flags.video_present = true;
    if (layer_buffer->flags.hdr)
       layer_stack_.flags.hdr_present = 1;
    if (layer_buffer->flags.secure)
        layer_stack_.flags.secure_present = true;
    layer->flags.updating = true;
    layer->flags.solid_fill = false;
    layer->flags.cursor = false;
    layer->flags.single_buffer = false;

    return error;
}

DisplayError SdmDisplay::AllocateMemoryForLayerGeometry(struct \
                                drm_output *output,
                                uint32_t index,
                                struct LayerGeometry \
                                *glayer) {
    /* Configure each layer */
    uint32_t num_visible_rects = 0;
    uint32_t num_dirty_rects = 0;

    if (index < output->view_count) {
     if (glayer == NULL) {
          DLOGE("no layer geometry for the display!\n");
          return kErrorParameters;
     }
     /* It's permissive the visible/dirty region can be NULL */
     num_visible_rects = glayer->visible_regions.count;
     for (uint32_t j = 0; j < num_visible_rects; j++) {
          LayerRect visible_rect {};
          layer_stack_.layers.at(index)->visible_regions.push_back(visible_rect);
     }

     num_dirty_rects = glayer->dirty_regions.count;
     for (uint32_t j = 0; j < num_dirty_rects; j++) {
          LayerRect dirty_rect {};
          layer_stack_.layers.at(index)->dirty_regions.push_back(dirty_rect);
     }
    }

    return kErrorNone;
}

DisplayError SdmDisplay::AddGeometryLayerToLayerStack(struct drm_output *output, uint32_t index,
                                                      struct LayerGeometry *glayer, bool is_skip)
{
    DisplayError error = kErrorNone;

    AllocateMemoryForLayerGeometry(output, index, glayer);
    error = PopulateLayerGeometryOnToLayerStack(output, index, glayer, is_skip);

    return error;
}

int SdmDisplay::PrepareFbLayerGeometry(struct drm_output *output,
                        struct LayerGeometry **fb_glayer) {
    struct LayerGeometry *fb_layer;
    *fb_glayer = fb_layer = reinterpret_cast<struct LayerGeometry *> \
                              (zalloc(sizeof *fb_layer));

    fb_layer->width = output->base.width;
    fb_layer->height = output->base.height;
    fb_layer->unaligned_width = output->base.width;
    fb_layer->unaligned_height = output->base.height;

    fb_layer->format = GetMappedFormatFromGbm(output->format);
    fb_layer->composition = SDM_COMPOSITION_FB_TARGET;

    fb_layer->src_rect.left = (float)0.0;
    fb_layer->src_rect.top = (float)0.0;
    fb_layer->src_rect.right = (float)output->base.width;
    fb_layer->src_rect.bottom = (float)output->base.height;
    fb_layer->dst_rect.left = (float)0.0;
    fb_layer->dst_rect.top = (float)0.0;
    fb_layer->dst_rect.right = (float)output->base.width;
    fb_layer->dst_rect.bottom = (float)output->base.height;

    fb_layer->visible_regions.rects = reinterpret_cast<struct Rect *> \
                              (zalloc(sizeof(struct Rect)));
    if (fb_layer->visible_regions.rects == NULL) {
        DLOGE("out of memory for allocating fb visible region\n");
        return -1;
    }
    fb_layer->visible_regions.rects[0] = fb_layer->dst_rect;
    fb_layer->visible_regions.count = 1;

    fb_layer->dirty_regions.rects = reinterpret_cast<struct Rect *> \
                              (zalloc(sizeof(struct Rect)));
    if (fb_layer->dirty_regions.rects == NULL) {
        DLOGE("out of memory for allocating fb dirty region\n");
        return -1;
    }
    fb_layer->dirty_regions.rects[0] = fb_layer->dst_rect;
    fb_layer->dirty_regions.count = 1;

    fb_layer->blending = SDM_BLENDING_PREMULTIPLIED;
    /*
     * output->base.width/height should be already transformed, so just
     * keep no transform for output.
     */
    fb_layer->transform = SDM_TRANSFORM_NORMAL;
    fb_layer->plane_alpha = 0xFF;

    fb_layer->flags.skip = 0;
    fb_layer->flags.is_cursor = 0;
    fb_layer->flags.has_ubwc_buf = output->framebuffer_ubwc;

    return 0;
}

int SdmDisplayInterface::GetDrmMasterFd() {
    DRMMaster *master = nullptr;
    int ret = DRMMaster::GetInstance(&master);
    int fd;

    if (ret < 0) {
        DLOGE("Failed to acquire DRMMaster instance");
        return kErrorNotSupported;
        }
    master->GetHandle(&fd);
    return fd;
}

int SdmDisplay::PrepareNormalLayerGeometry(struct drm_output *output,
                         struct LayerGeometry **glayer,
                         struct sdm_layer *sdm_layer) {
    struct drm_backend *b = (struct drm_backend *)output->base.compositor->backend;
    struct LayerGeometry *layer;
    struct weston_view *ev = sdm_layer->view;
    struct weston_surface *es = ev->surface;
    bool is_cursor = sdm_layer->is_cursor;
    uint32_t format = GBM_FORMAT_XBGR8888;
    struct linux_dmabuf_buffer *dmabuf;
    struct gbm_buffer *gbm_buf;
    pixman_region32_t r;

    *glayer = layer = reinterpret_cast<struct LayerGeometry *> \
                           (zalloc(sizeof *layer));
    /* Prepare layer buffer information */
    layer->width = es->width;
    layer->height = es->height;
    layer->fb_id = -1;
    layer->format = SDM_BUFFER_FORMAT_RGBX_8888;

    if (!sdm_layer->is_skip) {
        struct gbm_bo *bo;
        //check whether the buffer resource is created by linux dma buf
        if ((dmabuf = linux_dmabuf_buffer_get(es->buffer_ref.buffer->resource))) {
            struct gbm_import_fd_data gbm_dmabuf = {
                .fd     = dmabuf->dmabuf_fd[0],
                .width  = dmabuf->width,
                .height = dmabuf->height,
                .stride = dmabuf->stride[0],
                .format = dmabuf->format
            };
            bo = gbm_bo_import(b->gbm, GBM_BO_IMPORT_FD, &gbm_dmabuf, GBM_BO_USE_SCANOUT);
        } else if ((gbm_buf = gbm_buffer_get(es->buffer_ref.buffer->resource))) {
          struct gbm_buf_info gbm_bufinfo = {
              .fd           = gbm_buf->fd,
              .metadata_fd  = gbm_buf->metadata_fd,
              .width        = gbm_buf->width,
              .height       = gbm_buf->height,
              .format       = gbm_buf->format
          };
          bo = gbm_bo_import(b->gbm, GBM_BO_IMPORT_GBM_BUF_TYPE,
                             &gbm_bufinfo,
                             GBM_BO_USE_SCANOUT);
        } else {
            bo = gbm_bo_import(b->gbm, GBM_BO_IMPORT_GBM_BUF_TYPE,
                               wl_resource_get_user_data(es->buffer_ref.buffer->resource),
                               GBM_BO_USE_SCANOUT);
        }

         if (bo == NULL)
            DLOGE("fail to import gbm bo!\n");
         else {
            uint32_t width, height;
            uint32_t *fbid;
            uint32_t fb_id, stride, handle, size;
            uint32_t fb_id1;

            //save gbm bo in sdm layer for future reference.
            sdm_layer->bo = bo;

            width = gbm_bo_get_width(bo);
            height = gbm_bo_get_height(bo);
            stride = gbm_bo_get_stride(bo);
            handle = gbm_bo_get_handle(bo).u32;
            format = gbm_bo_get_format(bo);
            int drm_fd = SdmDisplayInterface::GetDrmMasterFd();

            uint32_t handles[4], pitches[4], offsets[4];
            handles[0] = handle;
            pitches[0] = stride;
            offsets[0] = 0;

            uint32_t alignedWidth = 0;
            uint32_t alignedHeight = 0;
            uint32_t secure_status = 0;
            uint32_t ubwc_status = 0;
            void *prm = reinterpret_cast<void *> (&layer->color_metadata);

            gbm_perform(GBM_PERFORM_GET_BO_ALIGNED_WIDTH, bo, &alignedWidth);
            gbm_perform(GBM_PERFORM_GET_BO_ALIGNED_HEIGHT, bo, &alignedHeight);
            gbm_perform(GBM_PERFORM_GET_SECURE_BUFFER_STATUS, bo, &secure_status);
            gbm_perform(GBM_PERFORM_GET_METADATA, bo, GBM_METADATA_GET_COLOR_METADATA, prm);
            gbm_perform(GBM_PERFORM_GET_UBWC_STATUS, bo, &ubwc_status);

            // Override buffer width/height to reflect aligned width and aligned height.
            layer->width = alignedWidth;
            layer->height = alignedHeight;
            layer->unaligned_width = width;
            layer->unaligned_height = height;
            layer->ion_fd = gbm_bo_get_fd(bo);
            layer->flags.secure_present = secure_status;
            layer->flags.has_ubwc_buf = ubwc_status;

            bool hdr_layer = layer->color_metadata.colorPrimaries == ColorPrimaries_BT2020 &&
                             (layer->color_metadata.transfer == Transfer_SMPTE_ST2084 ||
                             layer->color_metadata.transfer == Transfer_HLG);

            // Set to true if incoming layer has HDR support and Display supports HDR functionality
            if (!disable_hdr_handling_)
                layer->flags.hdr_present = hdr_layer;
        }
    }

    if (NeedConvertGbmFormat(ev, format))
        format = ConvertToOpaqueGbmFormat(format);
    layer->format = GetMappedFormatFromGbm(format);

    /* Get src/dst rect */
    ComputeSrcDstRect(output, ev, &layer->src_rect, &layer->dst_rect);
    /* Get visible rects */
    if (GetVisibleRegion(output, ev, &sdm_layer->overlap, &layer->visible_regions))
        return -1;
    /*
     * Get dirty rects. It's not surprised a view has null damage region, that means
     * the buffer content of view is not changed since last frame, eg. cursor.
     */
    if (ComputeDirtyRegion(ev, &layer->dirty_regions))
        return -1;

    /* Set no transform for view since bounding box already been applied by transform */
    layer->transform = SDM_TRANSFORM_NORMAL;
    /* Get global alpha */
    layer->plane_alpha = GetGlobalAlpha(ev);

    /* TODO: update property src_config, csc, color_fill, scaler ... */
    layer->flags.is_cursor = is_cursor;
    layer->flags.video_present = GetVideoPresenceByFormatFromGbm(format);

    /* compute whether this view has no blending */
    pixman_region32_init_rect(&r, 0, 0, ev->surface->width, ev->surface->height);
    pixman_region32_subtract(&r, &r, &ev->surface->opaque);

    if (!pixman_region32_not_empty(&r) && (layer->plane_alpha == 0xFF))
        layer->blending = SDM_BLENDING_NONE;
    else
        layer->blending = SDM_BLENDING_COVERAGE;

    // Video layers are always opaque
    if (layer->flags.video_present) {
        layer->blending = SDM_BLENDING_NONE;
    }

    /* Initialize all views with GPU composition first, SDM will update them after prepare */
    layer->composition = SDM_COMPOSITION_GPU;

    return 0;
}

DisplayError SdmDisplay::PrePrepareLayerStack(struct drm_output *output) {
    DisplayError error = kErrorNone;
    struct sdm_layer *sdm_layer = NULL, *next_sdm_layer = NULL;
    struct LayerGeometry *glayer = NULL;
    uint32_t gpu_target_index = GET_GPU_TARGET_SLOT(output->view_count);
    uint32_t index = 0;
    LayerBufferFlags layerBufferFlags;

    if (shutdown_pending_) {
    return kErrorShutDown;
    }

    FreeLayerStack();
    AllocLayerStackMemory(output);

#if SDM_DISPLAY_DEBUG
    DLOGW("gpu_target_index = %d\n", gpu_target_index);
#endif

    /* If no view can be handled by SDM, just skip below and prepare fb target directly. */
    if (gpu_target_index > 0) {
        wl_list_for_each_reverse(sdm_layer, &output->sdm_layer_list, link) {
            glayer = NULL;
            if(PrepareNormalLayerGeometry(output, &glayer, sdm_layer)) {
                DLOGE("fail to prepare normal layer geometry.");
                return kErrorUndefined;
            }

            error = AddGeometryLayerToLayerStack(output, index, glayer, sdm_layer->is_skip);
            if (error) {
                DLOGE("failed add Geometry Layer to LayerStack.");
                FreeLayerGeometry(glayer);
                return kErrorUndefined;
            }
            FreeLayerGeometry(glayer);

            // Pass the wl_resource handle from sdm layer to layer stack
            // to use it for egl image creation in tone mapping
            layerBufferFlags = layer_stack_.layers.at(index)->input_buffer.flags;
            if (layerBufferFlags.video && layerBufferFlags.hdr)
                layer_stack_.layers.at(index)->userdata =
                           sdm_layer->view->surface->buffer_ref.buffer->resource;

            index++;
            if (sdm_layer->is_skip)
                layer_stack_.flags.skip_present = true;
        }
    }

    int err = PrepareFbLayerGeometry(output, &glayer);
    if (err) {
        DLOGE("failed to prepare Layer Geometry Fb target\n");
        return kErrorUndefined;
    } else {
        error = AddGeometryLayerToLayerStack(output, index, glayer, false);
        if (error) {
            DLOGE("fail to prepare Fb target: Add Geometry failure fb\n");
            FreeLayerGeometry(glayer);
            return kErrorUndefined;
        }
        FreeLayerGeometry(glayer);
    }

    return error;
}

DisplayError SdmDisplay::PrePrepare(struct drm_output *output)
{
    DisplayError error = kErrorNone;

    error = PrePrepareLayerStack(output);
    if (error ) {
        DLOGE("function failed!\n", error);
    }

    return error;
}

DisplayError SdmDisplay::PostPrepare(struct drm_output *output)
{
    DisplayError error = kErrorNone;
    int index = 0;
    struct sdm_layer *sdm_layer = NULL;

    wl_list_for_each_reverse(sdm_layer, &output->sdm_layer_list, link) {
      Layer *layer = layer_stack_.layers.at(index);

      if (layer->composition == kCompositionGPU) {
        sdm_layer->composition_type = SDM_COMPOSITION_GPU;
      } else {
        sdm_layer->composition_type = SDM_COMPOSITION_OVERLAY;
      }
      index++;
    }

    return error;
}

static void GetLayerStackDump(void *layerStack, char *buffer, uint32_t length) {
  if (!buffer || !length) {
    return;
  }
  static int frame_count=0;
  buffer[0] = '\0';
  struct LayerStack *layer_stack;
  layer_stack = reinterpret_cast<struct LayerStack *>(layerStack);
  DLOGI("\n-------- Display Manager: Layer Stack Dump --------");
  fprintf(stderr,"Frame:%d LayerStack: NumLayers:%d flags:0x%x\n",
                frame_count, layer_stack->layers.size(), layer_stack->flags);

  for (uint32_t i = 0; i < layer_stack->layers.size(); i++) {
      char buf[LEN_LOCAL] = {0};

      struct Layer *layer;
      struct LayerBuffer buffer;
      layer = layer_stack->layers.at(i);
      buffer = layer->input_buffer;

      memset(buf, '\0', LEN_LOCAL);
      sprintf(buf, "Layer: %d\n    width  = %d,     height = %d", i,
        layer->input_buffer.width, layer->input_buffer.height);
      sprintf(buf, "%s\n LayerComposition = %#x", buf, layer->composition);
      sprintf(buf, "%s\n src_rect (LTRB) = %4.2f, %4.2f, %4.2f, %4.2f",
        buf, layer->src_rect.left, layer->src_rect.top,
        layer->src_rect.right, layer->src_rect.bottom);
      sprintf(buf, "%s\n dst_rect (LTRB) = %4.2f, %4.2f, %4.2f, %4.2f", buf,
        layer->dst_rect.left, layer->dst_rect.top, layer->dst_rect.right,
        layer->dst_rect.bottom);
      sprintf(buf, "%s\n LayerBlending = %#x", buf, layer->blending);
      sprintf(buf,"%s\n LayerTransform:rotation= %f,flip_horizontal=%s,flip_vertical=%s",
        buf, layer->transform.rotation, (layer->transform.flip_horizontal? \
        "true":"false"), (layer->transform.flip_vertical? "true":"false"));
      sprintf(buf, "%s\n Plane Alpha = %#x, frame_rate = %d,  solid_fill_color = %d",
        buf, layer->plane_alpha, layer->frame_rate, layer->solid_fill_color);
      sprintf(buf, "%s\n LayerFlags = %#x", buf, layer->flags);
      sprintf(buf, "%s\t LayerFlags.skip = %d", buf, layer->flags.skip);
      sprintf(buf, "%s\n LayerBuffer Flags: hdr:%d secure:%d video:%d", buf,
                    buffer.flags.hdr, buffer.flags.secure, buffer.flags.video);

      fprintf(stderr,"\n%s\n", buf);
  }
  frame_count++;

  return;
}

DisplayError SdmDisplay::Prepare(struct drm_output *output)
{
    DisplayError error = kErrorNone;
    char dump_buffer[8192] = {0};

    error = PrePrepare(output);

#if SDM_DISPLAY_DUMP_LAYER_STACK
    // Dump all input layers of the layer stack:
    GetLayerStackDump(&layer_stack_, dump_buffer, sizeof(dump_buffer));
#endif

    error = display_intf_->Prepare(&layer_stack_);
    DumpInterface::GetDump(dump_buffer, sizeof(dump_buffer));
    error = PostPrepare(output);
    if (error != kErrorNone)
     DLOGE("function failed Error= %d\n", error);

    return error;
}

DisplayError SdmDisplay::PreCommit()
{
    DisplayError error = kErrorNone;

    if (layer_stack_.flags.hdr_present) {
        int status = -1;
        if (tone_mapper_) {
            status = tone_mapper_->HandleToneMap(&layer_stack_);
            if (status != 0) {
                DLOGE("Error handling HDR in ToneMapper, status code = %d", status);
            }
        } else {
            DLOGD("HandleToneMap failed due to invalid tone_mapper_ instance");
        }
    } else {
        if (tone_mapper_)
            tone_mapper_->Terminate();
        else
            DLOGD("ToneMap Terminate failed due to invalid tone_mapper_ instance");
    }

    return error;
}

DisplayError SdmDisplay::PostCommit()
{
    DisplayError error = kErrorNone;

    if (tone_mapper_ && tone_mapper_->IsActive()) {
        tone_mapper_->PostCommit(&layer_stack_);
     }

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

    return error;
}

DisplayError SdmDisplay::Commit(struct drm_output *output)
{
    DisplayError ret = kErrorNone;

    uint32_t layer_count = layer_stack_.layers.size();

    uint32_t GPUTarget_index = layer_count-1;
    Layer *GpuTargetlayer;
    uint32_t fb_id = output->next->fb_id;

    GpuTargetlayer = layer_stack_.layers.at(GPUTarget_index);

    GpuTargetlayer->input_buffer.planes[0].fd = output->next->ion_fd;

    PreCommit();

    ret = display_intf_->Commit(&layer_stack_);
    PostCommit();

    return ret;
}

/* Adding following  support functions */
LayerBufferFormat SdmDisplay::GetSDMFormat(uint32_t src_fmt, struct LayerGeometryFlags flags)
{
    LayerBufferFormat format = kFormatInvalid;

    if (flags.has_ubwc_buf) {
        switch (src_fmt) {
            case SDM_BUFFER_FORMAT_RGBA_8888:
                format = sdm::kFormatRGBA8888Ubwc;
                break;
            case SDM_BUFFER_FORMAT_RGBX_8888:
                format = sdm::kFormatRGBX8888Ubwc;
                break;
            case SDM_BUFFER_FORMAT_BGR_565:
                format = sdm::kFormatBGR565Ubwc;
                break;
            case SDM_BUFFER_FORMAT_RGBA_2101010:
                format = sdm::kFormatRGBA1010102Ubwc;
                break;
            case SDM_BUFFER_FORMAT_YCbCr_420_SP_VENUS:
            case SDM_BUFFER_FORMAT_NV12_ENCODEABLE:
                format = sdm::kFormatYCbCr420SPVenusUbwc;
                break;
            case SDM_BUFFER_FORMAT_YCbCr_420_TP10_UBWC:
                format = sdm::kFormatYCbCr420TP10Ubwc;
                break;
            case SDM_BUFFER_FORMAT_YCbCr_420_P010_UBWC:
                format = sdm::kFormatYCbCr420P010Ubwc;
                break;
            default:
                DLOGE("Unsupported UBWC format %d\n", src_fmt);
                return sdm::kFormatInvalid;
        }

        return format;
    }

    switch (src_fmt) {
        case SDM_BUFFER_FORMAT_RGBA_8888:
            format = sdm::kFormatRGBA8888;
            break;
        case SDM_BUFFER_FORMAT_RGBA_5551:
            format = sdm::kFormatRGBA5551;
            break;
        case SDM_BUFFER_FORMAT_RGBA_4444:
            format = sdm::kFormatRGBA4444;
            break;
        case SDM_BUFFER_FORMAT_BGRA_8888:
            format = sdm::kFormatBGRA8888;
            break;
        case SDM_BUFFER_FORMAT_RGBX_8888:
            format = sdm::kFormatRGBX8888;
            break;
        case SDM_BUFFER_FORMAT_BGRX_8888:
            format = sdm::kFormatBGRX8888;
            break;
        case SDM_BUFFER_FORMAT_RGB_888:
            format = sdm::kFormatRGB888;
            break;
        case SDM_BUFFER_FORMAT_RGB_565:
            format = sdm::kFormatRGB565;
            break;
        case SDM_BUFFER_FORMAT_BGR_565:
            format = sdm::kFormatBGR565;
            break;
        case SDM_BUFFER_FORMAT_NV12_ENCODEABLE:
        case SDM_BUFFER_FORMAT_YCbCr_420_SP_VENUS:
            format = sdm::kFormatYCbCr420SemiPlanarVenus;
            break;
        case SDM_BUFFER_FORMAT_ABGR_2101010:
            format = sdm::kFormatABGR2101010;
            break;
        case SDM_BUFFER_FORMAT_RGBA_2101010:
            format = sdm::kFormatRGBA1010102;
            break;
        case SDM_BUFFER_FORMAT_YCbCr_420_TP10_UBWC:
            format = sdm::kFormatYCbCr420TP10Ubwc;
            break;
        case SDM_BUFFER_FORMAT_YCbCr_420_P010_UBWC:
            format = sdm::kFormatYCbCr420P010Ubwc;
            break;
        case SDM_BUFFER_FORMAT_YV12:
            format = sdm::kFormatYCrCb420PlanarStride16;
            break;
        case SDM_BUFFER_FORMAT_YCrCb_420_SP:
            format = sdm::kFormatYCrCb420SemiPlanar;
            break;
        case SDM_BUFFER_FORMAT_YCbCr_420_SP:
            format = sdm::kFormatYCbCr420SemiPlanar;
            break;
        case SDM_BUFFER_FORMAT_YCbCr_422_SP:
            format = sdm::kFormatYCbCr422H2V1SemiPlanar;
            break;
        case SDM_BUFFER_FORMAT_YCbCr_422_I:
            format = sdm::kFormatYCbCr422H2V1Packed;
            break;
        case SDM_BUFFER_FORMAT_P010:
            format = sdm::kFormatYCbCr420P010;
            break;
        default:
            DLOGE("Unsupported format %d\n", src_fmt);
            return sdm::kFormatInvalid;
    }

    return format;
}

LayerBlending SdmDisplay::GetSDMBlending(uint32_t source)
{
    sdm::LayerBlending blending = sdm::kBlendingPremultiplied;

    switch (source) {
    case SDM_BLENDING_PREMULTIPLIED:
         blending = sdm::kBlendingPremultiplied;
         break;
    case SDM_BLENDING_COVERAGE:
         blending = sdm::kBlendingCoverage;
         break;
    case SDM_BLENDING_NONE:
    default:
         blending = sdm::kBlendingOpaque;
         break;
    }

    return blending;
}

bool SdmDisplay::GetVideoPresenceByFormatFromGbm(uint32_t fmt)
{
    bool is_video_present = false;

    switch (fmt) {
       case GBM_FORMAT_NV12:
       case GBM_FORMAT_YCbCr_420_TP10_UBWC:
       case GBM_FORMAT_YCbCr_420_P010_UBWC:
       case GBM_FORMAT_P010:
            is_video_present = true;
            break;
       default:
            is_video_present = false;
            break;
    }

    return is_video_present;
}

uint32_t SdmDisplay::GetMappedFormatFromGbm(uint32_t fmt)
{
    uint32_t ret = SDM_BUFFER_FORMAT_INVALID;

    switch (fmt) {
    case GBM_FORMAT_ARGB8888:
         ret = SDM_BUFFER_FORMAT_BGRA_8888;
         break;
    case GBM_FORMAT_XRGB8888:
         ret = SDM_BUFFER_FORMAT_BGRX_8888;
         break;
    case GBM_FORMAT_ABGR8888:
         ret = SDM_BUFFER_FORMAT_RGBA_8888;
         break;
    case GBM_FORMAT_XBGR8888:
         ret = SDM_BUFFER_FORMAT_RGBX_8888;
         break;
    case GBM_FORMAT_BGRA8888:
    case GBM_FORMAT_RGBA8888:
         ret = SDM_BUFFER_FORMAT_ARGB_8888;
         break;
    case GBM_FORMAT_ARGB4444:
    case GBM_FORMAT_ABGR4444:
         ret = SDM_BUFFER_FORMAT_RGBA_4444;
         break;
    case GBM_FORMAT_ARGB1555:
    case GBM_FORMAT_ABGR1555:
         ret = SDM_BUFFER_FORMAT_RGBA_5551;
         break;
    case GBM_FORMAT_RGB565:
         ret = SDM_BUFFER_FORMAT_BGR_565;
         break;
    case GBM_FORMAT_BGR565:
         ret = SDM_BUFFER_FORMAT_RGB_565;
         break;
    case GBM_FORMAT_RGB888:
         ret = SDM_BUFFER_FORMAT_BGR_888;
         break;
    case GBM_FORMAT_BGR888:
         ret = SDM_BUFFER_FORMAT_RGB_888;
         break;
    case GBM_FORMAT_NV12:
         ret = SDM_BUFFER_FORMAT_YCbCr_420_SP_VENUS;
         break;
    case GBM_FORMAT_ABGR2101010:
         ret = SDM_BUFFER_FORMAT_RGBA_2101010;
         break;
    case GBM_FORMAT_YCbCr_420_TP10_UBWC:
         ret = SDM_BUFFER_FORMAT_YCbCr_420_TP10_UBWC;
         break;
    case GBM_FORMAT_YCbCr_420_P010_UBWC:
        ret = SDM_BUFFER_FORMAT_YCbCr_420_P010_UBWC;
        break;
    case GBM_FORMAT_P010:
        ret = SDM_BUFFER_FORMAT_P010;
        break;
    default:
         DLOGE("Unsupported GBM format %s\n", FourccToString(fmt));
         break;
    }

    return ret;
}

bool SdmDisplay::NeedConvertGbmFormat(struct weston_view *ev, uint32_t format)
{
    pixman_region32_t r;
    bool need_convert = false;

    if (IsTransparentGbmFormat(format)) {
     pixman_region32_init_rect(&r, 0, 0,
             ev->surface->width,
             ev->surface->height);
     pixman_region32_subtract(&r, &r, &ev->surface->opaque);

     if (!pixman_region32_not_empty(&r))
         need_convert = true;

     pixman_region32_fini(&r);
    }

    return need_convert;
}

uint32_t SdmDisplay::ConvertToOpaqueGbmFormat(uint32_t format)
{
    uint32_t ret = GBM_FORMAT_XRGB8888;

    switch (format) {
    case GBM_FORMAT_ARGB8888:
     ret = GBM_FORMAT_XRGB8888;
     break;
    case GBM_FORMAT_BGRA8888:
     ret = GBM_FORMAT_BGRX8888;
     break;
    case GBM_FORMAT_RGBA8888:
     ret = GBM_FORMAT_RGBX8888;
     break;
    case GBM_FORMAT_ABGR8888:
     ret = GBM_FORMAT_XBGR8888;
     break;
    default:
     break;
    }

    return ret;
}

void SdmDisplay::ComputeSrcDstRect(struct drm_output *output, struct weston_view *ev,
                    struct Rect *src_ret, struct Rect *dst_ret)
{
    struct weston_buffer_viewport *viewport = &ev->surface->buffer_viewport;
    pixman_region32_t src_rect, dest_rect;
    pixman_box32_t *box, tbox;
    float sx1, sy1, sx2, sy2;

    /* dst rect */
    pixman_region32_init(&dest_rect);
    pixman_region32_intersect(&dest_rect, &ev->transform.boundingbox, &output->base.region);

    pixman_region32_translate(&dest_rect, -output->base.x, -output->base.y);
    box = pixman_region32_extents(&dest_rect);

    tbox = weston_transformed_rect(output->base.width,
                                   output->base.height,
                                   (wl_output_transform)output->base.transform,
                                   output->base.current_scale,
                                   *box);
    dst_ret->left = tbox.x1;
    dst_ret->right = tbox.x2;
    dst_ret->top = tbox.y1;
    dst_ret->bottom = tbox.y2;
    pixman_region32_fini(&dest_rect);

    /* src rect */
    pixman_region32_init(&src_rect);
    pixman_region32_intersect(&src_rect, &ev->transform.boundingbox,
                              &output->base.region);
    box = pixman_region32_extents(&src_rect);

    weston_view_from_global_float(ev, box->x1, box->y1, &sx1, &sy1);
    weston_surface_to_buffer_float(ev->surface, sx1, sy1, &sx1, &sy1);
    weston_view_from_global_float(ev, box->x2, box->y2, &sx2, &sy2);
    weston_surface_to_buffer_float(ev->surface, sx2, sy2, &sx2, &sy2);
    pixman_region32_fini(&src_rect);
    src_ret->left = sx1;
    src_ret->top = sy1;
    src_ret->right = sx2;
    src_ret->bottom = sy2;
}

int SdmDisplay::ComputeDirtyRegion(struct weston_view *ev,
                    struct RectArray *dirty) {
    pixman_region32_t temp;
    struct weston_surface *es = ev->surface;
    pixman_box32_t *rectangles = NULL;
    int n, i;

    pixman_region32_init(&temp);
    pixman_region32_intersect_rect(&temp, &es->damage, 0,
                    0, es->width, es->height);
    rectangles = pixman_region32_rectangles(&temp, &n);
    if (!n) {
     pixman_region32_fini(&temp);
     return 0;
    }

    dirty->rects = reinterpret_cast<struct Rect *> \
                    (zalloc(n * sizeof(struct Rect)));
    if (dirty->rects == NULL) {
     DLOGE("out of memory for allocating dirty region\n");
     return -1;
    }
    for (i = 0; i < n; i++) {
     dirty->rects[i].left = (float)rectangles[i].x1;
     dirty->rects[i].top = (float)rectangles[i].y1;
     dirty->rects[i].right = (float)rectangles[i].x2;
     dirty->rects[i].bottom = (float)rectangles[i].y2;
    }
    dirty->count = n;
    pixman_region32_fini(&temp);

    return 0;
}

uint8_t SdmDisplay::GetGlobalAlpha(struct weston_view *ev)
{
    if (ev->alpha > 1.0f)
     return 0xFF;
    else if (ev->alpha < 0.0f)
     return 0;
    else
     return (uint8_t)(0xFF * ev->alpha);
}

int SdmDisplay::GetVisibleRegion(struct drm_output *output, struct weston_view *ev,
                                         pixman_region32_t *aboved_opaque,
                                         struct RectArray *visible)
{
    pixman_region32_t temp;
    pixman_box32_t *rectangles = NULL;
    int n, i;

    pixman_region32_init(&temp);
    pixman_region32_copy(&temp, &ev->transform.boundingbox);
    pixman_region32_subtract(&temp, &temp, aboved_opaque);
    pixman_region32_intersect(&temp, &output->base.region, &temp);
    pixman_region32_translate(&temp, -output->base.x, -output->base.y);

    rectangles = pixman_region32_rectangles(&temp, &n);
    if (!n) {
     pixman_region32_fini(&temp);
     return 0;
    }

    visible->rects = reinterpret_cast<struct Rect *> (zalloc(n * sizeof(struct Rect)));
    if (visible->rects == NULL) {
     DLOGE("out of memory for allocating visible region\n");
     return -1;
    }
    for (i = 0; i < n; i++) {
     visible->rects[i].left = (float)rectangles[i].x1;
     visible->rects[i].top = (float)rectangles[i].y1;
     visible->rects[i].right = (float)rectangles[i].x2;
     visible->rects[i].bottom = (float)rectangles[i].y2;
    }
    visible->count = n;
    pixman_region32_fini(&temp);

    return 0;
}

bool SdmDisplay::IsTransparentGbmFormat(uint32_t format)
{
    return false;
}

const char *SdmDisplay::GetDisplayString() {
    switch (display_type_) {
    case kPrimary:
      return "primary";
    case kHDMI:
      return "hdmi";
    case kVirtual:
      return "virtual";
    default:
      return "invalid";
  }
}

DisplayError SdmDisplay::EnablePllUpdate(int32_t enable)
{
  DisplayError error;

  error = display_intf_->EnablePllUpdate(enable);
  if (error != kErrorNone) {
    DLOGE("%s pll update failed. Error = %d",
      enable ? "enable" : "disable", error);
    return error;
  }

  return kErrorNone;
}

DisplayError SdmDisplay::UpdateDisplayPll(int32_t ppm)
{
  DisplayError error;

  error = display_intf_->UpdateDisplayPll(ppm);
  if (error != kErrorNone) {
    DLOGE("Update display pll failed. Error = %d", error);
    return error;
  }

  return kErrorNone;
}

DisplayError SdmDisplay::GetHdrInfo(struct DisplayHdrInfo *display_hdr_info) {
    DisplayError error;

    DisplayConfigFixedInfo fixed_info = {};
    error = display_intf_->GetConfig(&fixed_info);

    if (error != kErrorNone) {
        DLOGE("Failed to get fixed info. Error = %d", error);
        return error;
    }

    hdr_supported_ = fixed_info.hdr_supported;

    if (!fixed_info.hdr_supported) {
        DLOGI("HDR is not supported");
        return error;
    }

    static const float kLuminanceFactor = 10000.0;
    // luminance is expressed in the unit of 0.0001 cd/m2, convert it to 1cd/m2.
    max_luminance_ = FLOAT(fixed_info.max_luminance)/kLuminanceFactor;
    max_average_luminance_ = FLOAT(fixed_info.average_luminance)/kLuminanceFactor;
    min_luminance_ = FLOAT(fixed_info.min_luminance)/kLuminanceFactor;

    display_hdr_info->hdr_supported = fixed_info.hdr_supported;
    display_hdr_info->hdr_eotf = fixed_info.hdr_eotf;
    display_hdr_info->hdr_metadata_type_one = fixed_info.hdr_metadata_type_one;
    display_hdr_info->max_luminance = fixed_info.max_luminance;
    display_hdr_info->average_luminance = fixed_info.average_luminance;
    display_hdr_info->min_luminance = fixed_info.min_luminance;

    return error;
}

DisplayError SdmDisplay::GetHdcpProtocol(struct DisplayHdcpProtocol *display_hdcp_protocol) {
    DisplayError error;

    DisplayConfigFixedInfo fixed_info = {};
    error = display_intf_->GetConfig(&fixed_info);

    if (error != kErrorNone) {
        DLOGE("Failed to get fixed info. Error = %d", error);
        return error;
    }

    hdcp_version_ = fixed_info.hdcp_version;

    display_hdcp_protocol->hdcp_version = fixed_info.hdcp_version;
    display_hdcp_protocol->hdcp_interface_type = fixed_info.hdcp_interface_type;

    return error;
}


SdmNullDisplay::SdmNullDisplay(DisplayType type, CoreInterface *core_intf) {
}

SdmNullDisplay::~SdmNullDisplay() {
}

DisplayError SdmNullDisplay::CreateDisplay() {
  return kErrorNone;
}
DisplayError SdmNullDisplay::DestroyDisplay() {
  return kErrorNone;
}
DisplayError SdmNullDisplay::Prepare(struct drm_output *output) {
  return kErrorNone;
}
DisplayError SdmNullDisplay::Commit(struct drm_output *output) {
  /**
   * TODO: We need to handle releasing the buffer references such that
   * the video buffers/frames keep moving forward in time even though
   * not displayed. This will be done at a later point of time.
   */
  return kErrorNone;
}
DisplayError SdmNullDisplay::SetDisplayState(DisplayState state) {
  return kErrorNone;
}

DisplayError SdmNullDisplay::SetVSyncState(bool enable, struct drm_output *output) {
  /**
   * TODO: drm_output_ needs to be re-initialized based on the preferred supported mode
   *       of the plugged-in display. The recent Weston release contains better APIs
   *       to handle this case. Hence this implementation will be improved based upon
   *       the recent Weston release updates.
   */
  drm_output_ = output;
  return kErrorNone;
}

DisplayError SdmNullDisplay::GetDisplayConfiguration(struct DisplayConfigInfo *display_config) {
  uint32_t props_value[3] = {0};
  char null_display_props[MAX_PROP_STR_SIZE] = {0};
  char *prop = NULL, *saveptr = NULL;

  // sdm.null.resolution format is width:height:fps
  SdmDisplayDebugger::Get()->GetProperty(SDM_NULL_DISPLAY_RESOLUTON_PROP_NAME, null_display_props);

  prop = strtok_r(null_display_props, ":", &saveptr);
  for (int i =0; i<3 && prop != NULL; i++)
  {
    props_value[i] = UINT32(atoi(prop));
    prop = strtok_r(NULL, ":", &saveptr);
  }

  if (props_value[0] == 0 || props_value[1] == 0) {
    display_config->x_pixels = SDM_DEAFULT_NULL_DISPLAY_WIDTH;
    display_config->y_pixels = SDM_DEAFULT_NULL_DISPLAY_HEIGHT;
  } else {
    display_config->x_pixels = props_value[0];
    display_config->y_pixels = props_value[1];
  }

  if (props_value[2] == 0)
    display_config->fps = SDM_DEAFULT_NULL_DISPLAY_FPS;
  else
    display_config->fps = props_value[2];

  display_config->x_dpi = SDM_DEAFULT_NULL_DISPLAY_X_DPI;
  display_config->y_dpi = SDM_DEAFULT_NULL_DISPLAY_Y_DPI;
  display_config->vsync_period_ns = UINT32(1000000000/display_config->fps);
  display_config->is_yuv = SDM_DEAFULT_NULL_DISPLAY_IS_YUV;

  return kErrorNone;
}
DisplayError SdmNullDisplay::RegisterCb(int display_id, vblank_cb_t vbcb) {
  vblank_cb_   = vbcb;

  return kErrorNone;
}
DisplayError SdmNullDisplay::EnablePllUpdate(int32_t enable) {
  return kErrorNone;
}
DisplayError SdmNullDisplay::UpdateDisplayPll(int32_t ppm) {
  return kErrorNone;
}
DisplayError SdmNullDisplay::GetHdrInfo(struct DisplayHdrInfo *display_hdr_info) {
  return kErrorNone;
}
DisplayError SdmNullDisplay::GetHdcpProtocol(struct DisplayHdcpProtocol *display_hdcp_protocol) {
  return kErrorNone;
}


SdmDisplayProxy::SdmDisplayProxy(DisplayType type, CoreInterface *core_intf,
                                 SdmDisplayBufferAllocator *buffer_allocator)
  : disp_type_(type), core_intf_(core_intf),
    sdm_disp_(type, core_intf, buffer_allocator), null_disp_(type, core_intf) {

    display_intf_ = &sdm_disp_;
    buffer_allocator_ = buffer_allocator;
    std::thread uevent_thread(UeventThread, this);
    uevent_thread_.swap(uevent_thread);
}

SdmDisplayProxy::~SdmDisplayProxy () {
    uevent_thread_exit_ = true;
    uevent_thread_.detach();
}

int SdmDisplayProxy::HandleHotplug(bool connected) {
  DisplayError error = kErrorNone;

  DLOGI("HandleHotplug = %d", connected);

  if (connected) {
    if (display_intf_->GetDisplayIntfType() == null_disp) {
      display_intf_ = &sdm_disp_;
      error = display_intf_->CreateDisplay();
      if (error != kErrorNone) {
        DLOGE("Failed to create display %d", error);
        display_intf_ = &null_disp_;
        return error;
      }
      display_intf_->SetDisplayState(kStateOn);
      display_intf_->SetVSyncState(true, drm_output_);

      if (hotplug_cb_) {
        hotplug_cb_(disp_type_, connected, drm_output_);
      }

      DLOGI("Display is connected successfully.");
    } else {
      DLOGI("Display is already connected.");
    }
  } else {
    if (display_intf_->GetDisplayIntfType() == sdm_disp) {
      if (hotplug_cb_) {
        hotplug_cb_(disp_type_, connected, drm_output_);
      }

      display_intf_->SetVSyncState(false, drm_output_);
      display_intf_->DestroyDisplay();

      display_intf_ = &null_disp_;

      DLOGI("Display is disconnected successfully.");
    } else {
      DLOGI("Display is already disconnected.");
    }
  }
}

void *SdmDisplayProxy::UeventThread(void *context) {
  if (context) {
    return reinterpret_cast<SdmDisplayProxy *>(context)->UeventThreadHandler();
  }

  return NULL;
}

#define HAL_PRIORITY_URGENT_DISPLAY (-8)

void *SdmDisplayProxy::UeventThreadHandler() {
  static char uevent_data[4096];
  int length = 0;
  prctl(PR_SET_NAME, uevent_thread_name_, 0, 0, 0);
  setpriority(PRIO_PROCESS, 0, HAL_PRIORITY_URGENT_DISPLAY);
  if (!uevent_init()) {
    DLOGE("Failed to init uevent");
    pthread_exit(0);
    return NULL;
  }

  DLOGI("SdmDisplayProxy::UeventThreadHandler");

  while (!uevent_thread_exit_) {
    // keep last 2 zeroes to ensure double 0 termination
    length = uevent_next_event(uevent_data, INT32(sizeof(uevent_data)) - 2);
    string s(uevent_data, length);

    if (s.find("name=HDMI-A-1") != string::npos) {
      bool connected = s.find("status=connected") != string::npos;
      DLOGI("HDMI is %s !\n", connected ? "connected" : "removed");
      HandleHotplug(connected);
    }
  }
  pthread_exit(0);

  return NULL;
}
#ifdef __cplusplus
}
#endif

}  // namespace sdm
