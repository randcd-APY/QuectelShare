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


#include <assert.h>
#include <unistd.h>
#include <sys/types.h>

#include "compositor.h"
#include "gbm-buffer-backend.h"
#include "gbm-buffer-backend-server-protocol.h"

static void
gbm_buffer_destroy_params(struct wl_resource *params_resource);

static void
gbm_buffer_backend_create_params(struct wl_client *client,
               struct wl_resource *gbmbuf_resource,
               uint32_t params_id);

static void
gbm_buffer_backend_create_buffer(struct wl_client *client,
        struct wl_resource *params_resource,
        int32_t fd,
        int32_t metadata_fd,
        uint32_t width,
        uint32_t height,
        uint32_t format,
        uint32_t flags);

static void
gbm_buffer_backend_destroy(struct wl_client *client,
    struct wl_resource *resource);

static const struct gbm_buffer_params_interface gbm_buffer_params_implementation = {
    gbm_buffer_backend_destroy,
    gbm_buffer_backend_create_buffer
};

static void
gbm_buffer_destroy(struct gbm_buffer *buffer)
{
  // Destroy gbm bo if it is still valid
  if (buffer->bo) {
    gbm_bo_destroy(buffer->bo);
    buffer->bo = NULL;
  }

  free(buffer);
}


static void
gbm_wl_buffer_destroy(struct wl_client *client,
    struct wl_resource *resource)
{
    GBM_PROTOCOL_LOG(LOG_DBG,"gbm_wl_buffer_destroy::Invoked\n");

    wl_resource_destroy(resource);

    GBM_PROTOCOL_LOG(LOG_DBG,"gbm_wl_buffer_destroy::Exited\n");
}

static void
destroy_params(struct wl_resource *params_resource)
{
    struct gbm_buffer *buffer;

    buffer = wl_resource_get_user_data(params_resource);

    if (!buffer)
        return;

    gbm_buffer_destroy(buffer);
}

static const struct wl_buffer_interface gbm_buffer_implementation = {
    gbm_wl_buffer_destroy
};

static void
gbm_buffer_destroy_params(struct wl_resource *params_resource)
{
    struct gbm_buffer *buffer;

    buffer = wl_resource_get_user_data(params_resource);

    if (!buffer)
        return;

    gbm_buffer_destroy(buffer);
}

static void
gbm_buffer_backend_create_params(struct wl_client *client,
               struct wl_resource *gbmbuf_resource,
               uint32_t params_id)
{
    struct weston_compositor *compositor;
    struct gbm_buffer *buffer;
    uint32_t version;
    int i;

    version = wl_resource_get_version(gbmbuf_resource);
    compositor = wl_resource_get_user_data(gbmbuf_resource);

    buffer = zalloc(sizeof *buffer);
    if (!buffer)
        goto err_out;

    buffer->fd = -1;
    buffer->metadata_fd = -1;
    buffer->compositor = compositor;
    buffer->params_resource =
        wl_resource_create(client,
                   &gbm_buffer_params_interface,
                   version, params_id);
    if (!buffer->params_resource)
        goto err_dealloc;

    wl_resource_set_implementation(buffer->params_resource,
                       &gbm_buffer_params_implementation,
                       buffer, gbm_buffer_destroy_params);

    return;

err_dealloc:
    free(buffer);

err_out:
    wl_resource_post_no_memory(gbmbuf_resource);
}

WL_EXPORT struct gbm_buffer *
gbm_buffer_get(struct wl_resource *resource)
{
    struct gbm_buffer *buffer;

    GBM_PROTOCOL_LOG(LOG_DBG,"gbm_buffer_get::Invoked\n");

    if (!resource)
        return NULL;

    if (!wl_resource_instance_of(resource, &wl_buffer_interface,
                     &gbm_buffer_implementation))
        return NULL;

    buffer = wl_resource_get_user_data(resource);
    assert(buffer);
    assert(buffer->buffer_resource == resource);

    GBM_PROTOCOL_LOG(LOG_DBG,"gbm_buffer_get::Exited\n");

    return buffer;
}

WL_EXPORT int
gbm_buf_info_get(struct wl_resource *resource, struct gbm_buf_info *gbo_info)
{
    struct gbm_buffer *buffer = gbm_buffer_get(resource);

    if ((NULL != buffer) && (NULL != gbo_info))
    {
        gbo_info->fd = buffer->fd;
        gbo_info->metadata_fd = buffer->metadata_fd;
        gbo_info->width = buffer->width;
        gbo_info->height = buffer->height;
        gbo_info->format = buffer->format;
    }
    else
        return -1;

    return 0;
}

static void
destroy_gbm_buffer(struct wl_resource *resource)
{
    struct gbm_buffer *buffer;

    GBM_PROTOCOL_LOG(LOG_DBG,"destroy_gbm_buffer::Invoked\n");


    buffer = wl_resource_get_user_data(resource);
    assert(buffer->buffer_resource == resource);
    assert(!buffer->params_resource);

    if (buffer->user_data_destroy_func)
        buffer->user_data_destroy_func(buffer);

    gbm_buffer_destroy(buffer);

    GBM_PROTOCOL_LOG(LOG_DBG,"destroy_gbm_buffer::Exited\n");
}

static void
gbm_buffer_backend_create_buffer(struct wl_client *client,
        struct wl_resource *params_resource,
        int32_t fd,
        int32_t metadata_fd,
        uint32_t width,
        uint32_t height,
        uint32_t format,
        uint32_t flags)
{
    struct gbm_buffer *buffer;
    uint32_t version;
    int i;
    bool ret = true;


    GBM_PROTOCOL_LOG(LOG_DBG,"gbm_buffer_backend_create_buffer::Invoked\n");

    buffer = wl_resource_get_user_data(params_resource);

    if (!buffer) {
        weston_log("gbm_buffer_backend_create_buffer::buffer already used\n");
        close(fd);
        close(metadata_fd);
        return;
    }

    assert(buffer->params_resource == params_resource);
    assert(!buffer->buffer_resource);

    wl_resource_set_user_data(buffer->params_resource, NULL);
    buffer->params_resource = NULL;

    buffer->fd = fd;
    buffer->metadata_fd = metadata_fd;
    buffer->width  = width;
    buffer->height = height;
    buffer->format = format;
    buffer->flags  = flags;

    buffer->buffer_resource = wl_resource_create(client,
                    &wl_buffer_interface,
                    1, 0);
    if (!buffer->buffer_resource){
        wl_resource_post_no_memory(params_resource);
        goto err_buffer;
    }

    wl_resource_set_implementation(buffer->buffer_resource,
                &gbm_buffer_implementation,
                buffer, destroy_gbm_buffer);

    ret = weston_compositor_import_gbm_buffer(buffer->compositor, buffer);
    if (ret == false) {
      weston_log("gbm_buffer_backend_create_buffer:: import_gbm_buffer failed\n");
    }
    // gbm bo is imported to buffer from above function to use in below perform call
    unsigned int secure_status = 0;
    gbm_perform(GBM_PERFORM_GET_SECURE_BUFFER_STATUS, buffer->bo, &secure_status);
    // Override return value if format is part of skip list.
    if ((format == GBM_FORMAT_P010) ||
        ((format == GBM_FORMAT_NV12 ||
          format == GBM_FORMAT_YCbCr_420_TP10_UBWC) && secure_status)) {
      ret = true;
    }

    if (ret == false) {
      goto err_failed;
    }

    GBM_PROTOCOL_LOG(LOG_DBG,"gbm_buffer_backend_create_buffer::Exited- gracefully\n");

    gbm_buffer_params_send_created(params_resource,
                      buffer->buffer_resource);

    return;

err_buffer:
    if (buffer->user_data_destroy_func)
        buffer->user_data_destroy_func(buffer);

err_failed:
    gbm_buffer_params_send_failed(params_resource);
    gbm_buffer_destroy(buffer);

    GBM_PROTOCOL_LOG(LOG_DBG,"gbm_buffer_backend_create_buffer::Exited with Error\n");
}


static void
gbm_buffer_backend_destroy(struct wl_client *client,
    struct wl_resource *resource)
{

    GBM_PROTOCOL_LOG(LOG_DBG,"gbm_buffer_backend_destroy::Invoked\n");


    wl_resource_destroy(resource);

    GBM_PROTOCOL_LOG(LOG_DBG,"gbm_buffer_backend_destroy::Invoked\n");
}

static const struct gbm_buffer_backend_interface gbm_buffer_backend_implementation = {
    gbm_buffer_backend_destroy,
    gbm_buffer_backend_create_params
};

static void
bind_gbm_buffer_backend(struct wl_client *client,
          void *data, uint32_t version, uint32_t id)
{
    struct weston_compositor *compositor = data;
    struct wl_resource *resource;

    GBM_PROTOCOL_LOG(LOG_DBG,"bind_gbm_buffer_backend::Invoked\n");

    resource = wl_resource_create(client, &gbm_buffer_backend_interface,
                    version, id);
    if (resource == NULL) {
        wl_client_post_no_memory(client);
        return;
    }

    wl_resource_set_implementation(resource,
            &gbm_buffer_backend_implementation,
            compositor, NULL);

    GBM_PROTOCOL_LOG(LOG_DBG,"bind_gbm_buffer_backend::Exited\n");
}

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
WL_EXPORT int
gbm_buffer_backend_setup(struct weston_compositor *compositor)
{
    GBM_PROTOCOL_LOG(LOG_DBG,"gbm_buffer_backend_setup::Invoked\n");

    if (!wl_global_create(compositor->wl_display,
                &gbm_buffer_backend_interface, 1,
                compositor, bind_gbm_buffer_backend))
        return -1;
    GBM_PROTOCOL_LOG(LOG_DBG,"gbm_buffer_backend_setup::Exited\n");

    return 0;
}

/** Set renderer-private data
 *
 * Set the user data for the linux_dmabuf_buffer. It is invalid to overwrite
 * a non-NULL user data with a new non-NULL pointer. This is meant to
 * protect against renderers fighting over gbm_buffer user data
 * ownership.
 *
 * The renderer-private data is usually set from the
 * weston_renderer::import_gbm_buffer hook.
 *
 * \param buffer The gbm_buffer object to set for.
 * \param data The new renderer-private data pointer.
 * \param func Destructor function to be called for the renderer-private
 *             data when the gbm_buffer gets destroyed.
 *
 * \sa weston_compositor_import_gbm_buffer
 */
WL_EXPORT void
gbm_buffer_backend_set_user_data(struct gbm_buffer *buffer,
                  void *data,
                  gbm_buffer_user_data_destroy_func func)
{
    assert(data == NULL || buffer->user_data == NULL);

    buffer->user_data = data;
    buffer->user_data_destroy_func = func;
}

/** Get renderer-private data
 *
 * Get the user data from the gbm_buffer.
 *
 * \param buffer The gbm_buffer to query.
 * \return Renderer-private data pointer.
 *
 * \sa gbm_buffer_backend_get_user_data
 */
WL_EXPORT void *
gbm_buffer_backend_get_user_data(struct gbm_buffer *buffer)
{
    return buffer->user_data;
}

/** Resolve an internal compositor error by disconnecting the client.
 *
 * This function is used in cases when the gbmbuf-based wl_buffer
 * turns out unusable and there is no fallback path. This is used by
 * renderers which are the fallback path in the first place.
 *
 * It is possible the fault is caused by a compositor bug, the underlying
 * graphics stack bug or normal behaviour, or perhaps a client mistake.
 * In any case, the options are to either composite garbage or nothing,
 * or disconnect the client. This is a helper function for the latter.
 *
 * The error is sent as a INVALID_OBJECT error on the client's wl_display.
 *
 * \param buffer The gbm_buffer that is unusable.
 * \param msg A custom error message attached to the protocol error.
 */
WL_EXPORT void
gbm_buffer_send_server_error(struct gbm_buffer *buffer,
                      const char *msg)
{
    struct wl_client *client;
    struct wl_resource *display_resource;
    uint32_t id;

    assert(buffer->buffer_resource);
    id = wl_resource_get_id(buffer->buffer_resource);
    client = wl_resource_get_client(buffer->buffer_resource);
    display_resource = wl_client_get_object(client, 1);

    assert(display_resource);
    wl_resource_post_error(display_resource,
                   WL_DISPLAY_ERROR_INVALID_OBJECT,
                   "gbmbuf server error with "
                   "wl_buffer@%u: %s", id, msg);
}



