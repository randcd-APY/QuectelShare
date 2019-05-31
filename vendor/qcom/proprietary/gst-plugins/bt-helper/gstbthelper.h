/*
*Copyright (c) 2017 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/audio/audio.h>
#include "qahw_api.h"
#include "qahw_defs.h"

typedef struct _gstbt
{
  GstElement *pipeline;
  GstBuffer *buf;
  GstElement *appsrc, *qahwsink;
  GstFlowReturn ret;
  guint blocksize;
  GstMapInfo info;
  gchar bt_addr[256];
  gboolean is_compressed;
}gstbt;

void init_gst_pipeline(gstbt * ptr, audio_format_t fmt, int sample_rate, int channel_count, int flags, char * device_address);
int allocate_gst_buffer(gstbt * ptr, guint8** data);
void send_gst_data(gstbt * ptr,int size, int offset);
void close_gst_pipeline(gstbt * ptr);
void play_gst_ringtone(gstbt * ptr, char * ring_tone);

