/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "gstmediarecorder.h"
#include <gst/app/gstappsink.h>
#include <stdio.h>

static GstFlowReturn
on_new_sample_from_sink (GstElement * elt, gpointer * data)
{
  GstSample *sample;
  GstBuffer *app_buffer, *buffer;
  GstElement *source;
  GstMapInfo info;

  MediaRecorder * ptr = (MediaRecorder *)data;
  /* get the sample from appsink */
  sample = gst_app_sink_pull_sample (GST_APP_SINK (elt));
  buffer = gst_sample_get_buffer (sample);
  gst_buffer_map (buffer, &info, GST_MAP_READ);
  ptr->callGetBufferEvent(info.data,info.size);
  gst_buffer_unmap (buffer, &info);
  gst_sample_unref (sample);
  return GST_FLOW_OK;
}

static gboolean handle_bus_msgs (GstBus * bus, GstMessage * msg, gpointer user_data)
{
  MediaRecorder * ptr = (MediaRecorder *)user_data;
  return ptr->handle_bus_msg (bus, msg);

}

static void *main_loop_thread(void *tptr)
{
  MediaRecorder * ptr = (MediaRecorder *)tptr;
  ptr->runMainLoop();
}

gboolean MediaRecorder::handle_bus_msg (GstBus * bus, GstMessage * msg)
{

  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_ERROR:{
      GError *err;
      gchar *dbg;

      /* dump graph on error */
      GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS (GST_BIN (m_pipeline),
            GST_DEBUG_GRAPH_SHOW_ALL, "gst-play.error");

      gst_message_parse_error (msg, &err, &dbg);
      if (dbg != NULL)
        g_printerr ("ERROR debug information: %s\n", dbg);
      g_clear_error (&err);
      g_free (dbg);

      /* flush any other error messages from the bus and clean up */
      gst_element_set_state (m_pipeline, GST_STATE_NULL);
      m_RecordFailedEvent(-1, m_streamid);
      break;
    }

    default:
      break;
  }

  return TRUE;
}
MediaRecorder::MediaRecorder(gint streamid)
{
  m_loop = NULL;
        m_source = NULL;
        m_sink = NULL;
        m_pipeline = NULL;
        m_streamid = streamid;
        m_RecordFailedEvent = NULL;
        m_GetBufferEvent = NULL;

}

MediaRecorder::~MediaRecorder()
{
}

gint MediaRecorder::InitRecorder(AudioProp * prop)
{
  GstElement * capsfilter;
  GstCaps *filtercaps;
  GstBus *bus;
  if(!gst_init_check(NULL,NULL,NULL))
    return -1;
  m_pipeline = gst_pipeline_new ("audio-recorder");
  m_source   = gst_element_factory_make ("qahwsrc","recorder-source");
  if(!m_source)
    return -1;
  g_object_set (G_OBJECT (m_source),
               "blocksize", prop->bufsize,
               "ffv-state", prop->ffv_state,
               "ffv-ec-ref-dev", prop->ffv_ec_ref_dev,
               "ffv-channel", prop->ffv_channel_index,
               NULL);

  capsfilter = gst_element_factory_make ("capsfilter", "filter");
  if(!capsfilter)
    return -1;
  m_sink = gst_element_factory_make ("appsink", "sink");
  if(!m_sink)
    return -1;
  m_loop = g_main_loop_new (NULL, FALSE);
  bus = gst_pipeline_get_bus (GST_PIPELINE (m_pipeline));
  m_busWatch = gst_bus_add_watch (bus, handle_bus_msgs, this);
  gst_object_unref (bus);
  g_object_set (G_OBJECT (m_sink), "emit-signals", TRUE, "sync", FALSE, NULL);
  g_signal_connect (m_sink, "new-sample",
        G_CALLBACK (on_new_sample_from_sink), this);
  gst_bin_add_many (GST_BIN (m_pipeline), m_source, capsfilter, m_sink, NULL);
  gst_element_link_many (m_source, capsfilter, m_sink, NULL);
  filtercaps = gst_caps_new_simple ("audio/x-raw",
            "format", G_TYPE_STRING, prop->format,
            "layout",  G_TYPE_STRING, "interleaved",
            "rate", G_TYPE_INT, prop->rate,
            "channels", G_TYPE_INT, prop->channels,
            NULL);
  g_object_set (G_OBJECT (capsfilter), "caps", filtercaps, NULL);
  gst_caps_unref (filtercaps);

  return 0;
}

gint MediaRecorder:: StartRecorder()
{
  pthread_t t_id;
  if(m_pipeline)
  {
    gst_element_set_state (m_pipeline, GST_STATE_PLAYING);
    if (gst_element_get_state (m_pipeline, NULL, NULL, -1) == GST_STATE_CHANGE_FAILURE) {
      g_error ("Failed to go into PLAYING state");
      return -1;
    }
    pthread_create(&t_id, NULL, main_loop_thread, this);
    return 0;
  }

  return -1;
}

gint MediaRecorder::StopRecorder()
{
  if(m_pipeline)
  {
    GstState state;
    GstState pending;
    GstStateChangeReturn stateChangeRet = gst_element_get_state(m_pipeline, &state, &pending, GST_CLOCK_TIME_NONE );
    if (GST_STATE_CHANGE_SUCCESS == stateChangeRet && GST_STATE_NULL == state) {
        printf("alreadyStopped \n");
    } else if (GST_STATE_CHANGE_ASYNC == stateChangeRet && GST_STATE_NULL == pending) {
        printf("alreadyStopping\n");
    } else {
        stateChangeRet = gst_element_set_state(m_pipeline, GST_STATE_NULL);
        if (GST_STATE_CHANGE_FAILURE == stateChangeRet) {
            m_pipeline = NULL;
            return -1;
        } else if (GST_STATE_CHANGE_ASYNC == stateChangeRet) {
            printf("doStopPending\n");
            return 0;
        } else {
        }

    }
    m_pipeline = NULL;
    return 0;
  }

  return -1;
}

void MediaRecorder::Clear()
{
   g_main_loop_quit(m_loop);
}
void MediaRecorder::registerRecordFailedEvent(RecordFailedEvent *eventFunc)
{
  m_RecordFailedEvent = eventFunc;
}
void MediaRecorder::registerGetBufferEvent(GetBufferEvent *eventFunc)
{
  m_GetBufferEvent = eventFunc;
}

void MediaRecorder::callGetBufferEvent(guint8 * data, int size)
{
  m_GetBufferEvent(data,size,m_streamid);
}
void MediaRecorder::runMainLoop()
{
  g_main_loop_run(m_loop);
  if(m_pipeline){
    gst_element_set_state (m_pipeline, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (m_pipeline));
    m_pipeline = NULL;
  }
}
