/*
* Copyright (c) 2015 - 2018, The Linux Foundation. All rights reserved.
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
*/

#include <drm_lib_loader.h>
#include <drm_master.h>
#include <drm_res_mgr.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <utils/debug.h>
#include <utils/sys.h>
#include <utils/formats.h>
#include <drm/msm_drm.h>
#include <vector>
#include <map>
#include <utility>
#include <sys/time.h>

#include "hw_hdmi_drm.h"

#define __CLASS__ "HWHDMIDRM"

#define DRM_MODE_FLAG_PIC_AR_SHIFT 24

#ifndef DRM_MODE_FLAG_PIC_AR_MASK
#define DRM_MODE_FLAG_PIC_AR_MASK (0x0F<<DRM_MODE_FLAG_PIC_AR_SHIFT)
#endif

#ifndef DRM_MODE_FLAG_SUPPORTS_RGB
#define DRM_MODE_FLAG_SUPPORTS_RGB (1<<20)
#endif

#ifndef DRM_MODE_FLAG_SUPPORTS_YUV
#define DRM_MODE_FLAG_SUPPORTS_YUV (1<<21)
#endif

#define DRM_MODE_FLAG_FMT_MASK (3<<20)
#define DRM_MODE_FLAG_SUPPORTS_SHIFT 20
#define  MIN_HDR_RESET_WAITTIME 2
#define HDR_ENABLE   DRM_MSM_HDR_ENABLE
#define HDR_DISABLE  DRM_MSM_HDR_DISABLE
#define HDR_RESET    DRM_MSM_HDR_RESET

#define MDP_HDR_EOTF_SMTPE_ST2084	HDR_EOTF_SMTPE_ST2084
#define MDP_HDR_EOTF_HLG		HDR_EOTF_HLG

using drm_utils::DRMMaster;
using drm_utils::DRMResMgr;
using drm_utils::DRMLibLoader;
using drm_utils::DRMBuffer;
using sde_drm::GetDRMManager;
using sde_drm::DestroyDRMManager;
using sde_drm::DRMDisplayType;
using sde_drm::DRMDisplayToken;
using sde_drm::DRMConnectorInfo;
using sde_drm::DRMPPFeatureInfo;
using sde_drm::DRMOps;
using sde_drm::DRMTopology;

namespace sdm {

HWHDMIDRM::HWHDMIDRM(BufferSyncHandler *buffer_sync_handler, BufferAllocator *buffer_allocator,
                     HWInfoInterface *hw_info_intf)
  : HWDeviceDRM(buffer_sync_handler, buffer_allocator, hw_info_intf),
  active_config_index_(0) {
  HWDeviceDRM::device_type_ = kDeviceHDMI;
  HWDeviceDRM::device_name_ = "HDMI Display Device";
}

// TODO(user) : split function in base class and avoid code duplicacy
// by using base implementation for this basic stuff
DisplayError HWHDMIDRM::Init() {
  DisplayError error = kErrorNone;

  default_mode_ = (DRMLibLoader::GetInstance()->IsLoaded() == false);

  if (!default_mode_) {
    DRMMaster *drm_master = {};
    int dev_fd = -1;
    DRMMaster::GetInstance(&drm_master);
    drm_master->GetHandle(&dev_fd);
    DRMLibLoader::GetInstance()->FuncGetDRMManager()(dev_fd, &drm_mgr_intf_);
    if (drm_mgr_intf_->RegisterDisplay(DRMDisplayType::TV, &token_)) {
      DLOGE("RegisterDisplay failed");
      return kErrorResources;
    }

    drm_mgr_intf_->CreateAtomicReq(token_, &drm_atomic_intf_);
    drm_mgr_intf_->GetConnectorInfo(token_.conn_id, &connector_info_);
    InitializeConfigs();
   }
  hw_info_intf_->GetHWResourceInfo(&hw_resource_);
  // TODO(user): In future, remove has_qseed3 member, add version and pass version to constructor
  if (hw_resource_.has_qseed3) {
    hw_scale_ = new HWScaleDRM(HWScaleDRM::Version::V2);
  }

  if (error != kErrorNone) {
    return error;
  }

  return error;
}

void HWHDMIDRM::PopulateHWPanelInfo() {
  hw_panel_info_ = {};

  HWDeviceDRM::PopulateHWPanelInfo();
#ifdef DRM_MSM_EXT_PANEL_HDR_CTRL
  hw_panel_info_.hdr_metadata_type_one = connector_info_.hdr_prop.hdr_metadata_type_one;
  hw_panel_info_.hdr_enabled = connector_info_.hdr_prop.hdr_supported;
  hw_panel_info_.hdr_eotf = connector_info_.hdr_prop.hdr_eotf;
  hw_panel_info_.peak_luminance = connector_info_.hdr_prop.hdr_max_luminance;
  hw_panel_info_.average_luminance = connector_info_.hdr_prop.hdr_avg_luminance;
  hw_panel_info_.blackness_level = connector_info_.hdr_prop.hdr_min_luminance;
#endif
}

DisplayError HWHDMIDRM::GetNumDisplayAttributes(uint32_t *count) {
  *count = UINT32(connector_info_.modes.size());
  if (*count <= 0) {
    return kErrorHardware;
  }

  return kErrorNone;
}

DisplayError HWHDMIDRM::GetActiveConfig(uint32_t *active_config_index) {
  *active_config_index = active_config_index_;
  return kErrorNone;
}

DisplayError HWHDMIDRM::GetDisplayAttributes(uint32_t index,
                                            HWDisplayAttributes *display_attributes) {
  *display_attributes = display_attributes_;
  drmModeModeInfo mode = {};
  uint32_t mm_width = 0;
  uint32_t mm_height = 0;
  DRMTopology topology = DRMTopology::SINGLE_LM;

  if (default_mode_) {
    DRMResMgr *res_mgr = nullptr;
    int ret = DRMResMgr::GetInstance(&res_mgr);
    if (ret < 0) {
      DLOGE("Failed to acquire DRMResMgr instance");
      return kErrorResources;
    }

    res_mgr->GetMode(&mode);
    res_mgr->GetDisplayDimInMM(&mm_width, &mm_height);
  } else {
    if (index >= connector_info_.modes.size()) {
      DLOGE("Invalid mode index %d mode size %d", index, UINT32(connector_info_.modes.size()));
      return kErrorResources;
    }
    mode = connector_info_.modes[index];
    mm_width = mode.hdisplay;
    mm_height = mode.vdisplay;
    topology = connector_info_.topology;
  }

  display_attributes-> x_pixels = mode.hdisplay;
  display_attributes->y_pixels = mode.vdisplay;
  display_attributes->fps = mode.vrefresh;
  display_attributes->vsync_period_ns = UINT32(1000000000L / display_attributes->fps);

  /*
              Active                 Front           Sync           Back
              Region                 Porch                          Porch
     <-----------------------><----------------><-------------><-------------->
     <----- [hv]display ----->
     <------------- [hv]sync_start ------------>
     <--------------------- [hv]sync_end --------------------->
     <-------------------------------- [hv]total ----------------------------->
   */

  display_attributes->v_front_porch = mode.vsync_start - mode.vdisplay;
  display_attributes->v_pulse_width = mode.vsync_end - mode.vsync_start;
  display_attributes->v_back_porch = mode.vtotal - mode.vsync_end;
  display_attributes->v_total = mode.vtotal;

  display_attributes->h_total = mode.htotal;
  uint32_t h_blanking = mode.htotal - mode.hdisplay;
  display_attributes->is_device_split =
      (topology == DRMTopology::DUAL_LM || topology == DRMTopology::DUAL_LM_MERGE);
  display_attributes->h_total += display_attributes->is_device_split ? h_blanking : 0;

  display_attributes->x_dpi = (FLOAT(mode.hdisplay) * 25.4f) / FLOAT(mm_width);
  display_attributes->y_dpi = (FLOAT(mode.vdisplay) * 25.4f) / FLOAT(mm_height);

  return kErrorNone;
}

DisplayError HWHDMIDRM::SetDisplayAttributes(uint32_t index) {
  // TODO check if index will start from 0? then >=
  if (index >= connector_info_.modes.size()) {
    DLOGE("Invalid mode index %d mode size %d", index, UINT32(connector_info_.modes.size()));
    return kErrorNotSupported;
  }

  active_config_index_ = index;
  // Get the display attributes for current active config index
  GetDisplayAttributes(active_config_index_, &display_attributes_);

  frame_rate_ = display_attributes_.fps;
  current_mode_ = connector_info_.modes[index];

  drm_atomic_intf_->Perform(DRMOps::CRTC_SET_MODE, token_.crtc_id, &connector_info_.modes[index]);
  drm_atomic_intf_->Perform(DRMOps::CRTC_SET_ACTIVE, token_.crtc_id, 1);
  drm_atomic_intf_->Perform(DRMOps::CRTC_SET_OUTPUT_FENCE_OFFSET, token_.crtc_id, 1);

  if (drm_atomic_intf_->Commit(true /* synchronous */)) {
    DLOGE("Setting up CRTC %d, Connector %d for %s failed", token_.crtc_id, token_.conn_id,
        device_name_);
  return kErrorResources;
  }

  // Reload connector info for updated info after 1st commit
  drm_mgr_intf_->GetConnectorInfo(token_.conn_id, &connector_info_);
  DLOGI("Setup CRTC %d, Connector %d for %s", token_.crtc_id, token_.conn_id, device_name_);

  PopulateDisplayAttributes();
  PopulateHWPanelInfo();
  UpdateMixerAttributes();
  return kErrorNone;
}

DisplayError HWHDMIDRM::GetConfigIndex(char *mode, uint32_t *index) {

  uint32_t width = 0, height = 0, fps = 0, format = 0;
  int32_t aspect_ratio = -1;

  //mode should be in width:height:fps:format:aspect_ratio
  if (sscanf(mode, "%u:%u:%u:%u:%d", &width, &height, &fps, &format, &aspect_ratio) != 5)
     return kErrorParameters;

  for (size_t idex = 0; idex < connector_info_.modes.size(); idex ++) {
    if ((height == connector_info_.modes[idex].vdisplay) &&
        (width == connector_info_.modes[idex].hdisplay) &&
        (fps == connector_info_.modes[idex].vrefresh) &&
        (format & ((connector_info_.modes[idex].flags & DRM_MODE_FLAG_FMT_MASK)
                    >> DRM_MODE_FLAG_SUPPORTS_SHIFT)))
    {
      if ((aspect_ratio >= 0) && ((aspect_ratio << DRM_MODE_FLAG_PIC_AR_SHIFT) !=
          (connector_info_.modes[idex].flags & DRM_MODE_FLAG_PIC_AR_MASK)))
        continue;
      *index = UINT32(idex);
      return kErrorNone;
    }
   }
  return kErrorNotSupported;
}
#ifdef DRM_MSM_EXT_PANEL_HDR_CTRL
static int32_t GetEOTF(const GammaTransfer &transfer) {
  int32_t hdr_transfer = -1;

  switch (transfer) {
  case Transfer_SMPTE_ST2084:
    hdr_transfer = MDP_HDR_EOTF_SMTPE_ST2084;
    break;
  case Transfer_HLG:
    hdr_transfer = MDP_HDR_EOTF_HLG;
    break;
  default:
    DLOGW("Unknown Transfer: %d", transfer);
  }

  return hdr_transfer;
}

DisplayError HWHDMIDRM::UpdateHDRMetaData(HWLayers *hw_layers) {
  static struct timeval hdr_reset_start,hdr_reset_end;
  static bool reset_hdr_flag = false;
  const HWHDRLayerInfo &hdr_layer_info = hw_layers->info.hdr_layer_info;
  if (!hw_panel_info_.hdr_enabled ) {
    return kErrorNone;
  }

  DisplayError error = kErrorNone;

  Layer hdr_layer = {};
  if (hdr_layer_info.operation == HWHDRLayerInfo::kSet && hdr_layer_info.layer_index > -1) {
    hdr_layer = *(hw_layers->info.stack->layers.at(UINT32(hdr_layer_info.layer_index)));
  }

  const LayerBuffer *layer_buffer = &hdr_layer.input_buffer;
  const MasteringDisplay &mastering_display = layer_buffer->color_metadata.masteringDisplayInfo;
  const ContentLightLevel &light_level = layer_buffer->color_metadata.contentLightLevel;
  const Primaries &primaries = mastering_display.primaries;

  if (hdr_layer_info.operation == HWHDRLayerInfo::kSet) {
    //reset reset_hdr_flag to handle where there are two consecutive HDR video playbacks with not
    //enough non-HDR frames in between to reset the HDR metadata.
    reset_hdr_flag = false;

    int32_t eotf = GetEOTF(layer_buffer->color_metadata.transfer);
    connector_info_.hdr_ctrl.hdr_meta.eotf = (eotf < 0) ? 0 : UINT32(eotf);
    connector_info_.hdr_ctrl.hdr_meta.white_point_x = primaries.whitePoint[0];
    connector_info_.hdr_ctrl.hdr_meta.white_point_y = primaries.whitePoint[1];
    connector_info_.hdr_ctrl.hdr_meta.display_primaries_x[0] = primaries.rgbPrimaries[0][0];
    connector_info_.hdr_ctrl.hdr_meta.display_primaries_y[0] = primaries.rgbPrimaries[0][1];
    connector_info_.hdr_ctrl.hdr_meta.display_primaries_x[1] = primaries.rgbPrimaries[1][0];
    connector_info_.hdr_ctrl.hdr_meta.display_primaries_y[1] = primaries.rgbPrimaries[1][1];
    connector_info_.hdr_ctrl.hdr_meta.display_primaries_x[2] = primaries.rgbPrimaries[2][0];
    connector_info_.hdr_ctrl.hdr_meta.display_primaries_y[2] = primaries.rgbPrimaries[2][1];
    connector_info_.hdr_ctrl.hdr_meta.min_luminance = mastering_display.minDisplayLuminance;
    connector_info_.hdr_ctrl.hdr_meta.max_luminance = mastering_display.maxDisplayLuminance/10000;
    connector_info_.hdr_ctrl.hdr_meta.max_content_light_level = light_level.maxContentLightLevel;
    connector_info_.hdr_ctrl.hdr_meta.max_average_light_level = light_level.minPicAverageLightLevel;
    connector_info_.hdr_ctrl.hdr_state = HDR_ENABLE;

    drm_atomic_intf_->Perform(DRMOps::CONNECTOR_SET_HDR_PROP, token_.conn_id,
                              &connector_info_.hdr_ctrl);

    DLOGI("kset: HDR metadata: MaxDisplayLuminance = %d MinDisplayLuminance = %d\n"
      "MaxContentLightLevel = %d MaxAverageLightLevel = %d Red_x = %d Red_y = %d Green_x = %d\n"
      "Green_y = %d Blue_x = %d Blue_y = %d WhitePoint_x = %d WhitePoint_y = %d EOTF = %d\n",
      connector_info_.hdr_ctrl.hdr_meta.max_luminance,
      connector_info_.hdr_ctrl.hdr_meta.min_luminance,
      connector_info_.hdr_ctrl.hdr_meta.max_content_light_level,
      connector_info_.hdr_ctrl.hdr_meta.max_average_light_level,
      connector_info_.hdr_ctrl.hdr_meta.display_primaries_x[0],
      connector_info_.hdr_ctrl.hdr_meta.display_primaries_y[0],
      connector_info_.hdr_ctrl.hdr_meta.display_primaries_x[1],
      connector_info_.hdr_ctrl.hdr_meta.display_primaries_y[1],
      connector_info_.hdr_ctrl.hdr_meta.display_primaries_x[2],
      connector_info_.hdr_ctrl.hdr_meta.display_primaries_y[2],
      connector_info_.hdr_ctrl.hdr_meta.white_point_x,
      connector_info_.hdr_ctrl.hdr_meta.white_point_y,
      connector_info_.hdr_ctrl.hdr_meta.eotf);
  }
  else if (hdr_layer_info.operation == HWHDRLayerInfo::kReset) {
        memset(&connector_info_.hdr_ctrl.hdr_meta,0,sizeof(connector_info_.hdr_ctrl.hdr_meta));
        connector_info_.hdr_ctrl.hdr_meta.hdr_supported = 1;
        connector_info_.hdr_ctrl.hdr_state = HDR_RESET;
        reset_hdr_flag = true;
        gettimeofday(&hdr_reset_start, NULL);
        drm_atomic_intf_->Perform(DRMOps::CONNECTOR_SET_HDR_PROP, token_.conn_id,
                                  &connector_info_.hdr_ctrl);

    DLOGI("kReset: HDR metadata: MaxDisplayLuminance = %d MinDisplayLuminance = %d\n"
      "MaxContentLightLevel = %d MaxAverageLightLevel = %d Red_x = %d Red_y = %d Green_x = %d\n"
      "Green_y = %d Blue_x = %d Blue_y = %d WhitePoint_x = %d WhitePoint_y = %d EOTF = %d\n",
      connector_info_.hdr_ctrl.hdr_meta.max_luminance,
      connector_info_.hdr_ctrl.hdr_meta.min_luminance,
      connector_info_.hdr_ctrl.hdr_meta.max_content_light_level,
      connector_info_.hdr_ctrl.hdr_meta.max_average_light_level,
      connector_info_.hdr_ctrl.hdr_meta.display_primaries_x[0],
      connector_info_.hdr_ctrl.hdr_meta.display_primaries_y[0],
      connector_info_.hdr_ctrl.hdr_meta.display_primaries_x[1],
      connector_info_.hdr_ctrl.hdr_meta.display_primaries_y[1],
      connector_info_.hdr_ctrl.hdr_meta.display_primaries_x[2],
      connector_info_.hdr_ctrl.hdr_meta.display_primaries_y[2],
      connector_info_.hdr_ctrl.hdr_meta.white_point_x,
      connector_info_.hdr_ctrl.hdr_meta.white_point_y,
      connector_info_.hdr_ctrl.hdr_meta.eotf);
   }
   //TODO: Below case handles the state transition from HDR_ENABLED to HDR_DISABLED.
   //As per HDMI spec requirement, we need to send zero metadata for atleast 2 sec after end of
   //playback.This timer calculates the 2 sec window after playback stops to stop sending metadata.
   //This will be replaced with an idle timer implementation in the future.
   else if (hdr_layer_info.operation == HWHDRLayerInfo::kNoOp) {
    if(reset_hdr_flag) {
     gettimeofday(&hdr_reset_end, NULL);
     float hdr_reset_time_start = (hdr_reset_start.tv_sec*1000 + hdr_reset_start.tv_usec/1000);
     float hdr_reset_time_end = (hdr_reset_end.tv_sec*1000 + hdr_reset_end.tv_usec/1000);

     if(((hdr_reset_time_end-hdr_reset_time_start)/1000) >= MIN_HDR_RESET_WAITTIME)
      {
       reset_hdr_flag = false;
       memset(&connector_info_.hdr_ctrl.hdr_meta,0,sizeof(connector_info_.hdr_ctrl.hdr_meta));
       connector_info_.hdr_ctrl.hdr_meta.hdr_supported = 1;
       connector_info_.hdr_ctrl.hdr_state = HDR_DISABLE;

       drm_atomic_intf_->Perform(DRMOps::CONNECTOR_SET_HDR_PROP, token_.conn_id,
                              &connector_info_.hdr_ctrl);
       DLOGI("kNoOp: HDR metadata: HDR_DISABLE");
      }
     }
    }
  return error;
}
#endif

DisplayError HWHDMIDRM::Validate(HWLayers *hw_layers) {
  HWDeviceDRM::ResetDisplayParams();

  return HWDeviceDRM::Validate(hw_layers);
}

DisplayError HWHDMIDRM::Commit(HWLayers *hw_layers) {
#ifdef DRM_MSM_EXT_PANEL_HDR_CTRL
  DisplayError error = UpdateHDRMetaData(hw_layers);
  if (error != kErrorNone) {
    return error;
  }
#endif
  return HWDeviceDRM::Commit(hw_layers);
}

DisplayError HWHDMIDRM::EnablePllUpdate(int32_t enable) {
  drm_atomic_intf_->Perform(DRMOps::CONNECTOR_ENABLE_PLL_UPDATE,
                         token_.conn_id, enable);

  return kErrorNone;
}

DisplayError HWHDMIDRM::UpdateDisplayPll(int32_t ppm) {
  drm_atomic_intf_->Perform(DRMOps::CONNECTOR_UPDATE_DISPLAY_PLL,
                         token_.conn_id, ppm);

  return kErrorNone;
}

}  // namespace sdm

