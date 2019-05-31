/*
*  Copyright (c) 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef __GST_QTIVDEC_H__
#define __GST_QTIVDEC_H__

#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/video/gstvideodecoder.h>
#include <gst/video/gstvideopool.h>
#include <gst/allocators/gstdmabuf.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <semaphore.h>
#include <linux/videodev2.h>
#include <gbm_priv.h>

#ifdef HAVE_LIBV4L2
#include <libv4l2.h>
#else
#define v4l2_fd_open(fd, flags) (fd)
#define v4l2_close    close
#define v4l2_dup      dup
#define v4l2_ioctl    ioctl
#define v4l2_read     read
#define v4l2_mmap     mmap
#define v4l2_munmap   munmap
#endif

/* Begin Declaration */
G_BEGIN_DECLS
#define GST_TYPE_QTIVDEC    (gst_qtivdec_get_type())
#define GST_QTIVDEC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_QTIVDEC,Gstqtivdec))
#define GST_QTIVDEC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_QTIVDEC,GstqtivdecClass))
#define GST_IS_QTIVDEC(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_QTIVDEC))
#define GST_IS_QTIVDEC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_QTIVDEC))

typedef struct _Gstqtivdec      Gstqtivdec;
typedef struct _GstqtivdecClass GstqtivdecClass;
typedef struct _GstqtivdecDownstreamMeta GstqtivdecDownstreamMeta;

#define MAX_BUFFERS  30

/* Maximum number of output buffers */
#define MAX_OUT_BUF  16

/* Maximum number of capture buffers (32 is the limit imposed by MFC */
#define MAX_CAP_BUF  32

/* Number of output planes */
#define OUT_PLANES  1

/* Number of capture planes */
#define CAP_PLANES  2

/* Maximum number of planes used in the application */
#define MAX_PLANES  CAP_PLANES

struct _Gstqtivdec
{
  GstVideoDecoder parent;

  gboolean silent;
  gboolean secure;
  gboolean decode_order;
  gboolean skip_frames;
  gboolean perf;
  gboolean zero_copy;
  gboolean operating_rate;
  void *handle;
  gint fd;
  void **input_buf_add;
  gint *input_buf_size;
  gint current_nb_buf_input;

  gint out_buf_flag[MAX_BUFFERS];
  gint out_buf_cnt;
  pthread_mutex_t lock;
  sem_t sem_done;
  sem_t sem_stop;
  /* State of an incoming or outgoing video stream for decoder. */
  GstVideoCodecState *input_state;
  GstVideoCodecState *output_state;

  gchar *format_in_to_str;
  gchar *format_out_to_str;

  GstCaps *caps_in;
  GstCaps *caps_out;

  gboolean output_setup;
  gboolean input_setup;

  gint width;
  gint height;
  __u32 streamformat;

  pthread_t poll_out_thread;

  GstBuffer *codec_data;
  GstBuffer *header;

  unsigned char sps_pps_buf[100];
  unsigned int sps_pps_size;

  GstVideoAlignment align;
  gint size_image;

  GstBuffer *downstream_buffers[MAX_BUFFERS];

  /* Output queue related */
  int out_buf_size;
  int out_buf_off[MAX_OUT_BUF];
  int out_ion_fd_secure[MAX_OUT_BUF];
  int out_ion_fd[MAX_OUT_BUF];
  void *out_ion_addr[MAX_OUT_BUF];
  /* Capture queue related */
  int cap_w;
  int cap_h;
  int cap_buf_cnt;
  int cap_buf_format;
  int cap_buf_fd[MAX_CAP_BUF];
  int cap_buf_meta_fd[MAX_CAP_BUF];
  void *cap_buf_addr[MAX_CAP_BUF];
  int cap_buf_size[CAP_PLANES][MAX_CAP_BUF];
  int cap_buf_stride[CAP_PLANES];
  int cap_buf_off[MAX_CAP_BUF][CAP_PLANES];
  int cap_buf_flag[MAX_CAP_BUF];
  int cap_ion_fd;
  void *cap_ion_addr;
  unsigned long total_captured;

  struct gbm_device *gbm;
  struct gbm_bo *cap_buf_bo[MAX_CAP_BUF];
  int fd_card0;
  int secure_ion_fd;
  int ion_fd;

  /* Extradata stuff */
  int extradata_index;
  int extradata_size;
  int extradata_ion_fd;
  void *extradata_ion_addr;
  int extradata_off[MAX_CAP_BUF];
  void *extradata_addr[MAX_CAP_BUF];

  struct ColorMetaData color_mdata;

  gint depth;
  gboolean reconfigure_pending;
  gboolean in_flush;
};

struct _GstqtivdecClass
{
  GstVideoDecoderClass parent_class;
};

struct _GstqtivdecDownstreamMeta
{
  GstMeta meta;
  gboolean acquired;
};

GType gst_qtivdec_get_type (void);
GType gst_qtivdec_downstream_meta_api_get_type (void);
const GstMetaInfo *gst_qtivdec_downstream_meta_get_info (void);
#define GST_QTIVDEC_DOWNSTREAM_META_GET(buf) ((GstqtivdecDownstreamMeta *)gst_buffer_get_meta(buf,gst_qtivdec_downstream_meta_api_get_type()))
#define GST_QTIVDEC_DOWNSTREAM_META_ADD(buf) ((GstqtivdecDownstreamMeta *)gst_buffer_add_meta(buf,gst_qtivdec_downstream_meta_get_info(),NULL))
G_END_DECLS
#endif /* __GST_QTIVDEC_H__ */
