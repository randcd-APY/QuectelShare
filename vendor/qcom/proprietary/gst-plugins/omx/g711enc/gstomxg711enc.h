/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _GST_OMXG711ENC_H_
#define _GST_OMXG711ENC_H_

#include <gst/audio/gstaudioencoder.h>
#include <OMX_Core.h>
#include <OMX_Component.h>
#include "../common/gstbufqueue.h"

G_BEGIN_DECLS

#define GST_TYPE_OMXG711ENC   (gst_qomx_g711enc_get_type())
#define GST_OMXG711ENC(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_OMXG711ENC,GstOmxg711enc))
#define GST_OMXG711ENC_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_OMXG711ENC,GstOmxg711encClass))
#define GST_IS_OMXG711ENC(obj)   (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_OMXG711ENC))
#define GST_IS_OMXG711ENC_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_OMXG711ENC))

typedef struct _GstOmxg711enc GstOmxg711enc;
typedef struct _GstOmxg711encClass GstOmxg711encClass;

struct _GstOmxg711enc
{
  GstAudioEncoder base_omxg711enc;
  OMX_PARAM_PORTDEFINITIONTYPE inputportFmt;
  OMX_PARAM_PORTDEFINITIONTYPE outputportFmt;
  OMX_AUDIO_PARAM_PCMMODETYPE pcmParams;
  OMX_PORT_PARAM_TYPE portParam;
  OMX_BUFFERHEADERTYPE  **pInputBufHdrs;
  OMX_BUFFERHEADERTYPE  **pOutputBufHdrs;
  BufQueue * pIpProtBuffers;
  BufQueue * pOpProtBuffers;
  OMX_COMPONENTTYPE* g711_enc_handle;
  guint32 encode_format;
  OMX_STRING audio_component;
  guint32 samplerate;
  guint32 channels;
};

struct _GstOmxg711encClass
{
  GstAudioEncoderClass base_omxg711enc_class;
};

GType gst_qomx_g711enc_get_type (void);

G_END_DECLS

#endif
