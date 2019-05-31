/*
*    Copyright (c) 2017, The Linux Foundation. All rights reserved.
*
*    Redistribution and use in source and binary forms, with or without
*    modification, are permitted provided that the following conditions are
*    met:
*    * Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*    * Redistributions in binary form must reproduce the above
*    copyright notice, this list of conditions and the following
*    disclaimer in the documentation and/or other materials provided
*    with the distribution.
*    * Neither the name of The Linux Foundation nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.

*    THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
*    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
*    ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
*    BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
*    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
*    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
*    OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
*    IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*
*    Copyright © 2014, 2015 Collabora, Ltd.
*
*    Permission to use, copy, modify, distribute, and sell this
*    software and its documentation for any purpose is hereby granted
*    without fee, provided that the above copyright notice appear in
*    all copies and that both that copyright notice and this permission
*    notice appear in supporting documentation, and that the name of
*    the copyright holders not be used in advertising or publicity
*    pertaining to distribution of the software without specific,
*    written prior permission.  The copyright holders make no
*    representations about the suitability of this software for any
*    purpose.  It is provided "as is" without express or implied
*    warranty.
*
*    THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
*    SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
*    FITNESS, IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
*    SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
*    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
*    AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
*    ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
*    THIS SOFTWARE.
*/

#ifndef WESTON_GBM_BUFFER_BACKEND_H
#define WESTON_GBM_BUFFER_BACKEND_H
#ifdef __cplusplus
extern "C" {
#endif

#include "gbm_priv.h"

#define GBM_BUFFER_BACKEND_PROTOCOL

#define MAX_NUM_PLANES (3)

#define LOG_FATAL    (0)
#define LOG_ERR      (1)
#define LOG_WARN     (2)
#define LOG_INFO     (3)
#define LOG_DBG      (4)
#define MAX_DBG_LEVEL  LOG_INFO

extern int g_prtcl_debug_level;


#define GBM_PROTOCOL_LOG(level, ...) do {  \
                            if (level <= MAX_DBG_LEVEL) { \
                                if(level==LOG_FATAL) \
                                  fprintf(stderr,"%s","GBM_FATAL::"); \
                                if(level==LOG_ERR) \
                                  fprintf(stderr,"%s", "GBM_ERR::"); \
                                if(level==LOG_WARN) \
                                  fprintf(stderr,"%s", "GBM_WARN::"); \
                                if(level==LOG_INFO) \
                                  fprintf(stderr,"%s", "GBM_INFO::"); \
                                if(level==LOG_DBG) \
                                  fprintf(stderr,"%s", "GBM_DBG::"); \
                                fprintf(stderr,"%s(%d)::",__func__,__LINE__); \
                                fprintf(stderr, __VA_ARGS__); \
                                fprintf(stderr, "\n"); \
                                fflush(stderr); \
                            } \
                        } while (0)


struct gbm_buffer;
typedef void (*gbm_buffer_user_data_destroy_func)(
                             struct gbm_buffer *buffer);
struct gbm_buffer {
    struct wl_resource *buffer_resource;
    struct wl_resource *params_resource;
    struct weston_compositor *compositor;
    int32_t fd;
    int32_t metadata_fd;
    uint32_t width;
    uint32_t height;
    uint32_t format;
    uint32_t flags;
    uint32_t offset[MAX_NUM_PLANES];
    uint32_t stride[MAX_NUM_PLANES];
    int32_t  num_planes;
    void *user_data;
    struct gbm_bo *bo;
    gbm_buffer_user_data_destroy_func user_data_destroy_func;
};

/** Advertise gbm_buffer_backend support
 *
 * Calling this initializes the gbm_buffer_backend protocol support, so that
 * the interface will be advertised to clients. Essentially it creates a
 * global. Do not call this function multiple times in the compositor's
 * lifetime. There is no way to deinit explicitly, globals will be reaped
 * when the wl_display gets destroyed.
 *
 * \param compositor The compositor to init for.
 * \return Zero on success, -1 on failure.
 */
int gbm_buffer_backend_setup(struct weston_compositor *compositor);

/** Get the gbm_buffer from a wl_buffer resource
 *
 * If the given wl_buffer resource was created through the gdb_buffer_backend
 * protocol interface, returns the gbm_buffer object. This can be used as a
 * type check for a wl_buffer.
 *
 * \param resource A wl_buffer resource.
 * \return The gbm_buffer if it exists, or NULL otherwise.
 */
struct gbm_buffer *gbm_buffer_get(struct wl_resource *resource);

/** Populate the gbm_buf_info from a wl_buffer resource
 *
 * If the given wl_buffer resource was created through the gdb_buffer_backend
 * protocol interface, populates the gbm_buffer_info.
 * This wraps the gbm_buffer_get and protects the gbm_buffer object against
 * unwanted exposure. Also it can be used as a type check for a wl_buffer.
 *
 * \param resource A wl_buffer resource.
 * \param gbo_info The gbm_buf_info to populate.
 * \return Zero on success, -1 on failure.
 */
int gbm_buf_info_get(struct wl_resource *resource, struct gbm_buf_info *gbo_info);

#ifdef __cplusplus
}
#endif

#endif /* WESTON_GBM_BUFFER_BACKEND_H */

