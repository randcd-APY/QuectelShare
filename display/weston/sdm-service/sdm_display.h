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

#ifndef __SDM_DISPLAY_H__
#define __SDM_DISPLAY_H__

#include <core/core_interface.h>
#include <core/display_interface.h>
#include <core/debug_interface.h>
#include <core/dump_interface.h>
#include <utils/debug.h>
#include <utils/constants.h>
#include <utils/formats.h>
#include <stdio.h>
#include <string>
#include <utility>
#include <map>
#include <vector>
#include <iostream>
#include <thread>

#include "sdm_display_debugger.h"
#include "sdm_display_interface.h"
#include "sdm_display_buffer_allocator.h"
#include "sdm_display_buffer_sync_handler.h"
#include "sdm_display_socket_handler.h"
#include "sdm_display_tonemapper.h"
#include "compositor-sdm-output.h"
#include "drm_master.h"

namespace sdm {
using namespace drm_utils;

using std::vector;
using std::iterator;
using std::string;
using std::to_string;
using std::map;
using std::pair;
using std::fstream;

enum SdmDisplayIntfType {null_disp, sdm_disp};

class SdmDisplayInterface {
  public:
    virtual ~SdmDisplayInterface() {}

    virtual DisplayError CreateDisplay() = 0;
    virtual DisplayError DestroyDisplay() = 0;
    virtual DisplayError Prepare(struct drm_output *output) = 0;
    virtual DisplayError Commit(struct drm_output *output) = 0;
    virtual DisplayError SetDisplayState(DisplayState state) = 0;
    virtual DisplayError SetVSyncState(bool enable, struct drm_output *output) = 0;
    virtual DisplayError GetDisplayConfiguration(struct DisplayConfigInfo *display_config) = 0;
    virtual DisplayError RegisterCb(int display_id, vblank_cb_t vbcb) = 0;
    virtual DisplayError EnablePllUpdate(int32_t enable) = 0;
    virtual DisplayError UpdateDisplayPll(int32_t ppm) = 0;
    virtual DisplayError GetHdrInfo(struct DisplayHdrInfo *display_hdr_info) = 0;
    virtual DisplayError GetHdcpProtocol(struct DisplayHdcpProtocol *display_hdcp_protocol) = 0;
    virtual SdmDisplayIntfType GetDisplayIntfType() = 0;

    static int GetDrmMasterFd();
};

class SdmNullDisplay : public SdmDisplayInterface {
  public:
    SdmNullDisplay(DisplayType type, CoreInterface *core_intf);
    ~SdmNullDisplay();

    SdmDisplayIntfType GetDisplayIntfType() {
      return null_disp;
    }
    DisplayError CreateDisplay();
    DisplayError DestroyDisplay();
    DisplayError Prepare(struct drm_output *output);
    DisplayError Commit(struct drm_output *output);
    DisplayError SetDisplayState(DisplayState state);
    DisplayError SetVSyncState(bool enable, struct drm_output *output);
    DisplayError GetDisplayConfiguration(struct DisplayConfigInfo *display_config);
    DisplayError RegisterCb(int display_id, vblank_cb_t vbcb);
    DisplayError EnablePllUpdate(int32_t enable);
    DisplayError UpdateDisplayPll(int32_t ppm);
    DisplayError GetHdrInfo(struct DisplayHdrInfo *display_hdr_info);
    DisplayError GetHdcpProtocol(struct DisplayHdcpProtocol *display_hdcp_protocol);
};

class SdmDisplay : public SdmDisplayInterface, DisplayEventHandler, SdmDisplayDebugger {

 public:
    SdmDisplay(DisplayType type, CoreInterface *core_intf,
                                 SdmDisplayBufferAllocator *buffer_allocator);
    ~SdmDisplay();

    SdmDisplayIntfType GetDisplayIntfType() {
      return sdm_disp;
    }

    DisplayError CreateDisplay();
    DisplayError DestroyDisplay();
    DisplayError Prepare(struct drm_output *output);
    DisplayError Commit(struct drm_output *output);
    DisplayError SetDisplayState(DisplayState state);
    DisplayError SetVSyncState(bool enable, struct drm_output *output);
    DisplayError GetDisplayConfiguration(struct DisplayConfigInfo *display_config);
    DisplayError RegisterCb(int display_id, vblank_cb_t vbcb);
    DisplayError EnablePllUpdate(int32_t enable);
    DisplayError UpdateDisplayPll(int32_t ppm);

    DisplayError GetHdrInfo(struct DisplayHdrInfo *display_hdr_info);
    DisplayError GetHdcpProtocol(struct DisplayHdcpProtocol *display_hdcp_protocol);

 protected:
    virtual DisplayError VSync(const DisplayEventVSync &vsync);
    virtual DisplayError VSync(int fd, unsigned int sequence,
                               unsigned int tv_sec, unsigned int tv_usec,
                               void *data);
    virtual DisplayError PFlip(int fd, unsigned int sequence,
                               unsigned int tv_sec, unsigned int tv_usec,
                               void *data);
    virtual DisplayError CECMessage(char *message);
    virtual DisplayError Refresh();

 private:
    static const int kBufferDepth = 2;
    DisplayError FreeLayerStack();
    DisplayError FreeLayerGeometry(struct LayerGeometry *glayer);
    DisplayError AllocateMemoryForLayerGeometry(struct drm_output *output,
                                                uint32_t index,
                                                struct LayerGeometry *glayer);
    DisplayError AddGeometryLayerToLayerStack(struct drm_output *output,
                                              uint32_t index,
                                              struct LayerGeometry *glayer, bool is_skip);
    DisplayError AllocLayerStackMemory(struct drm_output *output);
    DisplayError PopulateLayerGeometryOnToLayerStack(struct drm_output *output,
                                                     uint32_t index,
                                                     struct LayerGeometry *glayer, bool is_skip);
    int PrepareNormalLayerGeometry(struct drm_output *output,
                                   struct LayerGeometry **glayer,
                                   struct sdm_layer *sdm_layer);
    int PrepareFbLayerGeometry(struct drm_output *output,
                               struct LayerGeometry **fb_glayer);
    DisplayError PrePrepareLayerStack(struct drm_output *output);
    DisplayError PrePrepare(struct drm_output *output);
    DisplayError PostPrepare(struct drm_output *output);
    DisplayError PreCommit();
    DisplayError PostCommit();
    LayerBufferFormat GetSDMFormat(uint32_t src_fmt,
                                   struct LayerGeometryFlags flags);
    LayerBlending GetSDMBlending(uint32_t source);
    void DumpInputBuffers(void *compositor_output);
    void DumpOutputBuffer(const BufferInfo& buffer_info,
                          void *base, int fence);
    const char*  GetDisplayString();
    /* support functions */
    const char * FourccToString(uint32_t fourcc);
    uint32_t GetMappedFormatFromGbm(uint32_t fmt);
    bool GetVideoPresenceByFormatFromGbm(uint32_t fmt);
    uint32_t GetMappedFormatFromShm(uint32_t fmt);
    bool NeedConvertGbmFormat(struct weston_view *ev, uint32_t format);
    uint32_t ConvertToOpaqueGbmFormat(uint32_t format);
    void ComputeSrcDstRect(struct drm_output *output, struct weston_view *ev,
                                  struct Rect *src_ret, struct Rect *dst_ret);
    int  ComputeDirtyRegion(struct weston_view *ev, struct RectArray *dirty);
    uint8_t GetGlobalAlpha(struct weston_view *ev);
    int GetVisibleRegion(struct drm_output *output, struct weston_view *ev,
                         pixman_region32_t *aboved_opaque, struct RectArray *visible);
    bool IsTransparentGbmFormat(uint32_t format);
    CoreInterface *core_intf_ = NULL;
    SdmDisplayBufferAllocator *buffer_allocator_;
    SdmDisplayBufferSyncHandler buffer_sync_handler_;
    SdmDisplaySocketHandler socket_handler_;
    DisplayEventHandler *client_event_handler_ = NULL;
    DisplayInterface *display_intf_ = NULL;
    DisplayType display_type_ = kDisplayMax;
    DisplayConfigVariableInfo variable_info_;
    HWDisplayInterfaceInfo hw_disp_info_;
    bool shutdown_pending_ = false;
    LayerStack layer_stack_;
    int  display_id_ = -1;
    uint32_t fps_ = 0;
    float max_luminance_ = 0.0;
    float max_average_luminance_ = 0.0;
    float min_luminance_ = 0.0;
    SdmDisplayToneMapper *tone_mapper_ = NULL;
    int disable_hdr_handling_ = 0;
    bool hdr_supported_ = false;
    uint32_t hdcp_version_ = 0;
};

class SdmDisplayProxy {
  public:
    SdmDisplayProxy(DisplayType type, CoreInterface *core_intf,
                    SdmDisplayBufferAllocator *buffer_allocator);
    ~SdmDisplayProxy();

    DisplayError CreateDisplay() {
      DisplayError rc = display_intf_->CreateDisplay();
      if (rc != kErrorNone)
        display_intf_ = &null_disp_;
      return kErrorNone;
    }
    DisplayError DestroyDisplay() { return display_intf_->DestroyDisplay(); }
    DisplayError Prepare(struct drm_output *output) {
      return display_intf_->Prepare(output);
    }
    DisplayError Commit(struct drm_output *output) {
      return display_intf_->Commit(output);
    }
    DisplayError SetDisplayState(DisplayState state) {
      return display_intf_->SetDisplayState(state);
    }
    DisplayError SetVSyncState(bool enable, struct drm_output *output) {
      return display_intf_->SetVSyncState(enable, output);
    }
    DisplayError GetDisplayConfiguration(struct DisplayConfigInfo *display_config) {
      return display_intf_->GetDisplayConfiguration(display_config);
    }
    DisplayError RegisterCbs(int display_id, sdm_cbs_t *cbs) {
      // TODO: move vblank_cb up?
      hotplug_cb_ = cbs->hotplug_cb;
      return display_intf_->RegisterCb(display_id, cbs->vblank_cb);
    }
    DisplayError EnablePllUpdate(int32_t enable) {
      return display_intf_->EnablePllUpdate(enable);
    }
    DisplayError UpdateDisplayPll(int32_t ppm) {
      return display_intf_->UpdateDisplayPll(ppm);
    }
    DisplayError GetHdrInfo(struct DisplayHdrInfo *display_hdr_info) {
      return display_intf_->GetHdrInfo(display_hdr_info);
    }
    DisplayError GetHdcpProtocol(struct DisplayHdcpProtocol *display_hdcp_protocol) {
      return display_intf_->GetHdcpProtocol(display_hdcp_protocol);
    }

    int HandleHotplug(bool connected);

  private:
    // Uevent thread
    static void *UeventThread(void *context);
    void *UeventThreadHandler();

    SdmDisplayInterface *display_intf_;
    DisplayType disp_type_;
    CoreInterface *core_intf_;
    SdmNullDisplay null_disp_;
    SdmDisplayBufferAllocator *buffer_allocator_;
    SdmDisplay sdm_disp_;
    std::thread uevent_thread_;
    bool uevent_thread_exit_ = false;
    const char *uevent_thread_name_ = "SDM_UeventThread";
    hotplug_cb_t hotplug_cb_;
};

}  // namespace sdm

#endif  // __SDM_DISPLAY_H__
