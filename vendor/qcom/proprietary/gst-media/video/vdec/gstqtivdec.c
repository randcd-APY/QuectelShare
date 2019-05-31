/*
*  Copyright (c) 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/*
 * element-videodec QTI v4l2 video decoder
 * By default non-secure ION memory is used:
 * gst-launch-1.0 filesrc location=<path to h264 file> ! h264parse ! qtivdec ! waylandsink
 *
 * Pass property secure=TRUE for secure ION memory:
 * gst-launch-1.0 filesrc location=<path to h264 file> ! h264parse ! qtivdec secure=TRUE ! waylandsink
 *
 * Refer to gst_videodec_class_init() for other properties and its default values.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "gstqtivdec.h"
#include "gstionallocator.h"

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <linux/msm_ion.h>
#include <media/msm_media_info.h>
#include <gst/gst.h>
//for adding meta info to gstbuffer and sending to waylandsink plugin
#include <gst/ionbuf/gstionbuf_meta.h>
#include <media/msm_vidc.h> //for V4L2 macros required for HDR
#include <gst_crypto.h>

#define GBM_ENABLED

GST_DEBUG_CATEGORY (gst_qtivdec_debug);
#define GST_CAT_DEFAULT gst_qtivdec_debug

#define MAX_DEVICES 33          /* Max V4L2 device instances tried */

#define NB_BUF_INPUT 1
#define NB_BUF_OUTPUT 1    /* Request minimum buffers from decoder to get required number of buffers */

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

#define LUMA_PLANE 0
#define CHROMA_PLANE 1

#define MAX_OPERATING_FRAME_RATE (300 << 16)

#define parent_class gst_qtivdec_parent_class

#define CASE(ENUM) case ENUM: return #ENUM;

G_DEFINE_TYPE (Gstqtivdec, gst_qtivdec, GST_TYPE_VIDEO_DECODER);

/* Extradata flags for HDR support */
int vui_present = 0;
int mastering_display_present = 0;
int light_level_present = 0;

/* Array to hold flags to determine whether HDR metadata has been sent for the corresponding buffer index */
char * metadata_perform_map = NULL;

enum
{
  PROP_0,
  PROP_DEVICE,
  PROP_SILENT,
  PROP_SECURE,
  PROP_DECODE_ORDER,
  PROP_SKIP_FRAMES,
  PROP_PERF,
  PROP_OPERATING_RATE,
  PROP_ZERO_COPY,
};

enum ColorSpace_t{
  ITU_R_601,
  ITU_R_601_FR,
  ITU_R_709,
  ITU_R_2020,
  ITU_R_2020_FR,
};
enum Range{
  RangeUnspecified,
  RangeFull,
  RangeLimited,
  RangeOther = 0xff,
};

enum Primaries_{
  PrimariesUnspecified,
  PrimariesBT709_5,       // Rec.ITU-R BT.709-5 or equivalent
  PrimariesBT470_6M,      // Rec.ITU-R BT.470-6 System M or equivalent
  PrimariesBT601_6_625,   // Rec.ITU-R BT.601-6 625 or equivalent
  PrimariesBT601_6_525,   // Rec.ITU-R BT.601-6 525 or equivalent
  PrimariesGenericFilm,   // Generic Film
  PrimariesBT2020,        // Rec.ITU-R BT.2020 or equivalent
  PrimariesOther = 0xff,
};

// this partially in sync with the transfer values in graphics.h prior to the transfers
// unlikely to be required by Android section
enum Transfer{
  TransferUnspecified,
  TransferLinear,         // Linear transfer characteristics
  TransferSRGB,           // sRGB or equivalent
  TransferSMPTE170M,      // SMPTE 170M or equivalent (e.g. BT.601/709/2020)
  TransferGamma22,        // Assumed display gamma 2.2
  TransferGamma28,        // Assumed display gamma 2.8
  TransferST2084,         // SMPTE ST 2084 for 10/12/14/16 bit systems
  TransferHLG,            // ARIB STD-B67 hybrid-log-gamma

  // transfers unlikely to be required by Android
  TransferSMPTE240M = 0x40, // SMPTE 240M
  TransferXvYCC,          // IEC 61966-2-4
  TransferBT1361,         // Rec.ITU-R BT.1361 extended gamut
  TransferST428,          // SMPTE ST 428-1
  TransferOther = 0xff,
};

enum MatrixCoeffs{
  MatrixUnspecified,
  MatrixBT709_5,          // Rec.ITU-R BT.709-5 or equivalent
  MatrixBT470_6M,         // KR=0.30, KB=0.11 or equivalent
  MatrixBT601_6,          // Rec.ITU-R BT.601-6 625 or equivalent
  MatrixSMPTE240M,        // SMPTE 240M or equivalent
  MatrixBT2020,           // Rec.ITU-R BT.2020 non-constant luminance
  MatrixBT2020Constant,   // Rec.ITU-R BT.2020 constant luminance
  MatrixOther = 0xff,
};

// this is in sync with the standard values in graphics.h
enum Standard{
  StandardUnspecified,
  StandardBT709,                  // PrimariesBT709_5 and MatrixBT709_5
  StandardBT601_625,              // PrimariesBT601_6_625 and MatrixBT601_6
  StandardBT601_625_Unadjusted,   // PrimariesBT601_6_625 and KR=0.222, KB=0.071
  StandardBT601_525,              // PrimariesBT601_6_525 and MatrixBT601_6
  StandardBT601_525_Unadjusted,   // PrimariesBT601_6_525 and MatrixSMPTE240M
  StandardBT2020,                 // PrimariesBT2020 and MatrixBT2020
  StandardBT2020Constant,         // PrimariesBT2020 and MatrixBT2020Constant
  StandardBT470M,                 // PrimariesBT470_6M and MatrixBT470_6M
  StandardFilm,                   // PrimariesGenericFilm and KR=0.253, KB=0.068
  StandardOther = 0xff,
};

/* GstVideoDecoder base class method */
static gboolean gst_qtivdec_start (GstVideoDecoder * decoder);
static gboolean gst_qtivdec_stop (GstVideoDecoder * video_decoder);
static gboolean gst_qtivdec_set_format (GstVideoDecoder * decoder,
    GstVideoCodecState * state);
static GstFlowReturn gst_qtivdec_handle_frame (GstVideoDecoder * decoder,
    GstVideoCodecFrame * frame);
static gboolean gst_v4l2dec_propose_allocation (GstVideoDecoder * decoder,
    GstQuery * query);

static void unmap_input_buf (Gstqtivdec * dec);
static __u32 to_v4l2_streamformat (GstStructure * s);
static gchar *v4l2_type_str (guint32 fmt);
static gchar *v4l2_fmt_str (guint32 pxfmt);

static GstFlowReturn
gst_qtivdec_decode (GstVideoDecoder * decoder, GstVideoCodecFrame * frame);
gboolean plugin_init (GstPlugin * plugin);
static int alloc_ion_buffer(size_t size, GstVideoDecoder * decoder);
int queue_buf_cap(GstVideoDecoder * decoder, int n);
int video_set_control(GstVideoDecoder * decoder);
int video_set_secure(GstVideoDecoder * decoder);
static void gst_qtivdec_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_qtivdec_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);
static int decoder_stop_capture(Gstqtivdec *dec);
static GstFlowReturn
gst_qtivdec_setup_output (GstVideoDecoder * decoder);

/* Port Reconfigure support functions */
static int video_subscribe_event(Gstqtivdec * dec, int event_type);
static int subscribe_events(Gstqtivdec * dec);

/* 10-bit and HDR support functions */
static int handle_video_event(GstVideoDecoder *decoder);
static int video_set_dpb(Gstqtivdec * dec,
      enum v4l2_mpeg_vidc_video_dpb_color_format format);
static void prepare_color_aspects_metadata(uint32_t primaries, uint32_t range,
      uint32_t transfer, uint32_t matrix, ColorMetaData *color_mdata);
static void printcolor_metadata(struct ColorMetaData * color_mdata);

struct pixel_format
{
  guint32 pixel_fmt_nb;
  gchar *pixel_fmt_str;
};

struct type_io_v4l2
{
  guint32 type_io_nb;
  gchar *type_io_str;
};

struct type_io_v4l2 type_io[] = {
  {V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE, (gchar *) "V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE"},
  {V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE, (gchar *) "V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE"},
};

static GstStaticPadTemplate gst_qtivdec_sink_template =
    GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("video/x-h264,"
        "stream-format = (string) { byte-stream },"
        "alignment = (string) { au }"
        ";"
        "video/x-h265,"
        "stream-format = (string) { byte-stream },"
        "alignment = (string) { au }"
        ";"
        "video/mpeg,"
        "mpegversion = (int) { 1, 2, 4 },"
        "systemstream = (boolean) false,"
        "parsed = (boolean) true"
        ";"
        "video/x-xvid;"
        "video/x-3ivx;"
        "video/x-divx,"
        "divxversion = (int) {3, 4, 5},"
        "parsed = (boolean) true"
        ";"
        "video/x-vp8"
        ";"
        "video/x-wmv,"
        "wmvversion = (int) 3"
        ";"
        "video/x-jpeg,"
        "parsed = (boolean) true"
        ";" "image/jpeg," "parsed = (boolean) true" ";")
    );

static GstStaticPadTemplate gst_qtivdec_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("video/x-raw, "
        "format = (string) NV12, "
        "width  = (int) [ 32, 4096 ], " "height =  (int) [ 32, 4096 ]"));

struct pixel_format px_formats[] = {
  {V4L2_PIX_FMT_NV12, (gchar *) "V4L2_PIX_FMT_NV12"},
  {V4L2_PIX_FMT_H264, (gchar *) "V4L2_PIX_FMT_H264"},
#ifdef V4L2_PIX_FMT_HEVC
  {V4L2_PIX_FMT_HEVC, (gchar *) "V4L2_PIX_FMT_HEVC"},
#endif
  {V4L2_PIX_FMT_MPEG1, (gchar *) "V4L2_PIX_FMT_MPEG1"},
  {V4L2_PIX_FMT_MPEG2, (gchar *) "V4L2_PIX_FMT_MPEG2"},
  {V4L2_PIX_FMT_MPEG4, (gchar *) "V4L2_PIX_FMT_MPEG4"},
  {V4L2_PIX_FMT_XVID, (gchar *) "V4L2_PIX_FMT_XVID"},
  {V4L2_PIX_FMT_DIVX, (gchar *) "V4L2_PIX_FMT_DIVX"},
  {V4L2_PIX_FMT_VP8, (gchar *) "V4L2_PIX_FMT_VP8"},
  {V4L2_PIX_FMT_VC1_ANNEX_G, (gchar *) "V4L2_PIX_FMT_VC1_ANNEX_G"},
  {V4L2_PIX_FMT_VC1_ANNEX_L, (gchar *) "V4L2_PIX_FMT_VC1_ANNEX_L"},
  {V4L2_PIX_FMT_MJPEG, (gchar *) "V4L2_PIX_FMT_MJPEG"},
};

static const int event_type[] = {
  V4L2_EVENT_MSM_VIDC_FLUSH_DONE,
  V4L2_EVENT_MSM_VIDC_PORT_SETTINGS_CHANGED_SUFFICIENT,
  V4L2_EVENT_MSM_VIDC_PORT_SETTINGS_CHANGED_INSUFFICIENT,
  V4L2_EVENT_MSM_VIDC_SYS_ERROR,
  V4L2_EVENT_MSM_VIDC_HW_OVERLOAD,
  V4L2_EVENT_MSM_VIDC_HW_UNSUPPORTED,
  V4L2_EVENT_MSM_VIDC_RELEASE_BUFFER_REFERENCE,
  V4L2_EVENT_MSM_VIDC_RELEASE_UNQUEUED_BUFFER,
};

static const char *
colorspace_to_string(int cspace)
{
  switch (cspace) {
  case MSM_VIDC_BT709_5:
    return "bt709";
  case MSM_VIDC_UNSPECIFIED:
    return "unspecified";
  case MSM_VIDC_BT470_6_M:
    return "bt470m";
  case MSM_VIDC_BT601_6_625:
    return "bt601/625";
  case MSM_VIDC_BT601_6_525:
    return "bt601/525";
  case MSM_VIDC_SMPTE_240M:
    return "smpte240m";
  case MSM_VIDC_GENERIC_FILM:
    return "generic";
  case MSM_VIDC_BT2020:
    return "bt2020";
  case MSM_VIDC_RESERVED_1:
    return "reserved1";
  case MSM_VIDC_RESERVED_2:
    return "reserved2";
  }
  return "unknown";
}

/* used for handling subscribed events raised by video decoder */
static int
handle_video_event(GstVideoDecoder *decoder)
{
  GstPad * src_pad = NULL;
  GstStructure *reconfigureStructure = gst_structure_new("Port_Reconfigure", NULL, NULL);
  GstEvent * reconfigureEvent = gst_event_new_custom(GST_EVENT_CUSTOM_DOWNSTREAM, reconfigureStructure);
  struct v4l2_event event;
  Gstqtivdec *dec = GST_QTIVDEC (decoder);

  GST_DEBUG ("In handle_video_event");
  memset(&event, 0, sizeof (event));

  if (ioctl(dec->fd, VIDIOC_DQEVENT, &event) < 0) {
    GST_ERROR ("failed to dequeue event: %m");
    return -1;
  }

  switch (event.type) {
  GST_DEBUG ("In handle_video_event -> switch");
  case V4L2_EVENT_MSM_VIDC_PORT_SETTINGS_CHANGED_INSUFFICIENT: {
    unsigned int *ptr = (unsigned int *)event.u.data;
    unsigned int height = ptr[0];
    unsigned int width = ptr[1];
    struct v4l2_decoder_cmd cmd;

    dec->reconfigure_pending = TRUE;

    GST_INFO("Port Reconfig received insufficient, new size %ux%u",
         width, height);

    if (ptr[2] & V4L2_EVENT_BITDEPTH_FLAG) {
      enum msm_vidc_pixel_depth depth = ptr[3];

      switch (depth) {
      case MSM_VIDC_BIT_DEPTH_10:
        dec->depth = 10;
        GST_INFO("Bit depth changed to 10 bits");
        break;
      case MSM_VIDC_BIT_DEPTH_8:
        dec->depth = 8;
        GST_INFO("Bit depth changed to 8 bits");
        break;
      default:
        dec->depth = 0;
        GST_INFO("Bit depth changed to 0 bits");
        break;
      }
    }

    if (ptr[2] & V4L2_EVENT_COLOUR_SPACE_FLAG) {
      unsigned int cspace = ptr[5];
      GST_INFO("Colorspace changed to %s",
           colorspace_to_string(cspace));
    }

    dec->width = width;
    dec->height = height;

    memset (&cmd, 0, sizeof cmd);
    cmd.cmd = V4L2_DEC_QCOM_CMD_FLUSH;
    cmd.flags = V4L2_DEC_QCOM_CMD_FLUSH_CAPTURE;
    if (v4l2_ioctl (dec->fd, VIDIOC_DECODER_CMD, &cmd) < 0)
      GST_ERROR_OBJECT (dec, "Unable to flush capture port (%s)", strerror(errno));

    break;
  }
  case V4L2_EVENT_MSM_VIDC_PORT_SETTINGS_CHANGED_SUFFICIENT:
    GST_DEBUG ("Setting changed sufficient");
    break;
  case V4L2_EVENT_MSM_VIDC_FLUSH_DONE: {
    unsigned int *ptr = (unsigned int *)event.u.data;
    unsigned int flags = ptr[0];
    GST_INFO("V4L2_EVENT_MSM_VIDC_FLUSH_DONE");

    if (dec->reconfigure_pending) {
      src_pad = gst_element_get_static_pad (GST_ELEMENT (dec), "src");
      gst_pad_push_event(src_pad,reconfigureEvent);
      decoder_stop_capture(dec);
      gst_qtivdec_setup_output(decoder);
      dec->reconfigure_pending = FALSE;
    }else if (dec->in_flush){
      if (flags & V4L2_QCOM_CMD_FLUSH_CAPTURE)
        GST_DEBUG ("Flush Done received on CAPTURE queue");
      if (flags & V4L2_QCOM_CMD_FLUSH_OUTPUT)
        GST_DEBUG ("Flush Done received on OUTPUT queue");
      sem_post (&dec->sem_stop);
    }
    break;
  }
  case V4L2_EVENT_MSM_VIDC_SYS_ERROR:
    GST_DEBUG ("SYS Error received");
    break;
  case V4L2_EVENT_MSM_VIDC_HW_OVERLOAD:
    GST_DEBUG ("HW Overload received");
    break;
  case V4L2_EVENT_MSM_VIDC_HW_UNSUPPORTED:
    GST_DEBUG ("HW Unsupported received");
    break;
  case V4L2_EVENT_MSM_VIDC_RELEASE_BUFFER_REFERENCE:
    GST_DEBUG ("Release buffer reference");
    break;
  case V4L2_EVENT_MSM_VIDC_RELEASE_UNQUEUED_BUFFER:
    GST_DEBUG ("Release unqueued buffer");
    break;
  default:
    GST_DEBUG ("unknown event type occurred %x", event.type);
    break;
  }

  return 0;
}

/* used for sending an ioctl to subscribe to an event */
static int
video_subscribe_event(Gstqtivdec * dec, int event_type)
{
  struct v4l2_event_subscription sub;

  memset(&sub, 0, sizeof(sub));
  sub.type = event_type;

  if (ioctl(dec->fd, VIDIOC_SUBSCRIBE_EVENT, &sub) < 0) {
    GST_ERROR ("failed to subscribe to event type %u: %m", sub.type);
    return -1;
  }

  return 0;
}

/* used for subscribing to the events listed in the event_type array */
static int
subscribe_events(Gstqtivdec * dec)
{
  const int n_events = sizeof(event_type) / sizeof(event_type[0]);
  int idx;

  for (idx = 0; idx < n_events; idx++) {
    if (video_subscribe_event(dec, event_type[idx]))
      return -1;
  }

  return 0;
}

int video_set_dpb(Gstqtivdec * dec,
      enum v4l2_mpeg_vidc_video_dpb_color_format format)
{
  struct v4l2_ext_control control[2] = {0};
  struct v4l2_ext_controls controls = {0};

  GST_LOG ("In video_set_dpb");

  control[0].id = V4L2_CID_MPEG_VIDC_VIDEO_STREAM_OUTPUT_MODE;
  control[0].value = V4L2_CID_MPEG_VIDC_VIDEO_STREAM_OUTPUT_PRIMARY;

  control[1].id = V4L2_CID_MPEG_VIDC_VIDEO_DPB_COLOR_FORMAT;
  control[1].value = format;

  controls.count = 2;
  controls.ctrl_class = V4L2_CTRL_CLASS_MPEG;
  controls.controls = control;

  if (ioctl(dec->fd, VIDIOC_S_EXT_CTRLS, &controls) < 0) {
    GST_ERROR ("failed to set dpb format: %m");
    return -1;
  }
  return 0;
}

static const char *extradata_type_to_string(int type)
{
  switch (type) {
    CASE(MSM_VIDC_EXTRADATA_NONE)
    CASE(MSM_VIDC_EXTRADATA_MB_QUANTIZATION)
    CASE(MSM_VIDC_EXTRADATA_INTERLACE_VIDEO)
    CASE(MSM_VIDC_EXTRADATA_VC1_FRAMEDISP)
    CASE(MSM_VIDC_EXTRADATA_VC1_SEQDISP)
    CASE(MSM_VIDC_EXTRADATA_TIMESTAMP)
    CASE(MSM_VIDC_EXTRADATA_S3D_FRAME_PACKING)
    CASE(MSM_VIDC_EXTRADATA_FRAME_RATE)
    CASE(MSM_VIDC_EXTRADATA_PANSCAN_WINDOW)
    CASE(MSM_VIDC_EXTRADATA_RECOVERY_POINT_SEI)
    CASE(MSM_VIDC_EXTRADATA_MPEG2_SEQDISP)
    CASE(MSM_VIDC_EXTRADATA_STREAM_USERDATA)
    CASE(MSM_VIDC_EXTRADATA_FRAME_QP)
    CASE(MSM_VIDC_EXTRADATA_FRAME_BITS_INFO)
    CASE(MSM_VIDC_EXTRADATA_VQZIP_SEI)
    CASE(MSM_VIDC_EXTRADATA_ROI_QP)
    CASE(MSM_VIDC_EXTRADATA_MASTERING_DISPLAY_COLOUR_SEI)
    CASE(MSM_VIDC_EXTRADATA_CONTENT_LIGHT_LEVEL_SEI)
    CASE(MSM_VIDC_EXTRADATA_PQ_INFO)
    CASE(MSM_VIDC_EXTRADATA_INPUT_CROP)
    CASE(MSM_VIDC_EXTRADATA_OUTPUT_CROP)
    CASE(MSM_VIDC_EXTRADATA_DIGITAL_ZOOM)
    CASE(MSM_VIDC_EXTRADATA_VPX_COLORSPACE_INFO)
    CASE(MSM_VIDC_EXTRADATA_MULTISLICE_INFO)
    CASE(MSM_VIDC_EXTRADATA_NUM_CONCEALED_MB)
    CASE(MSM_VIDC_EXTRADATA_INDEX)
    CASE(MSM_VIDC_EXTRADATA_ASPECT_RATIO)
    CASE(MSM_VIDC_EXTRADATA_METADATA_LTR)
    CASE(MSM_VIDC_EXTRADATA_METADATA_FILLER)
    CASE(MSM_VIDC_EXTRADATA_METADATA_MBI)
    CASE(MSM_VIDC_EXTRADATA_VUI_DISPLAY_INFO)
    CASE(MSM_VIDC_EXTRADATA_YUVSTATS_INFO)
    default: return "Unknown";
  }
}

/* used for populating the ColorMetaData structure */
static void
prepare_color_aspects_metadata (uint32_t primaries, uint32_t range,
      uint32_t transfer, uint32_t matrix,
      ColorMetaData *color_mdata)
{
  color_mdata->colorPrimaries = (enum ColorPrimaries) primaries;
  color_mdata->range = (enum ColorRange)range;
  color_mdata->transfer = (enum GammaTransfer)transfer;
  color_mdata->matrixCoefficients = (enum MatrixCoEfficients)matrix;
}

/* printcolor_metadata is a debug function to print the static HDR metadata for debug purposes */
static void
printcolor_metadata (struct ColorMetaData * color_mdata)
{
  GST_DEBUG ("colorPrimaries : %d", color_mdata->colorPrimaries);
  GST_DEBUG ("range : %d", color_mdata->range);
  GST_DEBUG ("transfer : %d", color_mdata->transfer);
  GST_DEBUG ("matrixCoefficients : %d", color_mdata->matrixCoefficients);

  for (uint8_t i = 0; i < 3; i++) {
    GST_DEBUG ("color_mdata.masteringDisplayInfo.primaries.rgbPrimaries[%d][0] : %u", i, color_mdata->masteringDisplayInfo.primaries.rgbPrimaries[i][0]);
    GST_DEBUG ("color_mdata.masteringDisplayInfo.primaries.rgbPrimaries[%d][1] : %u", i, color_mdata->masteringDisplayInfo.primaries.rgbPrimaries[i][1]);
  }
  GST_DEBUG ("color_mdata.masteringDisplayInfo.primaries.whitePoint[0] : %u",color_mdata->masteringDisplayInfo.primaries.whitePoint[0]);
  GST_DEBUG ("color_mdata.masteringDisplayInfo.primaries.whitePoint[1] : %u",color_mdata->masteringDisplayInfo.primaries.whitePoint[1]);
  GST_DEBUG ("color_mdata.masteringDisplayInfo.maxDisplayLuminance : %u",color_mdata->masteringDisplayInfo.maxDisplayLuminance);
  GST_DEBUG ("color_mdata.masteringDisplayInfo.minDisplayLuminance : %u",color_mdata->masteringDisplayInfo.minDisplayLuminance);
  GST_DEBUG ("color_mdata.contentLightLevel.maxContentLightLevel : %u",color_mdata->contentLightLevel.maxContentLightLevel);
  GST_DEBUG ("color_mdata.contentLightLevel.minPicAverageLightLevel : %u",color_mdata->contentLightLevel.minPicAverageLightLevel);
}

/* used for extracting the HDR metadata and populates the ColorMetaData structure
   using prepare_color_aspects_metadata function*/
static int
populate_colormetadata (Gstqtivdec * dec, enum msm_vidc_extradata_type type, void *data, int size)
{
  switch (type) {
  case MSM_VIDC_EXTRADATA_MASTERING_DISPLAY_COLOUR_SEI: {
    struct msm_vidc_mastering_display_colour_sei_payload *payload = data;

    if (size != sizeof (*payload)) {
      GST_ERROR ("extradata: Invalid data size for %s", extradata_type_to_string(type));
      return -1;
    }
    if(mastering_display_present == 0) {
       GST_DEBUG ("extradata: %s nDisplayPrimariesX={%u, %u, %u} "
         "nDisplayPrimariesY={%u, %u, %u} nWhitePointX=%u "
         "nWhitePointY=%u nMaxDisplayMasteringLuminance=%u"
         "nMinDisplayMasteringLuminance=%u",
         extradata_type_to_string(type),
         payload->nDisplayPrimariesX[0],
         payload->nDisplayPrimariesX[1],
         payload->nDisplayPrimariesX[2],
         payload->nDisplayPrimariesY[0],
         payload->nDisplayPrimariesY[1],
         payload->nDisplayPrimariesY[2],
         payload->nWhitePointX,
         payload->nWhitePointY,
         payload->nMaxDisplayMasteringLuminance,
         payload->nMinDisplayMasteringLuminance);

      if(mastering_display_present == 0)
        mastering_display_present = 1;

      for (uint8_t i = 0; i < 3; i++) {
        dec->color_mdata.masteringDisplayInfo.primaries.rgbPrimaries[i][0] = payload->nDisplayPrimariesX[i];
        dec->color_mdata.masteringDisplayInfo.primaries.rgbPrimaries[i][1] = payload->nDisplayPrimariesY[i];
      }
      dec->color_mdata.masteringDisplayInfo.primaries.whitePoint[0] = payload->nWhitePointX;
      dec->color_mdata.masteringDisplayInfo.primaries.whitePoint[1] = payload->nWhitePointY;
      dec->color_mdata.masteringDisplayInfo.maxDisplayLuminance = payload->nMaxDisplayMasteringLuminance;
      dec->color_mdata.masteringDisplayInfo.minDisplayLuminance = payload->nMinDisplayMasteringLuminance;
    }
    break;
  }
  case MSM_VIDC_EXTRADATA_CONTENT_LIGHT_LEVEL_SEI: {
    struct msm_vidc_content_light_level_sei_payload *payload = data;
    if (size != sizeof (*payload)) {
      GST_ERROR ("extradata: Invalid data size for %s",
        extradata_type_to_string(type));
      return -1;
    }
    if(light_level_present == 0) {
      GST_DEBUG ("extradata: %s nMaxContentLight : %u, nMaxPicAverageLight : %u",
        extradata_type_to_string(type),
        payload->nMaxContentLight,
        payload->nMaxPicAverageLight);

      if(light_level_present == 0)
        light_level_present = 1;

      dec->color_mdata.contentLightLevel.maxContentLightLevel = payload->nMaxContentLight;
      dec->color_mdata.contentLightLevel.minPicAverageLightLevel = payload->nMaxPicAverageLight;
    }
    break;
  }
  case MSM_VIDC_EXTRADATA_VUI_DISPLAY_INFO: {
    struct msm_vidc_vui_display_info_payload *payload = data;

    if (size != sizeof (*payload)) {
      GST_ERROR ("extradata: Invalid data size for %s",
        extradata_type_to_string(type));
      return -1;
    }
    if(vui_present == 0) {
      GST_DEBUG ("video_signal_present_flag : %u, video_format : %u, bit_depth_y : %u, bit_depth_c : %u, video_full_range_flag : %u, color_description_present_flag : %u, color_primaries : %u, transfer_characteristics : %u, matrix_coefficients : %u, chroma_location_info_present_flag : %u, chroma_format_idc : %u separate_color_plane_flag : %u", payload->video_signal_present_flag, payload->video_format, payload->bit_depth_y, payload->bit_depth_c, payload->video_full_range_flag, payload->color_description_present_flag, payload->color_primaries, payload->transfer_characteristics, payload->matrix_coefficients, payload->chroma_location_info_present_flag, payload->chroma_format_idc, payload->separate_color_plane_flag );

    if(vui_present == 0)
      vui_present = 1;


    prepare_color_aspects_metadata(payload->color_primaries,
      payload->video_full_range_flag,payload->transfer_characteristics,payload->matrix_coefficients,&dec->color_mdata);
    }
    break;
  }
  default:
    GST_DEBUG ("extradata: unhandled extradata header %s (%u)",
      extradata_type_to_string(type), type);
    return -1;
  }
  return 0;
}

/*used for parsing the extradata plane content, populates ColorMetaData structure
   using populate_colormetadata function and send it to display */
static void
parse_populate_and_send_extradata (Gstqtivdec * dec, const struct msm_vidc_extradata_header *hdr, int size, int index)
{
  unsigned int left;
  int ret, ret_gbm;
  void * prm = NULL;
  struct msm_vidc_extradata_index *payload;

  if (!hdr || size < 0)
    return false;

  left = size;

  while (left > sizeof (*hdr) && left >= hdr->size && hdr->type != MSM_VIDC_EXTRADATA_NONE) {
    if (hdr->type == MSM_VIDC_EXTRADATA_INDEX) {
      payload = (void *)hdr->data;
      ret = populate_colormetadata(dec, payload->type,
              (void *)((char*)hdr->data + sizeof (hdr->type)),
              hdr->data_size - sizeof (hdr->type));
    }
    else {
      ret = populate_colormetadata(dec, hdr->type,
              (void *)(hdr->data),
              hdr->data_size);
    }
    if (ret)
      return false;

    left -= hdr->size;
    hdr = (struct msm_vidc_extradata_header *)((char*)hdr + hdr->size);
  }

  prm=(void *)&dec->color_mdata;

  if(vui_present == 1 && metadata_perform_map[index] == 0) {
    printcolor_metadata(&dec->color_mdata);

    ret_gbm = gbm_perform(GBM_PERFORM_SET_METADATA,dec->cap_buf_bo[index],GBM_METADATA_SET_COLOR_METADATA,prm);

    if(ret_gbm==GBM_ERROR_NONE)
      GST_DEBUG ("Set Color Metadata Success\n");
    else
      GST_ERROR ("Set Color Metadata Failed\n");

    metadata_perform_map[index] = 1;
  }
}

static const gchar *interlace_mode[] = {
  "progressive",
  "interleaved",
  "mixed",
  "fields"
};

static GstVideoInterlaceMode
gst_interlace_mode_from_string (const gchar * mode)
{
  gint i;
  for (i = 0; i < G_N_ELEMENTS (interlace_mode); i++) {
    if (g_str_equal (interlace_mode[i], mode))
      return i;
  }
  return GST_VIDEO_INTERLACE_MODE_PROGRESSIVE;
}

static __u32
to_v4l2_streamformat (GstStructure * s)
{
  if (gst_structure_has_name (s, "video/x-h264"))
    return V4L2_PIX_FMT_H264;

#ifdef V4L2_PIX_FMT_HEVC
  if (gst_structure_has_name (s, "video/x-h265"))
    return V4L2_PIX_FMT_HEVC;
#endif

  if (gst_structure_has_name (s, "video/mpeg")) {
    gint mpegversion = 0;
    if (gst_structure_get_int (s, "mpegversion", &mpegversion)) {
      switch (mpegversion) {
        case 1:
          return V4L2_PIX_FMT_MPEG1;
          break;
        case 2:
          return V4L2_PIX_FMT_MPEG2;
          break;
        case 4:
          return V4L2_PIX_FMT_MPEG4;
          break;
        default:
          return 0;
          break;
      }
    }
  }

  if (gst_structure_has_name (s, "video/x-xvid"))
    return V4L2_PIX_FMT_MPEG4;

  if (gst_structure_has_name (s, "video/x-divx"))
    return V4L2_PIX_FMT_DIVX;

  if (gst_structure_has_name (s, "video/x-3ivx"))
    return V4L2_PIX_FMT_XVID;

  if (gst_structure_has_name (s, "video/x-vp8"))
    return V4L2_PIX_FMT_VP8;

  if (gst_structure_has_name (s, "video/x-wmv")) {
    const gchar *format;

    if ((format = gst_structure_get_string (s, "format"))
        && (g_str_equal (format, "WVC1")))
      return V4L2_PIX_FMT_VC1_ANNEX_G;
    else
      return V4L2_PIX_FMT_VC1_ANNEX_L;
  }

  if (gst_structure_has_name (s, "image/jpeg"))
    return V4L2_PIX_FMT_MJPEG;

  return 0;
}

/* used for debug I/O type (v4l2 object) */
static gchar *
v4l2_type_str (guint32 type)
{
  int i = 0;
  for (i = 0; i < ARRAY_SIZE (type_io); i++) {
    if (type_io[i].type_io_nb == type)
      return type_io[i].type_io_str;
  }
  return NULL;
}

/* used for debug pixelformat (v4l2 object) */
static gchar *
v4l2_fmt_str (guint32 fmt)
{
  int i = 0;
  for (i = 0; i < ARRAY_SIZE (px_formats); i++) {
    if (px_formats[i].pixel_fmt_nb == fmt)
      return px_formats[i].pixel_fmt_str;
  }
  return NULL;
}

static GstVideoFormat
to_gst_pixelformat (__u32 fmt)
{
  switch (fmt) {
    case V4L2_PIX_FMT_NV12:
      return GST_VIDEO_FORMAT_NV12;
    default:
      return GST_VIDEO_FORMAT_UNKNOWN;
  }
}

static void
gst_qtivdec_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  Gstqtivdec *dec = GST_QTIVDEC (object);

  switch (prop_id) {
    case PROP_SILENT:
      dec->silent = g_value_get_boolean (value);
      break;
    case PROP_SECURE:
      dec->secure = g_value_get_boolean (value);
      break;
    case PROP_DECODE_ORDER:
      dec->decode_order = g_value_get_boolean (value);
      break;
    case PROP_SKIP_FRAMES:
      dec->skip_frames = g_value_get_boolean (value);
      break;
    case PROP_PERF:
      dec->perf = g_value_get_boolean (value);
      break;
    case PROP_OPERATING_RATE:
      dec->operating_rate = g_value_get_boolean (value);
      break;
    case PROP_ZERO_COPY:
      dec->zero_copy = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_qtivdec_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  Gstqtivdec *dec = GST_QTIVDEC (object);

  switch (prop_id) {
    case PROP_SILENT:
      g_value_set_boolean (value, dec->silent);
      break;
    case PROP_SECURE:
      dec->secure = g_value_get_boolean (value);
      break;
    case PROP_DECODE_ORDER:
      dec->decode_order = g_value_get_boolean (value);
      break;
    case PROP_SKIP_FRAMES:
      dec->skip_frames = g_value_get_boolean (value);
      break;
    case PROP_PERF:
      dec->perf = g_value_get_boolean (value);
      break;
    case PROP_OPERATING_RATE:
      dec->operating_rate = g_value_get_boolean (value);
      break;
    case PROP_ZERO_COPY:
      dec->zero_copy = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* Open the device matching width/height/format as input */
static int
gst_qtivdec_open_device (Gstqtivdec * dec, __u32 fmt, __u32 width, __u32 height,
    GstVideoInterlaceMode interlace_mode)
{
  int fd = -1;
  int ret;
  gint i = 0;
  gboolean found;
  gchar path[100];
  struct v4l2_format s_fmt;
  int libv4l2_fd;
  GST_INFO_OBJECT (dec, "mm inside gst_qtivdec_open_device");
  memset (&s_fmt, 0, sizeof s_fmt);
  s_fmt.fmt.pix_mp.width = width;
  s_fmt.fmt.pix_mp.height = height;
  s_fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
  s_fmt.fmt.pix_mp.pixelformat = fmt;

  if (interlace_mode == GST_VIDEO_INTERLACE_MODE_PROGRESSIVE)
    s_fmt.fmt.pix_mp.field = V4L2_FIELD_NONE;
  else
    s_fmt.fmt.pix_mp.field = V4L2_FIELD_INTERLACED;

  found = FALSE;
  for (i = 0; i < MAX_DEVICES; i++) {
    snprintf (path, sizeof (path), "/dev/video%d", i);

    fd = open (path, O_RDWR, 0);
    if (fd < 0)
      continue;

    libv4l2_fd = v4l2_fd_open (fd, V4L2_DISABLE_CONVERSION);
    if (libv4l2_fd != -1)
      fd = libv4l2_fd;

    ret = v4l2_ioctl (fd, VIDIOC_S_FMT, &s_fmt);
    if (ret < 0) {
      v4l2_close (fd);
      continue;
    }
    GST_INFO_OBJECT(dec, "mm sucess set format sizeimage %d", s_fmt.fmt.pix_mp.plane_fmt[0].sizeimage);
    dec->out_buf_size = s_fmt.fmt.pix_mp.plane_fmt[0].sizeimage;

    found = TRUE;
    break;
  }

  if (!found) {
    GST_ERROR_OBJECT (dec,
        "No device found matching format %s(0x%x) and resolution %dx%d",
        v4l2_fmt_str (fmt), fmt, width, height);
    return -1;
  }

  GST_INFO_OBJECT (dec, "Device %s opened for format %s and %dx%d resolution",
      path, v4l2_fmt_str (fmt), width, height);
  return fd;
}

gboolean
plugin_init (GstPlugin * plugin)
{
  if (!gst_element_register (plugin, "qtivdec", GST_RANK_PRIMARY + 1,
          GST_TYPE_QTIVDEC))
    return FALSE;
  return TRUE;
}

/* initialize the qtivdec's class */
static void
gst_qtivdec_class_init (GstqtivdecClass * klass)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);
  GstVideoDecoderClass *video_decoder_class = GST_VIDEO_DECODER_CLASS (klass);
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&gst_qtivdec_src_template));

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&gst_qtivdec_sink_template));

  gobject_class->set_property = gst_qtivdec_set_property;
  gobject_class->get_property = gst_qtivdec_get_property;

  g_object_class_install_property (gobject_class, PROP_SILENT,
      g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
          FALSE, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_SECURE,
      g_param_spec_boolean ("secure", "Secure", "use secure buffers",
          FALSE, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_DECODE_ORDER,
      g_param_spec_boolean ("decode_order", "Decode_order", "output order decode",
          FALSE, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_SKIP_FRAMES,
      g_param_spec_boolean ("skip_frames", "Skip_frames", "used in trick play",
          FALSE, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_PERF,
      g_param_spec_boolean ("perf", "Perf", "used when run in turbo mode. \
        Replaced by OPERATING RATE.",
          TRUE, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_OPERATING_RATE,
      g_param_spec_boolean ("operating_rate", "Operating_rate", "For trick mode \
        for smaller resolution to control FF/REW with with higher operating rate",
          FALSE, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_ZERO_COPY,
      g_param_spec_boolean ("zero_copy", "zero_copy", "use ion-memory",
          FALSE, G_PARAM_READWRITE));

  video_decoder_class->start = GST_DEBUG_FUNCPTR (gst_qtivdec_start);
  video_decoder_class->stop = GST_DEBUG_FUNCPTR (gst_qtivdec_stop);
  video_decoder_class->set_format = GST_DEBUG_FUNCPTR (gst_qtivdec_set_format);
  video_decoder_class->handle_frame =
      GST_DEBUG_FUNCPTR (gst_qtivdec_handle_frame);
  video_decoder_class->propose_allocation = 
      GST_DEBUG_FUNCPTR(gst_v4l2dec_propose_allocation);

/* TODO: Following vmethods to be implemented */
/*
  gobject_class->dispose = GST_DEBUG_FUNCPTR (gst_qtivdec_dispose);
  gobject_class->finalize = GST_DEBUG_FUNCPTR (gst_qtivdec_finalize);
  video_decoder_class->finish = GST_DEBUG_FUNCPTR (gst_qtivdec_finish);
  video_decoder_class->flush = GST_DEBUG_FUNCPTR (gst_qtivdec_flush);
  video_decoder_class->src_query =
      GST_DEBUG_FUNCPTR (gst_qtivdec_src_query);
  video_decoder_class->sink_event =
      GST_DEBUG_FUNCPTR (gst_qtivdec_sink_event);
  element_class->change_state =
      GST_DEBUG_FUNCPTR (gst_qtivdec_change_state);
*/

  GST_DEBUG_CATEGORY_INIT (gst_qtivdec_debug, "qtivdec", 0,
      "qcv4l2 video decoder");

  gst_element_class_set_static_metadata (element_class,
  "V4L2 decoder", "Decoder/Video", "A v4l2 decoder", "QTI");
  }

/* Init the qtivdec structure */
static void
gst_qtivdec_init (Gstqtivdec * dec)
{
  GstVideoDecoder *decoder = (GstVideoDecoder *) dec;

  dec->current_nb_buf_input = 0;

  gst_video_decoder_set_packetized (decoder, TRUE);

  dec->input_buf_add = NULL;
  dec->input_buf_size = NULL;

  dec->output_setup = FALSE;
  dec->input_setup = FALSE;

  dec->header = NULL;
  dec->fd = -1;
  dec->depth = 8;
  dec->gbm = NULL;
  dec->fd_card0 = -1;
  dec->reconfigure_pending = FALSE;
  dec->in_flush = FALSE;
}

static gboolean
gst_qtivdec_start (GstVideoDecoder * decoder)
{
  Gstqtivdec *dec = GST_QTIVDEC (decoder);
  GST_DEBUG_OBJECT (dec, "Starting");
  GST_INFO_OBJECT (dec, "mm inside gst_qtivdec_start");
  pthread_mutex_init (&dec->lock, 0);
  sem_init (&dec->sem_done, 0, 0);
  sem_init (&dec->sem_stop, 0, 0);

  return TRUE;
}

static int decoder_stop_output(Gstqtivdec *dec)
{
  guint n;
  enum v4l2_buf_type type;
  struct v4l2_requestbuffers reqbuf;

  type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;

  if (v4l2_ioctl (dec->fd, VIDIOC_STREAMOFF, &type) < 0)
    GST_WARNING_OBJECT (dec, "Unable to stop stream on output err=%s",
        strerror (errno));

  GST_DEBUG_OBJECT (dec, "STREAM OFF OUTPUT");

  memset (&reqbuf, 0, sizeof reqbuf);
  reqbuf.memory = V4L2_MEMORY_USERPTR;
  reqbuf.type = type;

  if (ioctl(dec->fd, VIDIOC_REQBUFS, &reqbuf) < 0)
    GST_ERROR (dec, "REQBUFS with count=0 OUTPUT queue failed");
  else
    GST_DEBUG_OBJECT (dec, "REQBUFS with count=0 OUTPUT queue success");

  for (n = 0; n < dec->out_buf_cnt; n++){
    if (dec->out_ion_addr[n]) {
      if (munmap(dec->out_ion_addr[n], dec->out_buf_size))
        GST_ERROR ("mm failed to unmap out buffer: ");
    }
  }

  for (n = 0; n < dec->out_buf_cnt; n++){
    if (dec->out_ion_fd[n] >= 0) {
      if (close(dec->out_ion_fd[n]) < 0)
        GST_ERROR ("failed to close out ion buffer fd:");
    }

    if (dec->secure){
      if (dec->out_ion_fd_secure[n] >= 0) {
        if (close(dec->out_ion_fd_secure[n]) < 0)
          GST_ERROR ("failed to close out ion secure buffer fd:");
      }
    }
    dec->out_ion_fd[n] = -1;
    dec->out_ion_addr[n] = NULL;
    if (dec->secure)
      dec->out_ion_fd_secure[n] = -1;
  }
  dec->out_buf_size = 0;
  dec->out_buf_cnt = 0;
  return 0;
}

static int decoder_stop_capture(Gstqtivdec *dec)
{
  enum v4l2_buf_type type;
  struct v4l2_requestbuffers reqbuf;

  type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;

  if (v4l2_ioctl (dec->fd, VIDIOC_STREAMOFF, &type) < 0)
    GST_WARNING_OBJECT (dec, "Unable to stop stream on capture err=%s",
        strerror (errno));

  GST_DEBUG_OBJECT (dec, "STREAM OFF CAPTURE");

  memset (&reqbuf, 0, sizeof reqbuf);
  reqbuf.memory = V4L2_MEMORY_USERPTR;
  reqbuf.type = type;

  if (ioctl(dec->fd, VIDIOC_REQBUFS, &reqbuf) < 0)
    GST_ERROR (dec, "REQBUFS with count=0 CAPTURE queue failed");
  else
    GST_DEBUG_OBJECT (dec, "REQBUFS with count=0 CAPTURE queue success");

#ifdef GBM_ENABLED
/* Destroy GBM buffers */
  for (int n = 0; n < dec->cap_buf_cnt; n++) {
    if(!dec->secure){
      if (munmap(dec->cap_buf_addr[n], dec->cap_buf_size[0][n]))
        GST_ERROR ("failed to unmap buffer-plane 0");
    }
    gbm_bo_destroy(dec->cap_buf_bo[n]);

    dec->cap_buf_bo[n] = NULL;
    dec->cap_buf_fd[n] = -1;
    dec->cap_buf_addr[n] = NULL;
  }

  dec->cap_buf_cnt = 0;

  gbm_device_destroy(dec->gbm);
  dec->gbm = NULL;

  if (dec->fd_card0 != -1) {
    v4l2_close(dec->fd_card0);
    dec->fd_card0 = -1;
  }
#endif

  /* de-alloc extradata buffer */
  if (munmap(dec->extradata_ion_addr, dec->cap_buf_cnt * dec->extradata_size))
    GST_ERROR ("Failed to unmap buffer plane-1");
  else
    GST_DEBUG ("Successfully Unmapped Extradata ion buffer ");

  if (close(dec->extradata_ion_fd) < 0)
    GST_ERROR ("Failed to close out extradata ion buffer fd");
  else
    GST_DEBUG ("Successfully Closed Extradata ion buffer fd ");

  dec->extradata_ion_addr = NULL;
  dec->extradata_ion_fd = -1;
  dec->extradata_size = 0;

  return 0;
}

static int decoder_flush_and_stop(Gstqtivdec *dec)
{
  GstVideoDecoder *decoder = (GstVideoDecoder *) dec;
  struct v4l2_decoder_cmd cmd;
  int ret;
  dec->in_flush = TRUE;
  GST_INFO_OBJECT (dec, "mm inside decoder_flush_and_stop");

  memset (&cmd, 0, sizeof cmd);
  cmd.cmd = V4L2_DEC_QCOM_CMD_FLUSH;
  cmd.flags = V4L2_DEC_QCOM_CMD_FLUSH_OUTPUT | V4L2_DEC_QCOM_CMD_FLUSH_CAPTURE;
  if (v4l2_ioctl (dec->fd, VIDIOC_DECODER_CMD, &cmd) < 0)
    GST_WARNING_OBJECT (dec, "Unable to flush capture/output ports (%s)",
      strerror(errno));

  sem_wait (&dec->sem_stop);

  decoder_stop_output(dec);
  decoder_stop_capture(dec);

  memset (&cmd, 0, sizeof cmd);
  cmd.cmd = V4L2_DEC_CMD_STOP;

/* TODO: Stop decoder at the end. */
  ret = v4l2_ioctl (dec->fd, VIDIOC_DECODER_CMD, &cmd);

  if (ret < 0)
    GST_WARNING_OBJECT (dec, "Unable to stop decoder (%s)",
      strerror(errno));

  return 0;
}

/* Stop Stream + Munmaping + Close thread and device */
static gboolean
gst_qtivdec_stop (GstVideoDecoder * decoder)
{
  Gstqtivdec *dec = GST_QTIVDEC (decoder);
  GST_DEBUG_OBJECT (dec, "Stopping");
  GST_INFO_OBJECT (dec, "mm inside gst_qtivdec_stop");
  dec->input_setup = FALSE;
  dec->output_setup = FALSE;

  if (dec->header) {
    GST_DEBUG_OBJECT (dec, "mm inside gst_qtivdec_stop - 2");
    gst_buffer_unref (dec->header);
    dec->header = NULL;
  }

  decoder_flush_and_stop(dec);

  if(metadata_perform_map)
    free(metadata_perform_map);

  if (dec->input_state) {
    GST_DEBUG_OBJECT (dec, "mm inside gst_qtivdec_stop - 3");
    gst_video_codec_state_unref (dec->input_state);
    dec->input_state = NULL;
  }
  if (dec->output_state) {
    GST_DEBUG_OBJECT (dec, "mm inside gst_qtivdec_stop - 4");
    gst_video_codec_state_unref (dec->output_state);
    dec->output_state = NULL;
  }

  unmap_input_buf (dec);

  if (dec->input_buf_add) {
    GST_DEBUG_OBJECT (dec, "mm inside gst_qtivdec_stop - 5");
    free (dec->input_buf_add);
    dec->input_buf_add = NULL;

    free (dec->input_buf_size);
    dec->input_buf_size = NULL;

    dec->current_nb_buf_input = 0;
  }

  if (dec->fd != -1) {
    GST_DEBUG_OBJECT (dec, "mm inside gst_qtivdec_stop - 6");
    v4l2_close (dec->fd);
    dec->fd = -1;
  }

  if (dec->ion_fd != -1) {
    GST_DEBUG_OBJECT (dec, "mm inside gst_qtivdec_stop - 7");
    v4l2_close(dec->ion_fd);
    dec->ion_fd = -1;
  }

  if (dec->secure_ion_fd != -1) {
    GST_DEBUG_OBJECT (dec, "mm inside gst_qtivdec_stop - 8");
    v4l2_close(dec->secure_ion_fd);
    dec->secure_ion_fd = -1;
  }

  pthread_mutex_destroy (&dec->lock);
  sem_destroy (&dec->sem_done);
  sem_destroy (&dec->sem_stop);
  gst_ion_memory_destroy();

  GST_DEBUG_OBJECT (dec, "Stopped !!");

  return TRUE;
}

/* add video_set_control */
int video_set_control(GstVideoDecoder * decoder)
{
  Gstqtivdec *dec = GST_QTIVDEC (decoder);
  struct v4l2_control control = {0};

  if (dec->decode_order) {
    control.id = V4L2_CID_MPEG_VIDC_VIDEO_OUTPUT_ORDER;
    control.value = V4L2_MPEG_VIDC_VIDEO_OUTPUT_ORDER_DECODE;

    if (ioctl(dec->fd, VIDIOC_S_CTRL, &control) < 0) {
      GST_DEBUG_OBJECT (dec, "failed to set output order");
      return -1;
    }
  }

  if (dec->skip_frames) {
    control.id = V4L2_CID_MPEG_VIDC_VIDEO_PICTYPE_DEC_MODE;
    control.value = V4L2_MPEG_VIDC_VIDEO_PICTYPE_DECODE_ON;

    if (ioctl(dec->fd, VIDIOC_S_CTRL, &control) < 0) {
      GST_DEBUG_OBJECT (dec, "failed to set skip mode");
      return -1;
    }
  }

  if (dec->perf) {
    control.id = V4L2_CID_MPEG_VIDC_SET_PERF_LEVEL;
    control.value = V4L2_CID_MPEG_VIDC_PERF_LEVEL_TURBO;

    if (ioctl(dec->fd, VIDIOC_S_CTRL, &control) < 0) {
      GST_DEBUG_OBJECT (dec, "failed to set perf level");
      return -1;
    }
  }

  if (dec->operating_rate) {
    control.id = V4L2_CID_MPEG_VIDC_VIDEO_OPERATING_RATE;
    control.value = MAX_OPERATING_FRAME_RATE;

    if (ioctl(dec->fd, VIDIOC_S_CTRL, &control) < 0) {
      GST_DEBUG_OBJECT (dec, "failed to set operating rate");
      return -1;
    }
  }

  control.id = V4L2_CID_MPEG_VIDC_VIDEO_CONTINUE_DATA_TRANSFER;
  control.value = 1;

  if (ioctl(dec->fd, VIDIOC_S_CTRL, &control) < 0) {
    GST_DEBUG_OBJECT (dec, "failed to set data transfer mode");
    return -1;
  }

  control.id = V4L2_CID_MPEG_VIDC_VIDEO_CONCEAL_COLOR;
  control.value = 0x00ff;

  if (ioctl(dec->fd, VIDIOC_S_CTRL, &control) < 0) {
    GST_DEBUG_OBJECT (dec, "failed to set conceal color");
    return -1;
  }

  control.id = V4L2_CID_MPEG_VIDC_VIDEO_EXTRADATA;
  control.value = V4L2_MPEG_VIDC_EXTRADATA_DISPLAY_COLOUR_SEI;

  if (ioctl(dec->fd, VIDIOC_S_CTRL, &control) < 0) {
    GST_ERROR ("failed to enable display colour sei extradata");
    return -1;
  }

  control.id = V4L2_CID_MPEG_VIDC_VIDEO_EXTRADATA;
  control.value = V4L2_MPEG_VIDC_EXTRADATA_CONTENT_LIGHT_LEVEL_SEI;

  if (ioctl(dec->fd, VIDIOC_S_CTRL, &control) < 0) {
    GST_ERROR ("failed to enable display colour sei extradata");
    return -1;
  }

  control.id = V4L2_CID_MPEG_VIDC_VIDEO_EXTRADATA;
  control.value = V4L2_MPEG_VIDC_EXTRADATA_VUI_DISPLAY;

  if (ioctl(dec->fd, VIDIOC_S_CTRL, &control) < 0) {
    GST_ERROR ("failed to enable display colour sei extradata");
    return -1;
  }

  return 0;
}

/* add queue_buf_cap */
int queue_buf_cap(GstVideoDecoder * decoder, int n)
{
  Gstqtivdec *dec = GST_QTIVDEC (decoder);
  enum v4l2_buf_type type;
  struct v4l2_buffer buf;
  struct v4l2_plane planes[2];
  type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
  if (n >= dec->cap_buf_cnt){
    GST_DEBUG_OBJECT (dec, "tried to queue a non existing");
      return -1;
  }

  memset(&buf, 0, sizeof buf);
  memset(planes, 0, sizeof(planes));
  buf.type = type;
  buf.memory = V4L2_MEMORY_USERPTR;
  buf.index = n;
  buf.length = 2;
  buf.m.planes = planes;

  if (dec->secure) {
  buf.m.planes[0].m.userptr = (unsigned long)dec->cap_buf_fd[n];
  GST_DEBUG_OBJECT (dec, "queue cap secure");
  }else{
  GST_DEBUG_OBJECT (dec, "queue cap NON-secure");
  buf.m.planes[0].m.userptr = (unsigned long)dec->cap_buf_addr[n];
  }
  buf.m.planes[0].reserved[0] = dec->cap_buf_fd[n];
  buf.m.planes[0].reserved[1] = 0;
  buf.m.planes[0].length = dec->cap_buf_size[0][n];
  buf.m.planes[0].data_offset = 0;

  buf.m.planes[1].m.userptr = (unsigned long)dec->extradata_ion_addr;
  buf.m.planes[1].reserved[0] = dec->extradata_ion_fd;
  buf.m.planes[1].reserved[1] = dec->extradata_off[n];
  buf.m.planes[1].length = dec->extradata_size;
  buf.m.planes[1].bytesused = 0;
  buf.m.planes[1].data_offset = 0;

  if (ioctl(dec->fd, VIDIOC_QBUF, &buf) < 0){
    GST_DEBUG_OBJECT (dec, "failed to QBUF cap buffer");
    return -1;
  }

  GST_DEBUG_OBJECT (dec, "mm buf.m.planes[0].length %d", buf.m.planes[0].length);
  GST_DEBUG_OBJECT (dec, "mm buf.m.planes[0].bytesused %d", buf.m.planes[0].bytesused);
  GST_DEBUG_OBJECT (dec, "mm buf.m.planes[1].length %d", buf.m.planes[1].length);
  GST_DEBUG_OBJECT (dec, "mm buf.m.planes[1].bytesused %d", buf.m.planes[1].bytesused);

  dec->cap_buf_flag[n] = 1;

  return 0;
}

/* add alloc_ion_buffer_secure */
static int
alloc_ion_buffer_secure(size_t size, int type, GstVideoDecoder * decoder)
{
  Gstqtivdec *dec = GST_QTIVDEC (decoder);
  struct ion_allocation_data ion_alloc = { 0 };
  struct ion_fd_data ion_fd_data = { 0 };
  struct ion_handle_data ion_handle_data = { 0 };
  static int ion_fd = -1;
  int ret;
  GST_INFO_OBJECT (dec, "mm inside alloc_ion_buffer_secure");
  if (ion_fd < 0){
    ion_fd = open("/dev/ion", O_RDONLY);
    if (ion_fd < 0){
      GST_ERROR_OBJECT (dec, "Cannot open ion device");
      return -1;
    }
  }
  ion_alloc.len = size;
  ion_alloc.align = 4096;

  if(type == V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE){
    GST_INFO_OBJECT (dec, "mm inside alloc_ion_buffer_secure-1");
    ion_alloc.heap_id_mask = ION_HEAP(ION_SECURE_DISPLAY_HEAP_ID) | ION_HEAP(ION_SECURE_HEAP_ID);
    ion_alloc.flags = ION_SECURE | ION_FLAG_CP_BITSTREAM;
  }else if(type == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE){
    GST_INFO_OBJECT (dec, "mm inside alloc_ion_buffer_secure-2");
    ion_alloc.heap_id_mask = ION_HEAP(ION_SECURE_HEAP_ID);
    ion_alloc.flags = ION_SECURE | ION_FLAG_CP_PIXEL;
  }else{
    GST_ERROR_OBJECT (dec, "buff type error");
    return -1;
  }
  ion_alloc.handle = -1;

  if (v4l2_ioctl(ion_fd, ION_IOC_ALLOC, &ion_alloc) < 0){
    GST_DEBUG_OBJECT (dec, "mm Failed to allocate ion buffer");
    return -1;
  }

  ion_fd_data.handle = ion_alloc.handle;
  ion_fd_data.fd = -1;
  GST_DEBUG_OBJECT (dec, "mm Allocated %zd bytes ION secure buffer %d", ion_alloc.len, ion_alloc.handle);
  if (v4l2_ioctl(ion_fd, ION_IOC_MAP, &ion_fd_data) < 0){
    GST_DEBUG_OBJECT (dec, "mm Failed to map ion secure buffer");
    ret = -1;
  } else {
    GST_DEBUG_OBJECT (dec, "mm success to map ion secure buffer");
    ret = ion_fd_data.fd;
  }

  ion_handle_data.handle = ion_alloc.handle;
  if (ioctl(ion_fd, ION_IOC_FREE, &ion_handle_data) < 0)
  GST_DEBUG_OBJECT (dec, "mm Failed to free ion buffer");
  return ret;
}

/* add alloc_ion_buffer */
static int
alloc_ion_buffer(size_t size, GstVideoDecoder * decoder)
{
  Gstqtivdec *dec = GST_QTIVDEC (decoder);
  struct ion_allocation_data ion_alloc = { 0 };
  struct ion_fd_data ion_fd_data = { 0 };
  struct ion_handle_data ion_handle_data = { 0 };
  static int ion_fd = -1;
  int ret;
  GST_INFO_OBJECT (dec, "mm inside alloc_ion_buffer");
  if (ion_fd < 0){
    ion_fd = open("/dev/ion", O_RDONLY);
    if (ion_fd < 0){
      GST_ERROR_OBJECT (dec, "Cannot open ion device");
      return -1;
    }
  }

  ion_alloc.len = size;
  ion_alloc.align = 4096;
  ion_alloc.heap_id_mask = ION_HEAP(ION_IOMMU_HEAP_ID);
  ion_alloc.flags = 0;
  ion_alloc.handle = -1;

  if (v4l2_ioctl(ion_fd, ION_IOC_ALLOC, &ion_alloc) < 0){
    GST_DEBUG_OBJECT (dec, "mm Failed to allocate ion buffer");
    return -1;
  }

  ion_fd_data.handle = ion_alloc.handle;
  ion_fd_data.fd = -1;
  GST_DEBUG_OBJECT (dec, "mm Allocated %zd bytes ION buffer %d", ion_alloc.len, ion_alloc.handle);
  if (v4l2_ioctl(ion_fd, ION_IOC_MAP, &ion_fd_data) < 0){
    GST_DEBUG_OBJECT (dec, "mm Failed to map ion buffer");
    ret = -1;
  } else {
    GST_DEBUG_OBJECT (dec, "mm success to map ion buffer");
    ret = ion_fd_data.fd;
  }

  ion_handle_data.handle = ion_alloc.handle;
  if (ioctl(ion_fd, ION_IOC_FREE, &ion_handle_data) < 0)
  GST_DEBUG_OBJECT (dec, "mm Failed to free ion buffer");
  return ret;
}

int video_set_secure(GstVideoDecoder * decoder)
{
  Gstqtivdec *dec = GST_QTIVDEC (decoder);
  struct v4l2_control control = {0};
  control.id = V4L2_CID_MPEG_VIDC_VIDEO_SECURE;
  control.value = 1;
  GST_DEBUG_OBJECT (dec, "mm inside video_set_secure");
  if (ioctl(dec->fd, VIDIOC_S_CTRL, &control) < 0) {
    return -1;
  }
return 0;
}

static int
setup_extradata(GstVideoDecoder * decoder, int index, int size)
{
  Gstqtivdec *dec = GST_QTIVDEC (decoder);
  int off = 0;

  dec->extradata_index = index;
  dec->extradata_size = size;

    dec->extradata_ion_fd = alloc_ion_buffer(size * dec->cap_buf_cnt, decoder);

  if (dec->extradata_ion_fd < 0){
      GST_ERROR_OBJECT (dec, "mm failed extradata ion alloc extradata_ion_fd=%d, cap_buf_size=%d, dec->cap_buf_cnt=%d", dec->extradata_ion_fd, size, dec->cap_buf_cnt);
      return -1;
  }
  GST_DEBUG_OBJECT (dec, "mm sucess extradata ion alloc extradata_ion_fd=%d, cap_buf_size=%d, dec->cap_buf_cnt=%d", dec->extradata_ion_fd, size, dec->cap_buf_cnt);

    dec->extradata_ion_addr = mmap(NULL,
            size * dec->cap_buf_cnt,
            PROT_READ|PROT_WRITE,
            MAP_SHARED,
            dec->extradata_ion_fd,
            0);

  if (dec->extradata_ion_addr == MAP_FAILED) {
    GST_ERROR_OBJECT (dec, "mm mmap failed for extradata extradata_ion_fd=%d, cap_buf_size=%d, dec->cap_buf_cnt=%d", dec->extradata_ion_fd, size, dec->cap_buf_cnt);
    return -1;
  }
  GST_DEBUG_OBJECT (dec, "mm mmap sucess for extradata extradata_ion_fd=%d, cap_buf_size=%d, dec->cap_buf_cnt=%d", dec->extradata_ion_fd, size, dec->cap_buf_cnt);

  for (int i = 0; i < dec->cap_buf_cnt; i++) {
    dec->extradata_off[i] = off;
    dec->extradata_addr[i] = (char *)dec->extradata_ion_addr + off;
    GST_DEBUG_OBJECT (dec, "mm extradata extradata_off[%d]=%d, extradata_addr[%i]=%p", i, off, i, dec->extradata_addr[i]);
    off += size;
  }
  return 0;
}

/* Setup input (Output port for video decoder plugin) */
static gboolean
gst_qtivdec_setup_input (GstVideoDecoder * decoder)
{
  Gstqtivdec *dec = GST_QTIVDEC (decoder);
  struct v4l2_requestbuffers reqbuf;
  int ion_fd;
  int ion_fd_secure;
  void *buf_addr;
  int n;
  gint type;
  
  /* Memory mapping for input buffers in V4L2 */
  memset (&reqbuf, 0, sizeof reqbuf);
  reqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
  reqbuf.count = NB_BUF_INPUT;
  reqbuf.memory = V4L2_MEMORY_USERPTR;
  if (v4l2_ioctl (dec->fd, VIDIOC_REQBUFS, &reqbuf) < 0)
    goto error_ioc_reqbufs;

  dec->out_buf_cnt = reqbuf.count;

  dec->input_buf_add = malloc (sizeof (void *) * reqbuf.count);
  dec->input_buf_size = malloc (sizeof (void *) * reqbuf.count);

  GST_INFO_OBJECT(dec, "mm requested %d out buffers, got %d", NB_BUF_INPUT, reqbuf.count);

  if (dec->zero_copy==FALSE) {
    for (n = 0; n < dec->out_buf_cnt; n++){

      ion_fd = alloc_ion_buffer(dec->out_buf_size, decoder);
      if (ion_fd < 0){
        GST_ERROR_OBJECT (dec, "mm ion alloc failed for out buf#%d ion_fd=%d, out_buf_size=%d, out_buf_cnt=%d", n, ion_fd, dec->out_buf_size, dec->out_buf_cnt);
        goto error_ion_alloc_fail;
      }
      GST_DEBUG_OBJECT (dec, "mm ion alloc success for out buf#%d ion_fd=%d, out_buf_size=%d, out_buf_cnt=%d", n, ion_fd, dec->out_buf_size, dec->out_buf_cnt);

      buf_addr = v4l2_mmap(NULL, dec->out_buf_size,
         PROT_READ | PROT_WRITE, MAP_SHARED, ion_fd, 0);
      if (buf_addr == MAP_FAILED){
        GST_DEBUG_OBJECT (dec, "mm mmap failed for out buf#%d ion_fd=%d, out_buf_size=%d, dec->out_buf_cnt=%d", n, ion_fd, dec->out_buf_size, dec->out_buf_cnt);
        goto error_map_fail;
      }
      GST_DEBUG_OBJECT (dec, "mm mmap success for out buf#%d buf_addr=%p, out_buf_size=%d, dec->out_buf_cnt=%d", n, buf_addr, dec->out_buf_size, dec->out_buf_cnt);

      dec->out_ion_fd[n] = ion_fd;
      dec->out_ion_addr[n] = buf_addr;
      dec->input_buf_add[n] = (char *)buf_addr;
      dec->input_buf_size[n] = dec->out_buf_size;
      dec->out_buf_flag[n] = 0;

      if (dec->secure) {
        ion_fd_secure = alloc_ion_buffer_secure(dec->out_buf_size, reqbuf.type, decoder);
        if (ion_fd_secure < 0){
          GST_DEBUG_OBJECT (dec, "mm ion secure alloc failed for out buf#%d ion_fd_secure=%d, out_buf_size=%d, out_buf_cnt=%d", n, ion_fd_secure, dec->out_buf_size, dec->out_buf_cnt);
          goto error_ion_secure_alloc_fail;
        }
      GST_DEBUG_OBJECT (dec, "mm ion secure alloc success for out buf#%d ion_fd_secure=%d, out_buf_size=%d, out_buf_cnt=%d", n, ion_fd_secure, dec->out_buf_size, dec->out_buf_cnt);
      dec->out_ion_fd_secure[n] = ion_fd_secure;
      }
    }
  }
  else {
    GstBuffer *buf;
    gint fd_map;
    gsize offset;
    gpointer base;
    GstMapInfo mapinfo;
    GstAllocator *ion_allocator;

    ion_allocator = gst_ion_memory_create(dec->out_buf_cnt, dec->out_buf_size);

    for (n = 0; n < dec->out_buf_cnt; n++) {
      buf = gst_buffer_new_allocate(ion_allocator, dec->out_buf_size, NULL);
      gst_buffer_map(buf, &mapinfo, GST_MAP_READ);
      gst_ion_buffer_get_memory_info(buf, &fd_map, &offset, &base);
      dec->out_ion_fd[n] = fd_map;
      dec->out_ion_addr[n] = base;
      dec->input_buf_add[n] = (char *) mapinfo.data;
      dec->out_buf_off[n] = offset;
      dec->input_buf_size[n] = dec->out_buf_size;
      dec->out_buf_flag[n] = 0;
      gst_buffer_unmap(buf, &mapinfo);

      if (dec->secure) {
        ion_fd_secure = alloc_ion_buffer_secure(dec->out_buf_size, reqbuf.type, decoder);
        if (ion_fd_secure < 0){
          GST_DEBUG_OBJECT (dec, "mm ion secure alloc failed for out buf#%d ion_fd_secure=%d, out_buf_size=%d, out_buf_cnt=%d", n, ion_fd_secure, dec->out_buf_size, dec->out_buf_cnt);
          goto error_ion_secure_alloc_fail;
        }

        GST_DEBUG_OBJECT (dec, "mm ion secure alloc success for out buf#%d ion_fd_secure=%d, out_buf_size=%d, out_buf_cnt=%d", n, ion_fd_secure, dec->out_buf_size, dec->out_buf_cnt);
        dec->out_ion_fd_secure[n] = ion_fd_secure;
      }
    }
  }

  video_set_control(decoder);

  /* Start stream on input */
  type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
  if (v4l2_ioctl (dec->fd, VIDIOC_STREAMON, &type) < 0)
    goto error_ioc_streamon_out;
  GST_DEBUG_OBJECT (dec, "streamon on output queue - success");

  dec->input_setup = TRUE;

  return TRUE;

  /* Errors */
error_ioc_reqbufs:
  {
    GST_ERROR_OBJECT (dec, "Unable to request input buffers err=%s",
        strerror (errno));
    return FALSE;
  }
error_map_fail:
  {
    GST_ERROR_OBJECT (dec, "Failed to map input buffer");
    return FALSE;
  }
error_ioc_streamon_out:
  {
    GST_ERROR_OBJECT (dec, "Streamon (output) failed err=%s", strerror (errno));
    return FALSE;
  }
error_ion_alloc_fail:
  {
    GST_ERROR_OBJECT (dec, "mm ion alloc failed for out buf#%d ion_fd=%d, out_buf_size=%d, out_buf_cnt=%d", n, ion_fd, dec->out_buf_size, dec->out_buf_cnt);
    return FALSE;
  }
error_ion_secure_alloc_fail:
  {
    GST_DEBUG_OBJECT (dec, "mm ion secure alloc failed for out buf#%d ion_fd_secure=%d, out_buf_size=%d, out_buf_cnt=%d", n, ion_fd_secure, dec->out_buf_size, dec->out_buf_cnt);
    return FALSE;
  }
}

static gboolean
gst_qtivdec_set_format (GstVideoDecoder * decoder, GstVideoCodecState * state)
{
  Gstqtivdec *dec = GST_QTIVDEC (decoder);
  GstStructure *structure;
  const gchar *s;
  gint retval = 0;
  gint width = 0;
  gint height = 0;
  GstVideoInterlaceMode interlace_mode;
  guint32 streamformat;
  gint fd;

  GST_INFO_OBJECT (dec, "mm inside gst_qtivdec_set_format");
  GST_DEBUG_OBJECT (dec, "Setting format: %" GST_PTR_FORMAT, state->caps);

  structure = gst_caps_get_structure (state->caps, 0);

  streamformat = to_v4l2_streamformat (structure);
  if (!streamformat)
    goto error_format;

  retval = gst_structure_get_int (structure, "width", &width);
  retval &= gst_structure_get_int (structure, "height", &height);
  if (!retval)
    goto error_res;

  if ((s = gst_structure_get_string (structure, "interlace-mode")))
    interlace_mode = gst_interlace_mode_from_string (s);
  else
    interlace_mode = GST_VIDEO_INTERLACE_MODE_PROGRESSIVE;

  if (dec->input_setup) {
    /* Already setup, check to see if something has changed on input caps... */
    if ((dec->streamformat == streamformat) &&
        (dec->width == width) && (dec->height == height)) {
      goto done;                /* Nothing has changed */
    } else {
      GST_FIXME_OBJECT (dec, "V4L2_DEC_CMD_STOP in v4l2 driver");
      gst_qtivdec_stop (decoder);
    }
  }

  fd = gst_qtivdec_open_device (dec, streamformat, width, height,
      interlace_mode);
  if (fd == -1)
    goto error_device;

  dec->fd = fd;
  dec->streamformat = streamformat;
  dec->width = width;
  dec->height = height;

  if (dec->input_state)
      gst_video_codec_state_unref (dec->input_state);
  dec->input_state = gst_video_codec_state_ref (state);

  /* Header */
  dec->codec_data = state->codec_data;

  /* subscribe events */
  subscribe_events(dec);

/* check secure buffer */
  if (dec->secure) {
     if(video_set_secure(decoder)<0)
       goto error_secure;
     dec->handle = initCryptoLib();
  }

  if (!gst_qtivdec_setup_input (decoder))
    goto error_setup_input;

done:
  return TRUE;

  /* Errors */
error_format:
  {
    GST_ERROR_OBJECT (dec, "Unsupported format in caps: %" GST_PTR_FORMAT,
        state->caps);
    return FALSE;
  }
error_res:
  {
    GST_ERROR_OBJECT (dec, "Unable to get width/height value");
    return FALSE;
  }
error_device:
  {
    return FALSE;
  }
error_secure:
  {
    GST_ERROR_OBJECT(dec, "failed to set secure mode");
    return FALSE;
  }
error_setup_input:
  {
    GST_ERROR_OBJECT(dec, "failed to setup input");
    return FALSE;
  }
}

static void
unmap_input_buf (Gstqtivdec * dec)
{
  if (dec->input_buf_add)
    for (int i = 0; i < dec->current_nb_buf_input; i++)
      v4l2_munmap (dec->input_buf_add[i], dec->input_buf_size[i]);
  GST_INFO_OBJECT (dec, "mm inside unmap_input_buf");
}

static GstFlowReturn
add_meta_cap_buf (GstVideoDecoder *decoder, GstBuffer *out_buf, struct v4l2_buffer *v4l2buf)
{
  Gstqtivdec *dec = GST_QTIVDEC (decoder);
  GstIonBufFdMeta *meta;
  GstVideoInfo *info = &dec->output_state->info;
  GstVideoFrameFlags flags = 0;
  gint chroma_offset;
  gint chroma_alignment;

  meta = gst_buffer_add_ionbuf_meta(out_buf, dec->cap_buf_fd[v4l2buf->index], dec->cap_buf_meta_fd[v4l2buf->index], dec->width, dec->height, dec->cap_buf_format);

  if (!meta){
    GST_ERROR_OBJECT (dec, "Addition of ionBufInfo metadata to decoder output buffer failed.\n");
    goto out;
  }

  GST_DEBUG_OBJECT (dec, "mm inside meta. out_buf=%p, v4l2buf->index=%d, cap_buf_fd[v4l2buf->index]=%d  \
  cap_buf_meta_fd[v4l2buf->index]=%d, \
  cap_buf_size[0]=%d, \
  width=%u, \
  height=%u, \
  capture format=%u, \
  v4l2buf->m.planes[0].m.userptr=%lu, \
  v4l2buf->m.planes[0].reserved[0]=%d", out_buf, v4l2buf->index, dec->cap_buf_fd[v4l2buf->index], \
  dec->cap_buf_meta_fd[v4l2buf->index], \
  dec->cap_buf_size[0][v4l2buf->index], \
  dec->width, \
  dec->height, \
  dec->cap_buf_format, \
  v4l2buf->m.planes[0].m.userptr, v4l2buf->m.planes[0].reserved[0]);

  /* TODO: fix time stamp */
  /*  GST_BUFFER_TIMESTAMP(out_buf) = GST_TIMEVAL_TO_TIME(dqbuf.timestamp); */

  /* set top/bottom field first if v4l2_buffer has the information */
  if ((v4l2buf->field == V4L2_FIELD_INTERLACED_TB) ||
      (v4l2buf->field == V4L2_FIELD_INTERLACED)) {
    GST_BUFFER_FLAG_SET (out_buf, GST_VIDEO_BUFFER_FLAG_INTERLACED);
    GST_BUFFER_FLAG_SET (out_buf, GST_VIDEO_BUFFER_FLAG_TFF);
  } else if (v4l2buf->field == V4L2_FIELD_INTERLACED_BT) {
    GST_BUFFER_FLAG_SET (out_buf, GST_VIDEO_BUFFER_FLAG_INTERLACED);
    GST_BUFFER_FLAG_UNSET (out_buf, GST_VIDEO_BUFFER_FLAG_TFF);
  } else {
    /* per default, the frame is considered as progressive */
    GST_BUFFER_FLAG_UNSET (out_buf, GST_VIDEO_BUFFER_FLAG_INTERLACED);
    GST_BUFFER_FLAG_UNSET (out_buf, GST_VIDEO_BUFFER_FLAG_TFF);
  }

  /* buffer flags enhance the meta flags */
  if (GST_BUFFER_FLAG_IS_SET (out_buf, GST_VIDEO_BUFFER_FLAG_INTERLACED))
    flags |= GST_VIDEO_FRAME_FLAG_INTERLACED;
  if (GST_BUFFER_FLAG_IS_SET (out_buf, GST_VIDEO_BUFFER_FLAG_TFF))
    flags |= GST_VIDEO_FRAME_FLAG_TFF;

  /* fill alignment/offset */
  info->stride[LUMA_PLANE] = GST_ROUND_UP_128 (info->width);
  info->stride[CHROMA_PLANE] = info->stride[LUMA_PLANE];

  chroma_alignment = GST_ROUND_UP_128 (info->width) *
                     GST_ROUND_UP_32 (info->height);
  chroma_offset = GST_ROUND_UP_N (chroma_alignment, 4096);

  info->offset[CHROMA_PLANE] = chroma_offset;

  GST_INFO_OBJECT (dec, "mm inside push_frame_downstream-4");

  /* Add alignment info */

  gst_buffer_add_video_meta_full (out_buf,
                                  flags,
                                  GST_VIDEO_INFO_FORMAT (info),
                                  GST_VIDEO_INFO_WIDTH (info),
                                  GST_VIDEO_INFO_HEIGHT (info),
                                  GST_VIDEO_INFO_N_PLANES (info),
                                  info->offset, info->stride);

  return GST_FLOW_OK;

out:
  return GST_FLOW_ERROR;
}

static GstFlowReturn
push_frame_downstream(GstVideoDecoder *decoder)
{
  Gstqtivdec *dec = GST_QTIVDEC (decoder);
  GstBuffer *output_buffer;
  GstVideoCodecFrame *frame;
  GstFlowReturn ret = GST_FLOW_OK;
  void *extradata_addr = NULL;

  struct v4l2_buffer dqbuf;
  struct v4l2_plane planes[CAP_PLANES];
  GST_INFO_OBJECT (dec, "mm inside push_downstream");

  GST_DEBUG_OBJECT (dec, "trying to dequeue frame");
  memset (&dqbuf, 0, sizeof dqbuf);
  dqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
  dqbuf.memory = V4L2_MEMORY_USERPTR;
  dqbuf.m.planes = planes;
  dqbuf.length = CAP_PLANES;

  if (v4l2_ioctl (dec->fd, VIDIOC_DQBUF, &dqbuf) < 0){
    GST_ERROR_OBJECT (dec, "mm failed dequeue buffer (capture type), dqbuf.index %d", dqbuf.index);
    goto out; /* STREAMOFF */
  }
  GST_DEBUG_OBJECT (dec, "dequeue buffer (capture type) dqbuf.index %d",
                    dqbuf.index);
  if(dec->depth == 10) {
    GST_DEBUG ("extradata_addr[dqbuf.index] address : %p", dec->extradata_addr[dqbuf.index]);

    extradata_addr = dec->extradata_addr[dqbuf.index];

    if(extradata_addr)
      parse_populate_and_send_extradata(dec, extradata_addr, dec->extradata_size,dqbuf.index);
  }

  output_buffer = gst_buffer_new ();

  if(!output_buffer){
    GST_ERROR_OBJECT (dec, "mm failed alloc gst buffer)");
    goto out;
  }

  ret = add_meta_cap_buf (decoder, output_buffer, &dqbuf);
  if (ret != GST_FLOW_OK)
    goto out;

  frame = gst_video_decoder_get_oldest_frame (decoder);
  if (frame == NULL){
    GST_INFO_OBJECT (dec, "frame is NULL. mm inside push_frame_downstream-2");
    return GST_FLOW_EOS;;
  }
  GST_INFO_OBJECT (dec, "oldest_frame is = %p", frame);

  frame->output_buffer = output_buffer;
  frame->pts = output_buffer->pts;

  GST_INFO_OBJECT (dec, "mm inside push_frame_downstream-5");

  /* Decrease the refcount of the frame so that the frame is released by the
   * gst_video_decoder_finish_frame function and so that the output buffer is
   * writable when it's pushed downstream */
  gst_video_codec_frame_unref (frame);
  GST_INFO_OBJECT (dec, "mm inside push_frame_downstream-6");
  gst_video_decoder_finish_frame (decoder, frame);
  GST_INFO_OBJECT (dec, "mm inside push_frame_downstream-7");
  GST_DEBUG_OBJECT (dec, "-->Frame pushed buffer %p", output_buffer);

  if (queue_buf_cap(decoder, dqbuf.index)){
    GST_DEBUG_OBJECT (dec, "Enqueuing of cap buffers failed after save frame");
    return -1;
  }
  GST_DEBUG_OBJECT (dec, "Enqueuing of cap buffers success after save frame");
  return GST_FLOW_OK;

out:
  return GST_FLOW_ERROR;
}

/* The thread will capture the events
* 1- when v4l2 ready to accept new incomming frame. Dequeue output buf.
* 2- when v4l2 decoded frame ready to be pushed downstream.
*/
static void *
poll_out_thread (void *arg)
{
  GstVideoDecoder *decoder = (GstVideoDecoder *) arg;
  Gstqtivdec *dec = GST_QTIVDEC (decoder);
  GstFlowReturn ret = GST_FLOW_OK;
  gint exit = 0;
  struct pollfd pfd;
  short revents;
  struct v4l2_buffer dqbuf;
  struct v4l2_plane planes[OUT_PLANES];
  GST_INFO_OBJECT (dec, "mm inside poll_out_thread");
  GST_DEBUG_OBJECT (dec, "Poll out thread started");

  pfd.fd = dec->fd;
  pfd.events = POLLOUT | POLLWRNORM | POLLIN | POLLRDNORM | POLLPRI;

  while (exit == 0) {

    ret = poll(&pfd, 1, 10000);
    if (!ret) {
      GST_ERROR_OBJECT (dec, "Poll timeout error");
      continue;
    } else if (ret < 0) {
      GST_ERROR_OBJECT (dec, "Poll error %s", strerror (errno));
      break;
    }
    GST_LOG_OBJECT (dec, "mm inside poll_out_thread-1");
    revents = pfd.revents;

    if ((revents & (POLLIN | POLLRDNORM)) && (!dec->reconfigure_pending) && (!dec->in_flush)) {
      GST_DEBUG_OBJECT (dec, "revent for capture buffer type");
      ret = push_frame_downstream (decoder);
      if (ret != GST_FLOW_OK)
        GST_ERROR_OBJECT (dec, "Failed to push frame downstream");
      else
        GST_LOG_OBJECT (dec, "mm inside poll_out_thread-2");
    }

    if (revents & (POLLOUT | POLLWRNORM)) {

      GST_DEBUG_OBJECT (dec, "revent for output buffer type");

      /* Dequeue a V4L2 buffer where to write */
      memset (&dqbuf, 0, sizeof dqbuf);
      memset (planes, 0, sizeof(planes));
      dqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
      dqbuf.memory = V4L2_MEMORY_USERPTR;
      dqbuf.m.planes = planes;
      dqbuf.length = OUT_PLANES;
      if (v4l2_ioctl (dec->fd, VIDIOC_DQBUF, &dqbuf) < 0) {
        GST_ERROR_OBJECT (dec, "Error on output dqbuf %s", strerror (errno));
        continue;
      }
      GST_DEBUG_OBJECT (dec, "dequeued out buffer %d", dqbuf.index);

      pthread_mutex_lock(&dec->lock);
      dec->out_buf_flag[dqbuf.index] = 0;
      pthread_mutex_unlock(&dec->lock);

      sem_post (&dec->sem_done);
    }

    if (revents & POLLPRI) {
      GST_DEBUG ("In POLLPRI");
      handle_video_event(decoder);
    }

  }

  GST_DEBUG_OBJECT (dec, "-->Leaving the pool out thread!");

  return NULL;
}

/* setup Capture buffers */
static GstFlowReturn
gst_qtivdec_setup_output (GstVideoDecoder * decoder)
{
  Gstqtivdec *dec = GST_QTIVDEC (decoder);
  GstFlowReturn ret = GST_FLOW_OK;
  GstVideoAlignment align;
  struct v4l2_format g_fmt, s_fmt;
  struct v4l2_crop g_crop;
  struct v4l2_requestbuffers reqbufs;
  gint type;
  guint32 width, height, aligned_width, aligned_height;
  guint32 fmt;
  void *buf_addr;
  guint n;
#ifdef GBM_ENABLED
  struct gbm_bo *bo;
  guint32 size;
  gint meta_fd;
#else
  gint ion_fd;
#endif

  video_set_dpb(dec, dec->depth == 10 ?
                  V4L2_MPEG_VIDC_VIDEO_DPB_COLOR_FMT_TP10_UBWC :
                  V4L2_MPEG_VIDC_VIDEO_DPB_COLOR_FMT_NONE);

  GST_INFO_OBJECT (dec, "mm inside gst_qtivdec_setup_output");
  memset (&s_fmt, 0, sizeof s_fmt);
  s_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
  s_fmt.fmt.pix_mp.width = dec->width;
  s_fmt.fmt.pix_mp.height = dec->height;

  if (dec->depth == 8)
    s_fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_NV12;
  else
    s_fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_NV12_TP10_UBWC;

  ret = v4l2_ioctl (dec->fd, VIDIOC_S_FMT, &s_fmt);
  if (ret != 0)
    goto done;

  /* Get output frame format from V4L2 */
  memset (&g_fmt, 0, sizeof g_fmt);
  g_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
  ret = v4l2_ioctl (dec->fd, VIDIOC_G_FMT, &g_fmt);
  if (ret != 0)
    goto done;

  GST_DEBUG_OBJECT (dec,
      "Format found from V4L2 :fmt:%s, width:%d, height:%d, "
      "bytesperline:%d, sizeimage:%d, pixelfmt:%s, field:%d",
      v4l2_type_str (g_fmt.type), g_fmt.fmt.pix_mp.width,
      g_fmt.fmt.pix_mp.height, g_fmt.fmt.pix_mp.plane_fmt[0].bytesperline,
      g_fmt.fmt.pix_mp.plane_fmt[0].sizeimage,
      v4l2_fmt_str (g_fmt.fmt.pix_mp.pixelformat),
      g_fmt.fmt.pix_mp.field);

  aligned_width = g_fmt.fmt.pix_mp.width;
  aligned_height = g_fmt.fmt.pix_mp.height;
  fmt = g_fmt.fmt.pix_mp.pixelformat;

  /* Compute padding */
  gst_video_alignment_reset (&align);
  memset (&g_crop, 0, sizeof g_crop);
  g_crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
  if (v4l2_ioctl (dec->fd, VIDIOC_G_CROP, &g_crop) < 0) {
    GST_DEBUG_OBJECT (dec, "Not able to get crop, default to %dx%d",
        aligned_width, aligned_height);
    width = aligned_width;
    height = aligned_height;
  } else {
    GST_DEBUG_OBJECT (dec,
        "Crop found from V4L2 :fmt:%s, left:%d, top:%d, width:%d, height:%d",
        v4l2_type_str (g_crop.type), g_crop.c.left, g_crop.c.top,
        g_crop.c.width, g_crop.c.height);
    width = g_crop.c.width;
    height = g_crop.c.height;

    align.padding_left = g_crop.c.left;
    align.padding_top = g_crop.c.top;
    align.padding_right = aligned_width - align.padding_left - width;
    align.padding_bottom = aligned_height - align.padding_top - height;

    GST_DEBUG_OBJECT (dec,
        "Padding information deduced from V4L2 G_FMT/G_CROP: padding_left:%d, padding_right:%d, "
        "padding_top:%d, padding_bottom:%d",
        align.padding_left, align.padding_right,
        align.padding_top, align.padding_bottom);
  }

  dec->output_state =
      gst_video_decoder_set_output_state (GST_VIDEO_DECODER (dec),
      GST_VIDEO_FORMAT_NV12, width, height, dec->input_state);

  switch (g_fmt.fmt.pix.field) {
    case V4L2_FIELD_NONE:
      dec->output_state->info.interlace_mode =
          GST_VIDEO_INTERLACE_MODE_PROGRESSIVE;
      break;
    case V4L2_FIELD_INTERLACED:
    case V4L2_FIELD_INTERLACED_TB:
    case V4L2_FIELD_INTERLACED_BT:
      dec->output_state->info.interlace_mode =
          GST_VIDEO_INTERLACE_MODE_INTERLEAVED;
      break;
    default:
      /* keep the interlace mode as specified by the CAPS */
      break;
  }

  /* Set buffer alignment */
  gst_video_info_align (&dec->output_state->info, &align);
  dec->align = align;

  /* Set buffer size */
  dec->size_image = g_fmt.fmt.pix_mp.plane_fmt[0].sizeimage;
  dec->cap_buf_format = fmt;
  dec->cap_w = width;
  dec->cap_h = height;
  dec->cap_buf_stride[0] = g_fmt.fmt.pix_mp.plane_fmt[0].bytesperline;
  dec->cap_buf_stride[1] = g_fmt.fmt.pix_mp.plane_fmt[1].bytesperline;

  GST_DEBUG_OBJECT (dec, "mm g_fmt.fmt.pix_mp.plane_fmt[0].sizeimage=%d", g_fmt.fmt.pix_mp.plane_fmt[0].sizeimage);
  GST_DEBUG_OBJECT (dec, "mm g_fmt.fmt.pix_mp.plane_fmt[0].bytesperline=%d", g_fmt.fmt.pix_mp.plane_fmt[0].bytesperline);
  GST_DEBUG_OBJECT (dec, "mm g_fmt.fmt.pix_mp.plane_fmt[1].sizeimage=%d", g_fmt.fmt.pix_mp.plane_fmt[1].sizeimage);
  GST_DEBUG_OBJECT (dec, "mm g_fmt.fmt.pix_mp.plane_fmt[1].bytesperline=%d", g_fmt.fmt.pix_mp.plane_fmt[1].bytesperline);

  memset (&reqbufs, 0, sizeof reqbufs);
  reqbufs.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
  reqbufs.count = NB_BUF_OUTPUT;
  reqbufs.memory = V4L2_MEMORY_USERPTR;

  if (v4l2_ioctl (dec->fd, VIDIOC_REQBUFS, &reqbufs) < 0){
    GST_INFO_OBJECT(dec, "mm failed VIDIOC_REQBUFS");
    goto error_ioc_reqbufs;
  }

  /* Add 4 extra buffers to aid in timely rendering on display; cannot allocate max buffers (32) as in secure mode,
     Graphics can only support 256 MB of memory and 32 4k resolution buffers would exceed that */
  reqbufs.count = reqbufs.count + 4;

  if (v4l2_ioctl (dec->fd, VIDIOC_REQBUFS, &reqbufs) < 0){
    GST_INFO_OBJECT(dec, "mm failed VIDIOC_REQBUFS");
    goto error_ioc_reqbufs;
  }

  dec->cap_buf_cnt = reqbufs.count;
  GST_INFO_OBJECT(dec, "mm requested %d cap buffers, got %d", NB_BUF_OUTPUT, reqbufs.count);

  metadata_perform_map = (char*) malloc(dec->cap_buf_cnt*sizeof(char));
  memset((void*)metadata_perform_map, 0, dec->cap_buf_cnt);

#ifndef GBM_ENABLED
  for (n = 0; n < dec->cap_buf_cnt; n++) {
    dec->cap_buf_size[0][n] = g_fmt.fmt.pix_mp.plane_fmt[0].sizeimage;
    dec->cap_buf_size[1][n] = g_fmt.fmt.pix_mp.plane_fmt[1].sizeimage;
    if(dec->secure){
      ion_fd = alloc_ion_buffer_secure(dec->cap_buf_size[0][n], reqbufs.type, decoder);
    }else{
      ion_fd = alloc_ion_buffer(dec->cap_buf_size[0][n], decoder);
    }
    if (ion_fd < 0){
      GST_ERROR_OBJECT (dec, "mm failed ion alloc for cap buffers ion_fd=%d, cap_buf_size=%d, cap_buf_cnt=%d", ion_fd, dec->cap_buf_size[0][n], dec->cap_buf_cnt);
      return -1;
    }
    GST_DEBUG_OBJECT (dec, "mm success ion alloc for cap buffers ion_fd=%d, cap_buf_size=%d, cap_buf_cnt=%d", ion_fd, dec->cap_buf_size[0][n], dec->cap_buf_cnt);
    if(!dec->secure){
      buf_addr = mmap(NULL, dec->cap_buf_size[0][n],
      PROT_READ, MAP_SHARED, ion_fd, 0);
      if (buf_addr == MAP_FAILED) {
        GST_ERROR_OBJECT (dec, "mm mmap failed for cap buffers ion_fd=%d, cap_buf_size=%d, cap_buf_cnt=%d", ion_fd, dec->cap_buf_size[0][n], dec->cap_buf_cnt);
        return -1;
      }
      GST_DEBUG_OBJECT (dec, "mm mmap success for cap buffers buf_addr=%p, cap_buf_size=%d, cap_buf_cnt=%d", buf_addr, dec->cap_buf_size[0][n], dec->cap_buf_cnt);
      dec->cap_buf_addr[n] = buf_addr;
    }
    dec->cap_buf_fd[n] = ion_fd;
  }
#else
  if (!dec->gbm) {
    dec->fd_card0 = open("/dev/dri/card0", O_RDWR);
    if (dec->fd_card0 < 0) {
      GST_ERROR_OBJECT(dec, "failed to open dri device for gbm");
      return -1;
    }
    GST_INFO_OBJECT (dec, "Successfully open dri device for gbm");
    dec->gbm = gbm_create_device(dec->fd_card0);
    if (!dec->gbm) {
      GST_ERROR_OBJECT(dec, "Failed to create gbm device");
      v4l2_close(dec->fd_card0);
      return -1;
    }else
      GST_INFO_OBJECT (dec, "Successfully created gbm device");
  }

  GST_INFO_OBJECT (dec, "gbm bo Credentials: Width=%d, Height=%d, Format=0x%x",
                                                    width, height, fmt);

  for (n = 0; n < dec->cap_buf_cnt; n++) {
    if (dec->secure){
      bo = gbm_bo_create(dec->gbm, width, height,
               fmt, GBM_BO_USAGE_PROTECTED_QTI);
    }else{
      bo = gbm_bo_create(dec->gbm, width, height, fmt, 0);
    }

    if (!bo) {
      GST_ERROR_OBJECT (dec, "Failed to create BO at count =%d",n);
      return -1;
    }

    if(!dec->secure){
      if (gbm_perform(GBM_PERFORM_CPU_MAP_FOR_BO, bo, &buf_addr)) {
        GST_ERROR_OBJECT (dec, "failed to get bo buf_addr for buffer%d",n);
        return -1;
      }
    }

    if (gbm_perform(GBM_PERFORM_GET_BO_SIZE, bo, &size)) {
      GST_ERROR_OBJECT (dec, "failed to get bo size for buffer%d",n);
      return -1;
    }

    if(gbm_perform(GBM_PERFORM_GET_METADATA_ION_FD, bo, &meta_fd)) {
      GST_ERROR_OBJECT (dec, "failed to get meta fd for bo count =%d",n);
      return -1;
    }

    dec->cap_buf_bo[n]   = bo;
    dec->cap_buf_fd[n]   = gbm_bo_get_fd(bo);
    dec->cap_buf_meta_fd[n] = meta_fd;

    if(!dec->secure){
      dec->cap_buf_addr[n] = buf_addr;
    }
    dec->cap_buf_size[0][n] = size;
    dec->cap_buf_size[1][n] = g_fmt.fmt.pix_mp.plane_fmt[1].sizeimage;

    GST_DEBUG_OBJECT (dec, "mm success gbm alloc for capbuff#%d gbm_fd=%d, cap_buf_size=%d, cap_buf_cnt=%d, bo=%p", n, dec->cap_buf_fd[n], dec->cap_buf_size[0][n], dec->cap_buf_cnt, bo);
  }
#endif

  /* prepare extradata */
  setup_extradata(decoder, 1, g_fmt.fmt.pix_mp.plane_fmt[1].sizeimage);

  /* Queue all capture buffers */
  for (n = 0; n < dec->cap_buf_cnt; n++) {
    if (queue_buf_cap(decoder, n)){
      GST_DEBUG_OBJECT (dec, "Enqueuing of cap buffers failed");
      return -1;
    }
  }
  GST_DEBUG_OBJECT (dec, "Enqueuing of cap buffers success");

  /* Start streaming on output */
  type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
  if (v4l2_ioctl (dec->fd, VIDIOC_STREAMON, &type) < 0)
    goto error_ioc_streamon;

  GST_DEBUG_OBJECT (dec, "streamon on capture queue - success");

  if(!dec->reconfigure_pending)
  pthread_create (&dec->poll_out_thread, 0, poll_out_thread, decoder);

  dec->output_setup = TRUE;

done:
  return GST_FLOW_OK;

  /* Errors */
error_ioc_reqbufs:
  {
    GST_ERROR_OBJECT (dec, "Unable to request buffers err=%s",
        strerror (errno));
    return GST_FLOW_ERROR;
  }
error_ioc_streamon:
  {
    GST_ERROR_OBJECT (dec, "Streamon failed err=%s", strerror (errno));
    return GST_FLOW_ERROR;
  }
}

/* If output not setup yet then setup output
 * else : decode for each frame available */
static GstFlowReturn
gst_qtivdec_handle_frame (GstVideoDecoder * decoder, GstVideoCodecFrame * frame)
{
  Gstqtivdec *dec = GST_QTIVDEC (decoder);
  GstFlowReturn ret = GST_FLOW_OK;
  GST_INFO_OBJECT (dec, "mm inside gst_qtivdec_handle_frame-1");
  if (!dec->input_setup)
    return GST_FLOW_OK;

  /* Setup output if not yet done */
  if (!dec->output_setup) {
    ret = gst_qtivdec_setup_output (decoder);
    if (ret)
      return ret;
  }

  GST_INFO_OBJECT (dec, "mm inside gst_qtivdec_handle_frame-2 frame address is %p", frame);
  GST_DEBUG ("Enter with frame number : %d, TS: %"
  GST_TIME_FORMAT ", system_frame_number: %d, distance: %d, count: %d",
  frame->system_frame_number, GST_TIME_ARGS (frame->pts),
  frame->system_frame_number, frame->distance_from_sync, frame->ref_count);

  /* For every frame, decode */
  if (frame)
    return gst_qtivdec_decode (decoder, frame);
  else
    return GST_FLOW_EOS;
}

gboolean 
gst_v4l2dec_propose_allocation(GstVideoDecoder *decoder,
                               GstQuery *query) {
    Gstqtivdec *dec = GST_QTIVDEC (decoder);
    const gsize size = dec->out_buf_size;
    const gint min = dec->out_buf_cnt;
    GstAllocator *allocator;
    gboolean need_pool = FALSE;
    GstCaps *caps=NULL;

    GST_INFO_OBJECT(dec, "debug ---> %s number of buffers : %d, size : 0x%ld",
                    __func__, min, size);

    if (dec->zero_copy==FALSE) {
        return FALSE;
    }

    gst_query_parse_allocation(query, &caps, &need_pool);

    if(need_pool==FALSE) {
      return FALSE;
    }

    allocator = gst_ion_memory_allocator_get();
    gst_query_add_allocation_param(query, allocator, NULL);

    GST_INFO_OBJECT(dec, "debug ---> %s: allocator, %p", __func__, allocator);

    return TRUE;
}


/* push frame to V4L2 */
static GstFlowReturn
gst_qtivdec_decode (GstVideoDecoder * decoder, GstVideoCodecFrame * frame)
{
  GstMapInfo mapinfo = { 0, };
  GstBuffer *buf = NULL;
  guint8 *gstdata;
  static gint n =0;
  uint flags = 0;
  gsize gstsize;
  struct v4l2_buffer dqbuf;
  struct v4l2_buffer qbuf;
  struct v4l2_plane planes[OUT_PLANES];

  /* zero-copy */
  gint fd_map;
  gsize offset;
  gpointer base;
  gboolean is_ion_buffer;

  Gstqtivdec *dec = GST_QTIVDEC (decoder);

  GST_INFO_OBJECT (dec, "mm inside gst_qtivdec_decode");
  pthread_mutex_lock(&dec->lock);
  n = n % dec->out_buf_cnt;
  GST_DEBUG_OBJECT (dec, "inside gst_qtivdec_decode - n = %d, ", n);
  pthread_mutex_unlock(&dec->lock);

  if (n >= dec->out_buf_cnt) {
    goto error_no_bufs;
  }

  /* copy GST -> V4L2 */
  buf = frame->input_buffer;

  gst_buffer_map (buf, &mapinfo, GST_MAP_READ);
  gstdata = mapinfo.data;
  gstsize = mapinfo.size;

  if (gstsize > dec->input_buf_size[n]) {
    GST_ERROR_OBJECT (dec,
        "Size exceed, dest (%d) smaller than source (%" G_GSIZE_FORMAT ")\n",
        dec->input_buf_size[dqbuf.index], gstsize);
    gst_buffer_unmap (buf, &mapinfo);
    return GST_FLOW_ERROR;
  } 

  is_ion_buffer = gst_is_ion_buffer(buf);
  if(is_ion_buffer) {
      GST_DEBUG_OBJECT (dec, "debug ---> using ion buffer");
      gst_ion_buffer_get_memory_info(buf, &fd_map, &offset, &base);

      if (dec->secure) {
        copySecureBuf(dec->handle, 0, gstdata, dec->out_ion_fd_secure[n], gstsize);
      }
  }
  else {
      memcpy (dec->input_buf_add[n], gstdata, gstsize);

      if (dec->secure) {
        copySecureBuf(dec->handle, 0, dec->input_buf_add[n], dec->out_ion_fd_secure[n], gstsize);
      }
  }

  GST_DEBUG_OBJECT (dec, "mm memcpy dec->input_buf_add[%d]=%p, gstdata=%p, gstsize (%" G_GSIZE_FORMAT ")", n, dec->input_buf_add[n], gstdata, gstsize);
  GST_INFO_OBJECT (dec, "mm inside gst_qtivdec_decode frame address is %p", frame);
  gst_buffer_unmap (buf, &mapinfo);

  /* Unlock decoder before qbuf call:
   * qbuf will eventually block till frames
   * recycled
   */
  GST_VIDEO_DECODER_STREAM_UNLOCK (decoder);

  /* push frame. Queue V4L2 buffer */
  memset(&qbuf, 0, sizeof(qbuf));
  memset(planes, 0, sizeof(planes));
  qbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
  qbuf.memory = V4L2_MEMORY_USERPTR;
  qbuf.index = n;
  qbuf.length = OUT_PLANES;
  qbuf.m.planes = planes;
  qbuf.m.planes[0].length = dec->input_buf_size[qbuf.index];
  qbuf.m.planes[0].bytesused = gstsize;     /* access unit size */

  if(dec->secure){
    qbuf.m.planes[0].m.userptr = (unsigned long)dec->out_ion_fd_secure[n];
    qbuf.m.planes[0].reserved[0] = dec->out_ion_fd_secure[n];
    qbuf.m.planes[0].reserved[1] = 0;
  }else{
    if (is_ion_buffer) {
      qbuf.m.planes[0].m.userptr = (unsigned long) base;
      qbuf.m.planes[0].reserved[0] = fd_map;
      qbuf.m.planes[0].reserved[1] = offset;
    }
    else {
      qbuf.m.planes[0].m.userptr = (unsigned long)dec->out_ion_addr[n];
      qbuf.m.planes[0].reserved[0] = dec->out_ion_fd[n];
      qbuf.m.planes[0].reserved[1] = dec->out_buf_off[n];
    }
  }
  qbuf.m.planes[0].data_offset = 0;

  /* access unit timestamp */
  if (GST_CLOCK_TIME_IS_VALID (frame->dts)){
    GST_TIME_TO_TIMEVAL (frame->dts, qbuf.timestamp);
    GST_INFO_OBJECT (dec, "frame->dts (%" G_GUINT64_FORMAT ")", frame->dts);
  }else{
    GST_TIME_TO_TIMEVAL (frame->pts, qbuf.timestamp);
    GST_INFO_OBJECT (dec, "frame->pts (%" G_GUINT64_FORMAT ")", frame->pts);
  }

  /* TODO: fix timestamp */
  flags |= V4L2_QCOM_BUF_TIMESTAMP_INVALID;
  qbuf.flags = flags;

  if (v4l2_ioctl (dec->fd, VIDIOC_QBUF, &qbuf) < 0) {
    GST_DEBUG_OBJECT (dec, "Enqueuing of out buffers failed");
    GST_INFO_OBJECT (dec, "n=%d", n);
    GST_VIDEO_DECODER_STREAM_LOCK (decoder);
    goto error_ioctl_enqueue;
  }

  pthread_mutex_lock(&dec->lock);
  dec->out_buf_flag[n] = 1;
  pthread_mutex_unlock(&dec->lock);

  GST_DEBUG_OBJECT (dec, "Enqueuing of out buffers success out_buf_flag[%d]=%d, ", n, dec->out_buf_flag[n]);

  if (qbuf.flags & V4L2_BUF_FLAG_ERROR) {
    GST_DEBUG_OBJECT (dec, "qbuf.flags & V4L2_BUF_FLAG_ERROR");
    GST_VIDEO_CODEC_FRAME_SET_DECODE_ONLY (frame);
    gst_video_decoder_finish_frame (decoder, frame);
  }

  /* Wait the poll_out_thread to dequeue output buffer with
   * GST_VIDEO_DECODER_STREAM_UNLOCK.
   */
  sem_wait (&dec->sem_done);
  GST_DEBUG_OBJECT (dec, "mm inside gst_qtivdec_decode -1");

  GST_VIDEO_DECODER_STREAM_LOCK (decoder);

  GST_DEBUG_OBJECT (dec, "mm inside gst_qtivdec_decode -2");
  n++;
  return GST_FLOW_OK;

  /* ERRORS */
error_no_bufs:
  {
    GST_ERROR_OBJECT (dec, "No output buffers");
    return GST_FLOW_ERROR;
  }
error_ioctl_enqueue:
  {
    GST_ERROR_OBJECT (dec, "Enqueuing failed err=%s", strerror (errno));
    return GST_FLOW_ERROR;
  }
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    qtivdec,
    "QTI GST video decoder",
    plugin_init, VERSION, GST_LICENSE_UNKNOWN, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN);
