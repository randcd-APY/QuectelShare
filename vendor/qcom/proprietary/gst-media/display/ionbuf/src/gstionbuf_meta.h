/*
Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef __GSTIONBUF_META_H__
#define __GSTIONBUF_META_H__

#include <gst/gst.h>
#include <gst/video/gstvideometa.h>

G_BEGIN_DECLS

struct _GstIonBufFdMeta {
  GstMeta meta;

  int fd;
  int meta_fd;
  unsigned int width;
  unsigned int height;
  unsigned int format;
};

typedef struct _GstIonBufFdMeta GstIonBufFdMeta;

GType gst_ionBufFd_meta_api_get_type (void);
const GstMetaInfo * gst_ionBufFd_meta_get_info (void);

#define ION_BUF_FD_META_API_TYPE (gst_ionBufFd_meta_api_get_type())
#define gst_buffer_get_ionfd_meta(b)		\
  ((GstIonBufFdMeta*)gst_buffer_get_meta((b),ION_BUF_FD_META_API_TYPE))
#define ION_BUF_FD_META_INFO (gst_ionBufFd_meta_get_info ())

GstIonBufFdMeta *
gst_buffer_add_ionbuf_meta(GstBuffer * buffer, int fd, int meta_fd, unsigned int width, unsigned int height, unsigned int format);

G_END_DECLS

#endif /* __GSTIONBUF_META_H__ */
