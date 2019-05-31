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

#include "sdm_display.h"
#include "sdm_display_connect.h"
#include "uevent.h"

#ifdef __cplusplus
extern "C" {
#endif

#define __CLASS__ "SdmDisplayConnect"
namespace sdm {

#define SDM_DISPLAY_DEBUG 0

enum {
       FAIL,
       SUCCESS
};

CoreInterface *core_intf_ = NULL;
SdmDisplayBufferAllocator *buffer_allocator_;
SdmDisplayBufferSyncHandler buffer_sync_handler_;
SdmDisplaySocketHandler socket_handler_;
HWDisplayInterfaceInfo hw_disp_info_;
SdmDisplayProxy *display_[kDisplayMax] = {0};

int CreateCore()
{
    DisplayError error = kErrorNone;
    if (core_intf_) {
        DLOGW("Core was already created.");
        return kErrorNone;
    }
    buffer_allocator_ = new SdmDisplayBufferAllocator;

    error = CoreInterface::CreateCore(SdmDisplayDebugger::Get(),
                                      buffer_allocator_,
                                      &buffer_sync_handler_,
                                      &socket_handler_,
                                      &core_intf_);
    if (!core_intf_) {
        DLOGE("function failed. Error = %d", error);
        return error;
    }

    #if SDM_DISPLAY_DEBUG
    DLOGD("successfully created.");
    #endif

    return kErrorNone;
}

int DestroyCore()
{
    DisplayError error = kErrorNone;

    if (!core_intf_) {
        DLOGE("Core was already destroyed => core_intf_ = NULL");
        return kErrorNone;
    }

    for(int i = 0; i < kDisplayMax; i++) {
        if (display_[i] != NULL) {
            error = display_[i]->DestroyDisplay();
            if (error != kErrorNone) {
                DLOGE("Destroy action failed for display(%d). error = %d",
                      i, error);
                DLOGE("Trying to delete display(%d) anyways.", i);
            }
            delete display_[i];
            display_[i] = NULL;
        }
    }

    error = CoreInterface::DestroyCore();
    if (error != kErrorNone) {
        DLOGE("function failed. Error = %d", error);
        return error;
    }
    core_intf_ = NULL;
    delete buffer_allocator_;

    #if SDM_DISPLAY_DEBUG
    DLOGD("Core was destroyed successfully");
    #endif

    return kErrorNone;
}

int GetFirstDisplayType(int *display_id)
{
    DisplayError error = kErrorNone;

    *display_id = -1; /* Initialize with invalid display type */
    if (!core_intf_) {
        DLOGE("function failed as core was not created.");
        return kErrorNotSupported;
    }

    error = core_intf_->GetFirstDisplayInterfaceType(&hw_disp_info_);
    if (error != kErrorNone) {
        DLOGE("function GetFirstDisplayInterfaceType failed: error = %d",
              error);
        return error;
    }
    *display_id = hw_disp_info_.type;

    #if SDM_DISPLAY_DEBUG
    DLOGD("function successful: display id = %d", *display_id);
    #endif

    return kErrorNone;
}

int CreateDisplay(int display_id)
{
    DisplayError error = kErrorNone;

    if (display_id >= kDisplayMax || display_id < 0) {
        DLOGE("Display id(%d) out of range.", display_id);
        return kErrorParameters;
    }

    if (display_[display_id] != NULL) {
        DLOGE("Display(%d) was already created.", display_id);
        return kErrorNone;
    }

    if (core_intf_ == NULL) {
        DLOGE("Core is not created yet.");
        return kErrorNotSupported;
    }

    enum DisplayType display_type;
    switch(display_id) {
       case 0:  display_type  = kPrimary;    break;
       case 1:  display_type  = kHDMI;       break;
       case 2:  display_type  = kVirtual;    break;
       default: display_type  = kDisplayMax; break;
    }

    SdmDisplayProxy *sdm_display = new SdmDisplayProxy(display_type, core_intf_, buffer_allocator_);

    display_[display_id] = sdm_display;
    error = display_[display_id]->CreateDisplay() ;
    if (error != kErrorNone) {
        DLOGE("Failed to create display(%d)", display_id);
        delete display_[display_id];
        display_[display_id] = NULL;

        return error;
    }

    #if SDM_DISPLAY_DEBUG
    DLOGD("Display(%d) created successfully.", display_id);
    #endif

    return kErrorNone;
}

int Prepare(int display_id, struct drm_output *output)
{
    DisplayError error = kErrorNone;

    if (display_id >= kDisplayMax || display_id < 0) {
        DLOGE("Display id(%d) out of range.", display_id);
        return kErrorParameters;
    }

    if (!display_[display_id]) {
        DLOGE("Failed as Display(%d) not created yet.",
              display_id);
        return kErrorNotSupported;
    }

    error = display_[display_id]->Prepare(output);
    if (error != kErrorNone) {
        DLOGE("function failed with error = %d", error);
        return error;
    }

    #if SDM_DISPLAY_DEBUG
    DLOGD("function successful.");
    #endif

    return kErrorNone;
}

int Commit(int display_id, struct drm_output *output)
{
    DisplayError error = kErrorNone;

    if (display_id >= kDisplayMax || display_id < 0) {
        DLOGE("Display id(%d) out of range.", display_id);
        return kErrorParameters;
    }

    if (!display_[display_id]) {
        DLOGE("function failed as Display(%d) not created yet.",
              display_id);
        return kErrorNotSupported;
    }

    error = display_[display_id]->Commit(output);
    if (error != kErrorNone) {
        DLOGE("function failed with error = %d", error);
        return error;
    }

    #if SDM_DISPLAY_DEBUG
    DLOGD("function successful.");
    #endif

    return kErrorNone;
}

int DestroyDisplay(int display_id)
{
    DisplayError error = kErrorNone;

    if (display_id >= kDisplayMax || display_id < 0) {
        DLOGE("Display id(%d) out of range.", display_id);
        return kErrorParameters;
    }

    if (!display_[display_id]) {
        DLOGE("Display(%d) was already destroyed.", display_id);
        return kErrorNone;
    }

    SdmDisplayProxy *temp_display = display_[display_id];
    error = temp_display->DestroyDisplay();
    delete temp_display;
    display_[display_id] = NULL;

    if (error != kErrorNone) {
        DLOGE("function failed with error = %d", error);
        return error;
    }

    #if SDM_DISPLAY_DEBUG
    DLOGD("function successful.");
    #endif

    return kErrorNone;
}

bool GetDisplayConfiguration(int display_id, struct DisplayConfigInfo *display_config)
{
    DisplayError error = kErrorNone;

    if (display_id >= kDisplayMax || display_id < 0) {
        DLOGE("Display id(%d) out of range.", display_id);
        return FAIL;
    }

    if (!display_[display_id]) {
        DLOGE("function failed. Display(%d) not created yet.", display_id);
        return FAIL;
    }

    error = display_[display_id]->GetDisplayConfiguration(display_config);

    if (error != kErrorNone) {
        DLOGE("function failed with error = %d", error);
        return FAIL;
    }

    #if SDM_DISPLAY_DEBUG
    DLOGD("function successful.");
    #endif

    return SUCCESS;
}

bool GetDisplayHdrInfo(int display_id, struct DisplayHdrInfo *display_hdr_info)
{
    DisplayError error = kErrorNone;

    if (display_id >= kDisplayMax || display_id < 0) {
        DLOGE("Display id(%d) out of range.", display_id);
        return FAIL;
    }

    if (!display_[display_id]) {
        DLOGE("function failed. Display(%d) not created yet.", display_id);
        return FAIL;
    }

    error = display_[display_id]->GetHdrInfo(display_hdr_info);

    if (error != kErrorNone) {
        DLOGE("function failed with error = %d", error);
        return FAIL;
    }

    #if SDM_DISPLAY_DEBUG
    DLOGD("function successful.");
    #endif

    return SUCCESS;
}

bool GetDisplayHdcpProtocol(int display_id, struct DisplayHdcpProtocol *display_hdcp_protocol)
{
    DisplayError error = kErrorNone;

    if (display_id >= kDisplayMax || display_id < 0) {
        DLOGE("Display id(%d) out of range.", display_id);
        return FAIL;
    }

    if (!display_[display_id]) {
        DLOGE("function failed. Display(%d) not created yet.", display_id);
        return FAIL;
    }

    error = display_[display_id]->GetHdcpProtocol(display_hdcp_protocol);

    if (error != kErrorNone) {
        DLOGE("function failed with error = %d", error);
        return FAIL;
    }

    #if SDM_DISPLAY_DEBUG
    DLOGD("function successful.");
    #endif

    return SUCCESS;
}

int RegisterCbs(int display_id, sdm_cbs *cbs) {
    DisplayError error = kErrorNone;

    if (display_id >= kDisplayMax || display_id < 0) {
        DLOGE("Display id(%d) out of range.", display_id);
        return kErrorParameters;
    }

    if (!display_[display_id]) {
        DLOGE("function failed. Display(%d) not created yet.",
              display_id);
        return kErrorParameters;
    }

    error = display_[display_id]->RegisterCbs(display_id, cbs);

    if (error != kErrorNone) {
        DLOGE("function failed with error = %d", error);
        return error;
    }

    #if SDM_DISPLAY_DEBUG
    DLOGD("function successful.");
    #endif

    return kErrorNone;
}

int get_drm_master_fd() {

    int fd = SdmDisplayInterface::GetDrmMasterFd();

    #if SDM_DISPLAY_DEBUG
    DLOGD("fd is: %d \n", fd);
    #endif

    return fd;
}

int SetDisplayState(int display_id, int power_mode) {
    DisplayError error = kErrorNone;

    if (display_id >= kDisplayMax || display_id < 0) {
        DLOGE("Display id(%d) out of range.", display_id);
        return kErrorParameters;
    }

    if (!display_[display_id]) {
        DLOGE("function failed. Display(%d) not created yet.",
              display_id);
        return kErrorParameters;
    }

    /* When WESTON_DPMS_ON == 0, set state ON (kStateOn)     */
    /* for all other power modes, i.e. WESTON_DPMS_STANDBY,  */
    /* WESTON_DPMS_SUSPEND, WESTON_DPMS_OFF turn off display */
    /* set state off (kStateOff)                             */
    error = display_[display_id]->SetDisplayState((power_mode == \
                                                   WESTON_DPMS_ON)? \
                                                   kStateOn: kStateOff);
    if (error != kErrorNone) {
        DLOGE("function failed with error = %d", error);
        return error;
    }

    #if SDM_DISPLAY_DEBUG
    DLOGD("function successful.");
    #endif

    return kErrorNone;
}

int SetVSyncState(int display_id, bool state, struct drm_output *output)
{
    DisplayError error = kErrorNone;

    if (display_id >= kDisplayMax || display_id < 0) {
        DLOGE("Display id(%d) out of range.", display_id);
        return kErrorParameters;
    }

    if (!display_[display_id]) {
        DLOGE("function failed. Display(%d) not created yet.",
              display_id);
        return kErrorParameters;
    }

    error = display_[display_id]->SetVSyncState(state, output);
    if (error != kErrorNone) {
        DLOGE("function failed with error = %d", error);
        return error;
    }

    #if SDM_DISPLAY_DEBUG
    DLOGD("function successful.");
    #endif

    return kErrorNone;
}

int EnablePllUpdate(int display_id, int enable)
{
    return display_[display_id]->EnablePllUpdate(enable);
}

int UpdateDisplayPll(int display_id, int enable)
{
    return display_[display_id]->UpdateDisplayPll(enable);
}

}// namespace sdm
#ifdef __cplusplus
}
#endif
