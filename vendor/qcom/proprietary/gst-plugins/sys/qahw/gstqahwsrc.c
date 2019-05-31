/*
*Copyright (c) 2017 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * SECTION:element-qahwsrc
 * @see_also: qahwsink
 *
 * This element reads data from an audio card using the HAL audio API.
 *
 * <refsect2>
 * <title>Example pipelines</title>
 * |[
 * gst-launch-1.0 -v qahwsrc ! queue ! audioconvert ! qahwsink
 * ]| Record from a sound card using QAHW and playback
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <sys/ioctl.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <glib.h>

#include "gstqahwsrc.h"

GST_DEBUG_CATEGORY_EXTERN (qahw_debug);
#define GST_CAT_DEFAULT qahw_debug

#define DEFAULT_PROP_MODULE_ID		"audio.primary"
#define DEFAULT_PROP_INPUT_DEVICE       GST_QAHWSRC_INPUT_DEVICE_AMBIENT
#define DEFAULT_PROP_AUDIO_SOURCE       GST_QAHWSRC_AUDIO_SOURCE_MIC
#define DEFAULT_PROP_STREAM_PROFILE     "none"
#define DEFAULT_PROP_AUDIO_INPUT_FLAGS  GST_AUDIO_INPUT_FLAG_NONE
#define DEFAULT_PROP_DEVICE_ADDRESS     "input_stream"
#define DEFAULT_PROP_AUDIO_HANDLE       0x999
#define DEFAULT_PROP_FFV_STATE          0
#define DEFAULT_PROP_FFV_EC_REF_DEVICE  2
#define DEFAULT_PROP_FFV_CHANNEL_INDEX  0
#define DEFAULT_PROP_AUDIO_EFFECT_GET   "str.value"
#define DEFAULT_PROP_AUDIO_EFFECT_SET   "request.value"

enum
{
  PROP_0,
  PROP_MODULE_ID,
  PROP_INPUT_DEVICE,
  PROP_AUDIO_SOURCE,
  PROP_STREAM_PROFILE,
  PROP_AUDIO_INPUT_FLAGS,
  PROP_DEVICE_ADDRESS,
  PROP_AUDIO_HANDLE,
  PROP_KPI_MODE,
  PROP_FFV_STATE,
  PROP_FFV_EC_REF_DEVICE,
  PROP_FFV_CHANNEL_INDEX,
  PROP_AUDIO_EFFECT_GET,
  PROP_AUDIO_EFFECT_SET,
  PROP_LAST
};

/* signals and args */
enum
{
  SIGNAL_GET_PARAMETERS,

  LAST_SIGNAL
};

static guint gst_qahwsrc_signals[LAST_SIGNAL] = { 0 };

//Defined for KPI measurments
#define LATENCY_NODE "/sys/kernel/debug/audio_in_latency_measurement_node"
#define LATENCY_NODE_INIT_STR "1"

int ret = 0, count = 0;
FILE *fdLatencyNode = NULL;
struct timespec tsColdI, tsColdF, tsCont;
struct timespec tsClose;
guint64 tCold, tCont, tsec, tusec;
guint64 tClose;
char latencyBuf[200] = {0};

#define gst_qahwsrc_parent_class parent_class
G_DEFINE_TYPE (GstQahwSrc, gst_qahwsrc, GST_TYPE_AUDIO_SRC);

static void gst_qahwsrc_finalize (GObject * object);
static void gst_qahwsrc_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_qahwsrc_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec);
static GstStateChangeReturn gst_qahwsrc_change_state (GstElement * element,
    GstStateChange transition);

static gboolean gst_qahwsrc_open (GstAudioSrc * asrc);
static gboolean gst_qahwsrc_prepare (GstAudioSrc * asrc,
    GstAudioRingBufferSpec * spec);
static gboolean gst_qahwsrc_unprepare (GstAudioSrc * asrc);
static gboolean gst_qahwsrc_close (GstAudioSrc * asrc);
static guint gst_qahwsrc_read
    (GstAudioSrc * asrc, gpointer data, guint length, GstClockTime * timestamp);
static gchar *gst_qahw_get_parameters (GstQahwSrc * qahw, gchar * request);

static GstStaticPadTemplate qahwsrc_src_factory =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/x-raw, "
        "format = (string) {S16LE, S24LE}, "
        "layout = (string) interleaved, "
        "rate = (int) [ 1, MAX ], " "channels = (int) { 1, 2, 4 }")
    );

static GType
gst_qahwsrc_input_device_get_type (void)
{
  static volatile gsize input_device_type = 0;
  static const GEnumValue input_device[] = {
    {GST_QAHWSRC_INPUT_DEVICE_COMMUNICATION,
        "GST_QAHWSRC_INPUT_DEVICE_COMMUNICATION", "communication"},
    {GST_QAHWSRC_INPUT_DEVICE_AMBIENT, "GST_QAHWSRC_INPUT_DEVICE_AMBIENT",
        "ambient"},
    {GST_QAHWSRC_INPUT_DEVICE_BUILTIN_MIC,
        "GST_QAHWSRC_INPUT_DEVICE_BUILTIN_MIC", "builtin-mic"},
    {GST_QAHWSRC_INPUT_DEVICE_BLUETOOTH_SCO_HEADSET,
          "GST_QAHWSRC_INPUT_DEVICE_BLUETOOTH_SCO_HEADSET",
        "bluetooth-sco-headset"},
    {GST_QAHWSRC_INPUT_DEVICE_WIRED_HEADSET, "GST_QAHWSRC_WIRED_HEADSET",
        "wired-headset"},
    {GST_QAHWSRC_INPUT_DEVICE_AUX_DIGITAL,
        "GST_QAHWSRC_INPUT_DEVICE_AUX_DIGITAL", "aux-digital"},
    {GST_QAHWSRC_INPUT_DEVICE_HDMI, "GST_QAHWSRC_INPUT_DEVICE_HDMI", "hdmi"},
    {GST_QAHWSRC_INPUT_DEVICE_VOICE_CALL, "GST_QAHWSRC_INPUT_DEVICE_VOICE_CALL",
        "voice-call"},
    {GST_QAHWSRC_INPUT_DEVICE_TELEPHONY_RX,
        "GST_QAHWSRC_INPUT_DEVICE_TELEPHONY_RX", "telephony-rx"},
    {GST_QAHWSRC_INPUT_DEVICE_BACK_MIC, "GST_QAHWSRC_INPUT_DEVICE_BACK_MIC",
        "back-mic"},
    {GST_QAHWSRC_INPUT_DEVICE_REMOTE_SUBMIX,
        "GST_QAHWSRC_INPUT_DEVICE_REMOTE_SUBMIX", "remote-submix"},
    {GST_QAHWSRC_INPUT_DEVICE_ANALOG_DOCK_HEADSET,
          "GST_QAHWSRC_INPUT_DEVICE_ANALOG_DOCK_HEADSET",
        "analog-dock-headset"},
    {GST_QAHWSRC_INPUT_DEVICE_DIGITAL_DOCK_HEADSET,
          "GST_QAHWSRC_INPUT_DEVICE_DIGITAL_DOCK_HEADSET",
        "digital-dock-headset"},
    {GST_QAHWSRC_INPUT_DEVICE_USB_ACCESSORY,
        "GST_QAHWSRC_INPUT_DEVICE_USB_ACCESSORY", "usb-accessory"},
    {GST_QAHWSRC_INPUT_DEVICE_USB_DEVICE, "GST_QAHWSRC_INPUT_DEVICE_USB_DEVICE",
        "usb-device"},
    {GST_QAHWSRC_INPUT_DEVICE_FM_TUNER, "GST_QAHWSRC_INPUT_DEVICE_FM_TUNER",
        "fm-tuner"},
    {GST_QAHWSRC_INPUT_DEVICE_TV_TUNER, "GST_QAHWSRC_INPUT_DEVICE_TV_TUNER",
        "tv-tuner"},
    {GST_QAHWSRC_INPUT_DEVICE_LINE, "GST_QAHWSRC_INPUT_DEVICE_LINE", "line"},
    {GST_QAHWSRC_INPUT_DEVICE_SPDIF, "GST_QAHWSRC_INPUT_DEVICE_SPDIF", "spdif"},
    {GST_QAHWSRC_INPUT_DEVICE_BLUETOOTH_A2DP,
        "GST_QAHWSRC_INPUT_DEVICE_BLUETOOTH_A2DP", "bluetooth-a2dp"},
    {GST_QAHWSRC_INPUT_DEVICE_LOOPBACK, "GST_QAHWSRC_INPUT_DEVICE_LOOPBACK",
        "loopback"},
    {GST_QAHWSRC_INPUT_DEVICE_IP, "GST_QAHWSRC_INPUT_DEVICE_IP", "ip"},
    {GST_QAHWSRC_INPUT_DEVICE_PROXY, "GST_QAHWSRC_INPUT_DEVICE_PROXY", "proxy"},
    {GST_QAHWSRC_INPUT_DEVICE_DEFAULT, "GST_QAHWSRC_INPUT_DEVICE_DEFAULT",
        "default"},
    {0, NULL, NULL},
  };

  if (g_once_init_enter (&input_device_type)) {
    GType tmp = g_enum_register_static ("GstQahwSrcInputDevice", input_device);
    g_once_init_leave (&input_device_type, tmp);
  }

  return (GType) input_device_type;
}

static GType
gst_qahwsrc_audio_source_get_type (void)
{
  static volatile gsize audio_source_type = 0;
  static const GEnumValue audio_source[] = {
    {GST_QAHWSRC_AUDIO_SOURCE_DEFAULT, "GST_QAHWSRC_AUDIO_SOURCE_DEFAULT",
        "default"},
    {GST_QAHWSRC_AUDIO_SOURCE_MIC, "GST_QAHWSRC_AUDIO_SOURCE_MIC", "mic"},
    {GST_QAHWSRC_AUDIO_SOURCE_VOICE_UPLINK,
        "GST_QAHWSRC_AUDIO_SOURCE_VOICE_UPLINK", "voice-uplink"},
    {GST_QAHWSRC_AUDIO_SOURCE_VOICE_DOWNLINK,
        "GST_QAHWSRC_AUDIO_SOURCE_VOICE_DOWNLINK", "voice-downlink"},
    {GST_QAHWSRC_AUDIO_SOURCE_VOICE_CALL, "GST_QAHWSRC_AUDIO_SOURCE_VOICE_CALL",
        "voice-call"},
    {GST_QAHWSRC_AUDIO_SOURCE_CAMCORDER, "GST_QAHWSRC_AUDIO_SOURCE_CAMCORDER",
        "camcorder"},
    {GST_QAHWSRC_AUDIO_SOURCE_VOICE_RECOGNITION,
        "GST_QAHWSRC_AUDIO_SOURCE_VOICE_RECOGNITION", "voice-recognition"},
    {GST_QAHWSRC_AUDIO_SOURCE_VOICE_COMMUNICATION,
          "GST_QAHWSRC_AUDIO_SOURCE_VOICE_COMMUNICATION",
        "voice-communication"},
    {GST_QAHWSRC_AUDIO_SOURCE_REMOTE_SUBMIX,
        "GST_QAHWSRC_AUDIO_SOURCE_REMOTE_SUBMIX", "remote-submix"},
    {GST_QAHWSRC_AUDIO_SOURCE_UNPROCESSED,
        "GST_QAHWSRC_AUDIO_SOURCE_UNPROCESSED", "unprocessed"},
    {GST_QAHWSRC_AUDIO_SOURCE_FM_TUNER, "GST_QAHWSRC_AUDIO_SOURCE_FM_TUNER",
        "fm-tuner"},
    {GST_QAHWSRC_AUDIO_SOURCE_HOTWORD, "GST_QAHWSRC_AUDIO_SOURCE_HOTWORD",
        "hotword"},
    {0, NULL, NULL},
  };

  if (g_once_init_enter (&audio_source_type)) {
    GType tmp = g_enum_register_static ("GstQahwSrcAudioSource", audio_source);
    g_once_init_leave (&audio_source_type, tmp);
  }

  return (GType) audio_source_type;
}

GType
gst_audio_input_flags_get_type (void)
{
  static volatile gsize audio_input_flags_type = 0;
  static const GFlagsValue values[] = {
    {(guint) (GST_AUDIO_INPUT_FLAG_NONE), "No attributes", "none"},
    {(guint) (GST_AUDIO_INPUT_FLAG_FAST),
        "prefer an input that supports \"fast tracks\"", "fast"},
    {(guint) (GST_AUDIO_INPUT_FLAG_HW_HOTWORD),
        "prefer an input that captures from hw hotword source", "hw-hotword"},
    {(guint) (GST_AUDIO_INPUT_FLAG_RAW), "minimize signal processing", "raw"},
    {(guint) (GST_AUDIO_INPUT_FLAG_SYNC), "synchronize I/O streams", "sync"},
    {0, NULL, NULL}
  };

  if (g_once_init_enter (&audio_input_flags_type)) {
    GType tmp = g_flags_register_static ("GstAudioInputFlags", values);
    g_once_init_leave (&audio_input_flags_type, tmp);
  }

  return (GType) audio_input_flags_type;
}

static void
gst_qahwsrc_finalize (GObject * object)
{
  GstQahwSrc *src = GST_QAHW_SRC (object);

  g_free (src->device_address);
  g_free (src->module_id);
  g_free (src->stream_profile);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gst_qahwsrc_class_init (GstQahwSrcClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
  GstAudioSrcClass *gstaudiosrc_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;
  gstaudiosrc_class = (GstAudioSrcClass *) klass;

  gobject_class->finalize = gst_qahwsrc_finalize;
  gobject_class->get_property = gst_qahwsrc_get_property;
  gobject_class->set_property = gst_qahwsrc_set_property;

  gst_element_class_set_static_metadata (gstelement_class,
      "Audio source (QAHW)", "Source/Audio",
      "Read from a sound card via QAHW",
      "Vincent Penquerc'h <vincent.penquerch@collabora.co.uk>");

  gst_element_class_add_static_pad_template (gstelement_class,
      &qahwsrc_src_factory);

  gstaudiosrc_class->open = GST_DEBUG_FUNCPTR (gst_qahwsrc_open);
  gstaudiosrc_class->prepare = GST_DEBUG_FUNCPTR (gst_qahwsrc_prepare);
  gstaudiosrc_class->unprepare = GST_DEBUG_FUNCPTR (gst_qahwsrc_unprepare);
  gstaudiosrc_class->close = GST_DEBUG_FUNCPTR (gst_qahwsrc_close);
  gstaudiosrc_class->read = GST_DEBUG_FUNCPTR (gst_qahwsrc_read);
  gstelement_class->change_state = GST_DEBUG_FUNCPTR (gst_qahwsrc_change_state);

  klass->get_parameters = gst_qahw_get_parameters;

  g_object_class_install_property (gobject_class, PROP_MODULE_ID,
      g_param_spec_string ("module-id", "Module ID", "QAHW module ID",
          DEFAULT_PROP_MODULE_ID, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_INPUT_DEVICE,
      g_param_spec_enum ("input-device", "Input device", "QAHW input device",
          GST_TYPE_QAHWSRC_INPUT_DEVICE, DEFAULT_PROP_INPUT_DEVICE,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_AUDIO_SOURCE,
      g_param_spec_enum ("audio-source", "Audio source", "QAHW audio source",
          GST_TYPE_QAHWSRC_AUDIO_SOURCE, DEFAULT_PROP_AUDIO_SOURCE,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_STREAM_PROFILE,
      g_param_spec_string ("stream-profile", "Stream profile",
          "QAHW stream profile", DEFAULT_PROP_STREAM_PROFILE,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_AUDIO_INPUT_FLAGS,
      g_param_spec_flags ("audio-input-flags", "Audio input plags",
          "Audio input flags", GST_TYPE_AUDIO_INPUT_FLAGS,
          DEFAULT_PROP_AUDIO_INPUT_FLAGS,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_DEVICE_ADDRESS,
      g_param_spec_string ("device-address", "Device address",
          "QAHW device address", DEFAULT_PROP_DEVICE_ADDRESS,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_AUDIO_HANDLE,
      g_param_spec_int ("audio-handle", "Audio handle",
          "Audio I/O Handle", -1, G_MAXINT, DEFAULT_PROP_AUDIO_HANDLE,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_KPI_MODE,
      g_param_spec_boolean ("kpi-mode", "kpi-mode", "Enable KPI mode",
          FALSE, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_FFV_STATE,
      g_param_spec_int ("ffv-state", "ffv-state",
          "FFV State ON/OFF", -1, G_MAXINT, DEFAULT_PROP_FFV_STATE,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_FFV_EC_REF_DEVICE,
      g_param_spec_int ("ffv-ec-ref-dev", "ffv-ec-ref-dev",
          "FFV EC Reference device", -1, G_MAXINT, DEFAULT_PROP_FFV_EC_REF_DEVICE,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_FFV_CHANNEL_INDEX,
      g_param_spec_int ("ffv-channel", "ffv-channel",
          "FFV Channel index", -1, G_MAXINT, DEFAULT_PROP_FFV_CHANNEL_INDEX,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_AUDIO_EFFECT_GET,
      g_param_spec_string ("audio-effect-get", "audio-effect-get", "Audio Effect Get String:",
          DEFAULT_PROP_AUDIO_EFFECT_GET, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_AUDIO_EFFECT_SET,
      g_param_spec_string ("audio-effect-set", "audio-effect-set", "Audio Effect Request String:",
          DEFAULT_PROP_AUDIO_EFFECT_SET, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));


  /**
   * GstQahwSrc::get_parameters:
   * @qahwsrc: A #QahwSrc
   *
   * Clear all previously cached pt-mapping obtained with
   * #GstRtpBin::request-pt-map.
   */
  gst_qahwsrc_signals[SIGNAL_GET_PARAMETERS] =
      g_signal_new ("get-parameters", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION, G_STRUCT_OFFSET (GstQahwSrcClass,
          get_parameters), NULL, NULL, NULL, G_TYPE_STRING, 1, G_TYPE_STRING);
}

static void
gst_qahwsrc_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstQahwSrc *qahw;

  qahw = GST_QAHW_SRC (object);

  switch (prop_id) {
    case PROP_AUDIO_EFFECT_SET:
      qahw->audio_effect_set = g_value_dup_string (value);
      if (qahw->audio_effect_set == NULL) {
       qahw->audio_effect_set = g_strdup (DEFAULT_PROP_AUDIO_EFFECT_SET);
      }
      break;
    case PROP_AUDIO_EFFECT_GET:
      qahw->audio_effect_get = g_strdup (DEFAULT_PROP_AUDIO_EFFECT_GET);
      break;
    case PROP_MODULE_ID:
      g_free (qahw->module_id);
      qahw->module_id = g_value_dup_string (value);
      if (qahw->module_id == NULL) {
        qahw->module_id = g_strdup (DEFAULT_PROP_MODULE_ID);
      }
      break;
    case PROP_INPUT_DEVICE:
      qahw->input_device = g_value_get_enum (value);
      break;
    case PROP_AUDIO_SOURCE:
      qahw->audio_source = g_value_get_enum (value);
      break;
    case PROP_STREAM_PROFILE:
      g_free (qahw->stream_profile);
      qahw->stream_profile = g_value_dup_string (value);
      if (qahw->stream_profile == NULL) {
        qahw->stream_profile = g_strdup (DEFAULT_PROP_STREAM_PROFILE);
      }
      break;
    case PROP_AUDIO_INPUT_FLAGS:
      qahw->audio_input_flags = g_value_get_flags (value);
      break;
    case PROP_DEVICE_ADDRESS:
      g_free (qahw->device_address);
      qahw->device_address = g_value_dup_string (value);
      if (qahw->device_address == NULL) {
        qahw->device_address = g_strdup (DEFAULT_PROP_DEVICE_ADDRESS);
      }
      break;
    case PROP_AUDIO_HANDLE:
      qahw->audio_handle = g_value_get_int (value);
      break;
    case PROP_KPI_MODE:
      qahw->kpi_mode = g_value_get_boolean (value);
      qahw->audio_input_flags = GST_AUDIO_INPUT_FLAG_FAST;
      break;
    case PROP_FFV_STATE:
      qahw->ffv_state = g_value_get_int (value);
      break;
    case PROP_FFV_EC_REF_DEVICE:
      qahw->ffv_ec_ref_dev = g_value_get_int (value);
      break;
    case PROP_FFV_CHANNEL_INDEX:
      qahw->ffv_channel_index = g_value_get_int (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_qahwsrc_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstQahwSrc *qahw;

  qahw = GST_QAHW_SRC (object);

  switch (prop_id) {
    case PROP_AUDIO_EFFECT_GET:
      g_value_set_string(value, gst_qahw_get_parameters(qahw,qahw->audio_effect_set));
      break;
    case PROP_MODULE_ID:
      g_value_set_string (value, qahw->module_id);
      break;
    case PROP_INPUT_DEVICE:
      g_value_set_enum (value, qahw->input_device);
      break;
    case PROP_AUDIO_SOURCE:
      g_value_set_enum (value, qahw->audio_source);
      break;
    case PROP_STREAM_PROFILE:
      g_value_set_string (value, qahw->stream_profile);
      break;
    case PROP_AUDIO_INPUT_FLAGS:
      g_value_set_flags (value, qahw->audio_input_flags);
      break;
    case PROP_DEVICE_ADDRESS:
      g_value_set_string (value, qahw->device_address);
      break;
    case PROP_AUDIO_HANDLE:
      g_value_set_int (value, qahw->audio_handle);
      break;
    case PROP_KPI_MODE:
      g_value_set_int (value, qahw->kpi_mode);
      break;
    case PROP_FFV_STATE:
      g_value_set_int (value, qahw->ffv_state);
      break;
    case PROP_FFV_EC_REF_DEVICE:
      g_value_set_int (value, qahw->ffv_ec_ref_dev);
      break;
    case PROP_FFV_CHANNEL_INDEX:
      g_value_set_int (value, qahw->ffv_channel_index);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static GstStateChangeReturn
gst_qahwsrc_change_state (GstElement * element, GstStateChange transition)
{
  GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;

  ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);

  return ret;
}

static void
gst_qahwsrc_init (GstQahwSrc * qahwsrc)
{
  GST_DEBUG_OBJECT (qahwsrc, "initializing");

  qahwsrc->module_id = g_strdup (DEFAULT_PROP_MODULE_ID);
  qahwsrc->stream_profile = g_strdup (DEFAULT_PROP_STREAM_PROFILE);
  qahwsrc->input_device = DEFAULT_PROP_INPUT_DEVICE;
  qahwsrc->audio_source = DEFAULT_PROP_AUDIO_SOURCE;
  qahwsrc->audio_input_flags = DEFAULT_PROP_AUDIO_INPUT_FLAGS;
  qahwsrc->device_address = g_strdup (DEFAULT_PROP_DEVICE_ADDRESS);
  qahwsrc->audio_handle = DEFAULT_PROP_AUDIO_HANDLE;
  qahwsrc->kpi_mode = false;
  qahwsrc->audio_effect_get = g_strdup(DEFAULT_PROP_AUDIO_EFFECT_GET);
  qahwsrc->audio_effect_set = g_strdup(DEFAULT_PROP_AUDIO_EFFECT_SET);
}

static gboolean
qahwsrc_parse_spec (GstQahwSrc * qahw, GstAudioRingBufferSpec * spec)
{
  switch (spec->type) {
    case GST_AUDIO_RING_BUFFER_FORMAT_TYPE_RAW:
      switch (GST_AUDIO_INFO_FORMAT (&spec->info)) {
        case GST_AUDIO_FORMAT_S16LE:
          qahw->config.format = AUDIO_FORMAT_PCM_16_BIT;
          break;
        case GST_AUDIO_FORMAT_S24LE:
          qahw->config.format = AUDIO_FORMAT_PCM_24_BIT_PACKED;
          break;
        default:
          GST_ERROR_OBJECT (qahw, "Unsupported audio format: %d", spec->type);
          goto error;
      }
      break;
    default:
      goto error;

  }
  qahw->rate = GST_AUDIO_INFO_RATE (&spec->info);
  qahw->config.sample_rate = qahw->rate;

  qahw->channels = GST_AUDIO_INFO_CHANNELS (&spec->info);
  switch (qahw->channels) {
    case 1:
      qahw->config.channel_mask = AUDIO_CHANNEL_IN_MONO;
      break;
    case 2:
      qahw->config.channel_mask = AUDIO_CHANNEL_IN_STEREO;
      break;
    case 4:
      qahw->config.channel_mask = AUDIO_CHANNEL_INDEX_MASK_4;
      break;
    default:
      GST_ERROR_OBJECT (qahw, "Unsupported number of channels: %d",
          qahw->channels);
      goto error;
  }

  GST_INFO_OBJECT (qahw, "Spec parsed: %u Hz, %d channels",
      qahw->config.sample_rate, qahw->channels);

  return TRUE;

  /* ERRORS */
error:
  {
    return FALSE;
  }
}

static gboolean
gst_qahwsrc_open (GstAudioSrc * asrc)
{
  GstQahwSrc *qahw;

  qahw = GST_QAHW_SRC (asrc);

  qahw->module = qahw_load_module (qahw->module_id);
  if (!qahw->module) {
    GST_ELEMENT_ERROR (qahw, RESOURCE, OPEN_READ, (NULL),
        ("Could not load module."));
    return FALSE;
  }

  GST_LOG_OBJECT (qahw, "Opened device %s", qahw->module_id);

  return TRUE;
}

static gboolean
gst_qahwsrc_close (GstAudioSrc * asrc)
{
  GstQahwSrc *qahw = GST_QAHW_SRC (asrc);
  int err;

  GST_DEBUG_OBJECT (qahw, "close");
  if (qahw->module) {
    GST_DEBUG_OBJECT (qahw, "calling qahw_unload_module");
    err = qahw_unload_module (qahw->module);
    if (err) {
      GST_ERROR_OBJECT (qahw, "Error unloading module: %d", err);
      return FALSE;
    }
    qahw->module = NULL;
  }

  GST_DEBUG_OBJECT (qahw, "close succeeded");
  return TRUE;
}

static gboolean
gst_qahwsrc_prepare (GstAudioSrc * asrc, GstAudioRingBufferSpec * spec)
{
  GstQahwSrc *qahw;
  gint err;
  char *params;
  guint bpf = GST_AUDIO_INFO_BPF (&spec->info);
  guint rate = GST_AUDIO_INFO_RATE (&spec->info);

  qahw = GST_QAHW_SRC (asrc);
  GST_DEBUG_OBJECT (qahw, "prepare");

  if (!qahwsrc_parse_spec (qahw, spec))
    goto spec_parse;

  /* setup debug node if in kpi mode */
  if (qahw->kpi_mode) {
      fdLatencyNode = fopen(LATENCY_NODE,"r+");
      if (fdLatencyNode) {
          ret = fwrite(LATENCY_NODE_INIT_STR, sizeof(LATENCY_NODE_INIT_STR), 1, fdLatencyNode);
          if (ret < 1)
              GST_DEBUG_OBJECT(qahw, "error(%d) writing to debug node!, handle(%d)", ret, qahw->stream);
          fflush(fdLatencyNode);
      } else {
          GST_DEBUG_OBJECT(qahw, "debug node(%s) open failed!, handle(%d)", LATENCY_NODE, qahw->stream);
          fprintf(stdout, "debug node(%s) open failed!, handle(%d)", LATENCY_NODE, qahw->stream);
          goto open_failed;
      }
  }

  if(qahw->ffv_state)
  {
      /* set FFV params for the recording session */
      params = g_strdup_printf ("ffvOn=true;ffv_ec_ref_dev=%d;ffv_channel_index=%d",
                                qahw->ffv_ec_ref_dev, qahw->ffv_channel_index);

      GST_DEBUG_OBJECT (qahw, "Setting module parameters: %s", params);
      err = qahw_set_parameters(qahw->module, params);
      g_free (params);

      if (err)
        goto set_parameters_failed;
  }

  err =
      qahw_open_input_stream (qahw->module, qahw->audio_handle,
      qahw->input_device, &qahw->config, &qahw->stream, qahw->audio_input_flags,
      qahw->device_address, qahw->audio_source);
  if (err)
    goto open_failed;

  spec->segsize = qahw_in_get_buffer_size (qahw->stream);
  GST_DEBUG_OBJECT (qahw, "Opened input stream, segment size %d",
      spec->segsize);

  spec->latency_time = gst_util_uint64_scale (G_USEC_PER_SEC, spec->segsize,
      bpf * rate);
  spec->segtotal = spec->buffer_time / spec->latency_time;
  if (spec->segtotal * spec->latency_time < spec->buffer_time)
    spec->segtotal += 1;

  /* set profile for the recording session */
  params = g_strdup_printf ("audio_stream_profile=%s", qahw->stream_profile);
  GST_DEBUG_OBJECT (qahw, "Setting input stream parameters: %s", params);
  err = qahw_in_set_parameters (qahw->stream, params);
  g_free (params);
  if (err)
    goto set_parameters_failed;

  GST_DEBUG_OBJECT (qahw, "prepare succeeded");
  return TRUE;

  /* ERRORS */
spec_parse:
  {
    GST_ELEMENT_ERROR (qahw, RESOURCE, SETTINGS, (NULL),
        ("Error parsing spec"));
    return FALSE;
  }
open_failed:
  {
    GST_ELEMENT_ERROR (qahw, RESOURCE, SETTINGS, (NULL),
        ("Calling qahw_open_input_stream failed: %d", err));
    return FALSE;
  }
set_parameters_failed:
  {
    GST_ELEMENT_ERROR (qahw, RESOURCE, SETTINGS, (NULL),
        ("Calling qahw_in_set_parameters failed: %d", err));
    return FALSE;
  }
}

static gboolean
gst_qahwsrc_unprepare (GstAudioSrc * asrc)
{
  GstQahwSrc *qahw;
  int err;

  qahw = GST_QAHW_SRC (asrc);
  GST_DEBUG_OBJECT (qahw, "unprepare");

  GST_DEBUG_OBJECT (qahw, "calling qahw_in_standby");
  err = qahw_in_standby (qahw->stream);
  if (err) {
    GST_ERROR_OBJECT (qahw, "Failed to set input stream to standby: %d", err);
  }
  GST_DEBUG_OBJECT (qahw, "calling qahw_close_input_stream");
  err = qahw_close_input_stream (qahw->stream);
  if (err) {
    GST_ERROR_OBJECT (qahw, "Failed to close input stream: %d", err);
  }
  qahw->stream = NULL;

  GST_DEBUG_OBJECT (qahw, "unprepare succeeded");
  return TRUE;
}

static guint
gst_qahwsrc_read (GstAudioSrc * asrc, gpointer data, guint length,
    GstClockTime * timestamp)
{
  GstQahwSrc *qahw;
  qahw_in_buffer_t inbuf;
  ssize_t read;

  qahw = GST_QAHW_SRC (asrc);

  inbuf.buffer = data;
  inbuf.bytes = length;
  inbuf.timestamp=NULL;

  if (qahw->kpi_mode && count == 0) {
    ret = clock_gettime(CLOCK_REALTIME, &tsColdI);
    if (ret)
      GST_DEBUG_OBJECT(qahw, "error(%d) getting current time before first read!, handle(%d)", ret, qahw->stream);
  }

  read = qahw_in_read (qahw->stream, &inbuf);

  if (qahw->kpi_mode) {
    if (count == 0) {
      ret = clock_gettime(CLOCK_REALTIME, &tsColdF);
      if (ret)
        GST_DEBUG_OBJECT(qahw, "error(%d) getting current time after first read!, handle(%d)", ret, qahw->stream);
    } else if (count == 8) {
      /* 8th read done time is captured in kernel which would have trigger 9th read in DSP
      * 9th read is received by usersace at this time
      */
      ret = clock_gettime(CLOCK_REALTIME, &tsCont);
      if (ret)
        GST_DEBUG_OBJECT(qahw, "error(%d) getting current time after 8th read!, handle(%d)", ret, qahw->stream);

    }

    ret = clock_gettime(CLOCK_REALTIME, &tsClose);
    if (ret)
      GST_DEBUG_OBJECT(qahw, "error(%d) getting current time after 8th read!, handle(%d)", ret, qahw->stream);

    tClose = ((uint64_t)tsClose.tv_sec)*1000 - ((uint64_t)tsColdI.tv_sec)*1000 +
             ((uint64_t)tsClose.tv_nsec)/1000000 - ((uint64_t)tsColdI.tv_nsec)/1000000;

    //Run KPI for 10Sec
    if(tClose > 10000 && count > 8) {
      /* capture latency kpis if required */
      if (qahw->kpi_mode) {
        tCold = tsColdF.tv_sec*1000 - tsColdI.tv_sec*1000 +
                tsColdF.tv_nsec/1000000 - tsColdI.tv_nsec/1000000;

        fread((void *) latencyBuf, 100, 1, fdLatencyNode);
        fclose(fdLatencyNode);
        sscanf(latencyBuf, " %llu,%llu", &tsec, &tusec);
        tCont = ((uint64_t)tsCont.tv_sec)*1000 - tsec*1000 + ((uint64_t)tsCont.tv_nsec)/1000000 - tusec/1000;
        GST_DEBUG_OBJECT(qahw, "\n values from debug node %s, handle(%d)\n", latencyBuf, qahw->stream);
        GST_DEBUG_OBJECT(qahw, "\n cold latency %llums, continuous latency %llums, handle(%d)\n", tCold, tCont, qahw->stream);
        GST_DEBUG_OBJECT(qahw, " **Note: please add DSP Pipe/PP latency numbers to this, for final latency values\n");
        printf("\n cold latency %llums, continuous latency %" G_GUINT64_FORMAT ", handle(%d)\n", tCold, tCont, qahw->stream);
        printf(" **Note: please add DSP Pipe/PP latency numbers to this, for final latency values\n");
        g_assert_not_reached();
      }
    }
    count++;
  }

  return read;
}


static gchar *
gst_qahw_get_parameters (GstQahwSrc * qahw, gchar * request)
{
  gchar *str = NULL;

  if (qahw->module) {
    str = qahw_get_parameters (qahw->module, request);
    GST_DEBUG_OBJECT (qahw, "Requested params \"%s\" got \"%s\"", request, str);

  } else {
    g_critical ("The get-parameters action signal should only be used in state"
        " READY or higher");
  }

  return str;
}
