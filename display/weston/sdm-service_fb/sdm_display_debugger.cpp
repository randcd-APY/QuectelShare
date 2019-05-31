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
#include <stdio.h>
#include "sdm_display_debugger.h"
#include <cutils/properties.h>

#define __CLASS__ "SdmDisplayDebugger"

namespace sdm {

SdmDisplayDebugger SdmDisplayDebugger::debug_handler_;

static void Log(const char *prefix, const char *format, va_list list) {
  vprintf(format, list);
  printf("\n");
}

void SdmDisplayDebugger::config_debug_level(void) {
  FILE *fp = NULL;
  //file to configure debug level
  fp = fopen("/data/misc/display/sdm_dbg_cfg.txt", "r");
  if (fp) {
      fscanf(fp, "%d", &debug_level_);
      printf("\n sdm debug level configured to %d\n", debug_level_);
      fclose(fp);
  }
}

void SdmDisplayDebugger::Error(DebugTag tag, const char *format, ...) {
 // if (debug_level_ > NONE) {
      va_list list;
      va_start(list, format);
      Log("SDM_E: ", format, list);
      va_end(list);
//  }
}

void SdmDisplayDebugger::Warning(DebugTag tag, const char *format, ...) {
 // if (debug_level_ > ERROR) {
      va_list list;
      va_start(list, format);
      Log("SDM_W: ", format, list);
      va_end(list);
//  }
}

void SdmDisplayDebugger::Info(DebugTag tag, const char *format, ...) {
 // if (debug_level_ > WARNING) {
      va_list list;
      va_start(list, format);
      Log("SDM_I: ", format, list);
      va_end(list);
 // }
}

void SdmDisplayDebugger::Debug(DebugTag tag, const char *format, ...) {
//  if (debug_level_ > INFO) {
      va_list list;
      va_start(list, format);
      Log("SDM_D: ", format, list);
      va_end(list);
 // }
}

void SdmDisplayDebugger::Verbose(DebugTag tag, const char *format, ...) {
  //if (debug_level_ > DEBUG) {
      va_list list;
      va_start(list, format);
      Log("SDM_V: ", format, list);
      va_end(list);
  //}
}

DisplayError SdmDisplayDebugger::GetProperty(const char *property_name, int *value) {
  char property[PROPERTY_VALUE_MAX];

  if (debug_level_ > VERBOSE) {
      if (property_get(property_name, property, NULL) > 0) {
        *value = atoi(property);
        return kErrorNone;
      }
  }

  return kErrorNotSupported;
}

DisplayError SdmDisplayDebugger::GetProperty(const char *property_name, char *value) {
  if (debug_level_ > VERBOSE) {
      if (property_get(property_name, value, NULL) > 0) {
        return kErrorNone;
      }
  }

  return kErrorNotSupported;
}

DisplayError SdmDisplayDebugger::SetProperty(const char *property_name, const char *value) {
  if (debug_level_ > VERBOSE) {
      if (property_set(property_name, value) == 0) {
        return kErrorNone;
      }
  }

  return kErrorNotSupported;
}

}  // namespace sdm
