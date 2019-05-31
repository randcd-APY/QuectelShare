/*
*Copyright (c) 2017 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "gstbthelper.h"

static bool
on_message (GstBus * bus, GstMessage * message, gpointer user_data)
{
    gstbt * ptr = (gstbt *)user_data;
    switch (message->type) {
        case GST_MESSAGE_EOS:
          close_gst_pipeline(ptr);
            break;
        case GST_MESSAGE_WARNING:{
            GError *gerror;
            gchar *debug;

            gst_message_parse_warning (message, &gerror, &debug);
            gst_object_default_error (GST_MESSAGE_SRC (message), gerror, debug);
            g_error_free (gerror);
            g_free (debug);
            break;
        }
        case GST_MESSAGE_ERROR:{
            GError *gerror;
            gchar *debug;

            close_gst_pipeline(ptr);

            gst_message_parse_error (message, &gerror, &debug);
            gst_object_default_error (GST_MESSAGE_SRC (message), gerror, debug);
            g_error_free (gerror);
            g_free (debug);

            break;
        }
        default:
          break;
    }
    return TRUE;
}

void init_gst_pipeline(gstbt * ptr, audio_format_t fmt, int sample_rate, int channel_count, int flags, char * device_address)
{
    GstElement *qahwsink, *appsrc;
    GstAudioInfo info;
    GstCaps *caps;
    GstBus *bus;

    if (!gst_is_initialized ())
        gst_init (NULL, NULL);

    if (!sample_rate || !channel_count) {
        return;
    }

    if (ptr->pipeline) {
        return;
    }

    ptr->pipeline = gst_pipeline_new (NULL);
    appsrc = gst_element_factory_make ("appsrc", "appsrc");
    qahwsink = gst_element_factory_make ("qahwsink", "qahwsink");

    if(ptr->is_compressed==true)
        g_object_set(qahwsink,"bt-addr",ptr->bt_addr,NULL);

    gst_bin_add_many (GST_BIN (ptr->pipeline), appsrc, qahwsink, NULL);
    gst_element_link (appsrc, qahwsink);

    bus = gst_pipeline_get_bus (GST_PIPELINE (ptr->pipeline));
    gst_bus_add_watch (bus, (GstBusFunc) on_message, ptr);
    gst_object_unref (bus);

    if(ptr->is_compressed==true) {
      caps = gst_caps_new_simple ("audio/x-wma",
      "channels", G_TYPE_INT, channel_count,
      "rate", G_TYPE_INT, sample_rate, NULL);
    }
    else{
       gst_audio_info_init (&info);
       gst_audio_info_set_format (&info, GST_AUDIO_FORMAT_S16LE, sample_rate,
          channel_count, NULL);
       caps = gst_audio_info_to_caps (&info);
    }
    qahwsink = gst_bin_get_by_name (GST_BIN (ptr->pipeline), "qahwsink");
    appsrc = gst_bin_get_by_name (GST_BIN (ptr->pipeline), "appsrc");

    g_object_set (qahwsink, "audio-handle", 0x07, "output-device", 2, "output-flags",
            flags, "device-address", device_address, NULL);
    gst_app_src_set_caps (GST_APP_SRC (appsrc), caps);

    gst_element_set_state (ptr->pipeline, GST_STATE_PLAYING);
    gst_object_unref (qahwsink);
    gst_object_unref (appsrc);
    gst_caps_unref (caps);
}

int allocate_gst_buffer(gstbt * ptr, guint8** data)
{
  if (!ptr->pipeline)
   return -1;

  ptr->appsrc = gst_bin_get_by_name (GST_BIN (ptr->pipeline), "appsrc");
  ptr->qahwsink = gst_bin_get_by_name (GST_BIN (ptr->pipeline), "qahwsink");

  if (!ptr->is_compressed)
    g_object_get (ptr->qahwsink, "blocksize", &ptr->blocksize, NULL);
  ptr->buf = gst_buffer_new_allocate (NULL, ptr->blocksize, NULL);

  gst_buffer_map (ptr->buf, &ptr->info, GST_MAP_WRITE);
  *data = ptr->info.data;
  return ptr->info.size;
}


void send_gst_data(gstbt * ptr, int size, int offset)
{
    ptr->info.size = size;
    if (offset > 0)
    ptr->info.data += offset;
    gst_buffer_unmap (ptr->buf, &ptr->info);
    gst_buffer_resize (ptr->buf,0,size);
    gst_buffer_ref (ptr->buf);
    gst_app_src_push_buffer(ptr->appsrc, ptr->buf);
    gst_buffer_unref (ptr->buf);

   /*  if(ptr->ret != GST_FLOW_OK)
       printf("Buffer push error %d \n", ptr->ret);
    else
       printf("Buffer push success %d\n", size);   */

   gst_object_unref (ptr->appsrc);
   gst_object_unref (ptr->qahwsink);
}

void close_gst_pipeline(gstbt * ptr)
{
    if (!ptr->pipeline)
        return;

    gst_element_set_state (ptr->pipeline, GST_STATE_NULL);
    gst_object_unref (ptr->pipeline);
    ptr->pipeline = NULL;
}

void play_gst_ringtone(gstbt * ptr, char * ring_tone)
{
   int i,j;
   if (!ptr->pipeline) {
    return;
  }

  ptr->appsrc = gst_bin_get_by_name (GST_BIN (ptr->pipeline), "appsrc");
  // 40msec of 8kz 16-bit mono = 40*8*2 = 640 bytes
  ptr->buf = gst_buffer_new_allocate (NULL, 640, NULL);

  for(i = 0; i < 5; i++) {
     for(j = 0; j < 5; j++) {
        gst_buffer_fill (ptr->buf, 0, (void*)(ring_tone + j * 640), 640);
        gst_buffer_ref (ptr->buf);
        g_signal_emit_by_name (ptr->appsrc, "push-buffer", ptr->buf, &ptr->ret);
     }
  }

  gst_object_unref (ptr->appsrc);
}
