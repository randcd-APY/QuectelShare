/* Copyright (c) 2015-2017 The Linux Foundataion. All rights reserved.
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
 *
 */

#ifndef __CAMERA_LOG_H__
#define __CAMERA_LOG_H__

#define TAG "LIBCAM"
#include <stdio.h>
#include <android/log.h>

#define CAM_ERR(fmt, args...) do { \
  __android_log_print(ANDROID_LOG_ERROR,TAG,fmt, ##args); \
} while (0)

#define CAM_INFO(fmt, args...) do { \
   __android_log_print(ANDROID_LOG_INFO,TAG, fmt, ##args);  \
} while (0)


#if 1
#define CAM_DBG(fmt, args...) do { \
} while (0)
#else
#define CAM_DBG(fmt, args...) do { \
	__android_log_print(ANDROID_LOG_DEBUG,TAG, fmt, ##args); \
} while (0)
#endif 

#define CAM_PRINT(fmt, args...) do { \
  printf(fmt "\n", ##args); \
  __android_log_print(ANDROID_LOG_ERROR,TAG,fmt, ##args); \
} while (0)

#define LIBCAM_LOG_LEVEL_KPI

#ifdef LIBCAM_LOG_LEVEL_KPI
#include <cutils/properties.h>
#include <cutils/trace.h>

#define LIBCAM_KPI_DISABLE 0
#define LIBCAM_KPI_ONLY 1
extern uint32_t libcam_kpi_debug_mask;
extern uint32_t libcam_kpi_round_frame_cnt;

#define LIBCAM_KPI_GET_MASK() do {\
char prop[PROPERTY_VALUE_MAX];\
property_get("persist.libcam.kpi.debug", prop, "0"); \
libcam_kpi_debug_mask = atoi (prop); \
property_get("persist.libcam.kpi.frame.count", prop, "1800"); \
libcam_kpi_round_frame_cnt = atoi (prop); \
} while (0)

#define LIBCAM_KPI_BEGIN(name) do {\
if (libcam_kpi_debug_mask & LIBCAM_KPI_ONLY) { \
     atrace_begin(ATRACE_TAG_ALWAYS, name); \
}\
} while(0)

#define LIBCAM_KPI_END() do {\
if (libcam_kpi_debug_mask & LIBCAM_KPI_ONLY) { \
     atrace_end(ATRACE_TAG_ALWAYS); \
}\
} while (0)

#define LIBCAM_KPI_ASYNC_BEGIN(name, cookie) do {\
if (libcam_kpi_debug_mask & LIBCAM_KPI_ONLY) { \
     atrace_async_begin(ATRACE_TAG_ALWAYS, name, cookie); \
}\
} while (0)

#define LIBCAM_KPI_ASYNC_END(name, cookie) do {\
if (libcam_kpi_debug_mask & LIBCAM_KPI_ONLY) { \
     atrace_async_end(ATRACE_TAG_ALWAYS, name, cookie); \
}\
} while (0)

#define LIBCAM_KPI_DUMP_FRAME_TS(frame_cnt, str) do { \
if (libcam_kpi_debug_mask & LIBCAM_KPI_ONLY) { \
    ++frame_cnt; \
    if (frame_cnt >= libcam_kpi_round_frame_cnt) frame_cnt = 1; \
    if (frame_cnt >= 1 && frame_cnt <= 5) \
        LIBCAM_KPI_BEGIN(str); \
} \
} while (0)

#else
#define LIBCAM_KPI_GET_MASK() do {} while (0)
#define LIBCAM_KPI_BEGIN(name) do {} while (0)
#define LIBCAM_KPI_END() do {} while (0)
#define LIBCAM_KPI_ASYNC_BEGIN(name, cookie) do {} while (0)
#define LIBCAM_KPI_ASYNC_END(name, cookie) do {} while (0)
#define LIBCAM_KPI_DUMP_FRAME_TS(frame_cnt, str) do {} while(0)
#endif
#endif
