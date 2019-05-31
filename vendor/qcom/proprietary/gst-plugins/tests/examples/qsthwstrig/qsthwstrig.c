/*
*Copyright (c) 2017 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gst/gst.h>
#include <glib-unix.h>

#define DEFAULT_MODEL_LOCATION     "/data/HeySnapdragon.uim"
#define DEFAULT_FILE_LOCATION      "/data/capture.wav"
#define DEFAULT_DURATION           5

typedef struct
{
  GMainLoop *loop;
  GstElement *pipeline;
  guint signal_watch_id;
  gint duration;
} TestData;

static TestData *
test_data_new (gint duration)
{
  TestData *data = g_new0 (TestData, 1);
  data->loop = g_main_loop_new (NULL, FALSE);
  data->duration = duration;
  return data;
}

static void
test_data_free (TestData * data)
{
  g_main_loop_unref (data->loop);
#ifdef G_OS_UNIX
  if (data->signal_watch_id > 0)
    g_source_remove (data->signal_watch_id);
#endif
  g_free (data);
}

static gboolean
send_eos (GstElement * pipeline)
{
  g_print ("send eos event to the pipeline\n");
  gst_element_send_event (pipeline, gst_event_new_eos ());

  return G_SOURCE_REMOVE;
}

static gboolean
on_message (GstBus * bus, GstMessage * message, TestData * data)
{
  GMainLoop *loop = data->loop;
  GstElement *pipeline = data->pipeline;

  switch (message->type) {
    case GST_MESSAGE_ELEMENT:
    {
      const GstStructure *mstructure = gst_message_get_structure (message);
      if (gst_structure_has_name (mstructure, "GstQsthwSoundTrigger")) {
        g_timeout_add_seconds (data->duration, (GSourceFunc) send_eos,
            pipeline);
        g_print ("got recognition triggered message\n");
      }
      break;
    }
    case GST_MESSAGE_EOS:
      g_print ("got eos message, pipeline will be restarted\n");
      gst_element_set_state (pipeline, GST_STATE_READY);
      gst_element_get_state (pipeline, NULL, NULL, GST_CLOCK_TIME_NONE);
      gst_element_set_state (pipeline, GST_STATE_PLAYING);
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

#if defined(G_OS_UNIX)
static gboolean
intr_handler (gpointer user_data)
{
  TestData *data = (TestData *) user_data;
  GMainLoop *loop = data->loop;
  g_main_loop_quit (loop);
  data->signal_watch_id = 0;
  return FALSE;
}
#endif

int
main (int argc, char *argv[])
{
  GOptionContext *context;
  gchar *desc;
  GError *error = NULL;
  GstBus *bus;
  TestData *data;
  const gchar *model_location = DEFAULT_MODEL_LOCATION;
  const gchar *file_location = DEFAULT_FILE_LOCATION;
  guint duration = DEFAULT_DURATION;
  GOptionEntry entries[] = {
    {"model-location", 'm', 0, G_OPTION_ARG_STRING, &model_location,
        "Model file location", NULL},
    {"location", 'o', 0, G_OPTION_ARG_STRING, &file_location,
        "Location of the file to write", NULL},
    {"duration", 'd', 0, G_OPTION_ARG_INT, &duration,
        "Duration of recording file", NULL},
    {NULL}
  };

  data = test_data_new (duration);

  context = g_option_context_new ("-m model-location -o file-location "
      "-d duration");
  g_option_context_add_main_entries (context, entries, NULL);
  g_option_context_add_group (context, gst_init_get_option_group ());
  if (!g_option_context_parse (context, &argc, &argv, &error)) {
    g_print ("option parsing failed: %s\n", error->message);
    return -1;
  }
  if (argc > 1) {
    g_print ("%s", g_option_context_get_help (context, TRUE, NULL));
    exit (0);
  }
  g_option_context_free (context);

  gst_init (&argc, &argv);

  desc = g_strdup_printf ("qsthwstrig model-location=%s lab=1 ! "
      "wavenc ! filesink name=filesink location=%s", model_location,
      file_location);

  data->pipeline = gst_parse_launch (desc, &error);
  g_free (desc);

  if (error) {
    g_print ("pipeline could not be constructed: %s\n", error->message);
    g_error_free (error);
    return 1;
  }

  data->loop = g_main_loop_new (NULL, FALSE);
  bus = gst_pipeline_get_bus (GST_PIPELINE (data->pipeline));
  gst_bus_add_watch (bus, (GstBusFunc) on_message, data);
  gst_object_unref (bus);

#ifdef G_OS_UNIX
  data->signal_watch_id =
      g_unix_signal_add (SIGINT, (GSourceFunc) intr_handler, data);
#endif

  gst_element_set_state (data->pipeline, GST_STATE_PLAYING);
  g_main_loop_run (data->loop);

  /* cleanup and exit */
  gst_element_set_state (data->pipeline, GST_STATE_NULL);
  gst_object_unref (data->pipeline);

  test_data_free (data);

  return 0;
}
