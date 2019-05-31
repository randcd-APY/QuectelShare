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

#ifndef SDM_DISPLAY_INTERFACE_H
#define SDM_DISPLAY_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif
#include <color_metadata.h>

#define MAX_SDE_Layers          16
#define MAX_PIPE_WIDTH          2560
#define MAX_MIXER_WIDTH         2560

/* Buffer format enum */
enum {
       SDM_BUFFER_FORMAT_ARGB_8888,
       SDM_BUFFER_FORMAT_ABGR_8888,
       SDM_BUFFER_FORMAT_RGBA_8888,
       SDM_BUFFER_FORMAT_BGRA_8888,
       SDM_BUFFER_FORMAT_XRGB_8888,
       SDM_BUFFER_FORMAT_XBGR_8888,
       SDM_BUFFER_FORMAT_RGBX_8888,
       SDM_BUFFER_FORMAT_BGRX_8888,
       SDM_BUFFER_FORMAT_RGBA_5551,
       SDM_BUFFER_FORMAT_RGBA_4444,
       SDM_BUFFER_FORMAT_RGB_888,
       SDM_BUFFER_FORMAT_BGR_888,
       SDM_BUFFER_FORMAT_RGB_565,
       SDM_BUFFER_FORMAT_BGR_565,
       SDM_BUFFER_FORMAT_ABGR_2101010,
       SDM_BUFFER_FORMAT_RGBA_2101010,
       SDM_BUFFER_FORMAT_RGBA_8888_Ubwc,
       SDM_BUFFER_FORMAT_RGBX_8888_Ubwc,
       SDM_BUFFER_FORMAT_BGR565_Ubwc,
       SDM_BUFFER_FORMAT_YCbCr_420_P,
       SDM_BUFFER_FORMAT_YCrCb_420_P,
       SDM_BUFFER_FORMAT_YV12,
       SDM_BUFFER_FORMAT_YCbCr_420_SP,
       SDM_BUFFER_FORMAT_YCrCb_420_SP,
       SDM_BUFFER_FORMAT_NV12_ENCODEABLE,
       SDM_BUFFER_FORMAT_YCbCr_420_TP10_UBWC,
       SDM_BUFFER_FORMAT_YCbCr_420_P010_UBWC,
       SDM_BUFFER_FORMAT_P010,
       SDM_BUFFER_FORMAT_YCbCr_420_SP_VENUS,
       SDM_BUFFER_FORMAT_YCbCr_420_SP_VENUS_UBWC,
       SDM_BUFFER_FORMAT_YCbCr_422_SP,
       SDM_BUFFER_FORMAT_YCbCr_422_I,
       SDM_BUFFER_FORMAT_INVALID = 0xFFFFFFFF,
};



/*****************Prepare*****************/
/* Composition type for each layer*/
enum {
       SDM_COMPOSITION_GPU = 0,
       SDM_COMPOSITION_OVERLAY,
       SDM_COMPOSITION_HW_CURSOR,
       SDM_COMPOSITION_FB_TARGET

};

/* Blending type for each layer, now only support premultiplied blending */
enum {
       SDM_BLENDING_NONE = 0,
       SDM_BLENDING_PREMULTIPLIED,
       SDM_BLENDING_COVERAGE,
};

/* Rotation type*/
enum {
       SDM_TRANSFORM_NORMAL = 0x00,
       SDM_TRANSFORM_FLIP_H = 0x01,
       SDM_TRANSFORM_FLIP_V = 0x02,
       SDM_TRANSFORM_90 = 0x04,
       SDM_TRANSFORM_180 = 0x03,
       SDM_TRANSFORM_270 = 0x07,
};

struct Rect {
       float left;
       float top;
       float right;
       float bottom;
};

struct  RectArray {
       struct Rect *rects;
       uint32_t count;
};

/*
 * Layer flag which only be configurated by compositor. It will affect the SDM
 * strategy result.
 */
struct LayerGeometryFlags {
       uint32_t skip : 1;
       uint32_t is_cursor : 1;
       uint32_t has_ubwc_buf : 1;
       uint32_t video_present: 1;
       uint32_t secure_present: 1;
       uint32_t hdr_present: 1;
};


/* Layer geometry information filled by compositor */
/* TODO: Check if LayerGeometry from sdm layer could be re-used  */
struct LayerGeometry {
       /* Buffer information */
       uint32_t               width;
       uint32_t               height;
       uint32_t               unaligned_width;
       uint32_t               unaligned_height;
       uint32_t               format;
       uint32_t               fb_id;
       uint32_t               ion_fd;
       /* Layer information */
       uint32_t               composition; /*GPU, Overlay, HWCursor*/
       struct Rect            src_rect; /* srouce rectangle */
       struct Rect            dst_rect; /* destination rectangle */
       struct RectArray       visible_regions;
       struct RectArray       dirty_regions;
       uint32_t               blending;
       uint32_t               transform;
       uint8_t                plane_alpha; /* global alpha */
       struct LayerGeometryFlags        flags;
       ColorMetaData color_metadata;

       /*Hook for storing information relative to compositor. DO NOT MODIFY IT!!!*/
       const void *usr_data;
};

/* Scaling property */
struct ScalingInfo {
       struct Rect src;
       struct Rect dst;
       bool rotate_90;
};

/* Cursor position property */
struct CursorPosition {
       int x;
       int y;
};




struct DisplayConfigInfo {
  uint32_t x_pixels;          //!< Total number of pixels in X-direction on the display panel.
  uint32_t y_pixels;          //!< Total number of pixels in Y-direction on the display panel.
  float    x_dpi;             //!< Dots per inch in X-direction.
  float    y_dpi;             //!< Dots per inch in Y-direction.
  uint32_t fps;               //!< Frame rate per second.
  uint32_t vsync_period_ns;   //!< VSync period in nanoseconds.
  bool     is_yuv;            //!< If the display output is in YUV format.
};

typedef void (*vblank_cb_t)(unsigned int sequence, unsigned int tv_sec,
                           unsigned int tv_usec, struct drm_output *data);

typedef void (*hotplug_cb_t)(int disp, bool connected, struct drm_output *data);

typedef struct sdm_cbs {
  vblank_cb_t vblank_cb;
  hotplug_cb_t hotplug_cb;
} sdm_cbs_t;

#ifdef __cplusplus
}
#endif

#endif // SDM_DISPLAY_INTERFACE_H
