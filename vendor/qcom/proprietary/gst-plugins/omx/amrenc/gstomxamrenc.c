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
#include "gstomxamrenc.h"
#include <pthread.h>
#include "QOMX_AudioExtensions.h"
#include "QOMX_AudioIndexExtensions.h"

GST_DEBUG_CATEGORY_STATIC (gst_qomx_amrenc_debug_category);
#define GST_CAT_DEFAULT gst_qomx_amrenc_debug_category

/* prototypes */

#define CONFIG_VERSION_SIZE(param) \
    param.nVersion.nVersion = CURRENT_OMX_SPEC_VERSION;\
    param.nSize = sizeof(param);


#define AMR_HEADER_SIZE 6
#define AMRWB_HEADER_SIZE 9
#define FAILED(result) (result != OMX_ErrorNone)
#define SUCCEEDED(result) (result == OMX_ErrorNone)

#define false 0
#define true 1

enum
{
  ARG_0,
  ARG_BANDMODE,
  ARG_DTXENABLE,
  ARG_AMRWB_ENABLE,
};
#define DEFAULT_BANDMODE 7
#define DEFAULT_DTXENABLE 0
#define DEFAULT_AMRWB_ENABLE 0

const OMX_U32 CURRENT_OMX_SPEC_VERSION = 0x00000101;
pthread_mutex_t lock;
pthread_cond_t cond;
pthread_mutex_t elock;
pthread_cond_t econd;
pthread_cond_t fcond;
pthread_mutex_t etb_lock;
pthread_mutex_t etb_lock1;
pthread_cond_t etb_cond;

unsigned int input_buf_cnt = 0;
unsigned int output_buf_cnt = 0;
int used_ip_buf_cnt = 0;
volatile int event_is_done = 0;
volatile int ebd_event_is_done = 0;
volatile int fbd_event_is_done = 0;
volatile int etb_event_is_done = 0;
int ebd_cnt;
int bInputEosReached = 0;
int bOutputEosReached = 0;
int bInputEosReached_tunnel = 0;
int bFlushing = false;
int bPause    = false;
const char *in_filename;
const char *out_filename;

int timeStampLfile = 0;
int timestampInterval = 100;

char amr_header[6] = {0x23, 0x21, 0x41, 0x4D, 0x52, 0x0A};
char amrwb_header[9] = {0x23, 0x21, 0x41, 0x4D, 0x52,0x2D, 0x57, 0x42, 0x0A};

struct enc_meta_out{
        unsigned int offset_to_frame;
        unsigned int frame_size;
        unsigned int encoded_pcm_samples;
        unsigned int msw_ts;
        unsigned int lsw_ts;
        unsigned int nflags;
} __attribute__ ((packed));

void wait_for_event(void);
void event_complete(void);
void etb_wait_for_event(void);
void etb_event_complete(void);
OMX_ERRORTYPE EventHandler(OMX_IN OMX_HANDLETYPE hComponent,
                           OMX_IN OMX_PTR pAppData,
                           OMX_IN OMX_EVENTTYPE eEvent,
                           OMX_IN OMX_U32 nData1, OMX_IN OMX_U32 nData2,
                           OMX_IN OMX_PTR pEventData);
OMX_ERRORTYPE FillBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                              OMX_IN OMX_PTR pAppData,
                              OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
OMX_ERRORTYPE EmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                              OMX_IN OMX_PTR pAppData,
                              OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);


static GstStateChangeReturn
gst_omx_audio_enc_change_state (GstElement * element,
    GstStateChange transition);
static void gst_qomx_amrenc_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec);
static void gst_qomx_amrenc_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec);

static gboolean gst_qomx_amrenc_set_format (GstAudioEncoder * encoder,
    GstAudioInfo * info);
static GstFlowReturn gst_qomx_amrenc_handle_frame (GstAudioEncoder * encoder,
    GstBuffer * buffer);
static void gst_qomx_amrenc_flush (GstAudioEncoder * encoder);
static gboolean gst_qomx_amrenc_open (GstAudioEncoder * encoder);
static gboolean gst_qomx_amrenc_close (GstAudioEncoder * encoder);

static void gst_omx_amr_enc_loop (GstOmxamrenc *omxamrenc);

enum
{
  PROP_0
};

/* pad templates */

static GstStaticPadTemplate gst_qomx_amrenc_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/AMR, rate=(int)8000, channels=(int)1; "
      "audio/AMR-WB, rate=(int)16000, channels=(int)1")
    );

static GstStaticPadTemplate gst_qomx_amrenc_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/x-raw, rate=(int)[8000, 16000], channels=(int)1, format=(string)S16LE, layout=(string)interleaved")
    );


/* class initialization */

G_DEFINE_TYPE_WITH_CODE (GstOmxamrenc, gst_qomx_amrenc, GST_TYPE_AUDIO_ENCODER,
  GST_DEBUG_CATEGORY_INIT (gst_qomx_amrenc_debug_category, "omxamrenc", 0,
  "debug category for omxamrenc element"));

void wait_for_event(void)
{
  pthread_mutex_lock(&lock);
  printf("%s: event_is_done=%d", __FUNCTION__, event_is_done);
  while (event_is_done == 0) {
     pthread_cond_wait(&cond, &lock);
  }
  event_is_done = 0;
  pthread_mutex_unlock(&lock);
}

void event_complete(void)
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
    printf("%s: etb_event_is_done=%d", __FUNCTION__, etb_event_is_done);
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
        printf("\n OMX_EventCmdComplete event=%d data1=%u data2=%u\n",
                      (OMX_EVENTTYPE)eEvent, nData1, nData2);
        event_complete();
        break;

      case OMX_EventError:
        printf("\n OMX_EventError \n");
        break;

      case OMX_EventBufferFlag:
        printf("\n OMX_EventBufferFlag \n");
        bOutputEosReached = true;
        event_complete();
        break;

      case OMX_EventPortSettingsChanged:
        printf("\n OMX_EventPortSettingsChanged \n");
        break;

      default:
        printf("\n Unknown Event \n");
        break;
    }
    return OMX_ErrorNone;
}

static OMX_ERRORTYPE Allocate_Buffer ( OMX_COMPONENTTYPE *amr_enc_handle,
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
        error = OMX_AllocateBuffer(amr_enc_handle, &((*pBufHdrs)[bufCnt]),
                                   nPortIndex, NULL, bufSize);
    }

    return error;
}

OMX_ERRORTYPE FillBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                              OMX_IN OMX_PTR pAppData,
                              OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
  GstOmxamrenc * omxamrenc = pBuffer->pAppPrivate;
  buf_queue_push(omxamrenc->pOpProtBuffers, pBuffer);

  return OMX_ErrorNone;
}


OMX_ERRORTYPE EmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                              OMX_IN OMX_PTR pAppData,
                              OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
  GstOmxamrenc *omxamrenc = pBuffer->pAppPrivate;
  buf_queue_push(omxamrenc->pIpProtBuffers, pBuffer);
  return OMX_ErrorNone;
}

static void
gst_qomx_amrenc_class_init (GstOmxamrencClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstAudioEncoderClass *audio_encoder_class = GST_AUDIO_ENCODER_CLASS (klass);
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

  /* Setting up pads and setting metadata should be moved to
     base_class_init if you intend to subclass this class. */
  gst_element_class_add_static_pad_template (GST_ELEMENT_CLASS(klass),
      &gst_qomx_amrenc_src_template);
  gst_element_class_add_static_pad_template (GST_ELEMENT_CLASS(klass),
      &gst_qomx_amrenc_sink_template);

  gst_element_class_set_static_metadata (GST_ELEMENT_CLASS(klass),
      "OMX AMR Encoder", "Audio/Encoder",
      "QTI OMX AMR Encoder plugin",
      "Arindam Biswas<c_abiswa@qti.qualcomm.com>");
  element_class->change_state =
      GST_DEBUG_FUNCPTR (gst_omx_audio_enc_change_state);
  gobject_class->set_property = gst_qomx_amrenc_set_property;
  gobject_class->get_property = gst_qomx_amrenc_get_property;

  g_object_class_install_property (gobject_class, ARG_BANDMODE,
        g_param_spec_uint ("bandmode", "AMR-Bandmode",
            "Encoding bandmode",
            0, 8, DEFAULT_BANDMODE,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | GST_PARAM_MUTABLE_PLAYING));

  g_object_class_install_property (gobject_class, ARG_DTXENABLE,
        g_param_spec_uint ("dtx-enable", "AMR DTX enable",
            "Encoding DTX enable",
            0, 1, DEFAULT_DTXENABLE,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | GST_PARAM_MUTABLE_PLAYING));

  g_object_class_install_property (gobject_class, ARG_AMRWB_ENABLE,
        g_param_spec_uint ("amrwb-enable", "AMR WB enable",
            "Encoding AMR WB enable",
            0, 1, DEFAULT_AMRWB_ENABLE,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | GST_PARAM_MUTABLE_PLAYING));

  audio_encoder_class->set_format = GST_DEBUG_FUNCPTR (gst_qomx_amrenc_set_format);
  audio_encoder_class->handle_frame = GST_DEBUG_FUNCPTR (gst_qomx_amrenc_handle_frame);
  audio_encoder_class->flush = GST_DEBUG_FUNCPTR (gst_qomx_amrenc_flush);
  audio_encoder_class->open = GST_DEBUG_FUNCPTR (gst_qomx_amrenc_open);
  audio_encoder_class->close = GST_DEBUG_FUNCPTR (gst_qomx_amrenc_close);

}

static void
gst_qomx_amrenc_init (GstOmxamrenc *omxamrenc)
{
  omxamrenc->dtxenable = 0;
  omxamrenc->bandmode = 7;
  omxamrenc->amrwb_enable = 0;
  omxamrenc->samplerate = 8000;
  omxamrenc->channels = 1;
  omxamrenc->amr_enc_handle = 0;
  omxamrenc->pInputBufHdrs = NULL;
  omxamrenc->pOutputBufHdrs = NULL;
  omxamrenc->aud_comp = NULL;
  omxamrenc->amr_enc_handle = 0;
  pthread_cond_init(&cond, 0);
  pthread_mutex_init(&lock, 0);
  pthread_cond_init(&etb_cond, 0);
  pthread_mutex_init(&etb_lock, 0);
  pthread_mutex_init(&etb_lock1, 0);
  omxamrenc->pIpProtBuffers = buf_queue_new();
  omxamrenc->pOpProtBuffers = buf_queue_new();
}

void
gst_qomx_amrenc_set_property (GObject * object, guint property_id,
    const GValue * value, GParamSpec * pspec)
{
  GstOmxamrenc *omxamrenc = GST_OMXAMRENC (object);

  GST_DEBUG_OBJECT (omxamrenc, "set_property");

  switch (property_id) {
    case ARG_BANDMODE:
      omxamrenc->bandmode = g_value_get_uint (value);
      GST_DEBUG_OBJECT (omxamrenc, "omxamrenc->bandmode %d",omxamrenc->bandmode);
      break;
    case ARG_DTXENABLE:
      omxamrenc->dtxenable = g_value_get_uint (value);
      GST_DEBUG_OBJECT (omxamrenc, "omxamrenc->bandmode %d",omxamrenc->dtxenable);
      break;
    case ARG_AMRWB_ENABLE:
      omxamrenc->amrwb_enable = g_value_get_uint (value);
      GST_DEBUG_OBJECT (omxamrenc, "omxamrenc->bandmode %d",omxamrenc->amrwb_enable);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_qomx_amrenc_get_property (GObject * object, guint property_id,
    GValue * value, GParamSpec * pspec)
{
  GstOmxamrenc *omxamrenc = GST_OMXAMRENC (object);

  GST_DEBUG_OBJECT (omxamrenc, "get_property");

  switch (property_id) {
    case ARG_BANDMODE:
      g_value_set_uint (value,omxamrenc->bandmode);
      GST_DEBUG_OBJECT (omxamrenc, "omxamrenc->bandmode %d",omxamrenc->bandmode);
      break;
    case ARG_DTXENABLE:
      g_value_set_uint (value,omxamrenc->dtxenable);
      GST_DEBUG_OBJECT (omxamrenc, "omxamrenc->bandmode %d",omxamrenc->dtxenable);
      break;
    case ARG_AMRWB_ENABLE:
      g_value_set_uint (value,omxamrenc->amrwb_enable);
      GST_DEBUG_OBJECT (omxamrenc, "omxamrenc->bandmode %d",omxamrenc->amrwb_enable);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }

}

static gboolean
gst_qomx_amrenc_set_format (GstAudioEncoder * encoder, GstAudioInfo * info)
{
  GstOmxamrenc *omxamrenc = GST_OMXAMRENC (encoder);

  GST_DEBUG_OBJECT (omxamrenc, "set_format");

  unsigned int i;
  OMX_ERRORTYPE ret;
  OMX_BUFFERHEADERTYPE  * pBufHdr;
  /* Query the encoder input min buf requirements */
  CONFIG_VERSION_SIZE(omxamrenc->inputportFmt);

  /* Port for which the Client needs to obtain info */
  omxamrenc->inputportFmt.nPortIndex = omxamrenc->portParam.nStartPortNumber;

  OMX_GetParameter(omxamrenc->amr_enc_handle,OMX_IndexParamPortDefinition,&omxamrenc->inputportFmt);
  GST_DEBUG_OBJECT (omxamrenc, "Enc Input Buffer Count %u", omxamrenc->inputportFmt.nBufferCountMin);
  GST_DEBUG_OBJECT (omxamrenc, "Enc: Input Buffer Size %u", omxamrenc->inputportFmt.nBufferSize);

  if(OMX_DirInput != omxamrenc->inputportFmt.eDir) {
    GST_DEBUG_OBJECT (omxamrenc, "Enc: Expect Input Port");
    return FALSE;
  }

  omxamrenc->pcmparam.nPortIndex   = 0;
  omxamrenc->pcmparam.nChannels    =  info->channels;
  omxamrenc->pcmparam.nSamplingRate = info->rate;
  OMX_SetParameter(omxamrenc->amr_enc_handle,OMX_IndexParamAudioPcm,&omxamrenc->pcmparam);

  /* Query the encoder outport's min buf requirements */
  CONFIG_VERSION_SIZE(omxamrenc->outputportFmt);
  /* Port for which the Client needs to obtain info */
  omxamrenc->outputportFmt.nPortIndex = omxamrenc->portParam.nStartPortNumber + 1;

  OMX_GetParameter(omxamrenc->amr_enc_handle,OMX_IndexParamPortDefinition,&omxamrenc->outputportFmt);
  GST_DEBUG_OBJECT (omxamrenc, "Enc: Output Buffer Count %u", omxamrenc->outputportFmt.nBufferCountMin);
  GST_DEBUG_OBJECT (omxamrenc, "Enc: Output Buffer Size %u", omxamrenc->outputportFmt.nBufferSize);

  if(OMX_DirOutput != omxamrenc->outputportFmt.eDir) {
    GST_DEBUG_OBJECT (omxamrenc, "Enc: Expect Output Port");
    return FALSE;
  }

  CONFIG_VERSION_SIZE(omxamrenc->amrparam);

  omxamrenc->amrparam.nPortIndex   =  1;
  omxamrenc->amrparam.nChannels    =  info->channels; /* 1-> mono 2-> stereo*/
  omxamrenc->amrparam.eAMRBandMode = omxamrenc->bandmode;
  omxamrenc->amrparam.eAMRDTXMode = omxamrenc->dtxenable;
  OMX_SetParameter(omxamrenc->amr_enc_handle,OMX_IndexParamAudioAmr,&omxamrenc->amrparam);
  OMX_SendCommand(omxamrenc->amr_enc_handle, OMX_CommandStateSet, OMX_StateIdle,0);
  /* wait_for_event(); should not wait here event complete status will
     not come until enough buffer are allocated */
  output_buf_cnt = omxamrenc->outputportFmt.nBufferCountMin ;

  input_buf_cnt = omxamrenc->inputportFmt.nBufferCountActual; //  inputportFmt.nBufferCountMin + 5;
  GST_DEBUG_OBJECT (omxamrenc, "Transition to Idle State succesful...");
  /* Allocate buffer on decoder's i/p port */
  ret = Allocate_Buffer(omxamrenc->amr_enc_handle, &omxamrenc->pInputBufHdrs,
                        omxamrenc->inputportFmt.nPortIndex, input_buf_cnt,
                        omxamrenc->inputportFmt.nBufferSize);
  if (ret != OMX_ErrorNone || omxamrenc->pInputBufHdrs == NULL ) {
    GST_DEBUG_OBJECT (omxamrenc, "OMX_AllocateBuffer Input buffer error");
    return FALSE;
  }

  GST_DEBUG_OBJECT (omxamrenc, "omxamrenc->inputportFmt.nBufferSize =%d", omxamrenc->inputportFmt.nBufferSize);
  for(i=0; i < input_buf_cnt; i++) {
    omxamrenc->pInputBufHdrs[i]->pAppPrivate = omxamrenc;
    omxamrenc->pInputBufHdrs[i]->nAllocLen = omxamrenc->inputportFmt.nBufferSize;
    buf_queue_push(omxamrenc->pIpProtBuffers, omxamrenc->pInputBufHdrs[i]);
  }
  /* Allocate buffer on encoder's O/Pp port */
  ret = Allocate_Buffer(omxamrenc->amr_enc_handle, &omxamrenc->pOutputBufHdrs,
                        omxamrenc->outputportFmt.nPortIndex, output_buf_cnt,
                        omxamrenc->outputportFmt.nBufferSize);
  if (ret != OMX_ErrorNone || omxamrenc->pOutputBufHdrs == NULL ) {
    GST_DEBUG_OBJECT (omxamrenc, "OMX_AllocateBuffer Output buffer error");
    return FALSE;
  }
  else {
    GST_DEBUG_OBJECT (omxamrenc, "OMX_AllocateBuffer Output buffer success");
  }

  wait_for_event();

  OMX_SendCommand(omxamrenc->amr_enc_handle, OMX_CommandStateSet, OMX_StateExecuting,0);
  wait_for_event();

  GST_DEBUG_OBJECT (omxamrenc, "Start sending OMX_FILLthisbuffer");
  for(i=0; i < output_buf_cnt; i++) {
    omxamrenc->pOutputBufHdrs[i]->nOutputPortIndex = 1;
    omxamrenc->pOutputBufHdrs[i]->nFlags = omxamrenc->pOutputBufHdrs[i]->nFlags & (unsigned)~OMX_BUFFERFLAG_EOS;
    omxamrenc->pOutputBufHdrs[i]->pAppPrivate = omxamrenc;
    buf_queue_push(omxamrenc->pOpProtBuffers, omxamrenc->pOutputBufHdrs[i]);
  }
  for(i=0; i < output_buf_cnt; i++) {
    GST_DEBUG_OBJECT (omxamrenc,  "OMX_FillThisBuffer on output buf no.%d",i);
    pBufHdr = buf_queue_pop(omxamrenc->pOpProtBuffers);
    ret = OMX_FillThisBuffer(omxamrenc->amr_enc_handle, pBufHdr);
    if (OMX_ErrorNone != ret) {
      GST_DEBUG_OBJECT (omxamrenc, "OMX_FillThisBuffer failed with result %d", ret);
    }
    else {
      GST_DEBUG_OBJECT (omxamrenc, "OMX_FillThisBuffer success!");
    }
  }

  gst_pad_start_task (GST_AUDIO_ENCODER_SRC_PAD (omxamrenc),
  (GstTaskFunction) gst_omx_amr_enc_loop, encoder, NULL);
  return TRUE;
}

static void
gst_omx_amr_enc_loop (GstOmxamrenc *omxamrenc)
{
  size_t len = 0;
  struct enc_meta_out *meta = NULL;
  static gboolean isFirstFrame = true;
  OMX_BUFFERHEADERTYPE  * pBuffer = buf_queue_pop(omxamrenc->pOpProtBuffers);
  OMX_U8 *src = pBuffer->pBuffer;

  if(1 != src[0]){
    GST_DEBUG_OBJECT (omxamrenc, "Data corrupt");
    return;
  }

  /* Skip the first bytes */
  src += sizeof(unsigned char);
  meta = (struct enc_meta_out *)src;

  /*
  printf("offset=%d framesize=%d encoded_pcm[%d] msw_ts[%d]lsw_ts[%d] nflags[%d]\n",
                                                                       meta->offset_to_frame,
                                                                       meta->frame_size,
                          meta->encoded_pcm_samples, meta->msw_ts, meta->lsw_ts, meta->nflags);
   */

  len = meta->frame_size;
  if(len > 0)
  {
    GstBuffer *outbuf;
    GstMapInfo map = GST_MAP_INFO_INIT;

    if(isFirstFrame)
    {
      if(omxamrenc->amrwb_enable == 0)
      {
        outbuf = gst_buffer_new_and_alloc (len + AMR_HEADER_SIZE);
        gst_buffer_map (outbuf, &map, len + AMR_HEADER_SIZE);
      }
      else
      {
        outbuf = gst_buffer_new_and_alloc (len + AMRWB_HEADER_SIZE);
        gst_buffer_map (outbuf, &map, len + AMRWB_HEADER_SIZE);
      }
    }
    else
    {
      outbuf = gst_buffer_new_and_alloc (len);
      gst_buffer_map (outbuf, &map, len);
    }

    if(isFirstFrame)
    {
      if(omxamrenc->amrwb_enable == 0)
      {
        memcpy(map.data, amr_header, AMR_HEADER_SIZE);
        memcpy (map.data + AMR_HEADER_SIZE,
          pBuffer->pBuffer + sizeof(unsigned char) + meta->offset_to_frame,
          len);
      }
      else
      {

        memcpy(map.data, amrwb_header,AMRWB_HEADER_SIZE);
        memcpy (map.data + AMRWB_HEADER_SIZE,
          pBuffer->pBuffer + sizeof(unsigned char) + meta->offset_to_frame,
          len);
      }
      isFirstFrame = false;
    }
    else
       memcpy (map.data,
          pBuffer->pBuffer + sizeof(unsigned char) + meta->offset_to_frame,
          len);
    gst_buffer_unmap (outbuf, &map);
    GST_BUFFER_TIMESTAMP (outbuf) = pBuffer->nTimeStamp;
    GST_BUFFER_DURATION (outbuf) = pBuffer->nTickCount;
    gst_pad_push(GST_AUDIO_ENCODER(omxamrenc)->srcpad,outbuf);
  }

  OMX_FillThisBuffer(omxamrenc->amr_enc_handle, pBuffer);
}

static GstFlowReturn
gst_qomx_amrenc_handle_frame (GstAudioEncoder * encoder, GstBuffer * buffer)
{
  GstOmxamrenc *omxamrenc = GST_OMXAMRENC (encoder);
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

  pBufHdr = buf_queue_pop(omxamrenc->pIpProtBuffers);
  GST_DEBUG_OBJECT (omxamrenc, "pBufHdr->pBuffer= %x size= %d nAllocLen=%d",
                                 pBufHdr->pBuffer, size, pBufHdr->nAllocLen);
  pBufHdr->nFilledLen =
        MIN (size - offset, pBufHdr->nAllocLen - pBufHdr->nOffset);
  gst_buffer_extract (buffer, offset,
        pBufHdr->pBuffer + pBufHdr->nOffset,
        pBufHdr->nFilledLen);
  pBufHdr->nTickCount = duration;
  pBufHdr->nTimeStamp = timestamp;
  GST_DEBUG_OBJECT (omxamrenc, "OMX_EmptyThisBuffer");
  ret = OMX_EmptyThisBuffer(omxamrenc->amr_enc_handle, pBufHdr);
  if (OMX_ErrorNone != ret) {
    GST_DEBUG_OBJECT (omxamrenc, "OMX_EmptyThisBuffer failed with result %d", ret);
  }
  else {
    GST_DEBUG_OBJECT (omxamrenc, "OMX_EmptyThisBuffer success!");
  }

  return GST_FLOW_OK;
}

static void
gst_qomx_amrenc_flush (GstAudioEncoder * encoder)
{
  GstOmxamrenc *omxamrenc = GST_OMXAMRENC (encoder);

  GST_DEBUG_OBJECT (omxamrenc, "flush");

}

static gboolean
gst_qomx_amrenc_open (GstAudioEncoder * encoder)
{
  GstOmxamrenc *omxamrenc = GST_OMXAMRENC (encoder);
  GST_DEBUG_OBJECT (omxamrenc, "open");

  OMX_ERRORTYPE omxresult;
  OMX_U32 total = 0;
  typedef OMX_U8* OMX_U8_PTR;
  char *role = "audio_encoder";

  static OMX_CALLBACKTYPE call_back = {
      &EventHandler,&EmptyBufferDone,&FillBufferDone
  };

  /* Init. the OpenMAX Core */
  GST_DEBUG_OBJECT (omxamrenc, "Initializing OpenMAX Core....");
  omxresult = OMX_Init();
  if(OMX_ErrorNone != omxresult) {
    GST_DEBUG_OBJECT (omxamrenc, "Failed to Init OpenMAX core");
    return FALSE;
  }
  else {
    GST_DEBUG_OBJECT (omxamrenc, "OpenMAX Core Init Done");
  }

  GST_DEBUG_OBJECT (omxamrenc, "omxamrenc->amrwb_enable = %d",omxamrenc->amrwb_enable);
  if(omxamrenc->amrwb_enable)
    omxamrenc->aud_comp = "OMX.qcom.audio.encoder.amrwb";
  else
    omxamrenc->aud_comp = "OMX.qcom.audio.encoder.amrnb";

  /* Query for audio decoders*/
  OMX_GetComponentsOfRole(role, &total, 0);
  GST_DEBUG_OBJECT (omxamrenc, "Total components of role=%s :%u", role, total);
  omxresult = OMX_GetHandle((OMX_HANDLETYPE*)(&omxamrenc->amr_enc_handle),
      (OMX_STRING)omxamrenc->aud_comp, NULL, &call_back);
  if (FAILED(omxresult)) {
    GST_DEBUG_OBJECT (omxamrenc, "Failed to Load the component:%s", omxamrenc->aud_comp);
    return FALSE;
  }
  else
  {
    GST_DEBUG_OBJECT (omxamrenc, "Component %s is in LOADED state", omxamrenc->aud_comp);
  }

  /* Get the port information */
  CONFIG_VERSION_SIZE(omxamrenc->portParam);
  omxresult = OMX_GetParameter(omxamrenc->amr_enc_handle, OMX_IndexParamAudioInit,
                                (OMX_PTR)&omxamrenc->portParam);

  if(FAILED(omxresult)) {
    GST_DEBUG_OBJECT (omxamrenc, "Failed to get Port Param");
    return FALSE;
  }
  else
  {
     GST_DEBUG_OBJECT (omxamrenc, "portParam.nPorts:%u",
                                 omxamrenc->portParam.nPorts);
     GST_DEBUG_OBJECT (omxamrenc, "portParam.nStartPortNumber:%u",
                             omxamrenc->portParam.nStartPortNumber);
  }

  if(OMX_ErrorNone != omxresult)
  {
    GST_DEBUG_OBJECT (omxamrenc, "Set parameter failed");
  }

  return TRUE;
}

static gboolean
gst_qomx_amrenc_close (GstAudioEncoder * encoder)
{
  GstOmxamrenc *omxamrenc = GST_OMXAMRENC (encoder);
  GST_DEBUG_OBJECT (omxamrenc, "close");
  gst_pad_stop_task (GST_AUDIO_ENCODER_SRC_PAD (encoder));
  buf_queue_flush(omxamrenc->pIpProtBuffers);
  buf_queue_flush(omxamrenc->pOpProtBuffers);
  buf_queue_free(omxamrenc->pIpProtBuffers);
  buf_queue_free(omxamrenc->pOpProtBuffers);
  event_complete();
  OMX_FreeHandle(omxamrenc->amr_enc_handle);
  OMX_Deinit();
  pthread_cond_destroy(&cond);
  pthread_mutex_destroy(&lock);

  return TRUE;
}

static GstStateChangeReturn
gst_omx_audio_enc_change_state (GstElement * element, GstStateChange transition)
{
  GstOmxamrenc *omxamrenc = GST_OMXAMRENC (element);
  GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;

  switch (transition) {
    case GST_STATE_CHANGE_NULL_TO_READY:
      break;
    case GST_STATE_CHANGE_READY_TO_PAUSED:
      break;
    case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
      GST_DEBUG_OBJECT (omxamrenc, "state GST_STATE_CHANGE_PAUSED_TO_PLAYING");
      break;
    case GST_STATE_CHANGE_PAUSED_TO_READY:
      GST_DEBUG_OBJECT (omxamrenc, "state GST_STATE_CHANGE_PAUSED_TO_READY");
      gst_qomx_amrenc_close(GST_AUDIO_ENCODER(omxamrenc));
      break;
    default:
      break;
  }

  ret = GST_ELEMENT_CLASS (gst_qomx_amrenc_parent_class)->change_state (element,
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

  return gst_element_register (plugin, "omxamrenc", GST_RANK_NONE,
      GST_TYPE_OMXAMRENC);
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    omxamrenc,
    "QTI OMX AMR Encoder",
    plugin_init, VERSION, GST_LICENSE_UNKNOWN, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)

