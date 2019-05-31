/*
*Copyright (c) 2017 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * SECTION:element-qsthwstrig
 * @see_also: qahwsrc, qahwsink
 *
 * This element reads data from an audio card using the HAL audio API.
 *
 * <refsect2>
 * <title>Example pipelines</title>
 * |[
 * TODO
 * ]| TODO
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include "gstqsthwstrig.h"

GST_DEBUG_CATEGORY_EXTERN (qsthw_debug);
#define GST_CAT_DEFAULT qsthw_debug

static GstStaticPadTemplate src_template = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY);

#define DEFAULT_PROP_MODULE_ID          "soundtrigger.primary"
#define DEFAULT_PROP_MODEL_LOCATION     ""
#define DEFAULT_PROP_NUM_KEYWORDS       1
#define DEFAULT_PROP_NUM_USERS          1
#define DEFAULT_PROP_USER_IDENTIFICATION FALSE
#define DEFAULT_PROP_KEYWORD_CONFIDENCE_LEVEL   60
#define DEFAULT_PROP_USER_CONFIDENCE_LEVEL      60
#define DEFAULT_PROP_VENDOR_UUID        "68ab2d40-e860-11e3-95ef-0002a5d5c51b"
#define DEFAULT_PROP_LAB            FALSE

enum
{
  PROP_0,
  PROP_MODULE_ID,
  PROP_MODEL_LOCATION,
  PROP_NUM_KEYWORDS,
  PROP_NUM_USERS,
  PROP_USER_IDENTIFICATION,
  PROP_KEYWORD_CONFIDENCE_LEVEL,
  PROP_USER_CONFIDENCE_LEVEL,
  PROP_VENDOR_UUID,
  PROP_LAB,
  PROP_USER_MODULE_HANDLE,
  PROP_LAST
};

#define gst_qsthw_sound_trigger_parent_class parent_class
G_DEFINE_TYPE (GstQsthwSoundTrigger, gst_qsthw_sound_trigger,
    GST_TYPE_PUSH_SRC);

static void gst_qsthw_sound_trigger_finalize (GObject * object);
static void gst_qsthw_sound_trigger_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_qsthw_sound_trigger_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec);
static GstStateChangeReturn gst_qsthw_sound_trigger_change_state (GstElement *
    element, GstStateChange transition);
static gboolean gst_qsthw_sound_trigger_unlock (GstBaseSrc * bsrc);
static gboolean gst_qsthw_sound_trigger_unlock_stop (GstBaseSrc * bsrc);

static gboolean gst_qsthw_sound_trigger_load (GstQsthwSoundTrigger * qsthw);
static gboolean gst_qsthw_sound_trigger_unload (GstQsthwSoundTrigger * qsthw);
static gboolean gst_qsthw_sound_trigger_start (GstQsthwSoundTrigger * qsthw);
static gboolean gst_qsthw_sound_trigger_stop (GstQsthwSoundTrigger * qsthw);
static GstFlowReturn gst_qsthw_sound_trigger_create (GstPushSrc * psrc,
    GstBuffer ** outbuf);

/* Qsthwstrig signals and args */
enum
{
  LAST_SIGNAL
};

/* The part within the #if is adapted from qsthw_test.c */
#if 1
static gboolean
string_to_uuid (const char *str, sound_trigger_uuid_t * uuid)
{
  int tmp[10];

  if (str == NULL || uuid == NULL) {
    return -EINVAL;
  }

  if (sscanf (str, "%08x-%04x-%04x-%04x-%02x%02x%02x%02x%02x%02x",
          tmp, tmp + 1, tmp + 2, tmp + 3, tmp + 4, tmp + 5, tmp + 6,
          tmp + 7, tmp + 8, tmp + 9) < 10) {
    return -EINVAL;
  }
  uuid->timeLow = (uint32_t) tmp[0];
  uuid->timeMid = (uint16_t) tmp[1];
  uuid->timeHiAndVersion = (uint16_t) tmp[2];
  uuid->clockSeq = (uint16_t) tmp[3];
  uuid->node[0] = (uint8_t) tmp[4];
  uuid->node[1] = (uint8_t) tmp[5];
  uuid->node[2] = (uint8_t) tmp[6];
  uuid->node[3] = (uint8_t) tmp[7];
  uuid->node[4] = (uint8_t) tmp[8];
  uuid->node[5] = (uint8_t) tmp[9];

  return 0;
}
#endif

static void
gst_qsthw_sound_trigger_finalize (GObject * object)
{
  GstQsthwSoundTrigger *qsthw = GST_QSTHW_SOUND_TRIGGER (object);

  g_free (qsthw->vendor_uuid);
  g_free (qsthw->model_location);
  g_free (qsthw->module_id);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gst_qsthw_sound_trigger_class_init (GstQsthwSoundTriggerClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
  GstBaseSrcClass *gstbasesrc_class;
  GstPushSrcClass *gstpushsrc_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;
  gstbasesrc_class = (GstBaseSrcClass *) klass;
  gstpushsrc_class = (GstPushSrcClass *) klass;

  gobject_class->finalize = gst_qsthw_sound_trigger_finalize;
  gobject_class->get_property = gst_qsthw_sound_trigger_get_property;
  gobject_class->set_property = gst_qsthw_sound_trigger_set_property;

  gst_element_class_add_static_pad_template (gstelement_class, &src_template);

  gst_element_class_set_static_metadata (gstelement_class,
      "Sound trigger (QSTHW)", "Control",
      "Detect keywords via QSTHW and posts a message on the bus",
      "Vincent Penquerc'h <vincent.penquerch@collabora.co.uk>");

  g_object_class_install_property (gobject_class, PROP_MODULE_ID,
      g_param_spec_string ("module-id", "Module ID", "QSTHW module ID",
          DEFAULT_PROP_MODULE_ID, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_MODEL_LOCATION,
      g_param_spec_string ("model-location", "Model file location",
          "model file location", DEFAULT_PROP_MODEL_LOCATION,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_NUM_KEYWORDS,
      g_param_spec_uint ("num-keywords", "Number of keywords",
          "Number of keywords in the model file", 1, G_MAXUINT,
          DEFAULT_PROP_NUM_KEYWORDS,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_NUM_USERS,
      g_param_spec_uint ("num-users", "Number of users",
          "Number of users in the model file", 1, G_MAXUINT,
          DEFAULT_PROP_NUM_USERS, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_USER_IDENTIFICATION,
      g_param_spec_boolean ("user-identification", "User identification",
          "User identification", DEFAULT_PROP_USER_IDENTIFICATION,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_KEYWORD_CONFIDENCE_LEVEL,
      g_param_spec_uint ("keyword-confidence-level", "Keyword confidence level",
          "Keyword confidence level", 0, 100,
          DEFAULT_PROP_KEYWORD_CONFIDENCE_LEVEL,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_USER_CONFIDENCE_LEVEL,
      g_param_spec_uint ("user-confidence-level", "User confidence level",
          "User confidence level", 0, 100, DEFAULT_PROP_USER_CONFIDENCE_LEVEL,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_VENDOR_UUID,
      g_param_spec_string ("vendor-uuid", "Vendor UUID", "QSTHW vendor UUID",
          DEFAULT_PROP_VENDOR_UUID,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_LAB,
      g_param_spec_boolean ("lab", "Look ahead buffering",
          "Look ahead buffering", DEFAULT_PROP_LAB,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_USER_MODULE_HANDLE,
      g_param_spec_uint ("module-handle", "module-handle",
          "User module handle", 1, G_MAXUINT,
          DEFAULT_PROP_NUM_USERS, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  gstelement_class->change_state =
      GST_DEBUG_FUNCPTR (gst_qsthw_sound_trigger_change_state);

  gstbasesrc_class->unlock = GST_DEBUG_FUNCPTR (gst_qsthw_sound_trigger_unlock);
  gstbasesrc_class->unlock_stop =
      GST_DEBUG_FUNCPTR (gst_qsthw_sound_trigger_unlock_stop);

  gstpushsrc_class->create = GST_DEBUG_FUNCPTR (gst_qsthw_sound_trigger_create);
}

static void
gst_qsthw_sound_trigger_sound_model_callback (struct sound_trigger_model_event
    *event, void *cookie)
{
  GstQsthwSoundTrigger *qsthw = GST_QSTHW_SOUND_TRIGGER (cookie);

  GST_INFO_OBJECT (qsthw, "sound model callback triggered");
}

static void gst_qsthw_sound_trigger_recognition_callback (struct
    sound_trigger_recognition_event *event, void *cookie);

static GstAudioFormat
gst_qsthw_sound_trigger_parse_audio_format (GstQsthwSoundTrigger * qsthw,
    audio_config_t * config)
{
  GstAudioFormat format;

  switch (config->format) {
    case AUDIO_FORMAT_PCM_16_BIT:
      format = GST_AUDIO_FORMAT_S16LE;
      break;
    case AUDIO_FORMAT_PCM_24_BIT_PACKED:
      format = GST_AUDIO_FORMAT_S24LE;
      break;
    default:
      GST_WARNING_OBJECT (qsthw, "unknown audio format %d", config->format);
      format = GST_AUDIO_FORMAT_UNKNOWN;
      break;
  }

  return format;
}

static void
gst_qsthw_sound_trigger_capture_lab (GstQsthwSoundTrigger * qsthw,
    struct qsthw_phrase_recognition_event *qsthw_event)
{
  guint blocksize;
  struct sound_trigger_phrase_recognition_event phrase_event;
  audio_config_t *audio_config;
  gint sample_rate, channels;
  GstAudioFormat format;
  GstCaps *caps;

  phrase_event = qsthw_event->phrase_event;
  audio_config = &phrase_event.common.audio_config;
  sample_rate = audio_config->sample_rate;
  channels = audio_channel_count_from_in_mask (audio_config->channel_mask);
  format = gst_qsthw_sound_trigger_parse_audio_format (qsthw, audio_config);

  gst_audio_info_init (&qsthw->info);
  gst_audio_info_set_format (&qsthw->info, format, sample_rate, channels, NULL);
  caps = gst_audio_info_to_caps (&qsthw->info);

  GST_LOG_OBJECT (qsthw, "audio caps: %" GST_PTR_FORMAT, caps);

  if ((blocksize = qsthw_get_buffer_size (qsthw->module, qsthw->model)) <= 0) {
    GST_WARNING_OBJECT (qsthw, "Invalid Buffer size returned");
    return;
  }
  gst_base_src_set_blocksize (GST_BASE_SRC (qsthw), blocksize);

  gst_pad_push_event (GST_BASE_SRC_PAD (qsthw), gst_event_new_caps (caps));
  gst_caps_unref (caps);

  GST_OBJECT_LOCK (qsthw);
  qsthw->enable_capturing = TRUE;
  g_cond_signal (&qsthw->create_cond);
  GST_OBJECT_UNLOCK (qsthw);
}

static void
gst_qsthw_sound_trigger_recognition_callback (struct
    sound_trigger_recognition_event *event, void *cookie)
{
  GstQsthwSoundTrigger *qsthw = GST_QSTHW_SOUND_TRIGGER (cookie);
  GstStructure *structure;
  GstMessage *message;
  struct qsthw_phrase_recognition_event *qsthw_event;

  GST_INFO_OBJECT (qsthw, "recognition callback triggered");

  qsthw_event = (struct qsthw_phrase_recognition_event *) event;
  structure = gst_structure_new ("GstQsthwSoundTrigger",
      "status", G_TYPE_INT, event->status,
      "type", G_TYPE_INT, event->type,
      "model", G_TYPE_INT, event->model,
      "capture-available", G_TYPE_BOOLEAN, event->capture_available,
      "qsthw-timestamp", G_TYPE_UINT64, qsthw_event->timestamp, NULL);

  message = gst_message_new_element (GST_OBJECT (qsthw), structure);
  gst_element_post_message (GST_ELEMENT (qsthw), message);

  if (event->capture_available)
    gst_qsthw_sound_trigger_capture_lab (qsthw, qsthw_event);
}

static struct sound_trigger_phrase_sound_model *
gst_qsthw_sound_trigger_load_model (GstQsthwSoundTrigger * qsthw)
{
  struct sound_trigger_phrase_sound_model *model = NULL;
  gchar *contents = NULL;
  gsize length = 0;
  GError *error = NULL;
  guint p, j;
  guint num_users = qsthw->user_identification ? qsthw->num_users : 0;
  int user_id = qsthw->num_keywords;

  if (!g_file_get_contents (qsthw->model_location, &contents, &length, &error)) {
    GST_ERROR_OBJECT (qsthw, "Failed to load model file %s: %s",
        qsthw->model_location, error->message);
    g_error_free (error);
    return NULL;
  }

  model = g_malloc (sizeof (struct sound_trigger_phrase_sound_model) + length);
  memset (model, 0, sizeof (struct sound_trigger_phrase_sound_model));
  memcpy (((guint8 *) model) + sizeof (struct sound_trigger_phrase_sound_model),
      contents, length);
  g_free (contents);

  model->common.type = SOUND_MODEL_TYPE_KEYPHRASE;
  model->common.data_size = length;
  model->common.data_offset = sizeof (struct sound_trigger_phrase_sound_model);

  for (p = 0; p < qsthw->num_keywords; p++) {
    model->phrases[p].num_users = num_users;
    if (qsthw->user_identification)
      model->phrases[p].recognition_mode =
          RECOGNITION_MODE_VOICE_TRIGGER | RECOGNITION_MODE_USER_IDENTIFICATION;
    else
      model->phrases[p].recognition_mode = RECOGNITION_MODE_VOICE_TRIGGER;
  }

  if (string_to_uuid (qsthw->vendor_uuid, &model->common.vendor_uuid)) {
    GST_ERROR_OBJECT (qsthw, "Invalid vendor UUID: %s", qsthw->vendor_uuid);
    g_free (model);
    return NULL;
  }

  memset (&qsthw->config, 0, sizeof (struct sound_trigger_recognition_config));
  qsthw->config.capture_handle = AUDIO_IO_HANDLE_NONE;
  qsthw->config.capture_device = AUDIO_DEVICE_NONE;
  qsthw->config.capture_requested = qsthw->lab;
  qsthw->config.num_phrases = qsthw->num_keywords;

  model->num_phrases = qsthw->num_keywords;
  for (p = 0; p < model->num_phrases; ++p) {
    model->phrases[p].num_users = num_users;
    qsthw->config.phrases[p].id = p;
    qsthw->config.phrases[p].confidence_level = qsthw->keyword_confidence_level;
    qsthw->config.phrases[p].num_levels = num_users;

    for (j = 0; j < num_users; j++) {
      qsthw->config.phrases[p].levels[j].level = qsthw->user_confidence_level;
      qsthw->config.phrases[p].levels[j].user_id = user_id++;
    }

    if (qsthw->user_identification) {
      qsthw->config.phrases[p].recognition_modes =
          RECOGNITION_MODE_VOICE_TRIGGER | RECOGNITION_MODE_USER_IDENTIFICATION;
    } else {
      qsthw->config.phrases[p].recognition_modes =
          RECOGNITION_MODE_VOICE_TRIGGER;
    }
  }

  GST_INFO_OBJECT (qsthw, "Loaded %u keywords:", model->num_phrases);
  for (p = 0; p < model->num_phrases; ++p) {
    GST_INFO_OBJECT (qsthw, "  %s", model->phrases[p].text);
  }

  return model;
}

static gboolean
gst_qsthw_sound_trigger_load (GstQsthwSoundTrigger * qsthw)
{
  int err;

  qsthw->model_data = gst_qsthw_sound_trigger_load_model (qsthw);
  if (!qsthw->model_data)
    return FALSE;

  if(!qsthw->module){
    qsthw->module = qsthw_load_module (qsthw->module_id);
    if (!qsthw->module) {
      GST_ERROR_OBJECT (qsthw, "Failed to load module '%s'", qsthw->module_id);
      return FALSE;
    }
  }

  err =
      qsthw_load_sound_model (qsthw->module, &qsthw->model_data->common,
      gst_qsthw_sound_trigger_sound_model_callback, qsthw, &qsthw->model);
  if (err) {
    GST_ERROR_OBJECT (qsthw, "Failed to load sound trigger model: %d", err);
    return FALSE;
  }
  return TRUE;
}

static gboolean
gst_qsthw_sound_trigger_unload (GstQsthwSoundTrigger * qsthw)
{
  int err;

  err = qsthw_unload_sound_model (qsthw->module, qsthw->model);

  if (err) {
    GST_ERROR_OBJECT (qsthw, "Failed to unload sound model: %d", err);
    return FALSE;
  }
  qsthw->model = -1;
  g_free (qsthw->model_data);
  qsthw->model_data = NULL;

  if (qsthw->module && !qsthw->user_module) {
    err = qsthw_unload_module (qsthw->module);
    if (err) {
      GST_ERROR_OBJECT (qsthw, "Failed to unload module: %d", err);
      return FALSE;
    }
    qsthw->module = NULL;
  }
  else
    GST_DEBUG_OBJECT (qsthw, "Sound Trigger Module unloaded by app");

  return TRUE;
}

static gboolean
gst_qsthw_sound_trigger_start (GstQsthwSoundTrigger * qsthw)
{
  int err;

  qsthw->current_ts = GST_CLOCK_TIME_NONE;

  err =
      qsthw_start_recognition (qsthw->module, qsthw->model, &qsthw->config,
      gst_qsthw_sound_trigger_recognition_callback, qsthw);
  if (err) {
    GST_ERROR_OBJECT (qsthw, "Failed to start recognition: %d", err);
    return FALSE;
  }
  return TRUE;
}

static gboolean
gst_qsthw_sound_trigger_stop (GstQsthwSoundTrigger * qsthw)
{
  int err;

  GST_OBJECT_LOCK (qsthw);
  if (qsthw->enable_capturing) {
    qsthw->enable_capturing = FALSE;
    qsthw_stop_buffering (qsthw->module, qsthw->model);
  }
  GST_OBJECT_UNLOCK (qsthw);

  err = qsthw_stop_recognition (qsthw->module, qsthw->model);
  if (err) {
    GST_ERROR_OBJECT (qsthw, "Failed to stop recognition: %d", err);
    return FALSE;
  }
  return TRUE;
}

static void
gst_qsthw_sound_trigger_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstQsthwSoundTrigger *qsthw;
  sound_trigger_uuid_t uuid;
  guint modVal=0;
  qsthw = GST_QSTHW_SOUND_TRIGGER (object);

  switch (prop_id) {
    case PROP_MODULE_ID:
      g_free (qsthw->module_id);
      qsthw->module_id = g_value_dup_string (value);
      if (qsthw->module_id == NULL) {
        qsthw->module_id = g_strdup (DEFAULT_PROP_MODULE_ID);
      }
      g_value_dup_string (value);
      break;
    case PROP_MODEL_LOCATION:
      g_free (qsthw->model_location);
      qsthw->model_location = g_value_dup_string (value);
      if (qsthw->model_location == NULL) {
        qsthw->model_location = g_strdup (DEFAULT_PROP_MODEL_LOCATION);
      }
      break;
    case PROP_NUM_KEYWORDS:
      qsthw->num_keywords = g_value_get_uint (value);
      break;
    case PROP_NUM_USERS:
      qsthw->num_users = g_value_get_uint (value);
      break;
    case PROP_USER_IDENTIFICATION:
      qsthw->user_identification = g_value_get_boolean (value);
      break;
    case PROP_KEYWORD_CONFIDENCE_LEVEL:
      qsthw->keyword_confidence_level = g_value_get_uint (value);
      break;
    case PROP_USER_CONFIDENCE_LEVEL:
      qsthw->user_confidence_level = g_value_get_uint (value);
      break;

    case PROP_VENDOR_UUID:
      g_free (qsthw->vendor_uuid);
      qsthw->vendor_uuid = g_value_dup_string (value);
      if (qsthw->vendor_uuid == NULL) {
        qsthw->vendor_uuid = g_strdup (DEFAULT_PROP_VENDOR_UUID);
      } else if (string_to_uuid (qsthw->vendor_uuid, &uuid)) {
        g_free (qsthw->vendor_uuid);
        qsthw->vendor_uuid = g_strdup (DEFAULT_PROP_VENDOR_UUID);
        g_warning ("Invalid vendor UUID, using default");
      }
      break;
    case PROP_LAB:
      qsthw->lab = g_value_get_boolean (value);
      break;
    case PROP_USER_MODULE_HANDLE:
    {
        if(!qsthw->module){
            modVal = g_value_get_uint (value);
            qsthw->module = (gpointer*)modVal;
            qsthw->user_module = TRUE;
        }
        break;
    }
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_qsthw_sound_trigger_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstQsthwSoundTrigger *qsthw;

  qsthw = GST_QSTHW_SOUND_TRIGGER (object);

  switch (prop_id) {
    case PROP_MODULE_ID:
      g_value_set_string (value, qsthw->module_id);
      break;
    case PROP_MODEL_LOCATION:
      g_value_set_string (value, qsthw->model_location);
      break;
    case PROP_NUM_KEYWORDS:
      g_value_set_uint (value, qsthw->num_keywords);
      break;
    case PROP_NUM_USERS:
      g_value_set_uint (value, qsthw->num_users);
      break;
    case PROP_USER_IDENTIFICATION:
      g_value_set_boolean (value, qsthw->user_identification);
      break;
    case PROP_KEYWORD_CONFIDENCE_LEVEL:
      g_value_set_uint (value, qsthw->keyword_confidence_level);
      break;
    case PROP_USER_CONFIDENCE_LEVEL:
      g_value_set_uint (value, qsthw->user_confidence_level);
      break;
    case PROP_VENDOR_UUID:
      g_value_set_string (value, qsthw->vendor_uuid);
      break;
    case PROP_LAB:
      g_value_set_boolean (value, qsthw->lab);
      break;
    case PROP_USER_MODULE_HANDLE:
    {
        if(!qsthw->module){
            g_value_set_pointer (value, qsthw->module);
        }
        break;
    }
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static GstStateChangeReturn
gst_qsthw_sound_trigger_change_state (GstElement * element,
    GstStateChange transition)
{
  GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
  GstQsthwSoundTrigger *qsthw = GST_QSTHW_SOUND_TRIGGER (element);

  switch (transition) {
    case GST_STATE_CHANGE_NULL_TO_READY:
      if (!gst_qsthw_sound_trigger_load (qsthw))
        return GST_STATE_CHANGE_FAILURE;
      break;
    case GST_STATE_CHANGE_READY_TO_PAUSED:
      break;
    case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
      if (!gst_qsthw_sound_trigger_start (qsthw))
        return GST_STATE_CHANGE_FAILURE;
      break;
    default:
      break;
  }

  ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    GST_ERROR_OBJECT (qsthw, "Parent element failed to change state");
    return ret;
  }

  switch (transition) {
    case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
      if (!gst_qsthw_sound_trigger_stop (qsthw))
        return GST_STATE_CHANGE_FAILURE;
      break;
    case GST_STATE_CHANGE_PAUSED_TO_READY:
      break;
    case GST_STATE_CHANGE_READY_TO_NULL:
      if (!gst_qsthw_sound_trigger_unload (qsthw))
        return GST_STATE_CHANGE_FAILURE;
      break;
    default:
      break;
  }

  return ret;
}

static gboolean
gst_qsthw_sound_trigger_unlock (GstBaseSrc * bsrc)
{
  GstQsthwSoundTrigger *qsthw = GST_QSTHW_SOUND_TRIGGER (bsrc);

  GST_LOG_OBJECT (qsthw, "Flushing");

  GST_OBJECT_LOCK (qsthw);
  qsthw->flushing = TRUE;
  g_cond_signal (&qsthw->create_cond);
  GST_OBJECT_UNLOCK (qsthw);

  return TRUE;
}

static gboolean
gst_qsthw_sound_trigger_unlock_stop (GstBaseSrc * bsrc)
{
  GstQsthwSoundTrigger *qsthw = GST_QSTHW_SOUND_TRIGGER (bsrc);

  GST_LOG_OBJECT (qsthw, "No longer Flushing");

  GST_OBJECT_LOCK (qsthw);
  qsthw->flushing = FALSE;
  g_cond_signal (&qsthw->create_cond);
  GST_OBJECT_UNLOCK (qsthw);

  return TRUE;
}

static GstFlowReturn
gst_qsthw_sound_trigger_create (GstPushSrc * psrc, GstBuffer ** outbuf)
{
  GstQsthwSoundTrigger *qsthw = GST_QSTHW_SOUND_TRIGGER (psrc);
  GstMapInfo map;
  guint res;
  GstBuffer *buffer;
  GstClockTime duration;
  guint blocksize;

  GST_OBJECT_LOCK (qsthw);
  while (!qsthw->flushing && !qsthw->enable_capturing) {
    g_cond_wait (&qsthw->create_cond, GST_OBJECT_GET_LOCK (qsthw));
  }

  if (qsthw->flushing) {
    GST_OBJECT_UNLOCK (qsthw);
    return GST_FLOW_FLUSHING;
  }
  GST_OBJECT_UNLOCK (qsthw);

  blocksize = gst_base_src_get_blocksize (GST_BASE_SRC (qsthw));
  buffer = gst_buffer_new_allocate (NULL, blocksize, NULL);

  gst_buffer_map (buffer, &map, GST_MAP_WRITE);
  res = qsthw_read_buffer (qsthw->module, qsthw->model, map.data, map.size);
  gst_buffer_unmap (buffer, &map);

#ifdef ENABLE_HW8009
  gst_buffer_resize (buffer,0,res);
#else
  if (res != 0) {
    GST_ERROR_OBJECT (qsthw, "Error while reading buffer");
    gst_buffer_unref (buffer);
    return GST_FLOW_ERROR;
  }
#endif

  duration =
      GST_FRAMES_TO_CLOCK_TIME (blocksize / GST_AUDIO_INFO_BPF (&qsthw->info),
      GST_AUDIO_INFO_RATE (&qsthw->info));

  /* set the first buffer's timestamp to current running time */
  if (!GST_CLOCK_TIME_IS_VALID (qsthw->current_ts)) {
    GstClock *clock;
    GstClockTime base_time;

    GST_OBJECT_LOCK (qsthw);
    if ((clock = GST_ELEMENT_CLOCK (qsthw))) {
      base_time = GST_ELEMENT_CAST (qsthw)->base_time;
      qsthw->current_ts = gst_clock_get_time (clock) - base_time;
    } else {
      GST_INFO_OBJECT (qsthw, "No clock provided, playback start from zero");
      qsthw->current_ts = 0;
    }
    GST_OBJECT_UNLOCK (qsthw);
  }

  GST_BUFFER_PTS (buffer) = qsthw->current_ts;
  GST_BUFFER_DURATION (buffer) = duration;
  /* update current_ts for next buffer */
  qsthw->current_ts = qsthw->current_ts + duration;

  *outbuf = buffer;
  GST_DEBUG_OBJECT (qsthw, "create buffer %p", *outbuf);

  return GST_FLOW_OK;
}

static void
gst_qsthw_sound_trigger_init (GstQsthwSoundTrigger * qsthw)
{
  GST_DEBUG_OBJECT (qsthw, "initializing");

  qsthw->module_id = g_strdup (DEFAULT_PROP_MODULE_ID);
  qsthw->model_location = g_strdup (DEFAULT_PROP_MODEL_LOCATION);
  qsthw->num_keywords = DEFAULT_PROP_NUM_KEYWORDS;
  qsthw->num_users = DEFAULT_PROP_NUM_USERS;
  qsthw->user_identification = DEFAULT_PROP_USER_IDENTIFICATION;
  qsthw->keyword_confidence_level = DEFAULT_PROP_KEYWORD_CONFIDENCE_LEVEL;
  qsthw->user_confidence_level = DEFAULT_PROP_USER_CONFIDENCE_LEVEL;
  qsthw->vendor_uuid = g_strdup (DEFAULT_PROP_VENDOR_UUID);
  qsthw->model_data = NULL;
  qsthw->module = NULL;
  qsthw->model = -1;
  qsthw->user_module = false;

  qsthw->current_ts = GST_CLOCK_TIME_NONE;
  g_cond_init (&qsthw->create_cond);

  /* configure basesrc to be a live source */
  gst_base_src_set_live (GST_BASE_SRC (qsthw), TRUE);
  /* make basesrc output a segment in time */
  gst_base_src_set_format (GST_BASE_SRC (qsthw), GST_FORMAT_TIME);
}
