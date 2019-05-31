/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/audio/gstaudioencoder.h>
#include "gstomxg711enc.h"
#include "QOMX_AudioExtensions.h"
#include "QOMX_AudioIndexExtensions.h"


GST_DEBUG_CATEGORY_STATIC (gst_qomx_g711enc_debug_category);
#define GST_CAT_DEFAULT gst_qomx_g711enc_debug_category

#define false 0
#define true 1

#define CONFIG_VERSION_SIZE(param) \
    param.nVersion.nVersion = CURRENT_OMX_SPEC_VERSION;\
    param.nSize = sizeof(param);

#define FAILED(result) (result != OMX_ErrorNone)

#define SUCCEEDED(result) (result == OMX_ErrorNone)

#define DEBUG_PRINT printf

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164
#define ID_FACT 0x74636166

#define FORMAT_PCM   0x0001
#define FORMAT_ALAW  0x0006
#define FORMAT_MULAW 0x0007

pthread_mutex_t lock;
pthread_cond_t cond;
pthread_mutex_t elock;
pthread_cond_t econd;
pthread_cond_t fcond;
pthread_mutex_t etb_lock;
pthread_mutex_t etb_lock1;
pthread_cond_t etb_cond;

volatile int event_is_done = 0;
volatile int ebd_event_is_done = 0;
volatile int fbd_event_is_done = 0;
volatile int etb_event_is_done = 0;
int ebd_cnt;

struct __attribute__((__packed__)) g711_header {
  guint32 riff_id;
  guint32 riff_sz;
  guint32 riff_fmt;
  guint32 fmt_id;
  guint32 fmt_sz;
  guint16 audio_format;
  guint16 num_channels;
  guint32 sample_rate;
  guint32 byte_rate;       /* sample_rate * num_channels * bps / 8 */
  guint16 block_align;     /* num_channels * bps / 8 */
  guint16 bits_per_sample;
  guint16 extension_size;
  guint32 fact_id;
  guint32 fact_sz;
  guint32 sample_length;
  guint32 data_id;
  guint32 data_sz;
};

struct enc_meta_out{
        unsigned int offset_to_frame;
        unsigned int frame_size;
        unsigned int encoded_pcm_samples;
        unsigned int msw_ts;
        unsigned int lsw_ts;
        unsigned int nflags;
} __attribute__ ((packed));

static struct g711_header g711hdr;

/* prototypes */


static GstStateChangeReturn
gst_omx_audio_g711_enc_change_state (GstElement * element,
    GstStateChange transition);
static void gst_qomx_g711enc_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec);
static void gst_qomx_g711enc_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec);

static gboolean gst_qomx_g711enc_set_format (GstAudioEncoder * encoder,
    GstAudioInfo * info);
static GstFlowReturn gst_qomx_g711enc_handle_frame (GstAudioEncoder * encoder,
    GstBuffer * buffer);
static void gst_qomx_g711enc_flush (GstAudioEncoder * encoder);
static gboolean gst_qomx_g711enc_open (GstAudioEncoder * encoder);
static gboolean gst_qomx_g711enc_close (GstAudioEncoder * encoder);

static void gst_omx_g711_enc_loop (GstOmxg711enc *omxg711enc);

static OMX_ERRORTYPE Allocate_Buffer ( OMX_COMPONENTTYPE *g711_enc_handle,
                                       OMX_BUFFERHEADERTYPE  ***pBufHdrs,
                                       OMX_U32 nPortIndex,
                                       unsigned int bufCntMin, unsigned int bufSize);


static OMX_ERRORTYPE EventHandler(OMX_IN OMX_HANDLETYPE hComponent,
                                  OMX_IN OMX_PTR pAppData,
                                  OMX_IN OMX_EVENTTYPE eEvent,
                                  OMX_IN OMX_U32 nData1, OMX_IN OMX_U32 nData2,
                                  OMX_IN OMX_PTR pEventData);
static OMX_ERRORTYPE EmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                                     OMX_IN OMX_PTR pAppData,
                                     OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);

static OMX_ERRORTYPE FillBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                                     OMX_IN OMX_PTR pAppData,
                                     OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);

enum
{
  PROP_0,
  ARG_ENCFMT
};

#define DEFAULT_ENCFMT 0

/* pad templates */

static GstStaticPadTemplate gst_qomx_g711enc_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/x-alaw, rate=(int)8000, channels=(int)1;"
    "audio/x-mulaw, rate=(int)8000, channels=(int)1")
    );

static GstStaticPadTemplate gst_qomx_g711enc_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/x-raw, rate=(int)8000, channels=(int)1, format=(string)S16LE, layout=(string)interleaved")
    );


/* class initialization */

G_DEFINE_TYPE_WITH_CODE (GstOmxg711enc, gst_qomx_g711enc, GST_TYPE_AUDIO_ENCODER,
  GST_DEBUG_CATEGORY_INIT (gst_qomx_g711enc_debug_category, "omxg711enc", 0,
  "debug category for omxg711enc element"));

void wait_for_event(void)
{
  pthread_mutex_lock(&lock);
  DEBUG_PRINT("%s: event_is_done=%d", __FUNCTION__, event_is_done);
  while (event_is_done == 0) {
    pthread_cond_wait(&cond, &lock);
  }
  event_is_done = 0;
  pthread_mutex_unlock(&lock);
}

void event_complete(void )
{
  pthread_mutex_lock(&lock);
  if (event_is_done == 0) {
    event_is_done = 1;
    pthread_cond_broadcast(&cond);
  }
  pthread_mutex_unlock(&lock);
}

void etb_wait_for_event(void)
{
  pthread_mutex_lock(&etb_lock1);
  DEBUG_PRINT("%s: etb_event_is_done=%d", __FUNCTION__, etb_event_is_done);
  while (etb_event_is_done == 0) {
    pthread_cond_wait(&etb_cond, &etb_lock1);
  }
  etb_event_is_done = 0;
  pthread_mutex_unlock(&etb_lock1);
}

void etb_event_complete(void )
{
  pthread_mutex_lock(&etb_lock1);
  if (etb_event_is_done == 0) {
    etb_event_is_done = 1;
    pthread_cond_broadcast(&etb_cond);
  }
  pthread_mutex_unlock(&etb_lock1);
}

OMX_ERRORTYPE EventHandler(OMX_IN OMX_HANDLETYPE hComponent,
                           OMX_IN OMX_PTR pAppData,
                           OMX_IN OMX_EVENTTYPE eEvent,
                           OMX_IN OMX_U32 nData1, OMX_IN OMX_U32 nData2,
                           OMX_IN OMX_PTR pEventData)
{
  /* To remove warning for unused variable to keep prototype same */
  (void)hComponent;
  (void)pAppData;
  (void)pEventData;

  switch(eEvent) {
    case OMX_EventCmdComplete:
      DEBUG_PRINT("\n OMX_EventCmdComplete event=%d data1=%u data2=%u\n",
                                     (OMX_EVENTTYPE)eEvent, nData1, nData2);
      event_complete();
      break;
    case OMX_EventError:
      DEBUG_PRINT("\n OMX_EventError \n");
      break;
    case OMX_EventBufferFlag:
      DEBUG_PRINT("\n OMX_EventBufferFlag \n");
      event_complete();
      break;
    case OMX_EventPortSettingsChanged:
      DEBUG_PRINT("\n OMX_EventPortSettingsChanged \n");
      break;
    default:
      DEBUG_PRINT("\n Unknown Event \n");
      break;
  }
  return OMX_ErrorNone;
}

OMX_ERRORTYPE FillBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                              OMX_IN OMX_PTR pAppData,
                              OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
  GstOmxg711enc * omxg711enc = pBuffer->pAppPrivate;
  buf_queue_push(omxg711enc->pOpProtBuffers, pBuffer);

  return OMX_ErrorNone;
}

OMX_ERRORTYPE EmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                              OMX_IN OMX_PTR pAppData,
                              OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
  GstOmxg711enc * omxg711enc = pBuffer->pAppPrivate;
  buf_queue_push(omxg711enc->pIpProtBuffers, pBuffer);
  return OMX_ErrorNone;
}

static OMX_ERRORTYPE Allocate_Buffer ( OMX_COMPONENTTYPE *g711_enc_handle,
                                       OMX_BUFFERHEADERTYPE  ***pBufHdrs,
                                       OMX_U32 nPortIndex,
                                       unsigned int bufCntMin, unsigned int bufSize)
{
  OMX_ERRORTYPE error=OMX_ErrorNone;
  unsigned int bufCnt=0;

  /* To remove warning for unused variable to keep prototype same */

  *pBufHdrs= (OMX_BUFFERHEADERTYPE **)
                 malloc(sizeof(OMX_BUFFERHEADERTYPE*)*bufCntMin);

  for(bufCnt=0; bufCnt < bufCntMin; ++bufCnt) {
    error = OMX_AllocateBuffer(g711_enc_handle, &((*pBufHdrs)[bufCnt]),
                               nPortIndex, NULL, bufSize);
  }

  return error;
}

static void
gst_qomx_g711enc_class_init (GstOmxg711encClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstAudioEncoderClass *audio_encoder_class = GST_AUDIO_ENCODER_CLASS (klass);
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

  /* Setting up pads and setting metadata should be moved to
     base_class_init if you intend to subclass this class. */
  gst_element_class_add_static_pad_template (GST_ELEMENT_CLASS(klass),
      &gst_qomx_g711enc_src_template);
  gst_element_class_add_static_pad_template (GST_ELEMENT_CLASS(klass),
      &gst_qomx_g711enc_sink_template);

  gst_element_class_set_static_metadata (GST_ELEMENT_CLASS(klass),
      "OMX G711 Encoder", "Audio/Encoder",
      "QTI OMX G711 Encoder plugin",
      "Arindam Biswas<c_abiswa@qti.qualcomm.com>");
  element_class->change_state =
      GST_DEBUG_FUNCPTR (gst_omx_audio_g711_enc_change_state);
  gobject_class->set_property = gst_qomx_g711enc_set_property;
  gobject_class->get_property = gst_qomx_g711enc_get_property;

  g_object_class_install_property (gobject_class, ARG_ENCFMT,
        g_param_spec_uint ("encode-format", "AMR encode for mat G711MLAW :0 , G711ALAW: 1",
            "AMR encode for mat G711MLAW :0 , G711ALAW: 1",
            0, 1, DEFAULT_ENCFMT,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | GST_PARAM_MUTABLE_PLAYING));

  audio_encoder_class->set_format = GST_DEBUG_FUNCPTR (gst_qomx_g711enc_set_format);
  audio_encoder_class->handle_frame = GST_DEBUG_FUNCPTR (gst_qomx_g711enc_handle_frame);
  audio_encoder_class->flush = GST_DEBUG_FUNCPTR (gst_qomx_g711enc_flush);
  audio_encoder_class->open = GST_DEBUG_FUNCPTR (gst_qomx_g711enc_open);
  audio_encoder_class->close = GST_DEBUG_FUNCPTR (gst_qomx_g711enc_close);
}

static void
gst_qomx_g711enc_init (GstOmxg711enc *omxg711enc)
{
  pthread_cond_init(&cond, 0);
  pthread_mutex_init(&lock, 0);
  pthread_cond_init(&etb_cond, 0);
  pthread_mutex_init(&etb_lock, 0);
  pthread_mutex_init(&etb_lock1, 0);
  omxg711enc->encode_format = 0;
  omxg711enc->pInputBufHdrs = NULL;
  omxg711enc->pOutputBufHdrs = NULL;
  omxg711enc->audio_component = NULL;
  omxg711enc->g711_enc_handle = 0;
  omxg711enc->samplerate = 8000;
  omxg711enc->channels = 1;
  omxg711enc->pIpProtBuffers = buf_queue_new();
  omxg711enc->pOpProtBuffers = buf_queue_new();
}

void
gst_qomx_g711enc_set_property (GObject * object, guint property_id,
    const GValue * value, GParamSpec * pspec)
{
  GstOmxg711enc *omxg711enc = GST_OMXG711ENC (object);

  GST_DEBUG_OBJECT (omxg711enc, "set_property");

  switch (property_id) {
    case ARG_ENCFMT:
      omxg711enc->encode_format = g_value_get_uint (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_qomx_g711enc_get_property (GObject * object, guint property_id,
    GValue * value, GParamSpec * pspec)
{
  GstOmxg711enc *omxg711enc = GST_OMXG711ENC (object);

  GST_DEBUG_OBJECT (omxg711enc, "get_property");

  switch (property_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static gboolean
gst_qomx_g711enc_set_format (GstAudioEncoder * encoder, GstAudioInfo * info)
{
  GstOmxg711enc *omxg711enc = GST_OMXG711ENC (encoder);

  GST_DEBUG_OBJECT (omxg711enc, "set_format");
  unsigned int i;
  DEBUG_PRINT("Inside %s \n", __FUNCTION__);
  OMX_ERRORTYPE ret;
  int input_buf_cnt, output_buf_cnt;

  omxg711enc->inputportFmt.nPortIndex = omxg711enc->portParam.nStartPortNumber;

  OMX_GetParameter(omxg711enc->g711_enc_handle,OMX_IndexParamPortDefinition,&omxg711enc->inputportFmt);
  GST_DEBUG_OBJECT (omxg711enc, "Enc Input Buffer Count %u", omxg711enc->inputportFmt.nBufferCountMin);
  GST_DEBUG_OBJECT (omxg711enc, "Enc: Input Buffer Size %u", omxg711enc->inputportFmt.nBufferSize);

  if(OMX_DirInput != omxg711enc->inputportFmt.eDir) {
      GST_DEBUG_OBJECT (omxg711enc, "Enc: Expect Input Port");
      return FALSE;
  }

  omxg711enc->pcmParams.nPortIndex   = 0;
  omxg711enc->pcmParams.nChannels    =  info->channels;
  omxg711enc->pcmParams.bInterleaved = OMX_TRUE;
  omxg711enc->pcmParams.nSamplingRate = info->rate;
  OMX_SetParameter(omxg711enc->g711_enc_handle,OMX_IndexParamAudioPcm,&omxg711enc->pcmParams);

  /* Query the encoder outport's min buf requirements */
  //CONFIG_VERSION_SIZE(outputportFmt);
  /* Port for which the Client needs to obtain info */
  omxg711enc->outputportFmt.nPortIndex = omxg711enc->portParam.nStartPortNumber + 1;

  OMX_GetParameter(omxg711enc->g711_enc_handle,OMX_IndexParamPortDefinition,&omxg711enc->outputportFmt);
  GST_DEBUG_OBJECT (omxg711enc, "Enc: Output Buffer Count %u", omxg711enc->outputportFmt.nBufferCountMin);
  GST_DEBUG_OBJECT (omxg711enc, "Enc: Output Buffer Size %u", omxg711enc->outputportFmt.nBufferSize);

  if(OMX_DirOutput != omxg711enc->outputportFmt.eDir) {
    GST_DEBUG_OBJECT (omxg711enc, "Enc: Expect Output Port");
    return FALSE;
  }

  //CONFIG_VERSION_SIZE(pcmParams);

  omxg711enc->pcmParams.nPortIndex   =  1;
  omxg711enc->pcmParams.nChannels    =  info->channels; //Only mono is supported
  omxg711enc->pcmParams.nSamplingRate  =  info->rate;
  OMX_SetParameter(omxg711enc->g711_enc_handle,OMX_IndexParamAudioPcm,&omxg711enc->pcmParams);

  OMX_SendCommand(omxg711enc->g711_enc_handle, OMX_CommandStateSet, OMX_StateIdle,0);

  input_buf_cnt = omxg711enc->inputportFmt.nBufferCountActual; //  inputportFmt.nBufferCountMin + 5;
  GST_DEBUG_OBJECT (omxg711enc, "Transition to Idle State succesful...");
  /* Allocate buffer on decoder's i/p port */
  ret = Allocate_Buffer(omxg711enc->g711_enc_handle, &omxg711enc->pInputBufHdrs, omxg711enc->inputportFmt.nPortIndex,
                         input_buf_cnt, omxg711enc->inputportFmt.nBufferSize);
  if (ret != OMX_ErrorNone || omxg711enc->pInputBufHdrs == NULL) {
    GST_DEBUG_OBJECT (omxg711enc, "OMX_AllocateBuffer Input buffer error");
    return FALSE;
  }
  else {
    GST_DEBUG_OBJECT (omxg711enc, "OMX_AllocateBuffer Input buffer success");
  }

  for(i=0; i < input_buf_cnt; i++) {
    omxg711enc->pInputBufHdrs[i]->pAppPrivate = omxg711enc;
    omxg711enc->pInputBufHdrs[i]->nAllocLen = omxg711enc->inputportFmt.nBufferSize;
    buf_queue_push(omxg711enc->pIpProtBuffers, omxg711enc->pInputBufHdrs[i]);
  }
  output_buf_cnt = omxg711enc->outputportFmt.nBufferCountMin ;

  /* Allocate buffer on encoder's O/Pp port */
  ret = Allocate_Buffer(omxg711enc->g711_enc_handle, &omxg711enc->pOutputBufHdrs,
                        omxg711enc->outputportFmt.nPortIndex, output_buf_cnt,
                        omxg711enc->outputportFmt.nBufferSize);
  if (ret != OMX_ErrorNone || omxg711enc->pOutputBufHdrs == NULL) {
    GST_DEBUG_OBJECT (omxg711enc, "OMX_AllocateBuffer Output buffer error");
    return FALSE;
  }
  else {
    GST_DEBUG_OBJECT (omxg711enc, "OMX_AllocateBuffer Output buffer success");
  }

  GST_DEBUG_OBJECT (omxg711enc, "OMX_SendCommand encoder -> Executing");
  OMX_SendCommand(omxg711enc->g711_enc_handle, OMX_CommandStateSet, OMX_StateExecuting,0);
  wait_for_event();

  DEBUG_PRINT(" Start sending OMX_FILLthisbuffer\n");

  for(i=0; i < output_buf_cnt; i++) {
    GST_DEBUG_OBJECT (omxg711enc, "OMX_FillThisBuffer on output buf no.%d",i);
    omxg711enc->pOutputBufHdrs[i]->nOutputPortIndex = 1;
    omxg711enc->pOutputBufHdrs[i]->pAppPrivate = omxg711enc;
    omxg711enc->pOutputBufHdrs[i]->nFlags = omxg711enc->pOutputBufHdrs[i]->nFlags & (unsigned)~OMX_BUFFERFLAG_EOS;
    ret = OMX_FillThisBuffer(omxg711enc->g711_enc_handle, omxg711enc->pOutputBufHdrs[i]);
    if (OMX_ErrorNone != ret) {
      GST_DEBUG_OBJECT (omxg711enc, "OMX_FillThisBuffer failed with result %d", ret);
    }
    else {
      GST_DEBUG_OBJECT (omxg711enc, "OMX_FillThisBuffer success!");
    }
  }
  gst_pad_start_task (GST_AUDIO_ENCODER_SRC_PAD (omxg711enc),
      (GstTaskFunction) gst_omx_g711_enc_loop, encoder, NULL);
  return TRUE;
}

static OMX_ERRORTYPE attach_g711_header(int encode_format, int num_channels, int sample_rate, guint8 * data)
{

  memset(&g711hdr, 0, sizeof(struct g711_header));

  g711hdr.riff_id = ID_RIFF;
  g711hdr.riff_fmt = ID_WAVE;
  g711hdr.fmt_id = ID_FMT;
  g711hdr.fmt_sz = 18;

  //change format type from wav to g711
  if(encode_format == 0) {
    g711hdr.audio_format = FORMAT_MULAW;
  }
  else {
    g711hdr.audio_format = FORMAT_ALAW;
  }

  g711hdr.num_channels = num_channels;
  g711hdr.sample_rate = sample_rate;
  g711hdr.bits_per_sample = 8;
  g711hdr.byte_rate = g711hdr.sample_rate * g711hdr.num_channels * (g711hdr.bits_per_sample / 8);
  g711hdr.block_align = (guint16)((g711hdr.bits_per_sample / 8) * g711hdr.num_channels);
  g711hdr.extension_size = 0;
  g711hdr.fact_id = ID_FACT;
  g711hdr.fact_sz = 4;
  g711hdr.data_id = ID_DATA;
  g711hdr.data_sz = 0;
  g711hdr.riff_sz = g711hdr.data_sz + sizeof(g711hdr) - 8;

  memcpy(data, &g711hdr, sizeof(g711hdr));

  /*To Do : Attach Fact chunk for Non -PCM format */
  return OMX_ErrorNone;
}
static void
gst_omx_g711_enc_loop (GstOmxg711enc *omxg711enc)
{
  size_t len = 0;
  struct enc_meta_out *meta = NULL;
  static gboolean isFirstFrame = true;
  OMX_BUFFERHEADERTYPE  * pBuffer = buf_queue_pop(omxg711enc->pOpProtBuffers);
  OMX_U8 *src = pBuffer->pBuffer;
  src += sizeof(unsigned char);
  meta = (struct enc_meta_out *)src;
  len = meta->frame_size;
  if(len > 0)
  {
    GstBuffer *outbuf;
    GstMapInfo map = GST_MAP_INFO_INIT;

    if(isFirstFrame)
    {
       outbuf = gst_buffer_new_and_alloc (len + sizeof(g711hdr));
       gst_buffer_map (outbuf, &map, len + sizeof(g711hdr));
       attach_g711_header(omxg711enc->encode_format, omxg711enc->channels, omxg711enc->samplerate, map.data);
       memcpy(map.data + sizeof(g711hdr), pBuffer->pBuffer + sizeof(unsigned char) + meta->offset_to_frame,len);
       isFirstFrame = false;
    }
    else
    {
      outbuf = gst_buffer_new_and_alloc (len);
      gst_buffer_map (outbuf, &map, len);

      memcpy(map.data, pBuffer->pBuffer + sizeof(unsigned char) + meta->offset_to_frame,len);
    }
    gst_buffer_unmap (outbuf, &map);
    GST_BUFFER_TIMESTAMP (outbuf) = pBuffer->nTimeStamp;
    GST_BUFFER_DURATION (outbuf) = pBuffer->nTickCount;
    gst_pad_push(GST_AUDIO_ENCODER(omxg711enc)->srcpad,outbuf);

  }
  OMX_FillThisBuffer(omxg711enc->g711_enc_handle, pBuffer);
}

static GstFlowReturn
gst_qomx_g711enc_handle_frame (GstAudioEncoder * encoder, GstBuffer * buffer)
{
  GstOmxg711enc *omxg711enc = GST_OMXG711ENC (encoder);

  OMX_ERRORTYPE ret;
  gsize size;
  guint offset = 0;
  GstClockTime timestamp, duration;
  OMX_BUFFERHEADERTYPE  * pBufHdr;

  if(buffer == NULL) {
    return GST_FLOW_ERROR;
  }

  timestamp = GST_BUFFER_TIMESTAMP (buffer);
  duration = GST_BUFFER_DURATION (buffer);
  size = gst_buffer_get_size (buffer);

  pBufHdr = buf_queue_pop(omxg711enc->pIpProtBuffers);
  GST_DEBUG_OBJECT (omxg711enc, "pBufHdr->pBuffer= %x size= %d nAllocLen=%d",
                                   pBufHdr->pBuffer, size, pBufHdr->nAllocLen);
  pBufHdr->nFilledLen =
        MIN (size - offset, pBufHdr->nAllocLen - pBufHdr->nOffset);
  gst_buffer_extract (buffer, offset,
        pBufHdr->pBuffer + pBufHdr->nOffset,
        pBufHdr->nFilledLen);
  pBufHdr->nTickCount = duration;
  pBufHdr->nTimeStamp = timestamp;
  GST_DEBUG_OBJECT (omxg711enc, "OMX_EmptyThisBuffer");
  ret = OMX_EmptyThisBuffer(omxg711enc->g711_enc_handle, pBufHdr);
  if (OMX_ErrorNone != ret) {
    GST_DEBUG_OBJECT (omxg711enc, "OMX_EmptyThisBuffer failed with result %d", ret);
  }
  else {
    GST_DEBUG_OBJECT (omxg711enc, "OMX_EmptyThisBuffer success!");
  }

  return GST_FLOW_OK;
}

static void
gst_qomx_g711enc_flush (GstAudioEncoder * encoder)
{
  GstOmxg711enc *omxg711enc = GST_OMXG711ENC (encoder);

  GST_DEBUG_OBJECT (omxg711enc, "flush");

}

static gboolean
gst_qomx_g711enc_open (GstAudioEncoder * encoder)
{
  GstOmxg711enc *omxg711enc = GST_OMXG711ENC (encoder);

  GST_DEBUG_OBJECT (omxg711enc, "open");
  OMX_ERRORTYPE omxresult;
  OMX_U32 total = 0;
  typedef OMX_U8* OMX_U8_PTR;
  char *role ="audio_encoder";

  static OMX_CALLBACKTYPE call_back = {
    &EventHandler,&EmptyBufferDone,&FillBufferDone
  };
  GST_DEBUG_OBJECT (omxg711enc, "Initializing OpenMAX Core....");
  omxresult = OMX_Init();

  if(OMX_ErrorNone != omxresult) {
    GST_DEBUG_OBJECT (omxg711enc, "Failed to Init OpenMAX core");
    return FALSE;
  }
  else {
    GST_DEBUG_OBJECT (omxg711enc, "OpenMAX Core Init Done");
  }

  /* Query for audio decoders*/
  OMX_GetComponentsOfRole(role, &total, 0);
  GST_DEBUG_OBJECT (omxg711enc, "Total components of role=%s :%u", role, total);

  if(omxg711enc->encode_format)
    omxg711enc->audio_component = "OMX.qcom.audio.encoder.g711alaw";
  else
    omxg711enc->audio_component = "OMX.qcom.audio.encoder.g711mlaw";

  omxresult = OMX_GetHandle((OMX_HANDLETYPE*)(&omxg711enc->g711_enc_handle),
                      (OMX_STRING)omxg711enc->audio_component, NULL, &call_back);
  if (FAILED(omxresult)) {
    GST_DEBUG_OBJECT (omxg711enc, "\nFailed to Load the component:%s\n",
                                                omxg711enc->audio_component);
    return FALSE;
  }
  else
  {
    GST_DEBUG_OBJECT (omxg711enc, "Component %s is in LOADED state", omxg711enc->audio_component);
  }

  /* Get the port information */
  omxresult = OMX_GetParameter(omxg711enc->g711_enc_handle, OMX_IndexParamAudioInit,
                              (OMX_PTR)&omxg711enc->portParam);

  if(FAILED(omxresult)) {
    GST_DEBUG_OBJECT (omxg711enc, "Failed to get Port Param");
    return FALSE;
  }
  else
  {
    GST_DEBUG_OBJECT (omxg711enc, "portParam.nPorts:%u",
                                    omxg711enc->portParam.nPorts);
    GST_DEBUG_OBJECT (omxg711enc, "portParam.nStartPortNumber:%u",
                                    omxg711enc->portParam.nStartPortNumber);
  }

  return TRUE;
}

static gboolean
gst_qomx_g711enc_close (GstAudioEncoder * encoder)
{
  GstOmxg711enc *omxg711enc = GST_OMXG711ENC (encoder);

  GST_DEBUG_OBJECT (omxg711enc, "close");
  gst_pad_stop_task (GST_AUDIO_ENCODER_SRC_PAD (encoder));
  buf_queue_flush(omxg711enc->pIpProtBuffers);
  buf_queue_flush(omxg711enc->pOpProtBuffers);
  buf_queue_free(omxg711enc->pIpProtBuffers);
  buf_queue_free(omxg711enc->pOpProtBuffers);
  event_complete();
  OMX_FreeHandle(omxg711enc->g711_enc_handle);
  OMX_Deinit();
  pthread_cond_destroy(&cond);
  pthread_mutex_destroy(&lock);

  return TRUE;
}

static GstStateChangeReturn
gst_omx_audio_g711_enc_change_state (GstElement * element, GstStateChange transition)
{
  GstOmxg711enc *omxg711enc = GST_OMXG711ENC (element);
  GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;

  switch (transition) {
    case GST_STATE_CHANGE_NULL_TO_READY:
      break;
    case GST_STATE_CHANGE_READY_TO_PAUSED:
      break;
    case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
      GST_DEBUG_OBJECT (omxg711enc, "state GST_STATE_CHANGE_PAUSED_TO_PLAYING");
      break;
    case GST_STATE_CHANGE_PAUSED_TO_READY:
      gst_qomx_g711enc_close(GST_AUDIO_ENCODER(omxg711enc));
      break;
    default:
      break;
  }

  ret = GST_ELEMENT_CLASS (gst_qomx_g711enc_parent_class)->change_state (element,
      transition);

  if (ret == GST_STATE_CHANGE_FAILURE)
    return ret;

   switch (transition) {
    case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
      break;
    case GST_STATE_CHANGE_PAUSED_TO_READY:
      break;
    case GST_STATE_CHANGE_READY_TO_NULL:
      break;
    default:
      break;
  }

  return ret;
}
static gboolean
plugin_init (GstPlugin * plugin)
{
  return gst_element_register (plugin, "omxg711enc", GST_RANK_NONE,
      GST_TYPE_OMXG711ENC);
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    omxg711enc,
    "QTI OMX G711 Encoder",
    plugin_init, VERSION, GST_LICENSE_UNKNOWN, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)

