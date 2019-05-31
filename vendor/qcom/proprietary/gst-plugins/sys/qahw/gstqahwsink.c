/*
*Copyright (c) 2017 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * SECTION:element-qahwsink
 * @see_also: qahwsrc
 *
 * This element renders audio samples using the HAL audio API.
 *
 * <refsect2>
 * <title>Example pipelines</title>
 * |[
 * gst-launch-1.0 -v uridecodebin uri=file:///path/to/audio.ogg ! audioconvert ! audioresample ! qahwsink
 * ]| Play an Ogg/Vorbis file and output audio via QAHW.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>

#include <gst/base/gstbitreader.h>
#include "gstqahwsink.h"
#include "gstqahw-common.h"

struct wav_header {
    uint32_t riff_id;
    uint32_t riff_sz;
    uint32_t riff_fmt;
    uint32_t fmt_id;
    uint32_t fmt_sz;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    uint32_t data_id;
    uint32_t data_sz;
};

struct audio_config_params {
    qahw_module_handle_t *qahw_mod_handle;
    audio_io_handle_t handle;
    audio_devices_t input_device;
    audio_config_t config;
    audio_input_flags_t flags;
    const char* kStreamName ;
    audio_source_t kInputSource;
    char *file_name;
    volatile bool thread_exit;
};
struct proxy_data {
    struct audio_config_params acp;
    struct wav_header hdr;
};

struct proxy_data proxy_params;

#define AFE_PROXY_SAMPLING_RATE 48000
#define AFE_PROXY_CHANNEL_COUNT 2
#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164
#define FORMAT_PCM 1
#define WAV_HEADER_LENGTH_MAX 46

audio_io_handle_t stream_handle = 0x999;
gboolean proxy_thread_active = false;
pthread_t proxy_thread;

static const guint aac_sample_rates[] = { 96000, 88200, 64000, 48000, 44100,
  32000, 24000, 22050, 16000, 12000, 11025, 8000, 7350
};

gboolean wakelock_acquired = false;

GST_DEBUG_CATEGORY_EXTERN (qahw_debug);
#define GST_CAT_DEFAULT qahw_debug

G_DECLARE_FINAL_TYPE (GstQahwRingBuffer, gst_qahw_ring_buffer, GST,
    QAHW_RING_BUFFER, GstAudioRingBuffer)

     struct _GstQahwRingBuffer
     {
       GstAudioRingBuffer parent;
     };

G_DEFINE_TYPE (GstQahwRingBuffer, gst_qahw_ring_buffer,
    GST_TYPE_AUDIO_RING_BUFFER)
#define GST_TYPE_QAHW_RING_BUFFER (gst_qahw_ring_buffer_get_type ())
     static gboolean gst_qahw_ring_buffer_open_device (GstAudioRingBuffer * rb);
     static gboolean gst_qahw_ring_buffer_acquire (GstAudioRingBuffer * rb,
    GstAudioRingBufferSpec * spec);
     static gboolean gst_qahw_ring_buffer_release (GstAudioRingBuffer * rb);
     static gboolean gst_qahw_ring_buffer_close_device (GstAudioRingBuffer *
    rb);
     static gboolean gst_qahw_ring_buffer_start (GstAudioRingBuffer * rb);
     static gboolean gst_qahw_ring_buffer_pause (GstAudioRingBuffer * rb);
     static gboolean gst_qahw_ring_buffer_resume (GstAudioRingBuffer * rb);
     static gboolean gst_qahw_ring_buffer_stop (GstAudioRingBuffer * rb);
     static guint gst_qahw_ring_buffer_delay (GstAudioRingBuffer * rb);
     static gboolean gst_qahw_ring_buffer_activate (GstAudioRingBuffer * rb,
    gboolean active);
     static guint gst_qahw_ring_buffer_commit (GstAudioRingBuffer * rb,
    guint64 * sample, guint8 * data, gint in_samples, gint out_samples,
    gint * accum);
     static void gst_qahw_ring_buffer_clear_all (GstAudioRingBuffer * rb);

     static void gst_qahw_ring_buffer_init (GstQahwRingBuffer * self)
{
}

static void
gst_qahw_ring_buffer_class_init (GstQahwRingBufferClass * klass)
{
  GstAudioRingBufferClass *rb_class = GST_AUDIO_RING_BUFFER_CLASS (klass);

  rb_class->open_device = gst_qahw_ring_buffer_open_device;
  rb_class->acquire = gst_qahw_ring_buffer_acquire;
  rb_class->release = gst_qahw_ring_buffer_release;
  rb_class->close_device = gst_qahw_ring_buffer_close_device;
  rb_class->start = gst_qahw_ring_buffer_start;
  rb_class->stop = gst_qahw_ring_buffer_stop;
  rb_class->pause = gst_qahw_ring_buffer_pause;
  rb_class->resume = gst_qahw_ring_buffer_resume;
  rb_class->stop = gst_qahw_ring_buffer_stop;
  rb_class->delay = gst_qahw_ring_buffer_delay;
  rb_class->activate = gst_qahw_ring_buffer_activate;
  rb_class->commit = gst_qahw_ring_buffer_commit;
  rb_class->clear_all = gst_qahw_ring_buffer_clear_all;
}


#define DEFAULT_PROP_MODULE_ID		"audio.primary"
#define DEFAULT_PROP_OUTPUT_DEVICE      GST_QAHWSRC_OUTPUT_DEVICE_SPEAKER
#define DEFAULT_PROP_VOLUME             1.0f
#define DEFAULT_PROP_DEVICE_ADDRESS     ""
#define DEFAULT_PROP_KVPAIR_VALUES      ""
#define DEFAULT_PROP_AUDIO_HANDLE       0x999
#define DEFAULT_PROP_OUTPUT_FLAGS       GST_QAHWSRC_OUTPUT_FLAG_NON_BLOCKING | GST_QAHWSRC_OUTPUT_FLAG_COMPRESS_OFFLOAD | GST_QAHWSRC_OUTPUT_FLAG_DIRECT
#define DEFAULT_PROP_SESSION_TYPE       GST_QAHWSRC_OUTPUT_SESSION_DEFAULT

//Defined for KPI measurments
#define LATENCY_NODE "/sys/kernel/debug/audio_out_latency_measurement_node"
#define LATENCY_NODE_INIT_STR "1"
FILE *fd_latency_node = NULL;
int rc = 0;
int offset = 0;
size_t bytes_wanted = 0;
size_t write_length = 0;
size_t bytes_remaining = 0;
size_t bytes_written = 0;
char  *data = NULL;
static int pb_count = 0;
struct timespec ts_cold, ts_cont;
uint64_t tcold, tcont, scold = 0, uscold = 0, scont = 0, uscont = 0;

enum
{
  PROP_0,
  PROP_MODULE_ID,
  PROP_OUTPUT_DEVICE,
  PROP_VOLUME,
  PROP_LEFT_VOLUME,
  PROP_RIGHT_VOLUME,
  PROP_MUTE,
  PROP_DEVICE_ADDRESS,
  PROP_KVPAIR_VALUES,
  PROP_AUDIO_HANDLE,
  PROP_OUTPUT_FLAGS,
  PROP_SET_PARAM,
  PROP_DUMP_FILE,
  PROP_KPI_MODE,
  PROP_BT_ADDR,
  PROP_USER_MODULE_HANDLE,
  PROP_SESSION_TYPE,
  PROP_LAST
};

static void gst_qahw_sink_stream_volume_init (GstStreamVolume * volume);

#define gst_qahwsink_parent_class parent_class
G_DEFINE_TYPE_WITH_CODE (GstQahwSink, gst_qahwsink,
    GST_TYPE_AUDIO_BASE_SINK, G_IMPLEMENT_INTERFACE (GST_TYPE_STREAM_VOLUME,
        gst_qahw_sink_stream_volume_init));

static void gst_qahwsink_finalize (GObject * object);
static void gst_qahwsink_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_qahwsink_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec);

static GstAudioRingBuffer *gst_qahwsink_create_ringbuffer (GstAudioBaseSink *
    absink);

static gboolean gst_qahwsink_wait_event (GstBaseSink * sink, GstEvent * event);
static gboolean gst_qahwsink_event (GstBaseSink * sink, GstEvent * event);
static int async_callback (qahw_stream_callback_event_t event, void *param,
    void *cookie);

static GstClockTime gst_qahwsink_get_time (GstClock * clock,
    GstAudioBaseSink * sink);

int write_to_hal(GstQahwSink *qahw, char *data, size_t bytes, void *params_ptr);
//int measure_kpi_values(GstQahwSink *qahw , bool is_offload);

static GstStaticPadTemplate qahwsink_sink_factory =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (QAHW_SINK_CAPS)
    );
/* Not yet enabled
 *       "audio/x-wma; audio/x-alac; "
 */

/* The part within the #if is adapted from qahw_playback_test.c */
#if 1
#define FLAC_KVPAIR "music_offload_avg_bit_rate=%d;" \
                    "music_offload_flac_max_blk_size=%d;" \
                    "music_offload_flac_max_frame_size=%d;" \
                    "music_offload_flac_min_blk_size=%d;" \
                    "music_offload_flac_min_frame_size=%d;" \
                    "music_offload_sample_rate=%d;"

#define ALAC_KVPAIR "music_offload_alac_avg_bit_rate=%d;" \
                    "music_offload_alac_bit_depth=%d;" \
                    "music_offload_alac_channel_layout_tag=%d;" \
                    "music_offload_alac_compatible_version=%d;" \
                    "music_offload_alac_frame_length=%d;" \
                    "music_offload_alac_kb=%d;" \
                    "music_offload_alac_max_frame_bytes=%d;" \
                    "music_offload_alac_max_run=%d;" \
                    "music_offload_alac_mb=%d;" \
                    "music_offload_alac_num_channels=%d;" \
                    "music_offload_alac_pb=%d;" \
                    "music_offload_alac_sampling_rate=%d;" \
                    "music_offload_avg_bit_rate=%d;" \
                    "music_offload_sample_rate=%d;"

#define VORBIS_KVPAIR "music_offload_avg_bit_rate=%d;" \
                      "music_offload_sample_rate=%d;" \
                      "music_offload_vorbis_bitstream_fmt=%d;"

#define WMA_KVPAIR "music_offload_avg_bit_rate=%d;" \
                   "music_offload_sample_rate=%d;" \
                   "music_offload_wma_bit_per_sample=%d;" \
                   "music_offload_wma_block_align=%d;" \
                   "music_offload_wma_channel_mask=%d;" \
                   "music_offload_wma_encode_option=%d;" \
                   "music_offload_wma_format_tag=%d;"

static gboolean request_wake_lock(GstQahwSink * qahw, gboolean wakelock_acquired, gboolean enable)
{
  int system_ret;

  if (enable) {
    if (!wakelock_acquired) {
      system_ret = system("echo gst_plugins > /sys/power/wake_lock");
      if (system_ret < 0) {
        GST_ERROR_OBJECT (qahw, "Failed to acquire gst_plugins lock");
      } else {
        wakelock_acquired = true;
        GST_DEBUG_OBJECT (qahw, "Success to acquire audio_service lock");
      }
    } else
      GST_DEBUG_OBJECT (qahw, "Lock is already acquired");
  }

  if (!enable) {
    if (wakelock_acquired) {
      system_ret = system("echo gst_plugins > /sys/power/wake_unlock");
      if (system_ret < 0) {
        GST_ERROR_OBJECT (qahw, "Failed to release audio_service lock");
      } else {
        wakelock_acquired = false;
        GST_DEBUG_OBJECT (qahw, "Success to release audio_service lock");
      }
    } else
      GST_DEBUG_OBJECT (qahw, "No Lock is acquired to release");
  }

  return wakelock_acquired;
}

void *proxy_read (void* data)
{
    struct proxy_data* params = (struct proxy_data*) data;
    qahw_module_handle_t *qahw_mod_handle = params->acp.qahw_mod_handle;
    qahw_in_buffer_t in_buf;
    char *buffer;
    int rc = 0;
    int bytes_to_read, bytes_written = 0;
    FILE *fp = NULL;
    qahw_stream_handle_t* in_handle = NULL;

    rc = qahw_open_input_stream(qahw_mod_handle, params->acp.handle,
              params->acp.input_device, &params->acp.config, &in_handle,
              params->acp.flags, params->acp.kStreamName, params->acp.kInputSource);
    if (rc) {
        //GST_ERROR_OBJECT (qahw, "Could not open input stream %d \n",rc);
        fprintf(stderr, "Could not open input stream %d \n",rc);
        pthread_exit(0);
     }

    if (in_handle != NULL) {
        bytes_to_read = qahw_in_get_buffer_size(in_handle);
        buffer = (char *) calloc(1, bytes_to_read);
        if (buffer == NULL) {
            //GST_ERROR_OBJECT (qahw, "calloc failed!!\n");
            fprintf(stderr, "calloc failed!!\n");
            pthread_exit(0);
        }

        if ((fp = fopen(params->acp.file_name,"w"))== NULL) {
            //GST_ERROR_OBJECT (qahw,  "Cannot open file to dump proxy data\n");
            fprintf(stderr, "Cannot open file to dump proxy data\n");
            pthread_exit(0);
        }
        else {
          params->hdr.num_channels = audio_channel_count_from_in_mask(params->acp.config.channel_mask);
          params->hdr.sample_rate = params->acp.config.sample_rate;
          params->hdr.byte_rate = params->hdr.sample_rate * params->hdr.num_channels * 2;
          params->hdr.block_align = params->hdr.num_channels * 2;
          params->hdr.bits_per_sample = 16;
          fwrite(&params->hdr, 1, sizeof(params->hdr), fp);
        }
        memset(&in_buf,0, sizeof(qahw_in_buffer_t));
        in_buf.buffer = buffer;
        in_buf.bytes = bytes_to_read;

        while (!(params->acp.thread_exit)) {
            rc = qahw_in_read(in_handle, &in_buf);
            if (rc > 0) {
                bytes_written += fwrite((char *)(in_buf.buffer), sizeof(char), (int)in_buf.bytes, fp);
            }
        }
        params->hdr.data_sz = bytes_written;
        params->hdr.riff_sz = bytes_written + 36; //sizeof(hdr) - sizeof(riff_id) - sizeof(riff_sz)
        fseek(fp, 0L , SEEK_SET);
        fwrite(&params->hdr, 1, sizeof(params->hdr), fp);
        fclose(fp);
        rc = qahw_in_standby(in_handle);
        if (rc) {
            //GST_ERROR_OBJECT (qahw, "in standby failed %d \n", rc);
            fprintf(stderr, "in standby failed %d \n", rc);
        }
        rc = qahw_close_input_stream(in_handle);
        if (rc) {
            //GST_ERROR_OBJECT (qahw,  "could not close input stream %d \n", rc);
            fprintf(stderr, "could not close input stream %d \n", rc);
        }
        //GST_ERROR_OBJECT (qahw,  "pcm data saved to file %s", params->acp.file_name);
    }
    return 0;
}
static char *
read_kvpair (const char *kvpair_values, audio_format_t format)
{
  char *kvpair_type_tok, *kvpair, *kvpair_values_tok, *saveptr = NULL, *ptr;
  const char *token = NULL, *kvpair_type;
  int len = 0;
  int size = 0;
  gint tok;

  switch (format) {
    case AUDIO_FORMAT_FLAC:
      kvpair_type = FLAC_KVPAIR;
      break;
    case AUDIO_FORMAT_VORBIS:
      kvpair_type = VORBIS_KVPAIR;
      break;
    case AUDIO_FORMAT_ALAC:
      kvpair_type = ALAC_KVPAIR;
      break;
    case AUDIO_FORMAT_WMA:
      kvpair_type = WMA_KVPAIR;
      break;
    default:
      return NULL;
  }

  kvpair_type_tok = g_strdup (kvpair_type);
  size = strlen (kvpair_type);
  token = kvpair_type_tok;
  while ((token = strstr (token, "%d"))) {
    size += 16;                 /* enough to hold a %d */
    token += 2;
  }
  g_free (kvpair_type_tok);
  kvpair = ptr = g_malloc (size + 1);
  *kvpair = 0;

  kvpair_values_tok = g_strdup (kvpair_values);
  token = strtok_r (kvpair_values_tok, ",", &saveptr);
  while (token) {
    len = strcspn (kvpair_type, "=");
    size = len + strlen (token) + 2;
    tok = g_ascii_strtoll (token, NULL, 10);
    snprintf (ptr, size, kvpair_type, tok);
    ptr += size - 1;
    kvpair_type += len + 3;
    token = strtok_r (NULL, ",", &saveptr);
  }
  g_free (kvpair_values_tok);

  return kvpair;
}
#endif


void parse_aptx_dec_bd_addr(char *value, struct qahw_aptx_dec_param *aptx_cfg)
{
    int ba[6];
    char *str, *tok;
    uint32_t addr[3];
    int i = 0;

    tok = strtok_r(value, ":", &str);
    while (tok != NULL) {
        ba[i] = strtol(tok, NULL, 16);
        i++;
        tok = strtok_r(NULL, ":", &str);
    }
    addr[0] = (ba[0] << 8) | ba[1];
    addr[1] = ba[2];
    addr[2] = (ba[3] << 16) | (ba[4] << 8) | ba[5];

    aptx_cfg->bt_addr.nap = addr[0];
    aptx_cfg->bt_addr.uap = addr[1];
    aptx_cfg->bt_addr.lap = addr[2];
}


static GType
gst_qahwsink_output_device_get_type (void)
{
  static volatile gsize output_device_type = 0;
  static const GEnumValue output_device[] = {
    {GST_QAHWSRC_OUTPUT_DEVICE_EARPIECE, "GST_QAHWSRC_OUTPUT_DEVICE_EARPIECE",
        "earpiece"},
    {GST_QAHWSRC_OUTPUT_DEVICE_SPEAKER, "GST_QAHWSRC_OUTPUT_DEVICE_SPEAKER",
        "speaker"},
    {GST_QAHWSRC_OUTPUT_DEVICE_WIRED_HEADSET,
        "GST_QAHWSRC_OUTPUT_DEVICE_WIRED_HEADSET", "wired-headset"},
    {GST_QAHWSRC_OUTPUT_DEVICE_WIRED_HEADPHONE,
        "GST_QAHWSRC_OUTPUT_DEVICE_WIRED_HEADPHONE", "wired-headphone"},
    {GST_QAHWSRC_OUTPUT_DEVICE_BLUETOOTH_SCO,
        "GST_QAHWSRC_OUTPUT_DEVICE_BLUETOOTH_SCO", "bluetooth-sco"},
    {GST_QAHWSRC_OUTPUT_DEVICE_BLUETOOTH_SCO_HEADSET,
          "GST_QAHWSRC_OUTPUT_DEVICE_BLUETOOTH_SCO_HEADSET",
        "bluetooth-sco-headset"},
    {GST_QAHWSRC_OUTPUT_DEVICE_BLUETOOTH_SCO_CARKIT,
          "GST_QAHWSRC_OUTPUT_DEVICE_BLUETOOTH_SCO_CARKIT",
        "bluetooth-sco-carkit"},
    {GST_QAHWSRC_OUTPUT_DEVICE_BLUETOOTH_A2DP,
        "GST_QAHWSRC_OUTPUT_DEVICE_BLUETOOTH_A2DP", "bluetooth-a2dp"},
    {GST_QAHWSRC_OUTPUT_DEVICE_BLUETOOTH_A2DP_HEADPHONES,
          "GST_QAHWSRC_OUTPUT_DEVICE_BLUETOOTH_A2DP_HEADPHONES",
        "bluetooth-a2dp-headphones"},
    {GST_QAHWSRC_OUTPUT_DEVICE_BLUETOOTH_A2DP_SPEAKER,
          "GST_QAHWSRC_OUTPUT_DEVICE_BLUETOOTH_A2DP_SPEAKER",
        "bluetooth-a2dp-speaker"},
    {GST_QAHWSRC_OUTPUT_DEVICE_AUX_DIGITAL,
        "GST_QAHWSRC_OUTPUT_DEVICE_AUX_DIGITAL", "aux-digital"},
    {GST_QAHWSRC_OUTPUT_DEVICE_HDMI, "GST_QAHWSRC_OUTPUT_DEVICE_HDMI", "hdmi"},
    {GST_QAHWSRC_OUTPUT_DEVICE_ANALOG_DOCK_HEADSET,
          "GST_QAHWSRC_OUTPUT_DEVICE_ANALOG_DOCK_HEADSET",
        "analog-dock-headset"},
    {GST_QAHWSRC_OUTPUT_DEVICE_DIGITAL_DOCK_HEADSET,
          "GST_QAHWSRC_OUTPUT_DEVICE_DIGITAL_DOCK_HEADSET",
        "digital-dock-headset"},
    {GST_QAHWSRC_OUTPUT_DEVICE_USB_ACCESSORY,
        "GST_QAHWSRC_OUTPUT_DEVICE_USB_ACCESSORY", "usb-accessory"},
    {GST_QAHWSRC_OUTPUT_DEVICE_USB_DEVICE,
        "GST_QAHWSRC_OUTPUT_DEVICE_USB_DEVICE", "usb-device"},
    {GST_QAHWSRC_OUTPUT_DEVICE_REMOTE_SUBMIX,
        "GST_QAHWSRC_OUTPUT_DEVICE_REMOTE_SUBMIX", "remote-submix"},
    {GST_QAHWSRC_OUTPUT_DEVICE_TELEPHONY_TX,
        "GST_QAHWSRC_OUTPUT_DEVICE_TELEPHONY_TX", "telephony-tx"},
    {GST_QAHWSRC_OUTPUT_DEVICE_LINE, "GST_QAHWSRC_OUTPUT_DEVICE_LINE", "line"},
    {GST_QAHWSRC_OUTPUT_DEVICE_HDMI_ARC, "GST_QAHWSRC_OUTPUT_DEVICE_HDMI_ARC",
        "hdmi-arc"},
    {GST_QAHWSRC_OUTPUT_DEVICE_SPDIF, "GST_QAHWSRC_OUTPUT_DEVICE_SPDIF",
        "spdif"},
    {GST_QAHWSRC_OUTPUT_DEVICE_FM, "GST_QAHWSRC_OUTPUT_DEVICE_FM", "fm"},
    {GST_QAHWSRC_OUTPUT_DEVICE_AUX_LINE, "GST_QAHWSRC_OUTPUT_DEVICE_AUX_LINE",
        "aux-line"},
    {GST_QAHWSRC_OUTPUT_DEVICE_SPEAKER_SAFE,
        "GST_QAHWSRC_OUTPUT_DEVICE_SPEAKER_SAFE", "speaker-safe"},
    {GST_QAHWSRC_OUTPUT_DEVICE_IP, "GST_QAHWSRC_OUTPUT_DEVICE_IP", "ip"},
    {GST_QAHWSRC_OUTPUT_DEVICE_PROXY, "GST_QAHWSRC_OUTPUT_DEVICE_PROXY",
        "proxy"},
    {GST_QAHWSRC_OUTPUT_DEVICE_DEFAULT, "GST_QAHWSRC_OUTPUT_DEVICE_DEFAULT",
        "default"},
    {0, NULL, NULL},
  };

  if (g_once_init_enter (&output_device_type)) {
    GType tmp =
        g_enum_register_static ("GstQahwSrcOutputDevice", output_device);
    g_once_init_leave (&output_device_type, tmp);
  }

  return (GType) output_device_type;
}

static GType
gst_qahwsink_output_flags_get_type (void)
{
  static volatile gsize output_flags_type = 0;
  static const GFlagsValue output_flags[] = {
    {GST_QAHWSRC_OUTPUT_FLAG_NONE, "GST_QAHWSRC_OUTPUT_FLAG_NONE",
        "none"},
    {GST_QAHWSRC_OUTPUT_FLAG_DIRECT, "GST_QAHWSRC_OUTPUT_FLAG_DIRECT",
        "direct"},
    {GST_QAHWSRC_OUTPUT_FLAG_FAST,
        "GST_QAHWSRC_OUTPUT_FLAG_FAST", "fast"},
    {GST_QAHWSRC_OUTPUT_FLAG_DEEP_BUFFER,
        "GST_QAHWSRC_OUTPUT_FLAG_DEEP_BUFFER", "deep-buffer"},
    {GST_QAHWSRC_OUTPUT_FLAG_COMPRESS_OFFLOAD,
        "GST_QAHWSRC_OUTPUT_FLAG_COMPRESS_OFFLOAD", "compress-offload"},
    {GST_QAHWSRC_OUTPUT_FLAG_NON_BLOCKING,
          "GST_QAHWSRC_OUTPUT_FLAG_NON_BLOCKING",
        "non-blocking"},
    {GST_QAHWSRC_OUTPUT_FLAG_HW_AV_SYNC,
          "GST_QAHWSRC_OUTPUT_FLAG_HW_AV_SYNC",
        "hw-av-sync"},
    {GST_QAHWSRC_OUTPUT_FLAG_TTS,
        "GST_QAHWSRC_OUTPUT_FLAG_TTS", "tts"},
    {GST_QAHWSRC_OUTPUT_FLAG_RAW,
          "GST_QAHWSRC_OUTPUT_FLAG_RAW",
        "raw"},
    {GST_QAHWSRC_OUTPUT_FLAG_SYNC,
          "GST_QAHWSRC_OUTPUT_FLAG_SYNC",
        "sync"},
    {GST_QAHWSRC_OUTPUT_FLAG_IEC958_NONAUDIO,
        "GST_QAHWSRC_OUTPUT_FLAG_IEC958_NONAUDIO", "iec958-nonaudio"},
    {GST_QAHWSRC_OUTPUT_FLAG_VOIP_RX, "GST_QAHWSRC_OUTPUT_FLAG_VOIP_RX",
        "voip-rx"},
    {GST_QAHWSRC_OUTPUT_FLAG_COMPRESS_PASSTHROUGH,
          "GST_QAHWSRC_OUTPUT_FLAG_COMPRESS_PASSTHROUGH",
        "compress-passthrough"},
    {GST_QAHWSRC_OUTPUT_FLAG_DIRECT_PCM,
          "GST_QAHWSRC_OUTPUT_FLAG_DIRECT_PCM",
        "direct-pcm"},
    {0, NULL, NULL},
  };

  if (g_once_init_enter (&output_flags_type)) {
    GType tmp = g_flags_register_static ("GstQahwSrcOutputFlags", output_flags);
    g_once_init_leave (&output_flags_type, tmp);
  }

  return (GType) output_flags_type;
}

static GType
gst_qahwsink_session_get_type (void)
{
  static volatile gsize output_session_type = 0;
  static const GEnumValue session_type[] = {
    {GST_QAHWSRC_OUTPUT_SESSION_DEFAULT,
        "GST_QAHWSRC_OUTPUT_SESSION_DEFAULT", "Compressed-Offload"},
    {GST_QAHWSRC_OUTPUT_SESSION_DEEP_BUFFER,
        "GST_QAHWSRC_OUTPUT_SESSION_DEEP_BUFFER", "Deep-buffer-session"},
    {GST_QAHWSRC_OUTPUT_SESSION_DIRECT_PCM,
        "GST_QAHWSRC_OUTPUT_SESSION_DIRECT_PCM", "PCM-Offload-session"},
    {0, NULL, NULL},
  };

  if (g_once_init_enter (&output_session_type)) {
    GType tmp =
        g_enum_register_static ("GstQahwSrcSessionType", session_type);
    g_once_init_leave (&output_session_type, tmp);
  }

  return (GType) output_session_type;
}
static void
gst_qahwsink_finalize (GObject * object)
{
  GstQahwSink *sink = GST_QAHW_SINK (object);

  g_free (sink->device_address);
  g_free (sink->module_id);
  g_mutex_clear (&sink->qahw_lock);
  g_cond_clear (&sink->qahw_cond);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}


static void
gst_qahwsink_class_init (GstQahwSinkClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
  GstBaseSinkClass *gstbasesink_class;
  GstAudioBaseSinkClass *gstaudiobasesink_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;
  gstbasesink_class = (GstBaseSinkClass *) klass;
  gstaudiobasesink_class = (GstAudioBaseSinkClass *) klass;

  gobject_class->finalize = gst_qahwsink_finalize;
  gobject_class->get_property = gst_qahwsink_get_property;
  gobject_class->set_property = gst_qahwsink_set_property;

  gst_element_class_set_static_metadata (gstelement_class,
      "Audio sink (QAHW)", "Sink/Audio",
      "Output to a sound card via QAHW",
      "Vincent Penquerc'h <vincent.penquerch@collabora.co.uk>");

  gst_element_class_add_static_pad_template (gstelement_class,
      &qahwsink_sink_factory);

  gstbasesink_class->wait_event = GST_DEBUG_FUNCPTR (gst_qahwsink_wait_event);
  gstbasesink_class->event = GST_DEBUG_FUNCPTR (gst_qahwsink_event);

  gstaudiobasesink_class->create_ringbuffer = gst_qahwsink_create_ringbuffer;

  g_object_class_install_property (gobject_class, PROP_MODULE_ID,
      g_param_spec_string ("module-id", "Module ID", "QAHW module ID",
          DEFAULT_PROP_MODULE_ID, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_OUTPUT_DEVICE,
      g_param_spec_enum ("output-device", "Output device", "QAHW output device",
          GST_TYPE_QAHWSINK_OUTPUT_DEVICE, DEFAULT_PROP_OUTPUT_DEVICE,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_VOLUME,
      g_param_spec_double ("volume", "Volume", "Volume",
          0.0f, 10, DEFAULT_PROP_VOLUME,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
          GST_PARAM_MUTABLE_PLAYING));
  g_object_class_install_property (gobject_class, PROP_LEFT_VOLUME,
      g_param_spec_float ("left-volume", "Left Volume", "Left Volume",
          0.0f, 10, DEFAULT_PROP_VOLUME,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
          GST_PARAM_MUTABLE_PLAYING));
  g_object_class_install_property (gobject_class, PROP_RIGHT_VOLUME,
      g_param_spec_float ("right-volume", "Right Volume", "Right Volume",
          0.0f, 10, DEFAULT_PROP_VOLUME,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
          GST_PARAM_MUTABLE_PLAYING));
  g_object_class_install_property (gobject_class, PROP_MUTE,
      g_param_spec_boolean ("mute", "Mute", "mute",
          FALSE, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
          GST_PARAM_MUTABLE_PLAYING));
  g_object_class_install_property (gobject_class, PROP_DEVICE_ADDRESS,
      g_param_spec_string ("device-address", "Device address",
          "QAHW device address", DEFAULT_PROP_DEVICE_ADDRESS,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_KVPAIR_VALUES,
      g_param_spec_string ("kvpair-values", "Key/value pair values",
          "Values to set for the key/value pairs associated with the particular input format",
          DEFAULT_PROP_KVPAIR_VALUES,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_AUDIO_HANDLE,
      g_param_spec_int ("audio-handle", "Audio handle",
          "Audio I/O Handle", 0, G_MAXINT, DEFAULT_PROP_AUDIO_HANDLE,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_OUTPUT_FLAGS,
      g_param_spec_flags ("output-flags", "Output flags", "QAHW output flags",
          GST_TYPE_QAHWSINK_OUTPUT_FLAGS, DEFAULT_PROP_OUTPUT_FLAGS,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_SET_PARAM,
      g_param_spec_string ("set-param", "Set output parameter",
          "Set output parameter to device",
          NULL, G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS |
          GST_PARAM_MUTABLE_PLAYING));
  g_object_class_install_property (gobject_class, PROP_DUMP_FILE,
      g_param_spec_string ("dump-file", "Dump PCM data",
          "Dump pcm data to a file",
          NULL, G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS |
          GST_PARAM_MUTABLE_PLAYING));
  g_object_class_install_property (gobject_class, PROP_KPI_MODE,
      g_param_spec_boolean ("kpi-mode", "kpi-mode", "Enable KPI mode",
          FALSE, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_BT_ADDR,
      g_param_spec_string ("bt-addr", "Bt address",
          "Bt address for APTX",
          NULL, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS ));
  g_object_class_install_property (gobject_class, PROP_USER_MODULE_HANDLE,
      g_param_spec_pointer ("module-handle", "module-handle", "user module handle",
           G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_SESSION_TYPE,
      g_param_spec_enum ("session-type", "Session type", "QAHW session type",
          GST_TYPE_QAHWSINK_OUTPUT_SESSION_TYPE, DEFAULT_PROP_SESSION_TYPE,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
gst_qahw_sink_stream_volume_init (GstStreamVolume * volume)
{
}

static void
gst_qahw_sink_update_volume_unlocked (GstQahwSink * qahw)
{
  if (qahw->stream) {
    if (qahw->muted)
      qahw_out_set_volume (qahw->stream, 0, 0);
    else
      qahw_out_set_volume (qahw->stream, qahw->left_volume / 10.0,
          qahw->right_volume / 10.0);
  }
}

static void
gst_qahwsink_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstQahwSink *qahw;

  qahw = GST_QAHW_SINK (object);

  GST_DEBUG_OBJECT (qahw, "Setting prop: %d", prop_id);
  switch (prop_id) {
    case PROP_MODULE_ID:
      g_free (qahw->module_id);
      qahw->module_id = g_value_dup_string (value);
      if (qahw->module_id == NULL) {
        qahw->module_id = g_strdup (DEFAULT_PROP_MODULE_ID);
      }
      break;
    case PROP_OUTPUT_DEVICE:
      qahw->output_device = g_value_get_enum (value);
      break;
    case PROP_VOLUME:
      GST_QAHW_SINK_LOCK (qahw);
      qahw->left_volume = qahw->right_volume = g_value_get_double (value);
      gst_qahw_sink_update_volume_unlocked (qahw);
      GST_QAHW_SINK_UNLOCK (qahw);
      break;
    case PROP_LEFT_VOLUME:
      GST_QAHW_SINK_LOCK (qahw);
      qahw->left_volume = g_value_get_float (value);
      gst_qahw_sink_update_volume_unlocked (qahw);
      GST_QAHW_SINK_UNLOCK (qahw);
      break;
    case PROP_RIGHT_VOLUME:
      GST_QAHW_SINK_LOCK (qahw);
      qahw->right_volume = g_value_get_float (value);
      gst_qahw_sink_update_volume_unlocked (qahw);
      GST_QAHW_SINK_UNLOCK (qahw);
      break;
    case PROP_MUTE:
      GST_QAHW_SINK_LOCK (qahw);
      qahw->muted = g_value_get_boolean (value);
      gst_qahw_sink_update_volume_unlocked (qahw);
      GST_QAHW_SINK_UNLOCK (qahw);
      break;
    case PROP_DEVICE_ADDRESS:
      g_free (qahw->device_address);
      qahw->device_address = g_value_dup_string (value);
      if (qahw->device_address == NULL) {
        qahw->device_address = g_strdup (DEFAULT_PROP_DEVICE_ADDRESS);
      }
      break;
    case PROP_KVPAIR_VALUES:
      g_free (qahw->kvpair_values);
      qahw->kvpair_values = g_value_dup_string (value);
      if (qahw->kvpair_values == NULL) {
        qahw->kvpair_values = g_strdup (DEFAULT_PROP_KVPAIR_VALUES);
      }
      break;
    case PROP_AUDIO_HANDLE:
      qahw->audio_handle = g_value_get_int (value);
      break;
    case PROP_OUTPUT_FLAGS:
      qahw->output_flags = g_value_get_flags (value);
      GST_DEBUG_OBJECT (qahw, "qahw->output_flags: 0x%x", qahw->output_flags);
      break;
    case PROP_SET_PARAM:
    {
      GST_QAHW_SINK_LOCK (qahw);
      if (qahw->stream) {
        qahw_out_set_parameters (qahw->stream, g_value_get_string (value));
      } else {
        GST_WARNING_OBJECT (qahw, "device is not ready to set parameter");
      }
      GST_QAHW_SINK_UNLOCK (qahw);
      break;
    }
    case PROP_DUMP_FILE:
    {
      proxy_params.acp.file_name = g_value_dup_string (value);
      break;
    }
    case PROP_KPI_MODE:
    {
      qahw->kpi_mode = g_value_get_boolean (value);
      qahw->output_flags = GST_QAHWSRC_OUTPUT_FLAG_FAST;
      printf("Changed kpi_mode to %d\n", qahw->kpi_mode);
      break;
    }
    case PROP_BT_ADDR:
    {
      qahw->btaddr = g_value_dup_string (value);
      break;
    }
    case PROP_USER_MODULE_HANDLE:
    {
        if(!qahw->module){
            qahw->module = g_value_get_pointer (value);
            qahw->user_module = TRUE;
        }
        break;
    }
    case PROP_SESSION_TYPE:
      qahw->session_type = g_value_get_enum (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_qahwsink_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstQahwSink *qahw;

  qahw = GST_QAHW_SINK (object);

  switch (prop_id) {
    case PROP_MODULE_ID:
      g_value_set_string (value, qahw->module_id);
      break;
    case PROP_OUTPUT_DEVICE:
      g_value_set_enum (value, qahw->output_device);
      break;
    case PROP_VOLUME:
      g_value_set_double (value, MAX (qahw->left_volume, qahw->right_volume));
      break;
    case PROP_LEFT_VOLUME:
      g_value_set_float (value, qahw->left_volume);
      break;
    case PROP_RIGHT_VOLUME:
      g_value_set_float (value, qahw->right_volume);
      break;
    case PROP_DEVICE_ADDRESS:
      g_value_set_string (value, qahw->device_address);
      break;
    case PROP_KVPAIR_VALUES:
      g_value_set_string (value, qahw->kvpair_values);
      break;
    case PROP_MUTE:
      g_value_set_boolean (value, qahw->muted);
      break;
    case PROP_AUDIO_HANDLE:
      g_value_set_int (value, qahw->audio_handle);
      break;
    case PROP_OUTPUT_FLAGS:
      g_value_set_flags (value, qahw->output_flags);
      break;
    case PROP_KPI_MODE:
      g_value_set_boolean (value, qahw->kpi_mode);
      break;
    case PROP_BT_ADDR:
      g_value_set_string (value, qahw->btaddr);
      break;
    case PROP_USER_MODULE_HANDLE:
    {
        if(!qahw->module){
            g_value_set_pointer (value, qahw->module);
        }
        break;
    }
    case PROP_SESSION_TYPE:
      g_value_set_enum (value, qahw->session_type);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_qahwsink_init (GstQahwSink * qahwsink)
{
  GST_DEBUG_OBJECT (qahwsink, "initializing qahwsink");

  qahwsink->module_id = g_strdup (DEFAULT_PROP_MODULE_ID);
  qahwsink->output_device = DEFAULT_PROP_OUTPUT_DEVICE;
  qahwsink->left_volume = DEFAULT_PROP_VOLUME;
  qahwsink->right_volume = DEFAULT_PROP_VOLUME;
  qahwsink->device_address = g_strdup (DEFAULT_PROP_DEVICE_ADDRESS);
  qahwsink->kvpair_values = g_strdup (DEFAULT_PROP_KVPAIR_VALUES);
  qahwsink->module = NULL;
  qahwsink->stream = NULL;
  qahwsink->user_module = false;
  qahwsink->audio_handle = DEFAULT_PROP_AUDIO_HANDLE;
  qahwsink->output_flags = DEFAULT_PROP_OUTPUT_FLAGS;
  qahwsink->kpi_mode = false;
  qahwsink->is_aptx = false;
  proxy_params.hdr.riff_id = ID_RIFF;
  proxy_params.hdr.riff_sz = 0;
  proxy_params.hdr.riff_fmt = ID_WAVE;
  proxy_params.hdr.fmt_id = ID_FMT;
  proxy_params.hdr.fmt_sz = 16;
  proxy_params.hdr.audio_format = FORMAT_PCM;
  proxy_params.hdr.num_channels = 2;
  proxy_params.hdr.sample_rate = 44100;
  proxy_params.hdr.byte_rate = proxy_params.hdr.sample_rate * proxy_params.hdr.num_channels * 2;
  proxy_params.hdr.block_align = proxy_params.hdr.num_channels * 2;
  proxy_params.hdr.bits_per_sample = 16;
  proxy_params.hdr.data_id = ID_DATA;
  proxy_params.hdr.data_sz = 0;
  proxy_params.acp.file_name = "/data/pcmdump.pcm";
  g_mutex_init (&qahwsink->qahw_lock);
  g_cond_init (&qahwsink->qahw_cond);

  /* override with a custom clock */
  if (GST_AUDIO_BASE_SINK (qahwsink)->provided_clock)
    gst_object_unref (GST_AUDIO_BASE_SINK (qahwsink)->provided_clock);

  GST_AUDIO_BASE_SINK (qahwsink)->provided_clock = gst_system_clock_obtain ();
}

static GstAudioRingBuffer *
gst_qahwsink_create_ringbuffer (GstAudioBaseSink * absink)
{
  return g_object_new (GST_TYPE_QAHW_RING_BUFFER, NULL);
}

static int
gst_qahwsink_find_flac_streaminfo_offset (GstQahwSink * qahw,
    const guint8 * data, gsize size)
{
  guint8 type;

  if (size >= 5) {
    if (!memcmp (data, "fLaC", 4)) {
      type = data[4] & 0x7f;
      if (type == 0) {
        GST_INFO_OBJECT (qahw, "Found canonical fLaC streaminfo");
        return 4;
      }
    }
  }
  /* flacparse will prepend some data to the actual FLAC bitstream, so we
   * detect this too */
  if (size >= 14) {
    if (!memcmp (data, "\x7f" "FLAC", 5) && !memcmp (data + 9, "fLaC", 4)) {
      type = data[13] & 0x7f;
      if (type == 0) {
        GST_INFO_OBJECT (qahw, "Found flacparse type fLaC streaminfo");
        return 13;
      }
    }
  }
  return -1;
}

static char *
gst_qahw_autodetect_flac_kvpairs (GstQahwSink * qahw, int n_buffers,
    GstBuffer ** buffers)
{
  GstMapInfo map;
  int n;
  guint32 min_block_size, max_block_size;
  guint32 min_frame_size, max_frame_size;
  guint32 bitrate, sample_rate;
  guint8 bps;
  guint32 channels;
  gboolean STREAMINFO_found = FALSE;
  GstBitReader br;
  int offset;

  /* make this one up */
  bitrate = 192000;

  /* find STREAMINFO header */
  for (n = 0; n < n_buffers; ++n) {
    if (!buffers[n])
      continue;
    gst_buffer_map (buffers[n], &map, GST_MAP_READ);
    offset =
        gst_qahwsink_find_flac_streaminfo_offset (qahw, map.data, map.size);
    if (offset >= 0) {
      /* Found STREAMINFO */
      gst_bit_reader_init (&br, map.data, map.size);
      if (offset > 0)
        gst_bit_reader_skip (&br, offset * 8);
      gst_bit_reader_skip (&br, 1 + 7 + 24);    /* header */
      gst_bit_reader_get_bits_uint32 (&br, &min_block_size, 16);
      gst_bit_reader_get_bits_uint32 (&br, &max_block_size, 16);
      gst_bit_reader_get_bits_uint32 (&br, &min_frame_size, 24);
      gst_bit_reader_get_bits_uint32 (&br, &max_frame_size, 24);
      gst_bit_reader_get_bits_uint32 (&br, &sample_rate, 20);
      gst_bit_reader_get_bits_uint8 (&br, &channels, 3);
      gst_bit_reader_get_bits_uint8 (&br, &bps, 5);

      qahw->config.offload_info.bit_width = bps+1;

      STREAMINFO_found = TRUE;
    }
    gst_buffer_unmap (buffers[n], &map);
  }

  if (!STREAMINFO_found)
    return NULL;

  return g_strdup_printf (FLAC_KVPAIR, bitrate, min_block_size, max_frame_size,
      min_block_size, min_frame_size, sample_rate);
}

static char *
gst_qahw_autodetect_vorbis_kvpairs (GstQahwSink * qahw, int n_buffers,
    GstBuffer ** buffers)
{
  GstMapInfo map;
  int n,i;
  gint32 bitrate_upper =0,  bitrate_lower =0, bitrate_normal = 0;
  guint32 sample_rate = 0;
  guint32 block_size, framing = 0, channels;
  guint32 temp;
  gboolean STREAMINFO_found = FALSE;
  GstBitReader br;
  int offset;

  /* find STREAMINFO header */
  for (n = 0; n < n_buffers; ++n) {
    if (!buffers[n])
      continue;
    gst_buffer_map (buffers[n], &map, GST_MAP_READ);
    /*check for identification header is type*/
    if(map.data[0] != 1)
    {
      gst_buffer_unmap (buffers[n], &map);
      continue;
    }
    /*if (!memcmp (&map.data[1], "vorbis", 5) || map.size < 6) {
      gst_buffer_unmap (buffers[n], &map);
      return NULL;
    }*/

    gst_bit_reader_init (&br, map.data, map.size);

    /* skip version no */
    gst_bit_reader_skip (&br,64);

    /* Read Channels */
    gst_bit_reader_get_bits_uint32 (&br,&channels,32);

    /*Read sample rate*/
    gst_bit_reader_get_bits_uint32 (&br,&temp,32);
    sample_rate |= temp & 0x000000ff;
    sample_rate = sample_rate << 8;
    temp = temp >> 8;
    sample_rate |= temp & 0x000000ff;
    sample_rate = sample_rate << 8;
    temp = temp >> 8;
    sample_rate |= temp & 0x000000ff;
    sample_rate = sample_rate << 8;
    temp = temp >> 8;
    sample_rate |= temp;

     /*Read bitrate upper*/
    gst_bit_reader_get_bits_uint32 (&br,&bitrate_upper,32);

    /*Read bitrate normal*/
    gst_bit_reader_get_bits_uint32 (&br,&temp,32);
    bitrate_normal |= temp & 0x000000ff;
    bitrate_normal = bitrate_normal << 8;
    temp = temp >> 8;
    bitrate_normal |= temp & 0x000000ff;
    bitrate_normal = bitrate_normal << 8;
    temp = temp >> 8;
    bitrate_normal |= temp & 0x000000ff;
    bitrate_normal = bitrate_normal << 8;
    temp = temp >> 8;
    bitrate_normal |= temp;

    /* Read bitrate lower*/
    gst_bit_reader_get_bits_uint32 (&br,&bitrate_lower,32);

    /*Read frsming flag*/
    gst_bit_reader_get_bits_uint16 (&br,&temp,16);
    framing = temp & 0x000000ff;
    framing = 0;

    GST_DEBUG_OBJECT(qahw, "channels bitrate_normal , sample_rate, framing n %d %d %d %x %d\n",
                                              channels, bitrate_normal , sample_rate, framing, n);
    gst_buffer_unmap (buffers[n], &map);
    STREAMINFO_found = TRUE;
    break;
  }

  if (!STREAMINFO_found)
    return NULL;

   return g_strdup_printf (VORBIS_KVPAIR, bitrate_normal , sample_rate, framing);
}

static char *
gst_qahw_autodetect_alac_kvpairs (GstQahwSink * qahw, int n_buffers,
    GstBuffer ** buffers)
{
  GstMapInfo map;
  guint8 alac_compatible_version, alac_bit_depth;
  guint8 alac_kb, alac_mb, alac_pb;
  guint8 alac_num_channels;
  guint16 alac_max_run;
  guint8 alac_header[4];
  guint32 alac_avg_bit_rate;
  guint32 alac_channel_layout_tag;
  guint32 alac_frame_length, alac_max_frame_bytes;
  guint32 alac_sampling_rate;
  guint32 avg_bit_rate, sample_rate;
  gboolean codec_data_found = TRUE;
  GstBitReader br;
  int offset;

  if (!buffers[0])
    return NULL;

  gst_buffer_map (buffers[0], &map, GST_MAP_READ);
  gst_bit_reader_init (&br, map.data, map.size);
  gst_bit_reader_skip (&br, 32);
  gst_bit_reader_get_bits_uint8 (&br, &alac_header[0], 8);
  gst_bit_reader_get_bits_uint8 (&br, &alac_header[1], 8);
  gst_bit_reader_get_bits_uint8 (&br, &alac_header[2], 8);
  gst_bit_reader_get_bits_uint8 (&br, &alac_header[3], 8);

  if (!memcmp (alac_header, "alac", 4)) {
    GST_DEBUG_OBJECT(qahw, "Found ALAC data\n");
    /* Found codec_data */
    gst_bit_reader_skip (&br, 32);
    gst_bit_reader_get_bits_uint32 (&br, &alac_frame_length, 32);
    gst_bit_reader_get_bits_uint8 (&br, &alac_compatible_version, 8);
    gst_bit_reader_get_bits_uint8 (&br, &alac_bit_depth, 8);
    gst_bit_reader_get_bits_uint8 (&br, &alac_pb, 8);
    gst_bit_reader_get_bits_uint8 (&br, &alac_mb, 8);
    gst_bit_reader_get_bits_uint8 (&br, &alac_kb, 8);
    gst_bit_reader_get_bits_uint8 (&br, &alac_num_channels, 8);
    gst_bit_reader_get_bits_uint16 (&br, &alac_max_run, 16);
    gst_bit_reader_get_bits_uint32 (&br, &alac_max_frame_bytes, 32);
    gst_bit_reader_get_bits_uint32 (&br, &alac_avg_bit_rate, 32);
    gst_bit_reader_get_bits_uint32 (&br, &alac_sampling_rate, 32);

    avg_bit_rate = alac_avg_bit_rate;
    sample_rate = alac_sampling_rate;
    alac_channel_layout_tag = 0;

    GST_DEBUG_OBJECT (qahw, "alac_frame_length %.8x %d\n", alac_frame_length, alac_frame_length);
    GST_DEBUG_OBJECT (qahw, "alac_compatible_version %.2x %d\n", alac_compatible_version, alac_compatible_version);
    GST_DEBUG_OBJECT (qahw, "alac_bit_depth %.2x %d\n", alac_bit_depth, alac_bit_depth);
    GST_DEBUG_OBJECT (qahw, "alac_pb %.2x %d\n", alac_pb, alac_pb);
    GST_DEBUG_OBJECT (qahw, "alac_mb %.2x %d\n", alac_mb, alac_mb);
    GST_DEBUG_OBJECT (qahw, "alac_kb %.2x %d\n", alac_kb, alac_kb);
    GST_DEBUG_OBJECT (qahw, "alac_num_channels %.2x %d\n", alac_num_channels, alac_num_channels);
    GST_DEBUG_OBJECT (qahw, "alac_max_run %.4x %d\n", alac_max_run, alac_max_run);
    GST_DEBUG_OBJECT (qahw, "alac_max_frame_bytes %.8x %d\n", alac_max_frame_bytes, alac_max_frame_bytes);
    GST_DEBUG_OBJECT (qahw, "alac_avg_bit_rate %.8x %d\n", alac_avg_bit_rate, alac_avg_bit_rate);
    GST_DEBUG_OBJECT (qahw, "alac_sampling_rate %.8x %d\n", alac_sampling_rate, alac_sampling_rate);
    codec_data_found = TRUE;
  }
  gst_buffer_unmap (buffers[0], &map);

  if (!codec_data_found)
    return NULL;

  return g_strdup_printf (ALAC_KVPAIR, alac_avg_bit_rate, alac_bit_depth, alac_channel_layout_tag, alac_compatible_version,
                 alac_frame_length, alac_kb, alac_max_frame_bytes, alac_max_run, alac_mb,
                 alac_num_channels, alac_pb, alac_sampling_rate, avg_bit_rate, sample_rate);
}

static char *
gst_qahw_autodetect_kvpairs (GstQahwSink * qahw, GstCaps * caps)
{
  GstStructure *structure;
  const char *media_type;
  char *(*reader) (GstQahwSink *, int, GstBuffer **) = NULL;
  const GValue *streamheader, *header_value;
  int n;
  GArray *array;
  GstBuffer **headers;
  char *ret;
  const GValue *value = NULL;
  gint len;

  if (!caps)
    return NULL;
  structure = gst_caps_get_structure (caps, 0);
  media_type = gst_structure_get_name (structure);
  if (!strcmp (media_type, "audio/x-flac")) {
    reader = gst_qahw_autodetect_flac_kvpairs;
  } else if (!strcmp (media_type, "audio/x-vorbis")) {
    reader = gst_qahw_autodetect_vorbis_kvpairs;
  } else if (!strcmp (media_type, "audio/x-alac")) {
    reader = gst_qahw_autodetect_alac_kvpairs;
  }
  if (!reader)
    return NULL;

  if ( (!strcmp (media_type, "audio/x-flac")) ||
       (!strcmp (media_type, "audio/x-vorbis")) ) {
    streamheader = gst_structure_get_value (structure, "streamheader");
    if (!streamheader)
      return NULL;
    if (G_VALUE_TYPE (streamheader) != GST_TYPE_ARRAY)
      return NULL;

    array = g_value_peek_pointer (streamheader);
    headers = g_malloc (array->len * sizeof (GstBuffer *));
    for (n = 0; n < array->len; ++n) {
      header_value = &g_array_index (array, GValue, n);
      if (G_VALUE_TYPE (header_value) == GST_TYPE_BUFFER) {
        headers[n] = g_value_peek_pointer (header_value);
      } else {
        headers[n] = NULL;
      }
    }
    len = array->len;
  } else if (!strcmp (media_type, "audio/x-alac")) {
    len = 1;
    value = gst_structure_get_value (structure, "codec_data");
    if (value != NULL) {
      headers = g_malloc (sizeof (GstBuffer *));

      if (G_VALUE_TYPE (value) == GST_TYPE_BUFFER) {
        headers[0] = gst_value_get_buffer (value);
      } else {
        headers[0] = NULL;
      }
    }
    else
      return NULL;
  }

  ret = (*reader) (qahw, len, headers);
  g_free (headers);
  return ret;
}

static gboolean
qahwsink_parse_spec (GstQahwSink * qahw, GstAudioRingBufferSpec * spec)
{
  gint bitwidth = 16;

  if (GST_AUDIO_INFO_WIDTH (&spec->info) > 0)
    bitwidth = GST_AUDIO_INFO_WIDTH (&spec->info);

  memset (&qahw->config, 0, sizeof (qahw->config));
  switch (spec->type) {
    case GST_AUDIO_RING_BUFFER_FORMAT_TYPE_RAW:
      if (qahw->kpi_mode == false) {
        if(qahw->session_type == GST_QAHWSRC_OUTPUT_SESSION_DEEP_BUFFER) {
          qahw->output_flags = GST_QAHWSRC_OUTPUT_FLAG_DIRECT | GST_QAHWSRC_OUTPUT_FLAG_DEEP_BUFFER;
        } else if (qahw->session_type == GST_QAHWSRC_OUTPUT_SESSION_DIRECT_PCM) {
          qahw->output_flags = GST_QAHWSRC_OUTPUT_FLAG_DIRECT | GST_QAHWSRC_OUTPUT_FLAG_DIRECT_PCM;
        } else {
          qahw->output_flags = GST_QAHWSRC_OUTPUT_FLAG_DEEP_BUFFER | GST_QAHWSRC_OUTPUT_FLAG_DIRECT | GST_QAHWSRC_OUTPUT_FLAG_DIRECT_PCM;
        }
        GST_DEBUG_OBJECT (qahw, "qahw->output_flags: 0x%x", qahw->output_flags);
      }
      switch (GST_AUDIO_INFO_FORMAT (&spec->info)) {
        case GST_AUDIO_FORMAT_S16LE:
          qahw->config.format = AUDIO_FORMAT_PCM_16_BIT;
          break;
        case GST_AUDIO_FORMAT_S16BE:
          qahw->config.format = AUDIO_FORMAT_PCM_16_BIT;
          break;
        case GST_AUDIO_FORMAT_S24LE:
          qahw->config.format = AUDIO_FORMAT_PCM_24_BIT_PACKED;
          break;
        case GST_AUDIO_FORMAT_S24BE:
          qahw->config.format = AUDIO_FORMAT_PCM_24_BIT_PACKED;
          break;
        case GST_AUDIO_FORMAT_S32LE:
          qahw->config.format = AUDIO_FORMAT_PCM_32_BIT;
          break;
        case GST_AUDIO_FORMAT_S32BE:
          qahw->config.format = AUDIO_FORMAT_PCM_32_BIT;
          break;
        default:
          GST_ELEMENT_ERROR (qahw, STREAM, WRONG_TYPE, (NULL),
              ("Unsupported format: %u", GST_AUDIO_INFO_FORMAT (&spec->info)));
          goto error;
      }
      break;
    case GST_AUDIO_RING_BUFFER_FORMAT_TYPE_VORBIS:
      qahw->config.format = AUDIO_FORMAT_VORBIS;
      break;
    case GST_AUDIO_RING_BUFFER_FORMAT_TYPE_FLAC:
      qahw->config.format = AUDIO_FORMAT_FLAC;
      break;
    case GST_AUDIO_RING_BUFFER_FORMAT_TYPE_MPEG:
      qahw->config.format = AUDIO_FORMAT_MP3;
      break;
    case GST_AUDIO_RING_BUFFER_FORMAT_TYPE_ALAC:
      qahw->config.format = AUDIO_FORMAT_ALAC;
      break;
    case GST_AUDIO_RING_BUFFER_FORMAT_TYPE_WMA:
      qahw->is_aptx=true;
      qahw->config.format = AUDIO_FORMAT_APTX;
      GST_DEBUG_OBJECT (qahw, "Setting Audio format to APTX");
      break;
    case GST_AUDIO_RING_BUFFER_FORMAT_TYPE_MPEG4_AAC:
    case GST_AUDIO_RING_BUFFER_FORMAT_TYPE_MPEG2_AAC:
      qahw->config.format = AUDIO_FORMAT_AAC_ADTS_HE_V2;
      qahw->config.offload_info.format = AUDIO_FORMAT_AAC_ADTS_HE_V2;
      break;
    case GST_AUDIO_RING_BUFFER_FORMAT_TYPE_MPEG4_AAC_RAW:
    case GST_AUDIO_RING_BUFFER_FORMAT_TYPE_MPEG2_AAC_RAW:
      qahw->config.format = AUDIO_FORMAT_AAC_HE_V2;
      qahw->config.offload_info.format = AUDIO_FORMAT_AAC_HE_V2;
      break;
    default:
      GST_ELEMENT_ERROR (qahw, STREAM, WRONG_TYPE, (NULL),
          ("Unsupported format: %d", spec->type));
      goto error;
  }
  qahw->config.sample_rate = GST_AUDIO_INFO_RATE (&spec->info);
  qahw->config.channel_mask =
      audio_channel_out_mask_from_count (GST_AUDIO_INFO_CHANNELS (&spec->info));
  qahw->config.offload_info.channel_mask = qahw->config.channel_mask;
  qahw->config.offload_info.sample_rate = qahw->config.sample_rate;
  qahw->config.offload_info.format = qahw->config.format;
  qahw->config.offload_info.bit_width = bitwidth;

  GST_INFO_OBJECT (qahw, "Parsed spec: %u Hz, %d channels",
      qahw->config.sample_rate, GST_AUDIO_INFO_CHANNELS (&spec->info));

  qahw->config.offload_info.version = AUDIO_OFFLOAD_INFO_VERSION_CURRENT;
  qahw->config.offload_info.size = sizeof (audio_offload_info_t);

  return TRUE;

  /* ERRORS */
error:
  {
    return FALSE;
  }
}

static gboolean
gst_qahw_ring_buffer_open_device (GstAudioRingBuffer * rb)
{
  GstQahwRingBuffer *self = GST_QAHW_RING_BUFFER (rb);
  GstQahwSink *qahw = GST_QAHW_SINK (GST_OBJECT_PARENT (self));

  if(!qahw->module){
    GST_LOG_OBJECT (qahw, "Loading qahw module");
    qahw->module = qahw_load_module (qahw->module_id);
    if (!qahw->module) {
      GST_ELEMENT_ERROR (qahw, LIBRARY, INIT, (NULL), ("Could not load module."));
      return FALSE;
    }
  }

  GST_LOG_OBJECT (qahw, "Opened device 0x%x %s", qahw->module, qahw->module_id);

  return TRUE;
}

static gboolean
gst_qahw_ring_buffer_close_device (GstAudioRingBuffer * rb)
{
  GstQahwRingBuffer *self = GST_QAHW_RING_BUFFER (rb);
  GstQahwSink *qahw = GST_QAHW_SINK (GST_OBJECT_PARENT (self));
  int err;

  GST_OBJECT_LOCK (qahw);
  if (qahw->module && !qahw->user_module) {
    err = qahw_unload_module (qahw->module);
    if (err)
      GST_ELEMENT_ERROR (qahw, LIBRARY, SHUTDOWN, (NULL),
          ("Failed to unload module: %d", err));
    qahw->module = NULL;
  }
  GST_OBJECT_UNLOCK (qahw);

  return TRUE;
}

static gboolean
gst_qahw_ring_buffer_acquire (GstAudioRingBuffer * rb,
    GstAudioRingBufferSpec * spec)
{
  GstStructure *structure;
  const GValue *value;
  GstQahwRingBuffer *self = GST_QAHW_RING_BUFFER (rb);
  GstQahwSink *qahw = GST_QAHW_SINK (GST_OBJECT_PARENT (self));
  gint err;
  audio_output_flags_t flags;
  char *kvpair;
  qahw->writeCount = 0;
  guint bpf = GST_AUDIO_INFO_BPF (&spec->info);
  guint rate = GST_AUDIO_INFO_RATE (&spec->info);
  int ret = 0;

  if (!qahwsink_parse_spec (qahw, spec))
    goto spec_parse;

  if (qahw->kvpair_values && *qahw->kvpair_values) {
    kvpair = read_kvpair (qahw->kvpair_values, qahw->config.format);
  } else {
    kvpair = gst_qahw_autodetect_kvpairs (qahw, spec->caps);
  }

  //Read sample rate from codec data for AAC format
  if ((qahw->config.format == AUDIO_FORMAT_AAC_HE_V2) ||
      (qahw->config.format == AUDIO_FORMAT_AAC_ADTS_HE_V2)){
    structure = gst_caps_get_structure (spec->caps, 0);
    value = gst_structure_get_value (structure, "codec_data");
    if (value) {
      GstBuffer *buf = gst_value_get_buffer (value);

      if (buf) {
        GstMapInfo map;
        guint sr_idx;

        gst_buffer_map (buf, &map, GST_MAP_READ);

        sr_idx = ((map.data[0] & 0x07) << 1) | ((map.data[1] & 0x80) >> 7);
        qahw->config.sample_rate = aac_sample_rates[sr_idx];
        qahw->config.offload_info.sample_rate = aac_sample_rates[sr_idx];
        gst_buffer_unmap (buf, &map);

        printf("Sample rate retrieved: %d\n", qahw->config.sample_rate );
      }
    }
  }

  flags = qahw->output_flags;
  if (qahw->output_device & AUDIO_DEVICE_OUT_ALL_A2DP)
        qahw->output_device = AUDIO_DEVICE_OUT_PROXY;

   if (qahw->is_aptx==true){
      struct qahw_aptx_dec_param aptx_params;
      qahw_param_payload payload;
      qahw_param_id param_id;
      param_id = QAHW_PARAM_APTX_DEC;
      int rc=0;

      flags = GST_QAHWSRC_OUTPUT_FLAG_NONE;
      flags |= GST_QAHWSRC_OUTPUT_FLAG_NON_BLOCKING;
      flags |= GST_QAHWSRC_OUTPUT_FLAG_COMPRESS_OFFLOAD;

      parse_aptx_dec_bd_addr(qahw->btaddr, &aptx_params);
      payload.aptx_params = aptx_params;
      rc = qahw_set_param_data(qahw->module, param_id, &payload);
      GST_DEBUG_OBJECT (qahw, "Inside is_aptx block rc %d",rc);
   }
  err =
      qahw_open_output_stream (qahw->module, qahw->audio_handle,
      qahw->output_device, flags, &qahw->config, &qahw->stream,
      qahw->device_address);
  if (err)
    goto open_failed;

  if (qahw->kpi_mode == true) {
    fd_latency_node = fopen(LATENCY_NODE, "r+");
    if (fd_latency_node) {
      ret = fwrite(LATENCY_NODE_INIT_STR, sizeof(LATENCY_NODE_INIT_STR), 1, fd_latency_node);
      if (ret<1)
        GST_DEBUG_OBJECT(qahw, "error(%d) writing to debug node!", ret);
      fprintf(stderr, "error(%d) writing to debug node!", ret);
      fflush(fd_latency_node);
    } else {
      GST_DEBUG_OBJECT(qahw, "debug node(%s) open failed!", LATENCY_NODE);
      fprintf(stderr, "debug node(%s) open failed!", LATENCY_NODE);
      return -1;
    }
  }

  //if (qahw->kpi_mode == true) {
  //    measure_kpi_values(qahw, qahw->output_flags & AUDIO_OUTPUT_FLAG_COMPRESS_OFFLOAD);
  //    //rc = qahw_close_output_stream(qahw->stream);
  //    /* if (rc) {
  //        GST_LOG_OBJECT(qahw, "stream %d: could not close output stream %d, error - %d \n", params->stream_index, rc);
  //        fprintf(stderr, "stream %d: could not close output stream %d, error - %d \n", params->stream_index, rc);
  //    } */
  //    return 0;
  //}

  spec->segsize = qahw_out_get_buffer_size (qahw->stream);
  gst_base_sink_set_blocksize (GST_BASE_SINK (qahw), spec->segsize);
  GST_DEBUG_OBJECT (qahw, "Opened output stream, segment size %d",
      spec->segsize);

  if(qahw->kpi_mode == true) {
    bytes_wanted = spec->segsize;
    data = (char *) calloc (1, bytes_wanted);
    if (data == NULL) {
      GST_DEBUG_OBJECT(qahw, "calloc failed!!\n");
      fprintf(stderr, "calloc failed!!\n");
      return -ENOMEM;
    }
  }

  if (spec->type == GST_AUDIO_RING_BUFFER_FORMAT_TYPE_RAW) {
    spec->latency_time = gst_util_uint64_scale (G_USEC_PER_SEC, spec->segsize,
        bpf * rate);
#if 0
    spec->segtotal = spec->buffer_time / spec->latency_time;
    if (spec->segtotal * spec->latency_time < spec->buffer_time)
      spec->segtotal += 1;
#else
    spec->segtotal = 1;
#endif
  } else {
    /* for encoded format, we have no idea what a segment means in time,
     * so lets set it at 3 seconds.
     */
    spec->segtotal = 1;
  }

  if (kvpair && qahw->kpi_mode == false) {
    GST_DEBUG_OBJECT (qahw, "Setting kvpair: %s", kvpair);
    qahw_out_set_parameters (qahw->stream, kvpair);
    g_free (kvpair);
  }

  qahw->can_write = TRUE;
  qahw->paused = FALSE;
  qahw->drained = FALSE;

  /* Notify stream creation */
  {
    GstStructure *s;
    GstEvent *event;

    GST_DEBUG_OBJECT (qahw, "Pushing QahwStreamCreated event");

    s = gst_structure_new ("QahwStreamCreated", "io-handle", G_TYPE_INT,
        qahw->audio_handle, "output-device", G_TYPE_UINT, qahw->output_device,
        NULL);
    event = gst_event_new_custom (GST_EVENT_CUSTOM_UPSTREAM, s);
    gst_pad_push_event (GST_BASE_SINK_PAD (qahw), event);
  }

  if(qahw->kpi_mode == false) {
    gst_qahw_sink_update_volume_unlocked (qahw);
    qahw_out_set_callback (qahw->stream, async_callback, gst_object_ref (qahw));
  }

  if (qahw->output_device & AUDIO_DEVICE_OUT_PROXY)
  {
    proxy_params.acp.qahw_mod_handle = qahw->module;
    proxy_params.acp.handle = stream_handle;
    stream_handle--;
    proxy_params.acp.input_device = AUDIO_DEVICE_IN_PROXY;
    proxy_params.acp.flags = AUDIO_INPUT_FLAG_NONE;
    proxy_params.acp.config.channel_mask = audio_channel_in_mask_from_count(AFE_PROXY_CHANNEL_COUNT);
    proxy_params.acp.config.sample_rate = AFE_PROXY_SAMPLING_RATE;
    proxy_params.acp.config.format = AUDIO_FORMAT_PCM_16_BIT;
    proxy_params.acp.kStreamName = "input_stream";
    proxy_params.acp.kInputSource = AUDIO_SOURCE_UNPROCESSED;
    proxy_params.acp.thread_exit = false;

    err = pthread_create(&proxy_thread, NULL, proxy_read, (void *)&proxy_params);
    if (!err)
      proxy_thread_active = true;
  }

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
        ("Failed to open output stream: %d", err));
    return FALSE;
  }
}

static gboolean
gst_qahw_ring_buffer_release (GstAudioRingBuffer * rb)
{
  GstQahwRingBuffer *self = GST_QAHW_RING_BUFFER (rb);
  GstQahwSink *qahw = GST_QAHW_SINK (GST_OBJECT_PARENT (self));
  GstStructure *s;
  GstEvent *event;
  int err;

  s = gst_structure_new_empty ("QahwStreamDisposed");
  event = gst_event_new_custom (GST_EVENT_CUSTOM_UPSTREAM, s);
  gst_pad_push_event (GST_BASE_SINK_PAD (qahw), event);

  if (proxy_thread_active) {
       /*
        * DSP gives drain ack for last buffer which will close proxy thread before
        * app reads last buffer. So add sleep before exiting proxy thread to read
        * last buffer of data. This is not a calculated value.
        */
        usleep(500000);
        proxy_params.acp.thread_exit = true;
        GST_ERROR_OBJECT (qahw, "wait for proxy thread exit\n");
        pthread_join(proxy_thread, NULL);
  }

#ifdef ENABLE_HW8009
  qahw_out_set_parameters(qahw->stream, "dual_mono=false");
#else
  qahw_set_parameters(qahw->stream, "stereo_as_dual_mono=false");
#endif

  err = qahw_out_standby (qahw->stream);
  if (err) {
    GST_ERROR_OBJECT (qahw, "Failed to set standby mode: %d", err);
  }
  err = qahw_close_output_stream (qahw->stream);
  if (err) {
    GST_ERROR_OBJECT (qahw, "Failed to close output stream: %d", err);
  }
  qahw->stream = NULL;

  return TRUE;
}

static guint
gst_qahw_ring_buffer_commit (GstAudioRingBuffer * rb, guint64 * sample,
    guint8 * data, gint in_samples, gint out_samples, gint * accum)
{
  GstQahwRingBuffer *self = GST_QAHW_RING_BUFFER (rb);
  GstQahwSink *qahw = GST_QAHW_SINK (GST_OBJECT_PARENT (self));
  qahw_out_buffer_t buffer;
  size_t total_written = 0;
  ssize_t written = 0;
  guint bpf = MAX (1, GST_AUDIO_INFO_BPF (&rb->spec.info));
  guint length, to_write;
  int ret = 0;
  length = in_samples * bpf;

  //if (qahw->kpi_mode == true) {
      //measure_kpi_values(qahw, qahw->output_flags & AUDIO_OUTPUT_FLAG_COMPRESS_OFFLOAD);
      //rc = qahw_close_output_stream(qahw->stream);
      /* if (rc) {
          GST_LOG_OBJECT(qahw, "stream %d: could not close output stream %d, error - %d \n", params->stream_index, rc);
          fprintf(stderr, "stream %d: could not close output stream %d, error - %d \n", params->stream_index, rc);
      } */
      //return 0;
  //}

  if (qahw->kpi_mode == true && pb_count < 64) {
    if (!bytes_remaining) {
      bytes_remaining = write_length = bytes_wanted;
    }
    if (pb_count == 0) {
      ret = clock_gettime(CLOCK_REALTIME, &ts_cold);
      if (ret) {
        GST_DEBUG_OBJECT(qahw, "error(%d) fetching start time for cold latency", ret);
        fprintf(stderr, "error(%d) fetching start time for cold latency", ret);
        return 0;
      }
    } else if (pb_count == 16) {
      int *d = (int *)data;
      d[0] = 0x01010000;
      ret = clock_gettime(CLOCK_REALTIME, &ts_cont);
      if (ret) {
        GST_DEBUG_OBJECT(qahw, "error(%d) fetching start time for continuous latency", ret);
        fprintf(stderr, "error(%d) fetching start time for continuous latency", ret);
        return 0;
      }
    }
    offset = write_length - bytes_remaining;
    bytes_written = write_to_hal(qahw, data+offset, bytes_remaining, gst_object_ref (qahw));
    bytes_remaining -= bytes_written;
    GST_DEBUG_OBJECT(qahw, "bytes_written %zd, bytes_remaining %zd\n",
    bytes_written, bytes_remaining);

    if (pb_count == 16) {
      int *i = (int *)data;
      i[0] = 0x00000000;
    }
    pb_count++;
    return in_samples;
  } else if (qahw->kpi_mode == true && pb_count == 64) {
    char latency_buf[200] = {0};
    fread((void *) latency_buf, 100, 1, fd_latency_node);
    fclose(fd_latency_node);
    sscanf(latency_buf, " %llu,%llu,%*llu,%*llu,%llu,%llu", &scold, &uscold, &scont, &uscont);
    tcold = scold*1000 - ((uint64_t)ts_cold.tv_sec)*1000 + uscold/1000 - ((uint64_t)ts_cold.tv_nsec)/1000000;
    tcont = scont*1000 - ((uint64_t)ts_cont.tv_sec)*1000 + uscont/1000 - ((uint64_t)ts_cont.tv_nsec)/1000000;
    GST_DEBUG_OBJECT(qahw, "\n itr count %d\n", pb_count);
    GST_DEBUG_OBJECT(qahw, "\n values from debug node %s\n", latency_buf);
    GST_DEBUG_OBJECT(qahw, " cold latency %llums, continuous latency %llums,\n", tcold, tcont);
    GST_DEBUG_OBJECT(qahw, " **Note: please add DSP Pipe/PP latency numbers to this, for final latency values\n");
    printf(" cold latency %llums, continuous latency %llums,\n", tcold, tcont);
    printf(" **Note: please add DSP Pipe/PP latency numbers to this, for final latency values\n");
    pb_count++;
    return 0;
  }

  /* make sure the ringbuffer is started */
  if (G_UNLIKELY (g_atomic_int_get (&rb->state) !=
          GST_AUDIO_RING_BUFFER_STATE_STARTED)) {
    /* see if we are allowed to start it */
    if (G_UNLIKELY (g_atomic_int_get (&rb->may_start) == FALSE)) {
      GST_WARNING_OBJECT (qahw, "May not start ringbuffer");
      return 0;
    }

    GST_DEBUG_OBJECT (qahw, "start ringbuffer!");
    if (!gst_audio_ring_buffer_start (rb)) {
      GST_WARNING_OBJECT (qahw, "Starting ringbuffer failed");
      return 0;
    }
  }

  GST_LOG_OBJECT (qahw, "received %d audio samples buffer in %u bytes",
      in_samples, length);
  GST_MEMDUMP_OBJECT (qahw, "commiting", data, length);

  GST_QAHW_SINK_LOCK (qahw);
  while (length > 0) {
    to_write = length > rb->spec.segsize ? rb->spec.segsize : length;
    if(qahw->can_write) {
      GST_DEBUG_OBJECT (qahw, "running: %d paused: %d", qahw->running, qahw->paused);
      if (!qahw->running || qahw->paused) {
        break;
      }
      buffer.buffer = data + *accum;
      buffer.bytes = to_write;
      buffer.offset = 0;
      written = qahw_out_write (qahw->stream, &buffer);
      GST_TRACE_OBJECT (qahw, "written %u bytes", written);
      if (written < 0) {
        GST_ERROR_OBJECT (qahw, "Error writing to HAL: %zd", written);
        goto write_error;
      }
      length -= written;
      *accum += written;
      total_written += written;
    }
    else {
      GST_DEBUG_OBJECT (qahw, "Still waiting for async_callback...");
      GST_DEBUG_OBJECT (qahw, "written: %d to_write: %d", written, to_write);
      GST_DEBUG_OBJECT (qahw, "running: %d paused: %d", qahw->running, qahw->paused);
      if (!qahw->running || qahw->paused) {
        break;
      }
    }

    if (written < to_write) {
      qahw->can_write = FALSE;
      while (!qahw->can_write && qahw->running && !qahw->paused) {
        GST_DEBUG_OBJECT (qahw, "Short write, waiting");
        GST_QAHW_SINK_WAIT (qahw);
        GST_DEBUG_OBJECT (qahw, "Wait over");
      }
    }
  }
  GST_QAHW_SINK_UNLOCK (qahw);
  if(qahw->writeCount == 1){
#ifdef ENABLE_HW8009
    qahw_out_set_parameters(qahw->stream, "dual_mono=true");
#else
    qahw_set_parameters(qahw->stream, "stereo_as_dual_mono=true");
#endif
  }
  qahw->writeCount++;
  return total_written / bpf;

write_error:
  GST_QAHW_SINK_UNLOCK (qahw);
  return in_samples;
}

static GstFlowReturn
gst_qahwsink_event (GstBaseSink * bsink, GstEvent * event)
{
  GstQahwSink *qahw = GST_QAHW_SINK (bsink);

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_STREAM_START:
      GST_QAHW_SINK_LOCK (qahw);
      qahw->drained = FALSE;
      GST_QAHW_SINK_UNLOCK (qahw);
      break;
    default:
      break;
  }

  return GST_BASE_SINK_CLASS (parent_class)->event (bsink, event);
}

static GstFlowReturn
gst_qahwsink_wait_event (GstBaseSink * bsink, GstEvent * event)
{
  GstQahwSink *qahw = GST_QAHW_SINK (bsink);
  int err;

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_EOS:
      if(qahw->output_flags & GST_QAHWSRC_OUTPUT_FLAG_NON_BLOCKING)
      {
        GST_QAHW_SINK_LOCK (qahw);
        GST_DEBUG_OBJECT (qahw, "Draining");
        qahw->drained = FALSE;
        err = qahw_out_drain (qahw->stream, QAHW_DRAIN_ALL);
        if (err)
          GST_ERROR ("Error calling qahw_out_drain: %d", err);
        GST_DEBUG_OBJECT (qahw, "Waiting for drained");
        while (!qahw->drained && qahw->running && !qahw->paused)
          GST_QAHW_SINK_WAIT (qahw);
        GST_DEBUG_OBJECT (qahw, "Drained");
        GST_QAHW_SINK_UNLOCK (qahw);
      }
      break;
    default:
      break;
  }

  return GST_BASE_SINK_CLASS (parent_class)->wait_event (bsink, event);
}

static int
async_callback (qahw_stream_callback_event_t event, void *param, void *cookie)
{
  GstQahwSink *qahw = GST_QAHW_SINK (cookie);

  switch (event) {
    case QAHW_STREAM_CBK_EVENT_WRITE_READY:
      GST_DEBUG_OBJECT (qahw, "QAHW_STREAM_CBK_EVENT_WRITE_READY");
      //GST_QAHW_SINK_LOCK (qahw);
      qahw->can_write = TRUE;
      GST_QAHW_SINK_BROADCAST (qahw);
      //GST_QAHW_SINK_UNLOCK (qahw);
      break;
    case QAHW_STREAM_CBK_EVENT_DRAIN_READY:
      GST_DEBUG_OBJECT (qahw, "QAHW_STREAM_CBK_EVENT_DRAIN_READY");
      GST_QAHW_SINK_LOCK (qahw);
      qahw->drained = TRUE;
      GST_QAHW_SINK_BROADCAST (qahw);
      GST_QAHW_SINK_UNLOCK (qahw);
    default:
      break;
  }
  return 0;
}


static guint
gst_qahw_ring_buffer_delay (GstAudioRingBuffer * rb)
{
  GstQahwRingBuffer *self = GST_QAHW_RING_BUFFER (rb);
  GstQahwSink *qahw = GST_QAHW_SINK (GST_OBJECT_PARENT (self));
  GstAudioBaseSink *absink = GST_AUDIO_BASE_SINK (qahw);
  guint delay = 0;
  guint32 delay_ms = 0;

  GST_QAHW_SINK_LOCK (qahw);
  if (qahw->stream)
    delay_ms = qahw_out_get_latency (qahw->stream);
  GST_QAHW_SINK_UNLOCK (qahw);

  if (delay_ms == 0)
    return 0;

  GST_OBJECT_LOCK (qahw);
  delay = delay_ms * GST_AUDIO_INFO_RATE (&absink->ringbuffer->spec.info) /
      1000;
  GST_OBJECT_UNLOCK (qahw);

  return delay;
}


static gboolean
gst_qahw_ring_buffer_start (GstAudioRingBuffer * rb)
{
  GstQahwRingBuffer *self = GST_QAHW_RING_BUFFER (rb);
  GstQahwSink *qahw = GST_QAHW_SINK (GST_OBJECT_PARENT (self));

  GST_DEBUG_OBJECT (qahw, "start");

  //Acquire wakelock
  request_wake_lock(qahw, wakelock_acquired, true);

  return TRUE;
}

static gboolean
gst_qahw_ring_buffer_pause (GstAudioRingBuffer * rb)
{
  GstQahwRingBuffer *self = GST_QAHW_RING_BUFFER (rb);
  GstQahwSink *qahw = GST_QAHW_SINK (GST_OBJECT_PARENT (self));

  GST_DEBUG_OBJECT (qahw, "pause");

  GST_QAHW_SINK_LOCK (qahw);
  qahw->paused = TRUE;
  if (qahw->stream) {
    qahw_out_pause (qahw->stream);
  }
  GST_QAHW_SINK_BROADCAST (qahw);
  GST_QAHW_SINK_UNLOCK (qahw);

  return TRUE;
}

static gboolean
gst_qahw_ring_buffer_resume (GstAudioRingBuffer * rb)
{
  GstQahwRingBuffer *self = GST_QAHW_RING_BUFFER (rb);
  GstQahwSink *qahw = GST_QAHW_SINK (GST_OBJECT_PARENT (self));

  GST_DEBUG_OBJECT (qahw, "resume");

  GST_QAHW_SINK_LOCK (qahw);
  qahw->paused = FALSE;
  if (qahw->stream) {
    int err;

    err = qahw_out_resume (qahw->stream);
  }
  GST_QAHW_SINK_BROADCAST (qahw);
  GST_QAHW_SINK_UNLOCK (qahw);

  return TRUE;
}

static gboolean
gst_qahw_ring_buffer_stop (GstAudioRingBuffer * rb)
{
  GstQahwRingBuffer *self = GST_QAHW_RING_BUFFER (rb);
  GstQahwSink *qahw = GST_QAHW_SINK (GST_OBJECT_PARENT (self));

  GST_DEBUG_OBJECT (qahw, "stop");

  GST_QAHW_SINK_LOCK (qahw);
  GST_QAHW_SINK_BROADCAST (qahw);
  GST_QAHW_SINK_UNLOCK (qahw);

  //Release wakelock
  request_wake_lock(qahw, wakelock_acquired, false);

  return TRUE;
}


static gboolean
gst_qahw_ring_buffer_activate (GstAudioRingBuffer * rb, gboolean active)
{
  GstQahwRingBuffer *self = GST_QAHW_RING_BUFFER (rb);
  GstQahwSink *qahw = GST_QAHW_SINK (GST_OBJECT_PARENT (self));

  GST_DEBUG_OBJECT (qahw, "activate %d", active);

  GST_QAHW_SINK_LOCK (qahw);
  qahw->running = active;
  GST_QAHW_SINK_BROADCAST (qahw);
  GST_QAHW_SINK_UNLOCK (qahw);

  return TRUE;
}

static void
gst_qahw_ring_buffer_clear_all (GstAudioRingBuffer * rb)
{
  GstQahwRingBuffer *self = GST_QAHW_RING_BUFFER (rb);
  GstQahwSink *qahw = GST_QAHW_SINK (GST_OBJECT_PARENT (self));

  GST_DEBUG_OBJECT (qahw, "clear all (flush stop)");

  qahw->clock_offset = gst_qahwsink_get_time (NULL, GST_AUDIO_BASE_SINK (qahw));

  GST_QAHW_SINK_LOCK (qahw);
  qahw->drained = FALSE;
  if (qahw->stream) {
    int err;

    GST_QAHW_SINK_UNLOCK (qahw);

    err = qahw_out_flush (qahw->stream);
    g_assert_cmpint (err, ==, 0);
  } else {
    GST_QAHW_SINK_UNLOCK (qahw);
  }
}

/* Returns the current time of the system clock.
 */
static GstClockTime
gst_qahwsink_get_time (GstClock * clock, GstAudioBaseSink * sink)
{
  GstQahwSink *qahw = GST_QAHW_SINK (sink);
  GstClockTime time;

  if (!sink->ringbuffer || !sink->ringbuffer->acquired)
    return GST_CLOCK_TIME_NONE;

  if (!clock || !GST_IS_CLOCK (clock))
    return GST_CLOCK_TIME_NONE;

  GST_QAHW_SINK_LOCK (qahw);
  time = gst_clock_get_time (clock);
  GST_QAHW_SINK_UNLOCK (qahw);

  return time;
}

int write_to_hal(GstQahwSink *qahw, char *data, size_t bytes, void *params_ptr)
{
    //stream_config *stream_params = (stream_config*) params_ptr;

    ssize_t ret;
    GST_QAHW_SINK_LOCK (qahw);
   // pthread_mutex_lock(&stream_params->write_lock);
    qahw_out_buffer_t out_buf;

    memset(&out_buf,0, sizeof(qahw_out_buffer_t));
    out_buf.buffer = data;
    out_buf.bytes = bytes;

    ret = qahw_out_write(qahw->stream, &out_buf);
    if (ret < 0) {
        GST_DEBUG_OBJECT(qahw, "writing data to hal failed (ret = %zd)\n", ret);
    } else if (ret != bytes) {
        GST_DEBUG_OBJECT(qahw, "provided bytes %zd, written bytes %d\n", bytes, ret);
        GST_DEBUG_OBJECT(qahw, "waiting for event write ready\n");
        GST_QAHW_SINK_WAIT (qahw);
        //pthread_cond_wait(&stream_params->write_cond, &stream_params->write_lock);
        GST_DEBUG_OBJECT(qahw, "out of wait for event write ready\n");
    }

    GST_QAHW_SINK_UNLOCK (qahw);
    //pthread_mutex_unlock(&stream_params->write_lock);
    return ret;
}

/*
int measure_kpi_values(GstQahwSink *qahw , bool is_offload) {
    int rc = 0;
    int offset = 0;
    size_t bytes_wanted = 0;
    size_t write_length = 0;
    size_t bytes_remaining = 0;
    size_t bytes_written = 0;
    char  *data = NULL;
    int ret = 0, count = 0;
    struct timespec ts_cold, ts_cont;
    uint64_t tcold, tcont, scold = 0, uscold = 0, scont = 0, uscont = 0;

    if (is_offload) {
        GST_DEBUG_OBJECT(qahw, "Set callback for offload stream in kpi mesaurement usecase\n");
        qahw_out_set_callback (qahw->stream, async_callback, gst_object_ref (qahw));
    }

    FILE *fd_latency_node = fopen(LATENCY_NODE, "r+");
    if (fd_latency_node) {
        ret = fwrite(LATENCY_NODE_INIT_STR, sizeof(LATENCY_NODE_INIT_STR), 1, fd_latency_node);
        if (ret<1)
            GST_DEBUG_OBJECT(qahw, "error(%d) writing to debug node!", ret);
            fprintf(stderr, "error(%d) writing to debug node!", ret);
        fflush(fd_latency_node);
    } else {
        GST_DEBUG_OBJECT(qahw, "debug node(%s) open failed!", LATENCY_NODE);
        fprintf(stderr, "debug node(%s) open failed!", LATENCY_NODE);
        return -1;
    }

    bytes_wanted = qahw_out_get_buffer_size(qahw->stream);
    data = (char *) calloc (1, bytes_wanted);
    if (data == NULL) {
        GST_DEBUG_OBJECT(qahw, "calloc failed!!\n");
        fprintf(stderr, "calloc failed!!\n");
        return -ENOMEM;
    }

    while (count < 64) {
        if (!bytes_remaining) {
            bytes_remaining = write_length = bytes_wanted;
        }
        if (count == 0) {
            ret = clock_gettime(CLOCK_REALTIME, &ts_cold);
            if (ret) {
                GST_DEBUG_OBJECT(qahw, "error(%d) fetching start time for cold latency", ret);
                fprintf(stderr, "error(%d) fetching start time for cold latency", ret);
                return -1;
            }
        } else if (count == 16) {
            int *d = (int *)data;
            d[0] = 0x01010000;
            ret = clock_gettime(CLOCK_REALTIME, &ts_cont);
            if (ret) {
                GST_DEBUG_OBJECT(qahw, "error(%d) fetching start time for continuous latency", ret);
                fprintf(stderr, "error(%d) fetching start time for continuous latency", ret);
                return -1;
            }
        }

        offset = write_length - bytes_remaining;
        bytes_written = write_to_hal(qahw, data+offset, bytes_remaining, gst_object_ref (qahw));
        bytes_remaining -= bytes_written;
        GST_DEBUG_OBJECT(qahw, "bytes_written %zd, bytes_remaining %zd\n",
                bytes_written, bytes_remaining);

        if (count == 16) {
            int *i = (int *)data;
            i[0] = 0x00000000;
        }
        count++;
    }

    char latency_buf[200] = {0};
    fread((void *) latency_buf, 100, 1, fd_latency_node);
    fclose(fd_latency_node);
    sscanf(latency_buf, " %llu,%llu,%*llu,%*llu,%llu,%llu", &scold, &uscold, &scont, &uscont);
    tcold = scold*1000 - ts_cold.tv_sec*1000 + uscold/1000 - ts_cold.tv_nsec/1000000;
    tcont = scont*1000 - ts_cont.tv_sec*1000 + uscont/1000 - ts_cont.tv_nsec/1000000;
    GST_DEBUG_OBJECT(qahw, "\n values from debug node %s\n", latency_buf);
    GST_DEBUG_OBJECT(qahw, " cold latency %llums, continuous latency %llums,\n", tcold, tcont);
    GST_DEBUG_OBJECT(qahw, " **Note: please add DSP Pipe/PP latency numbers to this, for final latency values\n");
    return rc;
}
*/

