/*
*Copyright (c) 2017 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gst/gst.h>

#include <stdlib.h>

#define FIRST_CAPS "audio/x-raw,format=S16LE,channels=1,rate=44100"
#define SECOND_CAPS "audio/x-raw,format=S16LE,channels=1,rate=48000"

gboolean caps_changed = FALSE;
GMainLoop *main_loop = NULL;

static gboolean
on_message (GstBus * bus, GstMessage * message, GMainLoop * loop)
{
  switch (message->type) {
    case GST_MESSAGE_EOS:
      g_main_loop_quit (loop);
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

      gst_message_parse_error (message, &gerror, &debug);
      gst_object_default_error (GST_MESSAGE_SRC (message), gerror, debug);
      g_error_free (gerror);
      g_free (debug);
      g_main_loop_quit (loop);
      break;
    }
    default:
      break;
  }
  return TRUE;
}

static gboolean
change_sample_rate (GstElement * pipeline)
{
  GstElement *cf;
  GstCaps *caps;

  if (caps_changed) {
    g_print ("Done\n");
    g_main_loop_quit (main_loop);
    return G_SOURCE_REMOVE;
  }

  cf = gst_bin_get_by_name (GST_BIN (pipeline), "cf");
  if (!cf) {
    g_print ("Failed to find capsfilter\n");
    return FALSE;
  }

  g_print ("Changing sample rate\n");
  caps = gst_caps_from_string (SECOND_CAPS);
  g_object_set (cf, "caps", caps, NULL);
  gst_caps_unref (caps);

  gst_object_unref (cf);
  caps_changed = TRUE;

  return G_SOURCE_CONTINUE;
}

static void
sink_caps_changed_cb (GObject * pad, GParamSpec * pspec, gpointer user_data)
{
  GstCaps *caps;

  g_object_get (pad, "caps", &caps, NULL);

  if (caps) {
    gchar *str = gst_caps_to_string (caps);
    g_print ("Format changed: %s\n", str);
    g_free (str);
    gst_caps_unref (caps);
  }
}

int
main (int argc, char *argv[])
{
  GstElement *pipeline;
  gchar *descr;
  GError *error = NULL;
  GstStateChangeReturn ret;
  GstBus *bus;
  GstElement *sink;
  GstPad *sinkpad;

  gst_init (&argc, &argv);

  /* create a new pipeline */
  descr =
      g_strdup_printf ("qahwsrc num-buffers=500 ! capsfilter name=cf caps="
      FIRST_CAPS " ! fakesink name=sink ");
  pipeline = gst_parse_launch (descr, &error);

  if (error != NULL) {
    g_print ("could not construct pipeline: %s\n", error->message);
    g_error_free (error);
    exit (-1);
  }

  sink = gst_bin_get_by_name (GST_BIN (pipeline), "sink");
  sinkpad = gst_element_get_static_pad (sink, "sink");
  g_signal_connect (sinkpad, "notify::caps", G_CALLBACK (sink_caps_changed_cb),
      NULL);
  gst_object_unref (sinkpad);
  gst_object_unref (sink);

  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  switch (ret) {
    case GST_STATE_CHANGE_FAILURE:
      g_print ("failed to play the file\n");
      exit (-1);
    case GST_STATE_CHANGE_NO_PREROLL:
      /* for live sources, we need to set the pipeline to PLAYING before we can
       * receive a buffer. We don't do that yet */
      g_print ("live sources not supported yet\n");
      exit (-1);
    default:
      break;
  }
  ret = gst_element_get_state (pipeline, NULL, NULL, 5 * GST_SECOND);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_print ("failed to start playing\n");
    exit (-1);
  }

  /* setup a timer and start listening to events */
  main_loop = g_main_loop_new (NULL, FALSE);
  bus = gst_element_get_bus (pipeline);
  gst_bus_add_watch (bus, (GstBusFunc) on_message, main_loop);
  gst_object_unref (bus);
  g_timeout_add (2000, (GSourceFunc) change_sample_rate, pipeline);
  g_main_loop_run (main_loop);

  /* cleanup and exit */
  g_main_loop_unref (main_loop);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);

  return 0;
}
