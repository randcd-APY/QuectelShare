/*
Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "gstionbuf_meta.h"

GType
gst_ionBufFd_meta_api_get_type (void)
{
  static volatile GType type;
  static const gchar *tags[] = { NULL };
  if (g_once_init_enter (&type)) {
    GType _type = gst_meta_api_type_register ("ION_BUF_FD_META_API", tags);
    g_once_init_leave (&type, _type);
  }
  return type;
}

/* initialize HEAD of list of GstMeta items */
static gboolean
ionBufFd_meta_init (GstMeta * meta, gpointer params, GstBuffer * buffer)
{
  GstIonBufFdMeta *newmeta = (GstIonBufFdMeta *) meta;
  newmeta->fd = 0;
  newmeta->meta_fd = 0;
  newmeta->width = 0;
  newmeta->height = 0;
  newmeta->format=0;

  return TRUE;
}

static void
ionBufFd_meta_free (GstMeta * meta, GstBuffer * buffer)
{
  return;
}

static gboolean
ionBufFd_meta_transform (GstBuffer * transbuf, GstMeta * meta,
    GstBuffer * buffer, GQuark type, gpointer data)
{
  GstIonBufFdMeta *newmeta = (GstIonBufFdMeta *) meta;
  gst_buffer_add_ionbuf_meta (transbuf, newmeta->fd, newmeta->meta_fd, newmeta->width, newmeta->height, newmeta->format);
  return TRUE;
}

const GstMetaInfo *
gst_ionBufFd_meta_get_info (void)
{
  static const GstMetaInfo *meta_info = NULL;

  if (g_once_init_enter (&meta_info)) {
    const GstMetaInfo *mi =
        gst_meta_register (ION_BUF_FD_META_API_TYPE, "ionBufFdMeta",
        sizeof (GstIonBufFdMeta), ionBufFd_meta_init, ionBufFd_meta_free,
        ionBufFd_meta_transform);
    g_once_init_leave (&meta_info, mi);
  }
  return meta_info;
}

GstIonBufFdMeta *
gst_buffer_add_ionbuf_meta (GstBuffer * buffer, int fd,
                  int meta_fd, unsigned int width, unsigned int height, unsigned int format)
{
  GstIonBufFdMeta *meta;
  g_return_val_if_fail (GST_IS_BUFFER (buffer), NULL);

  meta =
      (GstIonBufFdMeta *) gst_buffer_add_meta (buffer, ION_BUF_FD_META_INFO,
      NULL);
  meta->fd = fd;
  meta->meta_fd = meta_fd;
  meta->width = width;
  meta->height = height;
  meta->format = format;
  
  GST_DEBUG("Adding ionBufFd metadata fd %d, meta_fd %d, width %u, height %u, format %d", fd, meta_fd, width, height, format);

  return meta;
}
