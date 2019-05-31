/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _GST_OMXAMRENC_H_
#define _GST_OMXAMRENC_H_

#include <gst/audio/gstaudioencoder.h>
#include <OMX_Core.h>
#include <OMX_Component.h>
#include "../common/gstbufqueue.h"

G_BEGIN_DECLS

#define GST_TYPE_OMXAMRENC   (gst_qomx_amrenc_get_type())
#define GST_OMXAMRENC(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_OMXAMRENC,GstOmxamrenc))
#define GST_OMXAMRENC_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_OMXAMRENC,GstOmxamrencClass))
#define GST_IS_OMXAMRENC(obj)   (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_OMXAMRENC))
#define GST_IS_OMXAMRENC_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_OMXAMRENC))

typedef struct _GstOmxamrenc GstOmxamrenc;
typedef struct _GstOmxamrencClass GstOmxamrencClass;

struct _GstOmxamrenc
{
  GstAudioEncoder base_omxamrenc;

  OMX_PARAM_PORTDEFINITIONTYPE inputportFmt;
  OMX_PARAM_PORTDEFINITIONTYPE outputportFmt;
  OMX_AUDIO_PARAM_AMRTYPE amrparam;
  OMX_AUDIO_PARAM_PCMMODETYPE    pcmparam;
  OMX_PORT_PARAM_TYPE portParam;
  OMX_PORT_PARAM_TYPE portFmt;
  OMX_ERRORTYPE error;
  guint32 dtxenable;
  guint32 bandmode;
  guint32 amrwb_enable;
  guint32 samplerate;
  guint32 channels;
  OMX_COMPONENTTYPE* amr_enc_handle;
  OMX_BUFFERHEADERTYPE  **pInputBufHdrs;
  OMX_BUFFERHEADERTYPE  **pOutputBufHdrs;
  BufQueue * pIpProtBuffers;
  BufQueue * pOpProtBuffers;
  OMX_STRING aud_comp;
};

struct _GstOmxamrencClass
{
  GstAudioEncoderClass base_omxamrenc_class;
};

//GType gst_omxamrenc_get_type (void);
GType gst_qomx_amrenc_get_type(void);

G_END_DECLS

#endif
