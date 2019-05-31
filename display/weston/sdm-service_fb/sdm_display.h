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

#ifndef __SDM_DISPLAY_H__
#define __SDM_DISPLAY_H__

#include <core/core_interface.h>
#include <core/display_interface.h>
#include <core/debug_interface.h>
#include <core/dump_interface.h>
#include <utils/debug.h>
#include <utils/constants.h>
#include <utils/formats.h>

#include "sdm_display_debugger.h"
#include "sdm_display_interface.h"
#include "sdm_display_buffer_allocator.h"
#include "sdm_display_buffer_sync_handler.h"
#include "sdm_display_socket_handler.h"
namespace sdm {

enum SdmDisplayIntfType {null_disp, sdm_disp};

class SdmDisplayInterface {
  public:
    virtual ~SdmDisplayInterface() {}

    virtual DisplayError CreateDisplay() = 0;
    virtual DisplayError DestroyDisplay() = 0;
    virtual DisplayError Commit(int fd) = 0;
};

class SdmDisplay : public SdmDisplayInterface, DisplayEventHandler, SdmDisplayDebugger {

 public:
    SdmDisplay(DisplayType type, CoreInterface *core_intf,
                                 SdmDisplayBufferAllocator *buffer_allocator);
    ~SdmDisplay();

    DisplayError CreateDisplay();
    DisplayError DestroyDisplay();
    DisplayError Commit(int fd);
    DisplayError VSync(const DisplayEventVSync &vsync);
    DisplayError VSync(int fd, unsigned int sequence,
                             unsigned int tv_sec, unsigned int tv_usec,
                             void *data);
    DisplayError PFlip(int fd, unsigned int sequence,
                             unsigned int tv_sec, unsigned int tv_usec,
                             void *data);

    DisplayError Refresh();
    DisplayError CECMessage(char *message);
    void SetFBTStride(int stride);

 protected:
    CoreInterface *core_intf_ = NULL;
    SdmDisplayBufferAllocator *buffer_allocator_;
    SdmDisplayBufferSyncHandler buffer_sync_handler_;
    SdmDisplaySocketHandler socket_handler_;
    DisplayInterface *display_intf_ = NULL;
    DisplayType display_type_ = kDisplayMax;
    DisplayConfigVariableInfo fb_config_;
    HWDisplayInterfaceInfo hw_disp_info_;
    LayerStack layer_stack_;
    Layer *gpu_target_layer_;
    Layer *dummy_layer_;
    int  display_id_ = -1;
 private:
    void PostCommit();
    void PrepareFbLayerGeometry();
};

}  // namespace sdm

#endif  // __SDM_DISPLAY_H__
