/*
*Copyright (c) 2017 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * SECTION:element-qahweffect
 * @see_also: qahweffect
 *
 * This base class filter enables DSP driver effect.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include "gstqahweffect.h"
#include "gstqahw-common.h"
#include <stdio.h>

GST_DEBUG_CATEGORY_EXTERN (qahw_debug);
#define GST_CAT_DEFAULT qahw_debug

enum
{
  PROP_0,
  PROP_ENABLE,
  PROP_LAST
};

#define gst_qahw_effect_parent_class parent_class
G_DEFINE_ABSTRACT_TYPE (GstQahwEffect, gst_qahw_effect,
    GST_TYPE_BASE_TRANSFORM);

static GstStaticPadTemplate qahw_equalizer_sink_factory =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (QAHW_SINK_CAPS)
    );

static GstStaticPadTemplate qahw_equalizer_src_factory =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (QAHW_SINK_CAPS)
    );

/* Called with object lock */
static gboolean
gst_qahw_effect_enable (GstQahwEffect * self)
{
  GstQahwEffectClass *klass = GST_QAHW_EFFECT_GET_CLASS (self);
  guint32 rc;
  gint reply;
  guint32 reply_size = sizeof (gint);

  if (!self->effect_handle)
    return FALSE;

  GST_DEBUG_OBJECT (self, "Calling effect enable\n");
  printf("Calling effect enable\n");
  rc = qahw_effect_command (self->effect_handle, QAHW_EFFECT_CMD_ENABLE, 0,
      NULL, &reply_size, &reply);
  if (rc != 0) {
    GST_OBJECT_UNLOCK (self);
    /* FIXME ENABLE always fails, so just ignore for now */
    GST_ELEMENT_WARNING (self, LIBRARY, SETTINGS,
        ("Failed to enable equalizer effect."),
        ("Returned \"%s\" (%i)", g_strerror (-rc), rc));
    printf("Failed to enable equalizer effect.Returned %s (%i)\n", g_strerror (-rc), rc);
    GST_OBJECT_LOCK (self);
    return FALSE;
  }

  if (klass->enable) {
    GST_DEBUG_OBJECT (self, "Calling sphere effect enable");
    printf("Calling sphere effect enable\n");
    if (!klass->enable (self))
      return FALSE;
    GST_DEBUG_OBJECT (self, "Returned from sphere effect enable");
  }

  return TRUE;
}

/* Called with object lock */
static gboolean
gst_qahw_effect_disable (GstQahwEffect * self)
{
  GstQahwEffectClass *klass = GST_QAHW_EFFECT_GET_CLASS (self);
  guint32 rc;
  gint reply;
  guint32 reply_size = sizeof (gint);

  if (!self->effect_handle)
    return FALSE;

  GST_DEBUG_OBJECT (self, "Calling effect disable");
  printf("Calling effect disable\n");
  rc = qahw_effect_command (self->effect_handle, QAHW_EFFECT_CMD_DISABLE, 0,
      NULL, &reply_size, &reply);
  if (rc != 0) {
    GST_OBJECT_UNLOCK (self);
    GST_ELEMENT_WARNING (self, LIBRARY, SETTINGS,
        ("Failed to disable equalizer effect."),
        ("Returned \"%s\" (%i)", g_strerror (-rc), rc));
    printf("Failed to disable equalizer effect.Returned %s (%i)\n", g_strerror (-rc), rc);
    GST_OBJECT_LOCK (self);
    return FALSE;
  }

  if (klass->disable) {
    GST_DEBUG_OBJECT (self, "Calling sphere effect disable");
    printf("Calling sphere effect disable\n");
    if (!klass->disable (self))
      return FALSE;
    GST_DEBUG_OBJECT (self, "Returned from sphere effect disable");
  }

  return TRUE;
}

static void
gst_qahw_effect_stream_created (GstQahwEffect * self, const GstStructure * s)
{
  GstQahwEffectClass *klass = GST_QAHW_EFFECT_GET_CLASS (self);
  gint audio_handle;
  audio_devices_t output_device;
  qahw_effect_offload_param_t offload_param;
  guint32 rc, reply, reply_size = sizeof (guint32);

  if (!gst_structure_get_int (s, "io-handle", &audio_handle)) {
    GST_WARNING_OBJECT (self,
        "Missing \"io-handle\" in QahwStreamCreated event.");
    printf("Missing io-handle in QahwStreamCreated event.\n");
    return;
  }

  if (!gst_structure_get_uint (s, "output-device", &output_device)) {
    GST_WARNING_OBJECT (self,
        "Missing \"io-handle\" in QahwStreamCreated event.");
    return;
  }

  GST_DEBUG_OBJECT (self,
      "Creating equalizer effect with io-handle=%i output-device=%i",
      audio_handle, output_device);
  printf("Creating equalizer effect with io-handle=%i output-device=%i \n",
      audio_handle, output_device);

  GST_OBJECT_LOCK (self);

  if (self->effect_handle) {
    GST_WARNING_OBJECT (self, "Effect is already created.");
    printf("Effect is already created.\n");
    GST_OBJECT_UNLOCK (self);
    return;
  }

  rc = qahw_effect_create (self->lib_handle, klass->uuid,
      audio_handle, &self->effect_handle);
  if (rc != 0) {
    GST_OBJECT_UNLOCK (self);
    GST_ELEMENT_ERROR (self, LIBRARY, INIT,
        ("Failed create equalizer effect."),
        ("Returned \"%s\" (%i)", g_strerror (-rc), rc));
    printf("Failed create equalizer effect.Returned %s (%i)\n", g_strerror (-rc), rc);
    return;
  }

  rc = qahw_effect_command (self->effect_handle, QAHW_EFFECT_CMD_SET_DEVICE,
      sizeof (audio_devices_t), &output_device, &reply_size, &reply);
  if (rc != 0) {
    GST_OBJECT_UNLOCK (self);
    GST_ELEMENT_ERROR (self, LIBRARY, SETTINGS,
        ("Failed to set effect output device."),
        ("Returned \"%s\" (%i)", g_strerror (-rc), rc));
    printf ("Failed to set effect output device.Returned %s (%i)\n", g_strerror (-rc), rc);
    return;
  }

  offload_param.isOffload = 1;
  offload_param.ioHandle = audio_handle;
  rc = qahw_effect_command (self->effect_handle, QAHW_EFFECT_CMD_OFFLOAD,
      sizeof (qahw_effect_offload_param_t), &offload_param, &reply_size,
      &reply);
  if (rc == 0)
    rc = reply;
  if (rc != 0) {
    GST_OBJECT_UNLOCK (self);
    GST_ELEMENT_ERROR (self, LIBRARY, SETTINGS,
        ("Failed to run offload command."),
        ("Returned \"%s\" (%i)", g_strerror (-rc), rc));
    printf("Failed to run offload command.Returned %s (%i)\n", g_strerror (-rc), rc);
    return;
  }

  if (klass->setup) {
    if (!klass->setup (self))
      return;
  }

  if (self->enabled) {
    if (!gst_qahw_effect_enable (self)) {
      GST_OBJECT_UNLOCK (self);
      return;
    }
  }

  GST_OBJECT_UNLOCK (self);
}

static void
gst_qahw_effect_stream_disposed (GstQahwEffect * self)
{
  guint32 rc;

  GST_DEBUG_OBJECT (self, "Received QahwStreamDisposed event.");
  printf("Received QahwStreamDisposed event.\n");
  GST_OBJECT_LOCK (self);

  if (self->effect_handle) {
    if (self->enabled)
      gst_qahw_effect_disable (self);

    memset (&self->effect_desc, 0, sizeof (qahw_effect_descriptor_t));

    rc = qahw_effect_release (self->lib_handle, self->effect_handle);
    self->effect_handle = NULL;
    if (rc != 0) {
      GST_OBJECT_UNLOCK (self);
      GST_ELEMENT_WARNING (self, LIBRARY, SETTINGS,
          ("Failed to release equalizer effect."),
          ("Returned \"%s\" (%i)", g_strerror (-rc), rc));
      printf("Failed to release equalizer effect.Returned %s (%i)\n", g_strerror (-rc), rc);
      GST_OBJECT_LOCK (self);
    }
  }

  GST_OBJECT_UNLOCK (self);
}

static gboolean
gst_qahw_effect_start (GstBaseTransform * btrans)
{
  GstQahwEffect *self = GST_QAHW_EFFECT (btrans);
  GstQahwEffectClass *klass = GST_QAHW_EFFECT_GET_CLASS (self);
  guint32 rc;

  if (!klass->library) {
    GST_ELEMENT_ERROR (self, CORE, NOT_IMPLEMENTED,
        ("Effect subclass didn't set \"library\" class parameter."), (NULL));
    printf("Effect subclass didn't set library class parameter.\n");
    return FALSE;
  }

  if (!klass->uuid) {
    GST_ELEMENT_ERROR (self, CORE, NOT_IMPLEMENTED,
        ("Effect subclass didn't set \"uuid\" class parameter."), (NULL));
    printf("Effect subclass didn't set uuid class parameter.\n");
    return FALSE;
  }

  self->lib_handle = qahw_effect_load_library (klass->library);
  if (!self->lib_handle) {
    GST_ELEMENT_ERROR (self, LIBRARY, INIT,
        ("Failed to load %s effect library.", klass->library), (NULL));
    printf("Failed to load %s effect library.\n", klass->library);
    return FALSE;
  }

  rc = qahw_effect_get_descriptor (self->lib_handle, klass->uuid,
      &self->effect_desc);
  if (rc != 0) {
    GST_ELEMENT_ERROR (self, LIBRARY, INIT,
        ("Failed to get effect descriptor."),
        ("Returned \"%s\" (%i)", g_strerror (-rc), rc));
    printf("Failed to get effect descriptor.Returned %s (%i)\n", g_strerror (-rc), rc);
    return FALSE;
  }

  GST_DEBUG_OBJECT (self, "Loaded %s Effect library.", self->effect_desc.name);
  printf("Loaded %s Effect library. \n", self->effect_desc.name);
  return TRUE;
}

static gboolean
gst_qahw_effect_stop (GstBaseTransform * btrans)
{
  GstQahwEffect *self = GST_QAHW_EFFECT (btrans);

  GST_DEBUG_OBJECT (self, "Stopping %s effect ...", self->effect_desc.name);
  printf("Stopping %s effect ...\n", self->effect_desc.name);
  gst_qahw_effect_stream_disposed (self);

  if (self->lib_handle) {
    guint32 rc;

    rc = qahw_effect_unload_library (self->lib_handle);
    self->lib_handle = NULL;

    if (rc != 0) {
      GST_ELEMENT_WARNING (self, LIBRARY, SHUTDOWN,
          ("Failed to unload effect library."),
          ("Returned \"%s\" (%i)", g_strerror (-rc), rc));
      printf("Failed to unload effect library. Returned %s (%i)\n", g_strerror (-rc), rc);
    }
  }

  return TRUE;
}

static gboolean
gst_qahw_effect_src_event (GstBaseTransform * btrans, GstEvent * event)
{
  GstQahwEffect *self = GST_QAHW_EFFECT (btrans);

  if (GST_EVENT_TYPE (event) == GST_EVENT_CUSTOM_UPSTREAM) {
    const GstStructure *s = gst_event_get_structure (event);

    if (gst_structure_has_name (s, "QahwStreamCreated"))
      gst_qahw_effect_stream_created (self, s);

    if (gst_structure_has_name (s, "QahwStreamDisposed"))
      gst_qahw_effect_stream_disposed (self);
  }

  /* always forward, in case there is other effects upstream */
  return GST_BASE_TRANSFORM_CLASS (parent_class)->src_event (btrans, event);
}

static void
gst_qahw_effect_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstQahwEffect *self;

  self = GST_QAHW_EFFECT (object);

  GST_OBJECT_LOCK (self);

  switch (prop_id) {
    case PROP_ENABLE:{
      gboolean enabled = g_value_get_boolean (value);

      if (enabled != self->enabled) {
        if (enabled)
          gst_qahw_effect_enable (self);
        else
          gst_qahw_effect_disable (self);
      }

      self->enabled = enabled;
      break;
    }
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }

  GST_OBJECT_UNLOCK (self);
}

static void
gst_qahw_effect_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstQahwEffect *self;

  self = GST_QAHW_EFFECT (object);

  GST_OBJECT_LOCK (self);

  switch (prop_id) {
    case PROP_ENABLE:
      g_value_set_boolean (value, self->enabled);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }

  GST_OBJECT_UNLOCK (self);
}

static void
gst_qahw_effect_init (GstQahwEffect * self)
{
  self->effect_handle = NULL;
}

static void
gst_qahw_effect_class_init (GstQahwEffectClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
  GstBaseTransformClass *gstbtrans_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;
  gstbtrans_class = (GstBaseTransformClass *) klass;

  gstbtrans_class->start = gst_qahw_effect_start;
  gstbtrans_class->stop = gst_qahw_effect_stop;
  gstbtrans_class->src_event = gst_qahw_effect_src_event;

  gobject_class->get_property = gst_qahw_effect_get_property;
  gobject_class->set_property = gst_qahw_effect_set_property;

  gst_element_class_set_static_metadata (gstelement_class,
      "Audio Equalizer Effect (QAHW)", "Filter/Effect/Audio",
      "Apply equalizer effect using QAHW",
      "Nicolas Dufresne <nicolas.dufresne@collabora.com>");

  gst_element_class_add_static_pad_template (gstelement_class,
      &qahw_equalizer_sink_factory);
  gst_element_class_add_static_pad_template (gstelement_class,
      &qahw_equalizer_src_factory);

  g_object_class_install_property (gobject_class, PROP_ENABLE,
      g_param_spec_boolean ("enable", "Enable", "Enable this effect", TRUE,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_CONSTRUCT));
}

gboolean
gst_qahw_effect_get_param (GstQahwEffect * self, gint param_id, gsize psize,
    gpointer param, gsize vsize, gpointer value)
{
  guint8 buf[GST_ROUND_UP_4 (sizeof (qahw_effect_param_t) + 128)] = { 0 };
  guint32 reply_size = sizeof (buf);
  qahw_effect_param_t *p = (qahw_effect_param_t *) buf;
  gint32 *pid = (gint32 *) p->data;
  gint rc;

  GST_DEBUG_OBJECT (self, "Getting param: param_id=%i, psize=%" G_GSIZE_FORMAT
      " param=%p vsize=%" G_GSIZE_FORMAT " value=%p", param_id, psize, param,
      vsize, value);

  printf("Getting param: param_id=%i, psize=%" G_GSIZE_FORMAT
      " param=%p vsize=%" G_GSIZE_FORMAT " value=%p \n", param_id, psize, param,
      vsize, value);

  g_assert (psize + vsize + sizeof (gint32) <= 128);

  if (!self->effect_handle)
    return FALSE;

  p->vsize = vsize;
  p->psize = psize + sizeof (gint32);
  *pid = param_id;
  memcpy (p->data + sizeof (gint32), param, psize);

  GST_DEBUG_OBJECT (self, "qahw_effect_param_t: size=%" G_GSIZE_FORMAT
      " vsize=%u psize=%u param=%p", reply_size, p->vsize, p->psize, p);
  printf( "qahw_effect_param_t: size=%" G_GSIZE_FORMAT
      " vsize=%u psize=%u param=%p \n", reply_size, p->vsize, p->psize, p);

  rc = qahw_effect_command (self->effect_handle, QAHW_EFFECT_CMD_GET_PARAM,
      sizeof (buf), p, &reply_size, p);

  if (rc == 0)
    rc = p->status;

  if (rc != 0) {
    GST_WARNING_OBJECT (self, "Failed to get effect parameter %i: %s (%i)",
        param_id, g_strerror (-rc), rc);
    printf("Failed to get effect parameter %i: %s (%i)\n",
        param_id, g_strerror (-rc), rc);
    return FALSE;
  }

  memcpy (value, buf + GST_ROUND_UP_N (p->psize, sizeof (gint)), p->vsize);

  return TRUE;
}

static void
gst_qahw_effect_set_param_async (GstElement * elem, gpointer data)
{
  GstQahwEffect *self = GST_QAHW_EFFECT (elem);
  qahw_effect_param_t *p = data;
  gint32 *pid = (gint32 *) p->data;
  gsize size;
  guint32 reply_size = sizeof (gint32);
  gint32 rc, reply;

  size = GST_ROUND_UP_4 (sizeof (qahw_effect_param_t) + p->psize + p->vsize);

  GST_DEBUG_OBJECT (self, "qahw_effect_param_t: size=%" G_GSIZE_FORMAT
      " vsize=%u psize=%u param=%p", size, p->vsize, p->psize, p);
  printf("qahw_effect_param_t: size=%" G_GSIZE_FORMAT
      " vsize=%u psize=%u param=%p \n", size, p->vsize, p->psize, p);
  rc = qahw_effect_command (self->effect_handle, QAHW_EFFECT_CMD_SET_PARAM,
      size, p, &reply_size, &reply);

  /* reply is always 0, ignoring */
  if (rc == 0)
    rc = p->status;

  if (rc != 0) {
    GST_WARNING_OBJECT (self, "Failed to set effect parameter %i: %s (%i)",
        *pid, g_strerror (-rc), rc);
  }
}

gboolean
gst_qahw_effect_set_param (GstQahwEffect * self, gint param_id, gsize psize,
    gpointer param, gsize vsize, gpointer value)
{
  gsize size;
  qahw_effect_param_t *p;
  gint32 *pid;

  GST_DEBUG_OBJECT (self, "Setting param: param_id=%i, psize=%" G_GSIZE_FORMAT
      " param=%p vsize=%" G_GSIZE_FORMAT " value=%p", param_id, psize, param,
      vsize, value);
  printf("Setting param: param_id=%i, psize=%" G_GSIZE_FORMAT
      " param=%p vsize=%" G_GSIZE_FORMAT " value=%p\n", param_id, psize, param,
      vsize, value);

  if (!self->effect_handle)
    return TRUE;

  size = GST_ROUND_UP_4 (sizeof (qahw_effect_param_t) + psize +
      sizeof (gint32) + vsize);
  p = (qahw_effect_param_t *) g_malloc0 (size);
  pid = (gint32 *) p->data;

  p->vsize = vsize;
  p->psize = psize + sizeof (gint32);
  *pid = param_id;
  memcpy (p->data + sizeof (gint32), param, psize);
  memcpy (p->data + GST_ROUND_UP_N (p->psize, sizeof (gint)), value, vsize);

  gst_element_call_async (GST_ELEMENT (self), gst_qahw_effect_set_param_async,
      p, g_free);

  return TRUE;
}
