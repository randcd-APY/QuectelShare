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

#ifndef __SDM_DISPLAY_DEBUGGER_H__
#define __SDM_DISPLAY_DEBUGGER_H__

#include <core/sdm_types.h>
#include <core/debug_interface.h>
#include <stdio.h>
#include <stdarg.h>

#include <cstdlib>
#include <cassert>
#include <stdint.h>

enum {
       NONE,
       ERROR,
       WARNING,
       INFO,
       DEBUG,
       VERBOSE,
       SET_GET_PROP
};

#define WLOG(tag, method, format, ...) SdmDisplayDebugger::Get()->method(tag, \
                                                            __CLASS__ "::%s: " format, \
                                                            __FUNCTION__, ##__VA_ARGS__)

#define WLOGE_IF(tag, format, ...) WLOG(tag, Error, format, ##__VA_ARGS__)
#define WLOGW_IF(tag, format, ...) WLOG(tag, Warning, format, ##__VA_ARGS__)
#define WLOGI_IF(tag, format, ...) WLOG(tag, Info, format, ##__VA_ARGS__)
#define WLOGD_IF(tag, format, ...) WLOG(tag, Debug, format, ##__VA_ARGS__)
#define WLOGV_IF(tag, format, ...) WLOG(tag, Verbose, format, ##__VA_ARGS__)

#define DLOGE(format, ...) WLOGE_IF(kTagNone, format, ##__VA_ARGS__)
#define DLOGD(format, ...) WLOGD_IF(kTagNone, format, ##__VA_ARGS__)
#define DLOGW(format, ...) WLOGW_IF(kTagNone, format, ##__VA_ARGS__)
#define DLOGI(format, ...) WLOGI_IF(kTagNone, format, ##__VA_ARGS__)
#define DLOGV(format, ...) WLOGV_IF(kTagNone, format, ##__VA_ARGS__)

namespace sdm {

class SdmDisplayDebugger : public DebugHandler {
 public:
  static inline SdmDisplayDebugger* Get() { return &debug_handler_; }
  void config_debug_level(void);

  // DebugHandler methods
  virtual void Error(DebugTag tag, const char *format, ...);
  virtual void Warning(DebugTag tag, const char *format, ...);
  virtual void Info(DebugTag tag, const char *format, ...);
  virtual void Debug(DebugTag tag, const char *format, ...);
  virtual void Verbose(DebugTag tag, const char *format, ...);
  virtual void BeginTrace(const char *class_name, const char *function_name,
                          const char *custom_string) { }
  virtual void EndTrace() { }
  virtual DisplayError GetProperty(const char *property_name, int *value);
  virtual DisplayError GetProperty(const char *property_name, char *value);
  virtual DisplayError SetProperty(const char *property_name, const char *value);

  int debug_level_ = INFO;

  SdmDisplayDebugger() {
    config_debug_level();
  }
 private:
  static SdmDisplayDebugger debug_handler_;
};

}  // namespace sdm

#endif  // __SDM_DISPLAY_DEBUGGER_H__
