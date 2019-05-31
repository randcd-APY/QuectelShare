/*
*Copyright (c) 2017 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gst/gst.h>
#include <gst/audio/audio.h>
#include <glib-unix.h>
#include <mm-audio/qahw_api/inc/qahw_api.h>
#include <mm-audio/qahw_api/inc/qahw_defs.h>

#define DEFAULT_SOURCE_TRACK       FALSE
#define SET_REQUEST                "audio-effect-set"
#define GET_STR                    "audio-effect-get"
#define SOUNDFOCUS_GAIN_STEP       "SoundFocus.gain_step"
#define SOUNDFOCUS_START_ANGLES    "SoundFocus.start_angles"
#define SOUNDFOCUS_ENABLE_SECTORS  "SoundFocus.enable_sectors"
#define SOUNDFOCUS_PARAMS          SOUNDFOCUS_START_ANGLES ";" SOUNDFOCUS_ENABLE_SECTORS ";" \
                                   SOUNDFOCUS_GAIN_STEP

#define SOURCETRACK_VAD            "SourceTrack.vad"
#define SOURCETRACK_DOA_SPEECH     "SourceTrack.doa_speech"
#define SOURCETRACK_DOA_NOISE      "SourceTrack.doa_noise"
#define SOURCETRACK_POLAR_ACTIVITY "SourceTrack.polar_activity"
#define SSR_NOISE_LEVEL            "ssr.noise_level"
#define SSR_NOISE_LEVEL_AFTER_NS   "ssr.noise_level_after_ns"
#define SOURCETRACK_PARAMS         SOURCETRACK_VAD ";" SOURCETRACK_DOA_SPEECH ";" \
                                   SOURCETRACK_DOA_NOISE ";" SOURCETRACK_POLAR_ACTIVITY ";" \
                                   SSR_NOISE_LEVEL ";" SSR_NOISE_LEVEL_AFTER_NS

#define MAGIC_HANDLE               0x999
#define DEFAULT_MODULE_ID          "audio.primary"

static gboolean source_track_timer (GstElement * qahwsrc);

typedef struct
{
  GMainLoop *loop;
  GstElement *pipeline;
  gint audio_handle;
  const gchar *module_id;
  gboolean is_multiple_source;
#ifdef G_OS_UNIX
  guint signal_watch_id;
#endif
} TestData;

static TestData *
test_data_new (void)
{
  TestData *data = g_new0 (TestData, 1);
  data->loop = g_main_loop_new (NULL, FALSE);
  data->audio_handle = MAGIC_HANDLE;
  data->module_id = DEFAULT_MODULE_ID;
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
on_message (GstBus * bus, GstMessage * message, TestData * data)
{
  GMainLoop *loop = data->loop;

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

static GstAudioFormat
parse_audio_format (gint in_format)
{
  GstAudioFormat format;
  switch (in_format) {
    case 16:
      format = GST_AUDIO_FORMAT_S16LE;
      break;
    case 24:
      format = GST_AUDIO_FORMAT_S24LE;
      break;
    default:
      g_warning ("Invalid audio format: %d", in_format);
      format = GST_AUDIO_FORMAT_UNKNOWN;
      break;
  }

  return format;
}

static gboolean
eos_timer (GstElement * src)
{
  g_message ("Source(%p) is finished", src);
  gst_element_send_event (src, gst_event_new_eos ());
  return G_SOURCE_REMOVE;
}

static GstElement *
parse_source (gchar * source, TestData * data, gboolean source_track,
    GError ** error)
{
  GstElement *src, *qahwsrc;
  GStrv tmp;
  gint channels, format, samplerate, duration;
  gchar *device, *audio_source, *location, *stream_profile;
  GstAudioInfo info;
  GstCaps *caps;
  gchar *caps_str;
  gchar *desc;

  tmp = g_strsplit (source, ",", -1);
  if (g_strv_length (tmp) != (data->is_multiple_source ? 8 : 7))
    goto input_error;

  device = tmp[0];
  channels = atoi (tmp[1]);
  format = atoi (tmp[2]);
  samplerate = atoi (tmp[3]);
  audio_source = tmp[4];
  duration = atoi (tmp[5]);
  location = tmp[6];

  gst_audio_info_init (&info);
  gst_audio_info_set_format (&info, parse_audio_format (format), samplerate,
      channels, NULL);
  caps = gst_audio_info_to_caps (&info);
  caps_str = gst_caps_to_string (caps);

  desc = g_strdup_printf ("qahwsrc name=qahwsrc module-id=%s input-device=%s "
      "audio-source=%s audio-handle=%d ! capsfilter caps=\"%s\" ! "
      "filesink name=filesink location=%s", data->module_id, device,
      audio_source, data->audio_handle, caps_str, location);
  data->audio_handle--;

  src = gst_parse_bin_from_description (desc, FALSE, error);
  if (!src)
    goto parse_desc_error;

  qahwsrc = gst_bin_get_by_name (GST_BIN (src), "qahwsrc");

  if (data->is_multiple_source) {
    GstElement *filesink;

    filesink = gst_bin_get_by_name (GST_BIN (src), "filesink");
    stream_profile = tmp[7];
    g_object_set (qahwsrc, "stream-profile", stream_profile, NULL);
    g_object_set (filesink, "async", FALSE, NULL);

    gst_object_unref (filesink);
  }

  if (source_track)
    g_timeout_add_full (G_PRIORITY_DEFAULT, 1000,
        (GSourceFunc) source_track_timer,
        g_object_ref (qahwsrc), (GDestroyNotify) gst_object_unref);
  g_timeout_add_seconds_full (G_PRIORITY_DEFAULT, duration,
      (GSourceFunc) eos_timer, g_object_ref (qahwsrc),
      (GDestroyNotify) gst_object_unref);
  gst_object_unref (qahwsrc);

  g_message
      ("Source(%p) created\n input-device:%s, audio-source:%s, duration:%d, "
      "location:%s\n caps:[%s]", qahwsrc, tmp[0],
      audio_source, duration, location, caps_str);
  g_free (caps_str);
  gst_caps_unref (caps);
  g_strfreev (tmp);

  return src;

input_error:
  g_set_error (error, GST_RESOURCE_ERROR, GST_RESOURCE_ERROR_FAILED,
      "Failed to parse source: %s\n", source);
  g_strfreev (tmp);
  return NULL;

parse_desc_error:
  g_free (caps_str);
  gst_caps_unref (caps);
  g_strfreev (tmp);
  return NULL;
}

static gboolean
source_track_timer (GstElement * qahwsrc)
{
  gchar *str = NULL;
  GStrv tmp;
  gint i, n_start_angles = 0, n_enable_sectors = 0, n_vad = 0, n_doa_noise = 0;

  g_object_set(qahwsrc, SET_REQUEST, SOUNDFOCUS_PARAMS, NULL);
  g_object_get(qahwsrc, GET_STR, &str, NULL);
  if (str == NULL) {
    g_warning ("Failed to get SoundFocus params");
    return G_SOURCE_CONTINUE;
  }

  g_message ("Source track timer called");

  tmp = g_strsplit_set (str, ",;", -1);
  for (i = 0; tmp[i]; i++) {
    gchar *target;
    if (!(target = strchr (tmp[i], '=')) || *(++target) == '\0')
      continue;

    if (g_str_has_prefix (tmp[i], SOUNDFOCUS_GAIN_STEP)) {
      g_message ("%s=%s", SOUNDFOCUS_GAIN_STEP, target);
    } else if (g_str_has_prefix (tmp[i], SOUNDFOCUS_START_ANGLES)) {
      g_message ("%s[%d]=%s", SOUNDFOCUS_START_ANGLES, n_start_angles, target);
      n_start_angles++;
    } else if (g_str_has_prefix (tmp[i], SOUNDFOCUS_ENABLE_SECTORS)) {
      g_message ("%s[%d]=%s", SOUNDFOCUS_ENABLE_SECTORS, n_enable_sectors,
          target);
      n_enable_sectors++;
    }
  }
  g_strfreev (tmp);
  g_free (str);

  str = NULL;

  g_object_set(qahwsrc, SET_REQUEST, SOURCETRACK_PARAMS, NULL);
  g_object_get(qahwsrc, GET_STR, &str, NULL);
  if (str == NULL) {
    g_warning ("Failed to get SoundFocus params\n");
    return G_SOURCE_CONTINUE;
  }

  tmp = g_strsplit_set (str, ",;", -1);
  for (i = 0; tmp[i]; i++) {
    gchar *target;
    if (!(target = strchr (tmp[i], '=')) || (*(++target) != '\0'))
      continue;

    if (g_str_has_prefix (tmp[i], SOURCETRACK_POLAR_ACTIVITY)) {
      g_message ("%s=%s", SOURCETRACK_POLAR_ACTIVITY, target);
    } else if (g_str_has_prefix (tmp[i], SOURCETRACK_VAD)) {
      g_message ("%s[%d]=%s", SOURCETRACK_VAD, n_vad, target);
      n_vad++;
    } else if (g_str_has_prefix (tmp[i], SOURCETRACK_DOA_SPEECH))
      g_message ("%s=%s", SOURCETRACK_DOA_SPEECH, target);
    else if (g_str_has_prefix (tmp[i], SOURCETRACK_DOA_NOISE)) {
      g_message ("%s[%d]=%s", SOURCETRACK_DOA_NOISE, n_doa_noise, target);
      n_doa_noise++;
    } else {
      g_message ("%s", tmp[i]);
    }
  }
  g_strfreev (tmp);
  g_free (str);

  return G_SOURCE_CONTINUE;
}

#if defined(G_OS_UNIX)
static gboolean
intr_handler (gpointer user_data)
{
  TestData *data = (TestData *) user_data;
  GMainLoop *loop = data->loop;
  g_main_loop_quit (loop);
  data->signal_watch_id = 0;
  return G_SOURCE_REMOVE;
}
#endif

int
main (int argc, char *argv[])
{
  GOptionContext *context;
  GError *error = NULL;
  GstBus *bus;
  TestData *data;
  gboolean source_track = DEFAULT_SOURCE_TRACK;
  GStrv *arg_srcs;
  GOptionEntry entries[] = {
    {"SourceTrack", 's', 0, G_OPTION_ARG_NONE, &source_track,
        "Enable audio source track timer", NULL},
    {G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_STRING_ARRAY, &arg_srcs, NULL},
    {NULL}
  };
  guint i;

  context = g_option_context_new ("SOURCE1 SOURCE2 ...");
  g_option_context_add_main_entries (context, entries, NULL);
  g_option_context_set_summary (context, "Each source has the following form\n\
      ------------------------------------------\n\
      SOURCE: device,channels,format,samplerate,duration,location,profile\n\
      ------------------------------------------\n\
      device : (string) {builtin-mic, wired-headset}\n\
      channels : (int) {1, 2, 4} \n\
      format : (int) {16, 24}\n\
      samplerate : (int) [1, MAX]\n\
      audioSource : (string) {default, mic, voice-uplink, voice-downlink\n\
                   voice-call, camcorder, voice-recognition, voice-communication\n\
                   remote-submix, unprocessed, fm-tuner, hotword}\n\
      duration : (int) [1, MAX]\n\
      location : (string) /data/rec.raw\n\
      profile : (string) {none, record_fluence, record_mec, record_unprocessed, ...}\n\
          only have to enter profile when running multiple sources.\n\
      ");
  g_option_context_add_group (context, gst_init_get_option_group ());
  if (!g_option_context_parse (context, &argc, &argv, &error)) {
    g_print ("option parsing failed: %s\n", error->message);
    return -1;
  }

  if (!arg_srcs) {
    g_print ("%s", g_option_context_get_help (context, TRUE, NULL));
    g_option_context_free (context);
    exit (0);
  }
  g_option_context_free (context);

  gst_init (&argc, &argv);

  data = test_data_new ();
  data->pipeline = gst_pipeline_new ("pipeline");

  data->is_multiple_source = (g_strv_length ((gchar **) arg_srcs) > 1);

  for (i = 0; arg_srcs[i]; i++) {
    GstElement *src;
    src = parse_source ((gchar *) arg_srcs[i], data, source_track, &error);
    if (!src)
      goto failed;

    gst_bin_add (GST_BIN (data->pipeline), src);
  }

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

failed:
  g_warning ("failed to parse source: %s", error->message);
  gst_object_unref (data->pipeline);
  test_data_free (data);
  return 1;
}
