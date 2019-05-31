/*
*Copyright (c) 2017 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * SECTION:element-qahwaudiosphere
 * @see_also: qahwaudiosphere
 *
 * This filters enables DSP audiosphere effect.
 *
 * <refsect2>
 * <title>Example pipelines</title>
 * |[
 * gst-launch-1.0 -v filesrc location=/path/to/audio.flac ! flacparse ! qahwaudiosphere ! qahwsink
 * ]| Play a FLAC file applying audiosphere effect to the stream.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <string.h>

#include "gstqahwaudiosphere.h"
#include "gstqahw-common.h"
#include <stdio.h>
GST_DEBUG_CATEGORY_EXTERN (qahw_debug);
#define GST_CAT_DEFAULT qahw_debug

enum
{
  PROP_0,
  PROP_STRENGTH,
  PROP_LAST
};

struct _GstQahwAudiosphere
{
  GstQahwEffect parent;
  gint strength;
};

#define gst_qahw_audiosphere_parent_class parent_class
G_DEFINE_TYPE (GstQahwAudiosphere, gst_qahw_audiosphere, GST_TYPE_QAHW_EFFECT);

static gboolean
gst_qahw_audiosphere_set_strength (GstQahwAudiosphere * self, gint16 strength)
{
  GstQahwEffect *effect = GST_QAHW_EFFECT (self);

  GST_DEBUG_OBJECT (self, "Setting strength to %i", strength);
  printf("Setting strength to %i\n", strength);
  if (gst_qahw_effect_set_param (effect, ASPHERE_PARAM_STRENGTH, 0, NULL,
          sizeof (gint16), &strength)) {
    if (self->strength != strength) {
      g_object_notify (G_OBJECT (self), "strength");
      self->strength = strength;
    }
  }

  return TRUE;
}

static gboolean
gst_qahw_audiosphere_setup (GstQahwEffect * effect)
{
  GstQahwAudiosphere *self = GST_QAHW_AUDIOSPHERE (effect);
  gst_qahw_audiosphere_set_strength (self, self->strength);
  return TRUE;
}

static gboolean
gst_qahw_audiosphere_enable (GstQahwEffect * effect)
{
  gboolean enable = TRUE;
  gsize size;
  qahw_effect_param_t *p;
  gint32 *pid;
  guint32 reply_size = sizeof (gint32);
  gint32 reply;

  GST_DEBUG_OBJECT (effect, "Setting Audio Sphere Enable Param");
  printf("Setting Audio Sphere Enable Param\n");

  if (!effect->effect_handle)
    return TRUE;

  size = GST_ROUND_UP_4 (sizeof (qahw_effect_param_t) +
                         sizeof (gint32) + sizeof (gint32));
  p = (qahw_effect_param_t *) g_malloc0 (size);
  pid = (gint32 *) p->data;

  p->vsize = sizeof (gint32);
  p->psize = sizeof (gint32);
  *pid = ASPHERE_PARAM_ENABLE;
  memcpy (p->data + GST_ROUND_UP_N (p->psize, sizeof (gint)), &enable, sizeof (gint32));

  return qahw_effect_command (effect->effect_handle, QAHW_EFFECT_CMD_SET_PARAM,
      size, p, &reply_size, &reply);
}

static gboolean
gst_qahw_audiosphere_disable (GstQahwEffect * effect)
{
  gboolean enable = FALSE;
  gsize size;
  qahw_effect_param_t *p;
  gint32 *pid;
  guint32 reply_size = sizeof (gint32);
  gint32 reply;

  GST_DEBUG_OBJECT (effect, "Setting Audio Sphere Disable Param");
  printf("Setting Audio Sphere Disable Param\n");

  if (!effect->effect_handle)
    return TRUE;

  size = GST_ROUND_UP_4 (sizeof (qahw_effect_param_t) +
                         sizeof (gint32) + sizeof (gint32));
  p = (qahw_effect_param_t *) g_malloc0 (size);
  pid = (gint32 *) p->data;

  p->vsize = sizeof (gint32);
  p->psize = sizeof (gint32);
  *pid = ASPHERE_PARAM_ENABLE;
  memcpy (p->data + GST_ROUND_UP_N (p->psize, sizeof (gint)), &enable, sizeof (gint32));

  return qahw_effect_command (effect->effect_handle, QAHW_EFFECT_CMD_SET_PARAM,
      size, p, &reply_size, &reply);
}

static void
gst_qahw_audiosphere_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstQahwAudiosphere *self = GST_QAHW_AUDIOSPHERE (object);

  GST_OBJECT_LOCK (self);

  switch (prop_id) {
    case PROP_STRENGTH:
      gst_qahw_audiosphere_set_strength (self, g_value_get_int (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }

  GST_OBJECT_UNLOCK (self);
}

static void
gst_qahw_audiosphere_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstQahwAudiosphere *self = GST_QAHW_AUDIOSPHERE (object);

  GST_OBJECT_LOCK (self);

  switch (prop_id) {
    case PROP_STRENGTH:
      g_value_set_int (value, self->strength);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }

  GST_OBJECT_UNLOCK (self);
}

static void
gst_qahw_audiosphere_init (GstQahwAudiosphere * self)
{
  /* nothing to do */
}

static void
gst_qahw_audiosphere_class_init (GstQahwAudiosphereClass * klass)
{
  GstElementClass *gstelement_class;
  GObjectClass *gobject_class;
  GstQahwEffectClass *effect_class;

  gstelement_class = GST_ELEMENT_CLASS (klass);
  effect_class = GST_QAHW_EFFECT_CLASS (klass);
  gobject_class = G_OBJECT_CLASS (klass);

  effect_class->library = QAHW_EFFECT_AUDIOSPHERE_LIBRARY;
  effect_class->uuid = SL_IID_AUDIOSPHERE_UUID;
  effect_class->setup = gst_qahw_audiosphere_setup;
  effect_class->enable = gst_qahw_audiosphere_enable;
  effect_class->disable = gst_qahw_audiosphere_disable;

  gobject_class->get_property = gst_qahw_audiosphere_get_property;
  gobject_class->set_property = gst_qahw_audiosphere_set_property;

  gst_element_class_set_static_metadata (gstelement_class,
      "Audio Sphere Effect (QAHW)", "Filter/Effect/Audio",
      "Apply audio sphere effect using QAHW",
      "Nicolas Dufresne <nicolas.dufresne@collabora.com>");

  g_object_class_install_property (gobject_class, PROP_STRENGTH,
      g_param_spec_int ("strength", "Strength",
          "The strength of this effect.", 0, 1000, 100,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_CONSTRUCT));
}
