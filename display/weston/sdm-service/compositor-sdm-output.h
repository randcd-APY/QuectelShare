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
#include "config.h"
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/vt.h>
#include <assert.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <time.h>

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <drm_fourcc.h>

#include <gbm.h>
#include <gbm_priv.h>
#include <libudev.h>

#include "shared/helpers.h"
#include "shared/timespec-util.h"
#include "libbacklight.h"
#include "compositor.h"
#include "gl-renderer.h"
#include "pixman-renderer.h"
#include "libinput-seat.h"
#include "launcher-util.h"
#include "vaapi-recorder.h"
#include "presentation_timing-server-protocol.h"
#include "linux-dmabuf.h"
#include "gbm-buffer-backend.h"

struct drm_backend {
       struct weston_backend base;
       struct weston_compositor *compositor;

       struct udev *udev;
       struct wl_event_source *drm_source;

       struct udev_monitor *udev_monitor;
       struct wl_event_source *udev_drm_source;

       struct {
                int id;
                int fd;
                char *filename;
       } drm;
       struct gbm_device *gbm;
       struct wl_listener session_listener;
       uint32_t format;
       int no_addfb2;
       int use_pixman;
       uint32_t prev_state;
       struct udev_input input;
       int32_t cursor_width;
       int32_t cursor_height;

       //TODO(user): these are not required. Need to remove
       uint32_t min_width, max_width;
       uint32_t min_height, max_height;
};

struct drm_edid {
       char eisa_id[13];
       char monitor_name[13];
       char pnp_id[5];
       char serial_number[13];
};

struct sdm_layer {
       struct wl_list link; /* drm_output::sdm_layer_list */
       struct weston_view *view;
       struct weston_buffer_reference buffer_ref;
       bool is_cursor;
       bool is_skip;
       struct gbm_bo *bo;
       uint32_t composition_type; /* type: enum SDM_COMPOSITION_XXXXX */
       pixman_region32_t overlap;
};

struct drm_output;

struct drm_fb {
       struct drm_output *output;
       uint32_t fb_id, stride, handle, size, ion_fd;
       int fd;
       int is_client_buffer;
       struct weston_buffer_reference buffer_ref;

       /* Used by gbm fbs */
       struct gbm_bo *bo;

       /* Used by dumb fbs */
       void *map;
};

struct drm_output {
       struct weston_output   base;

       uint32_t view_count;
       uint32_t crtc_id;
       int pipe;
       uint32_t connector_id;
       drmModeCrtcPtr original_crtc;
       struct drm_edid edid;
       drmModePropertyPtr dpms_prop;
       uint32_t format;

       enum dpms_enum dpms;

       int frame_pending;
       int page_flip_pending;
       int destroy_pending;

       struct gbm_surface *surface;
       struct gbm_bo *cursor_bo[2];
       struct weston_plane cursor_plane;
       struct weston_plane fb_plane;
        /* TODO(user):   Decide whether to use drm_plane or weston_plane */
        /* TODO(user):   struct drm_plane *primary_plane;                */
        /* TODO(user):   or struct weston_plane *primary_plane;          */

       struct weston_view *cursor_view;
       int current_cursor;
       struct drm_fb *current, *next;
       struct backlight *backlight;

       struct drm_fb *dumb[2];
       pixman_image_t *image[2];
       int current_image;
       pixman_region32_t previous_damage;

       struct vaapi_recorder *recorder;
       struct wl_listener recorder_frame_listener;

       struct wl_list plane_flip_list; /* drm_plane::flip_link */
       struct wl_list sdm_layer_list;  /* sdm_layer::link      */
       struct wl_list commited_layer_list;  /* sdm_layer::link */

       struct wl_event_source *finish_frame_timer;

       int vblank_ev_fd;
       struct wl_event_source *vblank_ev_source;
       struct {
           unsigned int frame;
           unsigned int sec;
           unsigned int usec;
       } last_vblank;
       // Indicate whether allocation of framebuffer is UBWC or not
       int framebuffer_ubwc;
};
